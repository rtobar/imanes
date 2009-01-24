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
   palette[INDEX].red = RED; \
   palette[INDEX].green = GREEN; \
   palette[INDEX].blue = BLUE;

extern nes_palette *palette;

#endif /* palette_h */
