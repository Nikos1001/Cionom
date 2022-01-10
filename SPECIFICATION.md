# Cíonom Language Specification
Here lies the specification of the language, aimed to be a relatively human-oriented layout of the language and its features as would be implemented in a conformant implementation - including some extensions an implementation might implement.

## Table of Contents
- [Cíonom Language Specification](#cíonom-language-specification)
  - [Table of Contents](#table-of-contents)
  - [Cíonom Grammar Specification](#cíonom-grammar-specification)
    - [Program](#program)
    - [Routine Declaration](#routine-declaration)
    - [Block](#block)
    - [Statement](#statement)
    - [Call](#call)
    - [Storage Declaration](#storage-declaration)
    - [Storage Specifier](#storage-specifier)
    - [Alignment Specifier](#alignment-specifier)
    - [Specifier Expression](#specifier-expression)
    - [Expression](#expression)
    - [Identifier](#identifier)
    - [Number](#number)
    - [String](#string)
  - [Cíonom Formation Semantics](#cíonom-formation-semantics)
    - [Program](#program-1)
    - [Routine Declaration](#routine-declaration-1)
    - [Block](#block-1)
    - [Statement](#statement-1)
    - [Call](#call-1)
    - [Storage Declaration](#storage-declaration-1)
    - [Storage Specifier](#storage-specifier-1)
    - [Alignment Specifier](#alignment-specifier-1)
    - [Specifier Expression](#specifier-expression-1)
    - [Expression](#expression-1)
    - [Identifier](#identifier-1)
    - [Number](#number-1)
    - [String](#string-1)
  - [Cíonom Language Extensions](#cíonom-language-extensions)
    - [C Preprocessor](#c-preprocessor)
    - [String Escape Sequences](#string-escape-sequences)
    - [Nonstandard Specifiers](#nonstandard-specifiers)
      - [Constant Marking](#constant-marking)
      - [Storage Strategy](#storage-strategy)
    - [Shadowing](#shadowing)
    - [Statement Qualification](#statement-qualification)
      - [Call Discard Constant](#call-discard-constant)
    - [Block Qualification](#block-qualification)
    - [Routine Qualification](#routine-qualification)
      - [Identifier Mangling](#identifier-mangling)
    - [C Arithmetic in Specifier Expressions](#c-arithmetic-in-specifier-expressions)
    - [Initializer Expressions](#initializer-expressions)
    - [Comments](#comments)
    - [Intrinsic Branch](#intrinsic-branch)
    - [Source Location](#source-location)

## Cíonom Grammar Specification

### Program
A set of 0 or more [routine declarations](#Routine-Declarations)

### Routine Declaration
An [identifier](#Identifier), optionally followed by 0 or more [storage declarations](#Storage-Declaration) delimited by `,`, optionally followed by a [block](#Block) otherwise a `;`.

### Block
A set of 0 or more [statements](#Statement), surrounded by `{` and `}`

### Statement
A [call](#Call), `return` or a [storage declaration](#Storage-Declaration), followed by `;`

### Call
An [identifier](#Identifier), followed by 0 or more [expressions](#Expression) delimited by `,`

### Storage Declaration
A [storage specifier](#Storage-Specifier), optionally followed by an [alignment specifier](#Alignment-Specifier), followed by an [identifier](#Identifier)

### Storage Specifier
`storage`, followed by a [specifier expression](#Specifier-Expression)

### Alignment Specifier
`alignment`, followed by a [specifier expression](#Specifier-Expression)

### Specifier Expression
An [expression](#Expression) surrounded by `<` and `>`

### Expression
A [number](#Number), [string](#String) or [identifier](#Identifier)

### Identifier
A sequence of non-whitespace characters excluding `{}<>;,"`

### Number
A sequence of digits

### String
A sequence of characters surrounded by `"`

## Cíonom Formation Semantics

### Program
The name of the file containing the program may be provided to the source by a conformant implementation under the [source location](#Source-Location) extension.

### Routine Declaration
The parameters of a routine follow the same lifetime semantics as storage within a block.

A routine declaration may be arbitrarily additionally qualified under a [routine qualification](#Routine-Qualification) extension.

### Block
A block may be arbitrarily additionally qualified under a [block qualification](#Block-Qualification) extension.

### Statement
A conformant implementation may allow statements to be arbitrarily qualified under a [statement qualification](#Statement-Qualification) extension.

### Call
Additionally the semantics of calls may be modified by additional qualification preceeding the routine identifier under a [statement qualification](#Statement-Qualification) extension. 

### Storage Declaration
The [Cíonom grammar specification](#Cíonom-Grammar-Specification) only allows for storage and alignment specifiers, with storage specifiers being mandatory. However a conformant implementation may provide any number of additional [nonstandard-specifiers](#Nonstandard-Specifiers). Additionally the [Cíonom grammar specification](#Cíonom-Grammar-Specification) states that a storage declaration is terminated by an identifier followed by `;`, however a conformant implementation is permitted to allow additional instruction beyond the identifer and before the `;` such as the provision of [initializer expressions](#Initializer-Expressions).

Storage is only valid within the routine it is declared - access outside of its enclosing routine after the routine's runtime has expired is undefined.

### Storage Specifier
The [Cíonom grammar specification](#Cíonom-Grammar-Specification) allows for alignment specifiers to specify any numeric value as storage, however this has practical limitations in the implementation. A conformant implementation may bound the valid values such as disallowing 0 or restricting storage to under 2kb.

The value provided to a storage specifier is taken to be unsigned.

### Alignment Specifier
The [Cíonom grammar specification](#Cíonom-Grammar-Specification) allows for alignment specifiers to specify any numeric value as alignment, however this has practical limitations in the implementation. A conformant implementation may limit the valid values for an alignment specifier however all powers of 2 from 0 to 3 must be available (i.e. 1, 2, 4 and 8). The specifics of how alignment functions is decided by the implementation.

The value provided to a alignment specifier is taken to be unsigned.

### Specifier Expression
The [Cíonom grammar specification](#Cíonom-Grammar-Specification) allows for identifiers in specifier expressions, to be expanded as numeric values (storage of specification `storage<8> alignment<8>`). However this will lead to runtime storage sizes (this may be avoided in some cases if the implementation provides provides [constant marking](#Constant-Marking) and [initializer expressions](#Initializer-Expressions)), this may be handled by the implementation either by allocating the value on the heap and automatically inserting a free at the end of the lifetime of the storage, or by creating a VLA (this may be controllable if the implementation provides a [storage strategy specifier](#Storage-Strategy)).

### Expression
When resolving to a value, a routine identifier will become a storage of specification `storage<8> alignment<8>` containing the address of the routine.

Routines are non-modifiable and the programmer should ensure that when passed to a routine call (or a [nonstandard-specifier](#Nonstandard-Specifiers) - Cíonom standard specifiers will never modify the specifier expression) that the storage will not be modified. This may be ensured by the implementation if it provides [constant marking](#Constant-Marking).

### Identifier
While the grammar allows for identifiers to contain any 
> sequence of non-whitespace characters excluding `{}<>;,"`

In practice the implementation may need to modify identifiers in order to export symbols or otherwise be conformant to an external specification. If this is the case, a conformant implementation should mangle the identifer by prefixing any disallowed characters with `__cionom_symbol_` followed by a suitable representation of the characters (possibly determined by an [identifier mangling routine qualification](#Identifier-Mangling)). For this reason a conformant implementation may reserve any identifier containing `__cionom`.

For example, given the identifier `s8+[s8]` but needing to comply to an alphanumeric symbol specification, the implementation may produce the symbol `s8__cionom_symbol_plus__cionom_symbol_lbrackets8__cionom_symbol_rbracket`.

Identifiers in core Cíonom must be unique per storage and routine however a conformant implementation may allow [shadowing](#Shadowing)).

### Number
When a number is placed in a program as an expression, it acts as the value of a storage of specification `storage<8> alignment<8>`, equivalent to an `Xint64_t` in C. Cíonom provides no arithmetic operations so signedness is determined by external code or by an extension such as [C arithmetic in specifier expressions](#C-Arithmetic-in-Specifier-Expressions).

Numbers are non-modifiable and the programmer should ensure that when passed to a routine call (or a [nonstandard-specifier](#Nonstandard-Specifiers) - Cíonom standard specifiers will never modify the specifier expression) that the storage will not be modified. This may be ensured by the implementation if it provides [constant marking](#Constant-Marking).

### String
When a string of length N characters is placed in a program as an expression, it acts as the value of a storage of specification `storage<N+1> alignment<1>`. The final byte of the storage will be 0 in order to be interoperable with C strings.

Strings are non-modifiable and the programmer should ensure that when passed to a routine call (or a [nonstandard-specifier](#Nonstandard-Specifiers) - Cíonom standard specifiers will never modify the specifier expression) that the storage will not be modified. This may be ensured by the implementation if it provides [constant marking](#Constant-Marking).

Cíonom strings do not support C escape sequences, however a conformant implementation may provide them an [extension](#String-Escape-Sequences).

## Cíonom Language Extensions

### C Preprocessor
Allows for the application of the C preprocessor to Cíonom source before being treated as Cíonom.

### String Escape Sequences
Allows strings to contain escape sequences similar to C string literals such as `\n` for newline or `\e` for escape. The codes provided by the extension are decided by the implementation.

### Nonstandard Specifiers
Allows specifiers in storage declarations other than `alignment`. These should be optional and may have semantics beyond those stated here. Additional specifiers may not take additional parameters and the implementation should instead opt for a specifier for every aspect of the desired behaviour (e.g. `foobar<a, b>` should instead be `foo<a> bar<b>`). These specifiers should be prefixed by `__cionom_extension_specifier_`.
#### Constant Marking
Allows for the marking of storage as constant with `__cionom_extension_specifier_constant`. This may be useful for implementations marking strings or numbers as constant to avoid inadvertant modification by functions, or to allow the programmer to mark a statement to not be modified if supplied alongside the [initializer expressions](#Initializer-Expressions) extension. Storage marked constant cannot be passed as a parameter to calls whose parameter declaration is not also marked constant. This may create issues with compatibility with non-extended code so an implementation may opt to also implement the [call discard constant](#Call-Discard-Constant) extension.
#### Storage Strategy
Allows the programmer to use `__cionom_extension_specifier_storage_strategy<"strategy">` to signal to the implementation which strategy should be used for allocation in a storage declaration. Suggested accepted values for strategy may be `heap`, `stack_constant` or `stack_variable`.
### Shadowing
Allows for the reuse of identifiers by storage declarations, with the evaluation of a shadowed identifier being determined by the last declaration using the identifier. To allow shadowing by a storage declaration the programmer may add `__cionom_extension_specifier_shadow`.

### Statement Qualification
Allows for arbitrary qualification of statements. These should be optional and may have semantics beyond those stated here. Statement qualification should take place before the start of a statement (i.e. preceeding the routine identifier in a call or before `return`). These qualifiers should be prefixed by `__cionom_extension_qualifier_statement_`. Statement qualification should not be used for storage declarations, instead the implementation should opt to use an additional specifier.
#### Call Discard Constant 
Allows for the discarding of constant specification in a call under the [constant marking](#Constant-Marking) extension. Allows the programmer to place `__cionom_extension_qualifier_statement_call_discard_constant` in a call statement to prevent the implementation from erroring around routines not marking parameters constant. This is to allow for implementations which implement constant marking to interoperate with code written for implementations which don't.

### Block Qualification
Allows for arbitrary qualification of blocks. These should be optional and may have semantics beyond those stated here. Block qualification should take place before the start of a block (i.e. preceeding the `{`). These qualifiers should be prefixed by `__cionom_extension_qualifier_block_`.

### Routine Qualification
Allows for arbitrary qualification of routines. These should be optional and may have semantics beyond those stated here. Routine qualification should take place before the start of a routine declaration (i.e. preceeding the identifier). These qualifiers should be prefixed by `__cionom_extension_qualifier_routine_`.
#### Identifier Mangling
Allows for the specification of the mangling of the identifier of a storage. This may be useful for interoperation with other languages or to provide nicer names in-language compared to external names. The programmer may use `__cionom_extension_qualifier_routine_mangling<"symbol_name">` to specify the name of the routine's symbol to export as a symbol for the implementation.

### C Arithmetic in Specifier Expressions
Allows for C arithmetic syntax in specifier expressions between constants (i.e. `storage<8 * 2>`). This may be provided to operate alongside a [C preprocessor](#C-Preprocessor) (so `storage<DEFINED_VALUE * 2>`) or to allow for more flexibility when specifying storage.

### Initializer Expressions
Allows for the initialization of a storage at declaration by placing `=` and an expression between the identifier and `;`. This will set the storage to the value of the expression. The implementation may decide how mismatches in storage and/or alignment specification are handled between the assigned expression and the declared storage.

### Comments
Allows for the creation of regions where the implementation will ignore all text by enclosing the text with `\``. This allows for the annotation of code by the programmer or the provision of inline documentation on interfaces.

### Intrinsic Branch
Allows for a `__cionom_extension_intrinsic_?` to be used as a more intuitive/native branching mechanism. A branch statement is formed as follows:
`__cionom_extension_intrinsic_?<condition_expression> evaluated_statement;` where the condition expression is taken as the value in its first byte (`storage<1>`), and if it does not equal 0 the evaluated statement will be executed. This may provide more flexibility than deferring to external code for branching.

### Source Location
Allows for implicit identifiers to be declared as indication of their location in the program's source. These would be `__cionom_extension_location_file` - specified to be a string (`storage<file_name_length+1> alignment<1>`), `__cionom_extension_location_line` - specified to be a number (`storage<8> alignment<8>`).
