// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

#include "include/cionom.h"

#include <genmemory.h>
#include <genstring.h>

#include <genlog.h>

static void cio_internal_emit_bytecode_cleanup_offsets(uint32_t** offsets) {
    if(!*offsets) return;

    gen_error_t* error = gen_memory_free((void**) offsets);
    if(error) {
        gen_error_print("cionom", error, GEN_ERROR_SEVERITY_FATAL);
        gen_error_abort();
    }
}

static void cio_internal_emit_bytecode_cleanup_code(unsigned char** code) {
    if(!*code) return;

    gen_error_t* error = gen_memory_free((void**) code);
    if(error) {
        gen_error_print("cionom", error, GEN_ERROR_SEVERITY_FATAL);
        gen_error_abort();
    }
}

gen_error_t* cio_emit_bytecode(const cio_program_t* const restrict program, unsigned char** const restrict out_bytecode, size_t* const restrict out_bytecode_length, const char* const restrict source, GEN_UNUSED const size_t source_length, const char* const restrict source_file, GEN_UNUSED const size_t source_file_length) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_emit_bytecode, GEN_FILE_NAME);
	if(error) return error;

	if(!program) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`program` was `NULL`");
	if(!out_bytecode) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`out_bytecode` was `NULL`");
	if(!out_bytecode_length) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`out_bytecode_length` was `NULL`");
	if(!source) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`source` was `NULL`");
	if(!source_file) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`source_file` was `NULL`");

	if(program->routines_length > 0b01111111) return gen_error_attach_backtrace(GEN_ERROR_TOO_LONG, GEN_LINE_NUMBER, "Number of routines exceeds maximum allowed by bytecode format");

	GEN_CLEANUP_FUNCTION(cio_internal_emit_bytecode_cleanup_offsets) uint32_t* offsets = NULL;
	if(program->routines_length) {
		error = gen_memory_allocate_zeroed((void**) &offsets, program->routines_length, sizeof(uint32_t));
		if(error) return error;
	}

	size_t code_size = 0;
	GEN_CLEANUP_FUNCTION(cio_internal_emit_bytecode_cleanup_code) unsigned char* code = NULL;
	// Codegen
	{
		for(size_t i = 0; i < program->routines_length; ++i) {
			const cio_routine_t* const routine = &program->routines[i];
			if(routine->external) continue;
			offsets[i] = (uint32_t) code_size;
			for(size_t j = 0; j < routine->calls_length; ++j) {
				const cio_call_t* const call = &routine->calls[j];
				error = gen_memory_reallocate_zeroed((void**) &code, code_size, code_size + call->parameters_length + 2, sizeof(unsigned char));
				if(error) return error;
				code[code_size] = 0b00000000; // Reserve space
				for(size_t k = 0; k < call->parameters_length; ++k) {
					// Emit push for each param
					if(call->parameters[k] >= UINT8_MAX) return gen_error_attach_backtrace_formatted(GEN_ERROR_TOO_LONG, GEN_LINE_NUMBER, "Value `%uz` exceeds maximum allowed by bytecode format", call->parameters[k]);
					code[code_size + k + 1] = 0b01111111 & call->parameters[k];
				}
				code_size += call->parameters_length + 2;
				size_t called = SIZE_MAX;
				// Locate called routine's index
				for(size_t k = 0; k < program->routines_length; ++k) {
					bool equal = false;
					error = gen_string_compare(call->identifier, GEN_STRING_NO_BOUNDS, program->routines[k].identifier, GEN_STRING_NO_BOUNDS, GEN_STRING_NO_BOUNDS, &equal);
					if(error) return error;

					if(equal) {
						called = k;
						break;
					}
				}
				if(called == SIZE_MAX) return gen_error_attach_backtrace(GEN_ERROR_NO_SUCH_OBJECT, GEN_LINE_NUMBER, "Call to undeclared routine");
				// Emit call
				code[code_size - 1] = 0b10000000 | (unsigned char) called;
			}
			error = gen_memory_reallocate_zeroed((void**) &code, code_size, code_size + 1, sizeof(unsigned char));
			if(error) return error;
			// Emit return
			code[code_size++] = 0b11111111;
		}

		if(code_size > INT32_MAX) return gen_error_attach_backtrace(GEN_ERROR_TOO_LONG, GEN_LINE_NUMBER, "Emitted code size exceeds maximum allowed by bytecode format");
	}

	size_t header_size = 0;
	GEN_CLEANUP_FUNCTION(cio_internal_emit_bytecode_cleanup_code) unsigned char* header = NULL;

	// Header
	{
		error = gen_memory_allocate_zeroed((void**) &header, ++header_size, sizeof(unsigned char));
		if(error) return error;

		header[0] = (unsigned char) program->routines_length;

		for(size_t i = 0; i < program->routines_length; ++i) {
			const cio_routine_t* const routine = &program->routines[i];

            size_t identifier_length = 0;
            error = gen_string_length(routine->identifier, GEN_STRING_NO_BOUNDS, GEN_STRING_NO_BOUNDS, &identifier_length);
            if(error) return error;

            error = gen_memory_reallocate_zeroed((void**) &header, header_size, header_size + 4 + identifier_length + 1, sizeof(unsigned char));
            if(error) return error;

            *(uint32_t*) &header[header_size] = routine->external ? 0xFFFFFFFF : offsets[i];

            for(size_t j = 0; j < identifier_length + 1; ++j) {
                header[header_size + j + 4] = (unsigned char) routine->identifier[j];
            }

            header_size += 4 + identifier_length + 1;
		}
	}

	*out_bytecode_length = header_size + code_size;
	error = gen_memory_allocate_zeroed((void**) out_bytecode, *out_bytecode_length, sizeof(unsigned char));
	if(error) return error;
    GEN_CLEANUP_FUNCTION(cio_internal_emit_bytecode_cleanup_code) unsigned char* bytecode_cleanup = *out_bytecode;

	error = gen_memory_copy(*out_bytecode, *out_bytecode_length, header, header_size, header_size);
	if(error) return error;

	if(code) {
		error = gen_memory_copy(*out_bytecode + header_size, *out_bytecode_length - header_size, code, code_size, code_size);
		if(error) return error;
	}

    bytecode_cleanup = NULL;

	return NULL;
}
