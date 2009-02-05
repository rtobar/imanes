#ifndef config_h
#define config_h

#include <stdint.h>

typedef struct _config {
	uint8_t show_spr;    /* Should we display sprites? */
	uint8_t show_bg;     /* Should we display the background */
} imanes_config;

/* This is the global configuration */
extern imanes_config config;

/* Initializes imanes configuration */
void initialize_configuration();

#endif /* config_h */
