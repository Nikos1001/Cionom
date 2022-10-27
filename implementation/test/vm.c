// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

#define GEN_TESTS_UNIT "vm"
#include <gentests.h>
#include <cionom.h>

static gen_error_t* gen_main(void) {
    GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) gen_main, GEN_FILE_NAME);
	if(error) return error;

// cio_vm_get_identifier
// cio_vm_initialize
// cio_vm_free
// cio_vm_dispatch_call
// cio_vm_push_frame
// cio_vm_pop_frame
// cio_vm_push
// cio_vm_get_frame
// cio_vm_get_frame_pointer
// cio_vm_dump_stack

    return NULL;
}
