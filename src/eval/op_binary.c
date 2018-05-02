#include <stdio.h>
#include "eval.h"
#include "parser/parser.h"
#include "util.h"
#include "acai/type.h"
#include "llvm/value.h"
#include "llvm/llvm.h"

tree *eval_op_binary(llvm_ctx *ctx, int op, tree *left, tree *right, unsigned int hint) {

	tree *lvalue, *rvalue, *p, *tidentifier;
	struct ast_list *iter_left, *iter_right;
	LLVMValueRef ptr;
	llvm_acai_value *lav = NULL;
	llvm_value_literal *lvl = NULL;

	llvm_scope *curr_scope = llvm_ctx_get_current_scope(ctx);

	left  = eval(ctx, left, hint | EVAL_HINT_DECL_VAR_DONT_INITIALIZE);
	right = eval(ctx, right, hint);

	switch(op) {

		case '=':
		case TOK_OP_ASSIGNDECLARE:

			if (right->type != LIST_EXPRESSION) {

				printf("Unknown right operand (%d) for assign operator on eval_op_binary()\n", left->type);
				return NULL;
			}

			if(left->type != LIST_IDENTIFIER) {

				printf("Unknown left operand (%d) for assign operator on eval_op_binary()\n", left->type);
				return NULL;
			}

			iter_right = tree_list_get_last(right);
			AST_LIST_FOREACH(left, iter_left) {

				lvl = NULL;
				lav = NULL;

				lvalue = AST_LIST_NODE(iter_left);

				rvalue = (iter_right != NULL ? AST_LIST_NODE(iter_right) : NULL);

				if(op == TOK_OP_ASSIGNDECLARE) {

					if(rvalue == NULL) {
						fprintf(stderr, "All variables must be assigned a value when using operator \":=\"\n");
						return NULL;
					}

					/* build a new typed_identifier from inferred rvalue */
					tidentifier = tree_new(TYPED_IDENTIFIER, tree_variable_type_new(llvm_acai_type_infer(rvalue), FALSE), lvalue);
					tidentifier->flags = hint;

					AST_CHILD_LEFT(tidentifier)->llvm_type = rvalue->llvm_type;

					if((hint & EVAL_HINT_DECL_VAR_CONST) != 0) { //is const

						rvalue->lvl = llvm_value_literal_new_from_node(ctx, rvalue);

						//copy value into identifier for constant
						AST_CHILD_RIGHT(tidentifier)->lvl = rvalue->lvl;
					}
					else {

						llvm_decl_var(ctx, tidentifier, FALSE);
					}

					lvalue = tidentifier;
				}

				if(rvalue == NULL) {

					llvm_acai_value_initialize(ctx, AST_CHILD_LEFT(lvalue)->v.i, AST_ACAI_VALUE(lvalue));
				}
				else {

					if(IS_LITERAL(rvalue)) {

						if((rvalue->flags & EVAL_HINT_DECL_VAR_CONST) == 0) {
							lvl = rvalue->lvl;
						}
						else {
							//rvalue=DECL_CONST TYPED_IDENTIFIER
							lvl = AST_CHILD_RIGHT(rvalue)->lvl;
						}
					}
					else if(rvalue->type == TYPED_IDENTIFIER) {

						if((rvalue->flags & EVAL_HINT_DECL_VAR_CONST) == 0) {
							lav = rvalue->av;
						}
						else {
							lvl = rvalue->lvl;
						}
					}
					else if(rvalue->type == TOK_NULL) {
						//zero initialized value
						rvalue = NULL;
					}
				}

				if((hint & EVAL_HINT_DECL_VAR_CONST) != 0) {
					lvalue->lvl = lvl;
				}

				curr_scope->symbols = llvm_symbol_list_prepend(curr_scope->symbols, lvalue);

				if((hint & (EVAL_HINT_DECL_VAR_CONST|EVAL_HINT_DECL_VAR_DONT_INITIALIZE)) == 0) {

					if(lvl != NULL) {

						LLVMValueRef llvm_val = lvl->value;
						LLVMTypeRef llvm_type = LLVMPointerType(lvl->type, 0);

						if(rvalue != NULL && rvalue->type == LIT_STRING) {

							llvm_val = LLVMBuildAlloca(ctx->builder, lvl->type, "");
							LLVMBuildStore(ctx->builder, lvl->value, llvm_val);

							llvm_type = LLVMPointerType(llvm_type, 0);
						}

						ptr = LLVMBuildBitCast(ctx->builder, llvm_acai_value_get_value(ctx, AST_ACAI_VALUE(lvalue)), llvm_type, "");
						LLVMBuildStore(ctx->builder, llvm_val, ptr);
					}
					else if(lav != NULL) {

						llvm_acai_value_copy(ctx, lav, AST_ACAI_VALUE(lvalue));
					}
					else if(rvalue == NULL) {
						//zero initialized value
					}
					else {
						fprintf(stderr, "Unable to find rvalue for identifier %s\n", lvalue->v.child[1]->v.s);
						return NULL;
					}
				}

				//advance right iterator
				if(iter_right != NULL)
					iter_right = iter_right->prev;
			}

			break;

		default:
			fprintf(stderr, "Binary op %d not executed by eval_op_binary()\n", op);
			break;
	}

	return NULL;
}
