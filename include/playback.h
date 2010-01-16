#ifndef playback_h
#define playback_h

#include <SDL/SDL.h>

#include "apu.h"

/**
 * Initialize the sound playback on the system
 */
void initialize_playback();

/**
 * Callback function for filling the audio card with data (SDL specific)
 */
void playback_fill_sound_card(void *userdata, Uint8 *stream, int len);


/**
 * Function used from the APU to fill the buffer that will go to the sound card
 */
void playback_fill_sound_buffer(uint8_t sample, nes_apu_channel channel);

/**
 * Pauses/resumes the playback of audio
 */
void playback_pause(int pause_on);

/**
 * Finishes the sound playback
 */
void end_playback();

#endif /* playback_h */
