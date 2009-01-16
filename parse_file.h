#ifndef parse_file_h
#define parse_file_h

#include "common.h"

#define NES_FILE     0
#define NO_NES_FILE  1

/** 
 * This function does all the checking about the NES file that should be
 * emulated. First it stats it, and after that it checks if is indeed a
 * iNES rom. Finally, it gets the information about the ROM
 */
ines_file *check_ines_file(char *);

/**
 * Maps all the ROM and VROM banks to main memory of our process
 */
void map_rom_memory(ines_file *);

#endif /* parse_file_h */
