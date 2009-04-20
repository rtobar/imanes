/*  ImaNES: I'm a NES. An intelligent NES emulator

    states.c   -    Internal states of emulation for ImaNES

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

#include "imaconfig.h"
#include "states.h"

imanes_state *load_state(int i) {

	char *user_imanes_dir;
	imanes_state *state;

	user_imanes_dir = get_imanes_dir();
	state = (imanes_state *)malloc(sizeof(imanes_state));

	return state;
}

void save_state(imanes_state *s, int i) {

	char *user_imanes_dir;

	user_imanes_dir = get_imanes_dir();

}
