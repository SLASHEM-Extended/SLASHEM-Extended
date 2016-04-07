/*	SCCS Id: @(#)exper.c	3.4	2002/11/20	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

/*STATIC_DCL*/ long FDECL(newuexp, (int));
STATIC_DCL int FDECL(enermod, (int));

#define PN_POLEARMS		(-1)
#define PN_SABER		(-2)
#define PN_HAMMER		(-3)
#define PN_WHIP			(-4)
#define PN_PADDLE		(-5)
#define PN_FIREARMS		(-6)
#define PN_ATTACK_SPELL		(-7)
#define PN_HEALING_SPELL	(-8)
#define PN_DIVINATION_SPELL	(-9)
#define PN_ENCHANTMENT_SPELL	(-10)
#define PN_PROTECTION_SPELL	(-11)
#define PN_BODY_SPELL		(-12)
#define PN_MATTER_SPELL		(-13)
#define PN_BARE_HANDED		(-14)
#define PN_MARTIAL_ARTS		(-15)
#define PN_RIDING		(-16)
#define PN_TWO_WEAPONS		(-17)
#ifdef LIGHTSABERS
#define PN_LIGHTSABER		(-18)
#endif

#ifndef OVLB

STATIC_DCL NEARDATA const short skill_names_indices[];
STATIC_DCL NEARDATA const char *odd_skill_names[];

#else	/* OVLB */

/* KMH, balance patch -- updated */
STATIC_OVL NEARDATA const short skill_names_indices[P_NUM_SKILLS] = {
	0,                DAGGER,         KNIFE,        AXE,
	PICK_AXE,         SHORT_SWORD,    BROADSWORD,   LONG_SWORD,
	TWO_HANDED_SWORD, SCIMITAR,       PN_SABER,     CLUB,
	PN_PADDLE,        MACE,           MORNING_STAR,   FLAIL,
	PN_HAMMER,        QUARTERSTAFF,   PN_POLEARMS,  SPEAR,
	JAVELIN,          TRIDENT,        LANCE,        BOW,
	SLING,            PN_FIREARMS,    CROSSBOW,       DART,
	SHURIKEN,         BOOMERANG,      PN_WHIP,      UNICORN_HORN,
#ifdef LIGHTSABERS
	PN_LIGHTSABER,
#endif
	PN_ATTACK_SPELL,     PN_HEALING_SPELL,
	PN_DIVINATION_SPELL, PN_ENCHANTMENT_SPELL,
	PN_PROTECTION_SPELL,            PN_BODY_SPELL,
	PN_MATTER_SPELL,
	PN_BARE_HANDED, 		PN_MARTIAL_ARTS, 
	PN_TWO_WEAPONS,
#ifdef STEED
	PN_RIDING,
#endif
};


STATIC_OVL NEARDATA const char * const odd_skill_names[] = {
    "no skill",
    "polearms",
    "saber",
    "hammer",
    "whip",
    "paddle",
    "firearms",
    "attack spells",
    "healing spells",
    "divination spells",
    "enchantment spells",
    "protection spells",
    "body spells",
    "matter spells",
    "bare-handed combat",
    "martial arts",
    "riding",
    "two-handed combat",
#ifdef LIGHTSABERS
    "lightsaber"
#endif
};

#endif	/* OVLB */

#define P_NAME(type) (skill_names_indices[type] > 0 ? \
		      OBJ_NAME(objects[skill_names_indices[type]]) : \
			odd_skill_names[-skill_names_indices[type]])

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

	if (lev == 1)  return (20L);     /* need 20           */
	if (lev == 2)  return (40L);    /* need 20           */
	if (lev == 3)  return (80L);    /* need 40          */
	if (lev == 4)  return (160L);    /* need 80          */
	if (lev == 5)  return (320L);   /* need 160          */
	if (lev == 6)  return (640L);   /* need 320          */
	if (lev == 7)  return (1280L);   /* need 640         */
	if (lev == 8)  return (2560L);   /* need 1280         */
	if (lev == 9)  return (5000L);   /* need 2440         */
	if (lev == 10) return (10000L);   /* need 5000         */
	if (lev == 11) return (20000L);  /* need 10000         */
	if (lev == 12) return (40000L);  /* need 20000         */
	if (lev == 13) return (80000L);  /* need 40000         */
	if (lev == 14) return (120000L);  /* need 40000         */
	if (lev == 15) return (170000L);  /* need 50000         */
	if (lev == 16) return (220000L);  /* need 50000         */
	if (lev == 17) return (270000L);  /* need 50000         */
	if (lev == 18) return (320000L);  /* need 50000        */
	if (lev == 19) return (380000L);  /* need 60000        */
	if (lev == 20) return (440000L);  /* need 60000        */
	if (lev == 21) return (500000L); /* need 60000        */
	if (lev == 22) return (560000L); /* need 60000        */
	if (lev == 23) return (620000L); /* need 60000        */
	if (lev == 24) return (680000L); /* need 60000        */
	if (lev == 25) return (740000L); /* need 60000       */
	if (lev == 26) return (800000L); /* need 60000       */
	if (lev == 27) return (860000L); /* need 60000       */
	if (lev == 28) return (930000L); /* need 70000       */
	if (lev == 29) return (1000000L); /* need 70000      */
	return (1500000L);

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
	if (lev == 13) return (150000L);
	if (lev == 14) return (250000L);
	if (lev == 15) return (300000L);
	if (lev == 16) return (350000L);
	if (lev == 17) return (400000L);
	if (lev == 18) return (450000L);
	if (lev == 19) return (500000L);
	if (lev == 20) return (550000L);
	if (lev == 21) return (600000L);
	if (lev == 22) return (650000L);
	if (lev == 23) return (700000L);
	if (lev == 24) return (750000L);
	if (lev == 25) return (800000L);
	if (lev == 26) return (850000L);
	if (lev == 27) return (900000L);
	if (lev == 28) return (950000L);
	if (lev == 29) return (1000000L);
	return (1500000L);

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
		tmp += 100;
	}

/*	For certain "extra nasty" monsters, give even more */
	if (extra_nasty(ptr)) tmp += (rnd(7) * mtmp->m_lev);

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

	if (dresistance && Drain_resistance && rn2(5) ) return;

	/* level drain is too strong. Let's nerf it a bit. --Amy */
	if (!force && (u.uexp > 320) && u.ulevel > 1) {
		expdrain = newuexp(u.ulevel) - newuexp(u.ulevel - 1);
		expdrain /= 5;
		expdrain = rnz(expdrain);
		if ((u.uexp - expdrain) > newuexp(u.ulevel - 1)) {
			/* drain some experience, but not enough to make you lose a level */
			pline("You feel your life draining away!");
			u.uexp -= expdrain;
			return;
		}
	}

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
	u.uhpmax -= rn2(3);
	u.uhpmax -= rnz(2);
	u.uhpmax -= rnz(3); /* making the drain for gain exploit much harder to perform --Amy */
	if (u.uhpmax < 1) u.uhpmax = 1;
	u.uhp -= num;
	u.uhp -= rn2(3);
	u.uhp -= rnz(3);
	u.uhp -= rnz(2);
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
	else if (u.uexp >= (1000000 + (100000 * u.xtralevelmult))) {
	    u.xtralevelmult++;
	    u.uexp = 1000000;
	    pline("You feel more experienced.");
	    pluslvl(TRUE);
	}
}

#if 0 /* The old newexplevel() */
{
	register int tmp;
	struct obj *ubook;

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
		u.uhpmax += rn2(3);
		u.uhp += tmp;
		u.uhpmax += rnz(2); /*making the game a bit easier --Amy */
		if (u.uhp < u.uhpmax) u.uhp = u.uhpmax;
		switch (Role_switch) {
			case PM_ARCHEOLOGIST: u.uenbase += rnd(4) + 1; break;
			case PM_BARBARIAN: u.uenbase += rnd(2); break;
			case PM_CAVEMAN: u.uenbase += rnd(2); break;
			/*case PM_DOPPELGANGER: u.uenbase += rnd(5) + 1; break;
			case PM_ELF: case PM_DROW: u.uenbase += rnd(5) + 1; break;*/
			case PM_FLAME_MAGE: u.uenbase += rnd(6) + 2; break;
			case PM_ACID_MAGE: u.uenbase += rnd(6) + 2; break;
			case PM_GNOME: u.uenbase += rnd(3); break;
			case PM_HEALER: u.uenbase += rnd(6) + 2; break;
			case PM_ICE_MAGE: u.uenbase += rnd(6) + 2; break;
			case PM_ELECTRIC_MAGE: u.uenbase += rnd(6) + 2; break;
#ifdef YEOMAN
			case PM_YEOMAN:
#endif
			case PM_KNIGHT: u.uenbase += rnd(3); break;
			/*case PM_HUMAN_WEREWOLF: u.uenbase += rnd(5) + 1; break;*/
			case PM_MONK: u.uenbase += rnd(5) + 1; break;
			case PM_ELPH: u.uenbase += rnd(5) + 1; break;
			case PM_NECROMANCER: u.uenbase += rnd(6) + 2; break;
			case PM_PRIEST: u.uenbase += rnd(6) + 2; break;
			case PM_CHEVALIER: u.uenbase += rnd(6) + 2; break;
			case PM_ROGUE: u.uenbase += rnd(4) + 1; break;
			/*case PM_MAIA: u.uenbase += rnd(4) + 1; break;
			case PM_GASTLY: u.uenbase += rnd(3) + 1; break;*/
			case PM_SAMURAI: u.uenbase += rnd(2); break;
#ifdef TOURIST
			case PM_TOURIST: u.uenbase += rnd(4) + 1; break;
#endif
			case PM_UNDEAD_SLAYER: u.uenbase += rnd(3); break;
			case PM_VALKYRIE: u.uenbase += rnd(2); break;
			case PM_WIZARD: u.uenbase += rnd(6) + 2; break;
			case PM_CONVICT: break;
			/*case PM_ALIEN: break;
			case PM_OGRO: break;
			case PM_KOBOLT: break;
			case PM_TROLLOR: break;
			case PM_GIGANT: break;*/
			case PM_WARRIOR: break;
			case PM_COURIER: break;
			default: u.uenbase += rnd(2) + 1; break;
		}
		if (u.uen < u.uenmax) u.uen = u.uenmax;

		flags.botl = 1;
	}
}
#endif /* old newexplevel() */

void
pluslvl(incr)
boolean incr;	/* true iff via incremental experience growth */
{		/*	(false for potion of gain level)      */
	register int num;
	struct obj *ubookz;

	if (!incr) You_feel("more experienced.");

	if (u.ulevel < MAXULEV) {

	if (!ishomicider || rn2(2)) {	/* homicider only gains hp/pw 50% of the time --Amy */
	/* a.k.a. "bullshit downside that every fun new race gets" (term coined by Khor) */

	num = newhp();
	num += rnz(2);
	if (flags.hybridization) num -= rn2(flags.hybridization + 1);
	if (num < 0) num = 0;
	if (Race_if(PM_SPRIGGAN) && !rn2(2)) num = 0;
	num += rn2(3);
	u.uhpmax += num;
	u.uhp += num;

	if ((u.ulevel >= u.urmaxlvlUP && u.ulevel < 30) && (u.uhp < u.uhpmax)) u.uhp = u.uhpmax;
	if (Upolyd) {
	    num = rnz(8); /* unfortunately will be lost upon unpolymorphing --Amy */
	    if (flags.hybridization) num -= rn2(flags.hybridization + 1);
	    if (num < 0) num = 0;
	    num += rn2(3);
	    u.mhmax += num;
	    u.mh += num;
		if ((u.ulevel >= u.urmaxlvlUP && u.ulevel < 30) && (u.mh < u.mhmax)) u.mh = u.mhmax;
	}
	if (u.ulevel < urole.xlev)
	    num = rn1((int)ACURR(A_WIS)/2 + urole.enadv.lornd + urace.enadv.lornd,
			urole.enadv.lofix + urace.enadv.lofix);
	else
	    num = rn1((int)ACURR(A_WIS)/2 + urole.enadv.hirnd + urace.enadv.hirnd,
			urole.enadv.hifix + urace.enadv.hifix);

	num = enermod(num);	/* M. Stephenson */
	num += (rn2(3) ? rnz(1) : rnz(2));
	if (flags.hybridization) num -= rn2(flags.hybridization + 1);
	if (num < 0) num = 0;
	num += rn2(3);
	u.uenmax += num;
	u.uen += num;
	} /* homicider check */

	} else { /* u.ulevel > MAXULEV */

	if (!ishomicider || rn2(2)) {	/* homicider only gains hp/pw 50% of the time --Amy */
	/* a.k.a. "bullshit downside that every fun new race gets" (term coined by Khor) */

	num = newhp();
	num += rnz(2);
	if (flags.hybridization) num -= rn2(flags.hybridization + 1);
	if (num < 0) num = 0;
	if (num > 1) num /= rnd(12);
	if (Race_if(PM_SPRIGGAN) && !rn2(2)) num = 0;
	num += rn2(2);
	u.uhpmax += num;
	u.uhp += num;

	if (Upolyd) {
	    num = rnz(8); /* unfortunately will be lost upon unpolymorphing --Amy */
	    if (flags.hybridization) num -= rn2(flags.hybridization + 1);
	    if (num < 0) num = 0;
	    if (num > 1) num /= rnd(12);
	    num += rn2(2);
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
	if (flags.hybridization) num -= rn2(flags.hybridization + 1);
	if (num < 0) num = 0;
	if (num > 1) num /= rnd(12);
	num += rn2(2);
	u.uenmax += num;
	u.uen += num;

	} /* homicider check */

	} /* u.ulevel > or < MAXULEV */


	if (u.ulevel >= u.urmaxlvlUP && u.ulevel < 30) u.urmaxlvlUP = (u.ulevel + 1);
	
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

		if (Race_if(PM_RODNEYAN) && u.ulevel > u.urmaxlvl) {

		u.urmaxlvl = u.ulevel;

		if (!rn2(2)) {ubookz = mkobj(SPBOOK_CLASS, FALSE); dropy(ubookz); pline("A book appeared at your feet!"); }

		}

		if (Race_if(PM_ASGARDIAN) && u.ulevel > u.urmaxlvl) {

		u.urmaxlvl = u.ulevel;

		if (!rn2(3)) { switch (rnd(50)) {

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

			default:
				break;

		    }

		  }

		}

		if (Race_if(PM_MISSINGNO) && u.ulevel > u.urmaxlvl) {

		u.urmaxlvl = u.ulevel;

		if (!rn2(3)) { switch (rnd(150)) {

			case 1: 
			case 2: 
			case 3: 
				if (!tech_known(T_BERSERK)) {    	learntech(T_BERSERK, FROMOUTSIDE, 1);
			    	You("learn how to perform berserk!");
				}
				break;
			case 4: 
			case 5: 
			case 6: 
				if (!tech_known(T_KIII)) {    	learntech(T_KIII, FROMOUTSIDE, 1);
			    	You("learn how to perform kiii!");
				}
				break;
			case 7: 
			case 8: 
			case 9: 
				if (!tech_known(T_RESEARCH)) {    	learntech(T_RESEARCH, FROMOUTSIDE, 1);
			    	You("learn how to perform research!");
				}
				break;
			case 10: 
			case 11: 
			case 12: 
				if (!tech_known(T_SURGERY)) {    	learntech(T_SURGERY, FROMOUTSIDE, 1);
			    	You("learn how to perform surgery!");
				}
				break;
			case 13: 
			case 14: 
			case 15: 
				if (!tech_known(T_REINFORCE)) {    	learntech(T_REINFORCE, FROMOUTSIDE, 1);
			    	You("learn how to perform reinforce memory!");
				}
				break;
			case 16: 
			case 17:
			case 18: 
				if (!tech_known(T_FLURRY)) {    	learntech(T_FLURRY, FROMOUTSIDE, 1);
			    	You("learn how to perform missile flurry!");
				}
				break;
			case 19: 
			case 20: 
			case 21: 
				if (!tech_known(T_PRACTICE)) {    	learntech(T_PRACTICE, FROMOUTSIDE, 1);
			    	You("learn how to perform weapon practice!");
				}
				break;
			case 22: 
			case 23: 
			case 24: 
				if (!tech_known(T_EVISCERATE)) {    	learntech(T_EVISCERATE, FROMOUTSIDE, 1);
			    	You("learn how to perform eviscerate!");
				}
				break;
			case 25: 
			case 26: 
			case 27: 
				if (!tech_known(T_HEAL_HANDS)) {    	learntech(T_HEAL_HANDS, FROMOUTSIDE, 1);
			    	You("learn how to perform healing hands!");
				}
				break;
			case 28: 
			case 29: 
			case 30: 
				if (!tech_known(T_CALM_STEED)) {    	learntech(T_CALM_STEED, FROMOUTSIDE, 1);
			    	You("learn how to perform calm steed!");
				}
				break;
			case 31: 
			case 32: 
			case 33: 
				if (!tech_known(T_TURN_UNDEAD)) {    	learntech(T_TURN_UNDEAD, FROMOUTSIDE, 1);
			    	You("learn how to perform turn undead!");
				}
				break;
			case 34: 
			case 35: 
			case 36: 
				if (!tech_known(T_VANISH)) {    	learntech(T_VANISH, FROMOUTSIDE, 1);
			    	You("learn how to perform vanish!");
				}
				break;
			case 37: 
			case 38: 
			case 39: 
				if (!tech_known(T_CUTTHROAT)) {    	learntech(T_CUTTHROAT, FROMOUTSIDE, 1);
			    	You("learn how to perform cutthroat!");
				}
				break;
			case 40: 
			case 41: 
				if (!tech_known(T_BLESSING)) {    	learntech(T_BLESSING, FROMOUTSIDE, 1);
			    	You("learn how to perform blessing!");
				}
				break;
			case 42: 
			case 43: 
			case 44: 
				if (!tech_known(T_E_FIST)) {    	learntech(T_E_FIST, FROMOUTSIDE, 1);
			    	You("learn how to perform elemental fist!");
				}
				break;
			case 45: 
			case 46: 
			case 47: 
				if (!tech_known(T_PRIMAL_ROAR)) {    	learntech(T_PRIMAL_ROAR, FROMOUTSIDE, 1);
			    	You("learn how to perform primal roar!");
				}
				break;
			case 48: 
			case 49: 
				if (!tech_known(T_LIQUID_LEAP)) {    	learntech(T_LIQUID_LEAP, FROMOUTSIDE, 1);
			    	You("learn how to perform liquid leap!");
				}
				break;
			case 50: 
			case 51: 
			case 52: 
				if (!tech_known(T_CRIT_STRIKE)) {    	learntech(T_CRIT_STRIKE, FROMOUTSIDE, 1);
			    	You("learn how to perform critical strike!");
				}
				break;
			case 53: 
			case 54: 
			case 55: 
				if (!tech_known(T_SIGIL_CONTROL)) {    	learntech(T_SIGIL_CONTROL, FROMOUTSIDE, 1);
			    	You("learn how to perform sigil of control!");
				}
				break;
			case 56: 
			case 57: 
			case 58: 
				if (!tech_known(T_SIGIL_TEMPEST)) {    	learntech(T_SIGIL_TEMPEST, FROMOUTSIDE, 1);
			    	You("learn how to perform sigil of tempest!");
				}
				break;
			case 59: 
			case 60: 
			case 61: 
				if (!tech_known(T_SIGIL_DISCHARGE)) {    	learntech(T_SIGIL_DISCHARGE, FROMOUTSIDE, 1);
			    	You("learn how to perform sigil of discharge!");
				}
				break;
			case 62: 
			case 63: 
			case 64: 
				if (!tech_known(T_RAISE_ZOMBIES)) {    	learntech(T_RAISE_ZOMBIES, FROMOUTSIDE, 1);
			    	You("learn how to perform raise zombies!");
				}
				break;
			case 65: 
				if (!tech_known(T_REVIVE)) {    	learntech(T_REVIVE, FROMOUTSIDE, 1);
			    	You("learn how to perform revivification!");
				}
				break;
			case 66: 
			case 67: 
			case 68: 
				if (!tech_known(T_WARD_FIRE)) {    	learntech(T_WARD_FIRE, FROMOUTSIDE, 1);
			    	You("learn how to perform ward against fire!");
				}
				break;
			case 69: 
			case 70: 
			case 71: 
				if (!tech_known(T_WARD_COLD)) {    	learntech(T_WARD_COLD, FROMOUTSIDE, 1);
			    	You("learn how to perform ward against cold!");
				}
				break;
			case 72: 
			case 73: 
			case 74: 
				if (!tech_known(T_WARD_ELEC)) {    	learntech(T_WARD_ELEC, FROMOUTSIDE, 1);
			    	You("learn how to perform ward against electricity!");
				}
				break;
			case 75: 
			case 76: 
			case 77: 
				if (!tech_known(T_TINKER)) {    	learntech(T_TINKER, FROMOUTSIDE, 1);
			    	You("learn how to perform tinker!");
				}
				break;
			case 78: 
			case 79: 
			case 80: 
				if (!tech_known(T_RAGE)) {    	learntech(T_RAGE, FROMOUTSIDE, 1);
			    	You("learn how to perform rage eruption!");
				}
				break;
			case 81: 
			case 82: 
			case 83: 
				if (!tech_known(T_BLINK)) {    	learntech(T_BLINK, FROMOUTSIDE, 1);
			    	You("learn how to perform blink!");
				}
				break;
			case 84: 
			case 85: 
			case 86: 
				if (!tech_known(T_CHI_STRIKE)) {    	learntech(T_CHI_STRIKE, FROMOUTSIDE, 1);
			    	You("learn how to perform chi strike!");
				}
				break;
			case 87: 
			case 88: 
			case 89: 
				if (!tech_known(T_DRAW_ENERGY)) {    	learntech(T_DRAW_ENERGY, FROMOUTSIDE, 1);
			    	You("learn how to perform draw energy!");
				}
				break;
			case 90: 
			case 91: 
			case 92: 
				if (!tech_known(T_CHI_HEALING)) {    	learntech(T_CHI_HEALING, FROMOUTSIDE, 1);
			    	You("learn how to perform chi healing!");
				}
				break;
			case 93: 
			case 94: 
			case 95: 
				if (!tech_known(T_DAZZLE)) {    	learntech(T_DAZZLE, FROMOUTSIDE, 1);
			    	You("learn how to perform dazzle!");
				}
				break;
			case 96: 
			case 97: 
			case 98: 
				if (!tech_known(T_BLITZ)) {    	learntech(T_BLITZ, FROMOUTSIDE, 1);
			    	You("learn how to perform chained blitz!");
				}
				break;
			case 99: 
			case 100: 
			case 101: 
				if (!tech_known(T_PUMMEL)) {    	learntech(T_PUMMEL, FROMOUTSIDE, 1);
			    	You("learn how to perform pummel!");
				}
				break;
			case 102: 
			case 103: 
			case 104: 
				if (!tech_known(T_G_SLAM)) {    	learntech(T_G_SLAM, FROMOUTSIDE, 1);
			    	You("learn how to perform ground slam!");
				}
				break;
			case 105: 
			case 106: 
			case 107: 
				if (!tech_known(T_DASH)) {    	learntech(T_DASH, FROMOUTSIDE, 1);
			    	You("learn how to perform air dash!");
				}
				break;
			case 108: 
			case 109: 
			case 110: 
				if (!tech_known(T_POWER_SURGE)) {    	learntech(T_POWER_SURGE, FROMOUTSIDE, 1);
			    	You("learn how to perform power surge!");
				}
				break;
			case 111: 
			case 112: 
			case 113: 
				if (!tech_known(T_SPIRIT_BOMB)) {    	learntech(T_SPIRIT_BOMB, FROMOUTSIDE, 1);
			    	You("learn how to perform spirit bomb!");
				}
				break;
			case 114: 
			case 115: 
			case 116: 
				if (!tech_known(T_DRAW_BLOOD)) {    	learntech(T_DRAW_BLOOD, FROMOUTSIDE, 1);
			    	You("learn how to perform draw blood!");
				}
				break;
			case 117: 
				if (!tech_known(T_WORLD_FALL)) {    	learntech(T_WORLD_FALL, FROMOUTSIDE, 1);
			    	You("learn how to perform world fall!");
				}
				break;
			case 118: 
			case 119: 
			case 120: 
				if (!tech_known(T_CREATE_AMMO)) {    	learntech(T_CREATE_AMMO, FROMOUTSIDE, 1);
			    	You("learn how to perform create ammo!");
				}
				break;
			case 121: 
			case 122: 
			case 123: 
				if (!tech_known(T_POKE_BALL)) {    	learntech(T_POKE_BALL, FROMOUTSIDE, 1);
			    	You("learn how to perform poke ball!");
				}
				break;
			case 124: 
			case 125: 
			case 126: 
				if (!tech_known(T_ATTIRE_CHARM)) {    	learntech(T_ATTIRE_CHARM, FROMOUTSIDE, 1);
			    	You("learn how to perform attire charm!");
				}
				break;
			case 127: 
			case 128: 
			case 129: 
				if (!tech_known(T_SUMMON_TEAM_ANT)) {    	learntech(T_SUMMON_TEAM_ANT, FROMOUTSIDE, 1);
			    	You("learn how to perform summon team ant!");
				}
				break;
			case 130: 
			case 131: 
			case 132: 
				if (!tech_known(T_JEDI_JUMP)) {    	learntech(T_JEDI_JUMP, FROMOUTSIDE, 1);
			    	You("learn how to perform jedi jump!");
				}
				break;
			case 133: 
			case 134: 
			case 135: 
				if (!tech_known(T_CHARGE_SABER)) {    	learntech(T_CHARGE_SABER, FROMOUTSIDE, 1);
			    	You("learn how to perform charge saber!");
				}
				break;
			case 136: 
			case 137: 
			case 138: 
				if (!tech_known(T_TELEKINESIS)) {    	learntech(T_TELEKINESIS, FROMOUTSIDE, 1);
			    	You("learn how to perform telekinesis!");
				}
				break;
			case 139: 
				if (!tech_known(T_EGG_BOMB)) {    	learntech(T_EGG_BOMB, FROMOUTSIDE, 1);
			    	You("learn how to perform egg bomb!");
				}
				break;
			case 140: 
			case 141: 
			case 142: 
				if (!tech_known(T_BOOZE)) {    	learntech(T_BOOZE, FROMOUTSIDE, 1);
			    	You("learn how to perform booze!");
				}
				break;

			default:
				break;

		    }

		  }
		}

		if (Role_if(PM_DQ_SLIME) && u.ulevel > u.urmaxlvlC) {

		u.urmaxlvlC = u.ulevel;

			int skillimprove = rnd(P_NUM_SKILLS);

			if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
				unrestrict_weapon_skill(skillimprove);
				pline("You can now learn the %s skill.", P_NAME(skillimprove));
			} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
				unrestrict_weapon_skill(skillimprove);
				P_MAX_SKILL(skillimprove) = P_BASIC;
				pline("You can now learn the %s skill.", P_NAME(skillimprove));
			} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
				P_MAX_SKILL(skillimprove) = P_SKILLED;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
				P_MAX_SKILL(skillimprove) = P_EXPERT;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
				P_MAX_SKILL(skillimprove) = P_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
				P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else pline("Unfortunately, you feel no different than before.");

		if (u.urmaxlvlC >= 10) {

			int skillimprove = rnd(P_NUM_SKILLS);

			if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
				unrestrict_weapon_skill(skillimprove);
				pline("You can now learn the %s skill.", P_NAME(skillimprove));
			} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
				unrestrict_weapon_skill(skillimprove);
				P_MAX_SKILL(skillimprove) = P_BASIC;
				pline("You can now learn the %s skill.", P_NAME(skillimprove));
			} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
				P_MAX_SKILL(skillimprove) = P_SKILLED;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
				P_MAX_SKILL(skillimprove) = P_EXPERT;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
				P_MAX_SKILL(skillimprove) = P_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
				P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else pline("Unfortunately, you feel no different than before.");

		}

		if (u.urmaxlvlC >= 20) {

			int skillimprove = rnd(P_NUM_SKILLS);

			if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
				unrestrict_weapon_skill(skillimprove);
				pline("You can now learn the %s skill.", P_NAME(skillimprove));
			} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
				unrestrict_weapon_skill(skillimprove);
				P_MAX_SKILL(skillimprove) = P_BASIC;
				pline("You can now learn the %s skill.", P_NAME(skillimprove));
			} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
				P_MAX_SKILL(skillimprove) = P_SKILLED;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
				P_MAX_SKILL(skillimprove) = P_EXPERT;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
				P_MAX_SKILL(skillimprove) = P_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
				P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else pline("Unfortunately, you feel no different than before.");

		}

		if (u.urmaxlvlC >= 30) {

			int skillimprove = rnd(P_NUM_SKILLS);

			if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
				unrestrict_weapon_skill(skillimprove);
				pline("You can now learn the %s skill.", P_NAME(skillimprove));
			} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
				unrestrict_weapon_skill(skillimprove);
				P_MAX_SKILL(skillimprove) = P_BASIC;
				pline("You can now learn the %s skill.", P_NAME(skillimprove));
			} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
				P_MAX_SKILL(skillimprove) = P_SKILLED;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
				P_MAX_SKILL(skillimprove) = P_EXPERT;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
				P_MAX_SKILL(skillimprove) = P_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
				P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else pline("Unfortunately, you feel no different than before.");

			skillimprove = rnd(P_NUM_SKILLS);

			if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
				unrestrict_weapon_skill(skillimprove);
				pline("You can now learn the %s skill.", P_NAME(skillimprove));
			} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
				unrestrict_weapon_skill(skillimprove);
				P_MAX_SKILL(skillimprove) = P_BASIC;
				pline("You can now learn the %s skill.", P_NAME(skillimprove));
			} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
				P_MAX_SKILL(skillimprove) = P_SKILLED;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
				P_MAX_SKILL(skillimprove) = P_EXPERT;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
				P_MAX_SKILL(skillimprove) = P_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
				P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else pline("Unfortunately, you feel no different than before.");

			skillimprove = rnd(P_NUM_SKILLS);

			if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
				unrestrict_weapon_skill(skillimprove);
				pline("You can now learn the %s skill.", P_NAME(skillimprove));
			} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
				unrestrict_weapon_skill(skillimprove);
				P_MAX_SKILL(skillimprove) = P_BASIC;
				pline("You can now learn the %s skill.", P_NAME(skillimprove));
			} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
				P_MAX_SKILL(skillimprove) = P_SKILLED;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
				P_MAX_SKILL(skillimprove) = P_EXPERT;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
				P_MAX_SKILL(skillimprove) = P_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
				P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else pline("Unfortunately, you feel no different than before.");

			skillimprove = rnd(P_NUM_SKILLS);

			if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
				unrestrict_weapon_skill(skillimprove);
				pline("You can now learn the %s skill.", P_NAME(skillimprove));
			} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
				unrestrict_weapon_skill(skillimprove);
				P_MAX_SKILL(skillimprove) = P_BASIC;
				pline("You can now learn the %s skill.", P_NAME(skillimprove));
			} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
				P_MAX_SKILL(skillimprove) = P_SKILLED;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
				P_MAX_SKILL(skillimprove) = P_EXPERT;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
				P_MAX_SKILL(skillimprove) = P_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
				P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else pline("Unfortunately, you feel no different than before.");

			skillimprove = rnd(P_NUM_SKILLS);

			if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
				unrestrict_weapon_skill(skillimprove);
				pline("You can now learn the %s skill.", P_NAME(skillimprove));
			} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
				unrestrict_weapon_skill(skillimprove);
				P_MAX_SKILL(skillimprove) = P_BASIC;
				pline("You can now learn the %s skill.", P_NAME(skillimprove));
			} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
				P_MAX_SKILL(skillimprove) = P_SKILLED;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
				P_MAX_SKILL(skillimprove) = P_EXPERT;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
				P_MAX_SKILL(skillimprove) = P_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
				P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else pline("Unfortunately, you feel no different than before.");

			skillimprove = rnd(P_NUM_SKILLS);

			if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
				unrestrict_weapon_skill(skillimprove);
				pline("You can now learn the %s skill.", P_NAME(skillimprove));
			} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
				unrestrict_weapon_skill(skillimprove);
				P_MAX_SKILL(skillimprove) = P_BASIC;
				pline("You can now learn the %s skill.", P_NAME(skillimprove));
			} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
				P_MAX_SKILL(skillimprove) = P_SKILLED;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
				P_MAX_SKILL(skillimprove) = P_EXPERT;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
				P_MAX_SKILL(skillimprove) = P_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
				P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else pline("Unfortunately, you feel no different than before.");

			skillimprove = rnd(P_NUM_SKILLS);

			if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
				unrestrict_weapon_skill(skillimprove);
				pline("You can now learn the %s skill.", P_NAME(skillimprove));
			} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
				unrestrict_weapon_skill(skillimprove);
				P_MAX_SKILL(skillimprove) = P_BASIC;
				pline("You can now learn the %s skill.", P_NAME(skillimprove));
			} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
				P_MAX_SKILL(skillimprove) = P_SKILLED;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
				P_MAX_SKILL(skillimprove) = P_EXPERT;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
				P_MAX_SKILL(skillimprove) = P_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
				P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else pline("Unfortunately, you feel no different than before.");

		}

		}

		if (Role_if(PM_BINDER) && u.ulevel > u.urmaxlvlC) {

		u.urmaxlvlC = u.ulevel;

		if (!rn2(3)) {ubookz = mkobj(SPBOOK_CLASS, FALSE); dropy(ubookz); pline("A book appeared at your feet!"); }

		}

		if (Role_if(PM_BARD) && u.ulevel > u.urmaxlvlD) {

		u.urmaxlvlD = u.ulevel;
		/* Yes I know, most of the names make no sense. They're from the bard patch. --Amy */

		if (u.urmaxlvlD == 3) {ubookz = mksobj(SPE_SLEEP, TRUE, FALSE); dropy(ubookz); pline("A book of lullaby appeared at your feet!"); }
		if (u.urmaxlvlD == 4) {ubookz = mksobj(SPE_CURE_BLINDNESS, TRUE, FALSE); dropy(ubookz); pline("A book of cause blindness appeared at your feet!"); }
		if (u.urmaxlvlD == 5) {ubookz = mksobj(SPE_CONFUSE_MONSTER, TRUE, FALSE); dropy(ubookz); pline("A book of cacophony appeared at your feet!"); }
		if (u.urmaxlvlD == 6) {ubookz = mksobj(SPE_CURE_SICKNESS, TRUE, FALSE); dropy(ubookz); pline("A book of cause sickness appeared at your feet!"); }
		if (u.urmaxlvlD == 7) {ubookz = mksobj(SPE_SLOW_MONSTER, TRUE, FALSE); dropy(ubookz); pline("A book of drowsiness appeared at your feet!"); }
		if (u.urmaxlvlD == 8) {ubookz = mksobj(SPE_HASTE_SELF, TRUE, FALSE); dropy(ubookz); pline("A book of haste pets appeared at your feet!"); }
		if (u.urmaxlvlD == 9) {ubookz = mksobj(RIN_PROTECTION_FROM_SHAPE_CHAN, TRUE, FALSE); dropy(ubookz); pline("A ring of silence appeared at your feet!"); }
		if (u.urmaxlvlD == 10) {ubookz = mksobj(SPE_CAUSE_FEAR, TRUE, FALSE); dropy(ubookz); pline("A book of despair appeared at your feet!"); }
		if (u.urmaxlvlD == 12) {ubookz = mksobj(SPE_FORCE_BOLT, TRUE, FALSE); dropy(ubookz); pline("A book of shatter appeared at your feet!"); }
		if (u.urmaxlvlD == 14) {ubookz = mksobj(CLOAK_OF_DISPLACEMENT, TRUE, FALSE); dropy(ubookz); pline("A cloak of ventriloquism appeared at your feet!"); }
		if (u.urmaxlvlD == 15) {ubookz = mksobj(SPE_CHARM_MONSTER, TRUE, FALSE); dropy(ubookz); pline("A book of friendship appeared at your feet!"); }
		if (u.urmaxlvlD == 20) {ubookz = mksobj(SPE_POLYMORPH, TRUE, FALSE); dropy(ubookz); pline("A book of change appeared at your feet!"); }

		}

		if ((Role_if(PM_ZYBORG) || Role_if(PM_MYSTIC)) && u.ulevel > u.urmaxlvlB) {

		u.urmaxlvlB = u.ulevel;

		if (!rn2(3)) { switch (rnd(150)) {

			case 1: 
			case 2: 
			case 3: 
				if (!tech_known(T_BERSERK)) {    	learntech(T_BERSERK, FROMOUTSIDE, 1);
			    	You("learn how to perform berserk!");
				}
				break;
			case 4: 
			case 5: 
			case 6: 
				if (!tech_known(T_KIII)) {    	learntech(T_KIII, FROMOUTSIDE, 1);
			    	You("learn how to perform kiii!");
				}
				break;
			case 7: 
			case 8: 
			case 9: 
				if (!tech_known(T_RESEARCH)) {    	learntech(T_RESEARCH, FROMOUTSIDE, 1);
			    	You("learn how to perform research!");
				}
				break;
			case 10: 
			case 11: 
			case 12: 
				if (!tech_known(T_SURGERY)) {    	learntech(T_SURGERY, FROMOUTSIDE, 1);
			    	You("learn how to perform surgery!");
				}
				break;
			case 13: 
			case 14: 
			case 15: 
				if (!tech_known(T_REINFORCE)) {    	learntech(T_REINFORCE, FROMOUTSIDE, 1);
			    	You("learn how to perform reinforce memory!");
				}
				break;
			case 16: 
			case 17:
			case 18: 
				if (!tech_known(T_FLURRY)) {    	learntech(T_FLURRY, FROMOUTSIDE, 1);
			    	You("learn how to perform missile flurry!");
				}
				break;
			case 19: 
			case 20: 
			case 21: 
				if (!tech_known(T_PRACTICE)) {    	learntech(T_PRACTICE, FROMOUTSIDE, 1);
			    	You("learn how to perform weapon practice!");
				}
				break;
			case 22: 
			case 23: 
			case 24: 
				if (!tech_known(T_EVISCERATE)) {    	learntech(T_EVISCERATE, FROMOUTSIDE, 1);
			    	You("learn how to perform eviscerate!");
				}
				break;
			case 25: 
			case 26: 
			case 27: 
				if (!tech_known(T_HEAL_HANDS)) {    	learntech(T_HEAL_HANDS, FROMOUTSIDE, 1);
			    	You("learn how to perform healing hands!");
				}
				break;
			case 28: 
			case 29: 
			case 30: 
				if (!tech_known(T_CALM_STEED)) {    	learntech(T_CALM_STEED, FROMOUTSIDE, 1);
			    	You("learn how to perform calm steed!");
				}
				break;
			case 31: 
			case 32: 
			case 33: 
				if (!tech_known(T_TURN_UNDEAD)) {    	learntech(T_TURN_UNDEAD, FROMOUTSIDE, 1);
			    	You("learn how to perform turn undead!");
				}
				break;
			case 34: 
			case 35: 
			case 36: 
				if (!tech_known(T_VANISH)) {    	learntech(T_VANISH, FROMOUTSIDE, 1);
			    	You("learn how to perform vanish!");
				}
				break;
			case 37: 
			case 38: 
			case 39: 
				if (!tech_known(T_CUTTHROAT)) {    	learntech(T_CUTTHROAT, FROMOUTSIDE, 1);
			    	You("learn how to perform cutthroat!");
				}
				break;
			case 40: 
			case 41: 
				if (!tech_known(T_BLESSING)) {    	learntech(T_BLESSING, FROMOUTSIDE, 1);
			    	You("learn how to perform blessing!");
				}
				break;
			case 42: 
			case 43: 
			case 44: 
				if (!tech_known(T_E_FIST)) {    	learntech(T_E_FIST, FROMOUTSIDE, 1);
			    	You("learn how to perform elemental fist!");
				}
				break;
			case 45: 
			case 46: 
			case 47: 
				if (!tech_known(T_PRIMAL_ROAR)) {    	learntech(T_PRIMAL_ROAR, FROMOUTSIDE, 1);
			    	You("learn how to perform primal roar!");
				}
				break;
			case 48: 
			case 49: 
				if (!tech_known(T_LIQUID_LEAP)) {    	learntech(T_LIQUID_LEAP, FROMOUTSIDE, 1);
			    	You("learn how to perform liquid leap!");
				}
				break;
			case 50: 
			case 51: 
			case 52: 
				if (!tech_known(T_CRIT_STRIKE)) {    	learntech(T_CRIT_STRIKE, FROMOUTSIDE, 1);
			    	You("learn how to perform critical strike!");
				}
				break;
			case 53: 
			case 54: 
			case 55: 
				if (!tech_known(T_SIGIL_CONTROL)) {    	learntech(T_SIGIL_CONTROL, FROMOUTSIDE, 1);
			    	You("learn how to perform sigil of control!");
				}
				break;
			case 56: 
			case 57: 
			case 58: 
				if (!tech_known(T_SIGIL_TEMPEST)) {    	learntech(T_SIGIL_TEMPEST, FROMOUTSIDE, 1);
			    	You("learn how to perform sigil of tempest!");
				}
				break;
			case 59: 
			case 60: 
			case 61: 
				if (!tech_known(T_SIGIL_DISCHARGE)) {    	learntech(T_SIGIL_DISCHARGE, FROMOUTSIDE, 1);
			    	You("learn how to perform sigil of discharge!");
				}
				break;
			case 62: 
			case 63: 
			case 64: 
				if (!tech_known(T_RAISE_ZOMBIES)) {    	learntech(T_RAISE_ZOMBIES, FROMOUTSIDE, 1);
			    	You("learn how to perform raise zombies!");
				}
				break;
			case 65: 
				if (!tech_known(T_REVIVE)) {    	learntech(T_REVIVE, FROMOUTSIDE, 1);
			    	You("learn how to perform revivification!");
				}
				break;
			case 66: 
			case 67: 
			case 68: 
				if (!tech_known(T_WARD_FIRE)) {    	learntech(T_WARD_FIRE, FROMOUTSIDE, 1);
			    	You("learn how to perform ward against fire!");
				}
				break;
			case 69: 
			case 70: 
			case 71: 
				if (!tech_known(T_WARD_COLD)) {    	learntech(T_WARD_COLD, FROMOUTSIDE, 1);
			    	You("learn how to perform ward against cold!");
				}
				break;
			case 72: 
			case 73: 
			case 74: 
				if (!tech_known(T_WARD_ELEC)) {    	learntech(T_WARD_ELEC, FROMOUTSIDE, 1);
			    	You("learn how to perform ward against electricity!");
				}
				break;
			case 75: 
			case 76: 
			case 77: 
				if (!tech_known(T_TINKER)) {    	learntech(T_TINKER, FROMOUTSIDE, 1);
			    	You("learn how to perform tinker!");
				}
				break;
			case 78: 
			case 79: 
			case 80: 
				if (!tech_known(T_RAGE)) {    	learntech(T_RAGE, FROMOUTSIDE, 1);
			    	You("learn how to perform rage eruption!");
				}
				break;
			case 81: 
			case 82: 
			case 83: 
				if (!tech_known(T_BLINK)) {    	learntech(T_BLINK, FROMOUTSIDE, 1);
			    	You("learn how to perform blink!");
				}
				break;
			case 84: 
			case 85: 
			case 86: 
				if (!tech_known(T_CHI_STRIKE)) {    	learntech(T_CHI_STRIKE, FROMOUTSIDE, 1);
			    	You("learn how to perform chi strike!");
				}
				break;
			case 87: 
			case 88: 
			case 89: 
				if (!tech_known(T_DRAW_ENERGY)) {    	learntech(T_DRAW_ENERGY, FROMOUTSIDE, 1);
			    	You("learn how to perform draw energy!");
				}
				break;
			case 90: 
			case 91: 
			case 92: 
				if (!tech_known(T_CHI_HEALING)) {    	learntech(T_CHI_HEALING, FROMOUTSIDE, 1);
			    	You("learn how to perform chi healing!");
				}
				break;
			case 93: 
			case 94: 
			case 95: 
				if (!tech_known(T_DAZZLE)) {    	learntech(T_DAZZLE, FROMOUTSIDE, 1);
			    	You("learn how to perform dazzle!");
				}
				break;
			case 96: 
			case 97: 
			case 98: 
				if (!tech_known(T_BLITZ)) {    	learntech(T_BLITZ, FROMOUTSIDE, 1);
			    	You("learn how to perform chained blitz!");
				}
				break;
			case 99: 
			case 100: 
			case 101: 
				if (!tech_known(T_PUMMEL)) {    	learntech(T_PUMMEL, FROMOUTSIDE, 1);
			    	You("learn how to perform pummel!");
				}
				break;
			case 102: 
			case 103: 
			case 104: 
				if (!tech_known(T_G_SLAM)) {    	learntech(T_G_SLAM, FROMOUTSIDE, 1);
			    	You("learn how to perform ground slam!");
				}
				break;
			case 105: 
			case 106: 
			case 107: 
				if (!tech_known(T_DASH)) {    	learntech(T_DASH, FROMOUTSIDE, 1);
			    	You("learn how to perform air dash!");
				}
				break;
			case 108: 
			case 109: 
			case 110: 
				if (!tech_known(T_POWER_SURGE)) {    	learntech(T_POWER_SURGE, FROMOUTSIDE, 1);
			    	You("learn how to perform power surge!");
				}
				break;
			case 111: 
			case 112: 
			case 113: 
				if (!tech_known(T_SPIRIT_BOMB)) {    	learntech(T_SPIRIT_BOMB, FROMOUTSIDE, 1);
			    	You("learn how to perform spirit bomb!");
				}
				break;
			case 114: 
			case 115: 
			case 116: 
				if (!tech_known(T_DRAW_BLOOD)) {    	learntech(T_DRAW_BLOOD, FROMOUTSIDE, 1);
			    	You("learn how to perform draw blood!");
				}
				break;
			case 117: 
				if (!tech_known(T_WORLD_FALL)) {    	learntech(T_WORLD_FALL, FROMOUTSIDE, 1);
			    	You("learn how to perform world fall!");
				}
				break;
			case 118: 
			case 119: 
			case 120: 
				if (!tech_known(T_CREATE_AMMO)) {    	learntech(T_CREATE_AMMO, FROMOUTSIDE, 1);
			    	You("learn how to perform create ammo!");
				}
				break;
			case 121: 
			case 122: 
			case 123: 
				if (!tech_known(T_POKE_BALL)) {    	learntech(T_POKE_BALL, FROMOUTSIDE, 1);
			    	You("learn how to perform poke ball!");
				}
				break;
			case 124: 
			case 125: 
			case 126: 
				if (!tech_known(T_ATTIRE_CHARM)) {    	learntech(T_ATTIRE_CHARM, FROMOUTSIDE, 1);
			    	You("learn how to perform attire charm!");
				}
				break;
			case 127: 
			case 128: 
			case 129: 
				if (!tech_known(T_SUMMON_TEAM_ANT)) {    	learntech(T_SUMMON_TEAM_ANT, FROMOUTSIDE, 1);
			    	You("learn how to perform summon team ant!");
				}
				break;
			case 130: 
			case 131: 
			case 132: 
				if (!tech_known(T_JEDI_JUMP)) {    	learntech(T_JEDI_JUMP, FROMOUTSIDE, 1);
			    	You("learn how to perform jedi jump!");
				}
				break;
			case 133: 
			case 134: 
			case 135: 
				if (!tech_known(T_CHARGE_SABER)) {    	learntech(T_CHARGE_SABER, FROMOUTSIDE, 1);
			    	You("learn how to perform charge saber!");
				}
				break;
			case 136: 
			case 137: 
			case 138: 
				if (!tech_known(T_TELEKINESIS)) {    	learntech(T_TELEKINESIS, FROMOUTSIDE, 1);
			    	You("learn how to perform telekinesis!");
				}
				break;
			case 139: 
				if (!tech_known(T_EGG_BOMB)) {    	learntech(T_EGG_BOMB, FROMOUTSIDE, 1);
			    	You("learn how to perform egg bomb!");
				}
				break;
			case 140: 
			case 141: 
			case 142: 
				if (!tech_known(T_BOOZE)) {    	learntech(T_BOOZE, FROMOUTSIDE, 1);
			    	You("learn how to perform booze!");
				}
				break;


			default:
				break;

		      }

		    }

		}

		if (isamerican && Role_if(PM_GLADIATOR) && (u.ulevel > u.urmaxlvlE) ) {

		u.urmaxlvlE = u.ulevel;

		if (!rn2(2)) { switch (rnd(150)) {

			case 1: 
			case 2: 
			case 3: 
				if (!tech_known(T_BERSERK)) {    	learntech(T_BERSERK, FROMOUTSIDE, 1);
			    	You("learn how to perform berserk!");
				}
				break;
			case 4: 
			case 5: 
			case 6: 
				if (!tech_known(T_KIII)) {    	learntech(T_KIII, FROMOUTSIDE, 1);
			    	You("learn how to perform kiii!");
				}
				break;
			case 7: 
			case 8: 
			case 9: 
				if (!tech_known(T_RESEARCH)) {    	learntech(T_RESEARCH, FROMOUTSIDE, 1);
			    	You("learn how to perform research!");
				}
				break;
			case 10: 
			case 11: 
			case 12: 
				if (!tech_known(T_SURGERY)) {    	learntech(T_SURGERY, FROMOUTSIDE, 1);
			    	You("learn how to perform surgery!");
				}
				break;
			case 13: 
			case 14: 
			case 15: 
				if (!tech_known(T_REINFORCE)) {    	learntech(T_REINFORCE, FROMOUTSIDE, 1);
			    	You("learn how to perform reinforce memory!");
				}
				break;
			case 16: 
			case 17:
			case 18: 
				if (!tech_known(T_FLURRY)) {    	learntech(T_FLURRY, FROMOUTSIDE, 1);
			    	You("learn how to perform missile flurry!");
				}
				break;
			case 19: 
			case 20: 
			case 21: 
				if (!tech_known(T_PRACTICE)) {    	learntech(T_PRACTICE, FROMOUTSIDE, 1);
			    	You("learn how to perform weapon practice!");
				}
				break;
			case 22: 
			case 23: 
			case 24: 
				if (!tech_known(T_EVISCERATE)) {    	learntech(T_EVISCERATE, FROMOUTSIDE, 1);
			    	You("learn how to perform eviscerate!");
				}
				break;
			case 25: 
			case 26: 
			case 27: 
				if (!tech_known(T_HEAL_HANDS)) {    	learntech(T_HEAL_HANDS, FROMOUTSIDE, 1);
			    	You("learn how to perform healing hands!");
				}
				break;
			case 28: 
			case 29: 
			case 30: 
				if (!tech_known(T_CALM_STEED)) {    	learntech(T_CALM_STEED, FROMOUTSIDE, 1);
			    	You("learn how to perform calm steed!");
				}
				break;
			case 31: 
			case 32: 
			case 33: 
				if (!tech_known(T_TURN_UNDEAD)) {    	learntech(T_TURN_UNDEAD, FROMOUTSIDE, 1);
			    	You("learn how to perform turn undead!");
				}
				break;
			case 34: 
			case 35: 
			case 36: 
				if (!tech_known(T_VANISH)) {    	learntech(T_VANISH, FROMOUTSIDE, 1);
			    	You("learn how to perform vanish!");
				}
				break;
			case 37: 
			case 38: 
			case 39: 
				if (!tech_known(T_CUTTHROAT)) {    	learntech(T_CUTTHROAT, FROMOUTSIDE, 1);
			    	You("learn how to perform cutthroat!");
				}
				break;
			case 40: 
			case 41: 
				if (!tech_known(T_BLESSING)) {    	learntech(T_BLESSING, FROMOUTSIDE, 1);
			    	You("learn how to perform blessing!");
				}
				break;
			case 42: 
			case 43: 
			case 44: 
				if (!tech_known(T_E_FIST)) {    	learntech(T_E_FIST, FROMOUTSIDE, 1);
			    	You("learn how to perform elemental fist!");
				}
				break;
			case 45: 
			case 46: 
			case 47: 
				if (!tech_known(T_PRIMAL_ROAR)) {    	learntech(T_PRIMAL_ROAR, FROMOUTSIDE, 1);
			    	You("learn how to perform primal roar!");
				}
				break;
			case 48: 
			case 49: 
				if (!tech_known(T_LIQUID_LEAP)) {    	learntech(T_LIQUID_LEAP, FROMOUTSIDE, 1);
			    	You("learn how to perform liquid leap!");
				}
				break;
			case 50: 
			case 51: 
			case 52: 
				if (!tech_known(T_CRIT_STRIKE)) {    	learntech(T_CRIT_STRIKE, FROMOUTSIDE, 1);
			    	You("learn how to perform critical strike!");
				}
				break;
			case 53: 
			case 54: 
			case 55: 
				if (!tech_known(T_SIGIL_CONTROL)) {    	learntech(T_SIGIL_CONTROL, FROMOUTSIDE, 1);
			    	You("learn how to perform sigil of control!");
				}
				break;
			case 56: 
			case 57: 
			case 58: 
				if (!tech_known(T_SIGIL_TEMPEST)) {    	learntech(T_SIGIL_TEMPEST, FROMOUTSIDE, 1);
			    	You("learn how to perform sigil of tempest!");
				}
				break;
			case 59: 
			case 60: 
			case 61: 
				if (!tech_known(T_SIGIL_DISCHARGE)) {    	learntech(T_SIGIL_DISCHARGE, FROMOUTSIDE, 1);
			    	You("learn how to perform sigil of discharge!");
				}
				break;
			case 62: 
			case 63: 
			case 64: 
				if (!tech_known(T_RAISE_ZOMBIES)) {    	learntech(T_RAISE_ZOMBIES, FROMOUTSIDE, 1);
			    	You("learn how to perform raise zombies!");
				}
				break;
			case 65: 
				if (!tech_known(T_REVIVE)) {    	learntech(T_REVIVE, FROMOUTSIDE, 1);
			    	You("learn how to perform revivification!");
				}
				break;
			case 66: 
			case 67: 
			case 68: 
				if (!tech_known(T_WARD_FIRE)) {    	learntech(T_WARD_FIRE, FROMOUTSIDE, 1);
			    	You("learn how to perform ward against fire!");
				}
				break;
			case 69: 
			case 70: 
			case 71: 
				if (!tech_known(T_WARD_COLD)) {    	learntech(T_WARD_COLD, FROMOUTSIDE, 1);
			    	You("learn how to perform ward against cold!");
				}
				break;
			case 72: 
			case 73: 
			case 74: 
				if (!tech_known(T_WARD_ELEC)) {    	learntech(T_WARD_ELEC, FROMOUTSIDE, 1);
			    	You("learn how to perform ward against electricity!");
				}
				break;
			case 75: 
			case 76: 
			case 77: 
				if (!tech_known(T_TINKER)) {    	learntech(T_TINKER, FROMOUTSIDE, 1);
			    	You("learn how to perform tinker!");
				}
				break;
			case 78: 
			case 79: 
			case 80: 
				if (!tech_known(T_RAGE)) {    	learntech(T_RAGE, FROMOUTSIDE, 1);
			    	You("learn how to perform rage eruption!");
				}
				break;
			case 81: 
			case 82: 
			case 83: 
				if (!tech_known(T_BLINK)) {    	learntech(T_BLINK, FROMOUTSIDE, 1);
			    	You("learn how to perform blink!");
				}
				break;
			case 84: 
			case 85: 
			case 86: 
				if (!tech_known(T_CHI_STRIKE)) {    	learntech(T_CHI_STRIKE, FROMOUTSIDE, 1);
			    	You("learn how to perform chi strike!");
				}
				break;
			case 87: 
			case 88: 
			case 89: 
				if (!tech_known(T_DRAW_ENERGY)) {    	learntech(T_DRAW_ENERGY, FROMOUTSIDE, 1);
			    	You("learn how to perform draw energy!");
				}
				break;
			case 90: 
			case 91: 
			case 92: 
				if (!tech_known(T_CHI_HEALING)) {    	learntech(T_CHI_HEALING, FROMOUTSIDE, 1);
			    	You("learn how to perform chi healing!");
				}
				break;
			case 93: 
			case 94: 
			case 95: 
				if (!tech_known(T_DAZZLE)) {    	learntech(T_DAZZLE, FROMOUTSIDE, 1);
			    	You("learn how to perform dazzle!");
				}
				break;
			case 96: 
			case 97: 
			case 98: 
				if (!tech_known(T_BLITZ)) {    	learntech(T_BLITZ, FROMOUTSIDE, 1);
			    	You("learn how to perform chained blitz!");
				}
				break;
			case 99: 
			case 100: 
			case 101: 
				if (!tech_known(T_PUMMEL)) {    	learntech(T_PUMMEL, FROMOUTSIDE, 1);
			    	You("learn how to perform pummel!");
				}
				break;
			case 102: 
			case 103: 
			case 104: 
				if (!tech_known(T_G_SLAM)) {    	learntech(T_G_SLAM, FROMOUTSIDE, 1);
			    	You("learn how to perform ground slam!");
				}
				break;
			case 105: 
			case 106: 
			case 107: 
				if (!tech_known(T_DASH)) {    	learntech(T_DASH, FROMOUTSIDE, 1);
			    	You("learn how to perform air dash!");
				}
				break;
			case 108: 
			case 109: 
			case 110: 
				if (!tech_known(T_POWER_SURGE)) {    	learntech(T_POWER_SURGE, FROMOUTSIDE, 1);
			    	You("learn how to perform power surge!");
				}
				break;
			case 111: 
			case 112: 
			case 113: 
				if (!tech_known(T_SPIRIT_BOMB)) {    	learntech(T_SPIRIT_BOMB, FROMOUTSIDE, 1);
			    	You("learn how to perform spirit bomb!");
				}
				break;
			case 114: 
			case 115: 
			case 116: 
				if (!tech_known(T_DRAW_BLOOD)) {    	learntech(T_DRAW_BLOOD, FROMOUTSIDE, 1);
			    	You("learn how to perform draw blood!");
				}
				break;
			case 117: 
				if (!tech_known(T_WORLD_FALL)) {    	learntech(T_WORLD_FALL, FROMOUTSIDE, 1);
			    	You("learn how to perform world fall!");
				}
				break;
			case 118: 
			case 119: 
			case 120: 
				if (!tech_known(T_CREATE_AMMO)) {    	learntech(T_CREATE_AMMO, FROMOUTSIDE, 1);
			    	You("learn how to perform create ammo!");
				}
				break;
			case 121: 
			case 122: 
			case 123: 
				if (!tech_known(T_POKE_BALL)) {    	learntech(T_POKE_BALL, FROMOUTSIDE, 1);
			    	You("learn how to perform poke ball!");
				}
				break;
			case 124: 
			case 125: 
			case 126: 
				if (!tech_known(T_ATTIRE_CHARM)) {    	learntech(T_ATTIRE_CHARM, FROMOUTSIDE, 1);
			    	You("learn how to perform attire charm!");
				}
				break;
			case 127: 
			case 128: 
			case 129: 
				if (!tech_known(T_SUMMON_TEAM_ANT)) {    	learntech(T_SUMMON_TEAM_ANT, FROMOUTSIDE, 1);
			    	You("learn how to perform summon team ant!");
				}
				break;
			case 130: 
			case 131: 
			case 132: 
				if (!tech_known(T_JEDI_JUMP)) {    	learntech(T_JEDI_JUMP, FROMOUTSIDE, 1);
			    	You("learn how to perform jedi jump!");
				}
				break;
			case 133: 
			case 134: 
			case 135: 
				if (!tech_known(T_CHARGE_SABER)) {    	learntech(T_CHARGE_SABER, FROMOUTSIDE, 1);
			    	You("learn how to perform charge saber!");
				}
				break;
			case 136: 
			case 137: 
			case 138: 
				if (!tech_known(T_TELEKINESIS)) {    	learntech(T_TELEKINESIS, FROMOUTSIDE, 1);
			    	You("learn how to perform telekinesis!");
				}
				break;
			case 139: 
				if (!tech_known(T_EGG_BOMB)) {    	learntech(T_EGG_BOMB, FROMOUTSIDE, 1);
			    	You("learn how to perform egg bomb!");
				}
				break;
			case 140: 
			case 141: 
			case 142: 
				if (!tech_known(T_BOOZE)) {    	learntech(T_BOOZE, FROMOUTSIDE, 1);
			    	You("learn how to perform booze!");
				}
				break;


			default:
				break;


			}

		  }

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
