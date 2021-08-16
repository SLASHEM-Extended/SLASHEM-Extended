/*	SCCS Id: @(#)potion.c	3.4	2002/10/02	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"


/* KMH, intrinsics patch
 * There are many changes here to support >32-bit properties.
 * Also, blessed potions are once again permitted to convey
 * permanent intrinsics.
 */


#ifdef OVLB
boolean notonhead = FALSE;

static NEARDATA int nothing, unkn;
static NEARDATA const char beverages[] = { POTION_CLASS, 0 };

STATIC_DCL long itimeout(long);
STATIC_DCL long itimeout_incr(long,int);
STATIC_DCL void ghost_from_bottle(void);
STATIC_DCL short mixtype(struct obj *,struct obj *);
STATIC_PTR void set_litI(int,int,void *);

/* force `val' to be within valid range for intrinsic timeout value */
STATIC_OVL long
itimeout(val)
long val;
{
    if (val >= TIMEOUT) val = TIMEOUT;
    else if (val < 1) val = 0;

    return val;
}

/* increment `old' by `incr' and force result to be valid intrinsic timeout */
STATIC_OVL long
itimeout_incr(old, incr)
long old;
int incr;
{
    return itimeout((old & TIMEOUT) + (long)incr);
}

/* set the timeout field of intrinsic `which' */
void
set_itimeout(which, val)
long *which, val;
{
    *which &= ~TIMEOUT;
    *which |= itimeout(val);
}

/* increment the timeout field of intrinsic `which' */
void
incr_itimeout(which, incr)
long *which;
int incr;
{
    set_itimeout(which, itimeout_incr(*which, incr));
}

boolean
is_musable(otmp)
struct obj *otmp;
{
	if (!otmp) {
		impossible("is_musable called with no object");
		return FALSE;
	}

	switch (otmp->otyp) {

		case SCR_TELEPORTATION:
		case POT_HEALING:
		case POT_CURE_WOUNDS:
		case POT_CURE_SERIOUS_WOUNDS:
		case POT_CURE_CRITICAL_WOUNDS:
		case POT_EXTRA_HEALING:
		case WAN_DIGGING:
		case WAN_CREATE_MONSTER:
		case SCR_CREATE_MONSTER:
		case SCR_CREATE_VICTIM:
		case WAN_TELEPORTATION:
		case POT_FULL_HEALING:
		case WAN_HEALING:
		case WAN_EXTRA_HEALING:
		case WAN_CREATE_HORDE:
		case POT_VAMPIRE_BLOOD:
		case POT_BLOOD:
		case WAN_FULL_HEALING:
		case SCR_TELE_LEVEL:
		case SCR_BRANCH_TELEPORT:
		case WAN_TELE_LEVEL:
		case SCR_ROOT_PASSWORD_DETECTION:
		case RIN_TIMELY_BACKUP:
		case SCR_SUMMON_UNDEAD:
		case WAN_SUMMON_UNDEAD:
		case SCR_HEALING:
		case SCR_EXTRA_HEALING:
		case SCR_POWER_HEALING:
		case SCR_WARPING:
		case WAN_DEATH:
		case WAN_SLEEP:
		case WAN_FIREBALL:
		case WAN_FIRE:
		case WAN_COLD:
		case WAN_LIGHTNING:
		case WAN_MAGIC_MISSILE:
		case WAN_STRIKING:
		case SCR_FIRE:
		case POT_PARALYSIS:
		case POT_BLINDNESS:
		case POT_CONFUSION:
		case POT_SLEEPING:
		case POT_ACID:
		case FROST_HORN:
		case FIRE_HORN:
		case TEMPEST_HORN:
		case ETHER_HORN:
		case SHADOW_HORN:
		case CHROME_HORN:
		case WAN_DRAINING:
		case WAN_INCREASE_MAX_HITPOINTS:
		case WAN_REDUCE_MAX_HITPOINTS:
		case SCR_EARTH:
		case POT_AMNESIA:
		case WAN_CANCELLATION:
		case POT_CYANIDE:
		case POT_RADIUM:
		case WAN_ACID:
		case SCR_TRAP_CREATION:
		case SCR_CREATE_TRAP:
		case WAN_TRAP_CREATION:
		case SCR_FLOOD:
		case SCR_LAVA:
		case SCR_GRAVE:
		case SCR_TUNNELS:
		case SCR_FARMING:
		case SCR_MOUNTAINS:
		case SCR_DIVING:
		case SCR_CRYSTALLIZATION:
		case SCR_MOORLAND:
		case SCR_URINE:
		case SCR_QUICKSAND:
		case SCR_STYX:
		case SCR_SNOW:
		case SCR_ASH:
		case SCR_SAND:
		case SCR_PAVING:
		case SCR_HIGHWAY:
		case SCR_GRASSLAND:
		case SCR_NETHER:
		case SCR_STALACTITE:
		case SCR_CRYPT:
		case SCR_BUBBLE_BOBBLE:
		case SCR_RAIN:
		case SCR_GROWTH:
		case SCR_ICE:
		case SCR_CLOUDS:
		case SCR_BARRHING:
		case WAN_SOLAR_BEAM:
		case WAN_POISON:
		case SCR_LOCKOUT:
		case WAN_BANISHMENT:
		case POT_HALLUCINATION:
		case POT_ICE:
		case POT_STUNNING:
		case POT_NUMBNESS:
		case POT_URINE:
		case POT_CANCELLATION:
		case POT_SLIME:
		case SCR_BAD_EFFECT:
		case WAN_BAD_EFFECT:
		case POT_FIRE:
		case POT_DIMNESS:
		case POT_SANITY:
		case WAN_SLOW_MONSTER:
		case WAN_FEAR:
		case POT_FEAR:
		case POT_GAIN_LEVEL:
		case WAN_GAIN_LEVEL:
		case WAN_MAKE_INVISIBLE:
		case POT_INVISIBILITY:
		case WAN_POLYMORPH:
		case WAN_MUTATION:
		case POT_SPEED:
		case WAN_SPEED_MONSTER:
		case POT_POLYMORPH:
		case POT_MUTATION:
		case WAN_CLONE_MONSTER:
		case SCR_DESTROY_ARMOR:
		case SCR_DESTROY_WEAPON:
		case SCR_STONING:
		case SCR_AMNESIA:
		case BAG_OF_TRICKS:
		case WAN_STONING:
		case WAN_DISINTEGRATION:
		case WAN_PARALYSIS:
		case WAN_CURSE_ITEMS:
		case WAN_AMNESIA:
		case WAN_BLEEDING:
		case WAN_UNDRESSING:
		case WAN_LEVITATION:
		case WAN_PSYBEAM:
		case WAN_HYPER_BEAM:
		case WAN_BAD_LUCK:
		case WAN_REMOVE_RESISTANCE:
		case WAN_CORROSION:
		case WAN_CHAOS_TERRAIN:
		case WAN_FLEECY_TERRAIN:
		case WAN_STAT_REDUCTION:
		case WAN_DISENCHANTMENT:
		case WAN_TREMBLING:
		case WAN_CONTAMINATION:
		case WAN_STARVATION:
		case WAN_CONFUSION:
		case WAN_SLIMING:
		case WAN_LYCANTHROPY:
		case WAN_FUMBLING:
		case WAN_PUNISHMENT:
		case SCR_PUNISHMENT:
		case WAN_MAKE_VISIBLE:
		case SCR_SUMMON_BOSS:
		case SCR_WOUNDS:
		case SCR_BULLSHIT:
		case SCR_CHAOS_TERRAIN:
		case SCR_NASTINESS:
		case SCR_NASTY_CURSE:
		case SCR_DEMONOLOGY:
		case SCR_ELEMENTALISM:
		case SCR_GIRLINESS:
		case WAN_SUMMON_SEXY_GIRL:
		case WAN_DISINTEGRATION_BEAM:
		case SCR_GROUP_SUMMONING:
		case WAN_CHROMATIC_BEAM:
		case WAN_STUN_MONSTER:
		case SCR_SUMMON_GHOST:
		case SCR_MEGALOAD:
		case SCR_VILENESS:
		case SCR_HYBRIDIZATION:
		case SCR_BAD_EQUIPMENT:
		case SCR_ENRAGE:
		case WAN_TIDAL_WAVE:
		case SCR_ANTIMATTER:
		case SCR_SUMMON_ELM:
		case WAN_SUMMON_ELM:
		case SCR_RELOCATION:
		case WAN_DRAIN_MANA:
		case WAN_FINGER_BENDING:
		case SCR_IMMOBILITY:
		case WAN_IMMOBILITY:
		case WAN_INSANITY:
		case WAN_BAD_EQUIPMENT:
		case SCR_FLOODING:
		case SCR_EGOISM:
		case WAN_EGOISM:
		case SCR_RUMOR:
		case SCR_MESSAGE:
		case SCR_ILLUSION:
		case SCR_VISIBLE_ITEM:
		case SCR_EVIL_VARIANT:
		case SCR_FEMINISM:
		case SCR_SIN:
		case WAN_SIN:
		case WAN_INERTIA:
		case WAN_TIME:
		case WAN_DESLEXIFICATION:
		case WAN_INFERNO:
		case WAN_ICE_BEAM:
		case WAN_THUNDER:
		case WAN_SLUDGE:
		case WAN_TOXIC:
		case WAN_NETHER_BEAM:
		case WAN_AURORA_BEAM:
		case WAN_GRAVITY_BEAM:
		case WAN_CHLOROFORM:
		case WAN_DREAM_EATER:
		case WAN_BUBBLEBEAM:
		case WAN_GOOD_NIGHT:
		case SCR_RAGNAROK:
		case SCR_OFFLEVEL_ITEM:
		case WAN_HASTE_MONSTER:
			return TRUE;
		default:
			return FALSE;

	}

	return FALSE;

}

boolean
ismusablenumber(number)
int number;
{
	switch (number) {

		case SCR_TELEPORTATION:
		case POT_HEALING:
		case POT_CURE_WOUNDS:
		case POT_CURE_SERIOUS_WOUNDS:
		case POT_CURE_CRITICAL_WOUNDS:
		case POT_EXTRA_HEALING:
		case WAN_DIGGING:
		case WAN_CREATE_MONSTER:
		case SCR_CREATE_MONSTER:
		case SCR_CREATE_VICTIM:
		case WAN_TELEPORTATION:
		case POT_FULL_HEALING:
		case WAN_HEALING:
		case WAN_EXTRA_HEALING:
		case WAN_CREATE_HORDE:
		case POT_VAMPIRE_BLOOD:
		case POT_BLOOD:
		case WAN_FULL_HEALING:
		case SCR_TELE_LEVEL:
		case SCR_BRANCH_TELEPORT:
		case WAN_TELE_LEVEL:
		case SCR_ROOT_PASSWORD_DETECTION:
		case RIN_TIMELY_BACKUP:
		case SCR_SUMMON_UNDEAD:
		case WAN_SUMMON_UNDEAD:
		case SCR_HEALING:
		case SCR_EXTRA_HEALING:
		case SCR_POWER_HEALING:
		case SCR_WARPING:
		case WAN_DEATH:
		case WAN_SLEEP:
		case WAN_FIREBALL:
		case WAN_FIRE:
		case WAN_COLD:
		case WAN_LIGHTNING:
		case WAN_MAGIC_MISSILE:
		case WAN_STRIKING:
		case SCR_FIRE:
		case POT_PARALYSIS:
		case POT_BLINDNESS:
		case POT_CONFUSION:
		case POT_SLEEPING:
		case POT_ACID:
		case FROST_HORN:
		case FIRE_HORN:
		case TEMPEST_HORN:
		case ETHER_HORN:
		case SHADOW_HORN:
		case CHROME_HORN:
		case WAN_DRAINING:
		case WAN_INCREASE_MAX_HITPOINTS:
		case WAN_REDUCE_MAX_HITPOINTS:
		case SCR_EARTH:
		case POT_AMNESIA:
		case WAN_CANCELLATION:
		case POT_CYANIDE:
		case POT_RADIUM:
		case WAN_ACID:
		case SCR_TRAP_CREATION:
		case SCR_CREATE_TRAP:
		case WAN_TRAP_CREATION:
		case SCR_FLOOD:
		case SCR_LAVA:
		case SCR_GRAVE:
		case SCR_TUNNELS:
		case SCR_FARMING:
		case SCR_MOUNTAINS:
		case SCR_DIVING:
		case SCR_CRYSTALLIZATION:
		case SCR_MOORLAND:
		case SCR_URINE:
		case SCR_QUICKSAND:
		case SCR_STYX:
		case SCR_SNOW:
		case SCR_ASH:
		case SCR_SAND:
		case SCR_PAVING:
		case SCR_HIGHWAY:
		case SCR_GRASSLAND:
		case SCR_NETHER:
		case SCR_STALACTITE:
		case SCR_CRYPT:
		case SCR_BUBBLE_BOBBLE:
		case SCR_RAIN:
		case SCR_GROWTH:
		case SCR_ICE:
		case SCR_CLOUDS:
		case SCR_BARRHING:
		case WAN_SOLAR_BEAM:
		case WAN_POISON:
		case SCR_LOCKOUT:
		case WAN_BANISHMENT:
		case POT_HALLUCINATION:
		case POT_ICE:
		case POT_STUNNING:
		case POT_NUMBNESS:
		case POT_URINE:
		case POT_CANCELLATION:
		case POT_SLIME:
		case SCR_BAD_EFFECT:
		case WAN_BAD_EFFECT:
		case POT_FIRE:
		case POT_DIMNESS:
		case POT_SANITY:
		case WAN_SLOW_MONSTER:
		case WAN_FEAR:
		case POT_FEAR:
		case POT_GAIN_LEVEL:
		case WAN_GAIN_LEVEL:
		case WAN_MAKE_INVISIBLE:
		case POT_INVISIBILITY:
		case WAN_POLYMORPH:
		case WAN_MUTATION:
		case POT_SPEED:
		case WAN_SPEED_MONSTER:
		case POT_POLYMORPH:
		case POT_MUTATION:
		case WAN_CLONE_MONSTER:
		case SCR_DESTROY_ARMOR:
		case SCR_DESTROY_WEAPON:
		case SCR_STONING:
		case SCR_AMNESIA:
		case BAG_OF_TRICKS:
		case WAN_STONING:
		case WAN_DISINTEGRATION:
		case WAN_PARALYSIS:
		case WAN_CURSE_ITEMS:
		case WAN_AMNESIA:
		case WAN_BLEEDING:
		case WAN_UNDRESSING:
		case WAN_LEVITATION:
		case WAN_PSYBEAM:
		case WAN_HYPER_BEAM:
		case WAN_BAD_LUCK:
		case WAN_REMOVE_RESISTANCE:
		case WAN_DISENCHANTMENT:
		case WAN_TREMBLING:
		case WAN_CONTAMINATION:
		case WAN_CORROSION:
		case WAN_CHAOS_TERRAIN:
		case WAN_FLEECY_TERRAIN:
		case WAN_STAT_REDUCTION:
		case WAN_STARVATION:
		case WAN_CONFUSION:
		case WAN_SLIMING:
		case WAN_LYCANTHROPY:
		case WAN_FUMBLING:
		case WAN_PUNISHMENT:
		case SCR_PUNISHMENT:
		case WAN_MAKE_VISIBLE:
		case SCR_SUMMON_BOSS:
		case SCR_WOUNDS:
		case SCR_BULLSHIT:
		case SCR_CHAOS_TERRAIN:
		case SCR_NASTINESS:
		case SCR_NASTY_CURSE:
		case SCR_DEMONOLOGY:
		case SCR_ELEMENTALISM:
		case SCR_GIRLINESS:
		case WAN_SUMMON_SEXY_GIRL:
		case WAN_DISINTEGRATION_BEAM:
		case SCR_GROUP_SUMMONING:
		case WAN_CHROMATIC_BEAM:
		case WAN_STUN_MONSTER:
		case SCR_SUMMON_GHOST:
		case SCR_MEGALOAD:
		case SCR_VILENESS:
		case SCR_HYBRIDIZATION:
		case SCR_BAD_EQUIPMENT:
		case SCR_ENRAGE:
		case WAN_TIDAL_WAVE:
		case SCR_ANTIMATTER:
		case SCR_SUMMON_ELM:
		case WAN_SUMMON_ELM:
		case SCR_RELOCATION:
		case WAN_DRAIN_MANA:
		case WAN_FINGER_BENDING:
		case SCR_IMMOBILITY:
		case WAN_IMMOBILITY:
		case WAN_INSANITY:
		case WAN_BAD_EQUIPMENT:
		case SCR_FLOODING:
		case SCR_EGOISM:
		case WAN_EGOISM:
		case SCR_RUMOR:
		case SCR_MESSAGE:
		case SCR_ILLUSION:
		case SCR_VISIBLE_ITEM:
		case SCR_EVIL_VARIANT:
		case SCR_FEMINISM:
		case SCR_SIN:
		case WAN_SIN:
		case WAN_INERTIA:
		case WAN_TIME:
		case WAN_DESLEXIFICATION:
		case WAN_INFERNO:
		case WAN_ICE_BEAM:
		case WAN_THUNDER:
		case WAN_SLUDGE:
		case WAN_TOXIC:
		case WAN_NETHER_BEAM:
		case WAN_AURORA_BEAM:
		case WAN_GRAVITY_BEAM:
		case WAN_CHLOROFORM:
		case WAN_DREAM_EATER:
		case WAN_BUBBLEBEAM:
		case WAN_GOOD_NIGHT:
		case SCR_RAGNAROK:
		case SCR_OFFLEVEL_ITEM:
		case WAN_HASTE_MONSTER:
			return TRUE;
		default:
			return FALSE;

	}

	return FALSE;

}

/* Practicant role gets a fine by Noroela --Amy */
void
fineforpracticant(fineamount, stonefine, arrowfine)
int fineamount, stonefine, arrowfine;
{
	/* do you already have a fine? if not, we want to start the timer that ticks down the time until you must have paid */
	boolean newfine = !(u.practicantpenalty || u.practicantstones || u.practicantarrows);
	if (newfine) u.practicanttime = 1000; /* always have 1000 turns to pay */

	u.practicantpenalty += fineamount;
	u.pract_finetimer = 0; /* reset timer for "how long did you get no fines at all?" */

	/* occasionally you have to pay in rocks or arrows */
	if (stonefine) u.practicantstones += stonefine;
	if (arrowfine) u.practicantarrows += arrowfine;
}

/* Practicant decides to pay, or waited until the timer ran out and pays automatically --Amy */
void
practicant_payup()
{
	int amountpaid = 0; /* the amount that you actually fork over */
	int stonespaid = 0;
	int arrowspaid = 0;

	if (u.practicantstones) { /* do rocks actually exist? note: if you genocide them it's your own damn fault */
		if (u.unobtainable == ROCK || u.unobtainable2 == ROCK || u.unobtainable3 == ROCK || u.unobtainable4 == ROCK || u.unobtainable5 == ROCK || u.unobtainable6 == ROCK || u.unobtainable7 == ROCK || u.unobtainable8 == ROCK || u.unobtainable9 == ROCK || u.unobtainable10 == ROCK) {
			u.practicantpenalty += u.practicantstones;
			u.practicantstones = 0;

			pline("%s booms: 'Since you can't pay in stones due to their nonexistence, pay in zorkmids instead!'", noroelaname());
		}
	}
	/* there are more than 10 kinds of arrows, so it's always possible in theory to pay an arrow fine */

	if (u.practicantpenalty) { /* do you actually have a fine in zorkmids? */

		/* do you have cash? */
		if (u.ugold > 0) {
			if (u.practicantpenalty > u.ugold) amountpaid = u.ugold;
			else amountpaid = u.practicantpenalty;

			You("pay %d zorkmids to %s.", amountpaid, noroelaname());

			u.ugold -= amountpaid;
			u.practicantcash += amountpaid;
			u.practicantpenalty -= amountpaid;
		}

		/* do you have gold in the bank? if yes, can use that to pay */
		if (u.practicantpenalty && u.bankcashamount) {
			if (u.practicantpenalty > u.bankcashamount) amountpaid = u.practicantpenalty;
			else amountpaid = u.bankcashamount;

			pline("Your bank account is used to pay %d zorkmids to %s.", amountpaid, noroelaname());

			u.bankcashamount -= amountpaid;
			u.practicantcash += amountpaid;
			u.practicantpenalty -= amountpaid;

		}

		if (u.ugold < 0) {
			impossible("your gold went below zero???");
			u.ugold = 0; /* fail safe */
		}

		if (u.bankcashamount < 0) {
			impossible("your bank cash amount went below zero???");
			u.bankcashamount = 0; /* fail safe */
		}

		if (u.practicantpenalty < 0) {
			impossible("practicant penalty went below zero???");
			u.practicantpenalty = 0; /* fail safe */
		}

		if (!u.practicantpenalty) {
			if (!u.practicantstones && !u.practicantarrows && !u.practicantpenalty) {
				u.practicanttime = 0;
				u.practicantseverity = 0;
				You("no longer have a fine to pay.");
			}
		} else {
			You("still need to pay %d more zorkmids to %s in %d turns.", u.practicantpenalty, noroelaname(), u.practicanttime);
		}
	}

	if (u.practicantstones) {
		register struct obj *prcstone;
findmorestones:
		prcstone = carrying(ROCK);
		if (prcstone) {
			if (prcstone->quan > u.practicantstones) {

				stonespaid += u.practicantstones;
				prcstone->quan -= u.practicantstones;
				prcstone->owt = weight(prcstone);
				u.practicantstones = 0;
				if (!u.practicantstones && !u.practicantarrows && !u.practicantpenalty) {
					u.practicanttime = 0;
					u.practicantseverity = 0;
					You("no longer have a fine to pay.");
				}
			} else {

				stonespaid += prcstone->quan;
				u.practicantstones -= prcstone->quan;
				useupall(prcstone);
				if (!u.practicantstones && !u.practicantarrows && !u.practicantpenalty) {
					u.practicanttime = 0;
					u.practicantseverity = 0;
					You("no longer have a fine to pay.");
				}

			}
			/* do you still have a stone penalty? let's check whether you have another stack */
			if (u.practicantstones) goto findmorestones;
		}
		if (stonespaid) {
			You("paid %d stones.", stonespaid);
		}
		if (u.practicantstones) You("still need to pay %d more stones to %s in %d turns.", u.practicantstones, noroelaname(), u.practicanttime);
	}

	if (u.practicantarrows) {
		register struct obj *prcarrow;
findmorearrows:
		prcarrow = carrying(ORCISH_ARROW);
		if (!prcarrow) prcarrow = carrying(ARROW);
		if (!prcarrow) prcarrow = carrying(PAPER_ARROW);
		if (!prcarrow) prcarrow = carrying(ODOR_SHOT);
		if (!prcarrow) prcarrow = carrying(FORBIDDEN_ARROW);
		if (!prcarrow) prcarrow = carrying(SILVER_ARROW);
		if (!prcarrow) prcarrow = carrying(ELVEN_ARROW);
		if (!prcarrow) prcarrow = carrying(DARK_ELVEN_ARROW);
		if (!prcarrow) prcarrow = carrying(YA);
		if (!prcarrow) prcarrow = carrying(DROVEN_ARROW);
		if (!prcarrow) prcarrow = carrying(GOLDEN_ARROW);
		if (!prcarrow) prcarrow = carrying(ANCIENT_ARROW);
		if (!prcarrow) prcarrow = carrying(BRONZE_ARROW);
		if (!prcarrow) prcarrow = carrying(WONDER_ARROW);
		if (prcarrow) {
			if (prcarrow->quan > u.practicantarrows) {
				arrowspaid += u.practicantarrows;
				prcarrow->quan -= u.practicantarrows;
				prcarrow->owt = weight(prcarrow);
				u.practicantarrows = 0;
				if (!u.practicantstones && !u.practicantarrows && !u.practicantpenalty) {
					u.practicanttime = 0;
					u.practicantseverity = 0;
					You("no longer have a fine to pay.");
				}
			} else {
				arrowspaid += prcarrow->quan;
				u.practicantarrows -= prcarrow->quan;
				useupall(prcarrow);
				if (!u.practicantstones && !u.practicantarrows && !u.practicantpenalty) {
					u.practicanttime = 0;
					u.practicantseverity = 0;
					You("no longer have a fine to pay.");
				}

			}
			/* do you still have an arrow penalty? let's check whether you have another stack */
			if (u.practicantarrows) goto findmorearrows;
		}
		if (arrowspaid) {
			You("paid %d arrows.", arrowspaid);
		}
		if (u.practicantarrows) You("still need to pay %d more arrows to %s in %d turns.", u.practicantarrows, noroelaname(), u.practicanttime);
	}
}

void
playerbleed(xtime)
long xtime;
{
	if (uleft && uleft->oartifact == ART_JANA_S_DIMINISHER && !rn2(100)) {
		u.youaredead = 1;
		pline("NETHACK caused a General Protection Fault at address 0010:50E8.");
		killer_format = KILLED_BY;
		killer = "Jana's diminisher";
		done(DIED);
		u.youaredead = 0;
	}
	if (uright && uright->oartifact == ART_JANA_S_DIMINISHER && !rn2(100)) {
		u.youaredead = 1;
		pline("NETHACK caused a General Protection Fault at address 0010:50E8.");
		killer_format = KILLED_BY;
		killer = "Jana's diminisher";
		done(DIED);
		u.youaredead = 0;
	}

	PlayerBleeds += xtime;
	if (PlayerBleeds > 100) You("have a hemorrhage!");
	else if (PlayerBleeds > 50) You("are bleeding profusely!");
	else pline("You're bleeding!");
	flags.botl = TRUE;
}

void
make_confused(xtime,talk)
long xtime;
boolean talk;
{
	long old = HConfusion;

	if (!xtime && old) {
		if (talk && (HeavyConfusion < 2) )
		    You_feel("less %s now.",
			FunnyHallu ? "trippy" : "confused");
	}
	if ((xtime && !old) || (!xtime && old)) flags.botl = TRUE;

	if (xtime && (uarmh && itemhasappearance(uarmh, APP_TWISTED_VISOR_HELMET))) {
		if (xtime > HConfusion) xtime += ((xtime - HConfusion) * 5);
	}

	set_itimeout(&HConfusion, xtime);
	if (xtime && !rn2(1000)) {
		pline(FunnyHallu ? "Huh? Who? Where? What? Is something going on?" : "You're badly confused!");
		set_itimeout(&HeavyConfusion, xtime);
	}
	if (xtime && (StatusTrapProblem || u.uprops[STATUS_FAILURE].extrinsic || have_statusstone()) ) set_itimeout(&HeavyConfusion, xtime);
	if (xtime && uarmf && uarmf->oartifact == ART_KRISTIN_S_NOBILITY) set_itimeout(&HeavyConfusion, xtime);
}

void
make_stunned(xtime,talk)
long xtime;
boolean talk;
{
	long old = HStun;

	if (!xtime && old) {
		if (talk && (HeavyStunned < 2) )
		    You_feel("%s now.",
			FunnyHallu ? "less wobbly" : "a bit steadier");
	}
	if (xtime && !old) {
		if (talk) {
			if (u.usteed)
				You("wobble in the saddle.");
			else
			You("%s...", stagger(youmonst.data, "stagger"));
		}
	}
	if ((!xtime && old) || (xtime && !old)) flags.botl = TRUE;

	set_itimeout(&HStun, xtime);
	if (xtime && !rn2(1000)) {
		pline(FunnyHallu ? "It's all wobbly! The world keeps on turning and spinning around..." : "You're badly staggering!");
		set_itimeout(&HeavyStunned, xtime);
	}
	if (xtime && (StatusTrapProblem || u.uprops[STATUS_FAILURE].extrinsic || have_statusstone()) ) set_itimeout(&HeavyStunned, xtime);
}

void
make_numbed(xtime,talk)
long xtime;
boolean talk;
{
	long old = HNumbed;

	if (!xtime && old) {
		if (talk && (HeavyNumbed < 2) )
		    You_feel("%s.",
			FunnyHallu ? "numbed yer" : "your numbness fading out");
	}
	if (xtime && !old) {
		if (talk) {
			pline(FunnyHallu ? "You feel numbed! Can't do!" : "You feel numbed!");
		}
	}
	if ((!xtime && old) || (xtime && !old)) flags.botl = TRUE;

	set_itimeout(&HNumbed, xtime);
	if (xtime && !rn2(1000)) {
		pline(FunnyHallu ? "You can't move! Okay, you can, but it's very difficult..." : "You feel badly numbed!");
		set_itimeout(&HeavyNumbed, xtime);
	}
	if (xtime && (StatusTrapProblem || u.uprops[STATUS_FAILURE].extrinsic || have_statusstone()) ) set_itimeout(&HeavyNumbed, xtime);
}

void
make_feared(xtime,talk)
long xtime;
boolean talk;
{
	long old = HFeared;

	if (!xtime && old) {
		if (talk && (HeavyFeared < 2) )
		    pline("%s",
			FunnyHallu ? "You're ready to rumble again!" : "You're no longer afraid.");
	}

	if (xtime && old) {
		if (talk)
		    You_feel("%s",
			FunnyHallu ? "that you just soiled yourself. Crap, where's a toilet if you need one?" : "even more afraid than before!");
	}
	if (xtime && !old) {
		if (talk) {
			pline(FunnyHallu ? "Suddenly you fear the dungeon around you is going to collapse. Quick! Where is the nearest exit?" : (Role_if(PM_PIRATE) || Role_if(PM_KORSAIR) || (uwep && uwep->oartifact == ART_ARRRRRR_MATEY) ) ? "Ye're lily livered." : "You are stricken with fear!");
		}
	}
	if ((!xtime && old) || (xtime && !old)) flags.botl = TRUE;

	set_itimeout(&HFeared, xtime);
	if (xtime && !rn2(1000)) {
		pline(FunnyHallu ? "ARRRRRGH! HELP! THERE'S A CRAZY AXE-SWINGING MURDERER CHASING AFTER YOU! RUN!!!" : "You're trembling heavily!");
		set_itimeout(&HeavyFeared, xtime);
	}
	if (xtime && (StatusTrapProblem || u.uprops[STATUS_FAILURE].extrinsic || have_statusstone()) ) set_itimeout(&HeavyFeared, xtime);
}

void
make_frozen(xtime,talk)
long xtime;
boolean talk;
{
	long old = HFrozen;

	if (!xtime && old) {
		if (talk && (HeavyFrozen < 2) )
		    pline(FunnyHallu ? "Aww... the tasty-looking ice disappeared." : "You are defrosted.");
	}
	if (xtime && !old) {
		if (talk) {
			pline(FunnyHallu ? "You feel cooling! ??D ??D Y???X!" : "You are frozen solid!");
		}
	}
	if ((!xtime && old) || (xtime && !old)) flags.botl = TRUE;

	if (xtime && Burned) make_burned(0L, TRUE);

	set_itimeout(&HFrozen, xtime);
	if (xtime && !rn2(1000)) {
		pline(FunnyHallu ? "So many ice-cream cones, and they're all supposed to belong to you... let's eat!" : "The ice is really freezing you rigid!");
		set_itimeout(&HeavyFrozen, xtime);
	}
	if (xtime && (StatusTrapProblem || u.uprops[STATUS_FAILURE].extrinsic || have_statusstone()) ) set_itimeout(&HeavyFrozen, xtime);
}

/* Burn and freezing cancel each other out. --Amy */

void
make_burned(xtime,talk)
long xtime;
boolean talk;
{
	long old = HBurned;

	if (!xtime && old) {
		if (talk && (HeavyBurned < 2) )
		    pline(FunnyHallu ? "Oh no, someone put out the fire!" : "Your burns disappear.");
	}
	if (xtime && !old) {
		if (talk) {
			pline(FunnyHallu ? "You're on FIRE! Oh yeah, baby!" : "You were burned!");
		}
	}
	if ((!xtime && old) || (xtime && !old)) flags.botl = TRUE;

	if (xtime && Frozen) make_frozen(0L, TRUE); 
	if (xtime) burn_away_slime();

	set_itimeout(&HBurned, xtime);
	if (xtime && !rn2(1000)) {
		pline(FunnyHallu ? "Uhh... the fire's getting a little bit too hot, even for your tastes!" : "You're badly burned!");
		set_itimeout(&HeavyBurned, xtime);
	}
	if (xtime && (StatusTrapProblem || (uarmf && uarmf->oartifact == ART_VERA_S_FREEZER) || u.uprops[STATUS_FAILURE].extrinsic || have_statusstone()) ) set_itimeout(&HeavyBurned, xtime);
}

void
make_dimmed(xtime,talk)
long xtime;
boolean talk;
{
	long old = HDimmed;

	if (!xtime && old) {
		if (talk && (HeavyDimmed < 2) )
		    pline(FunnyHallu ? "Whew, your marriage might be saved after all." : "You are no longer dimmed.");
	}
	if (xtime && !old) {
		if (talk) {
			pline(FunnyHallu ? "You feel worried about your marriage!" : "You were dimmed!");
		}
	}
	if ((!xtime && old) || (xtime && !old)) flags.botl = TRUE;

	set_itimeout(&HDimmed, xtime);
	if (xtime && !rn2(1000)) {
		pline(FunnyHallu ? "Life has no more meaning. Your wife has run away, your children are dead and people are setting fire to your home right now." : "You're badly dimmed!");
		set_itimeout(&HeavyDimmed, xtime);
	}
	if (xtime && (StatusTrapProblem || u.uprops[STATUS_FAILURE].extrinsic || have_statusstone()) ) set_itimeout(&HeavyDimmed, xtime);
}

void
make_sick(xtime, cause, talk, type)
long xtime;
const char *cause;	/* sickness cause */
boolean talk;
int type;
{
	long old = Sick;

	if (xtime > 0L) {
	    if (IntSick_resistance || (ExtSick_resistance && rn2(20)) ) return;

		if (uarmh && uarmh->oartifact == ART_WHY_NOT_DO_THE_REAL_THING && rn2(4)) {
			pline("Your helmet prevents the sickness from affecting you!");
			return;
		}

		/* Copper has fungicidal properties and therefore also sometimes kills the microorganisms that make you sick --Amy */
		if (uarm && objects[uarm->otyp].oc_material == MT_COPPER && !rn2(10)) {
			pline("Your copper armor prevents you from becoming %ssick!", old ? "even more " : "");
			return;
		}
		if (uarmc && objects[uarmc->otyp].oc_material == MT_COPPER && !rn2(10)) {
			pline("Your copper cloak prevents you from becoming %ssick!", old ? "even more " : "");
			return;
		}
		if (uarmh && objects[uarmh->otyp].oc_material == MT_COPPER && !rn2(10)) {
			pline("Your copper helmet prevents you from becoming %ssick!", old ? "even more " : "");
			return;
		}
		if (uarms && objects[uarms->otyp].oc_material == MT_COPPER && !rn2(10)) {
			pline("Your copper shield prevents you from becoming %ssick!", old ? "even more " : "");
			return;
		}
		if (uarmg && objects[uarmg->otyp].oc_material == MT_COPPER && !rn2(10)) {
			pline("Your copper pair of gauntlets prevents you from becoming %ssick!", old ? "even more " : "");
			return;
		}
		if (uarmf && objects[uarmf->otyp].oc_material == MT_COPPER && !rn2(10)) {
			pline("Your copper pair of boots prevents you from becoming %ssick!", old ? "even more " : "");
			return;
		}
		if (uarmu && objects[uarmu->otyp].oc_material == MT_COPPER && !rn2(10)) {
			pline("Your copper shirt prevents you from becoming %ssick!", old ? "even more " : "");
			return;
		}
		if (uamul && objects[uamul->otyp].oc_material == MT_COPPER && !rn2(10)) {
			pline("Your copper amulet prevents you from becoming %ssick!", old ? "even more " : "");
			return;
		}
		if (uimplant && objects[uimplant->otyp].oc_material == MT_COPPER && !rn2(10)) {
			pline("Your copper implant prevents you from becoming %ssick!", old ? "even more " : "");
			return;
		}
		if (uleft && objects[uleft->otyp].oc_material == MT_COPPER && !rn2(10)) {
			pline("Your copper left ring prevents you from becoming %ssick!", old ? "even more " : "");
			return;
		}
		if (uright && objects[uright->otyp].oc_material == MT_COPPER && !rn2(10)) {
			pline("Your copper right ring prevents you from becoming %ssick!", old ? "even more " : "");
			return;
		}
		if (ublindf && objects[ublindf->otyp].oc_material == MT_COPPER && !rn2(10)) {
			pline("Your copper blindfold prevents you from becoming %ssick!", old ? "even more " : "");
			return;
		}

		if (rn2(4) && uarmc && itemhasappearance(uarmc, APP_INALISH_CLOAK) ) {
			pline("Your inalish cloak prevents you from becoming %ssick!", old ? "even more " : "");
			return;
		}
		if (!rn2(3) && uarmc && itemhasappearance(uarmc, APP_PURE_CLOAK) ) {
			pline("Your pure cloak prevents you from becoming %ssick!", old ? "even more " : "");
			return;
		}

	    if (!old) {
		/* newly sick */
		You_feel(Role_if(PM_PIRATE) ? "poxy." : Role_if(PM_KORSAIR) ? "poxy." : (uwep && uwep->oartifact == ART_ARRRRRR_MATEY) ? "poxy." : "deathly sick.");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    } else {
		/* already sick */
		if (talk) You_feel("%s worse.",
			      rn2(2) ? "much" : "even");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    }

	    if (Race_if(PM_EROSATOR)) {
		xtime /= 3;
		if (xtime < 1) xtime = 1;
	    }

	    set_itimeout(&Sick, xtime);
	    if (Sickopathy) pline("You have %ld turns to live.", Sick);
	    if (type == SICK_VOMITABLE) u.cnd_sickfoodpois++;
	    else u.cnd_sickillness++;
	    u.usick_type |= type;
	    flags.botl = TRUE;
		stop_occupation();
	} else if (old && (type & u.usick_type)) {
	    /* was sick, now not */
	    u.usick_type &= ~type;
	    if (u.usick_type) { /* only partly cured */
		if (talk) You_feel("somewhat better.");
		set_itimeout(&Sick, Sick * 2); /* approximation */
	    } else {
		if (talk) pline(FunnyHallu ? "That cured your overdose!" : "What a relief!");
		Sick = 0L;		/* set_itimeout(&Sick, 0L) */
	    }
	    flags.botl = TRUE;
	}

	if (Sick) {
	    exercise(A_CON, FALSE);
	    if (cause) {
		(void) strncpy(u.usick_cause, cause, sizeof(u.usick_cause));
		u.usick_cause[sizeof(u.usick_cause)-1] = 0;
		}
	    else
		u.usick_cause[0] = 0;
	} else
	    u.usick_cause[0] = 0;
}

void
make_slimed(xtime)
long xtime;
{
	if (Race_if(PM_EROSATOR)) {
		xtime /= 4;
	}
	if (Slimed > 0 && Slimed < xtime) {
		impossible("make_slimed called with too long time!");
		return; /* don't prolong slimed time! --Amy */
	}
	u.cnd_slimingcount++;
	Slimed = xtime;
	flags.botl = 1;
}

void
make_vomiting(xtime, talk)
long xtime;
boolean talk;
{
	long old = Vomiting;

	if(!xtime && old)
	    if(talk) You_feel("much less nauseated now.");

	set_itimeout(&Vomiting, xtime);
}

static const char vismsg[] = "vision seems to %s for a moment but is %s now.";
static const char eyemsg[] = "%s momentarily %s.";

void
make_blinded(xtime, talk)
long xtime;
boolean talk;
{
	long old = Blinded;
	boolean u_could_see, can_see_now;
	int eyecnt;
	char buf[BUFSZ];

	/* we need to probe ahead in case the Eyes of the Overworld
	   are or will be overriding blindness */
	u_could_see = !Blind;
	Blinded = xtime ? 1L : 0L;
	can_see_now = !Blind;
	Blinded = old;		/* restore */

	if (u.usleep) talk = FALSE;

	if (can_see_now && !u_could_see) {	/* regaining sight */
	    if (talk) {
		if (FunnyHallu)
		    pline("Far out!  Everything is all cosmic again!");
		else
		    You("can see again.");
	    }
	} else if (old && !xtime) {
	    /* clearing temporary blindness without toggling blindness */
	    if (talk) {
		if (!haseyes(youmonst.data)) {
		    strange_feeling((struct obj *)0, (char *)0);
		} else if (Blindfolded) {
		    strcpy(buf, body_part(EYE));
		    eyecnt = eyecount(youmonst.data);
		    Your(eyemsg, (eyecnt == 1) ? buf : makeplural(buf),
			 (eyecnt == 1) ? "itches" : "itch");
		} else {	/* Eyes of the Overworld */
		    Your(vismsg, "brighten",
			 FunnyHallu ? "sadder" : "normal");
		}
	    }
	}

	if (u_could_see && !can_see_now) {	/* losing sight */
	    if (talk) {
		if (FunnyHallu)
		    pline("Oh, bummer!  Everything is dark!  Help!");
		else
		    pline("A cloud of darkness falls upon you.");
	    }
	    /* Before the hero goes blind, set the ball&chain variables. */
	    if (Punished) set_bc(0);
	} else if (!old && xtime) {
	    /* setting temporary blindness without toggling blindness */
	    if (talk) {
		if (!haseyes(youmonst.data)) {
		    strange_feeling((struct obj *)0, (char *)0);
		} else if (Blindfolded) {
		    strcpy(buf, body_part(EYE));
		    eyecnt = eyecount(youmonst.data);
		    Your(eyemsg, (eyecnt == 1) ? buf : makeplural(buf),
			 (eyecnt == 1) ? "twitches" : "twitch");
		} else {	/* Eyes of the Overworld */
		    Your(vismsg, "dim",
			 FunnyHallu ? "happier" : "normal");
		}
	    }
	}

	if (xtime < 0) {
		impossible("make_blinded called with negative blindness counter %ld", xtime);
		xtime = 0;
	}

	set_itimeout(&Blinded, xtime);
	if (xtime && !rn2(1000)) {
		pline(FunnyHallu ? "Aww, even the images in your mind have disappeared!" : "The darkness seems definite and impenetrable!");
		set_itimeout(&HeavyBlind, xtime);
	}
	if (xtime && (StatusTrapProblem || u.uprops[STATUS_FAILURE].extrinsic || have_statusstone()) ) set_itimeout(&HeavyBlind, xtime);

	if (u_could_see ^ can_see_now) {  /* one or the other but not both */
	    flags.botl = 1;
	    vision_full_recalc = 1;	/* blindness just got toggled */
	    if (Blind_telepat || Infravision) see_monsters();
	}
}

/* Helper functions for youprop.h to reduce macro overloading --Amy */
boolean
playerwearshighheels()
{

	if (!uarmf) return FALSE;
	/* uarmf is definitely defined now */

	if (uwep && uwep->oartifact == ART_GAYGUN) return TRUE;

	if ((uarmf->otyp == WEDGE_SANDALS) || (uarmf->otyp == EVELINE_WEDGE_SANDALS) || (uarmf->otyp == BUM_BUM_BOOTS) || (uarmf->otyp == MADELEINE_PLATEAU_BOOTS) || (uarmf->otyp == KATHARINA_PLATFORM_BOOTS) || (uarmf->otyp == FEMININE_PUMPS) || (uarmf->otyp == LEATHER_PEEP_TOES) || (uarmf->otyp == WENDY_LEATHER_PUMPS) || (uarmf->otyp == NELLY_LADY_PUMPS) || (uarmf->otyp == SANDRA_COMBAT_BOOTS) || (uarmf->otyp == CLAUDIA_WOODEN_SANDALS) || (uarmf && RngeIrregularity) || (uarmf->otyp == HIPPIE_HEELS) || (uarmf->otyp == SELF_WILLED_HEELS) || (uarmf->oartifact == ART_UPWARD_HEELS) || (uarmf->oartifact == ART_ABSURD_HEELED_TILESET) || (uarmf->oartifact == ART_SWARM_SOFT_HIGH_HEELS) || (uarmf->oartifact == ART_KATIE_MELUA_S_FEMALE_WEAPO) || (uarmf->oartifact == ART_ARVOGENIA_S_HIGH_HEELSES) || (uarmf->oartifact == ART_MANUELA_S_UNKNOWN_HEELS) || (uarmf->oartifact == ART_RITA_S_TENDER_STILETTOS) || (uarmf->oartifact == ART_ELENETTES) || (uarmf->oartifact == ART_ANASTASIA_S_UNEXPECTED_ABI) || (uarmf->oartifact == ART_HIGH_HEELED_HUG) || (Role_if(PM_BINDER) && uarmf->oartifact == ART_BINDER_CRASH) || (uarmf->otyp == PET_STOMPING_PLATFORM_BOOTS) || (uarmf->otyp == SENTIENT_HIGH_HEELED_SHOES) || (uarmf->otyp == ATSUZOKO_BOOTS) || (uarmf->otyp == COMBAT_STILETTOS) || (uarmf->otyp == FEMMY_STILETTO_BOOTS) || (uarmf->otyp == JUEN_PEEP_TOES) || (uarmf->otyp == JULIETTA_PEEP_TOES) || (uarmf->otyp == ITALIAN_HEELS) || (uarmf->otyp == LADY_BOOTS) || (uarmf->otyp == NADJA_BUCKLED_LADY_SHOES) || (uarmf->otyp == ELENA_COMBAT_BOOTS) || (uarmf->otyp == THAI_COMBAT_BOOTS) || (uarmf->otyp == MELTEM_COMBAT_BOOTS) || (uarmf->otyp == STILETTO_SANDALS) || (uarmf->otyp == NATALJE_BLOCK_HEEL_SANDALS) || (uarmf->otyp == HIGH_STILETTOS) || (uarmf->otyp == HIGH_HEELED_SKIERS) || (uarmf->otyp == UNFAIR_STILETTOS) || (uarmf->otyp == COVETED_BOOTS) || (uarmf->otyp == SKY_HIGH_HEELS) || (uarmf->otyp == RED_SPELL_HEELS) || (uarmf->otyp == DESTRUCTIVE_HEELS) || (uarmf->otyp == LONG_POINTY_HEELS) || (uarmf->otyp == VIOLET_BEAUTY_HEELS) || (uarmf->otyp == AUTOSCOOTER_HEELS) || (uarmf->otyp == SINFUL_HEELS) || (uarmf->otyp == KILLER_HEELS) || (uarmf->otyp == HIGH_SCORING_HEELS) || (uarmf->otyp == KRISTIN_COMBAT_BOOTS) || (uarmf->otyp == RUEA_COMBAT_BOOTS) || (uarmf->otyp == DORA_COMBAT_BOOTS) || (uarmf->otyp == JETTE_COMBAT_BOOTS) || (uarmf->otyp == SING_PLATFORM_BOOTS) || (uarmf->otyp == VICTORIA_COMBAT_BOOTS) || (uarmf->otyp == MELISSA_WEDGE_BOOTS) || (uarmf->otyp == ANITA_LADY_PUMPS) || (uarmf->otyp == HENRIETTA_COMBAT_BOOTS) || (uarmf->otyp == VERENA_STILETTO_SANDALS)) return TRUE;

	if (OBJ_DESCR(objects[uarmf->otyp])) {

		if (itemhasappearance(uarmf, APP_IRREGULAR_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_PRINTED_WEDGES)) return TRUE;
		if (itemhasappearance(uarmf, APP_MARY_JANES)) return TRUE;
		if (itemhasappearance(uarmf, APP_BALLET_HEELS)) return TRUE;
		if (itemhasappearance(uarmf, APP_NOBLE_SANDALS)) return TRUE;
		if (itemhasappearance(uarmf, APP_IMAGINARY_HEELS)) return TRUE;
		if (itemhasappearance(uarmf, APP_WEDGE_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_PLOF_HEELS)) return TRUE;
		if (itemhasappearance(uarmf, APP_WINTER_STILETTOS)) return TRUE;
		if (itemhasappearance(uarmf, APP_CLUNKY_HEELS)) return TRUE;
		if (itemhasappearance(uarmf, APP_PRINCESS_PUMPS)) return TRUE;
		if (itemhasappearance(uarmf, APP_ANKLE_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_SHADOWY_HEELS)) return TRUE;
		if (itemhasappearance(uarmf, APP_BLOCK_HEELED_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_DYKE_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_CUDDLE_CLOTH_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_BEAUTIFUL_HEELS)) return TRUE;
		if (itemhasappearance(uarmf, APP_EROTIC_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_ORGASM_PUMPS)) return TRUE;
		if (itemhasappearance(uarmf, APP_SPUTA_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_FEMMY_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_SHARP_EDGED_SANDALS)) return TRUE;
		if (itemhasappearance(uarmf, APP_PRINTED_SANDALS)) return TRUE;
		if (itemhasappearance(uarmf, APP_SKI_HEELS)) return TRUE;
		if (itemhasappearance(uarmf, APP_FEELGOOD_HEELS)) return TRUE;
		if (itemhasappearance(uarmf, APP_FETISH_HEELS)) return TRUE;
		if (itemhasappearance(uarmf, APP_ANKLE_STRAP_SANDALS)) return TRUE;
		if (itemhasappearance(uarmf, APP_FILIGREE_STILETTOS)) return TRUE;
		if (itemhasappearance(uarmf, APP_VELVET_PUMPS)) return TRUE;
		if (itemhasappearance(uarmf, APP_BONE_HEELS)) return TRUE;
		if (itemhasappearance(uarmf, APP_BUFFALO_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_LOLITA_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_WEAPON_LIGHT_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_RADIANT_HEELS)) return TRUE;
		if (itemhasappearance(uarmf, APP_WEDGE_SNEAKERS)) return TRUE;
		if (itemhasappearance(uarmf, APP_SEXY_HEELS)) return TRUE;
		if (itemhasappearance(uarmf, APP_STROKING_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_GENERIC_HIGH_HEELS)) return TRUE;
		if (itemhasappearance(uarmf, APP_FAILED_SHOES)) return TRUE;
		if (itemhasappearance(uarmf, APP_OPERA_PUMPS)) return TRUE;

	}

	if (uwep && uwep->oartifact == ART_DAMN_SKI_WEDGE && uarmf) return TRUE;

	return FALSE;

}

void
buttlovertrigger()
{
	u.negativeprotection++;
	if (evilfriday && u.ublessed > 0) {
		u.ublessed -= 1;
		if (u.ublessed < 0) u.ublessed = 0;
	}
	You_feel("humiliated!");
}

void
sjwtrigger()
{
	int fearduration = rnz(30 + (monster_difficulty() * 3));
	make_feared(HFeared + fearduration, TRUE);
	u.uprops[DEAC_FEAR_RES].intrinsic += fearduration;
	pline("omg sexism");
}

void
emerafrenzy()
{
	pline("You're getting angry!");

	if (u.berserktime) {
	    if (!obsidianprotection()) switch (rn2(11)) {
	    case 0: make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON),20), "emera sickness", TRUE, SICK_NONVOMITABLE);
		    break;
	    case 1: make_blinded(Blinded + 25, TRUE);
		    break;
	    case 2: if (!Confusion)
			You("suddenly feel %s.",
			    FunnyHallu ? "trippy" : "confused");
		    make_confused(HConfusion + 25, TRUE);
		    break;
	    case 3: make_stunned(HStun + 25, TRUE);
		    break;
	    case 4: make_numbed(HNumbed + 25, TRUE);
		    break;
	    case 5: make_frozen(HFrozen + 25, TRUE);
		    break;
	    case 6: make_burned(HBurned + 25, TRUE);
		    break;
	    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
		    break;
	    case 8: (void) make_hallucinated(HHallucination + 25, TRUE, 0L);
		    break;
	    case 9: make_feared(HFeared + 25, TRUE);
		    break;
	    case 10: make_dimmed(HDimmed + 25, TRUE);
		    break;
	    }

	} else u.berserktime = 25;

}

boolean
automore_active()
{
	if (AutomaticMorePrompt) {
		u.automorefuckthisshit = TRUE;
		return TRUE;
	}
	else {
		u.automorefuckthisshit = FALSE;
		return FALSE;
	}
}

boolean
ishighheeled(otmp)
struct obj *otmp;
{
	if (!otmp) return FALSE;

	if ((otmp)->otyp == WEDGE_SANDALS || (otmp)->otyp == EVELINE_WEDGE_SANDALS || (otmp)->otyp == MADELEINE_PLATEAU_BOOTS || (otmp)->otyp == KATHARINA_PLATFORM_BOOTS || (otmp)->otyp == BUM_BUM_BOOTS || (otmp)->otyp == FEMININE_PUMPS || (otmp)->otyp == LEATHER_PEEP_TOES || (otmp)->otyp == WENDY_LEATHER_PUMPS || (otmp)->otyp == NELLY_LADY_PUMPS || (otmp)->otyp == CLAUDIA_WOODEN_SANDALS || (otmp)->otyp == SANDRA_COMBAT_BOOTS || (otmp)->otyp == HIPPIE_HEELS || (otmp)->otyp == SELF_WILLED_HEELS || (otmp)->otyp == PET_STOMPING_PLATFORM_BOOTS || (otmp)->otyp == SENTIENT_HIGH_HEELED_SHOES || (otmp)->otyp == ATSUZOKO_BOOTS || (otmp)->otyp == COMBAT_STILETTOS || (otmp)->otyp == JUEN_PEEP_TOES || (otmp)->otyp == JULIETTA_PEEP_TOES || (otmp)->otyp == FEMMY_STILETTO_BOOTS || (otmp)->otyp == LADY_BOOTS || (otmp)->otyp == ELENA_COMBAT_BOOTS || (otmp)->otyp == NADJA_BUCKLED_LADY_SHOES || (otmp)->otyp == THAI_COMBAT_BOOTS || (otmp)->otyp == MELTEM_COMBAT_BOOTS || (otmp)->otyp == NATALJE_BLOCK_HEEL_SANDALS || (otmp)->otyp == ITALIAN_HEELS || (otmp)->otyp == STILETTO_SANDALS || (otmp)->otyp == HIGH_STILETTOS || (otmp)->otyp == HIGH_HEELED_SKIERS || (otmp)->otyp == UNFAIR_STILETTOS || (otmp)->otyp == COVETED_BOOTS || (otmp)->otyp == SKY_HIGH_HEELS || (otmp)->otyp == RED_SPELL_HEELS || (otmp)->otyp == VIOLET_BEAUTY_HEELS || (otmp)->otyp == AUTOSCOOTER_HEELS || (otmp)->otyp == DESTRUCTIVE_HEELS || (otmp)->otyp == SINFUL_HEELS || (otmp)->otyp == LONG_POINTY_HEELS || (otmp)->otyp == KILLER_HEELS || (otmp)->otyp == HIGH_SCORING_HEELS || (otmp)->otyp == KRISTIN_COMBAT_BOOTS || (otmp)->otyp == RUEA_COMBAT_BOOTS || (otmp)->otyp == DORA_COMBAT_BOOTS || (otmp)->otyp == JETTE_COMBAT_BOOTS || (otmp)->otyp == SING_PLATFORM_BOOTS || (otmp)->otyp == VICTORIA_COMBAT_BOOTS || (otmp)->otyp == MELISSA_WEDGE_BOOTS || (otmp)->otyp == ANITA_LADY_PUMPS || (otmp)->otyp == HENRIETTA_COMBAT_BOOTS || (otmp)->otyp == VERENA_STILETTO_SANDALS) return TRUE;

	if (OBJ_DESCR(objects[otmp->otyp])) {
		if (itemhasappearance(otmp, APP_IRREGULAR_BOOTS)) return TRUE;
		if (itemhasappearance(otmp, APP_PRINTED_WEDGES)) return TRUE;
		if (itemhasappearance(otmp, APP_MARY_JANES)) return TRUE;
		if (itemhasappearance(otmp, APP_BALLET_HEELS)) return TRUE;
		if (itemhasappearance(otmp, APP_NOBLE_SANDALS)) return TRUE;
		if (itemhasappearance(otmp, APP_IMAGINARY_HEELS)) return TRUE;
		if (itemhasappearance(otmp, APP_WEDGE_BOOTS)) return TRUE;
		if (itemhasappearance(otmp, APP_PLOF_HEELS)) return TRUE;
		if (itemhasappearance(otmp, APP_WINTER_STILETTOS)) return TRUE;
		if (itemhasappearance(otmp, APP_CLUNKY_HEELS)) return TRUE;
		if (itemhasappearance(otmp, APP_PRINCESS_PUMPS)) return TRUE;
		if (itemhasappearance(otmp, APP_ANKLE_BOOTS)) return TRUE;
		if (itemhasappearance(otmp, APP_SHADOWY_HEELS)) return TRUE;
		if (itemhasappearance(otmp, APP_BLOCK_HEELED_BOOTS)) return TRUE;
		if (itemhasappearance(otmp, APP_DYKE_BOOTS)) return TRUE;
		if (itemhasappearance(otmp, APP_CUDDLE_CLOTH_BOOTS)) return TRUE;
		if (itemhasappearance(otmp, APP_BEAUTIFUL_HEELS)) return TRUE;
		if (itemhasappearance(otmp, APP_EROTIC_BOOTS)) return TRUE;
		if (itemhasappearance(otmp, APP_ORGASM_PUMPS)) return TRUE;
		if (itemhasappearance(otmp, APP_SPUTA_BOOTS)) return TRUE;
		if (itemhasappearance(otmp, APP_FEMMY_BOOTS)) return TRUE;
		if (itemhasappearance(otmp, APP_SHARP_EDGED_SANDALS)) return TRUE;
		if (itemhasappearance(otmp, APP_PRINTED_SANDALS)) return TRUE;
		if (itemhasappearance(otmp, APP_SKI_HEELS)) return TRUE;
		if (itemhasappearance(otmp, APP_FEELGOOD_HEELS)) return TRUE;
		if (itemhasappearance(otmp, APP_FETISH_HEELS)) return TRUE;
		if (itemhasappearance(otmp, APP_ANKLE_STRAP_SANDALS)) return TRUE;
		if (itemhasappearance(otmp, APP_FILIGREE_STILETTOS)) return TRUE;
		if (itemhasappearance(otmp, APP_VELVET_PUMPS)) return TRUE;
		if (itemhasappearance(otmp, APP_BONE_HEELS)) return TRUE;
		if (itemhasappearance(otmp, APP_BUFFALO_BOOTS)) return TRUE;
		if (itemhasappearance(otmp, APP_LOLITA_BOOTS)) return TRUE;
		if (itemhasappearance(otmp, APP_WEAPON_LIGHT_BOOTS)) return TRUE;
		if (itemhasappearance(otmp, APP_RADIANT_HEELS)) return TRUE;
		if (itemhasappearance(otmp, APP_WEDGE_SNEAKERS)) return TRUE;
		if (itemhasappearance(otmp, APP_SEXY_HEELS)) return TRUE;
		if (itemhasappearance(otmp, APP_STROKING_BOOTS)) return TRUE;
		if (itemhasappearance(otmp, APP_GENERIC_HIGH_HEELS)) return TRUE;
		if (itemhasappearance(otmp, APP_FAILED_SHOES)) return TRUE;
		if (itemhasappearance(otmp, APP_OPERA_PUMPS)) return TRUE;

	}

	return FALSE;

}

boolean
ishighheeledb(number)
int number;
{
	if (number == WEDGE_SANDALS || number == EVELINE_WEDGE_SANDALS || number == MADELEINE_PLATEAU_BOOTS || number == KATHARINA_PLATFORM_BOOTS || number == BUM_BUM_BOOTS || number == FEMININE_PUMPS || number == LEATHER_PEEP_TOES || number == WENDY_LEATHER_PUMPS || number == NELLY_LADY_PUMPS || number == CLAUDIA_WOODEN_SANDALS || number == SANDRA_COMBAT_BOOTS || number == HIPPIE_HEELS || number == SELF_WILLED_HEELS || number == PET_STOMPING_PLATFORM_BOOTS || number == SENTIENT_HIGH_HEELED_SHOES || number == ATSUZOKO_BOOTS || number == COMBAT_STILETTOS || number == JUEN_PEEP_TOES || number == JULIETTA_PEEP_TOES || number == FEMMY_STILETTO_BOOTS || number == ITALIAN_HEELS || number == LADY_BOOTS || number == ELENA_COMBAT_BOOTS || number == NADJA_BUCKLED_LADY_SHOES || number == THAI_COMBAT_BOOTS || number == MELTEM_COMBAT_BOOTS || number == NATALJE_BLOCK_HEEL_SANDALS || number == STILETTO_SANDALS || number == HIGH_STILETTOS || number == HIGH_HEELED_SKIERS || number == UNFAIR_STILETTOS || number == COVETED_BOOTS || number == SKY_HIGH_HEELS || number == RED_SPELL_HEELS || number == VIOLET_BEAUTY_HEELS || number == AUTOSCOOTER_HEELS || number == DESTRUCTIVE_HEELS || number == SINFUL_HEELS || number == LONG_POINTY_HEELS || number == KILLER_HEELS || number == HIGH_SCORING_HEELS || number == KRISTIN_COMBAT_BOOTS || number == RUEA_COMBAT_BOOTS || number == DORA_COMBAT_BOOTS || number == JETTE_COMBAT_BOOTS || number == SING_PLATFORM_BOOTS || number == VICTORIA_COMBAT_BOOTS || number == MELISSA_WEDGE_BOOTS || number == ANITA_LADY_PUMPS || number == HENRIETTA_COMBAT_BOOTS || number == VERENA_STILETTO_SANDALS) return TRUE;

	if (OBJ_DESCR(objects[number])) {

		if (itemnumwithappearance(number, APP_IRREGULAR_BOOTS)) return TRUE;
		if (itemnumwithappearance(number, APP_PRINTED_WEDGES)) return TRUE;
		if (itemnumwithappearance(number, APP_MARY_JANES)) return TRUE;
		if (itemnumwithappearance(number, APP_BALLET_HEELS)) return TRUE;
		if (itemnumwithappearance(number, APP_NOBLE_SANDALS)) return TRUE;
		if (itemnumwithappearance(number, APP_IMAGINARY_HEELS)) return TRUE;
		if (itemnumwithappearance(number, APP_WEDGE_BOOTS)) return TRUE;
		if (itemnumwithappearance(number, APP_PLOF_HEELS)) return TRUE;
		if (itemnumwithappearance(number, APP_WINTER_STILETTOS)) return TRUE;
		if (itemnumwithappearance(number, APP_CLUNKY_HEELS)) return TRUE;
		if (itemnumwithappearance(number, APP_PRINCESS_PUMPS)) return TRUE;
		if (itemnumwithappearance(number, APP_ANKLE_BOOTS)) return TRUE;
		if (itemnumwithappearance(number, APP_SHADOWY_HEELS)) return TRUE;
		if (itemnumwithappearance(number, APP_BLOCK_HEELED_BOOTS)) return TRUE;
		if (itemnumwithappearance(number, APP_DYKE_BOOTS)) return TRUE;
		if (itemnumwithappearance(number, APP_CUDDLE_CLOTH_BOOTS)) return TRUE;
		if (itemnumwithappearance(number, APP_BEAUTIFUL_HEELS)) return TRUE;
		if (itemnumwithappearance(number, APP_EROTIC_BOOTS)) return TRUE;
		if (itemnumwithappearance(number, APP_ORGASM_PUMPS)) return TRUE;
		if (itemnumwithappearance(number, APP_SPUTA_BOOTS)) return TRUE;
		if (itemnumwithappearance(number, APP_FEMMY_BOOTS)) return TRUE;
		if (itemnumwithappearance(number, APP_SHARP_EDGED_SANDALS)) return TRUE;
		if (itemnumwithappearance(number, APP_PRINTED_SANDALS)) return TRUE;
		if (itemnumwithappearance(number, APP_SKI_HEELS)) return TRUE;
		if (itemnumwithappearance(number, APP_FEELGOOD_HEELS)) return TRUE;
		if (itemnumwithappearance(number, APP_FETISH_HEELS)) return TRUE;
		if (itemnumwithappearance(number, APP_ANKLE_STRAP_SANDALS)) return TRUE;
		if (itemnumwithappearance(number, APP_FILIGREE_STILETTOS)) return TRUE;
		if (itemnumwithappearance(number, APP_VELVET_PUMPS)) return TRUE;
		if (itemnumwithappearance(number, APP_BONE_HEELS)) return TRUE;
		if (itemnumwithappearance(number, APP_BUFFALO_BOOTS)) return TRUE;
		if (itemnumwithappearance(number, APP_LOLITA_BOOTS)) return TRUE;
		if (itemnumwithappearance(number, APP_WEAPON_LIGHT_BOOTS)) return TRUE;
		if (itemnumwithappearance(number, APP_RADIANT_HEELS)) return TRUE;
		if (itemnumwithappearance(number, APP_WEDGE_SNEAKERS)) return TRUE;
		if (itemnumwithappearance(number, APP_SEXY_HEELS)) return TRUE;
		if (itemnumwithappearance(number, APP_STROKING_BOOTS)) return TRUE;
		if (itemnumwithappearance(number, APP_GENERIC_HIGH_HEELS)) return TRUE;
		if (itemnumwithappearance(number, APP_FAILED_SHOES)) return TRUE;
		if (itemnumwithappearance(number, APP_OPERA_PUMPS)) return TRUE;

	}

	return FALSE;

}

boolean
playerwearssexyflats()
{
	/* wedge sneakers are a special case: if they count as wedge heels (i.e. their base type isn't some other heel type),
	 * they count as sexy flats too, otherwise they don't --Amy */

	if (PlayerInHighHeels && !(PlayerInWedgeHeels && uarmf && itemhasappearance(uarmf, APP_WEDGE_SNEAKERS)) && !(tech_inuse(T_HIGH_HEELED_SNEAKERS))) return FALSE;
	if (!uarmf) return FALSE;
	/* uarmf is definitely defined now */

	if ((uarmf->otyp == RUBBER_BOOTS) || (uarmf->otyp == ANASTASIA_DANCING_SHOES) || (uarmf->otyp == YVONNE_GIRL_SNEAKERS) || (uarmf->otyp == ELIF_SNEAKERS) || (uarmf->otyp == LOU_SNEAKERS) || (uarmf->otyp == ALMUT_SNEAKERS) || (uarmf->otyp == KRISTINA_PLATFORM_SNEAKERS) || (uarmf->otyp == KARIN_LADY_SANDALS) || (uarmf->otyp == SNEAKERS) || (uarmf->otyp == MAURAH_HUGGING_BOOTS) || (uarmf->otyp == SARAH_HUGGING_BOOTS) || (uarmf->otyp == ARABELLA_HUGGING_BOOTS) || (uarmf->otyp == LUDGERA_HIKING_BOOTS) || (uarmf->otyp == BRIGHT_CYAN_BEAUTIES) || (uarmf->otyp == DANCING_SHOES) || (uarmf->otyp == JESSICA_LADY_SHOES) || (uarmf->otyp == SOLVEJG_MOCASSINS) || (uarmf->otyp == SWEET_MOCASSINS) || (uarmf->otyp == SOFT_SNEAKERS) || (uarmf->otyp == KATI_GIRL_BOOTS) || (uarmf->otyp == MARLENA_HIKING_BOOTS) || (uarmf->otyp == ROLLER_BLADE) || (uarmf->otyp == JEANETTA_GIRL_BOOTS) || (uarmf->otyp == DIFFICULT_BOOTS) || (uarmf->otyp == AIRSTEP_BOOTS) || (uarmf->otyp == ANNA_HUGGING_BOOTS) || (uarmf->otyp == INA_HUGGING_BOOTS) || (uarmf->otyp == SYNTHETIC_SANDALS)) return TRUE;

	if (OBJ_DESCR(objects[uarmf->otyp])) {

		if (itemhasappearance(uarmf, APP_BUCKLED_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_PERSIAN_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_FLEECY_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_HUGGING_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_GENTLE_SNEAKERS)) return TRUE;
		if (itemhasappearance(uarmf, APP_MARJI_SHOES)) return TRUE;
		if (itemhasappearance(uarmf, APP_RAINBOW_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_WORN_OUT_SNEAKERS)) return TRUE;
		if (itemhasappearance(uarmf, APP_ROLLER_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_PLATFORM_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_PROFILED_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_PLATEAU_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_VELCRO_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_BLUE_SNEAKERS)) return TRUE;
		if (itemhasappearance(uarmf, APP_REFLECTIVE_SLIPPERS)) return TRUE;
		if (itemhasappearance(uarmf, APP_FUNGAL_SANDALS)) return TRUE;
		if (itemhasappearance(uarmf, APP_RED_SNEAKERS)) return TRUE;
		if (itemhasappearance(uarmf, APP_YELLOW_SNEAKERS)) return TRUE;
		if (itemhasappearance(uarmf, APP_PINK_SNEAKERS)) return TRUE;
		if (itemhasappearance(uarmf, APP_CALF_LEATHER_SANDALS)) return TRUE;
		if (itemhasappearance(uarmf, APP_VELCRO_SANDALS)) return TRUE;
		if (itemhasappearance(uarmf, APP_HEROINE_MOCASSINS)) return TRUE;
		if (itemhasappearance(uarmf, APP_HEAP_OF_SHIT_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_KOREAN_SANDALS)) return TRUE;
		if (itemhasappearance(uarmf, APP_GENTLE_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_FLUFFY_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_UNISEX_PUMPS)) return TRUE;
		if (itemhasappearance(uarmf, APP_WOODEN_CLOGS)) return TRUE;
		if (itemhasappearance(uarmf, APP_CYAN_SNEAKERS)) return TRUE;
		if (itemhasappearance(uarmf, APP_REGULAR_SNEAKERS)) return TRUE;
		if (itemhasappearance(uarmf, APP_ELITE_SNEAKERS)) return TRUE;
		if (itemhasappearance(uarmf, APP_FUR_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_WEDGE_SNEAKERS)) return TRUE;

	}

	return FALSE;

}

boolean
maybestilettoheels()
{
	if (!uarmf) return FALSE;
	/* uarmf is definitely defined now */

	if ((uarmf->otyp == LEATHER_PEEP_TOES) || (uarmf->oartifact == ART_ABSURD_HEELED_TILESET) || (uarmf->oartifact == ART_KATIE_MELUA_S_FEMALE_WEAPO) || (uarmf->oartifact == ART_RITA_S_TENDER_STILETTOS) || (Role_if(PM_BINDER) && uarmf->oartifact == ART_BINDER_CRASH) || (uarmf->otyp == SENTIENT_HIGH_HEELED_SHOES) || (uarmf->otyp == LONG_POINTY_HEELS) || (uarmf->otyp == ATSUZOKO_BOOTS) || (uarmf->otyp == COMBAT_STILETTOS) || (uarmf->otyp == JUEN_PEEP_TOES) || (uarmf->otyp == JULIETTA_PEEP_TOES) || (uarmf->otyp == FEMMY_STILETTO_BOOTS) || (uarmf->otyp == ITALIAN_HEELS) || (uarmf->otyp == STILETTO_SANDALS) || (uarmf->otyp == HIGH_STILETTOS) || (uarmf->otyp == UNFAIR_STILETTOS) || (uarmf->otyp == SKY_HIGH_HEELS) || (uarmf->otyp == RED_SPELL_HEELS) || (uarmf->otyp == KILLER_HEELS) || (uarmf->otyp == RUEA_COMBAT_BOOTS) || (uarmf->otyp == ANITA_LADY_PUMPS) || (uarmf->otyp == VERENA_STILETTO_SANDALS)) return TRUE;

	if (OBJ_DESCR(objects[uarmf->otyp])) {

		if (itemhasappearance(uarmf, APP_WINTER_STILETTOS)) return TRUE;
		if (itemhasappearance(uarmf, APP_IMAGINARY_HEELS)) return TRUE;
		if (itemhasappearance(uarmf, APP_BALLET_HEELS)) return TRUE;
		if (itemhasappearance(uarmf, APP_SHARP_EDGED_SANDALS)) return TRUE;
		if (itemhasappearance(uarmf, APP_PRINTED_SANDALS)) return TRUE;
		if (itemhasappearance(uarmf, APP_FETISH_HEELS)) return TRUE;
		if (itemhasappearance(uarmf, APP_ANKLE_STRAP_SANDALS)) return TRUE;
		if (itemhasappearance(uarmf, APP_FILIGREE_STILETTOS)) return TRUE;
		if (itemhasappearance(uarmf, APP_WEAPON_LIGHT_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_FAILED_SHOES)) return TRUE;

	}

	return FALSE;

}

boolean
maybeconeheels()
{
	if (!uarmf) return FALSE;
	/* uarmf is definitely defined now */

	if ((uarmf->otyp == FEMININE_PUMPS) || (uarmf->otyp == WENDY_LEATHER_PUMPS) || (uarmf->otyp == NELLY_LADY_PUMPS) || (uarmf->otyp == CLAUDIA_WOODEN_SANDALS) || (uarmf->otyp == SANDRA_COMBAT_BOOTS) || (uarmf->oartifact == ART_ARVOGENIA_S_HIGH_HEELSES) || (uarmf->oartifact == ART_UPWARD_HEELS) || (uarmf->oartifact == ART_ANASTASIA_S_UNEXPECTED_ABI) || (uarmf->otyp == DESTRUCTIVE_HEELS) || (uarmf->otyp == VIOLET_BEAUTY_HEELS) || (uarmf->otyp == AUTOSCOOTER_HEELS)) return TRUE;

	if (OBJ_DESCR(objects[uarmf->otyp])) {

		if (itemhasappearance(uarmf, APP_ANKLE_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_SHADOWY_HEELS)) return TRUE;
		if (itemhasappearance(uarmf, APP_BEAUTIFUL_HEELS)) return TRUE;
		if (itemhasappearance(uarmf, APP_NOBLE_SANDALS)) return TRUE;
		if (itemhasappearance(uarmf, APP_ORGASM_PUMPS)) return TRUE;
		if (itemhasappearance(uarmf, APP_MARY_JANES)) return TRUE;
		if (itemhasappearance(uarmf, APP_PRINCESS_PUMPS)) return TRUE;
		if (itemhasappearance(uarmf, APP_FEELGOOD_HEELS)) return TRUE;
		if (itemhasappearance(uarmf, APP_FEMMY_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_VELVET_PUMPS)) return TRUE;
		if (itemhasappearance(uarmf, APP_BONE_HEELS)) return TRUE;
		if (itemhasappearance(uarmf, APP_SEXY_HEELS)) return TRUE;
		if (itemhasappearance(uarmf, APP_OPERA_PUMPS)) return TRUE;

	}

	return FALSE;

}

boolean
maybeblockheels()
{
	if (!uarmf) return FALSE;
	/* uarmf is definitely defined now */

	if ((uarmf->otyp == HIPPIE_HEELS) || (uarmf->oartifact == ART_SWARM_SOFT_HIGH_HEELS) || (uarmf->otyp == SELF_WILLED_HEELS) || (uarmf->oartifact == ART_MANUELA_S_UNKNOWN_HEELS) || (uarmf->oartifact == ART_HIGH_HEELED_HUG) || (uarmf->otyp == COVETED_BOOTS) || (uarmf->otyp == SINFUL_HEELS) || (uarmf->otyp == LADY_BOOTS) || (uarmf->otyp == ELENA_COMBAT_BOOTS) || (uarmf->otyp == THAI_COMBAT_BOOTS) || (uarmf->otyp == MELTEM_COMBAT_BOOTS) || (uarmf->otyp == NATALJE_BLOCK_HEEL_SANDALS) || (uarmf->otyp == NADJA_BUCKLED_LADY_SHOES) || (uarmf->otyp == HIGH_SCORING_HEELS) || (uarmf->otyp == KRISTIN_COMBAT_BOOTS) || (uarmf->otyp == DORA_COMBAT_BOOTS) || (uarmf->otyp == JETTE_COMBAT_BOOTS) || (uarmf->otyp == VICTORIA_COMBAT_BOOTS) || (uarmf->otyp == HENRIETTA_COMBAT_BOOTS)) return TRUE;

	if (OBJ_DESCR(objects[uarmf->otyp])) {

		if (itemhasappearance(uarmf, APP_CLUNKY_HEELS)) return TRUE;
		if (itemhasappearance(uarmf, APP_BLOCK_HEELED_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_DYKE_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_CUDDLE_CLOTH_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_PLOF_HEELS)) return TRUE;
		if (itemhasappearance(uarmf, APP_EROTIC_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_SPUTA_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_LOLITA_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_STROKING_BOOTS)) return TRUE;

	}

	return FALSE;

}

boolean
maybewedgeheels()
{
	if (!uarmf) return FALSE;
	/* uarmf is definitely defined now */

	if ((uarmf->otyp == WEDGE_SANDALS) || (uarmf->otyp == EVELINE_WEDGE_SANDALS) || (uarmf->otyp == MADELEINE_PLATEAU_BOOTS) || (uarmf->otyp == KATHARINA_PLATFORM_BOOTS) || (uarmf->oartifact == ART_ELENETTES) || (uarmf->otyp == BUM_BUM_BOOTS) || (uarmf->otyp == PET_STOMPING_PLATFORM_BOOTS) || (uarmf->otyp == HIGH_HEELED_SKIERS) || (uarmf->otyp == SING_PLATFORM_BOOTS) || (uarmf->otyp == MELISSA_WEDGE_BOOTS)) return TRUE;

	if (OBJ_DESCR(objects[uarmf->otyp])) {

		if (itemhasappearance(uarmf, APP_IRREGULAR_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_PRINTED_WEDGES)) return TRUE;
		if (itemhasappearance(uarmf, APP_WEDGE_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_SKI_HEELS)) return TRUE;
		if (itemhasappearance(uarmf, APP_BUFFALO_BOOTS)) return TRUE;
		if (itemhasappearance(uarmf, APP_RADIANT_HEELS)) return TRUE;
		if (itemhasappearance(uarmf, APP_WEDGE_SNEAKERS)) return TRUE;

	}

	if (uwep && uwep->oartifact == ART_DAMN_SKI_WEDGE && uarmf) return TRUE;

	return FALSE;

}

/* does everything display in fleecy colors? (for mapglyph.c) --Amy */
boolean
everythingfleecy()
{
	if (uwep && uwep->oartifact == ART_ALASSEA_TELEMNAR) return TRUE;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_ALASSEA_TELEMNAR) return TRUE;
	if (uwep && uwep->oartifact == ART_THRANDUIL_LOSSEHELIN) return TRUE;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_THRANDUIL_LOSSEHELIN) return TRUE;
	if (uwep && uwep->oartifact == ART_HALLUCOLORSWANDIR) return TRUE;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_HALLUCOLORSWANDIR) return TRUE;
	if (uarm && uarm->oartifact == ART_MOEBIUS_ARMOR) return TRUE;

	return FALSE;

}

boolean
playerextrinsicaggravatemon()
{

	if (u.heavyaggravation || EAggravate_monster) return TRUE;
	if (RngePunishment) return TRUE;
	if (FemtrapActiveSolvejg) return TRUE;
	if (Race_if(PM_HC_ALIEN) && !flags.female) return TRUE;

	if (uarmc && itemhasappearance(uarmc, APP_AVENGER_CLOAK)) return TRUE;

	if ((uwep && uwep->oartifact == ART_ASIAN_WINTER) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_ASIAN_WINTER)) return TRUE;
	if ((uwep && uwep->oartifact == ART_FN_M____PARA) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_FN_M____PARA)) return TRUE;
	if ((uwep && uwep->oartifact == ART_KILLER_PIANO) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_KILLER_PIANO)) return TRUE;
	if ((uwep && uwep->oartifact == ART_PICK_OF_THE_GRAVE) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_PICK_OF_THE_GRAVE)) return TRUE;
	if ((uwep && uwep->oartifact == ART_CRUEL_PUNISHER) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_CRUEL_PUNISHER)) return TRUE;
	if ((uwep && uwep->oartifact == ART_SANDRA_S_EVIL_MINDDRILL) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_SANDRA_S_EVIL_MINDDRILL)) return TRUE;
	if ((uarmf && itemhasappearance(uarmf, APP_HEAP_OF_SHIT_BOOTS))) return TRUE;
	if ((uamul && uamul->oartifact == ART_MOSH_PIT_SCRAMBLE) || (uarmu && uarmu->oartifact == ART_HELEN_S_DISCARDED_SHIRT) || (uarm && uarm->oartifact == ART_HO_OH_S_FEATHERS) || (uarm && uarm->oartifact == ART_QUEEN_ARTICUNO_S_HULL) || (uarm && uarm->oartifact == ART_DON_SUICUNE_DOES_NOT_APPRO) || (uarm && uarm->oartifact == ART_DON_SUICUNE_USED_SELFDESTR)) return TRUE;
	if ((uarmc && uarmc->oartifact == ART_BROKEN_WINGS) || (uarmc && uarmc->oartifact == ART_GROUNDBUMMER) || (uwep && uwep->oartifact == ART_SANDRA_S_SECRET_WEAPON) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_SANDRA_S_SECRET_WEAPON)) return TRUE;
	if ((uarmf && uarmf->oartifact == ART_VERY_NICE_PERSON) || (uarmf && uarmf->oartifact == ART_MAILIE_S_CHALLENGE) || (uwep && uwep->oartifact == ART_WHY_DO_YOU_HAVE_SUCH_A_LIT) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_WHY_DO_YOU_HAVE_SUCH_A_LIT) || (uarmf && uarmf->oartifact == ART_LARISSA_S_ANGER) || (uarmf && uarmf->oartifact == ART_CORINA_S_SNOWY_TREAD)) return TRUE;
	if ((uarm && uarm->oartifact == ART_NON_BLADETURNER) || (uarmf && uarmf->oartifact == ART_BOOTS_OF_THE_MACHINE) || (uarmc && uarmc->oartifact == ART_UBERJACKAL_EFFECT)) return TRUE;
	if ((uwep && uwep->oartifact == ART_MANUELA_S_PRACTICANT_TERRO) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_MANUELA_S_PRACTICANT_TERRO) || (uarmc && uarmc->oartifact == ART_HENRIETTA_S_HEAVY_CASTER)) return TRUE;
	if ((uarmf && uarmf->oartifact == ART_HENRIETTA_S_DOGSHIT_BOOTS) || (uarmh && uarmh->oartifact == ART_SEVERE_AGGRAVATION) || (uarms && uarms->oartifact == ART_SHATTERED_DREAMS)) return TRUE;
	if ((uarmc && itemhasappearance(uarmc, APP_EXCREMENT_CLOAK) )) return TRUE;
	if ((uarms && uarms->oartifact == ART_NO_FUTURE_BUT_AGONY) || (uarmf && uarmf->oartifact == ART_CINDERELLA_S_SLIPPERS) || (uwep && uwep->oartifact == ART_GIRLFUL_BONKING) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_GIRLFUL_BONKING)) return TRUE;
	if ((uwep && uwep->oartifact == ART_HENRIETTA_S_MISTAKE) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_HENRIETTA_S_MISTAKE) || (uarmf && uarmf->oartifact == ART_ELLA_S_BLOODLUST) || (uarmf && uarmf->oartifact == ART_LOVELY_GIRL_PLATEAUS)) return TRUE;
	if ((uarmf && uarmf->oartifact == ART_SOLVEJG_S_STINKING_SLIPPER) || (uwep && uwep->oartifact == ART_RATTATTATTATTATT) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_RATTATTATTATTATT) || (uarmf && uarmf->oartifact == ART_I_M_A_BITCH__DEAL_WITH_IT)) return TRUE;
	if ((uarmf && uarmf->oartifact == ART_MANUELA_S_TORTURE_HEELS) || (uarmf && uarmf->oartifact == ART_MANUELA_S_UNKNOWN_HEELS) || (uarmf && uarmf->oartifact == ART_ALLYNONE) || (uarmf && uarmf->oartifact == ART_HERMES__UNFAIRNESS) || (uarmf && uarmf->oartifact == ART_HADES_THE_MEANIE)) return TRUE;
	if ((uarmf && uarmf->oartifact == ART_FUN_ALL_IN_ONE) || (uarmf && uarmf->oartifact == ART_PORCELAIN_ELEPHANT) || (uleft && uleft->oartifact == ART_RING_OF_WOE) || (uright && uright->oartifact == ART_RING_OF_WOE) || (uleft && uleft->oartifact == ART_GOLDENIVY_S_RAGE) || (uright && uright->oartifact == ART_GOLDENIVY_S_RAGE)) return TRUE;
	if ((uleft && uleft->oartifact == ART_GOLDENIVY_S_ENGAGEMENT_RIN) || (uright && uright->oartifact == ART_GOLDENIVY_S_ENGAGEMENT_RIN) || (uamul && uamul->oartifact == ART_SNOREFEST) || (uwep && uwep->oartifact == ART_ARMORWREAKER) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_ARMORWREAKER)) return TRUE;
	if ((uwep && uwep->oartifact == ART_HARKENSTONE) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_HARKENSTONE) || (uwep && uwep->oartifact == ART_KUSANAGI_NO_TSURUGI) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_KUSANAGI_NO_TSURUGI)) return TRUE;
	if ((uwep && uwep->oartifact == ART_ARABELLA_S_ARTIFACT_CREATI) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_ARABELLA_S_ARTIFACT_CREATI)) return TRUE;
	if ((uleft && uleft->oartifact == ART_TASTY_TAME_NASTY) || (uright && uright->oartifact == ART_TASTY_TAME_NASTY)) return TRUE;
	if ((uarmf && uarmf->oartifact == ART_EVERYWHERE_AT_ONCE) || (uarmf && uarmf->oartifact == ART_BITCHSMOKE) || (uarmf && uarmf->oartifact == ART_SORROW_AND_DESPAIR) || (uarmf && uarmf->oartifact == ART_NOW_YOU_ARE_HOWEVER_TO) || (uarmf && uarmf->oartifact == ART_WHINY_TEACHER_INSIDE_WOMAN)) return TRUE;

	return FALSE;

}

boolean
playerextrinsicfireres()
{
	if (EFire_resistance) return TRUE;
	if (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == FIRE_RES)) return TRUE;
	if ((uarmh && uarmh->oartifact == ART_SPECTRAL_RESISTANCE) || (uarmh && uarmh->oartifact == ART_JESTES_TAKA_KURWA) || (uarmu && uarmu->oartifact == ART_BEAM_MULTIPLIER) || (uleft && uleft->oartifact == ART_FIRE_NIGHT) || (uright && uright->oartifact == ART_FIRE_NIGHT) || (uarmf && uarmf->oartifact == ART_YET_ANOTHER_STUPID_IDEA)) return TRUE;
	if ((uwep && uwep->oartifact == ART_AND_YOUR_MORTAL_WORLD_SHAL) || (uarmf && uarmf->oartifact == ART_CLICHE_WEAR) || (uarmf && uarmf->oartifact == ART_JOSEFINE_S_EVILNESS) || (uarmf && uarmf->oartifact == ART_UNFELLABLE_TREE && u.burrowed) || (uwep && uwep->oartifact == ART_SAXS_BEAUTY) || (uarmf && uarmf->oartifact == ART_BITCHSMOKE)) return TRUE;
	if ((uarm && uarm->oartifact == ART_REQUIRED_POWER_PLANT_GEAR) || (uwep && uwep->oartifact == ART_PRISMATIC_PROTECTION) || (uarmh && uarmh->oartifact == ART_HAVE_ALL_YOU_NEED) || (uarmf && uarmf->oartifact == ART_BLUEDE) || (uarmf && uarmf->oartifact == ART_ENDLESS_DESEAMING) || (uwep && uwep->oartifact == ART_FAEAEAEAEAEAU)) return TRUE;
	if ((uarmc && uarmc->oartifact == ART_DRAGONHIDE_ARMER) || (uwep && uwep->oartifact == ART_MISTY_S_MELEE_PLEASURE) || (uimplant && uimplant->oartifact == ART_POTATOROK) || (uarmh && uarmh->oartifact == ART_WAR_MASK_OF_DURIN)) return TRUE;
	if ((powerfulimplants() && uimplant && uimplant->oartifact == ART_RUBBER_SHOALS) || (uimplant && uimplant->oartifact == ART_CORONATION_CULMINATION) || (uarmu && uarmu->oartifact == ART_BIENVENIDO_A_MIAMI) || (uarmu && uarmu->oartifact == ART_PRISMATIC_SHIRT) || (uarmf && uarmf->oartifact == ART_GRENEUVENIA_S_HUG)) return TRUE;
	if ((uarmh && uarmh->oartifact == ART_MASSIVE_IRON_CROWN_OF_MORG) || (uarmf && uarmf->oartifact == ART_TEMPERATOR) || (uarm && uarm->oartifact == ART_ARMOR_OF_EREBOR) || (uarmc && uarmc->oartifact == ART_FIREBURN_COLDSHATTER) || (uarmf && uarmf->oartifact == ART_NASTIST)) return TRUE;
	if ((uwep && uwep->oartifact == ART_STAFF_OF_THE_ARCHMAGI) || (uwep && uwep->oartifact == ART_WHAT_IT_SAYS_ON_THE_TIN) || (uarmg && uarmg->oartifact == ART_SPECTRATOR && (moves % 5 == 0) ) || (uarmh && uarmh->oartifact == ART_SECURE_BATHMASTER)) return TRUE;
	if ((uwep && uwep->oartifact == ART_WRATH_OF_HEAVEN) || (uwep && uwep->oartifact == ART_COLD_SOUL) || u.uprops[STORM_HELM].extrinsic || u.uprops[ELEMENT_RES].extrinsic) return TRUE;
	if ((uarmf && itemhasappearance(uarmf, APP_KOREAN_SANDALS) && (moves % 3 == 0) ) ) return TRUE;
	if ((uamul && uamul->oartifact == ART_DEMOBLING) || (uarmf && uarmf->oartifact == ART_AMPERSAND_HAREM) || (uarmf && uarmf->oartifact == ART_DESEAMING_GAME) || (uarms && uarms->oartifact == ART_SPECTRATE_ETTECKOR) || (uwep && uwep->oartifact == ART_SPECTRATE_ETTECKOR) || have_spectrumplinggem() ) return TRUE;

	return FALSE;
}

boolean
playerextrinsiccoldres()
{
	if (ECold_resistance) return TRUE;
	if (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == COLD_RES) ) return TRUE;
	if ((uarmh && uarmh->oartifact == ART_SPECTRAL_RESISTANCE) || (uarmg && uarmg->oartifact == ART_SIGNONS_STEEL_TOTAL) || (uarmf && uarmf->oartifact == ART_FAR_EAST_RELATION) || (uarmf && uarmf->oartifact == ART_MEPHISTO_S_BROGUES) || (uarmf && uarmf->oartifact == ART_TOO_MUCH_BRAVERY)) return TRUE;
	if ((uarmh && uarmh->oartifact == ART_JESTES_TAKA_KURWA) || (uarmf && uarmf->oartifact == ART_DON_T_FALL_INTO_THE_ABYSS) || (uarmf && uarmf->oartifact == ART_ANTJE_S_POWERSTRIDE) || (uarmf && uarmf->oartifact == ART_FINAL_EXAM_TIME) || (uarmf && uarmf->oartifact == ART_RARE_ASIAN_LADY)) return TRUE;
	if ((uarmf && uarmf->oartifact == ART_KATRIN_S_PARALYSIS) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_DUBAI_TOWER_BREAK) || (uarmf && uarmf->oartifact == ART_CLICHE_WEAR) || (uarmu && uarmu->oartifact == ART_BEAM_MULTIPLIER) || (uarmf && uarmf->oartifact == ART_JESSICA_S_TENDERNESS)) return TRUE;
	if ((uimplant && uimplant->oartifact == ART_CORONATION_CULMINATION) || (uarmu && uarmu->oartifact == ART_PRISMATIC_SHIRT) || (uarmf && uarmf->oartifact == ART_HIT_THEIR_HANDS) || (uarmf && uarmf->oartifact == ART_DEEP_SIGH) || (uarmf && uarmf->oartifact == ART_JOSEFINE_S_EVILNESS)) return TRUE;
	if ((uarmf && uarmf->oartifact == ART_ENDLESS_DESEAMING) || (uarmf && uarmf->oartifact == ART_WHAT_A_CUDDLY_COLOR) || (uarmc && uarmc->oartifact == ART_DRAGONHIDE_ARMER) || (uwep && uwep->oartifact == ART_SAXS_BEAUTY) || (uarmf && uarmf->oartifact == ART_NASTIST)) return TRUE;
	if ((uarmf && uarmf->oartifact == ART_JOHANETTA_S_ROUGH_GENTLENE) || (uarmf && uarmf->oartifact == ART_LARISSA_S_ANGER) || (uarmf && uarmf->oartifact == ART_BLUEDE) || (uarmg && uarmg->oartifact == ART_SPECTRATOR && (moves % 5 == 0) )) return TRUE;
	if ((uarmh && uarmh->oartifact == ART_SECURE_BATHMASTER) || (uwep && uwep->oartifact == ART_PRISMATIC_PROTECTION) || (uarmh && uarmh->oartifact == ART_MASSIVE_IRON_CROWN_OF_MORG) || (uarmf && uarmf->oartifact == ART_CHOICE_OF_MATTER) || (uarmu && uarmu->oartifact == ART_BIENVENIDO_A_MIAMI)) return TRUE;
	if ((uarmf && uarmf->oartifact == ART_YVONNE_S_MODEL_AMBITION) || (uarmf && uarmf->oartifact == ART_CORINA_S_UNFAIR_SCRATCHER) || (uarmf && uarmf->oartifact == ART_UNFELLABLE_TREE && u.burrowed) || (uarmh && uarmh->oartifact == ART_HAVE_ALL_YOU_NEED)) return TRUE;
	if ((uarmc && uarmc->oartifact == ART_FIREBURN_COLDSHATTER) || (uarm && uarm->oartifact == ART_ARMOR_OF_EREBOR) || (uarmf && uarmf->oartifact == ART_CORINA_S_SNOWY_TREAD) || (uarmc && uarmc->oartifact == ART_CLOAK_OF_THE_CONSORT)) return TRUE;
	if ((uwep && uwep->oartifact == ART_STAFF_OF_THE_ARCHMAGI) || (uwep && uwep->oartifact == ART_COLD_SOUL) || (uwep && uwep->oartifact == ART_RELEASE_FROM_CARE) || u.uprops[STORM_HELM].extrinsic || u.uprops[ELEMENT_RES].extrinsic) return TRUE;
	if ((uarmu && uarmu->oartifact == ART_GIVE_ME_STROKE__JO_ANNA) || (uarmf && uarmf->oartifact == ART_CLAUDIA_S_SELF_WILL) || (uarmf && uarmf->oartifact == ART_RUTH_S_UNDEAD_INLAY) || (uarmf && uarmf->oartifact == ART_NOW_YOU_LOOK_LIKE_A_BEGGAR) || (uwep && uwep->oartifact == ART_ELEMENTS_TIME_THREE) || (uarms && uarms->oartifact == ART_SPECTRATE_ETTECKOR) || (uwep && uwep->oartifact == ART_SPECTRATE_ETTECKOR) || (uarmf && uarmf->oartifact == ART_SARAH_S_SNEAKERS_OF_INSTAN) || have_spectrumplinggem() ) return TRUE;

	return FALSE;
}

boolean
playerextrinsicshockres()
{
	if (EShock_resistance) return TRUE;
	if (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == SHOCK_RES) ) return TRUE;
	if ((uarmh && uarmh->oartifact == ART_SPECTRAL_RESISTANCE) || (uarmf && uarmf->oartifact == ART_KATRIN_S_PARALYSIS) || (uarmf && uarmf->oartifact == ART_SOLVEJG_S_STINKING_SLIPPER) || (uarmu && uarmu->oartifact == ART_BEAM_MULTIPLIER) || (uarmf && uarmf->oartifact == ART_UNFELLABLE_TREE && u.burrowed)) return TRUE;
	if ((uarmc && uarmc->oartifact == ART_ACIDSHOCK_CASTLECRUSHER) || (uwep && uwep->oartifact == ART_COUNTERSTRIKE_CESTUS) || (uwep && uwep->oartifact == ART_PRISMATIC_PROTECTION) || (uarmf && uarmf->oartifact == ART_FIRST_PLACE_GUARANTEED) || (uarmf && uarmf->oartifact == ART_ENDLESS_DESEAMING)) return TRUE;
	if ((uimplant && uimplant->oartifact == ART_CORONATION_CULMINATION) || (uarmf && uarmf->oartifact == ART_JOSEFINE_S_EVILNESS) || (uleft && uleft->oartifact == ART_CORGON_S_RING) || (uright && uright->oartifact == ART_CORGON_S_RING) || (uarmf && uarmf->oartifact == ART_LARISSA_S_ANGER)) return TRUE;
	if ((uarmf && uarmf->oartifact == ART_JOHANETTA_S_ROUGH_GENTLENE) || (uwep && uwep->oartifact == ART_YVONNE_S_HONOR) || (uarmf && uarmf->oartifact == ART_PRETTY_ROOMMAID) || (uarmf && uarmf->oartifact == ART_YVONNE_S_MODEL_AMBITION) || (uarmf && uarmf->oartifact == ART_WHAT_A_CUDDLY_COLOR)) return TRUE;
	if ((uwep && uwep->oartifact == ART_SAXS_BEAUTY) || (uarmf && uarmf->oartifact == ART_BLUEDE) || (uwep && uwep->oartifact == ART_RESISTOMATIC) || (uarmf && uarmf->oartifact == ART_CLICHE_WEAR) || (uarmg && uarmg->oartifact == ART_SPECTRATOR && (moves % 5 == 0) ) || (uarmc && uarmc->oartifact == ART_INA_S_SORROW && u.uhunger < 0)) return TRUE;
	if ((uarmh && uarmh->oartifact == ART_HAVE_ALL_YOU_NEED) || (uarmf && uarmf->oartifact == ART_CORINA_S_UNFAIR_SCRATCHER) || (uarmh && uarmh->oartifact == ART_MASSIVE_IRON_CROWN_OF_MORG) || (uarmh && uarmh->oartifact == ART_STORMHELM) || (uarmc && uarmc->oartifact == ART_WEB_OF_THE_CHOSEN)) return TRUE;
	if ((uwep && uwep->oartifact == ART_STAFF_OF_THE_ARCHMAGI) || (uwep && uwep->oartifact == ART_FIRE_OF_HEAVEN) || (uwep && uwep->oartifact == ART_COLD_SOUL) || u.uprops[STORM_HELM].extrinsic || u.uprops[ELEMENT_RES].extrinsic) return TRUE;
	if ((uwep && uwep->oartifact == ART_COPPERED_OFF_FROM_ME) || (uarms && uarms->oartifact == ART_SPECTRATE_ETTECKOR) || (uwep && uwep->oartifact == ART_SPECTRATE_ETTECKOR) || (uarmf && uarmf->oartifact == ART_SARAH_S_SNEAKERS_OF_INSTAN) || have_spectrumplinggem() ) return TRUE;

	return FALSE;
}

boolean
playerextrinsicpoisonres()
{
	if (EPoison_resistance) return TRUE;
	if (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == POISON_RES) ) return TRUE;
	if ((uarmh && uarmh->oartifact == ART_SPECTRAL_RESISTANCE) || (uarmu && uarmu->oartifact == ART_GENTLE_SOFT_CLOTHING) || (uarm && uarm->oartifact == ART_REQUIRED_POWER_PLANT_GEAR) || (uarmc && uarmc->oartifact == ART_ACQUIRED_POISON_RESISTANCE)) return TRUE;
	if ((uarmc && uarmc->oartifact == ART_PREMIUM_VISCOSITY) || (uarmh && uarmh->oartifact == ART_BIG_BONNET) || (uarmg && uarmg->oartifact == ART_SIGNONS_STEEL_TOTAL) || (uarmf && uarmf->oartifact == ART_MEPHISTO_S_BROGUES) || (uwep && uwep->oartifact == ART_ALSO_MATTE_MASK)) return TRUE;
	if ((uwep && uwep->oartifact == ART_PERNICIOUS_GRID) || (uwep && uwep->oartifact == ART_PRISMATIC_PROTECTION) || (uarmh && uarmh->oartifact == ART_NYPHERISBANE) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_RHEA_S_MISSING_EYESIGHT) || (uarmf && uarmf->oartifact == ART_BLUEDE)) return TRUE;
	if ((uarmf && uarmf->oartifact == ART_EVERYTHING_IS_GREEN) || (uleft && uleft->oartifact == ART_RELIABLE_TRINSICS) || (uright && uright->oartifact == ART_RELIABLE_TRINSICS) || (uimplant && uimplant->oartifact == ART_CORONATION_CULMINATION) || (uarmu && uarmu->oartifact == ART_KEITH_S_UNDEROOS)) return TRUE;
	if ((uwep && uwep->oartifact == ART_POISON_BURST) || (uwep && uwep->oartifact == ART_SNAKELASH) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_LAUGHING_AT_MIDNIGHT) || (uwep && uwep->oartifact == ART_SAXS_BEAUTY) || (uarmu && uarmu->oartifact == ART_PRISMATIC_SHIRT)) return TRUE;
	if ((uarmc && uarmc->oartifact == ART_HEV_SUIT) || (uarmf && uarmf->oartifact == ART_MAILIE_S_CHALLENGE) || (uarmf && uarmf->oartifact == ART_CLICHE_WEAR) || (uarmh && uarmh->oartifact == ART_MASSIVE_IRON_CROWN_OF_MORG) || (uleft && uleft->oartifact == ART_GREEN_COLOR) || (uright && uright->oartifact == ART_GREEN_COLOR)) return TRUE;
	if ((uarmh && uarmh->oartifact == ART_TARI_FEFALAS) || (uarmg && uarmg->oartifact == ART_SPECTRATOR && (moves % 5 == 0) ) || (uarmf && uarmf->oartifact == ART_LEATHER_PUMPS_OF_HORROR) || (uarmf && uarmf->oartifact == ART_RHEA_S_COMBAT_PUMPS) || (uamul && uamul->oartifact == ART_WARNED_AND_PROTECTED)) return TRUE;
	if ((uarmh && uarmh->oartifact == ART_GREEN_STATUS) || (uamul && uamul->oartifact == ART_GOOD_BEE) || (uamul && uamul->oartifact == ART_BUEING) || (uwep && uwep->oartifact == ART_ERU_ILUVATAR_S_BIBLE) || (uarmh && uarmh->oartifact == ART_WAR_MASK_OF_DURIN) || (uwep && uwep->oartifact == ART_SERPENT_S_TOOTH)) return TRUE;
	if ((uarmf && uarmf->oartifact == ART_GRASSHOPPER_S_ANTIDOTE) || (uarmf && uarmf->oartifact == ART_AMPERSAND_HAREM) || (uarmf && uarmf->oartifact == ART_CLAUDIA_S_SELF_WILL) || (uarmf && uarmf->oartifact == ART_RUTH_S_UNDEAD_INLAY) || (uamul && uamul->oartifact == ART_AMULET_OF_SPLENDOR) || (uwep && uwep->oartifact == ART_COPPERED_OFF_FROM_ME) || (uarmf && uarmf->oartifact == ART_NOW_YOU_LOOK_LIKE_A_BEGGAR)) return TRUE;
	if ((uwep && uwep->oartifact == ART_REAL_WALKING) || (uwep && uwep->oartifact == ART_DANGER_SIGN) || (uarms && uarms->oartifact == ART_SPECTRATE_ETTECKOR) || (uwep && uwep->oartifact == ART_SPECTRATE_ETTECKOR) || (uwep && uwep->oartifact == ART_CHROME_GOOD) || have_spectrumplinggem() ) return TRUE;

	return FALSE;
}

boolean
playerextrinsicspeed()
{
	if (EFast || (HFast & ~INTRINSIC)) return TRUE;
	if (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == FAST) ) return TRUE;
	if ((uwep && uwep->oartifact == ART_FUMATA_YARI) || (uwep && uwep->oartifact == ART_INGRAM_MAC___) || (uarmc && uarmc->oartifact == ART_SPEEDRUNNER_S_DREAM) || (uarmh && uarmh->oartifact == ART_ELONA_S_SNAIL_TRAIL && Race_if(PM_ELONA_SNAIL)) || (uarmh && uarmh->oartifact == ART_REAL_SPEED_DEVIL)) return TRUE;
	if ((uarmc && uarmc->oartifact == ART_ROKKO_CHAN_S_SUIT) || (uwep && uwep->oartifact == ART_QUICKER_RHEOLOGY) || (uwep && uwep->oartifact == ART_EAMANE_LUINWE) || (uarmc && uarmc->oartifact == ART_FULLY_LIONIZED) || (uarmh && uarmh->oartifact == ART_LORSKEL_S_SPEED)) return TRUE;
	if ((uwep && uwep->oartifact == ART_ACCELERATION_CLAW) || (uarmc && uarmc->oartifact == ART_JANA_S_FAIRNESS_CUP) || (uarmf && uarmf->oartifact == ART_AMATEURSPORTS) || (uwep && uwep->oartifact == ART_BIG_SMOKE_S_MURDER_CAR) || (uimplant && uimplant->oartifact == ART_KATRIN_S_SUDDEN_APPEARANCE)) return TRUE;
	if ((uarmc && uarmc->oartifact == ART_FAST_CAMO_PREDATOR) || (uarmh && uarmh->oartifact == ART_ELESSAR_ELENDIL) || (uarmf && uarmf->oartifact == ART_CARMARK) || (uamul && uamul->oartifact == ART_ANASTASIA_S_LURE) || (uarmf && uarmf->oartifact == ART_UNTRAINED_HALF_MARATHON)) return TRUE;
	if ((uarmf && uarmf->oartifact == ART_FIRST_PLACE_GUARANTEED) || (uleft && uleft->oartifact == ART_REQUIRED_GLADNESS) || (uwep && uwep->otyp == WIND_BOW) || (uarmf && uarmf->oartifact == ART_VRRRRRRRRRRRR) || (uarmc && uarmc->oartifact == ART_KANGAROO_SNORT) || (uarmc && uarmc->oartifact == ART_FAST_SPEED_BUMP)) return TRUE;
	if ((uarmh && uarmh->oartifact == ART_DUE_DUE_DUE_DUE_BRMMMMMMM) || (uwep && uwep->oartifact == ART_NATALIA_S_MARK) || (uarmf && uarmf->oartifact == ART_UNEVEN_ENGINE) || (uwep && uwep->oartifact == ART_FEANARO_SINGOLLO) || (uarm && uarm->oartifact == ART_ROCKET_IMPULSE) || (uarmc && uarmc->oartifact == ART_LIGHTSPEED_TRAVEL)) return TRUE;
	if ((uright && uright->oartifact == ART_REQUIRED_GLADNESS) || (uarmf && uarmf->oartifact == ART_HIGH_DESIRE_OF_FATALITY) || (uwep && uwep->oartifact == ART_SPEEDHACK) || (uarmc && uarmc->oartifact == ART_JANA_S_SECRET_CAR) || (uamul && uamul->oartifact == ART_ARABELLA_S_DICINATOR)) return TRUE;
	if ((uarmc && uarmc->oartifact == ART_JANA_S_GRAVE_WALL) || (uleft && uleft->oartifact == ART_CRIMINAL_QUEEN) || (uright && uright->oartifact == ART_CRIMINAL_QUEEN) || (uwep && uwep->oartifact == ART_TENSA_ZANGETSU) || (uwep && uwep->oartifact == ART_TARMAC_CHAMPION) || (uwep && uwep->oartifact == ART_ZANKAI_HUNG_ZE_TUNG_DO_HAI)) return TRUE;
	if ((uwep && uwep->oartifact == ART_GARNET_ROD) || (uwep && uwep->oartifact == ART_THREE_HEADED_FLAIL) || (uwep && uwep->oartifact == ART_POGO_STICK) || u.uprops[MULTISHOES].extrinsic) return TRUE;
	if ((Role_if(PM_TRANSVESTITE) && PlayerInHighHeels) || (Role_if(PM_TOPMODEL) && PlayerInHighHeels) ) return TRUE;
	if (uarmf && uarmf->oartifact == ART_MAREYOUNGWOMAN_ACTION && (is_pool(u.ux, u.uy) || (levl[u.ux][u.uy].typ == FOUNTAIN)) ) return TRUE;
	if ((uarmf && uarmf->oartifact == ART_FASTER_THAN_ALL_OTHERS_INT) || (uleft && uleft->oartifact == ART_POLYFAST) || (uright && uright->oartifact == ART_POLYFAST) || (uwep && uwep->oartifact == ART_SHARPTOOTH_SAYER)) return TRUE;

	return FALSE;
}

/* does the item "otmp" have a specific randomized appearance? tested with oc_appearindex variable, because that's much
 * better than having to always check for regular, soviet and ancient randomized appearance string --Amy */
boolean
itemhasappearance(otmp, apptest)
struct obj *otmp;
int apptest;
{
	if (program_state.gameover) return FALSE; /* objects array might already be filled with garbage at this point */

	if (!otmp) return FALSE; /* error! shouldn't happen */
	if (otmp->otyp >= NUM_OBJECTS) return FALSE; /* out of bounds - better don't continue, to avoid segfaults */
	if (objects[otmp->otyp].oc_appearindex == apptest) return TRUE;

	return FALSE; /* catchall */
}

/* the same function, but testing for base item "itemnum" */
boolean
itemnumwithappearance(itemnum, apptest)
int itemnum, apptest;
{
	if (program_state.gameover) return FALSE; /* objects array might already be filled with garbage at this point */

	if (itemnum <= STRANGE_OBJECT) return FALSE; /* error! shouldn't happen */
	if (itemnum >= NUM_OBJECTS) return FALSE; /* error! shouldn't happen */
	if (objects[itemnum].oc_appearindex == apptest) return TRUE;

	return FALSE; /* catchall */
}

boolean
make_hallucinated(xtime, talk, mask)
long xtime;	/* nonzero if this is an attempt to turn on hallucination */
boolean talk;
long mask;	/* nonzero if resistance status should change by mask */
{
	long old = HHallucination;
	boolean changed = 0;
	const char *message, *verb;

	message = (!xtime) ? "Everything %s SO boring now." :
			     "Oh wow!  Everything %s so cosmic!";
	verb = (!Blind) ? "looks" : "feels";

	if (xtime && (uarmh && itemhasappearance(uarmh, APP_TWISTED_VISOR_HELMET))) {
		if (xtime > HHallucination) xtime += ((xtime - HHallucination) * 5);
	}

	if (mask) {
	    if (HHallucination) changed = TRUE;

	    if (!xtime) EHalluc_resistance |= mask;
	    else EHalluc_resistance &= ~mask;
	} else {
	    if (!EHalluc_resistance && (!!HHallucination != !!xtime))
		changed = TRUE;
	    set_itimeout(&HHallucination, xtime);
		if (xtime && !rn2(1000)) {
			pline("Now that was some fucked up shit you did there, huh? But who cares? Let's enjoy the colors!");
			set_itimeout(&HeavyHallu, xtime);
		}
		if (xtime && (StatusTrapProblem || u.uprops[STATUS_FAILURE].extrinsic || have_statusstone()) ) set_itimeout(&HeavyHallu, xtime);
		if (xtime && uarmc && uarmc->oartifact == ART_KANGAROO_SNORT) set_itimeout(&HeavyHallu, xtime);

	    /* clearing temporary hallucination without toggling vision */
	    if (!changed && !HHallucination && old && talk) {
		if (!haseyes(youmonst.data)) {
		    strange_feeling((struct obj *)0, (char *)0);
		} else if (Blind) {
		    char buf[BUFSZ];
		    int eyecnt = eyecount(youmonst.data);

		    strcpy(buf, body_part(EYE));
		    Your(eyemsg, (eyecnt == 1) ? buf : makeplural(buf),
			 (eyecnt == 1) ? "itches" : "itch");
		} else {	/* Grayswandir */
		    Your(vismsg, "flatten", "normal");
		}
	    }
	}

	if (changed) {
	    if (u.uswallow) {
		swallowed(0);	/* redraw swallow display */
	    } else {
		/* The see_* routines should be called *before* the pline. */
		see_monsters();
		see_objects();
		see_traps();
	    }

	    /* for perm_inv and anything similar
	    (eg. Qt windowport's equipped items display) */
	    update_inventory();

	    flags.botl = 1;
	    if (talk) pline(message, verb);

	    if (!(InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone())) (void) doredraw();

	}
	return changed;
}

/* For game balance we don't always want a bottle made. */
static void make_bottle(boolean certain)
{
	struct obj *otmp;

	if (certain || !rn2(8)) {
		otmp = mksobj(BOTTLE,TRUE,TRUE, FALSE);
/* We do not transfer curses/blessings, assuming that these a property
** of the actual potion, not the bottle. */
		if (otmp) {
			otmp->cursed = otmp->blessed = FALSE;
			You("are left with the empty bottle.");
			hold_another_object(otmp,"Oops! It drops to the floor!",(const char *) 0, (const char *) 0);
		}
	}
}


STATIC_OVL void
ghost_from_bottle()
{
	struct monst *mtmp = makemon(&mons[PM_GHOST], u.ux, u.uy, NO_MM_FLAGS);

	if (!mtmp) {
		pline("This bottle turns out to be empty.");
		make_bottle(TRUE);
		return;
	}
	if (Blind) {
		pline("As you open the bottle, %s emerges.", something);
		return;
	}
	pline("As you open the bottle, an enormous %s emerges!",
		Hallucination ? rndmonnam() : (const char *)"ghost");
	if(flags.verbose)
	    You("are frightened to death, and unable to move.");
	nomul(-3, "being frightened to death", TRUE);
	make_feared(HFeared + rnd(30 + (monster_difficulty() * 3) ),TRUE);
	nomovemsg = "You regain your composure.";
	make_bottle(FALSE);
}

/* "Quaffing is like drinking, except you spill more."  -- Terry Pratchett
 */

void
badeffect()

{

	struct obj *otmp;
	int nastytrapdur;
	int blackngdur;

	if (uarm && objects[(uarm)->otyp].oc_material == MT_INKA && !rn2(10)) {
		pline("Thanks to your inka armor, you averted misfortune!");
		return;
	}
	if (uarmf && objects[(uarmf)->otyp].oc_material == MT_INKA && !rn2(10)) {
		pline("Thanks to your inka footwear, you averted misfortune!");
		return;
	}
	if (uarmg && objects[(uarmg)->otyp].oc_material == MT_INKA && !rn2(10)) {
		pline("Thanks to your inka gloves, you averted misfortune!");
		return;
	}
	if (uarmh && objects[(uarmh)->otyp].oc_material == MT_INKA && !rn2(10)) {
		pline("Thanks to your inka helmet, you averted misfortune!");
		return;
	}
	if (uarms && objects[(uarms)->otyp].oc_material == MT_INKA && !rn2(10)) {
		pline("Thanks to your inka shield, you averted misfortune!");
		return;
	}
	if (uarmc && objects[(uarmc)->otyp].oc_material == MT_INKA && !rn2(10)) {
		pline("Thanks to your inka cloak, you averted misfortune!");
		return;
	}
	if (uarmu && objects[(uarmu)->otyp].oc_material == MT_INKA && !rn2(10)) {
		pline("Thanks to your inka shirt, you averted misfortune!");
		return;
	}
	if (uwep && uwep->oartifact == ART_LUCK_VERSUS_BAD && !rn2(2)) {
		pline("Thanks to your inka sling, you averted misfortune!");
		return;
	}

	if (!(PlayerCannotUseSkills)) {
		int shiichochance = 0;
		switch (P_SKILL(P_SHII_CHO)) {

			case P_BASIC:	shiichochance =  1; break;
			case P_SKILLED:	shiichochance =  2; break;
			case P_EXPERT:	shiichochance =  3; break;
			case P_MASTER:	shiichochance =  4; break;
			case P_GRAND_MASTER:	shiichochance =  5; break;
			case P_SUPREME_MASTER:	shiichochance =  6; break;
			default: shiichochance += 0; break;
		}

		if (shiichochance > rn2(100)) {
			/* avoid bad effect --Amy */
			return;
		}
	}

	u.cnd_badeffectcount++;

	switch (rnd(483)) {

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
		if (FunnyHallu) You_feel("rather trippy.");
		else You_feel("rather %s.", body_part(LIGHT_HEADED));
		make_confused(HConfusion + rnz(50),FALSE);
		break;

		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
		case 24:
		case 25:
		case 26:
		case 27:
		case 28:
		case 29:
		case 30:
		if (FunnyHallu) You_feel("uncontrollable.");
		else You_feel("stunned.");
		make_stunned(HStun + rnz(50),FALSE);
		break;

		case 31:
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
		case 38:
		case 39:
		case 40:
		case 41:
		case 42:
		case 43:
		case 44:
		case 45:
		if (FunnyHallu) You_feel("even weirder!");
		else You_feel("weirded out!");
		make_hallucinated(HHallucination + rnz(100),FALSE,0L);
		break;

		case 46:
		case 47:
		case 48:
		case 49:
		case 50:
		case 51:
		case 52:
		case 53:
		case 54:
		case 55:
		case 56:
		case 57:
		case 58:
		case 59:
		case 60:
		if (!Blind) pline("Everything suddenly goes dark.");
		make_blinded(Blinded+rnz(100),FALSE);
		if (!Blind) Your("%s", vision_clears);
		break;

		case 61:
		case 62:
		case 63:
		case 64:
		case 65:
		case 66:
		case 67:
		case 68:
		case 69:
		case 70:
		case 71:
		case 72:
		case 73:
		case 74:
		case 75:
		if (FunnyHallu) You_feel("like your body is trying to fall asleep!");
		else You_feel("numb.");
		make_numbed(HNumbed + rnz(150),FALSE);
		break;

		case 76:
		case 77:
		case 78:
		case 79:
		case 80:
		if (FunnyHallu) You_feel("a giant ice cream cone enclosing you!");
		else pline("You're getting the chills.");
		make_frozen(HFrozen + rnz(150),FALSE);
		break;

		case 81:
		case 82:
		case 83:
		case 84:
		case 85:
		if (FunnyHallu) You_feel("like you have dementia tremor!"); /* not a real name --Amy */
		else pline("Your hands start trembling violently!");
		incr_itimeout(&Glib, rnz(50) );
		break;

		case 86:
		case 87:
		case 88:
		case 89:
		case 90:
		if (FunnyHallu) You_feel("totally down! Seems you tried some illegal shit!");
		else You_feel("like you're going to throw up.");
	      make_vomiting(Vomiting+20, TRUE);
		if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
		break;

		case 91:
		case 92:
		case 93:
		case 94:
		case 95:
		case 96:
		case 97:
		case 98:
		case 99:
		case 100:
		pline("The world spins and goes dark.");
		flags.soundok = 0;
		nomul(-rnd(10), "helplessly knocked out", TRUE);
		nomovemsg = "You are conscious again.";
		afternmv = Hear_again;
		break;

		case 101:
		make_sick(rn1(25,25), "spreading food poisoning", TRUE, SICK_VOMITABLE);
		break;

		case 102:
		if (!Slimed && !flaming(youmonst.data) && !Unchanging && !slime_on_touch(youmonst.data) ) {
			You("don't feel very well.");
			make_slimed(100);
			stop_occupation();
			killer_format = KILLED_BY_AN;
			delayed_killer = "slimed by summoned slime";
		}
		break;

		case 103:

		if ((otmp = mksobj(LOADSTONE, TRUE, FALSE, FALSE)) != (struct obj *)0) {
		You_feel("burdened");
		otmp->quan = 1;
		otmp->owt = weight(otmp);
		if (pickup_object(otmp, 1, FALSE, TRUE) <= 0) {
		obj_extract_self(otmp);
		place_object(otmp, u.ux, u.uy);
		newsym(u.ux, u.uy); }
		}

		break;

		case 104:
		case 105:
		case 106:
		pline("You float up!");
		HLevitation &= ~I_SPECIAL;
		incr_itimeout(&HLevitation, rnz(50));

		break;

		case 107:
		case 108:
		case 109:
		case 110:
		case 111:
		create_critters(rnz(10), (struct permonst *)0);
		break;

		case 112:
		case 113:
		case 114:
		case 115:
		case 116:
		{coord mm;
		mm.x = u.ux;
		mm.y = u.uy;
		pline("Undead creatures are called forth from the grave!");   
		mkundead(&mm, FALSE, 0, FALSE);
		}
		break;

		case 117:
		case 118:
		case 119:
		case 120:
		case 121:
		case 122:
		case 123:
		case 124:
		case 125:
		case 126:
		case 127:
		case 128:
		case 129:
		case 130:
		case 131:
		case 132:
		You_feel("that monsters are aware of your presence.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Dazhe sovetskaya Pyat' Lo obostryayetsya v vashem nizkom igrovom masterstve." : "Woaaaaaah!");
		aggravate();

		break;

		case 133:
		case 134:
		case 135:
		case 136:
		case 137:
		You_feel("as if you need some help.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
		rndcurse();

		break;

		case 138:
			{register struct obj *obj;

			pline("Urgh! You feel a malevolent presence!");
			for(obj = invent; obj ; obj = obj->nobj)
				if (!rn2(5) && !stack_too_big(obj))	curse(obj);
			}
		break;

		case 139:
			if (!Antimagic || !rn2(StrongAntimagic ? 20 : 5)) {
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

		break;

		case 140:
		u.ugangr++;
		if (!rn2(5)) u.ugangr++;
		if (!rn2(25)) u.ugangr++;
		prayer_done();

		break;

		case 141:
			if (!Antimagic || !rn2(StrongAntimagic ? 20 : 5)) {
			    You("suddenly feel weaker!");
			    losestr(rnz(4), TRUE);
			    if (u.uhp < 1) {
				u.youaredead = 1;
				u.uhp = 0;
				killer_format = KILLED_BY;
				killer = "a fatally low strength";
				done(DIED);
				u.youaredead = 0;

				}
			}

		break;

		case 142:
		case 143:
		if ((!StrongSwimming || !rn2(10)) && (!StrongMagical_breathing || !rn2(10))) {
			water_damage(invent, FALSE, FALSE);
			if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);
		}
		if (Burned) make_burned(0L, TRUE);

		break;

		case 144:

		withering_damage(invent, FALSE, FALSE);

		break;

		case 145:
		if (!Stoned && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && !(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM)) ) {
			if (Hallucination && rn2(10)) pline("You are already stoned.");
			else {
				You("start turning to stone!");
				Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
				u.cnd_stoningcount++;
				stop_occupation();
				delayed_killer = "bad petrification effect";
			}
		}

		break;

		case 146:
		{
		int aligntype;
		aligntype = rn2((int)A_LAWFUL+2) - 1;
		 pline("A servant of %s appears!",aligns[1 - aligntype].noun);
		summon_minion(aligntype, TRUE);
		}
		break;

		case 147:
		case 148:
		case 149:
		case 150:
		case 151:
		case 152:
		case 153:
		case 154:
		case 155:
		case 156:
	      attrcurse();

		break;

		case 157:
		case 158:
		case 159:
		case 160:
		case 161:
		case 162:
		case 163:
		case 164:
		case 165:
		case 166:
		case 167:
		case 168:
		case 169:
		case 170:
		case 171:
		pline("It gets dark!");
		if (!shadowprotection()) do_clear_areaX(u.ux,u.uy, 15, set_litI, (void *)((char *)0));
		/* darkness around player's position */

		break;

		case 172:
		case 173:
		case 174:
		case 175:
		case 176:
		case 177:
		case 178:
		case 179:
		case 180:
		case 181:
		{	int rtrap;
		    int i, j, bd;
			bd = 1;
			if (!rn2(5)) bd += rnz(1);

		      for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
				if (!isok(u.ux + i, u.uy + j)) continue;
				if (levl[u.ux + i][u.uy + j].typ <= DBWALL) continue;
				if (t_at(u.ux + i, u.uy + j)) continue;

			      rtrap = randomtrap();
				if (!rn2(20)) makerandomtrap(TRUE);

				(void) maketrap(u.ux + i, u.uy + j, rtrap, 100, TRUE);
			}
			makerandomtrap(TRUE);
			if (!rn2(3)) makerandomtrap(TRUE);
		}
		break;

		case 182:
		case 183:
		case 184:
	      You_feel("yanked in a new direction!");
		(void) safe_teleds(FALSE);

		break;

		case 185:
		case 186:
		case 187:
		case 188:
		case 189:
		case 190:
		case 191:
		case 192:
		case 193:
		case 194:
		case 195:
		case 196:
		case 197:
		case 198:
		case 199:
		pline("You lose  Mana");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vasha magicheskaya energiya udalyayetsya v nastoyashcheye vremya. Skoro on budet raven nulyu, a zatem vy dolzhny igrat' bez zaklinaniy, potomu chto vy sosat', GA GA GA!" : "Due-l-ue-l-ue-l!");
		drain_en(rnz(monster_difficulty() + 1) );
		break;

		case 200:
		case 201:
		case 202:
	      You_feel("out of luck!");
			change_luck(-1);
			if (!rn2(10)) change_luck(-5);
			adjalign(-10);
			if (!rn2(10)) adjalign(-50);

		break;

		case 203:
		case 204:
		case 205:
		case 206:
		case 207:
		punishx();
		break;

		case 208:
		case 209:
		case 210:
		case 211:
		case 212:
		case 213:
		case 214:
		case 215:
		case 216:
		case 217:
		case 218:
		case 219:
		case 220:
		case 221:
		case 222:
		{int copcnt;
		copcnt = rnd(monster_difficulty() ) + 1;
		if (rn2(5)) copcnt = (copcnt / (rnd(4) + 1)) + 1;
		if (rn2(5)) copcnt /= 2;
		if (!rn2(5)) copcnt /= 2; /* don't make too many */
		if (!rn2(10)) copcnt /= 3;
		if (copcnt < 1) copcnt = 1;

		u.cnd_kopsummonamount++;

		if (uarmh && itemhasappearance(uarmh, APP_ANTI_GOVERNMENT_HELMET) ) {
			copcnt = (copcnt / 2) + 1;
		}

		if (RngeAntiGovernment) {
			copcnt = (copcnt / 2) + 1;
		}

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

	      while(--copcnt >= 0) {
			(void) makemon(mkclass(S_KOP,0), u.ux, u.uy, MM_ANGRY);

			if (!rn2(500)) {

				int koptryct = 0;
				int kox, koy;

				for (koptryct = 0; koptryct < 2000; koptryct++) {
					kox = rn1(COLNO-3,2);
					koy = rn2(ROWNO);

					if (kox && koy && isok(kox, koy) && (levl[kox][koy].typ > DBWALL) && !(t_at(kox, koy)) ) {
						(void) maketrap(kox, koy, KOP_CUBE, 0, FALSE);
						break;
						}
				}

			}

		} /* while */

		u.aggravation = 0;

		}
		break;

		case 223:
			losexp((char *)0, TRUE, FALSE); 

		break;

		case 224:
		    attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse();

		break;

		case 225:
		case 226:
		case 227:
		case 228:
		case 229:
		case 230:
		case 231:
		case 232:

			pline(FunnyHallu ? "You feel sinful... but do you really care?" : "You have a feeling of separation.");
			u.ublesscnt += rnz(ishaxor ? 150 : 300);

		break;

		case 233:
		case 234:
		case 235:
		case 236:
		case 237:
		case 238:
		case 239:
		case 240:
		case 241:
		case 242:
		if (FunnyHallu) You_feel("totally hot! Oh yeah, baby!");
		else pline("You're burning!");
		make_burned(HBurned + rnz(150),FALSE);
		break;

		case 243:
		case 244:
		case 245:
		case 246:
		case 247:
		case 248:
		case 249:
		case 250:
		case 251:
		case 252:
		case 253:
		case 254:
		case 255:
		case 256:
		case 257:
		if (FunnyHallu) You("panic! The alarm bells are ringing and you don't know how to get out!");
		else You_feel("afraid.");
		make_feared(HFeared + rnz(150),FALSE);
		break;

		case 258:
		case 259:
		case 260:
		case 261:
		case 262:
		case 263:
		case 264:
		case 265:
		case 266:
		case 267:
		case 268:
		case 269:
		case 270:
		case 271:
		case 272:
		case 273:
		case 274:
		case 275:
		case 276:
		case 277:
			deacrandomintrinsic(rnz(200));
			break;

		case 278:

		nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
		if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
		blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
		if (!blackngdur ) blackngdur = 500; /* fail safe */

		randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), blackngdur - (monster_difficulty() * 3));

		break;

		case 279:
		case 280:
		case 281:
		case 282:
		case 283:
		case 284:
		case 285:
		case 286:
		case 287:
		case 288:
		    adjalign(-rnd(20));
		    if (flags.soundok) You_hear("a slight rumbling...");
		    if (PlayerHearsSoundEffects) pline(issoviet ? "Pomolis'! Vash bog, bezuslovno, pomozhet vam!" : "Wumm.");

		break;

		case 289:

		    adjalign(-rnd(50));
			u.ualign.sins++;
			u.alignlim--;
		    if (flags.soundok) You_hear("a thunderous rumbling!");
		    if (PlayerHearsSoundEffects) pline(issoviet ? "Seychas ideal'noye vremya dlya molitvy! Vash bog super schastliv s toboy i, bezuslovno, vylechit' vse bolezni i predostavit' vam moshchnyy punkt!" : "Wummmmm! Wummmmm! Grummel!");

		break;

		case 290:

			pline("You are engulfed in flames!");
			(void) burnarmor(&youmonst);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5)) destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5)) destroy_item(SPBOOK_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5)) destroy_item(POTION_CLASS, AD_FIRE);

		break;

		case 291:
		case 292:
		{	
		      int bd = rnd(10);
			if (!rn2(5)) bd += rnz(10);

			while (bd-- >= 0) makerandomtrap(TRUE);

		}
		break;

		case 293:
		case 294:
		case 295:
		case 296:
		case 297:
		case 298:
		case 299:
		case 300:
		case 301:
		case 302:
		case 303:
		case 304:
		case 305:
		case 306:
		case 307:
			pline("You fall asleep!");
			fall_asleep(-rnd(15 + rnd((monster_difficulty() / 5) + 1) ), TRUE);

		break;

		case 308:
		case 309:

			pline("You are hit by a needle!");
		    poisoned("needle", rn2(6), "poison needle", 30);

		break;

		case 310:
		case 311:
		case 312:
		case 313:
		case 314:
			u_slow_down();

		break;

		case 315:
		case 316:
		case 317:
		case 318:
		case 319:
			pline("A laser beam hits you out of nowhere!");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vy poteryali linii!" : "DWUEUEUET!");
			losehp(monster_difficulty(),"laser beam out of nowhere",KILLED_BY_AN);

		break;

		case 320:
			pline("Suddenly your %s hurts!", body_part(rn2(19) ) );
			losehp(1 + u.chokhmahdamage + rnd(u.ualign.sins > 0 ? (isqrt(u.ualign.sins) + 1) : (1)),"escalating damage effect",KILLED_BY_AN);
			u.chokhmahdamage++;

		break;

		case 321:
			if (u.ugold) {
				u.ugold -= (u.ugold / 5);
				pline("Your purse feels lighter...");
			}

		break;

		case 322:
			if (!Unchanging && !Antimagic) {
				You("undergo a freakish metamorphosis!");
			      polyself(FALSE);
			}

		break;

		case 323:
			You("need reboot.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Eto poshel na khuy vverkh. No chto zhe vy ozhidali? Igra, v kotoruyu vy mozhete legko vyigrat'? Durak!" : "DUEUEDUET!");
			if (!Race_if(PM_UNGENOMOLD)) newman();
			else polyself(FALSE);
		break;

		case 324:
			if (FunnyHallu)
				pline("What a groovy feeling!");
			else
				You(Blind ? "%s and get dizzy..." :
					 "%s and your vision blurs...",
					    stagger(youmonst.data, "stagger"));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Imet' delo s effektami statusa ili sdat'sya!" : "Wrueue-ue-e-ue-e-ue-e...");
			make_stunned(HStun + rn1(7,16) + monster_difficulty(), FALSE);
			(void) make_hallucinated(HHallucination + rn1(7,16) + monster_difficulty(),TRUE,0L);

		break;

		case 325:
			if(!Blind)
				Your("vision bugged.");
			(void) make_hallucinated(HHallucination + rn1(10, 25) + rn1(10, 25) + monster_difficulty() + monster_difficulty(),TRUE,0L);

		break;

		case 326:
		case 327:
			if(!Blind)
				Your("vision turns to screen saver.");
			(void) make_hallucinated(HHallucination + rn1(10, 25) + monster_difficulty(),TRUE,0L);
		break;

		case 328:
			{
			    struct obj *objD = some_armor(&youmonst);

			    if (objD && drain_item(objD)) {
				Your("%s less effective.", aobjnam(objD, "seem"));
				u.cnd_disenchantamount++;
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
			    }
			}

		break;

		case 329:

			if (!Disint_resistance || !rn2(StrongDisint_resistance ? 1000 : 100) && !(evilfriday && (uarms || uarmc || uarm || uarmu)) ) {
				You_feel("like you're falling apart!");
	
				if (uarms) {
				    /* destroy shield; other possessions are safe */
				    if (!(EDisint_resistance & W_ARMS)) (void) destroy_arm(uarms);
				    break;
				} else if (uarmc) {
				    /* destroy cloak; other possessions are safe */
				    if (!(EDisint_resistance & W_ARMC)) (void) destroy_arm(uarmc);
				    break;
				} else if (uarm) {
				    /* destroy suit */
				    if (!(EDisint_resistance & W_ARM)) (void) destroy_arm(uarm);
				    break;
				} else if (uarmu) {
				    /* destroy shirt */
				    if (!(EDisint_resistance & W_ARMU)) (void) destroy_arm(uarmu);
				    break;
				} else {

					if (u.uhpmax > 20) {
						u.uhpmax -= rnd(20);
						if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
						losehp(rnz(100 + level_difficulty()), "a disintegration died", KILLED_BY);
						break;
					} else {
						u.youaredead = 1;
						done(DIED);
						u.youaredead = 0;
					}

				}
	
			}

		break;

		case 330:

			pline("Suddenly a lightning flash hits you!");
		    destroy_item(RING_CLASS, AD_ELEC);
		    destroy_item(WAND_CLASS, AD_ELEC);
		    destroy_item(AMULET_CLASS, AD_ELEC);

		break;

		case 331:
			pline("Suddenly it's freezing cold!");
			destroy_item(POTION_CLASS, AD_COLD);

		break;

		case 332:

			MCReduction += rnz(100 * (monster_difficulty() + 1));
			pline("The magic cancellation granted by your armor seems weaker now...");

		break;

		case 333:

		      u_slow_down();
			u.uprops[DEAC_FAST].intrinsic += (( rnd(10) + rnd(monster_difficulty() + 1) ) * 10);
			pline(u.inertia ? "You feel even slower." : "You slow down to a crawl.");
			u.inertia += (rnd(10) + rnd(monster_difficulty() + 1));

		break;

		case 334:

			if (powerfulimplants() && uimplant && uimplant->oartifact == ART_TIMEAGE_OF_REALMS) break;
		{
		int dmg;
		dmg = (rnd(10) + rnd( (monster_difficulty() * 2) + 1));
		switch (rnd(10)) {

			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				You_feel("life has clocked back.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Zhizn' razgonyal nazad, potomu chto vy ne smotreli, i teper' vy dolzhny poluchit', chto poteryannyy uroven' nazad." : "Kloeck!");
			      losexp("time", FALSE, FALSE); /* resistance is futile :D */
				break;
			case 6:
			case 7:
			case 8:
			case 9:
				switch (rnd(A_MAX)) {
					case A_STR:
						pline("You're not as strong as you used to be...");
						ABASE(A_STR) -= 5;
						if(ABASE(A_STR) < ATTRMIN(A_STR)) {dmg *= 3; ABASE(A_STR) = ATTRMIN(A_STR);}
						break;
					case A_DEX:
						pline("You're not as agile as you used to be...");
						ABASE(A_DEX) -= 5;
						if(ABASE(A_DEX) < ATTRMIN(A_DEX)) {dmg *= 3; ABASE(A_DEX) = ATTRMIN(A_DEX);}
						break;
					case A_CON:
						pline("You're not as hardy as you used to be...");
						ABASE(A_CON) -= 5;
						if(ABASE(A_CON) < ATTRMIN(A_CON)) {dmg *= 3; ABASE(A_CON) = ATTRMIN(A_CON);}
						break;
					case A_WIS:
						pline("You're not as wise as you used to be...");
						ABASE(A_WIS) -= 5;
						if(ABASE(A_WIS) < ATTRMIN(A_WIS)) {dmg *= 3; ABASE(A_WIS) = ATTRMIN(A_WIS);}
						break;
					case A_INT:
						pline("You're not as bright as you used to be...");
						ABASE(A_INT) -= 5;
						if(ABASE(A_INT) < ATTRMIN(A_INT)) {dmg *= 3; ABASE(A_INT) = ATTRMIN(A_INT);}
						break;
					case A_CHA:
						pline("You're not as beautiful as you used to be...");
						ABASE(A_CHA) -= 5;
						if(ABASE(A_CHA) < ATTRMIN(A_CHA)) {dmg *= 3; ABASE(A_CHA) = ATTRMIN(A_CHA);}
						break;
				}
				break;
			case 10:
				pline("You're not as powerful as you used to be...");
				ABASE(A_STR)--;
				ABASE(A_DEX)--;
				ABASE(A_CON)--;
				ABASE(A_WIS)--;
				ABASE(A_INT)--;
				ABASE(A_CHA)--;
				if(ABASE(A_STR) < ATTRMIN(A_STR)) {dmg *= 2; ABASE(A_STR) = ATTRMIN(A_STR);}
				if(ABASE(A_DEX) < ATTRMIN(A_DEX)) {dmg *= 2; ABASE(A_DEX) = ATTRMIN(A_DEX);}
				if(ABASE(A_CON) < ATTRMIN(A_CON)) {dmg *= 2; ABASE(A_CON) = ATTRMIN(A_CON);}
				if(ABASE(A_WIS) < ATTRMIN(A_WIS)) {dmg *= 2; ABASE(A_WIS) = ATTRMIN(A_WIS);}
				if(ABASE(A_INT) < ATTRMIN(A_INT)) {dmg *= 2; ABASE(A_INT) = ATTRMIN(A_INT);}
				if(ABASE(A_CHA) < ATTRMIN(A_CHA)) {dmg *= 2; ABASE(A_CHA) = ATTRMIN(A_CHA);}
				break;
		}
		if (dmg) losehp(dmg, "being timed", KILLED_BY);
		}

		break;

		case 335:

			(void) cancel_monst(&youmonst, (struct obj *)0, FALSE, TRUE, FALSE);

		break;

		case 336:

			forget(3);
			if (!rn2(10)) forget(rnd(25));
			{
			if (!strncmpi(plname, "Maud", 4) || !strncmpi(plalias, "Maud", 4))
				pline("Suddenly, your mind turns inward on itself!");
			else 
				pline("Suddenly, you are overwhelmed by a feeling that urges you to think of Maud.");
			}
			exercise(A_WIS, FALSE);

		break;

		case 337:
			if (IncreasedGravity) pline("Your load feels even heavier!");
			else pline("Your load feels heavier!");

			IncreasedGravity += rnz(5 * (monster_difficulty() + 1));

		break;

		case 338:
			if (Upolyd) u.mhmax--; /* lose one hit point */
			else u.uhpmax--; /* lose one hit point */
			if (u.mhmax < 1) u.mhmax = 1;
			if (u.uhpmax < 1) u.uhpmax = 1;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			if (u.mh > u.mhmax) u.mh = u.mhmax;
			pline("Your health has been drained!");

		break;

		case 339:
			u.uenmax--; /* lose one mana point */
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Your mana has been drained!");

		break;

		case 340:
		case 341:
		case 342:
		case 343:
		case 344:
		case 345:
		case 346:
		case 347:
		case 348:
		case 349:

		You_feel("bad!");
			if (!rn2(20)) losehp(d(10,8), "a bad damage effect", KILLED_BY);
			else if (!rn2(5)) losehp(d(6,8), "a bad damage effect", KILLED_BY);
			else losehp(d(4,6), "a bad damage effect", KILLED_BY);
		break;

		case 350:
		{    register struct obj *objX, *objX2;
		    for (objX = invent; objX; objX = objX2) {
		      objX2 = objX->nobj;
			if (!rn2(5)) rust_dmg(objX, xname(objX), 3, TRUE, &youmonst);
		    }
		}

		break;

		case 351:

			HFumbling = FROMOUTSIDE | rnd(5);
			incr_itimeout(&HFumbling, rnd(20));
			u.fumbleduration += rnz(1000);

		break;

		case 352:
			You_feel("a hole in your %s!", body_part(STOMACH) );
			morehungry(rnd(1000));

		break;

		case 353:
		case 354:
		case 355:
		case 356:
		case 357:
		case 358:
		case 359:
		case 360:
		case 361:
		case 362:
		case 363:
		case 364:
		case 365:
		case 366:
		case 367:
		case 368:
		case 369:
		case 370:
		case 371:
		case 372:

			pushplayer(FALSE); /* since it could be part of a monster attack routine --Amy */

		break;

		case 373:
			if (!Antimagic || !rn2(StrongAntimagic ? 20 : 5)) {
				struct obj *otmp2;

				otmp2 = uwep;
				if (otmp2 && stack_too_big(otmp2)) pline("Your fingers shake violently!");

				else if (otmp2 && otmp2->blessed && rn2(5)) pline("Your fingers shake violently!");
				else if (otmp2 && (otmp2->spe > 1) && (rn2(otmp2->spe)) ) pline("Your fingers shake violently!");
				else if (otmp2 && otmp2->oartifact && rn2(20)) pline("Your fingers shake violently!");
				else if (otmp2 && otmp2->greased) {
					pline("Your fingers shake violently!");
					 if (!rn2(2) || (isfriday && !rn2(2))) {
						pline_The("grease wears off.");
						otmp2->greased -= 1;
						update_inventory();
					 }
				}

				else if (!otmp2) pline("Your fingers itch.");
			      else {
					useupall(otmp2);
					pline("Your weapon evaporates!");
				}

			}

		break;

		case 374:
			if (!rn2(10)) skillcaploss();

		break;

		case 375:
			bad_artifact();
		break;

		case 376:
		case 377:
		case 378:
		case 379:
		case 380:
		if (FunnyHallu) You_feel("totally bad! Your wife is going to abandon you...");
		else pline("You're dimmed!");
		make_dimmed(HDimmed + rnz(150),FALSE);
		break;

		case 381:
		case 382:
		case 383:
		case 384:
		case 385:
		case 386:
		case 387:
		case 388:
		case 389:
		case 390:
			contaminate(rnd(10 + level_difficulty()), FALSE);
		break;
		case 391:
			contaminate(rnz(100 + (level_difficulty() * 10)), FALSE);
		break;

		case 392:
		case 393:
		case 394:
		case 395:
		case 396:
			You_feel("a painful sensation!");
			losehp(Upolyd ? ((u.mh / 10) + 1) : ((u.uhp / 10) + 1), "a painful attack", KILLED_BY);
		break;

		case 397:
			pline("Your %s are trembling!", makeplural(body_part(HAND)));
			u.tremblingamount++;
		break;

		case 398:
		case 399:
		case 400:
			techcapincrease((level_difficulty() + 1) * rnd(50));
		break;

		case 401:
		case 402:
		case 403:
		case 404:
		case 405:
			spellmemoryloss(level_difficulty() + 1);
		break;

		case 406:
		case 407:
		case 408:
		case 409:
		case 410:
			skilltrainingdecrease(level_difficulty() + 1);
		break;

		case 411:
			statdrain();
		break;

		case 412:
		case 413:
		case 414:

			bad_equipment(0);

		break;

		case 415:

			bad_equipment_heel();

		break;

		case 416:

			if (!rn2(64)) {
				ragnarok(TRUE);
				if (evilfriday) evilragnarok(TRUE, level_difficulty());
			} else if (!rn2(evilfriday ? 100 : 10000)) {
				u.datadeletedefer = 1;
				pline("OH MY GOD the dungeon master rolled the jackpot. You're screwed.");
				datadeleteattack();
			}

		break;

		case 417:
			nastytrapcurse();
		break;

		case 418:
		case 419:
		case 420:
		case 421:
		case 422:
		case 423:
		case 424:
		case 425:
		case 426:
		case 427:
			playerbleed(rnd(2 + (level_difficulty() * 10)));
			break;

		case 428:
		case 429:
		case 430:
			shank_player();
			break;

		case 431:
		case 432:
		case 433:
		case 434:
		case 435:
		case 436:
		case 437:
		case 438:
		case 439:
		case 440:
		case 441:
		case 442:
		case 443:
		case 444:
		case 445:
		case 446:
		case 447:
		case 448:
		case 449:
		case 450:
			u.urexp -= ((level_difficulty() + 1) * 50);
			if (u.urexp < 0) u.urexp = 0;
			Your("score is drained!");

			break;

		case 451:
			randomfeminismtrap(rnz( (level_difficulty() + 2) * rnd(50)));
			break;

		case 452:
			otmp = mksobj_at(rnd_class(RIGHT_MOUSE_BUTTON_STONE,NASTY_STONE), u.ux, u.uy, TRUE, FALSE, FALSE);
			if (otmp) {
				otmp->quan = 1L;
				otmp->owt = weight(otmp);
				if (!Blind) otmp->dknown = 1;
				if (otmp) {
					pline("%s lands in your knapsack!", Doname2(otmp));
					(void) pickup_object(otmp, 1L, TRUE, TRUE);
				}
			}
			break;

		case 453:
		case 454:
		case 455:
			statdebuff();
			break;

		case 456:
			if (!rn2(3)) evilspellforget();
			else if (!rn2(2)) eviltechincrease();
			else evilskilldecrease();

			break;

		case 457:
		case 458:

			{
				register struct obj *obj;

				pline("A brown glow surrounds you...");
				for(obj = invent; obj ; obj = obj->nobj)
					if (!rn2(10) && !stack_too_big(obj) && obj->oerodeproof) obj->oerodeproof = FALSE;
			}

			break;

		case 459:
		create_critters(rnz(50), (struct permonst *)0);
		break;

		case 460:
		{coord mm;
		mm.x = u.ux;
		mm.y = u.uy;
		mkundead(&mm, FALSE, 0, FALSE);
		mm.x = u.ux;
		mm.y = u.uy;
		mkundead(&mm, FALSE, 0, FALSE);
		mm.x = u.ux;
		mm.y = u.uy;
		mkundead(&mm, FALSE, 0, FALSE);
		pline("Undead creatures are called forth from the grave!");   
		}
		break;

		case 461:
		You_feel("as if you need some help.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
		rndcurse();
		rndcurse();
		rndcurse();
		break;

		case 462:
		{
		int aligntype;
		aligntype = rn2((int)A_LAWFUL+2) - 1;
		summon_minion(aligntype, TRUE);
		summon_minion(aligntype, TRUE);
		summon_minion(aligntype, TRUE);
		summon_minion(aligntype, TRUE);
		summon_minion(aligntype, TRUE);
		 pline("Servants of %s appear!",aligns[1 - aligntype].noun);
		}
		break;

		case 463:
		pline("You lose  Mana");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vasha magicheskaya energiya udalyayetsya v nastoyashcheye vremya. Skoro on budet raven nulyu, a zatem vy dolzhny igrat' bez zaklinaniy, potomu chto vy sosat', GA GA GA!" : "Due-l-ue-l-ue-l!");
		drain_en(rnz((monster_difficulty() * 10) + 1) );
		break;

		case 464:

			pline(FunnyHallu ? "You feel sinful... but do you really care?" : "You have a feeling of separation.");
			u.ublesscnt += rnz(ishaxor ? 3000 : 6000);

		break;

		case 465:
			deacrandomintrinsic(rnz(10000));
			break;

		case 466:
		    adjalign(-rnd(500));
		    if (flags.soundok) You_hear("a loud rumbling!");
		    if (PlayerHearsSoundEffects) pline(issoviet ? "Pomolis'! Vash bog, bezuslovno, pomozhet vam!" : "Wumm.");

		break;

		case 467:
			pline("Suddenly you're caught in a bluebeam trap!");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vy poteryali linii!" : "DWUEUEUET!");
			losehp(monster_difficulty() * rnd(5),"bluebeam trap out of nowhere",KILLED_BY_AN);

		break;

		case 468:
			pline("Suddenly poison gas surrounds you!");
			destroy_item(POTION_CLASS, AD_VENO);
			destroy_item(FOOD_CLASS, AD_VENO);

		break;

		case 469:
			if (Upolyd) u.mhmax -= 5;
			else u.uhpmax -= 5;
			if (u.mhmax < 1) u.mhmax = 1;
			if (u.uhpmax < 1) u.uhpmax = 1;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			if (u.mh > u.mhmax) u.mh = u.mhmax;
			pline("Your health has been severely drained!");

		break;

		case 470:
			u.uenmax -= 5;
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Your mana has been severely drained!");

		break;

		case 471:
		{    register struct obj *objX, *objX2;
		    for (objX = invent; objX; objX = objX2) {
		      objX2 = objX->nobj;
			if (!rn2(5)) rust_dmg(objX, xname(objX), rn2(4), TRUE, &youmonst);
		    }
		}
		break;

		case 472:
			pline("Your %s are trembling like a leaf!", makeplural(body_part(HAND)));
			u.tremblingamount += rnd(10);
		break;

		case 473:
			playerbleed(rnd(2 + (level_difficulty() * rnd(20))));
			break;

		case 474:
			shank_player();
			shank_player();
			shank_player();
			break;

		case 475:
			statdebuff();
			statdebuff();
			statdebuff();
			statdebuff();
			statdebuff();
			statdebuff();
			break;

		case 476:
			make_confused(HConfusion + rnz(150),FALSE);
			make_stunned(HStun + rnz(150),FALSE);
			make_hallucinated(HHallucination + rnz(150),FALSE,0L);
			make_blinded(Blinded+rnz(150),FALSE);
			make_numbed(HNumbed + rnz(150),FALSE);
			make_frozen(HFrozen + rnz(150),FALSE);
			make_burned(HBurned + rnz(150),FALSE);
			make_feared(HFeared + rnz(150),FALSE);
			make_dimmed(HDimmed + rnz(150),FALSE);
			pline("Suddenly you're afflicted with all the standard status effects!");
			break;

		case 477:
			pline("You're burdened by some dead weight!");

			u.graundweight += rnd(100);
			break;

		case 478:
			if (!rn2(1000)) {
				register struct monst *offmon;
newoffmon:
				if ((offmon = makemon((struct permonst *)0, 0, 0, NO_MM_FLAGS)) != 0) {

					register int inventcount = inv_cnt();

					if (inventcount > 0) {
						inventcount /= 8;
						if (inventcount < 1) inventcount = 1;

						while (inv_cnt() && inventcount) {
							char bufof[BUFSZ];
							bufof[0] = '\0';
							steal(offmon, bufof, TRUE, TRUE);
							inventcount--;
						}

					}

					mdrop_special_objs(offmon); /* make sure it doesn't tele to an unreachable place with the book of the dead or something */
					if (u.uevent.udemigod && !u.freeplaymode) break;
					else u_teleport_monB(offmon, FALSE);
					pline("Some of your possessions have been stolen!");

				} else {
					if (rn2(1000)) goto newoffmon;
					else pline("Somehow you feel that you just averted a major crisis.");
				}

			} else {
				pline("You are surrounded by a purple glow!");
				if (invent) {
				    struct obj *otmpi, *otmpii;
				    int itemportchance = 10 + rn2(21);
				    for (otmpi = invent; otmpi; otmpi = otmpii) {

					otmpii = otmpi->nobj;

					if (!rn2(itemportchance) && !(objects[otmpi->otyp].oc_material == MT_BONE && rn2(10)) && !stack_too_big(otmpi) ) {

						if (otmpi->owornmask & W_ARMOR) {
						    if (otmpi == uskin) {
							skinback(TRUE);		/* uarm = uskin; uskin = 0; */
						    }
						    if (otmpi == uarm) (void) Armor_off();
						    else if (otmpi == uarmc) (void) Cloak_off();
						    else if (otmpi == uarmf) (void) Boots_off();
						    else if (otmpi == uarmg) (void) Gloves_off();
						    else if (otmpi == uarmh) (void) Helmet_off();
						    else if (otmpi == uarms) (void) Shield_off();
						    else if (otmpi == uarmu) (void) Shirt_off();
						    /* catchall -- should never happen */
						    else setworn((struct obj *)0, otmpi ->owornmask & W_ARMOR);
						} else if (otmpi ->owornmask & W_AMUL) {
						    Amulet_off();
						} else if (otmpi ->owornmask & W_IMPLANT) {
						    Implant_off();
						} else if (otmpi ->owornmask & W_RING) {
						    Ring_gone(otmpi);
						} else if (otmpi ->owornmask & W_TOOL) {
						    Blindf_off(otmpi);
						} else if (otmpi ->owornmask & (W_WEP|W_SWAPWEP|W_QUIVER)) {
						    if (otmpi == uwep)
							uwepgone();
						    if (otmpi == uswapwep)
							uswapwepgone();
						    if (otmpi == uquiver)
							uqwepgone();
						}

						if (otmpi->owornmask & (W_BALL|W_CHAIN)) {
						    unpunish();
						} else if (otmpi->owornmask) {
						/* catchall */
						    setnotworn(otmpi);
						}

						dropx(otmpi);
					      if (otmpi->where == OBJ_FLOOR) rloco(otmpi);
						u.cnd_itemportcount++;
					}

				    }
				}

			}

			break;

		case 479:
		case 480:
			pline("You are surrounded by a translucent glow!");
			{
				register struct obj *objX, *objX2;
				for (objX = invent; objX; objX = objX2) {
					objX2 = objX->nobj;
					if (!rn2(5)) objX->oinvis = objX->oinvisreal = FALSE;
				}
			}
			break;

		case 481:
			if (u.enchantrecskill > 0) u.enchantrecskill--;
			if (u.weapchantrecskill > 0) u.weapchantrecskill--;
			if (u.bucskill > 0) u.bucskill--;
			You_feel("less knowledgable about equipment.");
			break;

		case 482:
			otmp = mksobj_at(rnd_class(ELIF_S_JEWEL,DORA_S_JEWEL), u.ux, u.uy, TRUE, FALSE, FALSE);
			if (otmp) {
				otmp->quan = 1L;
				otmp->owt = weight(otmp);
				if (!Blind) otmp->dknown = 1;
				if (otmp) {
					pline("%s stays in your inventory, and you get a bad feeling about it.", Doname2(otmp));
					(void) pickup_object(otmp, 1L, TRUE, TRUE);
				}
			}
			break;

		case 483:
			dropitemattack();
			break;

		default:
			break;
	}

}

void
reallybadeffect()

{

	struct obj *otmp;
	int nastytrapdur;
	int blackngdur;

	if (uarm && objects[(uarm)->otyp].oc_material == MT_LEAD && !rn2(10)) {
		pline("Thanks to your lead armor, you averted misfortune!");
		return;
	}
	if (uarmf && objects[(uarmf)->otyp].oc_material == MT_LEAD && !rn2(10)) {
		pline("Thanks to your lead footwear, you averted misfortune!");
		return;
	}
	if (uarmg && objects[(uarmg)->otyp].oc_material == MT_LEAD && !rn2(10)) {
		pline("Thanks to your lead gloves, you averted misfortune!");
		return;
	}
	if (uarmh && objects[(uarmh)->otyp].oc_material == MT_LEAD && !rn2(10)) {
		pline("Thanks to your lead helmet, you averted misfortune!");
		return;
	}
	if (uarms && objects[(uarms)->otyp].oc_material == MT_LEAD && !rn2(10)) {
		pline("Thanks to your lead shield, you averted misfortune!");
		return;
	}
	if (uarmc && objects[(uarmc)->otyp].oc_material == MT_LEAD && !rn2(10)) {
		pline("Thanks to your lead cloak, you averted misfortune!");
		return;
	}
	if (uarmu && objects[(uarmu)->otyp].oc_material == MT_LEAD && !rn2(10)) {
		pline("Thanks to your lead shirt, you averted misfortune!");
		return;
	}

	if (!(PlayerCannotUseSkills)) {
		int shiichochance = 0;
		switch (P_SKILL(P_SHII_CHO)) {

			case P_BASIC:	shiichochance =  1; break;
			case P_SKILLED:	shiichochance =  2; break;
			case P_EXPERT:	shiichochance =  3; break;
			case P_MASTER:	shiichochance =  4; break;
			case P_GRAND_MASTER:	shiichochance =  5; break;
			case P_SUPREME_MASTER:	shiichochance =  6; break;
			default: shiichochance += 0; break;
		}

		if (shiichochance > rn2(100)) {
			/* avoid bad effect --Amy */
			return;
		}
	}

	u.cnd_reallybadeffectcount++;

	switch (rnd(120)) {

		case 1:
		if (FunnyHallu) You_feel("rather trippy.");
		else You_feel("rather %s.", body_part(LIGHT_HEADED));
		make_confused(HConfusion + rnz(50),FALSE);
		break;

		case 2:
		if (FunnyHallu) You_feel("uncontrollable.");
		else You_feel("stunned.");
		make_stunned(HStun + rnz(50),FALSE);
		break;

		case 3:
		if (FunnyHallu) You_feel("even weirder!");
		else You_feel("weirded out!");
		make_hallucinated(HHallucination + rnz(100),FALSE,0L);
		break;

		case 4:
		if (!Blind) pline("Everything suddenly goes dark.");
		make_blinded(Blinded+rnz(100),FALSE);
		if (!Blind) Your("%s", vision_clears);
		break;

		case 5:
		if (FunnyHallu) You_feel("like your body is trying to fall asleep!");
		else You_feel("numb.");
		make_numbed(HNumbed + rnz(150),FALSE);
		break;

		case 6:
		if (FunnyHallu) You_feel("a giant ice cream cone enclosing you!");
		else pline("You're getting the chills.");
		make_frozen(HFrozen + rnz(150),FALSE);
		break;

		case 7:
		if (FunnyHallu) You_feel("like you have dementia tremor!"); /* not a real name --Amy */
		else pline("Your hands start trembling violently!");
		incr_itimeout(&Glib, rnz(50) );
		break;

		case 8:
		if (FunnyHallu) You_feel("totally down! Seems you tried some illegal shit!");
		else You_feel("like you're going to throw up.");
	      make_vomiting(Vomiting+20, TRUE);
		if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
		break;

		case 9:
		pline("The world spins and goes dark.");
		flags.soundok = 0;
		nomul(-rnd(10), "helplessly knocked out", TRUE);
		nomovemsg = "You are conscious again.";
		afternmv = Hear_again;
		break;

		case 10:
	    make_sick(rn1(25,25), "spreading food poisoning", TRUE, SICK_VOMITABLE);
		break;

		case 11:
		if (!Slimed && !flaming(youmonst.data) && !Unchanging && !slime_on_touch(youmonst.data) ) {
			You("don't feel very well.");
			make_slimed(100);
			stop_occupation();
		    killer_format = KILLED_BY_AN;
		    delayed_killer = "slimed by summoned slime";
		}
		break;

		case 12:

		if ((otmp = mksobj(LOADSTONE, TRUE, FALSE, FALSE)) != (struct obj *)0) {
		You_feel("burdened");
		otmp->quan = 1;
		otmp->owt = weight(otmp);
		if (pickup_object(otmp, 1, FALSE, TRUE) <= 0) {
		obj_extract_self(otmp);
		place_object(otmp, u.ux, u.uy);
		newsym(u.ux, u.uy); }
		}

		break;

		case 13:
		pline("You float up!");
		HLevitation &= ~I_SPECIAL;
		incr_itimeout(&HLevitation, rnz(50));

		break;

		case 14:
		create_critters(rnz(10), (struct permonst *)0);
		break;

		case 15:
		{coord mm;
		mm.x = u.ux;
		mm.y = u.uy;
		pline("Undead creatures are called forth from the grave!");   
		mkundead(&mm, FALSE, 0, FALSE);
		}
		break;

		case 16:
		You_feel("that monsters are aware of your presence.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Dazhe sovetskaya Pyat' Lo obostryayetsya v vashem nizkom igrovom masterstve." : "Woaaaaaah!");
		aggravate();

		break;

		case 17:
		You_feel("as if you need some help.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
		rndcurse();

		break;

		case 18:
			{register struct obj *obj;

			pline("Urgh! You feel a malevolent presence!");
			for(obj = invent; obj ; obj = obj->nobj)
				if (!rn2(5) && !stack_too_big(obj))	curse(obj);
			}
		break;

		case 19:
			if (!Antimagic || !rn2(StrongAntimagic ? 20 : 5)) {
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

		break;

		case 20:
		u.ugangr++;
		if (!rn2(5)) u.ugangr++;
		if (!rn2(25)) u.ugangr++;
		prayer_done();

		break;

		case 21:
			if (!Antimagic || !rn2(StrongAntimagic ? 20 : 5)) {
			    You("suddenly feel weaker!");
			    losestr(rnz(4), TRUE);
			    if (u.uhp < 1) {
				u.youaredead = 1;
				u.uhp = 0;
				killer_format = KILLED_BY;
				killer = "a fatally low strength";
				done(DIED);
				u.youaredead = 0;

				}
			}

		break;

		case 22:
		if ((!StrongSwimming || !rn2(10)) && (!StrongMagical_breathing || !rn2(10))) {
			water_damage(invent, FALSE, FALSE);
			if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);
		}
		if (Burned) make_burned(0L, TRUE);

		break;

		case 23:

		withering_damage(invent, FALSE, FALSE);

		break;

		case 24:
		if (!Stoned && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && !(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM)) ) {
			if (Hallucination && rn2(10)) pline("You are already stoned.");
			else {
				You("start turning to stone!");
				Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
				u.cnd_stoningcount++;
				stop_occupation();
				delayed_killer = "bad petrification effect";
			}
		}

		break;

		case 25:
		{
		int aligntype;
		aligntype = rn2((int)A_LAWFUL+2) - 1;
		 pline("A servant of %s appears!",aligns[1 - aligntype].noun);
		summon_minion(aligntype, TRUE);
		}
		break;

		case 26:
	      attrcurse();

		break;

		case 27:
		pline("It gets dark!");
	    if (!shadowprotection()) do_clear_areaX(u.ux,u.uy,		/* darkness around player's position */
		15, set_litI, (void *)((char *)0));

		break;

		case 28:
		{	int rtrap;
		    int i, j, bd;
			bd = 1;
			if (!rn2(5)) bd += rnz(1);

		      for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
				if (!isok(u.ux + i, u.uy + j)) continue;
				if (levl[u.ux + i][u.uy + j].typ <= DBWALL) continue;
				if (t_at(u.ux + i, u.uy + j)) continue;

			      rtrap = randomtrap();
				if (!rn2(20)) makerandomtrap(TRUE);

				(void) maketrap(u.ux + i, u.uy + j, rtrap, 100, TRUE);
			}
			makerandomtrap(TRUE);
			if (!rn2(3)) makerandomtrap(TRUE);
		}
		break;

		case 29:
	      You_feel("yanked in a new direction!");
		(void) safe_teleds(FALSE);

		break;

		case 30:
		pline("You lose  Mana");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vasha magicheskaya energiya udalyayetsya v nastoyashcheye vremya. Skoro on budet raven nulyu, a zatem vy dolzhny igrat' bez zaklinaniy, potomu chto vy sosat', GA GA GA!" : "Due-l-ue-l-ue-l!");
		drain_en(rnz(monster_difficulty() + 1) );
		break;

		case 31:
	      You_feel("out of luck!");
			change_luck(-1);
			if (!rn2(10)) change_luck(-5);
			adjalign(-10);
			if (!rn2(10)) adjalign(-50);

		break;

		case 32:
		punishx();
		break;

		case 33:
		{int copcnt;
		copcnt = rnd(monster_difficulty() ) + 1;
		if (rn2(5)) copcnt = (copcnt / (rnd(4) + 1)) + 1;
		if (rn2(5)) copcnt /= 2;
		if (!rn2(5)) copcnt /= 2; /* don't make too many */
		if (!rn2(10)) copcnt /= 3;
		if (copcnt < 1) copcnt = 1;

		u.cnd_kopsummonamount++;

		if (uarmh && itemhasappearance(uarmh, APP_ANTI_GOVERNMENT_HELMET) ) {
			copcnt = (copcnt / 2) + 1;
		}

		if (RngeAntiGovernment) {
			copcnt = (copcnt / 2) + 1;
		}

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

	      while(--copcnt >= 0) {
			(void) makemon(mkclass(S_KOP,0), u.ux, u.uy, MM_ANGRY);

			if (!rn2(500)) {

				int koptryct = 0;
				int kox, koy;

				for (koptryct = 0; koptryct < 2000; koptryct++) {
					kox = rn1(COLNO-3,2);
					koy = rn2(ROWNO);

					if (kox && koy && isok(kox, koy) && (levl[kox][koy].typ > DBWALL) && !(t_at(kox, koy)) ) {
						(void) maketrap(kox, koy, KOP_CUBE, 0, FALSE);
						break;
						}
				}

			}

		} /* while */

		u.aggravation = 0;

		}
		break;

		case 34:
			losexp((char *)0, TRUE, FALSE); 

		break;

		case 35:
		    attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse();

		break;

		case 36:

			pline(FunnyHallu ? "You feel sinful... but do you really care?" : "You have a feeling of separation.");
			u.ublesscnt += rnz(ishaxor ? 150 : 300);

		break;

		case 37:
		if (FunnyHallu) You_feel("totally hot! Oh yeah, baby!");
		else pline("You're burning!");
		make_burned(HBurned + rnz(150),FALSE);
		break;

		case 38:
		if (FunnyHallu) You("panic! The alarm bells are ringing and you don't know how to get out!");
		else You_feel("afraid.");
		make_feared(HFeared + rnz(150),FALSE);
		break;

		case 39:
			deacrandomintrinsic(rnz(200));
			break;

		case 40:

		nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
		if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
		blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
		if (!blackngdur ) blackngdur = 500; /* fail safe */

		randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), blackngdur - (monster_difficulty() * 3));

		break;

		case 41:
		    adjalign(-rnd(20));
		    if (flags.soundok) You_hear("a slight rumbling...");
		    if (PlayerHearsSoundEffects) pline(issoviet ? "Pomolis'! Vash bog, bezuslovno, pomozhet vam!" : "Wumm.");

		break;

		case 42:

		    adjalign(-rnd(50));
			u.ualign.sins++;
			u.alignlim--;
		    if (flags.soundok) You_hear("a thunderous rumbling!");
		    if (PlayerHearsSoundEffects) pline(issoviet ? "Seychas ideal'noye vremya dlya molitvy! Vash bog super schastliv s toboy i, bezuslovno, vylechit' vse bolezni i predostavit' vam moshchnyy punkt!" : "Wummmmm! Wummmmm! Grummel!");

		break;

		case 43:

			pline("You are engulfed in flames!");
			(void) burnarmor(&youmonst);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5)) destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5)) destroy_item(SPBOOK_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5)) destroy_item(POTION_CLASS, AD_FIRE);

		break;

		case 44:
		{	
		      int bd = rnd(10);
			if (!rn2(5)) bd += rnz(10);

			while (bd-- >= 0) makerandomtrap(TRUE);

		}
		break;

		case 45:
			pline("You fall asleep!");
			fall_asleep(-rnd(15 + rnd((monster_difficulty() / 5) + 1) ), TRUE);

		break;

		case 46:

			pline("You are hit by a needle!");
		    poisoned("needle", rn2(6), "poison needle", 30);

		break;

		case 47:
			u_slow_down();

		break;

		case 48:
			pline("A laser beam hits you out of nowhere!");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vy poteryali linii!" : "DWUEUEUET!");
			losehp(monster_difficulty(),"laser beam out of nowhere",KILLED_BY_AN);

		break;

		case 49:
			pline("Suddenly your %s hurts!", body_part(rn2(19) ) );
			losehp(1 + u.chokhmahdamage + rnd(u.ualign.sins > 0 ? (isqrt(u.ualign.sins) + 1) : (1)),"escalating damage effect",KILLED_BY_AN);
			u.chokhmahdamage++;

		break;

		case 50:
			if (u.ugold) {
				u.ugold -= (u.ugold / 5);
				pline("Your purse feels lighter...");
			}

		break;

		case 51:
			if (!Unchanging && !Antimagic) {
				You("undergo a freakish metamorphosis!");
			      polyself(FALSE);
			}

		break;

		case 52:
			You("need reboot.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Eto poshel na khuy vverkh. No chto zhe vy ozhidali? Igra, v kotoruyu vy mozhete legko vyigrat'? Durak!" : "DUEUEDUET!");
			if (!Race_if(PM_UNGENOMOLD)) newman();
			else polyself(FALSE);
		break;

		case 53:
			if (FunnyHallu)
				pline("What a groovy feeling!");
			else
				You(Blind ? "%s and get dizzy..." :
					 "%s and your vision blurs...",
					    stagger(youmonst.data, "stagger"));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Imet' delo s effektami statusa ili sdat'sya!" : "Wrueue-ue-e-ue-e-ue-e...");
			make_stunned(HStun + rn1(7,16) + monster_difficulty(), FALSE);
			(void) make_hallucinated(HHallucination + rn1(7,16) + monster_difficulty(),TRUE,0L);

		break;

		case 54:
			if(!Blind)
				Your("vision bugged.");
			(void) make_hallucinated(HHallucination + rn1(10, 25) + rn1(10, 25) + monster_difficulty() + monster_difficulty(),TRUE,0L);

		break;

		case 55:
			if(!Blind)
				Your("vision turns to screen saver.");
			(void) make_hallucinated(HHallucination + rn1(10, 25) + monster_difficulty(),TRUE,0L);
		break;

		case 56:
			{
			    struct obj *objD = some_armor(&youmonst);

			    if (objD && drain_item(objD)) {
				Your("%s less effective.", aobjnam(objD, "seem"));
				u.cnd_disenchantamount++;
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
			    }
			}

		break;

		case 57:

			if (!Disint_resistance || !rn2(StrongDisint_resistance ? 1000 : 100) && !(evilfriday && (uarms || uarmc || uarm || uarmu)) ) {
				You_feel("like you're falling apart!");
	
				if (uarms) {
				    /* destroy shield; other possessions are safe */
				    if (!(EDisint_resistance & W_ARMS)) (void) destroy_arm(uarms);
				    break;
				} else if (uarmc) {
				    /* destroy cloak; other possessions are safe */
				    if (!(EDisint_resistance & W_ARMC)) (void) destroy_arm(uarmc);
				    break;
				} else if (uarm) {
				    /* destroy suit */
				    if (!(EDisint_resistance & W_ARM)) (void) destroy_arm(uarm);
				    break;
				} else if (uarmu) {
				    /* destroy shirt */
				    if (!(EDisint_resistance & W_ARMU)) (void) destroy_arm(uarmu);
				    break;
				} else {

					if (u.uhpmax > 20) {
						u.uhpmax -= rnd(20);
						if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
						losehp(rnz(100 + level_difficulty()), "a disintegration died", KILLED_BY);
						break;
					} else {
						u.youaredead = 1;
						done(DIED);
						u.youaredead = 0;
					}

				}
	
			}

		break;

		case 58:

			pline("Suddenly a lightning flash hits you!");
		    destroy_item(RING_CLASS, AD_ELEC);
		    destroy_item(WAND_CLASS, AD_ELEC);
		    destroy_item(AMULET_CLASS, AD_ELEC);

		break;

		case 59:
			pline("Suddenly it's freezing cold!");
			destroy_item(POTION_CLASS, AD_COLD);

		break;

		case 60:

			MCReduction += rnz(100 * (monster_difficulty() + 1));
			pline("The magic cancellation granted by your armor seems weaker now...");

		break;

		case 61:

		      u_slow_down();
			u.uprops[DEAC_FAST].intrinsic += (( rnd(10) + rnd(monster_difficulty() + 1) ) * 10);
			pline(u.inertia ? "You feel even slower." : "You slow down to a crawl.");
			u.inertia += (rnd(10) + rnd(monster_difficulty() + 1));

		break;

		case 62:

			if (powerfulimplants() && uimplant && uimplant->oartifact == ART_TIMEAGE_OF_REALMS) break;
		{
		int dmg;
		dmg = (rnd(10) + rnd( (monster_difficulty() * 2) + 1));
		switch (rnd(10)) {

			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				You_feel("life has clocked back.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Zhizn' razgonyal nazad, potomu chto vy ne smotreli, i teper' vy dolzhny poluchit', chto poteryannyy uroven' nazad." : "Kloeck!");
			      losexp("time", FALSE, FALSE); /* resistance is futile :D */
				break;
			case 6:
			case 7:
			case 8:
			case 9:
				switch (rnd(A_MAX)) {
					case A_STR:
						pline("You're not as strong as you used to be...");
						ABASE(A_STR) -= 5;
						if(ABASE(A_STR) < ATTRMIN(A_STR)) {dmg *= 3; ABASE(A_STR) = ATTRMIN(A_STR);}
						break;
					case A_DEX:
						pline("You're not as agile as you used to be...");
						ABASE(A_DEX) -= 5;
						if(ABASE(A_DEX) < ATTRMIN(A_DEX)) {dmg *= 3; ABASE(A_DEX) = ATTRMIN(A_DEX);}
						break;
					case A_CON:
						pline("You're not as hardy as you used to be...");
						ABASE(A_CON) -= 5;
						if(ABASE(A_CON) < ATTRMIN(A_CON)) {dmg *= 3; ABASE(A_CON) = ATTRMIN(A_CON);}
						break;
					case A_WIS:
						pline("You're not as wise as you used to be...");
						ABASE(A_WIS) -= 5;
						if(ABASE(A_WIS) < ATTRMIN(A_WIS)) {dmg *= 3; ABASE(A_WIS) = ATTRMIN(A_WIS);}
						break;
					case A_INT:
						pline("You're not as bright as you used to be...");
						ABASE(A_INT) -= 5;
						if(ABASE(A_INT) < ATTRMIN(A_INT)) {dmg *= 3; ABASE(A_INT) = ATTRMIN(A_INT);}
						break;
					case A_CHA:
						pline("You're not as beautiful as you used to be...");
						ABASE(A_CHA) -= 5;
						if(ABASE(A_CHA) < ATTRMIN(A_CHA)) {dmg *= 3; ABASE(A_CHA) = ATTRMIN(A_CHA);}
						break;
				}
				break;
			case 10:
				pline("You're not as powerful as you used to be...");
				ABASE(A_STR)--;
				ABASE(A_DEX)--;
				ABASE(A_CON)--;
				ABASE(A_WIS)--;
				ABASE(A_INT)--;
				ABASE(A_CHA)--;
				if(ABASE(A_STR) < ATTRMIN(A_STR)) {dmg *= 2; ABASE(A_STR) = ATTRMIN(A_STR);}
				if(ABASE(A_DEX) < ATTRMIN(A_DEX)) {dmg *= 2; ABASE(A_DEX) = ATTRMIN(A_DEX);}
				if(ABASE(A_CON) < ATTRMIN(A_CON)) {dmg *= 2; ABASE(A_CON) = ATTRMIN(A_CON);}
				if(ABASE(A_WIS) < ATTRMIN(A_WIS)) {dmg *= 2; ABASE(A_WIS) = ATTRMIN(A_WIS);}
				if(ABASE(A_INT) < ATTRMIN(A_INT)) {dmg *= 2; ABASE(A_INT) = ATTRMIN(A_INT);}
				if(ABASE(A_CHA) < ATTRMIN(A_CHA)) {dmg *= 2; ABASE(A_CHA) = ATTRMIN(A_CHA);}
				break;
		}
		if (dmg) losehp(dmg, "being timed", KILLED_BY);
		}

		break;

		case 63:

			(void) cancel_monst(&youmonst, (struct obj *)0, FALSE, TRUE, FALSE);

		break;

		case 64:

			forget(3);
			if (!rn2(10)) forget(rnd(25));
			{
			if (!strncmpi(plname, "Maud", 4) || !strncmpi(plalias, "Maud", 4))
				pline("Suddenly, your mind turns inward on itself!");
			else 
				pline("Suddenly, you are overwhelmed by a feeling that urges you to think of Maud.");
			}
			exercise(A_WIS, FALSE);

		break;

		case 65:
			if (IncreasedGravity) pline("Your load feels even heavier!");
			else pline("Your load feels heavier!");

			IncreasedGravity += rnz(5 * (monster_difficulty() + 1));

		break;

		case 66:
			if (Upolyd) u.mhmax--; /* lose one hit point */
			else u.uhpmax--; /* lose one hit point */
			if (u.mhmax < 1) u.mhmax = 1;
			if (u.uhpmax < 1) u.uhpmax = 1;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			if (u.mh > u.mhmax) u.mh = u.mhmax;
			pline("Your health has been drained!");

		break;

		case 67:
			u.uenmax--; /* lose one mana point */
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Your mana has been drained!");

		break;

		case 68:

		You_feel("bad!");
			if (!rn2(20)) losehp(d(10,8), "a bad damage effect", KILLED_BY);
			else if (!rn2(5)) losehp(d(6,8), "a bad damage effect", KILLED_BY);
			else losehp(d(4,6), "a bad damage effect", KILLED_BY);
		break;

		case 69:
		{    register struct obj *objX, *objX2;
		    for (objX = invent; objX; objX = objX2) {
		      objX2 = objX->nobj;
			if (!rn2(5)) rust_dmg(objX, xname(objX), 3, TRUE, &youmonst);
		    }
		}

		break;

		case 70:

			HFumbling = FROMOUTSIDE | rnd(5);
			incr_itimeout(&HFumbling, rnd(20));
			u.fumbleduration += rnz(1000);

		break;

		case 71:
			You_feel("a hole in your %s!", body_part(STOMACH) );
			morehungry(rnd(1000));

		break;

		case 72:

			pushplayer(FALSE); /* since it could be part of a monster attack routine --Amy */

		break;

		case 73:
			if (!Antimagic || !rn2(StrongAntimagic ? 20 : 5)) {
				struct obj *otmp2;

				otmp2 = uwep;
				if (otmp2 && stack_too_big(otmp2)) pline("Your fingers shake violently!");

				else if (otmp2 && otmp2->blessed && rn2(5)) pline("Your fingers shake violently!");
				else if (otmp2 && (otmp2->spe > 1) && (rn2(otmp2->spe)) ) pline("Your fingers shake violently!");
				else if (otmp2 && otmp2->oartifact && rn2(20)) pline("Your fingers shake violently!");
				else if (otmp2 && otmp2->greased) {
					pline("Your fingers shake violently!");
					 if (!rn2(2) || (isfriday && !rn2(2))) {
						pline_The("grease wears off.");
						otmp2->greased -= 1;
						update_inventory();
					 }
				}

				else if (!otmp2) pline("Your fingers itch.");
			      else {
					useupall(otmp2);
					pline("Your weapon evaporates!");
				}

			}

		break;

		case 74:
			if (!rn2(10)) skillcaploss();

		break;

		case 75:
			bad_artifact();
		break;

		case 76:
		if (FunnyHallu) You_feel("totally bad! Your wife is going to abandon you...");
		else pline("You're dimmed!");
		make_dimmed(HDimmed + rnz(150),FALSE);
		break;

		case 77:
			contaminate(rnd(10 + level_difficulty()), FALSE);
		break;
		case 78:
			contaminate(rnz(100 + (level_difficulty() * 10)), FALSE);
		break;

		case 79:
			You_feel("a painful sensation!");
			losehp(Upolyd ? ((u.mh / 10) + 1) : ((u.uhp / 10) + 1), "a painful attack", KILLED_BY);
		break;

		case 80:
			pline("Your %s are trembling!", makeplural(body_part(HAND)));
			u.tremblingamount++;
		break;

		case 81:
			techcapincrease((level_difficulty() + 1) * rnd(50));
		break;

		case 82:
			spellmemoryloss(level_difficulty() + 1);
		break;

		case 83:
			skilltrainingdecrease(level_difficulty() + 1);
		break;

		case 84:
			statdrain();
		break;

		case 85:

			bad_equipment(0);

		break;

		case 86:

			bad_equipment_heel();

		break;

		case 87:

			if (!rn2(64)) {
				ragnarok(TRUE);
				if (evilfriday) evilragnarok(TRUE, level_difficulty());
			} else if (!rn2(evilfriday ? 100 : 10000)) {
				pline("OH MY GOD the dungeon master rolled the jackpot. You're screwed.");
				u.datadeletedefer = 1;
				datadeleteattack();
			}

		break;

		case 88:
			nastytrapcurse();
		break;

		case 89:
			playerbleed(rnd(2 + (level_difficulty() * 10)));
			break;

		case 90:
			shank_player();
			break;

		case 91:
			u.urexp -= ((level_difficulty() + 1) * 50);
			if (u.urexp < 0) u.urexp = 0;
			Your("score is drained!");

			break;

		case 92:
			randomfeminismtrap(rnz( (level_difficulty() + 2) * rnd(50)));
			break;

		case 93:
			otmp = mksobj_at(rnd_class(RIGHT_MOUSE_BUTTON_STONE,NASTY_STONE), u.ux, u.uy, TRUE, FALSE, FALSE);
			if (otmp) {
				otmp->quan = 1L;
				otmp->owt = weight(otmp);
				if (!Blind) otmp->dknown = 1;
				if (otmp) {
					pline("%s lands in your knapsack!", Doname2(otmp));
					(void) pickup_object(otmp, 1L, TRUE, TRUE);
				}
			}
			break;

		case 94:
			statdebuff();
			break;

		case 95:
			if (!rn2(3)) evilspellforget();
			else if (!rn2(2)) eviltechincrease();
			else evilskilldecrease();

			break;

		case 96:
			{
				register struct obj *obj;

				pline("A brown glow surrounds you...");
				for(obj = invent; obj ; obj = obj->nobj)
					if (!rn2(10) && !stack_too_big(obj) && obj->oerodeproof) obj->oerodeproof = FALSE;
			}
			break;

		case 97:
		create_critters(rnz(50), (struct permonst *)0);
		break;

		case 98:
		{coord mm;
		mm.x = u.ux;
		mm.y = u.uy;
		mkundead(&mm, FALSE, 0, FALSE);
		mm.x = u.ux;
		mm.y = u.uy;
		mkundead(&mm, FALSE, 0, FALSE);
		mm.x = u.ux;
		mm.y = u.uy;
		mkundead(&mm, FALSE, 0, FALSE);
		pline("Undead creatures are called forth from the grave!");   
		}
		break;

		case 99:
		You_feel("as if you need some help.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
		rndcurse();
		rndcurse();
		rndcurse();
		break;

		case 100:
		{
		int aligntype;
		aligntype = rn2((int)A_LAWFUL+2) - 1;
		summon_minion(aligntype, TRUE);
		summon_minion(aligntype, TRUE);
		summon_minion(aligntype, TRUE);
		summon_minion(aligntype, TRUE);
		summon_minion(aligntype, TRUE);
		 pline("Servants of %s appear!",aligns[1 - aligntype].noun);
		}
		break;

		case 101:
		pline("You lose  Mana");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vasha magicheskaya energiya udalyayetsya v nastoyashcheye vremya. Skoro on budet raven nulyu, a zatem vy dolzhny igrat' bez zaklinaniy, potomu chto vy sosat', GA GA GA!" : "Due-l-ue-l-ue-l!");
		drain_en(rnz((monster_difficulty() * 10) + 1) );
		break;

		case 102:

			pline(FunnyHallu ? "You feel sinful... but do you really care?" : "You have a feeling of separation.");
			u.ublesscnt += rnz(ishaxor ? 3000 : 6000);

		break;

		case 103:
			deacrandomintrinsic(rnz(10000));
			break;

		case 104:
		    adjalign(-rnd(500));
		    if (flags.soundok) You_hear("a loud rumbling!");
		    if (PlayerHearsSoundEffects) pline(issoviet ? "Pomolis'! Vash bog, bezuslovno, pomozhet vam!" : "Wumm.");

		break;

		case 105:
			pline("Suddenly you're caught in a bluebeam trap!");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vy poteryali linii!" : "DWUEUEUET!");
			losehp(monster_difficulty() * rnd(5),"bluebeam trap out of nowhere",KILLED_BY_AN);

		break;

		case 106:
			pline("Suddenly poison gas surrounds you!");
			destroy_item(POTION_CLASS, AD_VENO);
			destroy_item(FOOD_CLASS, AD_VENO);

		break;

		case 107:
			if (Upolyd) u.mhmax -= 5;
			else u.uhpmax -= 5;
			if (u.mhmax < 1) u.mhmax = 1;
			if (u.uhpmax < 1) u.uhpmax = 1;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			if (u.mh > u.mhmax) u.mh = u.mhmax;
			pline("Your health has been severely drained!");

		break;

		case 108:
			u.uenmax -= 5;
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Your mana has been severely drained!");

		break;

		case 109:
		{    register struct obj *objX, *objX2;
		    for (objX = invent; objX; objX = objX2) {
		      objX2 = objX->nobj;
			if (!rn2(5)) rust_dmg(objX, xname(objX), rn2(4), TRUE, &youmonst);
		    }
		}
		break;

		case 110:
			pline("Your %s are trembling like a leaf!", makeplural(body_part(HAND)));
			u.tremblingamount += rnd(10);
		break;

		case 111:
			playerbleed(rnd(2 + (level_difficulty() * rnd(20))));
			break;

		case 112:
			shank_player();
			shank_player();
			shank_player();
			break;

		case 113:
			statdebuff();
			statdebuff();
			statdebuff();
			statdebuff();
			statdebuff();
			statdebuff();
			break;

		case 114:
			make_confused(HConfusion + rnz(150),FALSE);
			make_stunned(HStun + rnz(150),FALSE);
			make_hallucinated(HHallucination + rnz(150),FALSE,0L);
			make_blinded(Blinded+rnz(150),FALSE);
			make_numbed(HNumbed + rnz(150),FALSE);
			make_frozen(HFrozen + rnz(150),FALSE);
			make_burned(HBurned + rnz(150),FALSE);
			make_feared(HFeared + rnz(150),FALSE);
			make_dimmed(HDimmed + rnz(150),FALSE);
			pline("Suddenly you're afflicted with all the standard status effects!");
			break;

		case 115:
			pline("You're burdened by some dead weight!");

			u.graundweight += rnd(100);
			break;

		case 116:
			if (!rn2(1000)) {
				register struct monst *offmon;
newoffmonX:
				if ((offmon = makemon((struct permonst *)0, 0, 0, NO_MM_FLAGS)) != 0) {

					register int inventcount = inv_cnt();

					if (inventcount > 0) {
						inventcount /= 8;
						if (inventcount < 1) inventcount = 1;

						while (inv_cnt() && inventcount) {
							char bufof[BUFSZ];
							bufof[0] = '\0';
							steal(offmon, bufof, TRUE, TRUE);
							inventcount--;
						}

					}

					mdrop_special_objs(offmon); /* make sure it doesn't tele to an unreachable place with the book of the dead or something */
					if (u.uevent.udemigod && !u.freeplaymode) break;
					else u_teleport_monB(offmon, FALSE);
					pline("Some of your possessions have been stolen!");

				} else {
					if (rn2(1000)) goto newoffmonX;
					else pline("Somehow you feel that you just averted a major crisis.");
				}

			} else {
				pline("You are surrounded by a purple glow!");
				if (invent) {
				    struct obj *otmpi, *otmpii;
				    int itemportchance = 10 + rn2(21);
				    for (otmpi = invent; otmpi; otmpi = otmpii) {

					otmpii = otmpi->nobj;

					if (!rn2(itemportchance) && !(objects[otmpi->otyp].oc_material == MT_BONE && rn2(10)) && !stack_too_big(otmpi) ) {

						if (otmpi->owornmask & W_ARMOR) {
						    if (otmpi == uskin) {
							skinback(TRUE);		/* uarm = uskin; uskin = 0; */
						    }
						    if (otmpi == uarm) (void) Armor_off();
						    else if (otmpi == uarmc) (void) Cloak_off();
						    else if (otmpi == uarmf) (void) Boots_off();
						    else if (otmpi == uarmg) (void) Gloves_off();
						    else if (otmpi == uarmh) (void) Helmet_off();
						    else if (otmpi == uarms) (void) Shield_off();
						    else if (otmpi == uarmu) (void) Shirt_off();
						    /* catchall -- should never happen */
						    else setworn((struct obj *)0, otmpi ->owornmask & W_ARMOR);
						} else if (otmpi ->owornmask & W_AMUL) {
						    Amulet_off();
						} else if (otmpi ->owornmask & W_IMPLANT) {
						    Implant_off();
						} else if (otmpi ->owornmask & W_RING) {
						    Ring_gone(otmpi);
						} else if (otmpi ->owornmask & W_TOOL) {
						    Blindf_off(otmpi);
						} else if (otmpi ->owornmask & (W_WEP|W_SWAPWEP|W_QUIVER)) {
						    if (otmpi == uwep)
							uwepgone();
						    if (otmpi == uswapwep)
							uswapwepgone();
						    if (otmpi == uquiver)
							uqwepgone();
						}

						if (otmpi->owornmask & (W_BALL|W_CHAIN)) {
						    unpunish();
						} else if (otmpi->owornmask) {
						/* catchall */
						    setnotworn(otmpi);
						}

						dropx(otmpi);
					      if (otmpi->where == OBJ_FLOOR) rloco(otmpi);
						u.cnd_itemportcount++;
					}

				    }
				}

			}

			break;

		case 117:
			pline("You are surrounded by a translucent glow!");
			{
				register struct obj *objX, *objX2;
				for (objX = invent; objX; objX = objX2) {
					objX2 = objX->nobj;
					if (!rn2(5)) objX->oinvis = objX->oinvisreal = FALSE;
				}
			}
			break;

		case 118:
			if (u.enchantrecskill > 0) u.enchantrecskill--;
			if (u.weapchantrecskill > 0) u.weapchantrecskill--;
			if (u.bucskill > 0) u.bucskill--;
			You_feel("less knowledgable about equipment.");
			break;

		case 119:
			otmp = mksobj_at(rnd_class(ELIF_S_JEWEL,DORA_S_JEWEL), u.ux, u.uy, TRUE, FALSE, FALSE);
			if (otmp) {
				otmp->quan = 1L;
				otmp->owt = weight(otmp);
				if (!Blind) otmp->dknown = 1;
				if (otmp) {
					pline("%s stays in your inventory, and you get a bad feeling about it.", Doname2(otmp));
					(void) pickup_object(otmp, 1L, TRUE, TRUE);
				}
			}
			break;

		case 120:
			dropitemattack();
			break;

		default:
		break;
	}

}

void
goodeffect()
{
	/* todo */
}

/* hybridragon race by rikersan: cannot be resistant to its own breath */
boolean
hybridragontype(attacktype)
int attacktype;
{
	if (!Race_if(PM_HYBRIDRAGON)) return FALSE;
	if (dmgtype(&mons[PM_HYBRIDRAGON], attacktype) ) return TRUE;
	return FALSE;
}

boolean
extralongsqueak()
{
	if (tech_inuse(T_EXTRA_LONG_SQUEAK)) {
		int squeakchance = 20;
		squeakchance += techlevX(get_tech_no(T_EXTRA_LONG_SQUEAK));
		if (rnd(100) < squeakchance) {
			pline("Your own farting noises deflect the hostile farting gas!");
			return TRUE;
		}
	}
	return FALSE;
}

void
statdrain()
{
	int statdrained = rn2(A_MAX);
	if (ABASE(statdrained) < 4) return;
	if (ABASE(statdrained) < (3 + rnd(8)) ) return;

	if (Race_if(PM_SUSTAINER) && rn2(50)) {
		pline("The stat drain doesn't seem to affect you.");
		return;
	}
	if (Role_if(PM_ASTRONAUT) && rn2(2)) {
		pline("Your steeled body prevents the stat loss!");
		return;
	}

	u.cnd_permstatdamageamount++;

	ABASE(statdrained) -= 1;
	AMAX(statdrained) -= 1;
	flags.botl = 1;
	switch (statdrained) {

		case A_STR:
			pline("Your strength falls off!"); break;
		case A_DEX:
			pline("Your dexterity falls off!"); break;
		case A_CON:
			pline("Your constitution falls off!"); break;
		case A_CHA:
			pline("Your charisma falls off!"); break;
		case A_INT:
			pline("Your intelligence falls off!"); break;
		case A_WIS:
			pline("Your wisdom falls off!"); break;

	}

}

void
ragnarok(guaranteed)
boolean guaranteed;
{
	register int x,y;

	if (rn2(64) && !guaranteed) return;

	u.aggravation = 1;
	u.heavyaggravation = 1;
	DifficultyIncreased += 1;
	HighlevelStatus += 1;
	EntireLevelMode += 1;
	if (!rn2(5)) DifficultyIncreased += rnz(100);
	if (!rn2(5)) HighlevelStatus += rnz(100);
	if (!rn2(5)) EntireLevelMode += rnz(100);

	pline("Let's Ragnarok!");
	u.cnd_ragnarokamount++;

	FalloutEffect += rnz(200);

	for (x = 0; x < COLNO; x++)
	  for (y = 0; y < ROWNO; y++) {

		if (isok(x,y) && (IS_STWALL(levl[x][y].typ) || levl[x][y].typ == ROOM || levl[x][y].typ == LAVAPOOL || levl[x][y].typ == CORR) && levl[x][y].typ != SDOOR && ((levl[x][y].wall_info & W_NONDIGGABLE) == 0) && !(*in_rooms(x,y,SHOPBASE)) && !rn2(5) ) {

			levl[x][y].typ = LAVAPOOL;
			blockorunblock_point(x,y);
			if (!(levl[x][y].wall_info & W_HARDGROWTH)) levl[x][y].wall_info |= W_EASYGROWTH;
			newsym(x, y);

			if (!rn2(3)) switch (rnd(10)) {
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
					(void) makemon(mkclass(S_DRAGON,0), x, y, rn2(10) ? MM_ADJACENTOK|MM_ANGRY : MM_ADJACENTOK|MM_ANGRY|MM_FRENZIED);
					break;
				case 6:
				case 7:
				case 8:
					(void) makemon(mkclass(S_GIANT,0), x, y, rn2(10) ? MM_ADJACENTOK|MM_ANGRY : MM_ADJACENTOK|MM_ANGRY|MM_FRENZIED);
					break;
				case 9:
				case 10:
					(void) makemon(mkclass(S_DEMON,0), x, y, rn2(10) ? MM_ADJACENTOK|MM_ANGRY : MM_ADJACENTOK|MM_ANGRY|MM_FRENZIED);
					break;
			}

		}

	}

	if (!rn2(5)) create_mplayers(rnd(12), TRUE);

	u.aggravation = 0;
	u.heavyaggravation = 0;

	stop_occupation();

}

void
evilragnarok(guaranteed, wflevel)
boolean guaranteed;
int wflevel;
{
	register struct monst *mtmp, *mtmp2;

	if (rn2(64) && !guaranteed) return;

	for (mtmp = fmon; mtmp; mtmp = mtmp2) {
		mtmp2 = mtmp->nmon;
		if ((mtmp->m_lev < wflevel) && mtmp->mtame) {
			if (u.usteed && mtmp == u.usteed) dismount_steed(DISMOUNT_GENERIC);
			mondead(mtmp);
			pline("One of your pets has died.");
		}
	}
	if (wflevel > u.ulevel) {
		u.youaredead = 1;
		pline("The world ends and you are dead. Goodbye.");
		killer_format = NO_KILLER_PREFIX;
		killer = "world fall";
		done(DIED);
		u.youaredead = 0;

	}

	stop_occupation();

}

void
destroyarmorattack()
{

	struct obj *otmp2;

	if (Antimagic && rn2(StrongAntimagic ? 20 : 5)) {
		shieldeff(u.ux, u.uy);
		pline("A field of force surrounds you!");
		return;
	}

	if (!rn2(10)) {

		register int randomchoice = rnd(4);

		switch (randomchoice) {
			case 1:
				otmp2 = uleft;
				if (!otmp2) {
					pline("Your left %s twitches.", body_part(FINGER));
					return;
				}
				break;
			case 2:
				otmp2 = uright;
				if (!otmp2) {
					pline("Your right %s twitches.", body_part(FINGER));
					return;
				}
				break;
			case 3:
				otmp2 = uamul;
				if (!otmp2) {
					pline("Your %s twitches.", body_part(NECK));
					return;
				}
				break;
			case 4:
				otmp2 = ublindf;
				if (!otmp2) {
					pline("Your fore%s twitches.", body_part(HEAD));
					return;
				}
				break;
		}

		if (stack_too_big(otmp2)) return; /* nothing happens, no message */
		else if (otmp2 && otmp2->blessed && rn2(5)) pline("Your %s%s shakes violently!", randomchoice == 1 ? "left " : randomchoice == 2 ? "right " : randomchoice == 4 ? "fore" : "", randomchoice == 1 ? body_part(FINGER) : randomchoice == 2 ? body_part(FINGER) : randomchoice == 3 ? body_part(NECK) : body_part(HEAD) );
		else if (otmp2 && (otmp2->spe > 1) && (rn2(otmp2->spe)) ) pline("Your %s%s shakes violently!", randomchoice == 1 ? "left " : randomchoice == 2 ? "right " : randomchoice == 4 ? "fore" : "", randomchoice == 1 ? body_part(FINGER) : randomchoice == 2 ? body_part(FINGER) : randomchoice == 3 ? body_part(NECK) : body_part(HEAD) );
		else if (otmp2 && otmp2->oartifact && rn2(20)) pline("Your %s%s shakes violently!", randomchoice == 1 ? "left " : randomchoice == 2 ? "right " : randomchoice == 4 ? "fore" : "", randomchoice == 1 ? body_part(FINGER) : randomchoice == 2 ? body_part(FINGER) : randomchoice == 3 ? body_part(NECK) : body_part(HEAD) );
		else if (otmp2 && otmp2->greased) {
			pline("Your %s%s shakes violently!", randomchoice == 1 ? "left " : randomchoice == 2 ? "right " : randomchoice == 4 ? "fore" : "", randomchoice == 1 ? body_part(FINGER) : randomchoice == 2 ? body_part(FINGER) : randomchoice == 3 ? body_part(NECK) : body_part(HEAD) );
			if (!rn2(2) || (isfriday && !rn2(2))) {
				pline_The("grease wears off.");
				otmp2->greased -= 1;
				update_inventory();
			}
		} else {
			useupall(otmp2);
			pline("Your %s%s is destroyed.", randomchoice == 1 ? "left " : randomchoice == 2 ? "right " : "", randomchoice == 1 ? "ring" : randomchoice == 2 ? "ring" : randomchoice == 3 ? "amulet" : "blindfold or similar tool" );
		}


	} else if (!rn2(4)) {

		otmp2 = uwep;
		if (!otmp2) {
			pline("Your %s twitch.", makeplural(body_part(HAND)));
			return;
		} else if (stack_too_big(otmp2)) return; /* nothing happens, no message */
		else if (otmp2 && otmp2->blessed && rn2(5)) pline("Your weapon shakes violently!");
		else if (otmp2 && (otmp2->spe > 1) && (rn2(otmp2->spe)) ) pline("Your weapon shakes violently!");
		else if (otmp2 && otmp2->oartifact && rn2(20)) pline("Your weapon shakes violently!");
		else if (otmp2 && otmp2->greased) {
			pline("Your weapon shakes violently!");
			if (!rn2(2) || (isfriday && !rn2(2))) {
				pline_The("grease wears off.");
				otmp2->greased -= 1;
				update_inventory();
			}
		} else {
			useupall(otmp2);
			pline("Your weapon evaporates!");
		}

	} else {

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

	stop_occupation();

}

/* allow the player to uncurse a choice item --Amy */
void
uncurseoneitem()
{
	char allowall[2];
	allowall[0] = ALL_CLASSES; allowall[1] = '\0';
	struct obj *obj; /* item to uncurse */
uncurseagain:
	if ( !(obj = getobj(allowall, "uncurse"))) {
		if (yn("Really exit with no object selected?") == 'y')
			pline("You just wasted the opportunity to uncurse an item.");
		else goto uncurseagain;
		pline("A feeling of loss comes over you.");
		return;
	}
	if (!stack_too_big(obj)) {
		uncurse(obj, TRUE);
		Your("%s glows brightly.", xname(obj)); /* doesn't reveal BUC; if it wasn't cursed, nothing happens */
	}
	else pline("The stack was too big and therefore the uncursing attempt failed.");

}

/* does the player character have anorexia? --Amy */
boolean
have_anorexia()
{
	if (FemtrapActiveIna) return TRUE;
	if (RngeAnorexia) return TRUE;
	if (Role_if(PM_TOPMODEL)) return TRUE;
	if (uarmc && uarmc->oartifact == ART_INA_S_SORROW) return TRUE;
	if (Role_if(PM_GOFF)) return TRUE;
	if (uarmc && itemhasappearance(uarmc, APP_ANOREXIA_CLOAK)) return TRUE;

	return FALSE;
}

/* does your equipment made of sand protect you from sand terrain? --Amy */
boolean
sandprotection()
{
	if (uarm && objects[(uarm)->otyp].oc_material == MT_SAND && !rn2(10)) {
		return TRUE;
	}
	if (uarmf && objects[(uarmf)->otyp].oc_material == MT_SAND && !rn2(10)) {
		return TRUE;
	}
	if (uarmg && objects[(uarmg)->otyp].oc_material == MT_SAND && !rn2(10)) {
		return TRUE;
	}
	if (uarmh && objects[(uarmh)->otyp].oc_material == MT_SAND && !rn2(10)) {
		return TRUE;
	}
	if (uarms && objects[(uarms)->otyp].oc_material == MT_SAND && !rn2(10)) {
		return TRUE;
	}
	if (uarmc && objects[(uarmc)->otyp].oc_material == MT_SAND && !rn2(10)) {
		return TRUE;
	}
	if (uarmu && objects[(uarmu)->otyp].oc_material == MT_SAND && !rn2(10)) {
		return TRUE;
	}
	if (uamul && objects[(uamul)->otyp].oc_material == MT_SAND && !rn2(10)) {
		return TRUE;
	}
	if (uimplant && objects[(uimplant)->otyp].oc_material == MT_SAND && !rn2(10)) {
		return TRUE;
	}
	if (uleft && objects[(uleft)->otyp].oc_material == MT_SAND && !rn2(10)) {
		return TRUE;
	}
	if (uright && objects[(uright)->otyp].oc_material == MT_SAND && !rn2(10)) {
		return TRUE;
	}
	if (ublindf && objects[(ublindf)->otyp].oc_material == MT_SAND && !rn2(10)) {
		return TRUE;
	}
	if (uwep && objects[(uwep)->otyp].oc_material == MT_SAND && !rn2(10)) {
		return TRUE;
	}
	if (u.twoweap && uswapwep && objects[(uswapwep)->otyp].oc_material == MT_SAND && !rn2(10)) {
		return TRUE;
	}

	return FALSE;
}

/* does your equipment made of chrome protect you from poison? --Amy */
boolean
chromeprotection()
{
	if (uarmc && itemhasappearance(uarmc, APP_PURE_CLOAK) && !rn2(3)) {
		pline("Your pure cloak prevents the poison effect!");
		return TRUE;
	}

	if (uarm && objects[(uarm)->otyp].oc_material == MT_CHROME && !rn2(10)) {
		pline("Your chrome equipment prevents the poison effect!");
		return TRUE;
	}
	if (uarmf && objects[(uarmf)->otyp].oc_material == MT_CHROME && !rn2(10)) {
		pline("Your chrome equipment prevents the poison effect!");
		return TRUE;
	}
	if (uarmg && objects[(uarmg)->otyp].oc_material == MT_CHROME && !rn2(10)) {
		pline("Your chrome equipment prevents the poison effect!");
		return TRUE;
	}
	if (uarmh && objects[(uarmh)->otyp].oc_material == MT_CHROME && !rn2(10)) {
		pline("Your chrome equipment prevents the poison effect!");
		return TRUE;
	}
	if (uarms && objects[(uarms)->otyp].oc_material == MT_CHROME && !rn2(10)) {
		pline("Your chrome equipment prevents the poison effect!");
		return TRUE;
	}
	if (uarmc && objects[(uarmc)->otyp].oc_material == MT_CHROME && !rn2(10)) {
		pline("Your chrome equipment prevents the poison effect!");
		return TRUE;
	}
	if (uarmu && objects[(uarmu)->otyp].oc_material == MT_CHROME && !rn2(10)) {
		pline("Your chrome equipment prevents the poison effect!");
		return TRUE;
	}
	if (uamul && objects[(uamul)->otyp].oc_material == MT_CHROME && !rn2(10)) {
		pline("Your chrome equipment prevents the poison effect!");
		return TRUE;
	}
	if (uimplant && objects[(uimplant)->otyp].oc_material == MT_CHROME && !rn2(10)) {
		pline("Your chrome equipment prevents the poison effect!");
		return TRUE;
	}
	if (uleft && objects[(uleft)->otyp].oc_material == MT_CHROME && !rn2(10)) {
		pline("Your chrome equipment prevents the poison effect!");
		return TRUE;
	}
	if (uright && objects[(uright)->otyp].oc_material == MT_CHROME && !rn2(10)) {
		pline("Your chrome equipment prevents the poison effect!");
		return TRUE;
	}
	if (ublindf && objects[(ublindf)->otyp].oc_material == MT_CHROME && !rn2(10)) {
		pline("Your chrome equipment prevents the poison effect!");
		return TRUE;
	}
	if (uwep && objects[(uwep)->otyp].oc_material == MT_CHROME && !rn2(10)) {
		pline("Your chrome equipment prevents the poison effect!");
		return TRUE;
	}
	if (u.twoweap && uswapwep && objects[(uswapwep)->otyp].oc_material == MT_CHROME && !rn2(10)) {
		pline("Your chrome equipment prevents the poison effect!");
		return TRUE;
	}

	return FALSE;
}

/* does your equipment made of shadowstuff protect you from a darkness attack? --Amy */
boolean
shadowprotection()
{
	if (uarm && objects[(uarm)->otyp].oc_material == MT_SHADOWSTUFF && !rn2(10)) {
		pline("Your shadowstuff equipment prevents the darkness effect!");
		return TRUE;
	}
	if (uarmf && objects[(uarmf)->otyp].oc_material == MT_SHADOWSTUFF && !rn2(10)) {
		pline("Your shadowstuff equipment prevents the darkness effect!");
		return TRUE;
	}
	if (uarmg && objects[(uarmg)->otyp].oc_material == MT_SHADOWSTUFF && !rn2(10)) {
		pline("Your shadowstuff equipment prevents the darkness effect!");
		return TRUE;
	}
	if (uarmh && objects[(uarmh)->otyp].oc_material == MT_SHADOWSTUFF && !rn2(10)) {
		pline("Your shadowstuff equipment prevents the darkness effect!");
		return TRUE;
	}
	if (uarms && objects[(uarms)->otyp].oc_material == MT_SHADOWSTUFF && !rn2(10)) {
		pline("Your shadowstuff equipment prevents the darkness effect!");
		return TRUE;
	}
	if (uarmc && objects[(uarmc)->otyp].oc_material == MT_SHADOWSTUFF && !rn2(10)) {
		pline("Your shadowstuff equipment prevents the darkness effect!");
		return TRUE;
	}
	if (uarmu && objects[(uarmu)->otyp].oc_material == MT_SHADOWSTUFF && !rn2(10)) {
		pline("Your shadowstuff equipment prevents the darkness effect!");
		return TRUE;
	}
	if (uamul && objects[(uamul)->otyp].oc_material == MT_SHADOWSTUFF && !rn2(10)) {
		pline("Your shadowstuff equipment prevents the darkness effect!");
		return TRUE;
	}
	if (uimplant && objects[(uimplant)->otyp].oc_material == MT_SHADOWSTUFF && !rn2(10)) {
		pline("Your shadowstuff equipment prevents the darkness effect!");
		return TRUE;
	}
	if (uleft && objects[(uleft)->otyp].oc_material == MT_SHADOWSTUFF && !rn2(10)) {
		pline("Your shadowstuff equipment prevents the darkness effect!");
		return TRUE;
	}
	if (uright && objects[(uright)->otyp].oc_material == MT_SHADOWSTUFF && !rn2(10)) {
		pline("Your shadowstuff equipment prevents the darkness effect!");
		return TRUE;
	}
	if (ublindf && objects[(ublindf)->otyp].oc_material == MT_SHADOWSTUFF && !rn2(10)) {
		pline("Your shadowstuff equipment prevents the darkness effect!");
		return TRUE;
	}
	if (uwep && objects[(uwep)->otyp].oc_material == MT_SHADOWSTUFF && !rn2(10)) {
		pline("Your shadowstuff equipment prevents the darkness effect!");
		return TRUE;
	}
	if (u.twoweap && uswapwep && objects[(uswapwep)->otyp].oc_material == MT_SHADOWSTUFF && !rn2(10)) {
		pline("Your shadowstuff equipment prevents the darkness effect!");
		return TRUE;
	}

	return FALSE;
}

/* does your equipment made of obsidian protect you from a cursed unihorn effect? --Amy */
boolean
obsidianprotection()
{
	if (uarm && objects[(uarm)->otyp].oc_material == MT_OBSIDIAN && !rn2(10)) {
		pline("Thanks to your obsidian equipment, the detrimental effect was avoided.");
		return TRUE;
	}
	if (uarmf && objects[(uarmf)->otyp].oc_material == MT_OBSIDIAN && !rn2(10)) {
		pline("Thanks to your obsidian equipment, the detrimental effect was avoided.");
		return TRUE;
	}
	if (uarmg && objects[(uarmg)->otyp].oc_material == MT_OBSIDIAN && !rn2(10)) {
		pline("Thanks to your obsidian equipment, the detrimental effect was avoided.");
		return TRUE;
	}
	if (uarmh && objects[(uarmh)->otyp].oc_material == MT_OBSIDIAN && !rn2(10)) {
		pline("Thanks to your obsidian equipment, the detrimental effect was avoided.");
		return TRUE;
	}
	if (uarms && objects[(uarms)->otyp].oc_material == MT_OBSIDIAN && !rn2(10)) {
		pline("Thanks to your obsidian equipment, the detrimental effect was avoided.");
		return TRUE;
	}
	if (uarmc && objects[(uarmc)->otyp].oc_material == MT_OBSIDIAN && !rn2(10)) {
		pline("Thanks to your obsidian equipment, the detrimental effect was avoided.");
		return TRUE;
	}
	if (uarmu && objects[(uarmu)->otyp].oc_material == MT_OBSIDIAN && !rn2(10)) {
		pline("Thanks to your obsidian equipment, the detrimental effect was avoided.");
		return TRUE;
	}
	if (uamul && objects[(uamul)->otyp].oc_material == MT_OBSIDIAN && !rn2(10)) {
		pline("Thanks to your obsidian equipment, the detrimental effect was avoided.");
		return TRUE;
	}
	if (uimplant && objects[(uimplant)->otyp].oc_material == MT_OBSIDIAN && !rn2(10)) {
		pline("Thanks to your obsidian equipment, the detrimental effect was avoided.");
		return TRUE;
	}
	if (uleft && objects[(uleft)->otyp].oc_material == MT_OBSIDIAN && !rn2(10)) {
		pline("Thanks to your obsidian equipment, the detrimental effect was avoided.");
		return TRUE;
	}
	if (uright && objects[(uright)->otyp].oc_material == MT_OBSIDIAN && !rn2(10)) {
		pline("Thanks to your obsidian equipment, the detrimental effect was avoided.");
		return TRUE;
	}
	if (ublindf && objects[(ublindf)->otyp].oc_material == MT_OBSIDIAN && !rn2(10)) {
		pline("Thanks to your obsidian equipment, the detrimental effect was avoided.");
		return TRUE;
	}
	if (uwep && objects[(uwep)->otyp].oc_material == MT_OBSIDIAN && !rn2(10)) {
		pline("Thanks to your obsidian equipment, the detrimental effect was avoided.");
		return TRUE;
	}
	if (u.twoweap && uswapwep && objects[(uswapwep)->otyp].oc_material == MT_OBSIDIAN && !rn2(10)) {
		pline("Thanks to your obsidian equipment, the detrimental effect was avoided.");
		return TRUE;
	}

	return FALSE;
}

/* will the player's weapon dull when hitting in melee? --Amy */
boolean
weaponwilldull(dwpn)
register struct obj *dwpn;
{
	int dullchance = 1000;
	int minenchant = 0; /* spe must be greater than this */

	if (!dwpn) return FALSE;

	if (objects[dwpn->otyp].oc_material == MT_PLATINUM) minenchant = 1;

	if (dwpn->otyp >= WEDGED_LITTLE_GIRL_SANDAL && dwpn->otyp <= PROSTITUTE_SHOE && !Role_if(PM_TRANSVESTITE) && !Role_if(PM_TRANSSYLVANIAN) && !Role_if(PM_SHOE_FETISHIST)) minenchant--;

	if (dwpn->oartifact == ART_CLEAN_MAULER) minenchant = -10;
	if (objects[dwpn->otyp].oc_material == MT_CERAMIC) minenchant = -10;

	if (dwpn->spe <= minenchant) return FALSE;

	if ((dwpn->spe <= rnd(8)) && rn2(8) ) return FALSE; /* less likely with lower enchantment --Amy */

	if (dwpn->oartifact == ART_CLEAN_MAULER) dullchance = 100;
	if (objects[dwpn->otyp].oc_material == MT_CERAMIC) dullchance = 100;
	if (objects[dwpn->otyp].oc_material == MT_LIQUID) dullchance = 250;

	if (dwpn->otyp >= WEDGED_LITTLE_GIRL_SANDAL && dwpn->otyp <= PROSTITUTE_SHOE && !Role_if(PM_TRANSVESTITE) && !Role_if(PM_TRANSSYLVANIAN) && !Role_if(PM_SHOE_FETISHIST)) dullchance /= 2;

	if (dullchance < 1) dullchance = 1; /* fail safe */

	if (objects[dwpn->otyp].oc_material == MT_GOLD && rn2(3)) return FALSE;

	if (objects[dwpn->otyp].oc_material == MT_ARCANIUM) return FALSE;
	if (objects[dwpn->otyp].oc_material == MT_SECREE) return FALSE;

	if (dwpn->oartifact && rn2(4)) return FALSE;

	if (dwpn->blessed && !rnl(6)) return FALSE;

	if (!rn2(dullchance)) return TRUE;

	return FALSE; /* fail safe */

}

boolean
armorwilldull(dwpn)
register struct obj *dwpn;
{
	int dullchance = 1000;
	int minenchant = 0; /* spe must be greater than this */

	if (!dwpn) return FALSE;

	if (objects[dwpn->otyp].oc_material == MT_PLATINUM) minenchant = 1;

	if (objects[dwpn->otyp].oc_material == MT_CERAMIC) minenchant = -10;

	if (Race_if(PM_RUSMOT)) minenchant -= 5;

	if (dwpn->spe <= minenchant) return FALSE;

	if ((dwpn->spe <= (Race_if(PM_RUSMOT) ? (rnd(8) - 5) : rnd(8)) ) && rn2(8) ) return FALSE; /* less likely with lower enchantment --Amy */

	if (objects[dwpn->otyp].oc_material == MT_CERAMIC) dullchance = 100;
	if (objects[dwpn->otyp].oc_material == MT_LIQUID) dullchance = 125;

	if (Race_if(PM_RUSMOT)) dullchance /= 5;

	if (dullchance < 1) dullchance = 1; /* fail safe */

	if (objects[dwpn->otyp].oc_material == MT_GOLD && rn2(3)) return FALSE;

	if (objects[dwpn->otyp].oc_material == MT_ARCANIUM) return FALSE;
	if (objects[dwpn->otyp].oc_material == MT_SECREE) return FALSE;

	if (dwpn->oartifact && rn2(4)) return FALSE;

	if (dwpn->blessed && !rnl(6)) return FALSE;

	if (!rn2(dullchance)) return TRUE;

	return FALSE; /* fail safe */

}

/* by jonadab: psychic waves can h@xx0r the player's implant --Amy */
void
maybehackimplant()
{
	int hackchance = 10;
	if (powerfulimplants()) hackchance *= 10;
	if (!uimplant) return;

	if (!(PlayerCannotUseSkills) && (powerfulimplants() || rn2(2)) ) {

		switch (P_SKILL(P_IMPLANTS)) {
			default: break;
			case P_BASIC: hackchance *= 2; break;
			case P_SKILLED: hackchance *= 3; break;
			case P_EXPERT: hackchance *= 5; break;
			case P_MASTER: hackchance *= 7; break;
			case P_GRAND_MASTER: hackchance *= 10; break;
			case P_SUPREME_MASTER: hackchance *= 15; break;
		}
	}

	if (!rn2(hackchance)) {
		pline("Oh no! Your implant was h@xx0red!");
		badeffect(); /* could theoretically destroy or unequip uimplant */
		if (uimplant && uimplant->blessed) unbless(uimplant);
		else if (uimplant) curse(uimplant);
		if (Blind_telepat || Unblind_telepat) deacrandomintrinsic(rnz(5000));
	}

}

/* stat debuff attack: reduces a random stat, temporarily */
void
statdebuff()
{
	if (Role_if(PM_ASTRONAUT) && rn2(2)) {
		pline("Your steeled body prevents the stat loss!");
		return;
	}

	switch (rnd(6)) {
		case 1:
			u.tsloss_str++;
			You_feel("temporarily weaker!");
			break;
		case 2:
			u.tsloss_dex++;
			You_feel("temporarily clumsier!");
			break;
		case 3:
			u.tsloss_con++;
			You_feel(rn2(2) ? "temporarily fralier!" : "temporarily frailer!");
			break;
		case 4:
			u.tsloss_wis++;
			You_feel("temporarily more naive!");
			break;
		case 5:
			u.tsloss_int++;
			You_feel("temporarily stupider!");
			break;
		case 6:
			u.tsloss_cha++;
			You_feel("temporarily more repulsive!");
			break;
		flags.botl = 1;
	}
}

/* the stat-swapping effect of AD_NEXU and other nexus effects --Amy */
void
nexus_swap()
{
	int strweirdness = 0;
	int strtempvar;
	int strfractionvar;
	int firststat;
	int secondstat;
newswap:
	firststat = rn2(A_MAX);
	secondstat = rn2(A_MAX);
	if (firststat == secondstat) goto newswap; /* make sure we have two different stats for swapping, please! */
	int firstswapstat = ABASE(firststat);
	int secondswapstat = ABASE(secondstat);

	if (firststat == A_STR) {
		strtempvar = ABASE(firststat);
		while (strtempvar >= STR19(19)) strtempvar--;
		while (strtempvar > 18 && strtempvar > secondstat) {
			if (strtempvar < STR18(10)) strfractionvar = (strtempvar - 18);
			strtempvar -= 10;
			if (!strfractionvar || (strfractionvar > rn2(10)) ) strweirdness++;
		}
	} else if (secondstat == A_STR) {
		strtempvar = ABASE(secondstat);
		while (strtempvar >= STR19(19)) strtempvar--;
		while (strtempvar > 18 && strtempvar > firststat) {
			if (strtempvar < STR18(10)) strfractionvar = (strtempvar - 18);
			strtempvar -= 10;
			if (!strfractionvar || (strfractionvar > rn2(10)) ) strweirdness++;
		}
	}

	int difference = (firstswapstat - secondswapstat);
	int strdifference = difference;

	if (secondstat == A_STR) {
		while (strdifference < -9 && strweirdness > 0) {
			strdifference += 9;
			strweirdness--;
		}
		ABASE(firststat) -= strdifference;
	} else ABASE(firststat) -= difference;

	if (firststat == A_STR) {
		while (strdifference > 9 && strweirdness > 0) {
			strdifference -= 9;
			strweirdness--;
		}
		ABASE(secondstat) += strdifference;
	} else ABASE(secondstat) += difference;
	
	AMAX(secondstat) = ABASE(secondstat);
	AMAX(firststat) = ABASE(firststat);
	pline("Your stats got scrambled!");


}

/* AD_DROP: forces player to drop random items, but no equipped ones --Amy */
void
dropitemattack()
{
	register struct obj *otmpi, *otmpii;
	register int numdroppeditems = 0;

	for (otmpi = invent; otmpi; otmpi = otmpii) {

		otmpii = otmpi->nobj;

		if (!rn2(50) && !stack_too_big(otmpi) ) {

			if (otmpi->owornmask & W_ARMOR) continue;
			else if (otmpi->owornmask & W_AMUL) {
				if (otmpi->cursed) continue;
				else Amulet_off();
			} else if (otmpi->owornmask & W_IMPLANT) {
				continue;
			} else if (otmpi->owornmask & W_RING) {
				if (otmpi->cursed) continue;
				Ring_gone(otmpi);
			} else if (otmpi->owornmask & W_TOOL) continue;
			else if (otmpi->owornmask & (W_WEP|W_SWAPWEP|W_QUIVER)) {
			    if (otmpi == uwep && otmpi->cursed) continue;

			    if (otmpi == uwep)
				uwepgone();
			    if (otmpi == uswapwep)
				uswapwepgone();
			    if (otmpi == uquiver)
				uqwepgone();
			}

			if ( (otmpi->otyp == LOADSTONE || otmpi->otyp == LOADBOULDER || otmpi->otyp == STARLIGHTSTONE || otmpi->otyp == LUCKSTONE || otmpi->otyp == HEALTHSTONE || otmpi->otyp == MANASTONE || otmpi->otyp == SLEEPSTONE || otmpi->otyp == STONE_OF_MAGIC_RESISTANCE || is_nastygraystone(otmpi) || is_feminismstone(otmpi)) && otmpi->cursed ) continue;

			if (otmpi->owornmask & (W_BALL|W_CHAIN)) continue;
			else if (otmpi->owornmask) {
			/* catchall */
			    setnotworn(otmpi);
			}

			dropx(otmpi);
			numdroppeditems++;
		}
	    }
	    pline("Uh-oh, you suddenly trip over your own %s%s", makeplural(body_part(FOOT)), numdroppeditems ? " and some of your possessions fall out of your knapsack!" : "! But you barely manage to avoid dropping your possessions.");
	    if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

	    if (numdroppeditems) {
			if (Levitation) scatter(u.ux,u.uy,10,VIS_EFFECTS|MAY_DESTROY,0);
			else scatter(u.ux,u.uy,10,VIS_EFFECTS,0); /* don't destroy fragile stuff (unless levitating) */
	    }

}

/* nivellation: rather than doing what some other variants are doing (imposing a hard cap on max HP and Pw), I decided
 * that it's much more interesting to instead have an attack/trap/badeffect that can reduce the max depending on how high
 * it already is; this effect will do nothing if the player's max is below a certain threshold */
void
nivellate()
{
	boolean type = rn2(2); /* 0 = HP, 1 = Pw */
	int nivellevel = u.ulevel;
	int lowerceiling = 1;
	int upperceiling = 1;
	int reduceamount = 1;

	if (uarmh && uarmh->oartifact == ART_FFP___MASK && rn2(5)) {
		pline("That butt-ugly mask prevented the corona viri from infecting you.");
		return;
	}

	if (uarm && objects[(uarm)->otyp].oc_material == MT_ANTIDOTIUM && !rn2(10)) {
		pline("Your antidotium armor prevents you from contracting corona!");
		return;
	}
	if (uarmf && objects[(uarmf)->otyp].oc_material == MT_ANTIDOTIUM && !rn2(10)) {
		pline("Your antidotium footwear prevents you from contracting corona!");
		return;
	}
	if (uarmg && objects[(uarmg)->otyp].oc_material == MT_ANTIDOTIUM && !rn2(10)) {
		pline("Your antidotium gloves prevents you from contracting corona!");
		return;
	}
	if (uarmh && objects[(uarmh)->otyp].oc_material == MT_ANTIDOTIUM && !rn2(10)) {
		pline("Your antidotium helmet prevents you from contracting corona!");
		return;
	}
	if (uarms && objects[(uarms)->otyp].oc_material == MT_ANTIDOTIUM && !rn2(10)) {
		pline("Your antidotium shield prevents you from contracting corona!");
		return;
	}
	if (uarmc && objects[(uarmc)->otyp].oc_material == MT_ANTIDOTIUM && !rn2(10)) {
		pline("Your antidotium cloak prevents you from contracting corona!");
		return;
	}
	if (uarmu && objects[(uarmu)->otyp].oc_material == MT_ANTIDOTIUM && !rn2(10)) {
		pline("Your antidotium shirt prevents you from contracting corona!");
		return;
	}
	if (uamul && objects[(uamul)->otyp].oc_material == MT_ANTIDOTIUM && !rn2(10)) {
		pline("Your antidotium amulet prevents you from contracting corona!");
		return;
	}
	if (uimplant && objects[(uimplant)->otyp].oc_material == MT_ANTIDOTIUM && !rn2(10)) {
		pline("Your antidotium implant prevents you from contracting corona!");
		return;
	}
	if (uleft && objects[(uleft)->otyp].oc_material == MT_ANTIDOTIUM && !rn2(10)) {
		pline("Your antidotium left ring prevents you from contracting corona!");
		return;
	}
	if (uright && objects[(uright)->otyp].oc_material == MT_ANTIDOTIUM && !rn2(10)) {
		pline("Your antidotium right ring prevents you from contracting corona!");
		return;
	}
	if (ublindf && objects[(ublindf)->otyp].oc_material == MT_ANTIDOTIUM && !rn2(10)) {
		pline("Your antidotium blindfold or other tool prevents you from contracting corona!");
		return;
	}
	if (uwep && objects[(uwep)->otyp].oc_material == MT_ANTIDOTIUM && !rn2(10)) {
		pline("Your antidotium weapon prevents you from contracting corona!");
		return;
	}
	if (u.twoweap && uswapwep && objects[(uswapwep)->otyp].oc_material == MT_ANTIDOTIUM && !rn2(10)) {
		pline("Your antidotium secondary weapon prevents you from contracting corona!");
		return;
	}

	/* at very low XLs, we'll be nice... */
	if (u.ulevel < 3) nivellevel = 3;
	if (u.ulevel == 3) nivellevel = 4;

	if (type == 0) { /* HP */

		if (Race_if(PM_SUSTAINER)) return;

		if (rnd(ACURR(A_CON)) > 14) {
			Your("constitution prevents the health drain.");
			return;
		}

		lowerceiling = (nivellevel * 10);
		upperceiling = (ACURR(A_CON) > 15) ? (nivellevel * ACURR(A_CON)) : (nivellevel * 15);

		if (Role_if(PM_ASTRONAUT)) {
			lowerceiling *= 6;
			lowerceiling /= 5;
			upperceiling *= 6;
			upperceiling /= 5;
		}
		if (Role_if(PM_BARBARIAN)) {
			lowerceiling *= 5;
			lowerceiling /= 4;
			upperceiling *= 5;
			upperceiling /= 4;
		}
		if (Role_if(PM_AUGURER)) {
			lowerceiling *= 4;
			lowerceiling /= 5;
			upperceiling *= 4;
			upperceiling /= 5;
		}
		if (Role_if(PM_BLEEDER)) {
			lowerceiling *= 2;
			upperceiling *= 2;
		}
		if (Role_if(PM_DRUID)) {
			lowerceiling /= 2;
			upperceiling /= 2;
		}
		if (Role_if(PM_ORDINATOR)) {
			lowerceiling /= 2;
			upperceiling /= 2;
		}
		if (Role_if(PM_UNBELIEVER)) {
			lowerceiling *= 2;
			upperceiling *= 2;
		}
		if (Role_if(PM_DANCER)) {
			lowerceiling /= 2;
			upperceiling /= 2;
		}
		if (Role_if(PM_WARRIOR)) {
			lowerceiling *= 3;
			lowerceiling /= 2;
			upperceiling *= 3;
			upperceiling /= 2;
		}
		if (Race_if(PM_YEEK)) {
			lowerceiling /= 2;
			upperceiling /= 2;
		}
		if (Race_if(PM_DUFFLEPUD)) {
			lowerceiling /= 2;
			upperceiling /= 2;
		}
		if (Race_if(PM_PLAYER_DOLGSMAN)) {
			lowerceiling *= 3;
			lowerceiling /= 4;
			upperceiling *= 3;
			upperceiling /= 4;
		}
		if (Race_if(PM_SPRIGGAN)) {
			lowerceiling *= 3;
			lowerceiling /= 4;
			upperceiling *= 3;
			upperceiling /= 4;
		}
		if (Race_if(PM_PLAYER_FAIRY)) {
			lowerceiling /= 2;
			upperceiling /= 2;
		}
		if (Race_if(PM_BACTERIA)) {
			lowerceiling *= 3;
			lowerceiling /= 2;
			upperceiling *= 3;
			upperceiling /= 2;
		}
		if (Race_if(PM_DEVELOPER)) {
			lowerceiling *= 3;
			lowerceiling /= 2;
			upperceiling *= 3;
			upperceiling /= 2;
		}
		if (Race_if(PM_GIGANT)) {
			lowerceiling *= 5;
			lowerceiling /= 4;
			upperceiling *= 5;
			upperceiling /= 4;
		}
		if (Race_if(PM_INKA)) {
			lowerceiling *= 6;
			lowerceiling /= 5;
			upperceiling *= 6;
			upperceiling /= 5;
		}
		if (Race_if(PM_ITAQUE)) {
			lowerceiling *= 7;
			lowerceiling /= 5;
			upperceiling *= 7;
			upperceiling /= 5;
		}
		if (Race_if(PM_PLAYER_CERBERUS)) {
			lowerceiling *= 5;
			lowerceiling /= 4;
			upperceiling *= 5;
			upperceiling /= 4;
		}
		if (Race_if(PM_PLAYER_JABBERWOCK)) {
			lowerceiling *= 4;
			lowerceiling /= 3;
			upperceiling *= 4;
			upperceiling /= 3;
		}
		if (Race_if(PM_PLAYER_FAIRY)) {
			lowerceiling *= 2;
			lowerceiling /= 3;
			upperceiling *= 2;
			upperceiling /= 3;
		}
		if (Race_if(PM_WEAPONIZED_DINOSAUR)) {
			lowerceiling *= 4;
			lowerceiling /= 3;
			upperceiling *= 4;
			upperceiling /= 3;
		}
		if (Race_if(PM_SHELL)) {
			lowerceiling *= 4;
			lowerceiling /= 3;
			upperceiling *= 4;
			upperceiling /= 3;
		}
		if (Race_if(PM_CARTHAGE)) {
			lowerceiling *= 9;
			lowerceiling /= 10;
			upperceiling *= 9;
			upperceiling /= 10;
		}
		if (Race_if(PM_VIKING)) {
			lowerceiling *= 9;
			lowerceiling /= 10;
			upperceiling *= 9;
			upperceiling /= 10;
		}

		if (lowerceiling < 10) lowerceiling = 10; /* fail safe */
		if (upperceiling < 15) upperceiling = 15; /* fail safe */

		if (u.uhpmax > upperceiling) {
			reduceamount = rnd(u.uhpmax / 10);
			u.uhpmax -= reduceamount;
			Your("health was drained by %d!", reduceamount);
		} else if (u.uhpmax > lowerceiling && !rn2(3)) {
			u.uhpmax--;
			Your("health was drained by 1!");
		}
		if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;

		if (Upolyd) {
			if (u.mhmax > upperceiling) {
				reduceamount = rnd(u.mhmax / 10);
				u.mhmax -= reduceamount;
				Your("polymorphed health was drained by %d!", reduceamount);
			} else if (u.mhmax > lowerceiling && !rn2(3)) {
				u.mhmax--;
				Your("polymorphed health was drained by 1!");
			}
			if (u.mh > u.mhmax) u.mh = u.mhmax;

		}

	} else { /* Pw */

		if (Race_if(PM_SUSTAINER)) return;

		if (rnd(ACURR(A_WIS)) > 14) {
			Your("wisdom prevents the mana drain.");
			return;
		}

		lowerceiling = (nivellevel * 10);
		upperceiling = (ACURR(A_WIS) > 15) ? (nivellevel * ACURR(A_WIS)) : (nivellevel * 15);

		if (Role_if(PM_ALTMER)) {
			lowerceiling *= 3;
			lowerceiling /= 2;
			upperceiling *= 3;
			upperceiling /= 2;
		}
		if (Role_if(PM_MASTERMIND)) {
			lowerceiling *= 4;
			lowerceiling /= 3;
			upperceiling *= 4;
			upperceiling /= 3;
		}
		if (Role_if(PM_PSYKER)) {
			lowerceiling *= 5;
			lowerceiling /= 4;
			upperceiling *= 5;
			upperceiling /= 4;
		}
		if (Role_if(PM_WIZARD)) {
			lowerceiling *= 5;
			lowerceiling /= 4;
			upperceiling *= 5;
			upperceiling /= 4;
		}
		if (Role_if(PM_UNBELIEVER)) {
			lowerceiling /= 5;
			upperceiling /= 5;
		}
		if (Role_if(PM_NOOB_MODE_BARB)) {
			lowerceiling /= 2;
			upperceiling /= 2;
		}
		if (Race_if(PM_YEEK)) {
			lowerceiling /= 2;
			upperceiling /= 2;
		}
		if (Race_if(PM_DUFFLEPUD)) {
			lowerceiling /= 2;
			upperceiling /= 2;
		}
		if (Race_if(PM_BACTERIA)) {
			lowerceiling *= 3;
			lowerceiling /= 2;
			upperceiling *= 3;
			upperceiling /= 2;
		}
		if (Race_if(PM_BRETON)) {
			lowerceiling *= 4;
			lowerceiling /= 3;
			upperceiling *= 4;
			upperceiling /= 3;
		}
		if (Race_if(PM_DEVELOPER)) {
			lowerceiling *= 3;
			lowerceiling /= 2;
			upperceiling *= 3;
			upperceiling /= 2;
		}
		if (Race_if(PM_TROLLOR)) {
			lowerceiling *= 2;
			lowerceiling /= 3;
			upperceiling *= 2;
			upperceiling /= 3;
		}
		if (Race_if(PM_REDGUARD)) {
			lowerceiling /= 2;
			upperceiling /= 2;
		}
		if (Race_if(PM_YOKUDA)) {
			lowerceiling *= 2;
			lowerceiling /= 3;
			upperceiling *= 2;
			upperceiling /= 3;
		}
		if (Race_if(PM_RODNEYAN)) {
			lowerceiling *= 2;
			upperceiling *= 2;
		}
		if (Race_if(PM_CARTHAGE)) {
			lowerceiling *= 9;
			lowerceiling /= 10;
			upperceiling *= 9;
			upperceiling /= 10;
		}
		if (Race_if(PM_LICH_WARRIOR)) {
			lowerceiling *= 5;
			lowerceiling /= 4;
			upperceiling *= 5;
			upperceiling /= 4;
		}
		if (Race_if(PM_WISP)) {
			lowerceiling *= 4;
			lowerceiling /= 3;
			upperceiling *= 4;
			upperceiling /= 3;
		}

		if (lowerceiling < 10) lowerceiling = 10; /* fail safe */
		if (upperceiling < 15) upperceiling = 15; /* fail safe */

		if (u.uenmax > upperceiling) {
			reduceamount = rnd(u.uenmax / 10);
			u.uenmax -= reduceamount;
			Your("mana was drained by %d!", reduceamount);
		} else if (u.uenmax > lowerceiling && !rn2(3)) {
			u.uenmax--;
			Your("mana was drained by 1!");
		}
		if (u.uen > u.uenmax) u.uen = u.uenmax;

	}

	if (uactivesymbiosis && !rn2(2)) {
		lowerceiling = 200;
		upperceiling = 300;
		reduceamount = 1;

		int symlevel = mons[u.usymbiote.mnum].mlevel;
		if (symlevel < 5) symlevel = 5;
		else if (symlevel == 5) symlevel = 6;
		if (u.usymbiote.mhpmax > (symlevel * 8)) upperceiling = (symlevel * 8);

		if (Role_if(PM_SYMBIANT)) {
			lowerceiling = 400;
			upperceiling = 500;
		}

		if (lowerceiling < 10) lowerceiling = 10; /* fail safe */
		if (upperceiling < 15) upperceiling = 15; /* fail safe */

		if (u.usymbiote.mhpmax > upperceiling) {
			reduceamount = rnd(u.usymbiote.mhpmax / 10);
			u.usymbiote.mhpmax -= reduceamount;
			Your("symbiote's health was drained by %d!", reduceamount);
		} else if (u.usymbiote.mhpmax > lowerceiling && !rn2(3)) {
			u.usymbiote.mhpmax--;
			Your("symbiote's health was drained by 1!");
		}
		if (u.usymbiote.mhp > u.usymbiote.mhpmax) u.usymbiote.mhp = u.usymbiote.mhpmax;

	}

	flags.botl = TRUE;
}

/* up-nivellation: increase the player's max HP or Pw if they're below average for current XL --Amy */
void
upnivel(guaranteed)
boolean guaranteed;
{
	if (issoviet && !guaranteed) return; /* lol */
	if (evilfriday && !guaranteed && rn2(3)) return; /* tough luck */
	if (u.ulevel < 4) return; /* not available yet */

	int nivellevel = u.ulevel;
	int ceiling = 1;
	int increaseamount = 1;

	{ /* HP */

		ceiling = (nivellevel * 10);

		if (ACURR(A_CON) < 11) {
			ceiling *= 4;
			ceiling /= 5;
		}
		if (ACURR(A_CON) < 7) {
			ceiling /= 2;
		}
		if (ACURR(A_CON) > 19) {
			ceiling *= 5;
			ceiling /= 4;
		}

		if (Role_if(PM_ASTRONAUT)) {
			ceiling *= 6;
			ceiling /= 5;
		}
		if (Role_if(PM_BARBARIAN)) {
			ceiling *= 5;
			ceiling /= 4;
		}
		if (Role_if(PM_AUGURER)) {
			ceiling *= 4;
			ceiling /= 5;
		}
		if (Role_if(PM_BLEEDER)) {
			ceiling *= 2;
		}
		if (Role_if(PM_DRUID)) {
			ceiling /= 2;
		}
		if (Role_if(PM_DANCER)) {
			ceiling /= 2;
		}
		if (Role_if(PM_ORDINATOR)) {
			ceiling /= 2;
		}
		if (Role_if(PM_UNBELIEVER)) {
			ceiling *= 2;
		}
		if (Role_if(PM_WARRIOR)) {
			ceiling *= 3;
			ceiling /= 2;
		}
		if (Race_if(PM_YEEK)) {
			ceiling /= 2;
		}
		if (Race_if(PM_DUFFLEPUD)) {
			ceiling /= 2;
		}
		if (Race_if(PM_PLAYER_DOLGSMAN)) {
			ceiling *= 3;
			ceiling /= 4;
		}
		if (Race_if(PM_SPRIGGAN)) {
			ceiling *= 3;
			ceiling /= 4;
		}
		if (Race_if(PM_PLAYER_FAIRY)) {
			ceiling /= 2;
		}
		if (Race_if(PM_BACTERIA)) {
			ceiling *= 3;
			ceiling /= 2;
		}
		if (Race_if(PM_DEVELOPER)) {
			ceiling *= 3;
			ceiling /= 2;
		}
		if (Race_if(PM_GIGANT)) {
			ceiling *= 5;
			ceiling /= 4;
		}
		if (Race_if(PM_INKA)) {
			ceiling *= 6;
			ceiling /= 5;
		}
		if (Race_if(PM_ITAQUE)) {
			ceiling *= 7;
			ceiling /= 5;
		}
		if (Race_if(PM_PLAYER_CERBERUS)) {
			ceiling *= 5;
			ceiling /= 4;
		}
		if (Race_if(PM_PLAYER_JABBERWOCK)) {
			ceiling *= 4;
			ceiling /= 3;
		}
		if (Race_if(PM_PLAYER_FAIRY)) {
			ceiling *= 2;
			ceiling /= 3;
		}
		if (Race_if(PM_WEAPONIZED_DINOSAUR)) {
			ceiling *= 4;
			ceiling /= 3;
		}
		if (Race_if(PM_SHELL)) {
			ceiling *= 4;
			ceiling /= 3;
		}
		if (Race_if(PM_CARTHAGE)) {
			ceiling *= 9;
			ceiling /= 10;
		}
		if (Race_if(PM_VIKING)) {
			ceiling *= 9;
			ceiling /= 10;
		}

		if (ceiling < 10) ceiling = 10; /* fail safe */

		if (u.uhpmax < ceiling) {
			increaseamount = (ceiling / 10);
			if (increaseamount < 1) increaseamount = 1; /* fail safe */
			u.uhpmax += increaseamount;
			if (u.uhpmax > ceiling) u.uhpmax = ceiling; /* fail safe */
			Your("health was recovered by %d.", increaseamount);
		}

		if (Upolyd) {

			if (u.mhmax < ceiling) {
				increaseamount = (ceiling / 10);
				if (increaseamount < 1) increaseamount = 1; /* fail safe */
				u.mhmax += increaseamount;
				if (u.mhmax > ceiling) u.mhmax = ceiling; /* fail safe */
				Your("polymorphed health was recovered by %d.", increaseamount);
			}

		}

	}
	{ /* Pw */

		ceiling = (nivellevel * 10);

		if (ACURR(A_WIS) < 11) {
			ceiling *= 4;
			ceiling /= 5;
		}
		if (ACURR(A_WIS) < 7) {
			ceiling /= 2;
		}
		if (ACURR(A_WIS) > 19) {
			ceiling *= 5;
			ceiling /= 4;
		}

		if (Role_if(PM_ALTMER)) {
			ceiling *= 3;
			ceiling /= 2;
		}
		if (Role_if(PM_MASTERMIND)) {
			ceiling *= 4;
			ceiling /= 3;
		}
		if (Role_if(PM_PSYKER)) {
			ceiling *= 5;
			ceiling /= 4;
		}
		if (Role_if(PM_WIZARD)) {
			ceiling *= 5;
			ceiling /= 4;
		}
		if (Role_if(PM_UNBELIEVER)) {
			ceiling /= 5;
		}
		if (Role_if(PM_NOOB_MODE_BARB)) {
			ceiling /= 2;
		}
		if (Race_if(PM_YEEK)) {
			ceiling /= 2;
		}
		if (Race_if(PM_DUFFLEPUD)) {
			ceiling /= 2;
		}
		if (Race_if(PM_BACTERIA)) {
			ceiling *= 3;
			ceiling /= 2;
		}
		if (Race_if(PM_BRETON)) {
			ceiling *= 4;
			ceiling /= 3;
		}
		if (Race_if(PM_DEVELOPER)) {
			ceiling *= 3;
			ceiling /= 2;
		}
		if (Race_if(PM_TROLLOR)) {
			ceiling *= 2;
			ceiling /= 3;
		}
		if (Race_if(PM_REDGUARD)) {
			ceiling /= 2;
		}
		if (Race_if(PM_YOKUDA)) {
			ceiling *= 2;
			ceiling /= 3;
		}
		if (Race_if(PM_RODNEYAN)) {
			ceiling *= 2;
		}
		if (Race_if(PM_CARTHAGE)) {
			ceiling *= 9;
			ceiling /= 10;
		}
		if (Race_if(PM_LICH_WARRIOR)) {
			ceiling *= 5;
			ceiling /= 4;
		}
		if (Race_if(PM_WISP)) {
			ceiling *= 4;
			ceiling /= 3;
		}

		if (ceiling < 10) ceiling = 10; /* fail safe */

		if (u.uenmax < ceiling) {
			increaseamount = (ceiling / 10);
			if (increaseamount < 1) increaseamount = 1; /* fail safe */
			u.uenmax += increaseamount;
			if (u.uenmax > ceiling) u.uenmax = ceiling; /* fail safe */
			Your("mana was recovered by %d.", increaseamount);
		}

	}

	if (uactivesymbiosis) {
		int symlevel = mons[u.usymbiote.mnum].mlevel;
		if (symlevel < 6) symlevel = 6;
		ceiling = (symlevel * 10);
		if (PlayerCannotUseSkills || (P_SKILL(P_SYMBIOSIS) < P_SKILLED)) ceiling = (symlevel * 8);
		if (Role_if(PM_SYMBIANT)) {
			ceiling *= 2;
		}

		if (u.usymbiote.mhpmax < ceiling) {
			int actualincrease = 1;
			increaseamount = (ceiling / 5);
			if (increaseamount < 1) increaseamount = 1; /* fail safe */
			actualincrease = u.usymbiote.mhpmax;
			u.usymbiote.mhpmax += increaseamount;
			if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
			if (u.usymbiote.mhpmax > ceiling) u.usymbiote.mhpmax = ceiling; /* fail safe */
			if ((u.usymbiote.mhpmax - actualincrease) > increaseamount) {
				increaseamount = (u.usymbiote.mhpmax - actualincrease);
			}

			if (increaseamount > 0) Your("symbiote's health was boosted by %d.", increaseamount);
		}

	}

	flags.botl = TRUE;
}

/* if you're an angel, shadowstuff items reduce your stats --Amy */
int
angelshadowstuff()
{
	int shadowitems = 0;

	if (!Race_if(PM_HUMANOID_ANGEL)) return 0;
	if (uarm && objects[(uarm)->otyp].oc_material == MT_SHADOWSTUFF) {
		shadowitems++;
	}
	if (uarmf && objects[(uarmf)->otyp].oc_material == MT_SHADOWSTUFF) {
		shadowitems++;
	}
	if (uarmg && objects[(uarmg)->otyp].oc_material == MT_SHADOWSTUFF) {
		shadowitems++;
	}
	if (uarmh && objects[(uarmh)->otyp].oc_material == MT_SHADOWSTUFF) {
		shadowitems++;
	}
	if (uarms && objects[(uarms)->otyp].oc_material == MT_SHADOWSTUFF) {
		shadowitems++;
	}
	if (uarmc && objects[(uarmc)->otyp].oc_material == MT_SHADOWSTUFF) {
		shadowitems++;
	}
	if (uarmu && objects[(uarmu)->otyp].oc_material == MT_SHADOWSTUFF) {
		shadowitems++;
	}
	if (uamul && objects[(uamul)->otyp].oc_material == MT_SHADOWSTUFF) {
		shadowitems++;
	}
	if (uimplant && objects[(uimplant)->otyp].oc_material == MT_SHADOWSTUFF) {
		shadowitems++;
	}
	if (uleft && objects[(uleft)->otyp].oc_material == MT_SHADOWSTUFF) {
		shadowitems++;
	}
	if (uright && objects[(uright)->otyp].oc_material == MT_SHADOWSTUFF) {
		shadowitems++;
	}
	if (ublindf && objects[(ublindf)->otyp].oc_material == MT_SHADOWSTUFF) {
		shadowitems++;
	}
	if (uwep && objects[(uwep)->otyp].oc_material == MT_SHADOWSTUFF) {
		shadowitems++;
	}
	if (u.twoweap && uswapwep && objects[(uswapwep)->otyp].oc_material == MT_SHADOWSTUFF) {
		shadowitems++;
	}
	return shadowitems;
}

/* Data delete: designed to be the worst random bad effect that you can receive --Amy
 * These effects are generally meant to be extremely destructive, and should really screw up the player. */
void
datadeleteattack()
{

	u.cnd_datadeleteamount++;
	Your("data is deleted!");
	stop_occupation();

	switch (rnd(23)) {


		case 1: /* very strong amnesia effect - let that poor sap forget a whole lot of stuff */
			forget(rn2(10) ? 35 : 100);
			forget_levels(rn2(10) ? 35 : 100);
			forget_objects(rn2(10) ? 35 : 100);
			u.bucskill = 0;
			u.enchantrecskill = 0;
			u.weapchantrecskill = 0;
			pline("Suddenly you don't remember anything.");
			break;
		case 2: /* delete a ton of their spells */
			while (rn2(10)) {
				losespells();
			}
			pline("Suddenly you don't remember your spells.");
			break;
		case 3: /* drain 10 random attributes permanently, and deal high damage if it's already at the minimum */
			pline("You feel a severe attribute loss.");
			{
			int attributelose = rn2(A_MAX);
			if(ABASE(attributelose) > ATTRMIN(attributelose)) {
				if (ABASE(attributelose) > 17) ABASE(attributelose) -= rnd(15);
				else if (ABASE(attributelose) > 13) ABASE(attributelose) -= rnd(10);
				else if (ABASE(attributelose) > 8) ABASE(attributelose) -= rnd(5);
				else ABASE(attributelose) -= 1;
				AMAX(attributelose) -= 1;
				if (AMAX(attributelose) > ABASE(attributelose)) AMAX(attributelose) = ABASE(attributelose);

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low stats", KILLED_BY);
			}
			if(ABASE(attributelose) > ATTRMIN(attributelose)) {
				if (ABASE(attributelose) > 17) ABASE(attributelose) -= rnd(15);
				else if (ABASE(attributelose) > 13) ABASE(attributelose) -= rnd(10);
				else if (ABASE(attributelose) > 8) ABASE(attributelose) -= rnd(5);
				else ABASE(attributelose) -= 1;
				AMAX(attributelose) -= 1;
				if (AMAX(attributelose) > ABASE(attributelose)) AMAX(attributelose) = ABASE(attributelose);

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low stats", KILLED_BY);
			}
			if(ABASE(attributelose) > ATTRMIN(attributelose)) {
				if (ABASE(attributelose) > 17) ABASE(attributelose) -= rnd(15);
				else if (ABASE(attributelose) > 13) ABASE(attributelose) -= rnd(10);
				else if (ABASE(attributelose) > 8) ABASE(attributelose) -= rnd(5);
				else ABASE(attributelose) -= 1;
				AMAX(attributelose) -= 1;
				if (AMAX(attributelose) > ABASE(attributelose)) AMAX(attributelose) = ABASE(attributelose);

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low stats", KILLED_BY);
			}
			if(ABASE(attributelose) > ATTRMIN(attributelose)) {
				if (ABASE(attributelose) > 17) ABASE(attributelose) -= rnd(15);
				else if (ABASE(attributelose) > 13) ABASE(attributelose) -= rnd(10);
				else if (ABASE(attributelose) > 8) ABASE(attributelose) -= rnd(5);
				else ABASE(attributelose) -= 1;
				AMAX(attributelose) -= 1;
				if (AMAX(attributelose) > ABASE(attributelose)) AMAX(attributelose) = ABASE(attributelose);

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low stats", KILLED_BY);
			}
			if(ABASE(attributelose) > ATTRMIN(attributelose)) {
				if (ABASE(attributelose) > 17) ABASE(attributelose) -= rnd(15);
				else if (ABASE(attributelose) > 13) ABASE(attributelose) -= rnd(10);
				else if (ABASE(attributelose) > 8) ABASE(attributelose) -= rnd(5);
				else ABASE(attributelose) -= 1;
				AMAX(attributelose) -= 1;
				if (AMAX(attributelose) > ABASE(attributelose)) AMAX(attributelose) = ABASE(attributelose);

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low stats", KILLED_BY);
			}
			if(ABASE(attributelose) > ATTRMIN(attributelose)) {
				if (ABASE(attributelose) > 17) ABASE(attributelose) -= rnd(15);
				else if (ABASE(attributelose) > 13) ABASE(attributelose) -= rnd(10);
				else if (ABASE(attributelose) > 8) ABASE(attributelose) -= rnd(5);
				else ABASE(attributelose) -= 1;
				AMAX(attributelose) -= 1;
				if (AMAX(attributelose) > ABASE(attributelose)) AMAX(attributelose) = ABASE(attributelose);

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low stats", KILLED_BY);
			}
			if(ABASE(attributelose) > ATTRMIN(attributelose)) {
				if (ABASE(attributelose) > 17) ABASE(attributelose) -= rnd(15);
				else if (ABASE(attributelose) > 13) ABASE(attributelose) -= rnd(10);
				else if (ABASE(attributelose) > 8) ABASE(attributelose) -= rnd(5);
				else ABASE(attributelose) -= 1;
				AMAX(attributelose) -= 1;
				if (AMAX(attributelose) > ABASE(attributelose)) AMAX(attributelose) = ABASE(attributelose);

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low stats", KILLED_BY);
			}
			if(ABASE(attributelose) > ATTRMIN(attributelose)) {
				if (ABASE(attributelose) > 17) ABASE(attributelose) -= rnd(15);
				else if (ABASE(attributelose) > 13) ABASE(attributelose) -= rnd(10);
				else if (ABASE(attributelose) > 8) ABASE(attributelose) -= rnd(5);
				else ABASE(attributelose) -= 1;
				AMAX(attributelose) -= 1;
				if (AMAX(attributelose) > ABASE(attributelose)) AMAX(attributelose) = ABASE(attributelose);

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low stats", KILLED_BY);
			}
			if(ABASE(attributelose) > ATTRMIN(attributelose)) {
				if (ABASE(attributelose) > 17) ABASE(attributelose) -= rnd(15);
				else if (ABASE(attributelose) > 13) ABASE(attributelose) -= rnd(10);
				else if (ABASE(attributelose) > 8) ABASE(attributelose) -= rnd(5);
				else ABASE(attributelose) -= 1;
				AMAX(attributelose) -= 1;
				if (AMAX(attributelose) > ABASE(attributelose)) AMAX(attributelose) = ABASE(attributelose);

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low stats", KILLED_BY);
			}
			if(ABASE(attributelose) > ATTRMIN(attributelose)) {
				if (ABASE(attributelose) > 17) ABASE(attributelose) -= rnd(15);
				else if (ABASE(attributelose) > 13) ABASE(attributelose) -= rnd(10);
				else if (ABASE(attributelose) > 8) ABASE(attributelose) -= rnd(5);
				else ABASE(attributelose) -= 1;
				AMAX(attributelose) -= 1;
				if (AMAX(attributelose) > ABASE(attributelose)) AMAX(attributelose) = ABASE(attributelose);

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low stats", KILLED_BY);
			}

			}
			break;
		case 4: /* drain all attributes permanently, and deal high damage for those that were already at the minimum */
			pline("You feel all your attributes draining.");
			if(ABASE(A_STR) > ATTRMIN(A_STR)) {
				if (ABASE(A_STR) > 17) ABASE(A_STR) -= rnd(15);
				else if (ABASE(A_STR) > 13) ABASE(A_STR) -= rnd(10);
				else if (ABASE(A_STR) > 8) ABASE(A_STR) -= rnd(5);
				else ABASE(A_STR) -= 1;
				AMAX(A_STR) -= 1;
				if (AMAX(A_STR) > ABASE(A_STR)) AMAX(A_STR) = ABASE(A_STR);

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low strength", KILLED_BY);
			}
			if(ABASE(A_DEX) > ATTRMIN(A_DEX)) {
				if (ABASE(A_DEX) > 17) ABASE(A_DEX) -= rnd(15);
				else if (ABASE(A_DEX) > 13) ABASE(A_DEX) -= rnd(10);
				else if (ABASE(A_DEX) > 8) ABASE(A_DEX) -= rnd(5);
				else ABASE(A_DEX) -= 1;
				AMAX(A_DEX) -= 1;
				if (AMAX(A_DEX) > ABASE(A_DEX)) AMAX(A_DEX) = ABASE(A_DEX);

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low dexterity", KILLED_BY);
			}
			if(ABASE(A_WIS) > ATTRMIN(A_WIS)) {
				if (ABASE(A_WIS) > 17) ABASE(A_WIS) -= rnd(15);
				else if (ABASE(A_WIS) > 13) ABASE(A_WIS) -= rnd(10);
				else if (ABASE(A_WIS) > 8) ABASE(A_WIS) -= rnd(5);
				else ABASE(A_WIS) -= 1;
				AMAX(A_WIS) -= 1;
				if (AMAX(A_WIS) > ABASE(A_WIS)) AMAX(A_WIS) = ABASE(A_WIS);

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low wisdom", KILLED_BY);
			}
			if(ABASE(A_INT) > ATTRMIN(A_INT)) {
				if (ABASE(A_INT) > 17) ABASE(A_INT) -= rnd(15);
				else if (ABASE(A_INT) > 13) ABASE(A_INT) -= rnd(10);
				else if (ABASE(A_INT) > 8) ABASE(A_INT) -= rnd(5);
				else ABASE(A_INT) -= 1;
				AMAX(A_INT) -= 1;
				if (AMAX(A_INT) > ABASE(A_INT)) AMAX(A_INT) = ABASE(A_INT);

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low intelligence", KILLED_BY);
			}
			if(ABASE(A_CHA) > ATTRMIN(A_CHA)) {
				if (ABASE(A_CHA) > 17) ABASE(A_CHA) -= rnd(15);
				else if (ABASE(A_CHA) > 13) ABASE(A_CHA) -= rnd(10);
				else if (ABASE(A_CHA) > 8) ABASE(A_CHA) -= rnd(5);
				else ABASE(A_CHA) -= 1;
				AMAX(A_CHA) -= 1;
				if (AMAX(A_CHA) > ABASE(A_CHA)) AMAX(A_CHA) = ABASE(A_CHA);

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low charisma", KILLED_BY);
			}
			if(ABASE(A_CON) > ATTRMIN(A_CON)) {
				if (ABASE(A_CON) > 17) ABASE(A_CON) -= rnd(15);
				else if (ABASE(A_CON) > 13) ABASE(A_CON) -= rnd(10);
				else if (ABASE(A_CON) > 8) ABASE(A_CON) -= rnd(5);
				else ABASE(A_CON) -= 1;
				AMAX(A_CON) -= 1;
				if (AMAX(A_CON) > ABASE(A_CON)) AMAX(A_CON) = ABASE(A_CON);

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low constitution", KILLED_BY);
			}
			break;
		case 5: /* you think you can pray, punk? now you can't :P */
			u.ublesscnt += 1000000;
			pline("You cannot pray any longer.");
			break;
		case 6: /* oh whoops, suddenly your alignment record is shit and it'll take a long time to repair it! */
			if (rn2(10)) {
				u.ualign.sins += 100;
				u.alignlim -= 100;
			} else {
				u.ualign.sins += 1000;
				u.alignlim -= 1000;
			}
		      adjalign(-1000);
			pline("Your alignment is nuked.");
			break;
		case 7: /* lose all experience levels - have fun rebuilding your char from XL1! */
			pline("Your level is resetted to 1.");
			while (u.ulevel > 1) losexp("deleterious level drain", TRUE, FALSE);
			if (u.uhpmax < (urole.hpadv.infix + urace.hpadv.infix)) {
				u.uhpmax = urole.hpadv.infix + urace.hpadv.infix;
				if (Role_if(PM_DQ_SLIME) && Race_if(PM_PLAYER_SLIME)) u.uhpmax += 20;
				if (u.uhp < u.uhpmax) u.uhp = u.uhpmax;
			}
			break;
		case 8: /* steal 20 random intrinsics, so the chance of stealing something important is high, muahahahaha */
			pline("You feel a loss of intrinsics...");
			attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse();
			break;
		case 9: /* deactivate an intrinsic for a million turns so you have to make do without it for a loooooong time */
			pline("One of your intrinsics is permanently deactivated!");
			deacrandomintrinsic(1000000);

			break;
		case 10: /* trash the player's skill caps - they're hard to restore! */
			pline("You feel much less skilled than before.");
			skillcaploss(); skillcaploss(); skillcaploss(); skillcaploss(); skillcaploss(); skillcaploss(); skillcaploss(); skillcaploss(); skillcaploss(); skillcaploss();
			break;
		case 11: /* damage all items in open inventory twice, with no real chance for them to resist */
			pline("Your possessions are severely damaged!");
			withering_damage(invent, FALSE, FALSE);
			withering_damage(invent, FALSE, FALSE);
			break;
		case 12: /* deal a lot of lethe damage to ensure that magic lamps and similar stuff gets ruined */
			pline("Your magical items turn into mundane ones.");
			lethe_damage(invent, FALSE, FALSE);
			lethe_damage(invent, FALSE, FALSE);
			lethe_damage(invent, FALSE, FALSE);
			lethe_damage(invent, FALSE, FALSE);
			lethe_damage(invent, FALSE, FALSE);
			actual_lethe_damage(invent, FALSE, FALSE);
			actual_lethe_damage(invent, FALSE, FALSE);
			actual_lethe_damage(invent, FALSE, FALSE);
			actual_lethe_damage(invent, FALSE, FALSE);
			actual_lethe_damage(invent, FALSE, FALSE);
			break;
		case 13: /* oh lol you just lost all your armor - better hope you have replacements! */
			pline("You lose all of your armor!");
			{
			struct obj *otmpD;
			otmpD = some_armor(&youmonst);
			if(otmpD) useup(otmpD);
			otmpD = some_armor(&youmonst);
			if(otmpD) useup(otmpD);
			otmpD = some_armor(&youmonst);
			if(otmpD) useup(otmpD);
			otmpD = some_armor(&youmonst);
			if(otmpD) useup(otmpD);
			otmpD = some_armor(&youmonst);
			if(otmpD) useup(otmpD);
			otmpD = some_armor(&youmonst);
			if(otmpD) useup(otmpD);
			otmpD = some_armor(&youmonst);
			if(otmpD) useup(otmpD);
			}

			break;
		case 14: /* divide max HP and Pw by two - especially nasty if your limits were high *evil grin* */
			pline("Your health and mana are severely damaged!");
			if (u.uhpmax > 1) u.uhpmax /= 2;
			else {
				killer_format = KILLED_BY;
				killer = "running out of health";
				done(DIED);
			}
			if (u.uenmax > 1) u.uenmax /= 2;
			else u.uenmax = 0;
			if (Upolyd) {
				u.mhmax /= 2;
				if (u.mh > u.mhmax) u.mh = u.mhmax;
			}
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			break;
		case 15: /* randomly put some techs on such a high timeout that you won't be able to use them for an eternity */
			datadeletetechs();
			pline("Some of your techniques no longer work...");
			break;
		case 16: /* each of your items has 1 in 5 chance of disappearing, with no saving throw, hahahahahahaha */
			pline("Your items are randomly deleted!");

			{
			struct obj *otmpD, *otmpE;

			for (otmpD = invent; otmpD; otmpD = otmpE) {
			      otmpE = otmpD->nobj;

				if (rn2(5)) continue;

				if (evades_destruction(otmpD) ) dropx(otmpD);
				else {

					/* if it's a bag of holding or other container, scatter the contents first --Amy */
					if (Has_contents(otmpD)) {
						dump_container(otmpD, FALSE, u.ux, u.uy);
						scatter(u.ux,u.uy,10,VIS_EFFECTS|MAY_HIT|MAY_DESTROY|MAY_FRACTURE,0);
					}
					/* and if it still somehow has contents now, delete them */
					if (Has_contents(otmpD)) delete_contents(otmpD);
					useup(otmpD);
				}
			}

			}

			break;
		case 17: /* curse and disenchant your entire inventory */
			pline("Your items are cursed and disenchanted!");
			{
			struct obj *otmpD, *otmpE;

			for (otmpD = invent; otmpD; otmpD = otmpE) {
			      otmpE = otmpD->nobj;

				curse(otmpD);
				if (otmpD->spe > -1) otmpD->spe = -1;
				/* bugsniper said this effect isn't evil enough; let's disenchant stuff down to -50 --Amy */
				else if (otmpD->spe > -50) otmpD->spe--;

			}

			}

			break;

		case 18: /* flip positively enchanted items, then disenchant every item by 5 more points */
			pline("Your items are ruined!");
			{
			struct obj *otmpD, *otmpE;

			for (otmpD = invent; otmpD; otmpD = otmpE) {
			      otmpE = otmpD->nobj;

				if (otmpD->spe > 0) {
					otmpD->spe = -(otmpD->spe);
					if (otmpD->spe < -50) otmpD->spe = -50;
				}
				if (otmpD->spe > -25) otmpD->spe -= 5;
				else if (otmpD->spe > -50) otmpD->spe--;
				if (otmpD->oerodeproof) otmpD->oerodeproof = 0; /* it's not erodeproof anymore */
				if (otmpD->greased) otmpD->greased = 0; /* and not greased either */
				otmpD->oinvis = otmpD->oinvisreal = 0; /* and of course also not invisible */

			}

			}

			break;

		case 19: /* reduce the player's effective carry cap for a looooong time */
			u.graundweight += rn1(2000, 1000);
			pline("Your encumbrance limit is much lower than before.");

			break;


		case 20: /* deactivate ALL intrinsics for a lengthy amount of time */
			u.uprops[DEAC_FIRE_RES].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_COLD_RES].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_SLEEP_RES].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_DISINT_RES].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_SHOCK_RES].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_POISON_RES].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_DRAIN_RES].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_SICK_RES].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_ANTIMAGIC].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_ACID_RES].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_STONE_RES].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_FEAR_RES].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_SEE_INVIS].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_TELEPAT].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_WARNING].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_SEARCHING].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_CLAIRVOYANT].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_INFRAVISION].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_DETECT_MONSTERS].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_INVIS].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_DISPLACED].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_STEALTH].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_JUMPING].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_TELEPORT_CONTROL].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_FLYING].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_MAGICAL_BREATHING].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_PASSES_WALLS].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_SLOW_DIGESTION].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_HALF_SPDAM].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_HALF_PHDAM].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_REGENERATION].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_ENERGY_REGENERATION].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_FAST].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_REFLECTING].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_FREE_ACTION].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_HALLU_PARTY].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_DRUNKEN_BOXING].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_STUNNOPATHY].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_NUMBOPATHY].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_DIMMOPATHY].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_FREEZOPATHY].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_STONED_CHILLER].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_CORROSIVITY].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_FEAR_FACTOR].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_BURNOPATHY].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_SICKOPATHY].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_KEEN_MEMORY].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_THE_FORCE].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_SIGHT_BONUS].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_VERSUS_CURSES].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_STUN_RES].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_CONF_RES].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_DOUBLE_ATTACK].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_QUAD_ATTACK].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_PSI_RES].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_WONDERLEGS].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_GLIB_COMBAT].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_MANALEECH].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_PEACEVISION].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_CONT_RES].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_DISCOUNT_ACTION].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_FULL_NUTRIENT].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_TECHNICALITY].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_SCENT_VIEW].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_DIMINISHED_BLEEDING].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_CONTROL_MAGIC].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_EXP_BOOST].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_HALLUC_RES].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_BLIND_RES].intrinsic += (rn2(10) ? 10000 : 100000);
			u.uprops[DEAC_ASTRAL_VISION].intrinsic += (rn2(10) ? 10000 : 100000);
			pline("All your in- and extrinsics are deactivated!");
			break;

		case 21: /* set all skills back to Unskilled and 0 points of training to ruin all the player's training effort */
			dataskilldecrease();
			pline("You lose all training in all skills!");
			break;

		case 22: /* lose 20 skill points forever, undoing any enhancements that you did with those points - you won't get them back, sucker :-P */
			lose_weapon_skill(20);
			pline("You lose your skill slots forever!");

			break;

		case 23: /* set the level of a random technique to -100 so you can't ever use it again, muahahahaha */
			techdrainsevere();
			break;

	}

	u.datadeletedefer = 0;

}

/* Do something bad to the player's pet --Amy */
void
badpeteffect(mtmp)
register struct monst *mtmp;
{
	if (!mtmp) return; /* shouldn't happen */
	if (!mtmp->mtame) return; /* shouldn't happen either */

	boolean vis = cansee(mtmp->mx,mtmp->my);

	switch (rnd(51)) {

		case 1:
		case 2:
		case 3:
			if (mtmp->mspeed != MSLOW) {
				mon_adjust_speed(mtmp, -1, (struct obj *)0);
				if (vis) pline("%s slows down.", Monnam(mtmp));
			}
			break;
		case 4:
		case 5:
		case 6:
			mtmp->mhpmax -= rnd(10);
			if (mtmp->mhpmax < 1) mtmp->mhpmax = 1;
			if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;
			if (vis) pline("%s's health is damaged.", Monnam(mtmp));
			break;
		case 7:
			mtmp->m_enmax -= rnd(10);
			if (mtmp->m_enmax < 0) mtmp->m_enmax = 0;
			if (mtmp->m_en > mtmp->m_enmax) mtmp->m_en = mtmp->m_enmax;
			if (vis) pline("%s seems less energized.", Monnam(mtmp));
			break;
		case 8:
			if (!mtmp->mcan) {
				cancelmonsterlite(mtmp);
				if (vis) pline("%s is cancelled.", Monnam(mtmp));
			}
			break;
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
			monflee(mtmp, rnd(1 + level_difficulty()), FALSE, TRUE);
			if (vis) pline("%s becomes afraid.", Monnam(mtmp));
			break;
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
			{
				int rnd_tmp;
				rnd_tmp = rnd(1 + (level_difficulty() * 2));
				if ((rnd_tmp += mtmp->mblinded) > 127) rnd_tmp = 127;
				mtmp->mblinded = rnd_tmp;
				mtmp->mcansee = 0;
			}
			if (vis) pline("%s is blinded.", Monnam(mtmp));
			break;
		case 19:
		case 20:
		case 21:
			{
				int rnd_tmp;
				rnd_tmp = rnd(1 + level_difficulty());
				if (rnd_tmp > 1) rnd_tmp = rnd(rnd_tmp);
				if (rnd_tmp > 127) rnd_tmp = 127;
				mtmp->mcanmove = 0;
				mtmp->mfrozen = rnd_tmp;
			}
			if (vis) pline("%s is paralyzed.", Monnam(mtmp));
			break;
		case 22:
		case 23:
		case 24:
		case 25:
		case 26:
			mtmp->mstun = TRUE;
			if (vis) pline("%s is stunned.", Monnam(mtmp));
			break;
		case 27:
		case 28:
		case 29:
		case 30:
		case 31:
			mtmp->mconf = TRUE;
			if (vis) pline("%s is confused.", Monnam(mtmp));
			break;
		case 32:
			if (!rn2(10000)) mtmp->willbebanished = TRUE;
			else if (!tele_restrict(mtmp)) {
				if (vis) pline("%s disappears.", Monnam(mtmp));
				(void) rloc(mtmp, FALSE);
			}
			break;
		case 33:
		case 34:
		case 35:
		case 36:
			if (vis) {
				if (!mtmp->bleedout) pline("%s starts bleeding.", Monnam(mtmp));
				else pline("%s's bleeding gets stronger.", Monnam(mtmp));
			}
			mtmp->bleedout += rnd(1 + (level_difficulty() * 5));
			break;
		case 37:
		case 38:
		case 39:
		case 40:
		case 41:
			mtmp->healblock += rnd(1 + (level_difficulty() * 5));
			if (vis) pline("%s's healing is blocked.", Monnam(mtmp));
			break;

		case 42:
		case 43:
		case 44:
		case 45:
		case 46:
			makedoghungry(mtmp, (1 + level_difficulty()) * rnd(50));
			if (vis) pline("%s looks hungry.", Monnam(mtmp));
			break;
		case 47:
		case 48:
			mtmp->inertia += rnd(1 + (level_difficulty() * rnd(50)));
			if (vis) pline("%s slows to a crawl.", Monnam(mtmp));
			break;
		case 49:
		case 50:
		case 51:
			if (mtmp->mhpmax > (Role_if(PM_ZOOKEEPER) ? 480 : 240)) {
				int reduction = rnd(mtmp->mhpmax / 10);
				if (reduction < 1) reduction = 1; /* shouldn't happen */
				mtmp->mhpmax -= reduction;
				if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;
				if (vis) pline("%s is in pain!", Monnam(mtmp));
			} else if (mtmp->mhpmax > (Role_if(PM_ZOOKEEPER) ? 320 : 160)) {
				mtmp->mhpmax--;
				if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;
				if (vis) pline("%s seems to be hurt!", Monnam(mtmp));
			}
			break;

		default: /* fail safe */
			monflee(mtmp, rnd(1 + level_difficulty()), FALSE, TRUE);
			if (vis) pline("%s becomes afraid.", Monnam(mtmp));
			break;

	}

}

/* Make all the bad stuff happen to your pet at once --Amy */
void
allbadpeteffects(mtmp)
register struct monst *mtmp;
{
	if (!mtmp) return; /* shouldn't happen */
	if (!mtmp->mtame) return; /* shouldn't happen either */

	boolean vis = cansee(mtmp->mx,mtmp->my);

	if (mtmp->mspeed != MSLOW) {
		mon_adjust_speed(mtmp, -1, (struct obj *)0);
		if (vis) pline("%s slows down.", Monnam(mtmp));
	}

	mtmp->mhpmax -= rnd(10);
	if (mtmp->mhpmax < 1) mtmp->mhpmax = 1;
	if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;
	if (vis) pline("%s's health is damaged.", Monnam(mtmp));

	mtmp->m_enmax -= rnd(10);
	if (mtmp->m_enmax < 0) mtmp->m_enmax = 0;
	if (mtmp->m_en > mtmp->m_enmax) mtmp->m_en = mtmp->m_enmax;
	if (vis) pline("%s seems less energized.", Monnam(mtmp));

	if (!mtmp->mcan) {
		cancelmonsterlite(mtmp);
		if (vis) pline("%s is cancelled.", Monnam(mtmp));
	}

	monflee(mtmp, rnd(1 + level_difficulty()), FALSE, TRUE);
	if (vis) pline("%s becomes afraid.", Monnam(mtmp));

	{
		int rnd_tmp;
		rnd_tmp = rnd(1 + (level_difficulty() * 2));
		if ((rnd_tmp += mtmp->mblinded) > 127) rnd_tmp = 127;
		mtmp->mblinded = rnd_tmp;
		mtmp->mcansee = 0;
	}
	if (vis) pline("%s is blinded.", Monnam(mtmp));

	{
		int rnd_tmp;
		rnd_tmp = rnd(1 + level_difficulty());
		if (rnd_tmp > 1) rnd_tmp = rnd(rnd_tmp);
		if (rnd_tmp > 127) rnd_tmp = 127;
		mtmp->mcanmove = 0;
		mtmp->mfrozen = rnd_tmp;
	}

	mtmp->mstun = TRUE;
	if (vis) pline("%s is stunned.", Monnam(mtmp));

	mtmp->mconf = TRUE;
	if (vis) pline("%s is confused.", Monnam(mtmp));

	if (vis) {
		if (!mtmp->bleedout) pline("%s starts bleeding.", Monnam(mtmp));
		else pline("%s's bleeding gets stronger.", Monnam(mtmp));
	}
	mtmp->bleedout += rnd(1 + (level_difficulty() * 5));

	mtmp->healblock += rnd(1 + (level_difficulty() * 5));
	if (vis) pline("%s's healing is blocked.", Monnam(mtmp));

	mtmp->inertia += rnd(1 + (level_difficulty() * rnd(50)));
	if (vis) pline("%s slows to a crawl.", Monnam(mtmp));

	makedoghungry(mtmp, (1 + level_difficulty()) * rnd(50));
	if (vis) pline("%s looks hungry.", Monnam(mtmp));

	if (mtmp->mhpmax > (Role_if(PM_ZOOKEEPER) ? 480 : 240)) {
		int reduction = rnd(mtmp->mhpmax / 10);
		if (reduction < 1) reduction = 1; /* shouldn't happen */
		mtmp->mhpmax -= reduction;
		if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;
		if (vis) pline("%s is in pain!", Monnam(mtmp));
	} else if (mtmp->mhpmax > (Role_if(PM_ZOOKEEPER) ? 320 : 160)) {
		mtmp->mhpmax--;
		if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;
		if (vis) pline("%s seems to be hurt!", Monnam(mtmp));
	}

}

/* The monster mtmp gains an egotype. It may roll one that it already has, in which case nothing happens. --Amy */
void
add_monster_egotype(mtmp)
register struct monst *mtmp;
{
	mtmp->isegotype = 1;
	switch (rnd(223)) {
		case 1:
		case 2:
		case 3: mtmp->egotype_thief = 1; break;
		case 4: mtmp->egotype_wallwalk = 1; break;
		case 5: mtmp->egotype_disenchant = 1; break;
		case 6:
		case 7: mtmp->egotype_rust = 1; break;
		case 8: 
		case 9: mtmp->egotype_corrosion = 1; break;
		case 10: 
		case 11: mtmp->egotype_decay = 1; break;
		case 12: mtmp->egotype_wither = 1; break;
		case 13: 
		case 14: 
		case 15: mtmp->egotype_grab = 1; break;
		case 16: 
		case 17: mtmp->egotype_flying = 1; break;
		case 18: 
		case 19: mtmp->egotype_hide = 1; break;
		case 20: 
		case 21: 
		case 22: mtmp->egotype_regeneration = 1; break;
		case 23: 
		case 24: 
		case 25: mtmp->egotype_undead = 1; break;
		case 26: mtmp->egotype_domestic = 1; break;
		case 27: mtmp->egotype_covetous = 1; break;
		case 28: 
		case 29: mtmp->egotype_avoider = 1; break;
		case 30: mtmp->egotype_petty = 1; break;
		case 31: mtmp->egotype_pokemon = 1; break;
		case 32: mtmp->egotype_slows = 1; break;
		case 33: mtmp->egotype_vampire = 1; break;
		case 34: mtmp->egotype_teleportself = 1; break;
		case 35: mtmp->egotype_teleportyou = 1; break;
		case 36: 
		case 37: mtmp->egotype_wrap = 1; break;
		case 38: mtmp->egotype_disease = 1; break;
		case 39: mtmp->egotype_slime = 1; break;
		case 40: 
		case 41: 
		case 42: 
		case 43: mtmp->egotype_engrave = 1; break;
		case 44: 
		case 45: mtmp->egotype_dark = 1; break;
		case 46: mtmp->egotype_luck = 1; break;
		case 47: 
		case 48: 
		case 49: mtmp->egotype_push = 1; break;
		case 50: mtmp->egotype_arcane = 1; break;
		case 51: mtmp->egotype_clerical = 1; break;
		case 52: 
		case 53: mtmp->egotype_armorer = 1; break;
		case 54: mtmp->egotype_tank = 1; break;
		case 55: 
		case 56: mtmp->egotype_speedster = 1; break;
		case 57: mtmp->egotype_racer = 1; break;
		case 58: mtmp->egotype_randomizer = 1; break;
		case 59: mtmp->egotype_blaster = 1; break;
		case 60: mtmp->egotype_multiplicator = 1; break;
		case 61: mtmp->egotype_gator = 1; break;
		case 62: mtmp->egotype_reflecting = 1; break;
		case 63: mtmp->egotype_hugger = 1; break;
		case 64: mtmp->egotype_mimic = 1; break;
		case 65: mtmp->egotype_permamimic = 1; break;
		case 66:
		case 67: mtmp->egotype_poisoner = 1; break;
		case 68: mtmp->egotype_elementalist = 1; break;
		case 69: mtmp->egotype_resistor = 1; break;
		case 70:
		case 71: mtmp->egotype_acidspiller = 1; break;
		case 72:
		case 73: mtmp->egotype_watcher = 1; break;
		case 74: mtmp->egotype_metallivore = 1; break;
		case 75: mtmp->egotype_lithivore = 1; break;
		case 76: mtmp->egotype_organivore = 1; break;
		case 77: mtmp->egotype_breather = 1; break;
		case 78: mtmp->egotype_beamer = 1; break;
		case 79:
		case 80: mtmp->egotype_troll = 1; break;
		case 81:
		case 82:
		case 83:
		case 84:
		case 85:
		case 86: mtmp->egotype_faker = 1; break;
		case 87:
		case 88:
		case 89:
		case 90: mtmp->egotype_farter = 1; break;
		case 91: mtmp->egotype_timer = 1; break;
		case 92: mtmp->egotype_thirster = 1; break;
		case 93: mtmp->egotype_watersplasher = 1; break;
		case 94: mtmp->egotype_cancellator = 1; break;
		case 95: mtmp->egotype_banisher = 1; break;
		case 96: mtmp->egotype_shredder = 1; break;
		case 97: mtmp->egotype_abductor = 1; break;
		case 98:
		case 99: mtmp->egotype_incrementor = 1; break;
		case 100: mtmp->egotype_mirrorimage = 1; break;
		case 101:
		case 102: mtmp->egotype_curser = 1; break;
		case 103: mtmp->egotype_horner = 1; break;
		case 104: mtmp->egotype_lasher = 1; break;
		case 105: mtmp->egotype_cullen = 1; break;
		case 106:
		case 107:
		case 108: mtmp->egotype_webber = 1; break;
		case 109: mtmp->egotype_itemporter = 1; break;
		case 110: mtmp->egotype_schizo = 1; break;
		case 111: mtmp->egotype_nexus = 1; break;
		case 112: mtmp->egotype_sounder = 1; break;
		case 113: mtmp->egotype_gravitator = 1; break;
		case 114: mtmp->egotype_inert = 1; break;
		case 115:
		case 116: mtmp->egotype_antimage = 1; break;
		case 117: mtmp->egotype_plasmon = 1; break;
		case 118:
		case 119:
		case 120: mtmp->egotype_weaponizer = 1; break;
		case 121: mtmp->egotype_engulfer = 1; break;
		case 122: mtmp->egotype_bomber = 1; break;
		case 123:
		case 124: mtmp->egotype_exploder = 1; break;
		case 125: mtmp->egotype_unskillor = 1; break;
		case 126: mtmp->egotype_blinker = 1; break;
		case 127: mtmp->egotype_psychic = 1; break;
		case 128: mtmp->egotype_abomination = 1; break;
		case 129: mtmp->egotype_gazer = 1; break;
		case 130: mtmp->egotype_seducer = 1; break;
		case 131:
			if (rn2(10)) {
				mtmp->egotype_flickerer = 1; break;
			} else {
				mtmp->egotype_amberite = 1; break;
			}
		case 132:
		case 133: mtmp->egotype_hitter = 1; break;
		case 134: mtmp->egotype_piercer = 1; break;
		case 135: mtmp->egotype_petshielder = 1; break;
		case 136: mtmp->egotype_displacer = 1; break;
		case 137: mtmp->egotype_lifesaver = 1; break;
		case 138: mtmp->egotype_venomizer = 1; break;
		case 139: mtmp->egotype_nastinator = 1; break;
		case 140: mtmp->egotype_baddie = 1; break;
		case 141: mtmp->egotype_dreameater = 1; break;
		case 142: mtmp->egotype_sludgepuddle = 1; break;
		case 143: mtmp->egotype_vulnerator = 1; break;
		case 144: mtmp->egotype_marysue = 1; break;
		case 145: mtmp->egotype_wallwalk = 1; break;
		case 146: mtmp->egotype_reflecting = 1; break;
		case 147: mtmp->egotype_mimic = 1; break;
		case 148: mtmp->egotype_permamimic = 1; break;
		case 149: mtmp->egotype_metallivore = 1; break;
		case 150: mtmp->egotype_lithivore = 1; break;
		case 151: mtmp->egotype_organivore = 1; break;
		case 152: mtmp->egotype_armorer = 1; break;
		case 153: mtmp->egotype_speedster = 1; break;
		case 154: mtmp->egotype_hitter = 1; break;
		case 155: mtmp->egotype_shader = 1; break;
		case 156: mtmp->egotype_amnesiac = 1; break;
		case 157: mtmp->egotype_trapmaster = 1; break;
		case 158: mtmp->egotype_midiplayer = 1; break;
		case 159: mtmp->egotype_rngabuser = 1; break;
		case 160: mtmp->egotype_mastercaster = 1; break;
		case 161: mtmp->egotype_aligner = 1; break;
		case 162: mtmp->egotype_sinner = 1; break;
		case 163: mtmp->egotype_aggravator = 1; break;
		case 164: mtmp->egotype_minator = 1; break;
		case 165: mtmp->egotype_contaminator = 1; break;
		case 166: mtmp->egotype_radiator = 1; break;
		case 167: mtmp->egotype_weeper = 1; break;
		case 168: mtmp->egotype_reactor = 1; break;
		case 169: mtmp->egotype_aligner = 1; break;
		case 170: mtmp->egotype_aligner = 1; break;
		case 171: mtmp->egotype_aggravator = 1; break;
		case 172: mtmp->egotype_contaminator = 1; break;
		case 173: mtmp->egotype_contaminator = 1; break;
		case 174: mtmp->egotype_contaminator = 1; break;
		case 175: mtmp->egotype_trembler = 1; break;
		case 176:
			if (!rn2(1000000)) mtmp->egotype_datadeleter = 1;
			else if (rn2(100)) mtmp->egotype_destructor = 1;
			else mtmp->egotype_worldender = 1;
			break;
		case 177: mtmp->egotype_damager = 1; break;
		case 178: mtmp->egotype_antitype = 1; break;
		case 179:
		case 180: mtmp->egotype_painlord = 1; break;
		case 181: mtmp->egotype_empmaster = 1; break;
		case 182: mtmp->egotype_spellsucker = 1; break;
		case 183: mtmp->egotype_eviltrainer = 1; break;
		case 184: mtmp->egotype_statdamager = 1; break;
		case 185: mtmp->egotype_damagedisher = 1; break;
		case 186: mtmp->egotype_thiefguildmember = 1; break;
		case 187:
		case 188: mtmp->egotype_rogue = 1; break;
		case 189: mtmp->egotype_steed = 1; break;
		case 190:
		case 191:
		case 192:
		case 193:
		case 194: mtmp->egotype_champion = 1; break;
		case 195:
		case 196:
		case 197: mtmp->egotype_boss = 1; break;
		case 198:
			if (rn2(100)) mtmp->egotype_atomizer = 1;
			else mtmp->egotype_laserpwnzor = 1;
			break;
		case 199:
		case 200:
		case 201: mtmp->egotype_perfumespreader = 1; break;
		case 202:
		case 203: mtmp->egotype_converter = 1; break;
		case 204: mtmp->egotype_wouwouer = 1; break;
		case 205: mtmp->egotype_allivore = 1; break;
		case 206:
			if (!rn2(5)) mtmp->egotype_nastycurser = 1;
			else mtmp->egotype_curser = 1;
			break;
		case 207: mtmp->egotype_sanitizer = 1; break;
		case 208: mtmp->egotype_badowner = 1; break;
		case 209:
		case 210:
		case 211: mtmp->egotype_bleeder = 1; break;
		case 212: mtmp->egotype_shanker = 1; break;
		case 213: mtmp->egotype_terrorizer = 1; break;
		case 214: mtmp->egotype_feminizer = 1; break;
		case 215: mtmp->egotype_levitator = 1; break;
		case 216: mtmp->egotype_illusionator = 1; break;
		case 217: mtmp->egotype_stealer = 1; break;
		case 218: mtmp->egotype_stoner = 1; break;
		case 219: mtmp->egotype_maecke = 1; break;
		case 220:
		case 221: mtmp->egotype_flamer = 1; break;
		case 222: mtmp->egotype_blasphemer = 1; break;
		case 223: mtmp->egotype_dropper = 1; break;

	}

}

/* Make a negatively enchanted or cursed item and automatically equip it for the player --Amy
 * Please ensure that this effect isn't too common, because it might also result in good stuff
 * eqflags controls a couple things:
 * 0 = item's enchantment is always negative
 * 1 = item's enchantment is randomized
 */
void
bad_equipment(eqflags)
int eqflags;
{
	register struct obj *otmp;
	int objtyp = 0;
	int tryct = 0;

newbadtry:
	objtyp = rn2(NUM_OBJECTS);

	if (objtyp == AMULET_OF_STRANGULATION) goto newbadtry;
	/* too evil, as it means you either have a way of uncursing it or are dead, which isn't what I consider "fun" --Amy */

	if (objects[objtyp].oc_prob < 1) {
		tryct++;
		if (tryct < 5000) goto newbadtry;
		else return;
	}

	/* no gems, chains or iron balls --Amy */
	if (!(objects[objtyp].oc_class == WEAPON_CLASS || is_weptoolbase(objtyp) || is_blindfoldbase(objtyp) || objects[objtyp].oc_class == RING_CLASS || objects[objtyp].oc_class == AMULET_CLASS || objects[objtyp].oc_class == IMPLANT_CLASS || objects[objtyp].oc_class == ARMOR_CLASS) ) {
		tryct++;
		if (tryct < 5000) goto newbadtry;
		else return;
	}

	/* we should have an eligible item now (if not, the above code called return) */
	otmp = mksobj(objtyp, TRUE, FALSE, FALSE);
	if (!otmp) return; /* fail safe */

	if (objects[otmp->otyp].oc_charged && eqflags == 0) {
		if (otmp->spe > 0) otmp->spe *= -1;
		if (otmp->spe == 0) otmp->spe = -rne(Race_if(PM_LISTENER) ? 3 : 2);
	}

	if (otmp) {
		(void) pickup_object(otmp, otmp->quan, TRUE, TRUE);
	}

	/* try to equip it! */

	if (otmp) {

		u.cnd_badequipcount++;
		if (otmp->oclass == WEAPON_CLASS || is_weptool(otmp)) {
			if (uwep) setnotworn(uwep);
			if (bimanual(otmp)) {
				if (uswapwep) uswapwepgone();
				if (uarms) remove_worn_item(uarms, TRUE);
			}
			if (!uwep) setuwep(otmp, FALSE, TRUE);
			if (otmp) curse(otmp);
		}

		else if (is_blindfold_slot(otmp)) {
			if (ublindf) remove_worn_item(ublindf, TRUE);
			Blindf_on(otmp);
			if (otmp) curse(otmp);
		}

		else if (otmp->oclass == RING_CLASS) {
			boolean righthand = rn2(2);
			if (righthand) {
				if (uright) remove_worn_item(uright, TRUE);
				setworn(otmp, RIGHT_RING);
				Ring_on(otmp);
				if (otmp) curse(otmp);
			} else {
				if (uleft) remove_worn_item(uleft, TRUE);
				setworn(otmp, LEFT_RING);
				Ring_on(otmp);
				if (otmp) curse(otmp);
			}
		}

		else if (otmp->oclass == AMULET_CLASS) {
			if (uamul) remove_worn_item(uamul, TRUE);
			setworn(otmp, W_AMUL);
			Amulet_on();
			if (otmp) curse(otmp);
		}

		else if (otmp->oclass == IMPLANT_CLASS) {
			if (uimplant) remove_worn_item(uimplant, TRUE);
			setworn(otmp, W_IMPLANT);
			Implant_on();
			if (otmp) curse(otmp);
		}

		else if (is_boots(otmp) && !(Race_if(PM_ELONA_SNAIL) && !ishighheeled(otmp)) ) {
			if (uarmf) remove_worn_item(uarmf, TRUE);
			setworn(otmp, W_ARMF);
			Boots_on();
			if (otmp) curse(otmp);
		}

		else if (is_gloves(otmp)) {
			if (uarmg) remove_worn_item(uarmg, TRUE);
			setworn(otmp, W_ARMG);
			Gloves_on();
			if (otmp) curse(otmp);
		}

		else if (is_helmet(otmp)) {
			if (uarmh) remove_worn_item(uarmh, TRUE);
			setworn(otmp, W_ARMH);
			Helmet_on();
			if (otmp) curse(otmp);
		}

		else if (is_cloak(otmp)) {
			if (uarmc) remove_worn_item(uarmc, TRUE);
			setworn(otmp, W_ARMC);
			Cloak_on();
			if (otmp) curse(otmp);
		}

		else if (is_shield(otmp)) {
			if (uarms) remove_worn_item(uarms, TRUE);
			setworn(otmp, W_ARMS);
			Shield_on();
			if (otmp) curse(otmp);
		}

		else if (is_shirt(otmp)) {
			if (uarmu) remove_worn_item(uarmu, TRUE);
			setworn(otmp, W_ARMU);
			Shirt_on();
			if (otmp) curse(otmp);
		}

		else if (is_suit(otmp)) {
			if (uskin) skinback(FALSE);
			if (uarm) remove_worn_item(uarm, TRUE);
			setworn(otmp, W_ARM);
			Armor_on();
			if (otmp) curse(otmp);
		}

	}

}

/* Make a pair of cursed heels and force-equip it, suggested by Malena */
void
bad_equipment_heel()
{
	register struct obj *otmp;
	int objtyp = 0;
	int tryct = 0;

newbadheeltry:
	objtyp = rn2(NUM_OBJECTS);
	if (objects[objtyp].oc_prob < 1) {
		tryct++;
		if (tryct < 50000) goto newbadheeltry;
		else return;
	}

	/* no gems, chains or iron balls --Amy */
	if (objects[objtyp].oc_class != ARMOR_CLASS || !ishighheeledb(objtyp) ) {
		tryct++;
		if (tryct < 50000) goto newbadheeltry;
		else return;
	}

	/* we should have an eligible item now (if not, the above code called return) */
	otmp = mksobj(objtyp, TRUE, FALSE, FALSE);
	if (!otmp) return; /* fail safe */

	if (objects[otmp->otyp].oc_charged) {
		if (otmp->spe > 0) otmp->spe *= -1;
		if (otmp->spe == 0) otmp->spe = -rne(Race_if(PM_LISTENER) ? 3 : 2);
	}

	if (otmp) {
		(void) pickup_object(otmp, otmp->quan, TRUE, TRUE);
	}

	/* try to equip it! */

	if (otmp) {

		u.cnd_badheelcount++;

		if (is_boots(otmp)) {
			if (uarmf) remove_worn_item(uarmf, TRUE);
			setworn(otmp, W_ARMF);
			Boots_on();
			if (otmp) curse(otmp);
		} else {
			impossible("bad_equipment_heel() made non-boot item");
			return;
		}

	}

}

/* climacterial role: equip random feminism shoes */
void
bad_equipment_femshoes()
{
	register struct obj *otmp;
	int objtyp = 0;
	int tryct = 0;

	/* this depends on objects.c listing them in the correct order */
	objtyp = rnd_class(FEMMY_STILETTO_BOOTS, VERENA_STILETTO_SANDALS);

	/* we should have an eligible item now */
	otmp = mksobj(objtyp, TRUE, FALSE, FALSE);
	if (!otmp) return; /* fail safe */

	if (otmp) {
		(void) pickup_object(otmp, otmp->quan, TRUE, TRUE);
	}

	/* try to equip it! */

	if (otmp) {

		u.cnd_badheelcount++;

		if (is_boots(otmp)) {
			if (uarmf) remove_worn_item(uarmf, TRUE);
			setworn(otmp, W_ARMF);
			Boots_on();
			if (otmp) curse(otmp);
		} else {
			impossible("bad_equipment_femshoes() made non-boot item");
			return;
		}

	}

}


void
bad_equipment_implant()
{
	register struct obj *otmp;
	int objtyp = 0;
	int tryct = 0;

	/* this depends on objects.c listing them in the correct order */
	objtyp = rnd_class(IMPLANT_OF_ABSORPTION, IMPLANT_OF_ENFORCING);

	/* we should have an eligible item now */
	otmp = mksobj(objtyp, TRUE, FALSE, FALSE);
	if (!otmp) return; /* fail safe */

	if (otmp) {
		(void) pickup_object(otmp, otmp->quan, TRUE, TRUE);
	}

	/* try to equip it! */

	if (otmp) {

		if (otmp->oclass == IMPLANT_CLASS) {
			/* climacterial role calls this only if uimplant is not set, but better safe than sorry... */
			if (uimplant) remove_worn_item(uimplant, TRUE);
			setworn(otmp, W_IMPLANT);
			Implant_on();
			if (otmp) curse(otmp);
		} else {
			impossible("bad_equipment_implant() made non-implant item");
			return;
		}

	}

}

/* shanking: Unequip a random item that the player is wearing --Amy */
void
shank_player()
{
	int tryct = 0; /* roll repeatedly until we find something that can be removed */

	/* this attack is supposed to be nasty, not helpful. So we'll make sure that cursed items don't come off. */
newshank:
	switch (rnd(13)) {
		case 1:
			if (uwep && !uwep->cursed) {
				setnotworn(uwep);
				Your("weapon is unequipped!");
				if (uswapwep) {
					uswapwepgone();
					pline("And so is your secondary weapon, for that matter.");
				}
				return;
			}
			break;
		case 2:
			if (ublindf && !ublindf->cursed) {
				remove_worn_item(ublindf, TRUE);
				Your("blindfold or other tool is unequipped!");
				return;
			}
			break;
		case 3:
			if (uright && !uright->cursed) {
				remove_worn_item(uright, TRUE);
				Your("right ring is unequipped!");
				return;
			}
			break;
		case 4:
			if (uleft && !uleft->cursed) {
				remove_worn_item(uleft, TRUE);
				Your("left ring is unequipped!");
				return;
			}
			break;
		case 5:
			if (uamul && !uamul->cursed) {
				remove_worn_item(uamul, TRUE);
				Your("amulet is unequipped!");
				return;
			}
			break;
		case 6:
			if (uimplant && !uimplant->cursed) {
				remove_worn_item(uimplant, TRUE);
				Your("implant is unequipped!");
				/* yes I know, this is an easy way to get rid of an unwanted implant --Amy */
				return;
			}
			break;
		case 7:
			if (uarmf && !uarmf->cursed) {
				remove_worn_item(uarmf, TRUE);
				Your("pair of boots is unequipped!");
				return;
			}
			break;
		case 8:
			if (uarmg && !uarmg->cursed) {
				remove_worn_item(uarmg, TRUE);
				Your("pair of gloves is unequipped!");
				return;
			}
			break;
		case 9:
			if (uarmh && !uarmh->cursed) {
				remove_worn_item(uarmh, TRUE);
				Your("helmet is unequipped!");
				return;
			}
			break;
		case 10:
			if (uarmc && !uarmc->cursed) {
				remove_worn_item(uarmc, TRUE);
				Your("cloak is unequipped!");
				return;
			}
			break;
		case 11:
			if (uarms && !uarms->cursed) {
				remove_worn_item(uarms, TRUE);
				Your("shield is unequipped!");
				return;
			}
			break;
		case 12:
			if (uarmu && !uarmu->cursed) {
				remove_worn_item(uarmu, TRUE);
				Your("shirt is unequipped!");
				return;
			}
			break;
		case 13:
			if (uarm && !uarm->cursed) {
				remove_worn_item(uarm, TRUE);
				Your("suit is unequipped!");
				return;
			}
			break;
	}
	if (tryct > 1000) return; /* we didn't find any eligible item */
	tryct++;
	goto newshank;
}

void
terrainterror()
{
	int terrtype = randomwalltype();
	int tryct = 0;

	int chaosx, chaosy;
	while (tryct < 20000) {
		chaosx = rn1(COLNO-3,2);
		chaosy = rn2(ROWNO);
		if (chaosx && chaosy && isok(chaosx, chaosy) && (levl[chaosx][chaosy].typ == ROOM || levl[chaosx][chaosy].typ == CORR) ) {
			levl[chaosx][chaosy].typ = terrtype;
			tryct = 100000;
		}
		tryct++;
	}

	if (!rn2(20)) {
		u.aggravation = 1;
		if (!rn2(10)) u.heavyaggravation = 1;
		if (!rn2(10)) HighlevelStatus += 1;
		reset_rndmonst(NON_PM);

		coord dd;
		int cx,cy;
		int randsp = rnd(5);
		int monstercolor, randmnst, randmnsx, i;
		struct permonst *randmonstforspawn;

		cx = rn2(COLNO);
		cy = rn2(ROWNO);

		if (!rn2(4)) {

			randmnst = (rn2(187) + 1);
			randmnsx = (rn2(100) + 1);

			for (i = 0; i < randsp; i++) {

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

			randmonstforspawn = rndmonst();

			for (i = 0; i < randsp; i++) {

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(randmonstforspawn, cx, cy, MM_ADJACENTOK);
			}

		}

		else if (!rn2(2)) {

			monstercolor = rnd(15);
			do { monstercolor = rnd(15); } while (monstercolor == CLR_BLUE);

			for (i = 0; i < randsp; i++) {

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(colormon(monstercolor), cx, cy, MM_ADJACENTOK);
			}

		}

		else {

			monstercolor = rnd(379);

			for (i = 0; i < randsp; i++) {

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(monstercolor), cx, cy, MM_ADJACENTOK);
			}

		}

		u.aggravation = 0;
		u.heavyaggravation = 0;

	}

}

void
giftartifact()
{
	boolean havegiftsohgod = u.ugifts;
	register struct obj *acqo;

	if (!havegiftsohgod) u.ugifts++;

	acqo = mk_artifact((struct obj *)0, !rn2(3) ? A_CHAOTIC : rn2(2) ? A_NEUTRAL : A_LAWFUL, TRUE);
	if (acqo) {
		dropy(acqo);

		if (P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_ISRESTRICTED) {
		    unrestrict_weapon_skill(get_obj_skill(acqo, TRUE));
		} else if (P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_UNSKILLED) {
			unrestrict_weapon_skill(get_obj_skill(acqo, TRUE));
			P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_BASIC;
		} else if (rn2(2) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_BASIC) {
			P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_SKILLED;
		} else if (!rn2(4) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_SKILLED) {
			P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_EXPERT;
		} else if (!rn2(10) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_EXPERT) {
			P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_MASTER;
		} else if (!rn2(100) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_MASTER) {
			P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_GRAND_MASTER;
		} else if (!rn2(200) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_GRAND_MASTER) {
			P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_SUPREME_MASTER;
		}
		if (Race_if(PM_RUSMOT)) {
			if (P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_ISRESTRICTED) {
			    unrestrict_weapon_skill(get_obj_skill(acqo, TRUE));
			} else if (P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_UNSKILLED) {
				unrestrict_weapon_skill(get_obj_skill(acqo, TRUE));
				P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_BASIC;
			} else if (rn2(2) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_BASIC) {
				P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_SKILLED;
			} else if (!rn2(4) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_SKILLED) {
				P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_EXPERT;
			} else if (!rn2(10) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_EXPERT) {
				P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_MASTER;
			} else if (!rn2(100) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_MASTER) {
				P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_GRAND_MASTER;
			} else if (!rn2(200) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_GRAND_MASTER) {
				P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_SUPREME_MASTER;
			}
		}

		discover_artifact(acqo->oartifact);

		if (!havegiftsohgod) u.ugifts--;
		pline("An artifact appeared beneath you!");
	}
	else pline("Opportunity knocked, but nobody was home.  Bummer.");

}

void
heraldgift()
{
	You_feel("like someone has touched your forehead...");

	int skillimprove = randomgoodskill();

	if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
		unrestrict_weapon_skill(skillimprove);
		pline("You can now learn the %s skill.", wpskillname(skillimprove));
	} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
		unrestrict_weapon_skill(skillimprove);
		P_MAX_SKILL(skillimprove) = P_BASIC;
		pline("You can now learn the %s skill.", wpskillname(skillimprove));
	} else if (P_MAX_SKILL(skillimprove) == P_BASIC) {
		P_MAX_SKILL(skillimprove) = P_SKILLED;
		pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
	} else if (!rn2(2) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
		P_MAX_SKILL(skillimprove) = P_EXPERT;
		pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
	} else if (!rn2(3) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
		P_MAX_SKILL(skillimprove) = P_MASTER;
		pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
	} else if (!rn2(5) && P_MAX_SKILL(skillimprove) == P_MASTER) {
		P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
		pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
	} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_GRAND_MASTER) {
		P_MAX_SKILL(skillimprove) = P_SUPREME_MASTER;
		pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
	} else pline("Unfortunately, you feel no different than before.");

	if (Race_if(PM_RUSMOT)) {
		if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
			unrestrict_weapon_skill(skillimprove);
			pline("You can now learn the %s skill.", wpskillname(skillimprove));
		} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
			unrestrict_weapon_skill(skillimprove);
			P_MAX_SKILL(skillimprove) = P_BASIC;
			pline("You can now learn the %s skill.", wpskillname(skillimprove));
		} else if (P_MAX_SKILL(skillimprove) == P_BASIC) {
			P_MAX_SKILL(skillimprove) = P_SKILLED;
			pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
		} else if (!rn2(2) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
			P_MAX_SKILL(skillimprove) = P_EXPERT;
			pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
		} else if (!rn2(3) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
			P_MAX_SKILL(skillimprove) = P_MASTER;
			pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
		} else if (!rn2(5) && P_MAX_SKILL(skillimprove) == P_MASTER) {
			P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
			pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
		} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_GRAND_MASTER) {
			P_MAX_SKILL(skillimprove) = P_SUPREME_MASTER;
			pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
		} else pline("Unfortunately, you feel no different than before.");
	}

}

/* nastycurse: select a random worn piece of armor and curse it, plus add a nastytrap egotype to it --Amy */
void
nastytrapcurse()
{
	register struct obj *ntobj = (struct obj *)0;
	int tryct = 0;
	long savewornmask;

	while (tryct++ < 1000) {
		switch (rnd(7)) {
			case 1:
				if (uarmc) {
					tryct = 5000;
					ntobj = uarmc;
				}
				break;
			case 2:
				if (uarm) {
					tryct = 5000;
					ntobj = uarm;
				}
				break;
			case 3:
				if (uarmu) {
					tryct = 5000;
					ntobj = uarmu;
				}
				break;
			case 4:
				if (uarmh) {
					tryct = 5000;
					ntobj = uarmh;
				}
				break;
			case 5:
				if (uarms) {
					tryct = 5000;
					ntobj = uarms;
				}
				break;
			case 6:
				if (uarmf) {
					tryct = 5000;
					ntobj = uarmf;
				}
				break;
			case 7:
				if (uarmg) {
					tryct = 5000;
					ntobj = uarmg;
				}
				break;
		}
	}

	if (!ntobj) return; /* Nothing happens... */
	if (tryct >= 1000 && tryct < 5000) return; /* no object found */

	savewornmask = ntobj->owornmask;
	setworn((struct obj *)0, ntobj->owornmask);
	ntobj->enchantment = randnastyenchantment();
	setworn(ntobj, savewornmask);
	curse(ntobj);
	pline("You hear the devils laugh, and your %s is surrounded by an aura of evilness...", xname(ntobj) );

}

/* conundrum items can make your potions, scrolls etc. break less often --Amy
 * returns TRUE if item can break, otherwise FALSE */
boolean
conundrumbreak()
{
	if (uwep && objects[uwep->otyp].oc_material == MT_CONUNDRUM && !rn2(10)) {
		return FALSE;
	}
	if (u.twoweap && uswapwep && objects[uswapwep->otyp].oc_material == MT_CONUNDRUM && !rn2(10)) {
		return FALSE;
	}
	if (uarm && objects[uarm->otyp].oc_material == MT_CONUNDRUM && !rn2(10)) {
		return FALSE;
	}
	if (uarmc && objects[uarmc->otyp].oc_material == MT_CONUNDRUM && !rn2(10)) {
		return FALSE;
	}
	if (uarmh && objects[uarmh->otyp].oc_material == MT_CONUNDRUM && !rn2(10)) {
		return FALSE;
	}
	if (uarms && objects[uarms->otyp].oc_material == MT_CONUNDRUM && !rn2(10)) {
		return FALSE;
	}
	if (uarmg && objects[uarmg->otyp].oc_material == MT_CONUNDRUM && !rn2(10)) {
		return FALSE;
	}
	if (uarmf && objects[uarmf->otyp].oc_material == MT_CONUNDRUM && !rn2(10)) {
		return FALSE;
	}
	if (uarmu && objects[uarmu->otyp].oc_material == MT_CONUNDRUM && !rn2(10)) {
		return FALSE;
	}
	if (uamul && objects[uamul->otyp].oc_material == MT_CONUNDRUM && !rn2(10)) {
		return FALSE;
	}
	if (uimplant && objects[uimplant->otyp].oc_material == MT_CONUNDRUM && !rn2(10)) {
		return FALSE;
	}
	if (uleft && objects[uleft->otyp].oc_material == MT_CONUNDRUM && !rn2(10)) {
		return FALSE;
	}
	if (uright && objects[uright->otyp].oc_material == MT_CONUNDRUM && !rn2(10)) {
		return FALSE;
	}
	if (ublindf && objects[ublindf->otyp].oc_material == MT_CONUNDRUM && !rn2(10)) {
		return FALSE;
	}

	return TRUE;
}

/* sanity - yes it's not a bug that you start at 0 sanity and gradually become more sane :P --Amy */
void
increasesanity(snamount)
int snamount;
{
	if (Upolyd && dmgtype(youmonst.data, AD_SPC2) && rn2(5)) return;
	if (Upolyd && dmgtype(youmonst.data, AD_INSA) && rn2(100)) return;
	if (Upolyd && dmgtype(youmonst.data, AD_SANI) && rn2(100)) return;
	if (Race_if(PM_PLAYER_FAIRY) && rn2(2)) return;
	if (StrongHalluc_resistance && snamount < 3 && rn2(3)) return;

	/* Mineral is supposed to be a material that shields you against sanity --Amy */
	if (uwep && objects[uwep->otyp].oc_material == MT_MINERAL && !rn2(20)) {
		pline("Your mineral weapon prevents you from receiving sanity!");
		return;
	}
	if (u.twoweap && uswapwep && objects[uswapwep->otyp].oc_material == MT_MINERAL && !rn2(20)) {
		pline("Your mineral off-hand weapon prevents you from receiving sanity!");
		return;
	}
	if (uarm && objects[uarm->otyp].oc_material == MT_MINERAL && !rn2(20)) {
		pline("Your mineral armor prevents you from receiving sanity!");
		return;
	}
	if (uarmc && objects[uarmc->otyp].oc_material == MT_MINERAL && !rn2(20)) {
		pline("Your mineral cloak prevents you from receiving sanity!");
		return;
	}
	if (uarmh && objects[uarmh->otyp].oc_material == MT_MINERAL && !rn2(20)) {
		pline("Your mineral helmet prevents you from receiving sanity!");
		return;
	}
	if (uarms && objects[uarms->otyp].oc_material == MT_MINERAL && !rn2(20)) {
		pline("Your mineral shield prevents you from receiving sanity!");
		return;
	}
	if (uarmg && objects[uarmg->otyp].oc_material == MT_MINERAL && !rn2(20)) {
		pline("Your mineral pair of gauntlets prevents you from receiving sanity!");
		return;
	}
	if (uarmf && objects[uarmf->otyp].oc_material == MT_MINERAL && !rn2(20)) {
		pline("Your mineral pair of boots prevents you from receiving sanity!");
		return;
	}
	if (uarmu && objects[uarmu->otyp].oc_material == MT_MINERAL && !rn2(20)) {
		pline("Your mineral shirt prevents you from receiving sanity!");
		return;
	}
	if (uamul && objects[uamul->otyp].oc_material == MT_MINERAL && !rn2(20)) {
		pline("Your mineral amulet prevents you from receiving sanity!");
		return;
	}
	if (uimplant && objects[uimplant->otyp].oc_material == MT_MINERAL && !rn2(20)) {
		pline("Your mineral implant prevents you from receiving sanity!");
		return;
	}
	if (uleft && objects[uleft->otyp].oc_material == MT_MINERAL && !rn2(20)) {
		pline("Your mineral left ring prevents you from receiving sanity!");
		return;
	}
	if (uright && objects[uright->otyp].oc_material == MT_MINERAL && !rn2(20)) {
		pline("Your mineral right ring prevents you from receiving sanity!");
		return;
	}
	if (ublindf && objects[ublindf->otyp].oc_material == MT_MINERAL && !rn2(20)) {
		pline("Your mineral blindfold prevents you from receiving sanity!");
		return;
	}

	if (!(PlayerCannotUseSkills)) {
		int sanityprotection = 0;

		switch (P_SKILL(P_SHII_CHO)) {
			case P_BASIC:	sanityprotection +=  1; break;
			case P_SKILLED:	sanityprotection +=  2; break;
			case P_EXPERT:	sanityprotection +=  3; break;
			case P_MASTER:	sanityprotection +=  4; break;
			case P_GRAND_MASTER:	sanityprotection +=  5; break;
			case P_SUPREME_MASTER:	sanityprotection +=  6; break;
			default: sanityprotection += 0; break;
		}
		switch (P_SKILL(P_MAKASHI)) {
			case P_BASIC:	sanityprotection +=  1; break;
			case P_SKILLED:	sanityprotection +=  2; break;
			case P_EXPERT:	sanityprotection +=  3; break;
			case P_MASTER:	sanityprotection +=  4; break;
			case P_GRAND_MASTER:	sanityprotection +=  5; break;
			case P_SUPREME_MASTER:	sanityprotection +=  6; break;
			default: sanityprotection += 0; break;
		}
		switch (P_SKILL(P_SORESU)) {
			case P_BASIC:	sanityprotection +=  1; break;
			case P_SKILLED:	sanityprotection +=  2; break;
			case P_EXPERT:	sanityprotection +=  3; break;
			case P_MASTER:	sanityprotection +=  4; break;
			case P_GRAND_MASTER:	sanityprotection +=  5; break;
			case P_SUPREME_MASTER:	sanityprotection +=  6; break;
			default: sanityprotection += 0; break;
		}
		switch (P_SKILL(P_ATARU)) {
			case P_BASIC:	sanityprotection +=  1; break;
			case P_SKILLED:	sanityprotection +=  2; break;
			case P_EXPERT:	sanityprotection +=  3; break;
			case P_MASTER:	sanityprotection +=  4; break;
			case P_GRAND_MASTER:	sanityprotection +=  5; break;
			case P_SUPREME_MASTER:	sanityprotection +=  6; break;
			default: sanityprotection += 0; break;
		}
		switch (P_SKILL(P_SHIEN)) {
			case P_BASIC:	sanityprotection +=  1; break;
			case P_SKILLED:	sanityprotection +=  2; break;
			case P_EXPERT:	sanityprotection +=  3; break;
			case P_MASTER:	sanityprotection +=  4; break;
			case P_GRAND_MASTER:	sanityprotection +=  5; break;
			case P_SUPREME_MASTER:	sanityprotection +=  6; break;
			default: sanityprotection += 0; break;
		}
		switch (P_SKILL(P_DJEM_SO)) {
			case P_BASIC:	sanityprotection +=  1; break;
			case P_SKILLED:	sanityprotection +=  2; break;
			case P_EXPERT:	sanityprotection +=  3; break;
			case P_MASTER:	sanityprotection +=  4; break;
			case P_GRAND_MASTER:	sanityprotection +=  5; break;
			case P_SUPREME_MASTER:	sanityprotection +=  6; break;
			default: sanityprotection += 0; break;
		}
		switch (P_SKILL(P_NIMAN)) {
			case P_BASIC:	sanityprotection +=  1; break;
			case P_SKILLED:	sanityprotection +=  2; break;
			case P_EXPERT:	sanityprotection +=  3; break;
			case P_MASTER:	sanityprotection +=  4; break;
			case P_GRAND_MASTER:	sanityprotection +=  5; break;
			case P_SUPREME_MASTER:	sanityprotection +=  6; break;
			default: sanityprotection += 0; break;
		}
		switch (P_SKILL(P_JUYO)) {
			case P_BASIC:	sanityprotection +=  1; break;
			case P_SKILLED:	sanityprotection +=  2; break;
			case P_EXPERT:	sanityprotection +=  3; break;
			case P_MASTER:	sanityprotection +=  4; break;
			case P_GRAND_MASTER:	sanityprotection +=  5; break;
			case P_SUPREME_MASTER:	sanityprotection +=  6; break;
			default: sanityprotection += 0; break;
		}
		switch (P_SKILL(P_VAAPAD)) {
			case P_BASIC:	sanityprotection +=  1; break;
			case P_SKILLED:	sanityprotection +=  2; break;
			case P_EXPERT:	sanityprotection +=  3; break;
			case P_MASTER:	sanityprotection +=  4; break;
			case P_GRAND_MASTER:	sanityprotection +=  5; break;
			case P_SUPREME_MASTER:	sanityprotection +=  6; break;
			default: sanityprotection += 0; break;
		}
		switch (P_SKILL(P_WEDI)) {
			case P_BASIC:	sanityprotection +=  1; break;
			case P_SKILLED:	sanityprotection +=  2; break;
			case P_EXPERT:	sanityprotection +=  3; break;
			case P_MASTER:	sanityprotection +=  4; break;
			case P_GRAND_MASTER:	sanityprotection +=  5; break;
			case P_SUPREME_MASTER:	sanityprotection +=  6; break;
			default: sanityprotection += 0; break;
		}
		if (sanityprotection > rn2(100)) return;
	}

	if (StrongHalluc_resistance && snamount > 2) snamount /= 3;

	if (Race_if(PM_HUMANOID_ANGEL) || youmonst.data->mlet == S_ANGEL) snamount *= 2;

	if (YouGetLotsOfSanity) snamount *= rnd(20);

	u.usanity += snamount;
	if (snamount < 10) pline("Your sanity increases.");
	else if (snamount < 100) pline("Your sanity increases greatly.");
	else if (snamount < 1000) pline("Your sanity increases tremendously.");
	else pline("Your sanity increases gigantically.");

	if (u.usanity > 900) {
		if (u.usanity <= 9000) You("begin to view the world as the terrible place it really is.");
		/* there should be some effect here but hell if I remember what it was supposed to be! */
	}

	if (u.usanity > 9000) {
		You("realize in shock that this dungeon is a truly atrocious place.");
		if (multi >= 0) {
			flags.soundok = 0;
			nomul(-(rnd(4 + ((u.usanity / 1000) - 8) ) ), "paralyzed by sanity", TRUE);
			nomovemsg = "You regain consciousness.";
		}
	}

	if (u.usanity > 90000) {
		pline("In fact, you're certain that the apocalypse is near as there's murder and pillaging taking place everywhere.");
	}

	if (rn2(10) && (u.usanity > rn2(1000)) ) {
		if (u.usanity > 90000) reallybadeffect();
		else badeffect();
	}

}

int
dodrink()
{
	register struct obj *otmp;
	char quaffables[SIZE(beverages) + 2];
	char *qp = quaffables;

	if (Strangled) {
		pline(FunnyHallu ? "You don't wanna do booze right now." : "If you can't breathe air, how can you drink liquid?");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	}
	if (uarmh && (uarmh->otyp == PLASTEEL_HELM || uarmh->otyp == HELM_OF_STORMS || uarmh->otyp == HELM_OF_DETECT_MONSTERS) ){
		pline("The %s covers your whole face.", xname(uarmh));
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	}

	*qp++ = ALLOW_FLOOROBJ;
	if (!u.uswallow && (IS_FOUNTAIN(levl[u.ux][u.uy].typ) ||
			    IS_SINK(levl[u.ux][u.uy].typ) ||
			    IS_TOILET(levl[u.ux][u.uy].typ) ||
			    IS_WELL(levl[u.ux][u.uy].typ) ||
			    IS_POISONEDWELL(levl[u.ux][u.uy].typ) ||
			    Underwater || (IS_POOL(levl[u.ux][u.uy].typ) && !(IS_CRYSTALWATER(levl[u.ux][u.uy].typ))) ))
	    *qp++ = ALLOW_THISPLACE;
	strcpy(qp, beverages);

	otmp = getobj(quaffables, "drink");
	if (otmp == &thisplace) {
	    if (IS_FOUNTAIN(levl[u.ux][u.uy].typ)) {
		drinkfountain();
		if (u.uprops[DEHYDRATION].extrinsic || Dehydration || have_dehydratingstone() ) {
			u.dehydrationtime = moves + 1001;
		}

		if (uarmh && itemhasappearance(uarmh, APP_WATER_PIPE_HELMET) ) {
			morehungry(-10);
		}

		if (RngeLiquidDiet) {
			morehungry(-10);
		}

		return 1;
	    }
	    else if (IS_WELL(levl[u.ux][u.uy].typ)) {

		You("draw water from a well.");
		u.cnd_wellamount++;
		if (u.ualign.type == A_NEUTRAL) adjalign(1);

		if (levl[u.ux][u.uy].blessedftn == 1) {
			pline("Wow, the water was magical!");
			levl[u.ux][u.uy].blessedftn = 0;
			int i, ii, lim;
			i = rn2(A_MAX);
			for (ii = 0; ii < A_MAX; ii++) {
				lim = AMAX(i);
				if (i == A_STR && u.uhs >= 3) --lim;	/* WEAK */
				if (ABASE(i) < lim) {
					ABASE(i) = lim;
					flags.botl = 1;
					break;
				}
				if(++i >= A_MAX) i = 0;
			}
			int stattoincrease = rn2(A_MAX);
			adjattrib(stattoincrease, 1, 0, TRUE);
			return 1;
		}

		if (level.flags.lethe) {
			pline("Whoops, you forgot that it contains lethe water.");
			if (FunnyHallu) pline("You also forgot whether lethe water can cause amnesia.");
			forget(ALL_SPELLS | ALL_MAP);
		}

		morehungry(-50);
		if (RngeLiquidDiet) {
			morehungry(-10);
		}
		reducesanity(10);
		healup(d(2,6) + rnz(u.ulevel), 0, FALSE, FALSE);

		{
			int i, ii, lim;

			i = rn2(A_MAX);		/* start at a random point */
			for (ii = 0; ii < A_MAX; ii++) {
				lim = AMAX(i);
				if (i == A_STR && u.uhs >= 3) --lim;	/* WEAK */
				if (ABASE(i) < lim) {
					if (rn2(10)) {
						ABASE(i)++;
						pline("Wow! This makes you feel good!");
					} else {
						AMAX(i) -= 1;
						pline("Oh no, apparently one of your stats was permanently not restored!");
					}
					flags.botl = 1;
					break; /* only restore one --Amy */
				}
				if(++i >= A_MAX) i = 0;
			}
		}

		if (!rn2(isfriday ? 5 : 10)) {
			levl[u.ux][u.uy].typ = POISONEDWELL;
			pline("Suddenly the well becomes poisoned...");
		} else if (!rn2(100)) {
			levl[u.ux][u.uy].typ = CORR;
			pline("The well dries up!");
		}
		more_experienced(1 * (deepest_lev_reached(FALSE) + 1), 0);
		newexplevel();
		upnivel(FALSE);
		return 1;
	    }
	    else if (IS_POISONEDWELL(levl[u.ux][u.uy].typ)) {
		You("draw water from a well.");
		u.cnd_poisonedwellamount++;
		if (u.ualign.type == A_NEUTRAL) adjalign(1);

		if (level.flags.lethe) {
			pline("Whoops, you forgot that it contains lethe water.");
			if (FunnyHallu) pline("You also forgot whether lethe water can cause amnesia.");
			forget(ALL_SPELLS | ALL_MAP);
		}

		morehungry(-50);
		if (RngeLiquidDiet) {
			morehungry(-10);
		}
		pline(FunnyHallu ? "Urgh - that tastes like cactus juice with full-length thorns in it!" : "Ecch - that must have been poisonous!");
		if(!Poison_resistance) {
		    losestr(rnd(4), TRUE);
		    losehp(rnd(15), "quaffing from a poisoned well", KILLED_BY);
		} else You("resist the effects but still don't feel so good.");
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_STR, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_DEX, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_CON, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_INT, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_WIS, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_CHA, -rnd(2), FALSE, TRUE);
		poisoned("The water", rn2(A_MAX), "poisoned well", 30);
		if ((u.ulycn != -1) && !rn2(10)) {
			you_unwere(TRUE);
		}
		
		if (!rn2(20)) {
			levl[u.ux][u.uy].typ = CORR;
			pline("The well dries up!");
		}
		more_experienced(5 * (deepest_lev_reached(FALSE) + 1), 0);
		newexplevel();
		return 1;

	    }
	    else if (IS_SINK(levl[u.ux][u.uy].typ)) {
		drinksink();
		if (u.uprops[DEHYDRATION].extrinsic || Dehydration || have_dehydratingstone() ) {
			u.dehydrationtime = moves + 1001;
		}

		if (uarmh && itemhasappearance(uarmh, APP_WATER_PIPE_HELMET) ) {
			morehungry(-10);
		}

		if (RngeLiquidDiet) {
			morehungry(-10);
		}

		return 1;
	    }
	    else if (IS_TOILET(levl[u.ux][u.uy].typ)) {
		drinktoilet();
		if (u.uprops[DEHYDRATION].extrinsic || Dehydration || have_dehydratingstone() ) {
			u.dehydrationtime = moves + 1001;
		}

		if (uarmh && itemhasappearance(uarmh, APP_WATER_PIPE_HELMET) ) {
			morehungry(-10);
		}

		if (RngeLiquidDiet) {
			morehungry(-10);
		}

		return 1;
	    }
	    pline(FunnyHallu ? "This water seems especially clean. In fact, it's the cleanest water you've ever seen." : "Do you know what lives in this water!");

		if (level.flags.lethe) {

			pline("In any case, you apparently forgot that it causes amnesia.");
			if (FunnyHallu) pline("You also forgot about Maud.");
			forget(rnd(10));

		}

		if (u.uprops[DEHYDRATION].extrinsic || Dehydration || have_dehydratingstone() ) {
			u.dehydrationtime = moves + 1001;
		}

		if (uarmh && itemhasappearance(uarmh, APP_WATER_PIPE_HELMET) ) {
			morehungry(-10);
		}

		if (RngeLiquidDiet) {
			morehungry(-10);
		}

	    return 1;
	}
	if(!otmp) return(0);

	if (InterruptEffect || u.uprops[INTERRUPT_EFFECT].extrinsic || have_interruptionstone()) {
		nomul(-(rnd(5)), "quaffing a potion", TRUE);
	}

	otmp->in_use = TRUE;		/* you've opened the stopper */

#define POTION_OCCUPANT_CHANCE(n) (13 + 2*(n))	/* also in muse.c */

	u.cnd_quaffcount++;

	use_skill(P_DEVICES,1);
	if (Race_if(PM_FAWN)) {
		use_skill(P_DEVICES,1);
	}
	if (Race_if(PM_SATRE)) {
		use_skill(P_DEVICES,1);
		use_skill(P_DEVICES,1);
	}

	if (itemhasappearance(otmp, APP_POTION_MILKY) || itemhasappearance(otmp, APP_POTION_GHOSTLY) || itemhasappearance(otmp, APP_POTION_HALLOWED) || itemhasappearance(otmp, APP_POTION_CAMPING) || itemhasappearance(otmp, APP_POTION_SPIRITUAL)) {
		if (flags.ghost_count < MAXMONNO && !rn2(POTION_OCCUPANT_CHANCE(flags.ghost_count))) {
			ghost_from_bottle();
			if (carried(otmp)) useup(otmp);
			else useupf(otmp, 1L);
			return(1);
		}
	}

	if (itemhasappearance(otmp, APP_POTION_SMOKY) &&
		    (flags.djinni_count < MAXMONNO) && !rn2(POTION_OCCUPANT_CHANCE(flags.djinni_count))) {
		djinni_from_bottle(otmp, 1);
		if (carried(otmp)) useup(otmp);
		else useupf(otmp, 1L);
		return(1);
	}
	if (itemhasappearance(otmp, APP_POTION_VAPOR) &&
		    (flags.dao_count < MAXMONNO) && !rn2(POTION_OCCUPANT_CHANCE(flags.dao_count))) {
		djinni_from_bottle(otmp, 2);
		if (carried(otmp)) useup(otmp);
		else useupf(otmp, 1L);
		return(1);
	}
	if (itemhasappearance(otmp, APP_POTION_FUMING) &&
		    (flags.efreeti_count < MAXMONNO) && !rn2(POTION_OCCUPANT_CHANCE(flags.efreeti_count))) {
		djinni_from_bottle(otmp, 3);
		if (carried(otmp)) useup(otmp);
		else useupf(otmp, 1L);
		return(1);
	}
	if (itemhasappearance(otmp, APP_POTION_SIZZLING) &&
		    (flags.marid_count < MAXMONNO) && !rn2(POTION_OCCUPANT_CHANCE(flags.marid_count))) {
		djinni_from_bottle(otmp, 4);
		if (carried(otmp)) useup(otmp);
		else useupf(otmp, 1L);
		return(1);
	}
	if (itemhasappearance(otmp, APP_POTION_WHISKY) &&
		    (flags.wineghost_count < MAXMONNO) && !rn2(POTION_OCCUPANT_CHANCE(flags.wineghost_count))) {
		djinni_from_bottle(otmp, 5);
		if (carried(otmp)) useup(otmp);
		else useupf(otmp, 1L);
		return(1);
	}
	if (itemhasappearance(otmp, APP_POTION_DIMLY_SHINING) && !rn2(10)) {

		int summondemon = ndemon(A_CHAOTIC);
		if (summondemon == NON_PM) {
			pline("Somehow, the potion evaporates with no effect.");
			return(1);
		}

		pline("%s", Blind ? "You hear an evil chuckle!" : "A miasma of stinking vapors coalesces around you!");
		make_pet_minion(summondemon, A_CHAOTIC);
		godvoice(A_CHAOTIC, "My minion shall serve thee!");

		if (carried(otmp)) useup(otmp);
		else useupf(otmp, 1L);
		return(1);
	}
	if (itemhasappearance(otmp, APP_POTION_GASEOUS) && !rn2(10)) {
		int summondemon = ntrminion();
		if (summondemon == NON_PM) {
			pline("Somehow, the potion evaporates with no effect.");
			return(1);
		}

		pline("%s", Blind ? "You hear the earth rumble..." : "A cloud of gray smoke gathers around you!");
		make_pet_minion(summondemon, A_NEUTRAL);
		godvoice(A_NEUTRAL, "My minion shall serve thee!");

		if (carried(otmp)) useup(otmp);
		else useupf(otmp, 1L);
		return(1);
	}
	if (itemhasappearance(otmp, APP_POTION_STARLIGHT) && !rn2(10)) {
		int summondemon = lminion();
		if (summondemon == NON_PM) {
			pline("Somehow, the potion evaporates with no effect.");
			return(1);
		}

		pline("%s", Blind ? "You feel the presence of goodness." : "There is a puff of white fog!");
		make_pet_minion(summondemon, A_LAWFUL);
		godvoice(A_LAWFUL, "My minion shall serve thee!");

		if (carried(otmp)) useup(otmp);
		else useupf(otmp, 1L);
		return(1);
	}

	if (itemhasappearance(otmp, APP_POTION_ENDBRINGER) && !rn2(64)) {
		ragnarok(TRUE);
		if (evilfriday) evilragnarok(TRUE,level_difficulty());
	}
	if (itemhasappearance(otmp, APP_POTION_DEADWEIGHT) && !rn2(10)) {
		pline("Some sinister force causes you to wear an item!");
		bad_equipment(0);
	}
	if (itemhasappearance(otmp, APP_POTION_PRESENT) && !rn2(10)) {
		pline("Some sinister force causes you to wear an artifact!");
		bad_artifact();
	}
	if (itemhasappearance(otmp, APP_POTION_MALEEN) && !rn2(10)) {
		pline("Some sinister force causes you to wear a pair of heels!");
		bad_equipment_heel();
	}
	if (itemhasappearance(otmp, APP_POTION_GLOSS) && !rn2(10)) {
		You("apply the lovely lip gloss that was contained inside.");
		(void) adjattrib(A_CHA, 1, FALSE, TRUE);
	}
	if (itemhasappearance(otmp, APP_POTION_GLAM)) {
		if (u.nailpolish < 10) {
			u.nailpolish++;
			pline("The potion contained nail polish! You use it to dye your nails.");
		} else pline("Sadly, the nail polish in the potion goes to waste since you've painted all your nails already.");
	}

	if (itemhasappearance(otmp, APP_POTION_RESERVATROL)) {
		(void) create_gas_cloud(u.ux, u.uy, 3+bcsign(otmp), 8+4*bcsign(otmp));
		You("smell chemicals.");
	}
	if (itemhasappearance(otmp, APP_POTION_SYMBIO) && !rn2(5)) {
		struct permonst *pm = 0;
		int attempts = 0;
		register struct monst *symbiomon;

newsymbio:
		do {
			pm = rndmonst();
			attempts++;

		} while ( (!pm || (pm && !(stationary(pm) || pm->mmove == 0 || pm->mlet == S_TURRET ))) && attempts < 50000);

		if (!pm && rn2(50) ) {
			attempts = 0;
			goto newsymbio;
		}
		if (pm && !(stationary(pm) || pm->mmove == 0 || pm->mlet == S_TURRET) && rn2(50) ) {
			attempts = 0;
			goto newsymbio;
		}

		if (pm) symbiomon = makemon(pm, u.ux, u.uy, NO_MM_FLAGS);
		if (symbiomon) {
			(void) tamedog(symbiomon, (struct obj *) 0, TRUE);
			pline("A potential symbiote forms out of nowhere!");
		}
	}
	if (itemhasappearance(otmp, APP_POTION_MICROBIOTIC) && !rn2(5)) {
		register struct monst *mtmp;
 	      mtmp = makemon(mkclass(S_FUNGUS,0), u.ux, u.uy, NO_MM_FLAGS);
		if (mtmp) {
			(void) tamedog(mtmp, (struct obj *) 0, TRUE);
			pline("A fungus forms out of nowhere!");
		}
	}
	if (itemhasappearance(otmp, APP_POTION_NITROGLYCERIN)) {
		struct obj *dynamite;
		dynamite = mksobj_at(STICK_OF_DYNAMITE, u.ux, u.uy, TRUE, FALSE, FALSE);
		if (dynamite) {
			if (dynamite->otyp != STICK_OF_DYNAMITE) delobj(dynamite);
			else {
				dynamite->dynamitekaboom = 1;
				dynamite->quan = 1;
				dynamite->owt = weight(dynamite);
				attach_bomb_blow_timeout(dynamite, 0, 0);
			}
		}
	}
	if (itemhasappearance(otmp, APP_POTION_PERFUME)) {
		int mondistance = 0;
		struct monst *mtmp3;
		int k, l;
		for (k = -5; k <= 5; k++) for(l = -5; l <= 5; l++) {
			if (!isok(u.ux + k, u.uy + l)) continue;

			mondistance = 1;
			if (k > 1) mondistance = k;
			if (k < -1) mondistance = -k;
			if (l > 1 && l > mondistance) mondistance = l;
			if (l < -1 && (-l > mondistance)) mondistance = -l;

			if ( (mtmp3 = m_at(u.ux + k, u.uy + l)) != 0) {
				mtmp3->mcanmove = 0;
				mtmp3->mfrozen = (rnd(16 - (mondistance * 2)));
				mtmp3->mstrategy &= ~STRAT_WAITFORU;
				mtmp3->mconf = TRUE;
				pline("%s becomes dizzy from the scent!", Monnam(mtmp3));
			}
		}

	}

	if (CurseAsYouUse && otmp && otmp->otyp != CANDELABRUM_OF_INVOCATION && otmp->otyp != SPE_BOOK_OF_THE_DEAD && otmp->otyp != BELL_OF_OPENING) curse(otmp);
	return dopotion(otmp);
}

int
dopotion(otmp)
register struct obj *otmp;
{
	int retval;

	otmp->in_use = TRUE;
	nothing = unkn = 0;

	if (u.uprops[DEHYDRATION].extrinsic || Dehydration || have_dehydratingstone() ) {
		u.dehydrationtime = moves + 1001;
	}

	if (uarmh && itemhasappearance(uarmh, APP_WATER_PIPE_HELMET) ) {
		morehungry(-10);
	}

	if (RngeLiquidDiet) {
		morehungry(-10);
	}

	if((retval = peffects(otmp)) >= 0) return(retval);

	if(nothing) {
	    unkn++;
	    You("have a %s feeling for a moment, then it passes.",
		  FunnyHallu ? "normal" : "peculiar");
	}
	if(otmp && otmp->dknown && !objects[otmp->otyp].oc_name_known) {
		if(!unkn) {
			makeknown(otmp->otyp);
			more_experienced(0,10);
		} else if(!objects[otmp->otyp].oc_uname)
			docall(otmp);
	}
	if (otmp && otmp->oartifact == ART_BERRYBREW) {
		make_bottle(TRUE);
	}
	if (otmp && carried(otmp)) {
		useup(otmp);
		make_bottle(FALSE);
	}
	else if (otmp && mcarried(otmp)) m_useup(otmp->ocarry, otmp);
	else if (otmp && otmp->where == OBJ_FLOOR) useupf(otmp, 1L);
	else if (otmp) dealloc_obj(otmp);		/* Dummy potion */

	if (uarmc && itemhasappearance(uarmc, APP_LEVUNTATION_CLOAK)) badeffect();

	return(1);
}

/* return -1 if potion is used up,  0 if error,  1 not used */
int
peffects(otmp)
	register struct obj	*otmp;
{
	register int i, ii, lim;

	if ( (DSTWProblem || u.uprops[DSTW_BUG].extrinsic || (uarmh && uarmh->oartifact == ART_UNIMPLEMENTED_FEATURE) || have_dstwstone() ) && !rn2(5)) {

		pline("The potion doesn't seem to work!"); /* DSTW = abbreviation for "doesn't seem to work" --Amy */
		u.cnd_nonworkpotioncount++;
		return(-1);

	}

	if (otmp->selfmade && !(5-rnl(6))) {
		pline("That potion was bad!");
		switch(rnl(5)) {
			case 0:
			case 1:
			case 2:
				break;

			case 3:
				if (Poison_resistance && (StrongPoison_resistance || rn2(10)) ) break;
				You_feel("sick.");
				losehp(rnd(20),"bad chemical knowledge",KILLED_BY);
				break;

			case 4:
				make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON),20),
			"bad potion", TRUE, SICK_VOMITABLE);
				break;
		}
		return(-1);
	}

	if (RngeLevuntation) badeffect();

	if (otmp->oartifact == ART_WONDER_WATERING_PLACE) {
		int coronatries = rnd(10);
		while (coronatries > 0) {
			coronatries--;
			upnivel(FALSE);
		}
	}

	if (otmp->otyp == POT_WONDER || otmp->otyp == POT_TERCES_DLU) {

		struct obj *wonderpot;
		wonderpot = mkobj(POTION_CLASS,FALSE, FALSE);
		if (wonderpot) otmp->otyp = wonderpot->otyp;
		if (otmp->otyp == GOLD_PIECE) otmp->otyp = POT_WATER; /* minimalist fix */
		if (wonderpot) obfree(wonderpot, (struct obj *)0);
		unkn++;

	}

	/* KMH, balance patch -- this is too cruel for novices */
	/* sometimes your constitution can be a little _too_ high! */
	/* edit: evil variant by Amy: nothing is too cruel :P */
	if ( ((evilfriday && (Role_if(PM_BARBARIAN) || ACURR(A_CON) > 15)) || Role_if(PM_NOOB_MODE_BARB)) && !rn2(5)) {
		pline("Strange ...");
		nothing++;
		return(-1);
	}

	switch(otmp->otyp){
	case POT_RESTORE_ABILITY:
	case SPE_RESTORE_ABILITY:
		unkn++;
		if(otmp->cursed) {
		    pline("Ulch!  This makes you feel mediocre!");

		    if (evilfriday) {
			adjattrib(rn2(A_MAX), -1, 0, TRUE);
		    }

		    break;
		} else {
		    pline("Wow!  This makes you feel %s!",
			  (otmp->blessed) ?
				(unfixable_trouble_count(FALSE) ? "better" : "great")
			  : "good");
		    i = rn2(A_MAX);		/* start at a random point */
		    for (ii = 0; ii < A_MAX; ii++) {
			lim = AMAX(i);
			if (i == A_STR && u.uhs >= 3) --lim;	/* WEAK */
			if (ABASE(i) < lim) {
			    if (otmp->otyp == SPE_RESTORE_ABILITY) {
					if (rn2(8)) ABASE(i)++;
					else {
						pline("It actually didn't work though...");
						AMAX(i) -= 1;
					}
			    }
			    else ABASE(i) = lim;
			    flags.botl = 1;
			    /* only first found if not blessed */
			    if (!otmp->blessed) break;
			}
			if(++i >= A_MAX) i = 0;
		    }
		}
		break;
	case POT_HALLUCINATION:
            makeknown (POT_HALLUCINATION);
		if (Hallucination || Halluc_resistance) nothing++;
		else makeknown(otmp->otyp);
		(void) make_hallucinated(itimeout_incr(HHallucination,
					   rn1(200, 600 - 300 * bcsign(otmp))),
				  TRUE, 0L);
		break;
	case POT_ICE:
		if(!Frozen) {
		    if (FunnyHallu) {
			pline("Giant ice-cream cones... mmmmm!");
			unkn++;
		    } else
			pline("Suddenly, you're frozen solid!");
		} else	nothing++;

		make_frozen(itimeout_incr(HFrozen,
					    rn1(35, 80 - 25 * bcsign(otmp))), FALSE);

		break;
	case POT_FEAR:
		if(!Feared) {
		    if (FunnyHallu) {
			You_feel("like you're dying from the inside! Waaaaah! Where's my Mommy?");
			unkn++;
		    } else
			pline("Suddenly, you're trembling in fear!");
		} else	nothing++;

		make_feared(itimeout_incr(HFeared,
					    rn1(200, 600 - 300 * bcsign(otmp))), FALSE);

		break;
	case POT_FIRE:
		if(!Burned) {
		    if (FunnyHallu) {
			pline("Wow! You just swallowed a flamethrower - your tongue can spit fire! Yee-haw!");
			unkn++;
		    } else
			pline("Ooph! Concentrated habanero chili peppers!");
		} else	nothing++;

		make_burned(itimeout_incr(HBurned,
					    rn1(100, 100 - 25 * bcsign(otmp))), FALSE);

		break;
	case POT_CURE_INSANITY:
		if (otmp->cursed) {
			increasesanity(rnz(100));
			unkn++;
			break;
		}
		if (!u.usanity) {
			unkn++;
			break;
		}
		if (otmp->blessed) decontaminate(100);
		while (u.usanity > 0) reducesanity(9999999);
		if (u.usanity < 0) u.usanity = 0;
            makeknown(POT_CURE_INSANITY);
		break;

	case POT_DIMNESS:
		if(!Dimmed) {
		    if (FunnyHallu) {
			pline("It's antidepressiva - no, wait, it's the opposite of it... oh no! Your life is worthless and you want to jump off a bridge!");
			unkn++;
		    } else
			pline("This tastes like bitter medicine, and your senses are dulled!");
		} else	nothing++;

		make_dimmed(itimeout_incr(HDimmed,
					    rn1(100, 100 - 25 * bcsign(otmp))), FALSE);

		break;
	case POT_SANITY:
            makeknown(POT_SANITY);
		pline("UGH! This tastes incredibly repulsive...");
		increasesanity(rnz(100));
		break;
	case POT_STUNNING:
		if(!Stunned) {
		    if (FunnyHallu) {
			pline("You wobble around! How funny!");
			unkn++;
		    } else
			pline("You're staggering!");
		} else	nothing++;

		make_stunned(itimeout_incr(HStun,
					    rn1(35, 80 - 25 * bcsign(otmp))), FALSE);

		break;
	case POT_NUMBNESS:
		if(!Numbed) {
		    if (FunnyHallu) {
			pline("Oh! Some of your limbs seem to be talking to you!");
			unkn++;
		    } else
			pline("Your limbs start feeling numb!");
		} else	nothing++;

		make_numbed(itimeout_incr(HNumbed,
					    rn1(100, 200 - 75 * bcsign(otmp))), FALSE);
		break;

	case POT_CANCELLATION:

		    attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse();

		unkn++;
		break;

	case POT_SLIME:

		if (!Slimed && !flaming(youmonst.data) && !Unchanging && !slime_on_touch(youmonst.data) ) {
		    You("don't feel very well.");
			make_slimed(100);
		    killer_format = KILLED_BY_AN;
		    delayed_killer = "slimed by a potion of slime";
		}
		unkn++;

		break;

	case POT_URINE:

		pline("Eek! This tastes indescibably bad...");

		if ((monstermoves - otmp->age) < 51) {

		exercise(A_WIS, FALSE);
		make_vomiting(Vomiting + rnd(10) + 5,TRUE);
		if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
		}
		else {
			make_sick(Sick ? Sick/2L + 1L : 10, "urine potion", TRUE, SICK_VOMITABLE);
			if ((!StrongPoison_resistance || !rn2(3)) && (!Poison_resistance || !rn2(3))) {
				losestr(StrongPoison_resistance ? 1 : Poison_resistance ? rnd(3) : rnd(8), TRUE);
				losehp(d(otmp->cursed ? 4 : 2, otmp->blessed ? 8 : 16), "drinking poisonous urine", KILLED_BY);
			}
		}

		unkn++;

		break;

	case POT_AMNESIA:
		pline(FunnyHallu? "This tastes like champagne!" :
			"This liquid bubbles and fizzes as you drink it.");
		forget((!otmp->blessed? ALL_SPELLS : 0) | ALL_MAP);
		if (FunnyHallu)
		    pline("Hakuna matata!");
		else
		    You_feel("your memories dissolve.");

		/* Blessed amnesia makes you forget lycanthropy, sickness */
		if (otmp->blessed) {
		    if (u.ulycn >= LOW_PM && !Race_if(PM_HUMAN_WEREWOLF) && !Role_if(PM_LUNATIC) && !Race_if(PM_AK_THIEF_IS_DEAD_) ) {
			You("forget your affinity to %s!",
					makeplural(mons[u.ulycn].mname));
			if (youmonst.data == &mons[u.ulycn])
			    you_unwere(FALSE);
			u.ulycn = NON_PM;	/* cure lycanthropy */
		    }
		    make_sick(0L, (char *) 0, TRUE, SICK_ALL);

		    /* You feel refreshed */
		    u.uhunger += 50 + rnd(50);
		    newuhs(FALSE);
		} else
		    exercise(A_WIS, FALSE);
		break;
	case POT_WATER:
		if(!otmp->blessed && !otmp->cursed) {
		    pline("This tastes like water.");
		    u.uhunger += rnd(10);
		    newuhs(FALSE);
		    break;
		}
		unkn++;
		if(is_undead(youmonst.data) || is_demon(youmonst.data) ||
				u.ualign.type == A_CHAOTIC) {
		    if(otmp->blessed) {
			pline("This burns like acid!");
			exercise(A_CON, FALSE);
			if (u.ulycn >= LOW_PM && !Race_if(PM_HUMAN_WEREWOLF) && !Role_if(PM_LUNATIC) && !Race_if(PM_AK_THIEF_IS_DEAD_) ) {
			    Your("affinity to %s disappears!",
				 makeplural(mons[u.ulycn].mname));
			    if (youmonst.data == &mons[u.ulycn])
				you_unwere(FALSE);
			    u.ulycn = NON_PM;	/* cure lycanthropy */
			}
			losehp(d(6,6), "potion of holy water", KILLED_BY_AN);
			if (uinsymbiosis) uncursesymbiote(FALSE);
		    } else if(otmp->cursed) {
			You_feel("quite proud of yourself.");
			healup(d(6,6),0,0,0);
			if (u.ulycn >= LOW_PM && !Upolyd) you_were();
			exercise(A_CON, TRUE);
		    }
		} else {
		    if(otmp->blessed) {
			You_feel("full of awe.");
			if(u.ualign.type == A_LAWFUL) healup(d(6,6),0,0,0);                        
			make_sick(0L, (char *) 0, TRUE, SICK_ALL);
			exercise(A_WIS, TRUE);
			exercise(A_CON, TRUE);
			if (u.ulycn >= LOW_PM && !Race_if(PM_HUMAN_WEREWOLF) && !Role_if(PM_LUNATIC) && !Race_if(PM_AK_THIEF_IS_DEAD_) ) {
			    you_unwere(TRUE);	/* "Purified" */
			}
			if (uinsymbiosis) uncursesymbiote(FALSE);
			/* make_confused(0L,TRUE); */
		    } else {
			if(u.ualign.type == A_LAWFUL) {
			    pline("This burns like acid!");
			    losehp(d(6,6), "potion of unholy water",
				KILLED_BY_AN);
			} else
			    You_feel("full of dread.");
			if (u.ulycn >= LOW_PM && !Upolyd) you_were();
			exercise(A_CON, FALSE);
		    }
		}
		break;
	case POT_BOOZE:

		if (otmp->oartifact == ART_BOOMSHINE) {
			pline("KABOOM!!! You are hit by a massive explosion!");
			losehp(rnd(50), "exploding boomshine", KILLED_BY);
		      u.uprops[CONFUSION].intrinsic |= FROMOUTSIDE;
			HConfusion |= FROMOUTSIDE;
			HConfusion |= FROMRACE;
			HConfusion |= FROMEXPER;
			u.boomshined = 1;
			pline("That was very stupid of you, and your body will never be as it used to be...");
		}

	case POT_WINE:
		unkn++;

		if (Role_if(PM_PIRATE) || Role_if(PM_KORSAIR) || (uwep && uwep->oartifact == ART_ARRRRRR_MATEY) ) pline("Ye splice the mainbrace.");
		else pline("Ooph!  This tastes like %s%s!",
		      otmp->odiluted ? "watered down " : "",
		      FunnyHallu ? "dandelion wine" : "liquid fire");
		if (!otmp->blessed)
		    make_confused(itimeout_incr(HConfusion, d(3,8)), FALSE);
		/* the whiskey makes us feel better */
		if (!otmp->odiluted) healup(Role_if(PM_DRUNK) ? rnz(20 + u.ulevel) : 1, 0, FALSE, FALSE);
		u.uhunger += 10 * (2 + bcsign(otmp));
		if (Race_if(PM_CLOCKWORK_AUTOMATON)) u.uhunger += 200;
		if (Race_if(PM_RUSMOT)) u.uhunger += 100;
		if (Role_if(PM_DRUNK)) u.uhunger += 100;
		newuhs(FALSE);
		exercise(A_WIS, FALSE);
		if(otmp->cursed && !Race_if(PM_RUSMOT)) {
			You((Role_if(PM_PIRATE) || Role_if(PM_KORSAIR) || (uwep && uwep->oartifact == ART_ARRRRRR_MATEY) ) ? "are loaded to the gunwhales." : "pass out.");
			multi = -rnd(15);
			nomovemsg = "You awake with a headache.";
		}
		break;
	case POT_ENLIGHTENMENT:
		if(otmp->cursed) {
			unkn++;
			You("have an uneasy feeling...");
			if (evilfriday) {
				adjattrib(A_WIS, -1, 0, TRUE);
				adjattrib(A_INT, -1, 0, TRUE);
			}
			exercise(A_WIS, FALSE);
		} else {
			if (otmp->blessed) {
				if (!rn2(3)) (void) adjattrib(A_INT, 1, FALSE, TRUE);
				if (!rn2(3)) (void) adjattrib(A_WIS, 1, FALSE, TRUE);
			}
			You_feel("self-knowledgeable...");
			display_nhwindow(WIN_MESSAGE, FALSE);
			enlightenment(0, 1);
			pline_The("feeling subsides.");
			exercise(A_WIS, TRUE);
		}
		break;
	case SPE_INVISIBILITY:
		/* spell cannot penetrate mummy wrapping */
		if (BInvis && uarmc->otyp == MUMMY_WRAPPING) {
			You_feel("rather itchy under your %s.", xname(uarmc));
			break;
		}
		/* FALLTHRU */
	case POT_INVISIBILITY:
		if (Invis || Blind || BInvis) {
		    nothing++;
		} else {
		    self_invis_message();
		}
		if (otmp->blessed) HInvis |= FROMOUTSIDE;
		else incr_itimeout(&HInvis, rn1(15,31));
		newsym(u.ux,u.uy);	/* update position */
		if(otmp->cursed) {
			pline(FunnyHallu ? "Somehow, you get the feeling there's a stalker waiting for you around the corner." : "For some reason, you feel your presence is known.");
			aggravate();
			if (evilfriday) {
				if (HInvis & INTRINSIC) {
					HInvis &= ~INTRINSIC;
					You_feel("paranoid.");
				}
				if (HInvis & TIMEOUT) {
					HInvis &= ~TIMEOUT;
					You_feel("paranoid.");
				}
			}
		}
		break;
	case POT_SEE_INVISIBLE:
		/* tastes like fruit juice in Rogue */
	case POT_FRUIT_JUICE:
	    {
		int msg = Invisible && !Blind;

		unkn++;
		if (otmp->cursed)
		    pline("Yecch!  This tastes %s.",
			  FunnyHallu ? "overripe" : "rotten");
		else
		    pline(FunnyHallu ?
		      "This tastes like 10%% real %s%s all-natural beverage." :
				"This tastes like %s%s.",
			  otmp->odiluted ? "reconstituted " : "",
			  fruitname(TRUE));
		if (otmp->otyp == POT_FRUIT_JUICE) {
		    u.uhunger += (otmp->odiluted ? 15 : 30) * (2 + bcsign(otmp));
		    newuhs(FALSE);
		    break;
		}
		if (!otmp->cursed) {
			/* Tell them they can see again immediately, which
			 * will help them identify the potion...
			 */
			make_blinded(0L,TRUE);
		}
		if (otmp->blessed)
			HSee_invisible |= FROMOUTSIDE;
		else
			incr_itimeout(&HSee_invisible, rn1(100,750));
		set_mimic_blocking(); /* do special mimic handling */
		see_monsters();	/* see invisible monsters */
		newsym(u.ux,u.uy); /* see yourself! */
		if (msg && !Blind) { /* Blind possible if polymorphed */
		    You("can see through yourself, but you are visible!");
		    unkn--;
		}

		if (otmp->cursed && otmp->otyp == POT_SEE_INVISIBLE && evilfriday) {
			if (HSee_invisible & INTRINSIC) {
				HSee_invisible &= ~INTRINSIC;
				You("%s!", FunnyHallu ? "tawt you taw a puttie tat" : "thought you saw something");
			}
			if (HSee_invisible & TIMEOUT) {
				HSee_invisible &= ~TIMEOUT;
				You("%s!", FunnyHallu ? "tawt you taw a puttie tat" : "thought you saw something");
			}
		}

		break;
	    }
	case POT_COFFEE:
		if (otmp->cursed)
		    pline("Yecch!  This tastes %s.",
			  FunnyHallu ? "like sewage" : "extremely bitter");
		else
		    pline(FunnyHallu ?
		      "This tastes like bean juice." :
				"This tastes like coffee." );
		    u.uhunger += (otmp->odiluted ? 10 : 20) * (2 + bcsign(otmp));
		    newuhs(FALSE);
		   if (!otmp->cursed) HSleep_resistance += rnd(25);

		if (otmp->cursed && evilfriday) {
			if (HSleep_resistance & INTRINSIC) {
				HSleep_resistance &= ~INTRINSIC;
				You_feel("tired all of a sudden.");
			}
			if (HSleep_resistance & TIMEOUT) {
				HSleep_resistance &= ~TIMEOUT;
				You_feel("tired all of a sudden.");
			}
		}

		break;

	case POT_RED_TEA:
		if (otmp->cursed)
		    pline("Yecch!  This tastes %s.",
			  FunnyHallu ? "like blood.." : "awful");
		else
		    pline(FunnyHallu ?
		      "This tastes like cherry juice." :
				"This tastes like red tea." );
		    u.uhunger += (otmp->odiluted ? 20 : 40) * (2 + bcsign(otmp));
		    newuhs(FALSE);
		   if (!otmp->cursed) HFire_resistance += rnd(25);

		if (otmp->cursed && evilfriday) {
			if (HFire_resistance & INTRINSIC) {
				HFire_resistance &= ~INTRINSIC;
				You_feel("warmer.");
			}
			if (HFire_resistance & TIMEOUT) {
				HFire_resistance &= ~TIMEOUT;
				You_feel("warmer.");
			}
		}

		break;

	case POT_OOLONG_TEA:
		if (otmp->cursed)
		    pline("Yecch!  This tastes %s.",
			  FunnyHallu ? "like death" : "very rotten");
		else
		    pline(FunnyHallu ?
		      "This tastes like peppermint." :
				"This tastes like oolong tea." );
		    u.uhunger += (otmp->odiluted ? 20 : 40) * (2 + bcsign(otmp));
		    newuhs(FALSE);
		   if (!otmp->cursed) HShock_resistance += rnd(25);

		if (otmp->cursed && evilfriday) {
			if (HShock_resistance & INTRINSIC) {
				HShock_resistance &= ~INTRINSIC;
				You_feel("like someone has zapped you.");
			}
			if (HShock_resistance & TIMEOUT) {
				HShock_resistance &= ~TIMEOUT;
				You_feel("like someone has zapped you.");
			}
		}

		break;

	case POT_GREEN_TEA:
		if (otmp->cursed)
		    pline("Yecch!  This tastes %s.",
			  FunnyHallu ? "like the contents of a trash can" : "poisonous");
		else
		    pline(FunnyHallu ?
		      "This tastes like your mother-in-law's tea!" :
				"This tastes like green tea." );
		    u.uhunger += (otmp->odiluted ? 20 : 40) * (2 + bcsign(otmp));
		    newuhs(FALSE);
		   if (!otmp->cursed) HCold_resistance += rnd(25);

		if (otmp->cursed && evilfriday) {
			if (HCold_resistance & INTRINSIC) {
				HCold_resistance &= ~INTRINSIC;
				You_feel("cooler.");
			}
			if (HCold_resistance & TIMEOUT) {
				HCold_resistance &= ~TIMEOUT;
				You_feel("cooler.");
			}
		}

		break;

	case POT_GREEN_MATE:
		if (otmp->cursed)
		    pline("Yecch!  This tastes %s.",
			  FunnyHallu ? "like pee" : "stale");
		else
		    pline(FunnyHallu ?
		      "This tastes like bitter woodruff!" :
				"This tastes like green mate." );
		    u.uhunger += (otmp->odiluted ? 20 : 40) * (2 + bcsign(otmp));
		    newuhs(FALSE);
		   if (!otmp->cursed) HPoison_resistance += rnd(25);

		if (otmp->cursed && evilfriday) {
			if (HPoison_resistance & INTRINSIC) {
				HPoison_resistance &= ~INTRINSIC;
				You_feel("a little sick!");
			}
			if (HPoison_resistance & TIMEOUT) {
				HPoison_resistance &= ~TIMEOUT;
				You_feel("a little sick!");
			}
		}

		break;

	case POT_COCOA:
		if (otmp->cursed)
		    pline("Yecch!  This tastes %s.",
			  FunnyHallu ? "like shit" : "wretched");
		else
		    pline(FunnyHallu ?
		      "This tastes like hot chocolate." :
				"This tastes like cocoa." );
		    u.uhunger += (otmp->odiluted ? 100 : 200) * (2 + bcsign(otmp));
		    newuhs(FALSE);
		   if (!otmp->cursed) HDisint_resistance += rnd(25);

		if (otmp->cursed && evilfriday) {
			if (HDisint_resistance & INTRINSIC) {
				HDisint_resistance &= ~INTRINSIC;
				You_feel("like you're going to break apart.");
			}
			if (HDisint_resistance & TIMEOUT) {
				HDisint_resistance &= ~TIMEOUT;
				You_feel("like you're going to break apart.");
			}
		}

		break;

	case POT_TERERE:
		if (otmp->cursed)
		    pline(FunnyHallu ? "This tastes like ecstasy spiked with poison." : "This tastes like stale alcohol." );
		else
		    pline(FunnyHallu ? "This tastes like ecstasy." : "This tastes like alcohol." );
		    u.uhunger += (otmp->odiluted ? 250 : 500) * (2 + bcsign(otmp));
		    newuhs(FALSE);

		make_confused(itimeout_incr(HConfusion, rnd(50 - 25 * bcsign(otmp))), FALSE);

		break;

	case POT_AOJIRU:
		if (otmp->cursed)
		    pline(FunnyHallu ? "This tastes like something that might kill you!" : "This tastes like illegal drugs." );
		else
		    pline(FunnyHallu ? "This tastes like some illegal shit." : "This tastes like drugs." );
		    u.uhunger += (otmp->odiluted ? 250 : 500) * (2 + bcsign(otmp));
		    newuhs(FALSE);

		make_stunned(itimeout_incr(HStun, rnd(50 - 25 * bcsign(otmp))), FALSE);

		break;

	case POT_ULTIMATE_TSUYOSHI_SPECIAL: {
		int time;
		if (otmp->cursed) {
			pline("Ulch! What in the hell was that???");
			adjattrib(A_CON,-1,-1, TRUE);
			break;
		}
		You_feel("super-powerful!");
		incr_itimeout(&Invulnerable, 8 + rn2(4) );
		time = 15 + d(otmp->blessed ? 4 : 2, 8);
		incr_itimeout(&HFast, time);
		incr_itimeout(&HSee_invisible, time);
		make_hallucinated(HHallucination + time + d(10, 3), FALSE, 0L);
		u.uhpmax += rnd(5);
		u.uhp = u.uhpmax;
		if (uactivesymbiosis) {
			u.usymbiote.mhpmax += 5;
			if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
		}
		} break;
	case POT_MEHOHO_BURUSASAN_G:
		pline("%s", fauxmessage());
		u.cnd_plineamount++;
		if (!rn2(3)) {
			pline("%s", fauxmessage());
			u.cnd_plineamount++;
		}
		if (otmp->cursed || !rn2(3)) {
			adjattrib(A_CON,-1,-1, TRUE);
		}
		morehungry(rn1(50, 101));
		make_confused(HConfusion + d(10,2),FALSE);
		if (otmp->blessed || !rn2(3)) {
			You(FunnyHallu ? "feel slightly the same." : "feel slightly different.");
			if (u.uhp < u.uhpmax) u.uhp++;
		}
		break;

	case POT_TRAINING:

		doubleskilltraining();
		break;

	case POT_TECH_LEVEL_UP:
		if(otmp->cursed) {
			unkn++;
			You("have a tummy ache...");
			if (evilfriday) {
				techdrain();
			}
		} else {
			You_feel("very good!");
			techlevelup();
	            makeknown(POT_TECH_LEVEL_UP);
		}

		break;

	case POT_BENEFICIAL_EFFECT:

		switch (rnd(13)) {

			case 1:
				if (ABASE(A_CHA) < ATTRMAX(A_CHA)) {
					You_feel("more %s!", flags.female ? "pretty" : "attractive");
					(void) adjattrib(A_CHA, 1, FALSE, TRUE);
					break;
				}
			break;
			case 2:
				if (ABASE(A_CON) < ATTRMAX(A_CON)) {
					You_feel("tougher!");
					(void) adjattrib(A_CON, 1, FALSE, TRUE);
					}
			break;
			case 3:
				u.uenmax += rnd(3);
				You_feel("a mild buzz.");
				flags.botl = 1;
			break;
			case 4:
				if (Upolyd) {
					u.mh++;
					u.mhmax++;
					if (u.mh > u.mhmax) u.mh = u.mhmax;
				} else {
					u.uhp++;
					u.uhpmax++;
					if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				}
				if (uactivesymbiosis) {
					u.usymbiote.mhpmax++;
					if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
				}
				You_feel("vitalized.");
				flags.botl = 1;
			break;
			case 5:
				You_feel("clairvoyant!");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vy znayete raspolozheniye, no ne lovushki, a te vse ravno budut poshel na khuy vverkh." : "Wschiiiiiiie!");
				incr_itimeout(&HClairvoyant, rnd(500));
			break;
			case 6:
			      You_feel("that was a smart thing to do.");
				gainlevelmaybe();
			break;
			case 7:
				if (Upolyd) u.mh = u.mhmax;
				else u.uhp = u.uhpmax;
				You_feel("restored to health!");
				flags.botl = 1;
			break;
			case 8:
				if (!(HInvis & INTRINSIC)) You_feel("hidden!");
				else You_feel("able to see what cannot be seen!");
				HInvis |= FROMOUTSIDE;
				HSee_invisible |= FROMOUTSIDE;
			break;
			case 9:
				gainstr((struct obj *)0, 0);
				pline(FunnyHallu ? "You feel like ripping out some trees!" : "You feel stronger!");
				break;
			break;
			case 10:
				if (ABASE(A_INT) < ATTRMAX(A_INT)) {
					(void) adjattrib(A_INT, 1, FALSE, TRUE);
				}
				else {
					pline(FunnyHallu ? "Eek, that tasted like rotten oversalted seaweed!" : "For some reason, that tasted bland.");
				}
			break;
			case 11:
				if (u.contamination && u.contamination < 1000) {
				decontaminate(100);
				}
			break;
			case 12:
				if (!Very_fast)
					You("are suddenly moving %sfaster.", Fast ? "" : "much ");
				else {
					Your("%s get new energy.", makeplural(body_part(LEG)));
				}
				incr_itimeout(&HFast, rn1(1000, 1000));
			break;
			case 13:
				You_feel("ethereal.");
				incr_itimeout(&HPasses_walls, rn1(10, 50));
			break;

		}

		break;

	case POT_RANDOM_INTRINSIC:
		Your("intrinsics change.");
		intrinsicgainorloss();
		break;

	case POT_PARALYSIS:
		if (Free_action && rn2(StrongFree_action ? 100 : 20))
		    You("stiffen momentarily.");
		else {
		    if (Levitation || Is_airlevel(&u.uz)||Is_waterlevel(&u.uz))
			You("are motionlessly suspended.");
		    else if (u.usteed)
			You("are frozen in place!");
		    else
			Your("%s are frozen to the %s!",
			     makeplural(body_part(FOOT)), surface(u.ux, u.uy));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
		    nomul(-(rn1(10, 25 - 12*bcsign(otmp))), "frozen by a potion", TRUE);
		    nomovemsg = You_can_move_again;
		    exercise(A_DEX, FALSE);
		}
		break;
	case POT_SLEEPING:
		if((Sleep_resistance || Free_action) && rn2(StrongSleep_resistance ? 20 : 5))
		    You("yawn.");
		else {
		    You("suddenly fall asleep!");
		    fall_asleep(-rn1(10, 25 - 12*bcsign(otmp)), TRUE);
		}
		break;
	case POT_MONSTER_DETECTION:
	case SPE_DETECT_MONSTERS:
		if (otmp->blessed) {
		    int x, y;

		    if (Detect_monsters) nothing++;
		    unkn++;
		    /* after a while, repeated uses become less effective */
		    if (HDetect_monsters >= 300L)
			i = 20;
		    else
			i = rn1(40,21);
		    if (otmp->otyp == SPE_DETECT_MONSTERS) {
			if (HDetect_monsters >= 300L) i = rnd(2);
			else i = rnd(5);
		    }
		    incr_itimeout(&HDetect_monsters, i);
		    for (x = 1; x < COLNO; x++) {
			for (y = 0; y < ROWNO; y++) {
			    if (memory_is_invisible(x, y)) {
				unmap_object(x, y);
				newsym(x,y);
			    }
			    if (MON_AT(x,y)) unkn = 0;
			}
		    }
		    see_monsters();
		    if (unkn) You_feel("lonely.");
		    break;
		}

		if (otmp->cursed && evilfriday) {
			forget(3);
		}

		if (monster_detect(otmp, 0))
			return(1);		/* nothing detected */
		exercise(A_WIS, TRUE);
		break;
	case POT_OBJECT_DETECTION:
	case SPE_DETECT_TREASURE:
		if (otmp->cursed && evilfriday) {
			forget(3);
		}

		if (object_detect(otmp, 0))
			return(1);		/* nothing detected */
		exercise(A_WIS, TRUE);
		break;
	case POT_SICKNESS:
	case POT_POISON:
		pline("Yecch!  This stuff tastes like poison.");
		if (otmp->blessed) {
		    pline("(But in fact it was mildly stale %s.)",
			  fruitname(TRUE));
		    if (!Role_if(PM_HEALER) && !Race_if(PM_HERBALIST)) {
			/* NB: blessed otmp->fromsink is not possible */
			losehp(1, "mildly contaminated potion", KILLED_BY_AN);
		    }
		} else {
		    if(Poison_resistance && (StrongPoison_resistance || rn2(10)) )
			pline(
			  "(But in fact it was biologically contaminated %s.)",
			      fruitname(TRUE));
		    if (Role_if(PM_HEALER) || Race_if(PM_HERBALIST))
			pline("Fortunately, you have been immunized.");
		    else {
			int typ = rn2(A_MAX);

			if (!SustainAbilityOn || !SustainLossSafe) {
			    poisontell(typ);
			    if (!StrongPoison_resistance || !rn2(3)) {
				(void) adjattrib(typ, Poison_resistance ? -1 : -rno(5), TRUE, TRUE);
			    }
			}
			if(!Poison_resistance) {
			    if (otmp->fromsink)
				losehp(rnd(10)+5*!!(otmp->cursed),
				       "contaminated tap water", KILLED_BY);
			    else
				losehp(rnd(10)+5*!!(otmp->cursed),
				       "contaminated potion", KILLED_BY_AN);
			}
			exercise(A_CON, FALSE);
		    }
		}
		if(Hallucination) {
			You("are shocked back to your senses!");
			(void) make_hallucinated(0L,FALSE,0L);
		}
		break;
	case POT_CONFUSION:
		if(!Confusion) {
		    if (FunnyHallu) {
			pline("What a trippy feeling!");
			unkn++;
		    } else if (Role_if(PM_PIRATE) || Role_if(PM_KORSAIR) || (uwep && uwep->oartifact == ART_ARRRRRR_MATEY) )
			pline("Blimey! Ye're one sheet to the wind!");
			else 
			pline("Huh, What?  Where am I?");
		} else	nothing++;
		make_confused(itimeout_incr(HConfusion,
					    rn1(35, 80 - 25 * bcsign(otmp))),
			      FALSE);
		break;
	case POT_CLAIRVOYANCE:
		/* KMH -- handle cursed, blessed, blocked */
		if (otmp->cursed)
			nothing++;
		else if (!BClairvoyant) {
			if (FunnyHallu) pline("Dude! See-through walls!");
			do_vicinity_map();
		}
		if (otmp->blessed)
			incr_itimeout(&HClairvoyant, rn1(50, 100));
		break;
	case POT_ESP:
	{
		const char *mod;

		/* KMH -- handle cursed, blessed */
		if (otmp->cursed) {
			if (HTelepat) mod = "less ";
			else {
			    unkn++;
			    mod = NULL;
			}
			HTelepat = 0;
		} else if (otmp->blessed) {
			mod = "fully ";
			incr_itimeout(&HTelepat, rn1(100, 200));
			HTelepat |= FROMOUTSIDE;
		} else {
			mod = "more ";
			incr_itimeout(&HTelepat, rn1(50, 100));
		}
		if (mod)
			You_feel(FunnyHallu ?
				"%sin touch with the cosmos." :
				"%smentally acute.", mod);
		see_monsters();
		break;
	}
	/* KMH, balance patch -- removed */
	/* but re-inserted by Amy */
	case POT_FIRE_RESISTANCE:
	       if(!(HFire_resistance & FROMOUTSIDE)) {
		if (FunnyHallu)
		   pline("You feel, like, totally cool!");
		   else You_feel("cooler.");
		   HFire_resistance += rn1(100,50 + 25 * bcsign(otmp) );
		   unkn++;
		   HFire_resistance |= FROMOUTSIDE;
		}
		break;
	case POT_RESISTANCE:
		You_feel("resistant to elemental attacks!");
		   HFire_resistance += rn1(100,50 + 25 * bcsign(otmp) );
		   HCold_resistance += rn1(100,50 + 25 * bcsign(otmp) );
		   HShock_resistance += rn1(100,50 + 25 * bcsign(otmp) );
		break;
	case POT_INVULNERABILITY:
		incr_itimeout(&Invulnerable, rn1(4, 8 + 4 * bcsign(otmp)));
		You_feel(FunnyHallu ?
				"like a super-duper hero!" : "invulnerable!");
		break;
	case POT_GAIN_ABILITY:
		if(otmp->cursed) {
		    pline("Ulch!  That potion tasted foul!");
		    if (evilfriday) {
			adjattrib(rn2(A_MAX), -1, 0, TRUE);
		    }
		    unkn++;
		} else if (SustainAbilityOn) {
		    nothing++;
		} else {      /* If blessed, increase all; if not, try up to */
		    int itmp; /* 6 times to find one which can be increased. */
		    i = -1;		/* increment to 0 */
		    for (ii = A_MAX; ii > 0; ii--) {
			i = (/*otmp->blessed ? i + 1 :*/ rn2(A_MAX));
			/* only give "your X is already as high as it can get"
			   message on last attempt (except blessed potions) */
			itmp = (otmp->blessed || ii == 1) ? 0 : -1;
			if (adjattrib(i, 1, itmp, TRUE) && !otmp->blessed)
			    break;
			if (rn2(5)) break; /* now, blessed ones no longer always increase every stat --Amy */
		    } /* but a blessed one has a chance to increase more than one stat, or increase one stat twice */
		}
		break;
	case POT_SPEED:

		if (evilfriday && otmp->cursed) {
			u.uprops[DEAC_FAST].intrinsic += rnd(500);
			pline(u.inertia ? "You feel even slower." : "You slow down to a crawl.");
			u.inertia += rnd(500);
			break; /* don't run the code that makes you fast */
		}

		if (otmp->blessed) {
			if (!(HFast & INTRINSIC)) {
				if (!Fast) You("speed up.");
				else Your("quickness feels more natural.");
				exercise(A_DEX, TRUE);
			}
			HFast |= FROMOUTSIDE;
			if (u.inertia) {
				u.inertia = 0;
				You_feel("less slow.");
			}
		} else if (!otmp->blessed && !otmp->cursed && u.inertia > 1) {
			u.inertia /= 2;
			Your("inertia counter is halved.");
		}

		if(Wounded_legs && !otmp->cursed && !u.usteed	/* heal_legs() would heal steeds legs */
		) {
			heal_legs();
			unkn++;
			break;
		} /* and fall through */
	case SPE_HASTE_SELF:
		if (!Very_fast)
			You("are suddenly moving %sfaster.",
				Fast ? "" : "much ");
		else {
			Your("%s get new energy.",
				makeplural(body_part(LEG)));
			unkn++;
		}
		exercise(A_DEX, TRUE);
		if (otmp->otyp == SPE_HASTE_SELF) incr_itimeout(&HFast, rn1(10, 10 + 10 * bcsign(otmp)));
		else incr_itimeout(&HFast, rn1(10, 100 + 60 * bcsign(otmp)));
		break;
	case POT_BLINDNESS:
		if(Blind) nothing++;
		make_blinded(itimeout_incr(Blinded,
					   rn1(200, 250 - 125 * bcsign(otmp))),
			     (boolean)!Blind);
		break;

	case POT_GAIN_LEVEL:
		if (otmp->cursed) {
			if (evilfriday) {
				losexp("a cursed potion of gain level", FALSE, TRUE);
			}
			unkn++;
			/* they went up a level */
			if((ledger_no(&u.uz) == 1 && ((u.uhave.amulet && u.amuletcompletelyimbued) || u.freeplaymode) ) ||
				Can_rise_up(u.ux, u.uy, &u.uz)) {
			    const char *riseup ="rise up, through the %s!";
			    /* [ALI] Special handling for quaffing potions
			     * off the floor (otmp won't be valid after
			     * we change levels otherwise).
			     */
			    if (otmp->where == OBJ_FLOOR) {
				if (otmp->quan > 1)
					(void) splitobj(otmp, 1);
				/* Make sure you're charged if in shop */
				otmp->quan++;
				useupf(otmp, 1);
				obj_extract_self(otmp);
			    }
			    if(ledger_no(&u.uz) == 1) {
			        You(riseup, ceiling(u.ux,u.uy));
				goto_level(&earth_level, FALSE, FALSE, FALSE);
				} else {
				/* Skipping levels during the ascension run is a cheap strategy. --Amy */
			        register int newlev = (u.freeplaymode) ? depth(&u.uz)-1 : (u.uhave.amulet || (u.uevent.udemigod && rn2(2)) ) ? depth(&u.uz)+1 : depth(&u.uz)-1;
				d_level newlevel;

				/* But using the new system to cheat the necessity to invoke would be even cheaper. */
				if(ledger_no(&u.uz) == 99 && u.uevent.udemigod && !u.uevent.invoked) {
				    pline("You crash into the floor.");
					nomul(-rnd(10), "lying on the floor, unable to get up", TRUE);
					nomovemsg = "You finally get up again.";
				    break;
				}

				get_level(&newlevel, newlev);
				if(on_level(&newlevel, &u.uz)) {
				    pline("It tasted bad.");
				    break;
				} else if (newlev < ledger_no(&u.uz)) You(riseup, ceiling(u.ux,u.uy));
				else pline("You slide downwards...");
				goto_level(&newlevel, FALSE, FALSE, FALSE);
				/* Don't complain. Instead, be glad that the obnoxious mysterious force doesn't exist! */
			    }
			}
			else You("have an uneasy feeling.");
			break;
		}

		{
			int urlvl = u.ulevel;
			gainlevelmaybe();
			if (otmp->blessed && (u.ulevel > urlvl) )
				/* blessed potions place you at a random spot in the
				 * middle of the new level instead of the low point
				 * Amy edit: only if you actually gained a level, and only up to 200k EXP for the new level */
				u.uexp = rndexp(TRUE);
		}

		break;
	case POT_DOWN_LEVEL:
		if (otmp->cursed) {
			unkn++;
			/* they went up a level */
			if((ledger_no(&u.uz) == 1 && u.uhave.amulet && u.amuletcompletelyimbued) ||
				Can_rise_up(u.ux, u.uy, &u.uz)) {
			    const char *riseup ="rise up, through the %s!";
			    /* [ALI] Special handling for quaffing potions
			     * off the floor (otmp won't be valid after
			     * we change levels otherwise).
			     */
			    if (otmp->where == OBJ_FLOOR) {
				if (otmp->quan > 1)
					(void) splitobj(otmp, 1);
				/* Make sure you're charged if in shop */
				otmp->quan++;
				useupf(otmp, 1);
				obj_extract_self(otmp);
			    }
			    if(ledger_no(&u.uz) == 1) {
			        You(riseup, ceiling(u.ux,u.uy));
				goto_level(&earth_level, FALSE, FALSE, FALSE);
				} else {
				/* Skipping levels during the ascension run is a cheap strategy. --Amy */
			        register int newlev = depth(&u.uz)+1;
				d_level newlevel;

				/* But using the new system to cheat the necessity to invoke would be even cheaper. */
				if(ledger_no(&u.uz) == 99) {
				    pline("You crash into the floor.");
					nomul(-rnd(10), "lying on the floor, unable to get up", TRUE);
					nomovemsg = "You finally get up again.";
				    break;
				}

				get_level(&newlevel, newlev);
				if(on_level(&newlevel, &u.uz)) {
				    pline("It tasted bad.");
				    break;
				}
				else pline("You slide downwards...");
				goto_level(&newlevel, FALSE, FALSE, FALSE);
			    }
			}
			else You("have an uneasy feeling.");
			break;
		}
		if (Drain_resistance && (StrongDrain_resistance || rn2(10)) ) {
		    You_feel("rejuvenating momentarily.");
		} else {
		    You("restore youth!");
		    losexp("return to the state before being born", FALSE, TRUE);
		}

		break;
	case POT_HEALING:
		You_feel("better.");
		healup(d(5,6) + rnz(u.ulevel) + 5 * bcsign(otmp),
		       otmp->blessed ? 2 : !otmp->cursed ? 1 : 0, 1+1*!!otmp->blessed, !otmp->cursed);

		if (evilfriday && otmp->cursed) {
			u.uhpmax--;
			if (u.uhpmax < 1) u.uhpmax = 1;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		}

		exercise(A_CON, TRUE);
		break;
	case POT_EXTRA_HEALING:
		You_feel("much better.");
		healup(d(6,8) + rnz(u.ulevel) + 5 * bcsign(otmp),
		       otmp->blessed ? 5 : !otmp->cursed ? 2 : 0,
		       !otmp->cursed, TRUE);
		if (evilfriday && otmp->cursed) {
			u.uhpmax -= 2;
			if (u.uhpmax < 1) u.uhpmax = 1;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		}

		(void) make_hallucinated(0L,TRUE,0L);
		exercise(A_CON, TRUE);
		exercise(A_STR, TRUE);
		break;
	case POT_FULL_HEALING:
		You_feel("completely healed.");
		healup(400 + rnz(u.ulevel), 4+4*bcsign(otmp), !otmp->cursed, TRUE);
		if (evilfriday && otmp->cursed) {
			u.uhpmax -= 4;
			if (u.uhpmax < 1) u.uhpmax = 1;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		}

		/* Restore one lost level if blessed */
		if (otmp->blessed && u.ulevel < u.ulevelmax) {
		    /* when multiple levels have been lost, drinking
		       multiple potions will only get half of them back */
		    u.ulevelmax -= 1;
		    gainlevelmaybe();
		}
		(void) make_hallucinated(0L,TRUE,0L);
		exercise(A_STR, TRUE);
		exercise(A_CON, TRUE);
		break;
	case POT_CURE_WOUNDS:
		You_feel("better.");
		healup(d(5,6) + rnz(u.ulevel) + 5 * bcsign(otmp), 0, 0, 0);
		exercise(A_CON, TRUE);
		break;
	case POT_CURE_SERIOUS_WOUNDS:
		You_feel("much better.");
		healup(d(6,8) + rnz(u.ulevel) + 5 * bcsign(otmp), 0, 0, 0);
		exercise(A_CON, TRUE);
		exercise(A_STR, TRUE);
		break;
	case POT_CURE_CRITICAL_WOUNDS:
		You_feel("completely healed.");
		healup(400 + rnz(u.ulevel),  0, 0, 0);
		exercise(A_STR, TRUE);
		exercise(A_CON, TRUE);
		break;
	case POT_LEVITATION:
	case SPE_LEVITATION:
		if (otmp->cursed) HLevitation &= ~I_SPECIAL;
		if(!Levitation) {
			/* kludge to ensure proper operation of float_up() */
			HLevitation = 1;
			float_up();
			/* reverse kludge */
			HLevitation = 0;
			if (otmp->cursed && !Is_waterlevel(&u.uz)) {
	if((u.ux != xupstair || u.uy != yupstair)
	   && (u.ux != sstairs.sx || u.uy != sstairs.sy || !sstairs.up)
	   && (!xupladder || u.ux != xupladder || u.uy != yupladder)
	) {
					You("hit your %s on the %s.",
						body_part(HEAD),
						ceiling(u.ux,u.uy));
					losehp(uarmh ? 1 : rnd(10),
						"colliding with the ceiling",
						KILLED_BY);
				} else (void) doup();
			}
		} else
			nothing++;
		if (otmp->blessed) {
		    incr_itimeout(&HLevitation, rn1(50,(otmp->otyp == SPE_LEVITATION) ? 50 : 250));
		    HLevitation |= I_SPECIAL;
		} else incr_itimeout(&HLevitation, rn1((otmp->otyp == SPE_LEVITATION) ? 30 : 140,10));
		spoteffects(FALSE);	/* for sinks */
		break;
	case POT_GAIN_ENERGY:			/* M. Stephenson */
		{       register int num , num2;
			if(otmp->cursed)
			    You_feel("lackluster.");
			else
			    pline("Magical energies course through your body.");
			num = rnd(25) + rnz(u.ulevel) + 5 * otmp->blessed + 10;                        
			num2 = rnd(2) + 2 * otmp->blessed + 1;
			u.uenmax += (otmp->cursed) ? -num2 : num2;
			u.uen += (otmp->cursed) ? -num : num;
			if(u.uenmax <= 0) u.uenmax = 0;
			if(u.uen <= 0) u.uen = 0;
			if(u.uen > u.uenmax) {
				u.uenmax += ((u.uen - u.uenmax) / 2);
				u.uen = u.uenmax;
			}
			flags.botl = 1;
			exercise(A_WIS, TRUE);
		}
		break;
	case POT_GAIN_HEALTH:			/* Amy */
		{       register int num , num2;
			if(otmp->cursed)
			    You_feel("drained.");
			else
			    pline("The essence of life flows through your body.");
			num = rnd(25) + 5 * otmp->blessed + 10;                        
			num2 = rnd(2) + 2 * otmp->blessed + 1;
			u.uhpmax += (otmp->cursed) ? -num2 : num2;
			u.uhp += (otmp->cursed) ? -num : num;
			if(u.uhpmax < 1) u.uhpmax = 1;
			if(u.uhp <= 0) {
				u.youaredead = 1;
				u.uhp = 0;
				pline("Oh no, your life force has run out...");
				killer_format = KILLED_BY;
				killer = "quaffing a cursed potion of gain health";
				done(DIED);
				u.youaredead = 0;
			}
			if(u.uhp > u.uhpmax) {
				u.uhpmax += ((u.uhp - u.uhpmax) / 2);
				u.uhp = u.uhpmax;
			}
			if (uactivesymbiosis && !otmp->cursed) {
				u.usymbiote.mhpmax += num2;
				if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
			}
			flags.botl = 1;
			exercise(A_WIS, TRUE);
		}
		break;
	case POT_BANISHING_FEAR:
		if (otmp->oartifact == ART_CURSED_PARTS) {
			u.uprops[ITEMCURSING].intrinsic |= FROMOUTSIDE;
			u.uprops[FEAR_RES].intrinsic |= FROMOUTSIDE;
			pline("You worry about cursed items, but you're not afraid at all anymore.");
		}
		{
			if(otmp->cursed) {
				make_feared(HFeared + rnd(100 + (monster_difficulty() * 5) ),TRUE);
			} else {
				You("banish your fears.");
				if (HFeared) make_feared(0L, TRUE);
				if (otmp->blessed) {
					incr_itimeout(&HFear_resistance, rnd(250) );
					if (Fear_resistance) You_feel("more resistant to fear!");
				}
			}
		}
		break;
	case SPE_BANISHING_FEAR:
		{
			You("banish your fears.");

			if (HeavyFeared) break;
			if (HFeared > (rn1(Role_if(PM_HEALER) ? 200 : 100, 20))) {
				int effreduction = rnd(HFeared / 2);
				if (effreduction > 0) {
					HFeared -= effreduction;
					Your("fear counter is reduced.");
				}
				if (!Role_if(PM_HEALER) && !rn2(500)) {
					pline("The spell backlashes!");
					badeffect();
				}
			} else {
				if (HFeared) make_feared(0L, TRUE);
			}
			if (otmp->blessed) {
				incr_itimeout(&HFear_resistance, rnd(250) );
				if (Fear_resistance) You_feel("more resistant to fear!");
			}

		}
		break;
	case POT_OIL:				/* P. Winner */
		{
			boolean good_for_you = FALSE;

			if (otmp->lamplit) {
			    if (likes_fire(youmonst.data)) {
				pline("Ahh, a refreshing drink.");
				good_for_you = TRUE;
			    } else {
				You("burn your %s.", body_part(FACE));
				losehp(d(Fire_resistance ? 1 : 3, 4),
				       "burning potion of oil", KILLED_BY_AN);
			    }
			} else if(otmp->cursed)
			    pline("This tastes like castor oil.");
			else
			    pline("That was smooth!");
			exercise(A_WIS, good_for_you);
		}
		if (Race_if(PM_CLOCKWORK_AUTOMATON)) u.uhunger += rn1(1000,1000);
		if (Race_if(PM_CLOCKWORK_AUTOMATON)) exercise(A_WIS, TRUE);
		break;
	case POT_ACID:
		if (Acid_resistance && (StrongAcid_resistance || rn2(10)) )
			/* Not necessarily a creature who _likes_ acid */
			pline("This tastes %s.", FunnyHallu ? "tangy" : "sour");
		else {
			pline("This burns%s!", otmp->blessed ? " a little" :
					otmp->cursed ? " a lot" : " like acid");
			losehp(d(otmp->cursed ? 2 : 1, otmp->blessed ? 4 : 8),
					"potion of acid", KILLED_BY_AN);
			exercise(A_CON, FALSE);
		}
		if (Stoned) fix_petrification();
		unkn++; /* holy/unholy water can burn like acid too */
		break;
	case POT_POLYMORPH:

		if (otmp->oartifact == ART_PLANECHANGERS) {
			HPolymorph |= FROMOUTSIDE;
			pline("Okay, now you got the intrinsic polymorphitis you wanted.");
		}

	case POT_MUTATION:
		You_feel("a little %s.", FunnyHallu ? "normal" : "strange");
		if (!Unchanging) polyself(FALSE);
		break;
	case POT_SALT_WATER:

		if (Race_if(PM_ELONA_SNAIL) && !Upolyd) {
			pline("It's salt! you start to melt."); /* capitalization is NOT an error --Amy */
			losehp(((u.uhpmax < 10) ? 9999 : (u.uhpmax - rnd(10))), "a potion of salt solution", KILLED_BY);
			break;
		}

		pline("Yuck... this is salt water!");
		exercise(A_WIS, FALSE);
		make_vomiting(Vomiting + rnd(10) + 5,TRUE);
		if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
		break;
	case POT_BLOOD:
	case POT_VAMPIRE_BLOOD:
		unkn++;
		u.uconduct.unvegan++;

		if (Role_if(PM_BLEEDER) && !otmp->cursed) {

		pline(FunnyHallu ? "It's the essence of life itself! Damn, you absolutely need more of this stuff! Where's a dealer when you need one?" : "Your tortured body experiences a strange sense of joy as your lips touch the warm red liquid.");
		if (otmp->otyp == POT_VAMPIRE_BLOOD) u.uhpmax += 1;
		u.uhp = u.uhpmax;

		}

		if (Race_if(PM_HEMOPHAGE) && !otmp->cursed) {

		pline(FunnyHallu ? "It's the essence of life itself! Damn, you absolutely need more of this stuff! Where's a dealer when you need one?" : "Your tortured body experiences a strange sense of joy as your lips touch the warm red liquid.");
		if (otmp->otyp == POT_VAMPIRE_BLOOD) u.uhpmax += 1;
		u.uhp = u.uhpmax;

		}

		if (maybe_polyd(is_vampire(youmonst.data), Race_if(PM_VAMPIRE)) || Role_if(PM_GOFF) ) {
		    violated_vegetarian();
		    if (otmp->cursed)
			pline("Yecch!  This %s.", FunnyHallu ?
			"liquid could do with a good stir" : "blood has congealed");
		    else pline(FunnyHallu ?
		      "The %s liquid stirs memories of home." :
		      "The %s blood tastes delicious.",
			  otmp->odiluted ? "watery" : "thick");
		    if (!otmp->cursed)
			lesshungry((otmp->odiluted ? 1 : 2) *
			  (otmp->otyp == POT_VAMPIRE_BLOOD ? 400 :
			  otmp->blessed ? 15 : 10));
		    if (otmp->otyp == POT_VAMPIRE_BLOOD && otmp->blessed) {
			int num = newhp();
			if (Upolyd) {
			    u.mhmax += num;
			    u.mh += num;
			} else {
			    u.uhpmax += num;
			    u.uhp += num;
			}
		    }
		} else if (otmp->otyp == POT_VAMPIRE_BLOOD) {
		    /* [CWC] fix conducts for potions of (vampire) blood -
		       doesn't use violated_vegetarian() to prevent
		       duplicated "you feel guilty" messages */
		    u.uconduct.unvegetarian++;
		    if (u.ualign.type == A_LAWFUL || Role_if(PM_MONK) || Role_if(PM_HALF_BAKED)) {
			You_feel("%sguilty about drinking such a vile liquid.",
				(Role_if(PM_MONK) || Role_if(PM_HALF_BAKED)) ? "especially " : "");
			u.ugangr++;
			adjalign(-50);
		    } else if (u.ualign.type == A_NEUTRAL)
			adjalign(-3);
		    exercise(A_CON, FALSE);
		    int vampirepolypm = PM_LONG_WORM_TAIL;
		    while (!is_vampire(&mons[vampirepolypm]) || !polyok(&mons[vampirepolypm]) || (notake(&mons[vampirepolypm]) && rn2(4) ) || ((mons[vampirepolypm].mlet == S_BAT) && rn2(2)) || ((mons[vampirepolypm].mlet == S_EYE) && rn2(2) ) || ((mons[vampirepolypm].mmove == 1) && rn2(4) ) || ((mons[vampirepolypm].mmove == 2) && rn2(3) ) || ((mons[vampirepolypm].mmove == 3) && rn2(2) ) || ((mons[vampirepolypm].mmove == 4) && !rn2(3) ) || ( (mons[vampirepolypm].mlevel < 10) && ((mons[vampirepolypm].mlevel + 1) < rnd(u.ulevel)) ) || (!haseyes(&mons[vampirepolypm]) && rn2(2) ) || ( is_nonmoving(&mons[vampirepolypm]) && rn2(5) ) || ( is_eel(&mons[vampirepolypm]) && rn2(5) ) || ( is_nonmoving(&mons[vampirepolypm]) && rn2(20) ) || (is_jonadabmonster(&mons[vampirepolypm]) && rn2(20)) || ( uncommon2(&mons[vampirepolypm]) && !rn2(4) ) || ( uncommon3(&mons[vampirepolypm]) && !rn2(3) ) || ( uncommon5(&mons[vampirepolypm]) && !rn2(2) ) || ( uncommon7(&mons[vampirepolypm]) && rn2(3) ) || ( uncommon10(&mons[vampirepolypm]) && rn2(5) ) || ( is_eel(&mons[vampirepolypm]) && rn2(20) ) ) {
				vampirepolypm = rn2(NUMMONS);
		    }
		    if (!Unchanging)
			polymon(vampirepolypm);
			/* This used to polymorph you permanently but I decided to get rid of that. --Amy */
		} else {
		    violated_vegetarian();
		    pline("Ugh.  That was vile.");
		    make_vomiting(Vomiting+d(10,8), TRUE);
			if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
		}
		break;

	case POT_CYANIDE:
		make_sick(Sick ? Sick/2L + 1L : 20, "cyanide potion", TRUE, SICK_VOMITABLE);
		losestr(StrongPoison_resistance ? 1 : Poison_resistance ? rnd(3) : rnd(10), TRUE);
		pline(FunnyHallu ? "This tastes a little bitter; maybe it's some sort of medicine?" : "CN(-) + HCl <==> HCN + Cl(-) ");
			losehp(d(otmp->cursed ? 4 : 2, otmp->blessed ? 8 : 16),
					"drinking cyanide", KILLED_BY);
		if (u.ulycn != -1) {
			you_unwere(TRUE);
		}
		break;
	case POT_RADIUM:
		pline(FunnyHallu ? "For some reason, that potion tastes... orange. Yes, the color orange, not the fruit." : "This was radioactive radium!");
		if (!rn2(3)) make_sick(Sick ? Sick/2L + 1L : 50,"radium potion", TRUE, SICK_VOMITABLE);
		if ((u.ulycn != -1) && !rn2(3)) {
			you_unwere(TRUE);
		}
		break;
	case POT_JOLT_COLA:
		You("are jolted back to your senses.");
		if (otmp->cursed) {
			You_feel("bad.");
			u.uhp -= rn2(10);
			if (u.uhp < 1) u.uhp = 1;	/* can't kill you */
		} else {
			if (Hallucination) make_hallucinated(0L,FALSE,0L);
			if (uactivesymbiosis) {
				u.usymbiote.mhpmax++;
				if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
			}
			if (otmp->blessed && !rn2(10)) {
				gainlevelmaybe();
			} else {
				u.uhp += 5;
				if (u.uhp > u.uhpmax) {
					if (otmp->blessed) u.uhpmax++;
					u.uhp = u.uhpmax;
				}
			}
		}
		break;

	case POT_EXTREME_POWER:
	{
		int x;

		x = rn2(otmp->blessed ? 20 : 10) + 1;
		if (otmp->cursed) {
			unkn++;
			You("have a bad taste in your mouth.");
			if (x >= u.uhp) x = u.uhp-1;
			losehp(x,"",0);		/* can't kill you */
			u.uhpmax -= x;
			break;
		}
		You_feel("%spowerful!",otmp->blessed ? "very " : "");
		u.uhp += x;
		u.uhpmax += x;
		if (uactivesymbiosis) {
			u.usymbiote.mhpmax += x;
			if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
		}
	} break;

	case POT_RECOVERY: 
		if (otmp->cursed) {
			unkn++;
			You("decide to recover yourself.");
			pline("Nothing happens, however.");
			break;
		}	
		if (otmp->blessed) {
			u.uhpmax += rn2(5);
			if (uactivesymbiosis) {
				u.usymbiote.mhpmax += rn2(5);
				if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
			}
		}
		u.uhp = u.uhpmax;
		You_feel("much, much better.");
		break;


	case POT_HEROISM: {
		/* a total overhaul ... */
		int time;

		if (otmp->cursed) {
			You_feel("cowardly.");

		    set_itimeout(&Invulnerable, 0);
			break;
		}
		You("are surrounded by a strong shimmering aura.");
		time = 10 + rn2(otmp->blessed ? 20 : 10);
		incr_itimeout(&Invulnerable, time);
		incr_itimeout(&HFast, time);
		incr_itimeout(&HSee_invisible, time);
		if (!rnl(3)) make_blinded(Blinded+time,TRUE);	
		u.uhpmax += rn2(5);
		if (uactivesymbiosis) {
			u.usymbiote.mhpmax += rn2(5);
			if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
		}
		u.uhp = u.uhpmax;
		} break;

	case POT_PORTER:
		if (FunnyHallu)
			You_feel("like hopping around!");
		else
			You_feel("very jumpy.");
		incr_itimeout(&HTeleportation, rnd(500));
		if (otmp->cursed) {

			if (HTeleport_control & INTRINSIC) {
				HTeleport_control &= ~INTRINSIC;
				You_feel("unable to control where you're going.");
			}
			if (HTeleport_control & TIMEOUT) {
				HTeleport_control &= ~TIMEOUT;
				You_feel("unable to control where you're going.");
			}

		}

		break;

	case POT_KEEN_MEMORY:
		if (FunnyHallu)
			You_feel("like remembering everything that ever happened to you!");
		else
			pline("Your memory keens.");
		incr_itimeout(&HKeen_memory, rnd(500 + 250 * bcsign(otmp) ));
		if (otmp->oartifact == ART_PERPETUAL_MEMORY) {
			pline("In fact, your memory was fortified for a very long time!");
			incr_itimeout(&HKeen_memory, rnd(7500));
		}

		if (evilfriday && otmp->cursed) {
			if (HKeen_memory & INTRINSIC) {
				HKeen_memory &= ~INTRINSIC;
				You_feel("a case of selective amnesia...");
			}
			if (HKeen_memory & TIMEOUT) {
				HKeen_memory &= ~TIMEOUT;
				You_feel("a case of selective amnesia...");
			}
		}

		break;

	case POT_NIGHT_VISION:
		if (FunnyHallu)
			pline("Everything is visible! Whoa! Look at all the stuff!");
		else
			pline("Your vision range increases.");
		incr_itimeout(&HSight_bonus, rnd(500 + 250 * bcsign(otmp) ));
		vision_full_recalc = 1;

		if (evilfriday && otmp->cursed) {
			if (HSight_bonus & INTRINSIC) {
				HSight_bonus &= ~INTRINSIC;
				You_feel("less perceptive!");
			}
			if (HSight_bonus & TIMEOUT) {
				HSight_bonus &= ~TIMEOUT;
				You_feel("less perceptive!");
			}
		}

		break;

 	case POT_PAN_GALACTIC_GARGLE_BLASTE:
		You_feel("like having your brain smashed out by a slice of lemon wrapped");
		pline("around a large gold brick.");
		if (otmp->cursed || !rn2(4)) {
			adjattrib(A_INT,-1,-1, TRUE);
		}
		make_confused(itimeout_incr(HConfusion, d(6,7)), FALSE);	/* 6d7 is max. 42 */

		if (otmp->blessed || !rn2(4) || u.ulycn != -1) {
			if (u.ulycn != -1 && !Race_if(PM_HUMAN_WEREWOLF) && !Role_if(PM_LUNATIC) && !Race_if(PM_AK_THIEF_IS_DEAD_) ) {
				You_feel("purified.");
				if (!Unchanging && (u.umonnum == u.ulycn) ) {
			    	    if (!Race_if(PM_UNGENOMOLD)) rehumanize();
					else polyself(FALSE);
				}
				u.ulycn = -1;
			}
			You_feel("remembered of %s.",
			FunnyHallu ? "Zaphod Breeblebrox"
				      : "very, very strong liquor.");
			if (u.uhp < u.uhpmax) u.uhp++;
			pline("Strangely, you feel better that before.");
		}
		break;	
				
	case POT_TERCES_DLU:

		pline("%s", fauxmessage());
		u.cnd_plineamount++;
		if (!rn2(3)) {
			pline("%s", fauxmessage());
			u.cnd_plineamount++;
		}

		break;

	case POT_WONDER:

		pline("ide by 0");

		break;

	case POT_HIDING:
	case POT_DECOY_MAKING:

		if(Confusion || Hallucination || metallivorous(youmonst.data)) {
		    pline("This tinfoil is gnarly!");
		    u.uhunger += (otmp->odiluted ? 5 : 10) * (2 + bcsign(otmp));
		    newuhs(FALSE);
		} else {
		    pline("Ecch - It includes tinfoil!");
		    make_vomiting(Vomiting + d(5, (2-bcsign(otmp))), TRUE);
			if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
		}

		break;

	default:
		impossible("What a funny potion! (%ld)", otmp->otyp);
		return(0);
	}
	return(-1);
}

void
healup(nhp, nxtra, curesick, cureblind)
	int nhp, nxtra;
	register boolean curesick, cureblind;
{

	if (uarmc && itemhasappearance(uarmc, APP_NURSE_CLOAK)) nhp *= 2;
	if (uarmh && uarmh->oartifact == ART_SEXYNESS_HAS_A_NAME) {
		nhp *= 2;
		if (Role_if(PM_HEALER)) nhp *= 2;
	}
	if (uwep && uwep->oartifact == ART_HEALHEALHEALHEAL) {
		nhp *= 2;
		if (Role_if(PM_HEALER)) nhp *= 2;
	}
	if (uarmf && uarmf->oartifact == ART_KATIE_MELUA_S_FLEECINESS) {
		nhp *= 2;
		if (Role_if(PM_HEALER)) nhp *= 2;
	}
	if (RngeNursery) nhp *= 2;

	if (nhp) {
		if (Upolyd) {
			u.mh += nhp;
			if(u.mh > u.mhmax) u.mh = (u.mhmax += nxtra);
			else if (!rn2(2)) u.mhmax += nxtra;
		} else {
			u.uhp += nhp;
			if(u.uhp > u.uhpmax) u.uhp = (u.uhpmax += nxtra);
			else if (!rn2(2)) u.uhpmax += nxtra;
		}
		if (uactivesymbiosis) {
			/* heal symbiote by about 10% of the amount you were healed; more if high skill --Amy */
			u.usymbiote.mhpmax += nxtra;
			if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
			if (nhp > 1) {
				int healsymamount = 10;
				int reallyhealsym = 0;
				if (!PlayerCannotUseSkills) {
					switch (P_SKILL(P_SYMBIOSIS)) {
						default: healsymamount = 10; break;
						case P_BASIC: healsymamount = 12; break;
						case P_SKILLED: healsymamount = 14; break;
						case P_EXPERT: healsymamount = 16; break;
						case P_MASTER: healsymamount = 18; break;
						case P_GRAND_MASTER: healsymamount = 20; break;
						case P_SUPREME_MASTER: healsymamount = 22; break;
					}
				} else {
					healsymamount = 10;
				}
				if (Role_if(PM_SYMBIANT)) healsymamount += 1;

				reallyhealsym = nhp;
				reallyhealsym *= healsymamount;
				reallyhealsym /= 10;
				reallyhealsym /= 10; /* it is no mistake that we do this twice... --Amy */
				if (reallyhealsym < 0) reallyhealsym = 0;

				if (reallyhealsym > 0) {
					u.usymbiote.mhp += reallyhealsym;
					if (u.usymbiote.mhp > u.usymbiote.mhpmax) u.usymbiote.mhp = u.usymbiote.mhpmax;
				}
			}
			if (flags.showsymbiotehp) flags.botl = TRUE;
		}

		if (PlayerBleeds) {
			PlayerBleeds -= nhp;
			if (PlayerBleeds < 0) PlayerBleeds = 0; /* fail safe */
			pline(PlayerBleeds ? "Your bleeding decreases." : "Your bleeding has stopped.");
		}

	}
	if(cureblind)	make_blinded(0L,TRUE);
	if(curesick)	make_sick(0L, (char *) 0, TRUE, SICK_ALL);
	exercise(A_CON, TRUE); /* addition by Amy */
	flags.botl = 1;
	return;
}

	/* For healing monsters - analogous to healup for players */
void
healup_mon(mtmp, nhp, nxtra, curesick, cureblind)
	struct monst *mtmp;
	int nhp, nxtra;
	register boolean curesick, cureblind;
{

	if (uarmc && itemhasappearance(uarmc, APP_NURSE_CLOAK)) nhp *= 2;
	if (uarmh && uarmh->oartifact == ART_SEXYNESS_HAS_A_NAME) {
		nhp *= 2;
		if (Role_if(PM_HEALER)) nhp *= 2;
	}
	if (uwep && uwep->oartifact == ART_HEALHEALHEALHEAL) {
		nhp *= 2;
		if (Role_if(PM_HEALER)) nhp *= 2;
	}
	if (uarmf && uarmf->oartifact == ART_KATIE_MELUA_S_FLEECINESS) {
		nhp *= 2;
		if (Role_if(PM_HEALER)) nhp *= 2;
	}
	if (RngeNursery) nhp *= 2;

	if (nhp) {
		mtmp->mhp += nhp;
		if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = (mtmp->mhpmax += nxtra);

		if (mtmp->bleedout && mtmp->bleedout <= nhp) {
			mtmp->bleedout = 0;
			if (canseemon(mtmp)) pline("%s's bleeding stops.", Monnam(mtmp));
		} else if (mtmp->bleedout) {
			mtmp->bleedout -= nhp;
			if (mtmp->bleedout < 0) mtmp->bleedout = 0; /* should never happen */
			if (canseemon(mtmp)) pline("%s's bleeding diminishes.", Monnam(mtmp));
		}
	}
#if 0
	if(cureblind) ; /* NOT DONE YET */
	if(curesick)  ; /* NOT DONE YET */
#endif 
	return;
}

void
strange_feeling(obj,txt)
register struct obj *obj;
register const char *txt;
{
	if (flags.beginner || !txt)
		You("have a %s feeling for a moment, then it passes.",
		FunnyHallu ? "normal" : "strange");
	else
		pline("%s", txt);

	if(!obj)	/* e.g., crystal ball finds no traps */
		return;

	if(obj->dknown && !objects[obj->otyp].oc_name_known &&
						!objects[obj->otyp].oc_uname)
		docall(obj);
	if (carried(obj) /*&& !(obj->oartifact)*/) useup(obj);
	else /*if (!(obj->oartifact))*/ useupf(obj, 1L);
}

const char *bottlenames[] = {
	"bottle", "phial", "flagon", "carafe", "flask", "jar", "vial"
};

const char *
bottlename()
{
	return bottlenames[rn2(SIZE(bottlenames))];
}

/* WAC -- monsters can throw potions around too! */
void
potionhit(mon, obj, your_fault)
register struct monst *mon; /* Monster that got hit */
register struct obj *obj;
boolean your_fault;
{
	register const char *botlnam = bottlename();
	boolean isyou = (mon == &youmonst);
	int distance;

	if (isyou) {
		if (obj && itemhasappearance(obj, APP_POTION_RESERVATROL) && isok(u.ux, u.uy)) {
			(void) create_gas_cloud(u.ux, u.uy, 3+bcsign(obj), 8+4*bcsign(obj));
			You("smell chemicals.");
		}
		if (obj && itemhasappearance(obj, APP_POTION_NITROGLYCERIN) && isok(u.ux, u.uy)) {
			struct obj *dynamite;
			dynamite = mksobj_at(STICK_OF_DYNAMITE, u.ux, u.uy, TRUE, FALSE, FALSE);
			if (dynamite) {
				if (dynamite->otyp != STICK_OF_DYNAMITE) delobj(dynamite);
				else {
					dynamite->dynamitekaboom = 1;
					dynamite->quan = 1;
					dynamite->owt = weight(dynamite);
					attach_bomb_blow_timeout(dynamite, 0, 0);
				}
			}
		}

	} else {
		if (obj && itemhasappearance(obj, APP_POTION_RESERVATROL) && isok(mon->mx, mon->my)) {
			(void) create_gas_cloud(mon->mx, mon->my, 3+bcsign(obj), 8+4*bcsign(obj));
			You("smell chemicals.");
		}
		if (obj && itemhasappearance(obj, APP_POTION_NITROGLYCERIN) && isok(mon->mx, mon->my)) {
			struct obj *dynamite;
			dynamite = mksobj_at(STICK_OF_DYNAMITE, mon->mx, mon->my, TRUE, FALSE, FALSE);
			if (dynamite) {
				if (dynamite->otyp != STICK_OF_DYNAMITE) delobj(dynamite);
				else {
					dynamite->dynamitekaboom = 1;
					dynamite->quan = 1;
					dynamite->owt = weight(dynamite);
					attach_bomb_blow_timeout(dynamite, 0, 0);
				}
			}
		}

	}

	if (your_fault) u.cnd_potionthrowyoucount++;
	else u.cnd_potionthrowmoncount++;

	if(isyou) {
		distance = 0;
		pline_The("%s crashes on your %s and breaks into shards.",
			botlnam, body_part(HEAD));
		losehp(rnd(2), "thrown potion", KILLED_BY_AN);
	} else {
		distance = distu(mon->mx,mon->my);
		if (!cansee(mon->mx,mon->my)) pline("Crash!");
		else {
		    char *mnam = mon_nam(mon);
		    char buf[BUFSZ];

		    if(has_head(mon->data)) {
			sprintf(buf, "%s %s",
				s_suffix(mnam),
				(notonhead ? "body" : "head"));
		    } else {
			strcpy(buf, mnam);
		    }
		    pline_The("%s crashes on %s and breaks into shards.",
			   botlnam, buf);
		}
		if(rn2(5) && mon->mhp > 1)
			mon->mhp--;
	}

	/* oil doesn't instantly evaporate */
	if (obj->otyp != POT_OIL && cansee(mon->mx,mon->my))
		pline("%s.", Tobjnam(obj, "evaporate"));

    if (isyou) {
	switch (obj->otyp) {
	case POT_OIL:
		if (obj->lamplit)
		    splatter_burning_oil(u.ux, u.uy);
		break;
	case POT_POLYMORPH:
	case POT_MUTATION:
		You_feel("a little %s.", FunnyHallu ? "normal" : "strange");
		if (!Unchanging && !Antimagic) polyself(FALSE);
		break;
	case POT_ACID:
		if (!Acid_resistance || (!StrongAcid_resistance && !rn2(5)) ) {
		    pline("This burns%s!", obj->blessed ? " a little" :
				    obj->cursed ? " a lot" : "");
		    losehp(d(obj->cursed ? 2 : 1, obj->blessed ? 4 : 8),
				    "potion of acid", KILLED_BY_AN);
		/* these things are way too weak and don't scale with monster difficulty at all! Let's change this! --Amy */
			if (obj->blessed) losehp(rnd( (monster_difficulty() + 3) / 4), "potion of acid", KILLED_BY_AN);
			else if (obj->cursed) losehp(rnd( (monster_difficulty() + 3) / 2), "potion of acid", KILLED_BY_AN);
			else losehp(rnd( (monster_difficulty() + 3) / 3), "potion of acid", KILLED_BY_AN);

		}
		if (evilfriday) {
			register struct obj *objX, *objX2;
			for (objX = invent; objX; objX = objX2) {
				objX2 = objX->nobj;
				if (!rn2(5)) rust_dmg(objX, xname(objX), 3, TRUE, &youmonst);
			}
		}

		if (Stoned) fix_petrification();
		break;
	case POT_AMNESIA:
		/* Uh-oh! */
		/* Amy edit: it's stupid if it's basically guaranteed that your helmet loses a point of enchantment. */
		/* In Soviet Russia, amnesia potions are as abundant as vodka, and no one needs enchanted helmets anyway
		 * because people are naturally cold resistant there. So they're throwing wild parties where they throw around
		 * those potions of sparkling champagne just for fun, not caring whether it disenchants people's helmets. */
		if (uarmh && is_helmet(uarmh) && (!rn2(3) || issoviet) && rn2(10 - (uarmh->cursed ? 8 : 0))) {
			get_wet(uarmh, TRUE);
			if (issoviet) pline("Da, vash shlem razocharovalsya! Da uzh! Tak tebe i nado!");
		}
		break;
	}
    } else {
	boolean angermon = TRUE;

	if (!your_fault) angermon = FALSE;
	switch (obj->otyp) {
	case POT_HEALING:
 do_healing:
		if (mon->data == &mons[PM_PESTILENCE]) goto do_illness;
		angermon = FALSE;
		if (canseemon(mon))
			pline("%s looks better.", Monnam(mon));
		healup_mon(mon, d(5,6) + rnz(u.ulevel) + 5 * bcsign(obj),
			!obj->cursed ? 1 : 0, 1+1*!!obj->blessed, !obj->cursed);
		break;
	case POT_EXTRA_HEALING:
		if (mon->data == &mons[PM_PESTILENCE]) goto do_illness;
		angermon = FALSE;
		if (canseemon(mon))
			pline("%s looks much better.", Monnam(mon));
		healup_mon(mon, d(6,8) + rnz(u.ulevel) + 5 * bcsign(obj),
			obj->blessed ? 5 : !obj->cursed ? 2 : 0,
			!obj->cursed, TRUE);
		break;
	case POT_FULL_HEALING:
		if (mon->data == &mons[PM_PESTILENCE]) goto do_illness;
		/*FALLTHRU*/
	case POT_RESTORE_ABILITY:
	case POT_GAIN_ABILITY:
		angermon = FALSE;
		    if (canseemon(mon))
			pline("%s looks sound and hale again.", Monnam(mon));
		healup_mon(mon, 400 + rnz(u.ulevel), 5+5*!!(obj->blessed), !(obj->cursed), 1);
		break;
	case POT_CURE_WOUNDS:
		if (mon->data == &mons[PM_PESTILENCE]) goto do_illness;
		angermon = FALSE;
		if (canseemon(mon)) pline("%s looks better.", Monnam(mon));
		healup_mon(mon, d(5,6) + rnz(u.ulevel) + 5 * bcsign(obj), 0, 0, 0);
		break;
	case POT_CURE_SERIOUS_WOUNDS:
		if (mon->data == &mons[PM_PESTILENCE]) goto do_illness;
		angermon = FALSE;
		if (canseemon(mon)) pline("%s looks much better.", Monnam(mon));
		healup_mon(mon, d(6,8) + rnz(u.ulevel) + 5 * bcsign(obj), 0, 0, 0);
		break;
	case POT_CURE_CRITICAL_WOUNDS:
		if (mon->data == &mons[PM_PESTILENCE]) goto do_illness;
		angermon = FALSE;
		if (canseemon(mon)) pline("%s looks sound and hale again.", Monnam(mon));
		healup_mon(mon, 400 + rnz(u.ulevel),  0, 0, 0);
		break;
	case POT_SICKNESS:
	case POT_POISON:
		if (mon->data == &mons[PM_PESTILENCE]) goto do_healing;
		if (dmgtype(mon->data, AD_DISE) ||
			   dmgtype(mon->data, AD_PEST) || /* won't happen, see prior goto */
			   resists_poison(mon)) {
		    if (canseemon(mon))
			pline("%s looks unharmed.", Monnam(mon));
		    break;
		}
 do_illness:
		if (canseemon(mon))
		    pline("%s looks rather ill.", Monnam(mon));
		if((mon->mhpmax > 3) && !resist(mon, POTION_CLASS, 0, NOTELL))
			mon->mhpmax /= 2;
		if((mon->mhp > 2) && !resist(mon, POTION_CLASS, 0, NOTELL))
			mon->mhp /= 2;
		if (mon->mhp > mon->mhpmax) mon->mhp = mon->mhpmax;
		break;
	case POT_CYANIDE:

		if (practicantterror && your_fault) {
			pline("%s thunders: 'This is absolutely intolerable! A first-semester practicant like you is still light years from receiving clearance for using such highly toxic substances! You not only have to pay a fine of 10000 zorkmids, I'm also giving you hall exclusion. Do not come back to the laboratory today, you hear?'", noroelaname());
			fineforpracticant(10000, 0, 0);

			if (((u.uevent.udemigod || u.uhave.amulet) && !u.freeplaymode) || CannotTeleport || (u.usteed && mon_has_amulet(u.usteed))) {
				pline("You shudder for a moment.");
				(void) safe_teleds(FALSE);
			} else if (playerlevelportdisabled()) { 
				pline("Somehow, the hall exclusion doesn't do anything.");
				break;
			} else {
				u.banishmentbeam = 1;
				nomul(-2, "having received hall exclusion", FALSE); /* because it's not called until you get another turn... */
			}

		}

		if (!resist(mon,POTION_CLASS,0,NOTELL)) {
			if (canseemon(mon)) {
				pline("%s looks deathly sick.", Monnam(mon));
			}
			if (mon->mhp < 10) {
				killed(mon);
			} else {
				mon->mhp /= 5;
				if (mon->mhp < 1) killed(mon);
			}

		}
		break;
	case POT_RADIUM:
	case POT_SLIME:	/* too lazy to code something else :D --Amy */
		if (!resist(mon,POTION_CLASS,0,NOTELL)) {
			if (canseemon(mon)) pline("%s looks very sick.",Monnam(mon));
			mon->mhp /= 4;
			if (mon->mhp < 1) killed(mon);
		}
		break;
	case POT_STUNNING:
	case POT_NUMBNESS: /* both of these will stun the monster --Amy */
		if(!resist(mon, POTION_CLASS, 0, NOTELL))  {
			mon->mstun = TRUE;
			if (canseemon(mon)) pline("%s trembles.",Monnam(mon));
		}
		break;

	case POT_CANCELLATION:

		(void) cancel_monst(mon, obj, TRUE, TRUE, FALSE);

		break;

	case POT_ICE: /* there's no frozen monster state, so we'll just slow down the monster --Amy */
		if(!resist(mon, POTION_CLASS, 0, NOTELL))  {
			mon_adjust_speed(mon, -2, (struct obj *)0 );
			if (canseemon(mon)) pline("%s slows to a crawl.",Monnam(mon));
		}
		break;

	case POT_DIMNESS:
	case POT_FEAR: /* monster flees --Amy */
		if(!is_undead(mon->data) && (!mon->egotype_undead) && !resist(mon, POTION_CLASS, 0, NOTELL))  {
		     monflee(mon, rnd(10), FALSE, TRUE);
			if (canseemon(mon)) pline("%s is panicking!",Monnam(mon));
		}
		break;

	case POT_FIRE: /* there's no burned monster state, so we'll take off some max HP instead --Amy */
		if(!resist(mon, POTION_CLASS, 0, NOTELL))  {
		mon->mhp -= rnd(3);
		mon->mhpmax -= rnd(3);
		if (mon->mhp > mon->mhpmax) mon->mhp = mon->mhpmax;
		if (canseemon(mon)) pline("%s is burned a little.",Monnam(mon));
		if (mon->mhp < 1 || mon->mhpmax < 1) monkilled(mon, "", AD_FIRE);;

		}

		break;

	case POT_CONFUSION:
	case POT_BOOZE:
	case POT_WINE:
		if(!resist(mon, POTION_CLASS, 0, NOTELL))  {mon->mconf = TRUE;
			if (canseemon(mon)) pline("%s is confused.",Monnam(mon));

		}
		break;
#if 0 /* NH 3.3.0 version */
	case POT_POLYMORPH:
		(void) bhitm(mon, obj);
		break;
#endif
	case POT_POLYMORPH:
	    /* [Tom] polymorph potion thrown 
	     * [max] added poor victim a chance to resist 
	     * magic resistance protects from polymorph traps, so make
	     * it guard against involuntary polymorph attacks too... 
	     */
	    if (resists_magm(mon)) {
                shieldeff(mon->mx, mon->my);
	    } else if (!resist (mon, POTION_CLASS, 0, NOTELL)) {
                mon_poly(mon, your_fault, "%s changes!");
                if (!Hallucination && canspotmon (mon))
                                makeknown (POT_POLYMORPH);
	    }
  		break;
	case POT_MUTATION:

		add_monster_egotype(mon);

  		break;
	case POT_INVISIBILITY:
		angermon = FALSE;
		mon_set_minvis(mon);
		break;
	case POT_SLEEPING:
		/* wakeup() doesn't rouse victims of temporary sleep */
		if (sleep_monst(mon, rnd(12), POTION_CLASS)) {
		    pline("%s falls asleep.", Monnam(mon));
		    slept_monst(mon);
		}
		break;
	case POT_PARALYSIS:

		/* monsters with paralysis attacks are immune now --Amy */
		if (mon->mcanmove && !(dmgtype(mon->data, AD_PLYS))) {
			mon->mcanmove = 0;
			/* really should be rnd(5) for consistency with players
			 * breathing potions, but...
			 */
			mon->mfrozen = rnd(25);
		}
		break;
	case POT_SPEED:
		angermon = FALSE;
		mon_adjust_speed(mon, 1, obj);
		break;
	case POT_BLINDNESS:
		if(haseyes(mon->data)) {
		    register int btmp = 64 + rn2(32) +
			rn2(32) * !resist(mon, POTION_CLASS, 0, NOTELL);
		    btmp += mon->mblinded;
		    mon->mblinded = min(btmp,127);
		    mon->mcansee = 0;
		}
		break;
	case POT_WATER:
		if (is_undead(mon->data) || is_demon(mon->data) ||
			is_were(mon->data)) {
		    if (obj->blessed) {
			pline("%s %s in pain!", Monnam(mon),
			      is_silent(mon->data) ? "writhes" : "shrieks");
			mon->mhp -= d(2,6);
			if (mon->mhp < 1) {
			    if (your_fault)
				killed(mon);
			    else
				monkilled(mon, "", AD_ACID);
			}
			else if (is_were(mon->data) && !is_human(mon->data))
			    new_were(mon);	/* revert to human */
		    } else if (obj->cursed) {
			angermon = FALSE;
			if (canseemon(mon))
			    pline("%s looks healthier.", Monnam(mon));
			mon->mhp += d(2,6);
			if (mon->mhp > mon->mhpmax) mon->mhp = mon->mhpmax;
			if (is_were(mon->data) && is_human(mon->data) &&
				!Protection_from_shape_changers)
			    new_were(mon);	/* transform into beast */
		    }
		} else if (splittinggremlin(mon->data)) {
		    angermon = FALSE;
		    (void)split_mon(mon, (struct monst *)0);
		} else if(mon->data == &mons[PM_FLAMING_SPHERE] ||
			mon->data == &mons[PM_IRON_GOLEM]) {
		    if (canseemon(mon))
			pline("%s %s.", Monnam(mon),
				mon->data == &mons[PM_IRON_GOLEM] ?
				"rusts" : "flickers");
		    mon->mhp -= d(1,6);
		    if (mon->mhp < 1) {
			if (your_fault)
			    killed(mon);
			else
			    monkilled(mon, "", AD_ACID);
		    }
		}
		break;
	case POT_AMNESIA:
		if (mon->mfrenzied) mon->mfrenzied = 0;
		if (splittinggremlin(mon->data)) {
			/* Gremlins multiply... */
			mon->mtame = FALSE;	
			(void)split_mon(mon, (struct monst *)0);
		}
		switch (monsndx(mon->data)) {
		case PM_FLAMING_SPHERE:
		case PM_IRON_GOLEM:
		    if (canseemon(mon)) pline("%s %s.", Monnam(mon),
			    monsndx(mon->data) == PM_IRON_GOLEM ?
			    "rusts" : "flickers");
		    mon->mhp -= d(1,6);
		    if (mon->mhp < 1)
			if (your_fault)
			    killed(mon);
			else
			    monkilled(mon, "", AD_ACID);
		    else
			mon->mtame = FALSE;	
		    break;
		case PM_WIZARD_OF_YENDOR:
		    if (your_fault) {
			if (canseemon(mon)) 
			    pline("%s laughs at you!", Monnam(mon));
			forget(1);
		    }
		    break;

		/* Adding some actually useful effects to all that flavor talk. --Amy */

		case PM_MEDUSA:
		    if (canseemon(mon))
			pline("%s looks like %s's having a bad hair day!", 
					Monnam(mon), mhe(mon));
		    if (obj->blessed) cancelmonsterlite(mon);
		    break;
		case PM_CROESUS:
		    if (canseemon(mon))
		        pline("%s says: 'My gold! I must count my gold!'", 
					Monnam(mon));
		    if (obj->blessed) mon->mconf = TRUE;
		    break;
		case PM_DEATH:
 		    if (canseemon(mon))
		        pline("%s pauses, then looks at you thoughtfully!", 
					Monnam(mon));
		    if (obj->blessed && mon->m_lev > 0) mon->m_lev--;
		    break;
		case PM_FAMINE:
		    if (canseemon(mon))
		        pline("%s looks unusually hungry!", Monnam(mon));
		    if (obj->blessed && mon->m_lev > 0) mon->m_lev--;
		    break;
		case PM_PESTILENCE:
		    if (canseemon(mon))
		        pline("%s looks unusually well!", Monnam(mon));
		    if (obj->blessed && mon->m_lev > 0) mon->m_lev--;
		    break;
		case PM_FRUSTRATION:
		    if (canseemon(mon))
		        pline("%s looks like something terrible happened to him!", Monnam(mon));
		    if (obj->blessed && mon->m_lev > 0) mon->m_lev--;
		    break;
		default:
		    if (mon->data->msound == MS_NEMESIS && canseemon(mon)
				    && your_fault) {
			pline("%s curses your ancestors!", Monnam(mon));
		      if (obj->blessed && mon->m_lev > 0) mon->m_lev--;
		      mon->mstun = TRUE;
		    } else if (mon->isshk) {
			angermon = FALSE;
			if (canseemon(mon))
			    pline("%s looks at you curiously!", 
					    Monnam(mon));
			make_happy_shk(mon, FALSE);
		    } else if (!is_covetous(mon->data) && !rn2(4) && !(splittinggremlin(mon->data)) && !resist(mon, POTION_CLASS, 0, 0)) {
			angermon = FALSE;
			if (canseemon(mon)) {
			    if (mon->msleeping) {
				wakeup(mon);
				pline("%s wakes up looking bewildered!", 
						Monnam(mon));
			    } else
				pline("%s looks bewildered!", Monnam(mon));
			    if (!mon->mfrenzied) mon->mpeaceful = TRUE;
			    mon->mtame = FALSE;	
			}
		    }
		    break;
		}
		break;
	case POT_OIL:
		if (obj->lamplit)
			splatter_burning_oil(mon->mx, mon->my);
		break;
/*
	case POT_GAIN_LEVEL:
	case POT_LEVITATION:
	case POT_FRUIT_JUICE:
	case POT_MONSTER_DETECTION:
	case POT_OBJECT_DETECTION:
		break;
*/
	/* KMH, balance patch -- added */
	case POT_ACID:
		if (!resists_acid(mon) && !resist(mon, POTION_CLASS, 0, NOTELL)) {
		    pline("%s %s in pain!", Monnam(mon),
			  is_silent(mon->data) ? "writhes" : "shrieks");
		    mon->mhp -= d(obj->cursed ? 2 : 1, obj->blessed ? 4 : 8);
		    if (mon->mhp < 1) {
			if (your_fault)
			    killed(mon);
			else
			    monkilled(mon, "", AD_ACID);
		    }
		}
		break;
	}
	if (angermon)
	    wakeup(mon);
	else
	    mon->msleeping = 0;
    }

	/* Note: potionbreathe() does its own docall() */
	if ((distance==0 || ((distance < 3) && rn2(5))) &&
	    (!breathless(youmonst.data) || haseyes(youmonst.data)))
		potionbreathe(obj);
	else if (obj->dknown && !objects[obj->otyp].oc_name_known &&
		   !objects[obj->otyp].oc_uname && cansee(mon->mx,mon->my))
		docall(obj);
	if(*u.ushops && obj->unpaid) {
	        register struct monst *shkp =
			shop_keeper(*in_rooms(u.ux, u.uy, SHOPBASE));

		if(!shkp)
		    obj->unpaid = 0;
		else {
		    (void)stolen_value(obj, u.ux, u.uy,
				 (boolean)shkp->mpeaceful, FALSE, TRUE);
		    subfrombill(obj, shkp);
		}
	}
	obfree(obj, (struct obj *)0);
}

/* vapors are inhaled or get in your eyes */
void
potionbreathe(obj)
register struct obj *obj;
{
	register int i, ii, isdone, kn = 0;

	if (itemhasappearance(obj, APP_POTION_SUBCLINICAL) || itemhasappearance(obj, APP_POTION_ABBREVIATOR) || itemhasappearance(obj, APP_POTION_MULLIONING) || itemhasappearance(obj, APP_POTION_DISLODGING) || itemhasappearance(obj, APP_POTION_REMONSTRATED) || itemhasappearance(obj, APP_POTION_CINERARIA) || itemhasappearance(obj, APP_POTION_SPURTED) || itemhasappearance(obj, APP_POTION_BASSETED)) {
		if (breathless(youmonst.data)) pline("Some weird substance gets into your %s!", body_part(EYE));
		else pline("Some weird stench gets into your %s!", body_part(NOSE));
		badeffect();
	}

	switch(obj->otyp) {
	case POT_RESTORE_ABILITY:
	case POT_GAIN_ABILITY:
		if(obj->cursed) {
		    if (!breathless(youmonst.data))
			pline("Ulch!  That potion smells terrible!");
		    else if (haseyes(youmonst.data)) {
			int numeyes = eyecount(youmonst.data);
			Your("%s sting%s!",
			     (numeyes == 1) ? body_part(EYE) : makeplural(body_part(EYE)),
			     (numeyes == 1) ? "s" : "");
		    }
		    if (evilfriday) {
			adjattrib(rn2(A_MAX), -1, 0, TRUE);
		    }
		    break;
		} else {
		    i = rn2(A_MAX);		/* start at a random point */
		    for(isdone = ii = 0; !isdone && ii < A_MAX; ii++) {
			if(ABASE(i) < AMAX(i)) {
			    ABASE(i)++;
			    /* only first found if not blessed */
			    isdone = !(obj->blessed);
			    flags.botl = 1;
			}
			if(++i >= A_MAX) i = 0;
		    }
		}
		break;
	case POT_FULL_HEALING:
	case POT_CURE_CRITICAL_WOUNDS:
		if (Upolyd && u.mh < u.mhmax) u.mh++, flags.botl = 1;
		if (u.uhp < u.uhpmax) u.uhp++, flags.botl = 1;
		/*FALL THROUGH*/
	case POT_EXTRA_HEALING:
	case POT_CURE_SERIOUS_WOUNDS:
		if (Upolyd && u.mh < u.mhmax) u.mh++, flags.botl = 1;
		if (u.uhp < u.uhpmax) u.uhp++, flags.botl = 1;
		/*FALL THROUGH*/
	case POT_HEALING:
	case POT_CURE_WOUNDS:
		if (Upolyd && u.mh < u.mhmax) u.mh++, flags.botl = 1;
		if (u.uhp < u.uhpmax) u.uhp++, flags.botl = 1;
		exercise(A_CON, TRUE);
		break;
	case POT_SICKNESS:
	case POT_POISON:
		if (!Role_if(PM_HEALER) && !Race_if(PM_HERBALIST)) {
			if (Upolyd) {
			    if (u.mh <= 5) u.mh = 1; else u.mh -= 5;
			} else {
			    if (u.uhp <= 5) u.uhp = 1; else u.uhp -= 5;
			}
			flags.botl = 1;
			exercise(A_CON, FALSE);
		}
		break;
	case POT_CYANIDE:
		if (u.uhp < 10) {
			/* DEATH */
			losehp(10,"a potion of cyanide",KILLED_BY);
		} else {
			u.uhp -= ((u.uhp / 3) + 1);
			if (u.uhp < 1) u.uhp = 1;		/* be generous */
		}
		flags.botl = 1;
		You_feel("very, very sick!");
		break;
	case POT_URINE:
		if (u.uhp < 3) {
			/* DEATH */
			losehp(3,"a potion of urine",KILLED_BY);
		} else {
			u.uhp -= ((u.uhp / 5) + 1);
			if (u.uhp < 1) u.uhp = 1;		/* be generous */
		}
		flags.botl = 1;
		You_feel("very poisoned!");
		break;
	case POT_RADIUM:
		u.uhp -= ((u.uhp / 4) + 1);
		if (u.uhp < 1) u.uhp = 1;		/* be generous */
		flags.botl = 1;
		You_feel("very sick!");
		break;
	case POT_HALLUCINATION:
		You("have a momentary vision.");

		(void) make_hallucinated(itimeout_incr(HHallucination,
					   rnd(30)), TRUE, 0L);
		break;
	case POT_CONFUSION:
	case POT_BOOZE:
	case POT_WINE:
		if(!Confusion)
			You_feel("somewhat dizzy.");
		make_confused(itimeout_incr(HConfusion, rnd(15)), FALSE);
		break;
	case POT_ICE:
		if(!Frozen)
			pline("You're suddenly shockfrosted!");
		make_frozen(itimeout_incr(HFrozen, rnd(15)), FALSE);
		break;
	case POT_FEAR:
		if(!Feared)
			pline("Your knees suddenly become softer!");
		make_feared(itimeout_incr(HFeared, rnd(50)), FALSE);
		break;
	case POT_FIRE:
		if(!Burned)
			pline("You caught fire!");
		make_burned(itimeout_incr(HBurned, rnd(30)), FALSE);
		break;
	case POT_SANITY:
		increasesanity(rnd((level_difficulty() * 5) + 20));
		break;
	case POT_DIMNESS:
		if(!Dimmed)
			pline("You are dimmed!");
		make_dimmed(itimeout_incr(HDimmed, rnd(30)), FALSE);
		break;
	case POT_STUNNING:
		if(!Stunned)
			pline("You stagger a bit...");
		make_stunned(itimeout_incr(HStun, rnd(15)), FALSE);
		break;
	case POT_NUMBNESS:
		if(!Numbed)
			pline("You're feeling a little numb!");
		make_numbed(itimeout_incr(HNumbed, rnd(30)), FALSE);
		break;
	case POT_CANCELLATION:
	      (void) cancel_monst(&youmonst, obj, FALSE, TRUE, FALSE);
		break;
	case POT_SLIME:
		if (!Slimed && !flaming(youmonst.data) && !Unchanging && !slime_on_touch(youmonst.data) ) {
		    You("don't feel very well.");
			stop_occupation();
			make_slimed(100);
		    killer_format = KILLED_BY_AN;
		    delayed_killer = "slimed by a potion of slime";
		}
		break;

	case POT_INVISIBILITY:
		if (!Blind && !Invis) {
		    kn++;
		    pline("For an instant you %s!",
			See_invisible ? "could see right through yourself"
			: "couldn't see yourself");
		}
		break;
	case POT_PARALYSIS:
		kn++;
		if (!Free_action || !rn2(StrongFree_action ? 100 : 20)) {
		    pline("%s seems to be holding you.", Something);
			if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
		    nomul(-rnd(5), "frozen by breathing a potion", TRUE);
		    nomovemsg = You_can_move_again;
		    exercise(A_DEX, FALSE);
		} else You("stiffen momentarily.");
		break;
	case POT_SLEEPING:
		kn++;
		if ((!Free_action && !Sleep_resistance) || !rn2(StrongSleep_resistance ? 20 : 5)) {
		    You_feel("rather tired.");
		    nomul(-rnd(5), "sleeping off a magical draught", TRUE);
		    nomovemsg = You_can_move_again;
		    exercise(A_DEX, FALSE);
		} else You("yawn.");
		break;
	case POT_SPEED:
		if (!Fast) Your("knees seem more flexible now.");
		incr_itimeout(&HFast, rnd(5));
		exercise(A_DEX, TRUE);
		break;
	case POT_BLINDNESS:
		if (!Blind && !u.usleep) {
		    kn++;
		    pline("It suddenly gets dark.");
		}
		make_blinded(itimeout_incr(Blinded, rnd(20)), FALSE);
		if (!Blind && !u.usleep) Your("%s", vision_clears);
		break;
	case POT_WATER:
		if (splittinggremlin(youmonst.data)) {
		    (void)split_mon(&youmonst, (struct monst *)0);
		} else if (u.ulycn >= LOW_PM) {
		    /* vapor from [un]holy water will trigger
		       transformation but won't cure lycanthropy */
		    if (obj->blessed && youmonst.data == &mons[u.ulycn])
			you_unwere(FALSE);
		    else if (obj->cursed && !Upolyd)
			you_were();
		}
		break;
	case POT_AMNESIA:
		if (splittinggremlin(youmonst.data)) {
		    (void)split_mon(&youmonst, (struct monst *)0);
		} else if(u.umonnum == PM_FLAMING_SPHERE) {
		    You("flicker!");
		    losehp(d(1,6),"potion of amnesia", KILLED_BY_AN);
		} else if(u.umonnum == PM_IRON_GOLEM) {
		    You("rust!");
		    losehp(d(1,6),"potion of amnesia", KILLED_BY_AN);
		}
		You_feel("dizzy!");
		forget(1 + rn2(5));
		break;
	case POT_ACID:
		if (Stoned) fix_petrification();
	case POT_POLYMORPH:
	case POT_MUTATION:
		exercise(A_CON, FALSE);
		break;
	case POT_BLOOD:
	case POT_VAMPIRE_BLOOD:
		if (maybe_polyd(is_vampire(youmonst.data), Race_if(PM_VAMPIRE))) {
		    exercise(A_WIS, FALSE);
		    You_feel("a %ssense of loss.",
		      obj->otyp == POT_VAMPIRE_BLOOD ? "terrible " : "");
		} else
		    exercise(A_CON, FALSE);
		break;
	case POT_SALT_WATER:

		/* supposed to be thrown by cleaner assholes if you're a snail --Amy */
		if (Race_if(PM_ELONA_SNAIL) && !Upolyd) {
			pline("It's salt! you start to melt."); /* capitalization is NOT an error --Amy */
			losehp(((u.uhpmax < 10) ? 9999 : (u.uhpmax - rnd(10))), "a potion of salt solution", KILLED_BY);
			break;
		}
		break;

/*
	case POT_GAIN_LEVEL:
	case POT_LEVITATION:
	case POT_FRUIT_JUICE:
	case POT_MONSTER_DETECTION:
	case POT_OBJECT_DETECTION:
	case POT_OIL:
		break;
*/
	}
	/* note: no obfree() */
	if (obj->dknown) {
	    if (kn)
		makeknown(obj->otyp);
	    else if (!objects[obj->otyp].oc_name_known &&
						!objects[obj->otyp].oc_uname && !Blind)
		docall(obj);
	}
}

STATIC_OVL short
mixtype(o1, o2)
register struct obj *o1, *o2;
/* returns the potion type when o1 is dipped in o2 */
{
	/* cut down on the number of cases below */
	if (o1->oclass == POTION_CLASS &&
	    (o2->otyp == POT_GAIN_LEVEL ||
	     o2->otyp == POT_GAIN_ENERGY ||
	     o2->otyp == POT_HEALING ||
	     o2->otyp == POT_EXTRA_HEALING ||
	     o2->otyp == POT_FULL_HEALING ||
	     o2->otyp == POT_ENLIGHTENMENT ||
	     o2->otyp == POT_FRUIT_JUICE)) {
		struct obj *swp;

		swp = o1; o1 = o2; o2 = swp;
	}

	switch (o1->otyp) {
		case POT_HEALING:
			switch (o2->otyp) {
			    case POT_SPEED:
			    case POT_GAIN_LEVEL:
			    case POT_GAIN_ENERGY:
				return POT_EXTRA_HEALING;
			}
			break;
		case POT_EXTRA_HEALING:
			switch (o2->otyp) {
			    case POT_GAIN_LEVEL:
			    case POT_GAIN_ENERGY:
				return POT_FULL_HEALING;
			}
			break;
		case POT_FULL_HEALING:
			switch (o2->otyp) {
			    case POT_GAIN_LEVEL:
			    case POT_GAIN_ENERGY:
				return POT_GAIN_ABILITY;
			}
			break;
		case UNICORN_HORN:
		case DARK_HORN:
		case SKY_HORN:
		case ARCANE_HORN:
			switch (o2->otyp) {
			    case POT_SICKNESS:
				return POT_FRUIT_JUICE;
			    case POT_HALLUCINATION:
			    case POT_BLINDNESS:
			    case POT_NUMBNESS:
			    case POT_STUNNING:
			    case POT_ICE:
			    case POT_FIRE:
			    case POT_FEAR:
			    case POT_DIMNESS:
			    case POT_CONFUSION:
			    case POT_BLOOD:
			    case POT_VAMPIRE_BLOOD:
				return POT_WATER;
			}
			break;
		case AMETHYST:		/* "a-methyst" == "not intoxicated" */
			if (o2->otyp == POT_BOOZE)
			    return POT_FRUIT_JUICE;
			break;
		case POT_GAIN_LEVEL:
		case POT_GAIN_ENERGY:
			switch (o2->otyp) {
			    case POT_CONFUSION:
				return (rn2(3) ? POT_BOOZE : POT_ENLIGHTENMENT);
			    case POT_HEALING:
				return POT_EXTRA_HEALING;
			    case POT_EXTRA_HEALING:
				return POT_FULL_HEALING;
			    case POT_FULL_HEALING:
				return POT_GAIN_ABILITY;
			    case POT_FRUIT_JUICE:
				return POT_SEE_INVISIBLE;
			    case POT_BOOZE:
				return POT_HALLUCINATION;
			}
			break;
		case POT_FRUIT_JUICE:
			switch (o2->otyp) {
			    case POT_SICKNESS:
				return POT_SICKNESS;
			    case POT_BLOOD:
				return POT_BLOOD;
			    case POT_VAMPIRE_BLOOD:
				return POT_VAMPIRE_BLOOD;
			    case POT_SPEED:
				return POT_BOOZE;
			    case POT_GAIN_LEVEL:
			    case POT_GAIN_ENERGY:
				return POT_SEE_INVISIBLE;
			}
			break;
		case POT_ENLIGHTENMENT:
			switch (o2->otyp) {
			    case POT_LEVITATION:
				if (rn2(3)) return POT_GAIN_LEVEL;
				break;
			    case POT_FRUIT_JUICE:
				return POT_BOOZE;
			    case POT_BOOZE:
				return POT_CONFUSION;
			}
			break;
	}
	/* MRKR: Extra alchemical effects. */

	if (o2->otyp == POT_ACID && o1->oclass == GEM_CLASS) {
	  const char *potion_descr;

	  /* Note: you can't create smoky, milky or clear potions */

	  switch (o1->otyp) {

	    /* white */

	  case DILITHIUM_CRYSTAL:
	    /* explodes - special treatment in dodip */
	    /* here we just want to return something non-zero */
	    return POT_WATER;
	    break;
	  case DIAMOND:
	    /* won't dissolve */
	    potion_descr = NULL;
	    break;
	  case OPAL:
	    potion_descr = "cloudy";
	    break;

	    /* red */

	  case RUBY:
	    potion_descr = "ruby";
	    break;
	  case GARNET:
	    potion_descr = "pink";
	    break;
	  case JASPER:
	    potion_descr = "purple-red";
	    break;

	    /* orange */

	  case JACINTH:
	    potion_descr = "orange";
	    break;
	  case AGATE:
	    potion_descr = "swirly";
	    break;

	    /* yellow */

	  case CITRINE:
	    potion_descr = "yellow";
	    break;
	  case CHRYSOBERYL:
	    potion_descr = "golden";
	    break;

	    /* yellowish brown */

	  case AMBER:
	    potion_descr = "brown";
	    break;
	  case TOPAZ:
	    potion_descr = "murky";
	    break;

	    /* green */

	  case EMERALD:
	    potion_descr = "emerald";
	    break;
	  case TURQUOISE:
	    potion_descr = "sky blue";
	    break;
	  case AQUAMARINE:
	    potion_descr = "cyan";
	    break;
	  case JADE:
	    potion_descr = "dark green";
	    break;

	    /* blue */

	  case SAPPHIRE:
	    potion_descr = "brilliant blue";
	    break;

	    /* violet */

	  case AMETHYST:
	    potion_descr = "magenta";
	    break;
	  case FLUORITE:
	    potion_descr = "white";
	    break;

	    /* black */

	  case BLACK_OPAL:
	    potion_descr = "black";
	    break;
	  case JET:
	    potion_descr = "dark";
	    break;
	  case OBSIDIAN:
	    potion_descr = "effervescent";
	    break;

		/* more by Amy */
	  case CIRMOCLINE:
	    potion_descr = "thaumaturgic";
	    break;
	  case DIOPTASE:
	    potion_descr = "shining";
	    break;
	  case MORGANITE:
	    potion_descr = "incandescent";
	    break;
	  case ORTHOCLASE:
	    potion_descr = "freezing";
	    break;
	  case ANDALUSITE:
	    potion_descr = "squishy";
	    break;
	  case PERIDOT:
	    potion_descr = "greasy";
	    break;
	  case CHAROITE:
	    potion_descr = "slimy";
	    break;
	  case AMAZONITE:
	    potion_descr = "soapy";
	    break;
	  case SODALITH:
	    potion_descr = "ochre";
	    break;
	  case VIVIANITE:
	    potion_descr = "steamy";
	    break;
	  case EPIDOTE:
	    potion_descr = "gooey";
	    break;
	  case CHALCEDON:
	    potion_descr = "silver";
	    break;
	  case CHRYSOCOLLA:
	    potion_descr = "dangerous";
	    break;
	  case MALACHITE:
	    potion_descr = "creepy";
	    break;
	  case COVELLINE:
	    potion_descr = "distilled";
	    break;
	  case ROSE_QUARTZ:
	    potion_descr = "warping";
	    break;
	  case KUNZITE:
	    potion_descr = "unnatural";
	    break;
	  case SPINEL:
	    potion_descr = "venomous";
	    break;
	  case CYAN_STONE:
	    potion_descr = "colorless";
	    break;
	  case LAPIS_LAZULI:
	    potion_descr = "alchemical";
	    break;
	  case MOONSTONE:
	    potion_descr = "fluorescent";
	    break;
	  case PREHNITE:
	    potion_descr = "illuminated";
	    break;
	  case DISTHENE:
	    potion_descr = "bright";
	    break;
	  case APOPHYLLITE:
	    potion_descr = "carcinogenic";
	    break;
	  case TOURMALINE:
	    potion_descr = "liquid";
	    break;
	  case RHODOCHROSITE:
	    potion_descr = "buzzing";
	    break;
	  case ANHYDRITE:
	    potion_descr = "concocted";
	    break;
	  case HALITE:
	    potion_descr = "blue";
	    break;
	  case MARBLE:
	    potion_descr = "gray";
	    break;
	  case MORION:
	    potion_descr = "plaid";
	    break;

	  default: potion_descr = NULL;
	  }

	  if (potion_descr) {
	    int typ;

	    /* find a potion that matches the description */

	    for (typ = bases[POTION_CLASS];
		 objects[typ].oc_class == POTION_CLASS;
		 typ++) {

	      if (strcmp(potion_descr, OBJ_DESCR(objects[typ])) == 0 && OBJ_NAME(objects[typ]) ) {
		/* preventing potion descriptions that don't match any really existing potion types from generating --Amy */
		return typ;
	      }
	    }
	  }
	}

	return 0;
}

/* Bills an object that's about to be downgraded, assuming that's not already
 * been done */
STATIC_OVL
void
pre_downgrade_obj(obj, used)
register struct obj *obj;
boolean *used;
{
    boolean dummy = FALSE;

    if (!used) used = &dummy;
    if (!*used) Your("%s for a moment.", aobjnam(obj, "sparkle"));
    if(obj->unpaid && costly_spot(u.ux, u.uy) && !*used) {
	You("damage it, you pay for it.");
	bill_dummy_object(obj);
    }
    *used = TRUE;
}

/* Implements the downgrading effect of potions of amnesia and Lethe water */
STATIC_OVL
void
downgrade_obj(obj, nomagic, used)
register struct obj *obj;
int nomagic;	/* The non-magical object to downgrade to */
boolean *used;
{
    pre_downgrade_obj(obj, used);
    obj->otyp = nomagic;
    obj->spe = 0;
    obj->owt = weight(obj);
    flags.botl = TRUE;
}

boolean
get_wet(obj, amnesia)
register struct obj *obj;
boolean amnesia;
/* returns TRUE if something happened (potion should be used up) */
{
	char Your_buf[BUFSZ];
	struct obj* otmp;
	boolean used = FALSE;

	if (uarmf && obj == uarmf && itemhasappearance(uarmf, APP_YELLOW_SNEAKERS) ) {
		pline("Urgh, your yellow sneakers hate getting wet!");
		nomul(-rnd(20), "getting their yellow sneakers wet", TRUE);
		losehp(rnd(10), "endangering their yellow sneakers", KILLED_BY);
	}

	if (stack_too_big(obj)) {
		return(FALSE);
	}

	if (obj && obj->oartifact == ART_ELIANE_S_SHIN_SMASH) {
		pline("The liquid destroys your footwear instantly.");
		useup(obj);
		return(TRUE);
	}

	if (snuff_lit(obj)) return(TRUE);

	if (obj->greased) {
		grease_protect(obj,(char *)0,&youmonst);
		return(FALSE);
	}
	(void) Shk_Your(Your_buf, obj);

	if (finalcancelled(obj)) return(FALSE);

	/* (Rusting shop goods ought to be charged for.) */
	switch (obj->oclass) {
	    case POTION_CLASS:
		if (obj->otyp == POT_WATER) {
		    if (amnesia) {
			Your("%s to sparkle.", aobjnam(obj,"start"));
			obj->odiluted = 0;
			obj->otyp = POT_AMNESIA;
			obj->blessed = FALSE;
			obj->cursed = TRUE;
			curse(obj);
			curse(obj);
			obj->finalcancel = TRUE;
			used = TRUE;
			break;
		    }
		    return FALSE;
		}

		/* Diluting a !ofAmnesia just gives water... */
		if (obj->otyp == POT_AMNESIA) {
			Your("%s flat.", aobjnam(obj, "become"));
			obj->odiluted = 0;
			obj->otyp = POT_WATER;
			obj->finalcancel = TRUE;
			used = TRUE;
			break;
		}

		/* KMH -- Water into acid causes an explosion */
		if (obj->otyp == POT_ACID) {
			pline("It boils vigorously!");
			You("are caught in the explosion!");
			losehp(StrongAcid_resistance ? 1 : Acid_resistance ? rnd(5) : rnd(10),
			       "elementary chemistry", KILLED_BY);
			if (amnesia) {
			    You_feel("a momentary lapse of reason!");
			    forget(2 + rn2(3));
			}
			makeknown(obj->otyp);
			used = TRUE;
			break;
		}
		if (obj->otyp == POT_OIL) {
			You("pollute the environment.");
			if (practicantterror && !u.pract_oilspill) {
				pline("%s thunders: 'That's a serious crime you committed there! You pay a fine of 10000 zorkmids, and additionally you have to carry some dead weight for the remaining day to ensure that you don't do such a thing again!'", noroelaname());
				fineforpracticant(10000, 0, 0);
				u.graundweight += 1000;
				u.pract_oilspill = TRUE;
			}
			adjalign(-sgn(u.ualign.type));
			if (u.ualign.type == A_LAWFUL) {
				u.ualign.sins++;
				u.alignlim--;
			}
			if (!rn2((u.ualign.type == A_LAWFUL) ? 3 : 10)) {
				int copcnt;
				u.cnd_kopsummonamount++;
				copcnt = rnd(monster_difficulty() ) + 1;
				if (rn2(5)) copcnt = (copcnt / (rnd(4) + 1)) + 1;
				if (Role_if(PM_CAMPERSTRIKER)) copcnt *= (rn2(5) ? 2 : rn2(5) ? 3 : 5);

				if (uarmh && itemhasappearance(uarmh, APP_ANTI_GOVERNMENT_HELMET) ) {
					copcnt = (copcnt / 2) + 1;
				}

				if (RngeAntiGovernment) {
					copcnt = (copcnt / 2) + 1;
				}

			      while(--copcnt >= 0) {
					(void) makemon(mkclass(S_KOP,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);

					if (!rn2(100)) {

						int koptryct = 0;
						int kox, koy;

						for (koptryct = 0; koptryct < 2000; koptryct++) {
							kox = rn1(COLNO-3,2);
							koy = rn2(ROWNO);

							if (kox && koy && isok(kox, koy) && (levl[kox][koy].typ > DBWALL) && !(t_at(kox, koy)) ) {
								(void) maketrap(kox, koy, KOP_CUBE, 0, FALSE);
								break;
								}
						}
					}
				} /* while */
			}
		}
		if (amnesia)
		    pline("%s %s completely.", Your_buf, aobjnam(obj,"dilute"));
		else
		    pline("%s %s%s.", Your_buf, aobjnam(obj,"dilute"),
		      		obj->odiluted ? " further" : "");
		if(obj->unpaid && costly_spot(u.ux, u.uy)) {
		    You("dilute it, you pay for it.");
		    bill_dummy_object(obj);
		}
		if (obj->odiluted || amnesia) {
			obj->odiluted = 0;
#ifdef UNIXPC
			obj->blessed = FALSE;
			obj->cursed = FALSE;
#else
			obj->blessed = obj->cursed = FALSE;
#endif
			obj->hvycurse = obj->prmcurse = FALSE;
			obj->morgcurse = obj->evilcurse = obj->bbrcurse = obj->stckcurse = FALSE;

			obj->otyp = POT_WATER;
		} else obj->odiluted++;
		used = TRUE;
		break;
	    case SCROLL_CLASS:
		if (obj->otyp != SCR_BLANK_PAPER  && !obj->oartifact && obj->otyp != SCR_HEALING && obj->otyp != SCR_EXTRA_HEALING && obj->otyp != SCR_STANDARD_ID && obj->otyp != SCR_HEAL_OTHER && obj->otyp != SCR_MANA && obj->otyp != SCR_GREATER_MANA_RESTORATION && obj->otyp != SCR_CURE && obj->otyp != SCR_PHASE_DOOR
#ifdef MAIL
		    && obj->otyp != SCR_MAIL
#endif
		    ) {
			if (!Blind) {
				boolean oq1 = obj->quan == 1L;
				pline_The("scroll%s %s.",
					  oq1 ? "" : "s", otense(obj, "fade"));
			}
			if(obj->unpaid && costly_spot(u.ux, u.uy)) {
			    You("erase it, you pay for it.");
			    bill_dummy_object(obj);
			}
			obj->otyp = SCR_BLANK_PAPER;
			if (Role_if(PM_ARTIST)) {
				You_feel("guilty for erasing a scroll.");
			    adjalign(-10);
			}
			obj->spe = 0;
			used = TRUE;
		} 
		break;
	    case SPBOOK_CLASS:
		if (obj->otyp != SPE_BLANK_PAPER) {
			if (obj->otyp == SPE_BOOK_OF_THE_DEAD) {
	pline("%s suddenly heats up; steam rises and it remains dry.",
				The(xname(obj)));
			} else {
			    if (!Blind) {
				    boolean oq1 = obj->quan == 1L;
				    pline_The("spellbook%s %s.",
					oq1 ? "" : "s", otense(obj, "fade"));
			    }
			    if(obj->unpaid) {
				subfrombill(obj, shop_keeper(*u.ushops));
			        You("erase it, you pay for it.");
			        bill_dummy_object(obj);
			    }
			    obj->otyp = SPE_BLANK_PAPER;
			}
			used = TRUE;
		}
		break;
	    case GEM_CLASS:
		if (amnesia && (obj->otyp == LUCKSTONE ||
			obj->otyp == LOADSTONE || obj->otyp == HEALTHSTONE ||
			obj->otyp == TOUCHSTONE))
		    downgrade_obj(obj, FLINT, &used);
		if (obj->otyp == SALT_CHUNK) {
			pline("The stone dissolves!");
			makeknown(obj->otyp);
			if (obj->oartifact == ART_CUBIC_SODIUM_CHLORIDE) {
				pline("The stone held hidden magical powers, and upon their release, grateful monsters come into existence!");
				struct monst *familiar;
				familiar = makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
				if (familiar) (void) tamedog(familiar, (struct obj *) 0, TRUE);
				familiar = makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
				if (familiar) (void) tamedog(familiar, (struct obj *) 0, TRUE);
				familiar = makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
				if (familiar) (void) tamedog(familiar, (struct obj *) 0, TRUE);
				familiar = makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
				if (familiar) (void) tamedog(familiar, (struct obj *) 0, TRUE);
				familiar = makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
				if (familiar) (void) tamedog(familiar, (struct obj *) 0, TRUE);

			}
			useup(obj);
			otmp = mksobj(POT_SALT_WATER,TRUE,FALSE, FALSE);
			if (otmp) {
				otmp->blessed = otmp->cursed = otmp->hvycurse = otmp->prmcurse = otmp->morgcurse = otmp->evilcurse = otmp->bbrcurse = otmp->stckcurse = 0;
				addinv(otmp);
				update_inventory();
			}
			return TRUE;
		}
		break;
	    case TOOL_CLASS:
		/* Artifacts aren't downgraded by amnesia */
		if (obj->otyp == BOTTLE) {
			pline("The bottle%s filled with water.",obj->quan > 1 ? "s are" : " is");
			obj->oclass = POTION_CLASS;
			obj->otyp = POT_WATER;
			/* retain curses/blessings etc. */
			return(TRUE);
		}
		if (amnesia && !obj->oartifact) {
		    switch (obj->otyp) {
			case MAGIC_LAMP:
			    /* Magic lamps forget their djinn... */
			    downgrade_obj(obj, OIL_LAMP, &used);
			    break;
			case MAGIC_CANDLE:
			    downgrade_obj(obj, 
					    rn2(2)? WAX_CANDLE : TALLOW_CANDLE,
					    &used);
			    break;
			case DRUM_OF_EARTHQUAKE:
			    downgrade_obj(obj, LEATHER_DRUM, &used);
			    break;
			case MAGIC_WHISTLE:
			    /* Magic whistles lose their powers... */
			    downgrade_obj(obj, TIN_WHISTLE, &used);
			    break;
			case MAGIC_FLUTE:
			    /* Magic flutes sound normal again... */
			    downgrade_obj(obj, WOODEN_FLUTE, &used);
			    break;
			case MAGIC_HARP:
			    /* Magic harps sound normal again... */
			    downgrade_obj(obj, WOODEN_HARP, &used);
			    break;
			case FIRE_HORN:
			case FROST_HORN:
			case TEMPEST_HORN:
			case ETHER_HORN:
			case SHADOW_HORN:
			case CHROME_HORN:
			case HORN_OF_PLENTY:
			    downgrade_obj(obj, TOOLED_HORN, &used);
			    break;
			case MAGIC_MARKER:
			    /* Magic markers run... */
			    if (obj->spe > 0) {
				pre_downgrade_obj(obj, &used);
				if ((obj->spe -= (3 + rn2(10))) < 0) 
				    obj->spe = 0;
			    }
			    break;
		    }
		}

		/* The only other tools that can be affected are pick axes and 
		 * unicorn horns... */
		if (!is_weptool(obj)) break;
		/* Drop through for disenchantment and rusting... */
		/* fall through */
	    case ARMOR_CLASS:
	    case WEAPON_CLASS:
	    case WAND_CLASS:
	    case RING_CLASS:
	    /* Just "fall through" to generic rustprone check for now. */
	    /* fall through */
	    default:
		switch(artifact_wet(obj, FALSE)) {
		    case -1: break;
		    default:
			return TRUE;
		}
		/* !ofAmnesia acts as a disenchanter... */
		if (amnesia && obj->spe > 0) {
		    pre_downgrade_obj(obj, &used);
		    drain_item(obj);
		}
		if (!obj->oerodeproof && !(Race_if(PM_CHIQUAI) && rn2(4)) && !(obj->oartifact && rn2(4)) && (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ) && is_rustprone(obj) &&
		    (obj->oeroded < MAX_ERODE) && !rn2(2)) {
			pline("%s %s some%s.",
			      Your_buf, aobjnam(obj, "rust"),
			      obj->oeroded ? " more" : "what");
			obj->oeroded++;
			if(obj->unpaid && costly_spot(u.ux, u.uy) && !used) {
			    You("damage it, you pay for it.");
			    bill_dummy_object(obj);
			}
			used = TRUE;
		} 
		break;
	}
	/* !ofAmnesia might strip away fooproofing... */
	if (amnesia && obj->oerodeproof && !rn2(13)) {
	    pre_downgrade_obj(obj, &used);
	    obj->oerodeproof = FALSE;
	}

	/* !ofAmnesia also strips blessed/cursed status... */

	if (amnesia && (obj->cursed || obj->blessed)) {
	    /* Blessed objects are valuable, cursed objects aren't, unless
	     * they're water.
	     */
	    if (obj->blessed || obj->otyp == POT_WATER)
		pre_downgrade_obj(obj, &used);
	    else if (!used) {
		Your("%s for a moment.", aobjnam(obj, "sparkle"));
		used = TRUE;
	    }
		/* Amy edit: you cannot easily uncurse everything, sorry */
	    if (obj->otyp == POT_WATER) uncurse(obj, FALSE);
	    unbless(obj);
	}

	if (used) 
	    update_inventory();
	else 
	    pline("%s %s wet.", Your_buf, aobjnam(obj,"get"));

	return used;
}


/* KMH, balance patch -- idea by Dylan O'Donnell <dylanw@demon.net>
 * The poor hacker's polypile.  This includes weapons, armor, and tools.
 * To maintain balance, magical categories (amulets, scrolls, spellbooks,
 * potions, rings, and wands) should NOT be supported.
 * Polearms are not currently implemented.
 */
int
upgrade_obj(obj)
register struct obj *obj;
/* returns 1 if something happened (potion should be used up) 
 * returns 0 if nothing happened
 * returns -1 if object exploded (potion should be used up) 
 */
{
	int chg, otyp = obj->otyp, otyp2;
	xchar ox, oy;
	long owornmask;
	struct obj *otmp;
	boolean explodes;
	char buf[BUFSZ];

	/* Check to see if object is valid */
	if (!obj)
		return 0;
	(void)snuff_lit(obj);
	if (obj->oartifact)
		/* WAC -- Could have some funky fx */
		return 0;

	if (is_hazy(obj)) return 0; /* poly weapon into mattock, upgrade to pick-axe, poly into 0:50 tinning kit, unpoly... +50 weapon :-P can't have that! --Amy */

	if (stack_too_big(obj))
		return 1;

	switch (obj->otyp)
	{
		/* weapons */
		case ORCISH_DAGGER:
			obj->otyp = DAGGER;
			break;
		case GREAT_DAGGER:
		case DAGGER:
			if (!rn2(2)) obj->otyp = ELVEN_DAGGER;
			else obj->otyp = DARK_ELVEN_DAGGER;
			break;
		case ELVEN_DAGGER:
		case DARK_ELVEN_DAGGER:
			obj->otyp = GREAT_DAGGER;
			break;
		case KNIFE:
			obj->otyp = STILETTO;
			break;
		case STILETTO:
			obj->otyp = KNIFE;
			break;
		case AXE:
			obj->otyp = BATTLE_AXE;
			break;
		case BATTLE_AXE:
			obj->otyp = AXE;
			break;
		case PICK_AXE:
			obj->otyp = DWARVISH_MATTOCK;
			break;
		case DWARVISH_MATTOCK:
			obj->otyp = PICK_AXE;
			break;
		case ORCISH_SHORT_SWORD:
			obj->otyp = SHORT_SWORD;
			break;
		case ELVEN_SHORT_SWORD:
		case DARK_ELVEN_SHORT_SWORD:
		case SHORT_SWORD:
			obj->otyp = DWARVISH_SHORT_SWORD;
			break;
		case DWARVISH_SHORT_SWORD:
			if (!rn2(2)) obj->otyp = ELVEN_SHORT_SWORD;
			else obj->otyp = DARK_ELVEN_SHORT_SWORD;
			break;
		case IRON_SABER:
			if (!rn2(2)) obj->otyp = SILVER_SABER;
			else obj->otyp = GOLDEN_SABER;
			break;
		case GOLDEN_SABER:
		case SILVER_SABER:
			obj->otyp = IRON_SABER;
		case BROADSWORD:
			obj->otyp = ELVEN_BROADSWORD;
			break;
		case ELVEN_BROADSWORD:
			obj->otyp = BROADSWORD;
			break;
		case MOP:
			obj->otyp = SPECIAL_MOP;
			break;
		case SPECIAL_MOP:
			obj->otyp = MOP;
			break;
		case CLUB:
			obj->otyp = AKLYS;
			break;
		case AKLYS:
			obj->otyp = CLUB;
			break;
		case SICKLE:
			obj->otyp = ELVEN_SICKLE;
			break;
		case ELVEN_SICKLE:
			obj->otyp = SICKLE;
			break;
		case FIRE_HOOK:
			obj->otyp = PLATINUM_FIRE_HOOK;
			break;
		case PLATINUM_FIRE_HOOK:
			obj->otyp = FIRE_HOOK;
			break;
		case ATHAME:
			obj->otyp = MERCURIAL_ATHAME;
			break;
		case MERCURIAL_ATHAME:
			obj->otyp = ATHAME;
			break;
		case SUGUHANOKEN:
			obj->otyp = GREAT_HOUCHOU;
			break;
		case GREAT_HOUCHOU:
			obj->otyp = SUGUHANOKEN;
			break;
		case WAR_HAMMER:
			obj->otyp = HEAVY_HAMMER;
			break;
		case HEAVY_HAMMER:
			obj->otyp = WAR_HAMMER;
			break;
		case ELVEN_BOW:
		case DARK_ELVEN_BOW:
		case YUMI:
		case ORCISH_BOW:
			obj->otyp = BOW;
			break;
		case BOW:
			switch (rn2(3)) {
				case 0: obj->otyp = ELVEN_BOW; break;
				case 1: obj->otyp = DARK_ELVEN_BOW; break;
				case 2: obj->otyp = YUMI; break;
			}
			break;
		case ELVEN_ARROW:
		case DARK_ELVEN_ARROW:
		case YA:
		case ORCISH_ARROW:
			obj->otyp = ARROW;
			break;
		case ARROW:
			switch (rn2(3)) {
				case 0: obj->otyp = ELVEN_ARROW; break;
				case 1: obj->otyp = DARK_ELVEN_ARROW; break;
				case 2: obj->otyp = YA; break;
			}
			break;
		/* armour */
		case ELVEN_MITHRIL_COAT:
			obj->otyp = DARK_ELVEN_MITHRIL_COAT;
			break;
		case DARK_ELVEN_MITHRIL_COAT:
			obj->otyp = ELVEN_MITHRIL_COAT;
			break;
		case ORCISH_CHAIN_MAIL:
			obj->otyp = CHAIN_MAIL;
			break;
		case CHAIN_MAIL:
			obj->otyp = ORCISH_CHAIN_MAIL;
			break;
		case OLIHARCON_SPLINT_MAIL:
			obj->otyp = SPLINT_MAIL;
			break;
		case SPLINT_MAIL:
			obj->otyp = OLIHARCON_SPLINT_MAIL;
			break;
		case STUDDED_LEATHER_ARMOR:
		case LEATHER_JACKET:
			obj->otyp = LEATHER_ARMOR;
			break;
		case LEATHER_ARMOR:
			obj->otyp = STUDDED_LEATHER_ARMOR;
			break;
		case PLATE_MAIL:
			if (!rn2(2)) obj->otyp = CRYSTAL_PLATE_MAIL;
			else obj->otyp = BRONZE_PLATE_MAIL;
			break;
		case BRONZE_PLATE_MAIL:
		case CRYSTAL_PLATE_MAIL:
			obj->otyp = PLATE_MAIL;
			break;
		case CAMOUFLAGED_CLOTHES:
			obj->otyp = SPECIAL_CAMOUFLAGED_CLOTHES;
			break;
		case SPECIAL_CAMOUFLAGED_CLOTHES:
			obj->otyp = CAMOUFLAGED_CLOTHES;
			break;

		/* robes */
		case ROBE:
			if (!rn2(2)) obj->otyp = ROBE_OF_PROTECTION;
			else obj->otyp = ROBE_OF_POWER;
			break;
		case ROBE_OF_PROTECTION:
		case ROBE_OF_POWER:
			obj->otyp = ROBE;
			break;
		/* cloaks */
		case CLOAK_OF_PROTECTION:
		case CLOAK_OF_INVISIBILITY:
		case CLOAK_OF_MAGIC_RESISTANCE:
		case CLOAK_OF_DISPLACEMENT:
		case DWARVISH_CLOAK:
		case ORCISH_CLOAK:
			if (!rn2(2)) obj->otyp = OILSKIN_CLOAK;
			else obj->otyp = ELVEN_CLOAK;
			break;
		case OILSKIN_CLOAK:
		case ELVEN_CLOAK:
			switch (rn2(4)) {
				case 0: obj->otyp = CLOAK_OF_PROTECTION; break;
				case 1: obj->otyp = CLOAK_OF_INVISIBILITY; break;
				case 2: obj->otyp = CLOAK_OF_MAGIC_RESISTANCE; break;
				case 3: obj->otyp = CLOAK_OF_DISPLACEMENT; break;
			}
			break;
		/* helms */
		case FEDORA:
			obj->otyp = ELVEN_LEATHER_HELM;
			break;
		case ELVEN_LEATHER_HELM:
			obj->otyp = FEDORA;
			break;
		case ELVEN_HELM:
			obj->otyp = HIGH_ELVEN_HELM;
			break;
		case HIGH_ELVEN_HELM:
			obj->otyp = ELVEN_HELM;
			break;
		case DENTED_POT:
			obj->otyp = ORCISH_HELM;
			break;
		case ORCISH_HELM:
		case HELM_OF_BRILLIANCE:
		case HELM_OF_TELEPATHY:
			obj->otyp = DWARVISH_IRON_HELM;
			break;
		case DWARVISH_IRON_HELM:
			if (!rn2(2)) obj->otyp = HELM_OF_BRILLIANCE;
			else obj->otyp = HELM_OF_TELEPATHY;
			break;
		case CORNUTHAUM:
			obj->otyp = DUNCE_CAP;
			break;
		case DUNCE_CAP:
			obj->otyp = CORNUTHAUM;
			break;
		/* gloves */
		case LEATHER_GLOVES:
			if (!rn2(2)) obj->otyp = GAUNTLETS_OF_SWIMMING;
			else obj->otyp = GAUNTLETS_OF_DEXTERITY;
			break;
		case GAUNTLETS_OF_SWIMMING:
		case GAUNTLETS_OF_DEXTERITY:
			obj->otyp = LEATHER_GLOVES;
			break;
		case GAUNTLETS:
			obj->otyp = SILVER_GAUNTLETS;
			break;
		case SILVER_GAUNTLETS:
			obj->otyp = GAUNTLETS;
			break;
		/* shields */
		case ELVEN_SHIELD:
			if (!rn2(2)) obj->otyp = URUK_HAI_SHIELD;
			else obj->otyp = ORCISH_SHIELD;
			break;
		case URUK_HAI_SHIELD:
		case ORCISH_SHIELD:
			obj->otyp = ELVEN_SHIELD;
			break;
		case DWARVISH_ROUNDSHIELD:
			obj->otyp = LARGE_SHIELD;
			break;
		case LARGE_SHIELD:
			obj->otyp = DWARVISH_ROUNDSHIELD;
			break;
		/* boots */
		case LOW_BOOTS:
			obj->otyp = HIGH_BOOTS;
			break;
		case HIGH_BOOTS:
			obj->otyp = LOW_BOOTS;
			break;
		case SNEAKERS:
			obj->otyp = SOFT_SNEAKERS;
			break;
		case SOFT_SNEAKERS:
			obj->otyp = SNEAKERS;
			break;
		/* NOTE:  Supposedly,  HIGH_BOOTS should upgrade to any of the
			other magic leather boots (except for fumble).  IRON_SHOES
			should upgrade to the iron magic boots,  unless
			the iron magic boots are fumble */
		/* rings,  amulets */
		case SACK:
			obj->otyp = rn2(5) ? OILSKIN_SACK : BAG_OF_HOLDING;
			break;
		case OILSKIN_SACK:
			obj->otyp = BAG_OF_HOLDING;
			break;
		case BAG_OF_HOLDING:
			obj->otyp = OILSKIN_SACK;
			break;
		case CHEST_OF_HOLDING:
			obj->otyp = CHEST;
			break;
		case CHEST:
			obj->otyp = CHEST_OF_HOLDING;
			break;
		case ICE_BOX:
			obj->otyp = rn2(2) ? ICE_BOX_OF_HOLDING : ICE_BOX_OF_WATERPROOFING;
			break;
		case ICE_BOX_OF_HOLDING:
		case ICE_BOX_OF_WATERPROOFING:
			obj->otyp = ICE_BOX;
			break;
		case TOWEL:
			obj->otyp = BLINDFOLD;
			break;
		case BLINDFOLD:
			obj->otyp = TOWEL;
			break;
		case CREDIT_CARD:
		case LOCK_PICK:
			obj->otyp = SKELETON_KEY;
			break;
		case SKELETON_KEY:
			obj->otyp = LOCK_PICK;
			break;
		case TALLOW_CANDLE:
			obj->otyp = WAX_CANDLE;
			break;
		case WAX_CANDLE:
			obj->otyp = TALLOW_CANDLE;
			break;
		case OIL_LAMP:
			obj->otyp = BRASS_LANTERN;
			break;
		case BRASS_LANTERN:
			obj->otyp = OIL_LAMP;
			break;
		case TIN_WHISTLE:
			obj->otyp = MAGIC_WHISTLE;
			break;
		case MAGIC_WHISTLE:
			obj->otyp = TIN_WHISTLE;
			break;
		case WOODEN_FLUTE:
			obj->otyp = MAGIC_FLUTE;
			obj->spe = rn1(5,10);
			break;
		case MAGIC_FLUTE:
			obj->otyp = WOODEN_FLUTE;
			break;
		case TOOLED_HORN:
			obj->otyp = rn1(HORN_OF_PLENTY - TOOLED_HORN, FROST_HORN);
			obj->spe = rn1(5,10);
			obj->known = 0;
			break;
		case HORN_OF_PLENTY:
		case FIRE_HORN:
		case FROST_HORN:
		case TEMPEST_HORN:
		case ETHER_HORN:
		case SHADOW_HORN:
		case CHROME_HORN:
			obj->otyp = TOOLED_HORN;
			break;
		case WOODEN_HARP:
			obj->otyp = MAGIC_HARP;
			obj->spe = rn1(5,10);
			obj->known = 0;
			break;
		case MAGIC_HARP:
			obj->otyp = WOODEN_HARP;
			break;
		case LEATHER_LEASH:
			obj->otyp = LEATHER_SADDLE;
			break;
		case LEATHER_SADDLE:
			obj->otyp = LEATHER_LEASH;
			break;
		case INKA_LEASH:
			obj->otyp = INKA_SADDLE;
			break;
		case INKA_SADDLE:
			obj->otyp = INKA_LEASH;
			break;
		case TIN_OPENER:
			obj->otyp = TINNING_KIT;
			obj->spe = rn1(20,20);
			obj->known = 0;
			break;
		case TINNING_KIT:
			obj->otyp = TIN_OPENER;
			obj->spe = 0;
			break;
		case EXPENSIVE_CAMERA:
			obj->otyp = MIRROR;
			break;
		case MIRROR:
			obj->otyp = EXPENSIVE_CAMERA;
			obj->spe = rn1(30,70);
			obj->known = 0;
			break;
		case CRYSTAL_BALL:
			/* "ball-point pen" */
			obj->otyp = MAGIC_MARKER;
			/* Keep the charges (crystal ball usually less than marker) */
			break;
		case MAGIC_MARKER:
			obj->otyp = CRYSTAL_BALL;
			chg = rn1(10,3);
			if (obj->spe > chg)
				obj->spe = chg;
			obj->known = 0;
			break;
		case K_RATION:
		case C_RATION:
		case LEMBAS_WAFER:
			if (!rn2(2)) obj->otyp = CRAM_RATION;
			else obj->otyp = FOOD_RATION;
			break;
		case FOOD_RATION:
		case CRAM_RATION:
			obj->otyp = LEMBAS_WAFER;
			break;
		case CHARRED_BREAD:
			obj->otyp = BREAD;
			break;
		case SLICE_OF_PIZZA:
			obj->otyp = PIZZA;
			break;
		case PIZZA:
			obj->otyp = SLICE_OF_PIZZA;
			break;
		case LOADSTONE:
			obj->otyp = FLINT;
			break;
		case FLINT:
			if (!rn2(2)) obj->otyp = LUCKSTONE;
			else obj->otyp = WHETSTONE; /*no more stacks of healthstones for cavedwarfs, sorry --Amy*/
			break;
		default:
			/* This object is not upgradable */
			return 0;
	}

	if (artifact_name(ONAME(obj), &otyp2) && otyp2 == obj->otyp) {
	    int n;
	    char c1, c2;

	    strcpy(buf, ONAME(obj));
	    n = rn2((int)strlen(buf));
	    c1 = lowc(buf[n]);
	    do c2 = 'a' + rn2('z'-'a'); while (c1 == c2);
	    buf[n] = (buf[n] == c1) ? c2 : highc(c2);  /* keep same case */
	    if (oname(obj, buf) != obj)
		panic("upgrade_obj: unhandled realloc");
	}

	if ((!carried(obj) || obj->unpaid) &&
		!is_hazy(obj) &&
		get_obj_location(obj, &ox, &oy, BURIED_TOO|CONTAINED_TOO) &&
		costly_spot(ox, oy)) {
	    char objroom = *in_rooms(ox, oy, SHOPBASE);
	    register struct monst *shkp = shop_keeper(objroom);

	    if ((!obj->no_charge ||
		 (Has_contents(obj) &&
		    (contained_cost(obj, shkp, 0L, FALSE, FALSE) != 0L)))
	       && inhishop(shkp)) {
		if(shkp->mpeaceful) {
		    if(*u.ushops && *in_rooms(u.ux, u.uy, 0) ==
			    *in_rooms(shkp->mx, shkp->my, 0) &&
			    !costly_spot(u.ux, u.uy))
			make_angry_shk(shkp, ox, oy);
		    else {
			pline("%s gets angry!", Monnam(shkp));
			hot_pursuit(shkp);
		    }
		} else Norep("%s is furious!", Monnam(shkp));
		otyp2 = obj->otyp;
		obj->otyp = otyp;
		/*
		 * [ALI] When unpaid containers are upgraded, the
		 * old container is billed as a dummy object, but
		 * it's contents are unaffected and will remain
		 * either unpaid or not as appropriate.
		 */
		otmp = obj->cobj;
		obj->cobj = NULL;
		if (costly_spot(u.ux, u.uy) && objroom == *u.ushops)
		    bill_dummy_object(obj);
		else
		    (void) stolen_value(obj, ox, oy, FALSE, FALSE, FALSE);
		obj->otyp = otyp2;
		obj->cobj = otmp;
	    }
	}

	/* The object was transformed */
	obj->owt = weight(obj);
	obj->oclass = objects[obj->otyp].oc_class;
	if (!objects[obj->otyp].oc_uses_known)
	    obj->known = 1;

	if (carried(obj)) {
	    if (obj == uskin) rehumanize();
	    /* Quietly remove worn item if no longer compatible --ALI */
	    owornmask = obj->owornmask;
	    if (owornmask & W_ARM && !is_suit(obj))
		owornmask &= ~W_ARM;
	    if (owornmask & W_ARMC && !is_cloak(obj))
		owornmask &= ~W_ARMC;
	    if (owornmask & W_ARMH && !is_helmet(obj))
		owornmask &= ~W_ARMH;
	    if (owornmask & W_ARMS && !is_shield(obj))
		owornmask &= ~W_ARMS;
	    if (owornmask & W_ARMG && !is_gloves(obj))
		owornmask &= ~W_ARMG;
	    if (owornmask & W_ARMF && !is_boots(obj))
		owornmask &= ~W_ARMF;
	    if (owornmask & W_ARMU && !is_shirt(obj))
		owornmask &= ~W_ARMU;
	    if (owornmask & W_TOOL && obj->otyp != BLINDFOLD && obj->otyp != CLIMBING_SET && obj->otyp != EYECLOSER && obj->otyp != DRAGON_EYEPATCH && obj->otyp != CONDOME && obj->otyp != SOFT_CHASTITY_BELT &&
	      obj->otyp != TOWEL && obj->otyp != LENSES && obj->otyp != RADIOGLASSES && obj->otyp != BOSS_VISOR && obj->otyp != BOSS_VISOR)
		owornmask &= ~W_TOOL;
	    otyp2 = obj->otyp;
	    obj->otyp = otyp;
	    if (obj->otyp == LEATHER_LEASH && obj->leashmon) o_unleash(obj);
	    if (obj->otyp == INKA_LEASH && obj->leashmon) o_unleash(obj);
	    remove_worn_item(obj, TRUE);
	    obj->otyp = otyp2;
	    obj->owornmask = owornmask;
	    setworn(obj, obj->owornmask);
	    puton_worn_item(obj);
	}

	if ((obj->otyp == BAG_OF_HOLDING || obj->otyp == CHEST_OF_HOLDING || obj->otyp == ICE_BOX_OF_HOLDING) && Has_contents(obj)) {
	    explodes = FALSE;

	    for (otmp = obj->cobj; otmp; otmp = otmp->nobj)
		if (mbag_explodes(otmp, 0)) { 
		    explodes = TRUE;
		    break;
		}

            if (explodes) {
		pline("As you upgrade your container, you are blasted by a magical explosion!");
		delete_contents(obj);
		if (carried(obj))
		    useup(obj);
		else
		    useupf(obj, obj->quan);
		losehp(d(6,6), "magical explosion", KILLED_BY_AN);
		return -1;
	    }
	}
	return 1;
}

int
dodip()
{
	struct obj *potion, *obj, *singlepotion;
	const char *tmp;
	uchar here;
	char allowall[2], qbuf[QBUFSZ], Your_buf[BUFSZ];
	short mixture;
	int res;
	boolean potfinalized = FALSE;

	allowall[0] = ALL_CLASSES; allowall[1] = '\0';
	if(!(obj = getobj(allowall, "dip")))
		return(0);

	here = levl[u.ux][u.uy].typ;
	/* Is there a fountain to dip into here? */
	if (IS_FOUNTAIN(here)) {
		if(yn("Dip it into the fountain?") == 'y') {
			dipfountain(obj);
			return(1);
		}
	} else if (IS_TOILET(here)) {        
		if(yn("Dip it into the toilet?") == 'y') {
			diptoilet(obj);
			return(1);
		}
	} else if (is_waterypool(u.ux,u.uy) && !(is_crystalwater(u.ux, u.uy) && !(Levitation || Flying)) ) {
		tmp = waterbody_name(u.ux,u.uy);
		sprintf(qbuf, "Dip it into the %s?", tmp);
		if (yn(qbuf) == 'y') {
		    if (Levitation) {
			floating_above(tmp);
		    } else if (u.usteed && !(uwep && uwep->oartifact == ART_SORTIE_A_GAUCHE) && !(powerfulimplants() && uimplant && uimplant->oartifact == ART_READY_FOR_A_RIDE) && !(bmwride(ART_DEEPER_LAID_BMW)) && !is_swimmer(u.usteed->data) && !u.usteed->egotype_watersplasher &&
			    (PlayerCannotUseSkills || P_SKILL(P_RIDING) < P_BASIC) ) {
			rider_cant_reach(); /* not skilled enough to reach */
		    } else {

			if (rn2(2) && !stack_too_big(obj) && !obj->oerodeproof && is_rustprone(obj) && !hard_to_destruct(obj) && (!obj->oartifact || !rn2(4)) && obj->oeroded == MAX_ERODE) {

				remove_worn_item(obj, FALSE);
				if (obj == uball) unpunish();
				useupall(obj);
				update_inventory();
				pline("The item rusted away completely!");

				return 1;
			}

			(void) get_wet(obj, level.flags.lethe);
			if (obj->otyp == POT_ACID) useup(obj);
		    }
		    return 1;
		}
	} else if (is_lava(u.ux,u.uy)) {
		sprintf(qbuf, "Dip it into the lava?");
		if (yn(qbuf) == 'y') {
		    if (Levitation) {
			floating_above("lava");
		    } else if (u.usteed && !(uwep && uwep->oartifact == ART_SORTIE_A_GAUCHE) && !(powerfulimplants() && uimplant && uimplant->oartifact == ART_READY_FOR_A_RIDE) && !(bmwride(ART_DEEPER_LAID_BMW)) && !is_swimmer(u.usteed->data) && !u.usteed->egotype_watersplasher &&
			    (PlayerCannotUseSkills || P_SKILL(P_RIDING) < P_BASIC) ) {
			rider_cant_reach(); /* not skilled enough to reach */
		    } else {

			if (obj && rn2(2) && !stack_too_big(obj) && !obj->oerodeproof && is_flammable(obj) && !hard_to_destruct(obj) && (!obj->oartifact || !rn2(4)) && obj->oeroded == MAX_ERODE) {

				remove_worn_item(obj, FALSE);
				if (obj == uball) unpunish();
				useupall(obj);
				update_inventory();
				pline("The item burned away completely!");

				return 1;
			}

			if (obj && !stack_too_big(obj) && !obj->oerodeproof && (obj->oclass == SCROLL_CLASS) ) {

				useupall(obj);
				update_inventory();
				pline("The scroll burns up!");

				return 1;
			}

			else if (obj && !stack_too_big(obj) && !evades_destruction(obj) && !obj->oerodeproof && (obj->oclass == SPBOOK_CLASS) ) {

				useupall(obj);
				update_inventory();
				pline("The book burns up!");

				return 1;
			}

			else if (obj && !stack_too_big(obj) && !obj->oerodeproof && (obj->oclass == POTION_CLASS) ) {

				pline("The potion explodes!");
				potionbreathe(obj);
				useupall(obj);
				update_inventory();

				return 1;
			}

			if (obj && !stack_too_big(obj) && (!obj->oartifact || !rn2(4)) && is_flammable(obj) && obj->oeroded < MAX_ERODE && !obj->oerodeproof && !rn2(2)) {
				pline("%s %s%s.", Yname2(obj), otense(obj, "burn"),
			      obj->oeroded+1 == MAX_ERODE ? " completely" :
			      obj->oeroded ? " further" : "");
				obj->oeroded++;
				return 1;
			}
			pline("%s is warmed.", Yname2(obj));

		    }
		    return 1;
		}

	}

	if(!(potion = getobj(beverages, "dip into")))
		return(0);
	if (potion == obj && potion->quan == 1L) {
		pline(FunnyHallu ? "The liquid inside that potion wobbles around. It's funny." : "That is a potion bottle, not a Klein bottle!");
		return 0;
	}

	if(potion->otyp != POT_WATER && obj->otyp == POT_WATER) {
	  /* swap roles, to ensure symmetry */
	  struct obj *otmp = potion;
	  potion = obj;
	  obj = otmp;
	} 
	potion->in_use = TRUE;          /* assume it will be used up */
	if(potion->otyp == POT_WATER) {
		boolean useeit = !Blind;
		if (useeit) (void) Shk_Your(Your_buf, obj);
		if (potion->blessed) {
			if (obj->cursed) {
				if (useeit)
				    pline("%s %s %s.",
					  Your_buf,
					  aobjnam(obj, "softly glow"),
					  hcolor(NH_AMBER));
				if (!stack_too_big(obj)) uncurse(obj, FALSE);
				else pline("Unfortunately, the stack was too big, so nothing happened.");
				obj->bknown=1;
	poof:
				if(!(objects[potion->otyp].oc_name_known) &&
				   !(objects[potion->otyp].oc_uname))
					docall(potion);
				useup(potion);
				u.cnd_alchemycount++;
				use_skill(P_DEVICES, 1);
				return(1);
			} else if(!obj->blessed) {
				if (useeit) {
				    tmp = hcolor(NH_LIGHT_BLUE);
				    pline("%s %s with a%s %s aura.",
					  Your_buf,
					  aobjnam(obj, "softly glow"),
					  index(vowels, *tmp) ? "n" : "", tmp);
				}
				if (!stack_too_big(obj)) bless(obj);
				else pline("Unfortunately, the stack was too big, so nothing happened.");
				obj->bknown=1;
				goto poof;
			}
		} else if (potion->cursed) {
			if (obj->blessed) {
				if (useeit)
				    pline("%s %s %s.",
					  Your_buf,
					  aobjnam(obj, "glow"),
					  hcolor((const char *)"brown"));
				if (!stack_too_big(obj)) unbless(obj);
				else pline("Unfortunately, the stack was too big, so nothing happened.");
				obj->bknown=1;
				goto poof;
			} else if(!obj->cursed) {
				if (useeit) {
				    tmp = hcolor(NH_BLACK);
				    pline("%s %s with a%s %s aura.",
					  Your_buf,
					  aobjnam(obj, "glow"),
					  index(vowels, *tmp) ? "n" : "", tmp);
				}
				if (!stack_too_big(obj)) curse(obj);
				else pline("Unfortunately, the stack was too big, so nothing happened.");
				obj->bknown=1;
				goto poof;
			}
		} else {
			switch(artifact_wet(obj,TRUE)) {
				/* Assume ZT_xxx is AD_xxx-1 */
				case -1: break;
				default:
					zap_over_floor(u.ux, u.uy,
					  (artifact_wet(obj,TRUE)-1), NULL);
					break;
			}
			if (get_wet(obj, FALSE))
			    goto poof;
		}
	} else if (potion->otyp == POT_AMNESIA || potion->otyp == POT_CANCELLATION) {
	    if (potion == obj) {
		obj->in_use = FALSE;
		potion = splitobj(obj, 1L);
		potion->in_use = TRUE;
	    }
	    if (get_wet(obj, TRUE)) goto poof;
	}
	/* WAC - Finn Theoderson - make polymorph and gain level msgs similar
	 * 	 Give out name of new object and allow user to name the potion
	 */
	/* KMH, balance patch -- idea by Dylan O'Donnell <dylanw@demon.net> */
	else if (potion->otyp == POT_GAIN_LEVEL && obj->oclass != POTION_CLASS) { /* this should fix it --Amy */
	/* thanks to the guy/girl figuring it out */
	    res = upgrade_obj(obj);

	    if (res != 0) {

		if (res == 1) { 
		     /* The object was upgraded */
		     pline("Hmm!  You don't recall dipping that into the potion.");
		     prinv((char *)0, obj, 0L);
		} /* else potion exploded */
		if (!objects[potion->otyp].oc_name_known &&
			!objects[potion->otyp].oc_uname)
		    docall(potion);
		useup(potion);
		update_inventory();
		exercise(A_WIS, TRUE);
		u.cnd_alchemycount++;
		use_skill(P_DEVICES, 1);
		return(1);
	    }
	    /* no return here, go for Interesting... message */
	} else if (obj->otyp == POT_POLYMORPH || obj->otyp == POT_MUTATION ||
		potion->otyp == POT_POLYMORPH || potion->otyp == POT_MUTATION) {
	    /* some objects can't be polymorphed */
	    if (obj->otyp == potion->otyp ||	/* both POT_POLY */
		    obj->otyp == WAN_POLYMORPH ||
		    obj->otyp == WAN_MUTATION ||
		    obj->otyp == SPE_POLYMORPH ||
		    obj->otyp == SPE_MUTATION ||
		    obj == uball || obj == uskin ||
		    obj_resists( (obj->otyp == POT_POLYMORPH || obj->otyp == POT_MUTATION) ?
				potion : obj, 5, 95)) {
		pline("%s", nothing_happens);
		if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
			pline("Oh wait, actually something bad happens...");
			badeffect();
		}
	    } else {
	    	boolean was_wep = FALSE, was_swapwep = FALSE, was_quiver = FALSE;
		short save_otyp = obj->otyp;
		/* KMH, conduct */
		u.uconduct.polypiles++;

		if (obj == uwep) was_wep = TRUE;
		else if (obj == uswapwep) was_swapwep = TRUE;
		else if (obj == uquiver) was_quiver = TRUE;

		obj = poly_obj(obj, STRANGE_OBJECT, FALSE);

		if (was_wep) setuwep(obj, TRUE, TRUE);
		else if (was_swapwep) setuswapwep(obj, TRUE);
		else if (was_quiver) setuqwep(obj);

		if (obj->otyp != save_otyp) {
			if (obj->otyp == POT_POLYMORPH) makeknown(POT_POLYMORPH);
			if (obj->otyp == POT_MUTATION) makeknown(POT_MUTATION);
			if (potion->otyp == POT_POLYMORPH) makeknown(POT_POLYMORPH);
			if (potion->otyp == POT_MUTATION) makeknown(POT_MUTATION);
			useup(potion);
			prinv((char *)0, obj, 0L);
			u.cnd_alchemycount++;
			use_skill(P_DEVICES, 1);
			return 1;
		} else {
			pline("Nothing seems to happen.");
			goto poof;
		}
	    }
	    potion->in_use = FALSE;	/* didn't go poof */
	    return(1);
	} else if (potion->otyp == POT_RESTORE_ABILITY && is_hazy(obj)) {
		/* KMH -- Restore ability will stop unpolymorphing */
		if (!stack_too_big(obj)) {stop_timer(UNPOLY_OBJ, (void *) obj);
		obj->oldtyp = STRANGE_OBJECT;
		if (!Blind)
			pline("%s seems less hazy.", Yname2(obj));
		}
		if (!objects[potion->otyp].oc_name_known && !objects[potion->otyp].oc_uname) docall(potion);
		useup(potion);
		u.cnd_alchemycount++;
		use_skill(P_DEVICES, 1);
		return (1);
	} else if(obj->oclass == POTION_CLASS && obj->otyp != potion->otyp) {
		/* Mixing potions is dangerous... */
		pline_The("potions mix...");
		/* KMH, balance patch -- acid is particularly unstable */
		if ((obj->cursed || obj->otyp == POT_ACID ||
		    potion->cursed || potion->otyp == POT_ACID || !rn2(10) || (stack_too_big(obj) && stack_too_big(obj)) || (stack_too_big(potion) && stack_too_big(potion) ) ) && !(uarmc && uarmc->oartifact == ART_NO_MORE_EXPLOSIONS && !(obj->otyp == POT_ACID || potion->otyp == POT_ACID) ) ) {
			pline("BOOM!  They explode!");
			if (practicantterror) {
				pline("%s booms: 'Quit trying to create bombs, maggot. 200 zorkmids.'", noroelaname());
				fineforpracticant(200, 0, 0);
			}
			exercise(A_STR, FALSE);
			if (!breathless(youmonst.data) || haseyes(youmonst.data))
				potionbreathe(obj);
			useup(obj);
			useup(potion);
			/* MRKR: an alchemy smock ought to be */
			/* some protection against this: */
			losehp(StrongAcid_resistance ? 1 : Acid_resistance ? rnd(5) : rnd(10),
			       "alchemic blast", KILLED_BY_AN);
			u.cnd_alchemycount++;
			use_skill(P_DEVICES, 1);
			return(1);
		}

		obj->blessed = obj->cursed = obj->bknown = 0;
		if (Blind || Hallucination) obj->dknown = 0;

		/* Amy edit: finalized potions are unlikely to work in alchemy */
		if (finalcancelled(obj) && !rn2(3)) {
			if (!Blind) pline_The("mixture glows brightly and evaporates.");
			useup(obj);
			useup(potion);
			u.cnd_alchemycount++;
			use_skill(P_DEVICES, 1);
			return(1);
		}
		if (finalcancelled(potion) && !rn2(3)) {
			if (!Blind) pline_The("mixture glows brightly and evaporates.");
			useup(obj);
			useup(potion);
			u.cnd_alchemycount++;
			use_skill(P_DEVICES, 1);
			return(1);
		}

		if ((mixture = mixtype(obj, potion)) != 0) {
			obj->otyp = mixture;
		} else {
		    switch (obj->odiluted ? 1 : rnd(8)) {
			case 1:
				obj->otyp = POT_WATER;
				break;
			case 2:
			case 3:
				obj->otyp = POT_SICKNESS;
				break;
			case 4:
				{
				  struct obj *otmp;
				  otmp = mkobj(POTION_CLASS,FALSE, FALSE);
				  obj->otyp = otmp->otyp;
				  obfree(otmp, (struct obj *)0);
				}
				break;
			default:
				if (!Blind)
			  pline_The("mixture glows brightly and evaporates.");
				useup(obj);
				useup(potion);
				u.cnd_alchemycount++;
				use_skill(P_DEVICES, 1);
				return(1);
		    }
		}

		obj->odiluted = (obj->otyp != POT_WATER);

		if (obj->otyp == POT_WATER && !Hallucination) {
			pline_The("mixture bubbles%s.",
				Blind ? "" : ", then clears");
		} else if (!Blind) {
			pline_The("mixture looks %s.",
				hcolor(OBJ_DESCR(objects[obj->otyp])));
		}

		useup(potion);
		u.cnd_alchemycount++;
		use_skill(P_DEVICES, 1);
		return(1);
	}
	if (!always_visible(obj)) {
	    if (potion->otyp == POT_INVISIBILITY && !obj->oinvis && !obj->oinvisreal) {
		if (!stack_too_big(obj)) {obj->oinvis = TRUE; if (!rn2(100)) obj->oinvisreal = TRUE;
		if (!Blind)
		    pline(!See_invisible ? "Where did %s go?" :
			  "Gee!  All of a sudden you can see right through %s.",
			  the(xname(obj)));
		}
		goto poof;
	    } else if (potion->otyp == POT_SEE_INVISIBLE && obj->oinvis) {
		if (!stack_too_big(obj)) {obj->oinvis = FALSE; if (obj->oinvisreal) obj->oinvis = TRUE;
		if (!Blind) {
		    if (!See_invisible)
			pline("So that's where %s went!", the(xname(obj)));
		    else
			You("can no longer see through %s.",
				the(xname(obj)));
		}
		}
		goto poof;
	    }
	}

	if(is_poisonable(obj)) {
	    if( (potion->otyp == POT_SICKNESS || potion->otyp == POT_POISON) && !obj->opoisoned) {
		char buf[BUFSZ];
		if (potion->quan > 1L)
		    sprintf(buf, "One of %s", the(xname(potion)));
		else
		    strcpy(buf, The(xname(potion)));
		pline("%s forms a coating on %s.",
		      buf, the(xname(obj)));
		if (!stack_too_big(obj)) obj->opoisoned = TRUE;
		else pline("Unfortunately there wasn't enough poison in there.");
		goto poof;
	    } else if( (potion->otyp == POT_CYANIDE) && !obj->superpoison) {
		char buf[BUFSZ];
		if (potion->quan > 1L)
		    sprintf(buf, "One of %s", the(xname(potion)));
		else
		    strcpy(buf, The(xname(potion)));
		pline("%s forms a very poisonous coating on %s.",
		      buf, the(xname(obj)));
		if (!stack_too_big(obj)) {
			obj->opoisoned = TRUE;
			obj->superpoison = TRUE;
		}
		else pline("Unfortunately there wasn't enough poison in there.");
		goto poof;
	    } else if(obj->opoisoned &&
		      (potion->otyp == POT_HEALING ||
		       potion->otyp == POT_EXTRA_HEALING ||
		       potion->otyp == POT_FULL_HEALING)) {
		pline("A coating wears off %s.", the(xname(obj)));
		if (!stack_too_big(obj)) obj->opoisoned = 0;
		else pline("Unfortunately it wasn't enough to completely remove the poison.");
		goto poof;
	    }
	}

	if (potion->otyp == POT_OIL) {
	    boolean wisx = FALSE;
	    if (potion->lamplit) {	/* burning */
		int omat = objects[obj->otyp].oc_material;
		/* the code here should be merged with fire_damage */
		if (catch_lit(obj)) {
		    /* catch_lit does all the work if true */
		} else if (obj->oerodeproof || obj_resists(obj, 5, 95) ||
			   !is_flammable(obj) || obj->oclass == FOOD_CLASS) {
		    pline("%s %s to burn for a moment.",
			  Yname2(obj), otense(obj, "seem"));
		} else {
		    if ((omat == MT_PLASTIC || omat == MT_PAPER) && !obj->oartifact)
			obj->oeroded = MAX_ERODE;
		    pline_The("burning oil %s %s.",
			    obj->oeroded == MAX_ERODE ? "destroys" : "damages",
			    yname(obj));
		    if (obj->oeroded == MAX_ERODE) {
			obj_extract_self(obj);
			obfree(obj, (struct obj *)0);
			obj = (struct obj *) 0;
		    } else {
			/* we know it's carried */
			if (obj->unpaid) {
			    /* create a dummy duplicate to put on bill */
			    verbalize("You burnt it, you bought it!");
			    bill_dummy_object(obj);
			}
			obj->oeroded++;
		    }
		}
	    } else if (potion->cursed) {
		pline_The("potion spills and covers your %s with oil.",
			  makeplural(body_part(FINGER)));
		incr_itimeout(&Glib, d(2,10));
	    } else if (obj->oclass != WEAPON_CLASS && !is_weptool(obj)) {
		/* the following cases apply only to weapons */
		goto more_dips;
	    /* Oil removes rust and corrosion, but doesn't unburn.
	     * Arrows, etc are classed as metallic due to arrowhead
	     * material, but dipping in oil shouldn't repair them.
	     */
	    } else if ((!is_rustprone(obj) && !is_corrodeable(obj)) ||
			is_ammo(obj) || (!obj->oeroded && !obj->oeroded2)) {
		/* uses up potion, doesn't set obj->greased */
		pline("%s %s with an oily sheen.",
		      Yname2(obj), otense(obj, "gleam"));
	    } else {
		pline("%s %s less %s.",
		      Yname2(obj), otense(obj, "are"),
		      (obj->oeroded && obj->oeroded2) ? "corroded and rusty" :
			obj->oeroded ? "rusty" : "corroded");
		if (!stack_too_big(obj) && obj->oeroded > 0) obj->oeroded--;
		if (!stack_too_big(obj) && obj->oeroded2 > 0) obj->oeroded2--;
		wisx = TRUE;
	    }
	    exercise(A_WIS, wisx);
	    makeknown(potion->otyp);
	    useup(potion);
		u.cnd_alchemycount++;
		use_skill(P_DEVICES, 1);
	    return 1;
	} else if (potion->otyp == POT_GAIN_LEVEL) {
	    res = upgrade_obj(obj);
	    if (res != 0) {
		if (res == 1) {
		    /* The object was upgraded */
		    pline("Hmm!  You don't recall dipping that into the potion.");
		    prinv((char *)0, obj, 0L);
		} /* else potion exploded */
		if (!objects[potion->otyp].oc_name_known &&
			!objects[potion->otyp].oc_uname)
		    docall(potion);
		useup(potion);
		update_inventory();
		exercise(A_WIS, TRUE);
		u.cnd_alchemycount++;
		use_skill(P_DEVICES, 1);
		return 1;
	    }
	    /* no return here, go for Interesting... message */
	}

	/* KMH, balance patch -- acid affects damage(proofing) */
	if (potion->otyp == POT_ACID && (obj->oclass == ARMOR_CLASS ||
		obj->oclass == WEAPON_CLASS || is_weptool(obj))) {
	    if (!potion->blessed && obj->oerodeproof && !stack_too_big(obj) ) {
		pline("%s %s golden shield.",  Yname2(obj),
			(obj->quan > 1L) ? "lose their" : "loses its");
		obj->oerodeproof = 0;
		makeknown(potion->otyp);
	    } else {
		pline("%s looks a little dull.", Yname2(obj));
		if (evilfriday) {
			if (obj->spe > -20) obj->spe--;
			rust_dmg(obj, xname(obj), 3, TRUE, &youmonst); /* DANGER: obj may have disappeared now --Amy */
			pline("In fact, it looks not just a little dull...");
		}
		if (!objects[potion->otyp].oc_name_known &&
			!objects[potion->otyp].oc_uname)
		    docall(potion);
	    }
	    exercise(A_WIS, FALSE);
  	    useup(potion);
	    u.cnd_alchemycount++;
	    use_skill(P_DEVICES, 1);
	    return 1;
	}
    more_dips:

	/* Allow filling of MAGIC_LAMPs to prevent identification by player */
	if ((obj->otyp == OIL_LAMP || obj->otyp == MAGIC_LAMP) &&
	   (potion->otyp == POT_OIL)) {

	    /* Turn off engine before fueling, turn off fuel too :-)  */
	    if (obj->lamplit || potion->lamplit) {
		useup(potion);
		explode(u.ux, u.uy, ZT_SPELL(ZT_FIRE), d(6,6), 0, EXPL_FIERY);
		exercise(A_WIS, FALSE);
		u.cnd_alchemycount++;
		use_skill(P_DEVICES, 1);
		return 1;
	    }
	    /* Adding oil to an empty magic lamp renders it into an oil lamp */
	    if ((obj->otyp == MAGIC_LAMP) && obj->spe == 0) {
		obj->otyp = OIL_LAMP;
		obj->age = 0;
	    }
	    if (obj->age > 1000L) {
                pline("%s %s full.", Yname2(obj), otense(obj, "are"));
		potion->in_use = FALSE; /* didn't go poof */
	    } else {
                You("fill your %s with oil.", yname(obj));
		if (!(uarmc && itemhasappearance(uarmc, APP_FUEL_CLOAK))) check_unpaid(potion);	/* Yendorian Fuel Tax */
		obj->age += 2*potion->age;	/* burns more efficiently */
		if (issoviet && obj->age > 1500L) obj->age = 1500L;
		useup(potion);
		exercise(A_WIS, TRUE);
		u.cnd_alchemycount++;
		use_skill(P_DEVICES, 1);
	    }
	    makeknown(POT_OIL);
	    obj->spe = 1;
	    update_inventory();
	    return 1;
	}

	potion->in_use = FALSE;         /* didn't go poof */
	if ((obj->otyp == UNICORN_HORN || obj->otyp == DARK_HORN || obj->otyp == SKY_HORN || obj->otyp == ARCANE_HORN || obj->oclass == GEM_CLASS) &&
	    (mixture = mixtype(obj, potion)) != 0) {
		char oldbuf[BUFSZ], newbuf[BUFSZ];
		short old_otyp = potion->otyp;
		boolean old_dknown = FALSE;
		boolean more_than_one = potion->quan > 1;
		if (potion && finalcancelled(potion)) potfinalized = TRUE;

		if ((obj->otyp == UNICORN_HORN || obj->otyp == DARK_HORN || obj->otyp == SKY_HORN || obj->otyp == ARCANE_HORN) && (obj->cursed || (obj->spe < (-rn1(10, 10)))) ) { /* uh-oh */
			pline("BOOM! The potion explodes!");
			if (practicantterror) {
				pline("%s booms: 'Quit trying to create bombs, maggot. 200 zorkmids.'", noroelaname());
				fineforpracticant(200, 0, 0);
			}
			potion->in_use = TRUE;
			if (!breathless(youmonst.data) || haseyes(youmonst.data)) potionbreathe(potion);
			useup(potion);
			losehp(StrongAcid_resistance ? 1 : Acid_resistance ? rnd(5) : rnd(10), "a cursed explosion", KILLED_BY);
			return(1);
			u.cnd_alchemycount++;
			use_skill(P_DEVICES, 1);
		}

		oldbuf[0] = '\0';
		if (potion->dknown) {
		    old_dknown = TRUE;
		    sprintf(oldbuf, "%s ",
			    hcolor(OBJ_DESCR(objects[potion->otyp])));
		}
		/* with multiple merged potions, split off one and
		   just clear it */
		if (potion->quan > 1L) {
		    singlepotion = splitobj(potion, 1L);
		} else singlepotion = potion;
		
		/* MRKR: Gems dissolve in acid to produce new potions */

		if (obj->oclass == GEM_CLASS && potion->otyp == POT_ACID) {
		    struct obj *singlegem = (obj->quan > 1L ? 
					     splitobj(obj, 1L) : obj);

		    singlegem->in_use = TRUE;
		    if (potion->otyp == POT_ACID && 
		      (obj->otyp == DILITHIUM_CRYSTAL || 
		       potion->cursed || !rn2(10))) {
			/* Just to keep them on their toes */

			singlepotion->in_use = TRUE;
			if (Hallucination && obj->otyp == DILITHIUM_CRYSTAL) {
			    /* Thanks to Robin Johnson */
			    pline("Warning, Captain!  The warp core has been breached!");
			}
			pline("BOOM!  %s explodes!", The(xname(singlegem)));
			if (practicantterror) {
				pline("%s booms: 'Quit trying to create bombs, maggot. 200 zorkmids.'", noroelaname());
				fineforpracticant(200, 0, 0);
			}
			exercise(A_STR, FALSE);
			if (!breathless(youmonst.data) || haseyes(youmonst.data))
			    potionbreathe(singlepotion);
			useup(singlegem);
			useup(singlepotion);
			/* MRKR: an alchemy smock ought to be */
			/* some protection against this: */
			losehp(StrongAcid_resistance ? 1 : Acid_resistance ? rnd(5) : rnd(10), 
			       "alchemic blast", KILLED_BY_AN);
			u.cnd_alchemycount++;
			use_skill(P_DEVICES, 1);
			return(1);	  
		    }

		    pline("%s dissolves in %s.", The(xname(singlegem)), 
			  the(xname(singlepotion)));
		    makeknown(POT_ACID);
		    useup(singlegem);
		}

		if(singlepotion->unpaid && costly_spot(u.ux, u.uy)) {
		    You("use it, you pay for it.");
		    bill_dummy_object(singlepotion);
		}

		if (singlepotion->otyp == mixture) {		  
		    /* no change - merge it back in */
		    if (more_than_one && !merged(&potion, &singlepotion)) {
			/* should never happen */
			impossible("singlepotion won't merge with parent potion.");
		    }
		} else {		  
		singlepotion->otyp = mixture;
		singlepotion->blessed = 0;
		if (mixture == POT_WATER)
		    singlepotion->cursed = singlepotion->odiluted = 0;
		else
		    singlepotion->cursed = obj->cursed;  /* odiluted left as-is */
		singlepotion->bknown = FALSE;
		if (Blind) {
		    singlepotion->dknown = FALSE;
		} else {
		    singlepotion->dknown = !Hallucination;
		    if (mixture == POT_WATER && singlepotion->dknown)
			sprintf(newbuf, "clears");
		    else
			sprintf(newbuf, "turns %s",
				hcolor(OBJ_DESCR(objects[mixture])));
		    pline_The("%spotion%s %s.", oldbuf,
			      more_than_one ? " that you dipped into" : "",
			      newbuf);
		    if(!objects[old_otyp].oc_uname &&
			!objects[old_otyp].oc_name_known && old_dknown) {
			struct obj fakeobj;
			fakeobj = zeroobj;
			fakeobj.dknown = 1;
			fakeobj.otyp = old_otyp;
			fakeobj.oclass = POTION_CLASS;
			docall(&fakeobj);
		    }
		}
		obj_extract_self(singlepotion);
		singlepotion = hold_another_object(singlepotion,
					"You juggle and drop %s!",
					doname(singlepotion), (const char *)0);
		update_inventory();
		}

		/* clearing potions can strain the unihorn; if you do it on a finalized potion, it always happens because
		 * finalized potions are actually meant to be used as what they are --Amy */
		if (obj && (obj->otyp == UNICORN_HORN || obj->otyp == DARK_HORN || obj->otyp == SKY_HORN || obj->otyp == ARCANE_HORN) && (potfinalized || !rn2(10)) ) {

			if (obj->spe > -20) obj->spe--;
			if(obj->blessed) unbless(obj);
			else if (!obj->blessed) curse(obj);

			pline(FunnyHallu ? "The tool is glowing in a wide array of colors!" : "Your unicorn horn seems less effective.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");

		}

		u.cnd_alchemycount++;
		use_skill(P_DEVICES, 1);
		return(1);
	}

	pline(FunnyHallu ? "Colorful..." : "Interesting...");
	return(1);
}


void
djinni_from_bottle(obj, kind)
register struct obj *obj;
int kind;
{
	struct monst *mtmp;
	int genie_type;        
	int chance;

#if 0
	/* KMH -- See comments in monst.c */
	switch (rn2(4)) {
		default:
		case 0: genie_type = PM_DJINNI; break;
		case 1: genie_type = PM_EFREETI; break;
		case 2: genie_type = PM_MARID; break;
		case 3: genie_type = PM_DAO; break;
	}
#else
	if (kind == 1) genie_type = PM_DJINNI;
	else if (kind == 2) genie_type = PM_DAO;
	else if (kind == 3) genie_type = PM_EFREETI;
	else if (kind == 4) genie_type = PM_MARID;
	else genie_type = PM_WINE_GHOST;
#endif
	if(!(mtmp = makemon(&mons[genie_type], u.ux, u.uy, NO_MM_FLAGS))){
		pline("It turns out to be empty.");
		make_bottle(TRUE);
		return;
	}

	if (!Blind) {
		pline("In a cloud of smoke, %s emerges!", a_monnam(mtmp));
		pline("%s speaks.", Monnam(mtmp));
	} else {
		You("smell acrid fumes.");
		pline("%s speaks.", Something);
	}

	chance = rn2(5);
	if (obj->blessed) chance = (chance == 4) ? rnd(4) : 0;
	else if (obj->cursed) chance = (chance == 0) ? rn2(4) : 4;
	/* 0,1,2,3,4:  b=80%,5,5,5,5; nc=20%,20,20,20,20; c=5%,5,5,5,80 */

	switch (chance) {
	case 0 :
		if (u.ulevel < 5) {
			verbalize("I'm sorry, you seem too inexperienced to receive my boon. Please take this enchanted purse with a hundred gold pieces instead!");
			(void) mkgold(100, u.ux, u.uy);
			mongone(mtmp);
		} else {
			verbalize("I am in your debt.  I will grant a boon!");
			if (!rn2(4)) makewish(evilfriday ? FALSE : TRUE);
			else othergreateffect();
			mongone(mtmp);
		}
		break;
	case 1 : verbalize("Thank you for freeing me!");
		(void) tamedog(mtmp, (struct obj *)0, FALSE);
		break;
	case 2 : verbalize("You freed me!");
		mtmp->mpeaceful = TRUE;
		set_malign(mtmp);
		break;
	case 3 : verbalize("It is about time!");
		pline("%s vanishes.", Monnam(mtmp));
		mongone(mtmp);
		break;
	default: verbalize("You disturbed me, fool!");
		break;
	}
	/* fix by Soviet5lo: don't make a bottle if it's from a lamp */
	if (obj->otyp != MAGIC_LAMP) make_bottle(FALSE);
}

/* clone a gremlin or mold (2nd arg non-null implies heat as the trigger);
   hit points are cut in half (odd HP stays with original) */
struct monst *
split_mon(mon, mtmp)
struct monst *mon,	/* monster being split */
	     *mtmp;	/* optional attacker whose heat triggered it */
{
	struct monst *mtmp2;
	char reason[BUFSZ];

	reason[0] = '\0';
	if (mtmp) sprintf(reason, " from %s heat",
			  (mtmp == &youmonst) ? (const char *)"your" :
			      (const char *)s_suffix(mon_nam(mtmp)));

	if (mon == &youmonst) {
	    mtmp2 = cloneu();
	    if (mtmp2) {
		mtmp2->mhpmax = u.mhmax / 2;
		u.mhmax -= mtmp2->mhpmax;
		flags.botl = 1;
		You("multiply%s!", reason);
	    }
	} else {
	    mtmp2 = clone_mon(mon, 0, 0);
	    if (mtmp2) {
		mtmp2->mhpmax = mon->mhpmax / 2;
		mon->mhpmax -= mtmp2->mhpmax;
		if (canspotmon(mon))
		    pline("%s multiplies%s!", Monnam(mon), reason);
	    }
	}
	return mtmp2;
}

#endif /* OVLB */

/*potion.c*/

STATIC_PTR void
set_litI(x,y,val)
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


