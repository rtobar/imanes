#ifndef palette_h
#define palette_h

#include <stdint.h>

/**
 * It keeps the current colors that can be drawn in the NES
 */
typedef struct _palette {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} nes_palette;


#define FILL_NES_PALETTE(INDEX, RED, GREEN, BLUE) \
   system_palette[INDEX].red = RED; \
   system_palette[INDEX].green = GREEN; \
   system_palette[INDEX].blue = BLUE;

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