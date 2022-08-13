// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+cionom@pm.me>

#include <cioextlib.h>
#include <cionom.h>
#include <genfs.h>

CIO_EXTLIB_BEGIN_DEFS

//* `path?` - Checks whether a path exists.
//* @param [0] The stack index containing the pointer to the first character of a NULL-terminated path string.
//* @reserve 1 if the path exists, 0 otherwise.
gen_error_t path__cionom_mangled_grapheme_question_mark(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(path__cionom_mangled_grapheme_question_mark);

	GEN_NULL_CHECK(vm);

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	bool exists = false;
	gen_error_t error = gen_path_exists((char*) caller[current[0]], &exists);
	GEN_ERROR_OUT_IF(error, "`gen_path_exists` failed");

	caller[caller_frame->height - 1] = exists;

	GEN_ALL_OK;
}

CIO_EXTLIB_END_DEFS
