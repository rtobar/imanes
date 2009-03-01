#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"
#include "debug.h"
#include "mapper.h"
#include "unrom.h"

void unrom_initialize_mapper() {
	mapper->regs = (uint8_t *)malloc(1);
	return;
}

int unrom_check_address(uint16_t address) {

	/* It is not necessary to check <= 0xFFFF because of the data range
	 * of a uint16_t :) */
	if( 0x8000 <= address ) {
		mapper->regs[0] = CPU->RAM[address];
		return 1;
	}

	return 0;
}

void unrom_switch_banks() 
{

	DEBUG( printf("Performing bank switching: Switching to bank %d of ROM\n",mapper->regs[0]) );
	memcpy(CPU->RAM+0x8000, mapper->file->rom + mapper->regs[0]*ROM_BANK_SIZE,
	       ROM_BANK_SIZE);
}

void unrom_reset()
{

	memcpy(CPU->RAM+0x8000, mapper->file->rom, ROM_BANK_SIZE);
	memcpy(CPU->RAM+0xC000, 
	       mapper->file->rom + (mapper->file->romBanks-1)*ROM_BANK_SIZE, 
	       ROM_BANK_SIZE);
}
