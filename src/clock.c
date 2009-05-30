/*  ImaNES: I'm a NES. An intelligent NES emulator

    clock.c   -    NES clock reference under ImaNES

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

#include "clock.h"

nes_clock *CLK;

void initialize_clock() {

	CLK = (nes_clock *)malloc(sizeof(nes_clock));

	CLK->cpu_cycles  = 0;
	CLK->nmi_ccycles = 0;
	CLK->ppu_cycles  = 0;
	CLK->nmi_pcycles = 0;

}

void dump_clock() {

	printf("CPU --- Total: %010ld    NMI: %05d       ", CLK->cpu_cycles, CLK->nmi_ccycles);
	printf("PPU --- Total: %010ld    NMI: %05d\n", CLK->ppu_cycles, CLK->nmi_pcycles);

}
