#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ctx.h"
#include "parser/parser.h"
#include "value.h"
#include "acai/type.h"
#include "util.h"

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

llvm_acai_value *llvm_acai_value_new() {

	return malloc(sizeof(llvm_acai_value));
}

llvm_acai_value *llvm_acai_value_new_alloca(llvm_ctx *ctx, char *llvm_identifier) {

	LLVMValueRef llvm_flags;
	llvm_acai_value *av = llvm_acai_value_new();

	/* alloc acai_type{} */
	av->begin = LLVMBuildAlloca(ctx->builder, llvm_value_type(), llvm_identifier);
	LLVMSetAlignment(av->begin, 32);

	return av;
}

llvm_acai_value *llvm_acai_value_initialize(llvm_ctx *ctx, int av_type, llvm_acai_value *av) {

	LLVMValueRef vptr;

	switch(av_type) {

		case AT_INT64:
		case AT_INTEGER:
			vptr = LLVMBuildBitCast(ctx->builder, llvm_acai_value_get_value(ctx, av), LLVMInt64TypeInContext(ctx->ctx), "");
			LLVMBuildStore(ctx->builder, LLVMConstInt(LLVMInt64TypeInContext(ctx->ctx), 0, FALSE), vptr);
			break;

		case AT_INT32:
			vptr = LLVMBuildBitCast(ctx->builder, llvm_acai_value_get_value(ctx, av), LLVMInt32TypeInContext(ctx->ctx), "");
			LLVMBuildStore(ctx->builder, LLVMConstInt(LLVMInt32TypeInContext(ctx->ctx), 0, FALSE), vptr);
			break;

		case AT_INT16:
			vptr = LLVMBuildBitCast(ctx->builder, llvm_acai_value_get_value(ctx, av), LLVMInt16TypeInContext(ctx->ctx), "");
			LLVMBuildStore(ctx->builder, LLVMConstInt(LLVMInt16TypeInContext(ctx->ctx), 0, FALSE), vptr);
			break;

		case AT_INT8:
			vptr = LLVMBuildBitCast(ctx->builder, llvm_acai_value_get_value(ctx, av), LLVMInt8TypeInContext(ctx->ctx), "");
			LLVMBuildStore(ctx->builder, LLVMConstInt(LLVMInt8TypeInContext(ctx->ctx), 0, FALSE), vptr);
			break;

		case AT_STRING:
			vptr = llvm_acai_value_get_flags(ctx, av);
			LLVMBuildStore(ctx->builder, LLVMConstInt(LLVMInt64TypeInContext(ctx->ctx), AVF_NULL, FALSE), vptr);
			break;

		default:
			fprintf(stderr, "Unknown acai type (%d) on llvm_acai_value_initialize()\n", av_type);
			return NULL;
	}

	if(av_type != AT_STRING) {
		//zero flags
		vptr = llvm_acai_value_get_flags(ctx, av);
		LLVMBuildStore(ctx->builder, LLVMConstInt(LLVMInt64TypeInContext(ctx->ctx), 0, FALSE), vptr);
	}

	return av;
}

llvm_acai_value *llvm_acai_value_new_alloca_with_type(llvm_ctx *ctx, char *llvm_identifier, int av_type, int initialize) {

	llvm_acai_value *av = llvm_acai_value_new_alloca(ctx, llvm_identifier);

	/* Set acai_type.type */
	LLVMBuildStore(ctx->builder, LLVMConstInt(LLVMInt64TypeInContext(ctx->ctx), av_type, FALSE), llvm_acai_value_get_type(ctx, av));

	if(initialize != FALSE) {
		llvm_acai_value_initialize(ctx, av_type, av);
	}

	return av;
}

LLVMValueRef llvm_acai_value_get_type(llvm_ctx *ctx, llvm_acai_value *lav) {

	if(lav->type == NULL)
		lav->type = LLVMBuildStructGEP(ctx->builder, lav->begin, 0, "");

	return lav->type;
}

LLVMValueRef llvm_acai_value_get_flags(llvm_ctx *ctx, llvm_acai_value *lav) {

	if(lav->flags == NULL)
		lav->flags = LLVMBuildStructGEP(ctx->builder, lav->begin, 1, "");

	return lav->flags;
}

LLVMValueRef llvm_acai_value_get_value(llvm_ctx *ctx, llvm_acai_value *lav) {

	if(lav->value == NULL)
		lav->value = LLVMBuildStructGEP(ctx->builder, lav->begin, 2, "");

	return lav->value;
}

void llvm_acai_value_copy(llvm_ctx *ctx, llvm_acai_value *src, llvm_acai_value *dst) {

	LLVMValueRef val = LLVMBuildLoad(ctx->builder, src->type, "");
	LLVMBuildStore(ctx->builder, val, dst->type);

	val = LLVMBuildLoad(ctx->builder, src->flags, "");
	LLVMBuildStore(ctx->builder, val, dst->flags);

	val = LLVMBuildLoad(ctx->builder, src->value, "");
	LLVMBuildStore(ctx->builder, val, dst->value);

	/* TODO: handle strings (are strings cow)? */
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

