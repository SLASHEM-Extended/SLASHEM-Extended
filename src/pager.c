/*	SCCS Id: @(#)pager.c	3.4	2003/08/13	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* This file contains the command routines dowhatis() and dohelp() and */
/* a few other help related facilities */

#include "hack.h"
#include "dlb.h"

STATIC_DCL boolean is_swallow_sym(int);
STATIC_DCL int append_str(char *, const char *);
STATIC_DCL struct permonst * lookat(int, int, char *, char *);
STATIC_DCL void checkfile(char *,struct permonst *,BOOLEAN_P,BOOLEAN_P);
STATIC_DCL int do_look(BOOLEAN_P);
STATIC_DCL boolean help_menu(int *);
#ifdef PORT_HELP
extern void port_help(void);
#endif
#ifdef EXTENDED_INFO
STATIC_DCL char * get_description_of_monster_type(struct permonst *, char *);
STATIC_DCL char * get_generation_description_of_monster_type(struct permonst *, char *);
STATIC_DCL char * get_resistance_description_of_monster_type(struct permonst *, char *);
STATIC_DCL char * get_flag_description_of_monster_type(struct permonst *, char *);
STATIC_DCL char * get_speed_description_of_monster_type(struct permonst *, char *);
STATIC_DCL int generate_list_of_resistances(char *, /*uchar*/unsigned long);
STATIC_DCL void append_newline_to_pline_string(char * buf);
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
	    sprintf(race, "%s ", urace.adj);
	}

	sprintf(buf, "%s%s%s called %s",
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
	    sprintf(race, "%s ", urace.adj);
	    sprintf(role, "%s ", urole.name);
	} else sprintf(race, "%s ", (!missingnoprotect || !Upolyd) ? mons[u.umonnum].mname : "undefined creature");

	sprintf(buf, "%s%s%s called %s",
		Invis ? "invisible " : "",
		race,
		role,
		plname);
#endif


	if (u.usteed) {
	    char steedbuf[BUFSZ];

	    sprintf(steedbuf, ", mounted on %s", y_monnam(u.usteed));
	    /* assert((sizeof buf >= strlen(buf)+strlen(steedbuf)+1); */
	    strcat(buf, steedbuf);
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
		sprintf(eos(buf), " [seen: %s%s%s%s%s]",
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
	sprintf(buf, "interior of %s",
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
	    sprintf(buf, "%s%s%s",
		    (mtmp->mx != x || mtmp->my != y) ?
			((mtmp->isshk && accurate)
				? "tail of " : "tail of a ") : "",
		    (mtmp->mfrenzied && accurate) ? "frenzied " :
		    (mtmp->mtame && accurate) ? "tame " :
		    (mtmp->mpeaceful && accurate) ? "peaceful " : "",
		    name);
	    if (Hallu_party && Hallucination)
		    sprintf(eos(buf), ", real color: %s", (mtmp->data->mcolor == CLR_BLACK) ? "black" : (mtmp->data->mcolor == CLR_RED) ? "red" : (mtmp->data->mcolor == CLR_GREEN) ? "green" : (mtmp->data->mcolor == CLR_BROWN) ? "brown" : (mtmp->data->mcolor == CLR_BLUE) ? "blue" : (mtmp->data->mcolor == CLR_MAGENTA) ? "magenta" : (mtmp->data->mcolor == CLR_CYAN) ? "cyan" : (mtmp->data->mcolor == CLR_GRAY) ? "gray" : (mtmp->data->mcolor == NO_COLOR) ? "colorless" : (mtmp->data->mcolor == CLR_ORANGE) ? "orange" : (mtmp->data->mcolor == CLR_BRIGHT_GREEN) ? "bright green" : (mtmp->data->mcolor == CLR_YELLOW) ? "yellow" : (mtmp->data->mcolor == CLR_BRIGHT_BLUE) ? "bright blue" : (mtmp->data->mcolor == CLR_BRIGHT_CYAN) ? "bright cyan" : (mtmp->data->mcolor == CLR_BRIGHT_MAGENTA) ? "bright magenta" : (mtmp->data->mcolor == CLR_WHITE) ? "white" : (mtmp->data->mcolor == CLR_MAX) ? "max" : "unknown" );

	    if (mon_wounds(mtmp)) { 
		strcat(buf, ", "); 
		strcat(buf, mon_wounds(mtmp)); 
	    }
	    if (u.ustuck == mtmp)
		strcat(buf, (Upolyd && sticks(youmonst.data)) ?
			", being held" : ", holding you");
	    if (mtmp->mleashed)
		strcat(buf, ", leashed to you");

	    if (mtmp->mtrapped && cansee(mtmp->mx, mtmp->my)) {
		struct trap *t = t_at(mtmp->mx, mtmp->my);
		int tt = t ? t->ttyp : NO_TRAP;

		/* newsym lets you know of the trap, so mention it here */
		if (tt == BEAR_TRAP || tt == PIT || tt == SHIT_PIT || tt == MANA_PIT || tt == ANOXIC_PIT ||
			tt == SPIKED_PIT || tt == GIANT_CHASM || tt == WEB)
		    sprintf(eos(buf), ", trapped in %s",
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
			strcat(monbuf, "normal vision");
			/* can't actually be 1 yet here */
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (useemon && mtmp->minvis) {
			strcat(monbuf, "see invisible");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if ((!mtmp->minvis || See_invisible) && !mtmp->minvisreal &&
			    see_with_infrared(mtmp)) {
			strcat(monbuf, "infravision");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (tp_sensemon(mtmp)) {
			strcat(monbuf, "telepathy");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (useemon && xraydist > 0 &&
			    distu(mtmp->mx, mtmp->my) <= xraydist) {
			/* Eyes of the Overworld */
			strcat(monbuf, "astral vision");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (Detect_monsters) {
			strcat(monbuf, "monster detection");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if ( (Role_if(PM_ACTIVISTOR) || Race_if(PM_PEACEMAKER) ) && mtmp->data == &mons[PM_TOPMODEL] ) {
			strcat(monbuf, "warned of topmodels");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (Role_if(PM_ACTIVISTOR) && type_is_pname(mtmp->data) && uwep && is_quest_artifact(uwep) ) {
			strcat(monbuf, "warned of unique monsters");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (uamul && uamul->otyp == AMULET_OF_POISON_WARNING && poisonous(mtmp->data)) {
			strcat(monbuf, "warned of poisonous monsters");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (uamul && uamul->otyp == AMULET_OF_OWN_RACE_WARNING && your_race(mtmp->data)) {
			strcat(monbuf, "warned of monsters that are the same race as you");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (uamul && uamul->otyp == AMULET_OF_COVETOUS_WARNING && (is_covetous(mtmp->data) || mtmp->egotype_covetous) ) {
			strcat(monbuf, "warned of covetous monsters");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (ublindf && ublindf->otyp == BOSS_VISOR && (is_covetous(mtmp->data) || mtmp->egotype_covetous) ) {
			strcat(monbuf, "warned of covetous monsters");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (Role_if(PM_PALADIN) && is_demon(mtmp->data)) {
			strcat(monbuf, "warned of demons");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (uarmc && uarmc->oartifact == ART_DEMONIC_UNDEAD_RADAR && is_demon(mtmp->data)) {
			strcat(monbuf, "warned of demons");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (isselfhybrid && strongmonst(mtmp->data) && is_wanderer(mtmp->data) ) {
			strcat(monbuf, "self-hybridization");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (isselfhybrid && monpolyok(mtmp->data) && !polyok(mtmp->data) && ((mtmp->data->mlevel < 30) || ((mtmp)->mhp % 2 != 0) )) {
			strcat(monbuf, "self-hybridization");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (Race_if(PM_VORTEX) && unsolid(mtmp->data)) {
			strcat(monbuf, "warned of unsolid creatures");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (Race_if(PM_VORTEX) && nolimbs(mtmp->data)) {
			strcat(monbuf, "warned of creatures without limbs");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (Race_if(PM_CORTEX) && unsolid(mtmp->data)) {
			strcat(monbuf, "warned of unsolid creatures");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (Race_if(PM_CORTEX) && nolimbs(mtmp->data)) {
			strcat(monbuf, "warned of creatures without limbs");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (Race_if(PM_LEVITATOR) && (is_flyer(mtmp->data) || mtmp->egotype_flying) ) {
			strcat(monbuf, "warned of flying monsters");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (Race_if(PM_RODNEYAN) && mon_has_amulet(mtmp) ) {
			strcat(monbuf, "amulet of yendor");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (Race_if(PM_RODNEYAN) && mon_has_special(mtmp) ) {
			strcat(monbuf, "covetous");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }

		    if (Stunnopathy && Stunned && always_hostile(mtmp->data) && (mtmp)->mhp % 4 != 0) {
			strcat(monbuf, "stunnopathy");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if ( (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmh->otyp]), "internet helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "vsemirnaya pautina shlem") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "keng dunyo veb-zarbdan") ) ) && (mtmp)->mhp % 9 == 0) {
			strcat(monbuf, "internet access");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (uarmh && uarmh->oartifact == ART_WEB_RADIO && (mtmp)->mhp % 9 == 0) {
			strcat(monbuf, "internet access");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (RngeInternetAccess && (mtmp)->mhp % 9 == 0) {
			strcat(monbuf, "internet access");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (Numbopathy && Numbed && (avoid_player(mtmp->data) || mtmp->egotype_avoider) ) {
			strcat(monbuf, "numbopathy");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (Freezopathy && Frozen && mtmp->data->mcolor == CLR_WHITE ) {
			strcat(monbuf, "freezopathy");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (uarmf && uarmf->oartifact == ART_VERA_S_FREEZER && mtmp->data->mcolor == CLR_WHITE ) {
			strcat(monbuf, "freezer vision");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (Burnopathy && Burned && infravision(mtmp->data) ) {
			strcat(monbuf, "burnopathy");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (Dimmopathy && Dimmed && mtmp->m_lev > u.ulevel ) {
			strcat(monbuf, "dimmopathy");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (Sickopathy && Sick && extra_nasty(mtmp->data) ) {
			strcat(monbuf, "sickopathy");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (uwep && uwep->oartifact == ART_TIGATOR_S_THORN && is_pokemon(mtmp->data) ) {
			strcat(monbuf, "pokemon vision");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (ublindf && ublindf->oartifact == ART_BREATHER_SHOW && attacktype(mtmp->data, AT_BREA)) {
			strcat(monbuf, "breather show");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (uarmc && uarmc->oartifact == ART_POKEWALKER && is_pokemon(mtmp->data) ) {
			strcat(monbuf, "pokemon vision");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (uarmc && uarmc->oartifact == ART_BUGNOSE && (mtmp->data->mlet == S_ANT || mtmp->data->mlet == S_XAN) ) {
			strcat(monbuf, "bugnose");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (uarmf && uarmf->oartifact == ART_FD_DETH && (mtmp->data->mlet == S_DOG || mtmp->data->mlet == S_FELINE) ) {
			strcat(monbuf, "FD Deth");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }
		    if (uarmg && uarmg->oartifact == ART_WHAT_S_UP_BITCHES && (mtmp->data->mlet == S_NYMPH) ) {
			strcat(monbuf, "bitchvision");
			if (ways_seen-- > 1) strcat(monbuf, ", ");
		    }

		    if (MATCH_WARN_OF_MON(mtmp) /*|| (uamul && uamul->otyp == AMULET_OF_UNDEAD_WARNING && (is_undead(mtmp->data) || mtmp->egotype_undead) ) || (uarmh && uarmh->otyp == HELMET_OF_UNDEAD_WARNING && (is_undead(mtmp->data) || mtmp->egotype_undead) )*/) {
		    	char wbuf[BUFSZ];
			if (Hallucination)
				strcat(monbuf, "paranoid delusion");
			else {
				sprintf(wbuf, "warned of %s",
					makeplural(mtmp->data->mname));
		    		strcat(monbuf, wbuf);
		    	}
		    	if (ways_seen-- > 1) strcat(monbuf, ", ");
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
			strcpy(buf, distant_name(otmp, xname));
			dealloc_obj(otmp);
		} else strcpy(buf, "very weird object");
	    }
	} else
	    strcpy(buf, distant_name(otmp, xname));

	if (IS_TREE(levl[x][y].typ))
	    strcat(buf, " stuck in a tree"); 
	else if (IS_IRONBAR(levl[x][y].typ))
	    strcat(buf, " stuck in iron bars"); 
	else if (IS_GRAVEWALL(levl[x][y].typ))
	    strcat(buf, " embedded in a grave wall"); 
	else if (IS_TUNNELWALL(levl[x][y].typ))
	    strcat(buf, " lying in a tunnel"); 
	else if (IS_FARMLAND(levl[x][y].typ))
	    strcat(buf, " on a farmland");
	else if (IS_MOUNTAIN(levl[x][y].typ))
	    strcat(buf, " atop a mountain"); 
	else if (levl[x][y].typ == STONE || levl[x][y].typ == SCORR)
	    strcat(buf, " embedded in stone");
	else if (IS_WALL(levl[x][y].typ) || levl[x][y].typ == SDOOR)
	    strcat(buf, " embedded in a wall");
	else if (closed_door(x,y))
	    strcat(buf, " embedded in a door");
	else if (is_waterypool(x,y))
	    strcat(buf, " in water");
	else if (is_styxriver(x,y))
	    strcat(buf, " in a styx river");
	else if (is_watertunnel(x,y))
	    strcat(buf, " in a water tunnel");
	else if (is_lava(x,y))
	    strcat(buf, " in molten lava");	/* [can this ever happen?] */
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

	strcpy(buf, defsyms[trap_to_defsym(tnum)].explanation);
    } else if(!glyph_is_cmap(glyph)) {
	strcpy(buf,"unexplored area");
    } else if( glyph == (GLYPH_CMAP_OFF + 44)) {
	strcpy(buf,"grayout");
    } else switch(glyph_to_cmap(glyph)) {
    case S_altar:
	if(!In_endgame(&u.uz))
	    sprintf(buf, "%s altar",
		align_str(Amask2align(levl[x][y].altarmask & ~AM_SHRINE)));
	else sprintf(buf, "aligned altar");
	break;
    case S_ndoor:
	if (is_drawbridge_wall(x, y) >= 0)
	    strcpy(buf,"open drawbridge portcullis");
	else if ((levl[x][y].doormask & ~D_TRAPPED) == D_BROKEN)
	    strcpy(buf,"broken door");
	else
	    strcpy(buf,"doorway");
	break;
    case S_cloud:
	strcpy(buf, Is_airlevel(&u.uz) ? "cloudy area" : "fog/vapor cloud");
	break;
    case S_water:
    case S_pool:
	strcpy(buf, level.flags.lethe? "sparkling water" : "water");
	break;
    case S_well:
	strcpy(buf, level.flags.lethe? "sparkling well" : "well");
	break;
    case S_poisonedwell:
	strcpy(buf, level.flags.lethe? "sparkling poisoned well" : "poisoned well");
	break;
    case S_crystalwater:
	strcpy(buf, level.flags.lethe? "sparkling crystal water" : "crystal water");
	break;
    case S_raincloud:
	strcpy(buf, level.flags.lethe? "sparkling rain cloud" : "rain cloud");
	break;
    case S_watertunnel:
	strcpy(buf, level.flags.lethe? "sparkling water tunnel" : "water tunnel");
	break;
    case S_fountain:
	strcpy(buf, level.flags.lethe? "sparkling fountain" : "fountain");
	break;
    default:
	strcpy(buf,defsyms[glyph_to_cmap(glyph)].explanation);
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
		if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
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
		    sprintf(out_str, "%c       %s", sym, an(monexplain[i]));
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
		sprintf(out_str, "%c       %s", sym, mon_interior);
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
		    sprintf(out_str, "%c       %s", sym, an(objexplain[i]));
		    firstmatch = objexplain[i];
		    found++;
		} else {
		    found += append_str(out_str, an(objexplain[i]));
		}
	    }
	}

	if (sym == DEF_INVISIBLE) {
	    if (!found) {
		sprintf(out_str, "%c       %s", sym, an(invisexplain));
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
			sprintf(out_str, "%c       a trap", sym);
			hit_trap = TRUE;
		    } else if (level.flags.lethe && !strcmp(x_str, "water")) {
			sprintf(out_str, "%c       sparkling water", sym);
		    } else {
			sprintf(out_str, "%c       %s", sym,
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
			sprintf(out_str, "%c       %s",
				sym, def_warnsyms[i].explanation);
			firstmatch = def_warnsyms[i].explanation;
			found++;
		} else {
			found += append_str(out_str, def_warnsyms[i].explanation);
		}
		/* Kludge: warning trumps boulders on the display.
		   Reveal the boulder too or player can get confused */
		if (from_screen && sobj_at(BOULDER, cc.x, cc.y))
			strcat(out_str, " co-located with a boulder");
		break;	/* out of for loop*/
	    }
	}
    
	/* if we ignored venom and list turned out to be short, put it back */
	if (skipped_venom && found < 2) {
	    x_str = objexplain[VENOM_CLASS];
	    if (!found) {
		sprintf(out_str, "%c       %s", sym, an(x_str));
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
		sprintf(out_str, "%c       %s", sym, an(firstmatch));
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
		    sprintf(temp_buf, " (%s)", firstmatch);
		    (void)strncat(out_str, temp_buf, BUFSZ-strlen(out_str)-1);
		    found = 1;	/* we have something to look up */
		}

		/* mainly for batman and ladiesman: print info about monster's gender --Amy */

		if (pm) {
		    struct monst *mtmpX = m_at(cc.x, cc.y);
		    if (mtmpX) {
			sprintf(temp_buf, " (base level %d)", mtmpX->data->mlevel);
			(void)strncat(out_str, temp_buf, BUFSZ-strlen(out_str)-1);

		    }
		    if (mtmpX && humanoid(mtmpX->data)) {
			sprintf(temp_buf, " (%s)", mtmpX->female ? "female" : "male");
			(void)strncat(out_str, temp_buf, BUFSZ-strlen(out_str)-1);

		    }
		}

		if (monbuf[0]) {
		    sprintf(temp_buf, " [seen: %s]", monbuf);
		    (void)strncat(out_str, temp_buf, BUFSZ-strlen(out_str)-1);
		}
#ifdef WIZARD
		if (wizard && pm) {
		    struct monst *mtmp = m_at(cc.x, cc.y);
		    if (mtmp && mtmp->oldmonnm != monsndx(pm)) {
			sprintf(temp_buf, " [polymorphed from a %s]",
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
		strcpy(temp_buf, level.flags.lethe 
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
		sprintf(frequency, ", with frequency %d.", (ptr->geno & G_FREQ));
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
	if (ptr->mflags5 & M5_JONADAB) {
		strcat(description, " Origin: jonadab's random monster generator script.");
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

		case AD_ALIN: return "reduce alignment record";
		case AD_SIN: return "increase sin counter";
		case AD_MINA: return "identity-specific nastiness";
		case AD_AGGR: return "aggravate monster";
		case AD_CONT: return "contamination";

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
		strcpy(cbuf, buf);
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
		sprintf(helpbuf, help_menu_items[i], PORT_ID);
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
	    free((void *)selected);
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
	"Captcha! Please type in the following word(s) to continue: 3loekjepue2'IP2wjl215ERplqvm&",
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
	"Wanna know what it means when the elder priest tentacles to tentacle you? It means that you just pushed AmyBSOD's berserk button. RAAAAAAAAAAAH! RAGE!",
	"Cannot find thread.vbx.",
	"Incoming message for Player 1. The message is from 'Bea'. It reads: 'Yes, I trampled your girlfriend to death with my black combat boots. It was especially fun to repeatedly stomp her defenseless body using my fleecy block heels.'",
	"Incoming message for Player 1. The message is from 'Juen'. It reads: 'My leather peep-toes are very gentle-soft and beautifully display my sweet feet with their painted toenails!'",
	"Incoming message for Player 1. The message is from 'Tobi'. It reads: 'I will infect the sluts with syphilis using Bandarchor, which is a new cryptovirus from India. When that Jane slut finally realizes what is going on, her entire PC will already be encrypted. HAR HAR HAR HAR HAR HAR HAR!'",
	"Incoming message for Player 1. The message is from 'Jane'. It reads: 'You think my boring dancing shoes and dark blue socks look cute?'",
	"Incoming message for Player 1. The message is from 'Eva'. It reads: 'The combination of orange mocassins with red socks makes my feet look very innocent.'",
	"Incoming message for Player 1. The message is from 'Sarah'. It reads: 'The rules said that if I win the game against you, I may repeatedly kick your shins with my dark blue hugging boots. But our match ended in a draw, and therefore I simply decide that I will kick you anyway. Hold still and receive your well-earned punishment.'",
	"Incoming message for Player 1. The message is from 'Dr. Delilah'. It reads: 'I heard you were molesting little girls in the bus, groping their buttocks. And I decided to punish you for that by displaying you on the public pillory for three hours each day, and all the girls that you molested will be able to kick you in the nuts whenever they want to.'",
	"Incoming message for Player 1. The message is from 'Gudrun'. It reads: 'You cannot walk in high heels, right? Do you want to be a model or not?'",
	"Incoming message for Player 1. The message is from 'Kati'. It reads: 'Yesterday I ran all the way from school to my home in Rhine Refuge (10 kilometers), now I'm ready to run a half marathon!'",
	"Incoming message for Player 1. The message is from 'Max the Badass'. It reads: 'Male girls won't win anyway!'",
	"Incoming message for Player 1. The message is from 'Anton'. It reads: 'You cock, you're gonna be stupid and run into a wall!'",
	"Incoming message for Player 1. The message is from 'Verena'. It reads: 'I will gladly duel you. My weapon of choice is a pair of stiletto sandals with which I'm going to whack you over the head repeatedly.'",
	"Incoming message for Player 1. The message is from 'Karin'. It reads: 'Hmm. Amazing. You're actually not as stupid as you look.'",
	"Incoming message for Player 1. The message is from 'Kerstin'. It reads: 'University is not like school. You really have to work hard and constantly learn for the exams. If you start learning one day before the exam, you will fail hard.'",
	"Incoming message for Player 1. The message is from 'Verena'. It reads: 'Oh, sorry! I didn't mean to step on your hand with my stiletto heel. Does it hurt? Shall I massage your fingers a bit?'",
	"Incoming message for Player 1. The message is from 'Nadja'. It reads: 'Frongnir will hopefully cure you of your craze by stealing all your money repeatedly.'",
	"Incoming message for Player 1. The message is from 'Karin'. It reads: 'Sorry, I just had to knee you in the nuts repeatedly while you slept. I just couldn't resist!'",
	"Incoming message for Player 1. The message is from 'Sandra'. It reads: 'Well, you've got quite the scratches on your legs.'",
	"Incoming message for Player 1. The message is from 'Arabella'. It reads: 'Wo bu hui bangzhu de ren xihuan ni, yinwei ni shi cong cuowu de guojia.'",
	"Incoming message for Player 1. The message is from 'Gallardo'. It reads: 'Huh, since when does Arabella own a mansion?'",
	"Incoming message for Player 1. The message is from 'Chaska'. It reads: 'No I cannot tell you who can teach French to you. Go away and don't disturb me no more!'",
	"Incoming message for Player 1. The message is from 'Annie'. It reads: 'Si vous ne pouvez pas parler francais, vous n'etes pas vraiment ici! Vous devez connaitre la langue, si vous voulez continuer a prendre part!'",
	"Incoming message for Player 1. The message is from 'Elif'. It reads: 'My bite can give you a lot of power by transforming you into a creature of the eternal darkness. Yes, I am a vampire, and I want to make you into one as well.'",
	"Incoming message for Player 1. The message is from 'Elif'. It reads: 'Sorry, but my sharp fingernails absolutely want to paint on your skin by opening bloody wounds.'",
	"Incoming message for Player 1. The message is from 'Elif'. It reads: 'I'm not a fan of sandals. Usually I always wear sneakers.'",
	"Incoming message for Player 1. The message is from 'Simone'. It reads: 'So you tried to open and take off my shoes without me noticing? Well, as long as you don't do it again, I will forgive you.'",
	"Incoming message for Player 1. The message is from 'Sabrina'. It reads: 'If your hand tries to fool around with my red block-heeled sandals, I will place my sweaty feet on top of it and crush your fingers.'",
	"Incoming message for Player 1. The message is from 'Anne'. It reads: 'I'll talk to you sexily while squeezing your unprotected fingers in my sandals using my sweaty feet.'",
	"Incoming message for Player 1. The message is from 'Ruth'. It reads: 'My tight sneakers cause my feet and socks to become really sweaty. If I take them off, you might fall over unconscious. Want me to do it?'",
	"Incoming message for Player 1. The message is from 'Julia'. It reads: 'What? Urea is supposedly the stuff that our pee is made out of? Eep!'",
	"Incoming message for Player 1. The message is from 'Danielle'. It reads: 'Your hands stand no chance against my dark green lady shoes. And I don't feel bad about making you bleed either. After all, you could just have kept your hands off my feet!'",
	"Incoming message for Player 1. The message is from 'Josefine'. It reads: 'It's really fun scratching your legs with my velcro sneakers' lashes while you're smelling my bare feet!'",
	"Incoming message for Player 1. The message is from 'Jil'. It reads: 'Enjoy inhaling the fragrance of my worn socks.'",
	"Incoming message for Player 1. The message is from 'Katrin'. It reads: 'Yes, I wear socks to my stiletto heels. I also like to use my worn socks as a weapon, since I discovered that the smell can make my opponents feel uncomfortable.'",
	"Incoming message for Player 1. The message is from 'Solvejg'. It reads: 'My orange mocassins emit a beguiling odor that is so strong, it can make you fall over unconscious!'",
	"Incoming message for Player 1. The message is from 'Thai'. It reads: 'I'm an erotic asian bitch!'",
	"Incoming message for Player 1. The message is from 'Mailie'. It reads: 'My pink girl sneakers emit a very powerful pink smell that can clear out entire buildings and cause armies of well-trained men to fall over unconscious.'",
	"Incoming message for Player 1. The message is from 'Heike'. It reads: 'I impregnated my hugging boots with hydrofluoric acid. Whenever I kick you, it will cause very painful burns that take a long time to heal.'",
	"Incoming message for Player 1. The message is from 'Katharina'. It reads: 'What, you're telling me to take off my socks since they supposedly stink so much? Okay, but I will then bind them to your nose so you can continue inhaling their smell.'",
	"Incoming message for Player 1. The message is from 'Nora'. It reads: 'You... found my red socks? And you... kept them with you until now? God, I really hope you didn't do... things with them... anyway, give them back to me now!'",
	"Incoming message for Player 1. The message is from 'Petra'. It reads: 'I know kickboxing.'",
	"You hear distant waves.",
	"You hear distant surf.",
	"You hear the distant sea.",
	"You hear the call of the ocean.",
	"You hear waves against the shore.",
	"You hear flowing water.",
	"You hear the sighing of waves.",
	"You hear quarrelling gulls.",
	"You hear the song of the deep.",
	"You hear rumbling in the deeps.",
	"You hear the singing of Eidothea.",
	"You hear the laughter of the protean nymphs.",
	"You hear rushing tides.",
	"You hear the elusive sea change.",
	"You hear the silence of the sea.",
	"You hear the passage of the albatross.",
	"You hear dancing raindrops.",
	"You hear coins rolling on the seabed.",
	"You hear treasure galleons crumbling in the depths.",
	"You hear waves lapping against a hull."
	"You hear the laughter of the nymph that just stole your artifact and teleported off the level.",
	"You hear the laughter of AmyBSOD who watches your game and constantly giggles while you struggle futilely against stiff odds.",
	"You experience a sync conflict.",
	"You realize that Amy forgot to merge a critical change and therefore the game is now unwinnable.",
	"The last monster you killed was absolutely vital for your ascension - you would have had to talk to it while holding the amulet. But now that it's dead, your game is unwinnable.",
	"The weresexyleatherpump summons a pair of sexy leather pumps!",
	"You start swooning over AmyBSOD's sweaty feet.",
	"The wereblockheeledcombatboot kicks you in the ass, which feels very fleecy-soft.",
	"The werehuggingboot places her treaded soles on your unprotected feet, causing wonderfully soothing pain.",
	"You hear 'bundlebundlebundle!!'",
	"You hear 'cuddlecuddlecuddle!!'",
	"You hear 'fleecelfleecelfleecel!!'",
	"WEOWEOWEOWEOWEOWEOWEO",
	"You are being chased by the kops.",
	"The Book of the Dead burns up!",
	"You let a woodchuck die on the oracle level, which causes the portal on the Plane of Fire to not exist so you can never win the game. And 2000 turns after you enter the Plane of Fire, an archonstorm will happen.",
	"You hear a dramatic sound that tells of imminent danger!",
	"You hear an innocuous tune that seems to decrease in pitch...",
	"You hear a terrible melody, and realize that there is no escape!",
	"You hear a wavy, repeating sound!",
	"You hear a deep metallic tune.",
	"You hear a standard combat jingle, except in the end it somehow sounds nonstandard...",
	"You hear a frantic melody that quickly rises and falls in pitch!",
	"You hear an extremely high-pitch jingle and instantly know that something completely abnormal must have happened!",
	"Your intrinsics change.",
	"Your intrinsics have been deactivated. All of them.",
	"You permanently lost all your intrinsics and have to get them back the hard way!",
	"You feel monsters setting up portals.",
	"You feel unable to change form! Now your doppelganger lost polymorph control! HA HA HA! Will you be a megan00b and ragequit over that, or are you going to do the sensible thing and equip a ring of polymorph control?",
	"You feel very hungry.",
	"You feel very hungry. Maybe you should visit a McDonalds.",
	"You feel very hungry. I've heard there's a Burger King just around the corner.",
	"You feel very hungry. Let me get you a Doenerteller Versace!",
	"You feel very hungry. What you need is some food down ya throat, I'd suggest shawarma today!",
	"You feel very hungry. You need BDSM. Which stands for Burger King, Doenerteller Versace, Shawarma and McDonalds.",
	"You feel like a rabble-rouser.",
	"You feel like a rabble-rouser. Now you acquired intrinsic conflict, which means that you burn nutrition like mad and shopkeepers will shoot you on sight with their blunderbuss.",
	"You feel constipated.",
	"You feel very forgetful!",
	"You acquire intrinsic map amnesia!",
	"Woohoo! Woohoo! WoohooWoohooWoohoo WoohooWoohooWoohooWoohooWoohoo!",
	"Suddenly you're exposed to the contamination.",
	"You start sinking in the shifting sand. This will quickly kill you, better try to get out ASAP.",
	"You are struggling against the shifting sand, but you're almost completely submerged now. Pull yourself out immediately or you'll end up in a sandy grave.",
	"You are caught in a sandstorm, and the sand gets in your eye!",
	"You are caught in a snowstorm!",
	"Continued exposure to the Styx River will cause contamination.",
	"The wagon burns you!",
	"Swimming in moorland causes continuous damage.",
	"Sparkling rain washes over you.",
	"Sparkling rain washes over you. Your bag of holding turns into a sack! Your shield of disintegration resistance turns into a small shield!",
	"Ulch - your divine body is tainted by that filthy yellow liquid!",
	"The yellow liquid actually feels comfortable on your skin.",
	"The yellow liquid tickles your skin.",
	"The yellow liquid severely corrodes your unprotected skin!",
	"The stalactite pierces you!",
	"Walking on paved floor makes lots of noise.",
	"Since you're not proficient at walking in high heels, you sprain your leg very painfully and crash into the floor.",
	"Since you're not proficient at walking in high heels, you sprain your leg very painfully and crash into the floor. You stupid woman, why don't you wear flats if you're incapable of moving around in heels?",
	"The Grim Reaper is waiting to take you away.",
	"The RNG decides to curse-weld an item to you.",
	"e - the heavily cursed blinking cloak named Disbelieving Powerlord (being worn).",
	"Your body suddenly becomes all stiff!",
	"Body of you was stiff like a stone.",
	"The yerles conbat plane throws a frag grenade.",
	"The yerles conbat plane curses that Bloodyshade entity for 'fixing' the 'spelling error'.",
	"The curased coffin mutters a curse. Your blessed greased rustproof +7 Excalibur glows with a black aura.",
	"The curased coffin mutters a curse. Bloodyshade's blessed greased rustproof +7 Excalibur glows with a black aura as a punishment for 'fixing' the 'spelling error'.",
	"The dungeon is getting more chaotic!",
	"The pentagram fades away completely.",
	"Climbing the table does not work while you're burdened.",
	"Jana pulls you into the urine lake!",
	"The racquet pulls you into the moorland!",
	"The cube pulls you into the shifting sand!",
	"The erinys pulls you into the styx river!",
	"The adverse conditions in the moorland hurt your health!",
	"You are pulled below the surface and suffocate.",
	"You're exposed to the styx river, and your contamination greatly increases.",
	"The nurse poisons the well!",
	"The zombie cures the mind flayer corpse of deadness!",
	"Whoops, you forgot that the well contains lethe water.",
	"You also forgot whether lethe water can cause amnesia.",
	"Suddenly the well becomes poisoned...",
	"You draw water from a well. An euryale crawls out of the well! The euryale gazes you. Body of you was stiff like a stone.",
	"It seems you drank too much coffee and therefore cannot sleep.",
	"You can't seem to fall asleep.",
	"You go to bed.",
	"You fly into the crystal water and drown.",
	"You hear a flaming sound.",
	"The ash floor melts and turns into lava.",
	"The ash floor melts and turns into lava. You fall into the lava! You burn to a crisp...",
	"You hear a burning sound.",
	"The farmland burns up!",
	"The farmland burns up! And your farm was on it!",
	"The farmland burns up! (At least buying the farm should be less expensive now.)",
	"The grass burns up!",
	"The snow melts away.",
	"The wagon bursts into flames!",
	"The light ray completely destroys the burning wagon.",
	"As the psybeam hits the burning wagon, it mysteriously transforms into a rain cloud!",
	"Remember, if you get a stream of obj_is_local error messages, SAVE THE GAME RIGHT AWAY. Don't continue playing if that bug strikes.",
	"This is a fake message. You can recognize real error messages since they have 'Program in disorder - please tell Amy about it' appended after them.",
	"You enter a nasty central!",
	"You forgot to bring your light source and can't see anything in this room.",
	"You got tons of trouble, baby!",
	"You see people with long pointy sticks who want to impale you!",
	"Your body warps strangely and you cease to exist... Do you want your possessions identified? [ynq] (n) _",
	"As you enter the heavily guarded army base, bullets start flying in your direction! TAKE COVER!",
	"You were fated to die here. DIE!",
	"You feel that you will meet your fate here.",
	"You disturbed some old wizened fool that lives here, and now you have to kill that stupid git!",
	"You disturbed some old wizened fool that lives here, and now he will kick your butt, you stupid git!",
	"You realize that the game has turned into DoomRL! Quick, ready your kalashnikov and BFG!",
	"Go directly to jail. Do not pass go. Do not collect 200 zorkmids.",
	"Go directly to jail. Do not pass go. Do not collect 200 zorkmids. Do you want your possessions identified? [ynq] (n) _",
	"The game suddenly turned into dnethack. The elder priest tentacles to tentacle you! Your cloak of magic resistance disintegrates!",
	"You smell rotting flesh.",
	"You hear shuffling noises.",
	"You hear a loud moan.",
	"You feel that you're near death!",
	"You don't have much longer to live...",
	"You hear a zombie shout 'BRAAAAAAAAAAINS!'",
	"You smell trouble.",
	"You feel that something's gonna happen.",
	"You feel that nothing is gonna happen.",
	"You sense several evil presences.",
	"You have to count to three, and if you fail, you can forget about your game!",
	"You are way in over your head!",
	"You hear metallic clashes.",
	"You are listening to someone practicing their sword skill.",
	"You hear a trumpet, followed by attack orders.",
	"You are ambushed by shadowy figures with gleaming swords!",
	"You should impale yourself on your weapon while you still can...",
	"You smell brimstone.",
	"You hear the bubbling of lava.",
	"You chime in on a pack of demons performing a satanic ritual.",
	"You feel that Mehrunes Dagon will open the Great Gate of Oblivion soon!",
	"You realize that the hellspawn invasion has already begun...",
	"You hear a female voice shouting angrily.",
	"You listen to a long, squeaking sound.",
	"You can hear the clacking noises of high heels walking on a hard floor.",
	"You fear that some stupid woman is gonna slap you silly!",
	"You can't resist thinking of a pair of brown leather boots with black block heels, and start wanking off to the thought.",
	"You feel a slight breeze.",
	"You hear crickets chirping.",
	"You can hear the cattle bellowing.",
	"You hear someone sing 'Old Mac Donald had a farm...'",
	"You can't help it but feel that something that looks normal is terribly amiss.",
	"You are chilled by cold air.",
	"You shiver for a moment.",
	"You feel that someone forgot to close the door to the freezer.",
	"You fear that you're gonna develop a lung inflammation.",
	"You hear someone calling you to the iceblock shooting.",
	"You are struck by a sense of emptiness.",
	"You sense a black hole in the fabric of reality.",
	"You feel that a special challenge awaits you.",
	"You hear Galadriel whispering 'It is very likely that you die on your journey, but you must go anyway...'",
	"You realize that the atmosphere does not contain oxygen! If you stay around you'll suffocate!",
	"You have a sense of familiarity.",
	"You feel relaxed.",
	"You hear a storeclerk making a transaction.",
	"You feel like returning to your burned home... wait, is there really a reason for doing so?",
	"You hope that you don't have to master the fleeing mine...",
	"You hear a loud siren.",
	"You can hear a police officer requesting reinforcement.",
	"You hear the whipping sound of a rubber hose.",
	"You realize that your cop wanted level is 6, and the forces of law are coming to bust you!",
	"You are chased by a speeding police car!",
	"You feel that you're in a hopeless situation.",
	"You are left hanging in suspense.",
	"You feel that the future is dark, and it gets even darker.",
	"You realize that the challenge of your life awaits you.",
	"You get the feeling that a true champion is going to challenge you soon.",
	"You will not be able to defeat Whitney's Miltank, because the fleecy Whitney will mop the floor with you wimp!",
	"You are not going to get past Pryce, since his ice pokemon can crash the game at will.",
	"You have to fight Lance and his Dragonite, but with your uselessly weak pokemon you don't stand a chance.",
	"You loser will never beat Gary, whose Meganium always uses 'Contro' and defeats each of your pokemon in one hit, hahaha!",
	"You sense the shuffling of random numbers.",
	"You hear the dungeon master testing a set of dice.",
	"You feel that this particular floor is especially random.",
	"You just know that the monsters on this level have the most evil and unbalanced attacks possible.",
	"You test your d20 several times, only to realize in shock that it always rolls 1!",
	"You hear a grating, annoying snore.",
	"You smell dragonbreath in the air.",
	"You hear a 'Klieau!' sound.",
	"You are hit by the 1st tailspike! You are hit by the 2nd tailspike! Warning: HP low! You are hit by the 3rd tailspike! You die. Do you want your possessions identified? DYWYPI? [ynq] (n) _",
	"You want to do wizard farming like in the old days, since it's more profitable than pudding farming.",
	"You hear a sound reminiscent of a frog.",
	"You feel that the gate to Hell has to be closed from the other side.",
	"You realize that the aliens have built a base here.",
	"You hear a deep voice chanting the name 'Jason'...",
	"You hear someone shout 'Neau!'",
	"You felt like you heard a faint noise, but can't make out what it is.",
	"You hear a loud yawn.",
	"You get the impression that the current atmosphere is quiet... too quiet.",
	"You listen to a soothing melody and a chant that sounds like 'Sleep-bundle-wing!'",
	"You suddenly fall asleep. (Hopefully no monster decides to feast on your exposed flesh before you wake up.)",
	"You inhale the scent of seawater.",
	"You listen to fishes splashing water around.",
	"You listen to the sound of gentle waves.",
	"You hear a 'splash splash' noise!",
	"You hear someone announce 'The entrance to Diver's Paradise is free today! Come on over and enjoy the most beautiful terrain in this dungeon - water!'",
	"You can hear animal sounds.",
	"You hear the sound of a caged animal knocking at the lattice.",
	"You hear the roar of hungry zoo animals.",
	"You hear a hissing sound, and it's right behind you!",
	"You look behind you and see a three-headed monkey!",
	"You feel that the air is hot around here.",
	"You feel a lack of humidity.",
	"You hear the roaring of a tornado.",
	"You seem to sink into the floor!",
	"You encounter a sweet asian girl, but as you move to greet her, she suddenly thrusts you into quicksand! You die. Do you want your possessions identified? DYWYPI? [ynq] (n) _",
	"You hear uncommon noises.",
	"You can feel the earth shaking.",
	"You sense a supernatural presence.",
	"You know that you'll see something not meant for mortal eyes.",
	"You hear Pale Night tempting you to remove her veil and look at what's underneath...",
	"You feel that you must free someone who's being held captive.",
	"You hear someone calling for you.",
	"You must free the prisoner unless you want him to be executed.",
	"You were too slow and now the prisoner is dead. You failure!",
	"You really have to hurry - if you don't free the prisoner in 10 turns, he will be killed!",
	"You realize that your Geiger counter has started ticking like mad.",
	"You smell the stench of contamination in the air.",
	"You listen to scientists talking about their experiments.",
	"You feel the presence of the ether generator!",
	"You can overhear the president saying 'That was the last straw! I'll bomb that fucking country now! I just need to get the code and press the big red button!'",
	"You sense the presence of beings from another universe.",
	"You hear the elder priest fiercely lashing his tentacles around.",
	"You feel an earthquake-like trembling, probably caused by a migmatite horror...",
	"You hear Morgoth muttering a chant.",
	"You hear the scream of Lina Inverse's high-pitched voice.",
	"You receive a stamped scroll from the mail demon! It reads: 'Muahahahaha, I kidnapped Princess Peach again. Sincerely, Bowser.'",
	"You start eating the hill giant corpse. Ulch - that meat was tainted! You die from your illness. Do you want your possessions identified? DYWYPI? [ynq] (n) _",
	"Farmland cannot be dug out.",
	"The mountain is too hard to dig into.",
	"The grave wall needs to be dug by moving into it.",
	"You crash into a farmland! Ouch!",
	"You crash into a farmland and die. In fact, you just bought the farm. Do you want your possessions identified?",
	"You cannot cross the farmland! Nature preservation and all that.",
	"While levitating, you cannot enter the tunnel.",
	"There was a monster hidden underneath the wall!",
	"You dig out the grave wall. You hit the NETHACK.EXE caused a General Protection Fault at address 000D:001D. Choose 'Close'. NETHACK will close.",
	"Haha, you probably wish you could do that.",
	"Well, I'm sure you'd love to see the overview, but no. Sorry.",
	"Things open up on the flipside!",
	"The portal radiates strange energy, and monsters appear from nowhere!",
	"You hear a horrific scream!",
	"You hear a horrific scream! Your mind reels from the noise!",
	"You have a severe coughing fit and an intense desire to vomit!",
	"You are consumed by your contamination!",
	"The contamination consumes you completely...",
	"Your contamination increases.",
	"Your body itches comfortably.",
	"The itching on your body increases.",
	"You seem to be developing ulcers.",
	"You feel like your digestive tract started to digest itself.",
	"You feel that your body is consuming itself from within.",
	"You feel terminally ill. Something tells you that you only have three days to live.",
	"You are now afflicted with fatal contamination. Seek medical attention immediately.",
	"You lose alignment points!",
	"You are struck by several ether thorns.",
	"Save begins...",
	"Save lost!",
	"Insufficient memory for action",
	"pure virtual function call",
	"You feel great!",
	"You feel mediocre.",
	"You feel not all that good.",
	"You feel that this is all nonsense.",
	"You suck.",
	"You pass out.",
	"Everything seems to glow for a second.",
	"Sun darkness.",
	"Shit, you're becoming blind!",
	"You feel very forgetful.",
	"You forgot whether you have amnesia.",
	"You are frozen!",
	"You are frozen, and a mumak and a leocrotta approach.",
	"You sense objects.",
	"You sense traps.",
	"You sense monsters.",
	"You sense water tiles.",
	"You sense a lack of staircases on this level.",
	"You sense a lack of staircases on this level. How are you going to get out?",
	"The world goes dark.",
	"The world goes dark. Is this an unexpected eclipse?",
	"You feel less experienced.",
	"It is hard to read in the dark.",
	"Your armor glows purple and turns into a cursed -10 ring mail.",
	"You hear a distant scream.",
	"You hear a strange warbling noise.",
	"Your hair stands on end for a moment.",
	"Your hair stands on end, and suddenly a lightning bolt hits you!",
	"You aren't wielding anything.",
	"You aren't wielding anything anymore because your weapon just evaporated.",
	"You can't go down now.",
	"The key doesn't fit.",
	"You don't see anything.",
	"The sphere crumbles into dust!",
	"Tweep!",
	"You must use a tinderbox to light things.",
	"Some magic force prevents you!",
	"Some magic force prevents you from existing!",
	"You overate!",
	"You underate!",
	"You underrated loser!",
	"Call it: You are wearing that!",
	"Your hands stop glowing blue.",
	"Your hands stop being attached to the rest of your body.",
	"Your hands stop being attached to the rest of your body. Have fun with closed doors!",
	"Your feet fall off. Now you can't use the kick command anymore.",
	"Your feet fall off. Now you can't use the kick command anymore, but what's worse, you also cannot walk around without them...",
	"You cut your way out!",
	"Your ISP cuts you out of the game and you find yourself back at the dgamelaunch screen!",
	"Your shield shatters.",
	"You are dying from slow poison.",
	"It it J. Random Hacker Goodbye player...",
	"You made the top ten list!",
	"If you die now, you'll be #1 on the top ten list!",
	"If you die now, you'll be #1 on the top ten list! But if you die later, your score will be reduced, nyah-nyah :)",
	"You hit the gray growth. You feel deathly sick. You claw the gray growth. You feel even worse.",
	"The drolem claws you! You can't move! The drolem claws you! You can't move! The drolem breathes --More-- You have died. Do you still insist that the kurwa role is easy?",
	"Number Points Name percent2u",
	"aeiou an a percentd too big The the Panic:",
	"You've been poisoned!",
	"Your burning flask explodes!",
	"You have to fill the liquid in the piston.",
	"A yellow flash blinds you!",
	"You hear a funny popping noise.",
	"You hear a funny popping noise. Apparently it was your cherry.",
	"You hear a funny popping noise, or is that a farting noise? Tee-hee-hee!",
	"You are impaled by a falling stalactoid!",
	"The quivering blob quivers!",
	"You get turned to stone!",
	"You feel really drugged out now.",
	"The grunthack zombie's moans have paralyzed you!",
	"The grunthack zombie's moans have paralyzed you! The zombie bites you! You feel deathly sick. The zombie bites you! You feel much worse. The zombie--More--",
	"The grabber has grabbed you!",
	"The grabber has grabbed you, and is threatening to steal your virginity!",
	"The grabber has grabbed you, and is threatening to steal your virginity! If he does, then he will become a virgin again while you're turned into a loli nymphet!",
	"The satyr plays its flute and puts you to sleep.",
	"The satyr plays its flute and puts you to sleep, then starts to do... nasty things to your helpless body.",
	"The satyr plays its flute and puts you to sleep, then starts to use his penis to take your virginity.",
	"The rope golem has entangled you!",
	"It moved!",
	"You are losing!",
	"It moved! You are losing!",
	"You hear a click.",
	"You hear a clack.",
	"You hear a click-clack sound.",
	"You hear a click-clack sound. Oh crap! It's a time bomb, and it's gonna blow! RUN!!!",
	"The door WAS locked.",
	"The door WAS locked. But now it no longer is, and the 200 trolls that were waiting behind it are pouring out.",
	"The truck ghost touches you! The truck ghost touches you! The truck ghost touches--More-- Warning: HP low!",
	"The ugod butts you! The ugod bites you! The ugod butts you! Kurwa, your life force is running out.",
	"The klieau manticore spits a tail spikes! You are hit by a barrage of 6 tail spikes! Camperstriker, all your powers will be lost...",
	"fuck this monster",
	"gay game is gay",
	"This room smells funny.",
	"This room smells funny. No wonder - it's a ranch, and all the cows and other farm animals also have to take a shit periodically!",
	"This room is painted in strange colors.",
	"This room is painted in fleecy colors.",
	"There's something evil here...",
	"There's something evil here... and as you look around, you see 5 truck ghosts and 10 ugods approaching.",
	"You finish eating the derp corpse. You are now afflicted with fatal contamination.",
	"You feel unusually happy.",
	"You feel usually happy.",
	"You feel usually happy, but right now you're suffering from a stroke of acute depression.",
	"You feel rather sad now.",
	"You feel rather sad now. This message means that your highest-level pet has just died out of the blue.",
	"Your tame truck ghost turns on you! The truck ghost touches you! Necromancer is about to die.",
	"You are drenched by several gallons of water.",
	"You are drenched by several gallons of water and drown. Do you want your possessions identified?",
	"You are drenched by several gallons of oil and spontaneously combust.",
	"You are drenched by several gallons of urine and melt away due to the acidity.",
	"An arrow shot you!",
	"An arrow shot you with its bow!",
	"You duck an arrow! But it turns around like a boomerang and hits you anyway!",
	"A dart whizzes by you and vanishes!",
	"One of your pieces of equipment suddenly whizzes by you and vanishes!",
	"A boulder hits you on the head.",
	"A boulder hits you on the head. You feel stupid! You lose  Intelligence",
	"A boulder bounces off your armor!",
	"A boulder bounces off your armor, which is destroyed in the process, but focus on the good news: at least you didn't get squashed!",
	"You hear a loud grinding noise.",
	"You hear a distant noise sliding wall",
	"You are engulfed in a huge explosion.",
	"You are engulfed in a huge explosion and reduced to ludicrous gibs.",
	"You hear a plink.",
	"You hear a plink. Damn, they could really repair the tap of that kitchen sink one day...",
	"What direction? Cancelled.",
	"The wand of fire glows, then fades. The bolt of fire hits you! Your cloak of magic resistance is burnt to ashes!",
	"The elder priest tentacles to tentacle you, and subjects you to the one message that no player ever wants to read: 'Your brain is cored like an apple!' Your last thought fades away. Do you want your possessions identified?",
	"The elder priest tentacles to tentacle you, and steals stickage from other monsters!",
	"The elder priest tentacles to tentacle you! Your blessed greased fireproof +5 tie-dye shirt of Shambhala is ripped to shreds!",
	"The elder priest tentacles to tentacle you! Your blessed greased +3 silver dragon scale mail is destroyed!",
	"The elder priest tentacles to tentacle you! Your speed boots vanish!",
	"The elder priest tentacles to tentacle you! You feel little mouths sucking on your skin.",
	"The elder priest tentacles to tentacle you! You feel little mouths sucking on your skin. (Eek, where are they coming from? You want to choke them to death by ramming a long, pointy stiletto heel into them.)",
	"The elder priest tentacles to tentacle you! You feel the tentacles bore into your skull!",
	"The elder priest tentacles to tentacle you! You feel the tentacles drill through your unprotected flesh and into your soul!",
	"The elder priest tentacles to tentacle you! You feel the tentacles drill through your unprotected flesh and into your soul and bla bla bla YOU DIE, GAME OVER, THANK YOU COME AGAIN.",
	"The elder priest tentacles to tentacle you! You feel the tentacles drill through your unprotected flesh and into your soul and bla bla bla YOU HAVE PLAYED ENOUGH FOR TODAY, SEE YOU AGAIN NEXT TIME IN DNETHACK WITH ITS GROSSLY UNBALANCED ASSHOLE ELDER PRIEST AND HIS ASSHOLE TENTACLES",
	"The elder priest tentacles to tentacle you! You feel violated and very fragile. Your soul seems a thin and tattered thing.",
	"The elder priest tentacles to tentacle you! You feel violated and very fragile, but very ANGRY above all, so you reach for your most sharp-edged high heel combat boots and SEVER that ASSHOLE tentacle and FUCKING crush it to a FUCKING BLOODY PULP, then tell the ASSHOLE elder priest that his FILTHY WHORE of a mother sucks the cock of SATAN!",
	"The elder priest tentacles to tentacle you! You feel a bit fragile, but strangely whole.",
	"The elder priest tentacles to tentacle you! You feel a bit fragile, but strangely whole. Good thing too, because now you're 'a bit agitated', which is to say REAL ANGRY, and empty 200 rounds of ammo of your heavy machine gun into that asshole's whatever-passes-for-a-brain, then hack up his dead body with a knife until all that's left of him are thin strips of mincemeat.",
	"The elder priest tentacles to tentacle you! You feel the tentacles spear into your unprotected body!",
	"The elder priest tentacles to tentacle you! You feel weak and helpless in the tentacles' grip!",
	"The elder priest tentacles to tentacle you! You feel weak and helpless in the tentacles' grip! But thankfully you have a secret weapon up your sleeve: a pair of steel-capped sandals. Furious like you never were before, you smash the asshole tentacles so hard that they are severed. And while the asshole elder priest is roaring in pain, you quickly whack him over the head with your massive heel. Like a maniac, you use the metallic high heel to hit his damn skull again and again and eventually manage to split it in two.",
	"The corridor glows briefly.",
	"The room is lit.",
	"The room was lit, but someone pushed the button and now it's unlit.",
	"You are not in a shop.",
	"You are not in a shop. You also don't know why that is considered relevant information.",
	"You have no money.",
	"You have no money. Gotta live on social welfare assistance for all eternity.",
	"You have no money, and nobody is gonna give you any, you filthy beggar scum!",
	"ruby silver ivory",
	"Do you accept?",
	"Fork failed!",
	"Type space to continue",
	"memory allocation problem.",
	"This version of SLASH'EM Extended is pirated. Report any software piracy to: 1-888-Amy-is-a-filthy-harlot",
	"Incoming message for Player 1. The message is from 'Yvonne'. It reads: 'I want to absolutely stomp your balls with my dark blue sneakers.'",
	"Incoming message for Player 1. The message is from 'Alida'. It reads: 'My winter boots are so adorable.'",
	"Incoming message for Player 1. The message is from 'Marleen'. It reads: 'Well hello there! Would you want to receive some pain from me? :-)'",
	"Incoming message for Player 1. The message is from 'Tobi'. It reads: 'Checkmate!'",
	"Incoming message for Player 1. The message is from 'Kati'. It reads: 'I want to do a survey in order to find out which boy has the sexiest butt.'",
	"Incoming message for Player 1. The message is from 'Kristin'. It reads: 'Bah, I only got a Mercedes. I want a real car, one that's good enough for a lady like me!'",
	"Incoming message for Player 1. The message is from 'Aee'. It reads: 'Why do I feel so empty?'",
	"Incoming message for Player 1. The message is from 'Aee'. It reads: 'What was that? If you want to sneak up on someone, you should not put on socks that can be smelled from half a kilometer away!'",
	"Incoming message for Player 1. The message is from 'Leo'. It reads: 'No, I refuse to shoot my friend Mohmar. I don't care what you say, I cannot be persuaded.'",
	"Incoming message for Player 1. The message is from 'Sunali'. It reads: 'Oh no, the robbers scored a goal!'",
	"Incoming message for Player 1. The message is from 'Robat'. It reads: 'GOALGOALGOALGOALGOALGOALGOOOOOOOOOOOOOOOOOOOOOOOOAL!!!'",
	"Incoming message for Player 1. The message is from 'Corina'. It reads: 'Keep the ball out of our half now for once! If this continues, it's only a matter of time until the opposing team scores a goal!'",
	"Incoming message for Player 1. The message is from 'Robat'. It reads: 'I'll still score a goal against you today, yes, trust me I will!'",
	"Incoming message for Player 1. The message is from 'Arne'. It reads: 'You full refuge!'",
	"Incoming message for Player 1. The message is from 'Arne'. It reads: 'You old asshole, I'm coming to get you.'",
	"Incoming message for Player 1. The message is from 'Arne'. It reads: 'I'm not done with you yet.'",
	"Incoming message for Player 1. The message is from 'Arne'. It reads: 'You're dead! DEAD!'",
	"Incoming message for Player 1. The message is from 'Arne'. It reads: 'What kind of assholes I'm up against here?'",
	"Incoming message for Player 1. The message is from 'Arne'. It reads: 'You're going down.'",
	"Incoming message for Player 1. The message is from 'Arne'. It reads: 'Accursed sow package!'",
	"Incoming message for Player 1. The message is from 'Arne'. It reads: 'Now I'll run amok and gun down everyone with my MP5!'",
	"Incoming message for Player 1. The message is from 'Arne'. It reads: 'Don't you dare calling me 'Asshole Arne' ever again!'",
	"Incoming message for Player 1. The message is from 'Arne'. It reads: 'Don't try to talk yourself out of it! I know you were serious when you called me 'Asshole Arne'!'",
	"Incoming message for Player 1. The message is from 'Arne'. It reads: 'Change your behavior right now! F-u-l-l r-e-f-u-g-e!'",
	"Incoming message for Player 1. The message is from 'Arne'. It reads: 'Neener-neener.'",
	"Incoming message for Player 1. The message is from 'Arne'. It reads: 'Gotta remove the disturbed object.'",
	"Incoming message for Player 1. The message is from 'Arne'. It reads: 'Another mucky type who thinks he has to announce something to me!'",
	"Incoming message for Player 1. The message is from 'Arne'. It reads: 'I'm serious, don't get too close to me or there won't be any guarantees!'",
	"Incoming message for Player 1. The message is from 'Arne'. It reads: 'I'm the King here! And I'll gun down everyone who doesn't respect me!'",
	"Incoming message for Player 1. The message is from 'Arne'. It reads: 'Last warning! Come out of your hiding place, asshole, or I'll make you fixed and finished!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Oh, I think there's someone there in the back!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Are you in my team?'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Don't shoot me for now, wait until I drew my gun!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Sed tote particulare we must finish el comte playername.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Hey! Why is the game running so fast now?'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'You're funny looking!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Does everyone look as funny as you around here?'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Hey, my team is the ultimate dreamteam, so come and join me!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'If you knew that I'm the supreme master at this game...'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'On salue, pomme de terre inconnue!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Guter Tachometer!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'I'll open fire at you now. Maybe I'll hit.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Ra-ta-ta-ta-ta-ta-ta-TA-ta-TA-ta-TA-ta-TA-ta-TA-ta-TA-ta-TA-ta-TA-ta!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Pschoeoeoeu! Tschieauwick! Peng!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Aqueles mostram ichs!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Hey, I think I can see you! Can you tell me whether you can see me too?'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Ah I see you! You're one of my teammates, right? I'll help you fight your opponents!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Oh, you're here again! I thought you were in some completely different place just a couple turns ago?'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'How do you manage to reach that dungeon level so quickly?'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Where did you get your weapon? I want one of those too!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Taigi, dabar mes tiesiog saudyti visa laika ant priesas!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Hurtig, stop ved! Jeg kan ikke lade nogen abschiesst dig!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'U hoeft niet langs me.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Issa ser nuri li inti l-ahjar tal-patata hija!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Atann ou a bat!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Konishiwa!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Fo... Krah Diin! Did I get you with my dragon shout? You should be slower now!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Yes, yes, I know that you have to use the mouse to aim in a first person shooter!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'What was the attack button again?'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Wait! I gotta reload my gun! Don't shoot me until I'm done!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Don't shoot me until I bandaged my wounds!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'What am I supposed to do when my gun is out of ammo?'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'What the hell is a 'hotkey' supposed to be? I have no idea!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'What's so special about hotkeys? Does my keyboard even have any of those?'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Mon dieu, sacre coeur!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Goed, bist een oude bedrieger, bist warschijnlijk de greetste bedrieger dat ik in mijn leven zag!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Ainsi de moi guerres aucun centesimo!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Well I'm playing badly on purpose, so the poor opponents think they stand a chance. I don't want them to become depressed.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'I'll play badly for another 30 seconds. If you can kill me until then, more power to you. Otherwise I will end you.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Okay, so I did get hit after all. But I'll still win this fight.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Eh, como eso e, err wait a moment, why are you shooting me?'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Err, my gun isn't shooting anymore! What? I'm out of ammo? Ah, I see! But what is the button I need to press for reloading?'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Oh tidak, saya telah diambil!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Mitae on tehnyt minulle? Koska milloin, koska tavoitteena?'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Saya akan menangkap Anda lagi!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Normalisht nuk i ploteson ne te gjitha, por kjo kohe tashme!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Mara moja juu ya kusikia hivyo!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Oh no, I think I'm losing.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'I seem to be wounded, but I can keep bleeding!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'What, you need to bandage your wounds in this game? What button does that?'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'err, hotkey for bandaging my wounds? Where is that supposed to be? Is it on the moon?'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Sum scalum dei succumbens.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Don't fire your gun non-stop, you're only wasting your ammo!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Que imaginan, se disturban?'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'What, why am I losing HP all the time? Only 44 HP left! Oh no, and now it's only 43! What is going on here?'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Why am I so slow all of a sudden, and why can't I jump anymore?'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'What? I set spacebar to jump, why isn't it working anymore?'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Jeg vet ikke hva jeg gjoeer naa!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Sada situacija nav noverots pirms!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'mano automobilis sugedo?'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Mis olen kaotamas kuidagi? Kuidas konservitooside oma?'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Ddigon! Mae'n ymerodraethau nawr!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'You know, I only pretend to be really bad at the game.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Am I going to be promoted to team leader rank?'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Il nemico e stato venduto.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Os ichs da mostra de.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'The master taught you a lesson.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Don't be sad about the fact that I'm the master, not you!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'You should train, then you might get as good as me at this game one day.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'The great Egas defeated you today.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'To parousiazei ichs.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'To einai opheilomeno.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Arrivederci! Ha, ha, ha!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Bow before the great Egas, hahaha!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Van moi oorlogen geen cent!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Don't take it personal, but you're very bad at this game.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'We think that you don't know how to play this game.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Did you see? I can aim, no problem!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Everything is going alright, I don't know what you're bickering about all the time?'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Stop pretending that I was really bad at the game!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'I'll play worse than usual for a couple minutes so the opponents also have a chance.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'The game is way too easy! No opponent can even dent me!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Stellen Sie sicher, dass auch Kriege von ihnen ueber den Haufen wirft!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Proc nemuze nepritel, protoze dokonce i vodnik? Endeavoring coz je asi ne?'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Grand przyznaje.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Deblocarea de mare e pe gaz!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Via con voi!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Poso graegora to kanei!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Agora estou remate porque iso pode nada.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Taim ag bhfad nios fearr a thabhairt duit...'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Waiting politics!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Maybe more ammo for my gun will be created over time if I just wait for a while.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'I'll win anyway, no matter which weapon I'm using, but there's something else I want to find out.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Dominus Patris, Signum Vitiosum haec esse...'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'A puskat en fogom mindig felel meg az ellenfelet, mert en vagyok a kapitanya, amelynek celja!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'My abilities are considerable.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'I'm a master who fell from the heavens.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Ever since I defeated Micky, I'm the grand master.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Not even you would have defeated Micky back then.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Not even you can live up to my might.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'I didn't know that this place is called the Dungeons of Doom!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'My hit point bar shows the value 100... is that a lot?'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'A minha arma e semper verdade, porque agora eu sei como usa-los.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Samo vem, kako je ta igra gre desno!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Budem vyhrajte!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Min pistol att skjuta alla motstaendare under hoegar.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Gravity killed me.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Let's abolish the law of gravity!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'What? Why did you type 'kill' there?'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'What are you writing in the weird console all the time?'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Huh? That was the suicide button? What's that supposed to be good for? It's useless! All it can do is maybe confuse the opponent!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'That was probably the grasp in the toilet.'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Err, why does the button between Alt and Ctrl seem to boot me out of the game? Now I need to struggle to get it back!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Why is my game gone when I press Ctrl, Alt and Delete at the same time?'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Quin va ser que, degut a que una vegada mes? El joc es configura de manera estupida!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: '?Por que es entonces que el suicidio boton E? Como ya he salido con demasiada frecuencia!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Cac khang khong co co hoi!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Nun zu Mach ich Sie jedoch voll und ganz fertig! Sie koennen die Erfahrung!'",
	"Incoming message for Player 1. The message is from 'Egas'. It reads: 'Zum... Haal Viik! Err, wait a moment, that didn't work at all? It disarmed me instead of you!'",
	"Incoming message for Player 1. The message is from 'Doctor Maex'. It reads: 'Nowadays it is hard to find a doctor who would make house calls... but Doctor Maex still does, hahaha!'",
	"Incoming message for Player 1. The message is from 'Doctor Maex'. It reads: 'Breathe easy, for I am Doctor Maex!'",
	"Incoming message for Player 1. The message is from 'Doctor Maex'. It reads: 'Don't worry, I'll only attack you. Our little war will play out uninterrupted and your delicate sensibilities will not be taxed.'",
	"Incoming message for Player 1. The message is from 'Doctor Maex'. It reads: 'Have a taste of my old medicine, hehehe!'",
	"Incoming message for Player 1. The message is from 'Doctor Maex'. It reads: 'The first sample is free, hahahahaha!'",
	"Incoming message for Player 1. The message is from 'Doctor Maex'. It reads: 'With every breath you take you'll get closer to defeat!'",
	"Incoming message for Player 1. The message is from 'Doctor Maex'. It reads: 'Are you ready for another chemistry lesson?'",
	"Incoming message for Player 1. The message is from 'Doctor Maex'. It reads: 'You have so many items, and they're all useless!'",
	"Incoming message for Player 1. The message is from 'Doctor Maex'. It reads: 'Argh, I am defeated. Where's that cyanide capsule?'",
	"Incoming message for Player 1. The message is from 'Doctor Maex'. It reads: 'You violated the Geneva Convention!'",
	"Incoming message for Player 1. The message is from 'Doctor Maex'. It reads: 'So you think my weapons weren't effective.'",
	"Incoming message for Player 1. The message is from 'Doctor Maex'. It reads: 'I'll gas you with prussic acid.'",
	"Incoming message for Player 1. The message is from 'Doctor Maex'. It reads: 'In the United States there are mass abortion camps, wihle in Germany there's only gas chambers.'",
	"Incoming message for Player 1. The message is from 'Doctor Maex'. It reads: 'You can tell that my poisonous gas is working - as soon as your flesh falls from the bones, hahaha!'",
	"Incoming message for Player 1. The message is from 'Doctor Maex'. It reads: 'Prepare to tell your three-eyed grandchildren of your defeat today!'",
	"Incoming message for Player 1. The message is from 'Doctor Maex'. It reads: 'You'll come for the toxin, but you'll stay - because you're dead! Hahaha!'",
	"Incoming message for Player 1. The message is from 'Doctor Maex'. It reads: 'Yeah, that's right, sit back and relax. Give my toxins more time to work!'",
	"Incoming message for Player 1. The message is from 'Doctor Maex'. It reads: 'I'll make you glow in the dark.'",
	"Incoming message for Player 1. The message is from 'Doctor Maex'. It reads: 'Are you going to attack me or are you afraid of what you might find out I'm capable of?'",
	"Incoming message for Player 1. The message is from 'Doctor Maex'. It reads: 'Our poison gas networks will poison you.'",
	"Incoming message for Player 1. The message is from 'Doctor Maex'. It reads: 'I'll tell everyone about your defeat!'",
	"Incoming message for Player 1. The message is from 'Doctor Maex'. It reads: 'You will make mistakes soon, and then the tide will turn.'",
	"Incoming message for Player 1. The message is from 'Doctor Maex'. It reads: 'Did I inhale too many of my own toxins? You can't be on your way to ascension!'",
	"Incoming message for Player 1. The message is from 'Doctor Maex'. It reads: 'My toxic trucks will roll again!'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'errrrrrrrr theres someone coming!'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'one has always so little ammo and thus no chance'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'here come much too much!'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'this game is too fast for me!'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'this game is not my cup of tea'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'you play like a madman'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'ill never become good at this game but at least i still have a life outside of the game'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'o no i was hit!'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'i dont know that the mouse is used for aiming'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'i cant play this game'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'no, not again...'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'the weapons in this game are retarded'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'why do i never have good weapons?'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'the guns all have too little ammo'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'what? why are you hostile all of a sudden?'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'wuaaaaa-a-a-a-aah, there come way too much!'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'ey those their team has more types than ours'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'there tunes which not'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'they cheated'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'i pressed the wrong button'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'i never have a chance no matter what i do'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'that is no fun'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'o no i dont wanna anymore'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'oh i am so bad'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'eeeerrrr what is that?'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'ey if you continue ill quit'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'oh is that a stupid map!'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'feier in se houl, haehaehae'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'and off-balled!'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'ha, ha, ha!'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'bragger! you can do nothing at all'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'opponent ass-deaf-auger, haehaehae'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'ill bomb you back to the stone age'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'im not complaining at all, everything is working perfectly'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'stop shouting around here'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'you can only cheat'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'not feeling so fucking full of yourself now huh'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'ra ta ta ta ta'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'so quickly goes that'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'now i killed you once and youre complaining already'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'im tired of you calling me a failure all the time'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'so you think youre the greatest'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'ratarataratarataratarataratarataratarataratarata'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'a little even i can hit'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'haehaehae, your game isnt running that well this time huh'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'i can only camp'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'call me a camper asshole, i dont care'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'i go again in the asshole-me-is-now-all-no-matter-asshole attitude'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'bla bla bla, im not listening to you'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'i have 100 hp left, lets see how long they last'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'im finding no one'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'i never find enemies here'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'there are not enough monsters on this dungeon level'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'o no, i am at losing'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'already again'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'is this boring'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'this level is no fun at all'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'huh? what?'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'err, what was there?'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'why am i dead all of a sudden?'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'eeeeeerrrrrrrr... oh you have to shoot with the mouse?'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'oooohhhhhh wrong button...'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'err where is the attack button?'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'nou, nou, nou, so it makes no fun'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'ey this gives it not, i have never a chance, no matter what i make'",
	"Incoming message for Player 1. The message is from 'Dictator P'. It reads: 'ey i can play however i want and never win'",
	"Incoming message for Player 1. The message is from 'Sophia'. It reads: 'Alert, all teammates, enemy spotted in the Dungeons of Doom!'",
	"Incoming message for Player 1. The message is from 'Sophia'. It reads: 'You're muted permanently now!'",
	"Incoming message for Player 1. The message is from 'Sophia'. It reads: 'Your subtle diplomacy won't work on me.'",
	"Incoming message for Player 1. The message is from 'Sophia'. It reads: 'I found a nest of enemies. Teammates, help me smoke them.'",
	"Incoming message for Player 1. The message is from 'Sophia'. It reads: 'Our team has to buckle the defense belt more tightly!'",
	"Incoming message for Player 1. The message is from 'Sophia'. It reads: 'I'll tell the boss what you have done.'",
	"Incoming message for Player 1. The message is from 'Sophia'. It reads: 'So you're even proud of your evil deeds, huh?'",
	"Incoming message for Player 1. The message is from 'Sophia'. It reads: 'One day you'll be licking the dust off my feet.'",
	"Incoming message for Player 1. The message is from 'Sophia'. It reads: 'You never learned any manners, huh?'",
	"Incoming message for Player 1. The message is from 'Sophia'. It reads: 'Attacking defenseless ladies, huh? I'll teach you to stop that!'",
	"Incoming message for Player 1. The message is from 'Sophia'. It reads: 'Another one who would underestimate a woman?'",
	"Incoming message for Player 1. The message is from 'Sophia'. It reads: 'I'll blow away your nuts.'",
	"Incoming message for Player 1. The message is from 'Sophia'. It reads: 'Where are you hiding again?'",
	"Incoming message for Player 1. The message is from 'Sophia'. It reads: 'You won't leave this dungeon, not even in a box.'",
	"Incoming message for Player 1. The message is from 'Sophia'. It reads: 'You will not escape this time.'",
	"Incoming message for Player 1. The message is from 'Sophia'. It reads: 'I'll defeat you right now.'",
	"Incoming message for Player 1. The message is from 'Sophia'. It reads: 'I'll defeat all your friends now, too.'",
	"Incoming message for Player 1. The message is from 'Sophia'. It reads: 'Nobody likes you!'",
	"Incoming message for Player 1. The message is from 'Sophia'. It reads: 'Go stand in a corner!'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'Come here and fight.'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'Come here right now!'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'Over here! My weapon is waiting for you!'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'I hope you're playing fair. No cheats, you hear?'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'If you cheat now, I'll never talk to you again.'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'Come on!'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'Your cowardly choice of weapons will not help you win the game.'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'What are you afraid of?'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'I'll spawn-kill you!'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'You think you have the upper hand now, but you're making mistakes soon, and then the tide of this game will turn...'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'Lookie what I found!'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'You're trying to kill time, huh.'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'Oh, a backdoor attack. I didn't realize you were so devious.'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'Put some effort into it.'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'Spawnkilling 0wnz ya!'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'Spawnkilling really does 0wn ya!'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'Don't just stand there after you respawn. Only n00bs forget to start running immediately after respawning!'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'Now you'll pay for your mistakes.'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'You can't kill what you can't see!'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'You'll never realize what's going on before it's too late.'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'My undercover strategies will make short work of you.'",
	"Incoming message for Player 1. The message is from 'Katzou'. It reads: 'My stealth assassins are all around this dungeon level - hidden from view, ready to strike.'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'I'll show you old bag who's boss.'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'You show ichs!'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'Flee, you midget.'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'You don't stand a chance.'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'You'll leave this dungeon level in an ambulance.'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'I'll break your bones one after the other.'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'Seeing you makes me want to vomit.'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'Bah, you stand no chance. I'll reduce you to dust!'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'I'm so fast that I can move at the speed of light!'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'Did you crawl out from under a heap of shit right now?'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'You're annoying!'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'You got a problem?'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'How do you manage to always look that ugly?'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'If you get into trouble, I'll certainly not help you.'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'fuck you shitty stalker!'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'I'll flog you!'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'You got problems!'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'How many problems do you want to have?'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'Dude, your sister looks like a steamroller.'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'Wanna know why prisons exist? To prevent people like you from walking around!'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'I'll give your address to the organ donor mafia, and they'll rip out your heart.'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'You have no business here! Get out right now!'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'You won't mess with me again!'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'Now is however silence!'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'I'll fix your lack of character with a mallet.'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'If you open your mouth again, I'll close it with my boot.'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'That I'll finish you off is not a threat but a promise.'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'You're the most fucked up bastard that's running around here!'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'You're great. Bragging all the time but can't take a hit from anything.'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'I'm glad that I'm not you!'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'Ey you want some of me? You stand no chance!'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'ah just fuck yourself...'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'I have a rifle!'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'Better call the ambulance, you coward.'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'If I find you, I'll make you end up in the wheelchair!'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'If I were you I'd be shitting myself!'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'Bah, I'll bash you to nirvana!'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'I'll ram my knife into your ugly body.'",
	"Incoming message for Player 1. The message is from 'David'. It reads: 'You erred into my area again!'",
	"Incoming message for Player 1. The message is from 'David'. It reads: 'No, you won't defeat me now.'",
	"Incoming message for Player 1. The message is from 'David'. It reads: 'You and your empty threats.'",
	"Incoming message for Player 1. The message is from 'David'. It reads: 'You should be glad someone is noticing you at all.'",
	"Incoming message for Player 1. The message is from 'David'. It reads: 'What did you think? You won't get rid of me that easily.'",
	"Incoming message for Player 1. The message is from 'David'. It reads: 'I'll shoot you to the moon.'",
	"Incoming message for Player 1. The message is from 'David'. It reads: 'Soon you're gonna be licking my boots.'",
	"Incoming message for Player 1. The message is from 'David'. It reads: 'I'll bang you off, you bad small type!'",
	"Incoming message for Player 1. The message is from 'David'. It reads: 'You're saying you want to challenge my gang again? Well just you wait!'",
	"Incoming message for Player 1. The message is from 'David'. It reads: 'I'll blow up your corpse with a grenade.'",
	"Incoming message for Player 1. The message is from 'David'. It reads: 'You can do nothing but make empty threats.'",
	"Incoming message for Player 1. The message is from 'David'. It reads: 'I wouldn't challenge David if I were you.'",
	"Incoming message for Player 1. The message is from 'David'. It reads: 'Don't think anyone is going to mourn if you die!'",
	"Incoming message for Player 1. The message is from 'David'. It reads: 'From tomorrow at 6 o'clock 10 we'll shoot back!'",
	"Incoming message for Player 1. The message is from 'David'. It reads: 'lol, like you were capable of anything...'",
	"Incoming message for Player 1. The message is from 'David'. It reads: 'This area still belongs to me and my gang!'",
	"Incoming message for Player 1. The message is from 'David'. It reads: 'My gang will eradicate you!'",
	"Incoming message for Player 1. The message is from 'David'. It reads: 'Start praying to your gods! My gang is going to hunt you to the end of time!'",
	"Incoming message for Player 1. The message is from 'David'. It reads: 'You just dug your own grave.'",
	"Incoming message for Player 1. The message is from 'David'. It reads: 'I'll annihilate you personally! What you did will not be unavenged!'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'I'm not easily defeated, unlike the weaklings you're normally fighting.'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'I'm not just anyone.'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'Don't get cocky now.'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'I'm the master of this battle and will defeat you single-handed!'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'Do your best, or this game will be very short!'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'Death Strikes!'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'I activated my GPS Scrambler.'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'I just turned invisible.'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'You apparently want to inhale a cloud of poisonous gas.'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'Are you done now? Did you exhaust your pathetic mana supply?'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'You should look for an easier game.'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'Wake me up when you're at the DYWYPI screen.'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'Come on, chlorine gas smells very nice! You should inhale deeply!'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'Ha, ha, ha, you old error...'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'You have an error, hahaha!'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'You're not even trying to play well, I'm really falling asleep right now...'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'your playing style is so totally sleep-inducing...'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'Are you really going to altar farm for ten hours straight only to die a couple dungeon levels later?'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'Just #quit already!'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'Yaaaaaaaaaaawn...'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'You're trying hard in this game, but it's all for naught.'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'weird, I thought you wanted to #quit...'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'huh? you obviously forgot to #quit!'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'didn't you want to #quit 10 minutes ago?'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'You should finally figure out how to use the #quit command.'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'Is your dying skill as good as your whiffing skill?'",
	"Incoming message for Player 1. The message is from 'Mohmar Deathstrike'. It reads: 'You have stolen my technology! Give it back now or you'll pay!'",
	"Incoming message for Player 1. The message is from 'Ronaldo'. It reads: 'watching out is called the rule'",
	"Incoming message for Player 1. The message is from 'Ronaldo'. It reads: 'you called me ronaldo again'",
	"Incoming message for Player 1. The message is from 'Ronaldo'. It reads: 'hold your flap'",
	"Incoming message for Player 1. The message is from 'Ronaldo'. It reads: 'from you i let myself nothing say'",
	"Incoming message for Player 1. The message is from 'Ronaldo'. It reads: 'tell me immediately why you always call me ronaldo'",
	"Incoming message for Player 1. The message is from 'Ronaldo'. It reads: 'flap hold, i am not called ronaldo damn it'",
	"Incoming message for Player 1. The message is from 'Ronaldo'. It reads: 'err what wants that from me'",
	"Incoming message for Player 1. The message is from 'Ronaldo'. It reads: 'stop calling me ronaldo'",
	"Incoming message for Player 1. The message is from 'Ronaldo'. It reads: 'you with your shitty ronaldo, that is not my name at all'",
	"Incoming message for Player 1. The message is from 'Ronaldo'. It reads: 'which should that'",
	"Incoming message for Player 1. The message is from 'Ronaldo'. It reads: 'what picture you yourself in'",
	"Incoming message for Player 1. The message is from 'Ronaldo'. It reads: 'i want back to my mummy'",
	"Incoming message for Player 1. The message is from 'Ronaldo'. It reads: 'i made myself into the trousers'",
	"Incoming message for Player 1. The message is from 'Ronaldo'. It reads: 'you should be punished for always calling me ronaldo'",
	"Incoming message for Player 1. The message is from 'Ronaldo'. It reads: 'you fear hare, go to your mummy'",
	"Incoming message for Player 1. The message is from 'Ronaldo'. It reads: 'i told you that you should not call me ronaldo'",
	"Incoming message for Player 1. The message is from 'Ronaldo'. It reads: 'what listen you however also not'",
	"Incoming message for Player 1. The message is from 'Ronaldo'. It reads: 'self debt'",
	"Incoming message for Player 1. The message is from 'Ronaldo'. It reads: 'you are me no matter'",
	"Incoming message for Player 1. The message is from 'Ronaldo'. It reads: 'can do me times with your ronaldo shit'",
	"Incoming message for Player 1. The message is from 'Ronaldo'. It reads: 'now you must be quiet'",
	"Incoming message for Player 1. The message is from 'Ronaldo'. It reads: 'of because of i would be called ronaldo'",
	"Incoming message for Player 1. The message is from 'Ronaldo'. It reads: 'who me ronaldo calls, can which experience'",
	"Incoming message for Player 1. The message is from 'Ronaldo'. It reads: 'how many times? i am not called ronaldo at all'",
	"Incoming message for Player 1. The message is from 'G-cheater'. It reads: 'I'm coming to defeat you.'",
	"Incoming message for Player 1. The message is from 'G-cheater'. It reads: 'You should be replaced by a menis - half man, half penis!'",
	"Incoming message for Player 1. The message is from 'G-cheater'. It reads: 'Well wait, I'll cheat you away now!'",
	"Incoming message for Player 1. The message is from 'G-cheater'. It reads: 'I'm gonna show you how good my cheating-fu is!'",
	"Incoming message for Player 1. The message is from 'G-cheater'. It reads: 'I'll fuck up the server admin if he doesn't enable cheats!'",
	"Incoming message for Player 1. The message is from 'G-cheater'. It reads: 'Last time I said 'yay', so this time I say 'Mikerudstrik'!'",
	"Incoming message for Player 1. The message is from 'G-cheater'. It reads: 'Ha, ha, ha, you can do nothing at all...'",
	"Incoming message for Player 1. The message is from 'G-cheater'. It reads: 'Go back to kindergarten.'",
	"Incoming message for Player 1. The message is from 'G-cheater'. It reads: 'Why are you so bad?'",
	"Incoming message for Player 1. The message is from 'G-cheater'. It reads: 'You can't cheat better than me.'",
	"Incoming message for Player 1. The message is from 'G-cheater'. It reads: 'The previous player I watched was bad already, but you? HA HA HA HA HA HA HA!'",
	"Incoming message for Player 1. The message is from 'G-cheater'. It reads: 'I'm a marital arts master, you weakling.'",
	"Incoming message for Player 1. The message is from 'G-cheater'. It reads: 'Yay! Mikerudstrik! Buschardaschar!'",
	"Incoming message for Player 1. The message is from 'G-cheater'. It reads: 'Aeaet-for-sossen!'",
	"Incoming message for Player 1. The message is from 'G-cheater'. It reads: 'Soldarsons! Bodwosno!'",
	"Incoming message for Player 1. The message is from 'G-cheater'. It reads: 'I need to reconfigure my cheating program!'",
	"Incoming message for Player 1. The message is from 'Jane'. It reads: 'Don't judge me by my low size.'",
	"Incoming message for Player 1. The message is from 'Jane'. It reads: 'I don't look dangerous, but I am.'",
	"Incoming message for Player 1. The message is from 'Jane'. It reads: 'A cute young girl is gonna bludgeon you now.'",
	"Incoming message for Player 1. The message is from 'Jane'. It reads: 'If you die, I'll be dancing on your grave.'",
	"Incoming message for Player 1. The message is from 'Jane'. It reads: 'I'll go drink a beer now.'",
	"Incoming message for Player 1. The message is from 'Jane'. It reads: 'I quickly need to smoke a cigarette!'",
	"Incoming message for Player 1. The message is from 'Jane'. It reads: 'I knew that you're gonna hide there.'",
	"Incoming message for Player 1. The message is from 'Jane'. It reads: 'You should learn some new tricks.'",
	"Incoming message for Player 1. The message is from 'Jane'. It reads: 'You should respect my gang!'",
	"Incoming message for Player 1. The message is from 'Jane'. It reads: 'Now I'm gonna teach you some respect!'",
	"Incoming message for Player 1. The message is from 'Jane'. It reads: 'I'll tell David that you're a threat for our gang!'",
	"Incoming message for Player 1. The message is from 'Hannes'. It reads: 'Hands in the air or I shoot!'",
	"Incoming message for Player 1. The message is from 'Hannes'. It reads: 'You can't evade the law.'",
	"Incoming message for Player 1. The message is from 'Hannes'. It reads: 'I busted many criminals already, and now I'll bust you!'",
	"Incoming message for Player 1. The message is from 'Hannes'. It reads: 'I will hold you, you for asshole!'",
	"Incoming message for Player 1. The message is from 'Hannes'. It reads: 'I know where your house lives! You can't hide from me.'",
	"Incoming message for Player 1. The message is from 'Hannes'. It reads: 'I am an officer of the law!'",
	"Incoming message for Player 1. The message is from 'Hannes'. It reads: 'Another no-good bastard!'",
	"Incoming message for Player 1. The message is from 'Hannes'. It reads: 'For your offense you'll get imprisoned.'",
	"Incoming message for Player 1. The message is from 'Hannes'. It reads: 'Last warning! Surrender or I'll shoot!'",
	"Incoming message for Player 1. The message is from 'Hannes'. It reads: 'Your head is gonna ring like a slot machine!'",
	"Incoming message for Player 1. The message is from 'Hannes'. It reads: 'My arrest jets will still hit you!'",
	"Incoming message for Player 1. The message is from 'Hannes'. It reads: 'I'll use water cannons and tear gas canisters on you!'",
	"Incoming message for Player 1. The message is from 'Hannes'. It reads: 'You're hereby convicted for violating the weapons law, since you're not allowed to be carrying that weapon.'",
	"Incoming message for Player 1. The message is from 'Hannes'. It reads: 'Last warning. Throw your gun away or I'll shoot you.'",
	"Incoming message for Player 1. The message is from 'Hannes'. It reads: 'You're busted. Tell your stupid excuses to the court judges.'",
	"Incoming message for Player 1. The message is from 'Hannes'. It reads: 'You're busted, and you'll share your prison cell with Paris Hilton.'",
	"Incoming message for Player 1. The message is from 'Hannes'. It reads: 'It's fun to be a cop!'",
	"Incoming message for Player 1. The message is from 'Hannes'. It reads: 'If you pay me 1000 $ right now, I'll let you run. Otherwise you will be imprisoned.'",
	"Incoming message for Player 1. The message is from 'Hannes'. It reads: 'I'm a policeman! All criminals, surrender or die!'",
	"Incoming message for Player 1. The message is from 'Hannes'. It reads: 'That is common!'",
	"Incoming message for Player 1. The message is from 'Hannes'. It reads: 'You murdered a policeman. The sentence is death. I will shoot you on sight.'",
	"The have returned to percentns lair.",
	"May is urf.",
	"Incoming message for Player 1. The message is from 'Marike'. It reads: 'I have very cute fingernails!'",
	"Incoming message for Player 1. The message is from 'Jannik'. It reads: 'I'm faster than all others.'",
	"Incoming message for Player 1. The message is from 'Jannik'. It reads: 'Yo wassup?'",
	"Incoming message for Player 1. The message is from 'Jannik'. It reads: 'I fear you're on my hit list.'",
	"Incoming message for Player 1. The message is from 'Jannik'. It reads: 'Prepare to be betrayed by me, hahaha!'",
	"Incoming message for Player 1. The message is from 'Jannik'. It reads: 'Now you'll see that I'm really fast!'",
	"Incoming message for Player 1. The message is from 'Jannik'. It reads: 'Man, you're so slow...'",
	"Incoming message for Player 1. The message is from 'Jannik'. It reads: 'No wonder you always get betrayed.'",
	"Incoming message for Player 1. The message is from 'Jannik'. It reads: 'What, you're calling me a traitor??? I didn't betray anyone!'",
	"Incoming message for Player 1. The message is from 'Jannik'. It reads: 'Are yoooooooooooooou sloooooooooooow.'",
	"Incoming message for Player 1. The message is from 'Jannik'. It reads: 'You're the traitor here!'",
	"Incoming message for Player 1. The message is from 'Jannik'. It reads: 'You really need some training!'",
	"Incoming message for Player 1. The message is from 'Jannik'. It reads: 'can you do something other than whining?'",
	"Incoming message for Player 1. The message is from 'Jannik'. It reads: 'Get out of my way!'",
	"Incoming message for Player 1. The message is from 'Jannik'. It reads: 'Disappear from my roadway! NOW!'",
	"Incoming message for Player 1. The message is from 'Jannik'. It reads: 'Oh, you're boring me. Go back home and kiss my ass.'",
	"Incoming message for Player 1. The message is from 'Jannik'. It reads: 'Stop that stupid talk! No one believes you if you state I were a traitor!'",
	"Incoming message for Player 1. The message is from 'Jannik'. It reads: 'So slowly I am coagulated enough.'",
	"Incoming message for Player 1. The message is from 'Jannik'. It reads: 'I'm faster than all those failures anyway...'",
	"Incoming message for Player 1. The message is from 'Jannik'. It reads: 'Hanging around and doing nothing is fun, hahaha!'",
	"Incoming message for Player 1. The message is from 'Nino'. It reads: 'My buddies will eliminate you soon.'",
	"Incoming message for Player 1. The message is from 'Nino'. It reads: 'Keep trying to gain my respect if you like. It won't work.'",
	"Incoming message for Player 1. The message is from 'Nino'. It reads: 'Assume your servant's position.'",
	"Incoming message for Player 1. The message is from 'Nino'. It reads: 'You're gonna look like swiss cheese!'",
	"Incoming message for Player 1. The message is from 'Nino'. It reads: 'Instead of world domination, all that you will get is eternal slavery.'",
	"Incoming message for Player 1. The message is from 'Nino'. It reads: 'All cool in Kabul!'",
	"Incoming message for Player 1. The message is from 'Nino'. It reads: 'You wanna mess with us?'",
	"Incoming message for Player 1. The message is from 'Flo-ooo'. It reads: 'Did you train? I didn't! Let's see if I'm still better than you!'",
	"Incoming message for Player 1. The message is from 'Flo-ooo'. It reads: 'Do you think you can play better than me? Let's find out.'",
	"Incoming message for Player 1. The message is from 'Flo-ooo'. It reads: 'Your playing skill is certainly not as high as mine.'",
	"Incoming message for Player 1. The message is from 'Flo-ooo'. It reads: 'POEZ! Yeeeeeeeeah!'",
	"Incoming message for Player 1. The message is from 'Flo-ooo'. It reads: 'You're really a noob, and a whiny one at that!'",
	"Incoming message for Player 1. The message is from 'Flo-ooo'. It reads: 'I'm voting CDU because nothing good ever came from SPD.'",
	"Incoming message for Player 1. The message is from 'Flo-ooo'. It reads: 'You should train more, then you might play better!'",
	"Incoming message for Player 1. The message is from 'Flo-ooo'. It reads: 'boring... don't you want to play better?'",
	"Incoming message for Player 1. The message is from 'Flo-ooo'. It reads: 'I know what I'm doing, and that's all you need to know.'",
	"Incoming message for Player 1. The message is from 'Flo-ooo'. It reads: 'Thankfully I'm with the POEZ and not the stupid Greens.'",
	"Incoming message for Player 1. The message is from 'Little Marie'. It reads: 'I'll get your ass!'",
	"Incoming message for Player 1. The message is from 'Little Marie'. It reads: 'You think the official boss of my gang was evil? Wait until you meet ME.'",
	"Incoming message for Player 1. The message is from 'Little Marie'. It reads: 'My gang hereby puts a death verdict on you. Oh wait. *I* put a death verdict on you. And I'll be the one that kills you, too!'",
	"Incoming message for Player 1. The message is from 'Little Marie'. It reads: 'I'm gonna smoke you in a pipe!'",
	"Incoming message for Player 1. The message is from 'Little Marie'. It reads: 'I can kill you in my sleep!'",
	"Incoming message for Player 1. The message is from 'Little Marie'. It reads: 'Only I have the power to change your state from alive to dead.'",
	"Incoming message for Player 1. The message is from 'Little Marie'. It reads: 'As long as I'm alive, I will turn your life into hell.'",
	"Incoming message for Player 1. The message is from 'Little Marie'. It reads: 'You have to pay for your arrogance!'",
	"Incoming message for Player 1. The message is from 'Little Marie'. It reads: 'No one wants to see you around here again!'",
	"Incoming message for Player 1. The message is from 'Little Marie'. It reads: 'Knowledge is power, and only I may have any of it!'",
	"Incoming message for Player 1. The message is from 'Little Marie'. It reads: 'You have the ass openly.'",
	"Incoming message for Player 1. The message is from 'Little Marie'. It reads: 'You filthy traitor!'",
	"Incoming message for Player 1. The message is from 'Little Marie'. It reads: 'You got hair on your chest!'",
	"Incoming message for Player 1. The message is from 'Little Marie'. It reads: 'You're obviously homosexual!'",
	"Incoming message for Player 1. The message is from 'Jessica'. It reads: 'You're gonna be defeated by a sweet asian girl now.'",
	"Incoming message for Player 1. The message is from 'Jessica'. It reads: 'Watch out what you'll say now...'",
	"Incoming message for Player 1. The message is from 'Jessica'. It reads: 'You look sinister to me.'",
	"Incoming message for Player 1. The message is from 'Jessica'. It reads: 'Are you a macho?'",
	"Incoming message for Player 1. The message is from 'Jessica'. It reads: 'Don't mess with a sweet asian woman!'",
	"Incoming message for Player 1. The message is from 'Jessica'. It reads: 'You apparently don't respect women!'",
	"Incoming message for Player 1. The message is from 'Jessica'. It reads: 'You're probably a wanted criminal.'",
	"Incoming message for Player 1. The message is from 'Jessica'. It reads: 'Come on, I'm such a sweet asian girl...'",
	"Incoming message for Player 1. The message is from 'Jessica'. It reads: 'May I test my light blue women's shoes on your hands?'",
	"Incoming message for Player 1. The message is from 'Jessica'. It reads: 'I'm sure I can kick your hands until you start bleeding.'",
	"Incoming message for Player 1. The message is from 'Jessica'. It reads: 'If you were an eunuch I wouldn't have a problem with you. But you aren't, so...'",
	"Incoming message for Player 1. The message is from 'Jessica'. It reads: 'Evil shapes lurk everywhere!'",
	"Incoming message for Player 1. The message is from 'Jessica'. It reads: 'Everything is way too dark here!'",
	"Incoming message for Player 1. The message is from 'Jessica'. It reads: 'Normally, women shouldn't be walking alone at night, but I have to...'",
	"Incoming message for Player 1. The message is from 'Jessica'. It reads: 'I want to use a nightly escort service!'",
	"Incoming message for Player 1. The message is from 'Jessica'. It reads: 'Your brain should explode due to the excessive violence you're using.'",
	"Incoming message for Player 1. The message is from 'Nadja'. It reads: 'I recognize your intentions by looking at the tip of your nose.'",
	"Incoming message for Player 1. The message is from 'Nadja'. It reads: 'If you're unprepared, you're in for a nasty surprise.'",
	"Incoming message for Player 1. The message is from 'Nadja'. It reads: 'You should get your IQ tested.'",
	"Incoming message for Player 1. The message is from 'Nadja'. It reads: 'You look like a crazy person.'",
	"Incoming message for Player 1. The message is from 'Nadja'. It reads: 'Are you crazy or do you only look like it?'",
	"Incoming message for Player 1. The message is from 'Nadja'. It reads: 'What do you think you are?'",
	"Incoming message for Player 1. The message is from 'Nadja'. It reads: 'You're really crazy! You don't just look like it!'",
	"Incoming message for Player 1. The message is from 'Nadja'. It reads: 'You should rethink your evil deeds.'",
	"Incoming message for Player 1. The message is from 'Nadja'. It reads: 'If you have a conscience, you will regret what you have done.'",
	"Incoming message for Player 1. The message is from 'Nadja'. It reads: 'If *you*, of all persons, are sane, then I'll become an astronaut.'",
	"Incoming message for Player 1. The message is from 'Nadja'. It reads: 'It should be impossible to be as crazy as you are!'",
	"Incoming message for Player 1. The message is from 'Nadja'. It reads: 'I'm beginning to doubt the Creation! Crazy people like you cannot have been conceived naturally!'",
	"Incoming message for Player 1. The message is from 'Nadja'. It reads: 'Answer your rhetorical questions yourself!'",
	"Incoming message for Player 1. The message is from 'Nadja'. It reads: 'Stop looking like a crazy person!'",
	"Incoming message for Player 1. The message is from 'Nadja'. It reads: 'Why do you always look so weird? You should seriously think about it!'",
	"Incoming message for Player 1. The message is from 'Nadja'. It reads: 'I told you enough times that you're really crazy!'",
	"Incoming message for Player 1. The message is from 'Nadja'. It reads: 'Stop giving me even more proof of the fact that you're crazy.'",
	"Incoming message for Player 1. The message is from 'Nadja'. It reads: 'No, I am not dead.'",
	"Incoming message for Player 1. The message is from 'Nadja'. It reads: 'Your actions are insidious and common!'",
	"Incoming message for Player 1. The message is from 'Nadja'. It reads: 'You're certainly the creation of a demented spirit.'",
	"Incoming message for Player 1. The message is from 'Nadja'. It reads: 'I want to face-lift you by force, to remove that stupid facial expression of yours.'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'Ey yo what you want here?'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'I finish you off if ye don't go away!'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'Look here, I'm showing you my asshole!'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'You can kiss my ass.'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'Now I'll genuinly finish you off fully.'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'Ey yo you're getting some now!'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'Ey yo you may not be here!'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'So you came here even though you may not, and therefore I'll fuck you fully in the ass now.'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'Somebody should call a medic for you!'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'I'll get you, and then I'll really give you a flogging!'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'I will kill you!'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'Ey yo you mother-fucker, I'll finish you off!'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'I'm gonna beat the shit out of you!'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'Fuck yourself!'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'Bah, you can just fuck yourself.'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'You're really an ass!'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'Stop being such an asshole!'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'Shut your fucking puss or I'll split your skull.'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'Ey if I get you in my fingers then are you dead.'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'Ey yo want some up your fat mouth?'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'May I fuck your sister?'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'Now I'm gonna JAM my thumb up your BUTT HOLE.'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'Better call an ambulance, you muck chap!'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'You're really looking like shit!'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'Ey yo you thought a bastard like you has any say?'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'Yes? Fuck you!'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'I'm gonna mutilate your corpse.'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'Nobody will recognize you anymore once I'm done with you.'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'Fuck you and shut up!'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'Ey yo you also know that you're a mother-fucker!'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'Ey yo I'm the king around here!'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'I'll help you get acquainted with my favorite tree, by slamming your skull into it.'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'Ey yo fuck you! FUCK YOU!'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'I will remove you from this world!'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'Your mother was a devil's whore!'",
	"Incoming message for Player 1. The message is from 'Juen'. It reads: 'Donnez-lui une chance, mais simplement parce qu'il ne peut pas, il ne veut pas dire qu'il ne pouvait pas apprendre maintenant!'",
	"Incoming message for Player 1. The message is from 'Annie'. It reads: 'Tout droit, asseyez-vous, mais vous avez besoin d'apprendre beaucoup, et rapidement!'",
	"Incoming message for Player 1. The message is from 'Annie'. It reads: 'Si vous etes le pire dans le premier examen de tous, vous devez quitter le parcours a nouveau!'",
	"Incoming message for Player 1. The message is from 'Sizzle'. It reads: 'Nethack is boring, why don't we play a game of Command & Conquer?'",
	"Incoming message for Player 1. The message is from 'Sizzle'. It reads: 'Am I sexy?'",
	"Incoming message for Player 1. The message is from 'Rhea'. It reads: 'As a journalist, I'm traveling a lot. I've been in Hawaii, but also in Afghanistan and Nepal...'",
	"Incoming message for Player 1. The message is from 'Urbaldi'. It reads: 'Screw the stupid gay school, I'll become a model!'",
	"Incoming message for Player 1. The message is from 'Fanny'. It reads: 'My buckled sandals killed half a dozen boys yesterday by slitting their legs until they died from blood loss.'",
	"Incoming message for Player 1. The message is from 'Nico'. It reads: 'You shit ass, I'll stab you!'",
	"Incoming message for Player 1. The message is from 'Miriam'. It reads: 'Do you agree that I look cute while strutting over the catwalk with my high-heeled lady boots?'",
	"Incoming message for Player 1. The message is from 'Miriam'. It reads: 'Of course it's worth mentioning that I'm wearing lady boots! There's not only gentlemen's boots to compare them to, but also little-girl boots, which wouldn't be my cup of tea since they're not elegant enough!'",
	"Incoming message for Player 1. The message is from 'Alice'. It reads: 'Stay here! If you can't be bothered to respect the teachers' instructions, I will bring you to the principal and he will decide your fate!'",
	"Incoming message for Player 1. The message is from 'Sue Lyn'. It reads: 'Okay, you may come with me if I go to the toilet, but you have to sexually please me. You have to massage my sexy butt cheeks non-stop while I'm taking a crap, or I will disfigure your face with my very sharp fingernails.'",
	"Incoming message for Player 1. The message is from 'Sue Lyn'. It reads: 'I want to hurt you with my fingernails. If you hold still, I'll just sting you repeatedly, but if you dare to do any move that I can interpret as 'trying to avoid my beautiful female nails' I'll start clawing at your skin like a beast.'",
	"Incoming message for Player 1. The message is from 'Sue Lyn'. It reads: 'If you get knocked out by my female thick winter boots, I'll be very angry. You have to endure the pain because I can't stand wimps at all!'",
	"Incoming message for Player 1. The message is from 'Juen'. It reads: 'My painted toenails will automatically try to scratch you if any exposed part of your body gets too close to them, so please be careful. I don't want you to get hurt.'",
	"Incoming message for Player 1. The message is from 'be silent kert'. It reads: 'You're expelled from the computer room for half a year and if I see someone play that Nethack shit ever again, I'll lock the room permanently for everyone.'",
	"Incoming message for Player 1. The message is from 'Saddam'. It reads: 'The Euro is not a Teuro, it just so happens that a pretzel which used to cost 1 Deutsche Mark costs 1 euro now.'",
	"Incoming message for Player 1. The message is from 'Mister Also Matte'. It reads: 'Got you, little rascal! Now you explain to me what you did there lately!'",
	"Incoming message for Player 1. The message is from 'Mister Also Matte'. It reads: 'I'm expecting a special package from my colleague. Do you have it? Give it to me!'",
	"Incoming message for Player 1. The message is from 'Mister Garbage Houses'. It reads: 'You are a stupid pupil asking stupid questions. I won't dignify you with an answer.'",
	"Incoming message for Player 1. The message is from 'Harro'. It reads: 'I will hold you, you for asshole!'",
	"Incoming message for Player 1. The message is from 'Miss Schrack'. It reads: 'I'll use my very fleecy black leather whip to punish you.'",
	"Incoming message for Player 1. The message is from 'Cristi', and it is sung to the tune of Yellow Submarine. It reads: 'Pla-yer-one-your-penisistoosmall, penisistoosmall, penisistoosmall. Pla-yer-one-is-homosexual, homosexual, homosexual.'",
	"Incoming message for Player 1. The message is from 'Marihuana'. It reads: 'Your chance is over now. You may not attend my school any longer. Pack your stuff and get lost! Now! Very far away from here! And never come back!'",
	"Incoming message for Player 1. The message is from 'Solvejg'. It reads: 'So you expected me to stop hurting you, now that your hand is bleeding? Hahaha. The fun is only starting now. I'll kick you repeatedly with my orange lady shoes and enjoy your blood squirting everywhere.'",
	"Incoming message for Player 1. The message is from 'Conny'. It reads: 'It's your choice: If you open the buckles of my high-heeled combat boots, they will scratch your legs with their zippers. Or if you pull open the zippers instead, the buckles will tear your skin apart.'",
	"Incoming message for Player 1. The message is from 'Thai'. It reads: 'I can kick you hospital-ripely!'",
	"Incoming message for Player 1. The message is from 'Conny'. It reads: 'If a guy behaves like an asshole, I don't feel bad about kicking him powerfully with my high-heeled combat boots. My conscience says that in this case he got it coming to him anyway.'",
	"You beareaucratic double-filer!",
	"Some watery tart throws Excalibur at you.",
	"Some watery tart decides to envelop your long sword, which vanishes in a black flash.",
	"You apparently overslept.",
	"Incoming message for Player 1. The message is from 'Juen'. It reads: 'Sorry for slitting your leg full length with my stiletto heel. I didn't mean it. Wait, let me use my first aid kit on you.'",
	"Weaum!",
	"The grunthack zombie spreads an airborne contagion. You feel deathly sick.",
	"The elder priest tentacles to tentacle you! Your bag of holding suddenly explodes, and all contents are destroyed!",
	"Your mana is absored.",
	"Magical reaction hurts you!",
	"You fall into the crystal water! Suddenly, gravity is reversed.",
	"You triggered an unfair attack trap, which allows monsters to randomly use any attack type in the game!",
	"You're standing on an elongation trap. As a consequence, monsters can now use their ranged attacks from infinitely far away.",
	"Your long sword attacks Berignac the invisible shopkeeper because you stepped on a bloodthirsty trap a while ago!",
	"You hear noise.",
	"You see noise.",
	"You see a pair of sweaty feet.",
	"You see an opening at the bottom of a wall and decide to put your toes through it, but suddenly something on the other side squashes your unprotected toes!",
	"You flee laughing!",
	"Attack re-selected.",
	"Attack de-selected.",
	"You lose the ability to attack in melee.",
	"The Ringwraith is IMPERVIOUS to the mundane weapon.",
	"Your weapon is destroyed!",
	"You cannot attack in this form!",
	"You cannot attack at all!",
	"Your hands stop glowing.",
	"Your hands stop existing. Now you cannot use weapons any longer.",
	"You must start walking the Straight Road from the startpoint.",
	"You must walk the Straight Road in correct order.",
	"You may not step off from the Straight Road.",
	"You slip on the icy floor.",
	"All of a sudden, a huge squad of monsters appears and ambushes you!",
	"You can't cross the chasm.",
	"You stop to avoid triggering the trap.",
	"You stepped on a known trap because you were stupid.",
	"You feel some rubble blocking your way.",
	"There is rubble blocking your way.",
	"There is a closed door blocking your way.",
	"You are about to leave a trap detected zone.",
	"You forgot that you have to press X to not die, and triggered a speed drain trap.",
	"The FTP daemon breathes megabytes! You feel very slow!",
	"You leave a trap detected zone.",
	"You leave a trap detected zone. You triggered a curse weapon trap! Your blessed rustproof +8 Excalibur is blasted by a terrible black aura and disintegrates!",
	"You found a trap!",
	"You cannot run in that direction.",
	"You are too confused to command your pets.",
	"You are too confused to play the game correctly, and as a result you keep making silly mistakes.",
	"You have no pets or companions.",
	"You have no friends, because nobody wants to have any business with the likes of you.",
	"You feel that AmyBSOD didn't put enough autocursing items in the game.",
	"Select the friendly monster:",
	"You cannot give orders to this monster.",
	"Select the target monster:",
	"This is not a correct target.",
	"This game is not dnethack, and therefore doesn't suffer from any horrible imbalances.",
	"This game is not Grunthack, but something less evil.",
	"This game is not FIQhack, but something where monsters cannot read scrolls of genocide. (Whose idea was that anyway??? I mean, seriously?)",
	"This game is not Nethack Fourk, but something that does not implement any of the more egregiously evil patch ideas. Only sane ones are in this variant.",
	"This game is not SLASHTHEM, but something buch better.",
	"This game is not SLASHTHEM, and therefore Amy's features didn't receive the dreaded 'removeremoveremove' treatment.",
	"This game is not SLASHTHEM, thank God. Who would want to play such an unplayable mess that deliberately reverts AmyBSOD's bugfixes despite the fact it's bloody obvious that my bugfixes are improving the game???",
	"You are already in a body.",
	"You are no longer in a body, and therefore cannot continue this game.",
	"Your body permanently stops obeying your commands. Have fun watching the AI take over. In order to advance turns, simply hit the space bar.",
	"You must select a corpse.",
	"You must eat a corpse.",
	"You must eat a tainted corpse and then spend the remaining game deathly sick. If you cure the sickness, or allow the timer to reach zero, you'll die.",
	"You have to complete ais523's Tower of Madness, which has been created by Satan and forces you to play through an entire level while deathly sick or you die automatically.",
	"Your spirit is incarnated in your new body.",
	"Your spirit is incarnated in a dnethack body. Please save your game and log in at the ascension.run server to play an ironman Binder. If you fail to ascend, your SLASH'EM Extended character dies automatically.",
	"You are already disembodied.",
	"You are already disembodied. The game should have been over already, but only now I realized that you're dead. Do you want your possessions identified?",
	"You are already disembodied. The asian girl's high heels are blood-smeared from mutilating your body.",
	"A cursed object is preventing you from leaving your body.",
	"A cursed object is preventing you from equipping the kickass weapon you just found.",
	"You do not manage to keep the corpse from rotting away.",
	"Your spirit leaves your body.",
	"The inscription shines in a bright light!",
	"The inscription shines in a bright light and disappears! In order to create another Elbereth, kindly use another charge from your wand of lightning.",
	"The inscription is enveloped in a dark aura!",
	"The inscription is enveloped in a dark aura! You actually engraved Morgoth instead of Elbereth, I don't know how you could typo that badly but it causes demons to appear out of thin air.",
	"The inscription releases a powerful storm!",
	"The inscription releases a powerful storm! You lost your house. This means that there is no longer a reason to escape the dungeon, so you really have to ascend or die in the attempt.",
	"A chasm appears in the floor!",
	"A chasm appears in the floor and swallows you!",
	"A chasm appears in the floor! You accidentally drop your blessed greased fixed +5 silver dragon scale mail into it!",
	"The monster simply flies over the chasm.",
	"The monster simply flies over the chasm and laughs: 'Neener-neener, catch me if you ca-an!'",
	"The monster falls in the chasm!",
	"The monster falls in the chasm, and sucks you into it too by using a vacuum cleaner!",
	"The inscription releases a blast of hellfire!",
	"You can't use this inscription for now.",
	"You start spinning around...",
	"You start spinning around... and suddenly crash into a wall.",
	"You start spinning around... and suddenly plummet into a pool of lethe water. Your amulet of life saving becomes a plain amulet!",
	"Press the space bar to continue",
	"You enter the arena briefly and bask in your glory.",
	"You are victorious. Enter the arena for the ceremony.",
	"You have won against all foes.",
	"Hey! You don't have the gold - get out of here!",
	"Hit any key to roll again",
	"I'll put you down for 0.",
	"Ok, I'll put you down for 9.",
	"The wheel spins to a stop and the winner is The Wizard of Yendor! Sorry player, you lose. Please start a new character.",
	"You came out a winner! We'll win next time, I'm sure.",
	"You lost gold! Haha, better head home.",
	"The barkeep gives you some gruel and a beer.",
	"You're a vampire and I don't have any food for you!",
	"The rooms are available only at night.",
	"The rooms are available only at night. Pick your girl: Juen will charge only $20 per night, Victoria is $100 and if you want Susanne you'll have to pay no less than $500.",
	"The rooms are available only during daylight for your kind.",
	"The rooms are available only during daylight for your kind, because no sane girl wants to sleep with you. But you're lucky, because Sing also works for vampires. One day with her will only cost you $50!",
	"You need a healer, not a room.",
	"You need a healer, not a room. I'd suggest Klara. She will charge $200 but once she's done treating your wounds, she'll be ready for hot sexy action with you, using her pointy white stiletto boots!",
	"You need a healer. I'll tell Ariane to come over to you, she's a highly professional nurse and knows how to hold your wounds open so it bleeds for a longer time.",
	"Sorry, but I don't want anyone dying in here.",
	"You awake refreshed for the new night.",
	"You awake refreshed for the new day.",
	"You already have been rewarded today.",
	"You already have been rewarded today. Since you insist on another reward, I'll summon an arch-lich. See whether he rewards you with a touch of death!",
	"You don't have anything appropriate.",
	"You don't have anything appropriate. All the clothes in your drawer look horrible! You can't get married like that! Go to Ahkatalapi's General Store on dungeon level 26 and buy a little black dress, or the woman of your dreams will look for another husband!",
	"Press space for more.",
	"Sorry, but that monster does not have a bounty on it.",
	"You have no right to choose that!",
	"You are not allowed to do that until you have paid back your loan.",
	"You do not have the gold!",
	"You didn't pay back your loan yet. Kops and enforcers have been summoned, as well as a ZAPM lawyer.",
	"Your possessions have been identified.",
	"Your possessions have been identified. DYWYPI?",
	"The hold of the Black Breath on you is broken!",
	"You just had your daily allowance!",
	"You just had your daily dose of sex! Victoria is exhausted now so please bugger off and don't annoy her anymore!",
	"You just had your daily dose of sex! Gudrun wants to see you again tomorrow though :-)",
	"You just had your daily dose of sex! Larissa casually struts away with her old worn-out plateau boots, but you know that you'll see her again tomorrow where you can get your shins kicked by her again!",
	"The air about you becomes charged...",
	"You know a little more of your fate.",
	"Well, you have no fate, but I'll keep your money anyway!",
	"You're fated to die on dungeon level 40.",
	"You already have a loan!",
	"You already have a loan, and you need to pay it back in 1000 turns! If you don't do so, the game will spawn 50 grandmaster mind flayers and 10 Wizards of Yendor to take your money by force.",
	"You see no quest level here.",
	"You see no building here.",
	"Are you mad? You want to anger the gods?",
	"Are you mad? You want to anger the gods? (Well, don't answer that.)",
	"You cannot do that.",
	"You smash into the fountain!",
	"You smash into the fountain! All your items became rusty and blanked!",
	"The fountain breaks!",
	"The fountain breaks! Water spurts out everywhere! You drown. Do you want your possessions identified?",
	"Some powerful force prevents your from teleporting.",
	"Some powerful force prevents your from knowing the English grammar rules.",
	"I see no up staircase here.",
	"You enter the previous area.",
	"You enter a maze of up staircases.",
	"You enter a maze of dumb-looking ASCII symbols, all alike.",
	"The space-time continuum can't be disrupted.",
	"You fall into the void.",
	"Brrrr! It's deadly cold.",
	"Some powerfull force prevents your from teleporting.",
	"I see no down staircase here.",
	"You deliberately jump through the trap door.",
	"You deliberately jump through the trap door, and find yourself in a void room. The truck ghost hits! The truck ghost hits! The truck ghost hits!--More--",
	"You enter the next area.",
	"You enter a maze of down staircases.",
	"You don't feel yourself experienced enough to go there...",
	"You cannot open chests.",
	"You have picked the lock.",
	"You failed to pick the lock.",
	"You cannot open doors.",
	"The door appears to be stuck.",
	"The door explodes! Careful: invisible Grunthack-style traps have been created everywhere around you.",
	"You have picked the lock.",
	"You see nothing there to open.",
	"There is a monster in the way!",
	"You cannot close doors.",
	"The door appears to be broken.",
	"You see nothing there to close.",
	"You see nothing there.",
	"You see nothing there to tunnel.",
	"You need to have a shovel or pick in your tool slot.",
	"You have cleared away the trees.",
	"You have cleared away the trees. The killer bees and black hornets who had their nests in them are very upset!",
	"You have finished the tunnel.",
	"You have finished the tunnel. As you enter it, it suddenly fills with water! Your chain mail rusts!",
	"You have found something!",
	"You have found something! It is a figurine of a lesbian girl named AmyBSOD!",
	"You have removed the rubble.",
	"You fail to make even the slightest of progress.",
	"You fail to make even the slightest of progress. Face it: a player like you just won't ascend this game.",
	"This will take some time.",
	"You cannot tunnel through doors.",
	"You cannot tunnel through air.",
	"You cannot tunnel through air. Only quantum mechanics can do so.",
	"I don't see any traps.",
	"I don't see any traps. That's because you triggered one that makes all future traps generate invisible.",
	"The chest is not trapped.",
	"The chest is not trapped. Really. I dare you to open it. :-P",
	"You have disarmed the chest.",
	"You failed to disarm the chest.",
	"You set off a trap!",
	"You set off a trap! Hahaha! Haha, ha!",
	"You disarm the monster trap.",
	"You set off the monster trap! It was a greater ugod, which suddenly comes alive!",
	"You see nothing there to disarm.",
	"You see nothing there to disarm. But remember, just because you cannot see it, doesn't mean there's no inventorylessness trap on that square...",
	"You smash into the door!",
	"The door crashes open!",
	"The door holds firm.",
	"A foul stench floods your nostrils!",
	"You bash the door. A foul stench floods your nostrils! Why that would happen from bashing a door, I don't know.",
	"Strange sweet liquid splashes onto you!",
	"Strange sweet liquid splashes onto you! It's sticky honey, and you hear the roaring of an angry bear!",
	"You are off-balance.",
	"You are off-balance. The erotic thai bitch topples you over, then stomps on your back with her very fleecy black block heels.",
	"You see nothing there to bash.",
	"You attack the empty air.",
	"You attack the empty air, but miss.",
	"You attack the empty air, but the atoms skillfully evade your blade!",
	"You see nothing there to spike.",
	"You have no spikes!",
	"You have no spikes! You're not a cactus after all!",
	"You jam the door with a spike.",
	"You are ambushed!",
	"You are ambushed! (insert Pokemon fight whoosh music here) A wild GIANT RAT appeared! What do you want to do? ATTACK, INVENTORY, SPELL, FLEE?",
	"You are too confused!",
	"Resting is impossible while undead!",
	"You have nothing with which to fire.",
	"You cannot fire with an instrument.",
	"You cannot fire with an instrument. (Why not? I'd expect it to fire musical notes at the enemy...)",
	"Hmmm, you seem to be unable to throw it.",
	"Hmmm, you seem to be unable to throw your piano. Maybe that's because it's too heavy, you retard?",
	"You cannot move in the overview display.",
	"There is no effect.",
	"The floor is impermeable.",
	"You sink through the floor.",
	"There is nothing above you but air.",
	"You rise through the ceiling.",
	"You can't use your powers yet.",
	"You can't use your powers yet. Hopefully you won't YASD before they become available.",
	"Your life slips away, and Melkor seems happier.",
	"Your life slips away, and your arms grow stronger.",
	"Your life slips away, and the evil powers grow stronger.",
	"That monster has no objects!",
	"The monster is guarding the treasures.",
	"Oops! The monster is now really *ANGRY*!",
	"The monster does not want your item.",
	"The monster does not want to chat.",
	"If they ever come up with a swashbuckling School, I think one of the courses should be Laughing, Then Jumping Off Something.",
	"It takes a big man to cry, but it takes a bigger man to laugh at that man.",
	"Dad always thought laughter was the best medicine, which I guess is why several of us died of tuberculosis.",
	"Ambition is like a frog sitting on a Venus Flytrap. The flytrap can bite and bite, but it won't bother the frog because it only has little tiny plant teeth. But some other stuff could happen and it could be like ambition.",
	"I'd rather be rich than stupid.",
	"We tend to scoff at the beliefs of the ancients. But we can't scoff at them personally, to their faces, and this is what annoys me.",
	"Probably the earliest flyswatters were nothing more than some sort of striking surface attached to the end of a long stick.",
	"As the evening sky faded from a salmon color to a sort of flint gray, I thought back to the salmon I caught that morning, and how gray he was, and how I named him Flint.",
	"When I was a kid my favorite relative was Uncle Caveman. After school we'd all go play in his cave, and every once in a while he would eat one of us. It wasn't until later that I found out that Uncle Caveman was a bear.",
	"Why do there have to be rules for everything? It's gotten to the point that rules dominate just about every aspect of our lives.  In fact, it might be said that rules have become the foot-long sticks of mankind.",
	"If I had a mine shaft, I don't think I would just abandon it. There's got to be a better way.",
	"It's amazing to me that one of the world's most feared diseases would be carried by one of the world's smallest animals: the real tiny dog.",
	"Sometimes life seems like a dream, especially when I look down and see that I forgot to put on my pants.",
	"I bet it was pretty hard to pick up girls if you had the Black Death.",
	"It's fascinating to think that all around us there's an invisible world we can't even see. I'm speaking, of course, of the World of the Invisible Scary Skeletons.",
	"Whenever I hear the sparrow chirping, watch the woodpecker chirp, catch a chirping trout, or listen to the sad howl of the chirp rat, I think: Oh boy! I'm going insane again.",
	"He was the kind of man who was not ashamed to show affection. I guess that's what I hated about him.",
	"The next time I have meat and mashed potatoes, I think I'll put a very large blob of potatoes on my plate with just a little piece of meat.",
	"It makes me mad when people say I turned and ran like a scared rabbit. Maybe it was like an angry rabbit, who was running to go fight in another fight, away from the first fight.",
	"Perhaps, if I am very lucky, the feeble efforts of my lifetime will someday be noticed, and maybe, in small way, they will be acknowledged as the greatest works of genius ever created by Man.",
	"Sometimes I think I'd be better off dead. No, wait, not me, you.",
	"If you ever catch on fire, try to avoid looking in a mirror, because I bet that will really throw you into a panic.",
	"Children need encouragement. If a kid gets an answer right, tell him it was a lucky guess. That way he develops a good, lucky feeling.",
	"The crows seemed to be calling his name, thought Caw.",
	"If your friend is already dead, and being eaten by vultures, I think it's okay to feed some bits of your friend to one of the vultures, to teach him to do some tricks. But only if you're serious about adopting the vulture.",
	"Broken promises don't upset me. I just think, why did they believe me?",
	"One thing vampire children have to be taught early on is, don't run with a wooden stake.",
	"Consider the daffodil. And while you're doing that, I'll be over here, looking through your stuff.",
	"I think my new thing will be to try to be a real happy guy. I'll just walk around being real happy until some jerk says something stupid to me.",
	"I hope some animal never bores a hole in my head and lays its eggs in my brain, because later you might think you're having a good idea but it's just eggs hatching.",
	"Whenever you read a good book, it's like the author is right there, in the room talking to you, which is why I don't like to read good books.",
	"What is it about a beautiful sunny afternoon, with the birds singing and the wind rustling through the leaves, that makes you want to get drunk?",
	"Instead of a trap door, what about a trap window? The guy looks out it, and if he leans too far, he falls out. Wait. I guess that's like a regular window.",
	"If I ever get real rich, I hope I'm not real mean to poor people, like I am now.",
	"Most of the time it was probably real bad being stuck down in a dungeon. But some days, when there was a bad storm outside, you'd look out your little window and think, 'Boy, I'm glad I'm not out in that.'",
	"I hope that after I die, people will say of me: 'That guy sure owed me a lot of money.'",
	"The tired and thirsty prospector threw himself down at the edge of the watering hole and started to drink. But then he looked around and saw skulls and bones everywhere. 'Uh-oh,' he thought. 'This watering hole is reserved for skeletons.'",
	"Anytime I see something screech across a room and latch onto someone's neck, and the guy screams and tries to get it off, I have to laugh, because what is that thing.",
	"I hope life isn't a big joke, because I don't get it.",
	"When you're going up the stairs and you take a step, kick the other leg up high behind you to keep people from following too close.",
	"I wonder if angels believe in ghosts.",
	"I don't understand people who say life is a mystery, because what is it they want to know?",
	"I think people tend to forget that trees are living creatures. They're sort of like dogs. Huge, quiet, motionless dogs, with bark instead of fur.",
	"Sometimes I think the world has gone completely mad. And then I think, 'Aw, who cares?' And then I think, 'Hey, what's for supper?'",
	"If a kid asks where rain comes from, I think a cute thing to tell him is 'God is crying'. And if he asks why God is crying, another cute thing to tell him is 'Probably because of something you did'.",
	"Contrary to popular belief, the most dangerous animal is not the lion or tiger or even the elephant. The most dangerous animal is a shark riding on an elephant, just trampling and eating everything they see.",
	"As I bit into the nectarine, it had a crisp juiciness about it that was very pleasurable- until I realized it wasn't a nectarine at all, but a HUMAN HEAD!!",
	"If you define cowardice as running away at the first sign of danger, screaming and tripping and begging for mercy, then yes, Mister Brave Man, I guess I am a coward.",
	"Blow ye winds, like the trumpet blows, but without that noise.",
	"The face of a child can say it all, especially the mouth part of the face.",
	"When I heard that trees grow a new 'ring' for each year they live, I thought, we humans are kind of like that; we grow a new layer of skin each year; and after many years we are thick and unwieldy from all our skin layers.",
	"It's too bad that whole families have been torn apart by something as simple as wild dogs.",
	"Even though he was an enemy of mine, I had to admit that what he had accomplished was a brilliant piece of strategy. First, he punched me, then he kicked me, then he punched me again.",
	"To me, truth is not some vague, foggy notion. Truth is real. And, at the same unreal. Fiction and fact and everything in-between, plus some things I can't remember, all rolled into one big 'thing'. This is truth, to me.",
	"If you're ever stuck in some thick undergrowth, in your underwear, don't stop and start thinking of what other words have 'under' in them, because that's probably the first sign of jungle madness.",
	"Sometimes the beauty of the world is so overwhelming, I just want to throw back my head and gargle. Just gargle and gargle, and I don't care who hears me, because I am beautiful.",
	"We used to laugh at Grandpa when he'd head off to go fishing. But we wouldn't be laughing when he'd come back with some whore he picked up in town.",
	"I think in one of my previous lives I was a mighty king, because I like people that do what I say.",
	"A man doesn't automatically get my respect. He has to get down in the dirt and beg for it.",
	"People think it would be fun to be a bird because you could fly. But they forget the negative side, which is the preening.",
	"When I think back on all the blessings I have been given in my life, I can't think of a single one, unless you count that rattlesnake that granted me all those wishes.",
	"Ash nazg durbatuluk, ash nazg gimbatul, ash nazg thrakatuluk agh burzum-ishi krimpatul.",
	"One Ring to rule them all, One Ring to find them, One Ring to bring them all, And in the darkness bind them.",
	"One fork to rule them all, One fork to find them, One fork to bring them all, And in Gehennom bind them.",
	"Oops! It feels deathly cold!",
	"Oops! It feels deathly cold! Hahaha, you were stupid enough to equip some cursed crap!",
	"Hmmm, it seems to be cursed.",
	"Hmmm, you seem to be unable to drop it.",
	"Hmmm, you seem to be unable to drop the loadstone.",
	"Hmmm, the opera cloak seems to be cursed.",
	"Hmmm, you seem to be unable to destroy it.",
	"Good merchants should not break anything...",
	"You see nothing special.",
	"That item had no inscription to remove.",
	"Inscription removed.",
	"You fuel your lamp.",
	"Your lamp is full.",
	"You combine the torches.",
	"Your torch is fully fueled.",
	"Your torch glows more brightly.",
	"You are not wielding a light.",
	"Your light cannot be refilled.",
	"You failed to sense the grid's mana.",
	"You have to wield a portable hole to use your abilities",
	"Failed!",
	"Noob!",
	"Wimp!",
	"Failure!",
	"Appended macros.",
	"Found no macro.",
	"Found a macro.",
	"Found a macro and automatically executed it. This cockatrice corpse tastes terrible! Do you want your possessions identified?",
	"Added a macro.",
	"Removed a macro.",
	"Appended keymaps.",
	"Found no keymap.",
	"Found a keymap.",
	"Added a keymap.",
	"Removed a keymap.",
	"Dumped monster attrchars.",
	"Dumped object attrchars.",
	"Dumped feature attrchars.",
	"Visual attrchar tables reset.",
	"Dumped color redefinitions.",
	"Someone forgot to describe this level!",
	"Looks like a typical quest level.",
	"You hear the sound of a market.",
	"Screen dump loaded.",
	"Screen dump saved.",
	"Starting macro recording, press this key again to stop. Note that if the action you want to record accepts the @ key, use it; it will remove your the need to inscribe stuff.",
	"Your wounds are polymorphed into less serious ones.",
	"Your wounds are polymorphed into mortal ones, and you have 5 turns to live.",
	"A new wound was created!",
	"A new wound was created by the sexy leather pump that keeps scratching up and down your legs!",
	"The buckled sandals keep slitting your legs full length and opening beautiful wounds!",
	"Your internal organs are rearranged!",
	"Your internal organs are rearranged! You no longer have a lung and therefore suffocate.",
	"You find living difficult in your present form!",
	"The Branding failed.",
	"You can't fetch when you're already standing on something.",
	"You can't fetch something that far away!",
	"There is no object at this place.",
	"You have no direct line of sight to that location.",
	"The object is too heavy.",
	"You make a high-pitched shriek!",
	"Oops! You call a monster.",
	"Oops! You call a monster. It is a thick farting girl.",
	"Oops! You call a monster. It is Demogorgon.",
	"Oops! You call a monster. It is dnethack's elder priest. And what did you call it? Why, names, of course!",
	"The dungeon collapses!",
	"The dungeon collapses! Hohaehaehaehaehae! Game over!",
	"Your shriek is so horrible that you damage your health!",
	"You have no powers you can use.",
	"No teleport on special levels...",
	"No teleport on special levels... and because someone turned on the Nethack Fourk switch, that applies to enemies you zap with your wand of teleportation, too.",
	"You go between.",
	"You fail to show the destination correctly!",
	"Your anti-magic field disrupts your magic attempts.",
	"This spell cannot be copied.",
	"You copy the spell!",
	"You feel your sanity slipping away!",
	"A black aura surrounds the corpse!",
	"You keep hold of your life force!",
	"You feel your life slipping away!",
	"You are hit by a gush of acid!",
	"You are hit by a gush of acid! Your magic lamp is destroyed!",
	"You receive a heavy shock!",
	"You receive a heavy shock! Your ring of conflict explodes!",
	"You receive a heavy shock! Your wand of death explodes! You are caught in a death field! You die...",
	"Roaring flames engulf you!",
	"Roaring flames engulf you! Your spellbook of finger of death burns to a crisp!",
	"Roaring flames engulf you! Your blessed scroll of genocide is destroyed!",
	"You are caught in a freezing liquid!",
	"You are caught in a freezing liquid! Your blessed potion of full healing shatters!",
	"You are surrounded by toxic gases!",
	"You are surrounded by toxic gases! The poison was deadly...",
	"A strange liquid splashes on you!",
	"A swirling cloud surrounds you!",
	"You are blasted by raw mana!",
	"You are blasted by raw mana! Your maximum mana was halved.",
	"Searing flames engulf the corpse!",
	"As you eat it you begin to feel your life flow getting stronger.",
	"That tastes good.",
	"That tastes very good.",
	"A fresh, clean essence rises, driving away wounds and poison.",
	"There is not enough meat.",
	"There is not enough meat. The damn vegetarians ate everything.",
	"There is not enough meat. You want to slaughter and eat some vegetarians to avoid starvation.",
	"Ugh! Raw meat!",
	"You feel rather sick.",
	"You feel rather sick, but not deathly sick, yet you don't feel all that healthy either.",
	"You quickly swallow the meat.",
	"You quickly swallow the meat. You cannibal! You will regret this!",
	"You quickly swallow the meat. The gods have nevertheless seen it and became angry.",
	"Mere victuals hold scant sustenance for a being such as yourself.",
	"Your hunger can only be satisfied with fresh blood!",
	"Sing's hunger can only be satisfied with fresh blood, and you look mighty tasty to her! Please remove any clothing that covers your neck, so her sharp teeth can bite you.",
	"The food of mortals is poor sustenance for you.",
	"Food is poor sustenance for you.",
	"You cannot split that.",
	"You hack some meat off the corpse.",
	"You feel less thirsty.",
	"You feel more thirsty.",
	"The potion makes you vomit!",
	"You wake up elsewhere with a sore head...",
	"You can't remember a thing, or how you got here!",
	"You feel your memories fade.",
	"You feel your memories fade. Who was Maud? And what happens if you bathe in the Lethe waters?",
	"Your nerves and muscles feel weak and lifeless!",
	"Your nerves and muscles feel weak and lifeless! Did the elder priest tentacle to tentacle you or what?",
	"Massive explosions rupture your body!",
	"A feeling of Death flows through your body.",
	"You feel life flow through your body!",
	"Your feel your head clear.",
	"An image of your surroundings forms in your mind...",
	"You begin to feel more enlightened...",
	"You begin to feel more enlightened... if you continue, you'll acquire intrinsic levitation.",
	"You begin to know yourself a little better...",
	"You begin to know yourself a little better, and suddenly understand why you want to be a lesbian woman...",
	"You feel the blood of life running through your veins!",
	"You feel the dark corruptions of Morgoth coming over you!",
	"The fountain is dried out.",
	"The fountain has dried up.",
	"You have no light by which to read.",
	"You feel the souls of the dead coming back from the Halls of Mandos.",
	"A message appears on the scroll. It says:",
	"The scroll disappears in a puff of smoke!",
	"There is a high-pitched humming noise.",
	"You feel as if someone is watching over you.",
	"Your hands begin to glow.",
	"The dungeon trembles...",
	"You must first pick up the staffs.",
	"You failed to use the staff properly.",
	"The staff has no charges left.",
	"You unstack your staff.",
	"You must first pick up the wands.",
	"You failed to use the wand properly.",
	"The wand has no charges left.",
	"This rod doesn't have enough mana for the rod tip.",
	"You can't zap a rod tip that's on the floor.",
	"You failed to use the rod properly.",
	"The rod does not have enough mana yet.",
	"A line of blue shimmering light appears.",
	"Cold winds begin to blow around you, carrying with them the stench of decay...",
	"Ancient, long-dead forms arise from the ground to serve you!",
	"The power of the ring destroys the world!",
	"You call the fire of Mount Doom!",
	"Your ring tries to take possession of your enemy's mind!",
	"Your bolts are covered in a fiery aura!",
	"Your nutsicles are covered in a fiery aura - AUUUUUGGGGGGHHHHH! This hurts even more than having them kicked with a stiletto heel!",
	"The fiery enchantment failed.",
	"You must wear it to activate it.",
	"You failed to activate it properly.",
	"It whines, glows and fades...",
	"You resume the development of the egg.",
	"The first spell is still charging!",
	"The second spell is still charging!",
	"You stop the development of the egg.",
	"Oops.  That object cannot be activated.",
	"The phial wells with dark light...",
	"The stone glows a deep green...",
	"You exterminate small life.",
	"You exterminate small life. But you forgot that your current race counts as 'small life', and therefore you die.",
	"The ring glows brightly...",
	"The ring glows deep red...",
	"The ring glows bright white...",
	"The ring glows deep blue...",
	"The ring glows intensely black...",
	"The stone reveals hidden mysteries...",
	"You are too weak to control the stone!",
	"Your armor is surrounded by lightning...",
	"Your armor is surrounded by lightning and disintegrates.",
	"You breathe the elements.",
	"You breathe the elements. The elemental beam bounces! The elemental beam hits you!",
	"Your armor glows many colours...",
	"Your armor glows many colours, and obtains the 'of hallucination' enchantment! Oh wow! Everything looks so cosmic!",
	"A heavenly choir sings...",
	"A heavenly choir sings, and you are bathed in radiance... Suddenly, a voice announces: 'Sorry, but your High Altar is on another Plane!'",
	"Your cloak glows many colours...",
	"Your cloak glows many colours and then disappears. It is now in the possession of a boss monster that was just spawned on this level.",
	"Your dagger is covered in frost...",
	"Your sword glows an intense white...",
	"You summon the Legion of the Dawn.",
	"Your morning star rages in fire...",
	"Your hammer glows white...",
	"Your crossbow glows deep red...",
	"Your mage staff glows deep blue...",
	"A Thunderlord comes from thin air!",
	"'I will burn you!'",
	"Your pick twists in your hands.",
	"A passage opens, and you step through.",
	"There is no wall there!",
	"Your drum shows you the world.",
	"Your horn glows deep red.",
	"Your horn emits a loud sound.",
	"Your horn calls for help.",
	"Your lochaber axe erupts in fire...",
	"Your spear is covered of darkness...",
	"Your hammer hits the floor...",
	"Your hammer tries to hit the floor, but lands on your feet instead...",
	"Your axe glows blood red...",
	"Your axe emits a black aura...",
	"Your weapon glows brightly...",
	"A line of sunlight appears.",
	"It glows extremely brightly...",
	"It throbs deep green...",
	"It is covered in sparks...",
	"It is covered in acid...",
	"It is covered in frost...",
	"It is covered in fire...",
	"It glows an intense red...",
	"It glows black...",
	"It glows an intense blue...",
	"It crackles with electricity...",
	"It grows magical spikes...",
	"It glows in scintillating colours...",
	"You launch a rocket!",
	"You launch a rocket! Dae-oe-oe-oe-oe-oe! Raeaeaeaeaeau! Tschaeauwieauw, well and!",
	"It floods the area with goodness...",
	"It floods the area with evil...",
	"The power of the artifact banishes evil!",
	"It lets out a long, shrill note...",
	"You summon a phantasmal servant.",
	"An elemental materialises...",
	"An elemental materialises... You fail to control it!",
	"It seems obedient to you.",
	"It seems obedient to you. But as soon as you turn your back on it, it will decapitate you. (And no, I won't tell you what it is.)",
	"The area fills with a stench of sulphur and brimstone.",
	"'NON SERVIAM! Wretch! I shall feast on thy mortal soul!'",
	"'What is thy bidding... Master?'",
	"Cold winds begin to blow around you, carrying with them the stench of decay...",
	"The dead arise... to punish you for disturbing them!",
	"It radiates deep purple...",
	"You feel a warm tingling inside...",
	"It lets out a shrill wail...",
	"It wells with clear light...",
	"It pulsates...",
	"Not on special levels!",
	"You open a Void Jumpgate. Choose a destination.",
	"You fail to exit the void correctly!",
	"It twists space around you...",
	"It glows soft white...",
	"Ahah, you wish.",
	"The light around you is absorbed... and released in a powerful bolt!",
	"You can't activate this when it's there!",
	"The ring explodes into a space distortion.",
	"You twirl and spores fly everywhere!",
	"Oh, no! Your mind has gone blank!",
	"Weird visions seem to dance before your eyes...",
	"Your brain is addled!",
	"Your mind unleashes its power in an uncontrollable storm!",
	"You feel safe.",
	"Mind-warping forces emanate from your brain!",
	"A wave of pure physical force radiates out from your body!",
	"Zap?",
	"You faint from the effort!",
	"You have damaged your mind!",
	"You morph back to your natural form!",
	"You are not wearing any cloaks of mimicry.",
	"You feel uneasy with this shape-change.",
	"Your shape-change goes horribly wrong!",
	"Your shape-change goes horribly right! Now you're Frankenstein's Monster!",
	"You manage to wrest your body back under control.",
	"You had best not travel with your extra limbs.",
	"You mimic a new pair of legs.",
	"You grow an affinity for walls.",
	"You grow an affinity for walls. Hopefully you can find a wall that lets you fuck it.",
	"You mimic a new pair of arms.",
	"You can't summon more pets",
	"This code should never be hit!",
	"Since the code to actually let you select one isn't here",
	"You will automatically get the activation 'Dawn'",
	"This option is not available",
	"Not enough experience for the flags you've selected.",
	"You cannot do that - you don't know how!",
	"Not enough experience.  Decrease power or deselect flags.",
	"error: this code can't ever be hit!",
	"Creating an artifact will result into a permanent loss of 10 hp.",
	"Not enough energy to enchant more than one object!",
	"Attempting to display too many items!",
	"You don't know any recipes!",
	"You can't be an alchemist without recipes!",
	"You don't know of anything you can make using that.",
	"Coding error: tval != TV_POTION",
	"You recall your old master teaching you about elemental item infusing.",
	"There's gotta be an easier way to get into all these vaults!",
	"You recall your old master reminiscing about legendary infusings",
	"You wonder about shocking daggers of slay evil.",
	"It suddenly occurs to you that artifacts don't *HAVE* to be permanent...",
	"You must wear gloves in order to do alchemy.",
	"You do not have enough essences.",
	"You cannot extract essences after it's been magically recharged.",
	"You cannot extract anything from that item.",
	"While destroying it, you gain insight into this item.",
	"You don't have the essences to recharge this item.",
	"Pray hard enough and your prayers might be answered.",
	"This place seems devoid of life.",
	"You called, but no help came.",
	"You don't currently own a body to use.",
	"You don't currently own a body to use. And your own body hates you and wishes you were dead.",
	"You lose control of your body!",
	"You are forced back into your body by your cursed items, you suffer a system shock!",
	"You make some ammo.",
	"Oh, no! You become undead!",
	"Oh, no! You become undead! Apparently you tried to cast the turn undead spell!",
	"Suddenly you feel that you're in a bad situation...",
	"Your body is damaged by the horrible forces of the spell!",
	"You have not selected a second rune!",
	"You have no mana!",
	"There are no runespells you can cast.",
	"You lack some essential rune(s) for this runespell!",
	"You have already learn the maximun number of runespells!",
	"You must have at least a level 20 antimagic skill to be able to disrupt the magic continuum.",
	"You stop disrupting the magic continuum.",
	"You start disrupting the magic continuum.",
	"You cannot use your detection abilities yet.",
	"You failed to extract a totem.",
	"You extract a totem from the dead corpse.",
	"You extract a totem from the living corpse. (I don't care that 'living corpse' makes no sense.)",
	"The summoning fails due to lack of room.",
	"The summoning fails.",
	"Hypnotise which pet?",
	"You can only hypnotise monsters that cannot move.",
	"You can only hypnotise pets and companions.",
	"You cannot hypnotise this monster.",
	"There is no pet here !",
	"There is no pet here unnecessary space!",
	"You are not in symbiosis.",
	"You make some boulders.",
	"You make some boulders. One of them bounces off your hard helmet. Repeat 59 times. --More-- You die.",
	"This section of the Straight Road looks less powerful.",
	"You cannot find out anything more about that.",
	"You stop laying a mana path.",
	"You stop expulsing mana winds.",
	"You stop your spell.",
	"The gates to Middle Earth are closing...",
	"Please finish up and/or save your game.",
	"The gates to Middle Earth are now closed.",
	"The sun has risen.",
	"The sun has set.",
	"You should pay your loan...",
	"You should pay your loan, otherwise Kopasker the shopkeeper will get *really* angry...",
	"The sun's rays scorch your undead flesh!",
	"*** LOW DEATHPOINT WARNING! ***",
	"You lose your extra pair of legs.",
	"You lose your extra pair of arms.",
	"You lose your affinity to walls.",
	"You cannot breathe water!  You suffocate!",
	"There is no air there!  You suffocate!",
	"Your light has gone out!",
	"Your light is growing faint.",
	"Teleport aborted.",
	"Your egg hatches!",
	"An egg hatches!",
	"You cannot recall from here.",
	"As an astral being you can't recall.",
	"The recall is cancelled by a powerful magic force!",
	"You feel yourself yanked upwards!",
	"You feel yourself yanked downwards!",
	"You are too hungry to travel.",
	"You can't travel during the day!",
	"Travel with your present light would be unsafe.",
	"You are too injured to travel.",
	"To flee the ambush you have to reach the edge of the map.",
	"Your light is too low on fuel for you to travel with it.",
	"The monsters have been waiting for you, and now you're ambushed. Bwarharharharhar!",
	"The arena absorbs all attempted magic!",
	"You're in the arena now. This is hand-to-hand!",
	"You see no fountain here.",
	"You cannot worship gods.",
	"Cmovie recording stopped.",
	"You feel different!",
	"Your pack overflows!",
	"Well done ! You reached the town ! You can now go down again.",
	"You have been saved by the Blood of Life!",
	"A tension leaves the air around you...",
	"Too many macro triggers!",
	"Character sheet creation successful.",
	"Score file unavailable.",
	"Hit any key to continue",
	"Death save failed!",
	"Warning! Could not place streamer!",
	"Destroyed Level",
	"Warning! Could not place vault object!",
	"Warning! Could not place vault trap!",
	"Could not allocate this vault here",
	"Warning! Could not place lesser vault!",
	"Refusing a greater vault.",
	"Sand vein.",
	"River of water.",
	"WARNING: Could not place guardian.",
	"Player Ghost.",
	"A 'size' dungeon level.",
	"Warning -- deleted corrupt 'ghost' file!",
	"Don't be silly; you don't believe in gods.",
	"You feel the dark powers of Melkor in you.  You can now use the Udun skill.",
	"You feel the fleecy colors of AmyBSOD in you.  You can now use the Udun skill.",
	"You don't worship anyone.",
	"duplicate f: entries for one corpse",
	"f: section for corpse flags only",
	"Can't specify r_info.txt index with f: section",
	"Limit on race flags is currently 6",
	"Activate flag without activate type",
	"Ignoring obsolete/defective 'f_info.raw' file.",
	"Savefile is currently in use.",
	"You feel the dark powers twisting your body!",
	"You feel the dark powers trying to twisting your body, but they fail.",
	"You are covered in acid!",
	"You are struck by electricity!",
	"You are enveloped in flames!",
	"You are covered with frost!",
	"You stand your ground!",
	"You are unaffected!",
	"You resist the effects!",
	"Energy drains from your pack!",
	"You quickly protect your money pouch!",
	"Nothing was stolen.",
	"You grab hold of your backpack!",
	"Moving object to black market...",
	"Your light dims.",
	"The thief flees laughing!",
	"You hear something appear nearby.",
	"You are engulfed in a whirlpool.",
	"You feel something focusing on your mind.",
	"You disbelieve the feeble spell.",
	"You hear many things appear nearby.",
	"You hear heavy steps nearby.",
	"You hear many creepy things appear nearby.",
	"You hear many powerful things appear nearby.",
	"You hear immortal beings appear nearby.",
	"The rune of protection is broken!",
	"There is a grinding sound.",
	"You hear a door burst open!",
	"The rune explodes!",
	"An explosive rune was disarmed.",
	"You overcome your fate, mortal!",
	"Compacting monsters...",
	"Too many monsters!",
	"Your sanity is shaken by reading the Necronomicon!",
	"You are getting insane!",
	"You feel much less sane than before.",
	"You forget everything in your utmost terror!",
	"You let out a bloodcurdling scream of fear!",
	"You are startled.",
	"You drop all your money in terror!",
	"WARNING: Refused monster: cannot cross terrain",
	"WARNING: Refused monster: NEVER_GENE",
	"WARNING: Refused monster: unique already on saved level",
	"WARNING: Refused monster: unique already on saved level. We don't tell you this, so you keep grinding Angband:99 even though Sauron is still waiting on Mount of Doom.",
	"WARNING: FORCE_DEPTH",
	"Warning! Could not allocate a new monster. Small level?",
	"Monster horde.",
	"It tries to breed but it fails!",
	"You pick up all objects on the floor.",
	"You pick up all objects on the floor. Your movements are slowed slightly by your load.",
	"You add the ammo to your quiver.",
	"We've been tricked!",
	"(breakage)",
	"(no floor space)",
	"(too many objects)",
	"You feel something roll beneath your feet.",
	"You carefully drop the poor monster on the floor.",
	"You combine some items in your pack.",
	"You reorder some items in your pack.",
	"You reorder some items in your pack, even though you never asked for that to happen. But the game helpfully decided to make it so that everything is in different slots now.",
	"You've failed to concentrate hard enough.",
	"You cannot convert this monster.",
	"This chest may be trapped.",
	"This chest is broken.",
	"This chest is full.",
	"You can't put a chest into itself.",
	"You can't put a chest into another one.",
	"The item disappeared!",
	"The item disappeared! Hoehoehoehoe, we don't tell you which one, so you may check your entire inventory and all the 20 pages of your bag of holding.",
	"You sense the world around you.",
	"You cook some food.",
	"You play tough.",
	"RAAAGH!",
	"You carefully set an explosive rune...",
	"You bash at a stone wall.",
	"You bash at a stone wall. Ouch! That hurts!",
	"You flash a bright aura.",
	"You flash a bright aura. Everything suddenly goes dark.",
	"You throw a dart of poison.",
	"You throw a dart of poison. But you threw it upwards, and it falls back down on your head.",
	"You examine your surroundings.",
	"You enter the straight road and fly beside the world. Where to exit?",
	"No recall on special levels..",
	"You are already in town!",
	"You make the trees grow!",
	"You bite into thin air!",
	"You grin and bare your fangs...",
	"You were not hungry.",
	"You were not hungry, but now you are.",
	"Yechh. That tastes foul.",
	"You emit an eldritch howl!",
	"You attempt to restore your lost energies.",
	"You attempt to restore your lost energies. But, like always when you try to do something, you fail horribly.",
	"Your eyes look mesmerising...",
	"Your eyes look mesmerising... and you were stupid enough to look into a mirror. You are freaked out.",
	"Radiation flows from your body!",
	"Radiation flows from your body! You're the Radioactive Man now!",
	"Ouch!  This wall is harder than your teeth!",
	"There's something in the way!",
	"You don't like the woody taste!",
	"This granite is very filling!",
	"You can't absorb energy from a discharged rod.",
	"There's no energy there to absorb!",
	"You suddenly have a headache!",
	"You don't see any monster in this direction.",
	"You sense no evil there!",
	"You sense no evil there, but I'll tell you now that Anastasia is invisible and will suck off your head with her vacuum cleaner.",
	"The evil creature vanishes in a puff of sulphurous smoke!",
	"Your invocation is ineffectual!",
	"You wave your hands in the air.",
	"Not enough enough energy to enchant more than one object!",
	"lua_stack:",
	"You don't have any activable skills or abilities.",
	"You have gained one new thaumaturgy spell.",
	"Uh?",
	"You need a skill level of at least 12.",
	"Your roots prevent the recall.",
	"The spellbinder is triggered!",
	"The attack penetrates your shield of invulnerability!",
	"You turn into an unthinking vegetable.",
	"*** LOW SANITY WARNING! ***",
	"Your body starts to scramble...",
	"The vein turns into mud!",
	"There is a searing blast of light!",
	"A demon emerges from Hell!",
	"Your psychic energy is drained!",
	"You dodge a magical attack!",
	"Something bounces!",
	"The attack bounces!",
	"You are hit by pure death!",
	"Daedaedaedaedaedaeueueueueueuei!",
	"You are hit by something sharp!",
	"You are hit by something sharp! Maybe it's a high-heeled lady shoe?",
	"You are hit by something wet!",
	"You are hit by a wave of anarchy!",
	"You are hit by a loud noise!",
	"You are hit by a loud noise, even though that should be unable to hit you physically!",
	"You are hit by something puzzling!",
	"You are hit by something puzzling! Maybe it was a ray of thoughts, which should actually be immaterial?",
	"You are hit by something static!",
	"You are hit by kinetic force!",
	"You are hit by kinetic force, and pushed into the nearby lava!",
	"There is an explosion!",
	"You are hit by something slow!",
	"You are hit by something slow, because you were such a lame slug and failed to move out of the way even though you really had plenty of time.",
	"The light scorches your flesh!",
	"The light forces you out of your incorporeal shadow form.",
	"You are hit by a blast from the past!",
	"You feel as if time is passing you by.",
	"You are hit by something heavy!",
	"You are hit by something heavy! It was a massive block of lead that squished you flat. You die.",
	"You are hit by something invigorating!",
	"You are hit by an aura of magic!",
	"Something falls from the sky on you!",
	"Something falls from the sky on you! An airplane!",
	"You are hit by something sharp and cold!",
	"You are hit by pure knowledge!",
	"You are hit by pure psionic energy!",
	"You are hit by something paralyzing!",
	"You are hit by pure anti-death energy!",
	"You are hit by pure protection!",
	"You are hit by pure protection, which actually sounds beneficial!",
	"You are knocked back!",
	"You are knocked back into lethe water!",
	"You are knocked back and crushed!",
	"You are severely crushed!",
	"You feel a little better.",
	"You feel a little better, but you're still deathly sick.",
	"The object resists the spell.",
	"You feel your life energies returning.",
	"You sense the presence of traps!",
	"You sense the presence of ways out of this area!",
	"You sense the presence of buried treasure!",
	"No stair creation in non dungeons...",
	"No stair creation on special levels...",
	"The curse is broken!",
	"It looks muffled.",
	"It makes you see tiny red animals.",
	"Gravel flies from it!",
	"It seems faster!",
	"It looks totally incorruptible.",
	"It looks completely grounded.",
	"It feels very warm.",
	"It feels very cool.",
	"It makes your stomach rumble.",
	"It makes breathing easier for you.",
	"It makes everything look darker.",
	"It makes you feel you are wearing glasses.",
	"It makes you feel deaf!",
	"It makes you feel like visiting a graveyard!",
	"It makes you feel normal.",
	"It is surrounded by a static feeling.",
	"It makes you feel you cannot become more stupid.",
	"It makes you feel you cannot become more stupid, because you're already more stupid than a rock.",
	"It makes you feel you cannot become uglier. Seriously, you're already more ugly than a moth-bitten corpse.",
	"It starts shining.",
	"It feels lighter.",
	"It makes you hear voices inside your head!",
	"It looks as good as new.",
	"Its position feels uncertain!",
	"You start hating animals.",
	"You start hating animals. Especially cats.",
	"You start hating animals. Dogs are the worst, because they'll shit on the pavements and then you run the risk of stepping into it.",
	"It looks extremely sharp!",
	"It looks extremely sharp! It is a very dangerous Katie Melua leather boot whose stiletto heel requires a weapon light!",
	"The ground trembles beneath you.",
	"The ground trembles beneath you. Be glad I told you that you're on the vibrating square or you'd never have found it.",
	"Ouch! You get zapped!",
	"Its smell makes you feel dizzy.",
	"Its smell makes you feel dizzy. Why did you enter the shower room shortly after a woman used it, anyway?",
	"It smells rotten.",
	"You think it bit you!",
	"It looks very confusing.",
	"It looks mightier than before.",
	"The recharge backfires, draining the rod further!",
	"You feel a sudden stirring nearby!",
	"You hear a sudden stirring in the distance!",
	"You hear a sudden stirring in the distance! This message means that a spectral anaconda has been spawned, which is way out of depth for this dungeon level!",
	"You must select a monster.",
	"You must select a monster on the genocide prompt. If you don't, you automatically genocide yourself.",
	"A mysterious force stops the genocide.",
	"Probing...",
	"That's all.",
	"The cave ceiling collapses!",
	"The cave floor twists in an unnatural way!",
	"The cave quakes!  You are pummeled with debris!",
	"You nimbly dodge the blast!",
	"You are bashed by rubble!",
	"You are crushed between the floor and ceiling!",
	"You can't trade places with that!",
	"Your teleportation is blocked!",
	"You feel the coldness of the Black Breath attacking you!",
	"Woah! You see 10 little Morgoths dancing before you!",
	"Huh? Who am I? What am I doing here?",
	"Your weapon now seems useless...",
	"Oh! You attracted some evil Thunderlords!",
	"The artifact resists your blessing!",
	"There is a static feeling in the air...",
	"You sense the presence of unnatural beings!",
	"Hmm, blinking there will take time.",
	"You will soon be recalled.",
	"You emerge in the wall!",
	"Never heard of that place!",
	"This place blocks my magic!",
	"You cannot go there yet!",
	"The Morgothian Curse withers away.",
	"The shopkeeper whispers something into your ear:",
	"No discount on random artifacts.",
	"Invalid response.",
	"You eventually agree upon the price.",
	"You quickly agree upon the price.",
	"You instantly agree upon the price.",
	"You can't steal from your home!",
	"There is no item to steal.",
	"You cannot carry that many different items.",
	"You cannot carry that many items.",
	"The shopkeeper retires.",
	"The shopkeeper brings out some new stock.",
	"You cannot take items from the museum!",
	"Your home is empty.",
	"I am currently out of stock.",
	"You do not have enough gold to buy one.",
	"Putting it into your home has extra-dimensional problems",
	"I have not the room in my store to keep it.",
	"Once you donate something, you cannot take it back.",
	"You have no special knowledge about that item.",
	"Entire inventory is shown.",
	"That command does not work in this store.",
	"You see no store here.",
	"The doors are locked.",
	"Your pack is so full that you flee the store...",
	"The item has arrived in the Black Market.",
	"There isn't enough room for it.",
	"You hear something move.",
	"Suddenly the cave shifts around you. The air is getting stale!",
	"As you touch the trap, the ground starts to shake.",
	"You prick yourself on a poisoned needle.",
	"You prick yourself on a needle.",
	"A spell hangs in the air.",
	"A mighty spell hangs in the air.",
	"An old and evil spell hangs in the air.",
	"The world whirls around you.",
	"You touch a poisoned part and can't move.",
	"A hidden explosive device explodes in your face.",
	"You suddenly don't remember what you were doing.",
	"You feel an alien force probing your mind.",
	"An age-old and hideous-sounding spell reverberates off the walls.",
	"A wretched-smelling gas cloud upsets your stomach.",
	"A powerful magic protected this.",
	"You hear a hollow noise echoing through the dungeons.",
	"Your backpack seems to vibrate strangely!",
	"You sense a great loss.",
	"Your head feels dizzy for a moment.",
	"You feel something touching you.",
	"You feel something touching you. It is a pair of female hands that wants to stroke-feel up you.",
	"All of your coins were stolen!",
	"A small fire works its way through your backpack. Some scrolls are burnt.",
	"The fire hasn't finished.",
	"You feel the air stabilise around you.",
	"You feel like staying around.",
	"You somehow feel like another person.",
	"You feel a lack of useful items.",
	"You feel as if you had a nightmare!",
	"You remember having a nightmare!",
	"You have a vision of a powerful enemy.",
	"You are pushed around!",
	"Your backpack seems to be turned upside down.",
	"You hear a wail of great disappointment.",
	"You have a feeling that this trap could be dangerous.",
	"Executing moving stairs trap on level with no stairs!",
	"You see some stairs move.",
	"You see a stair move.",
	"You hear distant scraping noises.",
	"You hear a noise, and then its echo.",
	"You notice something falling off the trap.",
	"You feel light-footed.",
	"You hear a growling sound.",
	"You have lost trust in your backpack!",
	"You hear an echoing cry of rage.",
	"The floor vibrates in a strange way.",
	"You feel some things in your pack vibrating.",
	"You suddenly feel you have time for self-reflection.",
	"You hear a sudden, strange sound.",
	"Gas sprouts out... you feel yourself transmute.",
	"Colors are scintillating around you. You see your past running before your eyes.",
	"Heavy fumes sprout out... you feel yourself transmute.",
	"Scintillating colors hypnotise you for a moment.",
	"You don't dare to set a trap in the darkness.",
	"You cannot set a trap on this.",
	"Unknown trapping kit type!",
	"oops! nonexistant trap!",
	"Unknown timer!",
	"Cannot create spoiler file.",
	"Successfully created a spoiler file.",
	"Allocated.",
	"Changes accepted.",
	"That is not a valid debug command.",
	"Your covered hands feel unsuitable for spellcasting.",
	"Your hands feel more suitable for spellcasting.",
	"The weight of your armor encumbers your movement.",
	"You feel able to move more freely.",
	"You have trouble wielding such a heavy bow.",
	"You have no trouble wielding your bow.",
	"You feel relieved to put down your heavy bow.",
	"You have trouble wielding such a heavy weapon.",
	"You have no trouble wielding your weapon.",
	"You feel relieved to put down your heavy weapon.",
	"You do not feel comfortable with your weapon.",
	"You feel comfortable with your weapon.",
	"You feel more comfortable after removing your weapon.",
	"The weight of your armor disrupts your balance.",
	"You regain your balance.",
	"More of your prophecy has been unearthed!",
	"You should see a soothsayer quickly.",
	"You feel something growing in you.",
	"You feel something growing in you. Apparently you're going to have a baby...",
	"The hideous thing growing in you seems to die.",
	"The hideous thing growing in you seems to die. Well done, now that goddamn baby is dead!",
	"Your weapon starts glowing.",
	"Your weapon starts glowing and will selfdestruct in 20 turns.",
	"Roots dive into the floor from your feet.",
	"The roots of your feet suddenly vanish.",
	"Air seems to fill your lungs without breathing.",
	"Water seems to fill your lungs.",
	"You need to breathe again.",
	"The water filling your lungs evaporates.",
	"You start absorbing the souls of your foes.",
	"You feel invulnerable.",
	"You are more vulnerable.",
	"You feel instable.",
	"You feel your body fade away.",
	"You are no longer invisible.",
	"Your hands are dripping with venom.",
	"Your hands are dripping with venom. Every potion you touch will turn into a potion of sickness.",
	"The venom source dries out.",
	"You are suddenly much less accurate.",
	"You are suddenly heavier.",
	"You are suddenly heavier. Did you eat a lot lately?",
	"You feel able to reach the clouds.",
	"You start meditating on yourself...",
	"You stop your self meditation.",
	"You stop your self meditation. After all, ascension is something you have to work for - just sitting around wanking off won't help!",
	"You start reflecting the world around you.",
	"You stop reflecting.",
	"You stop reflecting. Suddenly disintegration beams are very dangerous!",
	"You are now protected against space-time distortions.",
	"You are no longer very accurate.",
	"You feel protected against light's fluctuation.",
	"You are no longer transformed.",
	"You are no longer bright.",
	"Your mystic shield crumbles away.",
	"The prayer has expired.",
	"The holy aura vanishes.",
	"You feel strangely buoyant!",
	"You feel much less buoyant.",
	"You feel like a killing machine!",
	"You no longer feel safe from evil.",
	"You feel safe from good!",
	"You leave the physical world and turn into a wraith-being!",
	"You feel opaque.",
	"Invulnerability!",
	"The invulnerability wears off.",
	"The air around you charges with lightning!",
	"The air around you discharges.",
	"Your eyes begin to tingle!",
	"Your body regenerates much more slowly.",
	"You have been stunned.",
	"You have been heavily stunned.",
	"You have been knocked out.",
	"You have been knocked out by Nelly's lady pumps. Now she starts scratching up and down your legs with your heels while you're unable to fight back.",
	"A vicious blow hits your head.",
	"You have been given a graze.",
	"You have been given a nasty cut.",
	"You have been given a deep gash.",
	"You have been given a mortal wound.",
	"You have been horribly scarred.",
	"You are still weak.",
	"You have gorged yourself!",
	"You are getting faint from hunger!",
	"You feel different...",
	"Your death has been avenged -- you return to life.",
	"Victorious! You're on your way to becoming Champion.",
	"A new warrior steps forth!",
	"A blue horror appears!",
	"A magical staircase appears...",
	"'Thou needst a new form, mortal!'",
	"'Thou didst not deserve that, slave.'",
	"'Thou art growing arrogant, mortal.'",
	"'My pets, destroy the arrogant mortal!'",
	"'Death and destruction! This pleaseth me!'",
	"'Stay, mortal, and let me mold thee.'",
	"'I grow tired of thee, mortal.'",
	"'Thou needst a lesson in humility, mortal!'",
	"'Suffer, pathetic fool!'",
	"'Thou reliest too much on thy weapon.'",
	"'Thou reliest too much on thine equipment.'",
	"'Now thou shalt pay for annoying me.'",
	"'Die, mortal!'",
	"Nobody ever turns up...",
	"You can't wish for a wish!",
	"Your wish becomes truth!",
	"Your wish becomes truth! Victoria really appeared in person before you, and you come so hard at the sight of her beautiful brown leather boots with black block heels.",
	"Incoming message for Player 1. The message is from 'Wendy'. It reads: 'Leave my pink high heels alone!'",
	"Incoming message for Player 1. The message is from 'Wendy'. It reads: 'My pink high heels will crush your fingers!'",
	"Incoming message for Player 1. The message is from 'Inge'. It reads: 'My butt cheeks are capable of producing fart gas that works like pepper spray, causing people to choke. It's useful for getting rid of guys who try to hit on me.'",
	"Incoming message for Player 1. The message is from 'Max'. It reads: 'You wanker, this dungeon is my property! Get the hell out of here!'",
	"Incoming message for Player 1. The message is from 'Antonia'. It reads: 'I only got a movement speed of 6 because I broke one of my legs, which forces me to walk with crutches.'",
	"Incoming message for Player 1. The message is from 'Sabrina'. It reads: 'The water at the memorial must be bad! It looks incredibly clean, yet it felt really strange when I drank from it!'",
	"Incoming message for Player 1. The message is from 'Julia'. It reads: 'My combat boots are fleecy-soft!'",
	"Incoming message for Player 1. The message is from 'Flo-ooo'. It reads: 'Err, I don't really know...'",
	"Incoming message for Player 1. The message is from 'Paul'. It reads: 'Ey yo you mother-fucker!'",
	"Incoming message for Player 1. The message is from 'Magdalena'. It reads: 'I'll scratch both of your legs and you will bleed a lot. My combat boot heels are really looking forward to it, too.'",
	"Incoming message for Player 1. The message is from 'Juen'. It reads: 'You made the mistake of insulting my girlfriend. If you don't apologize with her right away, my combat boot heels will make the blood flow like rivers from your legs.'",
	"Incoming message for Player 1. The message is from 'Cristi'. It reads: 'Ranged weapons are stupid. I always use my trusty sword.'",
	"Incoming message for Player 1. The message is from 'Cristi'. It reads: 'Why would I use some retarded gun to shoot my enemy? I'm a melee fighter! Give me a sword and everything's alright! The only thing a gun is good for is to whack enemies with it, but swords are more effective!'",
	"Incoming message for Player 1. The message is from 'Rita'. It reads: 'I have a very long bundle.'",
	"Incoming message for Player 1. The message is from 'Rita'. It reads: 'Tell me that I look like a model!'",
	"Incoming message for Player 1. The message is from 'Sandra'. It reads: 'I have a swarming swing bundle :-)'",
	"Incoming message for Player 1. The message is from 'Antonia'. It reads: 'I'd like to be able to run, but I have to be glad that the crutches allow me to walk at all.'",
	"Incoming message for Player 1. The message is from 'Johanetta'. It reads: 'What, you're calling me a 'would-be noble'? But I'm really a noble!'",
	"Incoming message for Player 1. The message is from 'Johanetta'. It reads: 'If you call me a 'would-be noble' one more time, I'll imprison you in my castle's dungeon.'",
	"Incoming message for Player 1. The message is from 'Susanne'. It reads: 'I want to kick your head with my black block heels!'",
	"Incoming message for Player 1. The message is from 'Arne'. It reads: 'Newcomers? No idea!'",
	"Incoming message for Player 1. The message is from 'Hailbush'. It reads: 'Please don't step on my fingers with your high heels!'",
	"Incoming message for Player 1. The message is from 'Martin'. It reads: 'Whuh? I don't understand anything!'",
	"Incoming message for Player 1. The message is from 'H.'. It reads: 'You're dead and so are your children.'",
	"Incoming message for Player 1. The message is from 'H.'. It reads: 'You're dead! Your family is dead! Your friends are dead! I'm gonna FUCK you up and your children and your grand children.'",
	"Incoming message for Player 1. The message is from 'Solvejg'. It reads: 'My voice and the stench that emanates from my orange shoes mean that I might as well be wearing equipment that aggravates monsters.'",
	"Incoming message for Player 1. The message is from 'Solvejg'. It reads: 'Yes, I like to aggravate you with my odorous lady shoes. Enjoy it!'",
	"Incoming message for Player 1. The message is from 'Bea'. It reads: 'I can bludgeon you easily!'",
	"Incoming message for Player 1. The message is from 'Bea'. It reads: 'I'm only 13 years old, but I know karate. One kick and you're down.'",
	"Incoming message for Player 1. The message is from 'Markus'. It reads: 'Justice will be victorious.'",
	"Incoming message for Player 1. The message is from 'Thai'. It reads: 'I'm a black belt in karate.'",
	"Incoming message for Player 1. The message is from 'Robat'. It reads: 'Old beer drink, marijuana smoke!'",
	"Incoming message for Player 1. The message is from 'Tobi'. It reads: 'Hoyo hoyo, wololooooo! Hoyo hoyo!'",
	"Incoming message for Player 1. The message is from 'Tobi'. It reads: 'I'm a pig priest and I will convert you to my religion now! Hoyohoyowollomollo!'",
	"Incoming message for Player 1. The message is from 'Tobi'. It reads: 'Yes, I developed the pig priests myself! I don't care if you're a Macedonian with four-fold conversion resistance, because I can convert even five-fold conversion-resistant Chinese to my God!'",
	"Incoming message for Player 1. The message is from 'Pete'. It reads: 'You'll get yours, you guy!'",
	"Incoming message for Player 1. The message is from 'Pete'. It reads: 'Now here's a speed bump.'",
	"Incoming message for Player 1. The message is from 'Kati'. It reads: 'My butt cheeks are incredibly tender!'",
	"Incoming message for Player 1. The message is from 'Romann'. It reads: 'The reward is got as mine!'",
	"Incoming message for Player 1. The message is from 'Romann'. It reads: 'The firing bonus belongs to me!'",
	"Incoming message for Player 1. The message is from 'Leo'. It reads: 'Oh, you're enveloped by a dark ball of light!'",
	"Incoming message for Player 1. The message is from 'Flo-ooo'. It reads: 'All the robbers have to be united! Only then can the motherfuckers be defeated!'",
	"Incoming message for Player 1. The message is from 'Flo-ooo'. It reads: 'If you're a robber, join our cause - after all, we'll be the united robber class now. But if you're a motherfucker then you can expect to be killed by us!'",
	"Incoming message for Player 1. The message is from 'David'. It reads: 'We're going to New Vegas. Yes indeed.'",
	"Incoming message for Player 1. The message is from 'Ludgera'. It reads: 'I have to go on the loo quickly!'",
	"Incoming message for Player 1. The message is from 'Arabella'. It reads: 'Your right mouse button failed.'",
	"Incoming message for Player 1. The message is from 'Nino'. It reads: 'You don't have Caesar's tattoo, and therefore you're an enemy.'",
	"Incoming message for Player 1. The message is from 'Nino'. It reads: 'Caesar will punish you, and then you'll pay.'",
	"Incoming message for Player 1. The message is from 'Katharina'. It reads: 'My butt cheeks can produce very beautiful squeaking noises while pressing the fart gas in your face.'",
	"Incoming message for Player 1. The message is from 'Elena'. It reads: 'Yes, I know that my tender butt cheeks are irresistible. I'm not called the eroticity queen for nothing, after all.'",
	"Incoming message for Player 1. The message is from 'Coffin Nail'. It reads: 'One day I'll win the Monopoly world championship.'",
	"Incoming message for Player 1. The message is from 'Robat'. It reads: 'Dirty motherfucker! Die!'",
	"Incoming message for Player 1. The message is from 'Elif'. It reads: 'I want to feel you up very gently.'",
	"Incoming message for Player 1. The message is from 'Elif'. It reads: 'My fingernails want to claw at your unprotected skin and cause bloody wounds.'",
	"Incoming message for Player 1. The message is from 'Elif'. It reads: 'Would you want me to fart in your face with my very tender butt?'",
	"Incoming message for Player 1. The message is from 'Elif'. It reads: 'It's fun to kick your hands repeatedly and letting the rough soles of my sneakers slide along your skin.'",
	"Incoming message for Player 1. The message is from 'Conny'. It reads: 'I'm wearing a skirt and high-heeled boots.'",
	"Incoming message for Player 1. The message is from 'Corina'. It reads: 'We gotta eradicate the robber threat.'",
	"Incoming message for Player 1. The message is from 'Conse'. It reads: 'Fuck you, robber.'",
	"Incoming message for Player 1. The message is from 'Nicole'. It reads: 'I'm called the Queen of the Night.'",
	"Incoming message for Player 1. The message is from 'Sarah'. It reads: 'And then Sarah came.'",
	"Incoming message for Player 1. The message is from 'Larissa'. It reads: 'My plateau boots love to kick you in the shins.'",
	"Incoming message for Player 1. The message is from 'Almuth'. It reads: 'My sneakers are cuddly-soft and want to be caressed!'",

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
