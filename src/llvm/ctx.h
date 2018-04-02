#include <llvm-c/Core.h>

typedef struct {

	LLVMContextRef ctx;
	LLVMModuleRef module;
	LLVMBuilderRef builder;

} llvm_ctx;
