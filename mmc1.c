#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"
#include "debug.h"
#include "mapper.h"
#include "mmc1.h"
#include "ppu.h"

static int touched_regs[4];

void mmc1_initialize_mapper() {

	mapper->regs = (uint8_t *)malloc(4);
	bzero(mapper->regs,4);
	bzero(touched_regs,4);

	return;
}

int  mmc1_check_address(uint16_t address) {

	//int i;
	int touched = 0;
	uint8_t value;
	static uint8_t shifts[4] = {0, 0, 0, 0};

	if( 0x8000 <= address && address < 0xA000 ) {
		value = CPU->RAM[address];
		if( value & 0x80 ) {
			mapper->regs[0] = 0;
			shifts[0] = 0;
		}
		else
			mapper->regs[0] |= (value&0x01) << shifts[0]++;

		touched_regs[0] = 0;
		if( shifts[0] == 5 ) {
			shifts[0] = 0;
			touched_regs[0] = 1;
			touched = 1;
		}
	}

	else if( 0xA000 <= address && address < 0xC000 ) {
		value = CPU->RAM[address];
		if( value & 0x80 ) {
			mapper->regs[1] = 0;
			shifts[1] = 0;
		}
		else
			mapper->regs[1] |= (value&0x01) << shifts[1]++;

		touched_regs[1] = 0;
		if( shifts[1] == 5 ) {
			shifts[1] = 0;
			touched_regs[1] = 1;
			touched = 1;
		}
	}

	else if( 0xC000 <= address && address < 0xE000 ) {
		value = CPU->RAM[address];
		if( value & 0x80 ) {
			mapper->regs[2] = 0;
			shifts[2] = 0;
		}
		else
			mapper->regs[2] |= (value&0x01) << shifts[2]++;

		touched_regs[2] = 0;
		if( shifts[2] == 5 ) {
			shifts[2] = 0;
			touched_regs[2] = 1;
			touched = 1;
		}
	}

	else if( 0xE000 <= address ) {
		value = CPU->RAM[address];
		if( value & 0x80 ) {
			mapper->regs[3] = 0;
			shifts[3] = 0;
		}
		else
			mapper->regs[3] |= (value&0x01) << shifts[3]++;

		touched_regs[3] = 0;
		if( shifts[3] == 5 ) {
			shifts[3] = 0;
			touched_regs[3] = 1;
			touched = 1;
		}
	}

	return touched;
}

void mmc1_switch_banks() {

	int i;
	uint32_t offset = 0;

	DEBUG( printf("\nMMC1: ");
	for(i=0;i!=4;i++)
		printf("reg[%d]:%02x ", i, mapper->regs[i]);
	printf("\n");
	);

	/* First register has changed */
	//if( touched_regs[0] == 1 ) {
	if( 1 ) {

		touched_regs[0] = 0;

		DEBUG( printf("MMC1: Switching banks...\n") );

		PPU->mirroring = mapper->regs[0] & 0x01;
		if( !(mapper->regs[0] & 0x02 ) )
			PPU->mirroring = SINGLE_SCREEN_MIRRORING;

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
		if( mapper->file->vromBanks != 0 ) {

			/* Switch VROM banks. Banks sizes can be 8 Kb or 4 Kb.
			 * In both cases we fill form 0x0000 to 0x2000. */
			if( !(mapper->regs[0] & 0x10) ) {
				DEBUG( printf("MMC1: Switching 8 Kb VROM bank %d. Offset is ", mapper->regs[1]&0x0F) );
				offset = (mapper->regs[1] & 0x0F) * VROM_BANK_SIZE;
				DEBUG( printf("%04x\n", offset) );
				memcpy( PPU->VRAM, mapper->file->vrom+offset, VROM_BANK_SIZE);
				offset = 0;
			}
			else {
				DEBUG( printf("MMC1: Switching 4 Kb VROM banks %d/%d. Offsets are ", mapper->regs[1]&0x0F, mapper->regs[2]&0x0F) );

				offset = (mapper->regs[1] & 0x0F) * VROM_BANK_SIZE/2;
				DEBUG( printf("%04x/", offset) );
				memcpy( PPU->VRAM, mapper->file->vrom + offset,
				        VROM_BANK_SIZE/2);
				offset = (mapper->regs[2] & 0x0F) * VROM_BANK_SIZE/2;
				DEBUG( printf("%04x\n", offset) );
				memcpy( PPU->VRAM+0x1000, mapper->file->vrom + offset,
				        VROM_BANK_SIZE/2);
				offset = 0;
			}
		}

		/* 512 Kb roms */
		else if( mapper->file->romBanks == 32 && (mapper->regs[1] & 0x10) )
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
			offset += (mapper->regs[3] & 0x0F) * ROM_BANK_SIZE/2;
			DEBUG( printf("MMC1: Switching 32 Kb ROM bank %d and offset %04x to 0x8000\n", offset/ROM_BANK_SIZE, offset) );
			memcpy( CPU->RAM+0x8000, mapper->file->rom + offset,
			        ROM_BANK_SIZE*2);
		}
		else {
			offset += (mapper->regs[3] & 0x0F) * ROM_BANK_SIZE;
			DEBUG( printf("MMC1: Switching 16 Kb ROM bank %d and offset %04x to %04x\n", offset/ROM_BANK_SIZE, offset, 0x8000 + (mapper->regs[0]&0x04?0:0x4000)) );
			memcpy( CPU->RAM+0x8000 + ( mapper->regs[0]&0x04 ? 0 : 0x4000),
			        mapper->file->rom + offset, ROM_BANK_SIZE);
		}
	}

	return;
}

void mmc1_reset() {

	memcpy(CPU->RAM+0x8000, mapper->file->rom, ROM_BANK_SIZE);
	memcpy(CPU->RAM+0xC000, 
	       mapper->file->rom + (mapper->file->romBanks-1)*ROM_BANK_SIZE, 
	       ROM_BANK_SIZE);

}
