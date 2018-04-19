%error-verbose
%parse-param {llvm_ctx *ctx}
%{
  #include <stdio.h>
  #include "../eval/eval.h"
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
%token LIT_NULL

%token <i> TOK_TYPENAME
%token <s> TOK_IDENTIFIER

%token TOK_OP_ASSIGNDECLARE TOK_VARARGS TOK_ARRAY
%token OP_UNARY OP_BINARY

%token DECL_VAR DECL_CONST DECL_PARAM DECL_FUNC FUNC_SIG FUNC_CALL BLOCK
%token KEYWORD_IMPORT KEYWORD_AS KEYWORD_CONST KEYWORD_FUNC KEYWORD_RETURN
%token LIST_PARAMETER FUNC_RETURN_PARAMETER_LIST FUNC_RETURN_PARAM_TYPE DECL_RETURN
%token LIST_STATEMENT LIST_IDENTIFIER LIST_EXPRESSION LIST_TYPED_IDENTIFIER LIST_IDENTIFIER_DOTTED
%token TYPED_IDENTIFIER

%type <t> content import statement_list statement statement_spec simple_statement
%type <t> declaration_variable declaration_variable_value declaration_const declaration_func
%type <t> func_signature func_parameters parameter_list parameter func_return func_return_parameter_list func_return_param
%type <t> return block
%type <t> typed_identifier_list type identifier typed_identifier identifier_list dotted_identifier_list
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
	KEYWORD_IMPORT literal_string						{ $$ = tree_new(KEYWORD_IMPORT, $2, NULL); }
|	KEYWORD_IMPORT literal_string KEYWORD_AS identifier	{ $$ = tree_new(KEYWORD_IMPORT, $2, $4); }
;

statement_list:
	statement
|	statement_list statement	{ $$ = tree_list_prepend($1, $2); }
;

statement:
	statement_spec		{ $$ = tree_list_new(LIST_STATEMENT, $1); }
|	statement_spec ';'	{ $$ = tree_list_new(LIST_STATEMENT, $1); }
;

statement_spec:
	simple_statement
|	declaration_const
|	declaration_func
|	return
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
	typed_identifier_list '=' expression_list				{ $$ = tree_new(DECL_VAR, tree_op_new(OP_BINARY, '=', TRUE, $1, $3), NULL); }
|	identifier_list TOK_OP_ASSIGNDECLARE expression_list	{ $$ = tree_new(DECL_VAR, tree_op_new(OP_BINARY, TOK_OP_ASSIGNDECLARE, TRUE, $1, $3), NULL); }
;

declaration_const:
	KEYWORD_CONST declaration_variable_value				{  $$ = tree_new(DECL_CONST, $2, NULL); }
;

declaration_func:
	KEYWORD_FUNC dotted_identifier_list func_signature block	{ $$ = tree_func_new($2, $3, $4); }
;

func_signature:
	func_parameters					{ $$ = tree_new(FUNC_SIG, $1, NULL); }
|	func_parameters	func_return		{ $$ = tree_new(FUNC_SIG, $1, $2); }
;

func_parameters:
	'(' ')'							{ $$ = tree_new_empty(LIST_PARAMETER); }
|	'(' parameter_list ')'			{ $$ = $2; }
;

parameter_list:
	parameter						{ $$ = tree_list_new(LIST_PARAMETER, $1); }
|	parameter_list ',' parameter	{ $$ = tree_list_prepend($1, $3); }
;

parameter:
	typed_identifier					{ $$ = tree_new(DECL_PARAM, $1, NULL); }
|	typed_identifier '=' expression		{ $$ = tree_new(DECL_PARAM, $1, $3); }
|	TOK_VARARGS							{ $$ = tree_new_empty(TOK_VARARGS); }
;

func_return:
	'(' ')'									{ $$ = tree_new(DECL_RETURN, NULL, NULL); }
|	type									{ $$ = tree_new(DECL_RETURN, $1, NULL); }
|	'(' func_return_parameter_list ')'		{ $$ = tree_new(DECL_RETURN, $2, NULL); }
;

func_return_parameter_list:
	func_return_param									{ $$ = tree_list_new(FUNC_RETURN_PARAMETER_LIST, $1); }
|	func_return_parameter_list ',' func_return_param	{ $$ = tree_list_prepend($1, $3); }
;

func_return_param:
	type									{ $$ = tree_new(FUNC_RETURN_PARAM_TYPE, $1, NULL); }
|	typed_identifier
|	typed_identifier '=' expression			{ $$ = tree_new(DECL_VAR, tree_op_new(OP_BINARY, '=', TRUE, $1, $3), NULL);}
;

return:
	KEYWORD_RETURN					{ $$ = tree_new_empty(KEYWORD_RETURN); }
|	KEYWORD_RETURN expression_list	{ $$ = tree_new(KEYWORD_RETURN, $2, NULL); }
;

block:
	'{' '}'					{ $$ = tree_new(BLOCK, NULL, NULL); }
|	'{' statement_list '}'	{ $$ = tree_new(BLOCK, $2, NULL); }
;


typed_identifier_list:
	type identifier_list									{ $$ = tree_new(LIST_TYPED_IDENTIFIER, $1, $2); }
;

typed_identifier:
	type identifier											{ $$ = tree_new(TYPED_IDENTIFIER, $1, $2); }
;

type:
	TOK_TYPENAME	{ $$ = tree_variable_type_new($1, FALSE); }
|	type TOK_ARRAY	{ $1->v.vt.is_array = TRUE; $$ = $1; }
;

dotted_identifier_list:
	identifier												{ $$ = tree_list_new(LIST_IDENTIFIER_DOTTED, $1); }
|	dotted_identifier_list '.' identifier					{ $$ = tree_list_prepend($1, $3); }
;

identifier_list:
	identifier						{ $$ = tree_list_new(LIST_IDENTIFIER, $1); }
|	identifier_list ',' identifier	{ $$ = tree_list_prepend($1, $3); }
;

identifier:
	TOK_IDENTIFIER					{ $$ = tree_new_empty(TOK_IDENTIFIER); $$->v.s = $1; }
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
|	primary_expression '(' ')'					{ $$ = tree_new(FUNC_CALL, $1, NULL); }
|	primary_expression '(' expression_list ')'	{ $$ = tree_new(FUNC_CALL, $1, $3); }
|	TOK_VARARGS									{ $$ = tree_new_empty(TOK_VARARGS); }
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
|	LIT_NULL		{ $$ = tree_new_empty(LIT_NULL); }
;

literal_float:
	LIT_FLOAT		{ $$ = tree_new_empty(LIT_FLOAT); $$->v.f = $1; }
;

literal_integer:
	LIT_INTEGER		{ $$ = tree_new_empty(LIT_INTEGER); $$->v.i = $1; }
;

literal_string:
	LIT_STRING		{ $$ = tree_new_empty(LIT_STRING); $$->v.s = $1; }
;

literal_boolean:
	LIT_BOOLEAN		{ $$ = tree_new_empty(LIT_BOOLEAN); $$->v.i = $1; }
;
