#ifndef palette_h
#define palette_h

#include <stdint.h>

/**
 * It keeps the current colors that can be drawn in the NES
 */
typedef struct _palette {
	uint8_t  red;
	uint8_t  green;
	uint8_t  blue;
	uint32_t combined;
} nes_palette;


#define FILL_NES_PALETTE(PALETTE, INDEX, RED, GREEN, BLUE) \
   PALETTE[INDEX].red = RED; \
   PALETTE[INDEX].green = GREEN; \
   PALETTE[INDEX].blue = BLUE; \
   PALETTE[INDEX].combined = RED << 16 | GREEN << 8 | BLUE;

/* System palette with the actual colors */
extern nes_palette *system_palette;

/**
 * This function initializes the palette
 */
void initialize_palette();

/**
 *  Dumps the contents of the palette to the stdout
 */
void dump_palette();

#endif /* palette_h */
