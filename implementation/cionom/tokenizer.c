// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+genstone@pm.me>

#include "include/cionom.h"

#include <genstring.h>

#define ADVANCE (c = source[++offset])
#define IS_WHITESPACE (c == ' ' || c == '\t' || c == '\n')
#define IS_NUMBER (c > '0' && c < '9')
#define CHARACTER_TOKEN(t) \
	do { \
		token->type = t; \
		token->length = 1; \
	} while(0); \
	goto next_character

static __nodiscard gen_error_t cio_internal_set_sequence_type(cio_token_t* const restrict token, const char* const restrict source, const size_t source_len) {
	GEN_FRAME_BEGIN(cio_internal_set_sequence_type);

	GEN_INTERNAL_BASIC_PARAM_CHECK(token);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source);

	bool result = false;
	gen_error_t error = GEN_OK;

	error = gen_string_compare(source + token->offset, (source_len + 1) - token->offset, "return", sizeof("return"), token->length, &result);
	GEN_ERROR_OUT_IF(error, "`gen_string_compare` failed");
	if(result) {
		token->type = CIO_TOKEN_RETURN;
		GEN_ALL_OK;
	}
	error = gen_string_compare(source + token->offset, (source_len + 1) - token->offset, "storage", sizeof("storage"), token->length, &result);
	GEN_ERROR_OUT_IF(error, "`gen_string_compare` failed");
	if(result) {
		token->type = CIO_TOKEN_STORAGE;
		GEN_ALL_OK;
	}
	error = gen_string_compare(source + token->offset, (source_len + 1) - token->offset, "alignment", sizeof("alignment"), token->length, &result);
	GEN_ERROR_OUT_IF(error, "`gen_string_compare` failed");
	if(result) {
		token->type = CIO_TOKEN_ALIGNMENT;
		GEN_ALL_OK;
	}

	token->type = CIO_TOKEN_IDENTIFIER;
	GEN_ALL_OK;
}

gen_error_t cio_tokenize(const char* const restrict source, cio_token_t** const restrict out_tokens, size_t* const restrict out_n_tokens) {
	GEN_FRAME_BEGIN(cio_tokenize);

	GEN_INTERNAL_BASIC_PARAM_CHECK(source);
	GEN_INTERNAL_BASIC_PARAM_CHECK(out_tokens);
	GEN_INTERNAL_BASIC_PARAM_CHECK(out_n_tokens);

	*out_tokens = NULL;
	*out_n_tokens = 0;

	gen_error_t error = GEN_OK;

	size_t source_len = 0;
	error = gen_string_length(source, GEN_STRING_NO_BOUND, GEN_STRING_NO_BOUND, &source_len);
	GEN_ERROR_OUT_IF(error, "`gen_string_length` failed");

	size_t offset = 0;
	char c = 0;

	do {
		if(IS_WHITESPACE) goto next_character;

		error = grealloc((void**) out_tokens, ++*out_n_tokens, sizeof(cio_token_t));
		GEN_ERROR_OUT_IF(error, "`grealloc` failed");
		cio_token_t* const token = &(*out_tokens)[*out_n_tokens - 1];
		token->offset = offset;

		switch(c) {
			case '{': CHARACTER_TOKEN(CIO_TOKEN_BLOCK_START);
			case '}': CHARACTER_TOKEN(CIO_TOKEN_BLOCK_END);
			case '<': CHARACTER_TOKEN(CIO_TOKEN_SPECIFIER_EXPRESSION_START);
			case '>': CHARACTER_TOKEN(CIO_TOKEN_SPECIFIER_EXPRESSION_END);
			case ';': CHARACTER_TOKEN(CIO_TOKEN_STATEMENT_DELIMITER);
			case ',': CHARACTER_TOKEN(CIO_TOKEN_PARAMETER_DELIMITER);
			case '"': {
				token->type = CIO_TOKEN_STRING;
				while(ADVANCE != '"') {
					token->length = offset - token->offset;
				}
				goto next_character;
			}
			default: {
				if(IS_NUMBER) {
					token->type = CIO_TOKEN_NUMBER;
					do {
						token->length = offset - token->offset;
						ADVANCE;
					} while(IS_NUMBER);
					goto current_character;
				}
				else {
					while(true) {
						token->length = offset - token->offset;
						if(IS_WHITESPACE || c == '{' || c == '}' || c == '<' || c == '>' || c == ';' || c == ',') {
							error = cio_internal_set_sequence_type(token, source, source_len);
							GEN_ERROR_OUT_IF(error, "`cio_internal_set_sequence_type` failed");
							goto current_character;
						}
						ADVANCE;
					}
				}
			}
		}
	next_character:
		ADVANCE;
	current_character:
		continue;
	} while(c);

	GEN_ALL_OK;
}
