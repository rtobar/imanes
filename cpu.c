#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"

nes_cpu *CPU;

void initialize_cpu() {
	CPU = (nes_cpu *)malloc(sizeof(nes_cpu));

	/* Initialize all the instructions/addressing modes */
}

void dump_cpu() {

	printf("CPU Dumping:\n============\n\n");
	printf("PC: %04x\n", (unsigned int)CPU->PC);
	printf("A:  %02x\n", CPU->A);
	printf("X:  %02x\n", CPU->X);
	printf("Y:  %02x\n", CPU->Y);
	printf("SR: %02x\n", CPU->SR);
	printf("SP: %02x\n", CPU->SP);

	return;
}
