/*  ImaNES: I'm a NES. An intelligent NES emulator

    config.c   -    ImaNES configuration handling

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _MSC_VER
#include <direct.h>
#endif

#include "debug.h"
#include "imaconfig.h"

imanes_config config;

void initialize_configuration() {

	char *dummy;

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

	/* Start with the state 0, but don't load nor save it */
	config.current_state = 0;
	config.save_state = 0;
	config.load_state = 0;

	/* Create all directories if necessary */
	dummy = get_imanes_dir(States);    free(dummy);
	dummy = get_imanes_dir(Saves);     free(dummy);
	dummy = get_imanes_dir(Snapshots); free(dummy);
}

void load_user_configuration() {

	get_user_imanes_dir();

	return;
}

/* Internal method to check and create a directory.
 * It returns 0 when returns gracefully, -1 otherwise */
int check_and_create(char *dir) {

	int tmp;
	struct stat s;

	/* Check if the directory exists */
	tmp = stat(dir, &s);

	/* Not found, let's create it (we assume that $HOME exists) */
	if( tmp == -1 ) {
		fprintf(stderr,"Directory '%s' not found, creating it...\n", dir);
#ifdef _MSC_VER
		tmp = _mkdir(dir);
#else
		tmp = mkdir(dir, S_IRWXU | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
#endif

		if( tmp == -1 ) {
			fprintf(stderr,"Error while creating directory '%s': ", dir);
			perror(NULL);
			return -1;
		}
	}

	return 0;
}

char *get_user_imanes_dir() {

	char * user_home;
	char * user_imanes_dir;
#ifdef _MSC_VER
	size_t size;
#endif

#ifdef _MSC_VER
	_dupenv_s(&user_home, &size, "APPDATA");
	INFO( printf("Data directory is %s\n", user_home) );
#else
	user_home = getenv("HOME");
#endif

	if( user_home == NULL ) {
		fprintf(stderr, "Couldn't find user's home directory. Will not load user configuration\n");
		return NULL;
	}

	user_imanes_dir = (char *)malloc(strlen(user_home) + 9);
#ifdef _MSC_VER
	sprintf_s(user_imanes_dir,strlen(user_home)+9,"%s/Imanes",user_home);
#else
	sprintf(user_imanes_dir,"%s/.imanes", user_home);
#endif

	if( check_and_create(user_imanes_dir) )
		return NULL;

	return user_imanes_dir;
}

char *get_imanes_dir(imanes_dir dir) {

	char *user_imanes_dir;
	char *specific_dir;

	user_imanes_dir = get_user_imanes_dir();
	specific_dir = NULL;

	if( user_imanes_dir == NULL )
		return NULL;

	switch(dir) {

		case States:
			specific_dir = (char *)malloc(strlen(user_imanes_dir) + 8);
#ifdef _MSC_VER
			sprintf_s(user_imanes_dir,strlen(user_home)+8,"%s/states",user_home);
#else
			sprintf(specific_dir, "%s/states", user_imanes_dir);
#endif
			if( check_and_create(specific_dir) ) {
				free(specific_dir);
				free(user_imanes_dir);
				return NULL;
			}
			break;

		case Saves:
			specific_dir = (char *)malloc(strlen(user_imanes_dir) + 7);
#ifdef _MSC_VER
			sprintf_s(user_imanes_dir,strlen(user_home)+7,"%s/saves",user_home);
#else
			sprintf(specific_dir, "%s/saves", user_imanes_dir);
#endif
			if( check_and_create(specific_dir) ) {
				free(specific_dir);
				free(user_imanes_dir);
				return NULL;
			}
			break;

		case Snapshots:
			specific_dir = (char *)malloc(strlen(user_imanes_dir) + 11);
#ifdef _MSC_VER
			sprintf_s(user_imanes_dir,strlen(user_home)+11,"%s/snapshots",user_home);
#else
			sprintf(specific_dir, "%s/snapshots", user_imanes_dir);
#endif
			if( check_and_create(specific_dir) ) {
				free(specific_dir);
				free(user_imanes_dir);
				return NULL;
			}
			break;
	}

	return specific_dir;
}
