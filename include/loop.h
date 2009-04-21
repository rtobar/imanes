#ifndef loop_h
#define loop_h

#ifdef __APPLE__
#include <SDL/SDL_thread.h>
#else
#include <SDL_thread.h>
#endif

extern SDL_mutex *pause_mutex;

/**
 * This is the main program loop
 */
void main_loop();

/**
 * Ends the VBlank period
 */
void end_vblank();

extern int run_loop;

#endif /* loop_h */
