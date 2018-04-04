%option noyywrap yylineno
%{
#include "ast.h"
#include "parser.h"
%}

DIGIT			[0-9]
IDENTIFIER		[[:alpha:]_][[:alnum:]_]*

SINGLE_TOKEN	[\(\)]

%%

-?{DIGIT}+				{ return LIT_INTEGER; }
-?{DIGIT}+"."{DIGIT}*	{ return LIT_INTEGER; }

{IDENTIFIER}			{ return TOK_IDENTIFIER; }

[\t\r\n ]*				/* skip whitespaces */
