/*  ImaNES: I'm a NES. An intelligent NES emulator

    config.c   -    ImaNES configuration handling

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

#include "config.h"

imanes_config config;

void initialize_configuration() {

	/* Emulator window */
	config.show_front_spr = 1;
	config.show_back_spr = 1;
	config.show_bg  = 1;

	/* Start on non-pause and at 60 fps */
	config.pause = 0;
	config.run_fast = 0;

	/* Set default video scale factor */
	if( config.video_scale == 0 )
		config.video_scale = 1;

}
