#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include "llvm/ctx.h"
#include "llvm/init.h"
#include "parser/ast.h"
#include "parser/parser.h"

extern FILE *yyin;

void usage(char *argv0) {
	printf("Usage: %s input files\n", argv0);
}

int main(int argc, char *argv[]) {

	int opt, ret;
	llvm_ctx ctx;
#ifdef COMPILER
	char *output = "a.out";
#endif


	memset(&ctx, 0, sizeof(ctx));

#ifdef COMPILER
	while((opt = getopt(argc, argv, "h?o:")) != -1) {
#else
	while((opt = getopt(argc, argv, "h?")) != -1) {
#endif

		switch(opt) {
#ifdef COMPILER
			case 'o':
				break;
#endif
			case 'h':
			default:
				usage(argv[0]);
				return 1;
		}
	}

	llvm_init(&ctx);

	if(argc < 2) {
		fprintf(stderr, "%s: error: no input files\n", argv[0]);
		return 2;
	}

	//create a module from the first file
	llvm_create_module(argv[1], &ctx);
	llvm_create_main(&ctx);

	// parse just one file for now
	if(strcmp(argv[1], "-")==0)
		yyin = stdin;
	else
		yyin = fopen(argv[1], "r");

	ret = yyparse(&ctx);

	fclose(yyin);

	LLVMDumpModule(ctx.module);

	llvm_finish(&ctx);

	return ret;
}
