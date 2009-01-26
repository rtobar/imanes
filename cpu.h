#ifndef cpu_h
#define cpu_h

#include <stdint.h>

#include "common.h"
#include "instruction_set.h"

typedef struct {
	uint16_t PC;  /* Program counter. 16 bits long, but points 8 bit code */
	uint8_t  A;   /* Accumulator */
	uint8_t  X;   /* X register */
	uint8_t  Y;   /* Y register */
	uint8_t  SR;  /* Status register */
	uint8_t  SP;  /* Stack pointer */
	long long cycles; /* How many cycles has it given */

	/* Memory */
	uint8_t *RAM;
} nes_cpu;

/** SR flags */
#define N_FLAG 0x80
#define V_FLAG 0x40
//ignored      0x20
#define B_FLAG 0x10
#define D_FLAG 0x08
#define I_FLAG 0x04
#define Z_FLAG 0x02
#define C_FLAG 0x01

extern nes_cpu *CPU;

/**
 * Initialize the CPU with its registers
 */
void initialize_cpu();

/**
 * Dumps the current contents of the CPU to the stdout
 */
void dump_cpu();

/**
 * Given an operand, and an instruction, it executes it into the CPU
 */
void execute_instruction(instruction inst, operand oper); 

#endif /* cpu_h */
