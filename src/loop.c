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

#include "clock.h"
#include "common.h"
#include "cpu.h"
#include "debug.h"
#include "frame_control.h"
#include "gui.h"
#include "instruction_set.h"
#include "loop.h"
#include "mapper.h"
#include "ppu.h"
#include "screen.h"
#include "states.h"

/* When VBLANK ends, we clear some flags */
#define END_VBLANK() \
	do { \
		PPU->SR &= ~VBLANK_FLAG; \
		PPU->SR &= ~HIT_FLAG; \
		PPU->SR &= ~MAX_SPRITES_DRAWN; \
	} while(0)

int run_loop;

int main_loop(void *args) {

	uint8_t opcode;
	int standard_lines;
	unsigned long int ppu_cycles;
	operand operand = { 0, 0 };
	instruction inst;

	ppu_cycles = 0;
	standard_lines = 0;
	PPU->frames = 0;
	PPU->lines = -1;
	PPU->scanline_timeout = CYCLES_PER_SCANLINE;

	/* Get the initial time for the first screen drawing */
	start_timing();

	execute_reset();

	/* This is the main loop */
	for(run_loop = 1;run_loop;) {

		/* First, of all, we check if we should pause the emulation 
		 * If we are un pause, we go to GUI mode.
		 * We do so until the pause has been released by the user
		 * We also check if the user wants to quit the emulation */
		if( config.pause && run_loop ) {
			gui_set_background();
			gui_loop();
		}
		if( !run_loop )
			return 0;

		/* If we want to save our current state or load a new one,
		 * now is the time to do it! */
		if( config.save_state == 1 ) {
			save_state(config.current_state);
			config.save_state = 0;
		}
		else if( config.load_state == 1 ) {
			load_state(config.current_state);
			config.load_state = 0;
			ppu_cycles = CLK->ppu_cycles;
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

		DEBUG( printf("%04u 0x%04x - %02x: ",CLK->nmi_ccycles, CPU->PC, opcode) );
		/* Undocumented instruction */
		if( inst.size == 0 ) {
			fprintf(stderr,"\n\nUndocumented instruction: %02x\n",opcode);
			fprintf(stderr,"I'm exiting now... sorry :(\n");
			fprintf(stderr,"Close the window when finished\n");
			return -1;
		}

		/* Select operand depending on the addressing node */
		operand = get_operand(inst, CPU->PC);

		if( (CLK->nmi_ccycles + inst.cycles) >= 2270 &&
		    (PPU->SR&VBLANK_FLAG) ) {
			END_VBLANK();
		}

		/* Execute the given instruction */
		execute_instruction(inst,operand);

		XTREME( dump_cpu() );
		CPU->PC += inst.size;

		/* Update cycles count */
		ADD_CPU_CYCLES(inst.cycles);
		PPU->scanline_timeout -= (int)(CLK->ppu_cycles - ppu_cycles);
		ppu_cycles = CLK->ppu_cycles;

		/* A line has ended its scanning, draw it */
		if( PPU->scanline_timeout <= 0 ) {

			/* Get the user's input and process it */
			screen_loop();

			/* Set again the timeout to check the scanline */
			PPU->scanline_timeout += CYCLES_PER_SCANLINE;

			/* The NTSC screen works as follows:
			 *
			 * 1) 1 first pre-render scanline
			 * 2) 240 scanlines where pixels are drawn
			 *    (only 224 are actually shown)
			 * 3) 1 dummy scanline
			 * 4) VBlank period
			 *
			 * The pre-render scanline works as all the others, but
			 * doesn't get drawn.
			 **/

			if( PPU->lines < NES_SCREEN_HEIGHT ) {
				mapper->update();
				draw_line(PPU->lines++, PPU->frames);
				if( PPU->lines == (NES_SCREEN_HEIGHT - 8) &&
				    (!config.run_fast || !(PPU->frames%2)) )
					redraw_screen();
			}
			/* "Dummy" scanline at the end of the drawn scanlines */
			else if( PPU->lines == NES_SCREEN_HEIGHT )
				PPU->lines++;

			/* Start VBLANK period */
			else if( PPU->lines == NES_SCREEN_HEIGHT + 1 ) {

				PPU->SR |= VBLANK_FLAG;
				CLK->nmi_ccycles = 0;
				CLK->nmi_pcycles = 0;
				if( PPU->CR1 & VBLANK_ENABLE ) {
					execute_nmi();
					PPU->scanline_timeout -= 7;
					ppu_cycles = CLK->ppu_cycles;
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

					PPU->lines = 0;
					END_VBLANK();
					frame_sleep();

				}
			}
		}

	}

	return 0;
}
