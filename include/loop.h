#ifndef loop_h
#define loop_h

#include <pthread.h>

#include "common.h"

extern pthread_mutex_t pause_mutex;

/**
 * This is the main program loop
 */
void main_loop(ines_file *);

#endif /* loop_h */
