/* $Id: gtkhack.c,v 1.1 2003-05-03 11:12:27 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2002-2003 */
/* NetHack may be freely redistributed.  See license for details. */

#include <stdio.h>
#include <signal.h>
#include "winGTK.h"
#include "prxyclnt.h"
#include "proxycb.h"

int
main(int argc, char **argv)
{
    char *s;
#ifdef SIGPIPE
    signal(SIGPIPE, SIG_IGN);
#endif
    proxy_svc_set_ext_procs(win_GTK_init, &GTK_ext_procs);
    s = g_find_program_in_path("slashem");
    proxy_connect("file", s ? s : "slashem", &argc, argv);
    /* proxy_connect only returns if it failed */
    exit(1);
}

/*
 * Some utility functions that are normally provided by NetHack.
 */

char *
eos(char *s)
{
    while(*s)
	s++;
    return s;
}

int
getyear()
{
    time_t t;
    struct tm *tm;
    (void)time(&t);
    tm = localtime(&t);
    return tm->tm_year + 1900;
}

long *
alloc(bytes)
unsigned int bytes;
{
    void *p;
    p = malloc(bytes);
    if (!p)
	panic("GtkHack: Memory allocation failure; cannot get %u bytes", bytes);
    return (long *)p;
}

char
highc(c)		/* force 'c' into uppercase */
char c;
{
    return((char)(('a' <= c && c <= 'z') ? (c & ~040) : c));
}

char
lowc(c)			/* force 'c' into lowercase */
char c;
{
    return((char)(('A' <= c && c <= 'Z') ? (c | 040) : c));
}

int
nh_strncmpi(s1, s2, n)	/* case insensitive counted string comparison */
const char *s1, *s2;
int n;
{
    register char t1, t2;

    while (n--) {
	if (!*s2) return (*s1 != 0);    /* s1 >= s2 */
	else if (!*s1) return -1;       /* s1  < s2 */
	t1 = lowc(*s1++);
	t2 = lowc(*s2++);
	if (t1 != t2) return (t1 > t2) ? 1 : -1;
    }
    return 0;                           /* s1 == s2 */
}
