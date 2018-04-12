#include <stdlib.h>
#include <stdio.h>
#include "../parser/ast.h"
#include "ctx.h"
#include "../parser/parser.h"
#include "../acai/type.h"

tree *llvm_type_new(llvm_ctx *ctx, tree *node) {

	switch(node->v.vt.vtype) {

		case AT_INT64:
			node->llvm_type = LLVMInt64TypeInContext(ctx->ctx);
			break;

		case AT_FLOAT32:
			node->llvm_type = LLVMFloatTypeInContext(ctx->ctx);
			break;

		case AT_STRING:
			node->llvm_type = LLVMInt8TypeInContext(ctx->ctx);
			break;

		default:
			printf("Unknown variable type %d\n", node->v.vt.vtype);
			return NULL;
	}

	return node;
}

int llvm_acai_type_infer(tree *node) {

	switch(node->type) {

		case LIT_INTEGER:
			return AT_INT64;
		case LIT_FLOAT:
			return AT_FLOAT32;
		case LIT_STRING:
			return AT_STRING;
	}

	return -1;

}