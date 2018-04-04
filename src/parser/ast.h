#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE ~0
#endif

#define AST_CHILD_LEFT(node) node->v.child[0]
#define AST_CHILD_RIGHT(node) node->v.child[1]

struct ast_list {

	struct ast_list *next;
	struct ast_list *prev;
	void *node; // a tree
};

struct ast_vtype {
	int vtype;
	int is_array;
	void *content; // a tree
};

struct ast_op {
	int op;
	int is_assignment;
	void *child[2]; // tree
};

typedef struct ast_tree {

	int type;
	union {

		int i;
		float f;
		char *s;
		struct ast_list *list;
		struct ast_tree *child[2];
		struct ast_vtype vt;
		struct ast_op op;
	} v;

} tree;

tree *tree_new(int type);
tree *tree_new_with_children(int type, tree *left, tree *right);

void tree_free(tree *node);

tree *tree_list_new(int type, tree *child);
tree *tree_list_prepend(tree *parent, tree *child);
void tree_list_free(tree *parent);

tree *tree_variable_type_new(int type, int is_array);

tree *tree_op_new(int type, int op, int is_assignment, tree *left, tree *right);
void tree_op_free(tree *parent);
