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
	struct obj *wep = ((mon == &youmonst) ? uwep : MON_WEP(mon));

	return (boolean)(is_undead(ptr) || is_demon(ptr) || is_were(ptr) ||
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

	/* as of 3.2.0:  gray dragons, Angels, Oracle, Yeenoghu */
	if (dmgtype(ptr, AD_MAGM) || ptr == &mons[PM_BABY_GRAY_DRAGON] || ptr == &mons[PM_YOUNG_GRAY_DRAGON] || ptr == &mons[PM_YOUNG_ADULT_GRAY_DRAGON] ||
		dmgtype(ptr, AD_RBRE))	/* Chromatic Dragon */
	    return TRUE;
	/* check for magic resistance granted by wielded weapon */
	o = (mon == &youmonst) ? uwep : MON_WEP(mon);
	if (o && o->oartifact && defends(AD_MAGM, o))
	    return TRUE;
	/* check for magic resistance granted by worn or carried items */
	o = (mon == &youmonst) ? invent : mon->minvent;
	for ( ; o; o = o->nobj)
	    if ((o->owornmask && objects[o->otyp].oc_oprop == ANTIMAGIC) ||
		    (o->oartifact && protects(AD_MAGM, o)))
		return TRUE;
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
	o = is_you ? uwep : MON_WEP(mon);
	if (o && o->oartifact && defends(AD_BLND, o))
	    return TRUE;
	o = is_you ? invent : mon->minvent;
	for ( ; o; o = o->nobj)
	    if ((o->owornmask && objects[o->otyp].oc_oprop == BLINDED) ||
		    (o->oartifact && protects(AD_BLND, o)))
		return TRUE;
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
		if ((o->owornmask & W_ARMH) &&
		    (s = OBJ_DESCR(objects[o->otyp])) != (char *)0 &&
		    !strcmp(s, "visored helmet"))
		    return FALSE;
	}

	return TRUE;
}

#endif /* OVLB */
#ifdef OVL0

boolean
ranged_attk(ptr)	/* returns TRUE if monster can attack at range */
struct permonst *ptr;
{
	register int i, atyp;
	long atk_mask = (1L << AT_BREA) | (1L << AT_SPIT) | (1L << AT_GAZE);

	/* was: (attacktype(ptr, AT_BREA) || attacktype(ptr, AT_WEAP) ||
		attacktype(ptr, AT_SPIT) || attacktype(ptr, AT_GAZE) ||
		attacktype(ptr, AT_MAGC));
	   but that's too slow -dlc
	 */
	for(i = 0; i < NATTK; i++) {
	    atyp = ptr->mattk[i].aatyp;
	    if (atyp >= AT_WEAP) return TRUE;
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
	return ((bigmonst(ptr) || (ptr->msize > MZ_SMALL && !humanoid(ptr)) ||
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
	return((boolean)(dmgtype(ptr,AD_STCK) || dmgtype(ptr,AD_WRAP) ||
		attacktype(ptr,AT_HUGS)));
}

/* number of horns this type of monster has on its head */
int
num_horns(ptr)
struct permonst *ptr;
{
    switch (monsndx(ptr)) {
    case PM_LAMB:
    case PM_SHEEP:
    case PM_GOAT:
    case PM_COW:
    case PM_BULL:
    case PM_HORNED_DEVIL:	/* ? "more than one" */
    case PM_MINOTAUR:
    case PM_ASMODEUS:
    case PM_BALROG:
	return 2;
    case PM_WHITE_UNICORN:
    case PM_GRAY_UNICORN:
    case PM_BLACK_UNICORN:
    case PM_PURPLE_UNICORN:
    case PM_BIG_PURPLE_UNICORN:
    case PM_BIG_WHITE_UNICORN:
    case PM_BIG_GRAY_UNICORN:
    case PM_BIG_BLACK_UNICORN:
    case PM_UNDEAD_UNICORN:
    case PM_KI_RIN:
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
	if(mdef->data->mattk[i].aatyp == AT_NONE ||
		mdef->data->mattk[i].aatyp == AT_BOOM) {
	    adtyp = mdef->data->mattk[i].adtyp;
	    if ((adtyp == AD_ACID && !resists_acid(magr)) ||
		    (adtyp == AD_COLD && !resists_cold(magr)) ||
		    (adtyp == AD_FIRE && !resists_fire(magr)) ||
		    (adtyp == AD_ELEC && !resists_elec(magr)) ||
		    adtyp == AD_PHYS) {
		dmg = mdef->data->mattk[i].damn;
		if(!dmg) dmg = mdef->data->mlevel+1;
		dmg *= mdef->data->mattk[i].damd;
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
	if (i < LOW_PM || i >= NUMMONS) {
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
	    Strcpy(s+4, "ex");
	/* be careful with "ies"; "priest", "zombies" */
	else if (slen > 3 && !strcmpi(term-3, "ies") &&
		    (slen < 7 || strcmpi(term-7, "zombies")))
	    Strcpy(term-3, "y");
	/* luckily no monster names end in fe or ve with ves plurals */
	else if (slen > 3 && !strcmpi(term-3, "ves"))
	    Strcpy(term-3, "f");

	slen = strlen(str); /* length possibly needs recomputing */

    {
	static const struct alt_spl { const char* name; short pm_val; }
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
	return (boolean)((mtmp->data->mflags2 & M2_STALK) &&
				(!mtmp->mflee || u.uhave.amulet));
}

static const short grownups[][2] = {
	{PM_MOSQUITO, PM_GIANT_MOSQUITO},
	{PM_LITTLE_DOG, PM_DOG}, {PM_DOG, PM_LARGE_DOG},
	{PM_HELL_HOUND_PUP, PM_HELL_HOUND},
	{PM_ACID_HOUND_PUP, PM_ACID_HOUND},
	{PM_SHOCK_HOUND_PUP, PM_SHOCK_HOUND},
	{PM_HUSKY, PM_BULLDOG},

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
	{PM_WHITE_UNICORN, PM_BIG_WHITE_UNICORN},
	{PM_BLACK_UNICORN, PM_BIG_BLACK_UNICORN},
	{PM_PURPLE_UNICORN, PM_BIG_PURPLE_UNICORN},
	{PM_GRAY_UNICORN, PM_BIG_GRAY_UNICORN},
	{PM_PUMA, PM_LARGE_PUMA},
	{PM_KITTEN, PM_HOUSECAT}, {PM_HOUSECAT, PM_LARGE_CAT},
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
	{PM_MALKUTH_SEPHIRAH, PM_YESOD_SEPHIRAH}, 	{PM_YESOD_SEPHIRAH, PM_HOD_SEPHIRAH},
	{PM_HOD_SEPHIRAH, PM_GEVURAH_SEPHIRAH}, 	{PM_GEVURAH_SEPHIRAH, PM_CHOKHMAH_SEPHIRAH},

	{PM_HARROWBARK_WANDLE, PM_HARROWBARK_BOO},
	{PM_HARROWBARK_WALKER, PM_HARROWBARK_FIEND},

	{PM_DREAD_WITCH, PM_DIRE_WITCH},	{PM_DIRE_WITCH, PM_TWISTED_SYLPH},

#ifdef CONVICT
	{PM_GIANT_RAT, PM_ENORMOUS_RAT},
	{PM_ENORMOUS_RAT, PM_RODENT_OF_UNUSUAL_SIZE},
#endif	/* CONVICT */
	{PM_CAVE_SPIDER, PM_GIANT_SPIDER},
	{PM_OGRE, PM_OGRE_LORD}, {PM_OGRE_LORD, PM_OGRE_KING},
	{PM_ELF, PM_ELF_LORD}, {PM_WOODLAND_ELF, PM_ELF_LORD},
	{PM_GREEN_ELF, PM_ELF_LORD}, {PM_GREY_ELF, PM_ELF_LORD},
	{PM_ELF_LORD, PM_ELVENKING}, {PM_NETHER_LICH, PM_LICH},
	{PM_LICH, PM_DEMILICH}, {PM_DEMILICH, PM_MASTER_LICH},
	{PM_MASTER_LICH, PM_ARCH_LICH}, {PM_ARCH_LICH, PM_LICHKING},
	{PM_VAMPIRE, PM_VAMPIRE_LORD}, {PM_VAMPIRE_LORD, PM_VAMPIRE_MAGE}, 
	{PM_BAT, PM_GIANT_BAT}, {PM_VAMPIRE_MAGE, PM_VAMPIRE_DAZZLER}, 
	{PM_CHICKATRICE, PM_COCKATRICE},
	{PM_MAGIKARP, PM_GYARADOS},
	{PM_GOLDEEN, PM_SEAKING},
	{PM_ELF_BOXER, PM_ELF_CHAMPION},
	{PM_FUNNY_GIRL, PM_FUNNY_LADY},
	{PM_MAUD_AGENT, PM_DUAM_XNAHT_AGENT},
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

	{PM_CHUNNELER, PM_ROCK_CHUNNELER},	{PM_ROCK_CHUNNELER, PM_STEEL_CHUNNELER},

	{PM_TOMB_SCORPION, PM_GOLDENTAIL_SCORPION},

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
	{PM_YOUNG_YELLOW_DRAGON, PM_YOUNG_ADULT_YELLOW_DRAGON}, {PM_YOUNG_ADULT_YELLOW_DRAGON, PM_ADULT_YELLOW_DRAGON},
	{PM_ADULT_YELLOW_DRAGON, PM_OLD_YELLOW_DRAGON}, {PM_OLD_YELLOW_DRAGON, PM_VERY_OLD_YELLOW_DRAGON},
	{PM_VERY_OLD_YELLOW_DRAGON, PM_ANCIENT_YELLOW_DRAGON},

	{PM_RED_KITE, PM_DRAGONKIN_BROODMASTER},
	{PM_ELECTRIC_GEL, PM_CARNIVOROUS_GEL},

	{PM_DEATHWALKER_KNIGHT, PM_DEATHWALKER_BATTLEMASTER},

	{PM_WILD_WOLF, PM_HUNTING_WOLF},

	{PM_BORLOTH, PM_MOONBEAST},	{PM_MOONBEAST, PM_SPACE_BORLOTH},

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

	{PM_BABY_GRAY_DRAGON, PM_GRAY_DRAGON},
	{PM_BABY_RED_DRAGON, PM_RED_DRAGON},
	{PM_BABY_SILVER_DRAGON, PM_SILVER_DRAGON},
	{PM_BABY_DEEP_DRAGON, PM_DEEP_DRAGON},
	{PM_BABY_SHIMMERING_DRAGON, PM_SHIMMERING_DRAGON},
	{PM_BABY_WHITE_DRAGON, PM_WHITE_DRAGON},
	{PM_BABY_ORANGE_DRAGON, PM_ORANGE_DRAGON},
	{PM_BABY_BLACK_DRAGON, PM_BLACK_DRAGON},
	{PM_BABY_BLUE_DRAGON, PM_BLUE_DRAGON},
	{PM_BABY_GREEN_DRAGON, PM_GREEN_DRAGON},
	{PM_BABY_GOLDEN_DRAGON, PM_GOLDEN_DRAGON},
	{PM_BABY_YELLOW_DRAGON, PM_YELLOW_DRAGON},
	{PM_BABY_STONE_DRAGON, PM_STONE_DRAGON},
	{PM_BABY_CYAN_DRAGON, PM_CYAN_DRAGON},
	{PM_RED_NAGA_HATCHLING, PM_RED_NAGA},
	{PM_STRIPPER_GIRL, PM_STRIPPER_LADY},
	{PM_GRUESOME_NAGA_HATCHLING, PM_GRUESOME_NAGA},
	{PM_UNDEAD_NAGA_HATCHLING, PM_UNDEAD_NAGA},
	{PM_SIZZLE_NAGA_HATCHLING, PM_SIZZLE_NAGA},
	{PM_MULTI_HUED_NAGA_HATCHLING, PM_MULTI_HUED_NAGA},
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

	{PM_CATERPIE, PM_METAPOD},{PM_METAPOD, PM_BUTTERFREE},
	{PM_WEEDLE, PM_KAKUNA},{PM_KAKUNA, PM_BEEDRILL},

	{PM_PIDGEY, PM_PIDGEOTTO},{PM_PIDGEOTTO, PM_PIDGEOT},
	{PM_ABRA, PM_KADABRA},{PM_KADABRA, PM_ALAKAZAM},

	{PM_POLIWAG, PM_POLIWHIRL},{PM_POLIWHIRL, PM_POLIWRATH},

	{PM_SQUIRTLE, PM_WARTORTLE},{PM_WARTORTLE, PM_BLASTOISE},
	{PM_BULBASAUR, PM_IVYSAUR},{PM_IVYSAUR, PM_VENUSAUR},
	{PM_ODDISH, PM_GLOOM},{PM_GLOOM, PM_VILEPLUME},
	{PM_CHARMANDER, PM_CHARMELEON},{PM_CHARMELEON, PM_CHARIZARD},
	{PM_ZUBAT, PM_GOLBAT},	{PM_GOLBAT, PM_CROBAT},
	{PM_KRABBY, PM_KINGLER}, 	{PM_TENTACOOL, PM_TENTACRUEL},
	{PM_GREEN_NAGA_HATCHLING, PM_GREEN_NAGA},
	{PM_NEXUS_NAGA_HATCHLING, PM_NEXUS_NAGA},
	{PM_WHITE_NAGA_HATCHLING, PM_WHITE_NAGA},
	{PM_DOMESTIC_NAGA_HATCHLING, PM_DOMESTIC_NAGA},
	{PM_BLUE_NAGA_HATCHLING, PM_BLUE_NAGA},
	{PM_BLACK_NAGA_HATCHLING, PM_BLACK_NAGA},
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

	{PM_SOLDIER, PM_SERGEANT},
	{PM_SERGEANT, PM_LIEUTENANT},
	{PM_KICKING_NINJA, PM_FOURTH_WALL_BREAKER},
	{PM_LIEUTENANT, PM_CAPTAIN},
	{PM_CAPTAIN, PM_GENERAL},
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

	{PM_GASTLY, PM_HAUNTER}, 	{PM_HAUNTER, PM_GENGAR},

	{PM_DRAGONBALL_KID, PM_KAMEHAMEHA_FIGHTER},{PM_KAMEHAMEHA_FIGHTER, PM_SUPER_SAIYAN},

	{PM_ELONA_GIRL, PM_ELONA_LADY},

	{PM_KICKBOXING_GIRL, PM_KICKBOXING_LADY},{PM_ESTRELLA_GIRL, PM_ESTRELLARINA},
	{PM_LITTLE_GIRL, PM_YOUNG_ADULT_GIRL}, {PM_LITTLE_BOY, PM_YOUNG_ADULT_BOY},
	{PM_ASIAN_GIRL, PM_ASIAN_WOMAN}, {PM_ASIAN_WOMAN, PM_ASIAN_LADY},
	{PM_DARK_GIRL, PM_DARK_WOMAN}, {PM_DARK_WOMAN, PM_DARK_LADY},
	{PM_REDGUARD_GIRL, PM_REDGUARD_WOMAN}, {PM_REDGUARD_WOMAN, PM_REDGUARD_LADY},
	{PM_THIEVING_GIRL, PM_THIEVING_WOMAN}, {PM_THIEVING_WOMAN, PM_THIEVING_LADY},
	{PM_YOUNG_ADULT_GIRL, PM_YOUNG_WOMAN}, {PM_YOUNG_ADULT_BOY, PM_YOUNG_MAN},
	{PM_YOUNG_WOMAN, PM_ADULT_LADY}, {PM_YOUNG_MAN, PM_ADULT_GENTLEMAN},
	{PM_ADULT_LADY, PM_OLD_GRANNY}, {PM_ADULT_GENTLEMAN, PM_OLD_GRANDPA},
	{PM_NINJA_GIRL, PM_TAEKWONDO_GIRL}, {PM_NINJA_BOY, PM_BLACKBELT},
	{PM_DANCING_GIRL, PM_DANCING_QUEEN}, {PM_DANCING_GUY, PM_DANCING_KING},

	/*{PM_EEVEE, THEEEVEELUTION},*/

#ifdef KOPS
	{PM_KEYSTONE_KOP, PM_KOP_SERGEANT},
	{PM_KOP_SERGEANT, PM_KOP_LIEUTENANT},
	{PM_KOP_LIEUTENANT, PM_KOP_KAPTAIN},
	{PM_KOP_KAPTAIN, PM_KOP_KOMMISSIONER},
	{PM_KOP_KOMMISSIONER, PM_KOP_KCHIEF},
	{PM_KOP_KCHIEF, PM_KOP_KATCHER},

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
#endif
	/* WAC -- added killer coin piles */

	{PM_PILE_OF_COPPER_COINS, PM_PILE_OF_SILVER_COINS},
	{PM_PILE_OF_SILVER_COINS, PM_PILE_OF_PLATINUM_COINS},

	{PM_PILE_OF_KILLER_COINS, PM_LARGE_PILE_OF_KILLER_COINS},
	{PM_LARGE_PILE_OF_KILLER_COINS,PM_HUGE_PILE_OF_KILLER_COINS},
	/* KMH -- added more sequences */
	{PM_DINGO_PUPPY, PM_DINGO}, {PM_DINGO, PM_LARGE_DINGO},
	{PM_PONY, PM_HORSE}, {PM_HORSE, PM_WARHORSE},
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
	{PM_BRAIN_EATER, PM_MIND_SUCKER}, {PM_MIND_SUCKER, PM_MIND_FLAYER},
	{PM_MASTER_MIND_FLAYER, PM_GRANDMASTER_MIND_FLAYER}, {PM_LEPRECHAUN, PM_MEAN_ROBBER},
	{PM_GIANT_TRAPDOOR_SPIDER, PM_BLACK_WIDOW}, {PM_GIANT_SPIDER, PM_GIANT_TRAPDOOR_SPIDER}, 
	{PM_AGGRESSIVE_LICHEN, PM_POISON_LICHEN}, 
	{PM_LICHEN, PM_POISON_LICHEN}, {PM_POISON_LICHEN, PM_ORE_LICHEN}, 
	{PM_ORE_LICHEN, PM_MASTER_LICHEN}, {PM_MASTER_LICHEN, PM_GRANDMASTER_LICHEN}, 
	{PM_GNOME_THIEF, PM_GNOMISH_BRIGAND_THIEF}, {PM_GNOMISH_BRIGAND_THIEF, PM_GNOMISH_MASTER_THIEF},
	{PM_GNOLL_THIEF, PM_GNOLL_ROGUE}, {PM_GNOLL_ROGUE, PM_GNOLL_GANGSTER},
	{PM_OGRE_THIEF, PM_OGRE_BRIGAND_THIEF}, {PM_OGRE_BRIGAND_THIEF, PM_OGRE_MASTER_THIEF},
	{PM_BAD_EGG, PM_ANIMATED_ROTTEN_EGG},
	{PM_STATUE_GARGOYLE, PM_BLOOD_CLAW},
	{PM_BLOOD_CLAW, PM_DEATH_WING},
	{PM_XORN, PM_GREATER_XORN},

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
