#ifndef debug_h
#define debug_h

#include "imaconfig.h"

#define NORMAL_LEVEL  0
#define INFO_LEVEL    1
#define DEBUG_LEVEL   2
#define XTREME_LEVEL  3

#define NORMAL( X )  LOG(NORMAL_LEVEL, X)
#define INFO( X )    LOG(INFO_LEVEL, X)
#define DEBUG( X )   LOG(DEBUG_LEVEL, X)
#define XTREME( X )  LOG(XTREME_LEVEL, X)
#define LOG(level, X) \
	do { \
		if( config.verbosity >= level ) { X; } \
	} while(0);

extern int verbosity;

#endif /* debug_h */
