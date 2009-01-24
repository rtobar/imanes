#ifndef ppu_h
#define ppu_h

#include <stdint.h>

typedef struct _ppu {
	uint8_t flags;    /* PPU flags */
	uint8_t *VRAM;    /* Video RAM. Physical memory: 0x0000 -> 0x3FFF */
	uint8_t *SPR_RAM; /* 256 bytes area memory for sprite attributes */
} nes_ppu;

/* Global PPU used through all the program */
extern nes_ppu *PPU;

/**
 * This function initializes the NES PPU
 */
void initialize_ppu();

#endif /* ppu_h */
