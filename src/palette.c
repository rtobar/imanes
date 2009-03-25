/*  ImaNES: I'm a NES. An intelligent NES emulator

    palette.c   -    NES palette support for ImaNES

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

#include "common.h"
#include "palette.h"
#include "ppu.h"

nes_palette *system_palette;

static nes_palette *loopy_palette;
static nes_palette *other_palette;

void initialize_palette() {

	loopy_palette = (nes_palette *)malloc(sizeof(nes_palette)*NES_PALETTE_COLORS);
	other_palette = (nes_palette *)malloc(sizeof(nes_palette)*NES_PALETTE_COLORS);

	/* Loopy palette */
	FILL_NES_PALETTE(loopy_palette, 0x00, 0x75, 0x75, 0x75);
	FILL_NES_PALETTE(loopy_palette, 0x01, 0x27, 0x1B, 0x8F);
	FILL_NES_PALETTE(loopy_palette, 0x02, 0x00, 0x00, 0xAB);
	FILL_NES_PALETTE(loopy_palette, 0x03, 0x47, 0x00, 0x9F);
	FILL_NES_PALETTE(loopy_palette, 0x04, 0x8F, 0x00, 0x77);
	FILL_NES_PALETTE(loopy_palette, 0x05, 0xAB, 0x00, 0x13);
	FILL_NES_PALETTE(loopy_palette, 0x06, 0xA7, 0x00, 0x00);
	FILL_NES_PALETTE(loopy_palette, 0x07, 0x7F, 0x0B, 0x00);
	FILL_NES_PALETTE(loopy_palette, 0x08, 0x43, 0x2F, 0x00);
	FILL_NES_PALETTE(loopy_palette, 0x09, 0x00, 0x47, 0x00);
	FILL_NES_PALETTE(loopy_palette, 0x0A, 0x00, 0x51, 0x00);
	FILL_NES_PALETTE(loopy_palette, 0x0B, 0x00, 0x3F, 0x17);
	FILL_NES_PALETTE(loopy_palette, 0x0C, 0x1B, 0x3F, 0x5F);
	FILL_NES_PALETTE(loopy_palette, 0x0D, 0x00, 0x00, 0x00);
	FILL_NES_PALETTE(loopy_palette, 0x0E, 0x00, 0x00, 0x00);
	FILL_NES_PALETTE(loopy_palette, 0x0F, 0x00, 0x00, 0x00);
	FILL_NES_PALETTE(loopy_palette, 0x10, 0xBC, 0xBC, 0xBC);
	FILL_NES_PALETTE(loopy_palette, 0x11, 0x00, 0x73, 0xEF);
	FILL_NES_PALETTE(loopy_palette, 0x12, 0x23, 0x3B, 0xEF);
	FILL_NES_PALETTE(loopy_palette, 0x13, 0x83, 0x00, 0xF3);
	FILL_NES_PALETTE(loopy_palette, 0x14, 0xBF, 0x00, 0xBF);
	FILL_NES_PALETTE(loopy_palette, 0x15, 0xE7, 0x00, 0x5B);
	FILL_NES_PALETTE(loopy_palette, 0x16, 0xDB, 0x2B, 0x00);
	FILL_NES_PALETTE(loopy_palette, 0x17, 0xCB, 0x4F, 0x0F);
	FILL_NES_PALETTE(loopy_palette, 0x18, 0x8B, 0x73, 0x00);
	FILL_NES_PALETTE(loopy_palette, 0x19, 0x00, 0x97, 0x00);
	FILL_NES_PALETTE(loopy_palette, 0x1A, 0x00, 0xAB, 0x00);
	FILL_NES_PALETTE(loopy_palette, 0x1B, 0x00, 0x93, 0x3B);
	FILL_NES_PALETTE(loopy_palette, 0x1C, 0x00, 0x83, 0x8B);
	FILL_NES_PALETTE(loopy_palette, 0x1D, 0x00, 0x00, 0x00);
	FILL_NES_PALETTE(loopy_palette, 0x1E, 0x00, 0x00, 0x00);
	FILL_NES_PALETTE(loopy_palette, 0x1F, 0x00, 0x00, 0x00);
	FILL_NES_PALETTE(loopy_palette, 0x20, 0xFF, 0xFF, 0xFF);
	FILL_NES_PALETTE(loopy_palette, 0x21, 0x3F, 0xBF, 0xFF);
	FILL_NES_PALETTE(loopy_palette, 0x22, 0x5F, 0x97, 0xFF);
	FILL_NES_PALETTE(loopy_palette, 0x23, 0xA7, 0x8B, 0xFD);
	FILL_NES_PALETTE(loopy_palette, 0x24, 0xF7, 0x7B, 0xFF);
	FILL_NES_PALETTE(loopy_palette, 0x25, 0xFF, 0x77, 0xB7);
	FILL_NES_PALETTE(loopy_palette, 0x26, 0xFF, 0x77, 0x63);
	FILL_NES_PALETTE(loopy_palette, 0x27, 0xFF, 0x9B, 0x3B);
	FILL_NES_PALETTE(loopy_palette, 0x28, 0xF3, 0xBF, 0x3F);
	FILL_NES_PALETTE(loopy_palette, 0x29, 0x83, 0xD3, 0x13);
	FILL_NES_PALETTE(loopy_palette, 0x2A, 0x4F, 0xDF, 0x4B);
	FILL_NES_PALETTE(loopy_palette, 0x2B, 0x58, 0xF8, 0x98);
	FILL_NES_PALETTE(loopy_palette, 0x2C, 0x00, 0xEB, 0xDB);
	FILL_NES_PALETTE(loopy_palette, 0x2D, 0x00, 0x00, 0x00);
	FILL_NES_PALETTE(loopy_palette, 0x2E, 0x00, 0x00, 0x00);
	FILL_NES_PALETTE(loopy_palette, 0x2F, 0x00, 0x00, 0x00);
	FILL_NES_PALETTE(loopy_palette, 0x30, 0xFF, 0xFF, 0xFF);
	FILL_NES_PALETTE(loopy_palette, 0x31, 0xAB, 0xE7, 0xFF);
	FILL_NES_PALETTE(loopy_palette, 0x32, 0xC7, 0xD7, 0xFF);
	FILL_NES_PALETTE(loopy_palette, 0x33, 0xD7, 0xCB, 0xFF);
	FILL_NES_PALETTE(loopy_palette, 0x34, 0xFF, 0xC7, 0xFF);
	FILL_NES_PALETTE(loopy_palette, 0x35, 0xFF, 0xC7, 0xDB);
	FILL_NES_PALETTE(loopy_palette, 0x36, 0xFF, 0xBF, 0xB3);
	FILL_NES_PALETTE(loopy_palette, 0x37, 0xFF, 0xDB, 0xAB);
	FILL_NES_PALETTE(loopy_palette, 0x38, 0xFF, 0xE7, 0xA3);
	FILL_NES_PALETTE(loopy_palette, 0x39, 0xE3, 0xFF, 0xA3);
	FILL_NES_PALETTE(loopy_palette, 0x3A, 0xAB, 0xF3, 0xBF);
	FILL_NES_PALETTE(loopy_palette, 0x3B, 0xB3, 0xFF, 0xCF);
	FILL_NES_PALETTE(loopy_palette, 0x3C, 0x9F, 0xFF, 0xF3);
	FILL_NES_PALETTE(loopy_palette, 0x3D, 0x00, 0x00, 0x00);
	FILL_NES_PALETTE(loopy_palette, 0x3E, 0x00, 0x00, 0x00);
	FILL_NES_PALETTE(loopy_palette, 0x3F, 0x00, 0x00, 0x00);

	/* Other palette */
	FILL_NES_PALETTE(other_palette, 0x00, 0x80, 0x80, 0x80);
	FILL_NES_PALETTE(other_palette, 0x01, 0x00, 0x3D, 0xA6);
	FILL_NES_PALETTE(other_palette, 0x02, 0x00, 0x12, 0xB0);
	FILL_NES_PALETTE(other_palette, 0x03, 0x44, 0x00, 0x96);
	FILL_NES_PALETTE(other_palette, 0x04, 0xA1, 0x00, 0x5E);
	FILL_NES_PALETTE(other_palette, 0x05, 0xC7, 0x00, 0x28);
	FILL_NES_PALETTE(other_palette, 0x06, 0xBA, 0x06, 0x00);
	FILL_NES_PALETTE(other_palette, 0x07, 0x8C, 0x17, 0x00);
	FILL_NES_PALETTE(other_palette, 0x08, 0x5C, 0x2F, 0x00);
	FILL_NES_PALETTE(other_palette, 0x09, 0x10, 0x45, 0x00);
	FILL_NES_PALETTE(other_palette, 0x0A, 0x05, 0x4A, 0x00);
	FILL_NES_PALETTE(other_palette, 0x0B, 0x00, 0x47, 0x2E);
	FILL_NES_PALETTE(other_palette, 0x0C, 0x00, 0x41, 0x66);
	FILL_NES_PALETTE(other_palette, 0x0D, 0x00, 0x00, 0x00);
	FILL_NES_PALETTE(other_palette, 0x0E, 0x05, 0x05, 0x05);
	FILL_NES_PALETTE(other_palette, 0x0F, 0x05, 0x05, 0x05);
	FILL_NES_PALETTE(other_palette, 0x10, 0xC7, 0xC7, 0xC7);
	FILL_NES_PALETTE(other_palette, 0x11, 0x00, 0x77, 0xFF);
	FILL_NES_PALETTE(other_palette, 0x12, 0x21, 0x55, 0xFF);
	FILL_NES_PALETTE(other_palette, 0x13, 0x82, 0x37, 0xFA);
	FILL_NES_PALETTE(other_palette, 0x14, 0xEB, 0x2F, 0xB5);
	FILL_NES_PALETTE(other_palette, 0x15, 0xFF, 0x29, 0x50);
	FILL_NES_PALETTE(other_palette, 0x16, 0xFF, 0x22, 0x00);
	FILL_NES_PALETTE(other_palette, 0x17, 0xD6, 0x32, 0x00);
	FILL_NES_PALETTE(other_palette, 0x18, 0xC4, 0x62, 0x00);
	FILL_NES_PALETTE(other_palette, 0x19, 0x35, 0x80, 0x00);
	FILL_NES_PALETTE(other_palette, 0x1A, 0x05, 0x8F, 0x00);
	FILL_NES_PALETTE(other_palette, 0x1B, 0x00, 0x8A, 0x55);
	FILL_NES_PALETTE(other_palette, 0x1C, 0x00, 0x99, 0xCC);
	FILL_NES_PALETTE(other_palette, 0x1D, 0x21, 0x21, 0x21);
	FILL_NES_PALETTE(other_palette, 0x1E, 0x09, 0x09, 0x09);
	FILL_NES_PALETTE(other_palette, 0x1F, 0x09, 0x09, 0x09);
	FILL_NES_PALETTE(other_palette, 0x20, 0xFF, 0xFF, 0xFF);
	FILL_NES_PALETTE(other_palette, 0x21, 0x0F, 0xD7, 0xFF);
	FILL_NES_PALETTE(other_palette, 0x22, 0x69, 0xA2, 0xFF);
	FILL_NES_PALETTE(other_palette, 0x23, 0xD4, 0x80, 0xFF);
	FILL_NES_PALETTE(other_palette, 0x24, 0xFF, 0x45, 0xF3);
	FILL_NES_PALETTE(other_palette, 0x25, 0xFF, 0x61, 0x8B);
	FILL_NES_PALETTE(other_palette, 0x26, 0xFF, 0x88, 0x33);
	FILL_NES_PALETTE(other_palette, 0x27, 0xFF, 0x9C, 0x12);
	FILL_NES_PALETTE(other_palette, 0x28, 0xFA, 0xBC, 0x20);
	FILL_NES_PALETTE(other_palette, 0x29, 0x9F, 0xE3, 0x0E);
	FILL_NES_PALETTE(other_palette, 0x2A, 0x2B, 0xF0, 0x35);
	FILL_NES_PALETTE(other_palette, 0x2B, 0x0C, 0xF0, 0xA4);
	FILL_NES_PALETTE(other_palette, 0x2C, 0x05, 0xFB, 0xFF);
	FILL_NES_PALETTE(other_palette, 0x2D, 0x5E, 0x5E, 0x5E);
	FILL_NES_PALETTE(other_palette, 0x2E, 0x0D, 0x0D, 0x0D);
	FILL_NES_PALETTE(other_palette, 0x2F, 0x0D, 0x0D, 0x0D);
	FILL_NES_PALETTE(other_palette, 0x30, 0xFF, 0xFF, 0xFF);
	FILL_NES_PALETTE(other_palette, 0x31, 0xA6, 0xFC, 0xFF);
	FILL_NES_PALETTE(other_palette, 0x32, 0xB3, 0xEC, 0xFF);
	FILL_NES_PALETTE(other_palette, 0x33, 0xDA, 0xAB, 0xEB);
	FILL_NES_PALETTE(other_palette, 0x34, 0xFF, 0xA8, 0xF9);
	FILL_NES_PALETTE(other_palette, 0x35, 0xFF, 0xAB, 0xB3);
	FILL_NES_PALETTE(other_palette, 0x36, 0xFF, 0xD2, 0xB0);
	FILL_NES_PALETTE(other_palette, 0x37, 0xFF, 0xEF, 0xA6);
	FILL_NES_PALETTE(other_palette, 0x38, 0xFF, 0xF7, 0x9C);
	FILL_NES_PALETTE(other_palette, 0x39, 0xD7, 0xE8, 0x95);
	FILL_NES_PALETTE(other_palette, 0x3A, 0xA6, 0xED, 0xAF);
	FILL_NES_PALETTE(other_palette, 0x3B, 0xA2, 0xF2, 0xDA);
	FILL_NES_PALETTE(other_palette, 0x3C, 0x99, 0xFF, 0xFC);
	FILL_NES_PALETTE(other_palette, 0x3D, 0xDD, 0xDD, 0xDD);
	FILL_NES_PALETTE(other_palette, 0x3E, 0x11, 0x11, 0x11);
	FILL_NES_PALETTE(other_palette, 0x3F, 0x11, 0x11, 0x11);

	system_palette = loopy_palette;
}

void dump_palette() {

	int i;

	printf("No | Picture       | Sprites\n");
	for(i=0;i!=16;i++) {
		printf("%2d | %02x           | %02x\n", i, *(PPU->VRAM + 0x3F00 + i), *(PPU->VRAM + 0x3F10 + i) );
	}

	return;
}
