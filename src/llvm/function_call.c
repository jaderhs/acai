#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "ctx.h"
#include "acai.h"
#include "value.h"
#include "../util.h"
#include "../parser/ast.h"
#include "../parser/parser.h"

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
	char *str;
	tree *right;
	struct ast_list *l;
	LLVMValueRef func;
	LLVMValueRef args[3];

	LLVMValueRef val, *argv, arg;
	unsigned int argc, argcap;

	tree *node = AST_CHILD_LEFT(call);

	if(node->type == TOK_IDENTIFIER) {

		func = LLVMGetNamedFunction(ctx->module, node->v.s);

		i = 0;
		if(func == NULL) {
			str = malloc(16 + strlen(node->v.s));
			sprintf(str, "func-name-%s", node->v.s);

			func = acai_get_func_call();
			args[i] = LLVMBuildGlobalStringPtr(ctx->builder, node->v.s, str);
			i++;
		}

		right = AST_CHILD_RIGHT(call);

		if(right->type == LIST_EXPRESSION) {

			argc = 0;
			argcap = 10;
			argv = calloc(argcap, sizeof(LLVMValueRef));

			AST_LIST_FOREACH(right, l) {

				val = llvm_argument(ctx, AST_LIST_NODE(l));

				str = malloc(48);
				sprintf(str, "func-call-argv%d", (argc+1));

				arg = LLVMBuildAlloca(ctx->builder, llvm_value_type(), str);
				LLVMBuildStore(ctx->builder, val, arg);

				if(argc == argcap) {

					argcap += 10;
					argv = reallocarray(argv, argcap, sizeof(LLVMValueRef));
				}

				argv[argc] = arg;
				argc++;
			}

		}

		argv[argc] = LLVMConstNull(llvm_value_type());
		argc++;

		str = malloc(24 + strlen(node->v.s));
		sprintf(str, "func-call-%s-args", node->v.s);

		args[i] = LLVMConstInt(LLVMInt64Type(), argc, FALSE);
		args[++i] = LLVMBuildArrayAlloca(ctx->builder,
						LLVMPointerType(llvm_value_type(), 0),
						LLVMConstArray(LLVMPointerType(llvm_value_type(), 0), argv, argc),
						str);

		str = malloc(16 + strlen(node->v.s));
		sprintf(str, "func-call-%s", node->v.s);

		LLVMBuildCall(ctx->builder, func, args, i+1, str);
	}
}
