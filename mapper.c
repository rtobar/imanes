#include <string.h>
#include <strings.h>

#include "cnrom.h"
#include "mapper.h"
#include "mmc1.h"
#include "nrom.h"
#include "unrom.h"

nes_mapper *mapper;

nes_mapper mapper_list[] = {
	{ 0 , "NROM" , nrom_initialize_mapper , nrom_check_address,
	  nrom_switch_banks , nrom_reset  } ,
	{ 1 , "MMC1" , mmc1_initialize_mapper , mmc1_check_address,
	  mmc1_switch_banks , mmc1_reset  } ,
	{ 2 , "UNROM", unrom_initialize_mapper, unrom_check_address,
	  unrom_switch_banks, unrom_reset } ,
	{ 3 , "CNROM", cnrom_initialize_mapper, cnrom_check_address,
	  cnrom_switch_banks, cnrom_reset } ,
	{ -1 }
};
