#ifndef imaconfig_h
#define imaconfig_h

#include <stdint.h>

typedef struct _config {

	/* PPU layers */
	uint8_t show_front_spr;      /* Toogle show front sprites */
	uint8_t show_back_spr;       /* Toogle show back sprites */
	uint8_t show_bg;             /* Toogle show background */
	uint8_t show_screen_bg;      /* Toogle show background color */

	/* Emulation status */
	uint8_t show_fps;            /* Toogle show frames per second */
	uint8_t pause;               /* Toogle pause emulation */

	/* APU channels */
	uint8_t apu_triangle;        /* Toogle Triangle channel */
	uint8_t apu_square1;         /* Toogle 1st square channel */
	uint8_t apu_square2;         /* Toogle 2nd square channel */
	uint8_t apu_noise;           /* Toogle noise channel */
	uint8_t apu_dmc;             /* Toogle delta modulation channel */

	/* State-related */
	uint8_t current_state;       /* State to be loaded/saved */
	uint8_t save_state;          /* Flag to save our current state */
	uint8_t load_state;          /* Flag to load a state */

	/* Others */
	int video_scale;             /* Video scale factor */
	int verbosity;               /* How verbose imanes should be */
	int run_fast;                /* Run as fast as possible */
	int use_sdl_colors;          /* Let SDL convert RGB values */
	int sound_mute;              /* Do not output any sound */

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
