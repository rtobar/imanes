/*  ImaNES: I'm a NES. An intelligent NES emulator

    loop.c   -    Main loop for ImaNES

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
#include <time.h>
#include <unistd.h>

#include "common.h"
#include "cpu.h"
#include "debug.h"
#include "instruction_set.h"
#include "loop.h"
#include "mapper.h"
#include "ppu.h"
#include "screen.h"

#define DUMPS   1

pthread_mutex_t pause_mutex;

int run_loop;

void main_loop(ines_file *file) {

	uint8_t opcode;
	int scanline_timeout = CYCLES_PER_SCANLINE;
	int lines, standard_lines;
	int frames;
	int i;
	long tmp;
	struct timespec sleepTime = { 0, 2e7 };
	struct timespec startTime;
	struct timespec endTime;
	uint16_t pc_dumps[DUMPS] = { 0xffff };
	operand operand = { 0, 0 };
	instruction inst;

	frames = 0;
	lines = 0;
	standard_lines = 0;

	pthread_mutex_init(&pause_mutex, NULL);

	/* Get the initial time for the first screen drawing */
	clock_gettime(CLOCK_REALTIME, &startTime);

	execute_reset();

	/* This is the main loop */
	for(run_loop = 1;run_loop;) {

		/* First, of all, we check if we should pause the emulation 
		   We do so until the mutex has been released by the user
		   Anyways, we don't hold it locked, so the user can pause the
		   emulation again */
		pthread_mutex_lock(&pause_mutex);
		pthread_mutex_unlock(&pause_mutex);

		/* If we need to reset, call the reset routine */
		if( CPU->reset )
			execute_reset();

		/* Read opcode and full instruction :) */
		/* We don't read with read_cpu_ram since we're in PGR RAM section
		   and there's nor mirroring nor mm IOs there */
		opcode = CPU->RAM[CPU->PC];
		inst = instructions[opcode];
		instructions[opcode].executed++;

		XTREME( dump_cpu(); );
		DEBUG( printf("CPU->PC: 0x%04x - %02x: %s ", CPU->PC, opcode, inst.name) );
		/* Undocumented instruction */
		if( inst.size == 0 ) {
			fprintf(stderr,"\n\nUndocumented instruction: %02x\n",opcode);
			fprintf(stderr,"I'm exiting now... sorry :(\n");
			fprintf(stderr,"Close the window when finished\n");
			return;
		}

		/* Select operand depending on the addressing node */
		operand = get_operand(inst, CPU->PC);

		DEBUG( printf("operand: %04x / %02x\n", operand.address, operand.value) );
		XTREME( printf("\n\n") );

		/* Execute the given instruction */
		execute_instruction(inst,operand);

		CPU->PC += inst.size;
		CPU->cycles += inst.cycles;
		scanline_timeout -= inst.cycles;

		for(i=0;i!=DUMPS;i++)
			if(CPU->PC == pc_dumps[i])
				DEBUG( dump_cpu() );

		/* A line has ended its scanning, draw it */
		if( scanline_timeout <= 0 ) {

			if( lines < NES_SCREEN_HEIGHT ) {
				draw_line(lines++);
				mapper->update();
			}
			/* Start VBLANK period */
			else if( lines == NES_SCREEN_HEIGHT ) {
				PPU->SR |= VBLANK_FLAG;
				if( PPU->CR1 & VBLANK_ENABLE ) {
					CPU->cycles += 7;
					execute_nmi();
				}
				redraw_screen();
				lines++;
				frames++;
				standard_lines = 0;
			}
			/* VBLANK period */
			else {
				standard_lines++;

				/* End of VBLANK period */
				if( standard_lines == 20 ) {
					standard_lines = 0;
					lines = 0;
					PPU->SR &= ~VBLANK_FLAG;
					PPU->SR &= ~HIT_FLAG;

					/* Calculate how much we should sleep for 50/60 FPS */
					/* For this, we calculate the next "start" time,    */
					/* and then we calculate the different between it   */
					/* the actual time                                  */
					tmp = endTime.tv_sec;
					clock_gettime(CLOCK_REALTIME, &endTime);
					startTime.tv_nsec += 1.666666e7;
					if( startTime.tv_nsec > 1e9 ) {
						startTime.tv_sec++;
						startTime.tv_nsec -= 1e9;
					}
	
					if( endTime.tv_sec != tmp ) {
						INFO( fprintf(stderr,"Running at %d fps\n",frames) );
						frames = 0;
					}
	
					sleepTime.tv_nsec = startTime.tv_nsec - endTime.tv_nsec;
					sleepTime.tv_sec  = startTime.tv_sec  - endTime.tv_sec;
					if( sleepTime.tv_nsec < 0 ) {
						sleepTime.tv_sec--;
						sleepTime.tv_nsec += 1e9;
					}

					/* We were on pause or in fast run */
					if( sleepTime.tv_sec > 0 || sleepTime.tv_nsec > 1.666666e7 ) {
						clock_gettime(CLOCK_REALTIME, &startTime);
						sleepTime.tv_sec = 0;
						sleepTime.tv_nsec = 0;
					}
					if( sleepTime.tv_sec >= 0 && !config.run_fast )
						nanosleep(&sleepTime, NULL);
				}
			}

			/* Finally, we set again the timeout to check the scanline */
			scanline_timeout = CYCLES_PER_SCANLINE;
		}

	}

}
