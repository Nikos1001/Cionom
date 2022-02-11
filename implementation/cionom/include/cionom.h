// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+genstone@pm.me>

#include <gencommon.h>
#include <genstring.h>
#include <gendl.h>

typedef enum {
    CIO_TOKEN_IDENTIFIER,
    CIO_TOKEN_BLOCK,
    CIO_TOKEN_NUMBER
} cio_token_type_t;

typedef struct {
    cio_token_type_t type;
    size_t offset;
    size_t length;
} cio_token_t;

typedef struct {
    char* identifier;
    size_t parameters_length;
    size_t* parameters;

    const cio_token_t* token;
} cio_call_t;

typedef struct {
    char* identifier;
    size_t parameters;
    size_t calls_length;
    cio_call_t* calls;
    bool external;

    const cio_token_t* token;
} cio_routine_t;

typedef struct {
    size_t routines_length;
    cio_routine_t* routines;
} cio_program_t;

typedef enum
{
	CIO_BYTECODE_OPERATION_PUSH = 0xAA,
	CIO_BYTECODE_OPERATION_CALL = 0x55
} cio_bytecode_operation_t;

typedef struct cio_vm_t cio_vm_t;

typedef gen_error_t (*cio_routine_function_t)(cio_vm_t* const restrict);

typedef struct {
    size_t base;
    size_t height;
    size_t execution_offset;
} cio_frame_t;

typedef struct cio_vm_t {
    size_t stack_length;
    size_t* stack;
    size_t frames_used;
    size_t frames_length;
    cio_frame_t* frames;
    size_t callables_length;
    cio_routine_function_t* callables;
    size_t* callables_offsets;
    size_t bytecode_length;
    const size_t* bytecode;
    gen_dylib_t external_lib;
} cio_vm_t;

GEN_ERRORABLE cio_line_from_offset(const size_t offset, size_t* const restrict out_line, const char* const restrict source, const size_t source_length);
GEN_ERRORABLE cio_column_from_offset(const size_t offset, size_t* const restrict out_column, const char* const restrict source, const size_t source_length);
GEN_ERRORABLE cio_resolve_mangled(const char* const restrict identifier, cio_routine_function_t* const out_function, const gen_dylib_t lib);

GEN_ERRORABLE cio_tokenize(const char* const restrict source, const size_t source_length, cio_token_t** const restrict out_tokens, size_t* const restrict out_tokens_length);

GEN_ERRORABLE cio_parse(const cio_token_t* const restrict tokens, const size_t tokens_length, cio_program_t* const restrict out_program, const char* const restrict source, const size_t source_length, const char* const restrict source_file, const size_t source_file_length);
GEN_ERRORABLE cio_free_program(cio_program_t* const restrict program);

GEN_ERRORABLE cio_emit_bytecode(const cio_program_t* const restrict program, unsigned char** const restrict out_bytecode, size_t* const restrict out_bytecode_length, const char* const restrict source, const size_t source_length, const char* const restrict source_file, const size_t source_file_length);

GEN_ERRORABLE cio_vm_initialize_bytecode(const unsigned char* const restrict bytecode, const size_t bytecode_length, const size_t stack_length, cio_vm_t* const restrict out_instance);
GEN_ERRORABLE cio_vm_dispatch_call(cio_vm_t* const restrict vm, const size_t callable, const size_t argc);
GEN_ERRORABLE cio_vm_push_frame(cio_vm_t* const restrict vm);
GEN_ERRORABLE cio_vm_pop_frame(cio_vm_t* const restrict vm);
GEN_ERRORABLE cio_vm_push(cio_vm_t* const restrict vm);
GEN_ERRORABLE cio_vm_execute_routine(cio_vm_t* const restrict vm);
GEN_ERRORABLE cio_vm_dump_stack(const cio_vm_t* const restrict vm);
