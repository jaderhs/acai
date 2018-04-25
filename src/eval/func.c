#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "eval.h"
#include "../llvm/value.h"
#include "../parser/parser.h"
#include "../util.h"

tree *eval_func_decl(llvm_ctx *ctx, tree *node, unsigned int hint) {

	int i;
	char *str;
	tree *arg, *list_parameters, *identifier;
	struct ast_list *list;
	LLVMBuilderRef prev_builder;
	LLVMValueRef argv, argp, idx[1];

	LLVMTypeRef params[2];
	params[0] = LLVMInt64Type();
	params[1] = LLVMPointerType(LLVMPointerType(llvm_value_type(), 0), 0);

	LLVMAttributeRef attr = LLVMCreateEnumAttribute(ctx->ctx, LLVMGetEnumAttributeKindForName("alignstack", 10), 16);

	LLVMTypeRef func_spec = LLVMFunctionType(LLVMVoidType(), params, 2, FALSE);

	if(tree_list_length(node->v.func.identifier) > 1) {
		fprintf(stderr, "DECL_FUNC multi-identifier not implemented!\n");
		return NULL;
	}

	list = tree_list_get_first(node->v.func.identifier);

	str = malloc(32 + strlen(AST_LIST_NODE(list)->v.s));
	sprintf(str, "_acai_func_%s", AST_LIST_NODE(list)->v.s);

	node->v.func.llvm_func = LLVMAddFunction(ctx->module, str, func_spec);
	LLVMAddAttributeAtIndex(node->v.func.llvm_func, 0, attr);

	/* create new scope */
	prev_builder = ctx->builder;

	ctx->scope.local = llvm_scope_push_new(ctx->scope.local);
	ctx->builder = LLVMCreateBuilderInContext(ctx->ctx);

	LLVMBasicBlockRef bblock = LLVMAppendBasicBlockInContext(ctx->ctx, node->v.func.llvm_func, "entrypoint");
	LLVMPositionBuilderAtEnd(ctx->builder, bblock);

	/* Insert function parameters into scope */
	list_parameters = AST_CHILD_LEFT(((tree*) node->v.func.signature));

	argv = LLVMGetParam(node->v.func.llvm_func, 1);

	i = 0;
	AST_LIST_FOREACH(list_parameters, list) {

		arg = AST_LIST_NODE(list);

		if(arg->type == DECL_PARAM) {

			idx[0] = LLVMConstInt(LLVMInt64Type(), i, FALSE);
			argp = LLVMBuildGEP(ctx->builder, argv, idx, 1, "");
			argp = LLVMBuildLoad(ctx->builder, argp, "");

			identifier = tree_copy(AST_CHILD_LEFT(arg));

			identifier->av = llvm_acai_value_new();

			AST_ACAI_VALUE(identifier)->begin = argp;
			/* TODO: handle default value, typed identifier list */

			ctx->scope.local->identifiers = llvm_identifier_list_prepend(ctx->scope.local->identifiers, identifier);
			i++;
		}
		/* TODO: handle arg->type == TOK_VARARGS*/

	}

	/* Insert function into global scope */
	ctx->scope.global->identifiers = llvm_identifier_list_prepend(ctx->scope.global->identifiers, node);

	eval(ctx, node->v.func.body, hint);

	/* TODO: standardize a list of acai_value for return */

	LLVMBuildRetVoid(ctx->builder);
	/* restore previous scope */

	LLVMDisposeBuilder(ctx->builder);

	ctx->scope.local = llvm_scope_pop_free(ctx->scope.local);
	ctx->builder = prev_builder;

	return node;
}

tree *eval_func_return(llvm_ctx *ctx, tree *node, unsigned int hint) {

	tree *values = AST_CHILD_LEFT(node);

	if(values != NULL) {

	}

	return node;
}
