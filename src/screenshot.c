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
#include <unistd.h>

#ifdef __APPLE__
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#else
#include <SDL.h>
#include <SDL_thread.h>
#endif

#include "debug.h"
#include "screen.h"
#include "screenshot.h"

void save_screenshot() {

	int fd;
	int i;
	int j;
	unsigned int offset;
	unsigned int total_size;
	ssize_t written;
	void *buffer;
	void *color;
	uint16_t tmp16;
	uint32_t tmp32;
	Uint32 *pixels;

	total_size = 0x36 + (NES_SCREEN_WIDTH * NES_NTSC_HEIGHT)*3;
	buffer = malloc(total_size);

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
			color = pixels+i*NES_SCREEN_WIDTH+j;
			memcpy(buffer + offset++, color, 1);
			memcpy(buffer + offset++, color+1, 1);
			memcpy(buffer + offset++, color+2, 1);
		}
	}

	fd = open("screenshot.bmp", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	if( fd == -1 ) {
		perror("Error while opening 'screenshot.bmp'");
		return;
	}

	written = write(fd, (void *)buffer, total_size);
	if( written != total_size ) {
		perror("Error while saving snapshot");
		return;
	}
	close(fd);

	INFO( printf("Saved screenshot at '%s'\n", "screenshot.bmp") );

	free(buffer);
}
