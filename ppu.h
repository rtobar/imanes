#ifndef ppu_h
#define ppu_h

#include <stdint.h>

/* Flags for PPU CR1 */
#define VERTICAL_WRITE       (0x04)
#define SPR_PATTERN_ADDRESS  (0x08)
#define SCR_PATTERN_ADDRESS  (0x10)
#define SPRITE_SIZE_8x16     (0x20)
#define PPU_MASTER_SLAVE     (0x40)
#define VBLANK_ENABLE        (0x80)

/* Flags for the PPU CR2 */
#define MONOCHROME_MODE      (0x01)
#define DONTCLIP_BACKGROUND  (0x02)
#define DONTCLIP_SPRITES     (0x04)
#define SHOW_BACKGROUND      (0x08)
#define SHOW_SPRITES         (0x10)

/* Flags for the PPU Status Register */
#define IGNORE_VRAM_WRITE    (0x10)
#define MAX_SPRITES_DRAWN    (0x20)
#define HIT_FLAG             (0x40)
#define VBLANK_FLAG          (0x80)

/** Mirroring types */
#define HORIZONTAL_MIRRORING     0
#define VERTICAL_MIRRORING       1
#define SINGLE_SCREEN_MIRRORING  2
#define FOUR_SCREEN_MIRRORING    3

typedef struct _ppu {
	uint8_t CR1;        /* Control Register 1 */
	uint8_t CR2;        /* Control Register 2 */
	uint8_t SR;         /* Status Register */
	uint8_t *VRAM;      /* Video RAM. Physical memory: 0x0000 -> 0x3FFF */
	uint16_t vram_addr; /* Address to be written/read by 0x2007 CPU RAM */

	uint8_t *SPR_RAM; /* 256 bytes area memory for sprite attributes */
	uint8_t spr_addr; /* Address to be written by 0x2004 CPU RAM */

	uint8_t mirroring; /* Type of mirroring */
} nes_ppu;

/* Global PPU used through all the program */
extern nes_ppu *PPU;

/**
 * This function initializes the NES PPU
 */
void initialize_ppu();

/**
 * Initializes PPU's VRAM with contents from iNES file VROM
 */
void init_ppu_vram(ines_file *);

/**
 * Function called every time that we need to draw a scanline
 */
void draw_line(int line);

/**
 * Reads a value from a given PPU VRAM address. This method should
 * handles the mirroring that should ocurr in the PPU VRAM
 */
uint8_t read_ppu_vram(uint16_t address);

/**
 * Write a value in PPU VRAM space. This method handles the mirroring
 * that should ocurr in the PPU VRAM
 */
void write_ppu_vram(uint16_t address, uint8_t value);


#endif /* ppu_h */
