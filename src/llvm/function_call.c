#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "ctx.h"
#include "acai.h"
#include "value.h"
#include "../util.h"
#include "../parser/ast.h"
#include "../parser/parser.h"

int llvm_argument(llvm_ctx *ctx, tree *node, llvm_acai_value *val) {

	switch(node->type) {

		case LIT_INTEGER:
			llvm_value_new_integer(ctx, node->v.i, val);
			return TRUE;
	}

	printf("Unknown llvm node arg=%p type=%d\n", node, node->type);

	return FALSE;
}

void llvm_function_call(llvm_ctx *ctx, tree *call) {

	int i, j;
	char *str;
	tree *right;
	struct ast_list *l;
	LLVMValueRef func;
	LLVMValueRef args[3];

	llvm_acai_value acai_value;
	LLVMValueRef *val, atp, argv_array, *argv, arg;
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

				if(llvm_argument(ctx, AST_LIST_NODE(l), &acai_value) == FALSE) {

				}

				/* alloc acai_type{} */

				str = malloc(48);
				sprintf(str, "func-call-argv%d", (argc+1));

				arg = LLVMBuildAlloca(ctx->builder, llvm_value_type(), str);

				/* Set acai_type.type */

				str = malloc(48);
				sprintf(str, "func-call-argv%d.type", (argc+1));

				atp = LLVMBuildStructGEP(ctx->builder, arg, 0, str);
				LLVMBuildStore(ctx->builder, acai_value.acai_type, atp);

				/* Set acai_type.v */

				/* Get pointer for acai_Type.v */
				str = malloc(48);
				sprintf(str, "func-call-argv%d.v", (argc+1));

				atp = LLVMBuildStructGEP(ctx->builder, arg, 1, str);

				/* cast v to argument type */

				str = malloc(48);
				sprintf(str, "func-call-argv%d.v", (argc+1));

				atp = LLVMBuildBitCast(ctx->builder, atp, LLVMPointerType(acai_value.type, 0), str);
				LLVMBuildStore(ctx->builder, acai_value.val, atp);

				argv[argc] = arg;

				argc++;
				if(argc == argcap) {

					argcap += 10;
					argv = reallocarray(argv, argcap, sizeof(LLVMValueRef));
				}
			}

		}

		argv[argc] = LLVMConstNull(llvm_value_type());

		str = malloc(24 + strlen(node->v.s));
		sprintf(str, "func-call-%s-argv", node->v.s);
/*
		atp = LLVMBuildStructGEP(ctx->builder, arg, 0, str);
		LLVMBuildStore(ctx->builder, val[0], atp);
*/

		args[i++] = LLVMConstInt(LLVMInt64Type(), argc, FALSE);
		//args[++i] = LLVMConstNull(LLVMPointerType(LLVMPointerType(llvm_value_type(), 0), 0));
		argv_array = LLVMBuildAlloca(ctx->builder,
						LLVMArrayType(LLVMPointerType(llvm_value_type(), 0), argc),
//						LLVMConstArray(LLVMPointerType(llvm_value_type(), 0), argv, argc),
						str);

		/* store argv */
		LLVMValueRef idx[2];
		for(j = 0; j < argc; j++) {

			idx[0] = LLVMConstInt(LLVMInt64Type(), 0, FALSE);
			idx[1] = LLVMConstInt(LLVMInt64Type(), j, FALSE);

			atp = LLVMBuildGEP(ctx->builder, argv_array, idx, 2, "");
			LLVMBuildStore(ctx->builder, argv[j], atp);
		}

		atp = LLVMConstInt(LLVMInt64Type(), 0, FALSE);
		args[i++] = LLVMBuildGEP(ctx->builder, argv_array, &atp, 1, "");

		str = malloc(16 + strlen(node->v.s));
		sprintf(str, "func-call-%s", node->v.s);

		LLVMBuildCall(ctx->builder, func, args, i, str);
	}
}
