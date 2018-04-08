typedef struct {

	unsigned int cap;
	char *str;

} lex_str;

#define lex_str_str(ls) ls.str

void lex_str_init(lex_str *ls);
void lex_str_truncate(lex_str *ls);
void lex_str_append(lex_str *ls, char *str);
void lex_str_append_c(lex_str *ls, char c);
