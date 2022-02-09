// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+genstone@pm.me>

#include "include/cionom.h"

static const char cio_internal_vm_mangled_symbol_keys[] = {
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
    '&'
};
static const char* const cio_internal_vm_mangled_symbol_values[] = {
    "plus",
    "minus",
    "slash",
    "astericks",
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
    "ampersand"
};

static __nodiscard gen_error_t cio_internal_vm_resolve_external(const char* const restrict identifier, cio_routine_function_t* const out_function) {
    GEN_FRAME_BEGIN(cio_internal_vm_resolve_external);

    GEN_INTERNAL_BASIC_PARAM_CHECK(identifier);
    GEN_INTERNAL_BASIC_PARAM_CHECK(out_function);

    size_t identifier_length = 0;
    gen_error_t error = gen_string_length(identifier, GEN_STRING_NO_BOUND, GEN_STRING_NO_BOUND, &identifier_length);
    GEN_ERROR_OUT_IF(error, "`gen_string_length` failed");

    size_t mangled_length = 0;
    char* mangled = NULL;
    GEN_STRING_FOREACH(c, identifier_length, identifier) {
        if(c == '_' || isalnum(c)) {
            error = grealloc(&mangled, mangled_length, mangled_length + 1, sizeof(char));
            mangled[mangled_length++] = c;
            continue;
        }

        const char* value = NULL;

        // Prepend __cionom_mangled_grapheme_
    }

    GEN_ALL_OK;
}

gen_error_t cio_execute_bytecode(const unsigned char* const restrict bytecode, const size_t stack_length, cio_vm_t* const restrict out_instance) {
    GEN_FRAME_BEGIN(cio_execute_bytecode);

    GEN_INTERNAL_BASIC_PARAM_CHECK(bytecode);
    GEN_INTERNAL_BASIC_PARAM_CHECK(out_instance);

    out_instance->stack_length = stack_length;
    gen_error_t error = gzalloc((void**) &out_instance->stack, out_instance->stack_length, sizeof(size_t));
    GEN_ERROR_OUT_IF(error, "`gzalloc` failed");

    error = gfree(out_instance->stack);
    GEN_ERROR_OUT_IF(error, "`gfree` failed");

    GEN_ALL_OK;
}
