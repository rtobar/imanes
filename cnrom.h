#ifndef cnrom_h
#define cnrom_h

#include <stdint.h>

/* Implementation of mapper struct function pointers */
int  cnrom_check_address(uint16_t address);
void cnrom_switch_banks();
void cnrom_reset();

#endif
