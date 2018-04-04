#include <stdlib.h>
#include <string.h>
#include "builtin_registry.h"
#include <llvm-c/Core.h>

void builtin_register(llvm_ctx *ctx) {

	REGISTER_BUILTIN("print");
}

struct _builtin_func *_builtin = NULL;

void builtin_register_func(char *name, acai_func func) {

	struct _builtin_func *b = malloc(sizeof(struct _builtin_func));

	b->name = strdup(name);
	b->func = func;

	b->next = _builtin;
	_builtin = b;
}

LLVMValueRef get_builtin(char *identifier) {

	struct _builtin_func *p;
	for(p = _builtin; p != NULL; p = p->next) {

		if(strcmp(p->name, identifier) == 0)
			return p->func;
	}

	return NULL;
}
