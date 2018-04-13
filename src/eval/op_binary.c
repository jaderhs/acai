#include <stdio.h>
#include "eval.h"
#include "../parser/parser.h"
#include "../util.h"
#include "../llvm/value.h"
#include "../llvm/llvm.h"

#define IS_LITERAL(n) (n->type == LIT_INTEGER || n->type == LIT_FLOAT)

tree *eval_op_binary(llvm_ctx *ctx, int op, tree *left, tree *right, unsigned int hint) {

	tree *node1, *node2, *p;
	struct ast_list *iter_left, *iter_right;

	switch(op) {

		case '=':
		case TOK_OP_ASSIGNDECLARE:
			left = eval(ctx, left, hint | EVAL_HINT_DECL_VAR_DONT_INITIALIZE);

			if(left->type != LIST_IDENTIFIER) {
				printf("Unknown left operand (%d) for assign operator\n", left->type);
				return NULL;
			}

			right = eval(ctx, right, hint);

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

						if(node2->flags & EVAL_HINT_DECL_VAR_CONST) {

							//node2=DECL_CONT TYPED_IDENTIFIER
							val = AST_CHILD_RIGHT(node2)->llvm_value;

						} else {
							val = node2->llvm_value;
						}

						iter_right = iter_right->prev;

					} else {

						val = llvm_value_zero_initializer(ctx, AST_CHILD_LEFT(node1)->v.i);
					}

					if(op == TOK_OP_ASSIGNDECLARE && node1->type == TOK_IDENTIFIER) {

						//node1 = tok_identifier
						//node2 = value

						//build llvm value from the same type as val
						p = tree_new(TYPED_IDENTIFIER, tree_variable_type_new(llvm_acai_type_infer(node2), FALSE), node1);
						p->flags = hint;

						AST_CHILD_LEFT(p)->llvm_type = node2->llvm_type;

						if((hint & EVAL_HINT_DECL_VAR_CONST) != 0) {
							llvm_value_literal_new(ctx, node2);

							//copy value into identifier for constant
							AST_CHILD_RIGHT(p)->llvm_value = node2->llvm_value;
						}
						else {
							p->llvm_value = llvm_decl_var_assigndeclare(ctx, node1, node2);
						}

						p->llvm_type = node2->llvm_type;

						ctx->scope->identifiers = llvm_identifier_list_prepend(ctx->scope->identifiers, p);
						node1 = p;
					}

					if((hint & (EVAL_HINT_DECL_VAR_CONST|EVAL_HINT_DECL_VAR_DONT_INITIALIZE)) == 0) {
						LLVMBuildStore(ctx->builder, val, node1->llvm_value);
					}
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
