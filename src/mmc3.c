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
#include "i18n.h"
#include "mapper.h"
#include "mmc3.h"
#include "ppu.h"

/* Are we powering on the machine? */
static int powering_on;

static int irq_enabled;
static int irq_triggered;
static int zero_written;
static uint8_t irq_tmp;
static uint8_t irq_counter;

static uint8_t address_cmd;
static uint8_t prev_address_cmd;
static uint8_t prev_regs[8];

void mmc3_initialize_mapper() {

	mapper->regs = (uint8_t *)malloc(8);
	memset(mapper->regs,0,8);

	mapper->regs[0] = 0; /* 0x8000 and 0xA000 are switchable */
	powering_on = 1;

	address_cmd = 0;
	prev_address_cmd = 0xFF;
	memset(&prev_regs, 0, 8);

	irq_counter = 0;
	irq_tmp = 0;

	/* This will prevent for initial updates on the counter */
	zero_written = 1;
	irq_triggered = 0;

	return;
}

void mmc3_perform_vram_swap() {

	uint8_t  chr_mode;
	uint16_t offset;

	/* Depending on the mode we perform different swappings, like this:
	 *
	 *                $0000   $0400   $0800   $0C00   $1000   $1400   $1800   $1C00
	 *              +---------------+---------------+-------+-------+-------+-------+
	 * CHR Mode 0:  |     <R:0>     |     <R:1>     |  R:2  |  R:3  |  R:4  |  R:5  |
	 *              +---------------+---------------+---------------+---------------+
	 * CHR Mode 1:  |  R:2  |  R:3  |  R:4  |  R:5  |     <R:0>     |     <R:1>     |
	 *              +-------+-------+-------+-------+---------------+---------------+
	 */

	chr_mode = address_cmd & 0x80;

	/* <R:0> and <R:1>, they have an offset of 0x1000 when in CHR Mode 1 */
	offset = (chr_mode ? 0x1000 : 0);
	SWAP_VRAM_2K(offset,          mapper->regs[0] >> 1);
	SWAP_VRAM_2K(offset + 0x0800, mapper->regs[1] >> 1);

	/* R:2 - R:5, they have an offset of 0x1000 when in CHR Mode 0 */
	offset ^= 0x1000;
	SWAP_VRAM_1K(offset,          mapper->regs[2]);
	SWAP_VRAM_1K(offset + 0x0400, mapper->regs[3]);
	SWAP_VRAM_1K(offset + 0x0800, mapper->regs[4]);
	SWAP_VRAM_1K(offset + 0x0C00, mapper->regs[5]);

}

void mmc3_perform_ram_swap() {

	uint8_t  prg_mode;
	uint16_t offset;

	/* Depending on the mode we perform different swappings, like this:
	 *
	 *                $8000   $A000   $C000   $E000
	 *              +-------+-------+-------+-------+
	 * PRG Mode 0:  |  R:6  |  R:7  | { -2} | { -1} |
	 *              +-------+-------+-------+-------+
	 * PRG Mode 1:  | { -2} |  R:7  |  R:6  | { -1} |
	 *              +-------+-------+-------+-------+
	 */

	prg_mode = address_cmd & 0x40;

	/* {-1} is fixed and we already copy it on power-on */

	/* R:6 and R:7 */
	offset = (prg_mode ? 0xC000 : 0x8000);
	SWAP_RAM_8K(offset, mapper->regs[6]);
	SWAP_RAM_8K(0xA000, mapper->regs[7]);

	/* {-2} depends on the mode */
	offset = (prg_mode ? 0x8000 : 0xC000);
	SWAP_RAM_8K(offset, mapper->file->romBanks8k - 2);

}

int mmc3_check_address(uint16_t address) {

	uint8_t value;
	uint8_t tmp;

	if( address < 0x8000 )
		return 0;

	/* After 0x8000 there's no mirroring, so we can go and
	 * directly read the RAM memory */
	address &= 0xE001;
	value = CPU->RAM[address];

	/* This only set values, does not take any action */
	switch(address) {

		case 0x8000:
			address_cmd = value;

			/* Instantaneously produce a RAM or VRAM swap if the PRG mode
			 * or the CHR mode have changed, respectively. Note that
			 * both can happen at the same time. */
			if( ((address_cmd & 0x40) != (prev_address_cmd & 0x40)) || prev_address_cmd == 0xFF ) {
				INFO( printf(_("Change of PRG mode: %u\n"), address_cmd & 0x40) );
				mmc3_perform_ram_swap();
			}
			if( ((address_cmd & 0x80) != (prev_address_cmd & 0x80))  || prev_address_cmd == 0xFF ) {
				INFO( printf(_("Change of CHR mode: %u\n"), address_cmd & 0x80) );
				mmc3_perform_vram_swap();
			}

			/* Save the interesting bits to check it in the next iteration */
			prev_address_cmd = value & 0xC0;;
			break;

		case 0x8001:
			/* The register index */
			tmp = address_cmd & 0x07;
			mapper->regs[tmp] = value;

			INFO( printf("MMC3: Reg[%d] = $%02X at %d ", tmp, mapper->regs[tmp], PPU->lines) );

			/* Instantaneously produce a RAM or VRAM swap if the
			 * value for the written register changed. R:0 to R:5 are used
			 * for CHR swapping, R:6 and R:7 for PRG swapping. */
//			if( mapper->regs[tmp] != prev_regs[tmp] ) {
				INFO( printf(_("changed!\n")) );
				if( tmp > 5 )
					mmc3_perform_ram_swap();
				else
					mmc3_perform_vram_swap();
//			}
//			else
//				INFO( printf(_("ignoring\n")) );

			/* Save the value to check it in the next iteration */
			prev_regs[tmp] = value;
			break;

		case 0xA000:
			if( PPU->mirroring != FOUR_SCREEN_MIRRORING )
				PPU->mirroring = !(value & 0x1);
			break;

		case 0xA001:

			tmp = (value & 0x80) >> 7;
			if( tmp != CPU->sram_enabled ) {
				INFO( printf("%s SRAM\n", (tmp ? _("Enabling") : _("Disabling") ) ) );
				CPU->sram_enabled = tmp;
			}
			tmp = (value & 0x40) >> 6;
			if( tmp != (CPU->sram_enabled & SRAM_RO) ) {
				INFO( printf(_("SRAM switching to %s mode\n"), (tmp ? "RO": "RW") ) );
				if( tmp )
					CPU->sram_enabled |= SRAM_RO;
				else
					CPU->sram_enabled &= ~SRAM_RO;
			}
			break;

		case 0xC000:

			DEBUG( printf(_("MMC3: Writting %02x to 0xC000\n"), value) );
			if( value ) {
				irq_tmp = value;
				zero_written = 0;
			}
			else {
				irq_tmp = 0;
				if( !zero_written )
					irq_triggered = 1;
				zero_written = 1;
			}
			break;

		case 0xC001:
			DEBUG( printf(_("MMC3: Resetting counter to 0\n")) );
			irq_counter = 0;
			zero_written = 0;
			irq_triggered = 0;
			break;

		case 0xE000:
			irq_enabled = 0;
			break;

		case 0xE001:
			irq_enabled = 1;
			break;

		default:
			fprintf(stderr,_("Error, execution shouldn't have reached this point\n"));
			break;

	}

	return 0;
}

void mmc3_switch_banks() {
	return;
}

void mmc3_reset() {

	/* The last 8kb ROM bank is always fixed into 0xE000-0xFFFF */
	if( powering_on ) {
		SWAP_RAM_8K(0xE000, mapper->file->romBanks8k - 1);

		if( mapper->file->vromBanks != 0 )
			SWAP_VRAM(0, mapper->file->vrom, VROM_BANK_SIZE);

		powering_on = 0;
	}

	if( mapper->file->vromBanks != 0 )
		SWAP_VRAM(0, mapper->file->vrom, VROM_BANK_SIZE);

	return;
}

void mmc3_update() {

	/* Writting 0x00 to 0xC000 will produce a single IRQ 
	 * The process will be stopped until a non-zero value 
	 * is written in 0xC000*/
	if( zero_written ) {
		if( irq_triggered && irq_enabled ) {
			DEBUG( printf(_("MMC3: Triggering IRQ caused by 0 writing to 0xC000\n")) );
			CPU->SR &= ~B_FLAG;
			execute_irq();
			irq_triggered = 0;
		}
		return;
	}

	if( irq_counter == 0 ) {
		irq_counter = irq_tmp+1;
		DEBUG( printf(_("MMC3: Setting irq_counter to %u\n"), irq_counter) );
	}
	else {
		if( PPU->CR2 & (SHOW_BACKGROUND|SHOW_SPRITES) ) {
			irq_counter--;
			DEBUG( printf(_("MMC3: Decrementing irq_counter. New value: %u\n"), irq_counter) );
		}

		if( irq_counter == 0 && irq_enabled ) {
			DEBUG( printf(_("MMC3: Triggering IRQ\n")) );
			CPU->SR &= ~B_FLAG;
			execute_irq();
		}
	}

	return;
}

void mmc3_end_mapper() {
	free(mapper->regs);
}
