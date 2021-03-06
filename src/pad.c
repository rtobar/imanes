/*  ImaNES: I'm a NES. An intelligent NES emulator

    pad.c   -    Pad input emulation under ImaNES

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

#include "cpu.h"
#include "debug.h"
#include "i18n.h"
#include "imaconfig.h"
#include "screen.h"
#include "pad.h"
#include "ppu.h"

nes_pad pads[2];

void initialize_pads() {

	pads[0].plugged = 1;
	pads[0].pressed_keys = 0;
	pads[0].reads = 0;

	pads[1].plugged = 1;
	pads[1].pressed_keys = 0;
	pads[1].reads = 0;

}

void dump_pad(int p) {

	printf(_("Pad %d. Pressed button: "), p);

	if( pads[p].pressed_keys == 0 )
		printf("<none>");
	else {
		if( pads[p].pressed_keys & NES_A )
			printf("A ");
		if( pads[p].pressed_keys & NES_B )
			printf("B ");
		if( pads[p].pressed_keys & NES_SELECT )
			printf("SELECT ");
		if( pads[p].pressed_keys & NES_START )
			printf("START ");
		if( pads[p].pressed_keys & NES_UP )
			printf("UP ");
		if( pads[p].pressed_keys & NES_DOWN )
			printf("DOWN ");
		if( pads[p].pressed_keys & NES_LEFT )
			printf("LEFT ");
		if( pads[p].pressed_keys & NES_RIGHT )
			printf("RIGHT ");
	}
	printf("\n");

}

void nes_keydown(SDL_keysym keysym) {

	int i;

	switch( keysym.sym ) {

		/****************/
		/* First player */
		/****************/
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
		case SDLK_k:
			pads[0].pressed_keys |= NES_A;
			break;

		/* B button */
		case SDLK_j:
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

		/*****************/
		/* Second player */
		/*****************/
		/* Up */
		case SDLK_UP:
			pads[1].pressed_keys |= NES_UP;
			break;

		/* Down */
		case SDLK_DOWN:
			pads[1].pressed_keys |= NES_DOWN;
			break;

		/* Left */
		case SDLK_LEFT:
			pads[1].pressed_keys |= NES_LEFT;
			break;

		/* Right */
		case SDLK_RIGHT:
			pads[1].pressed_keys |= NES_RIGHT;
			break;

		/* A button */
		case SDLK_m:
			pads[1].pressed_keys |= NES_A;
			break;

		/* B button */
		case SDLK_n:
			pads[1].pressed_keys |= NES_B;
			break;

		/* Start button */
		case SDLK_LSHIFT:
			pads[1].pressed_keys |= NES_START;
			break;

		/* Select button */
		case SDLK_LCTRL:
			pads[1].pressed_keys |= NES_SELECT;
			break;

		/*****************/
		/* ImaNES layers */
		/*****************/
		/* Show screen background */
		case SDLK_1:
			INFO( printf(_("Screen background %s\n"), (config.show_screen_bg ? "OFF" : "ON")) );
			config.show_screen_bg = ( !config.show_screen_bg );
			break;

		/* Show back sprites */
		case SDLK_2:
			INFO( printf(_("Back sprites %s\n"), (config.show_back_spr ? "OFF": "ON")) );
			config.show_back_spr = ( !config.show_back_spr );
			break;

		/* Show background */
		case SDLK_3:
			INFO( printf(_("Background %s\n"), (config.show_bg  ? "OFF" : "ON")) );
			config.show_bg  = ( !config.show_bg );
			break;

		/* Show front sprites */
		case SDLK_4:
			INFO( printf(_("Front sprites %s\n"), (config.show_front_spr ? "OFF": "ON")) );
			config.show_front_spr = ( !config.show_front_spr );
			break;

		/*******************/
		/* ImaNES channels */
		/*******************/
		/* Toggle Square1 channel */
		case SDLK_5:
			INFO( printf(_("Square1 channel %s\n"), (config.apu_square1 ? "OFF" : "ON")) );
			config.apu_square1 = ( !config.apu_square1 );
			break;

		/* Toggle Square2 channel */
		case SDLK_6:
			INFO( printf(_("Square2 channel %s\n"), (config.apu_square2 ? "OFF" : "ON")) );
			config.apu_square2 = ( !config.apu_square2 );
			break;

		/* Toggle Triangle channel */
		case SDLK_7:
			INFO( printf(_("Triangle channel %s\n"), (config.apu_triangle ? "OFF" : "ON")) );
			config.apu_triangle = ( !config.apu_triangle );
			break;

		/* Toggle Noise channel */
		case SDLK_8:
			INFO( printf(_("Noise channel %s\n"), (config.apu_noise ? "OFF" : "ON")) );
			config.apu_noise = ( !config.apu_noise );
			break;

		/* Toggle DMC channel */
		case SDLK_9:
			INFO( printf(_("DMC channel %s\n"), (config.apu_dmc ? "OFF" : "ON")) );
			config.apu_dmc = ( !config.apu_dmc );
			break;

		/******************/
		/* ImaNES actions */
		/******************/
		/* Take a screenshot of the current screen at the end of the frame */
		case SDLK_F1:
			config.take_screenshot = 1;
			break;

		/* Save a state. We just set a flag, since the actual
		 * loading is done in the main loop. */
		case SDLK_F2:
			config.save_state = 1;
			break;

		/* Choose which state to use */
		case SDLK_F3:
			config.current_state++;
			if( config.current_state == 10 )
				config.current_state = 0;
			break;

		/* Load a state. We just set a flag, since the actual
		 * loading is done in the main loop. */
		case SDLK_F4:
			config.load_state = 1;
			break;

		/* Reset */
		case SDLK_F5:
			INFO( printf(_("Resetting NES\n")) );
			CPU->reset = 1;
			break;

		case SDLK_F6:
			INFO( printf(_("Show frames per second %s\n"), (config.show_fps ? "OFF": "ON")) );
			config.show_fps = ( !config.show_fps );
			break;

		/* This is for debugging */
		case SDLK_F7:
			dump_spr_ram();
			INFO( printf(_("Instructions never executed:\n")) );
			for(i=0;i!=INSTRUCTIONS_NUMBER;i++)
				if(instructions[i].size != 0 && !instructions[i].executed )
					printf("%02x - %s\n", instructions[i].opcode, instructions[i].name);
			break;

		case SDLK_F8:
			INFO( printf(_("%s sound recording (not yet implemented)"), (config.sound_rec ? _("Stopping") : _("Starting"))) );
			config.sound_rec = ( !config.sound_rec );
			break;

		/* Pause */
		case SDLK_ESCAPE:
			INFO( printf(_("%s emulation\n"), (config.pause ? _("Resuming") : _("Pausing"))) );
			config.pause = ( !config.pause );
			break;

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

		/****************/
		/* First player */
		/****************/
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
		case SDLK_k:
			pads[0].pressed_keys &= ~NES_A;
			break;

		/* B button */
		case SDLK_j:
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

		/*****************/
		/* Second player */
		/*****************/
		/* Up */
		case SDLK_UP:
			pads[1].pressed_keys &= ~NES_UP;
			break;

		/* Down */
		case SDLK_DOWN:
			pads[1].pressed_keys &= ~NES_DOWN;
			break;

		/* Left */
		case SDLK_LEFT:
			pads[1].pressed_keys &= ~NES_LEFT;
			break;

		/* Right */
		case SDLK_RIGHT:
			pads[1].pressed_keys &= ~NES_RIGHT;
			break;

		/* A button */
		case SDLK_m:
			pads[1].pressed_keys &= ~NES_A;
			break;

		/* B button */
		case SDLK_n:
			pads[1].pressed_keys &= ~NES_B;
			break;

		/* Start button */
		case SDLK_LSHIFT:
			pads[1].pressed_keys &= ~NES_START;
			break;

		/* Select button */
		case SDLK_LCTRL:
			pads[1].pressed_keys &= ~NES_SELECT;
			break;

		/* Run at 60 fps */
		case SDLK_BACKSPACE:
			config.run_fast = 0;
			break;

		default:
			break;
	}

}

