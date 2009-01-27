#ifndef debug_h
#define debug_h

#define NORMAL_LEVEL  0
#define INFO_LEVEL    1
#define DEBUG_LEVEL   2
#define XTREME_LEVEL  3

#define NORMAL( X )  if( verbosity >= NORMAL_LEVEL ) { X; }
#define INFO( X )    if( verbosity >= INFO_LEVEL   ) { X; }
#define DEBUG( X )   if( verbosity >= DEBUG_LEVEL  ) { X; }
#define XTREME( X )  if( verbosity >= XTREME_LEVEL ) { X; }

extern int verbosity;

#endif /* debug_h */
