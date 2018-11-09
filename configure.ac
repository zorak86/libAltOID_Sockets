AC_CONFIG_MACRO_DIRS([m4])

m4_define(LIBALTOIDMUTEXVERSION,2.0.2)
AC_INIT(alt_mutex, LIBALTOIDMUTEXVERSION, zorak.x86@gmail.com)
AM_INIT_AUTOMAKE(alt_mutex, LIBALTOIDMUTEXVERSION)
AM_MAINTAINER_MODE

AC_PROG_LIBTOOL

# Checks for programs.
AC_PROG_CXX
# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([pthread.h time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([pthread_create])

AC_OUTPUT(Makefile src/Makefile)
LT_INIT([win32-dll])
