// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

#define GEN_TESTS_UNIT "util"
#include <gentests.h>
#include <cionom.h>

extern gen_error_t* copy__cionom_mangled_grapheme_equals(cio_vm_t* const restrict vm);

GEN_PRAGMA(GEN_PRAGMA_DIAGNOSTIC_REGION_BEGIN)
GEN_PRAGMA(GEN_PRAGMA_DIAGNOSTIC_REGION_IGNORE("-Weverything"))
#include <dlfcn.h>
GEN_PRAGMA(GEN_PRAGMA_DIAGNOSTIC_REGION_END)

static gen_error_t* gen_main(void) {
    GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) gen_main, GEN_FILE_NAME);
	if(error) return error;

    {
        const char file[] = "test\nwoooooo\n";
        size_t line = 0;
        error = cio_line_from_offset(5, &line, file, sizeof(file) - 1);
    	if(error) return error;

        error = GEN_TESTS_EXPECT(2, line);
    	if(error) return error;

        size_t column = 0;
        error = cio_column_from_offset(5, &column, file, sizeof(file) - 1);
    	if(error) return error;

        error = GEN_TESTS_EXPECT(1, column);
    	if(error) return error;
    }

    {
        char* mangled = NULL;
        error = cio_mangle_identifier("&__+foo@bar", &mangled);     
    	if(error) return error;

        error = GEN_TESTS_EXPECT(false, mangled == NULL);
    	if(error) return error;

        error = GEN_TESTS_EXPECT("__cionom_mangled_grapheme_ampersand____cionom_mangled_grapheme_plusfoo__cionom_mangled_grapheme_atbar", mangled);
    	if(error) return error;
    }

    {
        const char external_lib_name[] = "cionom-external";
        gen_dynamic_library_handle_t handle = {0};
        error = gen_dynamic_library_handle_open(external_lib_name, sizeof(external_lib_name) - 1, &handle);
        if(error) return error;

        cio_routine_function_t proc = NULL;
        error = cio_resolve_external("copy=", &proc, &handle);
        if(error) return error;

        error = GEN_TESTS_EXPECT((void*) copy__cionom_mangled_grapheme_equals, (void*) proc);
        if(error) return error;

        error = gen_dynamic_library_handle_close(handle);
        if(error) return error;
    }

    return NULL;
}
