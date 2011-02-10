#ifndef queue_h
#define queue_h

#include <stdint.h>

/*
 * A queue stores DAC samples for a given channel. A sample is "timestamped"
 * with the PPU cycles that have passed since the last NMI on the NES.
 */
typedef struct _dac_queue {
	struct _dac_queue *next;  /* Pointer to next record on the queue */
	unsigned int nmi_pcycles; /* PPU cycles since the last NMI */
	uint8_t sample;           /* Sample played */
} dac_queue;

/* Pops the first element of the FIFO queue and frees its memory.
 * This operation returns the new head of the queue */
dac_queue *pop(dac_queue *q);

void push(dac_queue *q, uint8_t sample);

void clear(dac_queue *q);

#endif /* queue_h */
