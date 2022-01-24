// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+genstone@pm.me>

#include "include/cionom.h"

typedef unsigned cio_internal_sequence_t;

static __nodiscard gen_error_t cio_internal_parse_error(const cio_token_t* const restrict token, const char* const restrict message, const char* const restrict source, const size_t source_length, const char* const restrict source_file, __unused const size_t source_file_length) {
	GEN_FRAME_BEGIN(cio_internal_parse_error);

	GEN_INTERNAL_BASIC_PARAM_CHECK(token);
	GEN_INTERNAL_BASIC_PARAM_CHECK(message);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source_file);

	size_t line = 0;
	size_t column = 0;
	gen_error_t error = cio_line_from_offset(token->offset, &line, source, source_length);
	GEN_ERROR_OUT_IF(error, "`cio_line_from_offset` failed");
	error = cio_column_from_offset(token->offset, &column, source, source_length);
	GEN_ERROR_OUT_IF(error, "`cio_column_from_offset` failed");
	glogf(FATAL, "Malformed program: %s %s:%zu:%zu", message, source_file, line, column);

	GEN_ALL_OK;
}

static __nodiscard gen_error_t cio_internal_validate_sequence(const cio_token_t* const restrict tokens, const size_t tokens_length, const size_t first, const cio_internal_sequence_t* const restrict sequence, const size_t sequence_length, const char* const restrict source, const size_t source_length, const char* const restrict source_file, const size_t source_file_length) {
	GEN_FRAME_BEGIN(cio_internal_validate_sequence);

	GEN_INTERNAL_BASIC_PARAM_CHECK(tokens);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source_file);
	GEN_INTERNAL_BASIC_PARAM_CHECK(sequence);

	gen_error_t error = GEN_OK;

	if(first >= tokens_length) GEN_ERROR_OUT(GEN_TOO_SHORT, "`first` is beyond the bounds of `tokens`");

	if(tokens_length - first < sequence_length) {
		error = cio_internal_parse_error(&tokens[first], "Unexpected EOF", source, source_length, source_file, source_file_length);
		GEN_ERROR_OUT_IF(error, "`cio_internal_parse_error` failed");
		GEN_ERROR_OUT(GEN_BAD_CONTENT, "Parsing failed");
	}

	GEN_FOREACH_PTR(i, token, sequence_length, &tokens[first]) {
		if(!(token->type & sequence[i])) {
			error = cio_internal_parse_error(&tokens[first], "Unexpected token", source, source_length, source_file, source_file_length);
			GEN_ERROR_OUT_IF(error, "`cio_internal_parse_error` failed");
			GEN_ERROR_OUT(GEN_BAD_CONTENT, "Parsing failed");
		}
	}

	GEN_ALL_OK;
}

static const cio_internal_sequence_t identifier_sequence[] = {CIO_TOKEN_IDENTIFIER};
static const cio_internal_sequence_t storage_sequence[] = {CIO_TOKEN_STORAGE, CIO_TOKEN_SPECIFIER_EXPRESSION_START, CIO_TOKEN_NUMBER | CIO_TOKEN_IDENTIFIER | CIO_TOKEN_STRING, CIO_TOKEN_SPECIFIER_EXPRESSION_END};

gen_error_t cio_parse(const cio_token_t* const restrict tokens, const size_t tokens_length, cio_program_t* const restrict out_program, const char* const restrict source, const size_t source_length, const char* const restrict source_file, const size_t source_file_length) {
	GEN_FRAME_BEGIN(cio_parse);

	GEN_INTERNAL_BASIC_PARAM_CHECK(tokens);
	GEN_INTERNAL_BASIC_PARAM_CHECK(out_program);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source_file);

	gen_error_t error = GEN_OK;

	size_t offset = 0;

	GEN_FOREACH_PTR(i, token, tokens_length, tokens) {
		(void) token;

		error = cio_internal_validate_sequence(tokens, tokens_length, offset, identifier_sequence, sizeof(identifier_sequence) / sizeof(identifier_sequence[0]), source, source_length, source_file, source_file_length);
		GEN_ERROR_OUT_IF(error, "`cio_internal_validate_sequence` failed");

		error = cio_internal_validate_sequence(tokens, tokens_length, offset, storage_sequence, sizeof(storage_sequence) / sizeof(storage_sequence[0]), source, source_length, source_file, source_file_length);
		GEN_ERROR_OUT_IF(error, "`cio_internal_validate_sequence` failed");
	}

	GEN_ALL_OK;
}
