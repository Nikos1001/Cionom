// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

#include "include/cionom.h"

#include <genmemory.h>
#include <genstring.h>

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

#include <genlog.h>

gen_error_t* cio_internal_vm_execute_routine(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_internal_vm_execute_routine, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `NULL`");


	cio_frame_t* const frame = &vm->frames[vm->frames_used - 1];
	unsigned char instruction = vm->bytecode[vm->current_bytecode].bytecode[frame->execution_offset];

    gen_log_formatted(GEN_LOG_LEVEL_DEBUG, "cionom", "Commencing execution in BC %uz @ %uz w/ frame %uz", vm->current_bytecode, frame->execution_offset, vm->frames_used - 1);

	size_t argc = 0;
	while(instruction != 0b11111111) {
        gen_log_formatted(GEN_LOG_LEVEL_DEBUG, "cionom", "Decoding %uc in BC %uz @ %uz", instruction, vm->current_bytecode, frame->execution_offset);
		if(instruction & 0b10000000) {
			gen_log_formatted(GEN_LOG_LEVEL_DEBUG, "cionom", "call %ui", instruction & 0b01111111);

			// Callee takes ownership of lower stack items
			// Subtract 1 for reserve space
			frame->height -= argc - 1;
			error = cio_vm_dispatch_call(vm, instruction & 0b01111111, argc - 1);
			if(error) return error;
            gen_log(GEN_LOG_LEVEL_DEBUG, "cionom", "Call returned successfully");
			argc = 0;
		}
		else {
			gen_log_formatted(GEN_LOG_LEVEL_DEBUG, "cionom", "push %ui", instruction & 0b01111111);

			error = cio_vm_push(vm);
			if(error) return error;
			vm->stack[frame->base + frame->height - 1] = instruction & 0b01111111;
			++argc;
		}
		instruction = vm->bytecode[vm->current_bytecode].bytecode[++frame->execution_offset];
	}
	gen_log(GEN_LOG_LEVEL_DEBUG, "cionom", "ret");

	return NULL;
}

gen_error_t* cio_vm_dispatch_call(cio_vm_t* const restrict vm, const size_t callable, const size_t argc) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_vm_dispatch_call, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `NULL`");
	if(callable >= vm->bytecode[vm->current_bytecode].callables_length) return gen_error_attach_backtrace(GEN_ERROR_OUT_OF_BOUNDS, GEN_LINE_NUMBER, "`callable` was greater than `vm->callables_length`");

	error = cio_vm_push_frame(vm);
    if(error) return error;

	// Construct call
    size_t callable_remote_index = vm->bytecode[vm->current_bytecode].callables_indices[callable];
	vm->frames[vm->frames_used - 1].height = argc;
	vm->frames[vm->frames_used - 1].execution_offset = vm->bytecode[vm->current_bytecode].callables_offsets[callable];
    size_t old_bytecode = vm->current_bytecode;
    vm->current_bytecode = vm->bytecode[vm->current_bytecode].callables_bytecode_indices[callable];

    gen_log_formatted(GEN_LOG_LEVEL_DEBUG, "cionom", "Calling %t routine %uz (@%p) in BC %uz @ %uz", vm->bytecode[vm->current_bytecode].callables[callable] == cio_internal_vm_execute_routine ? "cionom" : "external", callable, (void*) vm->bytecode[vm->current_bytecode].callables[callable], vm->current_bytecode, vm->frames[vm->frames_used - 1].execution_offset);

	// Dispatch call
	error = vm->bytecode[vm->current_bytecode].callables[callable_remote_index](vm);
    if(error) return error;

    vm->current_bytecode = old_bytecode;

	error = cio_vm_pop_frame(vm);
    if(error) return error;

	return NULL;
}

gen_error_t* cio_vm_bundle_initialize(const unsigned char* const restrict bytecode, const size_t bytecode_length, const size_t stack_length, cio_vm_t* const restrict out_instance) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_vm_bundle_initialize, GEN_FILE_NAME);
	if(error) return error;

	if(!bytecode) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`bytecode` was `NULL`");
	if(!bytecode_length) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`bytecode_length` was 0");
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

    size_t bytecode_count = 0;
    const char*** routine_identifiers = NULL;

    for(size_t i = 0; i < bytecode_length; ++i) {
        error = gen_memory_reallocate_zeroed((void**) &out_instance->bytecode, bytecode_count, bytecode_count + 1, sizeof(cio_bytecode_t));
        if(error) return error;

        error = gen_memory_reallocate_zeroed((void**) &routine_identifiers, bytecode_count, bytecode_count + 1, sizeof(char**));
        if(error) return error;

        out_instance->bytecode[bytecode_count].callables_length = bytecode[0];

        if(out_instance->bytecode[bytecode_count].callables_length) {
            error = gen_memory_allocate_zeroed((void**) &out_instance->bytecode[bytecode_count].callables, out_instance->bytecode[bytecode_count].callables_length, sizeof(cio_routine_function_t));
            if(error) return error;
            error = gen_memory_allocate_zeroed((void**) &out_instance->bytecode[bytecode_count].callables_offsets, out_instance->bytecode[bytecode_count].callables_length, sizeof(size_t));
            if(error) return error;
            error = gen_memory_allocate_zeroed((void**) &out_instance->bytecode[bytecode_count].callables_bytecode_indices, out_instance->bytecode[bytecode_count].callables_length, sizeof(size_t));
            if(error) return error;
            error = gen_memory_allocate_zeroed((void**) &out_instance->bytecode[bytecode_count].callables_indices, out_instance->bytecode[bytecode_count].callables_length, sizeof(size_t));
            if(error) return error;
        }

        error = gen_memory_allocate_zeroed((void**) &routine_identifiers[bytecode_count], out_instance->bytecode[bytecode_count].callables_length, sizeof(char*));
        if(error) return error;

        size_t offset = 1;
        for(size_t j = 0; j < out_instance->bytecode[bytecode_count].callables_length; ++j) {
            out_instance->bytecode[bytecode_count].callables[j] = cio_internal_vm_execute_routine;
            out_instance->bytecode[bytecode_count].callables_offsets[j] = *(const uint32_t*) &bytecode[i + offset];
            out_instance->bytecode[bytecode_count].callables_bytecode_indices[j] = bytecode_count;
            out_instance->bytecode[bytecode_count].callables_indices[j] = j;
            offset += 4;

            routine_identifiers[bytecode_count][j] = (const char*) &bytecode[i + offset];

            size_t stride = 0;
            error = gen_string_length((const char*) &bytecode[i + offset], GEN_STRING_NO_BOUNDS, GEN_STRING_NO_BOUNDS, &stride);
            if(error) return error;

            offset += stride + 1;
        }

        out_instance->bytecode[bytecode_count].bytecode = bytecode + offset;

        for(i += offset + out_instance->bytecode[bytecode_count].callables_offsets[out_instance->bytecode[bytecode_count].callables_length - 1]; bytecode[i] != 0xFF; ++i);

        out_instance->bytecode[bytecode_count].size = i - offset;

        ++bytecode_count;
    }

    for(size_t i = 0; i < bytecode_count; ++i) {
        for(size_t j = 0; j < out_instance->bytecode[i].callables_length; ++j) {
            if(out_instance->bytecode[i].callables_offsets[j] == 0xFFFFFFFF) {
                for(size_t k = 0; k < bytecode_count; ++k) {
                    for(size_t l = 0; l < out_instance->bytecode[k].callables_length; ++l) {
                        if(out_instance->bytecode[k].callables_offsets[l] == 0xFFFFFFFF) continue;

                        bool equal = false;
                        error = gen_string_compare(routine_identifiers[i][j], GEN_STRING_NO_BOUNDS, routine_identifiers[k][l], GEN_STRING_NO_BOUNDS, GEN_STRING_NO_BOUNDS, &equal);
                        if(error) return error;

                        if(equal) {
                            gen_log_formatted(GEN_LOG_LEVEL_DEBUG, "cionom", "Resolving %t in BC %uz @ %uz (%uz)", routine_identifiers[i][j], k, out_instance->bytecode[k].callables_offsets[l], l);

                            out_instance->bytecode[i].callables_offsets[j] = out_instance->bytecode[k].callables_offsets[l];
                            out_instance->bytecode[i].callables_bytecode_indices[j] = k;
                            out_instance->bytecode[i].callables_indices[j] = l;
                            goto break2;
                        }
                    }
                }

                gen_log_formatted(GEN_LOG_LEVEL_DEBUG, "cionom", "Resolving %t in external code", routine_identifiers[i][j]);
                error = cio_resolve_external(routine_identifiers[i][j], &out_instance->bytecode[i].callables[j], &out_instance->external_lib);
                if(error) return error;

                break2: continue;
            }
        }
    }

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

    for(size_t i = 0; i < instance->bytecode_length; ++i) {
        error = gen_memory_free((void**) &instance->bytecode[i].callables);
        if(error) return error;

        error = gen_memory_free((void**) &instance->bytecode[i].callables_offsets);
        if(error) return error;

        error = gen_memory_free((void**) &instance->bytecode[i].callables_bytecode_indices);
        if(error) return error;

        error = gen_memory_free((void**) &instance->bytecode[i].callables_indices);
        if(error) return error;
    }

	error = gen_memory_free((void**) &instance->bytecode);
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
