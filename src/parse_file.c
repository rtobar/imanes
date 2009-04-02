/*  ImaNES: I'm a NES. An intelligent NES emulator

    parse_file.c   -    iNES file parsing for ImaNES

    Copyright (C) 2008   Rodrigo Tobar Carrizo

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _MSC_VER
#include <io.h>
#include <share.h>
#else
#include <unistd.h>
#endif

#include "common.h"
#include "cpu.h"
#include "parse_file.h"
#include "ppu.h"
#include "mapper.h"

ines_file *check_ines_file(char *file_path) {

	int i;
	int read_bytes;
	char *buff;
	struct stat stat_buf;
	ines_file *rom_file;

	/* Error handling */
	if( stat(file_path,&stat_buf) ) {
		buff = (char *)malloc(strlen(file_path) + 14);
#ifdef _MSC_VER
		sprintf_s(buff,strlen(file_path) + 14,"Couldn't open %s",file_path);
#else
		sprintf(buff,"Couldn't open %s",file_path);
#endif
		perror((const char *)buff);
		exit(EXIT_FAILURE);
	}

	rom_file = (ines_file *)malloc(sizeof(ines_file));
#ifdef _MSC_VER
	if( _sopen_s(&(rom_file->fd), file_path, O_RDONLY|_O_BINARY, _SH_DENYWR, _S_IREAD|_S_IWRITE) == -1 ) {
#else
	if( (rom_file->fd = open(file_path, O_RDONLY)) == -1 ) {
#endif
		buff = (char *)malloc(strlen(file_path) + 14);
#ifdef _MSC_VER
		sprintf_s(buff,strlen(file_path) + 14,"Couldn't open %s",file_path);
#else
		sprintf(buff,"Couldn't open %s",file_path);
#endif
		perror((const char *)buff);
		exit(EXIT_FAILURE);
	}

	/* Read the iNES magic bytes */
	buff = (char *)malloc(4);
#ifdef _MSC_VER
	read_bytes = _read(rom_file->fd, buff, 4);
#else
	read_bytes = read(rom_file->fd, buff, 4);
#endif

	printf("%c%c%c%d. Read bytes: %d\n", buff[0], buff[1], buff[2],buff[3], read_bytes);
	if( strncmp(buff,"NES\032",4) || read_bytes != 4 ) {
		printf("FD is %d\n", rom_file->fd);
		fprintf(stderr,"Error: %s is not a valid NES ROM\n",file_path);
		exit(EXIT_FAILURE);
	}

	/* ROM and VROM blocks */
#ifdef _MSC_VER
	read_bytes = _read(rom_file->fd, &(rom_file->romBanks), 1);
#else
	read_bytes = read(rom_file->fd, &(rom_file->romBanks), 1);
#endif
	if( read_bytes != 1 ) {
		fprintf(stderr,"Error: %s is not a valid NES ROM\n",file_path);
		exit(EXIT_FAILURE);
	}

#ifdef _MSC_VER
	read_bytes = _read(rom_file->fd, &(rom_file->vromBanks), 1);
#else
	read_bytes = read(rom_file->fd, &(rom_file->vromBanks), 1);
#endif
	if( read_bytes != 1 ) {
		fprintf(stderr,"Error: %s is not a valid NES ROM\n",file_path);
		exit(EXIT_FAILURE);
	}

	printf("File contains %hu 16kb ROM banks and %hu 8kb VROM banks\n",
          rom_file->romBanks, rom_file->vromBanks);

	/* Mapper, name table mirroring and others */
	buff = realloc(buff,2);
#ifdef _MSC_VER
	read_bytes = _read(rom_file->fd, buff, 2);
#else
	read_bytes = read(rom_file->fd, buff, 2);
#endif

	if( read_bytes != 2 ) {
		fprintf(stderr,"Error: %s is not a valid NES ROM\n",file_path);
		exit(EXIT_FAILURE);
	}

	/* Vert/Horiz mirroring */
	PPU->mirroring = buff[0] & 0x1;

	/* Four-screen mirroring */
	if( buff[0] & 0x08 )
		PPU->mirroring = FOUR_SCREEN_MIRRORING;

	printf("Mirroring type: %d\n", PPU->mirroring);

	CPU->sram_enabled = (buff[0] & 0x02) >> 1;
	printf("SRAM is %s\n", (CPU->sram_enabled ? "enabled" : "disabled") );
	rom_file->has_trainer  = buff[0] & 0x04;

	rom_file->mapper_id = (buff[1] & 0xF0) | ( (buff[0] >> 4) & 0x0F );

	/* Check which mappers we do support */
	mapper = NULL;
	for(i=0; mapper_list[i].id != -1; i++) {
		if( rom_file->mapper_id == mapper_list[i].id ) {
			mapper = mapper_list+i;
			mapper->file = rom_file;
			printf("ROM mapper is '%s'\n",mapper->name);
		}
	}

	if( mapper == NULL ) {
		fprintf(stderr,"Sorry, but we currently support cartridges using the mapper %d\n",rom_file->mapper_id);
		fprintf(stderr,"I'm exiting now.\n\n");
		exit(EXIT_FAILURE);
	}

	mapper->initialize_mapper();

	/* The rest of the header is ignored until now... */
	buff = realloc(buff,8);
#ifdef _MSC_VER
	read_bytes = _read(rom_file->fd, buff, 8);
#else
	read_bytes = read(rom_file->fd, buff, 8);
#endif
	if( read_bytes != 8 ) {
		fprintf(stderr,"Error: %s is not a valid NES ROM\n",file_path);
		exit(EXIT_FAILURE);
	}

	if( rom_file->has_trainer ) {

		printf("Trainer present in ROM file\n");
#ifdef _MSC_VER
		read_bytes = _read( rom_file->fd, buff, 512);
#else
		read_bytes = read( rom_file->fd, buff, 512);
#endif
		if( read_bytes != 512 ) {
			fprintf(stderr,"Error: %s is not a valid NES ROM\n",file_path);
			exit(EXIT_FAILURE);
		}
	}

	free(buff);

	return rom_file;
}

void map_rom_memory(ines_file *nes_rom) {

	int read_bytes;

	nes_rom->rom  = (uint8_t *)malloc(nes_rom->romBanks * ROM_BANK_SIZE);
	nes_rom->vrom = (uint8_t *)malloc(nes_rom->vromBanks*VROM_BANK_SIZE);

	/* Read and check */
#ifdef _MSC_VER
	read_bytes = _read(nes_rom->fd, (void *)nes_rom->rom ,
#else
	read_bytes = read(nes_rom->fd, (void *)nes_rom->rom ,
#endif
	                  nes_rom->romBanks * ROM_BANK_SIZE);
	if( read_bytes != nes_rom->romBanks * ROM_BANK_SIZE ) {
		fprintf(stderr,"Error: malformed file (ROM not complete)\n");
#ifdef _MSC_VER
		_close(nes_rom->fd);
#else
		close(nes_rom->fd);
#endif
		exit(EXIT_FAILURE);
	}

#ifdef _MSC_VER
	read_bytes = _read(nes_rom->fd, (void *)nes_rom->vrom,
#else
	read_bytes = read(nes_rom->fd, (void *)nes_rom->vrom,
#endif
	                  nes_rom->vromBanks*VROM_BANK_SIZE);
	if( read_bytes != nes_rom->vromBanks*VROM_BANK_SIZE ) {
		fprintf(stderr,"Error: malformed file (VROM not complete)\n");
#ifdef _MSC_VER
		_close(nes_rom->fd);
#else
		close(nes_rom->fd);
#endif
		exit(EXIT_FAILURE);
	}
	
#ifdef _MSC_VER
	_close(nes_rom->fd);
#else
	close(nes_rom->fd);
#endif
}

void free_ines_file(ines_file *file) {

	free(file);

}
