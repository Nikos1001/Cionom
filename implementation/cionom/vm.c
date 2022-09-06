// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

#include "include/cionom.h"

#include <genmemory.h>
#include <genstring.h>

// TODO: Reimplement me
// gen_error_t* cio_vm_dump_stack(const cio_vm_t* const restrict vm) {
// 	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_vm_dump_stack, GEN_FILE_NAME);
// 	if(error) return error;

// 	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `NULL`");

// 	glog(DEBUG, "Stack");
// 	GEN_FOREACH_PTR(i, frame, vm->frames_used, vm->frames) {
// 		glogf(DEBUG, "├ Frame %zu", i);
// 		GEN_FOREACH_PTR(j, stack_entry, frame->height, vm->stack + frame->base) {
// 			glogf(DEBUG, "| ├ 0x%zx: %zu", frame->base + j, *stack_entry);
// 		}
// 	}

// 	glog(DEBUG, "├ Unused");
// 	GEN_FOREACH_PTR(i, stack_entry, vm->stack_length - (vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height), vm->stack + vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height) {
// 		glogf(DEBUG, "├ 0x%zx: %zu", vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height + i, *stack_entry);
// 	}

// 	return NULL;
// }

gen_error_t* cio_vm_push_frame(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_vm_push_frame, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `NULL`");

	if(vm->frames_used >= vm->frames_length) return gen_error_attach_backtrace(GEN_ERROR_OUT_OF_SPACE, GEN_LINE_NUMBER, "No unused frames available to push");
	if(vm->frames_used) vm->frames[vm->frames_used].base = vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height;
	++vm->frames_used;

	return NULL;
}

gen_error_t* cio_vm_pop_frame(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_vm_pop_frame, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `NULL`");

	if(!vm->frames_used) return gen_error_attach_backtrace(GEN_ERROR_BAD_OPERATION, GEN_LINE_NUMBER, "Attemping to pop when no stack frames are active");
	--vm->frames_used;

	return NULL;
}

gen_error_t* cio_vm_push(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_vm_push, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `NULL`");
	if(!vm->frames_used) return gen_error_attach_backtrace(GEN_ERROR_NO_SUCH_OBJECT, GEN_LINE_NUMBER, "No frame available to push into");

	if(vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height < vm->stack_length)
		++vm->frames[vm->frames_used - 1].height;
	else
		return gen_error_attach_backtrace(GEN_ERROR_BAD_OPERATION, GEN_LINE_NUMBER, "Stack overflow");
	vm->stack[vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height - 1] = 0;

	return NULL;
}

static gen_error_t* cio_internal_vm_execute_routine(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_internal_vm_execute_routine, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `NULL`");

	cio_frame_t* const frame = &vm->frames[vm->frames_used - 1];
	unsigned char instruction = vm->bytecode[frame->execution_offset];
	size_t argc = 0;
	while(instruction != 0b11111111) {
		if(instruction & 0b10000000) {
			// glogf(DEBUG, "call %d", instruction & 0b01111111);

			// Callee takes ownership of lower stack items
			// Subtract 1 for reserve space
			frame->height -= argc - 1;
			error = cio_vm_dispatch_call(vm, instruction & 0b01111111, argc - 1);
			if(error) return error;
			argc = 0;
		}
		else {
			// glogf(DEBUG, "push %d", instruction & 0b01111111);

			error = cio_vm_push(vm);
			if(error) return error;
			vm->stack[frame->base + frame->height - 1] = instruction & 0b01111111;
			++argc;
		}
		instruction = vm->bytecode[++frame->execution_offset];
	}
	// glog(DEBUG, "ret");

	return NULL;
}

gen_error_t* cio_vm_dispatch_call(cio_vm_t* const restrict vm, const size_t callable, const size_t argc) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_vm_dispatch_call, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `NULL`");

	if(callable >= vm->callables_length) return gen_error_attach_backtrace(GEN_ERROR_OUT_OF_BOUNDS, GEN_LINE_NUMBER, "`callable` was greater than `vm->callables_length`");

	error = cio_vm_push_frame(vm);
    if(error) return error;

	// Dispatch call
	vm->frames[vm->frames_used - 1].height = argc;
	vm->frames[vm->frames_used - 1].execution_offset = vm->callables_offsets[callable];
	error = vm->callables[callable](vm);
    if(error) return error;

	error = cio_vm_pop_frame(vm);
    if(error) return error;

	return NULL;
}

gen_error_t* cio_vm_initialize_bytecode(const unsigned char* const restrict bytecode, const size_t bytecode_length, const size_t stack_length, cio_vm_t* const restrict out_instance) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_vm_initialize_bytecode, GEN_FILE_NAME);
	if(error) return error;

	if(!bytecode) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`bytecode` was `NULL`");
	if(!bytecode_length) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`bytecode_length` was `NULL`");
	if(!out_instance) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`out_instance` was `NULL`");

	out_instance->stack_length = stack_length;
	error = gen_memory_allocate_zeroed((void**) &out_instance->stack, out_instance->stack_length, sizeof(size_t));
    if(error) return error;
	out_instance->frames_length = stack_length;
	error = gen_memory_allocate_zeroed((void**) &out_instance->frames, out_instance->frames_length, sizeof(cio_frame_t));
    if(error) return error;
    static const char external_lib_name[] = "cionom-external";
	error = gen_dynamic_library_handle_open(external_lib_name, sizeof(external_lib_name) - 1, &out_instance->external_lib);
    if(error) return error;

	out_instance->callables_length = bytecode[0];
	if(out_instance->callables_length) {
		error = gen_memory_allocate_zeroed((void**) &out_instance->callables, out_instance->callables_length, sizeof(cio_routine_function_t));
        if(error) return error;
		error = gen_memory_allocate_zeroed((void**) &out_instance->callables_offsets, out_instance->callables_length, sizeof(size_t));
        if(error) return error;
	}

	size_t offset = 1;
	for(size_t i = 0; i < out_instance->callables_length; ++i) {
		out_instance->callables[i] = cio_internal_vm_execute_routine;
		out_instance->callables_offsets[i] = *(const uint32_t*) &bytecode[offset];
		offset += 4;

		if(out_instance->callables_offsets[i] == UINT32_MAX) {
			error = cio_resolve_external((const char*) &bytecode[offset], &out_instance->callables[i], &out_instance->external_lib);
            if(error) return error;

			size_t stride = 0;
			error = gen_string_length((const char*) &bytecode[offset], GEN_STRING_NO_BOUNDS, GEN_STRING_NO_BOUNDS, &stride);
            if(error) return error;

			offset += stride + 1;
		}
	}

	out_instance->bytecode_length = bytecode_length - offset;
	out_instance->bytecode = bytecode + offset;

	return NULL;
}

gen_error_t* cio_free_vm(cio_vm_t* const restrict instance) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_free_vm, GEN_FILE_NAME);
	if(error) return error;

	if(!instance) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`instance` was `NULL`");

	error = gen_memory_free((void**) &instance->stack);
    if(error) return error;

	error = gen_memory_free((void**) &instance->frames);
    if(error) return error;

	error = gen_dynamic_library_handle_close(instance->external_lib);
    if(error) return error;

	error = gen_memory_free((void**) &instance->callables);
    if(error) return error;

	error = gen_memory_free((void**) &instance->callables_offsets);
    if(error) return error;

	return NULL;
}

gen_error_t* cio_vm_get_frame(const cio_vm_t* const restrict vm, const size_t frame_offset, const cio_frame_t** const restrict out_pointer) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_vm_get_frame, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `NULL`");
	if(!out_pointer) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`out_pointer` was `NULL`");

	if(frame_offset >= vm->frames_length) return gen_error_attach_backtrace(GEN_ERROR_OUT_OF_BOUNDS, GEN_LINE_NUMBER, "`frame_offset` was greater than or equal to `vm->frames_length`");

	*out_pointer = &vm->frames[vm->frames_used - (frame_offset + 1)];

	return NULL;
}

gen_error_t* cio_vm_get_frame_pointer(const cio_vm_t* const restrict vm, const cio_frame_t* const restrict frame, size_t** const restrict out_pointer) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_vm_get_frame_pointer, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `NULL`");
	if(!frame) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`frame` was `NULL`");
	if(!out_pointer) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`out_pointer` was `NULL`");

	*out_pointer = &vm->stack[frame->base];

	return NULL;
}
