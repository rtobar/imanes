/*  ImaNES: I'm a NES. An intelligent NES emulator

    mmc3.c   -    MMC3 Mapper emulation under ImaNES

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

#include "cpu.h"
#include "debug.h"
#include "mapper.h"
#include "mmc3.h"
#include "ppu.h"

typedef enum _mmc3_action {
	SetCommand,
	SwapBanks,
	ChangeMirroring,
	ToogleSRAM,
	SetIRQ,
	EnableIRQ,
	DisableIRQ,
	ResetIRQ
} mmc3_action;

/* This is the action taken when writing into the address,
   used later to see what we should do when "switching banks" */
static mmc3_action action;

/* Are we powering on the machine? */
static int powering_on;

static int swapping_control;

static int irq_enabled;
static uint8_t irq_tmp;
static uint8_t irq_counter;

void mmc3_initialize_mapper() {

	mapper->regs = (uint8_t *)malloc(8);
	memset(mapper->regs,0,8);

	mapper->regs[0] = 0; /* 0x8000 and 0xA000 are switchable */
	powering_on = 1;
	swapping_control = 0;

	irq_counter = 0;

	return;
}

int  mmc3_check_address(uint16_t address) {

	if( address < 0x8000 )
		return 0;

	//printf("MMC3: Writing %02x into %04x\n", CPU->RAM[address], address);

	/* This only set values, does not take any action */
	if( address == 0x8000 ) {
		mapper->regs[0] = CPU->RAM[address];
		action = SetCommand;
		return 1;
	}

	if( address == 0x8001 ) {
		mapper->regs[1] = CPU->RAM[address];
		action = SwapBanks;
		return 1;
	}

	if( address == 0xA000 ) {
		mapper->regs[2] = CPU->RAM[address];
		action = ChangeMirroring;
		return 1;
	}

	if( address == 0xA001 ) {
		mapper->regs[3] = CPU->RAM[address];
		action = ToogleSRAM;
		return 1;
	}

	if( address == 0xC000 ) {
		mapper->regs[4] = CPU->RAM[address];
		action = SetIRQ;
		return 1;
	}

	if( address == 0xC001 ) {
		mapper->regs[5] = CPU->RAM[address];
		action = ResetIRQ;
		return 1;
	}

	if( address == 0xE000 ) {
		mapper->regs[6] = CPU->RAM[address];
		action = DisableIRQ;
		return 1;
	}

	if( address == 0xE001 ) {
		mapper->regs[7] = CPU->RAM[address];
		action = EnableIRQ;
		return 1;
	}

	return 0;
}

void mmc3_switch_banks() {

	uint8_t bank;
	uint8_t command;
	uint16_t offset;

	switch( action ) {

		case SetCommand:
			/* If we haven't changed the swapping control, then
			   we don't need to copy again the same {-2} ROM memory */
			if( swapping_control == (mapper->regs[0]&0x40) )
				break;

			if( mapper->regs[0] & 0x40 )
				offset = 0x8000;
			else
				offset = 0xC000;

			printf("MMC3: Reg0:%02x, Swapping {-2} bank to %04x\n", mapper->regs[0], offset);
			memcpy( CPU->RAM + offset,
			   mapper->file->rom + (mapper->file->romBanks-1)*ROM_BANK_SIZE,
			   ROM_BANK_SIZE/2);
			swapping_control = mapper->regs[0]&0x40;
			break;

		case SwapBanks:
			bank = mapper->regs[1];
			command = (mapper->regs[0]&0x7);
			printf("MMC3: bank:%02x   command:%02x   reg0:%02x\n", bank, command, mapper->regs[0]);

			/* Switch VROM page */
			if( command <= 5 ) {


				/* Copy 2 1Kb VROM pages */
				if( command <= 1 ) {
					offset = 0x800*command + ((mapper->regs[0]&0x80) << 5);
					bank &= 0xFE;
					printf("MMC3: Switching VROM bank %d to %04x\n", bank,offset);
					memcpy(PPU->VRAM+offset,
					       mapper->file->vrom+bank*1024, 2*1024);
				}
				/* Copy 1 Kb VROM page */
				else {
					offset = (command-2)*0x400 + (!(mapper->regs[0]&0x80))*0x1000;
					printf("MMC3: Switching VROM bank %d to %04x\n", bank,offset);
					memcpy(PPU->VRAM+offset,
					       mapper->file->vrom+bank*1024, 1024);
				}
			}
			else {

				offset = 0x8000;

				/* 0xA000 is present in both regions */
				if( command == 7 )
					offset += 0x2000;
				else if( mapper->regs[0] & 0x40 )
					offset += 0x4000;

				printf("MMC3: Switching ROM bank %02x into %04x\n", bank, offset);
				memcpy(CPU->RAM + offset,
				       mapper->file->rom + bank*ROM_BANK_SIZE/2,
				       ROM_BANK_SIZE/2);

			}

			break;

		case ChangeMirroring:
			if( PPU->mirroring != FOUR_SCREEN_MIRRORING )
				PPU->mirroring = !(mapper->regs[2] & 0x1);
			break;

		case ToogleSRAM:
			command = (mapper->regs[3] & 0x80) >> 7;
			if( command != CPU->sram_enabled ) {
				DEBUG( printf("%s SRAM\n", (command ? "Enabling" : "Disabling" ) ) );
				CPU->sram_enabled = command;
			}
			command = (mapper->regs[3] & 0x40) >> 6;
			if( command != (CPU->sram_enabled & SRAM_RO) ) {
				DEBUG( printf("SRAM switching to %s mode\n", (command ? "RO": "RW") ) );
				if( command )
					CPU->sram_enabled |= SRAM_RO;
				else
					CPU->sram_enabled &= ~SRAM_RO;
			}
			break;

		case SetIRQ:
			irq_tmp = mapper->regs[4];
			break;

		case ResetIRQ:
			irq_counter = 0;
			break;

		case DisableIRQ:
			irq_enabled = 0;
			break;

		case EnableIRQ:
			irq_enabled = 1;
			break;

		default:
			break;
	}

	return;
}

void mmc3_reset() {

	/* The last ROM bank is always fixed into 0xE000-0xFFFF */
	if( powering_on ) {
		memcpy( CPU->RAM + 0xE000,
	      mapper->file->rom+((mapper->file->romBanks*2)-1)*ROM_BANK_SIZE/2,
	      ROM_BANK_SIZE/2);
		memcpy( CPU->RAM + 0xC000,
		   mapper->file->rom + (mapper->file->romBanks-1)*ROM_BANK_SIZE,
		   ROM_BANK_SIZE/2);

		if( mapper->file->vromBanks != 0 ) {
			memcpy( PPU->VRAM ,
			   mapper->file->rom,
			   VROM_BANK_SIZE);
		}

		powering_on = 0;
	}

	if( mapper->file->vromBanks != 0 )
		memcpy( PPU->VRAM, mapper->file->vrom, 0x2000);

	return;
}

void mmc3_update() {

	if( irq_counter == 0 )
		irq_counter = irq_tmp;
	else {
		if( PPU->CR2 & (SHOW_BACKGROUND|SHOW_SPRITES) )
			irq_counter--;

		if( irq_counter == 0 && irq_enabled ) {
			printf("MMC3: Triggering IRQ\n");
			execute_irq();
		}
	}

	return;
}
