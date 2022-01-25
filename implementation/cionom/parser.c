// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+genstone@pm.me>

#include "include/cionom.h"

typedef unsigned cio_internal_sequence_t;

static const cio_internal_sequence_t cio_internal_token_identifier_sequence[] = {CIO_TOKEN_IDENTIFIER};
// static const cio_internal_sequence_t cio_internal_token_storage_sequence[] = {CIO_TOKEN_STORAGE};
// static const cio_internal_sequence_t cio_internal_token_alignment_sequence[] = {CIO_TOKEN_ALIGNMENT};
// static const cio_internal_sequence_t cio_internal_token_block_start_sequence[] = {CIO_TOKEN_BLOCK_START};
// static const cio_internal_sequence_t cio_internal_token_block_end_sequence[] = {CIO_TOKEN_BLOCK_END};
// static const cio_internal_sequence_t cio_internal_token_specifier_expression_start_sequence[] = {CIO_TOKEN_SPECIFIER_EXPRESSION_START};
// static const cio_internal_sequence_t cio_internal_token_specifier_expression_end_sequence[] = {CIO_TOKEN_SPECIFIER_EXPRESSION_END};
static const cio_internal_sequence_t cio_internal_token_statement_delimiter_sequence[] = {CIO_TOKEN_STATEMENT_DELIMITER};
static const cio_internal_sequence_t cio_internal_token_parameter_delimiter_sequence[] = {CIO_TOKEN_PARAMETER_DELIMITER};
// static const cio_internal_sequence_t cio_internal_token_number_sequence[] = {CIO_TOKEN_NUMBER};
// static const cio_internal_sequence_t cio_internal_token_string_sequence[] = {CIO_TOKEN_STRING};

static const cio_internal_sequence_t cio_internal_return_sequence[] = {CIO_TOKEN_RETURN, CIO_TOKEN_STATEMENT_DELIMITER};
static const cio_internal_sequence_t cio_internal_value_sequence[] = {CIO_TOKEN_STRING | CIO_TOKEN_IDENTIFIER | CIO_TOKEN_NUMBER};
static const cio_internal_sequence_t cio_internal_storage_sequence[] = {CIO_TOKEN_STORAGE, CIO_TOKEN_SPECIFIER_EXPRESSION_START, CIO_TOKEN_NUMBER, CIO_TOKEN_SPECIFIER_EXPRESSION_END};
static const cio_internal_sequence_t cio_internal_alignment_sequence[] = {CIO_TOKEN_ALIGNMENT, CIO_TOKEN_SPECIFIER_EXPRESSION_START, CIO_TOKEN_NUMBER, CIO_TOKEN_SPECIFIER_EXPRESSION_END};

#define CIO_INTERNAL_SEQUENCE_LENGTH(sequence) (sizeof(sequence) / sizeof(cio_internal_sequence_t))

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
			error = cio_internal_parse_error(&tokens[first + i], "Unexpected token", source, source_length, source_file, source_file_length);
			GEN_ERROR_OUT_IF(error, "`cio_internal_parse_error` failed");
			GEN_ERROR_OUT(GEN_BAD_CONTENT, "Parsing failed");
		}
	}

	GEN_ALL_OK;
}

static __nodiscard gen_error_t cio_internal_parse_storage(const cio_token_t* const restrict tokens, const size_t tokens_length, const size_t first, size_t* const restrict out_stride, cio_storage_t* const restrict out_storage, const char* const restrict source, const size_t source_length, const char* const restrict source_file, const size_t source_file_length) {
	GEN_FRAME_BEGIN(cio_internal_parse_storage);

	GEN_INTERNAL_BASIC_PARAM_CHECK(tokens);
	GEN_INTERNAL_BASIC_PARAM_CHECK(out_storage);
	GEN_INTERNAL_BASIC_PARAM_CHECK(out_stride);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source_file);

	gen_error_t error = GEN_OK;
	size_t offset = first;

	error = cio_internal_validate_sequence(tokens, tokens_length, offset, cio_internal_storage_sequence, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_storage_sequence), source, source_length, source_file, source_file_length);
	GEN_ERROR_OUT_IF(error, "`cio_internal_validate_sequence` failed");
	char* temporary_size_copy = NULL;
	error = gen_string_duplicate(source + tokens[offset + 2].offset, source_length - tokens[offset + 2].offset, tokens[offset + 2].length, &temporary_size_copy);
	GEN_ERROR_OUT_IF(error, "`gen_string_duplicate` failed");
	out_storage->size = strtoul(temporary_size_copy, NULL, 10);
	GEN_ERROR_OUT_IF_ERRNO(strtoul, errno);
	error = gfree(temporary_size_copy);
	GEN_ERROR_OUT_IF(error, "`gfree` failed");
	offset += CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_storage_sequence);

	error = cio_internal_validate_sequence(tokens, tokens_length, offset, cio_internal_alignment_sequence, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_alignment_sequence), source, source_length, source_file, source_file_length);
	GEN_ERROR_OUT_IF(error, "`cio_internal_validate_sequence` failed");
	char* temporary_alignment_copy = NULL;
	error = gen_string_duplicate(source + tokens[offset + 2].offset, source_length - tokens[offset + 2].offset, tokens[offset + 2].length, &temporary_alignment_copy);
	GEN_ERROR_OUT_IF(error, "`gen_string_duplicate` failed");
	out_storage->alignment = strtoul(temporary_alignment_copy, NULL, 10);
	GEN_ERROR_OUT_IF_ERRNO(strtoul, errno);
	error = gfree(temporary_alignment_copy);
	GEN_ERROR_OUT_IF(error, "`gfree` failed");
	offset += CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_alignment_sequence);

	error = cio_internal_validate_sequence(tokens, tokens_length, offset, cio_internal_token_identifier_sequence, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_token_identifier_sequence), source, source_length, source_file, source_file_length);
	GEN_ERROR_OUT_IF(error, "`cio_internal_validate_sequence` failed");
	error = gen_string_duplicate(source + tokens[offset].offset, source_length - tokens[offset].offset, tokens[offset].length, &out_storage->identifier);
	GEN_ERROR_OUT_IF(error, "`gen_string_duplicate` failed");
	offset += CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_token_identifier_sequence);

	*out_stride = offset - first;

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
			error = grealloc((void**) &routine->parameters, ++routine->parameters_length, sizeof(cio_storage_t));
			GEN_ERROR_OUT_IF(error, "`grealloc` failed");

			size_t stride = 0;
			error = cio_internal_parse_storage(tokens, tokens_length, offset, &stride, &routine->parameters[routine->parameters_length - 1], source, source_length, source_file, source_file_length);
			GEN_ERROR_OUT_IF(error, "`cio_internal_parse_storage` failed");
			GEN_FOREACH_PTR_ADVANCE(offset, token, tokens_length, tokens, stride);

			if(token->type & (CIO_TOKEN_BLOCK_START | CIO_TOKEN_STATEMENT_DELIMITER)) break;

			error = cio_internal_validate_sequence(tokens, tokens_length, offset, cio_internal_token_parameter_delimiter_sequence, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_token_parameter_delimiter_sequence), source, source_length, source_file, source_file_length);
			GEN_ERROR_OUT_IF(error, "`cio_internal_validate_sequence` failed");
			GEN_FOREACH_PTR_ADVANCE(offset, token, tokens_length, tokens, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_token_parameter_delimiter_sequence));
		}
		if(token->type & CIO_TOKEN_STATEMENT_DELIMITER) continue;

		GEN_FOREACH_PTR_ADVANCE(offset, token, tokens_length, tokens, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_token_statement_delimiter_sequence));

		while(!(token->type & CIO_TOKEN_BLOCK_END)) {
			error = grealloc((void**) &routine->statements, ++routine->statements_length, sizeof(cio_statement_t));
			GEN_ERROR_OUT_IF(error, "`grealloc` failed");

			cio_statement_t* const statement = &routine->statements[routine->statements_length - 1];

			if(token->type & CIO_TOKEN_STORAGE) {
				statement->type = CIO_STATEMENT_STORAGE;

				size_t stride = 0;
				error = cio_internal_parse_storage(tokens, tokens_length, offset, &stride, &statement->storage, source, source_length, source_file, source_file_length);
				GEN_ERROR_OUT_IF(error, "`cio_internal_parse_storage` failed");
				GEN_FOREACH_PTR_ADVANCE(offset, token, tokens_length, tokens, stride);

				error = cio_internal_validate_sequence(tokens, tokens_length, offset, cio_internal_token_statement_delimiter_sequence, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_token_statement_delimiter_sequence), source, source_length, source_file, source_file_length);
				GEN_ERROR_OUT_IF(error, "`cio_internal_validate_sequence` failed");
				GEN_FOREACH_PTR_ADVANCE(offset, token, tokens_length, tokens, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_token_statement_delimiter_sequence));
			}
			else if(token->type & CIO_TOKEN_IDENTIFIER) {
				statement->type = CIO_STATEMENT_CALL;

				error = gen_string_duplicate(source + token->offset, source_length - token->offset, token->length, &statement->call.identifier);
				GEN_ERROR_OUT_IF(error, "`gen_string_duplicate` failed");

				GEN_FOREACH_PTR_ADVANCE(offset, token, tokens_length, tokens, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_token_identifier_sequence));

				while(!(token->type & CIO_TOKEN_STATEMENT_DELIMITER)) {
					error = grealloc((void**) &statement->call.parameters, ++statement->call.parameters_length, sizeof(cio_expression_t));
					GEN_ERROR_OUT_IF(error, "`grealloc` failed");

					cio_expression_t* const parameter = &statement->call.parameters[statement->call.parameters_length - 1];

					error = cio_internal_validate_sequence(tokens, tokens_length, offset, cio_internal_value_sequence, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_value_sequence), source, source_length, source_file, source_file_length);
					GEN_ERROR_OUT_IF(error, "`cio_internal_validate_sequence` failed");
					switch(token->type) {
						case CIO_TOKEN_NUMBER: {
							parameter->type = CIO_EXPRESSION_NUMBER;

							char* temporary_number_copy = NULL;
							error = gen_string_duplicate(source + token->offset, source_length - token->offset, token->length, &temporary_number_copy);
							GEN_ERROR_OUT_IF(error, "`gen_string_duplicate` failed");
							parameter->number = strtoul(temporary_number_copy, NULL, 10);
							GEN_ERROR_OUT_IF_ERRNO(strtoul, errno);
							error = gfree(temporary_number_copy);
							GEN_ERROR_OUT_IF(error, "`gfree` failed");

							break;
						}
						case CIO_TOKEN_STRING: {
							parameter->type = CIO_EXPRESSION_STRING;

							error = gen_string_duplicate(source + token->offset + 1, source_length - token->offset, token->length - 2, &parameter->string);
							GEN_ERROR_OUT_IF(error, "`gen_string_duplicate` failed");

							break;
						}
						case CIO_TOKEN_IDENTIFIER: {
							parameter->type = CIO_EXPRESSION_STORAGE;

							error = gen_string_duplicate(source + token->offset, source_length - token->offset, token->length, &parameter->identifier);
							GEN_ERROR_OUT_IF(error, "`gen_string_duplicate` failed");

							break;
						}
						case CIO_TOKEN_RETURN:
						case CIO_TOKEN_STORAGE:
						case CIO_TOKEN_ALIGNMENT:
						case CIO_TOKEN_BLOCK_START:
						case CIO_TOKEN_BLOCK_END:
						case CIO_TOKEN_SPECIFIER_EXPRESSION_START:
						case CIO_TOKEN_SPECIFIER_EXPRESSION_END:
						case CIO_TOKEN_STATEMENT_DELIMITER:
						case CIO_TOKEN_PARAMETER_DELIMITER: {
							error = cio_internal_parse_error(token, "Unexpected token", source, source_length, source_file, source_file_length);
							GEN_ERROR_OUT_IF(error, "`cio_internal_parse_error` failed");
							GEN_ERROR_OUT(GEN_BAD_CONTENT, "Parsing failed");
						}
					}
					error = cio_internal_validate_sequence(tokens, tokens_length, offset, cio_internal_value_sequence, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_value_sequence), source, source_length, source_file, source_file_length);
					GEN_ERROR_OUT_IF(error, "`cio_internal_validate_sequence` failed");
					GEN_FOREACH_PTR_ADVANCE(offset, token, tokens_length, tokens, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_value_sequence));

					if(token->type & CIO_TOKEN_STATEMENT_DELIMITER) break;

					error = cio_internal_validate_sequence(tokens, tokens_length, offset, cio_internal_token_parameter_delimiter_sequence, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_token_parameter_delimiter_sequence), source, source_length, source_file, source_file_length);
					GEN_ERROR_OUT_IF(error, "`cio_internal_validate_sequence` failed");
					GEN_FOREACH_PTR_ADVANCE(offset, token, tokens_length, tokens, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_token_parameter_delimiter_sequence));
				}

				error = cio_internal_validate_sequence(tokens, tokens_length, offset, cio_internal_token_statement_delimiter_sequence, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_token_statement_delimiter_sequence), source, source_length, source_file, source_file_length);
				GEN_ERROR_OUT_IF(error, "`cio_internal_validate_sequence` failed");
				GEN_FOREACH_PTR_ADVANCE(offset, token, tokens_length, tokens, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_token_statement_delimiter_sequence));
			}
			else if(token->type & CIO_TOKEN_RETURN) {
				statement->type = CIO_STATEMENT_RETURN;

				error = cio_internal_validate_sequence(tokens, tokens_length, offset, cio_internal_return_sequence, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_return_sequence), source, source_length, source_file, source_file_length);
				GEN_ERROR_OUT_IF(error, "`cio_internal_validate_sequence` failed");
				GEN_FOREACH_PTR_ADVANCE(offset, token, tokens_length, tokens, CIO_INTERNAL_SEQUENCE_LENGTH(cio_internal_return_sequence));
			}
			else {
				error = cio_internal_parse_error(token, "Unexpected token", source, source_length, source_file, source_file_length);
				GEN_ERROR_OUT_IF(error, "`cio_internal_parse_error` failed");
				GEN_ERROR_OUT(GEN_BAD_CONTENT, "Parsing failed");
			}
		}
	}

	GEN_ALL_OK;
}
