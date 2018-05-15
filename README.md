# Açai
The Açai Programming Language

## Lexical Elements

### Comments

Single line start with // and stop at the end of the line.

Multi-line comments start with /\* and stop at \*/.

### Semicolons

Statements may optionally end with semicolons

### Identifiers

Identifiers start with underline (\_) or a letter, followed by alphanumeric characters.

### Keywords

- as
- break
- case
- const
- continue
- else
- elseif
- export
- for
- func
- global
- if
- import
- local
- return
- sizeof
- switch
- typedef
- typeof
- enum

### Operators

- \+ - \* / %
- & | ^ << >>
- += -= \*= /= %=
- &= |= ^= <<= >>=
- ++ --
- and or not
- \> < = :=
- \>= <= == !=
- () [] {} , ; .

### Literals

#### Integers

A sequence of digits, optionally prefixed with "0" for octal and "0x" for hexadecimal, e.g.:
```
1234
0xDEADBEEF
0777
```
