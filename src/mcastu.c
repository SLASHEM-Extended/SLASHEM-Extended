/*	SCCS Id: @(#)mcastu.c	3.4	2003/01/08	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "edog.h" /* sporkhack MGC_ENRAGE needs this */

/* monster mage spells */
#define MGC_PSI_BOLT	 0
#define MGC_CURE_SELF	 1
#define MGC_HASTE_SELF	 2
#define MGC_STUN_YOU	 3
#define MGC_DISAPPEAR	 4
#define MGC_WEAKEN_YOU	 5
#define MGC_DESTRY_ARMR	 6
#define MGC_CURSE_ITEMS	 7
#define MGC_AGGRAVATION	 8
#define MGC_SUMMON_MONS	 9
#define MGC_CLONE_WIZ	10
#define MGC_DEATH_TOUCH	11
#define MGC_CREATE_POOL	12
#define MGC_CALL_UNDEAD	13
#define MGC_WITHER	14
#define MGC_DAMAGE_ARMR	15
#define MGC_ENRAGE	16
#define MGC_DIVINE_WRATH	17
#define MGC_SUMMON_GHOST	18
#define MGC_MEGALOAD	19
#define MGC_LEVITATE	20

/* monster cleric spells */
#define CLC_OPEN_WOUNDS	 0
#define CLC_CURE_SELF	 1
#define CLC_CONFUSE_YOU	 2
#define CLC_PARALYZE	 3
#define CLC_BLIND_YOU	 4
#define CLC_INSECTS	 5
#define CLC_CURSE_ITEMS	 6
#define CLC_LIGHTNING	 7
#define CLC_FIRE_PILLAR	 8
#define CLC_GEYSER	 9
#define CLC_AGGRAVATION	 10
#define CLC_PETRIFY	 11 /* currently unused */
#define CLC_RANDOM	 12
#define CLC_VULN_YOU	 13

STATIC_DCL void FDECL(cursetxt,(struct monst *,BOOLEAN_P));
STATIC_DCL int FDECL(choose_magic_spell, (int));
STATIC_DCL int FDECL(choose_clerical_spell, (int));
STATIC_DCL void FDECL(cast_wizard_spell,(struct monst *, int,int));
STATIC_DCL void FDECL(cast_cleric_spell,(struct monst *, int,int));
STATIC_DCL boolean FDECL(is_undirected_spell,(unsigned int,int));
STATIC_DCL boolean FDECL(is_melee_spell,(unsigned int,int));
STATIC_DCL boolean FDECL(spell_would_be_useless,(struct monst *,unsigned int,int));
STATIC_PTR void FDECL(set_litZ, (int,int,genericptr_t));

#ifdef OVL0

extern const char * const flash_types[];	/* from zap.c */

/* feedback when frustrated monster couldn't cast a spell */
STATIC_OVL
void
cursetxt(mtmp, undirected)
struct monst *mtmp;
boolean undirected;
{
	if (canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my)) {
	    const char *point_msg;  /* spellcasting monsters are impolite */

	    if (undirected)
		point_msg = "all around, then curses";
	    else if ((Invis && !perceives(mtmp->data) &&
			(mtmp->mux != u.ux || mtmp->muy != u.uy)) ||
		    (youmonst.m_ap_type == M_AP_OBJECT &&
			youmonst.mappearance == STRANGE_OBJECT) ||
		    u.uundetected)
		point_msg = "and curses in your general direction";
	    else if (Displaced && (mtmp->mux != u.ux || mtmp->muy != u.uy))
		point_msg = "and curses at your displaced image";
	    else
		point_msg = "at you, then curses";

	    pline("%s points %s.", Monnam(mtmp), point_msg);
	} else if ((!(moves % 4) || !rn2(4))) {
	    if (flags.soundok) Norep("You hear a mumbled curse.");
	}
}

#endif /* OVL0 */
#ifdef OVLB

/* convert a level based random selection into a specific mage spell;
   inappropriate choices will be screened out by spell_would_be_useless() */
STATIC_OVL int
choose_magic_spell(spellval)
int spellval;
{

    switch (spellval) {
    case 45:
	if (!rn2(25)) return MGC_DIVINE_WRATH; /* waaaay too overpowered, so this will appear much more rarely --Amy */
	else return MGC_ENRAGE;
    case 44:
	return MGC_ENRAGE; /* we reduce the risk of getting a touch of death */
    case 43:
	return MGC_DEATH_TOUCH;
    case 42:
    case 41:
	if (!rn2(25)) return MGC_SUMMON_GHOST; /* Should be rare --Amy */
	else return MGC_CLONE_WIZ;
    case 40:
    case 39:
	return MGC_HASTE_SELF;
    case 38:
	if (!rn2(2)) return MGC_AGGRAVATION;
	else return MGC_SUMMON_MONS;
    case 37:
    case 36:
	return MGC_AGGRAVATION;
    case 35:
	if (!rn2(2)) return MGC_LEVITATE;
	else if (!rn2(5)) return MGC_MEGALOAD;
	else return MGC_CREATE_POOL;
    case 34:
    case 33:
	return MGC_CURSE_ITEMS;
    case 32:
	if (!rn2(2)) return MGC_AGGRAVATION;
	else return MGC_CALL_UNDEAD;
    case 31:
	if (!rn2(4)) return MGC_WITHER;
	else if (!rn2(2)) return MGC_DAMAGE_ARMR;
	else return MGC_DESTRY_ARMR;
    case 30:
    case 29:
	return MGC_WEAKEN_YOU;
    case 28:
    case 27:
	return MGC_DISAPPEAR;
    case 26:
	return MGC_STUN_YOU;
    case 25:
	return MGC_HASTE_SELF;
    case 24:
	return MGC_CURE_SELF;
    case 23:
	return MGC_PSI_BOLT;
    case 22:
	if (!rn2(25)) return MGC_DIVINE_WRATH; /* waaaay too overpowered, so this will appear much more rarely --Amy */
	else return MGC_ENRAGE;
    case 21:
	return MGC_ENRAGE; /* we reduce the risk of getting a touch of death */
    case 20:
	return MGC_DEATH_TOUCH;
    case 19:
    case 18:
	if (!rn2(25)) return MGC_SUMMON_GHOST; /* Should be rare --Amy */
	else return MGC_CLONE_WIZ;
    case 17:
    case 16:
	return MGC_HASTE_SELF;
    case 15:
	if (!rn2(2)) return MGC_AGGRAVATION;
	else return MGC_SUMMON_MONS;
    case 14:
    case 13:
	return MGC_AGGRAVATION;
    case 12:
	if (!rn2(2)) return MGC_LEVITATE;
	else if (!rn2(5)) return MGC_MEGALOAD;
	else return MGC_CREATE_POOL;
    case 11:
    case 10:
	return MGC_CURSE_ITEMS;
    case 9:
	if (!rn2(2)) return MGC_AGGRAVATION;
	else return MGC_CALL_UNDEAD;
    case 8:
	if (!rn2(4)) return MGC_WITHER;
	else if (!rn2(2)) return MGC_DAMAGE_ARMR;
	else return MGC_DESTRY_ARMR;
    case 7:
    case 6:
	return MGC_WEAKEN_YOU;
    case 5:
    case 4:
	return MGC_DISAPPEAR;
    case 3:
	return MGC_STUN_YOU;
    case 2:
	return MGC_HASTE_SELF;
    case 1:
	return MGC_CURE_SELF;
    case 0:
    default:
	return MGC_PSI_BOLT;
    }
}

/* convert a level based random selection into a specific cleric spell */
STATIC_OVL int
choose_clerical_spell(spellnum)
int spellnum;
{

    switch (spellnum) {
    case 41:
	/*if (rn2(10)) */return CLC_GEYSER;
	/*else return CLC_PETRIFY;*/ /* this is incorporated into CLC_GEYSER now, see below --Amy */
    case 40:
	return CLC_FIRE_PILLAR;
    case 39:
	return CLC_LIGHTNING;
    case 38:
    case 37:
	return CLC_CURSE_ITEMS;
    case 36:
	if (rn2(2)) return CLC_RANDOM;
	else if (!rn2(3)) return CLC_AGGRAVATION;
	else return CLC_INSECTS;
    case 35:
    case 34:
	return CLC_BLIND_YOU;
    case 33:
    case 32:
	return CLC_PARALYZE;
    case 31:
    case 30:
	return CLC_CONFUSE_YOU;
    case 29:
	return CLC_CURE_SELF;
    case 28:
	return (rn2(10) ? CLC_OPEN_WOUNDS : CLC_VULN_YOU);
    case 27:
	/*if (rn2(10)) */return CLC_GEYSER;
	/*else return CLC_PETRIFY;*/ /* this is incorporated into CLC_GEYSER now, see below --Amy */
    case 26:
	return CLC_FIRE_PILLAR;
    case 25:
	return CLC_LIGHTNING;
    case 24:
    case 23:
	return CLC_CURSE_ITEMS;
    case 22:
	if (rn2(2)) return CLC_RANDOM;
	else if (!rn2(3)) return CLC_AGGRAVATION;
	else return CLC_INSECTS;
    case 21:
    case 20:
	return CLC_BLIND_YOU;
    case 19:
    case 18:
	return CLC_PARALYZE;
    case 17:
    case 16:
	return CLC_CONFUSE_YOU;
    case 15:
	return CLC_CURE_SELF;
    case 14:
	return (rn2(10) ? CLC_OPEN_WOUNDS : CLC_VULN_YOU);
    case 13:
	/*if (rn2(10)) */return CLC_GEYSER;
	/*else return CLC_PETRIFY;*/ /* this is incorporated into CLC_GEYSER now, see below --Amy */
    case 12:
	return CLC_FIRE_PILLAR;
    case 11:
	return CLC_LIGHTNING;
    case 10:
    case 9:
	return CLC_CURSE_ITEMS;
    case 8:
	if (rn2(2)) return CLC_RANDOM;
	else if (!rn2(3)) return CLC_AGGRAVATION;
	else return CLC_INSECTS;
    case 7:
    case 6:
	return CLC_BLIND_YOU;
    case 5:
    case 4:
	return CLC_PARALYZE;
    case 3:
    case 2:
	return CLC_CONFUSE_YOU;
    case 1:
	return CLC_CURE_SELF;
    case 0:
    default:
	return (rn2(10) ? CLC_OPEN_WOUNDS : CLC_VULN_YOU);
    }
}

/* return values:
 * 1: successful spell
 * 0: unsuccessful spell
 */
int
castmu(mtmp, mattk, thinks_it_foundyou, foundyou)
	register struct monst *mtmp;
	register struct attack *mattk;
	boolean thinks_it_foundyou;
	boolean foundyou;
{
	int	dmg, ml = mtmp->m_lev;
	int ret;
	int spellnum = 0;
	int spellev, chance, difficulty, splcaster, learning;

	/* Three cases:
	 * -- monster is attacking you.  Search for a useful spell.
	 * -- monster thinks it's attacking you.  Search for a useful spell,
	 *    without checking for undirected.  If the spell found is directed,
	 *    it fails with cursetxt() and loss of mspec_used.
	 * -- monster isn't trying to attack.  Select a spell once.  Don't keep
	 *    searching; if that spell is not useful (or if it's directed),
	 *    return and do something else. 
	 * Since most spells are directed, this means that a monster that isn't
	 * attacking casts spells only a small portion of the time that an
	 * attacking monster does.
	 */
	if ((mattk->adtyp == AD_SPEL || mattk->adtyp == AD_CLRC) && ml) {
	    int cnt = 40;

	    do {
		spellnum = rn2(ml);
		/* Casting level is limited by available energy */
		spellev = spellnum / 7 + 1;
		if (spellev > 10) spellev = 10;
		if (spellev * 5 > mtmp->m_en) {
		    spellev = mtmp->m_en / 5;
		    spellnum = (spellev - 1) * 7 + 1;
		}
		if (mattk->adtyp == AD_SPEL)
		    spellnum = choose_magic_spell(spellnum);
		else
		    spellnum = choose_clerical_spell(spellnum);
		/* not trying to attack?  don't allow directed spells */
		if (!thinks_it_foundyou) {
		    if ( (!is_undirected_spell(mattk->adtyp, spellnum) && rn2(250) ) || is_melee_spell(mattk->adtyp, spellnum) || spell_would_be_useless(mtmp, mattk->adtyp, spellnum)) {
			if (foundyou)
			    impossible("spellcasting monster found you and doesn't know it?");
			return 0;
		    }
		    break;
		}
	    } while(--cnt > 0 &&
		    spell_would_be_useless(mtmp, mattk->adtyp, spellnum));
	    if (cnt == 0) return 0;
	} else {
	    /* Casting level is limited by available energy */
	    spellev = ml / 7 + 1;
	    if (spellev > 10) spellev = 10;
	    if (spellev * 5 > mtmp->m_en) {
		spellev = mtmp->m_en / 5;
		ml = (spellev - 1) * 7 + 1;
	    }
	}

	/* monster unable to cast spells? */
	if (mtmp->mcan || mtmp->m_en < 5 || mtmp->mspec_used || !ml) {
	    cursetxt(mtmp, is_undirected_spell(mattk->adtyp, spellnum));
	    return(0);
	}

	if (mattk->adtyp == AD_SPEL || mattk->adtyp == AD_CLRC) {
	    /*
	     * Spell use (especially MGC) is more common in Slash'EM.
	     * Still using mspec_used, just so monsters don't go bonkers.
	     */
#if 0
	    mtmp->mspec_used = 10 - mtmp->m_lev;
	    if (mtmp->mspec_used < 2) mtmp->mspec_used = 2;
#endif
	    mtmp->mspec_used = rn2(15) - mtmp->m_lev;
	    if (mattk->adtyp == AD_SPEL)
		mtmp->mspec_used = mtmp->mspec_used > 0 ? 2 : 0;
	    else if (mtmp->mspec_used < 2) mtmp->mspec_used = 2;
	}

	/* monster can cast spells, but is casting a directed spell at the
	   wrong place?  If so, give a message, and return.  Do this *after*
	   penalizing mspec_used. */
	if (!foundyou && thinks_it_foundyou &&
		!is_undirected_spell(mattk->adtyp, spellnum)) {
	    pline("%s casts a spell at %s!",
		canseemon(mtmp) ? Monnam(mtmp) : "Something",
		levl[mtmp->mux][mtmp->muy].typ == WATER
		    ? "empty water" : "thin air");
	    return(0);
	}

	nomul(0);

	mtmp->m_en -= spellev * 5; /* Use up the energy now */

	/* We should probably do similar checks to what is done for
	 * the player - armor, etc.
	 * Checks for armour and other intrinsic ability change splcaster
	 * Difficulty and experience affect chance
	 * Assume that monsters only cast spells that they know well
	 */
	splcaster = 15 - (mtmp->m_lev / 2); /* Base for a wizard is 5...*/

	if (splcaster < 5) splcaster = 5;
	if (splcaster > 20) splcaster = 20;

	chance = 11 * (mtmp->m_lev > 25 ? 18 : (12 + (mtmp->m_lev / 5)));
	chance++ ;  /* Minimum chance of 1 */

	difficulty = (spellev - 1) * 4 - (mtmp->m_lev - 1);
	    /* law of diminishing returns sets in quickly for
	     * low-level spells.  That is, higher levels quickly
	     * result in almost no gain
	     */
	learning = 15 * (-difficulty / spellev);
	chance += learning > 20 ? 20 : learning;

	/* clamp the chance */
	if (chance < 0) chance = 0;
	if (chance > 120) chance = 120;

	/* combine */
	chance = chance * (20-splcaster) / 15 - splcaster;

	/* Clamp to percentile */
	if (chance > 100) chance = 100;
	if (chance < 0) chance = 0;

#if 0
	if(rn2(ml*10) < (mtmp->mconf ? 100 : 20)) {	/* fumbled attack */
#else
	if (mtmp->mconf || rnd(100) > chance) { /* fumbled attack */
#endif
	    if (canseemon(mtmp) && flags.soundok)
		pline_The("air crackles around %s.", mon_nam(mtmp));
	    return(0);
	}
	if (canspotmon(mtmp) || !is_undirected_spell(mattk->adtyp, spellnum)) {
	    pline("%s casts a spell%s!",
		  canspotmon(mtmp) ? Monnam(mtmp) : "Something",
		  is_undirected_spell(mattk->adtyp, spellnum) ? "" :
		  (Invisible && !perceives(mtmp->data) && 
		   (mtmp->mux != u.ux || mtmp->muy != u.uy)) ?
		  " at a spot near you" :
		  (Displaced && (mtmp->mux != u.ux || mtmp->muy != u.uy)) ?
		  " at your displaced image" :
		  " at you");
	}

/*
 *	As these are spells, the damage is related to the level
 *	of the monster casting the spell.
 */
	if (!foundyou) {
	    /*dmg = 0;*/
	    if (mattk->adtyp != AD_SPEL && mattk->adtyp != AD_CLRC) {
		impossible(
	      "%s casting non-hand-to-hand version of hand-to-hand spell %d?",
			   Monnam(mtmp), mattk->adtyp);
		return(0);
	    }
	} /*else*/ if (mattk->damd)
	    dmg = d((int)((ml/2) + mattk->damn), (int)mattk->damd);
	else dmg = d((int)((ml/2) + 1), 6);
	if (Half_spell_damage && rn2(2) ) dmg = (dmg+1) / 2;

	ret = 1;

	switch (mattk->adtyp) {

	    case AD_FIRE:
		pline("You're enveloped in flames.");
		if(Fire_resistance && rn2(20) ) {
			shieldeff(u.ux, u.uy);
			pline("But you resist the effects.");
			dmg = 0;
		}
		if (Slimed) {
			pline("The slime is burned away!");
			Slimed =0;
		}
		burn_away_slime();
		break;
	    case AD_COLD:
		pline("You're covered in frost.");
		if(Cold_resistance && rn2(20) ) {
			shieldeff(u.ux, u.uy);
			pline("But you resist the effects.");
			dmg = 0;
		}
		break;
	    case AD_ACID:
		pline("You're covered in acid.");
		if(Acid_resistance && rn2(20) ) {
			shieldeff(u.ux, u.uy);
			pline("But you resist the effects.");
			dmg = 0;
		}
		break;
	    case AD_ELEC:
		pline("You're shocked.");
		if(Shock_resistance && rn2(20) ) {
			shieldeff(u.ux, u.uy);
			pline("But you resist the effects.");
			dmg = 0;
		}
		break;
	    case AD_DISN:
		verbalize("Avada Kedavra!");
		if((Antimagic && rn2(20) ) || nonliving(youmonst.data) || is_demon(youmonst.data) || Invulnerable || (Stoned_chiller && Stoned)) {
			shieldeff(u.ux, u.uy);
			pline("But you resist the effects.");
			dmg = 0;
		}
		else if (!rn2(20)) {
		    killer_format = KILLED_BY_AN;
		    killer = "Avada Kedavra curse";
		    done(DIED);
		    return 1; /* lifesaved */
		}
		break;
	    case AD_MAGM:
		You("are hit by a shower of missiles!");
		if(Antimagic && rn2(20) ) {
			shieldeff(u.ux, u.uy);
			pline_The("missiles bounce off!");
			dmg = 0;
		}
		break;
	    case AD_SPEL:	/* wizard spell */
	    case AD_CLRC:       /* clerical spell */
	    {
		if (mattk->adtyp == AD_SPEL)
		    cast_wizard_spell(mtmp, dmg, spellnum);
		else
		    cast_cleric_spell(mtmp, dmg, spellnum);
		dmg = 0; /* done by the spell casting functions */
		break;
	    }
	}
	if(dmg) mdamageu(mtmp, dmg);
	return(ret);
}


/* monster wizard and cleric spellcasting functions */
/*
   If dmg is zero, then the monster is not casting at you.
   If the monster is intentionally not casting at you, we have previously
   called spell_would_be_useless() and spellnum should always be a valid
   undirected spell.
   If you modify either of these, be sure to change is_undirected_spell()
   and spell_would_be_useless().
 */
STATIC_OVL
void
cast_wizard_spell(mtmp, dmg, spellnum)
struct monst *mtmp;
int dmg;
int spellnum;
{

	struct monst* mtmp2;
	int seen,count;
	struct edog* edog;
	struct obj *otmp;

    if (dmg == 0 && !is_undirected_spell(AD_SPEL, spellnum)) {
	impossible("cast directed wizard spell (%d) with dmg=0?", spellnum);
	return;
    }

    switch (spellnum) {
    case MGC_DEATH_TOUCH:
	pline("Oh no, %s's using the touch of death!", mhe(mtmp));
	if (nonliving(youmonst.data) || is_demon(youmonst.data)) {
	    You("seem no deader than before.");
	} else if (!Antimagic && rn2(mtmp->m_lev) > 12) {
	    if (Hallucination) {
		You("have an out of body experience.");
	    } else if (!rnd(50)) {
		killer_format = KILLED_BY_AN;
		killer = "touch of death";
		done(DIED);}
		else {
			dmg = d(8,6);
			/* Magic resistance or half spell damage will cut this in half... */
			/* and also prevent a reduction of maximum hit points */
			if (Antimagic || (Half_spell_damage && rn2(2)) ) {
				shieldeff(u.ux, u.uy);
				dmg /= 2;
				u.uhpmax -= dmg/2;
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				You("feel a loss of life.");
				losehp(dmg,"touch of death",KILLED_BY_AN);
			}
			else {
			You("feel drained...");
			u.uhpmax -= dmg/2;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			losehp(dmg,"touch of death",KILLED_BY_AN); }

		/*{pline("Whew! That could have been your demise.");*/
	    }
	} else {
	    if (Antimagic) shieldeff(u.ux, u.uy);
	    pline("Lucky for you, it didn't work!");
	}
	dmg = 0;
	break;
    case MGC_CREATE_POOL:
	if (levl[u.ux][u.uy].typ == ROOM || levl[u.ux][u.uy].typ == CORR) {
	    pline(Hallucination ? "Huh - the ground suddenly turned into a swimming pool!" : "A pool appears beneath you!");
	    levl[u.ux][u.uy].typ = POOL;
	    del_engr_at(u.ux, u.uy);
	    water_damage(level.objects[u.ux][u.uy], FALSE, TRUE);
	    spoteffects(FALSE);  /* possibly drown, notice objects */
	}
	else
	    impossible("bad pool creation?");
	dmg = 0;
	break;

    case MGC_MEGALOAD:
	if ((otmp = mksobj(LOADSTONE, TRUE, FALSE)) != (struct obj *)0) {
	pline(Hallucination ? "Aww, something's killing your good feelings!" : "You feel burdened");
	if (pickup_object(otmp, 1, FALSE) <= 0) {
	obj_extract_self(otmp);
	place_object(otmp, u.ux, u.uy);
	newsym(u.ux, u.uy); }
	}

	dmg = 0;
	break;

    case MGC_LEVITATE:
	pline(Hallucination ? "Wow... you're suddenly walking on air!" : "You float up!");
	HLevitation &= ~I_SPECIAL;
	incr_itimeout(&HLevitation, rnz(50));

	dmg = 0;
	break;

    case MGC_CLONE_WIZ:
	if (mtmp->iswiz && (flags.no_of_wizards == 1 || !rn2(20)) ) { /* let's have a small chance of triple trouble --Amy */
	    if (flags.no_of_wizards == 1) pline(Hallucination ? "Doublevision!" : "Double Trouble...");
	    else pline(Hallucination ? "Triplevision!" : "Triple Trouble...");
	    clonewiz();
	    dmg = 0;
	} else
	    pline(Hallucination ? "For a moment you had triplevision, but seeing double is funny enough." : "For a moment you saw another Wizard, but it disappeared.");
	break;
    case MGC_SUMMON_MONS:
    {
	int count;

	count = nasty(mtmp);	/* summon something nasty */
	if (mtmp->iswiz)
	    verbalize("Destroy the thief, my pet%s!", plur(count));
	else {
	    const char *mappear =
		(count == 1) ? "A monster appears" : "Monsters appear";

	    /* messages not quite right if plural monsters created but
	       only a single monster is seen */
	    if (Invisible && !perceives(mtmp->data) &&
				    (mtmp->mux != u.ux || mtmp->muy != u.uy))
		pline("%s around a spot near you!", mappear);
	    else if (Displaced && (mtmp->mux != u.ux || mtmp->muy != u.uy))
		pline("%s around your displaced image!", mappear);
	    else
		pline("%s from nowhere!", mappear);
	}
	dmg = 0;
	break;
    }
    case MGC_SUMMON_GHOST:
    {
		coord mm;   
		mm.x = u.ux;   
		mm.y = u.uy;   

	tt_mname(&mm, FALSE, 0);	/* create player-monster ghosts */
	if (mtmp->iswiz)
	    verbalize("Destroy the thief, my pets!");
	else {
	    const char *mappear =
		"Ghosts appear";

	    /* messages not quite right if plural monsters created but
	       only a single monster is seen */
	    if (Invisible && !perceives(mtmp->data) &&
				    (mtmp->mux != u.ux || mtmp->muy != u.uy))
		pline("%s around a spot near you!", mappear);
	    else if (Displaced && (mtmp->mux != u.ux || mtmp->muy != u.uy))
		pline("%s around your displaced image!", mappear);
	    else
		pline("%s from nowhere!", mappear);
	}
	dmg = 0;
	break;
    }


	case MGC_CALL_UNDEAD:
	{
		coord mm;   
		mm.x = u.ux;   
		mm.y = u.uy;   
		pline("Undead creatures are called forth from the grave!");   
		mkundead(&mm, FALSE, 0);   
	}
	dmg = 0;   
	break;   
    case MGC_AGGRAVATION:
	You_feel("that monsters are aware of your presence.");
	aggravate();
	dmg = 0;
	break;
    case MGC_CURSE_ITEMS:
	You_feel("as if you need some help.");
	rndcurse();
	dmg = 0;
	break;
    case MGC_DESTRY_ARMR:
	if (Antimagic && rn2(20)) {
	    shieldeff(u.ux, u.uy);
	    pline("A field of force surrounds you!");
	} else if (!destroy_arm(some_armor(&youmonst))) {
	    Your("skin itches.");
	}
	dmg = 0;
	break;

	/* from Sporkhack */
	/* Inspire critters to fight a little more vigorously...
	 *
	 * -- Peaceful critters may become hostile.
	 * -- Hostile critters may become berserk.
	 * -- Borderline tame critters, or tame critters
	 *    who have been treated poorly may ALSO become hostile!
	 */
	 case MGC_ENRAGE:
		for (mtmp2 = fmon; mtmp2; mtmp2 = mtmp2->nmon) {
			if (m_cansee(mtmp,mtmp2->mx,mtmp2->my) && rn2(3) &&
					mtmp2 != mtmp && distu(mtmp2->mx,mtmp2->my) < 16) {
				seen++;
				if (mtmp2->mtame) {
					edog = (mtmp2->isminion) ? 0 : EDOG(mtmp2);
					if (mtmp2->mtame <= /*3*/rnd(21) || (edog && edog->abuse >= /*5*/ rn2(6) )) {
						mtmp2->mtame = mtmp2->mpeaceful = 0;
						if (mtmp2->mleashed) { m_unleash(mtmp2,FALSE); }
						count++;
					}
				} else if (mtmp2->mpeaceful) {
					mtmp2->mpeaceful = 0;
					count++;
				} else {
					/*mtmp2->mberserk = 1;*/ /* removed because this attribute doesn't exist in this fork */
					mtmp2->mhp = mtmp2->mhpmax; /* let's heal them instead --Amy */
					count++;
				}
			}
		}
		/* Don't yell if we didn't see anyone to yell at. */
		if (seen && (!rn2(3) || mtmp->iswiz)) {
			verbalize("Get %s, you fools, or I'll have your figgin on a stick!",uhim());
		}
		if (count) {
			pline("It seems a little more dangerous here now...");
			doredraw();
		}
		dmg = 0;
		break;

    case MGC_DIVINE_WRATH: /* new idea by Amy. Yes, this is very evil. :D */

		u.ugangr++;
		if (!rn2(5)) u.ugangr++;
		if (!rn2(25)) u.ugangr++;
		prayer_done();

		dmg = 0;
		break;

    case MGC_WITHER:
	if (Antimagic && rn2(5)) {
	    shieldeff(u.ux, u.uy);
	    pline("A field of force surrounds you!");
	} else {
	    pline("You sense a sinister feeling of loss!");

	while (1) {
	    switch(rn2(5)) {
	    case 0:
		if (!uarmh || !wither_dmg(uarmh, xname(uarmh), rn2(4), FALSE, &youmonst))
			continue;
		break;
	    case 1:
		if (uarmc) {
		    (void)wither_dmg(uarmc, xname(uarmc), rn2(4), TRUE, &youmonst);
		    break;
		}
		/* Note the difference between break and continue;
		 * break means it was hit and didn't rust; continue
		 * means it wasn't a target and though it didn't rust
		 * something else did.
		 */
		if (uarm)
		    (void)wither_dmg(uarm, xname(uarm), rn2(4), TRUE, &youmonst);
#ifdef TOURIST
		else if (uarmu)
		    (void)wither_dmg(uarmu, xname(uarmu), rn2(4), TRUE, &youmonst);
#endif
		break;
	    case 2:
		if (!uarms || !wither_dmg(uarms, xname(uarms), rn2(4), FALSE, &youmonst))
		    continue;
		break;
	    case 3:
		if (!uarmg || !wither_dmg(uarmg, xname(uarmg), rn2(4), FALSE, &youmonst))
		    continue;
		break;
	    case 4:
		if (!uarmf || !wither_dmg(uarmf, xname(uarmf), rn2(4), FALSE, &youmonst))
		    continue;
		break;
		}
	    break; /* Out of while loop */
	    }
	}
	dmg = 0;
	break;
    case MGC_DAMAGE_ARMR:
	if (Antimagic && rn2(5)) {
	    shieldeff(u.ux, u.uy);
	    pline("A field of force surrounds you!");
	} else {
	    pline("Your body shakes!");

	while (1) {
	    switch(rn2(5)) {
	    case 0:
		if (!uarmh || !rust_dmg(uarmh, xname(uarmh), rn2(4), FALSE, &youmonst))
			continue;
		break;
	    case 1:
		if (uarmc) {
		    (void)rust_dmg(uarmc, xname(uarmc), rn2(4), TRUE, &youmonst);
		    break;
		}
		/* Note the difference between break and continue;
		 * break means it was hit and didn't rust; continue
		 * means it wasn't a target and though it didn't rust
		 * something else did.
		 */
		if (uarm)
		    (void)rust_dmg(uarm, xname(uarm), rn2(4), TRUE, &youmonst);
#ifdef TOURIST
		else if (uarmu)
		    (void)rust_dmg(uarmu, xname(uarmu), rn2(4), TRUE, &youmonst);
#endif
		break;
	    case 2:
		if (!uarms || !rust_dmg(uarms, xname(uarms), rn2(4), FALSE, &youmonst))
		    continue;
		break;
	    case 3:
		if (!uarmg || !rust_dmg(uarmg, xname(uarmg), rn2(4), FALSE, &youmonst))
		    continue;
		break;
	    case 4:
		if (!uarmf || !rust_dmg(uarmf, xname(uarmf), rn2(4), FALSE, &youmonst))
		    continue;
		break;
		}
	    break; /* Out of while loop */
	    }
	}
	dmg = 0;
	break;
    case MGC_WEAKEN_YOU:		/* drain strength */
	if (Antimagic && rn2(20)) {
	    shieldeff(u.ux, u.uy);
	    You_feel("momentarily weakened.");
	} else {
	    You("suddenly feel weaker!");
	    dmg = mtmp->m_lev - 6;
	    if (Half_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
	    losestr(rnd(dmg));
	    if (u.uhp < 1)
		done_in_by(mtmp);
	}
	dmg = 0;
	break;
    case MGC_DISAPPEAR:		/* makes self invisible */
	if (!mtmp->minvis && !mtmp->invis_blkd) {
	    if (canseemon(mtmp))
		pline("%s suddenly %s!", Monnam(mtmp),
		      !See_invisible ? "disappears" : "becomes transparent");
	    mon_set_minvis(mtmp);
	    dmg = 0;
	} else
	    impossible("no reason for monster to cast disappear spell?");
	break;
    case MGC_STUN_YOU:
	if ((Antimagic || Free_action) && rn2(20)) {
	    shieldeff(u.ux, u.uy);
	    if (!Stunned)
		You_feel("momentarily disoriented.");
	    make_stunned(1L, FALSE);
	} else {
	    You(Stunned ? "struggle to keep your balance." : "reel...");
	    dmg = d(ACURR(A_DEX) < 12 ? 6 : 4, 4);
	    if (Half_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
	    make_stunned(HStun + dmg, FALSE);
	}
	dmg = 0;
	break;
    case MGC_HASTE_SELF:
	mon_adjust_speed(mtmp, 1, (struct obj *)0);
	dmg = 0;
	break;
    case MGC_CURE_SELF:
	if (mtmp->mhp < mtmp->mhpmax) {
	    if (canseemon(mtmp))
		pline("%s looks better.", Monnam(mtmp));
	    /* note: player healing does 6d4; this used to do 1d8 */
	    if ((mtmp->mhp += d(3,6)) > mtmp->mhpmax)
		mtmp->mhp = mtmp->mhpmax;
	    dmg = 0;
	}
	break;
    case MGC_PSI_BOLT:
	/* prior to 3.4.0 Antimagic was setting the damage to 1--this
	   made the spell virtually harmless to players with magic res. */
	if (Antimagic && rn2(20)) {
	    shieldeff(u.ux, u.uy);
	    dmg = (dmg + 1) / 2;
	}
	if (dmg <= 5)
	    You("get a slight %sache.", body_part(HEAD));
	else if (dmg <= 10)
	    Your("brain is on fire!");
	else if (dmg <= 20)
	    Your("%s suddenly aches painfully!", body_part(HEAD));
	else
	    Your("%s suddenly aches very painfully!", body_part(HEAD));
	break;
    default:
	impossible("mcastu: invalid magic spell (%d)", spellnum);
	dmg = 0;
	break;
    }

    if (dmg) mdamageu(mtmp, dmg);
}

STATIC_OVL
void
cast_cleric_spell(mtmp, dmg, spellnum)
struct monst *mtmp;
int dmg;
int spellnum;
{
	int aligntype;

    if (dmg == 0 && !is_undirected_spell(AD_CLRC, spellnum)) {
	impossible("cast directed cleric spell (%d) with dmg=0?", spellnum);
	return;
    }

    switch (spellnum) {
    case CLC_GEYSER:

	switch (rnd(37) ) {
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:

	/* this is physical damage, not magical damage */
	pline("A sudden geyser slams into you from nowhere!");
	dmg = d(8, 6);
	water_damage(invent, FALSE, FALSE); /* Come on, every other source of water rusts your stuff. --Amy */
	if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);
	if (Burned) make_burned(0L, TRUE); /* you're enveloped in water, so the burn disappears */
	if (Half_physical_damage && rn2(2) ) dmg = (dmg + 1) / 2;
	break;

	case 20:
	/* antimatter storm --Amy */
	pline("You are caught in an antimatter storm!");
	dmg = d(8, 6);
	withering_damage(invent, FALSE, FALSE); /* This can potentially damage all of your inventory items. --Amy */
	if (Half_physical_damage && rn2(2) ) dmg = (dmg + 1) / 2;

	break;

	case 21:
	case 22:
	case 23:
	case 24:
	case 25:

		/* petrify - similar to cockatrice hissing --Amy */
		pline("You feel a massive burden on your chest!");
		if (!Stoned && !Stone_resistance && !(poly_when_stoned(youmonst.data) &&
				 polymon(PM_STONE_GOLEM)) ) {Stoned = 7; delayed_killer = "petrify spell";}
		dmg = 0;
		break;

	case 26:
	case 27:
	case 28:
	case 29:
	case 30:

		/* sliming - similar to green slime attack --Amy */
		    if (!Slimed && !flaming(youmonst.data) && !Unchanging && youmonst.data != &mons[PM_GREEN_SLIME])
		 {You("don't feel very well.");
		    Slimed = 100L;
		    flags.botl = 1;}
		dmg = 0;
		break;

	case 31:
	case 32:
	case 33:
	case 34:
	case 35:
	case 36:
	case 37:

	/* -1 - A_CHAOTIC, 0 - A_NEUTRAL, 1 - A_LAWFUL */
		aligntype = rn2((int)A_LAWFUL+2) - 1;
		 pline("A servant of %s appears!",aligns[1 - aligntype].noun); /* summon elm, from sporkhack */
		summon_minion(aligntype, TRUE);
		dmg = 0;
		 break;


	default: /*failsafe*/
		You_feel("that monsters are aware of your presence."); /* aggravate monster */
		aggravate();
		dmg = 0;
		break;
	}
	break;

    case CLC_FIRE_PILLAR:
	pline("A pillar of fire strikes all around you!");
	if (Fire_resistance && rn2(20)) {
	    shieldeff(u.ux, u.uy);
	    dmg = 0;
	} else
	    dmg = d(8, 6);
	if (Half_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
	burn_away_slime();
	if (!rn2(33)) (void) burnarmor(&youmonst);
		    if (!rn2(15)) /* new calculations --Amy */	destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (!rn2(15)) /* new calculations --Amy */	destroy_item(POTION_CLASS, AD_FIRE);
		    if (!rn2(15)) /* new calculations --Amy */	destroy_item(SPBOOK_CLASS, AD_FIRE);
	(void) burn_floor_paper(u.ux, u.uy, TRUE, FALSE);
	break;
    case CLC_LIGHTNING:
    {
	boolean reflects;

	/* WAC add lightning strike effect */
	zap_strike_fx(u.ux, u.uy, AD_ELEC - 1);
	pline("A bolt of lightning strikes down at you from above!");
	reflects = ureflects("It bounces off your %s%s.", "");
	if (!Blind) {
	    pline("You are blinded by the flash!");
	    make_blinded(Half_spell_damage ? 10L : 20L, FALSE);
	}
	if (reflects || Shock_resistance) {
	    shieldeff(u.ux, u.uy);
	    dmg = 0;
	    if (reflects)
		break;
	} else
	    dmg = d(8, 6);
	if (Half_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
		    if (!rn2(15)) /* new calculations --Amy */	destroy_item(WAND_CLASS, AD_ELEC);
		    if (!rn2(15)) /* new calculations --Amy */	destroy_item(RING_CLASS, AD_ELEC);
	break;
    }
    case CLC_CURSE_ITEMS:
	You_feel("as if you need some help.");
	rndcurse();
	dmg = 0;
	break;

    case CLC_AGGRAVATION: /* aggravate monster */
	You_feel("that monsters are aware of your presence."); /* aggravate monster */
	aggravate();
	dmg = 0;
	break;

    case CLC_RANDOM: /* inofficial names see below */
	switch (rnd(38) ) {

	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
		You_feel("a sense of intrinsic loss."); /* intrinsic loss */
	    attrcurse();
		dmg = 0;
		break;
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
		You_feel("a dark aura."); /* dark aura */
		int rangeX;

		rangeX = (rnz(1 + mtmp->m_lev)) / 2;
		if (rn2(4)) rangeX = rangeX / 2;

		if (rangeX < 1) rangeX = 1; if (rangeX > 15) rangeX = 15; 

	    do_clear_areaX(u.ux,u.uy,		/* darkness around player's position */
		/*15*/rangeX, set_litZ, (genericptr_t)((char *)0));
		    wake_nearby();
		dmg = 0;
                break;
	case 21:
	case 22:
	case 23:
		You_feel("very trippy all of a sudden!"); /* acid trip */
		int duratX;
		duratX = (rnz(1 + mtmp->m_lev));
		make_hallucinated(HHallucination + duratX,FALSE,0L);
		dmg = 0;
		break;
	case 24:
	case 25:
	      You_feel("endangered!!"); /* create traps */

			int rtrap;
		    int i, j, bd;
			bd = 1;
			if (!rn2(5)) bd += rnz(1);

		      for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
				if (!isok(u.ux + i, u.uy + j)) continue;
				if ((levl[u.ux + i][u.uy + j].typ != ROOM && levl[u.ux + i][u.uy + j].typ != CORR) || MON_AT(u.ux + i, u.uy + j)) continue;
				if (t_at(u.ux + i, u.uy + j)) continue;

			      rtrap = rnd(TRAPNUM-1);
				if (rtrap == HOLE) rtrap = PIT;
				if (rtrap == MAGIC_PORTAL) rtrap = PIT;
				if (rtrap == TRAPDOOR && !Can_dig_down(&u.uz)) rtrap = PIT;
				if (rtrap == LEVEL_TELEP && level.flags.noteleport) rtrap = SQKY_BOARD;
				if (rtrap == TELEP_TRAP && level.flags.noteleport) rtrap = SQKY_BOARD;
				if (rtrap == ROLLING_BOULDER_TRAP) rtrap = ROCKTRAP;
				if (rtrap == NO_TRAP) rtrap = ARROW_TRAP;
				if (rtrap == RMB_LOSS_TRAP && !Role_if(PM_SPACEWARS_FIGHTER) && rn2(2)) rtrap = ACID_POOL;
				if (rtrap == DISPLAY_TRAP && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 3)) rtrap = GLYPH_OF_WARDING;
				if (rtrap == SPELL_LOSS_TRAP && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 4)) rtrap = SLOW_GAS_TRAP;
				if (rtrap == YELLOW_SPELL_TRAP && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 5)) rtrap = POISON_GAS_TRAP;
				if (rtrap == MENU_TRAP && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 8)) rtrap = FIRE_TRAP;
				if (rtrap == SPEED_TRAP && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40)) rtrap = ICE_TRAP;
				if (rtrap == AUTOMATIC_SWITCHER && rn2(Race_if(PM_HAXOR) ? (Role_if(PM_GRADUATE) ? 125 : Role_if(PM_SPACEWARS_FIGHTER) ? 250 : Role_if(PM_GEEK) ? 250 : 500) : (Role_if(PM_GRADUATE) ? 250 : Role_if(PM_SPACEWARS_FIGHTER) ? 500 : Role_if(PM_GEEK) ? 500 : 1000) )) rtrap = SHOCK_TRAP;
				if (rtrap == AUTO_DESTRUCT_TRAP && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10)) rtrap = WATER_POOL;
				if (rtrap == MEMORY_TRAP && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20)) rtrap = SCYTHING_BLADE;
				if (rtrap == INVENTORY_TRAP && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50)) rtrap = ANIMATION_TRAP;
				if (rtrap == BLACK_NG_WALL_TRAP && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 50 : 100)) rtrap = UNKNOWN_TRAP;
				if (rtrap == SUPERSCROLLER_TRAP && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 100 : 200)) rtrap = WEB;
				if (rtrap == ACTIVE_SUPERSCROLLER_TRAP) rtrap = RUST_TRAP;

				(void) maketrap(u.ux + i, u.uy + j, rtrap);
			}
		dmg = 0;
		break;
	case 26:
	      You_feel("yanked in a new direction!"); /* relocation */
		(void) safe_teleds(FALSE);
		dmg = 0;
		break;
	case 27:
	case 28:
	case 29:
	      pline("Your mana is sapped!"); /* mana drain */
		int manastealX;
		manastealX = (rnz(1 + mtmp->m_lev));
		drain_en(manastealX);
		dmg = 0;
		break;
	case 30:
	      You_feel("an ominous droning wind!"); /* disengrave */
		register struct engr *ep = engr_at(u.ux,u.uy);
	      if (ep && sengr_at("Elbereth", u.ux, u.uy) ) {
		pline("Suddenly, the engraving beneath your feet smudges and dissolves!");
	      del_engr(ep);
	      ep = (struct engr *)0;
		}
		dmg = 0;
		break;
	case 31:
	case 32:
	case 33:
	      pline(Hallucination ? "You suddenly feel like you're on cold turkey!" : "Your hands start trembling!"); /* disarm */
		int glibberX;
		glibberX = (rnz(1 + mtmp->m_lev));
		    incr_itimeout(&Glib, glibberX);
		dmg = 0;
		break;
	case 34:
	case 35:
	case 36:
	      pline("You feel pulsating winds!"); /* slow */
		    u_slow_down();
		dmg = 0;
		break;
	case 37:
	      pline(Hallucination ? "You feel like you just got dumped by your girlfriend!" : "You feel out of luck!"); /* dementor force */
			change_luck(-1);
			if (!rn2(10)) change_luck(-5);
			adjalign(-10);
			if (!rn2(10)) adjalign(-50);
		dmg = 0;
		break;
	case 38:
	      pline("You feel burdened"); /* punishment - message is from Castle of the Winds */
			punishx();
		dmg = 0;
		break;
	default: /*failsafe*/
		You_feel("that monsters are aware of your presence."); /* aggravate monster */
		aggravate();
		dmg = 0;
		break;
	}


	break;

    case CLC_INSECTS:
      {
	/* Try for insects, and if there are none
	   left, go for (sticks to) snakes.  -3. */
	struct permonst *pm = mkclass(S_ANT,0);
	struct monst *mtmp2 = (struct monst *)0;
	char let = (pm ? S_ANT : S_SNAKE);
	boolean success;
	int i;
	coord bypos;
	int quan;

	/* Let's allow some variation. Unofficial spell names for each type of creature see below. --Amy */

	if (!rn2(10)) { switch (rnd(54)) {

		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
			let = S_SNAKE;
			break;
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			let = S_MIMIC;
			break;
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
			let = S_NYMPH;
			break;
		case 21:
		case 22:
		case 23:
			let = S_PIERCER;
			break;
		case 24:
		case 25:
		case 26:
		case 27:
		case 28:
			let = S_RODENT;
			break;
		case 29:
		case 30:
		case 31:
		case 32:
		case 33:
			let = S_SPIDER;
			break;
		case 34:
		case 35:
		case 36:
		case 37:
		case 38:
			let = S_WORM;
			break;
		case 39:
		case 40:
		case 41:
		case 42:
		case 43:
			let = S_BAT;
			break;
		case 44:
			let = S_UMBER;
			break;
		case 45:
		case 46:
			let = S_HUMAN;
			break;
		case 47:
		case 48:
			let = S_GOLEM;
			break;
		case 49:
		case 50:
		case 51:
			let = S_EEL;
			break;
		case 52:
		case 53:
		case 54:
			let = S_BAD_FOOD;
			break;
		default: /* failsafe */
			let = S_ANT;
			break;

		}

	}

	quan = (mtmp->m_lev < 2) ? 1 : rnd((int)mtmp->m_lev / 2);
	if (rn2(3)) quan = (quan / 2);
	if (quan < 1) quan = 1;
	success = pm ? TRUE : FALSE;
	for (i = 0; i <= quan; i++) {
	    if (!enexto(&bypos, mtmp->mux, mtmp->muy, mtmp->data))
		break;
	    if ((pm = mkclass(let,0)) != 0 &&
		    (mtmp2 = makemon(pm, bypos.x, bypos.y, NO_MM_FLAGS)) != 0) {
		success = TRUE;
		mtmp2->msleeping = mtmp2->mpeaceful = mtmp2->mtame = 0;
		set_malign(mtmp2);
	    }
	}
	/* Not quite right:
         * -- message doesn't always make sense for unseen caster (particularly
	 *    the first message)
         * -- message assumes plural monsters summoned (non-plural should be
         *    very rare, unlike in nasty())
         * -- message assumes plural monsters seen
         */
	if (!success)
	    pline("%s casts at a clump of sticks, but nothing happens.",
		Monnam(mtmp));
	else if (let == S_SNAKE) /* sticks to snakes */
	    pline("%s transforms a clump of sticks into snakes!",
		Monnam(mtmp));
	else if (let == S_MIMIC) /* garbage spam */
	    pline("%s conjures up random garbage!",
		Monnam(mtmp));
	else if (let == S_NYMPH) /* charming seduction */
	    pline("%s summons some beautiful ladies!",
		Monnam(mtmp));
	else if (let == S_PIERCER) /* piercing ceiling */
	    pline("You hear a shrill ringing sound.",
		Monnam(mtmp));
	else if (let == S_RODENT) /* rat swarm */
	    pline("%s summons rats!",
		Monnam(mtmp));
	else if (let == S_SPIDER) /* demonic spiders */
	    pline("%s summons spiders!",
		Monnam(mtmp));
	else if (let == S_WORM) /* can of worms */
	    pline("%s opens up a can of worms!",
		Monnam(mtmp));
	else if (let == S_BAT) /* flapping bats */
	    pline("%s summons a bat swarm!",
		Monnam(mtmp));
	else if (let == S_UMBER) /* hulking brutes */
	    pline("%s summons terrifying creatures!",
		Monnam(mtmp));
	else if (let == S_HUMAN) /* court summoning */
	    pline("%s summons interesting people from around the world!",
		Monnam(mtmp));
	else if (let == S_GOLEM) /* golem mastery */
	    pline("%s transforms a heap of junk into animated beings!",
		Monnam(mtmp));
	else if (let == S_EEL) /* deep sea trouble */
	    pline("%s opens up a can of whoop-ass!",
		Monnam(mtmp));
	else if (let == S_BAD_FOOD) /* mystic nature */
	    pline("%s summons mystic natures!",
		Monnam(mtmp));
	else if (Invisible && !perceives(mtmp->data) &&
				(mtmp->mux != u.ux || mtmp->muy != u.uy))
	    pline("%s summons insects around a spot near you!",
		Monnam(mtmp));
	else if (Displaced && (mtmp->mux != u.ux || mtmp->muy != u.uy))
	    pline("%s summons insects around your displaced image!",
		Monnam(mtmp));
	else
	    pline("%s summons insects!", Monnam(mtmp));
	dmg = 0;
	break;
      }
    case CLC_BLIND_YOU:
	/* note: resists_blnd() doesn't apply here */
	if (!Blinded) {
	    int num_eyes = eyecount(youmonst.data);
	    pline("Scales cover your %s!",
		  (num_eyes == 1) ?
		  body_part(EYE) : makeplural(body_part(EYE)));
	    make_blinded(Half_spell_damage ? 100L : 200L, FALSE);
	    if (!Blind) Your(vision_clears);
	    dmg = 0;
	} else
	    impossible("no reason for monster to cast blindness spell?");
	break;
    case CLC_PARALYZE:
	if ((Antimagic || Free_action) && rn2(20)) {
	    shieldeff(u.ux, u.uy);
	    if (multi >= 0)
		You("stiffen briefly.");
	    nomul(-1);
	} else {
	    if (multi >= 0)
		You("are frozen in place!");
	    dmg = 4 + (int)mtmp->m_lev;
	    if (Half_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
	    nomul(-dmg);
	}
	nomovemsg = 0;
	dmg = 0;
	break;
    case CLC_CONFUSE_YOU:
	if (Antimagic && rn2(20)) {
	    shieldeff(u.ux, u.uy);
	    You_feel("momentarily dizzy.");
	} else {
	    boolean oldprop = !!Confusion;

	    dmg = (int)mtmp->m_lev;
	    if (Half_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
	    make_confused(HConfusion + dmg, TRUE);
	    if (Hallucination)
		You_feel("%s!", oldprop ? "trippier" : "trippy");
	    else
		You_feel("%sconfused!", oldprop ? "more " : "");
	}
	dmg = 0;
	break;
    case CLC_CURE_SELF:
	if (mtmp->mhp < mtmp->mhpmax) {
	    if (canseemon(mtmp))
		pline("%s looks better.", Monnam(mtmp));
	    /* note: player healing does 6d4; this used to do 1d8 */
	    if ((mtmp->mhp += d(3,6)) > mtmp->mhpmax)
		mtmp->mhp = mtmp->mhpmax;
	    dmg = 0;
	}
	break;
    case CLC_OPEN_WOUNDS:

	if (Antimagic && rn2(20)) {
	    shieldeff(u.ux, u.uy);
	    dmg = (dmg + 1) / 2;
	}
	if (dmg <= 5)
	    Your("skin itches badly for a moment.");
	else if (dmg <= 10)
	    pline("Wounds appear on your body!");
	else if (dmg <= 20)
	    pline("Severe wounds appear on your body!");
	else
	    Your("body is covered with painful wounds!");
	break;
    case CLC_VULN_YOU: /* inspired by Sporkhack but enhanced by Amy */
	dmg *= 10;
	switch (rnd(109)) {

		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			u.uprops[DEAC_FIRE_RES].intrinsic += rnz(dmg);
			pline("You are prevented from having fire resistance!");
			break;
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
			u.uprops[DEAC_COLD_RES].intrinsic += rnz(dmg);
			pline("You are prevented from having cold resistance!");
			break;
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			u.uprops[DEAC_SLEEP_RES].intrinsic += rnz(dmg);
			pline("You are prevented from having sleep resistance!");
			break;
		case 16:
		case 17:
			u.uprops[DEAC_DISINT_RES].intrinsic += rnz(dmg);
			pline("You are prevented from having disintegration resistance!");
			break;
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
			u.uprops[DEAC_SHOCK_RES].intrinsic += rnz(dmg);
			pline("You are prevented from having shock resistance!");
			break;
		case 23:
		case 24:
		case 25:
		case 26:
		case 27:
			u.uprops[DEAC_POISON_RES].intrinsic += rnz(dmg);
			pline("You are prevented from having poison resistance!");
			break;
		case 28:
		case 29:
		case 30:
			u.uprops[DEAC_DRAIN_RES].intrinsic += rnz(dmg);
			pline("You are prevented from having drain resistance!");
			break;
		case 31:
		case 32:
			u.uprops[DEAC_SICK_RES].intrinsic += rnz(dmg);
			pline("You are prevented from having sickness resistance!");
			break;
		case 33:
		case 34:
			u.uprops[DEAC_ANTIMAGIC].intrinsic += rnz(dmg);
			pline("You are prevented from having magic resistance!");
			break;
		case 35:
		case 36:
		case 37:
		case 38:
			u.uprops[DEAC_ACID_RES].intrinsic += rnz(dmg);
			pline("You are prevented from having acid resistance!");
			break;
		case 39:
		case 40:
			u.uprops[DEAC_STONE_RES].intrinsic += rnz(dmg);
			pline("You are prevented from having petrification resistance!");
			break;
		case 41:
			u.uprops[DEAC_FEAR_RES].intrinsic += rnz(dmg);
			pline("You are prevented from having fear resistance!");
			break;
		case 42:
		case 43:
		case 44:
			u.uprops[DEAC_SEE_INVIS].intrinsic += rnz(dmg);
			pline("You are prevented from having see invisible!");
			break;
		case 45:
		case 46:
		case 47:
			u.uprops[DEAC_TELEPAT].intrinsic += rnz(dmg);
			pline("You are prevented from having telepathy!");
			break;
		case 48:
		case 49:
		case 50:
			u.uprops[DEAC_WARNING].intrinsic += rnz(dmg);
			pline("You are prevented from having warning!");
			break;
		case 51:
		case 52:
		case 53:
			u.uprops[DEAC_SEARCHING].intrinsic += rnz(dmg);
			pline("You are prevented from having automatic searching!");
			break;
		case 54:
			u.uprops[DEAC_CLAIRVOYANT].intrinsic += rnz(dmg);
			pline("You are prevented from having clairvoyance!");
			break;
		case 55:
		case 56:
		case 57:
		case 58:
		case 59:
			u.uprops[DEAC_INFRAVISION].intrinsic += rnz(dmg);
			pline("You are prevented from having infravision!");
			break;
		case 60:
			u.uprops[DEAC_DETECT_MONSTERS].intrinsic += rnz(dmg);
			pline("You are prevented from having detect monsters!");
			break;
		case 61:
		case 62:
		case 63:
			u.uprops[DEAC_INVIS].intrinsic += rnz(dmg);
			pline("You are prevented from having invisibility!");
			break;
		case 64:
			u.uprops[DEAC_DISPLACED].intrinsic += rnz(dmg);
			pline("You are prevented from having displacement!");
			break;
		case 65:
		case 66:
		case 67:
			u.uprops[DEAC_STEALTH].intrinsic += rnz(dmg);
			pline("You are prevented from having stealth!");
			break;
		case 68:
			u.uprops[DEAC_JUMPING].intrinsic += rnz(dmg);
			pline("You are prevented from having jumping!");
			break;
		case 69:
		case 70:
		case 71:
			u.uprops[DEAC_TELEPORT_CONTROL].intrinsic += rnz(dmg);
			pline("You are prevented from having teleport control!");
			break;
		case 72:
			u.uprops[DEAC_FLYING].intrinsic += rnz(dmg);
			pline("You are prevented from having flying!");
			break;
		case 73:
			u.uprops[DEAC_MAGICAL_BREATHING].intrinsic += rnz(dmg);
			pline("You are prevented from having magical breathing!");
			break;
		case 74:
			u.uprops[DEAC_PASSES_WALLS].intrinsic += rnz(dmg);
			pline("You are prevented from having phasing!");
			break;
		case 75:
		case 76:
			u.uprops[DEAC_SLOW_DIGESTION].intrinsic += rnz(dmg);
			pline("You are prevented from having slow digestion!");
			break;
		case 77:
			u.uprops[DEAC_HALF_SPDAM].intrinsic += rnz(dmg);
			pline("You are prevented from having half spell damage!");
			break;
		case 78:
			u.uprops[DEAC_HALF_PHDAM].intrinsic += rnz(dmg);
			pline("You are prevented from having half physical damage!");
			break;
		case 79:
		case 80:
		case 81:
		case 82:
		case 83:
			u.uprops[DEAC_REGENERATION].intrinsic += rnz(dmg);
			pline("You are prevented from having regeneration!");
			break;
		case 84:
		case 85:
			u.uprops[DEAC_ENERGY_REGENERATION].intrinsic += rnz(dmg);
			pline("You are prevented from having mana regeneration!");
			break;
		case 86:
		case 87:
		case 88:
			u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += rnz(dmg);
			pline("You are prevented from having polymorph control!");
			break;
		case 89:
		case 90:
		case 91:
		case 92:
		case 93:
			u.uprops[DEAC_FAST].intrinsic += rnz(dmg);
			pline("You are prevented from having speed!");
			break;
		case 94:
		case 95:
		case 96:
			u.uprops[DEAC_REFLECTING].intrinsic += rnz(dmg);
			pline("You are prevented from having reflection!");
			break;
		case 97:
		case 98:
		case 99:
			u.uprops[DEAC_FREE_ACTION].intrinsic += rnz(dmg);
			pline("You are prevented from having free action!");
			break;
		case 100:
			u.uprops[DEAC_HALLU_PARTY].intrinsic += rnz(dmg);
			pline("You are prevented from hallu partying!");
			break;
		case 101:
			u.uprops[DEAC_DRUNKEN_BOXING].intrinsic += rnz(dmg);
			pline("You are prevented from drunken boxing!");
			break;
		case 102:
			u.uprops[DEAC_STUNNOPATHY].intrinsic += rnz(dmg);
			pline("You are prevented from having stunnopathy!");
			break;
		case 103:
			u.uprops[DEAC_NUMBOPATHY].intrinsic += rnz(dmg);
			pline("You are prevented from having numbopathy!");
			break;
		case 104:
			u.uprops[DEAC_FREEZOPATHY].intrinsic += rnz(dmg);
			pline("You are prevented from having freezopathy!");
			break;
		case 105:
			u.uprops[DEAC_STONED_CHILLER].intrinsic += rnz(dmg);
			pline("You are prevented from being a stoned chiller!");
			break;
		case 106:
			u.uprops[DEAC_CORROSIVITY].intrinsic += rnz(dmg);
			pline("You are prevented from having corrosivity!");
			break;
		case 107:
			u.uprops[DEAC_FEAR_FACTOR].intrinsic += rnz(dmg);
			pline("You are prevented from having an increased fear factor!");
			break;
		case 108:
			u.uprops[DEAC_BURNOPATHY].intrinsic += rnz(dmg);
			pline("You are prevented from having burnopathy!");
			break;
		case 109:
			u.uprops[DEAC_SICKOPATHY].intrinsic += rnz(dmg);
			pline("You are prevented from having sickopathy!");
			break;
	}
	dmg = 0;
	break;

    default:
	impossible("mcastu: invalid clerical spell (%d)", spellnum);
	dmg = 0;
	break;
    }

    if (dmg) mdamageu(mtmp, dmg);
}

STATIC_DCL
boolean
is_undirected_spell(adtyp, spellnum)
unsigned int adtyp;
int spellnum;
{
    if (adtyp == AD_SPEL) {
	switch (spellnum) {
	case MGC_CLONE_WIZ:
	case MGC_SUMMON_MONS:
	case MGC_SUMMON_GHOST:
	case MGC_AGGRAVATION:
	case MGC_DISAPPEAR:
	case MGC_HASTE_SELF:
	case MGC_CURE_SELF:
	case MGC_CALL_UNDEAD:
	    return TRUE;
	default:
	    break;
	}
    } else if (adtyp == AD_CLRC) {
	switch (spellnum) {
	case CLC_INSECTS:
	case CLC_CURE_SELF:
	case CLC_AGGRAVATION:
	    return TRUE;
	default:
	    break;
	}
    }
    return FALSE;
}

STATIC_DCL
boolean
is_melee_spell(adtyp, spellnum)
unsigned int adtyp;
int spellnum;
{
    if (adtyp == AD_SPEL) {
	switch (spellnum) {
	case MGC_DEATH_TOUCH:
	    return TRUE;
	default:
	    break;
	}
    } else if (adtyp == AD_CLRC) {
	switch (spellnum) {
	default:
	    break;
	}
    }
    return FALSE;
}


/* Some spells are useless under some circumstances. */
STATIC_DCL
boolean
spell_would_be_useless(mtmp, adtyp, spellnum)
struct monst *mtmp;
unsigned int adtyp;
int spellnum;
{
    /* Some spells don't require the player to really be there and can be cast
     * by the monster when you're invisible, yet still shouldn't be cast when
     * the monster doesn't even think you're there.
     * This check isn't quite right because it always uses your real position.
     * We really want something like "if the monster could see mux, muy".
     */
    boolean mcouldseeu = couldsee(mtmp->mx, mtmp->my);

    if (adtyp == AD_SPEL) {
	/* aggravate monsters, etc. won't be cast by peaceful monsters */
	if (mtmp->mpeaceful && (spellnum == MGC_AGGRAVATION || !is_undirected_spell(AD_SPEL,spellnum) ||
               spellnum == MGC_SUMMON_MONS || spellnum == MGC_SUMMON_GHOST || spellnum == MGC_CLONE_WIZ || spellnum == MGC_CREATE_POOL ||
               spellnum == MGC_CALL_UNDEAD))
	    return TRUE;
	/* Don't go making everything else bonkers if you're peaceful! */
	if (spellnum == MGC_ENRAGE && (mtmp->mpeaceful || mtmp->mtame)) {
		return TRUE;
	}
	/* haste self when already fast */
	if (mtmp->permspeed == MFAST && spellnum == MGC_HASTE_SELF)
	    return TRUE;
	/* invisibility when already invisible */
	if ((mtmp->minvis || mtmp->invis_blkd) && spellnum == MGC_DISAPPEAR)
	    return TRUE;
	/* peaceful monster won't cast invisibility if you can't see invisible,
	   same as when monsters drink potions of invisibility.  This doesn't
	   really make a lot of sense, but lets the player avoid hitting
	   peaceful monsters by mistake */
	if (mtmp->mpeaceful && !See_invisible && spellnum == MGC_DISAPPEAR)
	    return TRUE;
	/* healing when already healed */
	if (mtmp->mhp == mtmp->mhpmax && spellnum == MGC_CURE_SELF)
	    return TRUE;

	/* summon monsters less often if the monster is low level --Amy */
	if ( (spellnum == MGC_SUMMON_MONS || spellnum == MGC_SUMMON_GHOST || spellnum == MGC_CALL_UNDEAD) && mtmp->m_lev < rnd(50) && rn2(5)) return TRUE;

	/* don't summon monsters if it doesn't think you're around */ /* or at least not as often --Amy */
	if (!mcouldseeu && ( (spellnum == MGC_SUMMON_MONS && rn2(5)) ||
		(spellnum == MGC_CALL_UNDEAD && rn2(5)) || 		(spellnum == MGC_SUMMON_GHOST && rn2(5)) ||
		(!mtmp->iswiz && spellnum == MGC_CLONE_WIZ)))
	    return TRUE;
	/* only lichs can cast call undead */ /* well, not anymore --Amy */
	/*if (mtmp->data->mlet != S_LICH && spellnum == MGC_CALL_UNDEAD)
	    return TRUE;*/
	/* pools can only be created in certain locations and then only
	 * rarely unless you're carrying the amulet.
	 */
	if ((levl[u.ux][u.uy].typ != ROOM && levl[u.ux][u.uy].typ != CORR /* lowered chance even with amulet --Amy */
		|| (!u.uhave.amulet && rn2(10)) || rn2(3) ) && spellnum == MGC_CREATE_POOL)
	    return TRUE;
	if ((!mtmp->iswiz || (flags.no_of_wizards > 1 && rn2(20)) )
						&& spellnum == MGC_CLONE_WIZ)
	    return TRUE;
    } else if (adtyp == AD_CLRC) {
	/* summon insects/sticks to snakes won't be cast by peaceful monsters */
	if (mtmp->mpeaceful && (spellnum == CLC_INSECTS || !is_undirected_spell(AD_CLRC,spellnum) || spellnum == CLC_AGGRAVATION ) )
	    return TRUE;
	/* healing when already healed */
	if (mtmp->mhp == mtmp->mhpmax && spellnum == CLC_CURE_SELF)
	    return TRUE;

	/* summon monsters less often if the monster is low level --Amy */
	if ( spellnum == CLC_INSECTS && mtmp->m_lev < rnd(50) && rn2(5)) return TRUE;

	/* don't summon insects if it doesn't think you're around */ /* or at least not as often --Amy */
	if (!mcouldseeu && spellnum == CLC_INSECTS && rn2(5) )
	    return TRUE;
	/* blindness spell on blinded player */
	if (Blinded && spellnum == CLC_BLIND_YOU)
	    return TRUE;
    }
    return FALSE;
}

#endif /* OVLB */
#ifdef OVL0

/* convert 1..10 to 0..9; add 10 for second group (spell casting) */
#define ad_to_typ(k) (10 + (int)k - 1)

int
buzzmu(mtmp, mattk)		/* monster uses spell (ranged) */
	register struct monst *mtmp;
	register struct attack  *mattk;
{
	/* don't print constant stream of curse messages for 'normal'
	   spellcasting monsters at range */
	if (mattk->adtyp > AD_SPC2)
	    return(0);

	if (mtmp->mcan) {
	    cursetxt(mtmp, FALSE);
	    return(0);
	}
	if(lined_up(mtmp) && rn2(3)) {
	    nomul(0);
	    if(mattk->adtyp && (mattk->adtyp < 11)) { /* no cf unsigned >0 */
		if(canseemon(mtmp))
		    pline("%s zaps you with a %s!", Monnam(mtmp),
			  flash_types[ad_to_typ(mattk->adtyp)]);
		buzz(-ad_to_typ(mattk->adtyp), (rn2(2) ? (int)mattk->damn : (int)mattk->damd ),
		     mtmp->mx, mtmp->my, sgn(tbx), sgn(tby));
	    } else impossible("Monster spell %d cast", mattk->adtyp-1);
	}
	return(1);
}

#endif /* OVL0 */

/*mcastu.c*/

STATIC_PTR void
set_litZ(x,y,val)
int x, y;
genericptr_t val;
{
	if (val)
	    levl[x][y].lit = 1;
	else {
	    levl[x][y].lit = 0;
	    snuff_light_source(x, y);
	}
}

