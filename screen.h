#ifndef screen_h
#define screen_h

/* Screen geometry */
#define NES_SCREEN_WIDTH  256
#define NES_SCREEN_HEIGHT 240
#define NES_SCREEN_BPP    32

/**
 * This method initializes the screen where everything is going to be drawn
 */ 
void init_screen(void);

/**
 * Draw a pixel on the screen.
 */
void draw_pixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue);

/**
 * Redraw screen
 */
void redraw_screen();

#endif
