#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include "llvm/init.h"

extern FILE *yyin;

void usage(char *argv0) {
	printf("Usage: %s input files\n", argv0);
}

int main(int argc, char *argv[]) {

	int opt;
	while((opt = getopt(argc, argv, "h?")) != -1) {

		switch(opt) {
			case 'h':
			default:
				usage(argv[0]);
				return 1;
		}
	}

	llvm_init();

	if(argc < 2) {
		fprintf(stderr, "%s: error: no input files\n", argv[0]);
		return 2;
	}

	return 0;
}
