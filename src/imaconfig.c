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

#include "debug.h"
#include "i18n.h"
#include "imaconfig.h"
#include "platform.h"

imanes_config config;

void initialize_configuration() {

	char *dummy;

	/* Emulator window */
	config.show_bg  = 1;
	config.show_front_spr = 1;
	config.show_back_spr = 1;
	config.show_screen_bg = 1;
	config.show_fps = 1;

	/* APU channels */
	config.apu_triangle = 1;
	config.apu_square1 = 1;
	config.apu_square2 = 1;
	config.apu_noise = 1;
	config.apu_dmc = 1;
	config.sound_mute = 0;

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

	config.take_screenshot = 0;

	/* Create all directories if necessary */
	dummy = get_imanes_dir(States);    free(dummy);
	dummy = get_imanes_dir(Saves);     free(dummy);
	dummy = get_imanes_dir(Snapshots); free(dummy);
}

int config_enabled(const char *value) {

	if( !strcmp("1", value) ||
	    !strcmp("enabled", value) ||
	    !strcmp("True", value) ||
	    !strcmp("true", value) )
		return 1;
	return 0;
}

void load_user_configuration() {

	unsigned int line_count;
	char *user_dir;
	char *config_file;
	char line[1024];
	char key[100];
	char value[100];
	FILE *file = NULL;

	user_dir = get_user_imanes_dir();

	if( user_dir == NULL ) {
		fprintf(stderr,_("Couldn't find user's configuration\n"));
		return;
	}

	config_file = (char *)malloc(strlen(user_dir) + 11);
	imanes_sprintf(config_file, strlen(user_dir) + 11, "%s%cimanes.rc", user_dir, DIR_SEP);
	file = fopen(config_file, "rt");

	if( file == NULL ) {
		fprintf(stderr,_("Error while opening configuration file '%s': "), config_file);
		perror(NULL);
		fprintf(stderr,_("No configuration will be loaded from configuration file\n"));
		free(config_file);
		free(user_dir);
		return;
	}

	line_count = 1;
	while( !feof(file) && fgets(line, 1024, file) != NULL ) {

		/* Skip empty lines and comments */
		if( strlen(line) == 0 )
			continue;
		if( line[0] == '#' )
			continue;

		/* Check that lines are valid */
		if( sscanf(line, "%s = %s", key, value) != 2 ) {
			fprintf(stderr, "Invalid configuration at line %d: %s", line_count, line);
		}

		/* Actually configure imanes */

		/* Square1 channel */
		if( !strcmp("square1", key) ) {
			if( config_enabled(value) )
				config.apu_square1 = 1;
			else
				config.apu_square1 = 0;
			INFO( printf("[config] Square 1 audio channel %s\n", (config.apu_square1 ? "enabled" : "disabled")) );
			continue;
		}
		/* Square2 channel */
		if( !strcmp("square2", key) ) {
			if( config_enabled(value) )
				config.apu_square2 = 1;
			else
				config.apu_square2 = 0;
			INFO( printf("[config] Square 2 audio channel %s\n", (config.apu_square2 ? "enabled" : "disabled")) );
			continue;
		}
		/* Triangle channel */
		if( !strcmp("triangle", key) ) {
			if( config_enabled(value) )
				config.apu_triangle = 1;
			else
				config.apu_triangle = 0;
			INFO( printf("[config] Triangle audio channel %s\n", (config.apu_triangle ? "enabled" : "disabled")) );
			continue;
		}
		/* Noise channel */
		if( !strcmp("noise", key) ) {
			if( config_enabled(value) )
				config.apu_noise = 1;
			else
				config.apu_noise = 0;
			INFO( printf("[config] Noise audio channel %s\n", (config.apu_noise ? "enabled" : "disabled")) );
			continue;
		}
		/* DMC channel */
		if( !strcmp("dmc", key) ) {
			if( config_enabled(value) )
				config.apu_dmc = 1;
			else
				config.apu_dmc = 0;
			INFO( printf("[config] DMC audio channel %s\n", (config.apu_dmc ? "enabled" : "disabled")) );
			continue;
		}

		line_count++;
	}

	free(user_dir);
	free(config_file);
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
		fprintf(stderr,_("Directory '%s' not found, creating it...\n"), dir);
		tmp = IMANES_MKDIR(dir);

		if( tmp == -1 ) {
			fprintf(stderr,_("Error while creating directory '%s': "), dir);
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

	_dupenv_s(&user_home, &size, "APPDATA");
#else
	user_home = getenv("HOME");
#endif

	if( user_home == NULL ) {
		fprintf(stderr, _("Couldn't find user's home directory. Will not load user configuration\n"));
		return NULL;
	}

	user_imanes_dir = (char *)malloc(strlen(user_home) + strlen(IMANES_USER_DIR) + 2);
	imanes_sprintf(user_imanes_dir,
	               strlen(user_home) + strlen(IMANES_USER_DIR) + 2,
	               "%s%c%s",user_home, DIR_SEP,IMANES_USER_DIR);

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
			imanes_sprintf(specific_dir,strlen(user_imanes_dir)+8,"%s%cstates",user_imanes_dir, DIR_SEP);
			if( check_and_create(specific_dir) ) {
				free(specific_dir);
				free(user_imanes_dir);
				return NULL;
			}
			break;

		case Saves:
			specific_dir = (char *)malloc(strlen(user_imanes_dir) + 7);
			imanes_sprintf(specific_dir,strlen(user_imanes_dir)+7,"%s%csaves",user_imanes_dir, DIR_SEP);
			if( check_and_create(specific_dir) ) {
				free(specific_dir);
				free(user_imanes_dir);
				return NULL;
			}
			break;

		case Snapshots:
			specific_dir = (char *)malloc(strlen(user_imanes_dir) + 13);
			imanes_sprintf(specific_dir,strlen(user_imanes_dir)+13,"%s%cscreenshots",user_imanes_dir, DIR_SEP);
			if( check_and_create(specific_dir) ) {
				free(specific_dir);
				free(user_imanes_dir);
				return NULL;
			}
			break;
	}

	free(user_imanes_dir);
	return specific_dir;
}
