// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

#include <cioextlib.h>
#include <cionom.h>

CIO_EXTLIB_BEGIN_DEFS

//* `+` - Add two numbers.
//* @param [0] The stack index containing the first number to add.
//* @param [1] The stack index containing the second number to add.
//* @reserve The result of the addition.
gen_error_t* __cionom_mangled_grapheme_plus(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) __cionom_mangled_grapheme_plus, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `GEN_NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	caller[caller_frame->height - 1] = caller[current[0]] + caller[current[1]];

	return GEN_NULL;
}

//* `-` - Subtract a number from another.
//* @param [0] The stack index containing the number to subtract from.
//* @param [1] The stack index containing the number to subtract.
//* @reserve The result of the subtraction.
gen_error_t* __cionom_mangled_grapheme_minus(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) __cionom_mangled_grapheme_minus, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `GEN_NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	caller[caller_frame->height - 1] = caller[current[0]] - caller[current[1]];

	return GEN_NULL;
}

//* `*` - Multiplies two numbers.
//* @param [0] The stack index containing the first number to multiply.
//* @param [1] The stack index containing the second number to multiply.
//* @reserve The result of the multiplication.
gen_error_t* __cionom_mangled_grapheme_asterisk(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) __cionom_mangled_grapheme_asterisk, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `GEN_NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	caller[caller_frame->height - 1] = caller[current[0]] * caller[current[1]];

	return GEN_NULL;
}

//* `/` - Divide a number by another.
//* @param [0] The stack index containing the number to divide.
//* @param [1] The stack index containing the number to divide by.
//* @reserve The result of the division.
gen_error_t* __cionom_mangled_grapheme_slash(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) __cionom_mangled_grapheme_slash, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `GEN_NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	if(caller[current[1]] == 0) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "Division by zero");
	caller[caller_frame->height - 1] = caller[current[0]] / caller[current[1]];

	return GEN_NULL;
}

//* `%` - Divide a number by another, returning the remainder.
//* @param [0] The stack index containing the number to divide.
//* @param [1] The stack index containing the number to divide by.
//* @reserve The remainder of the division.
gen_error_t* __cionom_mangled_grapheme_percentage(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) __cionom_mangled_grapheme_percentage, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `GEN_NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	if(caller[current[1]] == 0) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "Division by zero");
	caller[caller_frame->height - 1] = caller[current[0]] % caller[current[1]];

	return GEN_NULL;
}

CIO_EXTLIB_END_DEFS
