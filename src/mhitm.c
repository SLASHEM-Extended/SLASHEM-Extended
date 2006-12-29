/*	SCCS Id: @(#)mhitm.c	3.4	2003/01/02	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "artifact.h"
#include "edog.h"

extern boolean notonhead;
extern const char *breathwep[];		/* from mthrowu.c */

#define POLE_LIM 5	/* How far monsters can use pole-weapons */

#ifdef OVLB

static NEARDATA boolean vis, far_noise;
static NEARDATA long noisetime;
static NEARDATA struct obj *otmp;

static const char brief_feeling[] =
	"have a %s feeling for a moment, then it passes.";

STATIC_DCL char *FDECL(mon_nam_too, (char *,struct monst *,struct monst *));
STATIC_DCL void FDECL(mrustm, (struct monst *, struct monst *, struct obj *));
STATIC_DCL int FDECL(breamm, (struct monst *, struct monst *, struct attack *));
STATIC_DCL int FDECL(spitmm, (struct monst *, struct monst *, struct attack *));
STATIC_DCL int FDECL(thrwmm, (struct monst *, struct monst *));
STATIC_DCL int FDECL(hitmm, (struct monst *,struct monst *,struct attack *));
STATIC_DCL int FDECL(gazemm, (struct monst *,struct monst *,struct attack *));
STATIC_DCL int FDECL(gulpmm, (struct monst *,struct monst *,struct attack *));
STATIC_DCL int FDECL(explmm, (struct monst *,struct monst *,struct attack *));
STATIC_DCL int FDECL(mdamagem, (struct monst *,struct monst *,struct attack *));
STATIC_DCL void FDECL(mswingsm, (struct monst *, struct monst *, struct obj *));
STATIC_DCL void FDECL(noises,(struct monst *,struct attack *));
STATIC_DCL void FDECL(missmm,(struct monst *,struct monst *, int, int, struct attack *));
STATIC_DCL int FDECL(passivemm, (struct monst *, struct monst *, BOOLEAN_P, int));

/* Needed for the special case of monsters wielding vorpal blades (rare).
 * If we use this a lot it should probably be a parameter to mdamagem()
 * instead of a global variable.
 */
static int dieroll;

/* returns mon_nam(mon) relative to other_mon; normal name unless they're
   the same, in which case the reference is to {him|her|it} self */
STATIC_OVL char *
mon_nam_too(outbuf, mon, other_mon)
char *outbuf;
struct monst *mon, *other_mon;
{
	Strcpy(outbuf, mon_nam(mon));
	if (mon == other_mon)
	    switch (pronoun_gender(mon)) {
	    case 0:	Strcpy(outbuf, "himself");  break;
	    case 1:	Strcpy(outbuf, "herself");  break;
	    default:	Strcpy(outbuf, "itself"); break;
	    }
	return outbuf;
}

STATIC_OVL void
noises(magr, mattk)
	register struct monst *magr;
	register struct	attack *mattk;
{
	boolean farq = (distu(magr->mx, magr->my) > 15);

	if(flags.soundok && (farq != far_noise || moves-noisetime > 10)) {
		far_noise = farq;
		noisetime = moves;
		You_hear("%s%s.",
			(mattk->aatyp == AT_EXPL) ? "an explosion" : "some noises",
			farq ? " in the distance" : "");
	}
}

STATIC_OVL
void
missmm(magr, mdef, target, roll, mattk)
	register struct monst *magr, *mdef;
	struct attack *mattk;
	int target, roll;
{
	boolean nearmiss = (target == roll);
	const char *fmt;
        char buf[BUFSZ], mon_name[BUFSZ];

	register struct obj *blocker = (struct obj *)0;
	long mwflags = mdef->misc_worn_check;

		/* 3 values for blocker
		 *	No blocker:  (struct obj *) 0  
		 * 	Piece of armour:  object
		 */

	/* This is a hack,  since there is no fast equivalent for uarm, uarms, etc.  
	 * Technically, we really should check from the inside out...
	 */
	if (target < roll) {
	    for (blocker = mdef->minvent; blocker; blocker = blocker->nobj) {
		if (blocker->owornmask & mwflags) {
			target += ARM_BONUS(blocker);
			if (target > roll) break;
		}
	    }
	}

	if (vis) {
		if (!canspotmon(magr))
		    map_invisible(magr->mx, magr->my);
		if (!canspotmon(mdef))
		    map_invisible(mdef->mx, mdef->my);
		if (mdef->m_ap_type) seemimic(mdef);
		if (magr->m_ap_type) seemimic(magr);
		if (flags.verbose && !nearmiss && blocker) {
			fmt = "%s %s blocks";
			Sprintf(buf,fmt, s_suffix(Monnam(mdef)), 
				aobjnam(blocker, (char *)0));
	                pline("%s %s.", buf, mon_nam_too(mon_name, magr, mdef));
		} else {
		fmt = (could_seduce(magr,mdef,mattk) && !magr->mcan) ?
				"%s pretends to be friendly to" : 
				((flags.verbose && nearmiss) ? "%s just misses" : 
				  "%s misses");
		Sprintf(buf, fmt, Monnam(magr));
	                pline("%s %s.", buf, mon_nam_too(mon_name, mdef, magr));
		}
	} else  noises(magr, mattk);
}

/*
 *  fightm()  -- fight some other monster
 *
 *  Returns:
 *	0 - Monster did nothing.
 *	1 - If the monster made an attack.  The monster might have died.
 *
 *  There is an exception to the above.  If mtmp has the hero swallowed,
 *  then we report that the monster did nothing so it will continue to
 *  digest the hero.
 */
int
fightm(mtmp)		/* have monsters fight each other */
	register struct monst *mtmp;
{
	register struct monst *mon, *nmon;
	int result, has_u_swallowed;
#ifdef LINT
	nmon = 0;
#endif
	/* perhaps the monster will resist Conflict */
	if(resist(mtmp, RING_CLASS, 0, 0))
	    return(0);

	if(u.ustuck == mtmp) {
	    /* perhaps we're holding it... */
	    if(itsstuck(mtmp))
		return(0);
	}
	has_u_swallowed = (u.uswallow && (mtmp == u.ustuck));

	for(mon = fmon; mon; mon = nmon) {
	    nmon = mon->nmon;
	    if(nmon == mtmp) nmon = mtmp->nmon;
	    /* Be careful to ignore monsters that are already dead, since we
	     * might be calling this before we've cleaned them up.  This can
	     * happen if the monster attacked a cockatrice bare-handedly, for
	     * instance.
	     */
	    if(mon != mtmp && !DEADMONSTER(mon)) {
		if(monnear(mtmp,mon->mx,mon->my)) {
		    if(!u.uswallow && (mtmp == u.ustuck)) {
			if(!rn2(4)) {
			    pline("%s releases you!", Monnam(mtmp));
			    setustuck(0);
			} else
			    break;
		    }

		    /* mtmp can be killed */
		    bhitpos.x = mon->mx;
		    bhitpos.y = mon->my;
		    notonhead = 0;
		    result = mattackm(mtmp,mon);

		    if (result & MM_AGR_DIED) return 1;	/* mtmp died */
		    /*
		     *  If mtmp has the hero swallowed, lie and say there
		     *  was no attack (this allows mtmp to digest the hero).
		     */
		    if (has_u_swallowed) return 0;

		    /* Allow attacked monsters a chance to hit back. Primarily
		     * to allow monsters that resist conflict to respond.
		     */
		    if ((result & MM_HIT) && !(result & MM_DEF_DIED) &&
			rn2(4) && mon->movement >= NORMAL_SPEED) {
			mon->movement -= NORMAL_SPEED;
			notonhead = 0;
			(void) mattackm(mon, mtmp);	/* return attack */
		    }

		    return ((result & MM_HIT) ? 1 : 0);
		}
	    }
	}
	return 0;
}

/*
 * mattackm() -- a monster attacks another monster.
 *
 * This function returns a result bitfield:
 *
 *	    --------- aggressor died
 *	   /  ------- defender died
 *	  /  /  ----- defender was hit
 *	 /  /  /
 *	x  x  x
 *
 *	0x4	MM_AGR_DIED
 *	0x2	MM_DEF_DIED
 *	0x1	MM_HIT
 *	0x0	MM_MISS
 *
 * Each successive attack has a lower probability of hitting.  Some rely on the
 * success of previous attacks.  ** this doen't seem to be implemented -dl **
 *
 * In the case of exploding monsters, the monster dies as well.
 */
int
mattackm(magr, mdef)
    register struct monst *magr,*mdef;
{
    int		    i,		/* loop counter */
		    tmp,	/* amour class difference */
		    strike,	/* hit this attack */
		    attk,	/* attack attempted this time */
		    struck = 0,	/* hit at least once */
		    res[NATTK];	/* results of all attacks */
    struct attack   *mattk, alt_attk;
    struct permonst *pa, *pd;
    /*
     * Pets don't use "ranged" attacks for fear of hitting their master
     */
    boolean range;

    if (!magr || !mdef) return(MM_MISS);		/* mike@genat */
    if (!magr->mcanmove || magr->msleeping) return(MM_MISS);
    pa = magr->data;  pd = mdef->data;

    /* Grid bugs cannot attack at an angle. */
    if (pa == &mons[PM_GRID_BUG] && magr->mx != mdef->mx
						&& magr->my != mdef->my)
	return(MM_MISS);

    range = !magr->mtame && !monnear(magr, mdef->mx, mdef->my);

    /* Calculate the armour class differential. */
    tmp = find_mac(mdef) + magr->m_lev;
    if (mdef->mconf || !mdef->mcanmove || mdef->msleeping) {
	tmp += 4;
	mdef->msleeping = 0;
    }

    /* undetect monsters become un-hidden if they are attacked */
    if (mdef->mundetected) {
	mdef->mundetected = 0;
	newsym(mdef->mx, mdef->my);
	if(canseemon(mdef) && !sensemon(mdef)) {
	    if (u.usleep) You("dream of %s.",
				(mdef->data->geno & G_UNIQ) ?
				a_monnam(mdef) : makeplural(m_monnam(mdef)));
	    else pline("Suddenly, you notice %s.", a_monnam(mdef));
	}
    }

    /* Elves hate orcs. */
    if (is_elf(pa) && is_orc(pd)) tmp++;


    /* Set up the visibility of action */
    vis = (cansee(magr->mx,magr->my) && cansee(mdef->mx,mdef->my) && (canspotmon(magr) || canspotmon(mdef)));

    /*	Set flag indicating monster has moved this turn.  Necessary since a
     *	monster might get an attack out of sequence (i.e. before its move) in
     *	some cases, in which case this still counts as its move for the round
     *	and it shouldn't move again.
     */
    magr->mlstmv = monstermoves;

    /* Now perform all attacks for the monster. */
    for (i = 0; i < NATTK; i++) {
	res[i] = MM_MISS;
	mattk = getmattk(pa, i, res, &alt_attk);
	otmp = (struct obj *)0;
	attk = 1;
	switch (mattk->aatyp) {
	    case AT_BREA:
	    case AT_SPIT:
		if (range) {
		    if (mattk->aatyp == AT_BREA)
			res[i] = breamm(magr, mdef, mattk);
		    else
			res[i] = spitmm(magr, mdef, mattk);
		    /* We can't distinguish no action from failed attack
		     * so assume defender doesn't waken unless actually hit.
		     */
		    strike = res[i] & MM_HIT;
		} else
		    strike = 0;
		attk = 0;
		break;

	    case AT_MAGC:
		/* [ALI] Monster-on-monster spell casting always fails. This
		 * is partly for balance reasons and partly because the
		 * amount of code required to implement it is prohibitive.
		 */
		strike = 0;
		attk = 0;
		if (canseemon(magr) && couldsee(magr->mx, magr->my)) {
		    char buf[BUFSZ];
		    Strcpy(buf, Monnam(magr));
		    if (vis)
			pline("%s points at %s, then curses.", buf,
				mon_nam(mdef));
		    else
			pline("%s points and curses at something.", buf);
		} else if (flags.soundok)
		    Norep("You hear a mumbled curse.");
		break;

	    case AT_WEAP:
		/* "ranged" attacks */
#ifdef REINCARNATION
		if (!Is_rogue_level(&u.uz) && range) {
#else
		if (range) {
#endif
		    res[i] = thrwmm(magr, mdef);
		    attk = 0;
		    strike = res[i] & MM_HIT;
		    break;
		}
		/* "hand to hand" attacks */
		if (magr->weapon_check == NEED_WEAPON || !MON_WEP(magr)) {
		    magr->weapon_check = NEED_HTH_WEAPON;
		    if (mon_wield_item(magr) != 0) return 0;
		}
		possibly_unwield(magr, FALSE);
		otmp = MON_WEP(magr);

		if (otmp) {
		    if (vis) mswingsm(magr, mdef, otmp);
		    tmp += hitval(otmp, mdef);
		}
		/* fall through */
	    case AT_CLAW:
	    case AT_KICK:
	    case AT_BITE:
	    case AT_STNG:
	    case AT_TUCH:
	    case AT_BUTT:
	    case AT_TENT:
		/* Nymph that teleported away on first attack? */
		if (distmin(magr->mx,magr->my,mdef->mx,mdef->my) > 1)
		    return MM_MISS;
		/* Monsters won't attack cockatrices physically if they
		 * have a weapon instead.  This instinct doesn't work for
		 * players, or under conflict or confusion. 
		 */
		if (!magr->mconf && !Conflict && otmp &&
		    mattk->aatyp != AT_WEAP && touch_petrifies(mdef->data)) {
		    strike = 0;
		    break;
		}
		dieroll = rnd(20 + i);
		strike = (tmp > dieroll);
		if (strike) {
		    res[i] = hitmm(magr, mdef, mattk);
		    if((mdef->data == &mons[PM_BLACK_PUDDING] || mdef->data == &mons[PM_BROWN_PUDDING])
		       && otmp && objects[otmp->otyp].oc_material == IRON
		       && mdef->mhp > 1 && !mdef->mcan)
		    {
			if (clone_mon(mdef, 0, 0)) {
			    if (vis) {
				char buf[BUFSZ];

				Strcpy(buf, Monnam(mdef));
				pline("%s divides as %s hits it!", buf, mon_nam(magr));
			    }
			}
		    }
		} else
		    missmm(magr, mdef, tmp, dieroll, mattk);
		/* KMH -- don't accumulate to-hit bonuses */
		if (otmp)
		    tmp -= hitval(otmp, mdef);
		break;

	    case AT_HUGS:	/* automatic if prev two attacks succeed */
		strike = (i >= 2 && res[i-1] == MM_HIT && res[i-2] == MM_HIT);
		if (strike)
		    res[i] = hitmm(magr, mdef, mattk);

		break;

	    case AT_GAZE:
		strike = 0;	/* will not wake up a sleeper */
		res[i] = gazemm(magr, mdef, mattk);
		break;

	    case AT_EXPL:
		res[i] = explmm(magr, mdef, mattk);
		if (res[i] == MM_MISS) { /* cancelled--no attack */
		    strike = 0;
		    attk = 0;
		} else
		    strike = 1;	/* automatic hit */
		break;

	    case AT_ENGL:
#ifdef STEED
		if (u.usteed && (mdef == u.usteed)) {
		    strike = 0;
		    break;
		} 
#endif
		/* Engulfing attacks are directed at the hero if
		 * possible. -dlc
		 */
		if (u.uswallow && magr == u.ustuck)
		    strike = 0;
		else {
		    if ((strike = (tmp > (dieroll = rnd(20+i)))))
			res[i] = gulpmm(magr, mdef, mattk);
		    else
			missmm(magr, mdef, tmp, dieroll, mattk);
		}
		break;

	    default:		/* no attack */
		strike = 0;
		attk = 0;
		break;
	}

	if (attk && !(res[i] & MM_AGR_DIED))
	    res[i] = passivemm(magr, mdef, strike, res[i] & MM_DEF_DIED);

	if (res[i] & MM_DEF_DIED) return res[i];

	/*
	 *  Wake up the defender.  NOTE:  this must follow the check
	 *  to see if the defender died.  We don't want to modify
	 *  unallocated monsters!
	 */
	if (strike) mdef->msleeping = 0;

	if (res[i] & MM_AGR_DIED)  return res[i];
	/* return if aggressor can no longer attack */
	if (!magr->mcanmove || magr->msleeping) return res[i];
	if (res[i] & MM_HIT) struck = 1;	/* at least one hit */
    }

    return(struck ? MM_HIT : MM_MISS);
}

/* monster attempts breath attack against another monster */
STATIC_OVL int
breamm(magr, mdef, mattk)
struct monst *magr, *mdef;
struct attack *mattk;
{
    /* if new breath types are added, change AD_ACID to max type */
    int typ = mattk->adtyp == AD_RBRE ? rnd(AD_ACID) : mattk->adtyp;
    int mhp;

    if (linedup(mdef->mx, mdef->my, magr->mx, magr->my)) {
	if (magr->mcan) {
	    if (flags.soundok) {
		if (canseemon(magr))
		    pline("%s coughs.", Monnam(magr));
		else
		    You_hear("a cough.");
	    }
	} else if (!magr->mspec_used && rn2(3)) {
	    if (typ >= AD_MAGM && typ <= AD_ACID) {
		if (canseemon(magr))
		    pline("%s breathes %s!", Monnam(magr), breathwep[typ-1]);
		mhp = mdef->mhp;
		buzz((int)(-20 - (typ-1)), (int)mattk->damn,
			magr->mx, magr->my, sgn(tbx), sgn(tby));
		nomul(0);
		/* breath runs out sometimes. */
		if (!rn2(3))
		    magr->mspec_used = 10+rn2(20);
		return (mdef->mhp < 1 ? MM_DEF_DIED : 0) |
		       (mdef->mhp < mhp ? MM_HIT : 0) |
		       (magr->mhp < 1 ? MM_AGR_DIED : 0);
	    } else impossible("Breath weapon %d used", typ-1);
	}
    }
    return MM_MISS;
}

/* monster attempts spit attack against another monster */
STATIC_OVL int
spitmm(magr, mdef, mattk)
struct monst *magr, *mdef;
struct attack *mattk;
{
    register struct obj *obj;
    int mhp;

    if (magr->mcan) {
	if (flags.soundok) {
	    if (canseemon(magr))
		pline("A dry rattle comes from %s throat.",
			s_suffix(mon_nam(magr)));
	    else
		You_hear("a dry rattle.");
	}
	return MM_MISS;
    }

    if (linedup(mdef->mx, mdef->my, magr->mx, magr->my)) {
	switch (mattk->adtyp) {
	    case AD_BLND:
	    case AD_DRST:
		obj = mksobj(BLINDING_VENOM, TRUE, FALSE);
		break;
	    default:
		impossible("bad attack type in spitmm");
	    /* fall through */
	    case AD_ACID:
		obj = mksobj(ACID_VENOM, TRUE, FALSE);
		break;
	}
	if (!rn2(BOLT_LIM - distmin(magr->mx, magr->my, mdef->mx, mdef->my))) {
	    if (canseemon(magr))
		pline("%s spits venom!", Monnam(magr));
	    mhp = mdef->mhp;
	    m_throw(magr, magr->mx, magr->my, sgn(tbx), sgn(tby),
		    distmin(magr->mx, magr->my, mdef->mx, mdef->my), obj);
	    nomul(0);
	    return (mdef->mhp < 1 ? MM_DEF_DIED : 0) |
		   (mdef->mhp < mhp ? MM_HIT : 0) |
		   (magr->mhp < 1 ? MM_AGR_DIED : 0);
	}
    }
    return MM_MISS;
}

/* monster attempts ranged weapon attack against another monster */
STATIC_OVL int
thrwmm(magr, mdef)
struct monst *magr, *mdef;
{
    struct obj *obj, *mwep;
    schar skill;
    int multishot, mhp;
    const char *onm;

    /* Rearranged beginning so monsters can use polearms not in a line */
    if (magr->weapon_check == NEED_WEAPON || !MON_WEP(magr)) {
	magr->weapon_check = NEED_RANGED_WEAPON;
	/* mon_wield_item resets weapon_check as appropriate */
	if(mon_wield_item(magr) != 0) return MM_MISS;
    }

    /* Pick a weapon */
    obj = select_rwep(magr);
    if (!obj) return MM_MISS;

    if (is_pole(obj)) {
	int dam, hitv, vis = canseemon(magr);

	if (dist2(magr->mx, magr->my, mdef->mx, mdef->my) > POLE_LIM ||
		!m_cansee(magr, mdef->mx, mdef->my))
	    return MM_MISS;	/* Out of range, or intervening wall */

	if (vis) {
	    onm = xname(obj);
	    pline("%s thrusts %s.", Monnam(magr),
		  obj_is_pname(obj) ? the(onm) : an(onm));
	}

	dam = dmgval(obj, mdef);
	hitv = 3 - distmin(mdef->mx, mdef->my, magr->mx, magr->my);
	if (hitv < -4) hitv = -4;
	if (bigmonst(mdef->data)) hitv++;
	hitv += 8 + obj->spe;
	if (dam < 1) dam = 1;

	if (find_mac(mdef) + hitv <= rnd(20)) {
	    if (flags.verbose && canseemon(mdef))
		pline("It misses %s.", mon_nam(mdef));
	    else if (vis)
		pline("It misses.");
	    return MM_MISS;
	} else {
	    if (flags.verbose && canseemon(mdef))
		pline("It hits %s%s", a_monnam(mdef), exclam(dam));
	    else if (vis)
		pline("It hits.");
	    if (objects[obj->otyp].oc_material == SILVER &&
		    hates_silver(mdef->data) && canseemon(mdef)) {
		if (vis)
		    pline_The("silver sears %s flesh!",
			    s_suffix(mon_nam(mdef)));
		else
		    pline("%s flesh is seared!", s_suffix(Monnam(mdef)));
	    }
	    mdef->mhp -= dam;
	    if (mdef->mhp < 1) {
		if (canseemon(mdef))
		    pline("%s is %s!", Monnam(mdef),
			    (nonliving(mdef->data) || !canspotmon(mdef))
			    ? "destroyed" : "killed");
		mondied(mdef);
		return MM_DEF_DIED | MM_HIT;
	    }
	    else
		return MM_HIT;
	}
    }

    if (!linedup(mdef->mx, mdef->my, magr->mx, magr->my))
	return MM_MISS;

    skill = objects[obj->otyp].oc_skill;
    mwep = MON_WEP(magr);		/* wielded weapon */

    if (ammo_and_launcher(obj, mwep) && objects[mwep->otyp].oc_range &&
	    dist2(magr->mx, magr->my, mdef->mx, mdef->my) >
	    objects[mwep->otyp].oc_range * objects[mwep->otyp].oc_range)
	return MM_MISS; /* Out of range */

    /* Multishot calculations */
    multishot = 1;
    if ((ammo_and_launcher(obj, mwep) || skill == P_DAGGER ||
	    skill == -P_DART || skill == -P_SHURIKEN) && !magr->mconf) {
	/* Assumes lords are skilled, princes are expert */
	if (is_prince(magr->data)) multishot += 2;
	else if (is_lord(magr->data)) multishot++;

	/*  Elven Craftsmanship makes for light,  quick bows */
	if (obj->otyp == ELVEN_ARROW && !obj->cursed)
	    multishot++;
	if (mwep && mwep->otyp == ELVEN_BOW && !mwep->cursed) multishot++;
	/* 1/3 of object enchantment */
	if (mwep && mwep->spe > 1)
	    multishot += rounddiv(mwep->spe, 3);
	/* Some randomness */
	if (multishot > 1)
	    multishot = rnd(multishot);
#ifdef FIREARMS
	if (mwep && objects[mwep->otyp].oc_rof && is_launcher(mwep))
	    multishot += objects[mwep->otyp].oc_rof;
#endif

	switch (monsndx(magr->data)) {
	case PM_RANGER:
		multishot++;
		break;
	case PM_ROGUE:
		if (skill == P_DAGGER) multishot++;
		break;
	case PM_NINJA:
	case PM_SAMURAI:
		if (obj->otyp == YA && mwep &&
		    mwep->otyp == YUMI) multishot++;
		break;
	default:
	    break;
	}
	/* racial bonus */
	if ((is_elf(magr->data) &&
		obj->otyp == ELVEN_ARROW &&
		mwep && mwep->otyp == ELVEN_BOW) ||
	    (is_orc(magr->data) &&
		obj->otyp == ORCISH_ARROW &&
		mwep && mwep->otyp == ORCISH_BOW))
	    multishot++;

	if ((long)multishot > obj->quan) multishot = (int)obj->quan;
	if (multishot < 1) multishot = 1;
	/* else multishot = rnd(multishot); */
    }

    if (canseemon(magr)) {
	char onmbuf[BUFSZ];

	if (multishot > 1) {
	    /* "N arrows"; multishot > 1 implies obj->quan > 1, so
	       xname()'s result will already be pluralized */
	    Sprintf(onmbuf, "%d %s", multishot, xname(obj));
	    onm = onmbuf;
	} else {
	    /* "an arrow" */
	    onm = singular(obj, xname);
	    onm = obj_is_pname(obj) ? the(onm) : an(onm);
	}
	m_shot.s = ammo_and_launcher(obj,mwep) ? TRUE : FALSE;
	pline("%s %s %s!", Monnam(magr),
#ifdef FIREARMS
	      m_shot.s ? is_bullet(obj) ? "fires" : "shoots" : "throws",
	      onm);
#else
	      m_shot.s ? "shoots" : "throws", onm);
#endif
	m_shot.o = obj->otyp;
    } else {
	m_shot.o = STRANGE_OBJECT;	/* don't give multishot feedback */
    }

    mhp = mdef->mhp;
    m_shot.n = multishot;
    for (m_shot.i = 1; m_shot.i <= m_shot.n; m_shot.i++)
	m_throw(magr, magr->mx, magr->my, sgn(tbx), sgn(tby),
		distmin(magr->mx, magr->my, mdef->mx, mdef->my), obj);
    m_shot.n = m_shot.i = 0;
    m_shot.o = STRANGE_OBJECT;
    m_shot.s = FALSE;

    nomul(0);

    return (mdef->mhp < 1 ? MM_DEF_DIED : 0) | (mdef->mhp < mhp ? MM_HIT : 0) |
	   (magr->mhp < 1 ? MM_AGR_DIED : 0);
}

/* Returns the result of mdamagem(). */
STATIC_OVL int
hitmm(magr, mdef, mattk)
	register struct monst *magr,*mdef;
	struct	attack *mattk;
{
	if(vis){
		int compat;
		char buf[BUFSZ], mdef_name[BUFSZ];

		if (!canspotmon(magr))
		    map_invisible(magr->mx, magr->my);
		if (!canspotmon(mdef))
		    map_invisible(mdef->mx, mdef->my);
		if(mdef->m_ap_type) seemimic(mdef);
		if(magr->m_ap_type) seemimic(magr);
		if((compat = could_seduce(magr,mdef,mattk)) && !magr->mcan) {
			Sprintf(buf, "%s %s", Monnam(magr),
				mdef->mcansee ? "smiles at" : "talks to");
			pline("%s %s %s.", buf, mon_nam(mdef),
				compat == 2 ?
					"engagingly" : "seductively");
		} else {
		    char magr_name[BUFSZ];

		    Strcpy(magr_name, Monnam(magr));
		    switch (mattk->aatyp) {
			case AT_BITE:
				Sprintf(buf,"%s bites", magr_name);
				break;
			case AT_STNG:
				Sprintf(buf,"%s stings", magr_name);
				break;
			case AT_BUTT:
				Sprintf(buf,"%s butts", magr_name);
				break;
			case AT_TUCH:
				Sprintf(buf,"%s touches", magr_name);
				break;
			case AT_TENT:
				Sprintf(buf, "%s tentacles suck",
					s_suffix(magr_name));
				break;
			case AT_HUGS:
				if (magr != u.ustuck) {
				    Sprintf(buf,"%s squeezes", magr_name);
				    break;
				}
			case AT_MULTIPLY:
				/* No message. */
				break;
			default:
				Sprintf(buf,"%s hits", magr_name);
		    }
		    pline("%s %s.", buf, mon_nam_too(mdef_name, mdef, magr));
		}
	} else  noises(magr, mattk);
	return(mdamagem(magr, mdef, mattk));
}

/* Returns the same values as mdamagem(). */
STATIC_OVL int
gazemm(magr, mdef, mattk)
	register struct monst *magr, *mdef;
	struct attack *mattk;
{
	char buf[BUFSZ];

	if(vis) {
		Sprintf(buf,"%s gazes at", Monnam(magr));
		pline("%s %s...", buf, mon_nam(mdef));
	}

	if (magr->mcan || !magr->mcansee ||
	    (magr->minvis && !perceives(mdef->data)) ||
	    !mdef->mcansee || mdef->msleeping) {
	    if(vis) pline("but nothing happens.");
	    return(MM_MISS);
	}
	/* call mon_reflects 2x, first test, then, if visible, print message */
	if (magr->data == &mons[PM_MEDUSA] && mon_reflects(mdef, (char *)0)) {
	    if (canseemon(mdef))
		(void) mon_reflects(mdef,
				    "The gaze is reflected away by %s %s.");
	    if (mdef->mcansee) {
		if (mon_reflects(magr, (char *)0)) {
		    if (canseemon(magr))
			(void) mon_reflects(magr,
					"The gaze is reflected away by %s %s.");
		    return (MM_MISS);
		}
		if (mdef->minvis && !perceives(magr->data)) {
		    if (canseemon(magr)) {
			pline("%s doesn't seem to notice that %s gaze was reflected.",
			      Monnam(magr), mhis(magr));
		    }
		    return (MM_MISS);
		}
		if (canseemon(magr))
		    pline("%s is turned to stone!", Monnam(magr));
		monstone(magr);
		if (magr->mhp > 0) return (MM_MISS);
		return (MM_AGR_DIED);
	    }
	}

	return(mdamagem(magr, mdef, mattk));
}

/* Returns the same values as mattackm(). */
STATIC_OVL int
gulpmm(magr, mdef, mattk)
	register struct monst *magr, *mdef;
	register struct	attack *mattk;
{
	xchar	ax, ay, dx, dy;
	int	status;
	char buf[BUFSZ];
	struct obj *obj;

	if (mdef->data->msize >= MZ_HUGE) return MM_MISS;

	if (vis) {
		Sprintf(buf,"%s swallows", Monnam(magr));
		pline("%s %s.", buf, mon_nam(mdef));
	}
	for (obj = mdef->minvent; obj; obj = obj->nobj)
	    (void) snuff_lit(obj);

	/*
	 *  All of this maniuplation is needed to keep the display correct.
	 *  There is a flush at the next pline().
	 */
	ax = magr->mx;
	ay = magr->my;
	dx = mdef->mx;
	dy = mdef->my;
	/*
	 *  Leave the defender in the monster chain at it's current position,
	 *  but don't leave it on the screen.  Move the agressor to the def-
	 *  ender's position.
	 */
	remove_monster(ax, ay);
	place_monster(magr, dx, dy);
	newsym(ax,ay);			/* erase old position */
	newsym(dx,dy);			/* update new position */

	status = mdamagem(magr, mdef, mattk);

	if ((status & MM_AGR_DIED) && (status & MM_DEF_DIED)) {
	    ;					/* both died -- do nothing  */
	}
	else if (status & MM_DEF_DIED) {	/* defender died */
	    /*
	     *  Note:  remove_monster() was called in relmon(), wiping out
	     *  magr from level.monsters[mdef->mx][mdef->my].  We need to
	     *  put it back and display it.	-kd
	     */
	    place_monster(magr, dx, dy);
	    newsym(dx, dy);
	}
	else if (status & MM_AGR_DIED) {	/* agressor died */
	    place_monster(mdef, dx, dy);
	    newsym(dx, dy);
	}
	else {					/* both alive, put them back */
	    if (cansee(dx, dy))
		pline("%s is regurgitated!", Monnam(mdef));

	    place_monster(magr, ax, ay);
	    place_monster(mdef, dx, dy);
	    newsym(ax, ay);
	    newsym(dx, dy);
	}

	return status;
}

STATIC_OVL int
explmm(magr, mdef, mattk)
	register struct monst *magr, *mdef;
	register struct	attack *mattk;
{
	int result;

	if (magr->mcan)
	    return MM_MISS;

	if(cansee(magr->mx, magr->my))
		pline("%s explodes!", Monnam(magr));
	else	noises(magr, mattk);

	remove_monster(magr->mx, magr->my);     /* MAR */
	result = mdamagem(magr, mdef, mattk);
	place_monster(magr,magr->mx, magr->my); /* MAR */

	/* Kill off agressor if it didn't die. */
	if (!(result & MM_AGR_DIED)) {
	    mondead(magr);
	    if (magr->mhp > 0) return result;	/* life saved */
	    result |= MM_AGR_DIED;
	}
	/* KMH -- Player gets blame for flame/freezing sphere */
	if (magr->isspell && !(result & MM_DEF_DIED))
		setmangry(mdef);
	/* give this one even if it was visible, except for spell creatures */
	if (magr->mtame && !magr->isspell)
	    You(brief_feeling, "melancholy");

	return result;
}

/*
 *  See comment at top of mattackm(), for return values.
 */
STATIC_OVL int
mdamagem(magr, mdef, mattk)
	register struct monst	*magr, *mdef;
	register struct attack	*mattk;
{
	struct obj *obj;
	char buf[BUFSZ];
	struct permonst *pa = magr->data, *pd = mdef->data;
	int armpro, num, tmp = d((int)mattk->damn, (int)mattk->damd);
	boolean cancelled;
	int canhitmon, objenchant;        
        boolean nohit = FALSE;

	if (touch_petrifies(pd) && !resists_ston(magr)) {
	    long protector = attk_protection((int)mattk->aatyp),
		 wornitems = magr->misc_worn_check;

	    /* wielded weapon gives same protection as gloves here */
	    if (otmp != 0) wornitems |= W_ARMG;

	    if (protector == 0L ||
		  (protector != ~0L && (wornitems & protector) != protector)) {
		if (poly_when_stoned(pa)) {
		    mon_to_stone(magr);
		    return MM_HIT; /* no damage during the polymorph */
		}
		if (vis) pline("%s turns to stone!", Monnam(magr));
		monstone(magr);
		if (magr->mhp > 0) return 0;
		else if (magr->mtame && !vis)
		    You(brief_feeling, "peculiarly sad");
		return MM_AGR_DIED;
	    }
	}

	canhitmon = 0;
	if (need_one(mdef))    canhitmon = 1;
	if (need_two(mdef))    canhitmon = 2;
	if (need_three(mdef))  canhitmon = 3;
	if (need_four(mdef))   canhitmon = 4;

	if (mattk->aatyp == AT_WEAP && otmp) {
	    objenchant = otmp->spe;
	    if (objenchant < 0) objenchant = 0;
	    if (otmp->oartifact) {
		if (otmp->spe < 2) objenchant += 1;
		else objenchant = 2;
	    }
#ifdef LIGHTSABERS
	    if (is_lightsaber(otmp)) objenchant = 4;
#endif
	} else objenchant = 0;

	/* a monster that needs a +1 weapon to hit it hits as a +1 weapon... */
	if (need_one(magr))    objenchant = 1;
	if (need_two(magr))    objenchant = 2;
	if (need_three(magr))  objenchant = 3;
	if (need_four(magr))   objenchant = 4;
	/* overridden by specific flags */
	if (hit_as_one(magr))    objenchant = 1;
	if (hit_as_two(magr))    objenchant = 2;
	if (hit_as_three(magr))  objenchant = 3;
	if (hit_as_four(magr))   objenchant = 4;

	if (objenchant < canhitmon) nohit = TRUE;

	/* cancellation factor is the same as when attacking the hero */
	armpro = magic_negation(mdef);
	cancelled = magr->mcan || !((rn2(3) >= armpro) || !rn2(50));

	switch(mattk->adtyp) {
	    case AD_DGST:
		if (nohit) nohit = FALSE;                
		/* eating a Rider or its corpse is fatal */
		if (is_rider(mdef->data)) {
		    if (vis)
			pline("%s %s!", Monnam(magr),
			      mdef->data == &mons[PM_FAMINE] ?
				"belches feebly, shrivels up and dies" :
			      mdef->data == &mons[PM_PESTILENCE] ?
				"coughs spasmodically and collapses" :
				"vomits violently and drops dead");
		    mondied(magr);
		    if (magr->mhp > 0) return 0;	/* lifesaved */
		    else if (magr->mtame && !vis)
			You(brief_feeling, "queasy");
		    return MM_AGR_DIED;
		}
		if(flags.verbose && flags.soundok) verbalize("Burrrrp!");
		tmp = mdef->mhp;
		/* Use up amulet of life saving */
		if (!!(obj = mlifesaver(mdef))) m_useup(mdef, obj);

		/* Is a corpse for nutrition possible?  It may kill magr */
		if (!corpse_chance(mdef, magr, TRUE) || magr->mhp < 1)
		    break;

		/* Pets get nutrition from swallowing monster whole.
		 * No nutrition from G_NOCORPSE monster, eg, undead.
		 * DGST monsters don't die from undead corpses
		 */
		num = monsndx(mdef->data);
		if (magr->mtame && !magr->isminion &&
		    !(mvitals[num].mvflags & G_NOCORPSE)) {
		    struct obj *virtualcorpse = mksobj(CORPSE, FALSE, FALSE);
		    int nutrit;

		    virtualcorpse->corpsenm = num;
		    virtualcorpse->owt = weight(virtualcorpse);
		    nutrit = dog_nutrition(magr, virtualcorpse);
		    dealloc_obj(virtualcorpse);

		    /* only 50% nutrition, 25% of normal eating time */
		    if (magr->meating > 1) magr->meating = (magr->meating+3)/4;
		    if (nutrit > 1) nutrit /= 2;
		    EDOG(magr)->hungrytime += nutrit;
		}
		break;
	    case AD_STUN:
		if (magr->mcan) break;
		if (canseemon(mdef))
		    pline("%s %s for a moment.", Monnam(mdef),
			  makeplural(stagger(mdef->data, "stagger")));
		mdef->mstun = 1;
		goto physical;
	    case AD_LEGS:
		if (magr->mcan) {
		    tmp = 0;
		    break;
		}
		goto physical;
	    case AD_WERE:
	    case AD_HEAL:
	    case AD_PHYS:
physical:
		if (mattk->aatyp == AT_WEAP && otmp) {
		    if (otmp->otyp == CORPSE &&
			    touch_petrifies(&mons[otmp->corpsenm]) && nohit)
			nohit = FALSE;
		} else if(nohit) break;                
		if (mattk->aatyp == AT_KICK && thick_skinned(pd)) {
		    tmp = 0;
		} else if(mattk->aatyp == AT_WEAP) {
		    if(otmp) {
			if (otmp->otyp == CORPSE &&
				touch_petrifies(&mons[otmp->corpsenm]))
			    goto do_stone;

			/* WAC -- Real weapon?
			 * Could be stuck with a cursed bow/polearm it wielded
			 */
			if (/* if you strike with a bow... */
			    is_launcher(otmp) ||
			    /* or strike with a missile in your hand... */
			    (is_missile(otmp) || is_ammo(otmp)) ||
#ifdef LIGHTSABERS
			    /* lightsaber that isn't lit ;) */
			    (is_lightsaber(otmp) && !otmp->lamplit) ||
#endif
			    /* WAC -- or using a pole at short range... */
			    (is_pole(otmp))) {
			    /* then do only 1-2 points of damage */
			    if (pd == &mons[PM_SHADE] && otmp->otyp != SILVER_ARROW)
				tmp = 0;
			    else
				tmp = rnd(2);

#if 0 /* Monsters don't wield boomerangs */
		    	    if(otmp->otyp == BOOMERANG /* && !rnl(3) */) {
				pline("As %s hits you, %s breaks into splinters.",
				      mon_nam(mtmp), the(xname(otmp)));
				useup(otmp);
				otmp = (struct obj *) 0;
				possibly_unwield(mtmp);
				if (pd != &mons[PM_SHADE])
				    tmp++;
		    	    }
#endif			
			} else tmp += dmgval(otmp, mdef);

			/* MRKR: Handling damage when hitting with */
			/*       a burning torch */

			if(otmp->otyp == TORCH && otmp->lamplit
			   && !resists_fire(mdef)) {

			  if (!Blind) {
			    static char outbuf[BUFSZ];
			    char *s = Shk_Your(outbuf, otmp);

			    boolean water = (mdef->data ==
					     &mons[PM_WATER_ELEMENTAL]);

			    pline("%s %s %s%s %s%s.", s, xname(otmp),
				  (water ? "vaporize" : "burn"),
				  (otmp->quan > 1L ? "" : "s"),
				  (water ? "part of " : ""), mon_nam(mdef));
			  }

			  burn_faster(otmp, 1);

			  tmp++;
			  if (resists_cold(mdef)) tmp += rnd(3);

			  if (!rn2(2) && burnarmor(mdef)) {
			    if (!rn2(3))
			      (void)destroy_mitem(mdef, POTION_CLASS, AD_FIRE);
			    if (!rn2(3))
			      (void)destroy_mitem(mdef, SCROLL_CLASS, AD_FIRE);
			    if (!rn2(5))
			      (void)destroy_mitem(mdef, SPBOOK_CLASS, AD_FIRE);
			  }
			}

                        /* WAC Weres get seared */
                        if(otmp && objects[otmp->otyp].oc_material == SILVER &&
                          (hates_silver(pd))) {
                                tmp += 8;
                                if (vis) pline("The silver sears %s!", mon_nam(mdef));
                        }
                        /* Stakes do extra dmg agains vamps */
                        if (otmp && otmp->otyp == WOODEN_STAKE && is_vampire(pd)) {
                                if(otmp->oartifact == ART_STAKE_OF_VAN_HELSING) {
                                        if (!rn2(10)) {
                                                if (vis) {
                                                        Strcpy(buf, Monnam(magr));
                                                        pline("%s plunges the stake into the heart of %s.",
                                                                buf, mon_nam(mdef));
                                                        pline("%s's body vaporizes!", Monnam(mdef));
                                                }
                                                mondead(mdef); /* no corpse */
                                                if (mdef->mhp < 0) return (MM_DEF_DIED |
                                                        (grow_up(magr,mdef) ? 0 : MM_AGR_DIED));                                                
                                        } else {
                                                if (vis) {
                                                        Strcpy(buf, Monnam(magr));
                                                        pline("%s drives the stake into %s.",
                                                                buf, mon_nam(mdef));
                                                }
                                                tmp += rnd(6) + 2;
                                        }
                                } else {
                                        if (vis) {
                                                Strcpy(buf, Monnam(magr));
                                                pline("%s drives the stake into %s.",
                                                        buf, mon_nam(mdef));
                                        }
                                        tmp += rnd(6);
                                }
                        }

                        if (otmp && otmp->oartifact) {
			    (void)artifact_hit(magr,mdef, otmp, &tmp, dieroll);
			    if (mdef->mhp <= 0)
				return (MM_DEF_DIED |
					(grow_up(magr,mdef) ? 0 : MM_AGR_DIED));
			}
			if (otmp && tmp)
				mrustm(magr, mdef, otmp);
		    }
		} else if (magr->data == &mons[PM_PURPLE_WORM] &&
			    mdef->data == &mons[PM_SHRIEKER]) {
		    /* hack to enhance mm_aggression(); we don't want purple
		       worm's bite attack to kill a shrieker because then it
		       won't swallow the corpse; but if the target survives,
		       the subsequent engulf attack should accomplish that */
		    if (tmp >= mdef->mhp) tmp = mdef->mhp - 1;
		}
		break;
	    case AD_FIRE:
		if (nohit) break;
		
		if (cancelled) {
		    tmp = 0;
		    break;
		}
		if (vis)
		    pline("%s is %s!", Monnam(mdef),
			  on_fire(mdef->data, mattk));
		if (pd == &mons[PM_STRAW_GOLEM] ||
		    pd == &mons[PM_WAX_GOLEM] ||
		    pd == &mons[PM_PAPER_GOLEM]) {
			if (vis) pline("%s burns completely!", Monnam(mdef));
			mondied(mdef);
			if (mdef->mhp > 0) return 0;
			else if (mdef->mtame && !vis)
			    pline("May %s roast in peace.", mon_nam(mdef));
			return (MM_DEF_DIED | (grow_up(magr,mdef) ?
							0 : MM_AGR_DIED));
		}
		tmp += destroy_mitem(mdef, SCROLL_CLASS, AD_FIRE);
		tmp += destroy_mitem(mdef, SPBOOK_CLASS, AD_FIRE);
		if (resists_fire(mdef)) {
		    if (vis)
			pline_The("fire doesn't seem to burn %s!",
								mon_nam(mdef));
		    shieldeff(mdef->mx, mdef->my);
		    golemeffects(mdef, AD_FIRE, tmp);
		    tmp = 0;
		}
		/* only potions damage resistant players in destroy_item */
		tmp += destroy_mitem(mdef, POTION_CLASS, AD_FIRE);
		break;
	    case AD_COLD:
		if (nohit) break;
		
		if (cancelled) {
		    tmp = 0;
		    break;
		}
		if (vis) pline("%s is covered in frost!", Monnam(mdef));
		if (resists_cold(mdef)) {
		    if (vis)
			pline_The("frost doesn't seem to chill %s!",
								mon_nam(mdef));
		    shieldeff(mdef->mx, mdef->my);
		    golemeffects(mdef, AD_COLD, tmp);
		    tmp = 0;
		}
		tmp += destroy_mitem(mdef, POTION_CLASS, AD_COLD);
		break;
	    case AD_ELEC:
		if (nohit) break;
		
		if (cancelled) {
		    tmp = 0;
		    break;
		}
		if (vis) pline("%s gets zapped!", Monnam(mdef));
		tmp += destroy_mitem(mdef, WAND_CLASS, AD_ELEC);
		if (resists_elec(mdef)) {
		    if (vis) pline_The("zap doesn't shock %s!", mon_nam(mdef));
		    shieldeff(mdef->mx, mdef->my);
		    golemeffects(mdef, AD_ELEC, tmp);
		    tmp = 0;
		}
		/* only rings damage resistant players in destroy_item */
		tmp += destroy_mitem(mdef, RING_CLASS, AD_ELEC);
		break;
	    case AD_ACID:
		if (nohit) break;
		
		if (magr->mcan) {
		    tmp = 0;
		    break;
		}
		if (resists_acid(mdef)) {
		    if (vis)
			pline("%s is covered in acid, but it seems harmless.",
			      Monnam(mdef));
		    tmp = 0;
		} else if (vis) {
		    pline("%s is covered in acid!", Monnam(mdef));
		    pline("It burns %s!", mon_nam(mdef));
		}
		if (!rn2(30)) erode_armor(mdef, TRUE);
		if (!rn2(6)) erode_obj(MON_WEP(mdef), TRUE, TRUE);
		break;
	    case AD_RUST:
		if (magr->mcan) break;
		if (pd == &mons[PM_IRON_GOLEM]) {
			if (vis) pline("%s falls to pieces!", Monnam(mdef));
			mondied(mdef);
			if (mdef->mhp > 0) return 0;
			else if (mdef->mtame && !vis)
			    pline("May %s rust in peace.", mon_nam(mdef));
			return (MM_DEF_DIED | (grow_up(magr,mdef) ?
							0 : MM_AGR_DIED));
		}
		hurtmarmor(mdef, AD_RUST);
		mdef->mstrategy &= ~STRAT_WAITFORU;
		tmp = 0;
		break;
	    case AD_CORR:
		if (magr->mcan) break;
		hurtmarmor(mdef, AD_CORR);
		mdef->mstrategy &= ~STRAT_WAITFORU;
		tmp = 0;
		break;
	    case AD_DCAY:
		if (magr->mcan) break;
		if (pd == &mons[PM_WOOD_GOLEM] ||
		    pd == &mons[PM_LEATHER_GOLEM]) {
			if (vis) pline("%s falls to pieces!", Monnam(mdef));
			mondied(mdef);
			if (mdef->mhp > 0) return 0;
			else if (mdef->mtame && !vis)
			    pline("May %s rot in peace.", mon_nam(mdef));
			return (MM_DEF_DIED | (grow_up(magr,mdef) ?
							0 : MM_AGR_DIED));
		}
		hurtmarmor(mdef, AD_DCAY);
		tmp = 0;
		break;
	    case AD_STON:
		if (magr->mcan) break;
		if (mattk->aatyp == AT_GAZE && mon_reflects(mdef, (char *)0)) {
		    tmp = 0;
		    (void) mon_reflects(mdef, "But it reflects from %s %s!");
		    if (poly_when_stoned(pa)) {
			mon_to_stone(magr);
			break;
		    }
		    if (!resists_ston(magr)) {
			if (vis) pline("%s turns to stone!", Monnam(magr));
			monstone(magr);
			if (magr->mhp > 0) return 0;
			else if (magr->mtame && !vis)
			    You(brief_feeling, "peculiarly sad");
			return MM_AGR_DIED;
		    }
		}
 do_stone:
		/* may die from the acid if it eats a stone-curing corpse */
		if (munstone(mdef, FALSE)) goto post_stone;
		if (poly_when_stoned(pd)) {
			mon_to_stone(mdef);
			tmp = 0;
			break;
		}
		if (!resists_ston(mdef)) {
			if (vis) pline("%s turns to stone!", Monnam(mdef));
			monstone(mdef);
 post_stone:		if (mdef->mhp > 0) return 0;
			else if (mdef->mtame && !vis)
			    You(brief_feeling, "peculiarly sad");
			return (MM_DEF_DIED | (grow_up(magr,mdef) ?
							0 : MM_AGR_DIED));
		}
		tmp = (mattk->adtyp == AD_STON ? 0 : 1);
		break;
	    case AD_TLPT:
		if (!cancelled && tmp < mdef->mhp && !tele_restrict(mdef)) {
		    char mdef_Monnam[BUFSZ];
		    /* save the name before monster teleports, otherwise
		       we'll get "it" in the suddenly disappears message */
		    if (vis) Strcpy(mdef_Monnam, Monnam(mdef));
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		    (void) rloc(mdef, FALSE);
		    if (vis && !canspotmon(mdef)
#ifdef STEED
		    	&& mdef != u.usteed
#endif
		    	)
			pline("%s suddenly disappears!", mdef_Monnam);
		}
		break;
	    case AD_SLEE:
		if (nohit) break;                
		
		if (cancelled) break;
		if (mattk->aatyp == AT_GAZE && mon_reflects(mdef, (char *)0)) {
		    tmp = 0;
		    (void) mon_reflects(mdef, "But it reflects from %s %s!");
		    if (sleep_monst(magr, rnd(10), -1))
			if (vis) pline("%s is put to sleep!", Monnam(magr));
		    break;
		}

		if (!cancelled && !mdef->msleeping &&
			sleep_monst(mdef, rnd(10), -1)) {
		    if (vis) {
			Strcpy(buf, Monnam(mdef));
			pline("%s is put to sleep by %s.", buf, mon_nam(magr));
		    }
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		    slept_monst(mdef);
		}
		break;
	    /* WAC DEATH (gaze) */
	    case AD_DETH:
		if (rn2(16)) {
		    /* No death, but still cause damage */
		    break;
		} 
		if (vis && mattk->aatyp == AT_GAZE) 
		    pline("%s gazes intently!", Monnam(magr));
		if (mattk->aatyp == AT_GAZE && mon_reflects(mdef, (char *)0)) {
		    /* WAC reflected gaze 
		     * Oooh boy...that was a bad move :B 
		     */
		    tmp = 0;
		    if (vis) {
			shieldeff(mdef->mx, mdef->my);
			(void) mon_reflects(mdef, "But it reflects from %s %s!");
		    }
		    if (resists_magm(magr)) {
			if (vis) pline("%s shudders momentarily...", Monnam(magr));
			break;
		    }
		    if (vis) pline("%s dies!", Monnam(magr));
		    mondied(magr);
		    if (magr->mhp > 0) return 0;  /* lifesaved */
		    else if (magr->mtame && !vis)
			You(brief_feeling, "peculiarly sad");
		    return MM_AGR_DIED;
		} else if (is_undead(mdef->data)) {
		    /* Still does normal damage */
		    if (vis) pline("Something didn't work...");
		    break;
		} else if (resists_magm(mdef)) {
		    if (vis) pline("%s shudders momentarily...", Monnam(mdef));
		} else {
		    tmp = mdef->mhp;
		}
		break;
	    case AD_PLYS:
		if (nohit) break;                
		if(!cancelled && mdef->mcanmove) {
		    if (vis) {
			Strcpy(buf, Monnam(mdef));
			pline("%s is frozen by %s.", buf, mon_nam(magr));
		    }
		    mdef->mcanmove = 0;
		    mdef->mfrozen = rnd(10);
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		}
		break;
	    case AD_TCKL:
		if(!cancelled && mdef->mcanmove) {
		    if (vis) {
			Strcpy(buf, Monnam(magr));
			pline("%s mercilessly tickles %s.", buf, mon_nam(mdef));
		    }
		    mdef->mcanmove = 0;
		    mdef->mfrozen = rnd(10);
		    mdef->mstrategy &= ~STRAT_WAITFORU;
  		}
		break;
	    case AD_SLOW:
		if (nohit) break;
		if(!cancelled && vis && mdef->mspeed != MSLOW) {
		    unsigned int oldspeed = mdef->mspeed;

		    mon_adjust_speed(mdef, -1, (struct obj *)0);
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		    if (mdef->mspeed != oldspeed && vis)
			pline("%s slows down.", Monnam(mdef));
		}
		break;
	    case AD_CONF:
		if (nohit) break;
		/* Since confusing another monster doesn't have a real time
		 * limit, setting spec_used would not really be right (though
		 * we still should check for it).
		 */
		if (!magr->mcan && !mdef->mconf && !magr->mspec_used) {
		    if (vis) pline("%s looks confused.", Monnam(mdef));
		    mdef->mconf = 1;
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		}
		break;
	    case AD_DREN:
		if (nohit) break;
	    	if (resists_magm(mdef)) {
		    if (vis) {
			shieldeff(mdef->mx,mdef->my);
			pline("%s is unaffected.", Monnam(mdef));
		    }
	    	} else {
	    	    mon_drain_en(mdef, 
				((mdef->m_lev > 0) ? (rnd(mdef->m_lev)) : 0) + 1);
	    	}	    
	    case AD_BLND:
		if (nohit) break;                
	       
		if (can_blnd(magr, mdef, mattk->aatyp, (struct obj*)0)) {
		    register unsigned rnd_tmp;

		    if (vis && mdef->mcansee)
			pline("%s is blinded.", Monnam(mdef));
		    rnd_tmp = d((int)mattk->damn, (int)mattk->damd);
		    if ((rnd_tmp += mdef->mblinded) > 127) rnd_tmp = 127;
		    mdef->mblinded = rnd_tmp;
		    mdef->mcansee = 0;
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		}
		tmp = 0;
		break;
	    case AD_HALU:
		if (!magr->mcan && haseyes(pd) && mdef->mcansee) {
		    if (vis) pline("%s looks %sconfused.",
				    Monnam(mdef), mdef->mconf ? "more " : "");
		    mdef->mconf = 1;
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		}
		tmp = 0;
		break;
	    case AD_CURS:
		if (nohit) break;
		
		if (!night() && (pa == &mons[PM_GREMLIN])) break;
		if (!magr->mcan && !rn2(10)) {
		    mdef->mcan = 1;	/* cancelled regardless of lifesave */
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		    if (is_were(pd) && pd->mlet != S_HUMAN)
			were_change(mdef);
		    if (pd == &mons[PM_CLAY_GOLEM]) {
			    if (vis) {
				pline("Some writing vanishes from %s head!",
				    s_suffix(mon_nam(mdef)));
				pline("%s is destroyed!", Monnam(mdef));
			    }
			    mondied(mdef);
			    if (mdef->mhp > 0) return 0;
			    else if (mdef->mtame && !vis)
				You(brief_feeling, "strangely sad");
			    return (MM_DEF_DIED | (grow_up(magr,mdef) ?
							0 : MM_AGR_DIED));
		    }
		    if (flags.soundok) {
			    if (!vis) You_hear("laughter.");
			    else pline("%s chuckles.", Monnam(magr));
		    }
		}
		break;
	    case AD_SGLD:
		tmp = 0;
#ifndef GOLDOBJ
		if (magr->mcan || !mdef->mgold) break;
		/* technically incorrect; no check for stealing gold from
		 * between mdef's feet...
		 */
		magr->mgold += mdef->mgold;
		mdef->mgold = 0;
#else
                if (magr->mcan) break;
		/* technically incorrect; no check for stealing gold from
		 * between mdef's feet...
		 */
                {
		    struct obj *gold = findgold(mdef->minvent);
		    if (!gold) break;
                    obj_extract_self(gold);
		    add_to_minv(magr, gold);
                }
#endif
		mdef->mstrategy &= ~STRAT_WAITFORU;
		if (vis) {
		    Strcpy(buf, Monnam(magr));
		    pline("%s steals some gold from %s.", buf, mon_nam(mdef));
		}
		if (!tele_restrict(magr)) {
		    (void) rloc(magr, FALSE);
		    if (vis && !canspotmon(magr))
			pline("%s suddenly disappears!", buf);
		}
		break;
	    case AD_DRLI:
		if (nohit) break;                

		if (!cancelled && magr->mtame && !magr->isminion &&
			is_vampire(pa) && mattk->aatyp == AT_BITE &&
			has_blood(pd))
		    EDOG(magr)->hungrytime += ((int)((mdef->data)->cnutrit / 20) + 1);
		
		if (!cancelled && rn2(2) && !resists_drli(mdef)) {
			tmp = d(2,6);
			if (vis)
			    pline("%s suddenly seems weaker!", Monnam(mdef));
			mdef->mhpmax -= tmp;
			if (mdef->m_lev == 0)
				tmp = mdef->mhp;
			else mdef->m_lev--;
			/* Automatic kill if drained past level 0 */
		}
		break;
#ifdef SEDUCE
	    case AD_SSEX:
#endif
	    case AD_SITM:	/* for now these are the same */
	    case AD_SEDU:
		if (magr->mcan) break;
		/* find an object to steal, non-cursed if magr is tame */
		for (obj = mdef->minvent; obj; obj = obj->nobj)
		    if (!magr->mtame || !obj->cursed)
			break;

		if (obj) {
			char onambuf[BUFSZ], mdefnambuf[BUFSZ];

			/* make a special x_monnam() call that never omits
			   the saddle, and save it for later messages */
			Strcpy(mdefnambuf, x_monnam(mdef, ARTICLE_THE, (char *)0, 0, FALSE));

			otmp = obj;
#ifdef STEED
			if (u.usteed == mdef &&
					otmp == which_armor(mdef, W_SADDLE))
				/* "You can no longer ride <steed>." */
				dismount_steed(DISMOUNT_POLY);
#endif
			obj_extract_self(otmp);
			if (otmp->owornmask) {
				mdef->misc_worn_check &= ~otmp->owornmask;
				if (otmp->owornmask & W_WEP)
				    setmnotwielded(mdef,otmp);
				otmp->owornmask = 0L;
				update_mon_intrinsics(mdef, otmp, FALSE, FALSE);
			}
			/* add_to_minv() might free otmp [if it merges] */
			if (vis)
				Strcpy(onambuf, doname(otmp));
			(void) add_to_minv(magr, otmp);
			if (vis) {
				Strcpy(buf, Monnam(magr));
				pline("%s steals %s from %s!", buf,
				    onambuf, mdefnambuf);
			}
			possibly_unwield(mdef, FALSE);
			mdef->mstrategy &= ~STRAT_WAITFORU;
			mselftouch(mdef, (const char *)0, FALSE);
			if (mdef->mhp <= 0)
				return (MM_DEF_DIED | (grow_up(magr,mdef) ?
							0 : MM_AGR_DIED));
			if (magr->data->mlet == S_NYMPH &&
			    !tele_restrict(magr)) {
			    (void) rloc(magr, FALSE);
			    if (vis && !canspotmon(magr))
				pline("%s suddenly disappears!", buf);
			}
		}
		tmp = 0;
		break;
	    case AD_DRST:
	    case AD_DRDX:
	    case AD_DRCO:
		if (nohit) break;
		
		if (!cancelled && !rn2(8)) {
		    if (vis)
			pline("%s %s was poisoned!", s_suffix(Monnam(magr)),
			      mpoisons_subj(magr, mattk));
		    if (resists_poison(mdef)) {
			if (vis)
			    pline_The("poison doesn't seem to affect %s.",
				mon_nam(mdef));
		    } else {
			if (rn2(10)) tmp += rn1(10,6);
			else {
			    if (vis) pline_The("poison was deadly...");
			    tmp = mdef->mhp;
			}
		    }
		}
		break;
	    case AD_DRIN:
		if (notonhead || !has_head(pd)) {
		    if (vis) pline("%s doesn't seem harmed.", Monnam(mdef));
		    /* Not clear what to do for green slimes */
		    tmp = 0;
		    break;
		}
		if ((mdef->misc_worn_check & W_ARMH) && rn2(8)) {
		    if (vis) {
			Strcpy(buf, s_suffix(Monnam(mdef)));
			pline("%s helmet blocks %s attack to %s head.",
				buf, s_suffix(mon_nam(magr)),
				mhis(mdef));
		    }
		    break;
		}
		if (vis) pline("%s brain is eaten!", s_suffix(Monnam(mdef)));
		if (mindless(pd)) {
		    if (vis) pline("%s doesn't notice.", Monnam(mdef));
		    break;
		}
		tmp += rnd(10); /* fakery, since monsters lack INT scores */
		if (magr->mtame && !magr->isminion) {
		    EDOG(magr)->hungrytime += rnd(60);
		    magr->mconf = 0;
		}
		if (tmp >= mdef->mhp && vis)
		    pline("%s last thought fades away...",
			          s_suffix(Monnam(mdef)));
		break;
	    case AD_SLIM:
		if (cancelled) break;   /* physical damage only */
		if (!rn2(4) && !flaming(mdef->data) &&
				mdef->data != &mons[PM_GREEN_SLIME]) {
		    if (newcham(mdef, &mons[PM_GREEN_SLIME], FALSE, vis)) {
			mdef->oldmonnm = PM_GREEN_SLIME;
			(void) stop_timer(UNPOLY_MON, (genericptr_t) mdef);
		    }
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		    tmp = 0;
		}
		break;
	    case AD_STCK:
		if (cancelled) tmp = 0;
		break;
	    case AD_WRAP: /* monsters cannot grab one another, it's too hard */
		if (magr->mcan) tmp = 0;
		break;
	    case AD_ENCH:
		/* There's no msomearmor() function, so just do damage */
	     /* if (cancelled) break; */
		break;
	    case AD_POLY:
		if (!magr->mcan && tmp < mdef->mhp) {
		    if (resists_magm(mdef)) {
			/* magic resistance protects from polymorph traps, so
			 * make it guard against involuntary polymorph attacks
			 * too... */
			if (vis) shieldeff(mdef->mx, mdef->my);
			break;
		    }
#if 0
		    if (!rn2(25) || !mon_poly(mdef)) {
			if (vis)
			    pline("%s shudders!", Monnam(mdef));
			/* no corpse after system shock */
			tmp = rnd(30);
		    } else 
#endif
		    (void) mon_poly(mdef, FALSE,
			    "%s undergoes a freakish metamorphosis!");
		}
		break;
	    case AD_CALM:	/* KMH -- koala attack */
		/* Certain monsters aren't even made peaceful. */
		if (!mdef->iswiz && mdef->data != &mons[PM_MEDUSA] &&
			!(mdef->data->mflags3 & M3_COVETOUS) &&
			!(mdef->data->geno & G_UNIQ) &&
			(magr->mtame || mdef->mtame)) {
		    if (vis) pline("%s looks calmer.", Monnam(mdef));
		    if (mdef == u.usteed)
			dismount_steed(DISMOUNT_THROWN);
		    mdef->mpeaceful = 1;
		    mdef->mtame = 0;
		    tmp = 0;
		}
		break;
	    default:	tmp = 0;
			break;
	}
	if(!tmp) return(MM_MISS);

	/* STEPHEN WHITE'S NEW CODE */
	if (objenchant < canhitmon && vis) {
			Strcpy(buf, Monnam(magr));
			pline("%s doesn't seem to harm %s.", buf,
								mon_nam(mdef));
		return(MM_HIT);
	}
	/* WAC -- Caveman Primal Roar ability */
	if (magr->mtame != 0 && tech_inuse(T_PRIMAL_ROAR)) {
		tmp *= 2; /* Double Damage! */
	}
	if((mdef->mhp -= tmp) < 1) {
	    if (m_at(mdef->mx, mdef->my) == magr) {  /* see gulpmm() */
		remove_monster(mdef->mx, mdef->my);
		mdef->mhp = 1;	/* otherwise place_monster will complain */
		place_monster(mdef, mdef->mx, mdef->my);
		mdef->mhp = 0;
	    }
	    /* get experience from spell creatures */
	    if (magr->uexp) mon_xkilled(mdef, "", (int)mattk->adtyp);
	    else monkilled(mdef, "", (int)mattk->adtyp);

	    if (mdef->mhp > 0) return 0; /* mdef lifesaved */

	    if (mattk->adtyp == AD_DGST) { 
		/* various checks similar to dog_eat and meatobj.
		 * after monkilled() to provide better message ordering */
		if (mdef->cham != CHAM_ORDINARY) {
		    (void) newcham(magr, (struct permonst *)0, FALSE, TRUE);
		} else if (mdef->data == &mons[PM_GREEN_SLIME]) {
		    (void) newcham(magr, &mons[PM_GREEN_SLIME], FALSE, TRUE);
		} else if (mdef->data == &mons[PM_WRAITH]) {
		    (void) grow_up(magr, (struct monst *)0);
		    /* don't grow up twice */
		    return (MM_DEF_DIED | (magr->mhp > 0 ? 0 : MM_AGR_DIED));
		} else if (mdef->data == &mons[PM_NURSE]) {
		    magr->mhp = magr->mhpmax;
		}
	    }

	    return (MM_DEF_DIED |
		    ((magr->mhp > 0 && grow_up(magr,mdef)) ? 0 : MM_AGR_DIED));
	}
	return(MM_HIT);
}

#endif /* OVLB */


#ifdef OVL0

int
noattacks(ptr)			/* returns 1 if monster doesn't attack */
	struct	permonst *ptr;
{
	int i;

	for(i = 0; i < NATTK; i++)
		if(ptr->mattk[i].aatyp) return(0);

	return(1);
}

/* `mon' is hit by a sleep attack; return 1 if it's affected, 0 otherwise */
int
sleep_monst(mon, amt, how)
struct monst *mon;
int amt, how;
{
	if (resists_sleep(mon) ||
		(how >= 0 && resist(mon, (char)how, 0, NOTELL))) {
	    shieldeff(mon->mx, mon->my);
	} else if (mon->mcanmove) {
	    amt += (int) mon->mfrozen;
	    if (amt > 0) {	/* sleep for N turns */
		mon->mcanmove = 0;
		mon->mfrozen = min(amt, 127);
	    } else {		/* sleep until awakened */
		mon->msleeping = 1;
	    }
	    return 1;
	}
	return 0;
}

/* sleeping grabber releases, engulfer doesn't; don't use for paralysis! */
void
slept_monst(mon)
struct monst *mon;
{
	if ((mon->msleeping || !mon->mcanmove) && mon == u.ustuck &&
		!sticks(youmonst.data) && !u.uswallow) {
	    pline("%s grip relaxes.", s_suffix(Monnam(mon)));
	    unstuck(mon);
	}
}

#endif /* OVL0 */
#ifdef OVLB

STATIC_OVL void
mrustm(magr, mdef, obj)
register struct monst *magr, *mdef;
register struct obj *obj;
{
	boolean is_acid;

	if (!magr || !mdef || !obj) return; /* just in case */

	if (dmgtype(mdef->data, AD_CORR))
	    is_acid = TRUE;
	else if (dmgtype(mdef->data, AD_RUST))
	    is_acid = FALSE;
	else
	    return;

	if (!mdef->mcan &&
	    (is_acid ? is_corrodeable(obj) : is_rustprone(obj)) &&
	    (is_acid ? obj->oeroded2 : obj->oeroded) < MAX_ERODE) {
		if (obj->greased || obj->oerodeproof || (obj->blessed && rn2(3))) {
		    if (cansee(mdef->mx, mdef->my) && flags.verbose)
			pline("%s weapon is not affected.",
			                 s_suffix(Monnam(magr)));
		    if (obj->greased && !rn2(2)) obj->greased = 0;
		} else {
		    if (cansee(mdef->mx, mdef->my)) {
			pline("%s %s%s!", s_suffix(Monnam(magr)),
			    aobjnam(obj, (is_acid ? "corrode" : "rust")),
			    (is_acid ? obj->oeroded2 : obj->oeroded)
				? " further" : "");
		    }
		    if (is_acid) obj->oeroded2++;
		    else obj->oeroded++;
		}
	}
}

STATIC_OVL void
mswingsm(magr, mdef, otemp)
register struct monst *magr, *mdef;
register struct obj *otemp;
{
	char buf[BUFSZ];
	if (!flags.verbose || Blind || !mon_visible(magr)) return;
	Strcpy(buf, mon_nam(mdef));
	pline("%s %s %s %s at %s.", Monnam(magr),
	      (objects[otemp->otyp].oc_dir & PIERCE) ? "thrusts" : "swings",
	      mhis(magr), singular(otemp, xname), buf);
}

/*
 * Passive responses by defenders.  Does not replicate responses already
 * handled above.  Returns same values as mattackm.
 */
STATIC_OVL int
passivemm(magr,mdef,mhit,mdead)
register struct monst *magr, *mdef;
boolean mhit;
int mdead;
{
	register struct permonst *mddat = mdef->data;
	register struct permonst *madat = magr->data;
	char buf[BUFSZ];
	int i, tmp;

	for(i = 0; ; i++) {
	    if(i >= NATTK) return (mdead | mhit); /* no passive attacks */
	    if(mddat->mattk[i].aatyp == AT_NONE /*||
	       mddat->mattk[i].aatyp == AT_BOOM*/) break;
	}
	if (mddat->mattk[i].damn)
	    tmp = d((int)mddat->mattk[i].damn,
				    (int)mddat->mattk[i].damd);
	else if(mddat->mattk[i].damd)
	    tmp = d((int)mddat->mlevel+1, (int)mddat->mattk[i].damd);
	else
	    tmp = 0;

	/* These affect the enemy even if defender killed */
	switch(mddat->mattk[i].adtyp) {
	    case AD_ACID:
		if (mhit && !rn2(2)) {
		    Strcpy(buf, Monnam(magr));
		    if(canseemon(magr))
			pline("%s is splashed by %s acid!",
			      buf, s_suffix(mon_nam(mdef)));
		    if (resists_acid(magr)) {
			if(canseemon(magr))
			    pline("%s is not affected.", Monnam(magr));
			tmp = 0;
		    }
		} else tmp = 0;
		goto assess_dmg;
		case AD_MAGM:
	    /* wrath of gods for attacking Oracle */
	    if(resists_magm(magr)) {
		if(canseemon(magr)) {
		shieldeff(magr->mx, magr->my);
		pline("A hail of magic missiles narrowly misses %s!",mon_nam(magr));
		}
	    } else {
		if(canseemon(magr))
			pline(magr->data == &mons[PM_WOODCHUCK] ? "ZOT!" : 
			"%s is hit by magic missiles appearing from thin air!",Monnam(magr));
		goto assess_dmg;
	    }
	    break;
	    case AD_ENCH:	/* KMH -- remove enchantment (disenchanter) */
		if (mhit && !mdef->mcan && otmp) {
				drain_item(otmp);
		    /* No message */
		}
		break;
	    default:
		break;
	}
	if (mdead || mdef->mcan) return (mdead|mhit);

	/* These affect the enemy only if defender is still alive */
	if (rn2(3)) switch(mddat->mattk[i].adtyp) {
	    case AD_PLYS: /* Floating eye */
		if (tmp > 127) tmp = 127;
		if (mddat == &mons[PM_FLOATING_EYE]) {
		    if (!rn2(4)) tmp = 127;
		    if (magr->mcansee && haseyes(madat) && mdef->mcansee &&
			(perceives(madat) || !mdef->minvis)) {
			Sprintf(buf, "%s gaze is reflected by %%s %%s.",
				s_suffix(mon_nam(mdef)));
			if (mon_reflects(magr,
					 canseemon(magr) ? buf : (char *)0))
				return(mdead|mhit);
			Strcpy(buf, Monnam(magr));
			if(canseemon(magr))
			    pline("%s is frozen by %s gaze!",
				  buf, s_suffix(mon_nam(mdef)));
			magr->mcanmove = 0;
			magr->mfrozen = tmp;
			return (mdead|mhit);
		    }
		} else { /* gelatinous cube */
		    Strcpy(buf, Monnam(magr));
		    if(canseemon(magr))
			pline("%s is frozen by %s.", buf, mon_nam(mdef));
		    magr->mcanmove = 0;
		    magr->mfrozen = tmp;
		    return (mdead|mhit);
		}
		return 1;
	    case AD_COLD:
		if (resists_cold(magr)) {
		    if (canseemon(magr)) {
			pline("%s is mildly chilly.", Monnam(magr));
			golemeffects(magr, AD_COLD, tmp);
		    }
		    tmp = 0;
		    break;
		}
		if(canseemon(magr))
		    pline("%s is suddenly very cold!", Monnam(magr));
		mdef->mhp += tmp / 2;
		if (mdef->mhpmax < mdef->mhp) mdef->mhpmax = mdef->mhp;
		if (mdef->mhpmax > ((int) (mdef->m_lev+1) * 8))
		    (void)split_mon(mdef, magr);
		break;
	    case AD_STUN:
		if (!magr->mstun) {
		    magr->mstun = 1;
		    if (canseemon(magr))
			pline("%s %s...", Monnam(magr),
			      makeplural(stagger(magr->data, "stagger")));
		}
		tmp = 0;
		break;
	    case AD_FIRE:
		if (resists_fire(magr)) {
		    if (canseemon(magr)) {
			pline("%s is mildly warmed.", Monnam(magr));
			golemeffects(magr, AD_FIRE, tmp);
		    }
		    tmp = 0;
		    break;
		}
		if(canseemon(magr))
		    pline("%s is suddenly very hot!", Monnam(magr));
		break;
	    case AD_ELEC:
		if (resists_elec(magr)) {
		    if (canseemon(magr)) {
			pline("%s is mildly tingled.", Monnam(magr));
			golemeffects(magr, AD_ELEC, tmp);
		    }
		    tmp = 0;
		    break;
		}
		if(canseemon(magr))
		    pline("%s is jolted with electricity!", Monnam(magr));
		break;
	    default: tmp = 0;
		break;
	}
	else tmp = 0;

    assess_dmg:
	if((magr->mhp -= tmp) <= 0) {
		/* get experience from spell creatures */
		if (mdef->uexp) mon_xkilled(magr, "", (int)mddat->mattk[i].adtyp);
		else monkilled(magr, "", (int)mddat->mattk[i].adtyp);

		return (mdead | mhit | MM_AGR_DIED);
	}
	return (mdead | mhit);
}

/* "aggressive defense"; what type of armor prevents specified attack
   from touching its target? */
long
attk_protection(aatyp)
int aatyp;
{
    long w_mask = 0L;

    switch (aatyp) {
    case AT_NONE:
    case AT_SPIT:
    case AT_EXPL:
    case AT_BOOM:
    case AT_GAZE:
    case AT_BREA:
    case AT_MAGC:
	w_mask = ~0L;		/* special case; no defense needed */
	break;
    case AT_CLAW:
    case AT_TUCH:
    case AT_WEAP:
	w_mask = W_ARMG;	/* caller needs to check for weapon */
	break;
    case AT_KICK:
	w_mask = W_ARMF;
	break;
    case AT_BUTT:
	w_mask = W_ARMH;
	break;
    case AT_HUGS:
	w_mask = (W_ARMC|W_ARMG); /* attacker needs both to be protected */
	break;
    case AT_BITE:
    case AT_STNG:
    case AT_ENGL:
    case AT_TENT:
    default:
	w_mask = 0L;		/* no defense available */
	break;
    }
    return w_mask;
}

#endif /* OVLB */

/*mhitm.c*/

