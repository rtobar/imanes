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

void save_screenshot() {

	int fd;
	ssize_t written;
	void *buffer;
	uint16_t tmp16;
	uint32_t tmp32;

	buffer = malloc(0x46);

	/* Magic number */
	memset(buffer, 'B', 1);
	memset(buffer + 0x01, 'M', 1);

	/* file size, reserved data and offset */
	tmp32 = 0x46;
	memcpy(buffer + 0x02, &tmp32, 4);
	memset(buffer + 0x06, 0, 4);
	tmp32 = 0x36;
	memcpy(buffer + 0x0A, &tmp32, 4);

	/*** DIB header ***/
	tmp32 = 0x28;
	memcpy(buffer + 0x0E, &tmp32, 4);
	/* Dimensions */
	//tmp32 = NES_SCREEN_WIDTH;
	tmp32 = 2;
	memcpy(buffer + 0x12, &tmp32, 4);
	//tmp32 = NES_NTSC_HEIGHT;
	tmp32 = 2;
	memcpy(buffer + 0x16, &tmp32, 4);
	tmp16 = 1;
	memcpy(buffer + 0x1A, &tmp16, 2);
	/* bpp */
	tmp16 = 24;
	memcpy(buffer + 0x1C, &tmp16, 2);
	/* Compression method */
	memset(buffer + 0x1E, 0, 4);
	/* Raw data size */
	//tmp32 = (NES_SCREEN_WIDTH * NES_NTSC_HEIGHT)*3;
	tmp32 = 0x10;
	memcpy(buffer + 0x22, &tmp32, 4);
	/* Resolutions */
	tmp32 = 2835;
	memcpy(buffer + 0x26, &tmp32, 4);
	memcpy(buffer + 0x2A, &tmp32, 4);
	/* Palette colors */
	memset(buffer + 0x2E, 0, 4);
	memset(buffer + 0x32, 0, 4);

	/* Pixel data */
	memset(buffer + 0x36, 0, 1);
	memset(buffer + 0x37, 0, 1);
	memset(buffer + 0x38, 255, 1);
	memset(buffer + 0x39, 255, 1);
	memset(buffer + 0x3A, 255, 1);
	memset(buffer + 0x3B, 255, 1);
	memset(buffer + 0x3C, 0, 1);
	memset(buffer + 0x3D, 0, 1);
	memset(buffer + 0x3E, 255, 1);
	memset(buffer + 0x3F, 0, 1);
	memset(buffer + 0x40, 0, 1);
	memset(buffer + 0x41, 0, 1);
	memset(buffer + 0x42, 255, 1);
	memset(buffer + 0x43, 0, 1);
	memset(buffer + 0x44, 0, 1);
	memset(buffer + 0x45, 0, 1);

	fd = open("screenshot.bmp", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	if( fd == -1 ) {
		perror("Error while opening 'screenshot.bmp'");
		return;
	}

	written = write(fd, (void *)buffer, 0x46);
	if( written != 0x46 ) {
		perror("Error while saving snapshot");
		return;
	}
	close(fd);

	free(buffer);
}
