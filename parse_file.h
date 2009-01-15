#ifndef parse_file_h
#define parse_file_h

#define NES_FILE     0
#define NO_NES_FILE  1

/** 
 * This function does all the checking about the NES file that should be
 * emulated. First it stats it, and after that it checks if is indeed a
 * iNES rom. Finally, it gets the information about the ROM
 */
void check_ines_file(char *);

#endif /* parse_file_h */
