# Cíonom Reference Implementation Documentation

## Bytecode

### Bytecode Modules
Each bytecode module (`.ibc`) generally corresponds to a single compiled source file (`.cio`). These consist of a header followed by the bytecode that comprises the module.

#### **Header**
The structure of the module header is as follows:
```
Reserved: 1
Routine Table Length: 1
Routine Table...
```
(See [Extensions](#Extensions) for potential cases where reserved fields may be used)

Where a routine table entry is as follows:
```
Offset: 32 || This will be 0xFFFFFFFF for externally resolved routines
Identifier... || A C-string containing the unmangled symbol name
```

#### **Code**
Routines within the code section are output back-to-back with no metadata or rising/falling edge other than a single terminating `0xFF` signifying a return.

Each opcode-operand pairing is 1 byte where the most significant/leftmost bit is the opcode and the remainder of the byte is an operand.

Opcode 0 corresponds to a push and 1 refers to a call - hence the return byte could be thought of as `call 0x7F`. The encoding `push 0x7F` is reserved (See [Extensions](#Extensions) for potential cases where it may be used).

The remaining 7 operand bits for a `call` instruction correspond to the index into the routine table for the routine to be called.
The operand bits for a `push` instruction correspond to the value to push.

### Executable Bundles

The Cíonom VM under the reference implementation can consume a set of concatonated bytecode modules as a single bundled executable (`.cbe`). This allows for the resolution of symbols between bytecode files.

This allows a program to circumvent the limitations of individual Cíonom modules.

## Extensions

### Header
The first bit of a Cíonom bytecode module is reserved for use by the implementation. The reference implementation makes use of this bit to indicate the presence extensions.

Each extension is indicated as follows:

```
Continuation: 1
ID: 7
Data...
```

The continuation bit can be set to indicate that there is another extension after the first - this can be chained.

Some extensions are header-only and others also operate in the code section of the bytecode module aswell. The latter do not neccesarily need to be remarked in the header but failure to do so may result in a diagnostic and is not guaranteed to work across releases.

In-code extensions use a secondary mechanism documented [below](#Code).

The extensions currently supported by this are as follows (extensions which do not require runtime support are not listed):
Name|Option|ID|Header Data|Code Data|Description
---|---|---|---|---|---
Elide Reserve Space|`--extension=elide-reserve-space`|`0`|-|-|Allows the VM to be informed that reserve space has been elided with a call
Constants|`--extension=constants`|`2`|The constant data|-|Allows the insertion of file contents into the module header. This may be listed multiple times to embed multiple files
Nil Calls|`--extension=nil-calls`|`3`|The routine indices for `__cionom_extension_nil_call` and `__cionom_extension_nil_call_frame`|-|Allows the insertion of full/partial no-op calls
Breakpoints|`--extension=breakpoints`|`4`|-|-|Allows halting of code execution to return to a debugger attached to a running program
Debug Info|`--extension=debug-info`|5|The debug info|-|Allows the insertion of extra information into the module header to aid with debugging
Encode Stack Length|`--extension=encode-stack-length`|6|The stack length|-|Informs the VM of the preferred stack length for the bytecode module. In [bundled executables](#Executable-Bundles) this only applies to the first module in the bundle

### Code
The encoding `push 0x7F` is reserved for use by the implementation. The reference implementation uses this to provide extension mechanisms without modifying the existing bytecode format.

The use of an extension is encoded as follows:
```
push id
[push data]
push 0x7F
```
(Routine IDs and the format of extension data is detailed in the table above)

## Command Line Reference

### Basic Usage

The typical workflow for compiling and executing the Cíonom source file `examples/hello_world.cio` via. the reference implementation is as follows:
```
cionom-cli --emit-bytecode=a.ibc examples/hello_world.cio
cionom-cli --execute-bundle=main --stack-length=32 a.ibc
```
```
[cionom            ] [info    ] Hello, world!
```
