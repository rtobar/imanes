/*  ImaNES: I'm a NES. An intelligent NES emulator

    instruction_set.c   -    6502 CPU instruction set definition for ImaNES

    Copyright (C) 2009   Rodrigo Tobar Carrizo

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

#include "clock.h"
#include "common.h"
#include "cpu.h"
#include "debug.h"
#include "i18n.h"
#include "instruction_set.h"

instruction instructions[OPCODES_NUMBER];

void initialize_instruction_set() {

	/*********************/
	/** "Legal" opcodes **/
	/*********************/
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


	/*********************/
	/** Illegal opcodes **/
	/*********************/
	/* AHX instruction (it seems not to be present in the 6502) */
	SET_INSTRUCTION_ADDR_DATA( AHX, INDIR_IND, 0x93, 2, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( AHX, ABS_INDY,  0x9F, 3, 5, NORMAL);

	/* ANC instruction (it comes in two flavors) */
	SET_INSTRUCTION_ADDR_DATA( ANC, IMMEDIATE, 0x0B, 2, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ANC, IMMEDIATE, 0x2B, 2, 2, NORMAL);

	/* ALR instruction */
	SET_INSTRUCTION_ADDR_DATA( ALR, IMMEDIATE, 0x4B, 2, 2, NORMAL);

	/* ARR instrunction */
	SET_INSTRUCTION_ADDR_DATA( ARR, IMMEDIATE, 0x6B, 2, 2, NORMAL);

	/* DCP instruction */
	SET_INSTRUCTION_ADDR_DATA( DCP, ZEROPAGE,  0xC7, 2, 5, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( DCP, ZERO_INDX, 0xD7, 2, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( DCP, ABSOLUTE,  0xCF, 3, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( DCP, ABS_INDX,  0xDF, 3, 7, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( DCP, ABS_INDY,  0xDB, 3, 7, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( DCP, IND_INDIR, 0xC3, 2, 8, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( DCP, INDIR_IND, 0xD3, 2, 8, NORMAL);

	/* ISC instruction */
	SET_INSTRUCTION_ADDR_DATA( ISC, ZEROPAGE,  0xE7, 2, 5, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ISC, ZERO_INDX, 0xF7, 2, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ISC, ABSOLUTE,  0xEF, 3, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ISC, ABS_INDX,  0xFF, 3, 7, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ISC, ABS_INDY,  0xFB, 3, 7, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ISC, IND_INDIR, 0xE3, 2, 8, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ISC, INDIR_IND, 0xF3, 2, 8, NORMAL);

	/* LAS instruction */
	SET_INSTRUCTION_ADDR_DATA( LAS, ABS_INDY,  0xBB, 3, 4, PAGE);

	/* LAX instruction */
	SET_INSTRUCTION_ADDR_DATA( LAX, IMMEDIATE, 0xAB, 2, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( LAX, ZEROPAGE,  0xA7, 2, 3, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( LAX, ZERO_INDY, 0xB7, 2, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( LAX, ABSOLUTE,  0xAF, 3, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( LAX, ABS_INDY,  0xBF, 3, 4, PAGE);
	SET_INSTRUCTION_ADDR_DATA( LAX, IND_INDIR, 0xA3, 2, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( LAX, INDIR_IND, 0xB3, 2, 5, PAGE);

	/* NOP instruction. The addressing modes repeat several times.
	 * Please refer to http://www.oxyron.de/html/opcodes02.html
	 * for further details */
	SET_INSTRUCTION_ADDR_DATA( NOP, ZEROPAGE,  0x04, 2, 3, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( NOP, ABSOLUTE,  0x0C, 3, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( NOP, ZERO_INDX, 0x14, 2, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( NOP, IMPLIED,   0x1A, 1, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( NOP, ABS_INDX,  0x1C, 3, 4, PAGE);
	SET_INSTRUCTION_ADDR_DATA( NOP, ZERO_INDX, 0x34, 2, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( NOP, IMPLIED,   0x3A, 1, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( NOP, ABS_INDX,  0x3C, 3, 4, PAGE);
	SET_INSTRUCTION_ADDR_DATA( NOP, ZEROPAGE,  0x44, 2, 3, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( NOP, ZERO_INDX, 0x54, 2, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( NOP, IMPLIED,   0x5A, 1, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( NOP, ABS_INDX,  0x5C, 3, 4, PAGE);
	SET_INSTRUCTION_ADDR_DATA( NOP, ZEROPAGE,  0x64, 2, 3, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( NOP, ZERO_INDX, 0x74, 2, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( NOP, IMPLIED,   0x7A, 1, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( NOP, ABS_INDX,  0x7C, 3, 4, PAGE);
	SET_INSTRUCTION_ADDR_DATA( NOP, IMMEDIATE, 0x80, 2, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( NOP, IMMEDIATE, 0x82, 2, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( NOP, IMMEDIATE, 0x89, 2, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( NOP, IMMEDIATE, 0xC2, 2, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( NOP, ZERO_INDX, 0xD4, 2, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( NOP, IMPLIED,   0xDA, 1, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( NOP, ABS_INDX,  0xDC, 3, 4, PAGE);
	SET_INSTRUCTION_ADDR_DATA( NOP, IMMEDIATE, 0xE2, 2, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( NOP, ZERO_INDX, 0xF4, 2, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( NOP, IMPLIED,   0xFA, 1, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( NOP, ABS_INDX,  0xFC, 3, 4, PAGE);

	/* RLA instruction */
	SET_INSTRUCTION_ADDR_DATA( RLA, ZEROPAGE,  0x27, 2, 5, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( RLA, ZERO_INDX, 0x37, 2, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( RLA, ABSOLUTE,  0x2F, 3, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( RLA, ABS_INDX,  0x3F, 3, 7, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( RLA, ABS_INDY,  0x3B, 3, 7, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( RLA, IND_INDIR, 0x23, 2, 8, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( RLA, INDIR_IND, 0x33, 2, 8, NORMAL);
	
	/* SAX instruction */
	SET_INSTRUCTION_ADDR_DATA( SAX, ZEROPAGE,  0x87, 2, 3, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( SAX, ZERO_INDY, 0x97, 2, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( SAX, ABSOLUTE,  0x8F, 3, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( SAX, IND_INDIR, 0x83, 2, 6, NORMAL);

	/* SBX instruction */
	SET_INSTRUCTION_ADDR_DATA( SBX, IMMEDIATE, 0xCB, 2, 2, NORMAL);

	/* SHX instruction (it seems not to be present in the 6502) */
	SET_INSTRUCTION_ADDR_DATA( SHX, ABS_INDY,  0x9E, 3, 5, NORMAL);

	/* SHY instruction (it seems not to be present in the 6502) */
	SET_INSTRUCTION_ADDR_DATA( SHY, ABS_INDX,  0x9C, 3, 5, NORMAL);

	/* SBC instruction (this is the only illegal opcode) */
	SET_INSTRUCTION_ADDR_DATA( SBC, IMMEDIATE, 0xEB, 2, 2, NORMAL);

	/* SLO instruction */
	SET_INSTRUCTION_ADDR_DATA( SLO, ZEROPAGE,  0x07, 2, 5, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( SLO, ZERO_INDX, 0x17, 2, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( SLO, ABSOLUTE,  0x0F, 3, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( SLO, ABS_INDX,  0x1F, 3, 7, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( SLO, ABS_INDY,  0x1B, 3, 7, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( SLO, IND_INDIR, 0x03, 2, 8, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( SLO, INDIR_IND, 0x13, 2, 8, NORMAL);

	/* SRE instruction */
	SET_INSTRUCTION_ADDR_DATA( SRE, ZEROPAGE,  0x47, 2, 5, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( SRE, ZERO_INDX, 0x57, 2, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( SRE, ABSOLUTE,  0x4F, 3, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( SRE, ABS_INDX,  0x5F, 3, 7, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( SRE, ABS_INDY,  0x5B, 3, 7, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( SRE, IND_INDIR, 0x43, 2, 8, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( SRE, INDIR_IND, 0x53, 2, 8, NORMAL);

	/* RRA instruction */
	SET_INSTRUCTION_ADDR_DATA( RRA, ZEROPAGE,  0x67, 2, 5, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( RRA, ZERO_INDX, 0x77, 2, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( RRA, ABSOLUTE,  0x6F, 3, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( RRA, ABS_INDX,  0x7F, 3, 7, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( RRA, ABS_INDY,  0x7B, 3, 7, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( RRA, IND_INDIR, 0x63, 2, 8, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( RRA, INDIR_IND, 0x73, 2, 8, NORMAL);

	/* TAS instruction */
	SET_INSTRUCTION_ADDR_DATA( TAS, ABS_INDY,  0x9B, 3, 5, NORMAL);

	/* XAA instruction */
	SET_INSTRUCTION_ADDR_DATA( XAA, IMMEDIATE, 0x8B, 2, 2, NORMAL);

	return;
}

void get_operand_immediate(instruction inst, uint16_t inst_address, operand *oper) {
	oper->value = CPU->RAM[inst_address+1];
	DEBUG( printf(" #$%02x", oper->value) );
}

void get_operand_absolute(instruction inst, uint16_t inst_address, operand *oper) {
	oper->address = CPU->RAM[inst_address+1] | (CPU->RAM[inst_address + 2]  << 8);
	DEBUG( printf(" $%04x", oper->address) );
}

void get_operand_zeropage(instruction inst, uint16_t inst_address, operand *oper) {
	oper->address = CPU->RAM[inst_address+1];
	DEBUG( printf(" $%02x", oper->address) );
}

void get_operand_implied(instruction inst, uint16_t inst_address, operand *oper) {
}

void get_operand_indirect(instruction inst, uint16_t inst_address, operand *oper) {

	uint16_t address;

	address = CPU->RAM[inst_address+1] | (CPU->RAM[inst_address+2] << 8);
	DEBUG( printf(" ($%04x)", address) );
	oper->address =  read_cpu_ram(address);

	/* If address is $xxFF, the next read wraps page */
	if( address%0x100 == 0xFF )
		address -= 0x100;
	oper->address |= (read_cpu_ram(address+1) << 8);
}

void get_operand_abs_indx(instruction inst, uint16_t inst_address, operand *oper) {

	uint16_t address;

	address = ( CPU->RAM[inst_address+1] | (CPU->RAM[inst_address+2] << 8) );
	DEBUG( printf(" $%04x,X", address) );
	oper->address = address + CPU->X;
	if( ((address&0x100) != (oper->address&0x100)) &&
	    inst.cycle_change == CYCLE_PAGE )
		ADD_CPU_CYCLES(1);
}

void get_operand_abs_indy(instruction inst, uint16_t inst_address, operand *oper) {

	uint16_t address;

	address = ( CPU->RAM[inst_address+1] | (CPU->RAM[inst_address+2] << 8) );
	DEBUG( printf(" $%04x,Y", address) );
	oper->address = address + CPU->Y;
	if( ((address&0x100) != (oper->address&0x100)) &&
	    inst.cycle_change == CYCLE_PAGE )
		ADD_CPU_CYCLES(1);
}

void get_operand_ind_indir(instruction inst, uint16_t inst_address, operand *oper) {

	uint16_t address;

	address = CPU->RAM[inst_address+1] + CPU->X;
	DEBUG( printf(" ($%02x,X)", address - CPU->X) );
	if( address > 0xFF )
		address -= 0x100;
	oper->address = read_cpu_ram(address);
	if( address == 0xFF )
		address -= 0x100;
	oper->address |= read_cpu_ram(address+1) << 8;
}

void get_operand_indir_ind(instruction inst, uint16_t inst_address, operand *oper) {

	uint16_t address;

	address = CPU->RAM[inst_address+1];
	DEBUG( printf(" ($%02x),Y", address) );
	oper->address = read_cpu_ram(address);
	if( address == 0xFF )
		address -= 0x100;
	oper->address |= read_cpu_ram(address+1) << 8;
	if( ((oper->address&0x100) != ((oper->address+CPU->Y)&0x100)) &&
	    inst.cycle_change == CYCLE_PAGE )
		ADD_CPU_CYCLES(1);
	oper->address += CPU->Y;
}

void get_operand_relative(instruction inst, uint16_t inst_address, operand *oper) {
	oper->value = CPU->RAM[inst_address+1];
	DEBUG( printf(" $%02x", oper->value) );
}

void get_operand_accum(instruction inst, uint16_t inst_address, operand *oper) {
	DEBUG( printf(" A") );
}

void get_operand_zero_indx(instruction inst, uint16_t inst_address, operand *oper) {
	oper->address = CPU->RAM[inst_address+1] + CPU->X;
	DEBUG( printf(" $%02x,X", oper->address - CPU->X) );
	if( oper->address >= 0x0100 )
		oper->address -= 0x100;
}

void get_operand_zero_indy(instruction inst, uint16_t inst_address, operand *oper) {

	oper->address = CPU->RAM[inst_address+1] + CPU->Y;
	DEBUG( printf(" $%02x,Y", oper->address - CPU->Y) );
	if( oper->address >= 0x0100 )
		oper->address -= 0x100;
}

void get_operand_default(instruction inst, uint16_t inst_address, operand *oper) {
	fprintf(stderr,_("Hey!!! You haven't written the %d addressing mode!!!\n"), inst.addr_mode);
}

void (*get_operand_functions[])(instruction inst, uint16_t inst_address, operand *) = {
	&get_operand_immediate,
	&get_operand_absolute,
	&get_operand_zeropage,
	&get_operand_implied,
	&get_operand_indirect,
	&get_operand_abs_indx,
	&get_operand_abs_indy,
	&get_operand_zero_indx,
	&get_operand_zero_indy,
	&get_operand_ind_indir,
	&get_operand_indir_ind,
	&get_operand_accum,
	&get_operand_relative,
	&get_operand_default,
};

operand get_operand(instruction inst, uint16_t inst_address) {

	char lower_name[4];
	operand oper = { 0xDEAD, 0xBE};

	DEBUG( inst_lowercase(inst.name, lower_name) );
	DEBUG( printf("%s", lower_name) );
	get_operand_functions[inst.addr_mode](inst, inst_address, &oper);
	DEBUG( printf("\n") );

	return oper;
}
