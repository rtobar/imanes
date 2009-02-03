#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "common.h"
#include "cpu.h"
#include "debug.h"
#include "instruction_set.h"
#include "loop.h"
#include "ppu.h"
#include "screen.h"

void main_loop(ines_file *file) {

	uint8_t opcode;
	uint8_t *inst_address;
	int scanline_timeout = CYCLES_PER_SCANLINE;
	int lines, standard_lines;
	struct timespec sleepTime = { 0, 10000000 };
	operand operand = { 0, 0 };
	instruction inst;

	lines = 0;
	standard_lines = 0;
	/* This is the main loop */
	for(;;) {

		/* inst_address saves the current process' memory
		 * pointer for the NES CPU' PC. This way we can
		 * debug easier the value of NES PC */
		inst_address = CPU->RAM + CPU->PC;

		/* Check if we are at the end of the ROM :O */
		if( inst_address == file->rom + file->romBanks * 16*1024 ) {
			fprintf(stderr,"Oops, we've reached the end of the instructions\n");
			fprintf(stderr,"Weird, hah?\n");
			exit(EXIT_FAILURE);
		}

		/* Read opcode and full instruction :) */
		opcode = *(inst_address);
		inst = instructions[opcode];

		DEBUG( printf("CPU->PC: 0x%04x - %02x: %s", CPU->PC, opcode, inst.name) );
		/* Undocumented instruction */
		if( inst.size == 0 ) {
			fprintf(stderr,"\n\nUndocumented instruction: %02x\n",opcode);
			fprintf(stderr,"I'm exiting now... sorry :(\n");
			exit(EXIT_FAILURE);
		}

		/* Select operand depending on the addressing node */
		operand = get_operand(inst, inst_address);

		DEBUG( printf(" operand: %04x\n", operand.address) );
		/* Execute the given instruction */
		execute_instruction(inst,operand);

		CPU->PC += inst.size;
		CPU->cycles += inst.cycles;
		scanline_timeout -= inst.cycles;

		/* A line has ended its scanning, draw it */
		if( scanline_timeout <= 0 ) {

			if( lines < NES_SCREEN_HEIGHT + 3 ) {
				draw_line(lines);
				lines++;
			}
			/* Start VBLANK period */
			else if( lines == NES_SCREEN_HEIGHT + 3) {
				PPU->SR |= VBLANK_FLAG;
				if( PPU->CR1 & VBLANK_ENABLE ) {
					CPU->cycles += 7;
					execute_nmi();
				}
				lines++;
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
					nanosleep(&sleepTime, NULL);
				}
			}

			/* Finally, we set again the timeout to check the scanline */
			scanline_timeout = CYCLES_PER_SCANLINE;
		}

		/* Draw the screen */
		//draw_screen();
	}

}
