/*  ImaNES: I'm a NES. An intelligent NES emulator

    mmc3.c   -    MMC3 Mapper emulation under ImaNES

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
#include "mmc3.h"
#include "ppu.h"

void mmc3_initialize_mapper() {

	mapper->regs = (uint8_t *)malloc(8);
	bzero(mapper->regs,8);
	return;
}

int  mmc3_check_address(uint16_t address) {

	if( address == 0x8000 ) {
		mapper->regs[0] = CPU->RAM[address];
		return 1;
	}

	if( address == 0x8001 ) {
		mapper->regs[1] = CPU->RAM[address];
		return 1;
	}

	if( address == 0xA000 ) {
		mapper->regs[2] = CPU->RAM[address];
		return 1;
	}

	if( address == 0xA001 ) {
		mapper->regs[3] = CPU->RAM[address];
		return 1;
	}

	if( address == 0xC000 ) {
		mapper->regs[4] = CPU->RAM[address];
		return 1;
	}

	if( address == 0xC001 ) {
		mapper->regs[5] = CPU->RAM[address];
		return 1;
	}

	if( address == 0xE000 ) {
		mapper->regs[6] = CPU->RAM[address];
		return 1;
	}

	if( address == 0xE001 ) {
		mapper->regs[7] = CPU->RAM[address];
		return 1;
	}

	return 0;
}

void mmc3_switch_banks() {

	return;
}

void mmc3_reset() {

	return;
}
