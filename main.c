#include <stdio.h>
#include <stdlib.h>

#include "parse_file.h"

void usage(char *argv[]) {
	fprintf(stderr,"Usage: %s <rom file>\n",argv[0]);
}

int main(int args, char *argv[]) {

	if( args < 2 ) {
		usage(argv);
		exit(EXIT_FAILURE);
	}

	check_ines_file(argv[1]);

	return 0;
}
