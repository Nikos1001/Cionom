// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

#define GEN_TESTS_UNIT "vm"
#include <gentests.h>
#include <genmemory.h>
#include <cionom.h>

extern gen_error_t* printn(cio_vm_t* const restrict vm);
extern gen_error_t* cio_vm_internal_execute_routine(cio_vm_t* const restrict vm);

static gen_error_t* gen_main(void) {
    GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) gen_main, GEN_FILE_NAME);
	if(error) return error;

    const unsigned char bytecode[] = {
        /*
            printn 1

            __cionom_entrypoint 0
            :
                printn 42
            :
         */
        0x02,

        0xFF, 0xFF, 0xFF, 0xFF,
        'p', 'r', 'i', 'n', 't', 'n', '\0',

        0x00, 0x00, 0x00, 0x00,
        '_', '_', 'c', 'i', 'o', 'n', 'o', 'm', '_', 'e', 'n', 't', 'r', 'y', 'p', 'o', 'i', 'n', 't', '\0',

        0x5F, 0x5F, 0x63, 0x69,
        0x6F, 0x6E, 0x6F, 0x6D,
        0x5F, 0x65, 0x6E, 0x74,
        0x72, 0x79, 0x70, 0x6F,
        0x69, 0x6E, 0x74, 0x00,
        0x00, 0x2A, 0x80, 0xFF
    };

    cio_vm_t vm = {0};
    cio_warning_settings_t warning_settings = {0};
    error = gen_memory_set(&warning_settings, sizeof(warning_settings), gen_true);
	if(error) return error;

    error = cio_vm_initialize(bytecode, sizeof(bytecode) / sizeof(bytecode[0]), 1024, gen_true, &vm, gen_false, &warning_settings);
	if(error) return error;

    cio_callable_t* printn_callable = GEN_NULL;
    {
        error = cio_vm_get_identifier(&vm, "printn", &printn_callable, gen_false);
        if(error) return error;

        error = GEN_TESTS_EXPECT(gen_false, printn_callable == GEN_NULL);
        if(error) return error;

        error = GEN_TESTS_EXPECT("printn", printn_callable->identifier);
        if(error) return error;

        error = GEN_TESTS_EXPECT(sizeof("printn") - 1, printn_callable->identifier_length);
        if(error) return error;

        error = GEN_TESTS_EXPECT((void*) printn, (void*) printn_callable->function);
        if(error) return error;

        error = GEN_TESTS_EXPECT(0, printn_callable->bytecode_index);
        if(error) return error;

        error = GEN_TESTS_EXPECT(CIO_ROUTINE_EXTERNAL, printn_callable->offset);
        if(error) return error;

        error = GEN_TESTS_EXPECT(0, printn_callable->routine_index);
        if(error) return error;
    }

    {
        cio_callable_t* callable = GEN_NULL;
        error = cio_vm_get_identifier(&vm, "__cionom_entrypoint", &callable, gen_false);
        if(error) return error;

        error = GEN_TESTS_EXPECT(gen_false, callable == GEN_NULL);
        if(error) return error;

        error = GEN_TESTS_EXPECT("__cionom_entrypoint", callable->identifier);
        if(error) return error;

        error = GEN_TESTS_EXPECT(sizeof("__cionom_entrypoint") - 1, callable->identifier_length);
        if(error) return error;

        error = GEN_TESTS_EXPECT((void*) cio_vm_internal_execute_routine, (void*) callable->function);
        if(error) return error;

        error = GEN_TESTS_EXPECT(0, callable->bytecode_index);
        if(error) return error;

        error = GEN_TESTS_EXPECT(0, callable->offset);
        if(error) return error;

        error = GEN_TESTS_EXPECT(1, callable->routine_index);
        if(error) return error;
    }

    {
        // TODO: Lib stdout redirects in lib-init?

        error = cio_vm_push_frame(&vm);
        if(error) return error;

        error = cio_vm_push(&vm);
        if(error) return error;

        error = cio_vm_push(&vm);
        if(error) return error;

        error = cio_vm_dispatch_callable(&vm, printn_callable, 1);
        if(error) return error;

        error = cio_vm_pop_frame(&vm);
        if(error) return error;
    }

    {
        error = cio_vm_push_frame(&vm);
        if(error) return error;
        error = cio_vm_push(&vm);
        if(error) return error;

        cio_frame_t* frame = {0};
        error = cio_vm_get_frame(&vm, 0, &frame);
        if(error) return error;

        error = GEN_TESTS_EXPECT(gen_false, frame == GEN_NULL);
        if(error) return error;

        error = GEN_TESTS_EXPECT(0, frame->base);
        if(error) return error;
        error = GEN_TESTS_EXPECT(1, frame->height);
        if(error) return error;
        error = GEN_TESTS_EXPECT(0, frame->execution_offset);
        if(error) return error;

        gen_size_t* ptr = GEN_NULL;
        error = cio_vm_get_frame_pointer(&vm, frame, &ptr);
        if(error) return error;

        error = GEN_TESTS_EXPECT(gen_false, ptr == GEN_NULL);
        if(error) return error;

        error = GEN_TESTS_EXPECT((void*) vm.stack, ptr);
        if(error) return error;

        error = cio_vm_pop_frame(&vm);
        if(error) return error;
    }

    error = cio_vm_free(&vm);
    if(error) return error;

    return GEN_NULL;
}
