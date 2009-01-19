#include <SDL/SDL.h>

#include "common.h"
#include "screen.h"

void init_screen() {
	
	SDL_Surface *screen;
	char window_title[30];

	if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr,"Error when initializing screen: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	screen = SDL_SetVideoMode(NES_SCREEN_WIDTH, NES_SCREEN_HEIGHT, NES_SCREEN_BPP, 0);
	if( screen == NULL ) {
		fprintf(stderr,"Eror while setting vide mode: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	sprintf(window_title,"TobyNES emulator version %.1f",TOBYNES_VERSION);
	SDL_WM_SetCaption(window_title,window_title);

	/* Here comes the main event loop */
}
