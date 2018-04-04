%option noyywrap yylineno
%{
#include "ast.h"
#include "../llvm/ctx.h"
#include "parser.h"
%}

DIGIT			[0-9]
IDENTIFIER		[[:alpha:]_][[:alnum:]_]*

SINGLE_TOKEN	[\(\)=]

%%

-?{DIGIT}+				{ yylval.i = strtol(yytext, NULL, 10); return LIT_INTEGER; }
-?{DIGIT}+"."{DIGIT}*	{ yylval.f = atof(yytext); return LIT_FLOAT; }

{IDENTIFIER}			{ yylval.s = strdup(yytext); return TOK_IDENTIFIER; }

{SINGLE_TOKEN}			{ return yytext[0]; }

	/* operators */

":="					{ return TOK_OP_ASSIGNDECLARE; }


[\t\r\n ]*				/* skip whitespaces */
