#include <stdlib.h>

#include "mapper.h"
#include "cpu.h"

int unrom_check_address(uint16_t address) {

	if( 0x8000 <= address && address <= 0xFFFF ) {
		mapper->reg1 = CPU->RAM[address];
		return 1;
	}

	return 0;
}

void unrom_switch_banks(ines_file *file) {
	//memcpy();
}

void unrom_reset(ines_file *file)
{
	memcpy(CPU->RAM+0x8000, file->rom, ROM_BANK_SIZE);
	memcpy(CPU->RAM+0xC000, 
			file->rom+file->(romBanks-1)*ROM_BANK_SIZE, ROM_BANK_SIZE);
}
