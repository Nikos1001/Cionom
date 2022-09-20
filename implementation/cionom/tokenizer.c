// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

#include "include/cionom.h"

#include <genmemory.h>

static void cio_tokenize_internal_cleanup_tokens(cio_token_t** tokens) {
    if(!*tokens) return;

    gen_error_t* error = gen_memory_free((void**) tokens);
    if(error) {
        gen_error_print("cionom", error, GEN_ERROR_SEVERITY_FATAL);
        gen_error_abort();
    }
}

gen_error_t* cio_tokenize(const char* const restrict source, const size_t source_length, cio_token_t** const restrict out_tokens, size_t* const restrict out_tokens_length) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_tokenize, GEN_FILE_NAME);
	if(error) return error;

	if(!source) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`source` was `NULL`");
	if(!out_tokens) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`out_tokens` was `NULL`");
	if(!out_tokens_length) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`out_tokens_length` was `NULL`");

	size_t offset = 0;
	char c = source[offset];

    GEN_CLEANUP_FUNCTION(cio_tokenize_internal_cleanup_tokens) cio_token_t* tokens_cleanup = *out_tokens;

	do {
		if((c == ' ' || c == '\t' || c == '\n' || c == '\0' || c == '\r')) {
			(c = source[++offset]);
			continue;
		}

		error = gen_memory_reallocate_zeroed((void**) out_tokens, *out_tokens_length, *out_tokens_length + 1, sizeof(cio_token_t));
        if(error) return error;
		++*out_tokens_length;
		cio_token_t* const token = &(*out_tokens)[*out_tokens_length - 1];
		token->offset = offset;

		if(c == ':') {
			token->type = CIO_TOKEN_BLOCK;
			token->length = 1;
			(c = source[++offset]);
			continue;
		}
		else if((c >= '0' && c <= '9')) {
			token->type = CIO_TOKEN_NUMBER;
			do {
				(c = source[++offset]);
			} while((c >= '0' && c <= '9'));
			token->length = offset - token->offset;
			continue;
		}
		else {
			token->type = CIO_TOKEN_IDENTIFIER;
			do {
				(c = source[++offset]);
			} while(!(c == ' ' || c == '\t' || c == '\n' || c == '\0' || c == '\r'));
			token->length = offset - token->offset;
			continue;
		}
	} while(offset < source_length);

    tokens_cleanup = NULL;

	return NULL;
}
