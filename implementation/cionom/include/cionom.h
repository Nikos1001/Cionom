// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+cionom@pm.me>

/**
 * @file cionom.h
 * Contains the public facing Cíonom API.
 * This is the only header required for processing source, emitting bytecode and executing bytecode.
 */

#ifndef CIONOM_H
#define CIONOM_H

#include <gencommon.h>
#include <genstring.h>
#include <gendynamiclibrary.h>

/**
 * The type of a source token.
 */
typedef enum {
    /**
     * A routine identifier.
     */
    CIO_TOKEN_IDENTIFIER,
    /**
     * A block delimiter `:`.
     */
    CIO_TOKEN_BLOCK,
    /**
     * A numeric literal.
     */
    CIO_TOKEN_NUMBER
} cio_token_type_t;

/**
 * A source token.
 */
typedef struct {
    /**
     * The type of this token.
     */
    cio_token_type_t type;
    /**
     * The offset into the source buffer at which this token begins.
     * This value is intended to be an offset into the source buffer provided to `cio_emit_bytecode`.
     */
    size_t offset;
    /**
     * The number of characters in the source which this token occupies.
     */
    size_t length;
} cio_token_t;

/**
 * A call node in the program representation.
 */
typedef struct {
    /**
     * The identifier of the routine to call.
     */
    char* identifier;
    /**
     * The number of parameters to this call.
     */
    size_t parameters_length;
    /**
     * The parameters to this call - a list of numeric constants.
     */
    size_t* parameters;

    /**
     * The token this call was constructed from.
     * May be invalid if the token buffer has been freed.
     */
    const cio_token_t* token;
} cio_call_t;

/**
 * A routine node in the program representation.
 */
typedef struct {
    /**
     * The identifier of the routine.
     */
    char* identifier;
    /**
     * The number of parameters to the routine.
     */
    size_t parameters;
    /**
     * The length of the routine's body
     */
    size_t calls_length;
    /**
     * The routine's body.
     */
    cio_call_t* calls;
    /**
     * Whether this routine should be resolved externally
     */
    bool external;

    /**
     * The token this routine was constructed from.
     * May be invalid if the token buffer has been freed.
     */
    const cio_token_t* token;
} cio_routine_t;

/**
 * The program representation.
 */
typedef struct {
    /**
     * The number of routines in the program.
     */
    size_t routines_length;
    /**
     * The program's routines.
     */
    cio_routine_t* routines;
} cio_program_t;

typedef struct cio_vm_t cio_vm_t;

/**
 * The underlying function to be called for a routine.
 */
typedef gen_error_t (*cio_routine_function_t)(cio_vm_t* const restrict);

/**
 * A call frame in the VM.
 */
typedef struct {
    /**
     * The offset of the call frame.
     */
    size_t base;
    /**
     * The height of the call frame.
     */
    size_t height;
    /**
     * The current point of execution in the bytecode for the call frame.
     */
    size_t execution_offset;
} cio_frame_t;

/**
 * The VM state.
 */
typedef struct cio_vm_t {
    /**
     * The length of the stack.
     */
    size_t stack_length;
    /**
     * The stack - consists of a buffer of `size_t`s.
     */
    size_t* stack;

    /**
     * The number of call frames currently in use.
     */
    size_t frames_used;
    /**
     * The number of call frames available to use.
     */
    size_t frames_length;
    /**
     * The call frames.
     */
    cio_frame_t* frames;

    /**
     * The number of callables.
     */
    size_t callables_length;
    /**
     * The underlying function for the callables.
     */
    cio_routine_function_t* callables;
    /**
     * The offsets of the callables.
     */
    size_t* callables_offsets;

    /**
     * The length of the bytecode to execute.
     */
    size_t bytecode_length;
    /**
     * The bytecode to execute.
     */
    const unsigned char* bytecode;

    /**
     * The library handle from which to load externally resolved routines.
     */
    gen_dylib_t external_lib;
} cio_vm_t;

/**
 * Gets the line number from an offset into a source buffer.
 * @param[in] offset the offset to get line number from.
 * @param[out] out_line a pointer to storage for the line number.
 * @param[in] source the source buffer.
 * @param[in] source_length the length of the source buffer.
 * @return an error code.
 */
GEN_ERRORABLE cio_line_from_offset(const size_t offset, size_t* const restrict out_line, const char* const restrict source, const size_t source_length);
/**
 * Gets the column from an offset into a source buffer.
 * @param[in] offset the offset to get column from.
 * @param[out] out_column a pointer to storage for the column.
 * @param[in] source the source buffer.
 * @param[in] source_length the length of the source buffer.
 * @return an error code.
 */
GEN_ERRORABLE cio_column_from_offset(const size_t offset, size_t* const restrict out_column, const char* const restrict source, const size_t source_length);
/**
 * Mangles an identifier to contain only `A-Za-z0-9_`
 * @param[in] identifier the identifier to mangle.
 * @param[out] out_mangled a pointer to storage for a pointer to the mangled identifier buffer. Must be freed.
 * @return an error code.
 */
GEN_ERRORABLE cio_mangle_identifier(const char* const restrict identifier, char** const restrict out_mangled);
/**
 * Resolves an external routine identifier to native code.
 * @param[in] identifier the identifier to resolve.
 * @param[out] out_function a pointer to storage for the underlying function.
 * @param[in] lib the library to resolve the identifier from.
 * @return an error code. 
 */
GEN_ERRORABLE cio_resolve_external(const char* const restrict identifier, cio_routine_function_t* const out_function, const gen_dylib_t lib);

/**
 * Generates a token buffer from a source buffer.
 * @param[in] source the source buffer to tokenize.
 * @param[in] source_length the length of the source buffer to tokenize.
 * @param[out] out_tokens a pointer to storage for a pointer to the token buffer. Must be freed.
 * @param[out] out_tokens_length a pointer to storage for the length of the token buffer.
 * @return an error code.
 */
GEN_ERRORABLE cio_tokenize(const char* const restrict source, const size_t source_length, cio_token_t** const restrict out_tokens, size_t* const restrict out_tokens_length);

/**
 * Parses a token buffer into a program representation.
 * @param[in] tokens the token buffer to parse.
 * @param[in] tokens_length the length of the token buffer to parse.
 * @param[out] out_program a pointer to storage for the program representation. Must freed with `cio_free_program`.
 * @param[in] source the source buffer from which the token buffer was tokenized.
 * @param[in] source_length the length of the source buffer from which the token buffer was tokenized.
 * @param[in] source_file file name from which the source buffer was read.
 * @param[in] source_file_length the length of the file name from which the source buffer was read.
 * @return an error code.
 */
GEN_ERRORABLE cio_parse(const cio_token_t* const restrict tokens, const size_t tokens_length, cio_program_t* const restrict out_program, const char* const restrict source, const size_t source_length, const char* const restrict source_file, const size_t source_file_length);
/**
 * Properly cleans up and frees the contents of a program representation.
 * Does not free the program representation container itself as allocation is done by the user.
 * @param[in,out] program the program to free.
 * @return an error code.
 */
GEN_ERRORABLE cio_free_program(cio_program_t* const restrict program);

/**
 * Emits executable bytecode for a program representation.
 * @param[in] program the program to consume.
 * @param[out] out_bytecode a pointer to storage for a pointer to the emitted bytecode buffer. Must be freed.
 * @param[out] out_bytecode_length a pointer to storage for the length of the emitted bytecode buffer.
 * @param[in] source the source buffer from which the program representation was derived.
 * @param[in] source_length the length of the source buffer from which the program representation was derived.
 * @param[in] source_file file name from which the source buffer was read.
 * @param[in] source_file_length the length of the file name from which the source buffer was read.
 * @return an error code.
 */
GEN_ERRORABLE cio_emit_bytecode(const cio_program_t* const restrict program, unsigned char** const restrict out_bytecode, size_t* const restrict out_bytecode_length, const char* const restrict source, const size_t source_length, const char* const restrict source_file, const size_t source_file_length);

/**
 * Creates and initializes a VM to execute a bytecode buffer.
 * @param[in] bytecode the bytecode buffer to execute.
 * @param[in] bytecode_length the length of the bytecode buffer to execute.
 * @param[in] stack_length the length of the stack to execute with.
 * @param[out] out_instance a pointer to storage for the created VM.
 * @return an error code.
 */
GEN_ERRORABLE cio_vm_initialize_bytecode(const unsigned char* const restrict bytecode, const size_t bytecode_length, const size_t stack_length, cio_vm_t* const restrict out_instance);

/**
 * Destroys a VM.
 * @param[out] instance the VM instance to destroy.
 * @return an error code.
 */
GEN_ERRORABLE cio_free_vm(const cio_vm_t* const restrict instance);

/**
 * Dispatches a call to a callable in a VM.
 * @param[in,out] vm the VM to call in.
 * @param[in] callable the index of the callable to call.
 * @param[in] argc the number of unframed stack elements to take when constructing the callee stack frame.
 * @return an error code. 
 */
GEN_ERRORABLE cio_vm_dispatch_call(cio_vm_t* const restrict vm, const size_t callable, const size_t argc);
/**
 * Pushes a new stack frame in a VM.
 * @param[in,out] vm the VM to push a new stack frame in.
 * @return an error code.
 */
GEN_ERRORABLE cio_vm_push_frame(cio_vm_t* const restrict vm);
/**
 * Pops the last stack frame in a VM.
 * @param[in,out] vm the VM to pop a stack frame from.
 * @return an error code.
 */
GEN_ERRORABLE cio_vm_pop_frame(cio_vm_t* const restrict vm);
/**
 * Pushes a value into the last stack frame in a VM.
 * @param[in,out] vm the VM to push a value into the last frame of.
 * @return an error code.
 */
GEN_ERRORABLE cio_vm_push(cio_vm_t* const restrict vm);
/**
 * Gets a stack frame.
 * @param[in] vm the VM containing the stack frame.
 * @param[in] frame_offset the offset of the stack frame to get back from the current stack frame.
 * @param[out] out_pointer pointer to storage for a pointer to the stack frame.
 * @return an error code.
 */
GEN_ERRORABLE cio_vm_get_frame(const cio_vm_t* const restrict vm, const size_t frame_offset, const cio_frame_t** const restrict out_pointer);
/**
 * Gets a pointer into the stack for a stack frame.
 * @param[in] vm the VM containing the stack.
 * @param[in] frame the stack frame to get a pointer for.
 * @param[out] out_pointer pointer to storage for a pointer to the beginning of the stack frame within the stack.
 * @return an error code.
 */
GEN_ERRORABLE cio_vm_get_frame_pointer(const cio_vm_t* const restrict vm, const cio_frame_t* const restrict frame, size_t** const restrict out_pointer);
/**
 * Dumps the stack state of a VM for diagnostic purposes.
 * @param[in] vm the VM whose stack should be dumped.
 * @return an error code.
 */
GEN_ERRORABLE cio_vm_dump_stack(const cio_vm_t* const restrict vm);

#endif
