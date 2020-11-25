/*	SCCS Id: @(#)weapon.c	3.4	2002/11/07	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/*
 *	This module contains code for calculation of "to hit" and damage
 *	bonuses for any given weapon used, as well as weapons selection
 *	code for monsters.
 */
#include "hack.h"

#ifdef DUMP_LOG
STATIC_DCL int enhance_skill(boolean);
#endif

static void give_may_advance_msg(int);
STATIC_PTR int practice(void);
/*static int get_obj_skill(struct obj *);*/

static void mon_ignite_lightsaber(struct obj *, struct monst *);

/*WAC practicing needs a delay counter*/
static NEARDATA schar delay;            /* moves left for practice */
static NEARDATA boolean speed_advance = FALSE;

STATIC_DCL void give_may_advance_msg(int);

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

	if (!u.howtoenhanceskills) {
		u.howtoenhanceskills = TRUE;
		pline("(Remember to use the #enhance command to actually spend your skill points; this does not happen automatically.)");
	}
}

STATIC_DCL boolean can_advance(int, BOOLEAN_P);
STATIC_DCL boolean could_advance(int);
STATIC_DCL boolean peaked_skill(int);
STATIC_DCL int slots_required(int);
STATIC_DCL boolean can_practice(int); /* WAC for Practicing */

#ifdef OVL1

STATIC_DCL char *skill_level_name(int,char *);
STATIC_DCL char *skill_level_name_max(int,char *);
STATIC_DCL void skill_advance(int);

#endif	/* OVL1 */

#ifdef OVLB

static NEARDATA const char kebabable[] = {
	S_XORN, S_DRAGON, S_JABBERWOCK, S_NAGA, S_GIANT, '\0'
};

const char *
wpskillname(skill)
int skill;
{
	skill = abs(skill); /* for darts and stuff --Amy */

	switch (skill) {

		case P_DAGGER:
			return "dagger";
		case P_KNIFE:
			return "knife";
		case P_AXE:
			return "axe";
		case P_PICK_AXE:
			return "pick-axe";
		case P_SHORT_SWORD:
			return "short sword";
		case P_BROAD_SWORD:
			return "broadsword";
		case P_LONG_SWORD:
			return "long sword";
		case P_TWO_HANDED_SWORD:
			return "two-handed sword";
		case P_SCIMITAR:
			return "scimitar";
		case P_SABER:
			return "saber";
		case P_CLUB:
			return "club";
		case P_PADDLE:
			return "paddle";
		case P_MACE:
			return "mace";
		case P_MORNING_STAR:
			return "morning star";
		case P_FLAIL:
			return "flail";
		case P_HAMMER:
			return "hammer";
		case P_QUARTERSTAFF:
			return "quarterstaff";
		case P_ORB:
			return "orb";
		case P_CLAW:
			return "claw";
		case P_GRINDER:
			return "grinder";
		case P_POLEARMS:
			return "polearms";
		case P_SPEAR:
			return "spear";
		case P_JAVELIN:
			return "javelin";
		case P_TRIDENT:
			return "trident";
		case P_LANCE:
			return "lance";
		case P_BOW:
			return "bow";
		case P_SLING:
			return "sling";
		case P_FIREARM:
			return "firearms";
		case P_CROSSBOW:
			return "crossbow";
		case P_DART:
			return "dart";
		case P_SHURIKEN:
			return "shuriken";
		case P_BOOMERANG:
			return "boomerang";
		case P_WHIP:
			return "whip";
		case P_UNICORN_HORN:
			return "unicorn horn";
		case P_LIGHTSABER:
			return "lightsaber";
		case P_ATTACK_SPELL:
			return "attack spells";
		case P_HEALING_SPELL:
			return "healing spells";
		case P_DIVINATION_SPELL:
			return "divination spells";
		case P_ENCHANTMENT_SPELL:
			return "enchantment spells";
		case P_PROTECTION_SPELL:
			return "protection spells";
		case P_BODY_SPELL:
			return "body spells";
		case P_OCCULT_SPELL:
			return "occult spells";
		case P_ELEMENTAL_SPELL:
			return "elemental spells";
		case P_CHAOS_SPELL:
			return "chaos spells";
		case P_MATTER_SPELL:
			return "matter spells";
		case P_BARE_HANDED_COMBAT:
			return "bare-handed combat";
		case P_HIGH_HEELS:
			return "high heels";
		case P_GENERAL_COMBAT:
			return "general combat";
		case P_SHIELD:
			return "shield";
		case P_BODY_ARMOR:
			return "body armor";
		case P_TWO_HANDED_WEAPON:
			return "two-handed weapons";
		case P_POLYMORPHING:
			return "polymorphing";
		case P_DEVICES:
			return "devices";
		case P_SEARCHING:
			return "searching";
		case P_SPIRITUALITY:
			return "spirituality";
		case P_PETKEEPING:
			return "petkeeping";
		case P_MISSILE_WEAPONS:
			return "missile weapons";
		case P_TECHNIQUES:
			return "techniques";
		case P_IMPLANTS:
			return "implants";
		case P_SEXY_FLATS:
			return "sexy flats";
		case P_MEMORIZATION:
			return "memorization";
		case P_GUN_CONTROL:
			return "gun control";
		case P_SQUEAKING:
			return "squeaking";
		case P_SYMBIOSIS:
			return "symbiosis";
		case P_SHII_CHO:
			return "form I (Shii-Cho)";
		case P_MAKASHI:
			return "form II (Makashi)";
		case P_SORESU:
			return "form III (Soresu)";
		case P_ATARU:
			return "form IV (Ataru)";
		case P_SHIEN:
			return "form V (Shien)";
		case P_DJEM_SO:
			return "form V (Djem So)";
		case P_NIMAN:
			return "form VI (Niman)";
		case P_JUYO:
			return "form VII (Juyo)";
		case P_VAAPAD:
			return "form VII (Vaapad)";
		case P_WEDI:
			return "form VII (Wedi)";
		case P_MARTIAL_ARTS:
			return "martial arts";
		case P_TWO_WEAPON_COMBAT:
			return "two-weapon combat";
		case P_RIDING:
			return "riding";

		default: return "unknown";
	}
	return "unknown";
}

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
		tmp += (skill_bonus(P_TWO_WEAPON_COMBAT)) - (10 + rnd(10));
	/* Amy note: twoweaponing is supposed to be sacrificing to-hit and defense for more damage output, so I made the
	 * to-hit malus higher than it used to be. However, the damage bonuses have also been increased. */

/*	Put weapon vs. monster type "to hit" bonuses in below:	*/

	/* Blessed weapons used against undead or demons */
	if (Is_weapon && otmp->blessed &&
	   (is_demon(ptr) || is_undead(ptr))) tmp += 2;

	/* KMH, balance patch -- new macro */
	if (is_spear(otmp) && index(kebabable, ptr->mlet)) tmp += 2;

	/* iron chains give bonus versus thick-skinned monsters --Amy */
	if (otmp->otyp == IRON_CHAIN && thick_skinned(ptr)) tmp += 4;
	if (otmp->otyp == STONE_CHAIN && thick_skinned(ptr)) tmp += 4;
	if (otmp->otyp == HEAVY_CHAIN && thick_skinned(ptr)) tmp += 4;
	if (otmp->otyp == GLASS_CHAIN && thick_skinned(ptr)) tmp += 4;
	if (otmp->otyp == BRICK_CHAIN && thick_skinned(ptr)) tmp += 4;
	if (otmp->otyp == METAL_CHAIN && thick_skinned(ptr)) tmp += 4;
	if (otmp->otyp == WAX_CHAIN && thick_skinned(ptr)) tmp += 4;
	if (otmp->otyp == ALLOY_CHAIN && thick_skinned(ptr)) tmp += 4;
	if (otmp->otyp == BUBBLE_CHAIN && thick_skinned(ptr)) tmp += 4;
	if (otmp->otyp == CLOTH_CHAIN && thick_skinned(ptr)) tmp += 4;
	if (otmp->otyp == POURPOOR_LASH && thick_skinned(ptr)) tmp += 6;
	if (otmp->otyp == ROTATING_CHAIN && thick_skinned(ptr)) tmp += 8;
	if (otmp->otyp == GOLD_CHAIN && thick_skinned(ptr)) tmp += 8;
	if (otmp->otyp == CLAY_CHAIN && thick_skinned(ptr)) tmp += 8;
	if (otmp->otyp == SCRAP_CHAIN && thick_skinned(ptr)) tmp += 8;
	if (otmp->otyp == FOAM_CHAIN && thick_skinned(ptr)) tmp += 8;
	if (otmp->otyp == COMPOST_CHAIN && thick_skinned(ptr)) tmp += 8;
	if (otmp->otyp == PAPER_CHAIN && thick_skinned(ptr)) tmp += 8;
	if (otmp->otyp == PLASTIC_CHAIN && thick_skinned(ptr)) tmp += 8;
	if (otmp->otyp == WOOD_CHAIN && thick_skinned(ptr)) tmp += 8;
	if (otmp->otyp == TAR_CHAIN && thick_skinned(ptr)) tmp += 8;
	if (otmp->otyp == WONDER_CHAIN && thick_skinned(ptr)) tmp += 12;
	if (otmp->otyp == LIQUID_CHAIN && thick_skinned(ptr)) tmp += 12;
	if (otmp->otyp == NULL_CHAIN && thick_skinned(ptr)) tmp += 12;
	if (otmp->otyp == GEMSTONE_LASH && thick_skinned(ptr)) tmp += 12;
	if (otmp->otyp == SCOURGE && thick_skinned(ptr)) tmp += 15;
	if (otmp->otyp == ELYSIUM_SCOURGE && thick_skinned(ptr)) tmp += 15;
	if (otmp->otyp == GRANITE_SCOURGE && thick_skinned(ptr)) tmp += 15;
	if (otmp->otyp == ANTIDOTE_SCOURGE && thick_skinned(ptr)) tmp += 15;
	if (otmp->otyp == CELESTIAL_SCOURGE && thick_skinned(ptr)) tmp += 15;
	if (otmp->otyp == DISGUSTING_CHAIN && thick_skinned(ptr)) tmp += 15;
	if (otmp->otyp == CHROME_SCOURGE && thick_skinned(ptr)) tmp += 15;
	if (otmp->otyp == SHADOW_SCOURGE && thick_skinned(ptr)) tmp += 15;
	if (otmp->otyp == COPPER_SCOURGE && thick_skinned(ptr)) tmp += 15;
	if (otmp->otyp == SILK_SCOURGE && thick_skinned(ptr)) tmp += 15;
	if (otmp->otyp == FLESH_LASH && thick_skinned(ptr)) tmp += 20;
	if (otmp->otyp == NUNCHIAKU && thick_skinned(ptr)) tmp += 24;
	if (otmp->otyp == CONUNDRUM_NUNCHIAKU && thick_skinned(ptr)) tmp += 24;
	if (otmp->otyp == CONCRETE_NUNCHIAKU && thick_skinned(ptr)) tmp += 24;
	if (otmp->otyp == ELASTHAN_CHAIN && thick_skinned(ptr)) tmp += 24;
	if (otmp->otyp == NANO_NUNCHIAKU && thick_skinned(ptr)) tmp += 24;
	if (otmp->otyp == LEAD_NUNCHIAKU && thick_skinned(ptr)) tmp += 24;
	if (otmp->otyp == SILVER_NUNCHIAKU && thick_skinned(ptr)) tmp += 24;
	if (otmp->otyp == LEATHER_NUNCHIAKU && thick_skinned(ptr)) tmp += 24;
	if (otmp->otyp == METEORIC_NUNCHIAKU && thick_skinned(ptr)) tmp += 24;
	if (otmp->otyp == ZEBETITE_NUNCHIAKU && thick_skinned(ptr)) tmp += 24;
	if (otmp->otyp == MITHRIL_LASH && thick_skinned(ptr)) tmp += 28;
	if (otmp->otyp == HOSTAGE_CHAIN && thick_skinned(ptr)) tmp += 32;
	if (otmp->otyp == GLASS_HOSTAGE_CHAIN && thick_skinned(ptr)) tmp += 32;
	if (otmp->otyp == MINERAL_HOSTAGE_CHAIN && thick_skinned(ptr)) tmp += 32;
	if (otmp->otyp == ELYSIUM_HOSTAGE_CHAIN && thick_skinned(ptr)) tmp += 32;
	if (otmp->otyp == NUCLEAR_HOSTAGE_CHAIN && thick_skinned(ptr)) tmp += 32;
	if (otmp->otyp == SAND_HOSTAGE_CHAIN && thick_skinned(ptr)) tmp += 32;
	if (otmp->otyp == ALLOY_HOSTAGE_CHAIN && thick_skinned(ptr)) tmp += 32;
	if (otmp->otyp == FOAM_HOSTAGE_CHAIN && thick_skinned(ptr)) tmp += 32;
	if (otmp->otyp == ETHER_HOSTAGE_CHAIN && thick_skinned(ptr)) tmp += 32;
	if (otmp->otyp == OBSIDIAN_HOSTAGE_CHAIN && thick_skinned(ptr)) tmp += 32;

	/* KMH -- Paddles are effective against insects */
	if (otmp->otyp == FLY_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN))
		tmp += 5;
	if (otmp->otyp == VERMIN_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN))
		tmp += 5;
	if (otmp->otyp == INSECT_SQUASHER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN))
		tmp += 12;
	if (otmp->otyp == LASER_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN))
		tmp += 15;
	if (otmp->otyp == TENNIS_RACKET && ptr->mlet == S_BAT)
		tmp += 20;

	if (otmp->otyp == FLY_SWATTER && ptr == &mons[PM_INVINCIBLE_SUPERMAN]) tmp += 25;
	if (otmp->otyp == VERMIN_SWATTER && ptr == &mons[PM_INVINCIBLE_SUPERMAN]) tmp += 25;
	if (otmp->otyp == INSECT_SQUASHER && ptr == &mons[PM_INVINCIBLE_SUPERMAN]) tmp += 50;
	if (otmp->otyp == LASER_SWATTER && ptr == &mons[PM_INVINCIBLE_SUPERMAN]) tmp += 75;

	/* blunt weapons versus undead (Diablo 2) */
	if ((objects[otmp->otyp].oc_skill == P_PICK_AXE || objects[otmp->otyp].oc_skill == P_CLUB || objects[otmp->otyp].oc_skill == P_MACE || objects[otmp->otyp].oc_skill == P_PADDLE || objects[otmp->otyp].oc_skill == P_MORNING_STAR || otmp->otyp == FLAIL || otmp->otyp == KNOUT || otmp->otyp == FLOGGER || otmp->otyp == WEIGHTED_FLAIL || otmp->otyp == RIDING_CROP || otmp->otyp == ETHER_KNOUT || otmp->otyp == SPIKERACK || otmp->otyp == CHAIN_AND_SICKLE || otmp->otyp == OBSID || objects[otmp->otyp].oc_skill == P_HAMMER) && is_undead(ptr)) tmp += 3;
	if (objects[otmp->otyp].oc_skill == P_QUARTERSTAFF && is_undead(ptr)) tmp += 8;
	if (otmp->otyp == TWO_HANDED_FLAIL && is_undead(ptr)) tmp += 8;

	/* as well as silver bullets */
	if (otmp->otyp == SILVER_BULLET && is_undead(ptr)) tmp += 8;

	/* lances versus animals */
	if (objects[otmp->otyp].oc_skill == P_LANCE && is_animal(ptr)) tmp += 3;
	if (otmp->otyp == SURVIVAL_KNIFE && is_animal(ptr)) tmp += 3;

	/* polearms versus golems and designated steeds */
	if (objects[otmp->otyp].oc_skill == P_POLEARMS && (ptr->mlet == S_GOLEM || ptr->mlet == S_CENTAUR || ptr->mlet == S_UNICORN)) tmp += 3;

	if ( (otmp->otyp == SICKLE || otmp->otyp == ELVEN_SICKLE || otmp->otyp == SCYTHE) && ptr->mlet == S_BAD_FOOD) tmp += 10;

	/* electric sword versus quantum mechanic */
	if (otmp->otyp == ELECTRIC_SWORD && ptr->mlet == S_QUANTMECH) tmp += 10;

	/* shotgun versus bears or other quadrupeds */
	if (otmp->otyp == SHOTGUN_SHELL && ptr->mlet == S_QUADRUPED) tmp += 10;
	if (otmp->otyp == LEAD_SHOT && ptr->mlet == S_QUADRUPED) tmp += 10;

	/* axes versus umber hulks */
	if (objects[otmp->otyp].oc_skill == P_AXE && ptr->mlet == S_UMBER) tmp += 3;

	/* whips for lashing people's asses :P */
	if (objects[otmp->otyp].oc_skill == P_WHIP && ptr->mlet == S_HUMAN) tmp += 2;

	/* trident is highly effective against swimmers */
	if (otmp->otyp == TRIDENT && is_swimmer(ptr)) {
	   if (is_waterypool(mon->mx, mon->my)) tmp += 4;
	   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) tmp += 2;
	}

	if (otmp->otyp == FOURDENT && is_swimmer(ptr)) {
	   if (is_waterypool(mon->mx, mon->my)) tmp += 4;
	   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) tmp += 2;
	}

	if (otmp->otyp == SHOOMDENT && is_swimmer(ptr)) {
	   if (is_waterypool(mon->mx, mon->my)) tmp += 4;
	   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) tmp += 2;
	}

	if (otmp->otyp == DIFFICULT_TRIDENT && is_swimmer(ptr)) {
	   if (is_waterypool(mon->mx, mon->my)) tmp += 4;
	   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) tmp += 2;
	}

	if (otmp->otyp == STYGIAN_PIKE && is_swimmer(ptr)) {
	   if (is_waterypool(mon->mx, mon->my)) tmp += 10;
	   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) tmp += 5;
	}

	if (otmp->otyp == MARE_TRIDENT && is_swimmer(ptr)) {
	   if (is_waterypool(mon->mx, mon->my)) tmp += 10;
	   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) tmp += 5;
	}

	if (otmp->otyp == PLOW && is_swimmer(ptr)) {
	   if (is_waterypool(mon->mx, mon->my)) tmp += 10;
	   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) tmp += 5;
	}

	if (otmp->otyp == MANCATCHER && is_swimmer(ptr)) {
	   if (is_waterypool(mon->mx, mon->my)) tmp += 10;
	   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) tmp += 5;
	}

	if (otmp->otyp == TWO_HANDED_TRIDENT && is_swimmer(ptr)) {
	   if (is_waterypool(mon->mx, mon->my)) tmp += 12;
	   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) tmp += 6;
	}

	/* pick-axe used against xorns and earth elementals */
	/* WAC made generic against "rock people" */
	/* KMH, balance patch -- allow all picks */
	if (is_pick(otmp) &&
/*           (passes_walls(ptr) && thick_skinned(ptr))) tmp += 2;*/
           (made_of_rock(ptr))) tmp += 2;


	/* invisible weapons against monsters who can't see invisible */
	if (otmp->oinvis && !perceives(ptr)) tmp += 3;
	if (otmp->oinvisreal) tmp += 7;

	if (Role_if(PM_AKLYST) && (otmp->otyp == AKLYS || otmp->otyp == BLOW_AKLYS) && !(PlayerCannotUseSkills) ) {
		switch (P_SKILL(P_CLUB)) {

			case P_BASIC:	tmp +=  rnd(2); break;
			case P_SKILLED:	tmp +=  rnd(4); break;
			case P_EXPERT:	tmp +=  rnd(6); break;
			case P_MASTER:	tmp +=  rnd(8); break;
			case P_GRAND_MASTER:	tmp +=  rnd(10); break;
			case P_SUPREME_MASTER:	tmp +=  rnd(12); break;
			default: tmp += 0; break;
		}
	}

	if (bimanual(otmp) && !is_missile(otmp) && !is_ammo(otmp) && !is_launcher(otmp) && !(PlayerCannotUseSkills)) {
		switch (P_SKILL(P_TWO_HANDED_WEAPON)) {

			case P_BASIC:	tmp +=  1; break;
			case P_SKILLED:	tmp +=  2; break;
			case P_EXPERT:	tmp +=  3; break;
			case P_MASTER:	tmp +=  4; break;
			case P_GRAND_MASTER:	tmp +=  5; break;
			case P_SUPREME_MASTER:	tmp +=  6; break;
			default: tmp += 0; break;
		}
		switch (P_SKILL(P_VAAPAD)) {

			case P_BASIC:	tmp +=  2; break;
			case P_SKILLED:	tmp +=  4; break;
			case P_EXPERT:	tmp +=  6; break;
			case P_MASTER:	tmp +=  8; break;
			case P_GRAND_MASTER:	tmp +=  10; break;
			case P_SUPREME_MASTER:	tmp +=  12; break;
			default: tmp += 0; break;
		}

	}

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

	if (otmp->oartifact == ART_HOUCHOU)
	        return 9999;

	if (bigmonst(ptr)) {
	    if (objects[otyp].oc_wldam)
		tmp = rnd(objects[otyp].oc_wldam);
	    switch (otyp) {
		case IRON_CHAIN:
		case CROSSBOW_BOLT:
		case DROVEN_BOLT:
		case KOKKEN:
		case MORNING_STAR:
		case BRONZE_MORNING_STAR:
		case SPINED_BALL:
		case JAGGED_STAR:
		case DEVIL_STAR:
		case PARTISAN:
		case RUNESWORD:
		case CHAINWHIP:
		case MITHRIL_WHIP:
		case BASTERD_SWORD:
		case WHITE_FLOWER_SWORD:
		case ELVEN_BROADSWORD:
		case DWARVISH_BATTLE_AXE:
		case BROADSWORD:	tmp++; break;

		case FLAIL:
		case KNOUT:
		case OBSID:
		case RANSEUR:
		case HUGE_CLUB:
		case VOULGE:		tmp += rnd(4); break;

		case ACID_VENOM:
		case SEGFAULT_VENOM:
		case HALBERD:
		case SPETUM:		tmp += rnd(6); break;

		case TAIL_SPIKES:	tmp += rnd(6); tmp += rnd(6); tmp += rnd(6); tmp += rnd(6); tmp += rnd(6); tmp += rnd(6);
 						break;

		case BATTLE_AXE:
		case BARDICHE:
		case SCYTHE:
		case STYGIAN_PIKE:
		case MANCATCHER:
		case MARE_TRIDENT:
		case TRIDENT:		tmp += d(2,4); break;

		case INFERNAL_BARDICHE: tmp += d(3,4); break;

		case TSURUGI:
		case TWO_HANDED_TRIDENT:
		case DWARVISH_MATTOCK:
		case TWO_HANDED_SWORD:	tmp += d(2,6); break;

		case SCIMITAR:
			if(otmp->oartifact == ART_REAVER) tmp += d(1,8); break;

		case GREEN_LIGHTSABER:  tmp += rnd(13); break;
		case BLUE_LIGHTSABER:   tmp += rnd(12); break;
		case MYSTERY_LIGHTSABER:   tmp += rnd(12); break;
		case VIOLET_LIGHTSABER:	tmp += rnd(8); break;
		case YELLOW_LIGHTSABER:	tmp += rnd(12); break;
		case LIGHTWHIP:	tmp += rnd(8); break;
		case ELECTRIC_CIGARETTE:	tmp += rnd(10); break;
		case NANO_HAMMER:	tmp += rnd(8); break;

		case WHITE_DOUBLE_LIGHTSABER: 
					if (otmp->altmode) tmp += rnd(12);
					/* fallthrough */
		case WHITE_LIGHTSABER:	tmp += rnd(14); break;

		case RED_DOUBLE_LIGHTSABER: 
					if (otmp->altmode) tmp += rnd(15);
					/* fallthrough */
		case RED_LIGHTSABER:    tmp += rnd(10); break;
	    }
	} else {
	    if (objects[otyp].oc_wsdam)
		tmp = rnd(objects[otyp].oc_wsdam);
	    switch (otyp) {
		case IRON_CHAIN:
		case CROSSBOW_BOLT:
		case DROVEN_BOLT:
		case KOKKEN:
		case MACE:
		case ELVEN_MACE:
		case SILVER_MACE:
		case ORNATE_MACE:
		case FLANGED_MACE:
		case REINFORCED_MACE:
		case WAR_HAMMER:
		case MALLET:
		case FLAIL:
		case KNOUT:
		case OBSID:
		case SPETUM:
		case PITCHFORK:
		case TWO_HANDED_FLAIL:
		case STYGIAN_PIKE:
		case MANCATCHER:
		case MARE_TRIDENT:
		case DWARVISH_BATTLE_AXE:
		case CHAINWHIP:
		case MITHRIL_WHIP:
		case TRIDENT:		tmp++; break;

		case BATTLE_AXE:
		case BARDICHE:
		case SCYTHE:
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
		case TWO_HANDED_TRIDENT:
		case HUGE_CLUB:
		case VOULGE:		tmp += rnd(4); break;

		case INFERNAL_ANCUS:
		case INFERNAL_BARDICHE: tmp += d(2,4); break;

		case WHITE_FLOWER_SWORD:
						tmp += rnd(2); break;

		case GREEN_LIGHTSABER:  tmp += rnd(9); break;
		case BLUE_LIGHTSABER:   tmp += rnd(8); break;
		case MYSTERY_LIGHTSABER:   tmp += rnd(8); break;
		case VIOLET_LIGHTSABER:	tmp += rnd(6); break;
		case YELLOW_LIGHTSABER:	tmp += rnd(10); break;
		case LIGHTWHIP:	tmp += rnd(8); break;
		case ELECTRIC_CIGARETTE:	tmp += rnd(10); break;
		case NANO_HAMMER:	tmp += rnd(8); break;

		case WHITE_DOUBLE_LIGHTSABER:
					if (otmp->altmode) tmp += rnd(15);
					/* fallthrough */
		case WHITE_LIGHTSABER:	tmp += rnd(11); break;

		case RED_DOUBLE_LIGHTSABER:
					if (otmp->altmode) tmp += rnd(12);
					/* fallthrough */
		case RED_LIGHTSABER:    tmp += rnd(6); break;

		case SEGFAULT_VENOM:
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

	if ((objects[otyp].oc_material <= MT_LEATHER || (objects[otyp].oc_material == MT_INKA && !hates_inka(ptr)) || (objects[otyp].oc_material == MT_SILK) || (objects[otyp].oc_material == MT_COMPOST) ) && thick_skinned(ptr) && tmp > 0)
		/* thick skinned/scaled creatures don't feel it */
		tmp = 1;
	if ((is_shade(ptr) || mon->egotype_shader) && objects[otyp].oc_material != MT_SILVER && objects[otyp].oc_material != MT_ARCANIUM)
		tmp = 0;

	/* "very heavy iron ball"; weight increase is in increments of 600 */
	if (otmp->oclass == BALL_CLASS && tmp > 0) {

		int wt = (int)objects[otmp->otyp].oc_weight;

		if ((int)otmp->owt > wt) {
			wt = ((int)otmp->owt - wt) / 600;
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
	    if (objects[otyp].oc_material == MT_SILVER && hates_silver(ptr))
		bonus += rnd(20);
	    if (objects[otyp].oc_material == MT_COPPER && hates_copper(ptr))
		bonus += 20;
	    if (objects[otyp].oc_material == MT_PLATINUM && hates_platinum(ptr))
		bonus += 20;
	    if (otmp->cursed && (hates_cursed(ptr) || (ptr == youmonst.data && (youmonst.data->mlet == S_ANGEL || Race_if(PM_HUMANOID_ANGEL)))) ) {
		bonus += 4;
		if (otmp->hvycurse) bonus += 4;
		if (otmp->prmcurse) bonus += 7;
		if (otmp->bbrcurse) bonus += 15;
		if (otmp->evilcurse) bonus += 15;
		if (otmp->morgcurse) bonus += 15;
		}
	    if (objects[otyp].oc_material == MT_VIVA && hates_viva(ptr))
		bonus += 20;
	    if (objects[otyp].oc_material == MT_INKA && hates_inka(ptr))
		bonus += 5;
	    if (otyp == ODOR_SHOT && hates_odor(ptr))
		bonus += rnd(10);

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
		if (otmp->otyp == IRON_CHAIN && thick_skinned(ptr)) bonus += rnd(4);
		if (otmp->otyp == STONE_CHAIN && thick_skinned(ptr)) bonus += rnd(4);
		if (otmp->otyp == HEAVY_CHAIN && thick_skinned(ptr)) bonus += rnd(4);
		if (otmp->otyp == GLASS_CHAIN && thick_skinned(ptr)) bonus += rnd(4);
		if (otmp->otyp == BRICK_CHAIN && thick_skinned(ptr)) bonus += rnd(4);
		if (otmp->otyp == METAL_CHAIN && thick_skinned(ptr)) bonus += rnd(4);
		if (otmp->otyp == WAX_CHAIN && thick_skinned(ptr)) bonus += rnd(4);
		if (otmp->otyp == ALLOY_CHAIN && thick_skinned(ptr)) bonus += rnd(4);
		if (otmp->otyp == BUBBLE_CHAIN && thick_skinned(ptr)) bonus += rnd(4);
		if (otmp->otyp == CLOTH_CHAIN && thick_skinned(ptr)) bonus += rnd(4);
		if (otmp->otyp == POURPOOR_LASH && thick_skinned(ptr)) bonus += rnd(6);
		if (otmp->otyp == ROTATING_CHAIN && thick_skinned(ptr)) bonus += rnd(8);
		if (otmp->otyp == GOLD_CHAIN && thick_skinned(ptr)) bonus += rnd(8);
		if (otmp->otyp == CLAY_CHAIN && thick_skinned(ptr)) bonus += rnd(8);
		if (otmp->otyp == COMPOST_CHAIN && thick_skinned(ptr)) bonus += rnd(8);
		if (otmp->otyp == PAPER_CHAIN && thick_skinned(ptr)) bonus += rnd(8);
		if (otmp->otyp == PLASTIC_CHAIN && thick_skinned(ptr)) bonus += rnd(8);
		if (otmp->otyp == SCRAP_CHAIN && thick_skinned(ptr)) bonus += rnd(8);
		if (otmp->otyp == FOAM_CHAIN && thick_skinned(ptr)) bonus += rnd(8);
		if (otmp->otyp == WOOD_CHAIN && thick_skinned(ptr)) bonus += rnd(8);
		if (otmp->otyp == TAR_CHAIN && thick_skinned(ptr)) bonus += rnd(8);
		if (otmp->otyp == WONDER_CHAIN && thick_skinned(ptr)) bonus += rnd(12);
		if (otmp->otyp == LIQUID_CHAIN && thick_skinned(ptr)) bonus += rnd(12);
		if (otmp->otyp == NULL_CHAIN && thick_skinned(ptr)) bonus += rnd(12);
		if (otmp->otyp == GEMSTONE_LASH && thick_skinned(ptr)) bonus += rnd(12);
		if (otmp->otyp == SCOURGE && thick_skinned(ptr)) bonus += rnd(15);
		if (otmp->otyp == ELYSIUM_SCOURGE && thick_skinned(ptr)) bonus += rnd(15);
		if (otmp->otyp == GRANITE_SCOURGE && thick_skinned(ptr)) bonus += rnd(15);
		if (otmp->otyp == DISGUSTING_CHAIN && thick_skinned(ptr)) bonus += rnd(15);
		if (otmp->otyp == CHROME_SCOURGE && thick_skinned(ptr)) bonus += rnd(15);
		if (otmp->otyp == SHADOW_SCOURGE && thick_skinned(ptr)) bonus += rnd(15);
		if (otmp->otyp == COPPER_SCOURGE && thick_skinned(ptr)) bonus += rnd(15);
		if (otmp->otyp == ANTIDOTE_SCOURGE && thick_skinned(ptr)) bonus += rnd(15);
		if (otmp->otyp == CELESTIAL_SCOURGE && thick_skinned(ptr)) bonus += rnd(15);
		if (otmp->otyp == SILK_SCOURGE && thick_skinned(ptr)) bonus += rnd(15);
		if (otmp->otyp == FLESH_LASH && thick_skinned(ptr)) bonus += rnd(20);
		if (otmp->otyp == NUNCHIAKU && thick_skinned(ptr)) bonus += rnd(24);
		if (otmp->otyp == CONUNDRUM_NUNCHIAKU && thick_skinned(ptr)) bonus += rnd(24);
		if (otmp->otyp == CONCRETE_NUNCHIAKU && thick_skinned(ptr)) bonus += rnd(24);
		if (otmp->otyp == ELASTHAN_CHAIN && thick_skinned(ptr)) bonus += rnd(24);
		if (otmp->otyp == NANO_NUNCHIAKU && thick_skinned(ptr)) bonus += rnd(24);
		if (otmp->otyp == LEAD_NUNCHIAKU && thick_skinned(ptr)) bonus += rnd(24);
		if (otmp->otyp == SILVER_NUNCHIAKU && thick_skinned(ptr)) bonus += rnd(24);
		if (otmp->otyp == LEATHER_NUNCHIAKU && thick_skinned(ptr)) bonus += rnd(24);
		if (otmp->otyp == METEORIC_NUNCHIAKU && thick_skinned(ptr)) bonus += rnd(24);
		if (otmp->otyp == ZEBETITE_NUNCHIAKU && thick_skinned(ptr)) bonus += rnd(24);
		if (otmp->otyp == MITHRIL_LASH && thick_skinned(ptr)) bonus += rnd(28);
		if (otmp->otyp == HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);
		if (otmp->otyp == GLASS_HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);
		if (otmp->otyp == MINERAL_HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);
		if (otmp->otyp == ELYSIUM_HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);
		if (otmp->otyp == NUCLEAR_HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);
		if (otmp->otyp == SAND_HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);
		if (otmp->otyp == ETHER_HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);
		if (otmp->otyp == FOAM_HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);
		if (otmp->otyp == ALLOY_HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);
		if (otmp->otyp == OBSIDIAN_HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);

	    /* KMH -- Paddles are effective against insects */
	    if (otmp->otyp == FLY_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN))
		bonus += rnd(5);
	    if (otmp->otyp == VERMIN_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN))
		bonus += rnd(5);
	    if (otmp->otyp == INSECT_SQUASHER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN))
		bonus += rnd(12);
	    if (otmp->otyp == LASER_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN))
		bonus += 15;

	    if (otmp->otyp == FLY_SWATTER && ptr == &mons[PM_INVINCIBLE_SUPERMAN]) bonus += rnd(25);
	    if (otmp->otyp == VERMIN_SWATTER && ptr == &mons[PM_INVINCIBLE_SUPERMAN]) bonus += rnd(25);
	    if (otmp->otyp == INSECT_SQUASHER && ptr == &mons[PM_INVINCIBLE_SUPERMAN]) bonus += rnd(50);
	    if (otmp->otyp == LASER_SWATTER && ptr == &mons[PM_INVINCIBLE_SUPERMAN]) bonus += rnd(75);

	    /* trident is highly effective against swimmers */
	    if (otmp->otyp == TRIDENT && is_swimmer(ptr)) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 4;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 2;
		   if (Race_if(PM_NEMESIS)) bonus += 20;
	    }

	    if (otmp->otyp == FOURDENT && is_swimmer(ptr)) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 4;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 2;
		   if (Race_if(PM_NEMESIS)) bonus += 20;
	    }

	    if (otmp->otyp == SHOOMDENT && is_swimmer(ptr)) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 4;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 2;
		   if (Race_if(PM_NEMESIS)) bonus += 20;
	    }

	    if (otmp->otyp == DIFFICULT_TRIDENT && is_swimmer(ptr)) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 4;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 2;
		   if (Race_if(PM_NEMESIS)) bonus += 20;
	    }

	    if (otmp->otyp == STYGIAN_PIKE && is_swimmer(ptr)) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 10;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 5;
		   if (Race_if(PM_NEMESIS)) bonus += 20;
	    }

	    if (otmp->otyp == MANCATCHER && is_swimmer(ptr)) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 10;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 5;
		   if (Race_if(PM_NEMESIS)) bonus += 20;
	    }

	    if (otmp->otyp == MARE_TRIDENT && is_swimmer(ptr)) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 10;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 5;
		   if (Race_if(PM_NEMESIS)) bonus += 20;
	    }

	    if (otmp->otyp == PLOW && is_swimmer(ptr)) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 10;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 5;
		   if (Race_if(PM_NEMESIS)) bonus += 20;
	    }

	    if (otmp->otyp == TWO_HANDED_TRIDENT && is_swimmer(ptr)) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 12;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 6;
		   if (Race_if(PM_NEMESIS)) bonus += 20;
	    }

	    /* blunt weapons versus undead (Diablo 2) */
	    if ((objects[otmp->otyp].oc_skill == P_PICK_AXE || objects[otmp->otyp].oc_skill == P_CLUB || objects[otmp->otyp].oc_skill == P_MACE || objects[otmp->otyp].oc_skill == P_PADDLE || objects[otmp->otyp].oc_skill == P_MORNING_STAR || otmp->otyp == FLAIL || otmp->otyp == KNOUT || otmp->otyp == FLOGGER || otmp->otyp == WEIGHTED_FLAIL || otmp->otyp == RIDING_CROP || otmp->otyp == ETHER_KNOUT || otmp->otyp == SPIKERACK || otmp->otyp == CHAIN_AND_SICKLE || otmp->otyp == OBSID || objects[otmp->otyp].oc_skill == P_HAMMER) && is_undead(ptr)) bonus += rnd(2);
	    if (objects[otmp->otyp].oc_skill == P_QUARTERSTAFF && is_undead(ptr)) bonus += rnd(6);
	    if (otmp->otyp == TWO_HANDED_FLAIL && is_undead(ptr)) bonus += rnd(6);

	    /* as well as silver bullets */
	    if (otmp->otyp == SILVER_BULLET && is_undead(ptr)) bonus += 8;

	    /* lances versus animals */
	    if (objects[otmp->otyp].oc_skill == P_LANCE && is_animal(ptr)) bonus += rnd(2);
	    if (otmp->otyp == SURVIVAL_KNIFE && is_animal(ptr)) bonus += rnd(2);

	    /* polearms versus golems */
	    if (objects[otmp->otyp].oc_skill == P_POLEARMS && (ptr->mlet == S_GOLEM || ptr->mlet == S_CENTAUR || ptr->mlet == S_UNICORN) ) bonus += rnd(2);

	    if ((otmp->otyp == SICKLE || otmp->otyp == ELVEN_SICKLE || otmp->otyp == SCYTHE) && ptr->mlet == S_BAD_FOOD) bonus += rnd(10);

	    /* electric sword versus quantum mechanic */
	    if (otmp->otyp == ELECTRIC_SWORD && ptr->mlet == S_QUANTMECH) bonus += rnd(10);

	    /* shotgun versus bears or other quadrupeds */
	    if (otmp->otyp == SHOTGUN_SHELL && ptr->mlet == S_QUADRUPED) bonus += (10 + rnd(10));
	    if (otmp->otyp == LEAD_SHOT && ptr->mlet == S_QUADRUPED) bonus += (10 + rnd(10));

	    /* axes versus umber hulks */
	    if (objects[otmp->otyp].oc_skill == P_AXE && ptr->mlet == S_UMBER) bonus += rnd(2);

	    /* whips for lashing people's asses :P */
	    if (objects[otmp->otyp].oc_skill == P_WHIP && ptr->mlet == S_HUMAN) bonus += 1;

	    /* high heels will absolutely wreck the elder priest */
	    if ((otmp->otyp == WEDGED_LITTLE_GIRL_SANDAL || otmp->otyp == STURDY_PLATEAU_BOOT_FOR_GIRLS || otmp->otyp == HUGGING_BOOT || otmp->otyp == BLOCK_HEELED_COMBAT_BOOT || otmp->otyp == WOODEN_GETA || otmp->otyp == LACQUERED_DANCING_SHOE || otmp->otyp == HIGH_HEELED_SANDAL || otmp->otyp == SEXY_LEATHER_PUMP || otmp->otyp == STICKSHOE || otmp->otyp == SPIKED_BATTLE_BOOT || otmp->otyp == INKA_BOOT || otmp->otyp == SOFT_LADY_SHOE || otmp->otyp == STEEL_CAPPED_SANDAL || otmp->otyp == BLOCK_HEELED_SANDAL || otmp->otyp == PROSTITUTE_SHOE || otmp->otyp == DOGSHIT_BOOT) && (mon->data == &mons[PM_DNETHACK_ELDER_PRIEST_TM_])) bonus += 50;

	    if ((otmp->otyp == WEDGED_LITTLE_GIRL_SANDAL || otmp->otyp == STURDY_PLATEAU_BOOT_FOR_GIRLS || otmp->otyp == HUGGING_BOOT || otmp->otyp == BLOCK_HEELED_COMBAT_BOOT || otmp->otyp == WOODEN_GETA || otmp->otyp == LACQUERED_DANCING_SHOE || otmp->otyp == HIGH_HEELED_SANDAL || otmp->otyp == SEXY_LEATHER_PUMP || otmp->otyp == STICKSHOE || otmp->otyp == SPIKED_BATTLE_BOOT || otmp->otyp == INKA_BOOT || otmp->otyp == SOFT_LADY_SHOE || otmp->otyp == STEEL_CAPPED_SANDAL || otmp->otyp == BLOCK_HEELED_SANDAL || otmp->otyp == PROSTITUTE_SHOE || otmp->otyp == DOGSHIT_BOOT) && (attacktype(mon->data, AT_TENT))) {
			bonus += 2;
			if (otmp && otmp->spe > 0) bonus += otmp->spe;
		}

	    /* if the weapon is going to get a double damage bonus, adjust
	       this bonus so that effectively it's added after the doubling
		 Amy edit: "double" damage is no longer double damage, and therefore we'll only halve it half of the time */
	    if (bonus > 1 && otmp->oartifact && !rn2(2) && spec_dbon(otmp, mon, 25) >= 25)
		bonus = (bonus + 1) / 2;

	    tmp += bonus;
	}

	if (tmp > 0) {
		/* It's debateable whether a rusted blunt instrument
		   should do less damage than a pristine one, since
		   it will hit with essentially the same impact, but
		   there ought to some penalty for using damaged gear
		   so always subtract erosion even for blunt weapons. */
		tmp -= greatest_erosionX(otmp);
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

	if (otmp->oartifact == ART_HOUCHOU)
	        return 9999;

	if (bigmonst(ptr)) {
	    if (objects[otyp].oc_wldam)
		tmp = rnd(objects[otyp].oc_wldam);
	    switch (otyp) {
		case IRON_CHAIN:
		case CROSSBOW_BOLT:
		case DROVEN_BOLT:
		case KOKKEN:
		case MORNING_STAR:
		case BRONZE_MORNING_STAR:
		case SPINED_BALL:
		case JAGGED_STAR:
		case DEVIL_STAR:
		case PARTISAN:
		case RUNESWORD:
		case BASTERD_SWORD:
		case ELVEN_BROADSWORD:
		case DWARVISH_BATTLE_AXE:
		case WHITE_FLOWER_SWORD:
		case CHAINWHIP:
		case MITHRIL_WHIP:
		case BROADSWORD:	tmp++; break;

		case FLAIL:
		case KNOUT:
		case OBSID:
		case RANSEUR:
		case HUGE_CLUB:
		case VOULGE:		tmp += rnd(4); break;

		case ACID_VENOM:
		case SEGFAULT_VENOM:
		case HALBERD:
		case SPETUM:		tmp += rnd(6); break;

		case TAIL_SPIKES:	tmp += rnd(6); tmp += rnd(6); tmp += rnd(6); tmp += rnd(6); tmp += rnd(6); tmp += rnd(6);
 						break;

		case BATTLE_AXE:
		case BARDICHE:
		case SCYTHE:
		case STYGIAN_PIKE:
		case MANCATCHER:
		case MARE_TRIDENT:
		case TRIDENT:		tmp += d(2,4); break;

		case INFERNAL_BARDICHE: tmp += d(3,4); break;

		case TSURUGI:
		case DWARVISH_MATTOCK:
		case TWO_HANDED_TRIDENT:
		case TWO_HANDED_SWORD:	tmp += d(2,6); break;

		case SCIMITAR:
			if(otmp->oartifact == ART_REAVER) tmp += d(1,8); break;

		case GREEN_LIGHTSABER:  tmp += rnd(13); break;
		case BLUE_LIGHTSABER:   tmp += rnd(12); break;
		case MYSTERY_LIGHTSABER:   tmp += rnd(12); break;
		case VIOLET_LIGHTSABER:	tmp += rnd(8); break;
		case YELLOW_LIGHTSABER:	tmp += rnd(12); break;
		case LIGHTWHIP:	tmp += rnd(8); break;
		case ELECTRIC_CIGARETTE:	tmp += rnd(10); break;
		case NANO_HAMMER:	tmp += rnd(8); break;

		case WHITE_DOUBLE_LIGHTSABER: 
					if (otmp->altmode) tmp += rnd(12);
					/* fallthrough */
		case WHITE_LIGHTSABER:	tmp += rnd(14); break;

		case RED_DOUBLE_LIGHTSABER: 
					if (otmp->altmode) tmp += rnd(15);
					/* fallthrough */
		case RED_LIGHTSABER:    tmp += rnd(10); break;

	    }
	} else {
	    if (objects[otyp].oc_wsdam)
		tmp = rnd(objects[otyp].oc_wsdam);
	    switch (otyp) {
		case IRON_CHAIN:
		case CROSSBOW_BOLT:
		case DROVEN_BOLT:
		case KOKKEN:
		case MACE:
		case ELVEN_MACE:
		case SILVER_MACE:
		case ORNATE_MACE:
		case FLANGED_MACE:
		case REINFORCED_MACE:
		case WAR_HAMMER:
		case MALLET:
		case FLAIL:
		case KNOUT:
		case OBSID:
		case SPETUM:
		case PITCHFORK:
		case TWO_HANDED_FLAIL:
		case STYGIAN_PIKE:
		case MANCATCHER:
		case MARE_TRIDENT:
		case DWARVISH_BATTLE_AXE:
		case CHAINWHIP:
		case MITHRIL_WHIP:
		case TRIDENT:		tmp++; break;

		case BATTLE_AXE:
		case BARDICHE:
		case SCYTHE:
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
		case TWO_HANDED_TRIDENT:
		case HUGE_CLUB:
		case VOULGE:		tmp += rnd(4); break;

		case INFERNAL_ANCUS:
		case INFERNAL_BARDICHE: tmp += d(2,4); break;

		case WHITE_FLOWER_SWORD:
						tmp += rnd(2); break;

		case GREEN_LIGHTSABER:  tmp += rnd(9); break;
		case BLUE_LIGHTSABER:   tmp += rnd(8); break;
		case MYSTERY_LIGHTSABER:   tmp += rnd(8); break;
		case VIOLET_LIGHTSABER:	tmp += rnd(6); break;
		case YELLOW_LIGHTSABER:	tmp += rnd(10); break;
		case LIGHTWHIP:	tmp += rnd(8); break;
		case ELECTRIC_CIGARETTE:	tmp += rnd(10); break;
		case NANO_HAMMER:	tmp += rnd(8); break;

		case WHITE_DOUBLE_LIGHTSABER:
					if (otmp->altmode) tmp += rnd(15);
					/* fallthrough */
		case WHITE_LIGHTSABER:	tmp += rnd(11); break;

		case RED_DOUBLE_LIGHTSABER:
					if (otmp->altmode) tmp += rnd(12);
					/* fallthrough */
		case RED_LIGHTSABER:    tmp += rnd(6); break;

		case SEGFAULT_VENOM:
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

	if ((objects[otyp].oc_material <= MT_LEATHER || (objects[otyp].oc_material == MT_INKA && !hates_inka(ptr)) || (objects[otyp].oc_material == MT_SILK) || (objects[otyp].oc_material == MT_COMPOST) ) && thick_skinned(ptr) && tmp > 0)
		/* thick skinned/scaled creatures don't feel it */
		tmp = 1;
	if ((is_shade(ptr) || mon->egotype_shader) && objects[otyp].oc_material != MT_SILVER && objects[otyp].oc_material != MT_ARCANIUM)
		tmp = 0;

	/* "very heavy iron ball"; weight increase is in increments of 600 */
	if (otmp->oclass == BALL_CLASS && tmp > 0) {

		int wt = (int)objects[otmp->otyp].oc_weight;

		if ((int)otmp->owt > wt) {
			wt = ((int)otmp->owt - wt) / 600;
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
	    if (objects[otyp].oc_material == MT_SILVER && hates_silver(ptr))
		bonus += rnd(20);
	    if (objects[otyp].oc_material == MT_COPPER && hates_copper(ptr))
		bonus += 20;
	    if (objects[otyp].oc_material == MT_PLATINUM && hates_platinum(ptr))
		bonus += 20;
	    if (otmp->cursed && (hates_cursed(ptr) || (ptr == youmonst.data && (youmonst.data->mlet == S_ANGEL || Race_if(PM_HUMANOID_ANGEL)))) ) {
		bonus += 4;
		if (otmp->hvycurse) bonus += 4;
		if (otmp->prmcurse) bonus += 7;
		if (otmp->bbrcurse) bonus += 15;
		if (otmp->evilcurse) bonus += 15;
		if (otmp->morgcurse) bonus += 15;
		}
	    if (objects[otyp].oc_material == MT_VIVA && hates_viva(ptr))
		bonus += 20;
	    if (otyp == ODOR_SHOT && hates_odor(ptr))
		bonus += rnd(10);

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
		if (otmp->otyp == IRON_CHAIN && thick_skinned(ptr)) bonus += rnd(4);
		if (otmp->otyp == STONE_CHAIN && thick_skinned(ptr)) bonus += rnd(4);
		if (otmp->otyp == HEAVY_CHAIN && thick_skinned(ptr)) bonus += rnd(4);
		if (otmp->otyp == GLASS_CHAIN && thick_skinned(ptr)) bonus += rnd(4);
		if (otmp->otyp == BRICK_CHAIN && thick_skinned(ptr)) bonus += rnd(4);
		if (otmp->otyp == METAL_CHAIN && thick_skinned(ptr)) bonus += rnd(4);
		if (otmp->otyp == WAX_CHAIN && thick_skinned(ptr)) bonus += rnd(4);
		if (otmp->otyp == ALLOY_CHAIN && thick_skinned(ptr)) bonus += rnd(4);
		if (otmp->otyp == BUBBLE_CHAIN && thick_skinned(ptr)) bonus += rnd(4);
		if (otmp->otyp == CLOTH_CHAIN && thick_skinned(ptr)) bonus += rnd(4);
		if (otmp->otyp == POURPOOR_LASH && thick_skinned(ptr)) bonus += rnd(6);
		if (otmp->otyp == ROTATING_CHAIN && thick_skinned(ptr)) bonus += rnd(8);
		if (otmp->otyp == GOLD_CHAIN && thick_skinned(ptr)) bonus += rnd(8);
		if (otmp->otyp == CLAY_CHAIN && thick_skinned(ptr)) bonus += rnd(8);
		if (otmp->otyp == COMPOST_CHAIN && thick_skinned(ptr)) bonus += rnd(8);
		if (otmp->otyp == PAPER_CHAIN && thick_skinned(ptr)) bonus += rnd(8);
		if (otmp->otyp == PLASTIC_CHAIN && thick_skinned(ptr)) bonus += rnd(8);
		if (otmp->otyp == WOOD_CHAIN && thick_skinned(ptr)) bonus += rnd(8);
		if (otmp->otyp == TAR_CHAIN && thick_skinned(ptr)) bonus += rnd(8);
		if (otmp->otyp == FOAM_CHAIN && thick_skinned(ptr)) bonus += rnd(8);
		if (otmp->otyp == SCRAP_CHAIN && thick_skinned(ptr)) bonus += rnd(8);
		if (otmp->otyp == WONDER_CHAIN && thick_skinned(ptr)) bonus += rnd(12);
		if (otmp->otyp == LIQUID_CHAIN && thick_skinned(ptr)) bonus += rnd(12);
		if (otmp->otyp == NULL_CHAIN && thick_skinned(ptr)) bonus += rnd(12);
		if (otmp->otyp == GEMSTONE_LASH && thick_skinned(ptr)) bonus += rnd(12);
		if (otmp->otyp == SCOURGE && thick_skinned(ptr)) bonus += rnd(15);
		if (otmp->otyp == ELYSIUM_SCOURGE && thick_skinned(ptr)) bonus += rnd(15);
		if (otmp->otyp == GRANITE_SCOURGE && thick_skinned(ptr)) bonus += rnd(15);
		if (otmp->otyp == DISGUSTING_CHAIN && thick_skinned(ptr)) bonus += rnd(15);
		if (otmp->otyp == CHROME_SCOURGE && thick_skinned(ptr)) bonus += rnd(15);
		if (otmp->otyp == ANTIDOTE_SCOURGE && thick_skinned(ptr)) bonus += rnd(15);
		if (otmp->otyp == CELESTIAL_SCOURGE && thick_skinned(ptr)) bonus += rnd(15);
		if (otmp->otyp == SHADOW_SCOURGE && thick_skinned(ptr)) bonus += rnd(15);
		if (otmp->otyp == COPPER_SCOURGE && thick_skinned(ptr)) bonus += rnd(15);
		if (otmp->otyp == SILK_SCOURGE && thick_skinned(ptr)) bonus += rnd(15);
		if (otmp->otyp == FLESH_LASH && thick_skinned(ptr)) bonus += rnd(20);
		if (otmp->otyp == NUNCHIAKU && thick_skinned(ptr)) bonus += rnd(24);
		if (otmp->otyp == CONUNDRUM_NUNCHIAKU && thick_skinned(ptr)) bonus += rnd(24);
		if (otmp->otyp == CONCRETE_NUNCHIAKU && thick_skinned(ptr)) bonus += rnd(24);
		if (otmp->otyp == ELASTHAN_CHAIN && thick_skinned(ptr)) bonus += rnd(24);
		if (otmp->otyp == NANO_NUNCHIAKU && thick_skinned(ptr)) bonus += rnd(24);
		if (otmp->otyp == LEAD_NUNCHIAKU && thick_skinned(ptr)) bonus += rnd(24);
		if (otmp->otyp == SILVER_NUNCHIAKU && thick_skinned(ptr)) bonus += rnd(24);
		if (otmp->otyp == METEORIC_NUNCHIAKU && thick_skinned(ptr)) bonus += rnd(24);
		if (otmp->otyp == ZEBETITE_NUNCHIAKU && thick_skinned(ptr)) bonus += rnd(24);
		if (otmp->otyp == LEATHER_NUNCHIAKU && thick_skinned(ptr)) bonus += rnd(24);
		if (otmp->otyp == MITHRIL_LASH && thick_skinned(ptr)) bonus += rnd(28);
		if (otmp->otyp == HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);
		if (otmp->otyp == GLASS_HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);
		if (otmp->otyp == MINERAL_HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);
		if (otmp->otyp == ELYSIUM_HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);
		if (otmp->otyp == NUCLEAR_HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);
		if (otmp->otyp == SAND_HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);
		if (otmp->otyp == ETHER_HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);
		if (otmp->otyp == FOAM_HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);
		if (otmp->otyp == ALLOY_HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);
		if (otmp->otyp == OBSIDIAN_HOSTAGE_CHAIN && thick_skinned(ptr)) bonus += rnd(32);

		if (otmp->oclass == BALL_CLASS && RngePunishment) bonus += 5;

		if (!(PlayerCannotUseSkills)) {

	if (otmp->otyp == IRON_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += 1;
	if (otmp->otyp == IRON_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(2);
	if (otmp->otyp == IRON_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(3);
	if (otmp->otyp == IRON_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(4);
	if (otmp->otyp == IRON_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(5);

	if (otmp->otyp == STONE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += 1;
	if (otmp->otyp == STONE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(2);
	if (otmp->otyp == STONE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(3);
	if (otmp->otyp == STONE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(4);
	if (otmp->otyp == STONE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(5);

	if (otmp->otyp == HEAVY_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += 1;
	if (otmp->otyp == HEAVY_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(2);
	if (otmp->otyp == HEAVY_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(3);
	if (otmp->otyp == HEAVY_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(4);
	if (otmp->otyp == HEAVY_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(5);

	if (otmp->otyp == GLASS_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += 1;
	if (otmp->otyp == GLASS_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(2);
	if (otmp->otyp == GLASS_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(3);
	if (otmp->otyp == GLASS_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(4);
	if (otmp->otyp == GLASS_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(5);

	if (otmp->otyp == BRICK_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += 1;
	if (otmp->otyp == BRICK_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(2);
	if (otmp->otyp == BRICK_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(3);
	if (otmp->otyp == BRICK_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(4);
	if (otmp->otyp == BRICK_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(5);

	if (otmp->otyp == METAL_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += 1;
	if (otmp->otyp == METAL_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(2);
	if (otmp->otyp == METAL_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(3);
	if (otmp->otyp == METAL_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(4);
	if (otmp->otyp == METAL_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(5);

	if (otmp->otyp == WAX_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += 1;
	if (otmp->otyp == WAX_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(2);
	if (otmp->otyp == WAX_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(3);
	if (otmp->otyp == WAX_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(4);
	if (otmp->otyp == WAX_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(5);

	if (otmp->otyp == ALLOY_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += 1;
	if (otmp->otyp == ALLOY_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(2);
	if (otmp->otyp == ALLOY_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(3);
	if (otmp->otyp == ALLOY_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(4);
	if (otmp->otyp == ALLOY_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(5);

	if (otmp->otyp == BUBBLE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += 1;
	if (otmp->otyp == BUBBLE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(2);
	if (otmp->otyp == BUBBLE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(3);
	if (otmp->otyp == BUBBLE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(4);
	if (otmp->otyp == BUBBLE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(5);

	if (otmp->otyp == CLOTH_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += 1;
	if (otmp->otyp == CLOTH_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(2);
	if (otmp->otyp == CLOTH_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(3);
	if (otmp->otyp == CLOTH_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(4);
	if (otmp->otyp == CLOTH_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(5);

	if (otmp->otyp == POURPOOR_LASH && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += 1;
	if (otmp->otyp == POURPOOR_LASH && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(3);
	if (otmp->otyp == POURPOOR_LASH && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(5);
	if (otmp->otyp == POURPOOR_LASH && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(6);
	if (otmp->otyp == POURPOOR_LASH && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(8);

	if (otmp->otyp == ROTATING_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(2);
	if (otmp->otyp == ROTATING_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(4);
	if (otmp->otyp == ROTATING_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(6);
	if (otmp->otyp == ROTATING_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(8);
	if (otmp->otyp == ROTATING_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(10);

	if (otmp->otyp == GOLD_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(2);
	if (otmp->otyp == GOLD_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(4);
	if (otmp->otyp == GOLD_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(6);
	if (otmp->otyp == GOLD_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(8);
	if (otmp->otyp == GOLD_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(10);

	if (otmp->otyp == SCRAP_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(2);
	if (otmp->otyp == SCRAP_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(4);
	if (otmp->otyp == SCRAP_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(6);
	if (otmp->otyp == SCRAP_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(8);
	if (otmp->otyp == SCRAP_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(10);

	if (otmp->otyp == FOAM_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(2);
	if (otmp->otyp == FOAM_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(4);
	if (otmp->otyp == FOAM_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(6);
	if (otmp->otyp == FOAM_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(8);
	if (otmp->otyp == FOAM_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(10);

	if (otmp->otyp == CLAY_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(2);
	if (otmp->otyp == CLAY_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(4);
	if (otmp->otyp == CLAY_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(6);
	if (otmp->otyp == CLAY_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(8);
	if (otmp->otyp == CLAY_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(10);

	if (otmp->otyp == COMPOST_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(2);
	if (otmp->otyp == COMPOST_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(4);
	if (otmp->otyp == COMPOST_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(6);
	if (otmp->otyp == COMPOST_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(8);
	if (otmp->otyp == COMPOST_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(10);

	if (otmp->otyp == PAPER_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(2);
	if (otmp->otyp == PAPER_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(4);
	if (otmp->otyp == PAPER_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(6);
	if (otmp->otyp == PAPER_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(8);
	if (otmp->otyp == PAPER_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(10);

	if (otmp->otyp == PLASTIC_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(2);
	if (otmp->otyp == PLASTIC_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(4);
	if (otmp->otyp == PLASTIC_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(6);
	if (otmp->otyp == PLASTIC_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(8);
	if (otmp->otyp == PLASTIC_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(10);

	if (otmp->otyp == WOOD_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(2);
	if (otmp->otyp == WOOD_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(4);
	if (otmp->otyp == WOOD_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(6);
	if (otmp->otyp == WOOD_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(8);
	if (otmp->otyp == WOOD_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(10);

	if (otmp->otyp == TAR_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(2);
	if (otmp->otyp == TAR_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(4);
	if (otmp->otyp == TAR_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(6);
	if (otmp->otyp == TAR_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(8);
	if (otmp->otyp == TAR_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(10);

	if (otmp->otyp == WONDER_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(2);
	if (otmp->otyp == WONDER_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(5);
	if (otmp->otyp == WONDER_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(7);
	if (otmp->otyp == WONDER_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(9);
	if (otmp->otyp == WONDER_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(12);

	if (otmp->otyp == LIQUID_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(2);
	if (otmp->otyp == LIQUID_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(5);
	if (otmp->otyp == LIQUID_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(7);
	if (otmp->otyp == LIQUID_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(9);
	if (otmp->otyp == LIQUID_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(12);

	if (otmp->otyp == NULL_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(2);
	if (otmp->otyp == NULL_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(5);
	if (otmp->otyp == NULL_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(7);
	if (otmp->otyp == NULL_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(9);
	if (otmp->otyp == NULL_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(12);

	if (otmp->otyp == GEMSTONE_LASH && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(2);
	if (otmp->otyp == GEMSTONE_LASH && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(5);
	if (otmp->otyp == GEMSTONE_LASH && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(7);
	if (otmp->otyp == GEMSTONE_LASH && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(9);
	if (otmp->otyp == GEMSTONE_LASH && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(12);

	if (otmp->otyp == SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(3);
	if (otmp->otyp == SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(6);
	if (otmp->otyp == SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(9);
	if (otmp->otyp == SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(12);
	if (otmp->otyp == SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(15);

	if (otmp->otyp == ANTIDOTE_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(3);
	if (otmp->otyp == ANTIDOTE_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(6);
	if (otmp->otyp == ANTIDOTE_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(9);
	if (otmp->otyp == ANTIDOTE_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(12);
	if (otmp->otyp == ANTIDOTE_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(15);

	if (otmp->otyp == CELESTIAL_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(3);
	if (otmp->otyp == CELESTIAL_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(6);
	if (otmp->otyp == CELESTIAL_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(9);
	if (otmp->otyp == CELESTIAL_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(12);
	if (otmp->otyp == CELESTIAL_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(15);

	if (otmp->otyp == ELYSIUM_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(3);
	if (otmp->otyp == ELYSIUM_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(6);
	if (otmp->otyp == ELYSIUM_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(9);
	if (otmp->otyp == ELYSIUM_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(12);
	if (otmp->otyp == ELYSIUM_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(15);

	if (otmp->otyp == GRANITE_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(3);
	if (otmp->otyp == GRANITE_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(6);
	if (otmp->otyp == GRANITE_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(9);
	if (otmp->otyp == GRANITE_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(12);
	if (otmp->otyp == GRANITE_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(15);

	if (otmp->otyp == DISGUSTING_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(3);
	if (otmp->otyp == DISGUSTING_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(6);
	if (otmp->otyp == DISGUSTING_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(9);
	if (otmp->otyp == DISGUSTING_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(12);
	if (otmp->otyp == DISGUSTING_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(15);

	if (otmp->otyp == CHROME_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(3);
	if (otmp->otyp == CHROME_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(6);
	if (otmp->otyp == CHROME_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(9);
	if (otmp->otyp == CHROME_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(12);
	if (otmp->otyp == CHROME_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(15);

	if (otmp->otyp == SHADOW_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(3);
	if (otmp->otyp == SHADOW_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(6);
	if (otmp->otyp == SHADOW_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(9);
	if (otmp->otyp == SHADOW_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(12);
	if (otmp->otyp == SHADOW_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(15);

	if (otmp->otyp == COPPER_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(3);
	if (otmp->otyp == COPPER_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(6);
	if (otmp->otyp == COPPER_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(9);
	if (otmp->otyp == COPPER_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(12);
	if (otmp->otyp == COPPER_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(15);

	if (otmp->otyp == SILK_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(3);
	if (otmp->otyp == SILK_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(6);
	if (otmp->otyp == SILK_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(9);
	if (otmp->otyp == SILK_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(12);
	if (otmp->otyp == SILK_SCOURGE && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(15);

	if (otmp->otyp == FLESH_LASH && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(3);
	if (otmp->otyp == FLESH_LASH && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(7);
	if (otmp->otyp == FLESH_LASH && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(10);
	if (otmp->otyp == FLESH_LASH && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(13);
	if (otmp->otyp == FLESH_LASH && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(17);

	if (otmp->otyp == NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(4);
	if (otmp->otyp == NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(8);
	if (otmp->otyp == NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(12);
	if (otmp->otyp == NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(16);
	if (otmp->otyp == NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(20);

	if (otmp->otyp == CONUNDRUM_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(4);
	if (otmp->otyp == CONUNDRUM_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(8);
	if (otmp->otyp == CONUNDRUM_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(12);
	if (otmp->otyp == CONUNDRUM_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(16);
	if (otmp->otyp == CONUNDRUM_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(20);

	if (otmp->otyp == CONCRETE_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(4);
	if (otmp->otyp == CONCRETE_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(8);
	if (otmp->otyp == CONCRETE_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(12);
	if (otmp->otyp == CONCRETE_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(16);
	if (otmp->otyp == CONCRETE_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(20);

	if (otmp->otyp == ELASTHAN_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(4);
	if (otmp->otyp == ELASTHAN_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(8);
	if (otmp->otyp == ELASTHAN_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(12);
	if (otmp->otyp == ELASTHAN_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(16);
	if (otmp->otyp == ELASTHAN_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(20);

	if (otmp->otyp == NANO_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(4);
	if (otmp->otyp == NANO_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(8);
	if (otmp->otyp == NANO_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(12);
	if (otmp->otyp == NANO_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(16);
	if (otmp->otyp == NANO_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(20);

	if (otmp->otyp == METEORIC_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(4);
	if (otmp->otyp == METEORIC_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(8);
	if (otmp->otyp == METEORIC_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(12);
	if (otmp->otyp == METEORIC_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(16);
	if (otmp->otyp == METEORIC_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(20);

	if (otmp->otyp == ZEBETITE_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(4);
	if (otmp->otyp == ZEBETITE_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(8);
	if (otmp->otyp == ZEBETITE_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(12);
	if (otmp->otyp == ZEBETITE_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(16);
	if (otmp->otyp == ZEBETITE_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(20);

	if (otmp->otyp == LEAD_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(4);
	if (otmp->otyp == LEAD_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(8);
	if (otmp->otyp == LEAD_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(12);
	if (otmp->otyp == LEAD_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(16);
	if (otmp->otyp == LEAD_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(20);

	if (otmp->otyp == SILVER_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(4);
	if (otmp->otyp == SILVER_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(8);
	if (otmp->otyp == SILVER_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(12);
	if (otmp->otyp == SILVER_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(16);
	if (otmp->otyp == SILVER_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(20);

	if (otmp->otyp == LEATHER_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(4);
	if (otmp->otyp == LEATHER_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(8);
	if (otmp->otyp == LEATHER_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(12);
	if (otmp->otyp == LEATHER_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(16);
	if (otmp->otyp == LEATHER_NUNCHIAKU && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(20);

	if (otmp->otyp == MITHRIL_LASH && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(4);
	if (otmp->otyp == MITHRIL_LASH && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(9);
	if (otmp->otyp == MITHRIL_LASH && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(14);
	if (otmp->otyp == MITHRIL_LASH && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(18);
	if (otmp->otyp == MITHRIL_LASH && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(22);

	if (otmp->otyp == HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(5);
	if (otmp->otyp == HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(10);
	if (otmp->otyp == HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(15);
	if (otmp->otyp == HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(20);
	if (otmp->otyp == HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(25);

	if (otmp->otyp == GLASS_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(5);
	if (otmp->otyp == GLASS_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(10);
	if (otmp->otyp == GLASS_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(15);
	if (otmp->otyp == GLASS_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(20);
	if (otmp->otyp == GLASS_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(25);

	if (otmp->otyp == MINERAL_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(5);
	if (otmp->otyp == MINERAL_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(10);
	if (otmp->otyp == MINERAL_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(15);
	if (otmp->otyp == MINERAL_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(20);
	if (otmp->otyp == MINERAL_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(25);

	if (otmp->otyp == ALLOY_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(5);
	if (otmp->otyp == ALLOY_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(10);
	if (otmp->otyp == ALLOY_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(15);
	if (otmp->otyp == ALLOY_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(20);
	if (otmp->otyp == ALLOY_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(25);

	if (otmp->otyp == FOAM_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(5);
	if (otmp->otyp == FOAM_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(10);
	if (otmp->otyp == FOAM_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(15);
	if (otmp->otyp == FOAM_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(20);
	if (otmp->otyp == FOAM_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(25);

	if (otmp->otyp == ELYSIUM_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(5);
	if (otmp->otyp == ELYSIUM_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(10);
	if (otmp->otyp == ELYSIUM_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(15);
	if (otmp->otyp == ELYSIUM_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(20);
	if (otmp->otyp == ELYSIUM_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(25);

	if (otmp->otyp == NUCLEAR_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(5);
	if (otmp->otyp == NUCLEAR_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(10);
	if (otmp->otyp == NUCLEAR_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(15);
	if (otmp->otyp == NUCLEAR_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(20);
	if (otmp->otyp == NUCLEAR_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(25);

	if (otmp->otyp == SAND_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(5);
	if (otmp->otyp == SAND_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(10);
	if (otmp->otyp == SAND_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(15);
	if (otmp->otyp == SAND_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(20);
	if (otmp->otyp == SAND_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(25);

	if (otmp->otyp == ETHER_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(5);
	if (otmp->otyp == ETHER_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(10);
	if (otmp->otyp == ETHER_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(15);
	if (otmp->otyp == ETHER_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(20);
	if (otmp->otyp == ETHER_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(25);

	if (otmp->otyp == OBSIDIAN_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED) ) bonus += rnd(5);
	if (otmp->otyp == OBSIDIAN_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT) ) bonus += rnd(10);
	if (otmp->otyp == OBSIDIAN_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_MASTER) ) bonus += rnd(15);
	if (otmp->otyp == OBSIDIAN_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER) ) bonus += rnd(20);
	if (otmp->otyp == OBSIDIAN_HOSTAGE_CHAIN && thick_skinned(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER) ) bonus += rnd(25);

	}

	    /* KMH -- Paddles are effective against insects */
	    if (otmp->otyp == FLY_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN))
		bonus += rnd(5);
	    if (otmp->otyp == VERMIN_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN))
		bonus += rnd(5);
	    if (otmp->otyp == INSECT_SQUASHER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN))
		bonus += rnd(12);
	    if (otmp->otyp == LASER_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN))
		bonus += rnd(15);

		if (!(PlayerCannotUseSkills)) {

	if (otmp->otyp == FLY_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_SKILLED) ) bonus += rnd(2);
	if (otmp->otyp == FLY_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_EXPERT) ) bonus += rnd(5);
	if (otmp->otyp == FLY_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_MASTER) ) bonus += rnd(7);
	if (otmp->otyp == FLY_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_GRAND_MASTER) ) bonus += rnd(10);
	if (otmp->otyp == FLY_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_SUPREME_MASTER) ) bonus += rnd(15);
	if (otmp->otyp == VERMIN_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_SKILLED) ) bonus += rnd(2);
	if (otmp->otyp == VERMIN_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_EXPERT) ) bonus += rnd(5);
	if (otmp->otyp == VERMIN_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_MASTER) ) bonus += rnd(7);
	if (otmp->otyp == VERMIN_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_GRAND_MASTER) ) bonus += rnd(10);
	if (otmp->otyp == VERMIN_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_SUPREME_MASTER) ) bonus += rnd(15);
	if (otmp->otyp == INSECT_SQUASHER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_SKILLED) ) bonus += rnd(4);
	if (otmp->otyp == INSECT_SQUASHER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_EXPERT) ) bonus += rnd(8);
	if (otmp->otyp == INSECT_SQUASHER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_MASTER) ) bonus += rnd(12);
	if (otmp->otyp == INSECT_SQUASHER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_GRAND_MASTER) ) bonus += rnd(16);
	if (otmp->otyp == INSECT_SQUASHER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_SUPREME_MASTER) ) bonus += rnd(24);
	if (otmp->otyp == LASER_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_SKILLED) ) bonus += rnd(10);
	if (otmp->otyp == LASER_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_EXPERT) ) bonus += rnd(20);
	if (otmp->otyp == LASER_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_MASTER) ) bonus += rnd(30);
	if (otmp->otyp == LASER_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_GRAND_MASTER) ) bonus += rnd(40);
	if (otmp->otyp == LASER_SWATTER && (ptr->mlet == S_ANT || ptr->mlet == S_SPIDER || ptr->mlet == S_XAN) && (P_SKILL(P_PADDLE) == P_SUPREME_MASTER) ) bonus += rnd(50);

		}

	    if (otmp->otyp == FLY_SWATTER && ptr == &mons[PM_INVINCIBLE_SUPERMAN]) bonus += rnd(25);
	    if (otmp->otyp == VERMIN_SWATTER && ptr == &mons[PM_INVINCIBLE_SUPERMAN]) bonus += rnd(25);
	    if (otmp->otyp == INSECT_SQUASHER && ptr == &mons[PM_INVINCIBLE_SUPERMAN]) bonus += rnd(50);
	    if (otmp->otyp == LASER_SWATTER && ptr == &mons[PM_INVINCIBLE_SUPERMAN]) bonus += rnd(75);

	    /* trident is highly effective against swimmers */
	    if (otmp->otyp == TRIDENT && is_swimmer(ptr)) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 4;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 2;
		   if (Race_if(PM_NEMESIS)) bonus += 20;
	    }

	    if (otmp->otyp == FOURDENT && is_swimmer(ptr)) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 4;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 2;
		   if (Race_if(PM_NEMESIS)) bonus += 20;
	    }

	    if (otmp->otyp == SHOOMDENT && is_swimmer(ptr)) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 4;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 2;
		   if (Race_if(PM_NEMESIS)) bonus += 20;
	    }

	    if (otmp->otyp == DIFFICULT_TRIDENT && is_swimmer(ptr)) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 4;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 2;
		   if (Race_if(PM_NEMESIS)) bonus += 20;
	    }

	    if (otmp->otyp == STYGIAN_PIKE && is_swimmer(ptr)) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 10;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 5;
		   if (Race_if(PM_NEMESIS)) bonus += 20;
	    }

	    if (otmp->otyp == MANCATCHER && is_swimmer(ptr)) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 10;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 5;
		   if (Race_if(PM_NEMESIS)) bonus += 20;
	    }

	    if (otmp->otyp == MARE_TRIDENT && is_swimmer(ptr)) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 10;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 5;
		   if (Race_if(PM_NEMESIS)) bonus += 20;
	    }

	    if (otmp->otyp == PLOW && is_swimmer(ptr)) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 10;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 5;
		   if (Race_if(PM_NEMESIS)) bonus += 20;
	    }

	    if (otmp->otyp == TWO_HANDED_TRIDENT && is_swimmer(ptr)) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 12;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 6;
		   if (Race_if(PM_NEMESIS)) bonus += 20;
	    }

		if (!(PlayerCannotUseSkills)) {

	    if (otmp->otyp == TRIDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_SKILLED) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 2;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 1;
	    }

	    if (otmp->otyp == FOURDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_SKILLED) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 2;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 1;
	    }

	    if (otmp->otyp == SHOOMDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_SKILLED) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 2;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 1;
	    }

	    if (otmp->otyp == DIFFICULT_TRIDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_SKILLED) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 2;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 1;
	    }

	    if (otmp->otyp == STYGIAN_PIKE && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_SKILLED) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 4;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 2;
	    }

	    if (otmp->otyp == MANCATCHER && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_SKILLED) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 4;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 2;
	    }

	    if (otmp->otyp == MARE_TRIDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_SKILLED) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 4;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 2;
	    }

	    if (otmp->otyp == PLOW && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_SKILLED) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 4;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 2;
	    }

	    if (otmp->otyp == TWO_HANDED_TRIDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_SKILLED) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 6;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 3;
	    }

	    if (otmp->otyp == TRIDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_EXPERT) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 4;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 2;
	    }

	    if (otmp->otyp == FOURDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_EXPERT) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 4;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 2;
	    }

	    if (otmp->otyp == SHOOMDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_EXPERT) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 4;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 2;
	    }

	    if (otmp->otyp == DIFFICULT_TRIDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_EXPERT) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 4;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 2;
	    }

	    if (otmp->otyp == STYGIAN_PIKE && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_EXPERT) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 8;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 4;
	    }

	    if (otmp->otyp == MANCATCHER && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_EXPERT) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 8;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 4;
	    }

	    if (otmp->otyp == MARE_TRIDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_EXPERT) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 8;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 4;
	    }

	    if (otmp->otyp == PLOW && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_EXPERT) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 8;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 4;
	    }

	    if (otmp->otyp == TWO_HANDED_TRIDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_EXPERT) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 12;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 6;
	    }

	    if (otmp->otyp == TRIDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_MASTER) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 8;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 4;
	    }

	    if (otmp->otyp == FOURDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_MASTER) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 8;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 4;
	    }

	    if (otmp->otyp == SHOOMDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_MASTER) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 8;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 4;
	    }

	    if (otmp->otyp == DIFFICULT_TRIDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_MASTER) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 8;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 4;
	    }

	    if (otmp->otyp == STYGIAN_PIKE && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_MASTER) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 16;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 8;
	    }

	    if (otmp->otyp == MANCATCHER && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_MASTER) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 16;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 8;
	    }

	    if (otmp->otyp == MARE_TRIDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_MASTER) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 16;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 8;
	    }

	    if (otmp->otyp == PLOW && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_MASTER) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 16;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 8;
	    }

	    if (otmp->otyp == TWO_HANDED_TRIDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_MASTER) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 24;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 12;
	    }

	    if (otmp->otyp == TRIDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_GRAND_MASTER) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 16;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 8;
	    }

	    if (otmp->otyp == FOURDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_GRAND_MASTER) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 16;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 8;
	    }

	    if (otmp->otyp == SHOOMDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_GRAND_MASTER) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 16;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 8;
	    }

	    if (otmp->otyp == DIFFICULT_TRIDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_GRAND_MASTER) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 16;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 8;
	    }

	    if (otmp->otyp == STYGIAN_PIKE && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_GRAND_MASTER) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 32;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 16;
	    }

	    if (otmp->otyp == MANCATCHER && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_GRAND_MASTER) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 32;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 16;
	    }

	    if (otmp->otyp == MARE_TRIDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_GRAND_MASTER) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 32;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 16;
	    }

	    if (otmp->otyp == PLOW && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_GRAND_MASTER) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 32;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 16;
	    }

	    if (otmp->otyp == TWO_HANDED_TRIDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_GRAND_MASTER) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 48;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 24;
	    }

	    if (otmp->otyp == TRIDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_SUPREME_MASTER) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 20;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 10;
	    }

	    if (otmp->otyp == FOURDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_SUPREME_MASTER) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 20;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 10;
	    }

	    if (otmp->otyp == SHOOMDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_SUPREME_MASTER) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 20;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 10;
	    }

	    if (otmp->otyp == DIFFICULT_TRIDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_SUPREME_MASTER) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 20;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 10;
	    }

	    if (otmp->otyp == STYGIAN_PIKE && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_SUPREME_MASTER) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 40;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 20;
	    }

	    if (otmp->otyp == MANCATCHER && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_SUPREME_MASTER) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 40;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 20;
	    }

	    if (otmp->otyp == MARE_TRIDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_SUPREME_MASTER) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 40;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 20;
	    }

	    if (otmp->otyp == PLOW && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_SUPREME_MASTER) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 40;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 20;
	    }

	    if (otmp->otyp == TWO_HANDED_TRIDENT && is_swimmer(ptr) && (P_SKILL(P_TRIDENT) == P_SUPREME_MASTER) ) {
		   if (is_waterypool(mon->mx, mon->my)) bonus += 60;
		   else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE) bonus += 30;
	    }

		}

	    /* blunt weapons versus undead (Diablo 2) */
	    if ((objects[otmp->otyp].oc_skill == P_PICK_AXE || objects[otmp->otyp].oc_skill == P_CLUB || objects[otmp->otyp].oc_skill == P_MACE || objects[otmp->otyp].oc_skill == P_PADDLE || objects[otmp->otyp].oc_skill == P_MORNING_STAR || otmp->otyp == FLAIL || otmp->otyp == KNOUT || otmp->otyp == FLOGGER || otmp->otyp == WEIGHTED_FLAIL || otmp->otyp == RIDING_CROP || otmp->otyp == ETHER_KNOUT || otmp->otyp == SPIKERACK || otmp->otyp == CHAIN_AND_SICKLE || otmp->otyp == OBSID || objects[otmp->otyp].oc_skill == P_HAMMER) && is_undead(ptr)) bonus += rnd(2);

		if (!(PlayerCannotUseSkills)) {

		if (objects[otmp->otyp].oc_skill == P_ORB && P_SKILL(P_QUARTERSTAFF) == P_BASIC) bonus += 1;
		if (objects[otmp->otyp].oc_skill == P_ORB && P_SKILL(P_QUARTERSTAFF) == P_SKILLED) bonus += rnd(2);
		if (objects[otmp->otyp].oc_skill == P_ORB && P_SKILL(P_QUARTERSTAFF) == P_EXPERT) bonus += rnd(3);
		if (objects[otmp->otyp].oc_skill == P_ORB && P_SKILL(P_QUARTERSTAFF) == P_MASTER) bonus += rnd(4);
		if (objects[otmp->otyp].oc_skill == P_ORB && P_SKILL(P_QUARTERSTAFF) == P_GRAND_MASTER) bonus += rnd(5);
		if (objects[otmp->otyp].oc_skill == P_ORB && P_SKILL(P_QUARTERSTAFF) == P_SUPREME_MASTER) bonus += rnd(6);

		if (objects[otmp->otyp].oc_skill == P_CLAW && P_SKILL(P_MARTIAL_ARTS) == P_BASIC) bonus += 1;
		if (objects[otmp->otyp].oc_skill == P_CLAW && P_SKILL(P_MARTIAL_ARTS) == P_SKILLED) bonus += rnd(2);
		if (objects[otmp->otyp].oc_skill == P_CLAW && P_SKILL(P_MARTIAL_ARTS) == P_EXPERT) bonus += rnd(3);
		if (objects[otmp->otyp].oc_skill == P_CLAW && P_SKILL(P_MARTIAL_ARTS) == P_MASTER) bonus += rnd(4);
		if (objects[otmp->otyp].oc_skill == P_CLAW && P_SKILL(P_MARTIAL_ARTS) == P_GRAND_MASTER) bonus += rnd(5);
		if (objects[otmp->otyp].oc_skill == P_CLAW && P_SKILL(P_MARTIAL_ARTS) == P_SUPREME_MASTER) bonus += rnd(6);

		if (objects[otmp->otyp].oc_skill == P_GRINDER && P_SKILL(P_AXE) == P_BASIC) bonus += 1;
		if (objects[otmp->otyp].oc_skill == P_GRINDER && P_SKILL(P_AXE) == P_SKILLED) bonus += rnd(2);
		if (objects[otmp->otyp].oc_skill == P_GRINDER && P_SKILL(P_AXE) == P_EXPERT) bonus += rnd(3);
		if (objects[otmp->otyp].oc_skill == P_GRINDER && P_SKILL(P_AXE) == P_MASTER) bonus += rnd(4);
		if (objects[otmp->otyp].oc_skill == P_GRINDER && P_SKILL(P_AXE) == P_GRAND_MASTER) bonus += rnd(5);
		if (objects[otmp->otyp].oc_skill == P_GRINDER && P_SKILL(P_AXE) == P_SUPREME_MASTER) bonus += rnd(6);

	    if ((objects[otmp->otyp].oc_skill == P_PICK_AXE || objects[otmp->otyp].oc_skill == P_CLUB || objects[otmp->otyp].oc_skill == P_MACE || objects[otmp->otyp].oc_skill == P_PADDLE || objects[otmp->otyp].oc_skill == P_MORNING_STAR || otmp->otyp == FLAIL || otmp->otyp == KNOUT || otmp->otyp == FLOGGER || otmp->otyp == WEIGHTED_FLAIL || otmp->otyp == RIDING_CROP || otmp->otyp == ETHER_KNOUT || otmp->otyp == SPIKERACK || otmp->otyp == CHAIN_AND_SICKLE || otmp->otyp == OBSID || objects[otmp->otyp].oc_skill == P_HAMMER) && is_undead(ptr) && (P_SKILL(objects[otmp->otyp].oc_skill) == P_SKILLED) ) bonus += rnd(2);
	    if ((objects[otmp->otyp].oc_skill == P_PICK_AXE || objects[otmp->otyp].oc_skill == P_CLUB || objects[otmp->otyp].oc_skill == P_MACE || objects[otmp->otyp].oc_skill == P_PADDLE || objects[otmp->otyp].oc_skill == P_MORNING_STAR || otmp->otyp == FLAIL || otmp->otyp == KNOUT || otmp->otyp == FLOGGER || otmp->otyp == WEIGHTED_FLAIL || otmp->otyp == RIDING_CROP || otmp->otyp == ETHER_KNOUT || otmp->otyp == SPIKERACK || otmp->otyp == CHAIN_AND_SICKLE || otmp->otyp == OBSID || objects[otmp->otyp].oc_skill == P_HAMMER) && is_undead(ptr) && (P_SKILL(objects[otmp->otyp].oc_skill) == P_EXPERT) ) bonus += rnd(4);
	    if ((objects[otmp->otyp].oc_skill == P_PICK_AXE || objects[otmp->otyp].oc_skill == P_CLUB || objects[otmp->otyp].oc_skill == P_MACE || objects[otmp->otyp].oc_skill == P_PADDLE || objects[otmp->otyp].oc_skill == P_MORNING_STAR || otmp->otyp == FLAIL || otmp->otyp == KNOUT || otmp->otyp == FLOGGER || otmp->otyp == WEIGHTED_FLAIL || otmp->otyp == RIDING_CROP || otmp->otyp == ETHER_KNOUT || otmp->otyp == SPIKERACK || otmp->otyp == CHAIN_AND_SICKLE || otmp->otyp == OBSID || objects[otmp->otyp].oc_skill == P_HAMMER) && is_undead(ptr) && (P_SKILL(objects[otmp->otyp].oc_skill) == P_MASTER) ) bonus += rnd(5);
	    if ((objects[otmp->otyp].oc_skill == P_PICK_AXE || objects[otmp->otyp].oc_skill == P_CLUB || objects[otmp->otyp].oc_skill == P_MACE || objects[otmp->otyp].oc_skill == P_PADDLE || objects[otmp->otyp].oc_skill == P_MORNING_STAR || otmp->otyp == FLAIL || otmp->otyp == KNOUT || otmp->otyp == FLOGGER || otmp->otyp == WEIGHTED_FLAIL || otmp->otyp == RIDING_CROP || otmp->otyp == ETHER_KNOUT || otmp->otyp == SPIKERACK || otmp->otyp == CHAIN_AND_SICKLE || otmp->otyp == OBSID || objects[otmp->otyp].oc_skill == P_HAMMER) && is_undead(ptr) && (P_SKILL(objects[otmp->otyp].oc_skill) == P_GRAND_MASTER) ) bonus += rnd(7);
	    if ((objects[otmp->otyp].oc_skill == P_PICK_AXE || objects[otmp->otyp].oc_skill == P_CLUB || objects[otmp->otyp].oc_skill == P_MACE || objects[otmp->otyp].oc_skill == P_PADDLE || objects[otmp->otyp].oc_skill == P_MORNING_STAR || otmp->otyp == FLAIL || otmp->otyp == KNOUT || otmp->otyp == FLOGGER || otmp->otyp == WEIGHTED_FLAIL || otmp->otyp == RIDING_CROP || otmp->otyp == ETHER_KNOUT || otmp->otyp == SPIKERACK || otmp->otyp == CHAIN_AND_SICKLE || otmp->otyp == OBSID || objects[otmp->otyp].oc_skill == P_HAMMER) && is_undead(ptr) && (P_SKILL(objects[otmp->otyp].oc_skill) == P_SUPREME_MASTER) ) bonus += rnd(10);

		}

	    if (objects[otmp->otyp].oc_skill == P_QUARTERSTAFF && is_undead(ptr)) bonus += rnd(6);
	    if (otmp->otyp == TWO_HANDED_FLAIL && is_undead(ptr)) bonus += rnd(6);

		if (!(PlayerCannotUseSkills)) {

	    if (objects[otmp->otyp].oc_skill == P_QUARTERSTAFF && is_undead(ptr) && (P_SKILL(P_QUARTERSTAFF) == P_SKILLED)) bonus += rnd(5);
	    if (objects[otmp->otyp].oc_skill == P_QUARTERSTAFF && is_undead(ptr) && (P_SKILL(P_QUARTERSTAFF) == P_EXPERT)) bonus += rnd(10);
	    if (objects[otmp->otyp].oc_skill == P_QUARTERSTAFF && is_undead(ptr) && (P_SKILL(P_QUARTERSTAFF) == P_MASTER)) bonus += rnd(15);
	    if (objects[otmp->otyp].oc_skill == P_QUARTERSTAFF && is_undead(ptr) && (P_SKILL(P_QUARTERSTAFF) == P_GRAND_MASTER)) bonus += rnd(25);
	    if (objects[otmp->otyp].oc_skill == P_QUARTERSTAFF && is_undead(ptr) && (P_SKILL(P_QUARTERSTAFF) == P_SUPREME_MASTER)) bonus += rnd(35);

	    if (otmp->otyp == TWO_HANDED_FLAIL && is_undead(ptr) && (P_SKILL(P_FLAIL) == P_SKILLED)) bonus += rnd(5);
	    if (otmp->otyp == TWO_HANDED_FLAIL && is_undead(ptr) && (P_SKILL(P_FLAIL) == P_EXPERT)) bonus += rnd(10);
	    if (otmp->otyp == TWO_HANDED_FLAIL && is_undead(ptr) && (P_SKILL(P_FLAIL) == P_MASTER)) bonus += rnd(15);
	    if (otmp->otyp == TWO_HANDED_FLAIL && is_undead(ptr) && (P_SKILL(P_FLAIL) == P_GRAND_MASTER)) bonus += rnd(25);
	    if (otmp->otyp == TWO_HANDED_FLAIL && is_undead(ptr) && (P_SKILL(P_FLAIL) == P_SUPREME_MASTER)) bonus += rnd(35);

		}

	    /* as well as silver bullets */
	    if (otmp->otyp == SILVER_BULLET && is_undead(ptr)) bonus += 8;

		if (!(PlayerCannotUseSkills)) {

	    if (otmp->otyp == SILVER_BULLET && is_undead(ptr) && (P_SKILL(P_FIREARM) == P_SKILLED)) bonus += 8;
	    if (otmp->otyp == SILVER_BULLET && is_undead(ptr) && (P_SKILL(P_FIREARM) == P_EXPERT)) bonus += 16;
	    if (otmp->otyp == SILVER_BULLET && is_undead(ptr) && (P_SKILL(P_FIREARM) == P_MASTER)) bonus += 24;
	    if (otmp->otyp == SILVER_BULLET && is_undead(ptr) && (P_SKILL(P_FIREARM) == P_GRAND_MASTER)) bonus += 32;
	    if (otmp->otyp == SILVER_BULLET && is_undead(ptr) && (P_SKILL(P_FIREARM) == P_SUPREME_MASTER)) bonus += 40;

		}

	    /* lances versus animals */
	    if (objects[otmp->otyp].oc_skill == P_LANCE && is_animal(ptr)) bonus += rnd(2);
	    if (otmp->otyp == SURVIVAL_KNIFE && is_animal(ptr)) bonus += rnd(2);

		if (!(PlayerCannotUseSkills)) {
			if (Race_if(PM_SWIKNI) && objects[otmp->otyp].oc_skill == P_KNIFE && (P_SKILL(P_KNIFE) == P_SKILLED)) bonus += rnd(2);
			if (Race_if(PM_SWIKNI) && objects[otmp->otyp].oc_skill == P_KNIFE && (P_SKILL(P_KNIFE) == P_EXPERT)) bonus += rnd(4);
			if (Race_if(PM_SWIKNI) && objects[otmp->otyp].oc_skill == P_KNIFE && (P_SKILL(P_KNIFE) == P_MASTER)) bonus += rnd(5);
			if (Race_if(PM_SWIKNI) && objects[otmp->otyp].oc_skill == P_KNIFE && (P_SKILL(P_KNIFE) == P_GRAND_MASTER)) bonus += rnd(7);
			if (Race_if(PM_SWIKNI) && objects[otmp->otyp].oc_skill == P_KNIFE && (P_SKILL(P_KNIFE) == P_SUPREME_MASTER)) bonus += rnd(8);

		}

		if (!(PlayerCannotUseSkills)) {

	    if (objects[otmp->otyp].oc_skill == P_LANCE && is_animal(ptr) && (P_SKILL(P_LANCE) == P_SKILLED)) bonus += rnd(2);
	    if (objects[otmp->otyp].oc_skill == P_LANCE && is_animal(ptr) && (P_SKILL(P_LANCE) == P_EXPERT)) bonus += rnd(3);
	    if (objects[otmp->otyp].oc_skill == P_LANCE && is_animal(ptr) && (P_SKILL(P_LANCE) == P_MASTER)) bonus += rnd(5);
	    if (objects[otmp->otyp].oc_skill == P_LANCE && is_animal(ptr) && (P_SKILL(P_LANCE) == P_GRAND_MASTER)) bonus += rnd(6);
	    if (objects[otmp->otyp].oc_skill == P_LANCE && is_animal(ptr) && (P_SKILL(P_LANCE) == P_SUPREME_MASTER)) bonus += rnd(8);
	    if (otmp->otyp == SURVIVAL_KNIFE && is_animal(ptr) && (P_SKILL(P_KNIFE) == P_SKILLED)) bonus += rnd(2);
	    if (otmp->otyp == SURVIVAL_KNIFE && is_animal(ptr) && (P_SKILL(P_KNIFE) == P_EXPERT)) bonus += rnd(3);
	    if (otmp->otyp == SURVIVAL_KNIFE && is_animal(ptr) && (P_SKILL(P_KNIFE) == P_MASTER)) bonus += rnd(5);
	    if (otmp->otyp == SURVIVAL_KNIFE && is_animal(ptr) && (P_SKILL(P_KNIFE) == P_GRAND_MASTER)) bonus += rnd(6);
	    if (otmp->otyp == SURVIVAL_KNIFE && is_animal(ptr) && (P_SKILL(P_KNIFE) == P_SUPREME_MASTER)) bonus += rnd(8);

		}

		if (uarmg && itemhasappearance(uarmg, APP_FENCING_GLOVES) ) {
			if ( (objects[otmp->otyp].oc_skill == P_SHORT_SWORD) || (objects[otmp->otyp].oc_skill == P_BROAD_SWORD) || (objects[otmp->otyp].oc_skill == P_LONG_SWORD) || (objects[otmp->otyp].oc_skill == P_TWO_HANDED_SWORD) || (objects[otmp->otyp].oc_skill == P_SCIMITAR) || (objects[otmp->otyp].oc_skill == P_SABER) || (objects[otmp->otyp].oc_skill == P_DAGGER) ) {
			bonus += Role_if(PM_FENCER) ? 2 : 1;
			if (P_SKILL(objects[otmp->otyp].oc_skill) >= P_BASIC) bonus += Role_if(PM_FENCER) ? 2 : 1;
			if (P_SKILL(objects[otmp->otyp].oc_skill) >= P_SKILLED) bonus += Role_if(PM_FENCER) ? 2 : 1;
			if (P_SKILL(objects[otmp->otyp].oc_skill) >= P_EXPERT) bonus += Role_if(PM_FENCER) ? 2 : 1;
			if (P_SKILL(objects[otmp->otyp].oc_skill) >= P_MASTER) bonus += Role_if(PM_FENCER) ? 2 : 1;
			if (P_SKILL(objects[otmp->otyp].oc_skill) >= P_GRAND_MASTER) bonus += Role_if(PM_FENCER) ? 2 : 1;
			if (P_SKILL(objects[otmp->otyp].oc_skill) >= P_SUPREME_MASTER) bonus += Role_if(PM_FENCER) ? 2 : 1;
			}
		}

		if (uarmh && uarmh->oartifact == ART_SALADIN_S_DESERT_FOX) {
			if ( (objects[otmp->otyp].oc_skill == P_SHORT_SWORD) || (objects[otmp->otyp].oc_skill == P_BROAD_SWORD) || (objects[otmp->otyp].oc_skill == P_LONG_SWORD) || (objects[otmp->otyp].oc_skill == P_TWO_HANDED_SWORD) || (objects[otmp->otyp].oc_skill == P_SCIMITAR) || (objects[otmp->otyp].oc_skill == P_SABER) || (objects[otmp->otyp].oc_skill == P_DAGGER) ) {
			bonus += 1;
			if (P_SKILL(objects[otmp->otyp].oc_skill) >= P_BASIC) bonus += 1;
			if (P_SKILL(objects[otmp->otyp].oc_skill) >= P_SKILLED) bonus += 1;
			if (P_SKILL(objects[otmp->otyp].oc_skill) >= P_EXPERT) bonus += 1;
			if (P_SKILL(objects[otmp->otyp].oc_skill) >= P_MASTER) bonus += 1;
			if (P_SKILL(objects[otmp->otyp].oc_skill) >= P_GRAND_MASTER) bonus += 1;
			if (P_SKILL(objects[otmp->otyp].oc_skill) >= P_SUPREME_MASTER) bonus += 1;
			}
		}

		if (uarmh && uarmh->oartifact == ART_WAR_MASK_OF_DURIN && (objects[otmp->otyp].oc_skill == P_AXE)) {
			bonus += 5;
		}

	    /* polearms versus golems */
	    if (objects[otmp->otyp].oc_skill == P_POLEARMS && (ptr->mlet == S_GOLEM || ptr->mlet == S_CENTAUR || ptr->mlet == S_UNICORN)) bonus += rnd(2);

		if (!(PlayerCannotUseSkills)) {

	    if (objects[otmp->otyp].oc_skill == P_POLEARMS && (ptr->mlet == S_GOLEM || ptr->mlet == S_CENTAUR || ptr->mlet == S_UNICORN) && (P_SKILL(P_POLEARMS) == P_SKILLED)) bonus += rnd(2);
	    if (objects[otmp->otyp].oc_skill == P_POLEARMS && (ptr->mlet == S_GOLEM || ptr->mlet == S_CENTAUR || ptr->mlet == S_UNICORN) && (P_SKILL(P_POLEARMS) == P_EXPERT)) bonus += rnd(4);
	    if (objects[otmp->otyp].oc_skill == P_POLEARMS && (ptr->mlet == S_GOLEM || ptr->mlet == S_CENTAUR || ptr->mlet == S_UNICORN) && (P_SKILL(P_POLEARMS) == P_MASTER)) bonus += rnd(6);
	    if (objects[otmp->otyp].oc_skill == P_POLEARMS && (ptr->mlet == S_GOLEM || ptr->mlet == S_CENTAUR || ptr->mlet == S_UNICORN) && (P_SKILL(P_POLEARMS) == P_GRAND_MASTER)) bonus += rnd(9);
	    if (objects[otmp->otyp].oc_skill == P_POLEARMS && (ptr->mlet == S_GOLEM || ptr->mlet == S_CENTAUR || ptr->mlet == S_UNICORN) && (P_SKILL(P_POLEARMS) == P_SUPREME_MASTER)) bonus += rnd(12);

		}

	    if ( (otmp->otyp == SICKLE || otmp->otyp == ELVEN_SICKLE || otmp->otyp == SCYTHE) && ptr->mlet == S_BAD_FOOD) bonus += rnd(10);

		if (!(PlayerCannotUseSkills)) {

	    if ( (otmp->otyp == SICKLE || otmp->otyp == ELVEN_SICKLE || otmp->otyp == SCYTHE) && ptr->mlet == S_BAD_FOOD && (P_SKILL(P_POLEARMS) == P_SKILLED)) bonus += rnd(8);
	    if ( (otmp->otyp == SICKLE || otmp->otyp == ELVEN_SICKLE || otmp->otyp == SCYTHE) && ptr->mlet == S_BAD_FOOD && (P_SKILL(P_POLEARMS) == P_EXPERT)) bonus += rnd(16);
	    if ( (otmp->otyp == SICKLE || otmp->otyp == ELVEN_SICKLE || otmp->otyp == SCYTHE) && ptr->mlet == S_BAD_FOOD && (P_SKILL(P_POLEARMS) == P_MASTER)) bonus += rnd(25);
	    if ( (otmp->otyp == SICKLE || otmp->otyp == ELVEN_SICKLE || otmp->otyp == SCYTHE) && ptr->mlet == S_BAD_FOOD && (P_SKILL(P_POLEARMS) == P_GRAND_MASTER)) bonus += rnd(34);
	    if ( (otmp->otyp == SICKLE || otmp->otyp == ELVEN_SICKLE || otmp->otyp == SCYTHE) && ptr->mlet == S_BAD_FOOD && (P_SKILL(P_POLEARMS) == P_SUPREME_MASTER)) bonus += rnd(44);
		}

	    /* electric sword versus quantum mechanic */
	    if (otmp->otyp == ELECTRIC_SWORD && ptr->mlet == S_QUANTMECH) bonus += rnd(10);

		if (!(PlayerCannotUseSkills)) {

	    if (otmp->otyp == ELECTRIC_SWORD && ptr->mlet == S_QUANTMECH && (P_SKILL(P_LONG_SWORD) == P_SKILLED)) bonus += rnd(8);
	    if (otmp->otyp == ELECTRIC_SWORD && ptr->mlet == S_QUANTMECH && (P_SKILL(P_LONG_SWORD) == P_EXPERT)) bonus += rnd(16);
	    if (otmp->otyp == ELECTRIC_SWORD && ptr->mlet == S_QUANTMECH && (P_SKILL(P_LONG_SWORD) == P_MASTER)) bonus += rnd(25);
	    if (otmp->otyp == ELECTRIC_SWORD && ptr->mlet == S_QUANTMECH && (P_SKILL(P_LONG_SWORD) == P_GRAND_MASTER)) bonus += rnd(34);
	    if (otmp->otyp == ELECTRIC_SWORD && ptr->mlet == S_QUANTMECH && (P_SKILL(P_LONG_SWORD) == P_SUPREME_MASTER)) bonus += rnd(44);
		}

	    /* shotgun versus bears or other quadrupeds */
	    if (otmp->otyp == SHOTGUN_SHELL && ptr->mlet == S_QUADRUPED) bonus += (10 + rnd(10));
	    if (otmp->otyp == LEAD_SHOT && ptr->mlet == S_QUADRUPED) bonus += (10 + rnd(10));

		if (!(PlayerCannotUseSkills)) {
	    if (otmp->otyp == SHOTGUN_SHELL && ptr->mlet == S_QUADRUPED && (P_SKILL(P_FIREARM) == P_SKILLED)) bonus += rnd(10);
	    if (otmp->otyp == SHOTGUN_SHELL && ptr->mlet == S_QUADRUPED && (P_SKILL(P_FIREARM) == P_EXPERT)) bonus += rnd(22);
	    if (otmp->otyp == SHOTGUN_SHELL && ptr->mlet == S_QUADRUPED && (P_SKILL(P_FIREARM) == P_MASTER)) bonus += rnd(36);
	    if (otmp->otyp == SHOTGUN_SHELL && ptr->mlet == S_QUADRUPED && (P_SKILL(P_FIREARM) == P_GRAND_MASTER)) bonus += rnd(54);
	    if (otmp->otyp == SHOTGUN_SHELL && ptr->mlet == S_QUADRUPED && (P_SKILL(P_FIREARM) == P_SUPREME_MASTER)) bonus += rnd(72);

	    if (otmp->otyp == LEAD_SHOT && ptr->mlet == S_QUADRUPED && (P_SKILL(P_FIREARM) == P_SKILLED)) bonus += rnd(10);
	    if (otmp->otyp == LEAD_SHOT && ptr->mlet == S_QUADRUPED && (P_SKILL(P_FIREARM) == P_EXPERT)) bonus += rnd(22);
	    if (otmp->otyp == LEAD_SHOT && ptr->mlet == S_QUADRUPED && (P_SKILL(P_FIREARM) == P_MASTER)) bonus += rnd(36);
	    if (otmp->otyp == LEAD_SHOT && ptr->mlet == S_QUADRUPED && (P_SKILL(P_FIREARM) == P_GRAND_MASTER)) bonus += rnd(54);
	    if (otmp->otyp == LEAD_SHOT && ptr->mlet == S_QUADRUPED && (P_SKILL(P_FIREARM) == P_SUPREME_MASTER)) bonus += rnd(72);
		}

	    /* axes versus umber hulks */
	    if (objects[otmp->otyp].oc_skill == P_AXE && ptr->mlet == S_UMBER) bonus += rnd(2);

		if (!(PlayerCannotUseSkills)) {
	    if (objects[otmp->otyp].oc_skill == P_AXE && ptr->mlet == S_UMBER && (P_SKILL(P_AXE) == P_SKILLED)) bonus += rnd(2);
	    if (objects[otmp->otyp].oc_skill == P_AXE && ptr->mlet == S_UMBER && (P_SKILL(P_AXE) == P_EXPERT)) bonus += rnd(5);
	    if (objects[otmp->otyp].oc_skill == P_AXE && ptr->mlet == S_UMBER && (P_SKILL(P_AXE) == P_MASTER)) bonus += rnd(10);
	    if (objects[otmp->otyp].oc_skill == P_AXE && ptr->mlet == S_UMBER && (P_SKILL(P_AXE) == P_GRAND_MASTER)) bonus += rnd(17);
	    if (objects[otmp->otyp].oc_skill == P_AXE && ptr->mlet == S_UMBER && (P_SKILL(P_AXE) == P_SUPREME_MASTER)) bonus += rnd(25);
		}

	    /* whips for lashing people's asses :P */
	    if (objects[otmp->otyp].oc_skill == P_WHIP && ptr->mlet == S_HUMAN) bonus += 1;

		if (!(PlayerCannotUseSkills)) {
	    if (objects[otmp->otyp].oc_skill == P_WHIP && ptr->mlet == S_HUMAN && (P_SKILL(P_WHIP) == P_SKILLED)) bonus += 1;
	    if (objects[otmp->otyp].oc_skill == P_WHIP && ptr->mlet == S_HUMAN && (P_SKILL(P_WHIP) == P_EXPERT)) bonus += 2;
	    if (objects[otmp->otyp].oc_skill == P_WHIP && ptr->mlet == S_HUMAN && (P_SKILL(P_WHIP) == P_MASTER)) bonus += 3;
	    if (objects[otmp->otyp].oc_skill == P_WHIP && ptr->mlet == S_HUMAN && (P_SKILL(P_WHIP) == P_GRAND_MASTER)) bonus += 4;
	    if (objects[otmp->otyp].oc_skill == P_WHIP && ptr->mlet == S_HUMAN && (P_SKILL(P_WHIP) == P_SUPREME_MASTER)) bonus += 5;
		}

		if (objects[otmp->otyp].oc_skill == P_WHIP && uarmg && uarmg->oartifact == ART_FIFTY_SHADES_OF_FUCKED_UP) bonus += 10;

	    /* high heels will absolutely wreck the elder priest */
	    if ((otmp->otyp == WEDGED_LITTLE_GIRL_SANDAL || otmp->otyp == STURDY_PLATEAU_BOOT_FOR_GIRLS || otmp->otyp == HUGGING_BOOT || otmp->otyp == BLOCK_HEELED_COMBAT_BOOT || otmp->otyp == WOODEN_GETA || otmp->otyp == LACQUERED_DANCING_SHOE || otmp->otyp == HIGH_HEELED_SANDAL || otmp->otyp == SEXY_LEATHER_PUMP || otmp->otyp == STICKSHOE || otmp->otyp == SPIKED_BATTLE_BOOT || otmp->otyp == INKA_BOOT || otmp->otyp == SOFT_LADY_SHOE || otmp->otyp == STEEL_CAPPED_SANDAL || otmp->otyp == BLOCK_HEELED_SANDAL || otmp->otyp == PROSTITUTE_SHOE || otmp->otyp == DOGSHIT_BOOT) && (mon->data == &mons[PM_DNETHACK_ELDER_PRIEST_TM_])) bonus += 50;

	    if ((otmp->otyp == WEDGED_LITTLE_GIRL_SANDAL || otmp->otyp == STURDY_PLATEAU_BOOT_FOR_GIRLS || otmp->otyp == HUGGING_BOOT || otmp->otyp == BLOCK_HEELED_COMBAT_BOOT || otmp->otyp == WOODEN_GETA || otmp->otyp == LACQUERED_DANCING_SHOE || otmp->otyp == HIGH_HEELED_SANDAL || otmp->otyp == SEXY_LEATHER_PUMP || otmp->otyp == STICKSHOE || otmp->otyp == SPIKED_BATTLE_BOOT || otmp->otyp == INKA_BOOT || otmp->otyp == SOFT_LADY_SHOE || otmp->otyp == STEEL_CAPPED_SANDAL || otmp->otyp == BLOCK_HEELED_SANDAL || otmp->otyp == PROSTITUTE_SHOE || otmp->otyp == DOGSHIT_BOOT) && (attacktype(mon->data, AT_TENT))) {
			bonus += 2;
			if (otmp && otmp->spe > 0) bonus += otmp->spe;
		}

	    /* if the weapon is going to get a double damage bonus, adjust
	       this bonus so that effectively it's added after the doubling
		 Amy edit: "double" damage is no longer double damage, and therefore we'll only halve it half of the time */
	    if (bonus > 1 && otmp->oartifact && !rn2(2) && spec_dbon(otmp, mon, 25) >= 25)
		bonus = (bonus + 1) / 2;

	    tmp += bonus;
	}

	if (tmp > 0) {
		/* It's debateable whether a rusted blunt instrument
		   should do less damage than a pristine one, since
		   it will hit with essentially the same impact, but
		   there ought to some penalty for using damaged gear
		   so always subtract erosion even for blunt weapons. */
		tmp -= greatest_erosionX(otmp);
		if (tmp < 1) tmp = 1;
	}

	if (Race_if(PM_SWIKNI) && otmp && objects[otmp->otyp].oc_skill == P_KNIFE) { /* not -P_KNIFE --Amy */
		if ((!rn2(30 - (Luck*2))) && !(uarmc && uarmc->oartifact == ART_ROKKO_CHAN_S_SUIT)) { /* nice patch - critical hits --Amy */

			pline("Critical hit!");
			u.cnd_criticalcount++;
			tmp *= 2;

		}

	} else {
		if ((!rn2(100 - (Luck*2))) && !(uarmc && uarmc->oartifact == ART_ROKKO_CHAN_S_SUIT)) { /* nice patch - critical hits --Amy */

			pline("Critical hit!");
			u.cnd_criticalcount++;
			tmp *= 2;

		}
	}

	if (Race_if(PM_JAPURA) && (is_angbandmonster(mon->data) || is_cowmonster(mon->data) || is_animemonster(mon->data) || is_steammonster(mon->data) || is_dlordsmonster(mon->data) || is_dnethackmonster(mon->data) || is_jokemonster(mon->data) || is_diablomonster(mon->data) || is_jonadabmonster(mon->data) || is_evilpatchmonster(mon->data) || is_elonamonster(mon->data) ) ) {
		tmp *= 2;
	}

	/*if (tmp > 127) tmp = 127;*/ /* sanity check --Amy ... but I think it's not needed? */

	return(tmp);
}

#endif /* OVLB */
#ifdef OVL0

STATIC_DCL struct obj *oselect(struct monst *,int);
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
			(!touch_petrifies(&mons[otmp->corpsenm]) || (otmp->corpsenm == PM_PLAYERMON) || Stoned) ) &&
                    (!is_lightsaber(otmp) || otmp->age) &&
		    (!otmp->oartifact || touch_artifact(otmp,mtmp)))
		return otmp;
	}
	return (struct obj *)0;
}


/* WAC added the Ya, increased priority of silver dagger, added Spoon */
/* KMH -- added bullets */
static NEARDATA const int rwep[] =
{	
	DART_OF_DISINTEGRATION, /* supreme because it can instakill the player --Amy */

	EVIL_VARIANT_STONE, COMPLETELY_BAD_PART_STONE,
	RIGHT_MOUSE_BUTTON_STONE, DISPLAY_LOSS_STONE, SPELL_LOSS_STONE, YELLOW_SPELL_STONE,
	AUTO_DESTRUCT_STONE, MEMORY_LOSS_STONE, INVENTORY_LOSS_STONE, BLACKY_STONE, MENU_BUG_STONE,
	SPEEDBUG_STONE, SUPERSCROLLER_STONE, FREE_HAND_BUG_STONE, UNIDENTIFY_STONE, STONE_OF_THIRST,
	UNLUCKY_STONE, SHADES_OF_GREY_STONE, STONE_OF_FAINTING, STONE_OF_CURSING, STONE_OF_DIFFICULTY,
	DEAFNESS_STONE, ANTIMAGIC_STONE, WEAKNESS_STONE, ROT_THIRTEEN_STONE, BISHOP_STONE,
	CONFUSION_STONE, DROPBUG_STONE, DSTW_STONE, STATUS_STONE, ROTTEN_STONE, UNSKILLED_STONE,
	ALIGNMENT_STONE, STAIRSTRAP_STONE, UNINFORMATION_STONE, EGOSTONE, FAST_FORWARD_STONE,
	FARLOOK_STONE, RESPAWN_STONE, CAPTCHA_STONE, METABOLIC_STONE, STONE_OF_NO_RETURN, TIME_USE_STONE,
 	NONSACRED_STONE, STARVATION_STONE, DROPLESS_STONE, LOW_EFFECT_STONE, INVISO_STONE,
 	GHOSTLY_STONE, DEHYDRATING_STONE, STONE_OF_HATE, DIRECTIONAL_SWAP_STONE, NONINTRINSICAL_STONE,
 	DROPCURSE_STONE, STONE_OF_NAKED_STRIPPING, ANTILEVEL_STONE, BAD_PART_STONE,
 	STEALER_STONE, REBEL_STONE, SHIT_STONE, STONE_OF_MISFIRING, STONE_OF_PERMANENCE,

	ORANGE_SPELL_STONE, VIOLET_SPELL_STONE, LONGING_STONE, CURSED_PART_STONE, QUAVERSAL_STONE,
	APPEARANCE_SHUFFLING_STONE, BROWN_SPELL_STONE, CHOICELESS_STONE, GOLDSPELL_STONE, DEPROVEMENT_STONE,
	INITIALIZATION_STONE, GUSHLUSH_STONE, SOILTYPE_STONE, DANGEROUS_TERRAIN_STONE, FALLOUT_STONE, MOJIBAKE_STONE,
	GRAVATION_STONE, UNCALLED_STONE, EXPLODING_DICE_STONE, PERMACURSE_STONE, SHROUDED_IDENTITY_STONE,
	FEELER_GAUGE_STONE, LONG_SCREWUP_STONE, WING_YELLOW_STONE, LIFE_SAVING_STONE, CURSEUSE_STONE, CUT_NUTRITION_STONE,
	SKILL_LOSS_STONE, AUTOPILOT_STONE, FORCE_STONE, MONSTER_GLYPH_STONE, CHANGING_DIRECTIVE_STONE,
	CONTAINER_KABOOM_STONE, STEAL_DEGRADE_STONE, LEFT_INVENTORY_STONE, FLUCTUATING_SPEED_STONE,
	TARMUSTROKINGNORA_STONE, FAILURE_STONE, BRIGHT_CYAN_SPELL_STONE, FREQUENTATION_SPAWN_STONE, PET_AI_STONE,
	SATAN_STONE, REMEMBERANCE_STONE, POKELIE_STONE, AUTOPICKUP_STONE, DYWYPI_STONE, SILVER_SPELL_STONE,
	METAL_SPELL_STONE, PLATINUM_SPELL_STONE, MANLER_STONE, DOORNING_STONE, NOWNSIBLE_STONE, ELM_STREET_STONE,
	MONNOISE_STONE, RANG_CALL_STONE, RECURRING_SPELL_LOSS_STONE, ANTI_TRAINING_STONE, TECHOUT_STONE,
	STAT_DECAY_STONE, MOVEMORK_STONE, SANE_TREBLE_STONE, STATCREASE_STONE, SIMEOUT_STONE,

	LOOTCUT_STONE, MONSTER_SPEED_STONE, SCALING_STONE, INIMICAL_STONE, WHITE_SPELL_STONE,
	GREYOUT_STONE, QUASAR_STONE, MOMMY_STONE, HORROR_STONE, ARTIFICIAL_STONE, WEREFORM_STONE,
	ANTIPRAYER_STONE, EVIL_PATCH_STONE, HARD_MODE_STONE, SECRET_ATTACK_STONE, EATER_STONE, COVETOUS_STONE,
	NON_SEEING_STONE, DARKMODE_STONE, UNFINDABLE_STONE, HOMICIDE_STONE, MULTITRAPPING_STONE, WAKEUP_CALL_STONE,
	GRAYOUT_STONE, GRAY_CENTER_STONE, CHECKERBOARD_STONE, CLOCKWISE_STONE, COUNTERCLOCKWISE_STONE,
	LAG_STONE, BLESSCURSE_STONE, DELIGHT_STONE, DISCHARGE_STONE, TRASH_STONE, FILTERING_STONE,
	DEFORMATTING_STONE, FLICKER_STRIP_STONE, UNDRESSING_STONE, HYPER_BLUE_STONE, NO_LIGHT_STONE,
	PARANOIA_STONE, FLEECE_STONE, INTERRUPTION_STONE, DUSTBIN_STONE, BATTERY_STONE, BUTTERFINGER_STONE,
	MISCASTING_STONE, MESSAGE_SUPPRESSION_STONE, STUCK_ANNOUNCEMENT_STONE, STORM_STONE, MAXIMUM_DAMAGE_STONE,
	LATENCY_STONE, STARLIT_SKY_STONE, TRAP_KNOWLEDGE_STONE, HIGHSCORE_STONE, PINK_SPELL_STONE,
	GREEN_SPELL_STONE, EVC_STONE, UNDERLAID_STONE, DAMAGE_METER_STONE, WEIGHT_STONE, INFOFUCK_STONE,
	BLACK_SPELL_STONE, CYAN_SPELL_STONE, HEAP_STONE, BLUE_SPELL_STONE, TRON_STONE, RED_SPELL_STONE,
	TOO_HEAVY_STONE, ELONGATED_STONE, WRAPOVER_STONE, DESTRUCTION_STONE, MELEE_PREFIX_STONE,
	AUTOMORE_STONE, UNFAIR_ATTACK_STONE,

	DISCONNECT_STONE, SCREW_STONE, BOSSFIGHT_STONE, ENTIRE_LEVEL_STONE, BONE_STONE,
	AUTOCURSE_STONE, HIGHLEVEL_STONE, SPELL_MEMORY_STONE, SOUND_EFFECT_STONE,
	AMNESIA_STONE, BIGSCRIPT_STONE, BANK_STONE, MAP_STONE, TECHNIQUE_STONE, DISENCHANTMENT_STONE,
	VERISIERT_STONE, CHAOS_TERRAIN_STONE, MUTENESS_STONE, ENGRAVING_STONE, MAGIC_DEVICE_STONE,
	BOOK_STONE, LEVEL_STONE, QUIZ_STONE, LOW_STAT_STONE, TRAINING_STONE, EXERCISE_STONE,
	TURN_LIMIT_STONE, WEAK_SIGHT_STONE, CHATTER_STONE,

	STONE_OF_INTRINSIC_LOSS, BLOOD_LOSS_STONE, BAD_EFFECT_STONE, TRAP_CREATION_STONE,
	STONE_OF_VULNERABILITY, ITEM_TELEPORTING_STONE, NASTY_STONE,

	SPOON,
	BFG_AMMO, FRAG_GRENADE, GAS_GRENADE, ROCKET,
	LASER_BEAM, HEAVY_BLASTER_BOLT, BLASTER_BOLT,
	ANTIMATTER_BULLET, SILVER_BULLET, LEAD_BULLET, BULLET, LEAD_SHOT, SHOTGUN_SHELL,
	CERAMIC_SPEAR, LOADBOULDER, STARLIGHTSTONE, HOMING_TORPEDO, BRICK_MISSILE,
	COURSE_JAVELIN, SPIRIT_THROWER, DROVEN_SPEAR, DRAGON_SPEAR, 
	DWARVISH_SPEAR, SILVER_SPEAR, INKA_SPEAR, ELVEN_SPEAR, ASBESTOS_JAVELIN, STACK_JAVELIN, WINDMILL_BLADE,
	FLINT_SPEAR, BRONZE_SPEAR, LONG_STAKE, BAMBOO_SPEAR, RANDOSPEAR, SILK_SPEAR, SPEAR, ORCISH_SPEAR, CRAPPY_SPEAR,
	JAVELIN, NEEDLE, MYSTERY_SHURIKEN, NANO_SHURIKEN, CUBIC_STAR, TAR_STAR, SHURIKEN,
	DROVEN_ARROW, GOLDEN_ARROW, ANCIENT_ARROW, BRONZE_ARROW,
	YA, SILVER_ARROW, DROVEN_DAGGER, ELVEN_ARROW, DARK_ELVEN_ARROW, ODOR_SHOT, FORBIDDEN_ARROW, WONDER_ARROW,
	ARROW, ORCISH_ARROW, PAPER_ARROW, DROVEN_BOLT, KOKKEN, MINERAL_BOLT,
	POISON_BOLT, HEAVY_CROSSBOW_BOLT, ETHER_BOLT, MATERIAL_BOLT,
	CROSSBOW_BOLT, INFERIOR_BOLT, FLEECE_BOLT, PIN_BOLT, GREAT_DAGGER, SILVER_DAGGER, ELVEN_DAGGER, 
	RADIOACTIVE_DAGGER, SECRETION_DAGGER, BRITTLE_SPEAR, SOFT_STAR,
	DARK_ELVEN_DAGGER, WONDER_DAGGER, DAGGER, ORCISH_DAGGER,
	SURVIVAL_KNIFE, COLLUSION_KNIFE, CERAMIC_KNIFE, BITUKNIFE, UNKNOWN_KNIFE, MEASURER, STILETTO, KNIFE, VICTIM_KNIFE,
	LOADSTONE, CONUNDRUM_NUGGET, SILVER_SLINGSTONE, VOLCANIC_GLASS_FRAGMENT, FLINT, 
	LEAD_CLUMP, SLING_AMMO, BONE_FRAGMENT,
	SMALL_PIECE_OF_UNREFINED_MITHR, TALC, GRAPHITE, ROCK, 
	STONE_OF_MAGIC_RESISTANCE, 

	SLEEPSTONE, MANASTONE,
	SALT_CHUNK, TOUCHSTONE, HEALTHSTONE, WHETSTONE, LUCKSTONE,
	INKA_STINGER, RANDOM_DART, FLIMSY_DART, DART, SPIKE, CALTROP,
	SAND_DART,
	/* BOOMERANG, */ CREAM_PIE
	/* note: CREAM_PIE should NOT be #ifdef KOPS */
};

static NEARDATA const int pwep[] =
{	PETRIFYIUM_BAR, DISINTEGRATION_BAR, NASTYPOLE, GREAT_POLLAX, WOODEN_BAR, TRAFFIC_LIGHT, GIANT_SCYTHE,
	THRESHER, YITH_TENTACLE, FORCE_PIKE, DROVEN_LANCE, HEAVY_GRINDER,
	CRYPTIC_POLE, HELMET_BEARD, POKER_STICK, COURSE_LANCE, HALBERD, BARDICHE, BRICKLAYER_BAR,
	INFERNAL_BARDICHE, INFERNAL_ANCUS, ZEBES_POLE,
	PHYSICIAN_BAR, DARK_BAR, POLE_LANTERN, SPETUM, BILL_GUISARME, CELESTIAL_POLE,
	NANO_POLE, BOHEMIAN_EAR_SPOON, VOULGE, RANSEUR, GUISARME, GRAIN_SCYTHE, LEADBAR, VARIED_GRINDER,
	BLACK_HALBERD, GARDEN_FORK, PITCHFORK, STICKSHOE, GRINDER,
	GLAIVE, LUCERN_HAMMER, BEC_DE_CORBIN, FAUCHARD, ETERNAL_POLE, LONG_POLE, SHARP_POLE, PENIS_POLE, PIKE,
	PARTISAN, ELVEN_LANCE, SCYTHE, ELVEN_SICKLE, SICKLE, SPEC_LANCE, BRONZE_LANCE, LANCE,
	NOOB_POLLAX, PARASOL, UMBRELLA, CHAIN_AND_SICKLE, SPINED_BALL, BLOW_AKLYS, AKLYS
};


static struct obj *propellor;

struct obj *
select_rwep(mtmp,polespecial)	/* select a ranged weapon for the monster */
register struct monst *mtmp;
boolean polespecial; /* may use polearm for monster-versus-monster combat */
{
	register struct obj *otmp;
	int i;

	char mlet = mtmp->data->mlet;

	propellor = &zeroobj;
	Oselect(EGG); /* cockatrice egg */
	if(mlet == S_KOP)	/* pies are first choice for Kops */
	    Oselect(CREAM_PIE);
	if(throws_rocks(mtmp->data))	/* ...boulders for giants */
	    Oselect(BOULDER);

	/* Select polearms first; they do more damage and aren't expendable */
	/* The limit of 13 here is based on the monster polearm range limit
	 * (defined as 5 in mthrowu.c).  5 corresponds to a distance of 2 in
	 * one direction and 1 in another; one space beyond that would be 3 in
	 * one direction and 2 in another; 3^2+2^2=13.
	 */
	if ((dist2(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) <= 13 && couldsee(mtmp->mx, mtmp->my)) || polespecial) {
	    for (i = 0; i < SIZE(pwep); i++) {
		/* Only strong monsters can wield big (esp. long) weapons.
		 * Big weapon is basically the same as bimanual.
		 * All monsters can wield the remaining weapons.
		 */

		/* Amy edit: Allow two-handed weapons for everyone, and allow undead/demons to equip silver */

		if (((/*strongmonst(mtmp->data) &&*/ (mtmp->misc_worn_check & W_ARMS) == 0)
			|| !objects[pwep[i]].oc_bimanual) /*&&
		    (objects[pwep[i]].oc_material != MT_SILVER
			|| !hates_silver(mtmp->data))*/) {
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

		/* Throwing nasty gray stones is always allowed. --Amy */
		for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj) {
			if (is_nastygraystone(otmp)) {
				propellor = m_carrying(mtmp, CATAPULT);
				if (!propellor) propellor = m_carrying(mtmp, INKA_SLING);
				if (!propellor) propellor = m_carrying(mtmp, SNIPESLING);
				if (!propellor) propellor = m_carrying(mtmp, METAL_SLING);
				if (!propellor) propellor = m_carrying(mtmp, SLING);
				return otmp;
			}
		}

	    /* shooting gems from slings; this goes just before the darts */
	    /* (shooting rocks is already handled via the rwep[] ordering) */
	    if ( (rwep[i] == DART || rwep[i] == SPIKE) && !likes_gems(mtmp->data) &&
		    (m_carrying(mtmp, SLING) || m_carrying(mtmp, METAL_SLING) || m_carrying(mtmp, INKA_SLING) || m_carrying(mtmp, SNIPESLING) || m_carrying(mtmp, CATAPULT)) ) {		/* propellor */
		for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
		    if (otmp->oclass == GEM_CLASS /*&&
			    (otmp->otyp != LOADSTONE || !otmp->cursed)*/) {
			/* I'll allow monsters to fire loadstones even if they are cursed.
			 * Yes, monsters are cheating bastards. --Amy */
			propellor = m_carrying(mtmp, CATAPULT);
			if (!propellor) propellor = m_carrying(mtmp, INKA_SLING);
			if (!propellor) propellor = m_carrying(mtmp, SNIPESLING);
			if (!propellor) propellor = m_carrying(mtmp, METAL_SLING);
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
		  if (!propellor) propellor = (oselect(mtmp, WILDHILD_BOW));
		  if (!propellor) propellor = (oselect(mtmp, HYDRA_BOW));
		  if (!propellor) propellor = (oselect(mtmp, YUMI));
		  if (!propellor) propellor = (oselect(mtmp, ELVEN_BOW));
		  /* WAC added dark elven bow */
		  if (!propellor) propellor = (oselect(mtmp, DARK_ELVEN_BOW));
		  if (!propellor) propellor = (oselect(mtmp, COMPOST_BOW));
		  if (!propellor) propellor = (oselect(mtmp, WIND_BOW));
		  if (!propellor) propellor = (oselect(mtmp, BLUE_BOW));
		  if (!propellor) propellor = (oselect(mtmp, META_BOW));
		  if (!propellor) propellor = (oselect(mtmp, BOW));
		  if (!propellor) propellor = (oselect(mtmp, ORCISH_BOW));
		  break;
		case P_SLING:
		  propellor = (oselect(mtmp, CATAPULT));
		  if (!propellor) propellor = (oselect(mtmp, INKA_SLING));
		  if (!propellor) propellor = (oselect(mtmp, SNIPESLING));
		  if (!propellor) propellor = (oselect(mtmp, METAL_SLING));
		  if (!propellor) propellor = (oselect(mtmp, SLING));
		  break;
		case P_CROSSBOW:
		  propellor = (oselect(mtmp, DEMON_CROSSBOW));
		  if (!propellor) propellor = (oselect(mtmp, ZOOM_SHOT_CROSSBOW));
		  if (!propellor) propellor = (oselect(mtmp, BALLISTA));
		  if (!propellor) propellor = (oselect(mtmp, HELO_CROSSBOW));
		  if (!propellor) propellor = (oselect(mtmp, POWER_CROSSBOW));
		  if (!propellor) propellor = (oselect(mtmp, DROVEN_CROSSBOW));
		  if (!propellor) propellor = (oselect(mtmp, PILE_BUNKER));
		  if (!propellor) propellor = (oselect(mtmp, CROSSBOW));
		case P_FIREARM:
		  if ((objects[rwep[i]].w_ammotyp) == WP_BULLET) {
			propellor = (oselect(mtmp, HEAVY_MACHINE_GUN));
			if (!propellor) propellor = (oselect(mtmp, KALASHNIKOV));
			if (!propellor) propellor = (oselect(mtmp, ASSAULT_RIFLE));
			if (!propellor) propellor = (oselect(mtmp, SUBMACHINE_GUN));
			if (!propellor) propellor = (oselect(mtmp, SNIPER_RIFLE));
			if (!propellor) propellor = (oselect(mtmp, HUNTING_RIFLE));
			if (!propellor) propellor = (oselect(mtmp, RIFLE));
			if (!propellor) propellor = (oselect(mtmp, BEAM_REFLECTOR_GUN));
			if (!propellor) propellor = (oselect(mtmp, PISTOL));
			if (!propellor) propellor = (oselect(mtmp, FLINTLOCK));
		  } else if ((objects[rwep[i]].w_ammotyp) == WP_SHELL) {
			propellor = (oselect(mtmp, AUTO_SHOTGUN));
			if (!propellor) propellor = (oselect(mtmp, SAWED_OFF_SHOTGUN));
			if (!propellor) propellor = (oselect(mtmp, SHOTGUN));
			if (!propellor) propellor = (oselect(mtmp, PAPER_SHOTGUN));
		  } else if ((objects[rwep[i]].w_ammotyp) == WP_BLASTER) {
			propellor = (oselect(mtmp, ARM_BLASTER));
			if (!propellor) propellor = (oselect(mtmp, PROCESS_CARD));
			if (!propellor) propellor = (oselect(mtmp, RAYGUN));
			if (!propellor) propellor = (oselect(mtmp, HAND_BLASTER));
			if (!propellor) propellor = (oselect(mtmp, CUTTING_LASER));
		  } else if ((objects[rwep[i]].w_ammotyp) == WP_BFG) {
			propellor = (oselect(mtmp, BFG));
		  } else if ((objects[rwep[i]].w_ammotyp) == WP_ROCKET) {
			propellor = (oselect(mtmp, ROCKET_LAUNCHER));
		  } else if ((objects[rwep[i]].w_ammotyp) == WP_GRENADE) {
			propellor = (oselect(mtmp, GRENADE_LAUNCHER));
			if (!propellor) propellor = &zeroobj;  /* can toss grenades */
		  }
		  break;
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
			/* Amy edit: or the only melee weapon (ok to throw from a stack of wielded daggers though,
			 * even if it means possibly throwing them all) */
			if ((otmp = oselect(mtmp, rwep[i])) && !otmp->oartifact
			    && ( (!otmp->cursed && !(otmp->quan == 1) ) || otmp != MON_WEP(mtmp)))
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
	  IMPOSSIBLY_HEAVY_IRON_BALL, IMPOSSIBLY_HEAVY_MINERAL_BALL, IMPOSSIBLY_HEAVY_GLASS_BALL,
	  IMPOSSIBLY_HEAVY_NUCLEAR_BALL, IMPOSSIBLY_HEAVY_ELYSIUM_BALL, IMPOSSIBLY_HEAVY_SAND_BALL,
	  IMPOSSIBLY_HEAVY_ETHER_BALL, IMPOSSIBLY_HEAVY_OBSIDIAN_BALL, IMPOSSIBLY_HEAVY_ALLOY_BALL,

	  BULKY_MITHRIL_BALL,

	  EXTREMELY_HEAVY_IRON_BALL, HEAVY_ELASTHAN_BALL, HEAVY_CONCRETE_BALL, HEAVY_CONUNDRUM_BALL, HEAVY_NANO_BALL,
	  HEAVY_LEAD_BALL, HEAVY_SILVER_BALL, HEAVY_METEORIC_BALL, HEAVY_ZEBETITE_BALL,

	  BULKY_FLESH_BALL,

	  HEAVY_LEATHER_BALL,

	  REALLY_HEAVY_IRON_BALL, DISGUSTING_BALL, HEAVY_ELYSIUM_BALL, HEAVY_GRANITE_BALL, HEAVY_CHROME_BALL,
	  HEAVY_SHADOW_BALL, HEAVY_COPPER_BALL, HEAVY_ANTIDOTE_BALL, HEAVY_CELESTIAL_BALL,

	  BULKY_GEMSTONE_BALL,

	  WONDER_BALL, NULL_BALL, LIQUID_BALL,

	  HEAVY_SILK_BALL,

	  QUITE_HEAVY_IRON_BALL, HEAVY_COMPOST_BALL, HEAVY_GOLD_BALL, HEAVY_CLAY_BALL, HEAVY_PAPER_BALL, HEAVY_PLASTIC_BALL,
	  HEAVY_WOOD_BALL, HEAVY_SCRAP_BALL, HEAVY_FOAM_BALL,

	  BULKY_POURPOOR_BALL,

	  HEAVY_TAR_BALL,

	  HEAVY_IRON_BALL, VERY_HEAVY_BALL, HEAVY_GLASS_BALL, HEAVY_STONE_BALL, HEAVY_BRICK_BALL, HEAVY_METAL_BALL,
	  HEAVY_WAX_BALL, HEAVY_ALLOY_BALL, HEAVY_BUBBLE_BALL,

	  HEAVY_CLOTH_BALL,

	  NUCLEAR_HOSTAGE_CHAIN, ELYSIUM_HOSTAGE_CHAIN, MINERAL_HOSTAGE_CHAIN, GLASS_HOSTAGE_CHAIN, HOSTAGE_CHAIN,
	  SAND_HOSTAGE_CHAIN, ETHER_HOSTAGE_CHAIN, OBSIDIAN_HOSTAGE_CHAIN, FOAM_HOSTAGE_CHAIN, ALLOY_HOSTAGE_CHAIN,
	  MITHRIL_LASH,

	  ELASTHAN_CHAIN, CONCRETE_NUNCHIAKU, CONUNDRUM_NUNCHIAKU, NUNCHIAKU, NANO_NUNCHIAKU, LEAD_NUNCHIAKU,
	  SILVER_NUNCHIAKU, METEORIC_NUNCHIAKU, ZEBETITE_NUNCHIAKU,
	  FLESH_LASH,

	  COLOSSUS_BLADE, THUNDER_HAMMER, STEEL_CAPPED_SANDAL,
	  BASTERD_SWORD, CHAINSWORD, TSURUGI, DROVEN_GREATSWORD, DWARVISH_BATTLE_AXE, TWO_HANDED_TRIDENT,
	  MANCATCHER, STYGIAN_PIKE, MARE_TRIDENT, PLOW, ETERNIUM_MATTOCK,
	  LOG, SLEDGE_HAMMER, WHITE_FLOWER_SWORD, VOLCANIC_BROADSWORD, ELEGANT_BROADSWORD,
	  BLACK_AESTIVALIS, RUNESWORD, MALLET, HEAVY_HAMMER, BRIDGE_MUZZLE,
	  WOODEN_GETA, LACQUERED_DANCING_SHOE, HIGH_HEELED_SANDAL, SEXY_LEATHER_PUMP, SPIKED_BATTLE_BOOT, HOMING_TORPEDO,
	  DWARVISH_MATTOCK, SOFT_MATTOCK, YATAGAN, CHROME_BLADE, AIR_PRESSURE_HAMMER,
	  BENT_SABLE, HOE_SABLE, MYTHICAL_SABLE, INKA_BOOT, SOFT_LADY_SHOE, MARBLE_CHUNK,
	  BLOCK_HEELED_SANDAL, PROSTITUTE_SHOE, LIGHTWHIP, ELECTRIC_CIGARETTE, NANO_HAMMER, LASER_SWATTER,
	  RED_DOUBLE_LIGHTSABER, WHITE_DOUBLE_LIGHTSABER, RED_LIGHTSABER,
	  MYSTERY_LIGHTSABER, BLUE_LIGHTSABER, VIOLET_LIGHTSABER, WHITE_LIGHTSABER, YELLOW_LIGHTSABER,
	  GREEN_LIGHTSABER, PIANO, DESERT_SWORD,
	  WEDGED_LITTLE_GIRL_SANDAL, SOFT_GIRL_SNEAKER, STURDY_PLATEAU_BOOT_FOR_GIRLS, HUGGING_BOOT,
	  BLOCK_HEELED_COMBAT_BOOT, ORGANOBLADE, GUITAR, DARK_HORN, SHADOWBLADE, ROMAN_SWORD, ETHER_SAW, SKY_HORN,
	  TWO_HANDED_SWORD, YESTERDAY_STAR, DEVIL_STAR, BATTLE_AXE, HUGE_CLUB, CHEMISTRY_SPACE_AXE, PLATINUM_SABER,
	  GOLDEN_SABER, ETERNIUM_SABER, CRYPTIC_SABER, TWO_HANDED_FLAIL, BOAT_OAR,
	  MASSIVE_STAFF, BATTLE_STAFF, BACKHAND_MACE, OLDEST_STAFF,
	  REINFORCED_MACE, OSBANE_KATANA, GRANITE_IMPALER, FLAME_MOUNTAIN, LEAD_FILLED_MACE,
	  AUTOMATIC_KATANA, KATANA, ARCANE_HORN, UNICORN_HORN, CRYSKNIFE, ELECTRIC_SWORD,
	  DIFFICULT_TRIDENT, SHOOMDENT, FOURDENT, TRIDENT, CRYSTAL_SWORD, HEAVY_GRINDER,
	  ICKY_BLADE, WILD_BLADE, HEAVY_LONG_SWORD, MAIN_SWORD, BRICK_MISSILE, VARIED_GRINDER,
	  LONG_SWORD, OBSID, COURSE_JAVELIN, SPIRIT_THROWER, IMPACT_STAFF, BO_STAFF, WALKING_STICK, SPIKERACK, CUDSWORD,
	  DROVEN_SPEAR, MEATSWORD, BIDENHANDER, DIAMOND_SMASHER, GRINDER,

	  DISGUSTING_CHAIN, GRANITE_SCOURGE, ELYSIUM_SCOURGE, SCOURGE, CHROME_SCOURGE, SHADOW_SCOURGE, COPPER_SCOURGE,
	  ANTIDOTE_SCOURGE, CELESTIAL_SCOURGE,
	  GEMSTONE_LASH,
	  WONDER_CHAIN, NULL_CHAIN, LIQUID_CHAIN,
	  LEATHER_NUNCHIAKU,
	  COMPOST_CHAIN, CLAY_CHAIN, GOLD_CHAIN, ROTATING_CHAIN, PAPER_CHAIN, PLASTIC_CHAIN, WOOD_CHAIN, SCRAP_CHAIN,
	  FOAM_CHAIN,
	  POURPOOR_LASH,
	  SILK_SCOURGE,
	  HEAVY_CHAIN, GLASS_CHAIN, STONE_CHAIN, IRON_CHAIN, BRICK_CHAIN, METAL_CHAIN, WAX_CHAIN, ALLOY_CHAIN,
	  TAR_CHAIN, BUBBLE_CHAIN,

	  INKUTLASS, GREEN_SABER, RAPIER, ARCANE_RAPIER, LEATHER_SABER,
	  DOGSHIT_BOOT, CHROME_STAFF, CONTRO_STAFF, CALCULATOR, ELITE_BATLETH, RAKUYO, CROW_QUILL,
	  PLATINUM_FIRE_HOOK, FIRE_HOOK, DROVEN_DAGGER, ELVEN_BROADSWORD, BROADSWORD, SCIMITAR, SILVER_SABER,
	  MILL_PAIL, FLANGED_MACE, BATLETH, BRONZE_MORNING_STAR, STONE_STAR,
	  SPINED_BALL, RHYTHMIC_STAR, JAGGED_STAR, STEEL_WHIP,
	  SEXPLAY_WHIP, SILVER_SHORT_SWORD, SILVER_LONG_SWORD, IRON_SABER, VIBROBLADE, INKA_BLADE, ETERNIUM_BLADE,
	  DROVEN_SHORT_SWORD, SILVER_MACE, ASTERISK, NANO_AXE, SHINY_STAR,
        METAL_STAR, ETHER_KNOUT, CERAMIC_SPEAR, INFERNAL_AXE,
  	  MORNING_STAR, GREAT_HOUCHOU, DARK_ELVEN_SHORT_SWORD, HIGH_ELVEN_WARSWORD, ELVEN_SHORT_SWORD, DARKNESS_CLUB,
  	  DWARVISH_SHORT_SWORD, SUGUHANOKEN, SHORT_SWORD, METAL_CLUB, KNOUT, RIDING_CROP, IRON_BAR, TUBING_PLIERS,
	  ORCISH_SHORT_SWORD, ELVEN_MACE, PLASTIC_MACE, BRONZE_MACE, HEATH_BALL, KAMEREL_VAJRA, QUALITY_AXE,
	  MACE, MOON_AXE, OBSIDIAN_AXE, SPIRIT_AXE, SHARP_AXE, AXE, DRAGON_SPEAR, DWARVISH_SPEAR, SILVER_SPEAR, BELL_CLAWS,
	  INKA_SPEAR, ELVEN_SPEAR, FLINT_SPEAR, NANO_PICK, SHORT_BLADE, SAND_SWORD, LIGHTBULB, CESTUS,
	  BRONZE_SPEAR, LONG_STAKE, BAMBOO_SPEAR, RANDOSPEAR,
	  SILK_SPEAR, SPEAR, ORCISH_SPEAR, CRAPPY_SPEAR, LOWER_ARM_BLADE, CAMO_QATAR,
	  STAR_ROD, RUNED_ROD, WEIGHTED_FLAIL, FLAIL, FLOGGER, CHAIN_AND_SICKLE, BULLWHIP, ASBESTOS_JAVELIN, STACK_JAVELIN,
	  FIRE_STICK, TROUTSTAFF, FLINDBAR, QUARTERSTAFF, RAIN_PIPE, SPECIAL_MOP, SILVER_KHAKKHARA, FOAMY_STAFF,
	  MASSAGER, INSECT_SQUASHER, SPIKED_CLUB, BRICK_PICK, BASEBALL_BAT, PAPER_SWORD, QATAR,
	  GREAT_DAGGER, JAVELIN, BLOW_AKLYS, AKLYS, POURED_CLUB, JAGGED_TOOTH_CLUB, TRASH_SWORD,
	  NATURAL_STICK, BONE_CLUB, CLUB, ALLOY_CLUB, CIGARETTE, BUBBLETAR, CONUNDRUM_PICK,
	  BRONZE_PICK, CONGLOMERATE_PICK, MYSTERY_PICK, PICK_AXE, VERMIN_SWATTER, FLY_SWATTER, TENNIS_RACKET,
	  RADIOACTIVE_DAGGER, SECRETION_DAGGER, BRITTLE_SPEAR, JARED_STONE,
	  TELEPHONE, FUTON_SWATTER, MAGICAL_PAINTBRUSH, BROOM, MOP, ORNATE_MACE,

	  HEAVENLY_WHIP, VIPERWHIP, ETHER_WHIP, BULLETPROOF_CHAINWHIP, INKA_SHACKLE, RUBBER_HOSE, SECRET_WHIP,
	  WAR_HAMMER, MITHRIL_WHIP, CHAINWHIP, FLAME_WHIP, ROSE_WHIP,
	  SILVER_DAGGER, ELVEN_DAGGER, WOODEN_STAKE, WONDER_DAGGER, DAGGER, 
	  ORCISH_DAGGER, NOVICE_HAMMER, RAZOR_WHIP,
	  MERCURIAL_ATHAME, ATHAME, SCALPEL, SURVIVAL_KNIFE, CERAMIC_KNIFE, BUBBLEHORN, EAGLE_BALL,
	  COLLUSION_KNIFE, BITUKNIFE, VICTIM_KNIFE, UNKNOWN_KNIFE,
	  MEASURER, STILETTO, KNIFE, TORCH, WORM_TOOTH, OTAMA, CARDBOARD_FAN,
	  CLOTH_CHAIN
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
			&& ((/*strong &&*/ !wearing_shield)
			    || !objects[otmp->otyp].oc_bimanual))
		    return otmp;
	}

	/* Amy edit: clubs are such weak weapons, why the hell would giants use those...
	 * In Soviet Russia, legendary beings always wield the crappiest weapons available even if there's tons of
	 * better alternatives, because the communist government bans them from using anything else. */
	if(issoviet && is_giant(mtmp->data))	/* giants just love to use clubs */
	    Oselect(CLUB);

	/* only strong monsters can wield big (esp. long) weapons */
	/* big weapon is basically the same as bimanual */
	/* all monsters can wield the remaining weapons */

	/* Amy edit: see above, allow two-handed and silver for everyone */

	for (i = 0; i < SIZE(hwep); i++) {
	    if (hwep[i] == CORPSE && !resists_ston(mtmp) && !(mtmp->misc_worn_check & W_ARMG))
		continue;
	    if (((/*strong &&*/ !wearing_shield)
			|| !objects[hwep[i]].oc_bimanual) /*&&
		    (objects[hwep[i]].oc_material != MT_SILVER
			|| !hates_silver(mtmp->data))*/)
		Oselect(hwep[i]);
	}

	/* After all, YOU can equip a silver saber in your bare hands as a vampire no problem. --Amy
	 * This is supposed to be a *difficult* variant of NetHack, so we have exactly zero reason to penalize monsters. */

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
	if (!attacktype(mon->data, AT_WEAP) && !mon->egotype_weaponizer) {
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
	if (!(mw_tmp && mw_tmp->cursed && mon->weapon_check == NO_WEAPON_WANTED))
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
			(void)select_rwep(mon, FALSE);
			obj = propellor;
			
			break;
		case NEED_PICK_AXE:
			obj = m_carrying(mon, PICK_AXE);
			if (!obj) obj = m_carrying(mon, CONUNDRUM_PICK);
			if (!obj) obj = m_carrying(mon, CONGLOMERATE_PICK);
			if (!obj) obj = m_carrying(mon, MYSTERY_PICK);
			if (!obj) obj = m_carrying(mon, BRONZE_PICK);
			if (!obj) obj = m_carrying(mon, BRICK_PICK);
			if (!obj) obj = m_carrying(mon, NANO_PICK);
			/* KMH -- allow other picks */
			if (!obj && !which_armor(mon, W_ARMS))
			    obj = m_carrying(mon, DWARVISH_MATTOCK);
			if (!obj && !which_armor(mon, W_ARMS))
			    obj = m_carrying(mon, SOFT_MATTOCK);
			if (!obj && !which_armor(mon, W_ARMS))
			    obj = m_carrying(mon, ETERNIUM_MATTOCK);
			break;
		case NEED_AXE:
			/* currently, only 2 types of axe */
			obj = m_carrying(mon, DWARVISH_BATTLE_AXE);
			if (!obj || which_armor(mon, W_ARMS))
			    obj = m_carrying(mon, BATTLE_AXE);
			if (!obj || which_armor(mon, W_ARMS))
			    obj = m_carrying(mon, CHEMISTRY_SPACE_AXE);
			if (!obj || which_armor(mon, W_ARMS))
			    obj = m_carrying(mon, MOON_AXE);
			if (!obj) obj = m_carrying(mon, OBSIDIAN_AXE);
			if (!obj) obj = m_carrying(mon, SPIRIT_AXE);
			if (!obj) obj = m_carrying(mon, SHARP_AXE);
			if (!obj) obj = m_carrying(mon, QUALITY_AXE);
			if (!obj) obj = m_carrying(mon, INFERNAL_AXE);
			if (!obj) obj = m_carrying(mon, NANO_AXE);
			if (!obj) obj = m_carrying(mon, TUBING_PLIERS);
			if (!obj) obj = m_carrying(mon, AXE);
			break;
		case NEED_PICK_OR_AXE:
			/* prefer pick for fewer switches on most levels */
			obj = m_carrying(mon, DWARVISH_MATTOCK);
			if (!obj) obj = m_carrying(mon, SOFT_MATTOCK);
			if (!obj) obj = m_carrying(mon, ETERNIUM_MATTOCK);
			if (!obj) obj = m_carrying(mon, DWARVISH_BATTLE_AXE);
			if (!obj) obj = m_carrying(mon, BATTLE_AXE);
			if (!obj || which_armor(mon, W_ARMS)) {
			    obj = m_carrying(mon, PICK_AXE);
			    if (!obj) obj = m_carrying(mon, CONUNDRUM_PICK);
			    if (!obj) obj = m_carrying(mon, CONGLOMERATE_PICK);
			    if (!obj) obj = m_carrying(mon, MYSTERY_PICK);
			    if (!obj) obj = m_carrying(mon, BRONZE_PICK);
			    if (!obj) obj = m_carrying(mon, BRICK_PICK);
			    if (!obj) obj = m_carrying(mon, NANO_PICK);
			    if (!obj) obj = m_carrying(mon, MOON_AXE);
			    if (!obj) obj = m_carrying(mon, CHEMISTRY_SPACE_AXE);
			    if (!obj) obj = m_carrying(mon, OBSIDIAN_AXE);
			    if (!obj) obj = m_carrying(mon, SPIRIT_AXE);
			    if (!obj) obj = m_carrying(mon, SHARP_AXE);
			    if (!obj) obj = m_carrying(mon, QUALITY_AXE);
			    if (!obj) obj = m_carrying(mon, INFERNAL_AXE);
			    if (!obj) obj = m_carrying(mon, NANO_AXE);
			    if (!obj) obj = m_carrying(mon, TUBING_PLIERS);
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
			if (is_lightsaber(obj))
			    mon_ignite_lightsaber(obj, mon);
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
			sprintf(welded_buf, "%s welded to %s %s",
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
		if (is_lightsaber(obj))
		    mon_ignite_lightsaber(obj, mon);
		return /*1*/0;
	}
	mon->weapon_check = NEED_WEAPON;
	return 0;
}

static void
mon_ignite_lightsaber(obj, mon)
struct obj * obj;
struct monst * mon;
{
	/* No obj or not lightsaber */
	if (!obj || !is_lightsaber(obj)) return;

	// for some reason, the lightsaber prototype is created with
	// age == 0
	if (obj->oartifact == ART_LIGHTSABER_PROTOTYPE)
		obj->age = 300L;
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
		if ((obj->otyp == RED_DOUBLE_LIGHTSABER || obj->otyp == WHITE_DOUBLE_LIGHTSABER) && !obj->altmode) {
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

/* STEPHEN WHITE'S NEW CODE */
int
abon()		/* attack bonus for strength & dexterity */
{
	int sbon;
	int str = ACURR(A_STR), dex = ACURR(A_DEX);

	/* [Tom] lowered these a little */        
	if (str < 6) sbon = -1;
	else if (str < 8) sbon = 0;
	else if (str < 13) sbon = 1;
	else if (str < 17) sbon = 2;
	else if (str <= STR18(50)) sbon = 3;	/* up to 18/50 */
	else if (str < STR18(100)) sbon = 4;
	else if (str == STR18(100)) sbon = 5;  /* 18/00 */
	else if (str == STR19(19)) sbon = 6;  /* 19 */
	else if (str == STR19(20)) sbon = 6;  /* 20 */
	else if (str == STR19(21)) sbon = 7;  /* 21 */
	else if (str == STR19(22)) sbon = 7;  /* 22 */
	else if (str == STR19(23)) sbon = 8;  /* 23 */
	else if (str == STR19(24)) sbon = 9;  /* 24 */
	else sbon = 10;
  
	if (dex < 5) sbon -= 1;
	else if (dex < 7) sbon += 0;
	else if (dex < 10) sbon += 1;
	else if (dex < 11) sbon += 2;
	else if (dex < 12) sbon += 3;
	else if (dex < 13) sbon += 4;
	else if (dex < 15) sbon += 5;
	else if (dex == 15) sbon += 6;  /* 15 */
	else if (dex == 16) sbon += 7;  /* 16 */
	else if (dex == 17) sbon += 7;  /* 17 */
	else if (dex == 18) sbon += 8;  /* 18 */
	else if (dex == 19) sbon += 9;  /* 19 */
	else if (dex == 20) sbon += 10;  /* 20 */
	else if (dex == 21) sbon += 11;  /* 21 */
	else if (dex == 22) sbon += 11;  /* 22 */
	else if (dex == 23) sbon += 12;  /* 23 */
	else if (dex == 24) sbon += 12;  /* 24 */
	else sbon += 13;

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

	if (!rn2(2) && GushLevel > 14) sbon += 1;
	if (u.menoraget && u.bookofthedeadget && u.silverbellget && rn2(3)) sbon += 1;
	if (!rn2(3) && u.chaoskeyget && u.neutralkeyget && u.lawfulkeyget) sbon += 1;
	if (!rn2(10) && u.medusaremoved) sbon += 1;
	if (!rn2(5) && u.luckstoneget && u.sokobanfinished && u.deepminefinished) sbon += 1;
	if (!rn2(5) && GushLevel > 10) sbon += 1;
	if (!rn2(5) && GushLevel > 17) sbon += 1;
	if (!rn2(5) && GushLevel > 23) sbon += 1;
	if (!rn2(5) && GushLevel > 29) sbon += 1;

	/* In Soviet Russia, strength and dexterity bonuses are lower because seriously, who needs bonuses??? --Amy */
	if (issoviet && sbon > 1) sbon /= 2;

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

	if (!issoviet) {

		if (str < 5) return(-2);
		else if (str < 8) return(-1);
		else if (str < 10) return(0);
		else if (str < 14) return(1);
		else if (str < 18) return(2);
		else if (str == 18) return(3);		/* up to 18 */
		else if (str < STR18(30)) return(4);          /* up to 18/99 */
		else if (str < STR18(66)) return(5);          /* up to 18/99 */
		else if (str < STR18(100)) return(6);          /* up to 18/99 */
		else if (str == STR18(100)) return(7);         /* 18/00 */
		else if (str == STR19(19)) return(8);         /* 19 */
		else if (str == STR19(20)) return(8);         /* 20 */
		else if (str == STR19(21)) return(9);         /* 21 */
		else if (str == STR19(22)) return(9);         /* 22 */
		else if (str == STR19(23)) return(10);         /* 23 */
		else if (str == STR19(24)) return(10);        /* 24 */
		else return(11);

	} else {

		if (str < 5) return(-5);
		else if (str < 8) return(-4);
		else if (str < 10) return(-3);
		else if (str < 14) return(-2);
		else if (str < 18) return(-1);
		else if (str == 18) return(0);		/* up to 18 */
		else if (str < STR18(30)) return(1);          /* up to 18/99 */
		else if (str < STR18(66)) return(1);          /* up to 18/99 */
		else if (str < STR18(100)) return(2);          /* up to 18/99 */
		else if (str == STR18(100)) return(2);         /* 18/00 */
		else if (str == STR19(19)) return(3);         /* 19 */
		else if (str == STR19(20)) return(3);         /* 20 */
		else if (str == STR19(21)) return(3);         /* 21 */
		else if (str == STR19(22)) return(4);         /* 22 */
		else if (str == STR19(23)) return(4);         /* 23 */
		else if (str == STR19(24)) return(4);        /* 24 */
		else return(5);

	}

}

/* copy the skill level name into the given buffer */
STATIC_OVL char *
skill_level_name(skill, buf)
int skill;
char *buf;
{
    const char *ptr;

    switch (P_SKILL(skill)) {
	case P_ISRESTRICTED:    ptr = "Restricted"; break;
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
	case P_SUPREME_MASTER: 
            if (skill <= P_LAST_WEAPON)
                    ptr = "World Champion" ;
            else if (skill <= P_LAST_SPELL)
                    ptr = "Rodney";
	    else if (skill <= P_LAST_H_TO_H) 
	    	    ptr = "Supreme Master"; 
	    else ptr = "Unprecedented";
	    break;
	default:	     ptr = "Unknown";	break;
    }
    strcpy(buf, ptr);
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
	case P_ISRESTRICTED:    ptr = "(max Restricted)"; break;
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
	case P_SUPREME_MASTER: 
            if (skill <= P_LAST_WEAPON)
                    ptr = "(max World Champion)" ;
            else if (skill <= P_LAST_SPELL)
                    ptr = "(max Rodney)";
	    else if (skill <= P_LAST_H_TO_H) 
	    	    ptr = "(max Supreme Master)"; 
	    else ptr = "(max Unprecedented)";
	    break;
	default:	     ptr = "(max Unknown)";	break;
    }
    strcpy(buf, ptr);
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

	/* lightsaber forms would devour way too many skill points for little benefit, unless the lightsaber actually is
	 * your main weapon... jedi are meant to be very good with lightsabers anyway, and other roles shouldn't feel like
	 * they're throwing their skill points away, so there's now a chance of not using up a skill point for those skills
	 * In Soviet Russia, every skill always costs a skill point because that's communism for you. No exceptions will be
	 * made. If you don't agree with that, move to a non-communist country :P --Amy */

	if (skill == P_SHII_CHO && (P_SKILL(P_SHII_CHO) + 1) > u.lightsabermax1) {
		u.lightsabermax1 = (P_SKILL(P_SHII_CHO) + 1);
		if (!issoviet && rn2(Role_if(PM_JEDI) ? 4 : 2)) {
			You("got that skill without using up a skill point!");
			goto jediskip;
		}
	}
	if (skill == P_MAKASHI && (P_SKILL(P_MAKASHI) + 1) > u.lightsabermax2) {
		u.lightsabermax2 = (P_SKILL(P_MAKASHI) + 1);
		if (!issoviet && rn2(Role_if(PM_JEDI) ? 4 : 2)) {
			You("got that skill without using up a skill point!");
			goto jediskip;
		}
	}
	if (skill == P_SORESU && (P_SKILL(P_SORESU) + 1) > u.lightsabermax3) {
		u.lightsabermax3 = (P_SKILL(P_SORESU) + 1);
		if (!issoviet && rn2(Role_if(PM_JEDI) ? 4 : 2)) {
			You("got that skill without using up a skill point!");
			goto jediskip;
		}
	}
	if (skill == P_ATARU && (P_SKILL(P_ATARU) + 1) > u.lightsabermax4) {
		u.lightsabermax4 = (P_SKILL(P_ATARU) + 1);
		if (!issoviet && rn2(Role_if(PM_JEDI) ? 4 : 2)) {
			You("got that skill without using up a skill point!");
			goto jediskip;
		}
	}
	if (skill == P_SHIEN && (P_SKILL(P_SHIEN) + 1) > u.lightsabermax5) {
		u.lightsabermax5 = (P_SKILL(P_SHIEN) + 1);
		if (!issoviet && rn2(Role_if(PM_JEDI) ? 4 : 2)) {
			You("got that skill without using up a skill point!");
			goto jediskip;
		}
	}
	if (skill == P_DJEM_SO && (P_SKILL(P_DJEM_SO) + 1) > u.lightsabermax6) {
		u.lightsabermax6 = (P_SKILL(P_DJEM_SO) + 1);
		if (!issoviet && rn2(Role_if(PM_JEDI) ? 4 : 2)) {
			You("got that skill without using up a skill point!");
			goto jediskip;
		}
	}
	if (skill == P_NIMAN && (P_SKILL(P_NIMAN) + 1) > u.lightsabermax7) {
		u.lightsabermax7 = (P_SKILL(P_NIMAN) + 1);
		if (!issoviet && rn2(Role_if(PM_JEDI) ? 4 : 2)) {
			You("got that skill without using up a skill point!");
			goto jediskip;
		}
	}
	if (skill == P_JUYO && (P_SKILL(P_JUYO) + 1) > u.lightsabermax8) {
		u.lightsabermax8 = (P_SKILL(P_JUYO) + 1);
		if (!issoviet && rn2(Role_if(PM_JEDI) ? 4 : 2)) {
			You("got that skill without using up a skill point!");
			goto jediskip;
		}
	}
	if (skill == P_VAAPAD && (P_SKILL(P_VAAPAD) + 1) > u.lightsabermax9) {
		u.lightsabermax9 = (P_SKILL(P_VAAPAD) + 1);
		if (!issoviet && rn2(Role_if(PM_JEDI) ? 4 : 2)) {
			You("got that skill without using up a skill point!");
			goto jediskip;
		}
	}
	if (skill == P_WEDI && (P_SKILL(P_WEDI) + 1) > u.lightsabermax10) {
		u.lightsabermax10 = (P_SKILL(P_WEDI) + 1);
		if (!issoviet && rn2(Role_if(PM_JEDI) ? 4 : 2)) {
			You("got that skill without using up a skill point!");
			goto jediskip;
		}
	}
    
    u.weapon_slots -= slots_required(skill);
jediskip:
    P_SKILL(skill)++;
    u.skill_record[u.skills_advanced++] = skill;
    /* subtly change the advance message to indicate no more advancement */
    You("are now %s skilled in %s.",
	P_SKILL(skill) >= P_MAX_SKILL(skill) ? "most" : "more", wpskillname(skill));

	if (practicantterror && !u.pract_enhancing) {
		u.pract_enhanceamount++;
		if (u.pract_enhanceamount >= 30) {
			pline("%s booms: 'You now lose 10 intrinsics because you violated a holy rule.'", noroelaname());
			attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse();
			u.pract_enhancing = TRUE;
		}
	}

	if (!isdemagogue) {

	if (!tech_known(T_DISARM) && (P_SKILL(skill) == P_SKILLED) && 
    		skill <= P_LAST_WEAPON && skill != P_WHIP) {
    	learntech(T_DISARM, FROMOUTSIDE, 1);
    	You("learn how to perform disarm!");
	}

	if (skill == P_TRIDENT && P_SKILL(skill) == P_MASTER && !tech_known(T_SILENT_OCEAN)) {
	    	learntech(T_SILENT_OCEAN, FROMOUTSIDE, 1);
	    	You("learn how to perform silent ocean!");
	}
	if (skill == P_UNICORN_HORN && P_SKILL(skill) == P_EXPERT && !tech_known(T_GLOWHORN)) {
	    	learntech(T_GLOWHORN, FROMOUTSIDE, 1);
	    	You("learn how to perform glowhorn!");
	}
	if (skill == P_UNICORN_HORN && P_SKILL(skill) == P_GRAND_MASTER && !tech_known(T_STAT_RESIST)) {
	    	learntech(T_STAT_RESIST, FROMOUTSIDE, 1);
	    	You("learn how to perform stat resist!");
	}
	if (skill == P_CHAOS_SPELL && P_SKILL(skill) == P_MASTER && !tech_known(T_INTRINSIC_ROULETTE)) {
	    	learntech(T_INTRINSIC_ROULETTE, FROMOUTSIDE, 1);
	    	You("learn how to perform intrinsic roulette!");
	}
	if (skill == P_ELEMENTAL_SPELL && P_SKILL(skill) == P_MASTER && !tech_known(T_SPECTRAL_SWORD)) {
	    	learntech(T_SPECTRAL_SWORD, FROMOUTSIDE, 1);
	    	You("learn how to perform spectral sword!");
	}
	if (skill == P_OCCULT_SPELL && P_SKILL(skill) == P_MASTER && !tech_known(T_REVERSE_IDENTIFY)) {
	    	learntech(T_REVERSE_IDENTIFY, FROMOUTSIDE, 1);
	    	You("learn how to perform reverse identify!");
	}
	if (skill == P_SEARCHING && P_SKILL(skill) == P_MASTER && !tech_known(T_DETECT_TRAPS)) {
	    	learntech(T_DETECT_TRAPS, FROMOUTSIDE, 1);
	    	You("learn how to perform detect traps!");
	}
	if (skill == P_PETKEEPING && P_SKILL(skill) == P_SKILLED && !tech_known(T_DIRECTIVE)) {
	    	learntech(T_DIRECTIVE, FROMOUTSIDE, 1);
	    	You("learn how to perform directive!");
	}
	if (skill == P_MEMORIZATION && P_SKILL(skill) == P_MASTER && !tech_known(T_WONDER_YONDER)) {
	    	learntech(T_WONDER_YONDER, FROMOUTSIDE, 1);
	    	You("learn how to perform wonder yonder!");
	}
	if (skill == P_TWO_WEAPON_COMBAT && P_SKILL(skill) == P_EXPERT && !tech_known(T_SWAP_WEAPON)) {
	    	learntech(T_SWAP_WEAPON, FROMOUTSIDE, 1);
	    	You("learn how to perform swap weapon!");
	}
	if (skill == P_IMPLANTS && P_SKILL(skill) == P_MASTER && !tech_known(T_REMOVE_IMPLANT)) {
	    	learntech(T_REMOVE_IMPLANT, FROMOUTSIDE, 1);
	    	You("learn how to perform remove implant!");
	}
	if (skill == P_IMPLANTS && P_SKILL(skill) == P_GRAND_MASTER && !tech_known(T_REROLL_IMPLANT)) {
	    	learntech(T_REROLL_IMPLANT, FROMOUTSIDE, 1);
	    	You("learn how to perform reroll implant!");
	}
	if (skill == P_TECHNIQUES && P_SKILL(skill) == P_MASTER && !tech_known(T_TIME_STOP)) {
	    	learntech(T_TIME_STOP, FROMOUTSIDE, 1);
	    	You("learn how to perform time stop!");
	}
	if (skill == P_LANCE && P_SKILL(skill) == P_MASTER && !tech_known(T_MILDEN_CURSE)) {
	    	learntech(T_MILDEN_CURSE, FROMOUTSIDE, 1);
	    	You("learn how to perform milden curse!");
	}
	if (skill == P_POLEARMS && P_SKILL(skill) == P_MASTER && !tech_known(T_FORCE_FIELD)) {
	    	learntech(T_FORCE_FIELD, FROMOUTSIDE, 1);
	    	You("learn how to perform force field!");
	}
	if (skill == P_POLEARMS && P_SKILL(skill) == P_GRAND_MASTER && !tech_known(T_POINTINESS)) {
	    	learntech(T_POINTINESS, FROMOUTSIDE, 1);
	    	You("learn how to perform pointiness!");
	}
	if (skill == P_PADDLE && P_SKILL(skill) == P_MASTER && !tech_known(T_BUG_SPRAY)) {
	    	learntech(T_BUG_SPRAY, FROMOUTSIDE, 1);
	    	You("learn how to perform bug spray!");
	}
	if (skill == P_QUARTERSTAFF && P_SKILL(skill) == P_MASTER && !tech_known(T_WHIRLSTAFF)) {
	    	learntech(T_WHIRLSTAFF, FROMOUTSIDE, 1);
	    	You("learn how to perform whirlstaff!");
	}
	if (skill == P_CROSSBOW && P_SKILL(skill) == P_MASTER && !tech_known(T_DELIBERATE_CURSE)) {
	    	learntech(T_DELIBERATE_CURSE, FROMOUTSIDE, 1);
	    	You("learn how to perform deliberate curse!");
	}
	if (skill == P_RIDING && P_SKILL(skill) == P_MASTER && !tech_known(T_ACQUIRE_STEED)) {
	    	learntech(T_ACQUIRE_STEED, FROMOUTSIDE, 1);
	    	You("learn how to perform acquire steed!");
	}
	if (skill == P_RIDING && P_SKILL(skill) == P_GRAND_MASTER && !tech_known(T_SADDLING)) {
	    	learntech(T_SADDLING, FROMOUTSIDE, 1);
	    	You("learn how to perform saddling!");
	}
	if (skill == P_HIGH_HEELS && P_SKILL(skill) == P_EXPERT && !tech_known(T_SHOPPING_QUEEN)) {
	    	learntech(T_SHOPPING_QUEEN, FROMOUTSIDE, 1);
	    	You("learn how to perform shopping queen!");
	}
	if (skill == P_HIGH_HEELS && P_SKILL(skill) == P_MASTER && !tech_known(T_BEAUTY_CHARM)) {
	    	learntech(T_BEAUTY_CHARM, FROMOUTSIDE, 1);
	    	You("learn how to perform beauty charm!");
	}
	if (skill == P_HIGH_HEELS && P_SKILL(skill) == P_GRAND_MASTER && !tech_known(T_ASIAN_KICK)) {
	    	learntech(T_ASIAN_KICK, FROMOUTSIDE, 1);
	    	You("learn how to perform asian kick!");
	}
	if (skill == P_HIGH_HEELS && P_SKILL(skill) == P_GRAND_MASTER && !tech_known(T_LEGSCRATCH_ATTACK)) {
	    	learntech(T_LEGSCRATCH_ATTACK, FROMOUTSIDE, 1);
	    	You("learn how to perform legscratch attack!");
	}
	if (skill == P_HIGH_HEELS && P_SKILL(skill) == P_GRAND_MASTER && !tech_known(T_GROUND_STOMP)) {
	    	learntech(T_GROUND_STOMP, FROMOUTSIDE, 1);
	    	You("learn how to perform ground stomp!");
	}
	if (skill == P_HIGH_HEELS && P_SKILL(skill) == P_GRAND_MASTER && !tech_known(T_ATHLETIC_COMBAT)) {
	    	learntech(T_ATHLETIC_COMBAT, FROMOUTSIDE, 1);
	    	You("learn how to perform athletic combat!");
	}
	if (skill == P_SPIRITUALITY && P_SKILL(skill) == P_GRAND_MASTER && !tech_known(T_PRAYING_SUCCESS)) {
	    	learntech(T_PRAYING_SUCCESS, FROMOUTSIDE, 1);
	    	You("learn how to perform praying success!");
	}
	if (skill == P_DEVICES && P_SKILL(skill) == P_MASTER && !tech_known(T_OVER_RAY)) {
	    	learntech(T_OVER_RAY, FROMOUTSIDE, 1);
	    	You("learn how to perform over-ray!");
	}
	if (skill == P_BOOMERANG && P_SKILL(skill) == P_EXPERT && !tech_known(T_ENCHANTERANG)) {
	    	learntech(T_ENCHANTERANG, FROMOUTSIDE, 1);
	    	You("learn how to perform enchanterang!");
	}
	if (skill == P_BOOMERANG && P_SKILL(skill) == P_MASTER && !tech_known(T_BATMAN_ARSENAL)) {
	    	learntech(T_BATMAN_ARSENAL, FROMOUTSIDE, 1);
	    	You("learn how to perform batman arsenal!");
	}
	if (skill == P_BOOMERANG && P_SKILL(skill) == P_GRAND_MASTER && !tech_known(T_JOKERBANE)) {
	    	learntech(T_JOKERBANE, FROMOUTSIDE, 1);
	    	You("learn how to perform jokerbane!");
	}
	if (skill == P_WHIP && P_SKILL(skill) == P_EXPERT && !tech_known(T_CALL_THE_POLICE)) {
	    	learntech(T_CALL_THE_POLICE, FROMOUTSIDE, 1);
	    	You("learn how to perform call the police!");
	}
	if (skill == P_WHIP && P_SKILL(skill) == P_MASTER && !tech_known(T_DOMINATE)) {
	    	learntech(T_DOMINATE, FROMOUTSIDE, 1);
	    	You("learn how to perform dominate!");
	}
	if (skill == P_WHIP && P_SKILL(skill) == P_GRAND_MASTER && !tech_known(T_INCARNATION)) {
	    	learntech(T_INCARNATION, FROMOUTSIDE, 1);
	    	You("learn how to perform incarnation!");
	}
	if (skill == P_MARTIAL_ARTS && P_SKILL(skill) == P_GRAND_MASTER && !tech_known(T_COMBO_STRIKE)) {
	    	learntech(T_COMBO_STRIKE, FROMOUTSIDE, 1);
	    	You("learn how to perform combo strike!");
	}
	if (skill == P_POLYMORPHING && P_SKILL(skill) == P_MASTER && !tech_known(T_FUNGOISM)) {
	    	learntech(T_FUNGOISM, FROMOUTSIDE, 1);
	    	You("learn how to perform fungoism!");
	}
	if (skill == P_POLYMORPHING && P_SKILL(skill) == P_GRAND_MASTER && !tech_known(T_BECOME_UNDEAD)) {
	    	learntech(T_BECOME_UNDEAD, FROMOUTSIDE, 1);
	    	You("learn how to perform become undead!");
	}
	if (skill == P_BARE_HANDED_COMBAT && P_SKILL(skill) == P_MASTER && !tech_known(T_JIU_JITSU)) {
	    	learntech(T_JIU_JITSU, FROMOUTSIDE, 1);
	    	You("learn how to perform jiu-jitsu!");
	}
	if (skill == P_SHURIKEN && P_SKILL(skill) == P_MASTER && !tech_known(T_BLADE_ANGER)) {
	    	learntech(T_BLADE_ANGER, FROMOUTSIDE, 1);
	    	You("learn how to perform blade anger!");
	}
	if (skill == P_PETKEEPING && P_SKILL(skill) == P_EXPERT && !tech_known(T_RE_TAMING)) {
	    	learntech(T_RE_TAMING, FROMOUTSIDE, 1);
	    	You("learn how to perform re-taming!");
	}
	if (skill == P_SHII_CHO && P_SKILL(skill) == P_MASTER && !tech_known(T_UNCURSE_SABER)) {
	    	learntech(T_UNCURSE_SABER, FROMOUTSIDE, 1);
	    	You("learn how to perform uncurse saber!");
	}
	if (skill == P_MAKASHI && P_SKILL(skill) == P_SKILLED && !tech_known(T_ENERGY_CONSERVATION)) {
	    	learntech(T_ENERGY_CONSERVATION, FROMOUTSIDE, 1);
	    	You("learn how to perform energy conservation!");
	}
	if (skill == P_SORESU && P_SKILL(skill) == P_MASTER && !tech_known(T_ENCHANT_ROBE)) {
	    	learntech(T_ENCHANT_ROBE, FROMOUTSIDE, 1);
	    	You("learn how to perform enchant robe!");
	}
	if (skill == P_ATARU && P_SKILL(skill) == P_EXPERT && !tech_known(T_WILD_SLASHING)) {
	    	learntech(T_WILD_SLASHING, FROMOUTSIDE, 1);
	    	You("learn how to perform wild slashing!");
	}
	if (skill == P_SHIEN && P_SKILL(skill) == P_EXPERT && !tech_known(T_ABSORBER_SHIELD)) {
	    	learntech(T_ABSORBER_SHIELD, FROMOUTSIDE, 1);
	    	You("learn how to perform absorber shield!");
	}
	if (skill == P_DJEM_SO && P_SKILL(skill) == P_EXPERT && !tech_known(T_PSYCHO_FORCE)) {
	    	learntech(T_PSYCHO_FORCE, FROMOUTSIDE, 1);
	    	You("learn how to perform psycho force!");
	}
	if (skill == P_NIMAN && P_SKILL(skill) == P_MASTER && !tech_known(T_INTENSIVE_TRAINING)) {
	    	learntech(T_INTENSIVE_TRAINING, FROMOUTSIDE, 1);
	    	You("learn how to perform intensive training!");
	}
	if (skill == P_JUYO && P_SKILL(skill) == P_EXPERT && !tech_known(T_SURRENDER_OR_DIE)) {
	    	learntech(T_SURRENDER_OR_DIE, FROMOUTSIDE, 1);
	    	You("learn how to perform surrender or die!");
	}
	if (skill == P_VAAPAD && P_SKILL(skill) == P_EXPERT && !tech_known(T_PERILOUS_WHIRL)) {
	    	learntech(T_PERILOUS_WHIRL, FROMOUTSIDE, 1);
	    	You("learn how to perform perilous whirl!");
	}
	if (skill == P_PETKEEPING && P_SKILL(skill) == P_MASTER && P_SKILL(P_HIGH_HEELS) >= P_EXPERT && !tech_known(T_SUMMON_SHOE)) {
	    	learntech(T_SUMMON_SHOE, FROMOUTSIDE, 1);
	    	You("learn how to perform summon shoe!");
	}
	if (skill == P_HIGH_HEELS && P_SKILL(skill) == P_MASTER && P_SKILL(P_PETKEEPING) >= P_EXPERT && !tech_known(T_SUMMON_SHOE)) {
	    	learntech(T_SUMMON_SHOE, FROMOUTSIDE, 1);
	    	You("learn how to perform summon shoe!");
	}
	if (skill == P_PETKEEPING && P_SKILL(skill) == P_EXPERT && P_SKILL(P_HIGH_HEELS) >= P_MASTER && !tech_known(T_SUMMON_SHOE)) {
	    	learntech(T_SUMMON_SHOE, FROMOUTSIDE, 1);
	    	You("learn how to perform summon shoe!");
	}
	if (skill == P_HIGH_HEELS && P_SKILL(skill) == P_EXPERT && P_SKILL(P_PETKEEPING) >= P_MASTER && !tech_known(T_SUMMON_SHOE)) {
	    	learntech(T_SUMMON_SHOE, FROMOUTSIDE, 1);
	    	You("learn how to perform summon shoe!");
	}
	if (skill == P_SEXY_FLATS && P_SKILL(skill) == P_EXPERT && !tech_known(T_KICK_IN_THE_NUTS)) {
	    	learntech(T_KICK_IN_THE_NUTS, FROMOUTSIDE, 1);
	    	You("learn how to perform kick in the nuts!");
	}
	if (skill == P_SEXY_FLATS && P_SKILL(skill) == P_MASTER && !tech_known(T_DISARMING_KICK)) {
	    	learntech(T_DISARMING_KICK, FROMOUTSIDE, 1);
	    	You("learn how to perform disarming kick!");
	}
	if (skill == P_SEXY_FLATS && P_SKILL(skill) == P_GRAND_MASTER && !tech_known(T_INLAY_WARFARE)) {
	    	learntech(T_INLAY_WARFARE, FROMOUTSIDE, 1);
	    	You("learn how to perform inlay warfare!");
	}

	if (skill == P_SHII_CHO && P_SKILL(skill) == P_EXPERT && !tech_known(T_STEADY_HAND)) {
	    	learntech(T_STEADY_HAND, FROMOUTSIDE, 1);
	    	You("learn how to perform steady hand!");
	}
	if (skill == P_MAKASHI && P_SKILL(skill) == P_MASTER && !tech_known(T_FORCE_FILLING)) {
	    	learntech(T_FORCE_FILLING, FROMOUTSIDE, 1);
	    	You("learn how to perform force filling!");
	}
	if (skill == P_SORESU && P_SKILL(skill) == P_GRAND_MASTER && !tech_known(T_JEDI_TAILORING)) {
	    	learntech(T_JEDI_TAILORING, FROMOUTSIDE, 1);
	    	You("learn how to perform jedi tailoring!");
	}
	if (skill == P_ATARU && P_SKILL(skill) == P_SKILLED && !tech_known(T_INTRINSIC_SACRIFICE)) {
	    	learntech(T_INTRINSIC_SACRIFICE, FROMOUTSIDE, 1);
	    	You("learn how to perform intrinsic sacrifice!");
	}
	if (skill == P_DJEM_SO && P_SKILL(skill) == P_MASTER && !tech_known(T_BEAMSWORD)) {
	    	learntech(T_BEAMSWORD, FROMOUTSIDE, 1);
	    	You("learn how to perform beamsword!");
	}
	if (skill == P_NIMAN && P_SKILL(skill) == P_GRAND_MASTER && !tech_known(T_ENERGY_TRANSFER)) {
	    	learntech(T_ENERGY_TRANSFER, FROMOUTSIDE, 1);
	    	You("learn how to perform energy transfer!");
	}
	if (skill == P_JUYO && P_SKILL(skill) == P_MASTER && !tech_known(T_SOFTEN_TARGET)) {
	    	learntech(T_SOFTEN_TARGET, FROMOUTSIDE, 1);
	    	You("learn how to perform soften target!");
	}
	if (skill == P_VAAPAD && P_SKILL(skill) == P_MASTER && !tech_known(T_PROTECT_WEAPON)) {
	    	learntech(T_PROTECT_WEAPON, FROMOUTSIDE, 1);
	    	You("learn how to perform protect weapon!");
	}
	if (skill == P_SHIEN && P_SKILL(skill) == P_MASTER && !tech_known(T_POWERFUL_AURA)) {
	    	learntech(T_POWERFUL_AURA, FROMOUTSIDE, 1);
	    	You("learn how to perform powerful aura!");
	}

	if (skill == P_NIMAN && P_SKILL(skill) == P_EXPERT && P_SKILL(P_QUARTERSTAFF) >= P_EXPERT && !tech_known(T_BOOSTAFF)) {
	    	learntech(T_BOOSTAFF, FROMOUTSIDE, 1);
	    	You("learn how to perform boostaff!");
	}
	if (skill == P_QUARTERSTAFF && P_SKILL(skill) == P_EXPERT && P_SKILL(P_NIMAN) >= P_EXPERT && !tech_known(T_BOOSTAFF)) {
	    	learntech(T_BOOSTAFF, FROMOUTSIDE, 1);
	    	You("learn how to perform boostaff!");
	}

	if (skill == P_MAKASHI && P_SKILL(skill) == P_MASTER && P_SKILL(P_JAVELIN) >= P_MASTER && !tech_known(T_CLONE_JAVELIN)) {
	    	learntech(T_CLONE_JAVELIN, FROMOUTSIDE, 1);
	    	You("learn how to perform clone javelin!");
	}
	if (skill == P_JAVELIN && P_SKILL(skill) == P_MASTER && P_SKILL(P_MAKASHI) >= P_MASTER && !tech_known(T_CLONE_JAVELIN)) {
	    	learntech(T_CLONE_JAVELIN, FROMOUTSIDE, 1);
	    	You("learn how to perform clone javelin!");
	}

	if (skill == P_MACE && P_SKILL(skill) == P_EXPERT && P_SKILL(P_SHII_CHO) >= P_EXPERT && !tech_known(T_REFUGE)) {
	    	learntech(T_REFUGE, FROMOUTSIDE, 1);
	    	You("learn how to perform refuge!");
	}
	if (skill == P_SHII_CHO && P_SKILL(skill) == P_EXPERT && P_SKILL(P_MACE) >= P_EXPERT && !tech_known(T_REFUGE)) {
	    	learntech(T_REFUGE, FROMOUTSIDE, 1);
	    	You("learn how to perform refuge!");
	}

	if (skill == P_SORESU && P_SKILL(skill) == P_EXPERT && P_SKILL(P_MARTIAL_ARTS) >= P_MASTER && !tech_known(T_DRAINING_PUNCH)) {
	    	learntech(T_DRAINING_PUNCH, FROMOUTSIDE, 1);
	    	You("learn how to perform draining punch!");
	}
	if (skill == P_MARTIAL_ARTS && P_SKILL(skill) == P_EXPERT && P_SKILL(P_SORESU) >= P_MASTER && !tech_known(T_DRAINING_PUNCH)) {
	    	learntech(T_DRAINING_PUNCH, FROMOUTSIDE, 1);
	    	You("learn how to perform draining punch!");
	}
	if (skill == P_SORESU && P_SKILL(skill) == P_MASTER && P_SKILL(P_MARTIAL_ARTS) >= P_EXPERT && !tech_known(T_DRAINING_PUNCH)) {
	    	learntech(T_DRAINING_PUNCH, FROMOUTSIDE, 1);
	    	You("learn how to perform draining punch!");
	}
	if (skill == P_MARTIAL_ARTS && P_SKILL(skill) == P_MASTER && P_SKILL(P_SORESU) >= P_EXPERT && !tech_known(T_DRAINING_PUNCH)) {
	    	learntech(T_DRAINING_PUNCH, FROMOUTSIDE, 1);
	    	You("learn how to perform draining punch!");
	}

	if (skill == P_SORESU && P_SKILL(skill) == P_EXPERT && P_SKILL(P_BARE_HANDED_COMBAT) >= P_EXPERT && !tech_known(T_ESCROBISM)) {
	    	learntech(T_ESCROBISM, FROMOUTSIDE, 1);
	    	You("learn how to perform escrobism!");
	}
	if (skill == P_BARE_HANDED_COMBAT && P_SKILL(skill) == P_EXPERT && P_SKILL(P_SORESU) >= P_EXPERT && !tech_known(T_ESCROBISM)) {
	    	learntech(T_ESCROBISM, FROMOUTSIDE, 1);
	    	You("learn how to perform escrobism!");
	}

	if (skill == P_ATARU && P_SKILL(skill) == P_SKILLED && P_SKILL(P_SCIMITAR) >= P_SKILLED && !tech_known(T_PIRATE_BROTHERING)) {
	    	learntech(T_PIRATE_BROTHERING, FROMOUTSIDE, 1);
	    	You("learn how to perform pirate brothering!");
	}
	if (skill == P_SCIMITAR && P_SKILL(skill) == P_SKILLED && P_SKILL(P_ATARU) >= P_SKILLED && !tech_known(T_PIRATE_BROTHERING)) {
	    	learntech(T_PIRATE_BROTHERING, FROMOUTSIDE, 1);
	    	You("learn how to perform pirate brothering!");
	}

	if (skill == P_DJEM_SO && P_SKILL(skill) == P_SKILLED && P_SKILL(P_CROSSBOW) >= P_EXPERT && !tech_known(T_NUTS_AND_BOLTS)) {
	    	learntech(T_NUTS_AND_BOLTS, FROMOUTSIDE, 1);
	    	You("learn how to perform nuts and bolts!");
	}
	if (skill == P_CROSSBOW && P_SKILL(skill) == P_SKILLED && P_SKILL(P_DJEM_SO) >= P_EXPERT && !tech_known(T_NUTS_AND_BOLTS)) {
	    	learntech(T_NUTS_AND_BOLTS, FROMOUTSIDE, 1);
	    	You("learn how to perform nuts and bolts!");
	}
	if (skill == P_DJEM_SO && P_SKILL(skill) == P_EXPERT && P_SKILL(P_CROSSBOW) >= P_SKILLED && !tech_known(T_NUTS_AND_BOLTS)) {
	    	learntech(T_NUTS_AND_BOLTS, FROMOUTSIDE, 1);
	    	You("learn how to perform nuts and bolts!");
	}
	if (skill == P_CROSSBOW && P_SKILL(skill) == P_EXPERT && P_SKILL(P_DJEM_SO) >= P_SKILLED && !tech_known(T_NUTS_AND_BOLTS)) {
	    	learntech(T_NUTS_AND_BOLTS, FROMOUTSIDE, 1);
	    	You("learn how to perform nuts and bolts!");
	}

	if (skill == P_JUYO && P_SKILL(skill) == P_EXPERT && P_SKILL(P_POLEARMS) >= P_MASTER && !tech_known(T_DECAPABILITY)) {
	    	learntech(T_DECAPABILITY, FROMOUTSIDE, 1);
	    	You("learn how to perform decapability!");
	}
	if (skill == P_POLEARMS && P_SKILL(skill) == P_EXPERT && P_SKILL(P_JUYO) >= P_MASTER && !tech_known(T_DECAPABILITY)) {
	    	learntech(T_DECAPABILITY, FROMOUTSIDE, 1);
	    	You("learn how to perform decapability!");
	}
	if (skill == P_JUYO && P_SKILL(skill) == P_MASTER && P_SKILL(P_POLEARMS) >= P_EXPERT && !tech_known(T_DECAPABILITY)) {
	    	learntech(T_DECAPABILITY, FROMOUTSIDE, 1);
	    	You("learn how to perform decapability!");
	}
	if (skill == P_POLEARMS && P_SKILL(skill) == P_MASTER && P_SKILL(P_JUYO) >= P_EXPERT && !tech_known(T_DECAPABILITY)) {
	    	learntech(T_DECAPABILITY, FROMOUTSIDE, 1);
	    	You("learn how to perform decapability!");
	}

	if (skill == P_VAAPAD && P_SKILL(skill) == P_EXPERT && P_SKILL(P_TWO_HANDED_SWORD) >= P_EXPERT && !tech_known(T_NO_HANDS_CURSE)) {
	    	learntech(T_NO_HANDS_CURSE, FROMOUTSIDE, 1);
	    	You("learn how to perform no-hands curse!");
	}
	if (skill == P_TWO_HANDED_SWORD && P_SKILL(skill) == P_EXPERT && P_SKILL(P_VAAPAD) >= P_EXPERT && !tech_known(T_NO_HANDS_CURSE)) {
	    	learntech(T_NO_HANDS_CURSE, FROMOUTSIDE, 1);
	    	You("learn how to perform no-hands curse!");
	}

	if (skill == P_SEXY_FLATS && P_SKILL(skill) == P_EXPERT && P_SKILL(P_HIGH_HEELS) >= P_EXPERT && !tech_known(T_HIGH_HEELED_SNEAKERS)) {
	    	learntech(T_HIGH_HEELED_SNEAKERS, FROMOUTSIDE, 1);
	    	You("learn how to perform high-heeled sneakers!");
	}
	if (skill == P_HIGH_HEELS && P_SKILL(skill) == P_EXPERT && P_SKILL(P_SEXY_FLATS) >= P_EXPERT && !tech_known(T_HIGH_HEELED_SNEAKERS)) {
	    	learntech(T_HIGH_HEELED_SNEAKERS, FROMOUTSIDE, 1);
	    	You("learn how to perform high-heeled sneakers!");
	}

	if (skill == P_MAKASHI && P_SKILL(skill) == P_EXPERT && P_SKILL(P_ATARU) >= P_EXPERT && P_SKILL(P_VAAPAD) >= P_EXPERT && !tech_known(T_FORM_CHOICE)) {
	    	learntech(T_FORM_CHOICE, FROMOUTSIDE, 1);
	    	You("learn how to perform form choice!");
	}
	if (skill == P_VAAPAD && P_SKILL(skill) == P_EXPERT && P_SKILL(P_MAKASHI) >= P_EXPERT && P_SKILL(P_ATARU) >= P_EXPERT && !tech_known(T_FORM_CHOICE)) {
	    	learntech(T_FORM_CHOICE, FROMOUTSIDE, 1);
	    	You("learn how to perform form choice!");
	}
	if (skill == P_ATARU && P_SKILL(skill) == P_EXPERT && P_SKILL(P_VAAPAD) >= P_EXPERT && P_SKILL(P_MAKASHI) >= P_EXPERT && !tech_known(T_FORM_CHOICE)) {
	    	learntech(T_FORM_CHOICE, FROMOUTSIDE, 1);
	    	You("learn how to perform form choice!");
	}

	if (skill == P_GUN_CONTROL && P_SKILL(skill) == P_BASIC && !tech_known(T_SHOTTY_BLAST)) {
	    	learntech(T_SHOTTY_BLAST, FROMOUTSIDE, 1);
	    	You("learn how to perform shotty blast!");
	}
	if (skill == P_GUN_CONTROL && P_SKILL(skill) == P_SKILLED && !tech_known(T_AMMO_UPGRADE)) {
	    	learntech(T_AMMO_UPGRADE, FROMOUTSIDE, 1);
	    	You("learn how to perform ammo upgrade!");
	}
	if (skill == P_GUN_CONTROL && P_SKILL(skill) == P_EXPERT && !tech_known(T_LASER_POWER)) {
	    	learntech(T_LASER_POWER, FROMOUTSIDE, 1);
	    	You("learn how to perform laser power!");
	}
	if (skill == P_GUN_CONTROL && P_SKILL(skill) == P_MASTER && !tech_known(T_BIG_DADDY)) {
	    	learntech(T_BIG_DADDY, FROMOUTSIDE, 1);
	    	You("learn how to perform big daddy!");
	}
	if (skill == P_GUN_CONTROL && P_SKILL(skill) == P_GRAND_MASTER && !tech_known(T_SHUT_THAT_BITCH_UP)) {
	    	learntech(T_SHUT_THAT_BITCH_UP, FROMOUTSIDE, 1);
	    	You("learn how to perform shut that bitch up!");
	}
	if (skill == P_SQUEAKING && P_SKILL(skill) == P_BASIC && !tech_known(T_S_PRESSING)) {
	    	learntech(T_S_PRESSING, FROMOUTSIDE, 1);
	    	You("learn how to perform s-pressing!");
	}
	if (skill == P_SQUEAKING && P_SKILL(skill) == P_SKILLED && !tech_known(T_MELTEE)) {
	    	learntech(T_MELTEE, FROMOUTSIDE, 1);
	    	You("learn how to perform meltee!");
	}
	if (skill == P_SQUEAKING && P_SKILL(skill) == P_EXPERT && !tech_known(T_WOMAN_NOISES)) {
	    	learntech(T_WOMAN_NOISES, FROMOUTSIDE, 1);
	    	You("learn how to perform woman noises!");
	}
	if (skill == P_SQUEAKING && P_SKILL(skill) == P_MASTER && !tech_known(T_EXTRA_LONG_SQUEAK)) {
	    	learntech(T_EXTRA_LONG_SQUEAK, FROMOUTSIDE, 1);
	    	You("learn how to perform extra long squeak!");
	}
	if (skill == P_SQUEAKING && P_SKILL(skill) == P_GRAND_MASTER && !tech_known(T_SEXUAL_HUG)) {
	    	learntech(T_SEXUAL_HUG, FROMOUTSIDE, 1);
	    	You("learn how to perform sexual hug!");
	}
	if (skill == P_GUN_CONTROL && P_SKILL(skill) == P_GRAND_MASTER && P_SKILL(P_SQUEAKING) >= P_GRAND_MASTER && !tech_known(T_SEX_CHANGE)) {
	    	learntech(T_SEX_CHANGE, FROMOUTSIDE, 1);
	    	You("learn how to perform sex change!");
	}
	if (skill == P_SQUEAKING && P_SKILL(skill) == P_GRAND_MASTER && P_SKILL(P_GUN_CONTROL) >= P_GRAND_MASTER && !tech_known(T_SEX_CHANGE)) {
	    	learntech(T_SEX_CHANGE, FROMOUTSIDE, 1);
	    	You("learn how to perform sex change!");
	}
	if (skill == P_GUN_CONTROL && P_SKILL(skill) == P_MASTER && P_SKILL(P_FIREARM) >= P_MASTER && !tech_known(T_EVEN_MORE_AMMO)) {
	    	learntech(T_EVEN_MORE_AMMO, FROMOUTSIDE, 1);
	    	You("learn how to perform even more ammo!");
	}
	if (skill == P_FIREARM && P_SKILL(skill) == P_MASTER && P_SKILL(P_GUN_CONTROL) >= P_MASTER && !tech_known(T_EVEN_MORE_AMMO)) {
	    	learntech(T_EVEN_MORE_AMMO, FROMOUTSIDE, 1);
	    	You("learn how to perform even more ammo!");
	}
	if (skill == P_SQUEAKING && P_SKILL(skill) == P_EXPERT && P_SKILL(P_POLYMORPHING) >= P_MASTER && !tech_known(T_DOUBLESELF)) {
	    	learntech(T_DOUBLESELF, FROMOUTSIDE, 1);
	    	You("learn how to perform doubleself!");
	}
	if (skill == P_SQUEAKING && P_SKILL(skill) == P_MASTER && P_SKILL(P_POLYMORPHING) >= P_EXPERT && !tech_known(T_DOUBLESELF)) {
	    	learntech(T_DOUBLESELF, FROMOUTSIDE, 1);
	    	You("learn how to perform doubleself!");
	}
	if (skill == P_POLYMORPHING && P_SKILL(skill) == P_EXPERT && P_SKILL(P_SQUEAKING) >= P_MASTER && !tech_known(T_DOUBLESELF)) {
	    	learntech(T_DOUBLESELF, FROMOUTSIDE, 1);
	    	You("learn how to perform doubleself!");
	}
	if (skill == P_POLYMORPHING && P_SKILL(skill) == P_MASTER && P_SKILL(P_SQUEAKING) >= P_EXPERT && !tech_known(T_DOUBLESELF)) {
	    	learntech(T_DOUBLESELF, FROMOUTSIDE, 1);
	    	You("learn how to perform doubleself!");
	}
	if (skill == P_CHAOS_SPELL && P_SKILL(skill) == P_EXPERT && P_SKILL(P_POLYMORPHING) >= P_EXPERT && !tech_known(T_POLYFIX)) {
	    	learntech(T_POLYFIX, FROMOUTSIDE, 1);
	    	You("learn how to perform polyfix!");
	}
	if (skill == P_POLYMORPHING && P_SKILL(skill) == P_EXPERT && P_SKILL(P_CHAOS_SPELL) >= P_EXPERT && !tech_known(T_POLYFIX)) {
	    	learntech(T_POLYFIX, FROMOUTSIDE, 1);
	    	You("learn how to perform polyfix!");
	}
	if (skill == P_OCCULT_SPELL && P_SKILL(skill) == P_EXPERT && P_SKILL(P_SQUEAKING) >= P_EXPERT && !tech_known(T_SQUEAKY_REPAIR)) {
	    	learntech(T_SQUEAKY_REPAIR, FROMOUTSIDE, 1);
	    	You("learn how to perform squeaky repair!");
	}
	if (skill == P_SQUEAKING && P_SKILL(skill) == P_EXPERT && P_SKILL(P_OCCULT_SPELL) >= P_EXPERT && !tech_known(T_SQUEAKY_REPAIR)) {
	    	learntech(T_SQUEAKY_REPAIR, FROMOUTSIDE, 1);
	    	You("learn how to perform squeaky repair!");
	}
	if (skill == P_OCCULT_SPELL && P_SKILL(skill) == P_EXPERT && P_SKILL(P_GUN_CONTROL) >= P_EXPERT && !tech_known(T_BULLETREUSE)) {
	    	learntech(T_BULLETREUSE, FROMOUTSIDE, 1);
	    	You("learn how to perform bulletreuse!");
	}
	if (skill == P_GUN_CONTROL && P_SKILL(skill) == P_EXPERT && P_SKILL(P_OCCULT_SPELL) >= P_EXPERT && !tech_known(T_BULLETREUSE)) {
	    	learntech(T_BULLETREUSE, FROMOUTSIDE, 1);
	    	You("learn how to perform bulletreuse!");
	}
	if (skill == P_ELEMENTAL_SPELL && P_SKILL(skill) == P_EXPERT && P_SKILL(P_DEVICES) >= P_EXPERT && !tech_known(T_EXTRACHARGE)) {
	    	learntech(T_EXTRACHARGE, FROMOUTSIDE, 1);
	    	You("learn how to perform extracharge!");
	}
	if (skill == P_DEVICES && P_SKILL(skill) == P_EXPERT && P_SKILL(P_ELEMENTAL_SPELL) >= P_EXPERT && !tech_known(T_EXTRACHARGE)) {
	    	learntech(T_EXTRACHARGE, FROMOUTSIDE, 1);
	    	You("learn how to perform extracharge!");
	}

	if (skill == P_SYMBIOSIS && P_SKILL(skill) == P_BASIC && !tech_known(T_SYMBIOSIS)) {
	    	learntech(T_SYMBIOSIS, FROMOUTSIDE, 1);
	    	You("learn how to perform symbiosis!");
	}
	if (skill == P_SYMBIOSIS && P_SKILL(skill) == P_SKILLED && !tech_known(T_ADJUST_SYMBIOTE)) {
	    	learntech(T_ADJUST_SYMBIOTE, FROMOUTSIDE, 1);
	    	You("learn how to perform adjust symbiote!");
	}
	if (skill == P_SYMBIOSIS && P_SKILL(skill) == P_EXPERT && !tech_known(T_HEAL_SYMBIOTE)) {
	    	learntech(T_HEAL_SYMBIOTE, FROMOUTSIDE, 1);
	    	You("learn how to perform heal symbiote!");
	}
	if (skill == P_SYMBIOSIS && P_SKILL(skill) == P_MASTER && !tech_known(T_BOOST_SYMBIOTE)) {
	    	learntech(T_BOOST_SYMBIOTE, FROMOUTSIDE, 1);
	    	You("learn how to perform boost symbiote!");
	}
	if (skill == P_SYMBIOSIS && P_SKILL(skill) == P_GRAND_MASTER && !tech_known(T_POWERBIOSIS)) {
	    	learntech(T_POWERBIOSIS, FROMOUTSIDE, 1);
	    	You("learn how to perform powerbiosis!");
	}
	if (skill == P_SHORT_SWORD && P_SKILL(skill) == P_MASTER && !tech_known(T_UNDERTOW)) {
	    	learntech(T_UNDERTOW, FROMOUTSIDE, 1);
	    	You("learn how to perform undertow!");
	}
	if (skill == P_FIREARM && P_SKILL(skill) == P_MASTER && !tech_known(T_GRENADES_OF_COURSE)) {
	    	learntech(T_GRENADES_OF_COURSE, FROMOUTSIDE, 1);
	    	You("learn how to perform grenades of course!");
	}
	if (skill == P_CLUB && P_SKILL(skill) == P_MASTER && !tech_known(T_DEFINALIZE)) {
	    	learntech(T_DEFINALIZE, FROMOUTSIDE, 1);
	    	You("learn how to perform definalize!");
	}
	if (skill == P_MACE && P_SKILL(skill) == P_GRAND_MASTER && !tech_known(T_ANTI_INERTIA)) {
	    	learntech(T_ANTI_INERTIA, FROMOUTSIDE, 1);
	    	You("learn how to perform anti inertia!");
	}
	if (skill == P_SYMBIOSIS && P_SKILL(skill) == P_EXPERT && P_SKILL(P_IMPLANTS) >= P_EXPERT && !tech_known(T_IMPLANTED_SYMBIOSIS)) {
	    	learntech(T_IMPLANTED_SYMBIOSIS, FROMOUTSIDE, 1);
	    	You("learn how to perform implanted symbiosis!");
	}
	if (skill == P_IMPLANTS && P_SKILL(skill) == P_EXPERT && P_SKILL(P_SYMBIOSIS) >= P_EXPERT && !tech_known(T_IMPLANTED_SYMBIOSIS)) {
	    	learntech(T_IMPLANTED_SYMBIOSIS, FROMOUTSIDE, 1);
	    	You("learn how to perform implanted symbiosis!");
	}
	if (skill == P_SYMBIOSIS && P_SKILL(skill) == P_EXPERT && P_SKILL(P_POLYMORPHING) >= P_EXPERT && !tech_known(T_ASSUME_SYMBIOTE)) {
	    	learntech(T_ASSUME_SYMBIOTE, FROMOUTSIDE, 1);
	    	You("learn how to perform assume symbiote!");
	}
	if (skill == P_POLYMORPHING && P_SKILL(skill) == P_EXPERT && P_SKILL(P_SYMBIOSIS) >= P_EXPERT && !tech_known(T_ASSUME_SYMBIOTE)) {
	    	learntech(T_ASSUME_SYMBIOTE, FROMOUTSIDE, 1);
	    	You("learn how to perform assume symbiote!");
	}
	if (skill == P_SYMBIOSIS && P_SKILL(skill) == P_MASTER && P_SKILL(P_PETKEEPING) >= P_MASTER && !tech_known(T_GENERATE_OFFSPRING)) {
	    	learntech(T_GENERATE_OFFSPRING, FROMOUTSIDE, 1);
	    	You("learn how to perform generate offspring!");
	}
	if (skill == P_PETKEEPING && P_SKILL(skill) == P_MASTER && P_SKILL(P_SYMBIOSIS) >= P_MASTER && !tech_known(T_GENERATE_OFFSPRING)) {
	    	learntech(T_GENERATE_OFFSPRING, FROMOUTSIDE, 1);
	    	You("learn how to perform generate offspring!");
	}

	if (skill == P_WEDI && P_SKILL(skill) == P_EXPERT && !tech_known(T_STAR_DIGGING)) {
	    	learntech(T_STAR_DIGGING, FROMOUTSIDE, 1);
	    	You("learn how to perform star digging!");
	}
	if (skill == P_WEDI && P_SKILL(skill) == P_GRAND_MASTER && !tech_known(T_STARWARS_FRIENDS)) {
	    	learntech(T_STARWARS_FRIENDS, FROMOUTSIDE, 1);
	    	You("learn how to perform starwars friends!");
	}
	if (skill == P_WEDI && P_SKILL(skill) == P_MASTER && (Role_if(PM_SPACEWARS_FIGHTER) || Role_if(PM_CAMPERSTRIKER) || Role_if(PM_GANG_SCHOLAR) || Role_if(PM_WALSCHOLAR)) && !tech_known(T_STARWARS_FRIENDS)) {
	    	learntech(T_STARWARS_FRIENDS, FROMOUTSIDE, 1);
	    	You("learn how to perform starwars friends!");
	}

	if (skill == P_WEDI && P_SKILL(skill) == P_EXPERT && P_SKILL(P_PICK_AXE) >= P_EXPERT && !tech_known(T_USE_THE_FORCE_LUKE)) {
	    	learntech(T_USE_THE_FORCE_LUKE, FROMOUTSIDE, 1);
	    	You("learn how to perform use the force luke!");
	}
	if (skill == P_PICK_AXE && P_SKILL(skill) == P_EXPERT && P_SKILL(P_WEDI) >= P_EXPERT && !tech_known(T_USE_THE_FORCE_LUKE)) {
	    	learntech(T_USE_THE_FORCE_LUKE, FROMOUTSIDE, 1);
	    	You("learn how to perform use the force luke!");
	}

	if (skill == P_ORB && P_SKILL(skill) == P_BASIC && !tech_known(T_PACIFY)) {
	    	learntech(T_PACIFY, FROMOUTSIDE, 1);
	    	You("learn how to perform pacify!");
	}
	if (skill == P_ORB && P_SKILL(skill) == P_SKILLED && !tech_known(T_AFTERBURNER)) {
	    	learntech(T_AFTERBURNER, FROMOUTSIDE, 1);
	    	You("learn how to perform afterburner!");
	}
	if (skill == P_ORB && P_SKILL(skill) == P_EXPERT && !tech_known(T_BUGGARD)) {
	    	learntech(T_BUGGARD, FROMOUTSIDE, 1);
	    	You("learn how to perform buggard!");
	}
	if (skill == P_ORB && P_SKILL(skill) == P_MASTER && !tech_known(T_THUNDERSTORM)) {
	    	learntech(T_THUNDERSTORM, FROMOUTSIDE, 1);
	    	You("learn how to perform thunderstorm!");
	}
	if (skill == P_ORB && P_SKILL(skill) == P_GRAND_MASTER && !tech_known(T_AUTOKILL)) {
	    	learntech(T_AUTOKILL, FROMOUTSIDE, 1);
	    	You("learn how to perform autokill!");
	}
	if (skill == P_ORB && P_SKILL(skill) == P_GRAND_MASTER && !tech_known(T_CHAIN_THUNDERBOLT)) {
	    	learntech(T_CHAIN_THUNDERBOLT, FROMOUTSIDE, 1);
	    	You("learn how to perform chain thunderbolt!");
	}
	if (skill == P_ORB && P_SKILL(skill) == P_GRAND_MASTER && !tech_known(T_FLASHING_MISCHIEF)) {
	    	learntech(T_FLASHING_MISCHIEF, FROMOUTSIDE, 1);
	    	You("learn how to perform flashing mischief!");
	}
	if (skill == P_CLAW && P_SKILL(skill) == P_BASIC && !tech_known(T_KAMEHAMEHA)) {
	    	learntech(T_KAMEHAMEHA, FROMOUTSIDE, 1);
	    	You("learn how to perform kamehameha!");
	}
	if (skill == P_CLAW && P_SKILL(skill) == P_SKILLED && !tech_known(T_SHADOW_MANTLE)) {
	    	learntech(T_SHADOW_MANTLE, FROMOUTSIDE, 1);
	    	You("learn how to perform shadow mantle!");
	}
	if (skill == P_CLAW && P_SKILL(skill) == P_EXPERT && !tech_known(T_VACUUM_STAR)) {
	    	learntech(T_VACUUM_STAR, FROMOUTSIDE, 1);
	    	You("learn how to perform vacuum star!");
	}
	if (skill == P_CLAW && P_SKILL(skill) == P_MASTER && !tech_known(T_BLADE_SHIELD)) {
	    	learntech(T_BLADE_SHIELD, FROMOUTSIDE, 1);
	    	You("learn how to perform blade shield!");
	}
	if (skill == P_CLAW && P_SKILL(skill) == P_GRAND_MASTER && !tech_known(T_GREEN_WEAPON)) {
	    	learntech(T_GREEN_WEAPON, FROMOUTSIDE, 1);
	    	You("learn how to perform green weapon!");
	}
	if (skill == P_GRINDER && P_SKILL(skill) == P_BASIC && !tech_known(T_BALLSLIFF)) {
	    	learntech(T_BALLSLIFF, FROMOUTSIDE, 1);
	    	You("learn how to perform ballsliff!");
	}
	if (skill == P_GRINDER && P_SKILL(skill) == P_SKILLED && !tech_known(T_POLE_MELEE)) {
	    	learntech(T_POLE_MELEE, FROMOUTSIDE, 1);
	    	You("learn how to perform pole melee!");
	}
	if (skill == P_GRINDER && P_SKILL(skill) == P_EXPERT && !tech_known(T_CHOP_CHOP)) {
	    	learntech(T_CHOP_CHOP, FROMOUTSIDE, 1);
	    	You("learn how to perform chop chop!");
	}
	if (skill == P_GRINDER && P_SKILL(skill) == P_MASTER && !tech_known(T_BANISHMENT)) {
	    	learntech(T_BANISHMENT, FROMOUTSIDE, 1);
	    	You("learn how to perform banishment!");
	}
	if (skill == P_GRINDER && P_SKILL(skill) == P_GRAND_MASTER && !tech_known(T_PARTICIPATION_LOSS)) {
	    	learntech(T_PARTICIPATION_LOSS, FROMOUTSIDE, 1);
	    	You("learn how to perform participation loss!");
	}
	if (skill == P_CLAW && P_SKILL(skill) == P_EXPERT && P_SKILL(P_TWO_WEAPON_COMBAT) >= P_EXPERT && !tech_known(T_WEAPON_BLOCKER)) {
	    	learntech(T_WEAPON_BLOCKER, FROMOUTSIDE, 1);
	    	You("learn how to perform weapon blocker!");
	}
	if (skill == P_TWO_WEAPON_COMBAT && P_SKILL(skill) == P_EXPERT && P_SKILL(P_CLAW) >= P_EXPERT && !tech_known(T_WEAPON_BLOCKER)) {
	    	learntech(T_WEAPON_BLOCKER, FROMOUTSIDE, 1);
	    	You("learn how to perform weapon blocker!");
	}
	if (skill == P_ORB && P_SKILL(skill) == P_EXPERT && P_SKILL(P_MEMORIZATION) >= P_EXPERT && !tech_known(T_EXTRA_MEMORY)) {
	    	learntech(T_EXTRA_MEMORY, FROMOUTSIDE, 1);
	    	You("learn how to perform extra memory!");
	}
	if (skill == P_MEMORIZATION && P_SKILL(skill) == P_EXPERT && P_SKILL(P_ORB) >= P_EXPERT && !tech_known(T_EXTRA_MEMORY)) {
	    	learntech(T_EXTRA_MEMORY, FROMOUTSIDE, 1);
	    	You("learn how to perform extra memory!");
	}
	if (skill == P_GRINDER && P_SKILL(skill) == P_EXPERT && P_SKILL(P_LANCE) >= P_EXPERT && !tech_known(T_GRAP_SWAP)) {
	    	learntech(T_GRAP_SWAP, FROMOUTSIDE, 1);
	    	You("learn how to perform grap swap!");
	}
	if (skill == P_LANCE && P_SKILL(skill) == P_EXPERT && P_SKILL(P_GRINDER) >= P_EXPERT && !tech_known(T_GRAP_SWAP)) {
	    	learntech(T_GRAP_SWAP, FROMOUTSIDE, 1);
	    	You("learn how to perform grap swap!");
	}
	if (skill == P_GRINDER && P_SKILL(skill) == P_EXPERT && P_SKILL(P_CLAW) >= P_EXPERT && P_SKILL(P_ORB) >= P_EXPERT && !tech_known(T_DIABOLIC_MINION)) {
	    	learntech(T_DIABOLIC_MINION, FROMOUTSIDE, 1);
	    	You("learn how to perform diabolic minion!");
	}
	if (skill == P_CLAW && P_SKILL(skill) == P_EXPERT && P_SKILL(P_GRINDER) >= P_EXPERT && P_SKILL(P_ORB) >= P_EXPERT && !tech_known(T_DIABOLIC_MINION)) {
	    	learntech(T_DIABOLIC_MINION, FROMOUTSIDE, 1);
	    	You("learn how to perform diabolic minion!");
	}
	if (skill == P_ORB && P_SKILL(skill) == P_EXPERT && P_SKILL(P_CLAW) >= P_EXPERT && P_SKILL(P_GRINDER) >= P_EXPERT && !tech_known(T_DIABOLIC_MINION)) {
	    	learntech(T_DIABOLIC_MINION, FROMOUTSIDE, 1);
	    	You("learn how to perform diabolic minion!");
	}

	} /* demagogue check */

	if (Role_if(PM_BINDER) || Role_if(PM_ANACHRONOUNBINDER)) {

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
		case P_ORB:
			    HControlMagic |= FROMOUTSIDE; pline("Got control magic!"); break;
		break;
		case P_CLAW:
			    HStealth |= FROMOUTSIDE; pline("Got stealth!"); break;
		break;
		case P_GRINDER:
			    HExpBoost |= FROMOUTSIDE; pline("Got EXP boost!"); break;
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
		case P_OCCULT_SPELL:
			    HStun_resist |= FROMOUTSIDE; pline("Got stun resistance!"); break;
		break;
		case P_ELEMENTAL_SPELL:
			    HPoison_resistance |= FROMOUTSIDE; pline("Got poison resistance!"); break;
		break;
		case P_CHAOS_SPELL:
			    HConf_resist |= FROMOUTSIDE; pline("Got confusion resistance!"); break;
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
		case P_HIGH_HEELS:
			    HManaleech |= FROMOUTSIDE; pline("Got manaleech!"); break;
		break;
		case P_GENERAL_COMBAT:
			    HFire_resistance |= FROMOUTSIDE; pline("Got fire resistance!"); break;
		break;
		case P_SHIELD:
			    HCold_resistance |= FROMOUTSIDE; pline("Got cold resistance!"); break;
		break;
		case P_BODY_ARMOR:
			    HPoison_resistance |= FROMOUTSIDE; pline("Got poison resistance!"); break;
		break;
		case P_TWO_HANDED_WEAPON:
			    HShock_resistance |= FROMOUTSIDE; pline("Got shock resistance!"); break;
		break;
		case P_POLYMORPHING:
			    HPsi_resist |= FROMOUTSIDE; pline("Got psi resistance!"); break;
		break;
		case P_DEVICES:
			    HConf_resist |= FROMOUTSIDE; pline("Got confusion resistance!"); break;
		break;
		case P_SEARCHING:
			    HSleep_resistance |= FROMOUTSIDE; pline("Got sleep resistance!"); break;
		break;
		case P_SPIRITUALITY:
			    HStun_resist |= FROMOUTSIDE; pline("Got stun resistance!"); break;
		break;
		case P_PETKEEPING:
			    HTelepat |= FROMOUTSIDE; pline("Got telepathy!"); break;
		break;
		case P_MISSILE_WEAPONS:
			    HCont_resist |= FROMOUTSIDE; pline("Got contamination resistance!"); break;
		break;
		case P_IMPLANTS:
			    HFull_nutrient |= FROMOUTSIDE; pline("Got full nutrients!"); break;
		break;
		case P_TECHNIQUES:
			    HTechnicality |= FROMOUTSIDE; pline("Got technicality!"); break;
		break;
		case P_SEXY_FLATS:
			    HRegeneration |= FROMOUTSIDE; pline("Got regeneration!"); break;
		break;
		case P_MEMORIZATION:
			    HStealth |= FROMOUTSIDE; pline("Got stealth!"); break;
		break;
		case P_GUN_CONTROL:
			    HDiminishedBleeding |= FROMOUTSIDE; pline("Got diminished bleeding!"); break;
		break;
		case P_SQUEAKING:
			    HScentView |= FROMOUTSIDE; pline("Got scent view!"); break;
		break;
		case P_SYMBIOSIS:
			    HTelepat |= FROMOUTSIDE; pline("Got telepathy!"); break;
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
		case P_ORB:
				if (!tech_known(T_PREACHING)) {    	learntech(T_PREACHING, FROMOUTSIDE, 1);
			    	You("learn how to perform preaching!");
				}
		break;
		case P_CLAW:
				if (!tech_known(T_ON_THE_SAME_TEAM)) {    	learntech(T_ON_THE_SAME_TEAM, FROMOUTSIDE, 1);
			    	You("learn how to perform on the same team!");
				}
		break;
		case P_GRINDER:
				if (!tech_known(T_PERMAMORPH)) {    	learntech(T_PERMAMORPH, FROMOUTSIDE, 1);
			    	You("learn how to perform permamorph!");
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
		case P_OCCULT_SPELL:
				if (!tech_known(T_BLOOD_RITUAL)) {    	learntech(T_BLOOD_RITUAL, FROMOUTSIDE, 1);
			    	You("learn how to perform blood ritual!");
				}
		break;
		case P_ELEMENTAL_SPELL:
				if (!tech_known(T_ENT_S_POTION)) {    	learntech(T_ENT_S_POTION, FROMOUTSIDE, 1);
			    	You("learn how to perform ent's potion!");
				}
		break;
		case P_CHAOS_SPELL:
				if (!tech_known(T_LUCKY_GAMBLE)) {    	learntech(T_LUCKY_GAMBLE, FROMOUTSIDE, 1);
			    	You("learn how to perform lucky gamble!");
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
		case P_HIGH_HEELS:
				if (!tech_known(T_DOUBLE_TROUBLE)) {    	learntech(T_DOUBLE_TROUBLE, FROMOUTSIDE, 1);
			    	You("learn how to perform double trouble!");
				}
		break;
		case P_GENERAL_COMBAT:
				if (!tech_known(T_CONCENTRATING)) {    	learntech(T_CONCENTRATING, FROMOUTSIDE, 1);
			    	You("learn how to perform concentrating!");
				}
		break;
		case P_SHIELD:
				if (!tech_known(T_SHIELD_BASH)) {    	learntech(T_SHIELD_BASH, FROMOUTSIDE, 1);
			    	You("learn how to perform shield bash!");
				}
		break;
		case P_BODY_ARMOR:
				if (!tech_known(T_IRON_SKIN)) {    	learntech(T_IRON_SKIN, FROMOUTSIDE, 1);
			    	You("learn how to perform iron skin!");
				}
		break;
		case P_TWO_HANDED_WEAPON:
				if (!tech_known(T_EDDY_WIND)) {    	learntech(T_EDDY_WIND, FROMOUTSIDE, 1);
			    	You("learn how to perform eddy wind!");
				}
		break;
		case P_POLYMORPHING:
				if (!tech_known(T_POLYFORM)) {    	learntech(T_POLYFORM, FROMOUTSIDE, 1);
			    	You("learn how to perform polyform!");
				}
		break;
		case P_DEVICES:
				if (!tech_known(T_RECHARGE)) {    	learntech(T_RECHARGE, FROMOUTSIDE, 1);
			    	You("learn how to perform recharge!");
				}
		break;
		case P_SEARCHING:
				if (!tech_known(T_DOUBLE_THROWNAGE)) {    	learntech(T_DOUBLE_THROWNAGE, FROMOUTSIDE, 1);
			    	You("learn how to perform double thrownage!");
				}
		break;
		case P_SPIRITUALITY:
				if (!tech_known(T_SPIRITUALITY_CHECK)) {    	learntech(T_SPIRITUALITY_CHECK, FROMOUTSIDE, 1);
			    	You("learn how to perform spirituality check!");
				}
		break;
		case P_PETKEEPING:
				if (!tech_known(T_SUMMON_PET)) {    	learntech(T_SUMMON_PET, FROMOUTSIDE, 1);
			    	You("learn how to perform summon pet!");
				}
		break;
		case P_MEMORIZATION:
				if (!tech_known(T_ZAP_EM)) {    	learntech(T_ZAP_EM, FROMOUTSIDE, 1);
			    	You("learn how to perform zap em!");
				}
		break;
		case P_MISSILE_WEAPONS:
				if (!tech_known(T_DECONTAMINATE)) {    	learntech(T_DECONTAMINATE, FROMOUTSIDE, 1);
			    	You("learn how to perform decontaminate!");
				}
		break;
		case P_IMPLANTS:
				if (!tech_known(T_WONDERSPELL)) {    	learntech(T_WONDERSPELL, FROMOUTSIDE, 1);
			    	You("learn how to perform wonderspell!");
				}
		break;
		case P_TECHNIQUES:
				if (!tech_known(T_RESET_TECHNIQUE)) {    	learntech(T_RESET_TECHNIQUE, FROMOUTSIDE, 1);
			    	You("learn how to perform reset technique!");
				}
		break;
		case P_SEXY_FLATS:
				if (!tech_known(T_DIAMOND_BARRIER)) {    	learntech(T_DIAMOND_BARRIER, FROMOUTSIDE, 1);
			    	You("learn how to perform diamond barrier!");
				}
		break;
		case P_SQUEAKING:
				if (!tech_known(T_SKILLOMORPH)) {    	learntech(T_SKILLOMORPH, FROMOUTSIDE, 1);
			    	You("learn how to perform skillomorph!");
				}
		break;
		case P_GUN_CONTROL:
				if (!tech_known(T_CARD_TRICK)) {    	learntech(T_CARD_TRICK, FROMOUTSIDE, 1);
			    	You("learn how to perform card trick!");
				}
		break;
		case P_SYMBIOSIS:
				if (!tech_known(T_TERRAIN_CLEANUP)) {    	learntech(T_TERRAIN_CLEANUP, FROMOUTSIDE, 1);
			    	You("learn how to perform terrain cleanup!");
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
#ifdef DUMP_LOG
{
	return enhance_skill(FALSE);
}

void dump_weapon_skill()
{
	enhance_skill(TRUE);
}

int enhance_skill(boolean want_dump)
/* This is the original enhance_weapon_skill() function slightly modified
 * to write the skills to the dump file. I added the wrapper functions just
 * because it looked like the easiest way to add a parameter to the
 * function call. - Jukka Lahtinen, August 2001
 */
#endif
{
    int pass, i, n, len, longest,
	to_advance, eventually_advance, maxxed_cnt;
    char buf[BUFSZ], sklnambuf[BUFSZ], sklnambuftwo[BUFSZ];
    const char *prefix;
    menu_item *selected;
    anything any;
    winid win;
    boolean speedy = FALSE;
#ifdef DUMP_LOG
    char buf2[BUFSZ];
    boolean logged;
#endif

	boolean restrpass = 0;
	boolean restrdo = 0;

#ifdef WIZARD
#ifdef DUMP_LOG
	if (!want_dump)
#endif
	if (wizard && yn("Advance skills without practice?") == 'y')
	    speedy = TRUE;
#endif

	do {
	    /* find longest available skill name, count those that can advance */
	    to_advance = eventually_advance = maxxed_cnt = 0;
	    for (longest = 0, i = 0; i < P_NUM_SKILLS; i++) {
		if (P_RESTRICTED(i) && !(want_dump || wizard)) continue;

		/* stupidity... If you fucking have the fucking skill and fucking trained it while being fucking polymorphed,
		 * then there's ZERO FUCKING REASON TO NOT FUCKING SHOW IT IN THE FUCKING DUMP!!! --Amy */
		/*if (i == P_TWO_WEAPON_COMBAT &&
			youmonst.data->mattk[1].aatyp != AT_WEAP)
		    continue;*/

		if ((len = strlen(wpskillname(i))) > longest)
		    longest = len;
		if (can_advance(i, speedy)) to_advance++;
		else if (could_advance(i)) eventually_advance++;
		else if (peaked_skill(i)) maxxed_cnt++;
	    }

#ifdef DUMP_LOG
	    if (want_dump) {
		sprintf(buf2,"Your skills at the end (%d slot%s left)", u.weapon_slots, plur(u.weapon_slots)),
		dump("",buf2);
	    } else {
#endif
	    win = create_nhwindow(NHW_MENU);
	    start_menu(win);

	    /* start with a legend if any entries will be annotated
	       with "*" or "#" below */
	    if (eventually_advance > 0 || maxxed_cnt > 0) {
		any.a_void = 0;
		if (eventually_advance > 0) {
		    sprintf(buf,
			    "(Skill%s flagged by \"*\" may be enhanced %s.)",
			    plur(eventually_advance),
			    (u.ulevel < MAXULEV) ?
				"when you're more experienced" :
				"if skill slots become available");
		    add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE,
			     buf, MENU_UNSELECTED);
		}
		if (maxxed_cnt > 0) {
		    sprintf(buf,
		  "(Skill%s flagged by \"#\" cannot be enhanced any further.)",
			    plur(maxxed_cnt));
		    add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE,
			     buf, MENU_UNSELECTED);
		}
		add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE,
			     "", MENU_UNSELECTED);
	    }
#ifdef DUMP_LOG
	    } /* want_dump or not */
#endif

	    /* List the skills, making ones that could be advanced
	       selectable.  List the miscellaneous skills first.
	       Possible future enhancement:  list spell skills before
	       weapon skills for spellcaster roles. */

restrpasstwo:

	  for (pass = 0; pass < SIZE(skill_ranges); pass++)
	    for (i = skill_ranges[pass].first;
		 i <= skill_ranges[pass].last; i++) {
		/* Print headings for skill types */
		any.a_void = 0;
		if (i == skill_ranges[pass].first)
#ifdef DUMP_LOG
		if (want_dump) {
		    if (!restrpass && !restrdo) dump("  ",(char *)skill_ranges[pass].name);
		    else if (!restrdo) {
				dump("  ", "Restricted Skills");
				restrdo = TRUE;
		    }
		    logged=FALSE;
		} else 
#endif
		{
			if (!restrpass && !restrdo) add_menu(win, NO_GLYPH, &any, 0, 0, iflags.menu_headings,
			     skill_ranges[pass].name, MENU_UNSELECTED);
			else if (!restrdo) {add_menu(win, NO_GLYPH, &any, 0, 0, iflags.menu_headings, "Restricted Skills", MENU_UNSELECTED);
				restrdo = TRUE;
			}
		}
#ifdef DUMP_LOG
		if (want_dump) {
		    if (P_SKILL(i) >= P_UNSKILLED && !restrpass) {

			(void) skill_level_name(i, sklnambuf);
			(void) skill_level_name_max(i, sklnambuftwo);

		 	sprintf(buf2,"%-*s [%s] %s (%d of %d)",
			    longest, wpskillname(i), sklnambuf, sklnambuftwo, P_ADVANCE(i),
			    practice_needed_to_advance(P_SKILL(i), i) );
			dump("    ",buf2);
			logged=TRUE;
		    } else if (P_SKILL(i) < P_UNSKILLED && restrpass) {

			(void) skill_level_name(i, sklnambuf);
			(void) skill_level_name_max(i, sklnambuftwo);

		 	sprintf(buf2,"%-*s [%s] %s (%d of %d)",
			    longest, wpskillname(i), sklnambuf, sklnambuftwo, P_ADVANCE(i),
			    practice_needed_to_advance(P_SKILL(i), i) );
			dump("    ",buf2);
			logged=TRUE;
		    } else if (i == skill_ranges[pass].last && !logged) {
			dump("    ","(none)");
		    }
               } else {
#endif

		if (P_RESTRICTED(i) && (!(want_dump || wizard) || !restrpass) ) continue;
		if (!P_RESTRICTED(i) && restrpass) continue;
		/* I don't like the idea of hidden skills. They probably won't appear in the endgame dump either! --Amy */
		/*if (i == P_TWO_WEAPON_COMBAT &&
			youmonst.data->mattk[1].aatyp != AT_WEAP)
		    continue;*/
		/* And therefore we're showing the two-weapon combat skill at all times now. */
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
		if (wizard || RngeSkillReveal) {
		    if (!iflags.menu_tab_sep)
			sprintf(buf, " %s%-*s %-12s %-12s %4d(%4d)",
			    prefix, longest, wpskillname(i), sklnambuf, sklnambuftwo,
			    P_ADVANCE(i),
			    practice_needed_to_advance(P_SKILL(i), i));
		    else
			sprintf(buf, " %s%s\t%s\t%s\t%5d(%4d)",
			    prefix, wpskillname(i), sklnambuf, sklnambuftwo,
			    P_ADVANCE(i),
			    practice_needed_to_advance(P_SKILL(i), i));
		 } else
#endif
		{
		    if (!iflags.menu_tab_sep)
			sprintf(buf, " %s %-*s %s %s",
			    prefix, longest, wpskillname(i), sklnambuf, sklnambuftwo);
		    else
			sprintf(buf, " %s%s\t%s\t%s",
			    prefix, wpskillname(i), sklnambuf, sklnambuftwo);
		}
		any.a_int = can_advance(i, speedy) ? i+1 : 0;
		add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE,
			 buf, MENU_UNSELECTED);
#ifdef DUMP_LOG
		} /* !want_dump */
#endif
	    }

	    if (!restrpass && (wizard || want_dump)) {
		restrpass = TRUE;
		goto restrpasstwo;
	    }

	    strcpy(buf, (to_advance > 0) ? "Pick a skill to advance:" :
					   "Current skills:");
/*#ifdef WIZARD*/
/*	    if (wizard && !speedy)*/
		sprintf(eos(buf), "  (%d slot%s available)",
			u.weapon_slots, plur(u.weapon_slots));
/*#endif*/
#ifdef DUMP_LOG
	    if (want_dump) {
		dump("","");
		n=0;
	    } else {
#endif
	    end_menu(win, buf);
	    n = select_menu(win, to_advance ? PICK_ONE : PICK_NONE, &selected);
	    destroy_nhwindow(win);
	    if (n > 0) {
		n = selected[0].item.a_int - 1;	/* get item selected */
		free((void *)selected);
		skill_advance(n);
		restrpass = FALSE;
		restrdo = FALSE;
		/* check for more skills able to advance, if so then .. */
		for (n = i = 0; i < P_NUM_SKILLS; i++) {
		    if (can_advance(i, speedy)) {
			if (!speedy) You_feel("you could be more dangerous!");
			n++;
			break;
		    }
		}
	    }
#ifdef DUMP_LOG
	    }
#endif
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

	if (isdemagogue) {

		if (P_ADVANCE(skill) < degree) P_ADVANCE(skill) = 0;
		else P_ADVANCE(skill) -= degree;

		register int tryct, tryct2, i;

		if (!P_RESTRICTED(skill)) {

			tryct = 2000;
			tryct2 = 10;
			i = 0;

			while (u.skills_advanced && tryct && (P_ADVANCE(skill) < practice_needed_to_advance_nonmax(P_SKILL(skill) - 1, skill) ) ) {
				lose_last_spent_skill();
				i++;
				tryct--;

			}

			while (i) {
				if (evilfriday) pline("This is the evil variant. Your skill point is lost forever.");
				else u.weapon_slots++;  /* because every skill up costs one slot --Amy */
				i--;
			}

			/* still higher than the cap? that probably means you started with some knowledge of the skill... */
			while (tryct2 && P_ADVANCE(skill) < practice_needed_to_advance_nonmax(P_SKILL(skill) - 1, skill) ) {
				P_SKILL(skill)--;
				if (evilfriday) pline("This is the evil variant. Your skill point is lost forever.");
				else u.weapon_slots++;
				tryct2--;

			}

		}

		/* learn techniques if you get a skill to zero */
		if (P_ADVANCE(skill) == 0) {

			if (skill == P_TRIDENT && !tech_known(T_SILENT_OCEAN)) {
				learntech(T_SILENT_OCEAN, FROMOUTSIDE, 1);
				You("learn how to perform silent ocean!");
			}
			if (skill == P_UNICORN_HORN && !tech_known(T_GLOWHORN)) {
				learntech(T_GLOWHORN, FROMOUTSIDE, 1);
				You("learn how to perform glowhorn!");
			}
			if (skill == P_UNICORN_HORN && !tech_known(T_STAT_RESIST)) {
				learntech(T_STAT_RESIST, FROMOUTSIDE, 1);
				You("learn how to perform stat resist!");
			}
			if (skill == P_CHAOS_SPELL && !tech_known(T_INTRINSIC_ROULETTE)) {
				learntech(T_INTRINSIC_ROULETTE, FROMOUTSIDE, 1);
				You("learn how to perform intrinsic roulette!");
			}
			if (skill == P_ELEMENTAL_SPELL && !tech_known(T_SPECTRAL_SWORD)) {
				learntech(T_SPECTRAL_SWORD, FROMOUTSIDE, 1);
				You("learn how to perform spectral sword!");
			}
			if (skill == P_OCCULT_SPELL && !tech_known(T_REVERSE_IDENTIFY)) {
				learntech(T_REVERSE_IDENTIFY, FROMOUTSIDE, 1);
				You("learn how to perform reverse identify!");
			}
			if (skill == P_SEARCHING && !tech_known(T_DETECT_TRAPS)) {
				learntech(T_DETECT_TRAPS, FROMOUTSIDE, 1);
				You("learn how to perform detect traps!");
			}
			if (skill == P_PETKEEPING && !tech_known(T_DIRECTIVE)) {
				learntech(T_DIRECTIVE, FROMOUTSIDE, 1);
				You("learn how to perform directive!");
			}
			if (skill == P_MEMORIZATION && !tech_known(T_WONDER_YONDER)) {
				learntech(T_WONDER_YONDER, FROMOUTSIDE, 1);
				You("learn how to perform wonder yonder!");
			}
			if (skill == P_TWO_WEAPON_COMBAT && !tech_known(T_SWAP_WEAPON)) {
				learntech(T_SWAP_WEAPON, FROMOUTSIDE, 1);
				You("learn how to perform swap weapon!");
			}
			if (skill == P_IMPLANTS && !tech_known(T_REMOVE_IMPLANT)) {
				learntech(T_REMOVE_IMPLANT, FROMOUTSIDE, 1);
				You("learn how to perform remove implant!");
			}
			if (skill == P_IMPLANTS && !tech_known(T_REROLL_IMPLANT)) {
				learntech(T_REROLL_IMPLANT, FROMOUTSIDE, 1);
				You("learn how to perform reroll implant!");
			}
			if (skill == P_TECHNIQUES && !tech_known(T_TIME_STOP)) {
				learntech(T_TIME_STOP, FROMOUTSIDE, 1);
				You("learn how to perform time stop!");
			}
			if (skill == P_LANCE && !tech_known(T_MILDEN_CURSE)) {
				learntech(T_MILDEN_CURSE, FROMOUTSIDE, 1);
				You("learn how to perform milden curse!");
			}
			if (skill == P_POLEARMS && !tech_known(T_FORCE_FIELD)) {
				learntech(T_FORCE_FIELD, FROMOUTSIDE, 1);
				You("learn how to perform force field!");
			}
			if (skill == P_POLEARMS && !tech_known(T_POINTINESS)) {
				learntech(T_POINTINESS, FROMOUTSIDE, 1);
				You("learn how to perform pointiness!");
			}
			if (skill == P_PADDLE && !tech_known(T_BUG_SPRAY)) {
				learntech(T_BUG_SPRAY, FROMOUTSIDE, 1);
				You("learn how to perform bug spray!");
			}
			if (skill == P_QUARTERSTAFF && !tech_known(T_WHIRLSTAFF)) {
				learntech(T_WHIRLSTAFF, FROMOUTSIDE, 1);
				You("learn how to perform whirlstaff!");
			}
			if (skill == P_CROSSBOW && !tech_known(T_DELIBERATE_CURSE)) {
				learntech(T_DELIBERATE_CURSE, FROMOUTSIDE, 1);
				You("learn how to perform deliberate curse!");
			}
			if (skill == P_RIDING && !tech_known(T_ACQUIRE_STEED)) {
				learntech(T_ACQUIRE_STEED, FROMOUTSIDE, 1);
				You("learn how to perform acquire steed!");
			}
			if (skill == P_RIDING && !tech_known(T_SADDLING)) {
				learntech(T_SADDLING, FROMOUTSIDE, 1);
				You("learn how to perform saddling!");
			}
			if (skill == P_HIGH_HEELS && !tech_known(T_SHOPPING_QUEEN)) {
				learntech(T_SHOPPING_QUEEN, FROMOUTSIDE, 1);
				You("learn how to perform shopping queen!");
			}
			if (skill == P_HIGH_HEELS && !tech_known(T_BEAUTY_CHARM)) {
				learntech(T_BEAUTY_CHARM, FROMOUTSIDE, 1);
				You("learn how to perform beauty charm!");
			}
			if (skill == P_HIGH_HEELS && !tech_known(T_ASIAN_KICK)) {
				learntech(T_ASIAN_KICK, FROMOUTSIDE, 1);
				You("learn how to perform asian kick!");
			}
			if (skill == P_HIGH_HEELS && !tech_known(T_LEGSCRATCH_ATTACK)) {
				learntech(T_LEGSCRATCH_ATTACK, FROMOUTSIDE, 1);
				You("learn how to perform legscratch attack!");
			}
			if (skill == P_HIGH_HEELS && !tech_known(T_GROUND_STOMP)) {
				learntech(T_GROUND_STOMP, FROMOUTSIDE, 1);
				You("learn how to perform ground stomp!");
			}
			if (skill == P_HIGH_HEELS && !tech_known(T_ATHLETIC_COMBAT)) {
				learntech(T_ATHLETIC_COMBAT, FROMOUTSIDE, 1);
				You("learn how to perform athletic combat!");
			}
			if (skill == P_SPIRITUALITY && !tech_known(T_PRAYING_SUCCESS)) {
				learntech(T_PRAYING_SUCCESS, FROMOUTSIDE, 1);
				You("learn how to perform praying success!");
			}
			if (skill == P_DEVICES && !tech_known(T_OVER_RAY)) {
				learntech(T_OVER_RAY, FROMOUTSIDE, 1);
				You("learn how to perform over-ray!");
			}
			if (skill == P_BOOMERANG && !tech_known(T_ENCHANTERANG)) {
				learntech(T_ENCHANTERANG, FROMOUTSIDE, 1);
				You("learn how to perform enchanterang!");
			}
			if (skill == P_BOOMERANG && !tech_known(T_BATMAN_ARSENAL)) {
				learntech(T_BATMAN_ARSENAL, FROMOUTSIDE, 1);
				You("learn how to perform batman arsenal!");
			}
			if (skill == P_BOOMERANG && !tech_known(T_JOKERBANE)) {
				learntech(T_JOKERBANE, FROMOUTSIDE, 1);
				You("learn how to perform jokerbane!");
			}
			if (skill == P_WHIP && !tech_known(T_CALL_THE_POLICE)) {
				learntech(T_CALL_THE_POLICE, FROMOUTSIDE, 1);
				You("learn how to perform call the police!");
			}
			if (skill == P_WHIP && !tech_known(T_DOMINATE)) {
				learntech(T_DOMINATE, FROMOUTSIDE, 1);
				You("learn how to perform dominate!");
			}
			if (skill == P_WHIP && !tech_known(T_INCARNATION)) {
				learntech(T_INCARNATION, FROMOUTSIDE, 1);
				You("learn how to perform incarnation!");
			}
			if (skill == P_MARTIAL_ARTS && !tech_known(T_COMBO_STRIKE)) {
				learntech(T_COMBO_STRIKE, FROMOUTSIDE, 1);
				You("learn how to perform combo strike!");
			}
			if (skill == P_POLYMORPHING && !tech_known(T_FUNGOISM)) {
				learntech(T_FUNGOISM, FROMOUTSIDE, 1);
				You("learn how to perform fungoism!");
			}
			if (skill == P_POLYMORPHING && !tech_known(T_BECOME_UNDEAD)) {
				learntech(T_BECOME_UNDEAD, FROMOUTSIDE, 1);
				You("learn how to perform become undead!");
			}
			if (skill == P_BARE_HANDED_COMBAT && !tech_known(T_JIU_JITSU)) {
				learntech(T_JIU_JITSU, FROMOUTSIDE, 1);
				You("learn how to perform jiu-jitsu!");
			}
			if (skill == P_SHURIKEN && !tech_known(T_BLADE_ANGER)) {
				learntech(T_BLADE_ANGER, FROMOUTSIDE, 1);
				You("learn how to perform blade anger!");
			}
			if (skill == P_PETKEEPING && !tech_known(T_RE_TAMING)) {
				learntech(T_RE_TAMING, FROMOUTSIDE, 1);
				You("learn how to perform re-taming!");
			}
			if (skill == P_SHII_CHO && !tech_known(T_UNCURSE_SABER)) {
				learntech(T_UNCURSE_SABER, FROMOUTSIDE, 1);
				You("learn how to perform uncurse saber!");
			}
			if (skill == P_MAKASHI && !tech_known(T_ENERGY_CONSERVATION)) {
				learntech(T_ENERGY_CONSERVATION, FROMOUTSIDE, 1);
				You("learn how to perform energy conservation!");
			}
			if (skill == P_SORESU && !tech_known(T_ENCHANT_ROBE)) {
				learntech(T_ENCHANT_ROBE, FROMOUTSIDE, 1);
				You("learn how to perform enchant robe!");
			}
			if (skill == P_ATARU && !tech_known(T_WILD_SLASHING)) {
				learntech(T_WILD_SLASHING, FROMOUTSIDE, 1);
				You("learn how to perform wild slashing!");
			}
			if (skill == P_SHIEN && !tech_known(T_ABSORBER_SHIELD)) {
				learntech(T_ABSORBER_SHIELD, FROMOUTSIDE, 1);
				You("learn how to perform absorber shield!");
			}
			if (skill == P_DJEM_SO && !tech_known(T_PSYCHO_FORCE)) {
				learntech(T_PSYCHO_FORCE, FROMOUTSIDE, 1);
				You("learn how to perform psycho force!");
			}
			if (skill == P_NIMAN && !tech_known(T_INTENSIVE_TRAINING)) {
				learntech(T_INTENSIVE_TRAINING, FROMOUTSIDE, 1);
				You("learn how to perform intensive training!");
			}
			if (skill == P_JUYO && !tech_known(T_SURRENDER_OR_DIE)) {
				learntech(T_SURRENDER_OR_DIE, FROMOUTSIDE, 1);
				You("learn how to perform surrender or die!");
			}
			if (skill == P_VAAPAD && !tech_known(T_PERILOUS_WHIRL)) {
				learntech(T_PERILOUS_WHIRL, FROMOUTSIDE, 1);
				You("learn how to perform perilous whirl!");
			}
			if (skill == P_PETKEEPING && P_ADVANCE(P_HIGH_HEELS) == 0 && !tech_known(T_SUMMON_SHOE)) {
				learntech(T_SUMMON_SHOE, FROMOUTSIDE, 1);
				You("learn how to perform summon shoe!");
			}
			if (skill == P_HIGH_HEELS && P_ADVANCE(P_PETKEEPING) == 0 && !tech_known(T_SUMMON_SHOE)) {
				learntech(T_SUMMON_SHOE, FROMOUTSIDE, 1);
				You("learn how to perform summon shoe!");
			}
			if (skill == P_PETKEEPING && P_ADVANCE(P_HIGH_HEELS) == 0 && !tech_known(T_SUMMON_SHOE)) {
				learntech(T_SUMMON_SHOE, FROMOUTSIDE, 1);
				You("learn how to perform summon shoe!");
			}
			if (skill == P_HIGH_HEELS && P_ADVANCE(P_PETKEEPING) == 0 && !tech_known(T_SUMMON_SHOE)) {
				learntech(T_SUMMON_SHOE, FROMOUTSIDE, 1);
				You("learn how to perform summon shoe!");
			}
			if (skill == P_SEXY_FLATS && !tech_known(T_KICK_IN_THE_NUTS)) {
				learntech(T_KICK_IN_THE_NUTS, FROMOUTSIDE, 1);
				You("learn how to perform kick in the nuts!");
			}
			if (skill == P_SEXY_FLATS && !tech_known(T_DISARMING_KICK)) {
				learntech(T_DISARMING_KICK, FROMOUTSIDE, 1);
				You("learn how to perform disarming kick!");
			}
			if (skill == P_SEXY_FLATS && !tech_known(T_INLAY_WARFARE)) {
				learntech(T_INLAY_WARFARE, FROMOUTSIDE, 1);
				You("learn how to perform inlay warfare!");
			}

			if (skill == P_SHII_CHO && !tech_known(T_STEADY_HAND)) {
				learntech(T_STEADY_HAND, FROMOUTSIDE, 1);
				You("learn how to perform steady hand!");
			}
			if (skill == P_MAKASHI && !tech_known(T_FORCE_FILLING)) {
				learntech(T_FORCE_FILLING, FROMOUTSIDE, 1);
				You("learn how to perform force filling!");
			}
			if (skill == P_SORESU && !tech_known(T_JEDI_TAILORING)) {
				learntech(T_JEDI_TAILORING, FROMOUTSIDE, 1);
				You("learn how to perform jedi tailoring!");
			}
			if (skill == P_ATARU && !tech_known(T_INTRINSIC_SACRIFICE)) {
				learntech(T_INTRINSIC_SACRIFICE, FROMOUTSIDE, 1);
				You("learn how to perform intrinsic sacrifice!");
			}
			if (skill == P_DJEM_SO && !tech_known(T_BEAMSWORD)) {
				learntech(T_BEAMSWORD, FROMOUTSIDE, 1);
				You("learn how to perform beamsword!");
			}
			if (skill == P_NIMAN && !tech_known(T_ENERGY_TRANSFER)) {
				learntech(T_ENERGY_TRANSFER, FROMOUTSIDE, 1);
				You("learn how to perform energy transfer!");
			}
			if (skill == P_JUYO && !tech_known(T_SOFTEN_TARGET)) {
				learntech(T_SOFTEN_TARGET, FROMOUTSIDE, 1);
				You("learn how to perform soften target!");
			}
			if (skill == P_VAAPAD && !tech_known(T_PROTECT_WEAPON)) {
				learntech(T_PROTECT_WEAPON, FROMOUTSIDE, 1);
				You("learn how to perform protect weapon!");
			}
			if (skill == P_SHIEN && !tech_known(T_POWERFUL_AURA)) {
				learntech(T_POWERFUL_AURA, FROMOUTSIDE, 1);
				You("learn how to perform powerful aura!");
			}

			if (skill == P_NIMAN && P_ADVANCE(P_QUARTERSTAFF) == 0 && !tech_known(T_BOOSTAFF)) {
				learntech(T_BOOSTAFF, FROMOUTSIDE, 1);
				You("learn how to perform boostaff!");
			}
			if (skill == P_QUARTERSTAFF && P_ADVANCE(P_NIMAN) == 0 && !tech_known(T_BOOSTAFF)) {
				learntech(T_BOOSTAFF, FROMOUTSIDE, 1);
				You("learn how to perform boostaff!");
			}

			if (skill == P_MAKASHI && P_ADVANCE(P_JAVELIN) == 0 && !tech_known(T_CLONE_JAVELIN)) {
				learntech(T_CLONE_JAVELIN, FROMOUTSIDE, 1);
				You("learn how to perform clone javelin!");
			}
			if (skill == P_JAVELIN && P_ADVANCE(P_MAKASHI) == 0 && !tech_known(T_CLONE_JAVELIN)) {
				learntech(T_CLONE_JAVELIN, FROMOUTSIDE, 1);
				You("learn how to perform clone javelin!");
			}

			if (skill == P_MACE && P_ADVANCE(P_SHII_CHO) == 0 && !tech_known(T_REFUGE)) {
				learntech(T_REFUGE, FROMOUTSIDE, 1);
				You("learn how to perform refuge!");
			}
			if (skill == P_SHII_CHO && P_ADVANCE(P_MACE) == 0 && !tech_known(T_REFUGE)) {
				learntech(T_REFUGE, FROMOUTSIDE, 1);
				You("learn how to perform refuge!");
			}

			if (skill == P_SORESU && P_ADVANCE(P_MARTIAL_ARTS) == 0 && !tech_known(T_DRAINING_PUNCH)) {
				learntech(T_DRAINING_PUNCH, FROMOUTSIDE, 1);
				You("learn how to perform draining punch!");
			}
			if (skill == P_MARTIAL_ARTS && P_ADVANCE(P_SORESU) == 0 && !tech_known(T_DRAINING_PUNCH)) {
				learntech(T_DRAINING_PUNCH, FROMOUTSIDE, 1);
				You("learn how to perform draining punch!");
			}
			if (skill == P_SORESU && P_ADVANCE(P_MARTIAL_ARTS) == 0 && !tech_known(T_DRAINING_PUNCH)) {
				learntech(T_DRAINING_PUNCH, FROMOUTSIDE, 1);
				You("learn how to perform draining punch!");
			}
			if (skill == P_MARTIAL_ARTS && P_ADVANCE(P_SORESU) == 0 && !tech_known(T_DRAINING_PUNCH)) {
				learntech(T_DRAINING_PUNCH, FROMOUTSIDE, 1);
				You("learn how to perform draining punch!");
			}

			if (skill == P_SORESU && P_ADVANCE(P_BARE_HANDED_COMBAT) == 0 && !tech_known(T_ESCROBISM)) {
				learntech(T_ESCROBISM, FROMOUTSIDE, 1);
				You("learn how to perform escrobism!");
			}
			if (skill == P_BARE_HANDED_COMBAT && P_ADVANCE(P_SORESU) == 0 && !tech_known(T_ESCROBISM)) {
				learntech(T_ESCROBISM, FROMOUTSIDE, 1);
				You("learn how to perform escrobism!");
			}

			if (skill == P_ATARU && P_ADVANCE(P_SCIMITAR) == 0 && !tech_known(T_PIRATE_BROTHERING)) {
				learntech(T_PIRATE_BROTHERING, FROMOUTSIDE, 1);
				You("learn how to perform pirate brothering!");
			}
			if (skill == P_SCIMITAR && P_ADVANCE(P_ATARU) == 0 && !tech_known(T_PIRATE_BROTHERING)) {
				learntech(T_PIRATE_BROTHERING, FROMOUTSIDE, 1);
				You("learn how to perform pirate brothering!");
			}

			if (skill == P_DJEM_SO && P_ADVANCE(P_CROSSBOW) == 0 && !tech_known(T_NUTS_AND_BOLTS)) {
				learntech(T_NUTS_AND_BOLTS, FROMOUTSIDE, 1);
				You("learn how to perform nuts and bolts!");
			}
			if (skill == P_CROSSBOW && P_ADVANCE(P_DJEM_SO) == 0 && !tech_known(T_NUTS_AND_BOLTS)) {
				learntech(T_NUTS_AND_BOLTS, FROMOUTSIDE, 1);
				You("learn how to perform nuts and bolts!");
			}
			if (skill == P_DJEM_SO && P_ADVANCE(P_CROSSBOW) == 0 && !tech_known(T_NUTS_AND_BOLTS)) {
				learntech(T_NUTS_AND_BOLTS, FROMOUTSIDE, 1);
				You("learn how to perform nuts and bolts!");
			}
			if (skill == P_CROSSBOW && P_ADVANCE(P_DJEM_SO) == 0 && !tech_known(T_NUTS_AND_BOLTS)) {
				learntech(T_NUTS_AND_BOLTS, FROMOUTSIDE, 1);
				You("learn how to perform nuts and bolts!");
			}

			if (skill == P_JUYO && P_ADVANCE(P_POLEARMS) == 0 && !tech_known(T_DECAPABILITY)) {
				learntech(T_DECAPABILITY, FROMOUTSIDE, 1);
				You("learn how to perform decapability!");
			}
			if (skill == P_POLEARMS && P_ADVANCE(P_JUYO) == 0 && !tech_known(T_DECAPABILITY)) {
				learntech(T_DECAPABILITY, FROMOUTSIDE, 1);
				You("learn how to perform decapability!");
			}
			if (skill == P_JUYO && P_ADVANCE(P_POLEARMS) == 0 && !tech_known(T_DECAPABILITY)) {
				learntech(T_DECAPABILITY, FROMOUTSIDE, 1);
				You("learn how to perform decapability!");
			}
			if (skill == P_POLEARMS && P_ADVANCE(P_JUYO) == 0 && !tech_known(T_DECAPABILITY)) {
				learntech(T_DECAPABILITY, FROMOUTSIDE, 1);
				You("learn how to perform decapability!");
			}

			if (skill == P_VAAPAD && P_ADVANCE(P_TWO_HANDED_SWORD) == 0 && !tech_known(T_NO_HANDS_CURSE)) {
				learntech(T_NO_HANDS_CURSE, FROMOUTSIDE, 1);
				You("learn how to perform no-hands curse!");
			}
			if (skill == P_TWO_HANDED_SWORD && P_ADVANCE(P_VAAPAD) == 0 && !tech_known(T_NO_HANDS_CURSE)) {
				learntech(T_NO_HANDS_CURSE, FROMOUTSIDE, 1);
				You("learn how to perform no-hands curse!");
			}

			if (skill == P_SEXY_FLATS && P_ADVANCE(P_HIGH_HEELS) == 0 && !tech_known(T_HIGH_HEELED_SNEAKERS)) {
				learntech(T_HIGH_HEELED_SNEAKERS, FROMOUTSIDE, 1);
				You("learn how to perform high-heeled sneakers!");
			}
			if (skill == P_HIGH_HEELS && P_ADVANCE(P_SEXY_FLATS) == 0 && !tech_known(T_HIGH_HEELED_SNEAKERS)) {
				learntech(T_HIGH_HEELED_SNEAKERS, FROMOUTSIDE, 1);
				You("learn how to perform high-heeled sneakers!");
			}

			if (skill == P_GUN_CONTROL && !tech_known(T_SHOTTY_BLAST)) {
			    	learntech(T_SHOTTY_BLAST, FROMOUTSIDE, 1);
			    	You("learn how to perform shotty blast!");
			}
			if (skill == P_GUN_CONTROL && !tech_known(T_AMMO_UPGRADE)) {
			    	learntech(T_AMMO_UPGRADE, FROMOUTSIDE, 1);
			    	You("learn how to perform ammo upgrade!");
			}
			if (skill == P_GUN_CONTROL && !tech_known(T_LASER_POWER)) {
			    	learntech(T_LASER_POWER, FROMOUTSIDE, 1);
			    	You("learn how to perform laser power!");
			}
			if (skill == P_GUN_CONTROL && !tech_known(T_BIG_DADDY)) {
			    	learntech(T_BIG_DADDY, FROMOUTSIDE, 1);
			    	You("learn how to perform big daddy!");
			}
			if (skill == P_GUN_CONTROL && !tech_known(T_SHUT_THAT_BITCH_UP)) {
			    	learntech(T_SHUT_THAT_BITCH_UP, FROMOUTSIDE, 1);
			    	You("learn how to perform shut that bitch up!");
			}
			if (skill == P_SQUEAKING && !tech_known(T_S_PRESSING)) {
			    	learntech(T_S_PRESSING, FROMOUTSIDE, 1);
			    	You("learn how to perform s-pressing!");
			}
			if (skill == P_SQUEAKING && !tech_known(T_MELTEE)) {
			    	learntech(T_MELTEE, FROMOUTSIDE, 1);
			    	You("learn how to perform meltee!");
			}
			if (skill == P_SQUEAKING && !tech_known(T_WOMAN_NOISES)) {
			    	learntech(T_WOMAN_NOISES, FROMOUTSIDE, 1);
			    	You("learn how to perform woman noises!");
			}
			if (skill == P_SQUEAKING && !tech_known(T_EXTRA_LONG_SQUEAK)) {
			    	learntech(T_EXTRA_LONG_SQUEAK, FROMOUTSIDE, 1);
			    	You("learn how to perform extra long squeak!");
			}
			if (skill == P_SQUEAKING && !tech_known(T_SEXUAL_HUG)) {
			    	learntech(T_SEXUAL_HUG, FROMOUTSIDE, 1);
			    	You("learn how to perform sexual hug!");
			}
			if (skill == P_GUN_CONTROL && P_ADVANCE(P_SQUEAKING) == 0 && !tech_known(T_SEX_CHANGE)) {
			    	learntech(T_SEX_CHANGE, FROMOUTSIDE, 1);
			    	You("learn how to perform sex change!");
			}
			if (skill == P_SQUEAKING && P_ADVANCE(P_GUN_CONTROL) == 0 && !tech_known(T_SEX_CHANGE)) {
			    	learntech(T_SEX_CHANGE, FROMOUTSIDE, 1);
			    	You("learn how to perform sex change!");
			}
			if (skill == P_GUN_CONTROL && P_ADVANCE(P_FIREARM) == 0 && !tech_known(T_EVEN_MORE_AMMO)) {
			    	learntech(T_EVEN_MORE_AMMO, FROMOUTSIDE, 1);
			    	You("learn how to perform even more ammo!");
			}
			if (skill == P_FIREARM && P_ADVANCE(P_GUN_CONTROL) == 0 && !tech_known(T_EVEN_MORE_AMMO)) {
			    	learntech(T_EVEN_MORE_AMMO, FROMOUTSIDE, 1);
			    	You("learn how to perform even more ammo!");
			}
			if (skill == P_SQUEAKING && P_ADVANCE(P_POLYMORPHING) == 0 && !tech_known(T_DOUBLESELF)) {
			    	learntech(T_DOUBLESELF, FROMOUTSIDE, 1);
			    	You("learn how to perform doubleself!");
			}
			if (skill == P_SQUEAKING && P_ADVANCE(P_POLYMORPHING) == 0 && !tech_known(T_DOUBLESELF)) {
			    	learntech(T_DOUBLESELF, FROMOUTSIDE, 1);
			    	You("learn how to perform doubleself!");
			}
			if (skill == P_CHAOS_SPELL && P_ADVANCE(P_POLYMORPHING) == 0 && !tech_known(T_POLYFIX)) {
			    	learntech(T_POLYFIX, FROMOUTSIDE, 1);
			    	You("learn how to perform polyfix!");
			}
			if (skill == P_POLYMORPHING && P_ADVANCE(P_CHAOS_SPELL) == 0 && !tech_known(T_POLYFIX)) {
			    	learntech(T_POLYFIX, FROMOUTSIDE, 1);
			    	You("learn how to perform polyfix!");
			}
			if (skill == P_OCCULT_SPELL && P_ADVANCE(P_SQUEAKING) == 0 && !tech_known(T_SQUEAKY_REPAIR)) {
			    	learntech(T_SQUEAKY_REPAIR, FROMOUTSIDE, 1);
			    	You("learn how to perform squeaky repair!");
			}
			if (skill == P_SQUEAKING && P_ADVANCE(P_OCCULT_SPELL) == 0 && !tech_known(T_SQUEAKY_REPAIR)) {
			    	learntech(T_SQUEAKY_REPAIR, FROMOUTSIDE, 1);
			    	You("learn how to perform squeaky repair!");
			}
			if (skill == P_OCCULT_SPELL && P_ADVANCE(P_GUN_CONTROL) == 0 && !tech_known(T_BULLETREUSE)) {
			    	learntech(T_BULLETREUSE, FROMOUTSIDE, 1);
			    	You("learn how to perform bulletreuse!");
			}
			if (skill == P_GUN_CONTROL && P_ADVANCE(P_OCCULT_SPELL) == 0 && !tech_known(T_BULLETREUSE)) {
			    	learntech(T_BULLETREUSE, FROMOUTSIDE, 1);
			    	You("learn how to perform bulletreuse!");
			}
			if (skill == P_ELEMENTAL_SPELL && P_ADVANCE(P_DEVICES) == 0 && !tech_known(T_EXTRACHARGE)) {
			    	learntech(T_EXTRACHARGE, FROMOUTSIDE, 1);
			    	You("learn how to perform extracharge!");
			}
			if (skill == P_DEVICES && P_ADVANCE(P_ELEMENTAL_SPELL) == 0 && !tech_known(T_EXTRACHARGE)) {
			    	learntech(T_EXTRACHARGE, FROMOUTSIDE, 1);
			    	You("learn how to perform extracharge!");
			}

			if (skill == P_MAKASHI && P_ADVANCE(P_ATARU) == 0 && P_ADVANCE(P_VAAPAD) == 0 && !tech_known(T_FORM_CHOICE)) {
				learntech(T_FORM_CHOICE, FROMOUTSIDE, 1);
				You("learn how to perform form choice!");
			}
			if (skill == P_VAAPAD && P_ADVANCE(P_MAKASHI) == 0 && P_ADVANCE(P_ATARU) == 0 && !tech_known(T_FORM_CHOICE)) {
				learntech(T_FORM_CHOICE, FROMOUTSIDE, 1);
				You("learn how to perform form choice!");
			}
			if (skill == P_ATARU && P_ADVANCE(P_VAAPAD) == 0 && P_ADVANCE(P_MAKASHI) == 0 && !tech_known(T_FORM_CHOICE)) {
				learntech(T_FORM_CHOICE, FROMOUTSIDE, 1);
				You("learn how to perform form choice!");
			}

			if (skill == P_WEDI && !tech_known(T_STAR_DIGGING)) {
				learntech(T_STAR_DIGGING, FROMOUTSIDE, 1);
				You("learn how to perform star digging!");
			}
			if (skill == P_WEDI && !tech_known(T_STARWARS_FRIENDS)) {
				learntech(T_STARWARS_FRIENDS, FROMOUTSIDE, 1);
				You("learn how to perform starwars friends!");
			}

			if (skill == P_WEDI && P_ADVANCE(P_PICK_AXE) == 0 && !tech_known(T_USE_THE_FORCE_LUKE)) {
				learntech(T_USE_THE_FORCE_LUKE, FROMOUTSIDE, 1);
				You("learn how to perform use the force luke!");
			}
			if (skill == P_PICK_AXE && P_ADVANCE(P_WEDI) == 0 && !tech_known(T_USE_THE_FORCE_LUKE)) {
				learntech(T_USE_THE_FORCE_LUKE, FROMOUTSIDE, 1);
				You("learn how to perform use the force luke!");
			}

			if (skill == P_SYMBIOSIS && !tech_known(T_SYMBIOSIS)) {
			    	learntech(T_SYMBIOSIS, FROMOUTSIDE, 1);
			    	You("learn how to perform symbiosis!");
			}
			if (skill == P_SYMBIOSIS && !tech_known(T_ADJUST_SYMBIOTE)) {
			    	learntech(T_ADJUST_SYMBIOTE, FROMOUTSIDE, 1);
			    	You("learn how to perform adjust symbiote!");
			}
			if (skill == P_SYMBIOSIS && !tech_known(T_HEAL_SYMBIOTE)) {
			    	learntech(T_HEAL_SYMBIOTE, FROMOUTSIDE, 1);
			    	You("learn how to perform heal symbiote!");
			}
			if (skill == P_SYMBIOSIS && !tech_known(T_BOOST_SYMBIOTE)) {
			    	learntech(T_BOOST_SYMBIOTE, FROMOUTSIDE, 1);
			    	You("learn how to perform boost symbiote!");
			}
			if (skill == P_SYMBIOSIS && !tech_known(T_POWERBIOSIS)) {
			    	learntech(T_POWERBIOSIS, FROMOUTSIDE, 1);
			    	You("learn how to perform powerbiosis!");
			}
			if (skill == P_SHORT_SWORD && !tech_known(T_UNDERTOW)) {
			    	learntech(T_UNDERTOW, FROMOUTSIDE, 1);
			    	You("learn how to perform undertow!");
			}
			if (skill == P_FIREARM && !tech_known(T_GRENADES_OF_COURSE)) {
			    	learntech(T_GRENADES_OF_COURSE, FROMOUTSIDE, 1);
			    	You("learn how to perform grenades of course!");
			}
			if (skill == P_CLUB && !tech_known(T_DEFINALIZE)) {
			    	learntech(T_DEFINALIZE, FROMOUTSIDE, 1);
			    	You("learn how to perform definalize!");
			}
			if (skill == P_MACE && !tech_known(T_ANTI_INERTIA)) {
			    	learntech(T_ANTI_INERTIA, FROMOUTSIDE, 1);
			    	You("learn how to perform anti inertia!");
			}
			if (skill == P_SYMBIOSIS && P_ADVANCE(P_IMPLANTS) == 0 && !tech_known(T_IMPLANTED_SYMBIOSIS)) {
			    	learntech(T_IMPLANTED_SYMBIOSIS, FROMOUTSIDE, 1);
			    	You("learn how to perform implanted symbiosis!");
			}
			if (skill == P_IMPLANTS && P_ADVANCE(P_SYMBIOSIS) == 0 && !tech_known(T_IMPLANTED_SYMBIOSIS)) {
			    	learntech(T_IMPLANTED_SYMBIOSIS, FROMOUTSIDE, 1);
			    	You("learn how to perform implanted symbiosis!");
			}
			if (skill == P_SYMBIOSIS && P_ADVANCE(P_POLYMORPHING) == 0 && !tech_known(T_ASSUME_SYMBIOTE)) {
			    	learntech(T_ASSUME_SYMBIOTE, FROMOUTSIDE, 1);
			    	You("learn how to perform assume symbiote!");
			}
			if (skill == P_POLYMORPHING && P_ADVANCE(P_SYMBIOSIS) == 0 && !tech_known(T_ASSUME_SYMBIOTE)) {
			    	learntech(T_ASSUME_SYMBIOTE, FROMOUTSIDE, 1);
			    	You("learn how to perform assume symbiote!");
			}
			if (skill == P_SYMBIOSIS && P_ADVANCE(P_PETKEEPING) == 0 && !tech_known(T_GENERATE_OFFSPRING)) {
			    	learntech(T_GENERATE_OFFSPRING, FROMOUTSIDE, 1);
			    	You("learn how to perform generate offspring!");
			}
			if (skill == P_PETKEEPING && P_ADVANCE(P_SYMBIOSIS) == 0 && !tech_known(T_GENERATE_OFFSPRING)) {
			    	learntech(T_GENERATE_OFFSPRING, FROMOUTSIDE, 1);
			    	You("learn how to perform generate offspring!");
			}
			if (skill == P_ORB && !tech_known(T_PACIFY)) {
			    	learntech(T_PACIFY, FROMOUTSIDE, 1);
			    	You("learn how to perform pacify!");
			}
			if (skill == P_ORB && !tech_known(T_AFTERBURNER)) {
			    	learntech(T_AFTERBURNER, FROMOUTSIDE, 1);
			    	You("learn how to perform afterburner!");
			}
			if (skill == P_ORB && !tech_known(T_BUGGARD)) {
			    	learntech(T_BUGGARD, FROMOUTSIDE, 1);
			    	You("learn how to perform buggard!");
			}
			if (skill == P_ORB && !tech_known(T_THUNDERSTORM)) {
			    	learntech(T_THUNDERSTORM, FROMOUTSIDE, 1);
			    	You("learn how to perform thunderstorm!");
			}
			if (skill == P_ORB && !tech_known(T_AUTOKILL)) {
			    	learntech(T_AUTOKILL, FROMOUTSIDE, 1);
			    	You("learn how to perform autokill!");
			}
			if (skill == P_ORB && !tech_known(T_CHAIN_THUNDERBOLT)) {
			    	learntech(T_CHAIN_THUNDERBOLT, FROMOUTSIDE, 1);
			    	You("learn how to perform chain thunderbolt!");
			}
			if (skill == P_ORB && !tech_known(T_FLASHING_MISCHIEF)) {
			    	learntech(T_FLASHING_MISCHIEF, FROMOUTSIDE, 1);
			    	You("learn how to perform flashing mischief!");
			}
			if (skill == P_CLAW && !tech_known(T_KAMEHAMEHA)) {
			    	learntech(T_KAMEHAMEHA, FROMOUTSIDE, 1);
			    	You("learn how to perform kamehameha!");
			}
			if (skill == P_CLAW && !tech_known(T_SHADOW_MANTLE)) {
			    	learntech(T_SHADOW_MANTLE, FROMOUTSIDE, 1);
			    	You("learn how to perform shadow mantle!");
			}
			if (skill == P_CLAW && !tech_known(T_VACUUM_STAR)) {
			    	learntech(T_VACUUM_STAR, FROMOUTSIDE, 1);
			    	You("learn how to perform vacuum star!");
			}
			if (skill == P_CLAW && !tech_known(T_BLADE_SHIELD)) {
			    	learntech(T_BLADE_SHIELD, FROMOUTSIDE, 1);
			    	You("learn how to perform blade shield!");
			}
			if (skill == P_CLAW && !tech_known(T_GREEN_WEAPON)) {
			    	learntech(T_GREEN_WEAPON, FROMOUTSIDE, 1);
			    	You("learn how to perform green weapon!");
			}
			if (skill == P_GRINDER && !tech_known(T_BALLSLIFF)) {
			    	learntech(T_BALLSLIFF, FROMOUTSIDE, 1);
			    	You("learn how to perform ballsliff!");
			}
			if (skill == P_GRINDER && !tech_known(T_POLE_MELEE)) {
			    	learntech(T_POLE_MELEE, FROMOUTSIDE, 1);
			    	You("learn how to perform pole melee!");
			}
			if (skill == P_GRINDER && !tech_known(T_CHOP_CHOP)) {
			    	learntech(T_CHOP_CHOP, FROMOUTSIDE, 1);
			    	You("learn how to perform chop chop!");
			}
			if (skill == P_GRINDER && !tech_known(T_BANISHMENT)) {
			    	learntech(T_BANISHMENT, FROMOUTSIDE, 1);
			    	You("learn how to perform banishment!");
			}
			if (skill == P_GRINDER && !tech_known(T_PARTICIPATION_LOSS)) {
			    	learntech(T_PARTICIPATION_LOSS, FROMOUTSIDE, 1);
			    	You("learn how to perform participation loss!");
			}
			if (skill == P_CLAW && P_ADVANCE(P_TWO_WEAPON_COMBAT) == 0 && !tech_known(T_WEAPON_BLOCKER)) {
			    	learntech(T_WEAPON_BLOCKER, FROMOUTSIDE, 1);
			    	You("learn how to perform weapon blocker!");
			}
			if (skill == P_TWO_WEAPON_COMBAT && P_ADVANCE(P_CLAW) == 0 && !tech_known(T_WEAPON_BLOCKER)) {
			    	learntech(T_WEAPON_BLOCKER, FROMOUTSIDE, 1);
			    	You("learn how to perform weapon blocker!");
			}
			if (skill == P_ORB && P_ADVANCE(P_MEMORIZATION) == 0 && !tech_known(T_EXTRA_MEMORY)) {
			    	learntech(T_EXTRA_MEMORY, FROMOUTSIDE, 1);
			    	You("learn how to perform extra memory!");
			}
			if (skill == P_MEMORIZATION && P_ADVANCE(P_ORB) == 0 >= P_EXPERT && !tech_known(T_EXTRA_MEMORY)) {
			    	learntech(T_EXTRA_MEMORY, FROMOUTSIDE, 1);
			    	You("learn how to perform extra memory!");
			}
			if (skill == P_GRINDER && P_ADVANCE(P_LANCE) == 0 && !tech_known(T_GRAP_SWAP)) {
			    	learntech(T_GRAP_SWAP, FROMOUTSIDE, 1);
			    	You("learn how to perform grap swap!");
			}
			if (skill == P_LANCE && P_ADVANCE(P_GRINDER) == 0 && !tech_known(T_GRAP_SWAP)) {
			    	learntech(T_GRAP_SWAP, FROMOUTSIDE, 1);
			    	You("learn how to perform grap swap!");
			}
			if (skill == P_GRINDER && P_ADVANCE(P_ORB) == 0 && P_ADVANCE(P_CLAW) == 0 && !tech_known(T_DIABOLIC_MINION)) {
			    	learntech(T_DIABOLIC_MINION, FROMOUTSIDE, 1);
			    	You("learn how to perform diabolic minion!");
			}
			if (skill == P_CLAW && P_ADVANCE(P_GRINDER) == 0 && P_ADVANCE(P_ORB) == 0 && !tech_known(T_DIABOLIC_MINION)) {
			    	learntech(T_DIABOLIC_MINION, FROMOUTSIDE, 1);
			    	You("learn how to perform diabolic minion!");
			}
			if (skill == P_ORB && P_ADVANCE(P_CLAW) == 0 && P_ADVANCE(P_GRINDER) == 0 && !tech_known(T_DIABOLIC_MINION)) {
			    	learntech(T_DIABOLIC_MINION, FROMOUTSIDE, 1);
			    	You("learn how to perform diabolic minion!");
			}

		}

		return;

	}

	advance_before = can_advance(skill, FALSE);
	if (Extra_wpn_practice) degree *= 2;
	if (StrongExtra_wpn_practice) degree *= 2;

	if (Role_if(PM_ANACHRONOUNBINDER) && u.acutraining > 0) degree *= 2;

	if (Race_if(PM_ELONA_SNAIL)) degree *= (1 + rnd(2)); /* snail trains skills 2.5 times as fast --Amy */

	if (Race_if(PM_PERVERT) && skill == P_SPIRITUALITY) degree *= 2;
	if (Race_if(PM_MAYMES) && (skill == P_FIREARM || skill == P_BOW || skill == P_CROSSBOW)) degree *= 2;
	if (Race_if(PM_AZTPOK) && skill == P_SPIRITUALITY) {
		if (P_ADVANCE(skill) >= 4320) degree *= 7;
		else if (P_ADVANCE(skill) >= 2500) degree *= 6;
		else if (P_ADVANCE(skill) >= 1280) degree *= 5;
		else if (P_ADVANCE(skill) >= 540) degree *= 4;
		else if (P_ADVANCE(skill) >= 160) degree *= 3;
		else if (P_ADVANCE(skill) >= 20) degree *= 2;
	}
	if (Race_if(PM_EGYMID) && skill == P_SPIRITUALITY) degree *= 2;
	if (Race_if(PM_ITAQUE) && !rn2(10)) degree *= 2;
	if (uwep && uwep->oartifact == ART_GEOLOGY_RULES_THE_WORLD && skill == P_PICK_AXE) degree *= 3;
	if (uarmc && uarmc->oartifact == ART_FINDING_THYSELF && skill == P_SPIRITUALITY) degree *= 5;
	if (uarmu && uarmu->oartifact == ART_GIVE_ME_STROKE__JO_ANNA && skill == P_HIGH_HEELS) degree *= 2;
	if (uarmh && uarmh->oartifact == ART_THERE_ARE_SEVERAL_OF_THEM && skill == P_SPIRITUALITY) degree *= 2;

	if (skill == 0) goto screwupsdone; /* just me covering my butt in case the game somehow thinks you had used
	* some skill that doesn't do anything and thinks it now has to set a blown timer --Amy */

	/* squeaking and gun control are hard to train for the wrong gender --Amy */
	if (((skill == P_SQUEAKING && !flags.female) || (skill == P_GUN_CONTROL && flags.female)) && !Role_if(PM_GENDERSTARIST)) {
		int advchance = 1;
		if (P_ADVANCE(skill) >= 4320) advchance = 21;
		else if (P_ADVANCE(skill) >= 2560) advchance = 13;
		else if (P_ADVANCE(skill) >= 1280) advchance = 8;
		else if (P_ADVANCE(skill) >= 540) advchance = 5;
		else if (P_ADVANCE(skill) >= 160) advchance = 3;
		else if (P_ADVANCE(skill) >= 20) advchance = 2;
		if (advchance > 1 && rn2(advchance)) return;
	}

	/* orb, claw and grinder are hard to train for the wrong alignment --Amy */
	if (((skill == P_ORB && u.ualign.type != A_LAWFUL) || (skill == P_CLAW && u.ualign.type != A_NEUTRAL) || (skill == P_GRINDER && u.ualign.type != A_CHAOTIC)) /* diablist goes here */ ) {
		int advchance = 1;
		if (P_ADVANCE(skill) >= 4320) advchance = 21;
		else if (P_ADVANCE(skill) >= 2560) advchance = 13;
		else if (P_ADVANCE(skill) >= 1280) advchance = 8;
		else if (P_ADVANCE(skill) >= 540) advchance = 5;
		else if (P_ADVANCE(skill) >= 160) advchance = 3;
		else if (P_ADVANCE(skill) >= 20) advchance = 2;
		if (advchance > 1 && rn2(advchance)) return;
	}

	if (Race_if(PM_CARTHAGE) && skill != P_LANCE && skill != P_RIDING) {
		int advchance = 1;
		if (P_ADVANCE(skill) >= 4320) advchance = 6;
		else if (P_ADVANCE(skill) >= 2560) advchance = 5;
		else if (P_ADVANCE(skill) >= 1280) advchance = 4;
		else if (P_ADVANCE(skill) >= 540) advchance = 3;
		else if (P_ADVANCE(skill) >= 160) advchance = 2;
		if (advchance > 1 && rn2(advchance)) return;
	}

	if (skill == u.untrainableskill) return; /* cannot train this skill at all, no matter what */
	if ((skill == u.halfspeedskill) && rn2(2)) return;
	if ((skill == u.fifthspeedskill) && rn2(5)) return;
	if ((skill == u.basiclimitskill) && P_ADVANCE(skill) >= 20 ) return;
	if ((skill == u.skilledlimitskill) && P_ADVANCE(skill) >= 160 ) return;
	if ((skill == u.expertlimitskill) && P_ADVANCE(skill) >= 540 ) return;
	if (skill == u.earlytrainingskill) {
		if (u.earlytrainingblown) return;
		if (moves < u.earlytrainingtimer) {
			u.earlytrainingblown = TRUE;
			return;
		}
	}
	if (skill == u.frtrainingskill) {
		if (u.frtrainingblown) return;
		if (moves < u.frtrainingtimer) {
			u.frtrainingblown = TRUE;
			return;
		}
	}

	if (skill == u.latetrainingskill) {
		if (moves > u.latetrainingtimer) {
			return;
		}
	}

	if (skill == u.lavtrainingskill) {
		if (moves < u.lavtrainingtimer) {
			return;
		}
	}

	if (skill == u.slowtrainingskill) { /* If this stacks with squeaking or gun control, that's a feature :P --Amy */
		int advchance = 1;
		if (P_ADVANCE(skill) >= 4320) advchance = 21;
		else if (P_ADVANCE(skill) >= 2560) advchance = 13;
		else if (P_ADVANCE(skill) >= 1280) advchance = 8;
		else if (P_ADVANCE(skill) >= 540) advchance = 5;
		else if (P_ADVANCE(skill) >= 160) advchance = 3;
		else if (P_ADVANCE(skill) >= 20) advchance = 2;
		if (advchance > 1 && rn2(advchance)) return;
	}

screwupsdone:

	if (Role_if(PM_GRENADONIN) && skill != P_FIREARM && rn2(3)) return;

	if (Race_if(PM_MAGYAR) && !rn2(2)) return;
	if (Race_if(PM_DUNADAN) && !rn2(2)) return;
	if (Race_if(PM_PLAYER_SKELETON) && rn2(3)) return;

	if (Race_if(PM_KORONST) && rn2(3) && (skill <= P_LAST_WEAPON && skill != P_SLING && skill != P_FIREARM)) return;

	if (!PlayerCannotTrainSkills || u.uprops[TRAINING_DEACTIVATED].extrinsic || have_trainingstone()) P_ADVANCE(skill) += degree;
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
	if (u.weapon_slots > 0) {
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
	    if (evilfriday) pline("This is the evil variant. Your skill point is lost forever.");
	    else u.weapon_slots = slots_required(skill) - 1;
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

/* lose the last skill you've advanced, regardless of the # of skill points you have left --Amy
 * IMPORTANT: This function expects the caller to take care of refunding the points you've lost! */
void
lose_last_spent_skill()
{
	boolean maybe_loose_disarm = FALSE;
	int skill;

	if (u.skills_advanced) {
		skill = u.skill_record[--u.skills_advanced];
		if (P_SKILL(skill) <= P_UNSKILLED)
			panic("lose_weapon_skill (%d) called by lose_last_spent_skill()", skill);
		if (skill <= P_LAST_WEAPON && skill != P_WHIP && P_SKILL(skill) == P_SKILLED)
			maybe_loose_disarm = TRUE;
		P_SKILL(skill)--;	/* drop skill one level */

		/* caller will refund the points */
	}

	if (maybe_loose_disarm && tech_known(T_DISARM)) {
		int i;
		for(i = u.skills_advanced - 1; i >= 0; i--) {
			skill = u.skill_record[i];
			if (skill <= P_LAST_WEAPON && skill != P_WHIP && P_SKILL(skill) >= P_SKILLED)
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
	    return ((martial_bonus() && !u.disablemartial) ? P_MARTIAL_ARTS :
				P_BARE_HANDED_COMBAT);
    if ( ((obj->oclass == BALL_CLASS) || (obj->oclass == CHAIN_CLASS)) /*&& Role_if(PM_CONVICT)*/)
        return objects[obj->otyp].oc_skill;
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

	if (PlayerCannotUseSkills) bonus -= 4;
	else switch (P_SKILL(type)) {
	    default: impossible(bad_skill, P_SKILL(type)); /* fall through */
	    case P_ISRESTRICTED:
	    case P_UNSKILLED:   bonus = -4; break;
	    case P_BASIC:       bonus =  rn2(2); break;
		    case P_SKILLED:     bonus =  1 + rn2(4); break;
		    case P_EXPERT:      bonus =  1 + rnd(6) ; break;
		    case P_MASTER:	bonus =  1 + rnd(8); break;
		    case P_GRAND_MASTER:	bonus =  1 + rnd(10); break;
		    case P_SUPREME_MASTER:	bonus =  1 + rnd(13); break;
	}
/* WAC -- No longer needed here...  */
#if 0
    } else if (type == P_TWO_WEAPON_COMBAT) {
	skill = P_SKILL(P_TWO_WEAPON_COMBAT);
	if (P_SKILL(wep_type) < skill) skill = P_SKILL(wep_type);
	if (PlayerCannotUseSkills) skill = P_ISRESTRICTED;
	switch (skill) {
	    default: impossible(bad_skill, skill); /* fall through */
	    case P_ISRESTRICTED:
	    case P_UNSKILLED:   bonus = -9; break;
	    case P_BASIC:	bonus = -7; break;
	    case P_SKILLED:	bonus = -5; break;
	    case P_EXPERT:	bonus = -3; break;
	    case P_MASTER:	bonus = -1; break;
	    case P_GRAND_MASTER:	bonus =  1; break;
	    case P_SUPREME_MASTER:	bonus =  3; break;
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
	if (!(PlayerCannotUseSkills)) bonus = P_SKILL(type);
	bonus = max(bonus,P_UNSKILLED) - 1;	/* unskilled => 0 */
	bonus = ((bonus + 2) * (martial_bonus() ? 2 : 1)) / 2;
	if (martial_bonus()) {
		bonus *= 3;
		bonus /= 2;
	}
    }

	/* KMH -- It's harder to hit while you are riding */
	if (u.usteed) {

		if (PlayerCannotUseSkills) bonus -= 2;
		else switch (P_SKILL(P_RIDING)) {
		    case P_ISRESTRICTED:
		    case P_UNSKILLED:   bonus -= 2; break;
		    case P_BASIC:       bonus -= 1; break;
		    case P_SKILLED:     break;
		    case P_EXPERT:      bonus += 2; break;
		    case P_MASTER:	bonus += 4; break;
		    case P_GRAND_MASTER:	bonus += 6; break;
		    case P_SUPREME_MASTER:	bonus += 9; break;
		}
		if (type == P_LANCE) bonus++;
	}

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

	if (PlayerCannotUseSkills) bonus -= 2;
	else switch (P_SKILL(type)) {
	    default: impossible("weapon_dam_bonus: bad skill %d",P_SKILL(type));
		     /* fall through */
	    case P_ISRESTRICTED:
	    case P_UNSKILLED:	bonus = -2; break;
	    case P_BASIC:	bonus =  0; break;
	    case P_SKILLED:	bonus =  1 + rn2(2); break;
	    case P_EXPERT:	bonus =  1 + rn2(4); break;
	    case P_MASTER:	bonus =  1 + rn2(6); break;
	    case P_GRAND_MASTER:bonus =  1 + rn2(10); break;
	    case P_SUPREME_MASTER:bonus =  1 + rn2(13); break;
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
	if (!(PlayerCannotUseSkills)) bonus = P_SKILL(type);
	bonus = max(bonus,P_UNSKILLED) - 1;	/* unskilled => 0 */
	bonus = ((bonus + 1) * (martial_bonus() ? 3 : 1)) / 2;

	/* CAVEAT: martial arts seems to use its own martial_dmg() function in uhitm.c and does not run this code!!!
	 * and speaking of which, bare-handed combat doesn't seem to do so either... */

	if (!(PlayerCannotUseSkills) && type == P_MARTIAL_ARTS) {

		switch (P_SKILL(P_MARTIAL_ARTS)) {
		    default:
		    case P_ISRESTRICTED:
		    case P_UNSKILLED:	bonus += 0; break;
		    case P_BASIC:	bonus += 0; break;
		    case P_SKILLED:	bonus += rnd(4); break;
		    case P_EXPERT:	bonus += rnd(8); break;
		    case P_MASTER:	bonus += rnd(12); break;
		    case P_GRAND_MASTER:	bonus += rnd(17); break;
		    case P_SUPREME_MASTER:	bonus += rnd(23); break;
		}

	} else if (!(PlayerCannotUseSkills) && type == P_BARE_HANDED_COMBAT) {

		switch (P_SKILL(P_BARE_HANDED_COMBAT)) {
		    default:
		    case P_ISRESTRICTED:
		    case P_UNSKILLED:	bonus += 0; break;
		    case P_BASIC:	bonus += 0; break;
		    case P_SKILLED:	bonus += rnd(3); break;
		    case P_EXPERT:	bonus += rnd(5); break;
		    case P_MASTER:	bonus += rnd(8); break;
		    case P_GRAND_MASTER:	bonus += rnd(12); break;
		    case P_SUPREME_MASTER:	bonus += rnd(16); break;
		}

	}

	if (Glib_combat && IsGlib && ((type == P_BARE_HANDED_COMBAT) || (type == P_MARTIAL_ARTS)) ) {

		bonus += rnd(u.ulevel);

	}

    } /* Misc skills aren't usually called by weapons */

	/* KMH -- Riding gives some thrusting damage */
	if (u.usteed && !(PlayerCannotUseSkills) ) {
		switch (P_SKILL(P_RIDING)) {
		    case P_ISRESTRICTED:
		    case P_UNSKILLED:   break;
		    case P_BASIC:       bonus += 1; break;
		    case P_SKILLED:     bonus += 3; break;
		    case P_EXPERT:      bonus += 5; break;
		    case P_MASTER:      bonus += 7; break;
		    case P_GRAND_MASTER:      bonus += 10; break;
		    case P_SUPREME_MASTER:      bonus += 15; break;
		}
	}

	if (Role_if(PM_AKLYST) && weapon && (weapon->otyp == AKLYS || weapon->otyp == BLOW_AKLYS) && !(PlayerCannotUseSkills) ) {
		switch (P_SKILL(P_CLUB)) {

			case P_BASIC:	bonus += rnd(2); break;
			case P_SKILLED:	bonus += rnd(4); break;
			case P_EXPERT:	bonus += rnd(6); break;
			case P_MASTER:	bonus += rnd(8); break;
			case P_GRAND_MASTER:	bonus += rnd(10); break;
			case P_SUPREME_MASTER:	bonus += rnd(12); break;
			default: bonus += 0; break;
		}
	}

	/* Jedi are simply better... but not as much as they used to --Amy */
	if (Role_if(PM_JEDI) && !rn2(2) && weapon && !(PlayerCannotUseSkills) && is_lightsaber(weapon)){
		switch (P_SKILL(type)){
			case P_SUPREME_MASTER: bonus += rnd(5); break; /* fall through removed by Amy */
			case P_GRAND_MASTER: bonus += rnd(4); break; /* fall through removed by Amy */
			case P_MASTER: bonus += rnd(3); break; /* fall through removed by Amy */
			case P_EXPERT: bonus += rnd(2); break; /* fall through removed by Amy */
			case P_SKILLED: bonus += 1; break;
			case P_BASIC: bonus += (rn2(2) ? 1 : 0); break;
			case P_UNSKILLED: break;
			case P_ISRESTRICTED: break;
			default: impossible("unknown lightsaber skill for a jedi"); break;
		}
	}
	else if (Race_if(PM_BORG) && !rn2(4) && weapon && !(PlayerCannotUseSkills) && is_lightsaber(weapon)){
		switch (P_SKILL(type)){
			case P_SUPREME_MASTER: bonus += rno(4); break; /* fall through removed by Amy */
			case P_GRAND_MASTER: bonus += rno(3); break; /* fall through removed by Amy */
			case P_MASTER: bonus += rno(2); break; /* fall through removed by Amy */
			case P_EXPERT: bonus += 1; break; /* fall through removed by Amy */
			case P_SKILLED: bonus += (rn2(2) ? 1 : 0); break;
			case P_BASIC: bonus += (!rn2(3) ? 1 : 0); break;
			case P_UNSKILLED: break;
			case P_ISRESTRICTED: break;
			default: impossible("unknown lightsaber skill for a borg"); break;
		}
	}

	/* Ogres are supposed to have a use for that weak starting club of theirs after all --Amy */
	if (Race_if(PM_OGRO) && weapon && weapon_type(weapon) == P_CLUB){

		bonus += 2;
		if (u.ulevel >= 15) bonus += 1;
		if (u.ulevel >= 30) bonus += 1;
	}

	/* quarterback is very good with baseball bats --Amy */
	if (Role_if(PM_QUARTERBACK) && weapon && weapon_type(weapon) == P_CLUB) bonus += 2;

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

	if (Race_if(PM_INKA) && weapon && objects[weapon->otyp].oc_material == MT_INKA) {
		bonus += 5;
	}

	if (Role_if(PM_BARD) && weapon && (weapon->otyp == GUITAR || weapon->otyp == PIANO)) bonus += 10;
	if (Role_if(PM_MUSICIAN) && weapon && (weapon->otyp == GUITAR || weapon->otyp == PIANO)) bonus += 5;

	if (uarmf && uarmf->oartifact == ART_NATALIA_S_PUNISHER && weapon && weapon_type(weapon) == P_HAMMER) bonus += 8;

	if (uarmc && weapon && weapon_type(weapon) == P_HAMMER && itemhasappearance(uarmc, APP_AVENGER_CLOAK) ) bonus += 5;

	if (RngeVengeance && weapon && weapon_type(weapon) == P_HAMMER) bonus += rnd(5);

	if (Role_if(PM_TRANSSYLVANIAN) && weapon && weapon_type(weapon) == P_HAMMER){

		bonus += rnd(2);
		if (rn2(2) && u.ulevel >= 15) bonus += 1;
		if (rn2(2) && u.ulevel >= 30) bonus += 1;
	}

	/* high heel skill improves whacking damage, but using them in melee does not train the skill --Amy */
	if (weapon && !(PlayerCannotUseSkills) && (weapon->otyp == WEDGED_LITTLE_GIRL_SANDAL || weapon->otyp == STURDY_PLATEAU_BOOT_FOR_GIRLS || weapon->otyp == BLOCK_HEELED_COMBAT_BOOT || weapon->otyp == WOODEN_GETA || weapon->otyp == LACQUERED_DANCING_SHOE || weapon->otyp == HIGH_HEELED_SANDAL || weapon->otyp == SEXY_LEATHER_PUMP || weapon->otyp == STICKSHOE || weapon->otyp == INKA_BOOT || weapon->otyp == STEEL_CAPPED_SANDAL || weapon->otyp == BLOCK_HEELED_SANDAL || weapon->otyp == PROSTITUTE_SHOE || weapon->otyp == DOGSHIT_BOOT) ) {

		switch (P_SKILL(P_HIGH_HEELS)) {
			case P_BASIC: bonus += 1; break;
			case P_SKILLED: bonus += 2; break;
			case P_EXPERT: bonus += 3; break;
			case P_MASTER: bonus += 5; break;
			case P_GRAND_MASTER: bonus += 7; break;
			case P_SUPREME_MASTER: bonus += 10; break;

		}

	}

	/* sexy flats skill too --Amy */
	if (weapon && !(PlayerCannotUseSkills) && (weapon->otyp == HUGGING_BOOT || weapon->otyp == SPIKED_BATTLE_BOOT || weapon->otyp == SOFT_LADY_SHOE || weapon->otyp == SOFT_GIRL_SNEAKER) ) {

		switch (P_SKILL(P_SEXY_FLATS)) {
			case P_BASIC: bonus += 1; break;
			case P_SKILLED: bonus += 2; break;
			case P_EXPERT: bonus += 3; break;
			case P_MASTER: bonus += 5; break;
			case P_GRAND_MASTER: bonus += 7; break;
			case P_SUPREME_MASTER: bonus += 10; break;

		}

	}

	if (RngeHighHeeling && weapon && (weapon->otyp == WEDGED_LITTLE_GIRL_SANDAL || weapon->otyp == STURDY_PLATEAU_BOOT_FOR_GIRLS || weapon->otyp == BLOCK_HEELED_COMBAT_BOOT || weapon->otyp == WOODEN_GETA || weapon->otyp == LACQUERED_DANCING_SHOE || weapon->otyp == HIGH_HEELED_SANDAL || weapon->otyp == STICKSHOE || weapon->otyp == SEXY_LEATHER_PUMP || weapon->otyp == INKA_BOOT || weapon->otyp == STEEL_CAPPED_SANDAL || weapon->otyp == BLOCK_HEELED_SANDAL || weapon->otyp == PROSTITUTE_SHOE || weapon->otyp == DOGSHIT_BOOT) ) bonus += rnd(10);

	if (uarmf && uarmf->oartifact == ART_VERENA_S_DUELING_SANDALS && weapon && (weapon->otyp == WEDGED_LITTLE_GIRL_SANDAL || weapon->otyp == STURDY_PLATEAU_BOOT_FOR_GIRLS || weapon->otyp == BLOCK_HEELED_COMBAT_BOOT || weapon->otyp == WOODEN_GETA || weapon->otyp == LACQUERED_DANCING_SHOE || weapon->otyp == HIGH_HEELED_SANDAL || weapon->otyp == STICKSHOE || weapon->otyp == SEXY_LEATHER_PUMP || weapon->otyp == INKA_BOOT || weapon->otyp == STEEL_CAPPED_SANDAL || weapon->otyp == BLOCK_HEELED_SANDAL || weapon->otyp == PROSTITUTE_SHOE || weapon->otyp == DOGSHIT_BOOT) ) bonus += rnd(10);

	if (uarmf && uarmf->oartifact == ART_HUGGING__GROPING_AND_STROK && weapon && (weapon->otyp == WEDGED_LITTLE_GIRL_SANDAL || weapon->otyp == STURDY_PLATEAU_BOOT_FOR_GIRLS || weapon->otyp == BLOCK_HEELED_COMBAT_BOOT || weapon->otyp == WOODEN_GETA || weapon->otyp == LACQUERED_DANCING_SHOE || weapon->otyp == HIGH_HEELED_SANDAL || weapon->otyp == STICKSHOE || weapon->otyp == SEXY_LEATHER_PUMP || weapon->otyp == INKA_BOOT || weapon->otyp == STEEL_CAPPED_SANDAL || weapon->otyp == BLOCK_HEELED_SANDAL || weapon->otyp == PROSTITUTE_SHOE || weapon->otyp == DOGSHIT_BOOT) ) bonus += 2;

	if (weapon && weapon->oartifact == ART_PUNISHMENT_FOR_YOU && !(PlayerCannotUseSkills) ) {

		switch (P_SKILL(P_FLAIL)) {
			case P_BASIC: bonus += 3; break;
			case P_SKILLED: bonus += 6; break;
			case P_EXPERT: bonus += 9; break;
			case P_MASTER: bonus += 12; break;
			case P_GRAND_MASTER: bonus += 15; break;
			case P_SUPREME_MASTER: bonus += 18; break;

		}

	}

	if (weapon && weapon->oartifact == ART_NEVER_WILL_THIS_BE_USEFUL && !(PlayerCannotUseSkills) ) {

		switch (P_SKILL(P_TRIDENT)) {
			case P_BASIC: bonus += 4; break;
			case P_SKILLED: bonus += 8; break;
			case P_EXPERT: bonus += 12; break;
			case P_MASTER: bonus += 16; break;
			case P_GRAND_MASTER: bonus += 20; break;
			case P_SUPREME_MASTER: bonus += 24; break;

		}

	}

	/* boomerang damage bonus for Batman */
	if (Race_if(PM_BATMAN) && weapon && weapon_type(weapon) == P_BOOMERANG){

		bonus += 2;
		if (u.ulevel >= 15) bonus += 2;
		if (u.ulevel >= 30) bonus += 2;
	}

	if (Race_if(PM_BATMAN) && weapon && weapon_type(weapon) == -P_BOOMERANG){

		bonus += 2;
		if (u.ulevel >= 15) bonus += 2;
		if (u.ulevel >= 30) bonus += 2;
	}

	/* more boomerang damage bonus */

	if (!(PlayerCannotUseSkills)) {

	if (weapon && weapon_type(weapon) == P_BOOMERANG && (P_SKILL(P_BOOMERANG) == P_SKILLED) ) bonus += rnd(3);
	if (weapon && weapon_type(weapon) == P_BOOMERANG && (P_SKILL(P_BOOMERANG) == P_EXPERT) ) bonus += rnd(6);
	if (weapon && weapon_type(weapon) == P_BOOMERANG && (P_SKILL(P_BOOMERANG) == P_MASTER) ) bonus += rnd(9);
	if (weapon && weapon_type(weapon) == P_BOOMERANG && (P_SKILL(P_BOOMERANG) == P_GRAND_MASTER) ) bonus += rnd(12);
	if (weapon && weapon_type(weapon) == P_BOOMERANG && (P_SKILL(P_BOOMERANG) == P_SUPREME_MASTER) ) bonus += rnd(15);

	if (weapon && weapon_type(weapon) == -P_BOOMERANG && (P_SKILL(P_BOOMERANG) == P_SKILLED) ) bonus += rnd(3);
	if (weapon && weapon_type(weapon) == -P_BOOMERANG && (P_SKILL(P_BOOMERANG) == P_EXPERT) ) bonus += rnd(6);
	if (weapon && weapon_type(weapon) == -P_BOOMERANG && (P_SKILL(P_BOOMERANG) == P_MASTER) ) bonus += rnd(9);
	if (weapon && weapon_type(weapon) == -P_BOOMERANG && (P_SKILL(P_BOOMERANG) == P_GRAND_MASTER) ) bonus += rnd(12);
	if (weapon && weapon_type(weapon) == -P_BOOMERANG && (P_SKILL(P_BOOMERANG) == P_SUPREME_MASTER) ) bonus += rnd(15);

	}

	if (!rn2(50) && u.menoraget && u.bookofthedeadget && u.silverbellget) bonus += rnd(5);
	if (!rn2(20) && u.chaoskeyget && u.neutralkeyget && u.lawfulkeyget) bonus += 1;
	if (!rn2(25) && u.medusaremoved) bonus += 1;
	if (!rn2(15) && u.luckstoneget && u.sokobanfinished && u.deepminefinished) bonus += 1;

	/* damage bonus for using racial equipment */

	if ((Race_if(PM_ELF) || Race_if(PM_PLAYER_MYRKALFR) || Role_if(PM_ELPH)) && weapon && (weapon->otyp == ELVEN_DAGGER || weapon->otyp == ELVEN_BOW || weapon->otyp == ELVEN_ARROW || weapon->otyp == ELVEN_SPEAR || weapon->otyp == ELVEN_SHORT_SWORD || weapon->otyp == HIGH_ELVEN_WARSWORD || weapon->otyp == ELVEN_BROADSWORD) ) bonus += 1;
	if ((Race_if(PM_DROW) || Role_if(PM_TWELPH)) && weapon && (weapon->otyp == DARK_ELVEN_DAGGER || weapon->otyp == DROVEN_CROSSBOW || weapon->otyp == DROVEN_BOLT || weapon->otyp == DROVEN_BOW || weapon->otyp == DROVEN_ARROW || weapon->otyp == DARK_ELVEN_ARROW || weapon->otyp == DARK_ELVEN_BOW || weapon->otyp == DARK_ELVEN_SHORT_SWORD) ) bonus += 1;
	if (Race_if(PM_ORC) && weapon && (weapon->otyp == ORCISH_DAGGER || weapon->otyp == ORCISH_BOW || weapon->otyp == ORCISH_ARROW || weapon->otyp == ORCISH_SPEAR || weapon->otyp == ORCISH_SHORT_SWORD ) ) bonus += 1;
	if ((Race_if(PM_DWARF) || Role_if(PM_MIDGET)) && weapon && (weapon->otyp == DWARVISH_SHORT_SWORD || weapon->otyp == DWARVISH_SPEAR) ) bonus += 1;
	if ((Race_if(PM_GNOME) || Role_if(PM_GOLDMINER)) && weapon && (weapon->otyp == AKLYS || weapon->otyp == BLOW_AKLYS || weapon->otyp == CROSSBOW || weapon->otyp == CROSSBOW_BOLT) ) bonus += 1;
	if ((Race_if(PM_HOBBIT) || Role_if(PM_RINGSEEKER)) && weapon && (weapon->otyp == SLING || weapon->otyp == INKA_SLING || weapon->otyp == SNIPESLING || weapon->otyp == METAL_SLING || weapon_type(weapon) == -P_SLING) ) bonus += 1;
	if ((Race_if(PM_HOBBIT) || Role_if(PM_RINGSEEKER)) && weapon && weapon->otyp == CATAPULT) bonus += 2;
	if (Race_if(PM_INKA) && weapon && (weapon->otyp == INKA_BLADE || weapon->otyp == INKUTLASS || weapon->otyp == NATURAL_STICK || weapon->otyp == VERMIN_SWATTER || weapon->otyp == INKA_BOOT || weapon->otyp == INKA_SPEAR || weapon->otyp == INKA_STINGER || weapon->otyp == INKA_SHACKLE )) bonus += 2;

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
    } else if (!(PlayerCannotUseSkills) && type <= P_LAST_WEAPON) {	/* bonus for highly skilled weapon, edit by Amy: also when dual-wielding */
	switch (P_SKILL(type)) {
	    case P_EXPERT:	bonus =  1; break;
	    case P_MASTER:	bonus =  2; break;
	    case P_GRAND_MASTER:bonus =  3; break;
	    case P_SUPREME_MASTER:bonus =  5; break;
	    default: bonus = 0; break;
	}
    }

	if (u.twoweap && !(PlayerCannotUseSkills)) {
	/* With shields being as strong as they are, and heavy two-handed weapons getting massive skill-dependant damage
	 * bonuses, we need to make sure that twoweaponing isn't completely useless. --Amy */
		switch (P_SKILL(P_TWO_WEAPON_COMBAT)) {

	    case P_SKILLED:	bonus +=  1; break;
	    case P_EXPERT:	bonus +=  rnd(2); break;
	    case P_MASTER:	bonus +=  rnd(3); break;
	    case P_GRAND_MASTER:bonus +=  rnd(4); break;
	    case P_SUPREME_MASTER:bonus +=  rnd(5); break;
	    default: bonus += 0; break;
		}
	}

	if (u.twoweap && !(PlayerCannotUseSkills) && tech_inuse(T_EDDY_WIND) ) {
		switch (P_SKILL(P_TWO_WEAPON_COMBAT)) {

	    case P_BASIC:		bonus +=  4; break;
	    case P_SKILLED:	bonus +=  6; break;
	    case P_EXPERT:	bonus +=  8; break;
	    case P_MASTER:	bonus +=  10; break;
	    case P_GRAND_MASTER:bonus +=  12; break;
	    case P_SUPREME_MASTER:bonus +=  14; break;
	    default: bonus += 2; break;
		}

	}

	if (!(PlayerCannotUseSkills)) {
		switch (P_SKILL(P_GENERAL_COMBAT)) {

	    case P_BASIC:		bonus +=  1; break;
	    case P_SKILLED:	bonus +=  rnd(2); break;
	    case P_EXPERT:	bonus +=  rnd(3); break;
	    case P_MASTER:	bonus +=  rnd(4); break;
	    case P_GRAND_MASTER:bonus +=  rnd(5); break;
	    case P_SUPREME_MASTER:bonus +=  rnd(6); break;
	    default: bonus += 0; break;
		}

	}

	if (weapon && is_lightsaber(weapon) && weapon->lamplit) {
		if (tech_inuse(T_ESCROBISM) && uarm && (uarm->otyp >= ROBE && uarm->otyp <= ROBE_OF_WEAKNESS)) {
			if (uarm && uarm->spe > 0) bonus += ((uarm->spe * 2) + rnd(5));
			else bonus += rnd(5);
		}

	}

	if (!(PlayerCannotUseSkills) && weapon && is_lightsaber(weapon) && weapon->lamplit) {
		switch (P_SKILL(P_SHII_CHO)) {

			case P_BASIC:	bonus +=  rn2(2); break;
			case P_SKILLED:	bonus +=  rn2(3); break;
			case P_EXPERT:	bonus +=  rn2(4); break;
			case P_MASTER:	bonus +=  rn2(5); break;
			case P_GRAND_MASTER:	bonus +=  rn2(6); break;
			case P_SUPREME_MASTER:	bonus +=  rn2(7); break;
			default: bonus += 0; break;
		}
	}

	if (!(PlayerCannotUseSkills) && weapon && is_lightsaber(weapon) && weapon->lamplit && !uarms && !u.twoweap) {
		switch (P_SKILL(P_MAKASHI)) {

			case P_BASIC:	bonus +=  rn2(3); break;
			case P_SKILLED:	bonus +=  rn2(5); break;
			case P_EXPERT:	bonus +=  rn2(7); break;
			case P_MASTER:	bonus +=  rn2(9); break;
			case P_GRAND_MASTER:	bonus +=  rn2(11); break;
			case P_SUPREME_MASTER:	bonus +=  rn2(13); break;
			default: bonus += 0; break;
		}
	}

	if (uwep && weapon && (uwep == weapon) && weapon->otyp == FLAME_MOUNTAIN && !uarms && !u.twoweap) {
		bonus += rnd(10);
	}

	if (!(PlayerCannotUseSkills) && uwep && is_lightsaber(uwep) && uwep->lamplit && u.twoweap && uswapwep && is_lightsaber(uswapwep) && uswapwep->lamplit) {
		switch (P_SKILL(P_ATARU)) {

			case P_BASIC:	bonus +=  rnd(2); break;
			case P_SKILLED:	bonus +=  rnd(4); break;
			case P_EXPERT:	bonus +=  rnd(6); break;
			case P_MASTER:	bonus +=  rnd(8); break;
			case P_GRAND_MASTER:	bonus +=  rnd(10); break;
			case P_SUPREME_MASTER:	bonus +=  rnd(12); break;
			default: bonus += 0; break;
		}
	}

	if (uarm && uarm->oartifact == ART_MAEDHROS_SARALONDE) bonus += 2;

	if (weapon && bimanual(weapon) && !is_missile(weapon) && !is_ammo(weapon) && !is_launcher(weapon) && !(is_pole(weapon) && !u.usteed) && !(PlayerCannotUseSkills)) {
		switch (P_SKILL(P_TWO_HANDED_WEAPON)) {

			case P_BASIC:	bonus +=  rn2(3); break;
			case P_SKILLED:	bonus +=  rn2(5); break;
			case P_EXPERT:	bonus +=  rn2(7); break;
			case P_MASTER:	bonus +=  rn2(9); break;
			case P_GRAND_MASTER:	bonus +=  rn2(11); break;
			case P_SUPREME_MASTER:	bonus +=  rn2(13); break;
			default: bonus += 0; break;
		}

	}

	if (!(PlayerCannotUseSkills) && weapon && is_lightsaber(weapon) && weapon->lamplit && bimanual(weapon) && weapon->altmode) {
		switch (P_SKILL(P_VAAPAD)) {

			case P_BASIC:	bonus +=  rn2(4); break;
			case P_SKILLED:	bonus +=  rn2(7); break;
			case P_EXPERT:	bonus +=  rn2(10); break;
			case P_MASTER:	bonus +=  rn2(13); break;
			case P_GRAND_MASTER:	bonus +=  rn2(16); break;
			case P_SUPREME_MASTER:	bonus +=  rn2(19); break;
			default: bonus += 0; break;
		}

	}


	if (uarmc && uarmc->oartifact == ART_DISBELIEVING_POWERLORD) bonus += rnd(5);

	if (!(PlayerCannotUseSkills)) {

	if (weapon && weapon_type(weapon) == P_DAGGER && (P_SKILL(P_DAGGER) == P_SKILLED) ) bonus += 1;
	if (weapon && weapon_type(weapon) == P_DAGGER && (P_SKILL(P_DAGGER) == P_EXPERT) ) bonus += rnd(2);
	if (weapon && weapon_type(weapon) == P_DAGGER && (P_SKILL(P_DAGGER) == P_MASTER) ) bonus += rnd(4);
	if (weapon && weapon_type(weapon) == P_DAGGER && (P_SKILL(P_DAGGER) == P_GRAND_MASTER) ) bonus += rnd(6);
	if (weapon && weapon_type(weapon) == P_DAGGER && (P_SKILL(P_DAGGER) == P_SUPREME_MASTER) ) bonus += rnd(8);

	if (weapon && weapon->otyp == GREAT_DAGGER && (P_SKILL(P_DAGGER) == P_SKILLED) ) bonus += 1;
	if (weapon && weapon->otyp == GREAT_DAGGER && (P_SKILL(P_DAGGER) == P_EXPERT) ) bonus += rnd(2);
	if (weapon && weapon->otyp == GREAT_DAGGER && (P_SKILL(P_DAGGER) == P_MASTER) ) bonus += rnd(3);
	if (weapon && weapon->otyp == GREAT_DAGGER && (P_SKILL(P_DAGGER) == P_GRAND_MASTER) ) bonus += rnd(4);
	if (weapon && weapon->otyp == GREAT_DAGGER && (P_SKILL(P_DAGGER) == P_SUPREME_MASTER) ) bonus += rnd(5);

	if (weapon && weapon->otyp == WOODEN_STAKE && (P_SKILL(P_DAGGER) == P_SKILLED) ) bonus += 1;
	if (weapon && weapon->otyp == WOODEN_STAKE && (P_SKILL(P_DAGGER) == P_EXPERT) ) bonus += rnd(2);
	if (weapon && weapon->otyp == WOODEN_STAKE && (P_SKILL(P_DAGGER) == P_MASTER) ) bonus += rnd(2);
	if (weapon && weapon->otyp == WOODEN_STAKE && (P_SKILL(P_DAGGER) == P_GRAND_MASTER) ) bonus += rnd(3);
	if (weapon && weapon->otyp == WOODEN_STAKE && (P_SKILL(P_DAGGER) == P_SUPREME_MASTER) ) bonus += rnd(4);

	if (weapon && weapon_type(weapon) == P_KNIFE && (P_SKILL(P_KNIFE) == P_SKILLED) ) bonus += 1;
	if (weapon && weapon_type(weapon) == P_KNIFE && (P_SKILL(P_KNIFE) == P_EXPERT) ) bonus += rnd(3);
	if (weapon && weapon_type(weapon) == P_KNIFE && (P_SKILL(P_KNIFE) == P_MASTER) ) bonus += rnd(5);
	if (weapon && weapon_type(weapon) == P_KNIFE && (P_SKILL(P_KNIFE) == P_GRAND_MASTER) ) bonus += rnd(7);
	if (weapon && weapon_type(weapon) == P_KNIFE && (P_SKILL(P_KNIFE) == P_SUPREME_MASTER) ) bonus += rnd(11);

	if (weapon && weapon->otyp == BATTLE_AXE && (P_SKILL(P_AXE) == P_SKILLED) ) bonus += rnd(6);
	if (weapon && weapon->otyp == BATTLE_AXE && (P_SKILL(P_AXE) == P_EXPERT) ) bonus += rnd(12);
	if (weapon && weapon->otyp == BATTLE_AXE && (P_SKILL(P_AXE) == P_MASTER) ) bonus += rnd(18);
	if (weapon && weapon->otyp == BATTLE_AXE && (P_SKILL(P_AXE) == P_GRAND_MASTER) ) bonus += rnd(24);
	if (weapon && weapon->otyp == BATTLE_AXE && (P_SKILL(P_AXE) == P_SUPREME_MASTER) ) bonus += rnd(30);

	if (weapon && weapon->otyp == DWARVISH_BATTLE_AXE && (P_SKILL(P_AXE) == P_SKILLED) ) bonus += rnd(6);
	if (weapon && weapon->otyp == DWARVISH_BATTLE_AXE && (P_SKILL(P_AXE) == P_EXPERT) ) bonus += rnd(12);
	if (weapon && weapon->otyp == DWARVISH_BATTLE_AXE && (P_SKILL(P_AXE) == P_MASTER) ) bonus += rnd(18);
	if (weapon && weapon->otyp == DWARVISH_BATTLE_AXE && (P_SKILL(P_AXE) == P_GRAND_MASTER) ) bonus += rnd(24);
	if (weapon && weapon->otyp == DWARVISH_BATTLE_AXE && (P_SKILL(P_AXE) == P_SUPREME_MASTER) ) bonus += rnd(30);

	if (weapon && weapon->otyp == CHEMISTRY_SPACE_AXE && (P_SKILL(P_AXE) == P_SKILLED) ) bonus += rnd(6);
	if (weapon && weapon->otyp == CHEMISTRY_SPACE_AXE && (P_SKILL(P_AXE) == P_EXPERT) ) bonus += rnd(12);
	if (weapon && weapon->otyp == CHEMISTRY_SPACE_AXE && (P_SKILL(P_AXE) == P_MASTER) ) bonus += rnd(18);
	if (weapon && weapon->otyp == CHEMISTRY_SPACE_AXE && (P_SKILL(P_AXE) == P_GRAND_MASTER) ) bonus += rnd(24);
	if (weapon && weapon->otyp == CHEMISTRY_SPACE_AXE && (P_SKILL(P_AXE) == P_SUPREME_MASTER) ) bonus += rnd(30);

	if (weapon && weapon->otyp == DWARVISH_MATTOCK && (P_SKILL(P_PICK_AXE) == P_SKILLED) ) bonus += rnd(6);
	if (weapon && weapon->otyp == DWARVISH_MATTOCK && (P_SKILL(P_PICK_AXE) == P_EXPERT) ) bonus += rnd(11);
	if (weapon && weapon->otyp == DWARVISH_MATTOCK && (P_SKILL(P_PICK_AXE) == P_MASTER) ) bonus += rnd(16);
	if (weapon && weapon->otyp == DWARVISH_MATTOCK && (P_SKILL(P_PICK_AXE) == P_GRAND_MASTER) ) bonus += rnd(21);
	if (weapon && weapon->otyp == DWARVISH_MATTOCK && (P_SKILL(P_PICK_AXE) == P_SUPREME_MASTER) ) bonus += rnd(26);

	if (weapon && weapon->otyp == SOFT_MATTOCK && (P_SKILL(P_PICK_AXE) == P_SKILLED) ) bonus += rnd(6);
	if (weapon && weapon->otyp == SOFT_MATTOCK && (P_SKILL(P_PICK_AXE) == P_EXPERT) ) bonus += rnd(11);
	if (weapon && weapon->otyp == SOFT_MATTOCK && (P_SKILL(P_PICK_AXE) == P_MASTER) ) bonus += rnd(16);
	if (weapon && weapon->otyp == SOFT_MATTOCK && (P_SKILL(P_PICK_AXE) == P_GRAND_MASTER) ) bonus += rnd(21);
	if (weapon && weapon->otyp == SOFT_MATTOCK && (P_SKILL(P_PICK_AXE) == P_SUPREME_MASTER) ) bonus += rnd(26);

	if (weapon && weapon->otyp == ETERNIUM_MATTOCK && (P_SKILL(P_PICK_AXE) == P_SKILLED) ) bonus += rnd(6);
	if (weapon && weapon->otyp == ETERNIUM_MATTOCK && (P_SKILL(P_PICK_AXE) == P_EXPERT) ) bonus += rnd(11);
	if (weapon && weapon->otyp == ETERNIUM_MATTOCK && (P_SKILL(P_PICK_AXE) == P_MASTER) ) bonus += rnd(16);
	if (weapon && weapon->otyp == ETERNIUM_MATTOCK && (P_SKILL(P_PICK_AXE) == P_GRAND_MASTER) ) bonus += rnd(21);
	if (weapon && weapon->otyp == ETERNIUM_MATTOCK && (P_SKILL(P_PICK_AXE) == P_SUPREME_MASTER) ) bonus += rnd(26);

	if (weapon && weapon->otyp == BROOM && (P_SKILL(P_PADDLE) == P_SKILLED) ) bonus += rnd(6);
	if (weapon && weapon->otyp == BROOM && (P_SKILL(P_PADDLE) == P_EXPERT) ) bonus += rnd(11);
	if (weapon && weapon->otyp == BROOM && (P_SKILL(P_PADDLE) == P_MASTER) ) bonus += rnd(16);
	if (weapon && weapon->otyp == BROOM && (P_SKILL(P_PADDLE) == P_GRAND_MASTER) ) bonus += rnd(21);
	if (weapon && weapon->otyp == BROOM && (P_SKILL(P_PADDLE) == P_SUPREME_MASTER) ) bonus += rnd(26);

	if (weapon && weapon->otyp == MOP && (P_SKILL(P_PADDLE) == P_SKILLED) ) bonus += rnd(6);
	if (weapon && weapon->otyp == MOP && (P_SKILL(P_PADDLE) == P_EXPERT) ) bonus += rnd(11);
	if (weapon && weapon->otyp == MOP && (P_SKILL(P_PADDLE) == P_MASTER) ) bonus += rnd(16);
	if (weapon && weapon->otyp == MOP && (P_SKILL(P_PADDLE) == P_GRAND_MASTER) ) bonus += rnd(21);
	if (weapon && weapon->otyp == MOP && (P_SKILL(P_PADDLE) == P_SUPREME_MASTER) ) bonus += rnd(26);

	if (weapon && weapon->otyp == SPECIAL_MOP && (P_SKILL(P_PADDLE) == P_SKILLED) ) bonus += rnd(6);
	if (weapon && weapon->otyp == SPECIAL_MOP && (P_SKILL(P_PADDLE) == P_EXPERT) ) bonus += rnd(11);
	if (weapon && weapon->otyp == SPECIAL_MOP && (P_SKILL(P_PADDLE) == P_MASTER) ) bonus += rnd(16);
	if (weapon && weapon->otyp == SPECIAL_MOP && (P_SKILL(P_PADDLE) == P_GRAND_MASTER) ) bonus += rnd(21);
	if (weapon && weapon->otyp == SPECIAL_MOP && (P_SKILL(P_PADDLE) == P_SUPREME_MASTER) ) bonus += rnd(26);

	if (weapon && weapon->otyp == BOAT_OAR && (P_SKILL(P_PADDLE) == P_SKILLED) ) bonus += rnd(6);
	if (weapon && weapon->otyp == BOAT_OAR && (P_SKILL(P_PADDLE) == P_EXPERT) ) bonus += rnd(11);
	if (weapon && weapon->otyp == BOAT_OAR && (P_SKILL(P_PADDLE) == P_MASTER) ) bonus += rnd(16);
	if (weapon && weapon->otyp == BOAT_OAR && (P_SKILL(P_PADDLE) == P_GRAND_MASTER) ) bonus += rnd(21);
	if (weapon && weapon->otyp == BOAT_OAR && (P_SKILL(P_PADDLE) == P_SUPREME_MASTER) ) bonus += rnd(26);

	if (weapon && weapon->otyp == MAGICAL_PAINTBRUSH && (P_SKILL(P_PADDLE) == P_SKILLED) ) bonus += rnd(6);
	if (weapon && weapon->otyp == MAGICAL_PAINTBRUSH && (P_SKILL(P_PADDLE) == P_EXPERT) ) bonus += rnd(11);
	if (weapon && weapon->otyp == MAGICAL_PAINTBRUSH && (P_SKILL(P_PADDLE) == P_MASTER) ) bonus += rnd(16);
	if (weapon && weapon->otyp == MAGICAL_PAINTBRUSH && (P_SKILL(P_PADDLE) == P_GRAND_MASTER) ) bonus += rnd(21);
	if (weapon && weapon->otyp == MAGICAL_PAINTBRUSH && (P_SKILL(P_PADDLE) == P_SUPREME_MASTER) ) bonus += rnd(26);

	if (weapon && weapon->otyp == PICK_AXE && (P_SKILL(P_PICK_AXE) == P_SKILLED) ) bonus += rnd(4);
	if (weapon && weapon->otyp == PICK_AXE && (P_SKILL(P_PICK_AXE) == P_EXPERT) ) bonus += rnd(8);
	if (weapon && weapon->otyp == PICK_AXE && (P_SKILL(P_PICK_AXE) == P_MASTER) ) bonus += rnd(12);
	if (weapon && weapon->otyp == PICK_AXE && (P_SKILL(P_PICK_AXE) == P_GRAND_MASTER) ) bonus += rnd(16);
	if (weapon && weapon->otyp == PICK_AXE && (P_SKILL(P_PICK_AXE) == P_SUPREME_MASTER) ) bonus += rnd(20);

	if (weapon && weapon->otyp == CONGLOMERATE_PICK && (P_SKILL(P_PICK_AXE) == P_SKILLED) ) bonus += rnd(4);
	if (weapon && weapon->otyp == CONGLOMERATE_PICK && (P_SKILL(P_PICK_AXE) == P_EXPERT) ) bonus += rnd(8);
	if (weapon && weapon->otyp == CONGLOMERATE_PICK && (P_SKILL(P_PICK_AXE) == P_MASTER) ) bonus += rnd(12);
	if (weapon && weapon->otyp == CONGLOMERATE_PICK && (P_SKILL(P_PICK_AXE) == P_GRAND_MASTER) ) bonus += rnd(16);
	if (weapon && weapon->otyp == CONGLOMERATE_PICK && (P_SKILL(P_PICK_AXE) == P_SUPREME_MASTER) ) bonus += rnd(20);

	if (weapon && weapon->otyp == CONUNDRUM_PICK && (P_SKILL(P_PICK_AXE) == P_SKILLED) ) bonus += rnd(4);
	if (weapon && weapon->otyp == CONUNDRUM_PICK && (P_SKILL(P_PICK_AXE) == P_EXPERT) ) bonus += rnd(8);
	if (weapon && weapon->otyp == CONUNDRUM_PICK && (P_SKILL(P_PICK_AXE) == P_MASTER) ) bonus += rnd(12);
	if (weapon && weapon->otyp == CONUNDRUM_PICK && (P_SKILL(P_PICK_AXE) == P_GRAND_MASTER) ) bonus += rnd(16);
	if (weapon && weapon->otyp == CONUNDRUM_PICK && (P_SKILL(P_PICK_AXE) == P_SUPREME_MASTER) ) bonus += rnd(20);

	if (weapon && weapon->otyp == MYSTERY_PICK && (P_SKILL(P_PICK_AXE) == P_SKILLED) ) bonus += rnd(4);
	if (weapon && weapon->otyp == MYSTERY_PICK && (P_SKILL(P_PICK_AXE) == P_EXPERT) ) bonus += rnd(8);
	if (weapon && weapon->otyp == MYSTERY_PICK && (P_SKILL(P_PICK_AXE) == P_MASTER) ) bonus += rnd(12);
	if (weapon && weapon->otyp == MYSTERY_PICK && (P_SKILL(P_PICK_AXE) == P_GRAND_MASTER) ) bonus += rnd(16);
	if (weapon && weapon->otyp == MYSTERY_PICK && (P_SKILL(P_PICK_AXE) == P_SUPREME_MASTER) ) bonus += rnd(20);

	if (weapon && weapon->otyp == BRONZE_PICK && (P_SKILL(P_PICK_AXE) == P_SKILLED) ) bonus += rnd(4);
	if (weapon && weapon->otyp == BRONZE_PICK && (P_SKILL(P_PICK_AXE) == P_EXPERT) ) bonus += rnd(8);
	if (weapon && weapon->otyp == BRONZE_PICK && (P_SKILL(P_PICK_AXE) == P_MASTER) ) bonus += rnd(12);
	if (weapon && weapon->otyp == BRONZE_PICK && (P_SKILL(P_PICK_AXE) == P_GRAND_MASTER) ) bonus += rnd(16);
	if (weapon && weapon->otyp == BRONZE_PICK && (P_SKILL(P_PICK_AXE) == P_SUPREME_MASTER) ) bonus += rnd(20);

	if (weapon && weapon->otyp == BRICK_PICK && (P_SKILL(P_PICK_AXE) == P_SKILLED) ) bonus += rnd(4);
	if (weapon && weapon->otyp == BRICK_PICK && (P_SKILL(P_PICK_AXE) == P_EXPERT) ) bonus += rnd(8);
	if (weapon && weapon->otyp == BRICK_PICK && (P_SKILL(P_PICK_AXE) == P_MASTER) ) bonus += rnd(12);
	if (weapon && weapon->otyp == BRICK_PICK && (P_SKILL(P_PICK_AXE) == P_GRAND_MASTER) ) bonus += rnd(16);
	if (weapon && weapon->otyp == BRICK_PICK && (P_SKILL(P_PICK_AXE) == P_SUPREME_MASTER) ) bonus += rnd(20);

	if (weapon && weapon->otyp == NANO_PICK && (P_SKILL(P_PICK_AXE) == P_SKILLED) ) bonus += rnd(4);
	if (weapon && weapon->otyp == NANO_PICK && (P_SKILL(P_PICK_AXE) == P_EXPERT) ) bonus += rnd(8);
	if (weapon && weapon->otyp == NANO_PICK && (P_SKILL(P_PICK_AXE) == P_MASTER) ) bonus += rnd(12);
	if (weapon && weapon->otyp == NANO_PICK && (P_SKILL(P_PICK_AXE) == P_GRAND_MASTER) ) bonus += rnd(16);
	if (weapon && weapon->otyp == NANO_PICK && (P_SKILL(P_PICK_AXE) == P_SUPREME_MASTER) ) bonus += rnd(20);

	if (weapon && weapon->otyp == TSURUGI && (P_SKILL(P_TWO_HANDED_SWORD) == P_SKILLED) ) bonus += rnd(6);
	if (weapon && weapon->otyp == TSURUGI && (P_SKILL(P_TWO_HANDED_SWORD) == P_EXPERT) ) bonus += rnd(12);
	if (weapon && weapon->otyp == TSURUGI && (P_SKILL(P_TWO_HANDED_SWORD) == P_MASTER) ) bonus += rnd(18);
	if (weapon && weapon->otyp == TSURUGI && (P_SKILL(P_TWO_HANDED_SWORD) == P_GRAND_MASTER) ) bonus += rnd(24);
	if (weapon && weapon->otyp == TSURUGI && (P_SKILL(P_TWO_HANDED_SWORD) == P_SUPREME_MASTER) ) bonus += rnd(30);

	if (weapon && weapon->otyp == DROVEN_GREATSWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_SKILLED) ) bonus += rnd(6);
	if (weapon && weapon->otyp == DROVEN_GREATSWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_EXPERT) ) bonus += rnd(12);
	if (weapon && weapon->otyp == DROVEN_GREATSWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_MASTER) ) bonus += rnd(18);
	if (weapon && weapon->otyp == DROVEN_GREATSWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_GRAND_MASTER) ) bonus += rnd(24);
	if (weapon && weapon->otyp == DROVEN_GREATSWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_SUPREME_MASTER) ) bonus += rnd(30);

	if (weapon && weapon->otyp == CHAINSWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_SKILLED) ) bonus += rnd(6);
	if (weapon && weapon->otyp == CHAINSWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_EXPERT) ) bonus += rnd(12);
	if (weapon && weapon->otyp == CHAINSWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_MASTER) ) bonus += rnd(18);
	if (weapon && weapon->otyp == CHAINSWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_GRAND_MASTER) ) bonus += rnd(24);
	if (weapon && weapon->otyp == CHAINSWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_SUPREME_MASTER) ) bonus += rnd(30);

	if (weapon && weapon->otyp == BIDENHANDER && (P_SKILL(P_TWO_HANDED_SWORD) == P_SKILLED) ) bonus += rnd(6);
	if (weapon && weapon->otyp == BIDENHANDER && (P_SKILL(P_TWO_HANDED_SWORD) == P_EXPERT) ) bonus += rnd(12);
	if (weapon && weapon->otyp == BIDENHANDER && (P_SKILL(P_TWO_HANDED_SWORD) == P_MASTER) ) bonus += rnd(18);
	if (weapon && weapon->otyp == BIDENHANDER && (P_SKILL(P_TWO_HANDED_SWORD) == P_GRAND_MASTER) ) bonus += rnd(24);
	if (weapon && weapon->otyp == BIDENHANDER && (P_SKILL(P_TWO_HANDED_SWORD) == P_SUPREME_MASTER) ) bonus += rnd(30);

	if (weapon && weapon->otyp == ETHER_SAW && (P_SKILL(P_TWO_HANDED_SWORD) == P_SKILLED) ) bonus += rnd(6);
	if (weapon && weapon->otyp == ETHER_SAW && (P_SKILL(P_TWO_HANDED_SWORD) == P_EXPERT) ) bonus += rnd(12);
	if (weapon && weapon->otyp == ETHER_SAW && (P_SKILL(P_TWO_HANDED_SWORD) == P_MASTER) ) bonus += rnd(18);
	if (weapon && weapon->otyp == ETHER_SAW && (P_SKILL(P_TWO_HANDED_SWORD) == P_GRAND_MASTER) ) bonus += rnd(24);
	if (weapon && weapon->otyp == ETHER_SAW && (P_SKILL(P_TWO_HANDED_SWORD) == P_SUPREME_MASTER) ) bonus += rnd(30);

	if (weapon && weapon->otyp == SHADOWBLADE && (P_SKILL(P_TWO_HANDED_SWORD) == P_SKILLED) ) bonus += rnd(6);
	if (weapon && weapon->otyp == SHADOWBLADE && (P_SKILL(P_TWO_HANDED_SWORD) == P_EXPERT) ) bonus += rnd(12);
	if (weapon && weapon->otyp == SHADOWBLADE && (P_SKILL(P_TWO_HANDED_SWORD) == P_MASTER) ) bonus += rnd(18);
	if (weapon && weapon->otyp == SHADOWBLADE && (P_SKILL(P_TWO_HANDED_SWORD) == P_GRAND_MASTER) ) bonus += rnd(24);
	if (weapon && weapon->otyp == SHADOWBLADE && (P_SKILL(P_TWO_HANDED_SWORD) == P_SUPREME_MASTER) ) bonus += rnd(30);

	if (weapon && weapon->otyp == ROMAN_SWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_SKILLED) ) bonus += rnd(6);
	if (weapon && weapon->otyp == ROMAN_SWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_EXPERT) ) bonus += rnd(12);
	if (weapon && weapon->otyp == ROMAN_SWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_MASTER) ) bonus += rnd(18);
	if (weapon && weapon->otyp == ROMAN_SWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_GRAND_MASTER) ) bonus += rnd(24);
	if (weapon && weapon->otyp == ROMAN_SWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_SUPREME_MASTER) ) bonus += rnd(30);

	if (weapon && weapon->otyp == ORGANOBLADE && (P_SKILL(P_TWO_HANDED_SWORD) == P_SKILLED) ) bonus += rnd(6);
	if (weapon && weapon->otyp == ORGANOBLADE && (P_SKILL(P_TWO_HANDED_SWORD) == P_EXPERT) ) bonus += rnd(12);
	if (weapon && weapon->otyp == ORGANOBLADE && (P_SKILL(P_TWO_HANDED_SWORD) == P_MASTER) ) bonus += rnd(18);
	if (weapon && weapon->otyp == ORGANOBLADE && (P_SKILL(P_TWO_HANDED_SWORD) == P_GRAND_MASTER) ) bonus += rnd(24);
	if (weapon && weapon->otyp == ORGANOBLADE && (P_SKILL(P_TWO_HANDED_SWORD) == P_SUPREME_MASTER) ) bonus += rnd(30);

	if (weapon && weapon->otyp == COLOSSUS_BLADE && (P_SKILL(P_TWO_HANDED_SWORD) == P_SKILLED) ) bonus += rnd(12);
	if (weapon && weapon->otyp == COLOSSUS_BLADE && (P_SKILL(P_TWO_HANDED_SWORD) == P_EXPERT) ) bonus += rnd(24);
	if (weapon && weapon->otyp == COLOSSUS_BLADE && (P_SKILL(P_TWO_HANDED_SWORD) == P_MASTER) ) bonus += rnd(36);
	if (weapon && weapon->otyp == COLOSSUS_BLADE && (P_SKILL(P_TWO_HANDED_SWORD) == P_GRAND_MASTER) ) bonus += rnd(48);
	if (weapon && weapon->otyp == COLOSSUS_BLADE && (P_SKILL(P_TWO_HANDED_SWORD) == P_SUPREME_MASTER) ) bonus += rnd(60);

	if (weapon && weapon->otyp == BASTERD_SWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_SKILLED) ) bonus += rnd(6);
	if (weapon && weapon->otyp == BASTERD_SWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_EXPERT) ) bonus += rnd(12);
	if (weapon && weapon->otyp == BASTERD_SWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_MASTER) ) bonus += rnd(18);
	if (weapon && weapon->otyp == BASTERD_SWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_GRAND_MASTER) ) bonus += rnd(24);
	if (weapon && weapon->otyp == BASTERD_SWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_SUPREME_MASTER) ) bonus += rnd(30);

	if (weapon && weapon->otyp == TWO_HANDED_SWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_SKILLED) ) bonus += rnd(6);
	if (weapon && weapon->otyp == TWO_HANDED_SWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_EXPERT) ) bonus += rnd(12);
	if (weapon && weapon->otyp == TWO_HANDED_SWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_MASTER) ) bonus += rnd(18);
	if (weapon && weapon->otyp == TWO_HANDED_SWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_GRAND_MASTER) ) bonus += rnd(24);
	if (weapon && weapon->otyp == TWO_HANDED_SWORD && (P_SKILL(P_TWO_HANDED_SWORD) == P_SUPREME_MASTER) ) bonus += rnd(30);

	if (weapon && weapon->otyp == GOLDEN_SABER && (P_SKILL(P_SABER) == P_SKILLED) ) bonus += rnd(2);
	if (weapon && weapon->otyp == GOLDEN_SABER && (P_SKILL(P_SABER) == P_EXPERT) ) bonus += rnd(4);
	if (weapon && weapon->otyp == GOLDEN_SABER && (P_SKILL(P_SABER) == P_MASTER) ) bonus += rnd(7);
	if (weapon && weapon->otyp == GOLDEN_SABER && (P_SKILL(P_SABER) == P_GRAND_MASTER) ) bonus += rnd(10);
	if (weapon && weapon->otyp == GOLDEN_SABER && (P_SKILL(P_SABER) == P_SUPREME_MASTER) ) bonus += rnd(14);

	if (weapon && weapon->otyp == ETERNIUM_SABER && (P_SKILL(P_SABER) == P_SKILLED) ) bonus += rnd(2);
	if (weapon && weapon->otyp == ETERNIUM_SABER && (P_SKILL(P_SABER) == P_EXPERT) ) bonus += rnd(4);
	if (weapon && weapon->otyp == ETERNIUM_SABER && (P_SKILL(P_SABER) == P_MASTER) ) bonus += rnd(7);
	if (weapon && weapon->otyp == ETERNIUM_SABER && (P_SKILL(P_SABER) == P_GRAND_MASTER) ) bonus += rnd(10);
	if (weapon && weapon->otyp == ETERNIUM_SABER && (P_SKILL(P_SABER) == P_SUPREME_MASTER) ) bonus += rnd(14);

	if (weapon && weapon->otyp == PLATINUM_SABER && (P_SKILL(P_SABER) == P_SKILLED) ) bonus += rnd(2);
	if (weapon && weapon->otyp == PLATINUM_SABER && (P_SKILL(P_SABER) == P_EXPERT) ) bonus += rnd(4);
	if (weapon && weapon->otyp == PLATINUM_SABER && (P_SKILL(P_SABER) == P_MASTER) ) bonus += rnd(7);
	if (weapon && weapon->otyp == PLATINUM_SABER && (P_SKILL(P_SABER) == P_GRAND_MASTER) ) bonus += rnd(10);
	if (weapon && weapon->otyp == PLATINUM_SABER && (P_SKILL(P_SABER) == P_SUPREME_MASTER) ) bonus += rnd(14);

	if (weapon && weapon->otyp == METAL_CLUB && (P_SKILL(P_CLUB) == P_SKILLED) ) bonus += rnd(2);
	if (weapon && weapon->otyp == METAL_CLUB && (P_SKILL(P_CLUB) == P_EXPERT) ) bonus += rnd(4);
	if (weapon && weapon->otyp == METAL_CLUB && (P_SKILL(P_CLUB) == P_MASTER) ) bonus += rnd(7);
	if (weapon && weapon->otyp == METAL_CLUB && (P_SKILL(P_CLUB) == P_GRAND_MASTER) ) bonus += rnd(10);
	if (weapon && weapon->otyp == METAL_CLUB && (P_SKILL(P_CLUB) == P_SUPREME_MASTER) ) bonus += rnd(14);

	if (weapon && weapon->otyp == DIAMOND_SMASHER && (P_SKILL(P_CLUB) == P_SKILLED) ) bonus += rnd(2);
	if (weapon && weapon->otyp == DIAMOND_SMASHER && (P_SKILL(P_CLUB) == P_EXPERT) ) bonus += rnd(4);
	if (weapon && weapon->otyp == DIAMOND_SMASHER && (P_SKILL(P_CLUB) == P_MASTER) ) bonus += rnd(7);
	if (weapon && weapon->otyp == DIAMOND_SMASHER && (P_SKILL(P_CLUB) == P_GRAND_MASTER) ) bonus += rnd(10);
	if (weapon && weapon->otyp == DIAMOND_SMASHER && (P_SKILL(P_CLUB) == P_SUPREME_MASTER) ) bonus += rnd(14);

	if (weapon && weapon->otyp == LOG && (P_SKILL(P_CLUB) == P_SKILLED) ) bonus += rnd(6);
	if (weapon && weapon->otyp == LOG && (P_SKILL(P_CLUB) == P_EXPERT) ) bonus += rnd(12);
	if (weapon && weapon->otyp == LOG && (P_SKILL(P_CLUB) == P_MASTER) ) bonus += rnd(18);
	if (weapon && weapon->otyp == LOG && (P_SKILL(P_CLUB) == P_GRAND_MASTER) ) bonus += rnd(24);
	if (weapon && weapon->otyp == LOG && (P_SKILL(P_CLUB) == P_SUPREME_MASTER) ) bonus += rnd(30);

	if (weapon && weapon->otyp == HUGE_CLUB && (P_SKILL(P_CLUB) == P_SKILLED) ) bonus += rnd(6);
	if (weapon && weapon->otyp == HUGE_CLUB && (P_SKILL(P_CLUB) == P_EXPERT) ) bonus += rnd(12);
	if (weapon && weapon->otyp == HUGE_CLUB && (P_SKILL(P_CLUB) == P_MASTER) ) bonus += rnd(18);
	if (weapon && weapon->otyp == HUGE_CLUB && (P_SKILL(P_CLUB) == P_GRAND_MASTER) ) bonus += rnd(24);
	if (weapon && weapon->otyp == HUGE_CLUB && (P_SKILL(P_CLUB) == P_SUPREME_MASTER) ) bonus += rnd(30);

	if (weapon && weapon->otyp == BATTLE_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_SKILLED) ) bonus += rnd(5);
	if (weapon && weapon->otyp == BATTLE_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_EXPERT) ) bonus += rnd(10);
	if (weapon && weapon->otyp == BATTLE_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_MASTER) ) bonus += rnd(15);
	if (weapon && weapon->otyp == BATTLE_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_GRAND_MASTER) ) bonus += rnd(20);
	if (weapon && weapon->otyp == BATTLE_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_SUPREME_MASTER) ) bonus += rnd(25);

	if (weapon && weapon->otyp == MASSIVE_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_SKILLED) ) bonus += rnd(5);
	if (weapon && weapon->otyp == MASSIVE_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_EXPERT) ) bonus += rnd(10);
	if (weapon && weapon->otyp == MASSIVE_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_MASTER) ) bonus += rnd(15);
	if (weapon && weapon->otyp == MASSIVE_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_GRAND_MASTER) ) bonus += rnd(20);
	if (weapon && weapon->otyp == MASSIVE_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_SUPREME_MASTER) ) bonus += rnd(25);

	if (weapon && weapon->otyp == OLDEST_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_SKILLED) ) bonus += rnd(5);
	if (weapon && weapon->otyp == OLDEST_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_EXPERT) ) bonus += rnd(10);
	if (weapon && weapon->otyp == OLDEST_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_MASTER) ) bonus += rnd(15);
	if (weapon && weapon->otyp == OLDEST_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_GRAND_MASTER) ) bonus += rnd(20);
	if (weapon && weapon->otyp == OLDEST_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_SUPREME_MASTER) ) bonus += rnd(25);

	if (weapon && weapon->otyp == IMPACT_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_SKILLED) ) bonus += rnd(5);
	if (weapon && weapon->otyp == IMPACT_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_EXPERT) ) bonus += rnd(10);
	if (weapon && weapon->otyp == IMPACT_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_MASTER) ) bonus += rnd(15);
	if (weapon && weapon->otyp == IMPACT_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_GRAND_MASTER) ) bonus += rnd(20);
	if (weapon && weapon->otyp == IMPACT_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_SUPREME_MASTER) ) bonus += rnd(25);

	if (weapon && weapon->otyp == BO_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_SKILLED) ) bonus += rnd(5);
	if (weapon && weapon->otyp == BO_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_EXPERT) ) bonus += rnd(10);
	if (weapon && weapon->otyp == BO_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_MASTER) ) bonus += rnd(15);
	if (weapon && weapon->otyp == BO_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_GRAND_MASTER) ) bonus += rnd(20);
	if (weapon && weapon->otyp == BO_STAFF && (P_SKILL(P_QUARTERSTAFF) == P_SUPREME_MASTER) ) bonus += rnd(25);

	if (weapon && weapon->otyp == SLEDGE_HAMMER && (P_SKILL(P_HAMMER) == P_SKILLED) ) bonus += rnd(5);
	if (weapon && weapon->otyp == SLEDGE_HAMMER && (P_SKILL(P_HAMMER) == P_EXPERT) ) bonus += rnd(10);
	if (weapon && weapon->otyp == SLEDGE_HAMMER && (P_SKILL(P_HAMMER) == P_MASTER) ) bonus += rnd(15);
	if (weapon && weapon->otyp == SLEDGE_HAMMER && (P_SKILL(P_HAMMER) == P_GRAND_MASTER) ) bonus += rnd(20);
	if (weapon && weapon->otyp == SLEDGE_HAMMER && (P_SKILL(P_HAMMER) == P_SUPREME_MASTER) ) bonus += rnd(25);

	if (weapon && weapon->otyp == THUNDER_HAMMER && (P_SKILL(P_HAMMER) == P_SKILLED) ) bonus += rnd(6);
	if (weapon && weapon->otyp == THUNDER_HAMMER && (P_SKILL(P_HAMMER) == P_EXPERT) ) bonus += rnd(12);
	if (weapon && weapon->otyp == THUNDER_HAMMER && (P_SKILL(P_HAMMER) == P_MASTER) ) bonus += rnd(18);
	if (weapon && weapon->otyp == THUNDER_HAMMER && (P_SKILL(P_HAMMER) == P_GRAND_MASTER) ) bonus += rnd(24);
	if (weapon && weapon->otyp == THUNDER_HAMMER && (P_SKILL(P_HAMMER) == P_SUPREME_MASTER) ) bonus += rnd(30);

	if (weapon && weapon_type(weapon) == P_POLEARMS && (P_SKILL(P_POLEARMS) == P_SKILLED) ) bonus += 1;
	if (weapon && weapon_type(weapon) == P_POLEARMS && (P_SKILL(P_POLEARMS) == P_EXPERT) ) bonus += rnd(2);
	if (weapon && weapon_type(weapon) == P_POLEARMS && (P_SKILL(P_POLEARMS) == P_MASTER) ) bonus += rnd(3);
	if (weapon && weapon_type(weapon) == P_POLEARMS && (P_SKILL(P_POLEARMS) == P_GRAND_MASTER) ) bonus += rnd(4);
	if (weapon && weapon_type(weapon) == P_POLEARMS && (P_SKILL(P_POLEARMS) == P_SUPREME_MASTER) ) bonus += rnd(5);

	if (weapon && weapon_type(weapon) == P_TRIDENT && (P_SKILL(P_TRIDENT) == P_SKILLED) ) bonus += 1;
	if (weapon && weapon_type(weapon) == P_TRIDENT && (P_SKILL(P_TRIDENT) == P_EXPERT) ) bonus += rnd(2);
	if (weapon && weapon_type(weapon) == P_TRIDENT && (P_SKILL(P_TRIDENT) == P_MASTER) ) bonus += rnd(3);
	if (weapon && weapon_type(weapon) == P_TRIDENT && (P_SKILL(P_TRIDENT) == P_GRAND_MASTER) ) bonus += rnd(4);
	if (weapon && weapon_type(weapon) == P_TRIDENT && (P_SKILL(P_TRIDENT) == P_SUPREME_MASTER) ) bonus += rnd(5);

	if (weapon && weapon_type(weapon) == P_LANCE && (P_SKILL(P_LANCE) == P_SKILLED) ) bonus += 1;
	if (weapon && weapon_type(weapon) == P_LANCE && (P_SKILL(P_LANCE) == P_EXPERT) ) bonus += rnd(3);
	if (weapon && weapon_type(weapon) == P_LANCE && (P_SKILL(P_LANCE) == P_MASTER) ) bonus += rnd(5);
	if (weapon && weapon_type(weapon) == P_LANCE && (P_SKILL(P_LANCE) == P_GRAND_MASTER) ) bonus += rnd(7);
	if (weapon && weapon_type(weapon) == P_LANCE && (P_SKILL(P_LANCE) == P_SUPREME_MASTER) ) bonus += rnd(9);

	if (weapon && weapon_type(weapon) == P_WHIP && (P_SKILL(P_WHIP) == P_SKILLED) ) bonus += rnd(2);
	if (weapon && weapon_type(weapon) == P_WHIP && (P_SKILL(P_WHIP) == P_EXPERT) ) bonus += rnd(4);
	if (weapon && weapon_type(weapon) == P_WHIP && (P_SKILL(P_WHIP) == P_MASTER) ) bonus += rnd(6);
	if (weapon && weapon_type(weapon) == P_WHIP && (P_SKILL(P_WHIP) == P_GRAND_MASTER) ) bonus += rnd(8);
	if (weapon && weapon_type(weapon) == P_WHIP && (P_SKILL(P_WHIP) == P_SUPREME_MASTER) ) bonus += rnd(10);

	}

	/* ceramic weapons are super sharp and deal more damage; this also affects blunt ones because realism sux :P --Amy */
	if (weapon && objects[weapon->otyp].oc_material == MT_CERAMIC) bonus += 2;
	/* meteosteel is also high-quality */
	if (weapon && objects[weapon->otyp].oc_material == MT_METEOSTEEL) bonus += 1;

    return bonus;
}

/*
 * Return damage bonus for using ranged weapons. Added by Amy to make the later parts of the game less unplayable.
 */
int
ranged_dam_bonus(weapon)
struct obj *weapon;

{

    int type, wep_type, bonus = 0;

    if (!weapon) return 0;

    wep_type = weapon_type(weapon);

    type = wep_type;
    if (type == P_NONE) {
	bonus = 0;
    } else if (!(PlayerCannotUseSkills) && type <= P_LAST_WEAPON) {	/* bonus for highly skilled, non-dual-wielded weapon */
	switch (P_SKILL(type)) {
	    case P_SKILLED:	bonus =  1; break;
	    case P_EXPERT:	bonus =  1; break;
	    case P_MASTER:	bonus =  2; break;
	    case P_GRAND_MASTER:bonus =  2; break;
	    case P_SUPREME_MASTER:bonus =  3; break;
	    default: bonus = 0; break;
	}
    }

	if (weapon && weapon->otyp == DART_OF_DISINTEGRATION) {
		bonus += rnd(20);
	}

	if (Race_if(PM_GERTEUT)) bonus++;

	if (Race_if(PM_FRO) && (wep_type == P_FIREARM || wep_type == -P_FIREARM)) bonus += rnd(10);

	if (!(PlayerCannotUseSkills)) {

	if (weapon && weapon->otyp == GREAT_DAGGER && (P_SKILL(P_DAGGER) == P_EXPERT) ) bonus += rnd(2);
	if (weapon && weapon->otyp == GREAT_DAGGER && (P_SKILL(P_DAGGER) == P_MASTER) ) bonus += rnd(5);
	if (weapon && weapon->otyp == GREAT_DAGGER && (P_SKILL(P_DAGGER) == P_GRAND_MASTER) ) bonus += rnd(8);
	if (weapon && weapon->otyp == GREAT_DAGGER && (P_SKILL(P_DAGGER) == P_SUPREME_MASTER) ) bonus += rnd(12);

	if (weapon && weapon->otyp == DROVEN_DAGGER && (P_SKILL(P_DAGGER) == P_SKILLED) ) bonus += rnd(10);
	if (weapon && weapon->otyp == DROVEN_DAGGER && (P_SKILL(P_DAGGER) == P_EXPERT) ) bonus += rnd(20);
	if (weapon && weapon->otyp == DROVEN_DAGGER && (P_SKILL(P_DAGGER) == P_MASTER) ) bonus += rnd(35);
	if (weapon && weapon->otyp == DROVEN_DAGGER && (P_SKILL(P_DAGGER) == P_GRAND_MASTER) ) bonus += rnd(50);
	if (weapon && weapon->otyp == DROVEN_DAGGER && (P_SKILL(P_DAGGER) == P_SUPREME_MASTER) ) bonus += rnd(65);

	if (weapon && weapon->otyp == DROVEN_SPEAR && (P_SKILL(P_DAGGER) == P_SKILLED) ) bonus += rnd(12);
	if (weapon && weapon->otyp == DROVEN_SPEAR && (P_SKILL(P_DAGGER) == P_EXPERT) ) bonus += rnd(24);
	if (weapon && weapon->otyp == DROVEN_SPEAR && (P_SKILL(P_DAGGER) == P_MASTER) ) bonus += rnd(40);
	if (weapon && weapon->otyp == DROVEN_SPEAR && (P_SKILL(P_DAGGER) == P_GRAND_MASTER) ) bonus += rnd(60);
	if (weapon && weapon->otyp == DROVEN_SPEAR && (P_SKILL(P_DAGGER) == P_SUPREME_MASTER) ) bonus += rnd(80);

	/* crysknife reverts when thrown, so give lots of extra damage if you still decide to throw it */
	if (weapon && weapon->otyp == CRYSKNIFE && (P_SKILL(P_KNIFE) == P_SKILLED) ) bonus += rnd(16);
	if (weapon && weapon->otyp == CRYSKNIFE && (P_SKILL(P_KNIFE) == P_EXPERT) ) bonus += rnd(30);
	if (weapon && weapon->otyp == CRYSKNIFE && (P_SKILL(P_KNIFE) == P_MASTER) ) bonus += rnd(50);
	if (weapon && weapon->otyp == CRYSKNIFE && (P_SKILL(P_KNIFE) == P_GRAND_MASTER) ) bonus += rnd(80);
	if (weapon && weapon->otyp == CRYSKNIFE && (P_SKILL(P_KNIFE) == P_SUPREME_MASTER) ) bonus += rnd(120);

	if (weapon && weapon->otyp == SPIRIT_THROWER && (P_SKILL(P_JAVELIN) == P_SKILLED) ) bonus += rnd(4);
	if (weapon && weapon->otyp == SPIRIT_THROWER && (P_SKILL(P_JAVELIN) == P_EXPERT) ) bonus += rnd(8);
	if (weapon && weapon->otyp == SPIRIT_THROWER && (P_SKILL(P_JAVELIN) == P_MASTER) ) bonus += rnd(12);
	if (weapon && weapon->otyp == SPIRIT_THROWER && (P_SKILL(P_JAVELIN) == P_GRAND_MASTER) ) bonus += rnd(16);
	if (weapon && weapon->otyp == SPIRIT_THROWER && (P_SKILL(P_JAVELIN) == P_SUPREME_MASTER) ) bonus += rnd(20);

	if (weapon && weapon->otyp == TORPEDO && (P_SKILL(P_JAVELIN) == P_SKILLED) ) bonus += rnd(5);
	if (weapon && weapon->otyp == TORPEDO && (P_SKILL(P_JAVELIN) == P_EXPERT) ) bonus += rnd(10);
	if (weapon && weapon->otyp == TORPEDO && (P_SKILL(P_JAVELIN) == P_MASTER) ) bonus += rnd(15);
	if (weapon && weapon->otyp == TORPEDO && (P_SKILL(P_JAVELIN) == P_GRAND_MASTER) ) bonus += rnd(20);
	if (weapon && weapon->otyp == TORPEDO && (P_SKILL(P_JAVELIN) == P_SUPREME_MASTER) ) bonus += rnd(25);

	if (weapon && weapon->otyp == HOMING_TORPEDO && (P_SKILL(P_JAVELIN) == P_SKILLED) ) bonus += rnd(5);
	if (weapon && weapon->otyp == HOMING_TORPEDO && (P_SKILL(P_JAVELIN) == P_EXPERT) ) bonus += rnd(10);
	if (weapon && weapon->otyp == HOMING_TORPEDO && (P_SKILL(P_JAVELIN) == P_MASTER) ) bonus += rnd(15);
	if (weapon && weapon->otyp == HOMING_TORPEDO && (P_SKILL(P_JAVELIN) == P_GRAND_MASTER) ) bonus += rnd(20);
	if (weapon && weapon->otyp == HOMING_TORPEDO && (P_SKILL(P_JAVELIN) == P_SUPREME_MASTER) ) bonus += rnd(25);

	if (weapon && weapon->otyp == SHURIKEN && (P_SKILL(P_SHURIKEN) == P_MASTER) ) bonus += 1;
	if (weapon && weapon->otyp == SHURIKEN && (P_SKILL(P_SHURIKEN) == P_GRAND_MASTER) ) bonus += rnd(2);
	if (weapon && weapon->otyp == SHURIKEN && (P_SKILL(P_SHURIKEN) == P_SUPREME_MASTER) ) bonus += rnd(3);
	if (weapon && weapon->otyp == NEEDLE && (P_SKILL(P_SHURIKEN) == P_MASTER) ) bonus += 1;
	if (weapon && weapon->otyp == NEEDLE && (P_SKILL(P_SHURIKEN) == P_GRAND_MASTER) ) bonus += rnd(2);
	if (weapon && weapon->otyp == NEEDLE && (P_SKILL(P_SHURIKEN) == P_SUPREME_MASTER) ) bonus += rnd(3);

	if (weapon && is_lightsaber(weapon) && weapon->lamplit) {
		switch (P_SKILL(P_DJEM_SO)) {
			case P_BASIC: bonus += rnd(5); break;
			case P_SKILLED: bonus += rnd(10); break;
			case P_EXPERT: bonus += rnd(15); break;
			case P_MASTER: bonus += rnd(20); break;
			case P_GRAND_MASTER: bonus += rnd(25); break;
			case P_SUPREME_MASTER: bonus += rnd(30); break;
		}

	}

	}

	if (weapon && objects[weapon->otyp].oc_material == MT_CERAMIC) bonus += 2;
	if (weapon && objects[weapon->otyp].oc_material == MT_METEOSTEEL) bonus += 1;

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

		if (PlayerCannotUseSkills) bonus = -2;
		else switch (P_SKILL(type)) {
		    default: impossible("skill_bonus: bad skill %d",P_SKILL(type));
			     /* fall through */
		    case P_ISRESTRICTED:
		    case P_UNSKILLED:   bonus = -2; break;
		    case P_BASIC:       bonus =  0; break;
		    case P_SKILLED:     bonus =  1; break;
		    case P_EXPERT:      bonus =  2; break;
		    case P_MASTER:	bonus =  3; break;
		    case P_GRAND_MASTER:	bonus =  4; break;
		    case P_SUPREME_MASTER:	bonus =  5; break;
		}
    } else if (!(PlayerCannotUseSkills) && (type == P_BARE_HANDED_COMBAT || type == P_MARTIAL_ARTS) ) {
		bonus = (P_SKILL(type) * (martial_bonus() ? 2 : 1)) / 2;
    } else {
    		/* Misc. */
		if (PlayerCannotUseSkills) bonus = -2;
		else switch (P_SKILL(type)) {
		    default: impossible("skill_bonus: bad skill %d",P_SKILL(type));
			     /* fall through */
		    case P_ISRESTRICTED:
		    case P_UNSKILLED:   bonus = -2; break;
		    case P_BASIC:       bonus =  0; break;
		    case P_SKILLED:     bonus =  1; break;
		    case P_EXPERT:      bonus =  2; break;
		    case P_MASTER:	bonus =  3; break;
		    case P_GRAND_MASTER:	bonus =  4; break;
		    case P_SUPREME_MASTER:	bonus =  5; break;
		}
    }

    return bonus;
}

/* Try to return an associated skill for the specified object */
int get_obj_skill(obj, extraskills)
struct obj *obj;
boolean extraskills;
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
	if (obj->otyp == LEATHER_SADDLE) skill = P_RIDING;
	if (obj->otyp == INKA_SADDLE) skill = P_RIDING;

	if (obj->otyp >= SMALL_SHIELD && obj->otyp <= SHIELD_OF_REFLECTION) skill = P_SHIELD;
	if (obj->otyp >= PLATE_MAIL && obj->otyp <= YELLOW_DRAGON_SCALES) skill = P_BODY_ARMOR;
	if (obj->otyp >= WAN_LIGHT && obj->otyp <= WAN_PSYBEAM) skill = P_DEVICES;
	if (obj->otyp == SCR_SYMBIOSIS) skill = P_SYMBIOSIS;
	if (obj->otyp == SCR_CREATE_FAMILIAR) skill = P_PETKEEPING;
	if (obj->otyp == SCR_ALLY) skill = P_PETKEEPING;
	if (obj->otyp == SCR_TAMING) skill = P_PETKEEPING;
	if (obj->otyp == TIN_WHISTLE) skill = P_PETKEEPING;
	if (obj->otyp == GRASS_WHISTLE) skill = P_PETKEEPING;
	if (obj->otyp == MAGIC_WHISTLE) skill = P_PETKEEPING;
	if (obj->otyp == FIRE_HORN) skill = P_PETKEEPING;
	if (obj->otyp == FROST_HORN) skill = P_PETKEEPING;
	if (obj->otyp == TEMPEST_HORN) skill = P_PETKEEPING;
	if (obj->otyp == ETHER_HORN) skill = P_PETKEEPING;
	if (obj->otyp == CHROME_HORN) skill = P_PETKEEPING;
	if (obj->otyp == SHADOW_HORN) skill = P_PETKEEPING;
	if (obj->otyp == DARK_MAGIC_WHISTLE) skill = P_PETKEEPING;
	if (obj->otyp == LEATHER_LEASH) skill = P_PETKEEPING;
	if (obj->otyp == INKA_LEASH) skill = P_PETKEEPING;
	if (obj->otyp == FIGURINE) skill = P_PETKEEPING;
	if (obj->otyp == WOODEN_FLUTE) skill = P_PETKEEPING;
	if (obj->otyp == MAGIC_FLUTE) skill = P_PETKEEPING;
	if (obj->otyp == WOODEN_HARP) skill = P_PETKEEPING;
	if (obj->otyp == MAGIC_HARP) skill = P_PETKEEPING;
	if (obj->otyp == MANACLOAK) skill = P_MEMORIZATION;
	if (obj->otyp == CLOAK_OF_MEMORY) skill = P_MEMORIZATION;
	if (obj->otyp == CORNUTHAUM) skill = P_MEMORIZATION;
	if (obj->otyp == GAUNTLETS_OF_FAST_CASTING) skill = P_MEMORIZATION;
	if (obj->otyp == HELM_OF_BRILLIANCE) skill = P_MEMORIZATION;
	if (obj->otyp == RIN_MEMORY) skill = P_MEMORIZATION;
	if (obj->otyp == AMULET_OF_POWER) skill = P_MEMORIZATION;
	if (obj->otyp == POT_KEEN_MEMORY) skill = P_MEMORIZATION;
	if (obj->otyp == SCR_WONDER) skill = P_MEMORIZATION;
	if (obj->otyp == RIN_POLYMORPH) skill = P_POLYMORPHING;
	if (obj->otyp == RIN_POLYMORPH_CONTROL) skill = P_POLYMORPHING;
	if (obj->otyp == RIN_PROTECTION_FROM_SHAPE_CHAN) skill = P_POLYMORPHING;
	if (obj->otyp == AMULET_OF_POLYMORPH) skill = P_POLYMORPHING;
	if (obj->otyp == AMULET_OF_POLYMORPH_CONTROL) skill = P_POLYMORPHING;
	if (obj->otyp == AMULET_OF_CHANGE) skill = P_POLYMORPHING;
	if (obj->otyp == AMULET_OF_UNCHANGING) skill = P_POLYMORPHING;
	if (obj->otyp == POT_POLYMORPH) skill = P_POLYMORPHING;
	if (obj->otyp == LENSES) skill = P_SEARCHING;
	if (obj->otyp == RADIOGLASSES) skill = P_SEARCHING;
	if (obj->otyp == BOSS_VISOR) skill = P_SEARCHING;
	if (obj->otyp == STETHOSCOPE) skill = P_SEARCHING;
	if (obj->otyp == RIN_SEARCHING) skill = P_SEARCHING;
	if (obj->otyp == RIN_TRAP_REVEALING) skill = P_SEARCHING;
	if (obj->otyp == GOD_O_METER) skill = P_SPIRITUALITY;
	if (obj->otyp == POT_WATER) skill = P_SPIRITUALITY;
	if (obj->otyp == HOLY_WAFER) skill = P_SPIRITUALITY;
	if (obj->otyp == SCR_CONSECRATION) skill = P_SPIRITUALITY;
	if (obj->otyp == SCR_CREATE_ALTAR) skill = P_SPIRITUALITY;
	if (obj->otyp == SYMBIOTE) skill = P_SYMBIOSIS;

	if (extraskills == TRUE) { /* outside of char initialization, more items can unlock skills (god gifts etc.) --Amy */
		if (obj->otyp == TIN_OPENER) skill = P_MARTIAL_ARTS;
		if (obj->otyp == BUDO_NO_SASU) skill = P_MARTIAL_ARTS;
		if (obj->otyp == CAN_OF_GREASE) skill = P_BARE_HANDED_COMBAT;
		if (obj->otyp == LUBRICANT_CAN) skill = P_BARE_HANDED_COMBAT;
		if (obj->otyp == TINNING_KIT) skill = P_TWO_HANDED_WEAPON;
		if (obj->otyp == BINNING_KIT) skill = P_TWO_HANDED_WEAPON;
		if (obj->otyp == CONDOME) skill = P_TWO_WEAPON_COMBAT;
		if (obj->otyp == SOFT_CHASTITY_BELT) skill = P_TWO_WEAPON_COMBAT;
		if (obj->otyp == MAGIC_MARKER) skill = P_TECHNIQUES;
		if (obj->otyp == FELT_TIP_MARKER) skill = P_TECHNIQUES;
		if (obj->otyp == MATERIAL_KIT) skill = P_MISSILE_WEAPONS;
		if (obj->otyp == BLINDFOLD) skill = P_SHII_CHO;
		if (obj->otyp == EYECLOSER) skill = P_MAKASHI;
		if (obj->otyp == DRAGON_EYEPATCH) skill = P_SORESU;
		if (obj->otyp == TOWEL) skill = P_ATARU;
		if (obj->otyp == CHARGER) skill = P_NIMAN;
		if (obj->otyp == MIRROR) skill = P_SHIEN;
		if (obj->otyp == HAIRCLIP) skill = P_SQUEAKING;
		if (obj->otyp == LOCK_PICK) skill = P_GUN_CONTROL;
		if (obj->otyp == EXPENSIVE_CAMERA) skill = P_DJEM_SO;
		if (obj->otyp == SWITCHER) skill = P_VAAPAD;
		if (obj->otyp == PACK_OF_FLOPPIES) skill = P_WEDI;
		if (obj->otyp == INTELLIGENCE_PACK) skill = P_JUYO;

		if (skill == P_NONE) { /* gifts should always be able to unlock a skill; random if none is associated --Amy */
			skill = randomgoodskill();
		}
	}
	/* yeah I know they make no sense, but who cares? I want god gifts to be able to unlock every skill --Amy
	 * since some of those are part of various roles' starting inventories and I don't actually want those roles to
	 * start with the respective skills, the extraskills check was invented */

	if (obj->oclass == IMPLANT_CLASS) skill = P_IMPLANTS;

	if (obj->otyp == WEDGE_SANDALS || obj->otyp == FEMININE_PUMPS || obj->otyp == LEATHER_PEEP_TOES || obj->otyp == HIPPIE_HEELS || obj->otyp == PET_STOMPING_PLATFORM_BOOTS || obj->otyp == SENTIENT_HIGH_HEELED_SHOES || obj->otyp == ATSUZOKO_BOOTS || obj->otyp == COMBAT_STILETTOS || obj->otyp == ITALIAN_HEELS || obj->otyp == LADY_BOOTS || obj->otyp == STILETTO_SANDALS || obj->otyp == HIGH_STILETTOS || obj->otyp == UNFAIR_STILETTOS || obj->otyp == SKY_HIGH_HEELS || obj->otyp == SINFUL_HEELS || obj->otyp == COVETED_BOOTS || obj->otyp == BUM_BUM_BOOTS || obj->otyp == SELF_WILLED_HEELS || obj->otyp == HIGH_HEELED_SKIERS || obj->otyp == RED_SPELL_HEELS || obj->otyp == DESTRUCTIVE_HEELS || obj->otyp == LONG_POINTY_HEELS || obj->otyp == VIOLET_BEAUTY_HEELS || obj->otyp == AUTOSCOOTER_HEELS || obj->otyp == KILLER_HEELS || obj->otyp == HIGH_SCORING_HEELS || obj->otyp == FEMMY_STILETTO_BOOTS || obj->otyp == MADELEINE_PLATEAU_BOOTS || obj->otyp == WENDY_LEATHER_PUMPS || obj->otyp == KATHARINA_PLATFORM_BOOTS || obj->otyp == ELENA_COMBAT_BOOTS || obj->otyp == THAI_COMBAT_BOOTS || obj->otyp == NADJA_BUCKLED_LADY_SHOES || obj->otyp == SANDRA_COMBAT_BOOTS || obj->otyp == NATALJE_BLOCK_HEEL_SANDALS || obj->otyp == MELTEM_COMBAT_BOOTS || obj->otyp == CLAUDIA_WOODEN_SANDALS || obj->otyp == NELLY_LADY_PUMPS || obj->otyp == EVELINE_WEDGE_SANDALS || obj->otyp == JUEN_PEEP_TOES || obj->otyp == JULIETTA_PEEP_TOES) skill = P_HIGH_HEELS;

	if (obj->otyp == RUBBER_BOOTS || obj->otyp == SNEAKERS || obj->otyp == DANCING_SHOES || obj->otyp == SWEET_MOCASSINS || obj->otyp == SOFT_SNEAKERS || obj->otyp == ROLLER_BLADE || obj->otyp == DIFFICULT_BOOTS || obj->otyp == AIRSTEP_BOOTS || obj->otyp == SYNTHETIC_SANDALS || obj->otyp == BRIGHT_CYAN_BEAUTIES || obj->otyp == MARLENA_HIKING_BOOTS || obj->otyp == ANASTASIA_DANCING_SHOES || obj->otyp == JESSICA_LADY_SHOES || obj->otyp == SOLVEJG_MOCASSINS || obj->otyp == ELIF_SNEAKERS || obj->otyp == JEANETTA_GIRL_BOOTS || obj->otyp == YVONNE_GIRL_SNEAKERS || obj->otyp == MAURAH_HUGGING_BOOTS || obj->otyp == SARAH_HUGGING_BOOTS || obj->otyp == LUDGERA_HIKING_BOOTS || obj->otyp == KATI_GIRL_BOOTS || obj->otyp == KARIN_LADY_SANDALS || obj->otyp == KRISTINA_PLATFORM_SNEAKERS || obj->otyp == LOU_SNEAKERS || obj->otyp == ALMUT_SNEAKERS || obj->otyp == ARABELLA_HUGGING_BOOTS) skill = P_SEXY_FLATS;

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
	int i;
	int implantbonus;

	/* initialize skill array; by default, everything is restricted */
	for (skill = 0; skill < P_NUM_SKILLS; skill++) {
	    P_SKILL(skill) = P_ISRESTRICTED;
	    P_MAX_SKILL(skill) = P_ISRESTRICTED;
	    P_ADVANCE(skill) = 0;
	}

	if (Role_if(PM_DEMAGOGUE)) {
		for (skill = 0; skill < P_NUM_SKILLS; skill++) {
			P_ADVANCE(skill) = 20 + rn2(1260); /* random value from 20 to 1279 --Amy */
		}
	}

	if (isamerican) return; /* all skills start out restricted --Amy */
	if (Role_if(PM_ANACHRONOUNBINDER)) return;

	/* Walk through array to set skill maximums */
	for (; class_skill->skill != P_NONE; class_skill++) {
	    skill = class_skill->skill;
	    skmax = class_skill->skmax;

	    /* Amy note: gun control and squeaking are gender-specific at game start (but you can unlock the other later)
	     * except if you're a genderstarist, then you'll start with both */
	    if (!Role_if(PM_GENDERSTARIST) && flags.female && skill == P_GUN_CONTROL) continue;
	    if (!Role_if(PM_GENDERSTARIST) && !flags.female && skill == P_SQUEAKING) continue;
	    /* orb, claw and grinder are alignment-specific at game start except if you're a diablist */
	    if (u.ualign.type != A_LAWFUL && skill == P_ORB) continue;
	    if (u.ualign.type != A_NEUTRAL && skill == P_CLAW) continue;
	    if (u.ualign.type != A_CHAOTIC && skill == P_GRINDER) continue;

	    P_MAX_SKILL(skill) = skmax;
	    if (P_SKILL(skill) == P_ISRESTRICTED)       /* skill pre-set */
			P_SKILL(skill) = P_UNSKILLED;
	    /* Really high potential in the skill
	     * Right now only used for H to H skills
	     */
	    if ((P_MAX_SKILL(skill) > P_EXPERT) && !Race_if(PM_BASTARD) && !Race_if(PM_YEEK) && !isamerican && !Role_if(PM_BINDER) && !Role_if(PM_DEMAGOGUE) && !Role_if(PM_POLITICIAN) && !Role_if(PM_MURDERER) ) P_SKILL(skill) = P_BASIC;

		if ((P_MAX_SKILL(skill) >= P_UNSKILLED) && !Race_if(PM_BASTARD) && !Race_if(PM_YEEK) && !isamerican && !Role_if(PM_BINDER) && !Role_if(PM_DEMAGOGUE) && !Role_if(PM_POLITICIAN) && !Role_if(PM_MURDERER) && Race_if(PM_TUMBLRER) ) P_SKILL(skill) = P_MAX_SKILL(skill);

	}

	/* Set skill for all objects in inventory to be basic */
	if(!Role_if(PM_POLITICIAN) && !Role_if(PM_WILD_TALENT) && !Role_if(PM_SOCIAL_JUSTICE_WARRIOR) && !isamerican && !Role_if(PM_MURDERER)) for (obj = invent; obj; obj = obj->nobj) {
	    skill = get_obj_skill(obj, FALSE);
	    if (skill != P_NONE && !(!Role_if(PM_GENDERSTARIST) && flags.female && skill == P_GUN_CONTROL) && !(!Role_if(PM_GENDERSTARIST) && !flags.female && skill == P_SQUEAKING) && !(u.ualign.type != A_LAWFUL && skill == P_ORB) && !(u.ualign.type != A_NEUTRAL && skill == P_CLAW) && !(u.ualign.type != A_CHAOTIC && skill == P_GRINDER) ) {
		if (!Role_if(PM_BINDER) && !Role_if(PM_DEMAGOGUE) && !Race_if(PM_BASTARD) && !Race_if(PM_YEEK) ) P_SKILL(skill) = P_BASIC;
		else P_SKILL(skill) = P_UNSKILLED;
		/* KMH -- If you came into the dungeon with it, you should at least be skilled */
		if (P_MAX_SKILL(skill) < P_EXPERT) { /* edit by Amy: let's make it expert. */
			if (wizard) pline("Warning: %s should be at least expert.  Fixing...", wpskillname(skill));
			P_MAX_SKILL(skill) = P_EXPERT;
		}
		if (!Role_if(PM_BINDER) && !Role_if(PM_DEMAGOGUE) && !Race_if(PM_BASTARD) && !Race_if(PM_YEEK) && Race_if(PM_TUMBLRER)) P_SKILL(skill) = P_MAX_SKILL(skill);
	    }
	}

	if (Role_if(PM_DOOM_MARINE) && (!Race_if(PM_BASTARD) && !Race_if(PM_YEEK) )) {

		P_SKILL(P_FIREARM) = P_MAX_SKILL(P_FIREARM);
		P_SKILL(P_BARE_HANDED_COMBAT) = P_MAX_SKILL(P_BARE_HANDED_COMBAT);
		P_SKILL(P_TWO_HANDED_SWORD) = P_BASIC;

	}

	if (Role_if(PM_WILD_TALENT)) {
		register int skilltochange = randomgoodskill();
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_BASIC;
		skilltochange = randomgoodskill();
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_BASIC;
		skilltochange = randomgoodskill();
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_BASIC;
		skilltochange = randomgoodskill();
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_BASIC;
		skilltochange = randomgoodskill();
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_BASIC;
		skilltochange = randomgoodskill();
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_SKILLED;
		skilltochange = randomgoodskill();
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_SKILLED;
		skilltochange = randomgoodskill();
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_SKILLED;
		skilltochange = randomgoodskill();
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_SKILLED;
		skilltochange = randomgoodskill();
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_SKILLED;
		skilltochange = randomgoodskill();
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_EXPERT;
		skilltochange = randomgoodskill();
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_EXPERT;
		skilltochange = randomgoodskill();
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_EXPERT;
		skilltochange = randomgoodskill();
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_EXPERT;
		skilltochange = randomgoodskill();
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_EXPERT;
		skilltochange = randomgoodskill();
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_EXPERT;
		skilltochange = randomgoodskill();
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_EXPERT;
		skilltochange = randomgoodskill();
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_MASTER;
		skilltochange = randomgoodskill();
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_MASTER;
		skilltochange = randomgoodskill();
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_MASTER;
		skilltochange = randomgoodskill();
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_GRAND_MASTER;
	}

	if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) {
		register int sjwskillamount = 10 + rn2(21); /* 10-30 */
		register int skilltochange;

		while (sjwskillamount > 0) {
			sjwskillamount--;
			skilltochange = randomgoodskill();
			unrestrict_weapon_skill(skilltochange);
			switch (rnd(18)) {
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
					P_MAX_SKILL(skilltochange) = P_BASIC;
					break;
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
					P_MAX_SKILL(skilltochange) = P_SKILLED;
					break;
				case 12:
				case 13:
				case 14:
				case 15:
					P_MAX_SKILL(skilltochange) = P_EXPERT;
					break;
				case 16:
				case 17:
					P_MAX_SKILL(skilltochange) = P_MASTER;
					break;
				case 18:
					P_MAX_SKILL(skilltochange) = P_GRAND_MASTER;
					break;
			}
		}

	}

	if (Role_if(PM_TRACER)) {
		register int skilltochange = rnd(P_LAST_WEAPON);
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_BASIC;
		skilltochange = rnd(P_LAST_WEAPON);
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_BASIC;
		skilltochange = rnd(P_LAST_WEAPON);
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_BASIC;
		skilltochange = rnd(P_LAST_WEAPON);
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_SKILLED;
		skilltochange = rnd(P_LAST_WEAPON);
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_SKILLED;
		skilltochange = rnd(P_LAST_WEAPON);
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_SKILLED;
		skilltochange = rnd(P_LAST_WEAPON);
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_EXPERT;
		skilltochange = rnd(P_LAST_WEAPON);
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_EXPERT;
		skilltochange = rnd(P_LAST_WEAPON);
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_EXPERT;

		skilltochange = P_FIRST_SPELL + rn2((P_LAST_SPELL - P_FIRST_SPELL) + 1);
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_BASIC;
		skilltochange = P_FIRST_SPELL + rn2((P_LAST_SPELL - P_FIRST_SPELL) + 1);
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_SKILLED;
		skilltochange = P_FIRST_SPELL + rn2((P_LAST_SPELL - P_FIRST_SPELL) + 1);
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_EXPERT;

rerollone:
		skilltochange = P_FIRST_H_TO_H + rn2((P_LAST_H_TO_H - P_FIRST_H_TO_H) + 1);
		if (skilltochange >= P_SHII_CHO && skilltochange <= P_WEDI && rn2(10)) goto rerollone;
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_BASIC;
rerolltwo:
		skilltochange = P_FIRST_H_TO_H + rn2((P_LAST_H_TO_H - P_FIRST_H_TO_H) + 1);
		if (skilltochange >= P_SHII_CHO && skilltochange <= P_WEDI && rn2(10)) goto rerolltwo;
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_SKILLED;
rerollthree:
		skilltochange = P_FIRST_H_TO_H + rn2((P_LAST_H_TO_H - P_FIRST_H_TO_H) + 1);
		if (skilltochange >= P_SHII_CHO && skilltochange <= P_WEDI && rn2(10)) goto rerollthree;
		unrestrict_weapon_skill(skilltochange);
		P_MAX_SKILL(skilltochange) = P_EXPERT;

	}

	/* implant skill is rarely granted by the role, but many handicapped races get bonuses to it --Amy */
	implantbonus = 0;
	switch (Race_switch) {
		default: break;
		case PM_AK_THIEF_IS_DEAD_: implantbonus = 3; break;
		case PM_PLAYER_GLORKUM: implantbonus = 3; break;
		case PM_BATMAN: implantbonus = 1; break;
		case PM_BURNINATOR: implantbonus = 2; break;
		case PM_CHIROPTERAN: implantbonus = 1; break;
		case PM_CLOCKWORK_AUTOMATON: implantbonus = 2; break;
		case PM_ARMED_COCKATRICE: implantbonus = 3; break;
		case PM_CORTEX: implantbonus = 2; break;
		case PM_DEATHMOLD: implantbonus = 4; break;
		case PM_DESTABILIZER: implantbonus = 3; break;
		case PM_WEAPONIZED_DINOSAUR: implantbonus = 1; break;
		case PM_HUMANLIKE_DRAGON: implantbonus = 2; break;
		case PM_HYBRIDRAGON: implantbonus = 2; break;
		case PM_FAWN: implantbonus = 1; break;
		case PM_FELID: implantbonus = 1; break;
		case PM_WEAPON_CUBE: implantbonus = 5; break;
		case PM_PLAYER_SLIME: implantbonus = 6; break;
		case PM_WEAPON_BUG: implantbonus = 3; break;
		case PM_INSECTOID: implantbonus = 3; break;
		case PM_KLACKON: implantbonus = 3; break;
		case PM_JELLY: implantbonus = 5; break;
		case PM_LEVITATOR: implantbonus = 3; break;
		case PM_MISSINGNO: implantbonus = 6; break;
		case PM_MOULD: implantbonus = 5; break;
		case PM_GOAULD: implantbonus = 5; break;
		case PM_ETHEREALOID: implantbonus = 5; break;
		case PM_INCORPOREALOID: implantbonus = 5; break;
		case PM_OCTOPODE: implantbonus = 6; break;
		case PM_BACTERIA: implantbonus = 6; break;
		case PM_POLYINITOR: implantbonus = 5; break;
		case PM_SATRE: implantbonus = 5; break;
		case PM_SHOE: implantbonus = 4; break;
		case PM_SNAKEMAN: implantbonus = 1; break;
		case PM_TRANSFORMER: implantbonus = 2; break;
		case PM_TURTLE: implantbonus = 2; break;
		case PM_UNALIGNMENT_THING: implantbonus = 5; break;
		case PM_UNGENOMOLD: implantbonus = 3; break;
		case PM_PLAYER_SHEEP: implantbonus = 3; break;
		case PM_PLAYER_HOUND: implantbonus = 3; break;
		case PM_KUTAR: implantbonus = 3; break;
		case PM_PLAYER_CERBERUS: implantbonus = 3; break;
		case PM_PLAYER_UNICORN: implantbonus = 1; break;
		case PM_VORTEX: implantbonus = 5; break;
		case PM_WISP: implantbonus = 4; break;
		case PM_TONBERRY: implantbonus = 4; break;
		case PM_WORM_THAT_WALKS: implantbonus = 4; break;
		case PM_HUMAN_WRAITH: implantbonus = 3; break;
		case PM_WEAPON_XORN: implantbonus = 2; break;
		case PM_PLAYER_ZRUTY: implantbonus = 4; break;
		case PM_PLAYER_HULK: implantbonus = 5; break;
		case PM_METAL: implantbonus = 5; break;
	}

	if (implantbonus == 1) {

		if (P_RESTRICTED(P_IMPLANTS)) {
			P_SKILL(P_IMPLANTS) = P_UNSKILLED;
			P_ADVANCE(P_IMPLANTS) = 0;
			P_MAX_SKILL(P_IMPLANTS) = P_BASIC;
		} else switch (P_MAX_SKILL(P_IMPLANTS)) {
			case P_BASIC:
				P_MAX_SKILL(P_IMPLANTS) = P_SKILLED;
				break;
			case P_SKILLED:
				P_MAX_SKILL(P_IMPLANTS) = P_EXPERT;
				break;
			case P_EXPERT:
				P_MAX_SKILL(P_IMPLANTS) = P_MASTER;
				P_SKILL(P_IMPLANTS) = P_BASIC;
				P_ADVANCE(P_IMPLANTS) = 20;
				break;
			case P_MASTER:
				P_MAX_SKILL(P_IMPLANTS) = P_GRAND_MASTER;
				P_SKILL(P_IMPLANTS) = P_BASIC;
				P_ADVANCE(P_IMPLANTS) = 20;
				break;
			case P_GRAND_MASTER:
				P_MAX_SKILL(P_IMPLANTS) = P_SUPREME_MASTER;
				P_SKILL(P_IMPLANTS) = P_SKILLED;
				P_ADVANCE(P_IMPLANTS) = 160;
				break;
		}
	}

	if (implantbonus == 2) {

		if (P_RESTRICTED(P_IMPLANTS)) {
			P_SKILL(P_IMPLANTS) = P_UNSKILLED;
			P_ADVANCE(P_IMPLANTS) = 0;
			P_MAX_SKILL(P_IMPLANTS) = P_SKILLED;
		} else switch (P_MAX_SKILL(P_IMPLANTS)) {
			case P_BASIC:
				P_MAX_SKILL(P_IMPLANTS) = P_EXPERT;
				break;
			case P_SKILLED:
			case P_EXPERT:
				P_MAX_SKILL(P_IMPLANTS) = P_MASTER;
				P_SKILL(P_IMPLANTS) = P_BASIC;
				P_ADVANCE(P_IMPLANTS) = 20;
				break;
			case P_MASTER:
				P_MAX_SKILL(P_IMPLANTS) = P_GRAND_MASTER;
				P_SKILL(P_IMPLANTS) = P_BASIC;
				P_ADVANCE(P_IMPLANTS) = 20;
				break;
			case P_GRAND_MASTER:
				P_MAX_SKILL(P_IMPLANTS) = P_SUPREME_MASTER;
				P_SKILL(P_IMPLANTS) = P_SKILLED;
				P_ADVANCE(P_IMPLANTS) = 160;
				break;
		}
	}

	if (implantbonus == 3) {

		if (P_RESTRICTED(P_IMPLANTS)) {
			P_SKILL(P_IMPLANTS) = P_UNSKILLED;
			P_ADVANCE(P_IMPLANTS) = 0;
			P_MAX_SKILL(P_IMPLANTS) = P_EXPERT;
		} else switch (P_MAX_SKILL(P_IMPLANTS)) {
			case P_BASIC:
				P_MAX_SKILL(P_IMPLANTS) = P_EXPERT;
				break;
			case P_SKILLED:
				P_MAX_SKILL(P_IMPLANTS) = P_MASTER;
				P_SKILL(P_IMPLANTS) = P_BASIC;
				P_ADVANCE(P_IMPLANTS) = 20;
				break;
			case P_EXPERT:
				P_MAX_SKILL(P_IMPLANTS) = P_GRAND_MASTER;
				P_SKILL(P_IMPLANTS) = P_BASIC;
				P_ADVANCE(P_IMPLANTS) = 20;
				break;
			case P_MASTER:
			case P_GRAND_MASTER:
				P_MAX_SKILL(P_IMPLANTS) = P_SUPREME_MASTER;
				P_SKILL(P_IMPLANTS) = P_SKILLED;
				P_ADVANCE(P_IMPLANTS) = 160;
				break;
		}
	}

	if (implantbonus == 4) {

		if (P_RESTRICTED(P_IMPLANTS)) {
			P_SKILL(P_IMPLANTS) = P_BASIC;
			P_ADVANCE(P_IMPLANTS) = 20;
			P_MAX_SKILL(P_IMPLANTS) = P_MASTER;
		} else switch (P_MAX_SKILL(P_IMPLANTS)) {
			case P_BASIC:
				P_MAX_SKILL(P_IMPLANTS) = P_MASTER;
				P_SKILL(P_IMPLANTS) = P_BASIC;
				P_ADVANCE(P_IMPLANTS) = 20;
				break;
			case P_SKILLED:
			case P_EXPERT:
				P_MAX_SKILL(P_IMPLANTS) = P_GRAND_MASTER;
				P_SKILL(P_IMPLANTS) = P_BASIC;
				P_ADVANCE(P_IMPLANTS) = 20;
				break;
			case P_MASTER:
			case P_GRAND_MASTER:
				P_MAX_SKILL(P_IMPLANTS) = P_SUPREME_MASTER;
				P_SKILL(P_IMPLANTS) = P_SKILLED;
				P_ADVANCE(P_IMPLANTS) = 160;
				break;
		}
	}

	if (implantbonus == 5) {

		if (P_RESTRICTED(P_IMPLANTS)) {
			P_SKILL(P_IMPLANTS) = P_BASIC;
			P_ADVANCE(P_IMPLANTS) = 20;
			P_MAX_SKILL(P_IMPLANTS) = P_GRAND_MASTER;
		} else switch (P_MAX_SKILL(P_IMPLANTS)) {
			case P_BASIC:
			case P_SKILLED:
				P_MAX_SKILL(P_IMPLANTS) = P_GRAND_MASTER;
				P_SKILL(P_IMPLANTS) = P_BASIC;
				P_ADVANCE(P_IMPLANTS) = 20;
				break;
			case P_EXPERT:
			case P_MASTER:
			case P_GRAND_MASTER:
				P_MAX_SKILL(P_IMPLANTS) = P_SUPREME_MASTER;
				P_SKILL(P_IMPLANTS) = P_SKILLED;
				P_ADVANCE(P_IMPLANTS) = 160;
				break;
		}
	}

	if (implantbonus == 6) {
		P_MAX_SKILL(P_IMPLANTS) = P_SUPREME_MASTER;
		P_SKILL(P_IMPLANTS) = P_SKILLED;
		P_ADVANCE(P_IMPLANTS) = 160;
	}

	/* Batman obviously has legendary boomerang abilities --Amy */
	if (Race_if(PM_BATMAN)) {
		if (P_RESTRICTED(P_BOOMERANG)) {	
			P_SKILL(P_BOOMERANG) = P_UNSKILLED;
			P_ADVANCE(P_BOOMERANG) = 0;
		}
		P_MAX_SKILL(P_BOOMERANG) = P_GRAND_MASTER;
	}

	if (Race_if(PM_STICKER)) {

		if (P_RESTRICTED(P_DEVICES)) {
			P_SKILL(P_DEVICES) = P_UNSKILLED;
			P_ADVANCE(P_DEVICES) = 0;
			P_MAX_SKILL(P_DEVICES) = P_BASIC;
		} else {
			P_SKILL(P_DEVICES) = P_BASIC;
			if (P_MAX_SKILL(P_DEVICES) == P_EXPERT) P_MAX_SKILL(P_DEVICES) = P_MASTER;
			else if (P_MAX_SKILL(P_DEVICES) == P_MASTER) P_MAX_SKILL(P_DEVICES) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_DEVICES) = P_SUPREME_MASTER;
		}

	}

	if (Race_if(PM_FAWN)) {

		if (P_RESTRICTED(P_DEVICES)) {
			P_SKILL(P_DEVICES) = P_UNSKILLED;
			P_ADVANCE(P_DEVICES) = 0;
			P_MAX_SKILL(P_DEVICES) = P_BASIC;
		} else {
			P_SKILL(P_DEVICES) = P_BASIC;
			if (P_MAX_SKILL(P_DEVICES) == P_EXPERT) P_MAX_SKILL(P_DEVICES) = P_MASTER;
			else if (P_MAX_SKILL(P_DEVICES) == P_MASTER) P_MAX_SKILL(P_DEVICES) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_DEVICES) = P_SUPREME_MASTER;
		}

	}

	if (Race_if(PM_SATRE)) {

		if (P_RESTRICTED(P_DEVICES)) {
			P_SKILL(P_DEVICES) = P_BASIC;
			P_ADVANCE(P_DEVICES) = 20;
			P_MAX_SKILL(P_DEVICES) = P_SKILLED;
		} else {
			P_SKILL(P_DEVICES) = P_BASIC;
			if (P_MAX_SKILL(P_DEVICES) == P_EXPERT) P_MAX_SKILL(P_DEVICES) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_DEVICES) = P_SUPREME_MASTER;
		}

	}

	if (Race_if(PM_MACTHEIST)) {

		if (P_RESTRICTED(P_RIDING)) {
			P_SKILL(P_RIDING) = P_UNSKILLED;
			P_ADVANCE(P_RIDING) = 0;
			P_MAX_SKILL(P_RIDING) = P_BASIC;
		} else {
			if (P_MAX_SKILL(P_RIDING) == P_EXPERT) P_MAX_SKILL(P_RIDING) = P_MASTER;
			else if (P_MAX_SKILL(P_RIDING) == P_MASTER) P_MAX_SKILL(P_RIDING) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_RIDING) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_LANCE)) {
			P_SKILL(P_LANCE) = P_UNSKILLED;
			P_ADVANCE(P_LANCE) = 0;
			P_MAX_SKILL(P_LANCE) = P_BASIC;
		} else {
			if (P_MAX_SKILL(P_LANCE) == P_EXPERT) P_MAX_SKILL(P_LANCE) = P_MASTER;
			else if (P_MAX_SKILL(P_LANCE) == P_MASTER) P_MAX_SKILL(P_LANCE) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_LANCE) = P_SUPREME_MASTER;
		}

	}

	if (Race_if(PM_CARTHAGE)) {

		if (P_RESTRICTED(P_LANCE)) {
			P_SKILL(P_LANCE) = P_UNSKILLED;
			P_ADVANCE(P_LANCE) = 0;
			P_MAX_SKILL(P_LANCE) = P_BASIC;
		}
		if (P_RESTRICTED(P_RIDING)) {
			P_SKILL(P_RIDING) = P_UNSKILLED;
			P_ADVANCE(P_RIDING) = 0;
			P_MAX_SKILL(P_RIDING) = P_BASIC;
		}

		for (skill = 0; skill < P_NUM_SKILLS; skill++) {
			if (skill == P_RIDING || skill == P_LANCE) continue;
			if (P_MAX_SKILL(skill) == P_GRAND_MASTER) P_MAX_SKILL(skill) = P_MASTER;

		}

	}

	if (Race_if(PM_PERVERT)) {

		for (skill = 0; skill < P_NUM_SKILLS; skill++) {
			if (P_MAX_SKILL(skill) == P_EXPERT && (skill == P_RIDING || skill <= P_LAST_WEAPON)) P_MAX_SKILL(skill) = P_MASTER;

		}

		if (P_RESTRICTED(P_RIDING)) {
			P_SKILL(P_RIDING) = P_UNSKILLED;
			P_ADVANCE(P_RIDING) = 0;
			P_MAX_SKILL(P_RIDING) = P_EXPERT;
		}

	}

	if (Race_if(PM_BOVER)) {

		if (P_RESTRICTED(P_RIDING)) {
			P_SKILL(P_RIDING) = P_UNSKILLED;
			P_ADVANCE(P_RIDING) = 0;
			P_MAX_SKILL(P_RIDING) = P_BASIC;
		}

	}

	if (Race_if(PM_KUTAR)) {

		P_SKILL(P_SEARCHING) = P_ISRESTRICTED;
		P_MAX_SKILL(P_SEARCHING) = P_ISRESTRICTED;
		P_ADVANCE(P_SEARCHING) = 0;

	}

	if (Race_if(PM_MANSTER)) {

		for (skill = 0; skill < P_NUM_SKILLS; skill++) {
			if (P_MAX_SKILL(skill) == P_EXPERT) P_MAX_SKILL(skill) = P_BASIC;
			else if (P_MAX_SKILL(skill) == P_MASTER) P_MAX_SKILL(skill) = P_SKILLED;
			else if (P_MAX_SKILL(skill) == P_GRAND_MASTER) P_MAX_SKILL(skill) = P_EXPERT;
			else if (P_MAX_SKILL(skill) == P_SUPREME_MASTER) P_MAX_SKILL(skill) = P_MASTER;

		}
	}

	if (Race_if(PM_WOOKIE)) {

		for (skill = 0; skill < P_NUM_SKILLS; skill++) {

			if (skill == P_LIGHTSABER || (skill >= P_SHII_CHO && skill <= P_WEDI)) {
				if (P_RESTRICTED(skill)) {
					P_SKILL(skill) = P_UNSKILLED;
					P_ADVANCE(skill) = 0;
					P_MAX_SKILL(skill) = P_EXPERT;
				}
				else if (P_MAX_SKILL(skill) == P_EXPERT) P_MAX_SKILL(skill) = P_MASTER;
				else if (P_MAX_SKILL(skill) == P_MASTER) P_MAX_SKILL(skill) = P_GRAND_MASTER;
				else P_MAX_SKILL(skill) = P_SUPREME_MASTER;

			} else {
				if (P_MAX_SKILL(skill) == P_EXPERT) P_MAX_SKILL(skill) = P_SKILLED;
				else if (P_MAX_SKILL(skill) == P_MASTER) P_MAX_SKILL(skill) = P_EXPERT;
				else if (P_MAX_SKILL(skill) == P_GRAND_MASTER) P_MAX_SKILL(skill) = P_MASTER;
				else if (P_MAX_SKILL(skill) == P_SUPREME_MASTER) P_MAX_SKILL(skill) = P_GRAND_MASTER;
			}

		}
	}

	if (Race_if(PM_MONGUNG)) {

		P_SKILL(P_RIDING) = P_ISRESTRICTED;
		P_MAX_SKILL(P_RIDING) = P_ISRESTRICTED;
		P_ADVANCE(P_RIDING) = 0;
	}

	if (Race_if(PM_KORONST)) {

		for (skill = 0; skill < P_NUM_SKILLS; skill++) {
			if (skill > P_LAST_WEAPON || skill == P_SLING || skill == P_FIREARM) continue;
			if (P_MAX_SKILL(skill) == P_EXPERT) P_MAX_SKILL(skill) = P_SKILLED;
			else if (P_MAX_SKILL(skill) == P_MASTER) P_MAX_SKILL(skill) = P_EXPERT;
			else if (P_MAX_SKILL(skill) == P_GRAND_MASTER) P_MAX_SKILL(skill) = P_MASTER;
			else if (P_MAX_SKILL(skill) == P_SUPREME_MASTER) P_MAX_SKILL(skill) = P_GRAND_MASTER;

		}
	}

	if (Race_if(PM_YUGGER)) {

		for (skill = 0; skill < P_NUM_SKILLS; skill++) {
			if (P_MAX_SKILL(skill) >= P_EXPERT) P_MAX_SKILL(skill) = P_EXPERT;
			if (P_RESTRICTED(skill)) {
				P_SKILL(skill) = P_UNSKILLED;
				P_ADVANCE(skill) = 0;
				P_MAX_SKILL(skill) = P_BASIC;
			}

		}
	}

	if (Race_if(PM_HERALD)) {

		for (skill = 0; skill < P_NUM_SKILLS; skill++) {
			if (P_MAX_SKILL(skill) == P_EXPERT) P_MAX_SKILL(skill) = P_SKILLED;
			else if (P_MAX_SKILL(skill) == P_MASTER) P_MAX_SKILL(skill) = P_EXPERT;
			else if (P_MAX_SKILL(skill) == P_GRAND_MASTER) P_MAX_SKILL(skill) = P_MASTER;
			else if (P_MAX_SKILL(skill) == P_SUPREME_MASTER) P_MAX_SKILL(skill) = P_GRAND_MASTER;

		}
	}

	if (Race_if(PM_DEVELOPER)) {

		for (skill = 0; skill < P_NUM_SKILLS; skill++) {
			if (P_RESTRICTED(skill)) {
				P_SKILL(skill) = P_UNSKILLED;
				P_ADVANCE(skill) = 0;
				P_MAX_SKILL(skill) = P_BASIC;
			}
			else if (P_MAX_SKILL(skill) == P_EXPERT) P_MAX_SKILL(skill) = P_MASTER;
			else if (P_MAX_SKILL(skill) == P_MASTER) P_MAX_SKILL(skill) = P_GRAND_MASTER;
			else if (P_MAX_SKILL(skill) == P_GRAND_MASTER) P_MAX_SKILL(skill) = P_SUPREME_MASTER;

		}
	}

	if (Race_if(PM_INKA)) {
		if (P_RESTRICTED(P_OCCULT_SPELL)) {
			P_SKILL(P_OCCULT_SPELL) = P_UNSKILLED;
			P_ADVANCE(P_OCCULT_SPELL) = 0;
			P_MAX_SKILL(P_OCCULT_SPELL) = P_EXPERT;
		} else {
			if (P_MAX_SKILL(P_OCCULT_SPELL) == P_EXPERT) P_MAX_SKILL(P_OCCULT_SPELL) = P_MASTER;
			else if (P_MAX_SKILL(P_OCCULT_SPELL) == P_MASTER) P_MAX_SKILL(P_OCCULT_SPELL) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_OCCULT_SPELL) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_SPIRITUALITY)) {
			P_SKILL(P_SPIRITUALITY) = P_UNSKILLED;
			P_ADVANCE(P_SPIRITUALITY) = 0;
			P_MAX_SKILL(P_SPIRITUALITY) = P_BASIC;
		}
	}

	if (Race_if(PM_TONBERRY)) {
		if (P_RESTRICTED(P_AXE)) {
			P_SKILL(P_AXE) = P_UNSKILLED;
			P_ADVANCE(P_AXE) = 0;
			P_MAX_SKILL(P_AXE) = P_EXPERT;
		} else {
			if (P_MAX_SKILL(P_AXE) == P_EXPERT) P_MAX_SKILL(P_AXE) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_AXE) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_KNIFE)) {
			P_SKILL(P_KNIFE) = P_UNSKILLED;
			P_ADVANCE(P_KNIFE) = 0;
			P_MAX_SKILL(P_KNIFE) = P_EXPERT;
		} else {
			if (P_MAX_SKILL(P_KNIFE) == P_EXPERT) P_MAX_SKILL(P_KNIFE) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_KNIFE) = P_SUPREME_MASTER;
		}
	}

	if (Race_if(PM_ENGCHIP)) {
		if (P_RESTRICTED(P_PETKEEPING)) {
			P_SKILL(P_PETKEEPING) = P_BASIC;
			P_ADVANCE(P_PETKEEPING) = 20;
			P_MAX_SKILL(P_PETKEEPING) = P_SKILLED;
		} else {
			if (P_MAX_SKILL(P_PETKEEPING) == P_EXPERT) P_MAX_SKILL(P_PETKEEPING) = P_MASTER;
			else if (P_MAX_SKILL(P_PETKEEPING) == P_MASTER) P_MAX_SKILL(P_PETKEEPING) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_PETKEEPING) = P_SUPREME_MASTER;
			P_SKILL(P_PETKEEPING) = P_BASIC;
		}
	}

	if (Race_if(PM_MAGYAR)) {
		if (P_RESTRICTED(P_BOW)) {
			P_SKILL(P_BOW) = P_UNSKILLED;
			P_ADVANCE(P_BOW) = 0;
			P_MAX_SKILL(P_BOW) = P_EXPERT;
		} else {
			P_SKILL(P_BOW) = P_BASIC;
			if (P_MAX_SKILL(P_BOW) == P_EXPERT) P_MAX_SKILL(P_BOW) = P_MASTER;
			else if (P_MAX_SKILL(P_BOW) == P_MASTER) P_MAX_SKILL(P_BOW) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_BOW) = P_SUPREME_MASTER;
		}

	}

	if (Race_if(PM_TURMENE)) {
		if (P_RESTRICTED(P_FIREARM)) {
			P_SKILL(P_FIREARM) = P_BASIC;
			P_ADVANCE(P_FIREARM) = 20;
			P_MAX_SKILL(P_FIREARM) = P_EXPERT;
		} else {
			P_SKILL(P_FIREARM) = P_BASIC;
			if (P_MAX_SKILL(P_FIREARM) == P_EXPERT) P_MAX_SKILL(P_FIREARM) = P_MASTER;
			else if (P_MAX_SKILL(P_FIREARM) == P_MASTER) P_MAX_SKILL(P_FIREARM) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_FIREARM) = P_SUPREME_MASTER;
		}

	}

	if (Race_if(PM_JAPURA)) {
		if (P_RESTRICTED(P_LONG_SWORD)) {
			P_SKILL(P_LONG_SWORD) = P_UNSKILLED;
			P_ADVANCE(P_LONG_SWORD) = 0;
			P_MAX_SKILL(P_LONG_SWORD) = P_BASIC;
		} else {
			P_SKILL(P_LONG_SWORD) = P_BASIC;
			if (P_MAX_SKILL(P_LONG_SWORD) == P_EXPERT) P_MAX_SKILL(P_LONG_SWORD) = P_MASTER;
			else if (P_MAX_SKILL(P_LONG_SWORD) == P_MASTER) P_MAX_SKILL(P_LONG_SWORD) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_LONG_SWORD) = P_SUPREME_MASTER;
		}

	}

	if (Race_if(PM_JAVA)) {
		if (P_RESTRICTED(P_JAVELIN)) {
			P_SKILL(P_JAVELIN) = P_UNSKILLED;
			P_ADVANCE(P_JAVELIN) = 0;
			P_MAX_SKILL(P_JAVELIN) = P_EXPERT;
		} else {
			P_SKILL(P_JAVELIN) = P_BASIC;
			if (P_MAX_SKILL(P_JAVELIN) == P_EXPERT) P_MAX_SKILL(P_JAVELIN) = P_MASTER;
			else if (P_MAX_SKILL(P_JAVELIN) == P_MASTER) P_MAX_SKILL(P_JAVELIN) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_JAVELIN) = P_SUPREME_MASTER;
		}

	}

	if (Race_if(PM_NEMESIS)) {
		if (P_RESTRICTED(P_TRIDENT)) {
			P_SKILL(P_TRIDENT) = P_UNSKILLED;
			P_ADVANCE(P_TRIDENT) = 0;
			P_MAX_SKILL(P_TRIDENT) = P_EXPERT;
		} else {
			P_SKILL(P_TRIDENT) = P_BASIC;
			if (P_MAX_SKILL(P_TRIDENT) == P_EXPERT) P_MAX_SKILL(P_TRIDENT) = P_MASTER;
			else if (P_MAX_SKILL(P_TRIDENT) == P_MASTER) P_MAX_SKILL(P_TRIDENT) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_TRIDENT) = P_SUPREME_MASTER;
		}
	}

	if (Race_if(PM_KORONST)) {
		if (P_RESTRICTED(P_SLING)) {
			P_SKILL(P_SLING) = P_BASIC;
			P_ADVANCE(P_SLING) = 20;
			P_MAX_SKILL(P_SLING) = P_MASTER;
		} else {
			P_SKILL(P_SLING) = P_BASIC;
			if (P_MAX_SKILL(P_SLING) == P_EXPERT) P_MAX_SKILL(P_SLING) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_SLING) = P_SUPREME_MASTER;
		}
	}

	if (Race_if(PM_FRO)) {
		if (P_RESTRICTED(P_AXE)) {
			P_SKILL(P_AXE) = P_UNSKILLED;
			P_ADVANCE(P_AXE) = 0;
			P_MAX_SKILL(P_AXE) = P_EXPERT;
		} else {
			P_SKILL(P_AXE) = P_BASIC;
			if (P_MAX_SKILL(P_AXE) == P_EXPERT) P_MAX_SKILL(P_AXE) = P_MASTER;
			else if (P_MAX_SKILL(P_AXE) == P_MASTER) P_MAX_SKILL(P_AXE) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_AXE) = P_SUPREME_MASTER;
		}

		if (P_RESTRICTED(P_RIDING)) {
			P_SKILL(P_RIDING) = P_UNSKILLED;
			P_ADVANCE(P_RIDING) = 0;
			P_MAX_SKILL(P_RIDING) = P_BASIC;
		}

	}

	if (Race_if(PM_SWIKNI)) {
		if (P_RESTRICTED(P_KNIFE)) {
			P_SKILL(P_KNIFE) = P_BASIC;
			P_ADVANCE(P_KNIFE) = 20;
			P_MAX_SKILL(P_KNIFE) = P_MASTER;
		} else {
			P_SKILL(P_KNIFE) = P_BASIC;
			if (P_MAX_SKILL(P_KNIFE) == P_EXPERT) P_MAX_SKILL(P_KNIFE) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_KNIFE) = P_SUPREME_MASTER;
		}

		if (P_RESTRICTED(P_FIREARM)) {
			P_SKILL(P_FIREARM) = P_UNSKILLED;
			P_ADVANCE(P_FIREARM) = 0;
			P_MAX_SKILL(P_FIREARM) = P_SKILLED;
		}

	}

	if (Race_if(PM_BULDOZGAR)) {
		if (P_RESTRICTED(P_BODY_ARMOR)) {
			P_SKILL(P_BODY_ARMOR) = P_UNSKILLED;
			P_ADVANCE(P_BODY_ARMOR) = 0;
			P_MAX_SKILL(P_BODY_ARMOR) = P_EXPERT;
		} else {
			P_SKILL(P_BODY_ARMOR) = P_BASIC;
			if (P_MAX_SKILL(P_BODY_ARMOR) == P_EXPERT) P_MAX_SKILL(P_BODY_ARMOR) = P_MASTER;
			else if (P_MAX_SKILL(P_BODY_ARMOR) == P_MASTER) P_MAX_SKILL(P_BODY_ARMOR) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_BODY_ARMOR) = P_SUPREME_MASTER;
		}


	}

	if (Race_if(PM_PLAYER_ASURA)) {
		if (P_RESTRICTED(P_TWO_WEAPON_COMBAT)) {
			P_SKILL(P_TWO_WEAPON_COMBAT) = P_UNSKILLED;
			P_ADVANCE(P_TWO_WEAPON_COMBAT) = 0;
			P_MAX_SKILL(P_TWO_WEAPON_COMBAT) = P_EXPERT;
		} else {
			P_SKILL(P_TWO_WEAPON_COMBAT) = P_BASIC;
			if (P_MAX_SKILL(P_TWO_WEAPON_COMBAT) == P_EXPERT) P_MAX_SKILL(P_TWO_WEAPON_COMBAT) = P_MASTER;
			else if (P_MAX_SKILL(P_TWO_WEAPON_COMBAT) == P_MASTER) P_MAX_SKILL(P_TWO_WEAPON_COMBAT) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_TWO_WEAPON_COMBAT) = P_SUPREME_MASTER;
		}

	}

	if (Race_if(PM_GREURO)) {
		if (P_MAX_SKILL(P_BOW) > P_BASIC) {
			P_MAX_SKILL(P_BOW) = P_BASIC;
			if (P_SKILL(P_BOW) > P_UNSKILLED) P_SKILL(P_BOW) = P_UNSKILLED;
			if (P_ADVANCE(P_BOW) > 0) P_ADVANCE(P_BOW) = 0;

		}
		if (P_ADVANCE(P_BOW)) P_ADVANCE(P_BOW) = 0;

		if (P_MAX_SKILL(P_CROSSBOW) > P_BASIC) {
			P_MAX_SKILL(P_CROSSBOW) = P_BASIC;
			if (P_SKILL(P_CROSSBOW) > P_UNSKILLED) P_SKILL(P_CROSSBOW) = P_UNSKILLED;
			if (P_ADVANCE(P_CROSSBOW) > 0) P_ADVANCE(P_CROSSBOW) = 0;

		}
		if (P_ADVANCE(P_CROSSBOW)) P_ADVANCE(P_CROSSBOW) = 0;

		if (P_RESTRICTED(P_RIDING)) {
			P_SKILL(P_RIDING) = P_BASIC;
			P_ADVANCE(P_RIDING) = 20;
			P_MAX_SKILL(P_RIDING) = P_BASIC;
		} else {
			P_SKILL(P_RIDING) = P_BASIC;
			if (P_MAX_SKILL(P_RIDING) == P_EXPERT) P_MAX_SKILL(P_RIDING) = P_MASTER;
			else if (P_MAX_SKILL(P_RIDING) == P_MASTER) P_MAX_SKILL(P_RIDING) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_RIDING) = P_SUPREME_MASTER;
		}

		if (P_RESTRICTED(P_POLEARMS)) {
			P_SKILL(P_POLEARMS) = P_BASIC;
			P_ADVANCE(P_POLEARMS) = 20;
			P_MAX_SKILL(P_POLEARMS) = P_EXPERT;
		} else {
			P_SKILL(P_POLEARMS) = P_BASIC;
			if (P_MAX_SKILL(P_POLEARMS) == P_EXPERT) P_MAX_SKILL(P_POLEARMS) = P_MASTER;
			else if (P_MAX_SKILL(P_POLEARMS) == P_MASTER) P_MAX_SKILL(P_POLEARMS) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_POLEARMS) = P_SUPREME_MASTER;
		}

		if (P_RESTRICTED(P_LANCE)) {
			P_SKILL(P_LANCE) = P_BASIC;
			P_ADVANCE(P_LANCE) = 20;
			P_MAX_SKILL(P_LANCE) = P_EXPERT;
		} else {
			P_SKILL(P_LANCE) = P_BASIC;
			if (P_MAX_SKILL(P_LANCE) == P_EXPERT) P_MAX_SKILL(P_LANCE) = P_MASTER;
			else if (P_MAX_SKILL(P_LANCE) == P_MASTER) P_MAX_SKILL(P_LANCE) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_LANCE) = P_SUPREME_MASTER;
		}

	}

	if (Race_if(PM_BACTERIA)) {
		if (P_RESTRICTED(P_SYMBIOSIS)) {
			P_SKILL(P_SYMBIOSIS) = P_UNSKILLED;
			P_ADVANCE(P_SYMBIOSIS) = 0;
			P_MAX_SKILL(P_SYMBIOSIS) = P_EXPERT;
		} else {
			P_SKILL(P_SYMBIOSIS) = P_BASIC;
			if (P_MAX_SKILL(P_SYMBIOSIS) == P_EXPERT) P_MAX_SKILL(P_SYMBIOSIS) = P_MASTER;
			else if (P_MAX_SKILL(P_SYMBIOSIS) == P_MASTER) P_MAX_SKILL(P_SYMBIOSIS) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_SYMBIOSIS) = P_SUPREME_MASTER;
		}

	}

	if (Race_if(PM_GOAULD)) {
		if (P_RESTRICTED(P_SYMBIOSIS)) {
			P_SKILL(P_SYMBIOSIS) = P_BASIC;
			P_ADVANCE(P_SYMBIOSIS) = 20;
			P_MAX_SKILL(P_SYMBIOSIS) = P_MASTER;
		} else {
			P_SKILL(P_SYMBIOSIS) = P_BASIC;
			if (P_MAX_SKILL(P_SYMBIOSIS) == P_EXPERT) P_MAX_SKILL(P_SYMBIOSIS) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_SYMBIOSIS) = P_SUPREME_MASTER;
		}

	}

	if (Race_if(PM_ELONA_SNAIL)) {

		if (P_RESTRICTED(P_MARTIAL_ARTS)) {
			P_SKILL(P_MARTIAL_ARTS) = P_BASIC;
			P_ADVANCE(P_MARTIAL_ARTS) = 20;
			P_MAX_SKILL(P_MARTIAL_ARTS) = P_SKILLED;
		} else {
			P_SKILL(P_MARTIAL_ARTS) = P_BASIC;
			if (P_MAX_SKILL(P_MARTIAL_ARTS) == P_EXPERT) P_MAX_SKILL(P_MARTIAL_ARTS) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_MARTIAL_ARTS) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_BOW)) {
			P_SKILL(P_BOW) = P_BASIC;
			P_ADVANCE(P_BOW) = 20;
			P_MAX_SKILL(P_BOW) = P_SKILLED;
		} else {
			P_SKILL(P_BOW) = P_BASIC;
			if (P_MAX_SKILL(P_BOW) == P_EXPERT) P_MAX_SKILL(P_BOW) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_BOW) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_CROSSBOW)) {
			P_SKILL(P_CROSSBOW) = P_BASIC;
			P_ADVANCE(P_CROSSBOW) = 20;
			P_MAX_SKILL(P_CROSSBOW) = P_SKILLED;
		} else {
			P_SKILL(P_CROSSBOW) = P_BASIC;
			if (P_MAX_SKILL(P_CROSSBOW) == P_EXPERT) P_MAX_SKILL(P_CROSSBOW) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_CROSSBOW) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_SLING)) {
			P_SKILL(P_SLING) = P_BASIC;
			P_ADVANCE(P_SLING) = 20;
			P_MAX_SKILL(P_SLING) = P_SKILLED;
		} else {
			P_SKILL(P_SLING) = P_BASIC;
			if (P_MAX_SKILL(P_SLING) == P_EXPERT) P_MAX_SKILL(P_SLING) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_SLING) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_FIREARM)) {
			P_SKILL(P_FIREARM) = P_BASIC;
			P_ADVANCE(P_FIREARM) = 20;
			P_MAX_SKILL(P_FIREARM) = P_SKILLED;
		} else {
			P_SKILL(P_FIREARM) = P_BASIC;
			if (P_MAX_SKILL(P_FIREARM) == P_EXPERT) P_MAX_SKILL(P_FIREARM) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_FIREARM) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_DART)) {
			P_SKILL(P_DART) = P_BASIC;
			P_ADVANCE(P_DART) = 20;
			P_MAX_SKILL(P_DART) = P_SKILLED;
		} else {
			P_SKILL(P_DART) = P_BASIC;
			if (P_MAX_SKILL(P_DART) == P_EXPERT) P_MAX_SKILL(P_DART) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_DART) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_SHURIKEN)) {
			P_SKILL(P_SHURIKEN) = P_BASIC;
			P_ADVANCE(P_SHURIKEN) = 20;
			P_MAX_SKILL(P_SHURIKEN) = P_SKILLED;
		} else {
			P_SKILL(P_SHURIKEN) = P_BASIC;
			if (P_MAX_SKILL(P_SHURIKEN) == P_EXPERT) P_MAX_SKILL(P_SHURIKEN) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_SHURIKEN) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_PETKEEPING)) {
			P_SKILL(P_PETKEEPING) = P_BASIC;
			P_ADVANCE(P_PETKEEPING) = 20;
			P_MAX_SKILL(P_PETKEEPING) = P_SKILLED;
		} else {
			P_SKILL(P_PETKEEPING) = P_BASIC;
			if (P_MAX_SKILL(P_PETKEEPING) == P_EXPERT) P_MAX_SKILL(P_PETKEEPING) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_PETKEEPING) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_SYMBIOSIS)) {
			P_SKILL(P_SYMBIOSIS) = P_BASIC;
			P_ADVANCE(P_SYMBIOSIS) = 20;
			P_MAX_SKILL(P_SYMBIOSIS) = P_SKILLED;
		} else {
			P_SKILL(P_SYMBIOSIS) = P_BASIC;
			if (P_MAX_SKILL(P_SYMBIOSIS) == P_EXPERT) P_MAX_SKILL(P_SYMBIOSIS) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_SYMBIOSIS) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_MEMORIZATION)) {
			P_SKILL(P_MEMORIZATION) = P_BASIC;
			P_ADVANCE(P_MEMORIZATION) = 20;
			P_MAX_SKILL(P_MEMORIZATION) = P_SKILLED;
		} else {
			P_SKILL(P_MEMORIZATION) = P_BASIC;
			if (P_MAX_SKILL(P_MEMORIZATION) == P_EXPERT) P_MAX_SKILL(P_MEMORIZATION) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_MEMORIZATION) = P_SUPREME_MASTER;
		}
		if (flags.female || Role_if(PM_GENDERSTARIST)) {
			if (P_RESTRICTED(P_SQUEAKING)) {
				P_SKILL(P_SQUEAKING) = P_BASIC;
				P_ADVANCE(P_SQUEAKING) = 20;
				P_MAX_SKILL(P_SQUEAKING) = P_SKILLED;
			} else {
				P_SKILL(P_SQUEAKING) = P_BASIC;
				if (P_MAX_SKILL(P_SQUEAKING) == P_EXPERT) P_MAX_SKILL(P_SQUEAKING) = P_GRAND_MASTER;
				else P_MAX_SKILL(P_SQUEAKING) = P_SUPREME_MASTER;
			}
		}
		if (!flags.female || Role_if(PM_GENDERSTARIST)) {
			if (P_RESTRICTED(P_GUN_CONTROL)) {
				P_SKILL(P_GUN_CONTROL) = P_BASIC;
				P_ADVANCE(P_GUN_CONTROL) = 20;
				P_MAX_SKILL(P_GUN_CONTROL) = P_SKILLED;
			} else {
				P_SKILL(P_GUN_CONTROL) = P_BASIC;
				if (P_MAX_SKILL(P_GUN_CONTROL) == P_EXPERT) P_MAX_SKILL(P_GUN_CONTROL) = P_GRAND_MASTER;
				else P_MAX_SKILL(P_GUN_CONTROL) = P_SUPREME_MASTER;
			}
		}
		if (P_RESTRICTED(P_MISSILE_WEAPONS)) {
			P_SKILL(P_MISSILE_WEAPONS) = P_BASIC;
			P_ADVANCE(P_MISSILE_WEAPONS) = 20;
			P_MAX_SKILL(P_MISSILE_WEAPONS) = P_SKILLED;
		} else {
			P_SKILL(P_MISSILE_WEAPONS) = P_BASIC;
			if (P_MAX_SKILL(P_MISSILE_WEAPONS) == P_EXPERT) P_MAX_SKILL(P_MISSILE_WEAPONS) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_MISSILE_WEAPONS) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_TECHNIQUES)) {
			P_SKILL(P_TECHNIQUES) = P_BASIC;
			P_ADVANCE(P_TECHNIQUES) = 20;
			P_MAX_SKILL(P_TECHNIQUES) = P_SKILLED;
		} else {
			P_SKILL(P_TECHNIQUES) = P_BASIC;
			if (P_MAX_SKILL(P_TECHNIQUES) == P_EXPERT) P_MAX_SKILL(P_TECHNIQUES) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_TECHNIQUES) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_IMPLANTS)) {
			P_SKILL(P_IMPLANTS) = P_BASIC;
			P_ADVANCE(P_IMPLANTS) = 20;
			P_MAX_SKILL(P_IMPLANTS) = P_EXPERT;
		} else {
			P_SKILL(P_IMPLANTS) = P_SKILLED;
			P_MAX_SKILL(P_IMPLANTS) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_DEVICES)) {
			P_SKILL(P_DEVICES) = P_BASIC;
			P_ADVANCE(P_DEVICES) = 20;
			P_MAX_SKILL(P_DEVICES) = P_SKILLED;
		} else {
			P_SKILL(P_DEVICES) = P_BASIC;
			if (P_MAX_SKILL(P_DEVICES) == P_EXPERT) P_MAX_SKILL(P_DEVICES) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_DEVICES) = P_SUPREME_MASTER;
		}

		if (P_RESTRICTED(P_SEARCHING)) {
			P_SKILL(P_SEARCHING) = P_UNSKILLED;
			P_ADVANCE(P_SEARCHING) = 0;
			P_MAX_SKILL(P_SEARCHING) = P_BASIC;
		} else {
			P_SKILL(P_SEARCHING) = P_BASIC;
			if (P_MAX_SKILL(P_SEARCHING) == P_EXPERT) P_MAX_SKILL(P_SEARCHING) = P_MASTER;
			else if (P_MAX_SKILL(P_SEARCHING) == P_MASTER) P_MAX_SKILL(P_SEARCHING) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_SEARCHING) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_POLYMORPHING)) {
			P_SKILL(P_POLYMORPHING) = P_UNSKILLED;
			P_ADVANCE(P_POLYMORPHING) = 0;
			P_MAX_SKILL(P_POLYMORPHING) = P_BASIC;
		} else {
			P_SKILL(P_POLYMORPHING) = P_BASIC;
			if (P_MAX_SKILL(P_POLYMORPHING) == P_EXPERT) P_MAX_SKILL(P_POLYMORPHING) = P_MASTER;
			else if (P_MAX_SKILL(P_POLYMORPHING) == P_MASTER) P_MAX_SKILL(P_POLYMORPHING) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_POLYMORPHING) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_SPIRITUALITY)) {
			P_SKILL(P_SPIRITUALITY) = P_UNSKILLED;
			P_ADVANCE(P_SPIRITUALITY) = 0;
			P_MAX_SKILL(P_SPIRITUALITY) = P_BASIC;
		} else {
			P_SKILL(P_SPIRITUALITY) = P_BASIC;
			if (P_MAX_SKILL(P_SPIRITUALITY) == P_EXPERT) P_MAX_SKILL(P_SPIRITUALITY) = P_MASTER;
			else if (P_MAX_SKILL(P_SPIRITUALITY) == P_MASTER) P_MAX_SKILL(P_SPIRITUALITY) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_SPIRITUALITY) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_RIDING)) {
			P_SKILL(P_RIDING) = P_UNSKILLED;
			P_ADVANCE(P_RIDING) = 0;
			P_MAX_SKILL(P_RIDING) = P_BASIC;
		} else {
			P_SKILL(P_RIDING) = P_BASIC;
			if (P_MAX_SKILL(P_RIDING) == P_EXPERT) P_MAX_SKILL(P_RIDING) = P_MASTER;
			else if (P_MAX_SKILL(P_RIDING) == P_MASTER) P_MAX_SKILL(P_RIDING) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_RIDING) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_ATTACK_SPELL)) {
			P_SKILL(P_ATTACK_SPELL) = P_UNSKILLED;
			P_ADVANCE(P_ATTACK_SPELL) = 0;
			P_MAX_SKILL(P_ATTACK_SPELL) = P_BASIC;
		} else {
			P_SKILL(P_ATTACK_SPELL) = P_BASIC;
			if (P_MAX_SKILL(P_ATTACK_SPELL) == P_EXPERT) P_MAX_SKILL(P_ATTACK_SPELL) = P_MASTER;
			else if (P_MAX_SKILL(P_ATTACK_SPELL) == P_MASTER) P_MAX_SKILL(P_ATTACK_SPELL) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_ATTACK_SPELL) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_HEALING_SPELL)) {
			P_SKILL(P_HEALING_SPELL) = P_UNSKILLED;
			P_ADVANCE(P_HEALING_SPELL) = 0;
			P_MAX_SKILL(P_HEALING_SPELL) = P_BASIC;
		} else {
			P_SKILL(P_HEALING_SPELL) = P_BASIC;
			if (P_MAX_SKILL(P_HEALING_SPELL) == P_EXPERT) P_MAX_SKILL(P_HEALING_SPELL) = P_MASTER;
			else if (P_MAX_SKILL(P_HEALING_SPELL) == P_MASTER) P_MAX_SKILL(P_HEALING_SPELL) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_HEALING_SPELL) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_DIVINATION_SPELL)) {
			P_SKILL(P_DIVINATION_SPELL) = P_UNSKILLED;
			P_ADVANCE(P_DIVINATION_SPELL) = 0;
			P_MAX_SKILL(P_DIVINATION_SPELL) = P_BASIC;
		} else {
			P_SKILL(P_DIVINATION_SPELL) = P_BASIC;
			if (P_MAX_SKILL(P_DIVINATION_SPELL) == P_EXPERT) P_MAX_SKILL(P_DIVINATION_SPELL) = P_MASTER;
			else if (P_MAX_SKILL(P_DIVINATION_SPELL) == P_MASTER) P_MAX_SKILL(P_DIVINATION_SPELL) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_DIVINATION_SPELL) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_ENCHANTMENT_SPELL)) {
			P_SKILL(P_ENCHANTMENT_SPELL) = P_UNSKILLED;
			P_ADVANCE(P_ENCHANTMENT_SPELL) = 0;
			P_MAX_SKILL(P_ENCHANTMENT_SPELL) = P_BASIC;
		} else {
			P_SKILL(P_ENCHANTMENT_SPELL) = P_BASIC;
			if (P_MAX_SKILL(P_ENCHANTMENT_SPELL) == P_EXPERT) P_MAX_SKILL(P_ENCHANTMENT_SPELL) = P_MASTER;
			else if (P_MAX_SKILL(P_ENCHANTMENT_SPELL) == P_MASTER) P_MAX_SKILL(P_ENCHANTMENT_SPELL) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_ENCHANTMENT_SPELL) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_PROTECTION_SPELL)) {
			P_SKILL(P_PROTECTION_SPELL) = P_UNSKILLED;
			P_ADVANCE(P_PROTECTION_SPELL) = 0;
			P_MAX_SKILL(P_PROTECTION_SPELL) = P_BASIC;
		} else {
			P_SKILL(P_PROTECTION_SPELL) = P_BASIC;
			if (P_MAX_SKILL(P_PROTECTION_SPELL) == P_EXPERT) P_MAX_SKILL(P_PROTECTION_SPELL) = P_MASTER;
			else if (P_MAX_SKILL(P_PROTECTION_SPELL) == P_MASTER) P_MAX_SKILL(P_PROTECTION_SPELL) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_PROTECTION_SPELL) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_BODY_SPELL)) {
			P_SKILL(P_BODY_SPELL) = P_UNSKILLED;
			P_ADVANCE(P_BODY_SPELL) = 0;
			P_MAX_SKILL(P_BODY_SPELL) = P_BASIC;
		} else {
			P_SKILL(P_BODY_SPELL) = P_BASIC;
			if (P_MAX_SKILL(P_BODY_SPELL) == P_EXPERT) P_MAX_SKILL(P_BODY_SPELL) = P_MASTER;
			else if (P_MAX_SKILL(P_BODY_SPELL) == P_MASTER) P_MAX_SKILL(P_BODY_SPELL) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_BODY_SPELL) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_MATTER_SPELL)) {
			P_SKILL(P_MATTER_SPELL) = P_UNSKILLED;
			P_ADVANCE(P_MATTER_SPELL) = 0;
			P_MAX_SKILL(P_MATTER_SPELL) = P_BASIC;
		} else {
			P_SKILL(P_MATTER_SPELL) = P_BASIC;
			if (P_MAX_SKILL(P_MATTER_SPELL) == P_EXPERT) P_MAX_SKILL(P_MATTER_SPELL) = P_MASTER;
			else if (P_MAX_SKILL(P_MATTER_SPELL) == P_MASTER) P_MAX_SKILL(P_MATTER_SPELL) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_MATTER_SPELL) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_OCCULT_SPELL)) {
			P_SKILL(P_OCCULT_SPELL) = P_UNSKILLED;
			P_ADVANCE(P_OCCULT_SPELL) = 0;
			P_MAX_SKILL(P_OCCULT_SPELL) = P_BASIC;
		} else {
			P_SKILL(P_OCCULT_SPELL) = P_BASIC;
			if (P_MAX_SKILL(P_OCCULT_SPELL) == P_EXPERT) P_MAX_SKILL(P_OCCULT_SPELL) = P_MASTER;
			else if (P_MAX_SKILL(P_OCCULT_SPELL) == P_MASTER) P_MAX_SKILL(P_OCCULT_SPELL) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_OCCULT_SPELL) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_ELEMENTAL_SPELL)) {
			P_SKILL(P_ELEMENTAL_SPELL) = P_UNSKILLED;
			P_ADVANCE(P_ELEMENTAL_SPELL) = 0;
			P_MAX_SKILL(P_ELEMENTAL_SPELL) = P_BASIC;
		} else {
			P_SKILL(P_ELEMENTAL_SPELL) = P_BASIC;
			if (P_MAX_SKILL(P_ELEMENTAL_SPELL) == P_EXPERT) P_MAX_SKILL(P_ELEMENTAL_SPELL) = P_MASTER;
			else if (P_MAX_SKILL(P_ELEMENTAL_SPELL) == P_MASTER) P_MAX_SKILL(P_ELEMENTAL_SPELL) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_ELEMENTAL_SPELL) = P_SUPREME_MASTER;
		}
		if (P_RESTRICTED(P_CHAOS_SPELL)) {
			P_SKILL(P_CHAOS_SPELL) = P_UNSKILLED;
			P_ADVANCE(P_CHAOS_SPELL) = 0;
			P_MAX_SKILL(P_CHAOS_SPELL) = P_BASIC;
		} else {
			P_SKILL(P_CHAOS_SPELL) = P_BASIC;
			if (P_MAX_SKILL(P_CHAOS_SPELL) == P_EXPERT) P_MAX_SKILL(P_CHAOS_SPELL) = P_MASTER;
			else if (P_MAX_SKILL(P_CHAOS_SPELL) == P_MASTER) P_MAX_SKILL(P_CHAOS_SPELL) = P_GRAND_MASTER;
			else P_MAX_SKILL(P_CHAOS_SPELL) = P_SUPREME_MASTER;
		}

		P_SKILL(P_HIGH_HEELS) = P_SKILLED;
		P_ADVANCE(P_HIGH_HEELS) = 160;
		P_MAX_SKILL(P_HIGH_HEELS) = P_SUPREME_MASTER;

	}

	/* navi is good with high heels --Amy */
	if (Race_if(PM_NAVI)) {
		if (P_RESTRICTED(P_HIGH_HEELS)) {	
			P_SKILL(P_HIGH_HEELS) = P_UNSKILLED;
			P_ADVANCE(P_HIGH_HEELS) = 0;
		}
		if (P_MAX_SKILL(P_HIGH_HEELS) < P_EXPERT) P_MAX_SKILL(P_HIGH_HEELS) = P_EXPERT;
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
	if (urole.petnum == PM_PONY)
	    P_SKILL(P_RIDING) = P_BASIC;

	/*
	 * Make sure we haven't missed setting the max on a skill
	 * & set advance
	 */
	for (skill = 0; skill < P_NUM_SKILLS; skill++) {
	    if (!P_RESTRICTED(skill)) {
		if (P_MAX_SKILL(skill) < P_SKILL(skill)) {
		    impossible("skill_init: curr > max: %s", wpskillname(skill));
		    P_MAX_SKILL(skill) = P_SKILL(skill);
		}
		if (!Role_if(PM_DEMAGOGUE)) P_ADVANCE(skill) = practice_needed_to_advance(P_SKILL(skill)-1,skill);
	    }
	}

	if (Race_if(PM_SKILLOR)) {

		int skillimprove = randomgoodskill();

		if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
			unrestrict_weapon_skill(skillimprove);
			P_MAX_SKILL(skillimprove) = P_BASIC;
		} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
			unrestrict_weapon_skill(skillimprove);
			P_MAX_SKILL(skillimprove) = P_BASIC;
		} else if (P_MAX_SKILL(skillimprove) == P_BASIC) {
			P_MAX_SKILL(skillimprove) = P_SKILLED;
		} else if (P_MAX_SKILL(skillimprove) == P_SKILLED) {
			P_MAX_SKILL(skillimprove) = P_EXPERT;
		} else if (P_MAX_SKILL(skillimprove) == P_EXPERT) {
			P_MAX_SKILL(skillimprove) = P_MASTER;
		} else if (P_MAX_SKILL(skillimprove) == P_MASTER) {
			P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
		} else if (P_MAX_SKILL(skillimprove) == P_GRAND_MASTER) {
			P_MAX_SKILL(skillimprove) = P_SUPREME_MASTER;
		}

	}

	/* if you somehow got the other gender's skill, deactivate it now --Amy */
	if (!Role_if(PM_GENDERSTARIST) && flags.female && P_MAX_SKILL(P_GUN_CONTROL) >= P_BASIC) {
		P_MAX_SKILL(P_GUN_CONTROL) = P_ISRESTRICTED;
		P_SKILL(P_GUN_CONTROL) = P_ISRESTRICTED;
		P_ADVANCE(P_GUN_CONTROL) = 0;
	}
	if (!Role_if(PM_GENDERSTARIST) && !flags.female && P_MAX_SKILL(P_SQUEAKING) >= P_BASIC) {
		P_MAX_SKILL(P_SQUEAKING) = P_ISRESTRICTED;
		P_SKILL(P_SQUEAKING) = P_ISRESTRICTED;
		P_ADVANCE(P_SQUEAKING) = 0;
	}
	/* or if you somehow got the other alignment's skill */
	if (u.ualign.type != A_CHAOTIC && P_MAX_SKILL(P_GRINDER) >= P_BASIC) {
		P_MAX_SKILL(P_GRINDER) = P_ISRESTRICTED;
		P_SKILL(P_GRINDER) = P_ISRESTRICTED;
		P_ADVANCE(P_GRINDER) = 0;
	}
	if (u.ualign.type != A_NEUTRAL && P_MAX_SKILL(P_CLAW) >= P_BASIC) {
		P_MAX_SKILL(P_CLAW) = P_ISRESTRICTED;
		P_SKILL(P_CLAW) = P_ISRESTRICTED;
		P_ADVANCE(P_CLAW) = 0;
	}
	if (u.ualign.type != A_LAWFUL && P_MAX_SKILL(P_ORB) >= P_BASIC) {
		P_MAX_SKILL(P_ORB) = P_ISRESTRICTED;
		P_SKILL(P_ORB) = P_ISRESTRICTED;
		P_ADVANCE(P_ORB) = 0;
	}

	/* If you somehow start with a skill at high enough proficiency, learn the associated techniques
	 * otherwise you might be unable to get the tech at all --Amy */

	if (!Role_if(PM_DEMAGOGUE)) {

	for (i = P_FIRST_WEAPON; i <= P_LAST_WEAPON; i++) {
		if (!tech_known(T_DISARM) && (P_SKILL(i) >= P_SKILLED) && i <= P_LAST_WEAPON && i != P_WHIP) {
			learntech(T_DISARM, FROMOUTSIDE, 1);
		}
	}

	if (P_SKILL(P_TRIDENT) >= P_MASTER && !tech_known(T_SILENT_OCEAN)) {
	    	learntech(T_SILENT_OCEAN, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_UNICORN_HORN) >= P_EXPERT && !tech_known(T_GLOWHORN)) {
	    	learntech(T_GLOWHORN, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_UNICORN_HORN) >= P_GRAND_MASTER && !tech_known(T_STAT_RESIST)) {
	    	learntech(T_STAT_RESIST, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_CHAOS_SPELL) >= P_MASTER && !tech_known(T_INTRINSIC_ROULETTE)) {
	    	learntech(T_INTRINSIC_ROULETTE, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_ELEMENTAL_SPELL) >= P_MASTER && !tech_known(T_SPECTRAL_SWORD)) {
	    	learntech(T_SPECTRAL_SWORD, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_OCCULT_SPELL) >= P_MASTER && !tech_known(T_REVERSE_IDENTIFY)) {
	    	learntech(T_REVERSE_IDENTIFY, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_SEARCHING) >= P_MASTER && !tech_known(T_DETECT_TRAPS)) {
	    	learntech(T_DETECT_TRAPS, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_PETKEEPING) >= P_SKILLED && !tech_known(T_DIRECTIVE)) {
	    	learntech(T_DIRECTIVE, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_MEMORIZATION) >= P_MASTER && !tech_known(T_WONDER_YONDER)) {
	    	learntech(T_WONDER_YONDER, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_TWO_WEAPON_COMBAT) >= P_EXPERT && !tech_known(T_SWAP_WEAPON)) {
	    	learntech(T_SWAP_WEAPON, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_IMPLANTS) >= P_MASTER && !tech_known(T_REMOVE_IMPLANT)) {
	    	learntech(T_REMOVE_IMPLANT, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_IMPLANTS) >= P_GRAND_MASTER && !tech_known(T_REROLL_IMPLANT)) {
	    	learntech(T_REROLL_IMPLANT, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_TECHNIQUES) >= P_MASTER && !tech_known(T_TIME_STOP)) {
	    	learntech(T_TIME_STOP, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_LANCE) >= P_MASTER && !tech_known(T_MILDEN_CURSE)) {
	    	learntech(T_MILDEN_CURSE, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_POLEARMS) >= P_MASTER && !tech_known(T_FORCE_FIELD)) {
	    	learntech(T_FORCE_FIELD, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_POLEARMS) >= P_GRAND_MASTER && !tech_known(T_POINTINESS)) {
	    	learntech(T_POINTINESS, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_PADDLE) >= P_MASTER && !tech_known(T_BUG_SPRAY)) {
	    	learntech(T_BUG_SPRAY, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_QUARTERSTAFF) >= P_MASTER && !tech_known(T_WHIRLSTAFF)) {
	    	learntech(T_WHIRLSTAFF, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_CROSSBOW) >= P_MASTER && !tech_known(T_DELIBERATE_CURSE)) {
	    	learntech(T_DELIBERATE_CURSE, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_RIDING) >= P_MASTER && !tech_known(T_ACQUIRE_STEED)) {
	    	learntech(T_ACQUIRE_STEED, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_RIDING) >= P_GRAND_MASTER && !tech_known(T_SADDLING)) {
	    	learntech(T_SADDLING, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_HIGH_HEELS) >= P_EXPERT && !tech_known(T_SHOPPING_QUEEN)) {
	    	learntech(T_SHOPPING_QUEEN, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_HIGH_HEELS) >= P_MASTER && !tech_known(T_BEAUTY_CHARM)) {
	    	learntech(T_BEAUTY_CHARM, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_HIGH_HEELS) >= P_GRAND_MASTER && !tech_known(T_ASIAN_KICK)) {
	    	learntech(T_ASIAN_KICK, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_HIGH_HEELS) >= P_GRAND_MASTER && !tech_known(T_LEGSCRATCH_ATTACK)) {
	    	learntech(T_LEGSCRATCH_ATTACK, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_HIGH_HEELS) >= P_GRAND_MASTER && !tech_known(T_GROUND_STOMP)) {
	    	learntech(T_GROUND_STOMP, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_HIGH_HEELS) >= P_GRAND_MASTER && !tech_known(T_ATHLETIC_COMBAT)) {
	    	learntech(T_ATHLETIC_COMBAT, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_SPIRITUALITY) >= P_GRAND_MASTER && !tech_known(T_PRAYING_SUCCESS)) {
	    	learntech(T_PRAYING_SUCCESS, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_DEVICES) >= P_MASTER && !tech_known(T_OVER_RAY)) {
	    	learntech(T_OVER_RAY, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_BOOMERANG) >= P_EXPERT && !tech_known(T_ENCHANTERANG)) {
	    	learntech(T_ENCHANTERANG, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_BOOMERANG) >= P_MASTER && !tech_known(T_BATMAN_ARSENAL)) {
	    	learntech(T_BATMAN_ARSENAL, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_BOOMERANG) >= P_GRAND_MASTER && !tech_known(T_JOKERBANE)) {
	    	learntech(T_JOKERBANE, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_WHIP) >= P_EXPERT && !tech_known(T_CALL_THE_POLICE)) {
	    	learntech(T_CALL_THE_POLICE, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_WHIP) >= P_MASTER && !tech_known(T_DOMINATE)) {
	    	learntech(T_DOMINATE, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_WHIP) >= P_GRAND_MASTER && !tech_known(T_INCARNATION)) {
	    	learntech(T_INCARNATION, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_MARTIAL_ARTS) >= P_GRAND_MASTER && !tech_known(T_COMBO_STRIKE)) {
	    	learntech(T_COMBO_STRIKE, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_POLYMORPHING) >= P_MASTER && !tech_known(T_FUNGOISM)) {
	    	learntech(T_FUNGOISM, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_POLYMORPHING) >= P_GRAND_MASTER && !tech_known(T_BECOME_UNDEAD)) {
	    	learntech(T_BECOME_UNDEAD, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_BARE_HANDED_COMBAT) >= P_MASTER && !tech_known(T_JIU_JITSU)) {
	    	learntech(T_JIU_JITSU, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_SHURIKEN) >= P_MASTER && !tech_known(T_BLADE_ANGER)) {
	    	learntech(T_BLADE_ANGER, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_PETKEEPING) >= P_EXPERT && !tech_known(T_RE_TAMING)) {
	    	learntech(T_RE_TAMING, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_SHII_CHO) >= P_MASTER && !tech_known(T_UNCURSE_SABER)) {
	    	learntech(T_UNCURSE_SABER, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_MAKASHI) >= P_SKILLED && !tech_known(T_ENERGY_CONSERVATION)) {
	    	learntech(T_ENERGY_CONSERVATION, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_SORESU) >= P_MASTER && !tech_known(T_ENCHANT_ROBE)) {
	    	learntech(T_ENCHANT_ROBE, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_ATARU) >= P_EXPERT && !tech_known(T_WILD_SLASHING)) {
	    	learntech(T_WILD_SLASHING, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_SHIEN) >= P_EXPERT && !tech_known(T_ABSORBER_SHIELD)) {
	    	learntech(T_ABSORBER_SHIELD, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_DJEM_SO) >= P_EXPERT && !tech_known(T_PSYCHO_FORCE)) {
	    	learntech(T_PSYCHO_FORCE, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_NIMAN) >= P_MASTER && !tech_known(T_INTENSIVE_TRAINING)) {
	    	learntech(T_INTENSIVE_TRAINING, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_JUYO) >= P_EXPERT && !tech_known(T_SURRENDER_OR_DIE)) {
	    	learntech(T_SURRENDER_OR_DIE, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_VAAPAD) >= P_EXPERT && !tech_known(T_PERILOUS_WHIRL)) {
	    	learntech(T_PERILOUS_WHIRL, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_PETKEEPING) >= P_MASTER && P_SKILL(P_HIGH_HEELS) >= P_EXPERT && !tech_known(T_SUMMON_SHOE)) {
	    	learntech(T_SUMMON_SHOE, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_HIGH_HEELS) >= P_MASTER && P_SKILL(P_PETKEEPING) >= P_EXPERT && !tech_known(T_SUMMON_SHOE)) {
	    	learntech(T_SUMMON_SHOE, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_SEXY_FLATS) >= P_EXPERT && !tech_known(T_KICK_IN_THE_NUTS)) {
	    	learntech(T_KICK_IN_THE_NUTS, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_SEXY_FLATS) >= P_MASTER && !tech_known(T_DISARMING_KICK)) {
	    	learntech(T_DISARMING_KICK, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_SEXY_FLATS) >= P_GRAND_MASTER && !tech_known(T_INLAY_WARFARE)) {
	    	learntech(T_INLAY_WARFARE, FROMOUTSIDE, 1);
	}

	if (P_SKILL(P_SHII_CHO) >= P_EXPERT && !tech_known(T_STEADY_HAND)) {
	    	learntech(T_STEADY_HAND, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_MAKASHI) >= P_MASTER && !tech_known(T_FORCE_FILLING)) {
	    	learntech(T_FORCE_FILLING, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_SORESU) >= P_GRAND_MASTER && !tech_known(T_JEDI_TAILORING)) {
	    	learntech(T_JEDI_TAILORING, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_ATARU) >= P_SKILLED && !tech_known(T_INTRINSIC_SACRIFICE)) {
	    	learntech(T_INTRINSIC_SACRIFICE, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_DJEM_SO) >= P_MASTER && !tech_known(T_BEAMSWORD)) {
	    	learntech(T_BEAMSWORD, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_NIMAN) >= P_GRAND_MASTER && !tech_known(T_ENERGY_TRANSFER)) {
	    	learntech(T_ENERGY_TRANSFER, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_JUYO) >= P_MASTER && !tech_known(T_SOFTEN_TARGET)) {
	    	learntech(T_SOFTEN_TARGET, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_VAAPAD) >= P_MASTER && !tech_known(T_PROTECT_WEAPON)) {
	    	learntech(T_PROTECT_WEAPON, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_SHIEN) >= P_MASTER && !tech_known(T_POWERFUL_AURA)) {
	    	learntech(T_POWERFUL_AURA, FROMOUTSIDE, 1);
	}

	if (P_SKILL(P_NIMAN) >= P_EXPERT && P_SKILL(P_QUARTERSTAFF) >= P_EXPERT && !tech_known(T_BOOSTAFF)) {
	    	learntech(T_BOOSTAFF, FROMOUTSIDE, 1);
	}

	if (P_SKILL(P_MAKASHI) >= P_MASTER && P_SKILL(P_JAVELIN) >= P_MASTER && !tech_known(T_CLONE_JAVELIN)) {
	    	learntech(T_CLONE_JAVELIN, FROMOUTSIDE, 1);
	}

	if (P_SKILL(P_MACE) >= P_EXPERT && P_SKILL(P_SHII_CHO) >= P_EXPERT && !tech_known(T_REFUGE)) {
	    	learntech(T_REFUGE, FROMOUTSIDE, 1);
	}

	if (P_SKILL(P_SORESU) >= P_EXPERT && P_SKILL(P_MARTIAL_ARTS) >= P_MASTER && !tech_known(T_DRAINING_PUNCH)) {
	    	learntech(T_DRAINING_PUNCH, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_MARTIAL_ARTS) >= P_EXPERT && P_SKILL(P_SORESU) >= P_MASTER && !tech_known(T_DRAINING_PUNCH)) {
	    	learntech(T_DRAINING_PUNCH, FROMOUTSIDE, 1);
	}

	if (P_SKILL(P_SORESU) >= P_EXPERT && P_SKILL(P_BARE_HANDED_COMBAT) >= P_EXPERT && !tech_known(T_ESCROBISM)) {
	    	learntech(T_ESCROBISM, FROMOUTSIDE, 1);
	}

	if (P_SKILL(P_ATARU) >= P_SKILLED && P_SKILL(P_SCIMITAR) >= P_SKILLED && !tech_known(T_PIRATE_BROTHERING)) {
	    	learntech(T_PIRATE_BROTHERING, FROMOUTSIDE, 1);
	}

	if (P_SKILL(P_DJEM_SO) >= P_SKILLED && P_SKILL(P_CROSSBOW) >= P_EXPERT && !tech_known(T_NUTS_AND_BOLTS)) {
	    	learntech(T_NUTS_AND_BOLTS, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_CROSSBOW) >= P_SKILLED && P_SKILL(P_DJEM_SO) >= P_EXPERT && !tech_known(T_NUTS_AND_BOLTS)) {
	    	learntech(T_NUTS_AND_BOLTS, FROMOUTSIDE, 1);
	}

	if (P_SKILL(P_JUYO) >= P_EXPERT && P_SKILL(P_POLEARMS) >= P_MASTER && !tech_known(T_DECAPABILITY)) {
	    	learntech(T_DECAPABILITY, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_POLEARMS) >= P_EXPERT && P_SKILL(P_JUYO) >= P_MASTER && !tech_known(T_DECAPABILITY)) {
	    	learntech(T_DECAPABILITY, FROMOUTSIDE, 1);
	}

	if (P_SKILL(P_VAAPAD) >= P_EXPERT && P_SKILL(P_TWO_HANDED_SWORD) >= P_EXPERT && !tech_known(T_NO_HANDS_CURSE)) {
	    	learntech(T_NO_HANDS_CURSE, FROMOUTSIDE, 1);
	}

	if (P_SKILL(P_SEXY_FLATS) >= P_EXPERT && P_SKILL(P_HIGH_HEELS) >= P_EXPERT && !tech_known(T_HIGH_HEELED_SNEAKERS)) {
	    	learntech(T_HIGH_HEELED_SNEAKERS, FROMOUTSIDE, 1);
	}

	if (P_SKILL(P_GUN_CONTROL) >= P_BASIC && !tech_known(T_SHOTTY_BLAST)) {
	    	learntech(T_SHOTTY_BLAST, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_GUN_CONTROL) >= P_SKILLED && !tech_known(T_AMMO_UPGRADE)) {
	    	learntech(T_AMMO_UPGRADE, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_GUN_CONTROL) >= P_EXPERT && !tech_known(T_LASER_POWER)) {
	    	learntech(T_LASER_POWER, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_GUN_CONTROL) >= P_MASTER && !tech_known(T_BIG_DADDY)) {
	    	learntech(T_BIG_DADDY, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_GUN_CONTROL) >= P_GRAND_MASTER && !tech_known(T_SHUT_THAT_BITCH_UP)) {
	    	learntech(T_SHUT_THAT_BITCH_UP, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_SQUEAKING) >= P_BASIC && !tech_known(T_S_PRESSING)) {
	    	learntech(T_S_PRESSING, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_SQUEAKING) >= P_SKILLED && !tech_known(T_MELTEE)) {
	    	learntech(T_MELTEE, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_SQUEAKING) >= P_EXPERT && !tech_known(T_WOMAN_NOISES)) {
	    	learntech(T_WOMAN_NOISES, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_SQUEAKING) >= P_MASTER && !tech_known(T_EXTRA_LONG_SQUEAK)) {
	    	learntech(T_EXTRA_LONG_SQUEAK, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_SQUEAKING) >= P_GRAND_MASTER && !tech_known(T_SEXUAL_HUG)) {
	    	learntech(T_SEXUAL_HUG, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_GUN_CONTROL) >= P_GRAND_MASTER && P_SKILL(P_SQUEAKING) >= P_GRAND_MASTER && !tech_known(T_SEX_CHANGE)) {
	    	learntech(T_SEX_CHANGE, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_GUN_CONTROL) >= P_MASTER && P_SKILL(P_FIREARM) >= P_MASTER && !tech_known(T_EVEN_MORE_AMMO)) {
	    	learntech(T_EVEN_MORE_AMMO, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_SQUEAKING) >= P_EXPERT && P_SKILL(P_POLYMORPHING) >= P_MASTER && !tech_known(T_DOUBLESELF)) {
	    	learntech(T_DOUBLESELF, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_SQUEAKING) >= P_MASTER && P_SKILL(P_POLYMORPHING) >= P_EXPERT && !tech_known(T_DOUBLESELF)) {
	    	learntech(T_DOUBLESELF, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_CHAOS_SPELL) >= P_EXPERT && P_SKILL(P_POLYMORPHING) >= P_EXPERT && !tech_known(T_POLYFIX)) {
	    	learntech(T_POLYFIX, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_OCCULT_SPELL) >= P_EXPERT && P_SKILL(P_SQUEAKING) >= P_EXPERT && !tech_known(T_SQUEAKY_REPAIR)) {
	    	learntech(T_SQUEAKY_REPAIR, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_OCCULT_SPELL) >= P_EXPERT && P_SKILL(P_GUN_CONTROL) >= P_EXPERT && !tech_known(T_BULLETREUSE)) {
	    	learntech(T_BULLETREUSE, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_ELEMENTAL_SPELL) >= P_EXPERT && P_SKILL(P_DEVICES) >= P_EXPERT && !tech_known(T_EXTRACHARGE)) {
	    	learntech(T_EXTRACHARGE, FROMOUTSIDE, 1);
	}

	if (P_SKILL(P_MAKASHI) >= P_EXPERT && P_SKILL(P_ATARU) >= P_EXPERT && P_SKILL(P_VAAPAD) >= P_EXPERT && !tech_known(T_FORM_CHOICE)) {
	    	learntech(T_FORM_CHOICE, FROMOUTSIDE, 1);
	}

	if (P_SKILL(P_WEDI) >= P_EXPERT && !tech_known(T_STAR_DIGGING)) {
	    	learntech(T_STAR_DIGGING, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_WEDI) >= P_GRAND_MASTER && !tech_known(T_STARWARS_FRIENDS)) {
	    	learntech(T_STARWARS_FRIENDS, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_WEDI) >= P_MASTER && (Role_if(PM_SPACEWARS_FIGHTER) || Role_if(PM_CAMPERSTRIKER) || Role_if(PM_GANG_SCHOLAR) || Role_if(PM_WALSCHOLAR)) && !tech_known(T_STARWARS_FRIENDS)) {
	    	learntech(T_STARWARS_FRIENDS, FROMOUTSIDE, 1);
	}

	if (P_SKILL(P_WEDI) >= P_EXPERT && P_SKILL(P_PICK_AXE) >= P_EXPERT && !tech_known(T_USE_THE_FORCE_LUKE)) {
	    	learntech(T_USE_THE_FORCE_LUKE, FROMOUTSIDE, 1);
	}

	if (P_SKILL(P_SYMBIOSIS) >= P_BASIC && !tech_known(T_SYMBIOSIS)) {
	    	learntech(T_SYMBIOSIS, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_SYMBIOSIS) >= P_SKILLED && !tech_known(T_ADJUST_SYMBIOTE)) {
	    	learntech(T_ADJUST_SYMBIOTE, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_SYMBIOSIS) >= P_EXPERT && !tech_known(T_HEAL_SYMBIOTE)) {
	    	learntech(T_HEAL_SYMBIOTE, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_SYMBIOSIS) >= P_MASTER && !tech_known(T_BOOST_SYMBIOTE)) {
	    	learntech(T_BOOST_SYMBIOTE, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_SYMBIOSIS) >= P_GRAND_MASTER && !tech_known(T_POWERBIOSIS)) {
	    	learntech(T_POWERBIOSIS, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_SHORT_SWORD) >= P_MASTER && !tech_known(T_UNDERTOW)) {
	    	learntech(T_UNDERTOW, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_FIREARM) >= P_MASTER && !tech_known(T_GRENADES_OF_COURSE)) {
	    	learntech(T_GRENADES_OF_COURSE, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_CLUB) >= P_MASTER && !tech_known(T_DEFINALIZE)) {
	    	learntech(T_DEFINALIZE, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_MACE) >= P_GRAND_MASTER && !tech_known(T_ANTI_INERTIA)) {
	    	learntech(T_ANTI_INERTIA, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_SYMBIOSIS) >= P_EXPERT && P_SKILL(P_IMPLANTS) >= P_EXPERT && !tech_known(T_IMPLANTED_SYMBIOSIS)) {
	    	learntech(T_IMPLANTED_SYMBIOSIS, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_SYMBIOSIS) >= P_EXPERT && P_SKILL(P_POLYMORPHING) >= P_EXPERT && !tech_known(T_ASSUME_SYMBIOTE)) {
	    	learntech(T_ASSUME_SYMBIOTE, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_SYMBIOSIS) >= P_MASTER && P_SKILL(P_PETKEEPING) >= P_MASTER && !tech_known(T_GENERATE_OFFSPRING)) {
	    	learntech(T_GENERATE_OFFSPRING, FROMOUTSIDE, 1);
	}

	if (P_SKILL(P_ORB) >= P_BASIC && !tech_known(T_PACIFY)) {
	    	learntech(T_PACIFY, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_ORB) >= P_SKILLED && !tech_known(T_AFTERBURNER)) {
	    	learntech(T_AFTERBURNER, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_ORB) >= P_EXPERT && !tech_known(T_BUGGARD)) {
	    	learntech(T_BUGGARD, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_ORB) >= P_MASTER && !tech_known(T_THUNDERSTORM)) {
	    	learntech(T_THUNDERSTORM, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_ORB) >= P_GRAND_MASTER && !tech_known(T_AUTOKILL)) {
	    	learntech(T_AUTOKILL, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_ORB) >= P_GRAND_MASTER && !tech_known(T_CHAIN_THUNDERBOLT)) {
	    	learntech(T_CHAIN_THUNDERBOLT, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_ORB) >= P_GRAND_MASTER && !tech_known(T_FLASHING_MISCHIEF)) {
	    	learntech(T_FLASHING_MISCHIEF, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_CLAW) >= P_BASIC && !tech_known(T_KAMEHAMEHA)) {
	    	learntech(T_KAMEHAMEHA, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_CLAW) >= P_SKILLED && !tech_known(T_SHADOW_MANTLE)) {
	    	learntech(T_SHADOW_MANTLE, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_CLAW) >= P_EXPERT && !tech_known(T_VACUUM_STAR)) {
	    	learntech(T_VACUUM_STAR, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_CLAW) >= P_MASTER && !tech_known(T_BLADE_SHIELD)) {
	    	learntech(T_BLADE_SHIELD, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_CLAW) >= P_GRAND_MASTER && !tech_known(T_GREEN_WEAPON)) {
	    	learntech(T_GREEN_WEAPON, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_GRINDER) >= P_BASIC && !tech_known(T_BALLSLIFF)) {
	    	learntech(T_BALLSLIFF, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_GRINDER) >= P_SKILLED && !tech_known(T_POLE_MELEE)) {
	    	learntech(T_POLE_MELEE, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_GRINDER) >= P_EXPERT && !tech_known(T_CHOP_CHOP)) {
	    	learntech(T_CHOP_CHOP, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_GRINDER) >= P_MASTER && !tech_known(T_BANISHMENT)) {
	    	learntech(T_BANISHMENT, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_GRINDER) >= P_GRAND_MASTER && !tech_known(T_PARTICIPATION_LOSS)) {
	    	learntech(T_PARTICIPATION_LOSS, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_CLAW) >= P_EXPERT && P_SKILL(P_TWO_WEAPON_COMBAT) >= P_EXPERT && !tech_known(T_WEAPON_BLOCKER)) {
	    	learntech(T_WEAPON_BLOCKER, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_ORB) >= P_EXPERT && P_SKILL(P_MEMORIZATION) >= P_EXPERT && !tech_known(T_EXTRA_MEMORY)) {
	    	learntech(T_EXTRA_MEMORY, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_GRINDER) >= P_EXPERT && P_SKILL(P_LANCE) >= P_EXPERT && !tech_known(T_GRAP_SWAP)) {
	    	learntech(T_GRAP_SWAP, FROMOUTSIDE, 1);
	}
	if (P_SKILL(P_GRINDER) >= P_EXPERT && P_SKILL(P_CLAW) >= P_EXPERT && P_SKILL(P_ORB) >= P_EXPERT && !tech_known(T_DIABOLIC_MINION)) {
	    	learntech(T_DIABOLIC_MINION, FROMOUTSIDE, 1);
	}

	} /* demagogue check */

}

void
xtraskillinit()
{

	struct obj *obj;
	int skill;

	/* Set skill for all objects in inventory to be basic */
	if (!Role_if(PM_POLITICIAN) && !Role_if(PM_WILD_TALENT) && !Role_if(PM_SOCIAL_JUSTICE_WARRIOR) && !isamerican && !Role_if(PM_ANACHRONOUNBINDER) && !Role_if(PM_MURDERER) ) for (obj = invent; obj; obj = obj->nobj) {
	    skill = get_obj_skill(obj, FALSE);
	    if (skill != P_NONE) {
		if(!Role_if(PM_BINDER) && !Role_if(PM_DEMAGOGUE) && !Race_if(PM_BASTARD) && !Race_if(PM_YEEK) ) P_SKILL(skill) = P_BASIC;
		else P_SKILL(skill) = P_UNSKILLED;
		/* KMH -- If you came into the dungeon with it, you should at least be skilled */
		if (P_MAX_SKILL(skill) < P_EXPERT) { /* edit by Amy: let's make it expert. */
			P_MAX_SKILL(skill) = P_EXPERT;
		}
		if (!Role_if(PM_BINDER) && !Role_if(PM_DEMAGOGUE) && !Race_if(PM_BASTARD) && !Race_if(PM_YEEK) && Race_if(PM_TUMBLRER)) P_SKILL(skill) = P_MAX_SKILL(skill);
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
	use_skill(weapon_type(uwep), 5); /* Amy edit: waaaaaay too easy to max out the skill with the SLASH'EM values! */
	return(0);
}

void
doubleskilltraining()
{
	You("may double your amount of training points in a skill of your choice!");

	int acquiredskill;
	acquiredskill = 0;

	pline("Pick a skill to train. The prompt will loop until you actually make a choice.");

	while (acquiredskill == 0) { /* ask the player what they want --Amy */

	if (P_ADVANCE(P_DAGGER) && !(P_RESTRICTED(P_DAGGER)) && yn("Do you want to train the dagger skill?")=='y') {
		P_ADVANCE(P_DAGGER) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_KNIFE) && !(P_RESTRICTED(P_KNIFE)) && yn("Do you want to train the knife skill?")=='y') {
		P_ADVANCE(P_KNIFE) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_AXE) && !(P_RESTRICTED(P_AXE)) && yn("Do you want to train the axe skill?")=='y') {
		P_ADVANCE(P_AXE) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_PICK_AXE) && !(P_RESTRICTED(P_PICK_AXE)) && yn("Do you want to train the pick-axe skill?")=='y') {
		P_ADVANCE(P_PICK_AXE) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_SHORT_SWORD) && !(P_RESTRICTED(P_SHORT_SWORD)) && yn("Do you want to train the short sword skill?")=='y') {
		P_ADVANCE(P_SHORT_SWORD) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_BROAD_SWORD) && !(P_RESTRICTED(P_BROAD_SWORD)) && yn("Do you want to train the broad sword skill?")=='y') {
		P_ADVANCE(P_BROAD_SWORD) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_LONG_SWORD) && !(P_RESTRICTED(P_LONG_SWORD)) && yn("Do you want to train the long sword skill?")=='y') {
		P_ADVANCE(P_LONG_SWORD) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_TWO_HANDED_SWORD) && !(P_RESTRICTED(P_TWO_HANDED_SWORD)) && yn("Do you want to train the two-handed sword skill?")=='y') {
		P_ADVANCE(P_TWO_HANDED_SWORD) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_SCIMITAR) && !(P_RESTRICTED(P_SCIMITAR)) && yn("Do you want to train the scimitar skill?")=='y') {
		P_ADVANCE(P_SCIMITAR) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_SABER) && !(P_RESTRICTED(P_SABER)) && yn("Do you want to train the saber skill?")=='y') {
		P_ADVANCE(P_SABER) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_CLUB) && !(P_RESTRICTED(P_CLUB)) && yn("Do you want to train the club skill?")=='y') {
		P_ADVANCE(P_CLUB) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_PADDLE) && !(P_RESTRICTED(P_PADDLE)) && yn("Do you want to train the paddle skill?")=='y') {
		P_ADVANCE(P_PADDLE) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_MACE) && !(P_RESTRICTED(P_MACE)) && yn("Do you want to train the mace skill?")=='y') {
		P_ADVANCE(P_MACE) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_MORNING_STAR) && !(P_RESTRICTED(P_MORNING_STAR)) && yn("Do you want to train the morning star skill?")=='y') {
		P_ADVANCE(P_MORNING_STAR) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_FLAIL) && !(P_RESTRICTED(P_FLAIL)) && yn("Do you want to train the flail skill?")=='y') {
		P_ADVANCE(P_FLAIL) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_HAMMER) && !(P_RESTRICTED(P_HAMMER)) && yn("Do you want to train the hammer skill?")=='y') {
		P_ADVANCE(P_HAMMER) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_QUARTERSTAFF) && !(P_RESTRICTED(P_QUARTERSTAFF)) && yn("Do you want to train the quarterstaff skill?")=='y') {
		P_ADVANCE(P_QUARTERSTAFF) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_ORB) && !(P_RESTRICTED(P_ORB)) && yn("Do you want to train the orb skill?")=='y') {
		P_ADVANCE(P_ORB) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_CLAW) && !(P_RESTRICTED(P_CLAW)) && yn("Do you want to train the claw skill?")=='y') {
		P_ADVANCE(P_CLAW) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_GRINDER) && !(P_RESTRICTED(P_GRINDER)) && yn("Do you want to train the grinder skill?")=='y') {
		P_ADVANCE(P_GRINDER) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_POLEARMS) && !(P_RESTRICTED(P_POLEARMS)) && yn("Do you want to train the polearms skill?")=='y') {
		P_ADVANCE(P_POLEARMS) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_SPEAR) && !(P_RESTRICTED(P_SPEAR)) && yn("Do you want to train the spear skill?")=='y') {
		P_ADVANCE(P_SPEAR) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_JAVELIN) && !(P_RESTRICTED(P_JAVELIN)) && yn("Do you want to train the javelin skill?")=='y') {
		P_ADVANCE(P_JAVELIN) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_TRIDENT) && !(P_RESTRICTED(P_TRIDENT)) && yn("Do you want to train the trident skill?")=='y') {
		P_ADVANCE(P_TRIDENT) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_LANCE) && !(P_RESTRICTED(P_LANCE)) && yn("Do you want to train the lance skill?")=='y') {
		P_ADVANCE(P_LANCE) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_BOW) && !(P_RESTRICTED(P_BOW)) && yn("Do you want to train the bow skill?")=='y') {
		P_ADVANCE(P_BOW) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_SLING) && !(P_RESTRICTED(P_SLING)) && yn("Do you want to train the sling skill?")=='y') {
		P_ADVANCE(P_SLING) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_FIREARM) && !(P_RESTRICTED(P_FIREARM)) && yn("Do you want to train the firearms skill?")=='y') {
		P_ADVANCE(P_FIREARM) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_CROSSBOW) && !(P_RESTRICTED(P_CROSSBOW)) && yn("Do you want to train the crossbow skill?")=='y') {
		P_ADVANCE(P_CROSSBOW) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_DART) && !(P_RESTRICTED(P_DART)) && yn("Do you want to train the dart skill?")=='y') {
		P_ADVANCE(P_DART) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_SHURIKEN) && !(P_RESTRICTED(P_SHURIKEN)) && yn("Do you want to train the shuriken skill?")=='y') {
		P_ADVANCE(P_SHURIKEN) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_BOOMERANG) && !(P_RESTRICTED(P_BOOMERANG)) && yn("Do you want to train the boomerang skill?")=='y') {
		P_ADVANCE(P_BOOMERANG) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_WHIP) && !(P_RESTRICTED(P_WHIP)) && yn("Do you want to train the whip skill?")=='y') {
		P_ADVANCE(P_WHIP) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_UNICORN_HORN) && !(P_RESTRICTED(P_UNICORN_HORN)) && yn("Do you want to train the unicorn horn skill?")=='y') {
		P_ADVANCE(P_UNICORN_HORN) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_LIGHTSABER) && !(P_RESTRICTED(P_LIGHTSABER)) && yn("Do you want to train the lightsaber skill?")=='y') {
		P_ADVANCE(P_LIGHTSABER) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_ATTACK_SPELL) && !(P_RESTRICTED(P_ATTACK_SPELL)) && yn("Do you want to train the attack spell skill?")=='y') {
		P_ADVANCE(P_ATTACK_SPELL) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_HEALING_SPELL) && !(P_RESTRICTED(P_HEALING_SPELL)) && yn("Do you want to train the healing spell skill?")=='y') {
		P_ADVANCE(P_HEALING_SPELL) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_DIVINATION_SPELL) && !(P_RESTRICTED(P_DIVINATION_SPELL)) && yn("Do you want to train the divination spell skill?")=='y') {
		P_ADVANCE(P_DIVINATION_SPELL) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_ENCHANTMENT_SPELL) && !(P_RESTRICTED(P_ENCHANTMENT_SPELL)) && yn("Do you want to train the enchantment spell skill?")=='y') {
		P_ADVANCE(P_ENCHANTMENT_SPELL) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_PROTECTION_SPELL) && !(P_RESTRICTED(P_PROTECTION_SPELL)) && yn("Do you want to train the protection spell skill?")=='y') {
		P_ADVANCE(P_PROTECTION_SPELL) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_BODY_SPELL) && !(P_RESTRICTED(P_BODY_SPELL)) && yn("Do you want to train the body spell skill?")=='y') {
		P_ADVANCE(P_BODY_SPELL) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_OCCULT_SPELL) && !(P_RESTRICTED(P_OCCULT_SPELL)) && yn("Do you want to train the occult spell skill?")=='y') {
		P_ADVANCE(P_OCCULT_SPELL) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_ELEMENTAL_SPELL) && !(P_RESTRICTED(P_ELEMENTAL_SPELL)) && yn("Do you want to train the elemental spell skill?")=='y') {
		P_ADVANCE(P_ELEMENTAL_SPELL) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_CHAOS_SPELL) && !(P_RESTRICTED(P_CHAOS_SPELL)) && yn("Do you want to train the chaos spell skill?")=='y') {
		P_ADVANCE(P_CHAOS_SPELL) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_MATTER_SPELL) && !(P_RESTRICTED(P_MATTER_SPELL)) && yn("Do you want to train the matter spell skill?")=='y') {
		P_ADVANCE(P_MATTER_SPELL) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_RIDING) && !(P_RESTRICTED(P_RIDING)) && yn("Do you want to train the riding skill?")=='y') {
		P_ADVANCE(P_RIDING) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_HIGH_HEELS) && !(P_RESTRICTED(P_HIGH_HEELS)) && yn("Do you want to train the high heels skill?")=='y') {
		P_ADVANCE(P_HIGH_HEELS) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_GENERAL_COMBAT) && !(P_RESTRICTED(P_GENERAL_COMBAT)) && yn("Do you want to train the general combat skill?")=='y') {
		P_ADVANCE(P_GENERAL_COMBAT) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_SHIELD) && !(P_RESTRICTED(P_SHIELD)) && yn("Do you want to train the shield skill?")=='y') {
		P_ADVANCE(P_SHIELD) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_BODY_ARMOR) && !(P_RESTRICTED(P_BODY_ARMOR)) && yn("Do you want to train the body armor skill?")=='y') {
		P_ADVANCE(P_BODY_ARMOR) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_TWO_HANDED_WEAPON) && !(P_RESTRICTED(P_TWO_HANDED_WEAPON)) && yn("Do you want to train the two-handed weapon skill?")=='y') {
		P_ADVANCE(P_TWO_HANDED_WEAPON) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_TWO_WEAPON_COMBAT) && !(P_RESTRICTED(P_TWO_WEAPON_COMBAT)) && yn("Do you want to train the two-weapon combat skill?")=='y') {
		P_ADVANCE(P_TWO_WEAPON_COMBAT) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_POLYMORPHING) && !(P_RESTRICTED(P_POLYMORPHING)) && yn("Do you want to train the polymorphing skill?")=='y') {
		P_ADVANCE(P_POLYMORPHING) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_DEVICES) && !(P_RESTRICTED(P_DEVICES)) && yn("Do you want to train the devices skill?")=='y') {
		P_ADVANCE(P_DEVICES) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_SEARCHING) && !(P_RESTRICTED(P_SEARCHING)) && yn("Do you want to train the searching skill?")=='y') {
		P_ADVANCE(P_SEARCHING) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_SPIRITUALITY) && !(P_RESTRICTED(P_SPIRITUALITY)) && yn("Do you want to train the spirituality skill?")=='y') {
		P_ADVANCE(P_SPIRITUALITY) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_PETKEEPING) && !(P_RESTRICTED(P_PETKEEPING)) && yn("Do you want to train the petkeeping skill?")=='y') {
		P_ADVANCE(P_PETKEEPING) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_MISSILE_WEAPONS) && !(P_RESTRICTED(P_MISSILE_WEAPONS)) && yn("Do you want to train the missile weapons skill?")=='y') {
		P_ADVANCE(P_MISSILE_WEAPONS) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_TECHNIQUES) && !(P_RESTRICTED(P_TECHNIQUES)) && yn("Do you want to train the techniques skill?")=='y') {
		P_ADVANCE(P_TECHNIQUES) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_IMPLANTS) && !(P_RESTRICTED(P_IMPLANTS)) && yn("Do you want to train the implants skill?")=='y') {
		P_ADVANCE(P_IMPLANTS) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_SEXY_FLATS) && !(P_RESTRICTED(P_SEXY_FLATS)) && yn("Do you want to train the sexy flats skill?")=='y') {
		P_ADVANCE(P_SEXY_FLATS) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_MEMORIZATION) && !(P_RESTRICTED(P_MEMORIZATION)) && yn("Do you want to train the memorization skill?")=='y') {
		P_ADVANCE(P_MEMORIZATION) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_SQUEAKING) && !(P_RESTRICTED(P_SQUEAKING)) && yn("Do you want to train the squeaking skill?")=='y') {
		P_ADVANCE(P_SQUEAKING) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_SYMBIOSIS) && !(P_RESTRICTED(P_SYMBIOSIS)) && yn("Do you want to train the symbiosis skill?")=='y') {
		P_ADVANCE(P_SYMBIOSIS) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_GUN_CONTROL) && !(P_RESTRICTED(P_GUN_CONTROL)) && yn("Do you want to train the gun control skill?")=='y') {
		P_ADVANCE(P_GUN_CONTROL) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_SHII_CHO) && !(P_RESTRICTED(P_SHII_CHO)) && yn("Do you want to train the form I (Shii-Cho) skill?")=='y') {
		P_ADVANCE(P_SHII_CHO) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_MAKASHI) && !(P_RESTRICTED(P_MAKASHI)) && yn("Do you want to train the form II (Makashi) skill?")=='y') {
		P_ADVANCE(P_MAKASHI) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_SORESU) && !(P_RESTRICTED(P_SORESU)) && yn("Do you want to train the form III (Soresu) skill?")=='y') {
		P_ADVANCE(P_SORESU) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_ATARU) && !(P_RESTRICTED(P_ATARU)) && yn("Do you want to train the form IV (Ataru) skill?")=='y') {
		P_ADVANCE(P_ATARU) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_SHIEN) && !(P_RESTRICTED(P_SHIEN)) && yn("Do you want to train the form V (Shien) skill?")=='y') {
		P_ADVANCE(P_SHIEN) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_DJEM_SO) && !(P_RESTRICTED(P_DJEM_SO)) && yn("Do you want to train the form V (Djem So) skill?")=='y') {
		P_ADVANCE(P_DJEM_SO) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_NIMAN) && !(P_RESTRICTED(P_NIMAN)) && yn("Do you want to train the form VI (Niman) skill?")=='y') {
		P_ADVANCE(P_NIMAN) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_JUYO) && !(P_RESTRICTED(P_JUYO)) && yn("Do you want to train the form VII (Juyo) skill?")=='y') {
		P_ADVANCE(P_JUYO) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_VAAPAD) && !(P_RESTRICTED(P_VAAPAD)) && yn("Do you want to train the form VII (Vaapad) skill?")=='y') {
		P_ADVANCE(P_VAAPAD) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_WEDI) && !(P_RESTRICTED(P_WEDI)) && yn("Do you want to train the form VIII (Wedi) skill?")=='y') {
		P_ADVANCE(P_WEDI) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_BARE_HANDED_COMBAT) && !(P_RESTRICTED(P_BARE_HANDED_COMBAT)) && yn("Do you want to train the bare-handed combat skill?")=='y') {
		P_ADVANCE(P_BARE_HANDED_COMBAT) *= 2;
		acquiredskill = 1; }
	else if (P_ADVANCE(P_MARTIAL_ARTS) && !(P_RESTRICTED(P_MARTIAL_ARTS)) && yn("Do you want to train the martial arts skill?")=='y') {
		P_ADVANCE(P_MARTIAL_ARTS) *= 2;
		acquiredskill = 1; }
	else if (yn("Do you want to train no skill at all?")=='y') {
		acquiredskill = 1; }
	}
	pline("Training complete!");

}

void
unrestrictskillchoice()
{
	int acquiredskill;
	acquiredskill = 0;

	pline("Pick a skill to unrestrict. The prompt will loop until you actually make a choice.");

	while (acquiredskill == 0) { /* ask the player what they want --Amy */

	if (P_RESTRICTED(P_DAGGER) && yn("Do you want to learn the dagger skill?")=='y') {
		    unrestrict_weapon_skill(P_DAGGER);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_KNIFE) && yn("Do you want to learn the knife skill?")=='y') {
		    unrestrict_weapon_skill(P_KNIFE);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_AXE) && yn("Do you want to learn the axe skill?")=='y') {
		    unrestrict_weapon_skill(P_AXE);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_PICK_AXE) && yn("Do you want to learn the pick-axe skill?")=='y') {
		    unrestrict_weapon_skill(P_PICK_AXE);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_SHORT_SWORD) && yn("Do you want to learn the short sword skill?")=='y') {
		    unrestrict_weapon_skill(P_SHORT_SWORD);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_BROAD_SWORD) && yn("Do you want to learn the broad sword skill?")=='y') {
		    unrestrict_weapon_skill(P_BROAD_SWORD);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_LONG_SWORD) && yn("Do you want to learn the long sword skill?")=='y') {
		    unrestrict_weapon_skill(P_LONG_SWORD);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_TWO_HANDED_SWORD) && yn("Do you want to learn the two-handed sword skill?")=='y') {
		    unrestrict_weapon_skill(P_TWO_HANDED_SWORD);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_SCIMITAR) && yn("Do you want to learn the scimitar skill?")=='y') {
		    unrestrict_weapon_skill(P_SCIMITAR);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_SABER) && yn("Do you want to learn the saber skill?")=='y') {
		    unrestrict_weapon_skill(P_SABER);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_CLUB) && yn("Do you want to learn the club skill?")=='y') {
		    unrestrict_weapon_skill(P_CLUB);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_PADDLE) && yn("Do you want to learn the paddle skill?")=='y') {
		    unrestrict_weapon_skill(P_PADDLE);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_MACE) && yn("Do you want to learn the mace skill?")=='y') {
		    unrestrict_weapon_skill(P_MACE);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_MORNING_STAR) && yn("Do you want to learn the morning star skill?")=='y') {
		    unrestrict_weapon_skill(P_MORNING_STAR);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_FLAIL) && yn("Do you want to learn the flail skill?")=='y') {
		    unrestrict_weapon_skill(P_FLAIL);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_HAMMER) && yn("Do you want to learn the hammer skill?")=='y') {
		    unrestrict_weapon_skill(P_HAMMER);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_QUARTERSTAFF) && yn("Do you want to learn the quarterstaff skill?")=='y') {
		    unrestrict_weapon_skill(P_QUARTERSTAFF);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_ORB) && yn("Do you want to learn the orb skill?")=='y') {
		    unrestrict_weapon_skill(P_ORB);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_CLAW) && yn("Do you want to learn the claw skill?")=='y') {
		    unrestrict_weapon_skill(P_CLAW);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_GRINDER) && yn("Do you want to learn the grinder skill?")=='y') {
		    unrestrict_weapon_skill(P_GRINDER);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_POLEARMS) && yn("Do you want to learn the polearms skill?")=='y') {
		    unrestrict_weapon_skill(P_POLEARMS);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_SPEAR) && yn("Do you want to learn the spear skill?")=='y') {
		    unrestrict_weapon_skill(P_SPEAR);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_JAVELIN) && yn("Do you want to learn the javelin skill?")=='y') {
		    unrestrict_weapon_skill(P_JAVELIN);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_TRIDENT) && yn("Do you want to learn the trident skill?")=='y') {
		    unrestrict_weapon_skill(P_TRIDENT);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_LANCE) && yn("Do you want to learn the lance skill?")=='y') {
		    unrestrict_weapon_skill(P_LANCE);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_BOW) && yn("Do you want to learn the bow skill?")=='y') {
		    unrestrict_weapon_skill(P_BOW);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_SLING) && yn("Do you want to learn the sling skill?")=='y') {
		    unrestrict_weapon_skill(P_SLING);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_FIREARM) && yn("Do you want to learn the firearms skill?")=='y') {
		    unrestrict_weapon_skill(P_FIREARM);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_CROSSBOW) && yn("Do you want to learn the crossbow skill?")=='y') {
		    unrestrict_weapon_skill(P_CROSSBOW);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_DART) && yn("Do you want to learn the dart skill?")=='y') {
		    unrestrict_weapon_skill(P_DART);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_SHURIKEN) && yn("Do you want to learn the shuriken skill?")=='y') {
		    unrestrict_weapon_skill(P_SHURIKEN);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_BOOMERANG) && yn("Do you want to learn the boomerang skill?")=='y') {
		    unrestrict_weapon_skill(P_BOOMERANG);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_WHIP) && yn("Do you want to learn the whip skill?")=='y') {
		    unrestrict_weapon_skill(P_WHIP);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_UNICORN_HORN) && yn("Do you want to learn the unicorn horn skill?")=='y') {
		    unrestrict_weapon_skill(P_UNICORN_HORN);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_LIGHTSABER) && yn("Do you want to learn the lightsaber skill?")=='y') {
		    unrestrict_weapon_skill(P_LIGHTSABER);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_ATTACK_SPELL) && yn("Do you want to learn the attack spell skill?")=='y') {
		    unrestrict_weapon_skill(P_ATTACK_SPELL);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_HEALING_SPELL) && yn("Do you want to learn the healing spell skill?")=='y') {
		    unrestrict_weapon_skill(P_HEALING_SPELL);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_DIVINATION_SPELL) && yn("Do you want to learn the divination spell skill?")=='y') {
		    unrestrict_weapon_skill(P_DIVINATION_SPELL);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_ENCHANTMENT_SPELL) && yn("Do you want to learn the enchantment spell skill?")=='y') {
		    unrestrict_weapon_skill(P_ENCHANTMENT_SPELL);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_PROTECTION_SPELL) && yn("Do you want to learn the protection spell skill?")=='y') {
		    unrestrict_weapon_skill(P_PROTECTION_SPELL);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_BODY_SPELL) && yn("Do you want to learn the body spell skill?")=='y') {
		    unrestrict_weapon_skill(P_BODY_SPELL);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_OCCULT_SPELL) && yn("Do you want to learn the occult spell skill?")=='y') {
		    unrestrict_weapon_skill(P_OCCULT_SPELL);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_ELEMENTAL_SPELL) && yn("Do you want to learn the elemental spell skill?")=='y') {
		    unrestrict_weapon_skill(P_ELEMENTAL_SPELL);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_CHAOS_SPELL) && yn("Do you want to learn the chaos spell skill?")=='y') {
		    unrestrict_weapon_skill(P_CHAOS_SPELL);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_MATTER_SPELL) && yn("Do you want to learn the matter spell skill?")=='y') {
		    unrestrict_weapon_skill(P_MATTER_SPELL);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_RIDING) && yn("Do you want to learn the riding skill?")=='y') {
		    unrestrict_weapon_skill(P_RIDING);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_HIGH_HEELS) && yn("Do you want to learn the high heels skill?")=='y') {
		    unrestrict_weapon_skill(P_HIGH_HEELS);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_GENERAL_COMBAT) && yn("Do you want to learn the general combat skill?")=='y') {
		    unrestrict_weapon_skill(P_GENERAL_COMBAT);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_SHIELD) && yn("Do you want to learn the shield skill?")=='y') {
		    unrestrict_weapon_skill(P_SHIELD);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_BODY_ARMOR) && yn("Do you want to learn the body armor skill?")=='y') {
		    unrestrict_weapon_skill(P_BODY_ARMOR);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_TWO_HANDED_WEAPON) && yn("Do you want to learn the two-handed weapon skill?")=='y') {
		    unrestrict_weapon_skill(P_TWO_HANDED_WEAPON);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_TWO_WEAPON_COMBAT) && yn("Do you want to learn the two-weapon combat skill?")=='y') {
		    unrestrict_weapon_skill(P_TWO_WEAPON_COMBAT);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_POLYMORPHING) && yn("Do you want to learn the polymorphing skill?")=='y') {
		    unrestrict_weapon_skill(P_POLYMORPHING);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_DEVICES) && yn("Do you want to learn the devices skill?")=='y') {
		    unrestrict_weapon_skill(P_DEVICES);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_SEARCHING) && yn("Do you want to learn the searching skill?")=='y') {
		    unrestrict_weapon_skill(P_SEARCHING);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_SPIRITUALITY) && yn("Do you want to learn the spirituality skill?")=='y') {
		    unrestrict_weapon_skill(P_SPIRITUALITY);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_PETKEEPING) && yn("Do you want to learn the petkeeping skill?")=='y') {
		    unrestrict_weapon_skill(P_PETKEEPING);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_MISSILE_WEAPONS) && yn("Do you want to learn the missile weapons skill?")=='y') {
		    unrestrict_weapon_skill(P_MISSILE_WEAPONS);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_TECHNIQUES) && yn("Do you want to learn the techniques skill?")=='y') {
		    unrestrict_weapon_skill(P_TECHNIQUES);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_IMPLANTS) && yn("Do you want to learn the implants skill?")=='y') {
		    unrestrict_weapon_skill(P_IMPLANTS);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_SEXY_FLATS) && yn("Do you want to learn the sexy flats skill?")=='y') {
		    unrestrict_weapon_skill(P_SEXY_FLATS);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_MEMORIZATION) && yn("Do you want to learn the memorization skill?")=='y') {
		    unrestrict_weapon_skill(P_MEMORIZATION);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_GUN_CONTROL) && yn("Do you want to learn the gun control skill?")=='y') {
		    unrestrict_weapon_skill(P_GUN_CONTROL);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_SQUEAKING) && yn("Do you want to learn the squeaking skill?")=='y') {
		    unrestrict_weapon_skill(P_SQUEAKING);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_SYMBIOSIS) && yn("Do you want to learn the symbiosis skill?")=='y') {
		    unrestrict_weapon_skill(P_SYMBIOSIS);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_SHII_CHO) && yn("Do you want to learn the form I (Shii-Cho) skill?")=='y') {
		    unrestrict_weapon_skill(P_SHII_CHO);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_MAKASHI) && yn("Do you want to learn the form II (Makashi) skill?")=='y') {
		    unrestrict_weapon_skill(P_MAKASHI);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_SORESU) && yn("Do you want to learn the form III (Soresu) skill?")=='y') {
		    unrestrict_weapon_skill(P_SORESU);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_ATARU) && yn("Do you want to learn the form IV (Ataru) skill?")=='y') {
		    unrestrict_weapon_skill(P_ATARU);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_SHIEN) && yn("Do you want to learn the form V (Shien) skill?")=='y') {
		    unrestrict_weapon_skill(P_SHIEN);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_DJEM_SO) && yn("Do you want to learn the form V (Djem So) skill?")=='y') {
		    unrestrict_weapon_skill(P_DJEM_SO);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_NIMAN) && yn("Do you want to learn the form VI (Niman) skill?")=='y') {
		    unrestrict_weapon_skill(P_NIMAN);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_JUYO) && yn("Do you want to learn the form VII (Juyo) skill?")=='y') {
		    unrestrict_weapon_skill(P_JUYO);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_VAAPAD) && yn("Do you want to learn the form VII (Vaapad) skill?")=='y') {
		    unrestrict_weapon_skill(P_VAAPAD);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_WEDI) && yn("Do you want to learn the form VIII (Wedi) skill?")=='y') {
		    unrestrict_weapon_skill(P_WEDI);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_BARE_HANDED_COMBAT) && yn("Do you want to learn the bare-handed combat skill?")=='y') {
		    unrestrict_weapon_skill(P_BARE_HANDED_COMBAT);	acquiredskill = 1; }
	else if (P_RESTRICTED(P_MARTIAL_ARTS) && yn("Do you want to learn the martial arts skill?")=='y') {
		    unrestrict_weapon_skill(P_MARTIAL_ARTS);	acquiredskill = 1; }
	else if (yn("Do you want to learn no new skill at all?")=='y') {
		    acquiredskill = 1; }
	}

	pline("Check out what you got!");

}

void
additionalskilltraining()
{
	You("may add a few training points in a skill of your choice!");

	int acquiredskill;
	acquiredskill = 0;

	pline("Pick a skill to train. The prompt will loop until you actually make a choice.");

	while (acquiredskill == 0) { /* ask the player what they want --Amy */

	if (!(P_RESTRICTED(P_DAGGER)) && yn("Do you want to train the dagger skill?")=='y') {
		P_ADVANCE(P_DAGGER) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_KNIFE)) && yn("Do you want to train the knife skill?")=='y') {
		P_ADVANCE(P_KNIFE) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_AXE)) && yn("Do you want to train the axe skill?")=='y') {
		P_ADVANCE(P_AXE) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_PICK_AXE)) && yn("Do you want to train the pick-axe skill?")=='y') {
		P_ADVANCE(P_PICK_AXE) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_SHORT_SWORD)) && yn("Do you want to train the short sword skill?")=='y') {
		P_ADVANCE(P_SHORT_SWORD) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_BROAD_SWORD)) && yn("Do you want to train the broad sword skill?")=='y') {
		P_ADVANCE(P_BROAD_SWORD) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_LONG_SWORD)) && yn("Do you want to train the long sword skill?")=='y') {
		P_ADVANCE(P_LONG_SWORD) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_TWO_HANDED_SWORD)) && yn("Do you want to train the two-handed sword skill?")=='y') {
		P_ADVANCE(P_TWO_HANDED_SWORD) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_SCIMITAR)) && yn("Do you want to train the scimitar skill?")=='y') {
		P_ADVANCE(P_SCIMITAR) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_SABER)) && yn("Do you want to train the saber skill?")=='y') {
		P_ADVANCE(P_SABER) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_CLUB)) && yn("Do you want to train the club skill?")=='y') {
		P_ADVANCE(P_CLUB) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_PADDLE)) && yn("Do you want to train the paddle skill?")=='y') {
		P_ADVANCE(P_PADDLE) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_MACE)) && yn("Do you want to train the mace skill?")=='y') {
		P_ADVANCE(P_MACE) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_MORNING_STAR)) && yn("Do you want to train the morning star skill?")=='y') {
		P_ADVANCE(P_MORNING_STAR) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_FLAIL)) && yn("Do you want to train the flail skill?")=='y') {
		P_ADVANCE(P_FLAIL) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_HAMMER)) && yn("Do you want to train the hammer skill?")=='y') {
		P_ADVANCE(P_HAMMER) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_QUARTERSTAFF)) && yn("Do you want to train the quarterstaff skill?")=='y') {
		P_ADVANCE(P_QUARTERSTAFF) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_ORB)) && yn("Do you want to train the orb skill?")=='y') {
		P_ADVANCE(P_ORB) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_CLAW)) && yn("Do you want to train the claw skill?")=='y') {
		P_ADVANCE(P_CLAW) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_GRINDER)) && yn("Do you want to train the grinder skill?")=='y') {
		P_ADVANCE(P_GRINDER) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_POLEARMS)) && yn("Do you want to train the polearms skill?")=='y') {
		P_ADVANCE(P_POLEARMS) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_SPEAR)) && yn("Do you want to train the spear skill?")=='y') {
		P_ADVANCE(P_SPEAR) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_JAVELIN)) && yn("Do you want to train the javelin skill?")=='y') {
		P_ADVANCE(P_JAVELIN) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_TRIDENT)) && yn("Do you want to train the trident skill?")=='y') {
		P_ADVANCE(P_TRIDENT) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_LANCE)) && yn("Do you want to train the lance skill?")=='y') {
		P_ADVANCE(P_LANCE) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_BOW)) && yn("Do you want to train the bow skill?")=='y') {
		P_ADVANCE(P_BOW) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_SLING)) && yn("Do you want to train the sling skill?")=='y') {
		P_ADVANCE(P_SLING) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_FIREARM)) && yn("Do you want to train the firearms skill?")=='y') {
		P_ADVANCE(P_FIREARM) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_CROSSBOW)) && yn("Do you want to train the crossbow skill?")=='y') {
		P_ADVANCE(P_CROSSBOW) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_DART)) && yn("Do you want to train the dart skill?")=='y') {
		P_ADVANCE(P_DART) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_SHURIKEN)) && yn("Do you want to train the shuriken skill?")=='y') {
		P_ADVANCE(P_SHURIKEN) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_BOOMERANG)) && yn("Do you want to train the boomerang skill?")=='y') {
		P_ADVANCE(P_BOOMERANG) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_WHIP)) && yn("Do you want to train the whip skill?")=='y') {
		P_ADVANCE(P_WHIP) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_UNICORN_HORN)) && yn("Do you want to train the unicorn horn skill?")=='y') {
		P_ADVANCE(P_UNICORN_HORN) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_LIGHTSABER)) && yn("Do you want to train the lightsaber skill?")=='y') {
		P_ADVANCE(P_LIGHTSABER) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_ATTACK_SPELL)) && yn("Do you want to train the attack spell skill?")=='y') {
		P_ADVANCE(P_ATTACK_SPELL) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_HEALING_SPELL)) && yn("Do you want to train the healing spell skill?")=='y') {
		P_ADVANCE(P_HEALING_SPELL) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_DIVINATION_SPELL)) && yn("Do you want to train the divination spell skill?")=='y') {
		P_ADVANCE(P_DIVINATION_SPELL) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_ENCHANTMENT_SPELL)) && yn("Do you want to train the enchantment spell skill?")=='y') {
		P_ADVANCE(P_ENCHANTMENT_SPELL) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_PROTECTION_SPELL)) && yn("Do you want to train the protection spell skill?")=='y') {
		P_ADVANCE(P_PROTECTION_SPELL) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_BODY_SPELL)) && yn("Do you want to train the body spell skill?")=='y') {
		P_ADVANCE(P_BODY_SPELL) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_OCCULT_SPELL)) && yn("Do you want to train the occult spell skill?")=='y') {
		P_ADVANCE(P_OCCULT_SPELL) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_ELEMENTAL_SPELL)) && yn("Do you want to train the elemental spell skill?")=='y') {
		P_ADVANCE(P_ELEMENTAL_SPELL) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_CHAOS_SPELL)) && yn("Do you want to train the chaos spell skill?")=='y') {
		P_ADVANCE(P_CHAOS_SPELL) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_MATTER_SPELL)) && yn("Do you want to train the matter spell skill?")=='y') {
		P_ADVANCE(P_MATTER_SPELL) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_RIDING)) && yn("Do you want to train the riding skill?")=='y') {
		P_ADVANCE(P_RIDING) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_HIGH_HEELS)) && yn("Do you want to train the high heels skill?")=='y') {
		P_ADVANCE(P_HIGH_HEELS) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_GENERAL_COMBAT)) && yn("Do you want to train the general combat skill?")=='y') {
		P_ADVANCE(P_GENERAL_COMBAT) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_SHIELD)) && yn("Do you want to train the shield skill?")=='y') {
		P_ADVANCE(P_SHIELD) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_BODY_ARMOR)) && yn("Do you want to train the body armor skill?")=='y') {
		P_ADVANCE(P_BODY_ARMOR) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_TWO_HANDED_WEAPON)) && yn("Do you want to train the two-handed weapon skill?")=='y') {
		P_ADVANCE(P_TWO_HANDED_WEAPON) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_TWO_WEAPON_COMBAT)) && yn("Do you want to train the two-weapon combat skill?")=='y') {
		P_ADVANCE(P_TWO_WEAPON_COMBAT) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_POLYMORPHING)) && yn("Do you want to train the polymorphing skill?")=='y') {
		P_ADVANCE(P_POLYMORPHING) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_DEVICES)) && yn("Do you want to train the devices skill?")=='y') {
		P_ADVANCE(P_DEVICES) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_SEARCHING)) && yn("Do you want to train the searching skill?")=='y') {
		P_ADVANCE(P_SEARCHING) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_SPIRITUALITY)) && yn("Do you want to train the spirituality skill?")=='y') {
		P_ADVANCE(P_SPIRITUALITY) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_PETKEEPING)) && yn("Do you want to train the petkeeping skill?")=='y') {
		P_ADVANCE(P_PETKEEPING) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_MISSILE_WEAPONS)) && yn("Do you want to train the missile weapons skill?")=='y') {
		P_ADVANCE(P_MISSILE_WEAPONS) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_TECHNIQUES)) && yn("Do you want to train the techniques skill?")=='y') {
		P_ADVANCE(P_TECHNIQUES) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_IMPLANTS)) && yn("Do you want to train the implants skill?")=='y') {
		P_ADVANCE(P_IMPLANTS) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_SEXY_FLATS)) && yn("Do you want to train the sexy flats skill?")=='y') {
		P_ADVANCE(P_SEXY_FLATS) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_MEMORIZATION)) && yn("Do you want to train the memorization skill?")=='y') {
		P_ADVANCE(P_MEMORIZATION) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_SQUEAKING)) && yn("Do you want to train the squeaking skill?")=='y') {
		P_ADVANCE(P_SQUEAKING) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_SYMBIOSIS)) && yn("Do you want to train the symbiosis skill?")=='y') {
		P_ADVANCE(P_SYMBIOSIS) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_GUN_CONTROL)) && yn("Do you want to train the gun control skill?")=='y') {
		P_ADVANCE(P_GUN_CONTROL) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_SHII_CHO)) && yn("Do you want to train the form I (Shii-Cho) skill?")=='y') {
		P_ADVANCE(P_SHII_CHO) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_MAKASHI)) && yn("Do you want to train the form II (Makashi) skill?")=='y') {
		P_ADVANCE(P_MAKASHI) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_SORESU)) && yn("Do you want to train the form III (Soresu) skill?")=='y') {
		P_ADVANCE(P_SORESU) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_ATARU)) && yn("Do you want to train the form IV (Ataru) skill?")=='y') {
		P_ADVANCE(P_ATARU) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_SHIEN)) && yn("Do you want to train the form V (Shien) skill?")=='y') {
		P_ADVANCE(P_SHIEN) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_DJEM_SO)) && yn("Do you want to train the form V (Djem So) skill?")=='y') {
		P_ADVANCE(P_DJEM_SO) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_NIMAN)) && yn("Do you want to train the form VI (Niman) skill?")=='y') {
		P_ADVANCE(P_NIMAN) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_JUYO)) && yn("Do you want to train the form VII (Juyo) skill?")=='y') {
		P_ADVANCE(P_JUYO) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_VAAPAD)) && yn("Do you want to train the form VII (Vaapad) skill?")=='y') {
		P_ADVANCE(P_VAAPAD) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_WEDI)) && yn("Do you want to train the form VIII (Wedi) skill?")=='y') {
		P_ADVANCE(P_WEDI) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_BARE_HANDED_COMBAT)) && yn("Do you want to train the bare-handed combat skill?")=='y') {
		P_ADVANCE(P_BARE_HANDED_COMBAT) += rnd(10);
		acquiredskill = 1; }
	else if (!(P_RESTRICTED(P_MARTIAL_ARTS)) && yn("Do you want to train the martial arts skill?")=='y') {
		P_ADVANCE(P_MARTIAL_ARTS) += rnd(10);
		acquiredskill = 1; }
	else if (yn("Do you want to train no skill at all?")=='y') {
		acquiredskill = 1; }
	}
	pline("Training complete!");

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
		You("cannot increase your skill in %s.", wpskillname(weapon_type(uwep)));
	else You("cannot learn much about %s right now.",
                wpskillname(weapon_type(uwep)));
                
}

void
grinderattack(direction)
int direction;
/* 1 = east, 2 = southeast, 3 = north, 4 = northwest, 5 = west, 6 = southwest, 7 = south, 8 = northeast */
{
	int grndax, grnday, grndbx, grndby, grndcx, grndcy, grnddx, grnddy;
	struct monst *mtmp;
	int grindingdamage = 1;
	if (!PlayerCannotUseSkills) {
		switch (P_SKILL(P_GRINDER)) {
			default: break;
			case P_BASIC:
				grindingdamage += 1;
				break;
			case P_SKILLED:
				grindingdamage += rnd(2);
				break;
			case P_EXPERT:
				grindingdamage += rnd(3);
				break;
			case P_MASTER:
				grindingdamage += rnd(4);
				break;
			case P_GRAND_MASTER:
				grindingdamage += rnd(5);
				break;
			case P_SUPREME_MASTER:
				grindingdamage += rnd(6);
				break;
		}
		switch (P_SKILL(P_AXE)) {
			default: break;
			case P_BASIC:
				grindingdamage += 1;
				break;
			case P_SKILLED:
				grindingdamage += rnd(2);
				break;
			case P_EXPERT:
				grindingdamage += rnd(3);
				break;
			case P_MASTER:
				grindingdamage += rnd(4);
				break;
			case P_GRAND_MASTER:
				grindingdamage += rnd(5);
				break;
			case P_SUPREME_MASTER:
				grindingdamage += rnd(6);
				break;
		}
	}
	if (uwep && objects[uwep->otyp].oc_skill == (tech_inuse(T_GRAP_SWAP) ? P_LANCE : P_GRINDER) && uwep->spe > 0) grindingdamage += uwep->spe;

	if (grindingdamage > 1) grindingdamage = rnd(grindingdamage);

	switch (direction) {

		case 1:
			grndax = u.ux + 1;
			grnday = u.uy;
			grndbx = u.ux + 2;
			grndby = u.uy;
			grndcx = u.ux + 1;
			grndcy = u.uy + 1;
			grnddx = u.ux + 1;
			grnddy = u.uy - 1;
			break;
		case 2:
			grndax = u.ux + 1;
			grnday = u.uy + 1;
			grndbx = u.ux + 1;
			grndby = u.uy;
			grndcx = u.ux;
			grndcy = u.uy + 1;
			grnddx = u.ux + 2;
			grnddy = u.uy + 2;
			break;
		case 3:
			grndax = u.ux;
			grnday = u.uy - 1;
			grndbx = u.ux;
			grndby = u.uy - 2;
			grndcx = u.ux - 1;
			grndcy = u.uy - 1;
			grnddx = u.ux + 1;
			grnddy = u.uy - 1;
			break;
		case 4:
			grndax = u.ux - 1;
			grnday = u.uy - 1;
			grndbx = u.ux - 1;
			grndby = u.uy;
			grndcx = u.ux;
			grndcy = u.uy - 1;
			grnddx = u.ux - 2;
			grnddy = u.uy - 2;
			break;
		case 5:
			grndax = u.ux - 1;
			grnday = u.uy;
			grndbx = u.ux - 2;
			grndby = u.uy;
			grndcx = u.ux - 1;
			grndcy = u.uy + 1;
			grnddx = u.ux - 1;
			grnddy = u.uy - 1;
			break;
		case 6:
			grndax = u.ux - 1;
			grnday = u.uy + 1;
			grndbx = u.ux - 1;
			grndby = u.uy;
			grndcx = u.ux;
			grndcy = u.uy + 1;
			grnddx = u.ux - 2;
			grnddy = u.uy + 2;
			break;
		case 7:
			grndax = u.ux;
			grnday = u.uy + 1;
			grndbx = u.ux;
			grndby = u.uy + 2;
			grndcx = u.ux - 1;
			grndcy = u.uy + 1;
			grnddx = u.ux + 1;
			grnddy = u.uy + 1;
			break;
		case 8:
			grndax = u.ux + 1;
			grnday = u.uy - 1;
			grndbx = u.ux + 1;
			grndby = u.uy;
			grndcx = u.ux;
			grndcy = u.uy - 1;
			grnddx = u.ux + 2;
			grnddy = u.uy - 2;
			break;
		default:
			grndax = -1;
			grnday = -1;
			grndbx = -1;
			grndby = -1;
			grndcx = -1;
			grndcy = -1;
			grnddx = -1;
			grnddy = -1;
			impossible("grinder attack %d used", direction);
			break;

	}

	if (isok(grndax, grnday) && (mtmp = m_at(grndax, grnday)) && !(mtmp->mtame) && !(mtmp->mpeaceful) && !DEADMONSTER(mtmp)) {
		Your("weapon grinds %s!", mon_nam(mtmp));
		hurtmon(mtmp, grindingdamage);
	}
	if (isok(grndbx, grndby) && (mtmp = m_at(grndbx, grndby)) && !(mtmp->mtame) && !(mtmp->mpeaceful) && !DEADMONSTER(mtmp)) {
		Your("weapon grinds %s!", mon_nam(mtmp));
		hurtmon(mtmp, grindingdamage);
	}
	if (isok(grndcx, grndcy) && (mtmp = m_at(grndcx, grndcy)) && !(mtmp->mtame) && !(mtmp->mpeaceful) && !DEADMONSTER(mtmp)) {
		Your("weapon grinds %s!", mon_nam(mtmp));
		hurtmon(mtmp, grindingdamage);
	}
	if (isok(grnddx, grnddy) && (mtmp = m_at(grnddx, grnddy)) && !(mtmp->mtame) && !(mtmp->mpeaceful) && !DEADMONSTER(mtmp)) {
		Your("weapon grinds %s!", mon_nam(mtmp));
		hurtmon(mtmp, grindingdamage);
	}

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

/* for random bad effect: lose a lot of knowledge in a skill
 * this will only select unrestricted ones that you have actually trained */
void
evilskilldecrease()
{
	int pickskill, tryct, tryct2, i, lossamount;

eviltryagain:
	pickskill = rnd(P_RIDING);
	if (rn2(1000) && (P_RESTRICTED(pickskill) || (P_ADVANCE(pickskill) < 1) )) goto eviltryagain;

	if (P_RESTRICTED(pickskill) || (P_ADVANCE(pickskill) < 1)) return; /* apparently you have no eligible skills */

	lossamount = rnd(P_ADVANCE(pickskill));

	if ((P_ADVANCE(pickskill)) < lossamount) P_ADVANCE(pickskill) = 0;
	else P_ADVANCE(pickskill) -= lossamount;

	if (!P_RESTRICTED(pickskill)) {
		pline("Your %s skill falls off, and you're much less skilled than before.", wpskillname(pickskill));

		tryct = 2000;
		tryct2 = 10;
		i = 0;

		while (u.skills_advanced && tryct && (P_ADVANCE(pickskill) < practice_needed_to_advance_nonmax(P_SKILL(pickskill) - 1, pickskill) ) ) {
			lose_last_spent_skill();
			i++;
			tryct--;
		}

		while (i) {
			if (evilfriday) pline("This is the evil variant. Your skill point is lost forever.");
			else u.weapon_slots++;  /* because every skill up costs one slot --Amy */
			i--;
		}

		/* still higher than the cap? that probably means you started with some knowledge of the skill... */
		while (tryct2 && P_ADVANCE(pickskill) < practice_needed_to_advance_nonmax(P_SKILL(pickskill) - 1, pickskill) ) {
			P_SKILL(pickskill)--;
			if (evilfriday) pline("This is the evil variant. Your skill point is lost forever.");
			else u.weapon_slots++;
			tryct2--;
		}

	}

}

/* data delete: unconditionally set all the skills to zero, even if you've spent the previous 200k turns training them
 * you can still train them back up from zero, but all training will be lost, no matter how much it was --Amy */
void
dataskilldecrease()
{
	int pickskill, tryct, tryct2, i;
	tryct = 50000;
	i = 0;
	int permloss = 0;

	while (u.skills_advanced && tryct) {
		lose_last_spent_skill();
		i++;
		tryct--;
	}

	while (i) {
		if (!evilfriday) u.weapon_slots++;  /* because every skill up costs one slot --Amy */
		else permloss++;
		i--;
	}

	/* still higher than the cap? that probably means you started with some knowledge of the skill... */

	for (pickskill = 0; pickskill < P_NUM_SKILLS; pickskill++) {

		if (pickskill > P_NONE) {

			tryct2 = 20;
			P_ADVANCE(pickskill) = 0; /* zonk all training of that skill */

			while (tryct2 && !P_RESTRICTED(pickskill) && P_ADVANCE(pickskill) < practice_needed_to_advance_nonmax(P_SKILL(pickskill) - 1, pickskill) ) {
				P_SKILL(pickskill)--;
				if (!evilfriday) u.weapon_slots++;
				else permloss++;
				tryct2--;
			}
		}
	}

	if (permloss > 0) You("permanently lost %d skill points because this is the evil variant.", permloss);

}

void
skilltrainingdecrease(lossamount)
int lossamount;
{
	int pickskill, tryct, tryct2, i;

	if (isdemagogue) { /* skill loss could be beneficial for them, so you get something else instead */

		pline("You are frenzied!");
		if (u.berserktime) {
		    if (!obsidianprotection()) switch (rn2(11)) {
		    case 0:
				make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON),20),
				"demagogic sickness", TRUE, SICK_NONVOMITABLE);
			    break;
		    case 1: make_blinded(Blinded + lossamount, TRUE);
			    break;
		    case 2: if (!Confusion)
				You("suddenly feel %s.",
				    FunnyHallu ? "trippy" : "confused");
			    make_confused(HConfusion + lossamount, TRUE);
			    break;
		    case 3: make_stunned(HStun + lossamount, TRUE);
			    break;
		    case 4: make_numbed(HNumbed + lossamount, TRUE);
			    break;
		    case 5: make_frozen(HFrozen + lossamount, TRUE);
			    break;
		    case 6: make_burned(HBurned + lossamount, TRUE);
			    break;
		    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
			    break;
		    case 8: (void) make_hallucinated(HHallucination + lossamount, TRUE, 0L);
			    break;
		    case 9: make_feared(HFeared + lossamount, TRUE);
			    break;
		    case 10: make_dimmed(HDimmed + lossamount, TRUE);
			    break;
		    }

		} else u.berserktime = lossamount;

		demagogueparole();
		aggravate();

		if (!rn2(100)) {

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;
			register int randsp, randmnst, randmnsx, monstercolor;
			struct permonst *randmonstforspawn;

		      cx = rn2(COLNO);
		      cy = rn2(ROWNO);

			if (!rn2(4)) {

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			randmnst = (rn2(187) + 1);
			randmnsx = (rn2(100) + 1);

				if (wizard || !rn2(10)) pline("You suddenly feel a surge of tension!");

			for (i = 0; i < randsp; i++) {
			/* This function will fill the map with a random amount of monsters of one class. --Amy */

			if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

			if (randmnst < 6)
		 	    (void) makemon(mkclass(S_ANT,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 9)
		 	    (void) makemon(mkclass(S_BLOB,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 11)
		 	    (void) makemon(mkclass(S_COCKATRICE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 15)
		 	    (void) makemon(mkclass(S_DOG,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 18)
		 	    (void) makemon(mkclass(S_EYE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 22)
		 	    (void) makemon(mkclass(S_FELINE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 24)
		 	    (void) makemon(mkclass(S_GREMLIN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 29)
		 	    (void) makemon(mkclass(S_HUMANOID,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 33)
		 	    (void) makemon(mkclass(S_IMP,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 36)
		 	    (void) makemon(mkclass(S_JELLY,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 41)
		 	    (void) makemon(mkclass(S_KOBOLD,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 44)
		 	    (void) makemon(mkclass(S_LEPRECHAUN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 47)
		 	    (void) makemon(mkclass(S_MIMIC,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 50)
		 	    (void) makemon(mkclass(S_NYMPH,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 54)
		 	    (void) makemon(mkclass(S_ORC,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 55)
		 	    (void) makemon(mkclass(S_PIERCER,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 58)
		 	    (void) makemon(mkclass(S_QUADRUPED,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 62)
		 	    (void) makemon(mkclass(S_RODENT,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 65)
		 	    (void) makemon(mkclass(S_SPIDER,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 66)
		 	    (void) makemon(mkclass(S_TRAPPER,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 69)
		 	    (void) makemon(mkclass(S_UNICORN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 71)
		 	    (void) makemon(mkclass(S_VORTEX,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 73)
		 	    (void) makemon(mkclass(S_WORM,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 75)
		 	    (void) makemon(mkclass(S_XAN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 76)
		 	    (void) makemon(mkclass(S_LIGHT,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 77)
		 	    (void) makemon(mkclass(S_ZOUTHERN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 78)
		 	    (void) makemon(mkclass(S_ANGEL,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 81)
		 	    (void) makemon(mkclass(S_BAT,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 83)
		 	    (void) makemon(mkclass(S_CENTAUR,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 86)
		 	    (void) makemon(mkclass(S_DRAGON,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 89)
		 	    (void) makemon(mkclass(S_ELEMENTAL,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 94)
		 	    (void) makemon(mkclass(S_FUNGUS,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 99)
		 	    (void) makemon(mkclass(S_GNOME,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 102)
		 	    (void) makemon(mkclass(S_GIANT,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 103)
		 	    (void) makemon(mkclass(S_JABBERWOCK,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 104)
		 	    (void) makemon(mkclass(S_KOP,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 105)
		 	    (void) makemon(mkclass(S_LICH,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 108)
		 	    (void) makemon(mkclass(S_MUMMY,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 110)
		 	    (void) makemon(mkclass(S_NAGA,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 113)
		 	    (void) makemon(mkclass(S_OGRE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 115)
		 	    (void) makemon(mkclass(S_PUDDING,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 116)
		 	    (void) makemon(mkclass(S_QUANTMECH,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 118)
		 	    (void) makemon(mkclass(S_RUSTMONST,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 121)
		 	    (void) makemon(mkclass(S_SNAKE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 123)
		 	    (void) makemon(mkclass(S_TROLL,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 124)
		 	    (void) makemon(mkclass(S_UMBER,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 125)
		 	    (void) makemon(mkclass(S_VAMPIRE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 127)
		 	    (void) makemon(mkclass(S_WRAITH,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 128)
		 	    (void) makemon(mkclass(S_XORN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 130)
		 	    (void) makemon(mkclass(S_YETI,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 135)
		 	    (void) makemon(mkclass(S_ZOMBIE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 145)
		 	    (void) makemon(mkclass(S_HUMAN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 147)
		 	    (void) makemon(mkclass(S_GHOST,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 149)
		 	    (void) makemon(mkclass(S_GOLEM,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 152)
		 	    (void) makemon(mkclass(S_DEMON,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 155)
		 	    (void) makemon(mkclass(S_EEL,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 160)
		 	    (void) makemon(mkclass(S_LIZARD,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 162)
		 	    (void) makemon(mkclass(S_BAD_FOOD,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 165)
		 	    (void) makemon(mkclass(S_BAD_COINS,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 166) {
				if (randmnsx < 96)
		 	    (void) makemon(mkclass(S_HUMAN,0), cx, cy, MM_ADJACENTOK);
				else
		 	    (void) makemon(mkclass(S_NEMESE,0), cx, cy, MM_ADJACENTOK);
				}
			else if (randmnst < 171)
		 	    (void) makemon(mkclass(S_GRUE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 176)
		 	    (void) makemon(mkclass(S_WALLMONST,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 180)
		 	    (void) makemon(mkclass(S_RUBMONST,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 181) {
				if (randmnsx < 99)
		 	    (void) makemon(mkclass(S_HUMAN,0), cx, cy, MM_ADJACENTOK);
				else
		 	    (void) makemon(mkclass(S_ARCHFIEND,0), cx, cy, MM_ADJACENTOK);
				}
			else if (randmnst < 186)
		 	    (void) makemon(mkclass(S_TURRET,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 187)
		 	    (void) makemon(mkclass(S_FLYFISH,0), cx, cy, MM_ADJACENTOK);
			else
		 	    (void) makemon((struct permonst *)0, cx, cy, MM_ADJACENTOK);

			}

			}

			else if (!rn2(3)) {

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			randmonstforspawn = rndmonst();

			if (wizard || !rn2(10)) You_feel("the arrival of monsters!");

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(randmonstforspawn, cx, cy, MM_ADJACENTOK);
			}

			}

			else if (!rn2(2)) {

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			monstercolor = rnd(15);
			do { monstercolor = rnd(15); } while (monstercolor == CLR_BLUE);

			if (wizard || !rn2(10)) You_feel("a colorful sensation!");

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(colormon(monstercolor), cx, cy, MM_ADJACENTOK);
			}

			}

			else {

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			monstercolor = rnd(376);

			if (wizard || !rn2(10)) You_feel("that a group has arrived!");

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(monstercolor), cx, cy, MM_ADJACENTOK);
			}

			}

			u.aggravation = 0;


		}

		return;
	}

	pickskill = rnd(P_RIDING);

	/* this is so that the effect is actually noticeable... --Amy */
	if (!rn2(10)) lossamount *= (1 + rnd(9));
	lossamount *= rnd(5);

	if ((P_ADVANCE(pickskill)) < lossamount) P_ADVANCE(pickskill) = 0;
	else P_ADVANCE(pickskill) -= lossamount;

	if (!P_RESTRICTED(pickskill)) {
		pline("Your %s skill deteriorates.", wpskillname(pickskill));

		tryct = 2000;
		tryct2 = 10;
		i = 0;

		while (u.skills_advanced && tryct && (P_ADVANCE(pickskill) < practice_needed_to_advance_nonmax(P_SKILL(pickskill) - 1, pickskill) ) ) {
			lose_last_spent_skill();
			i++;
			tryct--;
		}

		while (i) {
			if (evilfriday) pline("This is the evil variant. Your skill point is lost forever.");
			else u.weapon_slots++;  /* because every skill up costs one slot --Amy */
			i--;
		}

		/* still higher than the cap? that probably means you started with some knowledge of the skill... */
		while (tryct2 && P_ADVANCE(pickskill) < practice_needed_to_advance_nonmax(P_SKILL(pickskill) - 1, pickskill) ) {
			P_SKILL(pickskill)--;
			if (evilfriday) pline("This is the evil variant. Your skill point is lost forever.");
			else u.weapon_slots++;
			tryct2--;
		}

	}

}

#endif /* OVLB */

/*weapon.c*/
