dnl	SLASHEM autoconf template
AC_INIT(SLASHEM, 0.0.8, http://sourceforge.net/bugs/?group_id=9746)
dnl Avoid AC_REVISION() because we check configure into CVS
dnl and it will cause confusing revision numbers to be included
dnl in configure if developers forget to check in configure.ac
dnl before running bootstrap.sh and checking in configure.
# $Revision: 1.4 $
AC_CONFIG_AUX_DIR(sys/autoconf)

dnl  ----- Precious variables
AC_ARG_VAR(CC, [C compiler])
AC_ARG_VAR(CHGRP, [sets GAMEUID (set this to true as a no op)])
AC_ARG_VAR(CHOWN, [sets GAMEGRP (set this to true as a no op)])

AC_PROG_LN_S

dnl  ----- Check options
dnl  ----- Window systems
AC_MSG_CHECKING(for valid window system configuration)
AC_ARG_ENABLE(tty-graphics, 
	      AC_HELP_STRING([--enable-tty-graphics],
			     [use tty-graphics (default=yes)]),
			     enable_tty_graphics="$enableval",enable_tty_graphics="yes")
AC_ARG_ENABLE(proxy-graphics, 
	      AC_HELP_STRING([--enable-proxy-graphics],
			     [use proxy-graphics (default=auto)]),
			     enable_proxy_graphics="$enableval",enable_proxy_graphics="auto")
AC_ARG_ENABLE(x11-graphics, 
	      AC_HELP_STRING([--enable-x11-graphics],
			     [use x11-graphics (default=no)]),
			     enable_x11_graphics="$enableval",enable_x11_graphics="no")
AC_ARG_ENABLE(sdl-graphics, 
	      AC_HELP_STRING([--enable-sdl-graphics],
			     [use sdl-graphics (default=no)]),
			     enable_sdl_graphics="$enableval",enable_sdl_graphics="no")
AC_ARG_ENABLE(gl-graphics, 
	      AC_HELP_STRING([--enable-gl-graphics],
			     [use gl-graphics (enables sdl-graphics) (default=no)]),
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
dnl  ------ Main OS types
case $target_os in
     mingw32 )		AC_DEFINE(WIN32,[],[An MS-Windows system]);;
     *bsd* )		AC_DEFINE(BSD,[],[A BSD like system]);;
     * )		AC_DEFINE(SYSV,[],[A System V like system]);;
esac
dnl ------ Common subtypes
case $target_os in
     *cygwin* )		AC_DEFINE(CYGWIN32,[],[Unix on win32]);;
     *linux* )		AC_DEFINE(LINUX,[],[GNU Linux]);;
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
		AC_DEFINE(TTY_GRAPHICS, [], [Console output])
		break])
done
fi
AC_SUBST(WINTTYLIB)

if test "$enable_proxy_graphics" != "no"
then
	if test "$enable_proxy_graphics" = "yes"
	then
	    PKG_CHECK_MODULES(NETHACK_PROXY, nhproxy >= 0.9.0)
	else
	    PKG_CHECK_MODULES(NETHACK_PROXY, nhproxy >= 0.9.0, :, :)
	fi
	if test -n "$NETHACK_PROXY_LIBS"
	then
	    WINSRC="${WINSRC} \$(WINPROXYSRC)"
	    WINOBJ="${WINOBJ} \$(WINPROXYOBJ)"
	    WINLIB="${WINLIB} ${NETHACK_PROXY_LIBS}"
	    CNF_SHARE_DATND="${CNF_SHARE_DATND} \$(CNF_SHARE_PROXYND)"
	    AC_DEFINE(PROXY_GRAPHICS, [], [proxy graphics])
	fi
fi
AC_SUBST(NETHACK_PROXY_CFLAGS)

if test "$enable_x11_graphics" != "no"
then
	WINSRC="${WINSRC} \$(WINX11SRC)"
	WINOBJ="${WINOBJ} \$(WINX11OBJ)"
	WINLIB="${WINLIB} \$(WINX11LIB)"
	CNF_SHARE_DATND="${CNF_SHARE_DATND} \$(CNF_SHARE_X11ND)"
	AC_DEFINE(X11_GRAPHICS, [], [X11 graphics])
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
	 AC_DEFINE(SDL_GRAPHICS, [], [Software SDL graphics])
	 if test "$enable_gl_graphics" != "no"
	 then
		GL_LIB_VARIANTS="GL opengl32"
		for gl_lib in ${GL_LIB_VARIANTS}; do
		  AC_CHECK_LIB(${gl_lib}, glBegin,
		  [WINGLLIB="-l${gl_lib}"
		   WINLIB="${WINLIB} \$(WINGLLIB)"
		   AC_DEFINE(GL_GRAPHICS, [], [OpenGL graphics])
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
		 [AC_DEFINE(USE_REGEX_MATCH, [], [Use regular expressions])
		  AC_DEFINE(GNU_REGEX, [], [GNU regex is available])
		 ],
		 [AC_CHECK_TYPE(regex_t,
				[AC_DEFINE(USE_REGEX_MATCH, [], [Use regular expressions])
				 AC_DEFINE(POSIX_REGEX, [], [Posix regex is available])
				],,)
		 ])	
fi
AC_CHECK_TYPE(void,,AC_DEFINE(NOVOID, [], [No void type available]))


dnl  ----- Check for library functions
AC_CHECK_DECL(strncmpi, AC_DEFINE(STRNCMPI, [], [strncmpi available]))

dnl  ----- Check for compile-time options
# We use auto for data librarian to avoid breaking the rule that autoconf
# should use the same defaults as the manual configuration, but all platforms
# that support autoconf also support DLB, so the automatic choice is always yes.
NETHACK_ARG(data-librarian,[Data librarian],DLB,auto,
  [enable_data_librarian=yes])
NETHACK_ARG(sinks,[Kitchen sinks],SINKS,yes)
NETHACK_ARG(light-src-spell,[Spells are light sources],LIGHT_SRC_SPELL,yes)
NETHACK_ARG(wallified-maze,[Fancy mazes],WALLIFIED_MAZE,yes)
NETHACK_ARG(reincarnation,[Special Rogue-like levels],REINCARNATION,no)
NETHACK_ARG(blackmarket,[Blackmarket],BLACKMARKET,yes)
NETHACK_ARG(kops,[Keystone Kops],KOPS,yes)
NETHACK_ARG(seduce,[Succubi/incubi seduction],SEDUCE,yes)
NETHACK_ARG(invisible-objects,[Invisible objects],INVISIBLE_OBJECTS,yes)
NETHACK_ARG(unpolypile,[Object unpolymorphing],UNPOLYPILE,yes)
NETHACK_ARG(wallet-o-p,[Perseus' Wallet],WALLET_O_P,yes)
NETHACK_ARG(lightsabers,[Lightsabers],LIGHTSABERS,yes)
NETHACK_ARG(d-saber,[Dimsabers],LIGHTSABERS,yes)
NETHACK_ARG(p-spoon,[Spoon],P_SPOON,yes)
NETHACK_ARG(firearms,[Guns and Bullets],FIREARMS,yes)
NETHACK_ARG(eaten-memory,[Remember which monsters have been eaten],
  EATEN_MEMORY,yes)
NETHACK_ARG(steed,[Ability to ride monsters],STEED,yes)
NETHACK_ARG(dwarf,[Dwarf race],DWARF,yes)
NETHACK_ARG(tourist,[Tourist class],TOURIST,yes)
NETHACK_ARG(yeoman,[Yeoman class],YEOMAN,yes)
NETHACK_ARG(zouthern,[Zoutherner class],ZOUTHERN,no)
NETHACK_ARG(redo,[Redo last command],REDO,yes)
NETHACK_ARG(clipping,[Allow smaller screens],CLIPPING,yes)
NETHACK_ARG(menu-color,[TTY graphics menu color],MENU_COLOR,yes)
NETHACK_ARG(elbereth,[Engraving the E-word repels monsters],ELBERETH,yes)
NETHACK_ARG(noartifactwish,[Prevent wishing for special artifacts],
  NOARTIFACTWISH,no)
NETHACK_ARG(no-bones,[Disable loading and saving bones levels],NO_BONES,no)
NETHACK_ARG(user-sounds,
  [Allow user-defined regex mapping from messages to sounds (Qt with NAS only)],
  USER_SOUNDS,no)
NETHACK_ARG(exp-on-botl,[Show experience on bottom line],EXP_ON_BOTL,yes)
NETHACK_ARG(score-on-botl,[Show score on bottom line],SCORE_ON_BOTL,no)
#NETHACK_ARG(borg,[Borg player (only under DOS)],BORG,no)
NETHACK_ARG(keep-save,[Keep save files after restore],KEEP_SAVE,no)
NETHACK_ARG(charon,[Charon's boat, enables Cerebus (not implemented)],CHARON,no)
NETHACK_ARG(show-dmg,[Display damage reports],SHOW_DMG,yes)
NETHACK_ARG(show-weight,[Display object weight],SHOW_WEIGHT,yes)
NETHACK_ARG(other-services,[Shopkeeper services],OTHER_SERVICES,yes)
NETHACK_ARG(dungeon-growth,[Dungeon growth],DUNGEON_GROWTH,yes)
#NETHACK_ARG(shout,[Shouting and pet commands (not implemented)],SHOUT,no)
NETHACK_ARG(display-layers,[Support transparent tilesets],DISPLAY_LAYERS,yes)
NETHACK_ARG(goldobj,[Gold is kept on object chains (experimental)],GOLDOBJ,no)
NETHACK_ARG(autopickup-exceptions,[Exceptions to autopickup (experimental)],
  AUTOPICKUP_EXCEPTIONS,no)

AC_MSG_CHECKING([whether to enable wizmode])
AC_ARG_ENABLE([wizmode], AC_HELP_STRING([--enable-wizmode=USER],
	[enable wizard mode for the specified user (default=wizard)]),
	if test "$enableval" != no; then enable_wizmode=yes; wizard=$enableval;
	else enable_wizmode=no; fi, enable_wizmode=yes; wizard="wizard")
if test "$enable_wizmode" != "no" ; then
	AC_MSG_RESULT([yes (user $wizard)])
	AC_DEFINE_UNQUOTED([WIZARD], ["$wizard"], [The person allowed to use the -D option])
else
	AC_MSG_RESULT([no])
fi


NETHACK_LINKS=
NETHACK_LINKS_ADD([include/file.h])
NETHACK_LINKS_ADD([dat/data.base])
NETHACK_LINKS_ADD([dat/rumors.tru])
NETHACK_LINKS_ADD([dat/rumors.fal])
NETHACK_LINKS_ADD([dat/quest.txt])
NETHACK_LINKS_ADD([dat/oracles.txt])
NETHACK_LINKS_ADD([dat/dungeon.def])
NETHACK_LINKS_ADD([dat/help])
NETHACK_LINKS_ADD([dat/hh])
NETHACK_LINKS_ADD([dat/cmdhelp])
NETHACK_LINKS_ADD([dat/history])
NETHACK_LINKS_ADD([dat/opthelp])
NETHACK_LINKS_ADD([dat/wizhelp])
NETHACK_LINKS_ADD([dat/gypsy.txt])
NETHACK_LINKS_ADD([win/share/tile_t.h])

AC_CONFIG_LINKS(${NETHACK_LINKS})

dnl  ----- Process the Makefiles
AC_CONFIG_FILES([Makefile:sys/autoconf/Makefile.top]
	[src/Makefile:sys/autoconf/Makefile.src]
	[doc/Makefile:sys/autoconf/Makefile.doc]
	[dat/Makefile:sys/autoconf/Makefile.dat]
	[util/Makefile:sys/autoconf/Makefile.utl])

NETHACK_CONFIG_HEADER([include/autoconf.h:sys/autoconf/autoconf.hin])

AC_OUTPUT
