#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"

nes_cpu *CPU;

void initialize_cpu() {
	CPU = (nes_cpu *)malloc(sizeof(nes_cpu));

	/* Initialize all the instructions/addressing modes */
}

void dump_cpu() {

	printf("CPU Dumping:\n============\n\n");
	printf("PC: %04x\n", (unsigned int)CPU->PC);
	printf("A:  %02x\n", CPU->A);
	printf("X:  %02x\n", CPU->X);
	printf("Y:  %02x\n", CPU->Y);
	printf("SR: %02x\n", CPU->SR);
	printf("SP: %02x\n", CPU->SP);

	return;
}

void main_cpu_loop(ines_file *file) {

	uint8_t opcode;
	uint16_t operand = 0;
	instruction inst;
	CPU->PC = file->rom;

	/* This is the main loop */
	while(1) {

		/* Check if we are at the end of the ROM :O */
		if( CPU->PC == file->rom + file->romBanks * 16*1024 ) {
			fprintf(stderr,"Oops, we've reached the end of the instructions\n");
			fprintf(stderr,"Weird, hah?\n");
			exit(EXIT_FAILURE);
		}

		/* Read opcode and full instruction :) */
		opcode = *(CPU->PC);
		inst = instructions[opcode];

		/* Undocumented instruction */
		if( inst.size == 0 ) {
			CPU->PC += 2;
			continue;
		}

		/* Select operand depending on the addressing node */
		switch( inst.addr_mode ) {

			case ADDR_IMMEDIATE:
				operand = *(CPU->PC + 1);
				break;

			case ADDR_ABSOLUTE:
				operand = (*(CPU->PC + 1) << 8) | *(CPU->PC + 2);
				break;

			case ADDR_ZEROPAGE:
				operand = *(CPU->PC + 1);
				break;

			case ADDR_IMPLIED:
				break;

			case ADDR_INDIRECT:
				break;

			case ADDR_ABS_INDX:
				operand = ((*(CPU->PC + 1) << 8) | *(CPU->PC + 2)) + CPU->X;
				break;

			case ADDR_ABS_INDY:
				operand = ((*(CPU->PC + 1) << 8) | *(CPU->PC + 2)) + CPU->Y;
				break;

			case ADDR_ZERO_INDX:
			case ADDR_ZERO_INDY:
			case ADDR_IND_INDIR:
			case ADDR_INDIR_IND:
			case ADDR_RELATIVE:
				break;

		}

		printf("%02x: %s operand: %04x\n", opcode, inst.name, operand);
		
		CPU->PC += inst.size;
	}

}
