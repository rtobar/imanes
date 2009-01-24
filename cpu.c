#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "cpu.h"
#include "ppu.h"

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
