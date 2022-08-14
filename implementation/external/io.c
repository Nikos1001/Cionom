// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+cionom@pm.me>

#include <cioextlib.h>
#include <cionom.h>

CIO_EXTLIB_BEGIN_DEFS

//* `printc*` - Print pointer.
//* @param [0] The stack index containing the pointer to the first character of a NULL-terminated string.
//* @reserve Empty.
gen_error_t printc__cionom_mangled_grapheme_asterisk(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(printc__cionom_mangled_grapheme_asterisk);

	GEN_NULL_CHECK(vm);

	// gen_error_t error = cio_vm_dump_stack(vm);
	// GEN_ERROR_OUT_IF(error, "`cio_vm_dump_stack` failed");

	glogf(INFO, "%s", (char*) vm->stack[vm->frames[vm->frames_used - 2].base + vm->stack[vm->frames[vm->frames_used - 1].base + vm->frames[vm->frames_used - 1].height - 1]]);

	GEN_ALL_OK;
}

//* `printn` - Print value from stack.
//* @param [0] The stack index containing the value to print.
//* @reserve Empty.
gen_error_t printn(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(printn);

	GEN_NULL_CHECK(vm);

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	glogf(INFO, "%zu", caller[current[0]]);

	GEN_ALL_OK;
}

//* `printc` - Print value from stack as a character.
//* @param [0] The stack index containing the value to print.
//* @reserve Empty.
gen_error_t printc(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(printc);

	GEN_NULL_CHECK(vm);

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	glogf(INFO, "%c", (char) caller[current[0]]);

	GEN_ALL_OK;
}

//* `readn` - Read number from console input.
//* @reserve The read value.
gen_error_t readn(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(readn);

	GEN_NULL_CHECK(vm);

	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	scanf("%zu", &caller[caller_frame->height - 1]);
	GEN_ERROR_OUT_ERRNO(scanf, errno);

	GEN_ALL_OK;
}

//* `readc*` - Read null terminated string of characters.
//* @param [0] The stack index of a pointer to a buffer in which to store read characters.
//* @param [1] The number of characters to read.
//* @reserve Empty.
gen_error_t readc__cionom_mangled_grapheme_asterisk(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(readc__cionom_mangled_grapheme_asterisk);

	GEN_NULL_CHECK(vm);

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	char* buff = (char*) caller[current[0]];

	fgets(buff, (int) current[1], stdin);
	GEN_ERROR_OUT_IF_ERRNO(fgets, errno);

	size_t length = 0;
	gen_error_t error = gen_string_length(buff, GEN_STRING_NO_BOUND, current[1], &length);
	GEN_ERROR_OUT_IF(error, "`gen_string_length` failed");
	if(buff[length - 1] == '\n') buff[length - 1] = '\0';

	GEN_ALL_OK;
}

CIO_EXTLIB_END_DEFS
