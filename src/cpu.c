/*  ImaNES: I'm a NES. An intelligent NES emulator

    cpu.c   -    CPU emulation under ImaNES

    Copyright (C) 2008   Rodrigo Tobar Carrizo

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "cpu.h"
#include "debug.h"
#include "instruction_set.h"
#include "mapper.h"
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
	CPU->sram_enabled = 0;
	CPU->sram_enabled &= ~SRAM_ENABLE;
	CPU->SR = R_FLAG; /* It is never ever used, but always set */

	return;
}

void dump_cpu() {

	printf("A:%02x  ", CPU->A);
	printf("P:%02x  ", CPU->SR);
	printf("X:%02x  ", CPU->X);
	printf("Y:%02x  ", CPU->Y);
	printf("SP:01%02x  ", CPU->SP);
	printf("PC:%04x   ", CPU->PC);
	printf("Flags:[");
	printf("%s", (CPU->SR & N_FLAG) ?    "N" : "." );
	printf("%sR", (CPU->SR & V_FLAG) ?   "V" : "." );
	printf("%s", (CPU->SR & B_FLAG) ?    "B" : "." );
	printf("%s", (CPU->SR & D_FLAG) ?    "D" : "." );
	printf("%s", (CPU->SR & I_FLAG) ?    "I" : "." );
	printf("%s", (CPU->SR & Z_FLAG) ?    "Z" : "." );
	printf("%s]\n", (CPU->SR & C_FLAG) ? "C" : "." );

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
	mapper->reset();
}

void execute_instruction(instruction inst, operand oper) {

	uint8_t  tmp;
	uint16_t tmp16;

	switch(inst.instr_id) {

		case ADC: /* TODO: check C and V flags */
			if( inst.addr_mode != ADDR_IMMEDIATE )
				oper.value = read_cpu_ram(oper.address);
			tmp16 = CPU->A + oper.value + (CPU->SR & C_FLAG);

			/* If result is over 0xFF, then the carry is 1 */
			if( tmp16 > 0xFF )
				CPU->SR |= C_FLAG;
			else
				CPU->SR &= ~C_FLAG;

			/* Set overflow flag if needed */
			if( ( ((CPU->A^tmp16)      & 0x80) != 0 ) &&
			    ( ((CPU->A^oper.value) & 0x80) == 0 ) )
				CPU->SR |= V_FLAG;
			else
				CPU->SR &= ~V_FLAG;

			CPU->A = tmp16 & 0xFF; /* Truncate to 8 bits */
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
				tmp = CPU->A & 0x80;
				CPU->A <<= 1;
				update_flags(CPU->A, N_FLAG | Z_FLAG);
			}
			else {
				oper.value = read_cpu_ram(oper.address);
				tmp = oper.value & 0x80;
				oper.value <<= 1;
				update_flags(oper.value, N_FLAG | Z_FLAG);
				write_cpu_ram(oper.address, oper.value);
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
			execute_irq();
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
			stack_push( (CPU->PC+2) >> 8 );
			stack_push( (CPU->PC+2) & 0xFF );
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
				tmp = CPU->A & 0x1;
				CPU->A >>= 1;
				update_flags(CPU->A, Z_FLAG | N_FLAG);
			}
			else {
				oper.value = read_cpu_ram(oper.address);
				tmp = oper.value & 0x1;
				oper.value >>= 1;
				write_cpu_ram( oper.address, oper.value );
				update_flags( oper.value , Z_FLAG | N_FLAG);
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
			CPU->SR |= B_FLAG;
			stack_push( CPU->SR );
			break;

		case PLA:
			CPU->A = stack_pull();
			update_flags(CPU->A, N_FLAG | Z_FLAG);
			break;

		case PLP:
			CPU->SR = stack_pull();
			CPU->SR |= R_FLAG; /* R_FLAG should be _always_ set */
			break;

		case ROL:
			if( inst.addr_mode == ADDR_ACCUM ) {
				tmp = CPU->A & 0x80;
				CPU->A <<= 1;
				CPU->A |= (CPU->SR & C_FLAG);
				update_flags( CPU->A, N_FLAG | Z_FLAG);
			} else {
				oper.value = read_cpu_ram(oper.address);
				tmp = oper.value & 0x80;
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
			CPU->SR |= R_FLAG; /* R_FLAG should be _always_ set */
			CPU->PC =  stack_pull();
			CPU->PC |= stack_pull() << 8;
			CPU->PC -= inst.size;
			break;

		case RTS:
			CPU->PC =  stack_pull();
			CPU->PC |= stack_pull() << 8;
			CPU->PC++;
			CPU->PC -= inst.size;
			break;

		case SBC:
			if( inst.addr_mode != ADDR_IMMEDIATE )
				oper.value = read_cpu_ram(oper.address);
			tmp16 = CPU->A - oper.value - (1 - (CPU->SR & C_FLAG));

			/* If result is over 0xFF, then the carry is 1 */
			if( tmp16 >= 0xFF )
				CPU->SR &= ~C_FLAG;
			else
				CPU->SR |= C_FLAG;

			/* Set overflow flag if needed */
			if( ( ((CPU->A^tmp16)      & 0x80) != 0 ) &&
			    ( ((CPU->A^oper.value) & 0x80) != 0 ) )
				CPU->SR |= V_FLAG;
			else
				CPU->SR &= ~V_FLAG;

			CPU->A = tmp16 & 0xFF; /* truncate to 8 bits */

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

inline void update_flags(int8_t value, uint8_t flags) {

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

	static unsigned int strobe_pad = 0;
	int i;

	XTREME( if( 0x2000 <= address && address <= 0x2006 ) {
		printf("PPU: Write to PPU[%d]=$%02X PC=%04X\n", address - 0x2000, value, CPU->PC);
	} );

	/* Convert the address to handle mirroring */
	if( 0x0800 <= address && address < 0x2000 ) {
		DEBUG( printf("CPU Address mirroring: from %04x to ", address) );
		address = address - 0x800 * ((address >> 11) & 0x3);
		DEBUG( printf("%04x\n",address) );
	}

	if( 0x2008 <= address && address < 0x4000 ) {
		DEBUG( printf("CPU Address mirroring: from %04x to ", address) );
		address = address - 0x8 * ((address >> 3) & 0x7FF);
		DEBUG( printf("%04x\n",address) );
	}

	/* SRAM can be disabled or in RO mode */
	if( 0x6000 <= address && address < 0x8000 && 
	 ( !(CPU->sram_enabled&SRAM_ENABLE) || CPU->sram_enabled&SRAM_RO ) ) {
		DEBUG( printf("Write to %04x not allowed\n", address) );
		return;
	}

	switch( address ) {

		/* PPU control registers */
		case 0x2000:
			PPU->CR1 = value;
			PPU->temp_addr = (PPU->temp_addr&0xF3FF) | ((value&0x3)<<10);
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
			PPU->SPR_RAM[PPU->spr_addr++] = value;
			break;

		case 0x2005:
			/* First write */
			if( PPU->latch ) {
				PPU->x = value & 0x7;
				PPU->temp_addr = (PPU->temp_addr&0xFFE0) | ((value&0xF8)>>3);

				/* This is, anyways, still correct, since I draw
				 * the entire line at once in the same thread. This means
				 * that setting PPU->x will not affect at all in the
				 * drawing of the actual line */
				PPU->latch = 0;
			}
			/* Second write */
			else {
				PPU->temp_addr = (PPU->temp_addr&0xFC1F) | ((value&0xF8)<<2);
				PPU->temp_addr = (PPU->temp_addr&0x8FFF) | ((value&0x7)<<12);
				PPU->latch = 1;
			}
			break;

		/* PPU VRAM address */
		case 0x2006:
			/* First write */
			if( PPU->latch ) {
				PPU->temp_addr = (PPU->temp_addr&0x00FF) | ((value&0x3F)<<8);
				PPU->latch = 0;
			}
			/* Second write */
			else {
				PPU->temp_addr = (PPU->temp_addr&0xFF00) | value;
				PPU->vram_addr = PPU->temp_addr;
				PPU->latch = 1;
			}
			break;

		/* Data written into PPU->vram_address */
		case 0x2007:
			if( !(PPU->SR & IGNORE_VRAM_WRITE) ) {
				write_ppu_vram(PPU->vram_addr, value);
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
				PPU->SPR_RAM[i] = read_cpu_ram(address+i);
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

	/* Check if mapper need to come into action */
	if( mapper->check_address(address) ) {
		mapper->switch_banks();
		return;
	}

	XTREME( 
	printf("%04x: ", address & 0xfff0 );
	for(i=0;i!=0x10;i++)
		printf("%02x ", CPU->RAM[(address&0xfff0) + i]);
	printf("\n");
	);

	return;
}

uint8_t read_cpu_ram(uint16_t address) {

	uint8_t ret_val = 0;

	/* Convert the address to handle mirroring */
	if( 0x0800 <= address && address < 0x2000 ) {
		DEBUG( printf("CPU Adress mirroring: from %04x to ", address) );
		address = address - 0x800 * ((address >> 11) & 0x3);
		DEBUG( printf("%04x\n",address) );
	}

	if( 0x2008 <= address && address < 0x4000 ) {
		DEBUG( printf("CPU Address mirroring: from %04x to ", address) );
		address = address - 0x8 * ((address >> 3) & 0x7FF);
		DEBUG( printf("%04x\n",address) );
	}

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
		PPU->latch = 1;
	}

	/* SPR-RAM access */
	else if( address == 0x2004 ) 
		ret_val = *(PPU->SPR_RAM + PPU->spr_addr);

	/* PPU VRAM */
	else if( address == 0x2007 )
		ret_val = read_ppu_vram(PPU->vram_addr);

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

	/* SRAM area */
	else if( 0x6000 <= address && address < 0x8000 ) {
		/* SRAM diabled */
		if( !(CPU->sram_enabled & SRAM_ENABLE) )
			return 0;
		else
			return CPU->RAM[address];
	}

	/* Normal RAM area */
	else
		ret_val = CPU->RAM[address];

	XTREME( printf("Returning %02x from %04x\n", ret_val, address) );
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

	/* NMI clears the B_FLAG from CPU status */
	CPU->SR &= ~B_FLAG;

	/* Push the PC and the SR */
	/* Finally, jump to the interrupt vector */
	stack_push( CPU->PC >> 8 );
	stack_push( CPU->PC & 0xFF );
	stack_push( CPU->SR );
	CPU->PC = (*(CPU->RAM + 0xFFFA) | (*(CPU->RAM + 0xFFFB)<<8) );

}

void execute_reset() {

	/* Let the mapper do its stuff */
	mapper->reset();

	/* Now, let's search for the RESET vector and point CPU->PC there */
	CPU->PC = *(CPU->RAM + 0xFFFC) | ( *(CPU->RAM + 0xFFFD) << 8 );
	CPU->reset = 0;
}

/* This is called by BRK and mappers IRQ triggers */
void execute_irq() {

	stack_push( (CPU->PC+2) & 0xFF );
	stack_push( (CPU->PC+2) >> 8 );
	stack_push( CPU->SR );
	CPU->PC = ( CPU->RAM[0xFFFE] | ( CPU->RAM[0xFFFF]<<8 ) );
}
