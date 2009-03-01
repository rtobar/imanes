#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"
#include "debug.h"
#include "mapper.h"
#include "mmc1.h"

void mmc1_initialize_mapper() {

	mapper->regs = (uint8_t *)malloc(4);
	bzero(mapper->regs,4);

}

int  mmc1_check_address(uint16_t address) {

	int i;
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
		if( shifts[0] == 5 ) {
			shifts[0] = 0;
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
		if( shifts[1] == 5 ) {
			shifts[1] = 0;
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
		if( shifts[2] == 5 ) {
			shifts[2] = 0;
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
		if( shifts[3] == 5 ) {
			shifts[3] = 0;
			touched = 1;
		}
	}

	if( 0x8000 <= address )
		for(i=0;i!=4;i++)
		printf("reg[%d]:%02x  shift[%d]:%d  address:%04x  value:%02x\n", i, mapper->regs[i], i, shifts[i], address, CPU->RAM[address]);

	return touched;
}

void mmc1_switch_banks() {

	return;
}

void mmc1_reset() {

	memcpy(CPU->RAM+0x8000, mapper->file->rom, ROM_BANK_SIZE);
	memcpy(CPU->RAM+0xC000, 
	       mapper->file->rom + (mapper->file->romBanks-1)*ROM_BANK_SIZE, 
	       ROM_BANK_SIZE);

}
