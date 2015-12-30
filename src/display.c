/*	SCCS Id: @(#)display.c	3.4	2003/02/19	*/
/* Copyright (c) Dean Luick, with acknowledgements to Kevin Darcy */
/* and Dave Cohrs, 1990.					  */
/* NetHack may be freely redistributed.  See license for details. */

/*
 *			THE NEW DISPLAY CODE
 *
 * The old display code has been broken up into three parts: vision, display,
 * and drawing.  Vision decides what locations can and cannot be physically
 * seen by the hero.  Display decides _what_ is displayed at a given location.
 * Drawing decides _how_ to draw a monster, fountain, sword, etc.
 *
 * The display system uses information from the vision system to decide
 * what to draw at a given location.  The routines for the vision system
 * can be found in vision.c and vision.h.  The routines for display can
 * be found in this file (display.c) and display.h.  The drawing routines
 * are part of the window port.  See doc/window.doc for the drawing
 * interface.
 *
 * The display system deals with an abstraction called a glyph.  Anything
 * that could possibly be displayed has a unique glyph identifier.
 *
 * What is seen on the screen is a combination of what the hero remembers
 * and what the hero currently sees.  Objects and dungeon features (walls
 * doors, etc) are remembered when out of sight.  Monsters and temporary
 * effects are not remembered.  Each location on the level has an
 * associated glyph.  This is the hero's _memory_ of what he or she has
 * seen there before.
 *
 * Display rules:
 *
 *	If the location is in sight, display in order:
 *		visible (or sensed) monsters
 *		visible objects
 *		known traps
 *		background
 *
 *	If the location is out of sight, display in order:
 *		sensed monsters (telepathy)
 *		memory
 *
 *
 *
 * Here is a list of the major routines in this file to be used externally:
 *
 * newsym
 *
 * Possibly update the screen location (x,y).  This is the workhorse routine.
 * It is always correct --- where correct means following the in-sight/out-
 * of-sight rules.  **Most of the code should use this routine.**  This
 * routine updates the map and displays monsters.
 *
 *
 * map_background
 * map_object
 * map_trap
 * map_invisible
 * unmap_object
 *
 * If you absolutely must override the in-sight/out-of-sight rules, there
 * are two possibilities.  First, you can mess with vision to force the
 * location in sight then use newsym(), or you can  use the map_* routines.
 * The first has not been tried [no need] and the second is used in the
 * detect routines --- detect object, magic mapping, etc.  The map_*
 * routines *change* what the hero remembers.  All changes made by these
 * routines will be sticky --- they will survive screen redraws.  Do *not*
 * use these for things that only temporarily change the screen.  These
 * routines are also used directly by newsym().  unmap_object is used to
 * clear a remembered object when/if detection reveals it isn't there.
 *
 *
 * show_glyph
 *
 * This is direct (no processing in between) buffered access to the screen.
 * Temporary screen effects are run through this and its companion,
 * flush_screen().  There is yet a lower level routine, print_glyph(),
 * but this is unbuffered and graphic dependent (i.e. it must be surrounded
 * by graphic set-up and tear-down routines).  Do not use print_glyph().
 *
 *
 * see_monsters
 * see_objects
 * see_traps
 *
 * These are only used when something affects all of the monsters or
 * objects or traps.  For objects and traps, the only thing is hallucination.
 * For monsters, there are hallucination and changing from/to blindness, etc.
 *
 *
 * tmp_at
 *
 * This is a useful interface for displaying temporary items on the screen.
 * Its interface is different than previously, so look at it carefully.
 *
 *
 *
 * Parts of the rm structure that are used:
 *
 *	typ	- What is really there.
 *	glyph	- What the hero remembers.  This will never be a monster.
 *		  Monsters "float" above this.
 *	lit	- True if the position is lit.  An optimization for
 *		  lit/unlit rooms.
 *	waslit	- True if the position was *remembered* as lit.
 *	seenv	- A vector of bits representing the directions from which the
 *		  hero has seen this position.  The vector's primary use is
 *		  determining how walls are seen.  E.g. a wall sometimes looks
 *		  like stone on one side, but is seen as a wall from the other.
 *		  Other uses are for unmapping detected objects and felt
 *		  locations, where we need to know if the hero has ever
 *		  seen the location.
 *	flags   - Additional information for the typ field.  Different for
 *		  each typ.
 *	horizontal - Indicates whether the wall or door is horizontal or
 *		     vertical.
 */
#include "hack.h"
#include "region.h"

STATIC_DCL void FDECL(display_monster,(XCHAR_P,XCHAR_P,struct monst *,int,XCHAR_P));
STATIC_DCL int FDECL(swallow_to_glyph, (int, int));
STATIC_DCL void FDECL(display_warning,(struct monst *));

STATIC_DCL int FDECL(check_pos, (int, int, int));
#ifdef WA_VERBOSE
STATIC_DCL boolean FDECL(more_than_one, (int, int, int, int, int));
#endif
STATIC_DCL int FDECL(set_twall, (int,int, int,int, int,int, int,int));
STATIC_DCL int FDECL(set_wall, (int, int, int));
STATIC_DCL int FDECL(set_corn, (int,int, int,int, int,int, int,int));
STATIC_DCL int FDECL(set_crosswall, (int, int));
STATIC_DCL void FDECL(set_seenv, (struct rm *, int, int, int, int));
STATIC_DCL void FDECL(t_warn, (struct rm *));
STATIC_DCL int FDECL(wall_angle, (struct rm *));
STATIC_DCL int FDECL(back_to_cmap, (XCHAR_P, XCHAR_P));

STATIC_VAR boolean transp;    /* cached transparency flag for current tileset */

#ifdef INVISIBLE_OBJECTS
/*
 * vobj_at()
 *
 * Returns a pointer to an object if the hero can see an object at the
 * given location.  This takes care of invisible objects.  NOTE, this
 * assumes that the hero is not blind and on top of the object pile.
 * It does NOT take into account that the location is out of sight, or,
 * say, one can see blessed, etc.
 */
struct obj *
vobj_at(x,y)
    xchar x,y;
{
    register struct obj *obj = level.objects[x][y];

    while (obj) {
	if ((!obj->oinvis || See_invisible) && !obj->oinvisreal) return obj;
	obj = obj->nexthere;
    }
    return ((struct obj *) 0);
}
#endif	/* else vobj_at() is defined in display.h */

/*
 * magic_map_background()
 *
 * This function is similar to map_background (see below) except we pay
 * attention to and correct unexplored, lit ROOM and CORR spots.
 */
void
magic_map_background(x, y, show)
    xchar x,y;
    int  show;
{
    int cmap = back_to_cmap(x,y);	/* assumes hero can see x,y */
    struct rm *lev = &levl[x][y];

    /*
     * Correct for out of sight lit corridors and rooms that the hero
     * doesn't remember as lit.
     */
    if (!cansee(x,y) && !lev->waslit) {
	/* Floor spaces are dark if unlit.  Corridors are dark if unlit. */
	if (lev->typ == ROOM && cmap == S_room)
	    cmap = S_darkroom;
	else if (lev->typ == CORR && cmap == S_litcorr)
	    cmap = S_corr;
    }
    if (level.flags.hero_memory)
#ifdef DISPLAY_LAYERS
	lev->mem_bg = cmap;
#else
	lev->glyph = cmap_to_glyph(cmap);
#endif
    if (show || transp) show_glyph(x,y, cmap_to_glyph(cmap));
}

/*
 * The routines map_background(), map_object(), and map_trap() could just
 * as easily be:
 *
 *	map_glyph(x,y,glyph,show)
 *
 * Which is called with the xx_to_glyph() in the call.  Then I can get
 * rid of 3 routines that don't do very much anyway.  And then stop
 * having to create fake objects and traps.  However, I am reluctant to
 * make this change.
 */
/* FIXME: some of these use xchars for x and y, and some use ints.  Make
 * this consistent.
 */

/*
 * map_background()
 *
 * Make the real background part of our map.  This routine assumes that
 * the hero can physically see the location.  Update the screen if directed.
 */
void
map_background(x, y, show)
    register xchar x,y;
    register int  show;
{
    register int cmap = back_to_cmap(x,y);

    if (level.flags.hero_memory)
#ifdef DISPLAY_LAYERS
	levl[x][y].mem_bg = cmap;
#else
	levl[x][y].glyph = cmap_to_glyph(cmap);
#endif
    if (show || transp) show_glyph(x,y, cmap_to_glyph(cmap));
}

/*
 * map_trap()
 *
 * Map the trap and print it out if directed.  This routine assumes that the
 * hero can physically see the location.
 */
void
map_trap(trap, show)
    register struct trap *trap;
    register int	 show;
{
    register int x = trap->tx, y = trap->ty;
    register int cmap = trap_to_cmap(trap);

    if (level.flags.hero_memory)
#ifdef DISPLAY_LAYERS
	levl[x][y].mem_trap = 1 + cmap - MAXDCHARS;
#else
	levl[x][y].glyph = cmap_to_glyph(cmap);
#endif
    if (show || transp) show_glyph(x, y, cmap_to_glyph(cmap));
}

/*
 * map_object()
 *
 * Map the given object.  This routine assumes that the hero can physically
 * see the location of the object.  Update the screen if directed.
 */
void
map_object(obj, show)
    register struct obj *obj;
    register int	show;
{
    register int x = obj->ox, y = obj->oy;
    register int glyph = obj_to_glyph(obj);

	/*pline("object type %d",obj->otyp);*/

    if (level.flags.hero_memory)
#ifdef DISPLAY_LAYERS
	if ((levl[x][y].mem_corpse = glyph_is_body(glyph)))
	    levl[x][y].mem_obj = 1 + glyph_to_body(glyph);
	else
	    levl[x][y].mem_obj = 1 + glyph_to_obj(glyph);
#else
	levl[x][y].glyph = glyph;
#endif

    if (show) show_glyph(x, y, glyph);
}

/*
 * map_invisible()
 *
 * Make the hero remember that a square contains an invisible monster.
 * This is a special case in that the square will continue to be displayed
 * this way even when the hero is close enough to see it.  To get rid of
 * this and display the square's actual contents, use unmap_object() followed
 * by newsym() if necessary.
 */
void
map_invisible(x, y)
register xchar x, y;
{
    if (x != u.ux || y != u.uy) { /* don't display I at hero's location */
	if (level.flags.hero_memory)
#ifdef DISPLAY_LAYERS
	    levl[x][y].mem_invis = 1;
#else
	    levl[x][y].glyph = GLYPH_INVISIBLE;
#endif
	show_glyph(x, y, GLYPH_INVISIBLE);
    }
}

/*
 * unmap_object()
 *
 * Remove something from the map when the hero realizes it's not there any
 * more.  Replace it with background or known trap, but not with any other
 * If this is used for detection, a full screen update is imminent anyway;
 * if this is used to get rid of an invisible monster notation, we might have
 * to call newsym().
 */
void
unmap_object(x, y)
    register int x, y;
{
#ifndef DISPLAY_LAYERS
    register struct trap *trap;
#endif

    if (!level.flags.hero_memory) return;

#ifdef DISPLAY_LAYERS
    levl[x][y].mem_invis = levl[x][y].mem_corpse = levl[x][y].mem_obj = 0;
#else
    if ((trap = t_at(x,y)) != 0 && trap->tseen && !covers_traps(x,y))
	map_trap(trap, 0);
    else if (levl[x][y].seenv) {
	struct rm *lev = &levl[x][y];

	map_background(x, y, 0);

	/* turn remembered dark room squares dark */
	if (!lev->waslit && lev->glyph == cmap_to_glyph(S_room) &&
							    lev->typ == ROOM)
	    lev->glyph = cmap_to_glyph(S_darkroom);
    } else
	levl[x][y].glyph = cmap_to_glyph(S_stone);	/* default val */
#endif
}

#define DETECTED 	2
#define PHYSICALLY_SEEN 1
#define is_worm_tail(mon)	((mon) && ((x != (mon)->mx)  || (y != (mon)->my)))

/*
 * map_location()
 *
 * Make whatever at this location show up.  This is only for non-living
 * things.  This will not handle feeling invisible objects correctly.
 *
 * Internal to display.c, this is a #define for speed.
 */
#ifdef DISPLAY_LAYERS
#define _map_location(x,y,show)						\
{									\
    register struct obj   *obj;						\
    register struct trap  *trap;					\
									\
    if (level.flags.hero_memory) {					\
	if ((obj = vobj_at(x, y)) && !covers_objects(x, y))		\
	    map_object(obj, FALSE);					\
	else								\
	    levl[x][y].mem_corpse = levl[x][y].mem_obj = 0;		\
	if ((trap = t_at(x, y)) && trap->tseen && !covers_traps(x, y))	\
	    map_trap(trap, FALSE);					\
	else								\
	    levl[x][y].mem_trap = 0;					\
	map_background(x, y, FALSE);					\
	if (show) show_glyph(x, y, memory_glyph(x, y));			\
    } else if ((obj = vobj_at(x,y)) && !covers_objects(x,y))		\
	map_object(obj,show);						\
    else if ((trap = t_at(x,y)) && trap->tseen && !covers_traps(x,y))	\
	map_trap(trap,show);						\
    else								\
	map_background(x,y,show);					\
}
#else	/* DISPLAY_LAYERS */
#define _map_location(x,y,show)						\
{									\
    register struct obj   *obj;						\
    register struct trap  *trap;					\
									\
    if ((obj = vobj_at(x,y)) && !covers_objects(x,y))			\
	map_object(obj,show);						\
    else if ((trap = t_at(x,y)) && trap->tseen && !covers_traps(x,y))	\
	map_trap(trap,show);						\
    else								\
	map_background(x,y,show);					\
}
#endif	/* DISPLAY_LAYERS */

void
map_location(x,y,show)
    int x, y, show;
{
    _map_location(x,y,show);
}

int memory_glyph(x, y)
    int x, y;
{
#ifdef DISPLAY_LAYERS
    if (levl[x][y].mem_invis)
	return GLYPH_INVISIBLE;
    else if (levl[x][y].mem_obj)
	if (levl[x][y].mem_corpse)
	    return body_to_glyph(levl[x][y].mem_obj - 1);
	else
	    return objnum_to_glyph(levl[x][y].mem_obj - 1);
    else if (levl[x][y].mem_trap)
	return cmap_to_glyph(levl[x][y].mem_trap - 1 + MAXDCHARS);
    else
	return cmap_to_glyph(levl[x][y].mem_bg);
#else
    return levl[x][y].glyph;
#endif
}

void clear_memory_glyph(x, y, to)
    int x, y, to;
{
#ifdef DISPLAY_LAYERS
    levl[x][y].mem_bg = to;
    levl[x][y].mem_trap = 0;
    levl[x][y].mem_obj = 0;
    levl[x][y].mem_corpse = 0;
    levl[x][y].mem_invis = 0;
#else
    levl[x][y].glyph = cmap_to_glyph(to);
#endif
}

/*
 * display_monster()
 *
 * Note that this is *not* a map_XXXX() function!  Monsters sort of float
 * above everything.
 *
 * Yuck.  Display body parts by recognizing that the display position is
 * not the same as the monster position.  Currently the only body part is
 * a worm tail.
 *
 */
STATIC_OVL void
display_monster(x, y, mon, sightflags, worm_tail)
    register xchar x, y;	/* display position */
    register struct monst *mon;	/* monster to display */
    int sightflags;		/* 1 if the monster is physically seen */
    				/* 2 if detected using Detect_monsters */
    register xchar worm_tail;	/* mon is actually a worm tail */
{
    register boolean mon_mimic = (mon->m_ap_type != M_AP_NOTHING);
    register int sensed = mon_mimic &&
	( (Protection_from_shape_changers && !permamimic(mon->data) && !(mon->egotype_permamimic) ) || sensemon(mon));
    /*
     * We must do the mimic check first.  If the mimic is mimicing something,
     * and the location is in sight, we have to change the hero's memory
     * so that when the position is out of sight, the hero remembers what
     * the mimic was mimicing.
     */

    if (mon_mimic && (sightflags == PHYSICALLY_SEEN)) {
	switch (mon->m_ap_type) {
	    default:
		impossible("display_monster:  bad m_ap_type value [ = %d ]",
							(int) mon->m_ap_type);
	    case M_AP_NOTHING:
		show_glyph(x, y, mon_to_glyph(mon));
		break;

	    case M_AP_FURNITURE: {
		/*
		 * This is a poor man's version of map_background().  I can't
		 * use map_background() because we are overriding what is in
		 * the 'typ' field.  Maybe have map_background()'s parameters
		 * be (x,y,glyph) instead of just (x,y).
		 *
		 * mappearance is currently set to an S_ index value in
		 * makemon.c.
		 */
		register int glyph = cmap_to_glyph(mon->mappearance);
#ifdef DISPLAY_LAYERS
		levl[x][y].mem_bg = mon->mappearance;
#else
		levl[x][y].glyph = glyph;
#endif
		if (!sensed) show_glyph(x,y, glyph);
		break;
	    }

	    case M_AP_OBJECT: {
		struct obj obj;	/* Make a fake object to send	*/
				/* to map_object().		*/
		obj.ox = x;
		obj.oy = y;
		obj.otyp = mon->mappearance;
		obj.corpsenm = PM_TENGU;	/* if mimicing a corpse */
		map_object(&obj,!sensed);
		break;
	    }

	    case M_AP_MONSTER:
		show_glyph(x,y, monnum_to_glyph(what_mon((int)mon->mappearance)));
		break;
	}
	
    }

    /* If the mimic is unsucessfully mimicing something, display the monster */
    if (!mon_mimic || sensed) {
	int num;

	/* [ALI] Only use detected glyphs when monster wouldn't be
	 * visible by any other means.
	 */
	if (sightflags == DETECTED) {
	    if (worm_tail)
		num = detected_monnum_to_glyph(what_mon(PM_LONG_WORM_TAIL));
	    else
		num = detected_mon_to_glyph(mon);
	} else if (mon->mtame && !Hallucination) {
	    if (worm_tail)
		num = petnum_to_glyph(PM_LONG_WORM_TAIL);
	    else
		num = pet_to_glyph(mon);
	} else {
	    if (worm_tail)
		num = monnum_to_glyph(what_mon(PM_LONG_WORM_TAIL));
	    else
		num = mon_to_glyph(mon);
	}
	show_glyph(x,y,num);
    }
}
/*
 * display_warning()
 *
 * This is also *not* a map_XXXX() function!  Monster warnings float
 * above everything just like monsters do, but only if the monster
 * is not showing.
 *
 * Do not call for worm tails.
 */
STATIC_OVL void
display_warning(mon)
    register struct monst *mon;
{
    int x = mon->mx, y = mon->my;
    int wl = (int) (mon->m_lev / 6);
    int glyph;

    if (mon_warning(mon)) {
        if (wl > WARNCOUNT - 1) wl = WARNCOUNT - 1;
	/* 3.4.1: this really ought to be rn2(WARNCOUNT), but value "0"
	   isn't handled correctly by the what_is routine so avoid it 
	if (Hallucination) wl = rn1(WARNCOUNT-1,1);*/
	if (Hallucination) wl = rn2(WARNCOUNT); /* seems the error was a 1 in pager.c where it's supposed to be a 0 --Amy */
        glyph = warning_to_glyph(wl);
    } else if (MATCH_WARN_OF_MON(mon)) {
	glyph = mon_to_glyph(mon);
    } else {
    	impossible("display_warning did not match warning type?");
        return;
    }
    show_glyph(x, y, glyph);
}

/*
 * feel_location()
 *
 * Feel the given location.  This assumes that the hero is blind and that
 * the given position is either the hero's or one of the eight squares
 * adjacent to the hero (except for a boulder push).
 */
void
feel_location(x, y)
    xchar x, y;
{
    struct rm *lev = &(levl[x][y]);
    struct obj *boulder;
    register struct monst *mon;

    /* If the hero's memory of an invisible monster is accurate, we want to keep
     * him from detecting the same monster over and over again on each turn.
     * We must return (so we don't erase the monster).  (We must also, in the
     * search function, be sure to skip over previously detected 'I's.)
     */
    if (memory_is_invisible(x,y) && m_at(x,y)) return;

    /* The hero can't feel non pool locations while under water. */
    if (Underwater && !Is_waterlevel(&u.uz) && ! is_pool(x,y))
	return;

    /* Set the seen vector as if the hero had seen it.  It doesn't matter */
    /* if the hero is levitating or not.				  */
    set_seenv(lev, u.ux, u.uy, x, y);

    if (Levitation && !Is_airlevel(&u.uz) && !Is_waterlevel(&u.uz)) {
	/*
	 * Levitation Rules.  It is assumed that the hero can feel the state
	 * of the walls around herself and can tell if she is in a corridor,
	 * room, or doorway.  Boulders are felt because they are large enough.
	 * Anything else is unknown because the hero can't reach the ground.
	 * This makes things difficult.
	 *
	 * Check (and display) in order:
	 *
	 *	+ Stone, walls, and closed doors.
	 *	+ Boulders.  [see a boulder before a doorway]
	 *	+ Doors.
	 *	+ Room/water positions
	 *	+ Everything else (hallways!)
	 */
	if (IS_ROCK(lev->typ) || (IS_DOOR(lev->typ) &&
				(lev->doormask & (D_LOCKED | D_CLOSED)))) {
	    map_background(x, y, 1);
	} else if ((boulder = sobj_at(BOULDER,x,y)) != 0) {
	    map_object(boulder, 1);
	} else if (IS_DOOR(lev->typ)) {
	    map_background(x, y, 1);
	} else if (IS_ROOM(lev->typ) || IS_POOL(lev->typ)) {
	    /*
	     * An open room or water location.  Normally we wouldn't touch
	     * this, but we have to get rid of remembered boulder symbols.
	     * This will only occur in rare occations when the hero goes
	     * blind and doesn't find a boulder where expected (something
	     * came along and picked it up).  We know that there is not a
	     * boulder at this location.  Show fountains, pools, etc.
	     * underneath if already seen.  Otherwise, show the appropriate
	     * floor symbol.
	     *
	     * Similarly, if the hero digs a hole in a wall or feels a location
	     * that used to contain an unseen monster.  In these cases,
	     * there's no reason to assume anything was underneath, so
	     * just show the appropriate floor symbol.  If something was
	     * embedded in the wall, the glyph will probably already
	     * reflect that.  Don't change the symbol in this case.
	     *
	     * This isn't quite correct.  If the boulder was on top of some
	     * other objects they should be seen once the boulder is removed.
	     * However, we have no way of knowing that what is there now
	     * was there then.  So we let the hero have a lapse of memory.
	     * We could also just display what is currently on the top of the
	     * object stack (if anything).
	     */
	    if (remembered_object(x, y) == BOULDER) {
		if (lev->typ != ROOM && lev->seenv) {
		    map_background(x, y, 1);
		} else {

#ifdef DISPLAY_LAYERS
		    /*lev->mem_bg = lev->waslit ? S_room : S_stone;*/
			lev->mem_bg = (!lev->waslit) ? S_darkroom : S_room;
#else
		    lev->glyph = (!lev->waslit) ? cmap_to_glyph(S_darkroom) : cmap_to_glyph(S_room);
		    /*lev->glyph = lev->waslit ? cmap_to_glyph(S_room) :
					       cmap_to_glyph(S_stone);*/
#endif
		    show_glyph(x, y, memory_glyph(x, y));
		}
#ifdef DISPLAY_LAYERS
	    } else if ((lev->mem_bg >= S_stone && lev->mem_bg < S_darkroom) ||
		       memory_is_invisible(x, y)) {
		lev->mem_bg = (!lev->waslit ? S_darkroom : S_room); 
#else
	    } else if ((lev->glyph >= cmap_to_glyph(S_stone) &&
			lev->glyph < cmap_to_glyph(S_darkroom)) ||
		       glyph_is_invisible(levl[x][y].glyph)) {
		lev->glyph = (!cansee(x,y) && !lev->waslit) ? cmap_to_glyph(S_darkroom) :
					   cmap_to_glyph(S_room);
		/*lev->glyph = lev->waslit ? cmap_to_glyph(S_room) :
					   cmap_to_glyph(S_stone);*/
#endif
		show_glyph(x, y, memory_glyph(x, y));
	    }
	} else {
	    /* We feel it (I think hallways are the only things left). */
	    map_background(x, y, 1);
	    /* Corridors are never felt as lit (unless remembered that way) */
	    /* (lit_corridor only).					    */
#ifdef DISPLAY_LAYERS
	    if (lev->typ == CORR && lev->mem_bg == S_litcorr && !lev->waslit)
		show_glyph(x, y, cmap_to_glyph(lev->mem_bg = S_corr));
#else
	    if (lev->typ == CORR &&
		    lev->glyph == cmap_to_glyph(S_litcorr) && !lev->waslit)
		show_glyph(x, y, lev->glyph = cmap_to_glyph(S_corr));
#endif
	}
    } else {
	_map_location(x, y, 1);

	if (Punished) {
	    /*
	     * A ball or chain is only felt if it is first on the object
	     * location list.  Otherwise, we need to clear the felt bit ---
	     * something has been dropped on the ball/chain.  If the bit is
	     * not cleared, then when the ball/chain is moved it will drop
	     * the wrong glyph.
	     */
	    if (uchain->ox == x && uchain->oy == y) {
		if (level.objects[x][y] == uchain)
		    u.bc_felt |= BC_CHAIN;
		else
		    u.bc_felt &= ~BC_CHAIN;	/* do not feel the chain */
	    }
	    if (!carried(uball) && uball->ox == x && uball->oy == y) {
		if (level.objects[x][y] == uball)
		    u.bc_felt |= BC_BALL;
		else
		    u.bc_felt &= ~BC_BALL;	/* do not feel the ball */
	    }
	}

	/* Floor spaces are dark if unlit.  Corridors are dark if unlit. */
#ifdef DISPLAY_LAYERS
	if (lev->typ == ROOM && lev->mem_bg == S_room && !lev->waslit) {
	    lev->mem_bg = S_darkroom;
	    show_glyph(x,y, memory_glyph(x, y));
	} else if (lev->typ == CORR &&
		    lev->mem_bg == S_litcorr && !lev->waslit) {
	    lev->mem_bg = S_corr;
	    show_glyph(x,y, memory_glyph(x, y));
	}
#else
	if (lev->typ == ROOM &&
		    lev->glyph == cmap_to_glyph(S_room) && !lev->waslit)
	    show_glyph(x,y, lev->glyph = cmap_to_glyph(S_darkroom));
	else if (lev->typ == CORR &&
		    lev->glyph == cmap_to_glyph(S_litcorr) && !lev->waslit)
	    show_glyph(x,y, lev->glyph = cmap_to_glyph(S_corr));
#endif
    }
    /* draw monster on top if we can sense it */
    if ((x != u.ux || y != u.uy) && (mon = m_at(x,y)) && sensemon(mon))
	display_monster(x, y, mon,
		(tp_sensemon(mon) || MATCH_WARN_OF_MON(mon)) ? PHYSICALLY_SEEN : DETECTED,
		is_worm_tail(mon));
}

/*
 * newsym()
 *
 * Possibly put a new glyph at the given location.
 */
void
newsym(x,y)
    register int x,y;
{
    register struct monst *mon;
    register struct rm *lev = &(levl[x][y]);
    register int see_it;
    register xchar worm_tail;

    if (in_mklev) return;
	if ( (Superscroller || u.uprops[SUPERSCROLLER_ACTIVE].extrinsic || have_superscrollerstone() ) && rn2(10) ) { show_glyph(x, y, cmap_to_glyph(S_stone)); return;}

    /* only permit updating the hero when swallowed */
    if (u.uswallow) {
	if (x == u.ux && y == u.uy) display_self();
	return;
    }
    if (Underwater && !Is_waterlevel(&u.uz)) {
	/* don't do anything unless (x,y) is an adjacent underwater position */
	int dx, dy;
	if (!is_pool(x,y)) return;
	dx = x - u.ux;	if (dx < 0) dx = -dx;
	dy = y - u.uy;	if (dy < 0) dy = -dy;
	if (dx > 1 || dy > 1) return;
    }

    /* Can physically see the location. */
    if (cansee(x,y)) {
        NhRegion* reg = visible_region_at(x,y);
	/*
	 * Don't use templit here:  E.g.
	 *
	 *	lev->waslit = !!(lev->lit || templit(x,y));
	 *
	 * Otherwise we have the "light pool" problem, where non-permanently
	 * lit areas just out of sight stay remembered as lit.  They should
	 * re-darken.
	 *
	 * Perhaps ALL areas should revert to their "unlit" look when
	 * out of sight.
	 */
	lev->waslit = (lev->lit!=0);	/* remember lit condition */

	if (reg != NULL && ACCESSIBLE(lev->typ)) {
	    show_region(reg,x,y);
	    return;
	}
	if (x == u.ux && y == u.uy) {
	    if (senseself()) {
		_map_location(x,y,0);	/* map *under* self */
		display_self();
	    } else
		/* we can see what is there */
		_map_location(x,y,1);
	}
	else {
	    mon = m_at(x,y);
	    worm_tail = is_worm_tail(mon);
	    see_it = mon && (worm_tail
		? ((!mon->minvis || See_invisible) && !mon->minvisreal)
		: (mon_visible(mon)) || tp_sensemon(mon) || MATCH_WARN_OF_MON(mon) || (Role_if(PM_ACTIVISTOR) && mon->data == &mons[PM_TOPMODEL]) || (Race_if(PM_PEACEMAKER) && mon->data == &mons[PM_TOPMODEL]) || (Role_if(PM_ACTIVISTOR) && type_is_pname(mon->data) && uwep && is_quest_artifact(uwep) ) || (uamul && uamul->otyp == AMULET_OF_UNDEAD_WARNING && (is_undead(mon->data) || mon->egotype_undead) ) || (uamul && uamul->otyp == AMULET_OF_POISON_WARNING && poisonous(mon->data) ) || (uamul && uamul->otyp == AMULET_OF_OWN_RACE_WARNING && your_race(mon->data) ) || (Role_if(PM_PALADIN) && is_demon(mon->data) ) || (Race_if(PM_VORTEX) && unsolid(mon->data) ) || (Race_if(PM_VORTEX) && nolimbs(mon->data) ) || (Race_if(PM_CORTEX) && unsolid(mon->data) ) || (Race_if(PM_CORTEX) && nolimbs(mon->data) ) || (uamul && uamul->otyp == AMULET_OF_COVETOUS_WARNING && (is_covetous(mon->data) || mon->egotype_covetous) ) || (Stunnopathy && Stunned && always_hostile(mon->data) && (mon)->mhp % 4 != 0) || (Numbopathy && Numbed && (avoid_player(mon->data) || mon->egotype_avoider) ) || (Sickopathy && Sick && extra_nasty(mon->data) ) || (Freezopathy && Frozen && mon->data->mcolor == CLR_WHITE ) || (Burnopathy && Burned && infravision(mon->data) ) || (Race_if(PM_RODNEYAN) && mon_has_amulet(mon)) || (Race_if(PM_RODNEYAN) && mon_has_special(mon)) || (Race_if(PM_LEVITATOR) && (is_flyer(mon->data) || mon->egotype_flying) ) || (isselfhybrid && strongmonst(mon->data) && is_wanderer(mon->data) ) || (isselfhybrid && monpolyok(mon->data) && !polyok(mon->data) && ((mon->data->mlevel < 30) || ((mon)->mhp % 2 != 0) ) )  );
	    if (mon && (see_it || (!worm_tail && Detect_monsters))) {
		if (mon->mtrapped) {
		    struct trap *trap = t_at(x, y);
		    int tt = trap ? trap->ttyp : NO_TRAP;

		    /* if monster is in a physical trap, you see the trap too */
		    if (trap && (tt == BEAR_TRAP || tt == PIT ||
			tt == SPIKED_PIT || tt == GIANT_CHASM || tt == SHIT_PIT || tt == WEB) && (trap && !trap->hiddentrap)) {
			trap->tseen = TRUE;
		    }
		}
		_map_location(x,y,0);	/* map under the monster */
		/* also gets rid of any invisibility glyph */
		display_monster(x, y, mon, see_it? PHYSICALLY_SEEN : DETECTED, worm_tail);
	    }
	    else if (mon && mon_warning(mon) && !is_worm_tail(mon))
	        display_warning(mon);
	    else if (memory_is_invisible(x,y))
		map_invisible(x, y);
	    else
		_map_location(x,y,1);	/* map the location */
	}
    }

    /* Can't see the location. */
    else {
	if (x == u.ux && y == u.uy) {
	    feel_location(u.ux, u.uy);		/* forces an update */

	    if (senseself()) display_self();
	}
	else if ((mon = m_at(x,y))
		&& ((see_it = (tp_sensemon(mon) || MATCH_WARN_OF_MON(mon) || (Role_if(PM_ACTIVISTOR) && mon->data == &mons[PM_TOPMODEL]) || (Race_if(PM_PEACEMAKER) && mon->data == &mons[PM_TOPMODEL]) || (Role_if(PM_ACTIVISTOR) && type_is_pname(mon->data) && uwep && is_quest_artifact(uwep) ) 	|| (uamul && uamul->otyp == AMULET_OF_UNDEAD_WARNING && (is_undead(mon->data) || mon->egotype_undead) ) || (uamul && uamul->otyp == AMULET_OF_POISON_WARNING && poisonous(mon->data) ) || (uamul && uamul->otyp == AMULET_OF_OWN_RACE_WARNING && your_race(mon->data) ) || (Role_if(PM_PALADIN) && is_demon(mon->data) ) || (Race_if(PM_VORTEX) && unsolid(mon->data) ) || (Race_if(PM_VORTEX) && nolimbs(mon->data) ) || (Race_if(PM_CORTEX) && unsolid(mon->data) ) || (Race_if(PM_CORTEX) && nolimbs(mon->data) ) || (uamul && uamul->otyp == AMULET_OF_COVETOUS_WARNING && (is_covetous(mon->data) || mon->egotype_covetous) ) || (Stunnopathy && Stunned && always_hostile(mon->data) && (mon)->mhp % 4 != 0) || (Numbopathy && Numbed && (avoid_player(mon->data) || mon->egotype_avoider) ) || (Sickopathy && Sick && extra_nasty(mon->data) ) || (Freezopathy && Frozen && mon->data->mcolor == CLR_WHITE ) || (Burnopathy && Burned && infravision(mon->data) ) || (Race_if(PM_RODNEYAN) && mon_has_amulet(mon)) || (Race_if(PM_RODNEYAN) && mon_has_special(mon)) || (Race_if(PM_LEVITATOR) && (is_flyer(mon->data) || mon->egotype_flying) ) || (isselfhybrid && strongmonst(mon->data) && is_wanderer(mon->data) ) || (isselfhybrid && monpolyok(mon->data) && !polyok(mon->data) && ((mon->data->mlevel < 30) || ((mon)->mhp % 2 != 0) ) ) 	    		|| (see_with_infrared(mon) && mon_visible(mon))))
		    || Detect_monsters)
		&& !is_worm_tail(mon)) {
	    /* Monsters are printed every time. */
	    /* This also gets rid of any invisibility glyph */
	    display_monster(x, y, mon, see_it ? 0 : DETECTED, 0);
	}
	else if ((mon = m_at(x,y)) && mon_warning(mon) &&
		 !is_worm_tail(mon)) {
	        display_warning(mon);
	}		

	/*
	 * If the location is remembered as being both dark (waslit is false)
	 * and lit (glyph is a lit room or lit corridor) then it was either:
	 *
	 *	(1) A dark location that the hero could see through night
	 *	    vision.
	 *
	 *	(2) Darkened while out of the hero's sight.  This can happen
	 *	    when cursed scroll of light is read.
	 *
	 * In either case, we have to manually correct the hero's memory to
	 * match waslit.  Deciding when to change waslit is non-trivial.
	 *
	 *  Note:  If flags.lit_corridor is set, then corridors act like room
	 *	   squares.  That is, they light up if in night vision range.
	 *	   If flags.lit_corridor is not set, then corridors will
	 *	   remain dark unless lit by a light spell and may darken
	 *	   again, as discussed above.
	 *
	 * These checks and changes must be here and not in back_to_glyph().
	 * They are dependent on the position being out of sight.
	 */
	else if (!lev->waslit) {
#ifdef DISPLAY_LAYERS
	    if (lev->mem_bg == S_litcorr && lev->typ == CORR) {
		lev->mem_bg = S_corr;
		show_glyph(x, y, memory_glyph(x, y));
	    } else if (lev->mem_bg == S_room && lev->typ == ROOM) {
		lev->mem_bg = S_darkroom;
		show_glyph(x, y, memory_glyph(x, y));
	    }
#else	/* DISPLAY_LAYERS */
	    if (lev->glyph == cmap_to_glyph(S_litcorr) && lev->typ == CORR)
		show_glyph(x, y, lev->glyph = cmap_to_glyph(S_corr));
	    else if (lev->glyph == cmap_to_glyph(S_room) && lev->typ == ROOM)
		show_glyph(x, y, lev->glyph = cmap_to_glyph(S_darkroom));
#endif	/* DISPLAY_LAYERS */
	    else
		goto show_mem;
	} else {
show_mem:
	    show_glyph(x, y, memory_glyph(x, y));
	}
    }
}

#undef is_worm_tail

/*
 * shieldeff()
 *
 * Put magic shield pyrotechnics at the given location.  This *could* be
 * pulled into a platform dependent routine for fancier graphics if desired.
 */
void
shieldeff(x,y)
    xchar x,y;
{
    register int i;

    if (!flags.sparkle) return;
    if (cansee(x,y)) {	/* Don't see anything if can't see the location */
#ifdef ALLEG_FX
    	if (iflags.usealleg) {
	    newsym(x,y);
	    if(alleg_shield(x,y))
                return;
    	}
#endif
	for (i = 0; i < SHIELD_COUNT; i++) {
	    show_glyph(x, y, cmap_to_glyph(shield_static[i]));
	    flush_screen(1);	/* make sure the glyph shows up */
	    delay_output();
	}
	newsym(x,y);		/* restore the old information */
    }
}


/*
 * tmp_at()
 *
 * Temporarily place glyphs on the screen.  Do not call delay_output().  It
 * is up to the caller to decide if it wants to wait [presently, everyone
 * but explode() wants to delay].
 *
 * Call:
 *	(DISP_BEAM,   glyph)	open, initialize glyph
 *      (DISP_BEAM_ALWAYS, glyph)    open, initialize glyph
 *	(DISP_FLASH,  glyph)	open, initialize glyph
 *	(DISP_ALWAYS, glyph)	open, initialize glyph
 *	(DISP_CHANGE, glyph)	change glyph
 *	(DISP_END,    0)	close & clean up (second argument doesn't
 *				matter)
 *	(DISP_FREEMEM, 0)	only used to prevent memory leak during
 *				exit)
 *	(x, y)			display the glyph at the location
 *
 * DISP_BEAM  - Display the given glyph at each location, but do not erase
 *		any until the close call.
 * WAC added beam_always for lightning strikes
 * DISP_BEAM_ALWAYS- Like DISP_BEAM, but vision is not taken into account.
 * DISP_FLASH - Display the given glyph at each location, but erase the
 *		previous location's glyph.
 * DISP_ALWAYS- Like DISP_FLASH, but vision is not taken into account.
 */

static struct tmp_glyph {
    coord saved[COLNO];	/* previously updated positions */
    int sidx;		/* index of next unused slot in saved[] */
    int style;		/* either DISP_BEAM or DISP_FLASH or DISP_ALWAYS */
    int glyph;		/* glyph to use when printing */
    struct tmp_glyph *cont;	/* Used if saved[] is full */
    struct tmp_glyph *prev;
} tgfirst;
static struct tmp_glyph *tglyph = (struct tmp_glyph *)0;

void
tmp_at(x, y)
    int x, y;
{
    struct tmp_glyph *tmp, *cont;

    switch (x) {
	case DISP_BEAM:
        case DISP_BEAM_ALWAYS:
	case DISP_FLASH:
	case DISP_ALWAYS:
	    if (!tglyph)
		tmp = &tgfirst;
	    else	/* nested effect; we need dynamic memory */
		tmp = (struct tmp_glyph *)alloc(sizeof (struct tmp_glyph));
	    tmp->prev = tglyph;
	    tglyph = tmp;
	    tglyph->sidx = 0;
	    tglyph->style = x;
	    tglyph->glyph = y;
	    tglyph->cont = (struct tmp_glyph *)0;
	    flush_screen(0);	/* flush buffered glyphs */
	    return;
	case DISP_FREEMEM:  /* in case game ends with tmp_at() in progress */
	    while (tglyph) {
		cont = tglyph->cont;
		while (cont) {
		    tmp = cont->cont;
		    if (cont != &tgfirst) free((genericptr_t)cont);
		    cont = tmp;
		}
		tmp = tglyph->prev;
		if (tglyph != &tgfirst) free((genericptr_t)tglyph);
		tglyph = tmp;
	    }
	    return;

	default:
	    break;
    }

    if (!tglyph) panic("tmp_at: tglyph not initialized");

    switch (x) {
	case DISP_CHANGE:
	    tglyph->glyph = y;
	    break;

	case DISP_END:
	    if (tglyph->style == DISP_BEAM || tglyph->style == DISP_BEAM_ALWAYS) {
		register int i;

		/* Erase (reset) from source to end */
		for (i = 0; i < tglyph->sidx; i++)
		    newsym(tglyph->saved[i].x, tglyph->saved[i].y);
		cont = tglyph->cont;
		while (cont) {
		    for (i = 0; i < cont->sidx; i++)
			newsym(cont->saved[i].x, cont->saved[i].y);
		    tmp = cont->cont;
		    if (cont != &tgfirst) free((genericptr_t)cont);
		    cont = tmp;
		}
	     /* tglyph->cont = (struct tmp_glyph *)0; */
	    } else {		/* DISP_FLASH or DISP_ALWAYS */
		if (tglyph->sidx)	/* been called at least once */
		    newsym(tglyph->saved[0].x, tglyph->saved[0].y);
	    }
	 /* tglyph->sidx = 0; -- about to be freed, so not necessary */
	    tmp = tglyph->prev;
	    if (tglyph != &tgfirst) free((genericptr_t)tglyph);
	    tglyph = tmp;
	    break;

	default:	/* do it */
	    if (tglyph->style == DISP_BEAM || tglyph->style == DISP_BEAM_ALWAYS) {
		if (!cansee(x,y) && tglyph->style == DISP_BEAM) break;
		/* save pos for later erasing */
		if (tglyph->sidx >= SIZE(tglyph->saved)) {
		    tmp = (struct tmp_glyph *)alloc(sizeof (struct tmp_glyph));
		    *tmp = *tglyph;
		    tglyph->prev = (struct tmp_glyph *)0;
		    tmp->cont = tglyph;
		    tglyph = tmp;
		    tglyph->sidx = 0;
		}
		tglyph->saved[tglyph->sidx].x = x;
		tglyph->saved[tglyph->sidx].y = y;
		tglyph->sidx += 1;
	    } else {	/* DISP_FLASH/ALWAYS */
		if (tglyph->sidx) { /* not first call, so reset previous pos */
		    newsym(tglyph->saved[0].x, tglyph->saved[0].y);
		    tglyph->sidx = 0;	/* display is presently up to date */
		}
		if (!cansee(x,y) && tglyph->style != DISP_ALWAYS) break;
		tglyph->saved[0].x = x;
		tglyph->saved[0].y = y;
		tglyph->sidx = 1;
	    }

	    show_glyph(x, y, tglyph->glyph);	/* show it */
	    flush_screen(0);			/* make sure it shows up */
	    break;
    } /* end case */
}

#ifdef DISPLAY_LAYERS
int
glyph_is_floating(glyph)
int glyph;
{
    return glyph_is_monster(glyph) || glyph_is_explosion(glyph) ||
	    glyph_is_zap_beam(glyph) || glyph_is_swallow(glyph) ||
	    glyph_is_warning(glyph) || tglyph && glyph == tglyph->glyph;
}
#endif

/*
 * swallowed()
 *
 * The hero is swallowed.  Show a special graphics sequence for this.  This
 * bypasses all of the display routines and messes with buffered screen
 * directly.  This method works because both vision and display check for
 * being swallowed.
 */
void
swallowed(first)
    int first;
{
    static xchar lastx, lasty;	/* last swallowed position */
    int swallower, left_ok, rght_ok;

    if (first)
	cls();
    else {
	register int x, y;

	/* Clear old location */
	for (y = lasty-1; y <= lasty+1; y++)
	    for (x = lastx-1; x <= lastx+1; x++)
		if (isok(x,y)) show_glyph(x,y,cmap_to_glyph(S_stone));
    }

#ifdef ALLEG_FX
    if(!iflags.usealleg || !alleg_swallowed(u.ux,u.uy)) {
#endif
    swallower = monsndx(u.ustuck->data);
    /* assume isok(u.ux,u.uy) */
    left_ok = isok(u.ux-1,u.uy);
    rght_ok = isok(u.ux+1,u.uy);
    /*
     *  Display the hero surrounded by the monster's stomach.
     */
    if(isok(u.ux, u.uy-1)) {
	if (left_ok)
	show_glyph(u.ux-1, u.uy-1, swallow_to_glyph(swallower, S_sw_tl));
	show_glyph(u.ux  , u.uy-1, swallow_to_glyph(swallower, S_sw_tc));
	if (rght_ok)
	show_glyph(u.ux+1, u.uy-1, swallow_to_glyph(swallower, S_sw_tr));
    }

    if (left_ok)
    show_glyph(u.ux-1, u.uy  , swallow_to_glyph(swallower, S_sw_ml));
    display_self();
    if (rght_ok)
    show_glyph(u.ux+1, u.uy  , swallow_to_glyph(swallower, S_sw_mr));

    if(isok(u.ux, u.uy+1)) {
	if (left_ok)
	show_glyph(u.ux-1, u.uy+1, swallow_to_glyph(swallower, S_sw_bl));
	show_glyph(u.ux  , u.uy+1, swallow_to_glyph(swallower, S_sw_bc));
	if (rght_ok)
	show_glyph(u.ux+1, u.uy+1, swallow_to_glyph(swallower, S_sw_br));
    }
#ifdef ALLEG_FX
    }
#endif

    /* Update the swallowed position. */
    lastx = u.ux;
    lasty = u.uy;
}

/*
 * under_water()
 *
 * Similar to swallowed() in operation.  Shows hero when underwater
 * except when in water level.  Special routines exist for that.
 */
void
under_water(mode)
    int mode;
{
    static xchar lastx, lasty;
    static boolean dela;
    register int x, y;

    /* swallowing has a higher precedence than under water */
    if (Is_waterlevel(&u.uz) || u.uswallow) return;

    /* full update */
    if (mode == 1 || dela) {
	cls();
	dela = FALSE;
    }
    /* delayed full update */
    else if (mode == 2) {
	dela = TRUE;
	return;
    }
    /* limited update */
    else {
	for (y = lasty-1; y <= lasty+1; y++)
	    for (x = lastx-1; x <= lastx+1; x++)
		if (isok(x,y))
		    show_glyph(x,y,cmap_to_glyph(S_stone));
    }
    for (x = u.ux-1; x <= u.ux+1; x++)
	for (y = u.uy-1; y <= u.uy+1; y++)
	    if (isok(x,y) && is_pool(x,y)) {
		if (Blind && !(x == u.ux && y == u.uy))
		    show_glyph(x,y,cmap_to_glyph(S_stone));
		else	
		    newsym(x,y);
	    }
    lastx = u.ux;
    lasty = u.uy;
}

/*
 *	under_ground()
 *
 *	Very restricted display.  You can only see yourself.
 */
void
under_ground(mode)
    int mode;
{
    static boolean dela;

    /* swallowing has a higher precedence than under ground */
    if (u.uswallow) return;

    /* full update */
    if (mode == 1 || dela) {
	cls();
	dela = FALSE;
    }
    /* delayed full update */
    else if (mode == 2) {
	dela = TRUE;
	return;
    }
    /* limited update */
    else
	newsym(u.ux,u.uy);
}


/* ========================================================================= */

/*
 * Loop through all of the monsters and update them.  Called when:
 *	+ going blind & telepathic
 *	+ regaining sight & telepathic
 *      + getting and losing infravision 
 *	+ hallucinating
 *	+ doing a full screen redraw
 *	+ see invisible times out or a ring of see invisible is taken off
 *	+ when a potion of see invisible is quaffed or a ring of see
 *	  invisible is put on
 *	+ gaining telepathy when blind [givit() in eat.c, pleased() in pray.c]
 *	+ losing telepathy while blind [xkilled() in mon.c, attrcurse() in
 *	  sit.c]
 */
void
see_monsters()
{
    register struct monst *mon;

    if (defer_see_monsters) return;

    for (mon = fmon; mon; mon = mon->nmon) {
	if (DEADMONSTER(mon)) continue;
	newsym(mon->mx,mon->my);
	if (mon->wormno) see_wsegs(mon);
    }
#ifdef STEED
    /* when mounted, hero's location gets caught by monster loop */
    if (!u.usteed)
#endif
    newsym(u.ux, u.uy);
}

/*
 * Block/unblock light depending on what a mimic is mimicing and if it's
 * invisible or not.  Should be called only when the state of See_invisible
 * changes.
 */
void
set_mimic_blocking()
{
    register struct monst *mon;

    for (mon = fmon; mon; mon = mon->nmon) {
	if (DEADMONSTER(mon)) continue;
	if (mon->minvis &&
	   ((mon->m_ap_type == M_AP_FURNITURE &&
	     (mon->mappearance == S_vcdoor || mon->mappearance == S_hcdoor)) ||
	    (mon->m_ap_type == M_AP_OBJECT && mon->mappearance == BOULDER))) {
	    if(See_invisible)
		block_point(mon->mx, mon->my);
	    else
		unblock_point(mon->mx, mon->my);
	}
    }
}

/*
 * Loop through all of the object *locations* and update them.  Called when
 *	+ hallucinating.
 */
void
see_objects()
{
    register struct obj *obj;
    for(obj = fobj; obj; obj = obj->nobj)
	if (vobj_at(obj->ox,obj->oy) == obj) newsym(obj->ox, obj->oy);
}

/*
 * Update hallucinated traps.
 */
void
see_traps()
{
    struct trap *trap;
    int glyph;

    for (trap = ftrap; trap; trap = trap->ntrap) {
	glyph = glyph_at(trap->tx, trap->ty);
	if (glyph_is_trap(glyph))
	    newsym(trap->tx, trap->ty);
    }
}

/*
 * Put the cursor on the hero.  Flush all accumulated glyphs before doing it.
 */
void
curs_on_u()
{
    flush_screen(1);	/* Flush waiting glyphs & put cursor on hero */
}

int
doredraw()
{
    docrt();
    return 0;
}

void
docrt()
{
    register int x,y;
    register struct rm *lev;
    int i, glyph;

    if (!u.ux) return; /* display isn't ready yet */

    transp = FALSE;
    if (tileset[0])
	for(i = 0; i < no_tilesets; ++i)
	    if (!strcmpi(tileset, tilesets[i].name)) {
		transp = !!(tilesets[i].flags & TILESET_TRANSPARENT);
		break;
	    }

    if (u.uswallow) {
	swallowed(1);
	return;
    }
    if (Underwater && !Is_waterlevel(&u.uz)) {
	under_water(1);
	return;
    }
    if (u.uburied) {
	under_ground(1);
	return;
    }

    /* shut down vision */
    vision_recalc(2);

    /*
     * This routine assumes that cls() does the following:
     *      + fills the physical screen with the symbol for rock
     *      + clears the glyph buffer
     */
    cls();

    /* display memory */
    for (x = 1; x < COLNO; x++) {
	lev = &levl[x][0];
	for (y = 0; y < ROWNO; y++, lev++)
	    if ((glyph = memory_glyph(x,y)) != cmap_to_glyph(S_stone))
		show_glyph(x,y,glyph);
    }

    /* see what is to be seen */
    vision_recalc(0);

    /* overlay with monsters */
    see_monsters();

    flags.botlx = 1;	/* force a redraw of the bottom line */

}


/* ========================================================================= */
/* Glyph Buffering (3rd screen) ============================================ */

typedef struct {
    xchar new;		/* perhaps move this bit into the rm strucure. */
    int   glyph;
} gbuf_entry;

static gbuf_entry gbuf[ROWNO][COLNO];
static char gbuf_start[ROWNO];
static char gbuf_stop[ROWNO];

#ifdef DUMP_LOG
/* D: Added to dump screen to output file */
STATIC_PTR uchar get_glyph_char(glyph)
int glyph;
{
    uchar   ch;
    register int offset;

    if (glyph >= NO_GLYPH)
        return 0;

    /*
     *  Map the glyph back to a character.
     *
     *  Warning:  For speed, this makes an assumption on the order of
     *		  offsets.  The order is set in display.h.
     */
    if ((offset = (glyph - GLYPH_WARNING_OFF)) >= 0) {	/* a warning flash */
	ch = def_warnsyms[offset].sym;
    } else if ((offset = (glyph - GLYPH_SWALLOW_OFF)) >= 0) {	/* swallow */
	/* see swallow_to_glyph() in display.c */
	ch = (uchar) defsyms[S_sw_tl + (offset & 0x7)].sym;
    } else if ((offset = (glyph - GLYPH_ZAP_OFF)) >= 0) {	/* zap beam */
	/* see zapdir_to_glyph() in display.c */
	ch = defsyms[S_vbeam + (offset & 0x3)].sym;
    } else if ((offset = (glyph - GLYPH_CMAP_OFF)) >= 0) {	/* cmap */
	ch = defsyms[offset].sym;
    } else if ((offset = (glyph - GLYPH_OBJ_OFF)) >= 0) {	/* object */
	ch = def_oc_syms[(int)objects[offset].oc_class];
    } else if ((offset = (glyph - GLYPH_RIDDEN_OFF)) >= 0) { /* mon ridden */
	ch = def_monsyms[(int)mons[offset].mlet];
    } else if ((offset = (glyph - GLYPH_BODY_OFF)) >= 0) {	/* a corpse */
	ch = def_oc_syms[(int)objects[CORPSE].oc_class];
    } else if ((offset = (glyph - GLYPH_DETECT_OFF)) >= 0) { /* mon detect */
	ch = def_monsyms[(int)mons[offset].mlet];
    } else if ((offset = (glyph - GLYPH_INVIS_OFF)) >= 0) {  /* invisible */
	ch = DEF_INVISIBLE;
    } else if ((offset = (glyph - GLYPH_PET_OFF)) >= 0) {	/* a pet */
	ch = def_monsyms[(int)mons[offset].mlet];
    } else {						    /* a monster */
	ch = monsyms[(int)mons[glyph].mlet];
    }
    return ch;
}

#ifdef TTY_GRAPHICS
extern const char * FDECL(compress_str, (const char *));
#else
const char*
compress_str(str) /* copied from win/tty/wintty.c */
const char *str;
{
	static char cbuf[BUFSZ];
	/* compress in case line too long */
	if((int)strlen(str) >= 80) {
		register const char *bp0 = str;
		register char *bp1 = cbuf;

		do {
			if(*bp0 != ' ' || bp0[1] != ' ')
				*bp1++ = *bp0;
		} while(*bp0++);
	} else
	    return str;
	return cbuf;
}
#endif /* TTY_GRAPHICS */

/* Take a screen dump */
void dump_screen()
{
    register int x,y;
    int lastc;
    /* D: botl.c has a closer approximation to the size, but we'll go with
     *    this */
    char buf[300], *ptr;
    
    for (y = 0; y < ROWNO; y++) {
	lastc = 0;
	ptr = buf;
	for (x = 1; x < COLNO; x++) {
	    uchar c = get_glyph_char(gbuf[y][x].glyph);
	    *ptr++ = c;
	    if (c != ' ')
		lastc = x;
	}
	buf[lastc] = '\0';
	dump("", buf);
    }
    dump("", "");
    bot1str(buf);
    ptr = (char *) compress_str((const char *) buf);
    dump("", ptr);
    bot2str(buf);
    dump("", buf);
    dump("", "");
    dump("", "");
}
#endif /* DUMP_LOG */

/*
 * Store the glyph in the 3rd screen for later flushing.
 */
void
show_glyph(x,y,glyph)
    int x, y, glyph;
{
    /*
     * Check for bad positions and glyphs.
     */
    if (!isok(x, y)) {
	const char *text;
	int  offset;

	/* column 0 is invalid, but it's often used as a flag, so ignore it */
	if (x == 0) return;

	/*
	 *  This assumes an ordering of the offsets.  See display.h for
	 *  the definition.
	 */

	if (glyph >= GLYPH_WARNING_OFF) {	/* a warning */
	    text = "warning";		offset = glyph - GLYPH_WARNING_OFF;
	} else if (glyph >= GLYPH_SWALLOW_OFF) {	/* swallow border */
	    text = "swallow border";	offset = glyph - GLYPH_SWALLOW_OFF;
	} else if (glyph >= GLYPH_ZAP_OFF) {		/* zap beam */
	    text = "zap beam";		offset = glyph - GLYPH_ZAP_OFF;
	} else if (glyph >= GLYPH_EXPLODE_OFF) {	/* explosion */
	    text = "explosion";		offset = glyph - GLYPH_EXPLODE_OFF;
	} else if (glyph >= GLYPH_CMAP_OFF) {		/* cmap */
	    text = "cmap_index";	offset = glyph - GLYPH_CMAP_OFF;
	} else if (glyph >= GLYPH_OBJ_OFF) {		/* object */
	    text = "object";		offset = glyph - GLYPH_OBJ_OFF;
	} else if (glyph >= GLYPH_RIDDEN_OFF) {		/* ridden mon */
	    text = "ridden mon";	offset = glyph - GLYPH_RIDDEN_OFF;
	} else if (glyph >= GLYPH_BODY_OFF) {		/* a corpse */
	    text = "corpse";		offset = glyph - GLYPH_BODY_OFF;
	} else if (glyph >= GLYPH_DETECT_OFF) {		/* detected mon */
	    text = "detected mon";	offset = glyph - GLYPH_DETECT_OFF;
	} else if (glyph >= GLYPH_INVIS_OFF) {		/* invisible mon */
	    text = "invisible mon";	offset = glyph - GLYPH_INVIS_OFF;
	} else if (glyph >= GLYPH_PET_OFF) {		/* a pet */
	    text = "pet";		offset = glyph - GLYPH_PET_OFF;
	} else {					/* a monster */
	    text = "monster";		offset = glyph;
	}

	impossible("show_glyph:  bad pos %d %d with glyph %d [%s %d].",
						x, y, glyph, text, offset);
	return;
    }

	if ( (RMBLoss || u.uprops[RMB_LOST].extrinsic || have_rmbstone()) && glyph >= GLYPH_OBJ_OFF && !(glyph >= GLYPH_CMAP_OFF && glyph < (GLYPH_CMAP_OFF + 12) ) && !(glyph >= (GLYPH_CMAP_OFF + 19) && glyph < (GLYPH_CMAP_OFF + 23) ) )
	return;

    if (glyph >= MAX_GLYPH) {
	impossible("show_glyph:  bad glyph %d [max %d] at (%d,%d).",
					glyph, MAX_GLYPH, x, y);
	return;
    }

    /* [ALI] In transparent mode it is not sufficient just to consider
     * the foreground glyph, we also need to consider the background.
     * Rather than extend the display module to do this, for the time
     * being we just turn off optimization and rely on the windowing port
     * to ignore redundant calls to print_glyph().
     */
    if (transp || gbuf[y][x].glyph != glyph) {
	gbuf[y][x].glyph = glyph;
	gbuf[y][x].new   = 1;
	if (gbuf_start[y] > x) gbuf_start[y] = x;
	if (gbuf_stop[y]  < x) gbuf_stop[y]  = x;
    }
}


/*
 * Reset the changed glyph borders so that none of the 3rd screen has
 * changed.
 */
#define reset_glyph_bbox()			\
    {						\
	int i;					\
						\
	for (i = 0; i < ROWNO; i++) {		\
	    gbuf_start[i] = COLNO-1;		\
	    gbuf_stop[i]  = 0;			\
	}					\
    }


static gbuf_entry nul_gbuf = { 0, cmap_to_glyph(S_stone) };
/*
 * Turn the 3rd screen into stone.
 */
void
clear_glyph_buffer()
{
    register int x, y;
    register gbuf_entry *gptr;

    for (y = 0; y < ROWNO; y++) {
	gptr = &gbuf[y][0];
	for (x = COLNO; x; x--) {
	    *gptr++ = nul_gbuf;
	}
    }
    reset_glyph_bbox();
}

/*
 * Assumes that the indicated positions are filled with S_stone glyphs.
 */
void
row_refresh(start,stop,y)
    int start,stop,y;
{
    register int x;

    for (x = start; x <= stop; x++)
	if (gbuf[y][x].glyph != cmap_to_glyph(S_stone))
	    print_glyph(WIN_MAP,x,y,gbuf[y][x].glyph);
}

void
cls()
{

	if (YellowSpells || u.uprops[YELLOW_SPELLS].extrinsic || have_yellowspellstone()) return;

    display_nhwindow(WIN_MESSAGE, FALSE); /* flush messages */
    flags.botlx = 1;		/* force update of botl window */
    clear_nhwindow(WIN_MAP);	/* clear physical screen */
    clear_glyph_buffer();	/* this is sort of an extra effort, but OK */
}

/*
 * Synch the third screen with the display.
 */
void
flush_screen(cursor_on_u)
    int cursor_on_u;
{
    /* Prevent infinite loops on errors:
     *	    flush_screen->print_glyph->impossible->pline->flush_screen
     */
    static   boolean flushing = 0;
    static   boolean delay_flushing = 0;
    register int x,y;

    if (cursor_on_u == -1) delay_flushing = !delay_flushing;
    if (delay_flushing) return;
    if (flushing) return;	/* if already flushing then return */
    flushing = 1;

    for (y = 0; y < ROWNO; y++) {
	register gbuf_entry *gptr = &gbuf[y][x = gbuf_start[y]];
	for (; x <= gbuf_stop[y]; gptr++, x++)
	    if (gptr->new) {
		print_glyph(WIN_MAP,x,y,gptr->glyph);
		gptr->new = 0;
	    }
    }

    if (cursor_on_u) curs(WIN_MAP, u.ux,u.uy); /* move cursor to the hero */
    display_nhwindow(WIN_MAP, FALSE);
    reset_glyph_bbox();
#ifdef ALLEG_FX
    if (iflags.usealleg) alleg_vid_refresh();
#endif
    flushing = 0;
    if(flags.botl || flags.botlx) bot();
}

/* ========================================================================= */

/*
 * back_to_cmap()
 *
 * Use the information in the rm structure at the given position to create
 * a glyph of a background.
 *
 * I had to add a field in the rm structure (horizontal) so that we knew
 * if open doors and secret doors were horizontal or vertical.  Previously,
 * the screen symbol had the horizontal/vertical information set at
 * level generation time.
 *
 * I used the 'ladder' field (really doormask) for deciding if stairwells
 * were up or down.  I didn't want to check the upstairs and dnstairs
 * variables.
 */
STATIC_OVL int
back_to_cmap(x,y)
    xchar x,y;
{
    int idx;
    struct rm *ptr = &(levl[x][y]);

    switch (ptr->typ) {
    /* KMH -- support arboreal levels */
	case SCORR:
	case STONE:
	    idx = level.flags.arboreal ? S_tree : /*S_stone*/S_dungwall;
	    break;
	case ROOM:

	    if (MapTrapEffect || u.uprops[MAPBUG].extrinsic || have_mapstone()) idx = S_stone;

	    else idx = (!cansee(x,y) && !ptr->waslit) ? S_darkroom : S_room;
	    break;
	case CORR:

	    if (MapTrapEffect || u.uprops[MAPBUG].extrinsic || have_mapstone()) idx = S_stone;

	    else idx = (ptr->waslit || flags.lit_corridor) ? S_litcorr : S_corr;
	    break;
	case HWALL:
	case VWALL:
	case TLCORNER:
	case TRCORNER:
	case BLCORNER:
	case BRCORNER:
	case CROSSWALL:
	case TUWALL:
	case TDWALL:
	case TLWALL:
	case TRWALL:
	case SDOOR:
	    idx = ptr->seenv ? wall_angle(ptr) : /*S_stone*/S_dungwall;
	    break;
	case IRONBARS:		idx = S_bars;     break;
	case DOOR:
	    if (ptr->doormask) {
		if (ptr->doormask & D_BROKEN)
		    idx = S_ndoor;
		else if (ptr->doormask & D_ISOPEN)
		    idx = (ptr->horizontal) ? S_hodoor : S_vodoor;
		else			/* else is closed */
		    idx = (ptr->horizontal) ? S_hcdoor : S_vcdoor;
	    } else
		idx = S_ndoor;
	    break;
	case TREE:		idx = S_tree;     break;
	case POOL:
	case MOAT:		idx = S_pool;	  break;
	case STAIRS:
	    idx = (ptr->ladder & LA_DOWN) ? S_dnstair : S_upstair;
	    break;
	case LADDER:
	    idx = (ptr->ladder & LA_DOWN) ? S_dnladder : S_upladder;
	    break;
	case FOUNTAIN:		idx = S_fountain; break;
	case SINK:		idx = S_sink;     break;
	case TOILET:            idx = S_toilet;   break;
	case GRAVE:		idx = S_grave;    break;
	case ALTAR:             idx = S_altar;    break;
	case THRONE:		idx = S_throne;   break;
	case LAVAPOOL:		idx = S_lava;	  break;
	case ICE:		idx = S_ice;      break;
	case AIR:		idx = S_air;	  break;
	case CLOUD:		idx = S_cloud;	  break;
	case WATER:		idx = S_water;	  break;
	case DBWALL:
	    idx = (ptr->horizontal) ? S_hcdbridge : S_vcdbridge;
	    break;
	case DRAWBRIDGE_UP:
	    switch(ptr->drawbridgemask & DB_UNDER) {
	    case DB_MOAT:  idx = S_pool; break;
	    case DB_LAVA:  idx = S_lava; break;
	    case DB_ICE:   idx = S_ice;  break;
	    case DB_FLOOR: idx = (!cansee(x,y) && !ptr->waslit) ? S_darkroom : S_room; break;
	    default:
		impossible("Strange db-under: %d",
			   ptr->drawbridgemask & DB_UNDER);
		idx = (!cansee(x,y) && !ptr->waslit) ? S_darkroom : S_room; /* something is better than nothing */
		break;
	    }
	    break;
	case DRAWBRIDGE_DOWN:
	    idx = (ptr->horizontal) ? S_hodbridge : S_vodbridge;
	    break;
	default:
	    impossible("back_to_glyph:  unknown level type [ = %d ]",ptr->typ);
	    idx = (!cansee(x,y) && !ptr->waslit) ? S_darkroom : S_room;
	    break;
    }
    return idx;
}

int
back_to_glyph(x,y)
    xchar x,y;
{
    return cmap_to_glyph(back_to_cmap(x,y));
}


/*
 * swallow_to_glyph()
 *
 * Convert a monster number and a swallow location into the correct glyph.
 * If you don't want a patchwork monster while hallucinating, decide on
 * a random monster in swallowed() and don't use what_mon() here.
 */
STATIC_OVL int
swallow_to_glyph(mnum, loc)
    int mnum;
    int loc;
{
    if (loc < S_sw_tl || S_sw_br < loc) {
	impossible("swallow_to_glyph: bad swallow location");
	loc = S_sw_br;
    }
    return ((int) (what_mon(mnum)<<3) | (loc - S_sw_tl)) + GLYPH_SWALLOW_OFF;
}



/*
 * zapdir_to_glyph()
 *
 * Change the given zap direction and beam type into a glyph.  Each beam
 * type has four glyphs, one for each of the symbols below.  The order of
 * the zap symbols [0-3] as defined in rm.h are:
 *
 *	|  S_vbeam	( 0, 1) or ( 0,-1)
 *	-  S_hbeam	( 1, 0) or (-1,	0)
 *	\  S_lslant	( 1, 1) or (-1,-1)
 *	/  S_rslant	(-1, 1) or ( 1,-1)
 */
int
zapdir_to_glyph(dx, dy, beam_type)
    register int dx, dy;
    int beam_type;
{
    if (beam_type >= NUM_ZAP) {
	impossible("zapdir_to_glyph:  illegal beam type");
	beam_type = 0;
    }
    dx = (dx == dy) ? 2 : (dx && dy) ? 3 : dx ? 1 : 0;

    return ((int) ((beam_type << 2) | dx)) + GLYPH_ZAP_OFF;
}


/*
 * Utility routine for dowhatis() used to find out the glyph displayed at
 * the location.  This isn't necessarily the same as the glyph in the levl
 * structure, so we must check the "third screen".
 */
int
glyph_at(x, y)
    xchar x,y;
{
    if(x < 0 || y < 0 || x >= COLNO || y >= ROWNO)
	return cmap_to_glyph(S_room);			/* XXX */
    return gbuf[y][x].glyph;
}


/* ------------------------------------------------------------------------- */
/* Wall Angle -------------------------------------------------------------- */

/*#define WA_VERBOSE*/	/* give (x,y) locations for all "bad" spots */

#ifdef WA_VERBOSE

static const char *FDECL(type_to_name, (int));
static void FDECL(error4, (int,int,int,int,int,int));

static int bad_count[MAX_TYPE]; /* count of positions flagged as bad */
static const char *type_names[MAX_TYPE] = {
	"STONE",	"VWALL",	"HWALL",	"TLCORNER",
	"TRCORNER",	"BLCORNER",	"BRCORNER",	"CROSSWALL",
	"TUWALL",	"TDWALL",	"TLWALL",	"TRWALL",
	"DBWALL",	"SDOOR",	"SCORR",	"POOL",
	"MOAT",		"WATER",	"DRAWBRIDGE_UP","LAVAPOOL",
	"DOOR",		"CORR",		"ROOM",		"STAIRS",
	"LADDER",	"FOUNTAIN",	"THRONE",	"SINK",
	"ALTAR",	"ICE",		"DRAWBRIDGE_DOWN","AIR",
	"CLOUD"
};


static const char *
type_to_name(type)
    int type;
{
    return (type < 0 || type > MAX_TYPE) ? "unknown" : type_names[type];
}

STATIC_OVL void
error4(x, y, a, b, c, dd)
    int x, y, a, b, c, dd;
{
    pline("set_wall_state: %s @ (%d,%d) %s%s%s%s",
	type_to_name(levl[x][y].typ), x, y,
	a ? "1":"", b ? "2":"", c ? "3":"", dd ? "4":"");
    bad_count[levl[x][y].typ]++;
}
#endif /* WA_VERBOSE */

/*
 * Return 'which' if position is implies an unfinshed exterior.  Return
 * zero otherwise.  Unfinished implies outer area is rock or a corridor.
 *
 * Things that are ambigious: lava
 */
STATIC_OVL int
check_pos(x, y, which)
    int x, y, which;
{
    int type;
    if (!isok(x,y)) return which;
    type = levl[x][y].typ;
    if (IS_ROCK(type) || type == CORR || type == SCORR) return which;
    return 0;
}

/* Return TRUE if more than one is non-zero. */
/*ARGSUSED*/
#ifdef WA_VERBOSE
STATIC_OVL boolean
more_than_one(x, y, a, b, c)
    int x, y, a, b, c;
{
#if defined(MAC_MPW)
# pragma unused ( x,y )
#endif
    if ((a && (b|c)) || (b && (a|c)) || (c && (a|b))) {
	error4(x,y,a,b,c,0);
	return TRUE;
    }
    return FALSE;
}
#else
#define more_than_one(x, y, a, b, c) (((a) && ((b)|(c))) || ((b) && ((a)|(c))) || ((c) && ((a)|(b))))
#endif

/* Return the wall mode for a T wall. */
STATIC_OVL int
set_twall(x0,y0, x1,y1, x2,y2, x3,y3)
int x0,y0, x1,y1, x2,y2, x3,y3;
{
    int wmode, is_1, is_2, is_3;

    is_1 = check_pos(x1, y1, WM_T_LONG);
    is_2 = check_pos(x2, y2, WM_T_BL);
    is_3 = check_pos(x3, y3, WM_T_BR);
    if (more_than_one(x0, y0, is_1, is_2, is_3)) {
	wmode = 0;
    } else {
	wmode = is_1 + is_2 + is_3;
    }
    return wmode;
}

/* Return wall mode for a horizontal or vertical wall. */
STATIC_OVL int
set_wall(x, y, horiz)
    int x, y, horiz;
{
    int wmode, is_1, is_2;

    if (horiz) {
	is_1 = check_pos(x,y-1, WM_W_TOP);
	is_2 = check_pos(x,y+1, WM_W_BOTTOM);
    } else {
	is_1 = check_pos(x-1,y, WM_W_LEFT);
	is_2 = check_pos(x+1,y, WM_W_RIGHT);
    }
    if (more_than_one(x, y, is_1, is_2, 0)) {
	wmode = 0;
    } else {
	wmode = is_1 + is_2;
    }
    return wmode;
}

/*
 * If an invisible monster has gone away, that will be discovered.  If an
 * invisible monster has appeared, this will _not_ be discovered since
 * searching only finds one monster per turn so we must check that separately.
 *
 * Return a wall mode for a corner wall. (x4,y4) is the "inner" position.
 */
STATIC_OVL int
set_corn(x1,y1, x2,y2, x3,y3, x4,y4)
	int x1, y1, x2, y2, x3, y3, x4, y4;
{
    int wmode, is_1, is_2, is_3, is_4;

    is_1 = check_pos(x1, y1, 1);
    is_2 = check_pos(x2, y2, 1);
    is_3 = check_pos(x3, y3, 1);
    is_4 = check_pos(x4, y4, 1);	/* inner location */

    /*
     * All 4 should not be true.  So if the inner location is rock,
     * use it.  If all of the outer 3 are true, use outer.  We currently
     * can't cover the case where only part of the outer is rock, so
     * we just say that all the walls are finished (if not overridden
     * by the inner section).
     */
    if (is_4) {
	wmode = WM_C_INNER;
    } else if (is_1 && is_2 && is_3)
	wmode = WM_C_OUTER;
     else
	wmode = 0;	/* finished walls on all sides */

    return wmode;
}

/* Return mode for a crosswall. */
STATIC_OVL int
set_crosswall(x, y)
    int x, y;
{
    int wmode, is_1, is_2, is_3, is_4;

    is_1 = check_pos(x-1, y-1, 1);
    is_2 = check_pos(x+1, y-1, 1);
    is_3 = check_pos(x+1, y+1, 1);
    is_4 = check_pos(x-1, y+1, 1);

    wmode = is_1+is_2+is_3+is_4;
    if (wmode > 1) {
	if (is_1 && is_3 && (is_2+is_4 == 0)) {
	    wmode = WM_X_TLBR;
	} else if (is_2 && is_4 && (is_1+is_3 == 0)) {
	    wmode = WM_X_BLTR;
	} else {
#ifdef WA_VERBOSE
	    error4(x,y,is_1,is_2,is_3,is_4);
#endif
	    wmode = 0;
	}
    } else if (is_1)
	wmode = WM_X_TL;
    else if (is_2)
	wmode = WM_X_TR;
    else if (is_3)
	wmode = WM_X_BR;
    else if (is_4)
	wmode = WM_X_BL;

    return wmode;
}

/* Called from mklev.  Scan the level and set the wall modes. */
void
set_wall_state()
{
    int x, y;
    int wmode;
    struct rm *lev;

#ifdef WA_VERBOSE
    for (x = 0; x < MAX_TYPE; x++) bad_count[x] = 0;
#endif

    for (x = 0; x < COLNO; x++)
	for (lev = &levl[x][0], y = 0; y < ROWNO; y++, lev++) {
	    switch (lev->typ) {
		case SDOOR:
		    wmode = set_wall(x, y, (int) lev->horizontal);
		    break;
		case VWALL:
		    wmode = set_wall(x, y, 0);
		    break;
		case HWALL:
		    wmode = set_wall(x, y, 1);
		    break;
		case TDWALL:
		    wmode = set_twall(x,y, x,y-1, x-1,y+1, x+1,y+1);
		    break;
		case TUWALL:
		    wmode = set_twall(x,y, x,y+1, x+1,y-1, x-1,y-1);
		    break;
		case TLWALL:
		    wmode = set_twall(x,y, x+1,y, x-1,y-1, x-1,y+1);
		    break;
		case TRWALL:
		    wmode = set_twall(x,y, x-1,y, x+1,y+1, x+1,y-1);
		    break;
		case TLCORNER:
		    wmode = set_corn(x-1,y-1, x,y-1, x-1,y, x+1,y+1);
		    break;
		case TRCORNER:
		    wmode = set_corn(x,y-1, x+1,y-1, x+1,y, x-1,y+1);
		    break;
		case BLCORNER:
		    wmode = set_corn(x,y+1, x-1,y+1, x-1,y, x+1,y-1);
		    break;
		case BRCORNER:
		    wmode = set_corn(x+1,y, x+1,y+1, x,y+1, x-1,y-1);
		    break;
		case CROSSWALL:
		    wmode = set_crosswall(x, y);
		    break;

		default:
		    wmode = -1;	/* don't set wall info */
		    break;
	    }

	if (wmode >= 0)
	    lev->wall_info = (lev->wall_info & ~WM_MASK) | wmode;
	}

#ifdef WA_VERBOSE
    /* check if any bad positions found */
    for (x = y = 0; x < MAX_TYPE; x++)
	if (bad_count[x]) {
	    if (y == 0) {
		y = 1;	/* only print once */
		pline("set_wall_type: wall mode problems with: ");
	    }
	    pline("%s %d;", type_names[x], bad_count[x]);
	}
#endif /* WA_VERBOSE */
}

/* ------------------------------------------------------------------------- */
/* This matrix is used here and in vision.c. */
unsigned char seenv_matrix[3][3] = { {SV2,   SV1, SV0},
				     {SV3, SVALL, SV7},
				     {SV4,   SV5, SV6} };

#define sign(z) ((z) < 0 ? -1 : ((z) > 0 ? 1 : 0))

/* Set the seen vector of lev as if seen from (x0,y0) to (x,y). */
STATIC_OVL void
set_seenv(lev, x0, y0, x, y)
    struct rm *lev;
    int x0, y0, x, y;	/* from, to */
{
    int dx = x-x0, dy = y0-y;
    lev->seenv |= seenv_matrix[sign(dy)+1][sign(dx)+1];
}

/* ------------------------------------------------------------------------- */

/* T wall types, one for each row in wall_matrix[][]. */
#define T_d 0
#define T_l 1
#define T_u 2
#define T_r 3

/*
 * These are the column names of wall_matrix[][].  They are the "results"
 * of a tdwall pattern match.  All T walls are rotated so they become
 * a tdwall.  Then we do a single pattern match, but return the
 * correct result for the original wall by using different rows for
 * each of the wall types.
 */
#define T_stone  0
#define T_tlcorn 1
#define T_trcorn 2
#define T_hwall  3
#define T_tdwall 4

static const int wall_matrix[4][5] = {
    { S_stone, S_tlcorn, S_trcorn, S_hwall, S_tdwall },	/* tdwall */
    { S_stone, S_trcorn, S_brcorn, S_vwall, S_tlwall },	/* tlwall */
    { S_stone, S_brcorn, S_blcorn, S_hwall, S_tuwall },	/* tuwall */
    { S_stone, S_blcorn, S_tlcorn, S_vwall, S_trwall },	/* trwall */
};


/* Cross wall types, one for each "solid" quarter.  Rows of cross_matrix[][]. */
#define C_bl 0
#define C_tl 1
#define C_tr 2
#define C_br 3

/*
 * These are the column names for cross_matrix[][].  They express results
 * in C_br (bottom right) terms.  All crosswalls with a single solid
 * quarter are rotated so the solid section is at the bottom right.
 * We pattern match on that, but return the correct result depending
 * on which row we'ere looking at.
 */
#define C_trcorn 0
#define C_brcorn 1
#define C_blcorn 2
#define C_tlwall 3
#define C_tuwall 4
#define C_crwall 5

static const int cross_matrix[4][6] = {
    { S_brcorn, S_blcorn, S_tlcorn, S_tuwall, S_trwall, S_crwall },
    { S_blcorn, S_tlcorn, S_trcorn, S_trwall, S_tdwall, S_crwall },
    { S_tlcorn, S_trcorn, S_brcorn, S_tdwall, S_tlwall, S_crwall },
    { S_trcorn, S_brcorn, S_blcorn, S_tlwall, S_tuwall, S_crwall },
};


/* Print out a T wall warning and all interesting info. */
STATIC_OVL void
t_warn(lev)
    struct rm *lev;
{
    static const char warn_str[] = "wall_angle: %s: case %d: seenv = 0x%x";
    const char *wname;

    if (lev->typ == TUWALL) wname = "tuwall";
    else if (lev->typ == TLWALL) wname = "tlwall";
    else if (lev->typ == TRWALL) wname = "trwall";
    else if (lev->typ == TDWALL) wname = "tdwall";
    else wname = "unknown";
    impossible(warn_str, wname, lev->wall_info & WM_MASK,
	(unsigned int) lev->seenv);
}


/*
 * Return the correct graphics character index using wall type, wall mode,
 * and the seen vector.  It is expected that seenv is non zero.
 *
 * All T-wall vectors are rotated to be TDWALL.  All single crosswall
 * blocks are rotated to bottom right.  All double crosswall are rotated
 * to W_X_BLTR.  All results are converted back.
 *
 * The only way to understand this is to take out pen and paper and
 * draw diagrams.  See rm.h for more details on the wall modes and
 * seen vector (SV).
 */
STATIC_OVL int
wall_angle(lev)
    struct rm *lev;
{
    register unsigned int seenv = lev->seenv & 0xff;
    const int *row;
    int col, idx;

#define only(sv, bits)	(((sv) & (bits)) && ! ((sv) & ~(bits)))
    switch (lev->typ) {
	case TUWALL:
		row = wall_matrix[T_u];
		seenv = (seenv >> 4 | seenv << 4) & 0xff;/* rotate to tdwall */
		goto do_twall;
	case TLWALL:
		row = wall_matrix[T_l];
		seenv = (seenv >> 2 | seenv << 6) & 0xff;/* rotate to tdwall */
		goto do_twall;
	case TRWALL:
		row = wall_matrix[T_r];
		seenv = (seenv >> 6 | seenv << 2) & 0xff;/* rotate to tdwall */
		goto do_twall;
	case TDWALL:
		row = wall_matrix[T_d];
do_twall:
		switch (lev->wall_info & WM_MASK) {
		    case 0:
			if (seenv == SV4) {
			    col = T_tlcorn;
			} else if (seenv == SV6) {
			    col = T_trcorn;
			} else if (seenv & (SV3|SV5|SV7) ||
					    ((seenv & SV4) && (seenv & SV6))) {
			    col = T_tdwall;
			} else if (seenv & (SV0|SV1|SV2)) {
			    col = (seenv & (SV4|SV6) ? T_tdwall : T_hwall);
			} else {
			    t_warn(lev);
			    col = T_stone;
			}
			break;
		    case WM_T_LONG:
			if (seenv & (SV3|SV4) && !(seenv & (SV5|SV6|SV7))) {
			    col = T_tlcorn;
			} else if (seenv&(SV6|SV7) && !(seenv&(SV3|SV4|SV5))) {
			    col = T_trcorn;
			} else if ((seenv & SV5) ||
				((seenv & (SV3|SV4)) && (seenv & (SV6|SV7)))) {
			    col = T_tdwall;
			} else {
			    /* only SV0|SV1|SV2 */
			    if (! only(seenv, SV0|SV1|SV2) )
				t_warn(lev);
			    col = T_stone;
			}
			break;
		    case WM_T_BL:
#if 0	/* older method, fixed */
			if (only(seenv, SV4|SV5)) {
			    col = T_tlcorn;
			} else if ((seenv & (SV0|SV1|SV2)) &&
					only(seenv, SV0|SV1|SV2|SV6|SV7)) {
			    col = T_hwall;
			} else if (seenv & SV3 ||
			    ((seenv & (SV0|SV1|SV2)) && (seenv & (SV4|SV5)))) {
			    col = T_tdwall;
			} else {
			    if (seenv != SV6)
				t_warn(lev);
			    col = T_stone;
			}
#endif	/* 0 */
			if (only(seenv, SV4|SV5))
			    col = T_tlcorn;
			else if ((seenv & (SV0|SV1|SV2|SV7)) &&
					!(seenv & (SV3|SV4|SV5)))
			    col = T_hwall;
			else if (only(seenv, SV6))
			    col = T_stone;
			else
			    col = T_tdwall;
			break;
		    case WM_T_BR:
#if 0	/* older method, fixed */
			if (only(seenv, SV5|SV6)) {
			    col = T_trcorn;
			} else if ((seenv & (SV0|SV1|SV2)) &&
					    only(seenv, SV0|SV1|SV2|SV3|SV4)) {
			    col = T_hwall;
			} else if (seenv & SV7 ||
			    ((seenv & (SV0|SV1|SV2)) && (seenv & (SV5|SV6)))) {
			    col = T_tdwall;
			} else {
			    if (seenv != SV4)
				t_warn(lev);
			    col = T_stone;
			}
#endif	/* 0 */
			if (only(seenv, SV5|SV6))
			    col = T_trcorn;
			else if ((seenv & (SV0|SV1|SV2|SV3)) &&
					!(seenv & (SV5|SV6|SV7)))
			    col = T_hwall;
			else if (only(seenv, SV4))
			    col = T_stone;
			else
			    col = T_tdwall;

			break;
		    default:
			if (wizard) impossible("wall_angle: unknown T wall mode %d",
				lev->wall_info & WM_MASK);
			col = T_stone;
			break;
		}
		idx = row[col];
		break;

	case SDOOR:
		if (lev->horizontal) goto horiz;
		/* fall through */
	case VWALL:
		switch (lev->wall_info & WM_MASK) {
		    case 0: idx = seenv ? S_vwall : /*S_stone*/S_dungwall; break;
		    case 1: idx = seenv & (SV1|SV2|SV3|SV4|SV5) ? S_vwall :
								  /*S_stone*/S_dungwall;
			    break;
		    case 2: idx = seenv & (SV0|SV1|SV5|SV6|SV7) ? S_vwall :
								  /*S_stone*/S_dungwall;
			    break;
		    default:
			if (wizard) impossible("wall_angle: unknown vwall mode %d",
				lev->wall_info & WM_MASK);
			idx = /*S_stone*/S_dungwall;
			break;
		}
		break;

	case HWALL:
horiz:
		switch (lev->wall_info & WM_MASK) {
		    case 0: idx = seenv ? S_hwall : /*S_stone*/S_dungwall; break;
		    case 1: idx = seenv & (SV3|SV4|SV5|SV6|SV7) ? S_hwall :
								  /*S_stone*/S_dungwall;
			    break;
		    case 2: idx = seenv & (SV0|SV1|SV2|SV3|SV7) ? S_hwall :
								  /*S_stone*/S_dungwall;
			    break;
		    default:
			if (wizard) impossible("wall_angle: unknown hwall mode %d",
				lev->wall_info & WM_MASK);
			idx = /*S_stone*/S_dungwall;
			break;
		}
		break;

#define set_corner(idx, lev, which, outer, inner, name)	\
    switch ((lev)->wall_info & WM_MASK) {				    \
	case 0:		 idx = which; break;				    \
	case WM_C_OUTER: idx = seenv &  (outer) ? which : /*S_stone*/S_dungwall; break;   \
	case WM_C_INNER: idx = seenv & ~(inner) ? which : /*S_stone*/S_dungwall; break;   \
	default:							    \
	    if (wizard) impossible("wall_angle: unknown %s mode %d", name,		    \
		(lev)->wall_info & WM_MASK);				    \
	    idx = /*S_stone*/S_dungwall;						    \
	    break;							    \
    }

	case TLCORNER:
	    set_corner(idx, lev, S_tlcorn, (SV3|SV4|SV5), SV4, "tlcorn");
	    break;
	case TRCORNER:
	    set_corner(idx, lev, S_trcorn, (SV5|SV6|SV7), SV6, "trcorn");
	    break;
	case BLCORNER:
	    set_corner(idx, lev, S_blcorn, (SV1|SV2|SV3), SV2, "blcorn");
	    break;
	case BRCORNER:
	    set_corner(idx, lev, S_brcorn, (SV7|SV0|SV1), SV0, "brcorn");
	    break;


	case CROSSWALL:
		switch (lev->wall_info & WM_MASK) {
		    case 0:
			if (seenv == SV0)
			    idx = S_brcorn;
			else if (seenv == SV2)
			    idx = S_blcorn;
			else if (seenv == SV4)
			    idx = S_tlcorn;
			else if (seenv == SV6)
			    idx = S_trcorn;
			else if (!(seenv & ~(SV0|SV1|SV2)) &&
					(seenv & SV1 || seenv == (SV0|SV2)))
			    idx = S_tuwall;
			else if (!(seenv & ~(SV2|SV3|SV4)) &&
					(seenv & SV3 || seenv == (SV2|SV4)))
			    idx = S_trwall;
			else if (!(seenv & ~(SV4|SV5|SV6)) &&
					(seenv & SV5 || seenv == (SV4|SV6)))
			    idx = S_tdwall;
			else if (!(seenv & ~(SV0|SV6|SV7)) &&
					(seenv & SV7 || seenv == (SV0|SV6)))
			    idx = S_tlwall;
			else
			    idx = S_crwall;
			break;

		    case WM_X_TL:
			row = cross_matrix[C_tl];
			seenv = (seenv >> 4 | seenv << 4) & 0xff;
			goto do_crwall;
		    case WM_X_TR:
			row = cross_matrix[C_tr];
			seenv = (seenv >> 6 | seenv << 2) & 0xff;
			goto do_crwall;
		    case WM_X_BL:
			row = cross_matrix[C_bl];
			seenv = (seenv >> 2 | seenv << 6) & 0xff;
			goto do_crwall;
		    case WM_X_BR:
			row = cross_matrix[C_br];
do_crwall:
			if (seenv == SV4)
			    idx = /*S_stone*/S_dungwall;
			else {
			    seenv = seenv & ~SV4;	/* strip SV4 */
			    if (seenv == SV0) {
				col = C_brcorn;
			    } else if (seenv & (SV2|SV3)) {
				if (seenv & (SV5|SV6|SV7))
				    col = C_crwall;
				else if (seenv & (SV0|SV1))
				    col = C_tuwall;
				else
				    col = C_blcorn;
			    } else if (seenv & (SV5|SV6)) {
				if (seenv & (SV1|SV2|SV3))
				    col = C_crwall;
				else if (seenv & (SV0|SV7))
				    col = C_tlwall;
				else
				    col = C_trcorn;
			    } else if (seenv & SV1) {
				col = seenv & SV7 ? C_crwall : C_tuwall;
			    } else if (seenv & SV7) {
				col = seenv & SV1 ? C_crwall : C_tlwall;
			    } else {
				if (wizard) impossible(
				    "wall_angle: bottom of crwall check");
				col = C_crwall;
			    }

			    idx = row[col];
			}
			break;

		    case WM_X_TLBR:
			if ( only(seenv, SV1|SV2|SV3) )
			    idx = S_blcorn;
			else if ( only(seenv, SV5|SV6|SV7) )
			    idx = S_trcorn;
			else if ( only(seenv, SV0|SV4) )
			    idx = /*S_stone*/S_dungwall;
			else
			    idx = S_crwall;
			break;

		    case WM_X_BLTR:
			if ( only(seenv, SV0|SV1|SV7) )
			    idx = S_brcorn;
			else if ( only(seenv, SV3|SV4|SV5) )
			    idx = S_tlcorn;
			else if ( only(seenv, SV2|SV6) )
			    idx = /*S_stone*/S_dungwall;
			else
			    idx = S_crwall;
			break;

		    default:
			if (wizard) impossible("wall_angle: unknown crosswall mode");
			idx = /*S_stone*/S_dungwall;
			break;
		}
		break;

	default:
	    if (wizard) impossible("wall_angle: unexpected wall type %d", lev->typ);
	    idx = /*S_stone*/S_dungwall;
    }
    return idx;
}

/*display.c*/
