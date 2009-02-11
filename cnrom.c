#include <stdlib.h>

#include "cnrom.h"
#include "cpu.h"

int cnrom_check_address(uint16_t address) {

	if( 0x8000 <= address && address < 0xFFFF ) {
		mapper->reg1 = CPU->RAM[address];
		return 1;
	}

	return 0;
}

void cnrom_switch_banks() {
	
	//memcpy();

}
