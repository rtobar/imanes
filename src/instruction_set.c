/*  ImaNES: I'm a NES. An intelligent NES emulator

    instruction_set.c   -    6502 CPU instruction set definition for ImaNES

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

#include "cpu.h"
#include "debug.h"
#include "instruction_set.h"

instruction *instructions;

void inst_lowercase(char *inst_name, char *ret) {

	int i;

	for(i=0;i!=3;i++)
		ret[i] = inst_name[i] + 32;
	ret[3] = '\0';

	return;
}

void initialize_instruction_set() {

	instructions = (instruction *)malloc(sizeof(instruction)*
	                                     OPCODES_NUMBER);

	
	/* ADC instruction */
	SET_INSTRUCTION_ADDR_DATA( ADC, IMMEDIATE, 0x69, 2, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ADC, ZEROPAGE,  0x65, 2, 3, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ADC, ZERO_INDX, 0x75, 2, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ADC, ABSOLUTE,  0x6D, 3, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ADC, ABS_INDX,  0x7D, 3, 4, PAGE);
	SET_INSTRUCTION_ADDR_DATA( ADC, ABS_INDY,  0x79, 3, 4, PAGE);
	SET_INSTRUCTION_ADDR_DATA( ADC, IND_INDIR, 0x61, 2, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ADC, INDIR_IND, 0x71, 2, 5, PAGE);

	/* AND instruction */
	SET_INSTRUCTION_ADDR_DATA( AND, IMMEDIATE, 0x29, 2, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( AND, ZEROPAGE,  0x25, 2, 3, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( AND, ZERO_INDX, 0x35, 2, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( AND, ABSOLUTE,  0x2D, 3, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( AND, ABS_INDX,  0x3D, 3, 4, PAGE);
	SET_INSTRUCTION_ADDR_DATA( AND, ABS_INDY,  0x39, 3, 4, PAGE);
	SET_INSTRUCTION_ADDR_DATA( AND, IND_INDIR, 0x21, 2, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( AND, INDIR_IND, 0x31, 2, 5, PAGE);

	/* ASL instruction */
	SET_INSTRUCTION_ADDR_DATA( ASL, ACCUM,     0x0A, 1, 2, NORMAL); 
	SET_INSTRUCTION_ADDR_DATA( ASL, ZEROPAGE,  0x06, 2, 5, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ASL, ZERO_INDX, 0x16, 2, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ASL, ABSOLUTE,  0x0E, 3, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ASL, ABS_INDX,  0x1E, 3, 7, NORMAL);

	/* BCC instruction */
	SET_INSTRUCTION_ADDR_DATA( BCC, RELATIVE, 0x90, 2, 2, BRANCH);

	/* BCS instruction */
	SET_INSTRUCTION_ADDR_DATA( BCS, RELATIVE, 0xB0, 2, 2, BRANCH);

	/* BEQ instruction */
	SET_INSTRUCTION_ADDR_DATA( BEQ, RELATIVE, 0xF0, 2, 2, BRANCH);

	/* BIT instruction */
	SET_INSTRUCTION_ADDR_DATA( BIT, ZEROPAGE, 0x24, 2, 3, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( BIT, ABSOLUTE, 0x2C, 3, 4, NORMAL);

	/* BMI instruction */
	SET_INSTRUCTION_ADDR_DATA( BMI, RELATIVE, 0x30, 2, 2, BRANCH);

	/* BNE instruction */
	SET_INSTRUCTION_ADDR_DATA( BNE, RELATIVE, 0xD0, 2, 2, BRANCH);
	
	/* BPL instruction */
	SET_INSTRUCTION_ADDR_DATA( BPL, RELATIVE, 0x10, 2, 2, BRANCH);

	/* BRK instruction */
	SET_INSTRUCTION_ADDR_DATA( BRK, IMPLIED, 0x00, 1, 7, NORMAL);

	/* BVC instruction */
	SET_INSTRUCTION_ADDR_DATA( BVC, RELATIVE, 0x50, 2, 2, BRANCH);

	/* BVS instruction */
	SET_INSTRUCTION_ADDR_DATA( BVS, RELATIVE, 0x70, 2, 2, BRANCH);

	/* CLC instruction */ 
	SET_INSTRUCTION_ADDR_DATA( CLC, IMPLIED, 0x18, 1, 2, NORMAL);

	/* CLD instruction */ 
	SET_INSTRUCTION_ADDR_DATA( CLD, IMPLIED, 0xD8, 1, 2, NORMAL);

	/* CLI Instruction */
	SET_INSTRUCTION_ADDR_DATA( CLI, IMPLIED, 0x58, 1, 2, NORMAL);

	/* CLV Instruction */
	SET_INSTRUCTION_ADDR_DATA( CLV, IMPLIED, 0xB8, 1, 2, NORMAL);

	/* CMP instruction */
	SET_INSTRUCTION_ADDR_DATA( CMP, IMMEDIATE, 0xC9, 2, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( CMP, ZEROPAGE,  0xC5, 2, 3, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( CMP, ZERO_INDX, 0xD5, 2, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( CMP, ABSOLUTE,  0xCD, 3, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( CMP, ABS_INDX,  0xDD, 3, 4, PAGE);
	SET_INSTRUCTION_ADDR_DATA( CMP, ABS_INDY,  0xD9, 3, 4, PAGE);
	SET_INSTRUCTION_ADDR_DATA( CMP, IND_INDIR, 0xC1, 2, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( CMP, INDIR_IND, 0xD1, 2, 5, PAGE);

	/* CPX instruction */
	SET_INSTRUCTION_ADDR_DATA( CPX, IMMEDIATE, 0xE0, 2, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( CPX, ZEROPAGE,  0xE4, 2, 3, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( CPX, ABSOLUTE,  0xEC, 3, 4, NORMAL);

	/* CPY instruction */
	SET_INSTRUCTION_ADDR_DATA( CPY, IMMEDIATE, 0xC0, 2, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( CPY, ZEROPAGE,  0xC4, 2, 3, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( CPY, ABSOLUTE,  0xCC, 3, 4, NORMAL);

	/* DEC instruction */
	SET_INSTRUCTION_ADDR_DATA( DEC, ZEROPAGE,  0xC6, 2, 5, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( DEC, ZERO_INDX, 0xD6, 2, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( DEC, ABSOLUTE,  0xCE, 3, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( DEC, ABS_INDX,  0xDE, 3, 7, NORMAL);

	/* DEX instruction */
	SET_INSTRUCTION_ADDR_DATA( DEX, IMPLIED, 0xCA, 1, 2, NORMAL);

	/* DEY instruction */
	SET_INSTRUCTION_ADDR_DATA( DEY, IMPLIED, 0x88, 1, 2, NORMAL);

	/* EOR instruction */
	SET_INSTRUCTION_ADDR_DATA( EOR, IMMEDIATE, 0x49, 2, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( EOR, ZEROPAGE,  0x45, 2, 3, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( EOR, ZERO_INDX, 0x55, 2, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( EOR, ABSOLUTE,  0x4D, 3, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( EOR, ABS_INDX,  0x5D, 3, 4, PAGE);
	SET_INSTRUCTION_ADDR_DATA( EOR, ABS_INDY,  0x59, 3, 4, PAGE);
	SET_INSTRUCTION_ADDR_DATA( EOR, IND_INDIR, 0x41, 2, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( EOR, INDIR_IND, 0x51, 2, 5, PAGE);


	/* INC instruction */
	SET_INSTRUCTION_ADDR_DATA( INC, ZEROPAGE,  0xE6, 2, 5, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( INC, ZERO_INDX, 0xF6, 2, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( INC, ABSOLUTE,  0xEE, 3, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( INC, ABS_INDX,  0xFE, 3, 7, NORMAL);

	/* INX instruction */
	SET_INSTRUCTION_ADDR_DATA( INX, IMPLIED, 0xE8, 1, 2, NORMAL);

	/* INY instruction */
	SET_INSTRUCTION_ADDR_DATA( INY, IMPLIED, 0xC8, 1, 2, NORMAL);

	/* JMP instruction*/
	SET_INSTRUCTION_ADDR_DATA( JMP, ABSOLUTE, 0x4C, 3, 3, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( JMP, INDIRECT, 0x6C, 3, 5, NORMAL);

	/* JSR instruction */
	SET_INSTRUCTION_ADDR_DATA( JSR, ABSOLUTE, 0x20, 3, 6, NORMAL);

	/* LDA instruction */
	SET_INSTRUCTION_ADDR_DATA( LDA, IMMEDIATE, 0xA9, 2, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( LDA, ZEROPAGE,  0xA5, 2, 3, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( LDA, ZERO_INDX, 0xB5, 2, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( LDA, ABSOLUTE,  0xAD, 3, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( LDA, ABS_INDX,  0xBD, 3, 4, PAGE);
	SET_INSTRUCTION_ADDR_DATA( LDA, ABS_INDY,  0xB9, 3, 4, PAGE);
	SET_INSTRUCTION_ADDR_DATA( LDA, IND_INDIR, 0xA1, 2, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( LDA, INDIR_IND, 0xB1, 2, 5, PAGE);
	
	/* LDX instruction */
	SET_INSTRUCTION_ADDR_DATA( LDX, IMMEDIATE, 0xA2, 2, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( LDX, ZEROPAGE,  0xA6, 2, 3, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( LDX, ZERO_INDY, 0xB6, 2, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( LDX, ABSOLUTE,  0xAE, 3, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( LDX, ABS_INDY,  0xBE, 3, 4, PAGE);

	/* LDY instruction */
	SET_INSTRUCTION_ADDR_DATA( LDY, IMMEDIATE, 0xA0, 2, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( LDY, ZEROPAGE,  0xA4, 2, 3, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( LDY, ZERO_INDX, 0xB4, 2, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( LDY, ABSOLUTE,  0xAC, 3, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( LDY, ABS_INDX,  0xBC, 3, 4, PAGE);

	/* LSR instruction */
	SET_INSTRUCTION_ADDR_DATA( LSR, ACCUM,     0x4A, 1, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( LSR, ZEROPAGE,  0x46, 2, 5, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( LSR, ZERO_INDX, 0x56, 2, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( LSR, ABSOLUTE,  0x4E, 3, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( LSR, ABS_INDX,  0x5E, 3, 7, NORMAL);

	/* NOP instruction */
	SET_INSTRUCTION_ADDR_DATA( NOP, IMPLIED, 0xEA, 1, 2, NORMAL);

	/* ORA instruction */
	SET_INSTRUCTION_ADDR_DATA( ORA, IMMEDIATE, 0x09, 2, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ORA, ZEROPAGE,  0x05, 2, 3, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ORA, ZERO_INDX, 0x15, 2, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ORA, ABSOLUTE,  0x0D, 3, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ORA, ABS_INDX,  0x1D, 3, 4, PAGE);
	SET_INSTRUCTION_ADDR_DATA( ORA, ABS_INDY,  0x19, 3, 4, PAGE);
	SET_INSTRUCTION_ADDR_DATA( ORA, IND_INDIR, 0x01, 2, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ORA, INDIR_IND, 0x11, 2, 5, PAGE);

	/* PHA instruction */
	SET_INSTRUCTION_ADDR_DATA( PHA, IMPLIED, 0x48, 1, 3, NORMAL);

	/* PHP instruction */
	SET_INSTRUCTION_ADDR_DATA( PHP, IMPLIED, 0x08, 1, 3, NORMAL);

	/* PLA instruction */
	SET_INSTRUCTION_ADDR_DATA( PLA, IMPLIED, 0x68, 1, 4, NORMAL);

	/* PLP instruction */
	SET_INSTRUCTION_ADDR_DATA( PLP, IMPLIED, 0x28, 1, 4, NORMAL);

	/* ROL instruction */
	SET_INSTRUCTION_ADDR_DATA( ROL, ACCUM,     0x2A, 1, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ROL, ZEROPAGE,  0x26, 2, 5, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ROL, ZERO_INDX, 0x36, 2, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ROL, ABSOLUTE,  0x2E, 3, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ROL, ABS_INDX,  0x3E, 3, 7, NORMAL);

	/* ROR instruction */
	SET_INSTRUCTION_ADDR_DATA( ROR, ACCUM,     0x6A, 1, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ROR, ZEROPAGE,  0x66, 2, 5, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ROR, ZERO_INDX, 0x76, 2, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ROR, ABSOLUTE,  0x6E, 3, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ROR, ABS_INDX,  0x7E, 3, 7, NORMAL);

	/* RTI instruction */
	SET_INSTRUCTION_ADDR_DATA( RTI, IMPLIED, 0x40, 1, 6, NORMAL);

	/* RTS instruction */
	SET_INSTRUCTION_ADDR_DATA( RTS, IMPLIED, 0x60, 1, 6, NORMAL);

	/* SBC instruction */
	SET_INSTRUCTION_ADDR_DATA( SBC, IMMEDIATE, 0xE9, 2, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( SBC, ZEROPAGE,  0xE5, 2, 3, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( SBC, ZERO_INDX, 0xF5, 2, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( SBC, ABSOLUTE,  0xED, 3, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( SBC, ABS_INDX,  0xFD, 3, 4, PAGE);
	SET_INSTRUCTION_ADDR_DATA( SBC, ABS_INDY,  0xF9, 3, 4, PAGE);
	SET_INSTRUCTION_ADDR_DATA( SBC, IND_INDIR, 0xE1, 2, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( SBC, INDIR_IND, 0xF1, 2, 5, PAGE);

	/* SEC instruction */
	SET_INSTRUCTION_ADDR_DATA( SEC, IMPLIED, 0x38, 1, 2, NORMAL);

	/* SED instruction */
	SET_INSTRUCTION_ADDR_DATA( SED, IMPLIED, 0xF8, 1, 2, NORMAL);

	/* SEI instruction */
	SET_INSTRUCTION_ADDR_DATA( SEI, IMPLIED, 0x78, 1, 2, NORMAL);

	/* STA instruction */
	SET_INSTRUCTION_ADDR_DATA( STA, ZEROPAGE,  0x85, 2, 3, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( STA, ZERO_INDX, 0x95, 2, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( STA, ABSOLUTE,  0x8D, 3, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( STA, ABS_INDX,  0x9D, 3, 5, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( STA, ABS_INDY,  0x99, 3, 5, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( STA, IND_INDIR, 0x81, 2, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( STA, INDIR_IND, 0x91, 2, 6, NORMAL);

	/* STX instruction */
	SET_INSTRUCTION_ADDR_DATA( STX, ZEROPAGE,  0x86, 2, 3, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( STX, ZERO_INDY, 0x96, 2, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( STX, ABSOLUTE,  0x8E, 3, 4, NORMAL);

	/* STY instruction */
	SET_INSTRUCTION_ADDR_DATA( STY, ZEROPAGE,  0x84, 2, 3, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( STY, ZERO_INDX, 0x94, 2, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( STY, ABSOLUTE,  0x8C, 3, 4, NORMAL);

	/* TAX instruction */
	SET_INSTRUCTION_ADDR_DATA( TAX, IMPLIED, 0xAA, 1, 2, NORMAL);

	/* TAY instruction */
	SET_INSTRUCTION_ADDR_DATA( TAY, IMPLIED, 0xA8, 1, 2, NORMAL);

	/* TSX instruction */
	SET_INSTRUCTION_ADDR_DATA( TSX, IMPLIED, 0xBA, 1, 2, NORMAL);

	/* TXA instruction */
	SET_INSTRUCTION_ADDR_DATA( TXA, IMPLIED, 0x8A, 1, 2, NORMAL);

	/* TXS instruction */
	SET_INSTRUCTION_ADDR_DATA( TXS, IMPLIED, 0x9A, 1, 2, NORMAL);

	/* TYA instruction */
	SET_INSTRUCTION_ADDR_DATA( TYA, IMPLIED, 0x98, 1, 2, NORMAL);


	/** Illegal opcodes **/
	/* LAX instruction */
	SET_INSTRUCTION_ADDR_DATA( LAX, IMMEDIATE, 0xAB, 2, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( LAX, ZEROPAGE,  0xA7, 2, 3, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( LAX, ZERO_INDY, 0xB7, 2, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( LAX, ABSOLUTE,  0xAF, 3, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( LAX, ABS_INDY,  0xBF, 3, 4, PAGE);
	SET_INSTRUCTION_ADDR_DATA( LAX, IND_INDIR, 0xA3, 2, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( LAX, INDIR_IND, 0xB3, 2, 5, PAGE);
	
	return;
}

operand get_operand(instruction inst, uint16_t inst_address) {

	uint16_t address;
	char lower_name[4];
	operand oper = { 0xDEAD, 0xBE};

	DEBUG( inst_lowercase(inst.name, lower_name) );
	DEBUG( printf("%s", lower_name) );

	switch( inst.addr_mode ) {

		case ADDR_IMMEDIATE:
			oper.value = CPU->RAM[inst_address+1];
			DEBUG( printf(" #$%02x", oper.value) );
			break;

		case ADDR_ABSOLUTE:
			oper.address = CPU->RAM[inst_address+1] | (CPU->RAM[inst_address + 2]  << 8);
			DEBUG( printf(" $%04x", oper.address) );
			break;

		case ADDR_ZEROPAGE:
			oper.address = CPU->RAM[inst_address+1];
			DEBUG( printf(" $%02x", oper.address) );
			break;

		case ADDR_IMPLIED:
			break;

		/* For indirect addressing, we need to use read_cpu_ram*/
		case ADDR_INDIRECT:
			address = CPU->RAM[inst_address+1] | (CPU->RAM[inst_address+2] << 8);
			DEBUG( printf(" ($%04x)", address) );
			oper.address =  read_cpu_ram(address);

			/* If address is $xxFF, the next read wraps page */
			if( address%0x100 == 0xFF )
				address -= 0x100;
			oper.address |= (read_cpu_ram(address+1) << 8);
			break;

		case ADDR_ABS_INDX:
			address = ( CPU->RAM[inst_address+1] | (CPU->RAM[inst_address+2] << 8) );
			DEBUG( printf(" $%04x,X", address) );
			oper.address = address + CPU->X;
			if( ((address&0x100) != (oper.address&0x100)) &&
			    inst.cycle_change == CYCLE_PAGE )
				CPU->cycles++;
			break;

		case ADDR_ABS_INDY:
			address = ( CPU->RAM[inst_address+1] | (CPU->RAM[inst_address+2] << 8) );
			DEBUG( printf(" $%04x,Y", address) );
			oper.address = address + CPU->Y;
			if( ((address&0x100) != (oper.address&0x100)) &&
			    inst.cycle_change == CYCLE_PAGE )
				CPU->cycles++;
			break;

		case ADDR_IND_INDIR:
			address = CPU->RAM[inst_address+1] + CPU->X;
			DEBUG( printf(" ($%02x,X)", address - CPU->X) );
			if( address > 0xFF )
				address -= 0x100;
			oper.address = read_cpu_ram(address);
			if( address == 0xFF )
				address -= 0x100;
			oper.address |= read_cpu_ram(address+1) << 8;
			break;

		case ADDR_INDIR_IND:
			address = CPU->RAM[inst_address+1];
			DEBUG( printf(" ($%02x),Y", address) );
			oper.address = read_cpu_ram(address);
			if( address == 0xFF )
				address -= 0x100;
			oper.address |= read_cpu_ram(address+1) << 8;
			if( ((oper.address&0x100) != ((oper.address+CPU->Y)&0x100)) &&
			    inst.cycle_change == CYCLE_PAGE )
				CPU->cycles++;
			oper.address += CPU->Y;
			break;

		case ADDR_RELATIVE:
			oper.value = CPU->RAM[inst_address+1];
			DEBUG( printf(" $%02x", oper.value) );
			break;

		case ADDR_ACCUM:
			DEBUG( printf(" A") );
			break;

		/* Wrap around if resulting address is out of zeropage */
		case ADDR_ZERO_INDX:
			oper.address = CPU->RAM[inst_address+1] + CPU->X;
			DEBUG( printf(" $%02x,X", oper.address - CPU->X) );
			if( oper.address >= 0x0100 )
				oper.address -= 0x100;
			break;

		case ADDR_ZERO_INDY:
			oper.address = CPU->RAM[inst_address+1] + CPU->Y;
			DEBUG( printf(" $%02x,Y", oper.address - CPU->Y) );
			if( oper.address >= 0x0100 )
				oper.address -= 0x100;
			break;

		default:
			fprintf(stderr,"Hey!!! You haven't written the %d addressing mode!!!\n", inst.addr_mode);
	}

	DEBUG( printf("\n") );
	return oper;
}

