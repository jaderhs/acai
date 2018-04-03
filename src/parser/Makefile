parser: lexer parser.y
	bison -o parser.c parser.y

lexer: lex.l
	flex -o lex.c lex.l

clean:
	rm -f lex.c parser.c parser.h
