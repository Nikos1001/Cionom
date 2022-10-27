// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

#include "include/cionom.h"

#include <genmemory.h>
#include <genstring.h>
#include <genlog.h>

#ifndef CIO_VM_DEBUG_PRINTS
#define CIO_VM_DEBUG_PRINTS GEN_DISABLED
#endif

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

static gen_error_t* cio_vm_internal_execute_routine(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_vm_internal_execute_routine, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `NULL`");

	cio_frame_t* const frame = &vm->frames[vm->frames_used - 1];
#ifdef __ANALYZER
    cio_instruction_t* instruction = calloc(1, sizeof(cio_instruction_t));
#else
	const cio_instruction_t* instruction = (const cio_instruction_t*) &vm->bytecode[vm->current_bytecode].bytecode[frame->execution_offset];
#endif

#if CIO_VM_DEBUG_PRINTS == GEN_ENABLED
    gen_log_formatted(GEN_LOG_LEVEL_DEBUG, "cionom", "Commencing execution in BC %uz @ %uz w/ frame %uz", vm->current_bytecode, frame->execution_offset, vm->frames_used - 1);
#endif

	size_t argc = 0;
    bool elide_reserve_space = false;
	while(!(instruction->opcode == CIO_CALL && instruction->operand == CIO_OPERAND_MAX)) {
#if CIO_VM_DEBUG_PRINTS == GEN_ENABLED
        gen_log_formatted(GEN_LOG_LEVEL_DEBUG, "cionom", "Decoding %uc (%uc %uc) in BC %uz @ %uz", *(const uint8_t*) instruction, instruction->opcode, instruction->operand, vm->current_bytecode, frame->execution_offset);
#endif
		if(instruction->opcode == CIO_CALL) {
#if CIO_VM_DEBUG_PRINTS == GEN_ENABLED
			gen_log_formatted(GEN_LOG_LEVEL_DEBUG, "cionom", "call %uc (argc = %uz (effective argc = %uz), argc[0] = %uz)", instruction->operand, argc, argc - (1 * !elide_reserve_space), vm->stack[frame->base + frame->height - argc]);
#endif

			// Callee takes ownership of lower stack items
			// Subtract 1 for reserve space
			frame->height -= argc - (1 * !elide_reserve_space);
			error = cio_vm_dispatch_call(vm, instruction->operand, argc - (1 * !elide_reserve_space));
            elide_reserve_space = false;
#ifdef __ANALYZER
			if(error) {
                free(instruction);
                return error;
            }
#else
			if(error) return error;
#endif

#if CIO_VM_DEBUG_PRINTS == GEN_ENABLED
            gen_log(GEN_LOG_LEVEL_DEBUG, "cionom", "Call returned successfully");
#endif
			argc = 0;
		}
		else {
#if CIO_VM_DEBUG_PRINTS == GEN_ENABLED
			gen_log_formatted(GEN_LOG_LEVEL_DEBUG, "cionom", "push %uc", instruction->operand);
#endif

            if(instruction->operand == CIO_OPERAND_MAX && frame->height) {
#if CIO_VM_DEBUG_PRINTS == GEN_ENABLED
    			gen_log_formatted(GEN_LOG_LEVEL_DEBUG, "cionom", "Processing extension ID %uc", vm->stack[frame->base + frame->height - 1]);
#endif
                switch(vm->stack[frame->base + frame->height - 1]) {
                    case CIO_EXTENSION_ID_ELIDE_RESERVE_SPACE: {
                        // vm->bytecode[vm->current_bytecode].extension_settings.elide_reserve_space
                        elide_reserve_space = true;
                        break;
                    }
                    case CIO_EXTENSION_ID_BREAKPOINTS: {
                        // TODO: Implement breakpoints
                        break;
                    }
                    default: {
                        return gen_error_attach_backtrace_formatted(GEN_ERROR_BAD_CONTENT, GEN_LINE_NUMBER, "Extension ID %uc is unrecognized in bytecode %uz @ %uz", vm->stack[frame->base + frame->height - 1], vm->current_bytecode, frame->execution_offset);
                    }
                }
    
                frame->height--; // Remove extension ID
                argc = 0;
#ifdef __ANALYZER
#else
        		instruction = (const cio_instruction_t*) &vm->bytecode[vm->current_bytecode].bytecode[++frame->execution_offset];
#endif
                continue;
           }

            if(instruction->operand == CIO_OPERAND_MAX && vm->warning_settings->consume_reserved_encoding) {
                // TODO: Add source info/disassembly here once debugging "stuff" is implemented

                error = gen_log_formatted(vm->warning_settings->fatal_warnings ? GEN_LOG_LEVEL_FATAL : GEN_LOG_LEVEL_WARNING, "cionom", "Encoding `push 0x7F` is reserved [%tconsume_reserved_encoding]", vm->warning_settings->fatal_warnings ? "fatal_warnings, " : "");
                if(error) return error;

                if(vm->warning_settings->fatal_warnings) {
                    return gen_error_attach_backtrace_formatted(GEN_ERROR_IN_USE, GEN_LINE_NUMBER, "Encoding `push 0x7F` is reserved [%tconsume_reserved_encoding]", vm->warning_settings->fatal_warnings ? "fatal_warnings, " : "");
                }
            }

			error = cio_vm_push(vm);
#ifdef __ANALYZER
			if(error) {
                free(instruction);
                return error;
            }
#else
			if(error) return error;
#endif
			vm->stack[frame->base + frame->height - 1] = instruction->operand;
			++argc;
		}

#ifdef __ANALYZER
#else
		instruction = (const cio_instruction_t*) &vm->bytecode[vm->current_bytecode].bytecode[++frame->execution_offset];
#endif
	}

#ifdef __ANALYZER
    free(instruction);
#endif

#if CIO_VM_DEBUG_PRINTS == GEN_ENABLED
    gen_log_formatted(GEN_LOG_LEVEL_DEBUG, "cionom", "Decoding %uc (%uc %uc) in BC %uz @ %uz", *(uint8_t*) &instruction, instruction->opcode, instruction->operand, vm->current_bytecode, frame->execution_offset);
	gen_log(GEN_LOG_LEVEL_DEBUG, "cionom", "ret");
#endif

	return NULL;
}

gen_error_t* cio_vm_dispatch_callable(cio_vm_t* const restrict vm, const cio_callable_t* callable, const size_t argc) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_vm_dispatch_callable, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `NULL`");

    error = cio_vm_push_frame(vm);
    if(error) return error;

	// Construct call
    size_t callable_remote_index = callable->routine_index;
	vm->frames[vm->frames_used - 1].height = argc;
	vm->frames[vm->frames_used - 1].execution_offset = callable->offset;
    size_t old_bytecode = vm->current_bytecode;
    vm->current_bytecode = callable->bytecode_index;

#if CIO_VM_DEBUG_PRINTS == GEN_ENABLED
    gen_log_formatted(GEN_LOG_LEVEL_DEBUG, "cionom", "Calling %t routine %uz (@%p) in BC %uz @ %uz", vm->bytecode[vm->current_bytecode].callables[callable_remote_index].function == cio_vm_internal_execute_routine ? "cionom" : "external", callable_remote_index, (void*) vm->bytecode[vm->current_bytecode].callables[callable_remote_index].function, vm->current_bytecode, vm->frames[vm->frames_used - 1].execution_offset);
#endif

    if(callable_remote_index >= vm->bytecode[vm->current_bytecode].callables_length) return gen_error_attach_backtrace(GEN_ERROR_OUT_OF_BOUNDS, GEN_LINE_NUMBER, "The index of the callable in remote module was greater than the remote module's callables length");

	// Dispatch call
	error = vm->bytecode[vm->current_bytecode].callables[callable_remote_index].function(vm);
    if(error) return error;

    vm->current_bytecode = old_bytecode;

	error = cio_vm_pop_frame(vm);
    if(error) return error;

	return NULL;
}

gen_error_t* cio_vm_dispatch_call(cio_vm_t* const restrict vm, const size_t callable, const size_t argc) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_vm_dispatch_call, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `NULL`");
	if(callable >= vm->bytecode[vm->current_bytecode].callables_length) return gen_error_attach_backtrace(GEN_ERROR_OUT_OF_BOUNDS, GEN_LINE_NUMBER, "`callable` was greater than the current module's callables length");

	return cio_vm_dispatch_callable(vm, &vm->bytecode[vm->current_bytecode].callables[callable], argc);
}

gen_error_t* cio_vm_get_identifier(cio_vm_t* const restrict vm, const char* identifier, cio_callable_t* restrict * const restrict out_callable, bool vminit) {
    GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_vm_get_identifier, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `NULL`");
	if(!identifier) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`identifier` was `NULL`");
	if(!out_callable) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`out_callable` was `NULL`");

    size_t len = 0;
    error = gen_string_length(identifier, GEN_STRING_NO_BOUNDS, GEN_STRING_NO_BOUNDS, &len);
    if(error) return error;

    // TODO: Generate & reference linear list of routine
    //       Definitions (callables) at VM init instead
    //       Of maintaining per-bytecode.
    //       Also allows module-agnostic calls by the native
    //       Application.

#if CIO_VM_DEBUG_PRINTS == GEN_ENABLED
        // TODO: Maybe this should be a warning
        if(!vm->bytecode_length) gen_log(GEN_LOG_LEVEL_DEBUG, "cionom", "Executable bundle has no callables");
#endif

    cio_callable_t* extref = NULL;    

    for(size_t i = 0; i < vm->bytecode_length; ++i) {
#if CIO_VM_DEBUG_PRINTS == GEN_ENABLED
        // TODO: Maybe this should be a warning
        if(!vm->bytecode[i].callables_length) gen_log_formatted(GEN_LOG_LEVEL_DEBUG, "cionom", "Bytecode module %uz has no callables", i);
#endif
        for(size_t j = 0; j < vm->bytecode[i].callables_length; ++j) {
#if CIO_VM_DEBUG_PRINTS == GEN_ENABLED
            gen_log_formatted(GEN_LOG_LEVEL_DEBUG, "cionom", "Trying to resolve `%t` against `%t` from bytecode module %uz routine %uz/%uz...", identifier, vm->bytecode[i].callables[j].identifier, i, j, vm->bytecode[i].callables_length);
#endif

            bool equal = false;
            if(vm->bytecode[i].callables[j].identifier_length == len) {
                error = gen_string_compare(identifier, GEN_STRING_NO_BOUNDS, vm->bytecode[i].callables[j].identifier, GEN_STRING_NO_BOUNDS, vm->bytecode[i].callables[j].identifier_length, &equal);
                if(error) return error;
            }

            if(vm->bytecode[i].callables[j].offset == CIO_ROUTINE_EXTERNAL) {
                if(equal) extref = &vm->bytecode[i].callables[j]; // Fallback to pure-external routine
                continue;
            }

            if(equal) {
                *out_callable = &vm->bytecode[i].callables[j];
                return NULL;
            }
        }
    }

    if(!vminit) {
        if(extref) {
#if CIO_VM_DEBUG_PRINTS == GEN_ENABLED
            gen_log_formatted(GEN_LOG_LEVEL_WARNING, "cionom", "Resolving identifier %t to purely external callable", identifier);
#endif
            *out_callable = extref;
            return NULL;
        }
    }

    return gen_error_attach_backtrace_formatted(GEN_ERROR_NO_SUCH_OBJECT, GEN_LINE_NUMBER, "Could not find identifier `%t`", identifier);
}

gen_error_t* cio_vm_initialize(const unsigned char* const restrict bytecode, const size_t bytecode_length, const size_t stack_length, bool resolve_externals, cio_vm_t* const restrict out_instance, const cio_warning_settings_t* const restrict warning_settings) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_vm_initialize, GEN_FILE_NAME);
	if(error) return error;

    // TODO: Verify that modules are actually modules

	if(!bytecode) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`bytecode` was `NULL`");
	if(!bytecode_length) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`bytecode_length` was 0");
	if(!out_instance) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`out_instance` was `NULL`");

    out_instance->warning_settings = warning_settings;

	out_instance->stack_length = stack_length;
	error = gen_memory_allocate_zeroed((void**) &out_instance->stack, out_instance->stack_length, sizeof(size_t));
    if(error) return error;
	out_instance->frames_length = stack_length;
	error = gen_memory_allocate_zeroed((void**) &out_instance->frames, out_instance->frames_length, sizeof(cio_frame_t));
    if(error) return error;
    static const char external_lib_name[] = "cionom-external";
	error = gen_dynamic_library_handle_open(external_lib_name, sizeof(external_lib_name) - 1, &out_instance->external_lib);
    if(error) return error;

    cio_routine_function_t onload = NULL;
    error = gen_dynamic_library_handle_get_symbol(&out_instance->external_lib, "__cionom_extlib_onload", sizeof("__cionom_extlib_onload") - 1, (void**) &onload);
    if(error && error->type != GEN_ERROR_NO_SUCH_OBJECT) return error;

    for(size_t i = 0; i < bytecode_length; ++i) {
#if CIO_VM_DEBUG_PRINTS == GEN_ENABLED
            error = gen_log_formatted(GEN_LOG_LEVEL_DEBUG, "cionom", "Began decoding bytecode module %uz at offset %p in bundle", out_instance->bytecode_length, i);
            if(error) return error;
#endif

        error = gen_memory_reallocate_zeroed((void**) &out_instance->bytecode, out_instance->bytecode_length, out_instance->bytecode_length + 1, sizeof(cio_bytecode_t));
        if(error) return error;

        cio_bytecode_t* module = &out_instance->bytecode[out_instance->bytecode_length];

        module->callables_length = bytecode[i] & 0b01111111;

        size_t offset = 1;
        if(bytecode[i] & 0b10000000) {
#if CIO_VM_DEBUG_PRINTS == GEN_ENABLED
            error = gen_log_formatted(GEN_LOG_LEVEL_DEBUG, "cionom", "Module %uz contains extension data", out_instance->bytecode_length);
            if(error) return error;
#endif
            ++offset;

            do {
                error = gen_memory_reallocate_zeroed((void**) &module->extensions, module->extensions_length, module->extensions_length + 1, sizeof(cio_extension_data_t));
                if(error) return error;

                cio_extension_data_t* extension = &module->extensions[module->extensions_length];

                // Doing this here because otherwise clang thinks all cases have been covered
                switch(bytecode[offset] & 0b01111111) {
                    case CIO_EXTENSION_ID_ELIDE_RESERVE_SPACE: {
                        module->extension_settings.elide_reserve_space = true;
                        break;
                    }
                    case CIO_EXTENSION_ID_CONSTANTS: {
                        module->extension_settings.constants = true;
                        // TODO: Process extension data for `CIO_EXTENSION_ID_CONSTANTS`
                        break;
                    }
                    case CIO_EXTENSION_ID_NIL_CALLS: {
                        module->extension_settings.nil_calls = true;
                        // TODO: Process extension data for `CIO_EXTENSION_ID_NIL_CALLS`
                        break;
                    }
                    case CIO_EXTENSION_ID_BREAKPOINTS: {
                        module->extension_settings.breakpoints = true;
                        break;
                    }
                    case CIO_EXTENSION_ID_DEBUG_INFO: {
                        module->extension_settings.debug_info = true;
                        // TODO: Process extension data for `CIO_EXTENSION_ID_DEBUG_INFO`
                        break;
                    }
                    case CIO_EXTENSION_ID_ENCODE_STACK_LENGTH: {
                        module->extension_settings.encode_stack_length = true;
                        // TODO: Process extension data for `CIO_EXTENSION_ID_DEBUG_INFO`
                        break;
                    }
                    default: {
                        return gen_error_attach_backtrace_formatted(GEN_ERROR_BAD_CONTENT, GEN_LINE_NUMBER, "Extension ID %uc is unrecognized in bytecode %uz", extension->id, out_instance->bytecode_length);
                    }
                }
                extension->id = bytecode[offset] & 0b01111111;

                module->extensions_length++;
            } while(bytecode[offset] & 0b10000000);
        }

        if(module->callables_length) {
            error = gen_memory_allocate_zeroed((void**) &module->callables, module->callables_length, sizeof(cio_callable_t));
            if(error) return error;
        }

        for(size_t j = 0; j < module->callables_length; ++j) {
#ifdef __ANALYZER
            cio_routine_table_entry_t* entry = malloc(sizeof(cio_routine_table_entry_t));
#else
            const cio_routine_table_entry_t* entry = (const cio_routine_table_entry_t*) &bytecode[i + offset];
#endif

            size_t stride = 0;
            error = gen_string_length(entry->name, GEN_STRING_NO_BOUNDS, GEN_STRING_NO_BOUNDS, &stride);
#ifdef __ANALYZER
            if(error) {
                free(entry);
                return error;
            }
#else
            if(error) return error;
#endif

            module->callables[j] = (cio_callable_t) {entry->name, stride, cio_vm_internal_execute_routine, out_instance->bytecode_length, entry->offset, j};

#if CIO_VM_DEBUG_PRINTS == GEN_ENABLED
            error = gen_log_formatted(GEN_LOG_LEVEL_DEBUG, "cionom", "Appended routine table entry for %t routine `%tz` in bytecode module %uz at index %uz/%uz", entry->offset == CIO_ROUTINE_EXTERNAL ? "external" : "internal", entry->name, stride, out_instance->bytecode_length, j, module->callables_length);
            if(error) return error;
#endif

            offset += sizeof(entry->offset) + stride + 1;
#ifdef __ANALYZER
            free(entry);
#endif
        }

#if CIO_VM_DEBUG_PRINTS == GEN_ENABLED
        error = gen_log_formatted(GEN_LOG_LEVEL_DEBUG, "cionom", "Set bytecode module %uz code block at offset %p in bundle", out_instance->bytecode_length, offset + i);
        if(error) return error;
#endif
        size_t code_block_offset = i + offset;
        module->bytecode = &bytecode[i + offset];

        i += offset;

        // Add on offset of last callable
        i += module->callables[module->callables_length - 1].offset;

        for(; bytecode[i] != 0xFF; ++i);

        module->size = (i - code_block_offset) + 1;
#if CIO_VM_DEBUG_PRINTS == GEN_ENABLED
        error = gen_log_formatted(GEN_LOG_LEVEL_DEBUG, "cionom", "Bytecode code block size %uz", module->size);
        if(error) return error;
#endif

        ++out_instance->bytecode_length;
    }

    if(resolve_externals) {
        for(size_t i = 0; i < out_instance->bytecode_length; ++i) {
            for(size_t j = 0; j < out_instance->bytecode[i].callables_length; ++j) {
                if(out_instance->bytecode[i].callables[j].offset != CIO_ROUTINE_EXTERNAL) continue;

                cio_callable_t* callable = NULL;
#ifdef __ANALYZER
                cio_callable_t dummy = {0};
                callable = &dummy;
#endif
#if CIO_VM_DEBUG_PRINTS == GEN_ENABLED
                gen_log_formatted(GEN_LOG_LEVEL_DEBUG, "cionom", "Trying to resolve `%t`...", out_instance->bytecode[i].callables[j].identifier);
#endif

                error = cio_vm_get_identifier(out_instance, out_instance->bytecode[i].callables[j].identifier, &callable, true);
                if(error && error->type == GEN_ERROR_NO_SUCH_OBJECT) {
#if CIO_VM_DEBUG_PRINTS == GEN_ENABLED
                    gen_log_formatted(GEN_LOG_LEVEL_DEBUG, "cionom", "Resolving `%t` in external code", out_instance->bytecode[i].callables[j].identifier);
#endif
                    error = cio_resolve_external(out_instance->bytecode[i].callables[j].identifier, &out_instance->bytecode[i].callables[j].function, &out_instance->external_lib);
                    if(error) return error;
    
                    continue;
                }
                else if(error) return error;

                out_instance->bytecode[i].callables[j].offset = callable->offset;
                out_instance->bytecode[i].callables[j].bytecode_index = callable->bytecode_index;
                out_instance->bytecode[i].callables[j].routine_index = callable->routine_index;
            }
        }
    }

    if(onload) {
        error = onload(out_instance);
        if(error) return error;
    }

	return NULL;
}

gen_error_t* cio_vm_free(cio_vm_t* const restrict instance) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_vm_free, GEN_FILE_NAME);
	if(error) return error;

	if(!instance) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`instance` was `NULL`");

    if(instance->stack) {
        error = gen_memory_free((void**) &instance->stack);
        if(error) return error;
    }

    if(instance->frames) {
        error = gen_memory_free((void**) &instance->frames);
        if(error) return error;
    }

    if(instance->external_lib) {
        error = gen_dynamic_library_handle_close(instance->external_lib);
        if(error) return error;
    }

    for(size_t i = 0; i < instance->bytecode_length; ++i) {
        if(instance->bytecode[i].callables) {
            error = gen_memory_free((void**) &instance->bytecode[i].callables);
            if(error) return error;
        }
    }

    if(instance->bytecode) {
        error = gen_memory_free((void**) &instance->bytecode);
        if(error) return error;
    }

	return NULL;
}

gen_error_t* cio_vm_get_frame(const cio_vm_t* const restrict vm, const size_t frame_offset, cio_frame_t** restrict out_pointer) {
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
