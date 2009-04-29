#ifndef imaconfig_h
#define imaconfig_h

#include <stdint.h>

typedef struct _config {
	uint8_t show_front_spr;      /* Should we display front sprites? */
	uint8_t show_back_spr;       /* Should we display back sprites? */
	uint8_t show_bg;             /* Should we display the background */
	uint8_t show_screen_bg;      /* Background color behind everything */
	uint8_t pause;               /* Pause emulation */

	/* State-related */
	uint8_t current_state;       /* State to be loaded/saved */
	uint8_t save_state;          /* Flag to save our current state */
	uint8_t load_state;          /* Flag to load a state */

	int video_scale;             /* Video scale factor */
	int verbosity;               /* How verbose imanes should be */
	int run_fast;                /* Run as fast as possible */
	int use_sdl_colors;          /* Let SDL convert RGB values */

	int take_screenshot;         /* Should we take a screenshot? */

	char *rom_file;             /* Name of the rom file */
} imanes_config;

typedef enum _imanes_dir {
	States,    /* To save internal states of ImaNES*/
	Saves,     /* To save ROM's SRAM */
	Snapshots  /* To save snapshots taken from ImaNES */
} imanes_dir;

/* This is the global configuration */
extern imanes_config config;

/* Initializes imanes configuration */
void initialize_configuration();

/* Loads per-user configuration */
void load_user_configuration();

/* Checks the existence (and creates) of per-user imanes config directory
 * and returns its name */
char *get_user_imanes_dir();

/** Checks and returns internal config directories under the per-user
 * imanes config directory */
char *get_imanes_dir(imanes_dir dir);

#endif /* imaconfig_h */
