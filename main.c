#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cpu.h"
#include "instruction_set.h"
#include "loop.h"
#include "mapper.h"
#include "parse_file.h"
#include "ppu.h"
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

	setbuf(stdout,NULL);
	setbuf(stderr,NULL);

	/* Initialize static data */
	initialize_mapper_list();
	initialize_instruction_set();
	initialize_cpu();
	initialize_ppu();

	nes_rom = check_ines_file(argv[1]);
	map_rom_memory(nes_rom);
	//init_screen();

	/* Main loop */
	main_loop(nes_rom);

	free_ines_file(nes_rom);
	return 0;
}
