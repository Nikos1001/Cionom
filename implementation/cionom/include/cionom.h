// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+genstone@pm.me>

#include <gencommon.h>
#include <genstring.h>

GEN_ERRORABLE cio_line_from_offset(const size_t offset, size_t* const restrict out_line, const char* const restrict source, const size_t source_length);
GEN_ERRORABLE cio_column_from_offset(const size_t offset, size_t* const restrict out_column, const char* const restrict source, const size_t source_length);

typedef struct {
    enum {
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
        CIO_TOKEN_STRING
    } type;
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
        cio_storage_t* storage;
        const char* string;
        size_t number;
    };
};

struct cio_storage {
    const char* identifier;
    enum {
        CIO_SIZE_CONSTANT,
        CIO_SIZE_VARIABLE,
    } size_type;
    union {
        size_t constant;
        const cio_storage_t* variable;
    } size;
    size_t alignment;
};

struct cio_call {
    cio_routine_t* routine;
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
