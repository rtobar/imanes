#include <stdint.h>
#include <stdio.h>

#include "cpu.h"
#include "instruction_set.h"
#include "loop.h"

void main_loop(ines_file *file) {

	uint8_t opcode;
	CPU->PC = file->rom;

	/* This is the main loop */
	while(1) {

		opcode = *(CPU->PC);
		printf("opcode is 0x%02x\n", opcode);
		CPU->PC++;
	}

}
