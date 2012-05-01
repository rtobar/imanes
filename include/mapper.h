#ifndef mapper_h
#define mapper_h

#include <stdint.h>

#include "common.h"

#define MAX_MAPPER_NAME_SIZE 100

/** Common structure for all mappers */
typedef struct _mapper {

	int id;                             /* Mapper id */
	char name[MAX_MAPPER_NAME_SIZE];    /* Mapper name */
	unsigned int reg_count;             /* Number of registers */

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

	/* Frees the resources used by the mapper */
	void (*end_mapper)();

	/* Registers */
	uint8_t *regs;

	/* Associated nes file pointer */
	ines_file *file;

} nes_mapper;

/* Dumps the contents of the internal mapper registers into stdout */
void dump_mapper();

/* Mapper list from http://fms.komkon.org/EMUL8/NES.html */
extern nes_mapper mapper_list[];

/* Global current mapper variable */
extern nes_mapper *mapper;

#define SWAP_RAM( ram_start, prg_start, size ) \
	memcpy(CPU->RAM + ram_start, prg_start, size)

#define SWAP_RAM_8K( start, bank ) \
	SWAP_RAM( start, mapper->file->rom + (bank) * 0x2000, 0x2000 )

#define SWAP_RAM_16K( start, bank ) \
	SWAP_RAM( start, mapper->file->rom + (bank) * 0x4000, 0x4000 )



#define SWAP_VRAM( vram_start, chr_start, size ) \
	memcpy(PPU->VRAM + vram_start, chr_start, size)

#define SWAP_VRAM_1K( address, bank ) \
	SWAP_VRAM(address, mapper->file->vrom + (bank) * 0x0400, 0x0400 )

#define SWAP_VRAM_2K( address, bank ) \
	SWAP_VRAM(address, mapper->file->vrom + (bank) * 0x0800, 0x0800 )

#endif /* mapper_h */
