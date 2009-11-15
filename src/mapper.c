/*  ImaNES: I'm a NES. An intelligent NES emulator

    mapper.c   -    Generic mapper support for ImaNES

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
#include <string.h>

#include "cnrom.h"
#include "i18n.h"
#include "mapper.h"
#include "mmc1.h"
#include "mmc3.h"
#include "nrom.h"
#include "unrom.h"

nes_mapper *mapper;

nes_mapper mapper_list[] = {
	{ NROM_ID , "NROM" , 0, nrom_initialize_mapper , nrom_check_address,
	  nrom_switch_banks , nrom_reset,  nrom_update , nrom_end_mapper } ,
	{ MMC1_ID , "MMC1" , 4, mmc1_initialize_mapper , mmc1_check_address,
	  mmc1_switch_banks , mmc1_reset,  mmc1_update , mmc1_end_mapper} ,
	{ UNROM_ID, "UNROM", 1, unrom_initialize_mapper, unrom_check_address,
	  unrom_switch_banks, unrom_reset, unrom_update, unrom_end_mapper } ,
	{ CNROM_ID, "CNROM", 1, cnrom_initialize_mapper, cnrom_check_address,
	  cnrom_switch_banks, cnrom_reset, cnrom_update, cnrom_end_mapper } ,
	{ MMC3_ID , "MMC3" , 8, mmc3_initialize_mapper , mmc3_check_address,
	  mmc3_switch_banks , mmc3_reset,  mmc3_update , mmc3_end_mapper } ,
	{ -1 }
};

void dump_mapper() {

	int i;

	printf("%s: ", mapper->name);

	if( mapper->reg_count ) {
		for(i=0; i!=mapper->reg_count; i++)
			printf("reg%d: %02x ", i, mapper->regs[i]);
		printf("\n");
	}
	else {
		printf(_("<no registers>\n"));
	}
}
