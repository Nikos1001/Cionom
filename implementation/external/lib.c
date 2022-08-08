// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+cionom@pm.me>

#include <cionom.h>

GEN_DIAG_REGION_BEGIN
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma clang diagnostic ignored "-Wreserved-identifier"

//* `copy*[+]=c` - Copy value into pointer indexed.
//* @note Treats buffer and value as a char.
//* @param [0] The stack index containing the pointer.
//* @param [1] The index to apply to pointer.
//* @param [2] The value to copy.
//* @reserve Empty.
gen_error_t copy__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_left_bracket__cionom_mangled_grapheme_plus__cionom_mangled_grapheme_right_bracket__cionom_mangled_grapheme_equalsc(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(copy__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_left_bracket__cionom_mangled_grapheme_plus__cionom_mangled_grapheme_right_bracket__cionom_mangled_grapheme_equalsc);

	GEN_NULL_CHECK(vm);

	((unsigned char*) vm->stack[vm->frames[vm->frames_used - 2].base + vm->stack[vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height - 3]])[vm->stack[vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height - 2]] = (unsigned char) vm->stack[vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height - 1];

	GEN_ALL_OK;
}

//* `copy=` -  Copy value into stack.
//* @param [0] The stack index to copy into.
//* @param [1] The value to copy.
//* @reserve Empty.
gen_error_t copy__cionom_mangled_grapheme_equals(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(copy__cionom_mangled_grapheme_equals);

	GEN_NULL_CHECK(vm);

	vm->stack[vm->frames[vm->frames_used - 2].base + vm->stack[vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height - 2]] = vm->stack[vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height - 1];

	GEN_ALL_OK;
}

//* `print*` - Print pointer.
//* @param [0] The stack index containing the pointer to the first character of a NULL-terminated string.
//* @reserve Empty.
gen_error_t print__cionom_mangled_grapheme_asterisk(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(print__cionom_mangled_grapheme_asterisk);

	GEN_NULL_CHECK(vm);

	glogf(INFO, "%s", (char*) vm->stack[vm->frames[vm->frames_used - 2].base + vm->stack[vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height - 1]]);

	GEN_ALL_OK;
}

//* `print` - Print value from stack.
//* @param [0] The stack index containing the value to print.
//* @reserve Empty.
gen_error_t print(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(print);

	GEN_NULL_CHECK(vm);

	glogf(INFO, "%zu", vm->stack[vm->frames[vm->frames_used - 2].base + vm->stack[vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height - 1]]);

	GEN_ALL_OK;
}

//* `alloc` - Allocate a buffer.
//* @param [0] The number of bytes to allocate.
//* @reserve A pointer to the allocated buffer.
gen_error_t alloc(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(alloc);

	GEN_NULL_CHECK(vm);

	gen_error_t error = gzalloc((void**) &vm->stack[vm->frames[vm->frames_used - 2].base + vm->frames[vm->frames_used - 2].height - 1], vm->stack[vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height - 1], 1);
	GEN_ERROR_OUT_IF(error, "`gzalloc` failed");

	// glogf(DEBUG, "Allocated %zu bytes @ %p", vm->stack[vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height - 1], (void*) vm->stack[vm->frames[vm->frames_used - 2].base + vm->frames[vm->frames_used - 2].height - 1]);

	GEN_ALL_OK;
}

//* `free` - Free an allocated buffer.
//* @param [0] The stack index of the pointer to free.
//* @reserve Empty.
gen_error_t free__cionom_mangled_grapheme_asterisk(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(free__cionom_mangled_grapheme_asterisk);

	GEN_NULL_CHECK(vm);

	gen_error_t error = gfree((void**) vm->stack[vm->frames[vm->frames_used - 2].base + vm->stack[vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height - 1]]);
	GEN_ERROR_OUT_IF(error, "`gfree` failed");

	// glogf(DEBUG, "Freed %p", (void*) vm->stack[vm->frames[vm->frames_used - 2].base + vm->stack[vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height - 1]]);

	GEN_ALL_OK;
}

//* `+` - Add two numbers.
//* @param [0] The stack index containing the first number to add.
//* @param [1] The stack index containing the second number to add.
//* @reserve The result of the addition.
gen_error_t __cionom_mangled_grapheme_plus(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(__cionom_mangled_grapheme_plus);

	GEN_NULL_CHECK(vm);

	size_t lhs = vm->stack[vm->frames[vm->frames_used - 2].base + vm->stack[vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height - 1]];
	size_t rhs = vm->stack[vm->frames[vm->frames_used - 2].base + vm->stack[vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height - 2]];
	vm->stack[vm->frames[vm->frames_used - 2].base + vm->frames[vm->frames_used - 2].height - 1] = lhs + rhs;

	GEN_ALL_OK;
}

GEN_DIAG_REGION_END
