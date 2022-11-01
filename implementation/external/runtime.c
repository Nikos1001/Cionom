// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

#include "include/extlib_detail.h"

#include <cioextlib.h>
#include <cionom.h>

CIO_EXTLIB_BEGIN_DEFS

//* `!` - Exits the program with a generic error.
//* @reserve Empty.
gen_error_t* __cionom_mangled_grapheme_bang(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) __cionom_mangled_grapheme_bang, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `GEN_NULL`");

    return gen_error_attach_backtrace(GEN_ERROR_UNKNOWN, GEN_LINE_NUMBER, "The program was terminated");
}

//* `set!` - Sets the routine to be used as the exception handler.
//* @param [0] The routine index to use as the new exception handler.
//* @reserve Empty.
gen_error_t* set__cionom_mangled_grapheme_bang(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) __cionom_mangled_grapheme_bang, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `GEN_NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);

    cio_callable_t** exception_callable = &((cio_extlib_data_t*) vm->external_lib_storage)->exception_callable;

    *exception_callable = &vm->bytecode[vm->current_bytecode].callables[current[0]];

    return GEN_NULL;
}

//* `unset!` - Sets the routine to be used as the exception handler.
//* @reserve Empty.
gen_error_t* unset__cionom_mangled_grapheme_bang(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) unset__cionom_mangled_grapheme_bang, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `GEN_NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);

    cio_callable_t** exception_callable = &((cio_extlib_data_t*) vm->external_lib_storage)->exception_callable;

    *exception_callable = GEN_NULL;

    return GEN_NULL;
}


CIO_EXTLIB_END_DEFS
