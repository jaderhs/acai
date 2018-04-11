#include <stdlib.h>
#include <stdio.h>
#include "../parser/ast.h"
#include "ctx.h"
#include "../acai/type.h"

tree *llvm_type_new(llvm_ctx *ctx, tree *node) {

	switch(node->v.vt.vtype) {

		case AT_INT64:
			node->llvm_type = LLVMInt64TypeInContext(ctx->ctx);
			break;

		case AT_FLOAT32:
			node->llvm_type = LLVMFloatTypeInContext(ctx->ctx);
			break;

		default:
			printf("Unknown variable type %d\n", node->v.vt.vtype);
			return NULL;
	}

	return node;
}
