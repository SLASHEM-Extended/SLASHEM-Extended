/*	SCCS Id: @(#)exper.c	3.4	2002/11/20	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

/*STATIC_DCL*/ long newuexp(int);
STATIC_DCL int enermod(int);

/*STATIC_OVL*/ long
newuexp(lev)
int lev;
{
	/* KMH, balance patch -- changed again! */
	/*if (lev < 9) return (10L * (1L << lev));
	if (lev < 13) return (10000L * (1L << (lev - 9)));
	if (lev == 13) return (150000L);
	return (50000L * ((long)(lev - 9)));*/
	/*              Old XP routine */
	/* if (lev < 10) return (10L * (1L << lev));            */
	/* if (lev < 20) return (10000L * (1L << (lev - 10)));  */
	/* return (10000000L * ((long)(lev - 19)));             */
/*      if (lev == 1)  return (75L);
	if (lev == 2)  return (150L);
	if (lev == 3)  return (300L);
	if (lev == 4)  return (600L);
	if (lev == 5)  return (1200L); */

	/* completely overhauled by Amy */

	if (!issoviet) {

	if (lev == 1)  return (10L);     /* need 20           */
	if (lev == 2)  return (30L);    /* need 20           */
	if (lev == 3)  return (70L);    /* need 40          */
	if (lev == 4)  return (150L);    /* need 80          */
	if (lev == 5)  return (300L);   /* need 160          */
	if (lev == 6)  return (600L);   /* need 320          */
	if (lev == 7)  return (1200L);   /* need 640         */
	if (lev == 8)  return (2500L);   /* need 1280         */
	if (lev == 9)  return (5000L);   /* need 2440         */
	if (lev == 10) return (10000L);   /* need 5000         */
	if (lev == 11) return (20000L);  /* need 10000         */
	if (lev == 12) return (40000L);  /* need 20000         */
	if (lev == 13) return (80000L);  /* need 40000         */
	if (lev == 14) return (130000L);  /* need 50000         */
	if (lev == 15) return (200000L);  /* need 70000         */
	if (lev == 16) return (280000L);  /* need 80000         */
	if (lev == 17) return (380000L);  /* need 100000         */
	if (lev == 18) return (500000L);  /* need 120000        */
	if (lev == 19) return (650000L);  /* need 150000        */
	if (lev == 20) return (850000L);  /* need 200000        */
	if (lev == 21) return (1100000L); /* need 250000        */
	if (lev == 22) return (1400000L); /* need 300000        */
	if (lev == 23) return (1800000L); /* need 400000        */
	if (lev == 24) return (2300000L); /* need 500000        */
	if (lev == 25) return (3000000L); /* need 700000       */
	if (lev == 26) return (3800000L); /* need 800000       */
	if (lev == 27) return (4800000L); /* need 1000000       */
	if (lev == 28) return (6000000L); /* need 1200000       */
	if (lev == 29) return (8000000L); /* need 2000000      */
	return (15000000L);

	} else {

	if (lev == 1)  return (40L);
	if (lev == 2)  return (80L);
	if (lev == 3)  return (160L);
	if (lev == 4)  return (320L);
	if (lev == 5)  return (640L);
	if (lev == 6)  return (1280L);
	if (lev == 7)  return (2560L);
	if (lev == 8)  return (5120L);
	if (lev == 9)  return (10000L);
	if (lev == 10) return (20000L);
	if (lev == 11) return (40000L);
	if (lev == 12) return (80000L);
	if (lev == 13) return (130000L);
	if (lev == 14) return (200000L);
	if (lev == 15) return (280000L);
	if (lev == 16) return (380000L);
	if (lev == 17) return (480000L);
	if (lev == 18) return (580000L);
	if (lev == 19) return (680000L);
	if (lev == 20) return (850000L);
	if (lev == 21) return (1100000L);
	if (lev == 22) return (1400000L);
	if (lev == 23) return (1800000L);
	if (lev == 24) return (2300000L);
	if (lev == 25) return (3000000L);
	if (lev == 26) return (3800000L);
	if (lev == 27) return (4800000L);
	if (lev == 28) return (6000000L);
	if (lev == 29) return (8000000L);
	return (15000000L);

	}

}

STATIC_OVL int
enermod(en)
int en;
{
	switch (Role_switch) {
		/* WAC 'F' and 'I' get bonus similar to 'W' */
		case PM_FLAME_MAGE:
		case PM_ICE_MAGE:
		case PM_ELECTRIC_MAGE:
		case PM_POISON_MAGE:
		case PM_ACID_MAGE:
		case PM_OCCULT_MASTER:
		case PM_CHAOS_SORCEROR:
		case PM_ELEMENTALIST:
	case PM_PRIEST:
	case PM_WIZARD:
	    return(2 * en);
	case PM_HEALER:
	case PM_KNIGHT:
	    return((3 * en) / 2);
	case PM_BARBARIAN:
	case PM_VALKYRIE:
	case PM_VANILLA_VALK:
	    return((3 * en) / 4);
	case PM_NOOB_MODE_BARB:
	    return(en / 2);
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

	int monlevforexp, monlevdistance;

	monlevforexp = mtmp->m_lev;
	monlevdistance = 0;

	/* overleveled monsters shouldn't give so much bonus; after all, a level 49 church mouse just takes more thwacks
	 * to kill compared to a level 1 one, no need to give millions of experience for that --Amy */
	if (mtmp->m_lev > mtmp->data->mlevel) {
		monlevdistance = mtmp->m_lev - mtmp->data->mlevel;
		if (monlevdistance > 1) monlevdistance /= 2;
	}
	monlevforexp -= monlevdistance;

	tmp = 1 + (monlevforexp * monlevforexp);

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
	    if(tmp2 > AD_PHYS && tmp2 < AD_BLND) tmp += 2*monlevforexp;
	    else if((tmp2 == AD_DRLI) || (tmp2 == AD_STON) ||
	    		(tmp2 == AD_SLIM)) tmp += 50;
	    else if(tmp != AD_PHYS) tmp += monlevforexp;
		/* extra heavy damage bonus */
	    if((int)(ptr->mattk[i].damd * ptr->mattk[i].damn) > 23)
		tmp += monlevforexp;
	    if (tmp2 == AD_WRAP && ptr->mlet == S_EEL) { /* edited by Amy */
		tmp *= 11;
		tmp /= 10;
	    }
	}

/*	For certain "extra nasty" monsters, give even more */
	if (extra_nasty(ptr)) tmp += (rnd(7) * monlevforexp);

/*	For higher level monsters, an additional bonus is given */
	if(monlevforexp > 8) tmp += 50;
	/* Amy edit: high experience levels require lots of XP, but high-level monsters don't give all that much more XP
	 * than low-level ones? gotta fix that... */
	if(monlevforexp > 10 && monlevdistance < 3) {
		int hilvlmod = (monlevforexp - 9);
		tmp += (hilvlmod * hilvlmod);
	}
	if(monlevforexp > 20 && monlevdistance < 6) {
		int hilvlmod = (monlevforexp - 19);
		tmp += (hilvlmod * hilvlmod * rnd(10));
	}
	if(monlevforexp > 30 && monlevdistance < 9) {
		int hilvlmod = (monlevforexp - 29);
		tmp += (hilvlmod * hilvlmod * 10);
	}
	if(monlevforexp > 40 && monlevdistance < 12) {
		int hilvlmod = (monlevforexp - 39);
		tmp += (hilvlmod * hilvlmod * 100);
	}
	/* Amy edit again: but it's still not enough... */
	if (monlevforexp > 0 && tmp > 0) {
		tmp *= (100 + (monlevforexp * 2));
		tmp /= 100;
	}

	/* fluidators are meant to make your life hard, not be a ready source of experience points --Amy */
	if (monstersoundtype(mtmp) == MS_FLUIDATOR && tmp > 1) {
		tmp /= 20;
		if (tmp < 1) tmp = 1;
	}
	if (monstersoundtype(mtmp) == MS_BULLETATOR && tmp > 1) {
		tmp /= 20;
		if (tmp < 1) tmp = 1;
	}

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
	if ( (u.uprops[ANTILEVELING].extrinsic || Antileveling || (uamul && uamul->oartifact == ART_NAZGUL_S_REVENGE) || (uimplant && uimplant->oartifact == ART_CORE_HALO_OF_AURELION_THE_) || have_antilevelstone() ) && !(u.ulevel < 10 && !rn2(u.ulevel + 1)) ) return;

	if ((exp > 0) && Race_if(PM_YEEK)) exp *= 2;
	if ((exp > 0) && uarmc && uarmc->oartifact == ART_TOO_MANY_AFFIXES) exp *= 2;
	if (exp > 0) {
		if (ExpBoost) exp *= (StrongExpBoost ? 3 : 2);
		if (uarmf && uarmf->oartifact == ART_SYSTEM_OF_SEXUAL_PLEASURE) {
			exp *= 11;
			exp /= 10;
		}
		if (uarm && uarm->oartifact == ART_OH_NO_I_SAVED__AM_I_NEVERT) {
			exp *= 11;
			exp /= 10;
		}
	}
	if (uarmc && uarmc->oartifact == ART_ARTIFICIAL_FAKE_DIFFICULTY && (exp > 1)) exp /= 2;
	if (Race_if(PM_ETHEREALOID) && (exp > 1)) exp /= 2;
	if (Race_if(PM_INCORPOREALOID) && (exp > 1)) exp /= 2;
	if (Race_if(PM_MANSTER) && (exp > 1)) {
		exp *= 2;
		exp /= 3;
	}

	u.uexp += exp;
	u.urexp += 4*exp + rexp;
	if (uarm && uarm->oartifact == ART_ARABELLA_S_LIGHTSWITCH) u.urexp += 4*exp + rexp;
	if(exp || flags.showscore) flags.botl = 1;
	if (u.urexp >= (Role_if(PM_WIZARD) ? 1000 : 2000))
		flags.beginner = 0;
}

void
losexp(drainer,force,dresistance)	/* e.g., hit by drain life attack */
const char *drainer;	/* cause of death, if drain should be fatal */
boolean force;		/* Force the loss of an experience level */
boolean dresistance;	/* level drain resistance can protect you */
{
	register int num;
	int expdrain;

#ifdef WIZARD
	/* explicit wizard mode requests to reduce level are never fatal. */
	if (drainer && !strcmp(drainer, "#levelchange"))
		drainer = 0;
#endif

	if (dresistance && Drain_resistance && rn2(StrongDrain_resistance ? 10 : 5) ) return;

	/* level drain is too strong. Let's nerf it a bit. --Amy */
	/* In Soviet Russia, level drain will always drain at least one level, because fuck you, stupid player. You're
	 * not supposed to stand a chance in this game. --Amy */
	if (!force && (u.uexp > 320) && !issoviet && u.ulevel > 1) {
		expdrain = newuexp(u.ulevel) - newuexp(u.ulevel - 1);
		expdrain /= (isfriday ? 3 : 5);
		expdrain = rnz(expdrain);
		if ((u.uexp - expdrain) > newuexp(u.ulevel - 1)) {
			/* drain some experience, but not enough to make you lose a level */
			You_feel("your life draining away!");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vy tol'ko chto poteryali odin uroven', skoro vy poteryayete vse urovni i umeret'." : "Due-l-ue-l-ue-l!");
			u.uexp -= expdrain;
			return;
		}
	} else if (!force && (u.uexp > 20) && (u.uexp <= 320) && !issoviet && u.ulevel > 1) {
		expdrain = newuexp(u.ulevel) - newuexp(u.ulevel - 1);
		expdrain /= (isfriday ? 3 : 5);
		expdrain *= 3;
		expdrain = rnz(expdrain);
		if ((u.uexp - expdrain) > newuexp(u.ulevel - 1)) {
			/* drain some experience, but not enough to make you lose a level */
			You_feel("your life draining away!");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vy tol'ko chto poteryali odin uroven', skoro vy poteryayete vse urovni i umeret'." : "Due-l-ue-l-ue-l!");
			u.uexp -= expdrain;
			return;
		}
	} else if (!force && (u.uexp > 10) && !issoviet && u.ulevel == 1) {
		expdrain = 20;
		expdrain /= (isfriday ? 3 : 5);
		expdrain *= 3;
		expdrain = rnz(expdrain);
		if ((u.uexp - expdrain) > 0) {
			/* drain some experience, but not enough to make you lose a level */
			You_feel("your life draining away!");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vy tol'ko chto poteryali odin uroven', skoro vy poteryayete vse urovni i umeret'." : "Due-l-ue-l-ue-l!");
			u.uexp -= expdrain;
			return;
		}
	}

	if (issoviet) pline("BWAR KHAR (gryaznyy smekh) govorit, chto tip bloka l'da!");

	if (u.ulevel > 1) {
		pline("%s level %d.", Goodbye(), u.ulevel--);
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vy tol'ko chto poteryali odin uroven', skoro vy poteryayete vse urovni i umeret'." : "Due-l-ue-l-ue-l!");
		/* remove intrinsic abilities */
		adjabil(u.ulevel + 1, u.ulevel);
		reset_rndmonst(NON_PM);	/* new monster selection */
	} else {
		if (drainer) {
			u.youaredead = 1;
			killer_format = KILLED_BY;
			killer = drainer;
			done(DIED);
			u.youaredead = 0;
		}
		/* no drainer or lifesaved */
		u.uexp = 0;
	}
	num = newhp();
	u.uhpmax -= num;
	u.uhpmax -= rn2(3);
	u.uhpmax -= rnz(2);
	u.uhpmax -= rnz(3); /* making the drain for gain exploit much harder to perform --Amy */
	if (u.ulevel >= 19) u.uhpmax -= rnd(2);
	if (u.ulevel >= 24) u.uhpmax -= rnd(2);
	if (u.ulevel >= 27) u.uhpmax -= rnd(3);
	if (u.ulevel >= 29) u.uhpmax -= rnd(10);
	if (u.uhpmax < 1) u.uhpmax = 1;
	u.uhp -= num;
	u.uhp -= rn2(3);
	u.uhp -= rnz(3);
	u.uhp -= rnz(2);
	if (u.ulevel >= 19) u.uhp -= rnd(2);
	if (u.ulevel >= 24) u.uhp -= rnd(2);
	if (u.ulevel >= 27) u.uhp -= rnd(3);
	if (u.ulevel >= 29) u.uhp -= rnd(10);
	if (u.uhp < 1) u.uhp = 1;
	else if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;

	/* screwy vanilla programmers... they were so lazy and forgot to make it so that your polymorph form gets drained! */
	if (Upolyd) {
		u.mhmax -= num;
		u.mhmax -= rn2(3);
		u.mhmax -= rnz(2);
		u.mhmax -= rnz(3); /* making the drain for gain exploit much harder to perform --Amy */
		if (u.ulevel >= 19) u.mhmax -= rnd(2);
		if (u.ulevel >= 24) u.mhmax -= rnd(2);
		if (u.ulevel >= 27) u.mhmax -= rnd(3);
		if (u.ulevel >= 29) u.mhmax -= rnd(10);
		if (u.mhmax < 1) u.mhmax = 1;
		u.mh -= num;
		u.mh -= rn2(3);
		u.mh -= rnz(3);
		u.mh -= rnz(2);
		if (u.ulevel >= 19) u.mh -= rnd(2);
		if (u.ulevel >= 24) u.mh -= rnd(2);
		if (u.ulevel >= 27) u.mh -= rnd(3);
		if (u.ulevel >= 29) u.mh -= rnd(10);
		if (u.mh < 1) u.mh = 1;
		else if (u.mh > u.mhmax) u.mh = u.mhmax;
	}

	if (u.ulevel < urole.xlev)
	    num = rn1((int)ACURR(A_WIS)/2 + urole.enadv.lornd + urace.enadv.lornd,
			urole.enadv.lofix + urace.enadv.lofix);
	else
	    num = rn1((int)ACURR(A_WIS)/2 + urole.enadv.hirnd + urace.enadv.hirnd,
			urole.enadv.hifix + urace.enadv.hifix);
	num = enermod(num);		/* M. Stephenson */
	u.uenmax -= num;
	u.uenmax -= rn2(3);
	u.uenmax -= rnz(3);
	u.uenmax -= (rn2(3) ? rnz(1) : rnz(2));
	if (u.uenmax < 0) u.uenmax = 0;
	u.uen -= num;
	u.uen -= rn2(3);
	u.uen -= rnz(3);
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
	else if (u.uexp >= (10000000 + (2000000 * u.xtralevelmult))) {
		You_feel("more experienced.");
		pluslvl(TRUE); /* will increase the xtralevelmult variable */
		/* leveling via EXP can keep giving you skill slots --Amy
		 * but now also via gain level, because that requires more and more potions, too
		 * effect moved to pluslvl() function */
	}
}

/* for calculations where stuff is supposed to get better late into the game, so that the char doesn't suck --Amy
 * "boostmult" is a multiplier that determines how much of an impact the boost value has */
int
boosted_ulevel(boostmult)
int boostmult;
{
	int boostedlevel = u.ulevel;

	boostedlevel += (boost_power_value() * boostmult);

	return boostedlevel;
}

/* if your level is rather high already, gain level effects shouldn't always give you a full level --Amy
 * important: increase u.uexp value even when you have anti-experience! */
void
gainlevelmaybe()
{
	if (u.ulevel >= MAXULEV && ((u.uexp + 200000) < (10000000 + (2000000 * u.xtralevelmult)) ) ) {
		u.uexp += 200000;
		flags.botl = TRUE;
		You("gain experience.");
	} else if (u.ulevel < MAXULEV && ((u.uexp + 200000) < newuexp(u.ulevel)) ) {
		u.uexp += 200000;
		flags.botl = TRUE;
		You("gain experience.");
	} else pluslvl(FALSE);
}

void
pluslvl(incr)
boolean incr;	/* true iff via incremental experience growth */
{		/*	(false for potion of gain level)      */
	register int num;

	if (!incr) You_feel("more experienced.");

	if (u.ulevel >= MAXULEV) {
		u.uexp = 10000000; /* reset counter for gain level */
		u.xtralevelmult++;
		u.cnd_overlevelcount++;
		u.weapon_slots++; /* leveling past 30 can keep giving you skill slots --Amy */
		techlevelup(); /* and also improve your techniques --Amy */
	}

	if (u.ulevel < MAXULEV) {

	if (!((Deprovement || u.uprops[DEPROVEMENT].extrinsic || have_deprovementstone()) && !(u.ulevel < 10 && !rn2(u.ulevel + 1)) && rn2(10) ) ) {

	/* homicider only gains 50% of the usual amount --Amy */
	/* a.k.a. "bullshit downside that every fun new race gets" (term coined by Khor) */

	num = newhp();
	num += rnz(2);
	if (num < 0) num = 0;
	num += rn2(3);
	if (u.ulevel >= 19) num += rnd(2);
	if (u.ulevel >= 24) num += rnd(2);
	if (u.ulevel >= 27) num += rnd(3);
	if (u.ulevel >= 29) num += rnd(10);

	if (Race_if(PM_YEEK) || Race_if(PM_DUFFLEPUD) || Race_if(PM_PLAYER_FAIRY)) num /= 2;
	if (Race_if(PM_PLAYER_DOLGSMAN)) {
		num *= 3;
		num /= 4;
	}

	if (ishomicider && num > 0) {
		num /= 2;
		if (num > 0 && !rn2(2)) num--;
	}
	if (Race_if(PM_SPRIGGAN) && num > 0) {
		num /= 2;
		if (num > 0 && !rn2(2)) num--;
	}

	u.uhpmax += num;
	u.uhp += num;

	if ((u.ulevel >= u.urmaxlvlUP && u.ulevel < 30) && !issoviet && (u.uhp < u.uhpmax)) u.uhp = u.uhpmax;
	if (issoviet) pline("Vy dazhe ne poluchayete polnyye linii, potomu chto sovetskiy ne ponimayet, kak rolevyye igry rabotayut!");
	/* In Soviet Russia, you don't get full health on leveling up because seriously, who needs that? --Amy */
	if (Upolyd) {
	    num = rnz(8); /* unfortunately will be lost upon unpolymorphing --Amy */
	    if (num < 0) num = 0;
	    num += rn2(3);

	    if (Race_if(PM_YEEK) || Race_if(PM_DUFFLEPUD) || Race_if(PM_PLAYER_FAIRY)) num /= 2;
	    if (Race_if(PM_PLAYER_DOLGSMAN)) {
		num *= 3;
		num /= 4;
	    }

	    u.mhmax += num;
	    u.mh += num;

		if ((u.ulevel >= u.urmaxlvlUP && u.ulevel < 30) && !issoviet && (u.mh < u.mhmax)) u.mh = u.mhmax;
	}
	if (u.ulevel < urole.xlev)
	    num = rn1((int)ACURR(A_WIS)/2 + urole.enadv.lornd + urace.enadv.lornd,
			urole.enadv.lofix + urace.enadv.lofix);
	else
	    num = rn1((int)ACURR(A_WIS)/2 + urole.enadv.hirnd + urace.enadv.hirnd,
			urole.enadv.hifix + urace.enadv.hifix);

	num = enermod(num);	/* M. Stephenson */
	num += (rn2(3) ? rnz(1) : rnz(2));
	if (num < 0) num = 0;
	num += rn2(3);

	if (Race_if(PM_YEEK) || Race_if(PM_DUFFLEPUD)) num /= 2;

	if (ishomicider && num > 0) {
		num /= 2;
		if (num > 0 && !rn2(2)) num--;
	}

	u.uenmax += num;
	u.uen += num;
	} /* homicider check */

	} else { /* u.ulevel > MAXULEV */

	if (!((Deprovement || u.uprops[DEPROVEMENT].extrinsic || have_deprovementstone()) && !(u.ulevel < 10 && !rn2(u.ulevel + 1)) && rn2(10) ) ) {

	num = newhp();
	num += rnz(2);
	if (num < 0) num = 0;
	if (num > 1) num /= rnd(12);
	num += rn2(2);

	if (Race_if(PM_YEEK) || Race_if(PM_DUFFLEPUD) || Race_if(PM_PLAYER_FAIRY)) num /= 2;
	if (Race_if(PM_PLAYER_DOLGSMAN)) {
		num *= 3;
		num /= 4;
	}

	if (ishomicider && num > 0) {
		num /= 2;
		if (num > 0 && !rn2(2)) num--;
	}
	if (Race_if(PM_SPRIGGAN) && num > 0) {
		num /= 2;
		if (num > 0 && !rn2(2)) num--;
	}

	u.uhpmax += num;
	u.uhp += num;

	if (Upolyd) {
	    num = rnz(8); /* unfortunately will be lost upon unpolymorphing --Amy */
	    if (num < 0) num = 0;
	    if (num > 1) num /= rnd(12);
	    num += rn2(2);

	    if (Race_if(PM_YEEK) || Race_if(PM_DUFFLEPUD) || Race_if(PM_PLAYER_FAIRY)) num /= 2;
	    if (Race_if(PM_PLAYER_DOLGSMAN)) {
		num *= 3;
		num /= 4;
	    }

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
	num += (rn2(3) ? rnz(1) : rnz(2));
	if (num < 0) num = 0;
	if (num > 1) num /= rnd(12);
	num += rn2(2);

	if (Race_if(PM_YEEK) || Race_if(PM_DUFFLEPUD)) num /= 2;

	if (ishomicider && num > 0) {
		num /= 2;
		if (num > 0 && !rn2(2)) num--;
	}

	u.uenmax += num;
	u.uen += num;

	} /* homicider check */

	} /* u.ulevel > or < MAXULEV */

	if(u.ulevel < MAXULEV) {
	    if (incr) {
		long tmp = newuexp(u.ulevel + 1);
		if (u.uexp >= tmp) u.uexp = tmp - 1;
	    } else {
		u.uexp = newuexp(u.ulevel);
	    }
	    ++u.ulevel;
	    if (u.ulevelmax < u.ulevel) u.ulevelmax = u.ulevel;

	    if (Race_if(PM_RELEASIER)) verbalize("Iii-am-debest!");
	    else pline("Welcome to experience level %d.", u.ulevel);

	    adjabil(u.ulevel - 1, u.ulevel);	/* give new intrinsics */
	    reset_rndmonst(NON_PM);		/* new monster selection */
	}

	exprecalc();

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
	/* don't make blessed gain level too strong... --Amy */
	if (gaining && ((newuexp(u.ulevel) - newuexp(u.ulevel - 1)) > 200000)) {
		maxexp = (newuexp(u.ulevel - 1)) + 200000;
	}
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

void
exprecalc(void)
{
	register struct obj *acqo;
	struct obj *ubookz;

	while (u.ulevel > u.urmaxlvlUP && u.ulevel < 30) {
		u.urmaxlvlUP++;

		if ( (Role_if(PM_FEMINIST) || Role_if(PM_GRENADONIN)) && !rn2(5)) {

			boolean havegifts = u.ugifts;

			if (!havegifts) u.ugifts++;

			acqo = mk_artifact((struct obj *)0, !rn2(3) ? A_CHAOTIC : rn2(2) ? A_NEUTRAL : A_LAWFUL, TRUE);
			if (acqo) {
			    dropy(acqo);
				int acqoskill = get_obj_skill(acqo, TRUE);
				if (P_MAX_SKILL(acqoskill) == P_ISRESTRICTED) {
					unrestrict_weapon_skill(acqoskill);
				} else if (P_MAX_SKILL(acqoskill) == P_UNSKILLED) {
					unrestrict_weapon_skill(acqoskill);
					P_MAX_SKILL(acqoskill) = P_BASIC;
				} else if (rn2(2) && P_MAX_SKILL(acqoskill) == P_BASIC) {
					P_MAX_SKILL(acqoskill) = P_SKILLED;
				} else if (!rn2(4) && P_MAX_SKILL(acqoskill) == P_SKILLED) {
					P_MAX_SKILL(acqoskill) = P_EXPERT;
				} else if (!rn2(10) && P_MAX_SKILL(acqoskill) == P_EXPERT) {
					P_MAX_SKILL(acqoskill) = P_MASTER;
				} else if (!rn2(100) && P_MAX_SKILL(acqoskill) == P_MASTER) {
					P_MAX_SKILL(acqoskill) = P_GRAND_MASTER;
				} else if (!rn2(200) && P_MAX_SKILL(acqoskill) == P_GRAND_MASTER) {
					P_MAX_SKILL(acqoskill) = P_SUPREME_MASTER;
				}

				if (Race_if(PM_RUSMOT)) {
					if (P_MAX_SKILL(acqoskill) == P_ISRESTRICTED) {
						unrestrict_weapon_skill(acqoskill);
					} else if (P_MAX_SKILL(acqoskill) == P_UNSKILLED) {
						unrestrict_weapon_skill(acqoskill);
						P_MAX_SKILL(acqoskill) = P_BASIC;
					} else if (rn2(2) && P_MAX_SKILL(acqoskill) == P_BASIC) {
						P_MAX_SKILL(acqoskill) = P_SKILLED;
					} else if (!rn2(4) && P_MAX_SKILL(acqoskill) == P_SKILLED) {
						P_MAX_SKILL(acqoskill) = P_EXPERT;
					} else if (!rn2(10) && P_MAX_SKILL(acqoskill) == P_EXPERT) {
						P_MAX_SKILL(acqoskill) = P_MASTER;
					} else if (!rn2(100) && P_MAX_SKILL(acqoskill) == P_MASTER) {
						P_MAX_SKILL(acqoskill) = P_GRAND_MASTER;
					} else if (!rn2(200) && P_MAX_SKILL(acqoskill) == P_GRAND_MASTER) {
						P_MAX_SKILL(acqoskill) = P_SUPREME_MASTER;
					}
				}

			    discover_artifact(acqo->oartifact);

				if (!havegifts) u.ugifts--;
				pline("An artifact appeared beneath you!");

			}

		}

		if (uarmh && uarmh->oartifact == ART_JESTES_TAKA_KURWA) {
			if (u.urmaxlvlUP == 4) NotSeenBug |= FROMOUTSIDE;
			else if (u.urmaxlvlUP == 8) GrayoutBug |= FROMOUTSIDE;
			else if (u.urmaxlvlUP == 12) DifficultyIncreased |= FROMOUTSIDE;
			else if (u.urmaxlvlUP == 16) UnfairAttackBug |= FROMOUTSIDE;
			else if (u.urmaxlvlUP == 20) EnmityBug |= FROMOUTSIDE;
			else if (u.urmaxlvlUP == 25) ElongationBug |= FROMOUTSIDE;
			else if (u.urmaxlvlUP == 30) Antileveling |= FROMOUTSIDE;
		}

		if (Role_if(PM_CLIMACTERIAL) && !rn2(3)) {

			boolean climintcheck = 0;
			int whichcheck = u.climauspices3;
			int displaycheck = 3;			
			switch (u.urmaxlvlUP) {
				default:
					whichcheck = 0; displaycheck = 0; goto climdone; break;
				case 2:
					whichcheck = u.climauspices3; displaycheck = 3; break;
				case 5:
					whichcheck = u.climauspices6; displaycheck = 6; break;
				case 8:
					whichcheck = u.climauspices9; displaycheck = 9; break;
				case 11:
					whichcheck = u.climauspices12; displaycheck = 12; break;
				case 14:
					whichcheck = u.climauspices15; displaycheck = 15; break;
				case 17:
					whichcheck = u.climauspices18; displaycheck = 18; break;
				case 20:
					whichcheck = u.climauspices21; displaycheck = 21; break;
				case 23:
					whichcheck = u.climauspices24; displaycheck = 24; break;
				case 26:
					whichcheck = u.climauspices27; displaycheck = 27; break;
				case 27:
					whichcheck = u.climauspices30; displaycheck = 30; break;
			}
			if (rnd(70) < ACURR(A_INT)) climintcheck = 1;

			if (climintcheck) { /* keyword "marlena" */
				switch (whichcheck) {
					case 1:
						pline("You receive an elaborate auspicious message: At experience level %d, the game will start spawning random fart traps, heel traps and farting monsters.", displaycheck);
						break;
					case 2:
						pline("You receive an elaborate auspicious message: At experience level %d, intelligent female monsters have greatly increased to-hit for their kick attacks.", displaycheck);
						break;
					case 3:
						pline("You receive an elaborate auspicious message: At experience level %d, some glyphs will display in a green color.", displaycheck);
						break;
					case 4:
						pline("You receive an elaborate auspicious message: At experience level %d, random shit traps will start spawning and you trigger them even if you fly.", displaycheck);
						break;
					case 5:
						pline("You receive an elaborate auspicious message: At experience level %d, your constitution is decreased, lashing monsters deal extra damage to you, and monsters with touch attacks can cause you to fart.", displaycheck);
						break;
					case 6:
						pline("You receive an elaborate auspicious message: At experience level %d, you randomly get hit with the aggravate monster effect and the chat command can irritate monsters.", displaycheck);
						break;
					case 7:
						pline("You receive an elaborate auspicious message: At experience level %d, intelligent female monsters have more HP, and their to-hit against you is increased while your to-hit against them is reduced.", displaycheck);
						break;
					case 8:
						pline("You receive an elaborate auspicious message: At experience level %d, farting monsters can fart at you from a distance, and if they find a toilet, they can paralyze you from very far away.", displaycheck);
						break;
					case 9:
						pline("You receive an elaborate auspicious message: At experience level %d, monsters with farting or high heel attacks may use them several times at once.", displaycheck);
						break;
					case 10:
						pline("You receive an elaborate auspicious message: At experience level %d, walking over a toilet can cause you to take a shit, your strength and dexterity are reduced, and monsters can use toilets to paralyze you.", displaycheck);
						break;
					case 11:
						pline("You receive an elaborate auspicious message: At experience level %d, a ghostly girl will start to follow you.", displaycheck);
						break;
					case 12:
						pline("You receive an elaborate auspicious message: At experience level %d, attacking an intelligent female monster causes you to get hit with retribution.", displaycheck);
						break;
					case 13:
						pline("You receive an elaborate auspicious message: At experience level %d, intelligent female monsters can attack you with winter stilettos.", displaycheck);
						break;
					case 14:
						pline("You receive an elaborate auspicious message: At experience level %d, you have to dance or get scarred by female toenails.", displaycheck);
						break;
					case 15:
						pline("You receive an elaborate auspicious message: At experience level %d, intelligent female monsters can scrape off more and more skin the more often they kick you.", displaycheck);
						break;
					case 16:
						pline("You receive an elaborate auspicious message: At experience level %d, toilets start spawning randomly and walking over one can cause you to take a crap.", displaycheck);
						break;
					case 17:
						pline("You receive an elaborate auspicious message: At experience level %d, you will have to fart randomly.", displaycheck);
						break;
					case 18:
						pline("You receive an elaborate auspicious message: At experience level %d, intelligent female monsters can fart at you.", displaycheck);
						break;
					case 19:
						pline("You receive an elaborate auspicious message: At experience level %d, stinking clouds can form randomly.", displaycheck);
						break;
					case 20:
						pline("You receive an elaborate auspicious message: At experience level %d, you may sometimes become heavily confused, and tiles can turn into clouds, occasionally with monsters.", displaycheck);
						break;
					case 21:
						pline("You receive an elaborate auspicious message: At experience level %d, monsters with loud farting attacks spawn over time.", displaycheck);
						break;
					case 22:
						pline("You receive an elaborate auspicious message: At experience level %d, intelligent female monsters deal more damage to you with kick attacks and may request that you clean their shoes.", displaycheck);
						break;
					case 23:
						pline("You receive an elaborate auspicious message: At experience level %d, intelligent female monsters have additional hug attacks.", displaycheck);
						break;
					case 24:
						pline("You receive an elaborate auspicious message: At experience level %d, intelligent female monsters move more quickly.", displaycheck);
						break;
					case 25:
						pline("You receive an elaborate auspicious message: At experience level %d, intelligent female monsters can knee you in the nuts repeatedly and occasionally also massage them.", displaycheck);
						break;
					case 26:
						pline("You receive an elaborate auspicious message: At experience level %d, intelligent female monsters with kick attacks can paralyze you.", displaycheck);
						break;
					case 27:
						pline("You receive an elaborate auspicious message: At experience level %d, intelligent female monsters can burn you with cigarettes.", displaycheck);
						break;
					case 28:
						pline("You receive an elaborate auspicious message: At experience level %d, monsters with kick attacks can wither your gear.", displaycheck);
						break;
					case 29:
						pline("You receive an elaborate auspicious message: At experience level %d, intelligent female monsters can kick the weapon out of your hands.", displaycheck);
						break;
					case 30:
						pline("You receive an elaborate auspicious message: At experience level %d, Julietta will subject you to random punishments.", displaycheck);
						break;
					case 31:
						pline("You receive an elaborate auspicious message: At experience level %d, the master of nasty traps will start constructing traps everywhere.", displaycheck);
						break;
					case 32:
						pline("You receive an elaborate auspicious message: At experience level %d, intelligent female monsters will attack you with random high heels.", displaycheck);
						break;
					case 33:
						pline("You receive an elaborate auspicious message: At experience level %d, perfume monsters will spawn much more frequently, and you trigger shit traps even if you fly.", displaycheck);
						break;
					case 34:
						pline("You receive an elaborate auspicious message: At experience level %d, monsters that use conversion sermon will spawn over time.", displaycheck);
						break;
					case 35:
						pline("You receive an elaborate auspicious message: At experience level %d, monsters with claw attacks spawn more often and can damage your worn boots.", displaycheck);
						break;
					case 36:
						pline("You receive an elaborate auspicious message: At experience level %d, monsters with squeaky farting attacks spawn more often, and all hostile farting monsters are very hard to kill.", displaycheck);
						break;
					case 37:
						pline("You receive an elaborate auspicious message: At experience level %d, feminism traps spawn much more often.", displaycheck);
						break;
					case 38:
						pline("You receive an elaborate auspicious message: At experience level %d, you develop anorexia and have reduced strength and constitution.", displaycheck);
						break;
					case 39:
						pline("You receive an elaborate auspicious message: At experience level %d, random shoe monsters will spawn over time and want you to clean them.", displaycheck);
						break;
					case 40:
						pline("You receive an elaborate auspicious message: At experience level %d, female monsters with kick attacks spawn much more often and are also sometimes forcibly spawned.", displaycheck);
						break;
					case 41:
						pline("You receive an elaborate auspicious message: At experience level %d, intelligent female monsters can try to steal your items.", displaycheck);
						break;
					case 42:
						pline("You receive an elaborate auspicious message: At experience level %d, intelligent female monsters can scratch you bloodily with their heels.", displaycheck);
						break;
					case 43:
						pline("You receive an elaborate auspicious message: At experience level %d, you sometimes fumble into a heap of shit, even if you fly.", displaycheck);
						break;
					case 44:
						pline("You receive an elaborate auspicious message: At experience level %d, an annoying blonde will start to annoy you to death.", displaycheck);
						break;
					case 45:
						pline("You receive an elaborate auspicious message: At experience level %d, female monsters will just swat your projectiles away.", displaycheck);
						break;
					case 46:
						pline("You receive an elaborate auspicious message: At experience level %d, female socks will attack you constantly and try to paralyze you.", displaycheck);
						break;
					case 47:
						pline("You receive an elaborate auspicious message: At experience level %d, grave walls will start appearing, and they have paralysis traps or hostile women underneath.", displaycheck);
						break;
					case 48:
						pline("You receive an elaborate auspicious message: At experience level %d, female monsters will start constructing paralysis traps for you to stumble into.", displaycheck);
						break;
					case 49:
						pline("You receive an elaborate auspicious message: At experience level %d, female monsters have super regeneration.", displaycheck);
						break;
					case 50:
						pline("You receive an elaborate auspicious message: At experience level %d, female monsters are no longer afraid of you and always know where you are.", displaycheck);
						break;
					case 51:
						pline("You receive an elaborate auspicious message: At experience level %d, women with block-heeled combat boots will be very angry at you and show you who's the boss of this dungeon.", displaycheck);
						break;
					case 52:
						pline("You receive an elaborate auspicious message: At experience level %d, some women will fart you in the face without emitting any kind of sound.", displaycheck);
						break;
					case 53:
						pline("You receive an elaborate auspicious message: At experience level %d, you turn into a thick girl with steel-capped sandals.", displaycheck);
						break;
					case 54:
						pline("You receive an elaborate auspicious message: At experience level %d, you'll start walking the dyke with your extra thick block heels.", displaycheck);
						break;
					case 55:
						pline("You receive an elaborate auspicious message: At experience level %d, you have to go to the toilet together with Antje, and may end up quitting the game because you consider the toilet minigame to be too icky.", displaycheck);
						break;
					case 56:
						pline("You receive an elaborate auspicious message: At experience level %d, female monsters will attack you with block-heeled boots, wooden sandals and soft sneakers.", displaycheck);
						break;
					case 57:
						pline("You receive an elaborate auspicious message: At experience level %d, you have to fight lots of monster shoes, pants and socks.", displaycheck);
						break;
					case 58:
						pline("You receive an elaborate auspicious message: At experience level %d, you have to step into dog shit every once in a while.", displaycheck);
						break;
					case 59:
						pline("You receive an elaborate auspicious message: At experience level %d, you develop 'anorexia' which means that you have to vomit whenever you eat, although that condition is actually called bulimia nervosa.", displaycheck);
						break;
					case 60:
						pline("You receive an elaborate auspicious message: At experience level %d, you start menstruating if you're female, or if you're not, you'll be whacked over the head by female wedge sandals repeatedly (which serves you right if you dare being male, muahahahaha!).", displaycheck);
						break;
					case 61:
						pline("You receive an elaborate auspicious message: At experience level %d, boss bitches will challenge you to a duel.", displaycheck);
						break;
					case 62:
						pline("You receive an elaborate auspicious message: At experience level %d, you'll see what kind of pants the girls are wearing.", displaycheck);
						break;
					case 63:
						pline("You receive an elaborate auspicious message: At experience level %d, you have to watch out because there might be heaps of shit where you don't expect them.", displaycheck);
						break;
					case 64:
						pline("You receive an elaborate auspicious message: At experience level %d, you're not allowed to freely attack women anymore, you sick bastard.", displaycheck);
						break;
					case 65:
						pline("You receive an elaborate auspicious message: At experience level %d, women are surrounded by a dark ball of light (i.e. black light).", displaycheck);
						break;
					case 66:
						pline("You receive an elaborate auspicious message: At experience level %d, monsters will trigger feminism traps for you, hahaha.", displaycheck);
						break;
					case 67:
						pline("You receive an elaborate auspicious message: At experience level %d, you'll have to sniff worn female pants on occasion.", displaycheck);
						break;
					case 68:
						pline("You receive an elaborate auspicious message: At experience level %d, feminism traps will affect you for a loooooong time.", displaycheck);
						break;
					case 69:
						pline("You receive an elaborate auspicious message: At experience level %d, women will powerfully kick you with their platform sandals.", displaycheck);
						break;
					case 70:
						pline("You receive an elaborate auspicious message: At experience level %d, you'll start encountering lots of ladies in elegant cone-heeled pumps.", displaycheck);
						break;
					case 71:
						pline("You receive an elaborate auspicious message: At experience level %d, thick women can hold you in place so that you can't run away.", displaycheck);
						break;
					case 72:
						pline("You receive an elaborate auspicious message: At experience level %d, some little girl wants to crap loudly to annoy you.", displaycheck);
						break;
					case 73:
						pline("You receive an elaborate auspicious message: At experience level %d, some girl will wait just until you're out of commission, and then she'll move up to you and fart you in the face.", displaycheck);
						break;
					case 74:
						pline("You receive an elaborate auspicious message: At experience level %d, feminism traps will be built for you, and I hope you'll trigger every last one of them because they make the game more exciting.", displaycheck);
						break;
					case 75:
						pline("You receive an elaborate auspicious message: At experience level %d, girls and women will regenerate health a bit more quickly.", displaycheck);
						break;
					case 76:
						pline("You receive an elaborate auspicious message: At experience level %d, you kinda turn into a gang scholar, so you'll have to deal with the Bang Gang and Anna's hussies.", displaycheck);
						break;
					case 77:
						pline("You receive an elaborate auspicious message: At experience level %d, you kinda turn into a shoe fetishist, meaning that you'll encounter lots of shoe monsters and they have some new kick attacks that everyone wants to experience.", displaycheck);
						break;
					case 78:
						pline("You receive an elaborate auspicious message: At experience level %d, ladies will come for you and produce very tender farting noises that you'll find irresistible.", displaycheck);
						break;
					case 79:
						pline("You receive an elaborate auspicious message: At experience level %d, feminism traps will start to behave like nasty traps.", displaycheck);
						break;
					case 80:
						pline("You receive an elaborate auspicious message: At experience level %d, you'll have a harder time fighting women.", displaycheck);
						break;
					case 81:
						pline("You receive an elaborate auspicious message: At experience level %d, female monsters will have additional armor class and resistances.", displaycheck);
						break;
					case 82:
						pline("You receive an elaborate auspicious message: At experience level %d, you'll hear clacking noises whenever someone walks around in high heels.", displaycheck);
						break;
					case 83:
						pline("You receive an elaborate auspicious message: At experience level %d, a nice girl will start following you around and have fun with you.", displaycheck);
						break;
					case 84:
						pline("You receive an elaborate auspicious message: At experience level %d, you can use high heels even without having the skill but cannot have certain properties if you aren't wearing any.", displaycheck);
						break;
					case 85:
						pline("You receive an elaborate auspicious message: At experience level %d, your shoes can turn into artifact missys but the patriarch will try to beat you up.", displaycheck);
						break;
					case 86:
						pline("You receive an elaborate auspicious message: At experience level %d, particularly feminine monsters are unlikely to die for real when they're killed.", displaycheck);
						break;
					case 87:
						pline("You receive an elaborate auspicious message: At experience level %d, monsters might occasionally polymorph into particularly feminine ones.", displaycheck);
						break;
					case 88:
						pline("You receive an elaborate auspicious message: At experience level %d, a little girl starts stealing your stuff and running away on occasion.", displaycheck);
						break;
					case 89:
						pline("You receive an elaborate auspicious message: At experience level %d, monsters with feminism attacks are spawning more often.", displaycheck);
					case 90:
						pline("You receive an elaborate auspicious message: At experience level %d, covetous shoes will spawn over time and attack you.", displaycheck);
					case 91:
						pline("You receive an elaborate auspicious message: At experience level %d, shoe monsters spawn more often and occasionally appear in groups.", displaycheck);
					case 92:
						pline("You receive an elaborate auspicious message: At experience level %d, shoe monsters may appear to attack you and then immediately disappear to another dungeon level.", displaycheck);
					case 93:
						pline("You receive an elaborate auspicious message: At experience level %d, farting or perfume monsters will be waiting for you on certain dungeon levels.", displaycheck);
					case 94:
						pline("You receive an elaborate auspicious message: At experience level %d, smaller female monsters are more likely to chase you aggressively.", displaycheck);
					case 95:
						pline("You receive an elaborate auspicious message: At experience level %d, girls who produce squeaky farting noises want to play with you, but if you treat them badly they'll try to kill you.", displaycheck);
					case 96:
						pline("You receive an elaborate auspicious message: At experience level %d, the Bang Gang will hunt you, and they have equipment that allows them to block your progress or create traps.", displaycheck);
					case 97:
						pline("You receive an elaborate auspicious message: At experience level %d, female item-stealing monsters are likely to levelport away with your stuff.", displaycheck);
					case 98:
						pline("You receive an elaborate auspicious message: At experience level %d, damaging a female monster causes you to be affected with a nastytrap effect.", displaycheck);
						break;
					case 99:
						pline("You receive an elaborate auspicious message: At experience level %d, female sneakers are upper strong.", displaycheck);
						break;
					case 100:
						pline("You receive an elaborate auspicious message: At experience level %d, hitting a female monster may cause aggravation.", displaycheck);
						break;
					case 101:
						pline("You receive an elaborate auspicious message: At experience level %d, perfume monsters shoot red gas bullets at you which emit a horrible stench.", displaycheck);
						break;
					case 102:
						pline("You receive an elaborate auspicious message: At experience level %d, being paralyzed can cause treaded high heels to attack you intermittently.", displaycheck);
						break;
					case 103:
						pline("You receive an elaborate auspicious message: At experience level %d, you're supposed to wear Sabrina's platform boots, and are chased by angry women. If you don't wear the boots, you won't be able to defeat the women, hahaha!", displaycheck);
						break;
					case 104:
						pline("You receive an elaborate auspicious message: At experience level %d, farting attacks, as well as catching a whiff from odoriferous socks or pants, can drain your very soul.", displaycheck);
						break;
				}
			} else pline("Your auspices say: %d %d", displaycheck, whichcheck);

		}
climdone:

		if (Role_if(PM_FEMINIST) && !rn2(3)) {

			boolean femintcheck = 0;
			if (rnd(70) < ACURR(A_INT)) femintcheck = 1;

			if (u.urmaxlvlUP == 3) {
				switch (u.femauspices4) {
					case 1:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 4, your potions will have a chance of not working when quaffed.");
						else pline("Your auspices say: 4 1");
						break;
					case 2:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 4, your techniques will stop working.");
						else pline("Your auspices say: 4 2");
						break;
					case 3:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 4, Elbereth engravings will no longer work.");
						else pline("Your auspices say: 4 3");
						break;
					case 4:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 4, zapping a wand will have a chance to make it explode.");
						else pline("Your auspices say: 4 4");
						break;
					case 5:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 4, reading a spellbook is no longer safe.");
						else pline("Your auspices say: 4 5");
						break;
					case 6:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 4, your magical effects will always be very low-level.");
						else pline("Your auspices say: 4 6");
						break;
					case 7:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 4, you will no longer be able to uncurse your items via scrolls of remove curse, holy water or cancellation.");
						else pline("Your auspices say: 4 7");
						break;
					case 8:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 4, the 'never mind' or 'nothing happens' messages will cause bad effects.");
						else pline("Your auspices say: 4 8");
						break;
					case 9:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 4, you will repeatedly lose spell memory.");
						else pline("Your auspices say: 4 9");
						break;
					case 10:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 4, your skill training will occasionally be reduced.");
						else pline("Your auspices say: 4 10");
						break;
					case 11:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 4, your techniques will occasionally receive increased timeouts.");
						else pline("Your auspices say: 4 11");
						break;
					case 12:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 4, you can no longer use items whose level is higher than yours.");
						else pline("Your auspices say: 4 12");
						break;
				}

			} else if (u.urmaxlvlUP == 5) {
				switch (u.femauspices6) {
					case 1:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 6, your right mouse button will fail.");
						else pline("Your auspices say: 6 1");
						break;
					case 2:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 6, your display will fail.");
						else pline("Your auspices say: 6 2");
						break;
					case 3:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 6, there will be low local memory.");
						else pline("Your auspices say: 6 3");
						break;
					case 4:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 6, you will get BIGscript.");
						else pline("Your auspices say: 6 4");
						break;
					case 5:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 6, you will be afflicted with weak sight.");
						else pline("Your auspices say: 6 5");
						break;
					case 6:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 6, you will see flicker strips.");
						else pline("Your auspices say: 6 6");
						break;
					case 7:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 6, menus will be fleecy-colored.");
						else pline("Your auspices say: 6 7");
						break;
					case 8:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 6, you start seeing mojibake glyphs.");
						else pline("Your auspices say: 6 8");
						break;
					case 9:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 6, all sanity effects will increase your sanity by much more than usual.");
						else pline("Your auspices say: 6 9");
						break;
					case 10:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 6, your techniques won't display anything.");
						else pline("Your auspices say: 6 10");
						break;
					case 11:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 6, lowercase monsters display as if they were uppercase.");
						else pline("Your auspices say: 6 11");
						break;
					case 12:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 6, the skill enhancing menu doesn't display anything.");
						else pline("Your auspices say: 6 12");
						break;
				}

			} else if (u.urmaxlvlUP == 7) {
				switch (u.femauspices8) {
					case 1:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 8, the entire game will display in shades of grey.");
						else pline("Your auspices say: 8 1");
						break;
					case 2:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 8, a ROT13 cypher will be activated.");
						else pline("Your auspices say: 8 2");
						break;
					case 3:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 8, you will have to solve captchas.");
						else pline("Your auspices say: 8 3");
						break;
					case 4:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 8, you will have to take part in the Nethack Quiz.");
						else pline("Your auspices say: 8 4");
						break;
					case 5:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 8, there will be sound effects.");
						else pline("Your auspices say: 8 5");
						break;
					case 6:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 8, walls will become hyper blue.");
						else pline("Your auspices say: 8 6");
						break;
					case 7:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 8, the pokedex will lie to you.");
						else pline("Your auspices say: 8 7");
						break;
					case 8:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 8, monsters will make noises.");
						else pline("Your auspices say: 8 8");
						break;
					case 9:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 8, increasing stats beyond a certain limit will become much harder.");
						else pline("Your auspices say: 8 9");
						break;
					case 10:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 8, the top line displays fleecy-colored messages.");
						else pline("Your auspices say: 8 10");
						break;
					case 11:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 8, the game becomes miscolored.");
						else pline("Your auspices say: 8 11");
						break;
					case 12:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 8, one specific color becomes fleecy.");
						else pline("Your auspices say: 8 12");
						break;
					case 13:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 8, matching colors start fleecing back and forth.");
						else pline("Your auspices say: 8 13");
						break;
				}

			} else if (u.urmaxlvlUP == 10) {
				switch (u.femauspices11) {
					case 1:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 11, your spells will become pink.");
						else pline("Your auspices say: 11 1");
						break;
					case 2:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 11, your spells will become cyan.");
						else pline("Your auspices say: 11 2");
						break;
					case 3:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 11, your spells will become black.");
						else pline("Your auspices say: 11 3");
						break;
					case 4:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 11, your spells will become red.");
						else pline("Your auspices say: 11 4");
						break;
					case 5:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 11, your spells will become green.");
						else pline("Your auspices say: 11 5");
						break;
					case 6:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 11, your spells will become blue.");
						else pline("Your auspices say: 11 6");
						break;
					case 7:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 11, your spells will become completely gray.");
						else pline("Your auspices say: 11 7");
						break;
					case 8:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 11, your spells will become white.");
						else pline("Your auspices say: 11 8");
						break;
					case 9:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 11, your spells will become yellow.");
						else pline("Your auspices say: 11 9");
						break;
					case 10:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 11, your spells will become orange.");
						else pline("Your auspices say: 11 10");
						break;
					case 11:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 11, your spells will become violet.");
						else pline("Your auspices say: 11 11");
						break;
					case 12:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 11, your spells will become brown.");
						else pline("Your auspices say: 11 12");
						break;
					case 13:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 11, your spells will become bright cyan.");
						else pline("Your auspices say: 11 13");
						break;
					case 14:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 11, your spells will become silver.");
						else pline("Your auspices say: 11 14");
						break;
					case 15:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 11, your spells will become metal.");
						else pline("Your auspices say: 11 15");
						break;
					case 16:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 11, your spells will become platinum.");
						else pline("Your auspices say: 11 16");
						break;
				}

			} else if (u.urmaxlvlUP == 12) {
				switch (u.femauspices13) {
					case 1:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 13, you will be subjected to random intrinsic loss.");
						else pline("Your auspices say: 13 1");
						break;
					case 2:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 13, you will start to temporarily lose intrinsics.");
						else pline("Your auspices say: 13 2");
						break;
					case 3:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 13, you will start suffering from blood loss.");
						else pline("Your auspices say: 13 3");
						break;
					case 4:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 13, all newly spawned monsters will be hostile.");
						else pline("Your auspices say: 13 4");
						break;
					case 5:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 13, covetous monsters start using their AI in a smarter way.");
						else pline("Your auspices say: 13 5");
						break;
					case 6:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 13, identifying items becomes much more difficult.");
						else pline("Your auspices say: 13 6");
						break;
					case 7:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 13, Satan will cause you to lose a turn when entering a new level.");
						else pline("Your auspices say: 13 7");
						break;
					case 8:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 13, monsters are generated with movement energy.");
						else pline("Your auspices say: 13 8");
						break;
					case 9:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 13, you will be in the bad part.");
						else pline("Your auspices say: 13 9");
						break;
					case 10:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 13, you will be in the completely bad part.");
						else pline("Your auspices say: 13 10");
						break;
				}

			} else if (u.urmaxlvlUP == 14) {
				switch (u.femauspices15) {
					case 1:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 15, you will get the speed bug.");
						else pline("Your auspices say: 15 1");
						break;
					case 2:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 15, your multi-turn actions will be interrupted every turn.");
						else pline("Your auspices say: 15 2");
						break;
					case 3:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 15, you can no longer teleport at all.");
						else pline("Your auspices say: 15 3");
						break;
					case 4:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 15, all items that you drop will automatically curse themselves.");
						else pline("Your auspices say: 15 4");
						break;
					case 5:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 15, monsters can randomly move faster than normal.");
						else pline("Your auspices say: 15 5");
						break;
					case 6:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 15, blessed items that you pick up will instantly become cursed.");
						else pline("Your auspices say: 15 6");
						break;
					case 7:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 15, using consumables will curse them first.");
						else pline("Your auspices say: 15 7");
						break;
					case 8:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 15, items that monsters steal from you will be degraded.");
						else pline("Your auspices say: 15 8");
						break;
					case 9:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 15, you cannot use spellbooks or tools unless their base item type is identified.");
						else pline("Your auspices say: 15 9");
						break;
					case 10:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 15, items lose their BUC, enchantment and rustproofing knowledge when monsters pick them up, and they no longer stack properly.");
						else pline("Your auspices say: 15 10");
						break;
				}

			} else if (u.urmaxlvlUP == 16) {
				switch (u.femauspices17) {
					case 1:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 17, the monster difficulty will be increased.");
						else pline("Your auspices say: 17 1");
						break;
					case 2:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 17, monsters start spawning much faster.");
						else pline("Your auspices say: 17 2");
						break;
					case 3:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 17, boss monsters become more common.");
						else pline("Your auspices say: 17 3");
						break;
					case 4:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 17, rare monsters become common.");
						else pline("Your auspices say: 17 4");
						break;
					case 5:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 17, high-level monsters become more common.");
						else pline("Your auspices say: 17 5");
						break;
					case 6:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 17, the minimum level for newly spawned monsters will start scaling with the actual level difficulty.");
						else pline("Your auspices say: 17 6");
						break;
					case 7:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 17, monster attacks use exploding dice to determine their damage.");
						else pline("Your auspices say: 17 7");
						break;
					case 8:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 17, chaotic monsters will live up to their alignment by making the dungeon more chaotic.");
						else pline("Your auspices say: 17 8");
						break;
					case 9:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 17, you're permanently burdened even if your inventory is empty.");
						else pline("Your auspices say: 17 9");
						break;
					case 10:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 17, your carry capacity can never be higher than 500.");
						else pline("Your auspices say: 17 10");
						break;
					case 11:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 17, your ranged weapon ammo mulches every time you fire.");
						else pline("Your auspices say: 17 11");
						break;
					case 12:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 17, your main inventory is limited to 52 items.");
						else pline("Your auspices say: 17 12");
						break;
				}

			} else if (u.urmaxlvlUP == 19) {
				switch (u.femauspices20) {
					case 1:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 20, your possessions start randomly unidentifying themselves.");
						else pline("Your auspices say: 20 1");
						break;
					case 2:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 20, you will get insufficient information about items.");
						else pline("Your auspices say: 20 2");
						break;
					case 3:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 20, dungeon floors become invisible.");
						else pline("Your auspices say: 20 3");
						break;
					case 4:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 20, monsters become stronger if many of their species have been generated already.");
						else pline("Your auspices say: 20 4");
						break;
					case 5:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 20, walls become invisible.");
						else pline("Your auspices say: 20 5");
						break;
					case 6:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 20, the map will fill up with fake I markers.");
						else pline("Your auspices say: 20 6");
						break;
					case 7:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 20, I markers become invisible and will cost a turn if you bump into them.");
						else pline("Your auspices say: 20 7");
						break;
					case 8:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 20, you are subjected to the wing yellow changer.");
						else pline("Your auspices say: 20 8");
						break;
					case 9:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 20, your sight is reduced if you don't trigger a trap for a while.");
						else pline("Your auspices say: 20 9");
						break;
					case 10:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 20, escaping past messages will do bad things.");
						else pline("Your auspices say: 20 10");
						break;
				}

			} else if (u.urmaxlvlUP == 21) {
				switch (u.femauspices22) {
					case 1:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 22, you can no longer cure status effects.");
						else pline("Your auspices say: 22 1");
						break;
					case 2:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 22, the dungeon starts becoming ever more chaotic.");
						else pline("Your auspices say: 22 2");
						break;
					case 3:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 22, altars and prayer become unsafe.");
						else pline("Your auspices say: 22 3");
						break;
					case 4:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 22, all newly spawned monsters will be invisible, and see invisible won't help.");
						else pline("Your auspices say: 22 4");
						break;
					case 5:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 22, all newly spawned traps will be invisible, and there is no way to make them visible again.");
						else pline("Your auspices say: 22 5");
						break;
					case 6:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 22, empty dungeon floor tiles will quickly turn into walls.");
						else pline("Your auspices say: 22 6");
						break;
					case 7:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 22, spells with low spell memory are difficult to cast.");
						else pline("Your auspices say: 22 7");
						break;
					case 8:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 22, any trap you trigger will become invisible.");
						else pline("Your auspices say: 22 8");
						break;
					case 9:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 22, bad effects turn into really bad effects.");
						else pline("Your auspices say: 22 9");
						break;
					case 10:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 22, the pokedex will display incorrect attack types, and sometimes monsters may actually use those.");
						else pline("Your auspices say: 22 10");
						break;
					case 11:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 22, many ways of detecting stuff in your surrounding area won't work anymore.");
						else pline("Your auspices say: 22 11");
						break;
					case 12:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 22, enhancing a skill drains its amount of training and if that puts it below the threshold for enhancing it, you cannot enhance it after all.");
						else pline("Your auspices say: 22 12");
						break;
					case 13:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 22, casting a spell puts you on a cooldown until you can cast again.");
						else pline("Your auspices say: 22 13");
						break;
				}

			} else if (u.urmaxlvlUP == 24) {
				switch (u.femauspices25) {
					case 1:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 25, the display will intermittently become all gray.");
						else pline("Your auspices say: 25 1");
						break;
					case 2:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 25, your position and the eight surrounding squares will be obscured.");
						else pline("Your auspices say: 25 2");
						break;
					case 3:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 25, you will contract the checkerboard disease.");
						else pline("Your auspices say: 25 3");
						break;
					case 4:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 25, you will no longer be able to identify the type of a trap.");
						else pline("Your auspices say: 25 4");
						break;
					case 5:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 25, you will no longer be able to determine what monsters are.");
						else pline("Your auspices say: 25 5");
						break;
					case 6:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 25, leveling up no longer increases your maximum HP and Pw.");
						else pline("Your auspices say: 25 6");
						break;
					case 7:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 25, some calculations will work as if your level were 1.");
						else pline("Your auspices say: 25 7");
						break;
					case 8:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 25, a manler will start chasing you.");
						else pline("Your auspices say: 25 8");
						break;
					case 9:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 25, you will get a giant explorer.");
						else pline("Your auspices say: 25 9");
						break;
					case 10:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 25, the in-game options will occasionally be set to different values.");
						else pline("Your auspices say: 25 10");
						break;
				}

			} else if (u.urmaxlvlUP == 27) {
				switch (u.femauspices28) {
					case 1:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 28, your maximum alignment record will decrease over time.");
						else pline("Your auspices say: 28 1");
						break;
					case 2:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 28, you will start suffering from recurring amnesia.");
						else pline("Your auspices say: 28 2");
						break;
					case 3:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 28, your items will start to disenchant themselves spontaneously.");
						else pline("Your auspices say: 28 3");
						break;
					case 4:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 28, all equipment you put on will autocurse.");
						else pline("Your auspices say: 28 4");
						break;
					case 5:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 28, monsters will be able to use their secret attack.");
						else pline("Your auspices say: 28 5");
						break;
					case 6:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 28, you will suffer from random item destruction.");
						else pline("Your auspices say: 28 6");
						break;
					case 7:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 28, monsters learn to use their ranged attacks from infinitely far away.");
						else pline("Your auspices say: 28 7");
						break;
					case 8:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 28, monsters will be able to use unfair attacks.");
						else pline("Your auspices say: 28 8");
						break;
					case 9:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 28, the presence of bosses causes a horrible call to ring in your head.");
						else pline("Your auspices say: 28 9");
						break;
					case 10:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 28, your equipment will dull much more quickly.");
						else pline("Your auspices say: 28 10");
						break;
				}

			} else if (u.urmaxlvlUP == 29) {
				switch (u.femauspices30) {
					case 1:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 30, you can no longer open your inventory.");
						else pline("Your auspices say: 30 1");
						break;
					case 2:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 30, killed monsters no longer drop corpses.");
						else pline("Your auspices say: 30 2");
						break;
					case 3:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 30, your directional keys will be inverted.");
						else pline("Your auspices say: 30 3");
						break;
					case 4:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 30, you always have to press Ctrl-R to see what happened.");
						else pline("Your auspices say: 30 8");
						break;
					case 5:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 30, you will barely be able to see the quasars.");
						else pline("Your auspices say: 30 4");
						break;
					case 6:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 30, your directional keys will be shifted 45 degrees clockwise.");
						else pline("Your auspices say: 30 5");
						break;
					case 7:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 30, your directional keys will be shifted 90 degrees counterclockwise.");
						else pline("Your auspices say: 30 6");
						break;
					case 8:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 30, bumping into monsters will cost a turn unless you use a certain prefix.");
						else pline("Your auspices say: 30 7");
						break;
					case 9:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 30, you won't see anything unless you stand on specific tiles.");
						else pline("Your auspices say: 30 9");
						break;
					case 10:
						if (femintcheck) pline("You receive an elaborate auspicious message: At experience level 30, your stats will slowly decay.");
						else pline("Your auspices say: 30 10");
						break;
				}

			}

		}

	}
	

	if (isproblematic && !rn2(3)) {
		/* no xlvl check - if you get drained repeatedly, your loss! I'm really mean :D --Amy */

		getnastytrapintrinsic();

	}

	/* slex has so many skills, you keep running out of slots all the damn time! Need to counteract that --Amy */
	while (u.ulevel > u.urmaxlvlN) {
		u.urmaxlvlN++;
		if (!Race_if(PM_BABYLONIAN)) {
			if (u.urmaxlvlN == 5) u.weapon_slots += 1;
			if (u.urmaxlvlN == 10) u.weapon_slots += 5;
			if (u.urmaxlvlN == 15) u.weapon_slots += 1;
			if (u.urmaxlvlN == 20) u.weapon_slots += 5;
			if (u.urmaxlvlN == 25) u.weapon_slots += 1;
			if (u.urmaxlvlN == 30) u.weapon_slots += 5;
		}
		if (u.urmaxlvlN > 19) { /* very high-level characters should have better maximum HP --Amy */
			u.uhp += 9;
			u.uhpmax += 9;
			flags.botl = TRUE;
		}
	}

	while (Race_if(PM_RODNEYAN) && u.ulevel > u.urmaxlvl) {

		u.urmaxlvl++;

		if (!rn2(2)) {
			ubookz = mkobj(SPBOOK_CLASS, FALSE, FALSE);
			if (ubookz) dropy(ubookz);
			pline("A book appeared at your feet!");
		}

	}

	while ( (Race_if(PM_ASGARDIAN) || (uwep && uwep->oartifact == ART_DOUBLE_MYSTERY)) && u.ulevel > u.urmaxlvlL) {

		u.urmaxlvlL++;

		if (!rn2(3)) { switch (rnd(63)) {

			case 1: 
			case 2: 
			case 3: 
			    HFire_resistance |= FROMOUTSIDE; pline("Got fire resistance!"); break;
			case 4: 
			case 5: 
			case 6: 
			    HCold_resistance |= FROMOUTSIDE; pline("Got cold resistance!"); break;
			case 7: 
			case 8: 
			case 9: 
			    HSleep_resistance |= FROMOUTSIDE; pline("Got sleep resistance!"); break;
			case 10: 
			case 11: 
			    HDisint_resistance |= FROMOUTSIDE; pline("Got disintegration resistance!"); break;
			case 12: 
			case 13: 
			case 14: 
			    HShock_resistance |= FROMOUTSIDE; pline("Got shock resistance!"); break;
			case 15: 
			case 16: 
			case 17: 
			    HPoison_resistance |= FROMOUTSIDE; pline("Got poison resistance!"); break;
			case 18: 
			    HDrain_resistance |= FROMOUTSIDE; pline("Got drain resistance!"); break;
			case 19: 
			    HSick_resistance |= FROMOUTSIDE; pline("Got sickness resistance!"); break;
			case 20: 
			    HAcid_resistance |= FROMOUTSIDE; pline("Got acid resistance!"); break;
			case 21: 
			case 22: 
			    HHunger |= FROMOUTSIDE; pline("You start to hunger rapidly!"); break;
			case 23: 
			case 24: 
			    HSee_invisible |= FROMOUTSIDE; pline("Got see invisible!"); break;
			case 25: 
			    HTelepat |= FROMOUTSIDE; pline("Got telepathy!"); break;
			case 26: 
			case 27: 
			    HWarning |= FROMOUTSIDE; pline("Got warning!"); break;
			case 28: 
			case 29: 
			    HSearching |= FROMOUTSIDE; pline("Got searching!"); break;
			case 30: 
			case 31: 
			    HStealth |= FROMOUTSIDE; pline("Got stealth!"); break;
			case 32: 
			case 33: 
			    HAggravate_monster |= FROMOUTSIDE; pline("You aggravate monsters!"); break;
			case 34: 
			    HConflict |= FROMOUTSIDE; pline("You start causing conflict!"); break;
			case 35: 
			case 36: 
			    HTeleportation |= FROMOUTSIDE; pline("Got teleportitis!"); break;
			case 37: 
			    HTeleport_control |= FROMOUTSIDE; pline("Got teleport control!"); break;
			case 38: 
			    HFlying |= FROMOUTSIDE; pline("Got flying!"); break;
			case 39: 
			    HSwimming |= FROMOUTSIDE; pline("Got swimming!"); break;
			case 40: 
			    HMagical_breathing |= FROMOUTSIDE; pline("Got unbreathing!"); break;
			case 41: 
			    HSlow_digestion |= FROMOUTSIDE; pline("Got slow digestion!"); break;
			case 42: 
			case 43: 
			    HRegeneration |= FROMOUTSIDE; pline("Got regeneration!"); break;
			case 44: 
			    HPolymorph |= FROMOUTSIDE; pline("Got polymorphitis!"); break;
			case 45: 
			    HPolymorph_control |= FROMOUTSIDE; pline("Got polymorph control!"); break;
			case 46: 
			case 47: 
			    HFast |= FROMOUTSIDE; pline("Got speed!"); break;
			case 48: 
			    HInvis |= FROMOUTSIDE; pline("Got invisibility!"); break;
			case 49: 
			    HManaleech |= FROMOUTSIDE; pline("Got manaleech!"); break;
			case 50: 
			    HPeacevision |= FROMOUTSIDE; pline("Got peacevision!"); break;
			case 51: 
			    HDefusing |= FROMOUTSIDE; pline("Got defusing power!"); break;
			case 52: 
			    HWinceState |= FROMOUTSIDE; pline("You start wincing!"); break;
			case 53: 
			    HPainSense |= FROMOUTSIDE; pline("Got pain sense!"); break;
			case 54: 
			    HBurdenedState |= FROMOUTSIDE; pline("Got burden!"); break;
			case 55: 
			    HMagicVacuum |= FROMOUTSIDE; pline("Got magic vacuum!"); break;
			case 56: 
			    HResistancePiercing |= FROMOUTSIDE; pline("Got resistance piercing!"); break;
			case 57: 
			    HFuckOverEffect |= FROMOUTSIDE; pline("Got fuck-over!"); break;
			case 58:
			case 59:
			case 60:
			    HMysteryResist |= FROMOUTSIDE; pline("Got mystery resistance!"); break;
			case 61:
			    HMagicFindBonus |= FROMOUTSIDE; pline("Got magic find!"); break;
			case 62:
			    HSpellboost |= FROMOUTSIDE; pline("Got spellboost!"); break;
			case 63:
			    HWildWeatherEffect |= FROMOUTSIDE; pline("Got wild weather!"); break;

			default:
				break;

			}

		}

	}

	while (Role_if(PM_SOCIAL_JUSTICE_WARRIOR) && u.ulevel > u.urmaxlvlK) {

		u.urmaxlvlK++;

		if (!rn2(4)) {
			int wondertech = rnd(MAXTECH-1);
		    	learntech_or_leveltech(wondertech, FROMOUTSIDE, 1);
			You("learn how to perform a new technique!");

		}

	}

	while (Role_if(PM_WILD_TALENT) && u.ulevel > u.urmaxlvlF) {

		u.urmaxlvlF++;

		if (!rn2(4)) { switch (rnd(63)) {

			case 1: 
			case 2: 
			case 3: 
			    HFire_resistance |= FROMOUTSIDE; pline("Got fire resistance!"); break;
			case 4: 
			case 5: 
			case 6: 
			    HCold_resistance |= FROMOUTSIDE; pline("Got cold resistance!"); break;
			case 7: 
			case 8: 
			case 9: 
			    HSleep_resistance |= FROMOUTSIDE; pline("Got sleep resistance!"); break;
			case 10: 
			case 11: 
			    HDisint_resistance |= FROMOUTSIDE; pline("Got disintegration resistance!"); break;
			case 12: 
			case 13: 
			case 14: 
			    HShock_resistance |= FROMOUTSIDE; pline("Got shock resistance!"); break;
			case 15: 
			case 16: 
			case 17: 
			    HPoison_resistance |= FROMOUTSIDE; pline("Got poison resistance!"); break;
			case 18: 
			    HDrain_resistance |= FROMOUTSIDE; pline("Got drain resistance!"); break;
			case 19: 
			    HSick_resistance |= FROMOUTSIDE; pline("Got sickness resistance!"); break;
			case 20: 
			    HAcid_resistance |= FROMOUTSIDE; pline("Got acid resistance!"); break;
			case 21: 
			case 22: 
			    HHunger |= FROMOUTSIDE; pline("You start to hunger rapidly!"); break;
			case 23: 
			case 24: 
			    HSee_invisible |= FROMOUTSIDE; pline("Got see invisible!"); break;
			case 25: 
			    HTelepat |= FROMOUTSIDE; pline("Got telepathy!"); break;
			case 26: 
			case 27: 
			    HWarning |= FROMOUTSIDE; pline("Got warning!"); break;
			case 28: 
			case 29: 
			    HSearching |= FROMOUTSIDE; pline("Got searching!"); break;
			case 30: 
			case 31: 
			    HStealth |= FROMOUTSIDE; pline("Got stealth!"); break;
			case 32: 
			case 33: 
			    HAggravate_monster |= FROMOUTSIDE; pline("You aggravate monsters!"); break;
			case 34: 
			    HConflict |= FROMOUTSIDE; pline("You start causing conflict!"); break;
			case 35: 
			case 36: 
			    HTeleportation |= FROMOUTSIDE; pline("Got teleportitis!"); break;
			case 37: 
			    HTeleport_control |= FROMOUTSIDE; pline("Got teleport control!"); break;
			case 38: 
			    HFlying |= FROMOUTSIDE; pline("Got flying!"); break;
			case 39: 
			    HSwimming |= FROMOUTSIDE; pline("Got swimming!"); break;
			case 40: 
			    HMagical_breathing |= FROMOUTSIDE; pline("Got unbreathing!"); break;
			case 41: 
			    HSlow_digestion |= FROMOUTSIDE; pline("Got slow digestion!"); break;
			case 42: 
			case 43: 
			    HRegeneration |= FROMOUTSIDE; pline("Got regeneration!"); break;
			case 44: 
			    HPolymorph |= FROMOUTSIDE; pline("Got polymorphitis!"); break;
			case 45: 
			    HPolymorph_control |= FROMOUTSIDE; pline("Got polymorph control!"); break;
			case 46: 
			case 47: 
			    HFast |= FROMOUTSIDE; pline("Got speed!"); break;
			case 48: 
			    HInvis |= FROMOUTSIDE; pline("Got invisibility!"); break;
			case 49: 
			    HManaleech |= FROMOUTSIDE; pline("Got manaleech!"); break;
			case 50: 
			    HPeacevision |= FROMOUTSIDE; pline("Got peacevision!"); break;
			case 51: 
			    HDefusing |= FROMOUTSIDE; pline("Got defusing power!"); break;
			case 52: 
			    HWinceState |= FROMOUTSIDE; pline("You start wincing!"); break;
			case 53: 
			    HPainSense |= FROMOUTSIDE; pline("Got pain sense!"); break;
			case 54: 
			    HBurdenedState |= FROMOUTSIDE; pline("Got burden!"); break;
			case 55: 
			    HMagicVacuum |= FROMOUTSIDE; pline("Got magic vacuum!"); break;
			case 56: 
			    HResistancePiercing |= FROMOUTSIDE; pline("Got resistance piercing!"); break;
			case 57: 
			    HFuckOverEffect |= FROMOUTSIDE; pline("Got fuck-over!"); break;
			case 58:
			case 59:
			case 60:
			    HMysteryResist |= FROMOUTSIDE; pline("Got mystery resistance!"); break;
			case 61:
			    HMagicFindBonus |= FROMOUTSIDE; pline("Got magic find!"); break;
			case 62:
			    HSpellboost |= FROMOUTSIDE; pline("Got spellboost!"); break;
			case 63:
			    HWildWeatherEffect |= FROMOUTSIDE; pline("Got wild weather!"); break;

			default:
				break;

			}

		}

	}

	while (Race_if(PM_MISSINGNO) && u.ulevel > u.urmaxlvlM) {

		u.urmaxlvlM++;

		if (!rn2(3)) learnrandomregulartech();
	}

	while (Role_if(PM_DQ_SLIME) && u.ulevel > u.urmaxlvlC) {

		u.urmaxlvlC++;

			int skillimprove = randomgoodskill();

			if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
				unrestrict_weapon_skill(skillimprove);
				pline("You can now learn the %s skill.", wpskillname(skillimprove));
			} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
				unrestrict_weapon_skill(skillimprove);
				P_MAX_SKILL(skillimprove) = P_BASIC;
				pline("You can now learn the %s skill.", wpskillname(skillimprove));
			} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
				P_MAX_SKILL(skillimprove) = P_SKILLED;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
				P_MAX_SKILL(skillimprove) = P_EXPERT;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
				P_MAX_SKILL(skillimprove) = P_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
				P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(200) && P_MAX_SKILL(skillimprove) == P_GRAND_MASTER) {
				P_MAX_SKILL(skillimprove) = P_SUPREME_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else pline("Unfortunately, you feel no different than before.");

		if (u.urmaxlvlC >= 10) {

			int skillimprove = randomgoodskill();

			if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
				unrestrict_weapon_skill(skillimprove);
				pline("You can now learn the %s skill.", wpskillname(skillimprove));
			} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
				unrestrict_weapon_skill(skillimprove);
				P_MAX_SKILL(skillimprove) = P_BASIC;
				pline("You can now learn the %s skill.", wpskillname(skillimprove));
			} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
				P_MAX_SKILL(skillimprove) = P_SKILLED;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
				P_MAX_SKILL(skillimprove) = P_EXPERT;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
				P_MAX_SKILL(skillimprove) = P_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
				P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(200) && P_MAX_SKILL(skillimprove) == P_GRAND_MASTER) {
				P_MAX_SKILL(skillimprove) = P_SUPREME_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else pline("Unfortunately, you feel no different than before.");

		}

		if (u.urmaxlvlC >= 20) {

			int skillimprove = randomgoodskill();

			if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
				unrestrict_weapon_skill(skillimprove);
				pline("You can now learn the %s skill.", wpskillname(skillimprove));
			} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
				unrestrict_weapon_skill(skillimprove);
				P_MAX_SKILL(skillimprove) = P_BASIC;
				pline("You can now learn the %s skill.", wpskillname(skillimprove));
			} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
				P_MAX_SKILL(skillimprove) = P_SKILLED;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
				P_MAX_SKILL(skillimprove) = P_EXPERT;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
				P_MAX_SKILL(skillimprove) = P_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
				P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(200) && P_MAX_SKILL(skillimprove) == P_GRAND_MASTER) {
				P_MAX_SKILL(skillimprove) = P_SUPREME_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else pline("Unfortunately, you feel no different than before.");

		}

		if (u.urmaxlvlC >= 30) {

			int skillimprove = randomgoodskill();

			if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
				unrestrict_weapon_skill(skillimprove);
				pline("You can now learn the %s skill.", wpskillname(skillimprove));
			} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
				unrestrict_weapon_skill(skillimprove);
				P_MAX_SKILL(skillimprove) = P_BASIC;
				pline("You can now learn the %s skill.", wpskillname(skillimprove));
			} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
				P_MAX_SKILL(skillimprove) = P_SKILLED;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
				P_MAX_SKILL(skillimprove) = P_EXPERT;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
				P_MAX_SKILL(skillimprove) = P_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
				P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(200) && P_MAX_SKILL(skillimprove) == P_GRAND_MASTER) {
				P_MAX_SKILL(skillimprove) = P_SUPREME_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else pline("Unfortunately, you feel no different than before.");

			skillimprove = randomgoodskill();

			if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
				unrestrict_weapon_skill(skillimprove);
				pline("You can now learn the %s skill.", wpskillname(skillimprove));
			} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
				unrestrict_weapon_skill(skillimprove);
				P_MAX_SKILL(skillimprove) = P_BASIC;
				pline("You can now learn the %s skill.", wpskillname(skillimprove));
			} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
				P_MAX_SKILL(skillimprove) = P_SKILLED;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
				P_MAX_SKILL(skillimprove) = P_EXPERT;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
				P_MAX_SKILL(skillimprove) = P_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
				P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(200) && P_MAX_SKILL(skillimprove) == P_GRAND_MASTER) {
				P_MAX_SKILL(skillimprove) = P_SUPREME_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else pline("Unfortunately, you feel no different than before.");

			skillimprove = randomgoodskill();

			if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
				unrestrict_weapon_skill(skillimprove);
				pline("You can now learn the %s skill.", wpskillname(skillimprove));
			} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
				unrestrict_weapon_skill(skillimprove);
				P_MAX_SKILL(skillimprove) = P_BASIC;
				pline("You can now learn the %s skill.", wpskillname(skillimprove));
			} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
				P_MAX_SKILL(skillimprove) = P_SKILLED;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
				P_MAX_SKILL(skillimprove) = P_EXPERT;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
				P_MAX_SKILL(skillimprove) = P_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
				P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(200) && P_MAX_SKILL(skillimprove) == P_GRAND_MASTER) {
				P_MAX_SKILL(skillimprove) = P_SUPREME_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else pline("Unfortunately, you feel no different than before.");

			skillimprove = randomgoodskill();

			if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
				unrestrict_weapon_skill(skillimprove);
				pline("You can now learn the %s skill.", wpskillname(skillimprove));
			} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
				unrestrict_weapon_skill(skillimprove);
				P_MAX_SKILL(skillimprove) = P_BASIC;
				pline("You can now learn the %s skill.", wpskillname(skillimprove));
			} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
				P_MAX_SKILL(skillimprove) = P_SKILLED;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
				P_MAX_SKILL(skillimprove) = P_EXPERT;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
				P_MAX_SKILL(skillimprove) = P_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
				P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(200) && P_MAX_SKILL(skillimprove) == P_GRAND_MASTER) {
				P_MAX_SKILL(skillimprove) = P_SUPREME_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else pline("Unfortunately, you feel no different than before.");

			skillimprove = randomgoodskill();

			if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
				unrestrict_weapon_skill(skillimprove);
				pline("You can now learn the %s skill.", wpskillname(skillimprove));
			} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
				unrestrict_weapon_skill(skillimprove);
				P_MAX_SKILL(skillimprove) = P_BASIC;
				pline("You can now learn the %s skill.", wpskillname(skillimprove));
			} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
				P_MAX_SKILL(skillimprove) = P_SKILLED;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
				P_MAX_SKILL(skillimprove) = P_EXPERT;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
				P_MAX_SKILL(skillimprove) = P_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
				P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(200) && P_MAX_SKILL(skillimprove) == P_GRAND_MASTER) {
				P_MAX_SKILL(skillimprove) = P_SUPREME_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else pline("Unfortunately, you feel no different than before.");

			skillimprove = randomgoodskill();

			if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
				unrestrict_weapon_skill(skillimprove);
				pline("You can now learn the %s skill.", wpskillname(skillimprove));
			} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
				unrestrict_weapon_skill(skillimprove);
				P_MAX_SKILL(skillimprove) = P_BASIC;
				pline("You can now learn the %s skill.", wpskillname(skillimprove));
			} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
				P_MAX_SKILL(skillimprove) = P_SKILLED;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
				P_MAX_SKILL(skillimprove) = P_EXPERT;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
				P_MAX_SKILL(skillimprove) = P_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
				P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(200) && P_MAX_SKILL(skillimprove) == P_GRAND_MASTER) {
				P_MAX_SKILL(skillimprove) = P_SUPREME_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else pline("Unfortunately, you feel no different than before.");

			skillimprove = randomgoodskill();

			if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
				unrestrict_weapon_skill(skillimprove);
				pline("You can now learn the %s skill.", wpskillname(skillimprove));
			} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
				unrestrict_weapon_skill(skillimprove);
				P_MAX_SKILL(skillimprove) = P_BASIC;
				pline("You can now learn the %s skill.", wpskillname(skillimprove));
			} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
				P_MAX_SKILL(skillimprove) = P_SKILLED;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
				P_MAX_SKILL(skillimprove) = P_EXPERT;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
				P_MAX_SKILL(skillimprove) = P_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
				P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(200) && P_MAX_SKILL(skillimprove) == P_GRAND_MASTER) {
				P_MAX_SKILL(skillimprove) = P_SUPREME_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else pline("Unfortunately, you feel no different than before.");

		}

	} /* DQ slime check */

	if (isdemagogue) { /* this is done here because you could use the recursion effect to circumvent it --Amy */

		if (u.ulevel == 5) {
			MysteriousForceActive |= FROMOUTSIDE;
		}
		if (u.ulevel == 9) {
			UnfairAttackBug |= FROMOUTSIDE;
		}
		if (u.ulevel == 13) {
			HighlevelStatus |= FROMOUTSIDE;
		}
		if (u.ulevel == 17) {
			TrapCreationProblem |= FROMOUTSIDE;
		}
		if (u.ulevel == 21) {
			UndressingEffect |= FROMOUTSIDE;
		}
		if (u.ulevel == 25) {
			OrangeSpells |= FROMOUTSIDE;
		}
		if (u.ulevel == 30) {
			SatanEffect |= FROMOUTSIDE;
		}

	}

	while (Role_if(PM_BINDER) && u.ulevel > u.urmaxlvlC) {

		u.urmaxlvlC++;

		if (!rn2(3)) {
			ubookz = mkobj(SPBOOK_CLASS, FALSE, FALSE);
			if (ubookz) dropy(ubookz);
			pline("A book appeared at your feet!"); }

	}

	while (Role_if(PM_BARD) && u.ulevel > u.urmaxlvlD) {

		u.urmaxlvlD++;
		/* Yes I know, most of the names make no sense. They're from the bard patch. --Amy */

		if (u.urmaxlvlD == 3) {
			ubookz = mksobj(SPE_SLEEP, TRUE, FALSE, FALSE);
			if (ubookz) dropy(ubookz);
			pline("A book of lullaby appeared at your feet!");
		}
		if (u.urmaxlvlD == 4) {
			ubookz = mksobj(SPE_CURE_BLINDNESS, TRUE, FALSE, FALSE);
			if (ubookz) dropy(ubookz);
			pline("A book of cause blindness appeared at your feet!");
		}
		if (u.urmaxlvlD == 5) {
			ubookz = mksobj(SPE_CONFUSE_MONSTER, TRUE, FALSE, FALSE);
			if (ubookz) dropy(ubookz);
			pline("A book of cacophony appeared at your feet!");
		}
		if (u.urmaxlvlD == 6) {
			ubookz = mksobj(SPE_CURE_SICKNESS, TRUE, FALSE, FALSE);
			if (ubookz) dropy(ubookz);
			pline("A book of cause sickness appeared at your feet!");
		}
		if (u.urmaxlvlD == 7) {
			ubookz = mksobj(SPE_SLOW_MONSTER, TRUE, FALSE, FALSE);
			if (ubookz) dropy(ubookz);
			pline("A book of drowsiness appeared at your feet!");
		}
		if (u.urmaxlvlD == 8) {
			ubookz = mksobj(SPE_HASTE_SELF, TRUE, FALSE, FALSE);
			if (ubookz) dropy(ubookz);
			pline("A book of haste pets appeared at your feet!");
		}
		if (u.urmaxlvlD == 9) {
			ubookz = mksobj(RIN_PROTECTION_FROM_SHAPE_CHAN, TRUE, FALSE, FALSE);
			if (ubookz) dropy(ubookz);
			pline("A ring of silence appeared at your feet!");
		}
		if (u.urmaxlvlD == 10) {
			ubookz = mksobj(SPE_CAUSE_FEAR, TRUE, FALSE, FALSE);
			if (ubookz) dropy(ubookz);
			pline("A book of despair appeared at your feet!");
		}
		if (u.urmaxlvlD == 12) {
			ubookz = mksobj(SPE_FORCE_BOLT, TRUE, FALSE, FALSE);
			if (ubookz) dropy(ubookz);
			pline("A book of shatter appeared at your feet!");
		}
		if (u.urmaxlvlD == 14) {
			ubookz = mksobj(CLOAK_OF_DISPLACEMENT, TRUE, FALSE, FALSE);
			if (ubookz) dropy(ubookz);
			pline("A cloak of ventriloquism appeared at your feet!");
		}
		if (u.urmaxlvlD == 15) {
			ubookz = mksobj(SPE_CHARM_MONSTER, TRUE, FALSE, FALSE);
			if (ubookz) dropy(ubookz);
			pline("A book of friendship appeared at your feet!");
		}
		if (u.urmaxlvlD == 20) {
			ubookz = mksobj(SPE_POLYMORPH, TRUE, FALSE, FALSE);
			if (ubookz) dropy(ubookz);
			pline("A book of change appeared at your feet!");
		}

	} /* bard check */

	while (Role_if(PM_ZYBORG) && u.ulevel > u.urmaxlvlB) {

		u.urmaxlvlB++;

		if (!rn2(3)) learnrandomregulartech();

	}

	while (Role_if(PM_ANACHRONOUNBINDER) && u.ulevel > u.urmaxlvlJ) {

		u.urmaxlvlJ++;

		int maxtrainingamount = 0;
		int skillnumber = 0;
		int actualskillselection = 0;
		int amountofpossibleskills = 1;
		int i;

		for (i = 0; i < P_NUM_SKILLS; i++) {
			if (P_SKILL(i) != P_ISRESTRICTED) continue;

			if (P_ADVANCE(i) > 0 && P_ADVANCE(i) >= maxtrainingamount) {
				if (P_ADVANCE(i) > maxtrainingamount) {
					amountofpossibleskills = 1;
					skillnumber = i;
					maxtrainingamount = P_ADVANCE(i);
				} else if (!rn2(amountofpossibleskills + 1)) {
					amountofpossibleskills++;
					skillnumber = i;
				} else {
					amountofpossibleskills++;
				}
			}
		}

		if (skillnumber > 0 && maxtrainingamount > 0) {
			unrestrict_weapon_skill(skillnumber);
			P_MAX_SKILL(skillnumber) = (maxtrainingamount >= 5000 ? P_SUPREME_MASTER : maxtrainingamount >= 500 ? P_GRAND_MASTER : maxtrainingamount >= 50 ? P_MASTER : P_EXPERT);
			pline("You can now learn the %s skill, with a new cap of %s.", wpskillname(skillnumber), maxtrainingamount >= 5000 ? "supreme master" : maxtrainingamount >= 500 ? "grand master" : maxtrainingamount >= 50 ? "master" : "expert");
		} else {
			pline("You've trained no unknown skills since your last level up and therefore you unfortunately don't learn anything new.");
		}

	} /* acu check */

	while (Role_if(PM_MYSTIC) && u.ulevel > u.urmaxlvlH) {

		u.urmaxlvlH++;

		if (!rn2(3)) learnrandomregulartech();

	}

	/* Lorskel wants wild talents to learn random techniques, and I agree that this is supposed to be the case. --Amy */
	while (Role_if(PM_WILD_TALENT) && u.ulevel > u.urmaxlvlG) {

		u.urmaxlvlG++;

		if (!rn2(5)) learnrandomregulartech();

	}

	if (Race_if(PM_PLAYER_SLIME) && !((Deprovement || u.uprops[DEPROVEMENT].extrinsic || have_deprovementstone()) && !(u.ulevel < 10 && !rn2(u.ulevel + 1)) && rn2(10) ) && Role_if(PM_DQ_SLIME) && (u.ulevel > u.urmaxlvlI) ) {

		u.urmaxlvlI++;

		u.uhpmax += rnd(10);
		u.uenmax += rnd(10);

	}

	while (isamerican && Role_if(PM_GLADIATOR) && (u.ulevel > u.urmaxlvlE) ) {

		u.urmaxlvlE++;

		if (!rn2(2)) learnrandomregulartech();

	}

	/* now, set the levels accordingly --Amy */
	if (u.urmaxlvl < u.ulevel) u.urmaxlvl = u.ulevel;
	if (u.urmaxlvlB < u.ulevel) u.urmaxlvlB = u.ulevel;
	if (u.urmaxlvlC < u.ulevel) u.urmaxlvlC = u.ulevel;
	if (u.urmaxlvlD < u.ulevel) u.urmaxlvlD = u.ulevel;
	if (u.urmaxlvlE < u.ulevel) u.urmaxlvlE = u.ulevel;
	if (u.urmaxlvlF < u.ulevel) u.urmaxlvlF = u.ulevel;
	if (u.urmaxlvlG < u.ulevel) u.urmaxlvlG = u.ulevel;
	if (u.urmaxlvlH < u.ulevel) u.urmaxlvlH = u.ulevel;
	if (u.urmaxlvlI < u.ulevel) u.urmaxlvlI = u.ulevel;
	if (u.urmaxlvlJ < u.ulevel) u.urmaxlvlJ = u.ulevel;
	if (u.urmaxlvlK < u.ulevel) u.urmaxlvlK = u.ulevel;
	if (u.urmaxlvlL < u.ulevel) u.urmaxlvlL = u.ulevel;
	if (u.urmaxlvlM < u.ulevel) u.urmaxlvlM = u.ulevel;
	if (u.urmaxlvlUP < u.ulevel) u.urmaxlvlUP = u.ulevel;

}

/*exper.c*/
