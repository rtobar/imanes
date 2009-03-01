#ifndef unrom_h
#define unrom_h

#include <stdint.h>

/* Implementation of mapper struct function pointers */
void unrom_initialize_mapper();
int  unrom_check_address(uint16_t address);
void unrom_switch_banks();
void unrom_reset();

#endif
