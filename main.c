#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "parse_file.h"
#include "screen.h"

void usage(char *argv[]) {
	fprintf(stderr,"Usage: %s <rom file>\n",argv[0]);
}

int main(int args, char *argv[]) {

	if( args < 2 ) {
		usage(argv);
		exit(EXIT_FAILURE);
	}

	check_ines_file(argv[1]);
	init_screen();

	/* Main loop */
	sleep(10);

	return 0;
}
