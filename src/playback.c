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
#include "i18n.h"
#include "playback.h"

uint8_t **sdl_audio_buffer;
static SDL_AudioSpec audio_spec;
static int audio_initialized;
Uint8 square_dac_outputs[32];
Uint8 tnd_dac_outputs[32];

void initialize_playback() {

	int i;
	int max_vol;
	SDL_AudioSpec desired;

	audio_initialized = 0;

	/* Initialize the SDL Audio subsytem */
	desired.freq     = 22050;
	desired.format   = AUDIO_U8;
	desired.channels = 1;
	desired.silence  = 0;    /* Calculated? */
	desired.samples  = 1024;
	desired.size     = 0;    /* Calculated? */
	desired.callback = playback_fill_sound_card;
	desired.userdata = (void *)NULL;

	/* Inform if we cannot initialize the audio subsystem */
	if( SDL_OpenAudio(&desired, &audio_spec) == -1 ) {
		fprintf(stderr,_("Cannot initialize audio: %s\n"), SDL_GetError());
		return;
	}

	/* sdl_audio_buffer = (uint8_t *)malloc(audio_spec.size*2); */
	sdl_audio_buffer = (uint8_t **)malloc(5*sizeof(uint8_t *));
	for(i=0;i!=5;i++)
		sdl_audio_buffer[i] = (uint8_t *)malloc(CYCLES_PER_SCANLINE*262*60/3);

	if( sdl_audio_buffer == NULL ) {
		fprintf(stderr,_("Cannot initialize audio: cannot allocate memory for audio buffer\n"));
		return;
	}

	audio_initialized = 1;

	/* Calculate our own final DAC outputs from the normalized
	 * We do this to avoid floating point calculations during ImaNES */
	/* TODO: This max_vol is fixed assuming 8-bits sound, we should check the audio_spec */
	max_vol = 0xFF;
	for(i=0;i!=32;i++)
		square_dac_outputs[i] = normal_square_dac_outputs[i]*max_vol;
	for(i=0;i!=204;i++)
		tnd_dac_outputs[i] = normal_tnd_dac_outputs[i]*max_vol;

}

void playback_fill_sound_card(void *userdata, Uint8 *stream, int len) {

	/* Fill with silence at the start */
	memset(stream, audio_spec.silence, len);

}

void playback_fill_sound_buffer(uint8_t sample, nes_apu_channel channel) {

	int index = CLK->nmi_pcycles/3;

	if( channel == Square1 )
		sdl_audio_buffer[index][0] = (Uint8)sample;
	if( channel == Square2 )
		sdl_audio_buffer[index][1] = sample;
	if( channel == Triangle )
		sdl_audio_buffer[index][2] = sample;
	if( channel == DMC )
		sdl_audio_buffer[index][3] = sample;
	if( channel == Noise )
		sdl_audio_buffer[index][4] = sample;

}

void playback_pause(int pause_on) {
	SDL_PauseAudio(pause_on);
}

void end_playback() {
	playback_pause(1);
}
