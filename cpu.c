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

void init_cpu_ram(ines_file *file) {

	/* 1 ROM bank games load twice to ensure vector tables */
	/* Free the file ROM (we don't need it anymore) */
	if( file->romBanks == 1 ) {
		memcpy( CPU->RAM + 0x8000, file->rom, 0x4000);
		memcpy( CPU->RAM + 0xC000, CPU->RAM + 0x8000, 0x4000);
	}
	/* 2 ROM bank games load one in 0x8000 and other in 0xC000 */
	/* Free the file ROM (we don't need it anymore) */
	else if (file->romBanks == 2 ) {
		memcpy( CPU->RAM + 0x8000, file->rom, 0x4000);
		memcpy( CPU->RAM + 0xC000, file->rom + 0x4000, 0x4000);
	}

	/* We first need to check out where the game begins... */
	/* For that, we see the address localted at the RESET vector */
	/* Remember that NES CPU is little endian */
	CPU->PC = *(CPU->RAM + 0xFFFC) | ( *(CPU->RAM + 0xFFFD) << 8 );

}

void execute_instruction(instruction inst, operand oper) {

	switch(inst.instr_id) {

		case ADC:
			if( inst.addr_mode == ADDR_IMMEDIATE )
				CPU->A += oper.value;
			else
				CPU->A += *(CPU->RAM + oper.address);

			CPU->A += (CPU->SR & C_FLAG);
			update_flags(CPU->A, N_FLAG | Z_FLAG | C_FLAG | V_FLAG);
			break;

		case AND:
			if( inst.addr_mode == ADDR_IMMEDIATE )
				CPU->A &= oper.value;
			else
				CPU->A &= *(CPU->RAM + oper.address);
			break;

		case ASL:
			if( inst.addr_mode == ADDR_ACCUM ) {
				CPU->A <<= 1;
				update_flags(CPU->A, N_FLAG | Z_FLAG | C_FLAG );
			}
			else {
				*(CPU->RAM + oper.address) = *(CPU->RAM + oper.address) << 1;
				update_flags(*(CPU->RAM + oper.address), N_FLAG | Z_FLAG | C_FLAG);
			}

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

		case BVC:
			if( ~CPU->SR & V_FLAG )
				CPU->PC += (int8_t)oper.value;
			break;

		case BVS:
			if( CPU->SR & V_FLAG )
				CPU->PC += (int8_t)oper.value;
			break;

		case CLC:
			CPU->SR &= ~C_FLAG;
			break;

		case CLD:
			CPU->SR &= ~D_FLAG;
			break;

		case CLI:
			CPU->SR &= ~I_FLAG;
			break;

		case CLV:
			CPU->SR &= ~V_FLAG;
			break;

		case CPX:
			if( inst.addr_mode == ADDR_IMMEDIATE )
				update_flags(CPU->X - oper.value, N_FLAG | Z_FLAG | C_FLAG);
			else
				update_flags(CPU->X - *(CPU->RAM + oper.address), N_FLAG | Z_FLAG | C_FLAG);
			break;

		case CPY:
			if( inst.addr_mode == ADDR_IMMEDIATE )
				update_flags(CPU->Y - oper.value, N_FLAG | Z_FLAG | C_FLAG);
			else
				update_flags(CPU->Y - *(CPU->RAM + oper.address), N_FLAG | Z_FLAG | C_FLAG);
			break;

		case DEC:
			*(CPU->RAM + oper.address) = *(CPU->RAM + oper.address) - 1;
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
			printf("\n\nBefore jumping...");
			dump_cpu();
			*(CPU->RAM + CPU->SP++) = (uint8_t)((CPU->PC+2) & 0xFF);
			*(CPU->RAM + CPU->SP++) = (uint8_t)((CPU->PC+2) >> 8);
			printf("%02x%02x\n", *(CPU->RAM + CPU->SP - 2), *(CPU->RAM + CPU->SP - 1));
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

		case RTS:
			CPU->PC =  *(CPU->RAM + --CPU->SP) << 8;
			CPU->PC |= *(CPU->RAM + --CPU->SP);
			printf("\n\nAfter coming back...");
			dump_cpu();
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
			fprintf(stderr,"%s: Still unimplemented\n", inst.name);
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

	static unsigned int which_byte = 0;

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


		/* PPU VRAM address */
		case 0x2006:
			if( !(which_byte & 0x1) ) {
				PPU->vram_addr = 0;
				PPU->vram_addr = (*(CPU->RAM + 0x2006) << 8);
			} else {
				PPU->vram_addr |= *(CPU->RAM + 0x2006);
			}
			which_byte++;
			break;

		/* Data written into PPU->vram_address */
		case 0x2007:
			*(PPU->VRAM + PPU->vram_addr) = *(CPU->RAM + 0x2007);
			if( PPU->CR1 & VERTICAL_WRITE)
				PPU->vram_addr += 32;
			else
				PPU->vram_addr++;
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
