/*	SCCS Id: @(#)ioctl.c	3.4	1990/22/02 */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* This cannot be part of hack.tty.c (as it was earlier) since on some
   systems (e.g. MUNIX) the include files <termio.h> and <sgtty.h>
   define the same constants, and the C preprocessor complains. */

#include "hack.h"

#if defined(BSD_JOB_CONTROL) || defined(POSIX_JOB_CONTROL)
# include <termios.h>
# include <sys/ioctl.h>
struct termios termio;
#endif

#ifdef SUSPEND	/* BSD isn't alone anymore... */
#include	<signal.h>
#endif

#if defined(TIOCGWINSZ) && (defined(BSD) || defined(ULTRIX) || defined(AIX_31) || defined(_BULL_SOURCE) || defined(SVR4))
#define USE_WIN_IOCTL
#include "tcap.h"	/* for LI and CO */
#endif

#ifdef _M_UNIX
extern void sco_mapon(void);
extern void sco_mapoff(void);
#endif
#ifdef __linux__
extern void linux_mapon(void);
extern void linux_mapoff(void);
#endif

#ifdef LINUX
extern void linux_mapon(void);
extern void linux_mapoff(void);
#endif

#ifdef AUX
void
catch_stp()
{
    signal(SIGTSTP, SIG_DFL);
    dosuspend();
}
#endif /* AUX */

void getwindowsz() {
    struct winsize ttsz;

    if (ioctl(0, TIOCGWINSZ, &ttsz) != -1) {
	/*
	 * Use the kernel's values for lines and columns if it has
	 * any idea.
	 */
	if (ttsz.ws_row)
	    LI = ttsz.ws_row;
	if (ttsz.ws_col)
	    CO = ttsz.ws_col;
    }
}

void
getioctls()
{
#define POSIX_TYPES
#ifdef BSD_JOB_CONTROL
	(void) ioctl(fileno(stdin), (int) TIOCGLTC, (char *) &ltchars);
	(void) ioctl(fileno(stdin), (int) TIOCSLTC, (char *) &ltchars0);
#else
# ifdef POSIX_TYPES
	(void) tcgetattr(fileno(stdin), &termio);
# else
#  if defined(TCSETS) && !defined(AIX_31)
	(void) ioctl(fileno(stdin), (int) TCGETS, &termio);
#  else
	(void) ioctl(fileno(stdin), (int) TCGETA, &termio);
#  endif
# endif
#endif
	getwindowsz();
#ifdef AUX
	( void ) signal ( SIGTSTP , catch_stp ) ;
#endif
}

void
setioctls()
{
#ifdef BSD_JOB_CONTROL
	(void) ioctl(fileno(stdin), (int) TIOCSLTC, (char *) &ltchars);
#else
# ifdef POSIX_TYPES
	(void) tcsetattr(fileno(stdin), TCSADRAIN, &termio);
# else
#  if defined(TCSETS) && !defined(AIX_31)
	(void) ioctl(fileno(stdin), (int) TCSETSW, &termio);
#  else
	(void) ioctl(fileno(stdin), (int) TCSETAW, &termio);
#  endif
# endif
#endif
}

#ifdef SUSPEND		/* No longer implies BSD */
int
dosuspend()
{

# ifdef SIGTSTP
	if(signal(SIGTSTP, SIG_IGN) == SIG_DFL) {
		suspend_nhwindows((char *)0);
#  ifdef _M_UNIX
		sco_mapon();
#  endif
#  ifdef LINUX
		linux_mapon();
#  endif
#  ifdef __linux__
		linux_mapon();
#  endif
		(void) signal(SIGTSTP, SIG_DFL);
#  ifdef AUX
		( void ) kill ( 0 , SIGSTOP ) ;
#  else
		(void) kill(0, SIGTSTP);
#  endif
#  ifdef LINUX
		linux_mapoff();
#  endif
#  ifdef _M_UNIX
		sco_mapoff();
#  endif
#  ifdef __linux__
		linux_mapoff();
#  endif
		resume_nhwindows();
	} else {
		pline("I don't think your shell has job control.");
	}
# else
	pline("Sorry, it seems we have no SIGTSTP here.  Try ! or S.");
# endif
	return(0);
}
#endif /* SUSPEND */
