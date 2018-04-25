#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ctx.h"
#include "parser/parser.h"

llvm_symbol_list *llvm_symbol_list_prepend(llvm_symbol_list *prev, tree *symbol) {

	llvm_symbol_list *list = malloc(sizeof(llvm_symbol_list));

	list->symbol = symbol;
	list->next = prev;

	return list;
}

void llvm_symbol_list_free(llvm_symbol_list *list) {
	free(list);
}

void llvm_symbol_list_free_all(llvm_symbol_list *list) {

	llvm_symbol_list *next;
	for(next = list->next; next != NULL; next = list->next) {

		if(list->symbol)
			tree_free(list->symbol);

		llvm_symbol_list_free(list);
		list = next;
	}
}

tree *llvm_symbol_list_lookup_by_name(llvm_symbol_list *list, char *name) {

	int cmp;
	char *symbol_name;

	for(; list != NULL; list = list->next) {

		switch(list->symbol->type) {

			case DECL_FUNC:

				symbol_name = tree_dotted_identifier_str(list->symbol->v.func.identifier);
				break;

			case DECL_PARAM:

				symbol_name = AST_CHILD_RIGHT(AST_CHILD_LEFT(list->symbol))->v.s;
				break;

			case TYPED_IDENTIFIER:

				symbol_name = AST_CHILD_RIGHT(list->symbol)->v.s;
				break;

			default:
				printf("Unknown symbol type in scope list: %d\n", list->symbol->type);
				return NULL;
		}

		cmp = strcmp(symbol_name, name);

		if(list->symbol->type == DECL_FUNC)
			free(symbol_name);

		if(cmp==0)
			return list->symbol;
	}

	return NULL;
}