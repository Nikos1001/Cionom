# Command Line Reference for Cíonom Reference Implementation

## Basic Usage

The typical workflow for compiling and executing the Cíonom source file `examples/hello_world.cio` via. the reference implementation is as follows (presuming that neccesary dynamic libraries are available and the `cionom-cli` binary is globally installed, some additional qualification like `LD_LIBRARY_PATH` or `./cionom-cli.out` may be neccesary in practise)
```
cionom-cli --emit-bytecode=a.ibc examples/hello_world.cio
cionom-cli --execute-bytecode=3 --stack-length=32 a.ibc
```

## Options

#### **Note**
*Only one "Action" option may be provided to a single compilation.*

### `--debug`
#### *Setting*
Emits extra debug information where possible. This flag currently has no behaviour.

### `--emit-bytecode[=FILE]`
#### *Action*
Emits bytecode from compilation of provided source file to the file `FILE`, or `a.ibc` if unspecified.

### `--execute-bytecode=ROUTINE`
#### *Action*
Executes the routine index `ROUTINE` in the provided bytecode file. Routines are ordered the same as they are in source and are zero-indexed.

### `--stack-length[=LENGTH]`
#### *Setting*
Sets the length of the stack available to the Cíonom VM, or `1024` if unspecified. The number of bytes occupied by the stack will be 8 times this number, plus overhead for increased stack frame count requirement.

### `--print-mangled-identifier=IDENTIFIER`
#### *Action*
Prints the Cíonom-mangled variant of an identifier. For example:
```
cionom-cli.out --print-mangled-identifier=@\=aaa      
Info: Result of mangling @=aaa is: `__cionom_mangled_grapheme_at__cionom_mangled_grapheme_equalsaaa`
```
