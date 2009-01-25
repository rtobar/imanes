#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "cpu.h"
#include "loop.h"
#include "ppu.h"

void main_loop(ines_file *file) {
	
	uint8_t opcode;
	uint16_t address;
	uint8_t *inst_address;
	uint16_t operand = 0;
	instruction inst;

	/* 1 ROM bank games load twice to ensure vector tables */
	/* Free the file ROM (we don't need it anymore) */
	if( file->romBanks == 1 ) {
		memcpy( CPU->RAM + 0x8000, file->rom, 0x4000);
		memcpy( CPU->RAM + 0xC000, CPU->RAM + 0x8000, 0x4000);
	}
	/* 2 ROM bank games load one in 0x8000 and other in 0xC000 */
	/* Free the file ROM (we don't need it anymore) */
	else if (file->romBanks == 2 ) {
		memcpy( CPU->RAM + 0x8000, file->rom, 0x4000);
		memcpy( CPU->RAM + 0xC000, file->rom + 0x4000, 0x4000);
	}

	/* Dump the VROM into the PPU VRAM area */
	if( file->romBanks == 1 ) {
		memcpy( PPU->VRAM , file->vrom, 0x2000);
	}

	/* We first need to check out where the game begins... */
	/* For that, we see the address localted at the RESET vector */
	/* Remember that NES CPU is little endian */
	CPU->PC = *(CPU->RAM + 0xFFFC) | ( *(CPU->RAM + 0xFFFD) << 8 );

	/* This is the main loop */
	while(1) {

		/* inst_address saves the current process' memory
		 * pointer for the NES CPU' PC. This way we can
		 * debug easier the value of NES PC */
		inst_address = CPU->RAM + CPU->PC;

		/* Check if we are at the end of the ROM :O */
		if( inst_address == file->rom + file->romBanks * 16*1024 ) {
			fprintf(stderr,"Oops, we've reached the end of the instructions\n");
			fprintf(stderr,"Weird, hah?\n");
			exit(EXIT_FAILURE);
		}

		/* Read opcode and full instruction :) */
		opcode = *(inst_address);
		inst = instructions[opcode];

		printf("CPU->PC: 0x%04x - %02x: %s", CPU->PC, opcode, inst.name);
		/* Undocumented instruction */
		if( inst.size == 0 ) {
			fprintf(stderr,"\n\nUndocumented instruction: %02x\n",opcode);
			fprintf(stderr,"I'm exiting now... sorry :(\n");
			exit(EXIT_FAILURE);
		}

		/* Select operand depending on the addressing node */
		switch( inst.addr_mode ) {

			case ADDR_IMMEDIATE:
				operand = *(inst_address + 1);
				break;

			case ADDR_ABSOLUTE:
				address = *(inst_address + 1) | (*(inst_address + 2)  << 8);
				operand = *(CPU->RAM + address);
				break;

			case ADDR_ZEROPAGE:
				address = *(inst_address + 1);
				operand = *(CPU->RAM + address);
				break;

			case ADDR_IMPLIED:
				break;

			case ADDR_INDIRECT:
				break;

			case ADDR_ABS_INDX:
				operand = ( *(inst_address + 1) | (*(inst_address + 2) << 8) ) + CPU->X;
				break;

			case ADDR_ABS_INDY:
				operand = ( *(inst_address + 1) | (*(inst_address + 2) << 8) ) + CPU->Y;
				break;

			case ADDR_ZERO_INDX:
			case ADDR_ZERO_INDY:
			case ADDR_IND_INDIR:
			case ADDR_INDIR_IND:
			case ADDR_RELATIVE:
				operand = *(inst_address + 1);
				break;

		}

		printf(" operand: %04x\n", operand);
		/* Execute the given instruction */
		execute_instruction(inst,operand);
		
		CPU->PC += inst.size;
		CPU->cycles += inst.cycles;

		/* Draw the screen */
		draw_screen();

		sleep(1);
	}

}
