#include <stdint.h>
#include <stdio.h>

#include "cpu.h"
#include "instruction_set.h"
#include "loop.h"

void main_loop(ines_file *file) {

	uint8_t opcode;
	uint8_t operand;
	uint16_t abs_address;
	instruction inst;
	CPU->PC = file->rom;

	/* This is the main loop */
	while(1) {

		/* Read opcode and full instruction :) */
		opcode = *(CPU->PC);
		inst = instructions[opcode];

		switch( inst.addr_mode ) {

			case ADDR_IMMEDIATE:
				operand = *(CPU->PC + 1);
				break;

			case ADDR_ABSOLUTE:
				abs_address = (*(CPU->PC + 1) << 4) | *(CPU->PC + 2);
				break;

			case ADDR_ZEROPAGE:
				abs_address = *(CPU->PC + 1);
				break;

			case ADDR_IMPLIED:
				break;

			case ADDR_INDIRECT:
				break;

			case ADDR_ABS_INDX:
				abs_address = ((*(CPU->PC + 1) << 4) | *(CPU->PC + 2)) + CPU->X;
				break;

			case ADDR_ABS_INDY:
				abs_address = ((*(CPU->PC + 1) << 4) | *(CPU->PC + 2)) + CPU->Y;
				break;

			case ADDR_ZERO_INDX:
			case ADDR_ZERO_INDY:
			case ADDR_IND_INDIR:
			case ADDR_INDIR_IND:
			case ADDR_RELATIVE:
				break;

		}

		
		CPU->PC += inst.size;
	}

}
