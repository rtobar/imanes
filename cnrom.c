#include <stdio.h>
#include <stdlib.h>

#include "cnrom.h"
#include "cpu.h"
#include "debug.h"
#include "mapper.h"
#include "ppu.h"

void cnrom_initialize_mapper() {
	mapper->regs = (uint8_t *)malloc(1);
	return;
}

int cnrom_check_address(uint16_t address) {

	/* It is not necessary to check <= 0xFFFF because of the data range
	 * of a uint16_t :) */
	if( 0x8000 <= address ) {
		mapper->regs[0] = CPU->RAM[address];
		return 1;
	}

	return 0;
}

void cnrom_switch_banks() {	

	/* Copy the VROM bank to the 0x0000 of VRAM */
	DEBUG( printf("Performing switch to bank %d of VROM\n", mapper->regs[0]));
	memcpy(PPU->VRAM, mapper->file->vrom + mapper->regs[0]*VROM_BANK_SIZE,
	       VROM_BANK_SIZE);

}

void cnrom_reset() 
{
	memcpy(CPU->RAM+0x8000, mapper->file->rom, ROM_BANK_SIZE);
	memcpy(CPU->RAM+0xC000,
	       mapper->file->rom + (mapper->file->romBanks-1)*ROM_BANK_SIZE, 
	       ROM_BANK_SIZE);
}
