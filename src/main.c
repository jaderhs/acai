#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include "llvm/ctx.h"
#include "llvm/init.h"

extern FILE *yyin;

void usage(char *argv0) {
	printf("Usage: %s input files\n", argv0);
}

int main(int argc, char *argv[]) {

	int opt;
	llvm_ctx ctx;

	memset(&ctx, 0, sizeof(ctx));

	while((opt = getopt(argc, argv, "h?")) != -1) {

		switch(opt) {
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

	LLVMDumpModule(ctx.module);

	llvm_finish(&ctx);

	return 0;
}
