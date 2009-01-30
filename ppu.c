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
		INFO( printf("Copying VROM to VRAM\n") );
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
					draw_pixel(i*8+pix, j*8+piy, system_palette[col_index]);
				}
			}
		}
	}

	redraw_screen();
}

void draw_line() {

	static unsigned int line = 0;

	int i;
	int tx; /* X coord inside a tile */
	int ty; /* Y coord inside a tile */
	int tmp;
	int big_sprite;
	int sprites;
	uint8_t drawable_sprites[8];
	uint8_t col_index;
	uint8_t byte1;
	uint8_t byte2;
	uint8_t byte3;
	uint8_t *name_table;
	uint8_t *attr_table;
	uint8_t *scr_patt_table;
	uint8_t *spr_patt_table;
	uint8_t tile;

	/* Name table depends on the 1st and 2nd bit of PPU CR1 */
	name_table     = PPU->VRAM + 0x2000 + 0x400*(PPU->CR1 & 0x03);
	attr_table     = name_table + 0x3C0;
	scr_patt_table = PPU->VRAM + ((PPU->CR1&SCR_PATTERN_ADDRESS)>>3)*0x1000;
	spr_patt_table = PPU->VRAM + ((PPU->CR1&SPR_PATTERN_ADDRESS)>>4)*0x1000;


	/* Identify which sprites have to be drawn */
	/* If 8x16 sprites, we check 32 sprites instead of 64 */
	big_sprite = (PPU->CR1 & SPRITE_SIZE_8x16)>>5;
	sprites = 0;
	for(i=0;i!=64/(big_sprite+1);i++) {
		tmp = *(PPU->SPR_RAM + 4*i*(big_sprite+1));
		if( tmp <= line && line <= tmp+8*(big_sprite+1) ) {
			drawable_sprites[sprites++] = i;
			if( sprites == 8 ) {
				PPU->SR |= MAX_SPRITES_DRAWN;
				break;
			}
		}
	}
	sprites--;


	ty = line & 0x07; /* ty = line % 8 */
	/* Draw the background */
	for(i=0;i!=NES_SCREEN_WIDTH/8;i++) {

		/* Get the 8x8 pixel table where the line is present */
		tile = *(name_table + i + (line >> 3)*NES_SCREEN_WIDTH/8);

		/* Bytes that participate on the lower bits for the color */
		byte1 = *(scr_patt_table + tile*0x10 + ty);
		byte2 = *(scr_patt_table + tile*0x10 + ty + 0x08);
		/* Byte participating on the higher bits for the color */
		byte3 = *(attr_table + (i >> 2) + (line >> 5)*NES_SCREEN_WIDTH/8);

		for(tx=0;tx!=8;tx++) {
			/* This is from the pattern table */
			col_index = ((byte1>>(7-tx))&0x1) | (((byte2>>(7-tx))&0x1)<<1);

			/* And this from the attribute table */
			tmp = 2*((line >> 4)&0x1) + ((i >> 1)&0x1);
			col_index |=  ((byte3 >> 2*tmp)&0x03) << 2;

			draw_pixel(i*8+tx, line, system_palette[*(PPU->VRAM + 0x3F00 + col_index)]);
		}
	}

	/* Draw the sprites */
	/* TODO: This shouldn't be like this, it's temporal only */
	for(i=sprites;i>=0;i--) {

		/* 8x8 sprites */
		if(!big_sprite) {
			ty = line - *(PPU->SPR_RAM + 4*drawable_sprites[i]);
			tile = *(PPU->SPR_RAM + 4*drawable_sprites[i] + 1);
			tmp  = *(PPU->SPR_RAM + 4*drawable_sprites[i] + 3); /* X origin */
			byte1 = *(spr_patt_table + tile*0x10 + ty);
			byte2 = *(spr_patt_table + tile*0x10 + ty + 0x08);
			byte3 = *(PPU->SPR_RAM + 4*drawable_sprites[i] + 2);
			for(tx=0;tx!=8;tx++) {
				col_index = ((byte1>>(7-tx))&0x1) | (((byte2>>(7-tx))&0x1)<<1);
				col_index |=  (byte3&0x03) << 2;

				draw_pixel( tmp + tx , line, system_palette[*(PPU->VRAM + 0x3F10 + col_index)]);
			}
			
		}

		else {
			fprintf(stderr,"Still not implemented :(\n");
		}
	}

	redraw_screen();

	line++;
	if( line >= NES_SCREEN_HEIGHT )
		line = 0;
}
