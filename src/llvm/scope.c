#include <stdlib.h>
#include <string.h>
#include "scope.h"

llvm_scope *llvm_scope_push_new(llvm_scope *prev) {

	llvm_scope *scope = malloc(sizeof(llvm_scope));

	memset(scope, 0, sizeof(llvm_scope));

	scope->next = prev;
	return scope;
}

llvm_scope *llvm_scope_pop_free(llvm_scope *curr) {

	llvm_scope *next = curr->next;

	llvm_symbol_list_free_all(curr->symbols);

	free(curr);
	return next;
}

