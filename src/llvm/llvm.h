
void llvm_function_call(llvm_ctx *ctx, tree *call);
tree *llvm_type_new(llvm_ctx *ctx, tree *node);
int llvm_acai_type_infer(tree *node);
LLVMValueRef llvm_decl_var(llvm_ctx *ctx, tree *typed_identifier);
LLVMValueRef llvm_decl_var_assigndeclare(llvm_ctx *ctx, tree *identifier, tree *node_literal);
