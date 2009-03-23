/*  ImaNES: I'm a NES. An intelligent NES emulator

    main.c   -    Main routine for ImaNES

    Copyright (C) 2008   Rodrigo Tobar Carrizo

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef __APPLE__
#include "config.h"
#else
#define PACKAGE_BUGREPORT "https://csrg.inf.utfsm.cl/flyspray/index.php?project=10"
#endif

#include "cpu.h"
#include "debug.h"
#include "imaconfig.h"
#include "instruction_set.h"
#include "loop.h"
#include "mapper.h"
#include "pad.h"
#include "palette.h"
#include "parse_file.h"
#include "ppu.h"
#include "screen.h"
#include "sram.h"

void usage(FILE *file, char *argv[]) {
	fprintf(file,"\nImaNES: I'm a NES\n\n");
	fprintf(file,"This program is licensed under the GPLv3 license.\n");
	fprintf(file,"For bug reports, please refer to: %s\n\n", PACKAGE_BUGREPORT);
	fprintf(file,"Usage: %s [options] <rom file>\n\n",argv[0]);
	fprintf(file,"Options:\n");
	fprintf(file,"  -v        Increase verbosity. More -v, more verbose. Default: 0\n");
	fprintf(file,"  -s <n>    Video scaling facotr. Default: 1\n");
	fprintf(file,"  -c        Use SDL color construction. Default: no\n");
	fprintf(file,"  -h,-?     Show this help and exit\n");
	fprintf(file,"\n");
}

void parse_options(int args, char *argv[]) {

	int opt;

	config.verbosity = 0;

	while( (opt = getopt(args, argv, "cvhHs:?")) != -1 ) {

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

			case 'c':
				config.use_sdl_colors = 1;
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

	char *rom_file;
	char *save_file;

	ines_file *nes_rom;

	/* Print NOW everything :D */
	setbuf(stdout,NULL);
	setbuf(stderr,NULL);

	/* Parse command line options */
	parse_options(args, argv);

	load_user_configuration();

	/* Initialize static data */
	initialize_configuration();
	initialize_palette();
	initialize_instruction_set();
	initialize_cpu();
	initialize_ppu();
	initialize_pads();

	rom_file = (char *)malloc(strlen(argv[optind]));
	save_file = (char *)malloc(strlen(argv[optind])+4);
	strcpy(rom_file,argv[optind]);
	strcpy(save_file,argv[optind]);
	strcat(save_file,".sav");

	/* Read the ines file and get all the ROM/VROM */
	nes_rom = check_ines_file(rom_file);
	map_rom_memory(nes_rom);

	/* Create the screen and a separate thread for the events */
	init_screen();

	load_sram(save_file);

	/* Main execution loop */
	main_loop(nes_rom);

	end_screen();
	free_ines_file(nes_rom);
	save_sram(save_file);

	return 0;
}
