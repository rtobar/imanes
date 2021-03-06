#ifndef cpu_h
#define cpu_h

#include <stdint.h>

#include "common.h"
#include "instruction_set.h"

/* SRAM has a enabled/disabled state plus a RO/RW status */
#define SRAM_ENABLE 0x01
#define SRAM_RO     0x02

typedef struct _cpu {

	/* Internal CPU registers */
	uint16_t PC;  /* Program counter. 16 bits long, but points 8 bit code */
	uint8_t  A;   /* Accumulator */
	uint8_t  X;   /* X register */
	uint8_t  Y;   /* Y register */
	uint8_t  SR;  /* Status register */
	uint8_t  SP;  /* Stack pointer */

	/* Memory */
	uint8_t *RAM;

	/* Reset button pressed */
	uint8_t reset;

	/* Enable SRAM storage */
	uint8_t sram_enabled;

} nes_cpu;

extern nes_cpu *CPU;

/** SR flags */
#define N_FLAG 0x80
#define V_FLAG 0x40
#define R_FLAG 0x20 /* It doesn't even physically exist */
#define B_FLAG 0x10 /* It doesn't physically exist either, but it is used */
#define D_FLAG 0x08
#define I_FLAG 0x04
#define Z_FLAG 0x02
#define C_FLAG 0x01

#define BEGIN_STACK (0x100)
#define STACK_SIZE  (0x100)

/**
 * Initialize the CPU with its registers
 */
void initialize_cpu();

/**
 * Dumps the current contents of the CPU to the stdout
 */
void dump_cpu();

/**
 * Dumps the current contents of the CPU stack
 */
void dump_stack();

/**
 * Dumps the current contents of a given RAM address,
 * until lenght bytes are dumped.
 */
void dump_ram(uint16_t address, unsigned int lenght);

/**
 * Given an operand, and an instruction, it executes it into the CPU
 */
void execute_instruction(instruction *inst, operand *oper);

/**
 * Read from a RAM address and return the value there. This function
 * handles all the mirroring at RAM level, as well as the memory mapped
 * IOs at the emulation level
 */
uint8_t read_cpu_ram(uint16_t address);

/**
 * Writw to RAM at the given address. This function
 * handles all the mirroring at RAM level, as well as the memory mapped
 * IOs at the emulation level
 */
void write_cpu_ram(uint16_t address, uint8_t value);

/**
 * Pushes the given value into the CPU's stack
 */
void stack_push(uint8_t value);

/**
 * Pulls a value from the CPU's stack
 */
uint8_t stack_pull();

/**
 * Executes an NMI interrupt
 */
void execute_nmi();

/**
 * Executes a reset on the NES
 */
void execute_reset();

/**
 * Executes an IRQ/BRK interrupt on the NES
 */
void execute_irq();

/**
 * Add cycles to certain opcodes executions
 * when crossing page bounds
 */
void add_cycles(uint8_t type, int8_t value);

/**
 * Frees all the resources used by the CPU 
 */
void end_cpu();

#endif /* cpu_h */
