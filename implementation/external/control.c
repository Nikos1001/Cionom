// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+cionom@pm.me>

#include <cioextlib.h>
#include <cionom.h>

CIO_EXTLIB_BEGIN_DEFS

//* `?` - Branches control flow to a routine based on a condition.
//* @param [0] The index of the routine to branch to if the condition is true.
//* @param [1] The index of the routine to branch to if the condition is false.
//* @param [2] The stack index of the condition.
//* @reserve Empty.
gen_error_t __cionom_mangled_grapheme_question_mark(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(__cionom_mangled_grapheme_question_mark);

	GEN_NULL_CHECK(vm);

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	gen_error_t error = cio_vm_dispatch_call(vm, caller[current[2]] ? current[0] : current[1], 0);
	GEN_ERROR_OUT_IF(error, "`cio_vm_dispatch_call` failed");

	GEN_ALL_OK;
}

CIO_EXTLIB_END_DEFS
