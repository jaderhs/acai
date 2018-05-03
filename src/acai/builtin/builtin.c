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

int acai_builtin_print(int argc, acai_value *argv[], acai_value *ret) {

	if(argc == 0)
		return 0;

	if(argc > 1)
		printf("(");

	for(int i = 0; i < argc; i++) {

		if(i > 0)
			printf(", ");

		if((argv[i]->flags & AVF_NULL) != 0) {
			printf("null");
		}
		else {

			switch(argv[i]->type) {

				case AT_FLOAT:
				case AT_FLOAT32:
					printf("%f", (float)argv[i]->v.f32);
					break;

				case AT_INTEGER:
				case AT_INT64:
					printf("%lld", argv[i]->v.i64);
					break;

				case AT_STRING:
					printf("%s", argv[i]->v.s);
					break;
			}
		}

	}

	if(argc > 1)
		printf(")");

	printf("\n");
	return 0;
}
