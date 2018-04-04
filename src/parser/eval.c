#include <stdio.h>
#include "eval.h"
#include "parser.h"

void eval(tree *node) {

	switch(node->type) {

		case DECL_VAR:
			break;

		case FUNC_CALL:
			break;

		default:
			printf("Tree type %d not executed by eval()\n", node->type);
			break;
	}

}
