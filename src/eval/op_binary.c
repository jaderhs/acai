#include <stdio.h>
#include "eval.h"
#include "parser/parser.h"
#include "util.h"
#include "llvm/value.h"
#include "llvm/llvm.h"

tree *eval_op_binary(llvm_ctx *ctx, int op, tree *left, tree *right, unsigned int hint) {

	tree *node_lvalue, *node_rvalue, *p, *tidentifier;
	struct ast_list *iter_left, *iter_right;
	LLVMValueRef ptr;

	llvm_scope *curr_scope = llvm_ctx_get_current_scope(ctx);

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

				ptr = LLVMBuildBitCast(ctx->builder, llvm_acai_value_get_value(ctx, AST_ACAI_VALUE(left)), LLVMPointerType(AST_VALUE_LITERAL(AST_LIST_NODE(tree_list_get_first(right)))->type, 0), "");

				LLVMBuildStore(ctx->builder, AST_LIST_NODE(tree_list_get_first(right))->lvl, ptr);
			}
			else if (right->type == LIST_EXPRESSION) {

				llvm_acai_value *lav = NULL;
				llvm_value_literal *val = NULL;

				iter_right = tree_list_get_last(right);

				AST_LIST_FOREACH(left, iter_left) {

					node_lvalue = AST_LIST_NODE(iter_left);

					if(iter_right) {

						node_rvalue = AST_LIST_NODE(iter_right);

						if(IS_LITERAL(node_rvalue)) {

							if((node_rvalue->flags & EVAL_HINT_DECL_VAR_CONST) == 0) {
								val = node_rvalue->lvl;
							}
							else {
								//node_rvalue=DECL_CONST TYPED_IDENTIFIER
								val = AST_CHILD_RIGHT(node_rvalue)->lvl;

							}
						}
						else if(node_rvalue->type == TYPED_IDENTIFIER) {

							if((node_rvalue->flags & EVAL_HINT_DECL_VAR_CONST) == 0) {
								lav = node_rvalue->av;
							}
							else {
								val = node_rvalue->lvl;
							}
						}

						iter_right = iter_right->prev;

					} else {

						val = llvm_value_zero_initializer(ctx, AST_CHILD_LEFT(node_lvalue)->v.i);
					}

					/* build correct typed_identifier lvalue for an assigndeclare */
					if(op == TOK_OP_ASSIGNDECLARE && node_lvalue->type == TOK_IDENTIFIER) {

						//node_lvalue = tok_identifier
						//node_rvalue = value

						//build llvm value from the same type as val
						tidentifier = tree_new(TYPED_IDENTIFIER, tree_variable_type_new(llvm_acai_type_infer(node_rvalue), FALSE), node_lvalue);
						tidentifier->flags = hint;

						AST_CHILD_LEFT(tidentifier)->llvm_type = node_rvalue->llvm_type;

						if((hint & EVAL_HINT_DECL_VAR_CONST) != 0) {

							node_rvalue->lvl = llvm_value_literal_new(ctx, node_rvalue);

							//copy value into identifier for constant
							AST_CHILD_RIGHT(tidentifier)->lvl = node_rvalue->lvl;
						}
						else {

							llvm_decl_var_assigndeclare(ctx, node_lvalue, tidentifier);
						}

//						tidentifier->llvm_type = node_rvalue->llvm_type;

						node_lvalue = tidentifier;
						iter_left->node = tidentifier;
					}

					if((hint & EVAL_HINT_DECL_VAR_CONST) != 0) {
						node_lvalue->lvl = val;
					}

					curr_scope->symbols = llvm_symbol_list_prepend(curr_scope->symbols, node_lvalue);

					if((hint & (EVAL_HINT_DECL_VAR_CONST|EVAL_HINT_DECL_VAR_DONT_INITIALIZE)) == 0) {

						if(val != NULL) {
							ptr = LLVMBuildBitCast(ctx->builder, llvm_acai_value_get_value(ctx, AST_ACAI_VALUE(node_lvalue)), LLVMPointerType(val->type, 0), "");
							LLVMBuildStore(ctx->builder, val->value, ptr);
						}
						else if(lav != NULL) {

							llvm_acai_value_copy(ctx, lav, AST_ACAI_VALUE(node_lvalue));
						}
						else {
							fprintf(stderr, "Unable to find rvalue for identifier %s\n", node_lvalue->v.child[1]->v.s);
							return NULL;
						}
					}
				}

			} else {
				fprintf(stderr, "Unknown right operand (%d) for assign operator\n", right->type);
				return NULL;
			}

			break;

		default:
			fprintf(stderr, "Binary op %d not executed by eval_op_binary()\n", op);
			break;
	}

	return NULL;
}
