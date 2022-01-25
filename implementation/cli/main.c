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

	size_t filename_length = 0;
	error = gen_string_length(args.file, GEN_PATH_MAX + 1, GEN_PATH_MAX, &filename_length);
	GEN_REQUIRE_NO_ERROR(error);

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
	size_t tokens_length = 0;
	error = cio_tokenize(source, source_length, &tokens, &tokens_length);
	GEN_REQUIRE_NO_ERROR(error);

	if(args.debug) {
		GEN_FOREACH_PTR(i, token, tokens_length, tokens) {
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

	cio_program_t program = {0};
	error = cio_parse(tokens, tokens_length, &program, source, source_length, args.file, filename_length);
	GEN_REQUIRE_NO_ERROR(error);

	if(args.debug) {
		glogf(DEBUG, "%s", args.file);
		GEN_FOREACH(i, routine, program.routines_length, program.routines) {
			glogf(DEBUG, "├ %s", routine.identifier);
			glog(DEBUG, "| ├ parameters:");
			GEN_FOREACH(j, parameter, routine.parameters_length, routine.parameters) {
				glogf(DEBUG, "| | ├ %s size: %zu alignment: %zu", parameter.identifier, parameter.size, parameter.alignment);
			}
			glog(DEBUG, "| ├ statements:");
			GEN_FOREACH(j, statement, routine.statements_length, routine.statements) {
				switch(statement.type) {
					case CIO_STATEMENT_STORAGE: {
						glogf(DEBUG, "| | ├ %s size: %zu alignment: %zu", statement.storage.identifier, statement.storage.size, statement.storage.alignment);
						break;
					}
					case CIO_STATEMENT_CALL: {
						glog(DEBUG, "| | ├ statement");
						break;
					}
					case CIO_STATEMENT_RETURN: {
						glog(DEBUG, "| | ├ return");
						break;
					}
				}
			}
		}
	}
}
