/*  ImaNES: I'm a NES. An intelligent NES emulator

    parse_file.c   -    iNES file parsing for ImaNES

    Copyright (C) 2009   Rodrigo Tobar Carrizo

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

#include "common.h"
#include "cpu.h"
#include "debug.h"
#include "i18n.h"
#include "parse_file.h"
#include "platform.h"
#include "ppu.h"
#include "mapper.h"

ines_file *check_ines_file(const char *file_path) {

	int i;
	char *buff;
	struct stat stat_buf;
	ines_file *rom_file;
	RW_RET read_bytes;

	/* Error handling */
	if( stat(file_path,&stat_buf) ) {
		buff = (char *)malloc(strlen(file_path) + 14);
		imanes_sprintf(buff,strlen(file_path) + 14,"Couldn't open %s",file_path);
		perror((const char *)buff);
		exit(EXIT_FAILURE);
	}

	rom_file = (ines_file *)malloc(sizeof(ines_file));
	IMANES_OPEN( rom_file->fd, file_path, IMANES_OPEN_READ);
	if( rom_file->fd == -1 ) {
		buff = (char *)malloc(strlen(file_path) + 14);
		imanes_sprintf(buff,strlen(file_path) + 14,"Couldn't open %s",file_path);
		perror((const char *)buff);
		exit(EXIT_FAILURE);
	}

	/* Read the iNES magic bytes */
	buff = (char *)malloc(4);
	read_bytes = IMANES_READ(rom_file->fd, buff, 4);

	if( strncmp(buff,"NES\032",4) || read_bytes != 4 ) {
		fprintf(stderr,_("Error: %s is not a valid NES ROM, incompatible header information\n"),file_path);
		exit(EXIT_FAILURE);
	}

	/* ROM and VROM blocks */
	read_bytes = IMANES_READ(rom_file->fd, &(rom_file->romBanks), 1);
	if( read_bytes != 1 ) {
		fprintf(stderr,_("Error: %s is not a valid NES ROM\n"),file_path);
		exit(EXIT_FAILURE);
	}

	read_bytes = IMANES_READ(rom_file->fd, &(rom_file->vromBanks), 1);
	if( read_bytes != 1 ) {
		fprintf(stderr,_("Error: %s is not a valid NES ROM\n"),file_path);
		exit(EXIT_FAILURE);
	}

	INFO( printf(_("File contains %hu 16kb ROM banks and %hu 8kb VROM banks\n"),
          rom_file->romBanks, rom_file->vromBanks) );

	/* Mapper, name table mirroring and others */
	buff = realloc(buff,2);
	read_bytes = IMANES_READ(rom_file->fd, buff, 2);

	if( read_bytes != 2 ) {
		fprintf(stderr,_("Error: %s is not a valid NES ROM\n"),file_path);
		exit(EXIT_FAILURE);
	}

	/* Vert/Horiz mirroring */
	PPU->mirroring = buff[0] & 0x1;

	/* Four-screen mirroring */
	if( buff[0] & 0x08 )
		PPU->mirroring = FOUR_SCREEN_MIRRORING;

	INFO( printf(_("Mirroring type: %d\n"), PPU->mirroring) );

	CPU->sram_enabled = (buff[0] & 0x02) >> 1;
	INFO( printf(_("SRAM is %s\n"), (CPU->sram_enabled ? _("enabled") : _("disabled")) ) );
	rom_file->has_trainer  = buff[0] & 0x04;

	rom_file->mapper_id = (buff[1] & 0xF0) | ( (buff[0] >> 4) & 0x0F );

	/* Check which mappers we do support */
	mapper = NULL;
	for(i=0; mapper_list[i].id != -1; i++) {
		if( rom_file->mapper_id == mapper_list[i].id ) {
			mapper = mapper_list+i;
			mapper->file = rom_file;
			INFO( printf(_("ROM mapper is '%s'\n"),mapper->name) );
		}
	}

	if( mapper == NULL ) {
		fprintf(stderr,_("Sorry, but we currently support cartridges using the mapper %d\n"),rom_file->mapper_id);
		fprintf(stderr,_("I'm exiting now.\n\n"));
		exit(EXIT_FAILURE);
	}

	mapper->initialize_mapper();

	/* The rest of the header is ignored until now... */
	buff = realloc(buff,8);
	read_bytes = IMANES_READ(rom_file->fd, buff, 8);
	if( read_bytes != 8 ) {
		fprintf(stderr,_("Error: %s is not a valid NES ROM\n"),file_path);
		exit(EXIT_FAILURE);
	}

	if( rom_file->has_trainer ) {

		INFO( printf(_("Trainer present in ROM file\n")) );
		read_bytes = IMANES_READ( rom_file->fd, buff, 512);
		if( read_bytes != 512 ) {
			fprintf(stderr,_("Error: %s is not a valid NES ROM\n"),file_path);
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
	read_bytes = IMANES_READ(nes_rom->fd, (void *)nes_rom->rom ,
	                  nes_rom->romBanks * ROM_BANK_SIZE);
	if( read_bytes != nes_rom->romBanks * ROM_BANK_SIZE ) {
		fprintf(stderr,_("Error: malformed file (ROM not complete)\n"));
		IMANES_CLOSE(nes_rom->fd);
		exit(EXIT_FAILURE);
	}

	read_bytes = IMANES_READ(nes_rom->fd, (void *)nes_rom->vrom,
	                  nes_rom->vromBanks*VROM_BANK_SIZE);
	if( read_bytes != nes_rom->vromBanks*VROM_BANK_SIZE ) {
		fprintf(stderr,_("Error: malformed file (VROM not complete)\n"));
		IMANES_CLOSE(nes_rom->fd);
		exit(EXIT_FAILURE);
	}
	
	IMANES_CLOSE(nes_rom->fd);

}

void free_ines_file(ines_file *file) {

	if( file->rom != NULL )
		free(file->rom);
	if( file->vrom != NULL )
		free(file->vrom);

	free(file);

}
