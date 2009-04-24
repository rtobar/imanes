#ifndef sram_h
#define sram_h

/* Function used to save the actual contents of the SRAM
 * to a persistent file on disk
 */
void save_sram(char * file);

/* Function used to get the contents of the SRAM from a
 * file on disk. Returns the file where the contents are saves
 */
char * load_sram(char * rom_file);

#endif /* sram_h */
