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
static uint8_t *normal_ppu_cycle_samples;

void initialize_playback() {

	int i;
	SDL_AudioSpec desired;

	if( config.sound_mute )
		return;

	/* Initial parameters for the SDL Audio subsytem */
	desired.freq     = 44100;
	desired.freq     = 22050;
	desired.format   = AUDIO_U8;
	desired.channels = 1;
	desired.samples  = 2048;
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

	INFO( printf("Started audio: %d Hz, %d %s, %d-sample-sized buffer. Silence is %d\n",
	       audio_spec.freq,
	       audio_spec.channels,
	       audio_spec.channels == 1 ? "channel" : "channels",
	       audio_spec.samples,
	       audio_spec.silence) );

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

	/* The array where we'll store the information about which samples
	 * should take extra PPU cycles on each callback iteration */
	normal_ppu_cycle_samples = (uint8_t *)malloc(audio_spec.samples);
	memset(normal_ppu_cycle_samples, 0, audio_spec.samples);

}

void playback_fill_sound_card(void *userdata, Uint8 *stream, int len) {

	/* static variables to keep history across several invocations */
	static struct timespec previousTime = {0, 0};
	static unsigned int calls_per_sec = 0;
	static unsigned long int previous_ppu_cycles = 0;
	static int previous_sound_rec = 0;
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
	unsigned long int elapsed_ppu_cycles;
	unsigned long int remained_ppu_cycles;
	unsigned long int step_ppu_cycles;
	unsigned long int previous_step_ppu_cycles;
	unsigned long int n_groups;
	unsigned long int division;
	unsigned long int modulo;
	unsigned long int initial_pos;
	unsigned long int i;
	struct timespec currentTime;

	uint8_t square1_sample;
	uint8_t square2_sample;
	uint8_t triangle_sample;
	uint8_t noise_sample;
	uint8_t dmc_sample;

	/* First of all, get the current PPU cycles. They will
	 * serve as an indication of the samples that we must
	 * process during this callback. */
	ppu_cycles = CLK->ppu_cycles;

	/* Some debugging information, proves useful from time to time */
	INFO(

		clock_gettime(CLOCK_REALTIME, &currentTime);
		calls_per_sec++;

		if( currentTime.tv_sec != previousTime.tv_sec ) {

			printf("Calls/s: %u, Len is %d\n", calls_per_sec, len);
			calls_per_sec = 0;

			DEBUG(
				printf("Elements in queue:");

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
			);

		}
	);

	/* Which is the number of elapsed PPU steps? With them we calculate
	 * how many cycles must be taken into account when constructing each
	 * sample */
	elapsed_ppu_cycles = (ppu_cycles - previous_ppu_cycles);
	ppu_steps_per_sample = elapsed_ppu_cycles/len;
	remained_ppu_cycles  = elapsed_ppu_cycles%len;

	/* The remainder steps are carefully scheduled to be considered not all
	 * into one single sample, but instead in a more distributed way. For
	 * this we create "groups" of consecutive samples that will take
	 * one of the remainder PPU steps into account.
	 *
	 * The total number of groups will be the numer of "empty" spaces + 1,
	 * thus spreading the remained PPU steps at their most.
	 *
	 * For this, we define the number and size of the "groups" (consecutive
	 * contributions of the remained to the samples) in the following way
	 *
	 *  _________________________
	 *  | N        | size       |
	 *  |----------+------------|
	 *  | modulo   | division+1 |
	 *  | n-modulo | division   |
	 *  |----------+------------|
	 *
	 */
	if( remained_ppu_cycles != 0 ) {
		if( remained_ppu_cycles == 1 )
			n_groups = 1;
		else
			n_groups = len - remained_ppu_cycles + 1;
		division = remained_ppu_cycles/n_groups;
		modulo   = remained_ppu_cycles%n_groups;

		/* We place first the (division+1) sized groups, with a silence after them */
		for(i=0; i!=modulo; i++)
			normal_ppu_cycle_samples[(i+1)*(division+2)-1] = 1;

		/* And now the (division) sized groups, which come after the (division+1) size groups.
		 * We don't process the last group, since it would yield
		 * a write passed the malloc'd area. */
		initial_pos = modulo*(division+2);
		for(i=0; i!=(n_groups - modulo); i++)
			normal_ppu_cycle_samples[initial_pos + (i+1)*(division+1) - 1] = 1;
	}

	/* Main loop where the buffer gets finally filled */
	step_ppu_cycles = previous_ppu_cycles;
	previous_step_ppu_cycles = previous_ppu_cycles;
	for(pos=0; pos!=len; pos++) {

		/* Last PPU to consider for this sample. Here we finally take
		 * into account all our previous calculations to see whether
		 * this sample should take an extra PPU cycles into account or
		 * not. */
		step_ppu_cycles += ppu_steps_per_sample;
		if( pos < remained_ppu_cycles )
			step_ppu_cycles++;

		/* Forget about nice grouping of remainder wave poits for the time being,
		   I need to focus first on the output rate of the samples
		if( !normal_ppu_cycle_samples[pos] )
			step_ppu_cycles++;
		else
			normal_ppu_cycle_samples[pos] = 0;
		*/

		/* Remove old samples from the queues */
		for(channel = 0; channel != 5; channel++) {
			removed = 0;
			while( dac[channel] != NULL && dac[channel]->ppu_cycles <= previous_step_ppu_cycles ) {
				dac[channel] = pop(dac[channel]);
				removed++;
			}
			INFO(
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

		/* Finally! This is our little sample */
		stream[pos] = sample;

		/* Reset for later use */
		last_square1_sample  = square1_sample;
		last_square2_sample  = square2_sample;
		last_triangle_sample = triangle_sample;
		last_noise_sample    = noise_sample;
		last_dmc_sample      = dmc_sample;

		previous_step_ppu_cycles = step_ppu_cycles;
	}

	/* Finally, set the 'previous' variables */
	INFO(
		previousTime.tv_sec = currentTime.tv_sec;
		previousTime.tv_nsec = currentTime.tv_nsec;
	);

	if( ppu_cycles != step_ppu_cycles )
		fprintf(stderr, "Mmmm, there's something wrong in here: PPU cycles: %lu, last step PPU cycles: %lu, previous PPU cycles: %lu. n_groups/division/modulo: %lu/%lu/%lu\n", ppu_cycles, step_ppu_cycles, previous_ppu_cycles, n_groups, division, modulo);
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
