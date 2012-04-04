/*  ImaNES: I'm a NES. An intelligent NES emulator

    apu.c   -    APU emulation under ImaNES

    Copyright (C) 2009   Rodrigo Tobar Carrizo

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>

#include "apu.h"
#include "clock.h"
#include "cpu.h"
#include "debug.h"
#include "i18n.h"
#include "imaconfig.h"
#include "playback.h"

nes_apu *APU;

/** The output that comes out from the sequencer
 *  on the triangle channel
 *
 *  This variable is local since it is accessed only in this module
 */
static uint8_t triangle_sequencer_output[32] = {
	0xF, 0xE, 0xD, 0xC, 0xB, 0xA, 0x9, 0x8,
	0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1, 0x0,
	0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
	0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF
};

/* Square channel sequencer outputs.
 *
 * The sequencer works depending on the current
 * duty cycle, like this:
 *
 *   d   waveform sequence
 *   ---------------------
 *        _       1
 *   0   - ------ 0 (12.5%)
 *
 *        __      1
 *   1   -  ----- 0 (25%)
 *
 *        ____    1
 *   2   -    --- 0 (50%)
 *
 *       _  _____ 1
 *   3    --      0 (25% negated)
 */
static uint8_t square_sequencer_output[4][8] = {
	{0, 1, 0, 0, 0, 0, 0, 0},
	{0, 1, 1, 0, 0, 0, 0, 0},
	{0, 1, 1, 1, 1, 0, 0, 0},
	{1, 0, 0, 1, 1, 1, 1, 1}
};

uint8_t square_dac_outputs[32] = {
	 0,  2,  5,  8, 11, 14, 16, 19, 21, 24, 26, 29, 31, 33, 35, 37,
	40, 42, 44, 46, 48, 50, 51, 53, 55, 57, 59, 60, 62, 64, 65, 67
};

uint8_t tnd_dac_outputs[204] = {
	  0,   1,   3,   5,   6,   8,  10,  11,  13,  14,  16,  18,  19,  21,  22,  24,  25,
	 27,  28,  30,  31,  33,  34,  36,  37,  38,  40,  41,  43,  44,  45,  47,  48,  49,
	 51,  52,  53,  55,  56,  57,  58,  60,  61,  62,  63,  65,  66,  67,  68,  69,  71,
	 72,  73,  74,  75,  76,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  89,  90,
	 91,  92,  93,  94,  95,  96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107,
	108, 109, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 118, 119, 120, 121, 122,
	123, 124, 124, 125, 126, 127, 128, 129, 129, 130, 131, 132, 133, 133, 134, 135, 136,
	137, 137, 138, 139, 140, 140, 141, 142, 143, 143, 144, 145, 146, 146, 147, 148, 148,
	149, 150, 151, 151, 152, 153, 153, 154, 155, 155, 156, 157, 157, 158, 159, 159, 160,
	161, 161, 162, 163, 163, 164, 164, 165, 166, 166, 167, 168, 168, 169, 169, 170, 171,
	171, 172, 172, 173, 174, 174, 175, 175, 176, 176, 177, 178, 178, 179, 179, 180, 180,
	181, 181, 182, 183, 183, 184, 184, 185, 185, 186, 186, 187, 187, 188, 188, 189, 189
};


uint8_t length_counter_reload_values[32] = {
	0x0A, 0xFE,
	0x14, 0x02,
	0x28, 0x04,
	0x50, 0x06,
	0xA0, 0x08,
	0x3C, 0x0A,
	0x0E, 0x0C,
	0x1A, 0x0E,
	0x0C, 0x10,
	0x18, 0x12,
	0x30, 0x14,
	0x60, 0x16,
	0xC0, 0x18,
	0x48, 0x1A,
	0x10, 0x1C,
	0x20, 0x1E
};

uint16_t noise_timer_periods[16] = {
	0x004,
	0x008,
	0x010,
	0x020,
	0x040,
	0x060,
	0x080,
	0x0A0,
	0x0CA,
	0x0FE,
	0x17C,
	0x1FC,
	0x2FA,
	0x3F8,
	0x7F2,
	0xFE4
};

void initialize_apu() {

	APU = (nes_apu *)malloc(sizeof(nes_apu));

	APU->commons = 0;

	/* Frame sequencer initialization */
	APU->frame_seq.step = 0;
	APU->frame_seq.clock_timeout = PPUCYCLES_STEP4;
	APU->frame_seq.int_flag = 0;

	/* Triangle channel initialization */
	APU->triangle.timer.timeout = 0;
	APU->triangle.timer.period = 0x07FF;
	APU->triangle.lc.counter = 0;
	APU->triangle.lc.halt = 0;
	APU->triangle.lc.enabled = 0;
	APU->triangle.sequencer_step = 0;
	APU->triangle.linear.counter = 0;
	APU->triangle.linear.reload = 0;
	APU->triangle.linear.halt = 0;
	APU->triangle.linear.control = 0;

	/* Square channels initialization */
	APU->square1.channel = Square1;
	APU->square1.timer.timeout = 0;
	APU->square1.timer.period = 0x07FF;

	APU->square1.sweep.enable = 0;
	APU->square1.sweep.negate = 0;
	APU->square1.sweep.shift = 0;
	APU->square1.sweep.timer.period = 0x08;
	APU->square1.sweep.timer.timeout = 0;

	APU->square1.envelope.disabled = 1;
	APU->square1.envelope.loop = 0;
	APU->square1.envelope.written = 0;
	APU->square1.envelope.counter = 0;
	APU->square1.envelope.timer.period = 0x10;
	APU->square1.envelope.timer.timeout = 0;

	APU->square1.lc.counter = 0;
	APU->square1.lc.halt = 0;

	APU->square1.duty_cycle = 0;
	APU->square1.sequencer_step = 0;

	APU->square2.channel = Square2;
	APU->square2.timer.timeout = 0;
	APU->square2.timer.period = 0x07FF;

	APU->square2.sweep.enable = 0;
	APU->square2.sweep.negate = 0;
	APU->square2.sweep.shift = 0;
	APU->square2.sweep.timer.period = 0x08;
	APU->square2.sweep.timer.timeout = 0;

	APU->square2.envelope.disabled = 1;
	APU->square2.envelope.loop = 0;
	APU->square2.envelope.written = 0;
	APU->square2.envelope.counter = 0;
	APU->square2.envelope.timer.period = 0x10;
	APU->square2.envelope.timer.timeout = 0;

	APU->square2.lc.counter = 0;
	APU->square2.lc.halt = 0;

	APU->square2.duty_cycle = 0;
	APU->square2.sequencer_step = 0;

	/* Noise channel initialization */
	APU->noise.envelope.disabled = 1;
	APU->noise.envelope.loop = 0;
	APU->noise.envelope.written = 0;
	APU->noise.envelope.counter = 0;
	APU->noise.envelope.timer.period = 0x10;
	APU->noise.envelope.timer.timeout = 0;

	APU->noise.lc.counter = 0;
	APU->noise.lc.halt = 0;
	APU->noise.lc.enabled = 0;

	APU->noise.timer.timeout = 0;
	APU->noise.timer.period = 0x07FF;

	APU->noise.shift = 1;
	APU->noise.random_mode = 0;
}

void dump_apu() {

	printf("0x4017:%02x  ", APU->commons);
	printf("FS: %d/%d\n", APU->frame_seq.step, APU->frame_seq.int_flag);

	/* Triangle channel registers */
	printf("Triangle channel:\n");
	printf(" Period:  %u\n", APU->triangle.timer.period);
	printf(" Timeout: %d\n", APU->triangle.timer.timeout);
	printf(" Linear Counter:\n");
	printf("   Current counter: %u\n", APU->triangle.linear.counter);
	printf("   Reload value:    %u\n", APU->triangle.linear.reload);
	printf("   Counter halt:    %u\n", APU->triangle.linear.halt);
	printf("   Counter control: %u\n", APU->triangle.linear.control);
	printf(" Length Counter: %u\n", APU->triangle.lc.counter);
	printf(" Length Halt:    %u\n", APU->triangle.lc.halt);
	printf(" Length Enabled: %u\n", APU->triangle.lc.enabled);
	printf(" Sequencer step: %u\n", APU->triangle.sequencer_step);

	return;
}

void clock_frame_sequencer() {

	/* Reset the clock timeout depending on the sequencer mode */
	APU->frame_seq.clock_timeout += ( (APU->commons & STEP_MODE5) ?
	                                PPUCYCLES_STEP5 : PPUCYCLES_STEP4);


	/* At any time, if the interrupt flag is set
    * and the IRQ disable is clear, CPU's IRQ is asserted */
	if( APU->frame_seq.int_flag && !(APU->commons & DISABLE_FRAME_IRQ) ) {
		CPU->SR &= ~B_FLAG;
		execute_irq();
	}

	/* Different actions depending on the step number
    * and the step mode of the frame sequencer */
	if( APU->commons & STEP_MODE5 ) {

		switch(APU->frame_seq.step) {

			case 0:
				clock_lc_sweep();
				clock_envelopes_tlc();
				break;

			case 1:
				clock_envelopes_tlc();
				break;

			case 2:
				clock_lc_sweep();
				clock_envelopes_tlc();
				break;

			case 3:
				clock_envelopes_tlc();
				break;

			case 4:
				/* Doesn't do anything? */
				break;
		}

	}
	else {

		switch(APU->frame_seq.step) {

			case 0:
				clock_envelopes_tlc();
				break;

			case 1:
				clock_lc_sweep();
				clock_envelopes_tlc();
				break;

			case 2:
				clock_envelopes_tlc();
				break;

			case 3:
				clock_lc_sweep();
				clock_envelopes_tlc();
				APU->frame_seq.int_flag = 1;
				break;

		}
	}

	/* Finally, we increase the step counter */
	APU->frame_seq.step++;
	if( APU->commons & STEP_MODE5 ) {
		if( APU->frame_seq.step == 5 ) {
			APU->frame_seq.step = 0;
		}
	}
	else
		if( APU->frame_seq.step == 4 ) {
			APU->frame_seq.step = 0;
		}

}

void clock_envelope(apu_envelope *e) {

	if( e->written ) {
		e->counter = 15;
		e->timer.timeout = e->timer.period;
	}
	else
		e->timer.timeout--;
	e->written = 0;

	if( e->timer.timeout <= 0 ) {
		if( e->loop && !e->counter )
			e->counter = 15;
		else if( e->counter )
			e->counter--;
	}

}

void clock_envelopes_tlc() {

	/* Clock triangle linear counter*/
	if( APU->triangle.linear.halt )
		APU->triangle.linear.counter = APU->triangle.linear.reload;
	else if( APU->triangle.linear.counter )
		APU->triangle.linear.counter--;

	if ( !APU->triangle.linear.control )
		APU->triangle.linear.halt = 0;

	/* Clock square channels' envelope */
	clock_envelope(&APU->square1.envelope);
	clock_envelope(&APU->square2.envelope);

	/* Clock noise channel's envelope */
	clock_envelope(&APU->noise.envelope);
}

void clock_length_counter(apu_length_counter *lc) {
	if( !lc->halt && lc->counter )
		lc->counter--;
}

void clock_sweep(nes_square_channel *s) {

	/* Calculate the new period */
	s->sweep.new_period = s->timer.period >> s->sweep.shift;
	if( s->sweep.negate ) {
		s->sweep.new_period = (!s->sweep.new_period) & 0x7FF;
		if( s->channel == Square2 )
			s->sweep.new_period++;
	}
	s->sweep.new_period = s->timer.period + s->sweep.new_period;

	s->sweep.timer.timeout--;
	if( s->sweep.written )
		s->sweep.timer.timeout = s->sweep.timer.period;
	s->sweep.written = 0;

	/* Possibily update the channel's period */
	if( !(s->timer.period < 8 || s->sweep.new_period > 0x7FF) &&
	    !s->sweep.enable && s->sweep.shift &&
	    s->sweep.timer.timeout <= 0 )
		s->timer.period = s->sweep.new_period;

}

void clock_lc_sweep() {

	/* Clock the length counters for triangle and square channels */
	clock_length_counter(&APU->triangle.lc);
	clock_length_counter(&APU->square1.lc);
	clock_length_counter(&APU->square2.lc);
	clock_length_counter(&APU->noise.lc);

	/* Clock sweep unit on square channels */
	clock_sweep(&APU->square1);
	clock_sweep(&APU->square2);

}

void clock_triangle_timer() {

	uint8_t dac_output;
	uint8_t index;

	/* Reset the timeout counter */
	APU->triangle.timer.timeout += APU->triangle.timer.period;

	/* Check if the linear counter allows us to pass through... */
	if( !APU->triangle.linear.counter )
		return;

	/* Check if the length counter allows us to pass through... */
	if( !APU->triangle.lc.counter )
		return;

	/* Clock the sequencer! :) */
	index = APU->triangle.sequencer_step++ & 0x1F;
	dac_output = triangle_sequencer_output[index];

	playback_add_sample(Triangle, dac_output);

}

void clock_square_timer(nes_square_channel *s) {

	uint8_t volume;

	/* Reset the timeout counter */
	s->timer.timeout += s->timer.period << 1; /* Timer output is divided by 2 */

	/* Clock the sequencer */
	s->sequencer_step++;
	if( square_sequencer_output[s->duty_cycle][s->sequencer_step] &&
	   !(s->timer.period < 8 || s->sweep.new_period > 0x7FF) ) {
		if( s->envelope.disabled )
			volume = s->envelope.timer.period-1;
		else
			volume = s->envelope.counter;

		playback_add_sample(s->channel, volume);
	}
	else
		playback_add_sample(s->channel, 0);

}

void clock_noise_timer() {

	uint8_t  preshift_bits;
	uint16_t new_bit;

	/* Reset the timer */
	APU->noise.timer.timeout += APU->noise.timer.period;

	/* Update shift register */
	preshift_bits = APU->noise.shift & 0x7F;
	APU->noise.shift >>= 1;
	APU->noise.shift &= 0x7FFF;

	new_bit = preshift_bits & 0x01;
	if( !APU->noise.random_mode )
		new_bit ^= (preshift_bits & 0x02) >> 1;
	else if( APU->noise.random_mode == 1 )
		new_bit ^= (preshift_bits & 0x40) >> 6;
	else
		NORMAL( printf("Invalid random mode in noise channel: %u\n", APU->noise.random_mode) );

	APU->noise.shift |= ((new_bit << 14) & 0x80);

	/* See if we can pass through from envelope to DAC */
	if( APU->noise.shift & 0x01 ) {
		playback_add_sample(Noise, 0);
		return;
	}

	if( !APU->noise.shift )
		return;
	if( !APU->noise.lc.counter )
		return;

	playback_add_sample(Noise, APU->noise.envelope.counter);
}

void clock_dmc_timer() {

	/* Reset the timer */
	APU->dmc.timer.timeout += APU->dmc.timer.period;

}

void end_apu() {

	if( APU != NULL )
		free(APU);

}
