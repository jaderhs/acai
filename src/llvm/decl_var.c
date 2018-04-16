#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <llvm-c/Core.h>
#include "../util.h"
#include "../parser/ast.h"
#include "ctx.h"
#include "llvm.h"
#include "value.h"

void llvm_decl_var(llvm_ctx *ctx, tree *typed_identifier) {

	char *str = malloc(32 + strlen(AST_CHILD_RIGHT(typed_identifier)->v.s));
	sprintf(str, "identifier-%s", AST_CHILD_RIGHT(typed_identifier)->v.s);

	/* alloc acai_type{} */
	LLVMValueRef at = LLVMBuildAlloca(ctx->builder, llvm_value_type(), str);
	LLVMSetAlignment(at, 32);

	/* Set acai_type.type */
	LLVMValueRef type_num = LLVMConstInt(LLVMInt64Type(), AST_CHILD_LEFT(typed_identifier)->v.i, FALSE);

	LLVMValueRef gep = LLVMBuildStructGEP(ctx->builder, at, 0, "");
	LLVMBuildStore(ctx->builder, type_num, gep);

	typed_identifier->llvm_at = at;
	typed_identifier->llvm_value = LLVMBuildStructGEP(ctx->builder, at, 2, "");
}

LLVMValueRef llvm_decl_var_assigndeclare(llvm_ctx *ctx, tree *identifier, tree *node_literal) {

	char *str = malloc(32 + strlen(identifier->v.s));
	sprintf(str, "identifier-%s", identifier->v.s);

	return LLVMBuildAlloca(ctx->builder, llvm_value_type(), str);
}
