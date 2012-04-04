#ifndef screen_h
#define screen_h

#include <SDL/SDL.h>

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
#define draw_pixel(x, y, color) \
do { \
	int loop1; \
	int loop2; \
	Uint32 colour; \
	Uint32 *pixmem32; \
	int realy = y - 8; \
	if( realy >= 0 && realy < NES_NTSC_HEIGHT ) { \
		/* This is the color that will be put in the pixel */ \
		if( config.use_sdl_colors ) \
			colour = SDL_MapRGB(nes_screen->format, color.red, color.green, color.blue); \
		else \
			colour = color.combined; \
		pixmem32 = (Uint32*)nes_screen->pixels; \
		if( config.video_scale != 1 ) { \
			for(loop1=0;loop1!=config.video_scale;loop1++) \
				for(loop2=0;loop2!=config.video_scale;loop2++) \
					pixmem32[config.video_scale*x+loop1+NES_SCREEN_WIDTH*config.video_scale*(realy*config.video_scale+loop2)] = colour; \
		} \
		else \
			pixmem32[x+NES_SCREEN_WIDTH*realy] = colour; \
	} \
} while(0);

/**
 * Shows the current fps in the emulation window title
 */
void show_fps();

/**
 * Redraw screen
 */
void redraw_screen();

#endif
