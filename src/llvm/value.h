#define AST_ACAI_VALUE(node)	((llvm_acai_value*)node->av)
#define AST_VALUE_LITERAL(node)	((llvm_value_literal*)node->lvl)

#define IS_LITERAL(n) (n->type == LIT_INTEGER || n->type == LIT_FLOAT || n->type == LIT_STRING)

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

llvm_acai_value *llvm_acai_value_new();
llvm_acai_value *llvm_acai_value_new_alloca(llvm_ctx *ctx, char *llvm_identifier);
llvm_acai_value *llvm_acai_value_new_alloca_with_type(llvm_ctx *ctx, char *llvm_identifier, int av_type, int initialize);

llvm_acai_value *llvm_acai_value_initialize(llvm_ctx *ctx, int av_type, llvm_acai_value *av);

LLVMValueRef llvm_acai_value_get_type(llvm_ctx *ctx, llvm_acai_value *lav);
LLVMValueRef llvm_acai_value_get_flags(llvm_ctx *ctx, llvm_acai_value *lav);
LLVMValueRef llvm_acai_value_get_value(llvm_ctx *ctx, llvm_acai_value *lav);

void llvm_acai_value_copy(llvm_ctx *ctx, llvm_acai_value *src, llvm_acai_value *dst);

llvm_value_literal *llvm_value_literal_new(llvm_ctx *ctx, tree *node);
llvm_value_literal *llvm_value_zero_initializer(llvm_ctx *ctx, int av_type);
