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
#include "debug.h"
#include "i18n.h"
#include "imaconfig.h"
#include "loop.h"
#include "pad.h"
#include "platform.h"
#include "screen.h"
#include "screenshot.h"

/* This is used by the screenshot utility */
SDL_Surface *nes_screen;

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
				INFO( printf(_("Quiting ImaNES\n")) );
				run_loop = 0;
				return;
		}
	}

}


void init_screen() {
	
	char window_title[13];

	SDL_Surface *icon;
	Uint8 mask[32*4] = {
		0x00, 0x00, 0x07, 0xF0,
		0x00, 0x00, 0x0F, 0xFC,
		0x00, 0x00, 0x1F, 0xFE,
		0x00, 0x00, 0x3F, 0xFE,
		0x00, 0x00, 0x3F, 0xFF,
		0x00, 0x00, 0x7F, 0xFF,
		0x00, 0x00, 0xFE, 0x1F,
		0x00, 0x00, 0xFC, 0x1F,
		0x00, 0x00, 0xF8, 0x1F,
		0x00, 0x00, 0xF0, 0x3F,
		0x00, 0x00, 0xF0, 0x7F,
		0x00, 0x07, 0xE0, 0xFF,
		0x00, 0x07, 0x73, 0xFE,
		0x00, 0x03, 0xFF, 0xFC,
		0x00, 0x0F, 0xFF, 0xF0,
		0x00, 0x0E, 0xF7, 0xC0,
		0x03, 0x89, 0xF3, 0x00,
		0x07, 0xE3, 0xB0, 0x00,
		0x0F, 0xF7, 0x20, 0x00,
		0x1F, 0xFE, 0x00, 0x00,
		0x3F, 0xFC, 0x00, 0x00,
		0x7F, 0xFE, 0x00, 0x00,
		0xFF, 0xFE, 0x00, 0x00,
		0xFF, 0xFF, 0x00, 0x00,
		0xFF, 0xFF, 0x00, 0x00,
		0xFF, 0xFF, 0x00, 0x00,
		0xFF, 0xFE, 0x00, 0x00,
		0xFF, 0xFE, 0x00, 0x00,
		0x7F, 0xFC, 0x00, 0x00,
		0x7F, 0xFC, 0x00, 0x00,
		0x7F, 0xFC, 0x00, 0x00,
		0x3D, 0xBC, 0x00, 0x00
	};

	if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr,_("Error when initializing screen: %s\n"), SDL_GetError());
		exit(EXIT_FAILURE);
	}


 	icon = SDL_LoadBMP("imanes.bmp");
	if( icon == NULL )
		fprintf(stderr,_("Could not load ImaNES icon :(\n"));
	else
		SDL_WM_SetIcon(icon, mask);


	nes_screen = SDL_SetVideoMode(NES_SCREEN_WIDTH*config.video_scale, NES_NTSC_HEIGHT*config.video_scale, NES_SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF);

	if( nes_screen == NULL ) {
		fprintf(stderr,_("Error while setting video mode: %s\n"), SDL_GetError());
		exit(EXIT_FAILURE);
	}

	imanes_sprintf(window_title,30,"ImaNES %s",IMANES_VERSION);

	SDL_ShowCursor(SDL_DISABLE);
}

void end_screen() {

	SDL_FreeSurface(nes_screen);
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

	/* Take screenshot */
	if( config.take_screenshot ) {
		config.take_screenshot = 0;
		save_screenshot();
	}

	if( SDL_Flip(nes_screen) == -1 ) {
		fprintf(stderr,_("Couldn't refresh screen :(\n"));
		fprintf(stderr,_("I'm exiting now\n"));
		SDL_Quit();
		exit(EXIT_FAILURE);
	}

}

void show_fps(int fps) {

	char window_title[23];

	if( config.show_fps )
		imanes_sprintf(window_title,23,"ImaNES %s - %d fps",IMANES_VERSION, fps);
	else
		imanes_sprintf(window_title,23,"ImaNES %s",IMANES_VERSION);
	SDL_WM_SetCaption(window_title, NULL);

}
