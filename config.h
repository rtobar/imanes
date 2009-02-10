#ifndef config_h
#define config_h

#include <stdint.h>

typedef struct _config {
	uint8_t show_spr;           	/* Should we display sprites? */
	uint8_t show_bg;            	/* Should we display the background */
	uint8_t show_pattern_tables;	/* Should we display PT in separate window */
	uint8_t show_name_tables;   	/* Should we display NT in separate window */
	uint8_t pause;              	/* Pause emulation */
} imanes_config;

/* This is the global configuration */
extern imanes_config config;

/* Initializes imanes configuration */
void initialize_configuration();

#endif /* config_h */
