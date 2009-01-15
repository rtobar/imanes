#ifndef common_h
#define common_h

#include <stdint.h>

typedef struct {
	int fd;
	uint8_t romBanks;
	uint8_t vromBanks;
} ines_file;

#endif /* common_h */
