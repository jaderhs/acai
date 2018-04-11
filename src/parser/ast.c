#include "ast.h"
#include "../llvm/ctx.h"
#include "parser.h"
#include <stdlib.h>
#include <string.h>

tree *tree_new(int type) {

	tree *node = malloc(sizeof(tree));
	memset(node, 0, sizeof(tree));

	node->type = type;

	return node;
}

void tree_free(tree *node) {

	if(node != NULL)
		free(node);
}

tree *tree_new_with_children(int type, tree *left, tree *right) {

	tree *parent = tree_new(type);
	AST_CHILD_LEFT(parent) = left;
	AST_CHILD_RIGHT(parent) = right;

	return parent;
}

tree *tree_list_new(int type, tree *child) {

	tree *parent = tree_new(type);
	parent->v.list = malloc(sizeof(struct ast_list));

	parent->v.list->next = NULL;
	parent->v.list->prev = NULL;
	parent->v.list->node = child;

	return parent;
}

tree *tree_list_prepend(tree *parent, tree *child) {

	struct ast_list *list = malloc(sizeof(struct ast_list));

	list->prev = NULL;
	list->node = child;
	list->next = parent->v.list;

	list->next->prev = list;

	parent->v.list = list;

	return parent;
}

void tree_list_free(tree *parent) {

	struct ast_list *list, *next;

	for(list = parent->v.list; list != NULL; list = next) {
		tree_free(list->node);

		next = list->next;
		free(list);
	}

	//tree_free(parent);
}

struct ast_list *tree_list_get_first(tree *parent) {

	return parent->v.list;
}

struct ast_list *tree_list_get_last(tree *parent) {

	struct ast_list *l = parent->v.list;

	while(l != NULL && l->next != NULL)
		l = l->next;

	return l;
}

tree *tree_variable_type_new(int type, int is_array) {

	tree *parent = tree_new(type);

	parent->type = TOK_TYPENAME;

	parent->v.vt.vtype = type;
	parent->v.vt.is_array = is_array;

	return parent;
}

tree *tree_op_new(int type, int op, int is_assignment, tree *left, tree *right) {

	tree *parent = tree_new(type);

	parent->v.op.op = op;
	parent->v.op.is_assignment = is_assignment;

	AST_OP_LEFT(parent) = left;
	AST_OP_RIGHT(parent) = right;

	return parent;
}

void tree_op_free(tree *parent) {

	tree_free(AST_OP_LEFT(parent));
	tree_free(AST_OP_RIGHT(parent));
}
