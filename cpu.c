#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "cpu.h"

nes_cpu *CPU;

void initialize_cpu() {

	CPU = (nes_cpu *)malloc(sizeof(nes_cpu));

	CPU->cycles = 0;

	/* Size of NES' RAM */
	CPU->RAM = (uint8_t *)malloc(NES_RAM_SIZE);
}

void dump_cpu() {

	printf("CPU Dumping:\n============\n\n");
	printf("PC: %04x\n", (unsigned int)CPU->PC);
	printf("A:  %02x\n", CPU->A);
	printf("X:  %02x\n", CPU->X);
	printf("Y:  %02x\n", CPU->Y);
	printf("SR: %02x\n", CPU->SR);
	printf("SP: %02x\n", CPU->SP);
	printf("Cycles: %lld\n", CPU->cycles);

	return;
}

void execute_instruction(instruction inst, uint16_t operand) {

	switch(inst.instr_id) {

		case BPL:
			if( (CPU->SR & N_FLAG) )
				CPU->PC += (int8_t)operand;
			break;

		case CLD:
			CPU->SR &= ~D_FLAG;
			break;

		case DEX:
			CPU->X--;
			break;

		case DEY:
			CPU->Y--;
			break;

		case INX:
			CPU->X++;
			break;

		case INY:
			CPU->Y++;
			break;

		case LDA:
			CPU->A = (int8_t)operand;
			break;

		case LDX:
			CPU->X = (int8_t)operand;
			break;

		case LDY:
			CPU->Y = (int8_t)operand;
			break;

		case SEI:
			CPU->SR |= I_FLAG;
			break;

		case STX:
			*(CPU->RAM + operand) = CPU->X;
			break;

		case STY:
			*(CPU->RAM + operand) = CPU->Y;
			break;

		case TSX:
			CPU->X = CPU->SP;
			break;
	}

}

void main_cpu_loop(ines_file *file) {

	uint8_t opcode;
	uint16_t address;
	uint16_t operand = 0;
	instruction inst;

	/* 1 ROM bank games load twice to ensure vector tables */
	/* Free the file ROM (we don't need it anymore) */
	if( file->romBanks == 1 ) {
		printf("CPU->RAM  = 0x%08x\n",(unsigned int)CPU->RAM);
		printf("file->rom = 0x%08x\n",(unsigned int)file->rom);
		memcpy( CPU->RAM + 0x8000, file->rom, 0x4000);
		memcpy( CPU->RAM + 0xC000, CPU->RAM + 0x8000, 0x4000);
	}
	/* 2 ROM bank games load one in 0x8000 and other in 0xC000 */
	/* Free the file ROM (we don't need it anymore) */
	else if (file->romBanks == 2 ) {
		printf("CPU->RAM  = 0x%08x\n",(unsigned int)CPU->RAM);
		printf("file->rom = 0x%08x\n",(unsigned int)file->rom);
		memcpy( CPU->RAM + 0x8000, file->rom, 0x4000);
		memcpy( CPU->RAM + 0xC000, file->rom + 0x4000, 0x4000);
	}

	/* We first need to check out where the game begins... */
	/* For that, we see the address localted at the RESET vector */
	/* Remember that NES CPU is little endian */
	CPU->PC = CPU->RAM + (*(CPU->RAM + 0xFFFC) | ( *(CPU->RAM + 0xFFFD) << 8 ));

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
			printf("Undocumented/Unimplemented instruction: %02x\n",inst.opcode);
			exit(EXIT_FAILURE);
		}

		/* Select operand depending on the addressing node */
		switch( inst.addr_mode ) {

			case ADDR_IMMEDIATE:
				operand = *(CPU->PC + 1);
				break;

			case ADDR_ABSOLUTE:
				address = *(CPU->PC + 1) | (*(CPU->PC + 2)  << 8);
				operand = *(CPU->RAM + address);
				break;

			case ADDR_ZEROPAGE:
				address = *(CPU->PC + 1);
				operand = *(CPU->RAM + address);
				break;

			case ADDR_IMPLIED:
				break;

			case ADDR_INDIRECT:
				break;

			case ADDR_ABS_INDX:
				operand = ( *(CPU->PC + 1) | (*(CPU->PC + 2) << 8) ) + CPU->X;
				break;

			case ADDR_ABS_INDY:
				operand = ( *(CPU->PC + 1) | (*(CPU->PC + 2) << 8) ) + CPU->Y;
				break;

			case ADDR_ZERO_INDX:
			case ADDR_ZERO_INDY:
			case ADDR_IND_INDIR:
			case ADDR_INDIR_IND:
			case ADDR_RELATIVE:
				operand = *(CPU->PC + 1);
				break;

		}

		printf("%02x: %s operand: %04x\n", opcode, inst.name, operand);

		/* Execute the given instruction */
		execute_instruction(inst,operand);
		
		CPU->PC += inst.size;
		CPU->cycles += inst.cycles;
	}

}
