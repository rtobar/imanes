#ifndef common_h
#define common_h

#include <stdint.h>

/* ImaNES information */
#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define PACKAGE_BUGREPORT "https://csrg.inf.utfsm.cl/flyspray/index.php?project=10"
#define PACKAGE_NAME      "ImaNES"
#define PACKAGE_VERSION   "0.1"
#define PACKAGE_STRING    "ImaNES 0.1"
#endif

#define IMANES_VERSION PACKAGE_VERSION

/* RAM sizes */
#define NES_RAM_SIZE     (0x10000)
#define NES_VRAM_SIZE    (0x4000)
#define NES_SPR_RAM_SIZE (0x100)

/* ROM sizes */
#define ROM_BANK_SIZE   (16*1024)
#define VROM_BANK_SIZE  ( 8*1024)

#define NES_PALETTE_COLORS (64)

#define CYCLES_PER_SCANLINE ((float)(341/3.))

typedef struct _ines_file {
	int fd;
	uint8_t romBanks;
	uint8_t vromBanks;
	uint8_t mapper_id;
	uint8_t *rom;
	uint8_t *vrom;

	int has_trainer;
} ines_file;

/* For a given file, and given its full path, get only the file name
 * _without_ extension */
char *get_filename(char *full_pathname);

/* Convert an instruction's name to lowercase */
void inst_lowercase(char *inst_name, char *ret);

#endif /* common_h */
