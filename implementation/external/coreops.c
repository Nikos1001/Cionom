// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

#include <cioextlib.h>
#include <cionom.h>

CIO_EXTLIB_BEGIN_DEFS

//* `copy*[+]=c` - Copy value into pointer indexed.
//* @note Treats pointer and value as a char.
//* @param [0] The stack index containing the pointer to copy to.
//* @param [1] The index to apply to pointer.
//* @param [2] The value to copy.
//* @reserve Empty.
gen_error_t* copy__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_left_bracket__cionom_mangled_grapheme_plus__cionom_mangled_grapheme_right_bracket__cionom_mangled_grapheme_equalsc(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) copy__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_left_bracket__cionom_mangled_grapheme_plus__cionom_mangled_grapheme_right_bracket__cionom_mangled_grapheme_equalsc, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	((char*) caller[current[0]])[current[1]] = (char) current[2];

	return NULL;
}

//* `copy*[+v]=c` - Copy value into pointer variably indexed.
//* @note Treats pointer and value as a char.
//* @param [0] The stack index containing the pointer to copy to.
//* @param [1] The stack index containing the index to apply to pointer.
//* @param [2] The value to copy.
//* @reserve Empty.
gen_error_t* copy__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_left_bracket__cionom_mangled_grapheme_plusv__cionom_mangled_grapheme_right_bracket__cionom_mangled_grapheme_equalsc(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) copy__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_left_bracket__cionom_mangled_grapheme_plusv__cionom_mangled_grapheme_right_bracket__cionom_mangled_grapheme_equalsc, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	((char*) caller[current[0]])[caller[current[1]]] = (char) current[2];

	return NULL;
}

//* `copy=*[+]c` - Copy value from pointer indexed.
//* @note Treats pointer and value as a char.
//* @param [0] The stack index to copy to.
//* @param [1] The stack index containing the pointer to copy from.
//* @param [2] The index to apply to pointer.
//* @reserve Empty.
gen_error_t* copy__cionom_mangled_grapheme_equals__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_left_bracket__cionom_mangled_grapheme_plus__cionom_mangled_grapheme_right_bracketc(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) copy__cionom_mangled_grapheme_equals__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_left_bracket__cionom_mangled_grapheme_plus__cionom_mangled_grapheme_right_bracketc, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	*(char*) &caller[current[0]] = ((char*) caller[current[1]])[current[2]];

	return NULL;
}

//* `copy=` -  Copy value into stack.
//* @param [0] The stack index to copy into.
//* @param [1] The value to copy.
//* @reserve Empty.
gen_error_t* copy__cionom_mangled_grapheme_equals(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) copy__cionom_mangled_grapheme_equals, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	caller[current[0]] = current[1];

	return NULL;
}

//* `copy=v` -  Copy variable into variable stack index.
//* @param [0] The stack index to copy into.
//* @param [1] The stack index to copy from.
//* @reserve Empty.
gen_error_t* copy__cionom_mangled_grapheme_equalsv(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) copy__cionom_mangled_grapheme_equalsv, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

    caller[current[0]] = caller[current[1]];

	return NULL;
}

//* `copy=cvv` -  Copy variable from caller stack frame into variable stack index.
//* @param [0] The stack index to copy into.
//* @param [1] The stack index of the stack index in the caller to copy from.
//* @reserve Empty.
gen_error_t* copy__cionom_mangled_grapheme_equalscvv(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) copy__cionom_mangled_grapheme_equalscvv, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller_caller, 2);

    caller[current[0]] = caller_caller[caller[current[1]]];

	return NULL;
}

CIO_EXTLIB_END_DEFS
