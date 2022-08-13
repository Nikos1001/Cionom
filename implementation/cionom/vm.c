// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+cionom@pm.me>

#include "include/cionom.h"

gen_error_t cio_vm_dump_stack(const cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(cio_vm_dump_stack);

	GEN_NULL_CHECK(vm);

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

	GEN_NULL_CHECK(vm);

	if(vm->frames_used >= vm->frames_length) GEN_ERROR_OUT(GEN_OUT_OF_SPACE, "No unused frames available to push");
	if(vm->frames_used) vm->frames[vm->frames_used].base = vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height;
	++vm->frames_used;

	GEN_ALL_OK;
}

gen_error_t cio_vm_pop_frame(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(cio_vm_pop_frame);

	GEN_NULL_CHECK(vm);

	if(!vm->frames_used) GEN_ERROR_OUT(GEN_BAD_OPERATION, "Attemping to pop when no stack frames are active");
	--vm->frames_used;

	GEN_ALL_OK;
}

gen_error_t cio_vm_push(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(cio_vm_push);

	GEN_NULL_CHECK(vm);
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

	GEN_NULL_CHECK(vm);

	gen_error_t error = GEN_OK;

	cio_frame_t* const frame = &vm->frames[vm->frames_used - 1];
	unsigned char instruction = vm->bytecode[frame->execution_offset];
	size_t argc = 0;
	while(instruction != 0b11111111) {
		if(instruction & 0b10000000) {
			// Callee takes ownership of lower stack items
			// Subtract 1 for reserve space
			frame->height -= argc - 1;
			error = cio_vm_dispatch_call(vm, instruction & 0b01111111, argc - 1);
			GEN_ERROR_OUT_IF(error, "`cio_vm_dispatch_call` failed");
			argc = 0;
		}
		else {
			error = cio_vm_push(vm);
			GEN_ERROR_OUT_IF(error, "`cio_vm_push` failed");
			vm->stack[frame->base + frame->height - 1] = instruction & 0b01111111;
			++argc;
		}
		instruction = vm->bytecode[++frame->execution_offset];
	}

	GEN_ALL_OK;
}

gen_error_t cio_vm_dispatch_call(cio_vm_t* const restrict vm, const size_t callable, const size_t argc) {
	GEN_FRAME_BEGIN(cio_vm_dispatch_call);

	GEN_NULL_CHECK(vm);

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

	GEN_NULL_CHECK(bytecode);
	GEN_NULL_CHECK(bytecode_length);
	GEN_NULL_CHECK(out_instance);

	out_instance->stack_length = stack_length;
	gen_error_t error = gzalloc((void**) &out_instance->stack, out_instance->stack_length, sizeof(size_t));
	GEN_ERROR_OUT_IF(error, "`gzalloc` failed");
	out_instance->frames_length = stack_length;
	error = gzalloc((void**) &out_instance->frames, out_instance->frames_length, sizeof(cio_frame_t));
	GEN_ERROR_OUT_IF(error, "`gzalloc` failed");
	error = gen_dylib_load(&out_instance->external_lib, "cionom-external");
	GEN_ERROR_OUT_IF(error, "`gen_dylib_load` failed");

	out_instance->callables_length = bytecode[0];
	error = gzalloc((void**) &out_instance->callables, out_instance->callables_length, sizeof(cio_routine_function_t));
	GEN_ERROR_OUT_IF(error, "`gzalloc` failed");
	error = gzalloc((void**) &out_instance->callables_offsets, out_instance->callables_length, sizeof(size_t));
	GEN_ERROR_OUT_IF(error, "`gzalloc` failed");

	size_t offset = 1;
	for(size_t i = 0; i < out_instance->callables_length; ++i) {
		out_instance->callables[i] = cio_internal_vm_execute_routine;
		out_instance->callables_offsets[i] = *(uint32_t*) &bytecode[offset];
		offset += 4;

		if(out_instance->callables_offsets[i] == UINT32_MAX) {
			error = cio_resolve_external((char*) &bytecode[offset], &out_instance->callables[i], out_instance->external_lib);
			GEN_ERROR_OUT_IF(error, "`cio_resolve_external` failed");

			size_t stride = 0;
			error = gen_string_length((char*) &bytecode[offset], GEN_STRING_NO_BOUND, GEN_STRING_NO_BOUND, &stride);
			GEN_ERROR_OUT_IF(error, "`gen_string_length` failed");

			offset += stride + 1;
		}
	}

	out_instance->bytecode_length = bytecode_length - offset;
	out_instance->bytecode = bytecode + offset;

	GEN_ALL_OK;
}

gen_error_t cio_free_vm(const cio_vm_t* const restrict instance) {
	GEN_FRAME_BEGIN(cio_free_vm);

	GEN_NULL_CHECK(instance);

	gen_error_t error = gfree(instance->stack);
	GEN_ERROR_OUT_IF(error, "`gfree` failed");

	error = gfree(instance->frames);
	GEN_ERROR_OUT_IF(error, "`gfree` failed");

	error = gen_dylib_unload(instance->external_lib);
	GEN_ERROR_OUT_IF(error, "`gen_dylib_unload` failed");

	error = gfree(instance->callables);
	GEN_ERROR_OUT_IF(error, "`gfree` failed");

	error = gfree(instance->callables_offsets);
	GEN_ERROR_OUT_IF(error, "`gfree` failed");

	GEN_ALL_OK;
}

gen_error_t cio_vm_get_frame(const cio_vm_t* const restrict vm, const size_t frame_offset, const cio_frame_t** const restrict out_pointer) {
	GEN_FRAME_BEGIN(cio_vm_get_frame);

	GEN_NULL_CHECK(vm);
	GEN_NULL_CHECK(out_pointer);

	if(frame_offset >= vm->frames_length) GEN_ERROR_OUT(GEN_OUT_OF_BOUNDS, "`frame_offset` was greater than or equal to `vm->frames_length`");

	*out_pointer = &vm->frames[vm->frames_used - (frame_offset + 1)];

	GEN_ALL_OK;
}

gen_error_t cio_vm_get_frame_pointer(const cio_vm_t* const restrict vm, const cio_frame_t* const restrict frame, size_t** const restrict out_pointer) {
	GEN_FRAME_BEGIN(cio_vm_get_frame_pointer);

	GEN_NULL_CHECK(vm);
	GEN_NULL_CHECK(frame);
	GEN_NULL_CHECK(out_pointer);

	*out_pointer = &vm->stack[frame->base];

	GEN_ALL_OK;
}
