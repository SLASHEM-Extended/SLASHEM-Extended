/*	SCCS Id: @(#)mapglyph.c	3.4	2003/01/08	*/
/* Copyright (c) David Cohrs, 1991				  */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#if defined(TTY_GRAPHICS)
#include "wintty.h"	/* for prototype of has_color() only */
#endif
#include "color.h"
#define HI_DOMESTIC CLR_WHITE	/* monst.c */

int explcolors[] = {
	CLR_BLACK,	/* dark    */
	CLR_GREEN,	/* noxious */
	CLR_BROWN,	/* muddy   */
	CLR_BLUE,	/* wet     */
	CLR_MAGENTA,	/* magical */
	CLR_ORANGE,	/* fiery   */
	CLR_WHITE,	/* frosty  */
};

#if !defined(TTY_GRAPHICS)
#define has_color(n)  TRUE
#endif

#ifdef TEXTCOLOR
#define zap_color(n)  color = iflags.use_color ? (Hallucination ? rn2(15) : (BlackNgWalls || have_blackystone() || u.uprops[BLACK_NG_WALLS].extrinsic ) ? CLR_BLACK : zapcolors[n]) : NO_COLOR
#define cmap_color(n) color = iflags.use_color ? (Hallucination ? rn2(15) : (BlackNgWalls || have_blackystone() || u.uprops[BLACK_NG_WALLS].extrinsic ) ? CLR_BLACK : defsyms[n].color) : NO_COLOR
#define obj_color(n)  color = iflags.use_color ? (Hallucination ? rn2(15) : (BlackNgWalls || have_blackystone() || u.uprops[BLACK_NG_WALLS].extrinsic ) ? CLR_BLACK : (ShadesOfGrey || u.uprops[SHADES_OF_GREY].extrinsic || have_shadesofgreystone()) ? ( (objects[n].oc_color == CLR_RED) ? CLR_BLACK : (objects[n].oc_color == CLR_BROWN) ? CLR_BLACK : (objects[n].oc_color == CLR_BLUE) ? CLR_BLACK : (objects[n].oc_color == CLR_GREEN) ? CLR_GRAY : (objects[n].oc_color == CLR_MAGENTA) ? CLR_GRAY : (objects[n].oc_color == CLR_CYAN) ? CLR_GRAY : (objects[n].oc_color == CLR_BRIGHT_BLUE) ? CLR_GRAY : (objects[n].oc_color == CLR_YELLOW) ? CLR_WHITE : (objects[n].oc_color == CLR_ORANGE) ? CLR_WHITE : (objects[n].oc_color == CLR_BRIGHT_GREEN) ? CLR_WHITE : (objects[n].oc_color == CLR_BRIGHT_MAGENTA) ? CLR_WHITE : (objects[n].oc_color == CLR_BRIGHT_CYAN) ? CLR_WHITE : objects[n].oc_color) : objects[n].oc_color) : NO_COLOR
#define mon_color(n)  color = iflags.use_color ? (Hallucination ? rn2(15) : (BlackNgWalls || have_blackystone() || u.uprops[BLACK_NG_WALLS].extrinsic ) ? CLR_BLACK : ( (ShadesOfGrey || u.uprops[SHADES_OF_GREY].extrinsic || have_shadesofgreystone()) && Infravision) ? ( (mons[n].mcolor == CLR_RED) ? CLR_BLACK : (mons[n].mcolor == CLR_BROWN) ? CLR_BLACK : (mons[n].mcolor == CLR_BLUE) ? CLR_BLACK : (mons[n].mcolor == CLR_GREEN) ? CLR_GRAY : (mons[n].mcolor == CLR_MAGENTA) ? CLR_GRAY : (mons[n].mcolor == CLR_CYAN) ? CLR_GRAY : (mons[n].mcolor == CLR_BRIGHT_BLUE) ? CLR_GRAY : (mons[n].mcolor == CLR_YELLOW) ? CLR_WHITE : (mons[n].mcolor == CLR_ORANGE) ? CLR_WHITE : (mons[n].mcolor == CLR_BRIGHT_GREEN) ? CLR_WHITE : (mons[n].mcolor == CLR_BRIGHT_MAGENTA) ? CLR_WHITE : (mons[n].mcolor == CLR_BRIGHT_CYAN) ? CLR_WHITE : mons[n].mcolor) : mons[n].mcolor) : NO_COLOR
#define invis_color(n) color = NO_COLOR
#define pet_color(n)  color = iflags.use_color ? (Hallucination ? rn2(15) : (BlackNgWalls || have_blackystone() || u.uprops[BLACK_NG_WALLS].extrinsic ) ? CLR_BLACK : ( (ShadesOfGrey || u.uprops[SHADES_OF_GREY].extrinsic || have_shadesofgreystone()) && Infravision) ? ( (mons[n].mcolor == CLR_RED) ? CLR_BLACK : (mons[n].mcolor == CLR_BROWN) ? CLR_BLACK : (mons[n].mcolor == CLR_BLUE) ? CLR_BLACK : (mons[n].mcolor == CLR_GREEN) ? CLR_GRAY : (mons[n].mcolor == CLR_MAGENTA) ? CLR_GRAY : (mons[n].mcolor == CLR_CYAN) ? CLR_GRAY : (mons[n].mcolor == CLR_BRIGHT_BLUE) ? CLR_GRAY : (mons[n].mcolor == CLR_YELLOW) ? CLR_WHITE : (mons[n].mcolor == CLR_ORANGE) ? CLR_WHITE : (mons[n].mcolor == CLR_BRIGHT_GREEN) ? CLR_WHITE : (mons[n].mcolor == CLR_BRIGHT_MAGENTA) ? CLR_WHITE : (mons[n].mcolor == CLR_BRIGHT_CYAN) ? CLR_WHITE : mons[n].mcolor) : mons[n].mcolor) : NO_COLOR
#define warn_color(n) color = iflags.use_color ? (Hallucination ? rn2(15) : (BlackNgWalls || have_blackystone() || u.uprops[BLACK_NG_WALLS].extrinsic ) ? CLR_BLACK : def_warnsyms[n].color) : NO_COLOR
#define explode_color(n) color = iflags.use_color ? (Hallucination ? rn2(15) : (BlackNgWalls || have_blackystone() || u.uprops[BLACK_NG_WALLS].extrinsic ) ? CLR_BLACK : explcolors[n]) : NO_COLOR
# if defined(REINCARNATION) && defined(ASCIIGRAPH)
#  define ROGUE_COLOR
# endif

#else	/* no text color */

#define zap_color(n)
#define cmap_color(n)
#define obj_color(n)
#define mon_color(n)
#define invis_color(n)
#define pet_color(c)
#define warn_color(n)
#define explode_color(n)
#endif

#ifdef ROGUE_COLOR
# if defined(USE_TILES) && defined(MSDOS)
#define HAS_ROGUE_IBM_GRAPHICS (iflags.IBMgraphics && !iflags.grmode && \
	Is_rogue_level(&u.uz))
# else
#define HAS_ROGUE_IBM_GRAPHICS (iflags.IBMgraphics && Is_rogue_level(&u.uz))
# endif
#endif

/*ARGSUSED*/
void
mapglyph(glyph, ochar, ocolor, ospecial, x, y)
int glyph, *ocolor, x, y;
int *ochar;
unsigned *ospecial;
{
	register int offset;
#if defined(TEXTCOLOR) || defined(ROGUE_COLOR)
	int color = NO_COLOR;
#endif
	uchar ch;
	unsigned special = 0;

    /*
     *  Map the glyph back to a character and color.
     *
     *  Warning:  For speed, this makes an assumption on the order of
     *		  offsets.  The order is set in display.h.
     */
    if ((offset = (glyph - GLYPH_WARNING_OFF)) >= 0) {	/* a warning flash */
    	ch = warnsyms[offset];
# ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS)
	    color = NO_COLOR;
	else
# endif
	    warn_color(offset);
    } else if ((offset = (glyph - GLYPH_SWALLOW_OFF)) >= 0) {	/* swallow */
	/* see swallow_to_glyph() in display.c */
	ch = (uchar) showsyms[S_sw_tl + (offset & 0x7)];
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS && iflags.use_color)
	    color = NO_COLOR;
	else
#endif
	    mon_color(offset >> 3);
    } else if ((offset = (glyph - GLYPH_ZAP_OFF)) >= 0) {	/* zap beam */
	/* see zapdir_to_glyph() in display.c */
	ch = showsyms[S_vbeam + (offset & 0x3)];
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS && iflags.use_color)
	    color = NO_COLOR;
	else
#endif
	    zap_color((offset >> 2));
    } else if ((offset = (glyph - GLYPH_EXPLODE_OFF)) >= 0) {	/* explosion */
	ch = showsyms[(offset % MAXEXPCHARS) + S_explode1];
	explode_color(offset / MAXEXPCHARS);
    } else if ((offset = (glyph - GLYPH_CMAP_OFF)) >= 0) {	/* cmap */
	ch = showsyms[offset];
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS && iflags.use_color) {
	    if (offset >= S_vwall && offset <= S_hcdoor)
		color = CLR_BROWN;
	    else if (offset >= S_arrow_trap && offset <= S_trap_percents)
		color = CLR_MAGENTA;
	    else if (offset == S_corr || offset == S_litcorr)
		color = CLR_GRAY;
	    else if (offset >= S_room && offset <= S_water && offset != S_darkroom)
		color = CLR_GREEN;
	    else
		color = NO_COLOR;
	} else
#endif
#ifdef TEXTCOLOR
	    /* provide a visible difference if normal and lit corridor
	     * use the same symbol */
	    if (iflags.use_color &&
		offset == S_litcorr && ch == showsyms[S_corr])
		color = CLR_WHITE;
	    else if (iflags.use_color &&
		     (offset == S_upstair || offset == S_dnstair) &&
		     (x == sstairs.sx && y == sstairs.sy))
		color = CLR_YELLOW;
		/* special level colors by Amy, code partly stolen from dnethack */
	    else if(Is_lawful_quest(&u.uz)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_BLACK;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BLACK;
			}
			else cmap_color(offset);
		} else if(Is_nymph_level(&u.uz)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_GREEN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_GREEN;
			}
			else cmap_color(offset);
		} else if(Is_forge_level(&u.uz)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_ORANGE;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_RED;
			}
			else cmap_color(offset);
		} else if(Is_hitch_level(&u.uz)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_BRIGHT_CYAN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_CYAN;
			}
			else cmap_color(offset);
		} else if(Is_compu_level(&u.uz)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_YELLOW;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BROWN;
			}
			else cmap_color(offset);
		} else if(Is_key_level(&u.uz)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_MAGENTA;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_MAGENTA;
			}
			else cmap_color(offset);
		} else if(Is_mtemple_level(&u.uz)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_ORANGE;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_ORANGE;
			}
			else cmap_color(offset);
		} else if(Is_stronghold(&u.uz)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_RED;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_RED;
			}
			else cmap_color(offset);
		} else if(In_sheol(&u.uz)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_BRIGHT_CYAN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_CYAN;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,BEEHIVE)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_YELLOW;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_YELLOW;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,COURT)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_MAGENTA;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_MAGENTA;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,SWAMP)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_GREEN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_GREEN;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,VAULT)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_YELLOW;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_YELLOW;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,MORGUE)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_BLACK;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BLACK;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,BARRACKS)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_RED;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_RED;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,ZOO)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_BROWN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BROWN;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,REALZOO)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_BROWN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BROWN;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,DELPHI)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_BRIGHT_BLUE;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_BLUE;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,TEMPLE)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_CYAN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_CYAN;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,GIANTCOURT)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_CYAN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_CYAN;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,LEPREHALL)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_GREEN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_GREEN;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,DRAGONLAIR)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_BRIGHT_MAGENTA;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_MAGENTA;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,BADFOODSHOP)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_RED;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_RED;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,COCKNEST)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_YELLOW;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_YELLOW;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,ANTHOLE)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_BROWN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BROWN;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,LEMUREPIT)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_BLACK;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BLACK;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,MIGOHIVE)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_BRIGHT_GREEN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_GREEN;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,FUNGUSFARM)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_BRIGHT_GREEN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_GREEN;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,CLINIC)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_ORANGE;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_ORANGE;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,TERRORHALL)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_BRIGHT_CYAN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_CYAN;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,RIVERROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_BRIGHT_BLUE;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_BLUE;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,ELEMHALL)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_GRAY;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_GRAY;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,ANGELHALL)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_WHITE;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_WHITE;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,NYMPHHALL)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_GREEN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_GREEN;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,SPIDERHALL)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_GRAY;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_GRAY;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,TROLLHALL)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_BROWN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BROWN;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,HUMANHALL)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_BRIGHT_BLUE;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_BLUE;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,GOLEMHALL)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_GRAY;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_GRAY;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,COINHALL)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_YELLOW;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_YELLOW;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,DOUGROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_BRIGHT_CYAN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_CYAN;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,ARMORY)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_CYAN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_CYAN;
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,TENSHALL)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = rn2(15);
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = rn2(15);
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,INSIDEROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = rn2(15);
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = rn2(15);
			}
			else cmap_color(offset);
		} else if(*in_rooms(x,y,POOLROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = CLR_BRIGHT_BLUE;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_BLUE;
			}
			else cmap_color(offset);
		} else if(*in_roomscolouur(x,y,1) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = 1;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 1;
			}
			else cmap_color(offset);
		} else if(*in_roomscolouur(x,y,2) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = 2;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 2;
			}
			else cmap_color(offset);
		} else if(*in_roomscolouur(x,y,3) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = 3;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 3;
			}
			else cmap_color(offset);
		} else if(*in_roomscolouur(x,y,4) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = 4;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 4;
			}
			else cmap_color(offset);
		} else if(*in_roomscolouur(x,y,5) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = 5;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 5;
			}
			else cmap_color(offset);
		} else if(*in_roomscolouur(x,y,6) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = 6;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 6;
			}
			else cmap_color(offset);
		} else if(*in_roomscolouur(x,y,7) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = 7;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 7;
			}
			else cmap_color(offset);
		} else if(*in_roomscolouur(x,y,8) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = 8;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 8;
			}
			else cmap_color(offset);
		} else if(*in_roomscolouur(x,y,9) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = 9;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 9;
			}
			else cmap_color(offset);
		} else if(*in_roomscolouur(x,y,10) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = 10;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 10;
			}
			else cmap_color(offset);
		} else if(*in_roomscolouur(x,y,11) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = 11;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 11;
			}
			else cmap_color(offset);
		} else if(*in_roomscolouur(x,y,12) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = 12;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 12;
			}
			else cmap_color(offset);
		} else if(*in_roomscolouur(x,y,13) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = 13;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 13;
			}
			else cmap_color(offset);
		} else if(*in_roomscolouur(x,y,14) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = 14;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 14;
			}
			else cmap_color(offset);
		} else if(*in_roomscolouur(x,y,20) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor){
				color = rn2(15);
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = rn2(15);
			}
			else cmap_color(offset);
		}
		else
#endif
	    if (color == NO_COLOR) cmap_color(offset);
		if (BlackNgWalls || have_blackystone() || u.uprops[BLACK_NG_WALLS].extrinsic ) color = CLR_BLACK;
		else if (ShadesOfGrey || u.uprops[SHADES_OF_GREY].extrinsic || have_shadesofgreystone() ) {
			switch (color) {
				case CLR_RED:
				case CLR_BROWN:
				case CLR_BLUE:
					color = CLR_BLACK;
					break;
				case CLR_GREEN:
				case CLR_MAGENTA:
				case CLR_CYAN:
				case CLR_BRIGHT_BLUE:
					color = CLR_GRAY;
					break;
				case CLR_YELLOW:
				case CLR_ORANGE:
				case CLR_BRIGHT_GREEN:
				case CLR_BRIGHT_MAGENTA:
				case CLR_BRIGHT_CYAN:
					color = CLR_WHITE;
					break;
			}
		}
		if (Hallucination) color = rn2(15);
    } else if ((offset = (glyph - GLYPH_OBJ_OFF)) >= 0) {	/* object */
	if (On_stairs(x,y) && levl[x][y].seenv) special |= MG_STAIRS;
	if (offset == BOULDER && iflags.bouldersym) ch = iflags.bouldersym;
	else ch = oc_syms[(int)objects[offset].oc_class];
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS && iflags.use_color) {
	    switch(objects[offset].oc_class) {
		case COIN_CLASS: color = CLR_YELLOW; break;
		case FOOD_CLASS: color = CLR_RED; break;
		default: color = CLR_BRIGHT_BLUE; break;
	    }
	} else
#endif
	    obj_color(offset);

	    struct obj *ptr;
	    int hilitecnt = 0;

	    for (ptr = level.objects[x][y]; ptr; ptr = ptr->nexthere) {
	        if (ptr->otyp == STATUE) continue;
	        if (ptr->oinvis) continue;
	        if (ptr->oinvisreal) continue;
	        hilitecnt += 1;
	        if (hilitecnt >= 2) break;
	    }

	    if (offset != BOULDER && hilitecnt >= 2) 
		  special |= MG_OBJPILE;

    } else if ((offset = (glyph - GLYPH_RIDDEN_OFF)) >= 0) {	/* mon ridden */
	ch = monsyms[(int)mons[offset].mlet];
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS)
	    /* This currently implies that the hero is here -- monsters */
	    /* don't ride (yet...).  Should we set it to yellow like in */
	    /* the monster case below?  There is no equivalent in rogue. */
	    color = NO_COLOR;	/* no need to check iflags.use_color */
	else
#endif
	    mon_color(offset);
	    special |= MG_RIDDEN;
    } else if ((offset = (glyph - GLYPH_BODY_OFF)) >= 0) {	/* a corpse */
	if (On_stairs(x,y) && levl[x][y].seenv) special |= MG_STAIRS;
	ch = oc_syms[(int)objects[CORPSE].oc_class];
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS && iflags.use_color)
	    color = CLR_RED;
	else
#endif
	    mon_color(offset);
	    special |= MG_CORPSE;

	    struct obj *ptr;
	    int hilitecnt = 0;

	    for (ptr = level.objects[x][y]; ptr; ptr = ptr->nexthere) {
	        if (ptr->otyp == STATUE) continue;
	        if (ptr->oinvis) continue;
	        if (ptr->oinvisreal) continue;
	        hilitecnt += 1;
	        if (hilitecnt >= 2) break;
	    }

	    if (offset != BOULDER && hilitecnt >= 2) 
		  special |= MG_OBJPILE;

    } else if ((offset = (glyph - GLYPH_DETECT_OFF)) >= 0) {	/* mon detect */
	ch = monsyms[(int)mons[offset].mlet];
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS)
	    color = NO_COLOR;	/* no need to check iflags.use_color */
	else
#endif
	    mon_color(offset);
	/* Disabled for now; anyone want to get reverse video to work? */
	/* is_reverse = TRUE; */
	    special |= MG_DETECT;
    } else if ((offset = (glyph - GLYPH_INVIS_OFF)) >= 0) {	/* invisible */
	ch = DEF_INVISIBLE;
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS)
	    color = NO_COLOR;	/* no need to check iflags.use_color */
	else
#endif
	    invis_color(offset);
	    special |= MG_INVIS;
    } else if ((offset = (glyph - GLYPH_PET_OFF)) >= 0) {	/* a pet */
	ch = monsyms[(int)mons[offset].mlet];
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS)
	    color = NO_COLOR;	/* no need to check iflags.use_color */
	else
#endif
	    pet_color(offset);
	    special |= MG_PET;
    } else {							/* a monster */

	struct monst *mtmp;

	ch = monsyms[(int)mons[glyph].mlet];

		mtmp = m_at(x, y);
		if (mtmp && mtmp->isegotype && !UninformationProblem && !u.uprops[UNINFORMATION].extrinsic && !have_uninformationstone() ) special |= MG_EGOTYPE;
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS && iflags.use_color) {
	    if (x == u.ux && y == u.uy)
		/* actually player should be yellow-on-gray if in a corridor */
		color = CLR_YELLOW;
	    else
		color = NO_COLOR;
	} else
#endif
	{
	    mon_color(glyph);
	    /* special case the hero for `showrace' option */
#ifdef TEXTCOLOR
	    if (iflags.use_color && x == u.ux && y == u.uy &&
		    iflags.showrace && !Upolyd)
		color = HI_DOMESTIC;
#endif
	}
    }

#ifdef TEXTCOLOR
    /* Turn off color if no color defined, or rogue level w/o PC graphics. */
# ifdef REINCARNATION
#  ifdef ASCIIGRAPH
    if (!has_color(color) || (Is_rogue_level(&u.uz) && !HAS_ROGUE_IBM_GRAPHICS))
#  else
    if (!has_color(color) || Is_rogue_level(&u.uz))
#  endif
# else
    if (!has_color(color))
# endif
	color = NO_COLOR;
#endif

    *ochar = (int)ch;
    *ospecial = special;
#ifdef TEXTCOLOR
    *ocolor = color;
#endif
    return;
}

/*mapglyph.c*/
