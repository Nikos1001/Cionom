// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+cionom@pm.me>

#include <cionom.h>
#include <genargs.h>
#include <genfs.h>

typedef struct {
	const char* file;
	bool debug;
	bool emit_bytecode;
	const char* bytecode_file;
	bool execute_bytecode;
	size_t routine;
	size_t stack_length;
	bool print_mangled_identifier;
	const char* identifier;
} cio_cli_args_t;

static const char* const restrict switches[] = {"debug", "emit-bytecode", "execute-bytecode", "stack-length", "print-mangled-identifier"};

static __nodiscard gen_error_t cio_cli_arg_handler(const gen_arg_type_t type, const size_t arg_n, const char* const restrict parameter, void* const restrict passthrough) {
	GEN_FRAME_BEGIN(cio_cli_arg_handler);

	cio_cli_args_t* const args = passthrough;

	switch(type) {
		case GEN_ARG_LONG: {
			switch(arg_n) {
				case 0: {
					if(parameter) GEN_ERROR_OUT(GEN_BAD_CONTENT, "`--debug` does not take a parameter");
					args->debug = true;
					break;
				}
				case 1: {
					args->emit_bytecode = true;
					args->bytecode_file = parameter ?: "a.ibc";
					if(!parameter) glog(WARNING, "No output file specified to `--emit-bytecode`, using `a.ibc`");
					break;
				}
				case 2: {
					if(!parameter) GEN_ERROR_OUT(GEN_BAD_CONTENT, "`--execute-bytecode` expected routine index");
					args->execute_bytecode = true;
					gen_error_t error = gen_string_number(parameter, GEN_STRING_NO_BOUND, GEN_STRING_NO_BOUND, &args->routine);
					GEN_ERROR_OUT_IF(error, "`gen_string_number` failed");
					break;
				}
				case 3: {
					if(!parameter) GEN_ERROR_OUT(GEN_BAD_CONTENT, "`--stack-length` expected stack length");
					gen_error_t error = gen_string_number(parameter, GEN_STRING_NO_BOUND, GEN_STRING_NO_BOUND, &args->stack_length);
					GEN_ERROR_OUT_IF(error, "`gen_string_number` failed");
					break;
				}
				case 4: {
					if(!parameter) GEN_ERROR_OUT(GEN_BAD_CONTENT, "`--print-mangled-identifier` expected identifier to mangle");
					args->print_mangled_identifier = true;
					args->identifier = parameter;
					break;
				}
				default: {
					GEN_ERROR_OUT(GEN_BAD_CONTENT, "Unrecognized parameter");
				}
			}
			break;
		}
		case GEN_ARG_SHORT: {
			GEN_ERROR_OUT(GEN_BAD_CONTENT, "Unrecognized parameter");
		}
		case GEN_ARG_RAW: {
			if(!args->file)
				args->file = parameter;
			else
				GEN_ERROR_OUT(GEN_TOO_LONG, "Passing multiple files is not permitted");
			break;
		}
	}

	GEN_ALL_OK;
}

int main(const int argc, const char* const* const argv) {
	GEN_FRAME_BEGIN(main);

	cio_cli_args_t args = {0};

	gen_error_t error = gen_parse_args(argc, argv, cio_cli_arg_handler, 0, NULL, sizeof(switches) / sizeof(switches[0]), switches, &args);
	GEN_REQUIRE_NO_ERROR(error);

	if(!args.file && !args.print_mangled_identifier) {
		glog(FATAL, "No bytecode file specified");
		GEN_REQUIRE_NO_REACH;
	}
	else if(args.file && args.print_mangled_identifier) {
		glog(FATAL, "Source file specified when using `--print-mangled-identifier`");
		GEN_REQUIRE_NO_REACH;
	}
	size_t operations = 0;
	operations += args.emit_bytecode;
	operations += args.execute_bytecode;
	operations += args.print_mangled_identifier;
	if(operations > 1) {
		glog(FATAL, "Cannot perform multiple operations in the same invocation");
		GEN_REQUIRE_NO_REACH;
	}
	else if(operations < 1) {
		glog(FATAL, "No operation specified");
		GEN_REQUIRE_NO_REACH;
	}

	if(!args.stack_length && args.execute_bytecode) {
		args.stack_length = 1024;
		glogf(WARNING, "`--stack-length` not specified, defaulting to %zu", args.stack_length);
	}
	else if(args.stack_length && !args.execute_bytecode) {
		glog(FATAL, "`--stack-length` specified for non-executing operation");
		GEN_REQUIRE_NO_REACH;
	}

	if(args.print_mangled_identifier) {
		char* mangled = NULL;
		error = cio_mangle_identifier(args.identifier, &mangled);
		GEN_REQUIRE_NO_ERROR(error);
		glogf(INFO, "Result of mangling %s is: `%s`", args.identifier, mangled);
		error = gfree(mangled);
		GEN_REQUIRE_NO_ERROR(error);
	}
	else {
		size_t filename_length = 0;
		error = gen_string_length(args.file, GEN_PATH_MAX + 1, GEN_PATH_MAX, &filename_length);
		GEN_REQUIRE_NO_ERROR(error);

		gen_filesystem_handle_t source_handle = {0};
		error = gen_filesystem_handle_open(&source_handle, args.file);
		GEN_REQUIRE_NO_ERROR(error);
		size_t source_length = 0;
		error = gen_filesystem_handle_size(&source_length, &source_handle);
		GEN_REQUIRE_NO_ERROR(error);
		char* source = NULL;
		error = gzalloc((void**) &source, source_length + 1, sizeof(char));
		GEN_REQUIRE_NO_ERROR(error);
		error = gen_filesystem_handle_read((unsigned char*) source, &source_handle, 0, source_length);
		GEN_REQUIRE_NO_ERROR(error);
		error = gen_filesystem_handle_close(&source_handle);
		GEN_REQUIRE_NO_ERROR(error);

		if(args.emit_bytecode) {
			cio_token_t* tokens = NULL;
			size_t tokens_length = 0;
			error = cio_tokenize(source, source_length, &tokens, &tokens_length);
			GEN_REQUIRE_NO_ERROR(error);

			cio_program_t program = {0};
			error = cio_parse(tokens, tokens_length, &program, source, source_length, args.file, filename_length);
			GEN_REQUIRE_NO_ERROR(error);

			unsigned char* bytecode = NULL;
			size_t bytecode_length = 0;
			error = cio_emit_bytecode(&program, &bytecode, &bytecode_length, source, source_length, args.file, filename_length);
			GEN_REQUIRE_NO_ERROR(error);

			gen_filesystem_handle_t bytecode_file = {0};
			bool exists = false;
			error = gen_path_exists(args.bytecode_file, &exists);
			GEN_REQUIRE_NO_ERROR(error);
			if(!exists) {
				error = gen_path_create_file(args.bytecode_file);
				GEN_REQUIRE_NO_ERROR(error);
			}
			else {
				error = gen_path_delete(args.bytecode_file);
				GEN_REQUIRE_NO_ERROR(error);
				error = gen_path_create_file(args.bytecode_file);
				GEN_REQUIRE_NO_ERROR(error);
			}
			if(bytecode) {
				error = gen_filesystem_handle_open(&bytecode_file, args.bytecode_file);
				GEN_REQUIRE_NO_ERROR(error);
				error = gen_filesystem_handle_write(&bytecode_file, bytecode_length, bytecode);
				GEN_REQUIRE_NO_ERROR(error);
				error = gen_filesystem_handle_close(&bytecode_file);
				GEN_REQUIRE_NO_ERROR(error);

				error = gfree(bytecode);
				GEN_REQUIRE_NO_ERROR(error);
			}

			error = cio_free_program(&program);
			GEN_REQUIRE_NO_ERROR(error);

			if(tokens) {
				error = gfree(tokens);
				GEN_REQUIRE_NO_ERROR(error);
			}
		}
		else if(args.execute_bytecode) {
			cio_vm_t vm = {0};
			error = cio_vm_initialize_bytecode((unsigned char*) source, source_length, args.stack_length, &vm);
			GEN_REQUIRE_NO_ERROR(error);

			error = cio_vm_push_frame(&vm);
			GEN_REQUIRE_NO_ERROR(error);
			error = cio_vm_push(&vm);
			GEN_REQUIRE_NO_ERROR(error);
			error = cio_vm_dispatch_call(&vm, args.routine, 0);
			GEN_REQUIRE_NO_ERROR(error);

			error = cio_free_vm(&vm);
			GEN_REQUIRE_NO_ERROR(error);
		}

		error = gfree(source);
		GEN_REQUIRE_NO_ERROR(error);
	}
}
