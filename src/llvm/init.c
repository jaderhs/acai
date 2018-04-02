#include <llvm-c/Target.h>
#include "ctx.h"

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
}

void llvm_finish(llvm_ctx *ctx) {

	LLVMDisposeBuilder(ctx->builder);
	LLVMDisposeModule(ctx->module);
}
