%error-verbose
%{
  #include <stdio.h>
  #include "ast.h"

/* Called by yyparse on error.  */
extern int yylineno;

void
yyerror (char const *s)
{
	fprintf (stderr, "Error at line %d: %s\n", yylineno, s);
}
%}

%union {
	float f;
	int i;
	tree *t;
}

%token <i> LIT_INTEGER
%token <f> LIT_FLOAT

%token KEYWORD_IMPORT KEYWORD_AS
%token TOK_IDENTIFIER TOK_TYPENAME

%type <t> content

%%

prog:
	%empty
|	prog content		{}
;

content:
	import
|	statement
;

import:
	KEYWORD_IMPORT literal_string						{ $$ = ast_node_new(KEYWORD_IMPORT, $2, NULL); }
|	KEYWORD_IMPORT literal_string KEYWORD_AS identifier	{ $$ = ast_node_new(KEYWORD_IMPORT, $2, $4); }
;

statement:
	statement_spec		{ $$ = ast_list_new(STATEMENT_LIST, $1); }
|	statement_spec ';'	{ $$ = ast_list_new(STATEMENT_LIST, $1); }
;

statement_spec:
	simple_statement
;

simple_statement:
	declaration_variable
;

declaration_variable:
	typed_identifier_list
;

typed_identifier_list:
	type identifier_list
;

type:
	TOK_TYPENAME
;

identifier_list:
	identifier
|	identifier_list ',' identifier
;

identifier:
	TOK_IDENTIFIER
;

literal_float:
	LIT_FLOAT		{ $$ = ast_lit_float_new($1); }
;

literal_integer:
	LIT_INTEGER		{ $$ = ast_lit_int_new($1); }
;

literal_string:
	LIT_STRING		{ $$ = ast_new(LIT_STRING); $$->v.str = $1; }
;
