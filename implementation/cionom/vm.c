// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+genstone@pm.me>

#include "include/cionom.h"

gen_error_t cio_vm_dump_stack(const cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(cio_vm_dump_stack);

	GEN_INTERNAL_BASIC_PARAM_CHECK(vm);

	glog(DEBUG, "Stack");
	GEN_FOREACH_PTR(i, frame, vm->frames_used, vm->frames) {
		glogf(DEBUG, "├ Frame %zu", i);
		GEN_FOREACH_PTR(j, stack_entry, frame->height, vm->stack + frame->base) {
			glogf(DEBUG, "| ├ 0x%zx: %zu", frame->base + j, *stack_entry);
		}
	}

	glog(DEBUG, "├ Unused");
	GEN_FOREACH_PTR(i, stack_entry, vm->stack_length - (vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height), vm->stack + vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height) {
		glogf(DEBUG, "├ 0x%zx: %zu", vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height + i, *stack_entry);
	}

	GEN_ALL_OK;
}

gen_error_t cio_vm_push_frame(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(cio_vm_push_frame);

	GEN_INTERNAL_BASIC_PARAM_CHECK(vm);

	if(vm->frames_used >= vm->frames_length) GEN_ERROR_OUT(GEN_OUT_OF_SPACE, "No unused frames available to push");
	if(vm->frames_used) vm->frames[vm->frames_used].base = vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height;
	++vm->frames_used;

	GEN_ALL_OK;
}

gen_error_t cio_vm_pop_frame(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(cio_vm_pop_frame);

	GEN_INTERNAL_BASIC_PARAM_CHECK(vm);

	if(!vm->frames_used) GEN_ERROR_OUT(GEN_BAD_OPERATION, "Attemping to pop when no stack frames are active");
	--vm->frames_used;

	GEN_ALL_OK;
}

gen_error_t cio_vm_push(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(cio_vm_push);

	GEN_INTERNAL_BASIC_PARAM_CHECK(vm);
	if(!vm->frames_used) GEN_ERROR_OUT(GEN_NO_SUCH_OBJECT, "No frame available to push into");

	if(vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height < vm->stack_length)
		++vm->frames[vm->frames_used - 1].height;
	else
		GEN_ERROR_OUT(GEN_BAD_OPERATION, "Stack overflow");
	vm->stack[vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height - 1] = 0;

	GEN_ALL_OK;
}

static __nodiscard gen_error_t cio_internal_vm_execute_routine(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(cio_internal_vm_execute_routine);

	GEN_INTERNAL_BASIC_PARAM_CHECK(vm);

	gen_error_t error = GEN_OK;

	cio_frame_t* const frame = &vm->frames[vm->frames_used - 1];
	const size_t* current = &vm->bytecode[frame->execution_offset];
	size_t argc = 0;
	while(frame->execution_offset < vm->bytecode_length && !(*current == CIO_BYTECODE_OPERATION_CALL && current[1] == SIZE_MAX)) {
		if(*current == CIO_BYTECODE_OPERATION_CALL) {
			// Callee takes ownership of lower stack items
			// Subtract 1 for reserve space
			frame->height -= argc - 1;
			error = cio_vm_dispatch_call(vm, current[1], argc - 1);
			GEN_ERROR_OUT_IF(error, "`cio_vm_dispatch_call` failed");
			argc = 0;
		}
		else if(*current == CIO_BYTECODE_OPERATION_PUSH) {
			error = cio_vm_push(vm);
			GEN_ERROR_OUT_IF(error, "`cio_vm_push` failed");
			vm->stack[frame->base + frame->height - 1] = current[1];
			++argc;
		}
		else {
			glogf(ERROR, "Bad encoding 0x%zx at offset %zu", *current, frame->execution_offset);
			GEN_ERROR_OUT(GEN_BAD_CONTENT, "Unknown instruction encoding encountered");
		}
		frame->execution_offset += 2;
		current = &vm->bytecode[frame->execution_offset];
	}

	GEN_ALL_OK;
}

gen_error_t cio_vm_dispatch_call(cio_vm_t* const restrict vm, const size_t callable, const size_t argc) {
	GEN_FRAME_BEGIN(cio_vm_dispatch_call);

	GEN_INTERNAL_BASIC_PARAM_CHECK(vm);

	if(callable >= vm->callables_length) GEN_ERROR_OUT(GEN_OUT_OF_BOUNDS, "`callable` was greater than `vm->callables_length`");

	gen_error_t error = cio_vm_push_frame(vm);
	GEN_ERROR_OUT_IF(error, "`cio_vm_push_frame` failed");

	// Dispatch call
	vm->frames[vm->frames_used - 1].height = argc;
	vm->frames[vm->frames_used - 1].execution_offset = vm->callables_offsets[callable];
	error = vm->callables[callable](vm);
	GEN_ERROR_OUT_IF(error, "Routine call failed");

	error = cio_vm_pop_frame(vm);
	GEN_ERROR_OUT_IF(error, "`cio_vm_pop_frame` failed");

	GEN_ALL_OK;
}

gen_error_t cio_vm_initialize_bytecode(const unsigned char* const restrict bytecode, const size_t bytecode_length, const size_t stack_length, cio_vm_t* const restrict out_instance) {
	GEN_FRAME_BEGIN(cio_vm_initialize_bytecode);

	GEN_INTERNAL_BASIC_PARAM_CHECK(bytecode);
	GEN_INTERNAL_BASIC_PARAM_CHECK(bytecode_length);
	GEN_INTERNAL_BASIC_PARAM_CHECK(out_instance);

	const size_t* const aligned_bytecode = (size_t*) bytecode;

	out_instance->bytecode_length = bytecode_length;
	out_instance->bytecode = aligned_bytecode;

	out_instance->stack_length = stack_length;
	gen_error_t error = gzalloc((void**) &out_instance->stack, out_instance->stack_length, sizeof(size_t));
	GEN_ERROR_OUT_IF(error, "`gzalloc` failed");
	out_instance->frames_length = stack_length;
	error = gzalloc((void**) &out_instance->frames, out_instance->frames_length, sizeof(cio_frame_t));
	GEN_ERROR_OUT_IF(error, "`gzalloc` failed");
	error = gen_dylib_load(&out_instance->external_lib, "cionom-external");
	GEN_ERROR_OUT_IF(error, "`gen_dylib_load` failed");

	out_instance->callables_length = aligned_bytecode[0];
	error = gzalloc((void**) &out_instance->callables, out_instance->callables_length, sizeof(cio_routine_function_t));
	GEN_ERROR_OUT_IF(error, "`gzalloc` failed");
	error = gzalloc((void**) &out_instance->callables_offsets, out_instance->callables_length, sizeof(size_t));
	GEN_ERROR_OUT_IF(error, "`gzalloc` failed");

	size_t current_offset = 1;
	GEN_FOREACH_PTR(i, callable, out_instance->callables_length, out_instance->callables) {
		out_instance->callables_offsets[i] = aligned_bytecode[current_offset];

		++current_offset;
		if(out_instance->callables_offsets[i] == SIZE_MAX) {
			error = cio_resolve_mangled((char*) &aligned_bytecode[current_offset], callable, out_instance->external_lib);
			GEN_ERROR_OUT_IF(error, "`cio_resolve_mangled` failed");

			size_t stride = 0;
			error = gen_string_length((char*) &aligned_bytecode[current_offset], GEN_STRING_NO_BOUND, GEN_STRING_NO_BOUND, &stride);
			GEN_ERROR_OUT_IF(error, "`gen_string_length` failed");

			current_offset += (size_t) (ceil((double) stride / (double) sizeof(size_t)));
			continue;
		}

		*callable = cio_internal_vm_execute_routine;
	}

	GEN_ALL_OK;
}
