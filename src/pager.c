/*	SCCS Id: @(#)pager.c	3.4	2003/08/13	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* This file contains the command routines dowhatis() and dohelp() and */
/* a few other help related facilities */

#include "hack.h"
#include "dlb.h"

STATIC_DCL boolean FDECL(is_swallow_sym, (int));
STATIC_DCL int FDECL(append_str, (char *, const char *));
STATIC_DCL struct permonst * FDECL(lookat, (int, int, char *, char *));
STATIC_DCL void FDECL(checkfile,
		      (char *,struct permonst *,BOOLEAN_P,BOOLEAN_P));
STATIC_DCL int FDECL(do_look, (BOOLEAN_P));
STATIC_DCL boolean FDECL(help_menu, (int *));
#ifdef PORT_HELP
extern void NDECL(port_help);
#endif
#ifdef EXTENDED_INFO
STATIC_DCL char * FDECL(get_description_of_monster_type, (struct permonst *, char *));
STATIC_DCL char * FDECL(get_generation_description_of_monster_type, (struct permonst *, char *));
STATIC_DCL char * FDECL(get_resistance_description_of_monster_type, (struct permonst *, char *));
STATIC_DCL char * FDECL(get_flag_description_of_monster_type, (struct permonst *, char *));
STATIC_DCL char * FDECL(get_speed_description_of_monster_type, (struct permonst *, char *));
STATIC_DCL int FDECL(generate_list_of_resistances, (char *, /*uchar*/unsigned long));
STATIC_DCL void FDECL(append_newline_to_pline_string, (char * buf));
#endif

extern const int monstr[];

/* Returns "true" for characters that could represent a monster's stomach. */
STATIC_OVL boolean
is_swallow_sym(c)
int c;
{
    int i;
    for (i = S_sw_tl; i <= S_sw_br; i++)
	if ((int)showsyms[i] == c) return TRUE;
    return FALSE;
}

/*
 * Append new_str to the end of buf if new_str doesn't already exist as
 * a substring of buf.  Return 1 if the string was appended, 0 otherwise.
 * It is expected that buf is of size BUFSZ.
 */
STATIC_OVL int
append_str(buf, new_str)
    char *buf;
    const char *new_str;
{
    int space_left;	/* space remaining in buf */

    if (strstri(buf, new_str)) return 0;

    space_left = BUFSZ - strlen(buf) - 1;
    (void) strncat(buf, " or ", space_left);
    (void) strncat(buf, new_str, space_left - 4);
    return 1;
}

/*
 * Return the name of the glyph found at (x,y).
 * If not hallucinating and the glyph is a monster, also monster data.
 */
STATIC_OVL struct permonst *
lookat(x, y, buf, monbuf)
    int x, y;
    char *buf, *monbuf;
{
    register struct monst *mtmp = (struct monst *) 0;
    struct permonst *pm = (struct permonst *) 0;
    int glyph;

    buf[0] = monbuf[0] = 0;
    glyph = glyph_at(x,y);
    if (u.ux == x && u.uy == y && senseself()) {
	char race[QBUFSZ];

	/* if not polymorphed, show both the role and the race */
	race[0] = 0;
	if (!Upolyd) {
	    Sprintf(race, "%s ", urace.adj);
	}

	Sprintf(buf, "%s%s%s called %s",
		Invis ? "invisible " : "",
		race,
		(!missingnoprotect || !Upolyd) ? mons[u.umonnum].mname : "undefined creature",
		plname);
	/* file lookup can't distinguish between "gnomish wizard" monster
	   and correspondingly named player character, always picking the
	   former; force it to find the general "wizard" entry instead */

	/* show the damn information for your polymorph form! --Amy */
	pm = (Upolyd ? &mons[u.umonnum] : rn2(2) ? &mons[urole.malenum] : &mons[urace.malenum] );

	if (Role_if(PM_WIZARD) && Race_if(PM_GNOME) && !Upolyd)
	    pm = &mons[PM_WIZARD];

#if 0
	char race[QBUFSZ], role[QBUFSZ];

	/* if not polymorphed, show both the role and the race */
	role[0] = 0;
	race[0] = 0;
	
	if (!Upolyd) {
	    Sprintf(race, "%s ", urace.adj);
	    Sprintf(role, "%s ", urole.name);
	} else Sprintf(race, "%s ", (!missingnoprotect || !Upolyd) ? mons[u.umonnum].mname : "undefined creature");

	Sprintf(buf, "%s%s%s called %s",
		Invis ? "invisible " : "",
		race,
		role,
		plname);
#endif


#ifdef STEED
	if (u.usteed) {
	    char steedbuf[BUFSZ];

	    Sprintf(steedbuf, ", mounted on %s", y_monnam(u.usteed));
	    /* assert((sizeof buf >= strlen(buf)+strlen(steedbuf)+1); */
	    Strcat(buf, steedbuf);
	}
#endif
	/* When you see yourself normally, no explanation is appended
	   (even if you could also see yourself via other means).
	   Sensing self while blind or swallowed is treated as if it
	   were by normal vision (cf canseeself()). */
	if ((Invisible || u.uundetected) && !Blind && !u.uswallow) {
	    unsigned how = 0;

	    if (Infravision)	 how |= 1;
	    if (Unblind_telepat) how |= 2;
	    if (Detect_monsters) how |= 4;

	    if (how)
		Sprintf(eos(buf), " [seen: %s%s%s%s%s]",
			(how & 1) ? "infravision" : "",
			/* add comma if telep and infrav */
			((how & 3) > 2) ? ", " : "",
			(how & 2) ? "telepathy" : "",
			/* add comma if detect and (infrav or telep or both) */
			((how & 7) > 4) ? ", " : "",
			(how & 4) ? "monster detection" : "");
	}
    } else if (u.uswallow) {
	/* all locations when swallowed other than the hero are the monster */
	Sprintf(buf, "interior of %s",
				    Blind ? "a monster" : a_monnam(u.ustuck));
	pm = u.ustuck->data;
    } else if (glyph_is_monster(glyph)) {
	bhitpos.x = x;
	bhitpos.y = y;
	mtmp = m_at(x,y);
	if (mtmp != (struct monst *) 0) {
	    char *name, monnambuf[BUFSZ];
	    boolean accurate = !Hallucination;

		if (FarlookProblem || (uarms && uarms->oartifact == ART_REAL_PSYCHOS_WEAR_PURPLE) || u.uprops[FARLOOK_BUG].extrinsic || have_farlookstone()) wakeup(mtmp);

	    if ( (mtmp->data == &mons[PM_COYOTE] || mtmp->data == &mons[PM_STALKING_COYOTE]) && accurate)
		name = coyotename(mtmp, monnambuf);
	    else
		name = distant_monnam(mtmp, ARTICLE_NONE, monnambuf);

	    pm = mtmp->data;
	    Sprintf(buf, "%s%s%s",
		    (mtmp->mx != x || mtmp->my != y) ?
			((mtmp->isshk && accurate)
				? "tail of " : "tail of a ") : "",
		    (mtmp->mtame && accurate) ? "tame " :
		    (mtmp->mpeaceful && accurate) ? "peaceful " : "",
		    name);
	    if (Hallu_party && Hallucination)
		    Sprintf(eos(buf), ", real color: %s", (mtmp->data->mcolor == CLR_BLACK) ? "black" : (mtmp->data->mcolor == CLR_RED) ? "red" : (mtmp->data->mcolor == CLR_GREEN) ? "green" : (mtmp->data->mcolor == CLR_BROWN) ? "brown" : (mtmp->data->mcolor == CLR_BLUE) ? "blue" : (mtmp->data->mcolor == CLR_MAGENTA) ? "magenta" : (mtmp->data->mcolor == CLR_CYAN) ? "cyan" : (mtmp->data->mcolor == CLR_GRAY) ? "gray" : (mtmp->data->mcolor == NO_COLOR) ? "colorless" : (mtmp->data->mcolor == CLR_ORANGE) ? "orange" : (mtmp->data->mcolor == CLR_BRIGHT_GREEN) ? "bright green" : (mtmp->data->mcolor == CLR_YELLOW) ? "yellow" : (mtmp->data->mcolor == CLR_BRIGHT_BLUE) ? "bright blue" : (mtmp->data->mcolor == CLR_BRIGHT_CYAN) ? "bright cyan" : (mtmp->data->mcolor == CLR_BRIGHT_MAGENTA) ? "bright magenta" : (mtmp->data->mcolor == CLR_WHITE) ? "white" : (mtmp->data->mcolor == CLR_MAX) ? "max" : "unknown" );

	    if (mon_wounds(mtmp)) { 
		Strcat(buf, ", "); 
		Strcat(buf, mon_wounds(mtmp)); 
	    }
	    if (u.ustuck == mtmp)
		Strcat(buf, (Upolyd && sticks(youmonst.data)) ?
			", being held" : ", holding you");
	    if (mtmp->mleashed)
		Strcat(buf, ", leashed to you");

	    if (mtmp->mtrapped && cansee(mtmp->mx, mtmp->my)) {
		struct trap *t = t_at(mtmp->mx, mtmp->my);
		int tt = t ? t->ttyp : NO_TRAP;

		/* newsym lets you know of the trap, so mention it here */
		if (tt == BEAR_TRAP || tt == PIT || tt == SHIT_PIT || tt == MANA_PIT ||
			tt == SPIKED_PIT || tt == GIANT_CHASM || tt == WEB)
		    Sprintf(eos(buf), ", trapped in %s",
			    an(defsyms[trap_to_defsym(tt)].explanation));
	    }

	    {
		int ways_seen = 0, normal = 0, xraydist;
		boolean useemon = (boolean) canseemon(mtmp);

		xraydist = (u.xray_range<0) ? -1 : u.xray_range * u.xray_range;
		/* normal vision */
		if ((mtmp->wormno ? worm_known(mtmp) : cansee(mtmp->mx, mtmp->my)) && !mtmp->minvisreal &&
			mon_visible(mtmp) && !mtmp->minvis) {
		    ways_seen++;
		    normal++;
		}
		/* see invisible */
		if (useemon && mtmp->minvis)
		    ways_seen++;
		/* infravision */
		if ((!mtmp->minvis || See_invisible) && !mtmp->minvisreal && see_with_infrared(mtmp))
		    ways_seen++;
		/* telepathy */
		if (tp_sensemon(mtmp))
		    ways_seen++;
		/* xray */
		if (useemon && xraydist > 0 &&
			distu(mtmp->mx, mtmp->my) <= xraydist)
		    ways_seen++;
		if (Detect_monsters)
		    ways_seen++;
		if (MATCH_WARN_OF_MON(mtmp) /*|| (uamul && uamul->otyp == AMULET_OF_UNDEAD_WARNING && (is_undead(mtmp->data) || mtmp->egotype_undead) ) || (uarmh && uarmh->otyp == HELMET_OF_UNDEAD_WARNING && (is_undead(mtmp->data) || mtmp->egotype_undead) )*/)		    ways_seen++;
		if ( (Role_if(PM_ACTIVISTOR) || Race_if(PM_PEACEMAKER)) && mtmp->data == &mons[PM_TOPMODEL] )
		    ways_seen++;
		if (Role_if(PM_ACTIVISTOR) && type_is_pname(mtmp->data) && uwep && is_quest_artifact(uwep) )
		    ways_seen++;
		if (uamul && uamul->otyp == AMULET_OF_POISON_WARNING && poisonous(mtmp->data))
		    ways_seen++;
		if (uamul && uamul->otyp == AMULET_OF_OWN_RACE_WARNING && your_race(mtmp->data))
		    ways_seen++;
		if (uamul && uamul->otyp == AMULET_OF_COVETOUS_WARNING && (is_covetous(mtmp->data) || mtmp->egotype_covetous) )
		    ways_seen++;
		if (Role_if(PM_PALADIN) && is_demon(mtmp->data))
		    ways_seen++;
		if (isselfhybrid && strongmonst(mtmp->data) && is_wanderer(mtmp->data) )
		    ways_seen++;
		if (isselfhybrid && monpolyok(mtmp->data) && !polyok(mtmp->data) && ((mtmp->data->mlevel < 30) || ((mtmp)->mhp % 2 != 0) ) ) 
		    ways_seen++;
		if (Race_if(PM_VORTEX) && unsolid(mtmp->data))
		    ways_seen++;
		if (Race_if(PM_VORTEX) && nolimbs(mtmp->data))
		    ways_seen++;
		if (Race_if(PM_CORTEX) && unsolid(mtmp->data))
		    ways_seen++;
		if (Race_if(PM_CORTEX) && nolimbs(mtmp->data))
		    ways_seen++;
		if (Race_if(PM_RODNEYAN) && mon_has_amulet(mtmp))
		    ways_seen++;
		if (Race_if(PM_RODNEYAN) && mon_has_special(mtmp))
		    ways_seen++;
		if (Stunnopathy && Stunned && always_hostile(mtmp->data) && (mtmp)->mhp % 4 != 0)
		    ways_seen++;
		if ( (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmh->otyp]), "internet helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "vsemirnaya pautina shlem") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "keng dunyo veb-zarbdan") ) ) && (mtmp)->mhp % 9 == 0)
		    ways_seen++;
		if (Numbopathy && Numbed && (avoid_player(mtmp->data) || mtmp->egotype_avoider) )
		    ways_seen++;
		if (Freezopathy && Frozen && mtmp->data->mcolor == CLR_WHITE )
		    ways_seen++;
		if (Burnopathy && Burned && infravision(mtmp->data) )
		    ways_seen++;
		if (Sickopathy && Sick && extra_nasty(mtmp->data) )
		    ways_seen++;
		if (Race_if(PM_LEVITATOR) && (is_flyer(mtmp->data) || mtmp->egotype_flying) )
		    ways_seen++;

		if (ways_seen > 1 || !normal) {
		    if (normal) {
			Strcat(monbuf, "normal vision");
			/* can't actually be 1 yet here */
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (useemon && mtmp->minvis) {
			Strcat(monbuf, "see invisible");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if ((!mtmp->minvis || See_invisible) && !mtmp->minvisreal &&
			    see_with_infrared(mtmp)) {
			Strcat(monbuf, "infravision");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (tp_sensemon(mtmp)) {
			Strcat(monbuf, "telepathy");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (useemon && xraydist > 0 &&
			    distu(mtmp->mx, mtmp->my) <= xraydist) {
			/* Eyes of the Overworld */
			Strcat(monbuf, "astral vision");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (Detect_monsters) {
			Strcat(monbuf, "monster detection");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if ( (Role_if(PM_ACTIVISTOR) || Race_if(PM_PEACEMAKER) ) && mtmp->data == &mons[PM_TOPMODEL] ) {
			Strcat(monbuf, "warned of topmodels");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (Role_if(PM_ACTIVISTOR) && type_is_pname(mtmp->data) && uwep && is_quest_artifact(uwep) ) {
			Strcat(monbuf, "warned of unique monsters");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (uamul && uamul->otyp == AMULET_OF_POISON_WARNING && poisonous(mtmp->data)) {
			Strcat(monbuf, "warned of poisonous monsters");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (uamul && uamul->otyp == AMULET_OF_OWN_RACE_WARNING && your_race(mtmp->data)) {
			Strcat(monbuf, "warned of monsters that are the same race as you");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (uamul && uamul->otyp == AMULET_OF_COVETOUS_WARNING && (is_covetous(mtmp->data) || mtmp->egotype_covetous) ) {
			Strcat(monbuf, "warned of covetous monsters");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (Role_if(PM_PALADIN) && is_demon(mtmp->data)) {
			Strcat(monbuf, "warned of demons");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (isselfhybrid && strongmonst(mtmp->data) && is_wanderer(mtmp->data) ) {
			Strcat(monbuf, "self-hybridization");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (isselfhybrid && monpolyok(mtmp->data) && !polyok(mtmp->data) && ((mtmp->data->mlevel < 30) || ((mtmp)->mhp % 2 != 0) )) {
			Strcat(monbuf, "self-hybridization");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (Race_if(PM_VORTEX) && unsolid(mtmp->data)) {
			Strcat(monbuf, "warned of unsolid creatures");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (Race_if(PM_VORTEX) && nolimbs(mtmp->data)) {
			Strcat(monbuf, "warned of creatures without limbs");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (Race_if(PM_CORTEX) && unsolid(mtmp->data)) {
			Strcat(monbuf, "warned of unsolid creatures");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (Race_if(PM_CORTEX) && nolimbs(mtmp->data)) {
			Strcat(monbuf, "warned of creatures without limbs");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (Race_if(PM_LEVITATOR) && (is_flyer(mtmp->data) || mtmp->egotype_flying) ) {
			Strcat(monbuf, "warned of flying monsters");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (Race_if(PM_RODNEYAN) && mon_has_amulet(mtmp) ) {
			Strcat(monbuf, "amulet of yendor");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (Race_if(PM_RODNEYAN) && mon_has_special(mtmp) ) {
			Strcat(monbuf, "covetous");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }

		    if (Stunnopathy && Stunned && always_hostile(mtmp->data) && (mtmp)->mhp % 4 != 0) {
			Strcat(monbuf, "stunnopathy");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if ( (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmh->otyp]), "internet helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "vsemirnaya pautina shlem") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "keng dunyo veb-zarbdan") ) ) && (mtmp)->mhp % 9 == 0) {
			Strcat(monbuf, "internet access");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (Numbopathy && Numbed && (avoid_player(mtmp->data) || mtmp->egotype_avoider) ) {
			Strcat(monbuf, "numbopathy");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (Freezopathy && Frozen && mtmp->data->mcolor == CLR_WHITE ) {
			Strcat(monbuf, "freezopathy");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (Burnopathy && Burned && infravision(mtmp->data) ) {
			Strcat(monbuf, "burnopathy");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (Sickopathy && Sick && extra_nasty(mtmp->data) ) {
			Strcat(monbuf, "sickopathy");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }

		    if (MATCH_WARN_OF_MON(mtmp) /*|| (uamul && uamul->otyp == AMULET_OF_UNDEAD_WARNING && (is_undead(mtmp->data) || mtmp->egotype_undead) ) || (uarmh && uarmh->otyp == HELMET_OF_UNDEAD_WARNING && (is_undead(mtmp->data) || mtmp->egotype_undead) )*/) {
		    	char wbuf[BUFSZ];
			if (Hallucination)
				Strcat(monbuf, "paranoid delusion");
			else {
				Sprintf(wbuf, "warned of %s",
					makeplural(mtmp->data->mname));
		    		Strcat(monbuf, wbuf);
		    	}
		    	if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		}
	    }
	}
    }
    else if (glyph_is_object(glyph)) {
	struct obj *otmp = vobj_at(x,y);

	if (!otmp || otmp->otyp != glyph_to_obj(glyph)) {
	    if (glyph_to_obj(glyph) != STRANGE_OBJECT) {
		otmp = mksobj(glyph_to_obj(glyph), FALSE, FALSE);
		if (otmp) {
			if (otmp->oclass == COIN_CLASS)
			    otmp->quan = 2L; /* to force pluralization */
			else if (otmp->otyp == SLIME_MOLD)
			    otmp->spe = current_fruit;	/* give the fruit a type */
			Strcpy(buf, distant_name(otmp, xname));
			dealloc_obj(otmp);
		} else Strcpy(buf, "very weird object");
	    }
	} else
	    Strcpy(buf, distant_name(otmp, xname));

	if (IS_TREE(levl[x][y].typ))
	    Strcat(buf, " stuck in a tree"); 
	else if (IS_IRONBAR(levl[x][y].typ))
	    Strcat(buf, " stuck in iron bars"); 
	else if (levl[x][y].typ == STONE || levl[x][y].typ == SCORR)
	    Strcat(buf, " embedded in stone");
	else if (IS_WALL(levl[x][y].typ) || levl[x][y].typ == SDOOR)
	    Strcat(buf, " embedded in a wall");
	else if (closed_door(x,y))
	    Strcat(buf, " embedded in a door");
	else if (is_pool(x,y))
	    Strcat(buf, " in water");
	else if (is_lava(x,y))
	    Strcat(buf, " in molten lava");	/* [can this ever happen?] */
    } else if (glyph_is_trap(glyph)) {
	int tnum = what_trap(glyph_to_trap(glyph));
	Strcpy(buf, defsyms[trap_to_defsym(tnum)].explanation);
    } else if(!glyph_is_cmap(glyph)) {
	Strcpy(buf,"unexplored area");
    } else switch(glyph_to_cmap(glyph)) {
    case S_altar:
	if(!In_endgame(&u.uz))
	    Sprintf(buf, "%s altar",
		align_str(Amask2align(levl[x][y].altarmask & ~AM_SHRINE)));
	else Sprintf(buf, "aligned altar");
	break;
    case S_ndoor:
	if (is_drawbridge_wall(x, y) >= 0)
	    Strcpy(buf,"open drawbridge portcullis");
	else if ((levl[x][y].doormask & ~D_TRAPPED) == D_BROKEN)
	    Strcpy(buf,"broken door");
	else
	    Strcpy(buf,"doorway");
	break;
    case S_cloud:
	Strcpy(buf, Is_airlevel(&u.uz) ? "cloudy area" : "fog/vapor cloud");
	break;
    case S_water:
    case S_pool:
	Strcpy(buf, level.flags.lethe? "sparkling water" : "water");
	break;
    case S_fountain:
	Strcpy(buf, level.flags.lethe? "sparkling fountain" : "fountain");
	break;
    default:
	Strcpy(buf,defsyms[glyph_to_cmap(glyph)].explanation);
	break;
    }

    return ((pm && !Hallucination) ? pm : (struct permonst *) 0);
}

/*
 * Look in the "data" file for more info.  Called if the user typed in the
 * whole name (user_typed_name == TRUE), or we've found a possible match
 * with a character/glyph and flags.help is TRUE.
 *
 * NOTE: when (user_typed_name == FALSE), inp is considered read-only and
 *	 must not be changed directly, e.g. via lcase(). We want to force
 *	 lcase() for data.base lookup so that we can have a clean key.
 *	 Therefore, we create a copy of inp _just_ for data.base lookup.
 */
STATIC_OVL void
checkfile(inp, pm, user_typed_name, without_asking)
    char *inp;
    struct permonst *pm;
    boolean user_typed_name, without_asking;
{
    dlb *fp;
    char buf[BUFSZ], newstr[BUFSZ];
    char *ep, *dbase_str;
    long txt_offset;
    int chk_skip;
    boolean found_in_file = FALSE, skipping_entry = FALSE;

    fp = dlb_fopen_area(NH_DATAAREA, NH_DATAFILE, "r");
    if (!fp) {
	pline("Cannot open data file!");
	return;
    }

    /* To prevent the need for entries in data.base like *ngel to account
     * for Angel and angel, make the lookup string the same for both
     * user_typed_name and picked name.
     */
    if (pm != (struct permonst *) 0 && !user_typed_name && !u.ughmemory)
	dbase_str = strcpy(newstr, pm->mname);
    else dbase_str = strcpy(newstr, inp);
    (void) lcase(dbase_str);

    if (!strncmp(dbase_str, "interior of ", 12))
	dbase_str += 12;
    if (!strncmp(dbase_str, "a ", 2))
	dbase_str += 2;
    else if (!strncmp(dbase_str, "an ", 3))
	dbase_str += 3;
    else if (!strncmp(dbase_str, "the ", 4))
	dbase_str += 4;
    if (!strncmp(dbase_str, "tame ", 5))
	dbase_str += 5;
    else if (!strncmp(dbase_str, "peaceful ", 9))
	dbase_str += 9;
    if (!strncmp(dbase_str, "invisible ", 10))
	dbase_str += 10;
    if (!strncmp(dbase_str, "statue of ", 10))
	dbase_str[6] = '\0';
    else if (!strncmp(dbase_str, "figurine of ", 12))
	dbase_str[8] = '\0';

    /* Make sure the name is non-empty. */
    if (*dbase_str) {
	/* adjust the input to remove "named " and convert to lower case */
	char *alt = 0;	/* alternate description */

	if ((ep = strstri(dbase_str, " named ")) != 0)
	    alt = ep + 7;
	else
	    ep = strstri(dbase_str, " called ");
	if (!ep) ep = strstri(dbase_str, ", ");
	if (ep && ep > dbase_str) *ep = '\0';

	/*
	 * If the object is named, then the name is the alternate description;
	 * otherwise, the result of makesingular() applied to the name is. This
	 * isn't strictly optimal, but named objects of interest to the user
	 * will usually be found under their name, rather than under their
	 * object type, so looking for a singular form is pointless.
	 */

	if (!alt)
	    alt = makesingular(dbase_str);
	else
	    if (user_typed_name)
		(void) lcase(alt);

	/* skip first record; read second */
	txt_offset = 0L;
	if (!dlb_fgets(buf, BUFSZ, fp) || !dlb_fgets(buf, BUFSZ, fp)) {
	    impossible("can't read 'data' file");
	    (void) dlb_fclose(fp);
	    return;
	} else if (sscanf(buf, "%8lx\n", &txt_offset) < 1 || txt_offset <= 0)
	    goto bad_data_file;

	/* look for the appropriate entry */
	while (dlb_fgets(buf,BUFSZ,fp)) {
	    if (*buf == '.') break;  /* we passed last entry without success */

	    if (digit(*buf)) {
		/* a number indicates the end of current entry */
		skipping_entry = FALSE;
	    } else if (!skipping_entry) {
		if (!(ep = index(buf, '\n'))) goto bad_data_file;
		*ep = 0;
		/* if we match a key that begins with "~", skip this entry */
		chk_skip = (*buf == '~') ? 1 : 0;
		if (pmatch(&buf[chk_skip], dbase_str) ||
			(alt && pmatch(&buf[chk_skip], alt))) {
		    if (chk_skip) {
			skipping_entry = TRUE;
			continue;
		    } else {
			found_in_file = TRUE;
			break;
		    }
		}
	    }
	}
    }

    if(found_in_file) {
	long entry_offset;
	int  entry_count;
	int  i;

	/* skip over other possible matches for the info */
	do {
	    if (!dlb_fgets(buf, BUFSZ, fp)) goto bad_data_file;
	} while (!digit(*buf));
	if (sscanf(buf, "%ld,%d\n", &entry_offset, &entry_count) < 2) {
bad_data_file:	impossible("'data' file in wrong format");
		(void) dlb_fclose(fp);
		return;
	}

	if (user_typed_name || without_asking || yn("More info?") == 'y') {
	    winid datawin;

	    if (dlb_fseek(fp, txt_offset + entry_offset, SEEK_SET) < 0) {
		pline("? Seek error on 'data' file!");
		(void) dlb_fclose(fp);
		return;
	    }
	    datawin = create_nhwindow(NHW_MENU);
	    for (i = 0; i < entry_count; i++) {
		if (!dlb_fgets(buf, BUFSZ, fp)) goto bad_data_file;
		if ((ep = index(buf, '\n')) != 0) *ep = 0;
		if (index(buf+1, '\t') != 0) (void) tabexpand(buf+1);
		putstr(datawin, 0, buf+1);
	    }
	    display_nhwindow(datawin, FALSE);
	    destroy_nhwindow(datawin);
	}
    } else if (user_typed_name)
	pline("I don't have any information on those things.");

    (void) dlb_fclose(fp);
}

/* getpos() return values */
#define LOOK_TRADITIONAL	0	/* '.' -- ask about "more info?" */
#define LOOK_QUICK		1	/* ',' -- skip "more info?" */
#define LOOK_ONCE		2	/* ';' -- skip and stop looping */
#define LOOK_VERBOSE		3	/* ':' -- show more info w/o asking */

/* also used by getpos hack in do_name.c */
const char what_is_an_unknown_object[] = "an unknown object";

STATIC_OVL int
do_look(quick)
    boolean quick;	/* use cursor && don't search for "more info" */
{
    char    out_str[BUFSZ], look_buf[BUFSZ];
    const char *x_str, *firstmatch = 0;
    struct permonst *pm = 0;
    int     i, ans = 0;
    int     sym;		/* typed symbol or converted glyph */
    int	    found;		/* count of matching syms found */
    coord   cc;			/* screen pos of unknown glyph */
    boolean save_verbose;	/* saved value of flags.verbose */
    boolean from_screen;	/* question from the screen */
    boolean need_to_look;	/* need to get explan. from glyph */
    boolean hit_trap;		/* true if found trap explanation */
    int skipped_venom;		/* non-zero if we ignored "splash of venom" */
    static const char *mon_interior = "the interior of a monster";

    if (quick) {
	from_screen = TRUE;	/* yes, we want to use the cursor */
    } else {
	i = ynq("Specify unknown object by cursor?");
	if (i == 'q') return 0;
	from_screen = (i == 'y');
    }

    if (from_screen) {
	cc.x = u.ux;
	cc.y = u.uy;
	sym = 0;		/* gcc -Wall lint */
    } else {
	getlin("Specify what? (type the word)", out_str);
	if (out_str[0] == '\0' || out_str[0] == '\033')
	    return 0;

	if (out_str[1]) {	/* user typed in a complete string */
	    checkfile(out_str, pm, TRUE, TRUE);
	    return 0;
	}
	sym = out_str[0];
    }

    /* Save the verbose flag, we change it later. */
    save_verbose = flags.verbose;
    flags.verbose = flags.verbose && !quick;
    /*
     * The user typed one letter, or we're identifying from the screen.
     */
    do {
	/* Reset some variables. */
	need_to_look = FALSE;
	pm = (struct permonst *)0;
	skipped_venom = 0;
	found = 0;
	out_str[0] = '\0';

	if (from_screen) {
	    int glyph;	/* glyph at selected position */

	    if (flags.verbose)
		pline("Please move the cursor to %s.",
		       what_is_an_unknown_object);
	    else {
		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		pline("Pick an object."); }

	    ans = getpos(&cc, quick, what_is_an_unknown_object);
	    if (ans < 0 || cc.x < 0) {
		flags.verbose = save_verbose;
		return 0;	/* done */
	    }
	    flags.verbose = FALSE;	/* only print long question once */

	    /* Convert the glyph at the selected position to a symbol. */
	    glyph = glyph_at(cc.x,cc.y);
	    if (glyph_is_cmap(glyph)) {
		sym = showsyms[glyph_to_cmap(glyph)];
	    } else if (glyph_is_trap(glyph)) {
		sym = showsyms[trap_to_defsym(glyph_to_trap(glyph))];
	    } else if (glyph_is_object(glyph)) {
		sym = oc_syms[(int)objects[glyph_to_obj(glyph)].oc_class];
		if (sym == '`' && iflags.bouldersym && (int)glyph_to_obj(glyph) == BOULDER)
			sym = iflags.bouldersym;
	    } else if (glyph_is_monster(glyph)) {
		/* takes care of pets, detected, ridden, and regular mons */
		sym = monsyms[(int)mons[glyph_to_mon(glyph)].mlet];
	    } else if (glyph_is_swallow(glyph)) {
		sym = showsyms[glyph_to_swallow(glyph)+S_sw_tl];
	    } else if (glyph_is_invisible(glyph)) {
		sym = DEF_INVISIBLE;
	    } else if (glyph_is_warning(glyph)) {
		sym = glyph_to_warning(glyph);
	    	sym = warnsyms[sym];
	    } else {
		impossible("do_look:  bad glyph %d at (%d,%d)",
						glyph, (int)cc.x, (int)cc.y);
		sym = ' ';
	    }
	}

	/*
	 * Check all the possibilities, saving all explanations in a buffer.
	 * When all have been checked then the string is printed.
	 */

	/* Check for monsters */
	for (i = 0; i < MAXMCLASSES; i++) {
	    if (sym == (from_screen ? monsyms[i] : def_monsyms[i]) &&
		monexplain[i]) {
		need_to_look = TRUE;
		if (!found) {
		    Sprintf(out_str, "%c       %s", sym, an(monexplain[i]));
		    firstmatch = monexplain[i];
		    found++;
		} else {
		    found += append_str(out_str, an(monexplain[i]));
		}
	    }
	}
	/* handle '@' as a special case if it refers to you and you're
	   playing a character which isn't normally displayed by that
	   symbol; firstmatch is assumed to already be set for '@' */
	if ((from_screen ?
		(sym == monsyms[S_HUMAN] && cc.x == u.ux && cc.y == u.uy) :
		(sym == def_monsyms[S_HUMAN] && !iflags.showrace)) &&
	    !(Race_if(PM_HUMAN) || Race_if(PM_ELF)) && !Upolyd)
	    found += append_str(out_str, "you");	/* tack on "or you" */

	/*
	 * Special case: if identifying from the screen, and we're swallowed,
	 * and looking at something other than our own symbol, then just say
	 * "the interior of a monster".
	 */
	if (u.uswallow && from_screen && is_swallow_sym(sym)) {
	    if (!found) {
		Sprintf(out_str, "%c       %s", sym, mon_interior);
		firstmatch = mon_interior;
	    } else {
		found += append_str(out_str, mon_interior);
	    }
	    need_to_look = TRUE;
	}

	/* Now check for objects */
	for (i = 1; i < MAXOCLASSES; i++) {
	    if (sym == (from_screen ? oc_syms[i] : def_oc_syms[i])) {
		need_to_look = TRUE;
		if (from_screen && i == VENOM_CLASS) {
		    skipped_venom++;
		    continue;
		}
		if (!found) {
		    Sprintf(out_str, "%c       %s", sym, an(objexplain[i]));
		    firstmatch = objexplain[i];
		    found++;
		} else {
		    found += append_str(out_str, an(objexplain[i]));
		}
	    }
	}

	if (sym == DEF_INVISIBLE) {
	    if (!found) {
		Sprintf(out_str, "%c       %s", sym, an(invisexplain));
		firstmatch = invisexplain;
		found++;
	    } else {
		found += append_str(out_str, an(invisexplain));
	    }
	}

#define is_cmap_trap(i) ((i) >= S_arrow_trap && (i) <= /*S_polymorph_trap*/S_trap_percents)
#define is_cmap_drawbridge(i) ((i) >= S_vodbridge && (i) <= S_hcdbridge)

	/* Now check for graphics symbols */
	for (hit_trap = FALSE, i = 0; i < MAXPCHARS; i++) {
	    x_str = defsyms[i].explanation;
	    if (sym == (from_screen ? showsyms[i] : defsyms[i].sym) && *x_str) {
		/* avoid "an air", "a water", or "a floor of a room" */
		int article = ((i == S_room)||(i == S_darkroom)) ? 2 :		/* 2=>"the" */
			      !(strcmp(x_str, "air") == 0 ||	/* 1=>"an"  */
				strcmp(x_str, "water") == 0);	/* 0=>(none)*/

		if (!found) {
		    if (is_cmap_trap(i)) {
			Sprintf(out_str, "%c       a trap", sym);
			hit_trap = TRUE;
		    } else if (level.flags.lethe && !strcmp(x_str, "water")) {
			Sprintf(out_str, "%c       sparkling water", sym);
		    } else {
			Sprintf(out_str, "%c       %s", sym,
				article == 2 ? the(x_str) :
				article == 1 ? an(x_str) : x_str);
		    }
		    firstmatch = x_str;
		    found++;
		} else if (!u.uswallow && !(hit_trap && is_cmap_trap(i)) &&
			   !(found >= 3 && is_cmap_drawbridge(i))) {
		    if (level.flags.lethe && !strcmp(x_str, "water"))
			found += append_str(out_str, "sparkling water");
		    else
		    	found += append_str(out_str,
					article == 2 ? the(x_str) :
					article == 1 ? an(x_str) : x_str);
		    if (is_cmap_trap(i)) hit_trap = TRUE;
		}

		if (i == S_altar || is_cmap_trap(i))
		    need_to_look = TRUE;
	    }
	}

	/* Now check for warning symbols */
	for (i = /*1*/0; i < WARNCOUNT; i++) { /* fixed annoying bug --Amy */
	    x_str = def_warnsyms[i].explanation;
	    if (sym == (from_screen ? warnsyms[i] : def_warnsyms[i].sym)) {
		if (!found) {
			Sprintf(out_str, "%c       %s",
				sym, def_warnsyms[i].explanation);
			firstmatch = def_warnsyms[i].explanation;
			found++;
		} else {
			found += append_str(out_str, def_warnsyms[i].explanation);
		}
		/* Kludge: warning trumps boulders on the display.
		   Reveal the boulder too or player can get confused */
		if (from_screen && sobj_at(BOULDER, cc.x, cc.y))
			Strcat(out_str, " co-located with a boulder");
		break;	/* out of for loop*/
	    }
	}
    
	/* if we ignored venom and list turned out to be short, put it back */
	if (skipped_venom && found < 2) {
	    x_str = objexplain[VENOM_CLASS];
	    if (!found) {
		Sprintf(out_str, "%c       %s", sym, an(x_str));
		firstmatch = x_str;
		found++;
	    } else {
		found += append_str(out_str, an(x_str));
	    }
	}

	/* handle optional boulder symbol as a special case */ 
	if (iflags.bouldersym && sym == iflags.bouldersym) {
	    if (!found) {
		firstmatch = "boulder";
		Sprintf(out_str, "%c       %s", sym, an(firstmatch));
		found++;
	    } else {
		found += append_str(out_str, "boulder");
	    }
	}
	
	/*
	 * If we are looking at the screen, follow multiple possibilities or
	 * an ambiguous explanation by something more detailed.
	 */
	if (from_screen) {
	    if (found > 1 || need_to_look) {
		char monbuf[BUFSZ];
		char temp_buf[BUFSZ];

		pm = lookat(cc.x, cc.y, look_buf, monbuf);
		firstmatch = look_buf;
		if (*firstmatch) {
		    Sprintf(temp_buf, " (%s)", firstmatch);
		    (void)strncat(out_str, temp_buf, BUFSZ-strlen(out_str)-1);
		    found = 1;	/* we have something to look up */
		}

		/* mainly for batman and ladiesman: print info about monster's gender --Amy */

		if (pm) {
		    struct monst *mtmpX = m_at(cc.x, cc.y);
		    if (mtmpX) {
			Sprintf(temp_buf, " (base level %d)", mtmpX->data->mlevel);
			(void)strncat(out_str, temp_buf, BUFSZ-strlen(out_str)-1);

		    }
		    if (mtmpX && humanoid(mtmpX->data)) {
			Sprintf(temp_buf, " (%s)", mtmpX->female ? "female" : "male");
			(void)strncat(out_str, temp_buf, BUFSZ-strlen(out_str)-1);

		    }
		}

		if (monbuf[0]) {
		    Sprintf(temp_buf, " [seen: %s]", monbuf);
		    (void)strncat(out_str, temp_buf, BUFSZ-strlen(out_str)-1);
		}
#ifdef WIZARD
		if (wizard && pm) {
		    struct monst *mtmp = m_at(cc.x, cc.y);
		    if (mtmp && mtmp->oldmonnm != monsndx(pm)) {
			Sprintf(temp_buf, " [polymorphed from a %s]",
				mons[mtmp->oldmonnm].mname);
			(void)strncat(out_str, temp_buf, BUFSZ-strlen(out_str)-1);
		    }
		}
#endif

#ifdef EXTENDED_INFO
		if(flags.pokedex && (pm != (struct permonst *) 0) ) {
			append_newline_to_pline_string(out_str);
			temp_buf[0]='\0';
			get_description_of_monster_type(pm, temp_buf);
			(void)strncat(out_str, temp_buf, BUFSZ-strlen(out_str)-1);
		}
#endif

	    }
	}

	/* Finally, print out our explanation. */
	if (found && !RMBLoss && !u.uprops[RMB_LOST].extrinsic && !have_rmbstone()) {

	    pline("%s", out_str);
	/*winid datawin = create_nhwindow(NHW_MENU);
	    putstr(datawin, 0, out_str);

	display_nhwindow(datawin, TRUE);
	destroy_nhwindow(datawin);*/

	    /* check the data file for information about this thing */
	    if (found == 1 && ans != LOOK_QUICK && ans != LOOK_ONCE && !u.ughmemory && !Race_if(PM_MISSINGNO) && 
			(ans == LOOK_VERBOSE || (flags.help && !quick))) {
		char temp_buf[BUFSZ];
		Strcpy(temp_buf, level.flags.lethe 
					&& !strcmp(firstmatch, "water")?
				"lethe" : firstmatch);
		checkfile(temp_buf, pm, FALSE, (boolean)(ans == LOOK_VERBOSE));
	    }
	} else {
	    if (!RMBLoss && !u.uprops[RMB_LOST].extrinsic && !have_rmbstone()) pline("I've never heard of such things.");
	}

    } while (from_screen && !quick && ans != LOOK_ONCE);

    flags.verbose = save_verbose;
    return 0;
}

#ifdef EXTENDED_INFO
int
plined_length(char * buf)
{
	int i = 0;
	int current_line = 0;
	int current_word = 0;
	int total = 0;
	do {
		boolean not_a_whitespace = (buf[i] != ' ' && buf[i] != '\0');
		boolean space_after_space = (i>0 && buf[i-1] == ' ' && buf[i] == ' ');
		//otherwise - end a word
		if (not_a_whitespace) {
			current_word++;
		} else if (space_after_space) {
			total++;
			current_line++;
			if (current_line == CO - 1) {
				current_line = 0;
			}
		} else {
			int available_space = CO - current_line - 1;
			if (current_word == 0) {
				//end of string, previous character was space
			} else if (buf[i] == ' ' && current_word < available_space) {
				//space after word will fit into current line
				total += current_word + 1;
				current_line += current_word + 1;
			} else if (current_word <= available_space) {
				//word will fit into line without space
				total += current_word;
				current_line = 0; //new line
			} else if (buf[i] == ' ' && current_word < CO - 1 ) {
				//word will fit into next line with a space
				total += current_word + 1 + available_space;
				current_line = current_word + 1;
			} else if (current_word < CO) {
				//word will fit into next line without space
				total += current_word + available_space;
				current_line = 0;
			} else {
				//word will take entire next line and more of the next one. Or maybe even multiple lines.
				if (current_line == 0) {
					total += current_word;
				} else {
					total += current_word + available_space;
				}
				current_line = current_word % (CO-1);
				if (buf[i] == ' ' && current_line > 0) {
					total++;
					current_line = (current_line+1) % (CO-1);
				}
			}
			current_word = 0;
		}
		i++;
	} while (buf[i-1] != '\0');
	return total;
}

void
append_newline_to_pline_string(char * buf)
{
	int length = plined_length(buf);
	int line_length = CO - 1;
	int addditional_required = (line_length - (length % line_length)) % line_length;
	boolean trailing_space = FALSE;
	if (addditional_required == 0 && buf[strlen(buf)-1] != ' ') {
		trailing_space = TRUE;
	}
	#ifdef USE_TILES
		trailing_space = TRUE;
	#endif
	if(trailing_space) {
		strcat(buf, " ");
	}
	#ifdef USE_TILES
		/* there is limited amount of lines available for message and wide text field. Adding enters would make text hard to read */
		return;
	#endif
	while(addditional_required--) {
		strcat(buf, " ");
	}
}

int
append(char * buf, int condition, char * text, boolean many)
{
	if (condition) {
		if (buf != NULL) {
			if (many) {
				(void)strcat(buf, ", ");
			}
			(void)strcat(buf, text);
		}
		return many+1;
	}
	return many;
}

int
generate_list_of_resistances(char * temp_buf, /*uchar*/unsigned long flags)
{
	int many = 0;
	many = append(temp_buf, (flags & MR_FIRE), "fire", many);
	many = append(temp_buf, (flags & MR_COLD), "cold", many);
	many = append(temp_buf, (flags & MR_SLEEP), "sleep", many);
	many = append(temp_buf, (flags & MR_DISINT), "disintegration", many);
	many = append(temp_buf, (flags & MR_ELEC), "electricity", many);
	many = append(temp_buf, (flags & MR_POISON), "poison", many);
	many = append(temp_buf, (flags & MR_ACID), "acid", many);
	many = append(temp_buf, (flags & MR_STONE), "petrification", many);
	many = append(temp_buf, (flags & MR_DRAIN), "level drain", many);
	many = append(temp_buf, (flags & MR_DEATH), "death magic", many);
	many = append(temp_buf, (flags & MR_PLUSONE), "needs +1 weapon to hit", many);
	many = append(temp_buf, (flags & MR_PLUSTWO), "needs +2 weapon to hit", many);
	many = append(temp_buf, (flags & MR_PLUSTHREE), "needs +3 weapon to hit", many);
	many = append(temp_buf, (flags & MR_PLUSFOUR), "needs +4 weapon to hit", many);
	many = append(temp_buf, (flags & MR_HITASONE), "hits as a +1 weapon", many);
	many = append(temp_buf, (flags & MR_HITASTWO), "hits as a +2 weapon", many);
	many = append(temp_buf, (flags & MR_HITASTHREE), "hits as a +3 weapon", many);
	many = append(temp_buf, (flags & MR_HITASFOUR), "hits as a +4 weapon", many);
	return many;
}

char *
get_generation_description_of_monster_type(struct permonst * ptr, char * temp_buf)
{
	int many = 0;
	if ((ptr->geno & G_NOGEN) == 0) {
/*		strcat(temp_buf, "Normally appears ");
		many = append(temp_buf, ((ptr->geno & G_NOHELL) == 0) && ((ptr->geno & G_HELL) == 0), "everywhere", many);
		many = append(temp_buf, (ptr->geno & G_NOHELL), "outside Gehennon", many);
		many = append(temp_buf, (ptr->geno & G_HELL), "in Gehennon", many);*/
		many = append(temp_buf, !(ptr->geno & G_NOGEN), "Normal generation", many);
	} else {
		many = append(temp_buf, (ptr->geno & G_NOGEN), "Special generation", many);
	}
	many = append(temp_buf, (ptr->geno & G_UNIQ), "unique", many);
	many = 0;
	many = append(temp_buf, (ptr->geno & G_SGROUP), " in groups", many);
	many = append(temp_buf, (ptr->geno & G_LGROUP), " in large groups", many);
	many = append(temp_buf, (ptr->geno & G_VLGROUP), " in very large groups", many);
	many = append(temp_buf, (ptr->geno & G_RGROUP), " in random groups", many);
	many = append(temp_buf, (ptr->geno & G_PLATOON), " of escorts", many);
	if ((ptr->geno & G_NOGEN) == 0) {
		char frequency[BUFSZ] = "";
		Sprintf(frequency, ", with frequency %d.", (ptr->geno & G_FREQ));
		strcat(temp_buf, frequency);
		if (uncommon2(ptr)) strcat(temp_buf, " Uncommon (by factor 2).");
		if (uncommon3(ptr)) strcat(temp_buf, " Uncommon (by factor 3).");
		if (uncommon5(ptr)) strcat(temp_buf, " Uncommon (by factor 5).");
		if (uncommon7(ptr)) strcat(temp_buf, " Uncommon (by factor 7).");
		if (uncommon10(ptr)) strcat(temp_buf, " Uncommon (by factor 10).");
	} else {
		strcat(temp_buf, ".");
	}
	return temp_buf;
}

char *
get_resistance_description_of_monster_type(struct permonst * ptr, char * description)
{
	char temp_buf[BUFSZ] = "";
	temp_buf[0] = '\0';
	int count = generate_list_of_resistances(temp_buf, ptr->mresists);
	if(count == 0) {
		strcat(description, " No resistances.");
	} else {
		strcat(description, " Resists ");
		strcat(description, temp_buf);
		strcat(description, ".");
	}

	temp_buf[0] = '\0';
	count = generate_list_of_resistances(temp_buf, ptr->mconveys);
	if ((ptr->geno & G_NOCORPSE) != 0) {
		strcat(description, " Leaves no corpse.");
	} else if (count == 0) {
		strcat(description, " No conveyed resistances.");
	} else {
		strcat(description, " Conveys ");
		strcat(description, temp_buf);
		if (count == 1) {
			strcat(description, " resistance.");
		} else {
			strcat(description, " resistances.");
		}
	}

	return description;
}

char *
get_flag_description_of_monster_type(struct permonst * ptr, char * description)
{
	char temp_buf[BUFSZ] = "";
	char size[BUFSZ] = "";
	char adjectives[BUFSZ] = "";
	char special_adjectives[BUFSZ] = "";
	char noun[BUFSZ] = "";

	if (verysmall(ptr)) {
		strcat(size, "small");
	} else if (bigmonst(ptr)) {
		strcat(size, "big");
	}

	int adjective_counter = 0;
	adjective_counter = append(adjectives, (ptr->geno & G_GENO), "genocidable", adjective_counter);
	adjective_counter = append(adjectives, !(ptr->geno & G_GENO), "not genocidable", adjective_counter);
	adjective_counter = append(adjectives, (breathless(ptr)), "breathless", adjective_counter);
	adjective_counter = append(adjectives, (amphibious(ptr)), "amphibious", adjective_counter);
	adjective_counter = append(adjectives, (passes_walls(ptr)), "phasing", adjective_counter);
	adjective_counter = append(adjectives, (amorphous(ptr)), "amorphous", adjective_counter);
	adjective_counter = append(adjectives, (noncorporeal(ptr)), "noncorporeal", adjective_counter);
	adjective_counter = append(adjectives, (unsolid(ptr)), "unsolid", adjective_counter);
	adjective_counter = append(adjectives, (acidic(ptr)), "acidic", adjective_counter);
	adjective_counter = append(adjectives, (carnivorous(ptr)), "carnivorous", adjective_counter);
	adjective_counter = append(adjectives, (herbivorous(ptr)), "herbivorous", adjective_counter);
	adjective_counter = append(adjectives, (metallivorous(ptr)), "metallivorous", adjective_counter);
	adjective_counter = append(adjectives, (lithivorous(ptr)), "lithivorous", adjective_counter);
	adjective_counter = append(adjectives, (poisonous(ptr)), "poisonous", adjective_counter);
	adjective_counter = append(adjectives, (regenerates(ptr)), "regenerating", adjective_counter);
	adjective_counter = append(adjectives, (can_teleport(ptr)), "teleporting", adjective_counter);
	adjective_counter = append(adjectives, (is_reviver(ptr)), "reviving", adjective_counter);
	adjective_counter = append(adjectives, (pm_invisible(ptr)), "invisible", adjective_counter);
	adjective_counter = append(adjectives, (thick_skinned(ptr)), "thick-skinned", adjective_counter);
	adjective_counter = append(adjectives, (hides_under(ptr)), "concealing", adjective_counter);
	adjective_counter = append(adjectives, (nonliving(ptr) && !is_undead(ptr)), "nonliving", adjective_counter);

	append(special_adjectives, (is_undead(ptr)), "undead", 0);
	append(special_adjectives, (is_demon(ptr)), "demon", 0);

	int many = 0;
	many = append(noun, (is_hider(ptr)), "hider", many);
	many = append(noun, (is_swimmer(ptr)), "swimmer", many);
	many = append(noun, (is_flyer(ptr)), "flyer", many);
	many = append(noun, (is_floater(ptr)), "floater", many);
	many = append(noun, (is_clinger(ptr)), "clinger", many);
	if (tunnels(ptr)) {
		if (needspick(ptr)) {
			many = append(noun, TRUE, "miner", many);
		} else {
			many = append(noun, TRUE, "digger", many);
		}
	}
	
	// <size><adjectives><special_adjectives><noun>
	if (strlen(size) > 0) {
		if (adjective_counter <= 1 && (strlen(special_adjectives) > 0 || strlen(noun) > 0)) {
			/* huge undead */
			/* small noncorporeal miner */
			strcat(temp_buf, size);
			strcat(temp_buf, " ");
		} else if (adjective_counter >= 1) {
			/* small, genocideable, amphibious swimmer */
			/* big, poisonous, invisible miner */
			/* big, poisonous, invisible hider, swimmer, flyer*/
			/* huge */
			/* small, noncorporeal */
			/* big, poisonous, invisible */
			/* big, poisonous, invisible*/
			strcat(temp_buf, size);
			strcat(temp_buf, ", ");
		} else if (adjective_counter == 0){
			/* small swimmer */
			/* big miner */
			/* big swimmer, flyer*/
			/* huge */
			/* small undead digger */
			strcat(temp_buf, size);
			strcat(temp_buf, " ");
		} else {
			impossible("impossible happened in get_flag_description_of_monster_type");
		}
	}
	if(strlen(adjectives) > 0) {
		strcat(temp_buf, adjectives);
		strcat(temp_buf, " ");
	}
	if(strlen(special_adjectives) > 0) {
		strcat(temp_buf, special_adjectives);
		strcat(temp_buf, " ");
	}
	if(strlen(noun) > 0) {
		strcat(temp_buf, noun);
		strcat(temp_buf, " ");
	}
	if (strlen(temp_buf) > 0) {
		upstart(temp_buf);
		temp_buf[strlen(temp_buf)-1] = '.'; //replaces last space
		strcat(description, " ");
		strcat(description, temp_buf);
	}
	if (perceives(ptr)) {
		strcat(description, " See invisible.");
	}
	if (control_teleport(ptr)) {
		strcat(description, " Controls teleport.");
	}
	if (your_race(ptr)) {
		strcat(description, " Is the same race as you.");
	}
	if (touch_petrifies(ptr)) {
		strcat(description, " It can turn you to stone if you touch it.");
	}
	if (slime_on_touch(ptr)) {
		strcat(description, " It can turn you to slime if you touch it.");
	}
	if (eating_is_fatal(ptr)) {
		strcat(description, " Eating its corpse is instantly fatal.");
	}
	if (is_mind_flayer(ptr)) {
		strcat(description, " Is a mind flayer.");
	}
	if (ptr->msound == MS_NEMESIS) {
		strcat(description, " Is a nemesis (immune to death rays).");
	}
	if (dmgtype(ptr, AD_MAGM) || ptr == &mons[PM_BABY_GRAY_DRAGON] || ptr == &mons[PM_YOUNG_GRAY_DRAGON] || ptr == &mons[PM_YOUNG_ADULT_GRAY_DRAGON] ||
		dmgtype(ptr, AD_RBRE) ) {
		strcat(description, " Magic resistant.");
	}
	if (is_giant(ptr)) {
		strcat(description, " Is a giant.");
	}
	if (is_male(ptr)) {
		strcat(description, " This monster is always male.");
	}
	if (is_female(ptr)) {
		strcat(description, " This monster is always female.");
	}
	if (is_neuter(ptr)) {
		strcat(description, " This monster is of neuter gender.");
	}
	if (hates_silver(ptr)) {
		strcat(description, " Silver weapons do extra damage to it.");
	}
	if (vegan(ptr)) {
		strcat(description, " May be eaten by vegans.");
	} else if (vegetarian(ptr)) {
		strcat(description, " May be eaten by vegetarians.");
	}
	/*
	Unfortunately keepdogs function is quite mysterious
		- Cthulhu and Orcus never follow (M2_STALK and STRAT_WAITFORU flag)
		- Vlad follows (M2_STALK and STRAT_WAITFORU flag)*/
	if (ptr->mflags2 & M2_STALK) {
		strcat(description, " Follows you to other levels.");
	}

	if (is_covetous(ptr)) {
		strcat(description, " Covetous.");
	}
	if (infravision(ptr)) {
		strcat(description, " Has infravision.");
	}
	if (infravisible(ptr)) {
		strcat(description, " Can be seen with infravision.");
	}
	if (throws_rocks(ptr)) {
		strcat(description, " Can pick up and throw rocks.");
	}
	if (can_betray(ptr)) {
		strcat(description, " Can spontaneously betray you if tame.");
	}
	if (cannot_be_tamed(ptr)) {
		strcat(description, " Can't be tamed.");
	}
	if (avoid_player(ptr)) {
		strcat(description, " Tries to avoid moving right next to you.");
	}
	if (is_domestic(ptr)) {
		strcat(description, " Can be tamed by throwing a certain kind of food.");
	}
	if (is_petty(ptr)) {
		strcat(description, " Can be tamed by throwing kelp frond or a poke ball.");
	}
	if (is_pokemon(ptr)) {
		strcat(description, " Is a pokemon (more likely to be caught in a poke ball).");
	}
	if (is_mplayer(ptr)) {
		strcat(description, " Is a playable role and thus likely to be well-armed.");
	}
	if (is_umplayer(ptr)) {
		strcat(description, " Is an undead player character. Be very careful.");
	}

	
	if (polyok(ptr)) {
		strcat(description, " Is a valid polymorph form.");
	} else if (monpolyok(ptr)) {
		strcat(description, " Is a valid polymorph form for monsters only.");
	} else {
		strcat(description, " Is not a valid polymorph form.");
	}
	return description;
}

char *
get_speed_description_of_monster_type(struct permonst * ptr, char * description)
{
	if(ptr->mmove > 35) {
		sprintf(description, "Extremely fast (%d). ", ptr->mmove);
	} else if(ptr->mmove > 19) {
		sprintf(description, "Very fast (%d). ", ptr->mmove);
	} else if(ptr->mmove > 12) {
		sprintf(description, "Fast (%d). ", ptr->mmove);
	} else if(ptr->mmove == 12) {
		sprintf(description, "Normal speed (%d). ", ptr->mmove);
	} else if(ptr->mmove > 8) {
		sprintf(description, "Slow (%d). ", ptr->mmove);
	} else if(ptr->mmove > 3) {
		sprintf(description, "Very slow (%d). ", ptr->mmove);
	} else if(ptr->mmove > 0) {
		sprintf(description, "Almost immobile (%d). ", ptr->mmove);
	} else {
		sprintf(description, "Sessile (%d). ", ptr->mmove);
	}

	if (is_nonmoving(ptr)) sprintf(description, "Can't move around. Speed %d. ", ptr->mmove);

	return description;
}

char *
get_description_of_attack_type(uchar id)
{
	switch(id){
		/*case AT_ANY: return "fake attack; dmgtype_fromattack wildcard";*/ /* no monster has that... --Amy */
		case AT_NONE: return "passive";
		case AT_CLAW: return "claw (punch, hit, etc.)";
		case AT_BITE: return "bite";
		case AT_KICK: return "kick";
		case AT_BUTT: return "head butt";
		case AT_TUCH: return "touches";
		case AT_STNG: return "sting";
		case AT_HUGS: return "crushing bearhug";
		case AT_SPIT: return "spits substance";
		case AT_ENGL: return "engulf";
		case AT_BREA: return "breath";
		case AT_EXPL: return "explodes - proximity";
		case AT_BOOM: return "explodes when killed";
		case AT_GAZE: return "gaze";
		case AT_TENT: return "tentacles";
		case AT_SCRA: return "scratch";
		case AT_LASH: return "lash";
		case AT_TRAM: return "trample";
		case AT_WEAP: return "uses weapon";
		case AT_MAGC: return "uses magic spell(s)";
		case AT_MULTIPLY: return "multiplies";
		case AT_BEAM: return "beam";
		default: 
		if (!missingnoprotect) {
		impossible("bug in get_description_of_attack_type(%d)", id); return "<MISSING DECRIPTION, THIS IS A BUG>";
		}
		else return "undefined attack";
	}
}

char *
get_description_of_damage_type(uchar id)
{
	switch(id){
		case AD_ANY: return "fake damage; attacktype_fordmg wildcard";
		case AD_PHYS: return "ordinary physical";
		case AD_MAGM: return "magic missiles";
		case AD_FIRE: return "fire damage";
		case AD_COLD: return "frost damage";
		case AD_SLEE: return "sleep";
		case AD_DISN: return "disintegration";
		case AD_ELEC: return "shock damage";
		case AD_DRST: return "drains strength";
		case AD_ACID: return "acid damage";
		case AD_SPC2: return "psybeam";
		case AD_BLND: return "blinds";
		case AD_STUN: return "stuns";
		case AD_SLOW: return "slows";
		case AD_PLYS: return "paralyses";
		case AD_DRLI: return "drains life levels";
		case AD_DREN: return "drains magic energy";
		case AD_LEGS: return "damages legs";
		case AD_STON: return "petrifies";
		case AD_STCK: return "sticks to you";
		case AD_SGLD: return "steals gold";
		case AD_SITM: return "steals item";
		case AD_SEDU: return "seduces and steals multiple items";
		case AD_TLPT: return "teleports you";
		case AD_RUST: return "rusts items";
		case AD_CONF: return "confuses";
		case AD_DGST: return "digestion";
		case AD_HEAL: return "heals wounds";
		case AD_WRAP: return "wraps around";
		case AD_WERE: return "confers lycanthropy";
		case AD_DRDX: return "drains dexterity";
		case AD_DRCO: return "drains constitution";
		case AD_DRIN: return "drains intelligence";
		case AD_DISE: return "confers diseases";
		case AD_DCAY: return "decays organics";
		case AD_SSEX: return "foocubus seduction";
		case AD_HALU: return "causes hallucination";
		case AD_DETH: return "unique Death attack";
		case AD_PEST: return "unique Pestilence attack";
		case AD_FAMN: return "unique Famine attack";
		case AD_SLIM: return "turns you into green slime";
		case AD_ENCH: return "remove enchantment";
		case AD_CORR: return "corrode armor";
		case AD_CLRC: return "random clerical spell";
		case AD_SPEL: return "random magic spell";
		case AD_RBRE: return "random effect";
		case AD_SAMU: return "hits, may steal Amulet";
		case AD_CURS: return "random curse";
		case AD_LITE: return "light";
		case AD_CALM: return "calms";
		case AD_POLY: return "polymorphs you";
		case AD_TCKL: return "tickles you";
		case AD_NGRA: return "removes engravings";
		case AD_GLIB: return "disarms you";
		case AD_DARK: return "causes darkness around you";
		case AD_WTHR: return "withers items";
		case AD_LUCK: return "drains luck";
		case AD_NUMB: return "numbness";
		case AD_FRZE: return "freezes you solid";
		case AD_BURN: return "burns you";
		case AD_FEAR: return "causes fear";
		case AD_DISP: return "pushes you away";

		case AD_NPRO: return "negative protection";
		case AD_POIS: return "poison damage";
		case AD_THIR: return "thirsty attack";
		case AD_LAVA: return "lava";
		case AD_FAKE: return "plines";
		case AD_LETH: return "lethe";
		case AD_CNCL: return "cancellation";
		case AD_BANI: return "banishment";
		case AD_WISD: return "drains wisdom";
		case AD_SHRD: return "shredding";
		case AD_WET: return "water damage";
		case AD_SUCK: return "sucking";
		case AD_MALK: return "high voltage";
		case AD_UVUU: return "nasty head spike";
		case AD_ABDC: return "abduction";
		case AD_AXUS: return "multi-element counterattack";
		case AD_CHKH: return "escalating damage";
		case AD_HODS: return "mirror attack";
		case AD_CHRN: return "cursed unicorn horn";
		case AD_WEEP: return "level teleport or level drain";
		case AD_VAMP: return "bloodsucking";
		case AD_WEBS: return "webbing";
		case AD_STTP: return "item teleportation";
		case AD_DEPR: return "depression";
		case AD_WRAT: return "unique Wrath attack";
		case AD_LAZY: return "unique Sloth attack";
		case AD_DRCH: return "drains charisma";
		case AD_DFOO: return "unique Pride attack";
		case AD_NEXU: return "nexus";
		case AD_TIME: return "time";
		case AD_PLAS: return "plasma";
		case AD_MANA: return "mana";
		case AD_SOUN: return "sound";
		case AD_GRAV: return "gravity";
		case AD_INER: return "inertia";
		case AD_SKIL: return "skill point drain";

		case AD_VENO: return "toxic venom";
		case AD_DREA: return "dream eater";
		case AD_NAST: return "nastiness";
		case AD_BADE: return "bad effect";
		case AD_SLUD: return "sludge";
		case AD_ICUR: return "item cursing";
		case AD_VULN: return "vulnerability";
		case AD_FUMB: return "fumbling";

		case AD_ENDS: return "placeholder attack";
		default:
		if (!missingnoprotect) {
		impossible("bug in get_description_of_damage_type(%d)", id); return "<MISSING DESCRIPTION, THIS IS A BUG>";
		}
		else return "undefined damage";
	}
}

char *
get_description_of_attack(struct attack *mattk, char * main_temp_buf)
{
	if(!(mattk->damn + mattk->damd + mattk->aatyp + mattk->adtyp)) {
		main_temp_buf[0]='\0';
		return main_temp_buf;
	}
	
	char temp_buf[BUFSZ] = "";
	if(mattk->damn + mattk->damd) {
		sprintf(main_temp_buf, "%dd%d", mattk->damn, mattk->damd);
		#ifndef USE_TILES
			strcat(main_temp_buf, ",");
		#endif
		strcat(main_temp_buf, " ");
	} else {
		main_temp_buf[0] = '\0';
	}
	#ifndef USE_TILES
		while (strlen(main_temp_buf) < 6) {
			strcat(main_temp_buf, " ");
		}
	#endif
	sprintf(temp_buf, "%s - %s", get_description_of_attack_type(mattk->aatyp), get_description_of_damage_type(mattk->adtyp));
	strcat(main_temp_buf, temp_buf);
	#ifdef USE_TILES
		strcat(main_temp_buf, "; ");
	#endif
	return main_temp_buf;
}

char *
get_description_of_monster_type(struct permonst * ptr, char * description)
{
	/*int monsternumber;*/
/*
	pline("%d<><><>", plined_length("12345678901234567890123456789012345678901234567890123456789012345678901234567890"));//0 passed
	pline("%d<><><>", plined_length("1234567890123456789012345678901234567890123456789012345678901234567890123456789"));
*/
	char temp_buf[BUFSZ] = "";
	char main_temp_buf[BUFSZ] = "";

	temp_buf[0]='\0';
	sprintf(temp_buf, "Accessing Pokedex entry for %s... ", (!missingnoprotect || !Upolyd || ((int)ptr < NUMMONS) ) ? ptr->mname : "this weird creature");
	strcat(description, temp_buf);
	append_newline_to_pline_string(description);
	strcat(description, " ");
	append_newline_to_pline_string(description);
	strcat(description, "Base statistics of this monster type:");
	append_newline_to_pline_string(description);

	/*monsternumber = monsndx(ptr);*/
	sprintf(temp_buf, "Base level = %d, difficulty = %d, AC = %d, magic resistance = %d, alignment %d. ", ptr->mlevel, monstr[monsndx(ptr)], ptr->ac, ptr->mr, ptr->maligntyp);
	strcat(description, temp_buf);
	temp_buf[0]='\0';
	strcat(description, get_speed_description_of_monster_type(ptr, temp_buf));

	temp_buf[0]='\0';
	strcat(description, get_generation_description_of_monster_type(ptr, temp_buf));
	temp_buf[0]='\0';
	strcat(description, get_resistance_description_of_monster_type(ptr, temp_buf));
	temp_buf[0]='\0';
	strcat(description, get_flag_description_of_monster_type(ptr, temp_buf));

	append_newline_to_pline_string(description);
	strcat(description, "Attacks:");
	append_newline_to_pline_string(description);
	struct attack *mattk;
	struct attack alt_attk;
	int sum[NATTK];
	int i;
	for(i = 0; i < NATTK; i++) {
		sum[i] = 0;
		mattk = getmattk(ptr, i, sum, &alt_attk);
		main_temp_buf[0]='\0';
		get_description_of_attack(mattk, temp_buf);
		if(temp_buf[0] == '\0') {
			if (i == 0) {
				#ifndef USE_TILES
					strcat(description, "    ");
				#endif
				strcat(description, "none");
				append_newline_to_pline_string(description);
			}
			break;
		}
		#ifndef USE_TILES
			strcat(main_temp_buf, "    ");
		#endif
		strcat(main_temp_buf, temp_buf);
		append_newline_to_pline_string(main_temp_buf);
		strcat(description, main_temp_buf);
	}
	return description;
}
#endif

int
dowhatis()
{
	return do_look(FALSE);
}

int
doquickwhatis()
{
	return do_look(TRUE);
}

int
doidtrap()
{
	register struct trap *trap;
	int x, y, tt;

	if (!getdir("^")) return 0;
	x = u.ux + u.dx;
	y = u.uy + u.dy;
	for (trap = ftrap; trap; trap = trap->ntrap)
	    if (trap->tx == x && trap->ty == y) {
		if (!trap->tseen) break;
		tt = trap->ttyp;
		if (u.dz) {
		    if (u.dz < 0 ? (tt == TRAPDOOR || tt == HOLE || tt == SHAFT_TRAP) :
			    tt == ROCKTRAP) break;
		}
		tt = what_trap(tt);
		pline("That is %s%s%s.",
		      an(defsyms[trap_to_defsym(tt)].explanation),
		      !trap->madeby_u ? "" : (tt == WEB) ? " woven" :
			  /* trap doors & spiked pits can't be made by
			     player, and should be considered at least
			     as much "set" as "dug" anyway */
			  (tt == HOLE || tt == PIT) ? " dug" : " set",
		      !trap->madeby_u ? "" : " by you");
		return 0;
	    }
	pline("I can't see a trap there.");
	return 0;
}

char *
dowhatdoes_core(q, cbuf)
char q;
char *cbuf;
{
	dlb *fp;
	char bufr[BUFSZ];
	register char *buf = &bufr[6], *ep, ctrl, meta;

	fp = dlb_fopen_area(NH_CMDHELPAREA, NH_CMDHELPFILE, "r");
	if (!fp) {
		pline("Cannot open data file!");
		return 0;
	}

  	ctrl = ((q <= '\033') ? (q - 1 + 'A') : 0);
	meta = ((0x80 & q) ? (0x7f & q) : 0);
	while(dlb_fgets(buf,BUFSZ-6,fp)) {
	    if ((ctrl && *buf=='^' && *(buf+1)==ctrl) ||
		(meta && *buf=='M' && *(buf+1)=='-' && *(buf+2)==meta) ||
		*buf==q) {
		if ((ep = index(buf, '\n')) != 0) *ep = 0;
#ifdef MSDOS
		if ((ep = index(buf, '\r')) != 0) *ep = 0;
#endif
		if (ctrl && buf[2] == '\t'){
			buf = bufr + 1;
			(void) strncpy(buf, "^?      ", 8);
			buf[1] = ctrl;
		} else if (meta && buf[3] == '\t'){
			buf = bufr + 2;
			(void) strncpy(buf, "M-?     ", 8);
			buf[2] = meta;
		} else if(buf[1] == '\t'){
			buf = bufr;
			buf[0] = q;
			(void) strncpy(buf+1, "       ", 7);
		}
		(void) dlb_fclose(fp);
		Strcpy(cbuf, buf);
		return cbuf;
	    }
	}
	(void) dlb_fclose(fp);
	return (char *)0;
}

int
dowhatdoes()
{
	char bufr[BUFSZ];
	char q, *reslt;

#if defined(UNIX) || defined(VMS)
	introff();
#endif
	q = yn_function("What command?", (char *)0, '\0');
#if defined(UNIX) || defined(VMS)
	intron();
#endif
	reslt = dowhatdoes_core(q, bufr);
	if (reslt)
		pline("%s", reslt);
	else
		pline("I've never heard of such commands.");
	return 0;
}

/* data for help_menu() */
static const char *help_menu_items[] = {
/* 0*/	"Long description of the game and commands.",
/* 1*/	"List of game commands.",
/* 2*/	"Concise history of Slash'EM.",
/* 3*/	"Info on a character in the game display.",
/* 4*/	"Info on what a given key does.",
/* 5*/	"List of game options.",
/* 6*/	"Longer explanation of game options.",
/* 7*/	"Full list of keyboard commands.",
/* 8*/	"List of extended commands.",
/* 9*/  "The NetHack license.",
#if 0
#ifndef MAC
/*WAC Add access to txt guidebook*/
/* 9*/  "The Slash'EM Guidebook.",
#endif
#endif
#ifdef PORT_HELP
	"%s-specific help and commands.",
#endif
#ifdef WIZARD
	"List of wizard-mode commands.",
#endif
	"",
	(char *)0
};

enum {
  LICENSE_SLOT=8,
#ifndef MAC
  GUIDEBOOK_SLOT,
#endif
#ifdef PORT_HELP
  PORT_HELP_ID,
#endif
#ifdef WIZARD
  WIZHLP_SLOT,
#endif
  NULL_SLOT
};

STATIC_OVL boolean
help_menu(sel)
	int *sel;
{
	winid tmpwin = create_nhwindow(NHW_MENU);
#ifdef PORT_HELP
	char helpbuf[QBUFSZ];
#endif
	int i, n;
	menu_item *selected;
	anything any;

	any.a_void = 0;		/* zero all bits */
	start_menu(tmpwin);
#ifdef WIZARD
	if (!wizard) help_menu_items[WIZHLP_SLOT] = "",
		     help_menu_items[WIZHLP_SLOT+1] = (char *)0;
#endif
	for (i = 0; help_menu_items[i]; i++)
#ifdef PORT_HELP
	    /* port-specific line has a %s in it for the PORT_ID */
	    if (help_menu_items[i][0] == '%') {
		Sprintf(helpbuf, help_menu_items[i], PORT_ID);
		any.a_int = PORT_HELP_ID + 1;
		add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
			 helpbuf, MENU_UNSELECTED);
	    } else
#endif
	    {
		any.a_int = (*help_menu_items[i]) ? i+1 : 0;
		add_menu(tmpwin, NO_GLYPH, &any, 0, 0,
			ATR_NONE, help_menu_items[i], MENU_UNSELECTED);
	    }
	end_menu(tmpwin, "Select one item:");
	n = select_menu(tmpwin, PICK_ONE, &selected);
	destroy_nhwindow(tmpwin);
	if (n > 0) {
	    *sel = selected[0].item.a_int - 1;
	    free((genericptr_t)selected);
	    return TRUE;
	}
	return FALSE;
}

int
dohelp()
{
	int sel = 0;

	if (help_menu(&sel)) {
		switch (sel) {
			case  0:  display_file_area(NH_HELP_AREA, NH_HELP, TRUE);
				  break;
			case  1:  display_file_area(NH_SHELP_AREA, NH_SHELP, TRUE);
				  break;
			case  2:  (void) dohistory();  break;
			case  3:  (void) dowhatis();  break;
			case  4:  (void) dowhatdoes();  break;
			case  5:  option_help();  break;
			case  6:  display_file_area(NH_OPTIONAREA,
				    NH_OPTIONFILE, TRUE);
				  break;
			case  7:   dokeylist();  break;
			case  8:  (void) doextlist();  break;
			case  9:  display_file_area(NH_LICENSE_AREA,
				    NH_LICENSE, TRUE);
				  break;
#if 0
#ifndef MAC
/*WAC add guidebook.*/
                        case  GUIDEBOOK_SLOT:  display_file_area(NH_GUIDEBOOK_AREA,
				    NH_GUIDEBOOK, TRUE);
				  break;
#endif
#endif
#ifdef PORT_HELP
			case PORT_HELP_ID:  port_help();  break;
#endif
#ifdef WIZARD
                        case  WIZHLP_SLOT:  display_file_area(NH_DEBUGHELP_AREA,
				    NH_DEBUGHELP, TRUE);
				  break;
#endif
		}
	}
	return 0;
}

int
dohistory()
{
	display_file_area(NH_HISTORY_AREA, NH_HISTORY, TRUE);
	return 0;
}

static NEARDATA const char * const bosstaunts[] = {
"cackles evilly.",
"cackles diabolically.",
"says: 'Surrender, miserable flea!'",
"says: 'Come get some!'",
"says: 'Let's rock!'",
"laughs devilishly.",
"says: 'Flee while you can, gnat!'",
"says: 'You are about to die, maggot!'",
"says: 'Read your prayers!'",
"hisses: 'Die!'",
"says: 'You don't have a chance, moron!'",
"says: 'Fear my wrath, fool!'",
"says: 'Feel my fury, dolt!'",
"says: 'Groo is a genius, compared to you!'",
"gives you a contemptuous glance.",
"says: 'Prepare to meet your Maker, fool!'",
"says: 'Perish, mortal!'",
"says: 'Your puny efforts make me laugh!'",
"says: 'Drop dead, wimp!'",
"says: 'You should have fled while you had the chance.'",
"screams: 'Die by my hand!'",
"says: 'Your last wish, punk?'",
"says: 'Your death shall be a slow, painful one.'",
"says: 'Your head shall be my next trophy.'",
"screams: 'You are DOOMED!'",
"grins sadistically.",
"says: 'This dungeon shall be your TOMB!'",
"laughs fiendishly.",
"says: 'Your fate is sealed, worm.'",
"says: 'Resistance is useless.'",
"says: 'Hell shall soon claim your remains.'",
"says: 'Thou shalt repent of thy cunning.'",
"says: 'Verily, thou shalt be one dead cretin.'",
"says: 'Surrender or die!'",
"says: 'Savor thy breath, it be thine last.'",
"says: 'Prepare to die, miscreant!'",
"says: 'You're history, dude!'",
"says: 'Feeling lucky, punk?'",
"says: 'You're toast!'",
"says: 'You're dead meat.'",
"says: 'Make my day.'",
"says: 'I shall flatten you!'",
"says: 'I could spare you, but why?'",
"says: 'Take this, you sissy!'",
"says: 'Nothing can save you now!'",
"says: 'This dungeon ain't big enough for the both of us.'",
"says: 'I'm gonna break your face!'",
"says: 'I hope you enjoy pain!'",
"says: 'Give me your best blow!'",
"says: 'Draw, if you are a man!'",
"says: 'A time to die, fool!'",
"bellows frighteningly!",
"says: 'You will never leave this dungeon alive!'",
"says: 'You'll leave this dungeon only in a wooden box!'",
"says: 'Your mother wears army boots!'",
"says: 'Drop that weapon, NOW!'",
"says: 'Life ain't for you, and I'm the cure!'",
"says: 'Resistance is futile. You will be terminated.'",
"says: 'Sight and smell of this, it gets me going.'",
"says: 'Victim is your name and you shall fall.'",
"says: 'Stepping out? You'll feel our hell on your back!'",
"says: 'Now I will waste my hate on you.'",
"says: 'Don't tread on me!'",
"says: 'So be it! Threaten no more!'",
"says: 'Kill for gain or shoot to maim, but I don't need a reason.'",
"says: 'You'll die as you lived, in a flash of the blade.'",
"says: 'You'd better stand cos there's no turning back.'",
"says: 'I just want to see your blood, I just want to stand and stare.'",
"says: 'I've been looking so long for you; you won't get away from my grasp.'",
"says: 'I'm coming after you; you can kiss your arse good-bye.'",
"says: 'It's official; you suck!'",
"sings: 'I hate you, you hate me, we're a helluva family.'",
"says: 'A mere mortal dares challenge *ME*?!'",
"says: 'There is no escape and that's for sure.'",
"says: 'This is the end; I won't take any more.'",
"says: 'Say good-bye to the world you live in.'",
"says: 'You've always been taking, but now you're giving.'",
"says: 'My brain's on fire with the feeling to kill.'",
"says: 'Don't try running away, because you're the one I'll find.'",
"says: 'I was looking for you to start up a fight.'",
"says: 'My innocent victims are slaughtered with wrath and despise!'",
"says: 'I have found you, and there is no place to run.'",
"says: 'My blood lust defies all my needs.'",
"says: 'And damn'd be him that first cries: Hold, enough!'",
"says: 'I can smell your blood, human!'",
"says: 'Has your folly led to this?'",
"wonders aloud how many experience points you're worth...",
"says: 'Pride yourself on this, that you were slain by a champion.'",
"thunders: 'May heaven have mercy on your soul, for I will have none.'",
"screams for your blood!",
"sighs: 'They send a poorer grade of adventurers down each year than the last.'",
"says: 'Your life-blood will baptise my blade!'",
"shouts: 'You will serve me in Valhalla!'",
"snickers: 'Mommy's not here to save you now!'",
"says: 'You're almost not worth killing... almost!'",
"leaps towards you with death in its eye.",
"sings: 'Cuts yer if ye stand, shoot yer if ye run.'",
"says: 'Another adventurer?  I just got through picking my teeth with the last.'",
"says: 'Your two ears will decorate my belt.'",
"says: 'I love all that blood.'",
"says: 'I don't want to hurt you. I only want to kill you.'",
"says: 'I like killing people, because it's so much fun.'",
"screams: 'I'm out to destroy and I will cut you down!'",
"says: 'Bring it on!'",
"announces: 'You have no chance to survive make your time.'",
"laughs at your feeble character.",
"says, 'Feel my wrath, fool!'",
"says, 'Death and destruction make me happy!'",
"whispers nasty things.",
"says, 'I'll slaughter you slowly...'",
"says, 'Now, you shall taste my wrath!'",
"giggles as it fingers its knife.",
"eyes your money pouch covetously.",
"says, 'You look like Nottingham's man to me!'",
"says, 'I bet I can shoot better than you...'",
"says, 'Give 'til it hurts!'",
"says, 'Don't force me to put an arrow in your skull...'",
"says, 'Kevin Costner has soiled my name!'",
"wonders aloud about the quality of your weapon.",
"spouts torrents of taunts.",
"says, 'Feel lucky, punk?'",
"says, 'I'll teach you to respect me!'",
"says, 'I'll make your short life nasty and brutish!'",
"fingers its blade and grins evilly.",
"snickers, 'Now, I strike a blow for *our* side!'",
"says, 'I don't get no respect... I'm gonna change that!'",
"calls your mother nasty names.",
"shouts 'Yo mama's so fat, she fell off the bed both sides!'",
"says, 'I'll bet your innards would taste real sweet...'",
"belches and spits.",
"scratches its armpits.",
"says, 'I love the smell of fresh blood.'",
"says, 'Yeeha! Another idiot to slaughter!'",
"hawks a loogie in your direction.",
"farts thunderously.",
"wonders aloud how many experience points you're worth.",
"says, 'I love being psychotic!'",
"says, 'My brain's on fire with the feeling to kill!'",
"says, 'I shall torture you slowly.'",
"calls you a scum-sucking pig-dog.",
"says, 'I shall break you!'",
"says, 'You're not so tough, loser!'",
"says, 'Heh-heh, heh-heh, killing people is cool.'",
"screams, 'Hey, I have rights too!'",
"says, 'You're just prejudiced against my kind, aren't you?'",
"says, 'Next time, I'm bringing more Uruks with me!'",
"says, 'I'll mess up all your stuff!'",
"says, 'Give me the Rheingold, or die!'",
"says, 'Come get some!'",

/* ToME devteam quote: "This next may be unnecessarily evil... :-]" */
"magically summons mighty undead opponents!",
"magically summons Cyberdemons!",
"summons special opponents!",

"says: 'All right, you savage, mindless creature. Prepare to meet your equal!'",
"says: 'I will slay you, and hack you, and waste you, and destroy you!'",
"says: 'The night stretches out on the Isengard!'",
"says: 'Trolls are strong, I am STRONGER!'",
"says: 'I am totally of the side of nobody since nobody is totally of my side...'",
"says, '640K should be enough for ANYBODY!'",
"says, 'Buy Windows 2000; the filesystem rocks!'",
"says, 'Linux?  Never heard of it...'",
"says, 'Resistance is futile--you will be assimilated.'",
"says, 'NT is the solution for ALL your needs!'",
"hacks out some code and calls it a Service Pack.",
"says, 'We don't have a monopoly... Mac OS still exists!'",
"wonders if (s)he should buy a small country.",
"says, 'Where will we let you go today?  The Recycle Bin!'",
"says, 'Ho ho ho! You're gonna die!'",
"says, 'You're gettin' COAL in your stocking!'",
"says, 'On Smasher, on Crasher, now dash away all!'",
"says, 'You're on the Naughty List!'",
"says, 'I'll sic my man-eating reindeer on you!'",
"says, 'I hate Christmas so much that I've gone psychotic!'",
"says, 'Repent, evildoer!'",
"says, 'My righteousness shall cleanse you!'",
"says, 'God may love you, but *I* don't!'",
"says, 'I shall smite thee with extreme prejudice!'",
"says, 'Hope you like eternal damnation!'",
"says, 'Verily, it is too late for thee.'",
"barks and bellows frighteningly!",
"says, 'Oh good, another chew toy!'",
"says, 'Yummy! I was getting tired of chicken...'",
"lets out an earsplitting howl.",
"says, 'Bad adventurer! No more living for you!'",
"snarls and howls.",
"brags, 'My power is beyond compare!'",
"snorts, 'A mere mortal dares challenge my might? HA!'",
"says, 'Not another one! I just finished chewing on the last!'",
"wonders aloud how many XP you're worth.",
"leafs through 'Evil Geniuses For Dummies'.",
"mutters, 'Another darn loser to kill...'",
"says, 'Angband shall claim your remains!'",
"says, 'Another 12 skulls and I get that reward from the Boss!'",
"yawns at your pathetic efforts to kill it.",
"says, 'Minions, slaughter this fool!'",
"says, 'Set thine house in order, for thou shalt die...'",
"says, 'I'm no god... God has MERCY!'",
"screams 'I came from the Hells for YOU!'",
"laughs out loudly.",
"screams 'ToME rules!'.",
"screams 'Your code is ugly!'",
"farts in your general direction.",
"tells you to stop acting like a crazy person.",
"flips you the bird.",
"tells you to stop staring at it all the damn time.",
"threatens to report you to the principal.",
"pulls down your trousers.",
"prepares to shoot you with a laser cannon.",
"is charging mana for a devastating attack spell.",
"laughs at your character being so low-leveled.",
"wonders how a woefully underequipped character like yours can even survive.",
"will definitely kill you now, you loser!!!",
"tells you to stop being such a bad player.",
"yawns and mutters, 'Why do those adventurers always have to be such lowly wimps?'",
"pulls an artifact sword and closes in on you!",
"draws a pistol and shouts, 'Stop! In the name of the law!'",
"shouts, 'Police! Don't move! Put your hands in the air!'",
"points a loaded gun at you.",
"says, 'I have you in my crosshairs now!'",
"laughs, 'Haven't been watching your HP, eh? Very well. Now, I will kill you.'",
"announces, 'I'm here because the RNG hath decreed that you've been playing enough for today. Prepare to die,'",
"announces, 'Just wait until I roll a 20 and use my touch of death on you!'",
"laughs, 'Bet your to-hit can't penetrate my AC of -40? Ha ha ha!'",
"says, 'I'm the last enemy you're fighting today... because I will KILL you!'",
"shouts, 'I'm looking forward to kill the poor sap that stumbles upon your bones file after I'm done with you!'",
"says: 'You shall not pass!'",
"says: 'Let's see if you are worthy!'",
"says: 'You are about to die, worm!'",
"says: 'You don't have a chance!'",
"says: 'Feel my fury!'",
"stares at you darkly.",
"says: 'Prepare to meet your destiny!'",
"says: 'Hell hath no fury like mine!'",
"says: 'Your last wish?'",
"says: 'Aye, you will make fine eating, when you have hung a bit.'",
"says: 'Prepare do die!'",
"says: 'You shall be bruised and battered to pieces when I'm through!'",
"says: 'And ever so my foes shall fall!'",
"says: 'Begone now ere my arrows fly!'",
"says: 'Hammer and tongs! Knocker and gongs!'",
"whispers nasty things.",
"says: 'I'll slaughter you slowly...'",
"eyes your money pouch covetously.",
"says: 'I bet I can shoot better than you...'",
"spouts torrents of obscenities.",
"attempts to read a Scroll of Curse Weapon at you!",
"snickers: 'Now, I strike a blow for *our* side!'",
"says: 'I love the smell of fresh blood.'",
"says: 'I shall torture you slowly.'",
"chuckles evilly.",
"says: 'I will prepare something particularly uncomfortable for you.'",
"says: 'I shall smite thee with extreme prejudice!'",
"says: 'Verily, it is too late for thee.'",
"brags: 'My power is beyond compare!'",
"says: 'A mere mortal dares challenge my might?'",
"says: 'Too bad you are unlucky. But even that would not help you now.'",
"sings: 'Swish, smack! Whip crack!'",
"says: 'Think of it this way: you are fated to die here. DIE!'",
"says: 'This is the end; I won't take anymore.'",
"says: 'Say goodbye to the world you live in.'",
"says: 'Your weapon is no match for mine!'",
"brags: 'I'm a friend of the Boss's, I am.'",
"says: 'I can't help but laugh at your pathetic attempts.'",
"lets out a truly awful cry of rage!",
"laughs at your wild swings.",
"says: 'I can smell your blood!'",
"thunders: 'May Eru have mercy on your soul, for I will have none.'",
"shouts: 'You are already dead!'",
"snickers: 'You and what army could harm me?'",
"sings: 'Clash, crash! Crush, smash!'",
"says: 'Consider yourself warned.'",
"says: 'I am fated never to die by the hand of a mortal. Just give up.'",
"says: 'I shall have my way with your women!'",
"says: 'You're not so tough, buttmunch!'",
"says: 'Get away! This spot is mine!'",
"says: 'I will soon close your eyes in eternal sleep.'",
"cries: 'You must pay me... with your life!'",
"says: 'Maybe I will just hack your head off.'",
"says: 'Did you hear what the ravens said? Revenge, that is what they cry!'",
"shouts: 'Hoiho! Hoiho! To arms! To arms!'",
"grumbles: 'I hate the happy, and I am never glad.'",
"cries: 'Keep away from the Ring!'",
"boasts: 'My spear will certainly cut down the wrongful one.'",
"cries: 'There! There shall my spear strike!'",
"grins: 'You will die soon, handsome hero!'",
"states: 'I am but avenging perjury.'",
"shouts: 'Give the Ring here!'",
"shouts: 'Hoiho! Hoiho-hoho!'",
"states: 'As I have renounced love, all who live shall soon renounce it!'",
"laughs insanely.",
"asks: 'Did you hear it? The nibelung hordes are rising from the depths!'",
"laughs: 'Ha ha ha ha! Beware!'",
"says: 'Beware, fool! Beware!'",
"says: 'Envy led you here, pitiful rogue!'",
"boasts: 'I dauntlessly defy everyone, everyone!'",
"yells: 'Tremble, on your knees before the master of the Ring.'",
"yells: 'Tremble with terror, abject throng!'",
"says: 'I am watching you everywhere, expect me where you do not perceive me!'",
"says: 'Feel my wrath, idle rascal!'",
"says: 'I have discovered you, you stupid thief!'",
"says: 'Are you still not afraid? You should be!'",
"says: 'I should really be shimmering!'",
"says: 'Open the gates of my hell, I will strike from the grave!'",
"complains: 'What's a burrahobbit got to do with my pocket, anyways?'",
"rejoices: 'No more roast mutton! Roast adventurer today!'",
"says: 'That'll teach yer!'",
"says: 'I won't take that from you!'",
"grumbles: 'Stop, greedy one! Leave something for me!'",
"shouts: 'Back, over-bold one!'",
"whines: 'You swindler, do you seek to vilify me?'",
"shouts: 'All your fault!'",
"shouts: 'Only your death will set things right!'",
"says: 'Don't call me clumsy!'",
"says: 'You smile at your own cowardice? Stand and fight, bastard!'",
"says: 'You aren't a caitiff that throws a cloak?!'",
"says: 'Now we shall resolve a matter which has long troubled me.'",
"cries: 'Coward! Is this the great warrior of whom I have heard so much?'",
"enjoys espresso coffee between battle.",
"says: 'Excuse me, I have no time to continue a small fight.'",
"says: 'It is a great pleasure to fight with such a worthy opponent as you.'",
"says: 'I will turn you into a coatrack.'",
"says: 'I'm a master of the Keep of the Four Worlds!'",
"says: 'Order will crumble.'",
"says: 'Obey the power of chaos!'",
"hisses: 'Your crime issss life. The sssentencce isss death.'",
"says: 'Freezer? Glad you asked! A box to store your body!'",
"says: 'If you buy a Grand-D machine, I'll throw Werewindle into the bargain.'",
"has an eye to stick you with a burial set.",
"says: 'Sorry but I kill you.'",
"says: 'Can I get you a chicken? Maybe some white mice and aluminum foil?'",
"says: 'Be made to realize my darkness hold.'",
"says: 'Power of shadow is infinite.'",
"says: 'With the Key of Kolwynia, I am invincible!'",
"says: 'Soul? What is it of use to?'",
"speaks: 'I smell you and I feel your air. I hear your breath. Come along!'",
"says: 'If you get off alive, you will be lucky.'",
"grimaces.",
"laughs with a devastating sound which shakes the ground.",
"asks: 'Where are those who dare approach me?'",
"gloats: 'I am old and strong, strong, strong.'",
"boasts: 'My armour is like tenfold shields, no blade can pierce me.'",
"boasts: 'My teeth are swords, my claws are spears, my breath is death.'",
"says: 'You will make a fine meal.'",
"says: 'I wanted a drink, now I have also found food.'",
"says: 'My fangs are not for chattering, soon you will feel them.'",
"says: 'My throat is well made to gulp you down.'",
"growls: 'Come here, young braggart.'",
"says: 'You cannot avoid the ballyhack.'",
"says: 'A mere mortal, Be burned to the ground by my fire.'",
"says: 'Against the power of Mordor there can be no victory.'",
"says: 'Time? What time do you think we have?'",
"says: 'Many that live deserve death. Some that die deserve life.'",
"says: 'I don't endure your sabotaging my plan!'",
"smiles diabolic.",
"dances apocalyptic.",
"says: 'Back Down. This situation only results in my victory, I know.'",
"says: 'I praise your bravery. But take good care of your life.'",
"says: 'It's unlucky for you to see me.'",
"says: 'Burn out! And turn to dust!'",
"says: 'The level 14 valkyrie who came before was storonger than you!'",
"says: 'The level 14 samurai who came before was fastorer than you!'",
"slurps and gibbers disgustingly.",
"oozes nasty, glistening slime all over the dungeon.",
"lets off a mind-numbing stench.",
"howls: 'Tekeli-li!  Tekeli-li!'",
"makes a chilling slithering sound.",
"howls: 'The OTHER GODS will feast on your brain!'",
"hisses: 'I'll feed you to the Hounds of Tindalos...'",
"hisses: 'Randolph Carter got off easy; you won't!'",
"seethes and fumes sickeningly.",
"hisses: 'I'll send you beyond Known Space to Azathoth!'",
"waves nasty-looking tentacles about.",
"picks its teeth with the bones of former players.",
"opens your mind to a vista of nameless cosmic horror!",
"opens your mind to a vista of endless 'Three's Company' reruns!",
"snorts and slobbers with glee.",
"states: 'The time has come.'",
"states: 'You will fall into my power, you will remain my slave!'",
"cries: 'Halt! I have the right weapon to fell you!'",
"yells: 'I will cut you down with your master's spear!'",
"says: 'Foolish worm, you are DOOMED!'",
"says: 'I'm the Big Bad Guy, and you're toast!'",
"shouts: 'MOO HA HA HA! I am DEATH incarnate!'",
"says: 'Prepare for your untimely demise!'",
"opens up a can of Whoop-Ass (tm).",
"picks its teeth with former adventurers' bones.",
"says: 'Maybe I won't kill you... NOT!'",
"yawns at your pathetic efforts to kill it.",
"says: 'Another day, another bastard to slaughter...'",
"says: 'I can't be bothered... minions, slaughter this fool!'",
"says: 'Such a doomed, pathetic gesture as yours verges on the heroic!'",
"says: 'Mere mortals such as you should not meddle the affair of the Powers!'",
"shouts: 'You cannot escape!'",
"says: 'You are checkmated!'",
"says: 'Huh! Come on!'",
"shouts: 'Weak! Weak!'",
"says: 'I announce beforehand that I kill you by sucking your blood!'",
"says: 'Your slow sleepy speed never kill me!'",
"says: 'You came here to die in vain!'",
"says: 'Count 3, then you die.'",
"says: 'I came back to kill you.'",
"shouts: 'AAATATATATATATATATATATATATATATATA!!!!'",
"says: 'You're already dead.'",
"shouts: 'Remember! Curiosity killed the cat!'",
"says: 'Get lost, you can't compare with my powers!'",
"says: 'Anyone who opposes me will be destroyed!'",
"says: 'Allow me to usher you to the sulfurous pits of hell!'",
"says: 'You were a fool for thinking that your showy moves would work on me!'",
"takes out its 5d5 longsword and says, 'No more Mr. Nice Guy!'",
"says: 'If you're going to cry, just go play Vanilla!'",
"says: 'Game balance? What is this ... game ... balance?'",
"wonders why monsters only get 4 blows when you get 16.",
"is ticked off!",
"laughs: 'My special dice always rolls a 20. I never miss.'",
"wonders if your to-hit rating is even capable of hitting anything at all.",
"laughs: 'You grinded 20 hours for those stats only to die to me now?'",
"laughs: 'Your stats are awfully bad! Did you not do any grinding? Loser.'",
"challenges you to a duel on life and death.",
"asks whether you know what you're getting into.",
"closes in on you to attack.",
"shouts 'Look at yourself, you're much too skinny! Why don't you eat?'",
"remarks that you need something to eat or you'll starve.",
"announces 'You are going down.'",
"says 'I learned to defend myself.'",
"threatens you with its very presence.",
"closes in on you while readying its weapons and spells.",
"screams 'If you kill me, I'll come back stronger than before!'",
"announces 'I hope for a fair battle.'",
"shouts 'Allah Akbar!'",
"shouts 'I will extract so much of your blood!'",
"announces 'You'll bleed for hours before I finish you off.'",
"wonders how long a weakling like you can withstand her might.",
"announces 'I'm gonna smash your toes.'",
"says 'I'm looking forward to having fun with you.'",
"threatens to crush you.",
"announces 'If you don't flee, I'll drive you several feet into the ground.'",
"says 'Any adventurer who dares to fight me will be annihilated.'",
"calls for a squad of minions and instructs them to take away all your food.",
"laughs 'You cannot defeat me. Ha ha ha.'",
"announces 'Pay 10 euros to me or you will be terminated.'",
"says 'Work hard if you don't want to anger me.'",
"eyes you suspiciously.",
"strides toward you with determination in its face.",
"announces 'If you touch me I'll make your hands bleed.'",
"says 'You have no right to be here. Now I will eliminate you.'",
"gives you a mean look.",
"is ready to kick your butt, you wimpy 'adventurer'!",
"announces 'Abide by my rules or you will be punished.'",
"says 'Adventurers are not permitted on this dungeon level.'",
"says 'You'll pay a fine of 2 euros for trespassing here.'",
"wants to demonstrate its kung-fu abilities to you.",
"calls for its fellow assistants.",
"announces 'Pay 100 euros immediately or I will kill you!'",
"shouts 'I'm not here to talk, I'm here to terminate your sorry existence!'",
"shouts 'I have friends in powerful positions who will fuck you up!'",
"points at you and screams 'Avada Kedavra!'",
"laughs 'I'll revive anyway. Every time you kill me your fine will increase by 200 euros.'",
"giggles 'I'll use the turbo power booster.'",
"boasts 'I own a helicopter!'",
"says 'I'm beautiful! Wouldn't you agree?'",
"regards you as a criminal and will wipe you off the face of the earth.",
"gets ready to fight you.",

};

static NEARDATA const char * const fake_plines[] = {
	"Demogorgon suddenly disappears!",
	"You are slowing down.",
	"Your limbs are stiffening.",
	"Do you want your possessions identified? DYWYPI?",
	"Warning: Low Local Memory. Freeing description strings.",
	"Not enough memory to create inventory window",
	"Your potion of full healing boils and explodes!",
	"Your potion of amnesia suddenly explodes! You feel dizzy!",
	"Your scroll of genocide catches fire and burns!",
	"Your 10 scrolls of teleportation suddenly crumble to dust.",
	"Your ring of free action turns to dust and vanishes!",
	"Your ring of conflict evaporates!",
	"Your wand of death (0:8) suddenly explodes!",
	"Your wand of wishing seems less effective.",
	"Your wand of bubblebeam explodes! Water washes over you!",
	"One of your objects was destroyed by rust!",
	"One of your objects was vaporized!",
	"Your weapon withered away!",
	"Your weapon disintegrates!",
	"Your equipment is teleported out of your inventory!",
	"It summons insects!",
	"It summons nymphs!",
	"It opens a gate!",
	"You hear a goblin reading a scroll of demonology. The inhabitants of the Elemental Planes appear!",
	"You hear a gnome reading a scroll of wishing.",
	"The soldier ant stings you! The poison was deadly...",
	"The soldier ant stings you! You feel weaker! You lose  Strength",
	"The killer bee stings you! You are bleeding out from your stinging injury!",
	"It breathes --More-- You have died.",
	"It breathes a disintegration blast!",
	"You feel weaker!",
	"You undergo a freakish metamorphosis!",
	"You feel a little strange.",
	"You feel a little normal.",
	"Your armor turns to dust and is blown away!",
	"Your cloak of magic resistance disintegrates!",
	"Your enchanted hawaiian shirt crumbles into tiny threads and is blown away!",
	"It stole an amulet of reflection.",
	"The water nymph stole an amulet of life saving!",
	"The buzzing nymph stole a wand of death (0:8)!",
	"The gnome thief stole a cloak of magic resistance!",
	"The sucker patch stole a ring of free action!",
	"It summons demon lords!",
	"The Wizard of Yendor casts a spell at you!",
	"The arch-lich casts a spell!",
	"Death reaches out with his deadly touch...",
	"It reaches out, and you feel fever and chills. You feel deathly sick.",
	"A Master Archon suddenly appears!",
	"The gnome lord turns into a migmatite horror!",
	"Oh no, it uses the touch of death!",
	"You feel deathly sick.",
	"You don't feel very well.",
	"You hear the cockatrice's hissing!",
	"A little dart shoots out at you! The dart was poisoned!",
	"A shuriken shoots out of you! The shuriken was poisoned! The poison was deadly...",
	"You hear a clank.",
	"Oh wow! Everything looks so cosmic!",
	"You stagger and your vision blurrs...",
	"Eating that is instantly fatal.",
	"Falling, you touch the cockatrice corpse.",
	"It seduces you and you start taking off your boots of levitation.",
	"A wide-angle disintegration beam hits you!",
	"A lava bolt hits you! Your shield burns to a cinder!",
	"An ice bolt hits you! Your crystal plate mail shatters into a thousand pieces!",
	"A thunderbolt hits you! Your amulet of life saving disintegrates!",
	"A poison bolt hits you! Your 5 potions of full healing turn into 5 potions of sickness!",
	"A water bolt hits you! Your gray dragon egg (laid by you) breaks!",
	"A plasma bolt hits you! Your cloak of magic resistance burns to ashes!",
	"A chaos bolt hits you! Your bag of holding suddenly explodes!",
	"A nether bolt hits you! Goodbye level 14. Goodbye level 13. You lose the ability to perform appraisal!",
	"An acid bolt hits you! Your scroll of teleportation is destroyed! Your helmet of telepathy is damaged!",
	"A sound bolt hits you! Your 2 potions of gain level break and shatter!",
	"A shards bolt hits you! You have sustained a mortal wound. You hear the wailing of the Banshee...",
	"A disenchantment bolt hits you! Your Grayswandir seems less effective.",
	"A mana bolt hits you! All of your items have been destroyed!",
	"A nexus bolt hits you! Suddenly you are not holding Excalibur anymore.",
	"You die...",
	"***LOW HITPOINT WARNING***",
	"You hear the wailing of the Banshee...",
	"Wizard is about to die.",
	"Oops... Suddenly, the dungeon collapses.",
	"You turn to stone.",
	"You are a statue.",
	"You have become a green slime.",
	"Your skin begins to peel away.",
	"You die of starvation.",
	"You are gasping for air.",
	"You suffocate.",
	"You faint from lack of food.",
	"You can't move!",
	"You fall asleep!",
	"You suddenly feel very tired!",
	"You are mercilessly tickled by the byakhee!",
	"You suddenly feel weaker! 10 points of strength got sapped!",
	"You float into the air!",
	"You float down. You fell into the water! A potion of acid explodes!",
	"Wait! The door really was an intrinsic eating mimic!",
	"Wait! That's a disenchanter mimic! Your weapon seems less effective.",
	"Your weapon seems less effective.",
	"Your knapsack seems less effective.",
	"Your Stormbringer was disenchanted!",
	"Remember - ascension is impossible!",
	"You don't stand a chance!",
	"You got no chance! No chance in hell!",
	"Weaklings like you won't ever ascend, so don't even think you can do it!",
	"YASD is waiting around the corner with big, pointy teeth!",
	"You have 35 turns to live.",
	"If you don't ascend within the next 10000 turns, you automatically die.",
	"Your last thought fades away.",
	"You feel dead inside.",
	"Unfortunately, you're still genocided.",
	"Blecch! Rotten food!",
	"Ecch - that must have been poisonous!",
	"Ulch - that fungoid vegetation was tainted!",
	"You get a strong feeling that the gods are angry...",
	"The voice of Moloch roars: 'Destroy him, my servants!'",
	"Asidonhopo hits you! Asidonhopo hits you! Asidonhopo hits you! You die...",
	"Vanzac screams: 'How dare you break my door?' Vanzac zaps a hexagonal wand!",
	"The gnome zaps a wand of death!",
	"The soldier zaps a wand of remove resistance!",
	"The giant spider zaps a wand of banishment!",
	"The black dragon breathes a disintegration blast!",
	"The giant eel drowns you... Do you want your possessions identified? DYWYPI? [ynq] (n) _",
	"Really quit [yes/no]? _",
	"Suddenly, one of the Vault's guards enters!",
	"You fall into the lava! You burn to a crisp.",
	"You die from your illness.",
	"The newt bites you! You die...",
	"You hit the floating eye! You are frozen by the floating eye!",
	"The porter lethe vortex engulfs you!",
	"The Banisher hits you!",
	"You feel the waters of the Lethe sweeping away your cares...",
	"The mind flayer's tentacles suck you! Your brain is eaten! You feel very stupid!",
	"The psych flayer blasts you! You feel very stupid! Everything suddenly goes dark.",
	"The disenchanter claws you! Your silver dragon scale mail seems less effective.",
	"Your silver saber named Grayswandir glows violently for a while and then evaporates.",
	"You try to attack the wood nymph. The wood nymph quickly snatches your weapon!",
	"Grateful for her release, she grants you a wish! For what do you wish?",
	"The djinni speaks. 'You disturbed me, fool!'",
	"An enormous ghost appears next to you! You are frightened to death, and unable to move.",
	"The priest intones: 'Pilgrim, you enter a desecrated place!'",
	"h - a cursed -5 unicorn horn.",
	"The troll giant rises from the dead!",
	"The barbarian named Mike the Raider wields a long sword named Vorpal Blade!",
	"One-Eyed Sam swings his long sword named Thiefbane. One-Eyed Sam hits you!",
	"Ashikaga Takauji picks up a tsurugi named The Tsurugi of Muramasa.",
	"The kobold throws 5 darts of disintegration!",
	"The soldier fires 3 antimatter bullets! The 1st antimatter bullet hits you! You die...",
	"The maud agent hurls a sparkling potion!",
	"The bone devil zaps a wand of create horde!",
	"The bolt of fire bounces! The bolt of fire hits you! The ice underneath you melts!",
	"You irradiate yourself with pure energy. You die...",
	"You're confused!",
	"You just blinded yourself!",
	"The thin crust of rock gives way, revealing lava underneath!",
	"CLICK! You have triggered a trap!",
	"Oops, you have triggered a trap.",
	"A trap has been sprung!",
	"You have sprung a trap!",
	"You trigger a death trap! You die... Do you want your possessions identified?",
	"All your possessions have been stolen!",
	"The beautiful water nymph seduces you and you start taking off your cloak.",
	"The blast of disintegration hits you!",
	/* The statu */ "es come to life!",
	/* The statues */ "come to life!",
	/* The Smirking Sne */ "ak Thief is dead!",
	/* You impale th */ "e Pale Wraith with a lightning stroke.",
	/* Undistributed points a */ "vailable, exit anyway?",
	/* This is a fake messa */ "ge broadcast by Arabella to confuse unwary players.",
	/* Castle of the Winds will only r */ "un in Protect mode",
	/* pul */ "verisiert!",
	/* Not eno */ "ugh memory to create inventory window",
	/* Com */ "mand Pending: Magic Spell...",
	/* integer div */ "ide by 0",
	/* Jetzt wirds sc */ "hwer!",
	/* Surtur ca */ "lls lightning down on you!",
	/* a rude str */ "aw mattress",
	"Arabella began to build a world miracle!", 
	"Arabella finished building a world miracle. You will lose if it exists for 2000 turns!",
	"Arabella launched a SCUD Storm!",
	"Your head is sucked off! You die...",
	"You choke over your food. You die...",
	"You bite the gnome lord. You feed on the lifeblood. You choke over it.",
	"You fall into a pit! You land on a set of sharp iron spikes! The spikes were poisoned!",
	"You triggered a teleporting items trap!",
	"You feel life has clocked back.",
	"Your stats got scrambled!",
	"All of your stats got reduced!",
	"Your maximum health was damaged!",
	"Your maximum health and mana was damaged!",
	"You feel less protected!",
	"You lose all divine protection!",
	"You feel the need to pull down your trousers.",
	"You suddenly see a beautiful woman. She's so sexy that you spontaneously come.",
	"You grow a strange desire to start wanking off spontaneously.",
	"You suddenly need to take a crap, and are immobilized while you undress.",
	"The gnome puts a wand of cancellation into your bag of holding!",
	"As you put a wand of cancellation inside, your bag named sack suddenly explodes!",
	"As you put the bag of holding into the bag of holding, it explodes!",
	"Your bag of holding suddenly explodes!",
	"You are hit by a lethe bolt! Suddenly, your bag of holding transforms into a large box!",
	"You are hit by a destruction bolt! Your gauntlets of power vanish!",
	"You triggered a bisection trap! The razor-sharp blade cuts you in half! You die...",
	"You triggered a Medusa trap! You turn to stone. Do you want your possessions identified?",
	"KABOOM! The door was booby-trapped! You stagger...",
	"KAABLAMM! You triggered a land mine!",
	"KABOOM! You triggered a bomb! You die...",
	"It misses.",
	"Its tentacles suck you!",
	"It lashes you! Your head spins in confusion.",
	"It hits! [-35 -> 1] Warning: HP low!",
	"You hear a flinging sound. You are hit by an arrow!",
	"You hear a spitting sound. You are hit by a segfault venom!--More-- Oops...",
	"Jubilex spits a segfault venom!--More-- Suddenly, the dungeon collapses.",
	"The manticore spits a disintegration venom! Your shield of reflection turns to dust and vanishes!",
	"A boss monster suddenly appears!",
	"It summons demons!",
	"It summons demon princes!",
	"It summons something invisible!",
	"It summons the Wizard of Yendor!",
	"The marilith opens a gate, and Demogorgon appears!",
	"You slow down to a crawl.",
	"You slow down.",
	"You slow down even more.",
	"You are prevented from having reflection!",
	"You are prevented from having magic resistance!",
	"You are prevented from having flying! You fall into the lava! You burn to a crisp.",
	"You feel your life draining away!",
	"You feel weaker!",
	"You feel clumsy!",
	"You feel stupid!",
	"You feel foolish!",
	"You feel very sick!",
	"You feel frailer!",
	"You feel fralier!",
	"You break out in hives!",
	"All your stats have been drained!",
	"You're not as agile as you used to be...",
	"You dream that you hear the howling of the CwnAnnwn...",
	"Rien ne va plus...",
	"SLASHEM.EXE has stopped working.",
	"nt|| - Not a valid save file",
	"NETHACK.EXE caused a General Protection Fault at address 000D:001D.",
	"APPLICATION ERROR - integer divide by 0",
	"Runtime error! Program: NETHACK.EXE",
	"R6025 - pure virtual function call",
	"Buffer overrun detected! Program: NETHACK.EXE",
	"A buffer overrun has been detected which has corrupted the program's internal state. The program cannot safely continue execution and must now be terminated.",
	"This application has requested the Runtime to terminate it in an unusual way. Please contact the application's support team for more information.",
	"Not enough memory to create inventory window",
	"Error: Nethack will only run in Protect mode",
	"ERROR: SIGNAL 11 WAS RAISED",
	"Not enough memory to initialize NetHack",
	"UNHANDLED EXCEPTION: ACCESS_VIOLATION (C0000005)",
	"An error has occurred in your application. If you choose Close, your application will be terminated. If you choose Ignore, you should save your work in a new file.",
	"Windows Subsystem service has stopped unexpectedly.",
	"nv4_disp.dll device driver is stuck in an infinite loop.",
	"Please wait while Bandarchor encrypts your personal data.",
	"Downloading the newest ransomware...",
	"File encryption complete! For recovery, you need to pay 2.5 bitcoins to the following wallet:--More--",
	"Your SLASH'EM account files are encrypted! The only way to get the decryption key is to ascend in this game, otherwise all your files are irreversibly deleted!",
	"WARNING: If you do not ascend in this game, all future games played under your account will start with all stats reduced by 10.",
	"WARNING: Moloch's Sanctum becomes permanently inaccessible in 3456 turns.",
	"Attention! If you do not reach the Astral Plane in 20000 turns, all the Riders become covetous and gain the ability to clone themselves!",
	"You hear a nearby zap. You feel dizzy!",
	"The goblin zaps a wand of disintegration! Your cloak of magic resistance is blown away!",
	"The ogre reads a scroll of destroy armor! Your gloves vanish!",
	"The newt reads a scroll of destroy weapon! Your long sword named Excalibur disintegrates!",
	"You feel the air around you grow charged...",
	"You feel very jumpy.",
	"Got aggravate monster!",
	"Got polymorphitis!",
	"You're insane if you think you can ascend!",
	"Message to the person sitting in front of the monitor: you will not escape this time.",
	"Message to the person sitting in front of the monitor: you are going down.",
	"This is Arabella speaking to you, player. Know this: I put nasty traps on every dungeon level and you will not be able to avoid all of them.",
	"This is Arabella speaking to you, player. Know this: Some parts of the dungeon have been designed by me specially to make sure you won't win the game.",
	"This is Arabella speaking to you, player. Know this: Marc is the only one who ever managed to defeat me in combat. You will not change that fact.",
	"This is Arabella speaking to you, player. Know this: Even if you're not a Spacewars Fighter or Camperstriker, I can still turn your life into hell.",
	"This is Arabella speaking to you, player. Know this: I own the Bank of Yendor and I decided not to give you back any money you deposit.",
	"This is Arabella speaking to you, player. Know this: If you like this game, consider playing the latest up-to-date version.",
	"This is Arabella speaking to you, player. Know this: If you lose to me, you have to clean the dog shit off of my profiled boots.",
	"This is Arabella speaking to you, player. Know this: I'm working hard to invent more nasty traps with which to obstruct the players.",
	"This is Arabella speaking to you, player. Know this: I like you, but not so much that I would allow you to win the game.",
	"This is Arabella speaking to you, player. Know this: My traps don't reveal themselves and display no message even if you trigger them.",
	"This is Arabella speaking to you, player. Know this: You are not Marc, so you cannot even hope to defeat me and win the game.",
	"This is Arabella speaking to you, player. Know this: My Asian girl boots will break your legs as soon as your character dies.",
	"This is Arabella speaking to you, player. Know this: I instructed my friend Anastasia to suck your head off with her vacuum cleaner.",
	"Incoming message for Player 1. The message is from 'Marc'. It reads: --More--",
	"Incoming message for Player 1. The message is from 'Sizzle'. It reads: 'Roger that.'",
	"Incoming message for Player 1. The message is from 'Lisa'. It reads: 'Please don't interrupt me, I'm playing a game in harder than hard mode. Unlike your game, mine actually requires skill.'",
	"Incoming message for Player 1. The message is from <sender name is illegible>. It reads: 'I am the world champion of SPACE WARS but I'm not playing anymore for fear of losing my title.'",
	"Incoming message for Player 1. The message is from 'Anastasia'. It reads: 'Sorry, but I have to kill you with my vacuum cleaner now.'",
	"Incoming message for Player 1. The message is from 'Henrietta'. It reads: 'Besos y fuderes, Henrietta, xxx. PS: GO TO HELL!!!'",
	"Incoming message for Player 1. The message is from 'Katrin'. It reads: 'Do you like it if my purple high heels dance on your toes?'",
	"Incoming message for Player 1. The message is from 'Jana'. It reads: 'You won't be able to beat the game unless you kill me, and I'm hiding on dungeon level 71. You will never even find me.'",
	"Incoming message for Player 1. The message is from 'Vlad the Impaler'. It reads: 'Nyah-nyah, I bet you'll never find out which dungeon level I teleported to! Nyah-nyah!'",
	"Incoming message for Player 1. The message is from 'Wizard of Yendor'. It reads: 'Fear my wrath, ass-fucker!'",
	"Incoming message for Player 1. The message is from 'Wizard of Yendor'. It reads: 'Verily, thou shalt be one dead piece of shit!'",
	"Incoming message for Player 1. The message is from 'Wizard of Yendor'. It reads: 'I will eat thee alive, startscummer!'",
	"Incoming message for Player 1. The message is from 'Wizard of Yendor'. It reads: 'Thine carcass shalt burn, pudding farmer!'",
	"Incoming message for Player 1. The message is from 'Wizard of Yendor'. It reads: 'Thou art doomed, cunt!'",
	"Incoming message for Player 1. The message is from 'Wizard of Yendor'. It reads: 'Hell shall soon claim thy remains, gay fag!'",
	"Incoming message for Player 1. The message is from 'Wizard of Yendor'. It reads: 'Even now thy life force ebbs, dirtbag!'",
	"Incoming message for Player 1. The message is from 'Irina'. It reads: 'Failed existences are those who failed in my test. They are not admitted into my exclusive circle.'",
	"Incoming message for Player 1. The message is from 'Kylie Lum'. It reads: 'Apply as a topmodel and join my show! If you're good then I'll make you famous!'",
	"Incoming message for Player 1. The message is from 'Bluescreen'. It reads: 'Thank you for playing SLASH'EM Extended!'",
	"Incoming message for Player 1. The message is from 'Bluescreen'. It reads: 'Need help? Come join me in IRC channel #slashemextended!'",
	"Incoming message for Player 1. The message is from 'Bluescreen'. It reads: 'When starting a new game, please always use the latest version. Older ones are bugged and will eventually be removed from the server.'",
	"Incoming message for Player 1. The message is from 'Famine'. It reads: 'I am waiting to eat you on the Astral Plane, adventurer.'",
	"Incoming message for Player 1. The message is from 'Death'. It reads: 'Face me on the Astral Plane if you're a man.'",
	"Incoming message for Player 1. The message is from 'Pestilence'. It reads: 'I will make sure you never reach the High Altar on the Astral Plane.'",
	"Incoming message for Player 1. The message is from 'Moloch'. It reads: 'Thou shalt suffer, infidel!'",
	"Incoming message for Player 1. The message is from 'High Priest of Moloch'. It reads: 'Infidel, you have entered Moloch's Sanctum! Be gone!'",
	"Incoming message for Player 1. The message is from 'Vecna'. It reads: 'I live, and if you decide to fight me then you will join my legion of undead!'",
	"Incoming message for Player 1. The message is from 'Vecna'. It reads: 'The warmth of life has entered my tomb. Prepare yourself, mortal, to serve me for eternity!'",
	"Incoming message for Player 1. The message is from 'Vecna'. It reads: 'Abandon your foolish quest! All that awaits you is the wrath of my master! You are too late to save the child. Now you'll join him - in hell.'",
	"Incoming message for Player 1. The message is from 'Miriam'. It reads: 'I would loooooooooooooove to stomp your sweet, unprotected toes with my high-heeled combat boots.'",
	"Incoming message for Player 1. The message is from 'The Prisoner'. It reads: 'SOS! Please, someone help me! I'm being tortured one level down from your location!'",
	"Incoming message for Player 1. The message is from 'Lord Surtur'. It reads: 'You won't defeat me. I'm covetous, so I can just warp away if I'm low on health.'",
	"Incoming message for Player 1. The message is from 'Final Boss'. It reads: 'oremoR nhoJ em llik tsum uoy emag eht niw oT'",
	"Incoming message for Player 1. The message is from 'Pete'. It reads: 'I am the beeeeeeeest!'",
	"Incoming message for Player 1. The message is from 'G-cheater'. It reads: 'Yaaaaaaaaaaaay!!!'",
	"Incoming message for Player 1. The message is from 'Gallardo'. It reads: 'There is a story about me written by Amy, but I just can't remember what it was called...'",
	"Incoming message for Player 1. The message is from 'Ella'. It reads: 'Keep your hands off me or I'll kick them until you start bleeding.'",
	"Incoming message for Player 1. The message is from 'Jil'. It reads: 'Stop trying to tell me that my socks stink, you liar! They emit a beguiling aroma!'",
	"Incoming message for Player 1. The message is from 'Katharina'. It reads: 'Please use your hands to feel up my butt cheeks.'",
	"Incoming message for Player 1. The message is from 'Jessica'. It reads: 'You need to sleep now, my dear. Good night.'",
	"Incoming message for Player 1. The message is from 'H.'. It reads: 'Just wait, one day the almighty God will avenge me and exact punishment on you, and then you're dead, asshole!'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'Ey yo you little-penis-wanker, you ain't getting in. Yo just fuck yourself or I'll put you on ice ey.'",
	"Incoming message for Player 1. The message is from 'Max'. It reads: 'Fuck you all!'",
	"Incoming message for Player 1. The message is from 'Larissa'. It reads: 'My shoe size is 36.'",
	"Incoming message for Player 1. The message is from 'Mailie'. It reads: 'Dear player, please just let me learn for tomorrow's math exam. That is, unless you want to help me understand the formulas, of course.'",
	"Incoming message for Player 1. The message is from 'Aee'. It reads: 'Huh? What?'",
	"Incoming message for Player 1. The message is from 'David'. It reads: 'Sorry for calling you a gay fag, I didn't mean it. Please forgive me!'",
	"Incoming message for Player 1. The message is from 'Jane'. It reads: 'How the heck do you know who Rullus is?'",
	"Incoming message for Player 1. The message is from 'Sophie'. It reads: 'Today I'm wearing my block-heeled combat boots, which happens only rarely! I put them on specially for you because you like them so much!'",
	"Incoming message for Player 1. The message is from 'Almuth'. It reads: 'I won the Scheffel and Apollinaire prizes because I'm simply the best!'",
	"Incoming message for Player 1. The message is from 'Rita'. It reads: 'I finished you off.'",
	"Incoming message for Player 1. The message is from 'Sven'. It reads: 'I will kill you know, and then I'll seize world domination.'",
	"Incoming message for Player 1. The message is from 'Meltem'. It reads: 'Do you also believe in Allah? If not, you should convert so your soul can be saved.'",
	"Incoming message for Player 1. The message is from 'Minnie Mouse'. It reads: 'I bludgeoned Big Bad Pete with my pink high-heeled leather pumps, and now I will do the same to you.'",
	"Incoming message for Player 1. The message is from 'Glittering Goldie'. It reads: 'I loved the facial expression of Scrooge when I forced him to walk on nails bare-footed!'",
	"Incoming message for Player 1. The message is from 'The Joker'. It reads: 'Are you ready, Batman? I've prepared some particularly nasty surprises for you!'",
	"Incoming message for Player 1. The message is from 'Poison Ivy'. It reads: 'The Human World does not understand the feelings of the Plant World. So I teach you!'",
	"Incoming message for Player 1. The message is from 'The Riddler'. It reads: 'I captured Police Chief Barbara and her assistant. If you ever want them liberated, you need to partake in my Maze of the Minotaur simulation program.'",
	"Incoming message for Player 1. The message is from 'Catwoman'. It reads: 'Do not get into my way!'",
	"Incoming message for Player 1. The message is from 'Two-Face'. It reads: 'Alright, you mugs! Let's go!'",
	"Incoming message for Player 1. The message is from 'Eveline'. It reads: 'I can whack you with my wedge sandals all day if you like.'",
	"Incoming message for Player 1. The message is from 'Magdalena'. It reads: 'Just hear yourself talk. You don't even believe your own bullshit, so stop trying to convince me!'",
	"Incoming message for Player 1. The message is from 'Len-kind'. It reads: 'Intruder alert. Apprehend immediately. Terminal force authorized.'",
	"Incoming message for Player 1. The message is from 'Sue Lyn'. It reads: 'Sorry, but I'm very sadistic. Try not to cry out in pain while I abuse you.'",
	"Incoming message for Player 1. The message is from 'Marihuana'. It reads: 'You are eliminated right now! In my function as principal of this school, I hereby prohibit you from ever coming closer than 5 miles to my school! Now GET OUT OF HERE BEFORE I GET ANGRY!'",
	"Incoming message for Player 1. The message is from 'James Fahrtum'. It reads: 'Sure, you may partake in my sports lesson if you want, as long as you don't sexually harass my girls.'",
	"Incoming message for Player 1. The message is from 'Miss Youngdragon'. It reads: 'What do you think you're doing here??? You'll get two hours of detention for that!'",
	"Incoming message for Player 1. The message is from 'Alice'. It reads: 'No, you cannot go up now. You're too young and so you stay right where you are.'",
	"Incoming message for Player 1. The message is from 'Harro'. It reads: 'You cannot sit on the stairs! I will not allow it, because that's my rule!'",
	"Incoming message for Player 1. The message is from 'Mister Head'. It reads: 'If you don't have a booklet next lesson, your mark (which is currently very good) will transform into a very bad one.'",
	"Incoming message for Player 1. The message is from 'Miss Schrack'. It reads: 'Now it's my turn to punish you. I'll pull down my trousers and produce very tender farting noises with my soft butt, which is looking totally sexy by the way.'",
	"Incoming message for Player 1. The message is from 'Pedalas'. It reads: 'You don't know that Russia is a GUS state? Bah, you don't know nothing at all!'",
	"Incoming message for Player 1. The message is from 'Jenni'. It reads: 'Yes, you are supposed to know when, why and how Martin Luther was sentenced to death. You failed your test. Come again in a week.'",
	"Incoming message for Player 1. The message is from 'Rejda'. It reads: 'Sorry, I can't take you as a partner because I already have someone else.'",
	"Incoming message for Player 1. The message is from 'Marike'. It reads: 'Do you like my long, pointy fingernails? They're great for cutting your soft flesh.'",
	"Incoming message for Player 1. The message is from 'Eva'. It reads: 'I'm almost out of cigarettes again.'",
	"Incoming message for Player 1. The message is from 'Kati'. It reads: 'You know that's most unfair, right?'",
	"Incoming message for Player 1. The message is from 'Friederike'. It reads: 'So, do you like my dancing shoes? Come on, admit that they're very soft and you'd like to stroke them.'",
	"Incoming message for Player 1. The message is from 'Thai'. It reads: 'If I kick you, you'll end up in the hospital.'",
	"Incoming message for Player 1. The message is from 'Peeta'. It reads: 'em elleteret end don no how tu wrait'",
	"Incoming message for Player 1. The message is from 'Draco Malfoy'. It reads: 'Death to mudbloods!'",
	"Incoming message for Player 1. The message is from 'Sauron'. It reads: 'As long as I, the Lord of the Rings, live, the Nazgul cannot ever be killed.'",
	"Incoming message for Player 1. The message is from 'Bellatrix Lestrange'. It reads: 'CRUCIO!!!'",
	"Incoming message for Player 1. The message is from 'Bella Swan'. It reads: 'oh edward is soooooooo right when he says sexual intercourse before marriage is bad!'",
	"Incoming message for Player 1. The message is from 'Anastasia Steele'. It reads: 'So much has happened over the last few weeks...'",
	"Incoming message for Player 1. The message is from 'Teresa'. It reads: 'Oh, did I squeeze your hands? Sorry.'",
	"Incoming message for Player 1. The message is from 'Yvonne'. It reads: 'Come on, pull down your pants so I can kick you in the nuts with my dark blue girl sneakers!'",
	"Incoming message for Player 1. The message is from 'Ronja'. It reads: 'I'm a nice girl, and therefore my soft block-heeled combat boots will scratch your exposed legs in a heavenly gentle way.'",
	"Incoming message for Player 1. The message is from 'Karin'. It reads: 'I like ice-cream. Especially vanilla.'",
	"Incoming message for Player 1. The message is from 'Leo'. It reads: 'What are you doing with your weird spear?'",
	"Incoming message for Player 1. The message is from 'Jennifer'. It reads: 'Stop your silly shenanigans.'",
	"Incoming message for Player 1. The message is from 'Ina'. It reads: 'I'm not anorexic, I'm too fat and I need to lose weight!'",
	"Incoming message for Player 1. The message is from 'Desiree'. It reads: 'I am a lower level spokeswoman!'",
	"Incoming message for Player 1. The message is from 'Birgit'. It reads: 'You leave my classroom now! Seriously, you think you can sleep during my lesson? OUT!!!'",
	"Incoming message for Player 1. The message is from 'Ruth'. It reads: 'You stink!'",
	"Incoming message for Player 1. The message is from 'Yasaman'. It reads: 'WTF?'",
	"Incoming message for Player 1. The message is from 'Katia'. It reads: 'If you do not allow me to use the boys' WC, my combat boot heels will scratch your legs until you bleed to death.'",
	"Incoming message for Player 1. The message is from 'type of ice block'. It reads: 'Harharhar!'",
	"Incoming message for Player 1. The message is from 'type of ice block'. It reads: 'Harharhar har!'",
	"Incoming message for Player 1. The message is from 'type of ice block'. It reads: 'Hoooooooo he he he he he...'",
	"Incoming message for Player 1. The message is from 'type of ice block'. It reads: 'Bwar har har har har!'",
	"Incoming message for Player 1. The message is from 'type of ice block'. It reads: 'Harharharharharharharrrrrrrrr!'",
	"Incoming message for Player 1. The message is from 'type of ice block'. It reads: 'Harharharharhar! Harharhar! Harhar!'",
	"Incoming message for Player 1. The message is from 'type of ice block'. It reads: 'Har har!'",
	"Incoming message for Player 1. The message is from 'type of ice block'. It reads: 'Harharhar you won't win harharhar!'",
	"Incoming message for Player 1. The message is from 'type of ice block'. It reads: 'Soon you won't have your money no more, hahaha!'",
	"Incoming message for Player 1. The message is from 'type of ice block'. It reads: 'If I become spokesman of our class, it will be my highest obligation to ensure justice for everyone.'",
	"Incoming message for Player 1. The message is from 'type of ice block'. It reads: 'You need to give me 2 euros until Friday. The money will be donated to the refugee aiding fonds, so more pupils from Syria can go to our school in future!'",
	"Incoming message for Player 1. The message is from 'Victoria'. It reads: 'I'll lock the door behind you, so you cannot go back anymore.'",
	"Incoming message for Player 1. The message is from 'Natalia'. It reads: 'My wedge heels are very practical. I often use them to beat my little son if he was naughty.'",
	"Incoming message for Player 1. The message is from 'Nora'. It reads: 'I cannot kill harmless animals. Doing so would be murder.'",
	"Incoming message for Player 1. The message is from 'Sophia'. It reads: 'Caesar's Legion will punish you for this, you filthy...'",
	"Incoming message for Player 1. The message is from 'Kastortransport'. It reads: 'It's your choice. Either you return the key to me until tomorrow, or I'll report you to the principal.'",
	"Incoming message for Player 1. The message is from 'Juen'. It reads: 'I'll give my leather peep-toe high heels to you as a gift.'",
	"Incoming message for Player 1. The message is from 'Tanja'. It reads: 'I'm not here to talk, I'm here to bludgeon you.'",
	"Incoming message for Player 1. The message is from 'Anne'. It reads: 'Get lost, you crazy freak! We don't want your type around here!'",
	"Incoming message for Player 1. The message is from 'Madeleine'. It reads: 'I caught a Mantine with my first pokeball!'",
	"Incoming message for Player 1. The message is from 'Manuela'. It reads: 'You! Pay a fine of 100 euros to me immediately or I will fuck you up and your children and your grand children.'",
	"Incoming message for Player 1. The message is from 'Natalje'. It reads: 'Help! Heeeeeeeeelp!'",
	"Incoming message for Player 1. The message is from 'Sing'. It reads: 'I'm not a Mary-Sue, I'm just better than you!'",
	"Incoming message for Player 1. The message is from 'Sing'. It reads: 'I didn't drink any blood in a while. You look pretty tasty to me...'",
	"Incoming message for Player 1. The message is from 'Sing'. It reads: 'Here's the punishment I invented for you. I forced all the girls to step into dog shit, and you have to clean all of their shoes now, one after the other.'",
	"Incoming message for Player 1. The message is from an anonymous watcher. It reads: 'still havent ascended? lol u suk hahahahahah'",
	"Incoming message for Player 1. The message is from an anonymous watcher. It reads: 'i bet i can play better than you'",
	"Incoming message for Player 1. The message is from an anonymous watcher. It reads: 'use the force luke!'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'My brand became more famous as I became more famous, and more opportunities presented themselves.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Make America GREAT again!'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Did you know that New York Construction News named Donald Trump the developer and owner of the year?'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I think I am a nice person. People who know me like me.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'It is an ability to become an entrepreneur, a great athlete, a great writer. You're either born with it or you're not.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'He was a strong, strict father, a no-nonsense kind of guy, but he didn't hit me.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I really value my reputation and I don't hesitate to sue.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Angelina Jolie is sort of amazing because everyone thinks she's like this great beauty. And I'm not saying she's an unattractive woman, but she's not a beauty, by any stretch of the imagination. I really understand beauty.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: '... you need love, you need trust, you need sex, you need lots of different things-all of which are very complex.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'For many years I've said that if someone screws you, screw them back. When somebody hurts you, just go after them as viciously and as violently as you can.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'If I get my name in the paper, if people pay attention, that's what matters.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I was always very much accepted by my father. He adored Donald Trump ...'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I know politicians who love women who don't even want to be known for that, because they might lose the gay vote, OK?'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'He taught me to keep my guard up. The world is a pretty vicious place.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Cash is king, and that's one of the beauties of the casino business.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'My marriage, it seemed, was the only area of my life in which I was willing to accept something less than perfection.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I am a really smart guy.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I will be the greatest jobs president that God ever created.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I don't frankly have time for total political correctness.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'We lose to Mexico, both in trade and at the border. We lose to everybody.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I will build a great, great wall on our southern border, and I will make Mexico pay for that wall. Mark my words.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'They're bringing drugs. They're bringing crime. They're rapists.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Jeb Bush has to like the Mexican illegals because of his wife.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'A nation WITHOUT BORDERS is not a nation at all. We must have a wall.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'That's one of the nice things. I mean, part of the beauty of me is that I'm very rich.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Sorry losers and haters, but my I.Q. is one of the highest—and you all know it!'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I would bomb the hell out of those oilfields. I wouldn't send many troops because you won't need 'em by the time I'm finished.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I try to learn from the past, but I plan for the future by focusing exclusively on the present.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'In the end, you're measured not by how much you undertake but by what you finally accomplish.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'An 'extremely credible source' has called my office and told me that Barack Obama's birth certificate is a fraud.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'You can't have Bush. The last thing we need is another Bush'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I'm talking about a lot of leverage. I want to win and we will win.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Free trade is terrible. Free trade can be wonderful if you have smart people. But we have stupid people.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'My Twitter account has over 3.5 million followers. Beat that!'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Sometimes by losing a battle you find a new way to win the war.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Hillary Clinton was the worst Secretary of State in the history of the United States. There's never been a Secretary of State so bad as Hillary.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I am a traditionalist. I have so many fabulous friends who happen to be gay, but I am a traditionalist.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Black guys counting my money! I hate it. The only kind of people I want counting my money are little short guys that wear yarmulkes every day.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Everything in life is luck.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I might lie to you like Hillary does all the time, but I'll never lie to Giacomo, okay?'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'If she gets to pick her judges - nothing you can do, folks. Although, the Second Amendment people. Maybe there is. I don't know.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Why can't we use nuclear weapons?'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Actually, I was only kidding. You can get that baby out of here. Don't worry, I think she really believed me that I love having a baby crying while I'm speaking.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I always wanted to get the Purple Heart. This was much easier.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Vladimir Putin is not going into Ukraine, OK, just so you understand. He's not gonna go into Ukraine, all right? You can mark it down. You can put it down.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I think I've made a lot of sacrifices. I work very, very hard.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I'd like to hear his wife say something.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I've had a beautiful, I've had a flawless campaign. You'll be writing books about this campaign.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I think I am, actually humble. I think I'm much more humble than you would understand.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Ask the gays what they think and what they do, in, not only Saudi Arabia, but many of these countries, and then you tell me — who's your friend, Donald Trump or Hillary Clinton?'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Appreciate the congrats for being right on radical Islamic terrorism, I don't want congrats, I want toughness & vigilance. We must be smart.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I've been treated very unfairly by this judge. Now, this judge is of Mexican heritage. I'm building a wall, OK? I'm building a wall.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Happy Cinco de Mayo! The best taco bowls are made in Trump Tower Grill. I love Hispanics!'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I think the only card she has is the women's card. She has got nothing else going. Frankly, if Hillary Clinton were a man, I don't think she would get 5%% of the vote.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I don't think I'm going to lose, but if I do, I don't think you're ever going to see me again, folks. I think I'll go to Turnberry and play golf or something.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'What do I know about it? All I know is what's on the internet.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'He referred to my hands, if they're small, something else must be small. I guarantee you there's no problem. I guarantee it.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'It is better to live one day as a lion than 100 years as a sheep.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'There may be somebody with tomatoes in the audience. If you see somebody getting ready to throw a tomato, knock the crap out of them, would you?'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'If and when the Vatican is attacked by ISIS, which as everyone knows is ISIS' ultimate trophy, I can promise you that the Pope would have only wished and prayed that Donald Trump would have been President because this would not have happened.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I could stand in the middle of Fifth Avenue and shoot somebody, and I wouldn't lose any voters, okay? It's, like, incredible.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I work very, very hard. I've created thousands and thousands of jobs, tens of thousands of jobs, built great structures. I've had tremendous success.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'There were people that were cheering on the other side of New Jersey, where you have large Arab populations. They were cheering as the World Trade Center came down.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'You could see there was blood coming out of her eyes. Blood coming out of her wherever.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I don't think Ivanka would do that, although she does have a very nice figure. I've said if Ivanka weren't my daughter, perhaps I'd be dating her.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'You know, it really doesn't matter what the media write as long as you've got a young and beautiful piece of ass.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Women: You have to treat them like s--t.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'We're gonna have businesses that used to be in New Hampshire, that are now in Mexico, come back to New Hampshire, and you can tell them to go f**k themselves. Because they let you down, and they left!'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'These people - I'd like to use really foul language. I won't do it. I was going to say they're really full of s**t, but I won't say that.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'My entire life, I've watched politicians bragging about how poor they are, how they came from nothing, how poor their parents and grandparents were. And I said to myself, if they can stay so poor for so many generations, maybe this isn't the kind of person we want to be electing to higher office. How smart can they be? They're morons.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Donald J. Trump is calling for a total and complete shutdown of Muslims entering the United States until our country's representatives can figure out what is going on.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Hillary Clinton was gonna beat Obama. I don't know who would be worse, I don't know, how could it be worse?'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I know where she went - it's disgusting, I don't want to talk about it. No, it's too disgusting. Don't say it, it's disgusting.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Look at that face! Would anyone vote for that? Can you imagine that, the face of our next president I mean, she's a woman, and I'm not s'posedta say bad things, but really, folks, come on. Are we serious?'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'All of the women on 'The Apprentice' flirted with me - consciously or unconsciously. That's to be expected.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I think the only difference between me and the other candidates is that I'm more honest and my women are more beautiful.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Perhaps she made the story up. I think that's what happened.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I like kids. I mean, I won't do anything to take care of them. I'll supply funds, and she'll take care of the kids.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I have a great relationship with the blacks.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'If Hillary Clinton can't satisfy her husband what makes her think she can satisfy America.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I don't think I've made mistakes. Every time somebody said I made a mistake, they do the polls and my numbers go up, so I guess I haven't made any mistakes.'",
	"Incoming message for Player 1. The message is from 'The Joker'. It reads: 'Tomorrow should be a real 'scream', if you know what I mean. Ha ha ha ha ha!'",
	"Incoming message for Player 1. The message is from 'Scarecrow'. It reads: 'You're always interfering with my plans. I won't forgive you again!'",
	"Incoming message for Player 1. The message is from 'The Riddler'. It reads: 'Very well then, riddle me this, player! What is the shortest distance between a point in Nome, Alaska and a point in Miami, Florida?'",
	"Incoming message for Player 1. The message is from 'The Riddler'. It reads: 'In music, the C key has no sharps. The A has 3 and the D has 2.'",
	"Incoming message for Player 1. The message is from 'The Riddler'. It reads: 'This is the final riddle. I have billions of eyes, yet I live in the darkness. I have millions of ears yet only four lobes. I have no muscles yet I rule two hemispheres. What am I?'",
	"Incoming message for Player 1. The message is from 'The Penguin'. It reads: 'Very stupid of you to show your face again, player. I'm going to pay you back for all the grief you've given me.'",
	"Incoming message for Player 1. The message is from 'Scarecrow'. It reads: 'I'll give you a big dose of my special gas!'",
	"Incoming message for Player 1. The message is from 'Clayface'. It reads: 'I will smash you! I won't let anyone stand in my way.'",
	"Incoming message for Player 1. The message is from 'Man-Bat'. It reads: 'I'll put you to rest here!'",
	"Incoming message for Player 1. The message is from 'Mister Greenly'. It reads: 'If you cannot solve this easy equation, the two of us will have a real problem.'",
	"Incoming message for Player 1. The message is from 'Driving Impulse'. It reads: 'Your punishment is 50 slaps, and I think I'll let the female pupils do the execution.'",
	"Incoming message for Player 1. The message is from 'Annie'. It reads: 'Avez-vous deja le francais? Si votre score etait sur une meilleure moyenne que quatre, nous pouvons parler.'",
	"Incoming message for Player 1. The message is from 'Ruea'. It reads: 'Kafir! Yuece Tanri sizi sevmez, ve sonra doenuestuermek yoksa cezalandirilacaktir!'",
	"Incoming message for Player 1. The message is from 'Hans'. It reads: 'You got up two minutes too late, so you have to pay a fine of 1 euro to me.'",
	"Incoming message for Player 1. The message is from 'Desert Dwarf'. It reads: 'What? No, you cannot be in here. You don't belong in my class, so get lost already.'",
	"Incoming message for Player 1. The message is from 'Al Capone'. It reads: 'Get up you lazy fuck, and run for 25 laps!'",
	"Incoming message for Player 1. The message is from 'Mister Also Matte'. It reads: 'If your Grandpa knew about your bad performance, he'd turn over in his grave!'",
	"Incoming message for Player 1. The message is from 'Mister Garbage Houses'. It reads: 'Until next lesson you'll write this sentence 40 times: 'I have to adhere to the instructions of my sports teachers at all times.' The same is valid for all other pupils.'",
	"Incoming message for Player 1. The message is from 'Dickmouth'. It reads: 'The teacher determines when the lesson ends, so you cannot leave yet!'",
	"Incoming message for Player 1. The message is from 'Raini'. It reads: 'If you say 'Fahrenheit' one more time, you will write the word 'Fahrenheit' 500 times until next lesson!'",
	"Incoming message for Player 1. The message is from 'be silent kert'. It reads: 'So that was the last straw. You know you're not allowed to play games in the computer room, really now! Everyone leave the room immediately or I'll lock it for the next week, really now!'",
	"Incoming message for Player 1. The message is from 'Consti'. It reads: 'Stop looking at your seat neighbor's sheet during the exam! He'll know already whether his answers are correct!'",
	"Incoming message for Player 1. The message is from 'Iris flax'. It reads: 'No, the ball is mine now, because soccer is not permitted here.'",
	"Incoming message for Player 1. The message is from 'Schrumpel'. It reads: 'Everyone who comes too late to my physics lesson gets detention. No exceptions.'",
	"Incoming message for Player 1. The message is from 'Riverman'. It reads: 'Leave my classroom! And don't even think about resisting, I'm stronger than you!'",
	"Incoming message for Player 1. The message is from 'Telpan'. It reads: 'I want to banish all stupid teachers from this school.'",
	"Incoming message for Player 1. The message is from 'Luisa'. It reads: 'I will demonstrate my true love for you by kicking you in the shins with my boots.'",
	"Incoming message for Player 1. The message is from 'Maladjusted Leon'. It reads: 'Ey you! What do you think you are doing there?'",
	"Incoming message for Player 1. The message is from 'Soft Sarah'. It reads: 'I like you, so I won't hurt you too badly. But my friend Marika is really angry and will bust you up real good.'",
	"Incoming message for Player 1. The message is from 'Marleen'. It reads: 'I can kick very well, wouldn't you agree?'",
	"Incoming message for Player 1. The message is from 'The Sweet High-Heel-Loving Asian Girl'. It reads: 'I have a very asian-sounding name.'",
	"Incoming message for Player 1. The message is from 'Non-Pretty Melanie'. It reads: 'But my hugging boots are lovely!'",
	"Incoming message for Player 1. The message is from 'The Sweet Redguard Girl'. It reads: 'Why do you call me 'redguard'? My skin color is pale white!'",
	"Incoming message for Player 1. The message is from 'The Hugging Topmodel'. It reads: 'Just fuck yourself.'",
	"Incoming message for Player 1. The message is from 'The Heeled Topmodel'. It reads: 'Are you working for Google or the NSA?'",
	"Incoming message for Player 1. The message is from 'Fanny'. It reads: 'I always go to the karate dojo to practice after school is over.'",
	"Incoming message for Player 1. The message is from 'Elena'. It reads: 'Would you like to pull down the soft cotton that covers my genital area? Then you need to allow me to abuse various parts of your body first.'",
	"Incoming message for Player 1. The message is from 'Barbara'. It reads: 'If I'm angry, I will use my sweet black little-girl sandals to burst open the blood caps of your hands.'",
	"Incoming message for Player 1. The message is from 'Dora'. It reads: 'If you touch my boots I will kill you. Seriously. This is not a joke.'",
	"Incoming message for Player 1. The message is from 'Johanetta'. It reads: 'Sooner or later my pink girl shoes will slam into your shins again.'",
	"Incoming message for Player 1. The message is from 'Tillmann'. It reads: 'You broke my girl-friend's brand-new pair of sneakers, and I will break your bones if you don't compensate her by paying 180 euros, asshole.'",
	"Incoming message for Player 1. The message is from 'Tillmann'. It reads: 'Ey you asshole, pay 5 euros to me or I'll smash your ugly face.'",
	"Incoming message for Player 1. The message is from 'Hannes'. It reads: 'Secrecy is part of my job description. I'm sorry, but I cannot tell you what my job is.'",
	"Incoming message for Player 1. The message is from 'Kristin'. It reads: 'May I step on your toes?'",
	"Incoming message for Player 1. The message is from 'Ludgera'. It reads: 'You have no right to take photos of me without my consent!'",
	"Incoming message for Player 1. The message is from 'System Administrator'. It reads: 'Don't ever #quit - several SLASH'EM Extended players have reported that they started to get good at the game once they stopped ragequitting!'",
	"Incoming message for Player 1. The message is from 'System Administrator'. It reads: 'If you ragequit every time something bad happens, you'll never even get close to ascending. So quit that bad habit already!'",
	"Incoming message for Player 1. The message is from 'System Administrator'. It reads: 'It's not the question if bullshit will happen in this game, the question is when it will happen. Ragequitting won't help - in fact, it will make you a worse player! DON'T DO IT!'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'Now you see me - now you're dead.'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'You cannot defeat me, player.'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'This will be your final battle, player. May whatever god you pray to have mercy on you, because I will not.'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'I'm getting bored of this game, player.'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'Player, I was bored minutes ago. Now I'm beginning to fall asleep!'",
	"Incoming message for Player 1. The message is from 'Rhea'. It reads: 'I silently opened your backpack and filmed it to put my feat in the newspaper. Don't worry, I won't tell any potential thieves about it, I promise.'",
	"Incoming message for Player 1. The message is from 'Ronaldo'. It reads: 'hey my name isnt ronaldo stop calling me that'",
	"Incoming message for Player 1. The message is from 'Julietta'. It reads: 'You have been warned.'",
	"Incoming message for Player 1. The message is from 'Sabine'. It reads: 'My block-heeled sandals will kick you in the head until you die!'",
	"Incoming message for Player 1. The message is from 'Corina'. It reads: 'If you don't want to have your legs scratched by my high-heeled lady boots, you'll have to treat them with due respect.'",
	"Incoming message for Player 1. The message is from 'Maurah'. It reads: '*sigh*'",
	"Incoming message for Player 1. The message is from 'Melissa'. It reads: 'Wow, that's some nice boots you're wearing! I like them!'",
	"Incoming message for Player 1. The message is from 'Ariane'. It reads: 'I hope you sustain a bleeding injury, so I can aid you in bleeding by holding your wounds open because I'd love watching you bleed for as long as possible.'",
	"Incoming message for Player 1. The message is from 'Jasieen'. It reads: 'I won the casting show 'Apokalyptia's Next Topmodel'! I'm so beautiful and happy now!'",
	"Incoming message for Player 1. The message is from 'Annemarie'. It reads: 'I own a helicopter!'",
	"Incoming message for Player 1. The message is from 'Roswitha'. It reads: 'I need to drink another glass of sparkling wine... *hic*'",
	"Incoming message for Player 1. The message is from 'Anita'. It reads: 'My high heels are black so the blood is harder to see after I scratched open someone else's legs.'",
	"Incoming message for Player 1. The message is from 'Solvejg'. It reads: 'I love to see your blood squirting in all directions, so I keep aiming my mocassins at the blood caps of your hands.'",
	"Incoming message for Player 1. The message is from 'Franzi'. It reads: '*sob* There are no nice days at all, there are only terrible days!'",
	"Incoming message for Player 1. The message is from 'Kerstin'. It reads: 'Can you pass me the acid, please?'",
	"Incoming message for Player 1. The message is from 'Verena'. It reads: 'Would you look at the beautiful landscape, it's so romantic with the lovely green forest everywhere!'",
	"Incoming message for Player 1. The message is from 'Mariya'. It reads: 'Do you like high heels?'",
	"Incoming message for Player 1. The message is from 'Emmelie'. It reads: '---'",
	"Incoming message for Player 1. The message is from 'Bettina'. It reads: 'There is something that you'll get for free: trouble. Just keep molesting strange girls and you'll have tons of it.'",
	"Incoming message for Player 1. The message is from 'Hilda'. It reads: 'The youth of today ain't got no manners at all! It used to be that you had to OBEY the adults' instructions back when I was a child!'",
	"Incoming message for Player 1. The message is from 'Nadja'. It reads: 'You're crazy. And stop staring at me all the damn time!'",
	"Incoming message for Player 1. The message is from 'Natascha'. It reads: 'My hobbies are aerobics and dancing.'",
	"Incoming message for Player 1. The message is from 'Gudrun'. It reads: 'Work hard and everything will be alright. But if you slack off, you'll be in trouble.'",
	"Incoming message for Player 1. The message is from 'Patricia'. It reads: 'My steel-capped sandals are very soft and suitable for solving conflicts in a rational, non-violent way! May I demonstrate that to you as I repeatedly whack you over the head with the lovely metal heel?'",
	"Incoming message for Player 1. The message is from 'Nadine'. It reads: 'I'm called Nadine.'",
	"You got an item! Fuckshitting cuntdicks!",
	"PLAYER DRUG BAG FUCK",
	"The Ho-oh stole a blessed greased fixed +5 silver dragon scale mail.",
	"It seduces you and you start to take off your boots. You fall into the water! You sink like a rock.",
	"You fall into the water! You sink like a rock.",
	"You fall into the lava, but it only burns slightly!",
	"Izchak gets angry! You miss Izchak. Izchak zaps an orthorhombic wand!",
	"You feel weak now.",
	"You feel drained...",
	"A malevolent black glow surrounds you...",
	"You lack the strength to cast spells.",
	"You are physically incapable of picking anything up.",
	"Off - you just blinded yourself!",
	"You split open your skull on the ground. You die...",
	"An arrow shoots out at you! You are hit by an arrow!",
	"BANG! A gun shoots at you! The bullet hits your head! This is instantly fatal.",
	"The marilith hits you! The marilith hits you! The marilith opens a gate! The marilith hits you! The marilith hits you! The marilith hits you! The marilith opens a gate! The marilith hits you!",
	"The hill orc zaps a wand of create horde!",
	"You wrest one final charge from the worn-out wand. The wand of create horde turns to dust.",
	"You miss it. It gets angry! You hear a nearby zap. The lightning bolt hits you!",
	"oy||",
	"P|<||",
	"*s",
	"om",
	"LH",
	"bar",
	"Your computer has been running for too long and is now going to shut down.",
	"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
	"Things that are here:  a potion  a potion  a potion  a potion  a potion  a potion--More--",
	"This fortune cookie tastes terrible! There is message in the cookie. It says:--More--",
	"This fortune cookie tastes terrible! There is message in the cookie. It says: 'suk me hahahahahah'",
	"This fortune cookie tastes terrible! There is message in the cookie. It says: 'Nobody will ever ascend SLASH'EM Extended.'",
	"This fortune cookie tastes terrible! There is message in the cookie. It says: 'Have a nice trip! --The local acid supplier.'",
	"This fortune cookie tastes terrible! There is message in the cookie. It says: 'You will get instakilled in 349 turns.'",
	"This fortune cookie tastes terrible! There is message in the cookie. It says: 'Air current noises are sexy!'",
	"This fortune cookie tastes terrible! There is message in the cookie. It says: 'They say that you were eating this fortune cookie too quickly.'",
	"This fortune cookie tastes terrible! There is message in the cookie. It says: 'This nethack fork is ten pounds of hacks.'",
	"This fortune cookie tastes terrible! There is message in the cookie. It says: 'There is a writing on a granite wall in the way.'",
	"You discover a construction, behind which there is a sexy girl waiting... but you can only see her butt. It seems that her name is Kati.",
	"You can't resist the temptation to caress Elena's sexy butt.",
	"Katharina produces beautiful farting noises with her sexy butt. The world spins and goes dark.",
	"Using her sexy butt, Karin produces beautiful noises and craps right in your face.",
	"You hear tender crapping noises.",
	"Kerstin produces beautiful crapping noises with her sexy butt. You just can't believe that someone could produce such erotic noises, and are immobilized by your feelings.",
	"You just cannot stop inhaling the stinking fart gas that Jessica produces with her tender butt.",
	"You hear a soft click. The disintegration beam hits you!",
	"A trap door in the ceiling opens and a rock falls on your head!",
	"A board beneath you squeaks loudly.",
	"You stepped on a trigger! Your ears are blasted by hellish noise!",
	"You stepped on a trigger! Your ears are blasted by hellish noise! Your cyan potion freezes and shatters!",
	"You stepped on a trigger! Your ears are blasted by hellish noise! Your potion of polymorph freezes and shatters!",
	"A bear trap closes on your feet!",
	"You are enveloped in a cloud of gas!",
	"You are frozen in place!",
	"You are sprayed with acid! Your plate mail corrodes!",
	"You are sprayed with acid! Your weapon got vaporized!",
	"You fell into a pool of water!",
	"You stepped on a trap of wishing! Unfortunately, nothing happens.",
	"From the dark stairway to hell, demons appear to surround you!",
	"You are pulled downwards...",
	"The giant eel drowns you...",
	"A vivid purple glow surrounds you...",
	"You are momentarily illuminated by a flash of light!",
	"You feel less protected!",
	"You murderer!",
	"You crumple-horned snorkack!",
	"You hear the rumble of distant thunder...",
	"You hear a clank.",
	"You hear a thud.",
	"You hear a thud. You hear a thud.",
	"You hear a ripping sound.",
	"You hear a chugging sound.",
	"You hear a nearby zap.",
	"You hear a blast.",
	"You hear some noises.",
	"You hear some noises in the distance.",
	"You have a sad feeling for a moment, then it passes.",
	"You feel worried about Idefix.",
	"You feel your magical energy dwindle to nothing!",
	"You feel your magical energy dwindling rapidly!",
	"You feel a little apathetic...",
	"You feel like you could use a nap.",
	"You have been playing for long enough now, so the RNG decides to kill your character.",
	"Suddenly, rocks fall and everyone dies. Do you want your possessions identified? DYWYPI? [ynq] (n) _",
	"Your purse feels lighter...",
	"Your belongings leave your body!",
	"You hear a chuckling laughter.",
	"You hear a chuckling laughter. You feel a little sick!",
	"The RNG determines to take you down a peg or two...",
	"You undergo a freakish metamorphosis!",
	"You feel a little strange. You turn into a black naga! You break out of your armor!",
	"Suddenly, there's glue all over you!",
	"Your position suddenly seems very uncertain!",
	"You suddenly get teleported!",
	"You are getting confused.",
	"You stagger...",
	"You stepped into a heap of shit!",
	"You feel yourself yanked in a direction you didn't know existed!",
	"The magic cancellation granted by your armor seems weaker now...",
	"You're hit by a massive explosion!",
	"A cloud of noxious gas billows out at you. The gas cloud was poisoned!",
	"You are jolted by a surge of electricity! Your wand of lightning explodes!",
	"You start turning to stone!",
	"Your weapon slips from your hands.",
	"You feel feverish.",
	"It seems a little more dangerous here now...",
	"You hear a mumbled incantation.",
	"You feel that monsters are aware of your presence.",
	"Thinking of Maud you forget everything else.",
	"Eek - you stepped into a heap of sticky glue!",
	"You are irradiated by pure mana!",
	"KAABLAMM!!!  You triggered a bomb!",
	"Click! You trigger a rolling boulder trap!",
	"You are decapitated by a guillotine! You die...",
	"Rocks fall all around you! Your elven leather helm does not protect you.",
	"You suddenly feel a surge of tension!",
	"You feel the arrival of monsters!",
	"You feel a colorful sensation!",
	"Cyanide gas blows in your face!",
	"You hear the roaring of an angry bear!",
	"You are enveloped by searing plasma radiation!",
	"One of your objects withered away!",
	"A potion explodes!",
	"One of your objects was destroyed by rust!",
	"WARNING: A Scud Storm has been launched.",
	"WARNING: Nuclear missile silo detected.",
	"WARNING: Level ends here!",
	"WARNING: Not enough memory for objects. Deleting objects from player's inventory to free space...",
	"WARNING: too many spells memorized. Resetting player's spell list.",
	"WARNING: array_playerintrinsics too large, deleting intrinsic_teleport_control",
	"WARNING: player is having too much luck. Increasing random monster spawn difficulty by 50...",
	"WARNING: Demogorgon was spawned on this level!",
	"WARNING: Assertion failure.",
	"WARNING: out_of_memory, emergency termination",
	"WARNING: system integrity has been damaged, initiating system halt",
	"WARNING: This version of SLASH'EM Extended is outdated. Please consider getting an up-to-date version.",
	"Arabella casts world fall. You die... Do you want your possessions identified? DYWYPI? [ynq] (n) _",
	"Anastasia uses a vacuum cleaner on you! Or is that a gluon gun? Anastasia sucks your head off! You die...",
	"Henrietta shoots 9 bullets! The 1st bullet hits you! The 2nd bullet hits you! The 3rd bullet hits you! Warning: HP low! The 4th bullet hits you!",
	"Katrin casts a spell at you! Suddenly, you're frozen in place! Katrin hits you! Katrin kicks you! You hear the wailing of the Banshee...",
	"Jana was hidden under aXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX--More-- Oops... Suddenly, the dungeon collapses. Error save file being written.",
	"Unreliable overflow, ignoring",
	"Ignoring unreliable datagram for player 1, would overflow",
	"Assertion Failure - Location: src/role.c, line 666, Expression: player_canascend == 0",
	"Halt - unrecoverable internal error 0035ffbc",
	"Bad syntax in options/player1/nethackrc.rc, line 0: OPTIONS=rest_on_space",
	"Monster master_mind_flayer stuck in wall - level design error",
	"Bad message string 'UHH! UHH! OH! OH! YES! IN! OUT! IN! OUT! DAMN! IS! THAT! YOUR! MOTHER! I! WILL! BLOW! A! GRENADE! UP! YOUR! SHIT! ASS! SHIT! CHARLIE!'",
	"Error - model potion_of_full_healing.jpg not found",
	"Could not load minor_demon_spawn_list(). Spawning Demogorgon instead.",
	"Fatal error: function void increase_player_luck() returning a non-zero value.",
	"Fatal error: user is trying to defy the wrath of the RNG. Calling god_zapsplayer() now.",
	"SLKJEWIUOIOSDGIOPSDGHIOPWEIOPWPHJBSDJLEJAKDYWYPIWELTJKEWZLKJWJEKLZ",
	"You have mail from user 'jonadab'! Read it? [yes/no]? _",
	"You have mail from user 'Bluescreen'! Read it? [yes/no]? y It reads: --More--",
	"You have mail from user 'kerio'! Read it? [yes/no]? The mail was covered with contact poison! You die...",
	"On salue, pomme de terre inconnue. to meet your kind here. You sure heard of the tragedy that befell Tristram--More-- format c:",
	"Can't write to save file",
	"Autosave failed! Reason: not enough free disk space",
	"It seduces you and you pull down your pants.",
	"It murmurs in your ear while helping you undress. You're naked.",
	"She gently massages your unprotected nuts, but suddenly pushes her knee right into your groin! OUCH!",
	"The incubus caresses your soft breasts, but all of a sudden he starts cutting them up with a knife!",
	"Mariya moves her sexy butt right in front of your nose and produces disgusting farting noises. You can't resist the temptation to inhale deeply...",
	"You suddenly feel desperate, so you grab a sexy leather pump and start to scratch up and down your own legs with the razor-sharp high heel.",
	"Nadja gets really angry and puts the buckles of her lady shoes on your shins, then scratches up and down very painfully, ripping a lot of your skin to shreds.",
	"Suddenly Victoria sits on you and places the heels of her fleecy combat boots on your hands. She asks: 'Do you like it?'",
	"You are losing blood!",
	"You are bleeding out from your stinging injury!",
	"You flinch!",
	"Beware, there will be no return! Still climb? [ynq] (y) _",
	"You hear a laughing voice announce: 'Wimpy player, better go back to playing Pac-Man!'",
	"You hear a laughing voice announce: 'Eh, still thinking you can beat this game?'",
	"You hear a laughing voice announce: 'It's fun to watch you play because bad stuff happens to you all the time.'",
	"You hear a laughing voice announce: 'Please stay alive for a while so your inevitable YASD will be more fun!'",
	"You hear a laughing voice announce: 'I'm watching your game. And judging from what I've seen so far, I'm sure you'll die in, let's say, 500 turns.'",
	"You hear a laughing voice announce: 'Did you know there is a gnome with the wand of death on this level?'",
	"You hear a laughing voice announce: 'I am watching you...'",
	"You hear a laughing voice announce: 'There is an inventorylessness trap on this level - good luck avoiding it!'",
	"You hear a laughing voice announce: 'One level down, Demogorgon is waiting for you!'",
	"You hear a laughing voice announce: 'Here's looking at you, kid.'",
	"You hear a laughing voice announce: 'The RNG is rigged. You cannot win.'",
	"You hear a laughing voice announce: 'I'm literally laughing my butt off when seeing your playstyle.'",
	"You hear a laughing voice announce: 'Keep confusing monsters, that way they can spawn more of their friends by reading create monster scrolls!'",
	"You hear a laughing voice announce: 'You just read another one of these silly messages!'",
	"SLASH'EM Extended Notification Service: Someone opened the ttyrec of your current game.",
	"SLASH'EM Extended Notification Service: Your amount of public watchers has increased by 1.",
	"SLASH'EM Extended Notification Service: You just got pinged in IRC channel #slashemextended.",
	"SLASH'EM Extended Notification Service: The ttyrec of your current run will be saved here:--More--",
	"SLASH'EM Extended Notification Service: User 'Bluescreen' invites you to join IRC channel #slashemextended!",
	"SLASH'EM Extended Notification Service: If you die now, you will be #1 on the high-score list!",
	"SLASH'EM Extended Notification Service: A person watching your game is trying to send you mail.",
	"SLASH'EM Extended Notification Service: Please make sure you're playing the most recent, up-to-date version of the game.",
	"SLASH'EM Extended Notification Service: Freenode has the #slashemextended and #em.slashem.me channels that you should join.",
	"SLASH'EM Extended Notification Service: In case of doubt, consult the SLASH'EM Extended page on Nethackwiki for tons of spoilers.",
	"SLASH'EM Extended Notification Service: There is a github source code repository for SLASH'EM Extended! If you ever want to go code diving, that's where you need to go!",
	"SLASH'EM Extended Notification Service: If you ascend, you will become immortal!",
	"SLASH'EM Extended Notification Service: Join the #em.slashem.me channel on Freenode! Lots of other players are there!",
	"SLASH'EM Extended Notification Service: There's an interesting Let's Play SLASH'EM Extended thread by AmyBSOD on forum.rpg.net if you're interested!",
	"You hear crashing rock.",
	"The dungeon acoustics noticeably change.",
	"You hear bubbling water.",
	"You hear water falling on coins.",
	"You hear the splashing of a naiad.",
	"You hear a slow drip.",
	"You hear a gurgling noise.",
	"You hear the tones of courtly conversation.",
	"You hear a sceptre pounded in judgment.",
	"You hear mosquitoes!",
	"You smell marsh gas!",
	"You hear screaming in the distance!",
	"You hear a faint whisper: \"Please leave your measurements for your custom-made coffin.\"",
	"You hear a door creak ominously.",
	"You hear hard breathing just a few steps behind you!",
	"You hear dragging footsteps coming closer!",
	"You hear anguished moaning and groaning coming out of the walls!",
	"You hear mad giggling directly behind you!",
	"You smell rotting corpses.",
	"You smell chloroform!",
	"You feel ice cold fingers stroking your neck.",
	"You feel a ghostly touch caressing your face.",
	"You feel somebody dancing on your grave.",
	"You feel something breathing down your neck.",
	"You feel as if the walls were closing in on you.",
	"You just stepped on something squishy.",
	"You smell horse dung.",
	"You hear someone counting money.",
	"You hear soft splashing.",
	"You hear gutteral voices chanting.",
	"You hear frenzied splashing.",
	"You hear someone scream: 'No! Stop! The Knife! Aaaaaahhhh!'",
	"You hear someone searching.",
	"You hear gently lapping water.",
	"You hear somebody screaming for help!",
	"You hear the footsteps of a guard on patrol.",
	"You hear a low buzzing.",
	"You hear an angry drone.",
	"You suddenly realize it is unnaturally quiet.",
	"An enormous ghost suddenly appears before you!",
	"You hear blades being honed.",
	"You hear loud snoring.",
	"You hear dice being thrown.",
	"You hear something about streptococus.",
	"You smell chloroform nearby.",
	"You hear someone cursing viruses.",
	"You hear leaves rustling.",
	"You hear a female voice singing a song.",
	"You catch a glimpse of sunlight.",
	"You hear mandibles clicking.",
	"You notice webs everywhere.",
	"You notice little openings in the floor.",
	"You hear a grunting noise.",
	"You notice a horrible smell.",
	"You hear loud footsteps going up and down the halls.",
	"You trip over a stinking pile of shit!",
	"You suddenly see a huge monster appearing right in front of you!",
	"You listen to a conversation.",
	"You hear people talking.",
	"You hear someone drawing a weapon.",
	"You sense that you are not alone...",
	"You hear a glass of wine breaking!",
	"You listen to a radio broadcast: For the next five days, the dungeon's estimated temperature high is 96 degrees. Now, is that in Fahrenheit or Kelvin?",
	"You hear rattling noises.",
	"You think you saw an object move on its own.",
	"You feel that something weird may be just around the corner.",
	"You hear the deafening noise of a metal object breaking apart!",
	"A wild WARMECH appeared!",
	"You notice the sound of clinging coins.",
	"You feel that, somewhere down here, there might be a whole lot of gold.",
	"You hear metal banging against a wall.",
	"You hear the sound of wispy wings.",
	"You can barely hear a whispering voice.",
	"You sense a glow of radiance.",
	"You think you just heard God speaking to you!",
	"You suddenly feel a wave of heat.",
	"You notice some heaps of earth.",
	"You sense a strong gust of wind.",
	"You hear a wave of water.",
	"You feel weirded out.",
	"You sense something strange about this place.",
	"You think you just saw something move.",
	"You think you're seeing white rabbits!",
	"You feel totally down, like you're on a bad trip!",
	"You have a weird feeling of chills and excitement.",
	"You sense that something isn't right here...",
	"You shudder in expectation.",
	"You hear the trickle of water.",
	"You sense a watery smell mixed with grass.",
	"You listen to an unusual ambient sound.",
	"You hear a fountain crash through solid rock with a mighty roar!",
	"You admire the beautiful mountains around here. Strange, how can mountains be in an underground dungeon?",
	"The killer bee stings you!",
	"The migo queen's tentacles suck you!",
	"It hits! Its tentacles suck you! You feel very stupid!",
	"The master mind flayer drinks a potion of invisibility. Suddenly you cannot see the master mind flayer.",
	"You feel aggravated at the invisible master lich.",
	"The rust monster eats an athame named Magicbane!",
	"The gelatinous cube eats a scroll of wishing!",
	"The rock mole eats a blessed stone of magic resistance!",
	"The ghast eats a black dragon corpse!",
	"You hear an awful gobbling noise!",
	"You hear a crunching sound.",
	"You hear a grating sound.",
	"You feel a certain tension.",
	"You feel a certain tension. This message was stolen from ADOM.",
	"You sense a certain tension.",
	"You sense a certain tension. Perhaps you should head for the nearest toilet...",
	"You sense a certain tension. That dreaded Dungeon Master probably filled the next room with arch-liches, disenchanters and master mind flayers.",
	"You sense a certain tension. That dreaded Dungeon Master probably filled the next room with black dragons, green slimes and invisible cockatrices.",
	"You sense a certain tension. That dreaded Dungeon Master probably filled the next room with luck suckers, Mothers-in-Law and questing beasts.",
	"It casts a spell!",
	"Something casts a spell at you!",
	"Something casts a spell at thin air!",
	"It breathes--More--",
	"You hear an exhaling sound.",
	"You hear a flinging sound.",
	"You hear a spitting sound.",
	"You are hit by a dagger!",
	"You are hit by a bullet! You are hit by a bullet! You are hit by a bullet! You are hit by a bullet!",
	"You are hit by an elven arrow! You are hit by an elven arrow! You are hit by an elven arrow!",
	"You are hit by an elven arrow! You are hit by an elven arrow! Elf is about to die.",
	"You are hit by a shuriken!",
	"You are hit by a dart! The dart was poisoned!",
	"The flagon crashes on your head and breaks into shards. Oh wow! Everything looks so cosmic!",
	"You hear a hurling sound.",
	"You hear a disenchanter reading a scroll labeled RAMBA ZAMBA.",
	"The maud agent hurls a sparkling potion! The carafe crashes on your head and breaks into shards.",
	"You hear a giant eel reading a scroll of create monster.",
	"The gnome drinks a potion of polymorph! The gnome turns into a hru!",
	"The lizard drinks a potion of polymorph! The lizard turns into a praetor!",
	"The killer bee drinks a potion of polymorph! The killer bee turns into a gorgon fly!",
	"It gazes directly at you! You are slowing down.",
	"You hear a sound reminiscent of an elephant stepping on a peanut.",
	"You hear a sound reminiscent of a seal barking.",
	"The ice crackles and melts. You fall into the water! You sink like a rock.",
	"You hear someone cursing shoplifters.",
	"You hear the chime of a cash register.",
      "You hear someone complaining about the prices.",
      "Somebody whispers: \"Food rations? Only 900 zorkmids.\"",
      "You feel the presence of some sort of force.",
      "There seems to be something special about this place.",
      "You hear someone say: \"Force field activated!\"",
      "You hear someone say: \"Warp panel test sequence is starting in three... two... one...\"",
	"It ululates! You kill it! You hear the studio audience applaud!",
	"The apostroph golem honks! You kill the dissolved undead potato! You hear the studio audience applaud!",
	"\"I've killed hundreds of pigs like you!\"",
	"\"You just made me break a nail!\"",
	"\"I'll kick your nuts flat!\"",
	"\"YOU DON'T STAND A CHANCE!\"",
	"\"YOU WILL DIE A SLOW, SLOW, DEATH...\"",
	"\"COME OUT! WE WILL HURT YOU!\"",
	"\"Your madness ends here, betrayer!\"",
	"\"You're not gonna ascend, puny mortal!\"",
	"\"Run, or die!\"",
	"\"Somebody call a medic!\"",
	"\"You call yourself a human being. I call you a pain in the butt, that's what I call you.\"",
	"\"Repent, and thou shalt be saved!\"",
	"\"*In a bored, monotone voice* Repent, and thou shalt be saved.\"",
	"\"At least you aren't that Mario guy. He demands I bring him back EVERY SINGLE TIME he dies.\"",
	"\"Die monster, you don't belong in this world!\"",
	"\"I am not burdened by any sort of genitals! Jealous you must be!\"",
	"\"I have HAD it with these motherfucking mortals on this motherfucking astral plane!\"",
	"\"I'm gonna LOVE and TOLERATE the SHIT out of you!\"",
	"\"Lawful is easy, I'll show you why!\"",
	"\"Stop right there criminal scum! Nobody breaks the law on my watch!\"",
	"\"Begone, and do not return!\"",
	"\"I will be rude to you if you disturb my work!\"",
	"\"You don't affect me. The trashy strolling is an eyesore.\"",
	"\"You have gained monaters from graduate school, but you are so weak. It's a great pity.\"",
	"\"Forget it! In any case you were not grasped by the legendary monster.\"",
	"\"I trained on the road of ultimate!\"",
	"\"Think of your impending death my sputa will flow down.\"",
	"\"Little strawberry me baby!\"",
	"\"Is my hair-clip beautiful? Contest?\"",
	"\"In spite of the monster, and the coach, only trashy, I will beat down them all.\"",
	"\"One day I will beat you down, and let you know my strength.\"",
	"\"BULLET GIVEN BY BET-BOY GIVEN BY BET-BOY CAN USE\"",
	"\"I am the son of Wind, the child of spirit.\"",
	"\"What don't stare me always!\"",
	"\"Because we have the spirit not to lose absolutely, whether or no, and we are called 'wintry poplar and willow' to see our might!\"",
	"\"Wei! You can't throw the butt casually!\"",
	"\"Everyone is playing the pocket monster, so am I. It is very lovely! Try it? You play it very well!\"",
	"\"The pocket monster said that I was lovely.\"",
	"\"Little familiarity grotty sight!\"",
	"\"How are you? I will visit you. Try to eat a piece of muskiness.\"",
	"\"Oh, it is you. You are more fearful than Msr.Yu.\"",
	"\"Glinting set off by rainbow fluttering fairily in the sky.\"",
	"\"Flapping the sea with ten legs bending!\"",
	"\"Avada Kedavra!\"",
	"\"Doth pain excite thee?  Wouldst thou prefer the whip?\"",
	"\"Abandon your foolish quest! All that awaits you is the wrath of my Master. You are too late to save the child. Now you'll join him - in hell.\"",
	"\"Only the chosen ones will survive purgatory.\"",
	"\"Lord Dagon awaits your soul in Oblivion.\"",
	"\"You're weak, just like the gods you pray to.\"",
	"\"This is your end, swit!\"",
	"\"You will suffer eternally!\"",
	"\"Soon you will be one of us.\"",
	"\"Die, coward!\"",
	"\"I'll cut you up slowly.\"",
	"\"Beg for your life, slave!\"",
	"\"You're gonna bleed!\"",
	"\"I'm gonna feast on your dead flesh!\"",
	"\"Your corpse will stink like a heap of shit!\"",
	"\"Morituri te salutant!\"",
	"\"Ph'nglui mglw'nafh Cthulhu R'lyeh wagh'nagl fthagn!\"",
	"\"Suck it!\"",
	"\"The next words out of your mouth better be some Mark Twain shit because it's going on your grave.\"",
	"\"What kind of loser has the time to make it this far? After I kill you, get a life!\"",
	"\"Where is your God now?\"",
	"\"You have no chance to survive make your time.\"",
	"\"You n'wah!\"",
	"\"Your mommy cannot bake!\"",
	"\"Your mother was a hamster, and your father smelt of elderberries!\"",
	"\"Your scream reminds me of your mother last night!\"",
	"\"You fight like a girl called Frankenstein!\"",
	"\"Come join the dark side, we have cookies!\"",
	"\"Stop sucking so bad at this game! Go read some spoilers!\"",
	"\"I AM... I AM A MONSTER! ... ... ... ... ... ... ... ... coach.\"",
	"\"I heard you were sleeping there.\"",
	"\"Er which hole? What droped? Failing thing, serve one right!\"",
	"\"GO AWAY!\"",
	"\"EN'SSICIBZ EDUDO\"",
	"\"Your inside is made of mud!\"",
	"\"Your daddy screamed REAL good before he died!\"",
	"\"You will die, just like your father!\"",
	"\"Die, filthy N'wah!\"",
	"\"As if feel something!\"",
	"\"Linked hole!\"",
	"\"EY REFLECT EFF INT!\"",
	"You feel a strange vibration beneath your feet.",
	"You are frozen solid!",
	"You're burning!",
	"Your potion of extra healing dilutes.",
	"Your potion of gain level dilutes further.",
	"Suddenly, your 2 scrolls of genocide transform!",
	"Your magic marker explodes!",
	"The scroll turns to dust as you pick it up.",
	"The scroll turns to dust as you touch it.",
	"As you touch the scroll, your mind turns inward of itself. Thinking of Maud you forget everything else.",
	"There's some graffiti on the ground here. Use 'r' to read it.",
	"The door resists!",
	"You see a door open.",
	"You hear a door open.",
	"You see a door crash open.",
	"You hear a door crash open.",
	"You feel vibrations.",
	"You feel shuddering vibrations.",
	"You hear a soft click.",
	"You hear a click.",
	"You hear several clicking sounds.",
	"You hear a rushing noise nearby.",
	"Your cloak of magic resistance is protected by the layer of grease! The grease wears off.",
	"The death ray hits it.",
	"The blast of disintegration misses it. The blast of disintegration hits it.",
	"You feel cold.",
	"You hear sizzling. The acid ray hits it.",
	"It breathes lightning. The door splinters!",
	"You hear a nearby zap. You hear crashing wood.",
	"You hear an exhaling sound. You smell smoke.",
	"You hear a flinging sound. The door absorbs the bolt!",
	"You see a puff of smoke.",
	"Some water evaporates.",
	"Your Stormbringer suddenly vibrates unexpectedly.",
	"Your Stormbringer suddenly vibrates unexpectedly and then detonates. Warning: HP low!",
	"You feel weaker! You lose  Strength",
	"You feel clumsy! You lose  Dexterity",
	"Your muscles won't obey you! You lose  Dexterity",
	"You feel stupid! You lose  Intelligence",
	"Your brain is on fire! You lose  Intelligence",
	"You feel foolish! You lose  Wisdom",
	"Your judgement is impared! You lose  Wisdom",
	"You feel very sick! You lose  Constitution",
	"You break out in hives! You lose  Charisma",
	"The gremlin chuckles. You feel a little sick!",
	"You lear laughter. Your senses fail!",
	"A voice announces: 'Thou be cursed!' You feel no longer immune to diseases!",
	"You hear a chuckling laughter. You feel unable to control where you're going.",
	"You feel warmer.",
	"You feel less jumpy.",
	"You feel a little sick!",
	"Your senses fail!",
	"You feel cooler.",
	"You feel paranoid.",
	"You thought you saw something!",
	"You feel slower.",
	"You feel clumsy.",
	"You feel vulnerable.",
	"You feel less attractive.",
	"You feel tired all of a sudden.",
	"You feel like you're going to break apart.",
	"You feel like someone has zapped you.",
	"You feel like someone is sucking out your life-force.",
	"You feel no longer immune to diseases!",
	"You feel that your radar has just stopped working!",
	"You feel unable to find something you lost!",
	"You feel a loss of mental capabilities!",
	"You feel shrouded in darkness.",
	"You feel that you can no longer sense monsters.",
	"You feel your legs shrinking.",
	"You feel unable to control where you're going.",
	"You feel you suddenly need to breathe!",
	"You feel your wounds are healing slower!",
	"You feel a loss of mystic power!",
	"You feel 42.",
	"You feel unable to change form!",
	"You feel less control over your own body.",
	"You feel worried about corrosion!",
	"You feel like you're burning calories faster.",
	"You feel like you just lost your wings!",
	"You feel less ethereal!",
	"You feel less protected from magic!",
	"You feel less reflexive!",
	"You feel less aquatic!",
	"You feel a loss of freedom!",
	"You feel a little anxious!",
	"You feel vaguely nervous.",
	"You feel a case of selective amnesia...",
	"You feel cursed!",
	"You feel a little stunned!",
	"You feel a little confused!",
	"You feel your attacks becoming slower!",
	"You feel your attacks becoming a lot slower!",
	"You feel less able to learn new stuff!",
	"You feel a little dead!",
	"You feel a little exposed!",
	"You feel empty-minded!",
	"You feel less magically attuned!",
	"You feel that you're going to die in turns time!",
	"You feel that you really ain't gonna ascend with this character...",
	"You feel that the life of your character is forfeit, better roll a new one right away...",
	"You feel that the RNG is out to get you!",
	"You feel a need to check Nethackwiki so you can see if there's a new version of SLASH'EM Extended available...",
	"You feel a need to press the 'v' key so you can see if you're playing the up-to-date version...",
	"You feel that Amy Bluescreenofdeath may have made a new version of this game already...",
	"You feel that Nethack is better than Crawl.",
	"You feel that Slash'EM Extended is the best Nethack variant available.",
	"You feel that ADOM sux compared to Nethack.",
	"You feel like giving AmyBSOD's other roguelike variant ToME-SX a try someday.",
	"You feel that ToME-SX might also be a good game, so you open the git repository page in your web browser.",
	"You feel that Nethack and Angband really are the two best major roguelikes ever created.",
	"You feel that reading these messages makes you feel stupid.",
	"You feel that you keep looking at the messages displayed on the top status line.",
	"You feel like skipping all the annoying battle spam messages, including actually important ones like 'You are slowing down'.",
	"You feel that AmyBSOD is trolling you with all the message spam, yet you keep reading it... :D",
	"You feel that AmyBSOD has no life since she typed in all those various bullshit messages you're reading.",
	"You feel that this message is false.",
	"You feel that this message is true.",
	"You feel that the previous message was false.",
	"You feel that the next message will be a fake one.",
	"You feel that the next message will completely change your life.",
	"You feel that tomorrow you will meet the woman of your dreams.",
	"You feel that you now know the meaning of life, the universe and everything else.",
	"You feel that now, after having played Nethack for 25 hours straight, it is time to go to bed.",
	"Remember, your body in real life has functions too - don't forget to drink, eat and sleep!",
	"Reminder: don't forget to go to the toilet!",
	"Don't neglect your real-life obligations because of Nethack, please!",
	"It's time for our super-special commercial announcement: Buy THE COMPLETE ANGBAND AND ANCIENT DOMAINS OF MYSTERY PACKAGE for just $50!!!",
	"It's time for our super-special commercial announcement: Fallout 4, just $60 and not dumbed down at all! Buy now!",
	"It's time for our super-special commercial announcement: Buy 5 dollars for the unbelievable price of just $2!",
	"It's time for our super-special commercial announcement: Amulet of Yendor, price $99999999999--More--",
	"It's time for our super-special commercial announcement: CALIGULA'S CASINO! PLACE YOUR BETS, WIN WIN WIN!",
	"It's time for our super-special commercial announcement: FISHING! FISHING! WIN! WIN!",
	"It's time for our super-special commercial announcement: Today there's a laptop with Micro$oft Windows 7 for sale! Just $2500!",
	"It's time for our super-special commercial announcement: e - a ring of become Noeda (being worn). Sub-5000 turn ascension virtually guaranteed!",
	"It's time for our super-special commercial announcement: 10 scrolls of destroy armor! Mass discount if you buy them all!",
	"It's time for our super-special commercial announcement: For just $50000, you can get your own heavily cursed thoroughly corroded very rusty -7 Grayswandir welded to your hand!",
	"It's time for our super-special commercial announcement: #em.slashem.me - you'll want to join this Freenode channel, because that's where you find other players!",
	"It's time for our super-special commercial announcement: Junethack, the free cross-variant Nethack tournament, is held each day during June! You should play too!",
	"It's time for our super-special commercial announcement: Buy the book 'Marc's Abenteuer' on Amazon today to read about Marc's roguelike adventures!",
	"It's time for our super-special commercial announcement: We have a premium-quality cheap plastic imitation of a figurine of AmyBSOD for sale today.",
	"It's time for our super-special commercial announcement: A bunch of 'Teamsplat Hardcore Autism Junethack 2016' dogtags, collected from the deceased rodneyan gladiator LostSouls! Straight out of the Halls of Mandos!",
	"It's time for our super-special commercial announcement: NETHACK 3.6.0!!! It UPDATED!!! For only $5 you can experience a whole new dimension of insanity, because Satan and Float Eyeler joined the dev team!",
	"It's time for our super-special commercial announcement: Buy a blessed tin of mind flayer meat for the unbelievably low price of only 20 000 zorkmids!",
	"It's time for our super-special commercial announcement: The Heart of Ahriman, price 2000 zorkmids. First buyer will get it. Exception: if you're a barbarian then fuck off.",
	"It's time for our super-special commercial announcement: A brand-new 'Dungeons of Doom'(TM) toilet for all your crapping needs! If you also buy 10 rolls of toilet paper, we'll throw in a free black dragon corpse!",
	"Your fingers seem ok for now.",
	"Your fingers start hurting from playing for so long.",
	"Nothing appears to happen.",
	"Somehing appears to happen.",
	"Nothing happens.",
	"Something happens.",
	"Something bad happens.",
	"You don't notice anything unusual.",
	"You notice something unusual.",
	"You don't really feel any worse.",
	"You feel worse.",
	"You can't tell that anything is going wrong.",
	"You can tell that something is going wrong.",
	"Your mood seems about as good as before.",
	"Your mood seems much worse as before.",
	"You don't even have a case of the heebie jeebies.",
	"You have a case of the heebie jeebies.",
	"You have a case of the willies for a moment, but it passes.",
	"You probably have nothing to fear but fear itself.",
	"You probably have nothing to fear but fear itself, and that monster that wants to eat you.",
	"The dungeon doesn't seem to be collapsing.",
	"The dungeon seems to be collapsing.",
	"Suddenly, the dungeon doesn't collapse.",
	"You don't see monsters appearing out of nowhere.",
	"You see monsters appearing out of nowhere.",
	"You see monsters crawling out of the walls.",
	"You don't have an urgent need to sit down.",
	"You have an urgent need to sit down.",
	"You have an urgent need to go to the toilet.",
	"You don't notice anything unnatural about how quiet it is.",
	"You notice something unnatural about how quiet it is.",
	"The gas spore explodes!",
	"You get blasted!",
	"You are caught in the highly explosive bomb's explosion!",
	"BOOM! It explodes!",
	"The newt bites you!",
	"Suddenly, you notice a gnoll.",
	"The superjedi ignites a green lightsaber.",
	"The jedi knight wields a red lightsaber!",
	"The mumak blasts you!",
	"You find a fire trap.",
	"Do you really want to step into that mind wipe trap?",
	"You find a super nupesell nix area.",
	"You find a magic portal.",
	"You trigger a magic portal, but it doesn't seem to work!",
	"You're pushed back!",
	"Yes.",
	"No.",
	"Really save?",
	"Your candle's flame is extinguished!",
	"Your magic lamp goes out!",
	"Your magic lamp turns into an oil lamp!",
	"Clash!",
	"You hear a twang.",
	"You hear a bugle playing reveille!",
	"It breathes on you!",
	"The green naga spits on you!",
	"The soldier fires a bullet!",
	"Something explodes inside your knapsack!",
	"You feel increasingly panicked about being in the dark!",
	"You hear a loud scraping noise.",
	"You can swim! Type #swim while swimming on the surface to dive down to the bottom.",
	"Press Ctrl^E or type #seal to engrave a seal of binding.",
	"#chat to a fresh seal to contact the spirit beyond.",
	"Press Ctrl^F or type #power to fire active spirit powers!",
	"Beware, droven armor evaporates in light!",
	"Use #monster to create a patch of darkness.",
	"Use #monster to adjust your clockspeed.",
	"You do not heal naturally. Use '.' to attempt repairs.",
	"Incantifiers eat magic, not food, and do not heal naturally.",
	"The engraving tumbles off the ring to join it's fellows.",
	"The leash would not come off!",
	"The kitten chokes on the leash!",
	"An incomprehensible sight meets your eyes!",
	"It makes a rather sad clonk.",
	"It seems inert.",
	"The cloak sweeps up a web!",
	"A web spins out from the cloak!",
	"The thoroughly tattered cloak falls to pieces!",
	"A monster is there that you couldn't see.",
	"Suddenly, and without warning, nothing happens.",
	"WARNING: Nothing happens!",
	"WARNING: something might have happened!",
	"WARNING: an out-of-depth monster was spawned!",
	"WARNING: an out-of-depth monster was spawned! Really! This is not a joke!",
	"WARNING: you still haven't ascended yet!",
	"Your grasp of physics is appalling.",
	"Is this a zen thing?",
	"You chip off the existing rune.",
	"The wood heals like the rune was never there.",
	"In order to eat, use the 'e' command.",
	"There is a seal on the tip of the Rod! You can't see it, you know it's there, just the same.",
	"You learn a new seal!",
	"You feel more confident in your weapon skills.",
	"You feel more confident in your fucking weapon skills.",
	"You feel more confident in your combat skills.",
	"You feel more confident in your spellcasting skills.",
	"You feel more confident in your high heels skill.",
	"You stumble because you're wearing clunky heels. Why are you wearing them if you can't walk in them, you stupid woman?",
	"There is a seal in the heart of the crystal, shining bright through the darkness.",
	"The cracks on the mirror's surface form part of a seal.",
	"dbon applies",
	"The webbing sticks to you. You're caught!",
	"The unholy blade drains your life!",
	"Your weapon has become more perfect!",
	"Your weapon has become more perfect! Yes, this sentence is not nonsensical in any way!",
	"The winds fan the flames into a roaring inferno!",
	"The winds blast the stone and sweep the fragments into a whirling dust storm!",
	"The winds whip the waters into a rolling fog!",
	"The hateful axe hamstrings you!",
	"The asian girl smashes your nuts flat!",
	"It blows apart in the wind.",
	"The white unicorn goes insane!",
	"The black blade drains your life!",
	"Silver starlight shines upon your blade!",
	"Death Reborn Revolution!",
	"Silence Wall!",
	"The walls of the dungeon quake!",
	"Dead Scream.",
	"Time Stop!",
	"Getsuga Tensho!",
	"What dance will you use (1/2/3)?",
	"Some no mai, Tsukishiro!",
	"Tsugi no mai, Hakuren!",
	"San no mai, Shirafune!",
	"Ice crackles around your weapon!",
	"The spiritual energy is released!",
	"A cloud of toxic smoke pours out!",
	"For a moment, you feel a crushing weight settle over you.",
	"Your weapon has become more flawed.",
	"Your weapon rattles warningly.",
	"Your weapon has become much more flawed!",
	"Your weapon rattles alarmingly.  It has become much more flawed!",
	"The strange symbols stare at you reproachfully.",
	"Cool it, Link.  It's just a book.",
	"The whisperers berate you ceaselessly.",
	"Following the instructions on the pages, you cast the spell!",
	"What is this strange command!?",
	"You strike the twin-bladed athame like a tuning fork. The beautiful chime is like nothing you have ever heard.",
	"Unrecognized Necronomicon effect.",
	"Unrecognized Lost Names effect.",
	"A cloud of steam rises.",
	"Icicles form and fall from the freezing ceiling.",
	"The altar falls into a chasm.",
	"What game do you think you are playing anyway?",
	"Aivan sairas kaveri kun tuollaista aikoo puuhata!",
	"Insufficient data for further analysis.",
	"Non sequitur. Your facts are uncoordinated.",
	"Type '?' for help.",
	"Invalid command.",
	"What?",
	"WHAT?!",
	"You must be out of your mind!",
	"You're killing me.",
	"Are you sure?",
	"Are you sure you know what you are doing?",
	"Aww, come on!",
	"That makes no sense.",
	"I beg your pardon.",
	"Degreelessness mode on.",
	"Degreelessness mode off.",
	"Syntax error.",
	"That doesn't compute.",
	"I don't understand you.",
	"???",
	"Sure. Piece of cake.",
	"Error.",
	"You can't do that!",
	"Help!",
	"Come again?",
	"Sorry?",
	"Sorry, I'm not sure I understand you.",
	"What's your point?",
	"Unknown command.",
	"Command not found.",
	"An unexpected error has occurred because an error of type 42 occurred.",
	"Somehow, you think that would never work.",
	"Welcome to level 42.",
	"Don't be ridiculous!",
	"lfae aierty agnxzcg?",
	"Soyha, azho bouate!",
	"I don't fully understand you.",
	"Why would anybody want to do THAT?",
	"Yes, yes, now tell me about your childhood.",
	"Satisfied?",
	"Something is wrong here.",
	"There's something wrong with YOU.",
	"You leap up 9' and perform a miraculous 5xSpiral in the air.",
	"Aw, shaddap!",
	"Shut up, smartass!",
	"I see little point in doing that.",
	"Oh, really?",
	"Very funny.",
	"You've got to be kidding!",
	"I'm not amused.",
	"I must have misheard you.",
	"Nothing happens.",
	"Where did you learn THAT command?",
	"When all else fails, read the instructions.",
	"Why not read the instructions first?",
	"Cut it out!",
	"Nothing interesting happens.",
	"Just how exactly am I supposed to do THAT?",
	"That's morally wrong and I won't do it.",
	"I'm not gonna take this abuse.",
	"AAAAAAAAAAAAAHHHHHHHHHHHHHHHRRRRRRRRRRRGGGGGGGGGGG!",
	"No more, if you value your character's life!",
	"Give it up, guy.",
	"Disk error. (a)bort, (r)etry, (f)ail?",
	"The image of the weeping angel is taking over your body!",
	"exerper: Hunger checks",
	"exerchk: testing.",
	"Explore mode is for local games, not public servers.",
	"No port-specific debug capability defined.",
	"A normal force teleports you away...",
	"Do chunks miss?",
	"Mon can't survive here",
	"Jump succeeds!",
	"Doing relocation.",
	"Checking new square for occupancy.",
	"The drawbridge closes in...",
	"Monsters sense the presence of you.",
	"Its mouth opens!",
	"The purple worm opens its mouth!",
	"Crash!  The ceiling collapses around you!",
	"The debris from your digging comes to life!",
	"Powerful winds force you down!",
	"Clang!",
	"Clink!",
	"Clonk!",
	"Whang!",
	"Plop!",
	"Splash!",
	"An alarm sounds!",
	"You penetrated a high security area!",
	"A voice booms: \"The Angel of the Pit hast fallen!  We have returned!\"",
	"It constricts your throat!",
	"Suddenly you cannot see yourself.",
	"The tentacles tear your cloak to shreads!",
	"split object,",
	"testing for betrayal",
	"THUD!",
	"Thump!",
	"CRASH!  You destroy it.",
	"You've disturbed the birds nesting high in the tree's branches!",
	"Some black feathers drift down.",
	"Some scraps of webbing drift down.",
	"You've attracted the tree's guardians!",
	"Some branches are swinging...",
	"Klunk!",
	"Ouch!  That hurts!",
	"WHAMMM!!!",
	"When you can't make them see the light, make them feel the heat.",
	"Every man must be free to become whatever God intends he should become.",
	"There you go again.",
	"Before I refuse to take your questions, I have an opening statement.",
	"If you're explaining, you're losing.",
	"As government expands, liberty contracts.",
	"Each individual is accountable for his actions.",
	"Government's first duty is to protect the people, not run their lives.",
	"The ultimate determinate in the struggle now going on for the world will not be bombs and rockets but a test of wills and ideas.",
	"If we lose freedom here, there is no place to escape to.",
	"This is the last stand on Earth.",
	"You and I have a rendezvous with destiny.",
	"We will preserve for our children this, the last best hope of man on earth, or we will sentence them to take the first step into a thousand years of darkness.",
	"Trust, but verify.",
	"The bombing begins in five minutes.",
	"Some people wonder all their lives if they've made a difference. The Marines don't have that problem.",
	"History teaches that war begins when governments believe the price of aggression is cheap.",
	"Freedom is never more than one generation away from extinction.",
	"Our people look for a cause to believe in.",
	"Today we did what we had to do.",
	"They counted on us to be passive. They counted wrong.",
	"Of the four wars in my lifetime, none came about because we were too strong.",
	"If it moves, tax it. If it keeps moving, regulate it. And if it stops moving, subsidize it.",
	"Tear down this wall.",
	"Open this gate!",
	"Come here to this gate!",
	"There are no easy answers, but there are simple answers.",
	"We must have the courage to do what we know is morally right.",
	"We are never defeated unless we give up on God.",
	"We will always remember.",
	"We will always be proud.",
	"We will always be prepared, so we will always be free.",
	"It is a weapon our adversaries in today's world do not have.",
	"They say the world has become too complex for simple answers. They are wrong.",
	"Don't be afraid to see what you see.",
	"Concentrated power has always been the enemy of liberty.",
	"Nothing happens when you pull the trigger.",
	"Ouch!",
	"It blinds you!",
	"The flying shards of mirror sear you!",
	"Splat!",
	"What a mess!",
	"u_on_sstairs: picking random spot",
	"You reform!",
	"Oh wow!  Great stuff!",
	"Tried to give an impossible intrinsic",
	"The corpse liquefies into a putrid broth, and you slurp it down and die!",
	"Segmentation fault -- core dumped.",
	"Bus error -- core dumped.",
	"Yo' mama -- core dumped.",
	"Suddenly you can see yourself.",
	"You are metal and springs, not flesh and blood. You cannot eat.",
	"Ulch - that was tainted with filth!",
	"Hakuna matata!",
	"That sounds really boring.",
	"There is no longer sufficient tension in your mainspring to maintain a high clock-speed",
	"Player 1 needs food badly!",
	"Player 1, all your powers will be lost...",
	"Player 1, your life-force is running out.",
	"Player 1 is about to die.",
	"The warding sign can be reinforced no further!",
	"Ouch!  You bump into a heavy door.",
	"A breaker penetrator attacks you by surprise!",
	"That was all.",
	"Select an inventory slot letter.",
	"Your key broke!",
	"Klick!",
	"Klick! Klick! Klick! Klick! Klick! Klick! Klick! Klick! Klick! Klick!--More-- How many boxes do you have??? (Don't say 'didn't count'!)",
	"Warning: no monster.",
	"Demogorgon, Prince of Demons, is near!",
	"Lamashtu, the Demon Queen, is near!",
	"Lightning crackles around you.",
	"You're enveloped in flames.",
	"You're covered in frost.",
	"A cascade of filth pours onto you!",
	"A torrent of burning acid rains down on you!",
	"It feels mildly uncomfortable.",
	"A sudden geyser erupts under your feet!",
	"A pillar of fire strikes all around you!",
	"Chunks of ice pummel you from all sides!",
	"A bolt of lightning strikes down at you from above!",
	"The world tears open, and an invincible superman steps through!",
	"A cloud of darkness falls upon you.",
	"You just soiled yourself. Get to the next toilet in 200 turns or all the monsters will laugh at you.",
	"You feel like reading one of Amy's smut fanfics.",
	"So thou thought thou couldst kill me, fool.",
	"You hear air currents.",
	"You hear erotic air current noises.",
	"You hear a farting sound.",
	"You hear a beautiful sound.",
	"You hear a disgusting sound.",
	"Something pulls down your pants.",
	"You feel childish.",
	"You feel like sitting in a corner.",
	"You hear scratching noises.",
	"You hear digging noises.",
	"You can definitely hear human screams.",
	"Somewhere on this level someone is getting killed, right now.",
	"You hear a scream.",
	"You hear a roar.",
	"You hear a chime.",
	"You smell poisonous gas.",
	"You suffocate!",
	"You smell something rotten.",
	"You smell decay.",
	"Your nostrils fill with a sharp stench of smoke.",
	"You feel deathly cold.",
	"You feel sweaty.",
	"You feel bad.",
	"You feel much worse.",
	"You feel even worse.",
	"You don't feel so good...",
	"You feel sick.",
	"You feel apathetic.",
	"You feel bored.",
	"You are suffering from sleep deprivation.",
	"You are suffering from withdrawal symptoms.",
	"A gray stone lands in your inventory! E - A GRAY STONE. YOUR MOVEMENTS ARE SLOWED SLIGHTLY BECAUSE OF YOUR LOAD.",
	"The gnome throws an inventory loss stone!",
	"The hobbit shoots a superscroller stone! The superscroller stone hits you!",
	"The invisible choir segfaults, as the program is in disorder. Congratulations mortal, I'll grant thee the gift of #quit!",
	"Winter is coming! Suddenly, you have no head. You die.",
	"Jubilex spits a segfault venom! Your game is contaminated - consider using #quit!",
	"we hate you, we wish you dead, at least we hope you suffer since we don't trust you a bit. i hope you trust as. best wishes, Rodney Co!",
	"You drop your weapon.",
	"We must stand up with firm resolve to strive for the eradication of terrorism, together with other nations of the world.",
	"Japan supports the U.S. position that it will not bow to terrorism. I think it is only natural for President Bush to hunt down the culprits and take firm steps against this serious crime.",
	"The terrorist acts are extremely heinous and outrageous and cannot be forgiven.",
	"It is a challenge not only to the U.S. but also to democracy, and I am outraged.",
	"Japan-U.S. alliance is becoming bigger and bigger.",
	"Not only for borth countries, but in the Asia-Pacific region and the entire world.",
	"The safe society is crumbling and this is a significant incident.",
	"Logic of previous the Ministry of Posts and Telecommunications that is not clear in meaning is not translate well to K*izumi Cabinet!",
	"I am not a entertainer but a politician.",
	"As K*izumi as the day is long...",
	"I don't think there's any going back to what politics was in this country even three weeks ago.",
	"What will the prime minister do if the anti-reform forces within the Liveral Democratic Party regain power?",
	"People are driving the LDP members, and the LDP members are driveing the party. That is a total reversal of the past.",
	"No pain, no gain.",
	"Here's a present for you. A compact disc of X-JAPAN!",
	"Forever...",
	"Liberal Democrats, You said it's good, Let's go voting.",
	"Perfect circle, round, round, Perfect circle.",
	"You stuck it out despite the pain. I was thrilled. Congratulations.",
	"The illegal entry was one thing and the abduction issue was another, although I think it is necessary for the government to take sufficient measures toward families of the abducted people.",
	"I am starting with the issue that is the most difficalt and which draws the strongest resistance.",
	"Though the ministry is making a hard effort to consider reforms, all of you will just have to wait and see.",
	"People call me a eccentric, but I am a man of reform.",
	"Structural reforms without sanctuaries.",
	"No structural reforms, no economic recovery.",
	"The belief -A pain of today makes affluence of tommorow- ",
	"Haw iz my 100,000,000 gold kasino?",
	"I speek cleerly, My speech is anbiguity.",
	"We hope the Japanese people acknowledge that Japan is a divine nation centering on the emperor.",
	"Lowbrow sex industries are always created first in Osaka. Excuse my language, but it is a spittoon.",
	"When there was a Y2K problem, the Japanese bought water and noodles. Americans bought pistols and guns.",
	"If a blackout happens in America, gangsters and murderers will always come out. It is that kind of society.",
	"I don't have the intention to do a job like this for long.",
	"In rugby, one person doesn't become a star, one person plays for all and all play for one.",
	"None of the Party executives, which whom I met Saturday night, think that I announced my resignation.",
	"The Japanese media have decided that that's what happend, and they feel they have to keep writing that regardless of what we actually said.",
	"How would I be able to leave the country when we must do everything to have the budget pass?",
	"I have been mistreated by the media as if I'm baby picked up under an overpass.",
	"How then could we ensure Japan's public safety and secure the nation's 'kokutai'?",
	"It was not a slip of the tongue. I didn't say I have retracted it.",
	"Berry fine today.",
	"I think I didn't mistake.",
	"foo are yu?",
	"My precious, wheres my precious?",
	"Nasty Hobbitsisisisis...",
	"She will kill them oh yes she will precious.",
	"Whats has its got in its pocketses, hmmm?",
	"No Master Hobbitsisisisis!",
	"The ring was ours for agesisisisis!",
	"Every way is guarded, silly foolsis!",
	"Weees wants some fishises.",
	"We've lost itses we have.",
	"He'll eastus all the world if he getsitses it.",
	"What a dainty little dish you will be for her.",
	"Hobbitses always SOOOO Polite.",
	"Makeses him drop his weapon precious.",
	"He has only four fingers on the black hand.",
	"If you findesis it, give it us back.",
	"You see a longsword (5d5).",
	"You see a Stick (1d77).",
	"Zantetsuken is sharp this night.",
	"Order will crumble.",
	"Obey the power of chaos!",
	"Thisssss dungeon issss guilty.",
	"Your crime isssss life.",
	"The sssentencce isss death!",
	"Your crime issss life. The sssentencce isss death.",
	"Can I get you a chicken? Maybe some white mice and aluminum foil?",
	"Be made to realize my darkness hold.",
	"Power of shadow is infinite.",
	"Soul? What is it of use to?",
	"You cannot avoid the ballyhack.",
	"I gave you the chance of aiding me willingly, but you have elected the way of pain.",
	"Against the power of Mordor there can be no victory.",
	"Do you know how the Orcs first came into being? They were elves once ...",
	"Time? What time do you think we have?",
	"The hour is later than you think. Sauron's forces are already moving. The Nine have left Minas Morgul.",
	"Moria... You fear to go into those mines. The dwarves delved too greedily and too deep.",
	"Always remember ... the Ring is trying to get back to its master. It wants to be found.",
	"Many that live deserve death. Some that die deserve life.",
	"There is only one Lord of the Ring, only one who can bend it to his will. And he does not share power.",
	"There are older and fouler things than Orcs in the deep places of the world.",
	"Who is Your malik? Eric? Corwin?",
	"To barge into our blood feud is to die!",
	"It's true, that uneasy-lies-the-head bit.",
	"I teach you bite of Grayswandir.",
	"This isn't exactly the Olympic Games.",
	"Whose idea was Sokoban???",
	"Somethig comes from above! Be careful!",
	"What is this stair?",
	"Because it's time, I choose this red door!",
	"Tonight, someone dies again...",
	"bootless bootless bootless bootless bootless bootless bootless effort",
	"WRYYYYYYYYY!!!!!!",
	"Make your dream come true in the heaven",
	"I can set forward the time and puch back the time as I like...",
	"Time, move for only me!",
	"A broken clock only have a detrimental effect on our doing.",
	"Nobody can stop the time, now...",
	"Internet Explorer is slow.",
	"Internet Explorer throws off some dorky packets.",
	"Internet Explorer tries to download a 100MB service pack.",
	"Internet Explorer requests a reboot, gets tired of asking nicely, and shuts down your PC.",
	"Internet Explorer quietly installs the latest malware.",
	"Internet Explorer is one big security loophole that allows drive-by downloads of Bandarchor to infect all files on your computer.",
	"Lookup failure, human race will go to ruin.",
	"Fist of the north star is invincible!!",
	"Antharom, Ansalom... what's the difference",
	"Here a louse.",
	"Die. Die. Damn thing is strong!",
	"On Error Resume Next",
	"Where's my damn Intellisense?",
	"Want some new features?",
	"10 PRINT 'I AM THE MASTER OF THE UNIVERSE!'",
	"20 GOTO 10",
	"How do you like 'No Spells'? There's plenty more where that came from!",
	"Have you heard about the new version of Hengband?",
	"Not until the restructure is done!",
	"Modifying this line shouldn't crash anything.",
	"Monkey see, monkey do!",
	"Don't you monkey with the monkey!",
	"Hear no evil, see no evil, speak no evil.",
	"There is danger! Danger!",
	"Base trickery, foul deceit!",
	"Is megalomania THAT bad?",
	"This would be a lot different if this was Sil!",
	"Something something Champs d'Elysees",
	"kill -9 adventurer, kill -9 adventurer",
	"kyary pamyu pamyu!",
	"yariyagattana!",
	"Two Wongs don't make a Wight!",
	"Can't we just fix this with a patch???",
	"Don't you like this game?",

};

const char *
fauxmessage()
{
	if (!rn2(20)) return (generate_garbage_string());
	return (fake_plines[rn2(SIZE(fake_plines))]);
}

const char *
bosstaunt()
{
	return (bosstaunts[rn2(SIZE(bosstaunts))]);
}

/*pager.c*/
