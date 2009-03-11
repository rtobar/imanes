AC_INIT( [imanes], [0.1] )

AM_INIT_AUTOMAKE
AC_PROG_CC

AC_CONFIG_HEADER([config.h])

# Check for SDL support
AC_CHECK_PROG( HAVE_SDL_CONFIG, [sdl-config], yes, no)
if test x"$HAVE_SDL_CONFIG" = x"no"
then
	AC_MSG_ERROR([No sdl-config script found in PATH])
fi

AC_MSG_CHECKING([SDL headers and libraries])

SDL_INCLUDEDIR=`sdl-config --cflags`
SDL_LIBDIR=`sdl-config --libs`

AC_MSG_RESULT([C flags: $SDL_INCLUDEDIR, libraries: $SDL_LIBDIR])

CPPFLAGS="$CFLAGS $SDL_INCLUDEDIR"
LDFLAGS="$LDFLAGS $SDL_LIBDIR"

AC_CHECK_HEADER( SDL.h, ,
	AC_MSG_ERROR([Couldn't find SDL headers])
)

AC_OUTPUT([
Makefile
src/Makefile
])
