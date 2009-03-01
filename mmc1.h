#ifndef mmc1_h
#define mmc1_h

#include <stdint.h>

/* Implementation of mapper struct function pointers */
void mmc1_initialize_mapper();
int  mmc1_check_address(uint16_t address);
void mmc1_switch_banks();
void mmc1_reset();

#endif /* mmc1_h */