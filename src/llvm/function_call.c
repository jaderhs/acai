#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "parser/ast.h"
#include "ctx.h"
#include "acai.h"
#include "value.h"
#include "util.h"
#include "parser/parser.h"

void llvm_argument_store(llvm_ctx *ctx, tree *node, llvm_value_literal *lvl) {

	LLVMValueRef llvm_str;

	/* store string */
	if(node->type == LIT_STRING) {
		llvm_str = LLVMBuildAlloca(ctx->builder, LLVMArrayType(LLVMInt8Type(), strlen(node->v.s)+1), "");
		LLVMBuildStore(ctx->builder, lvl->value, llvm_str);
	}

	/* cast acai.value to argument type */
	LLVMValueRef val_cast = LLVMBuildBitCast(ctx->builder, AST_ACAI_VALUE(node)->value, LLVMPointerType(lvl->type, 0), "");

	if(node->type == LIT_STRING) {
		LLVMBuildStore(ctx->builder, llvm_str, val_cast);
	}
	else {
		LLVMBuildStore(ctx->builder, lvl->value, val_cast);
	}

}

int llvm_argument(llvm_ctx *ctx, tree *node) {

	tree *identifier;
	llvm_acai_value acai_value;
	llvm_value_literal *lvl;

	if(node->type == TOK_IDENTIFIER) {

		identifier = llvm_identifier_list_lookup_by_name(ctx->scope->identifiers, node->v.s);
		if(identifier == NULL) {
			fprintf(stderr, "Identifier '%s' not found\n", node->v.s);
			return FALSE;
		}

		node->av = identifier->av;
		//llvm_value_new_identifier(ctx, p, &acai_value);
		//return TRUE;
	}
	else if((lvl = llvm_value_literal_new(ctx, node)) != NULL) {

		node->av = llvm_acai_value_new_alloca_with_type(ctx, "", lvl->acai_type);
		llvm_argument_store(ctx, node, lvl);
	}
	else {

		printf("Unknown llvm node arg=%p type=%d\n", node, node->type);
		return FALSE;
	}
	return TRUE;
}

void llvm_function_call(llvm_ctx *ctx, tree *call) {

	int i, j;
	char *str;
	tree *right, *rvalue;
	struct ast_list *l;
	LLVMValueRef func;
	LLVMValueRef args[3];

	LLVMValueRef valstr, off, atp, argv_array, *argv, arg;
	unsigned int argc, argcap;

	tree *node = AST_CHILD_LEFT(call);

	if(node->type == TOK_IDENTIFIER) {

		str = malloc(strlen(node->v.s) + 16);
		sprintf(str, "_acai_func_%s", node->v.s);

		/* TOOD: lookup function identifier in global scope and get default argument values */

		func = LLVMGetNamedFunction(ctx->module, str);
		if(func == NULL)
			func = LLVMGetNamedFunction(ctx->module, node->v.s);

		i = 0;
		if(func == NULL) {
			str = malloc(16 + strlen(node->v.s));
			sprintf(str, "func-name-%s", node->v.s);

			func = acai_get_func_call();
			args[i] = LLVMBuildGlobalStringPtr(ctx->builder, node->v.s, str);
			i++;
		}

		right = AST_CHILD_RIGHT(call);

		if(right->type == LIST_EXPRESSION) {

			argc = 0;
			argcap = 10;
			argv = calloc(argcap, sizeof(LLVMValueRef));

			AST_LIST_FOREACH(right, l) {

				rvalue = AST_LIST_NODE(l);

				if(llvm_argument(ctx, rvalue) == FALSE) {
					return;
				}

				argv[argc] = AST_ACAI_VALUE(rvalue)->begin;

				argc++;
				if(argc == argcap) {

					argcap += 10;
					argv = reallocarray(argv, argcap, sizeof(LLVMValueRef));
				}
			}

		}

		if(argc < //num of args in func, load default value for rargs)

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

		LLVMBuildCall(ctx->builder, func, args, i, str);
	}
}
