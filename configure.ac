dnl	SLASHEM autoconf template
dnl	SCCS Id: @(#)configure.ac	0.1	2005/04/22
AC_INIT(SLASHEM, 0.1, http://sourceforge.net/bugs/?group_id=9746)

dnl  ----- Precious variables
AC_ARG_VAR(CC, [C compiler])
AC_ARG_VAR(CHGRP, [sets GAMEUID (set this to true as a no op)])
AC_ARG_VAR(CHOWN, [sets GAMEGRP (set this to true as a no op)])


dnl  ----- Macros
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

dnl  ----- Check options
dnl  ----- Window systems
AC_MSG_CHECKING(for valid window system configuration)
AC_ARG_ENABLE(tty-graphics, 
	      AC_HELP_STRING([--enable-tty-graphics],
			     [use tty-graphics (default=yes)]),
			     enable_tty_graphics="$enableval",enable_tty_graphics="yes")
AC_ARG_ENABLE(x11-graphics, 
	      AC_HELP_STRING([--enable-x11-graphics],
			     [use x11-graphics (default=yes)]),
			     enable_x11_graphics="$enableval",enable_x11_graphics="no")
AC_ARG_ENABLE(sdl-graphics, 
	      AC_HELP_STRING([--enable-sdl-graphics],
			     [use sdl-graphics (default=yes)]),
			     enable_sdl_graphics="$enableval",enable_sdl_graphics="no")
AC_ARG_ENABLE(gl-graphics, 
	      AC_HELP_STRING([--enable-gl-graphics],
			     [use gl-graphics (enables sdl-graphics) (default=yes)]),
			     [enable_gl_graphics="$enableval"
			      if test "$enable_gl_graphics" != "no"
			      then
			      	enable_sdl_graphics="yes"
			      fi],
			     enable_gl_graphics="no")


dnl  ----- Sanity check - Must choose at least one
if test "$enable_tty_graphics" = "no" && test "$enable_x11_graphics" = "no"
then
	AC_MSG_RESULT([no])
	AC_MSG_ERROR([need to enable at least one window system])
fi
AC_MSG_RESULT([yes])

dnl  ----- Figure out a C compiler to use; set @CC@
AC_PROG_CC

dnl  ----- Check OS
AC_CANONICAL_TARGET
dnl  ------ Main unix types
case $target_os in
     *bsd* )		AC_DEFINE(BSD);;
     * )		AC_DEFINE(SYSV,[]);;
esac
dnl ------ Common subtypes
case $target_os in
     *cygwin* )		AC_DEFINE(CYGWIN32,[]);;
     *linux* )		AC_DEFINE(LINUX,[]);;
esac


dnl  ----- Check for programs
AC_PROG_LEX
AC_PROG_YACC
AC_CHECK_TOOLS(CHOWN,[chown true])
AC_CHECK_TOOLS(CHGRP,[chgrp true])

dnl  ----- Check for libraries

dnl  ----- Select window systems

WINSRC=
WINOBJ=
WINLIB=
CNF_SHARE_DATND=

WINTTYLIB=
if test "$enable_tty_graphics" != "no"
then
dnl  ----- For TTY, determine the termlib
dnl  ----- Check for termlib before termcap because Solaris termcap needs libucb.
TERMLIB_VARIANTS="ncurses curses termlib termcap terminfo"
for termlib in ${TERMLIB_VARIANTS}; do
  AC_CHECK_LIB(${termlib}, tgetent,
	       [WINTTYLIB="-l${termlib}"
	        WINSRC="${WINSRC} \$(WINTTYSRC)"
		WINOBJ="${WINOBJ} \$(WINTTYOBJ)"
		WINLIB="${WINLIB} \$(WINTTYLIB)"
		AC_DEFINE(TTY_GRAPHICS, [])
		break])
done
fi
AC_SUBST(WINTTYLIB)

if test "$enable_x11_graphics" != "no"
then
	WINSRC="${WINSRC} \$(WINX11SRC)"
	WINOBJ="${WINOBJ} \$(WINX11OBJ)"
	WINLIB="${WINLIB} \$(WINX11LIB)"
	CNF_SHARE_DATND="${CNF_SHARE_DATND} \$(CNF_SHARE_X11ND)"
	AC_DEFINE(X11_GRAPHICS, [])
fi

SDL_CONFIG_CFLAGS=
SDL_CONFIG_LIBS=
if test "$enable_sdl_graphics" != "no"
then
dnl CHECK FOR libpng
  AC_CHECK_TOOL(SDL_CONFIG,sdl-config, no)
  if test "SDL_CONFIG" = no 
  then
	dnl --Maybe don't need sdl-config?
	AC_CHECK_LIB(SDL,SDL_Init,,[SDL_CONFIG=])
  fi
  if test "SDL_CONFIG" != "no"
  then
	 SDL_CONFIG_CFLAGS="`${SDL_CONFIG} --cflags`"
	 SDL_CONFIG_LIBS="`${SDL_CONFIG} --libs`"
	 WINSRC="${WINSRC} \$(WINGLSRC)"
	 WINOBJ="${WINOBJ} \$(WINGLOBJ)"
	 WINLIB="${WINLIB} \$(WINSDLLIB)"
	 CNF_SHARE_DATND="${CNF_SHARE_DATND} \$(CNF_SHARE_GLND)"
	 AC_DEFINE(SDL_GRAPHICS, [])
	 if test "$enable_gl_graphics" != "no"
	 then
		GL_LIB_VARIANTS="GL opengl32"
		for gl_lib in ${GL_LIB_VARIANTS}; do
		  AC_CHECK_LIB(${gl_lib}, glBegin,
		  [WINGLLIB="-l${gl_lib}"
		   WINLIB="${WINLIB} \$(WINGLLIB)"
		   AC_DEFINE(GL_GRAPHICS, [])
		   break])
		done
	 fi
  fi
fi
AC_SUBST(SDL_CONFIG_CFLAGS)
AC_SUBST(SDL_CONFIG_LIBS)


AC_SUBST(WINSRC)
AC_SUBST(WINOBJ)
AC_SUBST(WINLIB)
AC_SUBST(CNF_SHARE_DATND)


dnl  ----- Check for header files
AC_HEADER_STDC
AC_CHECK_HEADER(regex.h, [have_regex_h=yes],[have_regex_h=no],/* TEST_REGEX */)

dnl ------ Check for data types
if test "$have_regex_h" = "yes"; then
   AC_CHECK_TYPE(struct re_pattern_buffer,
		 [AC_DEFINE(USE_REGEX_MATCH, [])
		  AC_DEFINE(GNU_REGEX, [])
		 ],
		 [AC_CHECK_TYPE(regex_t,
				[AC_DEFINE(USE_REGEX_MATCH, [])
				 AC_DEFINE(POSIX_REGEX, [])
				],,)
		 ])	
fi
AC_CHECK_TYPE(void,,AC_DEFINE(NOVOID, []))


dnl  ----- Check for library functions
AC_CHECK_DECL(strncmpi, AC_DEFINE(STRNCMPI, []))

dnl  ----- Check for compile-time options
NETHACK_DISABLE(sinks,sinks,[Kitchen sinks],SINKS)
NETHACK_DISABLE(light-src-spell,light_src_spell,
	[Spells are light sources],LIGHT_SRC_SPELL)
NETHACK_DISABLE(wallified-maze,wallified_maze,
	[Fancy mazes],WALLIFIED_MAZE)
NETHACK_ENABLE(reincarnation,reincarnation,
	[Special Rogue-like levels],REINCARNATION)
NETHACK_DISABLE(blackmarket,blackmarket,
	[Blackmarket],BLACKMARKET)
NETHACK_DISABLE(kops,kops,
	[Keystone Kops],KOPS)
NETHACK_DISABLE(seduce,seduce,
	[Succubi/incubi seduction],SEDUCE)
NETHACK_DISABLE(invisible-objects,invisible_objects,
	[Invisible objects],INVISIBLE_OBJECTS)
NETHACK_DISABLE(unpolypile,unpolypile,
	[Object unpolymorphing],UNPOLYPILE)
NETHACK_DISABLE(wallet-o-p,wallet_o_p,
	[Perseus' Wallet],WALLET_O_P)
NETHACK_DISABLE(lightsabers,lightsabers,
	[Lightsabers],LIGHTSABERS)
NETHACK_DISABLE(d-saber,d_saber,
	[Dimsabers],LIGHTSABERS)
NETHACK_DISABLE(p-spoon,p_spoon,
	[Spoon],P_SPOON)
NETHACK_DISABLE(firearms,firearms,
	[Guns and Bullets],FIREARMS)
NETHACK_DISABLE(eaten-memory,eaten_memory,
	[Remember which monsters have been eaten],EATEN_MEMORY)
NETHACK_DISABLE(steed,steed,
	[Ability to ride monsters],STEED)
NETHACK_DISABLE(dwarf,dwarf,
	[Dwarf race],DWARF)
NETHACK_DISABLE(tourist,tourist,
	[Tourist class],TOURIST)
NETHACK_DISABLE(yeoman,yeoman,
	[Yeoman class],YEOMAN)
NETHACK_ENABLE(zouthern,zouthern,
	[Zoutherner class],ZOUTHERN)


dnl  ----- Back up the existing config.h and unixconf.h if they exist
AC_CONFIG_COMMANDS_PRE([if test -f $srcdir/include/config.h
			then
			    echo moving $srcdir/include/config.h to $srcdir/include/config.bak; \
			    mv -f $srcdir/include/config.h $srcdir/include/config.bak
			fi])
AC_CONFIG_COMMANDS_PRE([if test -f $srcdir/include/unixconf.h
		        then
			    echo moving $srcdir/include/unixconf.h $srcdir/include/unixconf.bak; \

			    mv -f $srcdir/include/unixconf.h $srcdir/include/unixconf.bak
			fi])

dnl  ----- Process the Makefiles
AC_CONFIG_FILES([Makefile src/Makefile doc/Makefile dat/Makefile util/Makefile])
AC_CONFIG_LINKS([include/file.h:include/file.h
	dat/data.base:dat/data.base
	dat/rumors.tru:dat/rumors.tru
	dat/rumors.fal:dat/rumors.fal
	dat/quest.txt:dat/quest.txt
	dat/oracles.txt:dat/oracles.txt
	dat/dungeon.def:dat/dungeon.def
	dat/help:dat/help
	dat/hh:dat/hh
	dat/cmdhelp:dat/cmdhelp
	dat/history:dat/history
	dat/opthelp:dat/opthelp
	dat/wizhelp:dat/wizhelp
	dat/gypsy.txt:dat/gypsy.txt
	win/share/tile_t.h:win/share/tile_t.h])
AC_CONFIG_HEADERS([include/config.h:include/config.hin
	include/unixconf.h:include/unixconf.hin])
AC_OUTPUT
