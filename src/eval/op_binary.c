#include <stdio.h>
#include "eval.h"
#include "../parser/parser.h"
#include "../util.h"
#include "../llvm/value.h"
#include "../llvm/llvm.h"

tree *eval_op_binary(llvm_ctx *ctx, int op, tree *left, tree *right, unsigned int hint) {

	tree *node_lvalue, *node_rvalue, *p;
	struct ast_list *iter_left, *iter_right;
	LLVMValueRef ptr;

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

				ptr = LLVMBuildBitCast(ctx->builder, AST_ACAI_VALUE(left)->value, LLVMPointerType(AST_VALUE_LITERAL(AST_LIST_NODE(tree_list_get_first(right)))->type, 0), "");

				LLVMBuildStore(ctx->builder, AST_LIST_NODE(tree_list_get_first(right))->lvl, ptr);
			}
			else if (right->type == LIST_EXPRESSION) {

				llvm_value_literal *val;

				iter_right = tree_list_get_last(right);

				AST_LIST_FOREACH(left, iter_left) {

					node_lvalue = AST_LIST_NODE(iter_left);

					if(iter_right) {

						node_rvalue = AST_LIST_NODE(iter_right);

						if(node_rvalue->flags & EVAL_HINT_DECL_VAR_CONST) {

							//node_rvalue=DECL_CONST TYPED_IDENTIFIER
							val = AST_CHILD_RIGHT(node_rvalue)->lvl;
						} else {

							val = node_rvalue->lvl;
						}

						iter_right = iter_right->prev;

					} else {

						val = llvm_value_zero_initializer(ctx, AST_CHILD_LEFT(node_lvalue)->v.i);
					}

					if(op == TOK_OP_ASSIGNDECLARE && node_lvalue->type == TOK_IDENTIFIER) {

						//node_lvalue = tok_identifier
						//node_rvalue = value

						//build llvm value from the same type as val
						p = tree_new(TYPED_IDENTIFIER, tree_variable_type_new(llvm_acai_type_infer(node_rvalue), FALSE), node_lvalue);
						p->flags = hint;

						AST_CHILD_LEFT(p)->llvm_type = node_rvalue->llvm_type;

						if((hint & EVAL_HINT_DECL_VAR_CONST) != 0) {
							node_rvalue->lvl = llvm_value_literal_new(ctx, node_rvalue);

							//copy value into identifier for constant
							AST_CHILD_RIGHT(p)->lvl = node_rvalue->lvl;
						}
						else {

							llvm_decl_var_assigndeclare(ctx, node_lvalue, p);
						}

//						p->llvm_type = node_rvalue->llvm_type;

						ctx->scope->identifiers = llvm_identifier_list_prepend(ctx->scope->identifiers, p);
						node_lvalue = p;
					}

					if((hint & (EVAL_HINT_DECL_VAR_CONST|EVAL_HINT_DECL_VAR_DONT_INITIALIZE)) == 0) {

						ptr = LLVMBuildBitCast(ctx->builder, AST_ACAI_VALUE(node_lvalue)->value, LLVMPointerType(val->type, 0), "");
						LLVMBuildStore(ctx->builder, val->value, ptr);
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
