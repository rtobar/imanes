#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "cpu.h"
#include "debug.h"
#include "instruction_set.h"
#include "loop.h"
#include "mapper.h"
#include "pad.h"
#include "palette.h"
#include "parse_file.h"
#include "ppu.h"
#include "screen.h"

void usage(FILE *file, char *argv[]) {
	fprintf(file,"\nUsage: %s [options] <rom file>\n\n",argv[0]);
	fprintf(file,"Options:\n");
	fprintf(file,"  -v        Increase verbosity. More -v, more verbose. Default: 0\n");
	fprintf(file,"  -s        Video scaling facotr. Default: 1\n");
	fprintf(file,"  -h,-?     Show this help and exit\n");
	fprintf(file,"\n");
}

void parse_options(int args, char *argv[]) {

	int opt;

	config.verbosity = 0;

	while( (opt = getopt(args, argv, "vhHs:?")) != -1 ) {

		switch(opt) {
			case 'v':
				config.verbosity++;
				break;

			case 's':
				config.video_scale = atoi(optarg);
				if( config.video_scale == 0 ) {
					fprintf(stderr,"Error: invalid video scale factor. Must be an integer value\n");
					usage(stderr, argv);
					exit(EXIT_FAILURE);
				}
				break;

			case '?':
			case 'h':
			case 'H':
				usage(stdout,argv);
				exit(EXIT_SUCCESS);
				break;

			default:
				usage(stderr,argv);
				exit(EXIT_FAILURE);
				break;
		}

	}

	if( optind >= args ) {
		fprintf(stderr,"%s: Error: Expected ROM file, none given\n", argv[0]);
		usage(stderr,argv);
		exit(EXIT_FAILURE);
	}

}

int main(int args, char *argv[]) {

	ines_file *nes_rom;

	setbuf(stdout,NULL);
	setbuf(stderr,NULL);

	/* Parse command line options */
	parse_options(args, argv);

	/* Initialize static data */
	initialize_configuration();
	initialize_palette();
	initialize_instruction_set();
	initialize_cpu();
	initialize_ppu();
	initialize_pads();

	nes_rom = check_ines_file(argv[optind]);
	map_rom_memory(nes_rom);
	init_ppu_vram(nes_rom);
	init_screen();

	/* Main loop */
	main_loop(nes_rom);

	free_ines_file(nes_rom);
	return 0;
}
