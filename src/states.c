/*  ImaNES: I'm a NES. An intelligent NES emulator

    states.c   -    Internal states of emulation for ImaNES

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

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "clock.h"
#include "cpu.h"
#include "debug.h"
#include "imaconfig.h"
#include "mapper.h"
#include "platform.h"
#include "ppu.h"
#include "states.h"

static void *state;
static int last_save = -1;

void load_state(int i) {

	int fd;
	char *ss_dir;
	char *ss_file;
	char *tmp;
	char *buffer;
	unsigned int total_size;
	RW_RET read_bytes;

	/* This is the total size of the state */
	total_size = 
	/* CPU registers*/  7 +
	/* RAM dump */      0x0800 + 0xBFDF +
	/* PPU registers */ 12 + 2*sizeof(unsigned int) + sizeof(float) +
	/* VRAM dump */     0x4000 +
	/* SPR-RAM dump */  0x100 +
	/* CLK */           sizeof(unsigned int) + sizeof(unsigned long) +
	/* Mapper */        1 + sizeof(unsigned int) + mapper->reg_count;

	/* If we are loading the last state that we saved,
	 * we don't need to go and read the state file */
	if( last_save == config.current_state )
		buffer = state;
	else {
		/* Read the state from the corresponding file */
		ss_dir = get_imanes_dir(States);
		tmp = get_filename(config.rom_file);
		ss_file = (char *)malloc(strlen(ss_dir) + strlen(tmp) + 2 + 7);
		imanes_sprintf(ss_file,strlen(ss_dir)+strlen(tmp)+2+7,"%s%c%s-%02d.sta", ss_dir, DIR_SEP, tmp, config.current_state);

		IMANES_OPEN(fd, ss_file, IMANES_OPEN_READ);
		free(ss_dir);
		free(tmp);

		if( fd == -1 && errno == ENOENT) {
			fprintf(stderr,"Cannot load state %d because it doesn't exist\n", config.current_state);
			free(ss_file);
			return;
		}
		else if( fd == -1 ) {
			fprintf(stderr,"Error while opening '%s': ", ss_file);
			perror(NULL);
			free(ss_file);
			return;
		}
		buffer = (char *)malloc(total_size);
		read_bytes = IMANES_READ(fd, buffer, total_size);
		IMANES_CLOSE(fd);

		if( read_bytes != total_size ) {
			fprintf(stderr,"File '%s' is not a valid state file\n", ss_file);
			free(ss_file);
			return;
		}
		free(ss_file);
	}

	/* CPU dumping */
	memcpy(&(CPU->A),      buffer, 1); buffer++;
	memcpy(&(CPU->X),      buffer, 1); buffer++;
	memcpy(&(CPU->Y),      buffer, 1); buffer++;
	memcpy(&(CPU->SP),     buffer, 1); buffer++;
	memcpy(&(CPU->SR),     buffer, 1); buffer++;
	memcpy(&(CPU->PC),     buffer, 2); buffer += 2;

	/* RAM dumping */
	memcpy(CPU->RAM, buffer, 0x0800);
	buffer += 0x0800;
	memcpy(CPU->RAM + 0x4020, buffer, 0xBFDF);
	buffer += 0xBFDF;

	/* PPU dumping */
	memcpy(&(PPU->CR1), buffer, 1);       buffer++;
	memcpy(&(PPU->CR2), buffer, 1);       buffer++;
	memcpy(&(PPU->SR), buffer, 1);        buffer++;
	memcpy(&(PPU->mirroring), buffer, 1); buffer++;
	memcpy(&(PPU->x), buffer, 1);         buffer++;
	memcpy(&(PPU->latch), buffer, 1);     buffer++;
	memcpy(&(PPU->vram_addr), buffer, 2); buffer += 2;
	memcpy(&(PPU->temp_addr), buffer, 2); buffer += 2;
	memcpy(&(PPU->spr_addr), buffer, 2);  buffer += 2;
	memcpy(&(PPU->scanline_timeout), buffer, sizeof(float));
	buffer += sizeof(float);
	memcpy(&(PPU->lines), buffer, sizeof(unsigned int));
	buffer += sizeof(unsigned int);
	memcpy(&(PPU->frames), buffer, sizeof(unsigned int));
	buffer += sizeof(unsigned int);

	/* VRAM dumping */
	memcpy(PPU->VRAM, buffer, 0x4000);
	buffer += 0x4000;

	/* SPR-RAM dumping */
	memcpy(PPU->SPR_RAM, buffer, 0x100);
	buffer += 0x100;

	/* CLK dumping */
	memcpy(&(CLK->ppu_cycles), buffer, sizeof(unsigned long));
	buffer += sizeof(unsigned long);
	memcpy(&(CLK->nmi_pcycles), buffer, sizeof(unsigned int));
	buffer += sizeof(unsigned int);

	/* Mapper dumping */
	memcpy(&(mapper->id), buffer, 1);  buffer++;
	memcpy(&(mapper->reg_count), buffer, sizeof(unsigned int));
	buffer += sizeof(unsigned int);
	memcpy(mapper->regs, buffer, mapper->reg_count);

	mapper->reset();
	mapper->switch_banks();

	INFO( printf("Loaded state %d\n", config.current_state) );

	return;
}

void save_state(int i) {

	int fd;
	char *ss_dir;
	char *ss_file;
	char *tmp;
	char *buffer;
	unsigned int total_size;
	RW_RET written;

	ss_dir = get_imanes_dir(States);
	if( ss_dir == NULL ) {
		fprintf(stderr,"Couldn't save state: cannot reach states dir\n");
		return;
	}

	/* This is the total size of the state */
	total_size = 
	/* CPU registers*/  7 +
	/* RAM dump */      0x0800 + 0xBFDF +
	/* PPU registers */ 12 + 2*sizeof(unsigned int) + sizeof(float) +
	/* VRAM dump */     0x4000 +
	/* SPR-RAM dump */  0x100 +
	/* CLK */           sizeof(unsigned int) + sizeof(unsigned long) +
	/* Mapper */        1 + sizeof(unsigned int) + mapper->reg_count;

	/* Memory allocation for state information */
	buffer = (char *)malloc(total_size);

	/* CPU dumping */
	memcpy(buffer, &(CPU->A),  1); buffer++;
	memcpy(buffer, &(CPU->X),  1); buffer++;
	memcpy(buffer, &(CPU->Y),  1); buffer++;
	memcpy(buffer, &(CPU->SP), 1); buffer++;
	memcpy(buffer, &(CPU->SR), 1); buffer++;
	memcpy(buffer, &(CPU->PC), 2); buffer += 2;

	/* RAM dumping */
	/* We only need to dump the following sections:
	 *
	 * 0x0000 - 0x07FF
	 * 0x4020 - 0xFFFF
	 *
	 * Everything else is I/O mapped regiters or mirroring
	 */
	memcpy(buffer, CPU->RAM, 0x0800);
	buffer += 0x0800;
	memcpy(buffer, CPU->RAM + 0x4020, 0xBFDF);
	buffer += 0xBFDF;

	/* PPU dumping */
	memcpy(buffer, &(PPU->CR1), 1);       buffer++;
	memcpy(buffer, &(PPU->CR2), 1);       buffer++;
	memcpy(buffer, &(PPU->SR), 1);        buffer++;
	memcpy(buffer, &(PPU->mirroring), 1); buffer++;
	memcpy(buffer, &(PPU->x), 1);         buffer++;
	memcpy(buffer, &(PPU->latch), 1);     buffer++;
	memcpy(buffer, &(PPU->vram_addr), 2); buffer += 2;
	memcpy(buffer, &(PPU->temp_addr), 2); buffer += 2;
	memcpy(buffer, &(PPU->spr_addr), 2);  buffer += 2;
	memcpy(buffer, &(PPU->scanline_timeout), sizeof(float));
	buffer += sizeof(float);
	memcpy(buffer, &(PPU->lines), sizeof(unsigned int));
	buffer += sizeof(unsigned int);
	memcpy(buffer, &(PPU->frames), sizeof(unsigned int));
	buffer += sizeof(unsigned int);

	/* VRAM dumping */
	memcpy(buffer, PPU->VRAM, 0x4000);
	buffer += 0x4000;

	/* SPR-RAM dumping */
	memcpy(buffer, PPU->SPR_RAM, 0x100);
	buffer += 0x100;

	/* CLK dumping */
	memcpy(buffer, &(CLK->ppu_cycles), sizeof(unsigned long));
	buffer += sizeof(unsigned long);
	memcpy(buffer, &(CLK->nmi_pcycles), sizeof(unsigned int));
	buffer += sizeof(unsigned int);

	/* Mapper dumping */
	memcpy(buffer, &(mapper->id), 1);  buffer++;
	memcpy(buffer, &(mapper->reg_count), sizeof(unsigned int));
	buffer += sizeof(unsigned int);
	memcpy(buffer, mapper->regs, mapper->reg_count);
	buffer += mapper->reg_count;

	/* Get the pointer back to where it should be */
	buffer -= total_size;

	/* Finally, save it into a file */
	tmp = get_filename(config.rom_file);
	ss_file = (char *)malloc(strlen(ss_dir) + strlen(tmp) + 2 + 7);
	imanes_sprintf(ss_file,strlen(ss_dir)+strlen(tmp)+2+7,"%s%c%s-%02d.sta", ss_dir, DIR_SEP, tmp, config.current_state);

	IMANES_OPEN(fd,ss_file, IMANES_OPEN_WRITE);
	free(ss_dir);
	free(tmp);
	free(ss_file);

	written = IMANES_WRITE(fd, (void *)buffer, total_size);

	if( written != total_size )
		perror("Error while saving state to file");

	IMANES_CLOSE(fd);

	/* Copy the state into the a buffer, so we don't need to
	 * read the state file if we want to load the last saved state */
	state = realloc(state, total_size);
	memcpy(state, buffer, total_size);
	last_save = config.current_state;

	free(buffer);
	INFO( printf("Saved state %d\n", config.current_state) );

	return;
}
