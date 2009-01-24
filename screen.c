#include <pthread.h>
#include <SDL/SDL.h>
#include <stdlib.h>

#include "common.h"
#include "screen.h"

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

	/* The event loop should go in a separate thread */
	//pthread_create(&screen_thread, NULL, screen_loop, NULL);

}
