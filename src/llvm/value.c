#include <stdlib.h>
#include "ctx.h"
#include "value.h"
#include "../acai/type.h"
#include "../util.h"

LLVMTypeRef llvm_value_struct = NULL;

void llvm_value_init(llvm_ctx *ctx) {

	/* Build acai_value */

	LLVMTypeRef arg_body[2];
	arg_body[0] = LLVMInt32TypeInContext(ctx->ctx);
	arg_body[1] = LLVMArrayType(LLVMInt8TypeInContext(ctx->ctx), sizeof(((acai_value*)0)->v));

	llvm_value_struct = LLVMStructCreateNamed(ctx->ctx, "acai_value");
	LLVMStructSetBody(llvm_value_struct, arg_body, 2, FALSE);
}

LLVMTypeRef llvm_value_type(void) {
	return llvm_value_struct;
}

LLVMValueRef llvm_value_new_integer(llvm_ctx *ctx, int i) {

	LLVMValueRef values[2];
	values[0] = LLVMConstInt(LLVMInt32TypeInContext(ctx->ctx), AT_INTEGER, FALSE);
	values[1] = LLVMConstInt(LLVMInt64TypeInContext(ctx->ctx), i, FALSE);

	return LLVMConstNamedStruct(llvm_value_struct, values, 2);
}

void llvm_value_new_string() {

}
