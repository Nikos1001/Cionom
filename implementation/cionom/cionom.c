// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+genstone@pm.me>

#include "include/cionom.h"

gen_error_t cio_line_from_offset(const size_t offset, size_t* const restrict out_line, const char* const restrict source, const size_t source_length) {
	GEN_FRAME_BEGIN(cio_line_from_offset);

	GEN_INTERNAL_BASIC_PARAM_CHECK(source);
	GEN_INTERNAL_BASIC_PARAM_CHECK(out_line);

	if(offset >= source_length) GEN_ERROR_OUT(GEN_TOO_LONG, "`offset` exceeded `source_length`");

	*out_line = 1;
	GEN_STRING_FOREACH(c, source_length, source)
	if(*c == '\n') ++*out_line;

	GEN_ALL_OK;
}

gen_error_t cio_column_from_offset(const size_t offset, size_t* const restrict out_column, const char* const restrict source, const size_t source_length) {
	GEN_FRAME_BEGIN(cio_column_from_offset);

	GEN_INTERNAL_BASIC_PARAM_CHECK(source);
	GEN_INTERNAL_BASIC_PARAM_CHECK(out_column);

	if(offset >= source_length) GEN_ERROR_OUT(GEN_TOO_LONG, "`offset` exceeded `source_length`");

	*out_column = 1;
	GEN_STRING_FOREACH(c, source_length, source) {
		++*out_column;
		if(*c == '\n') *out_column = 1;
	}

	GEN_ALL_OK;
}
