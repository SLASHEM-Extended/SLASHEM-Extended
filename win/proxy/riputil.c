/* $Id: riputil.c,v 1.1 2001-12-11 20:43:49 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001-2002 */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

extern const char *killed_by_prefix[];

char *
get_killer_string(how)
int how;
{
    static char buf[BUFSZ];
    /* Put together death description */
    switch (killer_format) {
	default: impossible("bad killer format?");
	case KILLED_BY_AN:
	    if (Instant_Death) {
		Strcpy(buf, "instantly ");
		strcat(buf, killed_by_prefix[how]);
	    }
	    else if (Quick_Death) {
		Strcpy(buf, "quickly ");
		strcat(buf, killed_by_prefix[how]);
	    }
	    else
		Strcpy(buf, killed_by_prefix[how]);
	    Strcat(buf, an(killer));
	    break;
	case KILLED_BY:
	    if (Instant_Death) {
		Strcpy(buf, "instantly ");
		strcat(buf, killed_by_prefix[how]);
	    }
	    else if (Quick_Death) {
		Strcpy(buf, "quickly ");
		strcat(buf, killed_by_prefix[how]);
	    }
	    else
		Strcpy(buf, killed_by_prefix[how]);
	    Strcat(buf, killer);
	    break;
	case NO_KILLER_PREFIX:
	    Strcpy(buf, killer);
	    break;
    }
    return buf;
}
