#ifndef config_h
#define config_h

#include <stdint.h>

typedef struct _config {
	uint8_t show_front_spr;      /* Should we display front sprites? */
	uint8_t show_back_spr;       /* Should we display back sprites? */
	uint8_t show_bg;             /* Should we display the background */
	uint8_t pause;               /* Pause emulation */

	int video_scale;             /* Video scale factor */
	int verbosity;               /* How verbose imanes should be */
	int run_fast;                /* Run as fast as possible */
} imanes_config;

/* This is the global configuration */
extern imanes_config config;

/* Initializes imanes configuration */
void initialize_configuration();

#endif /* config_h */
