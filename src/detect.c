/*	SCCS Id: @(#)detect.c	3.4	2003/08/13	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/*
 * Detection routines, including crystal ball, magic mapping, and search
 * command.
 */

#include "hack.h"
#include "artifact.h"

extern boolean known;	/* from read.c */

STATIC_DCL void do_dknown_of(struct obj *);
STATIC_DCL boolean check_map_spot(int,int,CHAR_P,unsigned);
STATIC_DCL boolean clear_stale_map(CHAR_P,unsigned);
STATIC_DCL void sense_trap(struct trap *,XCHAR_P,XCHAR_P,int);
STATIC_DCL void show_map_spot(int,int);
STATIC_DCL void show_map_spotX(int,int);
STATIC_PTR void findone(int,int,void *);
STATIC_PTR void findoneX(int,int,void *);
STATIC_PTR void openone(int,int,void *);

/* Recursively search obj for an object in class oclass and return 1st found */
struct obj *
o_in(obj, oclass)
struct obj* obj;
char oclass;
{
    register struct obj* otmp;
    struct obj *temp;

    if (obj->oclass == oclass) return obj;

    /*
     * Pills inside medical kits are specially handled (see apply.c).
     * We don't want them to detect as food because then they will be
     * shown as pink pills, which are something quite different. In
     * practice the only other possible contents of medical kits are
     * bandages and phials, neither of which is detectable by any
     * means so we can simply avoid looking in medical kits.
     */
    if (Has_contents(obj) && obj->otyp != MEDICAL_KIT) {
	for (otmp = obj->cobj; otmp; otmp = otmp->nobj)
	    if (otmp->oclass == oclass) return otmp;
	    else if (Has_contents(otmp) && otmp->otyp != MEDICAL_KIT &&
		    (temp = o_in(otmp, oclass)))
		return temp;
    }
    return (struct obj *) 0;
}

/* Recursively search obj for an object made of specified material and return 1st found */
struct obj *
o_material(obj, material)
struct obj* obj;
unsigned material;
{
    register struct obj* otmp;
    struct obj *temp;

    if (objects[obj->otyp].oc_material == material) return obj;

    if (Has_contents(obj)) {
	for (otmp = obj->cobj; otmp; otmp = otmp->nobj)
	    if (objects[otmp->otyp].oc_material == material) return otmp;
	    else if (Has_contents(otmp) && (temp = o_material(otmp, material)))
		return temp;
    }
    return (struct obj *) 0;
}

STATIC_OVL void
do_dknown_of(obj)
struct obj *obj;
{
    struct obj *otmp;

    obj->dknown = 1;
    if (Has_contents(obj)) {
	for(otmp = obj->cobj; otmp; otmp = otmp->nobj)
	    do_dknown_of(otmp);
    }
}

/* Check whether the location has an outdated object displayed on it. */
STATIC_OVL boolean
check_map_spot(x, y, oclass, material)
int x, y;
register char oclass;
unsigned material;
{
	register int glyph;
	register struct obj *otmp;
	register struct monst *mtmp;

	glyph = glyph_at(x,y);
	if (glyph_is_object(glyph)) {
	    /* there's some object shown here */
	    if (oclass == ALL_CLASSES) {
		return((boolean)( !(level.objects[x][y] ||     /* stale if nothing here */
			    ((mtmp = m_at(x,y)) != 0 &&
				(
#ifndef GOLDOBJ
				 mtmp->mgold ||
#endif
						 mtmp->minvent)))));
	    } else {
		if (material && objects[glyph_to_obj(glyph)].oc_material == material) {
			/* the object shown here is of interest because material matches */
			for (otmp = level.objects[x][y]; otmp; otmp = otmp->nexthere)
				if (o_material(otmp, MT_GOLD)) return FALSE;
			/* didn't find it; perhaps a monster is carrying it */
			if ((mtmp = m_at(x,y)) != 0) {
				for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
					if (o_material(otmp, MT_GOLD)) return FALSE;
		        }
			/* detection indicates removal of this object from the map */
			return TRUE;
		}
	        if (oclass && objects[glyph_to_obj(glyph)].oc_class == oclass) {
			/* the object shown here is of interest because its class matches */
			for (otmp = level.objects[x][y]; otmp; otmp = otmp->nexthere)
				if (o_in(otmp, oclass)) return FALSE;
			/* didn't find it; perhaps a monster is carrying it */
#ifndef GOLDOBJ
			if ((mtmp = m_at(x,y)) != 0) {
				if (oclass == COIN_CLASS && mtmp->mgold)
					return FALSE;
				else for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
					if (o_in(otmp, oclass)) return FALSE;
		        }
#else
			if ((mtmp = m_at(x,y)) != 0) {
				for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
					if (o_in(otmp, oclass)) return FALSE;
		        }
#endif
			/* detection indicates removal of this object from the map */
			return TRUE;
	        }
	    }
	}
	return FALSE;
}

/*
   When doing detection, remove stale data from the map display (corpses
   rotted away, objects carried away by monsters, etc) so that it won't
   reappear after the detection has completed.  Return true if noticeable
   change occurs.
 */
STATIC_OVL boolean
clear_stale_map(oclass, material)
register char oclass;
unsigned material;
{
	register int zx, zy;
	register boolean change_made = FALSE;

	for (zx = 1; zx < COLNO; zx++)
	    for (zy = 0; zy < ROWNO; zy++)
		if (check_map_spot(zx, zy, oclass,material)) {
		    unmap_object(zx, zy);
		    change_made = TRUE;
		}

	return change_made;
}

/* look for gold, on the floor or in monsters' possession */
int
gold_detect(sobj)
register struct obj *sobj;
{
    register struct obj *obj;
    register struct monst *mtmp;
    int uw = u.uinwater;
    struct obj *temp;
    boolean stale;

    known = stale = clear_stale_map(COIN_CLASS,
				(unsigned)(sobj->blessed ? MT_GOLD : 0));

    if (DetectionMethodsDontWork) {
		if (sobj) strange_feeling(sobj, "Huh.");
		return 0;
    }

    /* look for gold carried by monsters (might be in a container) */
    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
    	if (DEADMONSTER(mtmp)) continue;	/* probably not needed in this case but... */
#ifndef GOLDOBJ
	if (mtmp->mgold || monsndx(mtmp->data) == PM_GOLD_GOLEM) {
#else
	if (findgold(mtmp->minvent) || monsndx(mtmp->data) == PM_GOLD_GOLEM) {
#endif
	    known = TRUE;
	    goto outgoldmap;	/* skip further searching */
	} else for (obj = mtmp->minvent; obj; obj = obj->nobj)
	    if (sobj->blessed && o_material(obj, MT_GOLD)) {
	    	known = TRUE;
	    	goto outgoldmap;
	    } else if (o_in(obj, COIN_CLASS)) {
		known = TRUE;
		goto outgoldmap;	/* skip further searching */
	    }
    }
    
    /* look for gold objects */
    for (obj = fobj; obj; obj = obj->nobj) {
	if (sobj->blessed && o_material(obj, MT_GOLD)) {
	    known = TRUE;
	    if (obj->ox != u.ux || obj->oy != u.uy) goto outgoldmap;
	} else if (o_in(obj, COIN_CLASS)) {
	    known = TRUE;
	    if (obj->ox != u.ux || obj->oy != u.uy) goto outgoldmap;
	}
    }

    if (!known) {
	/* no gold found on floor or monster's inventory.
	   adjust message if you have gold in your inventory */
	if (sobj) {
		char buf[BUFSZ];
		if (youmonst.data == &mons[PM_GOLD_GOLEM]) {
			sprintf(buf, "You feel like a million %s!",
				currency(2L));
		} else if (hidden_gold() ||
#ifndef GOLDOBJ
				u.ugold)
#else
			        money_cnt(invent))
#endif
			strcpy(buf,
				"You feel worried about your future financial situation.");
		else
			strcpy(buf, "You feel materially poor.");
		strange_feeling(sobj, buf);
        }
	return(1);
    }
    /* only under me - no separate display required */
    if (stale) docrt();
    You("notice some gold between your %s.", makeplural(body_part(FOOT)));
    return(0);

outgoldmap:
    cls();

    u.uinwater = 0;
    /* Discover gold locations. */
    for (obj = fobj; obj; obj = obj->nobj) {
    	if (sobj->blessed && (temp = o_material(obj, MT_GOLD))) {
	    if (temp != obj) {
		temp->ox = obj->ox;
		temp->oy = obj->oy;
	    }
	    map_object(temp,1);
	} else if ((temp = o_in(obj, COIN_CLASS))) {
	    if (temp != obj) {
		temp->ox = obj->ox;
		temp->oy = obj->oy;
	    }
	    map_object(temp,1);
	}
    }
    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
    	if (DEADMONSTER(mtmp)) continue;	/* probably overkill here */
#ifndef GOLDOBJ
	if (mtmp->mgold || monsndx(mtmp->data) == PM_GOLD_GOLEM) {
#else
	if (findgold(mtmp->minvent) || monsndx(mtmp->data) == PM_GOLD_GOLEM) {
#endif
	    struct obj gold;

	    gold.otyp = GOLD_PIECE;
	    gold.ox = mtmp->mx;
	    gold.oy = mtmp->my;
	    map_object(&gold,1);
	} else for (obj = mtmp->minvent; obj; obj = obj->nobj)
	    if (sobj->blessed && (temp = o_material(obj, MT_GOLD))) {
		temp->ox = mtmp->mx;
		temp->oy = mtmp->my;
		map_object(temp,1);
		break;
	    } else if ((temp = o_in(obj, COIN_CLASS))) {
		temp->ox = mtmp->mx;
		temp->oy = mtmp->my;
		map_object(temp,1);
		break;
	    }
    }
    
    newsym(u.ux,u.uy);
    You_feel("very greedy, and sense gold!");
    exercise(A_WIS, TRUE);
    display_nhwindow(WIN_MAP, TRUE);
    docrt();
    u.uinwater = uw;
    if (Underwater && !Swimming) under_water(2);
    if (u.uburied) under_ground(2);
    return(0);
}

/* returns 1 if nothing was detected		*/
/* returns 0 if something was detected		*/
int
food_detect(sobj)
register struct obj	*sobj;
{
    register struct obj *obj;
    register struct monst *mtmp;
    register int ct = 0, ctu = 0;
    boolean confused = (Confusion || (sobj && sobj->cursed)), stale;
    char oclass = confused ? POTION_CLASS : FOOD_CLASS;
    const char *what = confused ? something : "food";
    int uw = u.uinwater;

    boolean stupiddetect = (sobj && (sobj->otyp == SPE_DETECT_FOOD));
    boolean guaranteed = (!sobj || (sobj && (sobj->otyp != SPE_DETECT_FOOD)) );

    stale = clear_stale_map(oclass, 0);

    if (DetectionMethodsDontWork) {
	if (sobj) strange_feeling(sobj, "Huh.");
	return 0;
    }

    for (obj = fobj; obj; obj = obj->nobj)
	if (o_in(obj, oclass)) {
	    if (obj->ox == u.ux && obj->oy == u.uy) ctu++;
	    else ct++;
	}
    for (mtmp = fmon; mtmp && !ct; mtmp = mtmp->nmon) {
	/* no DEADMONSTER(mtmp) check needed since dmons never have inventory */
	for (obj = mtmp->minvent; obj; obj = obj->nobj)
	    if (o_in(obj, oclass)) {
		ct++;
		break;
	    }
    }
    
    if (!ct && !ctu) {
	known = stale && !confused;
	if (stale) {
	    docrt();
	    You("sense a lack of %s nearby.", what);
	    if (sobj && sobj->blessed) {
		if (!u.urealedibility) Your("%s starts to tingle.", body_part(NOSE));
		u.urealedibility += 1;
	    }
	} else if (sobj) {
	    char buf[BUFSZ];
	    sprintf(buf, "Your %s twitches%s.", body_part(NOSE),
			(sobj->blessed && !u.urealedibility) ? " then starts to tingle" : "");
	    if (sobj->blessed && !u.urealedibility) {
		boolean savebeginner = flags.beginner;	/* prevent non-delivery of */
		flags.beginner = FALSE;			/* 	message            */
		strange_feeling(sobj, buf);
		flags.beginner = savebeginner;
		u.urealedibility += 1;
	    } else {
		if (sobj->blessed) u.urealedibility += 1;
		strange_feeling(sobj, buf);
	    }
	}
	return !stale;
    } else if (!ct) {
	known = TRUE;
	You("%s %s nearby.", sobj ? "smell" : "sense", what);
	if (sobj && sobj->blessed) {
		if (!u.urealedibility) pline("Your %s starts to tingle.", body_part(NOSE));
		u.urealedibility += 1;
	}
    } else {
	struct obj *temp;
	known = TRUE;
	cls();
	u.uinwater = 0;
	for (obj = fobj; obj; obj = obj->nobj)
	    if ((temp = o_in(obj, oclass)) != 0) {
		if (temp != obj) {
		    temp->ox = obj->ox;
		    temp->oy = obj->oy;
		}
		if ( (guaranteed || rn2(2)) && !(stupiddetect && isok(temp->ox, temp->oy) && isok(u.ux, u.uy) && (dist2(u.ux, u.uy, temp->ox, temp->oy) > 1601) ) ) map_object(temp, 1);

	    }
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
	    /* no DEADMONSTER(mtmp) check needed since dmons never have inventory */
	    for (obj = mtmp->minvent; obj; obj = obj->nobj)
		if ((temp = o_in(obj, oclass)) != 0) {
		    temp->ox = mtmp->mx;
		    temp->oy = mtmp->my;

		    if ( (guaranteed || rn2(2)) && !(stupiddetect && isok(temp->ox, temp->oy) && isok(u.ux, u.uy) && (dist2(u.ux, u.uy, temp->ox, temp->oy) > 1601) ) ) map_object(temp,1);
		    break;	/* skip rest of this monster's inventory */
		}
	newsym(u.ux,u.uy);
	if (sobj) {
	    if (sobj->blessed) {
	    	Your("%s %s to tingle and you smell %s.", body_part(NOSE),
	    		u.urealedibility ? "continues" : "starts", what);
		u.urealedibility += 1;
	    } else
		Your("%s tingles and you smell %s.", body_part(NOSE), what);
	}
	else You("sense %s.", what);
	display_nhwindow(WIN_MAP, TRUE);
	exercise(A_WIS, TRUE);
	docrt();
	u.uinwater = uw;
	if (Underwater && !Swimming) under_water(2);
	if (u.uburied) under_ground(2);
    }
    return(0);
}

/*
 * Used for scrolls, potions, spells, and crystal balls.  Returns:
 *
 *	1 - nothing was detected
 *	0 - something was detected
 */
int
object_detect(detector, class, guaranteed_det)
struct obj	*detector;	/* object doing the detecting */
int		class;		/* an object class, 0 for all */
boolean guaranteed_det; /* who the fuck came up with this function... wanna be able to have it guaranteed even if "detector" is undefined --Amy */
{
    register int x, y;
    char stuff[BUFSZ];
    int is_cursed = (detector && detector->cursed);
    int do_dknown = (detector && (detector->oclass == POTION_CLASS ||
				    detector->oclass == SPBOOK_CLASS ||
					detector->oartifact ) &&
			detector->blessed);
    int guaranteed = (detector && !(detector->otyp == SPE_DETECT_TREASURE) && !(detector->otyp == SPE_RANDOM_DETECTION) && !(detector->otyp == SPE_MAP_LEVEL) && !(detector->otyp == SPE_MAGIC_MAPPING));
    boolean stupiddetect = (detector && (detector->otyp == SPE_DETECT_TREASURE || detector->otyp == SPE_RANDOM_DETECTION || detector->otyp == SPE_MAGIC_MAPPING));
    int likely = (detector && (detector->otyp == SPE_MAP_LEVEL));
    int ct = 0, ctu = 0;
    register struct obj *obj, *otmp = (struct obj *)0;
    register struct monst *mtmp;
    int uw = u.uinwater;
    int sym, boulder = 0;

    if (guaranteed_det) guaranteed = TRUE;

    if (DetectionMethodsDontWork) {
	if (detector) strange_feeling(detector, "Huh.");
	return 1;
    }

    if (class < 0 || class >= MAXOCLASSES) {
	impossible("object_detect:  illegal class %d", class);
	class = 0;
    }

    /* Special boulder symbol check - does the class symbol happen
     * to match iflags.bouldersym which is a user-defined?
     * If so, that means we aren't sure what they really wanted to
     * detect. Rather than trump anything, show both possibilities.
     * We can exclude checking the buried obj chain for boulders below.
     */
    sym = class ? def_oc_syms[class] : 0;
    if (sym && iflags.bouldersym && sym == iflags.bouldersym)
    	boulder = ROCK_CLASS;

    if (Hallucination || (Confusion && class == SCROLL_CLASS))
	strcpy(stuff, something);
    else
    	strcpy(stuff, class ? oclass_names[class] : "objects");
    if (boulder && class != ROCK_CLASS) strcat(stuff, " and/or large stones");

    if (do_dknown) for(obj = invent; obj; obj = obj->nobj) {
	if (guaranteed || (likely && rn2(2)) || !rn2(3)) do_dknown_of(obj);
	}

    for (obj = fobj; obj; obj = obj->nobj) {
	if ((!class && !boulder) || o_in(obj, class) || o_in(obj, boulder)) {
	    if (obj->ox == u.ux && obj->oy == u.uy) ctu++;
	    else ct++;
	}
	if (do_dknown && (guaranteed || (likely && rn2(2)) || !rn2(3)) ) do_dknown_of(obj);
    }

    for (obj = level.buriedobjlist; obj; obj = obj->nobj) {
	if (!class || o_in(obj, class)) {
	    if (obj->ox == u.ux && obj->oy == u.uy) ctu++;
	    else ct++;
	}
	if (do_dknown && (guaranteed || (likely && rn2(2)) || !rn2(3)) ) do_dknown_of(obj);
    }

    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	if (DEADMONSTER(mtmp)) continue;
	for (obj = mtmp->minvent; obj; obj = obj->nobj) {
	    if ((!class && !boulder) || o_in(obj, class) || o_in(obj, boulder)) ct++;
	    if (do_dknown && (guaranteed || (likely && rn2(2)) || !rn2(3)) ) do_dknown_of(obj);
	}
	if ((is_cursed && mtmp->m_ap_type == M_AP_OBJECT &&
	    (!class || class == objects[mtmp->mappearance].oc_class)) ||
#ifndef GOLDOBJ
	    (mtmp->mgold && (!class || class == COIN_CLASS))) {
#else
	    (findgold(mtmp->minvent) && (!class || class == COIN_CLASS))) {
#endif
	    ct++;
	    break;
	}
    }

    if (!clear_stale_map(!class ? ALL_CLASSES : class, 0) && !ct) {
	if (!ctu) {
	    if (detector)
		strange_feeling(detector, "You feel a lack of something.");
	    return 1;
	}

	You("sense %s nearby.", stuff);
	return 0;
    }

    cls();

    u.uinwater = 0;
/*
 *	Map all buried objects first.
 */
    for (obj = level.buriedobjlist; obj; obj = obj->nobj)
	if (!class || (otmp = o_in(obj, class))) {
	    if (class) {
		if (otmp != obj) {
		    otmp->ox = obj->ox;
		    otmp->oy = obj->oy;
		}
		if ((guaranteed || (likely && rn2(2)) || !rn2(3)) && !(stupiddetect && isok(obj->ox, obj->oy) && isok(u.ux, u.uy) && (dist2(u.ux, u.uy, obj->ox, obj->oy) > 1226) ) ) map_object(otmp, 1);
	    } else
		if ((guaranteed || (likely && rn2(2)) || !rn2(3)) && !(stupiddetect && isok(obj->ox, obj->oy) && isok(u.ux, u.uy) && (dist2(u.ux, u.uy, obj->ox, obj->oy) > 1226) ) ) map_object(obj, 1);
	}
    /*
     * If we are mapping all objects, map only the top object of a pile or
     * the first object in a monster's inventory.  Otherwise, go looking
     * for a matching object class and display the first one encountered
     * at each location.
     *
     * Objects on the floor override buried objects.
     */
    for (x = 1; x < COLNO; x++)
	for (y = 0; y < ROWNO; y++)
	    for (obj = level.objects[x][y]; obj; obj = obj->nexthere)
		if ((!class && !boulder) ||
		    (otmp = o_in(obj, class)) || (otmp = o_in(obj, boulder))) {
		    if (class || boulder) {
			if (otmp != obj) {
			    otmp->ox = obj->ox;
			    otmp->oy = obj->oy;
			}
			if ((guaranteed || (likely && rn2(2)) || !rn2(3)) && !(stupiddetect && isok(obj->ox, obj->oy) && isok(u.ux, u.uy) && (dist2(u.ux, u.uy, obj->ox, obj->oy) > 1226) ) ) map_object(otmp, 1);
		    } else
			if ((guaranteed || (likely && rn2(2)) || !rn2(3)) && !(stupiddetect && isok(obj->ox, obj->oy) && isok(u.ux, u.uy) && (dist2(u.ux, u.uy, obj->ox, obj->oy) > 1226) ) ) map_object(obj, 1);
		    break;
		}

    /* Objects in the monster's inventory override floor objects. */
    for (mtmp = fmon ; mtmp ; mtmp = mtmp->nmon) {
	if (DEADMONSTER(mtmp)) continue;
	for (obj = mtmp->minvent; obj; obj = obj->nobj)
	    if ((!class && !boulder) ||
		 (otmp = o_in(obj, class)) || (otmp = o_in(obj, boulder))) {
		if (!class && !boulder) otmp = obj;
		otmp->ox = mtmp->mx;		/* at monster location */
		otmp->oy = mtmp->my;
		if ((guaranteed || (likely && rn2(2)) || !rn2(3)) && !(stupiddetect && isok(obj->ox, obj->oy) && isok(u.ux, u.uy) && (dist2(u.ux, u.uy, obj->ox, obj->oy) > 1226) ) ) map_object(otmp, 1);
		break;
	    }
	/* Allow a mimic to override the detected objects it is carrying. */
	if (is_cursed && mtmp->m_ap_type == M_AP_OBJECT &&
		(!class || class == objects[mtmp->mappearance].oc_class)) {
	    struct obj temp;

	    temp.otyp = mtmp->mappearance;	/* needed for obj_to_glyph() */
	    temp.ox = mtmp->mx;
	    temp.oy = mtmp->my;
	    temp.corpsenm = PM_TENGU;		/* if mimicing a corpse */
	    map_object(&temp, 1);
#ifndef GOLDOBJ
	} else if (mtmp->mgold && (!class || class == COIN_CLASS)) {
#else
	} else if (findgold(mtmp->minvent) && (!class || class == COIN_CLASS)) {
#endif
	    struct obj gold;

	    gold.otyp = GOLD_PIECE;
	    gold.ox = mtmp->mx;
	    gold.oy = mtmp->my;
	    if ((guaranteed || (likely && rn2(2)) || !rn2(3)) && !(stupiddetect && isok(mtmp->mx, mtmp->my) && isok(u.ux, u.uy) && (dist2(u.ux, u.uy, mtmp->mx, mtmp->my) > 1226) ) ) map_object(&gold, 1);
	}
    }

    newsym(u.ux,u.uy);
    You("detect the %s of %s.", ct ? "presence" : "absence", stuff);
    display_nhwindow(WIN_MAP, TRUE);
    /*
     * What are we going to do when the hero does an object detect while blind
     * and the detected object covers a known pool?
     */
    docrt();	/* this will correctly reset vision */

    u.uinwater = uw;
    if (Underwater && !Swimming) under_water(2);
    if (u.uburied) under_ground(2);
    return 0;
}

/* function by Amy: reveal "tileamount" random tiles on the current level */
void
displayrandomtiles(tileamount)
int tileamount;
{
	int zx, zy;

	if (tileamount > 100000) tileamount = 100000; /* sanity check */

	if (tileamount < 0) {
		impossible("displayrandomtiles called with wrong argument %d?", tileamount);
		return;
	}

	while (tileamount > 0) {
		tileamount--;

		zx = rn2(COLNO);
		zy = rn2(ROWNO);

		if (isok(zx, zy)) show_map_spot(zx, zy);
	}
	pline("Part of the map has been revealed!");
}

/*
 * Used for artifact effects.  Returns:
 *
 *	1 - nothing was detected
 *	0 - something was detected
 */
int
artifact_detect(detector)
struct obj	*detector;	/* object doing the detecting */
{
    register int x, y;
    char stuff[BUFSZ];
    int is_cursed = (detector && detector->cursed);
    int do_dknown = (detector && (detector->oclass == POTION_CLASS ||
				    detector->oclass == SPBOOK_CLASS ||
					detector->oartifact) &&
			detector->blessed);
    int ct = 0;
    register struct obj *obj, *otmp = (struct obj *)0;
    register struct monst *mtmp;
    int uw = u.uinwater;

    if (DetectionMethodsDontWork) {
	if (detector) strange_feeling(detector, "Huh.");
	return 1;
    }

	if (is_cursed){ /* Possible false negative */
		strange_feeling(detector, "You feel a lack of something.");
	    return 1;
	}
	
    if (Hallucination)
		strcpy(stuff, something);
    else
    	strcpy(stuff, "artifacts");

    if (do_dknown) for(obj = invent; obj; obj = obj->nobj) do_dknown_of(obj);

    for (obj = fobj; obj; obj = obj->nobj) {
	if (obj && obj->oartifact) {
	    if (obj->ox != u.ux || obj->oy != u.uy) ct++;
	}
	if (do_dknown) do_dknown_of(obj);
    }

    for (obj = level.buriedobjlist; obj; obj = obj->nobj) {
	if (obj && obj->oartifact) {
	    if (obj->ox != u.ux || obj->oy != u.uy) ct++;
	}
	if (do_dknown) do_dknown_of(obj);
    }

    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	if (DEADMONSTER(mtmp)) continue;
		for (obj = mtmp->minvent; obj; obj = obj->nobj) {
			if (obj && obj->oartifact) ct++;
			if (do_dknown) do_dknown_of(obj);
		}
	}

    if (!clear_stale_map(ALL_CLASSES, 0) && !ct) {
		strange_feeling(detector, "You feel a lack of something.");
	    return 1;
	}

    cls();

    u.uinwater = 0;
/*
 *	Map all buried objects first.
 */
    for (obj = level.buriedobjlist; obj; obj = obj->nobj)
		if (obj && obj->oartifact) {
			map_object(obj, 1);
		}
    /*
     * If we are mapping all objects, map only the top object of a pile or
     * the first object in a monster's inventory.  Otherwise, go looking
     * for a matching object class and display the first one encountered
     * at each location.
     *
     * Objects on the floor override buried objects.
     */
    for (x = 1; x < COLNO; x++)
	for (y = 0; y < ROWNO; y++)
	    for (obj = level.objects[x][y]; obj; obj = obj->nexthere)
		if (obj && obj->oartifact) {
			map_object(obj, 1);
	break;
		}
    /* Objects in the monster's inventory override floor objects. */
    for (mtmp = fmon ; mtmp ; mtmp = mtmp->nmon) {
	if (DEADMONSTER(mtmp)) continue;
	for (obj = mtmp->minvent; obj; obj = obj->nobj)
	    if (obj && obj->oartifact) {
			map_object(obj, 1);
	break;
	    }
    }

    newsym(u.ux,u.uy);
    You("detect the %s of %s.", ct ? "presence" : "absence", stuff);
    display_nhwindow(WIN_MAP, TRUE);
    /*
     * What are we going to do when the hero does an object detect while blind
     * and the detected object covers a known pool?
     */
    docrt();	/* this will correctly reset vision */

    u.uinwater = uw;
    if (Underwater && !Swimming) under_water(2);
    if (u.uburied) under_ground(2);
    return 0;
}

void
water_detect()
{

    register int zx, zy;
	int detectamount = 0;

	if (DetectionMethodsDontWork) return;

    for (zx = 1; zx < COLNO; zx++)
	for (zy = 0; zy < ROWNO; zy++)
	    if (levl[zx][zy].typ == WATER || levl[zx][zy].typ == POOL || levl[zx][zy].typ == CRYSTALWATER || levl[zx][zy].typ == WATERTUNNEL || levl[zx][zy].typ == WELL || levl[zx][zy].typ == POISONEDWELL || levl[zx][zy].typ == RAINCLOUD || levl[zx][zy].typ == MOAT || levl[zx][zy].typ == FOUNTAIN || levl[zx][zy].typ == SINK || levl[zx][zy].typ == TOILET) {
			show_map_spot(zx, zy);
			detectamount++;
		}

	if (detectamount) pline("You detect some water.");
	else pline("This dungeon level does not seem watery at all.");

	flush_screen(1);			/* flush temp screen */
	display_nhwindow(WIN_MAP, TRUE);	/* wait */
	docrt();

}

void
water_detectX() /* for the spell, which is meant to be weaker than the scroll --Amy */
{

    register int zx, zy;
	int detectamount = 0;

	if (DetectionMethodsDontWork) return;

    for (zx = 1; zx < COLNO; zx++)
	for (zy = 0; zy < ROWNO; zy++)
	    if ((levl[zx][zy].typ == WATER || levl[zx][zy].typ == POOL || levl[zx][zy].typ == CRYSTALWATER || levl[zx][zy].typ == WATERTUNNEL || levl[zx][zy].typ == WELL || levl[zx][zy].typ == POISONEDWELL || levl[zx][zy].typ == RAINCLOUD || levl[zx][zy].typ == MOAT || levl[zx][zy].typ == FOUNTAIN || levl[zx][zy].typ == SINK || levl[zx][zy].typ == TOILET) && rn2(2)) {
			if (!(isok(zx, zy) && isok(u.ux, u.uy) && (dist2(u.ux, u.uy, zx, zy) > 901) ) ) show_map_spot(zx, zy);
			detectamount++;
		}

	if (detectamount) pline("You detect some water.");
	else pline("This dungeon level does not seem watery at all.");

	flush_screen(1);			/* flush temp screen */
	display_nhwindow(WIN_MAP, TRUE);	/* wait */
	docrt();

}

/*
 * Used by: crystal balls, potions, fountains
 *
 * Returns 1 if nothing was detected.
 * Returns 0 if something was detected.
 */
int
monster_detect(otmp, mclass)
register struct obj *otmp;	/* detecting object (if any) */
int mclass;			/* monster class, 0 for all */
{
    register struct monst *mtmp;
    int mcnt = 0;

    if (DetectionMethodsDontWork) {
	if (otmp) strange_feeling(otmp, "Huh.");
	return 1;
    }

    /* Note: This used to just check fmon for a non-zero value
     * but in versions since 3.3.0 fmon can test TRUE due to the
     * presence of dmons, so we have to find at least one
     * with positive hit-points to know for sure.
     */
    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
    	if (!DEADMONSTER(mtmp)) {
		mcnt++;
		break;
	}

    if (!mcnt) {
	if (otmp)
	    strange_feeling(otmp, FunnyHallu ?
			    "You get the heebie jeebies." :
			    "You feel threatened.");
	return 1;
    } else {
	boolean woken = FALSE;

	cls();
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if (!mclass || mtmp->data->mlet == mclass ||
		(mtmp->data == &mons[PM_LONG_WORM] && mclass == S_WORM_TAIL))
		    if ((mtmp->mx > 0) && !(otmp && otmp->oclass == SPBOOK_CLASS && (mtmp && isok(u.ux, u.uy) && isok(mtmp->mx, mtmp->my) && (dist2(u.ux,u.uy,mtmp->mx,mtmp->my) > 101) ) ) && ( !(otmp && otmp->oclass == SPBOOK_CLASS) || rn2(2) ) ) {
		    	if (mclass && def_monsyms[mclass] == ' ')
				show_glyph(mtmp->mx,mtmp->my,
					detected_mon_to_glyph(mtmp));
			else
				show_glyph(mtmp->mx,mtmp->my,mon_to_glyph(mtmp));
			/* don't be stingy - display entire worm */
			if (mtmp->data == &mons[PM_LONG_WORM]) detect_wsegs(mtmp,0);
		    }
	    if (otmp && otmp->cursed &&
		(mtmp->msleeping || !mtmp->mcanmove)) {
		mtmp->msleeping = mtmp->mfrozen = mtmp->masleep = 0;
		mtmp->mcanmove = 1;
		woken = TRUE;
	    }
	}
	display_self();
	You("sense the presence of monsters.");
	if (woken)
	    pline(FunnyHallu ? "They're after you! Now you know why you were always paranoid - they REALLY want to eat you alive!" : "Monsters sense the presence of you.");
	display_nhwindow(WIN_MAP, TRUE);
	docrt();
	if (Underwater && !Swimming) under_water(2);
	if (u.uburied) under_ground(2);
    }
    return 0;
}

STATIC_OVL void
sense_trap(trap, x, y, src_cursed)
struct trap *trap;
xchar x, y;
int src_cursed;
{
    if (Hallucination || src_cursed) {
	struct obj obj;			/* fake object */
	if (trap) {
	    obj.ox = trap->tx;
	    obj.oy = trap->ty;
	} else {
	    obj.ox = x;
	    obj.oy = y;
	}
	obj.otyp = (src_cursed) ? GOLD_PIECE : random_object();
	obj.corpsenm = random_monster();	/* if otyp == CORPSE */
	map_object(&obj,1);
    } else if (trap && !trap->hiddentrap) {
	map_trap(trap,1);
	trap->tseen = 1;
    } else {
	struct trap temp_trap;		/* fake trap */
	temp_trap.tx = x;
	temp_trap.ty = y;
	temp_trap.ttyp = BEAR_TRAP;	/* some kind of trap */
	map_trap(&temp_trap,1);
    }

}

/* the detections are pulled out so they can	*/
/* also be used in the crystal ball routine	*/
/* returns 1 if nothing was detected		*/
/* returns 0 if something was detected		*/
int
trap_detect(sobj)
register struct obj *sobj;
/* sobj is null if crystal ball, *scroll if gold detection scroll */
{
    register struct trap *ttmp;
    register struct obj *obj;
    register int door;
    int uw = u.uinwater;
    boolean found = FALSE;
    int x, y;

    if (DetectionMethodsDontWork) {
	if (sobj) strange_feeling(sobj, "Huh.");
	return 1;
    }

    for (ttmp = ftrap; ttmp; ttmp = ttmp->ntrap) {
	if (ttmp->tx != u.ux || ttmp->ty != u.uy)
	    goto outtrapmap;
	else found = TRUE;
    }
    for (obj = fobj; obj; obj = obj->nobj) {
	if ((obj->otyp==LARGE_BOX || obj->otyp==LEAD_BOX || obj->otyp==TOP_BOX || obj->otyp==TREASURE_CHEST || obj->otyp==LOOT_CHEST || obj->otyp==CHEST || obj->otyp==NANO_CHEST || obj->otyp==LARGE_BOX_OF_DIGESTION || obj->otyp==CHEST_OF_HOLDING) && obj->otrapped) {
	    if (obj->ox != u.ux || obj->oy != u.uy)
		goto outtrapmap;
	    else found = TRUE;
	}
    }
    for (door = 0; door < doorindex; door++) {
	x = doors[door].x;
	y = doors[door].y;
	if (levl[x][y].doormask & D_TRAPPED) {
	    if (x != u.ux || y != u.uy)
		goto outtrapmap;
	    else found = TRUE;
	}
    }
    if (!found) {
	char buf[42];
	sprintf(buf, "Your %s stop itching.", makeplural(body_part(TOE)));
	strange_feeling(sobj,buf);
	return(1);
    }
    /* traps exist, but only under me - no separate display required */
    Your("%s itch.", makeplural(body_part(TOE)));
    return(0);
outtrapmap:
    cls();

	/* nerf by Amy - only detect 50% of all traps, because trap detection is very powerful if you think about it... */
    u.uinwater = 0;
    for (ttmp = ftrap; ttmp; ttmp = ttmp->ntrap)
	if (rn2(2) && (ttmp->trapdiff < rnd(150)) && (!isfriday || (ttmp->trapdiff < rnd(150))) && (ttmp->trapdiff < rnd(150)) ) sense_trap(ttmp, 0, 0, sobj && sobj->cursed);

    for (obj = fobj; obj; obj = obj->nobj)
	if ((obj->otyp==LARGE_BOX || obj->otyp==LEAD_BOX || obj->otyp==TOP_BOX || obj->otyp==TREASURE_CHEST || obj->otyp==LOOT_CHEST || obj->otyp==CHEST || obj->otyp==NANO_CHEST || obj->otyp==LARGE_BOX_OF_DIGESTION || obj->otyp==CHEST_OF_HOLDING) && obj->otrapped)
	if (rn2(2)) sense_trap((struct trap *)0, obj->ox, obj->oy, sobj && sobj->cursed);

    for (door = 0; door < doorindex; door++) {
	x = doors[door].x;
	y = doors[door].y;
	if (levl[x][y].doormask & D_TRAPPED)
	if (rn2(2)) sense_trap((struct trap *)0, x, y, sobj && sobj->cursed);
    }

    newsym(u.ux,u.uy);
    You_feel("%s.", sobj && sobj->cursed ? "very greedy" : "entrapped");
    display_nhwindow(WIN_MAP, TRUE);
    docrt();
    u.uinwater = uw;
    if (Underwater && !Swimming) under_water(2);
    if (u.uburied) under_ground(2);
    return(0);
}

/* weaker trap detection, from spell; currently only the entrapping spell uses this --Amy */
int
trap_detectX(sobj)
register struct obj *sobj;
/* sobj is null if crystal ball, *scroll if gold detection scroll */
{
    register struct trap *ttmp;
    register struct obj *obj;
    register int door;
    int uw = u.uinwater;
    boolean found = FALSE;
    int x, y;

    if (DetectionMethodsDontWork) {
	if (sobj) strange_feeling(sobj, "Huh.");
	return 1;
    }

    for (ttmp = ftrap; ttmp; ttmp = ttmp->ntrap) {
	if (ttmp->tx != u.ux || ttmp->ty != u.uy)
	    goto outtrapmap;
	else found = TRUE;
    }
    for (obj = fobj; obj; obj = obj->nobj) {
	if ((obj->otyp==LARGE_BOX || obj->otyp==LEAD_BOX || obj->otyp==TOP_BOX || obj->otyp==CHEST || obj->otyp==NANO_CHEST || obj->otyp==TREASURE_CHEST || obj->otyp==LOOT_CHEST || obj->otyp==LARGE_BOX_OF_DIGESTION || obj->otyp==CHEST_OF_HOLDING) && obj->otrapped) {
	    if (obj->ox != u.ux || obj->oy != u.uy)
		goto outtrapmap;
	    else found = TRUE;
	}
    }
    for (door = 0; door < doorindex; door++) {
	x = doors[door].x;
	y = doors[door].y;
	if (levl[x][y].doormask & D_TRAPPED) {
	    if (x != u.ux || y != u.uy)
		goto outtrapmap;
	    else found = TRUE;
	}
    }
    if (!found) {
	char buf[42];
	sprintf(buf, "Your %s stop itching.", makeplural(body_part(TOE)));
	strange_feeling(sobj,buf);
	return(1);
    }
    /* traps exist, but only under me - no separate display required */
    Your("%s itch.", makeplural(body_part(TOE)));
    return(0);
outtrapmap:
    cls();

	/* nerf by Amy - only detect 50% of all traps, because trap detection is very powerful if you think about it... */
    u.uinwater = 0;
    for (ttmp = ftrap; ttmp; ttmp = ttmp->ntrap)
	if (!rn2(4) && !(isok(ttmp->tx, ttmp->ty) && isok(u.ux, u.uy) && (dist2(u.ux, u.uy, ttmp->tx, ttmp->ty) > 401) ) && (ttmp->trapdiff < rnd(150)) && (!isfriday || (ttmp->trapdiff < rnd(150))) && (ttmp->trapdiff < rnd(150)) ) sense_trap(ttmp, 0, 0, sobj && sobj->cursed);

    for (obj = fobj; obj; obj = obj->nobj)
	if ((obj->otyp==LARGE_BOX || obj->otyp==LEAD_BOX || obj->otyp==TOP_BOX || obj->otyp==CHEST || obj->otyp==NANO_CHEST || obj->otyp==TREASURE_CHEST || obj->otyp==LOOT_CHEST || obj->otyp==LARGE_BOX_OF_DIGESTION || obj->otyp==CHEST_OF_HOLDING) && obj->otrapped)
	if (!rn2(4) && !(isok(obj->ox, obj->oy) && isok(u.ux, u.uy) && (dist2(u.ux, u.uy, obj->ox, obj->oy) > 401) ))
		sense_trap((struct trap *)0, obj->ox, obj->oy, sobj && sobj->cursed);

    for (door = 0; door < doorindex; door++) {
	x = doors[door].x;
	y = doors[door].y;
	if (levl[x][y].doormask & D_TRAPPED)
	if (!rn2(4) && !(isok(x, y) && isok(u.ux, u.uy) && (dist2(u.ux, u.uy, x, y) > 401) )) sense_trap((struct trap *)0, x, y, sobj && sobj->cursed);
    }

    newsym(u.ux,u.uy);
    You_feel("%s.", sobj && sobj->cursed ? "very greedy" : "entrapped");
    display_nhwindow(WIN_MAP, TRUE);
    docrt();
    u.uinwater = uw;
    if (Underwater && !Swimming) under_water(2);
    if (u.uburied) under_ground(2);
    return(0);
}

const char *
level_distance(where)
d_level *where;
{
    register schar ll = depth(&u.uz) - depth(where);
    register boolean indun = (u.uz.dnum == where->dnum);

    if (ll < 0) {
	if (ll < (-8 - rn2(3))) {
	    if (!indun)	return "far away";
	    else	return "far below";
	}
	else if (ll < -1) {
	    if (!indun)	return "away below you";
	    else	return "below you";
	}
	else
	    if (!indun)	return "in the distance";
	    else	return "just below";
    } else if (ll > 0) {
	if (ll > (8 + rn2(3))) {
	    if (!indun)	return "far away";
	    else	return "far above";
	}
	else if (ll > 1) {
	    if (!indun)	return "away above you";
	    else	return "above you";
	}
	else
	    if (!indun)	return "in the distance";
	    else	return "just above";
    } else
	    if (!indun)	return "in the distance";
	    else	return "near you";
}

static const struct {
    const char *what;
    d_level *where;
} level_detects[] = {
  { "Delphi", &oracle_level },
  { "Medusa's lair", &medusa_level },
  { "a castle", &stronghold_level },
  { "the Wizard of Yendor's tower", &wiz1_level },
};

boolean
use_crystal_ball(obj)
struct obj *obj;
{
    char ch;
    int oops;

    if (Blind) {
	pline("Too bad you can't see %s.", the(xname(obj)));
	return 0;
    }

    if (obj->oartifact == ART_ABSOLUTE_CLARITY) {
	oops = (obj->cursed && rn2(2));
    } else {
	oops = (rnd(20) > ACURR(A_INT) || obj->cursed);
    }

    if (oops && (obj->spe > 0)) {
	switch (rnd((obj->oartifact && rn2(100)) ? 4 : 5)) {
	case 1 : pline("%s too much to comprehend!", Tobjnam(obj, "are"));
	    break;
	case 2 : pline("%s you!", Tobjnam(obj, "confuse"));
	    make_confused(HConfusion + rnd(50),FALSE);
	    break;
	case 3 : if (!resists_blnd(&youmonst)) {
		pline("%s your vision!", Tobjnam(obj, "damage"));
		make_blinded(Blinded + rnd(50),FALSE);
		if (!Blind) Your("%s", vision_clears);
	    } else {
		pline("%s your vision.", Tobjnam(obj, "assault"));
		You("are unaffected!");
	    }
	    break;
	case 4 : pline("%s your mind!", Tobjnam(obj, "zap"));
	    (void) make_hallucinated(HHallucination + rnd(50),FALSE,0L);
	    break;
	case 5 : pline("%s!", Tobjnam(obj, "explode"));
	    useup(obj);
	    obj = 0;	/* it's gone */
	    losehp(rnd(30), "exploding crystal ball", KILLED_BY_AN);
	    return 1;
	    break;
	}
	if (obj) consume_obj_charge(obj, TRUE);
	return 0;
    }

    if (Hallucination) {
	if (!obj->spe) {
	    pline("All you see is funky %s haze.", hcolor((char *)0));
	} else {
	    switch(rnd(6)) {
	    case 1 : You("grok some groovy globs of incandescent lava.");
		break;
	    case 2 : pline("Whoa!  Psychedelic colors, %s!",
			   poly_gender() == 1 ? "babe" : "dude");
		break;
	    case 3 : pline_The("crystal pulses with sinister %s light!",
				hcolor((char *)0));
		break;
	    case 4 : You("see goldfish swimming above fluorescent rocks.");
		break;
	    case 5 : You("see tiny snowflakes spinning around a miniature farmhouse.");
		break;
	    default: pline("Oh wow... like a kaleidoscope!");
		break;
	    }
	    consume_obj_charge(obj, TRUE);
	}
	return 0;
    }

    /* read a single character */
    if (flags.verbose) You("may look for an object or monster symbol.");
    ch = yn_function("What do you look for?", (char *)0, '\0');
    /* Don't filter out ' ' here; it has a use */
    if ((ch != def_monsyms[S_GHOST]) && index(quitchars,ch)) { 
	if (flags.verbose) pline("%s", Never_mind);
	if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
		pline("Oh wait, actually I do mind...");
		badeffect();
	}
	return 0;
    }
    You("peer into %s...", the(xname(obj)));
    if (obj->oartifact == ART_ABSOLUTE_CLARITY) {
	nomul(-rnd(3), "gazing into a crystal ball", TRUE);
    } else {
	nomul(-rnd(10), "gazing into a crystal ball", TRUE);
    }
    nomovemsg = "";
    if (obj->spe <= 0) {
	pline_The("vision is unclear.");
	if (!PlayerCannotUseSkills && P_SKILL(P_DEVICES) >= P_SKILLED) obj->known = TRUE;
    } else {
	int class;
	int ret = 0;

	makeknown(CRYSTAL_BALL);
	consume_obj_charge(obj, TRUE);

	/* special case: accept ']' as synonym for mimic
	 * we have to do this before the def_char_to_objclass check
	 */
	if (ch == DEF_MIMIC_DEF) ch = DEF_MIMIC;

	if ((class = def_char_to_objclass(ch)) != MAXOCLASSES)
		ret = object_detect((struct obj *)0, class, FALSE);
	else if ((class = def_char_to_monclass(ch)) != MAXMCLASSES)
		ret = monster_detect((struct obj *)0, class);
	else if (iflags.bouldersym && (ch == iflags.bouldersym))
		ret = object_detect((struct obj *)0, ROCK_CLASS, FALSE);
	else switch(ch) {
		case '^':
		    ret = trap_detect((struct obj *)0);
		    break;
		default:
		    {
		    int i = rn2(SIZE(level_detects));
		    You("see %s, %s.",
			level_detects[i].what,
			level_distance(level_detects[i].where));
		    }
		    ret = 0;
		    break;
	}

	if (ret) {
	    if (!rn2(100))  /* make them nervous */
		You("see the Wizard of Yendor gazing out at you.");
	    else pline_The("vision is unclear.");
	}
    }
    return 0;
}

STATIC_OVL void
show_map_spot(x, y)
register int x, y;
{
    register struct rm *lev;

    if (Confusion && rn2(7)) return;

    if (isfriday && !rn2(10)) return;

    lev = &levl[x][y];

    lev->seenv = SVALL;

    /* Secret corridors are found, but not secret doors. */
    if (lev->typ == SCORR) {
	lev->typ = CORR;
	unblock_point(x,y);
    }

    /* if we don't remember an object or trap there, map it */
#ifdef DISPLAY_LAYERS
    if (!lev->mem_obj && !lev->mem_trap) {
#else
    if (lev->typ == ROOM ?
	    (glyph_is_cmap(lev->glyph) && !glyph_is_trap(lev->glyph) &&
		glyph_to_cmap(lev->glyph) != ROOM) :
	    (!glyph_is_object(lev->glyph) && !glyph_is_trap(lev->glyph))) {
#endif
	if (level.flags.hero_memory) {
	    magic_map_background(x,y,0);
	    newsym(x,y);			/* show it, if not blocked */
	} else {
	    magic_map_background(x,y,1);	/* display it */
	}
    }
}

STATIC_OVL void
show_map_spotX(x, y)
register int x, y;
{
    register struct rm *lev;

    if (rn2(10)) return;

    if (isfriday && !rn2(10)) return;

    lev = &levl[x][y];

    lev->seenv = SVALL;

    /* Secret corridors are found, but not secret doors. */
    if (lev->typ == SCORR) {
	lev->typ = CORR;
	unblock_point(x,y);
    }

    /* if we don't remember an object or trap there, map it */
#ifdef DISPLAY_LAYERS
    if (!lev->mem_obj && !lev->mem_trap) {
#else
    if (lev->typ == ROOM ?
	    (glyph_is_cmap(lev->glyph) && !glyph_is_trap(lev->glyph) &&
		glyph_to_cmap(lev->glyph) != ROOM) :
	    (!glyph_is_object(lev->glyph) && !glyph_is_trap(lev->glyph))) {
#endif
	if (level.flags.hero_memory) {
	    magic_map_background(x,y,0);
	    newsym(x,y);			/* show it, if not blocked */
	} else {
	    magic_map_background(x,y,1);	/* display it */
	}
    }
}

void
do_mapping()
{
    register int zx, zy;
    int uw = u.uinwater;

    if (DetectionMethodsDontWork) return;

    u.uinwater = 0;
    for (zx = 1; zx < COLNO; zx++)
	for (zy = 0; zy < ROWNO; zy++)
	    show_map_spot(zx, zy);
    exercise(A_WIS, TRUE);
    u.uinwater = uw;
    if (!level.flags.hero_memory || Underwater) {
	flush_screen(1);			/* flush temp screen */
	display_nhwindow(WIN_MAP, TRUE);	/* wait */
	docrt();
    }
}

void
do_mappingX() /* magic mapping spell and roadmap cloak - they don't reveal as much info --Amy */
{
    register int zx, zy;
    int uw = u.uinwater;

    if (DetectionMethodsDontWork) return;

    u.uinwater = 0;
    for (zx = 1; zx < COLNO; zx++)
	for (zy = 0; zy < ROWNO; zy++)
	    if (!(isok(zx, zy) && isok(u.ux, u.uy) && (dist2(u.ux, u.uy, zx, zy) > 626) ) ) show_map_spotX(zx, zy);
    exercise(A_WIS, TRUE);
    u.uinwater = uw;
    if (!level.flags.hero_memory || Underwater) {
	flush_screen(1);			/* flush temp screen */
	display_nhwindow(WIN_MAP, TRUE);	/* wait */
	docrt();
    }
}

void
do_mappingY()
{
    register int zx, zy;
    int uw = u.uinwater;

    if (DetectionMethodsDontWork) return;

    u.uinwater = 0;
    for (zx = 1; zx < COLNO; zx++)
	for (zy = 0; zy < ROWNO; zy++)
	    if (rn2(10)) show_map_spot(zx, zy);
    exercise(A_WIS, TRUE);
    u.uinwater = uw;
    if (!level.flags.hero_memory || Underwater) {
	flush_screen(1);			/* flush temp screen */
	display_nhwindow(WIN_MAP, TRUE);	/* wait */
	docrt();
    }
}

void
do_mappingZ()
{
    register int zx, zy;
    int uw = u.uinwater;

    if (DetectionMethodsDontWork) return;

    u.uinwater = 0;
    for (zx = 1; zx < COLNO; zx++)
	for (zy = 0; zy < ROWNO; zy++)
	    if (rn2(2)) show_map_spot(zx, zy);
    exercise(A_WIS, TRUE);
    u.uinwater = uw;
    if (!level.flags.hero_memory || Underwater) {
	flush_screen(1);			/* flush temp screen */
	display_nhwindow(WIN_MAP, TRUE);	/* wait */
	docrt();
    }
}

void
do_vicinity_map()
{

    if (DetectionMethodsDontWork) return;
    if (level.flags.nommap) {
	pline("Something blocks your clairvoyance!");
	return;
    }

    register int zx, zy;
    int lo_y = (u.uy-6 < 0 ? 0 : u.uy-6),
	hi_y = (u.uy+7 > ROWNO ? ROWNO : u.uy+7),
	lo_x = (u.ux-6 < 1 ? 1 : u.ux-6),	/* avoid column 0 */
	hi_x = (u.ux+7 > COLNO ? COLNO : u.ux+7);

    for (zx = lo_x; zx < hi_x; zx++)
	for (zy = lo_y; zy < hi_y; zy++)
	    show_map_spot(zx, zy);

    if (!level.flags.hero_memory || Underwater) {
	flush_screen(1);			/* flush temp screen */
	display_nhwindow(WIN_MAP, TRUE);	/* wait */
	docrt();
    }
}

void
do_vicinity_map_huge()
{

    if (DetectionMethodsDontWork) return;
    if (level.flags.nommap) {
	pline("Something blocks your clairvoyance!");
	return;
    }

    register int zx, zy;
    int lo_y = (u.uy-12 < 0 ? 0 : u.uy-12),
	hi_y = (u.uy+13 > ROWNO ? ROWNO : u.uy+13),
	lo_x = (u.ux-12 < 1 ? 1 : u.ux-12),	/* avoid column 0 */
	hi_x = (u.ux+13 > COLNO ? COLNO : u.ux+13);

    for (zx = lo_x; zx < hi_x; zx++)
	for (zy = lo_y; zy < hi_y; zy++)
	    show_map_spot(zx, zy);

    if (!level.flags.hero_memory || Underwater) {
	flush_screen(1);			/* flush temp screen */
	display_nhwindow(WIN_MAP, TRUE);	/* wait */
	docrt();
    }
}

void
do_vicinity_mapX()
{

    if (DetectionMethodsDontWork) return;
    if (level.flags.nommap) {
	pline("Something blocks your clairvoyance!");
	return;
    }

    register int zx, zy;
    int lo_y = (u.uy-4 < 0 ? 0 : u.uy-4),
	hi_y = (u.uy+5 > ROWNO ? ROWNO : u.uy+5),
	lo_x = (u.ux-4 < 1 ? 1 : u.ux-4),	/* avoid column 0 */
	hi_x = (u.ux+5 > COLNO ? COLNO : u.ux+5);

    for (zx = lo_x; zx < hi_x; zx++)
	for (zy = lo_y; zy < hi_y; zy++)
	    if (!rn2(5)) show_map_spot(zx, zy);

    if (!level.flags.hero_memory || Underwater) {
	flush_screen(1);			/* flush temp screen */
	display_nhwindow(WIN_MAP, TRUE);	/* wait */
	docrt();
    }
}

/* convert a secret door into a normal door */
void
cvt_sdoor_to_door(lev)
struct rm *lev;
{
	int newmask = lev->doormask & ~WM_MASK;

#ifdef REINCARNATION
	if (Is_rogue_level(&u.uz))
	    /* rogue didn't have doors, only doorways */
	    newmask = D_NODOOR;
	else
#endif
	    /* newly exposed door is closed */
	    if (!(newmask & D_LOCKED)) newmask |= D_CLOSED;

	lev->typ = DOOR;
	lev->doormask = newmask;
}

STATIC_PTR void
findone(zx,zy,num)
int zx,zy;
void * num;
{
	register struct trap *ttmp;
	register struct monst *mtmp;

	int trapfindchance = 150;

	if (!(PlayerCannotUseSkills)) {
		switch (P_SKILL(P_SEARCHING)) {
			default: break;
			case P_BASIC: trapfindchance = 160; break;
			case P_SKILLED: trapfindchance = 170; break;
			case P_EXPERT: trapfindchance = 180; break;
			case P_MASTER: trapfindchance = 200; break;
			case P_GRAND_MASTER: trapfindchance = 225; break;
			case P_SUPREME_MASTER: trapfindchance = 250; break;
		}
	}

	trapfindchance += (boost_power_value() * 3);

	/* there should be at least some stat that makes it more likely to find a trap! --Amy */
	if (ACURR(A_WIS) > 14) trapfindchance += (ACURR(A_WIS) - 14);

	if(levl[zx][zy].typ == SDOOR) {
		cvt_sdoor_to_door(&levl[zx][zy]);	/* .typ = DOOR */
		You("find a secret door!");
		u.cnd_searchsecretcount++;
		use_skill(P_SEARCHING,1);
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		magic_map_background(zx, zy, 0);
		newsym(zx, zy);
		(*(int*)num)++;
	} else if(levl[zx][zy].typ == SCORR) {
		levl[zx][zy].typ = CORR;
		unblock_point(zx,zy);
		You("find a secret passage!");
		u.cnd_searchsecretcount++;
		use_skill(P_SEARCHING,1);
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		magic_map_background(zx, zy, 0);
		newsym(zx, zy);
		(*(int*)num)++;
	} else if ((ttmp = t_at(zx, zy)) != 0) {
		if(!ttmp->tseen && !ttmp->hiddentrap && (ttmp->trapdiff < rnd(trapfindchance)) && (!isfriday || (ttmp->trapdiff < rnd(trapfindchance))) && (ttmp->trapdiff < rnd(trapfindchance)) && ttmp->ttyp != STATUE_TRAP && ttmp->ttyp != SATATUE_TRAP) {

			if (!ttmp->tdetected && !rn2(3)) {
				use_skill(P_SEARCHING,1);
				ttmp->tdetected = TRUE;
			}

			ttmp->tseen = 1;
			newsym(zx,zy);
			(*(int*)num)++;
		}
	} else if ((mtmp = m_at(zx, zy)) != 0) {
		if(mtmp->m_ap_type) {
			seemimic(mtmp);
			(*(int*)num)++;
		}
		if (mtmp->mundetected &&
		    (is_hider(mtmp->data) || mtmp->egotype_hide || mtmp->egotype_mimic || mtmp->data->mlet == S_EEL)) {
			mtmp->mundetected = 0;
			newsym(zx, zy);
			(*(int*)num)++;
		}
		if (!canspotmon(mtmp) && !(monstersoundtype(mtmp) == MS_DEEPSTATE) && !(mtmp->egotype_deepstatemember) &&
				    !memory_is_invisible(zx, zy))
			map_invisible(zx, zy);
	} else if (memory_is_invisible(zx, zy)) {
		unmap_object(zx, zy);
		newsym(zx, zy);
		(*(int*)num)++;
	}
}

STATIC_PTR void
findoneX(zx,zy,num)
int zx,zy;
void * num;
{
	register struct trap *ttmp;
	register struct monst *mtmp;

	int trapfindchance = 150;

	if (!(PlayerCannotUseSkills)) {
		switch (P_SKILL(P_SEARCHING)) {
			default: break;
			case P_BASIC: trapfindchance = 160; break;
			case P_SKILLED: trapfindchance = 170; break;
			case P_EXPERT: trapfindchance = 180; break;
			case P_MASTER: trapfindchance = 200; break;
			case P_GRAND_MASTER: trapfindchance = 225; break;
			case P_SUPREME_MASTER: trapfindchance = 250; break;
		}
	}

	trapfindchance += (boost_power_value() * 3);

	/* there should be at least some stat that makes it more likely to find a trap! --Amy */
	if (ACURR(A_WIS) > 14) trapfindchance += (ACURR(A_WIS) - 14);

	if(!rn2(3) && levl[zx][zy].typ == SDOOR) {
		cvt_sdoor_to_door(&levl[zx][zy]);	/* .typ = DOOR */
		You("find a secret door!");
		u.cnd_searchsecretcount++;
		use_skill(P_SEARCHING,1);
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		magic_map_background(zx, zy, 0);
		newsym(zx, zy);
		(*(int*)num)++;
	} else if(!rn2(3) && levl[zx][zy].typ == SCORR) {
		levl[zx][zy].typ = CORR;
		unblock_point(zx,zy);
		You("find a secret passage!");
		u.cnd_searchsecretcount++;
		use_skill(P_SEARCHING,1);
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		magic_map_background(zx, zy, 0);
		newsym(zx, zy);
		(*(int*)num)++;
	} else if (!rn2(3) && (ttmp = t_at(zx, zy)) != 0) {
		if(!ttmp->tseen && !ttmp->hiddentrap && (ttmp->trapdiff < rnd(trapfindchance)) && (!isfriday || (ttmp->trapdiff < rnd(trapfindchance))) && (ttmp->trapdiff < rnd(trapfindchance)) && ttmp->ttyp != STATUE_TRAP && ttmp->ttyp != SATATUE_TRAP) {

			if (!ttmp->tdetected && !rn2(3)) {
				use_skill(P_SEARCHING,1);
				ttmp->tdetected = TRUE;
			}

			ttmp->tseen = 1;
			newsym(zx,zy);
			(*(int*)num)++;
		}
	} else if (!rn2(3) && (mtmp = m_at(zx, zy)) != 0) {
		if(mtmp->m_ap_type) {
			seemimic(mtmp);
			(*(int*)num)++;
		}
		if (mtmp->mundetected &&
		    (is_hider(mtmp->data) || mtmp->egotype_hide || mtmp->egotype_mimic || mtmp->data->mlet == S_EEL)) {
			mtmp->mundetected = 0;
			newsym(zx, zy);
			(*(int*)num)++;
		}
		if (!canspotmon(mtmp) && !(monstersoundtype(mtmp) == MS_DEEPSTATE) && !(mtmp->egotype_deepstatemember) &&
				    !memory_is_invisible(zx, zy))
			map_invisible(zx, zy);
	} else if (memory_is_invisible(zx, zy)) {
		unmap_object(zx, zy);
		newsym(zx, zy);
		(*(int*)num)++;
	}
}

STATIC_PTR void
openone(zx,zy,num)
int zx,zy;
void * num;
{
	register struct trap *ttmp;
	register struct obj *otmp;

	if(OBJ_AT(zx, zy)) {
		for(otmp = level.objects[zx][zy];
				otmp; otmp = otmp->nexthere) {
		    if(Is_box(otmp) && otmp->olocked) {
			otmp->olocked = 0;
			(*(int*)num)++;
		    }
		}
		/* let it fall to the next cases. could be on trap. */
	}
	if(levl[zx][zy].typ == SDOOR || (levl[zx][zy].typ == DOOR &&
		      (levl[zx][zy].doormask & (D_CLOSED|D_LOCKED)))) {
		if(levl[zx][zy].typ == SDOOR)
		    cvt_sdoor_to_door(&levl[zx][zy]);	/* .typ = DOOR */
		if(levl[zx][zy].doormask & D_TRAPPED) {
		    if(distu(zx, zy) < 3) b_trapped("door", 0);
		    else Norep("You %s an explosion!",
				cansee(zx, zy) ? "see" :
				   (flags.soundok ? "hear" :
						"feel the shock of"));
		    wake_nearto(zx, zy, 11*11);
		    levl[zx][zy].doormask = D_NODOOR;
		} else
		    levl[zx][zy].doormask = D_ISOPEN;
		unblock_point(zx, zy);
		newsym(zx, zy);
		(*(int*)num)++;
	} else if(levl[zx][zy].typ == SCORR) {
		levl[zx][zy].typ = CORR;
		unblock_point(zx, zy);
		newsym(zx, zy);
		(*(int*)num)++;
	} else if ((ttmp = t_at(zx, zy)) != 0) {
		if (!ttmp->tseen && !ttmp->hiddentrap && (ttmp->trapdiff < rnd(150)) && (!isfriday || (ttmp->trapdiff < rnd(150))) && (ttmp->trapdiff < rnd(150)) && ttmp->ttyp != STATUE_TRAP && ttmp->ttyp != SATATUE_TRAP) {
		    ttmp->tseen = 1;
		    newsym(zx,zy);
		    (*(int*)num)++;
		}
	} else if (find_drawbridge(&zx, &zy)) {
		/* make sure it isn't an open drawbridge */
		open_drawbridge(zx, zy);
		(*(int*)num)++;
	}
}

int
findit()	/* returns number of things found */
{
	int num = 0;

	if(u.uswallow) return(0);
	do_clear_area(u.ux, u.uy, BOLT_LIM, findone, (void *) &num);
	return(num);
}

int
finditX()	/* returns number of things found */
{
	int num = 0;

	if(u.uswallow) return(0);
	do_clear_area(u.ux, u.uy, 4 + rn2(2), findoneX, (void *) &num);
	return(num);
}

int
openit()	/* returns number of things found and opened */
{
	int num = 0;

	if(u.uswallow) {
		if (is_animal(u.ustuck->data)) {
			if (Blind) pline("Its mouth opens!");
			else pline("%s opens its mouth!", Monnam(u.ustuck));
		}
		expels(u.ustuck, u.ustuck->data, TRUE);
		return(-1);
	}

	do_clear_area(u.ux, u.uy, BOLT_LIM, openone, (void *) &num);
	return(num);
}

void
find_trap(trap)
struct trap *trap;
{
    int tt = what_trap(trap->ttyp);
    boolean cleared = FALSE;

    if (trap->hiddentrap) return;

    trap->tseen = 1;
    exercise(A_WIS, TRUE);
/*    if (Blind)*/
	feel_location(trap->tx, trap->ty);
/*    else*/
	if (!Blind) newsym(trap->tx, trap->ty);

#ifdef DISPLAY_LAYERS
    if (levl[trap->tx][trap->ty].mem_obj ||
	    memory_is_invisible(trap->tx, trap->ty)) {
#else
    if (levl[trap->tx][trap->ty].glyph != trap_to_glyph(trap)) {
#endif
    	/* There's too much clutter to see your find otherwise */
	cls();
	map_trap(trap, 1);
	display_self();
	cleared = TRUE;
    }

    You("find %s.", an(defsyms[trap_to_defsym(tt)].explanation));
	u.cnd_searchtrapcount++;
	if (!trap->tdetected) {
		mightbooststat(A_WIS);
		use_skill(P_SEARCHING, rnd(3));
		/* allow the skill to train faster if you find a nasty trap --Amy */
		if (is_nasty_trap(trap->ttyp)) use_skill(P_SEARCHING, rnd(10));
		trap->tdetected = TRUE;
	}

    if (cleared) {
	display_nhwindow(WIN_MAP, TRUE);	/* wait */
	docrt();
    }
}

int
dosearch0(aflag)
register int aflag;
{
#ifdef GCC_BUG
/* some versions of gcc seriously muck up nested loops. if you get strange
   crashes while searching in a version compiled with gcc, try putting
   #define GCC_BUG in *conf.h (or adding -DGCC_BUG to CFLAGS in the
   makefile).
 */
	volatile xchar x, y;
#else
	register xchar x, y;
#endif
	register struct trap *trap;
	register struct monst *mtmp;

	if (AntisearchEffect || u.uprops[ANTISEARCH_EFFECT].extrinsic || have_unfindablestone() || autismweaponcheck(ART_HER_UNREACHABLE_BROOK) ) return(1);

	int fundxtrachange = 10;
	if (!(PlayerCannotUseSkills)) {
		switch (P_SKILL(P_SEARCHING)) {
			default: break;
			case P_BASIC: fundxtrachange = 8; break;
			case P_SKILLED: fundxtrachange = 6; break;
			case P_EXPERT: fundxtrachange = 5; break;
			case P_MASTER: fundxtrachange = 4; break;
			case P_GRAND_MASTER: fundxtrachange = 3; break;
			case P_SUPREME_MASTER: fundxtrachange = 2; break;
		}
	}

	int trapdiffbonus = 0;
	if (!(PlayerCannotUseSkills)) {
		switch (P_SKILL(P_SEARCHING)) {
			default: break;
			case P_BASIC: trapdiffbonus = rnd(10); break;
			case P_SKILLED: trapdiffbonus = rnd(20); break;
			case P_EXPERT: trapdiffbonus = rnd(35); break;
			case P_MASTER: trapdiffbonus = rnd(50); break;
			case P_GRAND_MASTER: trapdiffbonus = rnd(75); break;
			case P_SUPREME_MASTER: trapdiffbonus = rnd(100); break;
		}
	}
	if (ublindf && ublindf->otyp == SHIELD_PATE_GLASSES) trapdiffbonus += rnd(50);

	if(u.uswallow) {
		if (!aflag)
			pline(FunnyHallu ? "There must be some door here, allowing you to get out..." : "What are you looking for?  The exit?");
	} else {
	    int fund = (uwep && uwep->oartifact &&
		    spec_ability(uwep, SPFX_SEARCH)) ?
		    uwep->spe : 0;
	    if (ublindf && ublindf->otyp == LENSES && !Blind)
		    fund += 2; /* JDS: lenses help searching */
	    if (ublindf && ublindf->otyp == RADIOGLASSES && !Blind)
		    fund += 2;
	    if (ublindf && ublindf->otyp == SHIELD_PATE_GLASSES && !Blind)
		    fund += 2;
	    if (ublindf && ublindf->otyp == BOSS_VISOR && !Blind)
		    fund += 2;
	    if (uarmh && uarmh->otyp == HELM_OF_DISCOVERY && !Blind)
		    fund += 1; /* Amy: helm of discovery also helps a bit */
	    if (fund > 5) fund = 5;
	    for(x = u.ux-1; x < u.ux+2; x++)
	      for(y = u.uy-1; y < u.uy+2; y++) {
		if(!isok(x,y)) continue;
		if(x != u.ux || y != u.uy) {
		    if (/*Blind &&*/ (!aflag || !rn2(fundxtrachange) || !rn2(fundxtrachange)) ) feel_location(x,y);
		    if(levl[x][y].typ == SDOOR) {
			if(rnl(7-fund) && rn2(fundxtrachange) && (rn2(fundxtrachange) || !rn2(2)) ) continue; /* better chance --Amy */
			cvt_sdoor_to_door(&levl[x][y]);	/* .typ = DOOR */
			You("find a secret door!");
			u.cnd_searchsecretcount++;
			use_skill(P_SEARCHING,1);
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			exercise(A_WIS, TRUE);
			nomul(0, 0, FALSE);
			if (/*Blind && */(!aflag || !rn2(fundxtrachange) || !rn2(fundxtrachange)) )
			    feel_location(x,y);	/* make sure it shows up */
			/*else*/
			if (!Blind) newsym(x,y);
		    } else if(levl[x][y].typ == SCORR) {
			if(rnl(7-fund) && rn2(fundxtrachange) && (rn2(fundxtrachange) || !rn2(2)) ) continue; /* better chance --Amy */
			levl[x][y].typ = CORR;
			unblock_point(x,y);	/* vision */
			You("find a secret passage!");
			u.cnd_searchsecretcount++;
			use_skill(P_SEARCHING,1);
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			exercise(A_WIS, TRUE);
			nomul(0, 0, FALSE);
			newsym(x,y);
		    } else {
		/* Be careful not to find anything in an SCORR or SDOOR */
			if((mtmp = m_at(x, y)) && (!aflag || !rn2(4)) ) {
			    if(mtmp->m_ap_type) {
				seemimic(mtmp);
		find:		exercise(A_WIS, TRUE);
				if (!canspotmon(mtmp)) {
				    if (memory_is_invisible(x, y)) {
					/* found invisible monster in a square
					 * which already has an 'I' in it.
					 * Logically, this should still take
					 * time and lead to a return(1), but if
					 * we did that the player would keep
					 * finding the same monster every turn.
					 */
					continue;
				    } else {
					You_feel("an unseen monster!");
					if (!(monstersoundtype(mtmp) == MS_DEEPSTATE) && !(mtmp->egotype_deepstatemember)) map_invisible(x, y);
				    }
				} else if (!sensemon(mtmp))
				    You("find %s.", a_monnam(mtmp));

				/* No idea why finding a monster stops searching the other squares. Bullshit, I say! --Amy */
				if (issoviet) {
					if (!rn2(20)) pline("Komanda poiska neozhidanno ostanovilas' iz-za oshibki v ispolnyayemom fayle sovetskogo l'da. Potomu chto v Sovetskoy Rossii vy nikogda ne smozhete nayti material, dazhe yesli on nakhoditsya ryadom s vami, kha-kha.");
					return(1);
				}
				else continue;
			    }
			    if(!canspotmon(mtmp)) {
				if (mtmp->mundetected &&
				   (is_hider(mtmp->data) || mtmp->egotype_hide || mtmp->egotype_mimic || mtmp->data->mlet == S_EEL))
					mtmp->mundetected = 0;
				newsym(x,y);
				goto find;
			    }
			}

			/* see if an invisible monster has moved--if Blind,
			 * feel_location() already did it
			 */
			if ( (!aflag || !rn2(4)) && !mtmp && !Blind &&
				    memory_is_invisible(x, y)) {
			    unmap_object(x,y);
			    newsym(x,y);
			}

			/* finding traps is much too hard. Let's increase the chance. --Amy */
			if ((trap = t_at(x,y)) && !trap->tseen && !trap->hiddentrap && (trap->trapdiff < rn2(100 + trapdiffbonus) ) && (!isfriday || (trap->trapdiff < rn2(100 + trapdiffbonus)) ) && (trap->trapdiff < rn2(100 + trapdiffbonus + trapdiffbonus) ) && (!rnl(8-fund) || !rn2(fundxtrachange) || (!rn2(fundxtrachange) && !rn2(2)) ) ) {
			    nomul(0, 0, FALSE);

			    if (trap->ttyp == STATUE_TRAP || trap->ttyp == SATATUE_TRAP) {
 				mtmp = activate_statue_trap(trap, x, y, FALSE);
 				if (mtmp != (struct monst *)0) {
				    u.cnd_searchtrapcount++;
				    exercise(A_WIS, TRUE);
				    mightbooststat(A_WIS);
				    use_skill(P_SEARCHING,rnd(3)); /* you found a trap, so the skill should train --Amy */
				}

				/* bah, so they made an extra bullshit return for statue traps... --Amy */
				if (issoviet) {
					if (!rn2(20)) pline("Komanda poiska neozhidanno ostanovilas' iz-za oshibki v ispolnyayemom fayle sovetskogo l'da. Potomu chto v Sovetskoy Rossii vy nikogda ne smozhete nayti material, dazhe yesli on nakhoditsya ryadom s vami, kha-kha.");
					return(1);
				}
			    } else {
				find_trap(trap);
			    }
			}
		    }
		}
	    }
	}
	return(1);
}

/* Pre-map the sokoban levels */
void
sokoban_detect()
{
	register int x, y, randa, randb, randc; /* randomly hide some stuff from view because I'm mean :D --Amy */
	register struct trap *ttmp;
	register struct obj *obj;

	randa = rn2(isfriday ? 8 : 20);
	randb = rn2(isfriday ? 4 : 10);
	randc = rn2(isfriday ? 4 : 10);

	/* if you're such a ch3at0r who intentionally picks races that also cause other level types to be generated,
	 * and actually think you'd still fully benefit from levels all being pre-mapped, tough luck :-P --Amy */
	int badluckor = 0;
	if (ismazewalker) badluckor++;
	if (isspecialist) badluckor++;

	if (badluckor == 1) {
		if (!rn2(2)) randa = FALSE;
		if (!rn2(2)) randb = FALSE;
		if (!rn2(2)) randc = FALSE;
	}
	if (badluckor == 2) {
		if (rn2(3)) randa = FALSE;
		if (rn2(3)) randb = FALSE;
		if (rn2(3)) randc = FALSE;
	}

	/* slight downside to sokosolver: I mean come on, all the other special levels that aren't sokoban levels also have a chance of being mapped,
	 * and all just because the portion of regular "random" levels becomes soko... so you'll have slightly lower odds of levels being mapped. */
	if (issokosolver) {
		if (!rn2(20)) randa = FALSE;
		if (!rn2(20)) randb = FALSE;
		if (!rn2(20)) randc = FALSE;
	}

	/* Map the background and boulders */
	for (x = 1; x < COLNO; x++)
	    for (y = 0; y < ROWNO; y++) {

		if (randa) {
	    	levl[x][y].seenv = SVALL;
	    	levl[x][y].waslit = TRUE;
	    	map_background(x, y, 1);
		}

	    	for (obj = level.objects[x][y]; obj; obj = obj->nexthere)
	    	    if ((obj->otyp == BOULDER) && randb)
	    	    	map_object(obj, 1);
	}

	/* Map the traps */
	for (ttmp = ftrap; ttmp; ttmp = ttmp->ntrap) {

			/* but only holes and pits --Amy */
			if ((ttmp->ttyp == HOLE || ttmp->ttyp == PIT) && randc && !ttmp->hiddentrap) {
		    ttmp->tseen = 1;
		    map_trap(ttmp, 1);
		}
	}
}


int
dosearch()
{
	if (uwep && uwep->oartifact == ART_VILEA_S_SECRET && !rn2(20)) {

		register struct monst *mtmp;

		for(mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		    if (DEADMONSTER(mtmp)) continue;
		    if (distu(mtmp->mx,mtmp->my) > 50) continue;
		    if(cansee(mtmp->mx,mtmp->my)) {
			if (!resist(mtmp, SCROLL_CLASS, 0, NOTELL))
				monflee(mtmp, rnd(10), FALSE, FALSE, FALSE);
		    }
		}

	}

	if (uarmh && uarmh->oartifact == ART_TEN_MINUTES_COLDER && !rn2(2)) {
		if (Upolyd) {
			u.mh++;
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		} else {
			u.uhp++;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		}
		flags.botl = TRUE;
	}

	if ((TarmuStrokingNora || u.uprops[TARMU_STROKING_NORA].extrinsic || (uarmh && uarmh->oartifact == ART_STROKING_COMBAT) || have_tarmustrokingnorastone()) && u.tarmustrokingturn < 1) {
		u.tarmustrokingturn = rnd(100);

		int tryct = 0;
		int x, y;
		boolean canbeinawall = FALSE;
		if (!rn2(Passes_walls ? 5 : 25)) canbeinawall = TRUE;

		for (tryct = 0; tryct < 2000; tryct++) {
			x = rn1(COLNO-3,2);
			y = rn2(ROWNO);

			if (isok(x, y) && ((levl[x][y].typ > DBWALL) || canbeinawall) && !(t_at(x, y)) ) {
				(void) maketrap(x, y, rndtrap(), 100, FALSE);
				break;
				}
		}

	}

	return(dosearch0(0));
}

/*detect.c*/
