// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

#include "include/cionom.h"

#include <genstring.h>
#include <genmemory.h>
#include <genlog.h>

static gen_error_t* cio_internal_parse_expect(const cio_token_t* const restrict token, const cio_token_type_t expected, const char* const restrict source, const size_t source_length, const char* const restrict source_file, const size_t source_file_length) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_internal_parse_expect, GEN_FILE_NAME);
	if(error) return error;

	if(!token) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`token` was `NULL`");
	if(!source) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`source` was `NULL`");
	if(!source_file) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`source_file` was `NULL`");

	if(token->type != expected) {
		size_t line = 0;
		size_t column = 0;
		error = cio_line_from_offset(token->offset, &line, source, source_length);
		if(error) return error;
		error = cio_column_from_offset(token->offset, &column, source, source_length);
		if(error) return error;
		error = gen_log_formatted(GEN_LOG_LEVEL_ERROR, "cionom", "Malformed program: Unexpected token %tz:%uz:%uz", source_file, source_file_length, line, column);
		if(error) return error;
		return gen_error_attach_backtrace(GEN_ERROR_BAD_CONTENT, GEN_LINE_NUMBER, "Parsing failed");
	}

	return NULL;
}

static void cio_parse_cleanup_program(cio_program_t** program) {
    if(!*program) return;

    gen_error_t* error = cio_free_program(*program);
    if(error) {
        gen_error_print("cionom", error, GEN_ERROR_SEVERITY_FATAL);
        gen_error_abort();
    }
}

gen_error_t* cio_parse(const cio_token_t* const restrict tokens, const size_t tokens_length, cio_program_t* const restrict out_program, const char* const restrict source, const size_t source_length, const char* const restrict source_file, const size_t source_file_length) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_parse, GEN_FILE_NAME);
	if(error) return error;

	if(!out_program) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`out_program` was `NULL`");
	if(!source) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`source` was `NULL`");
	if(!source_file) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`source_file` was `NULL`");

	if(!tokens) return NULL;

    GEN_CLEANUP_FUNCTION(cio_parse_cleanup_program) cio_program_t* program_cleanup = out_program;

    for(size_t i = 0; i < tokens_length; ++i) {
        const cio_token_t* token = &tokens[i];

		error = cio_internal_parse_expect(token, CIO_TOKEN_IDENTIFIER, source, source_length, source_file, source_file_length);
		if(error) return error;
		error = gen_memory_reallocate_zeroed((void**) &out_program->routines, out_program->routines_length, out_program->routines_length + 1, sizeof(cio_routine_t));
		if(error) return error;
		++out_program->routines_length;
		cio_routine_t* const routine = &out_program->routines[out_program->routines_length - 1];
		routine->token = token;
		error = gen_string_duplicate(source + token->offset, source_length - token->offset, token->length, &routine->identifier, NULL /* TODO: Identifier length */);
		if(error) return error;

        if(!(i + 1 < tokens_length)) return gen_error_attach_backtrace(GEN_ERROR_TOO_SHORT, GEN_LINE_NUMBER, "Unexpected EOF");
        token = &tokens[++i];

		error = cio_internal_parse_expect(token, CIO_TOKEN_NUMBER, source, source_length, source_file, source_file_length);
		if(error) return error;
		error = gen_string_number(source + token->offset, source_length - token->offset, token->length, &routine->parameters);
		if(error) return error;

		if(!(i < tokens_length) || tokens[i + 1].type != CIO_TOKEN_BLOCK) {
			routine->external = true;
			continue;
		}

        if(!(i + 2 < tokens_length)) return gen_error_attach_backtrace(GEN_ERROR_TOO_SHORT, GEN_LINE_NUMBER, "Unexpected EOF");
        i += 2;
        token = &tokens[i];

		while(token->type != CIO_TOKEN_BLOCK) {
			error = cio_internal_parse_expect(token, CIO_TOKEN_IDENTIFIER, source, source_length, source_file, source_file_length);
			if(error) return error;
			error = gen_memory_reallocate_zeroed((void**) &routine->calls, routine->calls_length, routine->calls_length + 1, sizeof(cio_call_t));
			if(error) return error;
			++routine->calls_length;
			cio_call_t* const call = &routine->calls[routine->calls_length - 1];
			call->token = token;
			error = gen_string_duplicate(source + token->offset, source_length - token->offset, token->length, &call->identifier, NULL);
			if(error) return error;

            if(!(i + 1 < tokens_length)) return gen_error_attach_backtrace(GEN_ERROR_TOO_SHORT, GEN_LINE_NUMBER, "Unexpected EOF");
            token = &tokens[++i];

			while(token->type == CIO_TOKEN_NUMBER) {
				error = gen_memory_reallocate_zeroed((void**) &call->parameters, call->parameters_length, call->parameters_length + 1, sizeof(size_t));
				if(error) return error;
				++call->parameters_length;
				error = gen_string_number(source + token->offset, source_length - token->offset, token->length, &call->parameters[call->parameters_length - 1]);
				if(error) return error;

                if(!(i + 1 < tokens_length)) return gen_error_attach_backtrace(GEN_ERROR_TOO_SHORT, GEN_LINE_NUMBER, "Unexpected EOF");
                token = &tokens[++i];
			}
		}
	}

    program_cleanup = NULL;

	return NULL;
}

gen_error_t* cio_free_program(cio_program_t* const restrict program) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_free_program, GEN_FILE_NAME);
	if(error) return error;

	if(!program) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`program` was `NULL`");

    for(size_t i = 0; i < program->routines_length; ++i) {
        if(program->routines[i].identifier) {
            error = gen_memory_free((void**) &program->routines[i].identifier);
            if(error) return error;
        }        
        for(size_t j = 0; j < program->routines[i].calls_length; ++j) {
            if(program->routines[i].calls[j].identifier) {
                error = gen_memory_free((void**) &program->routines[i].calls[j].identifier);
                if(error) return error;
            }        
            if(program->routines[i].calls[j].parameters) {
                error = gen_memory_free((void**) &program->routines[i].calls[j].parameters);
                if(error) return error;
            }        
        }
    }

    if(program->routines) {
        error = gen_memory_free((void**) &program->routines);
        if(error) return error;
    }        

	return NULL;
}
