OBJECTS = init.o function_call.o type.o decl_var.o symbol.o value.o acai.o scope.o
DEBUG=-g -ggdb

all: $(OBJECTS)
	ar rcs llvm.a $(OBJECTS)

%.o: %.c
	clang $(DEBUG) -I../ -I/usr/include/llvm-5.0 -I/usr/include/llvm-c-5.0 -c -o $@  $<

clean:
	rm -f $(OBJECTS) llvm.a
