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
#define MGC_NUMB_YOU	21
#define MGC_BURN_YOU	22
#define MGC_ESCALATION	23

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
#define CLC_SEPARATION	 14
#define CLC_FEAR_YOU	 15
#define CLC_FREEZE_YOU	 16
#define CLC_STAT_DRAIN	 17

STATIC_DCL void cursetxt(struct monst *,BOOLEAN_P);
STATIC_DCL int choose_magic_spell(int);
STATIC_DCL int choose_clerical_spell(int);
STATIC_DCL void cast_wizard_spell(struct monst *, int,int);
STATIC_DCL void cast_cleric_spell(struct monst *, int,int);
STATIC_DCL boolean is_undirected_spell(unsigned int,int);
STATIC_DCL boolean is_melee_spell(unsigned int,int);
STATIC_DCL boolean spell_would_be_useless(struct monst *,unsigned int,int);
STATIC_PTR void set_litZ(int,int,void *);
STATIC_DCL boolean arcaniumfail(void);

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
	    else if ((Invis && !perceives(mtmp->data) && (StrongInvis || !rn2(3)) &&
			(mtmp->mux != u.ux || mtmp->muy != u.uy)) ||
		    (youmonst.m_ap_type == M_AP_OBJECT &&
			youmonst.mappearance == STRANGE_OBJECT) ||
		    u.uundetected)
		point_msg = "and curses in your general direction";
	    else if (Displaced && (StrongDisplaced || !rn2(3)) && (mtmp->mux != u.ux || mtmp->muy != u.uy))
		point_msg = "and curses at your displaced image";
	    else
		point_msg = "at you, then curses";

	    pline("%s points %s.", Monnam(mtmp), point_msg);
	} else if ((!(moves % 4) || !rn2(4))) {
	    if (flags.soundok && !issoviet) Norep("You hear a mumbled curse.");
	}
}

#endif /* OVL0 */
#ifdef OVLB

/* will a monster fail to cast a spell? this happens if you wear arcanium equipment --Amy
 * it's not a bug that this can also affect spellcasting pets; returns TRUE if the monster actually fails to cast */
STATIC_OVL boolean
arcaniumfail()
{
	if (uwep && objects[uwep->otyp].oc_material == MT_ARCANIUM && !rn2(20)) {
		return TRUE;
	}
	if (u.twoweap && uswapwep && objects[uswapwep->otyp].oc_material == MT_ARCANIUM && !rn2(20)) {
		return TRUE;
	}
	if (uarm && objects[uarm->otyp].oc_material == MT_ARCANIUM && !rn2(20)) {
		return TRUE;
	}
	if (uarmc && objects[uarmc->otyp].oc_material == MT_ARCANIUM && !rn2(20)) {
		return TRUE;
	}
	if (uarmh && objects[uarmh->otyp].oc_material == MT_ARCANIUM && !rn2(20)) {
		return TRUE;
	}
	if (uarms && objects[uarms->otyp].oc_material == MT_ARCANIUM && !rn2(20)) {
		return TRUE;
	}
	if (uarmg && objects[uarmg->otyp].oc_material == MT_ARCANIUM && !rn2(20)) {
		return TRUE;
	}
	if (uarmf && objects[uarmf->otyp].oc_material == MT_ARCANIUM && !rn2(20)) {
		return TRUE;
	}
	if (uarmu && objects[uarmu->otyp].oc_material == MT_ARCANIUM && !rn2(20)) {
		return TRUE;
	}
	if (uamul && objects[uamul->otyp].oc_material == MT_ARCANIUM && !rn2(20)) {
		return TRUE;
	}
	if (uimplant && objects[uimplant->otyp].oc_material == MT_ARCANIUM && !rn2(20)) {
		return TRUE;
	}
	if (uleft && objects[uleft->otyp].oc_material == MT_ARCANIUM && !rn2(20)) {
		return TRUE;
	}
	if (uright && objects[uright->otyp].oc_material == MT_ARCANIUM && !rn2(20)) {
		return TRUE;
	}
	if (ublindf && objects[ublindf->otyp].oc_material == MT_ARCANIUM && !rn2(20)) {
		return TRUE;
	}

	return FALSE;
}

/* convert a level based random selection into a specific mage spell;
   inappropriate choices will be screened out by spell_would_be_useless() */
STATIC_OVL int
choose_magic_spell(spellval)
int spellval;
{
	if (EnthuActive) spellval = rnd(45);

    switch (spellval) {
    case 45:
	if (!rn2(25)) return MGC_DIVINE_WRATH; /* waaaay too overpowered, so this will appear much more rarely --Amy */
	else return MGC_ENRAGE;
    case 44:
	return MGC_ENRAGE; /* we reduce the risk of getting a touch of death */
    case 43:
	return MGC_DEATH_TOUCH;
    case 42:
	return MGC_ESCALATION;
    case 41:
	if (!rn2(25)) return MGC_SUMMON_GHOST; /* Should be rare --Amy */
	else return MGC_CLONE_WIZ;
    case 40:
    case 39:
	return MGC_HASTE_SELF;
    case 38:
	return MGC_SUMMON_MONS;
    case 37:
    case 36:
	return MGC_AGGRAVATION;
    case 35:
	if (!rn2(2)) return MGC_LEVITATE;
	else if (!rn2(5)) return MGC_MEGALOAD;
	else return MGC_CREATE_POOL;
    case 34:
	return MGC_NUMB_YOU;
    case 33:
	return MGC_CURSE_ITEMS;
    case 32:
	return MGC_CALL_UNDEAD;
    case 31:
	if (!rn2(4)) return MGC_WITHER;
	else if (!rn2(2)) return MGC_DAMAGE_ARMR;
	else return MGC_DESTRY_ARMR;
    case 30:
	return MGC_BURN_YOU;
    case 29:
	return MGC_WEAKEN_YOU;
    case 28:
	return MGC_NUMB_YOU;
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
	return MGC_ESCALATION;
    case 18:
	if (!rn2(25)) return MGC_SUMMON_GHOST; /* Should be rare --Amy */
	else return MGC_CLONE_WIZ;
    case 17:
    case 16:
	return MGC_HASTE_SELF;
    case 15:
	return MGC_SUMMON_MONS;
    case 14:
    case 13:
	return MGC_AGGRAVATION;
    case 12:
	if (!rn2(2)) return MGC_LEVITATE;
	else if (!rn2(5)) return MGC_MEGALOAD;
	else return MGC_CREATE_POOL;
    case 11:
	return MGC_NUMB_YOU;
    case 10:
	return MGC_CURSE_ITEMS;
    case 9:
	return MGC_CALL_UNDEAD;
    case 8:
	if (!rn2(4)) return MGC_WITHER;
	else if (!rn2(2)) return MGC_DAMAGE_ARMR;
	else return MGC_DESTRY_ARMR;
    case 7:
	return MGC_BURN_YOU;
    case 6:
	return MGC_WEAKEN_YOU;
    case 5:
	return MGC_NUMB_YOU;
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
	if (EnthuActive) spellnum = rnd(41);

    switch (spellnum) {
    case 41:
	return CLC_GEYSER;
    case 40:
	return CLC_FIRE_PILLAR;
    case 39:
	return CLC_LIGHTNING;
    case 38:
	return CLC_STAT_DRAIN;
    case 37:
	return CLC_CURSE_ITEMS;
    case 36:
	if (rn2(2)) return CLC_RANDOM;
	else if (!rn2(50)) return CLC_AGGRAVATION;
	else return CLC_INSECTS;
    case 35:
	return CLC_FREEZE_YOU;
    case 34:
	return CLC_BLIND_YOU;
    case 33:
	return CLC_FEAR_YOU;
    case 32:
	return CLC_PARALYZE;
    case 31:
	return CLC_SEPARATION;
    case 30:
	return CLC_CONFUSE_YOU;
    case 29:
	return CLC_CURE_SELF;
    case 28:
	return (rn2(10) ? CLC_OPEN_WOUNDS : CLC_VULN_YOU);
    case 27:
	return CLC_GEYSER;
    case 26:
	return CLC_FIRE_PILLAR;
    case 25:
	return CLC_LIGHTNING;
    case 24:
	return CLC_STAT_DRAIN;
    case 23:
	return CLC_CURSE_ITEMS;
    case 22:
	if (rn2(2)) return CLC_RANDOM;
	else if (!rn2(50)) return CLC_AGGRAVATION;
	else return CLC_INSECTS;
    case 21:
	return CLC_FREEZE_YOU;
    case 20:
	return CLC_BLIND_YOU;
    case 19:
	return CLC_FEAR_YOU;
    case 18:
	return CLC_PARALYZE;
    case 17:
	return CLC_SEPARATION;
    case 16:
	return CLC_CONFUSE_YOU;
    case 15:
	return CLC_CURE_SELF;
    case 14:
	return (rn2(10) ? CLC_OPEN_WOUNDS : CLC_VULN_YOU);
    case 13:
	return CLC_GEYSER;
    case 12:
	return CLC_FIRE_PILLAR;
    case 11:
	return CLC_LIGHTNING;
    case 10:
	return CLC_STAT_DRAIN;
    case 9:
	return CLC_CURSE_ITEMS;
    case 8:
	if (rn2(2)) return CLC_RANDOM;
	else if (!rn2(50)) return CLC_AGGRAVATION;
	else return CLC_INSECTS;
    case 7:
	return CLC_FREEZE_YOU;
    case 6:
	return CLC_BLIND_YOU;
    case 5:
	return CLC_FEAR_YOU;
    case 4:
	return CLC_PARALYZE;
    case 3:
	return CLC_SEPARATION;
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
	boolean monsterniman = FALSE;

	if (MON_WEP(mtmp)) { /* niman monster lightsaber form */
		struct obj *monweapon;
		monweapon = MON_WEP(mtmp);
		if (monweapon) {
			if (is_lightsaber(monweapon) && monweapon->lamplit) {
				ml += 5;
				if (mtmp->data->geno & G_UNIQ) ml += 5;
				monsterniman = TRUE;
			}
		}

	}

	int spellcasttype = mattk->adtyp;
	if (EnthuActive && !rn2(10)) {
		if (!rn2(3)) spellcasttype = AD_SPEL;
		else if (!rn2(2)) spellcasttype = AD_CLRC;
		else if (!rn2(100)) spellcasttype = AD_DISN;
		else {
			switch (rnd(9)) {
				case 1:
					spellcasttype = AD_MAGM; break;
				case 2:
					spellcasttype = AD_FIRE; break;
				case 3:
					spellcasttype = AD_COLD; break;
				case 4:
					spellcasttype = AD_ELEC; break;
				case 5:
					spellcasttype = AD_SLEE; break;
				case 6:
					spellcasttype = AD_DRST; break;
				case 7:
					spellcasttype = AD_ACID; break;
				case 8:
					spellcasttype = AD_LITE; break;
				case 9:
					spellcasttype = AD_SPC2; break;
			}
		}
	}

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
	if ((spellcasttype == AD_SPEL || spellcasttype == AD_CLRC || spellcasttype == AD_CAST) && ml) {
	    int cnt = 40;

	    do {
		spellnum = (issoviet ? rn2(ml) : rn3(ml));
		/* Casting level is limited by available energy */
		spellev = spellnum / 7 + 1;
		if (spellev > 10) spellev = 10;
		if (spellev * 5 > mtmp->m_en) {
		    spellev = mtmp->m_en / 5;
		    spellnum = (spellev - 1) * 7 + 1;
		}
		if (EnthuActive) {
		    spellnum = rn2(2) ? choose_magic_spell(spellnum) : choose_clerical_spell(spellnum);
		    while (rn2(7) && !spellnum) {
			spellnum = rn2(2) ? choose_magic_spell(spellnum) : choose_clerical_spell(spellnum);
		    }
		} if (spellcasttype == AD_SPEL)
		    spellnum = choose_magic_spell(spellnum);
		else if (spellcasttype == AD_CLRC)
		    spellnum = choose_clerical_spell(spellnum);
		else { /* AD_CAST - often reroll when psybolt or open wounds is chosen --Amy */

			if ((moves % 4 == 0) || ((moves + 1) % 4 == 0)) {
				spellnum = choose_clerical_spell(spellnum);
				spellcasttype = AD_CLRC;
			} else {
				spellnum = choose_magic_spell(spellnum);
				spellcasttype = AD_SPEL;
			}

			while (rn2(7) && !spellnum) {

				if ((moves % 4 == 0) || ((moves + 1) % 4 == 0)) {
					spellnum = choose_clerical_spell(spellnum);
					spellcasttype = AD_CLRC;
				} else {
					spellnum = choose_magic_spell(spellnum);
					spellcasttype = AD_SPEL;
				}

			}

		}
		/* not trying to attack?  don't allow directed spells */
		if (!thinks_it_foundyou) {
		    if ( (!is_undirected_spell(spellcasttype, spellnum) && rn2(250) ) || is_melee_spell(spellcasttype, spellnum) || spell_would_be_useless(mtmp, spellcasttype, spellnum)) {
			if (foundyou)
			    impossible("spellcasting monster found you and doesn't know it?");
			return 0;
		    }
		    break;
		}
	    } while(--cnt > 0 &&
		    spell_would_be_useless(mtmp, spellcasttype, spellnum));
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

	/* monster unable to cast spells?
	 * Amy note: it is not a bug that we're not using "autismweaponcheck" here - weapon has to be main one */
	if (mtmp->mcan || arcaniumfail() || (mtmp->data == &mons[PM_KLAPPTNIX]) || (RngeAntimagicA && !rn2(10)) || (RngeAntimagicB && !rn2(5)) || (RngeAntimagicC && !rn2(2)) || (RngeAntimagicD) || (RngeSpellDisruption && !rn2(5)) || mtmp->m_en < 5 || (mtmp->mspec_used && !(mtmp->data == &mons[PM_MAND_PENDING__MAGIC_SPELL___])) || !ml || u.antimagicshell || (uarmh && uarmh->otyp == HELM_OF_ANTI_MAGIC) || (uwep && uwep->otyp == DARK_SWORD) || (u.twoweap && uswapwep && uswapwep->otyp == DARK_SWORD) || (uarmc && uarmc->oartifact == ART_SHELLY && (moves % 3 == 0)) || (uarmc && uarmc->oartifact == ART_BLACK_VEIL_OF_BLACKNESS) || (uarmc && uarmc->oartifact == ART_ARABELLA_S_WAND_BOOSTER) || (uarmu && uarmu->oartifact == ART_ANTIMAGIC_SHELL) || (uwep && uwep->oartifact == ART_WINCHESTER_PREMIUM) || (uarmu && uarmu->oartifact == ART_ANTIMAGIC_FIELD) || Role_if(PM_UNBELIEVER) || (uwep && uwep->oartifact == ART_ARK_OF_THE_COVENANT) || (uwep && uwep->oartifact == ART_LONG_SWORD_OF_ETERNITY) || (uwep && uwep->oartifact == ART_HEAVY_CROSSBOW_OF_ETERNITY) || (uarmc && uarmc->oartifact == ART_SPELL_WARDED_WRAPPINGS_OF_) || (uwep && uwep->oartifact == ART_ANTIMAGICBANE) || (uarmc && (itemhasappearance(uarmc, APP_VOID_CLOAK) || itemhasappearance(uarmc, APP_SHELL_CLOAK)) && !rn2(5))  ) {
	    cursetxt(mtmp, is_undirected_spell(spellcasttype, spellnum));
	    return(0);
	}

	if (spellcasttype == AD_SPEL || spellcasttype == AD_CLRC || spellcasttype == AD_CAST) {
	    /*
	     * Spell use (especially MGC) is more common in Slash'EM.
	     * Still using mspec_used, just so monsters don't go bonkers.
	     */
#if 0
	    mtmp->mspec_used = 10 - mtmp->m_lev;
	    if (mtmp->mspec_used < 2) mtmp->mspec_used = 2;
#endif
	    mtmp->mspec_used = rn2(15) - mtmp->m_lev;
	    if (spellcasttype == AD_SPEL)
		mtmp->mspec_used = mtmp->mspec_used > 0 ? 2 : 0;
	    else if (mtmp->mspec_used < 2) mtmp->mspec_used = 2;
	}

	/* monster can cast spells, but is casting a directed spell at the
	   wrong place?  If so, give a message, and return.  Do this *after*
	   penalizing mspec_used. */
	if (!foundyou && thinks_it_foundyou &&
		!is_undirected_spell(spellcasttype, spellnum)) {
	    pline("%s casts a spell at %s!",
		canseemon(mtmp) ? Monnam(mtmp) : "Something",
		levl[mtmp->mux][mtmp->muy].typ == WATER
		    ? "empty water" : "thin air");
	    return(0);
	}

	nomul(0, 0, FALSE);

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

	if (monsterniman) {
		chance += 25;
		if (mtmp->data->geno & G_UNIQ) chance += 25;
	}

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
	if (canspotmon(mtmp) || !is_undirected_spell(spellcasttype, spellnum)) {
	    pline("%s casts a spell%s!",
		  canspotmon(mtmp) ? Monnam(mtmp) : "Something",
		  is_undirected_spell(spellcasttype, spellnum) ? "" :
		  (Invisible && !perceives(mtmp->data) && (StrongInvis || !rn2(3)) && 
		   (mtmp->mux != u.ux || mtmp->muy != u.uy)) ?
		  " at a spot near you" :
		  (Displaced && (StrongDisplaced || !rn2(3)) && (mtmp->mux != u.ux || mtmp->muy != u.uy)) ?
		  " at your displaced image" :
		  " at you");
	} else if (flags.soundok && !issoviet) You_hear("a mumbled incantation.");

/*
 *	As these are spells, the damage is related to the level
 *	of the monster casting the spell.
 */
	if (!foundyou) {
	    /*dmg = 0;*/
	    if (spellcasttype != AD_SPEL && spellcasttype != AD_CLRC && spellcasttype != AD_CAST) {
		impossible(
	      "%s casting non-hand-to-hand version of hand-to-hand spell %d?",
			   Monnam(mtmp), spellcasttype);
		return(0);
	    }
	} /*else*/ if (mattk->damd)
	    dmg = d((int)((ml/2) + mattk->damn), (int)mattk->damd);
	else dmg = d((int)((ml/2) + 1), 6);
	if (Half_spell_damage && rn2(2) ) dmg = (dmg+1) / 2;
	if (StrongHalf_spell_damage && rn2(2) ) dmg = (dmg+1) / 2;

	ret = 1;

	switch (spellcasttype) {

	    case AD_FIRE:
		pline("You're enveloped in flames.");

		if (Race_if(PM_LOWER_ENT)) dmg *= 2;

		if((Fire_resistance && rn2(StrongFire_resistance ? 20 : 5)) || FireImmunity) {
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
		if((Cold_resistance && rn2(StrongCold_resistance ? 20 : 5)) || ColdImmunity ) {
			shieldeff(u.ux, u.uy);
			pline("But you resist the effects.");
			dmg = 0;
		}
		break;
	    case AD_ACID:
		pline("You're covered in acid.");
		if((Acid_resistance && rn2(StrongAcid_resistance ? 20 : 5)) || AcidImmunity) {
			shieldeff(u.ux, u.uy);
			pline("But you resist the effects.");
			dmg = 0;
		}
		break;
	    case AD_ELEC:
		pline("You're shocked.");
		if((Shock_resistance && rn2(StrongShock_resistance ? 20 : 5)) || ShockImmunity ) {
			shieldeff(u.ux, u.uy);
			pline("But you resist the effects.");
			dmg = 0;
		}
		break;
	    case AD_DRST:
		pline("You're poisoned.");
		if(Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) {
			shieldeff(u.ux, u.uy);
			pline("But you resist the effects.");
			dmg = 0;
		}
		break;
	    case AD_LITE:
		pline("You're irradiated by light.");

		if (uarmh && uarmh->oartifact == ART_SECURE_BATHMASTER && rn2(20) ) {
			shieldeff(u.ux, u.uy);
			pline("But you resist the effects.");
			dmg = 0;
		} else if (maybe_polyd(is_vampire(youmonst.data), Race_if(PM_VAMPIRE)) || Role_if(PM_GOFF) ) {
			dmg *= 2; /* vampires are susceptible to sunlight --Amy */
			pline("Your pale skin is seared!");
		}
		break;
	    case AD_SPC2:
		if ((Psi_resist && rn2(StrongPsi_resist ? 100 : 20)) || obsidianprotection()) {
			shieldeff(u.ux, u.uy);
			pline("Something focuses on your mind, but you resist the effects.");
			dmg = 0;
			break;
		}

		pline("Your %s spins wildly.", body_part(HEAD) );

		switch (rnd(10)) {

			case 1:
			case 2:
			case 3:
				make_confused(HConfusion + dmg, FALSE);
				break;
			case 4:
			case 5:
			case 6:
				make_stunned(HStun + dmg, FALSE);
				break;
			case 7:
				make_confused(HConfusion + dmg, FALSE);
				make_stunned(HStun + dmg, FALSE);
				break;
			case 8:
				make_hallucinated(HHallucination + dmg, FALSE, 0L);
				break;
			case 9:
				make_feared(HFeared + dmg, FALSE);
				break;
			case 10:
				make_numbed(HNumbed + dmg, FALSE);
				break;

		}
		if (!rn2(200)) {
			forget(rnd(5), FALSE);
			pline("You forget some important things...");
		}
		if (!rn2(200)) {
			losexp("psionic drain", FALSE, TRUE);
		}
		if (!rn2(200)) {
			adjattrib(A_INT, -1, 1, TRUE);
			adjattrib(A_WIS, -1, 1, TRUE);
		}
		if (!rn2(200)) {
			pline("You scream in pain!");
			wake_nearby();
		}
		if (!rn2(200)) {
			badeffect();
		}
		if (!rn2(5)) increasesanity(rnz(5));

		break;
	    case AD_DISN:
		verbalize("Avada Kedavra!");
		if((Antimagic && rn2(StrongAntimagic ? 20 : 5) ) || nonliving(youmonst.data) || is_demon(youmonst.data) || Death_resistance || Invulnerable || ((PlayerInBlockHeels || PlayerInWedgeHeels) && tech_inuse(T_EXTREME_STURDINESS) && !rn2(2) ) || (StrongWonderlegs && !rn2(10) && Wounded_legs) || (Stoned_chiller && Stoned && !(u.stonedchilltimer) && !rn2(3)) ) {
			shieldeff(u.ux, u.uy);
			pline("But you resist the effects.");
			dmg = 0;
		}
		else if (!PlayerResistsDeathRays && !rn2(20)) {
		    u.youaredead = 1;
		    killer_format = KILLED_BY_AN;
		    killer = "Avada Kedavra curse";
		    done(DIED);
		    u.youaredead = 0;
		    return 1; /* lifesaved */
		}
		break;
	    case AD_MAGM:
		You("are hit by a shower of missiles!");
		if(Antimagic && !Race_if(PM_KUTAR) && rn2(StrongAntimagic ? 20 : 5) ) {
			shieldeff(u.ux, u.uy);
			pline_The("missiles bounce off!");
			dmg = 0;
		}
		break;
	    case AD_SPEL:	/* wizard spell */
	    case AD_CLRC:       /* clerical spell */
	    {
		if (spellcasttype == AD_SPEL)
		    cast_wizard_spell(mtmp, dmg, spellnum);
		else
		    cast_cleric_spell(mtmp, dmg, spellnum);
		dmg = 0; /* done by the spell casting functions */
		break;
	    }
	    case AD_CAST:       /* clerical spell */
	    {
		if ((moves % 4 == 0) || ((moves + 1) % 4 == 0))
		    cast_cleric_spell(mtmp, dmg, spellnum);
		else
		    cast_wizard_spell(mtmp, dmg, spellnum);
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
	if (nonliving(youmonst.data) || is_demon(youmonst.data) || Death_resistance) {
	    You("seem no deader than before.");
	} else if ((!Antimagic || rn2(StrongAntimagic ? 20 : 5)) && rn2(mtmp->m_lev) > 12) {
	    if (Hallucination) {
		You("have an out of body experience.");
	    } else if (!rnd(50) && !Antimagic) {
		u.youaredead = 1;
		killer_format = KILLED_BY_AN;
		killer = "touch of death";
		done(DIED);
		u.youaredead = 0;
	    } else {
			dmg = d(8,6);
			/* Magic resistance or half spell damage will cut this in half... */
			/* and also prevent a reduction of maximum hit points */
			if (Antimagic || (Half_spell_damage && rn2(2)) || (StrongHalf_spell_damage && rn2(2)) ) {
				shieldeff(u.ux, u.uy);
				dmg /= 2;
				u.uhpmax -= dmg/2;
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				You_feel("a loss of life.");
				losehp(dmg,"touch of death",KILLED_BY_AN);
			}
			else {
			You_feel("drained...");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vy odin shag blizhe k provalu v nastoyashcheye vremya. Pozdravleniya." : "Doaing!");
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
	    pline(FunnyHallu ? "Huh - the ground suddenly turned into a swimming pool!" : "A pool appears beneath you!");
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
	if (MysteryResist && rn2(2)) {
		dmg = 0;
		You("resist the megaload spell!");
		break;
	}
	if (chitinprotection() && rn2(2)) {
		dmg = 0;
		You("resist the megaload spell!");
		break;
	}
	if (StrongMysteryResist && rn2(2)) {
		dmg = 0;
		You("resist the megaload spell!");
		break;
	}

	if ((otmp = mksobj(LOADSTONE, TRUE, FALSE, FALSE)) != (struct obj *)0) {
	pline(FunnyHallu ? "Aww, something's killing your good feelings!" : "You feel burdened");
	otmp->quan = 1;
	otmp->owt = weight(otmp);
	if (pickup_object(otmp, 1, FALSE, TRUE) <= 0) {
	obj_extract_self(otmp);
	place_object(otmp, u.ux, u.uy);
	newsym(u.ux, u.uy); }
	}

	dmg = 0;
	break;

    case MGC_LEVITATE:
	pline(FunnyHallu ? "Wow... you're suddenly walking on air!" : "You float up!");
	HLevitation &= ~I_SPECIAL;
	incr_itimeout(&HLevitation, rnz(50));
	flags.botl = TRUE;

	dmg = 0;
	break;

    case MGC_CLONE_WIZ:
	if (mtmp->iswiz && (flags.no_of_wizards == 1 || !rn2(20)) ) { /* let's have a small chance of triple trouble --Amy */
	    if (flags.no_of_wizards == 1) pline(FunnyHallu ? "Doublevision!" : "Double Trouble...");
	    else pline(FunnyHallu ? "Triplevision!" : "Triple Trouble...");
	    clonewiz();
	    dmg = 0;
	} else
	    pline(FunnyHallu ? "For a moment you had triplevision, but seeing double is funny enough." : "For a moment you saw another Wizard, but it disappeared.");
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
	    if (Invisible && !perceives(mtmp->data) && (StrongInvis || !rn2(3)) &&
				    (mtmp->mux != u.ux || mtmp->muy != u.uy))
		pline("%s around a spot near you!", mappear);
	    else if (Displaced && (StrongDisplaced || !rn2(3)) && (mtmp->mux != u.ux || mtmp->muy != u.uy))
		pline("%s around your displaced image!", mappear);
	    else
		pline("%s from nowhere!", mappear);
	}

	if (mtmp->data == &mons[PM_WOK]) {
		int attempts = 0;
		struct permonst *pm = 0;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

newbossRLL:
		do {
			pm = rndmonst();
			attempts++;
			if (attempts && (attempts % 10000 == 0)) u.mondiffhack++;
			if (!rn2(2000)) reset_rndmonst(NON_PM);

		} while ( (!pm || (pm && !(pm->msound == MS_SUPERMAN ))) && attempts < 50000);

		if (!pm && rn2(50) ) {
			attempts = 0;
			goto newbossRLL;
		}
		if (pm && !(pm->msound == MS_SUPERMAN) && rn2(50) ) {
			attempts = 0;
			goto newbossRLL;
		}

		if (pm) (void) makemon(pm, 0, 0, MM_ANGRY|MM_FRENZIED);

		u.mondiffhack = 0;
		u.aggravation = 0;

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
	    if (Invisible && !perceives(mtmp->data) && (StrongInvis || !rn2(3)) &&
				    (mtmp->mux != u.ux || mtmp->muy != u.uy))
		pline("%s around a spot near you!", mappear);
	    else if (Displaced && (StrongDisplaced || !rn2(3)) && (mtmp->mux != u.ux || mtmp->muy != u.uy))
		pline("%s around your displaced image!", mappear);
	    else
		pline("%s from nowhere!", mappear);
	}
	dmg = 0;

	if (mtmp->data == &mons[PM_WOK]) {
		int attempts = 0;
		struct permonst *pm = 0;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

newbossRLM:
		do {
			pm = rndmonst();
			attempts++;
			if (attempts && (attempts % 10000 == 0)) u.mondiffhack++;
			if (!rn2(2000)) reset_rndmonst(NON_PM);

		} while ( (!pm || (pm && !(pm->msound == MS_SUPERMAN ))) && attempts < 50000);

		if (!pm && rn2(50) ) {
			attempts = 0;
			goto newbossRLM;
		}
		if (pm && !(pm->msound == MS_SUPERMAN) && rn2(50) ) {
			attempts = 0;
			goto newbossRLM;
		}

		if (pm) (void) makemon(pm, 0, 0, MM_ANGRY|MM_FRENZIED);

		u.mondiffhack = 0;
		u.aggravation = 0;

	}

	break;
    }


	case MGC_CALL_UNDEAD:
	{
		coord mm;
		mm.x = u.ux;
		mm.y = u.uy;
		pline("Undead creatures are called forth from the grave!");   
		mkundead(&mm, FALSE, 0, FALSE);
	}

	if (mtmp->data == &mons[PM_WOK]) {
		int attempts = 0;
		struct permonst *pm = 0;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

newbossRLN:
		do {
			pm = rndmonst();
			attempts++;
			if (attempts && (attempts % 10000 == 0)) u.mondiffhack++;
			if (!rn2(2000)) reset_rndmonst(NON_PM);

		} while ( (!pm || (pm && !(pm->msound == MS_SUPERMAN ))) && attempts < 50000);

		if (!pm && rn2(50) ) {
			attempts = 0;
			goto newbossRLN;
		}
		if (pm && !(pm->msound == MS_SUPERMAN) && rn2(50) ) {
			attempts = 0;
			goto newbossRLN;
		}

		if (pm) (void) makemon(pm, 0, 0, MM_ANGRY|MM_FRENZIED);

		u.aggravation = 0;
		u.mondiffhack = 0;

	}

	dmg = 0;   
	break;
    case MGC_AGGRAVATION:
	You_feel("that monsters are aware of your presence.");
	if (PlayerHearsSoundEffects) pline(issoviet ? "Dazhe sovetskaya Pyat' Lo obostryayetsya v vashem nizkom igrovom masterstve." : "Woaaaaaah!");
	aggravate();
	dmg = 0;
	break;
    case MGC_CURSE_ITEMS:
	if (MysteryResist && !rn2(3)) {
		dmg = 0;
		You("resist the itemcursing spell!");
		break;
	}
	if (chitinprotection() && !rn2(3)) {
		dmg = 0;
		You("resist the itemcursing spell!");
		break;
	}
	if (StrongMysteryResist && !rn2(3)) {
		dmg = 0;
		You("resist the itemcursing spell!");
		break;
	}
	You_feel("as if you need some help.");
	if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
	rndcurse();
	dmg = 0;
	break;
    case MGC_DESTRY_ARMR:
	if (chitinprotection() && rn2(3)) {
	    shieldeff(u.ux, u.uy);
	    pline("A field of force surrounds you!");
	} else if (MysteryResist && rn2(StrongMysteryResist ? 9 : 3)) {
	    shieldeff(u.ux, u.uy);
	    pline("A field of force surrounds you!");
	} else if (Antimagic && rn2(StrongAntimagic ? 20 : 5)) {
	    shieldeff(u.ux, u.uy);
	    pline("A field of force surrounds you!");
	} else {

		struct obj *otmp2;

		otmp2 = some_armor(&youmonst);

		if (otmp2 && otmp2->blessed && rn2(5)) pline("Your body shakes violently!");
		else if (otmp2 && (otmp2->spe > 1) && (rn2(otmp2->spe)) ) pline("Your body shakes violently!");
		else if (otmp2 && otmp2->oartifact && rn2(20)) pline("Your body shakes violently!");
		else if (otmp2 && otmp2->greased) {
			pline("Your body shakes violently!");
			 if (!rn2(2) || (isfriday && !rn2(2))) {
				pline_The("grease wears off.");
				otmp2->greased -= 1;
				update_inventory();
			 }
		}

		else if (!otmp2) pline("Your skin itches.");
	      else if(!destroy_arm(otmp2)) pline("Your skin itches.");
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

						int untamingchance = 10;

						if (!(PlayerCannotUseSkills)) {
							switch (P_SKILL(P_PETKEEPING)) {
								default: untamingchance = 10; break;
								case P_BASIC: untamingchance = 9; break;
								case P_SKILLED: untamingchance = 8; break;
								case P_EXPERT: untamingchance = 7; break;
								case P_MASTER: untamingchance = 6; break;
								case P_GRAND_MASTER: untamingchance = 5; break;
								case P_SUPREME_MASTER: untamingchance = 4; break;
							}
						}

						if (untamingchance > rnd(10) && !(Role_if(PM_DRAGONMASTER) && uarms && Is_dragon_shield(uarms) && mtmp2->data->mlet == S_DRAGON) && !((rnd(30 - ACURR(A_CHA))) < 4) ) {

							mtmp2->mtame = mtmp2->mpeaceful = 0;
							if (mtmp2->mleashed) { m_unleash(mtmp2,FALSE); }
							count++;

						}
					}
				} else if (mtmp2->mpeaceful && !is_infrastructure_monster(mtmp2)) {
					mtmp2->mpeaceful = 0;
					count++;
				} else {
					if (!is_infrastructure_monster(mtmp2)) {
						/*mtmp2->mberserk = 1;*/ /* removed because berserk doesn't exist in this fork */
						if (!rn2(5)) mtmp2->mfrenzied = 1; /* but we have something else instead now :D */
						mtmp2->mhp = mtmp2->mhpmax; /* let's heal them instead --Amy */
						count++;
					}
				}
			}
		}
		/* Don't yell if we didn't see anyone to yell at. */
		if (seen && (!rn2(3) || mtmp->iswiz)) {
			verbalize("Get %s, you fools, or I'll have your figgin on a stick!",uhim());
		}
		if (count) {
			pline("It seems a little more dangerous here now...");
			if (!(InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone())) doredraw();
		}
		dmg = 0;
		break;

    case MGC_DIVINE_WRATH: /* new idea by Amy. Yes, this is very evil. :D */

		if (chitinprotection() && !rn2(2)) {
			dmg = 0;
			You("resist the divine wrath spell!");
			break;
		}
		if (MysteryResist && !rn2(2)) {
			dmg = 0;
			You("resist the divine wrath spell!");
			break;
		}
		if (StrongMysteryResist && !rn2(2)) {
			dmg = 0;
			You("resist the divine wrath spell!");
			break;
		}

		u.ugangr++;
		if (!rn2(5)) u.ugangr++;
		if (!rn2(25)) u.ugangr++;
		prayer_done();

		dmg = 0;
		break;

    case MGC_WITHER:
	if (chitinprotection() && rn2(3)) {
	    shieldeff(u.ux, u.uy);
	    pline("A field of force surrounds you!");
	} else if (MysteryResist && rn2(StrongMysteryResist ? 5 : 3)) {
	    shieldeff(u.ux, u.uy);
	    pline("A field of force surrounds you!");
	} else if (Antimagic && rn2(StrongAntimagic ? 5 : 3)) {
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
		else if (uarmu)
		    (void)wither_dmg(uarmu, xname(uarmu), rn2(4), TRUE, &youmonst);
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
	if (chitinprotection() && rn2(3)) {
	    shieldeff(u.ux, u.uy);
	    pline("A field of force surrounds you!");
	} else if (MysteryResist && rn2(StrongMysteryResist ? 5 : 3)) {
	    shieldeff(u.ux, u.uy);
	    pline("A field of force surrounds you!");
	} else if (Antimagic && rn2(StrongAntimagic ? 5 : 3)) {
	    shieldeff(u.ux, u.uy);
	    pline("A field of force surrounds you!");
	} else {
	    pline("Your body shakes!");

	while (1) {
	    switch(rn2(5)) {
	    case 0:

		if (evilfriday && uarmh && uarmh->oerodeproof) {
			uarmh->oerodeproof = 0;
			pline("Harharhar, your helmet is no longer erosionproof!");
		}

		if (!uarmh || !rust_dmg(uarmh, xname(uarmh), rn2(4), FALSE, &youmonst))
			continue;
		break;
	    case 1:
		if (uarmc) {

			if (evilfriday && uarmc && uarmc->oerodeproof) {
				uarmc->oerodeproof = 0;
				pline("Harharhar, your cloak is no longer erosionproof!");
			}

		    (void)rust_dmg(uarmc, xname(uarmc), rn2(4), TRUE, &youmonst);
		    break;
		}
		/* Note the difference between break and continue;
		 * break means it was hit and didn't rust; continue
		 * means it wasn't a target and though it didn't rust
		 * something else did.
		 */
		if (uarm) {

			if (evilfriday && uarm && uarm->oerodeproof) {
				uarm->oerodeproof = 0;
				pline("Harharhar, your armor is no longer erosionproof!");
			}

		    (void)rust_dmg(uarm, xname(uarm), rn2(4), TRUE, &youmonst);
		} else if (uarmu) {

			if (evilfriday && uarmu && uarmu->oerodeproof) {
				uarmu->oerodeproof = 0;
				pline("Harharhar, your shirt is no longer erosionproof!");
			}

		    (void)rust_dmg(uarmu, xname(uarmu), rn2(4), TRUE, &youmonst);
		}
		break;
	    case 2:

		if (evilfriday && uarms && uarms->oerodeproof) {
			uarms->oerodeproof = 0;
			pline("Harharhar, your shield is no longer erosionproof!");
		}

		if (!uarms || !rust_dmg(uarms, xname(uarms), rn2(4), FALSE, &youmonst))
		    continue;
		break;
	    case 3:

		if (evilfriday && uarmg && uarmg->oerodeproof) {
			uarmg->oerodeproof = 0;
			pline("Harharhar, your gloves are no longer erosionproof!");
		}

		if (!uarmg || !rust_dmg(uarmg, xname(uarmg), rn2(4), FALSE, &youmonst))
		    continue;
		break;
	    case 4:

		if (evilfriday && uarmf && uarmf->oerodeproof) {
			uarmf->oerodeproof = 0;
			pline("Harharhar, your boots are no longer erosionproof!");
		}

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
	if (chitinprotection() && rn2(3)) {
	    shieldeff(u.ux, u.uy);
	    You_feel("momentarily weakened.");
	} else if (MysteryResist && rn2(StrongMysteryResist ? 9 : 3)) {
	    shieldeff(u.ux, u.uy);
	    You_feel("momentarily weakened.");
	} else if (Antimagic && rn2(StrongAntimagic ? 20 : 5)) {
	    shieldeff(u.ux, u.uy);
	    You_feel("momentarily weakened.");
	} else {
	    if (issoviet) pline("Vy chuvstvuyete sebya slabeye! Iosif Putin reshil, chto lyudi boryutsya protiv rezhima, dolzhny byt' nakazany i sovetskiy Pyat' Lo soglasilsya s nim!");
	    else You("suddenly feel weaker!");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Pochemu vy ne sledite luchshe dlya vashikh atributov, tak ili inache?" : "Due-l-ue-l-ue-l!");
	    dmg = rnd(mtmp->m_lev - 5);	/* nerf by Amy - why did this always do the maximum amount??? */
	    if (issoviet) dmg = mtmp->m_lev - rnd(5);
	    if (Half_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
	    if (StrongHalf_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
	    losestr(rnd(dmg), TRUE);
	    if (u.uhp < 1)
		done_in_by(mtmp);
	}
	dmg = 0;
	break;
    case MGC_DISAPPEAR:		/* makes self invisible */
	if (!mtmp->minvis && !mtmp->invis_blkd) {
	    if (canseemon(mtmp))
		pline("%s suddenly %s!", Monnam(mtmp), !See_invisible ? "disappears" : "becomes transparent");
	    mon_set_minvis(mtmp);
	    dmg = 0;
	} else
	    impossible("no reason for monster to cast disappear spell?");
	break;
    case MGC_NUMB_YOU:
	if (Antimagic && rn2(StrongAntimagic ? 3 : 2)) {
	    shieldeff(u.ux, u.uy);
	    if (!Numbed)
		You_feel("numb for a moment.");
	    make_numbed(1L, FALSE);
	} else {
	    You(Numbed ? "feel even more numb!" : "feel numb!");
	    if (Half_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
	    if (StrongHalf_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
	    make_numbed(HNumbed + dmg, FALSE);
	}
	dmg = 0;
	break;
    case MGC_BURN_YOU:
	if (Antimagic && rn2(StrongAntimagic ? 3 : 2)) {
	    shieldeff(u.ux, u.uy);
	    if (!Burned)
		You_feel("ablaze for a moment.");
	    make_burned(1L, FALSE);
	} else {
	    You(Burned ? "are burning more strongly!" : "are burning!");
	    if (Half_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
	    if (StrongHalf_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
	    make_burned(HBurned + dmg, FALSE);
	}
	dmg = 0;
	break;
    case MGC_STUN_YOU:
	if ((Antimagic || Free_action) && rn2((StrongAntimagic || StrongFree_action) ? 20 : 5)) {
	    shieldeff(u.ux, u.uy);
	    if (!Stunned)
		You_feel("momentarily disoriented.");
	    make_stunned(1L, FALSE);
	} else {
	    You(Stunned ? "struggle to keep your balance." : "reel...");
	    dmg = d(ACURR(A_DEX) < 12 ? 6 : 4, 4);
	    if (Half_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
	    if (StrongHalf_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
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
	    int healamount;
	    if (canseemon(mtmp))
		pline("%s looks better.", Monnam(mtmp));
	    /* note: player healing does 6d4; this used to do 1d8 */
		/* Amy note: boosted it so that it's no longer completely useless */
	    healamount = d(3,6) + rnz(1 + mtmp->m_lev);
	    if ((mtmp->mhp += healamount) > mtmp->mhpmax)
		mtmp->mhp = mtmp->mhpmax;

	    if (mtmp->bleedout && mtmp->bleedout <= healamount) {
			mtmp->bleedout = 0;
			pline("%s's bleeding stops.", Monnam(mtmp));
	    } else if (mtmp->bleedout) {
			mtmp->bleedout -= healamount;
			if (mtmp->bleedout < 0) mtmp->bleedout = 0; /* should never happen */
			pline("%s's bleeding diminishes.", Monnam(mtmp));
	    }

	    dmg = 0;
	}
	break;
    case MGC_PSI_BOLT:

	if (!rn2(500 - (mtmp->m_lev * 2) )) { /* wishing */

		switch (rnd(6)) {
			case 1:
				(void) mongets(mtmp, rnd_defensive_item(mtmp));
				break;
			case 2:
				(void) mongets(mtmp, rnd_offensive_item(mtmp));
				break;
			case 3:
				(void) mongets(mtmp, rnd_misc_item(mtmp));
				break;
			case 4:
				(void) mongets(mtmp, rnd_defensive_item_new(mtmp));
				break;
			case 5:
				(void) mongets(mtmp, rnd_offensive_item_new(mtmp));
				break;
			case 6:
				(void) mongets(mtmp, rnd_misc_item_new(mtmp));
				break;
		}
		pline("%s wishes for an object.", Monnam(mtmp) );

	break;
	}

	if (!rn2(400 - (mtmp->m_lev * 2) )) { /* summon boss */

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

	    {	coord cc;
		struct permonst *pm = 0;
		struct monst *mon;
		int attempts = 0;

		if (!enexto(&cc, mtmp->mx, mtmp->my, 0)) break;

newboss:
		do {
			pm = rndmonst();
			attempts++;
			if (attempts && (attempts % 10000 == 0)) u.mondiffhack++;
			if (!rn2(2000)) reset_rndmonst(NON_PM);

		} while ( (!pm || (pm && !(pm->geno & G_UNIQ))) && attempts < 50000);

		if (!pm && rn2(50) ) {
			attempts = 0;
			goto newboss;
		}
		if (pm && !(pm->geno & G_UNIQ) && rn2(50) ) {
			attempts = 0;
			goto newboss;
		}

		if (pm) mon = makemon(pm, cc.x, cc.y, NO_MM_FLAGS);

	    }
	    pline("A boss monster appears from nowhere!");

		u.mondiffhack = 0;
		u.aggravation = 0;

	break;
	}

	if (!rn2(300 - (mtmp->m_lev * 2) )) { /* bad effect */

		badeffect();

	break;
	}

	/* prior to 3.4.0 Antimagic was setting the damage to 1--this
	   made the spell virtually harmless to players with magic res. */
	if (Antimagic && !Race_if(PM_KUTAR) && rn2(StrongAntimagic ? 20 : 5)) {
	    shieldeff(u.ux, u.uy);
	    dmg = (dmg + 1) / 2;
	}

	/* Amy change: half damage, but also increase sanity */
	if (dmg > 1) dmg /= 2;

	if (dmg <= 5)
	    You("get a slight %sache.", body_part(HEAD));
	else if (dmg <= 10)
	    Your("brain is on fire!");
	else if (dmg <= 20)
	    Your("%s suddenly aches painfully!", body_part(HEAD));
	else
	    Your("%s suddenly aches very painfully!", body_part(HEAD));

	if (!Psi_resist || !rn2(StrongPsi_resist ? 20 : 5) ) increasesanity(dmg);

	break;
    case MGC_ESCALATION:

	if (Antimagic && !Race_if(PM_KUTAR) && rn2(StrongAntimagic ? 20 : 5)) {
	    shieldeff(u.ux, u.uy);
	    dmg = (dmg + 1) / 2;
	}
	dmg += u.chokhmahdamage;

	u.chokhmahdamage++;

	if (dmg <= 5)
	    You("get a %sache.", body_part(HEAD));
	else if (dmg <= 10)
	    Your("brain is on fire, and the pain seems to be getting stronger!");
	else if (dmg <= 20)
	    Your("%s suddenly aches really painfully!", body_part(HEAD));
	else
	    Your("%s suddenly aches extremely painfully!", body_part(HEAD));
	break;

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
	int oldhp, oldmp, oldhpmax, oldmpmax;

    if (dmg == 0 && !is_undirected_spell(AD_CLRC, spellnum)) {
	impossible("cast directed cleric spell (%d) with dmg=0?", spellnum);
	return;
    }

    switch (spellnum) {
    case CLC_GEYSER:

	switch (rnd(40) ) {
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
	if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vse promokli. Vy zhe pomnite, chtoby polozhit' vodu chuvstvitel'nyy material v konteyner, ne tak li?" : "Schwatschhhhhh!");
	if (uwep && uwep->otyp == UMBRELLA && rn2(2)) {
		pline("But your umbrella protects you from it.");
		break;
	}
	dmg = d(8, 6);
	if ((!StrongSwimming || !rn2(10)) && (!StrongMagical_breathing || !rn2(10))) {
		water_damage(invent, FALSE, FALSE); /* Come on, every other source of water rusts your stuff. --Amy */
		if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);
	}
	if (Burned) make_burned(0L, TRUE); /* you're enveloped in water, so the burn disappears */
	if (Half_physical_damage && (rn2(2) || (uwep && uwep->oartifact == ART_SOOTHE_)) ) dmg = (dmg + 1) / 2;
	if (StrongHalf_physical_damage && (rn2(2) || (uwep && uwep->oartifact == ART_SOOTHE_)) ) dmg = (dmg + 1) / 2;
	break;

	case 20:
	/* antimatter storm --Amy */

	if (chitinprotection() && rn2(2)) {
		dmg = 0;
		You("resist the antimatter spell!");
		break;
	}
	if (MysteryResist && rn2(2)) {
		dmg = 0;
		You("resist the antimatter spell!");
		break;
	}
	if (StrongMysteryResist && rn2(2)) {
		dmg = 0;
		You("resist the antimatter spell!");
		break;
	}

	pline("You are caught in an antimatter storm!");
	dmg = d(8, 6);
	withering_damage(invent, FALSE, FALSE); /* This can potentially damage all of your inventory items. --Amy */
	if (Half_physical_damage && (rn2(2) || (uwep && uwep->oartifact == ART_SOOTHE_)) ) dmg = (dmg + 1) / 2;
	if (StrongHalf_physical_damage && (rn2(2) || (uwep && uwep->oartifact == ART_SOOTHE_)) ) dmg = (dmg + 1) / 2;

	break;

	case 21:
	case 22:
	case 23:
	case 24:
	case 25:

		if (chitinprotection() && rn2(2)) {
			dmg = 0;
			You("resist the petrify spell!");
			break;
		}
		if (MysteryResist && rn2(2)) {
			dmg = 0;
			You("resist the petrify spell!");
			break;
		}
		if (StrongMysteryResist && rn2(2)) {
			dmg = 0;
			You("resist the petrify spell!");
			break;
		}

		/* petrify - similar to cockatrice hissing --Amy */
		You_feel("a massive burden on your chest!");
		if (!Stoned && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && !(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM)) ) {
			if (Hallucination && rn2(10)) pline("Good thing you are already stoned.");
			else {
				You("start turning to stone!");
				Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
				u.cnd_stoningcount++;
				stop_occupation();
				delayed_killer = "petrify spell";
			}
		}
		dmg = 0;
		break;

	case 26:
	case 27:
	case 28:
	case 29:
	case 30:

		if (chitinprotection() && rn2(2)) {
			dmg = 0;
			You("resist the sliming spell!");
			break;
		}
		if (MysteryResist && rn2(2)) {
			dmg = 0;
			You("resist the sliming spell!");
			break;
		}
		if (StrongMysteryResist && rn2(2)) {
			dmg = 0;
			You("resist the sliming spell!");
			break;
		}

		/* sliming - similar to green slime attack --Amy */
		    if (!Slimed && !flaming(youmonst.data) && !Unchanging && !slime_on_touch(youmonst.data) ) {
			You("don't feel very well.");
			stop_occupation();
			make_slimed(100);
		    killer_format = KILLED_BY_AN;
		    delayed_killer = "slimed by a monster spell";
		    }
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

	case 38:

		if (chitinprotection() && rn2(2)) {
			dmg = 0;
			You("resist the amnesia spell!");
			break;
		}
		if (MysteryResist && rn2(2)) {
			dmg = 0;
			You("resist the amnesia spell!");
			break;
		}
		if (StrongMysteryResist && rn2(2)) {
			dmg = 0;
			You("resist the amnesia spell!");
			break;
		}

		/* amnesia - evil patch idea by jonadab */
		forget(3, FALSE);
		{
		if (!strncmpi(plname, "Maud", 4) || !strncmpi(plalias, "Maud", 4))
			pline("As your mind turns inward on itself, you forget everything else.");
		else if (rn2(2))
			pline("Who was that Maud person anyway?");
		else
			pline("Thinking of Maud you forget everything else.");
		}

		 break;

	case 39:

		/* wraparound - evil patch idea */
		oldhp = u.uhp;
		oldmp = u.uen;
		oldhpmax = u.uhpmax;
		oldmpmax = u.uenmax;
	
		u.uen = oldhp;
		u.uhp = oldmp;
		u.uenmax = oldhpmax;
		u.uhpmax = oldmpmax;
		You_feel("a wraparound!");
		break;

	case 40:

		/* translucency - makes the player's items visible */
		pline("You are surrounded by a translucent glow!");
		{
			register struct obj *objX, *objX2;
			for (objX = invent; objX; objX = objX2) {
				objX2 = objX->nobj;
				if (!rn2(5)) objX->oinvis = objX->oinvisreal = FALSE;
			}
		}

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
	if (PlayerHearsSoundEffects) pline(issoviet ? "Gori detka gori!" : "Tschack-tschack-tschack-tschack-tschack");
	if ((Fire_resistance && rn2(StrongFire_resistance ? 20 : 5)) || FireImmunity) {
	    shieldeff(u.ux, u.uy);
	    dmg = 0;
	} else {
	    dmg = d(8, 6);
	    if (Race_if(PM_LOWER_ENT)) dmg *= 2;
	}
	if (Half_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
	if (StrongHalf_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
	burn_away_slime();
	if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 33)) (void) burnarmor(&youmonst);
	if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 3 : 15)) /* new calculations --Amy */	destroy_item(SCROLL_CLASS, AD_FIRE);
	if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 3 : 15)) /* new calculations --Amy */	destroy_item(POTION_CLASS, AD_FIRE);
	if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 3 : 15)) /* new calculations --Amy */	destroy_item(SPBOOK_CLASS, AD_FIRE);
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
	    make_blinded(StrongHalf_spell_damage ? 5L : Half_spell_damage ? 10L : 20L, FALSE);
	}
	if (reflects || ShockImmunity || (Shock_resistance && (StrongShock_resistance || rn2(10))) ) {
	    shieldeff(u.ux, u.uy);
	    dmg = 0;
	    if (reflects)
		break;
	} else
	    dmg = d(8, 6);
	if (Half_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
	if (StrongHalf_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
		if (isevilvariant || !rn2(issoviet ? 3 : 15)) /* new calculations --Amy */	destroy_item(WAND_CLASS, AD_ELEC);
		if (isevilvariant || !rn2(issoviet ? 3 : 15)) /* new calculations --Amy */	destroy_item(RING_CLASS, AD_ELEC);
		if (isevilvariant || !rn2(issoviet ? 15 : 75)) /* new calculations --Amy */	destroy_item(AMULET_CLASS, AD_ELEC);
	break;
    }
    case CLC_CURSE_ITEMS:

	if (chitinprotection() && !rn2(3)) {
		dmg = 0;
		You("resist the itemcursing spell!");
		break;
	}
	if (MysteryResist && !rn2(3)) {
		dmg = 0;
		You("resist the itemcursing spell!");
		break;
	}
	if (StrongMysteryResist && !rn2(3)) {
		dmg = 0;
		You("resist the itemcursing spell!");
		break;
	}

	You_feel("as if you need some help.");
	if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
	rndcurse();
	dmg = 0;
	break;

    case CLC_AGGRAVATION: /* aggravate monster */
	You_feel("that monsters are aware of your presence."); /* aggravate monster */
	aggravate();
	if (PlayerHearsSoundEffects) pline(issoviet ? "Dazhe sovetskaya Pyat' Lo obostryayetsya v vashem nizkom igrovom masterstve." : "Woaaaaaah!");
	dmg = 0;
	break;

    case CLC_RANDOM: /* inofficial names see below */
	switch (rnd(40) ) {

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
		/*15*/rangeX, set_litZ, (void *)((char *)0));
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
			boolean canbeinawall = FALSE;
			if (!rn2(Passes_walls ? 5 : 25)) canbeinawall = TRUE;

		      for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
				if (!isok(u.ux + i, u.uy + j)) continue;
				if ((levl[u.ux + i][u.uy + j].typ <= DBWALL) && !canbeinawall) continue;
				if (t_at(u.ux + i, u.uy + j)) continue;

			      rtrap = rndtrap();

				(void) maketrap(u.ux + i, u.uy + j, rtrap, 100, TRUE);
			}
		dmg = 0;
		break;
	case 26:
	      You_feel("yanked in a new direction!"); /* relocation */
		(void) safe_teleds_normalterrain(FALSE);
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
	      pline(FunnyHallu ? "You suddenly feel like you're on cold turkey!" : "Your hands start trembling!"); /* disarm */
		int glibberX;
		glibberX = (rnz(1 + mtmp->m_lev));
		    incr_itimeout(&Glib, glibberX);
		    flags.botl = TRUE;
		dmg = 0;
		break;
	case 34:
	case 35:
	case 36:
	      You_feel("pulsating winds!"); /* slow */
		    u_slow_down();
		dmg = 0;
		break;
	case 37:
	      pline(FunnyHallu ? "You feel like you just got dumped by your girlfriend!" : "You feel out of luck!"); /* dementor force */
			change_luck(-1);
			if (!rn2(10)) change_luck(-5);
			adjalign(-10);
			if (!rn2(10)) adjalign(-50);
		dmg = 0;
		break;
	case 38:
	      You_feel("burdened"); /* punishment - message is from Castle of the Winds */
			punishx();
		dmg = 0;
		break;
	case 39:
	case 40:
		pushplayer(FALSE); /* use the force */
		break;
	default: /*failsafe*/
		You_feel("that monsters are aware of your presence."); /* aggravate monster */
		aggravate();
		if (PlayerHearsSoundEffects) pline(issoviet ? "Dazhe sovetskaya Pyat' Lo obostryayetsya v vashem nizkom igrovom masterstve." : "Woaaaaaah!");
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

	if (!rn2(3)) { switch (rnd(54)) {

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

	if (Aggravate_monster) {
		u.aggravation = 1;
		reset_rndmonst(NON_PM);
	}

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

	u.aggravation = 0;

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
	else if (let == S_PIERCER) /* piercing ceiling */ {
	    if (flags.soundok) You_hear("a shrill ringing sound.");
	} else if (let == S_RODENT) /* rat swarm */
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
	else if (Invisible && !perceives(mtmp->data) && (StrongInvis || !rn2(3)) &&
				(mtmp->mux != u.ux || mtmp->muy != u.uy))
	    pline("%s summons insects around a spot near you!",
		Monnam(mtmp));
	else if (Displaced && (StrongDisplaced || !rn2(3)) && (mtmp->mux != u.ux || mtmp->muy != u.uy))
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
	    make_blinded(StrongHalf_spell_damage ? 50L : Half_spell_damage ? 100L : 200L, FALSE);
	    if (!Blind) Your("%s", vision_clears);
	    dmg = 0;
	} else
	    impossible("no reason for monster to cast blindness spell?");
	break;
    case CLC_PARALYZE:
	if ((Antimagic || Free_action) && rn2((StrongAntimagic || StrongFree_action) ? 20 : 5)) {
	    shieldeff(u.ux, u.uy);
	    if (multi >= 0)
		You("stiffen briefly.");
	    nomul(-1, "paralyzed by a monster spell", TRUE);
	} else {
	    if (multi >= 0)
		You("are frozen in place!");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
	    /* new calculations by Amy because otherwise this spell would be uber imba */
	    if (!issoviet) dmg = rnd( 2 + (rn2(3) ? ((int)mtmp->m_lev / 2) : rn2(2) ? ((int)mtmp->m_lev / 4) : (int)mtmp->m_lev) ) ;
	    else dmg = 4 + (int)mtmp->m_lev;;
	    if (Half_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
	    if (StrongHalf_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
	    if (chitinprotection() && rn2(2) ) dmg = (dmg + 1) / 2;
	    if (MysteryResist && rn2(2) ) dmg = (dmg + 1) / 2;
	    if (StrongMysteryResist && rn2(2) ) dmg = (dmg + 1) / 2;
	    if (issoviet) pline("Teper' vy mertvy. Sovetskaya smeyetsya, potomu chto vy, veroyatno, vlozhili dvesti chasov v etot kharakter.");
	    nomul(-dmg, "paralyzed by a monster spell", TRUE);
	}
	nomovemsg = 0;
	dmg = 0;
	break;
    case CLC_CONFUSE_YOU:
	if (Antimagic && rn2(StrongAntimagic ? 20 : 5)) {
	    shieldeff(u.ux, u.uy);
	    You_feel("momentarily dizzy.");
	} else {
	    boolean oldprop = !!Confusion;

	    dmg = (int)mtmp->m_lev;
	    if (Half_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
	    if (StrongHalf_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
	    make_confused(HConfusion + dmg, TRUE);
	    if (FunnyHallu)
		You_feel("%s!", oldprop ? "trippier" : "trippy");
	    else
		You_feel("%sconfused!", oldprop ? "more " : "");
	}
	dmg = 0;
	break;
    case CLC_FEAR_YOU:
	if (Antimagic && rn2(StrongAntimagic ? 3 : 2)) {
	    shieldeff(u.ux, u.uy);
	    if (!Feared)
		You_feel("afraid for a moment.");
	    make_feared(1L, FALSE);
	} else {
	    You(Feared ? "are even more scared!" : "are scared!");
	    if (Half_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
	    if (StrongHalf_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
	    make_feared(HFeared + dmg, FALSE);
	}
	dmg = 0;
	break;
    case CLC_FREEZE_YOU:
	if (Antimagic && rn2(StrongAntimagic ? 3 : 2)) {
	    shieldeff(u.ux, u.uy);
	    if (!Frozen)
		You_feel("frozen for a moment.");
	    make_frozen(1L, FALSE);
	} else {
	    You(Frozen ? "are freezing even more!" : "are freezing!");
	    if (Half_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
	    if (StrongHalf_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
	    make_frozen(HFrozen + dmg, FALSE);
	}
	dmg = 0;
	break;
    case CLC_SEPARATION:
	u.ublesscnt += (dmg * 10);
	pline(FunnyHallu ? "You feel sinful... but do you really care?" : "You have a feeling of separation.");
	dmg = 0;
	break;
    case CLC_STAT_DRAIN:		/* drain a random stat */
	if (chitinprotection() && rn2(3)) {
	    shieldeff(u.ux, u.uy);
	    pline("A field of force surrounds you!");
	} else if (MysteryResist && rn2(StrongMysteryResist ? 10 : 3)) {
	    shieldeff(u.ux, u.uy);
	    pline("A field of force surrounds you!");
	} else if (Antimagic && rn2(StrongAntimagic ? 10 : 3)) {
	    shieldeff(u.ux, u.uy);
	    You_feel("less powerful for a moment, but the feeling passes.");
	} else {
	    if (issoviet) pline("Pravitel'stvo prinimayet resheniye umen'shit' vashi ochki statusa, potomu chto vy stanovites' opasnost'.");
	    else You_feel("less powerful!");
	    dmg = rnd(mtmp->m_lev - 7);
	    if (issoviet) dmg = mtmp->m_lev - rnd(7);
	    if (Half_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
	    if (StrongHalf_spell_damage && rn2(2) ) dmg = (dmg + 1) / 2;
	    adjattrib(rn2(A_MAX), -dmg, 0, TRUE);
	}
	dmg = 0;
	break;

    case CLC_CURE_SELF:
	if (mtmp->mhp < mtmp->mhpmax) {
	    int healamount;
	    if (canseemon(mtmp))
		pline("%s looks better.", Monnam(mtmp));
	    /* note: player healing does 6d4; this used to do 1d8 */
		/* Amy note: boosted it so that it's no longer completely useless */
	    healamount = d(3,6) + rnz(1 + mtmp->m_lev);
	    if ((mtmp->mhp += healamount) > mtmp->mhpmax)
		mtmp->mhp = mtmp->mhpmax;

	    if (mtmp->bleedout && mtmp->bleedout <= healamount) {
			mtmp->bleedout = 0;
			pline("%s's bleeding stops.", Monnam(mtmp));
	    } else if (mtmp->bleedout) {
			mtmp->bleedout -= healamount;
			if (mtmp->bleedout < 0) mtmp->bleedout = 0; /* should never happen */
			pline("%s's bleeding diminishes.", Monnam(mtmp));
	    }

	    dmg = 0;
	}

	break;
    case CLC_OPEN_WOUNDS:

	if (!rn2(500 - (mtmp->m_lev * 2) )) { /* wishing */

		switch (rnd(6)) {
			case 1:
				(void) mongets(mtmp, rnd_defensive_item(mtmp));
				break;
			case 2:
				(void) mongets(mtmp, rnd_offensive_item(mtmp));
				break;
			case 3:
				(void) mongets(mtmp, rnd_misc_item(mtmp));
				break;
			case 4:
				(void) mongets(mtmp, rnd_defensive_item_new(mtmp));
				break;
			case 5:
				(void) mongets(mtmp, rnd_offensive_item_new(mtmp));
				break;
			case 6:
				(void) mongets(mtmp, rnd_misc_item_new(mtmp));
				break;
		}
		pline("%s wishes for an object.", Monnam(mtmp) );

	break;
	}

	if (!rn2(400 - (mtmp->m_lev * 2) )) { /* summon boss */

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

	    {	coord cc;
		struct permonst *pm = 0;
		struct monst *mon;
		int attempts = 0;

		if (!enexto(&cc, mtmp->mx, mtmp->my, 0)) break;

newboss:
		do {
			pm = rndmonst();
			attempts++;
			if (attempts && (attempts % 10000 == 0)) u.mondiffhack++;
			if (!rn2(2000)) reset_rndmonst(NON_PM);

		} while ( (!pm || (pm && !(pm->geno & G_UNIQ))) && attempts < 50000);

		if (!pm && rn2(50) ) {
			attempts = 0;
			goto newboss;
		}
		if (pm && !(pm->geno & G_UNIQ) && rn2(50) ) {
			attempts = 0;
			goto newboss;
		}

		if (pm) mon = makemon(pm, cc.x, cc.y, NO_MM_FLAGS);

	    }
	    pline("A boss monster appears from nowhere!");

		u.mondiffhack = 0;
		u.aggravation = 0;

	break;
	}

	if (!rn2(300 - (mtmp->m_lev * 2) )) { /* bad effect */

		badeffect();

	break;
	}

	if (Antimagic && !Race_if(PM_KUTAR) && rn2(StrongAntimagic ? 20 : 5)) {
	    shieldeff(u.ux, u.uy);
	    dmg = (dmg + 1) / 2;
	}

	/* Amy change: half damage, but also increase bleeding */
	if (dmg > 1) dmg /= 2;

	if (dmg <= 5)
	    Your("skin itches badly for a moment.");
	else if (dmg <= 10)
	    pline("Wounds appear on your body!");
	else if (dmg <= 20)
	    pline("Severe wounds appear on your body!");
	else
	    Your("body is covered with painful wounds!");

	playerbleed(dmg);

	break;
    case CLC_VULN_YOU: /* inspired by Sporkhack but enhanced by Amy */
	dmg *= 10;
	deacrandomintrinsic(rnz(dmg));
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
	if (mtmp->mpeaceful && See_invisible && !StrongSee_invisible && !(mtmp->seeinvisble) && spellnum == MGC_DISAPPEAR)
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
	if (((levl[u.ux][u.uy].typ != ROOM && levl[u.ux][u.uy].typ != CORR) /* lowered chance even with amulet --Amy */
		|| ((!u.uhave.amulet || u.freeplaymode) && rn2(10)) || rn2(3) ) && spellnum == MGC_CREATE_POOL)
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
	int spellcasttype = mattk->adtyp;

	if (EnthuActive) {
		if (!rn2(100)) spellcasttype = AD_DISN;
		else {
			switch (rnd(9)) {
				case 1:
					spellcasttype = AD_MAGM; break;
				case 2:
					spellcasttype = AD_FIRE; break;
				case 3:
					spellcasttype = AD_COLD; break;
				case 4:
					spellcasttype = AD_ELEC; break;
				case 5:
					spellcasttype = AD_SLEE; break;
				case 6:
					spellcasttype = AD_DRST; break;
				case 7:
					spellcasttype = AD_ACID; break;
				case 8:
					spellcasttype = AD_LITE; break;
				case 9:
					spellcasttype = AD_SPC2; break;
			}
		}
	}

	/* don't print constant stream of curse messages for 'normal'
	   spellcasting monsters at range */
	if ((spellcasttype > AD_SPC2) || (spellcasttype < AD_MAGM))
	    return(0);

	if (mtmp->mcan || arcaniumfail() || (RngeAntimagicA && !rn2(10)) || (RngeAntimagicB && !rn2(5)) || (RngeAntimagicC && !rn2(2)) || (RngeAntimagicD) || (RngeSpellDisruption && !rn2(5)) || u.antimagicshell || (uarmh && uarmh->otyp == HELM_OF_ANTI_MAGIC) || (uwep && uwep->otyp == DARK_SWORD) || (u.twoweap && uswapwep && uswapwep->otyp == DARK_SWORD) || (uarmc && uarmc->oartifact == ART_SHELLY && (moves % 3 == 0)) || (uarmc && uarmc->oartifact == ART_BLACK_VEIL_OF_BLACKNESS) || (uarmc && uarmc->oartifact == ART_ARABELLA_S_WAND_BOOSTER) || (uarmu && uarmu->oartifact == ART_ANTIMAGIC_SHELL) || (uarmu && uarmu->oartifact == ART_ANTIMAGIC_FIELD) || (uwep && uwep->oartifact == ART_WINCHESTER_PREMIUM) || Role_if(PM_UNBELIEVER) || (uwep && uwep->oartifact == ART_ARK_OF_THE_COVENANT) || (uwep && uwep->oartifact == ART_LONG_SWORD_OF_ETERNITY) || (uwep && uwep->oartifact == ART_HEAVY_CROSSBOW_OF_ETERNITY) || (uarmc && uarmc->oartifact == ART_SPELL_WARDED_WRAPPINGS_OF_) || (uwep && uwep->oartifact == ART_ANTIMAGICBANE) || (uarmc && (itemhasappearance(uarmc, APP_VOID_CLOAK) || itemhasappearance(uarmc, APP_SHELL_CLOAK)) && !rn2(5)) ) {
	    cursetxt(mtmp, FALSE);
	    return(0);
	}
	if(lined_up(mtmp) && rn2(3)) {
	    nomul(0, 0, FALSE);
	    if(spellcasttype && (spellcasttype < 11)) { /* no cf unsigned >0 */
		if(canseemon(mtmp))
		    pline("%s zaps you with a %s!", Monnam(mtmp),
			  flash_types[ad_to_typ(spellcasttype)]);
		else if (flags.soundok && !issoviet) You_hear("a buzzing sound.");
		buzz(-ad_to_typ(spellcasttype), (rn2(2) ? (int)mattk->damn : (int)mattk->damd ),
		     mtmp->mx, mtmp->my, sgn(tbx), sgn(tby));
	    } else impossible("Monster spell %d cast", spellcasttype-1);
	}
	return(1);
}

#endif /* OVL0 */

/*mcastu.c*/

STATIC_PTR void
set_litZ(x,y,val)
int x, y;
void * val;
{
	if (val)
	    levl[x][y].lit = 1;
	else {
	    levl[x][y].lit = 0;
	    snuff_light_source(x, y);
	}
}

