#include <stdlib.h>
#include <string.h>
#include "ctx.h"
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
