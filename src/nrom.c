/*  ImaNES: I'm a NES. An intelligent NES emulator

    nrom.c   -    NROM Mapper emulation under ImaNES

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
#include "nrom.h"
#include "ppu.h"

void nrom_initialize_mapper() {
	return;
}

int nrom_check_address(uint16_t address) {
	return 0;
}

/* This shouldn't be called never, anyways */
void nrom_switch_banks() {
	return;
}

void nrom_reset()
{
   /* 1 ROM bank games load twice to ensure vector tables */
   /* Free the file ROM (we don't need it anymore) */
   if( mapper->file->romBanks == 1 ) {
      memcpy( CPU->RAM + 0x8000, mapper->file->rom, ROM_BANK_SIZE);
      memcpy( CPU->RAM + 0xC000, CPU->RAM + 0x8000, ROM_BANK_SIZE);
   }
   /* 2 ROM bank games load one in 0x8000 and other in 0xC000 */
   /* Free the file ROM (we don't need it anymore) */
   else if (mapper->file->romBanks == 2 ) {
      memcpy( CPU->RAM + 0x8000, mapper->file->rom, ROM_BANK_SIZE);
      memcpy( CPU->RAM + 0xC000, mapper->file->rom + ROM_BANK_SIZE, ROM_BANK_SIZE);
   }

	/* Dump the VROM into the PPU VRAM area */
	if( mapper->file->vromBanks == 1 ) {
		INFO( printf("Copying VROM to VRAM\n") );
		memcpy( PPU->VRAM , mapper->file->vrom, 0x2000);
	}

	return;
}

void nrom_update() {
	return;
}
