/*  ImaNES: I'm a NES. An intelligent NES emulator

    screen.c   -    ImaNES screen handling

    Copyright (C) 2009   Rodrigo Tobar Carrizo

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

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "imaconfig.h"
#include "loop.h"
#include "pad.h"
#include "screen.h"
#include "screenshot.h"

/* This is used by the screenshot utility */
SDL_Surface *nes_screen;
SDL_Surface *imanes_cursor;
int x;
int y;

void screen_loop() {

	SDL_Event event;

	while( SDL_PollEvent(&event) ) {
		switch(event.type) {

			case SDL_KEYUP:
				nes_keyup(event.key.keysym);
				break;

			/* Alt-F4 in Windows should lead us to SDL_QUIT */
			case SDL_KEYDOWN:
				if( !(event.key.keysym.sym == SDLK_F4 &&
					(event.key.keysym.mod & KMOD_LALT)) ) {
					nes_keydown(event.key.keysym);
					break;
				}

			case SDL_QUIT:
				printf("Quiting ImaNES\n");
				run_loop = 0;
				return;

			case SDL_MOUSEMOTION:
				x = event.motion.x;
				y = event.motion.y;
				break;
		}
	}

}


void init_screen() {
	
	char window_title[13];
	Uint32 color_key;

	if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr,"Error when initializing screen: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	nes_screen = SDL_SetVideoMode(NES_SCREEN_WIDTH*config.video_scale, NES_NTSC_HEIGHT*config.video_scale, NES_SCREEN_BPP, 0);

	imanes_cursor = SDL_LoadBMP("../icons/imanes-cursor.bmp");
	color_key = SDL_MapRGB(imanes_cursor->format, 98, 251, 14);
	SDL_SetColorKey(imanes_cursor, SDL_SRCCOLORKEY, color_key);

	SDL_ShowCursor(SDL_DISABLE);

	if( nes_screen == NULL ) {
		fprintf(stderr,"Error while setting video mode: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

#ifdef _MSC_VER
	sprintf_s(window_title,30,"ImaNES %s",IMANES_VERSION);
#else
	sprintf(window_title,"ImaNES %s",IMANES_VERSION);
#endif

	SDL_WM_SetCaption(window_title,NULL);
}

void end_screen() {

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

	SDL_Rect dst;

	/* Take screenshot */
	if( config.take_screenshot ) {
		config.take_screenshot = 0;
		save_screenshot();
	}

	dst.x = x;
	dst.y = y;
	SDL_BlitSurface(imanes_cursor, NULL, nes_screen, &dst);

	if( SDL_Flip(nes_screen) == -1 ) {
		fprintf(stderr,"Couldn't refresh screen :(\n");
		fprintf(stderr,"I'm exiting now\n");
		SDL_Quit();
		exit(EXIT_FAILURE);
	}

}

void show_fps(int fps) {
	char window_title[23];

	if( config.show_fps ) {
#ifdef _MSC_VER
		sprintf_s(window_title,30,"ImaNES %s - %d fps",IMANES_VERSION, fps);
#else
		sprintf(window_title,"ImaNES %s - %d fps",IMANES_VERSION, fps);
#endif
	}
	else {
#ifdef _MSC_VER
		sprintf_s(window_title,30,"ImaNES %s",IMANES_VERSION);
#else
		sprintf(window_title,"ImaNES %s",IMANES_VERSION);
#endif
	}
	SDL_WM_SetCaption(window_title, NULL);

}
