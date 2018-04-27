#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "ctx.h"
#include "acai.h"
#include "value.h"
#include "util.h"
#include "acai/type.h"
#include "parser/parser.h"

void llvm_argument_store(llvm_ctx *ctx, tree *node, llvm_value_literal *lvl) {

	LLVMValueRef llvm_str;

	/* store string */
	if(node->type == LIT_STRING) {
		llvm_str = LLVMBuildAlloca(ctx->builder, LLVMArrayType(LLVMInt8Type(), strlen(node->v.s)+1), "");
		LLVMBuildStore(ctx->builder, lvl->value, llvm_str);
	}

	/* cast acai.value to argument type */
	LLVMValueRef val_cast = LLVMBuildBitCast(ctx->builder, llvm_acai_value_get_value(ctx, AST_ACAI_VALUE(node)), LLVMPointerType(lvl->type, 0), "");

	if(node->type == LIT_STRING) {
		LLVMBuildStore(ctx->builder, llvm_str, val_cast);
	}
	else {
		LLVMBuildStore(ctx->builder, lvl->value, val_cast);
	}

}

int llvm_argument(llvm_ctx *ctx, tree *node) {

	tree *identifier = NULL;
	llvm_acai_value acai_value;
	llvm_value_literal *lvl;

	if(node->type == TOK_IDENTIFIER) {

		if(ctx->scope.local != NULL)
			identifier = llvm_symbol_list_lookup_by_name(ctx->scope.local->symbols, node->v.s);

		if(identifier == NULL)
			identifier = llvm_symbol_list_lookup_by_name(ctx->scope.global->symbols, node->v.s);

		if(identifier == NULL) {
			fprintf(stderr, "Identifier '%s' not found\n", node->v.s);
			return FALSE;
		}

		node->av = identifier->av;
		//llvm_value_new_identifier(ctx, p, &acai_value);
		//return TRUE;
	}
	else if(node->type == TOK_NULL) {

		LLVMValueRef type;
		node->av = llvm_acai_value_new_alloca_with_type(ctx, "", AT_STRING, TRUE);
	}
	else if((lvl = llvm_value_literal_new(ctx, node)) != NULL) {

		node->av = llvm_acai_value_new_alloca_with_type(ctx, "", lvl->acai_type, FALSE);
		llvm_argument_store(ctx, node, lvl);
	}
	else {

		fprintf(stderr, "Unknown llvm node arg=%p type=%d\n", node, node->type);
		return FALSE;
	}
	return TRUE;
}

tree *llvm_function_call(llvm_ctx *ctx, tree *call) {

	int i, j;
	char *str;
	tree *func, *list_parameters, *right, *rvalue, *p;
	struct ast_list *l;
	LLVMValueRef llvm_func;
	LLVMValueRef args[3];

	LLVMValueRef valstr, off, atp, argv_array, *argv, arg;
	unsigned int argc, argcap;

	tree *node = AST_CHILD_LEFT(call);

	if(node->type != TOK_IDENTIFIER) {
		fprintf(stderr, "Unknown func type in llvm_function_call(): %d\n", node->type);
		return FALSE;
	}

	i = 0;

	func = llvm_symbol_list_lookup_by_name(ctx->scope.global->symbols, node->v.s);
	if(func == NULL) {

		str = malloc(16 + strlen(node->v.s));
		sprintf(str, "func-name-%s", node->v.s);

		llvm_func = acai_get_func_call();
		args[i] = LLVMBuildGlobalStringPtr(ctx->builder, node->v.s, str);
		i++;
	}
	else {
		llvm_func = func->v.func.llvm_func;
	}

	right = AST_CHILD_RIGHT(call);

	if(right->type == LIST_EXPRESSION) {

		argc = 0;
		argcap = 10;
		argv = calloc(argcap, sizeof(LLVMValueRef));

		AST_LIST_FOREACH(right, l) {

			rvalue = AST_LIST_NODE(l);

			if(llvm_argument(ctx, rvalue) == FALSE) {
				return FALSE;
			}

			argv[argc] = AST_ACAI_VALUE(rvalue)->begin;

			argc++;
			if(argc == argcap) {

				argcap += 10;
				argv = reallocarray(argv, argcap, sizeof(LLVMValueRef));
			}
		}

	}

	if(func != NULL) {

		list_parameters = AST_CHILD_LEFT(((tree*) func->v.func.signature));

		if(argc < tree_list_length(list_parameters)) {

			j = 0;
			AST_LIST_FOREACH(list_parameters, l) {

				if(j >= argc) {

					p = AST_LIST_NODE(l);
					if(p->type != DECL_PARAM) {
						fprintf(stderr, "Unknown type in function signature: %d\n", p->type);
						return FALSE;
					}

					rvalue = AST_CHILD_RIGHT(p);

					if(rvalue == NULL) {
						fprintf(stderr, "Missing function arguments on call to %s\n", node->v.s);
						return FALSE;
					}

					if(llvm_argument(ctx, rvalue) == FALSE) {
						return FALSE;
					}

					argv[argc] = AST_ACAI_VALUE(rvalue)->begin;

					argc++;
					if(argc == argcap) {

						argcap += 10;
						argv = reallocarray(argv, argcap, sizeof(LLVMValueRef));
					}
				}

				j++;
			}
		}
		//if(argc < //num of args in func, load default value for rargs)
	}

	argv[argc] = LLVMConstPointerNull(LLVMPointerType(llvm_value_type(), 0));

	str = malloc(24 + strlen(node->v.s));
	sprintf(str, "func-call-%s-argv", node->v.s);
/*
	atp = LLVMBuildStructGEP(ctx->builder, arg, 0, str);
	LLVMBuildStore(ctx->builder, val[0], atp);
*/

	args[i++] = LLVMConstInt(LLVMInt64Type(), argc, FALSE);
	//args[++i] = LLVMConstNull(LLVMPointerType(LLVMPointerType(llvm_value_type(), 0), 0));
	argv_array = LLVMBuildAlloca(ctx->builder,
					LLVMArrayType(LLVMPointerType(llvm_value_type(), 0), argc+1),
//						LLVMConstArray(LLVMPointerType(llvm_value_type(), 0), argv, argc),
					str);

	/* store argv */
	LLVMValueRef idx[2];
	idx[0] = LLVMConstInt(LLVMInt64Type(), 0, FALSE);
	for(j = 0; j < argc+1; j++) {
		idx[1] = LLVMConstInt(LLVMInt64Type(), j, FALSE);

		atp = LLVMBuildGEP(ctx->builder, argv_array, idx, 2, "");
		LLVMBuildStore(ctx->builder, argv[j], atp);
	}

	atp = LLVMConstInt(LLVMInt64Type(), 0, FALSE);
	args[i++] = LLVMBuildBitCast(ctx->builder, argv_array, LLVMPointerType(LLVMPointerType(llvm_value_type(), 0), 0), "");

	str = malloc(16 + strlen(node->v.s));
	sprintf(str, "func-call-%s", node->v.s);

	LLVMBuildCall(ctx->builder, llvm_func, args, i, str);

	return node;
}
