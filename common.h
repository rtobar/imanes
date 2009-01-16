#ifndef common_h
#define common_h

#include <stdint.h>

#define TOBYNES_VERSION 0.1

#define ROM_BANK_SIZE   (16*1024)
#define VROM_BANK_SIZE  ( 8*1024)

typedef struct {
	int fd;
	uint8_t romBanks;
	uint8_t vromBanks;
	uint8_t mapper_id;
	void *rom;
	void *vrom;
} ines_file;

#endif /* common_h */
