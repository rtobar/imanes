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

#include <time.h>

#include "apu.h"
#include "common.h"
#include "clock.h"
#include "cpu.h"
#include "debug.h"
#include "i18n.h"
#include "imaconfig.h"
#include "playback.h"
#include "queue.h"

static dac_queue *dac[5];

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

	/* DAC queues */
	dac[0] = NULL;
	dac[1] = NULL;
	dac[2] = NULL;
	dac[3] = NULL;
	dac[4] = NULL;

}

/* TODO: only processing triangle and square (both) channels here! */
void playback_fill_sound_card(void *userdata, Uint8 *stream, int len) {

	/* static variables to keep history of things */
	static int i = 0;
	static struct timespec previousTime = {0, 0};
	static unsigned long int previous_ppu_cycles = 0;

	Uint8 sample;
	int pos;
	unsigned int channel;
	unsigned long int ppu_cycles;
	unsigned long int ppu_steps_per_sample;
	unsigned long int step_ppu_cycles;
	struct timespec currentTime;

	uint8_t square1_sample;
	uint8_t square2_sample;
	uint8_t triangle_sample;

	ppu_cycles = CLK->ppu_cycles;
	i++;

	clock_gettime(CLOCK_REALTIME, &currentTime);
	if( currentTime.tv_sec != previousTime.tv_sec ) {
		printf("Called the callback %d times per second\n", i);
		i = 0;
	}

	/* Fill with silence at the start */
	memset(stream, audio_spec.silence, len);

	ppu_steps_per_sample = (ppu_cycles - previous_ppu_cycles)/len;

	/* See the latest DAC outputs and combine them as long as we need */
	step_ppu_cycles = previous_ppu_cycles;
	for(pos=0; pos!=len; pos++) {

		/* Check which sounds should be checked
		 * For this, we check that the front of every queue is
		 * the one that should get played, and then we take all the fronts,
		 * and combine them
		 */
		for(channel = 0; channel != 3; channel++) {
			while(dac[channel] != NULL && dac[channel]->ppu_cycles < step_ppu_cycles)
				dac[channel] = pop(dac[channel]);
		}

		sample = 0;
		square1_sample = 0;
		square2_sample = 0;
		triangle_sample = 0;

		if( dac[Square1] != NULL )
			square1_sample = dac[Square1]->sample;
		if( dac[Square2] != NULL )
			square2_sample = dac[Square2]->sample;
		if( dac[Triangle] != NULL )
			triangle_sample = dac[Triangle]->sample;

		sample += square_dac_outputs[square1_sample + square2_sample];
		sample += tnd_dac_outputs[triangle_sample];

		/* Finally! This is our little sample */
		stream[pos] = sample;

		step_ppu_cycles += ppu_steps_per_sample;
	}

	/* Finally, set the 'previous' variables */
	previousTime.tv_sec = currentTime.tv_sec;
	previousTime.tv_nsec = currentTime.tv_nsec;
	previous_ppu_cycles = ppu_cycles;
}

void playback_pause(int pause_on) {
	if( config.sound_mute )
		return;
	SDL_PauseAudio(pause_on);
}

void playback_add_sample(int channel, uint8_t sample) {
	SDL_LockAudio();
	dac[channel] = push(dac[channel], sample);
	SDL_UnlockAudio();
}

void end_playback() {
	if( config.sound_mute )
		return;
	playback_pause(1);
}
