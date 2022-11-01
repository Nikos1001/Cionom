// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

#include "include/extlib_detail.h"

#include <cionom.h>
#include <cioextlib.h>

#include <genmemory.h>
#include <genstring.h>
#include <genlog.h>

CIO_EXTLIB_BEGIN_DEFS

gen_error_t* __cionom_extlib_on_load(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) __cionom_extlib_on_load, GEN_FILE_NAME);
	if(error) return error;

    error = gen_memory_allocate_zeroed((void**) &vm->external_lib_storage, 1, sizeof(cio_extlib_data_t));
	if(error) return error;

    error = cio_vm_get_identifier(vm, "__cionom_extlib_default_exception_handler", &((cio_extlib_data_t*) vm->external_lib_storage)->exception_callable, gen_false);
	if(error && error->type != GEN_ERROR_NO_SUCH_OBJECT) return error;

    return GEN_NULL;
}

gen_error_t* __cionom_extlib_wrap_call(cio_vm_t* const restrict vm, const cio_routine_function_t call) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) __cionom_extlib_wrap_call, GEN_FILE_NAME);
	if(error) return error;

    cio_callable_t** exception_callable = &((cio_extlib_data_t*) vm->external_lib_storage)->exception_callable;

    // TODO: Return error if current callable is the exception handler

    if(vm->debug_prints) gen_log(GEN_LOG_LEVEL_DEBUG, "cionom", "Precall");
    error = call(vm);
    if(error && *exception_callable) {
        if(vm->debug_prints) gen_log(GEN_LOG_LEVEL_DEBUG, "cionom", "Exception!");

        gen_error_t ctx = *error;

        error = cio_vm_push_frame(vm);
    	if(error) return error;

        // Here we can elide reserve value as it is a special call

        // Error code
        error = cio_vm_push(vm);
    	if(error) return error;

        // Error context string
        error = cio_vm_push(vm);
    	if(error) return error;

        gen_size_t* frame_ptr = GEN_NULL;
        error = cio_vm_get_frame_pointer(vm, &vm->frames[vm->frames_used - 1], &frame_ptr);
    	if(error) return error;

        frame_ptr[0] = ctx.type;
        frame_ptr[1] = (gen_size_t) ctx.context;

        vm->frames[vm->frames_used - 1].height = 0;

        cio_callable_t* exception_callable_copy = *exception_callable;
        *exception_callable = GEN_NULL;
        error = cio_vm_dispatch_callable(vm, exception_callable_copy, 2);
    	if(error) return error;
        *exception_callable = exception_callable_copy;

        error = cio_vm_pop_frame(vm);
    	if(error) return error;
    }
    
    return error;
}

CIO_EXTLIB_END_DEFS
