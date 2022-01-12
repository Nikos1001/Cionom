// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+genstone@pm.me>

#include <gencommon.h>

typedef enum {
    CIO_TOKEN_IDENTIFIER,

    CIO_TOKEN_RETURN,
    CIO_TOKEN_STORAGE,
    CIO_TOKEN_ALIGNMENT,

    CIO_TOKEN_BLOCK_START,
    CIO_TOKEN_BLOCK_END,

    CIO_TOKEN_SPECIFIER_EXPRESSION_START,
    CIO_TOKEN_SPECIFIER_EXPRESSION_END,

    CIO_TOKEN_STATEMENT_DELIMITER,
    CIO_TOKEN_PARAMETER_DELIMITER,

    CIO_TOKEN_NUMBER,
    CIO_TOKEN_STRING,
} cio_token_type_t;

typedef struct {
    cio_token_type_t type;
    size_t offset;
    size_t length;
} cio_token_t;

GEN_ERRORABLE cio_tokenize(const char* const restrict source, cio_token_t** const restrict out_tokens, size_t* const restrict out_n_tokens);
