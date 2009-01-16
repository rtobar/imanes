#include <stdlib.h>

#include "instruction_set.h"

instruction *instructions;

void initialize_instruction_set() {

	instructions = (instruction *)malloc(sizeof(instruction)*
	                                     INSTRUCTIONS_NUMBER);

	
	/* ADC instruction */
	SET_INSTRUCTION_DATA( ADC, MODIF, MODIF, MODIF, NOMODIF, NOMODIF, MODIF );
	SET_INSTRUCTION_ADDR_DATA( ADC, IMMEDIATE, 0x69, 2, 2, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ADC, ZEROPAGE,  0x65, 2, 3, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ADC, ZERO_INDX, 0x75, 2, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ADC, ABSOLUTE,  0x6D, 3, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ADC, ABS_INDX,  0x7D, 3, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ADC, ABS_INDY,  0x79, 3, 4, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ADC, IND_INDIR, 0x61, 2, 6, NORMAL);
	SET_INSTRUCTION_ADDR_DATA( ADC, INDIR_IND, 0x71, 2, 5, NORMAL);

	return;
}
