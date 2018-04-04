#include <stdlib.h>
#include <malloc.h>
#include "ctx.h"
#include "acai.h"
#include "value.h"
#include "../util.h"
#include "../parser/ast.h"
#include "../parser/parser.h"
#include "../builtin/builtin.h"

LLVMValueRef llvm_argument(llvm_ctx *ctx, tree *node) {

	switch(node->type) {

		case LIT_INTEGER:
			return llvm_value_new_integer(ctx, node->v.i);
	}

	printf("Unknown llvm node arg=%p type=%d\n", node, node->type);

	return NULL;
}

void llvm_function_call(llvm_ctx *ctx, tree *call) {

	int i;
	tree *right;
	struct ast_list *l;
	LLVMValueRef func;
	LLVMValueRef args[3];

	LLVMValueRef *argv, arg;
	unsigned int argc, argcap;

	tree *node = AST_CHILD_LEFT(call);

	if(node->type == TOK_IDENTIFIER) {

		func = LLVMGetNamedFunction(ctx->module, node->v.s);

		right = AST_CHILD_RIGHT(call);

		if(right->type == LIST_EXPRESSION) {

			argc = 0;
			argcap = 10;
			argv = calloc(argcap, sizeof(LLVMValueRef));

			AST_LIST_FOREACH(right, l) {

				arg = llvm_argument(ctx, AST_LIST_NODE(l));

				if(argc == argcap) {

					argcap += 10;
					argv = reallocarray(argv, argcap, sizeof(LLVMValueRef));
				}

				argv[argc] = arg;
				argc++;
			}

		}

		i = 0;
		if(func == NULL) {
			func = acai_get_func_call();
			args[i] = LLVMBuildGlobalStringPtr(ctx->builder, node->v.s, "func-name");
			i++;
		}

		args[i] = LLVMConstInt(LLVMInt64Type(), argc, FALSE);
		args[++i] = LLVMBuildArrayAlloca(ctx->builder,
						LLVMArrayType(LLVMPointerType(llvm_value_type(), 0), argc),
						LLVMConstArray(LLVMPointerType(llvm_value_type(), 0), argv, argc),
						"args");

		LLVMBuildCall(ctx->builder, func, args, 2, node->v.s);
	}
}
