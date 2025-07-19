/*	SCCS Id: @(#)mondata.c	3.4	2003/06/02	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "eshk.h"
#include "epri.h"

/*	These routines provide basic data for any type of monster. */

#ifdef OVLB

void
set_mon_data(mon, ptr, flag)
struct monst *mon;
struct permonst *ptr;
int flag;
{
    mon->data = ptr;

    if (flag == -1) return;		/* "don't care" */

    if (flag == 1)
	mon->mintrinsics |= (ptr->mresists & MR_TYPEMASK);
    else
	mon->mintrinsics = (ptr->mresists & MR_TYPEMASK);

    return;
}

#endif /* OVLB */
#ifdef OVL0

struct attack *
attacktype_fordmg(ptr, atyp, dtyp)
struct permonst *ptr;
int atyp, dtyp;
{
    struct attack *a;

    for (a = &ptr->mattk[0]; a < &ptr->mattk[NATTK]; a++)
	if (a->aatyp == atyp && (dtyp == AD_ANY || a->adtyp == dtyp))
	    return a;

    return (struct attack *)0;
}

boolean
attacktype(ptr, atyp)
struct permonst *ptr;
int atyp;
{
    return attacktype_fordmg(ptr, atyp, AD_ANY) ? TRUE : FALSE;
}

boolean
canseemon(mon)
struct monst *mon;
{
	if (mon_visible(mon)) {
		if ((mon)->wormno) {
			if (worm_known(mon)) return TRUE;
		} else {
			if (cansee(mon->mx, mon->my) || see_with_infrared(mon)) return TRUE;
		}
	}

	return FALSE;
}

struct attack *
attdmgtype_fromattack(ptr, dtyp, atyp)
struct permonst *ptr;
int dtyp, atyp;
{
    struct attack *a;

    for (a = &ptr->mattk[0]; a < &ptr->mattk[NATTK]; a++)
	if (a->adtyp == dtyp && a->aatyp == atyp)
	    return a;

    return (struct attack *)0;
}

boolean
attackdamagetype(ptr, atyp, dtyp)
struct permonst *ptr;
int atyp, dtyp;
{
    return attacktype_fordmg(ptr, atyp, dtyp) ? TRUE : FALSE;
}

/* function to check whether a monster has an actual passive attack; AD_PHYS doesn't count --Amy */
boolean
haspassive(ptr)
struct permonst *ptr;
{
	int i;
	for (i = 0; ; i++) {
		if(i >= NATTK) return FALSE; /* no passive attacks */
		if (ptr->mattk[i].aatyp == AT_NONE && ptr->mattk[i].adtyp != AD_PHYS) return TRUE;
	}
}

#endif /* OVL0 */
#ifdef OVLB

boolean
poly_when_stoned(ptr)
    struct permonst *ptr;
{
    return((boolean)(is_golem(ptr) && ptr != &mons[PM_STONE_GOLEM] &&
	    !(mvitals[PM_STONE_GOLEM].mvflags & G_GENOD)));
	    /* allow G_EXTINCT */
}

boolean
resists_drli(mon)	/* returns TRUE if monster is drain-life resistant */
struct monst *mon;
{
	struct permonst *ptr = mon->data;
	struct obj *wep = ((mon == &youmonst) ? 0 : MON_WEP(mon));

	if (mon != &youmonst) {
		if (mercedesride(ART_STABITEGRITY, mon)) return TRUE;
	}

	return (boolean)(is_undead(ptr) || mon->egotype_undead || is_demon(ptr) || is_were(ptr) ||
			 ptr == &mons[PM_DEATH] || is_golem(ptr) ||
			 resists_drain(mon) ||
			 (wep && wep->oartifact && defends(AD_DRLI, wep)));
}

boolean
resists_magm(mon)	/* TRUE if monster is magic-missile resistant */
struct monst *mon;
{
	struct permonst *ptr = mon->data;
	struct obj *o;

	if (FemtrapActiveMarleen && humanoid(mon->data) && is_female(mon->data) && u.marleenproperties) return TRUE;

	/* as of 3.2.0:  gray dragons, Angels, Oracle, Yeenoghu */
	if (dmgtype(ptr, AD_MAGM) || dmgtype(ptr, AD_MCRE) || ptr == &mons[PM_BABY_GRAY_DRAGON] || ptr == &mons[PM_YOUNG_GRAY_DRAGON] || ptr == &mons[PM_YOUNG_ADULT_GRAY_DRAGON] || ptr == &mons[PM_DNETHACK_ELDER_PRIEST_TM_] ||
		dmgtype(ptr, AD_RBRE) || dmgtype(ptr, AD_RNG))	/* Chromatic Dragon */
	    return TRUE;
	/* check for magic resistance granted by wielded weapon */
	o = (mon == &youmonst) ? 0 : MON_WEP(mon);
	if (o && o->oartifact && defends(AD_MAGM, o))
	    return TRUE;

	if (mon != &youmonst) {
		if (mercedesride(ART_ITHILMAR, mon)) return TRUE;
	}

	/* check for magic resistance granted by worn or carried items */
	if (mon != &youmonst) {
		o = mon->minvent;
		for ( ; o; o = o->nobj)
		    if ((o->owornmask && objects[o->otyp].oc_oprop == ANTIMAGIC) ||
			    (o->oartifact && protects(AD_MAGM, o)))
			return TRUE;
	}
	return FALSE;
}

/* TRUE if monster is resistant to light-induced blindness */
boolean
resists_blnd(mon)
struct monst *mon;
{
	struct permonst *ptr = mon->data;
	boolean is_you = (mon == &youmonst);
	struct obj *o;

	if (is_you ? (Blind || u.usleep) :
		(mon->mblinded || !mon->mcansee || !haseyes(ptr) ||
		    /* BUG: temporary sleep sets mfrozen, but since
			    paralysis does too, we can't check it */
		    mon->msleeping))
	    return TRUE;
	/* yellow light, Archon; !dust vortex, !cobra, !raven */
	if (dmgtype_fromattack(ptr, AD_BLND, AT_EXPL) ||
		dmgtype_fromattack(ptr, AD_BLND, AT_GAZE))
	    return TRUE;
	o = is_you ? 0 : MON_WEP(mon);
	if (o && o->oartifact && defends(AD_BLND, o))
	    return TRUE;
	if (!is_you) {
		o = mon->minvent;
		for ( ; o; o = o->nobj)
		    if ((o->owornmask && objects[o->otyp].oc_oprop == BLINDED) ||
			    (o->oartifact && protects(AD_BLND, o)))
			return TRUE;
	}
	return FALSE;
}

/* TRUE if monster has one of the flags used for the ones spawned in the illusory castle --Amy */
boolean
isillusorymonster(mon)
struct monst *mon;
{
	struct permonst *ptr = mon->data;

	if (dmgtype(ptr, AD_SPC2)) return TRUE;
	if (dmgtype(ptr, AD_STUN)) return TRUE;
	if (dmgtype(ptr, AD_DRIN)) return TRUE;
	if (dmgtype(ptr, AD_HALU)) return TRUE;
	if (dmgtype(ptr, AD_DARK)) return TRUE;
	if (dmgtype(ptr, AD_FEAR)) return TRUE;
	if (dmgtype(ptr, AD_FAKE)) return TRUE;
	if (dmgtype(ptr, AD_LETH)) return TRUE;
	if (dmgtype(ptr, AD_CNCL)) return TRUE;
	if (dmgtype(ptr, AD_BANI)) return TRUE;
	if (dmgtype(ptr, AD_WISD)) return TRUE;
	if (dmgtype(ptr, AD_HODS)) return TRUE;
	if (dmgtype(ptr, AD_CHRN)) return TRUE;
	if (dmgtype(ptr, AD_WEEP)) return TRUE;
	if (dmgtype(ptr, AD_STTP)) return TRUE;
	if (dmgtype(ptr, AD_DEPR)) return TRUE;
	if (dmgtype(ptr, AD_LAZY)) return TRUE;
	if (dmgtype(ptr, AD_INER)) return TRUE;
	if (dmgtype(ptr, AD_VULN)) return TRUE;
	if (dmgtype(ptr, AD_AMNE)) return TRUE;
	if (dmgtype(ptr, AD_INSA)) return TRUE;
	if (dmgtype(ptr, AD_TRAP)) return TRUE;
	if (dmgtype(ptr, AD_SANI)) return TRUE;
	if (dmgtype(ptr, AD_SHAN)) return TRUE;
	if (dmgtype(ptr, AD_TERR)) return TRUE;
	if (dmgtype(ptr, AD_FEMI)) return TRUE;
	if (dmgtype(ptr, AD_LEVI)) return TRUE;
	if (dmgtype(ptr, AD_ILLU)) return TRUE;
	if (dmgtype(ptr, AD_MCRE)) return TRUE;
	if (dmgtype(ptr, AD_DEBU)) return TRUE;
	if (dmgtype(ptr, AD_UNPR)) return TRUE;
	if (dmgtype(ptr, AD_BLAS)) return TRUE;
	if (dmgtype(ptr, AD_INVE)) return TRUE;
	if (dmgtype(ptr, AD_WNCE)) return TRUE;
	if (shapechanger(ptr)) return TRUE;

	return FALSE;
}

/* TRUE iff monster can be blinded by the given attack */
/* Note: may return TRUE when mdef is blind (e.g. new cream-pie attack) */
boolean
can_blnd(magr, mdef, aatyp, obj)
struct monst *magr;		/* NULL == no specific aggressor */
struct monst *mdef;
uchar aatyp;
struct obj *obj;		/* aatyp == AT_WEAP, AT_SPIT */
{
	boolean is_you = (mdef == &youmonst);
	boolean check_visor = FALSE;
	struct obj *o;
	const char *s;

	/* no eyes protect against all attacks for now */
	if (!haseyes(mdef->data))
	    return FALSE;

	switch(aatyp) {
	case AT_EXPL: case AT_BOOM: case AT_GAZE: case AT_MAGC:
	case AT_BREA: /* assumed to be lightning */
	    /* light-based attacks may be cancelled or resisted */
	    if (magr && magr->mcan)
		return FALSE;
	    return !resists_blnd(mdef);

	case AT_WEAP: case AT_SPIT: case AT_NONE:
	    /* an object is used (thrown/spit/other) */
	    if (obj && (obj->otyp == CREAM_PIE)) {
		if (is_you && Blindfolded)
		    return FALSE;
	    } else if (obj && (obj->otyp == BLINDING_VENOM)) {
		/* all ublindf, including LENSES, protect, cream-pies too */
		if (is_you && (ublindf || u.ucreamed))
		    return FALSE;
		check_visor = TRUE;
	    } else if (obj && (obj->otyp == POT_BLINDNESS)) {
		return TRUE;	/* no defense */
	    } else
		return FALSE;	/* other objects cannot cause blindness yet */
	    if ((magr == &youmonst) && u.uswallow)
		return FALSE;	/* can't affect eyes while inside monster */
	    break;

	case AT_ENGL:
	    if (is_you && (Blindfolded || u.usleep || u.ucreamed))
		return FALSE;
	    if (!is_you && mdef->msleeping)
		return FALSE;
	    break;

	case AT_CLAW:
	    /* e.g. raven: all ublindf, including LENSES, protect */
	    if (is_you && ublindf)
		return FALSE;
	    if ((magr == &youmonst) && u.uswallow)
		return FALSE;	/* can't affect eyes while inside monster */
	    check_visor = TRUE;
	    break;

	case AT_TUCH: case AT_STNG:
	    /* some physical, blind-inducing attacks can be cancelled */
	    if (magr && magr->mcan)
		return FALSE;
	    break;

	default:
	    break;
	}

	/* check if wearing a visor (only checked if visor might help) */
	if (check_visor) {
	    o = (mdef == &youmonst) ? invent : mdef->minvent;
	    for ( ; o; o = o->nobj)
		if ((o->owornmask & W_ARMH) && (itemhasappearance(o, APP_VISORED_HELMET) || itemhasappearance(o, APP_ORANGE_VISORED_HELMET) || itemhasappearance(o, APP_TWISTED_VISOR_HELMET)) )
		    return FALSE;
	}

	if (RngeTrueSight && (mdef == &youmonst)) return FALSE;

	return TRUE;
}

#endif /* OVLB */
#ifdef OVL0

boolean
ranged_attk(ptr)	/* returns TRUE if monster can attack at range */
struct permonst *ptr;
{
	register int i, atyp;
	long atk_mask = (1L << AT_BREA) | (1L << AT_SPIT) | (1L << AT_GAZE) | (1L << AT_BEAM);

	/* was: (attacktype(ptr, AT_BREA) || attacktype(ptr, AT_WEAP) ||
		attacktype(ptr, AT_SPIT) || attacktype(ptr, AT_GAZE) ||
		attacktype(ptr, AT_MAGC));
	   but that's too slow -dlc
	 */
	for(i = 0; i < NATTK; i++) {
	    atyp = ptr->mattk[i].aatyp;
	    if (atyp >= AT_WEAP) return TRUE;
	    if (atyp == AT_HUGS) return TRUE;
	 /* assert(atyp < 32); */
	    if ((atk_mask & (1L << atyp)) != 0L) return TRUE;
	}

	return FALSE;
}

/* true iff the type of monster pass through iron bars */
boolean
passes_bars(mptr)
struct permonst *mptr;
{
    return (boolean) (passes_walls(mptr) || amorphous(mptr) ||
		      is_whirly(mptr) || verysmall(mptr) ||
		      (slithy(mptr) && !bigmonst(mptr)));
}

boolean
is_infrastructure_monster(mtmp)
struct monst *mtmp;
{
	if (!mtmp) return FALSE; /* shouldn't happen */

	if (mtmp->isshk || mtmp->isgd || mtmp->ispriest) return TRUE;

	if (mtmp->data == &mons[PM_CROUPIER]) return TRUE;
	if (mtmp->data == &mons[PM_MASTER_CROUPIER]) return TRUE;
	if (mtmp->data == &mons[PM_ELITE_CROUPIER]) return TRUE;
	if (mtmp->data == &mons[PM_EXPERIENCED_CROUPIER]) return TRUE;
	if (mtmp->data == &mons[PM_EXCEPTIONAL_CROUPIER]) return TRUE;

	if (mtmp->data == &mons[PM_SHOPKEEPER]) return TRUE;
	if (mtmp->data == &mons[PM_MASTER_SHOPKEEPER]) return TRUE;
	if (mtmp->data == &mons[PM_ELITE_SHOPKEEPER]) return TRUE;
	if (mtmp->data == &mons[PM_EXPERIENCED_SHOPKEEPER]) return TRUE;
	if (mtmp->data == &mons[PM_EXCEPTIONAL_SHOPKEEPER]) return TRUE;

	if (mtmp->data == &mons[PM_GUARD]) return TRUE;
	if (mtmp->data == &mons[PM_MASTER_GUARD]) return TRUE;
	if (mtmp->data == &mons[PM_ELITE_GUARD]) return TRUE;
	if (mtmp->data == &mons[PM_EXPERIENCED_GUARD]) return TRUE;
	if (mtmp->data == &mons[PM_EXCEPTIONAL_GUARD]) return TRUE;

	if (mtmp->data == &mons[PM_ALIGNED_PRIEST]) return TRUE;
	if (mtmp->data == &mons[PM_MASTER_PRIEST]) return TRUE;
	if (mtmp->data == &mons[PM_EXPERIENCED_PRIEST]) return TRUE;
	if (mtmp->data == &mons[PM_EXCEPTIONAL_PRIEST]) return TRUE;
	if (mtmp->data == &mons[PM_HIGH_PRIEST]) return TRUE;
	if (mtmp->data == &mons[PM_ELITE_PRIEST]) return TRUE;
	if (mtmp->data == &mons[PM_DNETHACK_ELDER_PRIEST_TM_]) return TRUE;

	return FALSE;
}

#endif /* OVL0 */
#ifdef OVL1

boolean
can_track(ptr)		/* returns TRUE if monster can track well */
	register struct permonst *ptr;
{
	if (uwep && uwep->oartifact == ART_EXCALIBUR)
		return TRUE;
	else
		return((boolean)haseyes(ptr));
}

#endif /* OVL1 */
#ifdef OVLB

boolean
sliparm(ptr)	/* creature will slide out of armor */
	register struct permonst *ptr;
{
	return((boolean)(is_whirly(ptr) || ptr->msize <= MZ_SMALL ||
			 noncorporeal(ptr)));
}

boolean
breakarm(ptr)	/* creature will break out of armor */
	register struct permonst *ptr;
{
	return ((bigmonst(ptr) /*|| (ptr->msize > MZ_SMALL && !humanoid(ptr))*/ ||
		/* special cases of humanoids that cannot wear body armor */
		ptr == &mons[PM_MARILITH] || ptr == &mons[PM_WINGED_GARGOYLE])
	      && !sliparm(ptr));
}
#endif /* OVLB */
#ifdef OVL1

boolean
sticks(ptr)	/* creature sticks other creatures it hits */
	register struct permonst *ptr;
{
	if ((ptr == youmonst.data) && uarmf && uarmf->oartifact == ART_KATRIN_S_PARALYSIS) return 1;
	/* I guess this can very rarely cause bugs if a monster is the same species as you... --Amy */

	return((boolean)(dmgtype(ptr,AD_STCK) || dmgtype(ptr,AD_WRAP) ||
		attacktype(ptr,AT_HUGS)));
}

/* can the monster use wands, hug attacks etc. from infinitely far away? --Amy */
boolean
elongation_monster(ptr)
register struct permonst *ptr;
{
	if (ptr == &mons[PM_UNITDEAD_JOKER]) return TRUE;
	if (ptr == &mons[PM_SHOTGUN_SNIPER]) return TRUE;
	if (ptr == &mons[PM_ELONGATED_KURWA]) return TRUE;
	if (ptr == &mons[PM_LONG_DISTANCE_OWLBEAR]) return TRUE;
	if (ptr == &mons[PM_LONGRANGE_PRICK]) return TRUE;
	if (ptr == &mons[PM_FLUIDATOR_EGERRE]) return TRUE;
	if (ptr == &mons[PM_FLUIDATOR_TRAH]) return TRUE;
	if (ptr == &mons[PM_KOREAN_ENERGY_FIELD]) return TRUE;

	return FALSE;
}

/* is the monster a minotaur (immune to Elbereth)? --Amy */
boolean
is_minotaur(ptr)
register struct permonst *ptr;
{
	if (ptr == &mons[PM_MINOTAUR]) return TRUE;
	if (ptr == &mons[PM_LOWERCASE_MINOTAUR]) return TRUE;
	if (ptr == &mons[PM_MINOTAUR_MAGICIAN]) return TRUE;
	if (ptr == &mons[PM_MINOTAUR_BOXER]) return TRUE;
	if (ptr == &mons[PM_MINOCHEATER]) return TRUE;
	if (ptr == &mons[PM_MINOTAUR_BALLERINA]) return TRUE;
	if (ptr == &mons[PM_MINOTARD]) return TRUE;
	if (ptr == &mons[PM_MINOTAUR_VORE]) return TRUE;
	if (ptr == &mons[PM_ERR_AXE_MINOTAUR]) return TRUE;
	if (ptr == &mons[PM_ROCKTAUR]) return TRUE;
	if (ptr == &mons[PM_PSEUDO_PETTY_MINOTAUR]) return TRUE;
	if (ptr == &mons[PM_SPECHOTAUR]) return TRUE;
	if (ptr == &mons[PM_MINOTAUR_LICH]) return TRUE;
	if (ptr == &mons[PM_MINOTAUR_MIMIC]) return TRUE;
	if (ptr == &mons[PM_CHASING_MINOTAUR]) return TRUE;
	if (ptr == &mons[PM_FAT_MINOTAUR]) return TRUE;
	if (ptr == &mons[PM_CHEATING_MINOTAUR]) return TRUE;
	if (ptr == &mons[PM_EVIL_PATCH_MINOTAUR]) return TRUE;
	if (ptr == &mons[PM_DUMB_MINOTAUR]) return TRUE;
	if (ptr == &mons[PM_GIGOTAUR]) return TRUE;
	if (ptr == &mons[PM_MINOTAUR_PRIESTESS]) return TRUE;
	if (ptr == &mons[PM_MINOTAUR_MATRON]) return TRUE;
	if (ptr == &mons[PM_ELDER_MINOTAUR]) return TRUE;
	if (ptr == &mons[PM_PETTY_ELDER_MINOTAUR]) return TRUE;
	if (ptr == &mons[PM_STEEL_MINOTAUR]) return TRUE;
	if (ptr == &mons[PM_ANCIENT_MINOTAUR]) return TRUE;
	if (ptr == &mons[PM_MINOTAUR_GRANDMASTER]) return TRUE;
	if (ptr == &mons[PM_MINOTAUR_ZOMBIE]) return TRUE;
	if (ptr == &mons[PM_ASTERIOS]) return TRUE;
	if (ptr == &mons[PM_CHIHYU]) return TRUE;
	if (ptr == &mons[PM_MOLOCH]) return TRUE;
	if (ptr == &mons[PM_TAURUSCROTTA]) return TRUE;
	if (ptr == &mons[PM_GRIDOTAUR]) return TRUE;
	if (ptr == &mons[PM_MAULOTAUR]) return TRUE;
	if (ptr == &mons[PM_XORNITAUR]) return TRUE;
	if (ptr == &mons[PM_FISHTAUR]) return TRUE;
	if (ptr == &mons[PM_ROARING_FISHTAUR]) return TRUE;
	if (ptr == &mons[PM_SAVEUP_MINOTAUR]) return TRUE;
	if (ptr == &mons[PM_MIMOTAUR]) return TRUE;
	if (ptr == &mons[PM_MINOTAUR_NPC]) return TRUE;

	return FALSE;

}

/* is the monster from the arabella room (doesn't drown in water or lava while in such a room)? --Amy */
boolean
is_arabellamonster(ptr)
register struct permonst *ptr;
{
	if (ptr->mlet == S_WORM_TAIL) return TRUE;

	if (ptr == &mons[PM_SUPERCHEST]) return TRUE;
	if (ptr == &mons[PM_ULTRACHEST]) return TRUE;
	if (ptr == &mons[PM_TELEPORT_TRAP]) return TRUE;
	if (ptr == &mons[PM_ULTRAYOUSEE]) return TRUE;
	if (ptr == &mons[PM_HIGHSCORE_DUMMY]) return TRUE;
	if (ptr == &mons[PM_DEEP_ROCK]) return TRUE;
	if (ptr == &mons[PM_SUPERDEEP_TYPE]) return TRUE;
	if (ptr == &mons[PM_ENEMY_FROM_LEVEL___]) return TRUE;
	if (ptr == &mons[PM_SUPER_STATUE]) return TRUE;
	if (ptr == &mons[PM_ELTRA_STATUE]) return TRUE;
	if (ptr == &mons[PM_CURSED_CALLED_BANDIT]) return TRUE;
	if (ptr == &mons[PM_HYPER_CAVE_BEAR]) return TRUE;
	if (ptr == &mons[PM_PERCENTS_____NIX]) return TRUE;
	if (ptr == &mons[PM_NIX]) return TRUE;
	if (ptr == &mons[PM_FAKE_CHARACTER]) return TRUE;
	if (ptr == &mons[PM_CHARACTER_EVP]) return TRUE;
	if (ptr == &mons[PM_SPACEWARS_FIGHTER]) return TRUE;
	if (ptr == &mons[PM_UNDEAD_SPACEWARS_FIGHTER]) return TRUE;
	if (ptr == &mons[PM_CAR_DRIVING_SUPERTHIEF]) return TRUE;
	if (ptr == &mons[PM_SUPERJEDI]) return TRUE;
	if (ptr == &mons[PM_DIVISION_THIEF]) return TRUE;
	if (ptr == &mons[PM_DIVISION_JEDI]) return TRUE;
	if (ptr == &mons[PM_CRITICALLY_INJURED_THIEF]) return TRUE;
	if (ptr == &mons[PM_CRITICALLY_INJURED_JEDI]) return TRUE;
	if (ptr == &mons[PM_HUGE_OGRE_THIEF]) return TRUE;
	if (ptr == &mons[PM_GUNNHILD_S_GENERAL_STORE]) return TRUE;
	if (ptr == &mons[PM_SUPERTHIEF]) return TRUE;
	if (ptr == &mons[PM_DIVISION_FOURTH_BOSS]) return TRUE;
	if (ptr == &mons[PM_XXXXXXXXXXXXXXXXXXXX]) return TRUE;
	if (ptr == &mons[PM_DOUBLE_AURORA_BOMBER]) return TRUE;
	if (ptr == &mons[PM_EVC_UFO]) return TRUE;
	if (ptr == &mons[PM_SLICK_RUEA]) return TRUE;
	if (ptr == &mons[PM_SHEER_SPACER]) return TRUE;
	if (ptr == &mons[PM_DIVISION_FIEND]) return TRUE;
	if (ptr == &mons[PM_CRITICALLY_INJURED_PERCENTS]) return TRUE;
	if (ptr == &mons[PM_OGRE_PERCENTS]) return TRUE;
	if (ptr == &mons[PM____BIT_SUBSYSTEM]) return TRUE;
	if (ptr == &mons[PM_YOUR_MAP_SHOWS_]) return TRUE;
	if (ptr == &mons[PM_HEAVILY_INJURED]) return TRUE;
	if (ptr == &mons[PM_YOU_SEE_]) return TRUE;
	if (ptr == &mons[PM_CRITICALLY_INJURED]) return TRUE;
	if (ptr == &mons[PM_IDE_BY__]) return TRUE;
	if (ptr == &mons[PM_VAILABLE__EXIT_ANYWAY_]) return TRUE;
	if (ptr == &mons[PM_MAND_PENDING__MAGIC_SPELL___]) return TRUE;
	if (ptr == &mons[PM_E_PALE_WRAITH_WITH_A_LIGHTNING_STROKE_]) return TRUE;
	if (ptr == &mons[PM_ARABELLA_SHOE]) return TRUE;
	if (ptr == &mons[PM_ANASTASIA_SHOE]) return TRUE;
	if (ptr == &mons[PM_HENRIETTA_SHOE]) return TRUE;
	if (ptr == &mons[PM_KATRIN_SHOE]) return TRUE;
	if (ptr == &mons[PM_JANA_SHOE]) return TRUE;

	return FALSE;

}

/* do ranged weapons pass through this monster every time? --Amy */
boolean
magicpassesthrough(mtmp)
struct monst *mtmp;
{
	if (mtmp->data == &mons[PM_ARABELLA_SHOE]) return TRUE;
	if (mtmp->data == &mons[PM_ANASTASIA_SHOE]) return TRUE;
	if (mtmp->data == &mons[PM_HENRIETTA_SHOE]) return TRUE;
	if (mtmp->data == &mons[PM_KATRIN_SHOE]) return TRUE;
	if (mtmp->data == &mons[PM_JANA_SHOE]) return TRUE;

	return FALSE;
}

/* can the monster swat projectiles away? --Amy */
boolean
swatting_monster(ptr)
register struct permonst *ptr;
{
	if (ptr == &mons[PM_UNATTACKABLE_HUNT_WOLF]) return TRUE;
	if (ptr == &mons[PM_UNATTACKABLE_RABID_WOLF]) return TRUE;
	if (ptr == &mons[PM_URWOLF]) return TRUE;
	if (ptr == &mons[PM_GRRRRR_URWOLF]) return TRUE;
	if (ptr == &mons[PM_DNETHACK_ELDER_PRIEST_TM_]) return TRUE;
	if (ptr == &mons[PM_ATHLEANNIE]) return TRUE;
	if (ptr == &mons[PM_AMMO_TURNER]) return TRUE;
	if (ptr == &mons[PM_FLUIDATOR_ANDER]) return TRUE;
	if (ptr == &mons[PM_FART_ATHLETE]) return TRUE;
	if (ptr == &mons[PM_TOWELBEAR]) return TRUE;
	if (ptr == &mons[PM_LIGHTSABER_ART_JEDI]) return TRUE;
	if (ptr == &mons[PM_MR__CONCLUSIO]) return TRUE;
	if (ptr == &mons[PM_YOUR_GAME_ENDS_NOW]) return TRUE;
	if (ptr == &mons[PM_ELITE_GENDAME]) return TRUE;
	if (ptr == &mons[PM_LILAC_FEMMY]) return TRUE;
	if (ptr == &mons[PM_LOOMING_SVETTE]) return TRUE;
	if (ptr == &mons[PM_OBSCURING_FLIER]) return TRUE;

	return FALSE;
}

/* can the monster use ECM? --Amy */
boolean
ecm_monster(ptr)
register struct permonst *ptr;
{
	if (ptr == &mons[PM_GREEN]) return TRUE;
	if (ptr == &mons[PM_JUNK_BOAT]) return TRUE;
	if (ptr == &mons[PM_LAND_JUNK_BOAT]) return TRUE;
	if (ptr == &mons[PM_ECM_ARCHER]) return TRUE;
	if (ptr == &mons[PM_ECM_NERVE_HEAD]) return TRUE;
	if (ptr == &mons[PM_HYPER_CAVE_BEAR]) return TRUE;

	return FALSE;
}

/* can the monster still move when you stopped time? --Amy */
boolean
immune_timestop(ptr)
register struct permonst *ptr;
{
	if (ptr == &mons[PM_DNETHACK_ELDER_PRIEST_TM_]) return TRUE;
	if (ptr == &mons[PM_ARIANE__LADY_OF_THE_ELEMENTS]) return TRUE;
	if (ptr == &mons[PM_TIKSRVZLLAT]) return TRUE;
	if (ptr == &mons[PM_MIKRAANESIS]) return TRUE;
	if (ptr == &mons[PM_SVEN]) return TRUE;
	if (ptr == &mons[PM_GRANDMASTER_SVEN]) return TRUE;
	if (ptr == &mons[PM_WORLD_PWNZOR]) return TRUE;
	if (dmgtype(ptr, AD_TIME)) return TRUE;

	return FALSE;
}

/* number of horns this type of monster has on its head */
int
num_horns(ptr)
struct permonst *ptr;
{
    switch (monsndx(ptr)) {
    case PM_LAMB:
    case PM_KNAPSACK_LAMB:
    case PM_SHEEP:
    case PM_THEME_SHEEP:
    case PM_KNAPSACK_SHEEP:
    case PM_GOAT:
    case PM_KNAPSACK_GOAT:
    case PM_COW:
    case PM_THEME_COW:
    case PM_HOLY_COW:
    case PM_ARMORED_COW:
    case PM_RABID_COW:
    case PM_KNAPSACK_COW:
    case PM_BULL:
    case PM_RABID_BULL:
    case PM_KNAPSACK_BULL:
    case PM_HORNED_DEVIL:	/* ? "more than one" */
    case PM_NASTY_HORNED_DEVIL:
    case PM_HORNED_REAPER:
    case PM_MINOTAUR:
    case PM_ASMODEUS:
    case PM_BALROG:
    case PM_BLAROG:
    case PM_ANCIPITAL:
    case PM_GIANT_HORNED_TOAD:
	return 2;
    case PM_WHITE_UNICORN:
    case PM_WINGED_WHITE_UNICORN:
    case PM_WINGED_GRAY_UNICORN:
    case PM_WINGED_BLACK_UNICORN:
    case PM_PROTECTED_WHITE_UNICORN:
    case PM_PROTECTED_GRAY_UNICORN:
    case PM_PROTECTED_BLACK_UNICORN:
    case PM_GRAY_UNICORN:
    case PM_BLACK_UNICORN:
    case PM_REGULAR_GIRL_SNEAKER:
    case PM_EXCEPTIONAL_GIRL_SNEAKER:
    case PM_ELITE_GIRL_SNEAKER:
    case PM_SMOKY_QUARTZ_UNICORN:
    case PM_JET_UNICORN:
    case PM_PEARL_UNICORN:
    case PM_BRICK_UNICORN:
    case PM_BIG_BRICK_UNICORN:
    case PM_ICEFLAME_UNICORN:
    case PM_CURICORN:
    case PM_JAPICORN:
    case PM_BLASTICORN:
    case PM_OFFWHITE_UNICORN:
    case PM_PALE_GRAY_UNICORN:
    case PM_BLACKISH_UNICORN:
    case PM_BUTTICORN:
    case PM_SQUEAKICORN:
    case PM_FARTICORN:
    case PM_PICK_HORN:
    case PM_DIG_HORN:
    case PM_PHASE_HORN:
    case PM_MATURE_UNICORN:
    case PM_BUBBLY_UNICORN:
    case PM_REPLICA_UNICORN:
    case PM_ALIEN_UNICORN:
    case PM_SLATE_UNICORN:
    case PM_SELF_FLAGELLATING_UNICORN:
    case PM_ARCANE_LIGHT_UNICORN:
    case PM_ARCANE_MEDIUM_UNICORN:
    case PM_ARCANE_EVIL_UNICORN:
    case PM_WIPED_UNICORN:
    case PM_GRAVY_UNICORN:
    case PM_COLLUDE_UNICORN:
    case PM_SKY_UNICORN:
    case PM_PLAYER_UNICORN:
    case PM_ILLUSION_HORN:
    case PM_BLOODY_HORN:
    case PM_DEVIOUS_HORN:
    case PM_PROSE_UNICORN:
    case PM_UNICORN_ZOMBIE:
    case PM_CEMENT_UNICORN:
    case PM_COCOON_ANTIHERO:
    case PM_EVIL_SAVIOR:
    case PM_BADFICINA:
    case PM_TAR_UNICORN:
    case PM_SLAU_UNICORN:
    case PM_LEYM_UNICORN:
    case PM_STED_UNICORN:
    case PM_BEGIN_UNICORN:
    case PM_WEIT_UNICORN:
    case PM_DORK_UNICORN:
    case PM_ETHER_UNICORN:
    case PM_TITAN_UNICORN:
    case PM_ALLA_UNICORN:
    case PM_WHITE_UNICORN_FOAL:
    case PM_GRAY_UNICORN_FOAL:
    case PM_BLACK_UNICORN_FOAL:
    case PM_PURE_WHITE_UNICORN:
    case PM_HANDSOME_GRAY_UNICORN:
    case PM_PORTER_BLACK_UNICORN:
    case PM_PURPLE_UNICORN:
    case PM_LAST_UNICORN:
    case PM_PINK_UNICORN:
    case PM_ORANGE_UNICORN:
    case PM_CYAN_UNICORN:
    case PM_BROWN_UNICORN:
    case PM_RED_UNICORN:
    case PM_YELLOW_UNICORN:
    case PM_GREEN_UNICORN:
    case PM_ULTRAVIOLET_UNICORN:
    case PM_XRAY_UNICORN:
    case PM_SPOTTED_UNICORN:
    case PM_LESSER_SPOTTED_UNICORN:
    case PM_POLECORN:
    case PM_HUGE_POLECORN:
    case PM_GOLDEN_UNICORN:
    case PM_DESATURATED_UNICORN:
    case PM_DULL_UNICORN:
    case PM_POWERLESS_UNICORN:
    case PM_UNICORN_SKELETON:
    case PM_NIGHTMARE_UNICORN:
    case PM_UNICORN_OF_AMBER:
    case PM_RAINBOW_UNICORN:
    case PM_BIG_PURPLE_UNICORN:
    case PM_GOOD_BLUE_UNICORN:
    case PM_FADING_GRAY_UNICORN:
    case PM_IMPENETRABLE_BLACK_UNICORN:
    case PM_BIG_WHITE_UNICORN:
    case PM_BIG_GRAY_UNICORN:
    case PM_BIG_BLACK_UNICORN:
    case PM_PETTY_BIG_WHITE_UNICORN:
    case PM_PETTY_BIG_GRAY_UNICORN:
    case PM_PETTY_BIG_BLACK_UNICORN:
    case PM_UNDEAD_UNICORN:
    case PM_DISGUISED_UNDEAD_UNICORN:
    case PM_GREAT_WHITE_UNICORN:
    case PM_GREAT_GRAY_UNICORN:
    case PM_GREAT_BLACK_UNICORN:
    case PM_SUPERPOWERED_WHITE_UNICORN:
    case PM_SUPERPOWERED_GRAY_UNICORN:
    case PM_SUPERPOWERED_BLACK_UNICORN:
    case PM_REAR_WHITE_UNICORN:
    case PM_REAR_GRAY_UNICORN:
    case PM_REAR_BLACK_UNICORN:
    case PM_BEHIDE_WHITE_UNICORN:
    case PM_BEHIDE_GRAY_UNICORN:
    case PM_BEHIDE_BLACK_UNICORN:
    case PM_KI_RIN:
    case PM_BEIGE_UNICORN:
    case PM_BLACK_MAGICORN:
    case PM_GRAY_MAGICORN:
    case PM_WHITE_MAGICORN:
    case PM_CONCRETE_UNICORN:
    case PM_ANTHRAZITE_UNICORN:
    case PM_HORNED_DEMON:
    case PM_RHYHORN:
    case PM_BLACKHORN:
    case PM_RHYDON:
    case PM_FROST_CHARGER:
    case PM_RHYPERIOR:
    case PM_OBSIDIAN_LORD:
    case PM_HORN_MONSTER:
    case PM_MUD_RUNNER:
    case PM_SQUIRRELHORN:
    case PM_HORN_HORSE:
	return 1;
    default:
	break;
    }
    return 0;
}

struct attack *
dmgtype_fromattack(ptr, dtyp, atyp)
struct permonst *ptr;
int dtyp, atyp;
{
    struct attack *a;

    for (a = &ptr->mattk[0]; a < &ptr->mattk[NATTK]; a++)
	if (a->adtyp == dtyp && (atyp == AT_ANY || a->aatyp == atyp))
	    return a;

    return (struct attack *)0;
}

boolean
dmgtype(ptr, dtyp)
struct permonst *ptr;
int dtyp;
{
    return dmgtype_fromattack(ptr, dtyp, AT_ANY) ? TRUE : FALSE;
}

/* returns the maximum damage a defender can do to the attacker via
 * a passive defense */
int
max_passive_dmg(mdef, magr)
    register struct monst *mdef, *magr;
{
    int	i, dmg = 0;
    uchar adtyp;

    for(i = 0; i < NATTK; i++)
	if(mdef->data->mattk[i].aatyp == AT_NONE || mdef->data->mattk[i].aatyp == AT_RATH ||
		mdef->data->mattk[i].aatyp == AT_BOOM) {
	    adtyp = mdef->data->mattk[i].adtyp;
	    if ((adtyp == AD_ACID && !resists_acid(magr)) ||
		    (adtyp == AD_COLD && !resists_cold(magr)) ||
		    (adtyp == AD_FIRE && !resists_fire(magr)) ||
		    (adtyp == AD_ELEC && !resists_elec(magr)) ||
		/* prevent pets from killing themselves on those karmic mold's passive attacks --Amy */
		    (adtyp != AD_ACID && adtyp != AD_COLD && adtyp != AD_FIRE && adtyp != AD_ELEC) ) {
		dmg = mdef->data->mattk[i].damn;
		if(!dmg) dmg = mdef->data->mlevel+1;
		dmg *= mdef->data->mattk[i].damd;

		/* buffer --Amy */
		dmg += magr->m_lev;
	    } else dmg = 0;

	    return dmg;
	}
    return 0;
}

#endif /* OVL1 */
#ifdef OVL0

int
monsndx(ptr)		/* return an index into the mons array */
	struct	permonst	*ptr;
{
	register int	i;

	if (ptr == &upermonst) return PM_PLAYERMON;

	i = (int)(ptr - &mons[0]);

	if ((i < LOW_PM || i >= NUMMONS) && !Race_if(PM_MISSINGNO) && !u.ughmemory ) {

		if (i == PM_PLAYERMON) {

			impossible("monsndx() called with -1 ptr; making PM_HUMAN instead");
			return PM_HUMAN;

		}

		/* ought to switch this to use `fmt_ptr' */
	    panic("monsndx - could not index monster (%d)",
		  i);
	    return NON_PM;		/* will not get here */
	}

	return(i);
}

#endif /* OVL0 */
#ifdef OVL1


int
name_to_mon(in_str)
const char *in_str;
{
	/* Be careful.  We must check the entire string in case it was
	 * something such as "ettin zombie corpse".  The calling routine
	 * doesn't know about the "corpse" until the monster name has
	 * already been taken off the front, so we have to be able to
	 * read the name with extraneous stuff such as "corpse" stuck on
	 * the end.
	 * This causes a problem for names which prefix other names such
	 * as "ettin" on "ettin zombie".  In this case we want the _longest_
	 * name which exists.
	 * This also permits plurals created by adding suffixes such as 's'
	 * or 'es'.  Other plurals must still be handled explicitly.
	 */
	register int i;
	register int mntmp = NON_PM;
	register char *s, *str, *term;
	char buf[BUFSZ];
	int len, slen;

	str = strcpy(buf, in_str);

	if (!strncmp(str, "a ", 2)) str += 2;
	else if (!strncmp(str, "an ", 3)) str += 3;

	slen = strlen(str);
	term = str + slen;

	if ((s = strstri(str, "vortices")) != 0)
	    strcpy(s+4, "ex");
	/* be careful with "ies"; "priest", "zombies" */
	else if (slen > 3 && !strcmpi(term-3, "ies") &&
		    (slen < 7 || strcmpi(term-7, "zombies")))
	    strcpy(term-3, "y");
	/* luckily no monster names end in fe or ve with ves plurals */
	else if (slen > 3 && !strcmpi(term-3, "ves"))
	    strcpy(term-3, "f");

	slen = strlen(str); /* length possibly needs recomputing */

    {
	static const struct alt_spl { const char* name; int pm_val; }
	    names[] = {
	    /* Alternate spellings */
		{ "grey dragon",	PM_GRAY_DRAGON },
		{ "baby grey dragon",	PM_BABY_GRAY_DRAGON },
		{ "grey unicorn",	PM_GRAY_UNICORN },
		{ "grey ooze",		PM_GRAY_OOZE },
		{ "gray-elf",		PM_GREY_ELF },
		{ "mindflayer",         PM_MIND_FLAYER },
		{ "master mindflayer",  PM_MASTER_MIND_FLAYER },
	    /* Hyphenated names */
		{ "ki rin",		PM_KI_RIN },
		{ "uruk hai",		PM_URUK_HAI },
		{ "orc captain",	PM_ORC_CAPTAIN },
		{ "woodland elf",	PM_WOODLAND_ELF },
		{ "green elf",		PM_GREEN_ELF },
		{ "grey elf",		PM_GREY_ELF },
		{ "gray elf",		PM_GREY_ELF },
		{ "elf lord",		PM_ELF_LORD },
#if 0	/* OBSOLETE */
		{ "high elf",		PM_HIGH_ELF },
#endif
		{ "olog hai",		PM_OLOG_HAI },
		{ "arch lich",		PM_ARCH_LICH },
	    /* Some irregular plurals */
		{ "incubi",		PM_INCUBUS },
		{ "succubi",		PM_SUCCUBUS },
		{ "violet fungi",	PM_VIOLET_FUNGUS },
		{ "homunculi",		PM_HOMUNCULUS },
		{ "baluchitheria",	PM_BALUCHITHERIUM },
		{ "lurkers above",	PM_LURKER_ABOVE },
		{ "cavemen",		PM_CAVEMAN },
		{ "cavewomen",		PM_CAVEWOMAN },
#ifndef ZOUTHERN
/*		{ "zruties",            PM_ZRUTY },*/
#endif
		{ "djinn",		PM_DJINNI },
		{ "mumakil",		PM_MUMAK },
		{ "erinyes",		PM_ERINYS },
		{ "giant lice",         PM_GIANT_LOUSE },	/* RJ */
	    /* falsely caught by -ves check above */
		{ "master of thief",	PM_MASTER_OF_THIEVES },
	    /* end of list */
		{ 0, 0 }
	};
	register const struct alt_spl *namep;

	for (namep = names; namep->name; namep++)
	    if (!strncmpi(str, namep->name, (int)strlen(namep->name)))
		return namep->pm_val;
    }

	for (len = 0, i = LOW_PM; i < NUMMONS; i++) {
	    register int m_i_len = strlen(mons[i].mname);
	    if (m_i_len > len && !strncmpi(mons[i].mname, str, m_i_len)) {
		if (m_i_len == slen) return i;	/* exact match */
		else if (slen > m_i_len &&
			(str[m_i_len] == ' ' ||
			 !strcmpi(&str[m_i_len], "s") ||
			 !strncmpi(&str[m_i_len], "s ", 2) ||
			 !strcmpi(&str[m_i_len], "'") ||
			 !strncmpi(&str[m_i_len], "' ", 2) ||
			 !strcmpi(&str[m_i_len], "'s") ||
			 !strncmpi(&str[m_i_len], "'s ", 3) ||
			 !strcmpi(&str[m_i_len], "es") ||
			 !strncmpi(&str[m_i_len], "es ", 3))) {
		    mntmp = i;
		    len = m_i_len;
		}
	    }
	}

	if (mntmp == NON_PM) mntmp = title_to_mon(str, (int *)0, (int *)0);
	return mntmp;
}

#endif /* OVL1 */
#ifdef OVL2

/* returns 3 values (0=male, 1=female, 2=none) */
int
gender(mtmp)
register struct monst *mtmp;
{
	if (is_neuter(mtmp->data)) return 2;
	return mtmp->female;
}

/* Like gender(), but lower animals and such are still "it". */
/* This is the one we want to use when printing messages. */
int
pronoun_gender(mtmp)
register struct monst *mtmp;
{
	if (!mtmp->isshk && (is_neuter(mtmp->data) || !canspotmon(mtmp))) return 2;
	return (humanoid(mtmp->data) || (mtmp->data->geno & G_UNIQ) ||
		type_is_pname(mtmp->data)) ? (int)mtmp->female : 2;
}

#endif /* OVL2 */
#ifdef OVLB

/* used for nearby monsters when you go to another level */
boolean
levl_follower(mtmp)
struct monst *mtmp;
{
	/* monsters with the Amulet--even pets--won't follow across levels */
	if (mon_has_amulet(mtmp)) return FALSE;

	/* some monsters will follow even while intending to flee from you */
	if (mtmp->mtame || mtmp->iswiz || is_fshk(mtmp)) return TRUE;

	/* stalking types follow, but won't when fleeing unless you hold
	   the Amulet */
	return (boolean)( ((mtmp->data->mflags2 & M2_STALK) || (!rn2(20) && !is_nonmoving(mtmp->data) && mtmp->data->msound != MS_LEADER) && !mtmp->isshk && !mtmp->ispriest) &&
				(!mtmp->mflee || (u.uhave.amulet && !u.freeplaymode))); /* non-stalking monsters occasionally follow too --Amy */
}

static const int grownups[][2] = {
	{PM_MOSQUITO, PM_GIANT_MOSQUITO},
	{PM_LITTLE_DOG, PM_DOG}, {PM_DOG, PM_LARGE_DOG},
	{PM_LITTLE_GREEN_DOG, PM_GREEN_DOG}, {PM_GREEN_DOG, PM_LARGE_GREEN_DOG},
	{PM_FERRET_KIT, PM_FERRET}, {PM_FERRET, PM_LARGE_FERRET},
	{PM_HELL_HOUND_PUP, PM_HELL_HOUND},
	{PM_ACID_HOUND_PUP, PM_ACID_HOUND},
	{PM_SHOCK_HOUND_PUP, PM_SHOCK_HOUND},
	{PM_HUSKY, PM_BULLDOG},
	{PM_POLICE_DOG, PM_BIG_POLICE_DOG},
	{PM_SMALL_LUGGAGE, PM_MEDIUM_LUGGAGE}, {PM_MEDIUM_LUGGAGE, PM_LARGE_LUGGAGE},

	{PM_AUTOMATIC_LITTLE_DOG, PM_AUTOMATIC_DOG}, {PM_AUTOMATIC_DOG, PM_AUTOMATIC_LARGE_DOG},
	{PM_AUTOMATIC_KITTEN, PM_AUTOMATIC_HOUSECAT}, {PM_AUTOMATIC_HOUSECAT, PM_AUTOMATIC_LARGE_CAT},
	{PM_BLACK_KITTEN, PM_BLACK_HOUSECAT}, {PM_BLACK_HOUSECAT, PM_BLACK_LARGE_CAT},

	{PM_MINION, PM_FANATIC_MINION},
	{PM_TRIAD_SLAVE, PM_FANATIC_TRIAD_SLAVE},
	{PM_SLAYER, PM_BERSERK_SLAYER},
	{PM_YAKUZA, PM_MOVED_YAKUZA},
	{PM_ICE_BOAR, PM_CONSUMED_ICE_BOAR},
	{PM_SOUTHSIDE_HOOD, PM_ANGRY_SOUTHSIDE_HOOD},
	{PM_FIRE_BOAR, PM_CONSUMED_FIRE_BOAR},
	{PM_MAFIA_TYPE, PM_SAVAGE_MAFIA_TYPE},
	{PM_HELL_SPAWN, PM_FRENZIED_HELL_SPAWN},
	{PM_COLOMBIAN_POTATO, PM_SUICIDE_DWARF},
	{PM_ICE_SPAWN, PM_FRENZIED_ICE_SPAWN},
	{PM_YARDIE, PM_AMOK_RUNNING_YARDIE},
	{PM_GREATER_HELL_SPAWN, PM_INSANE_HELL_SPAWN},
	{PM_NINE, PM_INSANE_NINE},
	{PM_GREATER_ICE_SPAWN, PM_INSANE_ICE_SPAWN},
	{PM_RED_JACK, PM_INSANE_RED_JACK},

	{PM_ROCK_MOLE, PM_HUGE_ROCK_MOLE},
	{PM_METAL_MOLE, PM_HUGE_METAL_MOLE},
	{PM_BATTLEHORSE, PM_FIERCE_BATTLEHORSE},
	{PM_BRICK_UNICORN, PM_BIG_BRICK_UNICORN},
	{PM_WHITE_UNICORN, PM_BIG_WHITE_UNICORN},
	{PM_BLACK_UNICORN, PM_BIG_BLACK_UNICORN},
	{PM_PURPLE_UNICORN, PM_BIG_PURPLE_UNICORN},
	{PM_GRAY_UNICORN, PM_BIG_GRAY_UNICORN},
	{PM_PUMA, PM_LARGE_PUMA},
	{PM_KITTEN, PM_HOUSECAT}, {PM_HOUSECAT, PM_LARGE_CAT},
	{PM_GREEN_KITTEN, PM_GREEN_HOUSECAT}, {PM_GREEN_HOUSECAT, PM_LARGE_GREEN_CAT},
	{PM_KOBOLD, PM_LARGE_KOBOLD}, {PM_LARGE_KOBOLD, PM_KOBOLD_LORD},
	{PM_GNOME, PM_GNOME_LORD}, {PM_GNOME_LORD, PM_GNOME_WARRIOR},
	{PM_DWARF, PM_DWARF_LORD}, {PM_DWARF_LORD, PM_DWARF_KING},
	{PM_MIND_FLAYER, PM_MASTER_MIND_FLAYER},
	{PM_ORC, PM_ORC_CAPTAIN}, {PM_HILL_ORC, PM_ORC_CAPTAIN},
	{PM_MORDOR_ORC, PM_ORC_CAPTAIN}, {PM_URUK_HAI, PM_ORC_CAPTAIN},
	{PM_SEWER_RAT, PM_GIANT_RAT},
	{PM_YORA_CHICK, PM_YORA_HAWK},
	{PM_AOA_DROPLET, PM_AOA},
	{PM_SPROW, PM_DRIDER},
	{PM_SMART_TRAPPER, PM_TRAPPER_KING},
	{PM_RHYHORN, PM_RHYDON}, {PM_RHYDON, PM_RHYPERIOR},
	{PM_DUNGEON_FERN_SPROUT, PM_DUNGEON_FERN}, {PM_SWAMP_FERN_SPROUT, PM_SWAMP_FERN},
	{PM_BLAZING_FERN_SPROUT, PM_BLAZING_FERN}, {PM_ARCTIC_FERN_SPROUT, PM_ARCTIC_FERN},
	{PM_BABY_METROID, PM_METROID}, {PM_METROID, PM_ALPHA_METROID},
	{PM_ALPHA_METROID, PM_GAMMA_METROID}, {PM_GAMMA_METROID, PM_ZETA_METROID},
	{PM_ZETA_METROID, PM_OMEGA_METROID}, {PM_OMEGA_METROID, PM_METROID_QUEEN},
	{PM_MONOTON, PM_DUTON}, 	{PM_DUTON, PM_TRITON},
	{PM_TRITON, PM_QUATON}, 	{PM_QUATON, PM_QUINON},
	{PM_QUINON, PM_HEXON}, 	{PM_HEXON, PM_HEPTON},
	{PM_HEPTON, PM_OCTON}, 	{PM_OCTON, PM_NONON},
	{PM_NONON, PM_DECON}, 	{PM_DECON, PM_ICOSON},
	{PM_MALKUTH_SEPHIRAH, PM_YESOD_SEPHIRAH}, 	{PM_YESOD_SEPHIRAH, PM_HOD_SEPHIRAH},
	{PM_HOD_SEPHIRAH, PM_GEVURAH_SEPHIRAH}, 	{PM_GEVURAH_SEPHIRAH, PM_CHOKHMAH_SEPHIRAH},

	{PM_HARROWBARK_WANDLE, PM_HARROWBARK_BOO},
	{PM_HARROWBARK_WALKER, PM_HARROWBARK_FIEND},
	{PM_SAEBR_TOOTHED_CAT, PM_SAEBR_TOOTHED_TIGER},

	{PM_ONANOCTER__A, PM_ONANOCTER__B},
	{PM_ONANOCTER__B, PM_ONANOCTER__C},
	{PM_ONANOCTER__C, PM_ONANOCTER__D},
	{PM_ONANOCTER__D, PM_ONANOCTER__E},
	{PM_ONANOCTER__E, PM_ONANOCTER__F},
	{PM_ONANOCTER__F, PM_ONANOCTER__G},

	{PM_DISINTEGRATOR, PM_MASTER_DISINTEGRATOR},
	{PM_PARALYSATOR_FLY, PM_GREAT_PARALYSATOR_FLY},

	{PM_DREAD_WITCH, PM_DIRE_WITCH},	{PM_DIRE_WITCH, PM_TWISTED_SYLPH},

	{PM_GIANT_RAT, PM_ENORMOUS_RAT},
	{PM_ENORMOUS_RAT, PM_RODENT_OF_UNUSUAL_SIZE},
	{PM_CAVE_SPIDER, PM_GIANT_SPIDER},
	{PM_OGRE, PM_OGRE_LORD}, {PM_OGRE_LORD, PM_OGRE_KING},
	{PM_ELF, PM_ELF_LORD}, {PM_WOODLAND_ELF, PM_ELF_LORD},
	{PM_GREEN_ELF, PM_ELF_LORD}, {PM_GREY_ELF, PM_ELF_LORD},
	{PM_ELF_LORD, PM_ELVENKING}, {PM_NETHER_LICH, PM_LICH},
	{PM_LICH, PM_DEMILICH}, {PM_DEMILICH, PM_MASTER_LICH},
	{PM_MASTER_LICH, PM_ARCH_LICH}, {PM_ARCH_LICH, PM_LICHKING},

	{PM_ON, PM_DEMION}, {PM_DEMION, PM_MASTER_ON},
	{PM_MASTER_ON, PM_ARCHON},

	{PM_ZOO_BAT, PM_GAOL_BAT},

	{PM_BABY_GATOR, PM_GATOR},

	{PM_ICHNEUMON_LARVA, PM_GIANT_ICHNEUMON},

	{PM_FIERY_DRAKKEN, PM_BIG_FIERY_DRAKKEN}, {PM_BIG_FIERY_DRAKKEN, PM_ENORMOUS_FIERY_DRAKKEN},
	{PM_ICY_DRAKKEN, PM_BIG_ICY_DRAKKEN}, {PM_BIG_ICY_DRAKKEN, PM_ENORMOUS_ICY_DRAKKEN},

	{PM_TELAMON, PM_SPLIYARYATID}, {PM_SPLIYARYATID, PM_LOADBEARER},

	{PM_VAMPIRE, PM_VAMPIRE_LORD}, {PM_VAMPIRE_LORD, PM_VAMPIRE_MAGE}, 
	{PM_BAT, PM_GIANT_BAT},
	{PM_CHICKATRICE, PM_COCKATRICE},
	{PM_MAGIKARP, PM_GYARADOS},
	{PM_GOLDEEN, PM_SEAKING},
	{PM_ELF_BOXER, PM_ELF_CHAMPION},
	{PM_FUNNY_GIRL, PM_FUNNY_LADY},
	{PM_JOYOUS_GIRL, PM_JOYOUS_LADY},
	{PM_MAUD_AGENT, PM_DUAM_XNAHT_AGENT},
	{PM_DUAM_XNAHT_AGENT, PM_MAUD_BADASS},

	{PM_DELUGE_AGENT, PM_FLOOD_SURFER},
	{PM_LAVA_LOVER, PM_CRISPY_BURNER},
	{PM_TREE_HUGGER, PM_ECOFRIENDLY_RESEARCHER},
	{PM_NORTHERNER, PM_COLD_PERSON},
	{PM_FORMER_PRISONER, PM_JOBLESS_WARDEN},
	{PM_PARANOID_SCHIZO, PM_SUPERSTITIOUS_PERSON},
	{PM_ACID_TRIPPER, PM_DRUG_ABUSER},
	{PM_SNOWBALL_THROWER, PM_ICE_REGENT},
	{PM_FRIGHTENING_CREEP, PM_TEETHLESS_CRONE},
	{PM_FIREBREATHER, PM_FLAME_PUNISHER},
	{PM_ELECTRO_SHOCKER, PM_TAZER},
	{PM_SQUEEZER, PM_JAMCLAMPER},
	{PM_FIENDISH_WARLOCK, PM_OCCULT_WARLOCK},

	{PM_LOWLY_DRAGON, PM_ADULT_DRAGON}, {PM_SLOW_GROWING_DRAGON, PM_TEENIE_DRAGON},
	{PM_TEENIE_DRAGON, PM_YOUNG_DRAGON}, {PM_GROWN_UP_DRAGON, PM_OLD_DRAGON},
	{PM_OLD_DRAGON, PM_VENERABLE_DRAGON}, {PM_YOUNG_DRAGON, PM_GROWN_UP_DRAGON},

	{PM_MINI_JABBERWOCK, PM_SMALL_JABBERWOCK},
	{PM_FALCON, PM_GIANT_FALCON},

	{PM_WATER_GIRL, PM_WATER_LADY},
	{PM_FIRE_GIRL, PM_FIERY_LADY},
	{PM_NATURE_GIRL, PM_NATURE_LADY},
	{PM_ICE_GIRL, PM_ICE_LADY},
	{PM_FEMALE_GUARD, PM_FEMALE_YEOMAN},
	{PM_MENTAL_GIRL, PM_MENTAL_LADY},
	{PM_DRUGGED_GIRL, PM_CRACKWHORE},
	{PM_SNOW_GIRL, PM_SNOW_QUEEN},
	{PM_DEFORMED_GIRL, PM_FEMALE_HULL},
	{PM_DRAGON_GIRL, PM_DRAGON_LADY},
	{PM_SADISTIC_GIRL, PM_SADISTIC_LADY},
	{PM_BDSM_GIRL, PM_BDSM_LADY},
	{PM_MAGICAL_GIRL, PM_MAGICAL_LADY},

	{PM_TINY_PSEUDODRAGON, PM_PSEUDODRAGON},
	{PM_PSEUDODRAGON, PM_RIDING_PSEUDODRAGON},
	{PM_RIDING_PSEUDODRAGON, PM_LARGE_PSEUDODRAGON},
	{PM_LARGE_PSEUDODRAGON, PM_WINGED_PSEUDODRAGON},
	{PM_WINGED_PSEUDODRAGON, PM_HUGE_PSEUDODRAGON},
	{PM_HUGE_PSEUDODRAGON, PM_GIGANTIC_PSEUDODRAGON},

	{PM_WEE_PSUEDODRAGON, PM_SEMI_WEE_PSUEDODRAGON},
	{PM_SEMI_WEE_PSUEDODRAGON, PM_FRIGGIN__HUGE_PSUEDODRAGON},

	{PM_REAL_PUNISHER, PM_MASTER_PUNISHER},
	{PM_STONER, PM_HEAVY_STONER},

	{PM_NIGHTCALF, PM_DOESNIGHTMARE},

	{PM_CHUNNELER, PM_ROCK_CHUNNELER},	{PM_ROCK_CHUNNELER, PM_STEEL_CHUNNELER},

	{PM_TOMB_SCORPION, PM_GOLDENTAIL_SCORPION},
	{PM_ACID_MONSTER, PM_GREAT_ACID_MONSTER},
	{PM_RANDOM_MONSTER, PM_GIANT_RANDOM_MONSTER},
	{PM_HUNGER_HULK, PM_GLUTTONOUS_HULK},
	{PM_DISENCHANTER, PM_BIG_DISENCHANTER},

	{PM_BABY_VOLTIGORE, PM_VOLTIGORE},
	{PM_MINI_TRIAD, PM_TRIAD}, {PM_TRIAD, PM_TRIAD_CHAMPION},

	{PM_HORSEA, PM_SEADRA},	{PM_SEADRA, PM_KINGDRA},
	{PM_DEMENTOR, PM_DEMENTOR_LORD},
	{PM_DEMENTOR_LORD, PM_DEMENTOR_MASTER},
	{PM_CURSED_SPIRIT, PM_DEVILISH_SPIRIT},
	{PM_BABY_DOMESTIC_DRAGON, PM_DOMESTIC_DRAGON},

	{PM_YOUNG_WHITE_DRAGON, PM_YOUNG_ADULT_WHITE_DRAGON}, {PM_YOUNG_ADULT_WHITE_DRAGON, PM_ADULT_WHITE_DRAGON},
	{PM_ADULT_WHITE_DRAGON, PM_OLD_WHITE_DRAGON}, {PM_OLD_WHITE_DRAGON, PM_VERY_OLD_WHITE_DRAGON},
	{PM_VERY_OLD_WHITE_DRAGON, PM_ANCIENT_WHITE_DRAGON},
	{PM_YOUNG_GREEN_DRAGON, PM_YOUNG_ADULT_GREEN_DRAGON}, {PM_YOUNG_ADULT_GREEN_DRAGON, PM_ADULT_GREEN_DRAGON},
	{PM_ADULT_GREEN_DRAGON, PM_OLD_GREEN_DRAGON}, {PM_OLD_GREEN_DRAGON, PM_VERY_OLD_GREEN_DRAGON},
	{PM_VERY_OLD_GREEN_DRAGON, PM_ANCIENT_GREEN_DRAGON},
	{PM_YOUNG_RED_DRAGON, PM_YOUNG_ADULT_RED_DRAGON}, {PM_YOUNG_ADULT_RED_DRAGON, PM_ADULT_RED_DRAGON},
	{PM_ADULT_RED_DRAGON, PM_OLD_RED_DRAGON}, {PM_OLD_RED_DRAGON, PM_VERY_OLD_RED_DRAGON},
	{PM_VERY_OLD_RED_DRAGON, PM_ANCIENT_RED_DRAGON},
	{PM_YOUNG_BLUE_DRAGON, PM_YOUNG_ADULT_BLUE_DRAGON}, {PM_YOUNG_ADULT_BLUE_DRAGON, PM_ADULT_BLUE_DRAGON},
	{PM_ADULT_BLUE_DRAGON, PM_OLD_BLUE_DRAGON}, {PM_OLD_BLUE_DRAGON, PM_VERY_OLD_BLUE_DRAGON},
	{PM_VERY_OLD_BLUE_DRAGON, PM_ANCIENT_BLUE_DRAGON},
	{PM_YOUNG_WEATHER_DRAGON, PM_YOUNG_ADULT_WEATHER_DRAGON}, {PM_YOUNG_ADULT_WEATHER_DRAGON, PM_ADULT_WEATHER_DRAGON},
	{PM_ADULT_WEATHER_DRAGON, PM_OLD_WEATHER_DRAGON}, {PM_OLD_WEATHER_DRAGON, PM_VERY_OLD_WEATHER_DRAGON},
	{PM_VERY_OLD_WEATHER_DRAGON, PM_ANCIENT_WEATHER_DRAGON},
	{PM_YOUNG_DARK_DRAGON, PM_YOUNG_ADULT_DARK_DRAGON}, {PM_YOUNG_ADULT_DARK_DRAGON, PM_ADULT_DARK_DRAGON},
	{PM_ADULT_DARK_DRAGON, PM_OLD_DARK_DRAGON}, {PM_OLD_DARK_DRAGON, PM_VERY_OLD_DARK_DRAGON},
	{PM_VERY_OLD_DARK_DRAGON, PM_ANCIENT_DARK_DRAGON},
	{PM_YOUNG_SILVER_DRAGON, PM_YOUNG_ADULT_SILVER_DRAGON}, {PM_YOUNG_ADULT_SILVER_DRAGON, PM_ADULT_SILVER_DRAGON},
	{PM_ADULT_SILVER_DRAGON, PM_OLD_SILVER_DRAGON}, {PM_OLD_SILVER_DRAGON, PM_VERY_OLD_SILVER_DRAGON},
	{PM_VERY_OLD_SILVER_DRAGON, PM_ANCIENT_SILVER_DRAGON},
	{PM_YOUNG_GRAY_DRAGON, PM_YOUNG_ADULT_GRAY_DRAGON}, {PM_YOUNG_ADULT_GRAY_DRAGON, PM_ADULT_GRAY_DRAGON},
	{PM_ADULT_GRAY_DRAGON, PM_OLD_GRAY_DRAGON}, {PM_OLD_GRAY_DRAGON, PM_VERY_OLD_GRAY_DRAGON},
	{PM_VERY_OLD_GRAY_DRAGON, PM_ANCIENT_GRAY_DRAGON},
	{PM_YOUNG_SHIMMERING_DRAGON, PM_YOUNG_ADULT_SHIMMERING_DRAGON}, {PM_YOUNG_ADULT_SHIMMERING_DRAGON, PM_ADULT_SHIMMERING_DRAGON},
	{PM_ADULT_SHIMMERING_DRAGON, PM_OLD_SHIMMERING_DRAGON}, {PM_OLD_SHIMMERING_DRAGON, PM_VERY_OLD_SHIMMERING_DRAGON},
	{PM_VERY_OLD_SHIMMERING_DRAGON, PM_ANCIENT_SHIMMERING_DRAGON},
	{PM_YOUNG_DEEP_DRAGON, PM_YOUNG_ADULT_DEEP_DRAGON}, {PM_YOUNG_ADULT_DEEP_DRAGON, PM_ADULT_DEEP_DRAGON},
	{PM_ADULT_DEEP_DRAGON, PM_OLD_DEEP_DRAGON}, {PM_OLD_DEEP_DRAGON, PM_VERY_OLD_DEEP_DRAGON},
	{PM_VERY_OLD_DEEP_DRAGON, PM_ANCIENT_DEEP_DRAGON},
	{PM_YOUNG_DEEPSTATE_DRAGON, PM_YOUNG_ADULT_DEEPSTATE_DRAGON}, {PM_YOUNG_ADULT_DEEPSTATE_DRAGON, PM_ADULT_DEEPSTATE_DRAGON},
	{PM_ADULT_DEEPSTATE_DRAGON, PM_OLD_DEEPSTATE_DRAGON}, {PM_OLD_DEEPSTATE_DRAGON, PM_VERY_OLD_DEEPSTATE_DRAGON},
	{PM_VERY_OLD_DEEPSTATE_DRAGON, PM_ANCIENT_DEEPSTATE_DRAGON},
	{PM_YOUNG_ORANGE_DRAGON, PM_YOUNG_ADULT_ORANGE_DRAGON}, {PM_YOUNG_ADULT_ORANGE_DRAGON, PM_ADULT_ORANGE_DRAGON},
	{PM_ADULT_ORANGE_DRAGON, PM_OLD_ORANGE_DRAGON}, {PM_OLD_ORANGE_DRAGON, PM_VERY_OLD_ORANGE_DRAGON},
	{PM_VERY_OLD_ORANGE_DRAGON, PM_ANCIENT_ORANGE_DRAGON},
	{PM_YOUNG_BLACK_DRAGON, PM_YOUNG_ADULT_BLACK_DRAGON}, {PM_YOUNG_ADULT_BLACK_DRAGON, PM_ADULT_BLACK_DRAGON},
	{PM_ADULT_BLACK_DRAGON, PM_OLD_BLACK_DRAGON}, {PM_OLD_BLACK_DRAGON, PM_VERY_OLD_BLACK_DRAGON},
	{PM_VERY_OLD_BLACK_DRAGON, PM_ANCIENT_BLACK_DRAGON},
	{PM_YOUNG_GOLDEN_DRAGON, PM_YOUNG_ADULT_GOLDEN_DRAGON}, {PM_YOUNG_ADULT_GOLDEN_DRAGON, PM_ADULT_GOLDEN_DRAGON},
	{PM_ADULT_GOLDEN_DRAGON, PM_OLD_GOLDEN_DRAGON}, {PM_OLD_GOLDEN_DRAGON, PM_VERY_OLD_GOLDEN_DRAGON},
	{PM_VERY_OLD_GOLDEN_DRAGON, PM_ANCIENT_GOLDEN_DRAGON},
	{PM_YOUNG_STONE_DRAGON, PM_YOUNG_ADULT_STONE_DRAGON}, {PM_YOUNG_ADULT_STONE_DRAGON, PM_ADULT_STONE_DRAGON},
	{PM_ADULT_STONE_DRAGON, PM_OLD_STONE_DRAGON}, {PM_OLD_STONE_DRAGON, PM_VERY_OLD_STONE_DRAGON},
	{PM_VERY_OLD_STONE_DRAGON, PM_ANCIENT_STONE_DRAGON},
	{PM_YOUNG_CYAN_DRAGON, PM_YOUNG_ADULT_CYAN_DRAGON}, {PM_YOUNG_ADULT_CYAN_DRAGON, PM_ADULT_CYAN_DRAGON},
	{PM_ADULT_CYAN_DRAGON, PM_OLD_CYAN_DRAGON}, {PM_OLD_CYAN_DRAGON, PM_VERY_OLD_CYAN_DRAGON},
	{PM_VERY_OLD_CYAN_DRAGON, PM_ANCIENT_CYAN_DRAGON},
	{PM_YOUNG_PSYCHIC_DRAGON, PM_YOUNG_ADULT_PSYCHIC_DRAGON}, {PM_YOUNG_ADULT_PSYCHIC_DRAGON, PM_ADULT_PSYCHIC_DRAGON},
	{PM_ADULT_PSYCHIC_DRAGON, PM_OLD_PSYCHIC_DRAGON}, {PM_OLD_PSYCHIC_DRAGON, PM_VERY_OLD_PSYCHIC_DRAGON},
	{PM_VERY_OLD_PSYCHIC_DRAGON, PM_ANCIENT_PSYCHIC_DRAGON},
	{PM_YOUNG_YELLOW_DRAGON, PM_YOUNG_ADULT_YELLOW_DRAGON}, {PM_YOUNG_ADULT_YELLOW_DRAGON, PM_ADULT_YELLOW_DRAGON},
	{PM_ADULT_YELLOW_DRAGON, PM_OLD_YELLOW_DRAGON}, {PM_OLD_YELLOW_DRAGON, PM_VERY_OLD_YELLOW_DRAGON},
	{PM_VERY_OLD_YELLOW_DRAGON, PM_ANCIENT_YELLOW_DRAGON},
	{PM_YOUNG_MERCURIAL_DRAGON, PM_YOUNG_ADULT_MERCURIAL_DRAGON}, {PM_YOUNG_ADULT_MERCURIAL_DRAGON, PM_ADULT_MERCURIAL_DRAGON},
	{PM_ADULT_MERCURIAL_DRAGON, PM_OLD_MERCURIAL_DRAGON}, {PM_OLD_MERCURIAL_DRAGON, PM_VERY_OLD_MERCURIAL_DRAGON},
	{PM_VERY_OLD_MERCURIAL_DRAGON, PM_ANCIENT_MERCURIAL_DRAGON},
	{PM_YOUNG_RAINBOW_DRAGON, PM_YOUNG_ADULT_RAINBOW_DRAGON}, {PM_YOUNG_ADULT_RAINBOW_DRAGON, PM_ADULT_RAINBOW_DRAGON},
	{PM_ADULT_RAINBOW_DRAGON, PM_OLD_RAINBOW_DRAGON}, {PM_OLD_RAINBOW_DRAGON, PM_VERY_OLD_RAINBOW_DRAGON},
	{PM_VERY_OLD_RAINBOW_DRAGON, PM_ANCIENT_RAINBOW_DRAGON},
	{PM_YOUNG_BLOOD_DRAGON, PM_YOUNG_ADULT_BLOOD_DRAGON}, {PM_YOUNG_ADULT_BLOOD_DRAGON, PM_ADULT_BLOOD_DRAGON},
	{PM_ADULT_BLOOD_DRAGON, PM_OLD_BLOOD_DRAGON}, {PM_OLD_BLOOD_DRAGON, PM_VERY_OLD_BLOOD_DRAGON},
	{PM_VERY_OLD_BLOOD_DRAGON, PM_ANCIENT_BLOOD_DRAGON},
	{PM_YOUNG_PLAIN_DRAGON, PM_YOUNG_ADULT_PLAIN_DRAGON}, {PM_YOUNG_ADULT_PLAIN_DRAGON, PM_ADULT_PLAIN_DRAGON},
	{PM_ADULT_PLAIN_DRAGON, PM_OLD_PLAIN_DRAGON}, {PM_OLD_PLAIN_DRAGON, PM_VERY_OLD_PLAIN_DRAGON},
	{PM_VERY_OLD_PLAIN_DRAGON, PM_ANCIENT_PLAIN_DRAGON},
	{PM_YOUNG_SKY_DRAGON, PM_YOUNG_ADULT_SKY_DRAGON}, {PM_YOUNG_ADULT_SKY_DRAGON, PM_ADULT_SKY_DRAGON},
	{PM_ADULT_SKY_DRAGON, PM_OLD_SKY_DRAGON}, {PM_OLD_SKY_DRAGON, PM_VERY_OLD_SKY_DRAGON},
	{PM_VERY_OLD_SKY_DRAGON, PM_ANCIENT_SKY_DRAGON},
	{PM_YOUNG_WATER_DRAGON, PM_YOUNG_ADULT_WATER_DRAGON}, {PM_YOUNG_ADULT_WATER_DRAGON, PM_ADULT_WATER_DRAGON},
	{PM_ADULT_WATER_DRAGON, PM_OLD_WATER_DRAGON}, {PM_OLD_WATER_DRAGON, PM_VERY_OLD_WATER_DRAGON},
	{PM_VERY_OLD_WATER_DRAGON, PM_ANCIENT_WATER_DRAGON},
	{PM_YOUNG_EVIL_DRAGON, PM_YOUNG_ADULT_EVIL_DRAGON}, {PM_YOUNG_ADULT_EVIL_DRAGON, PM_ADULT_EVIL_DRAGON},
	{PM_ADULT_EVIL_DRAGON, PM_OLD_EVIL_DRAGON}, {PM_OLD_EVIL_DRAGON, PM_VERY_OLD_EVIL_DRAGON},
	{PM_VERY_OLD_EVIL_DRAGON, PM_ANCIENT_EVIL_DRAGON},
	{PM_YOUNG_MAGIC_DRAGON, PM_YOUNG_ADULT_MAGIC_DRAGON}, {PM_YOUNG_ADULT_MAGIC_DRAGON, PM_ADULT_MAGIC_DRAGON},
	{PM_ADULT_MAGIC_DRAGON, PM_OLD_MAGIC_DRAGON}, {PM_OLD_MAGIC_DRAGON, PM_VERY_OLD_MAGIC_DRAGON},
	{PM_VERY_OLD_MAGIC_DRAGON, PM_ANCIENT_MAGIC_DRAGON},
	{PM_YOUNG_FEMINISM_DRAGON, PM_YOUNG_ADULT_FEMINISM_DRAGON}, {PM_YOUNG_ADULT_FEMINISM_DRAGON, PM_ADULT_FEMINISM_DRAGON},
	{PM_ADULT_FEMINISM_DRAGON, PM_OLD_FEMINISM_DRAGON}, {PM_OLD_FEMINISM_DRAGON, PM_VERY_OLD_FEMINISM_DRAGON},
	{PM_VERY_OLD_FEMINISM_DRAGON, PM_ANCIENT_FEMINISM_DRAGON},
	{PM_YOUNG_MAIDRAGON, PM_YOUNG_ADULT_MAIDRAGON}, {PM_YOUNG_ADULT_MAIDRAGON, PM_ADULT_MAIDRAGON},
	{PM_ADULT_MAIDRAGON, PM_OLD_MAIDRAGON}, {PM_OLD_MAIDRAGON, PM_VERY_OLD_MAIDRAGON},
	{PM_VERY_OLD_MAIDRAGON, PM_ANCIENT_MAIDRAGON},
	{PM_YOUNG_NEGATIVE_DRAGON, PM_YOUNG_ADULT_NEGATIVE_DRAGON}, {PM_YOUNG_ADULT_NEGATIVE_DRAGON, PM_ADULT_NEGATIVE_DRAGON},
	{PM_ADULT_NEGATIVE_DRAGON, PM_OLD_NEGATIVE_DRAGON}, {PM_OLD_NEGATIVE_DRAGON, PM_VERY_OLD_NEGATIVE_DRAGON},
	{PM_VERY_OLD_NEGATIVE_DRAGON, PM_ANCIENT_NEGATIVE_DRAGON},
	{PM_YOUNG_HEROIC_DRAGON, PM_YOUNG_ADULT_HEROIC_DRAGON}, {PM_YOUNG_ADULT_HEROIC_DRAGON, PM_ADULT_HEROIC_DRAGON},
	{PM_ADULT_HEROIC_DRAGON, PM_OLD_HEROIC_DRAGON}, {PM_OLD_HEROIC_DRAGON, PM_VERY_OLD_HEROIC_DRAGON},
	{PM_VERY_OLD_HEROIC_DRAGON, PM_ANCIENT_HEROIC_DRAGON},
	{PM_YOUNG_CANCEL_DRAGON, PM_YOUNG_ADULT_CANCEL_DRAGON}, {PM_YOUNG_ADULT_CANCEL_DRAGON, PM_ADULT_CANCEL_DRAGON},
	{PM_ADULT_CANCEL_DRAGON, PM_OLD_CANCEL_DRAGON}, {PM_OLD_CANCEL_DRAGON, PM_VERY_OLD_CANCEL_DRAGON},
	{PM_VERY_OLD_CANCEL_DRAGON, PM_ANCIENT_CANCEL_DRAGON},
	{PM_YOUNG_CORONA_DRAGON, PM_YOUNG_ADULT_CORONA_DRAGON}, {PM_YOUNG_ADULT_CORONA_DRAGON, PM_ADULT_CORONA_DRAGON},
	{PM_ADULT_CORONA_DRAGON, PM_OLD_CORONA_DRAGON}, {PM_OLD_CORONA_DRAGON, PM_VERY_OLD_CORONA_DRAGON},
	{PM_VERY_OLD_CORONA_DRAGON, PM_ANCIENT_CORONA_DRAGON},
	{PM_YOUNG_CONTRO_DRAGON, PM_YOUNG_ADULT_CONTRO_DRAGON}, {PM_YOUNG_ADULT_CONTRO_DRAGON, PM_ADULT_CONTRO_DRAGON},
	{PM_ADULT_CONTRO_DRAGON, PM_OLD_CONTRO_DRAGON}, {PM_OLD_CONTRO_DRAGON, PM_VERY_OLD_CONTRO_DRAGON},
	{PM_VERY_OLD_CONTRO_DRAGON, PM_ANCIENT_CONTRO_DRAGON},
	{PM_YOUNG_MYSTERY_DRAGON, PM_YOUNG_ADULT_MYSTERY_DRAGON}, {PM_YOUNG_ADULT_MYSTERY_DRAGON, PM_ADULT_MYSTERY_DRAGON},
	{PM_ADULT_MYSTERY_DRAGON, PM_OLD_MYSTERY_DRAGON}, {PM_OLD_MYSTERY_DRAGON, PM_VERY_OLD_MYSTERY_DRAGON},
	{PM_VERY_OLD_MYSTERY_DRAGON, PM_ANCIENT_MYSTERY_DRAGON},
	{PM_YOUNG_NOPE_DRAGON, PM_YOUNG_ADULT_NOPE_DRAGON}, {PM_YOUNG_ADULT_NOPE_DRAGON, PM_ADULT_NOPE_DRAGON},
	{PM_ADULT_NOPE_DRAGON, PM_OLD_NOPE_DRAGON}, {PM_OLD_NOPE_DRAGON, PM_VERY_OLD_NOPE_DRAGON},
	{PM_VERY_OLD_NOPE_DRAGON, PM_ANCIENT_NOPE_DRAGON},
	{PM_YOUNG_CRYSTALLINE_DRAGON, PM_YOUNG_ADULT_CRYSTALLINE_DRAGON}, {PM_YOUNG_ADULT_CRYSTALLINE_DRAGON, PM_ADULT_CRYSTALLINE_DRAGON},
	{PM_ADULT_CRYSTALLINE_DRAGON, PM_OLD_CRYSTALLINE_DRAGON}, {PM_OLD_CRYSTALLINE_DRAGON, PM_VERY_OLD_CRYSTALLINE_DRAGON},
	{PM_VERY_OLD_CRYSTALLINE_DRAGON, PM_ANCIENT_CRYSTALLINE_DRAGON},

	{PM_RED_KITE, PM_DRAGONKIN_BROODMASTER},
	{PM_ELECTRIC_GEL, PM_CARNIVOROUS_GEL},

	{PM_DEATHWALKER_KNIGHT, PM_DEATHWALKER_BATTLEMASTER},
	{PM_CONTACT_BEASTLING, PM_CONTACT_BEAST},

	{PM_FIRE_NAGA_HATCHLING, PM_FIRE_NAGA},
	{PM_COBALT_NAGA_HATCHLING, PM_COBALT_NAGA},
	{PM_GRAY_NAGA_HATCHLING, PM_GRAY_NAGA},

	{PM_WILD_WOLF, PM_HUNTING_WOLF},
	{PM_GAY_NAGA, PM_FAGA},
	{PM_HPH_NAGA, PM_MAGA},

	{PM_HELLKITTEN, PM_HELLCAT},

	{PM_GOOD_LITTLE_DOG, PM_NOT_SLOWING_DOWN_DOG},	{PM_NOT_SLOWING_DOWN_DOG, PM_XTRA_LARGE_DOG},
	{PM_POWER_KITTEN, PM_POWERFUL_HOUSECAT},	{PM_POWERFUL_HOUSECAT, PM_LARGE_COMBAT_CAT},

	{PM_DIREWOLF_CUB, PM_DIREWOLF},
	{PM_DIREWOLF, PM_LARGE_DIREWOLF},

	{PM_MYCONID_SCOUT, PM_MYCONID_WARRIOR},
	{PM_MYCONID_WARRIOR, PM_MYCONID_REAVER},
	{PM_MYCONID_REAVER, PM_MYCONID_ELDER},

	{PM_EAGLE, PM_GREAT_EAGLE},

	{PM_BORLOTH, PM_MOONBEAST},	{PM_MOONBEAST, PM_SPACE_BORLOTH},
	{PM_ENBUG, PM_PROGRAMMING_BUG},
	    {PM_PROGRAMMING_BUG, PM_SERIOUS_BUG},
		{PM_SERIOUS_BUG, PM_MEMORY_CORRUPTION},
	{PM_BABY_ROC, PM_ROC},
	{PM_CREEPING_VINE, PM_STRUNGLER_VINE},

	{PM_ITALIAN_GIRL, PM_ITALIAN_LASS},	{PM_ITALIAN_LASS, PM_ITALIAN_WOMAN}, 
	{PM_ITALIAN_WOMAN, PM_ITALIAN_DAME},	{PM_ITALIAN_DAME, PM_ITALIAN_LADY}, 
	{PM_FEMININE_GIRL, PM_FEMININE_LASS},	{PM_FEMININE_LASS, PM_FEMININE_WOMAN}, 
	{PM_FEMININE_WOMAN, PM_FEMININE_DAME},	{PM_FEMININE_DAME, PM_FEMININE_LADY}, 
	{PM_ELEGANT_GIRL, PM_ELEGANT_LASS},	{PM_ELEGANT_LASS, PM_ELEGANT_WOMAN}, 
	{PM_ELEGANT_WOMAN, PM_ELEGANT_DAME},	{PM_ELEGANT_DAME, PM_ELEGANT_LADY}, 

	{PM_TUNNEL_SPIDER, PM_CARRION_FANG},
	{PM_THORNED_STRIDER, PM_THORNED_ELDER},
	{PM_WATCHER_GEIST, PM_WATCHER_OVERLORD},
	{PM_SHADOW_RECLUSE, PM_TUNNEL_QUEEN},
	{PM_DARK_ZEALOT, PM_BLIGHT_QUEEN},

	{PM_BABY_TATZELWORM, PM_ADULT_TATZELWORM},
	{PM_BABY_AMPHITERE, PM_ADULT_AMPHITERE},
	{PM_BABY_DRAKEN, PM_ADULT_DRAKEN},
	{PM_BABY_LINDWORM, PM_ADULT_LINDWORM},
	{PM_BABY_SARKANY, PM_ADULT_SARKANY},
	{PM_BABY_SIRRUSH, PM_ADULT_SIRRUSH},
	{PM_BABY_LEVIATHAN, PM_ADULT_LEVIATHAN},
	{PM_BABY_WYVERN, PM_ADULT_WYVERN},
	{PM_BABY_GLOWING_DRAGON, PM_ADULT_GLOWING_DRAGON},
	{PM_BABY_GUIVRE, PM_ADULT_GUIVRE},

	{PM_CRYSTAL_DRAKE, PM_GREAT_CRYSTAL_DRAKE},

	{PM_ILLITHID_LARVA, PM_NEOTHELID},

	{PM_BABY_MULTI_HUED_DRAGON, PM_MULTI_HUED_DRAGON},
	{PM_BABY_BRONZE_DRAGON, PM_BRONZE_DRAGON},

	{PM_BABY_GRAY_DRAGON, PM_GRAY_DRAGON},
	{PM_BABY_MERCURIAL_DRAGON, PM_MERCURIAL_DRAGON},
	{PM_BABY_RAINBOW_DRAGON, PM_RAINBOW_DRAGON},
	{PM_BABY_BLOOD_DRAGON, PM_BLOOD_DRAGON},
	{PM_BABY_PLAIN_DRAGON, PM_PLAIN_DRAGON},
	{PM_BABY_SKY_DRAGON, PM_SKY_DRAGON},
	{PM_BABY_WATER_DRAGON, PM_WATER_DRAGON},
	{PM_BABY_EVIL_DRAGON, PM_EVIL_DRAGON},
	{PM_BABY_MAGIC_DRAGON, PM_MAGIC_DRAGON},
	{PM_BABY_RED_DRAGON, PM_RED_DRAGON},
	{PM_BABY_SILVER_DRAGON, PM_SILVER_DRAGON},
	{PM_BABY_DEEP_DRAGON, PM_DEEP_DRAGON},
	{PM_BABY_DEEPSTATE_DRAGON, PM_DEEPSTATE_DRAGON},
	{PM_BABY_SHIMMERING_DRAGON, PM_SHIMMERING_DRAGON},
	{PM_BABY_WHITE_DRAGON, PM_WHITE_DRAGON},
	{PM_BABY_ORANGE_DRAGON, PM_ORANGE_DRAGON},
	{PM_BABY_BLACK_DRAGON, PM_BLACK_DRAGON},
	{PM_BABY_BLUE_DRAGON, PM_BLUE_DRAGON},
	{PM_BABY_WEATHER_DRAGON, PM_WEATHER_DRAGON},
	{PM_BABY_DARK_DRAGON, PM_DARK_DRAGON},
	{PM_BABY_GREEN_DRAGON, PM_GREEN_DRAGON},
	{PM_BABY_GOLDEN_DRAGON, PM_GOLDEN_DRAGON},
	{PM_BABY_FEMINISM_DRAGON, PM_FEMINISM_DRAGON},
	{PM_BABY_MAIDRAGON, PM_MAIDRAGON},
	{PM_BABY_CANCEL_DRAGON, PM_CANCEL_DRAGON},
	{PM_BABY_NEGATIVE_DRAGON, PM_NEGATIVE_DRAGON},
	{PM_BABY_CORONA_DRAGON, PM_CORONA_DRAGON},
	{PM_BABY_CONTRO_DRAGON, PM_CONTRO_DRAGON},
	{PM_BABY_NOPE_DRAGON, PM_NOPE_DRAGON},
	{PM_BABY_CRYSTALLINE_DRAGON, PM_CRYSTALLINE_DRAGON},
	{PM_BABY_MYSTERY_DRAGON, PM_MYSTERY_DRAGON},
	{PM_BABY_HEROIC_DRAGON, PM_HEROIC_DRAGON},
	{PM_BABY_YELLOW_DRAGON, PM_YELLOW_DRAGON},
	{PM_BABY_STONE_DRAGON, PM_STONE_DRAGON},
	{PM_BABY_CYAN_DRAGON, PM_CYAN_DRAGON},
	{PM_BABY_PSYCHIC_DRAGON, PM_PSYCHIC_DRAGON},

	{PM_BABY_EMERALD_DRAGON, PM_EMERALD_DRAGON},
	{PM_BABY_COPPER_DRAGON, PM_COPPER_DRAGON},
	{PM_BABY_DIAMOND_DRAGON, PM_DIAMOND_DRAGON},
	{PM_BABY_RUBY_DRAGON, PM_RUBY_DRAGON},
	{PM_BABY_AMETHYST_DRAGON, PM_AMETHYST_DRAGON},
	{PM_BABY_PURPLE_DRAGON, PM_PURPLE_DRAGON},
	{PM_BABY_BRASS_DRAGON, PM_BRASS_DRAGON},
	{PM_BABY_PLATINUM_DRAGON, PM_PLATINUM_DRAGON},
	{PM_BABY_SAPPHIRE_DRAGON, PM_SAPPHIRE_DRAGON},

	{PM_WIMPY_GRAY_DRAGON, PM_GRAY_DRAGOM},
	{PM_WIMPY_MERCURIAL_DRAGON, PM_MERCURIAL_DRAGOM},
	{PM_WIMPY_RAINBOW_DRAGON, PM_RAINBOW_DRAGOM},
	{PM_WIMPY_BLOOD_DRAGON, PM_BLOOD_DRAGOM},
	{PM_WIMPY_PLAIN_DRAGON, PM_PLAIN_DRAGOM},
	{PM_WIMPY_SKY_DRAGON, PM_SKY_DRAGOM},
	{PM_WIMPY_WATER_DRAGON, PM_WATER_DRAGOM},
	{PM_WIMPY_EVIL_DRAGON, PM_EVIL_DRAGOM},
	{PM_WIMPY_MAGIC_DRAGON, PM_MAGIC_DRAGOM},
	{PM_WIMPY_RED_DRAGON, PM_RED_DRAGOM},
	{PM_WIMPY_SILVER_DRAGON, PM_SILVER_DRAGOM},
	{PM_WIMPY_DEEP_DRAGON, PM_DEEP_DRAGOM},
	{PM_WIMPY_SHIMMERING_DRAGON, PM_SHIMMERING_DRAGOM},
	{PM_WIMPY_WHITE_DRAGON, PM_WHITE_DRAGOM},
	{PM_WIMPY_ORANGE_DRAGON, PM_ORANGE_DRAGOM},
	{PM_WIMPY_BLACK_DRAGON, PM_BLACK_DRAGOM},
	{PM_WIMPY_BLUE_DRAGON, PM_BLUE_DRAGOM},
	{PM_WIMPY_WEATHER_DRAGON, PM_WEATHER_DRAGOM},
	{PM_WIMPY_DARK_DRAGON, PM_DARK_DRAGOM},
	{PM_WIMPY_GREEN_DRAGON, PM_GREEN_DRAGOM},
	{PM_WIMPY_GOLDEN_DRAGON, PM_GOLDEN_DRAGOM},
	{PM_WIMPY_FEMINISM_DRAGON, PM_FEMINISM_DRAGOM},
	{PM_WIMPY_MAIDRAGON, PM_MAIDRAGOM},
	{PM_WIMPY_CANCEL_DRAGON, PM_CANCEL_DRAGOM},
	{PM_WIMPY_NEGATIVE_DRAGON, PM_NEGATIVE_DRAGOM},
	{PM_WIMPY_CORONA_DRAGON, PM_CORONA_DRAGOM},
	{PM_WIMPY_CONTRO_DRAGON, PM_CONTRO_DRAGOM},
	{PM_WIMPY_NOPE_DRAGON, PM_NOPE_DRAGOM},
	{PM_WIMPY_CRYSTALLINE_DRAGON, PM_CRYSTALLINE_DRAGOM},
	{PM_WIMPY_MYSTERY_DRAGON, PM_MYSTERY_DRAGOM},
	{PM_WIMPY_HEROIC_DRAGON, PM_HEROIC_DRAGOM},
	{PM_WIMPY_YELLOW_DRAGON, PM_YELLOW_DRAGOM},
	{PM_WIMPY_STONE_DRAGON, PM_STONE_DRAGOM},
	{PM_WIMPY_CYAN_DRAGON, PM_CYAN_DRAGOM},
	{PM_WIMPY_PSYCHIC_DRAGON, PM_PSYCHIC_DRAGOM},

	{PM_WIMPY_EMERALD_DRAGON, PM_EMERALD_DRAGOM},
	{PM_WIMPY_COPPER_DRAGON, PM_COPPER_DRAGOM},
	{PM_WIMPY_DIAMOND_DRAGON, PM_DIAMOND_DRAGOM},
	{PM_WIMPY_RUBY_DRAGON, PM_RUBY_DRAGOM},
	{PM_WIMPY_AMETHYST_DRAGON, PM_AMETHYST_DRAGOM},
	{PM_WIMPY_PURPLE_DRAGON, PM_PURPLE_DRAGOM},
	{PM_WIMPY_BRASS_DRAGON, PM_BRASS_DRAGOM},
	{PM_WIMPY_PLATINUM_DRAGON, PM_PLATINUM_DRAGOM},
	{PM_WIMPY_SAPPHIRE_DRAGON, PM_SAPPHIRE_DRAGOM},

	{PM_CORONA_NAGA_HATCHLING, PM_CORONA_NAGA},
	{PM_BUBBLE_NAGA_HATCHLING, PM_BUBBLE_NAGA},
	{PM_ORNATE_NAGA_HATCHLING, PM_ORNATE_NAGA},
	{PM_MAGENTA_NAGA_HATCHLING, PM_MAGENTA_NAGA},
	{PM_CYAN_NAGA_HATCHLING, PM_CYAN_NAGA},
	{PM_PASTEL_NAGA_HATCHLING, PM_PASTEL_NAGA},
	{PM_TOME_NAGA_HATCHLING, PM_TOME_NAGA},
	{PM_ILLUSION_NAGA_HATCHLING, PM_ILLUSION_NAGA},
	{PM_ANTHRAZITE_NAGA_HATCHLING, PM_ANTHRAZITE_NAGA},
	{PM_STEAM_NAGA_HATCHLING, PM_STEAM_NAGA},
	{PM_LIGHT_NAGA_HATCHLING, PM_LIGHT_NAGA},
	{PM_PYRITE_NAGA_HATCHLING, PM_PYRITE_NAGA},
	{PM_RIP_NAGA_HATCHLING, PM_RIP_NAGA},
	{PM_PERFUMED_NAGA_HATCHLING, PM_PERFUMED_NAGA},
	{PM_OCHRE_NAGA_HATCHLING, PM_OCHRE_NAGA},
	{PM_VIOLET_NAGA_HATCHLING, PM_VIOLET_NAGA},
	{PM_SNOW_NAGA_HATCHLING, PM_SNOW_NAGA},
	{PM_DOME_NAGA_HATCHLING, PM_DOME_NAGA},
	{PM_POND_NAGA_HATCHLING, PM_POND_NAGA},
	{PM_SEA_NAGA_HATCHLING, PM_SEA_NAGA},
	{PM_GREY_NAGA_HATCHLING, PM_GREY_NAGA},
	{PM_TEAL_NAGA_HATCHLING, PM_TEAL_NAGA},
	{PM_YELLOW_NAGA_HATCHLING, PM_YELLOW_NAGA},
	{PM_RED_NAGA_HATCHLING, PM_RED_NAGA},
	{PM_PINK_NAGA_HATCHLING, PM_PINK_NAGA},
	{PM_PURPLE_NAGA_HATCHLING, PM_PURPLE_NAGA},
	{PM_SWAMP_NAGA_HATCHLING, PM_SWAMP_NAGA},
	{PM_OCEAN_NAGA_HATCHLING, PM_OCEAN_NAGA},
	{PM_BROWN_NAGA_HATCHLING, PM_BROWN_NAGA},
	{PM_SILVER_NAGA_HATCHLING, PM_SILVER_NAGA},
	{PM_NIGHT_NAGA_HATCHLING, PM_NIGHT_NAGA},
	{PM_TREE_NAGA_HATCHLING, PM_TREE_NAGA},
	{PM_YGGDRASIL_NAGA_HATCHLING, PM_YGGDRASIL_NAGA},
	{PM_ADVENTURER_NAGA_HATCHLING, PM_ADVENTURER_NAGA},

	{PM_LOTUS_NAGA_HATCHLING, PM_LOTUS_NAGA},
	{PM_BIONIC_NAGA_HATCHLING, PM_BIONIC_NAGA},
	{PM_ARABESQUE_NAGA_HATCHLING, PM_ARABESQUE_NAGA},
	{PM_COPPER_NAGA_HATCHLING, PM_COPPER_NAGA},
	{PM_RIVERSIDE_NAGA_HATCHLING, PM_RIVERSIDE_NAGA},
	{PM_STORM_NAGA_HATCHLING, PM_STORM_NAGA},
	{PM_WISDOM_NAGA_HATCHLING, PM_WISDOM_NAGA},

	{PM_NUU_CHUUL, PM_ELDER_CHUUL},
	{PM_BABY_CELESTIAL_DRAGON, PM_CELESTIAL_DRAGON},

	{PM_GLORKUM, PM_MASTER_GLORKUM}, {PM_MASTER_GLORKUM, PM_ARCHGLORKUM}, 

	{PM_WATER_NAGA_HATCHLING, PM_WATER_NAGA},
	{PM_STRIPPER_GIRL, PM_STRIPPER_LADY},
	{PM_GRUESOME_NAGA_HATCHLING, PM_GRUESOME_NAGA},
	{PM_UNDEAD_NAGA_HATCHLING, PM_UNDEAD_NAGA},
	{PM_SIZZLE_NAGA_HATCHLING, PM_SIZZLE_NAGA},
	{PM_BLADE_NAGA_HATCHLING, PM_BLADE_NAGA},
	{PM_MULTI_HUED_NAGA_HATCHLING, PM_MULTI_HUED_NAGA},
	{PM_SPIRIT_NAGA_HATCHLING, PM_SPIRIT_NAGA},
	{PM_NOVICE_LIGHT_WALL, PM_APPRENTICE_LIGHT_WALL}, 	{PM_APPRENTICE_LIGHT_WALL, PM_JOURNEYMAN_LIGHT_WALL},
	{PM_JOURNEYMAN_LIGHT_WALL, PM_EXPERT_LIGHT_WALL}, 	{PM_EXPERT_LIGHT_WALL, PM_MASTER_LIGHT_WALL},
	{PM_NOVICE_MEDIUM_WALL, PM_APPRENTICE_MEDIUM_WALL}, 	{PM_APPRENTICE_MEDIUM_WALL, PM_JOURNEYMAN_MEDIUM_WALL},
	{PM_JOURNEYMAN_MEDIUM_WALL, PM_EXPERT_MEDIUM_WALL}, 	{PM_EXPERT_MEDIUM_WALL, PM_MASTER_MEDIUM_WALL},
	{PM_NOVICE_DARK_WALL, PM_APPRENTICE_DARK_WALL}, 	{PM_APPRENTICE_DARK_WALL, PM_JOURNEYMAN_DARK_WALL},
	{PM_JOURNEYMAN_DARK_WALL, PM_EXPERT_DARK_WALL}, 	{PM_EXPERT_DARK_WALL, PM_MASTER_DARK_WALL},
	{PM_FLOOR_GRUE, PM_DARK_GRUE},	{PM_DARK_GRUE, PM_DARKER_GRUE},	{PM_DARKER_GRUE, PM_DARKEST_GRUE},
	{PM_ICE_GRUE, PM_FROST_GRUE},	{PM_POISON_GRUE, PM_VENOM_GRUE},	{PM_FIRE_GRUE, PM_LAVA_GRUE},
	{PM_LIGHTNING_GRUE, PM_ELECTRIC_GRUE},	{PM_ACID_GRUE, PM_CORROSIVE_GRUE},
	{PM_DROWZEE, PM_HYPNO},
	{PM_KOFFING, PM_WEEZING},
	{PM_MEOWTH, PM_PERSIAN},
	{PM_GEODUDE, PM_GRAVELER},	{PM_GRAVELER, PM_GOLEM},
	{PM_VOLTORB, PM_ELECTRODE},
	{PM_RATTATA, PM_RATICATE},		{PM_HOUNDOUR, PM_HOUNDOOM},
	{PM_SPEAROW, PM_FEAROW},	{PM_ONIX, PM_STEELIX},
	{PM_EKANS, PM_ARBOK},	{PM_GRIMER, PM_MUK},
	{PM_PIKACHU, PM_RAICHU},	{PM_SEEL, PM_DEWGONG},
	{PM_CLEFAIRY, PM_CLEFABLE},{PM_PONYTA, PM_RAPIDASH},
	{PM_VULPIX, PM_NINETALES}, 	{PM_GROWLITHE, PM_ARCANINE},

	{PM_BABY_GRYPHON, PM_GRYPHON},

	{PM_COCKTYPUS, PM_GIANT_COCKTYPUS}, {PM_GIANT_COCKTYPUS, PM_DIRE_COCKTYPUS},

	{PM_NIDORAN_MALE, PM_NIDORINO},{PM_NIDORINO, PM_NIDOKING},
	{PM_NIDORAN_FEMALE, PM_NIDORINA},{PM_NIDORINA, PM_NIDOQUEEN},

	{PM_CATERPIE, PM_METAPOD},{PM_METAPOD, PM_BUTTERFREE},
	{PM_WEEDLE, PM_KAKUNA},{PM_KAKUNA, PM_BEEDRILL},

	{PM_PIDGEY, PM_PIDGEOTTO},{PM_PIDGEOTTO, PM_PIDGEOT},
	{PM_ABRA, PM_KADABRA},{PM_KADABRA, PM_ALAKAZAM},

	{PM_POLIWAG, PM_POLIWHIRL},{PM_POLIWHIRL, PM_POLIWRATH},

	{PM_BELLSPROUT, PM_WEEPINBELL},{PM_WEEPINBELL, PM_VICTREBELL},

	{PM_DRATINI, PM_DRAGONAIR},{PM_DRAGONAIR, PM_DRAGONITE},

	{PM_SWEATY_FEET_GIRL, PM_GIRL_WHO_DOESN_T_TAKE_OFF_HER_SOCKS},
	{PM_STRING_TANGA_GIRL, PM_GIRL_WHO_USES_HER_UNDERWEAR_AS_A_WEAPON},

	{PM_SQUIRTLE, PM_WARTORTLE},{PM_WARTORTLE, PM_BLASTOISE},
	{PM_BULBASAUR, PM_IVYSAUR},{PM_IVYSAUR, PM_VENUSAUR},
	{PM_ODDISH, PM_GLOOM},{PM_GLOOM, PM_VILEPLUME},
	{PM_CHARMANDER, PM_CHARMELEON},{PM_CHARMELEON, PM_CHARIZARD},
	{PM_DELTA_CHARMANDER, PM_DELTA_CHARMELEON},{PM_DELTA_CHARMELEON, PM_DELTA_CHARIZARD},
	{PM_ZUBAT, PM_GOLBAT},	{PM_GOLBAT, PM_CROBAT},
	{PM_KRABBY, PM_KINGLER}, 	{PM_TENTACOOL, PM_TENTACRUEL},
	{PM_GREEN_NAGA_HATCHLING, PM_GREEN_NAGA},
	{PM_NEXUS_NAGA_HATCHLING, PM_NEXUS_NAGA},
	{PM_SLITH_NAGA_HATCHLING, PM_SLITH_NAGA},
	{PM_MACH_NAGA_HATCHLING, PM_MACH_NAGA},
	{PM_WHITE_NAGA_HATCHLING, PM_WHITE_NAGA},
	{PM_DOMESTIC_NAGA_HATCHLING, PM_DOMESTIC_NAGA},
	{PM_BLUE_NAGA_HATCHLING, PM_BLUE_NAGA},
	{PM_BLACK_NAGA_HATCHLING, PM_BLACK_NAGA},
	{PM_WIMPY_NAGA_HATCHLING, PM_WIMPY_NAGA},
	{PM_PLAY_NAGA_HATCHLING, PM_PLAY_NAGA},
	{PM_DARK_NAGA_HATCHLING, PM_DARK_NAGA},
	{PM_GOLDEN_NAGA_HATCHLING, PM_GOLDEN_NAGA},
	{PM_GREMLIN, PM_GREMLIN_LEADER}, 	{PM_GREMLIN_LEADER, PM_GREMLIN_WARLORD},
	{PM_GREMLIN_WARLORD, PM_GREMLIN_DEATHSUCKER},
	{PM_GUARDIAN_NAGA_HATCHLING, PM_GUARDIAN_NAGA},
	{PM_SMALL_MIMIC, PM_LARGE_MIMIC}, {PM_LARGE_MIMIC, PM_GIANT_MIMIC},
	{PM_BABY_LONG_WORM, PM_LONG_WORM},
	{PM_BABY_PURPLE_WORM, PM_PURPLE_WORM},
	{PM_BABY_CROCODILE, PM_CROCODILE},
	{PM_EXEGGCUTE, PM_EXEGGUTOR},
	{PM_STARYU, PM_STARMIE},

	{PM_LESSER_INTRINSIC_EATING_MIMIC, PM_INTRINSIC_EATING_MIMIC},
	{PM_INTRINSIC_EATING_MIMIC, PM_MASTER_INTRINSIC_EATING_MIMIC},

	{PM_GOLDEN_KNIGHT, PM_GOLDEN_HOLY_KNIGHT},

	{PM_SOLDIER, PM_SERGEANT},
	{PM_TEUTON_SOLDIER, PM_SERGEANT},
	{PM_FRANKISH_SOLDIER, PM_SERGEANT},
	{PM_VIDEO_SOLDIER, PM_SERGEANT},
	{PM_BRITISH_SOLDIER, PM_SERGEANT},
	{PM_AMERICAN_SOLDIER, PM_SERGEANT},
	{PM_COPPER_SOLDIER, PM_SERGEANT},
	{PM_ARAB_SOLDIER, PM_SERGEANT},
	{PM_ASIAN_SOLDIER, PM_SERGEANT},
	{PM_MEDIEVAL_SOLDIER, PM_SERGEANT},
	{PM_PAD_SOLDIER, PM_SERGEANT},
	{PM_GAUCHE_SOLDIER, PM_SERGEANT},
	{PM_SEAFARING_SOLDIER, PM_SERGEANT},
	{PM_BYZANTINE_SOLDIER, PM_SERGEANT},
	{PM_CELTIC_SOLDIER, PM_SERGEANT},
	{PM_VANILLA_SOLDIER, PM_SERGEANT},
	{PM_VIKING_SOLDIER, PM_SERGEANT},
	{PM_SWAMP_SOLDIER, PM_SERGEANT},
	{PM_JAVA_SOLDIER, PM_SERGEANT},
	{PM_IBERIAN_SOLDIER, PM_SERGEANT},
	{PM_ROHIRRIM_SOLDIER, PM_SERGEANT},
	{PM_SERGEANT, PM_LIEUTENANT},
	{PM_EXTRATERRESTRIAL_SERGEANT, PM_LIEUTENANT},
	{PM_MINOAN_SERGEANT, PM_LIEUTENANT},
	{PM_HUN_SERGEANT, PM_LIEUTENANT},
	{PM_MONGOL_SERGEANT, PM_LIEUTENANT},
	{PM_ORANGE_SERGEANT, PM_LIEUTENANT},
	{PM_TWOWEAP_SERGEANT, PM_LIEUTENANT},
	{PM_PERSIAN_SERGEANT, PM_LIEUTENANT},
	{PM_KICKING_NINJA, PM_FOURTH_WALL_BREAKER},
	{PM_SOFT_GIRL, PM_SOFT_WOMAN},
	{PM_LIEUTENANT, PM_CAPTAIN},
	{PM_YAMATO_LIEUTENANT, PM_CAPTAIN},
	{PM_CARTHAGE_LIEUTENANT, PM_CAPTAIN},
	{PM_ROMAN_LIEUTENANT, PM_CAPTAIN},
	{PM_CAPTAIN, PM_GENERAL},
	{PM_GOTHIC_CAPTAIN, PM_GENERAL},
	{PM_URBAN_CAMO_CAPTAIN, PM_GENERAL},
	{PM_WATCHMAN, PM_WATCH_CAPTAIN},
	{PM_WATCH_CAPTAIN, PM_WATCH_LIEUTENANT},
	{PM_WATCH_LIEUTENANT, PM_WATCH_LEADER},
	{PM_ALIGNED_PRIEST, PM_HIGH_PRIEST},
	{PM_STUDENT, PM_ARCHEOLOGIST},
	{PM_ATTENDANT, PM_HEALER},
	{PM_PAGE, PM_KNIGHT},
	{PM_ACOLYTE, PM_PRIEST},
	{PM_APPRENTICE, PM_WIZARD},
	{PM_MANES,PM_LEMURE},
	{PM_BARROW_WIGHT,PM_TUNNEL_WIGHT}, {PM_TUNNEL_WIGHT,PM_CASTLE_WIGHT},
	{PM_MUMAK_CALF, PM_DND_MUMAK},
	{PM_WOLLOH_CHICK, PM_WOLLOH},

	{PM_GASTLY, PM_HAUNTER}, 	{PM_HAUNTER, PM_GENGAR},

	{PM_DRAGONBALL_KID, PM_KAMEHAMEHA_FIGHTER},{PM_KAMEHAMEHA_FIGHTER, PM_SUPER_SAIYAN},

	{PM_IRRITATING_GIRL, PM_IRRITATING_WOMAN},{PM_IRRITATING_WOMAN, PM_IRRITATING_LADY},

	{PM_ELONA_GIRL, PM_ELONA_LADY},	{PM_BEAUTIFUL_GIRL, PM_WONDERFUL_LADY},
	{PM_SEXY_CHILD_MODEL, PM_SEXY_MODEL},	{PM_SKATER_GIRL, PM_SKATER_WOMAN},

	{PM_INKA_GIRL, PM_INKA_WOMAN},	{PM_INKA_WOMAN, PM_INKA_LADY},

	{PM_YENDORIAN_GIRL, PM_YENDORIAN_LASS},	{PM_YENDORIAN_LASS, PM_YENDORIAN_WOMAN},
	{PM_YENDORIAN_WOMAN, PM_YENDORIAN_LADY},	{PM_YENDORIAN_LADY, PM_YENDORIAN_GRANNY},
	{PM_DESERT_GIRL, PM_DESERT_LASS},	{PM_DESERT_LASS, PM_DESERT_WOMAN},
	{PM_DESERT_WOMAN, PM_DESERT_LADY},	{PM_DESERT_LADY, PM_DESERT_GRANNY},

	{PM_BABY_SEA_DRAGON, PM_EVIL_SEA_DRAGON},

	{PM_PUMPS_GIRL, PM_PUMPS_WOMAN}, {PM_PUMPS_WOMAN, PM_PUMPS_LADY},

	{PM_KICKBOXING_GIRL, PM_KICKBOXING_LADY},{PM_ESTRELLA_GIRL, PM_ESTRELLARINA},
	{PM_FORSAKEN_GIRL, PM_FORSAKEN_LADY},{PM_SWEET_COQUETTE, PM_EROTICITY_QUEEN},
	{PM_LITTLE_GIRL, PM_YOUNG_ADULT_GIRL}, {PM_LITTLE_BOY, PM_YOUNG_ADULT_BOY},
	{PM_ASIAN_GIRL, PM_ASIAN_WOMAN}, {PM_ASIAN_WOMAN, PM_ASIAN_LADY},
	{PM_TENDER_ASIAN_GIRL, PM_PRETTY_ASIAN_WOMAN}, {PM_PRETTY_ASIAN_WOMAN, PM_SEXY_ASIAN_LADY},
	{PM_DARK_GIRL, PM_DARK_WOMAN}, {PM_DARK_WOMAN, PM_DARK_LADY},
	{PM_MYSTERIOUS_GIRL, PM_MYSTERIOUS_WOMAN}, {PM_MYSTERIOUS_WOMAN, PM_MYSTERIOUS_LADY},
	{PM_REDGUARD_GIRL, PM_REDGUARD_WOMAN}, {PM_REDGUARD_WOMAN, PM_REDGUARD_LADY},
	{PM_HISPANIC_GIRL, PM_HISPANIC_WOMAN}, {PM_HISPANIC_WOMAN, PM_HISPANIC_LADY},
	{PM_ACTIVISTIC_GIRL, PM_ACTIVISTIC_WOMAN}, {PM_ACTIVISTIC_WOMAN, PM_ACTIVISTIC_LADY},
	{PM_ODOROUS_GIRL, PM_ODOROUS_WOMAN}, {PM_ODOROUS_WOMAN, PM_ODOROUS_LADY},
	{PM_THIEVING_GIRL, PM_THIEVING_WOMAN}, {PM_THIEVING_WOMAN, PM_THIEVING_LADY},
	{PM_CRAPPY_BRAT, PM_SPOILED_PRINCESS}, {PM_SPOILED_PRINCESS, PM_HAUGHTY_BITCH},
	{PM_YOUNG_ADULT_GIRL, PM_YOUNG_WOMAN}, {PM_YOUNG_ADULT_BOY, PM_YOUNG_MAN},
	{PM_YOUNG_WOMAN, PM_ADULT_LADY}, {PM_YOUNG_MAN, PM_ADULT_GENTLEMAN},
	{PM_ADULT_LADY, PM_OLD_GRANNY}, {PM_ADULT_GENTLEMAN, PM_OLD_GRANDPA},
	{PM_NINJA_GIRL, PM_TAEKWONDO_GIRL}, {PM_NINJA_BOY, PM_BLACKBELT},
	{PM_DANCING_GIRL, PM_DANCING_QUEEN}, {PM_DANCING_GUY, PM_DANCING_KING},
	{PM_YOGA_GIRL, PM_TAI_CHI_GIRL}, {PM_SEXY_GIRL, PM_SEXY_LADY}, 
	{PM_NORDIC_GIRL, PM_NORDIC_WOMAN}, {PM_NORDIC_WOMAN, PM_NORDIC_LADY},
	{PM_AMAZONAS_GIRL, PM_AMAZONAS_WOMAN}, {PM_AMAZONAS_WOMAN, PM_AMAZONAS_LADY},
	{PM_OASIS_GIRL, PM_OASIS_WOMAN}, {PM_OASIS_WOMAN, PM_OASIS_LADY},
	{PM_MECH_GIRL, PM_MECH_WOMAN}, {PM_MECH_WOMAN, PM_MECH_LADY},
	{PM_SLEEPY_GIRL, PM_SLEEPY_WOMAN}, {PM_SLEEPY_WOMAN, PM_SLEEPY_LADY},

	{PM_LESBIAN_GIRL, PM_LESBIAN_WOMAN}, {PM_LESBIAN_WOMAN, PM_LESBIAN_BEAUTY}, 
	{PM_GAY_BOY, PM_GAY_MAN}, {PM_GAY_MAN, PM_GAY_MATE}, 

	{PM_CHINESE_GIRL, PM_SEXY_CHINESE_WOMAN}, {PM_SEXY_CHINESE_WOMAN, PM_CHINESE_LADY},
	{PM_JAPANESE_GIRL, PM_JAPANESE_WOMAN}, {PM_JAPANESE_WOMAN, PM_JAPANESE_LADY},
	{PM_KOREAN_GIRL, PM_KOREAN_WOMAN}, {PM_KOREAN_WOMAN, PM_KOREAN_LADY},
	{PM_VIETNAMESE_GIRL, PM_VIETNAMESE_WOMAN}, {PM_VIETNAMESE_WOMAN, PM_VIETNAMESE_LADY},
	{PM_ANIME_GIRL, PM_ANIME_WOMAN}, {PM_ANIME_WOMAN, PM_ANIME_LADY},
	{PM_MANGA_GIRL, PM_MANGA_WOMAN}, {PM_MANGA_WOMAN, PM_MANGA_LADY},

	{PM_TURKISH_GIRL, PM_TURKISH_WOMAN}, {PM_TURKISH_WOMAN, PM_TURKISH_LADY},
	{PM_PERSIAN_GIRL, PM_PERSIAN_WOMAN}, {PM_PERSIAN_WOMAN, PM_PERSIAN_LADY},
	{PM_WUHANESE_GIRL, PM_WUHANESE_WOMAN}, {PM_WUHANESE_WOMAN, PM_WUHANESE_LADY},

	{PM_FARTING_GIRL, PM_FARTING_WOMAN}, {PM_FARTING_BOY, PM_FARTING_MAN},
	{PM_FARTING_WOMAN, PM_FARTING_QUEEN}, {PM_FARTING_MAN, PM_FARTING_KING},

	{PM_RETARD_CHILD, PM_RETARD_TEEN}, {PM_RETARD_TEEN, PM_RETARD_ADULT}, {PM_RETARD_ADULT, PM_RETARD_GIT},

	{PM_BATTLE_BAT, PM_WARBAT},
	{PM_OCTOTHORPE, PM_GIANT_OCTOTHORPE},
	{PM_REAL_OCTOTHORPE, PM_REAL_GIANT_OCTOTHORPE},

	{PM_MINI_SHADE, PM_SHADE},
	{PM_PSUEDOGNOLL, PM_PSUEDOGNOLL_LORD},
	{PM_PSUEDOGNOLL_LORD, PM_PSUEDOGNOLL_KING},
	{PM_FLIND, PM_FLIND_LORD},
	{PM_DRAYCOLICH, PM_ELDER_DRAYCOLICH},

	{PM_SCHINDA, PM_EVOLET},
	{PM_EVOLET, PM_EVOLTE},

	/*{PM_EEVEE, THEEEVEELUTION},*/

	{PM_KEYSTONE_KOP, PM_KOP_SERGEANT},
	{PM_KOP_SERGEANT, PM_KOP_LIEUTENANT},
	{PM_KOP_LIEUTENANT, PM_KOP_KAPTAIN},
	{PM_KOP_KAPTAIN, PM_KOP_KOMMISSIONER},
	{PM_KOP_KOMMISSIONER, PM_KOP_KCHIEF},
	{PM_KOP_KCHIEF, PM_KOP_KATCHER},

	{PM_KOP_PIE, PM_PIE_SERGEANT},
	{PM_PIE_SERGEANT, PM_PIE_LIEUTENANT},
	{PM_PIE_LIEUTENANT, PM_PIE_KAPTAIN},
	{PM_PIE_KAPTAIN, PM_PIE_KOMMISSIONER},
	{PM_PIE_KOMMISSIONER, PM_PIE_KCHIEF},
	{PM_PIE_KCHIEF, PM_PIE_KATCHER},

	{PM_KOP_HOSE, PM_HOSE_SERGEANT},
	{PM_HOSE_SERGEANT, PM_HOSE_LIEUTENANT},
	{PM_HOSE_LIEUTENANT, PM_HOSE_KAPTAIN},
	{PM_HOSE_KAPTAIN, PM_HOSE_KOMMISSIONER},
	{PM_HOSE_KOMMISSIONER, PM_HOSE_KCHIEF},
	{PM_HOSE_KCHIEF, PM_HOSE_KATCHER},

	{PM_EQ_KOP, PM_EQ_SERGEANT},
	{PM_EQ_SERGEANT, PM_EQ_LIEUTENANT},
	{PM_EQ_LIEUTENANT, PM_EQ_KAPTAIN},
	{PM_EQ_KAPTAIN, PM_EQ_KOMMISSIONER},
	{PM_EQ_KOMMISSIONER, PM_EQ_KCHIEF},
	{PM_EQ_KCHIEF, PM_EQ_KATCHER},

	{PM_BABY_GLOP_DRAGON, PM_GLOP_DRAGON},
	{PM_YOUNG_GLOP_DRAGON, PM_YOUNG_ADULT_GLOP_DRAGON}, {PM_YOUNG_ADULT_GLOP_DRAGON, PM_ADULT_GLOP_DRAGON},
	{PM_ADULT_GLOP_DRAGON, PM_OLD_GLOP_DRAGON}, {PM_OLD_GLOP_DRAGON, PM_VERY_OLD_GLOP_DRAGON},
	{PM_VERY_OLD_GLOP_DRAGON, PM_ANCIENT_GLOP_DRAGON},

	{PM_YOUNG_POISON_DRAGON, PM_YOUNG_ADULT_POISON_DRAGON}, {PM_YOUNG_ADULT_POISON_DRAGON, PM_ADULT_POISON_DRAGON},
	{PM_ADULT_POISON_DRAGON, PM_OLD_POISON_DRAGON}, {PM_OLD_POISON_DRAGON, PM_VERY_OLD_POISON_DRAGON},
	{PM_VERY_OLD_POISON_DRAGON, PM_ANCIENT_POISON_DRAGON},

	{PM_YOUNG_SLEEP_DRAGON, PM_YOUNG_ADULT_SLEEP_DRAGON}, {PM_YOUNG_ADULT_SLEEP_DRAGON, PM_ADULT_SLEEP_DRAGON},
	{PM_ADULT_SLEEP_DRAGON, PM_OLD_SLEEP_DRAGON}, {PM_OLD_SLEEP_DRAGON, PM_VERY_OLD_SLEEP_DRAGON},
	{PM_VERY_OLD_SLEEP_DRAGON, PM_ANCIENT_SLEEP_DRAGON},

	{PM_YOUNG_FLAME_DRAGON, PM_YOUNG_ADULT_FLAME_DRAGON}, {PM_YOUNG_ADULT_FLAME_DRAGON, PM_ADULT_FLAME_DRAGON},
	{PM_ADULT_FLAME_DRAGON, PM_OLD_FLAME_DRAGON}, {PM_OLD_FLAME_DRAGON, PM_VERY_OLD_FLAME_DRAGON},
	{PM_VERY_OLD_FLAME_DRAGON, PM_ANCIENT_FLAME_DRAGON},

	{PM_YOUNG_SPEAR_DRAGON, PM_YOUNG_ADULT_SPEAR_DRAGON}, {PM_YOUNG_ADULT_SPEAR_DRAGON, PM_ADULT_SPEAR_DRAGON},
	{PM_ADULT_SPEAR_DRAGON, PM_OLD_SPEAR_DRAGON}, {PM_OLD_SPEAR_DRAGON, PM_VERY_OLD_SPEAR_DRAGON},
	{PM_VERY_OLD_SPEAR_DRAGON, PM_ANCIENT_SPEAR_DRAGON},

	{PM_YOUNG_MULTICOLOR_DRAGON, PM_YOUNG_ADULT_MULTICOLOR_DRAGON}, {PM_YOUNG_ADULT_MULTICOLOR_DRAGON, PM_ADULT_MULTICOLOR_DRAGON},
	{PM_ADULT_MULTICOLOR_DRAGON, PM_OLD_MULTICOLOR_DRAGON}, {PM_OLD_MULTICOLOR_DRAGON, PM_VERY_OLD_MULTICOLOR_DRAGON},
	{PM_VERY_OLD_MULTICOLOR_DRAGON, PM_ANCIENT_MULTICOLOR_DRAGON},

	{PM_CTHULHICK, PM_CTHULHIME},

	{PM_RADIO_KOP, PM_RADIO_SERGEANT},
	{PM_RADIO_SERGEANT, PM_RADIO_LIEUTENANT},
	{PM_RADIO_LIEUTENANT, PM_RADIO_KAPTAIN},
	{PM_RADIO_KAPTAIN, PM_RADIO_KOMMISSIONER},
	{PM_RADIO_KOMMISSIONER, PM_RADIO_KCHIEF},
	{PM_RADIO_KCHIEF, PM_RADIO_KATCHER},

	{PM_MINATOR_KOP, PM_MINATOR_SERGEANT},
	{PM_MINATOR_SERGEANT, PM_MINATOR_LIEUTENANT},
	{PM_MINATOR_LIEUTENANT, PM_MINATOR_KAPTAIN},
	{PM_MINATOR_KAPTAIN, PM_MINATOR_KOMMISSIONER},
	{PM_MINATOR_KOMMISSIONER, PM_MINATOR_KCHIEF},
	{PM_MINATOR_KCHIEF, PM_MINATOR_KATCHER},

	{PM_RNG_KOP, PM_RNG_SERGEANT},
	{PM_RNG_SERGEANT, PM_RNG_LIEUTENANT},
	{PM_RNG_LIEUTENANT, PM_RNG_KAPTAIN},
	{PM_RNG_KAPTAIN, PM_RNG_KOMMISSIONER},
	{PM_RNG_KOMMISSIONER, PM_RNG_KCHIEF},
	{PM_RNG_KCHIEF, PM_RNG_KATCHER},

	{PM_SPECTRAL_KOP, PM_SPECTRAL_SERGEANT},
	{PM_SPECTRAL_SERGEANT, PM_SPECTRAL_LIEUTENANT},
	{PM_SPECTRAL_LIEUTENANT, PM_SPECTRAL_KAPTAIN},
	{PM_SPECTRAL_KAPTAIN, PM_SPECTRAL_KOMMISSIONER},
	{PM_SPECTRAL_KOMMISSIONER, PM_SPECTRAL_KCHIEF},
	{PM_SPECTRAL_KCHIEF, PM_SPECTRAL_KATCHER},

	{PM_VAMPIRE_KOP, PM_VAMPIRE_SERGEANT},
	{PM_VAMPIRE_SERGEANT, PM_VAMPIRE_LIEUTENANT},
	{PM_VAMPIRE_LIEUTENANT, PM_VAMPIRE_KAPTAIN},
	{PM_VAMPIRE_KAPTAIN, PM_VAMPIRE_KOMMISSIONER},
	{PM_VAMPIRE_KOMMISSIONER, PM_VAMPIRE_KCHIEF},
	{PM_VAMPIRE_KCHIEF, PM_VAMPIRE_KATCHER},

	{PM_SUCCUBUS_KOP, PM_SUCCUBUS_SERGEANT},
	{PM_SUCCUBUS_SERGEANT, PM_SUCCUBUS_LIEUTENANT},
	{PM_SUCCUBUS_LIEUTENANT, PM_SUCCUBUS_KAPTAIN},
	{PM_SUCCUBUS_KAPTAIN, PM_SUCCUBUS_KOMMISSIONER},
	{PM_SUCCUBUS_KOMMISSIONER, PM_SUCCUBUS_KCHIEF},
	{PM_SUCCUBUS_KCHIEF, PM_SUCCUBUS_KATCHER},

	{PM_YENDORIAN_KOP, PM_YENDORIAN_SERGEANT},
	{PM_YENDORIAN_SERGEANT, PM_YENDORIAN_LIEUTENANT},
	{PM_YENDORIAN_LIEUTENANT, PM_YENDORIAN_KAPTAIN},
	{PM_YENDORIAN_KAPTAIN, PM_YENDORIAN_KOMMISSIONER},
	{PM_YENDORIAN_KOMMISSIONER, PM_YENDORIAN_KCHIEF},
	{PM_YENDORIAN_KCHIEF, PM_YENDORIAN_KATCHER},

	{PM_OFFICER_JENNY, PM_OFFICER_ALINA},
	{PM_OFFICER_ALINA, PM_OFFICER_HANNA},
	{PM_OFFICER_HANNA, PM_OFFICER_JULIA},
	{PM_OFFICER_JULIA, PM_OFFICER_KATHARINA},
	{PM_OFFICER_KATHARINA, PM_OFFICER_ELENA},
	{PM_OFFICER_ELENA, PM_OFFICER_NATALIA},

	{PM_UNDEAD_KOP, PM_UNDEAD_SERGEANT},
	{PM_UNDEAD_SERGEANT, PM_UNDEAD_LIEUTENANT},
	{PM_UNDEAD_LIEUTENANT, PM_UNDEAD_KAPTAIN},
	{PM_UNDEAD_KAPTAIN, PM_UNDEAD_KOMMISSIONER},
	{PM_UNDEAD_KOMMISSIONER, PM_UNDEAD_KCHIEF},
	{PM_UNDEAD_KCHIEF, PM_UNDEAD_KATCHER},

	{PM_KEYSTONE_POLICEMAN, PM_POLICEMAN_SERGEANT},
	{PM_POLICEMAN_SERGEANT, PM_POLICEMAN_LIEUTENANT},
	{PM_POLICEMAN_LIEUTENANT, PM_POLICEMAN_KAPTAIN},
	{PM_POLICEMAN_KAPTAIN, PM_POLICEMAN_KOMMISSIONER},
	{PM_POLICEMAN_KOMMISSIONER, PM_POLICEMAN_KCHIEF},
	{PM_POLICEMAN_KCHIEF, PM_POLICEMAN_KATCHER},

	{PM_KEYSTONE_OFFICER, PM_OFFICER_SERGEANT},
	{PM_OFFICER_SERGEANT, PM_OFFICER_LIEUTENANT},
	{PM_OFFICER_LIEUTENANT, PM_OFFICER_KAPTAIN},
	{PM_OFFICER_KAPTAIN, PM_OFFICER_KOMMISSIONER},
	{PM_OFFICER_KOMMISSIONER, PM_OFFICER_KCHIEF},
	{PM_OFFICER_KCHIEF, PM_OFFICER_KATCHER},

	{PM_KEYSTONE_WARDER, PM_WARDER_SERGEANT},
	{PM_WARDER_SERGEANT, PM_WARDER_LIEUTENANT},
	{PM_WARDER_LIEUTENANT, PM_WARDER_KAPTAIN},
	{PM_WARDER_KAPTAIN, PM_WARDER_KOMMISSIONER},
	{PM_WARDER_KOMMISSIONER, PM_WARDER_KCHIEF},
	{PM_WARDER_KCHIEF, PM_WARDER_KATCHER},

	{PM_KEYSTONE_INSPECTOR, PM_INSPECTOR_SERGEANT},
	{PM_INSPECTOR_SERGEANT, PM_INSPECTOR_LIEUTENANT},
	{PM_INSPECTOR_LIEUTENANT, PM_INSPECTOR_KAPTAIN},
	{PM_INSPECTOR_KAPTAIN, PM_INSPECTOR_KOMMISSIONER},
	{PM_INSPECTOR_KOMMISSIONER, PM_INSPECTOR_KCHIEF},
	{PM_INSPECTOR_KCHIEF, PM_INSPECTOR_KATCHER},

	{PM_KEYSTONE_DETECTIVE, PM_DETECTIVE_SERGEANT},
	{PM_DETECTIVE_SERGEANT, PM_DETECTIVE_LIEUTENANT},
	{PM_DETECTIVE_LIEUTENANT, PM_DETECTIVE_KAPTAIN},
	{PM_DETECTIVE_KAPTAIN, PM_DETECTIVE_KOMMISSIONER},
	{PM_DETECTIVE_KOMMISSIONER, PM_DETECTIVE_KCHIEF},
	{PM_DETECTIVE_KCHIEF, PM_DETECTIVE_KATCHER},

	{PM_BULLETPROOF_KOP, PM_BULLETPROOF_SERGEANT},
	{PM_BULLETPROOF_SERGEANT, PM_BULLETPROOF_LIEUTENANT},
	{PM_BULLETPROOF_LIEUTENANT, PM_BULLETPROOF_KAPTAIN},
	{PM_BULLETPROOF_KAPTAIN, PM_BULLETPROOF_KOMMISSIONER},
	{PM_BULLETPROOF_KOMMISSIONER, PM_BULLETPROOF_KCHIEF},
	{PM_BULLETPROOF_KCHIEF, PM_BULLETPROOF_KATCHER},

	{PM_SMALL_GOAT_SPAWN, PM_GOAT_SPAWN},
	{PM_GOAT_SPAWN, PM_GIANT_GOAT_SPAWN},

	{PM_RAM, PM_LARGE_RAM},
	{PM_LARGE_RAM, PM_WARRAM},

	{PM_BABY_PENGUIN, PM_PENGUIN},
	{PM_PENGUIN, PM_EMPEROR_PENGUIN},
	{PM_EMPEROR_PENGUIN, PM_TUXEDO_PENGUIN},

	{PM_SHOCK_WOLF_CUB, PM_SHOCK_WOLF},
	{PM_GREEN_WOLF_CUB, PM_GREEN_WOLF},
	{PM_FROZEN_TIGER_CUB, PM_FROZEN_TIGER},
	{PM_PANTHER_CUB, PM_PANTHER},
	{PM_BABY_CAMEL, PM_GIANT_CAMEL},
	{PM_BABY_THUNDERBIRD, PM_THUNDERBIRD},
	{PM_BABY_HYDRA, PM_HYDRA},

	{PM_BABY_BROOD_WASP, PM_BROOD_WASP},

	/* WAC -- added killer coin piles */

	{PM_PILE_OF_COPPER_COINS, PM_PILE_OF_SILVER_COINS},
	{PM_PILE_OF_SILVER_COINS, PM_PILE_OF_PLATINUM_COINS},

	{PM_PILE_OF_KILLER_COINS, PM_LARGE_PILE_OF_KILLER_COINS},
	{PM_LARGE_PILE_OF_KILLER_COINS,PM_HUGE_PILE_OF_KILLER_COINS},
	/* KMH -- added more sequences */
	{PM_DINGO_PUPPY, PM_DINGO}, {PM_DINGO, PM_LARGE_DINGO},
	{PM_PONY, PM_HORSE}, {PM_HORSE, PM_WARHORSE},
	{PM_GREEN_PONY, PM_GREEN_HORSE}, {PM_GREEN_HORSE, PM_GREEN_WARHORSE},
	{PM__CHAN_PONY, PM__CHAN_HORSE}, {PM__CHAN_HORSE, PM__CHAN_WARHORSE},
	{PM_LARVA, PM_MAGGOT}, {PM_MAGGOT, PM_DUNG_WORM},
	{PM_WINTER_WOLF_CUB, PM_WINTER_WOLF},
	{PM_GIANT_TICK, PM_GIANT_FLEA}, {PM_GIANT_FLEA, PM_GIANT_LOUSE},	/* RJ */
	/* DS -- growing up, Lethe style */
	{PM_DEEP_ONE, PM_DEEPER_ONE}, {PM_DEEPER_ONE, PM_DEEPEST_ONE},
	{PM_LAMB, PM_SHEEP}, 
	{PM_SHOGGOTH, PM_GIANT_SHOGGOTH},
	{PM_GNOLL, PM_GNOLL_WARRIOR}, {PM_GNOLL_WARRIOR, PM_GNOLL_CHIEFTAIN},
	{PM_MIGO_DRONE, PM_MIGO_WARRIOR}, {PM_TSETSE_FLY, PM_TRETRE_FLY},
	{PM_GELATINOUS_CUBE, PM_GELATINOUS_DICE}, {PM_COCKATRICE, PM_RUBBER_CHICKEN},
	{PM_RUBBER_CHICKEN, PM_TURBO_CHICKEN}, {PM_FLOATING_EYE, PM_SWEEPING_EYE},
	{PM_HOBBIT, PM_HOBBIT_LORD}, {PM_HOBBIT_LORD, PM_HOBBIT_KING},
	{PM_HOBBIT_THIEF, PM_HOBBIT_BRIGAND_THIEF}, {PM_HOBBIT_BRIGAND_THIEF, PM_HOBBIT_MASTER_THIEF},
	{PM_DWARF_THIEF, PM_DWARVEN_BRIGAND_THIEF}, {PM_DWARVEN_BRIGAND_THIEF, PM_DWARVEN_MASTER_THIEF},
	{PM_DARK_THIEF, PM_DARKER_THIEF}, {PM_DARKER_THIEF, PM_DARKEST_THIEF},
	{PM_BRAIN_EATER, PM_MIND_SUCKER}, {PM_MIND_SUCKER, PM_MIND_FLAYER}, {PM_MINOR_MIND_FLAYER, PM_MIND_FLAYER},
	{PM_MASTER_MIND_FLAYER, PM_GRANDMASTER_MIND_FLAYER}, {PM_LEPRECHAUN, PM_MEAN_ROBBER},
	{PM_GIANT_TRAPDOOR_SPIDER, PM_BLACK_WIDOW}, {PM_GIANT_SPIDER, PM_GIANT_TRAPDOOR_SPIDER}, 
	{PM_AGGRESSIVE_LICHEN, PM_POISON_LICHEN}, 
	{PM_LICHEN, PM_POISON_LICHEN}, {PM_POISON_LICHEN, PM_ORE_LICHEN}, 
	{PM_ORE_LICHEN, PM_MASTER_LICHEN}, {PM_MASTER_LICHEN, PM_GRANDMASTER_LICHEN}, 

	{PM_STALKING_LICHEN, PM_STALKING_POISON_LICHEN}, {PM_STALKING_POISON_LICHEN, PM_STALKING_ORE_LICHEN}, 
	{PM_STALKING_ORE_LICHEN, PM_STALKING_MASTER_LICHEN}, {PM_STALKING_MASTER_LICHEN, PM_STALKING_GRANDMASTER_LICHEN}, 

	{PM_SPORE_LICHEN, PM_POISON_SPORE_LICHEN}, {PM_POISON_SPORE_LICHEN, PM_ORE_SPORE_LICHEN}, 
	{PM_ORE_SPORE_LICHEN, PM_MASTER_SPORE_LICHEN}, {PM_MASTER_SPORE_LICHEN, PM_GRANDMASTER_SPORE_LICHEN}, 

	{PM_COLONY_LICHEN, PM_POISON_COLONY_LICHEN}, {PM_POISON_COLONY_LICHEN, PM_ORE_COLONY_LICHEN}, 
	{PM_ORE_COLONY_LICHEN, PM_MASTER_COLONY_LICHEN}, {PM_MASTER_COLONY_LICHEN, PM_GRANDMASTER_COLONY_LICHEN}, 

	{PM_HARDCORE_CATERPILLAR, PM_BELINAY},
	{PM_PROMISING_ALIEN_LARVA, PM_SALMA},
	{PM_WENCHY_HUSSY, PM_ROXY},
	{PM_PERFUME_COMPANION, PM_TATJANA},

	{PM_PETTY_NOISE_HELPER, PM_KATI},
	{PM_NOISE_HELPER, PM_KATI},
	{PM_SQUEAKE_PART, PM_MARIKE},
	{PM_PETTY_LATEBLUTT, PM_BIRGIT},
	{PM_LATEBLUTT, PM_BIRGIT},
	{PM_BLUE_ABSENCE_OF_NOISE, PM_YVONNE},
	{PM_PETTY_EXCITING_ASS, PM_ANTJE},
	{PM_EXCITING_ASS, PM_ANTJE},
	{PM_ASIAN_BRIDE, PM_THAI},

	{PM_GNOME_THIEF, PM_GNOMISH_BRIGAND_THIEF}, {PM_GNOMISH_BRIGAND_THIEF, PM_GNOMISH_MASTER_THIEF},
	{PM_GNOLL_THIEF, PM_GNOLL_ROGUE}, {PM_GNOLL_ROGUE, PM_GNOLL_GANGSTER},
	{PM_OGRE_THIEF, PM_OGRE_BRIGAND_THIEF}, {PM_OGRE_BRIGAND_THIEF, PM_OGRE_MASTER_THIEF},
	{PM_BAD_EGG, PM_ANIMATED_ROTTEN_EGG},
	{PM_STATUE_GARGOYLE, PM_BLOOD_CLAW},
	{PM_BLOOD_CLAW, PM_DEATH_WING},
	{PM_XORN, PM_GREATER_XORN},
	{PM_LESSER_NISHIKIORI, PM_NISHIKIORI},
	{PM_PETIT_MIMIC, PM_SMALL_MIMIC},
	{PM_KILLER_BEE, PM_HUNTER_BEE},
	{PM_SLUG, PM_LORD_SLUG}, {PM_LORD_SLUG, PM_KING_SLUG},
	{PM_WEASEL, PM_LARGE_WEASEL},
	    {PM_LARGE_WEASEL, PM_SILVER_WEASEL},
		{PM_SILVER_WEASEL, PM_WHIRLWIND_WEASEL},
	{PM_RACOON_DOG, PM_LARGE_RACOON_DOG},
	    {PM_LARGE_RACOON_DOG, PM_RACOON_DOG_DRUNKER},
		{PM_RACOON_DOG_DRUNKER, PM_ELDER_RACOON_DOG},
		    {PM_ELDER_RACOON_DOG, PM_MYSTIC_RACOON_DOG},
	{PM_FOX, PM_LARGE_FOX},
	    {PM_LARGE_FOX, PM_SILVER_FOX},
		{PM_SILVER_FOX, PM_ELDER_FOX},
		    {PM_ELDER_FOX, PM_MYSTIC_FOX},
	{PM_RAGING_ORC, PM_ORC_BERSERKER},
	{PM_RAGING_OGRE, PM_OGRE_BERSERKER},
	{PM_SMALL_TARANTULA, PM_REALLY_BIG_TARANTULA},
	{PM_QUANTUM_MECHANIC, PM_QUANTUM_STATISTICAL_MECHANIC},
	{PM_TSUCHINOKO, PM_NODUCHI},
	{PM_NORMAL_FROG, PM_LEOPARD_FROG},
	{PM_GIANT_TOAD, PM_LORD_TOAD},
	    {PM_LORD_TOAD, PM_KING_TOAD},
	{PM_FROGMAN, PM_FROGMAN_LEADER},
	    {PM_FROGMAN_LEADER, PM_FROGMAN_CHIEFTAIN},
	{PM_CHICK, PM_CHICKEN}, {PM_CHICKEN, PM_LARGE_CHICKEN},
	{PM_TURKEY_CHICK, PM_TURKEY},
	{PM_COCKATOO_CHICK, PM_COCKATOO},
	{PM_PARROT_CHICK, PM_PARROT},
	{PM_WOLF_CUB, PM_WOLF},
	{PM_LION_CUB, PM_LION},
	{PM_HELLCAT, PM_PREDATOR_HELLCAT},
	{PM_TASLOI, PM_TASLOI_LOOKOUT},
	    {PM_TASLOI_LOOKOUT, PM_TASLOI_CHIEF},
	{PM_KAPPA, PM_SWAMP_KAPPA},
	{PM_GOBLIN_RAIDER, PM_GOBLIN_LEADER},
	{PM_CRUDE_ORC, PM_ORC_INFANTRY}, {PM_ORC_INFANTRY, PM_ORC_CAPTAIN},
	{PM_MOB_ORC, PM_CROWD_ORC},
	    {PM_CROWD_ORC, PM_WAR_ORC},
	{PM_ISENGARD_ORC, PM_ORC_CAPTAIN},
	{PM_ONSEN_PENGUIN, PM_ADELIE_ONSEN_PENGUIN},
	    {PM_ADELIE_ONSEN_PENGUIN, PM_EMPEROR_ONSEN_PENGUIN},
	{PM_LITTLE_PIG, PM_PIG},
	{PM_LITTLE_WILD_PIG, PM_WILD_PIG},
	{PM_BABY_WILD_BOAR, PM_BOAR},
	{PM_BABY_RIVER_HOG, PM_RED_RIVER_HOG},
	{PM_SILKY_ANTEATER, PM_ANTEATER},
		{PM_ANTEATER, PM_GIANT_ANTEATER},
	{PM_CULF, PM_BULL},
	{PM_SLIMEBESU, PM_SLIMEBESU_LORD},
	{PM_PETIT_PSEUDO_RAT, PM_PSEUDO_RAT},
		{PM_PSEUDO_RAT, PM_LARGE_PSEUDO_RAT},
	{PM_CENTIPEDE, PM_LARGE_CENTIPEDE},
	    {PM_LARGE_CENTIPEDE, PM_BIG_CENTIPEDE},
	{PM_WHITE_UNICORN_FOAL, PM_WHITE_UNICORN},
	{PM_GRAY_UNICORN_FOAL, PM_GRAY_UNICORN},
	{PM_BLACK_UNICORN_FOAL, PM_BLACK_UNICORN},
	{PM_LEOCENTAUR, PM_LEOCENTAUR_CHIEF},
	    {PM_LEOCENTAUR_CHIEF, PM_LEOCENTAUR_KING},
	{PM_TWO_HEADED_OGRE, PM_THREE_HEADED_OGRE},
	{PM_TWO_HEADED_SNAKE, PM_THREE_HEADED_SNAKE},
	{PM_TWO_FOOTED_SNAKE, PM_FOUR_FOOTED_SNAKE},
	    {PM_FOUR_FOOTED_SNAKE, PM_SIX_FOOTED_SNAKE},
	{PM_TWO_HEADED_TROLL, PM_THREE_HEADED_TROLL},
	{PM_BLACK_BEAR_CUB, PM_BLACK_BEAR},
	{PM_WOOD_BEAR_CUB, PM_WOOD_BEAR},
	{PM_LESSER_OMOCHI_BEAST, PM_OMOCHI_BEAST},
	{PM_TUNDRA_BEAR_CUB, PM_TUNDRA_BEAR},
	{PM_ICE_BEAR_CUB, PM_ICE_BEAR},
	{PM_LOWER_HULK, PM_UMBER_HULK},
	{PM_YUKINKO, PM_YUKIONNA},
	{PM_EGGMAN, PM_SOFT_BOILED_EGGMAN},
	    {PM_SOFT_BOILED_EGGMAN, PM_HARD_BOILED_EGGMAN},
	{PM_KOGEPAN, PM_CHARRED_KOGEPAN},
	    {PM_CHARRED_KOGEPAN, PM_BURNED_OUT_KOGEPAN},
	{PM_LITTLE_PISACA, PM_PISACA}, {PM_PISACA, PM_PISACA_LORD},

	{NON_PM,NON_PM}
};

int
little_to_big(montype)
int montype;
{
#ifndef AIXPS2_BUG
	register int i;

	for (i = 0; grownups[i][0] >= LOW_PM; i++)
		if(montype == grownups[i][0]) return grownups[i][1];

		if (montype == PM_EEVEE) return u.eeveelution;
	return montype;
#else
/* AIX PS/2 C-compiler 1.1.1 optimizer does not like the above for loop,
 * and causes segmentation faults at runtime.  (The problem does not
 * occur if -O is not used.)
 * lehtonen@cs.Helsinki.FI (Tapio Lehtonen) 28031990
 */
	int i;
	int monvalue;

	monvalue = montype;
	for (i = 0; grownups[i][0] >= LOW_PM; i++)
		if(montype == grownups[i][0]) monvalue = grownups[i][1];

		if (montype == PM_EEVEE) return u.eeveelution;
	return monvalue;
#endif
}

int
big_to_little(montype)
int montype;
{
	register int i;

	for (i = 0; grownups[i][0] >= LOW_PM; i++)
		if(montype == grownups[i][1]) return grownups[i][0];

		if (montype == u.eeveelution) return PM_EEVEE;
	return montype;
}

/*
 * Return the permonst ptr for the race of the monster.
 * Returns correct pointer for non-polymorphed and polymorphed
 * player.  It does not return a pointer to player role character.
 */
const struct permonst *
raceptr(mtmp)
struct monst *mtmp;
{
    if (mtmp == &youmonst && !Upolyd) return(&mons[urace.malenum]);
    else return(mtmp->data);
}

static const char *levitate[4]	= { "float", "Float", "wobble", "Wobble" };
static const char *flys[4]	= { "fly", "Fly", "flutter", "Flutter" };
static const char *flyl[4]	= { "fly", "Fly", "stagger", "Stagger" };
static const char *slither[4]	= { "slither", "Slither", "falter", "Falter" };
static const char *ooze[4]	= { "ooze", "Ooze", "tremble", "Tremble" };
static const char *immobile[4]	= { "wiggle", "Wiggle", "pulsate", "Pulsate" };
static const char *crawl[4]	= { "crawl", "Crawl", "falter", "Falter" };

const char *
locomotion(ptr, def)
const struct permonst *ptr;
const char *def;
{
	int capitalize = (*def == highc(*def));

	return (
		is_floater(ptr) ? levitate[capitalize] :
		(is_flyer(ptr) && ptr->msize <= MZ_SMALL) ? flys[capitalize] :
		(is_flyer(ptr) && ptr->msize > MZ_SMALL)  ? flyl[capitalize] :
		slithy(ptr)     ? slither[capitalize] :
		amorphous(ptr)  ? ooze[capitalize] :
		!ptr->mmove	? immobile[capitalize] :
		nolimbs(ptr)    ? crawl[capitalize] :
		def
	       );

}

const char *
stagger(ptr, def)
const struct permonst *ptr;
const char *def;
{
	int capitalize = 2 + (*def == highc(*def));

	return (
		is_floater(ptr) ? levitate[capitalize] :
		(is_flyer(ptr) && ptr->msize <= MZ_SMALL) ? flys[capitalize] :
		(is_flyer(ptr) && ptr->msize > MZ_SMALL)  ? flyl[capitalize] :
		slithy(ptr)     ? slither[capitalize] :
		amorphous(ptr)  ? ooze[capitalize] :
		!ptr->mmove	? immobile[capitalize] :
		nolimbs(ptr)    ? crawl[capitalize] :
		def
	       );

}

/* return a phrase describing the effect of fire attack on a type of monster */
const char *
on_fire(mptr, mattk)
struct permonst *mptr;
struct attack *mattk;
{
    const char *what;

    switch (monsndx(mptr)) {
    case PM_FLAMING_SPHERE:
    case PM_SUMMONED_FLAMING_SPHERE:
    case PM_FIRE_VORTEX:
    case PM_FIRE_ELEMENTAL:
    case PM_SALAMANDER:
	what = "already on fire";
	break;
    case PM_WATER_ELEMENTAL:
    case PM_FOG_CLOUD:
    case PM_STEAM_VORTEX:
	what = "boiling";
	break;
    case PM_ICE_VORTEX:
    case PM_GLASS_GOLEM:
	what = "melting";
	break;
    case PM_STONE_GOLEM:
    case PM_CLAY_GOLEM:
    case PM_GOLD_GOLEM:
    case PM_AIR_ELEMENTAL:
    case PM_EARTH_ELEMENTAL:
    case PM_DUST_VORTEX:
    case PM_ENERGY_VORTEX:
	what = "heating up";
	break;
    default:
	what = (mattk->aatyp == AT_HUGS) ? "being roasted" : "on fire";
	break;
    }
    return what;
}

#endif /* OVLB */

/*mondata.c*/
