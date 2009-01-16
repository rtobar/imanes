#include <stdio.h>

#include "cpu.h"

nes_cpu *CPU;

void initialize_cpu() {
	CPU = (nes_cpu *)malloc(sizeof(nes_cpu));
}

void dump_cpu() {

	printf("CPU Dumping:\n============\n\n");
	printf("A: %02x\n", CPU->A);
	printf("X: %02x\n", CPU->X);
	printf("Y: %02x\n", CPU->Y);
	printf("S: %02x\n", CPU->S);
	printf("P: %02x\n", CPU->P);

	return;
}
