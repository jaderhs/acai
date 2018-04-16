#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "eval.h"
#include "../parser/parser.h"
#include "../util.h"

tree *eval_func_decl(llvm_ctx *ctx, tree *node, unsigned int hint) {

	char *str;
	struct ast_list *list;
	LLVMBuilderRef prev_builder;

	LLVMTypeRef params[2];
	params[0] = LLVMInt32Type();
	params[1] = LLVMArrayType(LLVMPointerType(LLVMInt8Type(), 0), 0);

	LLVMAttributeRef attr = LLVMCreateEnumAttribute(ctx->ctx, LLVMGetEnumAttributeKindForName("alignstack", 10), 16);

	LLVMTypeRef func_spec = LLVMFunctionType(LLVMVoidType(), params, 2, FALSE);

	if(tree_list_length(node->v.func.identifier) > 1) {
		fprintf(stderr, "DECL_FUNC multi-identifier not implemented!\n");
		return NULL;
	}

	list = tree_list_get_first(node->v.func.identifier);

	str = malloc(32 + strlen(AST_LIST_NODE(list)->v.s));
	sprintf(str, "_acai_func_%s", AST_LIST_NODE(list)->v.s);

	node->llvm_value = LLVMAddFunction(ctx->module, str, func_spec);
	LLVMAddAttributeAtIndex(node->llvm_value, 0, attr);

	/* create new scope */

	prev_builder = ctx->builder;

	ctx->scope = llvm_scope_push_new(ctx->scope);
	ctx->builder = LLVMCreateBuilderInContext(ctx->ctx);

	LLVMBasicBlockRef bblock = LLVMAppendBasicBlockInContext(ctx->ctx, node->llvm_value, "entrypoint");
	LLVMPositionBuilderAtEnd(ctx->builder, bblock);

	eval(ctx, node->v.func.body, hint);

	LLVMBuildRetVoid(ctx->builder);
	/* restore previous scope */

	LLVMDisposeBuilder(ctx->builder);

	ctx->scope = llvm_scope_pop_free(ctx->scope);
	ctx->builder = prev_builder;

	return node;
}

tree *eval_func_return(llvm_ctx *ctx, tree *node, unsigned int hint) {

	tree *values = AST_CHILD_LEFT(node);

	if(values != NULL) {

	}

	return node;
}
