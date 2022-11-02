// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

#include <cioextlib.h>
#include <cionom.h>

CIO_EXTLIB_BEGIN_DEFS

//* `?` - Branches control flow to a routine based on a condition.
//* @param [0] The index of the routine to branch to if the condition is gen_true.
//* @param [1] The index of the routine to branch to if the condition is gen_false.
//* @param [2] The stack index of the condition.
//* @reserve Empty.
gen_error_t* __cionom_mangled_grapheme_question_mark(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) __cionom_mangled_grapheme_question_mark, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `GEN_NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

#ifdef __ANALYZER
#else
	error = cio_vm_dispatch_call(vm, caller[current[2]] ? current[0] : current[1], 0);
	if(error) return error;
#endif

	return GEN_NULL;
}

//* `?+-` - Branches control flow to a routine based on a condition.
//* @param [0] The index of the routine to branch to if the condition is greater than or equal to zero.
//* @param [1] The index of the routine to branch to if the condition is less than zero.
//* @param [2] The stack index of the condition.
//* @reserve Empty.
gen_error_t* __cionom_mangled_grapheme_question_mark__cionom_mangled_grapheme_plus__cionom_mangled_grapheme_minus(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) __cionom_mangled_grapheme_question_mark__cionom_mangled_grapheme_plus__cionom_mangled_grapheme_minus, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `GEN_NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	error = cio_vm_dispatch_call(vm, (gen_ssize_t) caller[current[2]] >= 0 ? current[0] : current[1], 0);
	if(error) return error;

	return GEN_NULL;
}

CIO_EXTLIB_END_DEFS
