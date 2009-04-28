/*  ImaNES: I'm a NES. An intelligent NES emulator

    loop.c   -    Main loop for ImaNES

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
#include <time.h>

#include "common.h"
#include "cpu.h"
#include "debug.h"
#include "instruction_set.h"
#include "loop.h"
#include "mapper.h"
#include "ppu.h"
#include "screen.h"
#include "states.h"

#define DUMPS   1

int run_loop;

int main_loop(void *args) {

	uint8_t opcode;
	int standard_lines;
	int i;
	unsigned long long cycles;
	uint16_t pc_dumps[DUMPS] = { 0xffff };
	operand operand = { 0, 0 };
	instruction inst;
#ifndef _MSC_VER
	long tmp;
	struct timespec sleepTime = { 0, (long)2e7 };
	struct timespec startTime;
	struct timespec endTime;
#endif

	PPU->frames = 0;
	cycles = 0;
	standard_lines = 0;
	PPU->lines = -1;
	PPU->scanline_timeout = CYCLES_PER_SCANLINE;

	/* Get the initial time for the first screen drawing */
#ifndef _MSC_VER
	clock_gettime(CLOCK_REALTIME, &startTime);
#endif

	execute_reset();

	/* This is the main loop */
	for(run_loop = 1;run_loop;) {

		/* First, of all, we check if we should pause the emulation 
		   We do so until the pause has been released by the user
		   We also check if the user wants to quit the emulation */
		while( config.pause && run_loop )
			screen_loop();
		if( !run_loop )
			return 0;

		/* If we want to save our current state or load a new one,
		 * now is the time to do it! */
		if( config.save_state == 1 ) {
			INFO( printf("Saving current state to %d\n", config.current_state) );
			dump_cpu();
			dump_ppu();
			save_state(config.current_state);
			config.save_state = 0;
		}
		else if( config.load_state == 1 ) {
			INFO( printf("Loading state %d\n", config.current_state) );
			load_state(config.current_state);
			dump_cpu();
			dump_ppu();
			cycles = CPU->cycles;
			config.load_state = 0;
		}

		/* If we need to reset, call the reset routine */
		if( CPU->reset )
			execute_reset();

		/* Read opcode and full instruction :) */
		/* We don't read with read_cpu_ram since we're in PGR RAM section
		   and there's nor mirroring nor mm IOs there */
		opcode = CPU->RAM[CPU->PC];
		inst = instructions[opcode];
		instructions[opcode].executed++;

		DEBUG( printf("%04d 0x%04x - %02x: ",CPU->nmi_cycles, CPU->PC, opcode) );
		/* Undocumented instruction */
		if( inst.size == 0 ) {
			fprintf(stderr,"\n\nUndocumented instruction: %02x\n",opcode);
			fprintf(stderr,"I'm exiting now... sorry :(\n");
			fprintf(stderr,"Close the window when finished\n");
			return -1;
		}

		/* Select operand depending on the addressing node */
		operand = get_operand(inst, CPU->PC);

		if( (CPU->nmi_cycles + inst.cycles) >= 2270 &&
		    (PPU->SR&VBLANK_FLAG) ) {
			end_vblank();
		}

		/* Execute the given instruction */
		execute_instruction(inst,operand);

		XTREME( dump_cpu() );
		CPU->PC += inst.size;

		/* Update cycles count */
		CPU->cycles += inst.cycles;
		CPU->nmi_cycles  += (unsigned int)(CPU->cycles - cycles);
		PPU->scanline_timeout -= (int)(CPU->cycles - cycles);
		cycles = CPU->cycles;

		for(i=0;i!=DUMPS;i++)
			if(CPU->PC == pc_dumps[i])
				DEBUG( dump_cpu() );

		/* A line has ended its scanning, draw it */
		if( PPU->scanline_timeout <= 0 ) {

			screen_loop();
			/* First, we set again the timeout to check the scanline */
			PPU->scanline_timeout += CYCLES_PER_SCANLINE;

			/* Every three lines, we should add one required cycle too
			 * (i.e., CYCLES_PER_SCANLINE != 113, but == 113.66666... */
			//PPU->scanline_timeout += !(PPU->lines%3) ? 2: 0;

			/* The NTSC screen works as follows:
			 *
			 * 1) 1 first useless scanline
			 * 2) 240 scanlines where pixels are drawn
			 *    (only 224 are actually shown)
			 * 3) 1 useless scanline
			 * 4) VBlank period
			 **/

			/* One empty scanline at the beggining and the end */
			if( PPU->lines == -1 || PPU->lines == NES_SCREEN_WIDTH ) {
				if( PPU->lines == -1 )
					mapper->update();
				PPU->lines++;
			}

			else if( PPU->lines < NES_SCREEN_HEIGHT ) {
				mapper->update();
				draw_line(PPU->lines++, PPU->frames);
				if( PPU->lines == (NES_SCREEN_HEIGHT - 8) &&
				    (!config.run_fast || !(PPU->frames%2)) )
					redraw_screen();
			}

			/* Start VBLANK period */
			else if( PPU->lines == NES_SCREEN_HEIGHT + 1 ) {

				PPU->SR |= VBLANK_FLAG;
				CPU->nmi_cycles = 0;
				if( PPU->CR1 & VBLANK_ENABLE ) {
					execute_nmi();
					PPU->scanline_timeout -= 7;
					cycles = CPU->cycles;
				}

				PPU->lines++;
				PPU->frames++;
				standard_lines = 0;
			}
			/* VBLANK period */
			else {
				standard_lines++;
				PPU->lines++;

				/* End of VBLANK period */
				if( standard_lines == 20 ) {

					PPU->lines = -1;
					end_vblank();

#ifndef _MSC_VER
					/* Calculate how much we should sleep for 50/60 FPS */
					/* For this, we calculate the next "start" time,    */
					/* and then we calculate the different between it   */
					/* the actual time                                  */
					tmp = endTime.tv_sec;
					clock_gettime(CLOCK_REALTIME, &endTime);
					startTime.tv_nsec += (long)1.666666e7;
					if( startTime.tv_nsec > 1e9 ) {
						startTime.tv_sec++;
						startTime.tv_nsec -= (long)1e9;
					}
	
					if( endTime.tv_sec != tmp ) {
						INFO( fprintf(stderr,"Running at %d fps\n",PPU->frames) );
						PPU->frames = 0;
					}
	
					sleepTime.tv_nsec = startTime.tv_nsec - endTime.tv_nsec;
					sleepTime.tv_sec  = startTime.tv_sec  - endTime.tv_sec;
					if( sleepTime.tv_nsec < 0 ) {
						sleepTime.tv_sec--;
						sleepTime.tv_nsec += (long)1e9;
					}

					/* We were on pause or in fast run */
					if( sleepTime.tv_sec > 0 || sleepTime.tv_nsec > 1.666666e7 ) {
						clock_gettime(CLOCK_REALTIME, &startTime);
						sleepTime.tv_sec = 0;
						sleepTime.tv_nsec = 0;
					}
					if( sleepTime.tv_sec >= 0 && !config.run_fast )
						nanosleep(&sleepTime, NULL);
#endif
				}
			}
		}

	}

	return 0;
}

void end_vblank() {

	PPU->SR &= ~VBLANK_FLAG;
	PPU->SR &= ~HIT_FLAG;
	PPU->SR &= ~MAX_SPRITES_DRAWN;

	return;
}
