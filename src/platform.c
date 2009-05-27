/*  ImaNES: I'm a NES. An intelligent NES emulator

    platform.c   -    ImaNES platform-specific code

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

#include <stdarg.h>
#include <stdio.h>

#include "platform.h"

int imanes_sprintf(char *str, int size, const char *format, ...) {

	int ret;
	va_list ap;

	va_start(ap,format);

#ifdef _MSC_VER
	ret = vsprintf_s(str, size, format, ap);
#else
	ret = vsprintf(str, format, ap);
#endif

	va_end(ap);

	return ret;
}
