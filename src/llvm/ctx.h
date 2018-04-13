#include <llvm-c/Core.h>
#include "identifier.h"

#ifndef _LLVM_CTX_H_
#define _LLVM_CTX_H_

typedef struct {

	LLVMContextRef ctx;
	LLVMModuleRef module;
	LLVMBuilderRef builder;

	llvm_scope_identifier_list *scope;
	llvm_identifier_list *global;

} llvm_ctx;

#endif
