/*	SCCS Id: @(#)artifact.c 3.3	2000/01/11	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "artifact.h"
#ifdef OVLB
#include "artilist.h"
#else
STATIC_DCL struct artifact artilist[];
#endif
/*
 * Note:  both artilist[] and artiexist[] have a dummy element #0,
 *        so loops over them should normally start at #1.  The primary
 *        exception is the save & restore code, which doesn't care about
 *        the contents, just the total size.
 */

extern boolean notonhead;       /* for long worms */

#define get_artifact(o) \
		(((o)&&(o)->oartifact) ? &artilist[(int) (o)->oartifact] : 0)
STATIC_DCL int FDECL(spec_applies, (const struct artifact *,struct monst *));
STATIC_DCL int FDECL(arti_invoke, (struct obj*));

/* The amount added to normal damage which should guarantee that the
   victim will be killed even after damage bonus/penalty adjustments.
   It needs to be big enough so that halving will still kill, but not
   so big that doubling ends up overflowing 15 bits.  This value used
   to be 1234, but it was possible for players to accumulate enough
   hit points so that taking (uhp + 1234)/2 damage was survivable. */
#define FATAL_DAMAGE 9999

#ifndef OVLB
STATIC_DCL int spec_dbon_applies;
STATIC_DCL xchar artidisco[NROFARTIFACTS];
#else   /* OVLB */
/* coordinate effects from spec_dbon() with messages in artifact_hit() */
STATIC_OVL int spec_dbon_applies = 0;

/* flags including which artifacts have already been created */
static boolean artiexist[1+NROFARTIFACTS+1];
/* and a discovery list for them (no dummy first entry here) */

STATIC_OVL xchar artidisco[NROFARTIFACTS];

STATIC_DCL void NDECL(hack_artifacts);
STATIC_DCL boolean FDECL(attacks, (int,struct obj *));


/* handle some special cases; must be called after u_init() */
STATIC_OVL void
hack_artifacts()
{
	struct artifact *art;
	int alignmnt = aligns[flags.initalign].value;

	/* Fix up the alignments of "gift" artifacts */
	for (art = artilist+1; art->otyp; art++)
	    if (art->role == Role_switch && art->alignment != A_NONE)
		art->alignment = alignmnt;

	/* Excalibur can be used by any lawful character, not just knights */
	if (!Role_if(PM_KNIGHT))
	    artilist[ART_EXCALIBUR].role = 0;

#if 0
	/* Fix up the gifts */
	if (urole.gift1arti) {
		artilist[urole.gift1arti].alignment = alignmnt;
		artilist[urole.gift1arti].role = Role_switch;
	}
	if (urole.gift2arti) {
		artilist[urole.gift2arti].alignment = alignmnt;
		artilist[urole.gift2arti].role = Role_switch;
	}
#endif
	/* Fix up the quest artifact */
	if (urole.questarti) {
	    artilist[urole.questarti].alignment = alignmnt;
	    artilist[urole.questarti].role = Role_switch;
	}
	return;
}


/* zero out the artifact existence list */
void
init_artifacts()
{
	(void) memset((genericptr_t) artiexist, 0, sizeof artiexist);
	(void) memset((genericptr_t) artidisco, 0, sizeof artidisco);
	hack_artifacts();

#if 0
	/* KMH -- Should be at least skilled in first artifact gifts */
	if (urole.gift1arti &&
		(objects[artilist[urole.gift1arti].otyp].oc_class == WEAPON_CLASS ||
		 objects[artilist[urole.gift1arti].otyp].oc_class == TOOL_CLASS)) {
		int skill = objects[artilist[urole.gift1arti].otyp].oc_skill;

		if (skill > P_NONE && P_SKILL(skill) < P_UNSKILLED)
			P_SKILL(skill) = P_UNSKILLED;
		if (skill > P_NONE && P_MAX_SKILL(skill) < P_SKILLED) {
			pline("Warning: %s should be at least skilled.  Fixing...",
					artilist[urole.gift1arti].name);
			P_MAX_SKILL(skill) = P_SKILLED;
		}
	}
	if (urole.gift2arti &&
		(objects[artilist[urole.gift2arti].otyp].oc_class == WEAPON_CLASS ||
		 objects[artilist[urole.gift2arti].otyp].oc_class == TOOL_CLASS)) {
		int skill = objects[artilist[urole.gift2arti].otyp].oc_skill;

	    if (skill > P_NONE && P_SKILL(skill) < P_UNSKILLED)
		P_SKILL(skill) = P_UNSKILLED;
	    if (skill > P_NONE && P_MAX_SKILL(skill) < P_SKILLED) {
		pline("Warning: %s should be at least skilled.  Fixing...",
				artilist[urole.gift1arti].name);
	    	P_MAX_SKILL(skill) = P_SKILLED;
	    }
	}
#endif 
	/* KMH -- Should be expert in quest artifact */
	if (urole.questarti &&
		(objects[artilist[urole.questarti].otyp].oc_class == WEAPON_CLASS ||
		 objects[artilist[urole.questarti].otyp].oc_class == TOOL_CLASS)) {
		int skill = objects[artilist[urole.questarti].otyp].oc_skill;

	    if (skill > P_NONE && P_SKILL(skill) < P_UNSKILLED)
		P_SKILL(skill) = P_UNSKILLED;
	    if (skill > P_NONE && P_MAX_SKILL(skill) < P_SKILLED) {
		pline("Warning: %s should be at least expert.  Fixing...",
				artilist[urole.questarti].name);
		P_MAX_SKILL(skill) = P_EXPERT;
	    }
}
}

void
save_artifacts(fd)
int fd;
{
	bwrite(fd, (genericptr_t) artiexist, sizeof artiexist);
	bwrite(fd, (genericptr_t) artidisco, sizeof artidisco);
}

void
restore_artifacts(fd)
int fd;
{
	mread(fd, (genericptr_t) artiexist, sizeof artiexist);
	mread(fd, (genericptr_t) artidisco, sizeof artidisco);
	hack_artifacts();       /* redo non-saved special cases */
}

const char *
artiname(artinum)
int artinum;
{
	if (artinum <= 0 || artinum > NROFARTIFACTS) return("");
	return(artilist[artinum].name);
}

/*
   Make an artifact.  If a specific alignment is specified, then an object of
   the appropriate alignment is created from scratch, or 0 is returned if
   none is available.  (If at least one aligned artifact has already been
   given, then unaligned ones also become eligible for this.)
   If no alignment is given, then 'otmp' is converted
   into an artifact of matching type, or returned as-is if that's not possible.
   For the 2nd case, caller should use ``obj = mk_artifact(obj, A_NONE);''
   for the 1st, ``obj = mk_artifact((struct obj *)0, some_alignment);''.
 */
struct obj *
mk_artifact(otmp, alignment)
struct obj *otmp;       /* existing object; ignored if alignment specified */
aligntyp alignment;     /* target alignment, or A_NONE */
{
	const struct artifact *a;
	int n, m;
	boolean by_align = (alignment != A_NONE);
	short o_typ = (by_align || !otmp) ? 0 : otmp->otyp;
	boolean unique = !by_align && otmp && objects[o_typ].oc_unique;
	short eligible[NROFARTIFACTS];


#if 0
	/* KMH, role patch -- Try first or second gift */
	m = urole.gift1arti;
	if (by_align && m && !artiexist[m]) {
		a = (struct artifact *)&artilist[m];
		goto make_artif;
	}
	m = urole.gift2arti;
	if (by_align && m && !artiexist[m]) {
		a = (struct artifact *)&artilist[m];
		goto make_artif;
	}
#endif

	/* gather eligible artifacts */
	for (n = 0, a = artilist+1, m = 1; a->otyp; a++, m++)
	    if ((!by_align ? a->otyp == o_typ :
		    (a->alignment == alignment ||
			(a->alignment == A_NONE && u.ugifts > 0))) &&
		(!(a->spfx & SPFX_NOGEN) || unique) && !artiexist[m]) {
		if (by_align && a->race != NON_PM && race_hostile(&mons[a->race]))
		    continue;	/* skip enemies' equipment */
		else if (by_align && Role_if(a->role))
		    goto make_artif;	/* 'a' points to the desired one */
		else
		    eligible[n++] = m;
	    }

	if (n) {                /* found at least one candidate */
	    m = eligible[rn2(n)];	/* [0..n-1] */
	    a = &artilist[m];

	    /* make an appropriate object if necessary, then christen it */
make_artif: if (by_align) otmp = mksobj((int)a->otyp, TRUE, FALSE);
	    otmp = oname(otmp, a->name);
	    otmp->oartifact = m;
	    artiexist[m] = TRUE;
	} else {
	    /* nothing appropriate could be found; return the original object */
	    if (by_align) otmp = 0;     /* (there was no original object) */
	}
	return otmp;
}

/*
 * Returns the full name (with articles and correct capitalization) of an
 * artifact named "name" if one exists, or NULL, it not.
 * The given name must be rather close to the real name for it to match.
 * The object type of the artifact is returned in otyp if the return value
 * is non-NULL.
 */
const char*
artifact_name(name, otyp)
const char *name;
short *otyp;
{
    register const struct artifact *a;
    register const char *aname;

    if(!strncmpi(name, "the ", 4)) name += 4;

    for (a = artilist+1; a->otyp; a++) {
	aname = a->name;
	if(!strncmpi(aname, "the ", 4)) aname += 4;
	if(!strcmpi(name, aname)) {
	    *otyp = a->otyp;
	    return a->name;
	}
    }

    return (char *)0;
}

boolean
exist_artifact(otyp, name)
register int otyp;
register const char *name;
{
	register const struct artifact *a;
	register boolean *arex;

	if (otyp && *name)
	    for (a = artilist+1,arex = artiexist+1; a->otyp; a++,arex++)
		if ((int) a->otyp == otyp && !strcmp(a->name, name))
		    return *arex;
	return FALSE;
}

void
artifact_exists(otmp, name, mod)
register struct obj *otmp;
register const char *name;
register boolean mod;
{
	register const struct artifact *a;

	if (otmp && *name)
	    for (a = artilist+1; a->otyp; a++)
		if (a->otyp == otmp->otyp && !strcmp(a->name, name)) {
		    register int m = a - artilist;
/*WAC add code to automatically light up Sunsword/holy spear of light on
        creation*/
		    /* For the check so artifact_light recognizes it */
		    otmp->oartifact = m;
		    if (!artiexist[m] && mod && (artifact_light(otmp) ||
						 otmp->otyp == MAGIC_CANDLE)) {
			begin_burn(otmp, FALSE);
			/* WAC light source deletion handled in obfree */
		    }
		    otmp->oartifact = (char)(mod ? m : 0);
		    if (mod) {
			otmp->quan = 1; /* guarantee only one of this artifact */
#ifdef UNPOLYPILE	/* Artifacts are immune to unpolypile --ALI */
			if (is_fuzzy(otmp)) {
			    (void) stop_timer(UNPOLY_OBJ, (genericptr_t) otmp);
			    otmp->oldtyp = STRANGE_OBJECT;
			}
#endif
		    }
		    otmp->age = 0;
		    if(otmp->otyp == RIN_INCREASE_DAMAGE)
			otmp->spe = 0;
		    artiexist[m] = mod;
		    break;
		}
	return;
}

int
nartifact_exist()
{
    int a = 0;
    int n = SIZE(artiexist);

    while(n > 1)
		if(artiexist[--n]) a++;

    return a;
}
#endif /* OVLB */
#ifdef OVL0

boolean
spec_ability(otmp, abil)
struct obj *otmp;
unsigned long abil;
{
	const struct artifact *arti = get_artifact(otmp);

	return((boolean)(arti && (arti->spfx & abil)));
}

#endif /* OVL0 */
#ifdef OVLB

boolean
restrict_name(otmp, name)  /* returns 1 if name is restricted for otmp->otyp */
register struct obj *otmp;
register const char *name;
{
	register const struct artifact *a;
	register const char *aname;

	if (!*name) return FALSE;
	if (!strncmpi(name, "the ", 4)) name += 4;

		/* Since almost every artifact is SPFX_RESTR, it doesn't cost
		   us much to do the string comparison before the spfx check.
		   Bug fix:  don't name multiple elven daggers "Sting".
		 */
	for (a = artilist+1; a->otyp; a++) {
	    if (a->otyp != otmp->otyp) continue;
	    aname = a->name;
	    if (!strncmpi(aname, "the ", 4)) aname += 4;
	    if (!strcmp(aname, name))
		return ((boolean)((a->spfx & (SPFX_NOGEN|SPFX_RESTR)) != 0 ||
			otmp->quan > 1L));
	}

	return FALSE;
}

STATIC_OVL boolean
attacks(adtyp, otmp)
register int adtyp;
register struct obj *otmp;
{
	register const struct artifact *weap;

	if ((weap = get_artifact(otmp)) != 0)
		return((boolean)(weap->attk.adtyp == adtyp));
	return FALSE;
}

boolean
defends(adtyp, otmp)
register int adtyp;
register struct obj *otmp;
{
	register const struct artifact *weap;

	if ((weap = get_artifact(otmp)) != 0)
		return((boolean)(weap->defn.adtyp == adtyp));
	return FALSE;
}

/* used for monsters */
boolean
protects(adtyp, otmp)
int adtyp;
struct obj *otmp;
{
	register const struct artifact *weap;

	if ((weap = get_artifact(otmp)) != 0)
		return (boolean)(weap->cary.adtyp == adtyp);
	return FALSE;
}

/*
 * a potential artifact has just been worn/wielded/picked-up or
 * unworn/unwielded/dropped.  Pickup/drop only set/reset the W_ART mask.
 */
void
set_artifact_intrinsic(otmp,on,wp_mask)
register struct obj *otmp;
boolean on;
long wp_mask;
{
	long *mask = 0;
	register const struct artifact *oart = get_artifact(otmp);
	uchar dtyp;
	long spfx;

	if (!oart) return;

	/* effects from the defn field */
	dtyp = (wp_mask != W_ART) ? oart->defn.adtyp : oart->cary.adtyp;

	if (dtyp == AD_FIRE)
	    mask = &EFire_resistance;
	else if (dtyp == AD_COLD)
	    mask = &ECold_resistance;
	else if (dtyp == AD_ELEC)
	    mask = &EShock_resistance;
	else if (dtyp == AD_MAGM)
	    mask = &EAntimagic;
	else if (dtyp == AD_DISN)
	    mask = &EDisint_resistance;
	else if (dtyp == AD_DRST)
	    mask = &EPoison_resistance;

	if (mask && wp_mask == W_ART && !on) {
	    /* find out if some other artifact also confers this intrinsic */
	    /* if so, leave the mask alone */
	    register struct obj* obj;
	    for(obj = invent; obj; obj = obj->nobj)
		if(obj != otmp && obj->oartifact) {
		    register const struct artifact *art = get_artifact(obj);
		    if(art->cary.adtyp == dtyp) {
			mask = (long *) 0;
			break;
		    }
		}
	}
	if(mask) {
	    if (on) *mask |= wp_mask;
	    else *mask &= ~wp_mask;
	}

	/* intrinsics from the spfx field; there could be more than one */
	spfx = (wp_mask != W_ART) ? oart->spfx : oart->cspfx;
	if(spfx && wp_mask == W_ART && !on) {
	    /* don't change any spfx also conferred by other artifacts */
	    register struct obj* obj;
	    for(obj = invent; obj; obj = obj->nobj)
		if(obj != otmp && obj->oartifact) {
		    register const struct artifact *art = get_artifact(obj);
		    spfx &= ~art->cspfx;
		}
	}

	if (spfx & SPFX_SEARCH) {
	    if(on) ESearching |= wp_mask;
	    else ESearching &= ~wp_mask;
	}
	if (spfx & SPFX_HALRES) {
	    /* make_hallucinated must (re)set the mask itself to get
	     * the display right */
	    /* restoring needed because this is the only artifact intrinsic
	     * that can print a message--need to guard against being printed
	     * when restoring a game
	     */
	    make_hallucinated((long)!on, restoring ? FALSE : TRUE, wp_mask);
	}
	if (spfx & SPFX_ESP) {
	    if(on) ETelepat |= wp_mask;
	    else ETelepat &= ~wp_mask;
	    see_monsters();
	}
	if (spfx & SPFX_STLTH) {
	    if (on) EStealth |= wp_mask;
	    else EStealth &= ~wp_mask;
	}
	if (spfx & SPFX_REGEN) {
	    if (on) ERegeneration |= wp_mask;
	    else ERegeneration &= ~wp_mask;
	}
	if (spfx & SPFX_TCTRL) {
	    if (on) ETeleport_control |= wp_mask;
	    else ETeleport_control &= ~wp_mask;
	}
	/* weapon warning is specially handled in mon.c */
	if (spfx & SPFX_WARN) {
	    if (spec_m2(otmp)) {
	    	if (on) {
			EWarn_of_mon |= wp_mask;
			flags.warntype |= spec_m2(otmp);
	    	} else {
			EWarn_of_mon &= ~wp_mask;
	    		flags.warntype &= ~spec_m2(otmp);
		}
		see_monsters();
	    } else {
		if (on) EWarning |= wp_mask;
	    	else EWarning &= ~wp_mask;
	    }
	}
	if (spfx & SPFX_EREGEN) {
	    if (on) EEnergy_regeneration |= wp_mask;
	    else EEnergy_regeneration &= ~wp_mask;
	}
	if (spfx & SPFX_HSPDAM) {
	    if (on) EHalf_spell_damage |= wp_mask;
	    else EHalf_spell_damage &= ~wp_mask;
	}
	if (spfx & SPFX_HPHDAM) {
	    if (on) EHalf_physical_damage |= wp_mask;
	    else EHalf_physical_damage &= ~wp_mask;
	}
	if (spfx & SPFX_XRAY) {
	    /* this assumes that no one else is using xray_range */
	    if (on) u.xray_range = 3;
	    else u.xray_range = -1;
	}
	/* KMH -- Reflection when wielded */
	if ((spfx & SPFX_REFLECT) && (wp_mask & W_WEP)) {
	    if (on) EReflecting |= wp_mask;
	    else EReflecting &= ~wp_mask;
	}

	if(wp_mask == W_ART && !on && oart->inv_prop) {
	    /* might have to turn off invoked power too */
	    if (oart->inv_prop <= LAST_PROP &&
		(u.uprops[oart->inv_prop].extrinsic & W_ARTI))
		(void) arti_invoke(otmp);
	}
}

/*
 * creature (usually player) tries to touch (pick up or wield) an artifact obj.
 * Returns 0 if the object refuses to be touched.
 * This routine does not change any object chains.
 * Ignores such things as gauntlets, assuming the artifact is not
 * fooled by such trappings.
 */
int
touch_artifact(obj,mon)
    struct obj *obj;
    struct monst *mon;
{
    register const struct artifact *oart = get_artifact(obj);
    boolean badclass, badalign, self_willed, yours;

    if(!oart) return 1;

    /* [ALI] Thiefbane has a special affinity with shopkeepers */
    if (mon->isshk && obj->oartifact == ART_THIEFBANE) return 1;

    yours = (mon == &youmonst);
    /* all quest artifacts are self-willed; it this ever changes, `badclass'
       will have to be extended to explicitly include quest artifacts */
    self_willed = ((oart->spfx & SPFX_INTEL) != 0);
      if (yours || !(mon->data->mflags3 & M3_WANTSALL)) {
	badclass = (self_willed && (!yours ||
			(oart->role != NON_PM && !Role_if(oart->role)) ||
			(oart->race != NON_PM && !Race_if(oart->race))));
	badalign = (oart->spfx & SPFX_RESTR) &&
		   oart->alignment != A_NONE &&
	    ((oart->alignment !=
	      (yours ? u.ualign.type : sgn(mon->data->maligntyp))) ||
	     (yours && u.ualign.record < 0));
    } else {    /* an M3_WANTSxxx monster */
	/* special monsters trying to take the Amulet, invocation tools or
	   quest item can touch anything except for `spec_applies' artifacts */
	badclass = badalign = FALSE;
    }
    /* weapons which attack specific categories of monsters are
       bad for them even if their alignments happen to match */
    if (!badalign && (oart->spfx & SPFX_DBONUS) != 0) {
	struct artifact tmp;

	tmp = *oart;
	tmp.spfx &= SPFX_DBONUS;
	badalign = !!spec_applies(&tmp, mon);
    }

    if (((badclass || badalign) && self_willed) ||
       (badalign && (!yours || !rn2(4))))  {
	int dmg;
	char buf[BUFSZ];

	if (!yours) return 0;
	You("are blasted by %s power!", s_suffix(the(xname(obj))));
	dmg = d((Antimagic ? 6 : 8), (self_willed ? 10 : 6));
	Sprintf(buf, "touching %s", oart->name);
	losehp(dmg, buf, KILLED_BY);
	exercise(A_WIS, FALSE);
    }

    /* can pick it up unless you're totally non-synch'd with the artifact */
    if (badclass && badalign && self_willed) {
	if (yours) pline("%s evades your grasp!", The(xname(obj)));
        return 0;
    }

    return 1;
}

#endif /* OVLB */
#ifdef OVL1

/* decide whether an artifact's special attacks apply against mtmp */
STATIC_OVL int
spec_applies(weap, mtmp)
register const struct artifact *weap;
struct monst *mtmp;
{
	int retval = TRUE;
	struct permonst *ptr;
	boolean yours;

	if(!(weap->spfx & (SPFX_DBONUS | SPFX_ATTK)))
	    return(weap->attk.adtyp == AD_PHYS);

	yours = (mtmp == &youmonst);
	ptr = mtmp->data;

	/* [ALI] Modified to support multiple DBONUS and ATTK flags set.
	 * Not all combinations are possible because many DBONUS flags
	 * use mtype and would conflict. Where combinations are possible,
	 * both checks must pass in order for the special attack to
	 * apply against mtmp.
	 */
	if (weap->spfx & SPFX_DMONS) {
	    retval &= (ptr == &mons[(int)weap->mtype]);
	} else if (weap->spfx & SPFX_DCLAS) {
	    retval &= (weap->mtype == (unsigned long)ptr->mlet);
	} else if (weap->spfx & SPFX_DFLAG1) {
	    retval &= ((ptr->mflags1 & weap->mtype) != 0L);
	} else if (weap->spfx & SPFX_DFLAG2) {
	    retval &= ((ptr->mflags2 & weap->mtype) ||
		(yours && !Upolyd && (urace.selfmask & weap->mtype)));
	}
	if (weap->spfx & SPFX_DALIGN) {
	    retval &= yours ? (u.ualign.type != weap->alignment) :
			      (ptr->maligntyp == A_NONE ||
				sgn(ptr->maligntyp) != weap->alignment);
	}
	if (weap->spfx & SPFX_ATTK) {
	    struct obj *defending_weapon = (yours ? uwep : MON_WEP(mtmp));

	    if (defending_weapon && defending_weapon->oartifact &&
		    defends((int)weap->attk.adtyp, defending_weapon))
		return FALSE;
	    switch(weap->attk.adtyp) {
		case AD_FIRE:
			if (yours ? Fire_resistance : resists_fire(mtmp))
			    retval = FALSE;
			break;
		case AD_COLD:
			if (yours ? Cold_resistance : resists_cold(mtmp))
			    retval = FALSE;
			break;
		case AD_ELEC:
			if (yours ? Shock_resistance : resists_elec(mtmp))
			    retval = FALSE;
			break;
		case AD_MAGM:
		case AD_STUN:
			if (yours ? Antimagic : (rn2(100) < ptr->mr))
			    retval = FALSE;
			break;
		case AD_DRST:
			if (yours ? Poison_resistance : resists_poison(mtmp))
			    retval = FALSE;
			break;
		case AD_DRLI:
			if (yours ? Drain_resistance : resists_drli(mtmp))
			    retval = FALSE;
			break;
		case AD_STON:
			if (yours ? Stone_resistance : resists_ston(mtmp))
			    retval = FALSE;
			break;
		default:        impossible("Weird weapon special attack.");
	    }
	}
	return retval;
}

/* return the M2 flags of monster that an artifact's special attacks apply against */
long
spec_m2(otmp)
struct obj *otmp;
{
	register const struct artifact *artifact = get_artifact(otmp);
	if (artifact)
		return artifact->mtype;
	return 0L;
}

/* special attack bonus */
int
spec_abon(otmp, mon)
struct obj *otmp;
struct monst *mon;
{
	register const struct artifact *weap = get_artifact(otmp);

	if (weap && spec_applies(weap, mon))
	    return weap->attk.damn ? (int)weap->attk.damn : 0;
	return 0;
}

/* special damage bonus */
int
spec_dbon(otmp, mon, tmp)
struct obj *otmp;
struct monst *mon;
int tmp;
{
	register const struct artifact *weap = get_artifact(otmp);

	if ((spec_dbon_applies = (weap && spec_applies(weap, mon))) != 0)
	    return weap->attk.damd ? (int)weap->attk.damd :
		    /* [ALI] Unlike melee weapons, damd == 0 means no
		     * bonus for launchers.
		     */
		    is_launcher(otmp) ? 0 : max(tmp,1);
	return 0;
}

/* add identified artifact to discoveries list */
void
discover_artifact(m)
xchar m;
{
    int i;

    /* look for this artifact in the discoveries list;
       if we hit an empty slot then it's not present, so add it */
    for (i = 0; i < NROFARTIFACTS; i++)
	if (artidisco[i] == 0 || artidisco[i] == m) {
	    artidisco[i] = m;
	    return;
	}
    /* there is one slot per artifact, so we should never reach the
       end without either finding the artifact or an empty slot... */
    impossible("couldn't discover artifact (%d)", (int)m);
}

/* used to decide whether an artifact has been fully identified */
boolean
undiscovered_artifact(m)
xchar m;
{
    int i;

    /* look for this artifact in the discoveries list;
       if we hit an empty slot then it's undiscovered */
    for (i = 0; i < NROFARTIFACTS; i++)
	if (artidisco[i] == m)
	    return FALSE;
	else if (artidisco[i] == 0)
	    break;
    return TRUE;
}

/* display a list of discovered artifacts; return their count */
int
disp_artifact_discoveries(tmpwin)
winid tmpwin;		/* supplied by dodiscover() */
{
    int i, m, otyp;
    char buf[BUFSZ];
    anything any;

    any.a_void = 0;
    for (i = 0; i < NROFARTIFACTS; i++) {
	if (artidisco[i] == 0) break;	/* empty slot implies end of list */
	if (i == 0)
	    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_INVERSE,
	      "Artifacts", MENU_UNSELECTED);
	m = artidisco[i];
	otyp = artilist[m].otyp;
	Sprintf(buf, "  %s [%s %s]", artiname(m),
		align_str(artilist[m].alignment), simple_typename(otyp));
	add_menu(tmpwin, objnum_to_glyph(otyp), &any, 0, 0, ATR_NONE,
	  buf, MENU_UNSELECTED);
    }
    return i;
}

#endif /* OVL1 */

#ifdef OVLB

/* Function used when someone attacks someone else with an artifact
 * weapon.  Only adds the special (artifact) damage, and returns a 1 if it
 * did something special (in which case the caller won't print the normal
 * hit message).  This should be called once upon every artifact attack;
 * dmgval() no longer takes artifact bonuses into account.  Possible
 * extension: change the killer so that when an orc kills you with
 * Stormbringer it's "killed by Stormbringer" instead of "killed by an orc".
 */
boolean
artifact_hit(magr, mdef, otmp, dmgptr, dieroll)
struct monst *magr, *mdef;
struct obj *otmp;
int *dmgptr;
int dieroll; /* needed for Magicbane and vorpal blades */
{
	boolean youattack = (magr == &youmonst);
	boolean youdefend = (mdef == &youmonst);
	boolean vis = (!youattack && magr && cansee(magr->mx, magr->my))
		|| (!youdefend && cansee(mdef->mx, mdef->my));
	boolean realizes_damage;

	static const char you[] = "you";
	char hittee[BUFSIZ];

	strcpy(hittee, youdefend ? you : mon_nam(mdef));

	/* The following takes care of most of the damage, but not all--
	 * the exception being for level draining, which is specially
	 * handled.  Messages are done in this function, however.
	 */
	*dmgptr += spec_dbon(otmp, mdef, *dmgptr);

	if (youattack && youdefend) {
		impossible("attacking yourself with weapon?");
		return FALSE;
	} else if (!spec_dbon_applies) {
		/* since damage bonus didn't apply, nothing more to do */
		return FALSE;
	}

	realizes_damage = (youdefend || vis);

	/* the four basic attacks: fire, cold, shock and missiles */
	if (attacks(AD_FIRE, otmp)) {
		if (realizes_damage) {
#if 0	/* OBSOLETE */
		    if (otmp->oartifact == ART_PICK_OF_FLANDAL_STEELSKIN)
			pline_The("fiery pick burns %s!", hittee);
		    else
#endif
		pline_The("fiery blade %s %s!",
			(mdef->data == &mons[PM_WATER_ELEMENTAL]) ?
			"vaporizes part of" : "burns", hittee);
		if (!rn2(4)) (void) destroy_mitem(mdef, POTION_CLASS, AD_FIRE);
		if (!rn2(4)) (void) destroy_mitem(mdef, SCROLL_CLASS, AD_FIRE);
		if (!rn2(7)) (void) destroy_mitem(mdef, SPBOOK_CLASS, AD_FIRE);
			return TRUE;
		}
	}
	if (attacks(AD_COLD, otmp)) {
		if (realizes_damage) {
			pline_The("ice-cold blade freezes %s!", hittee);
		if (!rn2(4)) (void) destroy_mitem(mdef, POTION_CLASS, AD_COLD);
			return TRUE;
		}
	}
	if (attacks(AD_ELEC, otmp)) {
		if (realizes_damage) {
			if(youattack && otmp != uwep 
			    && (!u.twoweap || otmp != uswapwep))
			    pline("%s hits %s!", The(xname(otmp)), hittee);
			pline("Lightning strikes %s!", hittee);
		if (!rn2(5)) (void) destroy_mitem(mdef, RING_CLASS, AD_ELEC);
		if (!rn2(5)) (void) destroy_mitem(mdef, WAND_CLASS, AD_ELEC);
			return TRUE;
		}
	}
	if (attacks(AD_MAGM, otmp)) {
		if (realizes_damage) {
			if(youattack && otmp != uwep 
			   && (!u.twoweap || otmp != uswapwep))
			    	pline("%s hits %s!", The(xname(otmp)), hittee);
				pline("A hail of magic missiles strikes %s!", hittee);
				return TRUE;
		}
	}

	/*
	 * Magicbane's intrinsic magic is incompatible with normal
	 * enchantment magic.  Thus, its effects have a negative
	 * dependence on spe.  Against low mr victims, it typically
	 * does "double athame" damage, 2d4.  Occasionally, it will
	 * cast unbalancing magic which effectively averages out to
	 * 4d4 damage (2.5d4 against high mr victims), for spe = 0.
	 */

#define MB_MAX_DIEROLL          8    /* rolls above this aren't magical */
#define MB_INDEX_INIT           (-1)
#define MB_INDEX_PROBE          0
#define MB_INDEX_STUN           1
#define MB_INDEX_SCARE          2
#define MB_INDEX_PURGE          3
#define MB_RESIST_ATTACK        (resist_index = attack_index)
#define MB_RESISTED_ATTACK      (resist_index == attack_index)
#define MB_UWEP_ATTACK          (youattack && ((otmp == uwep) \
	 			    || (u.twoweap && otmp == uswapwep)))

	if (attacks(AD_STUN, otmp) && (dieroll <= MB_MAX_DIEROLL)) {
		int attack_index = MB_INDEX_INIT;
		int resist_index = MB_INDEX_INIT;
		int scare_dieroll = MB_MAX_DIEROLL / 2;

		if (otmp->spe >= 3)
			scare_dieroll /= (1 << (otmp->spe / 3));

		*dmgptr += rnd(4);                      /* 3d4 */

		if (otmp->spe > rn2(10))                /* probe */
			attack_index = MB_INDEX_PROBE;
		else {                                  /* stun */
			attack_index = MB_INDEX_STUN;
			*dmgptr += rnd(4);              /* 4d4 */

			if (youdefend)
				make_stunned((HStun + 3), FALSE);
			else
				mdef->mstun = 1;
		}
		if (dieroll <= scare_dieroll) {         /* scare */
			attack_index = MB_INDEX_SCARE;
			*dmgptr += rnd(4);              /* 5d4 */

			if (youdefend) {
				if (Antimagic)
					MB_RESIST_ATTACK;
				else {
					nomul(-3);
					nomovemsg = "";
					if ((magr == u.ustuck)
						&& sticks(youmonst.data)) {
					    u.ustuck = (struct monst *)0;
					    You("release %s!", mon_nam(magr));
					}
				}
			} else if (youattack) {
				if (rn2(2) && resist(mdef,SPBOOK_CLASS,0,0)) {
				    MB_RESIST_ATTACK;
				} else {
				    if (mdef == u.ustuck) {
					if (u.uswallow)
					    expels(mdef,mdef->data,TRUE);
					else {
					    if (!sticks(youmonst.data)) {
						u.ustuck = (struct monst *)0;
						You("get released!");
					    }
					}
				    }
				    mdef->mflee = 1;
				    mdef->mfleetim += 3;
				}
			}
		}
		if (dieroll <= (scare_dieroll / 2)) {   /* purge */
			struct obj *ospell;
			struct permonst *old_uasmon = youmonst.data;

			attack_index = MB_INDEX_PURGE;
			*dmgptr += rnd(4);              /* 6d4 */

			/* Create a fake spell object, ala spell.c */
			ospell = mksobj(SPE_CANCELLATION, FALSE, FALSE);
			ospell->blessed = ospell->cursed = 0;
			ospell->quan = 20L;

			cancel_monst(mdef, ospell, youattack, FALSE, FALSE);

			if (youdefend) {
				if (old_uasmon != youmonst.data)
					/* rehumanized, no more damage */
					*dmgptr = 0;
				if (Antimagic)
					MB_RESIST_ATTACK;
			} else {
				if (!mdef->mcan)
					MB_RESIST_ATTACK;

				/* cancelled clay golems will die ... */
				else if (mdef->data == &mons[PM_CLAY_GOLEM])
					mdef->mhp = 1;
			}

			obfree(ospell, (struct obj *)0);
		}

		if (youdefend || mdef->mhp > 0) {  /* ??? -dkh- */
			static const char *mb_verb[4] =
				{"probe", "stun", "scare", "purge"};

			if (youattack || youdefend || vis) {
				pline_The("magic-absorbing blade %ss %s!",
					mb_verb[attack_index], hittee);

				if (MB_RESISTED_ATTACK) {
					pline("%s resist%s!",
					youdefend ? "You" : Monnam(mdef),
					youdefend ? "" : "s");

					shieldeff(youdefend ? u.ux : mdef->mx,
						youdefend ? u.uy : mdef->my);
				}
			}

			/* Much ado about nothing.  More magic fanfare! */
			if (MB_UWEP_ATTACK) {
				if (attack_index == MB_INDEX_PURGE) {
				    if (!MB_RESISTED_ATTACK &&
					attacktype(mdef->data, AT_MAGC)) {
					You("absorb magical energy!");
					u.uenmax++;
					u.uen++;
					flags.botl = 1;
				    }
				} else if (attack_index == MB_INDEX_PROBE) {
				    if (!rn2(4 * otmp->spe)) {
					pline_The("probe is insightful!");
					if (!canspotmon(mdef))
					    map_invisible(u.ux+u.dx,u.uy+u.dy);
					/* pre-damage status */
					probe_monster(mdef);
				    }
				}
			} else if (youdefend && !MB_RESISTED_ATTACK
				   && (attack_index == MB_INDEX_PURGE)) {
				You("lose magical energy!");
				if (u.uenmax > 0) u.uenmax--;
				if (u.uen > 0) u.uen--;
					flags.botl = 1;
			}

			/* all this magic is confusing ... */
			if (!rn2(12)) {
			    if (youdefend)
				make_confused((HConfusion + 4), FALSE);
			    else
				mdef->mconf = 1;

			    if (youattack || youdefend || vis)
				pline("%s %s confused.",
				      youdefend ? "You" : Monnam(mdef),
				      youdefend ? "are" : "is");
			}
		}
		return TRUE;
	}
	/* end of Magicbane code */

	/* STEPHEN WHITE'S NEW CODE */
	if (otmp->oartifact == ART_SERPENT_S_TONGUE) {
	    otmp->dknown = TRUE;
	    pline_The("twisted blade poisons %s!",
		    youdefend ? "you" : mon_nam(mdef));
	    if (youdefend ? Poison_resistance : resists_poison(mdef)) {
		if (youdefend)
		    You("are not affected by the poison.");
		else
		    pline("%s seems unaffected by the poison.", Monnam(mdef));
		return TRUE;
	    }
	    switch (rnd(10)) {
		case 1:
		case 2:
		case 3:
		case 4:
		    *dmgptr += d(1,6) + 2;
		    break;
		case 5:
		case 6:
		case 7:
		    *dmgptr += d(2,6) + 4;
		    break;
		case 8:
		case 9:
		    *dmgptr += d(3,6) + 6;
		    break;
		case 10:
		    pline_The("poison was deadly...");
		    *dmgptr = FATAL_DAMAGE +
			    (youdefend ? (Upolyd ? u.mh : u.uhp) : mdef->mhp);
		    break;
	    }
	    return TRUE;
	}

	   if (otmp->oartifact == ART_DOOMBLADE && dieroll < 6) {
		if (youattack)
		    You("plunge the Doomblade deeply into %s!",
			    mon_nam(mdef));
		else
		    pline("%s plunges the Doomblade deeply into %s!",
			    Monnam(magr), hittee);
		*dmgptr += rnd(4) * 5;
		return TRUE;
	   }
  /* END OF STEPHEN WHITE'S NEW CODE */

#if 0
	   if (otmp->oartifact == ART_SCALPEL && dieroll < 5) {
		/* faster than a speeding bullet is the Gray Mouser... */
		pline("There is a flurry of blows!");
		/* I suppose this could theoretically continue forever... */
		while (dieroll < 5) {
		   *dmgptr += rnd(8) + 1 + otmp->spe;
		   dieroll = rn2(11);
		}
	   }
	   if (otmp->oartifact == ART_HEARTSEEKER && dieroll < 3) {
		/* this weapon just sounds nasty... yuck... */
		if (!youdefend) {
		   You("plunge Heartseeker into %s!",mon_nam(mdef));
		} else {
		   pline("%s plunges Heartseeker into you!",mon_nam(mdef));
		}
		*dmgptr += rnd(6)+rnd(6)+rnd(6)+rnd(6)+4;
	   }
#endif


	/* We really want "on a natural 20" but Nethack does it in */
	/* reverse from AD&D. */
	if (spec_ability(otmp, SPFX_BEHEAD)) {
	    if (otmp->oartifact == ART_TSURUGI_OF_MURAMASA && dieroll < 3) {
		/* not really beheading, but so close, why add another SPFX */
		if (youattack && u.uswallow && mdef == u.ustuck) {
		    You("slice %s wide open!", mon_nam(mdef));
		    *dmgptr = mdef->mhp + FATAL_DAMAGE;
		    return TRUE;
		}
		if (!youdefend) {
			/* allow normal cutworm() call to add extra damage */
			if(notonhead)
			    return FALSE;

			if (bigmonst(mdef->data)) {
				if (youattack)
					You("slice deeply into %s!",
						mon_nam(mdef));
				else if (vis)
					pline("%s cuts deeply into %s!",
					      Monnam(magr), hittee);
				*dmgptr *= 2;
				return TRUE;
			}
			*dmgptr = (Upolyd ? u.mh : u.uhp) + FATAL_DAMAGE;
			pline_The("razor-sharp blade cuts %s in half!",
			      mon_nam(mdef));
			otmp->dknown = TRUE;
			return TRUE;
		} else {
			/* Invulnerable player won't be bisected */
			if (bigmonst(youmonst.data) || Invulnerable) {
				pline("%s cuts deeply into you!",
					Monnam(magr));
				*dmgptr *= 2;
				return TRUE;
			}

			/* Players with negative AC's take less damage instead
			 * of just not getting hit.  We must add a large enough
			 * value to the damage so that this reduction in
			 * damage does not prevent death.
			 */
			*dmgptr = u.uhp + FATAL_DAMAGE;
			pline_The("razor-sharp blade cuts you in half!");
			otmp->dknown = TRUE;
			return TRUE;
		}
	    } else if (dieroll < 3 || otmp->oartifact == ART_VORPAL_BLADE &&
				      mdef->data == &mons[PM_JABBERWOCK]) {
		static const char *behead_msg[2] = {
		     "%s beheads %s!",
		     "%s decapitates %s!"
		};
		const char *artiname = artilist[otmp->oartifact].name;

		if (youattack && u.uswallow && mdef == u.ustuck)
			return FALSE;
		if (!youdefend) {
			if (!has_head(mdef->data) || notonhead || u.uswallow) {
				if (youattack)
					pline("Somehow, you miss %s wildly.",
						mon_nam(mdef));
				else if (vis)
					pline("Somehow, %s misses wildly.",
						mon_nam(magr));
				*dmgptr = 0;
				return ((boolean)(youattack || vis));
			}
			if (noncorporeal(mdef->data) || amorphous(mdef->data)) {
				pline("%s slices through %s %s.",
				      artiname,
				      s_suffix(mon_nam(mdef)), mbodypart(mdef,NECK));
				return TRUE;
			}
			*dmgptr = mdef->mhp + FATAL_DAMAGE;
			pline(behead_msg[rn2(SIZE(behead_msg))],
			      artiname,
			      mon_nam(mdef));
			otmp->dknown = TRUE;
			return TRUE;
		} else {
			if (!has_head(youmonst.data)) {
				pline("Somehow, %s misses you wildly.",
					mon_nam(magr));
				*dmgptr = 0;
				return TRUE;
			}
			if (noncorporeal(youmonst.data) || amorphous(youmonst.data)) {
				pline("%s slices through your %s.",
				      artiname, body_part(NECK));
				return TRUE;
			}
			*dmgptr = (Upolyd ? u.mh : u.uhp) + FATAL_DAMAGE;

			if (Invulnerable) {
				pline("%s slices into your %s.",
				      artiname, body_part(NECK));
				return TRUE;
			}
			pline(behead_msg[rn2(SIZE(behead_msg))],
			      artiname, "you");
			otmp->dknown = TRUE;
			/* Should amulets fall off? */
			return TRUE;
		}
	    }
	}
	if (spec_ability(otmp, SPFX_DRLI)) {
		if (!youdefend) {
			if (vis) {
			    if(otmp->oartifact == ART_STORMBRINGER)
				pline_The("%s blade draws the life from %s!",
				      hcolor(Black),
				      mon_nam(mdef));
#if 0	/* OBSOLETE */
			    else if(otmp->oartifact == ART_TENTACLE_STAFF)
				pline("The writhing tentacles draw the life from %s!",
				      mon_nam(mdef));
#endif
			    else
				pline("%s draws the life from %s!",
				      The(distant_name(otmp, xname)),
				      mon_nam(mdef));
			}
			if (mdef->m_lev == 0) {
			    *dmgptr = mdef->mhp + FATAL_DAMAGE;
			} else {
			    int drain = rnd(8);
			    *dmgptr += drain;
			    mdef->mhpmax -= drain;
			    mdef->m_lev--;
			    drain /= 2;
			    if (drain) healup(drain, 0, FALSE, FALSE);
			}
			return vis;
		} else { /* youdefend */
			int oldhpmax = u.uhpmax;

			if (Blind)
				You_feel("an %s drain your life!",
				    otmp->oartifact == ART_STORMBRINGER ?
				    "unholy blade" : "object");
			else if (otmp->oartifact == ART_STORMBRINGER)
				pline_The("%s blade drains your life!",
				      hcolor(Black));
			else
				pline("%s drains your life!",
				      The(distant_name(otmp, xname)));
#ifndef DEVEL_BRANCH
			losexp("life drainage");
#else /* DEVEL_BRANCH */
			losexp("life drainage", FALSE);
#endif /* DEVEL_BRANCH */
			if (magr->mhp < magr->mhpmax) {
			    magr->mhp += (u.uhpmax - oldhpmax)/2;
			    if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			}
			return TRUE;
		}
	}
	/* WAC -- 1/6 chance of cancellation with foobane weapons */
	if (otmp->oartifact == ART_GIANTKILLER ||
	    otmp->oartifact == ART_ORCRIST ||
	    otmp->oartifact == ART_DRAGONBANE ||
	    otmp->oartifact == ART_DEMONBANE ||
	    otmp->oartifact == ART_WEREBANE ||
	    otmp->oartifact == ART_TROLLSBANE ||
#ifdef BLACKMARKET
	    otmp->oartifact == ART_THIEFBANE ||
#endif
	    otmp->oartifact == ART_OGRESMASHER ||
	    otmp->oartifact == ART_ELFRIST) {
		register const struct artifact *weap = get_artifact(otmp);

		if (weap && spec_applies(weap, mdef) && (dieroll < 4)) {
			if (realizes_damage) {
				pline("%s %s!", The(distant_name(otmp, xname)),
						(Blind ? "roars deafeningly" : 
							 "shines brilliantly"));
				pline("It strikes %s!", 
					(youdefend ? "you" : mon_nam(mdef)));
			}
			cancel_monst(mdef, otmp, youattack, TRUE, (magr == mdef));
			return TRUE;
		}
	}
	return FALSE;
}

static NEARDATA const char recharge_type[] = { ALLOW_COUNT, ALL_CLASSES, 0 };
static NEARDATA const char invoke_types[] = { ALL_CLASSES, 0 };
		/* #invoke: an "ugly check" filters out most objects */

int
doinvoke()
{
    register struct obj *obj;

    obj = getobj(invoke_types, "invoke");
    if(!obj) return 0;
    return arti_invoke(obj);
}

STATIC_OVL int
arti_invoke(obj)
    register struct obj *obj;
{
    register const struct artifact *oart = get_artifact(obj);
	    register struct monst *mtmp;
	    register struct monst *mtmp2;
	    register struct permonst *pm;

    int summon_loop;
    int unseen;
/*
    int kill_loop;
 */

    if(!oart || !oart->inv_prop) {
	if(obj->otyp == CRYSTAL_BALL)
	    use_crystal_ball(obj);
	else
	    pline(nothing_happens);
	return 1;
    }

    if(oart->inv_prop > LAST_PROP) {
	/* It's a special power, not "just" a property */
	if(obj->age > monstermoves) {
	    /* the artifact is tired :-) */
	    You_feel("that %s is ignoring you.", the(xname(obj)));
	    /* and just got more so; patience is essential... */
	    obj->age += (long) d(3,10);
	    return 1;
	}
	obj->age = monstermoves + rnz(100);

	switch(oart->inv_prop) {
	case TAMING: {
	    struct obj *pseudo = mksobj(SPE_CHARM_MONSTER, FALSE, FALSE);
	    pseudo->blessed = pseudo->cursed = 0;
	    pseudo->quan = 20L;                 /* do not let useup get it */
	    (void) seffects(pseudo);
	    obfree(pseudo, (struct obj *)0);    /* now, get rid of it */
	    break;
	  }
	case HEALING: {
	    int healamt = (u.uhpmax + 1 - u.uhp) / 2;
 	    if (Upolyd) healamt = (u.mhmax + 1 - u.mh) / 2;
	    if(healamt || Sick || (Blinded > 1))
		You_feel("better.");
	    else
		goto nothing_special;
 	    if (healamt > 0) {
 		if (Upolyd) u.mh += healamt;
 		else u.uhp += healamt;
 	    }
	    if(Sick) make_sick(0L,(char *)0,FALSE,SICK_ALL);
	    if(Slimed) Slimed = 0L;
	    if(Blinded > 1) make_blinded(0L,FALSE);
	    flags.botl = 1;
	    break;
	  }
	case ENERGY_BOOST: {
	    int epboost = (u.uenmax + 1 - u.uen) / 2;
	    if (epboost > 120) epboost = 120;           /* arbitrary */
	    else if (epboost < 12) epboost = u.uenmax - u.uen;
	    if(epboost) {
		You_feel("re-energized.");
		u.uen += epboost;
		flags.botl = 1;
	    } else
		goto nothing_special;
	    break;
	  }
	case UNTRAP: {
	    if(!untrap(TRUE)) {
		obj->age = 0; /* don't charge for changing their mind */
		return 0;
	    }
	    break;
	  }
	case CHARGE_OBJ: {
	    struct obj *otmp = getobj(recharge_type, "charge");
	    boolean b_effect;
            if (!rn2(4)) {
	       	if (!otmp) {
	        	obj->age = 0;
		        return 0;
	    	}
	    b_effect = obj->blessed && (Role_switch == oart->role || !oart->role);
	    recharge(otmp, b_effect ? 1 : obj->cursed ? -1 : 0);
	    break;
	    }
	    else pline("Nothing happens.");
            break;
	}
	case LEV_TELE:
	    level_tele();
	    break;
	/* STEPHEN WHITE'S NEW CODE */       
	case LIGHT_AREA:
	    if (!Blind)
			pline("%s shines brightly for an instant!", The(xname(obj)));
	    else
			pline("%s grows warm for a second!", The(xname(obj)));

	    litroom(TRUE, obj); /* Light up the room */

	    vision_recalc(0); /*clean up vision*/

		/* WAC - added effect to self, damage is now range dependant */
		if(is_undead(youmonst.data)) {
			You("burn in the radiance!");
			
			/* This is ground zero.  Not good news ... */
			u.uhp /= 100;

			if (u.uhp < 1) {
				u.uhp = 0;
				killer_format = KILLED_BY;
				killer = "the Holy Spear of Light";
				done(DIED);
			}
		}

	    /* Undead and Demonics can't stand the light */
	    unseen = 0;
	    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
			if (DEADMONSTER(mtmp)) continue;
	    	
			/* Range is 9 paces */
	    	if (distu(mtmp->mx,mtmp->my) > 81) continue;

			if (couldsee(mtmp->mx, mtmp->my) &&
				(is_undead(mtmp->data) || is_demon(mtmp->data)) &&
				!resist(mtmp, '\0', 0, TELL)) {
					if (canseemon(mtmp))
						pline("%s burns in the radiance!", Monnam(mtmp));
					else
						unseen++;
					/* damage now depends on distance, divisor ranges from 10 to 2 */
					mtmp->mhp /= (10 - (distu(mtmp->mx,mtmp->my)/10));
					if (mtmp->mhp < 1) mtmp->mhp = 1;
			}
	    }
	    if (unseen)
			You("hear %s of intense pain!", unseen > 1 ? "cries" : "a cry");
	    break;
	case DEATH_GAZE:
	    if (u.uluck < -9) { /* uh oh... */
			pline("The Eye turns on you!");
			u.uhp = 0;
			killer_format = KILLED_BY;
			killer = "the Eye of the Beholder";
			done(DIED);
	    }
	    pline("The Eye looks around with its icy gaze!");
		for (mtmp = fmon; mtmp; mtmp = mtmp2) {
			mtmp2 = mtmp->nmon;
			/* The eye is never blind ... */
			if (couldsee(mtmp->mx, mtmp->my) && !is_undead(mtmp->data)) {
				pline("%s screams in agony!",Monnam(mtmp));
				mtmp->mhp /= 3;
				if (mtmp->mhp < 1) mtmp->mhp = 1;
			}
		}
	    /* Tsk,tsk.. */
	    adjalign(-3);
	    u.uluck -= 3;
	    break;
	case SUMMON_UNDEAD:
	    if (u.uluck < -9) { /* uh oh... */
		u.uhp -= (rn2(20)+5);
		pline("The Hand claws you with its icy nails!");
		if (u.uhp <= 0) {
		  killer_format = KILLED_BY;
		  killer="the Hand of Vecna";
		  done(DIED);
		}
	    }
	    summon_loop = rn2(4) + 4;
	    pline("Creatures from the grave surround you!");
	    do {
	      switch (rn2(6)+1) {
		case 1: mtmp = makemon(mkclass(S_VAMPIRE,0), u.ux, u.uy, NO_MM_FLAGS);
		   break;
		case 2:
		case 3: mtmp = makemon(mkclass(S_ZOMBIE,0), u.ux, u.uy, NO_MM_FLAGS);
		   break;
		case 4: mtmp = makemon(mkclass(S_MUMMY,0), u.ux, u.uy, NO_MM_FLAGS);
		   break;
		case 5: mtmp = makemon(mkclass(S_GHOST,0), u.ux, u.uy, NO_MM_FLAGS);
		   break;
               default: mtmp = makemon(mkclass(S_WRAITH,0), u.ux, u.uy, NO_MM_FLAGS);
		   break;
	      }
	      if ((mtmp2 = tamedog(mtmp, (struct obj *)0)) != 0)
			mtmp = mtmp2;
	      mtmp->mtame = 30;
	      summon_loop--;
	    } while (summon_loop);
	    /* Tsk,tsk.. */
	    adjalign(-3);
	    u.uluck -= 3;
	    break;
	case PROT_POLY:
	    You("feel more observant.");
	    rescham();
	    break;
	case SUMMON_FIRE_ELEMENTAL:
	    pm = &mons[PM_FIRE_ELEMENTAL];
	    mtmp = makemon(pm, u.ux, u.uy, NO_MM_FLAGS);
   
	    pline("You summon an elemental.");
   
	    if ((mtmp2 = tamedog(mtmp, (struct obj *)0)) != 0)
			mtmp = mtmp2;
	    mtmp->mtame = 30;
	    break;
	case SUMMON_WATER_ELEMENTAL:
	    pm = &mons[PM_WATER_ELEMENTAL];
	    mtmp = makemon(pm, u.ux, u.uy, NO_MM_FLAGS);
   
	    pline("You summon an elemental.");
	    
	    if ((mtmp2 = tamedog(mtmp, (struct obj *)0)) != 0)
			mtmp = mtmp2;
	    mtmp->mtame = 30;
	    break;
	case OBJ_DETECTION:
		(void)object_detect(obj, 0);
		break;
	case CREATE_PORTAL: {
	    int i, num_ok_dungeons, last_ok_dungeon = 0;
	    d_level newlev;
	    extern int n_dgns; /* from dungeon.c */
	    winid tmpwin = create_nhwindow(NHW_MENU);
	    anything any;

	    any.a_void = 0;     /* set all bits to zero */
 #ifdef BLACKMARKET           
	    if (Is_blackmarket(&u.uz) && *u.ushops) {
		You("feel very disoriented for a moment.");
		break;
	    }
 #endif
	    start_menu(tmpwin);
	    /* use index+1 (cant use 0) as identifier */
	    for (i = num_ok_dungeons = 0; i < n_dgns; i++) {
		if (!dungeons[i].dunlev_ureached) continue;
		any.a_int = i+1;
		add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
			 dungeons[i].dname, MENU_UNSELECTED);
		num_ok_dungeons++;
		last_ok_dungeon = i;
	    }
	    end_menu(tmpwin, "Open a portal to which dungeon?");
	    if (num_ok_dungeons > 1) {
		/* more than one entry; display menu for choices */
		menu_item *selected;
		int n;

		n = select_menu(tmpwin, PICK_ONE, &selected);
		if (n <= 0) {
		    destroy_nhwindow(tmpwin);
		    goto nothing_special;
		}
		i = selected[0].item.a_int - 1;
		free((genericptr_t)selected);
	    } else
		i = last_ok_dungeon;    /* also first & only OK dungeon */
	    destroy_nhwindow(tmpwin);

	    /*
	     * i is now index into dungeon structure for the new dungeon.
	     * Find the closest level in the given dungeon, open
	     * a use-once portal to that dungeon and go there.
	     * The closest level is either the entry or dunlev_ureached.
	     */
	    newlev.dnum = i;
	    if(dungeons[i].depth_start >= depth(&u.uz))
		newlev.dlevel = dungeons[i].entry_lev;
	    else
		newlev.dlevel = dungeons[i].dunlev_ureached;
	    if(u.uhave.amulet || In_endgame(&u.uz) || In_endgame(&newlev) ||
	       newlev.dnum == u.uz.dnum) {
		You_feel("very disoriented for a moment.");
	    } else {
		if(!Blind) You("are surrounded by a shimmering sphere!");
		else You_feel("weightless for a moment.");
		goto_level(&newlev, FALSE, FALSE, FALSE);
	    }
	    break;
	  }
	case ENLIGHTENING:
	    enlightenment(0);
	    break;
	case CREATE_AMMO: {
	    struct obj *otmp = mksobj(ARROW, TRUE, FALSE);

	    if (!otmp) goto nothing_special;
	    otmp->blessed = obj->blessed;
	    otmp->cursed = obj->cursed;
	    otmp->bknown = obj->bknown;
	    if (obj->blessed) {
		if (otmp->spe < 0) otmp->spe = 0;
		otmp->quan += rnd(10);
	    } else if (obj->cursed) {
		if (otmp->spe > 0) otmp->spe = 0;
	    } else
		otmp->quan += rnd(5);
	    otmp->owt = weight(otmp);
	    otmp = hold_another_object(otmp, "Suddenly %s out.",
				       aobjnam(otmp, "fall"), (const char *)0);
	    break;
	  }
	}
    } else {
	long cprop = (u.uprops[oart->inv_prop].extrinsic ^= W_ARTI);
	boolean on = (cprop & W_ARTI) != 0; /* true if invoked prop just set */

	if(on && obj->age > monstermoves) {
	    /* the artifact is tired :-) */
	    u.uprops[oart->inv_prop].extrinsic ^= W_ARTI;
	    You_feel("that %s is ignoring you.", the(xname(obj)));
	    return 1;
	} else if(!on) {
	    /* when turning off property, determine downtime */
	    /* arbitrary for now until we can tune this -dlc */
	    obj->age = monstermoves + rnz(100);
	}

	if(cprop & ~W_ARTI) {
nothing_special:
	    /* you had the property from some other source too */
	    if (carried(obj))
		You_feel("a surge of power, but nothing seems to happen.");
	    return 1;
	}
	switch(oart->inv_prop) {
	case CONFLICT:
	    if(on) You_feel("like a rabble-rouser.");
	    else You_feel("the tension decrease around you.");
	    break;
	case LEVITATION:
	    if(on) float_up();
	    else (void) float_down(I_SPECIAL|TIMEOUT, W_ARTI);
	    break;
	case INVIS:
	    if (!See_invisible && !Blind) {
		newsym(u.ux,u.uy);
		if (on) {
		    Your("body takes on a %s transparency...",
			 Hallucination ? "normal" : "strange");
		} else {
		    Your("body seems to unfade...");
		}
	    } else goto nothing_special;
	    break;
	}
    }

    return 1;
}

/* WAC return TRUE if artifact is always lit */
boolean
artifact_light(obj)
    struct obj *obj;
{
    return (get_artifact(obj) && (
        (get_artifact(obj)->inv_prop == LIGHT_AREA)
        || (obj->oartifact == ART_CANDLE_OF_ETERNAL_FLAME)));
}


/*
 * Artifact is dipped into water
 * -1 not handled here (not used up here)
 *  0 no effect but used up
 *  else return
 *  AD_FIRE, etc.
 *  Note caller should handle what happens to the medium in these cases.
 *      This only prints messages about the actual artifact.
 */

int
artifact_wet(obj, silent)
struct obj *obj;
boolean silent;
{
	 if (!obj->oartifact) return (-1);
	 switch (artilist[(int) (obj)->oartifact].attk.adtyp) {
		 case AD_FIRE:
			 if (!silent) {
				pline("A cloud of steam rises.");
				pline("%s is untouched.", The(xname(obj)));
			 }
			 return (AD_FIRE);
		 case AD_COLD:
			 if (!silent) {
				pline("Icicles form and fall from the freezing %s.",
			             the(xname(obj)));
			 }
			 return (AD_COLD);
		 case AD_ELEC:
			 if (!silent) {
				pline_The("humid air crackles with electricity from %s.",
						the(xname(obj)));
			 }
			 return (AD_ELEC);
		 case AD_DRLI:
			 if (!silent) {
				pline("%s absorbs the water!", The(xname(obj)));
			 }
			 return (AD_DRLI);
		 default:
			 break;
	}
	return (-1);
}

/* KMH -- Talking artifacts are finally implemented */
void arti_speak(obj)
    struct obj *obj;
{
	register const struct artifact *oart = get_artifact(obj);
	const char *line;
	char buf[BUFSZ];


	/* Is this a speaking artifact? */
	if (!oart || !(oart->spfx & SPFX_SPEAK))
		return;

	line = getrumor(bcsign(obj), buf, TRUE);
	if (!*line)
		line = "Slash'EM rumors file closed for renovation.";
	pline("%s whispers:", The(xname(obj)));
	verbalize("%s", line);
	return;
}

  
#endif /* OVLB */

/*artifact.c*/
