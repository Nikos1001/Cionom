// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2021 TTG <prs.ttg+genstone@pm.me>

#include "include/cionom.h"

/*
Program Header
===

Symbol Table Header
==
|0 Number of entries|64

Symbol Table Entry
==
|0 External |1 Offset of routine|64

Code
===

Opcodes
==
All opcodes and operands are 64 bits in width

Push: 0x0
    Pushes a new entry onto the local stack frame with a value of 0
PushX: 0x1 [imm]
    Pushes a new entry onto the local stack frame with the value of [imm]
Pop: 0x2
    Removes a value from the local stack frame
Call: 0x3 [imm]
    Pushes a stack frame and jumps execution to the routine referred to by [imm]
*/

gen_error_t cio_emit_bytecode(const cio_program_t* const restrict program, uint8_t** const restrict out_bytecode, size_t* const restrict out_bytecode_length, const char* const restrict source, const size_t source_length, const char* const restrict source_file, const size_t source_file_length) {
    GEN_FRAME_BEGIN(cio_emit_bytecode);

    GEN_INTERNAL_BASIC_PARAM_CHECK(program);
    GEN_INTERNAL_BASIC_PARAM_CHECK(out_bytecode);
    GEN_INTERNAL_BASIC_PARAM_CHECK(out_bytecode_length);
    GEN_INTERNAL_BASIC_PARAM_CHECK(source);
    GEN_INTERNAL_BASIC_PARAM_CHECK(source_file);



    GEN_ALL_OK;
}
