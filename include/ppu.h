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
#define HORIZONTAL_MIRRORING       0
#define VERTICAL_MIRRORING         1
#define SINGLE_SCREEN_MIRRORING_A  2
#define SINGLE_SCREEN_MIRRORING_B  3
#define FOUR_SCREEN_MIRRORING      4

/* Sprite attributes */
#define SPRITE_BACK_PRIOR    (0x20)
#define SPRITE_FLIP_HORIZ    (0x40)
#define SPRITE_FLIP_VERT     (0x80)

typedef struct _ppu {

	/* Registers */
	uint8_t CR1;        /* Control Register 1 */
	uint8_t CR2;        /* Control Register 2 */
	uint8_t SR;         /* Status Register */

	/* Internal registers */
	uint8_t x;          /* Tiles X offset */
	uint8_t latch;      /* Used simultaneously by 0x2005/6 */
	uint16_t vram_addr; /* Address to be written/read by 0x2007 CPU RAM */
	uint16_t temp_addr; /* Temporal VRAM address */

	/* Associated memory */
	uint8_t *VRAM;      /* Video RAM. Physical memory: 0x0000 -> 0x3FFF */
	uint8_t *SPR_RAM;   /* 256 bytes area memory for sprite attributes */
	uint8_t spr_addr;   /* Address to be written by 0x2004 CPU RAM */

	uint8_t mirroring;     /* Type of mirroring */
	int scanline_timeout;  /* CPU cycles to next scanline */
	unsigned int lines;    /* Current scanline */

} nes_ppu;

/* Global PPU used through all the program */
extern nes_ppu *PPU;

/**
 * This function initializes the NES PPU
 */
void initialize_ppu();

/**
 * Function called every time that we need to draw a scanline
 */
void draw_line(int line, int frame);

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

/**
 * Dumps the content of the PPU to the stdout
 */
void dump_ppu();

/**
 * Dumps the contents of the SPR_RAM to the stdout
 */
void dump_spr_ram();

#endif /* ppu_h */
