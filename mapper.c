#include <string.h>
#include <strings.h>

#include "cnrom.h"
#include "mapper.h"
#include "nrom.h"

nes_mapper *mapper;

nes_mapper mapper_list[] = {
	{ 0 , "NROM"  , nrom_check_address  , nrom_switch_banks , nrom_reset } ,
	{ 2 , "UNROM" , unrom_check_address, unrom_switch_banks , unrom_reset } ,
	{ 3 , "CNROM" , cnrom_check_address , cnrom_switch_banks, cnrom_reset } ,
	{ -1 , NULL , NULL, NULL }
};
