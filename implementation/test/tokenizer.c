// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

#define GEN_TESTS_UNIT "tokenizer"
#include <gentests.h>
#include <genmemory.h>
#include <cionom.h>

static gen_error_t* gen_main(void) {
    GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) gen_main, GEN_FILE_NAME);
	if(error) return error;

    const char source[] = "x 1 $cio::__foobar 0 :c1 11 c11:";
    cio_token_t* tokens = GEN_NULL;
    gen_size_t length = 0;
    error = cio_tokenize(source, sizeof(source) - 1, &tokens, &length);
	if(error) return error;

    cio_token_t expected[] = {
        {CIO_TOKEN_IDENTIFIER, 0, 1},
        {CIO_TOKEN_NUMBER, 2, 1},
        {CIO_TOKEN_IDENTIFIER, 4, 14},
        {CIO_TOKEN_NUMBER, 19, 1},
        {CIO_TOKEN_BLOCK, 21, 1},
        {CIO_TOKEN_IDENTIFIER, 22, 2},
        {CIO_TOKEN_NUMBER, 25, 2},
        {CIO_TOKEN_IDENTIFIER, 28, 3},
        {CIO_TOKEN_BLOCK, 31, 1}
    };

    error = GEN_TESTS_EXPECT(sizeof(expected) / sizeof(expected[0]), length);
    if(error) return error;

    gen_bool_t equal = gen_false;
    error = gen_memory_compare(expected, sizeof(expected), tokens, sizeof(expected), sizeof(expected), &equal);
    if(error) return error;

    error = GEN_TESTS_EXPECT(gen_true, equal);
    if(error) return error;

    return GEN_NULL;
}
