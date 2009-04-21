/*  ImaNES: I'm a NES. An intelligent NES emulator

    states.c   -    Internal states of emulation for ImaNES

    Copyright (C) 2009   Rodrigo Tobar Carrizo

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

#include <stdlib.h>

#include "imaconfig.h"
#include "states.h"

void load_state(int i) {

	char *user_imanes_dir;

	user_imanes_dir = get_user_imanes_dir();

	free(user_imanes_dir);
	return;
}

void save_state(int i) {

	char *user_imanes_dir;
	void *buffer, *buffer_start;

	user_imanes_dir = get_imanes_dir(States);

	/* Memory allocation for state information */
	buffer = malloc(
	/* CPU registers*/  7 + sizeof(unsigned long long) +
	/* RAM dump */      0x10000 +
	/* PPU registers */ 12 + sizeof(unsigned int) +
	/* VRAM dump */     0x4000 +
	/* SPR-RAM dump */  0x100 +
	/* Mapper */        1 + sizeof(unsigned int) + mapper->reg_count 
	         );
	buffer_start = buffer;

	/* CPU dumping */
	memcpy(buffer, &(CPU->A),  1); buffer++;
	memcpy(buffer, &(CPU->X),  1); buffer++;
	memcpy(buffer, &(CPU->Y),  1); buffer++;
	memcpy(buffer, &(CPU->SP), 1); buffer++;
	memcpy(buffer, &(CPU->SR), 1); buffer++;
	memcpy(buffer, &(CPU->PC), 2); buffer += 2;
	memcpy(buffer, &(CPU->cycles), sizeof(unsigned long long));
   buffer += sizeof(unsigned long long);

	/* RAM dumping */
	memcpy(buffer, CPU->RAM, 0x10000);
	buffer += 0x10000;

	/* PPU dumping */
	memcpy(buffer, &(PPU->CR1), 1);       buffer++;
	memcpy(buffer, &(PPU->CR2), 1);       buffer++;
	memcpy(buffer, &(PPU->SR), 1);        buffer++;
	memcpy(buffer, &(PPU->mirroring), 1); buffer++;
	memcpy(buffer, &(PPU->x), 1);         buffer++;
	memcpy(buffer, &(PPU->latch), 1);     buffer++;
	memcpy(buffer, &(PPU->vram_addr), 2); buffer += 2;
	memcpy(buffer, &(PPU->temp_addr), 2); buffer += 2;
	memcpy(buffer, &(PPU->spr_addr), 2);  buffer += 2;
	memcpy(buffer, &(PPU->lines), sizeof(unsigned int));
	buffer += sizeof(unsigned int);

	/* VRAM dumping */
	memcpy(buffer, PPU->VRAM, 0x4000);
	buffer += 0x4000;

	/* SPR-RAM dumping */
	memcpy(buffer, PPU->SPR_RAM, 0x100);
	buffer += 0x100;

	/* Mapper dumping */
	memcpy(buffer, &(mapper->id), 1);  buffer++;
	memcpy(buffer, &(mapper->reg_count), sizeof(unsigned int));
	buffer += sizeof(unsigned int);
	memcpy(buffer, mapper->regs, mapper->reg_count);
	buffer += mapper->reg_count;

	free(user_imanes_dir);
	return;
}
