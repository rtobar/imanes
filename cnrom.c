#include <stdlib.h>

#include "cnrom.h"
#include "cpu.h"
#include "ppu.h"

int cnrom_check_address(uint16_t address) {

	if( 0x8000 <= address && address < 0xFFFF ) {
		mapper->reg1 = CPU->RAM[address];
		return 1;
	}

	return 0;
}

void cnrom_switch_banks() {
	
	/* Copy the VROM bank to the 0x0000 of VRAM */
	memcpy(PPU->VRAM, mapper->file->vrom + mapper->reg1*VROM_BANK_SIZE , VROM_BANK_SIZE);

}
