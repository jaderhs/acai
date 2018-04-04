#include "ast.h"
#include "parser.h"
#include <stdlib.h>

tree *tree_new(int type) {

	tree *node = malloc(sizeof(tree));

	node->type = type;

	return node;
}

void tree_free(tree *node) {
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

tree *tree_variable_type_new(int type, int is_array) {

	tree *parent = tree_new(type);

	parent->type = TOK_TYPENAME;

	parent->v.vt.vtype = type;
	parent->v.vt.is_array = is_array;

	return parent;
}
