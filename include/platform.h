#ifndef platform_h
#define platform_h

#undef IMANES_CLOSE  /* close() function */
#undef IMANES_WRITE  /* write() function */
#undef IMANES_READ   /* read() function */
#undef RW_RET        /* Type returned by read()/write() */

/* Definitions necessary for compilation issues */
#ifdef _MSC_VER
	#include <direct.h>

	#define RW_RET      int

	#define IMANES_CLOSE        _close
	#define IMANES_WRITE        _write
	#define IMANES_READ         _read
	#define IMANES_MKDIR(dir)   _mkdir(dir)
#else
	#include <sys/types.h>

	#define RW_RET      ssize_t

	#define IMANES_CLOSE        close
	#define IMANES_WRITE        write
	#define IMANES_READ         read
	#define IMANES_MKDIR(dir)   mkdir(dir, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)
#endif

/* Definitions for ImaNES */
#ifdef _MSC_VER
	#define DIR_SEP             '\\'
	#define IMANES_USER_DIR     "Imanes"
#else
	#define DIR_SEP             '/'
	#define IMANES_USER_DIR     ".imanes"
#endif

/**
 * Wrapper for platform-specific sprintf function
 */
int imanes_sprintf(char *str, int size, const char *format, ...);

#endif /* platform_h */
