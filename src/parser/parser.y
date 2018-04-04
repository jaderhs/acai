%error-verbose
%{
  #include <stdio.h>
  #include "ast.h"
  int yylex (void);

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
	char *s;
	tree *t;
}

%token <i> LIT_INTEGER
%token <f> LIT_FLOAT
%token <s> LIT_STRING

%token <i> TOK_TYPENAME
%token <s> TOK_IDENTIFIER

%token KEYWORD_IMPORT KEYWORD_AS
%token LIST_STATEMENT LIST_IDENTIFIER

%type <t> content import statement statement_spec simple_statement
%type <t> declaration_variable typed_identifier_list type identifier identifier_list
%type <t> expression unary_expression primary_expression operand literal
%type <t> literal_float literal_integer literal_string


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
	KEYWORD_IMPORT literal_string						{ $$ = tree_new_with_children(KEYWORD_IMPORT, $2, NULL); }
|	KEYWORD_IMPORT literal_string KEYWORD_AS identifier	{ $$ = tree_new_with_children(KEYWORD_IMPORT, $2, $4); }
;

statement:
	statement_spec		{ $$ = tree_list_new(LIST_STATEMENT, $1); }
|	statement_spec ';'	{ $$ = tree_list_new(LIST_STATEMENT, $1); }
;

statement_spec:
	simple_statement
;

simple_statement:
	declaration_variable
|	expression
;

declaration_variable:
	typed_identifier_list
;

typed_identifier_list:
	type identifier_list
;

type:
	TOK_TYPENAME	{ $$ = tree_variable_type_new($1, FALSE); }
;

identifier_list:
	identifier						{ $$ = tree_list_new(LIST_IDENTIFIER, $1); }
|	identifier_list ',' identifier	{ $$ = tree_list_prepend($1, $3); }
;

identifier:
	TOK_IDENTIFIER	{ $$ = tree_new(TOK_IDENTIFIER); $$->v.s = $1; }
;

expression:
	unary_expression
;

unary_expression:
	primary_expression
;

primary_expression:
	operand
;

operand:
	literal
|	identifier
;

literal:
	literal_float
|	literal_integer
|	literal_string
;

literal_float:
	LIT_FLOAT		{ $$ = tree_new(LIT_FLOAT); $$->v.f = $1; }
;

literal_integer:
	LIT_INTEGER		{ $$ = tree_new(LIT_INTEGER); $$->v.i = $1; }
;

literal_string:
	LIT_STRING		{ $$ = tree_new(LIT_STRING); $$->v.s = $1; }
;
