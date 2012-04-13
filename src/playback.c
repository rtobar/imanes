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

void initialize_playback() {

	int i;
	SDL_AudioSpec desired;

	if( config.sound_mute )
		return;

	/* Initial parameters for the SDL Audio subsytem */
	desired.freq     = 44100;
	/*desired.freq     = 1787;*/
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
		config.sound_mute = 1;
		return;
	}

	INFO( printf("Started audio: %d Hz, %d %s, %d-sample-sized buffer\n",
	       audio_spec.freq,
	       audio_spec.channels,
	       audio_spec.channels == 1 ? "channel" : "channels",
	       audio_spec.samples) );

	if( audio_spec.format != AUDIO_U8 ) {
		fprintf(stderr,_("Unsupported audio format: %d, no audio will be played\n"), audio_spec.format);
		config.sound_mute = 1;
		SDL_PauseAudio(1);
		return;
	}

	INFO(
		printf("Square outputs:");
		for(i=0;i!=32;i++)
			printf(" %u", square_dac_outputs[i]);
		printf("\nTriangle, DMC, Noise outputs:");
		for(i=0;i!=204;i++)
			printf(" %u", tnd_dac_outputs[i]);
		printf("\n");
	);

	/* DAC queues */
	dac[0] = NULL;
	dac[1] = NULL;
	dac[2] = NULL;
	dac[3] = NULL;
	dac[4] = NULL;

}

void playback_fill_sound_card(void *userdata, Uint8 *stream, int len) {

	/* static variables to keep history across several invocations */
	static struct timespec previousTime = {0, 0};
	static unsigned int calls_per_sec = 0;
	static unsigned long int previous_ppu_cycles = 0;
	static Uint8 last_square1_sample = 0;
	static Uint8 last_square2_sample = 0;
	static Uint8 last_triangle_sample = 0;
	static Uint8 last_noise_sample = 0;
	static Uint8 last_dmc_sample = 0;

	Uint8 sample;
	int pos;
	unsigned int channel;
	unsigned int removed;
	unsigned int length;
	unsigned long int ppu_cycles;
	unsigned long int ppu_steps_per_sample;
	unsigned long int step_ppu_cycles;
	struct timespec currentTime;

	uint8_t square1_sample;
	uint8_t square2_sample;
	uint8_t triangle_sample;
	uint8_t noise_sample;
	uint8_t dmc_sample;

	ppu_cycles = CLK->ppu_cycles;

	INFO(

		clock_gettime(CLOCK_REALTIME, &currentTime);
		calls_per_sec++;

		if( currentTime.tv_sec != previousTime.tv_sec ) {

			printf("Calls/s: %u, Len is %d. Elements in queue:", calls_per_sec, len);

			length = queue_length(dac[Square1]);
			if( length != 0 )
				printf(" Square1: %d", length);

			length = queue_length(dac[Square2]);
			if( length != 0 )
				printf(" Square2: %d", length);

			length = queue_length(dac[Triangle]);
			if( length != 0 )
				printf(" Triangle: %d", length);

			length = queue_length(dac[Noise]);
			if( length != 0 )
				printf(" Noise: %d", length);

			length = queue_length(dac[DMC]);
			if( length != 0 )
				printf(" DMC: %d", length);

			printf("\n");

			calls_per_sec = 0;
		}
	);

	ppu_steps_per_sample = (ppu_cycles - previous_ppu_cycles)/len;

	/* See the latest DAC outputs and combine them as long as we need */
	step_ppu_cycles = previous_ppu_cycles + ppu_steps_per_sample;

	/* printf("%lu cycles have passed, we'll play them in %u steps\n", ppu_cycles - previous_ppu_cycles, len); */
	for(pos=0; pos!=len; pos++) {

		/* Check which sample should be played in this step for each channel */
		for(channel = 0; channel != 5; channel++) {

			removed = 0;
			while(1) {

				/* If there are no samples, do nothing */
				if( dac[channel] == NULL || dac[channel]->next == NULL )
					break;

				/* If there is a next sample, check whether we should play that one
				 * instead of the one placed in the head of the queue. If we're ok,
				 * we finish checking */
				if( dac[channel]->next->ppu_cycles <= step_ppu_cycles ) {
					dac[channel] = pop(dac[channel]);
					removed++;
				}
				else
					break;

			}
			DEBUG(
				if( removed > 1 )
					printf("Removed %u samples from %u's channel DAC queue\n", removed, channel);
			);
		}

		/* Now combine the samples, if the corresponding channel is enabled */
		square1_sample  = last_square1_sample;
		square2_sample  = last_square2_sample;
		triangle_sample = last_triangle_sample;
		noise_sample    = last_noise_sample;
		dmc_sample      = last_dmc_sample;

		if( dac[Square1] != NULL && dac[Square1]->ppu_cycles <= step_ppu_cycles && config.apu_square1 )
			square1_sample = dac[Square1]->sample;
		if( dac[Square2] != NULL && dac[Square2]->ppu_cycles <= step_ppu_cycles && config.apu_square2 )
			square2_sample = dac[Square2]->sample;
		if( dac[Triangle] != NULL && dac[Triangle]->ppu_cycles <= step_ppu_cycles && config.apu_triangle )
			triangle_sample = dac[Triangle]->sample;
		if( dac[Noise] != NULL && dac[Noise]->ppu_cycles <= step_ppu_cycles && config.apu_noise )
			noise_sample = dac[Noise]->sample;
		if( dac[DMC] != NULL && dac[DMC]->ppu_cycles <= step_ppu_cycles && config.apu_dmc )
			dmc_sample = dac[DMC]->sample;

		sample  = square_dac_outputs[square1_sample + square2_sample];
		sample += tnd_dac_outputs[3*triangle_sample + 2*noise_sample + dmc_sample];

		/* If we got anything, then we should play silence (or not?) */
		if( !sample )
			sample = audio_spec.silence;

		/* Finally! This is our little sample */
		stream[pos] = sample;

		last_square1_sample  = square1_sample;
		last_square2_sample  = square2_sample;
		last_triangle_sample = triangle_sample;
		last_noise_sample    = noise_sample;
		last_dmc_sample      = dmc_sample;

		/* Step into the next sample */
		step_ppu_cycles += ppu_steps_per_sample;
	}

	/* Finally, set the 'previous' variables */
	INFO(
		previousTime.tv_sec = currentTime.tv_sec;
		previousTime.tv_nsec = currentTime.tv_nsec;
	);

	previous_ppu_cycles = ppu_cycles;
}

void playback_pause(int pause_on) {
	if( config.sound_mute )
		return;
	SDL_PauseAudio(pause_on);
}

void playback_add_sample(int channel, uint8_t sample) {

	if( config.sound_mute )
		return;

	if( (channel == Triangle && !config.apu_triangle) ||
	    (channel == Square1  && !config.apu_square1) ||
	    (channel == Square2  && !config.apu_square2) ||
	    (channel == Noise    && !config.apu_noise) ||
	    (channel == DMC      && !config.apu_dmc) )
		return;

	SDL_LockAudio();
	dac[channel] = push(dac[channel], sample);
	SDL_UnlockAudio();
}

void end_playback() {

	int i;
	if( config.sound_mute )
		return;

	playback_pause(1);
	for(i=0; i!=5; i++)
		while(dac[i]!=NULL)
			dac[i] = pop(dac[i]);
}
