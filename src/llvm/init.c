#include <llvm-c/Target.h>
#include "ctx.h"
#include "acai.h"
#include "value.h"
#include "../util.h"

void llvm_create_module(char *name, llvm_ctx *ctx) {

	ctx->module = LLVMModuleCreateWithNameInContext(name, ctx->ctx);
	ctx->builder = LLVMCreateBuilderInContext(ctx->ctx);
}

void llvm_init(llvm_ctx *ctx) {

	LLVMInitializeAllTargetInfos();
	LLVMInitializeAllTargets();
	LLVMInitializeAllTargetMCs();
	LLVMInitializeAllAsmParsers();
	LLVMInitializeAllAsmPrinters();

	ctx->ctx = LLVMGetGlobalContext();

	llvm_value_init(ctx);
}

void llvm_create_main(llvm_ctx *ctx) {

	LLVMTypeRef params[2];
	params[0] = LLVMInt32Type();
	params[1] = LLVMArrayType(LLVMPointerType(LLVMInt8Type(), 0), 0);

	LLVMTypeRef func_spec = LLVMFunctionType(LLVMVoidType(), params, 2, FALSE);

	LLVMValueRef func = LLVMAddFunction(ctx->module, "_acai_init", func_spec);

	LLVMBasicBlockRef bblock = LLVMAppendBasicBlockInContext(ctx->ctx, func, "entrypoint");
	LLVMPositionBuilderAtEnd(ctx->builder, bblock);
}

void llvm_finish(llvm_ctx *ctx) {

	LLVMDisposeBuilder(ctx->builder);
	LLVMDisposeModule(ctx->module);
}
