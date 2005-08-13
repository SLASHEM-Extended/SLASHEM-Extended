/* $Id: getopt.c,v 1.4.2.1 2005-08-13 11:28:08 prousu Exp $ */
/* Copyright (c) Slash'EM Development Team 2002 */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

/*
 * This module allows option value to be read by reading the values of
 * the game's internal variables.
 */

/*
 * boolopt[] is a private array of the options module. Defining the structure
 * here is a bit of a hack. If we don't end up merging this module with the
 * core options module then we should probably share information about this
 * variable in a rather more sensible fashion. --ALI
 */

static struct Bool_Opt
{
    const char *name;
    boolean *addr, initvalue;
    int optflags;
} *boolopt = (struct Bool_Opt *)0;

static int
get_option_bool(const char *option)
{
    int i;
    if (!boolopt)
	boolopt = (struct Bool_Opt *)nh_option_get_boolopt();
    for(i = 0; boolopt[i].name; i++)
	if (!strcmp(option, boolopt[i].name)) {
	    return boolopt[i].addr ? *(boolopt[i].addr) : FALSE;
	}
    if (!strcmp(option, "female"))  return flags.female;
    if (!strcmp(option, "male"))  return !flags.female;
    return -2;
}

static void
encode_escapes(buf, syms, length)
char *buf;
uchar *syms;
int length;
{
    int i, j;
    uchar v;
    for(i = j = 0; i < length; i++) {
	v = syms[i];
	if (v == 0 || v > 126) {
	    sprintf(buf + j, "\\%03d", v);
	    j += 4;
	} else if (v == '\\') {
	    buf[j++] = '\\';
	    buf[j++] = '\\';
	} else if (v < 0x20) {
	    buf[j++] = '^';
	    buf[j++] = v | 0x40;
	} else
	    buf[j++] = v;
    }
    buf[j] = '\0';
}

static char *
get_graphics_opts(buf, maxlen, offset)
char *buf;
int maxlen, offset;
{
    /* [ALI] Perhaps we should read save_showsyms on the rogue level? */
    encode_escapes(buf, showsyms + offset, maxlen);
    return buf;
}

/*
 * From options.c
 */

static void
oc_to_str(src,dest)
    char *src, *dest;
{
    int i;

    while ((i = (int) *src++) != 0) {
	if (i < 0 || i >= MAXOCLASSES)
	    impossible("oc_to_str:  illegal object class %d", i);
	else
	    *dest++ = def_oc_syms[i];
    }
    *dest = '\0';
}

char *get_option(const char *option)
{
    int b;
    static char buf[BUFSIZ];
    b = get_option_bool(option);
    if (b != -2)
	return b ? "yes" : "no";
    if (!strcmp(option, "pettype"))
	return preferred_pet == 'd' ? "dog" : preferred_pet == 'c' ? "cat" : "random";
    if (!strcmp(option, "ghoulname"))
	return ghoulname;
    if (!strcmp(option, "wolfname"))
	return wolfname;
    if (!strcmp(option, "catname"))
	return catname;
    if (!strcmp(option, "dogname"))
	return dogname;
    if (!strcmp(option, "horsename"))
	return horsename;
    if (!strcmp(option, "msghistory")) {
	sprintf(buf, "%d", iflags.msg_history);
	return buf;
    }
    if (!strcmp(option, "use_stone")) {
#if defined(CHANGE_COLOR) && defined(MAC)
	sprintf(buf, "%d", iflags.use_stone);
	return buf;
#else
	return "0";
#endif
    }
    /* background, use_stone, palette and hicolor are write only */
    if (!strcmp(option, "fruit"))  return pl_fruit;
    if (!strcmp(option, "graphics"))
	return get_graphics_opts(buf, MAXPCHARS, 0);
    if (!strcmp(option, "dungeon"))
	return get_graphics_opts(buf, MAXDCHARS, 0);
    if (!strcmp(option, "traps"))
	return get_graphics_opts(buf, MAXTCHARS, MAXDCHARS);
    if (!strcmp(option, "effects"))
	return get_graphics_opts(buf, MAXECHARS, MAXDCHARS + MAXTCHARS);
    if (!strcmp(option, "objects")) {
	encode_escapes(buf, oc_syms + 1, MAXOCLASSES - 1);
	return buf;
    }
    if (!strcmp(option, "monsters")) {
	encode_escapes(buf, monsyms + 1, MAXMCLASSES - 1);
	return buf;
    }
    if (!strcmp(option, "warnings")) {
	encode_escapes(buf, warnsyms, WARNCOUNT);
	return buf;
    }
    if (!strcmp(option, "name"))  return plname;
    if (!strcmp(option, "role"))  return pl_character;
    if (!strcmp(option, "race")) {
	buf[0] = pl_race;
	buf[1] = '\0';
	return buf;
    }
    if (!strcmp(option, "gender"))  return flags.initgend ? "female" : "male";
    if (!strcmp(option, "align"))
	switch (flags.initalign) {
	    case ROLE_LAWFUL:	return "lawful";
	    case ROLE_NEUTRAL:	return "neutral";
	    case ROLE_CHAOTIC:	return "chaotic";
	    default:	return "random";
	}
    if (!strcmp(option, "packorder"))  return flags.inv_order;
    if (!strcmp(option, "pickup_burden"))
	switch (flags.pickup_burden) {
	    case UNENCUMBERED:	return "u";
	    case SLT_ENCUMBER:	return "b";
	    case MOD_ENCUMBER:	return "s";
	    case HVY_ENCUMBER:	return "n";
	    case EXT_ENCUMBER:	return "o";
	    default:	return "l";
	}
    if (!strcmp(option, "pickup_types")) {
	oc_to_str(flags.pickup_types, buf);
	return buf;
    }
    if (!strcmp(option, "disclose"))  return flags.end_disclose;
    if (!strcmp(option, "scores")) {
	sprintf(buf, "%d top/ %d around%s", flags.end_top, flags.end_around,
		flags.end_own ? "/own" : "");
	return buf;
    }
    if (!strcmp(option, "suppress_alert")) {
	sprintf(buf, "%lu.%lu.%lu", FEATURE_NOTICE_VER_MAJ,
		FEATURE_NOTICE_VER_MIN, FEATURE_NOTICE_VER_PATCH);
	return buf;
    }
    if (!strcmp(option, "tileset"))  return tileset;
#ifdef VIDEOSHADES
    if (!strcmp(option, "videocolors")) {
	int i, icolor;
	char *bp = buf;
	icolor = CLR_RED;
	for(i = 0; i < 12; i++) {
	    if (icolor < (CLR_WHITE)) {
		if (bp != buf)
		    *bp++ = '-';
		sprintf(bp, "%d", ttycolors[icolor++]);
		bp = eos(bp);
		if ((icolor > CLR_CYAN) && (icolor < CLR_ORANGE))
		     icolor = CLR_ORANGE;
	    }
	}
	return buf;
    }
# ifdef MSDOS
    if (!strcmp(option, "videoshades")) {
	sprintf(buf, "%s-%s-%s",
		ttycolors[CLR_BLACK] == M_BLACK ? "dark" :
		ttycolors[CLR_BLACK] == M_GRAY ?  "normal" : "light",
		ttycolors[CLR_GRAY] == M_BLACK ? "dark" :
		ttycolors[CLR_GRAY] == M_GRAY ?  "normal" : "light",
		ttycolors[CLR_WHITE] == M_BLACK ? "dark" :
		ttycolors[CLR_WHITE] == M_GRAY ?  "normal" : "light");
    }
# endif
#endif
    if (!strcmp(option, "menustyle")) {
	switch (flags.menu_style) {
	    default:
	    case MENU_TRADITIONAL:	return "traditional";
	    case MENU_COMBINATION:	return "combination";
	    case MENU_PARTIAL:		return "partial";
	    case MENU_FULL:		return "full";
	}
    }
    if (!strcmp(option, "number_pad"))
	return iflags.num_pad ? iflags.num_pad_mode ? "2" : "1" : "0";
    return "";
}
