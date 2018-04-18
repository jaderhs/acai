#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "parser/ast.h"
#include "ctx.h"
#include "parser/parser.h"

llvm_scope_identifier_list *llvm_scope_push_new(llvm_scope_identifier_list *prev) {

	llvm_scope_identifier_list *list = malloc(sizeof(llvm_scope_identifier_list));

	list->identifiers = NULL;
	list->next = prev;

	return list;
}

llvm_scope_identifier_list *llvm_scope_pop_free(llvm_scope_identifier_list *curr) {

	llvm_scope_identifier_list *next = curr->next;
	free(curr);

	return next;
}




void llvm_scope_identifier_list_free(llvm_scope_identifier_list *list) {
	free(list);
}

llvm_identifier_list *llvm_identifier_list_prepend(llvm_identifier_list *prev, tree *identifier) {

	llvm_identifier_list *list = malloc(sizeof(llvm_identifier_list));

	list->identifier = identifier;
	list->next = prev;

	return list;
}

void llvm_identifier_list_free(llvm_identifier_list *list) {
	free(list);
}

tree *llvm_identifier_list_lookup_by_name(llvm_identifier_list *list, char *name) {

	tree *identifier;

	for(; list != NULL; list = list->next) {

		if(list->identifier->type == TYPED_IDENTIFIER) {

			identifier = AST_CHILD_RIGHT(list->identifier);

			if(strcmp(identifier->v.s, name)==0)
				return list->identifier;

		} else {
			printf("Unknown identifier type in scope list: %d\n", list->identifier->type);
			return NULL;
		}
	}

	return NULL;
}