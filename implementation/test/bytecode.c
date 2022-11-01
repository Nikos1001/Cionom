// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

#define GEN_TESTS_UNIT "bytecode"
#include <gentests.h>
#include <genmemory.h>
#include <cionom.h>

static gen_error_t* gen_main(void) {
    GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) gen_main, GEN_FILE_NAME);
	if(error) return error;

    cio_program_t program = {
        1,
        (cio_routine_t[]) {
            {
                "foo", 0, 1,
                (cio_call_t[]) {
                    {"foo", 0, GEN_NULL, &(cio_token_t){CIO_TOKEN_IDENTIFIER, 0, 0}}
                },
                gen_false,
                &(cio_token_t){CIO_TOKEN_IDENTIFIER, 0, 0}
            }
        }
    };

    unsigned char* bytecode = GEN_NULL;
    gen_size_t length = 0;
    cio_warning_settings_t warning_settings = {0};
    error = gen_memory_set(&warning_settings, sizeof(warning_settings), gen_true);
    if(error) return error;

    error = cio_module_emit(&program, &bytecode, &length, "", 0, "", 0, &warning_settings);
    if(error) return error;

    unsigned char expected[] = {
        // Header
        1, // Routine Table Length
            // `foo:`
            0, 0, 0, 0, // Offset
            'f', 'o', 'o', '\0', // Identifier

        // Code
            // `foo:`
            0x00, // `push 0x0`
            0x80, // `call 0x0`
            0xFF  // `ret`
    };

    error = GEN_TESTS_EXPECT(sizeof(expected), length);
    if(error) return error;

    gen_bool_t equal = gen_false;
    error = gen_memory_compare(expected, sizeof(expected), bytecode, length, length, &equal);
    if(error) return error;

    error = GEN_TESTS_EXPECT(gen_true, equal);
    if(error) return error;

    error = gen_memory_free((void**) &bytecode);    
    if(error) return error;

    return GEN_NULL;
}
