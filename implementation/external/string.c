// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

#include <cioextlib.h>
#include <cionom.h>

#include <genstring.h>

GEN_PRAGMA(GEN_PRAGMA_DIAGNOSTIC_REGION_BEGIN)
GEN_PRAGMA(GEN_PRAGMA_DIAGNOSTIC_REGION_IGNORE("-Weverything"))
#include <stdlib.h>
GEN_PRAGMA(GEN_PRAGMA_DIAGNOSTIC_REGION_END)

CIO_EXTLIB_BEGIN_DEFS

//* `lenc*` - Calculates the length of a null terminated string of characters.
//* @param [0] The stack index of a pointer to the first character in the string.
//* @reserve The number of characters in the string.
gen_error_t* lenc__cionom_mangled_grapheme_asterisk(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) lenc__cionom_mangled_grapheme_asterisk, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `GEN_NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	error = gen_string_length((char*) caller[current[0]], GEN_STRING_NO_BOUNDS, GEN_STRING_NO_BOUNDS, &caller[caller_frame->height - 1]);
	if(error) return error;

	return GEN_NULL;
}

//* `fpstring?` - Checks whether a GEN_NULL-terminated string is valid for conversion to a floating-point number.
//* @param [0] The stack index of a pointer to the first character in the string.
//* @reserve 1 if the string is valid for conversion, 0 otherwise.
gen_error_t* fpstring__cionom_mangled_grapheme_question_mark(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) fpstring__cionom_mangled_grapheme_question_mark, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `GEN_NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	char* p = GEN_NULL;
	strtod((char*) caller[current[0]], &p); // TODO: Replace with Genstone IO
	// glogf(DEBUG, "`%s` is %sa valid floating point literal", (char*) caller[current[0]], *p == '\0' ? "" : "not ");
	caller[caller_frame->height - 1] = *p == '\0';

	return GEN_NULL;
}

CIO_EXTLIB_END_DEFS
