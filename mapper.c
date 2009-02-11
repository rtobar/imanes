#include <string.h>
#include <strings.h>

#include "cnrom.h"
#include "mapper.h"
#include "nrom.h"

nes_mapper *mapper;

nes_mapper mapper_list[] = {
	{ 0 , "NROM"  , nrom_check_address  , nrom_switch_banks  } ,
	{ 2 , "CNROM" , cnrom_check_address , cnrom_switch_banks } ,
	{ -1 }
};
