#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "cpu.h"
#include "debug.h"
#include "instruction_set.h"
#include "pad.h"
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
	printf("SP: %02u\n", CPU->SP);
	printf("Cycles: %lld\n", CPU->cycles);

	return;
}

void dump_stack() {

	int i;

	printf("CPU Stack:\n==========\n\n");
	for( i=0; i!=CPU->SP;i++) {
		printf("%02x ", *(CPU->RAM + BEGIN_STACK + i));
	}
	printf("\n");
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

	uint8_t tmp;

	switch(inst.instr_id) {

		case ADC: /* TODO: check C and V flags */
			if( inst.addr_mode != ADDR_IMMEDIATE ) {
				check_read_mapped_io(oper.address);
				oper.value = *(CPU->RAM + oper.address);
			}
			CPU->A += oper.value + (CPU->SR & C_FLAG);
			update_flags(CPU->A, N_FLAG | Z_FLAG);
			break;

		case AND:
			if( inst.addr_mode != ADDR_IMMEDIATE ) {
				check_read_mapped_io(oper.address);
				oper.value = *(CPU->RAM + oper.address);
			}
			CPU->A &= oper.value;
			update_flags(CPU->A, N_FLAG | Z_FLAG );
			break;

		case ASL:
			if( inst.addr_mode == ADDR_ACCUM ) {
				if( CPU->A  & 0x8 )	
					CPU->SR |= C_FLAG;
				else
					CPU->SR &= ~C_FLAG;
				CPU->A <<= 1;
				update_flags(CPU->A, N_FLAG | Z_FLAG);
			}
			else {
				check_read_mapped_io(oper.address);
				if( *(CPU->RAM + oper.address)  & 0x8 )
					CPU->SR |= C_FLAG;
				else
					CPU->SR &= ~C_FLAG;
				*(CPU->RAM + oper.address) = *(CPU->RAM + oper.address) << 1;
				update_flags(*(CPU->RAM + oper.address), N_FLAG | Z_FLAG);
			}
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

		case BIT:
			check_read_mapped_io(oper.address);
			tmp = CPU->A & *(CPU->RAM + oper.address);
			if( (tmp >> 6)  & 0x01 )
				CPU->SR |= V_FLAG;
			update_flags(tmp, N_FLAG | Z_FLAG);
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

		case BRK:
			/* Set the interrupt flag, push the PC+2 (not a bug) and the SR */
			/* Finally, jump to the interrump vector */
			CPU->SR |= I_FLAG;
			*(CPU->RAM + BEGIN_STACK + CPU->SP++) = CPU->SR;
			*(CPU->RAM + BEGIN_STACK + CPU->SP++) = (uint8_t)((CPU->PC+2) & 0xFF);
			*(CPU->RAM + BEGIN_STACK + CPU->SP++) = (uint8_t)((CPU->PC+2) >> 8);
			CPU->PC = (*(CPU->RAM + 0xFFFE) | (*(CPU->RAM + 0xFFFF)<<8) );
			CPU->PC -= inst.size;
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

		case CMP:
			if( inst.addr_mode != ADDR_IMMEDIATE ) {
				check_read_mapped_io(oper.address);
				oper.value = *(CPU->RAM + oper.address);
			}
			if( CPU->A >= oper.value)
				CPU->SR |= C_FLAG;
			update_flags(CPU->A - oper.value, N_FLAG | Z_FLAG);
			break;

		case CPX:
			if( inst.addr_mode != ADDR_IMMEDIATE ) {
				check_read_mapped_io(oper.address);
				oper.value = *(CPU->RAM + oper.address);
			}
			if( CPU->X >= oper.value)
				CPU->SR |= C_FLAG;
			update_flags(CPU->X - oper.value, N_FLAG | Z_FLAG);
			break;

		case CPY:
			if( inst.addr_mode != ADDR_IMMEDIATE ) {
				check_read_mapped_io(oper.address);
				oper.value = *(CPU->RAM + oper.address);
			}
			if( CPU->Y >= oper.value)
				CPU->SR |= C_FLAG;
			update_flags(CPU->Y - oper.value, N_FLAG | Z_FLAG);
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

		case EOR:
			if( inst.addr_mode != ADDR_IMMEDIATE ) {
				check_read_mapped_io(oper.address);
				oper.value = *(CPU->RAM + oper.address);
			}
			CPU->A ^= oper.value;
			update_flags(CPU->A, N_FLAG | Z_FLAG);
			break;

		case INC:
			check_read_mapped_io(oper.address);
			*(CPU->RAM + oper.address) = *(CPU->RAM + oper.address)+1; 
			check_write_mapped_io(oper.address);
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
			CPU->PC = oper.address - inst.size;
			break;

		case JSR:
			*(CPU->RAM + BEGIN_STACK + CPU->SP++) = (uint8_t)((CPU->PC+inst.size) & 0xFF);
			*(CPU->RAM + BEGIN_STACK + CPU->SP++) = (uint8_t)((CPU->PC+inst.size) >> 8);
			CPU->PC = oper.address - inst.size;
			break;

		case LDA:
			if( inst.addr_mode != ADDR_IMMEDIATE ) {
				check_read_mapped_io(oper.address);
				oper.value = *(CPU->RAM + oper.address);
			}
			CPU->A = oper.value;
			update_flags(CPU->A, N_FLAG | Z_FLAG);
			break;

		case LDX:
			if( inst.addr_mode != ADDR_IMMEDIATE ) {
				check_read_mapped_io(oper.address);
				oper.value = *(CPU->RAM + oper.address);
			}
			CPU->X = oper.value;
			update_flags(CPU->X, N_FLAG | Z_FLAG);
			break;

		case LDY:
			if( inst.addr_mode != ADDR_IMMEDIATE ) {
				check_read_mapped_io(oper.address);
				oper.value = *(CPU->RAM + oper.address);
			}
			CPU->Y = oper.value;
			update_flags(CPU->Y, N_FLAG | Z_FLAG);
			break;

		case LSR:
			if( inst.addr_mode == ADDR_ACCUM ) {
				if( CPU->A  & 0x1 )	
					CPU->SR |= C_FLAG;
				else
					CPU->SR &= ~C_FLAG;
				CPU->A >>= 1;
				update_flags(CPU->A, Z_FLAG);
			}
			else {
				if( *(CPU->RAM + oper.address)  & 0x1 )	
					CPU->SR |= C_FLAG;
				else
					CPU->SR &= ~C_FLAG;
				*(CPU->RAM + oper.address) = *(CPU->RAM + oper.address) << 1;
				update_flags(*(CPU->RAM + oper.address), Z_FLAG);
			}
			break;

		case NOP: /* Perfect implemenation 8-) */
			break;

		case ORA:
			if( inst.addr_mode != ADDR_IMMEDIATE ) {
				check_read_mapped_io(oper.address);
				oper.value = *(CPU->RAM + oper.address);
			}
			CPU->A |= oper.value;
			update_flags(CPU->A, N_FLAG | Z_FLAG);
			break;

		case PHA:
			*(CPU->RAM + BEGIN_STACK + CPU->SP++) = CPU->A;
			break;

		case PHP:
			*(CPU->RAM + BEGIN_STACK + CPU->SP++) = CPU->SR;
			break;

		case PLA:
			CPU->A = *(CPU->RAM + BEGIN_STACK + --CPU->SP);
			break;

		case PLP:
			CPU->SR = *(CPU->RAM + BEGIN_STACK + --CPU->SP);
			break;

		case ROL:
			if( inst.addr_mode == ADDR_ACCUM ) {
				tmp = CPU->A >> 7;
				CPU->A <<= 1;
				CPU->A |= (CPU->SR & C_FLAG);
				update_flags( CPU->A, N_FLAG | Z_FLAG);
			} else {
				check_read_mapped_io(oper.address);
				tmp = *(CPU->RAM + oper.address) >> 7;
				*(CPU->RAM + oper.address) = *(CPU->RAM + oper.address) << 1;
				*(CPU->RAM + oper.address) |= (CPU->SR & C_FLAG);
				check_write_mapped_io(oper.address);
				update_flags( *(CPU->RAM + oper.address), N_FLAG | Z_FLAG);
			}
			if( tmp )
				CPU->SR |= C_FLAG;
			else
				CPU->SR &= ~C_FLAG;
			break;

		case ROR:
			if( inst.addr_mode == ADDR_ACCUM ) {
				tmp = CPU->A & 0x1;
				CPU->A >>= 1;
				CPU->A |= (CPU->SR & C_FLAG) << 7;
				update_flags( CPU->A, N_FLAG | Z_FLAG);
			} else {
				check_read_mapped_io(oper.address);
				tmp = *(CPU->RAM + oper.address) & 0x1;
				*(CPU->RAM + oper.address) = *(CPU->RAM + oper.address) >> 1;
				*(CPU->RAM + oper.address) = *(CPU->RAM + oper.address) | ((CPU->SR & C_FLAG) << 7);
				check_write_mapped_io(oper.address);
				CPU->SR |= tmp;
				update_flags( *(CPU->RAM + oper.address), N_FLAG | Z_FLAG);
			}
			if( tmp )
				CPU->SR |= C_FLAG;
			else
				CPU->SR &= ~C_FLAG;
			break;

		case RTS:
			CPU->PC =  *(CPU->RAM + BEGIN_STACK + --CPU->SP) << 8;
			CPU->PC |= *(CPU->RAM + BEGIN_STACK + --CPU->SP);
			CPU->PC -= inst.size;
			break;

		case RTI:
			CPU->PC =  *(CPU->RAM + BEGIN_STACK + --CPU->SP) << 8;
			CPU->PC |= *(CPU->RAM + BEGIN_STACK + --CPU->SP);
			CPU->SR = *(CPU->RAM + BEGIN_STACK + --CPU->SP);
			CPU->PC -= inst.size;
			break;

		case SBC:
			if( inst.addr_mode != ADDR_IMMEDIATE ) {
				check_read_mapped_io(oper.address);
				oper.value = *(CPU->RAM + oper.address);
			}
			CPU->A = CPU->A - oper.value - (CPU->SR & C_FLAG);
			update_flags(CPU->A, N_FLAG | Z_FLAG);
			break;

		case SEC:
			CPU->SR |= C_FLAG;
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
			CPU->X = BEGIN_STACK + CPU->SP;
			update_flags(CPU->X, N_FLAG | Z_FLAG);
			break;

		case TXA:
			CPU->A = CPU->X;
			update_flags(CPU->A, N_FLAG | Z_FLAG);
			break;

		case TXS:
			CPU->SP = CPU->X;
			CPU->SP -= BEGIN_STACK;
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

	static unsigned int first_write = 1;
	static unsigned int strobe_pad = 0;
	int i;

	XTREME( if( 0x2000 <= address && address <= 0x2006 ) {
		printf("PPU: Write to PPU[%d]=$%02X PC=%04X\n", address - 0x2000, *(CPU->RAM + address), CPU->PC);
	} );

	switch( address ) {

		/* PPU control registers */
		case 0x2000:
			PPU->CR1 = *(CPU->RAM + 0x2000);
			break;
		case 0x2001:
			PPU->CR2 = *(CPU->RAM + 0x2001);
			switch( (PPU->CR2 & 0x0E0) >> 5 ) {

				case 0:
					*(PPU->VRAM + 0x3F00) = 0x1D;
					break;
				case 1:
					*(PPU->VRAM + 0x3F00) = 0x12;
					break;
				case 2:
					*(PPU->VRAM + 0x3F00) = 0x1A;
					break;
				case 3:
					*(PPU->VRAM + 0x3F00) = 0x16;
					break;

				default:
					fprintf(stderr,"Oops, bad background color? %02x\n", PPU->CR2);
			}
			break;

		/* SPR-RAM Address */
		case 0x2003:
			PPU->spr_addr = *(CPU->RAM + 0x2003);
			break;

		case 0x2004:
			XTREME( printf("Writing into SPR RAM at address %02x\n", PPU->spr_addr) );
			*(PPU->SPR_RAM + PPU->spr_addr++) = *(CPU->RAM + 0x2004);
			break;

		/* PPU VRAM address */
		case 0x2006:
			if( first_write ) {
				PPU->vram_addr = 0;
				PPU->vram_addr = (*(CPU->RAM + 0x2006) << 8);
				first_write = 0;
			} else {
				PPU->vram_addr |= *(CPU->RAM + 0x2006);
				first_write = 1;
			}
			break;

		/* Data written into PPU->vram_address */
		case 0x2007:
			*(PPU->VRAM + PPU->vram_addr) = *(CPU->RAM + 0x2007);
			if( PPU->CR1 & VERTICAL_WRITE)
				PPU->vram_addr += 32;
			else
				PPU->vram_addr++;
			break;

		/* Sprite DMA */
		case 0x4014:
			address = *(CPU->RAM + 0x4014)*0x100;
			for(i=0;i!=256;i++) 
				*(PPU->SPR_RAM + i) = *(CPU->RAM + address + i);
			CPU->cycles += 512;
			break;

		/* 1st joystick */			
		case 0x4016:
			if( *(CPU->RAM + 0x4016) == 0x01 ) {
				strobe_pad = 1;
			}
			else if( *(CPU->RAM + 0x4016) == 0x00 && strobe_pad ) {
				pads[0].reads = 0;
				strobe_pad = 0;
			}
			break;
	}

}

void check_read_mapped_io(uint16_t address) {

	/* PPU Control Register 1 */
	if( address == 0x2000 )
		*(CPU->RAM + 0x2000) = PPU->CR1;
	
	/* PPU Control Register 1 */
	if( address == 0x2001 )
		*(CPU->RAM + 0x2000) = PPU->CR2;

	/* PPU Status Register */
	else if( address == 0x2002 ) {
		*(CPU->RAM + 0x2002) = PPU->SR;
		PPU->SR &= ~VBLANK_FLAG;
	}

	/* SPR-RAM access */
	else if( address == 0x2004 ) 
		*(CPU->RAM + 0x2003) = *(PPU->SPR_RAM + PPU->spr_addr);

	/* PPU VRAM */
	else if( address == 0x2007 )
		*(CPU->RAM + 0x2007) = *(PPU->VRAM + PPU->vram_addr);

	/* 1st Joystick */
	else if( address == 0x4016 ) {
		pads[0].reads++;
		*(CPU->RAM + 0x4016) = 0;
		if( !pads[0].plugged )
			*(CPU->RAM + 0x4016) = 2; // bit 1 set if not plugged

		/* If we should return a key state... */
		if( pads[0].reads <= 8 ) {
			*(CPU->RAM + 0x4016) |= ((pads[0].pressed_keys >> (pads[0].reads-1)) & 0x1); 
		}

		if( pads[0].reads == 32 )
			pads[0].reads = 0;
	}
}

/* Note: NMI is executed after inscreasing the PC! */
void execute_nmi() {

	DEBUG( printf("Executing NMI!\n") );
	/* Push the PC and the SR */
	/* Finally, jump to the interrump vector */
	*(CPU->RAM + BEGIN_STACK + CPU->SP++) = CPU->SR;
	*(CPU->RAM + BEGIN_STACK + CPU->SP++) = (uint8_t)((CPU->PC) & 0xFF);
	*(CPU->RAM + BEGIN_STACK + CPU->SP++) = (uint8_t)((CPU->PC) >> 8);
	CPU->PC = (*(CPU->RAM + 0xFFFA) | (*(CPU->RAM + 0xFFFB)<<8) );

}
