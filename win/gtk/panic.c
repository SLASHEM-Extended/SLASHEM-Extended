/*
  $Id: panic.c,v 1.1 2003-05-03 11:12:28 j_ali Exp $
 */
/*
  GTK+ NetHack Copyright (c) Issei Numata 1999-2000
               Copyright (c) Slash'EM Development Team 2001-2002
  GTK+ NetHack may be freely redistributed.  See license for details. 
*/

/* #define DEBUG */			/* Uncomment for debugging */

/*
 * This module implements panic() and friends for when linked as an
 * external proxy module. Based on util/panic.c and src/pline.c
 */

#include <sys/types.h>
#include <signal.h>

#define NEED_VARARGS
#include "config.h"

#ifdef AZTEC
#define abort() exit()
#endif
#ifdef VMS
extern void vms_abort(void);
#endif

/*VARARGS1*/
boolean panicking;
void panic(const char *,...);

void
panic VA_DECL(const char *,str)
	VA_START(str);
	VA_INIT(str, char *);
	if(panicking++)
#ifdef SYSV
	    (void)
#endif
		abort();    /* avoid loops - this should never happen*/

	fputs(" ERROR:  ", stderr);
	vfprintf(stderr, str, VA_ARGS);
	fputc('\n', stderr);
	fflush(stderr);
#if defined(UNIX) || defined(VMS)
# ifdef SYSV
		(void)
# endif
		    abort();	/* generate core dump */
#endif
	VA_END();
	exit(EXIT_FAILURE);		/* redundant */
	return;
}

/*VARARGS1*/
/* Note that these declarations rely on knowledge of the internals
 * of the variable argument handling stuff in "tradstdc.h"
 */

#if defined(USE_STDARG) || defined(USE_VARARGS)
static void vpline(const char *, va_list);

void
pline VA_DECL(const char *, line)
	VA_START(line);
	VA_INIT(line, char *);
	vpline(line, VA_ARGS);
	VA_END();
}

# ifdef USE_STDARG
static void
vpline(const char *line, va_list the_args) {
# else
static void
vpline(line, the_args) const char *line; va_list the_args; {
# endif

#else	/* USE_STDARG | USE_VARARG */

#define vpline pline

void
pline VA_DECL(const char *, line)
#endif	/* USE_STDARG | USE_VARARG */

	char pbuf[BUFSZ];
	extern int GTK_initialized;
/* Do NOT use VA_START and VA_END in here... see above */

	if (!line || !*line) return;
	if (index(line, '%')) {
	    vsprintf(pbuf,line,VA_ARGS);
	    line = pbuf;
	}
	if (!GTK_initialized) {
	    puts(line);
	    (void) fflush(stdout);
	} else {
	    nh_map_flush();
	    nh_message_putstr(line);
	}
}

/*VARARGS1*/
void
impossible VA_DECL(const char *, s)
	VA_START(s);
	VA_INIT(s, const char *);
	vpline(s,VA_ARGS);
	pline("External interface in disorder - perhaps you'd better #quit.");
	VA_END();
}

/*panic.c*/
