#include <stdio.h>
#include "eval.h"
#include "func.h"
#include "../parser/parser.h"
#include "../util.h"
#include "../llvm/value.h"
#include "../llvm/llvm.h"

tree *eval(llvm_ctx *ctx, tree *node, unsigned int hint) {

	tree *p, *type;
	struct ast_list *l;

	switch(node->type) {

		case LIT_NULL:
		case LIT_FLOAT:
		case LIT_INTEGER:
		case LIT_STRING:
			node->lvl = llvm_value_literal_new(ctx, node);
			return node;

		case TOK_IDENTIFIER:
			/* Lookup symbol */
			p = llvm_identifier_list_lookup_by_name(ctx->scope->identifiers, node->v.s);
			if(p == NULL)
				p = llvm_identifier_list_lookup_by_name(ctx->global, node->v.s);
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

				l->node = eval(ctx, l->node, hint);
			}
			return node;

		case LIST_IDENTIFIER:
			return node;

		case LIST_TYPED_IDENTIFIER:

			type = eval(ctx, AST_CHILD_LEFT(node), hint);

			//right node is identifier list
			AST_LIST_FOREACH(AST_CHILD_RIGHT(node), l) {

				p = tree_new(TYPED_IDENTIFIER, type, AST_LIST_NODE(l));

				p->flags = hint;

				if((hint & EVAL_HINT_DECL_VAR_CONST) == 0) {
					llvm_decl_var(ctx, p);
				}

				p->llvm_type = type->llvm_type;

				ctx->scope->identifiers = llvm_identifier_list_prepend(ctx->scope->identifiers, p);

				l->node = p;

				if((hint & (EVAL_HINT_DECL_VAR_CONST|EVAL_HINT_DECL_VAR_DONT_INITIALIZE)) == 0) {
					llvm_value_literal *val = llvm_value_zero_initializer(ctx, type->v.i);
					LLVMBuildStore(ctx->builder, val->value, AST_ACAI_VALUE(p)->value);
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

		case DECL_CONST:
			return eval(ctx, AST_CHILD_LEFT(node), hint | EVAL_HINT_DECL_VAR_CONST);

		case DECL_FUNC:
			return eval_func_decl(ctx, node, hint);

		case FUNC_CALL:
			llvm_function_call(ctx, node);
			break;

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
