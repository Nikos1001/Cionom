# Command Line Reference for Cíonom Reference Implementation

## Basic Usage

The typical workflow for compiling and executing the Cíonom source file `examples/hello_world.cio` via. the reference implementation is as follows:
```
cionom-cli --emit-bytecode=a.ibc examples/hello_world.cio
cionom-cli --execute-bytecode=4 --stack-length=32 a.ibc
```
```
[cionom            ] [info    ] Hello, world!
```

## Options

#### **Note**
*Only one "Action" option may be provided to a single compilation.*

### `--emit-bytecode[=FILE] SOURCE`
#### *Action*
Emits bytecode from compilation of `SOURCE` to the file `FILE`, or `a.ibc` if unspecified.

### `--execute-bytecode=ROUTINE BYTECODE`
#### *Action*
Executes the routine index `ROUTINE` in the `BYTECODE`. Routines are ordered the same as they are in source and are zero-indexed.

### `--stack-length[=LENGTH]`
#### *Setting*
Sets the length of the stack available to the Cíonom VM, or `1024` if unspecified. The number of bytes occupied by the stack will be 8 times this number, plus overhead for stack frame count requirement.

### `--mangle-identifier IDENTIFIER`
#### *Action*
Prints the Cíonom-mangled name of `IDENTIFIER`. For example:
```
cionom-cli --mangle-identifier @=aaa
```
```
[cionom-cli        ] [info    ] Result of mangling "@=aaa" is: `__cionom_mangled_grapheme_at__cionom_mangled_grapheme_equalsaaa`
```
