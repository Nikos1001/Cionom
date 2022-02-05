// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+genstone@pm.me>

#include "include/cionom.h"

static __nodiscard gen_error_t cio_internal_parse_expect(const cio_token_t* const restrict token, const cio_token_type_t expected, const char* const restrict source, const size_t source_length, const char* const restrict source_file, __unused const size_t source_file_length) {
	GEN_FRAME_BEGIN(cio_internal_parse_expect);

	GEN_INTERNAL_BASIC_PARAM_CHECK(token);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source_file);

	if(token->type != expected) {
		size_t line = 0;
		size_t column = 0;
		gen_error_t error = cio_line_from_offset(token->offset, &line, source, source_length);
		GEN_ERROR_OUT_IF(error, "`cio_line_from_offset` failed");
		error = cio_column_from_offset(token->offset, &column, source, source_length);
		GEN_ERROR_OUT_IF(error, "`cio_column_from_offset` failed");
		glogf(FATAL, "Malformed program: Unexpected token in %s:%zu:%zu", source_file, line, column);
		GEN_ERROR_OUT(GEN_BAD_CONTENT, "Parsing failed");
	}

	GEN_ALL_OK;
}

gen_error_t cio_parse(const cio_token_t* const restrict tokens, const size_t tokens_length, cio_program_t* const restrict out_program, const char* const restrict source, const size_t source_length, const char* const restrict source_file, const size_t source_file_length) {
	GEN_FRAME_BEGIN(cio_parse);

	GEN_INTERNAL_BASIC_PARAM_CHECK(tokens);
	GEN_INTERNAL_BASIC_PARAM_CHECK(out_program);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source_file);

	gen_error_t error = GEN_OK;

	GEN_FOREACH_PTR(offset, token, tokens_length, tokens) {
		error = cio_internal_parse_expect(token, CIO_TOKEN_IDENTIFIER, source, source_length, source_file, source_file_length);
		GEN_ERROR_OUT_IF(error, "`cio_internal_parse_expect` failed");
		error = grealloc((void**) &out_program->routines, out_program->routines_length, out_program->routines_length + 1, sizeof(cio_routine_t));
		++out_program->routines_length;
		GEN_ERROR_OUT_IF(error, "`grealloc` failed");
		cio_routine_t* const routine = &out_program->routines[out_program->routines_length - 1];
		routine->token = token;
		error = gen_string_duplicate(source + token->offset, source_length - token->offset, token->length, &routine->identifier);
		GEN_ERROR_OUT_IF(error, "`gen_string_duplicate` failed");
		GEN_FOREACH_PTR_ADVANCE(offset, token, tokens_length, tokens, 1);

		error = cio_internal_parse_expect(token, CIO_TOKEN_NUMBER, source, source_length, source_file, source_file_length);
		GEN_ERROR_OUT_IF(error, "`cio_internal_parse_expect` failed");
		error = gen_string_number(source + token->offset, source_length - token->offset, token->length, &routine->parameters);
		GEN_ERROR_OUT_IF(error, "`gen_string_number` failed");

		if(tokens[offset + 1].type != CIO_TOKEN_BLOCK) {
			routine->external = true;
			continue;
		}

		GEN_FOREACH_PTR_ADVANCE(offset, token, tokens_length, tokens, 2);

		while(token->type != CIO_TOKEN_BLOCK) {
			error = cio_internal_parse_expect(token, CIO_TOKEN_IDENTIFIER, source, source_length, source_file, source_file_length);
			GEN_ERROR_OUT_IF(error, "`cio_internal_parse_expect` failed");
			error = grealloc((void**) &routine->calls, routine->calls_length, routine->calls_length + 1, sizeof(cio_call_t));
			++routine->calls_length;
			GEN_ERROR_OUT_IF(error, "`grealloc` failed");
			cio_call_t* const call = &routine->calls[routine->calls_length - 1];
			call->token = token;
			error = gen_string_duplicate(source + token->offset, source_length - token->offset, token->length, &call->identifier);
			GEN_ERROR_OUT_IF(error, "`gen_string_duplicate` failed");
			GEN_FOREACH_PTR_ADVANCE(offset, token, tokens_length, tokens, 1);

			while(token->type == CIO_TOKEN_NUMBER) {
				error = grealloc((void**) &call->parameters, call->parameters_length, call->parameters_length + 1, sizeof(size_t));
				++call->parameters_length;
				GEN_ERROR_OUT_IF(error, "`grealloc` failed");
				error = gen_string_number(source + token->offset, source_length - token->offset, token->length, &call->parameters[call->parameters_length - 1]);
				GEN_ERROR_OUT_IF(error, "`gen_string_number` failed");
				GEN_FOREACH_PTR_ADVANCE(offset, token, tokens_length, tokens, 1);
			}
		}
	}

	GEN_ALL_OK;
}

#define gfree_if(ptr) \
	do { \
		if(ptr) { \
			error = gfree(ptr); \
			GEN_ERROR_OUT_IF(error, "`gfree` failed"); \
			ptr = NULL; \
		} \
	} while(0)

gen_error_t cio_free_program(cio_program_t* const restrict program) {
	GEN_FRAME_BEGIN(cio_free_program);

	GEN_INTERNAL_BASIC_PARAM_CHECK(program);

	gen_error_t error = GEN_OK;

	GEN_FOREACH_PTR(i, routine, program->routines_length, program->routines) {
		gfree_if(routine->identifier);
		GEN_FOREACH_PTR(j, call, routine->calls_length, routine->calls) {
			gfree_if(call->identifier);
			gfree_if(call->parameters);
		}
		gfree_if(routine->calls);
	}
	gfree_if(program->routines);
	program->routines_length = 0;

	GEN_ALL_OK;
}
