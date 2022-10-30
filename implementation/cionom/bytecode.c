// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

#include "include/cionom.h"

#include <genmemory.h>
#include <genstring.h>

#include <genlog.h>

static void cio_module_internal_emit_cleanup_offsets(uint32_t** offsets) {
    if(!*offsets) return;

    gen_error_t* error = gen_memory_free((void**) offsets);
    if(error) {
        gen_error_print("cionom", error, GEN_ERROR_SEVERITY_FATAL);
        gen_error_abort();
    }
}

static void cio_module_internal_emit_cleanup_code(cio_instruction_t** code) {
    if(!*code) return;

    gen_error_t* error = gen_memory_free((void**) code);
    if(error) {
        gen_error_print("cionom", error, GEN_ERROR_SEVERITY_FATAL);
        gen_error_abort();
    }
}

static void cio_module_internal_emit_cleanup_header(cio_header_t** header) {
    if(!*header) return;

    gen_error_t* error = gen_memory_free((void**) header);
    if(error) {
        gen_error_print("cionom", error, GEN_ERROR_SEVERITY_FATAL);
        gen_error_abort();
    }
}

static void cio_module_internal_emit_cleanup_bytecode(unsigned char** bytecode) {
    if(!*bytecode) return;

    gen_error_t* error = gen_memory_free((void**) bytecode);
    if(error) {
        gen_error_print("cionom", error, GEN_ERROR_SEVERITY_FATAL);
        gen_error_abort();
    }
}

gen_error_t* cio_module_emit(const cio_program_t* const restrict program, unsigned char** const restrict out_bytecode, size_t* const restrict out_bytecode_length, const char* const restrict source, const size_t source_length, const char* const restrict source_file, const size_t source_file_length, const cio_warning_settings_t* const restrict warning_settings) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) cio_module_emit, GEN_FILE_NAME);
	if(error) return error;

	if(!program) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`program` was `NULL`");
	if(!out_bytecode) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`out_bytecode` was `NULL`");
	if(!out_bytecode_length) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`out_bytecode_length` was `NULL`");
	if(!source) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`source` was `NULL`");
	if(!source_file) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`source_file` was `NULL`");

    if(program->routines_length >= CIO_ROUTINE_EXTERNAL) {
        error = gen_log_formatted(GEN_LOG_LEVEL_FATAL, "cionom", "%uz routines exceeds maximum of %uz allowed by bytecode format in %t", program->routines_length, (size_t) CIO_ROUTINE_EXTERNAL - 1, source_file);
        if(error) return error;

        return gen_error_attach_backtrace_formatted(GEN_ERROR_TOO_LONG, GEN_LINE_NUMBER, "%uz routines exceeds maximum of %uz allowed by bytecode format in %t", program->routines_length, (size_t) CIO_ROUTINE_EXTERNAL - 1, source_file);
    }

	GEN_CLEANUP_FUNCTION(cio_module_internal_emit_cleanup_offsets) uint32_t* offsets = NULL;
	if(program->routines_length) {
		error = gen_memory_allocate_zeroed((void**) &offsets, program->routines_length, sizeof(uint32_t));
		if(error) return error;
	}

	size_t code_size = 0;
	GEN_CLEANUP_FUNCTION(cio_module_internal_emit_cleanup_code) cio_instruction_t* code = NULL;

	// Codegen
	{
		for(size_t i = 0; i < program->routines_length; ++i) {
			const cio_routine_t* const routine = &program->routines[i];

			if(routine->external) {
                offsets[i] = CIO_ROUTINE_EXTERNAL;
                continue;
            }

            // Cache routine offsets for header generation
			offsets[i] = (uint32_t) code_size;

			for(size_t j = 0; j < routine->calls_length; ++j) {
				const cio_call_t* const call = &routine->calls[j];

                error = gen_memory_reallocate_zeroed((void**) &code, code_size, code_size + call->parameters_length + 2, sizeof(cio_instruction_t));
                if(error) return error;

                // Emit pushes
                {
                    code[code_size] = (cio_instruction_t) {0, CIO_PUSH}; // Reserve space

                    for(size_t k = 0; k < call->parameters_length; ++k) {
                        if(call->parameters[k] == CIO_OPERAND_MAX && warning_settings->emit_reserved_encoding) {
                            // TODO: Diagnostics like this can definitely be reconsolidated
                            //       into a more centralized interface.
                            //       Use to implement "show line of error" with an indicator.
                            //       Have a sink of some sort to allow the lib-user to
                            //       retrieve diagnostics.
                            size_t line = 0;
                            size_t column = 0;

                            error = cio_line_from_offset(call->token->offset, &line, source, source_length);
                            if(error) return error;

                            error = cio_column_from_offset(call->token->offset, &column, source, source_length);
                            if(error) return error;

                            error = gen_log_formatted(warning_settings->fatal_warnings ? GEN_LOG_LEVEL_FATAL : GEN_LOG_LEVEL_WARNING, "cionom", "Emission for call `%tz` resulted in reserved encoding `push %uc` in %t:%uz:%uz [%temit_reserved_encoding]", &source[call->token->offset], call->token->length, CIO_OPERAND_MAX, source_file, line, column, warning_settings->fatal_warnings ? "fatal_warnings, " : "");
                            if(error) return error;

                            if(warning_settings->fatal_warnings) {
                                return gen_error_attach_backtrace_formatted(GEN_ERROR_IN_USE, GEN_LINE_NUMBER, "Emission for call `%tz` resulted in reserved encoding `push %uc` in %t:%uz:%uz [%temit_reserved_encoding]", &source[call->token->offset], call->token->length, CIO_OPERAND_MAX, source_file, line, column, warning_settings->fatal_warnings ? "fatal_warnings, " : "");
                            }
                        }

                        if(call->parameters[k] > CIO_OPERAND_MAX && warning_settings->parameter_overflow) {
                            size_t line = 0;
                            size_t column = 0;

                            error = cio_line_from_offset(call->token->offset, &line, source, source_length);
                            if(error) return error;

                            error = cio_column_from_offset(call->token->offset, &column, source, source_length);
                            if(error) return error;

                            error = gen_log_formatted(warning_settings->fatal_warnings ? GEN_LOG_LEVEL_FATAL : GEN_LOG_LEVEL_WARNING, "cionom", "Emission for call `%tz` resulted in a value greater than the maximum encodable value `%uc` in %t:%uz:%uz [%tparameter_overflow]", &source[call->token->offset], call->token->length, CIO_OPERAND_MAX, source_file, line, column, warning_settings->fatal_warnings ? "fatal_warnings, " : "");
                            if(error) return error;

                            if(warning_settings->fatal_warnings) {
                                return gen_error_attach_backtrace_formatted(GEN_ERROR_TOO_LONG, GEN_LINE_NUMBER, "Emission for call `%tz` resulted in a value greater than the maximum encodable value `%uc` in %t:%uz:%uz [%tparameter_overflow]", &source[call->token->offset], call->token->length, CIO_OPERAND_MAX, source_file, line, column, warning_settings->fatal_warnings ? "fatal_warnings, " : "");
                            }
                        }

                        // Emit push for each param
                        code[code_size + k + 1] = (cio_instruction_t) {(uint8_t) call->parameters[k], CIO_PUSH};

                        // gen_log_formatted(GEN_LOG_LEVEL_DEBUG, "cionom", "Emitting `push %uz` (%uc)", call->parameters[k], *(uint8_t*) &code[code_size + k + 1]);
                    }

                    code_size += call->parameters_length + 2;
                }

				size_t called = SIZE_MAX;

				// Locate called routine's index
				for(size_t k = 0; k < program->routines_length; ++k) {
					bool equal = false;

					error = gen_string_compare(call->identifier, GEN_STRING_NO_BOUNDS, program->routines[k].identifier, GEN_STRING_NO_BOUNDS, GEN_STRING_NO_BOUNDS, &equal);
					if(error) return error;

					if(equal) {
						called = k;
						break;
					}
				}

                // Call to undeclared/undefined routine
				if(called == SIZE_MAX) {
                    size_t line = 0;
                    size_t column = 0;

                    error = cio_line_from_offset(call->token->offset, &line, source, source_length);
                    if(error) return error;

                    error = cio_column_from_offset(call->token->offset, &column, source, source_length);
                    if(error) return error;

                    error = gen_log_formatted(GEN_LOG_LEVEL_FATAL, "cionom", "Call to undeclared or undefined routine `%t` in %tz:%uz:%uz", call->identifier, source_file, source_file_length, line, column);
                    if(error) return error;

                    return gen_error_attach_backtrace_formatted(GEN_ERROR_NO_SUCH_OBJECT, GEN_LINE_NUMBER, "Call to undeclared or undefined routine `%t` in %tz:%uz:%uz", call->identifier, source_file, source_file_length, line, column);
                }

                if(call->parameters_length != program->routines[called].parameters && warning_settings->parameter_count_mismatch) {
                    size_t line = 0;
                    size_t column = 0;

                    error = cio_line_from_offset(call->token->offset, &line, source, source_length);
                    if(error) return error;

                    error = cio_column_from_offset(call->token->offset, &column, source, source_length);
                    if(error) return error;

                    error = gen_log_formatted(warning_settings->fatal_warnings ? GEN_LOG_LEVEL_FATAL : GEN_LOG_LEVEL_WARNING, "cionom", "Call `%tz` to routine `%t` with %uz parameters did not match routine parameter count %uz in %t:%uz:%uz [%tparameter_count_mismatch]", &source[call->token->offset], call->token->length, call->identifier, call->parameters_length, program->routines[called].parameters, source_file, line, column, warning_settings->fatal_warnings ? "fatal_warnings, " : "");
                    if(error) return error;

                    if(warning_settings->fatal_warnings) {
                        return gen_error_attach_backtrace_formatted(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "Call `%tz` to routine `%t` with %uz parameters did not match routine parameter count %uz in %t:%uz:%uz [%tparameter_count_mismatch]", &source[call->token->offset], call->token->length, call->identifier, call->parameters_length, program->routines[called].parameters, source_file, line, column, warning_settings->fatal_warnings ? "fatal_warnings, " : "");
                    }
                }

				// Emit call
				code[code_size - 1] = (cio_instruction_t) {(uint8_t) called, CIO_CALL};
			}

			error = gen_memory_reallocate_zeroed((void**) &code, code_size, code_size + 1, sizeof(unsigned char));
			if(error) return error;

			// Emit return
			code[code_size++] = (cio_instruction_t) {CIO_OPERAND_MAX, CIO_RET};
		}

        // TODO: Technically this is the limit for the *start* of a routine
        //       and the contents of a really long routine can extend beyond
        //       the external routine limit.
        //       This also might make more sense as a warning by default as
        //       we can just continue codegen past this point creating
        //       unaddressable routines.
		if(code_size >= CIO_ROUTINE_EXTERNAL) {
            error = gen_log_formatted(GEN_LOG_LEVEL_FATAL, "cionom", "Emitted code section size %uz exceeds maximum of %uz allowed by bytecode format in %t", code_size, (size_t) CIO_ROUTINE_EXTERNAL, source_file);
            if(error) return error;

            return gen_error_attach_backtrace_formatted(GEN_ERROR_TOO_LONG, GEN_LINE_NUMBER, "Emitted code section size %uz exceeds maximum of %uz allowed by bytecode format in %t", code_size, (size_t) CIO_ROUTINE_EXTERNAL, source_file);
        }
	}

	size_t header_size = sizeof(cio_header_t);
	GEN_CLEANUP_FUNCTION(cio_module_internal_emit_cleanup_header) cio_header_t* header = NULL;

	// Header
	{
		error = gen_memory_allocate_zeroed((void**) &header, header_size, 1);
		if(error) return error;

        // Output routine table length
		header->routine_table_length = (uint8_t) program->routines_length;

		for(size_t i = 0; i < program->routines_length; ++i) {
			const cio_routine_t* const routine = &program->routines[i];

            // Get the length of the routine identifier
            size_t identifier_length = 0;
            error = gen_string_length(routine->identifier, GEN_STRING_NO_BOUNDS, GEN_STRING_NO_BOUNDS, &identifier_length);
            if(error) return error;

            // Allocate an entry in the routine table
            error = gen_memory_reallocate_zeroed((void**) &header, header_size, header_size + sizeof(cio_routine_table_entry_t) + identifier_length + 1, 1);
            if(error) return error;

#ifdef __ANALYZER
            cio_routine_table_entry_t* entry = malloc(sizeof(cio_routine_table_entry_t));
#else
            cio_routine_table_entry_t* entry = (cio_routine_table_entry_t*) &header->routine_table[header_size - 1];
#endif

            // Set routine code offset
            entry->offset = offsets[i];

            // Copy in routine identifier
            error = gen_string_copy(entry->name, identifier_length + 1, routine->identifier, identifier_length + 1, identifier_length);
            if(error) return error;

            header_size += sizeof(cio_routine_table_entry_t) + identifier_length + 1;
		}
	}

	*out_bytecode_length = header_size + code_size;
	error = gen_memory_allocate_zeroed((void**) out_bytecode, *out_bytecode_length, sizeof(unsigned char));
	if(error) return error;
    GEN_CLEANUP_FUNCTION(cio_module_internal_emit_cleanup_bytecode) unsigned char* bytecode_cleanup = *out_bytecode;

	error = gen_memory_copy(*out_bytecode, *out_bytecode_length, header, header_size, header_size);
	if(error) return error;

	if(code) {
		error = gen_memory_copy(*out_bytecode + header_size, *out_bytecode_length - header_size, code, code_size, code_size);
		if(error) return error;
	}

    bytecode_cleanup = NULL;

	return NULL;
}
