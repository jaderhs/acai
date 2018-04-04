#include "../util.h"
#include "../llvm/ctx.h"

#define REGISTER_BUILTIN(name) builtin_register_func(ctx, name)

struct _builtin_func {
	char *name;
	void *func;
	struct _builtin_func *next;
};

void builtin_register_func(llvm_ctx *ctx, char *name);
