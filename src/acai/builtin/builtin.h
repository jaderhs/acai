#include <stdlib.h>

void *builtin_print(int argc, acai_type *argv[]);

acai_module_func builtin_funcs[] = {
	{"print", builtin_print, NULL},
	{NULL, NULL, NULL}
};

acai_module acai_module_builtin = {
	"builtin",
	~0,
	builtin_funcs
};
