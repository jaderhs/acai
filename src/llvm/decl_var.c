#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <llvm-c/Core.h>
#include "../parser/ast.h"
#include "ctx.h"

LLVMValueRef llvm_decl_var(llvm_ctx *ctx, tree *typed_identifier) {

	char *str = malloc(32 + strlen(AST_CHILD_RIGHT(typed_identifier)->v.s));
	sprintf(str, "identifier-%s", AST_CHILD_RIGHT(typed_identifier)->v.s);

	return LLVMBuildAlloca(ctx->builder, AST_CHILD_LEFT(typed_identifier)->llvm_type, str);
}

LLVMValueRef llvm_decl_var_assigndeclare(llvm_ctx *ctx, tree *identifier, tree *node_literal) {

	char *str = malloc(32 + strlen(identifier->v.s));
	sprintf(str, "identifier-%s", identifier->v.s);

	return LLVMBuildAlloca(ctx->builder, node_literal->llvm_type, str);
}
