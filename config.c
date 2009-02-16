#include "config.h"

imanes_config config;

void initialize_configuration() {

	/* Emulator window */
	config.show_front_spr = 1;
	config.show_back_spr = 1;
	config.show_bg  = 1;

	/* Start on non-pause */
	config.pause = 0;
}
