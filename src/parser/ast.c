#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "llvm/ctx.h"
#include "parser.h"

tree *tree_new_empty(int type) {

	tree *node = malloc(sizeof(tree));
	memset(node, 0, sizeof(tree));

	node->type = type;

	return node;
}

void tree_free(tree *node) {

	if(node != NULL)
		free(node);
}

tree *tree_copy(tree *src) {

	tree *dst = tree_new_empty(src->type);
	memcpy(dst, src, sizeof(tree));

	switch(dst->type) {

		case TOK_TYPENAME:
			break;

		case LIT_STRING:
		case TOK_IDENTIFIER:
			dst->v.s = strdup(src->v.s);
			break;

		case TYPED_IDENTIFIER:
			tree_set_child_left(dst, tree_copy(AST_CHILD_LEFT(src)));
			tree_set_child_right(dst, tree_copy(AST_CHILD_RIGHT(src)));
			break;

		default:
			fprintf(stderr, "Unknown tree type in tree_copy(): %d\n", dst->type);
			return NULL;
	}

	return dst;
}

tree *tree_new(int type, tree *left, tree *right) {

	tree *parent = tree_new_empty(type);

	AST_CHILD_LEFT(parent) = left;
	AST_CHILD_RIGHT(parent) = right;

	tree_set_parent(left, parent);
	tree_set_parent(right, parent);

	return parent;
}

tree *tree_list_new(int type, tree *child) {

	tree *parent = tree_new_empty(type);
	parent->v.list = malloc(sizeof(struct ast_list));

	parent->v.list->next = NULL;
	parent->v.list->prev = NULL;
	parent->v.list->node = child;

	tree_set_parent(child, parent);

	return parent;
}

tree *tree_list_prepend(tree *parent, tree *child) {

	struct ast_list *list = malloc(sizeof(struct ast_list));

	list->prev = NULL;
	list->node = child;
	list->next = parent->v.list;

	list->next->prev = list;

	parent->v.list = list;

	tree_set_parent(child, parent);

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

unsigned int tree_list_length(tree *parent) {

	unsigned int i = 0;
	struct ast_list *list;

	for(list = parent->v.list; list != NULL; list = list->next) {
		i++;
	}

	return i;
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

tree *tree_find_ancestor_by_type(tree *node, int type) {

	tree *p;
	for(p = node->parent; p != NULL; p = p->parent) {

		if(p->type == type)
			return p;

	}

	return NULL;
}

tree *tree_variable_type_new(int type, int is_array) {

	tree *parent = tree_new_empty(TOK_TYPENAME);

	parent->v.vt.vtype = type;
	parent->v.vt.is_array = is_array;

	return parent;
}

tree *tree_func_new(tree *identifier, tree *signature, tree *body) {

	tree *parent = tree_new_empty(DECL_FUNC);

	parent->v.func.identifier = identifier;
	parent->v.func.signature = signature;
	parent->v.func.body = body;

	tree_set_parent(body, parent);
	tree_set_parent(identifier, parent);
	tree_set_parent(signature, parent);

	return parent;
}

tree *tree_op_new(int type, int op, int is_assignment, tree *left, tree *right) {

	tree *parent = tree_new_empty(type);

	parent->v.op.op = op;
	parent->v.op.is_assignment = is_assignment;

	AST_OP_LEFT(parent) = left;
	AST_OP_RIGHT(parent) = right;

	tree_set_parent(left, parent);
	tree_set_parent(right, parent);

	return parent;
}

void tree_op_free(tree *parent) {

	tree_free(AST_OP_LEFT(parent));
	tree_free(AST_OP_RIGHT(parent));
}

char *tree_dotted_identifier_str(tree *dotted_identifier_list) {

	int cap = 1024;
	char *name = malloc(cap);

	name[0] = '\0';

	struct ast_list *l;
	AST_LIST_FOREACH(dotted_identifier_list, l) {

		if(strlen(AST_LIST_NODE(l)->v.s) > cap - strlen(name) + 2) {

			cap += strlen(AST_LIST_NODE(l)->v.s) + 1024;
			name = realloc(name, cap);
		}

		if(name[0] != '\0')
			strncat(name, ".", cap-1);

		strncat(name, AST_LIST_NODE(l)->v.s, cap-1);
	}

	return name;
}
