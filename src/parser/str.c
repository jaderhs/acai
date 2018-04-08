#include <stdlib.h>
#include <string.h>
#include "str.h"

void lex_str_init(lex_str *ls) {

	ls->cap = 0;
	ls->str = NULL;
}

void lex_str_truncate(lex_str *ls) {

	ls->cap = 0;
	if(ls->str)
		free(ls->str);

	ls->str = NULL;
}

void lex_str_append_with_len(lex_str *ls, char *src, int len) {

	int curlen;

	if(ls->str == NULL) {

		ls->cap += len + 1024;
		ls->str = malloc(ls->cap);

		memcpy(ls->str, src, len);
		ls->str[len] = '\0';

	}
	else {

		curlen = strlen(ls->str);

		if(ls->cap < curlen + len+1) {

			ls->cap += len + 1024;
			ls->str = realloc(ls->str, ls->cap);
		}

		memcpy(&ls->str[curlen], src, len);
		ls->str[curlen + len] = '\0';
	}
}

void lex_str_append(lex_str *ls, char *src) {
	lex_str_append_with_len(ls, src, strlen(src));
}

void lex_str_append_c(lex_str *ls, char c) {
	lex_str_append_with_len(ls, &c, 1);
}
