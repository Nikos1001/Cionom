// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

#include <cioextlib.h>
#include <cionom.h>

#include <genlog.h>
#include <genstring.h>
#include <genfilesystem.h>

GEN_PRAGMA(GEN_PRAGMA_DIAGNOSTIC_REGION_BEGIN)
GEN_PRAGMA(GEN_PRAGMA_DIAGNOSTIC_REGION_IGNORE("-Weverything"))
#include <stdio.h>
GEN_PRAGMA(GEN_PRAGMA_DIAGNOSTIC_REGION_END)

CIO_EXTLIB_BEGIN_DEFS

//* `printc*` - Print pointer.
//* @param [0] The stack index containing the pointer to the first character of a NULL-terminated string.
//* @reserve Empty.
gen_error_t* printc__cionom_mangled_grapheme_asterisk(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) printc__cionom_mangled_grapheme_asterisk, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom", "%t", (char*) caller[current[0]]);
	if(error) return error;

	return NULL;
}

//* `printn` - Print value.
//* @param [0] The value to print.
//* @reserve Empty.
gen_error_t* printn(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) printn, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom", "%uz", (size_t) current[0]);
	if(error) return error;

	return NULL;
}

//* `printnv` - Print value from stack.
//* @param [0] The stack index containing the value to print.
//* @reserve Empty.
gen_error_t* printnv(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) printnv, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom", "%uz", (size_t) caller[current[0]]);
	if(error) return error;

	return NULL;
}

//* `printc` - Print value from stack as a character.
//* @param [0] The stack index containing the value to print.
//* @reserve Empty.
gen_error_t* printc(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) printc, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	error = gen_log_formatted(GEN_LOG_LEVEL_INFO, "cionom", "%c", (char) caller[current[0]]);
	if(error) return error;

	return NULL;
}

//* `readn` - Read number from console input.
//* @reserve The read value.
gen_error_t* readn(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) readn, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

    char buffer[32 + 1] = {0};

    error = gen_filesystem_handle_file_read(&GEN_FILESYSTEM_HANDLE_STDIN, 0, sizeof(buffer) - 1, (unsigned char*) buffer);
    if(error) return error;

    for(size_t i = sizeof(buffer) - 1; i != SIZE_MAX; --i) {
        char c = buffer[i];
        if(c == ' ' || c == '\t' || c == '\n' || c == '\r') buffer[i] = '\0';
    }

    error = gen_string_number(buffer, sizeof(buffer), GEN_STRING_NO_BOUNDS, &caller[caller_frame->height - 1]);
    if(error) return error;

	return NULL;
}

//* `readc*` - Read null terminated string of characters.
//* @param [0] The stack index of a pointer to a buffer in which to store read characters.
//* @param [1] The number of characters to read.
//* @reserve Empty.
gen_error_t* readc__cionom_mangled_grapheme_asterisk(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) readc__cionom_mangled_grapheme_asterisk, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	char* buff = (char*) caller[current[0]];

	char* result = fgets(buff, (int) current[1], stdin);
    if(!result) return gen_error_attach_backtrace_formatted(gen_error_type_from_errno(), GEN_LINE_NUMBER, "Could not read string from console: %t", gen_error_description_from_errno());

	size_t length = 0;
	error = gen_string_length(buff, GEN_STRING_NO_BOUNDS, current[1], &length);
	if(error) return error;

	if(buff[length - 1] == '\n') buff[length - 1] = '\0';

	return NULL;
}

CIO_EXTLIB_END_DEFS
