#	NetHack Makefile.
#	SCCS Id: @(#)Makefile.src	3.4	2002/03/02

# newer makes predefine $(MAKE) to 'make' and do smarter processing of
# recursive make calls if $(MAKE) is used
# these makes allow $(MAKE) to be overridden by the environment if someone
# wants to (or has to) use something other than the standard make, so we do
# not want to unconditionally set $(MAKE) here
#
# unfortunately, some older makes do not predefine $(MAKE); if you have one of
# these, uncomment the following line
# (you will know that you have one if you get complaints about being unable
# to find 'makedefs')
# MAKE = make

# This makefile replaces the previous Makefile.unix, Makefile.xenix,
# Makefile.3B2, Makefile.att, and Makefile.tos.
# Set SYSTEM to one of:
#	'Sysunix'	-- generic UNIX
#	'Sys3B2'	-- AT&T 3B2, 3B5, etc.
#	'Sysatt'	-- AT&T UNIXPC, 7300, 3B1
#	'SysV-AT'	-- Microport 286 UNIX (put -DDUMB in CFLAGS)
#	'Systos'	-- Atari
#	'SysBe'		-- BeOS
SYSTEM = Sysunix

#
# Make sure that your bourne shell is specified here, as you have to spawn
# some of the commands (eg. depend) in bourne shell for them to work.
#
# For Systos users compiling on the ST, you'll either need a bourne shell
# clone or you'll need to do make depend, etc. by hand. In either case,
# the line below probably needs changing
SHELL=/bin/sh
# for Atari
# SHELL=E:/GEMINI2/MUPFEL.TTP

# Normally, the C compiler driver is used for linking:
LINK=$(CC)

# Pick the SYSSRC and SYSOBJ lines corresponding to your desired operating
# system.
#
# for UNIX systems
SYSSRC = ../sys/share/ioctl.c ../sys/share/unixtty.c ../sys/unix/unixmain.c \
	../sys/unix/unixunix.c ../sys/unix/unixres.c
SYSOBJ = ioctl.o unixmain.o unixtty.o unixunix.o unixres.o
#
# for Systos
# SYSSRC = ../sys/atari/tos.c ../sys/share/pcmain.c ../sys/share/pcsys.c \
#	../sys/share/pctty.c ../sys/share/pcunix.c
# SYSOBJ = tos.o pcmain.o pcsys.o pctty.o pcunix.o
#
# for BeOS
#SYSSRC = ../sys/be/bemain.c ../sys/share/unixtty.c ../sys/share/ioctl.c
#SYSOBJ = bemain.o unixtty.o ioctl.o


# if you are using gcc as your compiler:
#	uncomment the CC definition below if it's not in your environment
#	if you get setcgtty() warnings during execution, you are feeding gcc
#		a non-ANSI <sys/ioctl.h> -- either run fixincludes on it or use
#		-traditional in CFLAGS
# CC = gcc
#
#	For Bull DPX/2 systems at B.O.S. 2.0 or higher use the following:
#
# CC = gcc -ansi -D_BULL_SOURCE -D_XOPEN_SOURCE -D_POSIX_SOURCE
#
#	If you are using GCC 2.2.2 or higher on a DPX/2, just use:
#
# CC = gcc -ansi
#
#	For HP/UX 10.20 with GCC:
# CC = gcc -D_POSIX_SOURCE
#
#	For cross-compiling, eg. with gcc on Linux (see also CXX further down):
# CC = arm-linux-gcc
#
#
# if you're debugging and want gcc to check as much as possible, use:
# CC = gcc -W -Wimplicit -Wreturn-type -Wunused -Wformat -Wswitch -Wshadow -Wcast-qual -Wwrite-strings -DGCC_WARN

# flags may have to be changed as required
# flags for 286 Xenix:
# CFLAGS = -Ml2t16 -O -LARGE -I../include
# LFLAGS = -Ml -F 4000 -SEG 512

# flags for 286 Microport SysV-AT
# CFLAGS = -DDUMB -Ml -I../include
# LFLAGS = -Ml

# flags for Atari gcc (3.2.1)
# CFLAGS = -O -I../include
# LFLAGS = -s
# flags for Atari gcc (3.3)
# CFLAGS = -mshort -O2 -fomit-frame-pointer -I../include
# LFLAGS = -mshort -s

# flags for AIX 3.1 cc on IBM RS/6000 to define
# a suitable subset of standard libraries
# (note that there is more info regarding the "-qchars=signed"
# switch in file Install.unx note 8)
# CFLAGS = -D_NO_PROTO -D_XOPEN_SOURCE -O -I../include -qchars=signed
#
# Some of our subroutines are complex enough that this is required for full
# optimization under AIX 3.2 (I don't know about 3.1).
#
# CFLAGS = -D_NO_PROTO -D_XOPEN_SOURCE -D_ALL_SOURCE -O -I../include -qchars=signed -qmaxmem=5000

# flags for A/UX 2.01 using native cc or c89
# gcc predefines AUX so that's not needed there
# Remember to use -lcurses for WINLIB below !
# CFLAGS = -ZS -D_POSIX_SOURCE -O -I../include -DAUX

# flags for IRIX 4.0.x using native cc
# The include files are __STDC__, but have bugs involving const
# CFLAGS = -O -I../include -D__STDC__ -Dconst= -woff 100,293
# LFLAGS = -s

# flags for BSD/OS 2.0
# CFLAGS = -O -I../include -I/usr/X11/include
# LFLAGS = -L/usr/X11/lib

# flags for Linux
#   compile normally
# CFLAGS = -O2 -fomit-frame-pointer -I../include
# LFLAGS = -L/usr/X11R6/lib
#   OR compile backwards compatible a.out format
# CFLAGS = -O2 -b i486-linuxaout -fomit-frame-pointer -I../include
# LFLAGS = -b i486-linuxaout -L/usr/X11R6/lib

# flags for BeOS
#   on a Mac/BeBox:
#CC = mwcc
#CFLAGS = -r -I../include
#LINK = mwld
#LFLAGS = -map nethack.xMAP
#   on Intel:
#CFLAGS = -O -I../include
#LINK = gcc
#LFLAGS = -Xlinker -soname=_APP_

# flags for debugging:
# CFLAGS = -g -I../include

CFLAGS = -O -I../include
LFLAGS =

# Only used for the Gnome interface.
# When including the Gnome interface, you need to include gnome specific
# directories.  The ones given below is the usual spot for linux systems.
# The paths are for glibconfig.h and gnomesupport.h respectively.
#
#GNOMEINC=-I/usr/lib/glib/include -I/usr/lib/gnome-libs/include -I../win/gnome
GNOMEINC=`gnome-config gnome --cflags` -I../win/gnome

# QT windowing system
# Gentoo
# QTDIR = /usr/qt/2
# SuSE 6.1
# QTDIR = /usr/lib/qt
# RedHat 6.1
# QTDIR = /usr/lib/qt-2.0.1
#
QTLIBDIR=$(QTDIR)/lib
QTINCDIR=$(QTDIR)/include
QTBINDIR=$(QTDIR)/bin
# SuSE 6.1
# KDELIBDIR=/opt/kde/lib
# KDEINCDIR=/opt/kde/include
# RedHat 6.1
KDELIBDIR=/usr/lib
KDEINCDIR=/usr/include/kde
# CFLAGS = -O -I../include
# LFLAGS = -L/usr/X11/lib -L$(QTLIBDIR) -L$(KDELIBDIR)

# The Qt and Be window systems are written in C++, while the rest of
# NetHack is standard C.  If using Qt, uncomment the LINK line here to get
# the C++ libraries linked in.
CXXFLAGS = $(CFLAGS) -I. -I$(QTINCDIR)
# Qt is C++, so define the compiler and linker thus required:
CXX=g++
#LINK=g++
#	For cross-compiling, eg. with gcc on Linux (see also CC further up):
#CXX=arm-linux-g++
#LINK=arm-linux-gcc

# GTK windowing system
WINGTKCONFIG=PKG_CONFIG_PATH=/usr/devel/lib/pkgconfig pkg-config gtk+-2.0
WINGTKCFLAGS=`$(WINGTKCONFIG) --cflags`

# Set the WINSRC, WINOBJ, and WINLIB lines to correspond to your desired
# combination of windowing systems.  Also set windowing systems in config.h.
#
# files for a straight tty port using no native windowing system
WINTTYSRC = ../win/tty/getline.c ../win/tty/termcap.c ../win/tty/topl.c \
	../win/tty/wintty.c 
WINTTYOBJ = getline.o termcap.o topl.o wintty.o 
#
# files for an X11 port
# (tile.c is included in the defn. for WINCSRC, below)
WINX11SRC = ../win/X11/Window.c ../win/X11/dialogs.c ../win/X11/winX.c \
	../win/X11/winmap.c  ../win/X11/winmenu.c ../win/X11/winmesg.c \
	../win/X11/winmisc.c ../win/X11/winstat.c ../win/X11/wintext.c \
	../win/X11/winval.c
WINX11OBJ = Window.o dialogs.o winX.o winmap.o winmenu.o winmesg.o \
	winmisc.o winstat.o wintext.o winval.o nhwin.a
#
# Files for a Qt port
#
WINQTSRC = ../win/Qt/qt_win.cpp ../win/Qt/qt_clust.cpp ../win/Qt/qttableview.cpp
WINQTOBJ = qt_win.o qt_clust.o qttableview.o nhwin.a
#
# files for a GTK port
#
WINGTKSRC = ../win/gtk/gtk.c ../win/gtk/gtkgetlin.c \
	../win/gtk/gtkstatus.c ../win/gtk/gtkmenu.c ../win/gtk/gtkyn.c \
	../win/gtk/gtkextcmd.c ../win/gtk/gtkmap.c ../win/gtk/gtkmessage.c \
	../win/gtk/gtkmisc.c ../win/gtk/gtktile.c ../win/gtk/xshmmap.c \
	../win/gtk/gtkhackrc.c ../win/gtk/gtkprefs.c ../win/gtk/gtkprogress.c \
	../win/gtk/gtkext.c ../win/gtk/gtkint.c
WINGTKOBJ = gtk.o gtkgetlin.o gtkstatus.o gtkmenu.o gtkyn.o gtkextcmd.o \
 	gtkmap.o gtktile.o gtkmessage.o gtkmisc.o xshmmap.o gtkhackrc.o \
	gtkprefs.o gtkprogress.o gtkext.o gtkint.o nhwin.a
#
# Files for a Gnome port
#
WINGNOMESRC = ../win/gnome/gnaskstr.c ../win/gnome/gnbind.c \
      ../win/gnome/gnglyph.c ../win/gnome/gnmain.c ../win/gnome/gnmap.c \
      ../win/gnome/gnmenu.c ../win/gnome/gnmesg.c ../win/gnome/gnopts.c \
      ../win/gnome/gnplayer.c ../win/gnome/gnsignal.c \
      ../win/gnome/gnstatus.c ../win/gnome/gntext.c ../win/gnome/gnyesno.c \
      ../win/gnome/gnworn.c
WINGNOMEOBJ = gnaskstr.o gnbind.o gnglyph.o gnmain.o gnmap.o gnmenu.o \
      gnmesg.o gnopts.o gnplayer.o gnsignal.o gnstatus.o gntext.o \
      gnyesno.o gnworn.o nhwin.a
#
# Files for a Gem port
WINGEMSRC = ../win/gem/wingem.c ../win/gem/wingem1.c ../win/gem/load_img.c \
	../win/gem/gr_rect.c
WINGEMOBJ = wingem.o wingem1.o load_img.o gr_rect.o nhwin.a
#
# Files for a BeOS InterfaceKit port -- not ready for prime time
WINBESRC =
WINBEOBJ =
#WINBESRC = ../win/BeOS/winbe.cpp ../win/BeOS/NHWindow.cpp \
#     ../win/BeOS/NHMenuWindow.cpp ../win/BeOS/NHMapWindow.cpp
#WINBEOBJ = winbe.o NHWindow.o NHMenuWindow.o NHMapWindow.o nhwin.a
#
# Files for a proxy interface
WINPROXYSRC = ../win/proxy/winproxy.c ../win/proxy/callback.c
WINPROXYOBJ = winproxy.o callback.o nhwin.a
#
# Common files for proxy window interface and plug-in window interfaces.
PROXYCOMSRC = ../win/proxy/nhext.c ../win/proxy/nhxdr.c \
	../win/proxy/nhextxdr.c ../win/proxy/nhextio.c ../win/proxy/nhextnb.c \
	../win/proxy/compxdr.c
PROXYCOMOBJ = nhext.o nhxdr.o nhextxdr.o nhextio.o nhextnb.o compxdr.o
#
# Common files that are also shared with non-proxy window interfaces
PROXYSHRSRC = ../win/proxy/md5.c
PROXYSHROBJ = md5.o
#
# Files for the proxy utility modules. These are provided for use with window
# ports that support the proxy interface. The GTK window interface needs
# these files regardless of whether proxy support is enabled or not.
PROXYUTLSRC = ../win/proxy/dlbh.c ../win/proxy/mapid.c \
	../win/proxy/riputil.c ../win/proxy/getopt.c ../win/proxy/glyphmap.c
PROXYUTLOBJ = dlbh.o mapid.o riputil.o getopt.o glyphmap.o
#
# Files for plug-in window interfaces. These are needed to support external
# window ports and provide the interface to the game executable.  Currently,
# only the GTK window interface supports plug-in window interfaces.
PROXYCLNTSRC = ../win/proxy/proxysvc.c ../win/proxy/proxycb.c \
	../win/proxy/prxymap.c ../win/proxy/prxychar.c ../win/proxy/prxytile.c \
	../win/proxy/prxyconn.c
PROXYCLNTOBJ = proxysvc.o proxycb.o prxymap.o prxychar.o prxytile.o prxyconn.o
#
# Files for the SDL/GL window port
WINGLSRC = ../win/gl/gl_conf.c ../win/gl/gl_emul.c  \
    ../win/gl/gl_font.c ../win/gl/gl_hardw.c ../win/gl/gl_image.c  \
    ../win/gl/gl_main.c ../win/gl/gl_map.c ../win/gl/gl_menu.c  \
    ../win/gl/gl_opt.c ../win/gl/gl_role.c ../win/gl/gl_rendu.c  \
    ../win/gl/gl_softw.c ../win/gl/gl_stat.c ../win/gl/gl_text.c  \
    ../win/gl/gl_tile.c  ../win/gl/gl_unit.c ../win/gl/gl_win.c
WINGLOBJ = gl_conf.o gl_emul.o gl_font.o gl_hardw.o \
    gl_image.o gl_main.o gl_map.o gl_menu.o gl_opt.o \
    gl_rendu.o gl_role.o gl_softw.o gl_stat.o \
    gl_text.o gl_tile.o gl_unit.o gl_win.o nhwin.a
SDLGL_CFLAGS = `sdl-config --cflags`

# Use these declarations if you only want to support the TTY windowing system
WINSRC = $(WINTTYSRC)
WINOBJ = $(WINTTYOBJ)
# Use these declarations if you want to support the X11 windowing system
# WINSRC = $(WINTTYSRC) $(WINX11SRC)
# WINOBJ = $(WINTTYOBJ) $(WINX11OBJ)
# Use these declarations if you want to support the Qt windowing system
# WINSRC = $(WINTTYSRC) $(WINQTSRC)
# WINOBJ = $(WINTTYOBJ) $(WINQTOBJ)
# Use these declarations if you want to support a conventional
# GTK windowing system (only useful if not installing setuid or setgid)
# WINSRC = $(WINTTYSRC) $(WINGTKSRC) $(PROXYUTLSRC)
# WINOBJ = $(WINTTYOBJ) $(WINGTKOBJ) $(PROXYUTLOBJ)
# Use these declarations if you want to support external windowing systems
# and a proxified GTK windowing system (see Install.GTK)
# WINSRC = $(WINTTYSRC) $(WINGTKSRC) \
#	$(WINPROXYSRC) $(PROXYUTLSRC) $(PROXYCOMSRC) $(PROXYCLNTSRC)
# WINOBJ = $(WINTTYOBJ) $(WINGTKOBJ) \
#	$(WINPROXYOBJ) $(PROXYUTLOBJ) $(PROXYCOMOBJ) $(PROXYCLNTOBJ)
# Use these declarations to only support the SDL/GL windowing systems
# WINSRC = $(WINGLSRC)
# WINOBJ = $(WINGLOBJ)
# Use these declarations if you want to support external windowing systems
# WINSRC = $(WINTTYSRC) $(WINPROXYSRC) $(PROXYUTLSRC) $(PROXYCOMSRC)
# WINOBJ = $(WINTTYOBJ) $(WINPROXYOBJ) $(PROXYUTLOBJ) $(PROXYCOMOBJ)
# Use these declarations if you want to support all standard windowing systems
# WINSRC = $(WINTTYSRC) $(WINPROXYSRC) $(PROXYUTLSRC) $(PROXYCOMSRC) \
#	$(WINX11SRC) $(WINQTSRC) $(WINGTKSRC) $(WINGLSRC) $(WINGNOMESRC)
# WINOBJ = $(WINTTYOBJ) $(WINPROXYOBJ) $(PROXYUTLOBJ) $(PROXYCOMOBJ) \
#	$(WINX11OBJ) $(WINQTOBJ) $(WINGTKOBJ) $(WINGLOBJ) $(WINGNOMEOBJ)
#
# You must also change the definition of WINLIB (below) to match these

# on some systems the termcap library is in -ltermcap or -lcurses
# on 386 Xenix, the -ltermlib tputs() seems not to work; use -lcurses instead
# Sysatt uses shared library in lieu of this option
# Systos needs -lcurses16 if you use -mshort
# AIX 3.1 on RS/6000 likes -lcurses if TERMINFO defined in unixconf.h
# and -ltermcap otherwise
# Linux uses -lncurses (newer) or -ltermcap (older)
# Be uses -ltermcap
#
# libraries for tty ports
# WINTTYLIB = -ltermcap
# WINTTYLIB = -lcurses
# WINTTYLIB = -lcurses16
# WINTTYLIB = -lncurses
WINTTYLIB = -ltermlib
#
# libraries for X11
# If USE_XPM is defined in config.h, you will also need -lXpm here.
WINX11LIB = -lXaw -lXmu -lXext -lXt -lX11
# WINX11LIB = -lXaw -lXmu -lXt -lX11
# WINX11LIB = -lXaw -lXmu -lXext -lXt -lXpm -lX11 -lm
# WINX11LIB = -lXaw -lXmu -lXpm -lXext -lXt -lX11 -lSM -lICE -lm # BSD/OS 2.0
#
# libraries for Qt
WINQTLIB = -L$(QTLIBDIR) -lqt
#
# libraries for KDE (with Qt)
WINKDELIB = -lkdecore -lkdeui -lXext
#
# libraries for GTK
WINGTKLIB = `$(WINGTKCONFIG) --libs`
#
# libraries for Gnome
WINGNOMELIB = -lgnomeui -lgnome -lart_lgpl -lgtk -lgdk -lpopt
#
# libraries for Gem port
WINGEMLIB = -le_gem -lgem
#
# libraries for BeOS
WINBELIB = -lbe
#
# libraries for SDL/GL window port
WINSDLLIB = `sdl-config --libs` -lz -lpng
WINGLLIB = -lGL
#
# libraries for proxy
WINPROXYLIB = -lpthread

# Use this declaration if you only want to support the TTY windowing system
WINLIB = $(WINTTYLIB)
# Use this declaration if you want to support the X11 windowing system
# WINLIB = $(WINTTYLIB) $(WINX11LIB)
# Use this declaration if you want to support the Qt windowing system
# WINLIB = $(WINTTYLIB) $(WINQTLIB)
# Use this declaration if you want to support a conventional
# GTK windowing system (only useful if not installing setuid or setgid)
# WINLIB = $(WINTTYLIB) $(WINGTKLIB)
# Use this declaration if you want to support external windowing systems
# and a proxified GTK windowing system (see Install.GTK)
# WINLIB = $(WINTTYLIB) $(WINGTKLIB) $(WINPROXYLIB)
# Use this declaration to only support the SDL/GL windowing system
# WINLIB = $(WINGLLIB)
# Use this declaration if you want to support external windowing systems
# WINLIB = $(WINTTYLIB) $(WINPROXYLIB)
# Use this declaration if you want to support all standard windowing systems
# WINLIB = $(WINTTYLIB) $(WINPROXYLIB) $(WINGTKLIB) \
#	$(WINX11LIB) $(WINQTLIB) $(WINGNOMELIB) $(WINSDLLIB) $(WINGLLIB) 

# any other strange libraries your system needs (for Sysunix only -- the more
# specialized targets should already be right)
#
# on HP-UX 8.x, the malloc(3x) routines in libmalloc.a seem to align things
# better than the malloc(3) ones in libc.a
# LIBS = -lmalloc
#
# DPX/2's also use the malloc(3x) routines.  In addition, if you are building
# for X11, you must include libinet.a.
# LIBS = -lmalloc -linet
#
# Linux NetHack uses some bsd style ioctl functions, thus it is necessary to
# use the bsd libs.  (Only if still compiling as BSD in unixconf.h; recent
# versions compile fine using SYSV without this.)
# LIBS = -lbsd
#
# for CYGWIN32 aka cygwin 1.1.1
# LIBS = -lcygwin
#
# Solaris 2.x seems to work with the following
# LIBS = -lsocket -lnsl
#
# IRIX 4.0.x needs -lsun if NIS (YP) is being used for passwd file lookup
# LIBS = -lsun
#
LIBS =

# make NetHack
GAME     = slashem
# GAME     = slashem.prg

# if you defined RANDOM in unixconf.h/tosconf.h since your system did not come
# with a reasonable random number generator
# RANDOBJ = random.o
RANDOBJ =


# used by `make depend' to reconstruct this Makefile; you shouldn't need this
AWK	= nawk

# ----------------------------------------
#
# Nothing below this line should have to be changed.
#
# Other things that have to be reconfigured are in config.h,
# {unixconf.h, pcconf.h, tosconf.h}, and possibly system.h

MAKEDEFS = ../util/makedefs

# timestamp files to reduce `make' overhead and shorten .o dependency lists
CONFIG_H = ../src/config.h-t
HACK_H	= ../src/hack.h-t

# all .c that are part of the main NetHack program and are not operating- or
# windowing-system specific
HACKCSRC = allmain.c alloc.c apply.c artifact.c attrib.c ball.c bones.c \
	   botl.c cmd.c dbridge.c decl.c detect.c dig.c display.c dlb.c do.c \
	   do_name.c do_wear.c dog.c dogmove.c dokick.c dothrow.c drawing.c \
	   dungeon.c eat.c end.c engrave.c exper.c explode.c extralev.c \
	   files.c fountain.c hack.c hacklib.c invent.c light.c lock.c \
	   mail.c makemon.c mapglyph.c mcastu.c mhitm.c mhitu.c minion.c \
	   mklev.c mkmap.c \
	   mkmaze.c mkobj.c mkroom.c mon.c mondata.c monmove.c monst.c \
	   mplayer.c mthrowu.c muse.c music.c o_init.c objects.c objnam.c \
	   options.c pager.c pickup.c pline.c polyself.c potion.c pray.c \
	   priest.c quest.c questpgr.c read.c rect.c region.c restore.c rip.c \
	   rnd.c role.c rumors.c save.c shk.c shknam.c sit.c sounds.c sp_lev.c \
	   spell.c steal.c steed.c teleport.c timeout.c topten.c track.c trap.c \
	   u_init.c uhitm.c vault.c version.c vision.c weapon.c were.c wield.c \
	   windows.c wizard.c worm.c worn.c write.c zap.c \
	   gypsy.c tech.c

# all operating-system-dependent .c (for dependencies and such)
SYSCSRC = ../sys/atari/tos.c ../sys/share/pcmain.c ../sys/share/pcsys.c \
	../sys/share/pctty.c ../sys/share/pcunix.c ../sys/share/random.c \
	../sys/share/ioctl.c ../sys/share/unixtty.c ../sys/unix/unixmain.c \
	../sys/unix/unixunix.c ../sys/unix/unixres.c ../sys/be/bemain.c

# generated source files (tile.c is handled separately)
GENCSRC = monstr.c vis_tab.c  #tile.c

# all windowing-system-dependent .c (for dependencies and such)
WINCSRC = $(WINTTYSRC) $(WINX11SRC) $(WINGTKSRC) $(WINGNOMESRC) $(WINGEMSRC) \
	$(WINPROXYSRC) $(PROXYUTLSRC) $(PROXYCLNTSRC) $(PROXYCOMSRC) \
	$(PROXYSHRSRC) $(WINGLSRC) tile.c
# all windowing-system-dependent .cpp (for dependencies and such)
WINCXXSRC = $(WINQTSRC) $(WINBESRC)

# .c files for this version (for date.h)
VERSOURCES = $(HACKCSRC) $(SYSSRC) $(WINSRC) $(GENCSRC)

# .c files for all versions using this Makefile (for lint and tags)
CSOURCES = $(HACKCSRC) $(SYSSRC) $(WINCSRC) $(GENCSRC)


# all .h files except date.h, onames.h, pm.h, and vis_tab.h which would
# cause dependency loops if run through "make depend"
# and dgn_comp.h, dgn_file.h, lev_comp.h, special level & dungeon files.
#
HACKINCL = align.h amiconf.h artifact.h artilist.h attrib.h beconf.h color.h \
	config.h config1.h coord.h decl.h def_os2.h display.h dlb.h dungeon.h \
	edog.h emin.h engrave.h epri.h eshk.h extern.h flag.h func_tab.h \
	global.h hack.h lev.h macconf.h mfndpos.h micro.h mkroom.h \
	monattk.h mondata.h monflag.h monst.h monsym.h obj.h objclass.h \
	os2conf.h patchlevel.h pcconf.h permonst.h prop.h rect.h region.h rm.h \
	sp_lev.h spell.h system.h tcap.h timeout.h tosconf.h tradstdc.h \
	trampoli.h trap.h unixconf.h vault.h vision.h vmsconf.h wintty.h \
	winX.h winprocs.h wintype.h you.h youprop.h 

HSOURCES = $(HACKINCL) date.h onames.h pm.h vis_tab.h\
		lev_comp.h dgn_comp.h dgn_file.h

# the following .o's _must_ be made before any others (for makedefs)
FIRSTOBJ = monst.o objects.o alloc.o

HOBJ = $(FIRSTOBJ) allmain.o apply.o artifact.o attrib.o ball.o \
	bones.o botl.o cmd.o dbridge.o decl.o detect.o dig.o display.o dlb.o \
	do.o do_name.o do_wear.o dog.o dogmove.o dokick.o dothrow.o \
	drawing.o dungeon.o eat.o end.o engrave.o exper.o explode.o \
	extralev.o files.o fountain.o hack.o hacklib.o invent.o light.o \
	lock.o mail.o makemon.o mapglyph.o mcastu.o mhitm.o mhitu.o \
	minion.o mklev.o mkmap.o \
	mkmaze.o mkobj.o mkroom.o mon.o mondata.o monmove.o monstr.o \
	mplayer.o mthrowu.o muse.o music.o o_init.o objnam.o options.o \
	pager.o pickup.o pline.o polyself.o potion.o pray.o priest.o \
	quest.o questpgr.o read.o rect.o region.o restore.o rip.o rnd.o \
	role.o rumors.o save.o shk.o shknam.o sit.o sounds.o sp_lev.o spell.o \
	steal.o steed.o teleport.o timeout.o topten.o track.o trap.o u_init.o \
	uhitm.o vault.o vision.o vis_tab.o weapon.o were.o wield.o windows.o \
	wizard.o worm.o worn.o write.o zap.o \
	gypsy.o tech.o \
	$(RANDOBJ) $(SYSOBJ) $(WINOBJ) version.o
# the .o files from the HACKCSRC, SYSSRC, and WINSRC lists

$(GAME):	$(SYSTEM)
	@echo "$(GAME) is up to date."

Sysunix:	$(HOBJ) Makefile
	@echo "Loading ..."
	@$(LINK) $(LFLAGS) -o $(GAME) $(HOBJ) $(WINLIB) $(LIBS)
	@touch Sysunix

Sys3B2:	$(HOBJ) Makefile
	@echo "Loading ..."
	@$(LINK) $(LFLAGS) -o $(GAME) $(HOBJ) $(WINLIB) -lmalloc
	@touch Sys3B2

Sysatt:	$(HOBJ) Makefile
	@echo "Loading ..."
	@$(LD) $(LFLAGS) /lib/crt0s.o /lib/shlib.ifile -o $(GAME) $(HOBJ)
	@touch Sysatt

Systos:	$(HOBJ) Makefile
	@echo "Loading ..."
	@$(LINK) $(LFLAGS) -o $(GAME) $(HOBJ) $(WINLIB)
	@touch Systos

SysV-AT:	DUMB.Setup $(HOBJ) Makefile
	@echo "Loading ..."
	@$(LINK) $(LFLAGS) -o $(GAME) $(HOBJ) $(WINLIB)
	@touch SysV-AT

SysBe: $(HOBJ) Makefile
	@echo "Loading ..."
	@$(LINK) $(LFLAGS) -o $(GAME) $(HOBJ) $(WINLIB) $(LIBS)
	@xres -o $(GAME) ../win/BeOS/nethack.rsrc
	@mimeset -f $(GAME)
	@touch SysBe

DUMB.Setup:	../include/extern.h
	cp ../include/extern.h ../include/extern.h.BAK
	cat ../include/extern.h | \
		sed -e '/^E\ int\ /!b' \
			-e '/[^;/ 	]$$/N' \
			-e '/[(][*]occupation[)]/b' \
			-e '/[(][*]afternmv[)]/b' \
			-e '/float_down/b' \
			-e '/done1/b' \
			-e '/identify/b' \
			-e '/Hear_again/b' \
			-e '/hangup/b' \
			-e 's/^\(.*\)$$/\/\* \1 \/\*\*\//' | \
		sed -e '/^E\ void\ /!b' \
			-e '/[^;/ 	]$$/N' \
			-e 's/^\(.*\)$$/\/\* \1 \/\*\*\//' \
				>../include/extern.DUMB
	cp ../include/extern.DUMB ../include/extern.h
	@touch DUMB.Setup

all:	$(GAME)

#	dependencies for makedefs and its outputs, which the util
#	Makefile is responsible for keeping up to date
#

# special rules, to force update of makedefs, real dependencies should be
# below in the 'make depend' output.
monst.o:
	$(CC) $(CFLAGS) -c monst.c
	@rm -f $(MAKEDEFS)

objects.o:
	$(CC) $(CFLAGS) -c objects.c
	@rm -f $(MAKEDEFS)

../include/filename.h: $(MAKEDEFS)
	../util/makedefs -f

# Qt windowport meta-object-compiler output
qt_kde0.moc: ../include/qt_kde0.h
	$(QTBINDIR)/moc -o qt_kde0.moc ../include/qt_kde0.h

qt_win.moc: ../include/qt_win.h
	$(QTBINDIR)/moc -o qt_win.moc ../include/qt_win.h

qttableview.moc: ../include/qttableview.h
	$(QTDIR)/bin/moc -o qttableview.moc ../include/qttableview.h

# NetHack windows library for modules needed by more than one windows
# interface.
nhwin.a:	tile.o $(PROXYSHROBJ)
	$(AR) cr nhwin.a tile.o $(PROXYSHROBJ)

# NetHack proxy library for external window interfaces.
nhproxy.a:      $(PROXYCLNTOBJ) $(PROXYCOMOBJ) $(PROXYSHROBJ)
	$(AR) cr nhproxy.a $(PROXYCLNTOBJ) $(PROXYCOMOBJ) $(PROXYSHROBJ)

$(MAKEDEFS): ../util/makedefs.c  $(CONFIG_H) ../include/permonst.h \
		../include/objclass.h ../include/monsym.h \
		../include/artilist.h ../include/dungeon.h ../include/obj.h \
		../include/monst.h ../include/you.h ../include/flag.h \
		../include/dlb.h ../include/patchlevel.h ../include/qtext.h
	@( cd ../util ; $(MAKE) makedefs)

../include/onames.h: $(MAKEDEFS)
	@( cd ../util ; $(MAKE) ../include/onames.h )
../include/pm.h: $(MAKEDEFS)
	@( cd ../util ; $(MAKE) ../include/pm.h )
monstr.c: $(MAKEDEFS)
	@( cd ../util ; $(MAKE) ../src/monstr.c )
../include/vis_tab.h: $(MAKEDEFS)
	@( cd ../util ; $(MAKE) ../include/vis_tab.h )
# makedefs -z makes both vis_tab.h and vis_tab.c, but writes the .h first
vis_tab.c: ../include/vis_tab.h
tile.c: ../win/share/tilemap.c $(HACK_H)
	@( cd ../util ; $(MAKE) ../src/tile.c )

../win/gnome/gn_rip.h: ../win/X11/rip.xpm
	cp ../win/X11/rip.xpm ../win/gnome/gn_rip.h

#	date.h should be remade any time any of the source or include code
#	is modified.  Unfortunately, this would make the contents of this
#	file far more complex.  Since "hack.h" depends on most of the include
#	files, we kludge around this by making date.h dependent on hack.h,
#	even though it doesn't include this file.
#
#	hack.h depends on makedefs' output, so we know makedefs will be
#	up to date before being executed
../include/date.h:	$(VERSOURCES) $(HACK_H)
	../util/makedefs -v


lint:
# lint cannot have -p here because (i) capitals are meaningful:
# [Ww]izard, (ii) identifiers may coincide in the first six places:
# doweararm() versus dowearring().
# _flsbuf comes from <stdio.h>, a bug in the system libraries.
	@echo lint -axbh -DLINT ...
	@lint -axbh -I../include -DLINT $(CSOURCES) | sed '/_flsbuf/d'


tags: $(CSOURCES)
	@echo ctags -tw ...
	@ctags -tw $(CSOURCES)
	@( cd ../include ; ctags -tw $(HSOURCES) )
	@( cd ../util ; $(MAKE) tags )

clean:
	-rm -f *.o nhwin.a $(HACK_H) $(CONFIG_H)

spotless: clean
	-rm -f a.out core $(GAME) Sys*
	-rm -f ../include/date.h ../include/onames.h ../include/pm.h
	-rm -f monstr.c ../include/vis_tab.h vis_tab.c tile.c *.moc
	-rm -f ../win/gnome/gn_rip.h
	-rm -f ../include/filename.h

depend: ../sys/unix/depend.awk \
		$(SYSCSRC) $(WINCSRC) $(WINCXXSRC) $(GENCSRC) $(HACKCSRC)
	$(AWK) -f ../sys/unix/depend.awk ../include/*.h \
		$(SYSCSRC) $(WINCSRC) $(WINCXXSRC) $(GENCSRC) $(HACKCSRC) >makedep
	@echo '/^# DO NOT DELETE THIS LINE OR CHANGE ANYTHING BEYOND IT/+2,$$d' >eddep
	@echo '$$r makedep' >>eddep
	@echo 'w' >>eddep
	@cp Makefile Makefile.bak
	ed - Makefile < eddep
	@rm -f eddep makedep
	@echo '# DEPENDENCIES MUST END AT END OF FILE' >> Makefile
	@echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY' >> Makefile
	@echo '# see make depend above' >> Makefile
	- diff Makefile.bak Makefile
	@rm -f Makefile.bak

# DO NOT DELETE THIS LINE OR CHANGE ANYTHING BEYOND IT

# config.h timestamp
$(CONFIG_H): ../include/config.h ../include/config1.h ../include/tradstdc.h \
		../include/global.h ../include/coord.h ../include/vmsconf.h \
		../include/system.h ../include/unixconf.h ../include/os2conf.h \
		../include/micro.h ../include/pcconf.h ../include/tosconf.h \
		../include/amiconf.h ../include/macconf.h ../include/beconf.h \
		../include/wceconf.h ../include/ntconf.h ../include/nhlan.h
	touch $(CONFIG_H)
# hack.h timestamp
$(HACK_H): ../include/hack.h $(CONFIG_H) ../include/align.h \
		../include/dungeon.h ../include/monsym.h ../include/mkroom.h \
		../include/objclass.h ../include/youprop.h ../include/prop.h \
		../include/permonst.h ../include/monattk.h \
		../include/monflag.h ../include/mondata.h ../include/pm.h \
		../include/wintype.h ../include/decl.h ../include/quest.h \
		../include/obj.h ../include/spell.h ../include/tech.h \
		../include/you.h ../include/attrib.h ../include/monst.h \
		../include/skills.h ../include/onames.h ../include/color.h \
		../include/timeout.h ../include/trap.h ../include/flag.h \
		../include/rm.h ../include/vision.h ../include/display.h \
		../include/engrave.h ../include/rect.h ../include/region.h \
		../include/winprocs.h ../include/wintty.h ../include/trampoli.h
	touch $(HACK_H)
#
tos.o: ../sys/atari/tos.c $(HACK_H) ../include/tcap.h
	$(CC) $(CFLAGS) -c ../sys/atari/tos.c
pcmain.o: ../sys/share/pcmain.c $(HACK_H) ../include/dlb.h \
		../include/patchlevel.h #../include/win32api.h
	$(CC) $(CFLAGS) -c ../sys/share/pcmain.c
pcsys.o: ../sys/share/pcsys.c $(HACK_H)
	$(CC) $(CFLAGS) -c ../sys/share/pcsys.c
pctty.o: ../sys/share/pctty.c $(HACK_H)
	$(CC) $(CFLAGS) -c ../sys/share/pctty.c
pcunix.o: ../sys/share/pcunix.c $(HACK_H)
	$(CC) $(CFLAGS) -c ../sys/share/pcunix.c
random.o: ../sys/share/random.c $(HACK_H)
	$(CC) $(CFLAGS) -c ../sys/share/random.c
ioctl.o: ../sys/share/ioctl.c $(HACK_H) ../include/tcap.h
	$(CC) $(CFLAGS) -c ../sys/share/ioctl.c
unixtty.o: ../sys/share/unixtty.c $(HACK_H)
	$(CC) $(CFLAGS) -c ../sys/share/unixtty.c
unixmain.o: ../sys/unix/unixmain.c $(HACK_H) ../include/dlb.h
	$(CC) $(CFLAGS) -c ../sys/unix/unixmain.c
unixunix.o: ../sys/unix/unixunix.c $(HACK_H)
	$(CC) $(CFLAGS) -c ../sys/unix/unixunix.c
unixres.o: ../sys/unix/unixres.c $(CONFIG_H)
	$(CC) $(CFLAGS) -c ../sys/unix/unixres.c
bemain.o: ../sys/be/bemain.c $(HACK_H) ../include/dlb.h
	$(CC) $(CFLAGS) -c ../sys/be/bemain.c
getline.o: ../win/tty/getline.c $(HACK_H) ../include/func_tab.h
	$(CC) $(CFLAGS) -c ../win/tty/getline.c
termcap.o: ../win/tty/termcap.c $(HACK_H) ../include/tcap.h
	$(CC) $(CFLAGS) -c ../win/tty/termcap.c
topl.o: ../win/tty/topl.c $(HACK_H) ../include/tcap.h
	$(CC) $(CFLAGS) -c ../win/tty/topl.c
wintty.o: ../win/tty/wintty.c $(HACK_H) ../include/dlb.h \
		../include/patchlevel.h ../include/tcap.h
	$(CC) $(CFLAGS) -c ../win/tty/wintty.c
Window.o: ../win/X11/Window.c ../include/xwindowp.h ../include/xwindow.h \
		$(CONFIG_H)
	$(CC) $(CFLAGS) -c ../win/X11/Window.c
dialogs.o: ../win/X11/dialogs.c $(CONFIG_H)
	$(CC) $(CFLAGS) -c ../win/X11/dialogs.c
winX.o: ../win/X11/winX.c $(HACK_H) ../include/winX.h ../include/dlb.h \
		../include/patchlevel.h ../win/X11/nh72icon \
		../win/X11/nh56icon ../win/X11/nh32icon
	$(CC) $(CFLAGS) -c ../win/X11/winX.c
winmap.o: ../win/X11/winmap.c ../include/xwindow.h $(HACK_H) ../include/dlb.h \
		../include/winX.h ../include/patchlevel.h ../include/tile2x11.h
	$(CC) $(CFLAGS) -c ../win/X11/winmap.c
winmenu.o: ../win/X11/winmenu.c $(HACK_H) ../include/winX.h
	$(CC) $(CFLAGS) -c ../win/X11/winmenu.c
winmesg.o: ../win/X11/winmesg.c ../include/xwindow.h $(HACK_H) ../include/winX.h
	$(CC) $(CFLAGS) -c ../win/X11/winmesg.c
winmisc.o: ../win/X11/winmisc.c $(HACK_H) ../include/func_tab.h \
		../include/winX.h
	$(CC) $(CFLAGS) -c ../win/X11/winmisc.c
winstat.o: ../win/X11/winstat.c $(HACK_H) ../include/winX.h
	$(CC) $(CFLAGS) -c ../win/X11/winstat.c
wintext.o: ../win/X11/wintext.c $(HACK_H) ../include/winX.h ../include/xwindow.h
	$(CC) $(CFLAGS) -c ../win/X11/wintext.c
winval.o: ../win/X11/winval.c $(HACK_H) ../include/winX.h
	$(CC) $(CFLAGS) -c ../win/X11/winval.c
gtk.o: ../win/gtk/gtk.c ../include/md5.h ../include/winGTK.h $(HACK_H) \
		../include/nhxdr.h ../include/proxycom.h ../include/func_tab.h \
		../include/dlb.h ../include/patchlevel.h ../include/proxycb.h \
		../include/prxyclnt.h
	$(CC) $(CFLAGS) $(WINGTKCFLAGS) -c ../win/gtk/gtk.c
gtkgetlin.o: ../win/gtk/gtkgetlin.c ../include/winGTK.h $(HACK_H) \
		../include/nhxdr.h ../include/proxycom.h
	$(CC) $(CFLAGS) $(WINGTKCFLAGS) -c ../win/gtk/gtkgetlin.c
gtkstatus.o: ../win/gtk/gtkstatus.c ../include/winGTK.h $(HACK_H) \
		../include/nhxdr.h ../include/proxycom.h
	$(CC) $(CFLAGS) $(WINGTKCFLAGS) -c ../win/gtk/gtkstatus.c
gtkmenu.o: ../win/gtk/gtkmenu.c ../include/winGTK.h $(HACK_H) \
		../include/nhxdr.h ../include/proxycom.h ../include/patchlevel.h
	$(CC) $(CFLAGS) $(WINGTKCFLAGS) -c ../win/gtk/gtkmenu.c
gtkyn.o: ../win/gtk/gtkyn.c ../include/winGTK.h $(HACK_H) ../include/nhxdr.h \
		../include/proxycom.h
	$(CC) $(CFLAGS) $(WINGTKCFLAGS) -c ../win/gtk/gtkyn.c
gtkextcmd.o: ../win/gtk/gtkextcmd.c ../include/winGTK.h $(HACK_H) \
		../include/nhxdr.h ../include/proxycom.h ../include/proxycb.h \
		../include/func_tab.h
	$(CC) $(CFLAGS) $(WINGTKCFLAGS) -c ../win/gtk/gtkextcmd.c
gtkmap.o: ../win/gtk/gtkmap.c ../include/winGTK.h $(HACK_H) ../include/nhxdr.h \
		../include/proxycom.h ../include/dlb.h ../include/patchlevel.h \
		../include/proxycb.h ../include/prxyclnt.h \
		../win/gtk/gtkprogress.h
	$(CC) $(CFLAGS) $(WINGTKCFLAGS) -c ../win/gtk/gtkmap.c
gtkmessage.o: ../win/gtk/gtkmessage.c ../include/winGTK.h $(HACK_H) \
		../include/nhxdr.h ../include/proxycom.h
	$(CC) $(CFLAGS) $(WINGTKCFLAGS) -c ../win/gtk/gtkmessage.c
gtkmisc.o: ../win/gtk/gtkmisc.c ../include/winGTK.h $(HACK_H) \
		../include/nhxdr.h ../include/proxycom.h ../include/proxycb.h
	$(CC) $(CFLAGS) $(WINGTKCFLAGS) -c ../win/gtk/gtkmisc.c
gtktile.o: ../win/gtk/gtktile.c ../include/winGTK.h $(HACK_H) \
		../include/nhxdr.h ../include/proxycom.h ../include/dlb.h \
		../include/proxycb.h ../include/prxyclnt.h \
		../win/gtk/gtkprogress.h
	$(CC) $(CFLAGS) $(WINGTKCFLAGS) -c ../win/gtk/gtktile.c
xshmmap.o: ../win/gtk/xshmmap.c ../include/winGTK.h $(HACK_H) \
		../include/nhxdr.h ../include/proxycom.h
	$(CC) $(CFLAGS) $(WINGTKCFLAGS) -c ../win/gtk/xshmmap.c
gtkhackrc.o: ../win/gtk/gtkhackrc.c ../include/winGTK.h $(HACK_H) \
		../include/nhxdr.h ../include/proxycom.h ../include/prxyclnt.h
	$(CC) $(CFLAGS) $(WINGTKCFLAGS) -c ../win/gtk/gtkhackrc.c
gtkprefs.o: ../win/gtk/gtkprefs.c ../include/winGTK.h $(HACK_H) \
		../include/nhxdr.h ../include/proxycom.h ../include/patchlevel.h
	$(CC) $(CFLAGS) $(WINGTKCFLAGS) -c ../win/gtk/gtkprefs.c
gtkprogress.o: ../win/gtk/gtkprogress.c ../include/winGTK.h $(HACK_H) \
		../include/nhxdr.h ../include/proxycom.h \
		../win/gtk/gtkprogress.h
	$(CC) $(CFLAGS) $(WINGTKCFLAGS) -c ../win/gtk/gtkprogress.c
gtkext.o: ../win/gtk/gtkext.c ../include/winGTK.h $(HACK_H) ../include/nhxdr.h \
		../include/proxycom.h ../include/prxyclnt.h
	$(CC) $(CFLAGS) $(WINGTKCFLAGS) -c ../win/gtk/gtkext.c
gtkint.o: ../win/gtk/gtkint.c ../include/winGTK.h $(HACK_H) ../include/nhxdr.h \
		../include/proxycom.h ../include/prxyclnt.h \
		../include/func_tab.h ../include/dlb.h ../include/patchlevel.h
	$(CC) $(CFLAGS) $(WINGTKCFLAGS) -c ../win/gtk/gtkint.c
gnaskstr.o: ../win/gnome/gnaskstr.c ../win/gnome/gnaskstr.h \
		../win/gnome/gnmain.h
	$(CC) $(CFLAGS) $(GNOMEINC) -c ../win/gnome/gnaskstr.c
gnbind.o: ../win/gnome/gnbind.c ../win/gnome/gnbind.h ../win/gnome/gnmain.h \
		../win/gnome/gnmenu.h ../win/gnome/gnaskstr.h \
		../win/gnome/gnyesno.h
	$(CC) $(CFLAGS) $(GNOMEINC) -c ../win/gnome/gnbind.c
gnglyph.o: ../win/gnome/gnglyph.c ../win/gnome/gnglyph.h
	$(CC) $(CFLAGS) $(GNOMEINC) -c ../win/gnome/gnglyph.c
gnmain.o: ../win/gnome/gnmain.c ../win/gnome/gnmain.h ../win/gnome/gnsignal.h \
		../win/gnome/gnbind.h ../win/gnome/gnopts.h $(HACK_H) \
		../include/date.h
	$(CC) $(CFLAGS) $(GNOMEINC) -c ../win/gnome/gnmain.c
gnmap.o: ../win/gnome/gnmap.c ../win/gnome/gnmap.h ../win/gnome/gnglyph.h \
		../win/gnome/gnsignal.h $(HACK_H)
	$(CC) $(CFLAGS) $(GNOMEINC) -c ../win/gnome/gnmap.c
gnmenu.o: ../win/gnome/gnmenu.c ../win/gnome/gnmenu.h ../win/gnome/gnmain.h \
		../win/gnome/gnbind.h ../include/func_tab.h
	$(CC) $(CFLAGS) $(GNOMEINC) -c ../win/gnome/gnmenu.c
gnmesg.o: ../win/gnome/gnmesg.c ../win/gnome/gnmesg.h ../win/gnome/gnsignal.h
	$(CC) $(CFLAGS) $(GNOMEINC) -c ../win/gnome/gnmesg.c
gnopts.o: ../win/gnome/gnopts.c ../win/gnome/gnopts.h ../win/gnome/gnglyph.h \
		../win/gnome/gnmain.h ../win/gnome/gnmap.h $(HACK_H)
	$(CC) $(CFLAGS) $(GNOMEINC) -c ../win/gnome/gnopts.c
gnplayer.o: ../win/gnome/gnplayer.c ../win/gnome/gnplayer.h \
		../win/gnome/gnmain.h $(HACK_H)
	$(CC) $(CFLAGS) $(GNOMEINC) -c ../win/gnome/gnplayer.c
gnsignal.o: ../win/gnome/gnsignal.c ../win/gnome/gnsignal.h \
		../win/gnome/gnmain.h
	$(CC) $(CFLAGS) $(GNOMEINC) -c ../win/gnome/gnsignal.c
gnstatus.o: ../win/gnome/gnstatus.c ../win/gnome/gnstatus.h \
		../win/gnome/gnsignal.h ../win/gnome/gn_xpms.h \
		../win/gnome/gnomeprv.h
	$(CC) $(CFLAGS) $(GNOMEINC) -c ../win/gnome/gnstatus.c
gntext.o: ../win/gnome/gntext.c ../win/gnome/gntext.h ../win/gnome/gnmain.h \
		../win/gnome/gn_rip.h
	$(CC) $(CFLAGS) $(GNOMEINC) -c ../win/gnome/gntext.c
gnyesno.o: ../win/gnome/gnyesno.c ../win/gnome/gnbind.h ../win/gnome/gnyesno.h
	$(CC) $(CFLAGS) $(GNOMEINC) -c ../win/gnome/gnyesno.c
gnworn.o: ../win/gnome/gnworn.c ../win/gnome/gnworn.h ../win/gnome/gnglyph.h \
		../win/gnome/gnsignal.h ../win/gnome/gnomeprv.h
	$(CC) $(CFLAGS) $(GNOMEINC) -c ../win/gnome/gnworn.c
wingem.o: ../win/gem/wingem.c $(HACK_H) ../include/func_tab.h ../include/dlb.h \
		../include/patchlevel.h ../include/wingem.h
	$(CC) $(CFLAGS) -c ../win/gem/wingem.c
wingem1.o: ../win/gem/wingem1.c ../include/gem_rsc.h ../include/load_img.h \
		../include/gr_rect.h ../include/wintype.h ../include/wingem.h
	$(CC) $(CFLAGS) -c ../win/gem/wingem1.c
load_img.o: ../win/gem/load_img.c ../include/load_img.h
	$(CC) $(CFLAGS) -c ../win/gem/load_img.c
gr_rect.o: ../win/gem/gr_rect.c ../include/gr_rect.h
	$(CC) $(CFLAGS) -c ../win/gem/gr_rect.c
winproxy.o: ../win/proxy/winproxy.c $(HACK_H) ../include/patchlevel.h \
		../include/date.h ../include/nhxdr.h ../include/proxycom.h \
		../include/winproxy.h #../include/win32api.h
	$(CC) $(CFLAGS) -c ../win/proxy/winproxy.c
callback.o: ../win/proxy/callback.c $(HACK_H) ../include/func_tab.h \
		../include/md5.h ../include/nhxdr.h ../include/proxycom.h \
		../include/winproxy.h
	$(CC) $(CFLAGS) -c ../win/proxy/callback.c
dlbh.o: ../win/proxy/dlbh.c $(HACK_H) ../include/dlb.h
	$(CC) $(CFLAGS) -c ../win/proxy/dlbh.c
mapid.o: ../win/proxy/mapid.c $(HACK_H) ../include/nhxdr.h ../include/winproxy.h
	$(CC) $(CFLAGS) -c ../win/proxy/mapid.c
riputil.o: ../win/proxy/riputil.c $(HACK_H)
	$(CC) $(CFLAGS) -c ../win/proxy/riputil.c
getopt.o: ../win/proxy/getopt.c $(HACK_H)
	$(CC) $(CFLAGS) -c ../win/proxy/getopt.c
glyphmap.o: ../win/proxy/glyphmap.c $(HACK_H) ../include/nhxdr.h \
		../include/proxycom.h ../include/winproxy.h
	$(CC) $(CFLAGS) -c ../win/proxy/glyphmap.c
proxysvc.o: ../win/proxy/proxysvc.c ../include/nhxdr.h ../include/proxycom.h \
		../include/proxycb.h ../include/prxyclnt.h
	$(CC) $(CFLAGS) -c ../win/proxy/proxysvc.c
proxycb.o: ../win/proxy/proxycb.c ../include/nhxdr.h ../include/proxycom.h \
		../include/proxycb.h
	$(CC) $(CFLAGS) -c ../win/proxy/proxycb.c
prxymap.o: ../win/proxy/prxymap.c ../include/nhxdr.h ../include/proxycom.h \
		../include/prxyclnt.h
	$(CC) $(CFLAGS) -c ../win/proxy/prxymap.c
prxychar.o: ../win/proxy/prxychar.c ../include/nhxdr.h ../include/proxycom.h \
		../include/prxyclnt.h
	$(CC) $(CFLAGS) -c ../win/proxy/prxychar.c
prxytile.o: ../win/proxy/prxytile.c ../include/nhxdr.h ../include/proxycom.h \
		../include/prxyclnt.h ../include/proxycb.h
	$(CC) $(CFLAGS) -c ../win/proxy/prxytile.c
prxyconn.o: ../win/proxy/prxyconn.c ../include/nhxdr.h ../include/proxycom.h \
		../include/prxyclnt.h
	$(CC) $(CFLAGS) -c ../win/proxy/prxyconn.c
nhext.o: ../win/proxy/nhext.c ../include/nhxdr.h ../include/proxycom.h
	$(CC) $(CFLAGS) -c ../win/proxy/nhext.c
nhxdr.o: ../win/proxy/nhxdr.c ../include/nhxdr.h
	$(CC) $(CFLAGS) -c ../win/proxy/nhxdr.c
nhextxdr.o: ../win/proxy/nhextxdr.c ../include/nhxdr.h
	$(CC) $(CFLAGS) -c ../win/proxy/nhextxdr.c
nhextio.o: ../win/proxy/nhextio.c ../include/nhxdr.h
	$(CC) $(CFLAGS) -c ../win/proxy/nhextio.c
nhextnb.o: ../win/proxy/nhextnb.c ../include/nhxdr.h
	$(CC) $(CFLAGS) -c ../win/proxy/nhextnb.c
compxdr.o: ../win/proxy/compxdr.c ../include/nhxdr.h ../include/proxycom.h
	$(CC) $(CFLAGS) -c ../win/proxy/compxdr.c
md5.o: ../win/proxy/md5.c ../include/md5.h
	$(CC) $(CFLAGS) -c ../win/proxy/md5.c
gl_conf.o: ../win/gl/gl_conf.c $(HACK_H) ../include/dlb.h \
		../include/patchlevel.h ../include/winGL.h
	$(CC) $(CFLAGS) $(SDLGL_CFLAGS) -c ../win/gl/gl_conf.c
gl_emul.o: ../win/gl/gl_emul.c $(HACK_H) ../include/winGL.h \
		../include/func_tab.h
	$(CC) $(CFLAGS) $(SDLGL_CFLAGS) -c ../win/gl/gl_emul.c
gl_font.o: ../win/gl/gl_font.c $(HACK_H) ../include/patchlevel.h \
		../include/winGL.h
	$(CC) $(CFLAGS) $(SDLGL_CFLAGS) -c ../win/gl/gl_font.c
gl_hardw.o: ../win/gl/gl_hardw.c $(HACK_H) ../include/patchlevel.h \
		../include/winGL.h
	$(CC) $(CFLAGS) $(SDLGL_CFLAGS) -c ../win/gl/gl_hardw.c
gl_image.o: ../win/gl/gl_image.c $(HACK_H) ../include/patchlevel.h \
		../include/dlb.h ../include/winGL.h
	$(CC) $(CFLAGS) $(SDLGL_CFLAGS) -c ../win/gl/gl_image.c
gl_main.o: ../win/gl/gl_main.c $(HACK_H) ../include/dlb.h \
		../include/patchlevel.h ../include/winGL.h
	$(CC) $(CFLAGS) $(SDLGL_CFLAGS) -c ../win/gl/gl_main.c
gl_map.o: ../win/gl/gl_map.c $(HACK_H) ../include/patchlevel.h \
		../include/winGL.h
	$(CC) $(CFLAGS) $(SDLGL_CFLAGS) -c ../win/gl/gl_map.c
gl_menu.o: ../win/gl/gl_menu.c $(HACK_H) ../include/dlb.h \
		../include/patchlevel.h ../include/winGL.h
	$(CC) $(CFLAGS) $(SDLGL_CFLAGS) -c ../win/gl/gl_menu.c
gl_opt.o: ../win/gl/gl_opt.c $(HACK_H) ../include/dlb.h \
		../include/patchlevel.h ../include/date.h ../include/winGL.h
	$(CC) $(CFLAGS) $(SDLGL_CFLAGS) -c ../win/gl/gl_opt.c
gl_role.o: ../win/gl/gl_role.c $(HACK_H) ../include/dlb.h \
		../include/patchlevel.h ../include/winGL.h
	$(CC) $(CFLAGS) $(SDLGL_CFLAGS) -c ../win/gl/gl_role.c
gl_rendu.o: ../win/gl/gl_rendu.c $(HACK_H) ../include/patchlevel.h \
		../include/winGL.h
	$(CC) $(CFLAGS) $(SDLGL_CFLAGS) -c ../win/gl/gl_rendu.c
gl_softw.o: ../win/gl/gl_softw.c $(HACK_H) ../include/patchlevel.h \
		../include/winGL.h
	$(CC) $(CFLAGS) $(SDLGL_CFLAGS) -c ../win/gl/gl_softw.c
gl_stat.o: ../win/gl/gl_stat.c $(HACK_H) ../include/winGL.h
	$(CC) $(CFLAGS) $(SDLGL_CFLAGS) -c ../win/gl/gl_stat.c
gl_text.o: ../win/gl/gl_text.c $(HACK_H) ../include/dlb.h \
		../include/patchlevel.h ../include/winGL.h
	$(CC) $(CFLAGS) $(SDLGL_CFLAGS) -c ../win/gl/gl_text.c
gl_tile.o: ../win/gl/gl_tile.c $(HACK_H) ../include/patchlevel.h \
		../include/winGL.h
	$(CC) $(CFLAGS) $(SDLGL_CFLAGS) -c ../win/gl/gl_tile.c
gl_unit.o: ../win/gl/gl_unit.c $(HACK_H) ../include/winGL.h
	$(CC) $(CFLAGS) $(SDLGL_CFLAGS) -c ../win/gl/gl_unit.c
gl_win.o: ../win/gl/gl_win.c $(HACK_H) ../include/dlb.h \
		../include/patchlevel.h ../include/winGL.h
	$(CC) $(CFLAGS) $(SDLGL_CFLAGS) -c ../win/gl/gl_win.c
tile.o: tile.c $(HACK_H)
qt_win.o: ../win/Qt/qt_win.cpp $(HACK_H) ../include/func_tab.h \
		../include/dlb.h ../include/date.h ../include/patchlevel.h \
		../include/tile2x11.h ../include/qt_win.h \
		../include/qt_clust.h ../include/qt_kde0.h \
		../include/qt_xpms.h qt_win.moc qt_kde0.moc qttableview.moc
	$(CXX) $(CXXFLAGS) -c ../win/Qt/qt_win.cpp
qt_clust.o: ../win/Qt/qt_clust.cpp ../include/qt_clust.h
	$(CXX) $(CXXFLAGS) -c ../win/Qt/qt_clust.cpp
qttableview.o: ../win/Qt/qttableview.cpp ../include/qttableview.h
	$(CXX) $(CXXFLAGS) -c ../win/Qt/qttableview.cpp
monstr.o: monstr.c $(CONFIG_H)
vis_tab.o: vis_tab.c $(CONFIG_H) ../include/vis_tab.h
allmain.o: allmain.c $(HACK_H) ../include/patchlevel.h
alloc.o: alloc.c $(CONFIG_H)
apply.o: apply.c $(HACK_H) ../include/edog.h
artifact.o: artifact.c $(HACK_H) ../include/artifact.h ../include/artilist.h
attrib.o: attrib.c $(HACK_H)
ball.o: ball.c $(HACK_H)
bones.o: bones.c $(HACK_H) ../include/lev.h
botl.o: botl.c $(HACK_H)
cmd.o: cmd.c $(HACK_H) ../include/func_tab.h
dbridge.o: dbridge.c $(HACK_H)
decl.o: decl.c $(HACK_H)
detect.o: detect.c $(HACK_H) ../include/artifact.h
dig.o: dig.c $(HACK_H) ../include/edog.h
display.o: display.c $(HACK_H)
dlb.o: dlb.c $(CONFIG_H) ../include/dlb.h
do.o: do.c $(HACK_H) ../include/lev.h
do_name.o: do_name.c $(HACK_H)
do_wear.o: do_wear.c $(HACK_H)
dog.o: dog.c $(HACK_H) ../include/edog.h ../include/emin.h ../include/epri.h
dogmove.o: dogmove.c $(HACK_H) ../include/mfndpos.h ../include/edog.h \
		../include/emin.h ../include/epri.h
dokick.o: dokick.c $(HACK_H) ../include/eshk.h
dothrow.o: dothrow.c $(HACK_H) ../include/edog.h
drawing.o: drawing.c $(HACK_H) ../include/tcap.h
dungeon.o: dungeon.c $(HACK_H) ../include/dgn_file.h ../include/dlb.h
eat.o: eat.c $(HACK_H)
end.o: end.c $(HACK_H) ../include/eshk.h ../include/dlb.h
engrave.o: engrave.c $(HACK_H) ../include/lev.h
exper.o: exper.c $(HACK_H)
explode.o: explode.c $(HACK_H)
extralev.o: extralev.c $(HACK_H)
files.o: files.c $(HACK_H) ../include/dlb.h ../include/filename.h \
		../include/winGL.h ../include/winproxy.h
fountain.o: fountain.c $(HACK_H)
hack.o: hack.c $(HACK_H)
hacklib.o: hacklib.c $(HACK_H)
invent.o: invent.c $(HACK_H)
light.o: light.c $(HACK_H) ../include/lev.h
lock.o: lock.c $(HACK_H)
mail.o: mail.c $(HACK_H) ../include/mail.h
makemon.o: makemon.c $(HACK_H) ../include/epri.h ../include/emin.h \
		../include/edog.h
mapglyph.o: mapglyph.c $(HACK_H)
mcastu.o: mcastu.c $(HACK_H)
mhitm.o: mhitm.c $(HACK_H) ../include/artifact.h ../include/edog.h
mhitu.o: mhitu.c $(HACK_H) ../include/artifact.h ../include/edog.h
minion.o: minion.c $(HACK_H) ../include/emin.h ../include/epri.h
mklev.o: mklev.c $(HACK_H)
mkmap.o: mkmap.c $(HACK_H) ../include/sp_lev.h
mkmaze.o: mkmaze.c $(HACK_H) ../include/sp_lev.h ../include/lev.h
mkobj.o: mkobj.c $(HACK_H)
mkroom.o: mkroom.c $(HACK_H)
mon.o: mon.c $(HACK_H) ../include/mfndpos.h ../include/edog.h \
		../include/artifact.h
mondata.o: mondata.c $(HACK_H) ../include/eshk.h ../include/epri.h
monmove.o: monmove.c $(HACK_H) ../include/mfndpos.h ../include/artifact.h \
		../include/epri.h
monst.o: monst.c $(CONFIG_H) ../include/permonst.h ../include/align.h \
		../include/monattk.h ../include/monflag.h ../include/monsym.h \
		../include/dungeon.h ../include/eshk.h ../include/vault.h \
		../include/epri.h ../include/egyp.h ../include/color.h
mplayer.o: mplayer.c $(HACK_H)
mthrowu.o: mthrowu.c $(HACK_H)
muse.o: muse.c $(HACK_H) ../include/edog.h
music.o: music.c $(HACK_H) #interp.c
o_init.o: o_init.c $(HACK_H) ../include/lev.h
objects.o: objects.c $(CONFIG_H) ../include/obj.h ../include/objclass.h \
		../include/prop.h ../include/skills.h ../include/color.h
objnam.o: objnam.c $(HACK_H)
options.o: options.c $(CONFIG_H) ../include/objclass.h ../include/flag.h \
		$(HACK_H) ../include/tcap.h ../include/winGL.h \
		../include/filename.h
pager.o: pager.c $(HACK_H) ../include/dlb.h
pickup.o: pickup.c $(HACK_H)
pline.o: pline.c $(HACK_H) ../include/epri.h ../include/edog.h
polyself.o: polyself.c $(HACK_H)
potion.o: potion.c $(HACK_H)
pray.o: pray.c $(HACK_H) ../include/epri.h
priest.o: priest.c $(HACK_H) ../include/mfndpos.h ../include/eshk.h \
		../include/epri.h ../include/emin.h
quest.o: quest.c $(HACK_H) ../include/qtext.h
questpgr.o: questpgr.c $(HACK_H) ../include/dlb.h ../include/qtext.h
read.o: read.c $(HACK_H)
rect.o: rect.c $(HACK_H)
region.o: region.c $(HACK_H) ../include/lev.h
restore.o: restore.c $(HACK_H) ../include/lev.h ../include/tcap.h
rip.o: rip.c $(HACK_H)
rnd.o: rnd.c $(HACK_H)
role.o: role.c $(HACK_H)
rumors.o: rumors.c $(HACK_H) ../include/lev.h ../include/dlb.h
save.o: save.c $(HACK_H) ../include/lev.h
shk.o: shk.c $(HACK_H) ../include/eshk.h
shknam.o: shknam.c $(HACK_H) ../include/eshk.h
sit.o: sit.c $(HACK_H) ../include/artifact.h
sounds.o: sounds.c $(HACK_H) ../include/edog.h
sp_lev.o: sp_lev.c $(HACK_H) ../include/dlb.h ../include/sp_lev.h
spell.o: spell.c $(HACK_H) ../include/edog.h
steal.o: steal.c $(HACK_H)
steed.o: steed.c $(HACK_H)
teleport.o: teleport.c $(HACK_H)
timeout.o: timeout.c $(HACK_H) ../include/lev.h
topten.o: topten.c $(HACK_H) ../include/dlb.h ../include/patchlevel.h
track.o: track.c $(HACK_H)
trap.o: trap.c $(HACK_H)
u_init.o: u_init.c $(HACK_H)
uhitm.o: uhitm.c $(HACK_H)
vault.o: vault.c $(HACK_H) ../include/vault.h
version.o: version.c $(HACK_H) ../include/date.h ../include/patchlevel.h
vision.o: vision.c $(HACK_H) ../include/vis_tab.h
weapon.o: weapon.c $(HACK_H)
were.o: were.c $(HACK_H)
wield.o: wield.c $(HACK_H)
windows.o: windows.c $(HACK_H) ../include/wingem.h ../include/winGnome.h \
		../include/winGL.h ../include/winproxy.h
wizard.o: wizard.c $(HACK_H) ../include/qtext.h ../include/epri.h
worm.o: worm.c $(HACK_H) ../include/lev.h
worn.o: worn.c $(HACK_H)
write.o: write.c $(HACK_H)
zap.o: zap.c $(HACK_H)
gypsy.o: gypsy.c $(HACK_H) ../include/egyp.h ../include/qtext.h
tech.o: tech.c $(HACK_H)
# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
