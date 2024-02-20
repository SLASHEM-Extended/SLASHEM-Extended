/*	SCCS Id: @(#)minion.c	3.4	2003/01/09	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "emin.h"
#include "epri.h"

void
msummon(mon, ownloc)		/* mon summons a monster */
struct monst *mon;
boolean ownloc; /* TRUE = summon wherever I am (REQUIRES A MONSTER TO EXIST!!!), FALSE = summon at player's location --Amy */
{
	register struct permonst *ptr;
	register int dtype = NON_PM, cnt = 0;
	aligntyp atyp;
	struct monst *mtmp;

	if (mon) {
	    ptr = mon->data;
	    atyp = (ptr->maligntyp==A_NONE) ? A_NONE : sgn(ptr->maligntyp);
	    if (mon->ispriest || mon->data == &mons[PM_ALIGNED_PRIEST] || mon->data == &mons[PM_MASTER_PRIEST] || mon->data == &mons[PM_EXPERIENCED_PRIEST] || mon->data == &mons[PM_EXCEPTIONAL_PRIEST] || mon->data == &mons[PM_ELITE_PRIEST]
		|| mon->data == &mons[PM_ANGEL])
		atyp = EPRI(mon)->shralign;
	} else {
	    ptr = &mons[PM_WIZARD_OF_YENDOR];
	    atyp = (ptr->maligntyp==A_NONE) ? A_NONE : sgn(ptr->maligntyp);
	}
	    
	if (is_dprince(ptr) || (ptr == &mons[PM_WIZARD_OF_YENDOR])) {
	    dtype = (!rn2(20)) ? dprince(atyp) :
				 (!rn2(4)) ? dlord(atyp) : ndemon(atyp);
	    cnt = (!rn2(3) && is_ndemon(&mons[dtype])) ? 3 : 1;
	} else if (is_dlord(ptr)) {
	    dtype = (!rn2(50)) ? dprince(atyp) :
				 (!rn2(20)) ? dlord(atyp) : ndemon(atyp);
	    cnt = (!rn2(4) && is_ndemon(&mons[dtype])) ? 2 : 1;
	} else if (is_ndemon(ptr) || mon->egotype_gator) {
	    dtype = (!rn2(250)) ? dprince(atyp) : (!rn2(20)) ? dlord(atyp) :
				 (!rn2(6) || mon->data == &mons[PM_DEMON_SPOTTER] || mon->data == &mons[PM_FUNK_CAR]) ? ndemon(atyp) : monsndx(ptr);
	    cnt = 1;
	} else if (is_lminion(mon)) {
	    dtype = (is_lord(ptr) && !rn2(20)) ? llord() :
		     (is_lord(ptr) || !rn2(6) || mon->data == &mons[PM_DEMON_SPOTTER] || mon->data == &mons[PM_FUNK_CAR]) ? lminion() : monsndx(ptr);
	    cnt = (!rn2(4) && !is_lord(&mons[dtype])) ? 2 : 1;
	} else if (ptr == &mons[PM_ANGEL]) {
	    /* non-lawful angels can also summon */
	    if (!rn2(6)) {
		switch (atyp) { /* see summon_minion */
		case A_NEUTRAL:
		    dtype = PM_AIR_ELEMENTAL + rn2(5);
		    break;
		case A_CHAOTIC:
		case A_NONE:
		    dtype = ndemon(atyp);
		    break;
		}
	    } else {
		dtype = PM_ANGEL;
	    }
	    cnt = (!rn2(4) && !is_lord(&mons[dtype])) ? 2 : 1;
	}

	if (dtype == NON_PM) return;

	/* sanity checks */
	if (cnt > 1 && (mons[dtype].geno & G_UNIQ)) cnt = 1;
	/*
	 * If this daemon is unique and being re-summoned (the only way we
	 * could get this far with an extinct dtype), try another.
	 */
	if (mvitals[dtype].mvflags & G_GONE) {
	    dtype = ndemon(atyp);
	    if (dtype == NON_PM) return;
	}

	/* Mr. Conclusio can only be summoned if a sizable amount of demon princes have been summoned --Amy
	 * same for Clark Supes and Patient Zero
	 * the counter goes down over time so unless you're obviously farming, he should be impossible to spawn */
	if (dtype == PM_MR__CONCLUSIO && u.conclusiocount < 10) {
	    dtype = ndemon(atyp);
	    if (dtype == NON_PM) return;
	}
	if (dtype == PM_BOOM && u.ulevel < 10) {
	    dtype = ndemon(atyp);
	    if (dtype == NON_PM) return;
	}
	if (dtype == PM_CLARK_SUPES && u.conclusiocount < 10) {
	    dtype = ndemon(atyp);
	    if (dtype == NON_PM) return;
	}
	if (dtype == PM_PATIENT_ZERO && u.conclusiocount < 10) {
	    dtype = ndemon(atyp);
	    if (dtype == NON_PM) return;
	}

	while (cnt > 0) {
	    if (ownloc) mtmp = makemon(&mons[dtype], mon->mx, mon->my, MM_ADJACENTOK);
	    else mtmp = makemon(&mons[dtype], u.ux, u.uy, NO_MM_FLAGS);
	    if (mtmp && (dtype == PM_ANGEL)) {
		/* alignment should match the summoner */
		EPRI(mtmp)->shralign = atyp;
	    }
	    cnt--;

	    u.cnd_demongates++;
	    if (dtype >= PM_JUIBLEX && dtype <= PM_YEENOGHU) u.cnd_demonlordgates++;
	    if (dtype >= PM_ORCUS && dtype <= PM_DEMOGORGON) {
			u.cnd_demonprincegates++;
			u.conclusiocount++;
	    }
	}
}

void
summon_minion(alignment, talk)
aligntyp alignment;
boolean talk;
{
    register struct monst *mon;
    int mnum;

	if (Aggravate_monster) {
		u.aggravation = 1;
		reset_rndmonst(NON_PM);
	}

    switch ((int)alignment) {
	case A_LAWFUL:
	    mnum = lminion();
	    break;
	case A_NEUTRAL:
	    mnum = (rn2(2) ? ntrminion() : PM_AIR_ELEMENTAL + rn2(4));
	    break;
	case A_CHAOTIC:
	case A_NONE:
	    mnum = ndemon(alignment);
	    break;
	default:
	    impossible("unaligned player?");
	    mnum = ndemon(A_NONE);
	    break;
    }
    if (mnum == NON_PM) {
	mon = 0;
    } else if (mons[mnum].pxlth == 0) {
	struct permonst *pm = &mons[mnum];
	mon = makemon(pm, u.ux, u.uy, MM_EMIN);
	if (mon) {
	    mon->isminion = TRUE;
	    EMIN(mon)->min_align = alignment;
	}
    } else if (mnum == PM_ANGEL) {
	mon = makemon(&mons[mnum], u.ux, u.uy, NO_MM_FLAGS);
	if (mon) {
	    mon->isminion = TRUE;
	    EPRI(mon)->shralign = alignment;	/* always A_LAWFUL here */
	}
    } else
	mon = makemon(&mons[mnum], u.ux, u.uy, NO_MM_FLAGS);
    if (mon) {
	if (talk) {
	    pline_The("voice of %s booms:", align_gname(alignment));
	    verbalize("Thou shalt pay for thy indiscretion!");
	    if (!Blind)
		pline("%s appears before you.", Amonnam(mon));
	}
	mon->mpeaceful = FALSE;
	/* don't call set_malign(); player was naughty */
    }

	u.aggravation = 0;

}
#define Athome	(Inhell && !mtmp->cham)

int
demon_talk(mtmp)		/* returns 1 if it won't attack. */
register struct monst *mtmp;
{
	long cash, demand, offer;

	if (uwep && uwep->oartifact == ART_EXCALIBUR) {
	    pline("%s looks very angry.", Amonnam(mtmp));
	    mtmp->mpeaceful = mtmp->mtame = 0;
	    set_malign(mtmp);
	    newsym(mtmp->mx, mtmp->my);
	    return 0;
	}

	if (uwep && uwep->oartifact == ART_DEMONBANE) {
	    pline("%s looks very angry.", Amonnam(mtmp));
	    mtmp->mpeaceful = mtmp->mtame = 0;
	    set_malign(mtmp);
	    newsym(mtmp->mx, mtmp->my);
	    return 0;
	}

	/* Slight advantage given. */
	if (is_dprince(mtmp->data) && mtmp->minvis) {
	    mtmp->minvis = mtmp->perminvis = 0;
	    if (!Blind) pline("%s appears before you.", Amonnam(mtmp));
	    newsym(mtmp->mx,mtmp->my);
	}
	if (youmonst.data->mlet == S_DEMON) {	/* Won't blackmail their own. */
	    pline("%s says, \"Good hunting, %s.\"",
		  Amonnam(mtmp), flags.female ? "Sister" : "Brother");
	    if (!tele_restrict(mtmp)) (void) rloc(mtmp, FALSE);
	    return(1);
	}
/*#ifndef GOLDOBJ
	cash = u.ugold;
#else
	cash = money_cnt(invent);
#endif*/

	/* This isn't _that_ much better than the old way, but it removes
	 * the trivial case of people being able to bribe demons with 
	 * 10 gold pieces to bypass him.  You can still carry lots of gold,
	 * of course, but at least now you have to lug it with you.
	 * Amy edit: if you're a smart game developer like me, you figure out a way for the player to pay with stashed gold
	 * so you no longer accidentally anger the lord just because you forgot to take the stupid gold out first :P
	 * (like, did it really require someone with such a high IQ as myself to have such an obvious idea that should
	 * really be a no-brainer to everyone who has ever interacted with one of those lords????) */

	cash = rnz(15000);
	if (isfriday) cash *= 2;

	demand = (cash * (rnd(80) + 20 * Athome)) /
	    (100 * (1 + (sgn(u.ualign.type) == sgn(mtmp->data->maligntyp))));

	/* always make a demand, even if you have no gold, because you can pay with money in your container --Amy */
	{
		int tenth;
		tenth = u.ugold + hidden_gold();
		if (u.moneydebt) tenth -= u.moneydebt;
		if (u.superdebt) tenth -= u.superdebt;
		if (tenth < 0) tenth = 0;
		if (tenth > (u.ugold + hidden_gold()) ) tenth = 0; /* fail safe for overflows */

	    /* make sure that the demand is unmeetable if the monster
	       has the Amulet, preventing monster from being satisified
	       and removed from the game (along with said Amulet...) */
	    if (mon_has_amulet(mtmp))
		demand = cash + 99999999;

	    pline("%s demands %ld %s for safe passage.",
		  Amonnam(mtmp), demand, currency(demand));

	    /* it's ultra dumb if you have to carry the money out in the open! be able to pay with money in a box! --Amy */
	    if ((u.ugold < demand) && (tenth >= demand)) {
		pline("%s offers you a deal to pay off the demand of %ld %s later when you can afford it.", Amonnam(mtmp), demand, currency(demand) );
		if (yn("Accept the deal?") == 'y') {
			addplayerdebt(demand, TRUE);
			pline("%s vanishes, laughing about cowardly mortals.", Amonnam(mtmp));
			mongone(mtmp);
			return(1);
		}
	    }

	    if ((offer = bribe(mtmp)) >= demand) {
		pline("%s vanishes, laughing about cowardly mortals.",
		      Amonnam(mtmp));
	    } else if (offer > 0L && (long)rnd(40) > (demand - offer)) {
		pline("%s scowls at you menacingly, then vanishes.",
		      Amonnam(mtmp));
	    } else {
		pline("%s gets angry...", Amonnam(mtmp));
		mtmp->mpeaceful = 0;
		set_malign(mtmp);
		return 0;
	    }
	}
	mongone(mtmp);
	return(1);
}

int lawful_minion(int difficulty)
/* this routine returns the # of an appropriate minion,
   given a difficulty rating from 1 to 30 */
	/* These were way too overpowered. So I mixed in some weaker monsters. --Amy */
{
   difficulty = difficulty + rn2(5) - 2;
   if (difficulty < 0) difficulty = 0;
   if (difficulty > 30) difficulty = 30;
   difficulty /= 3;
   if (difficulty >= 1) difficulty = rnd(difficulty);

   if (rn2(2)) return (monsndx(mkclass(S_ANGEL,0)));

   switch (difficulty) {
      case 0: return PM_TENGU;
      case 1: return (rn2(2) ? PM_TENGU : PM_COUATL);
      case 2: return PM_WHITE_UNICORN;
      case 3: return (rn2(4) ? PM_WHITE_UNICORN : PM_ANGEL);
      case 4: return (rn2(5) ? PM_WOODLAND_ELF : PM_MOVANIC_DEVA);
      case 5: return (rn2(6) ? PM_GREEN_ELF : PM_MONADIC_DEVA);
      case 6: return (rn2(7) ? PM_GREY_ELF : PM_KI_RIN);
      case 7: return (rn2(8) ? PM_HIGH_ELF : PM_ASTRAL_DEVA);
      case 8: return (rn2(9) ? PM_ELF_LORD : PM_ARCHON);
      case 9: return (rn2(10) ? PM_ELVENKING : PM_PLANETAR);
      case 10: return (rn2(11) ? PM_BIG_WHITE_UNICORN : PM_SOLAR);

      default: return PM_TENGU;
   }
}

int neutral_minion(int difficulty)
/* this routine returns the # of an appropriate minion,
   given a difficulty rating from 1 to 30 */
{
   if (rn2(2)) return (monsndx(mkclass(S_ELEMENTAL,0)));

   difficulty = difficulty + rn2(9) - 4;
   if (difficulty < 0) difficulty = 0;
   if (difficulty > 30) difficulty = 30;
   if (difficulty >= 1) difficulty = rnd(difficulty);
   if (difficulty < 6) return PM_GRAY_UNICORN;
   if (difficulty < 15) return (PM_AIR_ELEMENTAL+rn2(4));
   return (PM_DJINNI /* +rn2(4) */);
}

int chaotic_minion(int difficulty)
/* this routine returns the # of an appropriate minion,
   given a difficulty rating from 1 to 30 */
{
   difficulty = difficulty + rn2(5) - 2;
   if (difficulty < 0) difficulty = 0;
   if (difficulty > 30) difficulty = 30;
   /* KMH, balance patch -- avoid using floating-point (not supported by all ports) */
/*   difficulty = (int)((float)difficulty / 1.5);*/
   difficulty = (difficulty * 2) / 3;
   if (difficulty >= 1) difficulty = rnd(difficulty);

   if (rn2(2)) return (monsndx(mkclass(S_DEMON,0)));

   switch (difficulty) {
      case 0: return PM_GREMLIN;
      case 1: return PM_DRETCH;
      case 2: return PM_IMP;
      case 3: return PM_BLACK_UNICORN;
      case 4: return PM_BLOOD_IMP;
      case 5: return PM_SPINED_DEVIL;
      case 6: return PM_SHADOW_WOLF;
      case 7: return PM_HELL_HOUND;
      case 8: return PM_HORNED_DEVIL;
      case 9: return PM_BEARDED_DEVIL;
      case 10: return PM_BAR_LGURA;
      case 11: return PM_CHASME;
      case 12: return PM_BARBED_DEVIL;
      case 13: return PM_VROCK;
      case 14: return PM_BABAU;
      case 15: return PM_NALFESHNEE;
      case 16: return PM_MARILITH;
      case 17: return PM_NABASSU;
      case 18: return PM_BONE_DEVIL;
      case 19: return PM_ICE_DEVIL;
      case 20: return PM_PIT_FIEND;
   }
   return PM_GREMLIN;
}

long
bribe(mtmp)
struct monst *mtmp;
{
	char buf[BUFSZ];
	long offer;
#ifdef GOLDOBJ
	long umoney = money_cnt(invent);
#endif

	getlin("How much will you offer?", buf);
	if (sscanf(buf, "%ld", &offer) != 1) offer = 0L;

	/*Michael Paddon -- fix for negative offer to monster*/
	/*JAR880815 - */
	if (offer < 0L) {
		You("try to shortchange %s, but fumble.",
			mon_nam(mtmp));
		return 0L;
	} else if (offer == 0L) {
		You("refuse.");
		return 0L;
#ifndef GOLDOBJ
	} else if (offer >= u.ugold) {
		You("give %s all your gold.", mon_nam(mtmp));
		offer = u.ugold;
	} else {
		You("give %s %ld %s.", mon_nam(mtmp), offer, currency(offer));
	}
	u.ugold -= offer;
	/* Who knows what the demon lord is doing with the money? You're not getting it back at least... --Amy */
	/*mtmp->mgold += offer;*/
#else
	} else if (offer >= umoney) {
		You("give %s all your money.", mon_nam(mtmp));
		offer = umoney;
	} else {
		You("give %s %ld %s.", mon_nam(mtmp), offer, currency(offer));
	}
	u.ugold -= offer;
	/*(void) money2mon(mtmp, offer);*/
#endif
	flags.botl = 1;
	return(offer);
}

int
dprince(atyp)
aligntyp atyp;
{
	int tryct, pm;

	for (tryct = 0; tryct < 20; tryct++) {
	    pm = rn1(PM_DEMOGORGON + 1 - PM_ORCUS, PM_ORCUS);

	    /* make sure Mr. Conclusio/Clark Supes/Patient Zero doesn't spawn early... --Amy */
	    if (pm == PM_MR__CONCLUSIO && u.conclusiocount < 10) return(dlord(atyp));
	    if (pm == PM_BOOM && u.ulevel < 10) return(dlord(atyp));
	    if (pm == PM_CLARK_SUPES && u.conclusiocount < 10) return(dlord(atyp));
	    if (pm == PM_PATIENT_ZERO && u.conclusiocount < 10) return(dlord(atyp));

	    if (!(mvitals[pm].mvflags & G_GONE) &&
		    (atyp == A_NONE || sgn(mons[pm].maligntyp) == sgn(atyp)))
		return(pm);
	}
	return(dlord(atyp));	/* approximate */
}

int
dlord(atyp)
aligntyp atyp;
{
	int tryct, pm;

	for (tryct = 0; tryct < 20; tryct++) {
	    pm = rn1(PM_YEENOGHU + 1 - PM_JUIBLEX, PM_JUIBLEX);
	    if (!(mvitals[pm].mvflags & G_GONE) &&
		    (atyp == A_NONE || sgn(mons[pm].maligntyp) == sgn(atyp)))
		return(pm);
	}
	return(ndemon(atyp));	/* approximate */
}

/* create lawful (good) lord */
int
llord()
{
	if (!(mvitals[PM_ARCHON].mvflags & G_GONE))
		return(PM_ARCHON);

	return(lminion());	/* approximate */
}

int
lminion()
{
	int	tryct;
	struct	permonst *ptr;

	for (tryct = 0; tryct < 20; tryct++) {
	    ptr = mkclass(S_ANGEL,0);
	    if (ptr && !is_lord(ptr))
		return(monsndx(ptr));
	}

	return NON_PM;
}

/* Neutral gods will be more dangerous, with an ability to summon lethe elementals and similar crap. --Amy */
int
ntrminion()
{
	int	tryct;
	struct	permonst *ptr;

	for (tryct = 0; tryct < 20; tryct++) {
	    ptr = mkclass(S_ELEMENTAL,0);
	    if (ptr && !is_lord(ptr))
		return(monsndx(ptr));
	}

	return NON_PM;
}

int
ndemon(atyp)
aligntyp atyp;
{
	int	tryct;
	struct	permonst *ptr;

	for (tryct = 0; tryct < 10000; tryct++) {

		if (!rn2(2000)) reset_rndmonst(NON_PM);

	    ptr = rndmonst();
	    if (ptr && is_ndemon(ptr) &&
		    (atyp == A_NONE || sgn(ptr->maligntyp) == sgn(atyp)))
		return(monsndx(ptr));
	}

	return NON_PM;
}

/*minion.c*/
