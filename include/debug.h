#ifndef debug_h
#define debug_h

#include "imaconfig.h"

#define NORMAL_LEVEL  0
#define INFO_LEVEL    1
#define DEBUG_LEVEL   2
#define XTREME_LEVEL  3

#define NORMAL( X )  if( config.verbosity >= NORMAL_LEVEL ) { X; }
#define INFO( X )    if( config.verbosity >= INFO_LEVEL   ) { X; }
#define DEBUG( X )   if( config.verbosity >= DEBUG_LEVEL  ) { X; }
#define XTREME( X )  if( config.verbosity >= XTREME_LEVEL ) { X; }

extern int verbosity;

#endif /* debug_h */
