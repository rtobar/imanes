/**
 * Note from the developer:
 *
 * The emulation of the APU is mostly based on the apu_ref.txt document,
 * available at http://nesdev.parodius.com/apu_ref.txt
 */

#ifndef apu_h
#define apu_h

#include <stdint.h>

#include "queue.h"

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
	Square1   = 0,
	Square2   = 1,
	Triangle  = 2,
	DMC       = 3,
	Noise     = 4
} nes_apu_channel;


/*
 * Common structures
 *
 * These are circuits that are common to some channels, and that
 * work with the same logic. Therefore, it's useful to have them
 * defined as units that then are assembled into the channels.
 */

/* Timer */
typedef struct _timer {
	int16_t  timeout;
	uint16_t period;
} apu_timer;

/* Lenght counter */
typedef struct _length_counter {
	uint8_t enabled;
	uint8_t halt;
	uint8_t counter;
} apu_length_counter;

/* Envelope unit */
typedef struct _envelope {
	uint8_t disabled;
	uint8_t loop;
	uint8_t written;
	uint8_t counter;
	uint8_t period;
	uint8_t timeout;
} apu_envelope;

/* Linear counter */
typedef struct _linear_counter {
	uint8_t counter;
	uint8_t reload;
	uint8_t halt;
	uint8_t control;
} apu_linear_counter;

/* Sweep unit */
typedef struct _sweep {
	uint8_t enable;
	uint8_t negate;
	uint8_t reload;
	uint8_t period;
	uint8_t timeout;
	uint8_t shift;
} apu_sweep;

/*
 * High-level APU devices
 *
 * These are the channels of the APU, plus the central frame sequencer
 */

/* Frame Sequencer */
typedef struct _frame_seq {

	int clock_timeout;  /* PPU cycles until the next sequencer clock */
	int8_t step;         /* In which step we are (1 ... 4/5) */
	int8_t int_flag;    /* Internal interrupt flag */

} nes_frame_seq;

/*
 * Triangle channel.
 *
 * The triangle channel contains a 32-step sequencer,
 * a length counter, a linear counter
 */
typedef struct _triangle_channel {

	apu_timer          timer;
	apu_length_counter lc;
	apu_linear_counter linear;

	/* Sequencer stuff */
	uint8_t sequencer_step;

} nes_triangle_channel;

/*
 * Square channel
 *
 * There are two squares channel (1 and 2, duh!). Although there are subtle minor
 * differences between their behavior, they work almost exactly the same way.
 * Each square channel contains an envelope generator, a sweep unit, a timer,
 * an 8-step sequencer, and a length counter.
 */
typedef struct _square_channel {

	nes_apu_channel    channel; /* To differentiate between the two */

	apu_timer          timer;
	apu_length_counter lc;
	apu_envelope       envelope;
	apu_sweep          sweep;

	/* Sequencer stuff */
	uint8_t duty_cycle;
	uint8_t sequencer_step;

} nes_square_channel;

/*
 * Noise channel
 *
 * The noise channel contains a length counter, an envelope generator, a timer,
 * and a 15-bits right shift register with feedback.
 */
typedef struct _noise_channel {

	apu_timer          timer;
	apu_length_counter lc;
	apu_envelope       envelope;

	/* Random stuff */
	uint8_t random_mode;

} nes_noise_channel;

typedef struct _delta_modulation_channel {

	apu_timer timer;

} nes_delta_modulation_channel;

/*
 * The APU
 *
 * The APU contains: a triangle channel, two square channels, a noise channel and
 * a delta modulation channel. Apart from these, there is a central frame sequencer
 * in charge of clocking some internal parts of the channels.
 */
typedef struct _apu {

	/* Registers */
	uint8_t commons;

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
extern dac_queue *dac[5];


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

/**
 * Loop-up table that stores the index-based
 * period values to be used by the noise timer.
 * Differently from the square and triangle channels' timer,
 * the noise channel timer's period is not set directly, but
 * an index is indicated, and the corresponding period value
 * is loaded into the timer.
 */
uint16_t noise_timer_periods[16];

#endif /* apu_h */
