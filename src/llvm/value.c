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
	arg_body[1] = LLVMArrayType(LLVMInt8TypeInContext(ctx->ctx), 8); //padding
	arg_body[2] = LLVMArrayType(LLVMInt8TypeInContext(ctx->ctx), sizeof(((acai_value*)0)->v));

	llvm_value_struct = LLVMStructCreateNamed(ctx->ctx, "struct.acai_value");
	LLVMStructSetBody(llvm_value_struct, arg_body, 3, FALSE);
}

LLVMTypeRef llvm_value_type(void) {
	return llvm_value_struct;
}

void llvm_value_literal_new(llvm_ctx *ctx, tree *node) {

	switch(node->type) {

		case LIT_INTEGER:

			node->llvm_type = LLVMInt64TypeInContext(ctx->ctx);
			node->llvm_value = LLVMConstInt(node->llvm_type, node->v.i, FALSE);
			break;

		case LIT_FLOAT:
			node->llvm_type = LLVMFloatTypeInContext(ctx->ctx);
			node->llvm_value = LLVMConstReal(node->llvm_type, node->v.f);
			break;

		default:
			fprintf(stderr, "Unknown literal type (%d) on llvm_value_literal_new()\n", node->type);
	}
}

LLVMValueRef llvm_value_zero_initializer(llvm_ctx *ctx, int type) {

	switch(type) {

		case AT_INT64:
		case AT_INTEGER:
			return LLVMConstInt(LLVMInt64TypeInContext(ctx->ctx), 0, FALSE);

		case AT_FLOAT32:
			return LLVMConstReal(LLVMFloatTypeInContext(ctx->ctx), 0.0);

	}

	return NULL;
}

void llvm_value_new_integer(llvm_ctx *ctx, int i, llvm_acai_value *val) {

	val->acai_type = LLVMConstInt(LLVMInt64TypeInContext(ctx->ctx), AT_INTEGER, FALSE);

	val->type = LLVMInt64TypeInContext(ctx->ctx);
	val->val = LLVMConstInt(val->type, i, FALSE);

//	LLVMValueRef *values = calloc(sizeof(LLVMValueRef), 2);
	//return LLVMConstNamedStruct(llvm_value_struct, values, 2);
}

void llvm_value_new_float(llvm_ctx *ctx, double f, llvm_acai_value *val) {

	val->acai_type = LLVMConstInt(LLVMInt64TypeInContext(ctx->ctx), AT_FLOAT, FALSE);

	val->type = LLVMFloatTypeInContext(ctx->ctx);
	val->val = LLVMConstReal(val->type, f);
}

void llvm_value_new_string(llvm_ctx *ctx, char *str, llvm_acai_value *val) {

	val->acai_type = LLVMConstInt(LLVMInt64TypeInContext(ctx->ctx), AT_STRING, FALSE);

	val->type = LLVMPointerType(LLVMInt8TypeInContext(ctx->ctx), 0);
	val->val = LLVMConstStringInContext(ctx->ctx, str, strlen(str), FALSE);//LLVMConstPointerNull(LLVMInt8TypeInContext(ctx->ctx));
}

void llvm_value_new_identifier(llvm_ctx *ctx, tree *identifier, llvm_acai_value *val) {

	if(identifier->type != TYPED_IDENTIFIER) {
		printf("Unknown value new identifier type %d\n", identifier->type);
		return;
	}

	val->acai_type = LLVMConstInt(LLVMInt64TypeInContext(ctx->ctx), AST_CHILD_LEFT(identifier)->v.vt.vtype, FALSE);

	val->type = AST_CHILD_LEFT(identifier)->llvm_type;
	val->val = LLVMBuildLoad(ctx->builder, identifier->llvm_value, "");

}
