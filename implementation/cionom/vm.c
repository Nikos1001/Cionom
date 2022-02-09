// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+genstone@pm.me>

#include "include/cionom.h"

static const char cio_internal_vm_mangled_grapheme_keys[] = {
	'+',
	'-',
	'/',
	'*',
	'=',
	'!',
	'#',
	'|',
	'\\',
	'\"',
	'\'',
	';',
	':',
	'`',
	'~',
	'.',
	',',
	'<',
	'>',
	'[',
	']',
	'{',
	'}',
	'(',
	')',
	'@',
	'$',
	'^',
	'%',
	'&'};
static const char* const cio_internal_vm_mangled_grapheme_values[] = {
	"plus",
	"minus",
	"slash",
	"asterisk",
	"equals",
	"bang",
	"hash",
	"pipe",
	"backslash",
	"double_quote",
	"single_quote",
	"semicolon",
	"colon",
	"backtick",
	"tilde",
	"full_stop",
	"comma",
	"left_chevron",
	"right_chevron",
	"left_bracket",
	"right_bracket",
	"left_brace",
	"right_brace",
	"left_parenthesis",
	"right_parenthesis",
	"at",
	"dollar",
	"circumflex",
	"percentage",
	"ampersand"};
static const char cionom_internal_vm_mangled_grapheme_prefix[] = "__cionom_mangled_grapheme_";

static __nodiscard gen_error_t cio_internal_vm_resolve_external(const char* const restrict identifier, cio_routine_function_t* const out_function, const gen_dylib_t lib) {
	GEN_FRAME_BEGIN(cio_internal_vm_resolve_external);

	GEN_INTERNAL_BASIC_PARAM_CHECK(identifier);
	GEN_INTERNAL_BASIC_PARAM_CHECK(out_function);

	size_t identifier_length = 0;
	gen_error_t error = gen_string_length(identifier, GEN_STRING_NO_BOUND, GEN_STRING_NO_BOUND, &identifier_length);
	GEN_ERROR_OUT_IF(error, "`gen_string_length` failed");

	size_t mangled_length = 0;
	char* mangled = NULL;
	GEN_STRING_FOREACH(c, identifier_length, identifier) {
		if(*c == '_' || isalnum(*c)) {
			error = grealloc((void**) &mangled, mangled_length, mangled_length + 2, sizeof(char));
			GEN_ERROR_OUT_IF(error, "`grealloc` failed");
			mangled[mangled_length++] = *c;
			continue;
		}

		const char* mangled_grapheme_value = NULL;
		GEN_STRING_FOREACH(key, sizeof(cio_internal_vm_mangled_grapheme_keys), cio_internal_vm_mangled_grapheme_keys) {
			if(*c == *key) {
				mangled_grapheme_value = cio_internal_vm_mangled_grapheme_values[key - cio_internal_vm_mangled_grapheme_keys];
				break;
			}
		}

		if(!mangled_grapheme_value) GEN_ERROR_OUT(GEN_BAD_CONTENT, "Invalid character encountered while mangling symbol");

		size_t mangled_grapheme_length = 0;
		error = gen_string_length(mangled_grapheme_value, GEN_STRING_NO_BOUND, GEN_STRING_NO_BOUND, &mangled_grapheme_length);
		GEN_ERROR_OUT_IF(error, "`gen_string_length` failed");

		error = grealloc((void**) &mangled, mangled_length, mangled_length + (sizeof(cionom_internal_vm_mangled_grapheme_prefix) - 1) + mangled_grapheme_length + 1, sizeof(char));
		GEN_ERROR_OUT_IF(error, "`grealloc` failed");

		mangled_length += (sizeof(cionom_internal_vm_mangled_grapheme_prefix) - 1) + mangled_grapheme_length;

		error = gen_string_append(mangled, mangled_length + 1, cionom_internal_vm_mangled_grapheme_prefix, sizeof(cionom_internal_vm_mangled_grapheme_prefix), sizeof(cionom_internal_vm_mangled_grapheme_prefix) - 1);
		GEN_ERROR_OUT_IF(error, "`gen_string_append` failed");
		error = gen_string_append(mangled, mangled_length + 1, mangled_grapheme_value, mangled_grapheme_length + 1, mangled_grapheme_length);
		GEN_ERROR_OUT_IF(error, "`gen_string_append` failed");
	}

	glogf(DEBUG, "Mangling external identifier `%s` as `%s`", identifier, mangled);

	error = gen_dylib_symbol((void*) out_function, lib, mangled);
	GEN_ERROR_OUT_IF(error, "`gen_dylib_symbol` failed");

	GEN_ALL_OK;
}

static __nodiscard gen_error_t cio_internal_vm_push_frame(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(cio_internal_vm_push_frame);

	GEN_INTERNAL_BASIC_PARAM_CHECK(vm);

	gen_error_t error = grealloc((void**) &vm->frames, vm->frames_length, vm->frames_length + 1, sizeof(cio_frame_t));
	GEN_ERROR_OUT_IF(error, "`grealloc` failed");
	if(vm->frames_length) vm->frames[vm->frames_length].base = vm->frames[vm->frames_length - 1].base + vm->frames[vm->frames_length - 1].height;
	++vm->frames_length;

	GEN_ALL_OK;
}

static __nodiscard gen_error_t cio_internal_vm_pop_frame(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(cio_internal_vm_pop_frame);

	GEN_INTERNAL_BASIC_PARAM_CHECK(vm);

	gen_error_t error = grealloc((void**) vm->frames, vm->frames_length, vm->frames_length - 1, sizeof(cio_frame_t));
	GEN_ERROR_OUT_IF(error, "`grealloc` failed");
	--vm->frames_length;

	GEN_ALL_OK;
}

static __nodiscard gen_error_t cio_internal_vm_push(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(cio_internal_vm_push);

	GEN_INTERNAL_BASIC_PARAM_CHECK(vm);
	if(!vm->frames_length) GEN_ERROR_OUT(GEN_NO_SUCH_OBJECT, "No frame available to push into");

	if(vm->frames[vm->frames_length - 1].base + vm->frames[vm->frames_length - 1].height < vm->stack_length)
		++vm->frames[vm->frames_length - 1].height;
	else
		GEN_ERROR_OUT(GEN_BAD_OPERATION, "Stack overflow");
	vm->stack[vm->frames[vm->frames_length - 1].base + vm->frames[vm->frames_length - 1].height - 1] = 0;

	GEN_ALL_OK;
}

static __nodiscard gen_error_t cio_internal_vm_execute_routine(cio_vm_t* const restrict vm) {
	GEN_FRAME_BEGIN(cio_internal_vm_execute_routine);

	GEN_INTERNAL_BASIC_PARAM_CHECK(vm);

	GEN_ALL_OK;
}

static __nodiscard gen_error_t cio_internal_vm_dispatch_call(cio_vm_t* const restrict vm, const size_t callable, const size_t* const restrict argv, const size_t argc) {
	GEN_FRAME_BEGIN(cio_internal_vm_dispatch_call);

	GEN_INTERNAL_BASIC_PARAM_CHECK(vm);
	if(argc) GEN_INTERNAL_BASIC_PARAM_CHECK(argv);
	if(callable > vm->callables_length) GEN_ERROR_OUT(GEN_OUT_OF_BOUNDS, "`callable` was greater than `vm->callables_length`");

	// Reserve value
	gen_error_t error = cio_internal_vm_push(vm);
	GEN_ERROR_OUT_IF(error, "`cio_internal_vm_push` failed");

	error = cio_internal_vm_push_frame(vm);
	GEN_ERROR_OUT_IF(error, "`cio_internal_vm_push_frame` failed");

	// Dispatch call
	vm->frames[vm->frames_length - 1].execution_offset = vm->callables_offsets[callable];
	error = vm->callables[callable](vm);
	GEN_ERROR_OUT_IF(error, "Routine call failed");

	error = cio_internal_vm_pop_frame(vm);
	GEN_ERROR_OUT_IF(error, "`cio_internal_vm_pop_frame` failed");

	GEN_ALL_OK;
}

gen_error_t cio_execute_bytecode(const unsigned char* const restrict bytecode, const size_t bytecode_length, const size_t stack_length, cio_vm_t* const restrict out_instance) {
	GEN_FRAME_BEGIN(cio_execute_bytecode);

	GEN_INTERNAL_BASIC_PARAM_CHECK(bytecode);
	GEN_INTERNAL_BASIC_PARAM_CHECK(bytecode_length);
	GEN_INTERNAL_BASIC_PARAM_CHECK(out_instance);

	const size_t* const aligned_bytecode = (size_t*) bytecode;

	out_instance->bytecode_length = bytecode_length;
	out_instance->bytecode = aligned_bytecode;

	out_instance->stack_length = stack_length;
	gen_error_t error = gzalloc((void**) &out_instance->stack, out_instance->stack_length, sizeof(size_t));
	GEN_ERROR_OUT_IF(error, "`gzalloc` failed");
	error = gen_dylib_load(&out_instance->external_lib, "cionom-external");
	GEN_ERROR_OUT_IF(error, "`gen_dylib_load` failed");

	out_instance->callables_length = aligned_bytecode[0];
	error = gzalloc((void**) &out_instance->callables, out_instance->callables_length, sizeof(cio_routine_function_t));
	GEN_ERROR_OUT_IF(error, "`gzalloc` failed");
	error = gzalloc((void**) &out_instance->callables_offsets, out_instance->callables_length, sizeof(size_t));
	GEN_ERROR_OUT_IF(error, "`gzalloc` failed");

	size_t current_offset = 1;
	GEN_FOREACH_PTR(i, callable, out_instance->callables_length, out_instance->callables) {
		out_instance->callables_offsets[i] = aligned_bytecode[current_offset];

		++current_offset;
		if(out_instance->callables_offsets[i] == SIZE_MAX) {
			error = cio_internal_vm_resolve_external((char*) &aligned_bytecode[current_offset], callable, out_instance->external_lib);
			GEN_ERROR_OUT_IF(error, "`cio_internal_vm_resolve_external` failed");

			size_t stride = 0;
			error = gen_string_length((char*) &aligned_bytecode[current_offset], GEN_STRING_NO_BOUND, GEN_STRING_NO_BOUND, &stride);
			GEN_ERROR_OUT_IF(error, "`gen_string_length` failed");

			current_offset += (size_t) (ceil((double) stride / (double) sizeof(size_t)));
			continue;
		}

		*callable = cio_internal_vm_execute_routine;
	}

	error = cio_internal_vm_push_frame(out_instance);
	GEN_ERROR_OUT_IF(error, "`cio_internal_vm_push_frame` failed");
	error = cio_internal_vm_push(out_instance);
	GEN_ERROR_OUT_IF(error, "`cio_internal_vm_push` failed");

	error = cio_internal_vm_dispatch_call(out_instance, 0, NULL, 0);
	GEN_ERROR_OUT_IF(error, "`cio_internal_vm_dispatch_call` failed");

	GEN_ALL_OK;
}
