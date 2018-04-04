#include <stdlib.h>
#include <stdio.h>
#include "module.h"

extern int _init(int, char **, char **);

int main(int argc, char *argv[], char *env[]) {

	module_register();

	return _init(argc, argv, env);
}

void _start(int argc, char **argv, char **env) {
	exit(main(argc, argv, env));
}

void *acai_func_call(char *name, int argc, acai_type *argv[]) {

	//split on '.'

	acai_func func = acai_module_search_func(NULL, name);

	if(func)
		return func(argc, argv);

	printf("UNKNOWN acai_func_call(\"%s\", %d, %p)\n", name, argc, argv);

	return NULL;
}
