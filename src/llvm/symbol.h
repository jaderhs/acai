#include <llvm-c/Core.h>
#include "parser/ast.h"

typedef struct _llvm_symbol_list {

	tree *symbol;
	struct _llvm_symbol_list *next;

} llvm_symbol_list;

llvm_symbol_list *llvm_symbol_list_prepend(llvm_symbol_list *prev, tree *symbol);
void llvm_symbol_list_free(llvm_symbol_list *list);
void llvm_symbol_list_free_all(llvm_symbol_list *list);

tree *llvm_symbol_list_lookup_by_name(llvm_symbol_list *list, char *name);
