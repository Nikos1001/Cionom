# Cíonom Language Specification
Here lies the specification of the language, aimed to be a relatively human-oriented layout of the language and its features as would be implemented in a conformant implementation - including some extensions an implementation might implement.

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
A sequence of characters

### Number
A sequence of digits
