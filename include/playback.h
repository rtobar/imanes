#ifndef playback_h
#define playback_h

#include <SDL/SDL.h>

/**
 * Initialize the sound playback on the system
 */
void initialize_playback();

/**
 * Callback function for filling the audio buffer (SDL specific)
 */
void playback_fill_sound_buffer(void *userdata, Uint8 *stream, int len);


/**
 * Pauses/resumes the playback of audio
 */
void playback_pause(int pause_on);

/**
 * Finishes the sound playback
 */
void end_playback();

#endif /* playback_h */
