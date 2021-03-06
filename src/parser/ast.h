#include <llvm-c/Core.h>

#define AST_CHILD_LEFT(node) node->v.child[0]
#define AST_CHILD_RIGHT(node) node->v.child[1]

#define AST_OP_LEFT(node) node->v.op.child[0]
#define AST_OP_RIGHT(node) node->v.op.child[1]

#define AST_LIST_FOREACH(node, iter) for(iter = tree_list_get_last(node); iter != NULL; iter = iter->prev)
#define AST_LIST_NODE(list) ((tree*)list->node)

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

struct ast_func {
	void *identifier;
	void *signature;
	void *body;

	LLVMValueRef llvm_func;
};

typedef struct ast_tree {

	int type;
	union {

		int i;
		double f;
		char *s;
		struct ast_list *list;
		struct ast_tree *child[2];
		struct ast_vtype vt;
		struct ast_op op;
		struct ast_func func;
	} v;

	struct ast_tree *parent;

	void *av;				//a struct llvm_acai_value pointer holding parameters/function declaration values
	void *lvl;				//a struct llvm_literal_value for rvalue's
	LLVMTypeRef llvm_type;	//a TYPED_IDENTIFIER llvm_type

	unsigned int flags;

} tree;

tree *tree_new_empty(int type);
tree *tree_new(int type, tree *left, tree *right);
tree *tree_copy(tree *src);

void tree_free(tree *node);

tree *tree_list_new(int type, tree *child);
tree *tree_list_prepend(tree *parent, tree *child);
void tree_list_free(tree *parent);
unsigned int tree_list_length(tree *parent);
struct ast_list *tree_list_get_first(tree *parent);
struct ast_list *tree_list_get_last(tree *parent);

tree *tree_find_ancestor_by_type(tree *node, int type);

#define tree_set_child_left(p, lchild)		p->v.child[0] = lchild; p->v.child[0]->parent = p;
#define tree_set_child_right(p, rchild)		p->v.child[1] = rchild; p->v.child[1]->parent = p;

#define tree_set_parent(child, p)			if(child != NULL) child->parent = p

tree *tree_variable_type_new(int type, int is_array);

tree *tree_func_new(tree *identifier, tree *signature, tree *body);

tree *tree_op_new(int type, int op, int is_assignment, tree *left, tree *right);
void tree_op_free(tree *parent);

char *tree_dotted_identifier_str(tree *dotted_identifier_list);
