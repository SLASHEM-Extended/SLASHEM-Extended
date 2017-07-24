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


	if (u.usteed) {
	    char steedbuf[BUFSZ];

	    Sprintf(steedbuf, ", mounted on %s", y_monnam(u.usteed));
	    /* assert((sizeof buf >= strlen(buf)+strlen(steedbuf)+1); */
	    Strcat(buf, steedbuf);
	}
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

		if (FarlookProblem || (uarms && uarms->oartifact == ART_REAL_PSYCHOS_WEAR_PURPLE) || (uarms && uarms->oartifact == ART_REAL_MEN_WEAR_PSYCHOS) || u.uprops[FARLOOK_BUG].extrinsic || have_farlookstone() || (uarmc && uarmc->oartifact == ART_LIGHT_OF_DECEPTION)) wakeup(mtmp);

	    if ( (mtmp->data == &mons[PM_COYOTE] || mtmp->data == &mons[PM_STALKING_COYOTE]) && accurate)
		name = coyotename(mtmp, monnambuf);
	    else
		name = distant_monnam(mtmp, ARTICLE_NONE, monnambuf);

	    pm = mtmp->data;
	    Sprintf(buf, "%s%s%s",
		    (mtmp->mx != x || mtmp->my != y) ?
			((mtmp->isshk && accurate)
				? "tail of " : "tail of a ") : "",
		    (mtmp->mfrenzied && accurate) ? "frenzied " :
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
		if (tt == BEAR_TRAP || tt == PIT || tt == SHIT_PIT || tt == MANA_PIT || tt == ANOXIC_PIT ||
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
		if (ublindf && ublindf->otyp == BOSS_VISOR && (is_covetous(mtmp->data) || mtmp->egotype_covetous) )
		    ways_seen++;
		if (Role_if(PM_PALADIN) && is_demon(mtmp->data))
		    ways_seen++;
		if (uarmc && uarmc->oartifact == ART_DEMONIC_UNDEAD_RADAR && is_demon(mtmp->data))
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
		if (uarmh && uarmh->oartifact == ART_WEB_RADIO && (mtmp)->mhp % 9 == 0)
		    ways_seen++;
		if (RngeInternetAccess && (mtmp)->mhp % 9 == 0)
		    ways_seen++;
		if (Numbopathy && Numbed && (avoid_player(mtmp->data) || mtmp->egotype_avoider) )
		    ways_seen++;
		if (Freezopathy && Frozen && mtmp->data->mcolor == CLR_WHITE )
		    ways_seen++;
		if (uarmf && uarmf->oartifact == ART_VERA_S_FREEZER && mtmp->data->mcolor == CLR_WHITE )
		    ways_seen++;
		if (Burnopathy && Burned && infravision(mtmp->data) )
		    ways_seen++;
		if (Dimmopathy && Dimmed && mtmp->m_lev > u.ulevel )
		    ways_seen++;
		if (Sickopathy && Sick && extra_nasty(mtmp->data) )
		    ways_seen++;
		if (uwep && uwep->oartifact == ART_TIGATOR_S_THORN && is_pokemon(mtmp->data) )
		    ways_seen++;
		if (ublindf && ublindf->oartifact == ART_BREATHER_SHOW && attacktype(mtmp->data, AT_BREA))
		    ways_seen++;
		if (uarmc && uarmc->oartifact == ART_POKEWALKER && is_pokemon(mtmp->data) )
		    ways_seen++;
		if (uarmc && uarmc->oartifact == ART_BUGNOSE && (mtmp->data->mlet == S_ANT || mtmp->data->mlet == S_XAN) )
		    ways_seen++;
		if (uarmf && uarmf->oartifact == ART_FD_DETH && (mtmp->data->mlet == S_DOG || mtmp->data->mlet == S_FELINE) )
		    ways_seen++;
		if (uarmg && uarmg->oartifact == ART_WHAT_S_UP_BITCHES && (mtmp->data->mlet == S_NYMPH) )
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
		    if (ublindf && ublindf->otyp == BOSS_VISOR && (is_covetous(mtmp->data) || mtmp->egotype_covetous) ) {
			Strcat(monbuf, "warned of covetous monsters");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (Role_if(PM_PALADIN) && is_demon(mtmp->data)) {
			Strcat(monbuf, "warned of demons");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (uarmc && uarmc->oartifact == ART_DEMONIC_UNDEAD_RADAR && is_demon(mtmp->data)) {
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
		    if (uarmh && uarmh->oartifact == ART_WEB_RADIO && (mtmp)->mhp % 9 == 0) {
			Strcat(monbuf, "internet access");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (RngeInternetAccess && (mtmp)->mhp % 9 == 0) {
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
		    if (uarmf && uarmf->oartifact == ART_VERA_S_FREEZER && mtmp->data->mcolor == CLR_WHITE ) {
			Strcat(monbuf, "freezer vision");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (Burnopathy && Burned && infravision(mtmp->data) ) {
			Strcat(monbuf, "burnopathy");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (Dimmopathy && Dimmed && mtmp->m_lev > u.ulevel ) {
			Strcat(monbuf, "dimmopathy");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (Sickopathy && Sick && extra_nasty(mtmp->data) ) {
			Strcat(monbuf, "sickopathy");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (uwep && uwep->oartifact == ART_TIGATOR_S_THORN && is_pokemon(mtmp->data) ) {
			Strcat(monbuf, "pokemon vision");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (ublindf && ublindf->oartifact == ART_BREATHER_SHOW && attacktype(mtmp->data, AT_BREA)) {
			Strcat(monbuf, "breather show");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (uarmc && uarmc->oartifact == ART_POKEWALKER && is_pokemon(mtmp->data) ) {
			Strcat(monbuf, "pokemon vision");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (uarmc && uarmc->oartifact == ART_BUGNOSE && (mtmp->data->mlet == S_ANT || mtmp->data->mlet == S_XAN) ) {
			Strcat(monbuf, "bugnose");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (uarmf && uarmf->oartifact == ART_FD_DETH && (mtmp->data->mlet == S_DOG || mtmp->data->mlet == S_FELINE) ) {
			Strcat(monbuf, "FD Deth");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (uarmg && uarmg->oartifact == ART_WHAT_S_UP_BITCHES && (mtmp->data->mlet == S_NYMPH) ) {
			Strcat(monbuf, "bitchvision");
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

	if (tnum == SUPERTHING_TRAP) {
		pline("Rien ne va plus!");
		TimerunBug += 1; /* ugh, ugly hack. But nomul() doesn't want to work... --Amy */
		u.riennevaplus = rnd(20);
	}

	if (tnum == ARABELLA_SPEAKER) {
		pline("NETHACK.EXE caused a General Protection Fault at address 000D:001D.");
		TimerunBug += 1;
		u.riennevaplus = rnd(30);
	}

	Strcpy(buf, defsyms[trap_to_defsym(tnum)].explanation);
    } else if(!glyph_is_cmap(glyph)) {
	Strcpy(buf,"unexplored area");
    } else if( glyph == (GLYPH_CMAP_OFF + 44)) {
	Strcpy(buf,"grayout");
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
		if (flags.moreforced) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
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

#define is_cmap_trap(i) ((i) >= S_arrow_trap && (i) <= S_timerun_trap)
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
		} else if (!u.uswallow && !(/*hit_trap && */is_cmap_trap(i)) &&
			   !(found >= 3 && is_cmap_drawbridge(i))) {

		/* traps should be able to be something other than ^ while not cluttering up the explanations! --Amy */

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
	if (found && !RMBLoss && !u.uprops[RMB_LOST].extrinsic && !(uarmh && uarmh->oartifact == ART_NO_RMB_VACATION) && !(uarmh && uarmh->oartifact == ART_WOLF_KING) && !(uamul && uamul->oartifact == ART_BUEING) && !have_rmbstone()) {

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
	    if (!RMBLoss && !u.uprops[RMB_LOST].extrinsic && !(uarmh && uarmh->oartifact == ART_NO_RMB_VACATION) && !(uarmh && uarmh->oartifact == ART_WOLF_KING) && !(uamul && uamul->oartifact == ART_BUEING) && !have_rmbstone()) pline("I've never heard of such things.");
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
	adjective_counter = append(adjectives, (organivorous(ptr)), "organivorous", adjective_counter);
	adjective_counter = append(adjectives, (poisonous(ptr)), "poisonous", adjective_counter);
	adjective_counter = append(adjectives, (regenerates(ptr)), "regenerating", adjective_counter);
	adjective_counter = append(adjectives, (can_teleport(ptr)), "teleporting", adjective_counter);
	adjective_counter = append(adjectives, (is_reviver(ptr)), "reviving", adjective_counter);
	adjective_counter = append(adjectives, (is_reflector(ptr) || (attackdamagetype(ptr, AT_BREA, AD_RBRE) ) ), "reflecting", adjective_counter);
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
	if (is_shade(ptr)) {
		strcat(description, " Non-silver weapons cannot harm it.");
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
	if (is_rat(ptr)) {
		strcat(description, " Can be tamed by throwing cheese.");
	}
	if (ptr->mlet == S_YETI) {
		strcat(description, " Can be tamed by throwing a banana.");
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
	if (ptr->mflags5 & M5_SPACEWARS) {
		strcat(description, " Origin: Castle of the Winds.");
	}
	if (ptr->mflags5 & M5_JOKE) {
		strcat(description, " Origin: Joke monster.");
	}
	if (ptr->mflags5 & M5_ANGBAND) {
		strcat(description, " Origin: Angband.");
	}
	if (ptr->mflags5 & M5_STEAMBAND) {
		strcat(description, " Origin: Steamband.");
	}
	if (ptr->mflags5 & M5_ANIMEBAND) {
		strcat(description, " Origin: Animeband.");
	}
	if (ptr->mflags5 & M5_DIABLO) {
		strcat(description, " Origin: Diablo.");
	}
	if (ptr->mflags5 & M5_DLORDS) {
		strcat(description, " Origin: Dungeon Lords.");
	}
	if (ptr->mflags5 & M5_VANILLA) {
		strcat(description, " Origin: Vanilla NetHack or SLASH'EM.");
	}
	if (ptr->mflags5 & M5_DNETHACK) {
		strcat(description, " Origin: DNetHack.");
	}
	if (ptr->mflags5 & M5_RANDOMIZED) {
		strcat(description, " Its stats and attacks vary from game to game.");
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
		case AD_DIMN: return "dims you";
		case AD_FEAR: return "causes fear";
		case AD_DISP: return "pushes you away";

		case AD_NPRO: return "negative protection";
		case AD_POIS: return "poison damage";
		case AD_THIR: return "thirsty attack";
		case AD_LAVA: return "lava";
		case AD_FAKE: return "plines";
		case AD_LETH: return "lethe";
		case AD_AMNE: return "map amnesia";
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

		case AD_ICEB: return "ice blocks";
		case AD_VAPO: return "vaporization";
		case AD_EDGE: return "stone edge";
		case AD_VOMT: return "vomiting";
		case AD_LITT: return "litter";
		case AD_FREN: return "frenzy";
		case AD_NGEN: return "negative enchantment";
		case AD_CHAO: return "chaos";
		case AD_INSA: return "insanity";
		case AD_TRAP: return "trapping";
		case AD_WGHT: return "weight increase";
		case AD_NTHR: return "nether";
		case AD_RNG: return "RNG intervention";
		case AD_MIDI: return "identity-specific attack";
		case AD_CAST: return "evil spellcasting";

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

	if (DeformattingBug || u.uprops[DEFORMATTING_BUG].extrinsic || have_deformattingstone()) {
		strcat(description, "Pokedex communication failure. Damn.");
		return description;
	}

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

		if (KnowledgeBug || u.uprops[KNOWLEDGE_BUG].extrinsic || have_trapknowledgestone()) {
			pline("That is a trap.");
			return 0;
		}

		if (u.dz) {
		    if (u.dz < 0 ? (tt == TRAPDOOR || tt == HOLE || tt == SHAFT_TRAP || tt == CURRENT_SHAFT) :
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

static NEARDATA const char * const soviettaunts[] = {
"Vy lamer, vy boites' detskikh lichey na rannikh urovnyakh podzemel'ya ili kak? Vernites' k igre vanili!",
"Tip ledyanogo bloka nadeyetsya, chto vas ub'yut po-nastoyashchemu nespravedlivo. Khar Khar Khar.",
"Znayete li vy, chto igrayete glupyy variant, kotoryy povtorno vstavlyayet oshibku povtornoy initsializatsii taymera ikru monstra, potomu chto sovetskaya Pyat' Lo ochen' glupa?",
"Sozdatel' slesh ikh ne znayet, kak zastavit' stetoskop rabotat'!",
"Komu-to eto ne nravitsya, yesli rozhki yedinorogov lomayutsya. No ya sdelal eto tak, chtoby oni vmesto etogo vyzyvali plokhiye posledstviya. KHA KHA KHA KHA KHA KHA KHA! Podpis': tip ledyanogo bloka.",
"Sovet vsegda imel khudshuyu otsenku v khimii i poetomu sdelal tak, chtoby nabor khimii ne rabotal. Ty seychas schastliv?",
"Yesli vy zabirayete artefakt, poka vash uroven' slishkom nizok, on delayet BUM i vy umirayete. Khvalite sovetskuyu pyaterku!",
"Vy, lamer, vy tol'ko igrayete v etom rezhime, potomu chto boites' nepriyatnykh vysechek! Glupyy nub, malen'koye ditya!",
"Yesli vy povyshayete svoy navyk verkhovoy yezdy, vashe vosstanovleniye zdorov'ya budet umen'shat'sya, potomu chto Sovetskiy chelovek nastroil igru sovershenno zabavno.",
"Chto, vy khotite dopolnitel'nuyu regeneratsiyu many? Pochemu togda vy igrayete v rezhime, kogda on otklyuchen? Vy dolzhny byt' umstvenno otstalymi!",
"Zameshatel'stvo i oglusheniye budut trakhat' vas tak zhe, kak vanil', potomu chto eto slesh ikh rasshiren i vedet sebya kak vanil'. Potomu chto sovetskiy tip Ledyanoy Blok ne lyubit variantov. Vmesto etogo on delayet udalennuyu i otmenennuyu versiyu i nazyvayet eto 'variantom'.",
"V igre yest' opredelennaya palochka, kotoraya mozhet udalit' monstrov, kotorykh net v vanili. Pozhaluysta, derzhites' za neye, potomu chto vy tozhe ne dolzhny sushchestvovat'.",
"Tip ledyanogo bloka ne nravitsya raznoobraziyu i poetomu sdelan tak, chto vy pochti vsegda poluchayete svoy skuchnyy spetsificheskiy artefakt. Radost'.",
"Poshel ty na khuy, yesli dumal, chto legko srovnyayesh' zheleznyye reshetki! Teper', dazhe yesli vy ispol'zuyete oruzhiye, kotoroye dolzhno byt' v sostoyanii sdelat' eto, ono pochti vsegda provalivayetsya. I svetovoy mech ne byl vypolnen, potomu chto sovet lenivyy.",
"Vam nravyatsya labirinty? YA dumayu, vam osobenno ponravitsya to mesto, gde tip ledyanogo bloka sdelal eto tak, chto vy vsegda mozhete vykapyvat' tol'ko odnu plitku s pomoshch'yu svoyey zhezla kopaniya. Ne trat'te vpustuyu svoi raskhody, KHAR KHAR!",
"Da, 'kto-to' khochet imet' vozmozhnost' delat' beskonechnyye altari. Udaleniye elementov na nikh ne privedet k ikh ischeznoveniyu. No KHA KHA KHA eto mozhet vyzvat' plokhiye posledstviya KHA KHA KHA!",
"Kak chast' filosofii dizayna VOZVRASHCHAT'SYA VOZVRASHCHAT'SYA VOZVRASHCHAT'SYA, kol'tsa snova pochti vsegda proglatyvayutsya, yesli vy uronite ikh v rakovinu.",
"Tip ledyanogo bloka deystvitel'no nenavidit vashi kishki, i, sledovatel'no, povtorno realizovana funktsiya, kotoruyu v bukval'nom smysle nikto ne lyubit: sobiraniye predmetov snova poydet povorot. Potomu chto Sovetskaya Pyataya Lo nenavidit vse zhivoye.",
"Raznoobraziye - der'mo, i poetomu zaplesnevelyye trupy vsegda prevrashchayutsya v gribok, a ne v sharik, zhele ili puding. Poluchayte udovol'stviye, pozvolyaya vashim chuvstvam prituplyat'sya!",
"Vy ne mozhete gallyutsinirovat' lordom-demonom! Kreml' i Yozef Putin zapreshchayut eto, i poetomu gallyutsinatsiya tol'ko pokazhet tipy monstrov, kotoryye ne unikal'ny, dazhe yesli eto glupo.",
"V Sovetskoy Rossii net ekzoticheskikh domashnikh zhivotnykh. Tol'ko sobaki i koshki sushchestvuyut, i vse te, kto khochet drugogo pitomtsa, mogut prosto trakhat'sya.",
"Yesli vy dostatochno glupy, chtoby ostavit' svoyu sobaku pozadi, ona ochen' bystro zabudet, chto vy kogda-libo sushchestvovali. Da, dostatochno neskol'kikh soten oborotov, kak v vanili, i eto, blyat', ne tak uzh vazhno, naskol'ko on byl syt. Potomu chto sovetskaya Pyat' Lo ne yavlyayetsya poklonnikom khoroshego igrovogo dizayna.",
"Vashe domashneye zhivotnoye tol'ko usugubit vas, s''yev trupy trupov, a takzhe trupy drugikh byvshikh pitomtsev. Potomu chto razrezat' ikh - eto govno! Smiris' s etim!",
"Vy ne mozhete priruchit' veshchi, kotoryye ne dolzhny byt' prirucheny! V otlichiye ot seksual'nogo plyushcha u vas ne budet absolyutno nikakogo shansa poluchit' total'nogo rytsarya-ranenogo, i vy umrete ot goloda, yesli poprobuyete!",
"Yeda v etom rezhime ochen' medlennaya. No vy, navernoye, ozhidali etogo. Ne ozhidayte, chto smozhete s''yest' trup drakona vo vremya bega vozneseniya - ili, yesli na to poshlo, s''yest' trup drakona do togo, kak on stanet slishkom starym, chtoby yest'. Yesli vas prervali, a zatem vozobnovili yest', vy poluchite smertel'no bol'noye. I Sovetskaya Pyaterka Lo odobryayet etu chush'.",
"Vash pitomets mozhet zabrat' konteynery, kotoryye ne pustyye, potomu chto Sovet sposoben soprotivlyat'sya snu, vypolnyaya klonirovaniye kreditov. Etot merzkiy ekspluatator.",
"Verkhovaya yezda pri konflikte dolzhna byt' nevozmozhnoy, po krayney mere, v etom rezhime, kotoryy byl sdelan tipom ledyanogo bloka! Vy budete postoyanno padat'. I togda sovetskiy budet smeyat'sya 'KHAR KHAR KHAR' vse vremya.",
"Vam pridotsya zashchishchat' lavochnikov i svyashchennikov ot domashnikh zhivotnykh. Potomu chto kto-to reshil, chto igra dolzhna byt' ochen' glupoy. I vam deystvitel'no ponravitsya, yesli vash yedinstvennyy postroyennyy khram prevratitsya v khram s privideniyami - na samom dele, on zastavit vas postroit' nastoyashchiy khram, osvyashchennyy v Sovetskoy Pyaterke!",
"Monstry, kotorym nuzhno nalozhit' zacharovannoye oruzhiye, sovershenno nevozmozhno pobedit', yesli u vas net takogo oruzhiya. Raduysya, chto ya poka ne delal razocharovaniy s etim dostoyaniyem! Ili, vozmozhno, ya sdelal ikh k nastoyashchemu vremeni. Potomu chto ya pochti takoy zhe zloy, kak tip ledyanogo bloka. Vypusk yazyka Kolon.",
"V etoy strane kazhdoye derevo prinosit plody. No ni u odnogo iz nikh net ekzoticheskikh sortov fruktov. V Sovetskoy Rossii slishkom kholodno dlya limonnogo dereva! Pochemu by tebe ne pereyekhat' v drugoye mesto? Vy umstvenno otstalyye ili chto-to v etom rode?",
"Vy mozhete legko sozdat' svoyu udachu s pomoshch'yu dragotsennykh kamney i yedinorogov, i vy, veroyatno, sdelayete eto, vy, sosunok. Pochemu by vam ne igrat' v ne sovetskom rezhime, a gde vse luchshe?",
"Vse spetsial'nyye urovni budut sushchestvovat' v nikh, vklyuchaya zlyye, kotoryye ya sozdal, potomu chto kharkharkhar khar!",
"V sovetskom rezhime, v kotorom vy igrayete, uroven' slozhnosti ne umen'shayetsya sluchaynym obrazom. Potomu chto boleye pozdniye chasti igry dolzhny byt' neigrabel'nymi.",
"Kto-to schital, chto izryadnaya portsiya gniloy pishchi 'slishkom zloy', khotya eto pochti nikogda ne sluchalos', i u vas yest' sposoby vylechit' yego. Takim obrazom, yego udalili, kak i vse ostal'noye, potomu chto vy igrayete yedva rasshirennyy, no ochen' udalennyy mod.",
"Vy poluchite vse vnutrennosti ot s''yedaniya trupov, potomu chto sovetskaya Pyat' Lo zabyla, chto yest' monstry boleye vysokogo urovnya, u kotorykh uzhe yest' boleye vysokiye shansy chto-to dat'.",
"Kha-kha-kha, yest' trupy tritonov, ne dast vam mnogo many. Prichina? Sovetskiy chelovek glupyy i ne umeyet programmirovat' igry! Vy nikogda ne zadumyvalis', pochemu on otkazalsya ot svoyey der'movoy igry i ostavil yeye tam, chtoby sgnit'?",
"Gnilyye ili v protivnom sluchaye plokhiye banki ne sdelayut vas boleye golodnymi tol'ko potomu, chto vy ikh yedite. Eto yeshche raz demonstratsiya togo, chto ikh dizaynery ne lyubyat raznoobraziye. V samom dele, raznoobraziye soset i igry budet luchshe bez nego. Ili, po krayney mere, imenno etogo tipa ledyanoy blok khochet, chtoby vy verili.",
"'Sovetskaya pyaterka' LYUBIMAYA NENAVIST' NENAVIDETS NENAVIDIT NEL'ZYA, chtoby vy nikogda ne mogli po-nastoyashchemu spastis' ot gniloy pishchi. Yesli vy potratite svyatuyu vodu, chtoby blagoslovit' yeye, vy vse ravno riskuyete byt' vybitym i ubitym. Moya top-model' Bettina khotela by pogovorit' s nim paru slov i toptat' yego sharami s yeye botinkami na vysokikh kablukakh.",
"Izmeneniya v pitanii, kogda vy golodny ili syty, ne sushchestvuyut, potomu chto eto Glupyy Rezhim (TM) Chistyy vzlom. Yesli vy teryayete soznaniye, vy budete golodat' do smerti vo vremya yedy, potomu chto vy poteryayete bol'she pishchi za khod, chem smozhete vernut'sya, a yesli nemnogo pereyest', vy zadokhnetes' do smerti. I tip ledyanogo bloka ser'yezno dumayet, chto eto khoroshiy igrovoy dizayn.",
"Pochti nevozmozhno umeret' v samyy pervyy khod, no komu-to ne nravitsya davat' soobshcheniye, kogda vy umirayete na vtoroy khod. Po krayney mere, vy poluchite drugoye soobshcheniye, kotoroye izdevayetsya nad sozdatelem slesh ikh, potomu chto on, chert voz'mi, zasluzhivayet togo, chtoby nasmekhat'sya nad yego glupymi tvoreniyami.",
"Svitok otpugivayushchego monstra budet besporyadochno goret' namnogo rezhe, no eto vse yeshche mozhet sluchit'sya, kha-kha!",
"Vash pervyy uroven' budet dlit'sya vechno bez vsyakoy vidimoy prichiny. Sovetskiy tip ledyanogo bloka, vidimo, vynul stranitsu iz knigi etogo belogo parnya ili kak by tam ni bylo yego imya, i na samom dele veril svoim yereticheskim myslyam, potomu chto on (sovetskiy) ne ispol'zuyet svoy mozg. Neveroyatno.",
"Utechka zhizni vsegda budet zabirat' u vas uroven', potomu chto trakhnite vas, glupyy igrok. U vas ne dolzhno byt' shansa.",
"Dazhe yesli vy povyshayete uroven', vy ne poluchite polnykh liniy, potomu chto eto ne ochen' khorosho sproyektirovannaya igra.",
"Vam vsegda nravilsya tot fakt, chto unichtozheniye predmetov proiskhodit tak chasto? Shag na odnoy lovushke ognya, BUM, poproshchaysya so vsemi svoimi zel'yami. Po krayney mere, slesh ne vklyuchal patch dlya unichtozheniya palochki. Vse zhe.",
"Vy ne dolzhny poluchat' zhelaniya ot glubokovodnykh demonov! Tol'ko melkiye budut rabotat', i dazhe yesli eto prosto glupo, sozdatel' slesh ikh reshil sdelat' imenno tak. Vmesto etogo vy dolzhny ispol'zovat' rasshirennuyu kosuyu chertu.",
"Algoritm vosstanovleniya podzemel'ya namnogo medlenneye iz-za kommunizma; Rabochim platyat za kolichestvo chasov, kotoryye oni pritvoryayutsya rabotayushchimi. Po krayney mere, eto oznachayet, chto oshibka sokhraneniya igry budet proiskhodit' rezhe...",
"Yesli vy voydete v spetsial'nuyu komnatu, monstry ne prosnutsya. Potomu chto avtor etogo glupogo varianta khochet sdelat' vse legko, tak chto vy mozhete kolot' monstrov, poka oni spyat.",
"Vam nravyatsya plokhiye interfeysy? Konechno, vy delayete! Dazhe yesli vam povezet i u vas yest' effekt polnoy inventarizatsii, on ne popadayet v konteynery, potomu chto tip ledyanogo bloka slishkom leniv, chtoby programmirovat' igru, kotoraya na samom dele khorosha.",
"Shtabery predmetov nikogda ne budut soprotivlyat'sya manipulirovaniyu, potomu chto eto slesh, i vy dolzhny zhdat', poka u vas ne ostanetsya dyuzhiny svitkov broska charov, prezhde chem ikh blagoslovit'. Zatem my vernem moshennichestvo, gde eti svitki ne mogut byt' natseleny igrokom.",
"Vy mozhete ispol'zovat' artefaktnyye klyuchi na obychnykh dveryakh, v sootvetstvii s sovetom 'potomu chto inache net nikakikh osnovaniy dlya polucheniya klyuchey artefakta, i eto ne imeyet smysla'. ZDRAVSTVUYTE??? Kto-to zabyl, chto glupyye veshchi sushchestvuyut tol'ko iz-za dverey v bashne?",
"Nikogda monstry s dal'noboynym oruzhiyem ne poluchat dopolnitel'nyye boyepripasy. Potomu chto eto bylo by izmeneniyem ot vanili, i Emi byla by toy, kto sdelal eto, poetomu eto dolzhno byt' glupo i, vozmozhno, ne tak.",
"Yesli igra prevrashchayet monstra, kotoryy mozhet spryatat'sya pod predmetami, on ne poluchit predmet, kotoryy mozhno skryt'. Dazhe zolotoy kusok. Potomu chto eto ne bylo sdelano v vanile, i po opredeleniyu varianty nikogda ne smogut izmenit' veshchi iz vanili.",
"Sovetskaya Pyaterka pytalas' ubrat' egoizma. No on poterpel neudachu, potomu chto yego variant - fuflo, i poetomu tol'ko displey dlya takikh monstrov isporchen. Bwar Khar Khar Khar on on on kha kha kha govorit tip ledyanogo bloka!",
"Neobychnyye monstry v Sovetskoy Rossii ne veshch'. Oni khoteli by polnost'yu otmenit' neobychnyye flagi, no, yesli ne poluchilos', razrabotchiki Glupyy variant (TM) sdelali tak, chto eti monstry vse yeshche otnositel'no rasprostraneny. Yesli vas ub'yut chto-to zloye, vasha vina!",
"Pri boleye vysokikh trudnostyakh s monstrami vy bol'she ne uvidite monstrov nizkogo urovnya. Eto osobenno zabavno na bege voskhozhdeniya, potomu chto togda igra budet postoyanno poyavlyat'sya, provalivayas' iz der'ma i pinat' vash zhalkiy zad. KHAR KHAR.",
"Vy igrayete v rezhime, v kotorom monstry vsegda porozhdayut vrazhdebnost', za ochen' nemnogimi isklyucheniyami. Potomu chto nikto ne lyubit rubit' ikh, i poetomu monstry takzhe ne lyubyat igrokov, kotoryye igrayut v nego.",
"V vanili net soobshcheniya, yesli nevidimyy monstr dyshit, plyuyet ili strelyayet v vas, i poetomu varianty tozhe mogut etogo ne delat'! Ili, po krayney mere, Sovet skazal by vam ob etom, khotya eto polnyy i polnyy bred.",
"Vy poluchite vyzov busheval, o da, vy budete. V dal'neyshem lichi budut nanosit' vsevozmozhnyye nepriyatnyye zaklinaniya, potomu chto eto ikh slesh, kotoryy (v otlichiye ot yeye varianta v Emi) ne pytayetsya sokhranit' veshchi normal'nymi.",
"Yesli vam ne povezlo nastol'ko, chto monstr budet slabeye, vasha sila rezko upadet do ochen' nizkogo znacheniya, i vy takzhe mozhete sdat'sya. No, po krayney mere, eto vyzhivaniye, v otlichiye ot zaklinaniya kantselyarskikh paralichey, kotoroye ozaglavleno 'Ty teper' mertv'.",
"Monstry, atakuyushchiye veb-sayty, generiruyut mnozhestvo sluchaynykh lovushek, potomu chto tip ledyanykh blokov lyubit nakruchivat' vas na kha-kha-kha!",
"K sozhaleniyu, sovetskaya Pyat' Lo ne lyubit spetsial'nykh atak, kogda monstry mogut udarit' vas v orekhi ili podobnyye veshchi, i, takim obrazom, oni sluchayutsya gorazdo rezhe. Veroyatno, eto myagkoye yaytso, kotoroye mozhno zalatat' dazhe za paru temno-sinikh krossov 'Ivonne', etot slabak!",
"Yest' odna veshch', kotoruyu sdelal ledyanoy blok, kotoryy vam mozhet dazhe ponravit'sya: on sdelal eto tak, chtoby vashe oborudovaniye ne stanovilos' skuchnym. Teper' postroyte yemu zolotoy tron.",
"Osteregaytes' pogloshchayushchikh monstrov, potomu chto oni vsegda budut vas bit'! Tip ledyanogo bloka nenavidit sluchaynost' s ognennoy strast'yu i, sledovatel'no, otklyuchayet vse vyzovy Amn's rn2 bez prichiny, krome kak byt' ogromnym chlenom.",
"Pomnite amneziyu ot chistki vanili? Togda vy budete LYUBIT' fakt, chto on stirayet stol'ko zhe, skol'ko i zdes', potomu chto eto slesh, i lyuboye izmeneniye, sdelannoye Emi, dolzhno byt' der'mom, dazhe yesli ono ponravitsya kazhdomu zdravomyslyashchemu igroku. Vernis'! Vernis'! Vernis'!",
"Dumayu, ty lyubish' russkikh zhenshchin, potomu chto oni - polnyye soblazniteli i zastavyat tebya snyat' vse svoye snaryazheniye i peredat' yego im. Net, spasitel'nyye broski bol'she ne sushchestvuyut, potomu chto Sovet udalil ikh vmeste so vsem ostal'nym. I ya nakleivayu tebe yazyk, potomu chto ty prodolzhayesh' igrat' v etu glupuyu versiyu.",
"Chto, vy khotite umen'shit' uron? Togda vam nuzhno poluchit' klass broni vyshe minus dvadtsati, i vam eto ne udastsya. Potomu chto eto rezhim ledyanogo bloka, i u vas ne dolzhno byt' shansa. Daye-na-da-na!",
"Vy ne mozhete izbezhat' napadeniy monstrov. V vanili oni vsegda srabatyvayut, i poetomu oni zdes'. Dazhe yesli oni chto-to deystvitel'no zlyye. Luchshe poigrayte v dzen-povedeniye, potomu chto v protivnom sluchaye vy poluchite udar. Nu chto zh, eto vse ravno proizoydet, tak chto ne bespokoytes'. Pochemu by tebe prosto ne sprygnut' s mosta?",
"Eto slesh, gde vy vsegda budete poluchat' polnyy uron. Poluchayete skidku, potomu chto u vas vysokiy uroven'? Net, konechno, net. Potomu chto gde by my byli, yesli by vyravnivaniye deystvitel'no sdelalo vashego personazha boleye sposobnym delat' veshchi? V Sovetskoy Rossii etogo ne mozhet byt'.",
"KTO-TO udalil graficheskuyu stroku dialoga iz soblazneniya sukkub. Potomu chto etot chelovek - neveroyatno khrabryy chelovek, i yemu nuzhno zapretit' trakhat'sya s chistym iskhodnym kodom.",
"Beremennost' ne sushchestvuyet. Po krayney mere zdes' eto ne tak. Ili, mozhet byt', vy prosto yadovity, i lyuboy rebenok srazu zhe rasplavitsya v vashem zheludke. Vo vsyakom sluchaye, yeshche odin primer sovetskoy kontseptsii 'YA TRUDNO NENAVISTI VSE VSE, CHTO YEST' NOVAYA I INNOVATSIONNAYA'.",
"Nekotoryye kommunisticheskiye neudachi ne sposobny ispol'zovat' fazovuyu dver' posle nayezda na imitatsiyu, kotoraya pritvoryayetsya dver'yu, poetomu oni nachnut poyavlyat'sya pozzhe. Potomu chto sovetskaya Pyat' Lo - ochen' plokhoy igrok. Poluchite eto, on dazhe udalil lovushki, poyavlyayushchiyesya v koridorakh, potomu chto on super glupyy i prodolzhayet vytesnyat' svoikh pitomtsev v nikh! Kakoy polnyy nub!",
"Tip ledyanogo bloka nevospriimchiv k skuke i poetomu sdelan tak, chto v Geennome sushchestvuyut tol'ko labirinty. Vy, s drugoy storony, navechno proklinayete otstalogo razrabotchika, kotoryy otkazalsya ot ochen' zhelannogo 'Gehennom', takzhe mogut imet' izmeneniya v komnatakh i koridorakh.",
"Nekotoryye kommunisticheskiye pediki ne v sostoyanii sdelat' razumnuyu veshch', kogda stolknulis' s troll'-kholla na vtorom urovne, i poetomu sdelali tak, chtoby oni nachali poyavlyat'sya pozzhe. Vy igrayete v eti pediki, ikh variant, tak chto vam tozhe legko, lamer! Poluchite udovol'stviye i nachnite igrat' slesh!",
"Spetsial'nyye urovni ne mogut imet' predmetov, skrytykh v stenakh, potomu chto OGO MOY BOG, KTO-TO OTKAZALSYA IZMENIT' VESHCHI IZ VANILI. I kto-to byl Emi, to yest' bukval'noye opredeleniye zla, poetomu on dolzhen nemedlenno poluchit' topor. Da, eto to, chto razrezayet ikh.",
"Tip ledyanogo bloka umen'shal veroyatnost' poyavleniya nepriyatnykh lovushek, potomu chto on takoy plokhoy igrok, chto ne mozhet spravit'sya s nimi. Yesli vy vyigrali igru, eto nichego ne znachit, potomu chto vy igrayete v rezhime s men'shey slozhnost'yu!",
"Tot, kto polnost'yu nichego ne ponimayet v chem-libo, sdelal eto tak, chto sluchaynyye zhertvenniki ne mogut byt' neprisoyedineny. Yesli vy sprosite menya, to sushchestvovaniye sovetskoy Pyat' Lo tozhe ne imeyet smysla !!!",
"Eto rezhim ledyanogo bloka, gde elementy, kotoryye mogut generirovat' v stekakh, nikogda ne budut poluchat' proizvol'nuyu dopolnitel'nuyu dobavlennuyu summu. Potomu chto v kommunizme vse normiruyetsya, i tenevoye pravitel'stvo garantiruyet, chto vy nikogda ne poluchite bol'she, chem drugiye.",
"Pechal'no izvestnyy modeder po imeni sovetskiy ne ponimal, pochemu ya sdelal vozmozhnym ocharovyvat' kamni i zheleznyye shariki. Poetomu on vernul yeye, tochno tak zhe, kak on otmenil vse ostal'noye. Prostoye sushchestvovaniye yego varianta pokhodit na bol'shoy znak 'yebat' tebya' dlya vsekh ser'yeznykh igrokov-khakerov.",
"Vy nikogda ne naydete pustykh ili otmenennykh palochek, potomu chto Sovetskaya pyaterka Lo ne khochet, chtoby podzemel'ye chuvstvovalo sebya 'zhivym', to yest' kak budto drugiye avantyuristy uzhe proshli. Ili mozhet byt', eto potomu, chto yemu ne nravitsya sluchaynost', i poetomu on dolzhen igrat' chto-to vrode pervoy partii Mario, gde vse predopredeleno, kak tol'ko vy nachinayete igru.",
"Khar khar khar, monstry v spetsial'nykh komnatakh nachinayut bodrstvovat'! U tebya net shansa! Ty glupyy kon'-okhuitel'nyy futbolist, ty!",
"Poskol'ku tip ledyanogo bloka absolyutno nesposoben poluchit' chto-to pravil'noye, on sdelal eto tak, chto morskiye monstry snova uzhasno slaby na zemle. Eto delayet ikh slishkom legko pobedit', i, takim obrazom, pobeda v etom rezhime vryad li yavlyayetsya dostizheniyem.",
"Sovetskaya pyaterka snova vvela staryy zhuk, v kotorom ubiystvo monstra, pogloshchayushchego vas, ne pozvolyayet yemu udalyat' predmety. I v to vremya kak on byl na etom, on takzhe sdelal eto tak, chtoby monstry v stenakh ne mogli ponizit' punkty takzhe, tol'ko nazlo igroka.",
"V etom duratskom sposobe otstalosti, monstry dolzhny byt' v sostoyanii ostavlyat' trupy i imet' ruki, inache oni ne smogut ostavit' kaplyu smerti. I kopy ne mogut ostavit' ni odnogo. Zachem? Ponyatiya ne imeyu! Etot mod ne imeyet smysla!",
"Tip ledyanogo bloka vnov' predstavil vse glupyye ogranicheniya monstrov, ispol'zuyushchikh predmety. V tom chisle te, gde na samom dele imeyet smysl ispol'zovat' monstry.",
"Ne pytaytes' zhelat' artefakta. V sluchaye neudachi, i, veroyatno, vy poluchite nichego, v otlichiye ot slesh em rasshirennyy. No, pover', slesh - eto prosto plokho, i tvorets dolzhen chuvstvovat' sebya plokho.",
"Znayete li vy, chto Sovet nenavidit vse, chto vy delayete? Delo v tom, chto on nastaivayet na tom, chto polimorfizatsiya v bol'shoy monstr vsegda narushayet vashi dospekhi, plashch i vse ostal'noye. Dazhe yesli monstr byl prosto kroshechnym nemnogo bol'shim. Razve ty ne lyubish' yego seychas?",
"Yesli vy poluchite bozhestvennuyu koronatsiyu ili kul'minatsiyu, vy vsegda poluchite tot zhe dar. Potomu chto v kommunizme vse odno i to zhe skuchnoye zanyatiye, mozhet i ne byt' raznoobraziya voobshche. Etot variant plokhoy, i sozdatel' dolzhen chuvstvovat' sebya plokho!",
"Da, konechno, vy mozhete predlagat' trupy tak, kak vy khotite! Sovetskiy tvoy drug, i sdelal tak, chtoby altar' nikogda ne ischeznet! Otdavayte zhertvu i prodolzhayte delat' eto, poka ne umrete ot starosti. No kazhdyy raz, kogda vy predlagayete trup, mozhet sluchit'sya chto-to plokhoye. KHAR KHAR KHAR.",
"V Sovetskoy Rossii yest' raketka zashchity, naryadu s vymogatel'stvom vykupa i raskhishcheniyem. Nu, vy znayete, kogo vy dolzhny obvinyat', yesli vash tselitel' i tselitel' i zver'ki budut zverski ubity mestnym naseleniyem gnomicheskikh min. YA ne vinovat, chto opredelennyy razrabotchik khochet plokho skonstruirovannuyu igru ...",
"K sozhaleniyu, amneziya budet povtorno randomizirovat' opisaniya predmetov gorazdo rezhe v etom rezhime. No po krayney mere on vse yeshche mozhet sdelat' eto, i vy budete ochen' rady, yesli eto proizoydet! Zakrytaya liniya s zamknutym konturom!",
"Slesh em delayet eto tak, chto neudachnoye chteniye knigi zaklinaniy vsegda paralizuyet vas dlya nechestivogo dolgogo vremeni. YA ispravil eto, no, konechno zhe, on dolzhen byl byt' izmenen obratno dlya glupogo rezhima gluposti, kotoryy nazyvayetsya slesh ikh.",
"Khoroshiye interfeysy dlya NENAVIST' NEDVIZHIMOSTI. On sdelal eto tak, chto moye prekrasnoye otobrazheniye ostavsheysya pamyati zaklinaniy prosto PROSHLO. I dobavil ochen' neubeditel'nyy, somnitel'nyy, neubeditel'nyy povod dlya etogo. Nado otpolirovat' svoye urodlivoye litso sapogom na vysokikh kablukakh v otmestku.",
"Tip ledyanogo bloka ne ochen' chasto nakladyvayet zaklinaniya, ili, mozhet byt', yedinstvennyye zaklinaniya, kotoryye on ispol'zuyet, iskhodyat iz yego priklada. Vo vsyakom sluchaye, on dumayet, chto Emi sdelat' zaklinaniya legche brosit' eto plokho, tak zhe, kak on dumayet, chto vse, chto ona delayet, plokho, i poetomu otmenil yego tak zhe, kak on otmenil chto-nibud' yeshche!",
"Ne nadeysya, yesli nachnesh' s neskol'kikh knig zaklinaniy s boltom sily. U vashey dovol'no volshebnoy devushki ne budet dopolnitel'noy pamyati zaklinaniy, i poetomu ona tak zhe ne mozhet primenyat' yeye, kak v protivnom sluchaye. Perestan'te igrat' v der'movyy sovetskiy rezhim, i eta problema ischeznet!",
"Vy ne mozhete sedlat' i katat'sya na chem ugodno. Sovetskiy, velikolepnyy sozdatel' samogo bol'shogo varianta razrusheniya seti, vernul vse glupyye ogranicheniya na yerundu i budet smeyat'sya nad toboy navsegda, poka ty ne smozhesh' katat'sya s sushchestvom, kotoroye inache sdelalo by chertovski khoroshuyu goru.",
"Vsplesk energii byl ochen' neuravnoveshennoy tekhnikoy v regulyarnom razreze. Ugaday, kto snova yego odolel? Aga. Po krayney mere, my ne realizovali vozmozhnost' ispol'zovat' yego v sochetanii s tsepnym blits, potomu chto eto bylo by deystvitel'no neuravnoveshennym.",
"On, on, on kha-kha-kha, vodnyye plitki schitayutsya bezopasnymi mestami dlya teleportatsii, potomu chto tak govorila 'Sovetskaya pyaterka'. Nu, po krayney mere, yesli vy umeyete plavat'. Vo vsyakom sluchaye, kto-to zabotitsya ob oborudovanii, stanovyashchemsya rzhavym.",
"Chto, vy khotite zashchitit' vashi veshchi ot erozii, ispol'zuya zhir? Ni za chto! Sozdatel' varianta fignya etogo ne pozvolyayet. I vy deystvitel'no glupy, potomu chto vy ne igrayete v svoy variant Emi, dazhe nesmotrya na to, chto eto namnogo luchshe v kazhdom otdel'nom sluchaye.",
"Glupost' v generatsii statuetok zdes' drugaya. Tip ledyanogo bloka pochemu-to ne nravitsya, chto ikh skryvayut pod monstrami, prosto iz strakha poluchit' statsionarnuyu plesen', kotoraya nemnogo glubina. Pozvol'te mne dogadat'sya: on dostatochno glup, chtoby atakovat' vse, chto dvizhetsya ili vyglyadit tak, kak budto on mozhet dvigat'sya, i, takim obrazom, yego vse vremya ubivayut.",
"Neispravnyye portaly v Sovetskoy Rossii terpet' ne mogut, i poetomu vy mozhete delat' portal'nyye tantsy vechno, chto prosto tak glupo. YA ne mogu poverit', chto sovetskaya Pyat' Lo deystvitel'no schitala eto deystvennoy taktikoy. No togda ya smotryu na ostal'nuyu chast' yego der'movoy vilki i, khorosho.",
"Tip ledyanogo bloka razdrazhalsya tem, chto on ne mog nayti lovushki na sundukakh s garantirovannym shansom na uspekh, poetomu on sdelal vernut'sya nazad vernut' veshch' i sdelal eto vozmozhnym snova. Pochemu on prosto ne dast vam amulet i ne pozvolit vam nachat' na vysokom altare vashey bogini na astral'nom plane, yesli on ne khochet rabotat' na yego pobedu?",
"Vy mozhete vzyat' vashe sladkoye vremya vsplytiya i, veroyatno, nemnogo pokhudet', potomu chto sovetskaya Pyat' Lo dayet vam po men'shey mere sto tysyach oborotov. Na samom dele, u vas mnogo vremeni, chtoby popolnit' svoi orekhi i pomassirovat' miluyu russkuyu zhenshchinu.",
"Ne igray v pogrom v sovetskom rezhime! Vy ne poluchite lishnikh trupov, chtoby nachat', no yest' lyudey po-prezhnemu kannibalizm. Yesli vam nuzhno, vyberite vmesto etogo fantom, khotya vy *budete* umirat' na Astral'nom plane, yesli Golod sluchayetsya byt' korystolyubivym. Byl tam, sdelal eto.",
"Poskol'ku u cheloveka, u kotorogo net kletok mozga, razvivayetsya etot rezhim, yady ne izvestny s samogo nachala igry. Prochitayte kommentariy v U INIT tochka gr, yesli khotite znat', pochemu eto takaya glupaya veshch', ili sprosite menya (Emi) v chate IRC.",
"Sluchaynost' ne dopuskayetsya pri kommunizme. Poetomu kazhdyy personazh, kotorogo vy nachinayete v sovetskom rezhime, nachinayetsya s odinakovogo kolichestva stat-ochkov. Duyet vas, yesli vy nachinayete podsmatrivat' za volshebnikom, potomu chto sleduyushcheye, chto my delayem, - eto takzhe ispravit' ikh inventar'.",
"Turisty, kotoryye priyezzhayut v Sovetskuyu Rossiyu, mogut prosto snyat' futbolki, khotya rubashka dolzhna byt' odnim iz punktov podpisi turista. No sovetskaya Pyat' Lo prinimayet glupyye dizaynerskiye resheniya. Nuzhno polnost'yu prizvat' yego za svoy variant der'ma.",
"Chto, vy khotite poluchit' boleye vysokiy reyting, potomu chto vy povyshayete uroven'? Zabud' eto! Tip ledyanogo bloka reshil, chto, poskol'ku imenno Emi sovershila eto izmeneniye, a Emi - yego smertel'nyy vrag, on dolzhen poluchit' toporom. I da, on ne dumayet, poka ne udalit vse.",
"Poskol'ku prostoye obrashcheniye k bonusu-khitu slishkom prosto, rezhim ledyanogo bloka izmenyayet zheleznyye tsepi i tyazhelyye zheleznyye shary, tak chto oni deystvitel'no vryad li udaryatsya. Udachi vyzhit' v nachale igry, kak katorzhnik.",
"Tip ledyanogo bloka, ogranichennogo dvoynym oruzhiyem. Vy nikogda ne stolknetes' s shirokoy storonoy ambara.",
"Khorosho, ty khochesh' uslyshat' otlichnyye novosti? Tot, kotoryy polnost'yu vzorvet vash razum? Zatem poluchite sleduyushcheye: Sovetskaya pyaterka tipa Ledyanogo bloka, otvetstvennaya za slesh-variant, povtorno vstavila oshibku brachnogo iskusstva. Teper' vash monakh snova sdelayet tol'ko odno ochko urona s veroyatnost'yu odin na chetyre. Bez shutok.",
"V Sovetskoy Rossii dlya etogo kursa podkhodit ryukzak. Pri ispol'zovanii Moshenniki vsegda poluchayet maksimal'nyy bonusnyy uron. Net, shturmovyye udary ne vyzyvayut neuravnoveshennosti.",
"Vy ser'yezno ozhidali bonusov za vysokuyu silu i lovkost', ne tak li? Nu, otstoyno byt' toboy, no ty igrayesh' glupyy variant, kogda te delyatsya napolovinu. I gore tebe, yesli tvoya sila na samom dele nizka, potomu chto togda ty ne mozhesh' nichego ubit'.",
"YA sdelal eto tak, chto vy mozhete razvernut' svoye oruzhiye bez ispol'zovaniya povorota. Ugadayte, kto byl bolvanom, kotoryy otmenil eto izmeneniye. Da, eto bylo sovetskoye.",
"Vse to, chto zastavlyayet igru stoit' igrat', bylo otmeneno v kosuyu chertu, v tom chisle i v tom sluchaye, kogda volshebnyye markery ne budut polnost'yu opustosheny, yesli vy popytayetes' chto-to napisat', imeya slishkom maloye kolichestvo chernil.",
"Kto-to reshil potroshit' palochku otozhdestvleniya s zabveniyem. I yedinstvennaya prichina, po kotoroy on eto sdelal, - dokazat' uzhe izvestnyy fakt, chto on idiot. Yego zovut 'Sovetskaya pyaterka', khotya ya predpochitayu nazyvat' yego tipom ledyanogo bloka.",
"Emi delala blagoslovlennyye predmety boleye stoykimi k otmene, i tip ledyanogo bloka reshil, chto eto glupo, bessmyslenno ili chto-to v etom rode. Chego ty ozhidal? Prinimaya logicheskiye resheniya? KHA KHA KHA KHA KHA KHA KHA. On izvesten uzhasno plokhim dizaynom igry, i on khochet zashchitit' svoyu plokhuyu reputatsiyu!",
"V sovetskom rezhime zaklinaniye 'Volshebnaya raketa' snova ul'tra moshchno. Potomu chto igrovoy balans nikogda ne byl bol'shoy chast'yu filosofii dizayna v Sovetskom Soyuze.",
"Kto-to schitayet, chto eto sovershenno zdorovo, yesli vashi luchi smerti prodolzhayut propuskat' volshebnika yada. Lovkost' bonus shans popast'? Ili luchshe udarit', yesli vash uroven' vyshe? Net, eto nikomu ne nuzhno! Slesh ikh takoy dryannoy variant, ya chestno udivlen, chto kto-to igrayet yego voobshche.",
"V svoyem stremlenii vernut' vse obratno k urovnyam vanili sovetskiy tip ledyanogo bloka sdelal eto tak, chto molniya snova oslepit vas kazhdyy raz, a prodolzhitel'nost' slepoty slishkom velika.",
};

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
	"You are struggling to keep your movements.",
	"Your limbs have turned to stone.",
	"You have turned to stone.",
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
	"You feel totally separated from the spirit world.",
	"You are feeling mildly nauseated.",
	"You feel slightly confused.",
	"You feel extremely confused.",
	"You can't seem to think straight.",
	"You can't seem to walk straight.",
	"You have become a green slime.",
	"You feel incredibly sick.",
	"You find it hard to breathe.",
	"You're gasping for air.",
	"You can no longer breathe.",
	"You're turning blue.",
	"Your skin begins to peel away.",
	"You die of starvation.",
	"You are gasping for air.",
	"You suffocate.",
	"Your limbs are getting oozy.",
	"You are turning into a green slime.",
	"You have become a green slime.",
	"You hear maniacal laughter!",
	"KAABLAMM!!! Your explosive boots suddenly detonate!",
	"You hear a dark orchestral melody, and all the lights go out...",
	"You make a lot of noise!",
	"Your persian boots demand a sacrifice for allowing you to wear them.",
	"You allow the persian boots to scratch over the full length of your shins with their zippers.",
	"Your legs are covered with deep wounds and you lose lots of blood!",
	"Your velcro boots decide to scratch up and down your shins with their lash, opening terrible wounds.",
	"Your neck is becoming constricted.",
	"Your blood is having trouble reaching your brain.",
	"The pressure on your neck increases.",
	"Your consciousness is fading.",
	"You are turning a little green.",
	"You are turning a little guernsey-spotted.",
	"You are turning a little romantic.",
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
	"You have a bad feeling in your stomach.",
	"You fall asleep.",
	"Your cloak emits a grating, annoying sound.",
	"You pass away due to being too weak.",
	"Tensa Zangetsu causes you to feel weaker!",
	"In fact, you feel almost faint...",
	"Tensa Zangetsu sapped your last bit of life and you die. Goodbye.",
	"Your stone of magic resistance curses itself!",
	"Your scratching wounds are bleeding much worse than before!",
	"You develop skin cancer!",
	"You still feel deathly sick.",
	"WARNING: You still feel deathly sick!",
	"WARNING: Your amulet will kill you in 5 turns!",
	"WARNING: If you do not ascend in 5000 turns, you die!",
	"You feel less resistant to fear.",
	"You feel less able to memorize things.",
	"You feel completely unable to memorize things.",
	"You feel more vulnerable to acid.",
	"You feel worried about getting sick.",
	"You feel more exposed.",
	"You feel stiffer.",
	"You develop lung cancer! Maybe it's time to lay off the shitting cigarettes?",
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
	"Savegame corruption done!",
	"You are no longer able to win the game.",
	"The game just became unwinnable.",
	"The game was unwinnable right from the start, and only now we have the courtesy of notifying you.",
	"The game was unwinnable since you made a certain wrong decision quite a while ago, and only now we have the courtesy of notifying you.",
	"The Bell of Opening was just deleted from your current game, and you can no longer win.",
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
	"Message to the person sitting in front of the monitor: finally put some effort into it and start playing better!",
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
	"This is Arabella speaking to you, player. Know this: I really want to stomp your toes with my profiled girl boots.",
	"This is Arabella speaking to you, player. Know this: My favorite nasty trap is the inventorylessness trap. If you trigger it, you will be unable to open your inventory.",
	"This is Arabella speaking to you, player. Know this: If you're really naughty, I'll place a teleporting items trap on your current dungeon level.",
	"This is Arabella speaking to you, player. Know this: My girlfriends and me are skilled in martial arts.",
	"This is Arabella speaking to you, player. Know this: I'd really like to dance with you, provided you are ready to do it bare-footed while I'll be wearing my boots.",
	"This is Arabella speaking to you, player. Know this: I also own a pair of grey Nike sneakers with which I like to squeeze the hands of others.",
	"This is Arabella speaking to you, player. Know this: Believe it or not, but I've also learned to walk in high heels. I don't do it often though.",
	"This is Arabella speaking to you, player. Know this: I don't like being stalked by guys.",
	"This is Arabella speaking to you, player. Know this: You will not win this game.",
	"This is Arabella speaking to you, player. Know this: You should play the Spacewars Fighter role once. Then you can try to play against me in person!",
	"This is Arabella speaking to you, player. Know this: I normally avoid the Inside Part, but here I'm immune to my own traps of course.",
	"This is Arabella speaking to you, player. Know this: Even I encountered the level bug a couple of times when I battled Marc.",
	"This is Arabella speaking to you, player. Know this: Gallardo also tried to defeat me repeatedly, but he never succeeded.",
	"This is Arabella speaking to you, player. Know this: Sizzle is not the best player in the world. He's okay, and I can actually respect him, but I'm not sure why he is in the official Top Ten list.",
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
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Sorry losers and haters, but my I.Q. is one of the highest-and you all know it!'",
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
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Ask the gays what they think and what they do, in, not only Saudi Arabia, but many of these countries, and then you tell me - who's your friend, Donald Trump or Hillary Clinton?'",
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
	"Incoming message for Player 1. The message is from 'Gruff Co-Worker'. It reads: 'You're not even supposed to exist! You're supposed to die!'",
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
	"~IIIIIIIIIIIIIIIIIIIIIIIIIIIIXI",
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
	"Darkness surrounds you.",
	"Bad idea!",
	"Dumb move!",
	"Well, that was stupid of you.",
	"You completely lose your sense of direction.",
	"You feel ignorant.",
	"You gain the power of Eru Illuvator!",
	"Bad for you - you just cursed yourself with Ina's anorexia. :-(",
	"A terrible black glow surrounds your nonfunctional wings and roots you to the ground.",
	"Your head spins, and is blasted by a terrible black aura!",
	"Your mind oscillates briefly.",
	"An evil curse goes off as you put on the veil.",
	"An evil curse goes off.",
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
	"WARNING: Game over!",
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
	"You hear a laughing voice announce: 'You're playing really badly. Why don't you train more?'",
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
	"You feel unlucky.",
	"You feel very unlucky.",
	"You feel out of luck.",
	"afk (ass-fucker)",
	"Suddenly, you know how to use the blessing technique!",
	"You contracted banishmentitis.",
	"You contracted levelteleportitis.",
	"You contracted levelteleportitis, and your teleport control was deactivated for 1 million turns.",
	"You begin to thrash about!",
	"Your dreams are shattered.",
	"Searing flames of evilness surround you.",
	"BEEEEEEEP! Your armor is cursed!",
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
	"Bugs are alerted to your position.",
	"Crack!",
	"Sizzle!",
	"Snap!",
	"Crackle!",
	"Pop!",
	"Poff!",
	"Kapow!",
	"KAABLAMM!",
	"BANG!",
	"WHANG!",
	"THWACK!",
	"Rattle/clink!",
	"Rumble!",
	"Whirl!",
	"Whoosh!",
	"Swoosh!",
	"Whaw!",
	"Boing!",
	"Boeoeoeoeoeoeoeoe!",
	"B",
	"BUG",
	"Bugnose!",
	"Burrrrrrrp!",
	"Burp!",
	"Bssssssssss!",
	"Sssssssssss!",
	"Pffffffft!",
	"Fffffffft!",
	"NOSE!",
	"Boom!",
	"Kaboom! Boom! Boom!",
	"Flummmmmmmm!",
	"Blobloblobloblo!",
	"Ratatatatatata!",
	"TRRRRRRRRRRR!",
	"BOOM BOOM BOOM BOOM BOOM BOOM BOOM!",
	"Paeaeaeaeaeaeaeau klink klink!",
	"Pschaeaeaeaeaeaeau!",
	"Poeoeoeoeoeoeoeoeu!",
	"Pschoeoeoeoeoeoeoeoeu!",
	"Pschaeaeaeaeaeaeaeau!",
	"Pouch-tschcktschck!",
	"Dae-oe-oe-oe-oe-oe!",
	"Faeaeaeaeaeaeaeau!",
	"Hahahahahahahaaaaaaaa!",
	"Ai hi hi hi!",
	"KABOOOOOOOOOOOOOOOOOM!",
	"Doing!",
	"Error!",
	"Fatal Error!",
	"Fake Error!",
	"Splash!",
	"KABOOM!",
	"KADOOM!",
	"CAEAUWAESH!",
	"Dweeeeeeau!",
	"Wi-oe-oe-oe-oe!",
	"Ha, hahahahahaiiiiiii!",
	"Tschieauwieauw!",
	"Tschieauwick!",
	"Tschaeauwieauw, well and!",
	"GLUI-LUILUILUINK!",
	"KADOOM! The boulder falls into the pit!",
	"An alarm sounds!",
	"You penetrated a high security area!",
	"A voice booms: \"The Angel of the Pit hast fallen!  We have returned!\"",
	"It constricts your throat!",
	"You feel down...",
	"You sit down and fall asleep.",
	"Mortal creatures cannot master such a powerful amulet, and are therefore afflicted by a dark, evil curse!",
	"You suddenly inhale an unhealthy amount of water!",
	"You feel intellectually poor.",
	"You feel materially poor.",
	"You feel filthy.",
	"You feel dirty.",
	"A terrible aura of darkness and eternal damnation surrounds you.",
	"You feel unsafe.",
	"You feel really insecure.",
	"You feel a need to visit your shrink.",
	"You feel a need to tell AmyBSOD that she is a filthy harlot.",
	"You feel a need to tell AmyBSOD that you don't have a chance in this game.",
	"You feel a need to laugh.",
	"You suddenly roll on the floor laughing.",
	"You laugh out loud. LOL!",
	"You start giggling madly.",
	"You can't see anymore.",
	"You are feeling a dangerous process in your body.",
	"Suddenly you cannot see yourself.",
	"The tentacles tear your cloak to shreads!",
	"split object,",
	"testing for betrayal",
	"THUD!",
	"Thump!",
	"You receive a static shock from your cloak!",
	"You receive an electric shock from your cloak!",
	"You are beginning to feel thirsty.",
	"You are dehydrated, your vision begins to blur...",
	"Your inventory items unidentified themselves.",
	"You have maprot.",
	"You lost spells.",
	"Your vampiric gloves drain your experience!",
	"Fatal attraction!",
	"You feel the Black Breath slowly draining you of life...",
	"You are clawed by your gauntlets and faint.",
	"You regain consciousness.",
	"Chaeaet!",
	"Your terrible eating disorder causes you to faint.",
	"Lovely fleecy caressing feelings stream into your legs!",
	"You're dressed like a '69 whore!",
	"You're dressed like a frickin' hardrocker!",
	"The velcro boots constrict your feet, and you can't take them off again!",
	"You turn into a Binder!",
	"You turn into a Failed Existence!",
	"A demonic aura envelopes your boots.",
	"You decided to pretend to be Ella? Now prove it!",
	"You immediately get all wet and horny.",
	"You hear a frightening crash in the distance...",
	"A monster was spawned!",
	"A monster rises from the grave!",
	"You feel a ghastly chill run down your spine!",
	"Your foe calls upon your soul!",
	"Boss monsters appear from nowhere!",
	"You feel like a statue!",
	"You suddenly have company.",
	"Oh! You feel that the curse is replicating itself!",
	"AmyBSOD puts an Ancient Morgothian Curse on you!",
	"You are afflicted with the Curse of Topi Ylinen!",
	"Your evil female battle boot drills into your mind with its spikes!",
	"You no longer know how to cast finger of death.",
	"You forget the 'magic mapping' spell.",
	"Alright, riddle me this: Is the following rumor true or not?",
	"You really believe everything you read, huh? Well, sucks to be you.",
	"Captcha! Please type in the following word(s) to continue: Penis",
	"Captcha! Please type in the following word(s) to continue: SLASH'EM EXTENDED",
	"Captcha! Please type in the following word(s) to continue: FIQ's wand balance patch sucks",
	"Captcha! Please type in the following word(s) to continue: Grxnar Grxnar",
	"Captcha! Please type in the following word(s) to continue: 500$",
	"Captcha! Please type in the following word(s) to continue: I love AmyBSOD",
	"Captcha! Please type in the following word(s) to continue: odd egg pocket fuck",
	"Captcha! Please type in the following word(s) to continue: screwstupidcaptchas",
	"Captcha! Please type in the following word(s) to continue: Eating Disorder",
	"Captcha! Please type in the following word(s) to continue: Omg is this retarded!",
	"Captcha! Please type in the following word(s) to continue: Ascension Is Impossible",
	"Captcha! Please type in the following word(s) to continue: Donald Trump",
	"Captcha! Please type in the following word(s) to continue: Saddam Hussein",
	"Captcha! Please type in the following word(s) to continue: Atler Hidolf",
	"Captcha! Please type in the following word(s) to continue: 3lökjepü2'IP2wjl215ERplqvm§",
	"Captcha! Please type in the following word(s) to continue: 11111llil1ll1ili11li1lilil11IILII!1L1IL1L1I",
	"Captcha! Please type in the following word(s) to continue: 6666696969699996669696999996996969669669699666969",
	"You will be punished. I will paralyze you, slow you and reduce your max HP and Pw.",
	"Gravity warps around you...",
	"NetHack Quiz! You will now tell me whether the following rumor is true or not!",
	"Now tell me if this rumor was true!",
	"Tough luck, player, it seems you're just not good enough and now there will be a bad effect to punish you, ha-ha!",
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
	"I'm a bitch, deal with it!",
	"Manuela put a terrible curse on your beautiful pair of red block-heeled combat boots.",
	"Your boots feel deathly cold. But seriously, you did expect them to autocurse, right?",
	"You angered your beautiful lilac pumps!",
	"Frustrated, the lovely leather pumps settle down.",
	"You get the feeling that something soft just slid along the full length of your legs!",
	"Oh no, this is a cloak of death!",
	"Your cloak is surrounded by a black aura.",
	"A gush of water squirts all over your body!",
	"You're paralyzed!",
	"Your blood squirts everywhere while the tender, but very sharp-edged heel is destroying every shred of your skin it can reach.",
	"In a terrible blood rush, the pink stiletto heels scratch up and down your legs again and again.",
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
	"You feel a little bad.",
	"You feel sick.",
	"You feel very sick.",
	"You hear very beautiful noises.",
	"You feel disenchanted.",
	"One of your items got disenchanted!",
	"Your equipment seems less effective.",
	"You are losing blood!",
	"You are losing lots of blood!",
	"A malignant aura surrounds you.",
	"You feel like you need some help.",
	"You feel something.",
	"You feel nothing.",
	"You feel that something has happened.",
	"You feel empty.",
	"You feel void.",
	"You feel useless.",
	"You sink below the surface and die.",
	"You feel unable to ever ascend this game.",
	"You will not ascend, because unlike casual NetHack variants, this one is really hard!",
	"You feel sweaty.",
	"You feel bad.",
	"You feel much worse.",
	"You feel even worse.",
	"You don't feel so good...",
	"You feel sick.",
	"You suddenly have to take a shit!",
	"Taking off your armor is going to take a while...",
	"You need to remove your cloak...",
	"You suddenly drop all of your armor!",
	"You did not watch out, and stepped into your own shit.",
	"You feel apathetic.",
	"You feel bored.",
	"You feel the wrath of an unknown entity.",
	"You feel sexual desire.",
	"You feel that something from your inventory just got stolen.",
	"It steals a blessed bag of holding!",
	"Your vision bugged.",
	"You got banished!",
	"Your money was stored, thanks.",
	"You acquire intrinsic map amnesia.",
	"You feel very disoriented but decide to move on.",
	"You are suffering from sleep deprivation.",
	"You are suffering from withdrawal symptoms.",
	"A mysterious force surrounds you...",
	"You feel that it's time to finally quit smoking.",
	"You take a dose of sedativa.",
	"You feel polyform.",
	"You sink deeper into the lava.",
	"You open a warp gate!",
	"You can hear Arabella announce: 'Sorry, but the time of your demise is drawing near.'",
	"You can hear Arabella giggling.",
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
	"You hear a telemarketer shout: 'MMMMM! Smells like Yasdorian, crispy-brain good brain yum yum!'",
	"You sit down.",
	"You get up.",
	"You stand still.",
	"You move.",
	"You stop moving.",
	"You see something.",
	"You groan.",
	"You are annoyed.",
	"You are angry.",
	"You are cheerful.",
	"You laugh.",
	"You cry.",
	"You fly.",
	"You look around.",
	"You stop in your tracks.",
	"You stop your occupation.",
	"You experience a disconnect.",
	"Your connection to the server was lost.",
	"You blink.",
	"You experience a lag spike.",
	"Did you know that AmyBSOD ascended a healer in this game?",
	"Did you know that the first ascended slex character was an illithid scientist?",
	"Did you know that AmyBSOD isn't conceited at all?",
	"Did you know that AmyBSOD ran a half marathon completely untrained in 2h 38min?",
	"You listen to a loudspeaker message.",
	"You switch on your radio.",
	"You feel corrupted.",
	"You feel extremely corrupted.",
	"You jump.",
	"You jump out of the picture and die.",
	"You jump up.",
	"You get down.",
	"You moved beyond the edge of the screen, and plummet to your death!",
	"You stretch.",
	"You do an aerobics exercise.",
	"You lift some weights.",
	"Scut!",
	"Shut the fuck up!",
	"The little girl gets wet.",
	"The orc invokes decapitation! You die.",
	"The Dracolich breathes nether.--More-- You die.",
	"Semzov the cleaner shoots you with his gun. You die.",
	"Semzov the cleaner throws a potion of salt solution.",
	"You are ambushed!",
	"The monsters have been waiting for you, and now you're ambushed. Bwarharharhar!",
	"Suddenly the monsters are coming for you with everything they got!",
	"A troll boss and 50 of his minions have been spawned.",
	"A troll boss and 50 of his minions have been spawned. Good luck preventing them from resurrecting.",
	"The staircase is a mimic.",
	"The mimic is a staircase.",
	"Wait! You ran into a devouring mimic! It totally digests you! You die...",
	"The lich expands a circle of magic. Warning: HP low!",
	"The cupid of love shoots you with his bow. Player 1 is about to die.",
	"Your relationship with Vanzac becomes 'Foe'! Vanzac wields a shotgun!",
	"You can't play shit.",
	"Loyter the crimson of Zanan throws a rock. You die.",
	"Everyone loves Shena's ass.",
	"Donald Trump will be the POTUS of the United States.",
	"The prostitute begins to take her clothes off.",
	"You turn into a writhing mass of primal chaos.",
	"You writhe in pain.",
	"You are severely hurt!",
	"You are on the verge of dying!",
	"You feel out of touch with the world.",
	"Your skill levels are reduced.",
	"Your maximum skill levels have been reduced.",
	"Your skills are deactivated!",
	"For the next 10000 turns, all of your spells will cost twice the usual amount of mana.",
	"Your current prayer timeout is 6257 turns.",
	"You have transgressed.",
	"Your current alignment record is -507.",
	"You trigger a technique trap! Your techniques stop working.",
	"Your disease gets worse.",
	"You gain the Poison Hands corruption: every potion you pick up automatically turns into poison.",
	"You gain the Mana Battery corruption: every wand you pick up automatically gets drained to 0 charges.",
	"You are dimmed.",
	"You are out of your mind.",
	"You suddenly break your neck due to overexertion.",
	"You gain the Xus Moda corruption: if you don't ascend this game, you cannot play Slash'EM Extended anymore until you beat ADOM.",
	"You gain the Xor Me Hsals corruption: your chances to ascend this game have been increased!",
	"You feel very lucky today!",
	"The Mages Guild Master casts a spell. You are hit by a nerve bolt!",
	"You shiver.",
	"You hear cursed whisperings.",
	"You dream of girls slamming their female hugging boots into your shins.",
	"You dream of wonderful, soothing pain.",
	"You dream of a sexy girl wearing a bikini and high-heeled combat boots.",
	"You dream of something totally hot, and when you check the area between your legs, you discover that you're all wet!",
	"You dream of a sexy farting butt.",
	"You dream of world peace.",
	"You are glad that you're not playing dnethack, which would require you to have the wiki open at all times to have any chance.",
	"You are glad that you're not playing nethack4, because that is a cursed satanic variant.",
	"You are glad that you're not playing Nethack Fourk, since you agree with AmyBSOD that the wand 'balance' patch sucks.",
	"You are glad that you're not playing FIQhack, because unlike that, Slash'EM Extended is well balanced.",
	"You are glad that you're not playing SLASHTHEM, since the type of ice block hates you (harharharharharharhar harharhar harhar).",
	"You are glad that you're not playing SLASHTHEM, for that's the hardly extended but much removed mod.",
	"You are glad that you're not playing SLASHTHEM, which has not been updated in a long while and purposefully re-inserted bugs that AmyBSOD had fixed.",
	"You are glad that AmyBSOD brought this beautiful game to you. :-)",
	"You are glad that AmyBSOD continues to work on Slash'EM Extended, and are eagerly awaiting the next release version.",
	"You are glad that AmyBSOD did not remove the transvestite and topmodel roles despite some prude people complaining.",
	"The khmer-speaking transvestite zaps a wand of disintegration!--More--",
	"The goblin zaps a wand of draining!--More-- Do you want your possessions identified?",
	"Hooooooooooooo, hae hae hae hae hae...",
	"Your wooden equipment spontaneously catches fire and burns to ashes.",
	"Your metallic equipment corrodes! One of your items got vaporized!",
	"You drop your weapon.",
	"You drop your weapon and it hits your left foot.  Ouch! That hurts!",
	"You fall down a shaft.",
	"You fall into a bottomless pit and die.",
	"You fall into a bottomless pit and jump out of the picture.",
	"Warning: 10000 turns left until the Astral Plane becomes permanently inaccessible!",
	"Warning: 5000 turns left until Vlad's Tower is sealed off forever!",
	"Warning: You must kill 10 nymphs over the course of the next 2000 turns or you automatically die.",
	"Warning: An anti-wishing countermeasure has been activated. You will instantly die if you try to wish for any item.",
	"Warning: If you break any conduct that you have not broken yet, the RNG will kill you outright.",
	"Warning: Your anti-virus software is out of date. Click here for a free update.",
	"Warning: You exceeded the permissible maximum speed by 30 mph. To pay your fine, visit this website please:--More--",
	"Warning: You have been sued due to copyright infringement. Open the attachment of our e-mail for further instructions.",
	"Warning: Your PC is too slow! Buy a new one!",
	"Warning: The chair you're sitting on has been rigged and will selfdestruct in 10 seconds.",
	"Message to the person sitting in front of the monitor: your chair will selfdestruct in one minute.",
	"Message to the person sitting in front of the monitor: please put some effort into it and play better!",
	"Message to the person sitting in front of the monitor: Try hard!",
	"You will get killed unless miracles happen.",
	"You will get killed, a hundred percent sure.",
	"If the Wizard of Yendor is a giant, you are less than the droppings of an ant.",
	"Vlad the Impaler can mince you with his eyes closed.",
	"Your quest nemesis is at least twice stronger than you.",
	"You hear the sounds of something you never, ever, want to anger.",
	"You hear a dreadful sound.",
	"The weight of your inventory crushes you.",
	"You drop something to wield your sword with both hands",
	"You can't use both a two-handed sword and a shield!",
	"The champion wields a three-handed sword!",
	"You fall down a well.",
	"Warning: Rodney will automatically wake up in 1000 turns. This is an anti-farming measure.",
	"Warning: Monsters will forever cease to have death drops in 1000 turns. This is an anti-farming measure.",
	"Warning: 1000 turns from now, you will no longer be able to use upstairs until you have the Amulet of Yendor. This is an anti-farming measure.",
	"Warning: You can no longer gain experience points or level up. This is an anti-farming measure.",
	"Warning: Food can no longer spawn now, and your prayer timeout has been increased greatly. Ascend already you lame farmer, and stop camping the same dungeon level for 20000 turns!",
	"Did you know that constant farming decreases your penis size?",
	"If you exploit game mechanics by doing lame things like pudding farming or long worm farming, you will become infertile in real life.",
	"Remember that Slash'EM Extended is not 'Grinding: The Game'. You're expected to work towards your goal, which is called 'ascension', and if you delay it for too long, the RNG will attempt to kill you off in unfair ways!",
	"You die of neurofibroma.",
	"g",
	"nt||",
	"ead with a mighty crash!",
	"You melt!",
	"You start to melt.",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'This was locker room banter, a private conversation that took place many years ago. Bill Clinton has said far worse to me on the golf course'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Bill Clinton has actually abused women and Hillary has bullied, attacked, shamed and intimidated his victims.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Robert Pattinson should not take back Kristen Stewart. She cheated on him like a dog & will do it again - just watch. He can do much better!'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Ariana Huffington is unattractive, both inside and out. I fully understand why her former husband left her for a man - he made a good decision.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Our great African-American President hasn't exactly had a positive impact on the thugs who are so happily and openly destroying Baltimore.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'If I were running 'The View', I'd fire Rosie O'Donnell. I mean, I'd look at her right in that fat, ugly face of hers, I'd say 'Rosie, you're fired.''",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'One of they key problems today is that politics is such a disgrace. Good people don't go into government.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'The other candidates - they went in, they didn't know the air conditioning didn't work. They sweated like dogs... How are they gonna beat ISIS? I don't think it's gonna happen.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Lyin' Ted Cruz just used a picture of Melania from a shoot in his ad. Be careful, Lyin' Ted, or I will spill the beans on your wife!'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I would bring back waterboarding and I'd bring back a hell of a lot worse than waterboarding.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'His wife, if you look at his wife, she was standing there. She had nothing to say. She probably - maybe she wasn't allowed to have anything to say.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I'm afraid the election is going to be rigged, I have to be honest.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'He's not a war hero... He was a war hero because he was captured. I like people who weren't captured.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'President Barack Obama was born in the United States. Period. He is the founder of ISIS. And, I would say the co-founder would be crooked Hillary Clinton.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Dwyane Wade's cousin was just shot and killed walking her baby in Chicago. Just what I have been saying. African-Americans will VOTE TRUMP!'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'It's about time that this country had somebody running it that has an idea about money.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Well, I have much better judgment than she does. There's no question about that. I also have a much better temperament than she has, you know?'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'And when you're a star, they let you do it. You can do anything. Grab them by the pussy. You can do anything.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I take advantage of the laws of the nation. Because I'm running a company.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I did try and fuck her... I moved on her like a bitch, but I couldn't get there. And she was married.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Even if the world is going to hell in a hand-basket, I won't lose a penny.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'We are going to have a massive, massive tax increase under Hillary Clinton.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Our jobs are being sucked out of our economy.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'But we have some bad hombres here and we're going to get them out.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'If that's the last jobs report for the election, I should win the election easily.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'The one thing you have over me is experience. But it's bad experience. The problem is you talk, but you don't get anything done.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'She should not be allowed to run. In that case, I say this is rigged.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'She will defeat ISIS? We should have never let ISIS happened in the first place. Right now, they're in 32 countries.'",
	"Incoming message for Player 1. The message is from 'a scientist'. It reads: 'I sure hope you know where the bathroom is, I forgot to wear my Depends today.'",
	"Incoming message for Player 1. The message is from 'Eveline'. It reads: 'I am very gentle.'",
	"Incoming message for Player 1. The message is from 'Melissa'. It reads: 'Don't I look chic? If there were any unmarried men around here, I'm sure I'd be irresistible for them!'",
	"Incoming message for Player 1. The message is from 'Josefine'. It reads: 'My velcro sneakers just want to play with you.'",
	"Incoming message for Player 1. The message is from 'May-Britt'. It reads: 'Where are those bum girls?'",
	"Incoming message for Player 1. The message is from 'trembling poplar'. It reads: 'Help me, I have parkinson!'",
	"Incoming message for Player 1. The message is from 'Inge'. It reads: 'Stop calling me by my first name!'",
	"Incoming message for Player 1. The message is from 'Ina'. It reads: 'Don't offer me sachertorte, I can't eat that or I'd become even thicker!'",
	"Incoming message for Player 1. The message is from 'Katrin'. It reads: 'Do you like it if my purple high heels scratch up and down your legs?'",
	"Incoming message for Player 1. The message is from 'Katrin'. It reads: 'Would you like it if I used my sticky chewing gum to rip the skin off your bones?'",
	"Incoming message for Player 1. The message is from 'Katrin'. It reads: 'I will have to torture you now. As soon as you volunteer to clean the shoes of all my girlfriends, I'll let you go immediately.'",
	"Incoming message for Player 1. The message is from 'Lisa'. It reads: 'I bought many pairs of high heels during my vacation in Prague.'",
	"Incoming message for Player 1. The message is from 'Pete'. It reads: 'Hahaha! No one can stop me.'",
	"Incoming message for Player 1. The message is from 'G-cheater'. It reads: 'Penis masturbation!'",
	"Incoming message for Player 1. The message is from 'G-cheater'. It reads: 'I! Want! To! Smoke! A! Cigarette! Now!'",
	"Incoming message for Player 1. The message is from 'Ella'. It reads: 'You'll get some bodily injury in a second! Throw at once now!'",
	"Incoming message for Player 1. The message is from 'Urbaldi'. It reads: 'I'm in need of your assistance! Please helf me!'",
	"Incoming message for Player 1. The message is from 'Jessica'. It reads: 'I only use my stiletto heels as lethal weapons in really dangerous situations.'",
	"Incoming message for Player 1. The message is from 'H.'. It reads: 'You're seriously going down. My friends and me will kill you, and then we'll fuck your women and daughters, and burn your sons.'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'Ey yo asshole you are now D-E-A-D yo hear me? I'll fuck you up you fucking fucktard ey yo ey!'",
	"Incoming message for Player 1. The message is from 'Mailie'. It reads: 'Instructions? Just read them instead of asking me, and now please let me continue working undisturbed.'",
	"Incoming message for Player 1. The message is from 'David'. It reads: 'You may not come in, because the cockroach's funeral is taking place here.'",
	"Incoming message for Player 1. The message is from 'Sophie'. It reads: 'I can cause a lot of damage with my black high-heeled combat boots!'",
	"Incoming message for Player 1. The message is from 'Rita'. It reads: 'So you think you can just give up and I'll spare you. Hahaha. I'm glad that I caught you depraved person, and now I will torture you. HAHAHA.'",
	"Incoming message for Player 1. The message is from 'Sven'. It reads: 'You live? Should have expected that. Traitors like you aren't easy to kill.'",
	"Incoming message for Player 1. The message is from 'Sven'. It reads: 'You traitor will die!'",
	"Incoming message for Player 1. The message is from 'Diablo player character'. It reads: 'Your madness ends here, betrayer!'",
	"Incoming message for Player 1. The message is from 'Meltem'. It reads: 'My sweetie, now I'll tell you what I like to do. With my boot heels I can not only scratch up and down your legs, but I also like to kick others in the nuts.'",
	"Incoming message for Player 1. The message is from 'Magdalena'. It reads: 'You will see right now how much I can hurt you with my 'stinking boots'.'",
	"Incoming message for Player 1. The message is from 'Magdalena'. It reads: 'Do yourself a favor and commit suicide, or my combat boot heels will make the blood flow like rivers from your veins.'",
	"Incoming message for Player 1. The message is from 'Sue Lyn'. It reads: 'What, I'm supposed to become a topmodel? But I have no high heels!'",
	"Incoming message for Player 1. The message is from 'Sue Lyn'. It reads: 'My name isn't Mary Sue, and I'm not perfect, I'm a satanist!'",
	"Incoming message for Player 1. The message is from 'Marihuana'. It reads: 'For your infringement, I will punish you. Until tomorrow you have to copy the house rules by hand three times.'",
	"Incoming message for Player 1. The message is from 'Eva'. It reads: 'I request you to duel me, because I like kicking your hands with my soft orange mocassins. And I especially love seeing your blood squirting in all directions.'",
	"Incoming message for Player 1. The message is from 'Kati'. It reads: 'You disappear from here at once or I'll use the Imperio curse on you!'",
	"Incoming message for Player 1. The message is from 'Friederike'. It reads: 'Hahaha, I peed into your shoes because I think that's fun! Hahaha!'",
	"Incoming message for Player 1. The message is from 'Thai'. It reads: 'I'm looking forward to using your toilet!'",
	"Incoming message for Player 1. The message is from 'Thai'. It reads: 'My black high-heeled combat boots are a black belt in karate.'",
	"Incoming message for Player 1. The message is from 'Thai'. It reads: 'I forgot to wear my high heels today.'",
	"Incoming message for Player 1. The message is from 'Yvonne'. It reads: 'Attack left! Feint right! Block and counter! Serrate serrate! Sword eddy left! Leap attack right! And TOUCHE, you motherfucker!'",
	"Incoming message for Player 1. The message is from 'Yvonne'. It reads: 'Motherfucker! KIIIIIIIII!'",
	"Incoming message for Player 1. The message is from 'Julia'. It reads: 'FAAS... RU MAAR!!!'",
	"Incoming message for Player 1. The message is from 'Corina'. It reads: 'ZUM... HAAL VIIK!!!'",
	"Incoming message for Player 1. The message is from 'Sandra'. It reads: 'FUS... RO DA!!!'",
	"Incoming message for Player 1. The message is from 'Ronja'. It reads: 'I liked being whipped. It was an appropriate punishment for what I had done.'",
	"Incoming message for Player 1. The message is from 'Leo'. It reads: 'Let me take a photo of you. Cheese! Or shall I rather say, Pinselbueschel?'",
	"Incoming message for Player 1. The message is from 'Birgit'. It reads: 'You will get what's coming to you, colleague.'",
	"Incoming message for Player 1. The message is from 'type of ice block'. It reads: 'Ha, hahahahahaiiiiiiiiii!'",
	"Incoming message for Player 1. The message is from 'type of ice block'. It reads: 'Harharhar! Harharhar!'",
	"Incoming message for Player 1. The message is from 'type of ice block'. It reads: 'Harharharharhar harharharharharharhar, hahaha ha-hahahahaha hahahahahaHARHARHARHARHARRRRRRRR-HARRRRRRRR!'",
	"Incoming message for Player 1. The message is from 'type of ice block'. It reads: 'Harharhar I got a lightning start!'",
	"Incoming message for Player 1. The message is from 'Victoria'. It reads: 'What, you get all hot and wet when I test my martial arts skills on you? If I had known that before...'",
	"Incoming message for Player 1. The message is from 'Victoria'. It reads: 'I will sit on you and place my block heels on your exposed hands.'",
	"Incoming message for Player 1. The message is from 'Natalia'. It reads: 'I need to use my special toothpaste every day or I'll die of fatal poisoning.'",
	"Incoming message for Player 1. The message is from 'Juen'. It reads: 'One night with me will only cost you 20 euros! Don't worry, I'm working for women too!'",
	"Incoming message for Player 1. The message is from 'Juen'. It reads: 'I truly adore my black high-heeled combat boots. They're my pride and joy.'",
	"Incoming message for Player 1. The message is from 'Madeleine'. It reads: 'Hahaha, I can't wait to tell Anita that I flew around on a Fearow!'",
	"Incoming message for Player 1. The message is from 'Madeleine'. It reads: 'Double Kick! Hahaha, it's super-effective versus Type Normal!'",
	"Incoming message for Player 1. The message is from 'Madeleine'. It reads: 'The creature looks like a Machamp, right? Flying attacks are super-effective versus those, and since the rifle bullets are flying I just kept firing my rifle at it.'",
	"Incoming message for Player 1. The message is from 'Manuela'. It reads: 'I'll flatten you, all of your body parts will successively be stomped flat by my combat boot heels.'",
	"Incoming message for Player 1. The message is from 'Manuela'. It reads: 'I like to slowly torture cats to death by kicking them with my high-heeled lady boots.'",
	"Incoming message for Player 1. The message is from 'Natalje'. It reads: 'Close the window! Now!'",
	"Incoming message for Player 1. The message is from 'Natalje'. It reads: 'Bueyuek Allah hilekar pitis, seni cezalandiracak.'",
	"Incoming message for Player 1. The message is from 'Viktor Krum'. It reads: 'Poshel na khuy blyad' pizdu. Ya khochu, chtoby ty umer v ogne.'",
	"Incoming message for Player 1. The message is from 'Ruea'. It reads: 'Kafir, Allah'a doenuestuermek ya da oel!'",
	"Incoming message for Player 1. The message is from 'Ruea'. It reads: 'Kafir, kutsal Tanri bu boelgede emanetler en kadirdir. Ve bunu degistirmek olmaz!'",
	"Incoming message for Player 1. The message is from 'Ruea'. It reads: 'Kafir, sizin yollariniz sapkin vardir! Islam sizi doenuestuermek zamanidir.'",
	"Incoming message for Player 1. The message is from 'Ruea'. It reads: 'Kafir, sen domuz pirzola yeme suc islemis! Allah sana cok kizgin!'",
	"Incoming message for Player 1. The message is from 'Ruea'. It reads: 'Allahu Akbar! Allahu Akbar!'",
	"Incoming message for Player 1. The message is from 'Ruea'. It reads: 'Kafir, Allah bueyuektuer ve seni yok eder!'",
	"Incoming message for Player 1. The message is from 'Ruea'. It reads: 'Kafir! Kac kere zaten gavur dedin? Sen inanc degistirmek icin ya da Tanri ilahi ceza kesin olacak var!'",
	"Incoming message for Player 1. The message is from 'Ruea'. It reads: 'Allah senin gibi kafirler sevmez ve cok zalim bir sekilde sizi cezalandiracaktir!'",
	"Incoming message for Player 1. The message is from 'Ruea'. It reads: 'Bizim kutsal cami kafirler tarafindan yerle bir olma tehlikesiyle karsi karsiya! Bueyuek Tanri bize yardim ve ilahi asker goendermesi gerekir!'",
	"Incoming message for Player 1. The message is from 'Ruea'. It reads: 'Kafir, Allah bueyuek ve gueclue oldugunu! Ona inanmaya baslarlar yoksa, aci olacak!'",
	"Incoming message for Player 1. The message is from 'Ruea'. It reads: 'Allah onlari oelduererek ve kutsal ateste kendi cesetlerini yakarak buetuen kafirleri cezalandiracaktir.'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'You still have time to flee with your life, wimp.'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'And another one who stood against me falls. And so it will be for you, player. No warning, no mercy.'",
	"Incoming message for Player 1. The message is from 'Doctor Maex'. It reads: 'I can't wait to tell my graduate college of your defeat this day!'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'How much longer shall I let you live?'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'How much longer do you intend to dance around like a clown?'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'Your tactics remind me of a child learning to play chess!'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'Fool! You have crossed the line. Your life is forfeit.'",
	"Incoming message for Player 1. The message is from 'Nadja'. It reads: 'You apologize with me now or I'll hurt you even more!'",
	"This is Arabella speaking to you, player. Know this: Your right mouse button will probably fail at some point. This makes items, traps and dungeon features invisible to you and also prevents you from farlooking things.",
	"This is Arabella speaking to you, player. Know this: One of the traps I installed in my base is the uninformation trap. Trigger it, and you will no longer be able to tell what your items actually are.",
	"This is Arabella speaking to you, player. Know this: I love the yellow spell trap. Not only does it make you consume double the usual amount of mana for spellcasting, it also causes interface screws!",
	"This is Arabella speaking to you, player. Know this: Triggering the speed bug trap is actually like a double-edged sword. It will randomize your speed each turn, meaning that sometimes you'll be slower than usual while at other times you're faster.",
	"This is Arabella speaking to you, player. Know this: I don't want you to stalk me.",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I do whine because I want to win, and I'm not happy about not winning, and I am a whiner, and I keep whining and whining until I win.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'And while I can't honestly say I need an 80-foot living room, I do get a kick out of having one.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: '... when you're rich, you can have as many kids as you want. Being rich makes it easier to have kids.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'What does it all mean when some wacko over in Syria can end the world with nuclear weapons?'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I just realized that if you listen to Carly Fiorina for more than ten minutes straight, you develop a massive headache. She has zero chance!'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I will be so good to women.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'Nothing wrong with ego.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'What a stiff, what a stiff, Lindsey Graham.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I did very well with Chinese people. Very well. Believe me.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: '... I'm married to my business. It's been a marriage of love. So, for a woman, frankly, it's not easy in terms of relationships. But there are a lot of assets.'",
	"Incoming message for Player 1. The message is from 'Donald Trump'. It reads: 'I don't want to be President. I'm 100 percent sure. I'd change my mind only if I saw this country continue to go down the tubes.'",
	"Incoming message for Player 1. The message is from 'Irina'. It reads: 'I'm not a model, but I can teach you to become one.'",
	"Incoming message for Player 1. The message is from 'Robat'. It reads: 'Dirty traitor, prepare to die.'",
	"Incoming message for Player 1. The message is from 'Robat'. It reads: 'Shut the fuck up you dirty traitor, and now fucking die!'",
	"Incoming message for Player 1. The message is from 'Flo-ooo'. It reads: 'We may not concentrate on internal conflicts now!'",
	"Incoming message for Player 1. The message is from 'Cristi'. It reads: 'You know what? I'm changing my allegiance. My friends are right, you are a damn heretic and you don't deserve anyone helping you.'",
	"Incoming message for Player 1. The message is from 'Sabrina'. It reads: 'Somehow I don't know how to cast the magic vacuum spell!'",
	"Incoming message for Player 1. The message is from 'Pete'. It reads: 'I will win!'",
	"Incoming message for Player 1. The message is from 'Nino'. It reads: 'Hahaha, now the great raiding party will commence!'",
	"Incoming message for Player 1. The message is from 'Sue Lyn'. It reads: 'My boots like to kick others!'",
	"Incoming message for Player 1. The message is from 'Sue Lyn'. It reads: 'I can produce beautiful farting noises with my sexy butt!'",
	"Incoming message for Player 1. The message is from 'an anonymous voter'. It reads: 'TRUMP TRUMP TRUMP TRUMP!'",
	"Incoming message for Player 1. The message is from 'Ruth'. It reads: 'Watch your tone. Just so you know, I'm holding a heavy glass bottle in my left hand.'",
	"Incoming message for Player 1. The message is from 'Ludgera'. It reads: 'If you want to encounter the mermaids, you have to swim out into the ocean and dive!'",
	"Incoming message for Player 1. The message is from 'Ruea'. It reads: 'Kafir, Allah beni tuvalete bok sesleri ueretmek icin izin! Eger o saygi yoksa sonsuza aci olacaktir!'",
	"Incoming message for Player 1. The message is from '<illegible>'. It reads: ''",
	"The win16 subsystem has insufficient resources to continue running. Click on OK, close your application and restart your machine.",
	"System resources depleted. Please restart your computer.",
	"The stethoscope pierces your heart!",
	"The ancient multi-hued dragon warps to you!",
	"Klack! The lid slides over your hand, and blood is shooting out.",
	"You cannot have this property extrinsically: magic resistance.",
	"You cannot have this property intrinsically: poison resistance.",
	"You cannot have this property at all: reflection.",
	"The following skill cannot be trained at all: long sword.",
	"The following skill is limited to basic proficiency: attack spells.",
	"You terribly hurt yourself with your blade!",
	"You come right as the lovely, soft boot heel hits you.",
	"You feel wonderful intense pain as the incredibly soft lady shoe hits you.",
	"Ulch! You're immersed with dog shit!",
	"Collusion!",
	"You inhale a horrific odor!",
	"You inhale a soothing aroma!",
	"You inhale the beguiling perfume of a persian woman!",
	"You inhale the vile stench of a sexy pair of orange women's mocassins!",
	"The hell temptress opens your chastity belt and starts to remove it...",
	"Your life is worthless and you want to jump off a bridge!",
	"You are dimmed!",
	"You and your steed are severely hurt!",
	"Your weapon is destroyed.",
	"You feel that your wife is gonna break up with you!",
	"You feel that there should really be lesbianism for everyone.",
	"You feel some dimness inside!",
	"You feel worried about your marriage!",
	"Life has no more meaning. Your wife has run away, your children are dead and people are setting fire to your home right now.",
	"Your equipment has gunk on it now.",
	"Your health increases, at the cost of nasty side effects.",
	"Your mind clears unexpectedly!",
	"The farting gas destroys your footwear instantly.",
	"Congratulations, you can now walk around in a pair of boots that won't come off, and whose previous owner fully stepped into a heap of dog shit.",
	"Your cloak is heavily cursed, and you hear Conny's voice wishing you good luck.",
	"Uh-oh... your spells became yellow.",
	"You feel that there is nothing but agony waiting for you, and as a proof, your shield just became cursed.",
	"Your teleport control has been deactivated for a very long time.",
	"You hear a high-pitched sound followed by a short, slightly lower-pitched one...",
	"Oh no... your heart... it's... getting... unsteady...",
	"BEEPBEEP BEEPBEEP BEEP BEEP BEEEEEEEEEEEEEEEEEEEEP!",
	"You die from a heart failure.",
	"The liquid destroys your footwear instantly.",
	"Escalation!",
	"You suddenly forget what you were doing. Maybe your thoughts ended up in the realms of Oblivion, who knows?",
	"All your personal data, documents, photos etc. are encrypted. Please pay 600 euros (approximately 1 bitcoin) to receive the decryption key.",
	"Hahaha hahahahaha, hahahahaha hahahahahahaha, HEHEHEHEHEHEHEE-HEE!!!",
	"You howl at the moon like a wolf while your helmet is surrounded by a terrible black aura.",
	"Well done! Your armor is now cursed.",
	"You turn into a soft girl!",
	"Uh-oh... there has been a strange increase in the number of trees lately. This is of course very dangerous :-), because if it turns out that this is the work of Poison Ivy, we'll have a major panic on our hands.",
	"Rattle/clink! Everyone in your vicinity woke up due to the loud sound.",
	"BANG! You die.",
	"You realize that you've made a horrible mistake.",
	"Your power's down, and therefore you cannot apply anything.",
	"This dungeon level does not seem watery at all.",
	"Things turn into an unrecognizable blur.",
	"You slow down greatly due to the rain.",
	"You take a selfie.",
	"You feel bad for tunneling, and are also blinded by heaps of earth flying around.",
	"You begin applying nail polish.",
	"You finish polishing your nails.",
	"Your wands are consumed to restore your mana.",
	"Your food dissolves!",
	"You put the knife to your lower arm and cut...",
	"You use a sharp object to cut open your belly...",
	"You slide your body along a rough surface and sustain terrible skin rashes.",
	"You rip your butt open with a metallic edge.",
	"You scratch up and down your leg with a sexy leather pump until it starts bleeding.",
	"You slit your leg full length with a sharp-edged zipper.",
	"Incoming message for Player 1. The message is from 'Lou'. It reads: 'Keep your garbage with you!'",
	"Incoming message for Player 1. The message is from 'Larissa'. It reads: 'If I kick you, I do it with full force. Don't cry out in pain!'",
	"Incoming message for Player 1. The message is from 'Sandra'. It reads: 'It's fun to scratch up and down your legs with my very sharp-edged combat boot heels!'",
	"Several monsters come out of a portal.",
	"You're immobilized by stationary monsters!",
	"Your experience is drained!",
	"You receive an electric shock out of nowhere!",
	"You receive a static shock out of nowhere!",
	"Alert! You are standing on a trap!",
	"You feel that the party is over!",
	"You feel a little drunk!",
	"You feel an uncontrolled stunning!",
	"You feel numbness spreading through your body!",
	"You feel that your marriage is no longer safe...",
	"You feel worried about the future!",
	"You feel ice-cold!",
	"You feel that you ain't gonna get time for relaxing anymore!",
	"You feel the protective layer on your skin disappearing!",
	"You feel fearful!",
	"You feel red-hot!",
	"You feel a loss of medical knowledge!",
	"You feel that all girls and women will scratch bloody wounds on your legs with their high heels!",
	"You feel fliction in your hands!",
	"You hear faint weeping...",
	"Your life is sapped!",
	"You emit a grating, annoying sound.",
	"You can listen to Rodney's taunts inside your head...",
	"The entrance was disconnected!",
	"Incoming message for Player 1. The message is from 'Anna'. It reads: 'Don't just call me 'hussy' - use my official title, please! Call me the Supreme Hussy Club Chairwoman!'",
	"Incoming message for Player 1. The message is from 'Lisa'. It reads: 'I'll slap you for overtaking me!'",
	"Incoming message for Player 1. The message is from 'Henrietta'. It reads: 'It is your fault that I stepped into a heap of dog shit! You tripped me! Now you have to clean my boots. By hand.'",
	"Incoming message for Player 1. The message is from 'Bluescreen'. It reads: 'Don't forget to join the #em.slashem.me IRC channel on Freenode if you haven't already!'",
	"Incoming message for Player 1. The message is from 'Bluescreen'. It reads: 'Every year there's the traditional Junethack tournament in June! Participate and collect all the trophies! Also join my clan, because we're the sexy players that play slex (and occasionally other variants too). :P'",
	"Incoming message for Player 1. The message is from 'Bluescreen'. It reads: 'Seriously, join the IRC channel #em.slashem.me now. Chances are you're a player who is not familiar with this NetHack variant, and I will be able to answer the 200 questions you probably have.'",
	"Incoming message for Player 1. The message is from 'Bluescreen'. It reads: 'Don't just play in the closet! Contact me - join the #em.slashem.me channel on Freenode IRC and I will give you hints on how to play, plus there's other players too who can also provide advice!'",
	"Incoming message for Player 1. The message is from 'Bluescreen'. It reads: 'If you want to make me very happy, do a screenshot or video Let's Play of this game! But alternately, posting an image of a pair of gentle asian peep-toe high heels works too. :)'",
	"Incoming message for Player 1. The message is from 'Famine'. It reads: 'Just thinking about you already makes me hungry. Come to the Astral Plane now so I can make you lose the game one step away from the high altar!'",
	"Incoming message for Player 1. The message is from 'Pestilence'. It reads: 'Are you ready to face me? I will keep making you deathly sick, until eventually your unicorn horn fails. And then you die.'",
	"Incoming message for Player 1. The message is from 'Death'. It reads: 'You have no chance, for I will drain all of your maximum hit points and kill you.'",
	"Incoming message for Player 1. The message is from 'Miriam'. It reads: 'It is so much fun to dance on your toes with the cone heels of my combat boots.'",
	"Incoming message for Player 1. The message is from 'Lydia Montenegro'. It reads: 'It's been a long while since the last time a burly man was ready to let me kick him in the nuts repeatedly. Just the thought of doing it again is enough to make me all wet...'",
	"Incoming message for Player 1. The message is from 'G-cheater'. It reads: 'Titty fuck!'",
	"Incoming message for Player 1. The message is from 'Ella'. It reads: 'Yeah right. Attack me from behind. You'll only have yourself to blame if I smash your skull into the wall, you miserable cunt.'",
	"Incoming message for Player 1. The message is from 'Gudrun'. It reads: 'Yes, our new hiding place is in Caesar's fort, now that the previous tenants moved out. All we have to do is to get on good terms with the legionnaires.'",
	"Incoming message for Player 1. The message is from 'Gudrun'. It reads: 'Benedetto sia Cesare e il suo impero!'",
	"Incoming message for Player 1. The message is from 'Gudrun'. It reads: 'Saluti. Felice di vederti.'",
	"Incoming message for Player 1. The message is from 'Gudrun'. It reads: 'Buon giorno, mi sarebbe piaciuto un po 'di ghiaccio ben mescolato, mescolato con tre palline di cioccolato, pero, e il mio amico qui ottiene la scelta su di me un po'.'",
	"Incoming message for Player 1. The message is from 'Jil'. It reads: 'You are crazy! We will fully decimate you!'",
	"Incoming message for Player 1. The message is from 'Katharina'. It reads: 'Please fight back! If you allow me to break your shins with my plateau boots, I'll burst out in tears!'",
	"Incoming message for Player 1. The message is from 'Jessica'. It reads: 'I'm afraid of telling you, but the type of ice block will get your money tomorrow, no matter what. And he will donate all of it to the refugee aiding fund.'",
	"Incoming message for Player 1. The message is from 'Jessica'. It reads: 'I voted for David, because his wavelength is closer to mine than yours. Please don't be angry!'",
	"Incoming message for Player 1. The message is from 'Jessica'. It reads: 'Oh God! Don't fight the type of ice block! You can never defeat him!'",
	"Incoming message for Player 1. The message is from 'Almuth'. It reads: 'Be glad that we protect you from the type of ice block. He is the mightiest robber in the airspace, err, of all times, and you could never stand up to him.'",
	"Incoming message for Player 1. The message is from 'H.'. It reads: 'Hahaha, you're out of luck now, sucker. I won't give you back the sword I stole from you, and you ain't getting no money from me either. Use your own money to buy a new sword, and watch out or I'll steal it again! HAHAHA!'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'Ey yo you dirty little asshole, I'll fuck you in the ass now. Write your last will too you fucker, ey yo ey.'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'EY YO YOU ASSHOLE ALREADY AGAIN! YOU ARE NOW D-E-A-D KNOW YA? EY YO YOU CANNOT GO FUCK MY GIRLFRIEND, REALLY NOW! IF YA NOT FUCK OFF RIGHT NOW I MAKE YOU DEADER THAN DEAD, REALLY NOW!'",
	"Incoming message for Player 1. The message is from 'Sandra'. It reads: 'Fork over your cash or I'll scratch your legs full length.'",
	"Incoming message for Player 1. The message is from 'Sabine'. It reads: 'You'll get a couple thwacks over the head now, you unfaithful bastard! You were hanging around with some stinking sluts with syphilis while I was gone, but my block-heeled sandals will punish you for that!'",
	"Incoming message for Player 1. The message is from 'Corina'. It reads: 'Did you just call me a bitch?'",
	"Incoming message for Player 1. The message is from 'Corina'. It reads: 'Don't call me a bitch ever again, or I'll smash my stiletto heel into your face.'",
	"Incoming message for Player 1. The message is from 'Klara'. It reads: 'Let me examine you. Hmm... this looks like a progressive osteoporosis to me! I can treat it, but it might sting a little. If you have morphine with you, now would be the time to use it.'",
	"Incoming message for Player 1. The message is from 'Maurah'. It reads: 'I will teach you what the term 'division of work' means today. One worker makes only soles, the next one creates the shafts, yet another one does the lacings... and so on. The result is that shoes can be produced much faster!'",
	"Incoming message for Player 1. The message is from 'Maurah'. It reads: 'Argh... my pink stiletto heels are killing my feet...'",
	"Incoming message for Player 1. The message is from 'Melissa'. It reads: 'Welcome. You're new around here. Honestly, you don't look like much to me, but if Gudrun brought you here then she probably sees some talent in you, so I'll welcome you too.'",
	"Incoming message for Player 1. The message is from 'Ariane'. It reads: 'If your legs stop bleeding, I will apply my very fleecy black combat boot heels to re-open the wounds.'",
	"Incoming message for Player 1. The message is from 'Ariane'. It reads: 'Please sustain a blood poisoning! I would absolutely adore watching your body struggle with the bacteria, not to mention the gradual skin discolorations you'd get!'",
	"Incoming message for Player 1. The message is from 'Jasieen'. It reads: 'My father whipped my little sister with a belt! He's such a terrible person! I'm so glad that I won't ever have to see him again.'",
	"Incoming message for Player 1. The message is from 'Annemarie'. It reads: 'I'll use the turbo power booster.'",
	"Incoming message for Player 1. The message is from 'Roswitha'. It reads: 'You can't eliminate me! I'm a honorable lady! And you won't get my lacquered darling pumps either!'",
	"Incoming message for Player 1. The message is from 'Anita'. It reads: 'Why am I being benevolent towards you anyway? Now you can however which expierience! My pumps will scratch bloody wounds on the entire length of your legs!'",
	"Incoming message for Player 1. The message is from 'Kerstin'. It reads: 'When I'm wearing my combat boots, I'm very gentle. Therefore I will only scratch your legs a bit with my block heels, and I watch out so it won't bleed.'",
	"Incoming message for Player 1. The message is from 'Kerstin'. It reads: 'Don't underestimate my wooden sandals. I will break your shins if you don't watch out, and in any case it will be rather painful.'",
	"Incoming message for Player 1. The message is from 'Kerstin'. It reads: 'Prepare to be bludgeoned by my dark blue Puma sneakers. And don't forget to inhale their beguiling odor. Oh, almost forgot: I also know Taekwondo. Let's see how long it takes for me to knock you down!'",
	"Incoming message for Player 1. The message is from 'Verena'. It reads: 'Help! I can't swim! Please rescue me! Pull me out of the water!'",
	"Incoming message for Player 1. The message is from 'Inge'. It reads: 'The gas takes a while to accumulate, but once it's finished, I can produce very tender farting noises with my butt cheeks to release it.'",
	"Incoming message for Player 1. The message is from 'David'. It reads: 'A boozer contest? Err... I'll take a range check on that.'",
	"Incoming message for Player 1. The message is from 'Sue Lyn'. It reads: 'I want to cut up your skin with my very sharp-edged female fingernails.'",
	"Incoming message for Player 1. The message is from 'Marihuana'. It reads: 'Alright, you insolent rascal. You will treat me with utmost respect or you will have a real problem! I don't tolerate your behavior, and if you step out of line one more time I will make sure you're eliminated permanently!'",
	"Incoming message for Player 1. The message is from 'Larissa'. It reads: 'Don't damage my plateau boots or I'll kick you very painfully!'",
	"Incoming message for Player 1. The message is from 'Sophie'. It reads: 'I love block heels! They're super comfortable!'",
	"Incoming message for Player 1. The message is from 'Almuth'. It reads: 'My wonderfully soft Reebok sneakers will slide along your hands again and again, and your blood will squirt everywhere.'",
	"Incoming message for Player 1. The message is from 'Alice'. It reads: 'You said you wanted to STIMULATE MY VULVA??? Shut up right now or I'll report you to the principal!'",
	"Incoming message for Player 1. The message is from 'Harro'. It reads: 'I was told that you were going around my back, telling others that I allegedly don't own a house. That's gonna cost you, rascal.'",
	"Incoming message for Player 1. The message is from 'Mister Head'. It reads: 'You failed your hockey lessons! Seriously, I've never seen a performance as bad as yours! You should be ashamed, and never touch a hockey stick ever again.'",
	"Incoming message for Player 1. The message is from 'Miss Schrack'. It reads: 'Now that my black lady pumps scratched your legs to the bone and are squirting blood with every step I take, you're free to go to your lesson.'",
	"Incoming message for Player 1. The message is from 'Dr. Ramov'. It reads: 'What the hell are YOU doing here? Go back to your classroom! NOW! And I don't care that you're bleeding like a slaughtered pig, because I got my doctor's degree by taking the short bus to school!'",
	"Incoming message for Player 1. The message is from 'Annie'. It reads: 'Je ne peux pas souffrir que de toute l'ecole, mais je ne se soucient pas ici dans mes classes mes regles.'",
	"Incoming message for Player 1. The message is from 'Marike'. It reads: 'I know why you can't concentrate. You must be thinking about my soft butt cheeks and the beautiful farting noises I can produce with them!'",
	"Incoming message for Player 1. The message is from 'Kati'. It reads: 'If my boyfriend slaps me on the butt, I usually come right away.'",
	"Incoming message for Player 1. The message is from 'Peeta'. It reads: 'wat katnis wus det'",
	"Incoming message for Player 1. The message is from 'Karin'. It reads: 'I have a lot of money, because I got a scholarship and don't have to pay for attending the university.'",
	"Incoming message for Player 1. The message is from 'Victoria'. It reads: 'If you want to pass through my room, you need to allow me to practice my martial arts on you. I will kick you repeatedly with my fleecy block-heeled combat boots.'",
	"Incoming message for Player 1. The message is from 'Natalia'. It reads: 'Once I tried to kill my naughty son by repeatedly whacking him over the head with my wedge sandals. Didn't work. Then I tried to at least break his shins by kicking him again and again. Didn't work either.'",
	"Incoming message for Player 1. The message is from 'Nora'. It reads: 'My red sneakers are very important to me! I must have them back!'",
	"Incoming message for Player 1. The message is from 'Sophia'. It reads: 'Just wait. Once my stiletto heels hit your legs, I will scratch your skin absolutely mercilessly and you will beg for mercy while bleeding to death.'",
	"Incoming message for Player 1. The message is from 'Sophia'. It reads: 'Now you made me really angry! I will split your damn skull with my high heels. I will whack you again and again, as many times as necessary in fact, until you finally die.'",
	"Incoming message for Player 1. The message is from 'Tanja'. It reads: 'Do you want to embrace me?'",
	"Incoming message for Player 1. The message is from 'Tanja'. It reads: 'Fighting with you in the arena was a lot of fun. I'm amazed you actually managed to avoid the taekwondo arts of my soft girl sneakers, and I very much enjoyed it when you caressed my footwear, too.'",
	"Incoming message for Player 1. The message is from 'Anne'. It reads: 'You're just a retard.'",
	"Incoming message for Player 1. The message is from 'Madeleine'. It reads: 'I must absolutely try out my plateau boots on you! Please, present me your shins! I'd love to kick you with my sweet girl boots!'",
	"Incoming message for Player 1. The message is from 'Madeleine'. It reads: 'The only combat move that my black calf-leather sandals know is the shin kick of love, but they will keep using it and cause a wonderful air current noise every time, err, I meant a sensation of nagging pain of course!'",
	"Incoming message for Player 1. The message is from 'Manuela'. It reads: 'I refuse to talk to you. And don't ever ask me stupid things again, like how many pairs of combat boots I have!'",
	"Incoming message for Player 1. The message is from 'Driving Impulse'. It reads: 'Ohh, so I caught you playing NetHack on the public PC! I didn't expect you, of all people, to do that, actually. Now I have to eliminate you from the computer room, sadly.'",
	"Incoming message for Player 1. The message is from 'Desert Dwarf'. It reads: 'No, I did not notice that one of my pupils got both his legs slit full length by a girl in black combat boots. And things that I don't see, don't exist. So you can just stop bothering me. But I am a very good teacher.'",
	"Incoming message for Player 1. The message is from 'Mister Garbage Houses'. It reads: 'You sat on the mat car, which is strictly prohibited. And then you did it again even though I admonished you. Now you write the sentence 'I may not sit down on the mat car' 80 times until tomorrow, and I don't care at all that it's your birthday today because I'm the biggest asshole teacher of this school and fucking proud of it.'",
	"Incoming message for Player 1. The message is from 'Dickmouth'. It reads: 'Caught you playing games during my lesson! So... you're playing a game where a bunch of letters and a @ sign are walking around in a house or whatever that's supposed to be? Anyway, I'll confiscate your playing device now.'",
	"Incoming message for Player 1. The message is from 'Raini'. It reads: 'You were using the wrong font, so you get a punishment: Until next lesson you will do 5 pages worth of exercises in the math book.'",
	"Incoming message for Player 1. The message is from 'Raini'. It reads: 'What, you knocked down another pupil and then kicked him in the ear while he was down? You'll get detention for that!'",
	"Incoming message for Player 1. The message is from 'be silent kert'. It reads: ''Quickly the be silent kert is coming!' Yes, trying to cheat me, huh? I swear, this is some top-grade bullshit you're pulling here.'",
	"Incoming message for Player 1. The message is from 'Iris flax'. It reads: 'My high heels are made of pure silver and can scrape off the skin from your bones in a matter of seconds.'",
	"Incoming message for Player 1. The message is from 'Schrumpel'. It reads: 'You dared scribbling something on the desk with a pencil that can easily be erased? Well, not with me, my friend. I give you detention even though that's actually way too harsh a punishment for such a laughable offense.'",
	"Incoming message for Player 1. The message is from 'Marika'. It reads: 'I will stomp your nuts flat.'",
	"Incoming message for Player 1. The message is from 'Marleen'. It reads: 'What, you're complaining that I'm supposed to kick you with softer shoes? But my combat boots have very soft block heels! Do you really want to tell me you don't find them soft enough?'",
	"Incoming message for Player 1. The message is from 'Non-Pretty Melanie'. It reads: 'I am pretty! And my Reebok sneakers look incredibly soft!'",
	"Incoming message for Player 1. The message is from 'The Heeled Topmodel'. It reads: 'Ever since I developed the eating disorder called 'veganism', I refuse to wear high heels because all of my pairs are made of organic material. Now I can only wear shoes made of cotton or synthetic material, even though that's really stupid.'",
	"Incoming message for Player 1. The message is from 'Fanny'. It reads: 'I bought yet another brand-new pair of sneakers! They look incredibly cute!'",
	"Incoming message for Player 1. The message is from 'Fanny'. It reads: 'My old velcro sneakers are no good anymore. All too often the evil boys from my class peed into them because they are evil.'",
	"Incoming message for Player 1. The message is from 'Fanny'. It reads: 'I love my winter boots. They're comfortable and I can also perform my karate moves with them.'",
	"Incoming message for Player 1. The message is from 'Fanny'. It reads: 'My female blue-green sneakers look like they absolutely want to kick male persons in the nuts!'",
	"Incoming message for Player 1. The message is from 'Fanny'. It reads: 'I got told that when aiming for the opponent's nuts, you can also use your knee. But I prefer using my feet.'",
	"Incoming message for Player 1. The message is from 'Elena'. It reads: 'So the only reason you wanted to undress me is because you wanted to caress my sexy butt cheeks? Is that it?'",
	"Incoming message for Player 1. The message is from 'Dora'. It reads: 'I wear hugging boots! Hahaha!'",
	"Incoming message for Player 1. The message is from 'Hannes'. It reads: 'My covert operations are top secret.'",
	"Incoming message for Player 1. The message is from 'Hannes'. It reads: 'Sorry, but the information you're requesting is classified.'",
	"Incoming message for Player 1. The message is from 'Ludgera'. It reads: 'Yes, I can kick you with my hiking boots if that's what you really want.'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'Yes, sure, keep shooting at me. With every bullet you fire you're just bringing yourself closer to defeat!'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'Apart from this player the battlefield is suspciously quiet today. Player, are you waiting for me to end you?'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'All is lost, player. You should surrender - for the good of your sanity!'",
	"Incoming message for Player 1. The message is from 'Rhea'. It reads: 'Yes, I clicked open all the drawers and tore up all the zippers of your backpack while you were on your way home. After all, I had to verify that you have everything with you that you need for school.'",
	"Incoming message for Player 1. The message is from 'Julia'. It reads: 'Oh, you request that I repeatedly whack you over the head with my very high-heeled combat boots? Wow, I thought you'd never ask! Very well. You will feel wonderful pain when the block heel slams onto your skull again and again.'",
	"Incoming message for Player 1. The message is from 'Sandra'. It reads: 'Being scratched by my cone-heeled combat boots is fun, because they can put such beautiful pretty lovely scratching wounds on your legs and it will certainly feel fleecy-soft!'",
	"Incoming message for Player 1. The message is from 'Ruea'. It reads: 'Tek bir gercek dindir ve Tuerkiye cumhurbaskani, onu buetuen duenyaya yayacak bilge bir adamdir! Allah bueyuek!'",
	"Incoming message for Player 1. The message is from 'Robat'. It reads: 'I'll gun down one of the girls and put a note down next to her corpse to warn all others so they know better than to mess with me.'",
	"Incoming message for Player 1. The message is from 'Julia'. It reads: 'Oh, no idea, I often don't understand what's wrong with my boyfriend... sometimes I really wish he'd just dump me and get another girl, but then again, no girl should ever have to go through my suffering...'",
	"Incoming message for Player 1. The message is from 'Magdalena'. It reads: 'Now my combat boot heels will really scratch you to death! You challenged me for the last time!'",
	"Incoming message for Player 1. The message is from 'Magdalena'. It reads: 'This is my declaration of love. You are now entitled to inhale the wonderful smell that emanates from my green socks, a privilege that I normally don't give to anyone except my boyfriend.'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'You're testing my patience, player. Remove your fat ass from my sights!'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'Player, if you do not remove your ass from this area, I will remove you permanently.'",
	"Incoming message for Player 1. The message is from 'Martin'. It reads: 'Hi there! Watch out, don't walk into my minefields!'",
	"Incoming message for Player 1. The message is from 'Martin'. It reads: 'My girlfriend can do magic. I want to be able to do it too, but somehow it doesn't work... oh well, I'll try it out on you! 'IMPERIO!' Did it work?'",
	"Incoming message for Player 1. The message is from 'H.'. It reads: 'I stole your sword. You seriously were such a twit and didn't watch out, so the sword is mine now.'",
	"Incoming message for Player 1. The message is from 'Susanne'. It reads: 'Ooh, malachite! How pretty!'",
	"Incoming message for Player 1. The message is from 'Robat'. It reads: 'You miserable hussy! You stinking bitch without a tongue! If I get you into my fingers I'll finish you off!'",
	"Incoming message for Player 1. The message is from 'Meltem'. It reads: 'You know what? If you promise to spend a romantic night with me today, I'll lend you 500 zorkmids.'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'The little black dress that you're looking at is meant for my girlfriend, not you. I would advise you to not think about stealing it if you know what's good for you...'",
	"Incoming message for Player 1. The message is from 'Robat'. It reads: 'You fight like a little girl, unlike me! I fight like a true warrior of the old school!'",
	"Incoming message for Player 1. The message is from 'Sabrina'. It reads: 'Oh, what are you doing here? Maybe you want to buy something? How about a golden amulet for only 1000 zorkmids? Or if you don't have much money, I can also offer you a whetstone for only 20 zorkmids!'",
	"Incoming message for Player 1. The message is from 'Robat'. It reads: 'Gimme your money. This a raid.'",
	"Incoming message for Player 1. The message is from 'Yvonne'. It reads: 'My boyfriend is not here today, so I want to have sex with you instead. Come on, I need my daily orgasm.'",
	"Christina Aguilera's perfume is beguiling, and you have trouble concentrating!",
	"You deeply inhale Elena's feminine scent.",
	"Anita's odor cloud made of concentrated perfume infiltrates your nose!",
	"Vanessa attacks you with a fragrance cloud!",
	"Sonja is close enough that you can smell her perfume... but it's way too concentrated, and inhaling the aroma makes you dizzy!",
	"The lovely scent of femininity floods your nostrils... until you realize that it's getting ever stronger, and you are having trouble when breathing!",
	"Hilda's perfume is so scentful that she reminds you of what your aunt smells like when she comes for a visit on Christmas! Ugh! (Bonus points if you actually have an aunt named Hilda.)",
	"You are glad you're not playing Quake 3 Arena because you suck at it anyway you fucking noob",
	"You hear a voice taunt you: 'Make an error, due due due! Make one error!'",
	"The item rusted away completely!",
	"You are standing next to a superthing from level 55!",
	"For some reason, you have to take a shit right now.",
	"For some reason, your asian butt cheeks have to take a shit right now.",
	"You feel that you have to take a shit, and so you do.",
	"Suddenly, you produce beautiful farting noises with your sexy butt.",
	"Elif suddenly kicks your hand with her very soft, female sneakers, and draws blood!",
	"Elif suddenly uses her very sharp-edged female fingernails and cuts your unprotected skin!",
	"Elif suddenly strokes you gently with her very soft, fleecy hands!",
	"Oh no! You were standing still for too long, and are horribly scarred by a bunch of female painted asian toenails. Your sexy high heels are also damaged.",
	"The nails cut you fatally and you die.",
	"You gotta keep dancing...",
	"You missed the beat! Continue dancing or suffer!",
	"Your loud voice aggravates the denizens of the dungeon.",
	"The air in the anoxic pit does not contain oxygen! You can't breathe!",
	"You feel that jonadab really should call the anoxic pit by its proper name, because 'hypoxic pit' sounds rather lame by comparison.",
	"You feel that the elder priest in dnethack is a filthy cheater. Thank all the gods you're playing a well-balanced variant instead!",
	"You feel that Sporkhack's percentage-based resistances are a bunch of bullshit because they never work right. Here in SLEX, you have 95%% certainty that a resistance will actually protect you!",
	"You feel that the gelatinous cubes and beholders in Grunthack are way more evil than anything SLEX has to throw at you.",
	"Donald Trump and Theresa Flauschebueschel May would make a beautiful couple.",
	"You feel that FIQhack's color scheme is really terrible and deliberately uses the most glaring, eye-straining colors that exist.",
	"You feel that Unnethack's Cthulhu is way overpowered, and the fact that wand zaps miss 9 out of 10 times in non-slex variants is also a bunch of bullshit.",
	"You certainly never want to play one of the variants where unskilled weapons are trashed on purpose (Unnethack, Sporkhack and Dynahack come to mind), because seriously, the early game is hard enough as it is already!",
	"You absolutely despise the irrationally high chance of lightning blowing up your wands and rings in vanillaoid nethack variants, and especially the fact that blue dragon breath ignores reflection in Dynahack. Good thing you're playing a sane variant instead!",
	"You agree with AmyBSOD that Nethack4 and related variants would do well to include a fucking rcfile that you can copy over easily. But no, those satanic variants have you go through a stupidly designed options menu for keybindings and shit, because Nethack4 was developed by SATAN!",
	"While you can see why the NetHack 3.6.0 dev team nerfed Elbereth, you know for a fact that SLEX's Elbereth nerf is way better, nerfing it to a point where it doesn't automatically win you the game but without making healers absolutely impossible to play in the early game",
	"You are just so glad that you don't have to play Nethack Fourk, where losing alignment points means you can just suicide as well because some sadistic developer never bothered to add a way to actually improve your alignment record.",
	"You are just so glad that you're not playing FIQhack, where you'll always have to race to any potions or scrolls on the ground because the next monster is going to waste them if you don't.",
	"You are just so glad that you're not playing SLASHTHEM, which is boring beyond belief, haphazardly reverts things that Amy changed (including reverting her bugfixes) and removing all the things that made the new roles and races unique.",
	"You are just so glad that you're not playing SLASHTHEM, where the balance is completely out of wh@ck and it's all just about getting the most overpowered artifacts as quickly as you can and then you've won already (yaaaaaaawn).",
	"You are just so glad that you're not playing SLASHTHEM, where the best features are being removed, the stupidest ones remain and new, even more stupid features are added.",
	"nothing went anymore",
	"Tchueb!",
	"Jeanetta uses her cute little boots to scrape a bit of skin off your shins!",
	"You long for more!",
	"You long for the permission to pull down the soft pants that cover Elena's sexy butt.",
	"You absolutely have to move your nose towards Katharina's sexy butt cheeks so she can fart you right in the face.",
	"Oh no! Some evil bastard bashed your tender, vulnerable butt cheeks!",
	"Your tender butt cheeks are stimulated by a caressing touch... and you start producing tender farting noises.",
	"You love the nasty traps that have been set up by Arabella.",
	"You love this game and its interface screw traps. Especially the yellow spell one. :-)",
	"Just a hint: Arabella put a checkerboard trap on this level. Don't trigger it!",
	"There is a clockwise spin trap somewhere one level down from your position. You really don't want to step into that!",
	"The next monster that is spawned will have a quasar trap underneath it. Just thought you could use a warning so you'll search for it instead of blindly running into it!",
	"Tschoerldelah!",
	"Daedidedaedaepooooooouch!",
	"Schiaeau!",
	"The dungeon no longer seems feminine.",
	"Your shins are no longer super sensitive to being kicked by women.",
	"The dungeon is less green now.",
	"You come back to your senses and realize that stepping into a heap of shit is, well, shit.",
	"You no longer have super tender butt cheeks, or a super tender body for that matter.",
	"At last, your voice is normal again, although you're also considerably less sexy than you used to be.",
	"You survived the wrath of the women, and feel safe from them now.",
	"The female dungeon inhabitants decide to make their butts Geneva-compliant again.",
	"Your lust for pretty women fades. Aww...",
	"You no longer need to constantly go to the toilet, and also your physique seems normal now.",
	"The lovely ghostly girl named Elif disappears, telling you that she enjoyed the time she spent with you.",
	"Somehow, you're still alive. Your legs are now safe from any imaginary winter stilettos.",
	"After dancing for a long time, you are exhausted. And thus, at last you may rest again. But your sexiness and all the other benefits fade.",
	"Jeanetta is done collecting your skin shreds. Better don't visit her trophy hall unless you got a strong stomach.",
	"There are no more toilets being built specifically for you.",
	"Your butt is no longer as sexy as before, and will no longer fart unless you tell it to do so.",
	"You no longer seem feminine.",
	"Your shins look like they really enjoy pain! Do you want me to phone Arabella so she can come and kick you repeatedly with her Asian girl boots? Maybe then you will come too! :-)",
	"The dungeon is red now.",
	"The dungeon becomes multicolored.",
	"The dungeon now displays everything in black and white.",
	"The dungeon displays... err, I mean, IS displayed, in black and white.",
	"The messages don't make sense.",
	"The messages make a lot of sense.",
	"The Amy is bored, and therefore keeps typing silly messages for you to read.",
	"Nothing's wrong.",
	"This is alright.",
	"Something does not tune.",
	"What is the matter?",
	"What is now already again loosely?",
	"Go! Squad!",
	"Fire in the ass hole!",
	"I will kick you in the god damn ass!",
	"When we realized you might actually make it here, we decided to draw straws to see who should stay behind to let you through. Unfortunately we didn't actually *have* straws, so we drew other things at our disposal. Obviously, I had the short one.",
	"You feel that you absolutely have to step into the next heap of dog shit you find because your inner voices command you to do so.",
	"You feel that you want to step into a heap of shit with profiled shoes, because it's fun to clean them again.",
	"Your butt cheeks become super tender.",
	"Your body becomes super tender, and you lose 10 points of constitution and 5 points of strength.",
	"You are less sexy than you used to be and lost 10 points of charisma.",
	"You read yet another stupid message, and the effect of this particular one is like being sucked by a mind flayer's tentacles. As a result, you lost 3 points of intelligence.",
	"The image of the weeping angel appears in your mind! You lose Wisdom!",
	"The weeping angel is zapping wand of digging!",
	"The weeping angel is zapping wand of digging! Deutsches Sprach schweres Sprach!",
	"You survived the wrath of the women... until now. But the warrior princess with her super-tender stiletto heels was just spawned on the level and will absolutely bludgeon you and scratch up and down your legs until you bleed to death.",
	"You are permanently interlocked with the weeping angel!",
	"WARNING: you have been contaminated by the image of the weeping angel! Read a scroll of remove curse or pray on a coaligned altar within 5000 turns, or you automatically die!",
	"The female dungeon inhabitants decide to make their butts produce large amounts of weaponized fart gas.",
	"Your lust for pretty women reaches a new high!",
	"Your lust for pretty women reaches a new high! Where are some pretty femmy lesbians that you might hit on?",
	"You developed diarrhea. You must use a toilet once every 1000 turns, or die of a stomach rupture.",
	"Your legs will never be safe from the feminine winter stilettos.",
	"No matter how exhausted you are, you may never rest again.",
	"Your sexiness fades... wait, were you ever sexy to begin with?",
	"Your consciousness fades... that's what playing this NetHack variant will do to you. (Don't you love AmyBSOD? :D)",
	"There is a trophy hall where all the skin shreds that were scraped off your shins by female high-heeled shoes are kept as exhibitions!",
	"Do you like high heels?",
	"Would you like it to get your legs scratched up and down by a pair of female high heels?",
	"Did you ever wonder why there are only female high heels, and no male ones?",
	"In the equality world, all women have to wear high heels at all times, and all men are forced to wear flats.",
	"On dungeon level 20, a toilet has been built specifically for you! If you sit on it while satiated, you will acquire intrinsic slow digestion!",
	"Your butt signals that it wants to fart in a very feminine way!",
	"Nasty!",
	"Now you check it.",
	"Keep funky.",
	"Enemy reduced!",
	"Due to your movement restriction, you cannot quicktravel!",
	"Due to AmyBSOD being evil, you cannot quicktravel!",
	"Arabella decided to deactivate quicktravel for you. Have fun walking everywhere manually!",
	"You do realize that roguelikes with 'auto-explore' commands might just as well have a button that plays the entire game for you, right? How lazy can one get?",
	"You feel that being kicked in the nuts by a lovely girl is a joyful experience for a guy, because it gives the tender female girl a chance of testing his pain threshold.",
	"Such a lovely, fleecy girl!",
	"The little girl swings herself around you!",
	"The lovely girl places her boots fully into your antinode bulge.",
	"You see here a difficult angular sign.",
	"Gliiiiiiiiiieaun uey!",
	"You're sensitive to light.",
	"Your ether disease gets worse.",
	"Your head becomes extra thick.",
	"Clouds of rain follow you.",
	"Mind lag!",
	"Laaaaaaaaaaaaaaaag!",
	"Do you feel that the game lags?",
	"Do you feel that the server lags?",
	"Is the server being laggy for you too?",
	"Do you want to commit sudoku or hari kari?",
	"Flicker strips are a lot of fun.",
	"Vote now for more fleecy-colored traps in SLEX!",
	"If Amy didn't budge on the transvestite role, and some may say it was considerably controversial, you can't really see her budging on much of anything. And that's a good thing, because otherwise SLEX would deteriorate into yet another boring casual game.",
	"Are you annoyed by certain features in this beautiful game? You can tell Amy about it, but don't expect her to actually change the things in question. She will only do so if she personally feels they ought to be changed.",
	"Polling users to find out what they want to have removed from the game? Uhh, no. That won't happen in SLASH'EM Extended, due to Amy's policy of not removing things.",
	"Being a social justice warrior and trying to get Amy to remove parts of the game that you dislike will not work. She's the Iron Lady of SLASH'EM Extended, and has never gave in to such requests in the past. So better don't expect her to do so now!",
	"Do you hate the interface screw traps of this game? Chances are that the answer is yes, and you're free to have that opinion of course. Unfortunately for you, they are a part of the game though and will not get removed no matter what. Accept their existence and enjoy the game. :-)",
	"You miss the evencore!",
	"Get off!  Get off!  Get off!  Get off!  Get off!  Get off!  Get off!  Get off! (Don't you also think the announcer must be crazy?)",
	"The top status line is getting tired of having to display all these messages and will shut down in 50 turns!",
	"If you want to prevent that your blood flows, you have to let it coagulate!",
	"You are enveloped by a dark ball of light. Dark. Light. A light that is dark. Yes, that absolutely makes sense. :P",
	"There's some things incapable of understanding!",
	"Air currents pull you into a shaft!",
	"Stupid gaping holes!",
	"You trip over a strange fountain!",
	"You trip over a strange throne!",
	"Suddenly, you realize that you forgot to think about Maud.",
	"Suddenly, you realize that you forgot to think about Maud. You also forgot that you were suffering from amnesia.",
	"You don't feel like yourself.",
	"You feel like you just turned into Amy!",
	"Pondering why you are running around in a dungeon looking for an amulet was just too much to comprehend.",
	"Poison washes over you!",
	"A feeling of benevolence washes over you, but you don't actually feel better!",
	"Suddenly your surroundings change.",
	"You hear squeaking noises. (At least it's not air current noises instead.)",
	"It's back to square one for you!",
	"Go directly to jail, do not pass go, do not collect 200 zorkmids.",
	"'Ha ha ha ha! Wa ha ha ha! You are so doomed'",
	"You are startled by a sudden sound.",
	"You are greatly startled by a sudden sound.",
	"You are completely startled by a sudden sound.",
	"You feel nauseated.",
	"Suddenly your tummy aches terribly!",
	"Now, the dungeon will be more feminine for a while!",
	"Your shins can expect to get kicked repeatedly by all the girls and women now!",
	"Green is the new favorite color, it seems!",
	"Suddenly, you feel that you're going to step into a heap of shit.",
	"Your butt cheeks suddenly feel very tender, and in fact, a similar thing is happening to your entire body!",
	"You suddenly have a very grating, aggravating voice, and you start to emit a beguiling odor! In fact, you're super sexy and sweet now!",
	"All the female denizens of the dungeon will show you their true power, and it will happen very soon!",
	"You feel that the girls and women are getting ready to use their sexy butts as weapons.",
	"You long for beautiful sexy women with tender butt cheeks and lovely high heels.",
	"You feel that you'll want to use the toilet more often. Also, somehow your physique seems weaker now...",
	"You feel that you angered the womanhood. If you now hit a woman, you will be hit with retribution!",
	"You just know that your legs are going to be ripped open by very sharp-edged combat boot heels.",
	"If you ever stand still for too long, a bunch of bloodthirsty female painted toenails is going to hurt your beautiful skin, so make sure you keep moving!",
	"Lots of shreds of skin will be scraped off your shins, and the girls will enjoy it.",
	"You feel that people are building toilets for you to use.",
	"Your sexy butt signals that it wants to produce beautiful farting noises!",
	"Your nuts can expect to get kicked repeatedly by all the girls and women now!",
	"Suddenly, you feel that you're going to step into a heap of shit. You quickly change into a pair of profiled shoes to ensure it'll be more fun.",
	"You want to have your ass resoled!",
	"A dim glow surrounds you...",
	"The floor around you vibrates in a strange way.",
	"CLICK! High tension! Fight for your life!!!",
	"CLICK! Monsters! So many monsters! Good luck surviving the seething horde!",
	"CLICK! All you can make out is a strong predominant color... and an endless amount of monsters.",
	"CLICK! The entire area is filled with monsters! And they have one thing in common: they want to make your life miserable!",
	"Your kitten triggers a trap and dies!",
	"Your domestic dragon triggers a trap and dies!",
	"You lead your saddled warhorse into a trap and it dies.",
	"You sense the death of a companion.",
	"You sense the death of a pure, innocent being.",
	"Nadja is really angry about the fact that you tried to hit her, and uses a pair of buckled shoes to scratch up and down your legs, ripping a lot of skin to shreds.",
	"Nadja notices that you're bleeding, which seems to make her even more angry as she continues slitting your legs full length with the metal buckle!",
	"You are severely hurt, but Nadja just doesn't want to stop punishing you with her very female buckled shoes. She continues destroying your unprotected skin and announces that if you hit her one more time, she will kill you.",
	"Please press the sweet, candy-like button!",
	"The beguiling stench emanating from your pink sneakers fills the area...",
	"Nad yes flax!",
	"You've forgotten who you are, but you are back.",
	"Flauusch! *bundlebundlebundle* Gogo junethack team Dislexiapater! TROPHY GET!",
	"*bundlebundlebundle*",
	"Play SLEX! *bundlebundlebundle*",
	"*b*u*n*d*l*e*",
	"*bundlebundlebundlebundlebundlebundlebundle bundle*",
	"Such a long bundle!",
	"Schwatsch!",
	"Somehow the information is fucked up and won't display.",
	"You can't use that direction!",
	"You can't use the same direction twice in a row!",
	"A sinister force prevents you from quicktraveling!",
	"A sinister force prevents you from quicktraveling! Could this be the work of Arabella?",
	"You do not have a weapon light!",
	"You crash into a set of iron bars! Ouch!",
	"You crash into a wall! Ouch!",
	"You crash into a wall! (Are you blind or retarded? If you are neither, you shouldn't be so stupid and run into a clearly visible wall!)",
	"There is a tree in the way, and I feel like telling you that!",
	"The bird claws sensitive parts of your body!",
	"Demogorgon deeply stings you for being such a spoiled princess.",
	"The demimondaine angrily cuts up your unprotected princess skin.",
	"The demimondaine pulls down your pants and starts to gently massage your nuts.",
	"The demimondaine pulls down your pants and starts to gently massage your nuts, but then she suddenly rams her sexy knee into them!",
	"Pokedex communication failure. Damn.",
	"That is a trap.",
	"There is no trap on this level.",
	"There is no trap on this level. Really.",
	"Your clumsy hands accidentally rip the scroll of genocide to pieces.",
	"Whoops, the potion of amnesia breaks as you try to pick it up. You forgot whether that will cause you to inhale its vapors...",
	"As you pick up the wand of wishing, energy drains from it!",
	"Urgh, your yellow sneakers hate getting wet!",
	"Eek!  Eek!  Eek!  Eek!  Eek!",
	"You cannot remove your suit to take off that up-down cloak.",
	"You faint from the terrible sounds.",
	"Your stomach fills.",
	"AmyBSOD has such a wonderful roommate!",
	"The Amy her roommate or roommaid is very wing-tufted and sometimes even has a special bundle!",
	"Eek! You can't stand farting gas!",
	"Bad luck! You die.",
	"Vrooooom, your ski heels speed up thanks to walking on snow!",
	"Jana suddenly appears and pees on you, and you melt instantly.",
	"In Nethack Fourk, monsters get a turn upon entering a new dungeon level, which can lead to your unavoidable death if it's minotaurs and mastodons. And apparently that's intentional. Mind you, the hypocritical creator of that fork complained about Yhelothar's bones levels back in the day.",
	"The watchmen in Nethack Fourk have stupid weird-ass names like 'Schtulman'. Seriously???",
	"Unnethack implemented a very evil patch idea where erosion can nuke your cloak of magic resistance. Granted, here in SLEX that can happen too, but at least I made it so that erosionproofing an armor piece is not such an unholy pain in the butt!",
	"SOMEONE (probably Satan) made it so that instead of two early-game trophies, Satanhack-based variants only have one for Junethack. Nethack Fourk then tried to make a mines end trophy... except that it does not work, I really wonder why couldn't Nethack4 and its descendants just keep the (WORKING!!!) vanilla 3.4.3 code?",
	"In Nethack Fourk, you actually cannot push boulders while hallucinating, which is some A-grade bullshit. Makes you wonder whether the developer planned to screw up that badly...",
	"You have mail from user 'AmyBSOD'. It reads: 'Currently I'm playing the worst nethack fork of all, Fourk, which I hereby decide to name Screwhack.'",
	"If you decide to play Nethack Fourk, well, if you lose alignment record, you might as well commit sudoku or hari kari because you can't get alignment from killing monsters and will thus be unable to do the quest.",
	"Don't play evil Nethack variants - you will die in screwhack because SOMEONE made it so that zapping a monster with tele does no longer work on a no-teleport level. Fuck this shit. SLASH'EM Extended certainly won't implement no bullshit wand destruction patches except maybe as a joke mode.",
	"Someone trashed the samurai in Nethack Fourk on purpose, because said someone never plays the samurai himself and therefore never noticed how godawfully weak they've become.",
	"When I (Amy) told my roommate about my death in Nethack Fourk during 2017's Junethack, she said that FIQ and jonadab shall forever be cursed.",
	"There is something completely retarded called wand 'balance' patch (should be renamed to wand destruction patch). Words can't even begin to describe that crap.",
	"The wand balance patch is apparently based on an essay written by SATAN, and the variant developers who implemented it are his vrock and balrog minions.",
	"Konnichi wa Erotic Thai Bitch, welcome to FIQHack! You are a lawful female human Samurai. The game will crash whenever you fumble around with your bag of holding because the nethack4 system is teh crash0r.",
	"In FIQhack, meleeing zombies or just standing next to them will zombify you without any specific message. Apparently they can passively spread airborne contagions or something.",
	"If a mordor orc reads stinking cloud in FIQhack's Minetown, the watch captain will become hostile to YOU, and kill you.",
	"All the stupid satanic nethack4 variants make it so that entering a new dungeon level gives turns for all the monsters on it, which was not the case in vanilla and can be deadly depending on what the monsters are.",
	"Dynahack inexplicably allows demon lords and princes to wake up spontaneously, and only God knows why. At least there's no guaranteed Demogorgon lair though!",
	"The save file is corrupted... The gamestate or save file is internally inconsistent. However, the game can be recovered from a backup, and then Nethack4 will crash again in exactly the same place. Genius.",
	"You hear a joshu pronouncing the formula on a scroll of genocide! Wiped out all samurai. Do you want your possessions identified? DYWYPI?",
	"FIQ should just accept the fact that letting monsters use all items in the game is a stupid idea! He should cut his losses and make a sane variant instead, where they don't randomly read genocide and wipe you out!",
	"In FIQhack you can name your character 'Erotic Thai Bitch', but certain other satanhack-based variants don't allow it because the 'genius' developers implemented sloppy fixes just to taunt the players. FIQhack confirmed better than Nethack4.",
	"The RNG decides to send in reinforcements to punish people who kill innocent monsters.",
	"Did you know that the elder priest in dnethack is an absolute MOTHER FUCKER? No single monster should be that strong, not even close, especially if it's a mandatory fight.",
	"Did you know that the elder priest in dnethack is an absolute MOTHER FUCKER? Chris must really hate all living beings because holy HELL is that thing's attack unbalanced or what. And there is zero defense against it.",
	"Did you know that the elder priest in dnethack is an absolute MOTHER FUCKER? As if the asshole tentacle attack isn't bad enough, he gave it a flag that gives it ZERO SPELLCASTING COOLDOWN TIME IN A VARIANT WHERE MONSTER SPELLCASTING IS ALREADY OVERPOWERED AS HECK.",
	"Did you know that the elder priest in dnethack is an absolute MOTHER FUCKER? Whoever invented that asshole tentacle attack should have its own real-life tentacle removed in a most painful way.",
	"Do not play dnethack. You will make it to the sanctum and basically step on everything along the way, except for Dagon (who is really unbalanced), but the real showstopper comes when you try to claim the amulet and find out the monster holding it has a 'fook you player, game over, all your equipment, stats and everything are suddenly meaningless, you die thank you come again' attack.",
	"Did you know that dnethack is the best variant if you don't consider slex? It has high heels!!!",
	"Did you know that there are high heels in dnethack too? Shame Chris didn't add a possibility to use the stiletto heels to sever the asshole elder priest's asshole tentacles and stomp them to a bloody pulp.",

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

const char *
soviettaunt()
{
	return (soviettaunts[rn2(SIZE(soviettaunts))]);
}

/*pager.c*/
