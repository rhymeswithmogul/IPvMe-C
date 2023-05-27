dnl  IPvMe
dnl  Copyright (c) 2022-2023 Colin Cogle <colin@colincogle.name>
dnl
dnl  This file, configure.ac, is part of IPvMe.
dnl  <https://github.com/rhymeswithmogul/IPvMe>
dnl
dnl  This program is free software: you can redistribute it and/or modify it
dnl  under the terms of the GNU Affero General Public License as published by
dnl  the Free Software Foundation, either version 3 of the License, or (at your
dnl  option) any later version.
dnl
dnl  This program is distributed in the hope that it will be useful, but
dnl  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public
dnl  License for more details.
dnl
dnl  You should have received a copy of the GNU Affero General Public License
dnl  along with this program. If not, see <http://gnu.org/licenses/agpl-3.0.html>.

AC_INIT([IPvMe], [1.0.0], [colin@colincogle.name], [ipvme], [https://github.com/rhymeswithmogul/IPvMe-C])
AC_REVISION([$Revision: 1 $])
AM_INIT_AUTOMAKE([foreign subdir-objects -Wall -Werror])
AC_PREREQ

dnl  Check for a C99 compiler with GNU extensions.
dnl  In case it's needed, this code is perfectly valid C11 and C17/C18, too.
AC_LANG([C])
AC_PROG_CC
AC_PROG_CC_C_O
AC_C_CONST
AC_REQUIRE_CPP
AM_CFLAGS="-std=gnu99"
dnl ^^ we'll append to that variable as we go.

dnl  Check for install.
AC_PROG_INSTALL

dnl  Define types
AC_TYPE_SIZE_T

dnl  Check for the standard I/O functions
AC_CHECK_HEADER([stdio.h], [], AC_MSG_ERROR([stdio.h is required to build this package.]))
AC_CHECK_FUNC([fprintf],   [], AC_MSG_ERROR([fprintf() is required to build this package but was not found in stdio.h.]))
AC_CHECK_FUNC([printf],    [], AC_MSG_ERROR([printf() is required to build this package but was not found in stdio.h.]))
AC_CHECK_FUNC([puts],      [], AC_MSG_ERROR([puts() is required to build this package but was not found in stdio.h.]))
AC_CHECK_FUNC([sprintf],   [], AC_MSG_ERROR([sprintf() is required to build this package but was not found in stdio.h.]))

dnl  Check for string functions.
AC_CHECK_HEADER([string.h], [], AC_MSG_ERROR([string.h is required to build this package.]))
AC_CHECK_FUNC([strcat],     [], AC_MSG_ERROR([strcat() is required to build this package but was not found in string.h.]))
AC_CHECK_FUNC([strcmp],     [], AC_MSG_ERROR([strcpy() is required to build this package but was not found in string.h.]))
AC_CHECK_FUNC([strcpy],     [], AC_MSG_ERROR([strcpy() is required to build this package but was not found in string.h.]))
AC_CHECK_FUNC([strlen],     [], AC_MSG_ERROR([strlen() is required to build this package but was not found in string.h.]))
AC_CHECK_FUNC([strtok],     [], AC_MSG_ERROR([strtok() is required to build this package but was not found in string.h.]))

dnl  Check for close().
AC_CHECK_HEADER([unistd.h], [], AC_MSG_ERROR([unistd.h is required to build this package.]))
AC_CHECK_FUNC([close],      [], AC_MSG_ERROR([close() is required to build this package but was not found in unistd.h.]))

dnl  Check for getopt.
AC_CHECK_HEADER([getopt.h],  [], AC_MSG_ERROR([getopt.h is required to build this package.]))
AC_CHECK_FUNC([getopt_long], [], AC_MSG_ERROR([getopt_long() is required to build this package but was not found in getopt.h.]))

dnl  Check for socket functions.
AC_CHECK_HEADER([netdb.h],      [], AC_MSG_ERROR([netdb.h is required to build this package.]))
AC_CHECK_HEADER([sys/socket.h], [], AC_MSG_ERROR([socket.h is required to build this package.]))
AC_CHECK_FUNC([recv],           [], AC_MSG_ERROR([recv() is required to build this package but was not found in sys/socket.h.]))
AC_CHECK_FUNC([send],           [], AC_MSG_ERROR([send() is required to build this package but was not found in sys/socket.h.]))
AC_CHECK_FUNC([socket],         [], AC_MSG_ERROR([socket() is required to build this package but was not found in sys/socket.h.]))

dnl  Check to see if we can compile this with threading support, unless the user
dnl  asked us not to.  If --disable-threads was *not* specified and we cannot
dnl  compile with pthread support for whatever reason, fail.
AC_ARG_ENABLE([threads], 
    [AS_HELP_STRING([--disable-threads], [disable threading support])],
    [enable_threads="no"],
    [enable_threads="yes"]
)
AS_IF([test "$enable_threads" = "no"], [
    AC_DEFINE([WITHOUT_THREADS], [1], [Disable threads])
],[
    AC_CHECK_HEADER([pthread.h],    [], AC_MSG_ERROR([Cannot compile with threading support because <pthread.h> is missing.]))
    AC_SEARCH_LIBS([pthread_create], [pthread], [], AC_MSG_ERROR([pthread_create is required to build this package without --disable-threads.]))
    AC_SEARCH_LIBS([pthread_join],   [pthread], [], AC_MSG_ERROR([pthread_join is required to build this package without --disable-threads.]))
    AM_CFLAGS="${AM_CFLAGS} -pthread"
])

AC_ARG_ENABLE([ipv4], 
    [AS_HELP_STRING([--disable-ipv4], [disable IPv4 support])],
    [enable_ipv4="no"],
    [enable_ipv4="yes"]
)
AS_IF([test "$enable_ipv4" = "no"], [
    AC_DEFINE([WITHOUT_IPV4], [1], [Disable IPv4 support])
])

AC_ARG_ENABLE([ipv6], 
    [AS_HELP_STRING([--disable-ipv6], [disable IPv6 support])],
    [enable_ipv6="no"],
    [enable_ipv6="yes"]
)
AS_IF([test "$enable_ipv6" = "no"], [
    AC_DEFINE([WITHOUT_IPV6], [1], [Disable IPv6 support])
])

AC_ARG_ENABLE([json], 
    [AS_HELP_STRING([--disable-ipv6], [disable JSON output option])],
    [enable_json="no"],
    [enable_json="yes"]
)
AS_IF([test "$enable_json" = "no"], [
    AC_DEFINE([WITHOUT_JSON], [1], [Disable JSON output option])
])

AC_ARG_ENABLE([xml], 
    [AS_HELP_STRING([--disable-xml], [disable XML output option])],
    [enable_xml="no"],
    [enable_xml="yes"]
)
AS_IF([test "$enable_xml" = "no"], [
    AC_DEFINE([WITHOUT_XML], [1], [Disable XML output option])
])

AC_ARG_ENABLE([debug], 
    [AS_HELP_STRING([--enable-debug], [build with added debugging output])],
    [enable_debug="yes"],
    [enable_debug="no"]
)
AS_IF([test "$enable_debug" = "yes"], [
    AC_DEFINE([DEBUG], 1, [Build with symbols and debugging output.])
    AM_CFLAGS="${AM_CFLAGS} -O0 -g3"
])

dnl  Done!
AC_SUBST([AM_CFLAGS])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
AC_MSG_RESULT([
############################
Run make to build this app.

OPTIONS:
  IPv4 support:     $enable_ipv4
  IPv6 support:     $enable_ipv6
  JSON support:     $enable_json
  XML support:      $enable_xml
  Multi-threaded:   $enable_threads
  Debugging output: $enable_debug
###########################
])