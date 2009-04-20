#ifndef states_h
#define states_h

#include "cpu.h"
#include "ppu.h"
#include "mapper.h"

/* Loads an ImaNES state */
void load_state(int i);

/* Saves an ImaNES state */
void save_state(int i);

#endif /* states_h */
