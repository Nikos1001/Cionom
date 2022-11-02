// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

#include <cioextlib.h>
#include <cionom.h>
#include <genfilesystem.h>
#include <genstring.h>

CIO_EXTLIB_BEGIN_DEFS

//* `path?` - Checks whether a path exists.
//* @param [0] The stack index containing the pointer to the first character of a GEN_NULL-terminated path string.
//* @reserve 1 if the path exists, 0 otherwise.
gen_error_t* path__cionom_mangled_grapheme_question_mark(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) path__cionom_mangled_grapheme_question_mark, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `GEN_NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	gen_bool_t exists = gen_false;
	error = gen_filesystem_path_exists((char*) caller[current[0]], GEN_STRING_NO_BOUNDS, &exists);
	if(error) return error;

	caller[caller_frame->height - 1] = exists;

	return GEN_NULL;
}

//* `pathcreatef` - Creates a new file at path.
//* @param [0] The stack index containing a pointer to the first character of a GEN_NULL-terminated path string.
//* @reserve Empty.
gen_error_t* pathcreatef(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) pathcreatef, GEN_FILE_NAME);
	if(error) return error;

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	error = gen_filesystem_path_create_file((char*) caller[current[0]], GEN_STRING_NO_BOUNDS);
	if(error) return error;

	return GEN_NULL;
}

//* `pathcreated` - Creates a new directory at path.
//* @param [0] The stack index containing a pointer to the first character of a GEN_NULL-terminated path string.
//* @reserve Empty.
gen_error_t* pathcreated(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) pathcreated, GEN_FILE_NAME);
	if(error) return error;

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	error = gen_filesystem_path_create_directory((char*) caller[current[0]], GEN_STRING_NO_BOUNDS);
	if(error) return error;

	return GEN_NULL;
}

CIO_EXTLIB_END_DEFS
