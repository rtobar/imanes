#include "config.h"

imanes_config config;

void initialize_configuration() {

	/* Emulator window */
	config.show_spr = 1;
	config.show_bg  = 1;

	/* Separate windows */
	config.show_pattern_tables = 0;
	config.show_name_tables   = 0;

	/* Start on non-pause */
	config.pause = 0;
}
