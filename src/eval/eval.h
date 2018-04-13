#include "../parser/ast.h"
#include "../llvm/ctx.h"

#define EVAL_HINT_DECL_VAR_DONT_INITIALIZE	1
#define EVAL_HINT_DECL_VAR_CONST			2

tree *eval(llvm_ctx *ctx, tree *node, unsigned int hint);
tree *eval_op_binary(llvm_ctx *ctx, int op, tree *left, tree *right, unsigned int hint);
