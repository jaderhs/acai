OBJECTS = parser.o lex.o ast.o eval.o
DEBUG=-g -ggdb

all: lexer $(OBJECTS)
	ar rcs parser.a $(OBJECTS)

%.o: %.c
	clang $(DEBUG) -I/usr/include/llvm-5.0 -I/usr/include/llvm-c-5.0 -c -o $@  $<

lexer: parser lex.l
	flex -o lex.c lex.l

parser: parser.y
	bison -d -o parser.c parser.y

clean:
	rm -f lex.c parser.c parser.h $(OBJECTS) parser.a
