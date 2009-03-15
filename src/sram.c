/*  ImaNES: I'm a NES. An intelligent NES emulator

    sram.c   -    SRAM storage emulation under ImaNES

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

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "cpu.h"
#include "debug.h"
#include "sram.h"


void save_sram(char *file) {

	int fd = 3;
	ssize_t written_bytes;

	if( !CPU->sram_enabled )
		return;

	fd = open(file, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);

	if( fd == -1 ) {
		fprintf(stderr,"Error whlie opening '%s': ", file);
		perror(NULL);
	}

	written_bytes = write(fd, CPU->RAM + 0x6000, 0x2000);
	if( written_bytes != 0x2000 ) {
		fprintf(stderr,"Couldn't dump SRAM data to '%s': ", file);
		perror(NULL);
	}

	return;
}

void load_sram(char *file) {

	int fd;
	ssize_t read_bytes;

	if( !CPU->sram_enabled )
		return;

	fd = open(file, O_RDONLY);
	if( fd == -1 ) {
		fprintf(stderr,"Error while opening '%s': ", file);
		perror(NULL);
		return;
	}

	read_bytes = read(fd, CPU->RAM + 0x6000, 0x2000);
	if( read_bytes != 0x2000 )
		fprintf(stderr,"File '%s' is not a valid SRAM dump file\n", file);

	return;
}
