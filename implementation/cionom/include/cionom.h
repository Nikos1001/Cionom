// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+genstone@pm.me>

#include <gencommon.h>
#include <genstring.h>

GEN_ERRORABLE cio_line_from_offset(const size_t offset, size_t* const restrict out_line, const char* const restrict source, const size_t source_length);
GEN_ERRORABLE cio_column_from_offset(const size_t offset, size_t* const restrict out_column, const char* const restrict source, const size_t source_length);

typedef enum {
    CIO_TOKEN_IDENTIFIER = 1 << 1,
    CIO_TOKEN_RETURN = 1 << 2,
    CIO_TOKEN_STORAGE = 1 << 3,
    CIO_TOKEN_ALIGNMENT = 1 << 4,
    CIO_TOKEN_BLOCK_START = 1 << 5,
    CIO_TOKEN_BLOCK_END = 1 << 6,
    CIO_TOKEN_SPECIFIER_EXPRESSION_START = 1 << 7,
    CIO_TOKEN_SPECIFIER_EXPRESSION_END = 1 << 8,
    CIO_TOKEN_STATEMENT_DELIMITER = 1 << 9,
    CIO_TOKEN_PARAMETER_DELIMITER = 1 << 10,
    CIO_TOKEN_NUMBER = 1 << 11,
    CIO_TOKEN_STRING = 1 << 12
} cio_token_type_t;

typedef struct {
    cio_token_type_t type;
    size_t offset;
    size_t length;
} cio_token_t;

GEN_ERRORABLE cio_tokenize(const char* const restrict source, const size_t source_length, cio_token_t** const restrict out_tokens, size_t* const restrict out_tokens_length);

typedef struct cio_expression cio_expression_t;
typedef struct cio_storage cio_storage_t;
typedef struct cio_call cio_call_t;
typedef struct cio_statement cio_statement_t;
typedef struct cio_routine cio_routine_t;
typedef struct cio_program cio_program_t;

struct cio_expression {
    enum {
        CIO_EXPRESSION_STORAGE,
        CIO_EXPRESSION_STRING,
        CIO_EXPRESSION_NUMBER
    } type;
    union {
        char* identifier;
        char* string;
        size_t number;
    };
};

struct cio_storage {
    char* identifier;
    size_t size;
    size_t alignment;
};

struct cio_call {
    char* identifier;
    size_t parameters_length;
    cio_expression_t* parameters;
};

struct cio_statement {
    enum {
        CIO_STATEMENT_STORAGE,
        CIO_STATEMENT_CALL,
        CIO_STATEMENT_RETURN
    } type;
    union {
        cio_storage_t storage;
        cio_call_t call;
    };
};

struct cio_routine {
    char* identifier;
    size_t parameters_length;
    cio_storage_t* parameters;
    size_t statements_length;
    cio_statement_t* statements;
};

struct cio_program {
    size_t routines_length;
    cio_routine_t* routines;
};

GEN_ERRORABLE cio_parse(const cio_token_t* const restrict tokens, const size_t tokens_length, cio_program_t* const restrict out_program, const char* const restrict source, const size_t source_length, const char* const restrict source_file, const size_t source_file_length);
