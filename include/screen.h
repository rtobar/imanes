#ifndef screen_h
#define screen_h

#ifdef __APPLE__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif

#include "palette.h"

/* Screen geometry */
#define NES_SCREEN_WIDTH  256
#define NES_SCREEN_HEIGHT 240
#define NES_NTSC_HEIGHT   224
#define NES_SCREEN_BPP    32

/* This is used by the screenshot utility */
extern SDL_Surface *nes_screen;

/* Screen loop */
void screen_loop(void);

/**
 * This method initializes the screen where everything is going to be drawn
 */ 
void init_screen(void);

/**
 * This method ends all the screen-related resources
 */
void end_screen(void);

/**
 * Draw a pixel on the screen.
 */
void draw_pixel(int x, int y, nes_palette color);

/**
 * Redraw screen
 */
void redraw_screen();

#endif
