#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include "llvm/ctx.h"
#include "llvm/init.h"
#include "llvm/acai.h"
#include "parser/parser.h"

extern FILE *yyin;

struct option optlong[] = {
	{"help", 0, NULL, 'h'},
	{NULL, 0, NULL, 0}
};

void usage(char *argv0) {
	printf("Usage: %s [options] <input files>\n"
			"Options:\n"
			"  -h, --help   Display available options\n"
#ifdef COMPILER
			"  -o <file>    Write output to <file>\n"
#elif REPL
			"  -v           Print verbose parser output\n"
#endif
			, argv0);
}

int main(int argc, char *argv[]) {

	int opt, ret;
	char *err;
	llvm_ctx ctx;
	LLVMTargetRef target;
#ifdef COMPILER
	char *output = "a.out";
#elif REPL
	int verbose =0 ;
#endif

	memset(&ctx, 0, sizeof(ctx));

#ifdef COMPILER
	while((opt = getopt_long(argc, argv, "h?o:", optlong, NULL)) != -1) {
#elif REPL
	while((opt = getopt_long(argc, argv, "h?v", optlong, NULL)) != -1) {
#endif

		switch(opt) {
#ifdef COMPILER
			case 'o':
				output = optarg;
				break;
#endif
#ifdef REPL
			case 'v':
				verbose = 1;
				break;
#endif
			case 'h':
			default:
				usage(argv[0]);
				return 1;
		}
	}

#ifdef COMPILER
	if(argc < 2) {
		fprintf(stderr, "%s: error: no input files\n", argv[0]);
		return 2;
	}
#endif

	llvm_init(&ctx);

	//create a module from the first file
	llvm_create_module(argv[1], &ctx);

	acai_init(&ctx);

	llvm_create_main(&ctx);

	char *triple = LLVMGetDefaultTargetTriple();
	LLVMSetTarget(ctx.module, triple);

//	LLVMRunPassManager(pm, ctx.module);

	if(LLVMGetTargetFromTriple(triple, &target, &err) != 0) {
		fprintf(stderr, "Error fetching target from triple '%s': %s\n", triple, err);
		return 5;
	}

	LLVMTargetMachineRef targetMachine = LLVMCreateTargetMachine(target, triple, "generic", "", LLVMCodeGenLevelNone, LLVMRelocDefault, LLVMCodeModelDefault);

	LLVMTargetDataRef dl = LLVMCreateTargetDataLayout(targetMachine);

	LLVMSetModuleDataLayout(ctx.module, dl);
	LLVMSetTarget(ctx.module, triple);

	// parse just one file for now
#ifdef COMPILER
	if(strcmp(argv[1], "-")==0)
		yyin = stdin;
	else
		yyin = fopen(argv[1], "r");
#else
	yyin = stdin;
#endif
	ret = yyparse(&ctx);

	fclose(yyin);

	if(ret != 0)
		return -(2+ret);

	LLVMBuildRetVoid(ctx.builder);

#ifdef COMPILER

	LLVMDumpModule(ctx.module);

	if(LLVMTargetMachineEmitToFile(targetMachine, ctx.module, "output.o", LLVMObjectFile, &err) != 0) {
		fprintf(stderr, "Error generating output: %s\n", err);
		LLVMDisposeMessage(err);
		return 6;
	}

	//link
	if(fork() == 0) {

		int i, arg_count = 0, arg_cap=100;
		struct dirent *dentry;

		char *arg, **args = calloc(arg_cap, sizeof(char*));

		args[arg_count++] = "/usr/bin/ld";
		args[arg_count++] = "acai/acai.a";

		DIR *d = opendir("acai/");
		while((dentry = readdir(d)) != NULL) {

			i = strlen(dentry->d_name);

			if(i > 2 && strcmp(&dentry->d_name[i - 2], ".a") == 0 && strcmp(dentry->d_name, "acai.a") != 0) {

				arg = malloc(i + 16);
				strcpy(arg, "acai/");
				strcat(arg, dentry->d_name);

				args[arg_count++] = arg;
			}

			if(arg_count == arg_cap) {
				arg_cap += 100;
				args = reallocarray(args, arg_cap, sizeof(char*));
			}
		}
		closedir(d);


		if(arg_count+7 == arg_cap) {
			arg_cap += 10;
			args = reallocarray(args, arg_cap, sizeof(char*));
		}

		args[arg_count++] = "output.o";

		args[arg_count++] = "-o";
		args[arg_count++] = output;

		args[arg_count++] = "-lc";
		args[arg_count++] = "-dynamic-linker";
		args[arg_count++] = "/lib64/ld-linux-x86-64.so.2";
		args[arg_count++] = NULL;

		execv(args[0], args);
	}

	LLVMDisposeTargetMachine(targetMachine);
#elif REPL



#endif

	llvm_finish(&ctx);

	return 0;
}
