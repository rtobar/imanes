#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "cpu.h"
#include "instruction_set.h"
#include "ppu.h"

nes_cpu *CPU;

void initialize_cpu() {

	CPU = (nes_cpu *)malloc(sizeof(nes_cpu));
	CPU->cycles = 0;
	CPU->RAM = (uint8_t *)malloc(NES_RAM_SIZE);
	CPU->SP  = 0;
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

		case AND:
			if( inst.addr_mode == ADDR_IMMEDIATE )
				CPU->A &= oper.value;
			else
				CPU->A &= *(CPU->RAM + oper.address);
			break;

		case BCC:
			if( ~CPU->SR & C_FLAG )
				CPU->PC += (int8_t)oper.value;
			break;

		case BCS:
			if( CPU->SR & C_FLAG )
				CPU->PC +=(int8_t)oper.value;
			break;

		case BEQ:
			if( CPU->SR & Z_FLAG )
				CPU->PC += (int8_t)oper.value;
			break;

		case BMI:
			if( CPU->SR & N_FLAG )
				CPU->PC += (int8_t)oper.value;
			break;

		case BNE:
			if( ~CPU->SR & Z_FLAG )
				CPU->PC += (int8_t)oper.value;
			break;

		case BPL:
			if( ~CPU->SR & N_FLAG )
				CPU->PC += (int8_t)oper.value;
			break;

		case CLD:
			CPU->SR &= ~D_FLAG;
			break;

		case DEC:
			*(CPU->RAM + oper.address) = *(CPU->RAM + oper.address) + 1;
			update_flags(*(CPU->RAM + oper.address), N_FLAG | Z_FLAG);
			break;

		case DEX:
			CPU->X--;
			update_flags(CPU->X, N_FLAG | Z_FLAG);
			break;

		case DEY:
			CPU->Y--;
			update_flags(CPU->Y, N_FLAG | Z_FLAG);
			break;

		case INX:
			CPU->X++;
			update_flags(CPU->X, N_FLAG | Z_FLAG);
			break;

		case INY:
			CPU->Y++;
			update_flags(CPU->Y, N_FLAG | Z_FLAG);
			break;

		case JMP:
			CPU->PC = oper.address;
			break;

		case JSR:
			*(CPU->RAM + CPU->SP++) = CPU->PC+2;
			*(CPU->RAM + CPU->SP++) = (CPU->PC+2) << 8;
			CPU->PC = oper.address - inst.size;
			break;

		case LDA:
			if( inst.addr_mode == ADDR_IMMEDIATE )
				CPU->A = oper.value;
			else {
				check_read_mapped_io(oper.address);
				CPU->A = *(CPU->RAM + oper.address);
			}
			update_flags(CPU->A, N_FLAG | Z_FLAG);
			break;

		case LDX:
			check_read_mapped_io(oper.address);
			CPU->X = *(CPU->RAM + oper.address);
			update_flags(CPU->X, N_FLAG | Z_FLAG);
			break;

		case LDY:
			check_read_mapped_io(oper.address);
			CPU->Y = *(CPU->RAM + oper.address);
			update_flags(CPU->Y, N_FLAG | Z_FLAG);
			break;

		case NOP:
			break;

		case SEI:
			CPU->SR |= I_FLAG;
			break;

		case STA:
			*(CPU->RAM + oper.address) = CPU->A;
			check_write_mapped_io(oper.address);
			break;

		case STX:
			*(CPU->RAM + oper.address) = CPU->X;
			check_write_mapped_io(oper.address);
			break;

		case STY:
			*(CPU->RAM + oper.address) = CPU->Y;
			check_write_mapped_io(oper.address);
			break;

		case TAX:
			CPU->X = CPU->A;
			update_flags(CPU->X, N_FLAG | Z_FLAG);
			break;

		case TAY:
			CPU->Y = CPU->A;
			update_flags(CPU->Y, N_FLAG | Z_FLAG);
			break;

		case TSX:
			CPU->X = CPU->SP;
			update_flags(CPU->X, N_FLAG | Z_FLAG);
			break;

		case TXA:
			CPU->A = CPU->X;
			update_flags(CPU->A, N_FLAG | Z_FLAG);
			break;

		case TXS:
			CPU->SP = CPU->X;
			update_flags(CPU->X, N_FLAG | Z_FLAG);
			break;

		case TYA:
			CPU->A = CPU->Y;
			update_flags(CPU->A, N_FLAG | Z_FLAG);
			break;

		default:
			fprintf(stderr,"%02x: Still unimplemented\n", inst.opcode);
			break;
	}

}

void update_flags(int8_t value, uint8_t flags) {

	/* 7th bit is set (negative number) */
	if( flags & N_FLAG ) {
		if( (value >> 7) )   CPU->SR |= N_FLAG;
		else                 CPU->SR &= ~N_FLAG;
	}

	if( flags & Z_FLAG ) {
		if( value == 0 )   CPU->SR |= Z_FLAG;
		else               CPU->SR &= ~Z_FLAG;
	}

}

void check_write_mapped_io(uint16_t address) {

	switch( address ) {

		/* PPU control registers */
		case 0x2000:
			PPU->CR1 = *(CPU->RAM + address);
			break;
		case 0x2001:
			PPU->CR2 = *(CPU->RAM + address);
			break;

		/* SPR-RAM Address */
		case 0x2003:
			PPU->spr_addr = *(CPU->RAM + 0x2003);
			break;

		case 0x2004:
			*(PPU->SPR_RAM + PPU->spr_addr) = *(CPU->RAM + 0x2004);
			break;
	}

}

void check_read_mapped_io(uint16_t address) {

	/* PPU Status Register */
	if( address == 0x2002 ) {
		*(CPU->RAM + 0x2002) = PPU->SR;
		PPU->SR &= ~VBLANK_FLAG;
	}

}
