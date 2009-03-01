#include <stdlib.h>

#include "cpu.h"
#include "debug.h"
#include "mapper.h"
#include "mmc1.h"

void mmc1_initialize_mapper() {

	mapper->regs = (uint8_t *)malloc(4);
}

int  mmc1_check_address(uint16_t address) {

	if( 0x8000 <= address && address < 0xA000 )
		mapper->regs[0] = CPU->RAM[address];
	else if( 0xA000 <= address && address < 0xC000 )
		mapper->regs[1] = CPU->RAM[address];
	else if( 0xC000 <= address && address < 0xE000 )
		mapper->regs[2] = CPU->RAM[address];
	else if( 0xE000 <= address )
		mapper->regs[3] = CPU->RAM[address];

	return 0;
}

void mmc1_switch_banks() {

}

void mmc1_reset() {

	memcpy(CPU->RAM+0x8000, mapper->file->rom, ROM_BANK_SIZE);
	memcpy(CPU->RAM+0xC000, 
	       mapper->file->rom + (mapper->file->romBanks-1)*ROM_BANK_SIZE, 
	       ROM_BANK_SIZE);

}
