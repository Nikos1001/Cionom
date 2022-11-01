// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Emily "TTG" Banerjee <prs.ttg+cionom@pm.me>

#include <cioextlib.h>
#include <cionom.h>

#include <genmemory.h>

CIO_EXTLIB_BEGIN_DEFS

//* `copy*[+]=c` - Copy value into pointer indexed.
//* @note Treats pointer and value as a char.
//* @param [0] The stack index containing the pointer to copy to.
//* @param [1] The index to apply to pointer.
//* @param [2] The value to copy.
//* @reserve Empty.
gen_error_t* copy__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_left_bracket__cionom_mangled_grapheme_plus__cionom_mangled_grapheme_right_bracket__cionom_mangled_grapheme_equalsc(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) copy__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_left_bracket__cionom_mangled_grapheme_plus__cionom_mangled_grapheme_right_bracket__cionom_mangled_grapheme_equalsc, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `GEN_NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	((char*) caller[current[0]])[current[1]] = (char) current[2];

	return GEN_NULL;
}

//* `copy*[+v]=c` - Copy value into pointer variably indexed.
//* @note Treats pointer and value as a char.
//* @param [0] The stack index containing the pointer to copy to.
//* @param [1] The stack index containing the index to apply to pointer.
//* @param [2] The value to copy.
//* @reserve Empty.
gen_error_t* copy__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_left_bracket__cionom_mangled_grapheme_plusv__cionom_mangled_grapheme_right_bracket__cionom_mangled_grapheme_equalsc(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) copy__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_left_bracket__cionom_mangled_grapheme_plusv__cionom_mangled_grapheme_right_bracket__cionom_mangled_grapheme_equalsc, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `GEN_NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	((char*) caller[current[0]])[caller[current[1]]] = (char) current[2];

	return GEN_NULL;
}


//* `copy*[+v]=v` - Copy variable into pointer variably indexed.
//* @note Treats pointer and value as a gen_size_t.
//* @param [0] The stack index containing the pointer to copy to.
//* @param [1] The stack index containing the index to apply to pointer.
//* @param [2] The stack index containing the value to copy.
//* @reserve Empty.
gen_error_t* copy__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_left_bracket__cionom_mangled_grapheme_plusv__cionom_mangled_grapheme_right_bracket__cionom_mangled_grapheme_equalsv(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) copy__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_left_bracket__cionom_mangled_grapheme_plusv__cionom_mangled_grapheme_right_bracket__cionom_mangled_grapheme_equalsc, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `GEN_NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	((gen_size_t*)caller[current[0]])[caller[current[1]]] = caller[current[2]];

	return GEN_NULL;
}

//* `copy=*[+]c` - Copy value from pointer indexed.
//* @note Treats pointer and value as a char.
//* @param [0] The stack index to copy to.
//* @param [1] The stack index containing the pointer to copy from.
//* @param [2] The index to apply to pointer.
//* @reserve Empty.
gen_error_t* copy__cionom_mangled_grapheme_equals__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_left_bracket__cionom_mangled_grapheme_plus__cionom_mangled_grapheme_right_bracketc(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) copy__cionom_mangled_grapheme_equals__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_left_bracket__cionom_mangled_grapheme_plus__cionom_mangled_grapheme_right_bracketc, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `GEN_NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	*(char*) &caller[current[0]] = ((char*) caller[current[1]])[current[2]];

	return GEN_NULL;
}

//* `copy=*[+v]` - Copy value from pointer indexed.
//* @note Treats pointer and value as a gen_size_t.
//* @param [0] The stack index to copy to.
//* @param [1] The stack index containing the pointer to copy from.
//* @param [2] The stack index containing the offset to apply. 
//* @reserve Empty.
gen_error_t* copy__cionom_mangled_grapheme_equals__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_left_bracket__cionom_mangled_grapheme_plusv__cionom_mangled_grapheme_right_bracket(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) copy__cionom_mangled_grapheme_equals__cionom_mangled_grapheme_asterisk__cionom_mangled_grapheme_left_bracket__cionom_mangled_grapheme_plus__cionom_mangled_grapheme_right_bracketc, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `GEN_NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	caller[current[0]] = ((gen_size_t*)caller[current[1]])[caller[current[2]]];

	return GEN_NULL;
}

//* `copy=` -  Copy value into stack.
//* @param [0] The stack index to copy into.
//* @param [1] The value to copy.
//* @reserve Empty.
gen_error_t* copy__cionom_mangled_grapheme_equals(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) copy__cionom_mangled_grapheme_equals, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `GEN_NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

	caller[current[0]] = current[1];

	return GEN_NULL;
}

//* `copy=v` -  Copy variable into variable stack index.
//* @param [0] The stack index to copy into.
//* @param [1] The stack index to copy from.
//* @reserve Empty.
gen_error_t* copy__cionom_mangled_grapheme_equalsv(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) copy__cionom_mangled_grapheme_equalsv, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `GEN_NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

    caller[current[0]] = caller[current[1]];

	return GEN_NULL;
}

//* `copy=cvv` -  Copy variable from caller stack frame into variable stack index.
//* @param [0] The stack index to copy into.
//* @param [1] The stack index of the stack index in the caller to copy from.
//* @reserve Empty.
gen_error_t* copy__cionom_mangled_grapheme_equalscvv(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) copy__cionom_mangled_grapheme_equalscvv, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `GEN_NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller_caller, 2);

    caller[current[0]] = caller_caller[caller[current[1]]];

	return GEN_NULL;
}

//* `callv` - Call a routine at an index. The `callv` call itself is transparent and acts as if the function itself was called except the first parameter.
//* TODO: Add variadic annotation on invocations of this once implemented .
//* @param [0] The stack index of the routine index to call.
//* @param [...] The parameters to the called routine.
//* @reserve The reserve value of the called routine.
gen_error_t* callv(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) callv, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `GEN_NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

    // Store all needed state for constructing call
    gen_size_t callee = caller[current[0]];
    gen_size_t* parameters = GEN_NULL;
    gen_size_t parameters_length = current_frame->height - 1;
    error = gen_memory_allocate_zeroed((void**) &parameters, parameters_length, sizeof(gen_size_t));
	if(error) return error;
    gen_size_t parameters_size = parameters_length * sizeof(gen_size_t);
    error = gen_memory_copy(parameters, parameters_size, &current[1], parameters_size, parameters_size);
	if(error) return error;

    // Destroy current frame
    error = cio_vm_pop_frame(vm);
	if(error) return error;

    // Push parameters into child frame
    for(gen_size_t i = 0; i < parameters_length; ++i) {
        error = cio_vm_push(vm);
    	if(error) return error;
        caller[caller_frame->height - 1] = parameters[i];
    }
    // Orphan parameters
    caller_frame->height -= parameters_length;

    // Execute routine on child frame
    error = cio_vm_dispatch_call(vm, callee, parameters_length);
	if(error) return error;

    // Push an empty frame so `cio_vm_dispatch_call` can correctly restore state
    error = cio_vm_push_frame(vm);
	if(error) return error;

    return GEN_NULL;
}


//* `rcall*` - Call a routine at a symbol. The `rcall*` call itself is transparent and acts as if the function itself was called except the first parameter.
//* TODO: Add variadic annotation on invocations of this once implemented .
//* @param [0] The stack index of a pointer to the start of a buffer containing the symbol name of the routine to call.
//* @param [...] The parameters to the called routine.
//* @reserve The reserve value of the called routine.
gen_error_t* rcall__cionom_mangled_grapheme_asterisk(cio_vm_t* const restrict vm) {
	GEN_TOOLING_AUTO gen_error_t* error = gen_tooling_push(GEN_FUNCTION_NAME, (void*) rcall__cionom_mangled_grapheme_asterisk, GEN_FILE_NAME);
	if(error) return error;

	if(!vm) return gen_error_attach_backtrace(GEN_ERROR_INVALID_PARAMETER, GEN_LINE_NUMBER, "`vm` was `GEN_NULL`");

	CIO_EXTLIB_GET_FRAME_EHD(vm, current, 0);
	CIO_EXTLIB_GET_FRAME_EHD(vm, caller, 1);

    // Store all needed state for constructing call
    char* callee = (char*) caller[current[0]];
    gen_size_t* parameters = GEN_NULL;
    gen_size_t parameters_length = current_frame->height - 1;
    error = gen_memory_allocate_zeroed((void**) &parameters, parameters_length, sizeof(gen_size_t));
	if(error) return error;
    gen_size_t parameters_size = parameters_length * sizeof(gen_size_t);
    error = gen_memory_copy(parameters, parameters_size, &current[1], parameters_size, parameters_size);
	if(error) return error;

    // Destroy current frame
    error = cio_vm_pop_frame(vm);
	if(error) return error;

    // Push parameters into child frame
    for(gen_size_t i = 0; i < parameters_length; ++i) {
        error = cio_vm_push(vm);
    	if(error) return error;
        caller[caller_frame->height - 1] = parameters[i];
    }
    // Orphan parameters
    caller_frame->height -= parameters_length;

    // Execute routine on child frame
    cio_callable_t* callable = GEN_NULL;
    error = cio_vm_get_identifier(vm, callee, &callable, gen_false);
	if(error) return error;
    error = cio_vm_dispatch_callable(vm, callable, parameters_length);
	if(error) return error;

    // Push an empty frame so `cio_vm_dispatch_call` can correctly restore state
    error = cio_vm_push_frame(vm);
	if(error) return error;

    return GEN_NULL;
}

CIO_EXTLIB_END_DEFS
