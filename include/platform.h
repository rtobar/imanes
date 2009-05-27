#ifndef platform_h
#define platform_h

#undef IMANES_OPEN   /* open() function */
#undef IMANES_CLOSE  /* close() function */
#undef IMANES_WRITE  /* write() function */
#undef IMANES_READ   /* read() function */
#undef IMANES_MKDIR  /* mkdir() function */
#undef RW_RET        /* Type returned by read()/write() */

#define IMANES_OPEN_READ  0
#define IMANES_OPEN_WRITE 1

/**
 * Definitions necessary for compilation issues. First we include
 * the common .h files, then we have type definitions, and finally
 * some macro definitions for common systems calls
 */
#ifdef _MSC_VER

	#include <direct.h>
	#include <io.h>
	#include <share.h>
	#include <sys/types.h>

	#define RW_RET      int

	#define IMANES_OPEN(fd,file,how) do { \
	            if( how == IMANES_OPEN_READ ) \
	                _sopen_s(&(fd),file, O_RDONLY|O_BINARY, SH_DENYWR, S_IREAD|S_IWRITE); \
	            else if( how == IMANES_OPEN_WRITE ) \
	                _sopen_s(&(fd),file, O_WRONLY|O_CREAT|O_BINARY, SH_DENYWR, S_IREAD|S_IWRITE); \
	            } while (0)
	#define IMANES_CLOSE        _close
	#define IMANES_WRITE        _write
	#define IMANES_READ         _read
	#define IMANES_MKDIR(dir)   _mkdir(dir)

#else

	#include <sys/types.h>
	#include <unistd.h>

	#define RW_RET      ssize_t

	#define IMANES_OPEN(fd,file,how) do { \
	            if( how == IMANES_OPEN_READ ) \
	                fd = open(file, O_RDONLY); \
	            else \
	                fd = open(file, O_CREAT|O_RDWR|O_SYNC|O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); \
	            } while (0)
	#define IMANES_CLOSE        close
	#define IMANES_WRITE        write
	#define IMANES_READ         read
	#define IMANES_MKDIR(dir)   mkdir(dir, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)

#endif /* _MSC_VER */


/*
 * These definitions are intended for ImaNES to work
 * correctly under the different supported operating systems
 */
#ifdef _MSC_VER
	#define DIR_SEP             '\\'
	#define IMANES_USER_DIR     "Imanes"
#else
	#define DIR_SEP             '/'
	#define IMANES_USER_DIR     ".imanes"
#endif /* _MSC_VER */


/**
 * Wrapper for platform-specific sprintf function
 */
int imanes_sprintf(char *str, int size, const char *format, ...);

#endif /* platform_h */
