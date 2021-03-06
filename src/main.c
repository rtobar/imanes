/*  ImaNES: I'm a NES. An intelligent NES emulator

    main.c   -    Main routine for ImaNES

    Copyright (C) 2009   Rodrigo Tobar Carrizo

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

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _MSC_VER
#include "XGetopt.h"
#else
#include <unistd.h>
#endif

#include "apu.h"
#include "clock.h"
#include "common.h"
#include "cpu.h"
#include "debug.h"
#include "gui.h"
#include "i18n.h"
#include "imaconfig.h"
#include "instruction_set.h"
#include "loop.h"
#include "mapper.h"
#include "pad.h"
#include "palette.h"
#include "parse_file.h"
#include "playback.h"
#include "ppu.h"
#include "screen.h"
#include "sram.h"

void usage(FILE *file, char *argv[]) {
	fprintf(file,_("\n%s: I'm a NES\n\n"), PACKAGE_NAME);
	fprintf(file,_("This program is licensed under the GPLv3 license.\n"));
	fprintf(file,_("For bug reports, please refer to %s\n\n"), PACKAGE_BUGREPORT);
	fprintf(file,_("Usage: %s [options] <rom file>\n\n"),argv[0]);
	fprintf(file,_("Options:\n"));
	fprintf(file,_("  -v        Increase verbosity. More -v, more verbose. Default: 0\n"));
	fprintf(file,_("  -s <n>    Video scaling factor. Default: 1\n"));
	fprintf(file,_("  -c        Use SDL color construction. Default: no\n"));
	fprintf(file,_("  -m        Mute sound. Default: no\n\n"));
	fprintf(file,_("  -h,-?     Show this help and exit\n"));
	fprintf(file,_("  -V        Show the current version of ImaNES and exit\n\n"));
	fprintf(file,_("ImaNES development is maintained by Rodrigo Tobar <rtobar@csrg.inf.utfsm.cl>\n"));
	fprintf(file,_("Please refer to the AUTHORS file for more details\n"));
	fprintf(file,"\n");
}

void print_version() {
	printf(_("\n%s version %s\n"), PACKAGE_NAME, PACKAGE_VERSION);
	printf(_("The current version of %s was compiled on %s, %s\n\n"), PACKAGE_NAME, __DATE__, __TIME__);
	printf(_("ImaNES development is maintained by Rodrigo Tobar <rtobar@csrg.inf.utfsm.cl>\n"));
	printf(_("Please refer to the AUTHORS file for more details\n"));
	printf("\n");
}

int parse_options(int args, char *argv[]) {

	int opt;

	config.verbosity = 0;

	while( (opt = getopt(args, argv, "mcvhHVs:?")) != -1 ) {

		switch(opt) {
			case 'm':
				config.sound_mute = 1;
				break;

			case 'v':
				config.verbosity++;
				break;

			case 's':
				config.video_scale = atoi(optarg);
				if( config.video_scale == 0 ) {
					fprintf(stderr,_("Error: invalid video scale factor. Must be an integer value\n"));
					return -1;
				}
				break;

			case 'c':
				config.use_sdl_colors = 1;
				break;

			case '?':
			case 'h':
			case 'H':
				usage(stdout,argv);
				return 0;

			case 'V':
				print_version();
				return 0;

			default:
				return -1;
		}

	}

	if( optind >= args ) {
		fprintf(stderr,_("%s: Error: Expected ROM file, none given\n"), argv[0]);
		return -1;
	}

	return 1;
}

int main(int args, char *argv[]) {

	char *save_file;
	ines_file *nes_rom;

	/* Print NOW everything :D */
#ifdef _MSC_VER
	setvbuf(stdout,NULL,_IONBF,0);
	setvbuf(stderr,NULL,_IONBF,0);
#else
	setbuf(stdout,NULL);
	setbuf(stderr,NULL);
#endif

	/* i18n stuff */
	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

	/* Parse command line options */
	initialize_configuration();
	switch ( parse_options(args, argv) ) {
		case -1:
			usage(stderr,argv);
			exit(EXIT_FAILURE);
		case 0:
			exit(EXIT_SUCCESS);
		default:
			break;
	}

	load_user_configuration();

	/* Initialize static data */
	initialize_palette();
	initialize_instruction_set();
	initialize_playback();
	initialize_apu();
	initialize_cpu();
	initialize_ppu();
	initialize_clock();
	initialize_pads();

	/* Read the ines file and get all the ROM/VROM */
	config.rom_file = argv[optind];
	nes_rom = check_ines_file(config.rom_file);
	map_rom_memory(nes_rom);
	save_file = load_sram(config.rom_file);

	/* Init the graphics engine */
	init_screen();
	init_gui();

	/* Main execution loop */
	main_loop();

	/* After finishing the emulation, save the SRAM if necessary */
	save_sram(save_file);
	free(save_file);

	/* Free all the used resources */
	mapper->end_mapper();
	end_screen();
	end_gui();
	end_ppu();
	end_cpu();
	end_apu();
	end_playback();
	free_ines_file(nes_rom);

	return 0;
}
