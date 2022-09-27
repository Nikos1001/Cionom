// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

#include "include/cionom.h"

#include <genmemory.h>
#include <genstring.h>

gen_error_t* cio_line_from_offset(const size_t offset, size_t* const restrict out_line, const char* const restrict source, const size_t source_length) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_line_from_offset, GEN_FILE_NAME);
	if(error) return error;

	if(!source) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`source` was `NULL`");
	if(!out_line) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`out_line` was `NULL`");

	if(offset >= source_length) return gen_error_attach_backtrace(GEN_ERROR_TOO_LONG, GEN_LINE_NUMBER, "`offset` exceeded `source_length`");

	*out_line = 1;
    for(size_t i = 0; i < offset; ++i) if(source[i] == '\n') ++*out_line;

	return NULL;
}

gen_error_t* cio_column_from_offset(const size_t offset, size_t* const restrict out_column, const char* const restrict source, const size_t source_length) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_column_from_offset, GEN_FILE_NAME);
	if(error) return error;

	if(!source) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`source` was `NULL`");
	if(!out_column) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`out_column` was `NULL`");

	if(offset >= source_length) return gen_error_attach_backtrace(GEN_ERROR_TOO_LONG, GEN_LINE_NUMBER, "`offset` exceeded `source_length`");

	*out_column = 1;
	for(size_t i = 0; i < offset; ++i) {
		++*out_column;
		if(source[i] == '\n') *out_column = 1;
	}

	return NULL;
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

static void cio_internal_mangle_identifier_cleanup_mangled(char** mangled) {
    if(!*mangled) return;

    gen_error_t* error = gen_memory_free((void**) mangled);
    if(error) {
        gen_error_print("cionom", error, GEN_ERROR_SEVERITY_FATAL);
        gen_error_abort();
    }
}

gen_error_t* cio_mangle_identifier(const char* const restrict identifier, char** const restrict out_mangled) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_mangle_identifier, GEN_FILE_NAME);
	if(error) return error;

	if(!identifier) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`identifier` was `NULL`");
	if(!out_mangled) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`out_mangled` was `NULL`");

	size_t identifier_length = 0;
	error = gen_string_length(identifier, GEN_STRING_NO_BOUNDS, GEN_STRING_NO_BOUNDS, &identifier_length);
	if(error) return error;

	size_t mangled_length = 0;
    GEN_CLEANUP_FUNCTION(cio_internal_mangle_identifier_cleanup_mangled) char* mangled_cleanup = *out_mangled;
    for(size_t i = 0; i < identifier_length; ++i) {
		if(identifier[i] == '_' || (identifier[i] >= '0' && identifier[i] <= '9') || (identifier[i] >= 'a' && identifier[i] <= 'z') || (identifier[i] >= 'A' && identifier[i] <= 'Z')) {
			error = gen_memory_reallocate_zeroed((void**) out_mangled, mangled_length, mangled_length + 2, sizeof(char));
        	if(error) return error;
			(*out_mangled)[mangled_length++] = identifier[i];
			continue;
		}

		const char* mangled_grapheme_value = NULL;
        for(size_t j = 0; j < sizeof(cio_internal_vm_mangled_grapheme_keys); ++j) {
			if(identifier[i] == cio_internal_vm_mangled_grapheme_keys[j]) {
				mangled_grapheme_value = cio_internal_vm_mangled_grapheme_values[j];
				break;
			}
		}

        // TODO: Warning for this and just append as-is.
		if(!mangled_grapheme_value) return gen_error_attach_backtrace_formatted(GEN_ERROR_BAD_CONTENT, GEN_LINE_NUMBER, "Invalid character encountered while mangling symbol `%t`: '%c'", identifier, identifier[i]);

		size_t mangled_grapheme_length = 0;
		error = gen_string_length(mangled_grapheme_value, GEN_STRING_NO_BOUNDS, GEN_STRING_NO_BOUNDS, &mangled_grapheme_length);
		if(error) return error;

		error = gen_memory_reallocate_zeroed((void**) out_mangled, mangled_length, mangled_length + (sizeof(cionom_internal_vm_mangled_grapheme_prefix) - 1) + mangled_grapheme_length + 1, sizeof(char));
		if(error) return error;

		mangled_length += (sizeof(cionom_internal_vm_mangled_grapheme_prefix) - 1) + mangled_grapheme_length;

		error = gen_string_append(*out_mangled, mangled_length + 1, cionom_internal_vm_mangled_grapheme_prefix, sizeof(cionom_internal_vm_mangled_grapheme_prefix), sizeof(cionom_internal_vm_mangled_grapheme_prefix) - 1);
		if(error) return error;
		error = gen_string_append(*out_mangled, mangled_length + 1, mangled_grapheme_value, mangled_grapheme_length + 1, mangled_grapheme_length);
		if(error) return error;
	}

    mangled_cleanup = NULL;

	return NULL;
}

static void cio_internal_resolve_external_cleanup_mangled(char** mangled) {
    if(!*mangled) return;

    gen_error_t* error = gen_memory_free((void**) mangled);
    if(error) {
        gen_error_print("cionom", error, GEN_ERROR_SEVERITY_FATAL);
        gen_error_abort();
    }
}

gen_error_t* cio_resolve_external(const char* const restrict identifier, cio_routine_function_t* const out_function, const gen_dynamic_library_handle_t* const restrict lib) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_resolve_external, GEN_FILE_NAME);
	if(error) return error;

	if(!identifier) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`identifier` was `NULL`");
	if(!out_function) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`out_function` was `NULL`");

	GEN_CLEANUP_FUNCTION(cio_internal_resolve_external_cleanup_mangled) char* mangled = NULL;
	error = cio_mangle_identifier(identifier, &mangled);
	if(error) return error;

    size_t mangled_length = 0;
    error = gen_string_length(mangled, GEN_STRING_NO_BOUNDS, GEN_STRING_NO_BOUNDS, &mangled_length);
	if(error) return error;

	error = gen_dynamic_library_handle_get_symbol(lib, mangled, mangled_length, (void*) out_function);
	if(error) return error;

	return NULL;
}
