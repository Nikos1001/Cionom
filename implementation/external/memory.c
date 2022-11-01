// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

#include <cioextlib.h>
#include <cionom.h>

#include <genmemory.h>

CIO_EXTLIB_BEGIN_DEFS

//* `alloc` - Allocate a buffer.
//* @param [0] The number of bytes to allocate.
//* @reserve A pointer to the allocated buffer.
gen_error_t* alloc(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) alloc, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `GEN_NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	error = gen_memory_allocate_zeroed((void**) &caller[caller_frame->height - 1], current[0], 1);
	if(error) return error;

	return GEN_NULL;
}

//* `allocv` - Allocate a buffer.
//* @param [0] The stack index containing the number of bytes to allocate.
//* @reserve A pointer to the allocated buffer.
gen_error_t* allocv(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) allocv, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `GEN_NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	error = gen_memory_allocate_zeroed((void**) &caller[caller_frame->height - 1], caller[current[0]], 1);
	if(error) return error;

	return GEN_NULL;
}

//* `free` - Free an allocated buffer.
//* @param [0] The stack index of the pointer to free.
//* @reserve Empty.
gen_error_t* free__cionom_mangled_grapheme_asterisk(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) free__cionom_mangled_grapheme_asterisk, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `GEN_NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	error = gen_memory_free((void**) &caller[current[0]]);
	if(error) return error;

	return GEN_NULL;
}

//* `buffcopy*->*+c` - Copy buffer to pointer with offset.
//* @note Treats pointer and value as a char.
//* @param [0] The stack index of the pointer to the beginning of the buffer to copy.
//* @param [1] The stack index of the pointer to copy to.
//* @param [2] The offset to apply to the pointer to copy to.
//* @param [3] The number of characters to copy.
gen_error_t* buffcopy__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_minus__cionom_mangled_grapheme_right_chevron__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_plusc(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) buffcopy__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_minus__cionom_mangled_grapheme_right_chevron__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_plusc, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `GEN_NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	error = gen_memory_copy((void*) (caller[current[1]] + current[2]), GEN_MEMORY_NO_BOUNDS, (void*) caller[current[0]], GEN_MEMORY_NO_BOUNDS, current[3]);
	if(error) return error;

	return GEN_NULL;
}

CIO_EXTLIB_END_DEFS
