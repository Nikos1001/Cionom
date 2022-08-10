// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+cionom@pm.me>

#include <cioextlib.h>
#include <cionom.h>

CIO_EXTLIB_BEGIN_DEFS

//* `?` - Branches control flow to a routine if a condition is met.
//* @param [0] The index of the routine to branch to.
//* @param [1] The stack index of the condition.
//* @reserve Empty.
gen_error_t __cionom_mangled_grapheme_question_mark(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(__cionom_mangled_grapheme_question_mark);

	GEN_NULL_CHECK(vm);

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);

	if(current[1]) {
		gen_error_t error = cio_vm_dispatch_call(vm, current[0], 0);
		GEN_ERROR_OUT_IF(error, "`cio_vm_dispatch_call` failed");
	}

	GEN_ALL_OK;
}

CIO_EXTLIB_END_DEFS
