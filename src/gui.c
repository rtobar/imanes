/*  ImaNES: I'm a NES. An intelligent NES emulator

    gui.c   -    ImaNES GUI implementation

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

#include <SDL/SDL.h>

#include "imaconfig.h"
#include "loop.h"
#include "screen.h"

void gui_keydown(SDL_keysym);
void redraw_gui();

static SDL_Surface *imanes_cursor;
static SDL_Surface *gui_bg;
static int _cursor_x;
static int _cursor_y;

void init_gui() {

	Uint32 color_key;

	gui_bg = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCCOLORKEY | SDL_SRCALPHA,
	                              NES_SCREEN_WIDTH*config.video_scale,
	                              NES_NTSC_HEIGHT*config.video_scale,
	                              NES_SCREEN_BPP,
	                              0, 0, 0, 0);

	imanes_cursor = SDL_LoadBMP("../icons/imanes-cursor.bmp");
	color_key = SDL_MapRGB(imanes_cursor->format, 98, 251, 14);
	SDL_SetColorKey(imanes_cursor, SDL_SRCCOLORKEY, color_key);
	SDL_ShowCursor(SDL_DISABLE);
	_cursor_x = 100;
	_cursor_y = 100;

	return;
}

void gui_loop() {

	SDL_Event event;

	while( config.pause ) {
		while( SDL_PollEvent(&event) ) {
			switch(event.type) {
	
				/* Alt-F4 in Windows should lead us to SDL_QUIT */
				case SDL_KEYDOWN:
					if( !(event.key.keysym.sym == SDLK_F4 &&
						(event.key.keysym.mod & KMOD_LALT)) ) {
						gui_keydown(event.key.keysym);
						break;
					}
	
				case SDL_QUIT:
					printf("Quiting ImaNES\n");
					run_loop = 0;
					config.pause = 0;
					return;
	
				case SDL_MOUSEMOTION:
					_cursor_x = event.motion.x;
					_cursor_y = event.motion.y;
					break;
			}
	
		}

		redraw_gui();
		usleep(20000);
	}

}

void gui_keydown(SDL_keysym keysym) {

	switch( keysym.sym ) {

		case SDLK_ESCAPE:
			config.pause = 0;
			break;

		default:
			break;
	}

}

void redraw_gui() {

	SDL_Rect dst;

	memcpy(nes_screen->pixels, gui_bg->pixels, sizeof(Uint32)*NES_SCREEN_WIDTH*config.video_scale*NES_NTSC_HEIGHT*config.video_scale);

	/* Finally, show where the cursor where is */
	dst.x = _cursor_x;
	dst.y = _cursor_y;
	SDL_BlitSurface(imanes_cursor, NULL, nes_screen, &dst);

	if( SDL_Flip(nes_screen) == -1 ) {
		fprintf(stderr,"Couldn't refresh screen :(\n");
		fprintf(stderr,"I'm exiting now\n");
		SDL_Quit();
		exit(EXIT_FAILURE);
	}
}

void gui_set_background() {

	memcpy(gui_bg->pixels, nes_screen->pixels, sizeof(Uint32)*NES_SCREEN_WIDTH*config.video_scale*NES_NTSC_HEIGHT*config.video_scale);

}