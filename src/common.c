/*  ImaNES: I'm a NES. An intelligent NES emulator

    common.c   -     Common utilities for ImaNES

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

#include <string.h>
#include <stdlib.h>

#include "platform.h"

void inst_lowercase(char *inst_name, char *ret) {

	int i;

	for(i=0;i!=3;i++)
		ret[i] = inst_name[i] + 32;
	ret[3] = '\0';

	return;
}

char *get_filename(char *full_pathname) {

	int i;
	char *tmp;

	/* Get just the name of the file */
	for(i=strlen(full_pathname); i>=0; i--)
		if( full_pathname[i] == DIR_SEP )
			break;

	tmp = (char *)malloc(strlen(full_pathname) - i);
	memcpy(tmp, full_pathname + i + 1, strlen(full_pathname) - i);

	/* Find where the extension of the file begins */
	for(i=strlen(tmp); i>=0; i--) {
		if( tmp[i] == '.' ) {
			tmp[i] = '\0';
			break;
		}
	}

	return tmp;
}
