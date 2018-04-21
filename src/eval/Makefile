OBJECTS = eval.o op_binary.o func.o
DEBUG=-g -ggdb

all: $(OBJECTS)
	ar rcs eval.a $(OBJECTS)

%.o: %.c
	clang $(DEBUG) -I ../ -I/usr/include/llvm-5.0 -I/usr/include/llvm-c-5.0 -c -o $@  $<

clean:
	rm -f $(OBJECTS) eval.a
