#ifndef platform_h
#define platform_h

#undef IMANES_CLOSE  /* close() function */
#undef IMANES_WRITE  /* write() function */
#undef IMANES_READ   /* read() function */
#undef RW_RET        /* Type returned by read()/write() */

#ifdef _MSC_VER
	#define IMANES_CLOSE   _close
	#define IMANES_WRITE   _write
	#define IMANES_READ    _read
	#define RW_RET      int
#else
	#include <sys/types.h>
	#define IMANES_CLOSE   close
	#define IMANES_WRITE   write
	#define IMANES_READ    read
	#define RW_RET      ssize_t
#endif

#endif /* platform_h */
