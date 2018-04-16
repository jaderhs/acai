CC=clang
DEBUG=-g -ggdb

compiler: main.c parser/parser.a llvm/llvm.a acai/acai.a eval/eval.a
	$(CC) -DCOMPILER -o compiler main.c parser/parser.a eval/eval.a llvm/llvm.a -I/usr/include/llvm-5.0 -I/usr/include/llvm-c-5.0 -L/usr/lib/llvm-5.0/lib -lLLVM $(DEBUG)

repl: main.c parser/parser.a llvm/llvm.a eval/eval.a
	$(CC) -o repl main.c parser/parser.a eval/eval.a llvm/llvm.a -I/usr/include/llvm-5.0 -I/usr/include/llvm-c-5.0 -L/usr/lib/llvm-5.0/lib -lLLVM $(DEBUG)

parser/parser.a:
	$(MAKE) -C parser

eval/eval.a:
	$(MAKE) -C eval

llvm/llvm.a:
	$(MAKE) -C llvm

acai/acai.a:
	$(MAKE) -C acai

clean:
	rm -f compiler repl a.out output.o
	$(MAKE) -C llvm clean
	$(MAKE) -C parser clean
	$(MAKE) -C eval clean
	$(MAKE) -C acai clean
