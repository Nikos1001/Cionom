// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+genstone@pm.me>

#include <cionom.h>
#include <genargs.h>
#include <genfs.h>

typedef struct {
	const char* file;
	bool debug;
} cio_cli_args_t;

static const char* const restrict switches[] = {"debug"};

static void cio_cli_arg_handler(const gen_arg_type_t type, const size_t arg_n, const char* const restrict parameter, void* const restrict passthrough) {
	cio_cli_args_t* const args = passthrough;

	switch(type) {
		case GEN_ARG_LONG: {
			switch(arg_n) {
				case 0: {
					args->debug = true;
					break;
				}
			}
			break;
		}
		case GEN_ARG_SHORT: {
			break;
		}
		case GEN_ARG_RAW: {
			if(!args->file)
				args->file = parameter;
			else {
				glog(FATAL, "Passing multiple files is not permitted");
				GEN_REQUIRE_NO_REACH;
			}
			break;
		}
	}
}

int main(const int argc, const char* const* const argv) {
	GEN_FRAME_BEGIN(main);

	cio_cli_args_t args = {0};

	gen_error_t error = gen_parse_args(argc, argv, cio_cli_arg_handler, 0, NULL, sizeof(switches) / sizeof(switches[0]), switches, &args);
	GEN_REQUIRE_NO_ERROR(error);

	if(!args.file) {
		glog(FATAL, "No source file specified");
		GEN_REQUIRE_NO_REACH;
	}

	gen_filesystem_handle_t source_handle = {0};
	error = gen_handle_open(&source_handle, args.file);
	GEN_REQUIRE_NO_ERROR(error);
	size_t source_length = 0;
	error = gen_handle_size(&source_length, &source_handle);
	GEN_REQUIRE_NO_ERROR(error);
	char* source = NULL;
	error = gzalloc((void**) &source, source_length + 1, sizeof(char));
	GEN_REQUIRE_NO_ERROR(error);
	error = gen_handle_read((uint8_t*) source, &source_handle, 0, source_length);
	GEN_REQUIRE_NO_ERROR(error);
	error = gen_handle_close(&source_handle);
	GEN_REQUIRE_NO_ERROR(error);

	cio_token_t* tokens = NULL;
	size_t n_tokens = 0;
	error = cio_tokenize(source, &tokens, &n_tokens);
	GEN_REQUIRE_NO_ERROR(error);

	if(args.debug) {
		GEN_FOREACH_PTR(i, token, n_tokens, tokens) {
			switch(token->type) {
				case CIO_TOKEN_IDENTIFIER: {
					glog(DEBUG, "CIO_TOKEN_IDENTIFIER");
					break;
				}
				case CIO_TOKEN_RETURN: {
					glog(DEBUG, "CIO_TOKEN_RETURN");
					break;
				}
				case CIO_TOKEN_STORAGE: {
					glog(DEBUG, "CIO_TOKEN_STORAGE");
					break;
				}
				case CIO_TOKEN_ALIGNMENT: {
					glog(DEBUG, "CIO_TOKEN_ALIGNMENT");
					break;
				}
				case CIO_TOKEN_BLOCK_START: {
					glog(DEBUG, "CIO_TOKEN_BLOCK_START");
					break;
				}
				case CIO_TOKEN_BLOCK_END: {
					glog(DEBUG, "CIO_TOKEN_BLOCK_END");
					break;
				}
				case CIO_TOKEN_SPECIFIER_EXPRESSION_START: {
					glog(DEBUG, "CIO_TOKEN_SPECIFIER_EXPRESSION_START");
					break;
				}
				case CIO_TOKEN_SPECIFIER_EXPRESSION_END: {
					glog(DEBUG, "CIO_TOKEN_SPECIFIER_EXPRESSION_END");
					break;
				}
				case CIO_TOKEN_STATEMENT_DELIMITER: {
					glog(DEBUG, "CIO_TOKEN_STATEMENT_DELIMITER");
					break;
				}
				case CIO_TOKEN_PARAMETER_DELIMITER: {
					glog(DEBUG, "CIO_TOKEN_PARAMETER_DELIMITER");
					break;
				}
				case CIO_TOKEN_NUMBER: {
					glog(DEBUG, "CIO_TOKEN_NUMBER");
					break;
				}
				case CIO_TOKEN_STRING: {
					glog(DEBUG, "CIO_TOKEN_STRING");
					break;
				}
			}
		}
	}
}
