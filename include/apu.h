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
#define DISABLE_FRAME_IRQ 0x40

/* Number or PPU cycles which define a step, rounded */
#define PPUCYCLES_STEP4  (27919) /* 27919.375 */
#define PPUCYCLES_STEP5  (22336) /* 22335.5 */

/* Frame Sequencer structure */
typedef struct _frame_seq {

	int clock_timeout;  /* PPU cycles until the next sequencer clock */
	int8_t step;         /* In which step we are (1 ... 4/5) */
	int8_t int_flag;    /* Internal interrupt flag */

} nes_frame_seq;

/* APU final structure */
typedef struct _apu {

	/* Registers */
	uint8_t length_ctr; /* 0x4015 */
	uint8_t commons;    /* 0x4017 */

	/* Frame sequencer */
	nes_frame_seq frame_seq;

} nes_apu;

extern nes_apu *APU;

/**
 * Lookup tables used to get the final normalized output
 * from the DACs.
 */


/**
 * Initialize the APU with its registers
 */
void initialize_apu();

/**
 * Dumps the current contents of the APU to the stdout
 */
void dump_apu();

/**
 * Clocks the frame sequencer.
 * When the frame sequencer is clocked,
 * a series of events are triggered,
 * depending on some flags and on the state of some counters.
 *
 * This method is called from the main loop of the emulation
 */
void clock_apu_sequencer();


/**
 * Clocks all the evelope units
 * and the triangle channel's linear counter
 */
void clock_envelopes_tlc();

/**
 * Clocks all linear counters and sweep units
 */
void clock_lc_sweep();

/**
 * Frees all the resources used by the APU 
 */
void end_apu();

#endif /* apu_h */
