#include <stdio.h>
#include "eval.h"
#include "parser.h"
#include "../util.h"
#include "../llvm/value.h"
#include "../llvm/llvm.h"

#define IS_LITERAL(n) (n->type == LIT_INTEGER || n->type == LIT_FLOAT)

tree *eval_op_binary(llvm_ctx *ctx, int op, tree *left, tree *right) {

	tree *node1, *node2, *p;
	struct ast_list *iter_left, *iter_right;

	switch(op) {

		case '=':
		case TOK_OP_ASSIGNDECLARE:
			left = eval(ctx, left);

			if(left->type != LIST_IDENTIFIER) {
				printf("Unknown left operand (%d) for assign operator\n", left->type);
				return NULL;
			}

			right = eval(ctx, right);

			if(IS_LITERAL(right)) {

				LLVMBuildStore(ctx->builder, AST_LIST_NODE(tree_list_get_first(right))->llvm_value, left->llvm_value);
			}
			else if (right->type == LIST_EXPRESSION) {

				LLVMValueRef val;

				iter_right = tree_list_get_last(right);

				AST_LIST_FOREACH(left, iter_left) {

					node1 = AST_LIST_NODE(iter_left);

					if(iter_right) {

						node2 = AST_LIST_NODE(iter_right);
						val = node2->llvm_value;

						iter_right = iter_right->prev;

					} else {

						val = llvm_value_zero_initializer(ctx, AST_CHILD_LEFT(node1)->v.i);
					}

					if(op == TOK_OP_ASSIGNDECLARE && node1->type == TOK_IDENTIFIER) {

						//build llvm value from the same type as val
						p = tree_new_with_children(TYPED_IDENTIFIER, tree_variable_type_new(node2->type, FALSE), node1);

						p->llvm_value = llvm_decl_var_assigndeclare(ctx, node1, node2);
						p->llvm_type = node2->llvm_type;

						ctx->scope->identifiers = llvm_identifier_list_prepend(ctx->scope->identifiers, p);
						node1 = p;
					}

					LLVMBuildStore(ctx->builder, val, node1->llvm_value);
				}

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

		case LIT_FLOAT:
		case LIT_INTEGER:
			llvm_value_literal_new(ctx, node);
			return node;

		case LIST_STATEMENT:

			p = NULL;
			AST_LIST_FOREACH(node, l) {

				p = eval(ctx, l->node);
			}
			return p;

		case LIST_EXPRESSION:

			p = NULL;
			AST_LIST_FOREACH(node, l) {

				l->node = eval(ctx, l->node);
			}
			return node;

		case LIST_IDENTIFIER:
			return node;

		case LIST_TYPED_IDENTIFIER:

			type = eval(ctx, AST_CHILD_LEFT(node));

			//right node is identifier list
			AST_LIST_FOREACH(AST_CHILD_RIGHT(node), l) {

				p = tree_new_with_children(TYPED_IDENTIFIER, type, AST_LIST_NODE(l));

				p->llvm_value = llvm_decl_var(ctx, p);
				p->llvm_type = type->llvm_type;

				ctx->scope->identifiers = llvm_identifier_list_prepend(ctx->scope->identifiers, p);

				l->node = p;
			}

			//return identifier list of typed_identifier
			return AST_CHILD_RIGHT(node);

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
