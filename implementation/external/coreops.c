// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+cionom@pm.me>

#include <cioextlib.h>
#include <cionom.h>

CIO_EXTLIB_BEGIN_DEFS

//* `copy*[+]=c` - Copy value into pointer indexed.
//* @note Treats pointer and value as a char.
//* @param [0] The stack index containing the pointer to copy to.
//* @param [1] The index to apply to pointer.
//* @param [2] The value to copy.
//* @reserve Empty.
gen_error_t copy__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_left_bracket__cionom_mangled_grapheme_plus__cionom_mangled_grapheme_right_bracket__cionom_mangled_grapheme_equalsc(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(copy__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_left_bracket__cionom_mangled_grapheme_plus__cionom_mangled_grapheme_right_bracket__cionom_mangled_grapheme_equalsc);

	GEN_NULL_CHECK(vm);

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	((char*) caller[current[0]])[current[1]] = (char) current[2];

	GEN_ALL_OK;
}

//* `copy=*[+]c` - Copy value from pointer indexed.
//* @note Treats pointer and value as a char.
//* @param [0] The stack index to copy to.
//* @param [1] The stack index containing the pointer to copy from.
//* @param [2] The index to apply to pointer.
//* @reserve Empty.
gen_error_t copy__cionom_mangled_grapheme_equals__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_left_bracket__cionom_mangled_grapheme_plus__cionom_mangled_grapheme_right_bracketc(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(copy__cionom_mangled_grapheme_equals__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_left_bracket__cionom_mangled_grapheme_plus__cionom_mangled_grapheme_right_bracketc);

	GEN_NULL_CHECK(vm);

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	*(char*) &caller[current[0]] = ((char*) caller[current[1]])[current[2]];

	GEN_ALL_OK;
}

//* `copy=` -  Copy value into stack.
//* @param [0] The stack index to copy into.
//* @param [1] The value to copy.
//* @reserve Empty.
gen_error_t copy__cionom_mangled_grapheme_equals(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(copy__cionom_mangled_grapheme_equals);

	GEN_NULL_CHECK(vm);

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	caller[current[0]] = current[1];

	GEN_ALL_OK;
}

CIO_EXTLIB_END_DEFS
