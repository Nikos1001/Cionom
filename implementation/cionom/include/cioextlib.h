// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

#include "cionom.h"

/**
 * Begins a block of runtime library definitions.
 */
#define CIO_EXTLIB_BEGIN_DEFS \
    GEN_PRAGMA(GEN_PRAGMA_DIAGNOSTIC_REGION_BEGIN) \
    GEN_PRAGMA(GEN_PRAGMA_DIAGNOSTIC_REGION_IGNORE("-Wmissing-prototypes")) \
    GEN_PRAGMA(GEN_PRAGMA_DIAGNOSTIC_REGION_IGNORE("-Wreserved-identifier")) \

/**
 * Ends a block of runtime library definitions.
 */
#define CIO_EXTLIB_END_DEFS GEN_PRAGMA(GEN_PRAGMA_DIAGNOSTIC_REGION_END)

/**
 * Gets a frame and its data into the local frame.
 * @note uses and performs error handling on `cio_vm_get_frame` and `cio_vm_get_frame_pointer`.
 * @param vm the vm pointer from which to fetch frames.
 * @param name the name to extract the frame's data pointer as, and the frame pointer as `name_frame`.
 * @param offset the offset of the stack frame back from the current frame.
 */
#define CIO_EXTLIB_GET_FRAME_EHD(vm, name, offset) \
    cio_frame_t* name##_frame = NULL; \
	size_t* name = NULL; \
	do { \
        error = cio_vm_get_frame(vm, offset, &name##_frame); \
        if(error) return error; \
        error = cio_vm_get_frame_pointer(vm, name##_frame, &name); \
        if(error) return error; \
    } while(0)

