// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+genstone@pm.me>

#include <cionom.h>
#include <genargs.h>
#include <genfs.h>

typedef struct {
	const char* file;
	bool debug;
	bool emit_bytecode;
	const char* bytecode_file;
} cio_cli_args_t;

static const char* const restrict switches[] = {"debug", "emit-bytecode"};

static void cio_cli_arg_handler(const gen_arg_type_t type, const size_t arg_n, const char* const restrict parameter, void* const restrict passthrough) {
	GEN_FRAME_BEGIN(cio_cli_arg_handler);

	cio_cli_args_t* const args = passthrough;

	switch(type) {
		case GEN_ARG_LONG: {
			switch(arg_n) {
				case 0: {
					if(parameter) {
						glogf(FATAL, "`%s` does not take a parameter", switches[arg_n]);
						GEN_REQUIRE_NO_REACH;
					}
					args->debug = true;
					break;
				}
				case 1: {
					if(!parameter) {
						glogf(FATAL, "`%s` expected output file", switches[arg_n]);
						GEN_REQUIRE_NO_REACH;
					}
					args->emit_bytecode = true;
					args->bytecode_file = parameter;
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
	error = gen_handle_read((unsigned char*) source, &source_handle, 0, source_length);
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
					glogf(DEBUG, "%zu. CIO_TOKEN_IDENTIFIER", i);
					break;
				}
				case CIO_TOKEN_BLOCK: {
					glogf(DEBUG, "%zu. CIO_TOKEN_BLOCK", i);
					break;
				}
				case CIO_TOKEN_NUMBER: {
					glogf(DEBUG, "%zu. CIO_TOKEN_NUMBER", i);
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
			glogf(DEBUG, "├ %s %zu", routine.identifier, routine.parameters);
			GEN_FOREACH(j, call, routine.calls_length, routine.calls) {
				glogf(DEBUG, "| ├ call %s", call.identifier);
				GEN_FOREACH(k, parameter, call.parameters_length, call.parameters) {
					glogf(DEBUG, "| | ├ %zu", parameter);
				}
			}
		}
	}

	unsigned char* bytecode = NULL;
	size_t bytecode_length = 0;
	error = cio_emit_bytecode(&program, &bytecode, &bytecode_length, source, source_length, args.file, filename_length);
	GEN_REQUIRE_NO_ERROR(error);

	if(args.emit_bytecode) {
		gen_filesystem_handle_t bytecode_file = {0};
		bool exists = false;
		error = gen_path_exists(args.bytecode_file, &exists);
		GEN_REQUIRE_NO_ERROR(error);
		if(!exists) {
			error = gen_path_create_file(args.bytecode_file);
			GEN_REQUIRE_NO_ERROR(error);
		}
		error = gen_handle_open(&bytecode_file, args.bytecode_file);
		GEN_REQUIRE_NO_ERROR(error);
		error = gen_handle_write(&bytecode_file, bytecode_length, bytecode);
		GEN_REQUIRE_NO_ERROR(error);
		error = gen_handle_close(&bytecode_file);
		GEN_REQUIRE_NO_ERROR(error);
	}

	cio_vm_t vm = {0};
	error = cio_vm_initialize_bytecode(bytecode, bytecode_length, 64, &vm);
	GEN_REQUIRE_NO_ERROR(error);

	error = cio_vm_push_frame(&vm);
	GEN_REQUIRE_NO_ERROR(error);
	error = cio_vm_push(&vm);
	GEN_REQUIRE_NO_ERROR(error);
	error = cio_vm_dispatch_call(&vm, 3, 0);
	GEN_REQUIRE_NO_ERROR(error);

	error = gfree(bytecode);
	GEN_REQUIRE_NO_ERROR(error);
	error = cio_free_program(&program);
	GEN_REQUIRE_NO_ERROR(error);
	error = gfree(tokens);
	GEN_REQUIRE_NO_ERROR(error);
	error = gfree(source);
	GEN_REQUIRE_NO_ERROR(error);
}
