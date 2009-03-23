/*  ImaNES: I'm a NES. An intelligent NES emulator

    ppu.c   -    PPU emulation under ImaNES

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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "debug.h"
#include "imaconfig.h"
#include "palette.h"
#include "ppu.h"
#include "screen.h"

nes_ppu *PPU;

void initialize_ppu() {

	PPU = (nes_ppu *)malloc(sizeof(nes_ppu));
	PPU->VRAM = (uint8_t *)malloc(NES_VRAM_SIZE);
	PPU->SPR_RAM = (uint8_t *)malloc(256);

	PPU->x = 0;
	PPU->latch = 1;
	PPU->vram_addr = 0;
	PPU->temp_addr = 0;
}

void dump_ppu() {

	printf("CR1:%02x  ", PPU->CR1);
	printf("CR2:%02x  ", PPU->CR2);
	printf("SR:%02x  ", PPU->SR);
	printf("VADDR:%04x\n", PPU->vram_addr);
	printf("SPRADDR:%04x\n", PPU->spr_addr);
	printf("Offsets: %03u,%03u\n", PPU->h_offset, PPU->v_offset);

}

void draw_line(int line) {

	int x;  /* Final x pixel coordinate */
	int y;  /* Final y pixel coordinate */
	int i;
	int j;
	int tx; /* X coord inside a tile */
	int ty; /* Y coord inside a tile */
	int tmp;
	int big_sprite;
	int bck_sprites; /* Counters for arrays bellow */
	int frt_sprites;
	int second_sprite; /* For 8x16 sprites */
	int first_bg_pixel;            /* For Sprite #0 hit flag */
	int drawn_back_sprites_idx;    /* For Sprite #0 hit flag */
	uint8_t drawn_back_sprites[8]; /* For Sprite #0 hit flag */
	uint8_t front_sprites[8];
	uint8_t back_sprites[8];
	uint8_t col_index;
	uint8_t byte1;
	uint8_t byte2;
	uint8_t byte3;
	uint8_t tile;
	uint16_t attr_table;
	uint16_t name_table;
	uint16_t orig_name_table;
	uint16_t spr_patt_table;
	uint16_t scr_patt_table;

	/* Name table depends on the 1st and 2nd bit of PPU CR1 */
	orig_name_table = 0x2000 + 0x400*(PPU->CR1 & 0x03);
	spr_patt_table  = ((PPU->CR1&SPR_PATTERN_ADDRESS)>>3)*0x1000;
	scr_patt_table  = ((PPU->CR1&SCR_PATTERN_ADDRESS)>>4)*0x1000;
	big_sprite      = (PPU->CR1 & SPRITE_SIZE_8x16)>>5;


	/* Update PPU registers */
	if( PPU->CR2 & (SHOW_BACKGROUND|SHOW_SPRITES) )
		if( line == 0 )
			PPU->vram_addr = PPU->temp_addr;
		PPU->vram_addr = (PPU->vram_addr&0xFBE0) | (PPU->temp_addr&0x041F);

	/* Identify which sprites have to be drawn */
	frt_sprites = 0;
	bck_sprites = 0;
	for(i=0;i!=64;i++) {
		tmp = *(PPU->SPR_RAM + 4*i) + 1;
		if( tmp <= line && line < tmp+8*(big_sprite+1) ) {
			if( *(PPU->SPR_RAM + 4*i + 2) & SPRITE_BACK_PRIOR )
				back_sprites[bck_sprites++] = i;
			else
				front_sprites[frt_sprites++] = i;
			if( (frt_sprites + bck_sprites) == 8 )
				break;
		}
	}
	if( (frt_sprites + bck_sprites) != 8 )
		PPU->SR &= ~MAX_SPRITES_DRAWN;
	else
		PPU->SR |= MAX_SPRITES_DRAWN;
	frt_sprites--;
	bck_sprites--;


	/* Fill all pixels with the "colour intensity" color */
	if( config.show_screen_bg ) {
		for(i=0;i!=NES_SCREEN_WIDTH;i++)
			draw_pixel(i, line, system_palette[*(PPU->VRAM + 0x3F00 )]);
	}
	else {
		for(i=0;i!=NES_SCREEN_WIDTH;i++)
			draw_pixel(i, line, system_palette[0]);
	}


	/* Draw the back sprites */
	drawn_back_sprites_idx = 0;
	if( config.show_back_spr && PPU->CR2&SHOW_SPRITES ) {
		for(i=bck_sprites;i>=0;i--) {

			/* Here we have color index and h/v flip */
			byte3 = *(PPU->SPR_RAM + 4*back_sprites[i] + 2);

			/* y coord. If V Flip... */
			ty = line - *(PPU->SPR_RAM + 4*back_sprites[i]) - 1;
			if( byte3 & SPRITE_FLIP_VERT )
				ty = 7 - ty;

			tile = *(PPU->SPR_RAM + 4*back_sprites[i] + 1);
			tmp  = *(PPU->SPR_RAM + 4*back_sprites[i] + 3); /* X origin */

			/* 8x16 sprites patter table depends on i being even or not */
			second_sprite = 0;
			if( big_sprite ) {
				spr_patt_table = 0x1000*(tile&0x1);
				tile &= 0xFE;
				if( ty >= 8 ) {
					ty -= 8;
					second_sprite = 1;
				}
			}

			byte1 = read_ppu_vram(spr_patt_table+(tile+second_sprite)*0x10 + ty);
			byte2 = read_ppu_vram(spr_patt_table+(tile+second_sprite)*0x10 + ty + 0x08);
			for(tx=0;tx!=8;tx++) {
				col_index = ((byte1>>(7-tx))&0x1) | (((byte2>>(7-tx))&0x1)<<1);
				col_index |=  (byte3&0x03) << 2;

				/* Don't draw background colors! */
				if( col_index & 0x03 ) {

					/* Horizontal flip? */
					if( byte3 & SPRITE_FLIP_HORIZ ) {
						x = tmp+7-tx;
						if( 0 <= x && x < NES_SCREEN_WIDTH ) {
							draw_pixel( x, line, system_palette[read_ppu_vram(0x3F10+col_index)]);
							if( back_sprites[i] == 0 )
								drawn_back_sprites[drawn_back_sprites_idx++] = x;
						}
					}
					else {
						x = tmp+tx;
						if( x < NES_SCREEN_WIDTH ) {
							draw_pixel( x, line, system_palette[read_ppu_vram(0x3F10+col_index)]);
							if( back_sprites[i] == 0 )
								drawn_back_sprites[drawn_back_sprites_idx++] = x;
						}
					}
				}

			}
		}
	}

	/* Draw the background tiles
	 * For this we have to consider the horizontal and vertical
	 * scrolling. Based on this, we choose the name table where the
	 * tiles come from.
	 */
	first_bg_pixel = -1;
	if( PPU->CR2&SHOW_BACKGROUND ) {

		y = (PPU->vram_addr&0x03E0) >> 5;
		ty = (PPU->vram_addr&0x7000) >> 12;
		orig_name_table += (PPU->vram_addr&0x0800);

		for(x=0;x!=NES_SCREEN_WIDTH;) {

			/* Name table*/
			name_table = orig_name_table + (PPU->vram_addr&0x0400);
			attr_table = name_table + 0x3C0;

			/* Entry in name table */
			i = (PPU->vram_addr&0x1F);

			/* Get the 8x8 pixel tile where the line is present */
			tile = read_ppu_vram(name_table + i + y*NES_SCREEN_WIDTH/8);

			/* Bytes that participate on the lower bits for the color */
			byte1 = read_ppu_vram(scr_patt_table + tile*0x10 + ty);
			byte2 = read_ppu_vram(scr_patt_table + tile*0x10 + ty + 0x08);
			/* Byte participating on the higher bits for the color */
			byte3 = read_ppu_vram(attr_table + (i >> 2) + (y >> 2)*NES_SCREEN_WIDTH/32);

			/* Draw the tile pixels */
			for(tx=(x?0:PPU->x); tx!=8; tx++) {

				/* This is from the pattern table */
				col_index = ((byte1>>(7-tx))&0x1) | (((byte2>>(7-tx))&0x1)<<1);

				/* And this from the attribute table */
				tmp = (((y >> 1)&0x1)<<1) + ((i >> 1)&0x1);
				col_index |=  ((byte3 >> 2*tmp)&0x03) << 2;

				if( col_index & 0x03 ) {
					if( first_bg_pixel == -1 )
						first_bg_pixel = x;

					for(j=0;j!=drawn_back_sprites_idx && !(PPU->SR & HIT_FLAG);j++)
						if( x == drawn_back_sprites[j] ) {
							PPU->SR |= HIT_FLAG;
							break;
						}
					if( config.show_bg )
						draw_pixel(x, line, system_palette[read_ppu_vram(0x3F00+col_index)]);
				}
				x++;
				if( x == NES_SCREEN_WIDTH )
					break;
			}

			/* X scroll update*/
			if( i+1 == 0x20 ) {
				PPU->vram_addr &= 0xFFE0;
				PPU->vram_addr ^= 0x400;
			}
			else
				PPU->vram_addr++;

		}
	}

	/* Draw the front sprites */
	if( config.show_front_spr && PPU->CR2&SHOW_SPRITES ) {
		for(i=frt_sprites;i>=0;i--) {

			/* Here we have color index and h/v flip */
			byte3 = *(PPU->SPR_RAM + 4*front_sprites[i] + 2);

			/* y coord. If V Flip... */
			ty = line - *(PPU->SPR_RAM + 4*front_sprites[i]) - 1;
			if( byte3 & SPRITE_FLIP_VERT )
				ty = 7 - ty;

			tile = *(PPU->SPR_RAM + 4*front_sprites[i] + 1);
			tmp  = *(PPU->SPR_RAM + 4*front_sprites[i] + 3); /* X origin */

			/* 8x16 sprites patter table depends on i being even or not */
			second_sprite = 0;
			if( big_sprite ) {
				spr_patt_table = 0x1000*(tile&0x1);
				tile &= 0xFE;
				if( ty >= 8 ) {
					ty -= 8;
					second_sprite = 1;
				}
			}

			byte1 = read_ppu_vram(spr_patt_table+(tile+second_sprite)*0x10 + ty);
			byte2 = read_ppu_vram(spr_patt_table+(tile+second_sprite)*0x10 + ty + 0x08);
			for(tx=0;tx!=8;tx++) {
				col_index = ((byte1>>(7-tx))&0x1) | (((byte2>>(7-tx))&0x1)<<1);
				col_index |=  (byte3&0x03) << 2;

				/* Don't draw background colors! */
				if( col_index & 0x03 ) {

					/* Horizontal flip? */
					if( byte3 & SPRITE_FLIP_HORIZ ) {
						x = tmp+7-tx;
						if( 0 <= x && x < NES_SCREEN_WIDTH ) {
							if( !(PPU->SR&HIT_FLAG) && x == first_bg_pixel
							    && front_sprites[i] == 0)
								PPU->SR |= HIT_FLAG;

							draw_pixel( x, line,
							   system_palette[read_ppu_vram(0x3F10+col_index)]);
						}
					}
					else {
						x = tmp+tx;
						if( x < NES_SCREEN_WIDTH ) {
							if( !(PPU->SR&HIT_FLAG) && (x == first_bg_pixel)
							    && front_sprites[i] == 0)
								PPU->SR |= HIT_FLAG;

							draw_pixel( x, line,
							system_palette[read_ppu_vram(0x3F10+col_index)]);
						}
					}

				}

			}
		}
	}

	/* Y scroll update */
	ty = ((PPU->vram_addr&0x7000) >> 12) + 1;
	PPU->vram_addr = (PPU->vram_addr&0x8FFF) | ((ty&0x07)<<12);
	if( ty == 0x8 ) {
		printf("ty: %d, switching y to ", ty);
		y = ((PPU->vram_addr&0x03E0) >> 5) + 1;
		printf("%d\n", y);
		if( y == 30 ) {
			y = 0;
			PPU->vram_addr ^= 0x800;
		}
		else if( y == 32 ) {
			y = 0;
		}
		PPU->vram_addr = (PPU->vram_addr&0xFC1F) | ((y&0x1F)<<5);
	}

}

uint8_t read_ppu_vram(uint16_t address) {

	/* This duplicates everything else on VRAM */
	if( 0x4000 <= address ) {
		XTREME( printf("PPU Address mirroring: from %04x to ", address) );
		address = address - 0x4000 * ((address >> 14) & 0x3);
		XTREME( printf("%04x\n",address) );
	}

	/* After palette mirroring */
	if( 0x3F20 <= address && address < 0x4000) {
		XTREME( printf("PPU Address mirroring: from %04x to ", address) );
		address = address - 0x20 * ((((address - 0x3F20) >> 5) & 0x7) + 1);
		XTREME( printf("%04x\n",address) );
	}

	/* Name and attribute tables before palette  */
	if( 0x3000 <= address && address < 0x3F00 ) {
		XTREME( printf("PPU Address mirroring: from %04x to ", address) );
		address = address - 0x1000;
		XTREME( printf("%04x\n",address) );
	}

	/* Inter palette mirroring */
	if( 0x3F10 <= address && address < 0x3F20 && !(address&0x03) ) {
		XTREME( printf("Palette mirroring: from %04x to ", address) );
		address -= 0x10;
		XTREME( printf("%04x\n",address) );
	}

	/* Name table mirroring. This depends on the type of mirroring
	 * that the ines file header states */
	switch( PPU->mirroring ) {

		case HORIZONTAL_MIRRORING:
			if( (0x2400 <= address && address < 0x2800) ||
			    (0x2C00 <= address && address < 0x3000)) {
				DEBUG(printf("PPU Address mirroring: from %04x to ", address));
				address -= 0x400;
				DEBUG(printf("%04x\n",address));
			}
			break;

		case VERTICAL_MIRRORING:
			if( 0x2800 <= address && address < 0x3000 ) {
				DEBUG(printf("PPU Address mirroring: from %04x to ", address));
				address -= 0x800;
				DEBUG(printf("%04x\n",address));
			}
			break;

		case SINGLE_SCREEN_MIRRORING:
			if( 0x2400 <= address && address < 0x3000 ) {
				DEBUG(printf("PPU Address mirroring: from %04x to ", address));
				address -= 0x400*( ((address - 0x2000) >> 10) & 0x3);
				DEBUG(printf("%04x\n",address));
			}
			break;

		default:
			break;
	}

	return PPU->VRAM[address];
}

void write_ppu_vram(uint16_t address, uint8_t value) {

	/* This duplicates everything else on VRAM */
	if( 0x4000 <= address ) {
		XTREME( printf("PPU Address mirroring: from %04x to ", address) );
		address = address - 0x4000 * ((address >> 14) & 0x3);
		XTREME( printf("%04x\n",address) );
	}

	/* After palette mirroring */
	if( 0x3F20 <= address && address < 0x4000) {
		XTREME( printf("PPU Address mirroring: from %04x to ", address) );
		address = address - 0x20 * ((((address - 0x3F20) >> 5) & 0x7) + 1);
		XTREME( printf("%04x\n",address) );
	}

	/* Name and attribute tables before palette  */
	if( 0x3000 <= address && address < 0x3F00 ) {
		XTREME( printf("PPU Address mirroring: from %04x to ", address) );
		address = address - 0x1000;
		XTREME( printf("%04x\n",address) );
	}

	/* Inter palette mirroring */
	if( 0x3F10 <= address && address < 0x3F20 && !(address&0x03) ) {
		XTREME( printf("Palette mirroring: from %04x to ", address) );
		address -= 0x10;
		XTREME( printf("%04x\n",address) );
	}
	//if( 0x3F00 <= address && address < 0x3F10 && !(address&0x03) ) {
	//	XTREME( printf("Palette mirroring: from %04x to ", address) );
	//	address = address - 0x04*( (address & 0xFF) >> 2 );
	//	XTREME( printf("%04x\n",address) );
	//}

	/* Name table mirroring. This depends on the type of mirroring
	 * that the ines file header states */
	switch( PPU->mirroring) {

		case HORIZONTAL_MIRRORING:
			if( (0x2400 <= address && address < 0x2800) ||
			    (0x2C00 <= address && address < 0x3000)) {
				XTREME(printf("PPU Address mirroring: from %04x to ", address));
				address -= 0x400;
				XTREME(printf("%04x\n",address));
			}
			break;

		case VERTICAL_MIRRORING:
			if( 0x2800 <= address && address < 0x3000 ) {
				XTREME(printf("PPU Address mirroring: from %04x to ", address));
				address -= 0x800;
				XTREME(printf("%04x\n",address));
			}
			break;

		case SINGLE_SCREEN_MIRRORING:
			if( 0x2400 <= address && address < 0x3000 ) {
				XTREME(printf("PPU Address mirroring: from %04x to ", address));
				address -= 0x400*( ((address - 0x2000) >> 10) & 0x3);
				XTREME(printf("%04x\n",address));
			}
		default:
			break;
	}

	PPU->VRAM[address] = value;

	return;
}
