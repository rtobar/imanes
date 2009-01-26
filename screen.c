#include <pthread.h>
#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "screen.h"

static SDL_Surface *nes_screen;

void *screen_loop(void *args) {

	SDL_Event event;

	/* Here comes the main event loop */
	while(1) {

		/* Check if there are pending events. If there are not, wait for it */
		if( !SDL_PollEvent(&event) ) {	
			SDL_WaitEvent(&event);
		}

		switch(event.type) {

			case SDL_KEYDOWN:
				printf("A key has been pressed, thanks :')\n");
				printf("The keysym is %u", event.key.keysym.sym);
				break;

			case SDL_KEYUP:
				break;

			case SDL_QUIT:
				exit(EXIT_SUCCESS);
				break;
		}
	}

}


void init_screen() {
	
	//pthread_t screen_thread;

	char window_title[30];

	if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr,"Error when initializing screen: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	nes_screen = SDL_SetVideoMode(NES_SCREEN_WIDTH, NES_SCREEN_HEIGHT, NES_SCREEN_BPP, 0);
	if( nes_screen == NULL ) {
		fprintf(stderr,"Eror while setting vide mode: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	sprintf(window_title,"TobyNES emulator version %.1f",TOBYNES_VERSION);
	SDL_WM_SetCaption(window_title,NULL);

	/* The event loop should go in a separate thread */
	//pthread_create(&screen_thread, NULL, screen_loop, NULL);

}

void draw_pixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue) {

	Uint32 colour;
	Uint32 *pixmem32;

	/* This is the colour that will be put in the pixel */
	colour = SDL_MapRGB(nes_screen->format, red, green, blue);
	
	/* x = x*4 (32 bits per pixel); y = y*WIDTH*4 for the same reason */
	x *= 4;
	y *= NES_SCREEN_WIDTH*4;

	pixmem32 = (void*)nes_screen->pixels  + y + x;
	*pixmem32 = colour;
}

void redraw_screen() {

	if( SDL_Flip(nes_screen) == -1 ) {
		fprintf(stderr,"Couldn't refresh screen :(\n");
		fprintf(stderr,"I'm exiting now\n");
		exit(EXIT_FAILURE);
	}

}
