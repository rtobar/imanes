#ifndef mmc3_h
#define mmc3_h

#include <stdint.h>

#define MMC3_ID (4)

/* Implementation of mapper struct function pointers */
void mmc3_initialize_mapper();
int  mmc3_check_address(uint16_t address);
void mmc3_switch_banks();
void mmc3_reset();
void mmc3_update();

#endif /* mmc3_h */
