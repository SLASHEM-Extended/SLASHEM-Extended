/*	SCCS Id: @(#)exper.c	3.4	2002/11/20	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

/*STATIC_DCL*/ long FDECL(newuexp, (int));
STATIC_DCL int FDECL(enermod, (int));

/*STATIC_OVL*/ long
newuexp(lev)
int lev;
{
	/* KMH, balance patch -- changed again! */
	if (lev < 9) return (20L * (1L << lev));
	if (lev < 13) return (10000L * (1L << (lev - 9)));
	if (lev == 13) return (150000L);
	return (50000L * ((long)(lev - 9)));
	/*              Old XP routine */
	/* if (lev < 10) return (10L * (1L << lev));            */
	/* if (lev < 20) return (10000L * (1L << (lev - 10)));  */
	/* return (10000000L * ((long)(lev - 19)));             */
/*      if (lev == 1)  return (75L);
	if (lev == 2)  return (150L);
	if (lev == 3)  return (300L);
	if (lev == 4)  return (600L);
	if (lev == 5)  return (1200L); */
#if 0
	if (lev == 1)  return (50L);     /* need 50           */
	if (lev == 2)  return (100L);    /* need 50           */
	if (lev == 3)  return (200L);    /* need 100          */
	if (lev == 4)  return (500L);    /* need 300          */
	if (lev == 5)  return (1000L);   /* need 500          */
	if (lev == 6)  return (1750L);   /* need 750          */
	if (lev == 7)  return (2750L);   /* need 1000         */
	if (lev == 8)  return (4250L);   /* need 1500         */
	if (lev == 9)  return (6250L);   /* need 2000         */
	if (lev == 10) return (8750L);   /* need 2500         */
	if (lev == 11) return (11750L);  /* need 3000         */
	if (lev == 12) return (15500L);  /* need 3750         */
	if (lev == 13) return (20000L);  /* need 4500         */
	if (lev == 14) return (25000L);  /* need 5000         */
	if (lev == 15) return (31000L);  /* need 6000         */
	if (lev == 16) return (38500L);  /* need 7500         */
	if (lev == 17) return (48000L);  /* need 9500         */
	if (lev == 18) return (60000L);  /* need 12000        */
	if (lev == 19) return (76000L);  /* need 16000        */
	if (lev == 20) return (97000L);  /* need 21000        */
	if (lev == 21) return (125000L); /* need 28000   +7   */
	if (lev == 22) return (163000L); /* need 38000   +10  */
	if (lev == 23) return (213000L); /* need 50000   +12  */
	if (lev == 24) return (279000L); /* need 66000  +16   */
	if (lev == 25) return (365000L); /* need 86000 + 20   */
	if (lev == 26) return (476000L); /* need 111000 + 25  */
	if (lev == 27) return (617000L); /* need 141000+ 30   */
	if (lev == 28) return (798000L); /* need 181000 + 40  */
	if (lev == 29) return (1034000L); /* need 236000 + 55 */
	return (1750000L);
#endif
}

STATIC_OVL int
enermod(en)
int en;
{
	switch (Role_switch) {
		/* WAC 'F' and 'I' get bonus similar to 'W' */
		case PM_FLAME_MAGE:
		case PM_ICE_MAGE:
	case PM_PRIEST:
	case PM_WIZARD:
	    return(2 * en);
	case PM_HEALER:
	case PM_KNIGHT:
	    return((3 * en) / 2);
	case PM_BARBARIAN:
	case PM_VALKYRIE:
	    return((3 * en) / 4);
	default:
	    return (en);
	}
}

int
experience(mtmp, nk)	/* return # of exp points for mtmp after nk killed */
	register struct	monst *mtmp;
	register int	nk;
#if defined(MAC_MPW)
# pragma unused(nk)
#endif
{
	register struct permonst *ptr = mtmp->data;
	int	i, tmp, tmp2;

	tmp = 1 + mtmp->m_lev * mtmp->m_lev;

/*	For higher ac values, give extra experience */
	if ((i = find_mac(mtmp)) < 3) tmp += (7 - i) * ((i < 0) ? 2 : 1);

/*	For very fast monsters, give extra experience */
	if (ptr->mmove > NORMAL_SPEED)
	    tmp += (ptr->mmove > (3*NORMAL_SPEED/2)) ? 5 : 3;

/*	For each "special" attack type give extra experience */
	for(i = 0; i < NATTK; i++) {
	    tmp2 = ptr->mattk[i].aatyp;
	    if(tmp2 > AT_BUTT) {

		if(tmp2 == AT_WEAP) tmp += 5;
		else if(tmp2 == AT_MAGC) tmp += 10;
		else tmp += 3;
	    }
	}

/*	For each "special" damage type give extra experience */
	for(i = 0; i < NATTK; i++) {
	    tmp2 = ptr->mattk[i].adtyp;
	    if(tmp2 > AD_PHYS && tmp2 < AD_BLND) tmp += 2*mtmp->m_lev;
	    else if((tmp2 == AD_DRLI) || (tmp2 == AD_STON) ||
	    		(tmp2 == AD_SLIM)) tmp += 50;
	    else if(tmp != AD_PHYS) tmp += mtmp->m_lev;
		/* extra heavy damage bonus */
	    if((int)(ptr->mattk[i].damd * ptr->mattk[i].damn) > 23)
		tmp += mtmp->m_lev;
	    if (tmp2 == AD_WRAP && ptr->mlet == S_EEL && !Amphibious)
		tmp += 1000;
	}

/*	For certain "extra nasty" monsters, give even more */
	if (extra_nasty(ptr)) tmp += (7 * mtmp->m_lev);

/*	For higher level monsters, an additional bonus is given */
	if(mtmp->m_lev > 8) tmp += 50;

#ifdef MAIL
	/* Mail daemons put up no fight. */
	if(mtmp->data == &mons[PM_MAIL_DAEMON]) tmp = 1;
#endif

	return(tmp);
}

void
more_experienced(exp, rexp)
	register int exp, rexp;
{
	u.uexp += exp;
	u.urexp += 4*exp + rexp;
	if(exp
#ifdef SCORE_ON_BOTL
	   || flags.showscore
#endif
	   ) flags.botl = 1;
	if (u.urexp >= (Role_if(PM_WIZARD) ? 1000 : 2000))
		flags.beginner = 0;
}

void
losexp(drainer,force)	/* e.g., hit by drain life attack */
const char *drainer;	/* cause of death, if drain should be fatal */
boolean force;		/* Force the loss of an experience level */
{
	register int num;

#ifdef WIZARD
	/* explicit wizard mode requests to reduce level are never fatal. */
	if (drainer && !strcmp(drainer, "#levelchange"))
		drainer = 0;
#endif

	if (!force && Drain_resistance) return;

	if (u.ulevel > 1) {
		pline("%s level %d.", Goodbye(), u.ulevel--);
		/* remove intrinsic abilities */
		adjabil(u.ulevel + 1, u.ulevel);
		reset_rndmonst(NON_PM);	/* new monster selection */
	} else {
		if (drainer) {
			killer_format = KILLED_BY;
			killer = drainer;
			done(DIED);
		}
		/* no drainer or lifesaved */
		u.uexp = 0;
	}
	num = newhp();
	u.uhpmax -= num;
	if (u.uhpmax < 1) u.uhpmax = 1;
	u.uhp -= num;
	if (u.uhp < 1) u.uhp = 1;
	else if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;

	if (u.ulevel < urole.xlev)
	    num = rn1((int)ACURR(A_WIS)/2 + urole.enadv.lornd + urace.enadv.lornd,
			urole.enadv.lofix + urace.enadv.lofix);
	else
	    num = rn1((int)ACURR(A_WIS)/2 + urole.enadv.hirnd + urace.enadv.hirnd,
			urole.enadv.hifix + urace.enadv.hifix);
	num = enermod(num);		/* M. Stephenson */
	u.uenmax -= num;
	if (u.uenmax < 0) u.uenmax = 0;
	u.uen -= num;
	if (u.uen < 0) u.uen = 0;
	else if (u.uen > u.uenmax) u.uen = u.uenmax;

	if (u.uexp > 0)
		u.uexp = newuexp(u.ulevel) - 1;
	flags.botl = 1;
}

/*
 * Make experience gaining similar to AD&D(tm), whereby you can at most go
 * up by one level at a time, extra expr possibly helping you along.
 * After all, how much real experience does one get shooting a wand of death
 * at a dragon created with a wand of polymorph??
 */
void
newexplevel()
{
	if (u.ulevel < MAXULEV && u.uexp >= newuexp(u.ulevel))
	    pluslvl(TRUE);
}

#if 0 /* The old newexplevel() */
{
	register int tmp;

	if(u.ulevel < MAXULEV && u.uexp >= newuexp(u.ulevel)) {

		u.ulevel++;
		if (u.ulevelmax < u.ulevel) u.ulevelmax = u.ulevel;	/* KMH */
		if (u.uexp >= newuexp(u.ulevel)) u.uexp = newuexp(u.ulevel) - 1;
		pline("Welcome to experience level %d.", u.ulevel);
		/* give new intrinsics */
		adjabil(u.ulevel - 1, u.ulevel);
		reset_rndmonst(NON_PM); /* new monster selection */
/* STEPHEN WHITE'S NEW CODE */                
		tmp = newhp();
		u.uhpmax += tmp;
		u.uhp += tmp;
		switch (Role_switch) {
			case PM_ARCHEOLOGIST: u.uenbase += rnd(4) + 1; break;
			case PM_BARBARIAN: u.uenbase += rnd(2); break;
			case PM_CAVEMAN: u.uenbase += rnd(2); break;
			case PM_DOPPELGANGER: u.uenbase += rnd(5) + 1; break;
			case PM_ELF: case PM_DROW: u.uenbase += rnd(5) + 1; break;
			case PM_FLAME_MAGE: u.uenbase += rnd(6) + 2; break;
			case PM_GNOME: u.uenbase += rnd(3); break;
			case PM_HEALER: u.uenbase += rnd(6) + 2; break;
			case PM_ICE_MAGE: u.uenbase += rnd(6) + 2; break;
#ifdef YEOMAN
			case PM_YEOMAN:
#endif
			case PM_KNIGHT: u.uenbase += rnd(3); break;
			case PM_HUMAN_WEREWOLF: u.uenbase += rnd(5) + 1; break;
			case PM_MONK: u.uenbase += rnd(5) + 1; break;
			case PM_NECROMANCER: u.uenbase += rnd(6) + 2; break;
			case PM_PRIEST: u.uenbase += rnd(6) + 2; break;
			case PM_ROGUE: u.uenbase += rnd(4) + 1; break;
			case PM_SAMURAI: u.uenbase += rnd(2); break;
#ifdef TOURIST
			case PM_TOURIST: u.uenbase += rnd(4) + 1; break;
#endif
			case PM_UNDEAD_SLAYER: u.uenbase += rnd(3); break;
			case PM_VALKYRIE: u.uenbase += rnd(2); break;
			case PM_WIZARD: u.uenbase += rnd(6) + 2; break;
			default: u.uenbase += rnd(2) + 1; break;
		}
		flags.botl = 1;
	}
}
#endif /* old newexplevel() */

void
pluslvl(incr)
boolean incr;	/* true iff via incremental experience growth */
{		/*	(false for potion of gain level)      */
	register int num;

	if (!incr) You_feel("more experienced.");
	num = newhp();
	u.uhpmax += num;
	u.uhp += num;
	if (Upolyd) {
	    num = rnd(8);
	    u.mhmax += num;
	    u.mh += num;
	}
	if (u.ulevel < urole.xlev)
	    num = rn1((int)ACURR(A_WIS)/2 + urole.enadv.lornd + urace.enadv.lornd,
			urole.enadv.lofix + urace.enadv.lofix);
	else
	    num = rn1((int)ACURR(A_WIS)/2 + urole.enadv.hirnd + urace.enadv.hirnd,
			urole.enadv.hifix + urace.enadv.hifix);
	num = enermod(num);	/* M. Stephenson */
	u.uenmax += num;
	u.uen += num;
	
	if(u.ulevel < MAXULEV) {
	    if (incr) {
		long tmp = newuexp(u.ulevel + 1);
		if (u.uexp >= tmp) u.uexp = tmp - 1;
	    } else {
		u.uexp = newuexp(u.ulevel);
	    }
	    ++u.ulevel;
	    if (u.ulevelmax < u.ulevel) u.ulevelmax = u.ulevel;
	    pline("Welcome to experience level %d.", u.ulevel);
	    adjabil(u.ulevel - 1, u.ulevel);	/* give new intrinsics */
	    reset_rndmonst(NON_PM);		/* new monster selection */
	}
	flags.botl = 1;
}

/* compute a random amount of experience points suitable for the hero's
   experience level:  base number of points needed to reach the current
   level plus a random portion of what it takes to get to the next level */
long
rndexp(gaining)
boolean gaining;	/* gaining XP via potion vs setting XP for polyself */
{
	long minexp, maxexp, diff, factor, result;

	minexp = (u.ulevel == 1) ? 0L : newuexp(u.ulevel - 1);
	maxexp = newuexp(u.ulevel);
	diff = maxexp - minexp,  factor = 1L;
	/* make sure that `diff' is an argument which rn2() can handle */
	while (diff >= (long)LARGEST_INT)
	    diff /= 2L,  factor *= 2L;
	result = minexp + factor * (long)rn2((int)diff);
	/* 3.4.1:  if already at level 30, add to current experience
	   points rather than to threshold needed to reach the current
	   level; otherwise blessed potions of gain level can result
	   in lowering the experience points instead of raising them */
	if (u.ulevel == MAXULEV && gaining) {
	    result += (u.uexp - minexp);
	    /* avoid wrapping (over 400 blessed potions needed for that...) */
	    if (result < u.uexp) result = u.uexp;
	}
	return result;
}

/*exper.c*/
