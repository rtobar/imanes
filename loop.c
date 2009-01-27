#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "cpu.h"
#include "debug.h"
#include "instruction_set.h"
#include "loop.h"
#include "ppu.h"

void main_loop(ines_file *file) {

	uint8_t opcode;
	uint8_t *inst_address;
	operand operand = { 0, 0 };
	instruction inst;

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

		DEBUG( printf(" operand: %04x / 0x%02x\n", operand.address, operand.value) );
		/* Execute the given instruction */
		execute_instruction(inst,operand);

		CPU->PC += inst.size;
		CPU->cycles += inst.cycles;

		/* Draw the screen */
		draw_screen();
	}

}
