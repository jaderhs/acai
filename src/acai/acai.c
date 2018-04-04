#include <stdio.h>
#include "type.h"

void acai_func_call(char *name, int argc, acai_type *argv[]) {
	printf ("acai_func_call(\"%s\", %d, %p)\n", name, argc, argv);
}
