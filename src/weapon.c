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
STATIC_DCL char *FDECL(skill_level_name_max, (int,char *));
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
	boolean Is_weapon = (otmp->oclass == WEAPON_CLASS || otmp->oclass == GEM_CLASS || otmp->oclass == BALL_CLASS || otmp->oclass == CHAIN_CLASS || is_weptool(otmp));

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

	/* iron chains give bonus versus thick-skinned monsters --Amy */
	if (otmp->otyp == IRON_CHAIN && thick_skinned(ptr)) tmp += 4;
	if (otmp->otyp == STONE_CHAIN && thick_skinned(ptr)) tmp += 4;
	if (otmp->otyp == GLASS_CHAIN && thick_skinned(ptr)) tmp += 4;
	if (otmp->otyp == ROTATING_CHAIN && thick_skinned(ptr)) tmp += 8;
	if (otmp->otyp == GOLD_CHAIN && thick_skinned(ptr)) tmp += 8;
	if (otmp->otyp == CLAY_CHAIN && thick_skinned(ptr)) tmp += 8;
	if (otmp->otyp == SCOURGE && thick_skinned(ptr)) tmp += 15;
	if (otmp->otyp == ELYSIUM_SCOURGE && thick_skinned(ptr)) tmp += 15;
	if (otmp->otyp == GRANITE_SCOURGE && thick_skinned(ptr)) tmp += 15;
	if (otmp->otyp == NUNCHIAKU && thick_skinned(ptr)) tmp += 24;
	if (otmp->otyp == CONUNDRUM_NUNCHIAKU && thick_skinned(ptr)) tmp += 24;
	if (otmp->otyp == CONCRETE_NUNCHIAKU && thick_skinned(ptr)) tmp += 24;
	if (otmp->otyp == HOSTAGE_CHAIN && thick_skinned(ptr)) tmp += 32;
	if (otmp->otyp == GLASS_HOSTAGE_CHAIN && thick_skinned(ptr)) tmp += 32;
	if (otmp->otyp == MINERAL_HOSTAGE_CHAIN && thick_skinned(ptr)) tmp += 32;
	if (otmp->otyp == ELYSIUM_HOSTAGE_CHAIN && thick_skinned(ptr)) tmp += 32;

	/* KMH -- Paddles are effective against insects */
	if (otmp->otyp == FLY_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN))
		tmp += 5;
	if (otmp->otyp == INSECT_SQUASHER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN))
		tmp += 12;
	if (otmp->otyp == LASER_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN))
		tmp += 15;

	if (otmp->otyp == FLY_SWATTER && ptr == &mons[PM_INVINCIBLE_SUPERMAN]) tmp += 25;
	if (otmp->otyp == INSECT_SQUASHER && ptr == &mons[PM_INVINCIBLE_SUPERMAN]) tmp += 50;
	if (otmp->otyp == LASER_SWATTER && ptr == &mons[PM_INVINCIBLE_SUPERMAN]) tmp += 75;

	/* blunt weapons versus undead (Diablo 2) */
	if ((objects[otmp->otyp].oc_skill == P_PICK_AXE || objects[otmp->otyp].oc_skill == P_CLUB || objects[otmp->otyp].oc_skill == P_MACE || objects[otmp->otyp].oc_skill == P_PADDLE || objects[otmp->otyp].oc_skill == P_MORNING_STAR || otmp->otyp == FLAIL || otmp->otyp == KNOUT || otmp->otyp == OBSID || objects[otmp->otyp].oc_skill == P_HAMMER) && is_undead(ptr)) tmp += 3;
	if (objects[otmp->otyp].oc_skill == P_QUARTERSTAFF && is_undead(ptr)) tmp += 8;

	/* as well as silver bullets */
	if (otmp->otyp == SILVER_BULLET && is_undead(ptr)) tmp += 8;

	/* lances versus animals */
	if (objects[otmp->otyp].oc_skill == P_LANCE && is_animal(ptr)) tmp += 3;

	/* polearms versus golems */
	if (objects[otmp->otyp].oc_skill == P_POLEARMS && ptr->mlet == S_GOLEM) tmp += 3;

	/* electric sword versus quantum mechanic */
	if (otmp->otyp == ELECTRIC_SWORD && ptr->mlet == S_QUANTMECH) tmp += 10;

	/* shotgun versus bears or other quadrupeds */
	if (otmp->otyp == SHOTGUN_SHELL && ptr->mlet == S_QUADRUPED) tmp += 10;

	/* axes versus umber hulks */
	if (objects[otmp->otyp].oc_skill == P_AXE && ptr->mlet == S_UMBER) tmp += 3;

	/* whips for lashing people's asses :P */
	if (objects[otmp->otyp].oc_skill == P_WHIP && ptr->mlet == S_HUMAN) tmp += 2;

	/* trident is highly effective against swimmers */
	if (otmp->otyp == TRIDENT && is_swimmer(ptr)) {
	   if (is_pool(mon->mx, mon->my)) tmp += 4;
	   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) tmp += 2;
	}

	if (otmp->otyp == STYGIAN_PIKE && is_swimmer(ptr)) {
	   if (is_pool(mon->mx, mon->my)) tmp += 10;
	   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) tmp += 5;
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
	if (otmp->oinvisreal) tmp += 7;
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
	boolean Is_weapon = (otmp->oclass == WEAPON_CLASS || otmp->oclass == GEM_CLASS || otmp->oclass == BALL_CLASS || otmp->oclass == CHAIN_CLASS || is_weptool(otmp));

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
		case DROVEN_BOLT:
		case MORNING_STAR:
		case JAGGED_STAR:
		case DEVIL_STAR:
		case PARTISAN:
		case RUNESWORD:
		case ELVEN_BROADSWORD:
		case BROADSWORD:	tmp++; break;

		case FLAIL:
		case KNOUT:
		case OBSID:
		case RANSEUR:
		case VOULGE:		tmp += rnd(4); break;

		case ACID_VENOM:
		case HALBERD:
		case SPETUM:		tmp += rnd(6); break;

		case TAIL_SPIKES:	tmp += rnd(6); tmp += rnd(6); tmp += rnd(6); tmp += rnd(6); tmp += rnd(6); tmp += rnd(6);
 						break;

		case BATTLE_AXE:
		case BARDICHE:
		case STYGIAN_PIKE:
		case TRIDENT:		tmp += d(2,4); break;

		case TSURUGI:
		case DWARVISH_MATTOCK:
		case TWO_HANDED_SWORD:	tmp += d(2,6); break;

		case SCIMITAR:
			if(otmp->oartifact == ART_REAVER) tmp += d(1,8); break;

#ifdef LIGHTSABERS
		case GREEN_LIGHTSABER:  tmp +=13; break;
#ifdef D_SABER
		case BLUE_LIGHTSABER:   tmp +=12; break;
#if 0
		case VIOLET_LIGHTSABER:
		case WHITE_LIGHTSABER:
		case YELLOW_LIGHTSABER:
#endif
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
		case DROVEN_BOLT:
		case MACE:
		case SILVER_MACE:
		case FLANGED_MACE:
		case REINFORCED_MACE:
		case WAR_HAMMER:
		case MALLET:
		case FLAIL:
		case KNOUT:
		case OBSID:
		case SPETUM:
		case STYGIAN_PIKE:
		case TRIDENT:		tmp++; break;

		case BATTLE_AXE:
		case BARDICHE:
		case BILL_GUISARME:
		case GUISARME:
		case LUCERN_HAMMER:
		case MORNING_STAR:
		case JAGGED_STAR:
		case DEVIL_STAR:
		case RANSEUR:
		case BROADSWORD:
		case ELVEN_BROADSWORD:
		case RUNESWORD:
		case VOULGE:		tmp += rnd(4); break;

#ifdef LIGHTSABERS
		case GREEN_LIGHTSABER:  tmp +=9; break;
#ifdef D_SABER
		case BLUE_LIGHTSABER:   tmp +=8; break;
#if 0
		case VIOLET_LIGHTSABER:
		case WHITE_LIGHTSABER:
		case YELLOW_LIGHTSABER:
#endif
#endif
		case RED_DOUBLE_LIGHTSABER:
					if (otmp->altmode) tmp += rnd(9);
					/* fallthrough */
		case RED_LIGHTSABER: 	tmp +=6; break;
#endif

		case ACID_VENOM:	tmp += rnd(6); break;
		case TAIL_SPIKES:	tmp += rnd(6); tmp += rnd(6); tmp += rnd(6); tmp += rnd(6); tmp += rnd(6); tmp += rnd(6); 
					break;
		case SCIMITAR:
			if(otmp->oartifact == ART_REAVER) tmp += d(1,8); break;
	    }
	}
	if (Is_weapon) {
		tmp += otmp->spe;
		/* negative enchantment mustn't produce negative damage */
		if (tmp < 0) tmp = 0;
	}

	if (objects[otyp].oc_material <= LEATHER && thick_skinned(ptr) && tmp > 0)
		/* thick skinned/scaled creatures don't feel it */
		tmp = 1;
	if (ptr == &mons[PM_SHADE] && objects[otyp].oc_material != SILVER)
		tmp = 0;

	/* "very heavy iron ball"; weight increase is in increments of 300 */
	if (otyp == HEAVY_IRON_BALL && tmp > 0) {
	    int wt = (int)objects[HEAVY_IRON_BALL].oc_weight;

	    if ((int)otmp->owt > wt) {
		wt = ((int)otmp->owt - wt) / 300;
		tmp += rnd(4 * wt);
		if (tmp > 100) tmp = 100;	/* objects[].oc_wldam */
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
	    /* Amy edit - re-enabled since floating eyes are annoying enough already if you're low-level */
	    if((objects[otyp].oc_dir & (PIERCE) ) && (ptr->mlet == S_EYE))
		  	bonus += 2;
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

	    if (is_spear(otmp) && index(kebabable, ptr->mlet)) bonus += rnd(2) ;

	    /* iron chains give bonus versus thick-skinned monsters --Amy */
	    if (otmp->otyp == IRON_CHAIN && thick_skinned(ptr)) bonus += rnd(4) ;
	    if (otmp->otyp == STONE_CHAIN && thick_skinned(ptr)) bonus += rnd(4) ;
	    if (otmp->otyp == GLASS_CHAIN && thick_skinned(ptr)) bonus += rnd(4) ;
	    if (otmp->otyp == ROTATING_CHAIN && thick_skinned(ptr)) bonus += rnd(8);
	    if (otmp->otyp == GOLD_CHAIN && thick_skinned(ptr)) bonus += rnd(8);
	    if (otmp->otyp == CLAY_CHAIN && thick_skinned(ptr)) bonus += rnd(8);
	    if (otmp->otyp == SCOURGE && thick_skinned(ptr)) bonus += rnd(15);
	    if (otmp->otyp == ELYSIUM_SCOURGE && thick_skinned(ptr)) bonus += rnd(15);
	    if (otmp->otyp == GRANITE_SCOURGE && thick_skinned(ptr)) bonus += rnd(15);
	    if (otmp->otyp == NUNCHIAKU && thick_skinned(ptr)) bonus += rnd(24);
	    if (otmp->otyp == CONUNDRUM_NUNCHIAKU && thick_skinned(ptr)) bonus += rnd(24);
	    if (otmp->otyp == CONCRETE_NUNCHIAKU && thick_skinned(ptr)) bonus += rnd(24);
	    if (otmp->otyp == HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);
	    if (otmp->otyp == GLASS_HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);
	    if (otmp->otyp == MINERAL_HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);
	    if (otmp->otyp == ELYSIUM_HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);

	    /* KMH -- Paddles are effective against insects */
	    if (otmp->otyp == FLY_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN))
		bonus += rnd(5);
	    if (otmp->otyp == INSECT_SQUASHER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN))
		bonus += rnd(12);
	    if (otmp->otyp == LASER_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN))
		bonus += 15;

	    if (otmp->otyp == FLY_SWATTER && ptr == &mons[PM_INVINCIBLE_SUPERMAN]) bonus += rnd(25);
	    if (otmp->otyp == INSECT_SQUASHER && ptr == &mons[PM_INVINCIBLE_SUPERMAN]) bonus += rnd(50);
	    if (otmp->otyp == LASER_SWATTER && ptr == &mons[PM_INVINCIBLE_SUPERMAN]) bonus += rnd(75);

	    /* trident is highly effective against swimmers */
	    if (otmp->otyp == TRIDENT && is_swimmer(ptr)) {
		   if (is_pool(mon->mx, mon->my)) bonus += 4;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 2;
	    }

	    if (otmp->otyp == STYGIAN_PIKE && is_swimmer(ptr)) {
		   if (is_pool(mon->mx, mon->my)) bonus += 10;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 5;
	    }

	    /* blunt weapons versus undead (Diablo 2) */
	    if ((objects[otmp->otyp].oc_skill == P_PICK_AXE || objects[otmp->otyp].oc_skill == P_CLUB || objects[otmp->otyp].oc_skill == P_MACE || objects[otmp->otyp].oc_skill == P_PADDLE || objects[otmp->otyp].oc_skill == P_MORNING_STAR || otmp->otyp == FLAIL || otmp->otyp == KNOUT || otmp->otyp == OBSID || objects[otmp->otyp].oc_skill == P_HAMMER) && is_undead(ptr)) bonus += rnd(2);
	    if (objects[otmp->otyp].oc_skill == P_QUARTERSTAFF && is_undead(ptr)) bonus += rnd(6);

	    /* as well as silver bullets */
	    if (otmp->otyp == SILVER_BULLET && is_undead(ptr)) bonus += 8;

	    /* lances versus animals */
	    if (objects[otmp->otyp].oc_skill == P_LANCE && is_animal(ptr)) bonus += rnd(2);

	    /* polearms versus golems */
	    if (objects[otmp->otyp].oc_skill == P_POLEARMS && ptr->mlet == S_GOLEM) bonus += rnd(2);

	    /* electric sword versus quantum mechanic */
	    if (otmp->otyp == ELECTRIC_SWORD && ptr->mlet == S_QUANTMECH) bonus += rnd(10);

	    /* shotgun versus bears or other quadrupeds */
	    if (otmp->otyp == SHOTGUN_SHELL && ptr->mlet == S_QUADRUPED) bonus += (10 + rnd(10));

	    /* axes versus umber hulks */
	    if (objects[otmp->otyp].oc_skill == P_AXE && ptr->mlet == S_UMBER) bonus += rnd(2);

	    /* whips for lashing people's asses :P */
	    if (objects[otmp->otyp].oc_skill == P_WHIP && ptr->mlet == S_HUMAN) bonus += 1;

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

	/*if (tmp > 127) tmp = 127;*/ /* sanity check --Amy ... but I think it's not needed? */

	return(tmp);
}

int
dmgvalX(otmp, mon)
struct obj *otmp;
struct monst *mon;
{
	int tmp = 0, otyp = otmp->otyp;
	struct permonst *ptr = mon->data;
	boolean Is_weapon = (otmp->oclass == WEAPON_CLASS || otmp->oclass == GEM_CLASS || otmp->oclass == BALL_CLASS || otmp->oclass == CHAIN_CLASS || is_weptool(otmp));

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
		case DROVEN_BOLT:
		case MORNING_STAR:
		case JAGGED_STAR:
		case DEVIL_STAR:
		case PARTISAN:
		case RUNESWORD:
		case ELVEN_BROADSWORD:
		case BROADSWORD:	tmp++; break;

		case FLAIL:
		case KNOUT:
		case OBSID:
		case RANSEUR:
		case VOULGE:		tmp += rnd(4); break;

		case ACID_VENOM:
		case HALBERD:
		case SPETUM:		tmp += rnd(6); break;

		case TAIL_SPIKES:	tmp += rnd(6); tmp += rnd(6); tmp += rnd(6); tmp += rnd(6); tmp += rnd(6); tmp += rnd(6);
 						break;

		case BATTLE_AXE:
		case BARDICHE:
		case STYGIAN_PIKE:
		case TRIDENT:		tmp += d(2,4); break;

		case TSURUGI:
		case DWARVISH_MATTOCK:
		case TWO_HANDED_SWORD:	tmp += d(2,6); break;

		case SCIMITAR:
			if(otmp->oartifact == ART_REAVER) tmp += d(1,8); break;

#ifdef LIGHTSABERS
		case GREEN_LIGHTSABER:  tmp +=13; break;
#ifdef D_SABER
		case BLUE_LIGHTSABER:   tmp +=12; break;
#if 0
		case VIOLET_LIGHTSABER:
		case WHITE_LIGHTSABER:
		case YELLOW_LIGHTSABER:
#endif
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
		case DROVEN_BOLT:
		case MACE:
		case SILVER_MACE:
		case FLANGED_MACE:
		case REINFORCED_MACE:
		case WAR_HAMMER:
		case MALLET:
		case FLAIL:
		case KNOUT:
		case OBSID:
		case SPETUM:
		case STYGIAN_PIKE:
		case TRIDENT:		tmp++; break;

		case BATTLE_AXE:
		case BARDICHE:
		case BILL_GUISARME:
		case GUISARME:
		case LUCERN_HAMMER:
		case MORNING_STAR:
		case JAGGED_STAR:
		case DEVIL_STAR:
		case RANSEUR:
		case BROADSWORD:
		case ELVEN_BROADSWORD:
		case RUNESWORD:
		case VOULGE:		tmp += rnd(4); break;

#ifdef LIGHTSABERS
		case GREEN_LIGHTSABER:  tmp +=9; break;
#ifdef D_SABER
		case BLUE_LIGHTSABER:   tmp +=8; break;
#if 0
		case VIOLET_LIGHTSABER:
		case WHITE_LIGHTSABER:
		case YELLOW_LIGHTSABER:
#endif
#endif
		case RED_DOUBLE_LIGHTSABER:
					if (otmp->altmode) tmp += rnd(9);
					/* fallthrough */
		case RED_LIGHTSABER: 	tmp +=6; break;
#endif

		case ACID_VENOM:	tmp += rnd(6); break;
		case TAIL_SPIKES:	tmp += rnd(6); tmp += rnd(6); tmp += rnd(6); tmp += rnd(6); tmp += rnd(6); tmp += rnd(6); 
					break;
		case SCIMITAR:
			if(otmp->oartifact == ART_REAVER) tmp += d(1,8); break;
	    }
	}
	if (Is_weapon) {
		tmp += otmp->spe;
		/* negative enchantment mustn't produce negative damage */
		if (tmp < 0) tmp = 0;
	}

	if (objects[otyp].oc_material <= LEATHER && thick_skinned(ptr) && tmp > 0)
		/* thick skinned/scaled creatures don't feel it */
		tmp = 1;
	if (ptr == &mons[PM_SHADE] && objects[otyp].oc_material != SILVER)
		tmp = 0;

	/* "very heavy iron ball"; weight increase is in increments of 300 */
	if (otyp == HEAVY_IRON_BALL && tmp > 0) {
	    int wt = (int)objects[HEAVY_IRON_BALL].oc_weight;

	    if ((int)otmp->owt > wt) {
		wt = ((int)otmp->owt - wt) / 300;
		tmp += rnd(4 * wt);
		if (tmp > 100) tmp = 100;	/* objects[].oc_wldam */
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
	    /* Amy edit - re-enabled since floating eyes are annoying enough already if you're low-level */
	    if((objects[otyp].oc_dir & (PIERCE) ) && (ptr->mlet == S_EYE))
		  	bonus += 2;
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

	    if (is_spear(otmp) && index(kebabable, ptr->mlet)) bonus += rnd(2) ;

	    /* iron chains give bonus versus thick-skinned monsters --Amy */
	    if (otmp->otyp == IRON_CHAIN && thick_skinned(ptr)) bonus += rnd(4) ;
	    if (otmp->otyp == STONE_CHAIN && thick_skinned(ptr)) bonus += rnd(4) ;
	    if (otmp->otyp == GLASS_CHAIN && thick_skinned(ptr)) bonus += rnd(4) ;
	    if (otmp->otyp == ROTATING_CHAIN && thick_skinned(ptr)) bonus += rnd(8);
	    if (otmp->otyp == GOLD_CHAIN && thick_skinned(ptr)) bonus += rnd(8);
	    if (otmp->otyp == CLAY_CHAIN && thick_skinned(ptr)) bonus += rnd(8);
	    if (otmp->otyp == SCOURGE && thick_skinned(ptr)) bonus += rnd(15);
	    if (otmp->otyp == ELYSIUM_SCOURGE && thick_skinned(ptr)) bonus += rnd(15);
	    if (otmp->otyp == GRANITE_SCOURGE && thick_skinned(ptr)) bonus += rnd(15);
	    if (otmp->otyp == NUNCHIAKU && thick_skinned(ptr)) bonus += rnd(24);
	    if (otmp->otyp == CONUNDRUM_NUNCHIAKU && thick_skinned(ptr)) bonus += rnd(24);
	    if (otmp->otyp == CONCRETE_NUNCHIAKU && thick_skinned(ptr)) bonus += rnd(24);
	    if (otmp->otyp == HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);
	    if (otmp->otyp == GLASS_HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);
	    if (otmp->otyp == MINERAL_HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);
	    if (otmp->otyp == ELYSIUM_HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);

	if (otmp->otyp == IRON_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += 1;
	if (otmp->otyp == IRON_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(2);
	if (otmp->otyp == IRON_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(3);
	if (otmp->otyp == IRON_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(4);

	if (otmp->otyp == STONE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += 1;
	if (otmp->otyp == STONE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(2);
	if (otmp->otyp == STONE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(3);
	if (otmp->otyp == STONE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(4);

	if (otmp->otyp == GLASS_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += 1;
	if (otmp->otyp == GLASS_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(2);
	if (otmp->otyp == GLASS_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(3);
	if (otmp->otyp == GLASS_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(4);

	if (otmp->otyp == ROTATING_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(2);
	if (otmp->otyp == ROTATING_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(4);
	if (otmp->otyp == ROTATING_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(6);
	if (otmp->otyp == ROTATING_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(8);

	if (otmp->otyp == GOLD_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(2);
	if (otmp->otyp == GOLD_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(4);
	if (otmp->otyp == GOLD_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(6);
	if (otmp->otyp == GOLD_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(8);

	if (otmp->otyp == CLAY_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(2);
	if (otmp->otyp == CLAY_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(4);
	if (otmp->otyp == CLAY_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(6);
	if (otmp->otyp == CLAY_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(8);

	if (otmp->otyp == SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(3);
	if (otmp->otyp == SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(6);
	if (otmp->otyp == SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(9);
	if (otmp->otyp == SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(12);

	if (otmp->otyp == ELYSIUM_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(3);
	if (otmp->otyp == ELYSIUM_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(6);
	if (otmp->otyp == ELYSIUM_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(9);
	if (otmp->otyp == ELYSIUM_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(12);

	if (otmp->otyp == GRANITE_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(3);
	if (otmp->otyp == GRANITE_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(6);
	if (otmp->otyp == GRANITE_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(9);
	if (otmp->otyp == GRANITE_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(12);

	if (otmp->otyp == NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(4);
	if (otmp->otyp == NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(8);
	if (otmp->otyp == NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(12);
	if (otmp->otyp == NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(16);

	if (otmp->otyp == CONUNDRUM_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(4);
	if (otmp->otyp == CONUNDRUM_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(8);
	if (otmp->otyp == CONUNDRUM_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(12);
	if (otmp->otyp == CONUNDRUM_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(16);

	if (otmp->otyp == CONCRETE_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(4);
	if (otmp->otyp == CONCRETE_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(8);
	if (otmp->otyp == CONCRETE_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(12);
	if (otmp->otyp == CONCRETE_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(16);

	if (otmp->otyp == HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(5);
	if (otmp->otyp == HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(10);
	if (otmp->otyp == HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(15);
	if (otmp->otyp == HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(20);

	if (otmp->otyp == GLASS_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(5);
	if (otmp->otyp == GLASS_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(10);
	if (otmp->otyp == GLASS_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(15);
	if (otmp->otyp == GLASS_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(20);

	if (otmp->otyp == MINERAL_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(5);
	if (otmp->otyp == MINERAL_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(10);
	if (otmp->otyp == MINERAL_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(15);
	if (otmp->otyp == MINERAL_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(20);

	if (otmp->otyp == ELYSIUM_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(5);
	if (otmp->otyp == ELYSIUM_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(10);
	if (otmp->otyp == ELYSIUM_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(15);
	if (otmp->otyp == ELYSIUM_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(20);

	    /* KMH -- Paddles are effective against insects */
	    if (otmp->otyp == FLY_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN))
		bonus += rnd(5);
	    if (otmp->otyp == INSECT_SQUASHER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN))
		bonus += rnd(12);
	    if (otmp->otyp == LASER_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN))
		bonus += rnd(15);

	if (otmp->otyp == FLY_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_SKILLED) ) bonus += rnd(2);
	if (otmp->otyp == FLY_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_EXPERT) ) bonus += rnd(5);
	if (otmp->otyp == FLY_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_MASTER) ) bonus += rnd(7);
	if (otmp->otyp == FLY_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_GRAND_MASTER) ) bonus += rnd(10);
	if (otmp->otyp == INSECT_SQUASHER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_SKILLED) ) bonus += rnd(4);
	if (otmp->otyp == INSECT_SQUASHER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_EXPERT) ) bonus += rnd(8);
	if (otmp->otyp == INSECT_SQUASHER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_MASTER) ) bonus += rnd(12);
	if (otmp->otyp == INSECT_SQUASHER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_GRAND_MASTER) ) bonus += rnd(16);
	if (otmp->otyp == LASER_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_SKILLED) ) bonus += rnd(10);
	if (otmp->otyp == LASER_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_EXPERT) ) bonus += rnd(20);
	if (otmp->otyp == LASER_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_MASTER) ) bonus += rnd(30);
	if (otmp->otyp == LASER_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_GRAND_MASTER) ) bonus += rnd(40);

	    if (otmp->otyp == FLY_SWATTER && ptr == &mons[PM_INVINCIBLE_SUPERMAN]) bonus += rnd(25);
	    if (otmp->otyp == INSECT_SQUASHER && ptr == &mons[PM_INVINCIBLE_SUPERMAN]) bonus += rnd(50);
	    if (otmp->otyp == LASER_SWATTER && ptr == &mons[PM_INVINCIBLE_SUPERMAN]) bonus += rnd(75);

	    /* trident is highly effective against swimmers */
	    if (otmp->otyp == TRIDENT && is_swimmer(ptr)) {
		   if (is_pool(mon->mx, mon->my)) bonus += 4;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 2;
	    }

	    if (otmp->otyp == STYGIAN_PIKE && is_swimmer(ptr)) {
		   if (is_pool(mon->mx, mon->my)) bonus += 10;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 5;
	    }

	    if (otmp->otyp == TRIDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_SKILLED) ) {
		   if (is_pool(mon->mx, mon->my)) bonus += 2;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 1;
	    }

	    if (otmp->otyp == STYGIAN_PIKE && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_SKILLED) ) {
		   if (is_pool(mon->mx, mon->my)) bonus += 4;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 2;
	    }

	    if (otmp->otyp == TRIDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_EXPERT) ) {
		   if (is_pool(mon->mx, mon->my)) bonus += 4;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 2;
	    }

	    if (otmp->otyp == STYGIAN_PIKE && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_EXPERT) ) {
		   if (is_pool(mon->mx, mon->my)) bonus += 8;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 4;
	    }

	    if (otmp->otyp == TRIDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_MASTER) ) {
		   if (is_pool(mon->mx, mon->my)) bonus += 8;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 4;
	    }

	    if (otmp->otyp == STYGIAN_PIKE && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_MASTER) ) {
		   if (is_pool(mon->mx, mon->my)) bonus += 16;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 8;
	    }

	    if (otmp->otyp == TRIDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_GRAND_MASTER) ) {
		   if (is_pool(mon->mx, mon->my)) bonus += 16;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 8;
	    }

	    if (otmp->otyp == STYGIAN_PIKE && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_GRAND_MASTER) ) {
		   if (is_pool(mon->mx, mon->my)) bonus += 32;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 16;
	    }

	    /* blunt weapons versus undead (Diablo 2) */
	    if ((objects[otmp->otyp].oc_skill == P_PICK_AXE || objects[otmp->otyp].oc_skill == P_CLUB || objects[otmp->otyp].oc_skill == P_MACE || objects[otmp->otyp].oc_skill == P_PADDLE || objects[otmp->otyp].oc_skill == P_MORNING_STAR || otmp->otyp == FLAIL || otmp->otyp == KNOUT || otmp->otyp == OBSID || objects[otmp->otyp].oc_skill == P_HAMMER) && is_undead(ptr)) bonus += rnd(2);

	    if ((objects[otmp->otyp].oc_skill == P_PICK_AXE || objects[otmp->otyp].oc_skill == P_CLUB || objects[otmp->otyp].oc_skill == P_MACE || objects[otmp->otyp].oc_skill == P_PADDLE || objects[otmp->otyp].oc_skill == P_MORNING_STAR || otmp->otyp == FLAIL || otmp->otyp == KNOUT || otmp->otyp == OBSID || objects[otmp->otyp].oc_skill == P_HAMMER) && is_undead(ptr) && (P_SKILL(objects[otmp->otyp].oc_skill) == P_SKILLED) ) bonus += rnd(2);
	    if ((objects[otmp->otyp].oc_skill == P_PICK_AXE || objects[otmp->otyp].oc_skill == P_CLUB || objects[otmp->otyp].oc_skill == P_MACE || objects[otmp->otyp].oc_skill == P_PADDLE || objects[otmp->otyp].oc_skill == P_MORNING_STAR || otmp->otyp == FLAIL || otmp->otyp == KNOUT || otmp->otyp == OBSID || objects[otmp->otyp].oc_skill == P_HAMMER) && is_undead(ptr) && (P_SKILL(objects[otmp->otyp].oc_skill) == P_EXPERT) ) bonus += rnd(4);
	    if ((objects[otmp->otyp].oc_skill == P_PICK_AXE || objects[otmp->otyp].oc_skill == P_CLUB || objects[otmp->otyp].oc_skill == P_MACE || objects[otmp->otyp].oc_skill == P_PADDLE || objects[otmp->otyp].oc_skill == P_MORNING_STAR || otmp->otyp == FLAIL || otmp->otyp == KNOUT || otmp->otyp == OBSID || objects[otmp->otyp].oc_skill == P_HAMMER) && is_undead(ptr) && (P_SKILL(objects[otmp->otyp].oc_skill) == P_MASTER) ) bonus += rnd(5);
	    if ((objects[otmp->otyp].oc_skill == P_PICK_AXE || objects[otmp->otyp].oc_skill == P_CLUB || objects[otmp->otyp].oc_skill == P_MACE || objects[otmp->otyp].oc_skill == P_PADDLE || objects[otmp->otyp].oc_skill == P_MORNING_STAR || otmp->otyp == FLAIL || otmp->otyp == KNOUT || otmp->otyp == OBSID || objects[otmp->otyp].oc_skill == P_HAMMER) && is_undead(ptr) && (P_SKILL(objects[otmp->otyp].oc_skill) == P_GRAND_MASTER) ) bonus += rnd(7);

	    if (objects[otmp->otyp].oc_skill == P_QUARTERSTAFF && is_undead(ptr)) bonus += rnd(6);
	    if (objects[otmp->otyp].oc_skill == P_QUARTERSTAFF && is_undead(ptr) && (P_SKILL(P_QUARTERSTAFF) == P_SKILLED)) bonus += rnd(5);
	    if (objects[otmp->otyp].oc_skill == P_QUARTERSTAFF && is_undead(ptr) && (P_SKILL(P_QUARTERSTAFF) == P_EXPERT)) bonus += rnd(10);
	    if (objects[otmp->otyp].oc_skill == P_QUARTERSTAFF && is_undead(ptr) && (P_SKILL(P_QUARTERSTAFF) == P_MASTER)) bonus += rnd(15);
	    if (objects[otmp->otyp].oc_skill == P_QUARTERSTAFF && is_undead(ptr) && (P_SKILL(P_QUARTERSTAFF) == P_GRAND_MASTER)) bonus += rnd(25);

	    /* as well as silver bullets */
	    if (otmp->otyp == SILVER_BULLET && is_undead(ptr)) bonus += 8;

	    if (otmp->otyp == SILVER_BULLET && is_undead(ptr) && (P_SKILL(P_FIREARM) == P_SKILLED)) bonus += 8;
	    if (otmp->otyp == SILVER_BULLET && is_undead(ptr) && (P_SKILL(P_FIREARM) == P_EXPERT)) bonus += 16;
	    if (otmp->otyp == SILVER_BULLET && is_undead(ptr) && (P_SKILL(P_FIREARM) == P_MASTER)) bonus += 24;
	    if (otmp->otyp == SILVER_BULLET && is_undead(ptr) && (P_SKILL(P_FIREARM) == P_GRAND_MASTER)) bonus += 32;

	    /* lances versus animals */
	    if (objects[otmp->otyp].oc_skill == P_LANCE && is_animal(ptr)) bonus += rnd(2);

	    if (objects[otmp->otyp].oc_skill == P_LANCE && is_animal(ptr) && (P_SKILL(P_LANCE) == P_SKILLED)) bonus += rnd(2);
	    if (objects[otmp->otyp].oc_skill == P_LANCE && is_animal(ptr) && (P_SKILL(P_LANCE) == P_EXPERT)) bonus += rnd(3);
	    if (objects[otmp->otyp].oc_skill == P_LANCE && is_animal(ptr) && (P_SKILL(P_LANCE) == P_MASTER)) bonus += rnd(5);
	    if (objects[otmp->otyp].oc_skill == P_LANCE && is_animal(ptr) && (P_SKILL(P_LANCE) == P_GRAND_MASTER)) bonus += rnd(6);

	    /* polearms versus golems */
	    if (objects[otmp->otyp].oc_skill == P_POLEARMS && ptr->mlet == S_GOLEM) bonus += rnd(2);

	    if (objects[otmp->otyp].oc_skill == P_POLEARMS && ptr->mlet == S_GOLEM && (P_SKILL(P_POLEARMS) == P_SKILLED)) bonus += rnd(2);
	    if (objects[otmp->otyp].oc_skill == P_POLEARMS && ptr->mlet == S_GOLEM && (P_SKILL(P_POLEARMS) == P_EXPERT)) bonus += rnd(4);
	    if (objects[otmp->otyp].oc_skill == P_POLEARMS && ptr->mlet == S_GOLEM && (P_SKILL(P_POLEARMS) == P_MASTER)) bonus += rnd(6);
	    if (objects[otmp->otyp].oc_skill == P_POLEARMS && ptr->mlet == S_GOLEM && (P_SKILL(P_POLEARMS) == P_GRAND_MASTER)) bonus += rnd(9);

	    /* electric sword versus quantum mechanic */
	    if (otmp->otyp == ELECTRIC_SWORD && ptr->mlet == S_QUANTMECH) bonus += rnd(10);

	    if (otmp->otyp == ELECTRIC_SWORD && ptr->mlet == S_QUANTMECH && (P_SKILL(P_LONG_SWORD) == P_SKILLED)) bonus += rnd(8);
	    if (otmp->otyp == ELECTRIC_SWORD && ptr->mlet == S_QUANTMECH && (P_SKILL(P_LONG_SWORD) == P_EXPERT)) bonus += rnd(16);
	    if (otmp->otyp == ELECTRIC_SWORD && ptr->mlet == S_QUANTMECH && (P_SKILL(P_LONG_SWORD) == P_MASTER)) bonus += rnd(25);
	    if (otmp->otyp == ELECTRIC_SWORD && ptr->mlet == S_QUANTMECH && (P_SKILL(P_LONG_SWORD) == P_GRAND_MASTER)) bonus += rnd(34);

	    /* shotgun versus bears or other quadrupeds */
	    if (otmp->otyp == SHOTGUN_SHELL && ptr->mlet == S_QUADRUPED) bonus += (10 + rnd(10));

	    if (otmp->otyp == SHOTGUN_SHELL && ptr->mlet == S_QUADRUPED && (P_SKILL(P_FIREARM) == P_SKILLED)) bonus += rnd(10);
	    if (otmp->otyp == SHOTGUN_SHELL && ptr->mlet == S_QUADRUPED && (P_SKILL(P_FIREARM) == P_EXPERT)) bonus += rnd(22);
	    if (otmp->otyp == SHOTGUN_SHELL && ptr->mlet == S_QUADRUPED && (P_SKILL(P_FIREARM) == P_MASTER)) bonus += rnd(36);
	    if (otmp->otyp == SHOTGUN_SHELL && ptr->mlet == S_QUADRUPED && (P_SKILL(P_FIREARM) == P_GRAND_MASTER)) bonus += rnd(54);

	    /* axes versus umber hulks */
	    if (objects[otmp->otyp].oc_skill == P_AXE && ptr->mlet == S_UMBER) bonus += rnd(2);

	    if (objects[otmp->otyp].oc_skill == P_AXE && ptr->mlet == S_UMBER && (P_SKILL(P_AXE) == P_SKILLED)) bonus += rnd(2);
	    if (objects[otmp->otyp].oc_skill == P_AXE && ptr->mlet == S_UMBER && (P_SKILL(P_AXE) == P_EXPERT)) bonus += rnd(5);
	    if (objects[otmp->otyp].oc_skill == P_AXE && ptr->mlet == S_UMBER && (P_SKILL(P_AXE) == P_MASTER)) bonus += rnd(10);
	    if (objects[otmp->otyp].oc_skill == P_AXE && ptr->mlet == S_UMBER && (P_SKILL(P_AXE) == P_GRAND_MASTER)) bonus += rnd(17);

	    /* whips for lashing people's asses :P */
	    if (objects[otmp->otyp].oc_skill == P_WHIP && ptr->mlet == S_HUMAN) bonus += 1;

	    if (objects[otmp->otyp].oc_skill == P_WHIP && ptr->mlet == S_HUMAN && (P_SKILL(P_WHIP) == P_SKILLED)) bonus += 1;
	    if (objects[otmp->otyp].oc_skill == P_WHIP && ptr->mlet == S_HUMAN && (P_SKILL(P_WHIP) == P_EXPERT)) bonus += 2;
	    if (objects[otmp->otyp].oc_skill == P_WHIP && ptr->mlet == S_HUMAN && (P_SKILL(P_WHIP) == P_MASTER)) bonus += 3;
	    if (objects[otmp->otyp].oc_skill == P_WHIP && ptr->mlet == S_HUMAN && (P_SKILL(P_WHIP) == P_GRAND_MASTER)) bonus += 4;

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

	if (!rn2(100 - (Luck*2))) { /* nice patch - critical hits --Amy */

		pline("Critical hit!");
		tmp *= 2;

	}

	/*if (tmp > 127) tmp = 127;*/ /* sanity check --Amy ... but I think it's not needed? */

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
	TORPEDO, SPIRIT_THROWER, DWARVISH_SPEAR, SILVER_SPEAR, ELVEN_SPEAR, SPEAR, ORCISH_SPEAR,
	JAVELIN, SHURIKEN, DROVEN_ARROW, YA, SILVER_ARROW, ELVEN_ARROW, DARK_ELVEN_ARROW, 
	ARROW, ORCISH_ARROW, DROVEN_BOLT, CROSSBOW_BOLT, SILVER_DAGGER, ELVEN_DAGGER, 
	DARK_ELVEN_DAGGER, DAGGER, ORCISH_DAGGER, KNIFE, FLINT, ROCK, 
	LOADSTONE, STONE_OF_MAGIC_RESISTANCE, 

	RIGHT_MOUSE_BUTTON_STONE, DISPLAY_LOSS_STONE, SPELL_LOSS_STONE, YELLOW_SPELL_STONE,
	AUTO_DESTRUCT_STONE, MEMORY_LOSS_STONE, INVENTORY_LOSS_STONE, BLACKY_STONE, MENU_BUG_STONE,
	SPEEDBUG_STONE, SUPERSCROLLER_STONE, FREE_HAND_BUG_STONE, UNIDENTIFY_STONE, STONE_OF_THIRST,
	UNLUCKY_STONE, SHADES_OF_GREY_STONE, STONE_OF_FAINTING, STONE_OF_CURSING, STONE_OF_DIFFICULTY,
	DEAFNESS_STONE, ANTIMAGIC_STONE, WEAKNESS_STONE, ROT_THIRTEEN_STONE, BISHOP_STONE,
	CONFUSION_STONE, DROPBUG_STONE, DSTW_STONE, STATUS_STONE,
	ALIGNMENT_STONE, STAIRSTRAP_STONE, UNINFORMATION_STONE,
	FARLOOK_STONE, RESPAWN_STONE, CAPTCHA_STONE,

	STONE_OF_INTRINSIC_LOSS, BLOOD_LOSS_STONE, BAD_EFFECT_STONE, TRAP_CREATION_STONE,
	STONE_OF_VULNERABILITY, ITEM_TELEPORTING_STONE, NASTY_STONE,

	LOADBOULDER, SLEEPSTONE, MANASTONE,
	SALT_CHUNK, TOUCHSTONE, HEALTHSTONE, WHETSTONE, LUCKSTONE, DART,
	/* BOOMERANG, */ CREAM_PIE
	/* note: CREAM_PIE should NOT be #ifdef KOPS */
};

static NEARDATA const int pwep[] =
{	COURSE_LANCE, HALBERD, BARDICHE, SPETUM, BILL_GUISARME, VOULGE, RANSEUR, GUISARME,
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
		    (m_carrying(mtmp, SLING) || m_carrying(mtmp, CATAPULT)) ) {		/* propellor */
		for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
		    if (otmp->oclass == GEM_CLASS /*&&
			    (otmp->otyp != LOADSTONE || !otmp->cursed)*/) {
			/* I'll allow monsters to fire loadstones even if they are cursed.
			 * Yes, monsters are cheating bastards. --Amy */
			propellor = m_carrying(mtmp, CATAPULT);
			if (!propellor) propellor = m_carrying(mtmp, SLING);
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
		  propellor = (oselect(mtmp, DROVEN_BOW));
		  if (!propellor) propellor = (oselect(mtmp, YUMI));
		  if (!propellor) propellor = (oselect(mtmp, ELVEN_BOW));
		  /* WAC added dark elven bow */
		  if (!propellor) propellor = (oselect(mtmp, DARK_ELVEN_BOW));
		  if (!propellor) propellor = (oselect(mtmp, BOW));
		  if (!propellor) propellor = (oselect(mtmp, ORCISH_BOW));
		  break;
		case P_SLING:
		  propellor = (oselect(mtmp, CATAPULT));
		  if (!propellor) propellor = (oselect(mtmp, SLING));
		  break;
		case P_CROSSBOW:
		  propellor = (oselect(mtmp, DEMON_CROSSBOW));
		  if (!propellor) propellor = (oselect(mtmp, DROVEN_CROSSBOW));
		  if (!propellor) propellor = (oselect(mtmp, CROSSBOW));
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
	IMPOSSIBLY_HEAVY_IRON_BALL, IMPOSSIBLY_HEAVY_MINERAL_BALL, IMPOSSIBLY_HEAVY_GLASS_BALL, IMPOSSIBLY_HEAVY_ELYSIUM_BALL, EXTREMELY_HEAVY_IRON_BALL, HEAVY_CONCRETE_BALL, HEAVY_CONUNDRUM_BALL, REALLY_HEAVY_IRON_BALL, HEAVY_ELYSIUM_BALL, HEAVY_GRANITE_BALL, QUITE_HEAVY_IRON_BALL, HEAVY_GOLD_BALL, HEAVY_CLAY_BALL, HEAVY_IRON_BALL, HEAVY_GLASS_BALL, HEAVY_STONE_BALL,
	NUNCHIAKU, SCOURGE, ROTATING_CHAIN, IRON_CHAIN,
	  TSURUGI, STYGIAN_PIKE, RUNESWORD, MALLET, HEAVY_HAMMER, 
	  WOODEN_GETA, LACQUERED_DANCING_SHOE, HIGH_HEELED_SANDAL, SEXY_LEATHER_PUMP, SPIKED_BATTLE_BOOT, TORPEDO,
	  DWARVISH_MATTOCK, BENT_SABLE, 
	  LASER_SWATTER,
#ifdef LIGHTSABERS
	  RED_DOUBLE_LIGHTSABER, RED_LIGHTSABER,
#ifdef D_SABER
	  BLUE_LIGHTSABER,
#if 0
		case VIOLET_LIGHTSABER:
		case WHITE_LIGHTSABER:
		case YELLOW_LIGHTSABER:
#endif
#endif
	  GREEN_LIGHTSABER,
#endif
	  WEDGED_LITTLE_GIRL_SANDAL, SOFT_GIRL_SNEAKER, STURDY_PLATEAU_BOOT_FOR_GIRLS, HUGGING_BOOT, BLOCK_HEELED_COMBAT_BOOT,
	  TWO_HANDED_SWORD, DEVIL_STAR, BATTLE_AXE, GOLDEN_SABER, BATTLE_STAFF, REINFORCED_MACE, 
	  KATANA, UNICORN_HORN, CRYSKNIFE, ELECTRIC_SWORD, TRIDENT, LONG_SWORD, OBSID, SPIRIT_THROWER,
	  ELVEN_BROADSWORD, BROADSWORD, SCIMITAR, SILVER_SABER, FLANGED_MACE, JAGGED_STAR, STEEL_WHIP,
	  SILVER_SHORT_SWORD, SILVER_LONG_SWORD, SILVER_MACE,
  	  MORNING_STAR, DARK_ELVEN_SHORT_SWORD, ELVEN_SHORT_SWORD, 
  	  DWARVISH_SHORT_SWORD, SHORT_SWORD, METAL_CLUB, KNOUT, 
	  ORCISH_SHORT_SWORD, MACE, AXE, DWARVISH_SPEAR, SILVER_SPEAR,
	  ELVEN_SPEAR, SPEAR, ORCISH_SPEAR, FLAIL, BULLWHIP, QUARTERSTAFF, INSECT_SQUASHER, BASEBALL_BAT,
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
		if (
		(otmp->oclass == WEAPON_CLASS || otmp->oclass == BALL_CLASS || otmp->oclass == CHAIN_CLASS)
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
	    if (hwep[i] == CORPSE && !resists_ston(mtmp) && !(mtmp->misc_worn_check & W_ARMG))
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
 * Thanks to a bug with saving and restoring, I allow monsters to wield weapons
 * without wasting a turn. --Amy
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
		    return /*1*/0;
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
		return /*1*/0;
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

#ifdef JEDI
	// for some reason, the lightsaber prototype is created with
	// age == 0
	if (obj->oartifact == ART_LIGHTSABER_PROTOTYPE)
		obj->age = 300L;
#endif
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

	/* [Tom] lowered these a little */        
	if (str < 6) sbon = -1;
	else if (str < 8) sbon = 0;
	else if (str < 17) sbon = 1;
	else if (str <= STR18(50)) sbon = 2;	/* up to 18/50 */
	else if (str < STR18(100)) sbon = 2;
	else if (str == STR18(100)) sbon = 3;  /* 18/00 */
	else if (str == STR19(19)) sbon = 3;  /* 19 */
	else if (str == STR19(20)) sbon = 4;  /* 20 */
	else if (str == STR19(21)) sbon = 4;  /* 21 */
	else if (str == STR19(22)) sbon = 5;  /* 22 */
	else if (str == STR19(23)) sbon = 5;  /* 23 */
	else if (str == STR19(24)) sbon = 6;  /* 24 */
	else sbon = 7;
  
	if (dex < 5) sbon -= 1;
	else if (dex < 7) sbon += 0;
	else if (dex < 10) sbon += 1;
	else if (dex < 12) sbon += 2;
	else if (dex < 15) sbon += 3;
	else if (dex == 15) sbon += 3;  /* 15 */
	else if (dex == 16) sbon += 4;  /* 16 */
	else if (dex == 17) sbon += 4;  /* 17 */
	else if (dex == 18) sbon += 5;  /* 18 */
	else if (dex == 19) sbon += 5;  /* 19 */
	else if (dex == 20) sbon += 6;  /* 20 */
	else if (dex == 21) sbon += 6;  /* 21 */
	else if (dex == 22) sbon += 7;  /* 22 */
	else if (dex == 23) sbon += 7;  /* 23 */
	else if (dex == 24) sbon += 8;  /* 24 */
	else sbon += 9;

/* Game tuning kludge: make it a bit easier for a low level character to hit */
	sbon += (u.ulevel < 3) ? 1 : 0;

/* and harder for high level characters because they often hit automatically anyway */

	if (!Upolyd) {
	if (!rn2(2) && u.ulevel > 10) sbon -= 1;
	if (!rn2(2) && u.ulevel > 13) sbon -= 1;
	if (!rn2(2) && u.ulevel > 16) sbon -= 1;
	if (!rn2(2) && u.ulevel > 19) sbon -= 1;
	if (!rn2(2) && u.ulevel > 22) sbon -= 1;
	if (!rn2(2) && u.ulevel > 24) sbon -= 1;
	if (!rn2(2) && u.ulevel > 26) sbon -= 1;
	if (!rn2(2) && u.ulevel > 28) sbon -= 1;
	}
	if (Upolyd) sbon += (adj_lev(&mons[u.umonnum]) - 3);

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
	if (str < 5) return(-2);
	else if (str < 8) return(-1);
	else if (str < 10) return(0);
	else if (str < 14) return(1);
	else if (str < 18) return(2);
	else if (str == 18) return(3);		/* up to 18 */
	else if (str < STR18(30)) return(4);          /* up to 18/99 */
	else if (str < STR18(66)) return(4);          /* up to 18/99 */
	else if (str < STR18(100)) return(5);          /* up to 18/99 */
	else if (str == STR18(100)) return(6);         /* 18/00 */
	else if (str == STR19(19)) return(7);         /* 19 */
	else if (str == STR19(20)) return(7);         /* 20 */
	else if (str == STR19(21)) return(7);         /* 21 */
	else if (str == STR19(22)) return(8);         /* 22 */
	else if (str == STR19(23)) return(8);         /* 23 */
	else if (str == STR19(24)) return(8);        /* 24 */
	else return(9);
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
                    ptr = "Legendary" ;
            else if (skill <= P_LAST_SPELL)
                    ptr = "Wizard";
	    else if (skill <= P_LAST_H_TO_H) 
	    	    ptr = "Grand Master"; 
	    else ptr = "Unprecedented";
	    break;
	default:	     ptr = "Unknown";	break;
    }
    Strcpy(buf, ptr);
    return buf;
}

/* copy the max skill level name into the given buffer */
STATIC_OVL char *
skill_level_name_max(skill, buf)
int skill;
char *buf;
{
    const char *ptr;

    switch (P_MAX_SKILL(skill)) {
	case P_UNSKILLED:    ptr = "(max Unskilled)"; break;
	case P_BASIC:	     ptr = "(max Basic)";     break;
	case P_SKILLED:	     ptr = "(max Skilled)";   break;
	case P_EXPERT:	     ptr = "(max Expert)";    break;
	case P_MASTER:	     ptr = "(max Master)";    break;
	case P_GRAND_MASTER: 
            if (skill <= P_LAST_WEAPON)
                    ptr = "(max Legendary)" ;
            else if (skill <= P_LAST_SPELL)
                    ptr = "(max Wizard)";
	    else if (skill <= P_LAST_H_TO_H) 
	    	    ptr = "(max Grand Master)"; 
	    else ptr = "(max Unprecedented)";
	    break;
	default:	     ptr = "(max Unknown)";	break;
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
      /*if (tmp >= P_MAX_SKILL(skill)) tmp *=2;*/
      if (tmp < 0) tmp = 0; /* for Restricted skills */
      if (tmp > 1) tmp = 1; /* for basic or higher --Amy*/
  
    /* The more difficult the training, the more slots it takes.
     *	unskilled -> basic	1
     *	basic -> skilled	2
     *	skilled -> expert	3
     */
    /*if (skill <= P_LAST_SPELL)*/
	return tmp; /*just always take one slot, regardless of skill level --Amy*/

      /* Fewer slots used up for unarmed or martial, miscellaneous skills
     *	unskilled -> basic	1
     *	basic -> skilled	1
     *	skilled -> expert	2
     *	expert -> master	2
     *	master -> grand master	3
     */
    /*return (tmp + 1) / 2;*/
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

	if (Role_if(PM_BINDER)) {

		if (P_SKILL(skill) == P_SKILLED) switch (skill) {

		case P_DAGGER:
			    HFire_resistance |= FROMOUTSIDE; pline("Got fire resistance!"); break;
		break;
		case P_KNIFE:
			    HCold_resistance |= FROMOUTSIDE; pline("Got cold resistance!"); break;
		break;
		case P_AXE:
			    HShock_resistance |= FROMOUTSIDE; pline("Got shock resistance!"); break;
		break;
		case P_PICK_AXE:
			    HPoison_resistance |= FROMOUTSIDE; pline("Got poison resistance!"); break;
		break;
		case P_SHORT_SWORD:
			    HCold_resistance |= FROMOUTSIDE; pline("Got cold resistance!"); break;
		break;
		case P_BROAD_SWORD:
			    HInvis |= FROMOUTSIDE; pline("Got invisibility!"); break;
		break;
		case P_LONG_SWORD:
			    HShock_resistance |= FROMOUTSIDE; pline("Got shock resistance!"); break;
		break;
		case P_TWO_HANDED_SWORD:
			    HPoison_resistance |= FROMOUTSIDE; pline("Got poison resistance!"); break;
		break;
		case P_SCIMITAR:
			    HFire_resistance |= FROMOUTSIDE; pline("Got fire resistance!"); break;
		break;
		case P_SABER:
			    HSleep_resistance |= FROMOUTSIDE; pline("Got sleep resistance!"); break;
		break;
		case P_CLUB:
			    HCold_resistance |= FROMOUTSIDE; pline("Got cold resistance!"); break;
		break;
		case P_PADDLE:
			    HSick_resistance |= FROMOUTSIDE; pline("Got sickness resistance!"); break;
		break;
		case P_MACE:
			    HFire_resistance |= FROMOUTSIDE; pline("Got fire resistance!"); break;
		break;
		case P_MORNING_STAR:
			    HPoison_resistance |= FROMOUTSIDE; pline("Got poison resistance!"); break;
		break;
		case P_FLAIL:
			    HDisint_resistance |= FROMOUTSIDE; pline("Got disintegration resistance!"); break;
		break;
		case P_HAMMER:
			    HStealth |= FROMOUTSIDE; pline("Got stealth!"); break;
		break;
		case P_QUARTERSTAFF:
			    HFast |= FROMOUTSIDE; pline("Got speed!"); break;
		break;
		case P_POLEARMS:
			    HShock_resistance |= FROMOUTSIDE; pline("Got shock resistance!"); break;
		break;
		case P_SPEAR:
			    HSleep_resistance |= FROMOUTSIDE; pline("Got sleep resistance!"); break;
		break;
		case P_JAVELIN:
			    HSearching |= FROMOUTSIDE; pline("Got searching!"); break;
		break;
		case P_TRIDENT:
			    HSee_invisible |= FROMOUTSIDE; pline("Got see invisible!"); break;
		break;
		case P_LANCE:
			    HSwimming |= FROMOUTSIDE; pline("Got swimming!"); break;
		break;
		case P_BOW:
			    HPoison_resistance |= FROMOUTSIDE; pline("Got poison resistance!"); break;
		break;
		case P_SLING:
			    HWarning |= FROMOUTSIDE; pline("Got warning!"); break;
		break;
		case P_FIREARM:
			    HDrain_resistance |= FROMOUTSIDE; pline("Got drain resistance!"); break;
		break;
		case P_CROSSBOW:
			    HAcid_resistance |= FROMOUTSIDE; pline("Got acid resistance!"); break;
		break;
		case P_DART:
			    HSearching |= FROMOUTSIDE; pline("Got searching!"); break;
		break;
		case P_SHURIKEN:
			    HSlow_digestion |= FROMOUTSIDE; pline("Got slow digestion!"); break;
		break;
		case P_BOOMERANG:
			    HFast |= FROMOUTSIDE; pline("Got speed!"); break;
		break;
		case P_WHIP:
			    HSee_invisible |= FROMOUTSIDE; pline("Got see invisible!"); break;
		break;
		case P_UNICORN_HORN:
			    HStealth |= FROMOUTSIDE; pline("Got stealth!"); break;
		break;
		case P_LIGHTSABER:
			    HTelepat |= FROMOUTSIDE; pline("Got telepathy!"); break;
		break;
		case P_ATTACK_SPELL:
			    HMagical_breathing |= FROMOUTSIDE; pline("Got unbreathing!"); break;
		break;
		case P_HEALING_SPELL:
			    HSlow_digestion |= FROMOUTSIDE; pline("Got slow digestion!"); break;
		break;
		case P_DIVINATION_SPELL:
			    HSwimming |= FROMOUTSIDE; pline("Got swimming!"); break;
		break;
		case P_ENCHANTMENT_SPELL:
			    HWarning |= FROMOUTSIDE; pline("Got warning!"); break;
		break;
		case P_PROTECTION_SPELL:
			    HPolymorph_control |= FROMOUTSIDE; pline("Got polymorph control!"); break;
		break;
		case P_BODY_SPELL:
			    HFlying |= FROMOUTSIDE; pline("Got flying!"); break;
		break;
		case P_MATTER_SPELL:
			    HTeleport_control |= FROMOUTSIDE; pline("Got teleport control!"); break;
		break;
		case P_BARE_HANDED_COMBAT:
			    HTeleportation |= FROMOUTSIDE; pline("Got teleportitis!"); break;
		break;
		case P_TWO_WEAPON_COMBAT:
			    HRegeneration |= FROMOUTSIDE; pline("Got regeneration!"); break;
		break;
		case P_RIDING:
			    HAcid_resistance |= FROMOUTSIDE; pline("Got acid resistance!"); break;
		break;

		default: break;

		}

		if (P_SKILL(skill) == P_EXPERT) switch (skill) {

		case P_DAGGER:
				if (!tech_known(T_REINFORCE)) {    	learntech(T_REINFORCE, FROMOUTSIDE, 1);
			    	You("learn how to perform reinforce memory!");
				}
		break;
		case P_KNIFE:
				if (!tech_known(T_CUTTHROAT)) {    	learntech(T_CUTTHROAT, FROMOUTSIDE, 1);
			    	You("learn how to perform cutthroat!");
				}
		break;
		case P_AXE:
				if (!tech_known(T_BERSERK)) {    	learntech(T_BERSERK, FROMOUTSIDE, 1);
			    	You("learn how to perform berserk!");
				}
		break;
		case P_PICK_AXE:
				if (!tech_known(T_TINKER)) {    	learntech(T_TINKER, FROMOUTSIDE, 1);
			    	You("learn how to perform tinker!");
				}
		break;
		case P_SHORT_SWORD:
				if (!tech_known(T_WARD_ELEC)) {    	learntech(T_WARD_ELEC, FROMOUTSIDE, 1);
			    	You("learn how to perform ward against electricity!");
				}
		break;
		case P_LONG_SWORD:
				if (!tech_known(T_RESEARCH)) {    	learntech(T_RESEARCH, FROMOUTSIDE, 1);
			    	You("learn how to perform research!");
				}
		break;
		case P_BROAD_SWORD:
				if (!tech_known(T_BOOZE)) {    	learntech(T_BOOZE, FROMOUTSIDE, 1);
			    	You("learn how to perform booze!");
				}
		break;
		case P_TWO_HANDED_SWORD:
				if (!tech_known(T_HEAL_HANDS)) {    	learntech(T_HEAL_HANDS, FROMOUTSIDE, 1);
			    	You("learn how to perform healing hands!");
				}
		break;
		case P_SCIMITAR:
				if (!tech_known(T_DRAW_BLOOD)) {    	learntech(T_DRAW_BLOOD, FROMOUTSIDE, 1);
			    	You("learn how to perform draw blood!");
				}
		break;
		case P_SABER:
				if (!tech_known(T_KIII)) {    	learntech(T_KIII, FROMOUTSIDE, 1);
			    	You("learn how to perform kiii!");
				}
		break;
		case P_CLUB:
				if (!tech_known(T_EVISCERATE)) {    	learntech(T_EVISCERATE, FROMOUTSIDE, 1);
			    	You("learn how to perform eviscerate!");
				}
		break;
		case P_PADDLE:
				if (!tech_known(T_RAISE_ZOMBIES)) {    	learntech(T_RAISE_ZOMBIES, FROMOUTSIDE, 1);
			    	You("learn how to perform raise zombies!");
				}
		break;
		case P_MACE:
				if (!tech_known(T_WARD_COLD)) {    	learntech(T_WARD_COLD, FROMOUTSIDE, 1);
			    	You("learn how to perform ward against cold!");
				}
		break;
		case P_MORNING_STAR:
				if (!tech_known(T_TELEKINESIS)) {    	learntech(T_TELEKINESIS, FROMOUTSIDE, 1);
			    	You("learn how to perform telekinesis!");
				}
		break;
		case P_FLAIL:
				if (!tech_known(T_BLINK)) {    	learntech(T_BLINK, FROMOUTSIDE, 1);
			    	You("learn how to perform blink!");
				}
		break;
		case P_HAMMER:
				if (!tech_known(T_LIQUID_LEAP)) {    	learntech(T_LIQUID_LEAP, FROMOUTSIDE, 1);
			    	You("learn how to perform liquid leap!");
				}
		break;
		case P_QUARTERSTAFF:
				if (!tech_known(T_CRIT_STRIKE)) {    	learntech(T_CRIT_STRIKE, FROMOUTSIDE, 1);
			    	You("learn how to perform critical strike!");
				}
		break;
		case P_POLEARMS:
				if (!tech_known(T_TURN_UNDEAD)) {    	learntech(T_TURN_UNDEAD, FROMOUTSIDE, 1);
			    	You("learn how to perform turn undead!");
				}
		break;
		case P_SPEAR:
				if (!tech_known(T_PRACTICE)) {    	learntech(T_PRACTICE, FROMOUTSIDE, 1);
			    	You("learn how to perform weapon practice!");
				}
		break;
		case P_JAVELIN:
				if (!tech_known(T_DRAW_ENERGY)) {    	learntech(T_DRAW_ENERGY, FROMOUTSIDE, 1);
			    	You("learn how to perform draw energy!");
				}
		break;
		case P_TRIDENT:
				if (!tech_known(T_POWER_SURGE)) {    	learntech(T_POWER_SURGE, FROMOUTSIDE, 1);
			    	You("learn how to perform power surge!");
				}
		break;
		case P_LANCE:
				if (!tech_known(T_REVIVE)) {    	learntech(T_REVIVE, FROMOUTSIDE, 1);
			    	You("learn how to perform revivification!");
				}
		break;
		case P_BOW:
				if (!tech_known(T_FLURRY)) {    	learntech(T_FLURRY, FROMOUTSIDE, 1);
			    	You("learn how to perform missile flurry!");
				}
		break;
		case P_SLING:
				if (!tech_known(T_VANISH)) {    	learntech(T_VANISH, FROMOUTSIDE, 1);
			    	You("learn how to perform vanish!");
				}
		break;
		case P_FIREARM:
				if (!tech_known(T_CREATE_AMMO)) {    	learntech(T_CREATE_AMMO, FROMOUTSIDE, 1);
			    	You("learn how to perform create ammo!");
				}
		break;
		case P_CROSSBOW:
				if (!tech_known(T_SIGIL_TEMPEST)) {    	learntech(T_SIGIL_TEMPEST, FROMOUTSIDE, 1);
			    	You("learn how to perform sigil of tempest!");
				}
		break;
		case P_DART:
				if (!tech_known(T_WARD_FIRE)) {    	learntech(T_WARD_FIRE, FROMOUTSIDE, 1);
			    	You("learn how to perform ward against fire!");
				}
		break;
		case P_SHURIKEN:
				if (!tech_known(T_SIGIL_CONTROL)) {    	learntech(T_SIGIL_CONTROL, FROMOUTSIDE, 1);
			    	You("learn how to perform sigil of control!");
				}
		break;
		case P_BOOMERANG:
				if (!tech_known(T_SIGIL_DISCHARGE)) {    	learntech(T_SIGIL_DISCHARGE, FROMOUTSIDE, 1);
			    	You("learn how to perform sigil of discharge!");
				}
		break;
		case P_WHIP:
				if (!tech_known(T_POKE_BALL)) {    	learntech(T_POKE_BALL, FROMOUTSIDE, 1);
			    	You("learn how to perform poke ball!");
				}
		break;
		case P_UNICORN_HORN:
				if (!tech_known(T_PRIMAL_ROAR)) {    	learntech(T_PRIMAL_ROAR, FROMOUTSIDE, 1);
			    	You("learn how to perform primal roar!");
				}
		break;
		case P_LIGHTSABER:
				if (!tech_known(T_CHARGE_SABER)) {    	learntech(T_CHARGE_SABER, FROMOUTSIDE, 1);
			    	You("learn how to perform charge saber!");
				}
		break;
		case P_ATTACK_SPELL:
				if (!tech_known(T_EGG_BOMB)) {    	learntech(T_EGG_BOMB, FROMOUTSIDE, 1);
			    	You("learn how to perform egg bomb!");
				}
		break;
		case P_HEALING_SPELL:
				if (!tech_known(T_SURGERY)) {    	learntech(T_SURGERY, FROMOUTSIDE, 1);
			    	You("learn how to perform surgery!");
				}
		break;
		case P_DIVINATION_SPELL:
				if (!tech_known(T_ATTIRE_CHARM)) {    	learntech(T_ATTIRE_CHARM, FROMOUTSIDE, 1);
			    	You("learn how to perform attire charm!");
				}
		break;
		case P_ENCHANTMENT_SPELL:
				if (!tech_known(T_BLESSING)) {    	learntech(T_BLESSING, FROMOUTSIDE, 1);
			    	You("learn how to perform blessing!");
				}
		break;
		case P_PROTECTION_SPELL:
				if (!tech_known(T_SUMMON_TEAM_ANT)) {    	learntech(T_SUMMON_TEAM_ANT, FROMOUTSIDE, 1);
			    	You("learn how to perform summon team ant!");
				}
		break;
		case P_BODY_SPELL:
				if (!tech_known(T_RAGE)) {    	learntech(T_RAGE, FROMOUTSIDE, 1);
			    	You("learn how to perform rage eruption!");
				}
		break;
		case P_MATTER_SPELL:
				if (!tech_known(T_WORLD_FALL)) {    	learntech(T_WORLD_FALL, FROMOUTSIDE, 1);
			    	You("learn how to perform world fall!");
				}
		break;
		case P_BARE_HANDED_COMBAT:
				if (!tech_known(T_DAZZLE)) {    	learntech(T_DAZZLE, FROMOUTSIDE, 1);
			    	You("learn how to perform dazzle!");
				}
		break;
		case P_TWO_WEAPON_COMBAT:
				if (!tech_known(T_JEDI_JUMP)) {    	learntech(T_JEDI_JUMP, FROMOUTSIDE, 1);
			    	You("learn how to perform jedi jump!");
				}
		break;
		case P_RIDING:
				if (!tech_known(T_CALM_STEED)) {    	learntech(T_CALM_STEED, FROMOUTSIDE, 1);
			    	You("learn how to perform calm steed!");
				}
		break;

		default: break;

		}

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
    char buf[BUFSZ], sklnambuf[BUFSZ], sklnambuftwo[BUFSZ];
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
		(void) skill_level_name_max(i, sklnambuftwo); /* show maximum to be more user friendly --Amy */
#ifdef WIZARD
		if (wizard) {
		    if (!iflags.menu_tab_sep)
			Sprintf(buf, " %s%-*s %-12s %-12s %4d(%4d)",
			    prefix, longest, P_NAME(i), sklnambuf, sklnambuftwo,
			    P_ADVANCE(i),
			    practice_needed_to_advance(P_SKILL(i), i));
		    else
			Sprintf(buf, " %s%s\t%s\t%s\t%5d(%4d)",
			    prefix, P_NAME(i), sklnambuf, sklnambuftwo,
			    P_ADVANCE(i),
			    practice_needed_to_advance(P_SKILL(i), i));
		 } else
#endif
		{
		    if (!iflags.menu_tab_sep)
			Sprintf(buf, " %s %-*s %s %s",
			    prefix, longest, P_NAME(i), sklnambuf, sklnambuftwo);
		    else
			Sprintf(buf, " %s%s\t%s\t%s",
			    prefix, P_NAME(i), sklnambuf, sklnambuftwo);
		}
		any.a_int = can_advance(i, speedy) ? i+1 : 0;
		add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE,
			 buf, MENU_UNSELECTED);
	    }

	    Strcpy(buf, (to_advance > 0) ? "Pick a skill to advance:" :
					   "Current skills:");
/*#ifdef WIZARD*/
/*	    if (wizard && !speedy)*/
		Sprintf(eos(buf), "  (%d slot%s available)",
			u.weapon_slots, plur(u.weapon_slots));
/*#endif*/
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
#ifdef CONVICT
    if ( ((obj->oclass == BALL_CLASS) || (obj->oclass == CHAIN_CLASS)) /*&& Role_if(PM_CONVICT)*/)
        return objects[obj->otyp].oc_skill;
#endif /* CONVICT */
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
		    case P_EXPERT:      bonus =  1 + rnd(2) ; break;
		    case P_MASTER:	bonus =  2 + rnd(3); break;
		    case P_GRAND_MASTER:	bonus =  3 + rnd(4); break;
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
		    case P_EXPERT:      bonus += 2; break;
		    case P_MASTER:	bonus += 4; break;
		    case P_GRAND_MASTER:	bonus += 6; break;
		}
		if (type == P_LANCE) bonus++;
	}
#endif
#ifdef JEDI
	/* Jedi are trained in lightsabers, no to-hit penalty for them */
	if (Role_if(PM_JEDI) && is_lightsaber(weapon))
		bonus-=objects[weapon->otyp].oc_hitbon;
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
		    case P_BASIC:       bonus += 1; break;
		    case P_SKILLED:     bonus += 3; break;
		    case P_EXPERT:      bonus += 5; break;
		    case P_MASTER:      bonus += 7; break;
		    case P_GRAND_MASTER:      bonus += 10; break;
		}
	}
#endif

#ifdef JEDI
	/* Jedi are simply better */
	if (Role_if(PM_JEDI) && weapon && is_lightsaber(weapon)){
		switch (P_SKILL(type)){
			case P_GRAND_MASTER: bonus +=10; break; /* fall through removed by Amy */
			case P_MASTER: bonus +=7; break; /* fall through removed by Amy */
			case P_EXPERT: bonus +=4; break; /* fall through removed by Amy */
			case P_SKILLED: bonus +=2; break;
			case P_BASIC: bonus += 1; break;
			case P_UNSKILLED: break;
			case P_ISRESTRICTED: break;
			default: impossible("unknown lightsaber skill for a jedi"); break;
		}
	}
#endif

	/* Ogres are supposed to have a use for that weak starting club of theirs after all --Amy */
	if (Race_if(PM_OGRO) && weapon && weapon_type(weapon) == P_CLUB){

		bonus += 2;
		if (u.ulevel >= 15) bonus += 1;
		if (u.ulevel >= 30) bonus += 1;
	}

	/* Navi are highly proficient with spears --Amy */
	if (Race_if(PM_NAVI) && weapon && weapon_type(weapon) == P_SPEAR){

		bonus += 3;
		if (u.ulevel >= 15) bonus += 1;
		if (u.ulevel >= 30) bonus += 1;
	}

	/* rubber hoses for jesters */
	if (Role_if(PM_JESTER) && weapon && weapon->otyp == RUBBER_HOSE){

		bonus += 3;
		if (u.ulevel >= 15) bonus += 2;
		if (u.ulevel >= 30) bonus += 2;
	}
	/* Transvestites can whack enemies using heels --Amy */
	if (Role_if(PM_TRANSVESTITE) && weapon && weapon_type(weapon) == P_HAMMER){

		bonus += 2;
		if (u.ulevel >= 15) bonus += 1;
		if (u.ulevel >= 30) bonus += 1;
	}

	/* boomerang damage bonus for Batman */
	if (Race_if(PM_BATMAN) && weapon && weapon_type(weapon) == P_BOOMERANG){

		bonus += 2;
		if (u.ulevel >= 15) bonus += 2;
		if (u.ulevel >= 30) bonus += 2;
	}

	/* more boomerang damage bonus */

	if (weapon && weapon_type(weapon) == P_BOOMERANG && (P_SKILL(P_BOOMERANG) == P_SKILLED) ) bonus += rnd(4);
	if (weapon && weapon_type(weapon) == P_BOOMERANG && (P_SKILL(P_BOOMERANG) == P_EXPERT) ) bonus += rnd(10);
	if (weapon && weapon_type(weapon) == P_BOOMERANG && (P_SKILL(P_BOOMERANG) == P_MASTER) ) bonus += rnd(20);
	if (weapon && weapon_type(weapon) == P_BOOMERANG && (P_SKILL(P_BOOMERANG) == P_GRAND_MASTER) ) bonus += rnd(40);

	/* add a little damage bonus for higher-level characters so the stronger monsters aren't too overpowered --Amy */

	if (u.ulevel >= 10) bonus += 1;
	if (u.ulevel >= 20) bonus += 1;
	if (u.ulevel >= 30) bonus += 1;

	/* damage bonus for using racial equipment */

	if ((Race_if(PM_ELF) || Role_if(PM_ELPH)) && weapon && (weapon->otyp == ELVEN_DAGGER || weapon->otyp == ELVEN_BOW || weapon->otyp == ELVEN_ARROW || weapon->otyp == ELVEN_SPEAR || weapon->otyp == ELVEN_SHORT_SWORD || weapon->otyp == ELVEN_BROADSWORD) ) bonus += 1;
	if (Race_if(PM_DROW) && weapon && (weapon->otyp == DARK_ELVEN_DAGGER || weapon->otyp == DROVEN_CROSSBOW || weapon->otyp == DROVEN_BOLT || weapon->otyp == DROVEN_BOW || weapon->otyp == DROVEN_ARROW || weapon->otyp == DARK_ELVEN_ARROW || weapon->otyp == DARK_ELVEN_BOW || weapon->otyp == DARK_ELVEN_SHORT_SWORD) ) bonus += 1;
	if (Race_if(PM_ORC) && weapon && (weapon->otyp == ORCISH_DAGGER || weapon->otyp == ORCISH_BOW || weapon->otyp == ORCISH_ARROW || weapon->otyp == ORCISH_SPEAR || weapon->otyp == ORCISH_SHORT_SWORD ) ) bonus += 1;
	if (Race_if(PM_DWARF) && weapon && (weapon->otyp == DWARVISH_SHORT_SWORD || weapon->otyp == DWARVISH_SPEAR) ) bonus += 1;
	if (Race_if(PM_GNOME) && weapon && (weapon->otyp == AKLYS || weapon->otyp == CROSSBOW || weapon->otyp == CROSSBOW_BOLT) ) bonus += 1;
	if (Race_if(PM_HOBBIT) && weapon && (weapon->otyp == SLING || weapon_type(weapon) == -P_SLING) ) bonus += 1;
	if (Race_if(PM_HOBBIT) && weapon && (weapon->otyp == CATAPULT || weapon_type(weapon) == -P_SLING) ) bonus += 2;

    return bonus;
}

/*
 * Return damage bonus for using melee weapons. Added by Amy to make the later parts of the game less unplayable.
 */
int
melee_dam_bonus(weapon)
struct obj *weapon;

{
    int type, wep_type, bonus = 0;

    if (!weapon) return 0;

    wep_type = weapon_type(weapon);

    type = wep_type;
    if (type == P_NONE) {
	bonus = 0;
    } else if (!u.twoweap && type <= P_LAST_WEAPON) {	/* bonus for highly skilled, non-dual-wielded weapon */
	switch (P_SKILL(type)) {
	    case P_EXPERT:	bonus =  1; break;
	    case P_MASTER:	bonus =  2; break;
	    case P_GRAND_MASTER:bonus =  3; break;
	    default: bonus = 0; break;
	}
    }

	if (weapon && weapon_type(weapon) == P_DAGGER && (P_SKILL(P_DAGGER) == P_SKILLED) ) bonus += 1;
	if (weapon && weapon_type(weapon) == P_DAGGER && (P_SKILL(P_DAGGER) == P_EXPERT) ) bonus += rnd(2);
	if (weapon && weapon_type(weapon) == P_DAGGER && (P_SKILL(P_DAGGER) == P_MASTER) ) bonus += rnd(4);
	if (weapon && weapon_type(weapon) == P_DAGGER && (P_SKILL(P_DAGGER) == P_GRAND_MASTER) ) bonus += rnd(6);

	if (weapon && weapon->otyp == GREAT_DAGGER && (P_SKILL(P_DAGGER) == P_SKILLED) ) bonus += rnd(2);
	if (weapon && weapon->otyp == GREAT_DAGGER && (P_SKILL(P_DAGGER) == P_EXPERT) ) bonus += rnd(4);
	if (weapon && weapon->otyp == GREAT_DAGGER && (P_SKILL(P_DAGGER) == P_MASTER) ) bonus += rnd(8);
	if (weapon && weapon->otyp == GREAT_DAGGER && (P_SKILL(P_DAGGER) == P_GRAND_MASTER) ) bonus += rnd(14);

	if (weapon && weapon->otyp == WOODEN_STAKE && (P_SKILL(P_DAGGER) == P_SKILLED) ) bonus += 1;
	if (weapon && weapon->otyp == WOODEN_STAKE && (P_SKILL(P_DAGGER) == P_EXPERT) ) bonus += rnd(2);
	if (weapon && weapon->otyp == WOODEN_STAKE && (P_SKILL(P_DAGGER) == P_MASTER) ) bonus += rnd(4);
	if (weapon && weapon->otyp == WOODEN_STAKE && (P_SKILL(P_DAGGER) == P_GRAND_MASTER) ) bonus += rnd(8);

	if (weapon && weapon_type(weapon) == P_KNIFE && (P_SKILL(P_KNIFE) == P_SKILLED) ) bonus += 1;
	if (weapon && weapon_type(weapon) == P_KNIFE && (P_SKILL(P_KNIFE) == P_EXPERT) ) bonus += rnd(3);
	if (weapon && weapon_type(weapon) == P_KNIFE && (P_SKILL(P_KNIFE) == P_MASTER) ) bonus += rnd(5);
	if (weapon && weapon_type(weapon) == P_KNIFE && (P_SKILL(P_KNIFE) == P_GRAND_MASTER) ) bonus += rnd(9);

	if (weapon && weapon->otyp == BATTLE_AXE && (P_SKILL(P_AXE) == P_SKILLED) ) bonus += rnd(8);
	if (weapon && weapon->otyp == BATTLE_AXE && (P_SKILL(P_AXE) == P_EXPERT) ) bonus += rnd(16);
	if (weapon && weapon->otyp == BATTLE_AXE && (P_SKILL(P_AXE) == P_MASTER) ) bonus += rnd(26);
	if (weapon && weapon->otyp == BATTLE_AXE && (P_SKILL(P_AXE) == P_GRAND_MASTER) ) bonus += rnd(38);

	if (weapon && weapon->otyp == DWARVISH_MATTOCK && (P_SKILL(P_PICK_AXE) == P_SKILLED) ) bonus += rnd(8);
	if (weapon && weapon->otyp == DWARVISH_MATTOCK && (P_SKILL(P_PICK_AXE) == P_EXPERT) ) bonus += rnd(14);
	if (weapon && weapon->otyp == DWARVISH_MATTOCK && (P_SKILL(P_PICK_AXE) == P_MASTER) ) bonus += rnd(20);
	if (weapon && weapon->otyp == DWARVISH_MATTOCK && (P_SKILL(P_PICK_AXE) == P_GRAND_MASTER) ) bonus += rnd(30);

	if (weapon && weapon->otyp == PICK_AXE && (P_SKILL(P_PICK_AXE) == P_SKILLED) ) bonus += rnd(4);
	if (weapon && weapon->otyp == PICK_AXE && (P_SKILL(P_PICK_AXE) == P_EXPERT) ) bonus += rnd(8);
	if (weapon && weapon->otyp == PICK_AXE && (P_SKILL(P_PICK_AXE) == P_MASTER) ) bonus += rnd(12);
	if (weapon && weapon->otyp == PICK_AXE && (P_SKILL(P_PICK_AXE) == P_GRAND_MASTER) ) bonus += rnd(18);

	if (weapon && weapon->otyp == TSURUGI && (P_SKILL(P_TWO_HANDED_SWORD) == P_SKILLED) ) bonus += rnd(8);
	if (weapon && weapon->otyp == TSURUGI && (P_SKILL(P_TWO_HANDED_SWORD) == P_EXPERT) ) bonus += rnd(16);
	if (weapon && weapon->otyp == TSURUGI && (P_SKILL(P_TWO_HANDED_SWORD) == P_MASTER) ) bonus += rnd(24);
	if (weapon && weapon->otyp == TSURUGI && (P_SKILL(P_TWO_HANDED_SWORD) == P_GRAND_MASTER) ) bonus += rnd(34);

	if (weapon && weapon->otyp == TWO_HANDED_SWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_SKILLED) ) bonus += rnd(8);
	if (weapon && weapon->otyp == TWO_HANDED_SWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_EXPERT) ) bonus += rnd(16);
	if (weapon && weapon->otyp == TWO_HANDED_SWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_MASTER) ) bonus += rnd(24);
	if (weapon && weapon->otyp == TWO_HANDED_SWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_GRAND_MASTER) ) bonus += rnd(34);

	if (weapon && weapon->otyp == GOLDEN_SABER && (P_SKILL(P_SABER) == P_SKILLED) ) bonus += rnd(2);
	if (weapon && weapon->otyp == GOLDEN_SABER && (P_SKILL(P_SABER) == P_EXPERT) ) bonus += rnd(4);
	if (weapon && weapon->otyp == GOLDEN_SABER && (P_SKILL(P_SABER) == P_MASTER) ) bonus += rnd(7);
	if (weapon && weapon->otyp == GOLDEN_SABER && (P_SKILL(P_SABER) == P_GRAND_MASTER) ) bonus += rnd(10);

	if (weapon && weapon->otyp == METAL_CLUB && (P_SKILL(P_CLUB) == P_SKILLED) ) bonus += rnd(2);
	if (weapon && weapon->otyp == METAL_CLUB && (P_SKILL(P_CLUB) == P_EXPERT) ) bonus += rnd(4);
	if (weapon && weapon->otyp == METAL_CLUB && (P_SKILL(P_CLUB) == P_MASTER) ) bonus += rnd(7);
	if (weapon && weapon->otyp == METAL_CLUB && (P_SKILL(P_CLUB) == P_GRAND_MASTER) ) bonus += rnd(10);

	if (weapon && weapon->otyp == BATTLE_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_SKILLED) ) bonus += rnd(6);
	if (weapon && weapon->otyp == BATTLE_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_EXPERT) ) bonus += rnd(12);
	if (weapon && weapon->otyp == BATTLE_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_MASTER) ) bonus += rnd(20);
	if (weapon && weapon->otyp == BATTLE_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_GRAND_MASTER) ) bonus += rnd(30);

	if (weapon && weapon_type(weapon) == P_POLEARMS && (P_SKILL(P_POLEARMS) == P_SKILLED) ) bonus += 1;
	if (weapon && weapon_type(weapon) == P_POLEARMS && (P_SKILL(P_POLEARMS) == P_EXPERT) ) bonus += rnd(2);
	if (weapon && weapon_type(weapon) == P_POLEARMS && (P_SKILL(P_POLEARMS) == P_MASTER) ) bonus += rnd(4);
	if (weapon && weapon_type(weapon) == P_POLEARMS && (P_SKILL(P_POLEARMS) == P_GRAND_MASTER) ) bonus += rnd(6);

	if (weapon && weapon_type(weapon) == P_TRIDENT && (P_SKILL(P_TRIDENT) == P_SKILLED) ) bonus += 1;
	if (weapon && weapon_type(weapon) == P_TRIDENT && (P_SKILL(P_TRIDENT) == P_EXPERT) ) bonus += rnd(2);
	if (weapon && weapon_type(weapon) == P_TRIDENT && (P_SKILL(P_TRIDENT) == P_MASTER) ) bonus += rnd(4);
	if (weapon && weapon_type(weapon) == P_TRIDENT && (P_SKILL(P_TRIDENT) == P_GRAND_MASTER) ) bonus += rnd(6);

	if (weapon && weapon_type(weapon) == P_LANCE && (P_SKILL(P_LANCE) == P_SKILLED) ) bonus += 1;
	if (weapon && weapon_type(weapon) == P_LANCE && (P_SKILL(P_LANCE) == P_EXPERT) ) bonus += rnd(3);
	if (weapon && weapon_type(weapon) == P_LANCE && (P_SKILL(P_LANCE) == P_MASTER) ) bonus += rnd(5);
	if (weapon && weapon_type(weapon) == P_LANCE && (P_SKILL(P_LANCE) == P_GRAND_MASTER) ) bonus += rnd(9);

	if (weapon && weapon_type(weapon) == P_WHIP && (P_SKILL(P_WHIP) == P_SKILLED) ) bonus += rnd(2);
	if (weapon && weapon_type(weapon) == P_WHIP && (P_SKILL(P_WHIP) == P_EXPERT) ) bonus += rnd(4);
	if (weapon && weapon_type(weapon) == P_WHIP && (P_SKILL(P_WHIP) == P_MASTER) ) bonus += rnd(7);
	if (weapon && weapon_type(weapon) == P_WHIP && (P_SKILL(P_WHIP) == P_GRAND_MASTER) ) bonus += rnd(10);

    return bonus;
}

/*
 * Return damage bonus for using ranged weapons. Added by Amy to make the later parts of the game less unplayable.
 */
int
ranged_dam_bonus(weapon)
struct obj *weapon;

{
    int bonus = 0;

    if (!weapon) return 0;

	if (weapon && weapon->otyp == GREAT_DAGGER && (P_SKILL(P_DAGGER) == P_EXPERT) ) bonus += rnd(2);
	if (weapon && weapon->otyp == GREAT_DAGGER && (P_SKILL(P_DAGGER) == P_MASTER) ) bonus += rnd(6);
	if (weapon && weapon->otyp == GREAT_DAGGER && (P_SKILL(P_DAGGER) == P_GRAND_MASTER) ) bonus += rnd(12);

	/* crysknife reverts when thrown, so give lots of extra damage if you still decide to throw it */
	if (weapon && weapon->otyp == CRYSKNIFE && (P_SKILL(P_KNIFE) == P_SKILLED) ) bonus += rnd(16);
	if (weapon && weapon->otyp == CRYSKNIFE && (P_SKILL(P_KNIFE) == P_EXPERT) ) bonus += rnd(30);
	if (weapon && weapon->otyp == CRYSKNIFE && (P_SKILL(P_KNIFE) == P_MASTER) ) bonus += rnd(50);
	if (weapon && weapon->otyp == CRYSKNIFE && (P_SKILL(P_KNIFE) == P_GRAND_MASTER) ) bonus += rnd(80);

	if (weapon && weapon->otyp == SPIRIT_THROWER && (P_SKILL(P_JAVELIN) == P_SKILLED) ) bonus += rnd(8);
	if (weapon && weapon->otyp == SPIRIT_THROWER && (P_SKILL(P_JAVELIN) == P_EXPERT) ) bonus += rnd(14);
	if (weapon && weapon->otyp == SPIRIT_THROWER && (P_SKILL(P_JAVELIN) == P_MASTER) ) bonus += rnd(20);
	if (weapon && weapon->otyp == SPIRIT_THROWER && (P_SKILL(P_JAVELIN) == P_GRAND_MASTER) ) bonus += rnd(30);

	if (weapon && weapon->otyp == TORPEDO && (P_SKILL(P_JAVELIN) == P_SKILLED) ) bonus += rnd(14);
	if (weapon && weapon->otyp == TORPEDO && (P_SKILL(P_JAVELIN) == P_EXPERT) ) bonus += rnd(24);
	if (weapon && weapon->otyp == TORPEDO && (P_SKILL(P_JAVELIN) == P_MASTER) ) bonus += rnd(36);
	if (weapon && weapon->otyp == TORPEDO && (P_SKILL(P_JAVELIN) == P_GRAND_MASTER) ) bonus += rnd(50);

	if (weapon && weapon->otyp == SHURIKEN && (P_SKILL(P_SHURIKEN) == P_MASTER) ) bonus += 1;
	if (weapon && weapon->otyp == SHURIKEN && (P_SKILL(P_SHURIKEN) == P_GRAND_MASTER) ) bonus += 2;

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

	if (isamerican) return; /* all skills start out restricted --Amy */

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
	    if ((P_MAX_SKILL(skill) > P_EXPERT) && !Race_if(PM_BASTARD) && !isamerican && !Role_if(PM_BINDER) && !Role_if(PM_POLITICIAN) && !Role_if(PM_MURDERER) ) P_SKILL(skill) = P_BASIC;

		if ((P_MAX_SKILL(skill) >= P_UNSKILLED) && !Race_if(PM_BASTARD) && !isamerican && !Role_if(PM_BINDER) && !Role_if(PM_POLITICIAN) && !Role_if(PM_MURDERER) && Race_if(PM_TUMBLRER) ) P_SKILL(skill) = P_MAX_SKILL(skill);

	}

	/* Set skill for all objects in inventory to be basic */
	if(!Role_if(PM_POLITICIAN) && !isamerican && !Role_if(PM_MURDERER)) for (obj = invent; obj; obj = obj->nobj) {
	    skill = get_obj_skill(obj);
	    if (skill != P_NONE) {
		if (!Role_if(PM_BINDER) && !Race_if(PM_BASTARD) ) P_SKILL(skill) = P_BASIC;
		else P_SKILL(skill) = P_UNSKILLED;
		/* KMH -- If you came into the dungeon with it, you should at least be skilled */
		if (P_MAX_SKILL(skill) < P_EXPERT) { /* edit by Amy: let's make it expert. */
			if (wizard) pline("Warning: %s should be at least expert.  Fixing...", P_NAME(skill));
			P_MAX_SKILL(skill) = P_EXPERT;
		}
		if (!Role_if(PM_BINDER) && !Race_if(PM_BASTARD) && Race_if(PM_TUMBLRER)) P_SKILL(skill) = P_MAX_SKILL(skill);
	    }
	}

	/* Batman obviously has legendary boomerang abilities --Amy */
	if (Race_if(PM_BATMAN)) {
		if (P_RESTRICTED(P_BOOMERANG)) {	
			P_SKILL(P_BOOMERANG) = P_UNSKILLED;
			P_ADVANCE(P_BOOMERANG) = 0;
		}
		P_MAX_SKILL(P_BOOMERANG) = P_GRAND_MASTER;
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

void
xtraskillinit()
{

	struct obj *obj;
	int skill;

	/* Set skill for all objects in inventory to be basic */
	if (!Role_if(PM_POLITICIAN) && !isamerican && !Role_if(PM_MURDERER) ) for (obj = invent; obj; obj = obj->nobj) {
	    skill = get_obj_skill(obj);
	    if (skill != P_NONE) {
		if(!Role_if(PM_BINDER) && !Race_if(PM_BASTARD) ) P_SKILL(skill) = P_BASIC;
		else P_SKILL(skill) = P_UNSKILLED;
		/* KMH -- If you came into the dungeon with it, you should at least be skilled */
		if (P_MAX_SKILL(skill) < P_EXPERT) { /* edit by Amy: let's make it expert. */
			P_MAX_SKILL(skill) = P_EXPERT;
		}
		if (!Role_if(PM_BINDER) && !Race_if(PM_BASTARD) && Race_if(PM_TUMBLRER)) P_SKILL(skill) = P_MAX_SKILL(skill);
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
