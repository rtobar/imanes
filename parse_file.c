#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "common.h"
#include "parse_file.h"

void check_ines_file(char *file_path) {

	char *buff;
	struct stat stat_buf;
	ines_file rom_file;

	/* Error handling */
	if( stat(file_path,&stat_buf) ) {
		buff = (char *)malloc(strlen(file_path) + 14);
		sprintf(buff,"Couldn't open %s",file_path);
		perror((const char *)buff);
		exit(EXIT_FAILURE);
	}

	if( (rom_file.fd = open(file_path, O_RDONLY)) == -1 ) {
		buff = (char *)malloc(strlen(file_path) + 14);
		sprintf(buff,"Couldn't open %s",file_path);
		perror((const char *)buff);
		exit(EXIT_FAILURE);
	}

	/* Read the iNES magic bytes */
	buff = (char *)malloc(4);
	read(rom_file.fd, buff, 4);
	if( strncmp(buff,"NES\032",4) ) {
		fprintf(stderr,"Error: %s is not a valid NES ROM\n",file_path);
		exit(EXIT_FAILURE);
	}

	/* ROM and VROM blocks */
	read(rom_file.fd, &(rom_file.romBanks), 1);
	read(rom_file.fd, &(rom_file.vromBanks), 1);
	printf("File contains %hu 16kb ROM banks and %hu 8kb VROM banks\n",
          rom_file.romBanks, rom_file.vromBanks);

	/* Mapper and other stuff */
	buff = realloc(buff,2);
	read(rom_file.fd, buff, 2);
	rom_file.mapper_id = (buff[1] & 0xFF00) | ( (buff[0] >> 4) & 0xFFFF );
	printf("File mapper ID is %hu\n",rom_file.mapper_id);
}
