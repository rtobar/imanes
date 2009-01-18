#include <stdint.h>
#include <stdio.h>

#include "cpu.h"
#include "instruction_set.h"
#include "loop.h"

void main_loop(ines_file *file) {

	uint8_t opcode;
	instruction inst;
	CPU->PC = file->rom;

	/* This is the main loop */
	while(1) {

		opcode = *(CPU->PC);
		inst = instructions[opcode];
		printf("Instruction is %02x, size %d bytes\n", inst.opcode, inst.size);
		CPU->PC += inst.size;
	}

}
