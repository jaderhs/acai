typedef struct {

	LLVMValueRef acai_type;

	LLVMTypeRef type;
	LLVMValueRef val;

} llvm_acai_value;


void llvm_value_init(llvm_ctx *ctx);
LLVMTypeRef llvm_value_type(void);

void llvm_value_literal_new(llvm_ctx *ctx, tree *node);
LLVMValueRef llvm_value_zero_initializer(llvm_ctx *ctx, int type);

void llvm_value_new_integer(llvm_ctx *ctx, int i, llvm_acai_value *val);
void llvm_value_new_float(llvm_ctx *ctx, double f, llvm_acai_value *val);
void llvm_value_new_string(llvm_ctx *ctx, char *str, llvm_acai_value *val);
void llvm_value_new_identifier(llvm_ctx *ctx, tree *identifier, llvm_acai_value *val);
