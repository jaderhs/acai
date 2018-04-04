%option noyywrap yylineno
%{
#include "ast.h"
#include "parser.h"
%}

DIGIT			[0-9]
IDENTIFIER		[[:alpha:]_][[:alnum:]_]*

SINGLE_TOKEN	[\(\)=]

%%

-?{DIGIT}+				{ return LIT_INTEGER; }
-?{DIGIT}+"."{DIGIT}*	{ return LIT_INTEGER; }

{IDENTIFIER}			{ return TOK_IDENTIFIER; }

{SINGLE_TOKEN}			{ return yytext[0]; }

	/* operators */

":="					{ return TOK_OP_ASSIGNDECLARE; }


[\t\r\n ]*				/* skip whitespaces */
