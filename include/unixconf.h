/*	SCCS Id: @(#)unixconf.h 3.4	1999/07/02	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifdef UNIX
#ifndef UNIXCONF_H
#define UNIXCONF_H


/* Need this to automatically test for BSD */
#if (defined(__unix__) || defined(unix)) && !defined(USG)
#include <sys/param.h>
#endif

/*
 * Some include files are in a different place under SYSV
 *	BSD		   SYSV
 * <sys/time.h>		<time.h>
 * <sgtty.h>		<termio.h>
 *
 * Some routines are called differently
 * index		strchr
 * rindex		strrchr
 *
 */

/* define exactly one of the following four choices */
/* #define BSD 1 */	/* define for 4.n/Free/Open/Net BSD  */
			/* also for relatives like SunOS 4.x, DG/UX, and */
			/* older versions of Linux */
/* #define ULTRIX */	/* define for Ultrix v3.0 or higher (but not lower) */
			/* Use BSD for < v3.0 */
			/* "ULTRIX" not to be confused with "ultrix" */
#define SYSV		/* define for System V, Solaris 2.x, newer versions */
			/* of Linux */
/* #define HPUX */	/* Hewlett-Packard's Unix, version 6.5 or higher */
			/* use SYSV for < v6.5 */


/* define any of the following that are appropriate */
#define SVR4		/* use in addition to SYSV for System V Release 4 */
			/* including Solaris 2+ */
#define NETWORK		/* if running on a networked system */
			/* e.g. Suns sharing a playground through NFS */
/* #define SUNOS4 */	/* SunOS 4.x */
#if !defined(BSD) && !defined(__APPLE__)
#define LINUX		/* Another Unix clone */
#endif
/* #define CYGWIN32 */	/* Unix on Win32 -- use with case sensitive defines */
/* #define GENIX */	/* Yet Another Unix Clone */
/* #define HISX */	/* Bull Unix for XPS Machines */
/* #define BOS */	/* Bull Open Software - Unix for DPX/2 Machines */
/* #define UNIXPC */	/* use in addition to SYSV for AT&T 7300/3B1 */
/* #define AIX_31 */	/* In AIX 3.1 (IBM RS/6000) use BSD ioctl's to gain
			 * job control (note that AIX is SYSV otherwise)
			 * Also define this for AIX 3.2 */

#define TERMINFO	/* uses terminfo rather than termcap */
			/* Should be defined for most SYSV, SVR4 (including
			 * Solaris 2+), HPUX, and Linux systems.  In
			 * particular, it should NOT be defined for the UNIXPC
			 * unless you remove the use of the shared library in
			 * the Makefile */
#define TEXTCOLOR	/* Use System V r3.2 terminfo color support */
			/* and/or ANSI color support on termcap systems */
			/* and/or X11 color */
#define POSIX_JOB_CONTROL /* use System V / Solaris 2.x / POSIX job control */
			/* (e.g., VSUSP) */
#define POSIX_TYPES	/* use POSIX types for system calls and termios */
			/* Define for many recent OS releases, including
			 * those with specific defines (since types are
			 * changing toward the standard from earlier chaos).
			 * For example, platforms using the GNU libraries,
			 * Linux, Solaris 2.x
			 */

/* #define OPENWINBUG */	/* avoid a problem using OpenWindows 3.0 for
				   X11 on SunOS 4.1.x, x>= 2.  Do not define
				   for other X11 implementations. */
/* #define PYRAMID_BUG */	/* avoid a bug on the Pyramid */
/* #define BSD_43_BUG */	/* for real 4.3BSD cc's without schain botch fix */
/* #define MICROPORT_BUG */	/* problems with large arrays in structs */
/* #define MICROPORT_286_BUG */ /* changes needed in termcap.c to get it to
				   run with Microport Sys V/AT version 2.4.
				   By Jay Maynard */
/* #define AIXPS_2BUG */	/* avoid a problem with little_to_big() optimization */

/* #define RANDOM */		/* if neither random/srandom nor lrand48/srand48
				   is available from your system */

/* see sys/unix/snd86unx.shr for more information on these */
/* #define UNIX386MUSIC */	/* play real music through speaker on systems
				   with music driver installed */
/* #define VPIX_MUSIC */	/* play real music through speaker on systems
				   with built-in VPIX support */

/*
 * ALI
 *
 * File areas can be used to place different Slash'EM files in different
 * directories. This is necessary to comply with Linux's FSSTD 1.2 and its
 * replacement FHS 2.0. Currently, these are only implemented for UNIX, but
 * other ports could follow suit by including similar defines in their port
 * specific config files and implementing the relevent functions.
 *
 * File areas are supposed to be opaque; that is the core system does not
 * interpret them in any way, it simply passes them to the port specific
 * functions to handle. This means that while the obvious mapping is onto
 * directories, there is nothing in the core code that assumes this. Non-UNIX
 * ports in particular may find that a more general mapping is desirable.
 *
 * The following file areas are supported:
 *
 * FILE_AREA_SAVE	For save files (R/W)
 * FILE_AREA_BONES	For bones files (R/W)
 * FILE_AREA_LEVL	For level files (R/W)
 * FILE_AREA_VAR	For other files which may be modified by Slash'EM
 * FILE_AREA_SHARE	For read-only, architechure independent, files 
 * FILE_AREA_UNSHARE	For read-only, architechure dependent, files 
 * FILE_AREA_DOC	For human-readable documentation
 */

/*#ifdef PUBLIC_SERVER*/
/*#define FILE_AREAS*/ 		/* Use file areas */
/*#endif*/

#ifdef FILE_AREAS

/*
 * File areas compatible with Linux's FSSTND 1.2.
 * Note: This has been superceeded by FHS 2.0, but is included because
 * many systems still seem to use it.
 */

/* #define FILE_AREA_VAR	"/var/lib/games/slex/" */
/* #define FILE_AREA_SAVE	"/var/lib/games/slex/save/" */
/* #define FILE_AREA_SHARE	"/usr/lib/games/slex/" */
/* #define FILE_AREA_UNSHARE	"/usr/lib/games/slex/" */
/* #define FILE_AREA_DOC	"/usr/doc/slex/" */

/*
 * File areas compatible with FHS 2.0
 */

#ifdef PUBLIC_SERVER
#ifndef HACKDIR
#define HACKDIR                 "/games/slex/slexdir"
#endif
#define FILE_AREA_VAR		HACKDIR "/"
#define FILE_AREA_SAVE		HACKDIR "/save/"
#define FILE_AREA_SHARE		HACKDIR "/"
#define FILE_AREA_UNSHARE	HACKDIR "/"
#define FILE_AREA_DOC		HACKDIR "/"

#define FILE_AREA_BONES		HACKDIR "/bones/"
#define FILE_AREA_LEVL		HACKDIR "/level/"

#else /* PUBLIC_SERVER */

#define FILE_AREA_VAR		"/var/games/slex/"
#define FILE_AREA_SAVE		"/var/games/slex/save/"
#define FILE_AREA_SHARE		"/usr/share/games/slex/"
#define FILE_AREA_UNSHARE	"/usr/lib/games/slex/"
#define FILE_AREA_DOC		"/usr/share/doc/slex/"

#define FILE_AREA_BONES		FILE_AREA_VAR
#define FILE_AREA_LEVL		FILE_AREA_VAR

#endif /* PUBLIC_SERVER */

#endif /* FILE_AREAS */

/*
 * The next two defines are intended mainly for the Andrew File System,
 * which does not allow hard links.  If NO_FILE_LINKS is defined, lock files
 * will be created in LOCKDIR using open() instead of in the playground using
 * link().
 *		Ralf Brown, 7/26/89 (from v2.3 hack of 10/10/88)
 */

/* #define NO_FILE_LINKS */	/* if no hard links */

#ifdef NO_FILE_LINKS
# ifdef FILE_AREAS
# define LOCKDIR FILE_AREA_VAR			/* where to put locks */
# else
# define LOCKDIR "/usr/games/lib/nethackdir"	/* where to put locks */
# endif
#endif

/*
 * Define DEF_PAGER as your default pager, e.g. "/bin/cat" or "/usr/ucb/more"
 * If defined, it can be overridden by the environment variable PAGER.
 * Hack will use its internal pager if DEF_PAGER is not defined.
 * (This might be preferable for security reasons.)
 * #define DEF_PAGER	".../mydir/mypager"
 */

/*
 * If you want the static parts of your playground on a read-only file
 * system, define VAR_PLAYGROUND to be where the variable parts are kept.
 */
/* #define VAR_PLAYGROUND "/var/lib/games/nethack" */



/*
 * Define PORT_HELP to be the name of the port-specfic help file.
 * This file is found in HACKDIR.
 * Normally, you shouldn't need to change this.
 * There is currently no port-specific help for Unix systems.
 */
/* #define PORT_HELP "Unixhelp" */

#ifdef TTY_GRAPHICS
/*
 * To enable the `timed_delay' option for using a timer rather than extra
 * screen output when pausing for display effect.  Requires that `msleep'
 * function be available (with time argument specified in milliseconds).
 * Various output devices can produce wildly varying delays when the
 * "extra output" method is used, but not all systems provide access to
 * a fine-grained timer.
 */
#define TIMED_DELAY	/* usleep() */

# ifdef TEXTCOLOR
#  define VIDEOSHADES
# endif
#endif

/*
 * If you define MAIL, then the player will be notified of new mail
 * when it arrives.  If you also define DEF_MAILREADER then this will
 * be the default mail reader, and can be overridden by the environment
 * variable MAILREADER; otherwise an internal pager will be used.
 * A stat system call is done on the mailbox every MAILCKFREQ moves.
 */

#define MAIL 			/* Deliver mail during the game */

#ifdef PUBLIC_SERVER		/* No reason to have mail except on a server. */
#define SIMPLE_MAIL		/* dgamelaunch simple mail */
#endif

/* The Andrew Message System does mail a little differently from normal
 * UNIX.  Mail is deposited in the user's own directory in ~/Mailbox
 * (another directory).  MAILBOX is the element that will be added on to
 * the user's home directory path to generate the Mailbox path - just in
 * case other Andrew sites do it differently from CMU.
 *
 *		dan lovinger
 *		dl2n+@andrew.cmu.edu (dec 19 1989)
 */

/* #define AMS */		/* use Andrew message system for mail */

/* NO_MAILREADER is for kerberos authenticating filesystems where it is
 * essentially impossible to securely exec child processes, like mail
 * readers, when the game is running under a special token.
 *
 *	       dan
 */

/* #define NO_MAILREADER */	/* have mail daemon just tell player of mail */

#ifdef	MAIL
# if defined(BSD) || defined(ULTRIX)
#  ifdef AMS
#define AMS_MAILBOX	"/Mailbox"
#  else
#   if defined(__FreeBSD__) || defined(__OpenBSD__)
#define DEF_MAILREADER "/usr/bin/mail"
#   else
#define DEF_MAILREADER	"/usr/ucb/Mail"
#   endif
#  endif
#else
# if (defined(SYSV) || defined(DGUX) || defined(HPUX)) && !defined(LINUX)
#  if defined(M_XENIX)
#define DEF_MAILREADER	"/usr/bin/mail"
#  else
#   ifdef __sgi
#define DEF_MAILREADER	"/usr/sbin/Mail"
#   else
#define DEF_MAILREADER	"/usr/bin/mailx"
#   endif
#  endif
# else
#define DEF_MAILREADER	"/bin/mail"
# endif
#endif

#endif	/* MAIL */



#ifdef COMPRESS
/* Some implementations of compress need a 'quiet' option.
 * If you've got one of these versions, put -q here.
 * You can also include any other strange options your compress needs.
 * If you have a normal compress, just leave it commented out.
 */
/* #define COMPRESS_OPTIONS "-q" */
#endif

#define FCMASK	0660	/* file creation mask */

/* fcntl(2) is a POSIX-portable call for manipulating file descriptors.
 * Comment out the USE_FCNTL if for some reason you have a strange
 * os/filesystem combination for which fcntl(2) does not work. */
#ifdef POSIX_TYPES
# define USE_FCNTL
#endif

/*
 * The remainder of the file should not need to be changed.
 */

#ifdef _AUX_SOURCE
# ifdef AUX /* gcc ? */
#  define _SYSV_SOURCE
#  define _BSD_SOURCE
#else
#  define AUX
# endif
#endif /* _AUX_SOURCE */

#if defined(LINUX) || defined(bsdi)
# ifndef POSIX_TYPES
#  define POSIX_TYPES
# endif
# ifndef POSIX_JOB_CONTROL
#  define POSIX_JOB_CONTROL
# endif
#endif

/*
 * BSD/ULTRIX systems are normally the only ones that can suspend processes.
 * Suspending NetHack processes cleanly should be easy to add to other systems
 * that have SIGTSTP in the Berkeley sense.  Currently the only such systems
 * known to work are HPUX and AIX 3.1; other systems will probably require
 * tweaks to unixtty.c and ioctl.c.
 *
 * POSIX defines a slightly different type of job control, which should be
 * equivalent for NetHack's purposes.  POSIX_JOB_CONTROL should work on
 * various recent SYSV versions (with possibly tweaks to unixtty.c again).
 */
#ifndef POSIX_JOB_CONTROL
# if defined(BSD) || defined(ULTRIX) || defined(HPUX) || defined(AIX_31)
#  define BSD_JOB_CONTROL
# else
#  if defined(SVR4)
#   define POSIX_JOB_CONTROL
#  endif
# endif
#endif
#if defined(BSD_JOB_CONTROL) || defined(POSIX_JOB_CONTROL) || defined(AUX)
#ifndef PUBLIC_SERVER
#define SUSPEND		/* let ^Z suspend the game */
#endif
#endif

/*
 * Define SAFERHANGUP to delay hangup processing until the main command
 * loop. 'safer' because it avoids certain cheats and also avoids losing
 * objects being thrown when the hangup occurs.  All unix windowports
 * support SAFERHANGUP (couldn't define it here otherwise).
 */
#define SAFERHANGUP

#if defined(BSD) || defined(ULTRIX)
#include <sys/time.h>
#else
#include <time.h>
#endif

#define HLOCK	"perm"	/* an empty file used for locking purposes */
#define HLOCK_AREA	FILE_AREA_VAR

#define tgetch getchar

/*#define SHELL*/		/* do not delete the '!' command */

/* -AJA- Escaping to a shell doesn't work when SDL port is running
 *       fullscreen (makes the computer seem frozen).  Undefining
 *       `SHELL' is a bit blunt though...
 */
#if defined(GL_GRAPHICS) || defined(SDL_GRAPHICS)
#undef SHELL
#endif

#include "system.h"

#if defined(POSIX_TYPES) || defined(__GNUC__)
#include <stdlib.h>
#include <unistd.h>
#endif

#if defined(POSIX_TYPES) || defined(__GNUC__) || defined(BSD) || defined(ULTRIX)
#include <sys/wait.h>
#endif

#if defined(BSD) || defined(ULTRIX)
# if !defined(DGUX) && !defined(SUNOS4)
#define memcpy(d, s, n)		bcopy(s, d, n)
#define memcmp(s1, s2, n)	bcmp(s2, s1, n)
# endif
# ifdef SUNOS4
#include <memory.h>
# endif
#else	/* therefore SYSV */
# ifndef index	/* some systems seem to do this for you */
#define index	strchr
# endif
# ifndef rindex
#define rindex	strrchr
# endif
#endif

/* Use the high quality random number routines. */
#if defined(BSD) || defined(LINUX) || defined(ULTRIX) || defined(CYGWIN32) || defined(RANDOM)
#define Rand()	random()
#else
#define Rand()	lrand48()
#endif

#ifdef TIMED_DELAY
# if defined(SUNOS4) || defined(LINUX) || defined(SVR4) /* [max] added SVR4 */
# define msleep(k) usleep((k)*1000)
# endif
# ifdef ULTRIX
# define msleep(k) napms(k)
# endif
#endif

#ifdef hc	/* older versions of the MetaWare High-C compiler define this */
# ifdef __HC__
#  undef __HC__
# endif
# define __HC__ hc
# undef hc
#endif

#if defined(GNOME_GRAPHICS)
#if defined(LINUX)
# include <linux/unistd.h>
# if defined(__NR_getresuid) && defined(__NR_getresgid)	/* ie., >= v2.1.44 */
#  define GETRES_SUPPORT
# endif
#else
# if defined(BSD) || defined(SVR4)
/*
 * [ALI] We assume that SVR4 means we can safely include syscall.h
 * (although it's really a BSDism). This is certainly true for Solaris 2.5,
 * Solaris 7, Solaris 8 and Compaq Tru64 5.1
 * Later BSD systems will have the getresid system calls.
 */
# include <sys/syscall.h>
# if (defined (SYS_getuid) || defined(SYS_getresuid)) && \
  (defined(SYS_getgid) || defined(SYS_getresgid))
#  define GETRES_SUPPORT
# endif
# endif	/* BSD || SVR4 */
#endif	/* LINUX */
#endif	/* GNOME_GRAPHICS */

#if defined(MENU_COLOR) && defined(USE_REGEX_MATCH)
# include <regex.h>
# ifndef RE_NO_GNU_OPS
#  define POSIX_REGEX
# else
#  define GNU_REGEX
# endif
#endif

#endif /* UNIXCONF_H */
#endif /* UNIX */
