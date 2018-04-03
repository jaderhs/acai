typedef struct ast_tree {

	int type;
	union {

		int i;
		float f;
		char *str;
	} v;

} tree;
