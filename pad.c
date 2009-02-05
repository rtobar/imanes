#include "config.h"
#include "pad.h"

nes_pad pads[2];

void initialize_pads() {

	/* TODO: Support 2nd pad */
	pads[0].plugged = 1;
	pads[0].pressed_keys = 0;
	pads[0].reads = 0;
}

void nes_keydown(SDL_keysym keysym) {

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

		/* Show sprites */
		case SDLK_F1:
			config.show_spr = (config.show_spr ? 0 : 1);
			break;

		/* Show background */
		case SDLK_F2:
			config.show_bg  = (config.show_bg  ? 0 : 1);
			break;

		/* Show pattern table */
		case SDLK_F3:
			config.show_patter_tables = (config.show_patter_tables ? 0 : 1);
			break;

		/* Show name table */
		case SDLK_F4:
			config.show_name_tables = (config.show_name_tables ? 0 : 1);
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

		default:
			break;
	}

}

