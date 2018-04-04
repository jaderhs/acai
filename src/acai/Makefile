OBJECTS = acai.o module.o
MODULES=builtin.a
DEBUG=-g -ggdb

all: $(OBJECTS) $(MODULES)
	ar rcs acai.a $(MODULES) $(OBJECTS)

%.o: %.c
	clang $(DEBUG) -I/usr/include/llvm-5.0 -I/usr/include/llvm-c-5.0 -c -o $@  $<

builtin.a:
	$(MAKE) -C builtin

clean:
	rm -f $(OBJECTS) $(MODULES) acai.a
	$(MAKE) -C builtin clean
