/*  ImaNES: I'm a NES. An intelligent NES emulator

    screenshot.c   -    Screenshot utility for ImaNES

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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _MSC_VER
#include <io.h>
#include <share.h>
#include <sys/types.h>
#else
#include <unistd.h>
#endif

#include "common.h"
#include "debug.h"
#include "imaconfig.h"
#include "screen.h"
#include "screenshot.h"

void save_screenshot() {

	int fd;
	int i;
	int j;
	unsigned int offset;
	unsigned int total_size;
	char *ss_dir;
	char *ss_file;
	char *tmp;
	char *buffer;
	char *color;
	uint16_t tmp16;
	uint32_t tmp32;
	Uint32 *pixels;
	struct stat s;
#ifdef _MSC_VER
	int written;
#else
	ssize_t written;
#endif

	total_size = 0x36 + (NES_SCREEN_WIDTH * NES_NTSC_HEIGHT)*3;
	buffer = (char *)malloc(total_size);

	/* Magic number */
	memset(buffer, 'B', 1);
	memset(buffer + 0x01, 'M', 1);

	/* file size, reserved data and offset */
	tmp32 = total_size;
	memcpy(buffer + 0x02, &tmp32, 4);
	memset(buffer + 0x06, 0, 4);
	tmp32 = 0x36;
	memcpy(buffer + 0x0A, &tmp32, 4);

	/*** DIB header ***/
	tmp32 = 0x28;
	memcpy(buffer + 0x0E, &tmp32, 4);
	/* Dimensions */
	tmp32 = NES_SCREEN_WIDTH;
	memcpy(buffer + 0x12, &tmp32, 4);
	tmp32 = NES_NTSC_HEIGHT;
	memcpy(buffer + 0x16, &tmp32, 4);
	tmp16 = 1;
	memcpy(buffer + 0x1A, &tmp16, 2);
	/* bpp */
	tmp16 = 24;
	memcpy(buffer + 0x1C, &tmp16, 2);
	/* Compression method */
	memset(buffer + 0x1E, 0, 4);
	/* Raw data size */
	tmp32 = (NES_SCREEN_WIDTH * NES_NTSC_HEIGHT)*3;
	memcpy(buffer + 0x22, &tmp32, 4);
	/* Resolutions */
	tmp32 = 2835;
	memcpy(buffer + 0x26, &tmp32, 4);
	memcpy(buffer + 0x2A, &tmp32, 4);
	/* Palette colors */
	memset(buffer + 0x2E, 0, 4);
	memset(buffer + 0x32, 0, 4);

	/* Pixel data */
	offset = 0x36;
	pixels = (Uint32*)nes_screen->pixels;
	for(i = NES_NTSC_HEIGHT - 1; i>=0; i--) {
		for(j = 0; j!= NES_SCREEN_WIDTH; j++) {
			color = (char *)(pixels+i*NES_SCREEN_WIDTH+j);
			memcpy(buffer + offset++, color, 1);
			memcpy(buffer + offset++, color+1, 1);
			memcpy(buffer + offset++, color+2, 1);
		}
	}

	ss_dir = get_imanes_dir(Snapshots);
	if( ss_dir == NULL ) {
		fprintf(stderr,"Couldn't save screenshot\n");
		free(buffer);
		free(ss_dir);
		return;
	}

	/* We loop until we find a name for a file that doesn't exist,
	 * so we don't overwrite an existing screenshot */
	tmp = get_filename(config.rom_file);
	ss_file = (char *)malloc(strlen(ss_dir) + strlen(tmp) + 4 + 7);
	for(i=0;;i++) {
#ifdef _MSC_VER
		sprintf_s(ss_file,strlen(ss_dir)+strlen(tmp)+4+7,"%s/%s-%04d.bmp", ss_dir, i, tmp, i);
#else
		sprintf(ss_file,"%s/%s-%04d.bmp", ss_dir, tmp, i);
#endif
		j = stat(ss_file, &s);
		if( j == -1 )
			break;
	}

#ifdef _MSC_VER
	_sopen_s(&fd,ss_file, O_WRONLY|O_CREAT, _SH_DENYWR, _S_IREAD|_S_IWRITE);
#else
	fd = open(ss_file, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
#endif
	free(ss_dir);
	free(tmp);
	free(ss_file);

	if( fd == -1 ) {
		tmp = (char *)malloc(23 + strlen(ss_file));
#ifdef _MSC_VER
		sprintf_s(tmp, strlen(ss_file)+23, "Error while opening '%s'", ss_file);
#else
		sprintf(tmp, "Error while opening '%s'", ss_file);
#endif
		perror(tmp);
		free(tmp);
		return;
	}

#ifdef _MSC_VER
	written = _write(fd, (void *)buffer, total_size);
#else
	written = write(fd, (void *)buffer, total_size);
#endif

	if( written != total_size ) {
		perror("Error while saving snapshot");
		return;
	}
#ifdef _MSC_VER
	_close(fd);
#else
	close(fd);
#endif

	INFO( printf("Saved screenshot at '%s'\n", ss_file) );

	free(buffer);
}
