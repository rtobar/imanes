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

#include "common.h"
#include "clock.h"
#include "cpu.h"
#include "debug.h"
#include "i18n.h"
#include "imaconfig.h"
#include "playback.h"
#include "queue.h"
#include <time.h>

static SDL_AudioSpec audio_spec;

/* These are the integer form of the normalized outputs
 * for square and tnd tables. We are using Uint8 here as
 * we are asking the sound card a fomat of AUDIO_U8. We also
 * assume that this is the returned format after SDL_OpenAudio,
 * and calculate these values according to that format.
 */
static Uint8 square_dac_outputs[32];
static Uint8 tnd_dac_outputs[204];

void initialize_playback() {

	int i;
	int max_vol;
	SDL_AudioSpec desired;

	if( config.sound_mute )
		return;

	/* Initial parameters for the SDL Audio subsytem */
	desired.freq     = 22050;
	desired.format   = AUDIO_U8;
	desired.channels = 1;
	desired.samples  = 1024;
	desired.callback = playback_fill_sound_card;
	desired.userdata = (void *)NULL;

	/* If we're using pulseaudio underneath, this would be a good idea */
	setenv("PULSE_PROP_application.name", "ImaNES", 1);
	setenv("PULSE_PROP_media.role", "game", 1);

	/* Inform if we cannot initialize the audio subsystem */
	if( SDL_OpenAudio(&desired, &audio_spec) == -1 ) {
		fprintf(stderr,_("Cannot initialize audio: %s\n"), SDL_GetError());
		return;
	}

	INFO( printf("Started audio: %d Hz, %d %s, %d-sample-sized buffer\n",
	       audio_spec.freq,
	       audio_spec.channels,
	       audio_spec.channels == 1 ? "channel" : "channels",
	       audio_spec.samples) );

	/* Calculate our own final DAC outputs from the normalized floating ones.
	 * We do this to avoid floating point calculations during ImaNES loop */
	max_vol = 0xFF;
	for(i=0;i!=32;i++)
		square_dac_outputs[i] = normal_square_dac_outputs[i]*max_vol;
	for(i=0;i!=204;i++)
		tnd_dac_outputs[i] = normal_tnd_dac_outputs[i]*max_vol;

}

void playback_fill_sound_card(void *userdata, Uint8 *stream, int len) {

	static int i = 0;
	static struct timespec currentTime;
	static struct timespec previousTime;

	i++;

	clock_gettime(CLOCK_REALTIME, &currentTime);
	if( currentTime.tv_sec != previousTime.tv_sec ) {
		printf("Called the callback %d times per second\n", i);
		i = 0;
	}
	previousTime.tv_sec = currentTime.tv_sec;

	/* Fill with silence at the start */
	memset(stream, audio_spec.silence, len);

	/* See the latest DAC outputs and combine them as long as we need */
}

void playback_pause(int pause_on) {
	if( config.sound_mute )
		return;
	SDL_PauseAudio(pause_on);
}

void end_playback() {
	if( config.sound_mute )
		return;
	playback_pause(1);
}
