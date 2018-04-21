#include "type.h"
#include "util.h"

#define ACAI_MODULE_REGISTER(name, module) acai_module_register(name, name, &module, FALSE)

typedef struct {
	char *name;
	acai_func func;

	void *userdata;
} acai_module_func;

typedef struct {

	char *name;
	int global;

	acai_module_func *funcs; //last value must be all null'ed
} acai_module;

void module_register(void);
void acai_module_register(char *name, char *prefix, acai_module *module, int global);
acai_func acai_module_search_func(char *module, char *name);
