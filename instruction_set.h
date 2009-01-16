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
#define ADDR_MODES_NUMBER 12

#define IMMEDIATE_MODE 0
#define ABSOLUTE_MODE  1
#define ZEROPAGE_MODE  2
#define IMPLIED_MODE   3
#define INDIR_ABS_MODE 4
#define ABS_INDX_MODE  5
#define ABS_INDY_MODE  6
#define ZERO_INDX_MODE 7
#define ZERO_INDY_MODE 8
#define IND_INDIR_MODE 9
#define INDIR_IND_MODE 10
#define ACCUM_MODE     11

typedef struct {
	int aid;          /* Addressing ID */
	uint8_t opcode;
	short size;
	short cycles;
} _inst_per_addr;

typedef struct {
	int id;            /* One from above's instructions IDs */
	uint8_t flags;     /* Flags set in the CPU->P register  */
	_inst_per_addr modes[ADDR_MODES_NUMBER]; /* Addressing modes */
} instruction;

extern instruction instructions[INSTRUCTIONS_NUMBER];

#endif /* instruction_set_h */
