OBJECTS = builtin.o
DEBUG=-g -ggdb

all: $(OBJECTS)
	ar rcs ../builtin.a $(OBJECTS)

%.o: %.c
	clang $(DEBUG) -I/usr/include/llvm-5.0 -I/usr/include/llvm-c-5.0 -c -o $@  $<

clean:
	rm -f $(OBJECTS)
