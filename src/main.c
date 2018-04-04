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
#include "parser/ast.h"
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
#endif
			, argv0);
}

int main(int argc, char *argv[]) {

	int opt, ret;
	llvm_ctx ctx;
#ifdef COMPILER
	char *err;
	LLVMTargetRef target;
	char *output = "a.out";
#endif

	memset(&ctx, 0, sizeof(ctx));

#ifdef COMPILER
	while((opt = getopt_long(argc, argv, "h?o:", optlong, NULL)) != -1) {
#else
	while((opt = getopt_long(argc, argv, "h?", optlong, NULL)) != -1) {
#endif

		switch(opt) {
#ifdef COMPILER
			case 'o':
				output = optarg;
				break;
#endif
			case 'h':
			default:
				usage(argv[0]);
				return 1;
		}
	}

	if(argc < 2) {
		fprintf(stderr, "%s: error: no input files\n", argv[0]);
		return 2;
	}

	llvm_init(&ctx);

	//create a module from the first file
	llvm_create_module(argv[1], &ctx);

	acai_init(&ctx);

	llvm_create_main(&ctx);

	// parse just one file for now
	if(strcmp(argv[1], "-")==0)
		yyin = stdin;
	else
		yyin = fopen(argv[1], "r");

	ret = yyparse(&ctx);

	fclose(yyin);

	LLVMBuildRetVoid(ctx.builder);

#ifdef COMPILER
	char *triple = LLVMGetDefaultTargetTriple();

	LLVMSetTarget(ctx.module, triple);

//	LLVMRunPassManager(pm, ctx.module);

	if(LLVMGetTargetFromTriple(triple, &target, &err) != 0) {
		fprintf(stderr, "Error fetching target from triple '%s': %s\n", triple, err);
		return 3;
	}

	LLVMTargetMachineRef targetMachine = LLVMCreateTargetMachine(target, triple, "generic", "", LLVMCodeGenLevelNone, LLVMRelocDefault, LLVMCodeModelDefault);

	LLVMTargetDataRef dl = LLVMCreateTargetDataLayout(targetMachine);

	LLVMSetModuleDataLayout(ctx.module, dl);
	LLVMSetTarget(ctx.module, triple);

	LLVMDumpModule(ctx.module);

	if(LLVMTargetMachineEmitToFile(targetMachine, ctx.module, "output.o", LLVMObjectFile, &err) != 0) {
		fprintf(stderr, "Error generating output: %s\n", err);
		LLVMDisposeMessage(err);
		return 4;
	}

	//link
	int i, arg_count = 0, arg_cap=100;
	struct dirent *dentry;

	char *arg, **args = calloc(arg_cap, sizeof(char*));

	args[arg_count++] = "/usr/bin/ld";

	DIR *d = opendir("acai/");
	while((dentry = readdir(d)) != NULL) {

		i = strlen(dentry->d_name);

		if(i > 2 && strcmp(&dentry->d_name[i - 2], ".a") == 0) {

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

	if(fork() == 0) {
		const char *cmd = "/usr/bin/ld";
		//execl(cmd, cmd, "-o", output, "output.o", "acai/acai.a", "-lc", "-dynamic-linker", "/lib64/ld-linux-x86-64.so.2", NULL);
		execv(args[0], args);
	}
#else
	LLVMDumpModule(ctx.module);
#endif

	llvm_finish(&ctx);

	return ret;
}
