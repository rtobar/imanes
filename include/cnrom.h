#ifndef cnrom_h
#define cnrom_h

#include <stdint.h>

#define CNROM_ID (3)

/* Implementation of mapper struct function pointers */
void cnrom_initialize_mapper();
int  cnrom_check_address(uint16_t address);
void cnrom_switch_banks();
void cnrom_reset();
void cnrom_update();
void cnrom_end_mapper();

#endif /* cnrom_h */
