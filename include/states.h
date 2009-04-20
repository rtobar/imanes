#ifndef states_h
#define states_h

#include "cpu.h"
#include "ppu.h"
#include "mapper.h"

/**
 * Internal ImaNES state information. It includes the CPU, PPU,
 * mapper internals and so.
 */
typedef struct _state {

	nes_cpu *CPU;

} imanes_state;

/**
 * Loads an ImaNES state
 */
imanes_state *load_state(int i);


/**
 * Saves an ImaNES state
 */
void save_state(imanes_state *s, int i);

#endif /* states_h */
