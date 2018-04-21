#include <llvm-c/Core.h>
#include "scope.h"

#ifndef _LLVM_CTX_H_
#define _LLVM_CTX_H_

typedef struct {

	LLVMContextRef ctx;
	LLVMModuleRef module;
	LLVMBuilderRef builder;

	struct {
		llvm_scope *global;
		llvm_scope *local;
	} scope;

} llvm_ctx;

#define llvm_ctx_get_current_scope(ctx) ctx->scope.local != NULL ? ctx->scope.local : ctx->scope.global

#endif
