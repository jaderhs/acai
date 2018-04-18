#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <llvm-c/Core.h>
#include "util.h"
#include "parser/ast.h"
#include "ctx.h"
#include "llvm.h"
#include "value.h"

void llvm_decl_var(llvm_ctx *ctx, tree *typed_identifier) {

	char *str = malloc(32 + strlen(AST_CHILD_RIGHT(typed_identifier)->v.s));
	sprintf(str, "identifier-%s", AST_CHILD_RIGHT(typed_identifier)->v.s);

	typed_identifier->av = llvm_acai_value_alloca_with_type(ctx, str, AST_CHILD_LEFT(typed_identifier)->v.i);
}

void llvm_decl_var_assigndeclare(llvm_ctx *ctx, tree *identifier, tree *node_literal) {

	char *str = malloc(32 + strlen(identifier->v.s));
	sprintf(str, "identifier-%s", identifier->v.s);

	node_literal->av = llvm_acai_value_alloca(ctx, str);
}
