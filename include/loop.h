#ifndef loop_h
#define loop_h

#ifdef __APPLE__
#include <SDL/SDL_thread.h>
#else
#include <SDL_thread.h>
#endif

/**
 * This is the main program loop
 */
int main_loop(void *);

/**
 * Ends the VBlank period
 */
void end_vblank();

extern int run_loop;

#endif /* loop_h */
