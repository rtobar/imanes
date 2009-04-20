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

#ifdef _MSC_VER
#include <io.h>
#include <share.h>
#include <sys/types.h>
#include <sys/stat.h>
#else
#include <unistd.h>
#endif

#include "cpu.h"
#include "debug.h"
#include "sram.h"


void save_sram(char *file) {

	int fd = 3;

#ifdef _MSC_VER
	int written_bytes;
#else
	ssize_t written_bytes;
#endif

	if( !CPU->sram_enabled )
		return;

#ifdef _MSC_VER
	fd = _sopen_s(&fd,file, O_WRONLY|O_CREAT, _SH_DENYWR, _S_IREAD|_S_IWRITE);
#else
	fd = open(file, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
#endif

	if( fd == -1 ) {
		fprintf(stderr,"Error whlie opening '%s': ", file);
		perror(NULL);
	}

#ifdef _MSC_VER
	written_bytes = _write(fd, CPU->RAM + 0x6000, 0x2000);
#else
	written_bytes = write(fd, CPU->RAM + 0x6000, 0x2000);
#endif

	if( written_bytes != 0x2000 ) {
		fprintf(stderr,"Couldn't dump SRAM data to '%s': ", file);
		perror(NULL);
	}

	return;
}

void load_sram(char *file) {

	int fd;

#ifdef _MSC_VER
	int read_bytes;
#else
	ssize_t read_bytes;
#endif

	if( !CPU->sram_enabled )
		return;
#ifdef _MSC_VER
	fd = _sopen_s(&fd,file, O_RDONLY, _SH_DENYWR, _S_IREAD|_S_IWRITE);
#else
	fd = open(file, O_RDONLY);
#endif

	if( fd == -1 ) {
		fprintf(stderr,"Error while opening '%s': ", file);
		perror(NULL);
		return;
	}

#ifdef _MSC_VER
	read_bytes = _read(fd, CPU->RAM + 0x6000, 0x2000);
#else
	read_bytes = read(fd, CPU->RAM + 0x6000, 0x2000);
#endif

	if( read_bytes != 0x2000 )
		fprintf(stderr,"File '%s' is not a valid SRAM dump file\n", file);

	return;
}
