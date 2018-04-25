#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ctx.h"
#include "parser/parser.h"

llvm_identifier_list *llvm_identifier_list_prepend(llvm_identifier_list *prev, tree *identifier) {

	llvm_identifier_list *list = malloc(sizeof(llvm_identifier_list));

	list->identifier = identifier;
	list->next = prev;

	return list;
}

void llvm_identifier_list_free(llvm_identifier_list *list) {
	free(list);
}

void llvm_identifier_list_free_all(llvm_identifier_list *list) {

	llvm_identifier_list *next;
	for(next = list->next; next != NULL; next = list->next) {

		if(list->identifier)
			tree_free(list->identifier);

		llvm_identifier_list_free(list);
		list = next;
	}
}

tree *llvm_identifier_list_lookup_by_name(llvm_identifier_list *list, char *name) {

	tree *identifier;

	for(; list != NULL; list = list->next) {

		identifier = NULL;
		if(list->identifier->type == DECL_FUNC) {
			identifier = list->identifier->v.func.identifier;
		}
		else if(list->identifier->type == TYPED_IDENTIFIER) {

			identifier = AST_CHILD_RIGHT(list->identifier);

		} else {

			printf("Unknown identifier type in scope list: %d\n", list->identifier->type);
			return NULL;
		}

		if(strcmp(identifier->v.s, name)==0)
			return identifier;
	}

	return NULL;
}