#include "symbol.h"

typedef struct _llvm_scope {

	llvm_symbol_list *symbols;

	struct _llvm_scope *next;

} llvm_scope;

llvm_scope *llvm_scope_push_new(llvm_scope *prev);
llvm_scope *llvm_scope_pop_free(llvm_scope *curr);
