# $Id: acinclude.m4,v 1.4 2005-08-12 18:59:15 j_ali Exp $

# Front-end of AC_ARG_ENABLE
# Usage:
#	NETHACK_ARG([feature], [description], [pre-processor switch], [default],
#	  [action-if-auto])
# Note that the action-if-auto argument is only used if the default is auto
# (and the user doesn't specify). The action should set the shell variable
# enable_feature, with any '-' characters changed into '_'.
AC_DEFUN([NETHACK_ARG],
  [AC_MSG_CHECKING(whether to enable $1)
   AC_ARG_ENABLE([$1], AC_HELP_STRING([--enable-$1],[$2 (default=$4)]),
     [],m4_if([$4], [auto], [$5], [enable_]m4_bpatsubst([$1], -, _)="[$4]"))
   AC_MSG_RESULT([$enable_]m4_bpatsubst([$1], -, _))
   if test "[$enable_]m4_bpatsubst([$1], -, _)" != "no" ; then
      AC_DEFINE_UNQUOTED([$3], [], [$2])
   fi
  ])

# Update if there is a newer source
AC_DEFUN([NETHACK_LINKS_ADD],
	if test ! -f $1 -o ${srcdir}/$1 -nt $1 ; then
	   NETHACK_LINKS="${NETHACK_LINKS} $1:$1"
	fi)

# Autoconf's standard AC_CONFIG_HEADERS() avoids undefining switches because
# of concern that it might undefine system switches (eg., POSIX_SOURCE).
# However, we need to override the settings in config.h etc., so we post-process
# the generated headers and force an explicit define and/or undefine.
AC_DEFUN([NETHACK_CONFIG_HEADER],
    [AC_CONFIG_HEADERS([$1],
      [sed -e 's,^/\* \(#undef .*\) \*/,\1,' \
        -e '/#define [^ ]*/ { h; s,#define \([^ ]*\).*,#undef \1,p; g; }'] \
        < m4_bpatsubst([$1], [:.*]) > m4_bpatsubst([$1], [[^.]*:.*], [new])
      [mv] m4_bpatsubst([$1], [[^.]*:.*], [new]) \
        m4_bpatsubst([$1], [:.*]),[])])
