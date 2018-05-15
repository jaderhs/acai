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

#### Floating point

Decimal representation of a floating-point constant, e.g.:
```
.55
0.
22.44
05.75
3.141592
1.e+3
1E3
12.34e-5
```

#### String

A sequence of characters. A three-digit octal (\*nnn*) and two-digit hexadecimal (\x*nn*) escapes represent individual bytes. UTF-8 characters are escaped using a "\u" prefix.
```
"abc"
"Hello world\n"
"日本語"
"\uE425"
"\xFF"
"\061\062\063"
```

## Constants

### Predefined

- true
- false

## Variables

Variables are statically typed. A unitialized variable is set to a zero value defined by it's type.
```
int x
int y = 99
z := "hello, world"
```

## Types

### Custom

Custom types are aliases to predefined types using the keyword **typedef**, e.g.:
```
typedef string MyString

typedef {
	int MyNumeric
	bool MyBoolean
}
```

### bool

The **bool** type has a value of *true* or *false*.
The zero value for **bool** is *false*.

### Numeric types

- byte (same as uchar)
- char / uchar (same as int8/uint8)

- int/uint (same as int64/uint64)
- int8/uint8
- int16/uint16
- int32/uint32
- int64/uint64
- int128/uint128

- float (same as float32)
- float32
- float64
- float80

### String

The **string** type represents an UTF-8 string.
Character arrays should use the **byte** type.

### Array

An array is a sequence of *N* elements of a single type.
```
int[99] x
byte[5] b = "test\0"
string[2] s = ["hello", "world!"]
``
