/*  ImaNES: I'm a NES. An intelligent NES emulator

    sram.c   -    SRAM storage emulation under ImaNES

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

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "cpu.h"
#include "debug.h"
#include "i18n.h"
#include "platform.h"
#include "sram.h"


void save_sram(char *save_file) {

	int fd;
	RW_RET written_bytes;

	if( !CPU->sram_enabled )
		return;

	IMANES_OPEN(fd,save_file, IMANES_OPEN_WRITE);

	if( fd == -1 ) {
		fprintf(stderr,_("Error while opening '%s': "), save_file);
		perror(NULL);
		return;
	}

	written_bytes = IMANES_WRITE(fd, CPU->RAM + 0x6000, 0x2000);

	if( written_bytes != 0x2000 ) {
		fprintf(stderr,_("Couldn't dump SRAM data to '%s': "), save_file);
		perror(NULL);
	}

	IMANES_CLOSE(fd);

	return;
}

char *load_sram(char *rom_file) {

	int fd;
	char *save_file;
	char *save_dir;
	char *tmp;
	RW_RET read_bytes;

	INFO( printf(_("Loading SRAM... ")) );
	save_dir = get_imanes_dir(Saves);

	if( save_dir == NULL ) {
		fprintf(stderr,_("Couldn't load SRAM because saves direcory cannot be accessed\n"));
		free(save_dir);
		return NULL;
	}

	/* Get just the name of the file */
	tmp = get_filename(rom_file);

	save_file = (char *)malloc(strlen(save_dir) + strlen(tmp) + 6);
	imanes_sprintf(save_file, strlen(save_dir) + strlen(tmp)+6, "%s%c%s.sav", save_dir, DIR_SEP, tmp);
	free(tmp);
	free(save_dir);

	/* If SRAM is not enabled, just return the name of the file */
	if( !CPU->sram_enabled ) {
		INFO( printf(_("SRAM disabled, not loading anything\n")) );
		return save_file;
	}

	IMANES_OPEN(fd,save_file, IMANES_OPEN_READ);

	if( fd == -1 ) {
		fprintf(stderr,_("Error while opening '%s': "), save_file);
		perror(NULL);
		return save_file;
	}

	read_bytes = IMANES_READ(fd, CPU->RAM + 0x6000, 0x2000);

	if( read_bytes != 0x2000 ) {
		fprintf(stderr,_("File '%s' is not a valid SRAM dump file, SRAM not loaded.\n"), save_file);
		memset(CPU->RAM + 0x6000, 0, 0x2000);
	}

	IMANES_CLOSE(fd);

	INFO( printf(_("done!\n")) );
	return save_file;
}
