/*  ImaNES: I'm a NES. An intelligent NES emulator

    playback.c   -    Sound playback implementation for ImaNES

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

#include "i18n.h"
#include "playback.h"

static SDL_AudioSpec audio_spec;
static int audio_initialized;

void initialize_playback() {

	SDL_AudioSpec desired;

	/* Initialize the SDL Audio subsytem */
	desired.freq     = 22050;
	desired.format   = AUDIO_U8;
	desired.channels = 1;
	desired.silence  = 0;    /* Calculated? */
	desired.samples  = 1024;
	desired.size     = 0;    /* Calculated? */
	desired.callback = playback_fill_sound_buffer;
	desired.userdata = (void *)NULL;

	/* Inform if we cannot initialize the audio subsystem */
	if( SDL_OpenAudio(&desired, &audio_spec) == -1 ) {
		fprintf(stderr,_("Cannot initialize audio: %s\n"), SDL_GetError());
		audio_initialized = 0;
	}
	audio_initialized = 1;

}

void playback_fill_sound_buffer(void *userdata, Uint8 *stream, int len) {

	printf("Entering to the callback function :)\n");
}

void playback_pause(int pause_on) {
	SDL_PauseAudio(pause_on);
}

void end_playback() {
	playback_pause(1);
}
