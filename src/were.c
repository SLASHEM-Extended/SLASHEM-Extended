/*	SCCS Id: @(#)were.c	3.4	2002/11/07	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

#ifdef OVL0

void
were_change(mon)
register struct monst *mon;
{
	if (!is_were(mon->data))
	    return;

	if (is_human(mon->data)) {
	    if (!Protection_from_shape_changers &&
			!rn2(night() ? (flags.moonphase == FULL_MOON ?  3 : 30)
				     : (flags.moonphase == FULL_MOON ? 10 : 50))) {
		new_were(mon);		/* change into animal form */
		if (flags.soundok && !canseemon(mon)) {
		    const char *howler;

		    switch (monsndx(mon->data)) {
		    case PM_HUMAN_WEREWOLF:	howler = "wolf";    break;
		    case PM_HUMAN_WEREJACKAL:	howler = "jackal";  break;
		    case PM_HUMAN_WEREPANTHER:	howler = "panther"; break;
		    case PM_HUMAN_WERETIGER:	howler = "tiger";   break;
		    default:			howler = (char *)0; break;
		    }
		    if (howler)
			You_hear("a %s howling at the moon.", howler);
		}
	    }
	} else if (!rn2(30) || Protection_from_shape_changers) {
	    new_were(mon);		/* change back into human form */
	}
}

#endif /* OVL0 */
#ifdef OVLB

int
counter_were(pm)
int pm;
{
	switch(pm) {
	    case PM_WEREWOLF:	      return(PM_HUMAN_WEREWOLF);
	    case PM_HUMAN_WEREWOLF:   return(PM_WEREWOLF);
	    case PM_WEREJACKAL:	      return(PM_HUMAN_WEREJACKAL);
	    case PM_HUMAN_WEREJACKAL: return(PM_WEREJACKAL);
	    case PM_WERERAT:	      return(PM_HUMAN_WERERAT);
	    case PM_HUMAN_WERERAT:    return(PM_WERERAT);
	    case PM_WEREPANTHER:      return(PM_HUMAN_WEREPANTHER);            
	    case PM_HUMAN_WEREPANTHER:return(PM_WEREPANTHER);
	    case PM_WERETIGER:        return(PM_HUMAN_WERETIGER);
	    case PM_HUMAN_WERETIGER:  return(PM_WERETIGER);
	    case PM_WERESNAKE:        return(PM_HUMAN_WERESNAKE);
	    case PM_HUMAN_WERESNAKE:  return(PM_WERESNAKE);
	    case PM_WERESPIDER:       return(PM_HUMAN_WERESPIDER);
	    case PM_HUMAN_WERESPIDER: return(PM_WERESPIDER);
	    default:		      return(0);
	}
}

void
new_were(mon)
register struct monst *mon;
{
	register int pm;

	pm = counter_were(monsndx(mon->data));
	if(!pm) {
	    impossible("unknown lycanthrope %s.", mon->data->mname);
	    return;
	}

	if(canseemon(mon) && !Hallucination)
	    pline("%s changes into a %s.", Monnam(mon),
			is_human(&mons[pm]) ? "human" :
			mons[pm].mname+4);

	set_mon_data(mon, &mons[pm], 0);
	if (mon->msleeping || !mon->mcanmove) {
	    /* transformation wakens and/or revitalizes */
	    mon->msleeping = 0;
	    mon->mfrozen = 0;	/* not asleep or paralyzed */
	    mon->mcanmove = 1;
	}
	/* regenerate by 1/4 of the lost hit points */
	mon->mhp += (mon->mhpmax - mon->mhp) / 4;
	newsym(mon->mx,mon->my);
	mon_break_armor(mon, FALSE);
	possibly_unwield(mon, FALSE);
	(void) stop_timer(UNPOLY_MON, (genericptr_t) mon);
	(void) start_timer(rn1(1000,1000), TIMER_MONSTER,
		UNPOLY_MON, (genericptr_t) mon);
}

int
were_summon(ptr,yours,visible,genbuf)	/* were-creature (even you) summons a horde */
register struct permonst *ptr;
register boolean yours;
int *visible;			/* number of visible helpers created */
char *genbuf;
{
	register int i, typ, pm = monsndx(ptr);
	register struct monst *mtmp;
	int total = 0;

	*visible = 0;
	if(Protection_from_shape_changers && !yours)
		return 0;
	/*
	 * Allow lycanthropes in normal form to summon hordes as well.  --ALI
	 */
	if (pm == PM_PLAYERMON)
	    pm = urace.malenum;
	for(i = rnd(2); i > 0; i--) {
	   switch(pm) {

		case PM_WERERAT:
		case PM_HUMAN_WERERAT:
			typ = rn2(3) ? PM_SEWER_RAT : rn2(3) ? PM_GIANT_RAT : PM_RABID_RAT ;
			if (genbuf) Strcpy(genbuf, "rat");
			break;
		case PM_WEREJACKAL:
		case PM_HUMAN_WEREJACKAL:
			typ = PM_JACKAL;
			if (genbuf) Strcpy(genbuf, "jackal");
			break;
		case PM_WEREWOLF:
		case PM_HUMAN_WEREWOLF:
			typ = rn2(5) ? PM_WOLF : PM_WINTER_WOLF ;
			if (genbuf) Strcpy(genbuf, "wolf");
			break;
		case PM_WEREPANTHER:
		case PM_HUMAN_WEREPANTHER:
			typ = rn2(5) ? PM_JAGUAR : PM_PANTHER ;
			if (genbuf) Strcpy(genbuf, "large cat");
			break;
		case PM_WERETIGER:
		case PM_HUMAN_WERETIGER:
			typ = rn2(5) ? PM_JAGUAR : PM_TIGER ;
			if (genbuf) Strcpy(genbuf, "large cat");
			break;
		case PM_WERESNAKE:
		case PM_HUMAN_WERESNAKE:
			typ = rn2(5) ? PM_SNAKE : PM_PIT_VIPER ;
			if (genbuf) Strcpy(genbuf, "snake");
			break;
		case PM_WERESPIDER:
		case PM_HUMAN_WERESPIDER:
			typ = rn2(5) ? PM_CAVE_SPIDER : PM_RECLUSE_SPIDER ;
			if (genbuf) Strcpy(genbuf, "spider");
			break;
		default:
			continue;
	    }
	    mtmp = makemon(&mons[typ], u.ux, u.uy, NO_MM_FLAGS);
	    if (mtmp) {
		total++;
		if (canseemon(mtmp)) *visible += 1;
	    }
	    if (yours && mtmp)
		(void) tamedog(mtmp, (struct obj *) 0);
	}
	return total;
}

void
you_were()
{
	char qbuf[QBUFSZ];

	if (Unchanging || (u.umonnum == u.ulycn)) return;
	if (Polymorph_control) {
	    /* `+4' => skip "were" prefix to get name of beast */
	    Sprintf(qbuf, "Do you want to change into %s? ",
		    an(mons[u.ulycn].mname+4));
	    if(yn(qbuf) == 'n') return;
	}
	(void) polymon(u.ulycn);
}

void
you_unwere(purify)
boolean purify;
{
	boolean in_wereform = (u.umonnum == u.ulycn);

	if (purify) {
	    if (Race_if(PM_HUMAN_WEREWOLF)) {
		/* An attempt to purify you has been made! */
		if (in_wereform && Unchanging) {
		    killer_format = NO_KILLER_PREFIX;
		    killer = "purified while stuck in creature form";
		    pline_The("purification was deadly...");
		    done(DIED);
		} else {
		    You_feel("very bad!");
		    if (in_wereform)
			rehumanize();
		    (void) adjattrib(A_STR, -rn1(3,3), 2);
		    (void) adjattrib(A_CON, -rn1(3,3), 1);
		    losehp(u.uhp - (u.uhp > 10 ? rnd(5) : 1), "purification",
			    KILLED_BY);
		}
		return;
	    }
	    You_feel("purified.");
	    u.ulycn = NON_PM;	/* cure lycanthropy */
	    upermonst.mflags2 &= ~M2_WERE;
	}
	if (!Unchanging && in_wereform &&
		(!Polymorph_control || yn("Remain in beast form?") == 'n'))
	    rehumanize();
}

#endif /* OVLB */

/*were.c*/
