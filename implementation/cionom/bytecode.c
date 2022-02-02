// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+genstone@pm.me>

#include "include/cionom.h"

/*
Program Header
===

Symbol Table Header
==
|0 Number of entries|`size_t` width

Symbol Table Entry
==
|0 External|1 Offset of routine into code section|`size_t` width

Code
===

Opcodes
==
All opcodes and operands are the width of `size_t`

Push: 0x0
    Pushes a new entry onto the local stack frame with a value of 0
PushX: 0x1 [imm]
    Pushes a new entry onto the local stack frame with the value of [imm]
Pop: 0x2
    Removes a value from the local stack frame
Call: 0x3 [imm]
    Pushes a stack frame and jumps execution to the routine referred to by [imm]
*/

typedef enum
{
	CIO_BYTECODE_OPERATION_PUSH,
	CIO_BYTECODE_OPERATION_PUSHX,
	CIO_BYTECODE_OPERATION_POP,
	CIO_BYTECODE_OPERATION_CALL
} cio_bytecode_operation_t;

gen_error_t cio_emit_bytecode(const cio_program_t* const restrict program, unsigned char** const restrict out_bytecode, size_t* const restrict out_bytecode_length, const char* const restrict source, __unused const size_t source_length, const char* const restrict source_file, __unused const size_t source_file_length) {
	GEN_FRAME_BEGIN(cio_emit_bytecode);

	GEN_INTERNAL_BASIC_PARAM_CHECK(program);
	GEN_INTERNAL_BASIC_PARAM_CHECK(out_bytecode);
	GEN_INTERNAL_BASIC_PARAM_CHECK(out_bytecode_length);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source_file);

	const size_t program_header_length = sizeof(size_t) + (program->routines_length * sizeof(size_t));
	alignas(size_t) unsigned char* program_header = NULL;
	gen_error_t error = gzalloc_aligned((void**) &program_header, program_header_length, sizeof(unsigned char), alignof(size_t));
	GEN_ERROR_OUT_IF(error, "`gzalloc` failed");

	(*(size_t*) program_header) = program->routines_length;

	alignas(size_t) unsigned char* code = NULL;
	size_t code_length = 0;

	GEN_FOREACH_PTR(i, routine, program->routines_length, program->routines) {
		if(routine->external) {
			*(size_t*) (&program_header[i * sizeof(size_t)]) = 0;
			program_header[i * sizeof(size_t)] |= ((char) 1) << (CHAR_BIT - 1);
			continue;
		}
	}

	*out_bytecode_length = program_header_length + code_length;
	error = gzalloc_aligned((void**) out_bytecode, *out_bytecode_length, sizeof(unsigned char), alignof(size_t));
	GEN_ERROR_OUT_IF(error, "`gzalloc_aligned` failed");
	memcpy(*out_bytecode, program_header, program_header_length);
	GEN_ERROR_OUT_IF_ERRNO(memcpy, errno);
	memcpy(*out_bytecode + program_header_length, code, code_length);
	GEN_ERROR_OUT_IF_ERRNO(memcpy, errno);

	GEN_ALL_OK;
}
