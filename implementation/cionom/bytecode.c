// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+genstone@pm.me>

#include "include/cionom.h"

/*
Program Header
===

Symbol Table Header
==
|0 Number of entries|64

Symbol Table Entry
==
|0 External |1 Offset of routine|64

Code
===

Opcodes
==
All opcodes and operands are 64 bits in width

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

gen_error_t cio_emit_bytecode(const cio_program_t* const restrict program, uint8_t** const restrict out_bytecode, size_t* const restrict out_bytecode_length, const char* const restrict source, __unused const size_t source_length, const char* const restrict source_file, __unused const size_t source_file_length) {
	GEN_DIAG_REGION_BEGIN
#pragma clang diagnostic ignored "-Wcast-align"

	GEN_FRAME_BEGIN(cio_emit_bytecode);

	GEN_INTERNAL_BASIC_PARAM_CHECK(program);
	GEN_INTERNAL_BASIC_PARAM_CHECK(out_bytecode);
	GEN_INTERNAL_BASIC_PARAM_CHECK(out_bytecode_length);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source_file);

	// https://stackoverflow.com/questions/28516413/c11-alignas-vs-clang-wcast-align
	// Clang fails to check alignment properly when delivering -Wcast-align

	const size_t program_header_length = sizeof(size_t) + (program->routines_length * sizeof(size_t));
	alignas(size_t) uint8_t* program_header = NULL;
	gen_error_t error = gzalloc_aligned((void**) &program_header, program_header_length, sizeof(uint8_t), alignof(size_t));
	GEN_ERROR_OUT_IF(error, "`gzalloc` failed");

	(*(size_t*) program_header) = program->routines_length;

	__unused alignas(size_t) uint8_t* code = NULL;

	GEN_FOREACH_PTR(i, routine, program->routines_length, program->routines) {
		if(routine->external) {
			*(size_t*) (&program_header[i * sizeof(size_t)]) = 0;
			program_header[i * sizeof(size_t)] |= 1 << (CHAR_BIT - 1);
			continue;
		}
	}

	GEN_ALL_OK;

	GEN_DIAG_REGION_END
}
