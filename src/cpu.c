/*  ImaNES: I'm a NES. An intelligent NES emulator

    cpu.c   -    CPU emulation under ImaNES

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

#include "apu.h"
#include "clock.h"
#include "common.h"
#include "cpu.h"
#include "debug.h"
#include "i18n.h"
#include "instruction_set.h"
#include "mapper.h"
#include "pad.h"
#include "palette.h"
#include "ppu.h"
#include "screen.h"

static int prev_a12_state  = 0;
static int prev_a12_cycles = 0;

uint8_t  tmp;
uint16_t tmp16;

/**
 * Given a value and a set of flags, check and update them if necessary
 */
inline void update_flags(int8_t value, uint8_t flags) {

	/* 7th bit is set (negative number) */
	if( flags & N_FLAG ) {
		if( value >> 7 )   CPU->SR |= N_FLAG;
		else               CPU->SR &= ~N_FLAG;
	}

	if( flags & Z_FLAG ) {
		if( value == 0 )   CPU->SR |= Z_FLAG;
		else               CPU->SR &= ~Z_FLAG;
	}

}

void ADC_func(instruction *inst, operand *oper){
	if( inst->addr_mode != ADDR_IMMEDIATE )
		oper->value = read_cpu_ram(oper->address);
	tmp16 = CPU->A + oper->value + (CPU->SR & C_FLAG);

	/* If result is over 0xFF, then the carry is 1 */
	if( tmp16 > 0xFF )
		CPU->SR |= C_FLAG;
	else
		CPU->SR &= ~C_FLAG;

	/* Set overflow flag if needed */
	if( ( ((CPU->A^tmp16)      & 0x80) != 0 ) &&
	    ( ((CPU->A^oper->value) & 0x80) == 0 ) )
		CPU->SR |= V_FLAG;
	else
		CPU->SR &= ~V_FLAG;

	CPU->A = tmp16 & 0xFF; /* Truncate to 8 bits */
	update_flags(CPU->A, N_FLAG | Z_FLAG);
}

void AND_func(instruction *inst, operand *oper){
	if( inst->addr_mode != ADDR_IMMEDIATE )
		oper->value = read_cpu_ram(oper->address);
	CPU->A &= oper->value;
	update_flags(CPU->A, N_FLAG | Z_FLAG );
}

void ASL_func(instruction *inst, operand *oper){
	if( inst->addr_mode == ADDR_ACCUM ) {
		tmp = CPU->A & 0x80;
		CPU->A <<= 1;
		update_flags(CPU->A, N_FLAG | Z_FLAG);
	}
	else {
		oper->value = read_cpu_ram(oper->address);
		tmp = oper->value & 0x80;
		oper->value <<= 1;
		update_flags(oper->value, N_FLAG | Z_FLAG);
		write_cpu_ram(oper->address, oper->value);
	}
	if( tmp )
		CPU->SR |= C_FLAG;
	else
		CPU->SR &= ~C_FLAG;
}

void BCC_func(instruction *inst, operand *oper){
	if( ~CPU->SR & C_FLAG ) {
		add_cycles(CYCLE_BRANCH, oper->value);
		CPU->PC += (int8_t)oper->value;
	}
}

void BCS_func(instruction *inst, operand *oper){
	if( CPU->SR & C_FLAG ) {
		add_cycles(CYCLE_BRANCH, oper->value);
		CPU->PC +=(int8_t)oper->value;
	}
}

void BEQ_func(instruction *inst, operand *oper){
	if( CPU->SR & Z_FLAG ) {
		add_cycles(CYCLE_BRANCH, oper->value);
		CPU->PC += (int8_t)oper->value;
	}
}

void BIT_func(instruction *inst, operand *oper){
	oper->value = read_cpu_ram(oper->address);
	if( (oper->value >> 6)  & 0x01 )
		CPU->SR |= V_FLAG;
	else
		CPU->SR &= ~V_FLAG;
	update_flags(oper->value, N_FLAG);
	update_flags(oper->value & CPU->A, Z_FLAG);
}

void BMI_func(instruction *inst, operand *oper){
	if( CPU->SR & N_FLAG ) {
		add_cycles(CYCLE_BRANCH, oper->value);
		CPU->PC += (int8_t)oper->value;
	}
}

void BNE_func(instruction *inst, operand *oper){
	if( ~CPU->SR & Z_FLAG ) {
		add_cycles(CYCLE_BRANCH, oper->value);
		CPU->PC += (int8_t)oper->value;
	}
}

void BPL_func(instruction *inst, operand *oper){
	if( ~CPU->SR & N_FLAG ) {
		add_cycles(CYCLE_BRANCH, oper->value);
		CPU->PC += (int8_t)oper->value;
	}
}

void BRK_func(instruction *inst, operand *oper){
	CPU->SR |= B_FLAG;
	execute_irq();
	CPU->PC -= inst->size;
}

void BVC_func(instruction *inst, operand *oper){
	if( ~CPU->SR & V_FLAG ) {
		add_cycles(CYCLE_BRANCH, oper->value);
		CPU->PC += (int8_t)oper->value;
	}
}

void BVS_func(instruction *inst, operand *oper){
	if( CPU->SR & V_FLAG ) {
		add_cycles(CYCLE_BRANCH, oper->value);
		CPU->PC += (int8_t)oper->value;
	}
}

void CLC_func(instruction *inst, operand *oper){
	CPU->SR &= ~C_FLAG;
}

void CLD_func(instruction *inst, operand *oper){
	CPU->SR &= ~D_FLAG;
}

void CLI_func(instruction *inst, operand *oper){
	CPU->SR &= ~I_FLAG;
}

void CLV_func(instruction *inst, operand *oper){
	CPU->SR &= ~V_FLAG;
}

void CMP_func(instruction *inst, operand *oper){
	if( inst->addr_mode != ADDR_IMMEDIATE )
		oper->value = read_cpu_ram(oper->address);
	if( CPU->A >= oper->value)
		CPU->SR |= C_FLAG;
	else
		CPU->SR &= ~C_FLAG;
	update_flags(CPU->A - oper->value, N_FLAG | Z_FLAG);
}

void CPX_func(instruction *inst, operand *oper){
	if( inst->addr_mode != ADDR_IMMEDIATE )
		oper->value = read_cpu_ram(oper->address);
	if( CPU->X >= oper->value)
		CPU->SR |= C_FLAG;
	else
		CPU->SR &= ~C_FLAG;
	update_flags(CPU->X - oper->value, N_FLAG | Z_FLAG);
}

void CPY_func(instruction *inst, operand *oper){
	if( inst->addr_mode != ADDR_IMMEDIATE )
		oper->value = read_cpu_ram(oper->address);
	if( CPU->Y >= oper->value)
		CPU->SR |= C_FLAG;
	else
		CPU->SR &= ~C_FLAG;
	update_flags(CPU->Y - oper->value, N_FLAG | Z_FLAG);
}

void DEC_func(instruction *inst, operand *oper){
	tmp = read_cpu_ram(oper->address) - 1;
	write_cpu_ram(oper->address, tmp);
	update_flags( tmp , N_FLAG | Z_FLAG);
}

void DEX_func(instruction *inst, operand *oper){
	CPU->X--;
	update_flags(CPU->X, N_FLAG | Z_FLAG);
}

void DEY_func(instruction *inst, operand *oper){
	CPU->Y--;
	update_flags(CPU->Y, N_FLAG | Z_FLAG);
}

void EOR_func(instruction *inst, operand *oper){
	if( inst->addr_mode != ADDR_IMMEDIATE )
		oper->value = read_cpu_ram(oper->address);
	CPU->A ^= oper->value;
	update_flags(CPU->A, N_FLAG | Z_FLAG);
}

void INC_func(instruction *inst, operand *oper){
	tmp = read_cpu_ram(oper->address) + 1;
	write_cpu_ram( oper->address, tmp);
	update_flags(tmp, N_FLAG | Z_FLAG);
}

void INX_func(instruction *inst, operand *oper){
	CPU->X++;
	update_flags(CPU->X, N_FLAG | Z_FLAG);
}

void INY_func(instruction *inst, operand *oper){
	CPU->Y++;
	update_flags(CPU->Y, N_FLAG | Z_FLAG);
}

void JMP_func(instruction *inst, operand *oper){
	CPU->PC = oper->address - inst->size;
}

void JSR_func(instruction *inst, operand *oper){
	stack_push( (CPU->PC+2) >> 8 );
	stack_push( (CPU->PC+2) & 0xFF );
	CPU->PC = oper->address - inst->size;
}

void LDA_func(instruction *inst, operand *oper){
	if( inst->addr_mode != ADDR_IMMEDIATE )
		oper->value = read_cpu_ram(oper->address);
	CPU->A = oper->value;
	update_flags(CPU->A, N_FLAG | Z_FLAG);
}

void LDX_func(instruction *inst, operand *oper){
	if( inst->addr_mode != ADDR_IMMEDIATE )
		oper->value = read_cpu_ram(oper->address);
	CPU->X = oper->value;
	update_flags(CPU->X, N_FLAG | Z_FLAG);
}

void LDY_func(instruction *inst, operand *oper){
	if( inst->addr_mode != ADDR_IMMEDIATE )
		oper->value = read_cpu_ram(oper->address);
	CPU->Y = oper->value;
	update_flags(CPU->Y, N_FLAG | Z_FLAG);
}

void LSR_func(instruction *inst, operand *oper){
	if( inst->addr_mode == ADDR_ACCUM ) {
		tmp = CPU->A & 0x1;
		CPU->A >>= 1;
		update_flags(CPU->A, Z_FLAG | N_FLAG);
	}
	else {
		oper->value = read_cpu_ram(oper->address);
		tmp = oper->value & 0x1;
		oper->value >>= 1;
		write_cpu_ram( oper->address, oper->value );
		update_flags( oper->value , Z_FLAG | N_FLAG);
	}
	if( tmp )
		CPU->SR |= C_FLAG;
	else
		CPU->SR &= ~C_FLAG;
}

void NOP_func(instruction *inst, operand *oper){
}

void ORA_func(instruction *inst, operand *oper){
	if( inst->addr_mode != ADDR_IMMEDIATE )
		oper->value = read_cpu_ram(oper->address);
	CPU->A |= oper->value;
	update_flags(CPU->A, N_FLAG | Z_FLAG);
}

void PHA_func(instruction *inst, operand *oper){
	stack_push( CPU->A );
}

void PHP_func(instruction *inst, operand *oper){
	CPU->SR |= B_FLAG;
	stack_push( CPU->SR );
}

void PLA_func(instruction *inst, operand *oper){
	CPU->A = stack_pull();
	update_flags(CPU->A, N_FLAG | Z_FLAG);
}

void PLP_func(instruction *inst, operand *oper){
	CPU->SR = stack_pull();
	CPU->SR |= R_FLAG; /* R_FLAG should be _always_ set */
}

void ROL_func(instruction *inst, operand *oper){
	if( inst->addr_mode == ADDR_ACCUM ) {
		tmp = CPU->A & 0x80;
		CPU->A <<= 1;
		CPU->A |= (CPU->SR & C_FLAG);
		update_flags( CPU->A, N_FLAG | Z_FLAG);
	} else {
		oper->value = read_cpu_ram(oper->address);
		tmp = oper->value & 0x80;
		oper->value <<= 1;
		oper->value |= (CPU->SR & C_FLAG);
		write_cpu_ram(oper->address, oper->value);
		update_flags( oper->value , N_FLAG | Z_FLAG);
	}
	if( tmp )
		CPU->SR |= C_FLAG;
	else
		CPU->SR &= ~C_FLAG;
}

void ROR_func(instruction *inst, operand *oper){
	if( inst->addr_mode == ADDR_ACCUM ) {
		tmp = CPU->A & 0x1;
		CPU->A >>= 1;
		CPU->A |= (CPU->SR & C_FLAG) << 7;
		update_flags( CPU->A, N_FLAG | Z_FLAG);
	} else {
		oper->value = read_cpu_ram(oper->address);
		tmp = oper->value & 0x1;
		oper->value >>= 1;
		oper->value |= ((CPU->SR & C_FLAG) << 7);
		write_cpu_ram(oper->address, oper->value);
		update_flags( oper->value , N_FLAG | Z_FLAG);
	}
	if( tmp )
		CPU->SR |= C_FLAG;
	else
		CPU->SR &= ~C_FLAG;
}

void RTI_func(instruction *inst, operand *oper){
	CPU->SR =  stack_pull();
	CPU->SR |= R_FLAG; /* R_FLAG should be _always_ set */
	CPU->PC =  stack_pull();
	CPU->PC |= stack_pull() << 8;
	CPU->PC -= inst->size;
}

void RTS_func(instruction *inst, operand *oper){
	CPU->PC =  stack_pull();
	CPU->PC |= stack_pull() << 8;
	CPU->PC++;
	CPU->PC -= inst->size;
}

void SBC_func(instruction *inst, operand *oper){
	if( inst->addr_mode != ADDR_IMMEDIATE )
		oper->value = read_cpu_ram(oper->address);
	tmp16 = CPU->A - oper->value - (1 - (CPU->SR & C_FLAG));

	/* If result is over 0xFF, then the carry is 1 */
	if( tmp16 > 0xFF )
		CPU->SR &= ~C_FLAG;
	else
		CPU->SR |= C_FLAG;

	/* Set overflow flag if needed */
	if( ( ((CPU->A^tmp16)      & 0x80) != 0 ) &&
			( ((CPU->A^oper->value) & 0x80) != 0 ) )
		CPU->SR |= V_FLAG;
	else
		CPU->SR &= ~V_FLAG;

	CPU->A = tmp16 & 0xFF; /* truncate to 8 bits */

	update_flags(CPU->A, N_FLAG | Z_FLAG);
}

void SEC_func(instruction *inst, operand *oper){
	CPU->SR |= C_FLAG;
}

void SED_func(instruction *inst, operand *oper){
	CPU->SR |= D_FLAG;
}

void SEI_func(instruction *inst, operand *oper){
	CPU->SR |= I_FLAG;
}

void STA_func(instruction *inst, operand *oper){
	write_cpu_ram(oper->address, CPU->A);
}

void STX_func(instruction *inst, operand *oper){
	write_cpu_ram(oper->address, CPU->X);
}

void STY_func(instruction *inst, operand *oper){
	write_cpu_ram(oper->address, CPU->Y);
}

void TAX_func(instruction *inst, operand *oper){
	CPU->X = CPU->A;
	update_flags(CPU->X, N_FLAG | Z_FLAG);
}

void TAY_func(instruction *inst, operand *oper){
	CPU->Y = CPU->A;
	update_flags(CPU->Y, N_FLAG | Z_FLAG);
}

void TSX_func(instruction *inst, operand *oper){
	CPU->X = CPU->SP;
	update_flags(CPU->X, N_FLAG | Z_FLAG);
}

void TXA_func(instruction *inst, operand *oper){
	CPU->A = CPU->X;
	update_flags(CPU->A, N_FLAG | Z_FLAG);
}

void TXS_func(instruction *inst, operand *oper){
	CPU->SP = CPU->X;
}

void TYA_func(instruction *inst, operand *oper){
	CPU->A = CPU->Y;
	update_flags(CPU->A, N_FLAG | Z_FLAG);
}

/** Illegal opcodes **/
void ANC_func(instruction *inst, operand *oper){
	CPU->A &= oper->value;
	if( (int8_t)CPU->A < 0 )
		CPU->SR |= C_FLAG;
	else
		CPU->SR &= ~C_FLAG;
	update_flags(CPU->A, N_FLAG | Z_FLAG );
}

void ALR_func(instruction *inst, operand *oper){
	CPU->A &= oper->value;
	if( CPU->A & 0x01 )
		CPU->SR |= C_FLAG;
	else
		CPU->SR &= ~C_FLAG;
	CPU->A >>= 1;
	update_flags(CPU->A, N_FLAG | Z_FLAG );
}

void ARR_func(instruction *inst, operand *oper){
	CPU->A &= oper->value;
	CPU->A >>= 1;
	CPU->A |= (CPU->SR & C_FLAG) << 7;
	if( CPU->A & 0x40 )
		CPU->SR |= C_FLAG;
	else
		CPU->SR &= ~C_FLAG;
	if( ((CPU->A&0x40)>>1) != (CPU->A&0x20) )
		CPU->SR |= V_FLAG;
	else
		CPU->SR &= ~V_FLAG;
	update_flags(CPU->A, N_FLAG | Z_FLAG );
}

void DCP_func(instruction *inst, operand *oper){
	tmp = read_cpu_ram(oper->address) - 1;
	write_cpu_ram(oper->address, tmp);
	if( CPU->A >= tmp)
		CPU->SR |= C_FLAG;
	else
		CPU->SR &= ~C_FLAG;
	update_flags(CPU->A - tmp, N_FLAG | Z_FLAG);
}

void ISC_func(instruction *inst, operand *oper){
	tmp = read_cpu_ram(oper->address) + 1;
	write_cpu_ram(oper->address, tmp);

	tmp16 = CPU->A - tmp - (1 - (CPU->SR & C_FLAG));

	/* If result is over 0xFF, then the carry is 1 */
	if( tmp16 > 0xFF )
		CPU->SR &= ~C_FLAG;
	else
		CPU->SR |= C_FLAG;

	/* Set overflow flag if needed */
	if( ( ((CPU->A^tmp16) & 0x80) != 0 ) &&
			( ((CPU->A^tmp) & 0x80) != 0 ) )
		CPU->SR |= V_FLAG;
	else
		CPU->SR &= ~V_FLAG;

	CPU->A = tmp16 & 0xFF; /* truncate to 8 bits */
	update_flags(CPU->A, N_FLAG | Z_FLAG);
}

void LAX_func(instruction *inst, operand *oper){
	if( inst->addr_mode != ADDR_IMMEDIATE )
		oper->value = read_cpu_ram(oper->address);
	CPU->A = oper->value;
	CPU->X = oper->value;
	update_flags(CPU->A, N_FLAG | Z_FLAG);
}

void RLA_func(instruction *inst, operand *oper){
	oper->value = read_cpu_ram(oper->address);
	tmp = oper->value & 0x80;
	oper->value <<= 1;
	oper->value |= (CPU->SR & C_FLAG);
	write_cpu_ram(oper->address, oper->value);
	if( tmp )
		CPU->SR |= C_FLAG;
	else
		CPU->SR &= ~C_FLAG;
	CPU->A &= oper->value;
	update_flags(CPU->A, N_FLAG | Z_FLAG);
}

void RRA_func(instruction *inst, operand *oper){
	/* Right shift */
	oper->value = read_cpu_ram(oper->address);
	tmp = oper->value & 0x1;
	oper->value >>= 1;
	oper->value |= ((CPU->SR & C_FLAG) << 7);
	write_cpu_ram(oper->address, oper->value);
	if( tmp )
		CPU->SR |= C_FLAG;
	else
		CPU->SR &= ~C_FLAG;

	/* ADC */
	tmp16 = CPU->A + oper->value + (CPU->SR & C_FLAG);

	/* If result is over 0xFF, then the carry is 1 */
	if( tmp16 > 0xFF )
		CPU->SR |= C_FLAG;
	else
		CPU->SR &= ~C_FLAG;

	/* Set overflow flag if needed */
	if( ( ((CPU->A^tmp16)      & 0x80) != 0 ) &&
			( ((CPU->A^oper->value) & 0x80) == 0 ) )
		CPU->SR |= V_FLAG;
	else
		CPU->SR &= ~V_FLAG;

	CPU->A = tmp16 & 0xFF; /* Truncate to 8 bits */
	update_flags(CPU->A, N_FLAG | Z_FLAG);
}

void SAX_func(instruction *inst, operand *oper){
	tmp = CPU->A & CPU->X;
	write_cpu_ram(oper->address, tmp);
	update_flags(tmp, N_FLAG | Z_FLAG);
}

void SBX_func(instruction *inst, operand *oper){
	CPU->X = CPU->A & CPU->X;

	if( CPU->X >= oper->value)
		CPU->SR |= C_FLAG;
	else
		CPU->SR &= ~C_FLAG;
	CPU->X -= oper->value;
	update_flags(CPU->X, N_FLAG | Z_FLAG);
}

void SHX_func(instruction *inst, operand *oper){
	tmp = CPU->X & (((oper->address & 0xFF00) >> 8) + 1);
	write_cpu_ram(oper->address, tmp);
}

void SHY_func(instruction *inst, operand *oper){
	tmp = CPU->Y & (((oper->address & 0xFF00) >> 8) + 1);
	write_cpu_ram(oper->address, tmp);
}

void SLO_func(instruction *inst, operand *oper){
	oper->value = read_cpu_ram(oper->address);
	if( oper->value & 0x80 )
		CPU->SR |= C_FLAG;
	else
		CPU->SR &= ~C_FLAG;
	oper->value <<= 1;
	write_cpu_ram(oper->address, oper->value);
	CPU->A |= oper->value;
	update_flags(CPU->A, N_FLAG | Z_FLAG);
}

void SRE_func(instruction *inst, operand *oper){
	oper->value = read_cpu_ram(oper->address);
	if( oper->value & 0x01 )
		CPU->SR |= C_FLAG;
	else
		CPU->SR &= ~C_FLAG;
	oper->value >>= 1;
	write_cpu_ram(oper->address, oper->value);
	CPU->A ^= oper->value;
	update_flags(CPU->A, N_FLAG | Z_FLAG);
}

void default_func(instruction *inst, operand *oper){
	fprintf(stderr,_("%s: Still unimplemented\n"), inst->name);
}

void (*ptr_to_inst[INSTRUCTIONS_NUMBER])(instruction *, operand *) = {
	&ADC_func,
	&AND_func,
	&ASL_func,
	&BCC_func,
	&BCS_func,
	&BEQ_func,
	&BIT_func,
	&BMI_func,
	&BNE_func,
	&BPL_func,
	&BRK_func,
	&BVC_func,
	&BVS_func,
	&CLC_func,
	&CLD_func,
	&CLI_func,
	&CLV_func,
	&CMP_func,
	&CPX_func,
	&CPY_func,
	&DEC_func,
	&DEX_func,
	&DEY_func,
	&EOR_func,
	&INC_func,
	&INX_func,
	&INY_func,
	&JMP_func,
	&JSR_func,
	&LDA_func,
	&LDX_func,
	&LDY_func,
	&LSR_func,
	&NOP_func,
	&ORA_func,
	&PHA_func,
	&PHP_func,
	&PLA_func,
	&PLP_func,
	&ROL_func,
	&ROR_func,
	&RTI_func,
	&RTS_func,
	&SBC_func,
	&SEC_func,
	&SED_func,
	&SEI_func,
	&STA_func,
	&STX_func,
	&STY_func,
	&TAX_func,
	&TAY_func,
	&TSX_func,
	&TXA_func,
	&TXS_func,
	&TYA_func,
	&default_func,
	&ANC_func,
	&ALR_func,
	&ARR_func,
	&DCP_func,
	&ISC_func,
	&default_func,
	&LAX_func,
	&RLA_func,
	&RRA_func,
	&SAX_func,
	&SBX_func,
	&SHX_func,
	&SHY_func,
	&SLO_func,
	&SRE_func,
	&default_func,
	&default_func
};

/* Reads the first PPU Control Register (0x2000) */
uint8_t _read_ppu_cr1(uint16_t address) {
	return PPU->CR1;
}

/* Reads the second PPU Control Register (0x2001) */
uint8_t _read_ppu_cr2(uint16_t address) {
	return PPU->CR2;
}

/* Reads the PPU Status Register (0x2002) */
uint8_t _read_ppu_st(uint16_t address) {
	uint8_t ret_val = PPU->SR;
	if( !(PPU->scanline_timeout <= 1 && PPU->lines == NES_SCREEN_HEIGHT ) )
		PPU->SR &= ~VBLANK_FLAG;
	PPU->latch = 1;
	return ret_val;
}

/* Reads the SPR-RAM address (0x2003) */
uint8_t _read_spr_ram_add(uint16_t address) {
	return PPU->spr_addr;
}

/* Accesses the SPR-RAM (0x2004) */
uint8_t _read_spr_ram(uint16_t address) {
	return PPU->SPR_RAM[PPU->spr_addr];
}

/* Read latch (TODO: which latch?) */
uint8_t _read_latch(uint16_t address) {
	return PPU->latch;
}

/* Read the PPU VRAM */
uint8_t _read_ppu_vram(uint16_t address) {

	uint8_t ret_val = 0;
	static uint8_t buffer = 0; /* Buffer when reading from 0x2007 */

	if( PPU->vram_addr < 0x3F00 ) {
		ret_val = buffer;
		buffer = read_ppu_vram(PPU->vram_addr);
	}
	/* Palette reads don't use the read buffer */
	else {
		ret_val = read_ppu_vram(PPU->vram_addr);
		buffer  = read_ppu_vram(PPU->vram_addr - 0x1000);
	}
	if( PPU->CR1 & VERTICAL_WRITE)
		PPU->vram_addr += 32;
	else
		PPU->vram_addr++;

	/* Check rising edge of A12 on PPU bus (needed by MMC3) */
	if( (PPU->vram_addr & 0x1000) &&
		!prev_a12_state )
		mapper->update();

	/* Save the A12 line status (needed by MMC3) */
	prev_a12_state  = PPU->vram_addr & 0x1000;
	prev_a12_cycles = CLK->ppu_cycles;

	return ret_val;
}

uint8_t _read_apu_sr(uint16_t address) {

	uint8_t ret_val = 0;

	ret_val |= ((APU->square1.lc.counter > 0 ? 1 : 0));
	ret_val |= ((APU->square2.lc.counter > 0 ? 1 : 0) << 1);
	ret_val |= ((APU->triangle.lc.counter > 0 ? 1 : 0) << 2);
	ret_val |= ((APU->noise.lc.counter > 0 ? 1 : 0) << 3);
	/* TODO: DMC sample bytes remaining > 0 << 4 */
	ret_val |= ((APU->frame_seq.int_flag & 0x1) << 6);
	/* TODO: IRQ from DMC << 7 */

	/* Finally, clear the FS interrupt flag */
	APU->frame_seq.int_flag = 0;

	return ret_val;
}

/* Read the 1st joystick (0x4016) */
uint8_t _read_joystick1(uint16_t address) {

	uint8_t ret_val = 0;

	/* If we should return a key state... */
	if( pads[0].reads < 8 )
		ret_val |= ((pads[0].pressed_keys >> (pads[0].reads)) & 0x1);

	/* This is the signature */
	else if ( pads[0].reads == 19 && pads[0].plugged )
		ret_val |= 0x01;

	pads[0].reads++;
	if( pads[0].reads == 32 )
		pads[0].reads = 0;

	return ret_val;
}

/* Read the 2nd Joystick (0x4017) */
uint8_t _read_joystick2(uint16_t address) {

	uint8_t ret_val = 0;

	/* If we should return a key state... */
	if( pads[1].reads < 8 )
		ret_val |= ((pads[1].pressed_keys >> (pads[1].reads)) & 0x1);

	/* This is the signature */
	else if ( pads[1].reads == 18 && pads[1].plugged )
		ret_val |= 0x01;

	pads[1].reads++;
	if( pads[1].reads == 32 )
		pads[1].reads = 0;

	return ret_val;
}

/* Read the SRAM (0x4017) */
uint8_t _read_sram(uint16_t address) {
	if( !(CPU->sram_enabled & SRAM_ENABLE) )
		return 0;
	else
		return CPU->RAM[address];

}

/* Readm SRAM */
uint8_t _read_ram(uint16_t address) {
	return CPU->RAM[address];
}

uint8_t (*read_cpu_ram_f[NES_RAM_SIZE])(uint16_t address);


nes_cpu *CPU;

void initialize_cpu() {

	unsigned int i = 0;

	CPU = (nes_cpu *)malloc(sizeof(nes_cpu));
	CPU->A = 0;
	CPU->X = 0;
	CPU->Y = 0;
	CPU->SR = R_FLAG; /* It is never ever used, but always set */
	CPU->SP  = 0xff; /* It decrements when pushing, increments when pulling */

	CPU->RAM = (uint8_t *)malloc(NES_RAM_SIZE);
	memset(CPU->RAM, 0, NES_RAM_SIZE);
	CPU->reset = 1;
	CPU->sram_enabled = 0;
	CPU->sram_enabled &= ~SRAM_ENABLE;

	/* The default is to call _read_ram when reading the RAM */
	for(i=0; i!= NES_RAM_SIZE; i++)
		read_cpu_ram_f[i] = &_read_ram;

	for(i=0x6000; i!=0x8000; i++)
		read_cpu_ram_f[i] = &_read_sram;

	read_cpu_ram_f[0x2000] = &_read_ppu_cr1;
	read_cpu_ram_f[0x2001] = &_read_ppu_cr2;
	read_cpu_ram_f[0x2002] = &_read_ppu_st;
	read_cpu_ram_f[0x2003] = &_read_spr_ram_add;
	read_cpu_ram_f[0x2004] = &_read_spr_ram;
	read_cpu_ram_f[0x2005] = &_read_latch;
	read_cpu_ram_f[0x2006] = &_read_latch;
	read_cpu_ram_f[0x2007] = &_read_ppu_vram;
	read_cpu_ram_f[0x4015] = &_read_apu_sr;
	read_cpu_ram_f[0x4016] = &_read_joystick1;
	read_cpu_ram_f[0x4017] = &_read_joystick2;

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

void dump_ram(uint16_t address, unsigned int lenght) {

	unsigned int i;
	unsigned short j;
	uint8_t tmp[16];

	for(i=0; i!=lenght; i++) {
		printf("%04X: ", address + 0x10*i);
		for(j=0; j!=16; j++) {
			tmp[j] = read_cpu_ram(address + 0x10*i + j);
			printf("%02X ", tmp[j]);
		}
		printf("| ");
		for(j=0; j!=16; j++) {
			/* Not-nice ASCII characters get a dot */
			if( tmp[j] < 32 || tmp[j] > 126 )
				tmp[j] = '.';
			printf("%c", tmp[j]);
		}
		printf("\n");
	}

}

void init_cpu_ram(ines_file *file) {
	mapper->reset();
}

void execute_instruction(instruction *inst, operand *oper) {
	(*ptr_to_inst[inst->instr_id])(inst, oper);
}

void write_cpu_ram(uint16_t address, uint8_t value) {

	static int strobe_pad = 0;
	int i;

	XTREME( if( 0x2000 <= address && address <= 0x2006 ) {
		printf(_("PPU: Write to PPU[%d]=$%02X PC=%04X\n"), address - 0x2000, value, CPU->PC);
	} );

	/* Convert the address to handle mirroring */
	if( 0x0800 <= address && address < 0x2000 ) {
		DEBUG( printf(_("CPU Address mirroring: from %04x to "), address) );
		address &= 0x7FF;
		DEBUG( printf("%04x\n",address) );
	}

	if( 0x2008 <= address && address < 0x4000 ) {
		DEBUG( printf(_("CPU Address mirroring: from %04x to "), address) );
		address = (address&0x7) + 0x2000;
		DEBUG( printf("%04x\n",address) );
	}

	/* SRAM can be disabled or in RO mode */
	if( 0x6000 <= address && address < 0x8000 &&
	 ( !(CPU->sram_enabled&SRAM_ENABLE) || CPU->sram_enabled&SRAM_RO ) ) {
		DEBUG( printf(_("Write to %04x not allowed\n"), address) );
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
			PPU->SPR_RAM[PPU->spr_addr++] = value;
			break;

		case 0x2005:
			/* First write */
			if( PPU->latch ) {
				PPU->x = value & 0x7;
				PPU->temp_addr = (PPU->temp_addr&0xFFE0) | ((value&0xF8)>>3);

				/* This is, anyways, still correct, since I draw
				 * the entire line at once. This means
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
				PPU->temp_addr  = (PPU->temp_addr&0xFF00) | value;
				PPU->vram_addr  = PPU->temp_addr;

				/* Check rising edge of A12 on PPU bus (needed by MMC3) */
				if( (PPU->vram_addr & 0x1000) &&
				    !prev_a12_state )
					mapper->update();

				/* Save the A12 line status (needed by MMC3) */
				prev_a12_state  = PPU->vram_addr & 0x1000;
				prev_a12_cycles = CLK->ppu_cycles;
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

				/* Check rising edge of A12 on PPU bus (needed by MMC3) */
				if( (PPU->vram_addr & 0x1000) &&
				    !prev_a12_state )
					mapper->update();

				/* Save the A12 line status (needed by MMC3) */
				prev_a12_state  = PPU->vram_addr & 0x1000;
				prev_a12_cycles = CLK->ppu_cycles;
			}
			break;

		/* 1st Square channel duty, envelope */
		case 0x4000:
			APU->square1.duty_cycle = (value&0xC0) >> 6;

			i = (value&0x20) >> 5;
			APU->square1.envelope.loop = i;
			APU->square1.lc.halt = i;

			APU->square1.envelope.disabled = (value&0x10) >> 4;
			APU->square1.envelope.timer.period = (value&0x0F) + 1;
			break;

		/* 1st Square channel sweep unit */
		case 0x4001:
			APU->square1.sweep.enable = (value&0x80) >> 7;
			APU->square1.sweep.timer.period = ((value&0x70) >> 4) + 1;
			APU->square1.sweep.negate = (value&0x08) >> 3;
			APU->square1.sweep.shift = value&0x07;
			APU->square1.sweep.written = 1;
			break;

		/* 1st Square channel period 8 lower bits */
		case 0x4002:
			APU->square1.timer.period &= 0x0700;
			APU->square1.timer.period |= value;
			APU->square1.timer.period++;
			break;

		/* 1st Square channel period 3 higher bits, length counter index */
		case 0x4003:
			APU->square1.timer.period &= 0x00FF;
			APU->square1.timer.period |= (value & 0x7) << 8;

			i = (value & 0xF8) >> 3;
			if( APU->square1.lc.enabled )
				APU->square1.lc.counter = length_counter_reload_values[i];

			APU->square1.sequencer_step = 0;
			APU->square1.envelope.written = 1;
			break;

		/* 2nd Square channel duty, envelope */
		case 0x4004:
			APU->square2.duty_cycle = (value&0xC0) >> 6;

			i = (value&0x20) >> 5;
			APU->square2.envelope.loop = i;
			APU->square2.lc.halt = i;

			APU->square2.envelope.disabled = (value&0x10) >> 4;
			APU->square2.envelope.timer.period = (value&0x0F) + 1;
			break;

		/* 2nd Square channel sweep unit */
		case 0x4005:
			APU->square2.sweep.enable = (value&0x80) >> 7;
			APU->square2.sweep.timer.period = ((value&0x70) >> 4) + 1;
			APU->square2.sweep.negate = (value&0x08) >> 3;
			APU->square2.sweep.shift = value&0x07;
			APU->square2.sweep.written = 1;
			break;

		/* 2nd Square channel period 8 lower bits */
		case 0x4006:
			APU->square2.timer.period &= 0x0700;
			APU->square2.timer.period |= value;
			APU->square2.timer.period++;
			break;

		/* 2nd Square channel period 3 higher bits, length counter index */
		case 0x4007:
			APU->square2.timer.period &= 0x00FF;
			APU->square2.timer.period |= (value & 0x7) << 8;

			i = (value & 0xF8) >> 3;
			if( APU->square2.lc.enabled )
				APU->square2.lc.counter = length_counter_reload_values[i];

			APU->square2.sequencer_step = 0;
			APU->square2.envelope.written = 1;
			break;

		/* Triangle channel linear counter, control */
		case 0x4008:
			i  = (value&0x80) >> 7;
			APU->triangle.linear.control = i;
			APU->triangle.lc.halt = i;

			APU->triangle.linear.reload = value&0x7F;

			break;

		/* Triangle channel period 8 lower bits */
		case 0x400A:
			APU->triangle.timer.period &= 0x0700;
			APU->triangle.timer.period |= value;
			APU->triangle.timer.period++;
			break;

		/* Triangle channel period 3 higher bits, length counter index */
		case 0x400B:
			APU->triangle.timer.period &= 0x00FF;
			APU->triangle.timer.period |= (value & 0x7) << 8;

			i = (value & 0xF8) >> 3;
			if( APU->triangle.lc.enabled )
				APU->triangle.lc.counter = length_counter_reload_values[i];
			APU->triangle.linear.halt = 1;
			break;

		/* Noise channel envelope */
		case 0x400C:
			i = (value&0x20) >> 5;
			APU->noise.envelope.loop = i;
			APU->noise.lc.halt = i;

			APU->noise.envelope.disabled = (value&0x10) >> 4;
			APU->noise.envelope.timer.period = (value&0x0F) + 1;
			break;

		/* Noise channel random mode, timer period index */
		case 0x400E:
			APU->noise.random_mode = (value&0x80) >> 7;
			APU->noise.timer.period = noise_timer_periods[ value&0x0F ];
			break;

		/* Noise channel length counter */
		case 0x400F:
			i = (value & 0xF8) >> 3;
			if( APU->noise.lc.enabled )
				APU->noise.lc.counter = length_counter_reload_values[i];
			break;

		/* Sprite DMA */
		case 0x4014:
			address = value*0x100;
			for(i=0;i!=256;i++)
				PPU->SPR_RAM[PPU->spr_addr++] = read_cpu_ram(address+i);
			ADD_CPU_CYCLES(512);
			break;

		/* APU Lenght Control */
		case 0x4015:
			APU->square1.lc.enabled = value&0x01;
			if( !APU->square1.lc.enabled )
				APU->square1.lc.counter = 0;

			APU->square2.lc.enabled = (value&0x02) >> 1;
			if( !APU->square2.lc.enabled )
				APU->square2.lc.counter = 0;

			APU->triangle.lc.enabled = (value&0x04) >> 2;
			if( !APU->triangle.lc.enabled )
				APU->triangle.lc.counter = 0;

			APU->noise.lc.enabled = (value&0x08) >> 3;
			if( !APU->noise.lc.enabled )
				APU->noise.lc.counter = 0;

			/* TODO: DMC IRQ flag clear, DMC start/stop */

			break;

		/* 1st and 2nd joysticks strobe */
		case 0x4016:
			if( value == 0x01 )
				strobe_pad = 1;
			else if( value == 0x00 && strobe_pad ) {
				strobe_pad = 0;
				pads[0].reads = 0;
				pads[1].reads = 0;
			}
			break;

		/* APU common */
		case 0x4017:
			APU->commons = value & 0xC0;

			/* Reset the frame sequencer and divider */
			APU->frame_seq.step = 0;
			if( APU->commons & STEP_MODE5 )
				APU->frame_seq.clock_timeout = PPUCYCLES_STEP5;
			else
				APU->frame_seq.clock_timeout = PPUCYCLES_STEP4;

			if( APU->commons & STEP_MODE5 )
				clock_frame_sequencer();
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
		DEBUG( printf(_("CPU Address mirroring: from %04x to "), address) );
		address &= 0x7FF;
		DEBUG( printf("%04x\n",address) );
	}

	if( 0x2008 <= address && address < 0x4000 ) {
		DEBUG( printf(_("CPU Address mirroring: from %04x to "), address) );
		address = (address&0x7) + 0x2000;
		DEBUG( printf("%04x\n",address) );
	}

	/* Read the value using the corresponding function pointer       */
	/* Otherwise (without function pointers) we couldn't inline this */
	/* method, as it would be too big (inlining to be done)          */
	ret_val = (*read_cpu_ram_f[address])(address);

	XTREME( printf(_("Returning %02x from %04x\n"), ret_val, address) );
	return ret_val;
}


void stack_push(uint8_t value) {

	/* The stack is top down. When someone pushes, the SP decreases */
	/* We need to use write_cpu_ram because of the mirroring */
	write_cpu_ram(BEGIN_STACK + CPU->SP--, value);

	return;
}

uint8_t stack_pull() {

	/* The stack is top down. When someone pulls, the SP increases */
	return CPU->RAM[BEGIN_STACK + ++CPU->SP];
}

/* Note: NMI is executed after inscreasing the PC! */
void execute_nmi() {

	/* This interrupt is not maskable, so we don't check
	 * the interrupt flag on the processor status register */
	DEBUG( printf(_("Executing NMI!\n")) );

	/* NMI clears the B_FLAG from CPU status */
	CPU->SR &= ~B_FLAG;

	/* Push the PC and the SR */
	/* Finally, jump to the interrupt vector */
	stack_push( (CPU->PC >> 8) & 0xFF );
	stack_push( CPU->PC & 0xFF );
	stack_push( CPU->SR );
	CPU->PC = (*(CPU->RAM + 0xFFFA) | (*(CPU->RAM + 0xFFFB)<<8) );

	ADD_CPU_CYCLES(7);
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

	/* If the interrupt flag is cleared, we reject the interrupt
	 * (BRK should still work anyways) */
	if( CPU->SR & I_FLAG  && !(CPU->SR & B_FLAG) )
		return;

	stack_push( ((CPU->PC+2) >> 8) & 0xFF );
	stack_push( (CPU->PC+2) & 0xFF );
	stack_push( CPU->SR );
	CPU->PC = ( CPU->RAM[0xFFFE] | ( CPU->RAM[0xFFFF]<<8 ) );

}

void add_cycles(uint8_t type, int8_t value) {

	if( type == CYCLE_BRANCH ) {
		if( ((CPU->PC+2)&0x100) == ((CPU->PC + value + 2)&0x100) )
			ADD_CPU_CYCLES(1);
		else
			ADD_CPU_CYCLES(2);
	}

	else if( type == CYCLE_PAGE ) {
		if( ((CPU->PC+2)&0x100) != ((CPU->PC + value + 2)&0x100) )
			ADD_CPU_CYCLES(1);
	}

}

void end_cpu() {

	if( CPU->RAM != NULL )
		free(CPU->RAM);
	if( CPU != NULL )
		free(CPU);
}
