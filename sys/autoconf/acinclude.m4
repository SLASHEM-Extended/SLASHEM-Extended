# $Id: acinclude.m4,v 1.2 2005-07-02 18:16:09 j_ali Exp $

AC_DEFUN([NETHACK_ENABLE],
  [AC_MSG_CHECKING(whether to enable $1)
   AC_ARG_ENABLE([$2], AC_HELP_STRING([--enable-$1],[$3 (default=yes)]),
     enable_$2="$enableval",enable_$2="no")
   AC_MSG_RESULT($enable_$2)
   if test "$enable_$2" != "no" ; then
      AC_DEFINE_UNQUOTED([$4], [], [$3])
   fi
  ])

AC_DEFUN([NETHACK_DISABLE],
  [AC_MSG_CHECKING(whether to enable $1)
   AC_ARG_ENABLE([$2], AC_HELP_STRING([--enable-$1],[$3 (default=yes)]),
     enable_$2="$enableval",enable_$2="yes")
   AC_MSG_RESULT($enable_$2)
   if test "$enable_$2" != "no" ; then
      AC_DEFINE_UNQUOTED([$4], [], [$3])
   fi
  ])

# Update if there is a newer source
AC_DEFUN([NETHACK_LINKS_ADD],
	if test ${srcdir}/$1 -nt $1 ; then
	   NETHACK_LINKS="${NETHACK_LINKS} $1:$1"
	fi)

# Autoconf's standard AC_CONFIG_HEADERS() avoids undefining switches because
# of concern that it might undefine system switches (eg., POSIX_SOURCE).
# However, we need to override the settings in config.h etc., so we
# post-process the generated headers and force an explicit define or undefine.
AC_DEFUN([NETHACK_CONFIG_HEADER],
    [AC_CONFIG_HEADERS([$1],
      [sed 's,^/\* \(#undef .*\) \*/,\1,'] < m4_bpatsubst([$1], [:.*]) \
        > m4_bpatsubst([$1], [[^.]*:.*], [new])
      [mv] m4_bpatsubst([$1], [[^.]*:.*], [new]) \
        m4_bpatsubst([$1], [:.*]),[])])
