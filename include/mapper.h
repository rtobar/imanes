#ifndef mapper_h
#define mapper_h

#include <stdint.h>

#include "common.h"

#define MAX_MAPPER_NAME_SIZE 100

/** Common structure for all mappers */
typedef struct _mapper {

	int id;                             /* Mapper id */
	char name[MAX_MAPPER_NAME_SIZE];    /* Mapper name */

	/* Initializes the mapper */
	void (*initialize_mapper)();

	/* Checks the written memory, and saves the data to the internal  */
	int  (*check_address)(uint16_t address);

	/* Performs the bank switchings */
	void (*switch_banks)();

	/* Performs the system memory fill at reset or at initialization */
	void (*reset)();

	/* Updates internal registers at the end of each scanline */
	void (*update)();

	/* Registers */
	uint8_t *regs;

	/* Associated nes file pointer */
	ines_file *file;

} nes_mapper;

/* Mapper list from http://fms.komkon.org/EMUL8/NES.html */
extern nes_mapper mapper_list[];

/* Global current mapper variable */
nes_mapper *mapper;

#endif /* mapper_h */
