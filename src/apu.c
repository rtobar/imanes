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

	APU->length_ctr = 0;
	APU->commons = 0;
	APU->step = 0;
	APU->clock_timeout = PPUCYCLES_STEP4;

	return;
}

void dump_apu() {

	printf("0x4015:%02x  ", APU->length_ctr);
	printf("0x4017:%02x  ", APU->commons);

	return;
}

void clock_apu_sequencer() {

	/* Reset the clock timeout depending on the sequencer mode */
	APU->clock_timeout += ( (APU->commons & STEP_MODE5) ?
	                        PPUCYCLES_STEP5 : PPUCYCLES_STEP4);

	/* Finally, we increase the step counter */
	APU->step++;
	if( APU->commons & STEP_MODE5 ) {
		if( APU->step == 5 ) {
			APU->step = 0;
		}
	}
	else
		if( APU->step == 4 ) {
			APU->step = 0;
		}

}

void end_apu() {

	if( APU != NULL )
		free(APU);
}
