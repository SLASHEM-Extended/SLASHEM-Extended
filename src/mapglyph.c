/*	SCCS Id: @(#)mapglyph.c	3.4	2003/01/08	*/
/* Copyright (c) David Cohrs, 1991				  */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#if defined(TTY_GRAPHICS)
#include "wintty.h"	/* for prototype of has_color() only */
#endif
#include "color.h"
#define HI_DOMESTIC CLR_WHITE	/* monst.c */

#include <ctype.h>	/* metanite64: for toupper() prototype */

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

STATIC_DCL int zap_color(int);
STATIC_DCL int cmap_color(int);
STATIC_DCL int obj_color(int);
STATIC_DCL int mon_color(int);
STATIC_DCL int invis_color(int);
STATIC_DCL int pet_color(int);
STATIC_DCL int warn_color(int);
STATIC_DCL int explode_color(int);

/* Yes kerio, I know that my code quality is shit and that you won't touch this file with a 20 foot pole. --Amy */

STATIC_OVL
int
zap_color(n)
int n;
{
	int color;
	if (!iflags.use_color) {
		return NO_COLOR;
	}

	color = zapcolors[n];
	if (ColorshiftEffect || u.uprops[COLORSHIFT_EFFECT].extrinsic || have_colorshiftstone()) color = colorshifting(color);
	if (MiscolorEffect || u.uprops[MISCOLOR_EFFECT].extrinsic || have_miscolorstone()) color = miscoloring(color);
	if (SpellColorGreen) color = CLR_GREEN;
	if (uamul && uamul->oartifact == ART_FLEECY_GREEN) color = CLR_GREEN;
	if (FemtrapActiveMarlena && !rn2(10) ) color = rn2(2) ? CLR_GREEN : CLR_BRIGHT_GREEN;
	if (BlackNgWalls || (uamul && uamul->oartifact == ART_BLACK_DARKNESS) || have_blackystone() || u.uprops[BLACK_NG_WALLS].extrinsic ) color = CLR_BLACK;
	if (uarmh && uarmh->oartifact == ART_BLUE_SCREEN_OF_DEATH) color = CLR_BRIGHT_BLUE;
	if (everythingfleecy()) color = rnd(15);
	if (uarmg && uarmg->oartifact == ART_JONADAB_S_KEYCODE && !rn2(5)) color = rnd(15);
	if (uarmc && itemhasappearance(uarmc, APP_FLEECELING_CLOAK) && !rn2(5) ) color = rnd(15);
	if ((OneRainbowEffect || u.uprops[ONE_RAINBOW_EFFECT].extrinsic || have_onerainbowstone() || (uarm && uarm->oartifact == ART_YOU_REALLY_HAVE_A_TOTAL_DA) || autismweaponcheck(ART_TASTE_THE_RAINBOW)) && (color == u.onerainbownumber) ) color = rnd(15);
	if (uamul && uamul->oartifact == ART_SUPER_GRAPHICS_CARD_BUG) {
		if (color > 0 && color < 16) {
			color++;
			if (color > 15) color = 1;
		}
	}

	return color;
}

STATIC_OVL
int
cmap_color(n)
int n;
{
	int color;
	if (!iflags.use_color) {
		return NO_COLOR;
	}

	color = defsyms[n].color;
	if (ColorshiftEffect || u.uprops[COLORSHIFT_EFFECT].extrinsic || have_colorshiftstone()) color = colorshifting(color);
	if (MiscolorEffect || u.uprops[MISCOLOR_EFFECT].extrinsic || have_miscolorstone()) color = miscoloring(color);
	if (SpellColorGreen) color = CLR_GREEN;
	if (uamul && uamul->oartifact == ART_FLEECY_GREEN) color = CLR_GREEN;
	if (FemtrapActiveMarlena && !rn2(10) ) color = rn2(2) ? CLR_GREEN : CLR_BRIGHT_GREEN;
	if (BlackNgWalls || (uamul && uamul->oartifact == ART_BLACK_DARKNESS) || have_blackystone() || u.uprops[BLACK_NG_WALLS].extrinsic ) color = CLR_BLACK;
	if (uarmh && uarmh->oartifact == ART_BLUE_SCREEN_OF_DEATH) color = CLR_BRIGHT_BLUE;
	if (everythingfleecy()) color = rnd(15);
	if (uarmg && uarmg->oartifact == ART_JONADAB_S_KEYCODE && !rn2(5)) color = rnd(15);
	if (uarmc && itemhasappearance(uarmc, APP_FLEECELING_CLOAK) && !rn2(5) ) color = rnd(15);
	if ((OneRainbowEffect || u.uprops[ONE_RAINBOW_EFFECT].extrinsic || have_onerainbowstone() || (uarm && uarm->oartifact == ART_YOU_REALLY_HAVE_A_TOTAL_DA) || autismweaponcheck(ART_TASTE_THE_RAINBOW)) && (color == u.onerainbownumber) ) color = rnd(15);
	if (uamul && uamul->oartifact == ART_SUPER_GRAPHICS_CARD_BUG) {
		if (color > 0 && color < 16) {
			color++;
			if (color > 15) color = 1;
		}
	}

	return color;

}

STATIC_OVL
int
obj_color(n)
int n;
{
	int color;
	if (!iflags.use_color) {
		return NO_COLOR;
	}

	color = objects[n].oc_color;

	if (ShadesOfGrey || (uarm && uarm->oartifact == ART_GRAYSCALE_WANDERER) || autismweaponcheck(ART____SHADES_OF_GRAYSWANDIR) || (uarm && uarm->oartifact == ART_GREY_FUCKERY) || (uamul && uamul->oartifact == ART_COLORLESS_VARIETY) || u.uprops[SHADES_OF_GREY].extrinsic || have_shadesofgreystone() || autismweaponcheck(ART_VINTAGE_MEMORY) || isblait) {
		switch (objects[n].oc_color) {
			case CLR_RED:
			case CLR_BROWN:
			case CLR_BLUE:
				color = CLR_BLACK; break;
			case CLR_GREEN:
			case CLR_MAGENTA:
			case CLR_CYAN:
			case CLR_BRIGHT_BLUE:
				color = CLR_GRAY; break;
			case CLR_YELLOW:
			case CLR_ORANGE:
			case CLR_BRIGHT_GREEN:
			case CLR_BRIGHT_MAGENTA:
			case CLR_BRIGHT_CYAN:
				color = CLR_WHITE; break;
		}
	}

	if (ColorshiftEffect || u.uprops[COLORSHIFT_EFFECT].extrinsic || have_colorshiftstone()) color = colorshifting(color);
	if (MiscolorEffect || u.uprops[MISCOLOR_EFFECT].extrinsic || have_miscolorstone()) color = miscoloring(color);
	if (SpellColorGreen) color = CLR_GREEN;
	if (uamul && uamul->oartifact == ART_FLEECY_GREEN) color = CLR_GREEN;
	if (FemtrapActiveMarlena && !rn2(10) ) color = rn2(2) ? CLR_GREEN : CLR_BRIGHT_GREEN;
	if (BlackNgWalls || (uamul && uamul->oartifact == ART_BLACK_DARKNESS) || have_blackystone() || u.uprops[BLACK_NG_WALLS].extrinsic ) color = CLR_BLACK;
	if (uarmh && uarmh->oartifact == ART_BLUE_SCREEN_OF_DEATH) color = CLR_BRIGHT_BLUE;
	if (everythingfleecy()) color = rnd(15);
	if (uarmg && uarmg->oartifact == ART_JONADAB_S_KEYCODE && !rn2(5)) color = rnd(15);
	if (uarmc && itemhasappearance(uarmc, APP_FLEECELING_CLOAK) && !rn2(5) ) color = rnd(15);
	if ((OneRainbowEffect || u.uprops[ONE_RAINBOW_EFFECT].extrinsic || have_onerainbowstone() || (uarm && uarm->oartifact == ART_YOU_REALLY_HAVE_A_TOTAL_DA) || autismweaponcheck(ART_TASTE_THE_RAINBOW)) && (color == u.onerainbownumber) ) color = rnd(15);
	if (uamul && uamul->oartifact == ART_SUPER_GRAPHICS_CARD_BUG) {
		if (color > 0 && color < 16) {
			color++;
			if (color > 15) color = 1;
		}
	}

	return color;

}

STATIC_OVL
int
mon_color(n)
int n;
{
	int color;
	if (!iflags.use_color) {
		return NO_COLOR;
	}

	boolean greyshades = FALSE;
	if ((ShadesOfGrey || (uarm && uarm->oartifact == ART_GRAYSCALE_WANDERER) || autismweaponcheck(ART____SHADES_OF_GRAYSWANDIR) || (uarm && uarm->oartifact == ART_GREY_FUCKERY) || u.uprops[SHADES_OF_GREY].extrinsic || have_shadesofgreystone() || autismweaponcheck(ART_VINTAGE_MEMORY) ) && Infravision) greyshades = TRUE;
	if ((uamul && uamul->oartifact == ART_COLORLESS_VARIETY) || isblait) greyshades = TRUE;

	color = mons[n].mcolor;

	if (mons[n].mflags4 & M4_MULTIHUED) color = rnd(15);

	if (greyshades) {
		switch (mons[n].mcolor) {
			case CLR_RED:
			case CLR_BROWN:
			case CLR_BLUE:
				color = CLR_BLACK; break;
			case CLR_GREEN:
			case CLR_MAGENTA:
			case CLR_CYAN:
			case CLR_BRIGHT_BLUE:
				color = CLR_GRAY; break;
			case CLR_YELLOW:
			case CLR_ORANGE:
			case CLR_BRIGHT_GREEN:
			case CLR_BRIGHT_MAGENTA:
			case CLR_BRIGHT_CYAN:
				color = CLR_WHITE; break;
		}
	}

	if (ColorshiftEffect || u.uprops[COLORSHIFT_EFFECT].extrinsic || have_colorshiftstone()) color = colorshifting(color);
	if (MiscolorEffect || u.uprops[MISCOLOR_EFFECT].extrinsic || have_miscolorstone()) color = miscoloring(color);
	if (SpellColorGreen) color = CLR_GREEN;
	if (uamul && uamul->oartifact == ART_FLEECY_GREEN) color = CLR_GREEN;
	if (FemtrapActiveMarlena && !rn2(10) ) color = rn2(2) ? CLR_GREEN : CLR_BRIGHT_GREEN;
	if (BlackNgWalls || (uamul && uamul->oartifact == ART_BLACK_DARKNESS) || have_blackystone() || u.uprops[BLACK_NG_WALLS].extrinsic ) color = CLR_BLACK;
	if (uarmc && itemhasappearance(uarmc, APP_COLORFADE_CLOAK) ) color = CLR_BLACK;
	if (uarmh && uarmh->oartifact == ART_BLUE_SCREEN_OF_DEATH) color = CLR_BRIGHT_BLUE;
	if (everythingfleecy()) color = rnd(15);
	if (uarmg && uarmg->oartifact == ART_JONADAB_S_KEYCODE && !rn2(5)) color = rnd(15);
	if (uarmc && itemhasappearance(uarmc, APP_FLEECELING_CLOAK) && !rn2(5) ) color = rnd(15);
	if ((OneRainbowEffect || u.uprops[ONE_RAINBOW_EFFECT].extrinsic || have_onerainbowstone() || (uarm && uarm->oartifact == ART_YOU_REALLY_HAVE_A_TOTAL_DA) || autismweaponcheck(ART_TASTE_THE_RAINBOW)) && (color == u.onerainbownumber) ) color = rnd(15);
	if (uamul && uamul->oartifact == ART_SUPER_GRAPHICS_CARD_BUG) {
		if (color > 0 && color < 16) {
			color++;
			if (color > 15) color = 1;
		}
	}

	return color;

}

STATIC_OVL
int
invis_color(n)
int n;
{
	return NO_COLOR;
}

STATIC_OVL
int
pet_color(n)
int n;
{
	int color;
	if (!iflags.use_color) {
		return NO_COLOR;
	}

	boolean greyshades = FALSE;
	if ((ShadesOfGrey || (uarm && uarm->oartifact == ART_GRAYSCALE_WANDERER) || autismweaponcheck(ART____SHADES_OF_GRAYSWANDIR) || (uarm && uarm->oartifact == ART_GREY_FUCKERY) || u.uprops[SHADES_OF_GREY].extrinsic || have_shadesofgreystone() || autismweaponcheck(ART_VINTAGE_MEMORY) ) && Infravision) greyshades = TRUE;
	if ((uamul && uamul->oartifact == ART_COLORLESS_VARIETY) || isblait) greyshades = TRUE;

	color = mons[n].mcolor;

	if (mons[n].mflags4 & M4_MULTIHUED) color = rnd(15);

	if (greyshades) {
		switch (mons[n].mcolor) {
			case CLR_RED:
			case CLR_BROWN:
			case CLR_BLUE:
				color = CLR_BLACK; break;
			case CLR_GREEN:
			case CLR_MAGENTA:
			case CLR_CYAN:
			case CLR_BRIGHT_BLUE:
				color = CLR_GRAY; break;
			case CLR_YELLOW:
			case CLR_ORANGE:
			case CLR_BRIGHT_GREEN:
			case CLR_BRIGHT_MAGENTA:
			case CLR_BRIGHT_CYAN:
				color = CLR_WHITE; break;
		}
	}

	if (ColorshiftEffect || u.uprops[COLORSHIFT_EFFECT].extrinsic || have_colorshiftstone()) color = colorshifting(color);
	if (MiscolorEffect || u.uprops[MISCOLOR_EFFECT].extrinsic || have_miscolorstone()) color = miscoloring(color);
	if (SpellColorGreen) color = CLR_GREEN;
	if (uamul && uamul->oartifact == ART_FLEECY_GREEN) color = CLR_GREEN;
	if (FemtrapActiveMarlena && !rn2(10) ) color = rn2(2) ? CLR_GREEN : CLR_BRIGHT_GREEN;
	if (BlackNgWalls || (uamul && uamul->oartifact == ART_BLACK_DARKNESS) || have_blackystone() || u.uprops[BLACK_NG_WALLS].extrinsic ) color = CLR_BLACK;
	if (uarmc && itemhasappearance(uarmc, APP_COLORFADE_CLOAK) ) color = CLR_BLACK;
	if (uarmh && uarmh->oartifact == ART_BLUE_SCREEN_OF_DEATH) color = CLR_BRIGHT_BLUE;
	if (everythingfleecy()) color = rnd(15);
	if (uarmg && uarmg->oartifact == ART_JONADAB_S_KEYCODE && !rn2(5)) color = rnd(15);
	if (uarmc && itemhasappearance(uarmc, APP_FLEECELING_CLOAK) && !rn2(5) ) color = rnd(15);
	if ((OneRainbowEffect || u.uprops[ONE_RAINBOW_EFFECT].extrinsic || have_onerainbowstone() || (uarm && uarm->oartifact == ART_YOU_REALLY_HAVE_A_TOTAL_DA) || autismweaponcheck(ART_TASTE_THE_RAINBOW)) && (color == u.onerainbownumber) ) color = rnd(15);
	if (uamul && uamul->oartifact == ART_SUPER_GRAPHICS_CARD_BUG) {
		if (color > 0 && color < 16) {
			color++;
			if (color > 15) color = 1;
		}
	}

	return color;

}

STATIC_OVL
int
warn_color(n)
int n;
{
	int color;
	if (!iflags.use_color) {
		return NO_COLOR;
	}

	color = def_warnsyms[n].color;
	if (ColorshiftEffect || u.uprops[COLORSHIFT_EFFECT].extrinsic || have_colorshiftstone()) color = colorshifting(color);
	if (MiscolorEffect || u.uprops[MISCOLOR_EFFECT].extrinsic || have_miscolorstone()) color = miscoloring(color);
	if (SpellColorGreen) color = CLR_GREEN;
	if (uamul && uamul->oartifact == ART_FLEECY_GREEN) color = CLR_GREEN;
	if (FemtrapActiveMarlena && !rn2(10) ) color = rn2(2) ? CLR_GREEN : CLR_BRIGHT_GREEN;
	if (BlackNgWalls || (uamul && uamul->oartifact == ART_BLACK_DARKNESS) || have_blackystone() || u.uprops[BLACK_NG_WALLS].extrinsic ) color = CLR_BLACK;
	if (uarmh && uarmh->oartifact == ART_BLUE_SCREEN_OF_DEATH) color = CLR_BRIGHT_BLUE;
	if (everythingfleecy()) color = rnd(15);
	if (uarmg && uarmg->oartifact == ART_JONADAB_S_KEYCODE && !rn2(5)) color = rnd(15);
	if (uarmc && itemhasappearance(uarmc, APP_FLEECELING_CLOAK) && !rn2(5) ) color = rnd(15);
	if ((OneRainbowEffect || u.uprops[ONE_RAINBOW_EFFECT].extrinsic || have_onerainbowstone() || (uarm && uarm->oartifact == ART_YOU_REALLY_HAVE_A_TOTAL_DA) || autismweaponcheck(ART_TASTE_THE_RAINBOW)) && (color == u.onerainbownumber) ) color = rnd(15);
	if (uamul && uamul->oartifact == ART_SUPER_GRAPHICS_CARD_BUG) {
		if (color > 0 && color < 16) {
			color++;
			if (color > 15) color = 1;
		}
	}

	return color;

}

STATIC_OVL
int
explode_color(n)
int n;
{
	int color;
	if (!iflags.use_color) {
		return NO_COLOR;
	}

	color = explcolors[n];
	if (ColorshiftEffect || u.uprops[COLORSHIFT_EFFECT].extrinsic || have_colorshiftstone()) color = colorshifting(color);
	if (MiscolorEffect || u.uprops[MISCOLOR_EFFECT].extrinsic || have_miscolorstone()) color = miscoloring(color);
	if (SpellColorGreen) color = CLR_GREEN;
	if (uamul && uamul->oartifact == ART_FLEECY_GREEN) color = CLR_GREEN;
	if (FemtrapActiveMarlena && !rn2(10) ) color = rn2(2) ? CLR_GREEN : CLR_BRIGHT_GREEN;
	if (BlackNgWalls || (uamul && uamul->oartifact == ART_BLACK_DARKNESS) || have_blackystone() || u.uprops[BLACK_NG_WALLS].extrinsic ) color = CLR_BLACK;
	if (uarmh && uarmh->oartifact == ART_BLUE_SCREEN_OF_DEATH) color = CLR_BRIGHT_BLUE;
	if (everythingfleecy()) color = rnd(15);
	if (uarmg && uarmg->oartifact == ART_JONADAB_S_KEYCODE && !rn2(5)) color = rnd(15);
	if (uarmc && itemhasappearance(uarmc, APP_FLEECELING_CLOAK) && !rn2(5) ) color = rnd(15);
	if ((OneRainbowEffect || u.uprops[ONE_RAINBOW_EFFECT].extrinsic || have_onerainbowstone() || (uarm && uarm->oartifact == ART_YOU_REALLY_HAVE_A_TOTAL_DA) || autismweaponcheck(ART_TASTE_THE_RAINBOW)) && (color == u.onerainbownumber) ) color = rnd(15);
	if (uamul && uamul->oartifact == ART_SUPER_GRAPHICS_CARD_BUG) {
		if (color > 0 && color < 16) {
			color++;
			if (color > 15) color = 1;
		}
	}

	return color;

}

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
	register struct engr *ep = engr_at(x,y);
	register int offset;
#if defined(TEXTCOLOR) || defined(ROGUE_COLOR)
	int color = NO_COLOR;
#endif
	uchar ch;
	unsigned special = 0;
	register struct trap *ttmp;

	register struct rm *colorroom;
	colorroom = &levl[x][y];

    /*
     *  Map the glyph back to a character and color.
     *
     *  Warning:  For speed, this makes an assumption on the order of
     *		  offsets.  The order is set in display.h.
     */
    if ((offset = (glyph - GLYPH_WARNING_OFF)) >= 0) {	/* a warning flash */
	if (On_stairs(x,y) && levl[x][y].seenv && !ThereIsNoLite) special |= MG_STAIRS;
	if (flags.graffitihilite && ep && ep->engr_txt[0] && !Blind && levl[x][y].seenv && !ThereIsNoLite) special |= MG_ENGRAVING;
    	ch = warnsyms[offset];
# ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS)
	    color = NO_COLOR;
	else
# endif
	    color = warn_color(offset);
    } else if ((offset = (glyph - GLYPH_SWALLOW_OFF)) >= 0) {	/* swallow */
	/* see swallow_to_glyph() in display.c */
	ch = (uchar) showsyms[S_sw_tl + (offset & 0x7)];
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS && iflags.use_color)
	    color = NO_COLOR;
	else
#endif
	    color = mon_color(offset >> 3);
    } else if ((offset = (glyph - GLYPH_ZAP_OFF)) >= 0) {	/* zap beam */
	/* see zapdir_to_glyph() in display.c */
	ch = showsyms[S_vbeam + (offset & 0x3)];
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS && iflags.use_color)
	    color = NO_COLOR;
	else
#endif
	    color = zap_color((offset >> 2));
    } else if ((offset = (glyph - GLYPH_EXPLODE_OFF)) >= 0) {	/* explosion */
	ch = showsyms[(offset % MAXEXPCHARS) + S_explode1];
	color = explode_color(offset / MAXEXPCHARS);
    } else if ((offset = (glyph - GLYPH_CMAP_OFF)) >= 0) {	/* cmap */
	if (On_stairs(x,y) && (ttmp = t_at(x,y)) && offset != S_grayglyph && !ThereIsNoLite && (ttmp && ttmp->tseen) && levl[x][y].seenv) special |= MG_STAIRS;
	if (flags.graffitihilite && offset != S_grayglyph && ep && ep->engr_txt[0] && !Blind && levl[x][y].seenv && !ThereIsNoLite) special |= MG_ENGRAVING;
	ch = showsyms[offset];
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS && iflags.use_color) {
	    if (offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall))
		color = CLR_BROWN;
	    else if (offset >= S_arrow_trap && offset <= S_timerun_trap)
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
		color = CLR_BRIGHT_MAGENTA;
		/* special level colors by Amy, code partly stolen from dnethack */
	    else if(Is_lawful_quest(&u.uz)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BLACK;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BLACK;
			}
			else color = cmap_color(offset);
		} else if(Is_nymph_level(&u.uz)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_GREEN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_GREEN;
			}
			else color = cmap_color(offset);
		} else if(In_spacebase(&u.uz)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BRIGHT_BLUE;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_BLUE;
			}
			else color = cmap_color(offset);
		} else if(In_sewerplant(&u.uz)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BROWN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_GRAY;
			}
			else color = cmap_color(offset);
		} else if(In_gammacaves(&u.uz)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = rn2(2) ? CLR_CYAN : CLR_BRIGHT_CYAN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = rn2(10) ? CLR_BRIGHT_CYAN : rn2(2) ? CLR_BRIGHT_GREEN : CLR_ORANGE;
			}
			else color = cmap_color(offset);
		} else if(In_mainframe(&u.uz)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BRIGHT_GREEN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_GREEN;
			}
			else color = cmap_color(offset);
		} else if(Is_forge_level(&u.uz)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_ORANGE;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_RED;
			}
			else color = cmap_color(offset);
		} else if(Is_hitch_level(&u.uz)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BRIGHT_CYAN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_CYAN;
			}
			else color = cmap_color(offset);
		} else if(Is_compu_level(&u.uz)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_YELLOW;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BROWN;
			}
			else color = cmap_color(offset);
		} else if(Is_key_level(&u.uz)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_MAGENTA;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_MAGENTA;
			}
			else color = cmap_color(offset);
		} else if(Is_mtemple_level(&u.uz)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_ORANGE;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_ORANGE;
			}
			else color = cmap_color(offset);
		} else if(Is_stronghold(&u.uz)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_RED;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_RED;
			}
			else color = cmap_color(offset);
		} else if(In_sheol(&u.uz)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BRIGHT_CYAN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_CYAN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,BEEHIVE)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_YELLOW;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_YELLOW;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,COURT)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_MAGENTA;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_MAGENTA;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,SWAMP)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_GREEN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_GREEN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,VAULT)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_YELLOW;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_YELLOW;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,MORGUE)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BLACK;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BLACK;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,BARRACKS)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_RED;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_RED;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,ZOO)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BROWN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BROWN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,REALZOO)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BROWN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BROWN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,DELPHI)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BRIGHT_BLUE;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_BLUE;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,TEMPLE)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_CYAN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_CYAN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,GIANTCOURT)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_CYAN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_CYAN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,LEPREHALL)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_GREEN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_GREEN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,DRAGONLAIR)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BRIGHT_MAGENTA;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_MAGENTA;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,BADFOODSHOP)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_RED;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_RED;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,COCKNEST)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_YELLOW;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_YELLOW;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,ANTHOLE)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BROWN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BROWN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,LEMUREPIT)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BLACK;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BLACK;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,MIGOHIVE)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BRIGHT_GREEN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_GREEN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,FUNGUSFARM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BRIGHT_GREEN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_GREEN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,CLINIC)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_ORANGE;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_ORANGE;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,TERRORHALL)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BRIGHT_CYAN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_CYAN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,RIVERROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BRIGHT_BLUE;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_BLUE;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,ELEMHALL)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_GRAY;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_GRAY;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,ANGELHALL)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_WHITE;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_WHITE;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,NYMPHHALL)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_GREEN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_GREEN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,SPIDERHALL)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_GRAY;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_GRAY;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,TROLLHALL)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BROWN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BROWN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,HUMANHALL)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BRIGHT_BLUE;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_BLUE;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,GOLEMHALL)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_GRAY;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_GRAY;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,CRYPTROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BRIGHT_BLUE;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_BLUE;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,TROUBLEZONE)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_MAGENTA;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_MAGENTA;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,WEAPONCHAMBER)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BROWN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BROWN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,HELLPIT)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_ORANGE;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_ORANGE;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,ROBBERCAVE)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_GRAY;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_GRAY;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,SANITATIONCENTRAL)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_CYAN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_CYAN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,FEMINISMROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BRIGHT_MAGENTA;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_MAGENTA;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,MEADOWROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BRIGHT_GREEN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_GREEN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,COOLINGCHAMBER)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BRIGHT_CYAN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_CYAN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,VOIDROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_MAGENTA;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_MAGENTA;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,HAMLETROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_GREEN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_GREEN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,KOPSTATION)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_MAGENTA;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_MAGENTA;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,BOSSROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = rnd(15);
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = rnd(15);
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,RNGCENTER)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = rnd(15);
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = rnd(15);
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,VARIANTROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = rnd(15);
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = rnd(15);
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,WIZARDSDORM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_CYAN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_CYAN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,DOOMEDBARRACKS)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BLACK;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BLACK;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,SLEEPINGROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_GRAY;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_GRAY;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,DIVERPARADISE)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BRIGHT_BLUE;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_BLUE;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,MENAGERIE)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_YELLOW;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_YELLOW;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,EMPTYDESERT)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BROWN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BROWN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,RARITYROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_YELLOW;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_YELLOW;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,EXHIBITROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_RED;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_RED;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,PRISONCHAMBER)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_WHITE;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_WHITE;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,NUCLEARCHAMBER)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_ORANGE;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_ORANGE;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,LEVELSEVENTYROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BLACK;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BLACK;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,PLAYERCENTRAL)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_MAGENTA;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_MAGENTA;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,COINHALL)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_YELLOW;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_YELLOW;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,DOUGROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BRIGHT_CYAN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_CYAN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,ARMORY)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_CYAN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_CYAN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,TENSHALL)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = rnd(15);
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = rnd(15);
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,EVILROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = rn2(2) ? CLR_MAGENTA : CLR_BRIGHT_MAGENTA;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = rn2(2) ? CLR_MAGENTA : CLR_BRIGHT_MAGENTA;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,RELIGIONCENTER)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BLACK;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BLACK;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,CHAOSROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = rnd(15);
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = rnd(15);
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,CURSEDMUMMYROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_CYAN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_CYAN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,MIXEDPOOL)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = rn2(2) ? CLR_BRIGHT_BLUE : CLR_BRIGHT_CYAN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = rn2(2) ? CLR_BRIGHT_BLUE : CLR_BRIGHT_CYAN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,ARDUOUSMOUNTAIN)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_WHITE;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_WHITE;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,CHANGINGROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BRIGHT_MAGENTA;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_MAGENTA;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,QUESTORROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_MAGENTA;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_MAGENTA;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,LEVELFFROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_GRAY;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_GRAY;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,VERMINROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BROWN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BROWN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,MIRASPA)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_YELLOW;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_YELLOW;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,MACHINEROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_GRAY;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_GRAY;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,SHOWERROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BRIGHT_BLUE;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_BLUE;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,GREENCROSSROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BRIGHT_GREEN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_GREEN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,CENTRALTEDIUM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_GREEN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_GREEN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,RUINEDCHURCH)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BLACK;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BLACK;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,RAMPAGEROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_GRAY;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_GRAY;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,GAMECORNER)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BRIGHT_BLUE;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_BLUE;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,ILLUSIONROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = !rn2(3) ? CLR_WHITE : !rn2(2) ? CLR_GRAY : CLR_CYAN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = !rn2(3) ? CLR_WHITE : !rn2(2) ? CLR_GRAY : CLR_CYAN;
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,INSIDEROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = rnd(15);
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = rnd(15);
			}
			else color = cmap_color(offset);
		} else if(*in_rooms(x,y,POOLROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = CLR_BRIGHT_BLUE;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = CLR_BRIGHT_BLUE;
			}
			else color = cmap_color(offset);
		} else if(*in_roomscolouur(x,y,1) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 1;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 1;
			}
			else color = cmap_color(offset);
		} else if(*in_roomscolouur(x,y,2) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 2;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 2;
			}
			else color = cmap_color(offset);
		} else if(*in_roomscolouur(x,y,3) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 3;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 3;
			}
			else color = cmap_color(offset);
		} else if(*in_roomscolouur(x,y,4) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 4;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 4;
			}
			else color = cmap_color(offset);
		} else if(*in_roomscolouur(x,y,5) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 5;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 5;
			}
			else color = cmap_color(offset);
		} else if(*in_roomscolouur(x,y,6) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 6;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 6;
			}
			else color = cmap_color(offset);
		} else if(*in_roomscolouur(x,y,7) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 7;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 7;
			}
			else color = cmap_color(offset);
		} else if(*in_roomscolouur(x,y,8) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 8;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 8;
			}
			else color = cmap_color(offset);
		} else if(*in_roomscolouur(x,y,9) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 9;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 9;
			}
			else color = cmap_color(offset);
		} else if(*in_roomscolouur(x,y,10) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 10;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 10;
			}
			else color = cmap_color(offset);
		} else if(*in_roomscolouur(x,y,11) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 11;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 11;
			}
			else color = cmap_color(offset);
		} else if(*in_roomscolouur(x,y,12) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 12;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 12;
			}
			else color = cmap_color(offset);
		} else if(*in_roomscolouur(x,y,13) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 13;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 13;
			}
			else color = cmap_color(offset);
		} else if(*in_roomscolouur(x,y,14) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 14;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 14;
			}
			else color = cmap_color(offset);
		} else if(*in_roomscolouur(x,y,15) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 15;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 15;
			}
			else color = cmap_color(offset);
		} else if(*in_roomscolouur(x,y,20) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = rnd(15);
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = rnd(15);
			}
			else color = cmap_color(offset);
		} else if(*in_roomscolouur(x,y,21) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = rn2(2) ? CLR_MAGENTA : CLR_BRIGHT_MAGENTA;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = rn2(2) ? CLR_MAGENTA : CLR_BRIGHT_MAGENTA;
			}
			else color = cmap_color(offset);
		} else if(*in_roomscolouur(x,y,22) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = rn2(2) ? CLR_BRIGHT_CYAN : CLR_BRIGHT_BLUE;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = rn2(2) ? CLR_BRIGHT_CYAN : CLR_BRIGHT_BLUE;
			}
			else color = cmap_color(offset);
		} else if(*in_roomscolouur(x,y,23) && *in_rooms(x,y,OROOM)) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = !rn2(3) ? CLR_WHITE : !rn2(2) ? CLR_GRAY : CLR_CYAN;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = !rn2(3) ? CLR_WHITE : !rn2(2) ? CLR_GRAY : CLR_CYAN;
			}
			else color = cmap_color(offset);
		}
		else if (colorroom->fleecycolor == 1) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 1;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 1;
			}
			else color = cmap_color(offset);
		}
		else if (colorroom->fleecycolor == 2) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 2;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 2;
			}
			else color = cmap_color(offset);
		}
		else if (colorroom->fleecycolor == 3) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 3;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 3;
			}
			else color = cmap_color(offset);
		}
		else if (colorroom->fleecycolor == 4) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 4;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 4;
			}
			else color = cmap_color(offset);
		}
		else if (colorroom->fleecycolor == 5) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 5;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 5;
			}
			else color = cmap_color(offset);
		}
		else if (colorroom->fleecycolor == 6) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 6;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 6;
			}
			else color = cmap_color(offset);
		}
		else if (colorroom->fleecycolor == 7) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 7;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 7;
			}
			else color = cmap_color(offset);
		}
		else if (colorroom->fleecycolor == 8) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 8;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 8;
			}
			else color = cmap_color(offset);
		}
		else if (colorroom->fleecycolor == 9) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 9;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 9;
			}
			else color = cmap_color(offset);
		}
		else if (colorroom->fleecycolor == 10) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 10;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 10;
			}
			else color = cmap_color(offset);
		}
		else if (colorroom->fleecycolor == 11) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 11;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 11;
			}
			else color = cmap_color(offset);
		}
		else if (colorroom->fleecycolor == 12) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 12;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 12;
			}
			else color = cmap_color(offset);
		}
		else if (colorroom->fleecycolor == 13) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 13;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 13;
			}
			else color = cmap_color(offset);
		}
		else if (colorroom->fleecycolor == 14) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 14;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 14;
			}
			else color = cmap_color(offset);
		}
		else if (colorroom->fleecycolor == 15) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 15;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 15;
			}
			else color = cmap_color(offset);
		}
		else if (colorroom->fleecycolor == 16) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = rnd(15);
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = rnd(15);
			}
			else color = cmap_color(offset);
		}
		else if ((100 + u.monstertimefinish + depth(&u.uz)) % 16 == 0) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 1;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 1;
			}
			else color = cmap_color(offset);
		} else if ((101 + u.monstertimefinish + depth(&u.uz)) % 16 == 0) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 2;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 2;
			}
			else color = cmap_color(offset);
		} else if ((102 + u.monstertimefinish + depth(&u.uz)) % 16 == 0) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 3;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 3;
			}
			else color = cmap_color(offset);
		} else if ((103 + u.monstertimefinish + depth(&u.uz)) % 16 == 0) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 4;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 4;
			}
			else color = cmap_color(offset);
		} else if ((104 + u.monstertimefinish + depth(&u.uz)) % 16 == 0) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 5;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 5;
			}
			else color = cmap_color(offset);
		} else if ((105 + u.monstertimefinish + depth(&u.uz)) % 16 == 0) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 6;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 6;
			}
			else color = cmap_color(offset);
		} else if ((106 + u.monstertimefinish + depth(&u.uz)) % 16 == 0) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 7;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 7;
			}
			else color = cmap_color(offset);
		} else if ((107 + u.monstertimefinish + depth(&u.uz)) % 16 == 0) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 8;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 8;
			}
			else color = cmap_color(offset);
		} else if ((108 + u.monstertimefinish + depth(&u.uz)) % 16 == 0) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 9;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 9;
			}
			else color = cmap_color(offset);
		} else if ((109 + u.monstertimefinish + depth(&u.uz)) % 16 == 0) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 10;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 10;
			}
			else color = cmap_color(offset);
		} else if ((110 + u.monstertimefinish + depth(&u.uz)) % 16 == 0) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 11;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 11;
			}
			else color = cmap_color(offset);
		} else if ((111 + u.monstertimefinish + depth(&u.uz)) % 16 == 0) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 12;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 12;
			}
			else color = cmap_color(offset);
		} else if ((112 + u.monstertimefinish + depth(&u.uz)) % 16 == 0) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 13;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 13;
			}
			else color = cmap_color(offset);
		} else if ((113 + u.monstertimefinish + depth(&u.uz)) % 16 == 0) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 14;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 14;
			}
			else color = cmap_color(offset);
		} else if ((114 + u.monstertimefinish + depth(&u.uz)) % 16 == 0) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = 15;
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = 15;
			}
			else color = cmap_color(offset);
		} else if ((115 + u.monstertimefinish + depth(&u.uz)) % 16 == 0) {
			if(offset >= S_vwall && offset <= S_hcdoor && !(offset >= S_rockwall && offset <= S_tunnelwall)){
				color = rnd(15);
			}
			else if(offset >= S_corr && offset <= S_litcorr){
				color = rnd(15);
			}
			else color = cmap_color(offset);
		} else
#endif
	    if (color == NO_COLOR) color = cmap_color(offset);

		if (ColorshiftEffect || u.uprops[COLORSHIFT_EFFECT].extrinsic || have_colorshiftstone()) color = colorshifting(color);
		if (MiscolorEffect || u.uprops[MISCOLOR_EFFECT].extrinsic || have_miscolorstone()) color = miscoloring(color);

		if (uarmh && uarmh->oartifact == ART_BLUE_SCREEN_OF_DEATH) color = CLR_BRIGHT_BLUE;
		else if (BlackNgWalls || (uamul && uamul->oartifact == ART_BLACK_DARKNESS) || have_blackystone() || u.uprops[BLACK_NG_WALLS].extrinsic ) color = CLR_BLACK;
		else if (FemtrapActiveMarlena && !rn2(10) ) color = (rn2(2) ? CLR_GREEN : CLR_BRIGHT_GREEN);
		else if (uamul && uamul->oartifact == ART_FLEECY_GREEN) color = CLR_GREEN;
		else if (SpellColorGreen) color = CLR_GREEN;
		else if (ShadesOfGrey || (uarm && uarm->oartifact == ART_GRAYSCALE_WANDERER) || autismweaponcheck(ART____SHADES_OF_GRAYSWANDIR) || (uarm && uarm->oartifact == ART_GREY_FUCKERY) || (uamul && uamul->oartifact == ART_COLORLESS_VARIETY) || u.uprops[SHADES_OF_GREY].extrinsic || have_shadesofgreystone() || autismweaponcheck(ART_VINTAGE_MEMORY) || isblait ) {
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
		if (SpellColorMetal && (offset >= S_vwall && offset <= S_hcdoor) && !(offset > S_rockwall && offset <= S_tunnelwall)) color = rn2(2) ? CLR_CYAN : CLR_BRIGHT_CYAN;
		if (SpellColorMetal && (offset == S_dungwall)) color = rn2(2) ? CLR_CYAN : CLR_BRIGHT_CYAN;
		if (WallsAreHyperBlue && (offset >= S_vwall && offset <= S_hcdoor) && !(offset > S_rockwall && offset <= S_tunnelwall)) color = CLR_BRIGHT_BLUE;
		if (WallsAreHyperBlue && (offset == S_dungwall)) color = CLR_BRIGHT_BLUE;
		if (SpellColorOrange && offset == S_grayglyph) color = CLR_ORANGE;
		if (SpellColorBrightCyan && offset == S_grayglyph) color = CLR_BRIGHT_CYAN;
		if (Race_if(PM_RELEASIER) && !rn2(3)) color = !rn2(3) ? CLR_CYAN : !rn2(2) ? CLR_WHITE : CLR_RED;
		if ((KnowledgeBug || u.uprops[KNOWLEDGE_BUG].extrinsic || have_trapknowledgestone()) && (offset == S_grayglyph)) color = rnd(15);
		if (Hallucination) color = rnd(15);
		if ((uarmc && itemhasappearance(uarmc, APP_FLEECELING_CLOAK)) && !rn2(5) ) color = rnd(15);
		if (uarmg && uarmg->oartifact == ART_JONADAB_S_KEYCODE && !rn2(5)) color = rnd(15);
		if (everythingfleecy()) color = rnd(15);
		if ((OneRainbowEffect || u.uprops[ONE_RAINBOW_EFFECT].extrinsic || have_onerainbowstone() || (uarm && uarm->oartifact == ART_YOU_REALLY_HAVE_A_TOTAL_DA) || autismweaponcheck(ART_TASTE_THE_RAINBOW)) && (color == u.onerainbownumber) ) color = rnd(15);
		if (uamul && uamul->oartifact == ART_SUPER_GRAPHICS_CARD_BUG) {
			if (color > 0 && color < 16) {
				color++;
				if (color > 15) color = 1;
			}
		}
    } else if ((offset = (glyph - GLYPH_OBJ_OFF)) >= 0) {	/* object */
	if (On_stairs(x,y) && !ThereIsNoLite && levl[x][y].seenv) special |= MG_STAIRS;
	if (flags.graffitihilite && ep && ep->engr_txt[0] && !Blind && levl[x][y].seenv && !ThereIsNoLite) special |= MG_ENGRAVING;
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
	    color = obj_color(offset);

	    struct obj *ptr;
	    int hilitecnt = 0;

	    if (level.objlist) {

		    for (ptr = level.objects[x][y]; ptr; ptr = ptr->nexthere) {
		        if (ptr && ptr->otyp == STATUE) continue;
		        if (ptr && ptr->oinvis) continue;
		        if (ptr && ptr->oinvisreal) continue;
		        hilitecnt += 1;
		        if (hilitecnt >= 2) break;
		    }
	    }

	    if (offset != BOULDER && !ThereIsNoLite && hilitecnt >= 2) 
		  special |= MG_OBJPILE;

    } else if ((offset = (glyph - GLYPH_RIDDEN_OFF)) >= 0) {	/* mon ridden */
	ch = monsyms[(int)mons[offset].mlet];
	if (CapsBugEffect || u.uprops[CAPS_BUG_EFFECT].extrinsic || have_capsstone()) ch = toupper(ch);
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS)
	    /* This currently implies that the hero is here -- monsters */
	    /* don't ride (yet...).  Should we set it to yellow like in */
	    /* the monster case below?  There is no equivalent in rogue. */
	    color = NO_COLOR;	/* no need to check iflags.use_color */
	else
#endif
	    color = mon_color(offset);
	    special |= MG_RIDDEN;
    } else if ((offset = (glyph - GLYPH_BODY_OFF)) >= 0) {	/* a corpse */
	if (On_stairs(x,y) && !ThereIsNoLite && levl[x][y].seenv) special |= MG_STAIRS;
	if (flags.graffitihilite && ep && ep->engr_txt[0] && !Blind && levl[x][y].seenv && !ThereIsNoLite) special |= MG_ENGRAVING;
	ch = oc_syms[(int)objects[CORPSE].oc_class];
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS && iflags.use_color)
	    color = CLR_RED;
	else
#endif
	    color = mon_color(offset);
	    special |= MG_CORPSE;

	    struct obj *ptr;
	    int hilitecnt = 0;

	    if (level.objlist) {
		    for (ptr = level.objects[x][y]; ptr; ptr = ptr->nexthere) {
		        if (ptr->otyp == STATUE) continue;
		        if (ptr->oinvis) continue;
		        if (ptr->oinvisreal) continue;
		        hilitecnt += 1;
		        if (hilitecnt >= 2) break;
		    }
		}

	    if (offset != BOULDER && !ThereIsNoLite && hilitecnt >= 2) 
		  special |= MG_OBJPILE;

    } else if ((offset = (glyph - GLYPH_DETECT_OFF)) >= 0) {	/* mon detect */
	if (On_stairs(x,y) && !ThereIsNoLite && levl[x][y].seenv) special |= MG_STAIRS;
	/*if (flags.graffitihilite && ep && ep->engr_txt[0] && !Blind && levl[x][y].seenv && !ThereIsNoLite) special |= MG_ENGRAVING;*/
	ch = monsyms[(int)mons[offset].mlet];
	if (CapsBugEffect || u.uprops[CAPS_BUG_EFFECT].extrinsic || have_capsstone()) ch = toupper(ch);
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS)
	    color = NO_COLOR;	/* no need to check iflags.use_color */
	else
#endif
	    color = mon_color(offset);
	/* Disabled for now; anyone want to get reverse video to work? */
	/* is_reverse = TRUE; */
	    special |= MG_DETECT;
    } else if ((offset = (glyph - GLYPH_INVIS_OFF)) >= 0) {	/* invisible */
	if (On_stairs(x,y) && !ThereIsNoLite && levl[x][y].seenv) special |= MG_STAIRS;
	if (flags.graffitihilite && ep && ep->engr_txt[0] && !Blind && levl[x][y].seenv && !ThereIsNoLite) special |= MG_ENGRAVING;
	ch = DEF_INVISIBLE;
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS)
	    color = NO_COLOR;	/* no need to check iflags.use_color */
	else
#endif
	    color = invis_color(offset);
	    special |= MG_INVIS;
    } else if ((offset = (glyph - GLYPH_PET_OFF)) >= 0) {	/* a pet */
	if (On_stairs(x,y) && !ThereIsNoLite && levl[x][y].seenv) special |= MG_STAIRS;
	/*if (flags.graffitihilite && ep && ep->engr_txt[0] && !Blind && levl[x][y].seenv && !ThereIsNoLite) special |= MG_ENGRAVING;*/
	ch = monsyms[(int)mons[offset].mlet];
	if (CapsBugEffect || u.uprops[CAPS_BUG_EFFECT].extrinsic || have_capsstone()) ch = toupper(ch);
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS)
	    color = NO_COLOR;	/* no need to check iflags.use_color */
	else
#endif
	    color = pet_color(offset);
	    if (!ThereIsNoLite) special |= MG_PET;
    } else {							/* a monster */
	if (On_stairs(x,y) && !ThereIsNoLite && levl[x][y].seenv) special |= MG_STAIRS;
	/*if (flags.graffitihilite && ep && ep->engr_txt[0] && !Blind && levl[x][y].seenv && !ThereIsNoLite) special |= MG_ENGRAVING;*/

	struct monst *mtmp;

	ch = monsyms[(int)mons[glyph].mlet];
	if (CapsBugEffect || u.uprops[CAPS_BUG_EFFECT].extrinsic || have_capsstone()) ch = toupper(ch);

	mtmp = m_at(x, y);
	if (mtmp && mtmp->isegotype && !mtmp->noegodisplay && !ThereIsNoLite && !PlayerUninformation) special |= MG_EGOTYPE;

	if (mtmp && !ThereIsNoLite && mtmp->mpeaceful && Peacevision && !Hallucination) special |= MG_PEACEFUL;

	if (mtmp && mtmp->mnum == PM_ULTRAYOUSEE) {
		if (!MessagesSuppressed && !Clairvoyant && !RightMouseButtonDoesNotGo) {
			MessageSuppression += 1; /* ugly hack */
			badeffect(); /* something bad happens and we don't tell you what it is :-P --Amy */
		}
	}

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
	    color = mon_color(glyph);
	    if (mtmp && mtmp->shinymonst) color = rnd(15);
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

/* color shift trap: every color has a matching color that it turns into, 50% of the time --Amy */
int
colorshifting(color)
int color;
{
	if (color == NO_COLOR) return NO_COLOR;

	int miscolor = color;
	switch (color) {
		case CLR_RED:
			miscolor = CLR_ORANGE; break;
		case CLR_BLUE:
			miscolor = CLR_BRIGHT_BLUE; break;
		case CLR_WHITE:
			miscolor = CLR_BLACK; break;
		case CLR_GREEN:
			miscolor = CLR_BRIGHT_GREEN; break;
		case CLR_BROWN:
			miscolor = CLR_YELLOW; break;
		case CLR_YELLOW:
			miscolor = CLR_BROWN; break;
		case CLR_BLACK:
			miscolor = CLR_WHITE; break;
		case CLR_GRAY:
			miscolor = CLR_GRAY; break; /* none available */
		case CLR_CYAN:
			miscolor = CLR_BRIGHT_CYAN; break;
		case CLR_MAGENTA:
			miscolor = CLR_BRIGHT_MAGENTA; break;
		case CLR_ORANGE:
			miscolor = CLR_RED; break;
		case CLR_BRIGHT_BLUE:
			miscolor = CLR_BLUE; break;
		case CLR_BRIGHT_CYAN:
			miscolor = CLR_CYAN; break;
		case CLR_BRIGHT_GREEN:
			miscolor = CLR_GREEN; break;
		case CLR_BRIGHT_MAGENTA:
			miscolor = CLR_MAGENTA; break;

	}
	if (rn2(2)) color = miscolor;
	return color;

}

int
miscoloring(color)
int color;
{
	if (color == NO_COLOR) return NO_COLOR;

	int miscolor = 0;

	switch (color) {
		case CLR_RED:
			miscolor = 1; break;
		case CLR_BLUE:
			miscolor = 2; break;
		case CLR_WHITE:
			miscolor = 3; break;
		case CLR_GREEN:
			miscolor = 4; break;
		case CLR_BROWN:
			miscolor = 5; break;
		case CLR_YELLOW:
			miscolor = 6; break;
		case CLR_BLACK:
			miscolor = 7; break;
		case CLR_GRAY:
			miscolor = 8; break;
		case CLR_CYAN:
			miscolor = 9; break;
		case CLR_MAGENTA:
			miscolor = 10; break;
		case CLR_ORANGE:
			miscolor = 11; break;
		case CLR_BRIGHT_BLUE:
			miscolor = 12; break;
		case CLR_BRIGHT_CYAN:
			miscolor = 13; break;
		case CLR_BRIGHT_GREEN:
			miscolor = 14; break;
		case CLR_BRIGHT_MAGENTA:
			miscolor = 15; break;
	}
	miscolor += u.miscolornumber;
	if (miscolor > 15) miscolor -= 15;

	switch (miscolor) {
		case 1:
			color = CLR_RED; break;
		case 2:
			color = CLR_BLUE; break;
		case 3:
			color = CLR_WHITE; break;
		case 4:
			color = CLR_GREEN; break;
		case 5:
			color = CLR_BROWN; break;
		case 6:
			color = CLR_YELLOW; break;
		case 7:
			color = CLR_BLACK; break;
		case 8:
			color = CLR_GRAY; break;
		case 9:
			color = CLR_CYAN; break;
		case 10:
			color = CLR_MAGENTA; break;
		case 11:
			color = CLR_ORANGE; break;
		case 12:
			color = CLR_BRIGHT_BLUE; break;
		case 13:
			color = CLR_BRIGHT_CYAN; break;
		case 14:
			color = CLR_BRIGHT_GREEN; break;
		case 15:
			color = CLR_BRIGHT_MAGENTA; break;
	}

	return color;
}

/*mapglyph.c*/
