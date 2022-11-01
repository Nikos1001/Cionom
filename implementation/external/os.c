// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

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

CIO_EXTLIB_END_DEFS
