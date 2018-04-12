%error-verbose
%parse-param {llvm_ctx *ctx}
%{
  #include <stdio.h>
  #include "eval.h"
  #include "../util.h"
  int yylex (void);

/* Called by yyparse on error.  */
extern int yylineno;

void
yyerror (llvm_ctx *ctx, char const *s)
{
	fprintf (stderr, "Error at line %d: %s\n", yylineno, s);
}
%}

%union {
	double f;
	int i;
	char *s;
	tree *t;
}

%token <i> LIT_INTEGER
%token <f> LIT_FLOAT
%token <s> LIT_STRING
%token <i> LIT_BOOLEAN

%token <i> TOK_TYPENAME
%token <s> TOK_IDENTIFIER

%token TOK_OP_ASSIGNDECLARE
%token OP_UNARY OP_BINARY

%token DECL_VAR DECL_CONST FUNC_CALL
%token KEYWORD_IMPORT KEYWORD_AS KEYWORD_CONST
%token LIST_STATEMENT LIST_IDENTIFIER LIST_EXPRESSION LIST_TYPED_IDENTIFIER
%token TYPED_IDENTIFIER

%type <t> content import statement statement_spec simple_statement
%type <t> declaration_variable declaration_variable_value declaration_const
%type <t> typed_identifier_list type identifier typed_identifier identifier_list
%type <t> expression expression_list unary_expression primary_expression operand literal
%type <t> literal_float literal_integer literal_string literal_boolean


%%

prog:
	%empty
|	prog content	{ eval(ctx, $2, 0); tree_free($2); }
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
|	declaration_const
;

simple_statement:
	declaration_variable
|	expression
;

declaration_variable:
	typed_identifier_list
|	declaration_variable_value
;

declaration_variable_value:
	typed_identifier_list '=' expression_list				{ $$ = tree_new_with_children(DECL_VAR, tree_op_new(OP_BINARY, '=', TRUE, $1, $3), NULL); }
|	identifier_list TOK_OP_ASSIGNDECLARE expression_list	{ $$ = tree_new_with_children(DECL_VAR, tree_op_new(OP_BINARY, TOK_OP_ASSIGNDECLARE, TRUE, $1, $3), NULL); }
;

declaration_const:
	KEYWORD_CONST declaration_variable_value				{  $$ = tree_new_with_children(DECL_CONST, $2, NULL); }
;

typed_identifier_list:
	type identifier_list									{ $$ = tree_new_with_children(LIST_TYPED_IDENTIFIER, $1, $2); }
;

typed_identifier:
	type identifier											{ $$ = tree_new_with_children(TYPED_IDENTIFIER, $1, $2); }
;

type:
	TOK_TYPENAME	{ $$ = tree_variable_type_new($1, FALSE); }
;

identifier_list:
	identifier						{ $$ = tree_list_new(LIST_IDENTIFIER, $1); }
|	identifier_list ',' identifier	{ $$ = tree_list_prepend($1, $3); }
;

identifier:
	TOK_IDENTIFIER					{ $$ = tree_new(TOK_IDENTIFIER); $$->v.s = $1; }
;

expression_list:
	expression						{ $$ = tree_list_new(LIST_EXPRESSION, $1); }
|	expression_list ',' expression	{ $$ = tree_list_prepend($1, $3); }
;

expression:
	unary_expression
;

unary_expression:
	primary_expression
;

primary_expression:
	operand
|	primary_expression '(' ')'					{ $$ = tree_new_with_children(FUNC_CALL, $1, NULL); }
|	primary_expression '(' expression_list ')'	{ $$ = tree_new_with_children(FUNC_CALL, $1, $3); }
;

operand:
	literal
|	identifier
;

literal:
	literal_float
|	literal_integer
|	literal_string
|	literal_boolean
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

literal_boolean:
	LIT_BOOLEAN		{ $$ = tree_new(LIT_BOOLEAN); $$->v.i = $1; }
;
