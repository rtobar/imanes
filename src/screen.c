/*  ImaNES: I'm a NES. An intelligent NES emulator

    screen.c   -    ImaNES screen handling

    Copyright (C) 2008   Rodrigo Tobar Carrizo

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <pthread.h>
#ifdef __APPLE__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "imaconfig.h"
#include "loop.h"
#include "pad.h"
#include "screen.h"

static pthread_t screen_thread;

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
				nes_keydown(event.key.keysym);
				break;

			case SDL_KEYUP:
				nes_keyup(event.key.keysym);
				break;

			case SDL_QUIT:
				run_loop = 0;
				return NULL;
		}
	}

}


void init_screen() {
	
	char window_title[30];

	if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr,"Error when initializing screen: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	nes_screen = SDL_SetVideoMode(NES_SCREEN_WIDTH*config.video_scale, NES_NTSC_HEIGHT*config.video_scale, NES_SCREEN_BPP, 0);
	if( nes_screen == NULL ) {
		fprintf(stderr,"Error while setting video mode: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

#ifdef _MSC_VER
	sprintf_s(window_title,30,"ImaNES emulator version %s",IMANES_VERSION);
#else
	sprintf(window_title,"ImaNES emulator version %s",IMANES_VERSION);
#endif

	SDL_WM_SetCaption(window_title,NULL);

	/* The event loop should go in a separate thread */
	pthread_create(&screen_thread, NULL, screen_loop, NULL);

}

void end_screen() {

	pthread_join(screen_thread,NULL);
	SDL_Quit();

}

void draw_pixel(int x, int y, nes_palette color) {

	int i;
	int j;
	Uint32 colour;
	Uint32 *pixmem32;

	y -= 8;
	if( y < 0 || y >= NES_NTSC_HEIGHT )
		return;

	/* This is the color that will be put in the pixel */
	if( config.use_sdl_colors )
		colour = SDL_MapRGB(nes_screen->format, color.red, color.green, color.blue);
	else
		colour = (color.red<<16) | ((color.green)<<8) | (color.blue);
	
	/* x = x*4 (32 bits per pixel); y = y*WIDTH*4 for the same reason */

	pixmem32 = (Uint32*)nes_screen->pixels;

	if( config.video_scale != 1 ) {
		for(i=0;i!=config.video_scale;i++)
			for(j=0;j!=config.video_scale;j++) 
				pixmem32[config.video_scale*x+i+NES_SCREEN_WIDTH*config.video_scale*(y*config.video_scale+j)] = colour;
	}
	else
		pixmem32[x+NES_SCREEN_WIDTH*y] = colour;

}

void redraw_screen() {

	if( SDL_Flip(nes_screen) == -1 ) {
		fprintf(stderr,"Couldn't refresh screen :(\n");
		fprintf(stderr,"I'm exiting now\n");
		SDL_Quit();
		exit(EXIT_FAILURE);
	}

}
