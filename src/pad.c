/*  ImaNES: I'm a NES. An intelligent NES emulator

    pad.c   -    Pad input emulation under ImaNES

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

#include "cpu.h"
#include "debug.h"
#include "imaconfig.h"
#include "loop.h"
#include "pad.h"

nes_pad pads[2];

void initialize_pads() {

	/* TODO: Support 2nd pad */
	pads[0].plugged = 1;
	pads[0].pressed_keys = 0;
	pads[0].reads = 0;
}

void nes_keydown(SDL_keysym keysym) {

	int i;

	switch( keysym.sym ) {

		/* Up */
		case SDLK_w:
			pads[0].pressed_keys |= NES_UP;
			break;

		/* Down */
		case SDLK_s:
			pads[0].pressed_keys |= NES_DOWN;
			break;

		/* Left */
		case SDLK_a:
			pads[0].pressed_keys |= NES_LEFT;
			break;

		/* Right */
		case SDLK_d:
			pads[0].pressed_keys |= NES_RIGHT;
			break;

		/* A button */
		case SDLK_j:
			pads[0].pressed_keys |= NES_A;
			break;

		/* B button */
		case SDLK_k:
			pads[0].pressed_keys |= NES_B;
			break;

		/* Start button */
		case SDLK_RETURN:
			pads[0].pressed_keys |= NES_START;
			break;

		/* Select button */
		case SDLK_RCTRL:
			pads[0].pressed_keys |= NES_SELECT;
			break;

		/* Show background */
		case SDLK_F1:
			INFO( printf("Background %s\n", (config.show_bg  ? "OFF" : "ON")) );
			config.show_bg  = ( !config.show_bg );
			break;

		/* Show front sprites */
		case SDLK_F2:
			INFO( printf("Front sprites %s\n", (config.show_front_spr ? "OFF": "ON")) );
			config.show_front_spr = ( !config.show_front_spr );
			break;

		/* Show front sprites */
		case SDLK_F3:
			INFO( printf("Back sprites %s\n", (config.show_back_spr ? "OFF": "ON")) );
			config.show_back_spr = ( !config.show_back_spr );
			break;

		case SDLK_F4:
			INFO( printf("Screen background %s\n", (config.show_screen_bg ? "OFF" : "ON")) );
			config.show_screen_bg = ( !config.show_screen_bg );
			break;

		/* Reset */
		case SDLK_F5:
			INFO( printf("Reseting NES\n") );
			CPU->reset = 1;
			break;

		/* Pause */
		case SDLK_F6:
			INFO( printf("%s emulation\n", (config.pause ? "Resuming" : "Pausing")) );
			if( !config.pause ) {
				pthread_mutex_lock(&pause_mutex);
				config.pause = 1;
			}
			else {
				pthread_mutex_unlock(&pause_mutex);
				config.pause = 0;
			}
			break;

		case SDLK_F7:
			INFO( printf("Instructions never executed:\n") );
			for(i=0;i!=INSTRUCTIONS_NUMBER;i++)
				if(instructions[i].size != 0 && !instructions[i].executed )
					printf("%02x - %s\n", instructions[i].opcode, instructions[i].name);

		/* Run as fast as possible */
		case SDLK_BACKSPACE:
			config.run_fast = 1;
			break;

		default:
			break;
	}

}

void nes_keyup(SDL_keysym keysym) {

	switch( keysym.sym ) {

		/* Up */
		case SDLK_w:
			pads[0].pressed_keys &= ~NES_UP;
			break;

		/* Down */
		case SDLK_s:
			pads[0].pressed_keys &= ~NES_DOWN;
			break;

		/* Left */
		case SDLK_a:
			pads[0].pressed_keys &= ~NES_LEFT;
			break;

		/* Right */
		case SDLK_d:
			pads[0].pressed_keys &= ~NES_RIGHT;
			break;

		/* A button */
		case SDLK_j:
			pads[0].pressed_keys &= ~NES_A;
			break;

		/* B button */
		case SDLK_k:
			pads[0].pressed_keys &= ~NES_B;
			break;

		/* Start button */
		case SDLK_RETURN:
			pads[0].pressed_keys &= ~NES_START;
			break;

		/* Select button */
		case SDLK_RCTRL:
			pads[0].pressed_keys &= ~NES_SELECT;
			break;

		/* Run at 60 fps */
		case SDLK_BACKSPACE:
			config.run_fast = 0;
			break;

		default:
			break;
	}

}
