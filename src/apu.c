/*  ImaNES: I'm a NES. An intelligent NES emulator

    apu.c   -    APU emulation under ImaNES

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

#include <stdio.h>
#include <stdlib.h>

#include "apu.h"
#include "debug.h"

nes_apu *APU;

void initialize_apu() {

	APU = (nes_apu *)malloc(sizeof(nes_apu));

	return;
}

void dump_apu() {

	printf("0x4015:%02x  ", APU->length_ctr);
	printf("0x4017:%02x  ", APU->commons);

	return;
}

void end_apu() {

	if( APU != NULL )
		free(APU);
}
