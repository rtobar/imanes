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


void save_sram(char *rom_file) {

	int fd = 3;
	char *save_file;

#ifdef _MSC_VER
	int written_bytes;
#else
	ssize_t written_bytes;
#endif

	if( !CPU->sram_enabled )
		return;

	return;
#ifdef _MSC_VER
	fd = _sopen_s(&fd,save_file, O_WRONLY|O_CREAT, _SH_DENYWR, _S_IREAD|_S_IWRITE);
#else
	fd = open(save_file, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
#endif

	if( fd == -1 ) {
		fprintf(stderr,"Error while opening '%s': ", save_file);
		perror(NULL);
	}

#ifdef _MSC_VER
	written_bytes = _write(fd, CPU->RAM + 0x6000, 0x2000);
#else
	written_bytes = write(fd, CPU->RAM + 0x6000, 0x2000);
#endif

	if( written_bytes != 0x2000 ) {
		fprintf(stderr,"Couldn't dump SRAM data to '%s': ", save_file);
		perror(NULL);
	}

	return;
}

void load_sram(char *rom_file) {

	int i;
	int fd;
	char *save_file;
	char *save_dir;
	char *tmp;
#ifdef _MSC_VER
#define SEP '\\'
	int read_bytes;
#else
#define SEP '/'
	ssize_t read_bytes;
#endif

	if( !CPU->sram_enabled )
		return;

	/* Get just the name of the file */
	for(i=strlen(rom_file); i>=0; i--) {
		if( rom_file[i] == SEP )
			break;
	}
	tmp = (char *)malloc(strlen(rom_file) - i);
	memcpy(tmp, rom_file + i + 1, strlen(rom_file) - i);

	/* Find where the extension of the file begins */
	printf("Given file is '%s'\n", tmp);
	for(i=strlen(tmp); i>=0; i--) {
		if( tmp[i] == '.' )
			break;
	}

	save_file = (char *)malloc(i+5);
#ifdef _MSC_VER
	sprintf_s(save_file, strlen(file)+5, "%s.sav", tmp);
#else
	strncpy(save_file, tmp, i);
	strcat(save_file, ".sav");
#endif
	free(tmp);
	printf("Saving file is '%s'\n", save_file);

	save_dir = get_imanes_dir(Saves);

	if( save_dir == NULL ) {
		fprintf(stderr,"Couldn't load SRAM because saves direcory cannot be accessed\n");
		free(save_dir);
		return;
	}

#ifdef _MSC_VER
	fd = _sopen_s(&fd,save_file, O_RDONLY, _SH_DENYWR, _S_IREAD|_S_IWRITE);
#else
	fd = open(save_file, O_RDONLY);
#endif

	if( fd == -1 ) {
		fprintf(stderr,"Error while opening '%s': ", save_file);
		perror(NULL);
		free(save_file);
		return;
	}

#ifdef _MSC_VER
	read_bytes = _read(fd, CPU->RAM + 0x6000, 0x2000);
#else
	read_bytes = read(fd, CPU->RAM + 0x6000, 0x2000);
#endif

	if( read_bytes != 0x2000 )
		fprintf(stderr,"File '%s' is not a valid SRAM dump file\n", save_file);

	free(save_file);
	return;
}
