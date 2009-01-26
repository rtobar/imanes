#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

	name_table = PPU->VRAM + 0x2000;

	DEBUG( for(i=0;i!=20;i++) {
		for(j=0;j!=16;j++)
			printf("%02x ",*(name_table+i*16+j));
		printf("\n");
	} );

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
					col_index = (byte >> (8-pix)) & 0x4;
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
