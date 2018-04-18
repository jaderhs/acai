#define AST_ACAI_VALUE(node)	((llvm_acai_value*)node->av)
#define AST_VALUE_LITERAL(node)	((llvm_value_literal*)node->lvl)

#define IS_LITERAL(n) (n->type == LIT_INTEGER || n->type == LIT_FLOAT)

typedef struct {

	LLVMValueRef begin;
	LLVMValueRef type;
	LLVMValueRef flags;
	LLVMValueRef value;

	LLVMTypeRef llvm_type;

} llvm_acai_value;

typedef struct {

	int acai_type;

	LLVMTypeRef type;
	LLVMValueRef value;

} llvm_value_literal;

void llvm_value_init(llvm_ctx *ctx);
LLVMTypeRef llvm_value_type(void);

llvm_acai_value *llvm_acai_value_alloca(llvm_ctx *ctx, char *llvm_identifier);
llvm_acai_value *llvm_acai_value_alloca_with_type(llvm_ctx *ctx, char *llvm_identifier, int av_type);

llvm_value_literal *llvm_value_literal_new(llvm_ctx *ctx, tree *node);
llvm_value_literal *llvm_value_zero_initializer(llvm_ctx *ctx, int av_type);
