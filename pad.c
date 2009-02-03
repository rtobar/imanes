#include "pad.h"

nes_pad pads[2];

void initialize_pads() {

	/* TODO: Support 2nd pad */
	pads[0].plugged = 1;
	pads[0].pressed_keys = 0;
	pads[0].reads = 0;
}

void nes_keydown(SDL_keysym keysym) {

	printf("Pressing a button...\n");
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

		default:
			break;
	}

}

void nes_keyup(SDL_keysym keysym) {

	printf("Releasing a button...\n");
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

