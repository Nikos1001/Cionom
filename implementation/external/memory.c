// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+cionom@pm.me>

#include <cioextlib.h>
#include <cionom.h>

CIO_EXTLIB_BEGIN_DEFS

//* `alloc` - Allocate a buffer.
//* @param [0] The number of bytes to allocate.
//* @reserve A pointer to the allocated buffer.
gen_error_t alloc(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(alloc);

	GEN_NULL_CHECK(vm);

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	gen_error_t error = gzalloc((void**) &caller[caller_frame->height - 1], current[0], 1);
	GEN_ERROR_OUT_IF(error, "`gzalloc` failed");

	GEN_ALL_OK;
}

//* `allocv` - Allocate a buffer.
//* @param [0] The stack index containing the number of bytes to allocate.
//* @reserve A pointer to the allocated buffer.
gen_error_t allocv(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(allocv);

	GEN_NULL_CHECK(vm);

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	gen_error_t error = gzalloc((void**) &caller[caller_frame->height - 1], caller[current[0]], 1);
	GEN_ERROR_OUT_IF(error, "`gzalloc` failed");

	GEN_ALL_OK;
}

//* `free` - Free an allocated buffer.
//* @param [0] The stack index of the pointer to free.
//* @reserve Empty.
gen_error_t free__cionom_mangled_grapheme_asterisk(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(free__cionom_mangled_grapheme_asterisk);

	GEN_NULL_CHECK(vm);

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	gen_error_t error = gfree((void*) caller[current[0]]);
	GEN_ERROR_OUT_IF(error, "`gfree` failed");

	GEN_ALL_OK;
}

//* `buffcopy*->*+c` - Copy buffer to pointer with offset.
//* @note Treats pointer and value as a char.
//* @param [0] The stack index of the pointer to the beginning of the buffer to copy.
//* @param [1] The stack index of the pointer to copy to.
//* @param [2] The offset to apply to the pointer to copy to.
//* @param [3] The number of characters to copy.
gen_error_t buffcopy__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_minus__cionom_mangled_grapheme_right_chevron__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_plusc(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(buffcopy__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_minus__cionom_mangled_grapheme_right_chevron__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_plusc);

	GEN_NULL_CHECK(vm);

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	gen_error_t error = gen_memory_copy((void*) caller[current[0]], GEN_MEMORY_NO_BOUNDS, (void*) (caller[current[1]] + current[2]), GEN_MEMORY_NO_BOUNDS, current[3]);
	GEN_ERROR_OUT_IF(error, "`gen_memory_copy` failed");

	GEN_ALL_OK;
}

CIO_EXTLIB_END_DEFS
