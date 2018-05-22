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
- $

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

A sequence of characters. A three-digit octal (\\*nnn*) and two-digit hexadecimal (\\x*nn*) escapes represent individual bytes. UTF-8 characters are escaped using a "\\u" prefix.
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
string z = "hello, world"
```

## Types

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

The zero value for a **string** is the empty string.

### Array

An array is a sequence of *N* elements of a single type.

The zero value for a **array** is a array filled with the zero value for it's type.

A character string may be represented by an **array** of the **byte** type.
```
int[99] x
byte[5] b = "test\0"
string[2] s = ["hello", "world!"]
```

### Object

An **object** is a sequence of variables and functions (fields). All types are represented internally as objects.

The zero value for a **object** has every field set as it's zero value.
```
object {
    int x
    string y
    func z()
}
```

#### Virtual Objects

A **virtual object** is a template of an object.

Any object that **implements** a virtual object **must** have the same non initialized methods and variables as the **virtual object**.
```
virtual object {
    int x
    func y() {return 99} /* y() has a default implementation */
    func z()
}
```

### Function

A **function** type represents functions with the same parameters and return.

The zero value for a **function** variable is **null**.
```
func()
func(int x) int
func(string a, ...) (int, float)
```

### List

A **list** is a sequence of elements of arbitrary types.

The zero value for a **list** variable is the a empty **list**.
```
list x
list y = [99, "hello", true]
```

### Dict

A **dict** is a group of elements of arbitrary types, each associated with a single key of a specified type.

The zero value for a **dict** variable is the a empty **dict**.
```
dict[string] x
dict[int] z = {1: 99, 2: "hello", 400: true}
```

### Set

A **set** represent a list of unique values.

The zero value for a **set** variable is the a empty **set**.
```
set x
set y = [1, 2, 3, 4, 3, 2, 1]
```

### Reference

A **reference** type is a reference to another type.

The zero value for a **reference** variable is **null**.
```
object *
int *
set *
```

## Declarations and Scope

### Constant declarations

A constant declaration bind a identifier list to a constant expression list.

```
const int MyConst1 = 99
const string MyConst2 = "Hello"
const {
    float MyConst3 = 2.0
    bool MyConst4 = false
}
const float x, string y, bool z = 3.14, "hello 123", true
```

### Custom types declaration and definition

Custom types are aliases to predefined types using the keyword **type**, e.g.:
```
type MyString = string     /* The '=' is optional */
type MySecondString string

type {
    MyNumeric int
    MyBoolean bool
}

type Color object {
    string Name
    float r, g, b
}
```

### Variable declaration

A variable represents an identifier associated with a type.

Non initialized variables are set to it's type zero value.

```
int x
float y, z
string hello = "world"
```

#### Variable type inference

Initialized variables may be declared without a type using the type inference assignment operator (*:=*), e.g.:
```
u := 99        /* same as int x = 99 */
v := uint(55)  /* same as uint x = 55 */
w := 55.44     /* same as float y = 55.44 */
x := "hello"   /* same as string z = "hello" */
y, z := false, {"test": 123}
```

### Function declaration

```
func hello() {
}

func world(int x) (float) { /* The parenthesis are optional if the result parameter is a single type */
}

func myfunc1(int w, string x) (int, bool z) { /* Result parameters may be named */
    return 0, false
}

func myfunc2(int v, string w="hello") (float, string("world"), list z=[1,2]) { /* Both result and calling rightmost parameters may have a default value */
    return 3.14
}

func myfunc3(int x, y, z, string a, b) { /* 'x','y' and 'z' are int. 'a' and 'b' are string */
}

func myfunc4(int x, ...) { /* '...' is a list of arguments */
    list myargs = ...
}

func myfunc5() {
    func subfunc1() {    /* Available inside myfunc5 as subfunc1 and outside as myfunc5.subfunc1() */
       print("hello from subfunc")
    }
}
```

### Method declarations

**Methods** may be declared within an **object** declaration block or outside using the *object_name.method* notation, e.g.:

```
object x {

    func method1(int y) int {
        return y + 42
    }

    func method2
}

func x.method2(float y) int {

    return y + 99
}
```

## Expressions

### Composite Literals

```
int[3] i1 = [1, 2, 3]    /* i1 is now array([1,2,3]) */
list l1 = [1, 2, 3]      /* l1 is now list([1,2,3]) */
set set1 = [1, 2, 3, 2, 1] /* set1 is now set([1,2,3]) */

dict[int] d1 = {1: "hello", 2: "ok"}

object o1 = {int y = 64; z := "hello"} /* generic object assignment */
```

### Function literal
A **function literal** represents an anonymous function.
```
f1 := func() int {return 42}

func sum = func(int x, y) int {return x + y}
```

## Statements

### If statement

**If statements** specify conditional execution branches based on a boolean condition.

```
if x > 99 {
    print("x larger than 99")
}
elseif x < 0 {
    print("x is negative")
}
else {
    print("x is neither larger than 99 nor negative")
}
```

A simple statement list may preceed the condition.

Variables declared in an **if** statement list are scoped inside the *if*/*elseif*/*else* blocks, e.g.:
```
if x := f(); int y = 42; x > y {
    print("x > y is True")
}
elseif x < y {
    print("x < y is True")
}
else {
    print("x is equal to y")
}
```

The **upscope operator** ($) allows a variable identifier declared inside an if statement list to be available after declaration to it's parent scope, e.g.:
```
if $x := f(); int $y = 42; x > y {
    print("x > y is True")
}

print("x value is ", x, " and y value is ", y) /* 'x' and 'y' are available here */
```

### Swith statements
```
switch x {
    case "hello":
        print("world")
        next /* fallthrough the next case */
    case "goodbye":
        print("my friend")
    case "ok":
        print("it sure is")
}
```

### Loops
Loops are created using a **for**, **do/while** or **while** statement.

A **for** loop has two forms.

#### Iteration statement for

Allows iteration of a object implementing the **iterable** **virtual object**. Both the **list** and **dict** types implement this template.
```
for item in mylist {
   print("Item from list is ", item)
}

for index, item in mylist {
    print("Item ", index, " from list is ", item)
}

for key, value in mydict {
    print("dict[", key, "]=", value)
}
```
#### Triple statement for

This statement takes at three statements after the **for** keyword.

The first statement is the *initialization statement*.

The middle statement is the *evaluation condition*. The loop will be executed while this condition is **true**.

The rightmost statement is the *post condition evaluation statement*.

Both the *initialization statement* and the *post condition evaluation statement* can be extended using a *statement list* inside a block.

```
for int i = 0; i < 10; i++ {
    print("The value of 'i' is ", i)
}

for {int i = 0; int j = 0; int k = 0} ; /* Multi statement initialization */
    i < 30 ;
    {i++; j++; k++} { /* Multi post condition statement */

        print("The sum of values is ", i + j + k)
    }
```

#### while

Execute the loop block until the **while** condition is **false**.
```
int i = 0
while i < 10 {
    print("The value of 'i' is ", i++)
}
```

#### do/while

Execute the loop block until the **while** condition is **false**.

```
float f = 0.0
do {
    f += 0.1
    print("The value of 'f' is ", f)
} while f < 1.0
```

### Defer

A **defer** is a block that's executed when an function scope ends.

Multiple **defer** blocks are executed in reverse order from declaration.

```
func myfunc(int x) int {

    object mylock
    lock(mylock)

    defer {
        unlock(mylock)
        print("Lock is now unlocked")
    }

    defer mydefer {
        print("A defer may have an identifier")
    }

    defer {
        string hello = "ok"
        print("This block will run before the previous block")
    }

    defer: print("For single expressions, you may use a colon")

    undefer /* Remove the last defer */
    undefer mydefer /* Remove a defer by identifier */
    undefer /* Remove the defer with the 'string hello' variable */

    if(x > 42)
        return 42 /* defer blocks will run here */

    do_something()

    return 99 /* defer blocks will run here */
}
```