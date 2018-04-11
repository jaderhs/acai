#include <llvm-c/Core.h>

typedef struct _llvm_identifier_list {

	tree *identifier;
	struct _llvm_identifier_list *next;

} llvm_identifier_list;

typedef struct _llvm_scope_identifier_list {

	llvm_identifier_list *identifiers;
	struct _llvm_scope_identifier_list *next;
	
} llvm_scope_identifier_list;

llvm_scope_identifier_list *llvm_scope_identifier_list_prepend_new(llvm_scope_identifier_list *prev);
void llvm_scope_identifier_list_free(llvm_scope_identifier_list *list);

llvm_identifier_list *llvm_identifier_list_prepend(llvm_identifier_list *prev, tree *identifier);
void llvm_identifier_list_free(llvm_identifier_list *list);

tree *llvm_identifier_list_lookup_by_name(llvm_identifier_list *list, char *name);
