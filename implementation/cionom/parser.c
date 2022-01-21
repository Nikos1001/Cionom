// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+genstone@pm.me>

#include "include/cionom.h"

__unused static __nodiscard gen_error_t cio_internal_parse_error(const cio_token_t* const restrict token, const char* const restrict message, const char* const restrict source, const size_t source_length, const char* const restrict source_file, __unused const size_t source_file_length) {
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

gen_error_t cio_parse(const cio_token_t* const restrict tokens, __unused const size_t tokens_length, cio_program_t* const restrict out_program, const char* const restrict source, __unused const size_t source_length, const char* const restrict source_file, __unused const size_t source_file_length) {
	GEN_FRAME_BEGIN(cio_parse);

	GEN_INTERNAL_BASIC_PARAM_CHECK(tokens);
	GEN_INTERNAL_BASIC_PARAM_CHECK(out_program);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source_file);

	GEN_ALL_OK;
}
