// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+cionom@pm.me>

#include "include/cionom.h"

gen_error_t cio_emit_bytecode(const cio_program_t* const restrict program, unsigned char** const restrict out_bytecode, size_t* const restrict out_bytecode_length, const char* const restrict source, __unused const size_t source_length, const char* const restrict source_file, __unused const size_t source_file_length) {
	GEN_FRAME_BEGIN(cio_emit_bytecode);

	GEN_NULL_CHECK(program);
	GEN_NULL_CHECK(out_bytecode);
	GEN_NULL_CHECK(out_bytecode_length);
	GEN_NULL_CHECK(source);
	GEN_NULL_CHECK(source_file);

	if(program->routines_length >= UINT8_MAX) GEN_ERROR_OUT(GEN_TOO_LONG, "Number of routines exceeds maximum allowed by bytecode format");

	gen_error_t error = GEN_OK;

	uint32_t* offsets = NULL;
	if(program->routines_length) {
		error = gzalloc((void**) &offsets, program->routines_length, sizeof(uint32_t));
		GEN_ERROR_OUT_IF(error, "`gzalloc` failed");
	}

	size_t code_size = 0;
	unsigned char* code = NULL;
	// Codegen
	{
		for(size_t i = 0; i < program->routines_length; ++i) {
			const cio_routine_t* const routine = &program->routines[i];
			if(routine->external) continue;
			offsets[i] = (uint32_t) code_size;
			for(size_t j = 0; j < routine->calls_length; ++j) {
				const cio_call_t* const call = &routine->calls[j];
				error = grealloc((void**) &code, code_size, code_size + call->parameters_length + 2, sizeof(unsigned char));
				GEN_ERROR_OUT_IF(error, "`grealloc` failed");
				code[code_size] = 0b00000000; // Reserve space
				for(size_t k = 0; k < call->parameters_length; ++k) {
					// Emit push for each param
					if(call->parameters[k] >= UINT8_MAX) GEN_ERROR_OUT(GEN_TOO_LONG, "Value exceeds maximum allowed by bytecode format");
					code[code_size + k + 1] = 0b01111111 & call->parameters[k];
				}
				code_size += call->parameters_length + 2;
				size_t called = SIZE_MAX;
				// Locate called routine's index
				for(size_t k = 0; k < program->routines_length; ++k) {
					bool equal = false;
					error = gen_string_compare(call->identifier, GEN_STRING_NO_BOUND, program->routines[k].identifier, GEN_STRING_NO_BOUND, GEN_STRING_NO_BOUND, &equal);
					GEN_ERROR_OUT_IF(error, "`gen_string_compare` failed");

					if(equal) {
						called = k;
						break;
					}
				}
				if(called == SIZE_MAX) GEN_ERROR_OUT(GEN_NO_SUCH_OBJECT, "Call to undeclared routine");
				// Emit call
				code[code_size - 1] = 0b10000000 | (unsigned char) called;
			}
			error = grealloc((void**) &code, code_size, code_size + 1, sizeof(unsigned char));
			GEN_ERROR_OUT_IF(error, "`grealloc` failed");
			// Emit return
			code[code_size++] = 0b11111111;
		}

		if(code_size > INT32_MAX) GEN_ERROR_OUT(GEN_TOO_LONG, "Emitted code size exceeds maximum allowed by bytecode format");
	}

	size_t header_size = 0;
	unsigned char* header = NULL;

	// Header
	{
		error = gzalloc((void**) &header, ++header_size, sizeof(unsigned char));
		GEN_ERROR_OUT_IF(error, "`gzalloc` failed");

		header[0] = (unsigned char) program->routines_length;

		for(size_t i = 0; i < program->routines_length; ++i) {
			const cio_routine_t* const routine = &program->routines[i];
			if(routine->external) {
				size_t identifier_length = 0;
				error = gen_string_length(routine->identifier, GEN_STRING_NO_BOUND, GEN_STRING_NO_BOUND, &identifier_length);
				GEN_ERROR_OUT_IF(error, "`gen_string_length` failed");

				error = grealloc((void**) &header, header_size, header_size + 4 + identifier_length + 1, sizeof(unsigned char));
				GEN_ERROR_OUT_IF(error, "`grealloc` failed");

				*(uint32_t*) &header[header_size] = 0xFFFFFFFF;

				for(size_t j = 0; j < identifier_length + 1; ++j) {
					header[header_size + j + 4] = (unsigned char) routine->identifier[j];
				}

				header_size += 4 + identifier_length + 1;
			}
			else {
				error = grealloc((void**) &header, header_size, header_size + 4, sizeof(unsigned char));
				GEN_ERROR_OUT_IF(error, "`grealloc` failed");

				*(uint32_t*) &header[header_size] = offsets[i];

				header_size += 4;
			}
		}
	}

	*out_bytecode_length = header_size + code_size;
	error = gzalloc((void**) out_bytecode, *out_bytecode_length, sizeof(unsigned char));
	GEN_ERROR_OUT_IF(error, "`gzalloc` failed");

	error = gen_memory_copy(header, header_size, *out_bytecode, *out_bytecode_length, header_size);
	GEN_ERROR_OUT_IF(error, "`gen_memory_copy` failed");

	if(code) {
		error = gen_memory_copy(code, code_size, *out_bytecode + header_size, *out_bytecode_length - header_size, code_size);
		GEN_ERROR_OUT_IF(error, "`gen_memory_copy` failed");
	}

	if(offsets) {
		error = gfree(offsets);
		GEN_ERROR_OUT_IF(error, "`gfree` failed");
	}

	GEN_ALL_OK;
}
