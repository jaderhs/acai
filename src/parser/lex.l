%option noyywrap yylineno
%{
#include "str.h"
#include "llvm/ctx.h"
#include "parser.h"
#include "acai/type.h"
%}

%x	string comment

DIGIT			[0-9]
IDENTIFIER		[[:alpha:]_][[:alnum:]_]*

SINGLE_TOKEN	[\(\)\{\}\[\]=,]

%%
	lex_str str;
	lex_str_init(&str);

-?{DIGIT}+					{ yylval.i = strtol(yytext, NULL, 10); return LIT_INTEGER; }
-?{DIGIT}+"."{DIGIT}*		{ yylval.f = atof(yytext); return LIT_FLOAT; }
-?"."{DIGIT}+				{ yylval.f = atof(yytext); return LIT_FLOAT; }
-?{DIGIT}+"e"[-+]?{DIGIT}+	{ yylval.f = atof(yytext); return LIT_FLOAT; }

"0x"[0-9a-fA-F]+			{ yylval.i = strtol(&yytext[2], NULL, 16); return LIT_INTEGER; }

"true"						{ yylval.i = 1; return LIT_BOOLEAN; }
"false"						{ yylval.i = 0; return LIT_BOOLEAN; }

"null"						{ return LIT_NULL; }

	/* operators */

":="						{ return TOK_OP_ASSIGNDECLARE; }

	/* types */

"const"						{ return KEYWORD_CONST; }

"int"						{ yylval.i = AT_INT64; return TOK_TYPENAME; }
"int8"						{ yylval.i = AT_INT8; return TOK_TYPENAME; }
"int16"						{ yylval.i = AT_INT16; return TOK_TYPENAME; }
"int32"						{ yylval.i = AT_INT32; return TOK_TYPENAME; }
"int64"						{ yylval.i = AT_INT64; return TOK_TYPENAME; }
"int128"					{ yylval.i = AT_INT128; return TOK_TYPENAME; }

"uint"						{ yylval.i = AT_UINT64; return TOK_TYPENAME; }
"uint8"						{ yylval.i = AT_UINT8; return TOK_TYPENAME; }
"uint16"					{ yylval.i = AT_UINT16; return TOK_TYPENAME; }
"uint32"					{ yylval.i = AT_UINT32; return TOK_TYPENAME; }
"uint64"					{ yylval.i = AT_UINT64; return TOK_TYPENAME; }
"uint128"					{ yylval.i = AT_UINT128; return TOK_TYPENAME; }

"float"						{ yylval.i = AT_FLOAT32; return TOK_TYPENAME; }
"float32"					{ yylval.i = AT_FLOAT32; return TOK_TYPENAME; }
"float64"					{ yylval.i = AT_FLOAT64; return TOK_TYPENAME; }
"float80"					{ yylval.i = AT_FLOAT80; return TOK_TYPENAME; }

"byte"						{ yylval.i = AT_UINT8; return TOK_TYPENAME; }

"string"					{ yylval.i = AT_STRING; return TOK_TYPENAME; }

"[]"						{ return TOK_ARRAY; }

"..."						{ return TOK_VARARGS; }

"func"						{ return KEYWORD_FUNC; }
"return"					{ return KEYWORD_RETURN; }

{SINGLE_TOKEN}				{ return yytext[0]; }

{IDENTIFIER}				{ yylval.s = strdup(yytext); return TOK_IDENTIFIER; }

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


"//"[^\n]*					/* skip one-line comments */

"/*"					BEGIN(comment);
<comment>[^*]*			/* skip everything that's not a '*' */
<comment>"*"+[^*/\n]*	/* skip '*' not followed by '/' */
<comment>"*/"			BEGIN(0);


[\t\r\n ]*				/* skip whitespaces */
