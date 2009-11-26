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
#include "cpu.h"
#include "debug.h"
#include "i18n.h"

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

float normal_square_dac_outputs[32] = {
	0.0,
	0.011609139523,
	0.022939481268,
	0.034000949216,
	0.044803001876,
	0.055354659249,
	0.065664527956,
	0.075740824648,
	0.085591397849,
	0.095223748338,
	0.104645048203,
	0.113862158648,
	0.122881646655,
	0.131709800594,
	0.140352644836,
	0.148815953469,
	0.157105263158,
	0.165225885226,
	0.173182917002,
	0.180981252493,
	0.188625592417,
	0.196120453657,
	0.203470178156,
	0.210678941312,
	0.217750759878,
	0.224689499435,
	0.231498881432,
	0.238182489841,
	0.244743777452,
	0.251186071817,
	0.257512580877,
	0.26372639829
};

float normal_tnd_dac_outputs[204] = {
	0.0066998239797,
	0.0133450201802,
	0.0199362540095,
	0.0264741801124,
	0.0329594425873,
	0.0393926751976,
	0.0457745015782,
	0.0521055354371,
	0.0583863807523,
	0.0646176319634,
	0.0707998741594,
	0.0769336832622,
	0.0830196262047,
	0.0890582611061,
	0.0950501374424,
	0.100995796213,
	0.106895770103,
	0.112750583643,
	0.118560753365,
	0.124326787952,
	0.130049188392,
	0.135728448113,
	0.141365053138,
	0.14695948221,
	0.15251220694,
	0.158023691931,
	0.163494394909,
	0.168924766855,
	0.174315252121,
	0.179666288558,
	0.184978307631,
	0.190251734534,
	0.195486988309,
	0.20068448195,
	0.205844622516,
	0.210967811236,
	0.216054443612,
	0.221104909524,
	0.226119593326,
	0.231098873945,
	0.236043124978,
	0.240952714781,
	0.245828006567,
	0.250669358488,
	0.25547712373,
	0.260251650593,
	0.264993282579,
	0.269702358474,
	0.274379212426,
	0.279024174026,
	0.283637568385,
	0.288219716211,
	0.292770933882,
	0.297291533519,
	0.301781823058,
	0.306242106318,
	0.310672683073,
	0.315073849115,
	0.319445896325,
	0.32378911273,
	0.328103782576,
	0.33239018638,
	0.336648600999,
	0.340879299684,
	0.345082552142,
	0.349258624592,
	0.353407779819,
	0.357530277233,
	0.361626372923,
	0.365696319704,
	0.369740367177,
	0.373758761775,
	0.377751746815,
	0.381719562545,
	0.385662446197,
	0.389580632027,
	0.393474351369,
	0.397343832675,
	0.401189301561,
	0.405010980853,
	0.408809090629,
	0.412583848258,
	0.416335468448,
	0.42006416328,
	0.423770142252,
	0.427453612317,
	0.431114777922,
	0.434753841046,
	0.438371001234,
	0.441966455639,
	0.445540399055,
	0.449093023949,
	0.452624520503,
	0.45613507664,
	0.459624878063,
	0.463094108285,
	0.466542948661,
	0.469971578423,
	0.473380174706,
	0.476768912581,
	0.480137965088,
	0.483487503258,
	0.486817696151,
	0.490128710876,
	0.493420712625,
	0.496693864697,
	0.499948328528,
	0.503184263714,
	0.506401828039,
	0.509601177503,
	0.512782466341,
	0.515945847055,
	0.519091470431,
	0.522219485572,
	0.525330039912,
	0.528423279245,
	0.531499347748,
	0.534558387999,
	0.537600541003,
	0.540625946214,
	0.543634741552,
	0.546627063429,
	0.549603046766,
	0.552562825016,
	0.55550653018,
	0.558434292832,
	0.561346242135,
	0.564242505858,
	0.5671232104,
	0.569988480806,
	0.572838440781,
	0.575673212715,
	0.578492917696,
	0.581297675528,
	0.584087604748,
	0.586862822642,
	0.589623445265,
	0.592369587453,
	0.59510136284,
	0.597818883874,
	0.600522261834,
	0.603211606842,
	0.605887027881,
	0.608548632807,
	0.611196528368,
	0.613830820211,
	0.616451612903,
	0.619059009941,
	0.621653113768,
	0.624234025783,
	0.626801846357,
	0.629356674846,
	0.631898609604,
	0.634427747993,
	0.636944186397,
	0.639448020236,
	0.641939343976,
	0.64441825114,
	0.646884834323,
	0.649339185202,
	0.651781394544,
	0.654211552222,
	0.656629747225,
	0.659036067667,
	0.661430600798,
	0.663813433018,
	0.666184649882,
	0.668544336113,
	0.670892575614,
	0.673229451475,
	0.675555045982,
	0.677869440632,
	0.680172716135,
	0.682464952431,
	0.684746228693,
	0.687016623339,
	0.689276214043,
	0.691525077737,
	0.69376329063,
	0.695990928205,
	0.698208065238,
	0.7004147758,
	0.702611133266,
	0.704797210326,
	0.706973078989,
	0.709138810594,
	0.711294475819,
	0.713440144682,
	0.715575886558,
	0.717701770177,
	0.71981786364,
	0.721924234418,
	0.724020949369,
	0.726108074733,
	0.72818567615,
	0.730253818662,
	0.732312566717,
	0.734361984183,
	0.736402134346,
	0.738433079925,
	0.740454883072,
	0.742467605381,
	0.744471307894,
	0.746466051108
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

void initialize_apu() {

	APU = (nes_apu *)malloc(sizeof(nes_apu));

	APU->length_ctr = 0;
	APU->commons = 0;

	/* Frame sequencer initialization */
	APU->frame_seq.step = 0;
	APU->frame_seq.clock_timeout = PPUCYCLES_STEP4;
	APU->frame_seq.int_flag = 0;

}

void dump_apu() {

	printf("0x4015:%02x  ", APU->length_ctr);
	printf("0x4017:%02x  ", APU->commons);
	printf("FS: %d/%d\n", APU->frame_seq.step, APU->frame_seq.int_flag);

	return;
}

void clock_frame_sequencer() {

	/* Reset the clock timeout depending on the sequencer mode */
	APU->frame_seq.clock_timeout += ( (APU->commons & STEP_MODE5) ?
	                                PPUCYCLES_STEP5 : PPUCYCLES_STEP4);


	INFO( dump_apu() );

	/* At any time, if the interrupt flag is set
    * and the IRQ disable is clear, CPU's IRQ is asserted */
	if( APU->frame_seq.int_flag && !(APU->commons & DISABLE_FRAME_IRQ) ) {
		printf("Triggering IRQ from APU\n");
		CPU->SR &= ~B_FLAG;
		execute_irq();
	}

	/* Different actions depending on the step number
    * and the step mode of the frame sequencer */
	if( APU->commons & STEP_MODE5 ) {

		switch(APU->frame_seq.step) {

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
				break;

			case 4:
				clock_envelopes_tlc();
				break;

			case 5:
				/* Doesn't do anything? */
				break;
		}

	}
	else {

		switch(APU->frame_seq.step) {

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
				clock_lc_sweep();
				clock_envelopes_tlc();
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

void clock_envelopes_tlc() {

	/* Clock triangle linear counter*/
	if( APU->triangle.linear_halt )
		APU->triangle.linear_counter = APU->triangle.linear_reload;
	else if( APU->triangle.linear_counter )
		APU->triangle.linear_counter--;

	if ( !APU->triangle.linear_control )
		APU->triangle.linear_halt = 0;
}

void clock_lc_sweep() {

	/* Clock the triangle lenght counter */
	if( !APU->triangle.length_halt && APU->triangle.length_counter )
		APU->triangle.length_counter--;

}

void clock_triangle_timer() {

	uint8_t dac_output;
	uint8_t index;

	/* Reset the timeout counter */
	APU->triangle.clock_timeout += APU->triangle.period;

	/* Check if the linear counter allows us to pass through... */
	if( !APU->triangle.linear_counter )
		return;

	/* Check if the length counter allows us to pass through... */
	if( !APU->triangle.length_counter )
		return;

	/* Clock the sequencer! :) */
	index = APU->triangle.sequencer_step++ & 0x1F;
	dac_output = triangle_sequencer_output[index];
}

void clock_square_timer(int square_channel) {
	APU->square1.clock_timeout += 1000;
	APU->square2.clock_timeout += 1000;
}

void clock_noise_timer() {
	APU->noise.clock_timeout += 1000;
}

void clock_dmc_timer() {
	APU->dmc.clock_timeout += 1000;
}

void end_apu() {

	if( APU != NULL )
		free(APU);

}
