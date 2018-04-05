#include <string.h>
#include "module.h"
#include "builtin/builtin.h"

void module_register(void) {

	//ACAI_MODULE_REGISTER("my-module", acai_module_mymodule);

	acai_module_register("builtin", NULL, acai_builtin_get_module(), TRUE);
}

struct acai_module_list {
	int global;
	char *prefix;
	acai_module *module;
	struct acai_module_list *next;
};

struct acai_module_list *modules = NULL;

void acai_module_register(char *name, char *prefix, acai_module *module, int global) {

	struct acai_module_list *list = malloc(sizeof(struct acai_module_list));

	list->global = global;
	list->prefix = prefix;
	list->module = module;
	list->next = modules;

	modules = list;
}

acai_func acai_module_search_func(char *module, char *name) {

	int i;
	struct acai_module_list *list;

	for(list = modules; list != NULL; list = list->next) {

		if(module == NULL && list->global == TRUE) {

			for(i = 0; list->module->funcs[i].name != NULL; i++) {

				if(strcmp(list->module->funcs[i].name, name) == 0) {

					return list->module->funcs[i].func;
				}
			}
		}
		//else prefixed module
	}

	return NULL;
}
