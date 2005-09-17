# $Id: acinclude.m4,v 1.5 2005-09-17 14:58:33 j_ali Exp $

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

# NETHACK_WIN_ENABLE(win, default, help-text)
# -------------------------------------------
AC_DEFUN([NETHACK_WIN_ENABLE],[AC_ARG_ENABLE($1-graphics,
  [AC_HELP_STRING([--enable-$1-graphics],
    m4_ifval([$3],[$3],[use $1-graphics]) [(default=$2)])],
  [enable_$1_graphics="$enableval"], [enable_$1_graphics="$2"])
  if test "$enable_$1_graphics" != "no"; then
    nethack_wins="$1 $nethack_wins"
  fi])

# NETHACK_AUTO_WIN_DISABLE(win)
# -----------------------------
# Disable a windowing system previously set to auto.
AC_DEFUN([NETHACK_AUTO_WIN_DISABLE],[enable_$1_graphics="no"
    nethack_wins=`echo $nethack_wins | sed 's/$1 //'`
])

# NETHACK__LINK_ADD(DEST, [SOURCE])
# ---------------------------------
# Update if there is a newer source
AC_DEFUN([NETHACK__LINK_ADD],
	[m4_ifval([$2],[if test ! -f $1 -o ${srcdir}/$2 -nt $1 ; then
	   NETHACK_LINKS="${NETHACK_LINKS} $1:$2"
	fi],[NETHACK__LINK_ADD([$1],[$1])])])
AC_DEFUN([NETHACK_LINKS_ADD],
	[AC_FOREACH([NETHACK_File],[$1],
	    [NETHACK__LINK_ADD(m4_bpatsubst(NETHACK_File,[:],[,]))])])

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

# NETHACK__ADD_FLAG(output-variable, existing-flags, flag)
# ---------------------------------
# Add a compiler flag to a variable. A number of flags are recognized and
# handled specially (specifically, -I, -L and -l).
AC_DEFUN([NETHACK__ADD_FLAG],[[
    case "$3" in
	-I*|-L*|-l*)
	    for flag in $2; do
		if test $flag = "$3"; then
		    break
		fi
	    done
	    if test $flag = "$3"; then
		$1="$2"
	    else
		case "$3" in
		    -L*)$1="$3 $2";;
		    *)  $1="$2 $3";;
		esac
	    fi
	    ;;
	*)  $1="$2 $3";;
    esac
]])

# NETHACK__ADD_FLAGS(output-variable, existing-flags, new-flags)
# ---------------------------------
# Front-end to NETHACK__ADD_FLAG. Add compiler flags to a variable.
AC_DEFUN([NETHACK__ADD_FLAGS],[
    for flag in $3; do
	NETHACK__ADD_FLAG([$1], [$2], [$flag])
    done
])

# NETHACK__TRY_LIBPNG(STUFF, cflags, libs, action-if, action-if-not)
# ---------------------------------
# Try linking with libpng using the given cflags and libs.
AC_DEFUN([NETHACK__TRY_LIBPNG],[
    nethack__save_cflags=$CFLAGS
    nethack__save_libs=$LIBS
    CFLAGS="$CFLAGS $2"
    LIBS="$LIBS $3"
    AC_MSG_CHECKING([if libpng is useable with $CFLAGS and $LIBS])
    AC_TRY_LINK([#include <png.h>],
      [png_structp ptr=
	    png_create_write_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);],
      [$1_CFLAGS="$2"; $1_LIBS="$3"; AC_MSG_RESULT([yes]); $4],
      [AC_MSG_RESULT([no]); $5])
    CFLAGS=$nethack__save_cflags
    LIBS=$nethack__save_libs])

# NETHACK_CHECK_LIBPNG(stuff, action-if, action-if-not)
# ---------------------------------
# Check if libpng can be used
AC_DEFUN([NETHACK_CHECK_LIBPNG],[
    # Slash'EM may well work with earlier versions,
    # but 1.2.4 is the earliest tested to date.
    PKG_CHECK_MODULES(NETHACK__LIBPNG, libpng >= 1.2.4,
      [nethack__succeeded=maybe], [nethack__succeeded=no])
    # Slash'EM doesn't use zlib directly, but libpng does. This should
    # all be handled in libpng.pc, but isn't in many versions of libpng.
    if test $nethack__succeeded = maybe; then
	NETHACK__ADD_FLAGS(nethack__libs, [$NETHACK__LIBPNG_LIBS], [-lz])
	NETHACK__TRY_LIBPNG($1, $NETHACK__LIBPNG_CFLAGS, $nethack__libs,
	  [nethack__succeeded=yes], [])
    fi
    # If the user has supplied flags, then believe them.
    if test $nethack__succeeded = maybe -a \
      \( -n "$ZLIB_CFLAGS" -o -n "$ZLIB_LIBS" \); then
	NETHACK__ADD_FLAGS(nethack__cflags, [$NETHACK__LIBPNG_CFLAGS],
	  [$ZLIB_CFLAGS])
	NETHACK__ADD_FLAGS(nethack__libs, [$NETHACK__LIBPNG_LIBS],
	  [$ZLIB_LIBS])
	NETHACK__TRY_LIBPNG($1, [$nethack__cflags], [$nethack__libs],
	  [nethack__succeeded=yes], [])
    fi
    # Otherwise, try pkg-config.
    if test $nethack__succeeded = maybe; then
	AC_MSG_CHECKING([for zlib])
	if $PKG_CONFIG --exists zlib; then
	    AC_MSG_RESULT([yes])
	    NETHACK__ADD_FLAGS(nethack__cflags, [$NETHACK__LIBPNG_CFLAGS],
	      [`$PKG_CONFIG --cflags zlib`])
	    NETHACK__ADD_FLAGS(nethack__libs, [$NETHACK__LIBPNG_LIBS],
	      [`$PKG_CONFIG --libs zlib`])
	    NETHACK__TRY_LIBPNG($1, [$nethack__cflags], [$nethack__libs],
	      [nethack__succeeded=yes], [])
	else
	    AC_MSG_RESULT([not found by pkg-config])
	fi
    fi
    # Try the same prefix as libpng since zlib is
    # likely to be installed in the same place.
    if test $nethack__succeeded = maybe; then
        nethack__libpng_prefix=`$PKG_CONFIG --variable=prefix libpng`
	NETHACK__ADD_FLAG(nethack__cflags, [$NETHACK__LIBPNG_CFLAGS],
	  [-I$nethack__libpng_prefix/include])
	NETHACK__ADD_FLAG(nethack__libs, [$NETHACK__LIBPNG_LIBS],
	  [-L$nethack__libpng_prefix/lib])
	NETHACK__ADD_FLAG(nethack__libs, [$nethack__libs], [-lz])
        NETHACK__TRY_LIBPNG($1, [$nethack__cflags], [$nethack__libs],
          [nethack__succeeded=yes], [])
    fi
    if test $nethack__succeeded = yes; then
	AC_MSG_CHECKING([$1_CFLAGS])
	AC_MSG_RESULT([${$1_CFLAGS}])
	AC_SUBST($1_CFLAGS)
	AC_MSG_CHECKING([$1_LIBS])
	AC_MSG_RESULT([${$1_LIBS}])
	AC_SUBST($1_LIBS)
	$2
    m4_ifval([$3],[else $3],[])
    fi
])
