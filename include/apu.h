#ifndef apu_h
#define apu_h

#include <stdint.h>

/* Flags for the 0x4015 register */
#define LENGTHCTR_DMC     0x10
#define LENGTHCTR_NOISE   0x08
#define LENGTHCTR_TRIANG  0x04
#define LENGTHCTR_PULSE2  0x02
#define LENGTHCTR_PULSE1  0x01

/* Flags for the 0x4017 register */
#define STEP_MODE5        0x80
#define DISABLE_FRAME_INT 0x40

typedef struct _apu {

	uint8_t length_ctr; /* 0x4015 */
	uint8_t commons;    /* 0x4017 */

} nes_apu;

extern nes_apu *APU;


/**
 * Initialize the APU with its registers
 */
void initialize_apu();

/**
 * Dumps the current contents of the APU to the stdout
 */
void dump_apu();

/**
 * Frees all the resources used by the APU 
 */
void end_apu();

#endif /* apu_h */
