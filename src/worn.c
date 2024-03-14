/*	SCCS Id: @(#)worn.c	3.4	2003/01/08	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

STATIC_DCL void m_lose_armor(struct monst *,struct obj *);
STATIC_DCL void m_dowear_type(struct monst *,long, BOOLEAN_P, BOOLEAN_P);
STATIC_DCL int extra_pref(struct monst *, struct obj *);

const struct worn {
	long w_mask;
	struct obj **w_obj;
} worn[] = {
	{ W_ARM, &uarm },
	{ W_ARMC, &uarmc },
	{ W_ARMH, &uarmh },
	{ W_ARMS, &uarms },
	{ W_ARMG, &uarmg },
	{ W_ARMF, &uarmf },
	{ W_ARMU, &uarmu },
	{ W_RINGL, &uleft },
	{ W_RINGR, &uright },
	{ W_WEP, &uwep },
	{ W_SWAPWEP, &uswapwep },
	{ W_QUIVER, &uquiver },
	{ W_AMUL, &uamul },
	{ W_IMPLANT, &uimplant },
	{ W_TOOL, &ublindf },
	{ W_BALL, &uball },
	{ W_CHAIN, &uchain },
	{ W_SADDLE, &usaddle },
	{ 0, 0 }
};

/* this only allows for one blocking property per item */
/* KMH -- Added Hobbits & stealth */
/* This only allows for one blocking item per property */
#define w_blocks(o,m) \
		((o->otyp == MUMMY_WRAPPING && ((m) & W_ARMC)) ? INVIS : \
		 (o->otyp == CORNUTHAUM && ((m) & W_ARMH) && \
			!Role_if(PM_WIZARD)) ? CLAIRVOYANT : \
		 (is_boots(o) && o->otyp != ELVEN_BOOTS && ((m) & W_ARMF) && \
		 	Role_if(PM_HOBBIT)) ? STEALTH : 0)
		/* note: monsters don't have clairvoyance, so your role
		   has no significant effect on their use of w_blocks() */

/* KMH, intrinsic patch.
 * Updated to use the extrinsic and blocked fields.
 */
void
setworn(obj, mask)
register struct obj *obj;
long mask;
{
	register const struct worn *wp;
	register struct obj *oobj;
	register int p, q;

	if ((mask & (W_ARM|I_SPECIAL)) == (W_ARM|I_SPECIAL)) {
	    /* restoring saved game; no properties are conferred via skin */
	    uskin = obj;
	 /* assert( !uarm ); */
	} else {
	    for(wp = worn; wp->w_mask; wp++) if(wp->w_mask & mask) {
		oobj = *(wp->w_obj);
		if(oobj && !(oobj->owornmask & wp->w_mask))
			pline("Setworn: mask = %ld.", wp->w_mask);
		if(oobj) {
		    if (u.twoweap && (oobj->owornmask & (W_WEP|W_SWAPWEP)) && uwep && bimanual(uwep) ) {
			if (uswapwep)
			    unwield(uswapwep, FALSE);
			u.twoweap = 0;
		    }
		    oobj->owornmask &= ~wp->w_mask;
		    if (wp->w_mask & ~(W_SWAPWEP|W_QUIVER)) {
			/* leave as "x = x <op> y", here and below, for broken
			 * compilers */
			p = objects[oobj->otyp].oc_oprop;
			u.uprops[p].extrinsic =
					u.uprops[p].extrinsic & ~wp->w_mask;

			q = oobj->enchantment;
			if (q) {
				u.uprops[oobj->enchantment].extrinsic = u.uprops[oobj->enchantment].extrinsic & ~wp->w_mask;
			}

			if ((p = w_blocks(oobj,mask)) != 0)
			    u.uprops[p].blocked &= ~wp->w_mask;
			if (oobj->oartifact)
			    set_artifact_intrinsic(oobj, 0, mask);
		    }
		}
		*(wp->w_obj) = obj;
		if(obj) {
		    obj->owornmask |= wp->w_mask;
		    /* Prevent getting/blocking intrinsics from wielding
		     * potions, through the quiver, etc.
		     * Allow weapon-tools, too.
		     * wp_mask should be same as mask at this point.
		     */
		    if (wp->w_mask & ~(W_SWAPWEP|W_QUIVER)) {
			if (obj->oclass == WEAPON_CLASS || is_weptool(obj) ||
					    mask != W_WEP) {
			    p = objects[obj->otyp].oc_oprop;
			    u.uprops[p].extrinsic =
					u.uprops[p].extrinsic | wp->w_mask;

				q = obj->enchantment;
				if (q) {
					u.uprops[obj->enchantment].extrinsic = u.uprops[obj->enchantment].extrinsic | wp->w_mask;
				}

			    if ((p = w_blocks(obj, mask)) != 0)
				u.uprops[p].blocked |= wp->w_mask;
			}
			if (obj->oartifact)
			    set_artifact_intrinsic(obj, 1, mask);
		    }
		}
	    }
	}
	update_inventory();
}

/* called e.g. when obj is destroyed */
/* KMH, intrinsic patch.
 * Updated to use the extrinsic and blocked fields.
 */
void
setnotworn(obj)
register struct obj *obj;
{
	register const struct worn *wp;
	register int p, q;

	if (!obj) return;
	if (obj == uwep || obj == uswapwep) {
	    if (uswapwep)
		unwield(uswapwep, FALSE);
	    if (uwep && bimanual(uwep)) u.twoweap = 0;
	}
	for(wp = worn; wp->w_mask; wp++)
	    if(obj == *(wp->w_obj)) {
		*(wp->w_obj) = 0;
		p = objects[obj->otyp].oc_oprop;
		u.uprops[p].extrinsic = u.uprops[p].extrinsic & ~wp->w_mask;

		q = obj->enchantment;
		if (q) {
			u.uprops[obj->enchantment].extrinsic = u.uprops[obj->enchantment].extrinsic & ~wp->w_mask;
		}

		obj->owornmask &= ~wp->w_mask;
		if (obj->oartifact)
		    set_artifact_intrinsic(obj, 0, wp->w_mask);
		if ((p = w_blocks(obj,wp->w_mask)) != 0)
		    u.uprops[p].blocked &= ~wp->w_mask;
	    }
	update_inventory();
}

void
mon_set_minvis(mon)
struct monst *mon;
{
	mon->perminvis = 1;
	if (!mon->invis_blkd) {
	    mon->minvis = 1;
	    if (!rn2(100)) mon->minvisreal = 1;
	    newsym(mon->mx, mon->my);		/* make it disappear */
	    if (mon->wormno) see_wsegs(mon);	/* and any tail too */
	}
}

void
mon_adjust_speed(mon, adjust, obj)
struct monst *mon;
int adjust;	/* positive => increase speed, negative => decrease */
struct obj *obj;	/* item to make known if effect can be seen */
{
    struct obj *otmp;
    boolean give_msg = !in_mklev, petrify = FALSE;
    unsigned int oldspeed = mon->mspeed;

    if (mercedesride(ART_SENATOR_S_SPEED, mon) && adjust < 0) {
	pline("%s doesn't slow down!", Monnam(mon));
	return;
    }

    switch (adjust) {
     case  2:
	mon->permspeed = MFAST;
	give_msg = FALSE;	/* special case monster creation */
	break;
     case  1:
	if (mon->permspeed == MSLOW) mon->permspeed = 0;
	else mon->permspeed = MFAST;
	break;
     case  0:			/* just check for worn speed boots */
	break;
     case -1:
	if (mon->permspeed == MFAST) mon->permspeed = 0;
	else mon->permspeed = MSLOW;
	break;
     case -2:
	mon->permspeed = MSLOW;
	give_msg = FALSE;	/* (not currently used) */
	break;
     case -3:			/* petrification */
	/* take away intrinsic speed but don't reduce normal speed */
	if (mon->permspeed == MFAST) mon->permspeed = 0;
	petrify = TRUE;
	break;
    }

    for (otmp = mon->minvent; otmp; otmp = otmp->nobj)
	if (otmp->owornmask && objects[otmp->otyp].oc_oprop == FAST)
	    break;
    if (otmp)		/* speed boots */
	mon->mspeed = MFAST;
    else
	mon->mspeed = mon->permspeed;

    if (give_msg && (mon->mspeed != oldspeed || petrify) && canseemon(mon)) {
	/* fast to slow (skipping intermediate state) or vice versa */
	const char *howmuch = (mon->mspeed + oldspeed == MFAST + MSLOW) ?
				"much " : "";

	if (petrify) {
	    /* mimic the player's petrification countdown; "slowing down"
	       even if fast movement rate retained via worn speed boots */
	    if (flags.verbose) pline("%s is slowing down.", Monnam(mon));
	} else if (adjust > 0 || mon->mspeed == MFAST)
	    pline("%s is suddenly moving %sfaster.", Monnam(mon), howmuch);
	else
	    pline("%s seems to be moving %sslower.", Monnam(mon), howmuch);

	/* might discover an object if we see the speed change happen, but
	   avoid making possibly forgotten book known when casting its spell */
	if (obj != 0 && obj->dknown &&
		objects[obj->otyp].oc_class != SPBOOK_CLASS)
	    makeknown(obj->otyp);
    }
}

/* armor put on or taken off; might be magical variety */
void
update_mon_intrinsics(mon, obj, on, silently)
struct monst *mon;
struct obj *obj;
boolean on, silently;
{
    int unseen;
    unsigned long mask;
    struct obj *otmp;
    int which = (int) objects[obj->otyp].oc_oprop;

    unseen = !canseemon(mon);
    if (!which) goto maybe_blocks;

    if (on) {
	switch (which) {
	 case INVIS:
	    mon->minvis = !mon->invis_blkd;
	    break;
	 case FAST:
	  {
	    boolean save_in_mklev = in_mklev;
	    if (silently) in_mklev = TRUE;
	    mon_adjust_speed(mon, 0, obj);
	    in_mklev = save_in_mklev;
	    break;
	  }
	/* properties handled elsewhere */
	 case ANTIMAGIC:
	 case REFLECTING:
	    break;
	/* properties which have no effect for monsters */
	 case CLAIRVOYANT:
	 case STEALTH:
	 case TELEPAT:
	    break;
	/* properties which should have an effect but aren't implemented */
	 case LEVITATION:
	 case WWALKING:
	    break;
	/* properties which maybe should have an effect but don't */
	 case DISPLACED:
	 case FUMBLING:
	 case JUMPING:
	 case PROTECTION:
	    break;
	 case DRAIN_RES:
	    mon->mintrinsics |= MR_DRAIN;
	    break;
	 default:
	    if (which <= 8) {	/* 1 thru 8 correspond to MR_xxx mask values */
		/* FIRE,COLD,SLEEP,DISINT,SHOCK,POISON,ACID,STONE */
		mask = (uchar) (1 << (which - 1));
		mon->mintrinsics |= (unsigned short) mask;
	    }
	    break;
	}
    } else {	    /* off */
	switch (which) {
	 case INVIS:
	    mon->minvis = mon->perminvis;
	    break;
	 case FAST:
	  {
	    boolean save_in_mklev = in_mklev;
	    if (silently) in_mklev = TRUE;
	    mon_adjust_speed(mon, 0, obj);
	    in_mklev = save_in_mklev;
	    break;
	  }
	 case DRAIN_RES:
	    mask = MR_DRAIN;
	    goto maybe_loose;
	 case FIRE_RES:
	 case COLD_RES:
	 case SLEEP_RES:
	 case DISINT_RES:
	 case SHOCK_RES:
	 case POISON_RES:
	 case ACID_RES:
	 case STONE_RES:
	    mask = (uchar) (1 << (which - 1));
 maybe_loose:
	    /* If the monster doesn't have this resistance intrinsically,
	       check whether any other worn item confers it.  Note that
	       we don't currently check for anything conferred via simply
	       carrying an object. */
	    if (!(mon->data->mresists & mask)) {
		for (otmp = mon->minvent; otmp; otmp = otmp->nobj)
		    if (otmp->owornmask &&
			    (int) objects[otmp->otyp].oc_oprop == which)
			break;
		if (!otmp)
		    mon->mintrinsics &= ~mask;
	    }
	    break;
	 default:
	    break;
	}
    }

 maybe_blocks:
    /* obj->owornmask has been cleared by this point, so we can't use it.
       However, since monsters don't wield armor, we don't have to guard
       against that and can get away with a blanket worn-mask value. */
    switch (w_blocks(obj,~0L)) {
     case INVIS:
	mon->invis_blkd = on ? 1 : 0;
	mon->minvis = on ? 0 : mon->perminvis;
	break;
     default:
	break;
    }

	if (!on && mon == u.usteed && obj->otyp == LEATHER_SADDLE)
	    dismount_steed(DISMOUNT_FELL);
	if (!on && mon == u.usteed && obj->otyp == INKA_SADDLE)
	    dismount_steed(DISMOUNT_FELL);
	if (!on && mon == u.usteed && obj->otyp == TANK_SADDLE)
	    dismount_steed(DISMOUNT_FELL);
	if (!on && mon == u.usteed && obj->otyp == BARDING)
	    dismount_steed(DISMOUNT_FELL);
	if (!on && mon == u.usteed && obj->otyp == MESH_SADDLE)
	    dismount_steed(DISMOUNT_FELL);
	if (!on && mon == u.usteed && obj->otyp == SEATBELT_SADDLE)
	    dismount_steed(DISMOUNT_FELL);

    /* if couldn't see it but now can, or vice versa, update display */
    if (!silently && (unseen ^ !canseemon(mon)))
	newsym(mon->mx, mon->my);
}

int
find_mac(mon)
register struct monst *mon;
{
	register struct obj *obj;
	int base = mon->data->ac;
	long mwflags = mon->misc_worn_check;

	if (mon->egotype_armorer) base -= 5;
	if (mon->egotype_tank) base -= 10;

	/* soresu monster lightsaber form */
	if (which_armor(mon, W_ARM)) {
		struct obj *monarmor = which_armor(mon, W_ARM);
		if (monarmor && (monarmor->otyp >= ROBE && monarmor->otyp <= ROBE_OF_WEAKNESS) ) {
			struct obj *monweapon;
			if (MON_WEP(mon)) {
				monweapon = MON_WEP(mon);
				if (monweapon) {
					if (is_lightsaber(monweapon) && monweapon->lamplit) base -= 10;
					if (mon->data->geno & G_UNIQ) base -= 10;
				}
			}			
		}
	}

	if (fordride(TANK_SADDLE, mon)) base -= 5;
	if (fordride(BARDING, mon)) base -= 3;

	if (mercedesride(ART_DEATH_SQUAD_MOBILE, mon)) base -= 5;

	if (FemtrapActiveMarleen && humanoid(mon->data) && is_female(mon->data) ) {
		if (SuperFemtrapMarleen) base -= (10 + mon->m_lev);
		else base -= mon->m_lev;
	}

	for (obj = mon->minvent; obj; obj = obj->nobj) {
	    if (obj->owornmask & mwflags)
		base -= ARM_BONUS(obj);
		/* since ARM_BONUS is positive, subtracting it increases AC */
	}

	if (is_wagon(mon->mx, mon->my)) base -= 10;

	if (uarm && uarm->oartifact == ART_INTERNAL_VIEW && base < 10) {
		int acamount = base - 10; /* example: base = -10, i.e. 20 points of AC */
		if (acamount < 0) acamount *= -1; /* acamount would now be 20 */

		acamount *= 7;
		acamount /= 10;
		/* acamount is 14 now */

		base = 10 - acamount; /* -4 instead of -10 */
	}

	if (!mon->mtame && !mon->mpeaceful && uwep && uwep->oartifact == ART_ROARBEAR && base < 10) {
		int acamount = base - 10;
		if (acamount < 0) acamount *= -1;

		acamount /= 2;

		base = 10 - acamount;

	}

	if (uarm && uarm->oartifact == ART_ETH_ITH && base < 10) {
		int acamount = base - 10;
		if (acamount < 0) acamount *= -1;

		acamount /= 2;

		base = 10 - acamount;

	}

	/* Monsters with too good AC should not be completely unhittable. --Amy */
	if (!rn2(20)) {
		int absoluteac = (-(base - 10));
		if (absoluteac < 0) absoluteac *= -1;
		if ((absoluteac / 2) > 0) base += rnd(absoluteac / 2);
		else base += 1;
		if (base > 10) base = 10;
	}

	return base;
}

/* weapons are handled separately; rings and eyewear aren't used by monsters */

/* Wear the best object of each type that the monster has.  During creation,
 * the monster can put everything on at once; otherwise, wearing takes time.
 * This doesn't affect monster searching for objects--a monster may very well
 * search for objects it would not want to wear, because we don't want to
 * check which_armor() each round.
 *
 * We'll let monsters put on shirts and/or suits under worn cloaks, but
 * not shirts under worn suits.  This is somewhat arbitrary, but it's
 * too tedious to have them remove and later replace outer garments,
 * and preventing suits under cloaks makes it a little bit too easy for
 * players to influence what gets worn.  Putting on a shirt underneath
 * already worn body armor is too obviously buggy...
 */
void
m_dowear(mon, creation)
register struct monst *mon;
boolean creation;
{
#define RACE_EXCEPTION TRUE
	/* Note the restrictions here are the same as in dowear in do_wear.c
	 * except for the additional restriction on intelligence.  (Players
	 * are always intelligent, even if polymorphed).
	 */
	/*if (verysmall(mon->data) || nohands(mon->data) || is_animal(mon->data))
		return;*/
	/* give mummies a chance to wear their wrappings
	 * and let skeletons wear their initial armor */
	/* well screw it, just let mindless creatures wear everything! --Amy */
	/*if (mindless(mon->data) && (!creation ||
	    (mon->data->mlet != S_MUMMY && mon->data != &mons[PM_SKELETON])))
		return;*/

	m_dowear_type(mon, W_AMUL, creation, FALSE);

	  if (!creation && is_animal(mon->data)) return;

	/* can't put on shirt if already wearing suit */
	if ( (!cantweararm(mon->data) || (creation && !verysmall(mon->data) && (mon->data->msize <= MZ_LARGE) ) ) || (mon->misc_worn_check & W_ARM) || mon->data->msize == MZ_SMALL)
	    m_dowear_type(mon, W_ARMU, creation, FALSE);
	/* treating small as a special case allows
	   hobbits, gnomes, and kobolds to wear cloaks */
	if ( (!cantweararm(mon->data) || (creation && !verysmall(mon->data) && (mon->data->msize <= MZ_LARGE) ) ) || mon->data->msize == MZ_SMALL)
	    m_dowear_type(mon, W_ARMC, creation, FALSE);
	m_dowear_type(mon, W_ARMH, creation, FALSE);

	/* shield needs to be checked below: certain types can be used with a two-hander --Amy */
	m_dowear_type(mon, W_ARMS, creation, FALSE);
	m_dowear_type(mon, W_ARMG, creation, FALSE);
	if ((!slithy(mon->data) && mon->data->mlet != S_CENTAUR) || !issoviet)
	    m_dowear_type(mon, W_ARMF, creation, FALSE);
	if (!cantweararm(mon->data) || (creation && !verysmall(mon->data) && (mon->data->msize <= MZ_LARGE) ) )
	    m_dowear_type(mon, W_ARM, creation, FALSE);
	else
	    m_dowear_type(mon, W_ARM, creation, RACE_EXCEPTION);
}

STATIC_OVL void
m_dowear_type(mon, flag, creation, racialexception)
struct monst *mon;
long flag;
boolean creation;
boolean racialexception;
{
	struct obj *old, *best, *obj;
	int m_delay = 0;
	int unseen = !canseemon(mon);
	char nambuf[BUFSZ];

	if (mon->mfrozen) return; /* probably putting previous item on */

	/* Get a copy of monster's name before altering its visibility */
	strcpy(nambuf, See_invisible ? Monnam(mon) : mon_nam(mon));

	old = which_armor(mon, flag);

	/* hostile monsters may occasionally cheat and simply shake off a cursed item --Amy
	 * In Soviet Russia, curses are absolute. No one may be able to ever take off a cursed item, not even monsters who
	 * are, by definition, completely unable to remove curses and therefore have zero chance to ever switch to
	 * something else. Because it's such an ingenious strategy to carry around a -10 leather armor and give it to
	 * a monster who will then forever have shitty AC. */
	if (old && old->cursed && (issoviet || mon->mpeaceful || mon->mtame || rn2(5)) ) return;
	if (old && flag == W_AMUL) return; /* no such thing as better amulets */
	best = old;

	for(obj = mon->minvent; obj; obj = obj->nobj) {
	    switch(flag) {
		case W_AMUL:
		    if (obj->oclass != AMULET_CLASS ||
			    (obj->otyp != AMULET_OF_MONSTER_SAVING && obj->otyp != AMULET_OF_DATA_STORAGE &&
				obj->otyp != AMULET_OF_REFLECTION))
			continue;
		    best = obj;
		    goto outer_break; /* no such thing as better amulets */
		case W_ARMU:
		    if (!is_shirt(obj)) continue;
		    break;
		case W_RING:
		    if (obj->otyp != RIN_TIMELY_BACKUP) continue;
		    best = obj;
		    goto outer_break; /* no such thing as better rings */
		case W_RINGL:
		    if (obj->otyp != RIN_TIMELY_BACKUP) continue;
		    best = obj;
		    goto outer_break; /* no such thing as better rings */
		case W_RINGR:
		    if (obj->otyp != RIN_TIMELY_BACKUP) continue;
		    best = obj;
		    goto outer_break; /* no such thing as better rings */
		case W_ARMC:
		    if (!is_cloak(obj)) continue;
		    break;
		case W_ARMH:
		    if (!is_helmet(obj)) continue;
		    /* (flimsy exception matches polyself handling) */
		    if (!creation && has_horns(mon->data) && !is_flimsy(obj)) continue;
		    break;
		case W_ARMS:
		    if (MON_WEP(mon) && bimanual(MON_WEP(mon)) && obj->otyp != GRIM_SHIELD) continue;
		    if (!is_shield(obj)) continue;
		    break;
		case W_ARMG:
		    if (!is_gloves(obj)) continue;
		    break;
		case W_ARMF:
		    if (!is_boots(obj)) continue;
		    break;
		case W_ARM:
		    if (!is_suit(obj)) continue;
		    if (!creation && racialexception && (racial_exception(mon, obj) < 1)) continue;
		    break;
	    }
	    if (obj->owornmask) continue;
	    /* I'd like to define a VISIBLE_ARM_BONUS which doesn't assume the
	     * monster knows obj->spe, but if I did that, a monster would keep
	     * switching forever between two -2 caps since when it took off one
	     * it would forget spe and once again think the object is better
	     * than what it already has.
	     */

	    if (FemtrapActiveKatia && mon->data->msound == MS_FART_LOUD && itemhasappearance(obj, APP_BLOCK_HEELED_BOOTS)) best = obj;

	    if (best && (ARM_BONUS(best) + extra_pref(mon,best) >= ARM_BONUS(obj) + extra_pref(mon,obj)))
		continue;
	    if (FemtrapActiveKatia && mon->data->msound == MS_FART_LOUD && best && itemhasappearance(best, APP_BLOCK_HEELED_BOOTS)) continue;

	    best = obj;
	}
outer_break:
	if (!best || best == old) return;

	/* if wearing a cloak, account for the time spent removing
	   and re-wearing it when putting on a suit or shirt */
	if ((flag == W_ARM
	  || flag == W_ARMU
			  ) && (mon->misc_worn_check & W_ARMC))
	    m_delay += 2;
	/* when upgrading a piece of armor, account for time spent
	   taking off current one */
	if (old)
	    m_delay += objects[old->otyp].oc_delay;

	if (old) /* do this first to avoid "(being worn)" */
	    old->owornmask = 0L;
	if (!creation) {
	    if (canseemon(mon)) {
		char buf[BUFSZ];

		if (old)
		    sprintf(buf, " removes %s and", distant_name(old, doname));
		else
		    buf[0] = '\0';
		pline("%s%s puts on %s.", Monnam(mon),
		      buf, distant_name(best,doname));
	    } /* can see it */
	    m_delay += objects[best->otyp].oc_delay;
	    mon->mfrozen = m_delay;
	    if (mon->mfrozen) mon->mcanmove = 0;
	}
	if (old)
	    update_mon_intrinsics(mon, old, FALSE, creation);
	mon->misc_worn_check |= flag;
	best->owornmask |= flag;
	update_mon_intrinsics(mon, best, TRUE, creation);
	/* if couldn't see it but now can, or vice versa, */
	if (!creation && (unseen ^ !canseemon(mon))) {
		if ((mon->minvis && !See_invisible) || mon->minvisreal) {
			pline("Suddenly you cannot see %s.", nambuf);
			makeknown(best->otyp);
		} /* else if (!mon->minvis) pline("%s suddenly appears!", Amonnam(mon)); */
	}
}
#undef RACE_EXCEPTION

struct obj *
which_armor(mon, flag)
struct monst *mon;
long flag;
{
	register struct obj *obj;

	for(obj = mon->minvent; obj; obj = obj->nobj)
		if (obj->owornmask & flag) return obj;
	return((struct obj *)0);
}

/* remove an item of armor and then drop it */
STATIC_OVL void
m_lose_armor(mon, obj)
struct monst *mon;
struct obj *obj;
{
	mon->misc_worn_check &= ~obj->owornmask;
	if (obj->owornmask)
	    update_mon_intrinsics(mon, obj, FALSE, FALSE);
	obj->owornmask = 0L;

	obj_extract_self(obj);
	place_object(obj, mon->mx, mon->my);
	/* call stackobj() if we ever drop anything that can merge */
	newsym(mon->mx, mon->my);
}

/* all objects with their bypass bit set should now be reset to normal */
void
clear_bypasses()
{
	struct obj *otmp, *nobj;
	struct monst *mtmp;

	for (otmp = fobj; otmp; otmp = nobj) {
	    nobj = otmp->nobj;
	    if (otmp->bypass) {
		otmp->bypass = 0;
		/* bypass will have inhibited any stacking, but since it's
		   used for polymorph handling, the objects here probably
		   have been transformed and won't be stacked in the usual
		   manner afterwards; so don't bother with this */
#if 0
		if (objects[otmp->otyp].oc_merge) {
		    xchar ox, oy;

		    (void) get_obj_location(otmp, &ox, &oy, 0);
		    stack_object(otmp);
		    newsym(ox, oy);
		}
#endif	/*0*/
	    }
	}
	/* invent and mydogs chains shouldn't matter here */
	for (otmp = migrating_objs; otmp; otmp = otmp->nobj)
	    otmp->bypass = 0;
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
		otmp->bypass = 0;
	}
	for (mtmp = migrating_mons; mtmp; mtmp = mtmp->nmon) {
	    for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
		otmp->bypass = 0;
	}
	flags.bypasses = FALSE;
}

void
bypass_obj(obj)
struct obj *obj;
{
	obj->bypass = 1;
	flags.bypasses = TRUE;
}

void
mon_break_armor(mon, polyspot)
struct monst *mon;
boolean polyspot;
{
	register struct obj *otmp;
	struct permonst *mdat = mon->data;
	boolean vis = cansee(mon->mx, mon->my);
	boolean handless_or_tiny = (nohands(mdat) || verysmall(mdat));
	const char *pronoun = mhim(mon),
			*ppronoun = mhis(mon);

	if (breakarm(mdat)) {
	    if ((otmp = which_armor(mon, W_ARM)) != 0) {
		if ((Is_dragon_scales(otmp) &&
			mdat == Dragon_scales_to_pm(otmp)) ||
		    (Is_dragon_mail(otmp) && mdat == Dragon_mail_to_pm(otmp)))
		    ;	/* no message here;
			   "the dragon merges with his scaly armor" is odd
			   and the monster's previous form is already gone */
		else if (vis)
		    pline("%s breaks out of %s armor!", Monnam(mon), ppronoun);
		else
		    You_hear("a cracking sound.");
		m_useup(mon, otmp);
	    }
	    if ((otmp = which_armor(mon, W_ARMC)) != 0) {
		if (otmp->oartifact) {
		    if (vis)
			pline("%s %s falls off!", s_suffix(Monnam(mon)),
				cloak_simple_name(otmp));
		    if (polyspot) bypass_obj(otmp);
		    m_lose_armor(mon, otmp);
		} else {
		    if (vis)
			pline("%s %s tears apart!", s_suffix(Monnam(mon)),
				cloak_simple_name(otmp));
		    else
			You_hear("a ripping sound.");
		    m_useup(mon, otmp);
		}
	    }
	    if ((otmp = which_armor(mon, W_ARMU)) != 0) {
		if (vis)
		    pline("%s shirt rips to shreds!", s_suffix(Monnam(mon)));
		else
		    You_hear("a ripping sound.");
		m_useup(mon, otmp);
	    }
	} else if (sliparm(mdat)) {
	    if ((otmp = which_armor(mon, W_ARM)) != 0) {
		if (vis)
		    pline("%s armor falls around %s!",
				 s_suffix(Monnam(mon)), pronoun);
		else
		    You_hear("a thud.");
		if (polyspot) bypass_obj(otmp);
		m_lose_armor(mon, otmp);
	    }
	    if ((otmp = which_armor(mon, W_ARMC)) != 0) {
		if (vis) {
		    if (is_whirly(mon->data))
			pline("%s %s falls, unsupported!",
				     s_suffix(Monnam(mon)), cloak_simple_name(otmp));
		    else
			pline("%s shrinks out of %s %s!", Monnam(mon),
						ppronoun, cloak_simple_name(otmp));
		}
		if (polyspot) bypass_obj(otmp);
		m_lose_armor(mon, otmp);
	    }
	    if ((otmp = which_armor(mon, W_ARMU)) != 0) {
		if (vis) {
		    if (sliparm(mon->data))
			pline("%s seeps right through %s shirt!",
					Monnam(mon), ppronoun);
		    else
			pline("%s becomes much too small for %s shirt!",
					Monnam(mon), ppronoun);
		}
		if (polyspot) bypass_obj(otmp);
		m_lose_armor(mon, otmp);
	    }
	}
	if (handless_or_tiny) {
	    /* [caller needs to handle weapon checks] */
	    if ((otmp = which_armor(mon, W_ARMG)) != 0) {
		if (vis)
		    pline("%s drops %s gloves%s!", Monnam(mon), ppronoun,
					MON_WEP(mon) ? " and weapon" : "");
		if (polyspot) bypass_obj(otmp);
		m_lose_armor(mon, otmp);
	    }
	    if ((otmp = which_armor(mon, W_ARMS)) != 0) {
		if (vis)
		    pline("%s can no longer hold %s shield!", Monnam(mon),
								ppronoun);
		else
		    You_hear("a clank.");
		if (polyspot) bypass_obj(otmp);
		m_lose_armor(mon, otmp);
	    }
	}
	if (handless_or_tiny || has_horns(mdat)) {
	    if ((otmp = which_armor(mon, W_ARMH)) != 0 &&
		    /* flimsy test for horns matches polyself handling */
		    (handless_or_tiny || !is_flimsy(otmp))) {
		if (vis)
		    pline("%s helmet falls to the %s!",
			  s_suffix(Monnam(mon)), surface(mon->mx, mon->my));
		else
		    You_hear("a clank.");
		if (polyspot) bypass_obj(otmp);
		m_lose_armor(mon, otmp);
	    }
	}
	if (handless_or_tiny || slithy(mdat) || mdat->mlet == S_CENTAUR) {
	    if ((otmp = which_armor(mon, W_ARMF)) != 0) {
		if (vis) {
		    if (is_whirly(mon->data))
			pline("%s boots fall away!",
				       s_suffix(Monnam(mon)));
		    else pline("%s boots %s off %s feet!",
			s_suffix(Monnam(mon)),
			verysmall(mdat) ? "slide" : "are pushed", ppronoun);
		}
		if (polyspot) bypass_obj(otmp);
		m_lose_armor(mon, otmp);
	    }
	}
	if (!can_saddle(mon)) {
	    if ((otmp = which_armor(mon, W_SADDLE)) != 0) {
		if (polyspot) bypass_obj(otmp);
		m_lose_armor(mon, otmp);
		if (vis)
		    pline("%s saddle falls off.", s_suffix(Monnam(mon)));
	    }
	    if (mon == u.usteed)
		goto noride;
	} else if (mon == u.usteed && !can_ride(mon)) {
	noride:
	    You("can no longer ride %s.", mon_nam(mon));
	    if (touch_petrifies(u.usteed->data) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && !(uarmg && !FingerlessGloves && uarmu && uarm && uarmc) && rnl(3)) {
		char buf[BUFSZ];

		You("touch %s.", mon_nam(u.usteed));
		sprintf(buf, "falling off a petrifying steed");
		instapetrify(buf);
	    }
	    dismount_steed(DISMOUNT_FELL);
	}
	return;
}

/* bias a monster's preferences towards armor that has special benefits. */
/* currently only does speed boots, but might be expanded if monsters get to
   use more armor abilities */
static int
extra_pref(mon, obj)
struct monst *mon;
struct obj *obj;
{
    if (obj) {
	if (obj->otyp == SPEED_BOOTS && mon->permspeed != MFAST)
	    return 20;
    }
    return 0;
}

/*
 * Exceptions to things based on race. Correctly checks polymorphed player race.
 * Returns:
 *	 0 No exception, normal rules apply.
 * 	 1 If the race/object combination is acceptable.
 *	-1 If the race/object combination is unacceptable.
 */
int
racial_exception(mon, obj)
struct monst *mon;
struct obj *obj;
{
    const struct permonst *ptr = raceptr(mon);

    /* Acceptable Exceptions: */
    /* Allow hobbits to wear elven armor - LoTR */
    if (is_hobbit(ptr) && is_elven_armor(obj))
	return 1;

    /* sporkhack gnomish suit - somehow never got worn by actual gnomes... --Amy */
    if (is_gnome(ptr) && is_gnomish_armor(obj))
	return 1;

    /* Unacceptable Exceptions: */
    /* Checks for object that certain races should never use go here */
    /*	return -1; */

    return 0;
}
/*worn.c*/
