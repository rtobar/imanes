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
#include "cpu.h"
#include "debug.h"

nes_apu *APU;

void initialize_apu() {

	APU = (nes_apu *)malloc(sizeof(nes_apu));

	APU->length_ctr = 0;
	APU->commons = 0;

	/* Frame sequencer initialization */
	APU->frame_seq.step = 0;
	APU->frame_seq.clock_timeout = PPUCYCLES_STEP4;
	APU->frame_seq.int_flag = 0;

	return;
}

void dump_apu() {

	printf("0x4015:%02x  ", APU->length_ctr);
	printf("0x4017:%02x  ", APU->commons);
	printf("FS: %d/%d\n", APU->frame_seq.step, APU->frame_seq.int_flag);

	return;
}

void clock_apu_sequencer() {

	/* Reset the clock timeout depending on the sequencer mode */
	APU->frame_seq.clock_timeout += ( (APU->commons & STEP_MODE5) ?
	                                PPUCYCLES_STEP5 : PPUCYCLES_STEP4);


	INFO( dump_apu() );

	/* At any time, if the interrupt flag is set
    * and the IRQ disable is clear, CPU's IRQ is asserted */
	if( APU->frame_seq.int_flag && !(APU->commons & DISABLE_FRAME_IRQ) ) {
		printf("Triggering IRQ from APU\n");
		execute_irq();
	}

	/* Different actions depending on the step number
    * and the step mode of the frame sequencer */
	if( APU->commons & STEP_MODE5 ) {
	}
	else {

		switch(APU->frame_seq.step) {

			case 1:
				break;

			case 2:
				break;

			case 3:
				break;

			case 4:
				APU->frame_seq.int_flag = 1;
				break;

		}
	}

	/* Finally, we increase the step counter */
	APU->frame_seq.step++;
	if( APU->commons & STEP_MODE5 ) {
		if( APU->frame_seq.step == 5 ) {
			APU->frame_seq.step = 0;
		}
	}
	else
		if( APU->frame_seq.step == 4 ) {
			APU->frame_seq.step = 0;
		}

}

void end_apu() {

	if( APU != NULL )
		free(APU);
}
