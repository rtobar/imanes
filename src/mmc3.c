/*  ImaNES: I'm a NES. An intelligent NES emulator

    mmc3.c   -    MMC3 Mapper emulation under ImaNES

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
#include "mapper.h"
#include "mmc3.h"
#include "ppu.h"

void mmc3_initialize_mapper() {

	return;
}

int  mmc3_check_address(uint16_t address) {

	return 0;
}

void mmc3_switch_banks() {

	return;
}

void mmc3_reset() {

	return;
}
