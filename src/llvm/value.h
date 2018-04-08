typedef struct {

	LLVMValueRef acai_type;

	LLVMTypeRef type;
	LLVMValueRef val;

} llvm_acai_value;


void llvm_value_init(llvm_ctx *ctx);
LLVMTypeRef llvm_value_type(void);

void llvm_value_new_integer(llvm_ctx *ctx, int i, llvm_acai_value *val);
void llvm_value_new_float(llvm_ctx *ctx, double f, llvm_acai_value *val);
void llvm_value_new_string(llvm_ctx *ctx, char *str, llvm_acai_value *val);
