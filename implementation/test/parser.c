// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

#define GEN_TESTS_UNIT "parser"
#include <gentests.h>
#include <genmemory.h>
#include <genstring.h>
#include <cionom.h>

static gen_error_t* gen_main(void) {
    GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) gen_main, GEN_FILE_NAME);
	if(error) return error;

    const char source[] = "foo0bar:";

    cio_token_t tokens[] = {
        // `foo 0`
        {CIO_TOKEN_IDENTIFIER, 0, 3},
        {CIO_TOKEN_NUMBER, 3, 1},

        // `bar 0`
        {CIO_TOKEN_IDENTIFIER, 4, 3},
        {CIO_TOKEN_NUMBER, 3, 1},
        // `:`
        {CIO_TOKEN_BLOCK, 7, 1},
            // `foo`
            {CIO_TOKEN_IDENTIFIER, 0, 3},
        // `:`
        {CIO_TOKEN_BLOCK, 7, 1}
    };

    cio_program_t program = {0};
    cio_warning_settings_t warning_settings = {0};
    error = gen_memory_set(&warning_settings, sizeof(warning_settings), gen_true);
    if(error) return error;

    error = cio_parse(tokens, sizeof(tokens) / sizeof(tokens[0]), &program, source, sizeof(source) - 1, "", 0, &warning_settings);
    if(error) return error;

    cio_program_t program_expected = {
        2,
        (cio_routine_t[]) {
            {
                "foo", 0, 0, GEN_NULL, gen_true, &(cio_token_t){CIO_TOKEN_IDENTIFIER, 0, 3}
            },
            {
                "bar", 0, 1,
                (cio_call_t[]) {
                    {"foo", 0, GEN_NULL, &(cio_token_t){CIO_TOKEN_IDENTIFIER, 0, 3}}
                },
                gen_false,
                &(cio_token_t){CIO_TOKEN_IDENTIFIER, 4, 3}
            }
        }
    };

    error = GEN_TESTS_EXPECT(program_expected.routines_length, program.routines_length);
    if(error) return error;

    {
        error = GEN_TESTS_EXPECT(program_expected.routines[0].identifier, program.routines[0].identifier);
        if(error) return error;

        error = GEN_TESTS_EXPECT(program_expected.routines[0].parameters, program.routines[0].parameters);
        if(error) return error;

        error = GEN_TESTS_EXPECT(program_expected.routines[0].calls_length, program.routines[0].calls_length);
        if(error) return error;

        error = GEN_TESTS_EXPECT((void*) program_expected.routines[0].calls, program.routines[0].calls);
        if(error) return error;

        error = GEN_TESTS_EXPECT(program_expected.routines[0].external, program.routines[0].external);
        if(error) return error;

        gen_bool_t equal = gen_false;
        error = gen_memory_compare(program_expected.routines[0].token, sizeof(cio_token_t), program.routines[0].token, sizeof(cio_token_t), sizeof(cio_token_t), &equal);
        if(error) return error;

        error = GEN_TESTS_EXPECT(gen_true, equal);
        if(error) return error;
    }

    {
        error = GEN_TESTS_EXPECT(program_expected.routines[1].identifier, program.routines[1].identifier);
        if(error) return error;

        error = GEN_TESTS_EXPECT(program_expected.routines[1].parameters, program.routines[1].parameters);
        if(error) return error;

        error = GEN_TESTS_EXPECT(program_expected.routines[1].calls_length, program.routines[1].calls_length);
        if(error) return error;

        error = GEN_TESTS_EXPECT(program_expected.routines[1].calls[0].identifier, program.routines[1].calls[0].identifier);
        if(error) return error;

        error = GEN_TESTS_EXPECT(program_expected.routines[1].calls[0].parameters_length, program.routines[1].calls[0].parameters_length);
        if(error) return error;

        error = GEN_TESTS_EXPECT((void*) program_expected.routines[1].calls[0].parameters, program.routines[1].calls[0].parameters);
        if(error) return error;

        gen_bool_t equal = gen_false;
        error = gen_memory_compare(program_expected.routines[1].calls[0].token, sizeof(cio_token_t), program.routines[1].calls[0].token, sizeof(cio_token_t), sizeof(cio_token_t), &equal);
        if(error) return error;

        error = GEN_TESTS_EXPECT(gen_true, equal);
        if(error) return error;

        error = GEN_TESTS_EXPECT(program_expected.routines[0].external, program.routines[0].external);
        if(error) return error;

        error = gen_memory_compare(program_expected.routines[1].token, sizeof(cio_token_t), program.routines[1].token, sizeof(cio_token_t), sizeof(cio_token_t), &equal);
        if(error) return error;

        error = GEN_TESTS_EXPECT(gen_true, equal);
        if(error) return error;
    }

    error = cio_program_free(&program);
    if(error) return error;

    return GEN_NULL;
}
