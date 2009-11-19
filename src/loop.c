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

#include "apu.h"
#include "clock.h"
#include "common.h"
#include "cpu.h"
#include "debug.h"
#include "frame_control.h"
#include "gui.h"
#include "i18n.h"
#include "instruction_set.h"
#include "loop.h"
#include "mapper.h"
#include "ppu.h"
#include "screen.h"
#include "states.h"

/* When VBLANK ends, we clear some flags */
#define END_VBLANK() \
	do { \
		if( !vblank_ended ) { \
			DEBUG( printf(_("Ending VBlank\n")) ); \
			PPU->SR &= ~VBLANK_FLAG; \
			PPU->SR &= ~HIT_FLAG; \
			PPU->SR &= ~MAX_SPRITES_DRAWN; \
		} \
		vblank_ended = 1; \
	} while(0);

int run_loop;

int main_loop(void *args) {

	uint8_t opcode;
	int standard_lines;
	int vblank_ended = 0;
	int a12_raised = 0;
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

	playback_pause(0);
	execute_reset();

	/* This is the main loop */
	for(run_loop = 1;run_loop;) {

		/* First, of all, we check if we should pause the emulation 
		 * If we are un pause, we go to GUI mode.
		 * We do so until the pause has been released by the user
		 * We also check if the user wants to quit the emulation */
		if( config.pause && run_loop ) {
			playback_pause(1);
			gui_set_background();
			gui_loop();
			playback_pause(0);
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

		/* A12 rising edge occurs at PPU cycle #260 on the scanline */
		if( !a12_raised && PPU->scanline_timeout <= 76 && (int)PPU->lines < NES_SCREEN_HEIGHT ) {
			mapper->update();
			a12_raised = 1;
		}

		/* Read opcode and full instruction :) */
		/* We don't read with read_cpu_ram since we're in PGR RAM section
		   and there's nor mirroring nor mm IOs there */
		opcode = CPU->RAM[CPU->PC];
		inst = instructions[opcode];
		instructions[opcode].executed++;

		DEBUG( printf("%04.0f 0x%04x - %02x: ",CLK->nmi_pcycles/3., CPU->PC, opcode) );
		/* Undocumented instruction */
		if( inst.size == 0 ) {
			fprintf(stderr,_("\n\nUndocumented instruction: %02x\n"),opcode);
			fprintf(stderr,_("I'm exiting now... sorry :(\n"));
			fprintf(stderr,_("Close the window when finished\n"));
			return -1;
		}

		/* Select operand depending on the addressing node */
		operand = get_operand(inst, CPU->PC);

		/* Clear the VBLANK flag if the execution of the instruction
		 * passes the instant when the VBLANK flag is cleared */
		if( CLK->nmi_pcycles + inst.cycles*3 >= 6820 &&
		    (PPU->SR&VBLANK_FLAG) )
			END_VBLANK();

		/* Set the VBLANK flag if the execution of the instruction
		 * passes the instant when the VBLANK flag is set */
		if( (PPU->lines == NES_SCREEN_HEIGHT) && PPU->scanline_timeout <= 1 )
			PPU->SR |= VBLANK_FLAG;

		/* Execute the given instruction */
		execute_instruction(inst,operand);

		XTREME( dump_cpu() );
		CPU->PC += inst.size;

		/* Update cycles count */
		ADD_CPU_CYCLES(inst.cycles);
		PPU->scanline_timeout -= (int)(CLK->ppu_cycles - ppu_cycles);
		APU->frame_seq.clock_timeout -= (int)(CLK->ppu_cycles - ppu_cycles);
		ppu_cycles = CLK->ppu_cycles;

		/* TODO: Add the other APU timers: square/triangle */

		/* The APU sequencer needs to clock */
		/* This sequencer then triggers */
		if( APU->frame_seq.clock_timeout <= 0 )
			clock_apu_sequencer();

		/* A line has ended its scanning, draw it */
		if( PPU->scanline_timeout <= 0 ) {

			/* Clear A12 flag to check again in the next line */
			a12_raised = 0;

			/* Get the user's input and process it */
			screen_loop();

			/* Set again the timeout to check the scanline */
			PPU->scanline_timeout += CYCLES_PER_SCANLINE;
			if( (PPU->CR2 & SHOW_BACKGROUND) && !(PPU->frames%2) && (PPU->lines == 19) )
				PPU->scanline_timeout--;

			/* The NTSC screen works as follows:
			 *
			 * 1) 1 first pre-render scanline
			 * 2) 240 scanlines where pixels are drawn
			 *    (only 224 are actually shown)
			 * 3) 1 dummy scanline
			 * 4) VBlank period (20 scanlines)
			 *
			 * The pre-render scanline works as all the others, but
			 * doesn't get drawn.
			 **/

			if( (int)PPU->lines < NES_SCREEN_HEIGHT ) {
				draw_line(PPU->lines++, PPU->frames);
				if( PPU->lines == (NES_SCREEN_HEIGHT - 8) &&
				    (!config.run_fast || !(PPU->frames%2)) )
					redraw_screen();
			}

			/* Start VBLANK period */
			else if( PPU->lines == NES_SCREEN_HEIGHT ) {

				PPU->SR |= VBLANK_FLAG;
				CLK->nmi_pcycles = (CYCLES_PER_SCANLINE - PPU->scanline_timeout)*3;
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

				/* End of VBLANK period */
				if( standard_lines == 20 ) {

					PPU->lines = -1;
					END_VBLANK();
					vblank_ended = 0;
					frame_sleep();

				}
			}
		}

	}

	return 0;
}
