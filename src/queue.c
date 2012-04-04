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

dac_queue *push(dac_queue *q, uint8_t sample) {

	dac_queue *tmp;

	if( q == NULL ) {
		q = (dac_queue *)malloc(sizeof(dac_queue));
		q->ppu_cycles = CLK->ppu_cycles;
		q->sample = sample;
		q->next = NULL;
		return q;
	}

	tmp = q;
	while(tmp->next != NULL) tmp = tmp->next;

	/* Won't queue same value twice */
	if( tmp->sample == sample )
		return q;

	tmp->next = (dac_queue *)malloc(sizeof(dac_queue));
	tmp->next->ppu_cycles = CLK->ppu_cycles;
	tmp->next->sample = sample;
	tmp->next->next = NULL;

	return q;
}

void clear(dac_queue *q) {

	if( q == NULL )
		return;

	do {
		q = pop(q);
	} while(q != NULL);

}

unsigned int queue_length(dac_queue *q) {

	unsigned int result = 0;

	while(q != NULL) {
		q = q->next;
		result++;
	}

	return result;
}
