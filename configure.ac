AC_CONFIG_MACRO_DIRS([m4])

m4_define([v_maj], [3])
m4_define([v_min], [0])
m4_define([v_mic], [1])
m4_define([project_version], [v_maj.v_min.v_mic])
m4_define([lt_cur], [m4_eval(v_maj+v_min)])
m4_define([lt_rev], [v_mic])
m4_define([lt_age], [v_min])

AC_INIT([alt_sockets], [project_version], zorak.x86@gmail.com)
AM_INIT_AUTOMAKE([1.11 subdir-objects])

AM_MAINTAINER_MODE

AC_PROG_LIBTOOL

version_info="lt_cur:lt_rev:lt_age"

# Checks for programs.
AC_PROG_CXX
# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([alt_mutex/locker_mutex.h])
AC_CHECK_HEADERS([alt_mutex/mutex_instance.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_LIB(pthread, pthread_create)
AC_CHECK_LIB(socket, setsockopt)

AC_CHECK_FUNCS([gethostbyname])
AC_CHECK_FUNCS([gethostbyname_r])
AC_CHECK_FUNCS([gettimeofday])

AX_PTHREAD()
AX_CHECK_OPENSSL([:],[AC_MSG_FAILURE([OpenSSL/LibreSSL not found])])

AM_LDFLAGS=
AC_CHECK_HEADER(windows.h, ac_cv_have_windows_h=1, ac_cv_have_windows_h=0)
if test x"$ac_cv_have_windows_h" = x"1"; then
	AM_LDFLAGS="-lws2_32"
fi

AC_SUBST([version_info])
AC_SUBST([AM_LDFLAGS])

LT_INIT([win32-dll])

AC_OUTPUT(Makefile src/Makefile)

if test x"$ac_cv_have_windows_h" = x"1"; then
       	echo "Compiling for Win32."
fi                   
