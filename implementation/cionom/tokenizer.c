// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+genstone@pm.me>

#include "include/cionom.h"

#define CIO_INTERNAL_TOKENIZER_ADVANCE (c = source[++offset])
#define CIO_INTERNAL_TOKENIZER_IS_WHITESPACE (c == ' ' || c == '\t' || c == '\n' || c == '\0')
#define CIO_INTERNAL_TOKENIZER_IS_NUMBER (c >= '0' && c <= '9')

gen_error_t cio_tokenize(const char* const restrict source, const size_t source_length, cio_token_t** const restrict out_tokens, size_t* const restrict out_tokens_length) {
	GEN_FRAME_BEGIN(cio_tokenize);

	GEN_INTERNAL_BASIC_PARAM_CHECK(source);
	GEN_INTERNAL_BASIC_PARAM_CHECK(out_tokens);
	GEN_INTERNAL_BASIC_PARAM_CHECK(out_tokens_length);

	*out_tokens = NULL;
	*out_tokens_length = 0;

	gen_error_t error = GEN_OK;

	size_t offset = 0;
	char c = source[offset];

	do {
		if(CIO_INTERNAL_TOKENIZER_IS_WHITESPACE) {
			CIO_INTERNAL_TOKENIZER_ADVANCE;
			continue;
		}

		error = grealloc((void**) out_tokens, *out_tokens_length, ++*out_tokens_length, sizeof(cio_token_t));
		GEN_ERROR_OUT_IF(error, "`grealloc` failed");
		cio_token_t* const token = &(*out_tokens)[*out_tokens_length - 1];
		token->offset = offset;

		if(c == ':') {
			token->type = CIO_TOKEN_BLOCK;
			token->length = 1;
			CIO_INTERNAL_TOKENIZER_ADVANCE;
			continue;
		}
		else if(CIO_INTERNAL_TOKENIZER_IS_NUMBER) {
			token->type = CIO_TOKEN_NUMBER;
			do {
				CIO_INTERNAL_TOKENIZER_ADVANCE;
			} while(CIO_INTERNAL_TOKENIZER_IS_NUMBER);
			token->length = offset - token->offset;
			continue;
		}
		else {
			token->type = CIO_TOKEN_IDENTIFIER;
			do {
				CIO_INTERNAL_TOKENIZER_ADVANCE;
			} while(!CIO_INTERNAL_TOKENIZER_IS_WHITESPACE);
			token->length = offset - token->offset;
			continue;
		}
	} while(offset < source_length);

	GEN_ALL_OK;
}
