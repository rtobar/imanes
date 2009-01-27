#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "debug.h"
#include "palette.h"
#include "ppu.h"
#include "screen.h"

nes_ppu *PPU;

void initialize_ppu() {

	PPU = (nes_ppu *)malloc(sizeof(nes_ppu));
	PPU->VRAM = (uint8_t *)malloc(NES_VRAM_SIZE);
	PPU->SPR_RAM = (uint8_t *)malloc(256);
	PPU->SR = VBLANK_FLAG;

}

void init_ppu_vram(ines_file *file) {

	/* Dump the VROM into the PPU VRAM area */
	if( file->vromBanks == 1 ) {
		printf("Copying VROM to VRAM\n");
		memcpy( PPU->VRAM , file->vrom, 0x2000);
	}

}

void draw_screen() {
	
	/* Let's checkout the name tables */
	int i;
	int j;
	int pix;
	int piy;
	int tile_number;
	uint8_t col_index;
	uint8_t byte;
	uint8_t *name_table;
	uint8_t tile;

	name_table = PPU->VRAM + 0x2000 + 0x400*(PPU->CR1 & 0x03);

	//INFO( printf("\nStarting dumping of nametable at 0x%04x\n", name_table - PPU->VRAM) );
	//INFO( for(i=0;i!=20;i++) {
	//	for(j=0;j!=16;j++)
	//		printf("%02x ",*(name_table+i*16+j));
	//	printf("\n");
	//} );

	for(j=0;j!=NES_SCREEN_HEIGHT/8;j++) {

		for(i=0;i!=NES_SCREEN_WIDTH/8;i++) {

			/* Get the 8x8 pixel to be drawn */
			tile_number = *(name_table + i + j*NES_SCREEN_HEIGHT/8);
			tile = *(name_table + tile_number*0x10);
			if( tile != 0 )
				printf("Tile is %d\n",tile);

			for(piy=0;piy!=8;piy++) {

				byte = *(PPU->VRAM + tile + piy) || (*(PPU->VRAM + tile+piy+1) << 1);
				for(pix=0;pix!=8;pix++) {
					col_index = (byte >> (7-pix)) & 0x4;
					draw_pixel(i*8+pix, j*8+piy,
					           system_palette[col_index].red, 
					           system_palette[col_index].green,
					           system_palette[col_index].blue);
				}
			}
		}
	}

	redraw_screen();
}

void draw_line() {

	static unsigned int line = 0;

	int i;
	int j;
	int pix;
	int piy;
	int tile_number;
	uint8_t col_index;
	uint8_t byte;
	uint8_t *name_table;
	uint8_t tile;

	/* Name table depends on the 1st and 2nd bit of PPY CR1 */
	name_table = PPU->VRAM + 0x2000 + 0x400*(PPU->CR1 & 0x03);

	j = line >> 3; /* line/8 */
	for(i=0;i!=NES_SCREEN_WIDTH/8;i++) {

		/* Get the 8x8 pixel to be drawn */
		tile_number = *(name_table + i + j*NES_SCREEN_HEIGHT/8);
		tile = *(name_table + tile_number*0x10);

		piy = line & 0x07; /* piy = line % 8 */

		byte = *(PPU->VRAM + tile + piy) || (*(PPU->VRAM + tile+piy+1) << 1);
		for(pix=0;pix!=8;pix++) {
			col_index = (byte >> (7-pix)) & 0x4;
			draw_pixel(i*8+pix, j*8+piy,
			           system_palette[col_index].red, 
			           system_palette[col_index].green,
			           system_palette[col_index].blue);
		}
	}

	redraw_screen();

	line++;
	if( line >= NES_SCREEN_HEIGHT )
		line = 0;
}
