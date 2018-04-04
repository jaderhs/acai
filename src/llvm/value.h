/* Prototype
struct acai_value {
	int type;
	union {
		int i;
		float f;
	
	} v;
}
*/

enum acai_value_type {
	AVT_INTEGER,
	AVT_FLOAT,
	AVT_STRING
};

void llvm_value_init(llvm_ctx *ctx);
LLVMTypeRef llvm_value_type(void);

LLVMValueRef llvm_value_new_integer(llvm_ctx *ctx, int i);
