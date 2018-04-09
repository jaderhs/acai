#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../module.h"
#include <llvm-c/Core.h>
#include "builtin.h"

acai_module_func builtin_funcs[] = {
	{"print", acai_builtin_print, NULL},
	{NULL, NULL, NULL}
};

acai_module acai_module_builtin = {
	"builtin",
	~0,
	builtin_funcs
};

acai_module *acai_builtin_get_module(void) {
	return &acai_module_builtin;
}

void *acai_builtin_print(int argc, acai_value *argv[]) {

	if(argc == 0)
		return NULL;

	if(argc > 1)
		printf("(");

	for(int i = 0; i < argc; i++) {

		if(i > 0)
			printf(", ");

		switch(argv[i]->type) {

			case AT_FLOAT:
			case AT_FLOAT32:
				printf("%f", argv[i]->v.f32);
				break;

			case AT_INTEGER:
				printf("%d", argv[i]->v.i);
				break;

			case AT_STRING:
				printf("%s", argv[i]->v.s);
				break;
		}

	}

	if(argc > 1)
		printf(")");

	printf("\n");
	return NULL;
}
