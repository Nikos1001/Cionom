# Cíonom Language Specification
Here lies the specification of the language, aimed to be a relatively human-oriented layout of the language and its features as would be implemented in a conformant implementation.

## Cíonom Grammar Specification
All the following constructs should be delimited by whitespace

### Program
A set of 0 or more [routine declarations](#Routine-Declaration)

### Routine Declaration
An [identifier](#Identifier), followed by a [number](#Number) followed optionally by a [block](#Block)

### Block
A set of 0 or more [calls](#Call), surrounded by `:`

### Call
An [identifier](#Identifier), followed by 0 or more [numbers](#Number)

### Identifier
A sequence of characters not beginning with a digit or `:`

### Number
A sequence of digits

## Semantics

### External Routines
If a [routine declaration](#Routine-Declaration) is not followed by a [block](#Block), it is determined to be external. External routines are resolved by the implementation to either other Cíonom files or foreign code. When resolving foreign code it may be neccesary to mangle Cíonom identifiers due to the allowance of characters whose usage is restricted in external specifications. In this case, the implementation may mangle the identifier by replacing usages of the restricted character `C` with `__cionom_mangled_grapheme_` followed by an appropriate name for `C`. For example, `apple$banana` may be mangled as `apple__cionom_mangled_grapheme_dollarbanana`. For this reason, the implementation may restrict routine identifiers from containing `__cionom`.

### Routine-Local Stack Frame
At the beginning of a routine call, a stack frame is created for the duration of the routine's runtime. This stack frame is initialized to the number of entries described in the [routine declaration](#Routine-Declaration). Every [call](#Call) will create a [reserve entry](#Reserve-Entry) in the stack frame - though this may be removed by the callee.

At the end of the routine's runtime - terminated at the closing `:` of the [block](#Block) or by external code returning - the entire stack frame will be discarded and accessing its contents is undefined.

### Reserve Entry
When a routine dispatches a call to another routine, it pushes a new entry in it's [stack frame](#Routine-Local-Stack-Frame). This can be used by the callee for returning values, or just for the sake of creating more storage. This may be discarded by the callee.

### Call Semantics
When dispatching a call, the parameters to the callee are pushed by the caller into the callee's [stack frame](#Routine-Local-Stack-Frame), to be torn down at the end of the call. Before the call itself is initiated and the callee stack frame is created, the caller should push a [reserve entry](#Reserve-Entry) into its own stack frame.
