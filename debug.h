#ifndef debug_h
#define debug_h

#define NORMAL_LEVEL  0
#define INFO_LEVEL    1
#define DEBUG_LEVEL   2

#define DEBUG( X )   if( verbosity > 2 ) { X; }

extern int verbosity;

#endif /* debug_h */
