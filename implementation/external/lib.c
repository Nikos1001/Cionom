// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+genstone@pm.me>

#include <cionom.h>

GEN_DIAG_REGION_BEGIN
#pragma clang diagnostic ignored "-Wmissing-prototypes"

gen_error_t storage(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(storage);

	GEN_INTERNAL_BASIC_PARAM_CHECK(vm);

	GEN_ERROR_OUT(GEN_NOT_IMPLEMENTED, "`storage` is not implemented yet :^)");
}

#define copy_value_to_indexed copy__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_left_bracket__cionom_mangled_grapheme_plus__cionom_mangled_grapheme_right_bracket__cionom_mangled_grapheme_equals
gen_error_t copy_value_to_indexed(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(copy_value_to_indexed);

	GEN_INTERNAL_BASIC_PARAM_CHECK(vm);

	GEN_ERROR_OUT(GEN_NOT_IMPLEMENTED, "`copy*[+]=` is not implemented yet :^)");
}

GEN_DIAG_REGION_END
