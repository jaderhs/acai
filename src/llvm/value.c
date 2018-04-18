#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../parser/ast.h"
#include "ctx.h"
#include "../parser/parser.h"
#include "value.h"
#include "../acai/type.h"
#include "../util.h"

LLVMTypeRef llvm_value_struct = NULL;

void llvm_value_init(llvm_ctx *ctx) {

	/* Build acai_value */
	LLVMTypeRef arg_body[3];
	arg_body[0] = LLVMInt64TypeInContext(ctx->ctx);
	arg_body[1] = LLVMInt64TypeInContext(ctx->ctx);
	arg_body[2] = LLVMArrayType(LLVMInt8TypeInContext(ctx->ctx), sizeof(((acai_value*)0)->v));

	llvm_value_struct = LLVMStructCreateNamed(ctx->ctx, "struct.acai_value");
	LLVMStructSetBody(llvm_value_struct, arg_body, 3, FALSE);
}

LLVMTypeRef llvm_value_type(void) {
	return llvm_value_struct;
}

llvm_acai_value *llvm_acai_value_alloca(llvm_ctx *ctx, char *llvm_identifier) {

	llvm_acai_value *av = malloc(sizeof(llvm_acai_value));

	/* alloc acai_type{} */
	av->begin = LLVMBuildAlloca(ctx->builder, llvm_value_type(), llvm_identifier);
	LLVMSetAlignment(av->begin, 32);

	av->type = LLVMBuildStructGEP(ctx->builder, av->begin, 0, "");

	/* Store pointer to flags and value */
	av->flags = LLVMBuildStructGEP(ctx->builder, av->begin, 1, "");
	av->value = LLVMBuildStructGEP(ctx->builder, av->begin, 2, "");

	return av;
}

llvm_acai_value *llvm_acai_value_alloca_with_type(llvm_ctx *ctx, char *llvm_identifier, int av_type) {

	llvm_acai_value *av = llvm_acai_value_alloca(ctx, llvm_identifier);

	/* Set acai_type.type */
	LLVMBuildStore(ctx->builder, LLVMConstInt(LLVMInt64TypeInContext(ctx->ctx), av_type, FALSE), av->type);
	return av;
}

llvm_value_literal *llvm_value_literal_new(llvm_ctx *ctx, tree *node) {

	int i;
	LLVMValueRef val[8];

	llvm_value_literal *lvl = malloc(sizeof(llvm_value_literal));

	switch(node->type) {

		case LIT_INTEGER:

			lvl->type = LLVMInt64TypeInContext(ctx->ctx);
			lvl->value = LLVMConstInt(lvl->type, node->v.i, FALSE);

			lvl->acai_type = AT_INTEGER;
			break;

		case LIT_FLOAT:
			lvl->type = LLVMFloatTypeInContext(ctx->ctx);
			lvl->value = LLVMConstReal(lvl->type, node->v.f);

			lvl->acai_type = AT_FLOAT;
			break;

		case LIT_STRING:
			lvl->type = LLVMArrayType(LLVMInt8TypeInContext(ctx->ctx), strlen(node->v.s) + 1);
			lvl->value = LLVMConstStringInContext(ctx->ctx, node->v.s, strlen(node->v.s), FALSE);

			lvl->acai_type = AT_STRING;
			break;

		case LIT_NULL:

			for(i = 0; i < 8; i++)
				val[i] = LLVMConstInt(LLVMInt8TypeInContext(ctx->ctx), 0, FALSE);

			lvl->type = LLVMArrayType(LLVMInt8TypeInContext(ctx->ctx), 0);
			lvl->value = LLVMConstArray(LLVMInt8TypeInContext(ctx->ctx), val, 8);

			lvl->acai_type = AT_STRING;
			break;

		default:
			fprintf(stderr, "Unknown literal type (%d) on llvm_value_literal_new()\n", node->type);
			return NULL;
	}

	return lvl;
}

llvm_value_literal *llvm_value_zero_initializer(llvm_ctx *ctx, int av_type) {

	llvm_value_literal *lvl = malloc(sizeof(llvm_value_literal));

	switch(av_type) {

		case AT_INT64:
		case AT_INTEGER:
			lvl->type = LLVMInt64TypeInContext(ctx->ctx);
			lvl->value = LLVMConstInt(lvl->type, 0, FALSE);
			break;

		case AT_FLOAT:
		case AT_FLOAT32:
			lvl->type = LLVMFloatTypeInContext(ctx->ctx);
			lvl->value = LLVMConstReal(lvl->type, 0.0);
			break;

		case AT_STRING:
			lvl->type = LLVMInt8TypeInContext(ctx->ctx);
			lvl->value = LLVMConstInt(lvl->type, 0, FALSE);
			break;

		default:
			fprintf(stderr, "Unknown literal type (%d) on llvm_value_zero_initializer()\n", av_type);
			return NULL;
	}

	return lvl;
}
