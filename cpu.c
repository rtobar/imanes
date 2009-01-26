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

void execute_instruction(instruction inst, operand oper) {

	switch(inst.instr_id) {

		case BPL:
			if( (CPU->SR & N_FLAG) )
				CPU->PC += (int8_t)oper.value;
			break;

		case BNE:
			if( CPU->SR & Z_FLAG )
				CPU->PC += (int8_t)oper.value;
			break;

		case CLD:
			CPU->SR &= ~D_FLAG;
			break;

		case DEC:
			*(CPU->RAM + oper.address) = *(CPU->RAM + oper.address) + 1;
			break;

		case DEX:
			CPU->X--;
			if( CPU->X == 0)  CPU->SR |= Z_FLAG;
			if( (int8_t)CPU->X < 0 )  CPU->SR |= N_FLAG;
			break;

		case DEY:
			CPU->Y--;
			if( CPU->Y == 0)  CPU->SR |= Z_FLAG;
			if( (int8_t)CPU->Y < 0 )  CPU->SR |= N_FLAG;
			break;

		case INX:
			CPU->X++;
			if( CPU->X == 0)  CPU->SR |= Z_FLAG;
			if( (int8_t)CPU->X < 0 )  CPU->SR |= N_FLAG;
			break;

		case INY:
			CPU->Y++;
			if( CPU->Y == 0)  CPU->SR |= Z_FLAG;
			if( (int8_t)CPU->Y < 0 )  CPU->SR |= N_FLAG;
			break;

		case LDA:
			CPU->A = (int8_t)oper.value;
			break;

		case LDX:
			CPU->X = (int8_t)oper.value;
			break;

		case LDY:
			CPU->Y = (int8_t)oper.value;
			break;

		case SEI:
			CPU->SR |= I_FLAG;
			break;

		case STX:
			*(CPU->RAM + oper.address) = CPU->X;
			break;

		case STY:
			*(CPU->RAM + oper.address) = CPU->Y;
			break;

		case TSX:
			CPU->X = CPU->SP;
			break;

		case TXS:
			CPU->SP = CPU->X;
			break;
	}

}
