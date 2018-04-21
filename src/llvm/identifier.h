#include <llvm-c/Core.h>
#include "parser/ast.h"

typedef struct _llvm_identifier_list {

	tree *identifier;
	struct _llvm_identifier_list *next;

} llvm_identifier_list;

llvm_identifier_list *llvm_identifier_list_prepend(llvm_identifier_list *prev, tree *identifier);
void llvm_identifier_list_free(llvm_identifier_list *list);

tree *llvm_identifier_list_lookup_by_name(llvm_identifier_list *list, char *name);
