CC=clang

compiler:
	$(CC) -o compiler main.c llvm/init.c -I/usr/include/llvm-5.0 -I/usr/include/llvm-c-5.0 -L/usr/lib/llvm-5.0/lib -lLLVM

repl:
	$(CC) -o repl main.c llvm/init.c -I/usr/include/llvm-5.0 -I/usr/include/llvm-c-5.0 -L/usr/lib/llvm-5.0/lib -lLLVM

clean:
	rm -f compiler repl
