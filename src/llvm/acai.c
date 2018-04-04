#include "ctx.h"
#include "value.h"
#include "../util.h"
#include <llvm-c/Core.h>

LLVMValueRef _acai_func_call;

void acai_init(llvm_ctx *ctx) {

	/* register function call lookup */

	LLVMTypeRef params[3];
	params[0] = LLVMPointerType(LLVMInt8Type(), 0); //name
	params[1] = LLVMInt64Type();
	params[2] = LLVMArrayType(LLVMPointerType(llvm_value_type(), 0), 0);

	LLVMTypeRef func_spec = LLVMFunctionType(LLVMPointerType(LLVMInt8Type(), 0), params, 3, FALSE);

	_acai_func_call = LLVMAddFunction(ctx->module, "acai_func_call", func_spec);
}

LLVMValueRef acai_get_func_call(void) {
	return _acai_func_call;
}
