/*	SCCS Id: @(#)weapon.c	3.4	2002/11/07	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/*
 *	This module contains code for calculation of "to hit" and damage
 *	bonuses for any given weapon used, as well as weapons selection
 *	code for monsters.
 */
#include "hack.h"

/* categories whose names don't come from OBJ_NAME(objects[type]) */
#define PN_POLEARMS		(-1)
#define PN_SABER		(-2)
#define PN_HAMMER		(-3)
#define PN_WHIP			(-4)
#define PN_PADDLE		(-5)
#define PN_FIREARMS		(-6)
#define PN_ATTACK_SPELL		(-7)
#define PN_HEALING_SPELL	(-8)
#define PN_DIVINATION_SPELL	(-9)
#define PN_ENCHANTMENT_SPELL	(-10)
#define PN_PROTECTION_SPELL	(-11)
#define PN_BODY_SPELL		(-12)
#define PN_MATTER_SPELL		(-13)
#define PN_BARE_HANDED		(-14)
#define PN_MARTIAL_ARTS		(-15)
#define PN_RIDING		(-16)
#define PN_TWO_WEAPONS		(-17)
#ifdef LIGHTSABERS
#define PN_LIGHTSABER		(-18)
#endif

static void FDECL(give_may_advance_msg, (int));
STATIC_PTR int NDECL(practice);
static int FDECL(get_obj_skill, (struct obj *));

#ifdef LIGHTSABERS
static void FDECL(mon_ignite_lightsaber, (struct obj *, struct monst *));
#endif

/*WAC practicing needs a delay counter*/
static NEARDATA schar delay;            /* moves left for practice */
static NEARDATA boolean speed_advance = FALSE;

STATIC_DCL void FDECL(give_may_advance_msg, (int));

#ifndef OVLB

STATIC_DCL NEARDATA const short skill_names_indices[];
STATIC_DCL NEARDATA const char *odd_skill_names[];

#else	/* OVLB */

/* KMH, balance patch -- updated */
STATIC_OVL NEARDATA const short skill_names_indices[P_NUM_SKILLS] = {
	0,                DAGGER,         KNIFE,        AXE,
	PICK_AXE,         SHORT_SWORD,    BROADSWORD,   LONG_SWORD,
	TWO_HANDED_SWORD, SCIMITAR,       PN_SABER,     CLUB,
	PN_PADDLE,        MACE,           MORNING_STAR,   FLAIL,
	PN_HAMMER,        QUARTERSTAFF,   PN_POLEARMS,  SPEAR,
	JAVELIN,          TRIDENT,        LANCE,        BOW,
	SLING,            PN_FIREARMS,    CROSSBOW,       DART,
	SHURIKEN,         BOOMERANG,      PN_WHIP,      UNICORN_HORN,
#ifdef LIGHTSABERS
	PN_LIGHTSABER,
#endif
	PN_ATTACK_SPELL,     PN_HEALING_SPELL,
	PN_DIVINATION_SPELL, PN_ENCHANTMENT_SPELL,
	PN_PROTECTION_SPELL,            PN_BODY_SPELL,
	PN_MATTER_SPELL,
	PN_BARE_HANDED, 		PN_MARTIAL_ARTS, 
	PN_TWO_WEAPONS,
#ifdef STEED
	PN_RIDING,
#endif
};


STATIC_OVL NEARDATA const char * const odd_skill_names[] = {
    "no skill",
    "polearms",
    "saber",
    "hammer",
    "whip",
    "paddle",
    "firearms",
    "attack spells",
    "healing spells",
    "divination spells",
    "enchantment spells",
    "protection spells",
    "body spells",
    "matter spells",
    "bare-handed combat",
    "martial arts",
    "riding",
    "two-handed combat",
#ifdef LIGHTSABERS
    "lightsaber"
#endif
};


STATIC_OVL void
give_may_advance_msg(skill)
int skill;
{
	You_feel("more %s in your %sskills.",
	    !P_RESTRICTED(skill) ? "confident" : "comfortable",
		skill == P_NONE ?
			"" :
		skill <= P_LAST_WEAPON ?
			"weapon " :
		skill <= P_LAST_SPELL ?
			"spell casting " :
            skill <= P_LAST_H_TO_H ?
	            "fighting ":
	            "");
}

#endif	/* OVLB */

STATIC_DCL boolean FDECL(can_advance, (int, BOOLEAN_P));
STATIC_DCL boolean FDECL(could_advance, (int));
STATIC_DCL boolean FDECL(peaked_skill, (int));
STATIC_DCL int FDECL(slots_required, (int));
STATIC_DCL boolean FDECL(can_practice, (int)); /* WAC for Practicing */

#ifdef OVL1

STATIC_DCL char *FDECL(skill_level_name, (int,char *));
STATIC_DCL void FDECL(skill_advance, (int));

#endif	/* OVL1 */

#ifdef OVLB

#define P_NAME(type) (skill_names_indices[type] > 0 ? \
		      OBJ_NAME(objects[skill_names_indices[type]]) : \
			odd_skill_names[-skill_names_indices[type]])

static NEARDATA const char kebabable[] = {
	S_XORN, S_DRAGON, S_JABBERWOCK, S_NAGA, S_GIANT, '\0'
};

/*
 *	hitval returns an integer representing the "to hit" bonuses
 *	of "otmp" against the monster.
 */
int
hitval(otmp, mon)
struct obj *otmp;
struct monst *mon;
{
	int	tmp = 0;
	struct permonst *ptr = mon->data;
	boolean Is_weapon = (otmp->oclass == WEAPON_CLASS || is_weptool(otmp));

	if (Is_weapon)
		tmp += otmp->spe;

/*	Put weapon specific "to hit" bonuses in below:		*/
	tmp += objects[otmp->otyp].oc_hitbon;
	tmp += weapon_hit_bonus(otmp);  /* weapon skill */
	if (u.twoweap && (otmp == uwep || otmp == uswapwep))
		tmp += (skill_bonus(P_TWO_WEAPON_COMBAT) * 2) - 5;

/*	Put weapon vs. monster type "to hit" bonuses in below:	*/

	/* Blessed weapons used against undead or demons */
	if (Is_weapon && otmp->blessed &&
	   (is_demon(ptr) || is_undead(ptr))) tmp += 2;

	/* KMH, balance patch -- new macro */
	if (is_spear(otmp) && index(kebabable, ptr->mlet)) tmp += 2;

	/* KMH -- Paddles are effective against insects */
	if (Is_weapon && (objects[otmp->otyp].oc_skill == P_PADDLE) &&
			(ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN))
		tmp += 2;

	/* trident is highly effective against swimmers */
	if (otmp->otyp == TRIDENT && is_swimmer(ptr)) {
	   if (is_pool(mon->mx, mon->my)) tmp += 4;
	   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) tmp += 2;
	}

	/* pick-axe used against xorns and earth elementals */
	/* WAC made generic against "rock people" */
	/* KMH, balance patch -- allow all picks */
	if (is_pick(otmp) &&
/*           (passes_walls(ptr) && thick_skinned(ptr))) tmp += 2;*/
           (made_of_rock(ptr))) tmp += 2;


#ifdef INVISIBLE_OBJECTS
	/* invisible weapons against monsters who can't see invisible */
	if (otmp->oinvis && !perceives(ptr)) tmp += 3;
#endif

	/* Check specially named weapon "to hit" bonuses */
	if (otmp->oartifact) tmp += spec_abon(otmp, mon);

	return tmp;
}

/* Historical note: The original versions of Hack used a range of damage
 * which was similar to, but not identical to the damage used in Advanced
 * Dungeons and Dragons.  I figured that since it was so close, I may as well
 * make it exactly the same as AD&D, adding some more weapons in the process.
 * This has the advantage that it is at least possible that the player would
 * already know the damage of at least some of the weapons.  This was circa
 * 1987 and I used the table from Unearthed Arcana until I got tired of typing
 * them in (leading to something of an imbalance towards weapons early in
 * alphabetical order).  The data structure still doesn't include fields that
 * fully allow the appropriate damage to be described (there's no way to say
 * 3d6 or 1d6+1) so we add on the extra damage in dmgval() if the weapon
 * doesn't do an exact die of damage.
 *
 * Of course new weapons were added later in the development of Nethack.  No
 * AD&D consistency was kept, but most of these don't exist in AD&D anyway.
 *
 * Second edition AD&D came out a few years later; luckily it used the same
 * table.  As of this writing (1999), third edition is in progress but not
 * released.  Let's see if the weapon table stays the same.  --KAA
 * October 2000: It didn't.  Oh, well.
 */

/*
 *	dmgval returns an integer representing the damage bonuses
 *	of "otmp" against the monster.
 */
int
dmgval(otmp, mon)
struct obj *otmp;
struct monst *mon;
{
	int tmp = 0, otyp = otmp->otyp;
	struct permonst *ptr = mon->data;
	boolean Is_weapon = (otmp->oclass == WEAPON_CLASS || is_weptool(otmp));

	if (otyp == CREAM_PIE) return 0;

# ifdef P_SPOON
	if (otmp->oartifact == ART_HOUCHOU)
	        return 9999;
# endif /* P_SPOON */

	if (bigmonst(ptr)) {
	    if (objects[otyp].oc_wldam)
		tmp = rnd(objects[otyp].oc_wldam);
	    switch (otyp) {
		case IRON_CHAIN:
		case CROSSBOW_BOLT:
		case MORNING_STAR:
		case PARTISAN:
		case RUNESWORD:
		case ELVEN_BROADSWORD:
		case BROADSWORD:	tmp++; break;

		case FLAIL:
		case RANSEUR:
		case VOULGE:		tmp += rnd(4); break;

		case ACID_VENOM:
		case HALBERD:
		case SPETUM:		tmp += rnd(6); break;

		case BATTLE_AXE:
		case BARDICHE:
		case TRIDENT:		tmp += d(2,4); break;

		case TSURUGI:
		case DWARVISH_MATTOCK:
		case TWO_HANDED_SWORD:	tmp += d(2,6); break;

#ifdef LIGHTSABERS
		case GREEN_LIGHTSABER:  tmp +=13; break;
#ifdef D_SABER
		case BLUE_LIGHTSABER:   tmp +=12; break;
#endif
		case RED_DOUBLE_LIGHTSABER: 
					if (otmp->altmode) tmp += rnd(11);
					/* fallthrough */
		case RED_LIGHTSABER:    tmp +=10; break;
#endif
	    }
	} else {
	    if (objects[otyp].oc_wsdam)
		tmp = rnd(objects[otyp].oc_wsdam);
	    switch (otyp) {
		case IRON_CHAIN:
		case CROSSBOW_BOLT:
		case MACE:
		case SILVER_MACE:
		case WAR_HAMMER:
		case FLAIL:
		case SPETUM:
		case TRIDENT:		tmp++; break;

		case BATTLE_AXE:
		case BARDICHE:
		case BILL_GUISARME:
		case GUISARME:
		case LUCERN_HAMMER:
		case MORNING_STAR:
		case RANSEUR:
		case BROADSWORD:
		case ELVEN_BROADSWORD:
		case RUNESWORD:
		case VOULGE:		tmp += rnd(4); break;

#ifdef LIGHTSABERS
		case GREEN_LIGHTSABER:  tmp +=9; break;
#ifdef D_SABER
		case BLUE_LIGHTSABER:   tmp +=8; break;
#endif
		case RED_DOUBLE_LIGHTSABER:
					if (otmp->altmode) tmp += rnd(9);
					/* fallthrough */
		case RED_LIGHTSABER: 	tmp +=6; break;
#endif

		case ACID_VENOM:	tmp += rnd(6); break;
	    }
	}
	if (Is_weapon) {
		tmp += otmp->spe;
		/* negative enchantment mustn't produce negative damage */
		if (tmp < 0) tmp = 0;
	}

	if (objects[otyp].oc_material <= LEATHER && thick_skinned(ptr))
		/* thick skinned/scaled creatures don't feel it */
		tmp = 0;
	if (ptr == &mons[PM_SHADE] && objects[otyp].oc_material != SILVER)
		tmp = 0;

	/* "very heavy iron ball"; weight increase is in increments of 160 */
	if (otyp == HEAVY_IRON_BALL && tmp > 0) {
	    int wt = (int)objects[HEAVY_IRON_BALL].oc_weight;

	    if ((int)otmp->owt > wt) {
		wt = ((int)otmp->owt - wt) / 160;
		tmp += rnd(4 * wt);
		if (tmp > 25) tmp = 25;	/* objects[].oc_wldam */
	    }
	}

/*	Put weapon vs. monster type damage bonuses in below:	*/
	if (Is_weapon || otmp->oclass == GEM_CLASS ||
		otmp->oclass == BALL_CLASS || otmp->oclass == CHAIN_CLASS) {
	    int bonus = 0;

	    if (otmp->blessed && (is_undead(ptr) || is_demon(ptr)))
		bonus += rnd(4);
	    if (is_axe(otmp) && is_wooden(ptr))
		bonus += rnd(4);
	    if (objects[otyp].oc_material == SILVER && hates_silver(ptr))
		bonus += rnd(20);

	    /* Ralf Engels - added more special cases*/
	    /* You can kill a eye with a needle */
	    /* WAC--currently disabled since spheres and gas spores are S_EYE too */
/*	    if((objects[otyp].oc_dir & (PIERCE) ) && (ptr->mlet == S_EYE))
		  	bonus += 2; */
	    /* You cut worms */
	    if((objects[otyp].oc_dir & (SLASH) ) && (ptr->mlet == S_WORM))
			bonus += 2;
	    /* You pierce blobs */
	    if((objects[otyp].oc_dir & (PIERCE) ) && (ptr->mlet == S_BLOB))
			bonus += 2;
	    /* You slash jellies */
	    if((objects[otyp].oc_dir & (SLASH) ) && (ptr->mlet == S_JELLY))
			bonus += 2;
	    /* concussion damage is better agains chitinous armour */
	    if( (objects[otyp].oc_dir & (WHACK) ) &&
	        (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN))
			bonus += 2; 
	    /* flyers can better be reached with a polearm */
	    if( (is_pole(otmp) || is_spear(otmp) ) && is_flyer(ptr) )
			bonus += 2; 
	    if (is_pick(otmp) && made_of_rock(ptr) ) 
			bonus += 3;

	    /* if the weapon is going to get a double damage bonus, adjust
	       this bonus so that effectively it's added after the doubling */
	    if (bonus > 1 && otmp->oartifact && spec_dbon(otmp, mon, 25) >= 25)
		bonus = (bonus + 1) / 2;

	    tmp += bonus;
	}

	if (tmp > 0) {
		/* It's debateable whether a rusted blunt instrument
		   should do less damage than a pristine one, since
		   it will hit with essentially the same impact, but
		   there ought to some penalty for using damaged gear
		   so always subtract erosion even for blunt weapons. */
		tmp -= greatest_erosion(otmp);
		if (tmp < 1) tmp = 1;
	}

	return(tmp);
}

#endif /* OVLB */
#ifdef OVL0

STATIC_DCL struct obj *FDECL(oselect, (struct monst *,int));
#define Oselect(x)	if ((otmp = oselect(mtmp, x)) != 0) return(otmp);

STATIC_OVL struct obj *
oselect(mtmp, x)
struct monst *mtmp;
int x;
{
	struct obj *otmp;

	for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj) {
	    if (otmp->otyp == x &&
		    /* never select non-cockatrice corpses */
		    !((x == CORPSE || x == EGG) &&
			!touch_petrifies(&mons[otmp->corpsenm])) &&
#ifdef LIGHTSABERS
                    (!is_lightsaber(otmp) || otmp->age) &&
#endif
		    (!otmp->oartifact || touch_artifact(otmp,mtmp)))
		return otmp;
	}
	return (struct obj *)0;
}


/* WAC added the Ya, increased priority of silver dagger, added Spoon */
/* KMH -- added bullets */
static NEARDATA const int rwep[] =
{	
#ifdef SPOON
	SPOON,
#endif
#ifdef FIREARMS
	FRAG_GRENADE, GAS_GRENADE, ROCKET, SILVER_BULLET, BULLET, SHOTGUN_SHELL,
#endif
	DWARVISH_SPEAR, SILVER_SPEAR, ELVEN_SPEAR, SPEAR, ORCISH_SPEAR,
	JAVELIN, SHURIKEN, YA, SILVER_ARROW, ELVEN_ARROW, DARK_ELVEN_ARROW, 
	ARROW, ORCISH_ARROW, CROSSBOW_BOLT, SILVER_DAGGER, ELVEN_DAGGER, 
	DARK_ELVEN_DAGGER, DAGGER, ORCISH_DAGGER, KNIFE, FLINT, ROCK, 
	LOADSTONE, LUCKSTONE, DART,
	/* BOOMERANG, */ CREAM_PIE
	/* note: CREAM_PIE should NOT be #ifdef KOPS */
};

static NEARDATA const int pwep[] =
{	HALBERD, BARDICHE, SPETUM, BILL_GUISARME, VOULGE, RANSEUR, GUISARME,
	GLAIVE, LUCERN_HAMMER, BEC_DE_CORBIN, FAUCHARD, PARTISAN, LANCE
};


static struct obj *propellor;

struct obj *
select_rwep(mtmp)	/* select a ranged weapon for the monster */
register struct monst *mtmp;
{
	register struct obj *otmp;
	int i;

#ifdef KOPS
	char mlet = mtmp->data->mlet;
#endif

	propellor = &zeroobj;
	Oselect(EGG); /* cockatrice egg */
#ifdef KOPS
	if(mlet == S_KOP)	/* pies are first choice for Kops */
	    Oselect(CREAM_PIE);
#endif
	if(throws_rocks(mtmp->data))	/* ...boulders for giants */
	    Oselect(BOULDER);

	/* Select polearms first; they do more damage and aren't expendable */
	/* The limit of 13 here is based on the monster polearm range limit
	 * (defined as 5 in mthrowu.c).  5 corresponds to a distance of 2 in
	 * one direction and 1 in another; one space beyond that would be 3 in
	 * one direction and 2 in another; 3^2+2^2=13.
	 */
	if (dist2(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) <= 13 && couldsee(mtmp->mx, mtmp->my)) {
	    for (i = 0; i < SIZE(pwep); i++) {
		/* Only strong monsters can wield big (esp. long) weapons.
		 * Big weapon is basically the same as bimanual.
		 * All monsters can wield the remaining weapons.
		 */
		if (((strongmonst(mtmp->data) && (mtmp->misc_worn_check & W_ARMS) == 0)
			|| !objects[pwep[i]].oc_bimanual) &&
		    (objects[pwep[i]].oc_material != SILVER
			|| !hates_silver(mtmp->data))) {
		    if ((otmp = oselect(mtmp, pwep[i])) != 0) {
			propellor = otmp; /* force the monster to wield it */
			return otmp;
		    }
		}
	    }
	}

	/*
	 * other than these two specific cases, always select the
	 * most potent ranged weapon to hand.
	 */
	for (i = 0; i < SIZE(rwep); i++) {
	    int prop;

	    /* shooting gems from slings; this goes just before the darts */
	    /* (shooting rocks is already handled via the rwep[] ordering) */
	    if (rwep[i] == DART && !likes_gems(mtmp->data) &&
		    m_carrying(mtmp, SLING)) {		/* propellor */
		for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
		    if (otmp->oclass == GEM_CLASS &&
			    (otmp->otyp != LOADSTONE || !otmp->cursed)) {
			propellor = m_carrying(mtmp, SLING);
			return otmp;
		    }
	    }


		/* KMH -- This belongs here so darts will work */
	    propellor = &zeroobj;

	    /* KMH, balance patch -- now using skills */
	    prop = (objects[rwep[i]]).oc_skill;
	    if (prop < 0) {
		switch (-prop) {
			/* WAC NOTE: remember to always start the 1st item in 
			 *   a list of propellors with a
			 * 	propellor = ...
			 *   and follow up with
			 *   if (!propellor) ...
			 */
		case P_BOW:
		  propellor = (oselect(mtmp, YUMI));
		  if (!propellor) propellor = (oselect(mtmp, ELVEN_BOW));
		  /* WAC added dark elven bow */
		  if (!propellor) propellor = (oselect(mtmp, DARK_ELVEN_BOW));
		  if (!propellor) propellor = (oselect(mtmp, BOW));
		  if (!propellor) propellor = (oselect(mtmp, ORCISH_BOW));
		  break;
		case P_SLING:
		  propellor = (oselect(mtmp, SLING));
		  break;
		case P_CROSSBOW:
		  propellor = (oselect(mtmp, CROSSBOW));
#ifdef FIREARMS
		case P_FIREARM:
		  if ((objects[rwep[i]].w_ammotyp) == WP_BULLET) {
			propellor = (oselect(mtmp, HEAVY_MACHINE_GUN));
			if (!propellor) propellor = (oselect(mtmp, ASSAULT_RIFLE));
			if (!propellor) propellor = (oselect(mtmp, SUBMACHINE_GUN));
			if (!propellor) propellor = (oselect(mtmp, SNIPER_RIFLE));
			if (!propellor) propellor = (oselect(mtmp, RIFLE));
			if (!propellor) propellor = (oselect(mtmp, PISTOL));
		  } else if ((objects[rwep[i]].w_ammotyp) == WP_SHELL) {
			propellor = (oselect(mtmp, AUTO_SHOTGUN));
			if (!propellor) propellor = (oselect(mtmp, SHOTGUN));
		  } else if ((objects[rwep[i]].w_ammotyp) == WP_ROCKET) {
			propellor = (oselect(mtmp, ROCKET_LAUNCHER));
		  } else if ((objects[rwep[i]].w_ammotyp) == WP_GRENADE) {
			propellor = (oselect(mtmp, GRENADE_LAUNCHER));
			if (!propellor) propellor = &zeroobj;  /* can toss grenades */
		  }
		  break;
#endif
		}
		if ((otmp = MON_WEP(mtmp)) && otmp->cursed && otmp != propellor
				&& mtmp->weapon_check == NO_WEAPON_WANTED)
			propellor = 0;
	    }
	    /* propellor = obj, propellor to use
	     * propellor = &zeroobj, doesn't need a propellor
	     * propellor = 0, needed one and didn't have one
	     */
	    if (propellor != 0) {
		/* Note: cannot use m_carrying for loadstones, since it will
		 * always select the first object of a type, and maybe the
		 * monster is carrying two but only the first is unthrowable.
		 */
		/* STEPHEN WHITE'S NEW CODE */
		if (rwep[i] != LOADSTONE) {
			/* Don't throw a cursed weapon-in-hand or an artifact */
			if ((otmp = oselect(mtmp, rwep[i])) && !otmp->oartifact
			    && (!otmp->cursed || otmp != MON_WEP(mtmp)))
				return(otmp);
		/* STEPHEN WHITE'S NEW CODE */
		/* KMH, balance patch -- removed stone of rotting */
		} else for(otmp=mtmp->minvent; otmp; otmp=otmp->nobj) {
		    if (otmp->otyp == LOADSTONE && !otmp->cursed)
			return otmp;
		}
	    }
	  }

	/* failure */
	return (struct obj *)0;
}

/* Weapons in order of preference */
/* WAC -- added dark elven short sword here */
/* WAC -- removed polearms */
static const NEARDATA short hwep[] = {
	  CORPSE,  /* cockatrice corpse */
	  TSURUGI, RUNESWORD, HEAVY_HAMMER, 
	  DWARVISH_MATTOCK, 
#ifdef LIGHTSABERS
	  RED_DOUBLE_LIGHTSABER, RED_LIGHTSABER,
#ifdef D_SABER
	  BLUE_LIGHTSABER,
#endif
	  GREEN_LIGHTSABER,
#endif
	  TWO_HANDED_SWORD, BATTLE_AXE,
	  KATANA, UNICORN_HORN, CRYSKNIFE, TRIDENT, LONG_SWORD,
	  ELVEN_BROADSWORD, BROADSWORD, SCIMITAR, SILVER_SABER,
	  SILVER_SHORT_SWORD, SILVER_LONG_SWORD, SILVER_MACE,
  	  MORNING_STAR, DARK_ELVEN_SHORT_SWORD, ELVEN_SHORT_SWORD, 
  	  DWARVISH_SHORT_SWORD, SHORT_SWORD,
	  ORCISH_SHORT_SWORD, MACE, AXE, DWARVISH_SPEAR, SILVER_SPEAR,
	  ELVEN_SPEAR, SPEAR, ORCISH_SPEAR, FLAIL, BULLWHIP, QUARTERSTAFF,
	  JAVELIN, AKLYS, CLUB, PICK_AXE, FLY_SWATTER, 
#ifdef KOPS
	  RUBBER_HOSE,
#endif /* KOPS */
	  WAR_HAMMER, SILVER_DAGGER, ELVEN_DAGGER, WOODEN_STAKE, DAGGER, 
	  ORCISH_DAGGER,
	  ATHAME, SCALPEL, KNIFE, TORCH, WORM_TOOTH
};

struct obj *
select_hwep(mtmp)	/* select a hand to hand weapon for the monster */
register struct monst *mtmp;
{
	register struct obj *otmp;
	register int i;
	boolean strong = strongmonst(mtmp->data);
	boolean wearing_shield = (mtmp->misc_worn_check & W_ARMS) != 0;

	/* prefer artifacts to everything else */
	for(otmp=mtmp->minvent; otmp; otmp = otmp->nobj) {
		if (otmp->oclass == WEAPON_CLASS
			&& otmp->oartifact && touch_artifact(otmp,mtmp)
			&& ((strong && !wearing_shield)
			    || !objects[otmp->otyp].oc_bimanual))
		    return otmp;
	}

	if(is_giant(mtmp->data))	/* giants just love to use clubs */
	    Oselect(CLUB);

	/* only strong monsters can wield big (esp. long) weapons */
	/* big weapon is basically the same as bimanual */
	/* all monsters can wield the remaining weapons */
	for (i = 0; i < SIZE(hwep); i++) {
	    if (hwep[i] == CORPSE && !(mtmp->misc_worn_check & W_ARMG))
		continue;
	    if (((strong && !wearing_shield)
			|| !objects[hwep[i]].oc_bimanual) &&
		    (objects[hwep[i]].oc_material != SILVER
			|| !hates_silver(mtmp->data)))
		Oselect(hwep[i]);
	}

	/* failure */
	return (struct obj *)0;
}

/* Called after polymorphing a monster, robbing it, etc....  Monsters
 * otherwise never unwield stuff on their own.  Might print message.
 */
void
possibly_unwield(mon, polyspot)
struct monst *mon;
boolean polyspot;
{
	struct obj *obj, *mw_tmp;

	if (!(mw_tmp = MON_WEP(mon)))
		return;
	for (obj = mon->minvent; obj; obj = obj->nobj)
		if (obj == mw_tmp) break;
	if (!obj) { /* The weapon was stolen or destroyed */
		MON_NOWEP(mon);
		mon->weapon_check = NEED_WEAPON;
		return;
	}
	if (!attacktype(mon->data, AT_WEAP)) {
		setmnotwielded(mon, mw_tmp);
		MON_NOWEP(mon);
		mon->weapon_check = NO_WEAPON_WANTED;
		obj_extract_self(obj);
		if (cansee(mon->mx, mon->my)) {
		    pline("%s drops %s.", Monnam(mon),
			  distant_name(obj, doname));
		    newsym(mon->mx, mon->my);
		}
		/* might be dropping object into water or lava */
		if (!flooreffects(obj, mon->mx, mon->my, "drop")) {
		    if (polyspot) bypass_obj(obj);
		    place_object(obj, mon->mx, mon->my);
		    stackobj(obj);
		}
		return;
	}
	/* The remaining case where there is a change is where a monster
	 * is polymorphed into a stronger/weaker monster with a different
	 * choice of weapons.  This has no parallel for players.  It can
	 * be handled by waiting until mon_wield_item is actually called.
	 * Though the monster still wields the wrong weapon until then,
	 * this is OK since the player can't see it.  (FIXME: Not okay since
	 * probing can reveal it.)
	 * Note that if there is no change, setting the check to NEED_WEAPON
	 * is harmless.
	 * Possible problem: big monster with big cursed weapon gets
	 * polymorphed into little monster.  But it's not quite clear how to
	 * handle this anyway....
	 */
	if (!(mw_tmp->cursed && mon->weapon_check == NO_WEAPON_WANTED))
	    mon->weapon_check = NEED_WEAPON;
	return;
}

/* Let a monster try to wield a weapon, based on mon->weapon_check.
 * Returns 1 if the monster took time to do it, 0 if it did not.
 */
int
mon_wield_item(mon)
register struct monst *mon;
{
	struct obj *obj;

	/* This case actually should never happen */
	if (mon->weapon_check == NO_WEAPON_WANTED) return 0;
	switch(mon->weapon_check) {
		case NEED_HTH_WEAPON:
			obj = select_hwep(mon);
			break;
		case NEED_RANGED_WEAPON:
			(void)select_rwep(mon);
			obj = propellor;
			
			break;
		case NEED_PICK_AXE:
			obj = m_carrying(mon, PICK_AXE);
			/* KMH -- allow other picks */
			if (!obj && !which_armor(mon, W_ARMS))
			    obj = m_carrying(mon, DWARVISH_MATTOCK);
			break;
		case NEED_AXE:
			/* currently, only 2 types of axe */
			obj = m_carrying(mon, BATTLE_AXE);
			if (!obj || which_armor(mon, W_ARMS))
			    obj = m_carrying(mon, AXE);
			break;
		case NEED_PICK_OR_AXE:
			/* prefer pick for fewer switches on most levels */
			obj = m_carrying(mon, DWARVISH_MATTOCK);
			if (!obj) obj = m_carrying(mon, BATTLE_AXE);
			if (!obj || which_armor(mon, W_ARMS)) {
			    obj = m_carrying(mon, PICK_AXE);
			    if (!obj) obj = m_carrying(mon, AXE);
			}
			break;
		default: impossible("weapon_check %d for %s?",
				mon->weapon_check, mon_nam(mon));
			return 0;
	}
	if (obj && obj != &zeroobj) {
		struct obj *mw_tmp = MON_WEP(mon);
		
		if (mw_tmp && mw_tmp->otyp == obj->otyp) {
		/* already wielding it */
#ifdef LIGHTSABERS
			if (is_lightsaber(obj))
			    mon_ignite_lightsaber(obj, mon);
#endif
			mon->weapon_check = NEED_WEAPON;
			return 0;
		}
		/* Actually, this isn't necessary--as soon as the monster
		 * wields the weapon, the weapon welds itself, so the monster
		 * can know it's cursed and needn't even bother trying.
		 * Still....
		 */
		if (mw_tmp && mw_tmp->cursed && mw_tmp->otyp != CORPSE) {
		    if (canseemon(mon)) {
			char welded_buf[BUFSZ];
			const char *mon_hand = mbodypart(mon, HAND);

			if (bimanual(mw_tmp)) mon_hand = makeplural(mon_hand);
			Sprintf(welded_buf, "%s welded to %s %s",
				otense(mw_tmp, "are"),
				mhis(mon), mon_hand);

			if (obj->otyp == PICK_AXE) {
			    pline("Since %s weapon%s %s,",
				  s_suffix(mon_nam(mon)),
				  plur(mw_tmp->quan), welded_buf);
			    pline("%s cannot wield that %s.",
				mon_nam(mon), xname(obj));
			} else {
			    pline("%s tries to wield %s.", Monnam(mon),
				doname(obj));
			    pline("%s %s %s!",
				  s_suffix(Monnam(mon)),
				  xname(mw_tmp), welded_buf);
			}
			mw_tmp->bknown = 1;
		    }
		    mon->weapon_check = NO_WEAPON_WANTED;
		    return 1;
		}
		mon->mw = obj;		/* wield obj */
		setmnotwielded(mon, mw_tmp);
		mon->weapon_check = NEED_WEAPON;
		if (canseemon(mon)) {
		    pline("%s wields %s!", Monnam(mon), doname(obj));
		    if (obj->cursed && obj->otyp != CORPSE) {
			pline("%s %s to %s %s!",
			    Tobjnam(obj, "weld"),
			    is_plural(obj) ? "themselves" : "itself",
			    s_suffix(mon_nam(mon)), mbodypart(mon,HAND));
			obj->bknown = 1;
		    }
		}
		if (artifact_light(obj) && !obj->lamplit) {
		    begin_burn(obj, FALSE);
		    if (canseemon(mon))
			pline("%s brilliantly in %s %s!",
			    Tobjnam(obj, "glow"), 
			    s_suffix(mon_nam(mon)), mbodypart(mon,HAND));
		}
		obj->owornmask = W_WEP;
#ifdef LIGHTSABERS
		if (is_lightsaber(obj))
		    mon_ignite_lightsaber(obj, mon);
#endif
		return 1;
	}
	mon->weapon_check = NEED_WEAPON;
	return 0;
}

#ifdef LIGHTSABERS
static void
mon_ignite_lightsaber(obj, mon)
struct obj * obj;
struct monst * mon;
{
	/* No obj or not lightsaber */
	if (!obj || !is_lightsaber(obj)) return;

	/* WAC - Check lightsaber is on */
	if (!obj->lamplit) {
	    if (obj->cursed && !rn2(2)) {
		if (canseemon(mon)) pline("%s %s flickers and goes out.", 
			s_suffix(Monnam(mon)), xname(obj));

	    } else {
		if (canseemon(mon)) {
			makeknown(obj->otyp);
			pline("%s ignites %s.", Monnam(mon),
				an(xname(obj)));
		}	    	
		begin_burn(obj, FALSE);
	    }
	} else {
		/* Double Lightsaber in single mode? Ignite second blade */
		if (obj->otyp == RED_DOUBLE_LIGHTSABER && !obj->altmode) {
		    /* Do we want to activate dual bladed mode? */
		    if (!obj->altmode && (!obj->cursed || rn2(4))) {
			if (canseemon(mon)) pline("%s ignites the second blade of %s.", 
				Monnam(mon), an(xname(obj)));
		    	obj->altmode = TRUE;
		    	return;
		    } else obj->altmode = FALSE;
		    lightsaber_deactivate(obj, TRUE);
		}
		return;
	}
}
#endif

/* STEPHEN WHITE'S NEW CODE */
int
abon()		/* attack bonus for strength & dexterity */
{
	int sbon;
	int str = ACURR(A_STR), dex = ACURR(A_DEX);

	if (Upolyd) return(adj_lev(&mons[u.umonnum]) - 3);
	/* [Tom] lowered these a little */        
	if (str < 6) sbon = -2;
	else if (str < 8) sbon = -1;
	else if (str < 17) sbon = 0;
	else if (str <= STR18(50)) sbon = 1;	/* up to 18/50 */
	else if (str < STR18(100)) sbon = 1;
	else if (str == STR18(100)) sbon = 2;  /* 18/00 */
	else if (str == STR19(19)) sbon = 2;  /* 19 */
	else if (str == STR19(20)) sbon = 3;  /* 20 */
	else if (str == STR19(21)) sbon = 3;  /* 21 */
	else if (str == STR19(22)) sbon = 4;  /* 22 */
	else if (str == STR19(23)) sbon = 4;  /* 23 */
	else if (str == STR19(24)) sbon = 5;  /* 24 */
	else sbon = 5;
  
	if (dex < 5) sbon -= 2;
	else if (dex < 7) sbon -= 1;
	else if (dex < 15) sbon += 0;
	else if (dex == 15) sbon += 1;  /* 15 */
	else if (dex == 16) sbon += 1;  /* 16 */
	else if (dex == 17) sbon += 1;  /* 17 */
	else if (dex == 18) sbon += 2;  /* 18 */
	else if (dex == 19) sbon += 2;  /* 19 */
	else if (dex == 20) sbon += 2;  /* 20 */
	else if (dex == 21) sbon += 3;  /* 21 */
	else if (dex == 22) sbon += 3;  /* 22 */
	else if (dex == 23) sbon += 3;  /* 23 */
	else if (dex == 24) sbon += 4;  /* 24 */
	else sbon += 5;

/* Game tuning kludge: make it a bit easier for a low level character to hit */
	sbon += (u.ulevel < 3) ? 1 : 0;
	return(sbon);
}

#endif /* OVL0 */
#ifdef OVL1

/* STEPHEN WHITE'S NEW CODE */
int
dbon()		/* damage bonus for strength */
{
	int str = ACURR(A_STR);

	if (Upolyd) return(0);
	/* [Tom] I lowered this a little */
	if (str < 6) return(-1);
	else if (str < 16) return(0);
	else if (str < 18) return(1);
	else if (str == 18) return(2);		/* up to 18 */
	else if (str < STR18(100)) return(3);          /* up to 18/99 */
	else if (str == STR18(100)) return(4);         /* 18/00 */
	else if (str == STR19(19)) return(5);         /* 19 */
	else if (str == STR19(20)) return(6);         /* 20 */
	else if (str == STR19(21)) return(7);         /* 21 */
	else if (str == STR19(22)) return(8);         /* 22 */
	else if (str == STR19(23)) return(9);         /* 23 */
	else if (str == STR19(24)) return(10);        /* 24 */
	else return(11);
}

/* copy the skill level name into the given buffer */
STATIC_OVL char *
skill_level_name(skill, buf)
int skill;
char *buf;
{
    const char *ptr;

    switch (P_SKILL(skill)) {
	case P_UNSKILLED:    ptr = "Unskilled"; break;
	case P_BASIC:	     ptr = "Basic";     break;
	case P_SKILLED:	     ptr = "Skilled";   break;
	case P_EXPERT:	     ptr = "Expert";    break;
	case P_MASTER:	     ptr = "Master";    break;
	case P_GRAND_MASTER: 
            if (skill <= P_LAST_WEAPON)
                    ptr = "Wizard" ;
            else if (skill <= P_LAST_SPELL)
                    ptr = "Legendary";
	    else if (skill <= P_LAST_H_TO_H) 
	    	    ptr = "Grand Master"; 
	    else ptr = "Unprecedented";
	    break;
	default:	     ptr = "Unknown";	break;
    }
    Strcpy(buf, ptr);
    return buf;
}

/* return the # of slots required to advance the skill */
STATIC_OVL int
slots_required(skill)
int skill;
{
    int tmp = P_SKILL(skill);

      /* WAC if you're over your class max,  it's twice as costly */      
      if (tmp >= P_MAX_SKILL(skill)) tmp *=2;
      if (tmp < 0) tmp = 0; /* for Restricted skills */
  
    /* The more difficult the training, the more slots it takes.
     *	unskilled -> basic	1
     *	basic -> skilled	2
     *	skilled -> expert	3
     */
    if (skill <= P_LAST_SPELL)
	return tmp;

      /* Fewer slots used up for unarmed or martial, miscellaneous skills
     *	unskilled -> basic	1
     *	basic -> skilled	1
     *	skilled -> expert	2
     *	expert -> master	2
     *	master -> grand master	3
     */
    return (tmp + 1) / 2;
}

/* return true if this skill can be advanced */
/*ARGSUSED*/
STATIC_OVL boolean
can_advance(skill, speedy)
int skill;
boolean speedy;
{
    return !P_RESTRICTED(skill)
	    && P_SKILL(skill) < P_MAX_SKILL(skill) && (
#ifdef WIZARD
	    (wizard && speedy) ||
#endif
	    (P_ADVANCE(skill) >=
		(unsigned) practice_needed_to_advance(P_SKILL(skill), skill)
	    && u.skills_advanced < P_SKILL_LIMIT
	    && u.weapon_slots >= slots_required(skill)));
}

/* WAC return true if skill can be practiced */
STATIC_OVL boolean
can_practice(skill)
int skill;
{
    return !P_RESTRICTED(skill)
            && P_SKILL(skill) < P_MAX_SKILL(skill)
            && u.skills_advanced < P_SKILL_LIMIT;
}



/* return true if this skill could be advanced if more slots were available */
STATIC_OVL boolean
could_advance(skill)
int skill;
{
    return !P_RESTRICTED(skill)
	    && P_SKILL(skill) < P_MAX_SKILL(skill) && (
	    (P_ADVANCE(skill) >=
		(unsigned) practice_needed_to_advance(P_SKILL(skill), skill)
	    && u.skills_advanced < P_SKILL_LIMIT));
}

/* return true if this skill has reached its maximum and there's been enough
   practice to become eligible for the next step if that had been possible */
STATIC_OVL boolean
peaked_skill(skill)
int skill;
{
    return !P_RESTRICTED(skill)
	    && P_SKILL(skill) >= P_MAX_SKILL(skill) && (
	    (P_ADVANCE(skill) >=
		(unsigned) practice_needed_to_advance(P_SKILL(skill), skill)));
}

STATIC_OVL void
skill_advance(skill)
int skill;
{
#ifdef WIZARD
    if (wizard && speed_advance && P_RESTRICTED(skill)) {
    	unrestrict_weapon_skill(skill);
	return;
    }
#endif
    
    u.weapon_slots -= slots_required(skill);
    P_SKILL(skill)++;
    u.skill_record[u.skills_advanced++] = skill;
    /* subtly change the advance message to indicate no more advancement */
    You("are now %s skilled in %s.",
	P_SKILL(skill) >= P_MAX_SKILL(skill) ? "most" : "more",
	P_NAME(skill));
    if (!tech_known(T_DISARM) && (P_SKILL(skill) == P_SKILLED) && 
    		skill <= P_LAST_WEAPON && skill != P_WHIP) {
    	learntech(T_DISARM, FROMOUTSIDE, 1);
    	You("learn how to perform disarm!");
    }
}

const static struct skill_range {
	short first, last;
	const char *name;
} skill_ranges[] = {
    { P_FIRST_H_TO_H, P_LAST_H_TO_H, "Fighting Skills" },
    { P_FIRST_WEAPON, P_LAST_WEAPON, "Weapon Skills" },
    { P_FIRST_SPELL,  P_LAST_SPELL,  "Spellcasting Skills" },
};

/*
 * The `#enhance' extended command.  What we _really_ would like is
 * to keep being able to pick things to advance until we couldn't any
 * more.  This is currently not possible -- the menu code has no way
 * to call us back for instant action.  Even if it did, we would also need
 * to be able to update the menu since selecting one item could make
 * others unselectable.
 */
int
enhance_weapon_skill()
{
    int pass, i, n, len, longest,
	to_advance, eventually_advance, maxxed_cnt;
    char buf[BUFSZ], sklnambuf[BUFSZ];
    const char *prefix;
    menu_item *selected;
    anything any;
    winid win;
    boolean speedy = FALSE;

#ifdef WIZARD
	if (wizard && yn("Advance skills without practice?") == 'y')
	    speedy = TRUE;
#endif

	do {
	    /* find longest available skill name, count those that can advance */
	    to_advance = eventually_advance = maxxed_cnt = 0;
	    for (longest = 0, i = 0; i < P_NUM_SKILLS; i++) {
		if (P_RESTRICTED(i)) continue;
		if (i == P_TWO_WEAPON_COMBAT &&
			youmonst.data->mattk[1].aatyp != AT_WEAP)
		    continue;
		if ((len = strlen(P_NAME(i))) > longest)
		    longest = len;
		if (can_advance(i, speedy)) to_advance++;
		else if (could_advance(i)) eventually_advance++;
		else if (peaked_skill(i)) maxxed_cnt++;
	    }

	    win = create_nhwindow(NHW_MENU);
	    start_menu(win);

	    /* start with a legend if any entries will be annotated
	       with "*" or "#" below */
	    if (eventually_advance > 0 || maxxed_cnt > 0) {
		any.a_void = 0;
		if (eventually_advance > 0) {
		    Sprintf(buf,
			    "(Skill%s flagged by \"*\" may be enhanced %s.)",
			    plur(eventually_advance),
			    (u.ulevel < MAXULEV) ?
				"when you're more experienced" :
				"if skill slots become available");
		    add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE,
			     buf, MENU_UNSELECTED);
		}
		if (maxxed_cnt > 0) {
		    Sprintf(buf,
		  "(Skill%s flagged by \"#\" cannot be enhanced any further.)",
			    plur(maxxed_cnt));
		    add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE,
			     buf, MENU_UNSELECTED);
		}
		add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE,
			     "", MENU_UNSELECTED);
	    }

	    /* List the skills, making ones that could be advanced
	       selectable.  List the miscellaneous skills first.
	       Possible future enhancement:  list spell skills before
	       weapon skills for spellcaster roles. */
	  for (pass = 0; pass < SIZE(skill_ranges); pass++)
	    for (i = skill_ranges[pass].first;
		 i <= skill_ranges[pass].last; i++) {
		/* Print headings for skill types */
		any.a_void = 0;
		if (i == skill_ranges[pass].first)
		    add_menu(win, NO_GLYPH, &any, 0, 0, iflags.menu_headings,
			     skill_ranges[pass].name, MENU_UNSELECTED);

		if (P_RESTRICTED(i)) continue;
		if (i == P_TWO_WEAPON_COMBAT &&
			youmonst.data->mattk[1].aatyp != AT_WEAP)
		    continue;
		/*
		 * Sigh, this assumes a monospaced font unless
		 * iflags.menu_tab_sep is set in which case it puts
		 * tabs between columns.
		 * The 12 is the longest skill level name.
		 * The "    " is room for a selection letter and dash, "a - ".
		 */
		if (can_advance(i, speedy))
		    prefix = "";	/* will be preceded by menu choice */
		else if (could_advance(i))
		    prefix = "  * ";
		else if (peaked_skill(i))
		    prefix = "  # ";
		else
		    prefix = (to_advance + eventually_advance +
				maxxed_cnt > 0) ? "    " : "";
		(void) skill_level_name(i, sklnambuf);
#ifdef WIZARD
		if (wizard) {
		    if (!iflags.menu_tab_sep)
			Sprintf(buf, " %s%-*s %-12s %4d(%4d)",
			    prefix, longest, P_NAME(i), sklnambuf,
			    P_ADVANCE(i),
			    practice_needed_to_advance(P_SKILL(i), i));
		    else
			Sprintf(buf, " %s%s\t%s\t%5d(%4d)",
			    prefix, P_NAME(i), sklnambuf,
			    P_ADVANCE(i),
			    practice_needed_to_advance(P_SKILL(i), i));
		 } else
#endif
		{
		    if (!iflags.menu_tab_sep)
			Sprintf(buf, " %s %-*s [%s]",
			    prefix, longest, P_NAME(i), sklnambuf);
		    else
			Sprintf(buf, " %s%s\t[%s]",
			    prefix, P_NAME(i), sklnambuf);
		}
		any.a_int = can_advance(i, speedy) ? i+1 : 0;
		add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE,
			 buf, MENU_UNSELECTED);
	    }

	    Strcpy(buf, (to_advance > 0) ? "Pick a skill to advance:" :
					   "Current skills:");
#ifdef WIZARD
	    if (wizard && !speedy)
		Sprintf(eos(buf), "  (%d slot%s available)",
			u.weapon_slots, plur(u.weapon_slots));
#endif
	    end_menu(win, buf);
	    n = select_menu(win, to_advance ? PICK_ONE : PICK_NONE, &selected);
	    destroy_nhwindow(win);
	    if (n > 0) {
		n = selected[0].item.a_int - 1;	/* get item selected */
		free((genericptr_t)selected);
		skill_advance(n);
		/* check for more skills able to advance, if so then .. */
		for (n = i = 0; i < P_NUM_SKILLS; i++) {
		    if (can_advance(i, speedy)) {
			if (!speedy) You_feel("you could be more dangerous!");
			n++;
			break;
		    }
		}
	    }
	} while (speedy && n > 0);
	return 0;
}

/*
 * Change from restricted to unrestricted, allowing P_BASIC as max.  This
 * function may be called with with P_NONE.  Used in pray.c.
 */
void
unrestrict_weapon_skill(skill)
int skill;
{
    if (skill < P_NUM_SKILLS && P_RESTRICTED(skill)) {
	P_SKILL(skill) = P_UNSKILLED;
	P_MAX_SKILL(skill) = P_BASIC;
	P_ADVANCE(skill) = 0;
    }
}

#endif /* OVL1 */
#ifdef OVLB

void
use_skill(skill,degree)
int skill;
int degree;
{
    boolean advance_before;

/*    if (skill != P_NONE && !P_RESTRICTED(skill)) {*/
    if (skill != P_NONE) {
	advance_before = can_advance(skill, FALSE);
	P_ADVANCE(skill) += degree;
	if (!advance_before && can_advance(skill, FALSE)) {
	    give_may_advance_msg(skill);
	    if (P_RESTRICTED(skill)) {
	    	unrestrict_weapon_skill(skill);
	    }
	}
    }
}

void
add_weapon_skill(n)
int n;	/* number of slots to gain; normally one */
{
    int i, before, after;

    for (i = 0, before = 0; i < P_NUM_SKILLS; i++)
	if (can_advance(i, FALSE)) before++;
    u.weapon_slots += n;
    for (i = 0, after = 0; i < P_NUM_SKILLS; i++)
	if (can_advance(i, FALSE)) after++;
    if (before < after)
	give_may_advance_msg(P_NONE);
}

void
lose_weapon_skill(n)
int n;	/* number of slots to lose; normally one */
{
    int skill;
    boolean maybe_loose_disarm = FALSE;

    while (--n >= 0) {
	/* deduct first from unused slots, then from last placed slot, if any */
	if (u.weapon_slots) {
	    u.weapon_slots--;
	} else if (u.skills_advanced) {
	    skill = u.skill_record[--u.skills_advanced];
	    if (P_SKILL(skill) <= P_UNSKILLED)
		panic("lose_weapon_skill (%d)", skill);
	    if (skill <= P_LAST_WEAPON && skill != P_WHIP &&
		    P_SKILL(skill) == P_SKILLED)
		maybe_loose_disarm = TRUE;
	    P_SKILL(skill)--;	/* drop skill one level */
	    /* Lost skill might have taken more than one slot; refund rest. */
	    u.weapon_slots = slots_required(skill) - 1;
	    /* It might now be possible to advance some other pending
	       skill by using the refunded slots, but giving a message
	       to that effect would seem pretty confusing.... */
	}
    }

    if (maybe_loose_disarm && tech_known(T_DISARM)) {
	int i;
	for(i = u.skills_advanced - 1; i >= 0; i--) {
	    skill = u.skill_record[i];
	    if (skill <= P_LAST_WEAPON && skill != P_WHIP &&
		    P_SKILL(skill) >= P_SKILLED)
		break;
	}
	if (i < 0)
	    learntech(T_DISARM, FROMOUTSIDE, -1);
    }
}

int
weapon_type(obj)
struct obj *obj;
{
	/* KMH, balance patch -- now uses the object table */
	int type;

	if (!obj)
		/* Not using a weapon */
	    return (martial_bonus() ? P_MARTIAL_ARTS :
				P_BARE_HANDED_COMBAT);
	if (obj->oclass != WEAPON_CLASS && obj->oclass != TOOL_CLASS &&
	    obj->oclass != GEM_CLASS)
		/* Not a weapon, weapon-tool, or ammo */
		return (P_NONE);
	type = objects[obj->otyp].oc_skill;
	return ((type < 0) ? -type : type);
}

int
uwep_skill_type()
{
	if (u.twoweap)
		return P_TWO_WEAPON_COMBAT;
	return weapon_type(uwep);
}

/*
 * Return hit bonus/penalty based on skill of weapon.
 * Treat restricted weapons as unskilled.
 */
int
weapon_hit_bonus(weapon)
struct obj *weapon;
{
    int type, wep_type, bonus = 0;
#if 0
    int skill;
#endif
    static const char bad_skill[] = "weapon_hit_bonus: bad skill %d";

    wep_type = weapon_type(weapon);
#if 0
    /* use two weapon skill only if attacking with one of the wielded weapons */
    type = (u.twoweap && (weapon == uwep || weapon == uswapwep)) ?
	    P_TWO_WEAPON_COMBAT : wep_type;
#endif
    type = wep_type;
    if (type == P_NONE) {
	bonus = 0;
    } else if (type <= P_LAST_WEAPON) {
	switch (P_SKILL(type)) {
	    default: impossible(bad_skill, P_SKILL(type)); /* fall through */
	    case P_ISRESTRICTED:
	    case P_UNSKILLED:   bonus = -4; break;
	    case P_BASIC:       bonus =  0; break;
		    case P_SKILLED:     bonus =  1; break;
		    case P_EXPERT:      bonus =  2; break;
		    case P_MASTER:	bonus =  3; break;
		    case P_GRAND_MASTER:	bonus =  4; break;
	}
/* WAC -- No longer needed here...  */
#if 0
    } else if (type == P_TWO_WEAPON_COMBAT) {
	skill = P_SKILL(P_TWO_WEAPON_COMBAT);
	if (P_SKILL(wep_type) < skill) skill = P_SKILL(wep_type);
	switch (skill) {
	    default: impossible(bad_skill, skill); /* fall through */
	    case P_ISRESTRICTED:
	    case P_UNSKILLED:   bonus = -9; break;
	    case P_BASIC:	bonus = -7; break;
	    case P_SKILLED:	bonus = -5; break;
	    case P_EXPERT:	bonus = -3; break;
	    case P_MASTER:	bonus = -1; break;
	    case P_GRAND_MASTER:	bonus =  1; break;
	}
	if (u.twoweap) bonus -= 2;
	}
#endif
        /* WAC  because we split Martial Arts and Bare handed */
    } else if (type <= P_LAST_H_TO_H) {
	/*
	 *	       b.h.  m.a.
	 *	unskl:	+1   n/a
	 *	basic:	+1    +3
	 *	skild:	+2    +4
	 *	exprt:	+2    +5
	 *	mastr:	+3    +6
	 *	grand:	+3    +7
	 */
	bonus = P_SKILL(type);
	bonus = max(bonus,P_UNSKILLED) - 1;	/* unskilled => 0 */
	bonus = ((bonus + 2) * (martial_bonus() ? 2 : 1)) / 2;
    }

#ifdef STEED
	/* KMH -- It's harder to hit while you are riding */
	if (u.usteed) {
		switch (P_SKILL(P_RIDING)) {
		    case P_ISRESTRICTED:
		    case P_UNSKILLED:   bonus -= 2; break;
		    case P_BASIC:       bonus -= 1; break;
		    case P_SKILLED:     break;
		    case P_EXPERT:      break;
		    case P_MASTER:	bonus += 1; break;
		    case P_GRAND_MASTER:	bonus += 2; break;
		}
		if (type == P_LANCE) bonus++;
	}
#endif

    return bonus;
}

/*
 * Return damage bonus/penalty based on skill of weapon.
 * Treat restricted weapons as unskilled.
 */
int
weapon_dam_bonus(weapon)
struct obj *weapon;
{
    int type, wep_type, bonus = 0;
#if 0
    int skill;
#endif

    wep_type = weapon_type(weapon);
#if 0
    /* use two weapon skill only if attacking with one of the wielded weapons */
    type = (u.twoweap && (weapon == uwep || weapon == uswapwep)) ?
	    P_TWO_WEAPON_COMBAT : wep_type;
#endif
    type = wep_type;
    if (type == P_NONE) {
	bonus = 0;
    } else if (type <= P_LAST_WEAPON) {
	switch (P_SKILL(type)) {
	    default: impossible("weapon_dam_bonus: bad skill %d",P_SKILL(type));
		     /* fall through */
	    case P_ISRESTRICTED:
	    case P_UNSKILLED:	bonus = -2; break;
	    case P_BASIC:	bonus =  0; break;
	    case P_SKILLED:	bonus =  1; break;
	    case P_EXPERT:	bonus =  2; break;
	    case P_MASTER:	bonus =  3; break;
	    case P_GRAND_MASTER:bonus =  4; break;
	}
#if 0
    } else if (type == P_TWO_WEAPON_COMBAT) {
	skill = P_SKILL(P_TWO_WEAPON_COMBAT);
	if (P_SKILL(wep_type) < skill) skill = P_SKILL(wep_type);
	switch (skill) {
	    default:
	    case P_ISRESTRICTED:
	    case P_UNSKILLED:	bonus = -3; break;
	    case P_BASIC:	bonus = -1; break;
	    case P_SKILLED:	bonus = 0; break;
	    case P_EXPERT:	bonus = 1; break;
	}
#endif
        /* from == P_bare... to < = P_last_h.... */
    } else if (type <= P_LAST_H_TO_H) {
	/*
	 *	       b.h.  m.a.
	 *	unskl:	 0   n/a
	 *	basic:	+1    +3
	 *	skild:	+1    +4
	 *	exprt:	+2    +6
	 *	mastr:	+2    +7
	 *	grand:	+3    +9
	 */
	bonus = P_SKILL(type);
	bonus = max(bonus,P_UNSKILLED) - 1;	/* unskilled => 0 */
	bonus = ((bonus + 1) * (martial_bonus() ? 3 : 1)) / 2;
    } /* Misc skills aren't usually called by weapons */

#ifdef STEED
	/* KMH -- Riding gives some thrusting damage */
	if (u.usteed) {
		switch (P_SKILL(P_RIDING)) {
		    case P_ISRESTRICTED:
		    case P_UNSKILLED:   break;
		    case P_BASIC:       break;
		    case P_SKILLED:     bonus += 1; break;
		    case P_EXPERT:      bonus += 2; break;
		}
	}
#endif

    return bonus;
}

int
skill_bonus(type)
int type;
{
    int bonus = 0;
    if (type == P_NONE) {
		bonus = 0;
    } else if (type <= P_LAST_WEAPON) {
		switch (P_SKILL(type)) {
		    default: impossible("skill_bonus: bad skill %d",P_SKILL(type));
			     /* fall through */
		    case P_ISRESTRICTED:
		    case P_UNSKILLED:   bonus = -2; break;
		    case P_BASIC:       bonus =  0; break;
		    case P_SKILLED:     bonus =  1; break;
		    case P_EXPERT:      bonus =  2; break;
		    case P_MASTER:	bonus =  3; break;
		    case P_GRAND_MASTER:	bonus =  4; break;
		}
    } else if (type == P_BARE_HANDED_COMBAT || type == P_MARTIAL_ARTS) {
		bonus = (P_SKILL(type) * (martial_bonus() ? 2 : 1)) / 2;
    } else {
    		/* Misc. */
		switch (P_SKILL(type)) {
		    default: impossible("skill_bonus: bad skill %d",P_SKILL(type));
			     /* fall through */
		    case P_ISRESTRICTED:
		    case P_UNSKILLED:   bonus = -2; break;
		    case P_BASIC:       bonus =  0; break;
		    case P_SKILLED:     bonus =  1; break;
		    case P_EXPERT:      bonus =  2; break;
		    case P_MASTER:	bonus =  3; break;
		    case P_GRAND_MASTER:	bonus =  4; break;
		}
    }

    return bonus;
}

/* Try to return an associated skill for the specified object */
static 
int get_obj_skill(obj)
struct obj *obj;
{
	int skill;
	
	/* Try for a weapon skill */
	skill = weapon_type(obj);

	/* Try for a spell type */
	if (skill == P_NONE && obj->oclass == SPBOOK_CLASS) 
		skill = spell_skilltype(obj->otyp);
	
	/* Should be integrated into oc_subtyp as soon as more skills are 
	 * invented 
	 */
#ifdef STEED
	if (obj->otyp == SADDLE) skill = P_RIDING;
#endif
    	/* Negative skills == not a skill */
	if (skill < P_NONE) skill = P_NONE;

	return (skill);
}

/*
 * Initialize weapon skill array for the game.  Start by setting all
 * skills to restricted, then set the skill for every weapon the
 * hero is holding, finally reading the given array that sets
 * maximums.
 */
void
skill_init(class_skill)
const struct def_skill *class_skill;
{
	struct obj *obj;
	int skmax, skill;

	/* initialize skill array; by default, everything is restricted */
	for (skill = 0; skill < P_NUM_SKILLS; skill++) {
	    P_SKILL(skill) = P_ISRESTRICTED;
	    P_MAX_SKILL(skill) = P_ISRESTRICTED;
	    P_ADVANCE(skill) = 0;
	}

	/* Walk through array to set skill maximums */
	for (; class_skill->skill != P_NONE; class_skill++) {
	    skill = class_skill->skill;
	    skmax = class_skill->skmax;

	    P_MAX_SKILL(skill) = skmax;
	    if (P_SKILL(skill) == P_ISRESTRICTED)       /* skill pre-set */
			P_SKILL(skill) = P_UNSKILLED;
	    /* Really high potential in the skill
	     * Right now only used for H to H skills
	     */
	    if (P_MAX_SKILL(skill) > P_EXPERT) P_SKILL(skill) = P_BASIC;
	}

	/* Set skill for all objects in inventory to be basic */
	for (obj = invent; obj; obj = obj->nobj) {
	    skill = get_obj_skill(obj);
	    if (skill != P_NONE) {
		P_SKILL(skill) = P_BASIC;
		/* KMH -- If you came into the dungeon with it, you should at least be skilled */
		if (P_MAX_SKILL(skill) < P_SKILLED) {
			pline("Warning: %s should be at least skilled.  Fixing...", P_NAME(skill));
			P_MAX_SKILL(skill) = P_SKILLED;
		}
	    }
	}

#if 0  /* This should all be handled above now... */
	/* set skills for magic */
    /* WAC - added setup for role is F, I, M or N*/
	if (Role_if(PM_HEALER) || Role_if(PM_MONK)) {
		P_SKILL(P_HEALING_SPELL) = P_BASIC;
	} else if (Role_if(PM_PRIEST)) {
		P_SKILL(P_PROTECTION_SPELL) = P_BASIC;
    } else if (Role_if(PM_NECROMANCER)) {
		P_SKILL(P_ATTACK_SPELL) = P_BASIC;
    } else if (Role_if(PM_FLAME_MAGE) || Role_if(PM_ICE_MAGE))  {
		P_SKILL(P_MATTER_SPELL) = P_BASIC;
	} else if (Role_if(PM_WIZARD)) {
		P_SKILL(P_ATTACK_SPELL) = P_BASIC;
		P_SKILL(P_ENCHANTMENT_SPELL) = P_BASIC;
	}

	/* High potential fighters already know how to use their hands. */
	if (P_MAX_SKILL(P_BARE_HANDED_COMBAT) > P_EXPERT)
	    P_SKILL(P_BARE_HANDED_COMBAT) = P_BASIC;
	if (P_MAX_SKILL(P_MARTIAL_ARTS) > P_EXPERT)
		P_SKILL(P_MARTIAL_ARTS) = P_BASIC;
#endif

	/* Roles that start with a horse know how to ride it */
#ifdef STEED
	if (urole.petnum == PM_PONY)
	    P_SKILL(P_RIDING) = P_BASIC;
#endif

	/*
	 * Make sure we haven't missed setting the max on a skill
	 * & set advance
	 */
	for (skill = 0; skill < P_NUM_SKILLS; skill++) {
	    if (!P_RESTRICTED(skill)) {
		if (P_MAX_SKILL(skill) < P_SKILL(skill)) {
		    impossible("skill_init: curr > max: %s", P_NAME(skill));
		    P_MAX_SKILL(skill) = P_SKILL(skill);
		}
		P_ADVANCE(skill) = practice_needed_to_advance(P_SKILL(skill)-1,skill);
	    }
	}
}

/*WAC  weapon practice code*/
STATIC_PTR int
practice()
{
	if (delay) {    /* not if (delay++), so at end delay == 0 */
		delay++;
		use_skill(weapon_type(uwep), 1);
		/*WAC a bit of practice so even if you're interrupted
		  you won't be wasting your time ;B*/
		return(1); /* still busy */
    }
	You("finish your practice session.");
	use_skill(weapon_type(uwep),
	    practice_needed_to_advance(P_SKILL(weapon_type(uwep)),weapon_type(uwep))/3);
	return(0);
}

void
practice_weapon()
{
	if (can_practice(weapon_type(uwep))
#ifdef WIZARD
	    || (wizard && (yn("Skill at normal max. Practice?") == 'y'))
#endif
	    ) {
		if (uwep)    
		    You("start practicing intensely with %s",doname(uwep));
		else
		    You("start practicing intensely with your %s %s.",
		            uarmg ? "gloved" : "bare",      /* Del Lamb */
		makeplural(body_part(HAND)));

		delay=-10;
		set_occupation(practice, "practicing", 0);
	} else if (P_SKILL(weapon_type(uwep)) >= P_MAX_SKILL(weapon_type(uwep)))
		You("cannot increase your skill in %s.", P_NAME(weapon_type(uwep)));
	else You("cannot learn much about %s right now.",
                P_NAME(weapon_type(uwep)));
                
}

void
setmnotwielded(mon,obj)
register struct monst *mon;
register struct obj *obj;
{
    if (!obj) return;
    if (artifact_light(obj) && obj->lamplit) {
	end_burn(obj, FALSE);
	if (canseemon(mon))
	    pline("%s in %s %s %s glowing.", The(xname(obj)),
		  s_suffix(mon_nam(mon)), mbodypart(mon,HAND),
		  otense(obj, "stop"));
    }
    obj->owornmask &= ~W_WEP;
}

#endif /* OVLB */

/*weapon.c*/
