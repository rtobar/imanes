#include "nrom.h"

inline int nrom_check_address(uint16_t address) {
	return 0;
}

/* This shouldn't be called never, anyways */
inline void nrom_switch_banks() {
	return;
}

void nrom_reset()
{
   /* 1 ROM bank games load twice to ensure vector tables */
   /* Free the file ROM (we don't need it anymore) */
   if( file->romBanks == 1 ) {
      memcpy( CPU->RAM + 0x8000, file->rom, ROM_BANK_SIZE);
      memcpy( CPU->RAM + 0xC000, CPU->RAM + 0x8000, ROM_BANK_SIZE);
   }
   /* 2 ROM bank games load one in 0x8000 and other in 0xC000 */
   /* Free the file ROM (we don't need it anymore) */
   else if (file->romBanks == 2 ) {
      memcpy( CPU->RAM + 0x8000, file->rom, ROM_BANK_SIZE);
      memcpy( CPU->RAM + 0xC000, file->rom + ROM_BANK_SIZE, ROM_BANK_SIZE);
   }
	return;
}
