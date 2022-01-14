// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+genstone@pm.me>

#include "include/cionom.h"

#include <genstring.h>

static __nodiscard gen_error_t cio_internal_parse_error(const cio_token_t* const restrict token, const char* const restrict source, const char* const restrict source_file, const char* const restrict message) {
	GEN_FRAME_BEGIN(cio_internal_parse_error);

	GEN_INTERNAL_BASIC_PARAM_CHECK(token);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source_file);
	GEN_INTERNAL_BASIC_PARAM_CHECK(message);

	size_t line = 0;
	size_t column = 0;
	gen_error_t error = cio_line_from_offset(token->offset, &line, source);
	GEN_ERROR_OUT_IF(error, "`cio_line_from_offset` failed");
	error = cio_column_from_offset(token->offset, &column, source);
	GEN_ERROR_OUT_IF(error, "`cio_column_from_offset` failed");
	glogf(FATAL, "Malformed program: %s %s:%zu:%zu", message, source_file, line, column);

	GEN_ALL_OK;
}

static __nodiscard gen_error_t cio_internal_parse_storage(const cio_token_t* const restrict tokens, const size_t n_tokens, const size_t offset, cio_storage_t* const restrict out_storage, const char* const restrict source, const char* const restrict source_file, size_t* const restrict out_offset, const cio_routine_t* const restrict owner) {
	GEN_FRAME_BEGIN(cio_internal_parse_storage);

	GEN_INTERNAL_BASIC_PARAM_CHECK(tokens);
	GEN_INTERNAL_BASIC_PARAM_CHECK(out_storage);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source_file);
	GEN_INTERNAL_BASIC_PARAM_CHECK(out_offset);

	gen_error_t error = GEN_OK;

	size_t source_len = 0;
	error = gen_string_length(source, GEN_STRING_NO_BOUND, GEN_STRING_NO_BOUND, &source_len);
	GEN_ERROR_OUT_IF(error, "`gen_string_length` failed");

	*out_offset = offset;

	if(tokens[*out_offset].type != CIO_TOKEN_STORAGE) {
		error = cio_internal_parse_error(&tokens[offset], source, source_file, "Unexpected token - Expected a storage declaration");
		GEN_ERROR_OUT_IF(error, "`cio_internal_parse_error` failed");
		GEN_ERROR_OUT(GEN_BAD_CONTENT, "Parsing failed");
	}

	if(tokens[++*out_offset].type != CIO_TOKEN_SPECIFIER_EXPRESSION_START) {
		if(*out_offset >= n_tokens) {
			error = cio_internal_parse_error(&tokens[offset], source, source_file, "Unexpected EOF while parsing storage declaration");
			GEN_ERROR_OUT_IF(error, "`cio_internal_parse_error` failed");
			GEN_ERROR_OUT(GEN_BAD_CONTENT, "Parsing failed");
		}
		error = cio_internal_parse_error(&tokens[offset], source, source_file, "Unexpected token - Expected a specifier expression");
		GEN_ERROR_OUT_IF(error, "`cio_internal_parse_error` failed");
		GEN_ERROR_OUT(GEN_BAD_CONTENT, "Parsing failed");
	}
	
	if(tokens[++*out_offset].type != CIO_TOKEN_IDENTIFIER || tokens[*out_offset].type != CIO_TOKEN_NUMBER) {
		if(*out_offset >= n_tokens) {
			error = cio_internal_parse_error(&tokens[offset], source, source_file, "Unexpected EOF while parsing storage declaration");
			GEN_ERROR_OUT_IF(error, "`cio_internal_parse_error` failed");
			GEN_ERROR_OUT(GEN_BAD_CONTENT, "Parsing failed");
		}
		error = cio_internal_parse_error(&tokens[offset], source, source_file, "Unexpected token - Expected a specifier expression");
		GEN_ERROR_OUT_IF(error, "`cio_internal_parse_error` failed");
		GEN_ERROR_OUT(GEN_BAD_CONTENT, "Parsing failed");
	}

	if(tokens[*out_offset].type == CIO_TOKEN_NUMBER) {
		out_storage->size_type = CIO_SIZE_CONSTANT;
		char* temp = NULL;
		error = gen_string_duplicate(source + tokens[*out_offset].offset, source_len - tokens[*out_offset].offset, tokens[*out_offset].length, &temp);
		GEN_ERROR_OUT_IF(error, "`gen_string_duplicate` failed");
		out_storage->size.constant = strtoull(temp, NULL, 10);
		GEN_ERROR_OUT_IF_ERRNO(strtoull, errno);
		error = gfree(temp);
		GEN_ERROR_OUT_IF(error, "`gfree` failed");		
	}
	else if(tokens[*out_offset].type == CIO_TOKEN_IDENTIFIER) {
		out_storage->size_type = CIO_SIZE_VARIABLE;
		out_storage->size.variable = NULL;
		GEN_FOREACH_PTR(i, statement, owner->n_statements, owner->statements) {
			if(statement->type == CIO_STATEMENT_STORAGE) {
				bool equal = false;
				error = gen_string_compare(statement->storage.identifier, GEN_STRING_NO_BOUND, source + tokens[*out_offset].offset, source_len - tokens[*out_offset].offset, tokens[*out_offset].length, &equal);
				GEN_ERROR_OUT_IF(error, "`gen_string_compare` failed");

				if(equal) {
					out_storage->size.variable = &statement->storage;
					break;
				}
			}
		}
		if(!out_storage->size.variable) {
			error = cio_internal_parse_error(&tokens[offset], source, source_file, "Unrecognized identifier in specifier expression");
			GEN_ERROR_OUT_IF(error, "`cio_internal_parse_error` failed");
			GEN_ERROR_OUT(GEN_BAD_CONTENT, "Parsing failed");
		}
	}

	GEN_ALL_OK;
}

gen_error_t cio_parse(const cio_token_t* const restrict tokens, const size_t n_tokens, const char* const restrict source, const char* const restrict source_file, cio_program_t* const restrict out_program) {
	GEN_FRAME_BEGIN(cio_parse);

	GEN_INTERNAL_BASIC_PARAM_CHECK(tokens);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source_file);
	GEN_INTERNAL_BASIC_PARAM_CHECK(out_program);

	gen_error_t error = GEN_OK;

	size_t source_len = 0;
	error = gen_string_length(source, GEN_STRING_NO_BOUND, GEN_STRING_NO_BOUND, &source_len);
	GEN_ERROR_OUT_IF(error, "`gen_string_length` failed");

	error = gzalloc((void**) out_program, 1, sizeof(cio_program_t));
	GEN_ERROR_OUT_IF(error, "`gzalloc` failed");


	GEN_FOREACH_PTR(offset, token, n_tokens, tokens) {
		if(token->type != CIO_TOKEN_IDENTIFIER) {
			error = cio_internal_parse_error(token, source, source_file, "Unexpected token - Expected a routine declaration");
			GEN_ERROR_OUT_IF(error, "`cio_internal_parse_error` failed");
			GEN_ERROR_OUT(GEN_BAD_CONTENT, "Parsing failed");
		}
		else {
			error = grealloc((void**) out_program->routines, ++out_program->n_routines, sizeof(cio_routine_t));
			GEN_ERROR_OUT_IF(error, "`grealloc` failed");

			error = gen_string_duplicate(source + token->offset, source_len - token->offset, token->length, &out_program->routines[out_program->n_routines - 1].identifier);
			GEN_ERROR_OUT_IF(error, "`gen_string_duplicate` failed");

			while(token->type != CIO_TOKEN_BLOCK_START && token->type != CIO_TOKEN_STATEMENT_DELIMITER) {
				GEN_FOREACH_PTR_NEXT(offset, token, n_tokens, tokens);

				if(token->type != CIO_TOKEN_STORAGE) {
					error = cio_internal_parse_error(token, source, source_file, "Unexpected token - Expected a parameter storage declaration");
					GEN_ERROR_OUT_IF(error, "`cio_internal_parse_error` failed");
					GEN_ERROR_OUT(GEN_BAD_CONTENT, "Parsing failed");
				}
				else {
					error = grealloc((void**) out_program->routines[out_program->n_routines - 1].parameters, ++out_program->routines[out_program->n_routines - 1].n_parameters, sizeof(cio_storage_t));
					GEN_ERROR_OUT_IF(error, "`grealloc` failed");
					
					size_t
					error = cio_internal_parse_storage(tokens, n_tokens, offset, &out_program->routines[out_program->n_routines - 1].parameters[out_program->routines[out_program->n_routines - 1].n_parameters - 1], source, source_file, )
				}
			}
		}
	}

	GEN_ALL_OK;
}
