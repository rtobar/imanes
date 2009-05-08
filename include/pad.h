#ifndef pad_h
#define pad_h

#ifdef __APPLE__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif
#include <stdint.h>

/* Joypad buttons */
#define NES_A      (0x01)
#define NES_B      (0x02)
#define NES_SELECT (0x04)
#define NES_START  (0x08)
#define NES_UP     (0x10)
#define NES_DOWN   (0x20)
#define NES_LEFT   (0x40)
#define NES_RIGHT  (0x80)

typedef struct _pad {
	uint8_t plugged;      /* The pad is plugged or not */
	uint8_t pressed_keys; /* Keys that are actually pressed, or'ed */
	uint8_t reads;
	uint8_t strobe_pad;   /* Flag to strobe pad */
} nes_pad;

extern nes_pad pads[2];

/* Initializes the pads */
void initialize_pads();

/* Dumps the contents of the given pad */
void dump_pad(int pad);

/* Update pads information for pressed keys */
void nes_keydown(SDL_keysym keysym);

/* Update pads information for released keys */
void nes_keyup(SDL_keysym keysym);

#endif /* pad_h */
