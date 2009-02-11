#ifndef nrom_h
#define nrom_h

#include <stdint.h>

#include "mapper.h"

/* Implementation of the "no mapper" mapper */
int  nrom_check_address(uint16_t address);
void nrom_switch_banks();
void nrom_reset();

#endif
