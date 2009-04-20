#ifndef nrom_h
#define nrom_h

#include <stdint.h>

#define NROM_ID (0)

/* Implementation of the "no mapper" mapper */
void nrom_initialize_mapper();
int  nrom_check_address(uint16_t address);
void nrom_switch_banks();
void nrom_reset();
void nrom_update();

#endif /* nrom_h */
