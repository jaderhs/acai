#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../module.h"
#include <llvm-c/Core.h>
#include "builtin.h"

void *builtin_print(int argc, acai_type *argv[]) {

	printf("builtin.print(%d)\n", argc);
	return NULL;
}
