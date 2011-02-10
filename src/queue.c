/*  ImaNES: I'm a NES. An intelligent NES emulator

    queue.c   -    FIFO Queue implementation for ImaNES

    Copyright (C) 2011   Rodrigo Tobar Carrizo

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

#include "clock.h"
#include "queue.h"

dac_queue *pop(dac_queue *q) {

	dac_queue *tmp;

	if( q == NULL )
		return NULL;

	tmp = q;
	q = q->next;
	free(tmp);

	return q;
}

void push(dac_queue *q, uint8_t sample) {

	if( q == NULL ) {
		q = (dac_queue *)malloc(sizeof(dac_queue));
		q->nmi_pcycles = CLK->nmi_pcycles;
		q->sample = sample;
		return;
	}

	while(q->next != NULL) q = q->next;

	/* Won't queue same value, just update it */
	if( q->sample == sample ) {
		q->nmi_pcycles = CLK->nmi_pcycles;
		return;
	}

	q->next = (dac_queue *)malloc(sizeof(dac_queue));
	q->next->nmi_pcycles = CLK->nmi_pcycles;
	q->next->sample = sample;

}

void clear(dac_queue *q) {

	if( q == NULL )
		return;

	do {
		q = pop(q);
	} while(q != NULL);

}
