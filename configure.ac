AC_INIT( [ImaNES],[git], [https://csrg.inf.utfsm.cl/flyspray/index.php?project=10],[imanes])

AM_GNU_GETTEXT([external])

AM_INIT_AUTOMAKE
AC_PROG_CC

AC_CONFIG_HEADER([config.h])

# Check SDL library existence
AC_CHECK_LIB( [SDL], [SDL_Init], ,
	AC_MSG_ERROR([SDL library is not present in your system])
)

AC_CHECK_FUNC( [clock_gettime] , ,
	AC_CHECK_LIB( [rt], [clock_gettime] , ,
		AC_MSG_ERROR(['clock_gettime' function cannot be found in your system])
	)
)

AC_CHECK_HEADER( [SDL/SDL.h], ,
	AC_MSG_ERROR([Couldn't find SDL headers])
)

AC_OUTPUT([ po/Makefile.in
Makefile
src/Makefile
])
