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

#define DUMPS   1

pthread_mutex_t pause_mutex;

void main_loop(ines_file *file) {

	uint8_t opcode;
	uint8_t *inst_address;
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

	/* This is the main loop */
	for(;;) {

		/* First, of all, we check if we should pause the emulation 
		   We do so until the mutex has been released by the user
		   Anyways, we don't hold it locked, so the user can pause the
		   emulation again */
		pthread_mutex_lock(&pause_mutex);
		pthread_mutex_unlock(&pause_mutex);

		/* Whenever we do a reset, we first need to check out where the game
 		 * begins. For that, we see the address located at the RESET vector
 		 */
		if( CPU->reset ) {
			CPU->PC = *(CPU->RAM + 0xFFFC) | ( *(CPU->RAM + 0xFFFD) << 8 );
			CPU->reset = 0;
		}

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

		DEBUG( printf(" operand: %04x / %02x\n", operand.address, operand.value) );
		/* Execute the given instruction */
		execute_instruction(inst,operand);

		for(i=0;i!=DUMPS;i++) {
			if(CPU->PC == pc_dumps[i] )
				dump_cpu();
		}

		CPU->PC += inst.size;
		CPU->cycles += inst.cycles;
		scanline_timeout -= inst.cycles;

		/* A line has ended its scanning, draw it */
		if( scanline_timeout <= 0 ) {

			if( lines < NES_SCREEN_HEIGHT ) {
				draw_line(lines++);
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

					/* Calculate how much we should sleep for 50/60 FPS */
					/* For this, we calculate the next "start" time,    */
					/* and then we calculate the different between it   */
					/* the actual time                                  */
					tmp = endTime.tv_sec;
					clock_gettime(CLOCK_REALTIME, &endTime);
					startTime.tv_nsec += 2e7;
					if( startTime.tv_nsec > 1e9 ) {
						startTime.tv_sec++;
						startTime.tv_nsec -= 1e9;
					}

					if( endTime.tv_sec != tmp ) {
						DEBUG( fprintf(stderr,"Running at %d fps\n",frames) );
						frames = 0;
					}

					sleepTime.tv_nsec = startTime.tv_nsec - endTime.tv_nsec;
					sleepTime.tv_sec  = startTime.tv_sec  - endTime.tv_sec;
					if( sleepTime.tv_nsec < 0 ) {
						sleepTime.tv_sec--;
						sleepTime.tv_nsec += 1e9;
					}
					if( sleepTime.tv_sec >= 0 )
						nanosleep(&sleepTime, NULL);
				}
			}

			/* Finally, we set again the timeout to check the scanline */
			scanline_timeout = CYCLES_PER_SCANLINE;
		}

	}

}
