#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "cpu.h"
#include "debug.h"
#include "instruction_set.h"
#include "pad.h"
#include "palette.h"
#include "ppu.h"

nes_cpu *CPU;

void initialize_cpu() {

	CPU = (nes_cpu *)malloc(sizeof(nes_cpu));
	CPU->cycles = 0;
	CPU->RAM = (uint8_t *)malloc(NES_RAM_SIZE);
	CPU->SP  = 0xff; /* It decrements when pushing, increments when pulling */
	CPU->reset = 1;

	return;
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

	printf("CPU Stack:\n==========\nStart: ");
	for( i=255; i!=(uint8_t)CPU->SP;i--) {
		printf("%02x ", CPU->RAM[BEGIN_STACK + i]);
	}
	printf("\n");

	return;
}

void init_cpu_ram(ines_file *file) {

	/* 1 ROM bank games load twice to ensure vector tables */
	/* Free the file ROM (we don't need it anymore) */
	if( file->romBanks == 1 ) {
		memcpy( CPU->RAM + 0x8000, file->rom, ROM_BANK_SIZE);
		memcpy( CPU->RAM + 0xC000, CPU->RAM + 0x8000, ROM_BANK_SIZE);
	}
	/* 2 ROM bank games load one in 0x8000 and other in 0xC000 */
	/* Free the file ROM (we don't need it anymore) */
	else if (file->romBanks == 2 ) {
		memcpy( CPU->RAM + 0x8000, file->rom, ROM_BANK_SIZE);
		memcpy( CPU->RAM + 0xC000, file->rom + ROM_BANK_SIZE, ROM_BANK_SIZE);
	}

}

void execute_instruction(instruction inst, operand oper) {

	uint8_t tmp;

	switch(inst.instr_id) {

		case ADC: /* TODO: check C and V flags */
			if( inst.addr_mode != ADDR_IMMEDIATE )
				oper.value = read_cpu_ram(oper.address);
			CPU->A += oper.value + (CPU->SR & C_FLAG);
			update_flags(CPU->A, N_FLAG | Z_FLAG);
			break;

		case AND:
			if( inst.addr_mode != ADDR_IMMEDIATE )
				oper.value = read_cpu_ram(oper.address);
			CPU->A &= oper.value;
			update_flags(CPU->A, N_FLAG | Z_FLAG );
			break;

		case ASL:
			if( inst.addr_mode == ADDR_ACCUM ) {
				tmp = CPU->A >> 7;
				CPU->A <<= 1;
				update_flags(CPU->A, N_FLAG | Z_FLAG);
			}
			else {
				oper.value = read_cpu_ram(oper.address);
				tmp = oper.value >> 7;
				write_cpu_ram(oper.value << 1, oper.address);
				update_flags(oper.value, N_FLAG | Z_FLAG);
			}
			if( tmp )	
				CPU->SR |= C_FLAG;
			else
				CPU->SR &= ~C_FLAG;
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
			oper.value = read_cpu_ram(oper.address);
			if( (oper.value >> 6)  & 0x01 )
				CPU->SR |= V_FLAG;
			else
				CPU->SR &= ~V_FLAG;
			update_flags(oper.value, N_FLAG);
			update_flags(oper.value & CPU->A, Z_FLAG);
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
			/* Finally, jump to the interrupt vector */
			CPU->SR |= B_FLAG;
			stack_push( CPU->SR );
			stack_push( (CPU->PC+2) & 0xFF );
			stack_push( (CPU->PC+2) >> 8 );
			CPU->PC = ( CPU->RAM[0xFFFE] | ( CPU->RAM[0xFFFF]<<8 ) );
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
			if( inst.addr_mode != ADDR_IMMEDIATE )
				oper.value = read_cpu_ram(oper.address);
			if( CPU->A >= oper.value)
				CPU->SR |= C_FLAG;
			else
				CPU->SR &= ~C_FLAG;
			update_flags(CPU->A - oper.value, N_FLAG | Z_FLAG);
			break;

		case CPX:
			if( inst.addr_mode != ADDR_IMMEDIATE )
				oper.value = read_cpu_ram(oper.address);
			if( CPU->X >= oper.value)
				CPU->SR |= C_FLAG;
			else
				CPU->SR &= ~C_FLAG;
			update_flags(CPU->X - oper.value, N_FLAG | Z_FLAG);
			break;

		case CPY:
			if( inst.addr_mode != ADDR_IMMEDIATE )
				oper.value = read_cpu_ram(oper.address);
			if( CPU->Y >= oper.value)
				CPU->SR |= C_FLAG;
			else
				CPU->SR &= ~C_FLAG;
			update_flags(CPU->Y - oper.value, N_FLAG | Z_FLAG);
			break;

		case DEC:
			tmp = read_cpu_ram(oper.address) - 1;
			write_cpu_ram(oper.address, tmp);
			update_flags( tmp , N_FLAG | Z_FLAG);
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
			if( inst.addr_mode != ADDR_IMMEDIATE )
				oper.value = read_cpu_ram(oper.address);
			CPU->A ^= oper.value;
			update_flags(CPU->A, N_FLAG | Z_FLAG);
			break;

		case INC:
			tmp = read_cpu_ram(oper.address) + 1;
			write_cpu_ram( oper.address, tmp);
			update_flags(tmp, N_FLAG | Z_FLAG);
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
			stack_push( (CPU->PC+inst.size) & 0xFF );
			stack_push( (CPU->PC+inst.size) >> 8 );
			CPU->PC = oper.address - inst.size;
			break;

		case LDA:
			if( inst.addr_mode != ADDR_IMMEDIATE )
				oper.value = read_cpu_ram(oper.address);
			CPU->A = oper.value;
			update_flags(CPU->A, N_FLAG | Z_FLAG);
			break;

		case LDX:
			if( inst.addr_mode != ADDR_IMMEDIATE )
				oper.value = read_cpu_ram(oper.address);
			CPU->X = oper.value;
			update_flags(CPU->X, N_FLAG | Z_FLAG);
			break;

		case LDY:
			if( inst.addr_mode != ADDR_IMMEDIATE )
				oper.value = read_cpu_ram(oper.address);
			CPU->Y = oper.value;
			update_flags(CPU->Y, N_FLAG | Z_FLAG);
			break;

		case LSR:
			if( inst.addr_mode == ADDR_ACCUM ) {
				tmp = CPU->A  & 0x1;
				CPU->A >>= 1;
				update_flags(CPU->A, Z_FLAG);
			}
			else {
				oper.value = read_cpu_ram(oper.address);
				tmp = oper.value & 0x1;
				oper.value <<= 1;
				write_cpu_ram( oper.address, oper.value );
				update_flags( oper.value , Z_FLAG);
			}
			if( tmp )
				CPU->SR |= C_FLAG;
			else
				CPU->SR &= ~C_FLAG;
			break;

		case NOP: /* Perfect implementation 8-) */
			break;

		case ORA:
			if( inst.addr_mode != ADDR_IMMEDIATE )
				oper.value = read_cpu_ram(oper.address);
			CPU->A |= oper.value;
			update_flags(CPU->A, N_FLAG | Z_FLAG);
			break;

		case PHA:
			stack_push( CPU->A );
			break;

		case PHP:
			stack_push( CPU->SR );
			break;

		case PLA:
			CPU->A = stack_pull();
			update_flags(CPU->A, N_FLAG | Z_FLAG);
			break;

		case PLP:
			CPU->SR = stack_pull();
			break;

		case ROL:
			if( inst.addr_mode == ADDR_ACCUM ) {
				tmp = CPU->A >> 7;
				CPU->A <<= 1;
				CPU->A |= (CPU->SR & C_FLAG);
				update_flags( CPU->A, N_FLAG | Z_FLAG);
			} else {
				oper.value = read_cpu_ram(oper.address);
				tmp = oper.value >> 7;
				oper.value <<= 1;
				oper.value |= (CPU->SR & C_FLAG);
				write_cpu_ram(oper.address, oper.value);
				update_flags( oper.value , N_FLAG | Z_FLAG);
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
				oper.value = read_cpu_ram(oper.address);
				tmp = oper.value & 0x1;
				oper.value >>= 1;
				oper.value |= ((CPU->SR & C_FLAG) << 7);
				write_cpu_ram(oper.address, oper.value);
				update_flags( oper.value , N_FLAG | Z_FLAG);
			}
			if( tmp )
				CPU->SR |= C_FLAG;
			else
				CPU->SR &= ~C_FLAG;
			break;

		case RTI:
			CPU->SR =  stack_pull();
			CPU->PC =  stack_pull() << 8;
			CPU->PC |= stack_pull();
			CPU->PC -= inst.size;
			break;

		case RTS:
			CPU->PC =  stack_pull() << 8;
			CPU->PC |= stack_pull();
			CPU->PC -= inst.size;
			break;

		case SBC:
			if( inst.addr_mode != ADDR_IMMEDIATE )
				oper.value = read_cpu_ram(oper.address);
			CPU->A = CPU->A - oper.value - (1 - (CPU->SR & C_FLAG));
			update_flags(CPU->A, N_FLAG | Z_FLAG);
			break;

		case SEC:
			CPU->SR |= C_FLAG;
			break;

		case SED:
			CPU->SR |= D_FLAG;
			break;

		case SEI:
			CPU->SR |= I_FLAG;
			break;

		case STA:
			write_cpu_ram(oper.address, CPU->A);
			break;

		case STX:
			write_cpu_ram(oper.address, CPU->X);
			break;

		case STY:
			write_cpu_ram(oper.address, CPU->Y);
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

void write_cpu_ram(uint16_t address, uint8_t value) {

	static unsigned int first_write = 1;
	static unsigned int strobe_pad = 0;
	int i;

	XTREME( if( 0x2000 <= address && address <= 0x2006 ) {
		printf("PPU: Write to PPU[%d]=$%02X PC=%04X\n", address - 0x2000, value, CPU->PC);
	} );

	/* Convert the address to handle mirroring */
	if( 0x0800 <= address && address < 0x2000 )
		address = address - 0x800 * ((address >> 11) & 0x3);

	if( 0x2008 <= address && address < 0x4000 )
		address = address - 0x8 * ((address >> 3) & 0x7FF);

	switch( address ) {

		/* PPU control registers */
		case 0x2000:
			PPU->CR1 = value;
			break;
		case 0x2001:
			PPU->CR2 = value;
			break;

		/* SPR-RAM Address */
		case 0x2003:
			PPU->spr_addr = value;
			break;

		case 0x2004:
			XTREME( printf("Writing into SPR RAM at address %02x\n", PPU->spr_addr) );
			*(PPU->SPR_RAM + PPU->spr_addr++) = value;
			break;

		/* PPU VRAM address */
		case 0x2006:
			if( first_write ) {
				PPU->vram_addr = 0;
				PPU->vram_addr = (value << 8);
				first_write = 0;
			} else {
				PPU->vram_addr |= value;
				first_write = 1;
			}
			break;

		/* Data written into PPU->vram_address */
		case 0x2007:
			if( !(PPU->SR & IGNORE_VRAM_WRITE) ) {
				*(PPU->VRAM + PPU->vram_addr) = value;
				if( PPU->CR1 & VERTICAL_WRITE)
					PPU->vram_addr += 32;
				else
					PPU->vram_addr++;
			}
			break;

		/* Sprite DMA */
		case 0x4014:
			address = value*0x100;
			for(i=0;i!=256;i++) 
				*(PPU->SPR_RAM + i) = *(CPU->RAM + address + i);
			CPU->cycles += 512;
			break;

		/* 1st joystick */			
		case 0x4016:
			if( value == 0x01 ) {
				strobe_pad = 1;
			}
			else if( value == 0x00 && strobe_pad ) {
				pads[0].reads = 0;
				strobe_pad = 0;
			}
			break;


		/* Normal RAM memory area */
		default:
			CPU->RAM[address] = value;
	}

	return;
}

uint8_t read_cpu_ram(uint16_t address) {

	uint8_t ret_val = 0;

	/* Convert the address to handle mirroring */
	if( 0x0800 <= address && address < 0x2000 )
		address = address - 0x800 * ((address >> 11) & 0x3);

	if( 0x2008 <= address && address < 0x4000 )
		address = address - 0x8 * ((address >> 3) & 0x7FF);

	/* PPU Control Register 1 */
	if( address == 0x2000 )
		ret_val = PPU->CR1;
	
	/* PPU Control Register 1 */
	if( address == 0x2001 )
		ret_val = PPU->CR2;

	/* PPU Status Register */
	else if( address == 0x2002 ) {
		ret_val = PPU->SR;
		PPU->SR &= ~VBLANK_FLAG;
	}

	/* SPR-RAM access */
	else if( address == 0x2004 ) 
		ret_val = *(PPU->SPR_RAM + PPU->spr_addr);

	/* PPU VRAM */
	else if( address == 0x2007 )
		ret_val = *(PPU->VRAM + PPU->vram_addr);

	/* 1st Joystick */
	else if( address == 0x4016 ) {
		pads[0].reads++;
		*(CPU->RAM + 0x4016) = 0;
		if( !pads[0].plugged )
			ret_val = 2; // bit 1 set if not plugged

		/* If we should return a key state... */
		if( pads[0].reads <= 8 ) {
			ret_val |= ((pads[0].pressed_keys >> (pads[0].reads-1)) & 0x1); 
		}

		if( pads[0].reads == 32 )
			pads[0].reads = 0;
	}

	/* Normal RAM area */
	else
		ret_val = CPU->RAM[address];

	return ret_val;
}


void stack_push(uint8_t value) {

	/* The stack is top down. When someone pushes, the SP decreases */
	/* We need to use write_cpu_ram because of the mirroring */
	write_cpu_ram( BEGIN_STACK + CPU->SP , value);
	CPU->SP--;

	return;
}

uint8_t stack_pull() {

	/* The stack is top down. When someone pulls, the SP increases */
	CPU->SP++;
	return CPU->RAM[BEGIN_STACK + CPU->SP];
}

/* Note: NMI is executed after inscreasing the PC! */
void execute_nmi() {

	DEBUG( printf("Executing NMI!\n") );
	/* Push the PC and the SR */
	/* Finally, jump to the interrupt vector */
	stack_push( (CPU->PC) & 0xFF );
	stack_push( (CPU->PC) >> 8 );
	stack_push( CPU->SR );
	CPU->PC = (*(CPU->RAM + 0xFFFA) | (*(CPU->RAM + 0xFFFB)<<8) );

}
