#include <stdlib.h>
#include <stdio.h>
#include "module.h"

extern int _acai_init(int, char **, char **);

int main(int argc, char *argv[], char *env[]) {

	module_register();

	return _acai_init(argc, argv, env);
}

void _start(int argc, char **argv, char **env) {
	exit(main(argc, argv, env));
}

int acai_func_call(char *name, int argc, acai_value *argv[], acai_value *ret) {

	//split on '.'

	acai_func func = acai_module_search_func(NULL, name);

	if(func)
		return func(argc, argv, ret);

	printf("UNKNOWN acai_func_call(\"%s\", %d, %p)\n", name, argc, argv);

	return 0;
}
