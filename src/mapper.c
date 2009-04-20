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

#include <string.h>
#ifndef _MSC_VER
#include <strings.h>
#endif

#include "cnrom.h"
#include "mapper.h"
#include "mmc1.h"
#include "mmc3.h"
#include "nrom.h"
#include "unrom.h"

nes_mapper *mapper;

nes_mapper mapper_list[] = {
	{ 0 , "NROM" , nrom_initialize_mapper , nrom_check_address,
	  nrom_switch_banks , nrom_reset,  nrom_update } ,
	{ 1 , "MMC1" , mmc1_initialize_mapper , mmc1_check_address,
	  mmc1_switch_banks , mmc1_reset,  mmc1_update } ,
	{ 2 , "UNROM", unrom_initialize_mapper, unrom_check_address,
	  unrom_switch_banks, unrom_reset, unrom_update } ,
	{ 3 , "CNROM", cnrom_initialize_mapper, cnrom_check_address,
	  cnrom_switch_banks, cnrom_reset, cnrom_update } ,
	{ 4 , "MMC3" , mmc3_initialize_mapper , mmc3_check_address,
	  mmc3_switch_banks , mmc3_reset,  mmc3_update } ,
	{ -1 }
};
