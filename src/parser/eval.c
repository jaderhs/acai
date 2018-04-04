#include <stdio.h>
#include "eval.h"
#include "parser.h"
#include "../llvm/llvm.h"

tree *eval(llvm_ctx *ctx, tree *node) {

	tree *p;
	struct ast_list *l;

	switch(node->type) {

		case LIST_STATEMENT:
		case LIST_EXPRESSION:

			AST_LIST_FOREACH(node, l) {

				eval(ctx, l->node);
			}

			break;

		case DECL_VAR:
			break;

		case FUNC_CALL:
			llvm_function_call(ctx, node);
			break;

		default:
			printf("Tree type %d not executed by eval()\n", node->type);
			break;
	}

	return node;
}
