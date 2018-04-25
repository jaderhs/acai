#include <stdio.h>
#include "eval.h"
#include "func.h"
#include "util.h"
#include "parser/parser.h"
#include "llvm/value.h"
#include "llvm/llvm.h"

tree *eval(llvm_ctx *ctx, tree *node, unsigned int hint) {

	tree *p, *type;
	struct ast_list *l;
	llvm_scope *curr_scope;

	switch(node->type) {

		case LIT_NULL:
		case LIT_FLOAT:
		case LIT_INTEGER:
		case LIT_STRING:
			node->lvl = llvm_value_literal_new(ctx, node);
			return node;

		case TOK_IDENTIFIER:
			/* Lookup symbol */

			p = NULL;
			if(ctx->scope.local != NULL)
				p = llvm_symbol_list_lookup_by_name(ctx->scope.local->symbols, node->v.s);

			if(p == NULL)
				p = llvm_symbol_list_lookup_by_name(ctx->scope.global->symbols, node->v.s);

			return p;

		case LIST_STATEMENT:

			p = NULL;
			AST_LIST_FOREACH(node, l) {

				p = eval(ctx, l->node, hint);
			}
			return p;

		case LIST_EXPRESSION:

			p = NULL;
			AST_LIST_FOREACH(node, l) {

				l->node = eval(ctx, AST_LIST_NODE(l), hint);
			}
			return node;

		case LIST_IDENTIFIER:
			return node;

		case LIST_TYPED_IDENTIFIER:

			curr_scope = llvm_ctx_get_current_scope(ctx);

			/* Convert to a LIST_IDENTIFIER of TYPED_IDENTIFIERS */
			type = eval(ctx, AST_CHILD_LEFT(node), hint);

			//right node is identifier list
			AST_LIST_FOREACH(AST_CHILD_RIGHT(node), l) {

				p = tree_new(TYPED_IDENTIFIER, type, AST_LIST_NODE(l));

				p->flags = hint;

				if((hint & EVAL_HINT_DECL_VAR_CONST) == 0) {
					llvm_decl_var(ctx, p);
				}

				p->llvm_type = type->llvm_type;

				curr_scope->symbols = llvm_symbol_list_prepend(curr_scope->symbols, p);

				l->node = p;

				if((hint & (EVAL_HINT_DECL_VAR_CONST|EVAL_HINT_DECL_VAR_DONT_INITIALIZE)) == 0) {

					llvm_value_literal *val = llvm_value_zero_initializer(ctx, type->v.i);
					LLVMBuildStore(ctx->builder, val->value, llvm_acai_value_get_value(ctx, AST_ACAI_VALUE(p)));
				}
			}

			//return identifier list of typed_identifier
			return AST_CHILD_RIGHT(node);

		case KEYWORD_RETURN:
			return eval_func_return(ctx, node, hint);

		case BLOCK:
			return eval(ctx, AST_CHILD_LEFT(node), hint);

		case DECL_VAR:
			return eval(ctx, AST_CHILD_LEFT(node), hint);

		case DECL_PARAM:
			p = AST_CHILD_RIGHT(node);

			if(p != NULL)
				AST_CHILD_RIGHT(node) = eval(ctx, p, hint);
			return node;

		case DECL_CONST:
			return eval(ctx, AST_CHILD_LEFT(node), hint | EVAL_HINT_DECL_VAR_CONST);

		case DECL_FUNC:
			return eval_func_decl(ctx, node, hint);

		case FUNC_CALL:
			return llvm_function_call(ctx, node);

		case TOK_TYPENAME:
			return llvm_type_new(ctx, node);

		case OP_BINARY:
			return eval_op_binary(ctx, node->v.op.op, AST_OP_LEFT(node), AST_OP_RIGHT(node), hint);

		default:
			printf("Tree type %d not executed by eval()\n", node->type);
			break;
	}

	return node;
}
