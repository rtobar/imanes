#ifndef cpu_h
#define cpu_h

#include <stdint.h>

typedef struct {
	uint8_t A;
	uint8_t X;
	uint8_t Y;
	uint8_t S;
	uint8_t P;
} nes_cpu;

extern nes_cpu *CPU;

/**
 * Initialize the CPU with its registers
 */
void initialize_cpu();

/**
 * Dumps the current contents of the CPU to the stdout
 */
void dump_CPU();

#endif /* cpu_h */
