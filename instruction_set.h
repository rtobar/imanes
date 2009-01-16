#ifndef instruction_set_h
#define instruction_set_h

#include <stdint.h>

/* 6502 instructions IDs */
#define INSTRUCTIONS_NUMBER 56

#define ADC 0
#define AND 1
#define ASL 2
#define BCC 3
#define BCS 4
#define BEQ 5
#define BIT 6
#define BMI 7
#define BNE 8
#define BPL 9
#define BRK 10
#define BVC 11
#define BVS 12
#define CLC 13
#define CLD 14
#define CLI 15
#define CLV 16
#define CMP 17
#define CPX 18
#define CPY 19
#define DEC 20
#define DEX 21
#define DEY 22
#define EOR 23
#define INC 24
#define INX 25
#define INY 26
#define JMP 27
#define JSR 28
#define LDA 29
#define LDX 30
#define LDY 31
#define LSR 32
#define NOP 33
#define ORA 34
#define PHA 35
#define PHP 36
#define PLA 37
#define PLP 38
#define ROL 39
#define ROR 40
#define RTI 41
#define RTS 42
#define SBC 43
#define SEC 44
#define SED 45
#define SEI 46
#define STA 47
#define STX 48
#define STY 49
#define TAX 50
#define TAY 51
#define TSX 52
#define TXA 53
#define TXS 54
#define TYA 55

/* Addressing modes */
#define ADDR_MODES_NUMBER 13

#define ADDR_IMMEDIATE 0  /* OPC #$BB    */
#define ADDR_ABSOLUTE  1  /* OPC $HHLL   */
#define ADDR_ZEROPAGE  2  /* OPC $BB     */
#define ADDR_IMPLIED   3  /* OPC         */
#define ADDR_INDIR_ABS 4  /* OPC $(HHLL) */
#define ADDR_ABS_INDX  5  /* OPC $HHLL,X */
#define ADDR_ABS_INDY  6  /* OPC $HHLL,Y */
#define ADDR_ZERO_INDX 7  /* OPC $LL,X   */
#define ADDR_ZERO_INDY 8  /* OPC $LL,Y   */
#define ADDR_IND_INDIR 9  /* OPC ($BB,X) */
#define ADDR_INDIR_IND 10 /* OPC ($LL),Y */
#define ADDR_ACCUM     11 /* OPC A       */
#define ADDR_RELATIVE  12 /* OPC $BB     */

/** These are differences on the normal cycle number of a instruction */
#define CYCLE_NORMAL  0
#define CYCLE_PAGE    1
#define CYCLE_BRANCH  2

typedef struct {
	uint8_t opcode;
	short size;
	short cycles;
	uint8_t cycle_change;
} _inst_per_addr;

#define SET_FLAG_NOMOD 0
#define SET_FLAG_MOD   1
#define SET_FLAG_1     2
#define SET_FLAG_0     3
#define SET_FLAG_M6    4
#define SET_FLAG_M7    5

typedef struct {
	int iid;                          /* One from above's instructions IDs */
	uint8_t flags_change[6];          /* Flags setting in the CPU->SR */
	_inst_per_addr modes[ADDR_MODES_NUMBER]; /* Addressing modes */
} instruction;

extern instruction *instructions;

#define SET_INSTRUCTION_DATA( INST, CHANGE_N, CHANGE_Z, CHANGE_C, \
                                    CHANGE_I, CHANGE_D, CHANGE_V ) \
   instructions[INST].iid = INST; \
   instructions[INST].flags_change[0] = SET_FLAG_##CHANGE_N; \
   instructions[INST].flags_change[1] = SET_FLAG_##CHANGE_Z; \
   instructions[INST].flags_change[2] = SET_FLAG_##CHANGE_C; \
   instructions[INST].flags_change[3] = SET_FLAG_##CHANGE_I; \
   instructions[INST].flags_change[4] = SET_FLAG_##CHANGE_D; \
   instructions[INST].flags_change[5] = SET_FLAG_##CHANGE_V;

#define SET_INSTRUCTION_ADDR_DATA( INST, ADDR_MODE, OPCODE, SIZE, CYCLES, \
                                   CHANGE ) \
   instructions[INST].modes[ADDR_##ADDR_MODE].opcode = OPCODE; \
   instructions[INST].modes[ADDR_##ADDR_MODE].size   = SIZE;   \
   instructions[INST].modes[ADDR_##ADDR_MODE].cycles = CYCLES; \
   instructions[INST].modes[ADDR_##ADDR_MODE].cycle_change = CYCLE_##CHANGE;

/**
 * Initializes the instruction set with the corresponding opcodes,
 * operation sizes and cycles in CPU
 */
void initialize_instruction_set();

#endif /* instruction_set_h */
