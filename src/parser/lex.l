%option noyywrap yylineno
%{
#include "ast.h"
#include "str.h"
#include "../llvm/ctx.h"
#include "parser.h"
%}

%x	string

DIGIT			[0-9]
IDENTIFIER		[[:alpha:]_][[:alnum:]_]*

SINGLE_TOKEN	[\(\)=,]

%%
	lex_str str;
	lex_str_init(&str);

-?{DIGIT}+					{ yylval.i = strtol(yytext, NULL, 10); return LIT_INTEGER; }
-?{DIGIT}+"."{DIGIT}*		{ yylval.f = atof(yytext); return LIT_FLOAT; }
-?"."{DIGIT}+				{ yylval.f = atof(yytext); return LIT_FLOAT; }
-?{DIGIT}+"e"[-+]?{DIGIT}+	{ yylval.f = atof(yytext); return LIT_FLOAT; }

"true"						{ yylval.i = 1; return LIT_BOOLEAN; }
"false"						{ yylval.i = 0; return LIT_BOOLEAN; }

{IDENTIFIER}				{ yylval.s = strdup(yytext); return TOK_IDENTIFIER; }

{SINGLE_TOKEN}				{ return yytext[0]; }

	/* operators */

":="						{ return TOK_OP_ASSIGNDECLARE; }

"\""						{ lex_str_truncate(&str); BEGIN(string); }
<string>\\n					{ lex_str_append_c(&str, '\n'); }
<string>\\t					{ lex_str_append_c(&str, '\t'); }
<string>\\r					{ lex_str_append_c(&str, '\r'); }
<string>\\b					{ lex_str_append_c(&str, '\b'); }
<string>\\f					{ lex_str_append_c(&str, '\f'); }
<string>\\0					{ lex_str_append_c(&str, '\0'); }
<string>\\\\				{ lex_str_append_c(&str, '\\'); }
<string>\\\"				{ lex_str_append_c(&str, '"'); }

<string>\\[0-9]{1,3}	{
							unsigned int result;
							int res = sscanf(yytext + 1, "%o", &result);
							if(res == EOF) {
								fprintf(stderr, "Error octal '%s' not terminated on line %d\n", yytext, yylineno);
								yyterminate();
							}
							else if(res == 0 || result > 0xFF) {
								fprintf(stderr, "Invalid octal '%s' on line %d\n", yytext, yylineno);
								yyterminate();
							}

							lex_str_append_c(&str, (unsigned char) result);
						}

<string>\\[0-9]+		{
							fprintf(stderr, "Invalid escape sequence '%s' on line %d\n", yytext, yylineno);
							yyterminate();
						}

<string>\\x[0-9a-fA-F]{1,2}	{
							unsigned int result;
							int res = sscanf(yytext + 2, "%x", &result);
							if(res == EOF) {
								fprintf(stderr, "Hex '%s' not terminated on line %d\n", yytext, yylineno);
								yyterminate();
							}
							else if(res == 0 || result > 0xFF) {
								fprintf(stderr, "Invalid hex '%s' on line %d\n", yytext, yylineno);
								yyterminate();
							}

							lex_str_append_c(&str, (unsigned char) result);
						}

<string>\n				{ lex_str_append_c(&str, '\n'); }
<string>[^\\"\n]*		{ lex_str_append(&str, yytext); }
<string>"\""			{ BEGIN(0); yylval.s = lex_str_str(str); return LIT_STRING; }


[\t\r\n ]*				/* skip whitespaces */
