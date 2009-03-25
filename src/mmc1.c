/*  ImaNES: I'm a NES. An intelligent NES emulator

    mmc1.c   -    MMC1 Mapper emulation under ImaNES

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

#include "cpu.h"
#include "debug.h"
#include "mapper.h"
#include "mmc1.h"
#include "ppu.h"

static int touched_reg;

void mmc1_initialize_mapper() {

	mapper->regs = (uint8_t *)malloc(4);
	bzero(mapper->regs,4);

	mapper->regs[0] = 0x04; /* Swap 0x8000 by default */
	return;
}

int  mmc1_check_address(uint16_t address) {

	uint8_t value;
	static uint8_t shifts = 0;
	static int saved = 0;

	/* Save the entering value */
	if( 0x8000 <= address ) {

		value = CPU->RAM[address];

		if( value & 0x80 ) {
			shifts = 0;
			saved = 0;
			return 0;
		}

		saved |= (value&0x01) << shifts++;
		printf("MMC1: saved:%02x\n", saved);

		/* Last write is the important */
		if( shifts == 5 ) {

			if( 0x8000 <= address && address < 0xA000 )
				mapper->regs[0] = saved;

			else if( 0xA000 <= address && address < 0xC000 )
				mapper->regs[1] = saved;

			else if( 0xC000 <= address && address < 0xE000 )
				mapper->regs[2] = saved;

			else if( 0xE000 <= address )
				mapper->regs[3] = saved;

			touched_reg = (address-0x8000) >> 13;
			shifts = 0;
			saved = 0;
			return 1;
		}
	}


	return 0;
}

void mmc1_switch_banks() {

	int i;
	int bank;
	uint32_t offset = 0;

	DEBUG( printf("\nMMC1: ");
	for(i=0;i!=4;i++)
		printf("reg[%d]:%02x ", i, mapper->regs[i]);
	printf("\n");
	);

	/* Reg 0 only has bank switching options,
	 * but changes the mirroring type */
	if( touched_reg == 0 ) {
		PPU->mirroring = !(mapper->regs[0] & 0x01);
		if( !(mapper->regs[0] & 0x02 ) )
			PPU->mirroring = SINGLE_SCREEN_MIRRORING;
		return;
	}

	DEBUG( printf("MMC1: Switching banks...\n") );
	/* We have three major options:
	 *  1) VROM packed catridges
	 *  2) 512 Kb packed catridges
	 *  3) 1024 Kb packed catridges.
	 *
	 * This distinction is for the usage of bit 4 of register 0
	 * and the usage of register 1 and 2. In the first case they are
	 * used to switch VROM banks, while in the second to help in the
	 * choose of the appropiate ROM bank to switch */

	/* VROM packed roms */
	if( mapper->file->vromBanks != 0 && (touched_reg == 1 || touched_reg == 2) ) {

		/* Switch VROM banks. Banks sizes can be 8 Kb or 4 Kb.
		 * In both cases we fill form 0x0000 to 0x2000. */
		if( !(mapper->regs[0] & 0x10) ) {
			bank = mapper->regs[1]&0x0F;
			DEBUG( printf("MMC1: Switching 8 Kb VROM bank %d. Offset is ",  bank) );
			offset = bank * VROM_BANK_SIZE;
			DEBUG( printf("%04x\n", offset) );
			memcpy( PPU->VRAM, mapper->file->vrom+offset, VROM_BANK_SIZE);
		}
		else {
			bank = mapper->regs[1]&0x0F;
			DEBUG( printf("MMC1: Switching 4 Kb VROM banks %d/%d. Offsets are ", bank, mapper->regs[2]&0x0F) );

			offset = bank * VROM_BANK_SIZE/2;
			DEBUG( printf("%04x/", offset) );
			memcpy( PPU->VRAM, mapper->file->vrom + offset,
			        VROM_BANK_SIZE/2);
			bank = (mapper->regs[2] & 0x0F);
			offset = bank * VROM_BANK_SIZE/2;
			DEBUG( printf("%04x\n", offset) );
			memcpy( PPU->VRAM+0x1000, mapper->file->vrom + offset,
			        VROM_BANK_SIZE/2);
		}

		offset = 0;
	}

	if( touched_reg == 0 || touched_reg == 3 ) {

		offset = 0;
		/* 512 Kb roms */
		if( mapper->file->romBanks == 32 && (mapper->regs[1] & 0x10) )
			offset = 0x40000;
	
		/* 1024 Kb roms */
		else if( mapper->file->romBanks == 64 ) {
			if( !(mapper->regs[0] & 0x10 ) && mapper->regs[1] & 0x10 )
				offset = 0x80000;
	
			else if( mapper->regs[0] & 0x10 )
				offset = 0x40000 * ( (mapper->regs[1]&0x10) | (mapper->regs[2]&0x10) );
		}
	
		/* Switch ROM banks. If swap 32 Kb, fill from 0x8000, 
		 * if 16 Kb, check which should be fill */
		if( !(mapper->regs[0] & 0x08 ) ) {
	
			/* Select the actual bank that will be switched */
			bank = (mapper->regs[3] & 0x0F);
			offset += bank * ROM_BANK_SIZE*2;
			printf("MMC1: Switching 32 Kb ROM bank %d and offset %04x to 0x8000\n", bank, offset);
			memcpy( CPU->RAM+0x8000, mapper->file->rom + offset,
			        ROM_BANK_SIZE*2);
		}
		else {
			bank = (mapper->regs[3] & 0x0F);
			offset += bank * ROM_BANK_SIZE;
			printf("MMC1: Switching 16 Kb ROM bank %d and offset %04x to %04x\n", bank, offset, 0x8000 + (mapper->regs[0]&0x04?0:0x4000));
			memcpy( CPU->RAM+0x8000 + ( mapper->regs[0]&0x04 ? 0 : 0x4000),
			        mapper->file->rom + offset, ROM_BANK_SIZE);
		}
	}

}

void mmc1_reset() {

	memcpy(CPU->RAM+0x8000, mapper->file->rom, ROM_BANK_SIZE);
	memcpy(CPU->RAM+0xC000, 
	       mapper->file->rom + (mapper->file->romBanks-1)*ROM_BANK_SIZE, 
	       ROM_BANK_SIZE);

}

void mmc1_update() {
	return;
}
