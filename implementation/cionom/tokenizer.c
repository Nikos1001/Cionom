// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+genstone@pm.me>

#include "include/cionom.h"

#define CIO_INTERNAL_TOKENIZER_ADVANCE (c = source[++offset])
#define CIO_INTERNAL_TOKENIZER_IS_WHITESPACE (c == ' ' || c == '\t' || c == '\n')
#define CIO_INTERNAL_TOKENIZER_IS_NUMBER (c > '0' && c < '9')
#define CIO_INTERNAL_TOKENIZER_CHARACTER_TOKEN(t) \
	token->type = t; \
	token->length = 1; \
	goto next_character

static __nodiscard gen_error_t cio_internal_set_sequence_type(cio_token_t* const restrict token, const char* const restrict source, const size_t source_length) {
	GEN_FRAME_BEGIN(cio_internal_set_sequence_type);

	GEN_INTERNAL_BASIC_PARAM_CHECK(token);
	GEN_INTERNAL_BASIC_PARAM_CHECK(source);

	bool result = false;
	gen_error_t error = GEN_OK;

	error = gen_string_compare(source + token->offset, (source_length + 1) - token->offset, "return", sizeof("return"), token->length, &result);
	GEN_ERROR_OUT_IF(error, "`gen_string_compare` failed");
	if(result) {
		token->type = CIO_TOKEN_RETURN;
		GEN_ALL_OK;
	}
	error = gen_string_compare(source + token->offset, (source_length + 1) - token->offset, "storage", sizeof("storage"), token->length, &result);
	GEN_ERROR_OUT_IF(error, "`gen_string_compare` failed");
	if(result) {
		token->type = CIO_TOKEN_STORAGE;
		GEN_ALL_OK;
	}
	error = gen_string_compare(source + token->offset, (source_length + 1) - token->offset, "alignment", sizeof("alignment"), token->length, &result);
	GEN_ERROR_OUT_IF(error, "`gen_string_compare` failed");
	if(result) {
		token->type = CIO_TOKEN_ALIGNMENT;
		GEN_ALL_OK;
	}

	token->type = CIO_TOKEN_IDENTIFIER;
	GEN_ALL_OK;
}

gen_error_t cio_tokenize(const char* const restrict source, const size_t source_length, cio_token_t** const restrict out_tokens, size_t* const restrict out_tokens_length) {
	GEN_FRAME_BEGIN(cio_tokenize);

	GEN_INTERNAL_BASIC_PARAM_CHECK(source);
	GEN_INTERNAL_BASIC_PARAM_CHECK(out_tokens);
	GEN_INTERNAL_BASIC_PARAM_CHECK(out_tokens_length);

	*out_tokens = NULL;
	*out_tokens_length = 0;

	gen_error_t error = GEN_OK;

	size_t offset = 0;
	char c = 0;

	do {
		if(CIO_INTERNAL_TOKENIZER_IS_WHITESPACE) goto next_character;

		error = grealloc((void**) out_tokens, ++*out_tokens_length, sizeof(cio_token_t));
		GEN_ERROR_OUT_IF(error, "`grealloc` failed");
		cio_token_t* const token = &(*out_tokens)[*out_tokens_length - 1];
		token->offset = offset;

		switch(c) {
			case '{': CIO_INTERNAL_TOKENIZER_CHARACTER_TOKEN(CIO_TOKEN_BLOCK_START);
			case '}': CIO_INTERNAL_TOKENIZER_CHARACTER_TOKEN(CIO_TOKEN_BLOCK_END);
			case '<': CIO_INTERNAL_TOKENIZER_CHARACTER_TOKEN(CIO_TOKEN_SPECIFIER_EXPRESSION_START);
			case '>': CIO_INTERNAL_TOKENIZER_CHARACTER_TOKEN(CIO_TOKEN_SPECIFIER_EXPRESSION_END);
			case ';': CIO_INTERNAL_TOKENIZER_CHARACTER_TOKEN(CIO_TOKEN_STATEMENT_DELIMITER);
			case ',': CIO_INTERNAL_TOKENIZER_CHARACTER_TOKEN(CIO_TOKEN_PARAMETER_DELIMITER);
			case '"': {
				token->type = CIO_TOKEN_STRING;
				while(CIO_INTERNAL_TOKENIZER_ADVANCE != '"') {
					token->length = offset - token->offset;
				}
				goto next_character;
			}
			default: {
				if(CIO_INTERNAL_TOKENIZER_IS_NUMBER) {
					token->type = CIO_TOKEN_NUMBER;
					do {
						token->length = offset - token->offset;
						CIO_INTERNAL_TOKENIZER_ADVANCE;
					} while(CIO_INTERNAL_TOKENIZER_IS_NUMBER);
					goto current_character;
				}
				else {
					while(true) {
						token->length = offset - token->offset;
						if(CIO_INTERNAL_TOKENIZER_IS_WHITESPACE || c == '{' || c == '}' || c == '<' || c == '>' || c == ';' || c == ',') {
							error = cio_internal_set_sequence_type(token, source, source_length);
							GEN_ERROR_OUT_IF(error, "`cio_internal_set_sequence_type` failed");
							goto current_character;
						}
						CIO_INTERNAL_TOKENIZER_ADVANCE;
					}
				}
			}
		}
	next_character:
		CIO_INTERNAL_TOKENIZER_ADVANCE;
	current_character:
		continue;
	} while(c);

	GEN_ALL_OK;
}
