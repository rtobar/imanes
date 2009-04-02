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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#endif

#include "imaconfig.h"

imanes_config config;

void initialize_configuration() {

	/* Emulator window */
	config.show_bg  = 1;
	config.show_front_spr = 1;
	config.show_back_spr = 1;
	config.show_screen_bg = 1;

	/* Start on non-pause and at 60 fps */
	config.pause = 0;
	config.run_fast = 0;

	/* Set default video scale factor */
	if( config.video_scale == 0 )
		config.video_scale = 1;

	/* Use our color construction */
	config.use_sdl_colors = 0;
}

void load_user_configuration() {

	int tmp;
	char * user_home;
	char * user_imanes_dir;
	struct stat s;
#ifdef _WIN32
	size_t size;
#endif

#ifdef _WIN32
	_dupenv_s(&user_home, &size, "APPDATA");
	printf("Data directory is %s\n", user_home);
#else
	user_home = getenv("HOME");
#endif

	if( user_home == NULL ) {
		fprintf(stderr, "Couldn't find user's home directory. Will not load user configuration\n");
		return;
	}

	user_imanes_dir = (char *)malloc(strlen(user_home) + 9);
#ifdef _WIN32
	sprintf_s(user_imanes_dir,strlen(user_home)+9,"%s/Imanes/",user_home);
#else
	sprintf(user_imanes_dir,"%s/.imanes/", user_home);
#endif

	/* Check if the directory exists */
	tmp = stat(user_imanes_dir, &s);

	/* Not found, let's create it (we assume that $HOME exists) */
	if( tmp == -1 ) {
		fprintf(stderr,"Directory '%s' not found, creating it...\n", user_imanes_dir);
#ifdef _WIN32
		tmp = _mkdir(user_imanes_dir);
#else
		tmp = mkdir(user_imanes_dir, S_IRWXU | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
#endif

		if( tmp == -1 ) {
			fprintf(stderr,"Error while creating directory '%s': ", user_imanes_dir);
			perror(NULL);
			return;
		}
	}

}
