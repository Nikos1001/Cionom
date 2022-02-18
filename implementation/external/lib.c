// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+cionom@pm.me>

#include <cionom.h>

GEN_DIAG_REGION_BEGIN
#pragma clang diagnostic ignored "-Wmissing-prototypes"

gen_error_t copy__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_left_bracket__cionom_mangled_grapheme_plus__cionom_mangled_grapheme_right_bracket__cionom_mangled_grapheme_equals(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(copy__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_left_bracket__cionom_mangled_grapheme_plus__cionom_mangled_grapheme_right_bracket__cionom_mangled_grapheme_equals);

	GEN_INTERNAL_BASIC_PARAM_CHECK(vm);

	((unsigned char*) vm->stack[vm->frames[vm->frames_used - 2].base + vm->stack[vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height - 3]])[vm->stack[vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height - 2]] = (unsigned char) vm->stack[vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height - 1];

	GEN_ALL_OK;
}

gen_error_t print__cionom_mangled_grapheme_asterisk(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(print__cionom_mangled_grapheme_asterisk);

	GEN_INTERNAL_BASIC_PARAM_CHECK(vm);

	glogf(INFO, "%s", (char*) vm->stack[vm->frames[vm->frames_used - 2].base + vm->stack[vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height - 1]]);

	GEN_ALL_OK;
}

gen_error_t alloc(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(alloc);

	GEN_INTERNAL_BASIC_PARAM_CHECK(vm);

	gen_error_t error = gzalloc((void**) &vm->stack[vm->frames[vm->frames_used - 2].base + vm->frames[vm->frames_used - 2].height - 1], vm->stack[vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height - 1], 1);
	GEN_ERROR_OUT_IF(error, "`gzalloc` failed");

	GEN_ALL_OK;
}

GEN_DIAG_REGION_END
