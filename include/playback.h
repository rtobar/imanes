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
 * Adds a sample into the specified channel's DAC for later playback
 */
void playback_add_sample(int channel, uint8_t sample);

/**
 * Pauses/resumes the playback of audio
 */
void playback_pause(int pause_on);

/**
 * Finishes the sound playback
 */
void end_playback();

#endif /* playback_h */
