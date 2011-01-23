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

typedef enum _nes_apu_channel {
	Square1,
	Square2,
	Triangle,
	DMC,
	Noise
} nes_apu_channel;

/**
 * The emulation of the APU is mostly based on the apu_ref.txt document,
 * available at http://nesdev.parodius.com/apu_ref.txt
 */

/* Frame Sequencer structure */
typedef struct _frame_seq {

	int clock_timeout;  /* PPU cycles until the next sequencer clock */
	int8_t step;         /* In which step we are (1 ... 4/5) */
	int8_t int_flag;    /* Internal interrupt flag */

} nes_frame_seq;

/* Triangle channel */
typedef struct _triangle_channel {

	/* Timer stuff */
	int16_t clock_timeout;
	uint16_t period;

	/* Linear Counter stuff */
	uint8_t linear_counter;
	uint8_t linear_reload;
	uint8_t linear_halt;
	uint8_t linear_control;

	/* Lenght Counter stuff */
	uint8_t length_enabled;
	uint8_t length_halt;
	uint8_t length_counter;

	/* Sequencer stuff */
	uint8_t sequencer_step;

} nes_triangle_channel;

typedef struct _square_channel {

	nes_apu_channel channel;

	/* Timer stuff */
	int16_t clock_timeout;
	uint16_t period;

	/* Sweep Unit stuff */
	uint8_t sweep_enable;
	uint8_t sweep_negate;
	uint8_t sweep_reload;
	uint8_t sweep_period;
	uint8_t sweep_timeout;
	uint8_t sweep_shift;

	/* Envelope stuff */
	uint8_t envelope_disabled;
	uint8_t envelope_loop;
	uint8_t envelope_written;
	uint8_t envelope_counter;
	uint8_t envelope_period;
	uint8_t envelope_timeout;

	/* Lenght Counter stuff */
	uint8_t length_enabled;
	uint8_t length_halt;
	uint8_t length_counter;

	/* Sequencer stuff */
	uint8_t duty_cycle;
	uint8_t sequencer_step;

} nes_square_channel;

typedef struct _noise_channel {

	/* Timer stuff */
	int16_t clock_timeout;
	uint16_t period;

	/* Random (needed?) */

	/* Lenght Counter stuff */

} nes_noise_channel;

typedef struct _delta_modulation_channel {

	/* Timer stuff */
	int16_t clock_timeout;
	uint16_t period;

	/* DMA Reader stuff (needed?) */

	/* Buffer stuff */

	/* Output stuff (needed?) */

	/* Counter stuff */

} nes_delta_modulation_channel;

/* APU final structure */
typedef struct _apu {

	/* Registers */
	uint8_t commons;    /* 0x4017 */

	/* Frame sequencer */
	nes_frame_seq frame_seq;

	/* Triangle channel */
	nes_triangle_channel triangle;

	/* Square channels */
	nes_square_channel square1;
	nes_square_channel square2;

	/* Noise channel */
	nes_noise_channel noise;

	/* DMC */
	nes_delta_modulation_channel dmc;
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
void clock_frame_sequencer();

/**
 * Clocks the timer unit of the triangle channel.
 */
void clock_triangle_timer();

/**
 * Clocks the timer unit of a square channel.
 * @param s A pointer to the square channel to use
 */
void clock_square_timer(nes_square_channel *s);

/**
 * Clocks the timer unit of the noise channel.
 */
void clock_noise_timer();

/**
 * Clocks the timer unit of the DMC channel.
 */
void clock_dmc_timer();

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


/* Normalized output of DAC for
 * square channel 1 + square channel 2
 * This is implemented as a lookup table. The index for this table
 * will be composed by:
 *
 * square1 + square2
 *
 * Each square DAC input is 4 bits long
 */
extern float normal_square_dac_outputs[32];

/* Normalized output of the DAC for the triangle channel,
 * the noise channel and the delta modulation channel
 *
 * This is implemented as a lookup table. The index for this table
 * will be composed by:
 *
 * 3*triangle + 2*noise + dmc
 *
 * Triangle's DAC input is 4 bits long
 * Noise's DAC input is 4 bits long
 * DMC DAC's input is 7 bits long
 */
extern float normal_tnd_dac_outputs[204];

/**
 * Look-up table for the Lenght Counters.
 * The index corresponds to the 5-bit value written
 * into the register of the associated Lenght counter,
 * and should be calculated as follows:
 *
 * (bits 7-3) >> 3
 */
uint8_t length_counter_reload_values[32];

#endif /* apu_h */
