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

Push: 0x0 [imm]
    Pushes a new entry onto the local stack frame with the value of [imm]
Pop: 0x1
    Removes a value from the local stack frame
Call: 0x2 [imm]
    Pushes a stack frame and jumps execution to the routine referred to by [imm]
*/

/*
Generation
===

`foo 3, 4, 0`

Where `foo` is index 0 in the symbol table would become the following bytecode:

```
push 3
push 4
push 0
call 0
pop
pop
pop
```
*/

typedef enum
{
	CIO_BYTECODE_OPERATION_PUSH = 0xAA,
	CIO_BYTECODE_OPERATION_CALL = 0x55
} cio_bytecode_operation_t;

gen_error_t cio_emit_bytecode(const cio_program_t* const restrict program, unsigned char** const restrict out_bytecode, size_t* const restrict out_bytecode_length, const char* const restrict source, const size_t source_length, const char* const restrict source_file, __unused const size_t source_file_length) {
	GEN_FRAME_BEGIN(cio_emit_bytecode);

	GEN_INTERNAL_BASIC_PARAM_CHECK(program);
	GEN_INTERNAL_BASIC_PARAM_CHECK(out_bytecode);
	GEN_INTERNAL_BASIC_PARAM_CHECK(out_bytecode_length);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source_file);

	const size_t program_header_length = program->routines_length + 1;
	size_t bytecode_length = program_header_length;
	size_t* bytecode = NULL;
	bytecode = NULL;
	gen_error_t error = gzalloc((void**) &bytecode, bytecode_length, sizeof(size_t));
	GEN_ERROR_OUT_IF(error, "`gzalloc` failed");

	bytecode[0] = program->routines_length;

	GEN_FOREACH_PTR(i, routine, program->routines_length, program->routines) {
		if(routine->external) {
			bytecode[i + 1] = SIZE_MAX;
			continue;
		}
		bytecode[i + 1] = bytecode_length;

		size_t stride = routine->calls_length * 2;
		GEN_FOREACH_PTR(j, stride_check_call, routine->calls_length, routine->calls) {
			stride += (stride_check_call->parameters_length * 2);
		}
		error = grealloc((void**) &bytecode, bytecode_length, bytecode_length + stride, sizeof(size_t));
		GEN_ERROR_OUT_IF(error, "`grealloc` failed");

		size_t insertion_offset = bytecode_length;
		GEN_FOREACH_PTR(j, call, routine->calls_length, routine->calls) {
			GEN_FOREACH_PTR(k, parameter, call->parameters_length, call->parameters) {
				bytecode[insertion_offset++] = CIO_BYTECODE_OPERATION_PUSH;
				bytecode[insertion_offset++] = *parameter;
			}
			bytecode[insertion_offset++] = CIO_BYTECODE_OPERATION_CALL;

			bool equal = false;
			size_t called_index = 0;
			GEN_FOREACH_PTR(k, called_routine, program->routines_length, program->routines) {
				error = gen_string_compare(call->identifier, GEN_STRING_NO_BOUND, called_routine->identifier, GEN_STRING_NO_BOUND, GEN_STRING_NO_BOUND, &equal);
				GEN_ERROR_OUT_IF(error, "`gen_string_compare` failed");
				if(equal) {
					called_index = k;
					break;
				}
			}
			if(!equal) {
				size_t line = 0;
				size_t column = 0;
				error = cio_line_from_offset(call->token->offset, &line, source, source_length);
				GEN_ERROR_OUT_IF(error, "`cio_line_from_offset` failed");
				error = cio_column_from_offset(call->token->offset, &column, source, source_length);
				GEN_ERROR_OUT_IF(error, "`cio_column_from_offset` failed");
				glogf(FATAL, "Invalid AST: Unknown routine identifier %s in %s:%zu:%zu", call->identifier, source_file, line, column);
				GEN_ERROR_OUT(GEN_BAD_CONTENT, "Bytecode generation failed");
			}
			bytecode[insertion_offset++] = called_index;
		}
		bytecode_length += stride;
	}

	*out_bytecode_length = bytecode_length * sizeof(size_t);
	*out_bytecode = (unsigned char*) bytecode;

	GEN_ALL_OK;
}
