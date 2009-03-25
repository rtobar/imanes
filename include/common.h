#ifndef common_h
#define common_h

#include <stdint.h>

/* Version string */
#define IMANES_VERSION "0.1"

/* RAM sizes */
#define NES_RAM_SIZE     (0x10000)
#define NES_VRAM_SIZE    (0x4000)
#define NES_SPR_RAM_SIZE (0x100)

/* ROM sizes */
#define ROM_BANK_SIZE   (16*1024)
#define VROM_BANK_SIZE  ( 8*1024)

#define NES_PALETTE_COLORS (64)

#define CYCLES_PER_SCANLINE (114)

typedef struct {
	int fd;
	uint8_t romBanks;
	uint8_t vromBanks;
	uint8_t mapper_id;
	uint8_t *rom;
	uint8_t *vrom;

	int has_trainer;
} ines_file;

#endif /* common_h */
