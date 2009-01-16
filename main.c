#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cpu.h"
#include "mapper.h"
#include "parse_file.h"
#include "screen.h"

void usage(char *argv[]) {
	fprintf(stderr,"Usage: %s <rom file>\n",argv[0]);
}

int main(int args, char *argv[]) {

	ines_file *nes_rom;

	if( args < 2 ) {
		usage(argv);
		exit(EXIT_FAILURE);
	}

	initialize_mapper_list();
	initialize_cpu();
	nes_rom = check_ines_file(argv[1]);
	map_rom_memory(nes_rom);
	init_screen();

	/* Main loop */
	sleep(10);

	return 0;
}
