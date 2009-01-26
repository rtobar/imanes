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

#define BEGIN_STACK (0x100)
#define STACK_SIZE  (0x100)

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
 * Initializes CPU's RAM with contents from iNES file ROM
 */
void init_cpu_ram(ines_file *);

/**
 * Given an operand, and an instruction, it executes it into the CPU
 */
void execute_instruction(instruction inst, operand oper); 

/**
 * Given a value and a set of flags, check and update them if necessary
 */
void update_flags(int8_t value, uint8_t flags);

/**
 * For special memory location where IO is mapped, we update the
 * corresponding information when writing to it
 */
void check_write_mapped_io(uint16_t address);

/**
 * For special memory location where IO is mapped, we update the
 * corresponding information when reading it
 */
void check_read_mapped_io(uint16_t address);

#endif /* cpu_h */
