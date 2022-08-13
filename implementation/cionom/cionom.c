// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+cionom@pm.me>

#include "include/cionom.h"

gen_error_t cio_line_from_offset(const size_t offset, size_t* const restrict out_line, const char* const restrict source, const size_t source_length) {
	GEN_FRAME_BEGIN(cio_line_from_offset);

	GEN_NULL_CHECK(source);
	GEN_NULL_CHECK(out_line);

	if(offset >= source_length) GEN_ERROR_OUT(GEN_TOO_LONG, "`offset` exceeded `source_length`");

	*out_line = 1;
	GEN_STRING_FOREACH(c, offset, source)
	if(*c == '\n') ++*out_line;

	GEN_ALL_OK;
}

gen_error_t cio_column_from_offset(const size_t offset, size_t* const restrict out_column, const char* const restrict source, const size_t source_length) {
	GEN_FRAME_BEGIN(cio_column_from_offset);

	GEN_NULL_CHECK(source);
	GEN_NULL_CHECK(out_column);

	if(offset >= source_length) GEN_ERROR_OUT(GEN_TOO_LONG, "`offset` exceeded `source_length`");

	*out_column = 1;
	GEN_STRING_FOREACH(c, offset, source) {
		++*out_column;
		if(*c == '\n') *out_column = 1;
	}

	GEN_ALL_OK;
}

static const char cio_internal_vm_mangled_grapheme_keys[] = {
	'+',
	'-',
	'/',
	'*',
	'=',
	'!',
	'?',
	'#',
	'|',
	'\\',
	'\"',
	'\'',
	';',
	':',
	'`',
	'~',
	'.',
	',',
	'<',
	'>',
	'[',
	']',
	'{',
	'}',
	'(',
	')',
	'@',
	'$',
	'^',
	'%',
	'&'};
static const char* const cio_internal_vm_mangled_grapheme_values[] = {
	"plus",
	"minus",
	"slash",
	"asterisk",
	"equals",
	"bang",
	"question_mark",
	"hash",
	"pipe",
	"backslash",
	"double_quote",
	"single_quote",
	"semicolon",
	"colon",
	"backtick",
	"tilde",
	"full_stop",
	"comma",
	"left_chevron",
	"right_chevron",
	"left_bracket",
	"right_bracket",
	"left_brace",
	"right_brace",
	"left_parenthesis",
	"right_parenthesis",
	"at",
	"dollar",
	"circumflex",
	"percentage",
	"ampersand"};
static const char cionom_internal_vm_mangled_grapheme_prefix[] = "__cionom_mangled_grapheme_";

gen_error_t cio_mangle_identifier(const char* const restrict identifier, char** const restrict out_mangled) {
	GEN_FRAME_BEGIN(cio_mangle_identifier);

	GEN_NULL_CHECK(identifier);
	GEN_NULL_CHECK(out_mangled);

	size_t identifier_length = 0;
	gen_error_t error = gen_string_length(identifier, GEN_STRING_NO_BOUND, GEN_STRING_NO_BOUND, &identifier_length);
	GEN_ERROR_OUT_IF(error, "`gen_string_length` failed");

	size_t mangled_length = 0;
	*out_mangled = NULL;
	GEN_STRING_FOREACH(c, identifier_length, identifier) {
		if(*c == '_' || isalnum(*c)) {
			error = grealloc((void**) out_mangled, mangled_length, mangled_length + 2, sizeof(char));
			GEN_ERROR_OUT_IF(error, "`grealloc` failed");
			(*out_mangled)[mangled_length++] = *c;
			continue;
		}

		const char* mangled_grapheme_value = NULL;
		GEN_STRING_FOREACH(key, sizeof(cio_internal_vm_mangled_grapheme_keys), cio_internal_vm_mangled_grapheme_keys) {
			if(*c == *key) {
				mangled_grapheme_value = cio_internal_vm_mangled_grapheme_values[key - cio_internal_vm_mangled_grapheme_keys];
				break;
			}
		}

		if(!mangled_grapheme_value) GEN_ERROR_OUT(GEN_BAD_CONTENT, "Invalid character encountered while mangling symbol");

		size_t mangled_grapheme_length = 0;
		error = gen_string_length(mangled_grapheme_value, GEN_STRING_NO_BOUND, GEN_STRING_NO_BOUND, &mangled_grapheme_length);
		GEN_ERROR_OUT_IF(error, "`gen_string_length` failed");

		error = grealloc((void**) out_mangled, mangled_length, mangled_length + (sizeof(cionom_internal_vm_mangled_grapheme_prefix) - 1) + mangled_grapheme_length + 1, sizeof(char));
		GEN_ERROR_OUT_IF(error, "`grealloc` failed");

		mangled_length += (sizeof(cionom_internal_vm_mangled_grapheme_prefix) - 1) + mangled_grapheme_length;

		error = gen_string_append(*out_mangled, mangled_length + 1, cionom_internal_vm_mangled_grapheme_prefix, sizeof(cionom_internal_vm_mangled_grapheme_prefix), sizeof(cionom_internal_vm_mangled_grapheme_prefix) - 1);
		GEN_ERROR_OUT_IF(error, "`gen_string_append` failed");
		error = gen_string_append(*out_mangled, mangled_length + 1, mangled_grapheme_value, mangled_grapheme_length + 1, mangled_grapheme_length);
		GEN_ERROR_OUT_IF(error, "`gen_string_append` failed");
	}

	GEN_ALL_OK;
}

gen_error_t cio_resolve_external(const char* const restrict identifier, cio_routine_function_t* const out_function, const gen_dylib_t lib) {
	GEN_FRAME_BEGIN(cio_resolve_external);

	GEN_NULL_CHECK(identifier);
	GEN_NULL_CHECK(out_function);

	char* mangled = NULL;
	gen_error_t error = cio_mangle_identifier(identifier, &mangled);
	GEN_ERROR_OUT_IF(error, "`cio_mangle_identifier` failed");

	// glogf(DEBUG, "Attempting to resolve `%s` externally as `%s`...", identifier, mangled);

	error = gen_dylib_symbol((void*) out_function, lib, mangled);
	GEN_ERROR_OUT_IF(error, "`gen_dylib_symbol` failed");
	error = gfree(mangled);
	GEN_ERROR_OUT_IF(error, "`gfree` failed");

	GEN_ALL_OK;
}
