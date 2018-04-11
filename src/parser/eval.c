#include <stdio.h>
#include "eval.h"
#include "parser.h"
#include "../llvm/value.h"
#include "../llvm/llvm.h"

#define IS_LITERAL(n) (n->type == LIT_INTEGER)

tree *eval_op_binary(llvm_ctx *ctx, int op, tree *left, tree *right) {

	switch(op) {

		case '=':
		case TOK_OP_ASSIGNDECLARE:
			left = eval(ctx, left);
			if(left->type != TYPED_IDENTIFIER) {
				printf("Unknown left operand (%d) for assign operator\n", left->type);
				return NULL;
			}

			right = eval(ctx, right);

			if(IS_LITERAL(right)) {

				LLVMBuildStore(ctx->builder, right->llvm_value, left->llvm_value);

			} else {
				printf("Unknown right operand (%d) for assign operator\n", right->type);
				return NULL;
			}

			break;

		default:
			printf("Binary op %d not executed by eval_op_binary()\n", op);
			break;
	}

	return NULL;
}

tree *eval(llvm_ctx *ctx, tree *node) {

	tree *p, *type;
	struct ast_list *l;

	switch(node->type) {

		case LIT_INTEGER:
			node->llvm_value = llvm_value_integer_new(ctx, node->v.i);
			return node;

		case LIST_STATEMENT:
		case LIST_EXPRESSION:

			p = NULL;
			AST_LIST_FOREACH(node, l) {

				p = eval(ctx, l->node);
			}
			return p;

		case LIST_TYPED_IDENTIFIER:

			type = eval(ctx, AST_CHILD_LEFT(node));

			//right node is identifier list
			p = NULL;
			AST_LIST_FOREACH(AST_CHILD_RIGHT(node), l) {

				p = tree_new_with_children(TYPED_IDENTIFIER, type, AST_LIST_NODE(l));
				p->llvm_value = llvm_decl_var(ctx, p);

				ctx->scope->identifiers = llvm_identifier_list_prepend(ctx->scope->identifiers, p);
			}

			//return last typed identifier
			return p;

		case DECL_VAR:
			return eval(ctx, AST_CHILD_LEFT(node));

		case FUNC_CALL:
			llvm_function_call(ctx, node);
			break;

		case TOK_TYPENAME:
			return llvm_type_new(ctx, node);

		case OP_BINARY:
			return eval_op_binary(ctx, node->v.op.op, AST_OP_LEFT(node), AST_OP_RIGHT(node));

		default:
			printf("Tree type %d not executed by eval()\n", node->type);
			break;
	}

	return node;
}
