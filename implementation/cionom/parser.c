// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+genstone@pm.me>

#include "include/cionom.h"

typedef unsigned cio_internal_sequence_t;

static const cio_internal_sequence_t cio_internal_token_identifier_sequence[] = {CIO_TOKEN_IDENTIFIER};
// static const cio_internal_sequence_t cio_internal_token_return_sequence[] = {CIO_TOKEN_RETURN};
// static const cio_internal_sequence_t cio_internal_token_storage_sequence[] = {CIO_TOKEN_STORAGE};
// static const cio_internal_sequence_t cio_internal_token_alignment_sequence[] = {CIO_TOKEN_ALIGNMENT};
// static const cio_internal_sequence_t cio_internal_token_block_start_sequence[] = {CIO_TOKEN_BLOCK_START};
// static const cio_internal_sequence_t cio_internal_token_block_end_sequence[] = {CIO_TOKEN_BLOCK_END};
// static const cio_internal_sequence_t cio_internal_token_specifier_expression_start_sequence[] = {CIO_TOKEN_SPECIFIER_EXPRESSION_START};
// static const cio_internal_sequence_t cio_internal_token_specifier_expression_end_sequence[] = {CIO_TOKEN_SPECIFIER_EXPRESSION_END};
// static const cio_internal_sequence_t cio_internal_token_statement_delimiter_sequence[] = {CIO_TOKEN_STATEMENT_DELIMITER};
static const cio_internal_sequence_t cio_internal_token_parameter_delimiter_sequence[] = {CIO_TOKEN_PARAMETER_DELIMITER};
// static const cio_internal_sequence_t cio_internal_token_number_sequence[] = {CIO_TOKEN_NUMBER};
// static const cio_internal_sequence_t cio_internal_token_string_sequence[] = {CIO_TOKEN_STRING};

static const cio_internal_sequence_t cio_internal_storage_sequence[] = {CIO_TOKEN_STORAGE, CIO_TOKEN_SPECIFIER_EXPRESSION_START, CIO_TOKEN_NUMBER, CIO_TOKEN_SPECIFIER_EXPRESSION_END};
static const cio_internal_sequence_t cio_internal_alignment_sequence[] = {CIO_TOKEN_ALIGNMENT, CIO_TOKEN_SPECIFIER_EXPRESSION_START, CIO_TOKEN_NUMBER, CIO_TOKEN_SPECIFIER_EXPRESSION_END};

#define CIO_INTERNAL_SEQUENCE_LENGTH(sequence) (sizeof(sequence) / sizeof(cio_token_type_t))

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
	glogf(FATAL, "Malformed program: %s in %s:%zu:%zu", message, source_file, line, column);

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

gen_error_t cio_parse(const cio_token_t* const restrict tokens, const size_t tokens_length, cio_program_t* const restrict out_program, const char* const restrict source, const size_t source_length, const char* const restrict source_file, const size_t source_file_length) {
	GEN_FRAME_BEGIN(cio_parse);

	GEN_INTERNAL_BASIC_PARAM_CHECK(tokens);
	GEN_INTERNAL_BASIC_PARAM_CHECK(out_program);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source_file);

	gen_error_t error = GEN_OK;

	GEN_FOREACH_PTR(offset, token, tokens_length, tokens) {
		(void) token;

		error = cio_internal_validate_sequence(tokens, tokens_length, offset, cio_internal_token_identifier_sequence, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_token_identifier_sequence), source, source_length, source_file, source_file_length);
		GEN_ERROR_OUT_IF(error, "`cio_internal_validate_sequence` failed");

		error = grealloc((void**) &out_program->routines, ++out_program->routines_length, sizeof(cio_routine_t));
		GEN_ERROR_OUT_IF(error, "`grealloc` failed");

		cio_routine_t* const routine = &out_program->routines[out_program->routines_length - 1];
		error = gen_string_duplicate(source + token->offset, source_length - token->offset, token->length, &routine->identifier);
		GEN_ERROR_OUT_IF(error, "`gen_string_duplicate` failed");

		GEN_FOREACH_PTR_ADVANCE(offset, token, tokens_length, tokens, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_token_identifier_sequence));

		while(!(token->type & (CIO_TOKEN_BLOCK_START | CIO_TOKEN_STATEMENT_DELIMITER))) {
			error = grealloc((void**) &routine->parameters, ++routine->parameters_length, sizeof(cio_routine_t));
			GEN_ERROR_OUT_IF(error, "`grealloc` failed");

			cio_storage_t* const parameter = &routine->parameters[routine->parameters_length - 1];

			error = cio_internal_validate_sequence(tokens, tokens_length, offset, cio_internal_storage_sequence, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_storage_sequence), source, source_length, source_file, source_file_length);
			GEN_ERROR_OUT_IF(error, "`cio_internal_validate_sequence` failed");
			char* temporary_size_copy = NULL;
			error = gen_string_duplicate(source + tokens[offset + 2].offset, source_length - tokens[offset + 2].offset, tokens[offset + 2].length, &temporary_size_copy);
			GEN_ERROR_OUT_IF(error, "`gen_string_duplicate` failed");
			parameter->size = strtoul(temporary_size_copy, NULL, 10);
			GEN_ERROR_OUT_IF_ERRNO(strtoul, errno);
			error = gfree(temporary_size_copy);
			GEN_ERROR_OUT_IF(error, "`gfree` failed");
			GEN_FOREACH_PTR_ADVANCE(offset, token, tokens_length, tokens, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_storage_sequence));

			error = cio_internal_validate_sequence(tokens, tokens_length, offset, cio_internal_alignment_sequence, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_alignment_sequence), source, source_length, source_file, source_file_length);
			GEN_ERROR_OUT_IF(error, "`cio_internal_validate_sequence` failed");
			char* temporary_alignment_copy = NULL;
			error = gen_string_duplicate(source + tokens[offset + 2].offset, source_length - tokens[offset + 2].offset, tokens[offset + 2].length, &temporary_alignment_copy);
			GEN_ERROR_OUT_IF(error, "`gen_string_duplicate` failed");
			parameter->alignment = strtoul(temporary_alignment_copy, NULL, 10);
			GEN_ERROR_OUT_IF_ERRNO(strtoul, errno);
			error = gfree(temporary_alignment_copy);
			GEN_ERROR_OUT_IF(error, "`gfree` failed");
			GEN_FOREACH_PTR_ADVANCE(offset, token, tokens_length, tokens, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_alignment_sequence));

			error = cio_internal_validate_sequence(tokens, tokens_length, offset, cio_internal_token_identifier_sequence, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_token_identifier_sequence), source, source_length, source_file, source_file_length);
			GEN_ERROR_OUT_IF(error, "`cio_internal_validate_sequence` failed");
			error = gen_string_duplicate(source + token->offset, source_length - token->offset, token->length, &parameter->identifier);
			GEN_ERROR_OUT_IF(error, "`gen_string_duplicate` failed");
			GEN_FOREACH_PTR_ADVANCE(offset, token, tokens_length, tokens, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_token_identifier_sequence));

			if(token->type & (CIO_TOKEN_BLOCK_START | CIO_TOKEN_STATEMENT_DELIMITER)) break;

			error = cio_internal_validate_sequence(tokens, tokens_length, offset, cio_internal_token_parameter_delimiter_sequence, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_token_parameter_delimiter_sequence), source, source_length, source_file, source_file_length);
			GEN_ERROR_OUT_IF(error, "`cio_internal_validate_sequence` failed");

			GEN_FOREACH_PTR_ADVANCE(offset, token, tokens_length, tokens, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_token_parameter_delimiter_sequence));
		}

		break;
	}

	GEN_ALL_OK;
}
