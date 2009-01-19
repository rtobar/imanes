#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "common.h"
#include "parse_file.h"
#include "mapper.h"

ines_file *check_ines_file(char *file_path) {

	char *buff;
	struct stat stat_buf;
	ines_file *rom_file;

	/* Error handling */
	if( stat(file_path,&stat_buf) ) {
		buff = (char *)malloc(strlen(file_path) + 14);
		sprintf(buff,"Couldn't open %s",file_path);
		perror((const char *)buff);
		exit(EXIT_FAILURE);
	}

	rom_file = (ines_file *)malloc(sizeof(ines_file));
	if( (rom_file->fd = open(file_path, O_RDONLY)) == -1 ) {
		buff = (char *)malloc(strlen(file_path) + 14);
		sprintf(buff,"Couldn't open %s",file_path);
		perror((const char *)buff);
		exit(EXIT_FAILURE);
	}

	/* Read the iNES magic bytes */
	buff = (char *)malloc(4);
	read(rom_file->fd, buff, 4);
	if( strncmp(buff,"NES\032",4) ) {
		fprintf(stderr,"Error: %s is not a valid NES ROM\n",file_path);
		exit(EXIT_FAILURE);
	}

	/* ROM and VROM blocks */
	read(rom_file->fd, &(rom_file->romBanks), 1);
	read(rom_file->fd, &(rom_file->vromBanks), 1);
	printf("File contains %hu 16kb ROM banks and %hu 8kb VROM banks\n",
          rom_file->romBanks, rom_file->vromBanks);

	/* Mapper and other stuff */
	buff = realloc(buff,2);
	read(rom_file->fd, buff, 2);
	rom_file->mapper_id = (buff[1] & 0xFF00) | ( (buff[0] >> 4) & 0xFFFF );
	printf("File mapper is '%s'\n",mapper_list[rom_file->mapper_id]);

	/* The rest of the header is ignored until now... */
	buff = realloc(buff,8);
	read(rom_file->fd, buff, 8);
	free(buff);

	return rom_file;
}

void map_rom_memory(ines_file *nes_rom) {

	int read_bytes;

	nes_rom->rom  = malloc(nes_rom->romBanks * ROM_BANK_SIZE);
	nes_rom->vrom = malloc(nes_rom->vromBanks*VROM_BANK_SIZE);

	/* Read and check */
	read_bytes = read(nes_rom->fd, nes_rom->rom ,
	                  nes_rom->romBanks * ROM_BANK_SIZE);
	if( read_bytes != nes_rom->romBanks * ROM_BANK_SIZE ) {
		fprintf(stderr,"Error: malformed file (ROM not complete)\n");
		close(nes_rom->fd);
		exit(EXIT_FAILURE);
	}

	read_bytes = read(nes_rom->fd, nes_rom->vrom,
	                  nes_rom->vromBanks*VROM_BANK_SIZE);
	if( read_bytes != nes_rom->vromBanks*VROM_BANK_SIZE ) {
		fprintf(stderr,"Error: malformed file (VROM not complete)\n");
		close(nes_rom->fd);
		exit(EXIT_FAILURE);
	}
	
	close(nes_rom->fd);
}

void free_ines_file(ines_file *file) {

	free(file->rom);
	free(file->vrom);
	free(file);

}
