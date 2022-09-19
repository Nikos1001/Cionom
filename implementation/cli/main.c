// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

#include <cionom.h>

#include <genmemory.h>
#include <genstring.h>
#include <genarguments.h>
#include <genfilesystem.h>
#include <genlog.h>

#ifndef CIO_CLI_STACK_LENGTH_FALLBACK
#define CIO_CLI_STACK_LENGTH_FALLBACK 1024
#endif

#ifndef CIO_CLI_BYTECODE_FILE_FALLBACK
#define CIO_CLI_BYTECODE_FILE_FALLBACK "a.ibc"
#endif

typedef enum {
    CIO_CLI_OPERATION_NONE,
    CIO_CLI_OPERATION_COMPILE,
    CIO_CLI_OPERATION_EXECUTE,
    CIO_CLI_OPERATION_MANGLE
} cio_cli_operation_t;

typedef enum {
    CIO_CLI_SWITCH_EMIT_BYTECODE,
    CIO_CLI_SWITCH_EXECUTE_BYTECODE,
    CIO_CLI_SWITCH_MANGLE_IDENTIFIER,
    CIO_CLI_SWITCH_STACK_LENGTH,
} cio_cli_switch_t;

static gen_error_t* gen_main(const size_t argc, const char* const restrict* const restrict argv) {
    GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) gen_main, GEN_FILE_NAME);
    if(error) return error;

    // TODO: `--bundle` - Emit a bundled executable for the provided list of bytecode files

    // TODO: `--seeking-execute` - Execute bytecode using a seek-based file consumption mode for VM to allow for very large files

    // TODO: `--disassemble` - Disassemble a bytecode file
    // TODO: `--assemble` - Assemble a bytecode file from bytecode assembly. Can use same tokenizer.

    // TODO: `--demangle-identifier` - Demangle a mangled identifier

    // TODO: `--debug` - Insert debug info
    //                   Treat `push0x7F` as a breakpoint
    //                   Show source if debug info is present on error; Otherwise dissassembly
    // TODO: `--help` - Print help menu
    // TODO: `--version` - Print version information
    // TODO: `--verbose` - Verbose output

    // TODO: `--mapfile` - Redirect exported routine names to be different to their internal names based on a file. Also contains list of files for constant data under `--extension=constants`

    // TODO: `--no-extension-encoding` - Treat the reserved encoding `push 0x7F` as a no-op instead of using it as an extension marker and remove all pushed entries preceeding it
    // TODO: `--no-extension-marker` - Ignore extensions in the bytecode header

    // TODO: `--extension=elide-reserve-space` - Allow calls to be prefixed with `::` to prevent the creation of reserve space
    // TODO: `--extension=bytecode-intrinsics` - Enable the use of `__cionom_push`, `__cionom_call`, `__cionom_return`
    //                                           and `__cionom_reserved_push0x7F` in code for direct control of bytecode
    //                                           emission (Maybe inline ASM would be better suited here)
    // TODO: `--extension=encode-default-routine` - Encodes the default entry point routine for the program in
    //                                              emitted bytecode
    // TODO: `--extension=elide-parameter-count` - Allow the emission of parameter counts on routine declarations/definitions
    // TODO: `--extension=constants` - Allows the insertion of files' contents into the module header. Also enables the use of `__cionom_constant*` (Gets a pointer to the constant data at an index)
    // TODO: `--extension=nil-calls` - Enable the use of `__cionom_nil_call` (Full no-op call, leaves parameters on stack) and `__cionom_nil_call_frame` (Partial no-op call, removes parameters from stack) - must be declared (goes into header extension data)
    // TODO: `--extension=preprocessor` - Enables a preprocessing step whereby files can be included and text patterns can be replaced (`|include` and `|macro`). Also allow the use of `||` to ignore the remainder of a line

    // TODO: `--fatal-warnings` - Treat warnings as fatal errors
    // TODO: `--warning=implicit-switch` - Warn for implicit switches such as stack length or file name
    // TODO: `--warning=erroneous-switch` - Warn for switches passed in scenarios where they take no effect
    // TODO: `--warning=emit-reserved-encoding` - Warn for calls which result in the reserved encoding `push 0x7F`
    // TODO: `--warning=reserved-identifier` - Warn for declaring routines which contain `__cionom` in their identifier
    // TODO: `--warning=parameter-overflow` - Warn for calls which provide a literal greater than the maximum encodable value `0x7F`
    // TODO: `--warning=header-extension` - Warn for bytecode which contains extensions in its header
    // TODO: `--warning=bytecode-extension` - Warn for bytecode which contains extensions during execution
    // TODO: `--warning=parameter-count-mismatch` - Warn for calls which provide more parameters than the declaration/definition specifies
    // TODO: `--warning=unmarked-extension` -  Warn for bytecode which makes use of extensions not denoted in the header
    // TODO: `--warning=duplicate-extension` - Warn for bytecode which denotes an extension multiple times in the header where doing so has no effect
    // TODO: `--warning=consume-reserved-encoding` - Warn for consumption of the reserved encoding `push 0x7F`
    // TODO: `--warning=routine-declared-defined` - Warn for routines which are both declared and defined in the same file

    if(!(argc - 1)) return gen_error_attach_backtrace(GEN_ERROR_TOO_SHORT, GEN_LINE_NUMBER, "No parameters specified");

    size_t* argument_lengths = NULL;
    error = gen_memory_allocate_zeroed((void**) &argument_lengths, argc - 1, sizeof(size_t));
	if(error) return error;

    for(size_t i = 0; i < argc - 1; ++i) {
        error = gen_string_length((argv + 1)[i], GEN_STRING_NO_BOUNDS, GEN_STRING_NO_BOUNDS, &argument_lengths[i]);
    	if(error) return error;
    }

    static const char* const restrict switches[] = {
        [CIO_CLI_SWITCH_EMIT_BYTECODE] = "emit-bytecode",
        [CIO_CLI_SWITCH_EXECUTE_BYTECODE] = "execute-bytecode",
        [CIO_CLI_SWITCH_MANGLE_IDENTIFIER] = "mangle-identifier",
        [CIO_CLI_SWITCH_STACK_LENGTH] = "stack-length"};

    static const size_t switches_lengths[] = {
        [CIO_CLI_SWITCH_EMIT_BYTECODE] = sizeof("emit-bytecode") - 1,
        [CIO_CLI_SWITCH_EXECUTE_BYTECODE] = sizeof("execute-bytecode") - 1,
        [CIO_CLI_SWITCH_MANGLE_IDENTIFIER] = sizeof("mangle-identifier") - 1,
        [CIO_CLI_SWITCH_STACK_LENGTH] = sizeof("stack-length") - 1};

    gen_arguments_parsed_t parsed = {0};
    error = gen_arguments_parse(argv + 1, argument_lengths, argc - 1, NULL, 0, switches, switches_lengths, sizeof(switches) / sizeof(char*), &parsed);
	if(error) return error;

    size_t stack_length = SIZE_MAX;
    size_t routine_index = 0;
    const char* file = NULL;
    size_t file_length = 0;

    cio_cli_operation_t operation = CIO_CLI_OPERATION_NONE;

    for(size_t i = 0; i < parsed.long_argument_count; ++i) {
        switch(parsed.long_argument_indices[i]) {
            case CIO_CLI_SWITCH_EMIT_BYTECODE: {
                if(operation) return gen_error_attach_backtrace(GEN_ERROR_BAD_OPERATION, GEN_LINE_NUMBER, "Multiple operations specified");

                if(!parsed.long_argument_parameters[i]) {
                    error = gen_log_formatted(GEN_LOG_LEVEL_WARNING, "cionom-cli", "`--%t` parameter not specified, defaulting to %t", switches[CIO_CLI_SWITCH_EMIT_BYTECODE], CIO_CLI_BYTECODE_FILE_FALLBACK);
                    if(error) return error;

                    file = CIO_CLI_BYTECODE_FILE_FALLBACK;
                    file_length = sizeof(CIO_CLI_BYTECODE_FILE_FALLBACK) - 1;
                }
                else {
                    file = parsed.long_argument_parameters[i];
                    file_length = parsed.long_argument_parameter_lengths[i];
                }

                operation = CIO_CLI_OPERATION_COMPILE;

                break;
            }
            
            case CIO_CLI_SWITCH_EXECUTE_BYTECODE: {
                if(!parsed.long_argument_parameters[i]) return gen_error_attach_backtrace_formatted(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`--%t` expected a parameter", switches[parsed.long_argument_indices[i]]);
                if(operation) return gen_error_attach_backtrace(GEN_ERROR_BAD_OPERATION, GEN_LINE_NUMBER, "Multiple operations specified");

                operation = CIO_CLI_OPERATION_EXECUTE;
                error = gen_string_number(parsed.long_argument_parameters[i], parsed.long_argument_parameter_lengths[i] + 1, GEN_STRING_NO_BOUNDS, &routine_index);
                if(error) return error;

                break;
            }
            case CIO_CLI_SWITCH_MANGLE_IDENTIFIER: {
                if(operation) return gen_error_attach_backtrace(GEN_ERROR_BAD_OPERATION, GEN_LINE_NUMBER, "Multiple operations specified");

                operation = CIO_CLI_OPERATION_MANGLE;

                break;
            }
            case CIO_CLI_SWITCH_STACK_LENGTH: {
                if(!parsed.long_argument_parameters[i]) return gen_error_attach_backtrace_formatted(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`--%t` expected a parameter", switches[parsed.long_argument_indices[i]]);
                if(stack_length != SIZE_MAX) return gen_error_attach_backtrace_formatted(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`--%t` specified multiple times", switches[parsed.long_argument_indices[i]]);

                error = gen_string_number(parsed.long_argument_parameters[i], parsed.long_argument_parameter_lengths[i] + 1, GEN_STRING_NO_BOUNDS, &stack_length);
                if(error) return error;

                break;
            }
            default: return gen_error_attach_backtrace(GEN_ERROR_UNKNOWN, GEN_LINE_NUMBER, "Something went wrong while parsing arguments");
        }
    }

    switch(operation) {
        case CIO_CLI_OPERATION_NONE: return gen_error_attach_backtrace(GEN_ERROR_BAD_OPERATION, GEN_LINE_NUMBER, "No operation specified");
        case CIO_CLI_OPERATION_COMPILE: {
            if(parsed.raw_argument_count > 1) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "Multiple files specified");
            if(!parsed.raw_argument_count) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "No source file specified");

            const char* source_file = (argv + 1)[parsed.raw_argument_indices[0]];

            if(stack_length != SIZE_MAX) {
                error = gen_log_formatted(GEN_LOG_LEVEL_WARNING, "cionom-cli", "`--%t` specified to `--%t`", switches[CIO_CLI_SWITCH_STACK_LENGTH], switches[CIO_CLI_SWITCH_EMIT_BYTECODE]);
                if(error) return error;
            }

            size_t filename_length = 0;
            error = gen_string_length(source_file, GEN_STRING_NO_BOUNDS, GEN_STRING_NO_BOUNDS, &filename_length);
            if(error) return error;

            gen_filesystem_handle_t source_handle = {0};
            error = gen_filesystem_handle_open(source_file, argument_lengths[parsed.raw_argument_indices[0]], &source_handle);
            if(error) return error;
            size_t source_length = 0;
            error = gen_filesystem_handle_file_size(&source_handle, &source_length);
            if(error) return error;
            char* source = NULL;
            error = gen_memory_allocate_zeroed((void**) &source, source_length + 1, sizeof(char));
            if(error) return error;
            error = gen_filesystem_handle_file_read(&source_handle, 0, source_length, (unsigned char*) source);
            if(error) return error;

			cio_token_t* tokens = NULL;
			size_t tokens_length = 0;
			error = cio_tokenize(source, source_length, &tokens, &tokens_length);
			if(error) return error;

			cio_program_t program = {0};
			error = cio_parse(tokens, tokens_length, &program, source, source_length, source_file, filename_length);
			if(error) return error;

			unsigned char* bytecode = NULL;
			size_t bytecode_length = 0;
			error = cio_emit_bytecode(&program, &bytecode, &bytecode_length, source, source_length, source_file, filename_length);
			if(error) return error;

			gen_filesystem_handle_t bytecode_file = {0};
			bool exists = false;
			error = gen_filesystem_path_exists(file, file_length, &exists);
			if(error) return error;
			if(!exists) {
				error = gen_filesystem_path_create_file(file, file_length);
				if(error) return error;
			}
			else {
				error = gen_filesystem_path_delete(file, file_length);
				if(error) return error;
				error = gen_filesystem_path_create_file(file, file_length);
				if(error) return error;
			}
			if(bytecode) {
				error = gen_filesystem_handle_open(file, file_length, &bytecode_file);
				if(error) return error;
				error = gen_filesystem_handle_file_write(&bytecode_file, bytecode, bytecode_length);
				if(error) return error;
				error = gen_filesystem_handle_close(&bytecode_file);
				if(error) return error;
            }

            break;
        }
        case CIO_CLI_OPERATION_EXECUTE: {
            if(parsed.raw_argument_count > 1) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "Multiple files specified");

            const char* bytecode_file = NULL;
            size_t bytecode_file_length = 0;

            if(!parsed.raw_argument_count) {
                error = gen_log_formatted(GEN_LOG_LEVEL_WARNING, "cionom-cli", "File not specified, defaulting to %t", CIO_CLI_BYTECODE_FILE_FALLBACK);
                if(error) return error;

                bytecode_file = CIO_CLI_BYTECODE_FILE_FALLBACK;
                bytecode_file_length = sizeof(CIO_CLI_BYTECODE_FILE_FALLBACK) - 1;
            }
            else {
                bytecode_file = (argv + 1)[parsed.raw_argument_indices[0]];
                bytecode_file_length = argument_lengths[parsed.raw_argument_indices[0]];
            }

            if(stack_length == SIZE_MAX) {
                error = gen_log_formatted(GEN_LOG_LEVEL_WARNING, "cionom-cli", "`--%t` not specified, defaulting to %uz", switches[CIO_CLI_SWITCH_STACK_LENGTH], CIO_CLI_STACK_LENGTH_FALLBACK);
                if(error) return error;

                stack_length = CIO_CLI_STACK_LENGTH_FALLBACK;
            }

            gen_filesystem_handle_t source_handle = {0};
            error = gen_filesystem_handle_open(bytecode_file, bytecode_file_length, &source_handle);
            if(error) return error;
            size_t source_length = 0;
            error = gen_filesystem_handle_file_size(&source_handle, &source_length);
            if(error) return error;
            char* source = NULL;
            error = gen_memory_allocate_zeroed((void**) &source, source_length + 1, sizeof(char));
            if(error) return error;
            error = gen_filesystem_handle_file_read(&source_handle, 0, source_length, (unsigned char*) source);
            if(error) return error;

			cio_vm_t vm = {0};
			error = cio_vm_bundle_initialize((unsigned char*) source, source_length, stack_length, &vm);
			if(error) return error;

			error = cio_vm_push_frame(&vm);
			if(error) return error;
			error = cio_vm_push(&vm);
			if(error) return error;
			error = cio_vm_dispatch_call(&vm, routine_index, 0);
			if(error) return error;

            break;
        }
        case CIO_CLI_OPERATION_MANGLE: {
            if(!parsed.raw_argument_count) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "No identifer specified");
            if(parsed.raw_argument_count > 1) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "Multiple identifers specified");

            char* mangled = NULL;
            error = cio_mangle_identifier((argv + 1)[parsed.raw_argument_indices[0]], &mangled);
            if(error) return error;

            error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom-cli", "Result of mangling \"%t\" is: `%t`", (argv + 1)[parsed.raw_argument_indices[0]], mangled);
            if(error) return error;

            break;
        }
    }

    return NULL;
}

int main(const int argc, const char* const* const argv) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) main, GEN_FILE_NAME);
	if(error) {
        gen_error_print("cionom-cli", error, GEN_ERROR_SEVERITY_FATAL);
        gen_error_abort();
    }

    error = gen_main((size_t) argc, argv);
    if(error) {
        gen_error_print("cionom-cli", error, GEN_ERROR_SEVERITY_FATAL);
        gen_error_abort();
    }
}
