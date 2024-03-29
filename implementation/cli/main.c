// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

#include <cionom.h>

#include <genmemory.h>
#include <genstring.h>
#include <genarguments.h>
#include <genfilesystem.h>
#include <genlog.h>

#ifndef CIO_CLI_VERSION
#define CIO_CLI_VERSION "unknown"
#endif

#ifndef CIO_CLI_ENTRY_ROUTINE_FALLBACK
#define CIO_CLI_ENTRY_ROUTINE_FALLBACK "__cionom_entrypoint"
#endif

#ifndef CIO_CLI_STACK_LENGTH_FALLBACK
#define CIO_CLI_STACK_LENGTH_FALLBACK 1024
#endif

#ifndef CIO_CLI_BYTECODE_FILE_FALLBACK
#define CIO_CLI_BYTECODE_FILE_FALLBACK "a.ibc"
#endif

#ifndef CIO_CLI_ASM_FILE_FALLBACK
#define CIO_CLI_ASM_FILE_FALLBACK "a.cas"
#endif

#ifndef CIO_CLI_BUNDLE_FILE_FALLBACK
#define CIO_CLI_BUNDLE_FILE_FALLBACK "a.cbe"
#endif

typedef enum {
    CIO_CLI_OPERATION_NONE,
    CIO_CLI_OPERATION_COMPILE,
    CIO_CLI_OPERATION_EXECUTE,
    CIO_CLI_OPERATION_MANGLE,
    CIO_CLI_OPERATION_DISASSEMBLE,
    CIO_CLI_OPERATION_BUNDLE,
    CIO_CLI_OPERATION_DEBUNDLE,
    CIO_CLI_OPERATION_VERSION,
    CIO_CLI_OPERATION_HELP
} cio_cli_operation_t;

typedef enum {
    CIO_CLI_SWITCH_EMIT_BYTECODE,
    CIO_CLI_SWITCH_EXECUTE_BUNDLE,
    CIO_CLI_SWITCH_MANGLE_IDENTIFIER,
    CIO_CLI_SWITCH_STACK_LENGTH,
    CIO_CLI_SWITCH_DISASSEMBLE,
    CIO_CLI_SWITCH_BUNDLE,
    CIO_CLI_SWITCH_DEBUNDLE,
    CIO_CLI_SWITCH_VERSION,
    CIO_CLI_SWITCH_FATAL_WARNINGS,
    CIO_CLI_SWITCH_WARNING,
    CIO_CLI_SWITCH_HELP,
    CIO_CLI_SWITCH_DEBUG_VM
} cio_cli_switch_t;

static gen_error_t* cio_cli_read_file(const char* path, unsigned char** out_file, gen_size_t* out_size) {
    GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_cli_read_file, GEN_FILE_NAME);
    if(error) return error;

    if(!path) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`path` was `GEN_NULL`");
    if(!out_file) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`out_file` was `GEN_NULL`");
    if(!out_size) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`out_size` was `GEN_NULL`");
    
    gen_filesystem_handle_t handle = {0};
    error = gen_filesystem_handle_open(path, GEN_STRING_NO_BOUNDS, &handle);
    if(error) return error;

    error = gen_filesystem_handle_lock(&handle);
    if(error) return error;

    error = gen_filesystem_handle_file_size(&handle, out_size);
    if(error) return error;

    error = gen_memory_allocate_zeroed((void**) out_file, *out_size, sizeof(unsigned char));
    if(error) return error;

    error = gen_filesystem_handle_file_read(&handle, 0, *out_size, *out_file);
    if(error) return error;

    error = gen_filesystem_handle_unlock(&handle);
    if(error) return error;

    error = gen_filesystem_handle_close(&handle);
    if(error) return error;

    return GEN_NULL;
}

static gen_error_t* cio_cli_recreate_write_file(const char* path, const unsigned char* buffer, gen_size_t size) {
    GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_cli_read_file, GEN_FILE_NAME);
    if(error) return error;

    if(!path) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`path` was `GEN_NULL`");
    if(!buffer) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`buffer` was `GEN_NULL`");
    
    gen_bool_t exists = gen_false;
    error = gen_filesystem_path_exists(path, GEN_STRING_NO_BOUNDS, &exists);
    if(error) return error;

    if(!exists) {
        error = gen_filesystem_path_create_file(path, GEN_STRING_NO_BOUNDS);
        if(error) return error;
    }
    else {
        error = gen_filesystem_path_delete(path, GEN_STRING_NO_BOUNDS);
        if(error) return error;

        error = gen_filesystem_path_create_file(path, GEN_STRING_NO_BOUNDS);
        if(error) return error;
    }

    gen_filesystem_handle_t handle = {0};
    error = gen_filesystem_handle_open(path, GEN_STRING_NO_BOUNDS, &handle);
    if(error) return error;

    error = gen_filesystem_handle_lock(&handle);
    if(error) return error;

    error = gen_filesystem_handle_file_write(&handle, buffer, 0, size);
    if(error) return error;

    error = gen_filesystem_handle_unlock(&handle);
    if(error) return error;

    error = gen_filesystem_handle_close(&handle);
    if(error) return error;

    return GEN_NULL;
}

// TODO: Separate out main

static gen_error_t* gen_main(const gen_size_t argc, const char* const restrict* const restrict argv) {
    GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) gen_main, GEN_FILE_NAME);
    if(error) return error;

    // TODO: `--assemble` - Assemble a bytecode file from bytecode assembly. Can use same tokenizer.

    // TODO: `--demangle-identifier` - Demangle a mangled identifier

    // TODO: `--mapfile` - Redirect exported routine names to be different to their internal names based on a file. Also contains list of files for constant data under `--extension=constants`

    // TODO: `--no-extension-encoding` - Treat the reserved encoding `push 0x7F` as a no-op instead of using it as an extension marker and remove all pushed entries preceeding it
    // TODO: `--no-extension-marker` - Ignore extensions in the bytecode header

    // TODO: `--extension=elide_reserve-space` - Allow calls to be prefixed with `::` to prevent the creation of reserve space
    // TODO: `--extension=bytecode_intrinsics` - Enable the use of `__cionom_push`, `__cionom_call`, `__cionom_return`
    //                                           and `__cionom_reserved_push0x7F` in code for direct control of bytecode
    //                                           emission (Maybe inline ASM would be better suited here)
    // TODO: `--extension=encode_stack_length` - Encodes the desired stack length for the program in emitted bytecode
    // TODO: `--extension=elide_parameter_count` - Allow the emission of parameter counts on routine declarations/definitions
    // TODO: `--extension=constants` - Allows the insertion of files' contents into the module header. Also enables the use of `__cionom_constant*` (Gets a pointer to the constant data at an index)
    // TODO: `--extension=nil_calls` - Enable the use of `__cionom_nil_call` (Full no-op call, leaves parameters on stack) and `__cionom_nil_call_frame` (Partial no-op call, removes parameters from stack) - must be declared (goes into header extension data)
    // TODO: `--extension=preprocessor` - Enables a preprocessing step whereby files can be included and text patterns can be replaced (`|include` and `|macro`). Also allow the use of `||` to ignore the remainder of a line
    // TODO: `--extension=breakpoints` - Enables the use of breakpoints to call back to a debugger attached to a running program
    // TODO: `--extension=debug_info` - Allows the insertion of extra information about the program into the header to aid in debugging

    // TODO: `--warning=entrypoint_parameter` - Warn for entrypoints which have non-zero parameter counts
    // TODO: `--warning=routine_duplicated` - Warn for routines which are defined multiple times in an executable bundle
    // TODO: `--warning=routine_shadows_native` - Warn for routines which shadow a definition in native code
    // TODO: `--warning=routine_declared_defined` - Warn for routines which are both declared and defined in the same file
    // TODO: `--warning=header_extension` - Warn for bytecode which contains extensions in its header
    // TODO: `--warning=bytecode_extension` - Warn for bytecode which contains extensions during execution
    // TODO: `--warning=unmarked_extension` -  Warn for bytecode which makes use of extensions not denoted in the header
    // TODO: `--warning=duplicate_extension` - Warn for bytecode which denotes an extension multiple times in the header where doing so has no effect

    if(!(argc - 1)) {
        error = gen_log(GEN_LOG_LEVEL_FATAL, "cionom-cli", "No parameters specified");
        if(error) return error;

        return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "No parameters specified");
    }

    gen_size_t* argument_lengths = GEN_NULL;
    error = gen_memory_allocate_zeroed((void**) &argument_lengths, argc - 1, sizeof(gen_size_t));
	if(error) return error;

    for(gen_size_t i = 0; i < argc - 1; ++i) {
        error = gen_string_length((argv + 1)[i], GEN_STRING_NO_BOUNDS, GEN_STRING_NO_BOUNDS, &argument_lengths[i]);
    	if(error) return error;
    }

    static const char* const restrict switches[] = {
        [CIO_CLI_SWITCH_EMIT_BYTECODE] = "emit-bytecode",
        [CIO_CLI_SWITCH_EXECUTE_BUNDLE] = "execute-bundle",
        [CIO_CLI_SWITCH_MANGLE_IDENTIFIER] = "mangle-identifier",
        [CIO_CLI_SWITCH_STACK_LENGTH] = "stack-length",
        [CIO_CLI_SWITCH_DISASSEMBLE] = "disassemble",
        [CIO_CLI_SWITCH_BUNDLE] = "bundle",
        [CIO_CLI_SWITCH_DEBUNDLE] = "debundle",
        [CIO_CLI_SWITCH_VERSION] = "version",
        [CIO_CLI_SWITCH_FATAL_WARNINGS] = "fatal-warnings",
        [CIO_CLI_SWITCH_WARNING] = "warning",
        [CIO_CLI_SWITCH_HELP] = "help",
        [CIO_CLI_SWITCH_DEBUG_VM] = "debug-vm"
    };

    static const gen_size_t switches_lengths[] = {
        [CIO_CLI_SWITCH_EMIT_BYTECODE] = sizeof("emit-bytecode") - 1,
        [CIO_CLI_SWITCH_EXECUTE_BUNDLE] = sizeof("execute-bundle") - 1,
        [CIO_CLI_SWITCH_MANGLE_IDENTIFIER] = sizeof("mangle-identifier") - 1,
        [CIO_CLI_SWITCH_STACK_LENGTH] = sizeof("stack-length") - 1,
        [CIO_CLI_SWITCH_DISASSEMBLE] = sizeof("disassemble") - 1,
        [CIO_CLI_SWITCH_BUNDLE] = sizeof("bundle") - 1,
        [CIO_CLI_SWITCH_DEBUNDLE] = sizeof("debundle") - 1,
        [CIO_CLI_SWITCH_VERSION] = sizeof("version") - 1,
        [CIO_CLI_SWITCH_FATAL_WARNINGS] = sizeof("fatal-warnings") - 1,
        [CIO_CLI_SWITCH_WARNING] = sizeof("warning") - 1,
        [CIO_CLI_SWITCH_HELP] = sizeof("help") - 1,
        [CIO_CLI_SWITCH_DEBUG_VM] = sizeof("debug-vm") - 1
    };

    gen_arguments_parsed_t parsed = {0};
    error = gen_arguments_parse(argv + 1, argument_lengths, argc - 1, GEN_NULL, 0, switches, switches_lengths, sizeof(switches) / sizeof(char*), &parsed);
	if(error) return error;

    gen_size_t stack_length = GEN_SIZE_MAX;
    const char* entry_routine = GEN_NULL;
    const char* file = GEN_NULL;

    gen_bool_t warn_implicit_switch = gen_false;
    gen_bool_t warn_implicit_switch_parameter = gen_false;
    gen_bool_t warn_implicit_file = gen_false;
    cio_warning_settings_t warning_settings = {0};
    gen_bool_t debug_vm = gen_false;

    cio_cli_operation_t operation = CIO_CLI_OPERATION_NONE;

    for(gen_size_t i = 0; i < parsed.long_argument_count; ++i) {
        // We need to parse this out first to enable diagnostics in other switches
        if(parsed.long_argument_indices[i] == CIO_CLI_SWITCH_FATAL_WARNINGS) {
            if(parsed.long_argument_parameters[i]) {
                error = gen_log_formatted(GEN_LOG_LEVEL_FATAL, "cionom-cli", "`--%t` does not take a parameter", switches[parsed.long_argument_indices[i]]);
                if(error) return error;

                return gen_error_attach_backtrace_formatted(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`--%t` does not take a parameter", switches[parsed.long_argument_indices[i]]);
            }
            if(warning_settings.fatal_warnings) {
                error = gen_log_formatted(GEN_LOG_LEVEL_FATAL, "cionom-cli", "`--%t` specified multiple times", switches[parsed.long_argument_indices[i]]);
                if(error) return error;

                return gen_error_attach_backtrace_formatted(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`--%t` specified multiple times", switches[parsed.long_argument_indices[i]]);
            }

            warning_settings.fatal_warnings = gen_true;
        }
        else if(parsed.long_argument_indices[i] == CIO_CLI_SWITCH_WARNING) {
            if(!parsed.long_argument_parameters[i]) {
                error = gen_log_formatted(GEN_LOG_LEVEL_FATAL, "cionom-cli", "`--%t` expected a parameter", switches[parsed.long_argument_indices[i]]);
                if(error) return error;

                return gen_error_attach_backtrace_formatted(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`--%t` expected a parameter", switches[parsed.long_argument_indices[i]]);
            }

            // TODO: Move this out into a more managable structure

            gen_bool_t equal = gen_false;
            error = gen_string_compare("all", sizeof("all"), parsed.long_argument_parameters[i], parsed.long_argument_parameter_lengths[i] + 1, GEN_STRING_NO_BOUNDS, &equal);
            if(error) return error;
            if(equal) {
                gen_bool_t fatal_warnings = warning_settings.fatal_warnings;

                error = gen_memory_set(&warning_settings, sizeof(cio_warning_settings_t), gen_true);
                if(error) return error;
                warn_implicit_switch_parameter = gen_true;
                warn_implicit_switch = gen_true;
                warn_implicit_file = gen_true;
                
                warning_settings.fatal_warnings = fatal_warnings;

                continue;
            }

            error = gen_string_compare("implicit_switch_parameter", sizeof("implicit_switch_parameter"), parsed.long_argument_parameters[i], parsed.long_argument_parameter_lengths[i] + 1, GEN_STRING_NO_BOUNDS, &equal);
            if(error) return error;
            if(equal && warn_implicit_switch_parameter) return gen_error_attach_backtrace(GEN_ERROR_TOO_LONG, GEN_LINE_NUMBER, "`--warning=warn_implicit_switch_parameter` specified multiple times");
            if(equal) {
                warn_implicit_switch_parameter = gen_true;
                continue;
            }
            error = gen_string_compare("implicit_switch", sizeof("implicit_switch"), parsed.long_argument_parameters[i], parsed.long_argument_parameter_lengths[i] + 1, GEN_STRING_NO_BOUNDS, &equal);
            if(error) return error;
            if(equal && warn_implicit_switch) return gen_error_attach_backtrace(GEN_ERROR_TOO_LONG, GEN_LINE_NUMBER, "`--warning=warn_implicit_switch` specified multiple times");
            if(equal) {
                warn_implicit_switch = gen_true;
                continue;
            }
            error = gen_string_compare("implicit_file", sizeof("implicit_file"), parsed.long_argument_parameters[i], parsed.long_argument_parameter_lengths[i] + 1, GEN_STRING_NO_BOUNDS, &equal);
            if(error) return error;
            if(equal && warn_implicit_file) return gen_error_attach_backtrace(GEN_ERROR_TOO_LONG, GEN_LINE_NUMBER, "`--warning=warn_implicit_file` specified multiple times");
            if(equal) {
                warn_implicit_file = gen_true;
                continue;
            }

            error = gen_string_compare("emit_reserved_encoding", sizeof("emit_reserved_encoding"), parsed.long_argument_parameters[i], parsed.long_argument_parameter_lengths[i] + 1, GEN_STRING_NO_BOUNDS, &equal);
            if(error) return error;
            if(equal && warning_settings.emit_reserved_encoding) return gen_error_attach_backtrace(GEN_ERROR_TOO_LONG, GEN_LINE_NUMBER, "`--warning=emit_reserved_encoding` specified multiple times");
            if(equal) {
                warning_settings.emit_reserved_encoding = gen_true;
                continue;
            }
            error = gen_string_compare("reserved_identifier", sizeof("reserved_identifier"), parsed.long_argument_parameters[i], parsed.long_argument_parameter_lengths[i] + 1, GEN_STRING_NO_BOUNDS, &equal);
            if(error) return error;
            if(equal && warning_settings.reserved_identifier) return gen_error_attach_backtrace(GEN_ERROR_TOO_LONG, GEN_LINE_NUMBER, "`--warning=reserved_identifier` specified multiple times");
            if(equal) {
                warning_settings.reserved_identifier = gen_true;
                continue;
            }
            error = gen_string_compare("parameter_overflow", sizeof("parameter_overflow"), parsed.long_argument_parameters[i], parsed.long_argument_parameter_lengths[i] + 1, GEN_STRING_NO_BOUNDS, &equal);
            if(error) return error;
            if(equal && warning_settings.parameter_overflow) return gen_error_attach_backtrace(GEN_ERROR_TOO_LONG, GEN_LINE_NUMBER, "`--warning=parameter_overflow` specified multiple times");
            if(equal) {
                warning_settings.parameter_overflow = gen_true;
                continue;
            }
            error = gen_string_compare("parameter_count_mismatch", sizeof("parameter_count_mismatch"), parsed.long_argument_parameters[i], parsed.long_argument_parameter_lengths[i] + 1, GEN_STRING_NO_BOUNDS, &equal);
            if(error) return error;
            if(equal && warning_settings.parameter_count_mismatch) return gen_error_attach_backtrace(GEN_ERROR_TOO_LONG, GEN_LINE_NUMBER, "`--warning=parameter_count_mismatch` specified multiple times");
            if(equal) {
                warning_settings.parameter_count_mismatch = gen_true;
                continue;
            }
            error = gen_string_compare("consume_reserved_encoding", sizeof("consume_reserved_encoding"), parsed.long_argument_parameters[i], parsed.long_argument_parameter_lengths[i] + 1, GEN_STRING_NO_BOUNDS, &equal);
            if(error) return error;
            if(equal && warning_settings.consume_reserved_encoding) return gen_error_attach_backtrace(GEN_ERROR_TOO_LONG, GEN_LINE_NUMBER, "`--warning=consume_reserved_encoding` specified multiple times");
            if(equal) {
                warning_settings.consume_reserved_encoding = gen_true;
                continue;
            }
        }
    }

    for(gen_size_t i = 0; i < parsed.long_argument_count; ++i) {
        switch(parsed.long_argument_indices[i]) {
            case CIO_CLI_SWITCH_EMIT_BYTECODE: {
                if(operation) {
                    error = gen_log(GEN_LOG_LEVEL_FATAL, "cionom-cli", "Multiple operations specified");
                    if(error) return error;

                    return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "Multiple operations specified");
                }

                if(!parsed.long_argument_parameters[i] && warn_implicit_switch_parameter) {
                    error = gen_log_formatted(warning_settings.fatal_warnings ? GEN_LOG_LEVEL_FATAL : GEN_LOG_LEVEL_WARNING, "cionom-cli", "`--%t` parameter not specified, defaulting to `%t` [%twarn_implicit_switch_parameter]", switches[parsed.long_argument_indices[i]], CIO_CLI_BYTECODE_FILE_FALLBACK, warning_settings.fatal_warnings ? "fatal_warnings, " : "");
                    if(error) return error;

                    if(warning_settings.fatal_warnings) {
                        return gen_error_attach_backtrace_formatted(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`--%t` parameter not specified, defaulting to `%t` [%twarn_implicit_switch_parameter]", switches[parsed.long_argument_indices[i]], CIO_CLI_BYTECODE_FILE_FALLBACK, warning_settings.fatal_warnings ? "fatal_warnings, " : "");
                    }
                }

                file = parsed.long_argument_parameters[i] ?: CIO_CLI_BYTECODE_FILE_FALLBACK;

                operation = CIO_CLI_OPERATION_COMPILE;

                break;
            }
            
            case CIO_CLI_SWITCH_EXECUTE_BUNDLE: {
                if(operation) {
                    error = gen_log(GEN_LOG_LEVEL_FATAL, "cionom-cli", "Multiple operations specified");
                    if(error) return error;

                    return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "Multiple operations specified");
                }

                if(!parsed.long_argument_parameters[i] && warn_implicit_switch_parameter) {
                    error = gen_log_formatted(warning_settings.fatal_warnings ? GEN_LOG_LEVEL_FATAL : GEN_LOG_LEVEL_WARNING, "cionom-cli", "`--%t` parameter not specified, defaulting to `%t` [%twarn_implicit_switch_parameter]", switches[parsed.long_argument_indices[i]], CIO_CLI_ENTRY_ROUTINE_FALLBACK, warning_settings.fatal_warnings ? "fatal_warnings, " : "");
                    if(error) return error;

                    if(warning_settings.fatal_warnings) {
                        return gen_error_attach_backtrace_formatted(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`--%t` parameter not specified, defaulting to `%t` [%twarn_implicit_switch_parameter]", switches[parsed.long_argument_indices[i]], CIO_CLI_ENTRY_ROUTINE_FALLBACK, warning_settings.fatal_warnings ? "fatal_warnings, " : "");
                    }
                }

                entry_routine = parsed.long_argument_parameters[i] ?: CIO_CLI_ENTRY_ROUTINE_FALLBACK;

                operation = CIO_CLI_OPERATION_EXECUTE;

                break;
            }

            case CIO_CLI_SWITCH_MANGLE_IDENTIFIER: {
                if(operation) {
                    error = gen_log(GEN_LOG_LEVEL_FATAL, "cionom-cli", "Multiple operations specified");
                    if(error) return error;

                    return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "Multiple operations specified");
                }

                if(parsed.long_argument_parameters[i]) {
                    error = gen_log_formatted(GEN_LOG_LEVEL_FATAL, "cionom-cli", "`--%t` does not take a parameter", switches[parsed.long_argument_indices[i]]);
                    if(error) return error;

                    return gen_error_attach_backtrace_formatted(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`--%t` does not take a parameter", switches[parsed.long_argument_indices[i]]);
                }

                operation = CIO_CLI_OPERATION_MANGLE;

                break;
            }

            case CIO_CLI_SWITCH_STACK_LENGTH: {
                if(!parsed.long_argument_parameters[i]) {
                    error = gen_log_formatted(GEN_LOG_LEVEL_FATAL, "cionom-cli", "`--%t` expected a parameter", switches[parsed.long_argument_indices[i]]);
                    if(error) return error;

                    return gen_error_attach_backtrace_formatted(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`--%t` expected a parameter", switches[parsed.long_argument_indices[i]]);
                }
                if(stack_length != GEN_SIZE_MAX) {
                    error = gen_log_formatted(GEN_LOG_LEVEL_FATAL, "cionom-cli", "`--%t` specified multiple times", switches[parsed.long_argument_indices[i]]);
                    if(error) return error;

                    return gen_error_attach_backtrace_formatted(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`--%t` specified multiple times", switches[parsed.long_argument_indices[i]]);
                }

                error = gen_string_number(parsed.long_argument_parameters[i], parsed.long_argument_parameter_lengths[i] + 1, GEN_STRING_NO_BOUNDS, &stack_length);
                if(error) return error;

                break;
            }

            case CIO_CLI_SWITCH_DISASSEMBLE: {
                if(operation) {
                    error = gen_log(GEN_LOG_LEVEL_FATAL, "cionom-cli", "Multiple operations specified");
                    if(error) return error;

                    return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "Multiple operations specified");
                }

                if(!parsed.long_argument_parameters[i] && warn_implicit_switch_parameter) {
                    error = gen_log_formatted(warning_settings.fatal_warnings ? GEN_LOG_LEVEL_FATAL : GEN_LOG_LEVEL_WARNING, "cionom-cli", "`--%t` parameter not specified, defaulting to `%t` [%twarn_implicit_switch_parameter]", switches[parsed.long_argument_indices[i]], CIO_CLI_ASM_FILE_FALLBACK, warning_settings.fatal_warnings ? "fatal_warnings, " : "");
                    if(error) return error;

                    if(warning_settings.fatal_warnings) {
                        return gen_error_attach_backtrace_formatted(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`--%t` parameter not specified, defaulting to `%t` [%twarn_implicit_switch_parameter]", switches[parsed.long_argument_indices[i]], CIO_CLI_ASM_FILE_FALLBACK, warning_settings.fatal_warnings ? "fatal_warnings, " : "");
                    }
                }
                file = parsed.long_argument_parameters[i] ?: CIO_CLI_ASM_FILE_FALLBACK;

                operation = CIO_CLI_OPERATION_DISASSEMBLE;

                break;
            }

            case CIO_CLI_SWITCH_BUNDLE: {
                if(operation) {
                    error = gen_log(GEN_LOG_LEVEL_FATAL, "cionom-cli", "Multiple operations specified");
                    if(error) return error;

                    return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "Multiple operations specified");
                }

                if(!parsed.long_argument_parameters[i] && warn_implicit_switch_parameter) {
                    error = gen_log_formatted(warning_settings.fatal_warnings ? GEN_LOG_LEVEL_FATAL : GEN_LOG_LEVEL_WARNING, "cionom-cli", "`--%t` parameter not specified, defaulting to `%t` [%twarn_implicit_switch_parameter]", switches[parsed.long_argument_indices[i]], CIO_CLI_BUNDLE_FILE_FALLBACK, warning_settings.fatal_warnings ? "fatal_warnings, " : "");
                    if(error) return error;
                    
                    if(warning_settings.fatal_warnings) {
                        return gen_error_attach_backtrace_formatted(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`--%t` parameter not specified, defaulting to `%t` [%twarn_implicit_switch_parameter]", switches[parsed.long_argument_indices[i]], CIO_CLI_BUNDLE_FILE_FALLBACK, warning_settings.fatal_warnings ? "fatal_warnings, " : "");
                    }
                }

                file = parsed.long_argument_parameters[i] ?: CIO_CLI_BUNDLE_FILE_FALLBACK;

                operation = CIO_CLI_OPERATION_BUNDLE;

                break;
            }

            case CIO_CLI_SWITCH_DEBUNDLE: {
                if(operation) {
                    error = gen_log(GEN_LOG_LEVEL_FATAL, "cionom-cli", "Multiple operations specified");
                    if(error) return error;

                    return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "Multiple operations specified");
                }

                if(!parsed.long_argument_parameters[i] && warn_implicit_switch_parameter) {
                    error = gen_log_formatted(warning_settings.fatal_warnings ? GEN_LOG_LEVEL_FATAL : GEN_LOG_LEVEL_WARNING, "cionom-cli", "`--%t` parameter not specified, defaulting to `%t` [%twarn_implicit_switch_parameter]", switches[parsed.long_argument_indices[i]], CIO_CLI_BUNDLE_FILE_FALLBACK, warning_settings.fatal_warnings ? "fatal_warnings, " : "");
                    if(error) return error;
                    
                    if(warning_settings.fatal_warnings) {
                        return gen_error_attach_backtrace_formatted(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`--%t` parameter not specified, defaulting to `%t` [%twarn_implicit_switch_parameter]", switches[parsed.long_argument_indices[i]], CIO_CLI_BUNDLE_FILE_FALLBACK, warning_settings.fatal_warnings ? "fatal_warnings, " : "");
                    }
                }

                file = parsed.long_argument_parameters[i] ?: CIO_CLI_BUNDLE_FILE_FALLBACK;

                operation = CIO_CLI_OPERATION_DEBUNDLE;

                break;
            }

            case CIO_CLI_SWITCH_VERSION: {
                if(operation) {
                    error = gen_log(GEN_LOG_LEVEL_FATAL, "cionom-cli", "Multiple operations specified");
                    if(error) return error;

                    return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "Multiple operations specified");
                }

                if(parsed.long_argument_parameters[i]) {
                    error = gen_log_formatted(GEN_LOG_LEVEL_FATAL, "cionom-cli", "`--%t` does not take a parameter", switches[parsed.long_argument_indices[i]]);
                    if(error) return error;

                    return gen_error_attach_backtrace_formatted(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`--%t` does not take a parameter", switches[parsed.long_argument_indices[i]]);
                }

                operation = CIO_CLI_OPERATION_VERSION;

                break;
            }

            case CIO_CLI_SWITCH_FATAL_WARNINGS: break; // Already handled above
            case CIO_CLI_SWITCH_WARNING: break; // Already handled above

            case CIO_CLI_SWITCH_HELP: {
                if(operation) {
                    error = gen_log(GEN_LOG_LEVEL_FATAL, "cionom-cli", "Multiple operations specified");
                    if(error) return error;

                    return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "Multiple operations specified");
                }

                if(parsed.long_argument_parameters[i]) {
                    error = gen_log_formatted(GEN_LOG_LEVEL_FATAL, "cionom-cli", "`--%t` does not take a parameter", switches[parsed.long_argument_indices[i]]);
                    if(error) return error;

                    return gen_error_attach_backtrace_formatted(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`--%t` does not take a parameter", switches[parsed.long_argument_indices[i]]);
                }

                operation = CIO_CLI_OPERATION_HELP;

                break;
            }

            case CIO_CLI_SWITCH_DEBUG_VM: {
                if(parsed.long_argument_parameters[i]) {
                    error = gen_log_formatted(GEN_LOG_LEVEL_FATAL, "cionom-cli", "`--%t` does not take a parameter", switches[parsed.long_argument_indices[i]]);
                    if(error) return error;

                    return gen_error_attach_backtrace_formatted(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`--%t` does not take a parameter", switches[parsed.long_argument_indices[i]]);
                }

                debug_vm = gen_true;

                break;
            }

            default: return gen_error_attach_backtrace(GEN_ERROR_UNKNOWN, GEN_LINE_NUMBER, "Something went wrong while parsing arguments");
        }
    }

    switch(operation) {
        case CIO_CLI_OPERATION_NONE: {
            error = gen_log(GEN_LOG_LEVEL_FATAL, "cionom-cli", "No operation specified");
            if(error) return error;

            return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "No operation specified");
        }
        case CIO_CLI_OPERATION_COMPILE: {
            if(parsed.raw_argument_count > 1) {
                error = gen_log(GEN_LOG_LEVEL_FATAL, "cionom-cli", "Multiple files specified");
                if(error) return error;

                return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "Multiple files specified");
            }
            if(!parsed.raw_argument_count) {
                error = gen_log(GEN_LOG_LEVEL_FATAL, "cionom-cli", "No source file specified");
                if(error) return error;

                return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "No source file specified");
            }

            const char* source_file = (argv + 1)[parsed.raw_argument_indices[0]];

            gen_size_t filename_length = 0;
            error = gen_string_length(source_file, GEN_STRING_NO_BOUNDS, GEN_STRING_NO_BOUNDS, &filename_length);
            if(error) return error;

            gen_size_t source_length = 0;
            char* source = GEN_NULL;
            error = cio_cli_read_file(source_file, (unsigned char**) &source, &source_length);
            if(error) return error;

			cio_token_t* tokens = GEN_NULL;
			gen_size_t tokens_length = 0;
			error = cio_tokenize(source, source_length, &tokens, &tokens_length);
			if(error) return error;

			cio_program_t program = {0};
			error = cio_parse(tokens, tokens_length, &program, source, source_length, source_file, filename_length, &warning_settings);
			if(error) return error;

			unsigned char* bytecode = GEN_NULL;
			gen_size_t bytecode_length = 0;
			error = cio_module_emit(&program, &bytecode, &bytecode_length, source, source_length, source_file, filename_length, &warning_settings);
			if(error) return error;

            error = cio_cli_recreate_write_file(file, bytecode, bytecode_length);
			if(error) return error;

            break;
        }
        case CIO_CLI_OPERATION_EXECUTE: {
            if(parsed.raw_argument_count > 1) {
                error = gen_log(GEN_LOG_LEVEL_FATAL, "cionom-cli", "Multiple files specified");
                if(error) return error;

                return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "Multiple files specified");
            }

            const char* bytecode_file = GEN_NULL;

            if(!parsed.raw_argument_count && warn_implicit_file) {
                error = gen_log_formatted(warning_settings.fatal_warnings ? GEN_LOG_LEVEL_FATAL : GEN_LOG_LEVEL_WARNING, "cionom-cli", "Bytecode bundle not specified, defaulting to `%t` [%twarn_implicit_file]", CIO_CLI_BUNDLE_FILE_FALLBACK, warning_settings.fatal_warnings ? "fatal_warnings, " : "");
                if(error) return error;

                if(warning_settings.fatal_warnings) {
                    return gen_error_attach_backtrace_formatted(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "Bytecode file not specified, defaulting to `%t` [%twarn_implicit_file]", CIO_CLI_BUNDLE_FILE_FALLBACK, warning_settings.fatal_warnings ? "fatal_warnings, " : "");
                }
            }

            bytecode_file = parsed.raw_argument_count ? (argv + 1)[parsed.raw_argument_indices[0]] : CIO_CLI_BUNDLE_FILE_FALLBACK;

            if(stack_length == GEN_SIZE_MAX && warn_implicit_switch) {
                error = gen_log_formatted(warning_settings.fatal_warnings ? GEN_LOG_LEVEL_FATAL : GEN_LOG_LEVEL_WARNING, "cionom-cli", "`--%t` not specified, defaulting to %uz [%twarn_implicit_switch]", switches[CIO_CLI_SWITCH_STACK_LENGTH], (gen_size_t) CIO_CLI_STACK_LENGTH_FALLBACK, warning_settings.fatal_warnings ? "fatal_warnings, " : "");
                if(error) return error;

                if(warning_settings.fatal_warnings) {
                    return gen_error_attach_backtrace_formatted(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`--%t` not specified, defaulting to %uz [%twarn_implicit_switch]", switches[CIO_CLI_SWITCH_STACK_LENGTH], (gen_size_t) CIO_CLI_STACK_LENGTH_FALLBACK, warning_settings.fatal_warnings ? "fatal_warnings, " : "");
                }

            }
            stack_length = stack_length != GEN_SIZE_MAX ? stack_length : CIO_CLI_STACK_LENGTH_FALLBACK;

            gen_size_t bytecode_length = 0;
            unsigned char* bytecode = GEN_NULL;
            error = cio_cli_read_file(bytecode_file, (unsigned char**) &bytecode, &bytecode_length);
            if(error) return error;

			cio_vm_t vm = {0};
			error = cio_vm_initialize((unsigned char*) bytecode, bytecode_length, stack_length, gen_true, &vm, debug_vm, &warning_settings);
			if(error) return error;

			error = cio_vm_push_frame(&vm);
			if(error) return error;
			error = cio_vm_push(&vm);
			if(error) return error;

            cio_callable_t* callable = GEN_NULL;
            error = cio_vm_get_identifier(&vm, entry_routine, &callable, gen_false);
			if(error) return error;

            vm.current_bytecode = callable->bytecode_index;
			error = cio_vm_dispatch_call(&vm, callable->routine_index, 0);
			if(error) return error;

            break;
        }
        case CIO_CLI_OPERATION_MANGLE: {
            if(!parsed.raw_argument_count) {
                error = gen_log(GEN_LOG_LEVEL_FATAL, "cionom-cli", "No identifier specified");
                if(error) return error;

                return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "No identifier specified");
            }
            if(parsed.raw_argument_count > 1) {
                error = gen_log(GEN_LOG_LEVEL_FATAL, "cionom-cli", "Multiple identifiers specified");
                if(error) return error;

                return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "Multiple identifiers specified");
            }

            char* mangled = GEN_NULL;
            error = cio_mangle_identifier((argv + 1)[parsed.raw_argument_indices[0]], &mangled);
            if(error) return error;

            error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom-cli", "Result of mangling \"%t\" is: `%t`", (argv + 1)[parsed.raw_argument_indices[0]], mangled);
            if(error) return error;

            break;
        }
        case CIO_CLI_OPERATION_DISASSEMBLE: {
            if(parsed.raw_argument_count > 1) {
                error = gen_log(GEN_LOG_LEVEL_FATAL, "cionom-cli", "Multiple files specified");
                if(error) return error;

                return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "Multiple files specified");
            }

            const char* bytecode_file = GEN_NULL;

            if(!parsed.raw_argument_count && warn_implicit_file) {
                error = gen_log_formatted(warning_settings.fatal_warnings ? GEN_LOG_LEVEL_FATAL : GEN_LOG_LEVEL_WARNING, "cionom-cli", "File not specified, defaulting to `%t` [%twarn_implicit_file]", CIO_CLI_BYTECODE_FILE_FALLBACK, warning_settings.fatal_warnings ? "fatal_warnings, " : "");
                if(error) return error;
                
                if(warning_settings.fatal_warnings) {
                    return gen_error_attach_backtrace_formatted(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "File not specified, defaulting to `%t` [%twarn_implicit_file]", CIO_CLI_BYTECODE_FILE_FALLBACK, warning_settings.fatal_warnings ? "fatal_warnings, " : "");
                }
            }

            bytecode_file = parsed.raw_argument_count ? (argv + 1)[parsed.raw_argument_indices[0]] : CIO_CLI_BYTECODE_FILE_FALLBACK;

            gen_size_t bytecode_length = 0;
            unsigned char* bytecode = GEN_NULL;
            error = cio_cli_read_file(bytecode_file, (unsigned char**) &bytecode, &bytecode_length);
            if(error) return error;

            cio_vm_t vm = {0};
            error = cio_vm_initialize(bytecode, bytecode_length, 1, gen_false, &vm, gen_false, &warning_settings);
            if(error) return error;

            if(vm.bytecode_length != 1) {
                error = gen_log_formatted(GEN_LOG_LEVEL_FATAL, "cionom-cli", "Cannot disassemble executable bundle of %uz modules `%t`. Pass individual modules instead", vm.bytecode_length, bytecode_file);
                if(error) return error;

                return gen_error_attach_backtrace_formatted(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "Cannot disassemble executable bundle of %uz modules `%t`. Pass individual modules instead", vm.bytecode_length, bytecode_file);
            }

            cio_bytecode_t* bytecode_meta = &vm.bytecode[0];

            char* cas_file = GEN_NULL;
            gen_size_t cas_file_size = 0;

            for(gen_size_t i = 0; i < bytecode_meta->callables_length; ++i) {
                if(bytecode_meta->callables[i].offset == CIO_ROUTINE_EXTERNAL) {
                    error = gen_memory_reallocate_zeroed((void**) &cas_file, cas_file_size, cas_file_size + bytecode_meta->callables[i].identifier_length + 9, sizeof(char));
                    if(error) return error;
                    
                    error = gen_string_format(bytecode_meta->callables[i].identifier_length + 9, &cas_file[cas_file_size], GEN_NULL, ":import %tz\n", sizeof(":import %tz\n") - 1, bytecode_meta->callables[i].identifier, bytecode_meta->callables[i].identifier_length);
                    if(error) return error;
                    
                    cas_file_size += bytecode_meta->callables[i].identifier_length + 9;
                }
            }

            if(cas_file) {
                error = gen_memory_reallocate_zeroed((void**) &cas_file, cas_file_size, cas_file_size + 1, sizeof(char));
                if(error) return error;

                cas_file[cas_file_size] = '\n';
                
                cas_file_size++;
            }

            for(gen_size_t i = 0; i < bytecode_meta->size; ++i) {
                for(gen_size_t j = 0; j < bytecode_meta->callables_length; ++j) {
                    if(i == bytecode_meta->callables[j].offset) {
                        error = gen_memory_reallocate_zeroed((void**) &cas_file, cas_file_size, cas_file_size + bytecode_meta->callables[j].identifier_length + 2, sizeof(char));
                        if(error) return error;
                        
                        error = gen_string_format(bytecode_meta->callables[j].identifier_length + 2, &cas_file[cas_file_size], GEN_NULL, "%tz:\n", sizeof("%tz:\n") - 1, bytecode_meta->callables[j].identifier, bytecode_meta->callables[j].identifier_length);
                        if(error) return error;
                        
                        cas_file_size += bytecode_meta->callables[j].identifier_length + 2;
                    }
                }

#ifdef __ANALYZER
                cio_instruction_t instruction = {0};
#else
                cio_instruction_t instruction = *(const cio_instruction_t*) &bytecode_meta->bytecode[i];
#endif

                switch(instruction.opcode) {
                    case CIO_PUSH: {
                        gen_size_t format_len = 0;
                        error = gen_string_format(GEN_STRING_NO_BOUNDS, GEN_NULL, &format_len, "\tpush %uc\n", sizeof("\tpush %uc\n") - 1, instruction.operand);
                        if(error) return error;
                        
                        error = gen_memory_reallocate_zeroed((void**) &cas_file, cas_file_size, cas_file_size + format_len, sizeof(char));
                        if(error) return error;

                        error = gen_string_format(format_len, &cas_file[cas_file_size], GEN_NULL, "\tpush %uc\n", sizeof("\tpush %uc\n") - 1, instruction.operand);
                        if(error) return error;

                        cas_file_size += format_len;
                        break;
                    }
                    case CIO_CALL: {
                        if(instruction.operand == CIO_OPERAND_MAX) {
                            error = gen_memory_reallocate_zeroed((void**) &cas_file, cas_file_size, cas_file_size + 5, sizeof(char));
                            if(error) return error;
                            
                            error = gen_string_copy(&cas_file[cas_file_size], 5, "\tret\n", sizeof("\tret\n"), 5);
                            if(error) return error;
                            
                            cas_file_size += 5;
                            break;
                        }

                        gen_size_t format_len = 0;
                        error = gen_string_format(GEN_STRING_NO_BOUNDS, GEN_NULL, &format_len, "\tcall %t\n", sizeof("\tcall %t\n") - 1, bytecode_meta->callables[instruction.operand].identifier);
                        if(error) return error;
                        
                        error = gen_memory_reallocate_zeroed((void**) &cas_file, cas_file_size, cas_file_size + format_len, sizeof(char));
                        if(error) return error;

                        error = gen_string_format(format_len, &cas_file[cas_file_size], GEN_NULL, "\tcall %t\n", sizeof("\tcall %t\n") - 1, bytecode_meta->callables[instruction.operand].identifier);
                        if(error) return error;

                        cas_file_size += format_len;
                        break;
                    }
                }
            }

            error = cio_cli_recreate_write_file(file, (unsigned char*) cas_file, cas_file_size);
			if(error) return error;

            break;
        }

        case CIO_CLI_OPERATION_BUNDLE: {
            // TODO: Verify that modules are actually modules

            if(!parsed.raw_argument_count) {
                error = gen_log(GEN_LOG_LEVEL_FATAL, "cionom-cli", "No bytecode files specified");
                if(error) return error;

                return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "No bytecode files specified");
            }

            unsigned char* buffer = GEN_NULL;
            gen_size_t buffer_size = 0;

            for(gen_size_t i = 0; i < parsed.raw_argument_count; ++i) {
                gen_size_t bytecode_length = 0;
                unsigned char* bytecode = GEN_NULL;
                error = cio_cli_read_file((argv + 1)[parsed.raw_argument_indices[i]], (unsigned char**) &bytecode, &bytecode_length);
                if(error) return error;

                error = gen_memory_reallocate_zeroed((void**) &buffer, buffer_size, buffer_size + bytecode_length, sizeof(unsigned char));
                if(error) return error;

                error = gen_memory_copy(&buffer[buffer_size], bytecode_length, bytecode, bytecode_length, bytecode_length);
                if(error) return error;

                error = gen_memory_free((void**) &bytecode);
                if(error) return error;

                buffer_size += bytecode_length;
            }

            error = cio_cli_recreate_write_file(file, buffer, buffer_size);
			if(error) return error;

            break;
        }
        case CIO_CLI_OPERATION_DEBUNDLE: {
            gen_size_t bytecode_length = 0;
            unsigned char* bytecode = GEN_NULL;
            error = cio_cli_read_file(file, (unsigned char**) &bytecode, &bytecode_length);
            if(error) return error;

            // TODO: Use debug info for filenames
            // TODO: Verify that modules are actually modules

            gen_size_t bytecode_count = 0;

            for(gen_size_t i = 0; i < bytecode_length; ++i) {
                gen_size_t begin = i;
                gen_size_t callables_length = bytecode[i];

                gen_size_t offset = 1;
                gen_uint32_t last_routine = 0;
                for(gen_size_t j = 0; j < callables_length; ++j) {
                    // This avoids alignment shenanigans
                    // Doesn't really matter but ASan has a hissy fit
                    error = gen_memory_copy(&last_routine, sizeof(gen_uint32_t), &bytecode[i + offset], bytecode_length - (i + offset), sizeof(gen_uint32_t));
                    if(error) return error;

                    offset += 4;

                    gen_size_t stride = 0;
                    error = gen_string_length((const char*) &bytecode[i + offset], GEN_STRING_NO_BOUNDS, GEN_STRING_NO_BOUNDS, &stride);
                    if(error) return error;

                    offset += stride + 1;
                }

                for(i += offset + last_routine; bytecode[i] != 0xFF; ++i);

                gen_size_t module_length = 0;
                error = gen_string_format(GEN_STRING_NO_BOUNDS, GEN_NULL, &module_length, "%uz.ibc", sizeof("%uz.ibc") - 1, bytecode_count);
                if(error) return error;
                char* module = GEN_NULL;
                error = gen_memory_allocate_zeroed((void**) &module, module_length + 1, sizeof(char));
                if(error) return error;
                error = gen_string_format(module_length, module, GEN_NULL, "%uz.ibc", sizeof("%uz.ibc") - 1, bytecode_count);
                if(error) return error;

                error = cio_cli_recreate_write_file(module, &bytecode[begin], (i - begin) + 1);
	    		if(error) return error;
        
                bytecode_count++;
            }

            break;
        }

        case CIO_CLI_OPERATION_VERSION: {
            error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom-cli", "Cíonom %t-%t", CIO_CLI_VERSION, GEN_BUILD_MODE == GEN_DEBUG ? "debug" : "release");
            if(error) return error;

            break;
        }

        case CIO_CLI_OPERATION_HELP: {
            const gen_size_t option_pad = 35;
            const gen_size_t suboption_pad = 30;

            error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom-cli", "USAGE: %t [OPTIONS...] [ARGUMENTS...]\n", argv[0]);
            if(error) return error;

            error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom-cli", "--%t[=FILE] SOURCE%czCompiles `SOURCE` to Cíonom bytecode\n%czPlaces output into `FILE` - otherwise `%t`", switches[CIO_CLI_SWITCH_EMIT_BYTECODE], ' ', option_pad - (2 + switches_lengths[CIO_CLI_SWITCH_EMIT_BYTECODE] + sizeof("[=FILE] SOURCE") - 1), ' ', GEN_LOG_RISING_EDGE_LENGTH + option_pad, CIO_CLI_BYTECODE_FILE_FALLBACK);
            if(error) return error;
            error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom-cli", "--%t[=ENTRY] BUNDLE%czExecutes the bundled executable `BUNDLE` - otherwise `%t`", switches[CIO_CLI_SWITCH_EXECUTE_BUNDLE], ' ', option_pad - (2 + switches_lengths[CIO_CLI_SWITCH_EXECUTE_BUNDLE] + sizeof("[=ENTRY] BUNDLE") - 1), CIO_CLI_BUNDLE_FILE_FALLBACK);
            if(error) return error;
            error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom-cli", "--%t IDENTIFIER%czMangles `IDENTIFIER` to a form suitable for native code identifier names", switches[CIO_CLI_SWITCH_MANGLE_IDENTIFIER], ' ', option_pad - (2 + switches_lengths[CIO_CLI_SWITCH_MANGLE_IDENTIFIER] + sizeof(" IDENTIFIER") - 1));
            if(error) return error;
            error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom-cli", "--%t=LENGTH%czSets the stack length for the VM to `LENGTH` when executing bundled executables\n%czIf unspecified stack length defaults to %ui", switches[CIO_CLI_SWITCH_STACK_LENGTH], ' ', option_pad - (2 + switches_lengths[CIO_CLI_SWITCH_STACK_LENGTH] + sizeof("=LENGTH") - 1), ' ', GEN_LOG_RISING_EDGE_LENGTH + option_pad, CIO_CLI_STACK_LENGTH_FALLBACK);
            if(error) return error;
            error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom-cli", "--%t[=FILE] BYTECODE%czDisassembles the bytecode file `BYTECODE`\n%czPlaces output into `FILE` - otherwise %t", switches[CIO_CLI_SWITCH_DISASSEMBLE], ' ', option_pad - (2 + switches_lengths[CIO_CLI_SWITCH_DISASSEMBLE] + sizeof("[=FILE] BYTECODE") - 1), ' ', GEN_LOG_RISING_EDGE_LENGTH + option_pad, CIO_CLI_ASM_FILE_FALLBACK);
            if(error) return error;
            error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom-cli", "--%t[=FILE] BYTECODE...%czBundles the bytecode files `BYTECODE...` into a bundled executable\n%czPlaces output into `FILE` - otherwise %t", switches[CIO_CLI_SWITCH_BUNDLE], ' ', option_pad - (2 + switches_lengths[CIO_CLI_SWITCH_BUNDLE] + sizeof("[=FILE] BYTECODE...") - 1), ' ', GEN_LOG_RISING_EDGE_LENGTH + option_pad, CIO_CLI_BUNDLE_FILE_FALLBACK);
            if(error) return error;
            error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom-cli", "--%t BUNDLE%czExtracts bytecode files from the bundled executable `BUNDLE` - otherwise %t\n%czPlaces output into `N.ibc` where `N` was the index of the module in the bundle", switches[CIO_CLI_SWITCH_DEBUNDLE], ' ', option_pad - (2 + switches_lengths[CIO_CLI_SWITCH_DEBUNDLE] + sizeof(" BUNDLE") - 1), CIO_CLI_BUNDLE_FILE_FALLBACK, ' ', GEN_LOG_RISING_EDGE_LENGTH + option_pad);
            if(error) return error;
            error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom-cli", "--%t%czPrints version information", switches[CIO_CLI_SWITCH_VERSION], ' ', option_pad - (2 + switches_lengths[CIO_CLI_SWITCH_VERSION]));
            if(error) return error;
            error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom-cli", "--%t%czTreats all warnings as fatal", switches[CIO_CLI_SWITCH_FATAL_WARNINGS], ' ', option_pad - (2 + switches_lengths[CIO_CLI_SWITCH_FATAL_WARNINGS]));
            if(error) return error;
            error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom-cli", "--%t=WARNING%czEnables the warning `WARNING`. Available warnings are listed below:", switches[CIO_CLI_SWITCH_WARNING], ' ', option_pad - (2 + switches_lengths[CIO_CLI_SWITCH_WARNING] + sizeof("=WARNING") - 1));
            if(error) return error;
            {
                error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom-cli", "\tall%czEnable all warnings", ' ', suboption_pad - (sizeof("all") - 1));
                if(error) return error;
                error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom-cli", "\timplicit_switch_parameter%czWarn on CLI switches which default on an unspecified parameter", ' ', suboption_pad - (sizeof("implicit_switch_parameter") - 1));
                if(error) return error;
                error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom-cli", "\timplicit_switch%czWarn on implicit CLI switches", ' ', suboption_pad - (sizeof("implicit_switch") - 1));
                if(error) return error;
                error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom-cli", "\timplicit_file%czWarn on implicit CLI file arguments", ' ', suboption_pad - (sizeof("implicit_file") - 1));
                if(error) return error;
                error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom-cli", "\temit_reserved_encoding%czWarn on emission of reserved bytecode encodings", ' ', suboption_pad - (sizeof("emit_reserved_encoding") - 1));
                if(error) return error;
                error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom-cli", "\treserved_identifier%czWarn on usages of reserved identifiers", ' ', suboption_pad - (sizeof("reserved_identifier") - 1));
                if(error) return error;
                error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom-cli", "\tparameter_overflow%czWarn on parameters exceeding maximum encodable value", ' ', suboption_pad - (sizeof("parameter_overflow") - 1));
                if(error) return error;
                error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom-cli", "\tparameter_count_mismatch%czWarn on calls which do not match the routine's declared parameter count", ' ', suboption_pad - (sizeof("parameter_count_mismatch") - 1));
                if(error) return error;
                error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom-cli", "\tconsume_reserved_encoding%czWarn on consumption of reserved bytecode encodings", ' ', suboption_pad - (sizeof("consume_reserved_encoding") - 1));
                if(error) return error;
            }
            error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom-cli", "--%t%czShow this menu", switches[CIO_CLI_SWITCH_HELP], ' ', option_pad - (2 + switches_lengths[CIO_CLI_SWITCH_HELP]));
            if(error) return error;

            break;            
        }
    }

    return GEN_NULL;
}

int main(const int argc, const char* const* const argv) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) main, GEN_FILE_NAME);
	if(error) {
        gen_error_print("cionom-cli", error, GEN_ERROR_SEVERITY_FATAL);
        gen_error_abort();
    }

    error = gen_main((gen_size_t) argc, argv);
    if(error) {
#if GEN_BUILD_MODE == GEN_DEBUG
        gen_error_print("cionom-cli", error, GEN_ERROR_SEVERITY_FATAL);
#else
        gen_log(GEN_LOG_LEVEL_FATAL, "cionom-cli", error->context);
#endif
        gen_error_abort();
    }
}
