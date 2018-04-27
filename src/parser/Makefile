OBJECTS = parser.o ast.o str.o
DEBUG=-g -ggdb

all: lex.o lex_i.o $(OBJECTS)
	ar rcs parser.a lex.o $(OBJECTS)
	ar rcs parser_i.a lex_i.o $(OBJECTS)

%.o: %.c
	clang $(DEBUG) -I ../ -I/usr/include/llvm-5.0 -I/usr/include/llvm-c-5.0 -c -o $@  $<

lex.c: parser lex.l
	flex -o lex.c lex.l

lex_i.c: parser lex.l
	flex -o lex_i.c -I lex.l

parser: parser.y
	bison -d -o parser.c parser.y

clean:
	rm -f lex.c lex_i.c parser.c parser.h $(OBJECTS) lex.o lex_i.o parser.a parser_i.a
