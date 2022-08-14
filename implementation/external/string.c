// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+cionom@pm.me>

#include <cioextlib.h>
#include <cionom.h>

CIO_EXTLIB_BEGIN_DEFS

//* `lenc*` - Calculates the length of a null terminated string of characters.
//* @param [0] The stack index of a pointer to the first character in the string.
//* @reserve The number of characters in the string.
gen_error_t lenc__cionom_mangled_grapheme_asterisk(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(lenc__cionom_mangled_grapheme_asterisk);

	GEN_NULL_CHECK(vm);

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	gen_error_t error = gen_string_length((char*) caller[current[0]], GEN_STRING_NO_BOUND, GEN_STRING_NO_BOUND, &caller[caller_frame->height - 1]);
	GEN_ERROR_OUT_IF(error, "`gen_string_length` failed");

	GEN_ALL_OK;
}

//* `fpstring?` - Checks whether a NULL-terminated string is valid for conversion to a floating-point number.
//* @param [0] The stack index of a pointer to the first character in the string.
//* @reserve 1 if the string is valid for conversion, 0 otherwise.
gen_error_t fpstring__cionom_mangled_grapheme_question_mark(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(fpstring__cionom_mangled_grapheme_question_mark);

	GEN_NULL_CHECK(vm);

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	char* p = NULL;
	strtod((char*) caller[current[0]], &p);
	// glogf(DEBUG, "`%s` is %sa valid floating point literal", (char*) caller[current[0]], *p == '\0' ? "" : "not ");
	caller[caller_frame->height - 1] = *p == '\0';

	GEN_ALL_OK;
}

CIO_EXTLIB_END_DEFS
