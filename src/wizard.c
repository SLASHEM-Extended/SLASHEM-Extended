/*	SCCS Id: @(#)wizard.c	3.4	2003/02/18	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* wizard code - inspired by rogue code from Merlyn Leroy (digi-g!brian) */
/*	       - heavily modified to give the wiz balls.  (genat!mike)   */
/*	       - dewimped and given some maledictions. -3. */
/*	       - generalized for 3.1 (mike@bullns.on01.bull.ca) */

#include "hack.h"
#include "qtext.h"
#include "epri.h"

extern const int monstr[];

#ifdef OVLB

STATIC_DCL short which_arti(int);
STATIC_DCL boolean mon_has_arti(struct monst *,SHORT_P);
STATIC_DCL struct monst *other_mon_has_arti(struct monst *,SHORT_P);
STATIC_DCL struct obj *on_ground(SHORT_P);
STATIC_DCL boolean you_have(int);
STATIC_DCL long target_on(int,struct monst *);
STATIC_DCL long strategy(struct monst *);

static NEARDATA const int nasties[] = {
	PM_COCKATRICE, PM_ETTIN, PM_STALKER, PM_MINOTAUR, PM_RED_DRAGON,
	PM_BLACK_DRAGON, PM_GREEN_DRAGON, PM_OWLBEAR, PM_PURPLE_WORM,
        PM_ROCK_TROLL, PM_XAN, PM_GREMLIN, PM_UMBER_HULK, /* PM_VAMPIRE_LORD,*/
/* WAC replaced vamp lords with mages :) */
        PM_VAMPIRE_MAGE,
	PM_XORN, /*PM_ZRUTY,*/ PM_ELF_LORD, PM_ELVENKING, PM_YELLOW_DRAGON,
	PM_LEOCROTTA, PM_BALUCHITHERIUM, PM_CARNIVOROUS_APE, PM_FIRE_GIANT,
	PM_COUATL, PM_CAPTAIN, PM_WINGED_GARGOYLE, PM_MASTER_MIND_FLAYER,
	PM_FIRE_ELEMENTAL, PM_JABBERWOCK, PM_MASTER_LICH, PM_OGRE_KING,
	PM_OLOG_HAI, PM_IRON_GOLEM, PM_OCHRE_JELLY,
        /* [Tom] added my "worst" monsters... heh heh */
        PM_RHAUMBUSUN, PM_BASILISK, PM_PYROLISK, PM_GREEN_SLIME, PM_DEMON_ORC,
        PM_MASTODON, PM_ROT_WORM, PM_SNOW_ORC, PM_SILVER_DRAGON, 
	PM_DISENCHANTER
	};

static NEARDATA const unsigned wizapp[] = {
	PM_HUMAN, PM_WATER_DEMON, PM_VAMPIRE,
	PM_RED_DRAGON, PM_TROLL, PM_UMBER_HULK,
	PM_XORN, PM_XAN, PM_COCKATRICE,
	PM_FLOATING_EYE,
	PM_GUARDIAN_NAGA,
	PM_TRAPPER
};

#endif /* OVLB */
#ifdef OVL0

/* certain magic portals should give a periodic hint if you're near; this is specifically for mandatory portals that you
 * have to find in order to advance the main questline, so (yes amateurhour, this is not a bug :P) it doesn't help you
 * locate alignment quest portals or devnull challenges or what have you, since the game can be won without them --Amy */
void
test_magic_portal()
{
	struct trap *ttmp;
	if ( (at_dgn_entrance("The Subquest") && u.silverbellget && !u.bellimbued) || (on_level(&sanctum_level, &u.uz) ) ) {
		if (!rn2(15)) {
		    for(ttmp = ftrap; ttmp; ttmp = ttmp->ntrap) {
			if((ttmp->ttyp == MAGIC_PORTAL) && !ttmp->tseen) {
			    int du = distu(ttmp->tx, ttmp->ty);
			    if (du <= 9) {
				pline(FunnyHallu ? "Something is vibrating like mad nearby!" : "The secret portal must be very near!");
			    } else if (du <= 64) {
				pline(FunnyHallu ? "There's some strong vibration close by!" : "You sense that the secret portal is close by!");
			    } else if (du <= 144) {
				pline(FunnyHallu ? "Weird, you're encountering vibrations here! Where may they be coming from?" : "Your automatic detector tells you that the secret portal isn't too far away from you!");
			    } break;
			}
		    }

		}
	}

}

/* If you've found the Amulet, make the Wizard appear after some time */
/* Also, give hints about portal locations, if amulet is worn/wielded -dlc */
void
amulet()
{
	struct monst *mtmp;
	struct trap *ttmp;
	struct obj *amu;

#if 0		/* caller takes care of this check */
	if (!u.uhave.amulet)
		return;
#endif
	if ((((amu = uamul) != 0 && amu->otyp == AMULET_OF_YENDOR) ||
	     ((amu = uwep) != 0 && amu->otyp == AMULET_OF_YENDOR))
	    && !rn2(15)) {
	    for(ttmp = ftrap; ttmp; ttmp = ttmp->ntrap) {
		if(ttmp->ttyp == MAGIC_PORTAL) {
		    int du = distu(ttmp->tx, ttmp->ty);
		    if (du <= 9)
			pline("%s hot!", Tobjnam(amu, "feel"));
		    else if (du <= 64)
			pline("%s very warm.", Tobjnam(amu, "feel"));
		    else if (du <= 144)
			pline("%s warm.", Tobjnam(amu, "feel"));
		    /* else, the amulet feels normal */
		    break;
		}
	    }
	}

	if (!flags.no_of_wizards)
		return;
	/* find Wizard, and wake him if necessary */
	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon)
	    if (!DEADMONSTER(mtmp) && mtmp->iswiz && mtmp->msleeping && !rn2(u.amuletcompletelyimbued ? 40 : 500)) {
		mtmp->msleeping = 0;
		if (distu(mtmp->mx,mtmp->my) > 2)
		    You(
    "get the creepy feeling that somebody noticed your taking the Amulet."
		    );
		return;
	    }
}

#endif /* OVL0 */
#ifdef OVLB

int
mon_has_amulet(mtmp)
register struct monst *mtmp;
{
	register struct obj *otmp;

	for(otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
		if(otmp->otyp == AMULET_OF_YENDOR) return(1);
	return(0);
}

int
mon_has_special(mtmp)
register struct monst *mtmp;
{
	register struct obj *otmp;

	for(otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
		if (is_macguffin(otmp) || is_quest_artifact(otmp))
			return 1;
	return(0);
}

/*
 *	New for 3.1  Strategy / Tactics for the wiz, as well as other
 *	monsters that are "after" something (defined via mflag3).
 *
 *	The strategy section decides *what* the monster is going
 *	to attempt, the tactics section implements the decision.
 */
#define STRAT(w, x, y, typ) (w | ((long)(x)<<16) | ((long)(y)<<8) | (long)typ)

#define M_Wants(mask)	(mtmp->data->mflags3 & (mask))

STATIC_OVL short
which_arti(mask)
	register int mask;
{
	switch(mask) {
	    case M3_WANTSAMUL:	return(AMULET_OF_YENDOR);
	    case M3_WANTSBELL:	return(BELL_OF_OPENING);
	    case M3_WANTSCAND:	return(CANDELABRUM_OF_INVOCATION);
	    case M3_WANTSBOOK:	return(SPE_BOOK_OF_THE_DEAD);
	    default:		break;	/* 0 signifies quest artifact */
	}
	return(0);
}

/*
 *	If "otyp" is zero, it triggers a check for the quest_artifact,
 *	since bell, book, candle, and amulet are all objects, not really
 *	artifacts right now.	[MRS]
 */
STATIC_OVL boolean
mon_has_arti(mtmp, otyp)
	register struct monst *mtmp;
	register short	otyp;
{
	register struct obj *otmp;

	for(otmp = mtmp->minvent; otmp; otmp = otmp->nobj) {
	    if(otyp) {
		if(otmp->otyp == otyp)
			return(1);
	    }
	     else if(is_quest_artifact(otmp)) return(1);
	}
	return(0);

}

STATIC_OVL struct monst *
other_mon_has_arti(mtmp, otyp)
	register struct monst *mtmp;
	register short	otyp;
{
	register struct monst *mtmp2;

	for(mtmp2 = fmon; mtmp2; mtmp2 = mtmp2->nmon)
	    /* no need for !DEADMONSTER check here since they have no inventory */
	    if(mtmp2 != mtmp)
		if(mon_has_arti(mtmp2, otyp)) return(mtmp2);

	return((struct monst *)0);
}

STATIC_OVL struct obj *
on_ground(otyp)
	register short	otyp;
{
	register struct obj *otmp;

	for (otmp = fobj; otmp; otmp = otmp->nobj)
	    if (otyp) {
		if (otmp->otyp == otyp)
		    return(otmp);
	    } else if (is_quest_artifact(otmp))
		return(otmp);
	return((struct obj *)0);
}

STATIC_OVL boolean
you_have(mask)
	register int mask;
{
	switch(mask) {
	    case M3_WANTSAMUL:	return(boolean)(u.uhave.amulet);
	    case M3_WANTSBELL:	return(boolean)(u.uhave.bell);
	    case M3_WANTSCAND:	return(boolean)(u.uhave.menorah);
	    case M3_WANTSBOOK:	return(boolean)(u.uhave.book);
	    case M3_WANTSARTI:	return(boolean)(u.uhave.questart);
	    default:		break;
	}
	return(0);
}

STATIC_OVL long
target_on(mask, mtmp)
	register int mask;
	register struct monst *mtmp;
{
	register short	otyp;
	register struct obj *otmp;
	register struct monst *mtmp2;

	if(!M_Wants(mask))	return(STRAT_NONE);

	otyp = which_arti(mask);
	if(!mon_has_arti(mtmp, otyp)) {
	    if(you_have(mask))
		return(STRAT(STRAT_PLAYER, u.ux, u.uy, mask));
	    else if((otmp = on_ground(otyp)))
		return(STRAT(STRAT_GROUND, otmp->ox, otmp->oy, mask));
	    else if((mtmp2 = other_mon_has_arti(mtmp, otyp)))
		return(STRAT(STRAT_MONSTR, mtmp2->mx, mtmp2->my, mask));
	}
	return(STRAT_NONE);
}

STATIC_OVL long
strategy(mtmp)
	register struct monst *mtmp;
{
	long strat, dstrat;

	if ( (!is_covetous(mtmp->data) && (!mtmp->egotype_covetous) ) ||
		/* perhaps a shopkeeper has been polymorphed into a master
		   lich; we don't want it teleporting to the stairs to heal
		   because that will leave its shop untended */
		(mtmp->isshk && inhishop(mtmp)))
	    return STRAT_NONE;

	switch((mtmp->mhp*3)/mtmp->mhpmax) {	/* 0-3 */

	   default:
	    case 0:	/* panic time - mtmp is almost snuffed */
			return(STRAT_HEAL);

	    case 1:	/* the wiz is less cautious */
			if(mtmp->data != &mons[PM_WIZARD_OF_YENDOR])
			    return(STRAT_HEAL);
			/* else fall through */

	    case 2:	dstrat = STRAT_HEAL;
			break;

	    case 3:	dstrat = STRAT_NONE;
			break;
	}

	if(flags.made_amulet)
	    if((strat = target_on(M3_WANTSAMUL, mtmp)) != STRAT_NONE)
		return(strat);

	if(u.uevent.invoked) {		/* priorities change once gate opened */

	    if((strat = target_on(M3_WANTSARTI, mtmp)) != STRAT_NONE)
		return(strat);
	    if((strat = target_on(M3_WANTSBOOK, mtmp)) != STRAT_NONE)
		return(strat);
	    if((strat = target_on(M3_WANTSBELL, mtmp)) != STRAT_NONE)
		return(strat);
	    if((strat = target_on(M3_WANTSCAND, mtmp)) != STRAT_NONE)
		return(strat);
	} else {

	    if((strat = target_on(M3_WANTSBOOK, mtmp)) != STRAT_NONE)
		return(strat);
	    if((strat = target_on(M3_WANTSBELL, mtmp)) != STRAT_NONE)
		return(strat);
	    if((strat = target_on(M3_WANTSCAND, mtmp)) != STRAT_NONE)
		return(strat);
	    if((strat = target_on(M3_WANTSARTI, mtmp)) != STRAT_NONE)
		return(strat);
	}
	return(dstrat);
}

int
tactics(mtmp)
	register struct monst *mtmp;
{
	long strat = strategy(mtmp);

	mtmp->mstrategy = (mtmp->mstrategy & STRAT_WAITMASK) | strat;

	switch (strat) {
	    case STRAT_HEAL:	/* hide and recover */
		/* if wounded, hole up on or near the stairs (to block them) */
		/* unless, of course, there are no stairs (e.g. endlevel) */
		mtmp->mavenge = 1; /* covetous monsters attack while fleeing */
		if (In_W_tower(mtmp->mx, mtmp->my, &u.uz) ||
			(mtmp->iswiz && !xupstair && !mon_has_amulet(mtmp))) {
		    if (!rn2(3 + mtmp->mhp/10)) (void) rloc(mtmp, FALSE);
		} else if (xupstair &&
			 (mtmp->mx != xupstair || mtmp->my != yupstair)) {
		    (void) mnearto(mtmp, xupstair, yupstair, TRUE);
		}
		/* if you're not around, cast healing spells */
		if (distu(mtmp->mx,mtmp->my) > (BOLT_LIM * BOLT_LIM))
		    if(mtmp->mhp <= mtmp->mhpmax - 8) {
			mtmp->mhp += rnd(8);
			if (mtmp->bleedout) {
				mtmp->bleedout -= rnd(8);
				if (mtmp->bleedout < 0) mtmp->bleedout = 0; /* fail safe */
			}
			return(1);
		    }
		/* fall through :-) */

	    case STRAT_NONE:	/* harrass */
		if (!rn2(!mtmp->mflee ? 5 : 33)) mnexto(mtmp);
		return(0);

	    default:		/* kill, maim, pillage! */
	    {
		long  where = (strat & STRAT_STRATMASK);
		xchar tx = STRAT_GOALX(strat),
		      ty = STRAT_GOALY(strat);
		int   targ = strat & STRAT_GOAL;
		struct obj *otmp;

		if(!targ) { /* simply wants you to close */
		    return(0);
		}
		if((u.ux == tx && u.uy == ty) || where == STRAT_PLAYER) {
		    /* player is standing on it (or has it) */
		    mnexto(mtmp);
		    return(0);
		}
		if(where == STRAT_GROUND) {
		    if(!MON_AT(tx, ty) || (mtmp->mx == tx && mtmp->my == ty)) {
			/* teleport to it and pick it up */
			rloc_to(mtmp, tx, ty);	/* clean old pos */

			if ((otmp = on_ground(which_arti(targ))) != 0) {
			    if (cansee(mtmp->mx, mtmp->my))
				pline("%s picks up %s.",
				    Monnam(mtmp),
				    (distu(mtmp->mx, mtmp->my) <= 5) ?
				     doname(otmp) : distant_name(otmp, doname));
			    obj_extract_self(otmp);
			    (void) mpickobj(mtmp, otmp, FALSE);
				m_dowear(mtmp, FALSE); /* thanks 5lo --Amy */
			    return(1);
			} else return(0);
		    } else {
			/* a monster is standing on it - cause some trouble */
			if (!rn2(5)) mnexto(mtmp);
			return(0);
		    }
	        } else { /* a monster has it - 'port beside it. */
		    (void) mnearto(mtmp, tx, ty, FALSE);
		    return(0);
		}
	    }
	}
	/*NOTREACHED*/
	return(0);
}

void
aggravate()
{
	register struct monst *mtmp;

	incr_itimeout(&HAggravate_monster, rnd( (monster_difficulty() + 1) * 5));
	/* gotta make sure aggravate monster actually does something after all! --Amy */

	/* Amy edit: stealth gives a chance of the monster not waking up; aggravate monster reduces that chance */
	int stealthchance = 0;
	if (Stealth) stealthchance += 20;
	if (StrongStealth) stealthchance += 30;
	if (StrongAggravate_monster) stealthchance /= 2;
	if (stealthchance < 0) stealthchance = 0; /* less than 0% chance makes no sense anyway --Amy */
	if (stealthchance > 0) stealthchance = rnd(stealthchance); /* some randomness */

	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon) {

		int distagravate = distu(mtmp->mx,mtmp->my);

		if (distagravate > 75) {
			distagravate /= 10;
			if (distagravate < 1) distagravate = 1;
			if (distagravate > 95) distagravate = 95;
		} else distagravate = 0;

		if (!DEADMONSTER(mtmp) && (rnd(100) > stealthchance) && (rnd(100) > distagravate) && !(Race_if(PM_VIETIS) && !rn2(3)) && !(Race_if(PM_KUTAR) && !rn2(3)) ) {

			if (mtmp->data->msound == MS_SNORE) continue; /* won't wake up from this effect */

			mtmp->msleeping = 0;
			if(!mtmp->mcanmove && !rn2(5)) {
				mtmp->mfrozen = 0;
				mtmp->masleep = 0;
				mtmp->mcanmove = 1;
			}
		}
	}
}

void
clonewiz()
{
	register struct monst *mtmp2;

	if ((mtmp2 = makemon(&mons[PM_WIZARD_OF_YENDOR],
				u.ux, u.uy, NO_MM_FLAGS)) != 0) {
	    mtmp2->msleeping = mtmp2->mtame = mtmp2->mpeaceful = 0;
		if (Race_if(PM_RODNEYAN)) mtmp2->mpeaceful = 1;

	    if (!u.uhave.amulet && rn2(2)) {  /* give clone a fake */
		(void) add_to_minv(mtmp2, mksobj(FAKE_AMULET_OF_YENDOR, TRUE, FALSE, FALSE));
	    }
	    mtmp2->m_ap_type = M_AP_MONSTER;
	    mtmp2->mappearance = wizapp[rn2(SIZE(wizapp))];
	    newsym(mtmp2->mx,mtmp2->my);
	}
}

/* also used by newcham() */
int
pick_nasty()
{
    /* To do?  Possibly should filter for appropriate forms when
       in the elemental planes or surrounded by water or lava. */
    return nasties[rn2(SIZE(nasties))];
}

/* create some nasty monsters, aligned or neutral with the caster */
/* a null caster defaults to a chaotic caster (e.g. the wizard) */
int
nasty(mcast)
	struct monst *mcast;
{
    register struct monst	*mtmp;
    register int	i, j, tmp;
    int castalign = (mcast ? mcast->data->maligntyp : -1);
    coord bypos;
    int count=0;

	if (Aggravate_monster) {
		u.aggravation = 1;
		reset_rndmonst(NON_PM);
	}

    if(!rn2(10) && Inhell) {
	msummon((struct monst *) 0, FALSE);	/* summons like WoY */
	count++;
    } else {
	tmp = (u.ulevel > 6) ? u.ulevel /6 : 1; /* just in case -- rph */
	/* if we don't have a casting monster, the nasties appear around you */
	if (rnd(20) > 19) { tmp = (tmp * (rnd(5))); } /* now, summoning storms are generally less nasty */
	if (rnd(40) > 39) { tmp = (tmp * (rnd(7))); } /* but occasionally they will summon a gigantic horde of stuff! --Amy */
	bypos.x = u.ux;
	bypos.y = u.uy;
	for(i = rnd(tmp); i > 0; --i)
	    for(j=0; j<20; j++) {
		int makeindex;

		/* Don't create more spellcasters of the monsters' level or
		 * higher--avoids chain summoners filling up the level.
		 */
		do {
		    makeindex = pick_nasty();
		} while(mcast && attacktype(&mons[makeindex], AT_MAGC) &&
			monstr[makeindex] >= monstr[mcast->mnum]);
		/* do this after picking the monster to place */
		if (mcast &&
		    !enexto(&bypos, mcast->mux, mcast->muy, &mons[makeindex]))
		    continue;
		/* if ((mtmp = makemon(&mons[makeindex],
				    bypos.x, bypos.y, NO_MM_FLAGS)) != 0) {
		    mtmp->msleeping = mtmp->mpeaceful = mtmp->mtame = 0;
		    set_malign(mtmp);
		} else GENOD? */
/* changing summon nasties so it can summon everything --Amy */
		    mtmp = makemon((struct permonst *)0,
					bypos.x, bypos.y, NO_MM_FLAGS);
		if(mtmp && (mtmp->data->maligntyp == 0 ||
		            sgn(mtmp->data->maligntyp) == sgn(castalign)) ) {
		    count++;
		    break;
		}
	    }
    }

	u.aggravation = 0;

    return count;
}

/*	Let's resurrect the wizard, for some unexpected fun.	*/
void
resurrect()
{
	struct monst *mtmp, **mmtmp;
	struct monst *zruti;
	long elapsed;
	const char *verb;

	if (!flags.no_of_wizards) {
	    /* make a new Wizard */
	    verb = "kill";
	    mtmp = makemon(&mons[PM_WIZARD_OF_YENDOR], u.ux, u.uy, MM_NOWAIT);
	} else {
	    /* look for a migrating Wizard */
	    verb = "elude";
	    mmtmp = &migrating_mons;
	    while ((mtmp = *mmtmp) != 0) {
		if (mtmp->iswiz &&
			/* if he has the Amulet, he won't bring it to you */
			!mon_has_amulet(mtmp) &&
			(elapsed = monstermoves - mtmp->mlstmv) > 0L) {
		    mon_catchup_elapsed_time(mtmp, elapsed);
		    if (elapsed >= LARGEST_INT) elapsed = LARGEST_INT - 1;
		    elapsed /= 50L;
		    if (mtmp->msleeping && rn2((int)elapsed + 1))
			mtmp->msleeping = 0;
		    if (mtmp->mfrozen == 1) { /* would unfreeze on next move */
			mtmp->mfrozen = 0;
			mtmp->mcanmove = 1;
			mtmp->masleep = 0;
		    }
		    if (mtmp->mcanmove && !mtmp->msleeping) {
			*mmtmp = mtmp->nmon;
			mon_arrive(mtmp, TRUE);
			/* note: there might be a second Wizard; if so,
			   he'll have to wait til the next resurrection */
			break;
		    }
		}
		mmtmp = &mtmp->nmon;
	    }
	}

	if (mtmp) {
		mtmp->msleeping = mtmp->mtame = mtmp->mpeaceful = 0;
		if (Race_if(PM_RODNEYAN)) mtmp->mpeaceful = 1;
		set_malign(mtmp);
		pline("A voice booms out...");
		if (!Race_if(PM_RODNEYAN)) verbalize("So thou thought thou couldst %s me, fool.", verb);
		else verbalize("Hi there again, %s", flags.female ? "sister" : "brother");
	}

	/* occasionally spawn the zrutinator as well --Amy */
	if (!rn2(3)) {
		zruti = makemon(&mons[PM_THE_ZRUTINATOR], u.ux, u.uy, MM_NOWAIT);
		if (zruti) {
			zruti->msleeping = zruti->mtame = zruti->mpeaceful = 0;
			if (Race_if(PM_RODNEYAN)) zruti->mpeaceful = 1;
			set_malign(zruti);
		}
	}

}

/*	Here, we make trouble for the poor shmuck who actually	*/
/*	managed to do in the Wizard.				*/
void
intervene()
{
	int which = Is_astralevel(&u.uz) ? rnd(4) : rn2(6);
	/* cases 0 and 5 don't apply on the Astral level */
	switch (which) {
	    case 0:
	    case 1:	You_feel("vaguely nervous.");
			break;
	    case 2:	if (!Blind) {
			    You("notice a %s glow surrounding you.",
				  hcolor(NH_BLACK));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
			}
			rndcurse();
			break;
	    case 3:	aggravate();
			break;
	    case 4:	(void)nasty((struct monst *)0);
			break;
	    case 5:	resurrect();
			break;
	}
}

void
wizdead()
{
	flags.no_of_wizards--;
	if (!u.uevent.udemigod) {
		u.uevent.udemigod = TRUE;
		u.udg_cnt = rn1(250, 50);
	}
}

const char * const random_insult[] = {
	"antic",
	"ass-fucker",
	"asshole",
	"assmunch",
	"bastard",
	"bitch",
	"blackguard",
	"blasphemous goat-lover", /* thanks Crawldragon */
	"caitiff",
	"chucklehead",
	"coistrel",
	"coward",
	"craven",
	"crazy person",
	"cretin",
	"crone",
	"crybaby",
	"cunt",
	"cur",
	"dastard",
	"demon fodder",
	"dimwit",
	"dirtbag",
	"do-gooder",
	"dolt",
	"dude",
	"emo",
	"faggot",
	"failure",
	"flea",
	"fool",
	"footpad",
	"fucktard",
	"gay fag",
	"girl with a penis",
	"gnat",
	"idiot",
	"ignoramus",
	"imbecile",
	"knave",
	"lamer",
	"loser",
	"maggot",
	"maledict",
	"miscreant",
	"moron",
	"mortal",
	"mother-fucker",
	"niddering",
	"penis-minstrel",
	"pervert",
	"piece of shit",
	"pig",
	"poltroon",
	"poof",
	"poppet",
	"prep",
	"pudding farmer",
	"punk",
	"rattlepate",
	"reprobate",
	"retard",
	"scapegrace",
	"shitbag",
	"simpleton",
	"sissy",
	"person who finds the word 'poofy' insulting",
	"startscummer",
	"stinker",
	"sucker",
	"traitor",
	"useless creature",
	"varlet",
	"victim",
	"villein",	/* (sic.) */
	"wanker",
	"weakling",
	"whiner",
	"wimp",
	"wittol",
	"worm",
	"worthless human being",
	"wretch",
};

const char * const random_malediction[] = {
	"All thine relatives and friends are being killed until thou surrender,",
	"A pox on thee,",
	"Eat a dick,",
	"Eat a pussy,",
	"Go back home and play with thy barbie dolls,",
	"Go back to playing Rodney's Tower,",
	"Go die in a ditch,",
	"Hast thou written thine last wish? Thou",
	"Hell shall soon claim thy remains,",
	"I chortle at thee, thou pathetic",
	"I hope thine pet dies,",
	"I'll fuck thee in the ass,",
	"I shall break thee and violate thine carcass,",
	"I shall violate thine mother when I'm done with thee,",
	"I will eat thee alive,",
	"Maybe I'll see how thou farest against dnethack's elder priest, huh,",
	"May thine innards be invaded by a donkey,",
	"May thou shrivel like a prune,",
	"May thy nose fall off in a freak accident involving a shady hooker,",
	"My forces are setting fire to thine home and murdering thy family right now,",
	"My forces shall hang ten of thine friends at noon unless thou surrender,",
	"Satanchop's low difficulty would match thine nonexistant skill level,",
	"Prepare to die, thou",
	"Resistance is useless,",
	"Rodney can never be defeated by a",
	"Stick that wand of death up thine ass,",
	"Surrender or die, thou",
	"There shall be no mercy, thou",
	"Thine carcass shalt burn,",
	"Thine dad should have used contraceptives,"
	"This is thine end,",
	"Thou shalt repent of thy cunning,",
	"Thou art as a flea to me,",
	"Thou art doomed,",
	"Thou cannot even ascend K-Measurer, let alone SLEX! Begone",
	"Thou standest no chance,",
	"Thunderhammer shall devour thee,",
	"Thy carcass will stink,",
	"Thy fate is sealed,",
	"Verily, thou shalt be one dead",
};

const char * const random_your_mother[] = {
	"Your mother hair is so nappy people have to cut her hair with a weed wacker!",
	"Your mother is so large that, in theory, she can see the universe dying around her.",
	"Your mother is so ghetto she makes spaghetti with ramen noodles and ketchup packets!",
	"Your mother is so old, her birth certificate said EXPIRED.",
	"Your mom is so poor when I stepped on a cigarete butt, she said why did you step on my heater.",
	"Your mother's such a slut her favorite appetizer is whorederves!",
	"Your mother and her friends are so dumb, people call them the blonde-tourage.",
	"Your mom's butt is so big, it looks like 2 pigs fighting over milk duds.",
	"Your mother is so greasy she squeezes Crisco from her hair to bake cookies.",
	"Yo mama's so dry it takes a gallon of water to get her pussy wet!",
	"Your mother is like a carpenter's dream - flat as a board and easy to nail.",
	"Your mother is so bald that when she stood near a wall Lil wayne sang Mirror On The Wall!",
	"Your mom is so slutty shes like a brick always geting slaped by mexicans!",
	"Your mother is so ghetto she makes a homeless person look like a millionaire!",
	"Your mummy's legs so skinny, she looks like a blow pop!",
	"Yo mama's so ghetto, her idea of a fortune cookie is a tortilla with a food stamp in it.",
	"Your mothers so poor I sat on the couch and a roach came up and said move over i pay rent!",
	"Your mother is so lazy she stole your identity just so she could spend more time with you.",
	"Yo mama's so dumb that she got her fingers stuck in a website!",
	"Your mother is so nasty, that i came home and asked what was for dinner. She opened her legs and said tuna suprise!",
	"Your mother is so bald birds land on her head!",
	"Your mom wish she had two cojones because she knows what it's like when the man drops the parachute son!",
	"Your mother is so greasy she sweats Crisco!",
	"Your mum's so dark, she could show up naked to a funeral.",
	"Your mother is so blind she couldn't even see god as her lord!",
	"Your mummy's like a streetlamp, you can find her turned on at night on any street corner.",
	"Your mothers so stupid when I told her that her birthday was just around the corner.....she went looking for it!",
	"Your mummy's house so small that when she was eating in the kitchen her elbows were in the living room!",
	"Your mother is so old an fat; that when she was in the shower she lifted her breast and a pilgrim fell out!",
	"Your mum's so dark every time she gets in a car the check oil light comes on!",
	"Your mom is such a dumb slut, she tried to fuck the atm for money!",
	"Your mother'S SUCH A SLUT THAT SUM PEOPLE SAY THE WIND BLOWS FOR FREE, HOW MUCH U CHARGE.",
	"Your mother's such a slut, she gives out frequent rider miles.",
	"Your mother is so poor she can't even afford a payday!",
	"Your mother bites her nails so much, her stomach needs a manicure!",
	"Your mother is like an arcade game, when you give her a quarter she lets you play with her joy stick.",
	"Your mother is so greasy and fat she uses bacon as a bandaid!",
	"Your old lady's so nasty, when you were being delivered, the doctor was wearing the oxygen mask.",
	"Your mother glasses are so thick, a blind person could see with them.",
	"Your mother is so old her pussy is like a dried up chalk board!",
	"Your mother is so poor I asked to go the toilet and she pointed me to a Pepsi can!",
	"Your mom is so skinny she uses bandaids for pillows!",
	"Your mom' so cross-eyed, everytime she cries tears fall down her back!",
	"Your mother is so old that her birth certificate says doctor Jesus!",
	"Your mother hair is so nappy ...even her eye brows are nappy!",
	"Your mothers so stupid she went to the beach to surf the internet!",
	"Your mom is so poor, she took out a 2nd mortgage on her cardboard box.",
	"Your mother is so old her first job was as Cain and Abel's babysitter.",
	"Your mother is so old she went to an antique store they wouldn't let her leave.",
	"Your mum's so dark she went to night school and was marked absent.",
	"Your mom such a slut the virgin mary caught aids just by looking at her I saw!",
	"Your mother is so old her first Christmas was the first Christmas!",
	"Your mom to give my kids back because she accidently swallowed them last night.",
	"Your mum's like the Pillsbury dough boy.....everybody pokes her.",
	"Your mom is so white that she got in the hot-tub and made creamer!",
	"Your mummy's legs are like curdled milk, white and chunky!",
	"Your mother is so poor children from Africa send her money!",
	"Your mom looks like she's been bobbing for apples in a chipper.",
	"Yo mama's so ghetto, the only gold she wears is on her teeth.",
	"Your mother is so poor and dark when she comes home the roaches sing We are family!",
	"Your mother is so old she knew Burger King while he was still a prince.",
	"Your mom is so skinny the idea of a sword came from her body shape.",
	"Your mother is so ghetto, she makes Kool-Aid without adding sugar.",
	"Your old lady's so old, she used to baby-sit Yoda.",
	"Yo mama's so intelligent that she makes me feel inferior when I don't use words good.",
	"Your mummy's legs are like the library, they're always open to the public.",
	"Your mother is so greasy, she uses pam as hairspray!",
	"Your mother is so ghetto that she still plays with baby toys!",
	"Your mother is so poor she drives a peanut.",
	"Your mum's so bald, you could draw a line down the middle of her head and it would look like my ass.",
	"Your mom pussy stank so much she had to contact poison control!",
	"Your mummy's house is so small, the front and back doors are the same door.",
	"Your mum's lips are so big, she uses Mop & Glow for lipstick.",
	"Your mother is so lazy that she arrived late at her own funeral!",
	"Your mother is so old I told her to act her own age and she got wings and started flying to heaven!",
	"Your mom is so skinny, she looks like a mic stand.",
	"Your mother is so greasy, I used her as an example to explain the drag coefficient of objects moving through a vacuum (9.8 m/s^2)!",
	"Your mothers like the neighborhood bike...Everybodys rode her!",
	"Your mothers teeth so big when she laughs she stabs herself in the chest!",
	"Your mom pussy so tight, all the guys need a pry her loose with a Crow Bar before they Bang her.",
	"Your mother is so ghetto she steals toilet paper from public restrooms!",
	"Your mom such a slut, instead of taking Money she gives STD's all night.",
	"Your mother is so old she owes Jesus 5 dollars!",
	"Your mom teeth is so yellow when she drink water it turns into lemonade!",
	"Your mummy's like a parking garage, three bucks and you're in.",
	"Your mum's so dark, if she had a red light she'd be a beeper.",
	"Yo mama's so dumb, when she was in the ROTC, the Sergeant told her to about-face and she said What about my face?",
	"Your mother is so broke she fuck the atm to get money!",
	"Your mummy's glasses are so thick, she can learn five different languages all at once.",
	"Your mum's so bald, that when she put on a sweater, folk thought she was a roll on deoderant!",
	"Your mum's neck is so wrinkled, she can grate cheese on it.",
	"Your mother is so nasty she brings crabs to the beach.",
	"Your mom is soo fat she ate the rest of my joke!",
	"Your moms jokes aren't funny.",
	"Your mother is so ghetto she uses kool aid packs as hair dye!",
	"Your mother is so greasy she used bacon as a band-aid!",
	"Your mother is so loose it'd be like opening a window and shagging the night.",
	"Your old lady's so old, she drove a chariot to high school.",
	"Your old lady's so old, when she was born, the Dead Sea was just getting sick.",
	"Your mother's teeth are so yellow that when she smiles she puts the sun out of buisness for good.",
	"Your mother is so poor ducks throw bread at her!",
	"Your mother dumb she went to mcdonalds, got a hamburger, put a crown on it, and said burger king!",
	"Your mother is so crossed eye she sees the future and the past at the same time!",
	"Your mother is so bald she braids her mustache!",
	"Your mother is so old I told her to act her own age, and she died.",
	"Your mum's so bald, Mr. Clean got jealous.",
	"Your mother is so ghetto she got her wedding dress at Wal-Mart!",
	"Yo mama's so dirty, the roaches wrote her an eviction notice.",
	"Your mom is so skinny, if she turned sideways and stuck out her tongue, she would look like a zipper.",
	"Your mom' so cross-eyed when she sees a bird, you don't know if it's up or down!",
	"Your old lady's so old, she took her drivers test on a dinosaur.",
	"Your mom, so fat that everytime she takes a step the Earth's orbit changes!",
	"Your mom is so skinny, she swallowed a meatball and thought she was pregnant.",
	"Your mother is so cross eyed, her husband left her for seeing someone on the side!",
	"Your mother is so nasty, I shoke her hand and she gave me gonnerhea!",
	"Your mom is so poor the roaches pay the light bill!",
	"Your old lady's so poor that she went to Five Below with a nickel.",
	"Your mother is so poor a tornado hit your house and did 10,000 dollars worth of improvement!",
	"Your mummy's house is so small, when she let me in, I was in the back yard.",
	"Your mom such a slut, she applied for a job at a strip club but they already had a stage.",
	"Your mother is so nasty, she had to cut the tampon string between her legs because the crabs kept on bungee jumping.",
	"Your mom such a slut she does her buisness outside a chinese shop!",
	"Your mom such a slut she had a threeway with The Tick and Swamp Thing.",
	"Your mother is so poor I went to her house and got robbed by a rat and raped by a roach.",
	"Your mother is so ghetto her face is on the front of a foodstamp.",
	"Your old lady's so nasty, she went to a hair salon and told the stylist to cut her hair, then she opened up her blouse!!",
	"Your mother is so greasy if Crisco had a football team, she'd be the mascot.",
	"Your mum's neck so long when she drank milk it expired before it got to her stomach!",
	"Your mother is so mean she takes more steroids than A-Rod, just to give you a proper whopping.",
	"Your mom is so poor when I came over her house I asked what happend to the color t.v she said we out of crayons!",
	"Your mom is so that archaeologists found ancient pottery in her vagina!",
	"Yo mama's so dumb she put two M&M's in her ears and thought she was listening go Eminem.",
	"Your mom nose so big that her neck broke from the weight!",
	"Your mother is so old......in history class she wrote down what she was doing!",
	"Your mother is so poor I stepped on a lit match and she said who turned off the heat.",
	"Your mother is so old, she knows which Testament is more accurate.",
	"Your mother's such a slut, that they're having to paternity test with the whole state of Texas just to find out who yo daddy is!",
	"Your mom is such a slut, her pants remind me of Vegas.... The kinda place I go to blow my Wad.",
	"Your mother is so ghetto when she runs out of lotion she uses crisco instead!",
	"Your mother house so small, she order a large pizza and had to eat it outside.",
	"Your mother is so nasty, the roaches in her house ask to be sprayed!",
	"Your mom is so skinny she can hang glide off a dorito!",
	"Your mother is so greasy she uses Crisco to wash her hair.",
	"Your mother is so bald, I could polish her head and take her bowling.",
	"Your mother is so old, I slapped her in the back and her tits fell off!",
	"Your mum's so dark when the police shot at her the bullets came back for flashlights!",
	"Your mother is so nasty, that the military weaponized her farts as weapons.",
	"Your mom teeth are so yellow when she closes her mouth her butt lights up!",
	"Yo mama's so fat when she stuck her finger into her belly button it had to be amputated.",
	"Your old lady's so poor, I farted and she said who turned on the heat!",
	"Your mother's such a slut, I slapped her, I punched her and I hurt her and she didn't charge me extra.",
	"Your mom whats for breakfast she put her foot on the table and said corn flakes!",
	"Your mother's such a slut,when they woke up in the morning, his bed was So Wet, the Bed Bugs Drowned.",
	"Your mother is so poor and stupid she thought the term blackout referred to not paying your electric bill!!!!",
	"Your mom is so poor, she bounces food stamps!!",
	"Your mom is so skinny, she inspires crack whores to diet.",
	"Your mother is so poor she put a happy meal on layaway!",
	"Your mother is so fat, when she had fever it caused global warming.",
	"Your mother is so old, she could see the light of Jesus when she was born.",
	"Yo mama's so nasty, even dogs won't sniff her crotch.",
	"Your old lady's so skinny, if she had dreads I'd grab her by the ankles and use her to mop the floor.",
	"Your mom is so poor that her face was on a food stamp card!",
	"Your mother is so ghetto her ps3 controller is made of cardboard and rubber bands!",
	"Your mom's a Slut, even Rapists use condoms.",
	"Your mother is a carpenters dream, she is flat and never been screwed!",
	"Your mom is so skinny, she stood sideways, and the teacher marked her absent.",
	"Your mothers so bald, we thought the sun was rising when she got up!",
	"Yo mama's so nasty, she bit the dog and gave it rabies.",
	"Your mom such a smart ass she threw her watch out the window, just to see if it could intelligently evolve into a bird.",
	"Your mother is so poor i asked her to use the bathroom she said 3rd bucket on the left!",
	"Your mothers like humpty dumpty first she gets humped then she gets dumped!",
	"Your mother's so skinny, she can see out the peephole with both eyes.",
	"Your mummy's like a 5 foot tall basketball hoop, it ain't that hard to score.",
	"Your mother's teeth so big, when I seen her I thought she was Chuckie Cheese!",
	"Your old lady's so nasty, she went swimming and made the Dead Sea.",
	"Your mother is so old she shits out fossils!",
	"Your mom is so poor when I ring the doorbell she says, DING!",
	"Your mother is so old she ran track with dinosaurs.",
	"Your mother's such a slut, her vagina should be in the NFL Hall of Fame for greatest wide-receiver.",
	"Your mom is so poor, I seen her walking down the street with one shoe on. I said Ma'am, did you lose a shoe? She said Nah, I just found one!",
	"Your mom such a slut, I asked her how to spell PENIS, and she said you should have asked me last night it was at the tip of my tounge.",
	"Your mother is so old when she breast feeds it comes out like powder.",
	"Your mom is such a slut that her legs are like peanut butta.. EASY TO SPREAD!",
	"Your mother's so stupid that she got her fingers stuck in a website!",
	"Your mom's feet are so big and nasty, when she wants jam, she gets someone to run a loaf of bread between her toes.",
	"Your mom's glasses are so thick she can solve mysteries faster than Velma on Scooby Doo.",
	"Your mom is so poor when I asked what's for dinner she put her shoe laces in a pot and said spaghetti!",
	"Your mom's been cocked more times than Elmer Fudd's shotgun.",
	"Your mother is so lazy that she came in last place in a recent snail marathon.",
	"Your mother is so nasty she's not even allowed to look at Thor's hammer.",
	"Yo mama's so dirty, she's got more clap than an auditorium.",
	"Your mom is so poor she went to Payless and couldn't afford to pay less!",
	"Your mother's such a slut, Yo daddy is a multiple-choice question.",
	"Your mother is so lazy she doesn't have a dining table, because she always eats in bed.",
	"Your mother's such a slut, she stuck in a cucumber and pulled out a pickle.",
	"Your mother is so poor i saw her kicking a can and ask her what she was doin she said moving!",
	"Your mother is so lazy she won the Netflix marathon.",
	"Your mom teeth so yellow, when she smiled it looked like she was giving out free urine samples.",
	"Your mum's so dark she looks like a satellite picture of North Korea at night.",
	"Your mummy's house is so small, she wakes up on a Lego bed singing Everything is Awesome.",
	"Your mother's such a slut she's like a vaccum she sucks blows and gets laid in the closet!",
	"Your mother is so poor and stupid, she draws Lincoln's face on a piece of paper and says it is a twenty!",
	"Your mother is so ghetto that when you asked her for a glass of milk she said pick a titty!",
	"Your mother got such a big butt, Nicki Minaj is jealous.",
	"Your mother is so greasey that she single-handedly increased oil production and lowered gas prices.",
	"Your mother is so lazy the only thing she gets excited about is cancelled plans.",
	"Your mother is so poor she speak's japoornese.",
	"Your mother is so Buddhist, I gave her a vacuum for Christmas and she returned it because it had too many attachments!",
	"Your old lady's so skinny, if she turned sideways and stuck out her tongue, she would look like a zipper.",
	"Your mother is so ghetto she wears red, neon green, purple, and yellow in the same damn outfit!",
	"Your mother is so old her butt crack sealed..",
	"Your mother is so old she sued Google for naming there search engine after her age.",
	"Your mum's like the Panama Canal, vessels full of seamen pass through her everyday.",
	"Your mom put da rat in ratchet!",
	"Your mom teeth so yellow when she smiled traffic slowed down!",
	"Yo mama's so nasty, she made Right Guard go left, Speed Stick slow down, and Ban come off strike.",
	"Your mother is so fine, she makes my testicals do the macarena.",
	"Your mom such and Animal, during a full moon, with her Chin up in the Air, she howls at the full moon.",
	"Your old lady's so old, she farts dust!",
	"Your mom is so tall she did a back flip and kicked Jesus in the chin!",
	"Your mothers so dumb, she thinks gluteus maximus is a Roman emperor.",
	"Your mother is so poor I walked into your house and 3 roaches tripped me & tried to take my wallet!",
	"Your mothers like a screen door after a couple of bangs she tends to loosen up.",
	"Your mother is so bald, that when i rubbed on her head i could see the future.",
	"Your mom is so short she wakes up in a lego house singing Everything is Awesome.",
	"Your mothers so fat einstien based the black hole theory on her Ass hole!",
	"Your mother is so old she dated john the baptist!",
	"Your mother is so ghetto when she does grocery shopping she hide packets of kool-aid under the box of ramen noodles.",
	"Your mum's pussy so nasty, they make you eat it during Fear Factor!",
	"Your mothers so fat when she died and went to heaven Jesus said there was no room so then she went to hell and the devil said HELL NO.",
	"Your old lady's so skinny, her nipples touch.",
	"Your mom is so poor, I took a piss on her front lawn and she thanked me for watering the lawn.",
	"Your mother is so old her pet bird was a Pteradactyl.",
	"Your mothers so Dumb, when the man says, Let's have a one Night Stand, she runs to the garage and chops off Three of the Night Tables legs!",
	"Yo mama's so ghetto, she pretends her pager is a cellphone!",
	"Your mother's teeth are so jacked, they look like a toolbox of rusty nails!",
	"Your mother is so bald, you can play Air Hockey on her head.",
	"Your mum's so dirty, she went swimming and made the Dead Sea.",
	"Your mom teeth is so bright the Sun needs glasses!",
	"Your mummy's gums are so black, she spits Yoo Hoo.",
	"Your mother is so Nasty, she puts salt water down her Trousers to Keep her crabs Fresh!",
	"Your mother's so stank, the US Government uses her bath water for chemical weapons.",
	"Your mom is so skinny, she could dive through a fence!",
	"Your mother is so cheap, instead of writing her mother a letter on stationery paper, she write her letter on toilet paper.",
	"Your mother's so stank, that her shit is glad to escape.",
	"Your mother is so poor she uses candy wrappers as wall paper!",
	"Yo mama's so ghetto, she brings fried chicken to the movie theater!",
	"Your mother is so poor she makes a homeless person look like a millionaire!",
	"Your mother is so poor she went to McDonald's and put a milkshake on layaway.",
	"Yo mama's so nasty, she puts ice down her drawers to keep the crabs fresh.",
	"Your mother is so lazy she's got a remote control just to operate her remote!",
	"Your mummy's house is so small, she ain't got room to change her mind.",
	"Your old lady's so skinny, her pants have one belt loop.",
	"Your old lady's so poor when i jumped in a puddle she said What are you doing in my bathtub?",
	"Your mom is such a whore she fucked the football team and had to get her stomach pumped!",
	"Your mom such a slut, she puts salt water down her Trousers to Keep her crabs Fresh!",
	"Your mummy's legs are like Jif, easy to spread.",
	"Your mom is so skinny she ate a M&M and looked 8 months pregnant!",
	"Your mom's bikini pictures!",
	"Your mom is so poor when I lit a match the roaches started singing clap your hands stomp your feet praise the lord we got heat!",
	"Your mum's mouth is so big, she speaks in surround sound.",
	"Your mother head is so big she needs cotton wool to fill up her brain!",
	"Your mother is so old she thought 50 shades of grey was the color of her hair!",
	"Your mother is so old she sat next to jesus in the 1st grade!",
	"Your mum's so dark, when she went to night school she got marked absent!",
	"Your mom's butt is so bony, she put her drawers on and cut them in two.",
	"Your old lady's so old, when she was in school there was no history class.",
	"Your mother is so gorgeous, if she was a washing machine, I would put my dirty load inside her.",
	"Your mom is so skinny she can't sideways when taking a selfie.",
	"Your mothers such a bitch that Gordon Ramsey didn't even like her.",
	"Your mother didn't learn her ABC's, she learned her STD's!",
	"Your mom is so smart, no one bothers to argue with her because they know she's always right.",
	"Your mummy's hair is so nappy, even Moses couldn't part it.",
	"Your mother is so bald you can see whats on her mind!",
	"Your mother is so poor and her credit is so bad, she couldn't use a free promo code at Redbox.",
	"Your mum's like peanut butter: brown, creamy, and easy to spread.",
	"Your mother is so poor she gotta eviction notice on her car!",
	"Your mom is soo bald even the hairdressers used her head for a mirror!",
	"Your mother got a credit card pussy, everybody swipe through it!",
	"Your mother's such a slut, she's like a race car driver...she burns a lot of rubbers...",
	"Your mother is so big, she went to the airport and asked for a ticket. And they gave her clearance to take off!",
	"Your mom such a slut, she licked 3 pop sicles and got pregant!",
	"Your mother is so ghetto she rated R for Ratchet!",
	"Your mother is so bald, when she puts on a turtle neck she looks like a busted condom.",
	"Your mom teeth so big when she sneezes she pokes a hole throw her stomach!",
	"Your mom is so poor when I saw her kicking a can down the street, I asked her what she was doing, she said Moving.",
	"Your mom such a slut, she dont need the internet she already world wide!",
	"Your mothers so stupid, when the man says, Let's have a one Night Stand, she runs to the garage and chops off Three of the Night Tables legs!",
	"Your mothers such a bitch the devil wouldn't let her go to hell and Jesus made her live forever.",
	"Your mother is so ghetto she sells her food stamps to get her nails done!",
	"Your mom is such a slut she will have sex with you for a ride home!",
	"Your mother is so bald, I thought she was Mr. Clean.",
	"Your old lady's so old, her butt crack sealed.",
	"Your mother is so nasty the million man march was at her house!",
	"Your mom is so skinny she played the part of the staff in the story of moses!",
	"Your mother and her friends are so stupid, people call them the blonde-tourage.",
	"Your mother is so fine, when god made her the other angels got jealous.",
	"Your mum's so dark she went to night school and got marked absent!",
	"Your mother's such a slut that when she walks her pussy claps!",
	"Your old lady's so old, she knew Cap'n Crunch while he was still a private.",
	"Your mother is so greasy I buttered my popcorn with her leg hairs.",
	"Your mother is so old, when she was a child, rainbows were in black and white!",
	"Your mom teeth are so yellow she spits butter on popcorn!",
	"Your mummy's like a postage stamp, you lick her, stick her, then send her away.",
	"Your mother is fat that scientists are worried she's helping the earth become a black hole.",
	"Your mom is so poor that when it rains she says kids shower time!",
	"Your mothers so stupid that she bought curtains for her computer just because it uses Windows!",
	"Your mother is so old they moved her out of the retirement home and in to the museum.",
	"Your mother is so ghetto when the doctor takes her blood only weed and kool-aid come out!",
	"Yo mama's so dumb the only letters in the alphabet she knows is K.F.C.",
	"Your mother is so lazy AND fat, she ate all the lunch meat, and didn't bother to pack yo lunch.",
	"Your mummy's house is so dirty, Stanley Steamer could get her house cleaner!",
	"Your mother is fat that scientists are worried she's helping the earth become a black hole.",
	"Your mothers glasses so thick she can see past the horizon line of a black hole!",
	"Yo mama's so ghetto, her baby daddy and her boyfriend share a bunk-bed.",
	"Your mother's vagina is so shaved everybody thought it was a credit card swipe!",
	"Your mummy's glasses are so thick, she knows exactly whats inside Taco Bell Beef.",
	"Your mother is so awkward and clumsy she makes Napoleon Dynamite look like James Bond.",
	"Your mother aint so bad...she would give you the hair off of her back!",
	"Your mothers clit was so dry that every time she'd masterbate she'd put 4 liters of lube just to get wet!",
	"Your old lady's so nasty, when I went to yo house said whats for dinner,",
	"Your mother is so poor she created a gmail account just so she can eat the spam!",
	"Your mother's so stank, when you were being delivered, the doctor was wearing the oxygen mask.",
	"Your mothers panties are so funky, the roaches check in but they don't check out.",
	"Your mother is so poor she can't afford to pay attention!",
	"Your mother's so stupid she put two M&M's in her ears and thought she was listening go Eminem.",
	"Yo mama's so nasty, she was playing in a sand box and a cat came along and buried her.",
	"Your mother is so old she planted the apple tree in The Garden of Eden!",
	"Your mummy's like a screen door, after a couple of bangs she loosens up.",
	"Your mother is so nasty, they call her the carpenters delight, flat as a board and easy to nail!",
	"Your old lady's so skinny, she can grate cheese on her ribs!",
	"Your mother is so ghetto the bitch breastfed you with gin and juice!",
	"Your mother is so old she got to see passion of the christ live!",
	"Your old lady's so nasty, we asked her what was fo dinner, she put her foot up on the table and said, Corns.",
	"Your mothers like a hardware store 5 cents a screw!",
	"Yo mama's so ghetto, she puts her food stamps in a money clip.",
	"Your mother is so poor I sat on the couch and a roach came up and said move over I pay rent!",
	"Your mother's such a slut, she's like the Suez Canal - Vessels full of Seamen passing through everyday...!",
	"Your mother is so old she got her bible signed by jesus!",
	"Your mother is so nasty that she farted and the dinosaurs came back to life.",
	"Your mom is so short, she was rejected by the munchkins AND the Oompa-Loompas!",
	"Your mom nose so big she makes Pinochio look like a cat!",
	"Your mom is so skinny she can dodge raindrops.",
	"Your mother is so hunchbacked, she can stand on her feet and her head at the same time.",
	"Your old lady's so old, she knew the Beatles when they were the New Kids on the Block.",
	"Your old lady's so poor, I stepped on a lit match and she said who turned off the heat!",
	"Your mom is so skinny when she wears skinney jeans they look like bell bottoms!",
	"Your mothers so poor when she gets mad she can't afford to fly off the handle so she's gotta go greyhound off the handle!",
	"Your mother is so old that when she was in school there was no history class.",
	"Yo mama's so nasty, she made Speed Stick slow down.",
	"Your mom's forehead looks so big people mistake it for the Great Wall of China.",
	"Your mother is so old that when i took a picture of her it came out black and white!",
	"Your mom like a christmas tree people hang their balls on her. Hey,Tell!",
	"Your mother is so easy im your father!",
	"Your mother is so old her birth certificate is in Roman numerals.",
	"Your mom like a chicken coop ... Cocks fly in and out all day!",
	"Your mother is so poor I saw her holding a penny and I asked Whatcha doing with that? and she said Taking my life savings to the bank!",
	"Your mum's lips are so big, she went to whisper in my ear and she whispered in both my ears at the same time!",
	"Your mum's so dark when you go swimming it looks like an oil spill.",
	"Your mother is so ghetto she uses expired foodstamps as tampons.",
	"Your mother is so mean, that Regina from Mean Girls is her sister.",
	"Your mummy's hips are so big, people set their drinks on them.",
	"Your mom teeth so yellow that when she looked up at the sun the sun said hey cuz!",
	"Your mom such a slut, she thinks L.O.V.E. stands for: Legs. Open. Very. Easy.",
	"Your mum's so bald, when she braids her hair, it remind me of stitches.",
	"Your mother is so mean even Simon Cowell is afraid to say anything bad about her.",
	"Your mom is so short even Peter Pan and the lost boys calls her a midget.",
	"Your mother is fat that when she sits on a semi they say 'Ok now give me the over size load sign.'",
	"Your mother is so grasey you had to use bacon for her bandaids!",
	"Your mothers so old she sat next to Ben Franklin in kindergarden!",
	"Yo mama's so fat and nasty, instead of using Tampons, she uses Ca. King Size Mattress's.",
	"Your mother is like a bus, big, 50 cents, and 20 people can ride her at once Deez nuts are so big that only!",
	"Your mom's glasses are so thick she can tell you what your son looks like before he's born.",
	"Your mother is so old that she remembers when the Grand Canyon was just a ditch!",
	"Your mother is so poor she cant afford to wash herself so she stands in the rain!",
	"Your mom's butt cheeks are so big, even Moses couldn't part them.",
	"Your mom is so poor the only word she knows is benefit!",
	"Your mum's so dark, her ass looks like two tires.",
	"Your mother i'd shave its butt and teach it how to walk backwards.",
	"Your mother is so lazy, she stuck her nose out the window and let the wind blow it.",
	"Your mom is soooo stupid that when her ink pen ran out of ink she asked where the pencil sharpener was!!!",
	"Your mom teeth are so yellow traffic slows down when she smiles!",
	"Your mother is so old she left her purse on noahs ark!",
	"Your mother had to feed you with a slingshot.",
	"Your mother's so stupid, when she was in the ROTC, the Sergeant told her to about-face and she said what about my face?",
	"Your mother is so old that she gangbanged with the hebrews.",
	"Your mother is so nasty she's like a nascar driver she burns 50 rubbers a day!",
	"Your mothers so nasty she brings crabs to the beach!",
	"Your mom must be a parking ticket because she got fine written all over her.",
	"Your mummy's hair is so nappy, she uses a rake to comb it.",
	"Your mom is so poor, she drives a Poor-shh!",
	"Your mummy's house is so small, I put my key in the lock and broke the back window.",
	"Your mom is so poor when i used the bathroom i used one stick to keep the roof up and another to scare the roaches away!",
	"Your mom teeth so yellow when she smiled she looked like a Chevrolet logo!",
	"Your mummy's like a Christmas tree everyone puts Balls on her!",
	"Yo mama's so nasty she did the splits and gave the floor a hickey!",
	"Your mother's such a slut, her pussy is called Jasmine, because it's always got Aladdin!",
	"Your mother is so nasty i went to her house and ask whats for dinner she opened she legs and said FISHSTCKS want some!",
	"Your mother is so poor i spent the nite at your house and in the morning I asked!",
	"Your mothers so stupid she put cat food down her pants to feed her pussy.",
	"Your mother is so ancient, when she went to the museum, the mummies took pictures of her and said DAYUM.",
	"Your mother is so old she looks like my scrotum sack...",
	"Your mom teeth are so yellow it looks like she brushes her teeth with vanilla wafers!",
	"Your mom teeth are so big she flosses with a blanket!",
	"Your old lady's so nasty, the roaches check in but they don't check out.",
	"Your old lady's so nasty, she's got more clap than an auditorium.",
	"Your mum's lips are so big, that ChapStick had to invent a spray.",
	"Your old lady's so old, she knew Burger King while he was still a prince.",
	"Your mother's so skinny, she uses Chapstick for deodorant.",
	"Yo mama's so nasty, she only changes her drawers once every 10000 miles.",
	"Your mother is so intelligent, she's the only person that can talk down to Stephen Hawking.",
	"Your mom, she wuz doin' deep knee bends over a parking meter!",
	"Your mother glasses are so thick, she can burn ants with them.",
	"Your mother is so nasty, she uses Dr. Scholl Odor Eaters for panty liners.",
	"Your mother is so old God signed her yearbook.",
	"Your mother is so old, that she grew up WITH the Flintstones!",
	"Your mother is so poor she waves around a Popsicle stick and calls it air conditioning.",
	"Your mummy's gums are so black, she spits Chocolate Milk.",
	"Your mummy's head is so big, instead of needing a pillow, she needs the Moon.",
	"Your mom is so poor when I ring the doorbell I hear the toilet flush!",
	"Your old lady's so shinny, she had to stand in the same place twice to cast a shadow.",
	"Your mother's such a slut, even the noble gases are attracted to her.",
	"Your mum's so bald she curls her hair with rice.",
	"Your mom's face just shows what can happen when you stick your head into a garbage disposal and go bobbing for leftovers!",
	"Your mom such a bitch she makes Nancy Grace look like Mary Poppins.",
	"Your mom such an old whore she slept with the Father, The Son, and the Holy Ghost!",
	"Your mother is like a flouresent light bulb she is old but she gets the job done.",
	"Your old lady's so old, she planted the first tree at Central Park.",
	"Your mom is so poor when she steped on a roach she said clap your hands stomp yo feet praise the lord we got somethin to eat.",
	"Your mothers so fat that when she eats a burger it sounds like Jurassic park on surround sound.",
	"Your mother and daddy so dark that your family pictures look like ultrasounds!",
	"Your mummy's house is so nasty, every time I open the Fridge Cock Roaches are riding four-wheelers.",
	"Your mother is so poor it took her 3 years to save a penny!",
	"Your mothers so dumb when I told her that her birthday was just around the corner.....she went looking for it!",
	"Your mother conforms to Planck's law. The greater the frequency with which she screws, the more energetic she gets.",
	"Your mom is so skinny, she uses a Band-Aid as a maxi-pad.",
	"Your mom such a slut, her favorite word is HARDER!",
	"Your mother is so old she gave historians the secrets to Mesopotamia.",
	"Your mother ass is so big, she bent over and got arrested for selling crack.",
	"Yo mama's so nasty she joined the four horseman: war, death, famine, disease and!",
	"Your mother is so mean, your bath toys were an iron and a toaster.",
	"Your mother is so old, the back of her head looks like a raisin.",
	"Your mother is so lazy she undercooks ramen noodles.",
	"Your mom's ankles are so ashy, it looks like she's wearing socks.",
	"Your mom is so short she became a sidekick to Ant-Man!",
	"Your mom is so poor your family ate cereal with a fork to save milk.",
	"Your mother is so old she was a crossing guard for when Moses parted the red sea!",
	"Your old lady's so nasty, when she leans over and farts, she needs to Re Wall Paper the whole living room!",
	"Your mother is so ghetto she breatfeeds kool-aid, shits chicken bones, and pisses grape soda!",
	"Your mummy's glasses are so thick, when she turns them around she can see yesterday.",
	"Your old lady's so old, when she reads the bible she reminisces.",
	"Your mum's so cross-eyed, when I put my dick in her mouth she said One at a time!",
	"Your mom is so poor they caught her shoplifting at Dollar General!",
	"Your mum's lips are so big, when she smiles she gets ChapStick on her ears.",
	"Your mother is so old she dated john the baptist!",
	"Your mom is so smart she broke Ken Jennings record for being the Worlds Biggest Smartass!",
	"Your mother house is so small, I threw a rock through the window and hit everyone inside.",
	"Your mother is so poor I asked her if I could use the bathroom and she said Just pick a corner!",
	"Your mum's so dark that her favorite dinosaur is a Tri-scared-a-cops!",
	"Your mothers so dumb that she bought curtains for her computer just because it uses Windows!",
	"Your mummy's head is so small, she got her ear pierced and died.",
	"Your mother is so nasty I called her to say hello, and she ended up giving me an ear infection.",
	"Your mother is so intelligent, that she knows!",
	"Your mother is so poor I saw here walkin down the street I asked her if she lost a shoe and she said no she just found one!",
	"Your mom such a bad christian, the only thing holy about her is the Swiss cheese in her fridge.",
	"Your mother is so poor people rob her house for practice.",
	"Your mother is so poor, she makes starving Africians look like multi-quadrillion aires!",
	"Your mom such a hot blonde, her eyes are bluer than Heisenberg's crystal!",
	"Your mother is so ghetto dat every thing she eats is in cans!",
	"Your mom such a stupid bartender, after a woman ordered a Margarita, she swam out to the Gulf of Mexico to get Margaret!",
	"Your mummy's is such a slut, she asked all the math majors to to figure out g(f(your mom)) just so they could f her first.",
	"Your mom is so poor when I went over her house and asked what's for dinner she opened her legs and said fish sticks What's the difference between!",
	"Your mother is so ghetto her engagement ring is from the pawn shop!",
	"Your mom is so smart and busy, she doesn't use big words because she doesn't have the time to explain it you again.",
	"Your old lady's so old, she baby-sat for Jesus.",
	"Your mother is so nasty, she walked up to a racoon and said you ain't got nothin on me!",
	"Your old lady's so nasty, when she pulls down her panties it sounds like Velcro.",
	"Your mummy's house so small when she put the key in the front door she stabbed 5 people in the hallway.",
	"Your mother's so wise, that Yoda texts her for advice.",
	"Your mother is so gassy when the plane ran out of fuel they used her as a back up!",
	"Your mom is so skinny I put a dime on her head and people mistook her for a Nail!",
	"Your mothers so dumb she put cat food down her pants to feed her pussy.",
	"Your mother is so intelligent her brains are as big as her boobs.",
	"Yo mama's so nasty, I talked to her over the computer and she gave me a virus.",
	"Your mum's so dark I shot her and the bullets came back with flashlights saying I can't find the bitch!",
	"Your mother is so poor she hangs colored socks as Christmas lights.",
	"Your mothers teeth are so yellow when she stands at a crosswalk the cars slow down!",
	"Your mother is so old her first pet was a T-Rex!",
	"Your mom is so short, she wakes up in a lego house singing Everything is Awesome.",
	"Your mummy's like a refrigerator, everyone puts their meat in her.",
	"Your mother is so bald that she took a shower and got brain-washed.",
	"Your mom such a slut that her vagina is like a 3 star hotel, there is always people coming in and out!",
	"Your mother is like a bowling ball..... round, heavy, and you can fit three fingers in.",
	"Your mother's so skinny, when I slapped her I got a paper cut!",
	"Your mother is so ghetto all her wigs are bright ass colors!",
	"Your mom is so skanky, her dildo came with jumper cables.",
	"Your mother is so lazy she makes Garfield look like Lebron James!",
	"Your mother is so old her social security number is 1!",
	"Your old lady's so old, she walked into an antique store and they kept her.",
	"Your mother is so nasty that pours salt water down her pants to keep her crabs fresh.",
	"Your mom is so short she's Mini-Me's.... Mini-Me!",
	"Your mom is so skinny the Olsen Twins called and said they want their eating disorder back.",
	"Yo mama's so ghetto, she does drive by shootings on the bus!",
	"Your mother is like a chicken coop, cocks go in and out all day!",
	"Your mother forehead's so big, you could show slides on it.",
	"Your mom pussy so tight, she had to drill a hole in her Gina so she can insert a Tampon!",
	"Your mum's so cross-eyed, she thought her only child was a twin.",
	"Your mother is so lazy her patronus is a sloth.",
	"Your mummy's house is so small, I put my key in the lock and stabbed everyone inside.",
	"Yo mama's so nasty, she dropped her toothbrush in the toilet. We had to get a new toilet.",
	"Your mom's ass is so big, its got more crack than Mayor Marion Barry.",
	"Your mom is so sweet she's giving me a toothache.",
	"Your mother is so bald that people could ice skate on her head.",
	"Your mother is so nasty third world countries eat the cheese from the crack of her ass!",
	"Your mother is so poor for Christmas she got a box, put two sticks on it, spun it and said son here's your xbox 360!",
	"Your mother's such a slut, she got her tubes tied and still got pregnant.",
	"Your mother is so mean, Bruce Banner turns green just thinking about her.",
	"Your mother is so ghetto her phone bill is nothing but 911 calls!",
	"Your mom stupid she drowned in death valley!",
	"Your mothers so dumb when the judge said order! she said pie and chips please!",
	"Your mom is so smart, she went to Florida State and everyone thought she was a tourist.",
	"Your old lady's so old, she's got hieroglyphics on her driver's license.",
	"Yo mama's so nasty, she stuck in a cucumber and pulled out a pickle.",
	"Your mother is so big dora can't explore her!",
	"Your mom is so poor when a visitor came to her house he asked, may I please use the bathroom she said pick a corner, any corner.",
	"Your mom is so smart, Doctors let her second guess their diagnosis.",
	"Your mom's glasses are so thick she can watch old movies in technicolor.",
	"Your mother is so poor she runs after a garbage truck with her grocery list!",
	"Your mom's glasses are so thick she can see a rainbow on a sunny day.",
	"Your mother head is filled of so much hot air she can fly like the birds and sing like a canary.",
	"Your mom is so poor when she found a coupon that said 50%% off, she went looking for the other half.",
	"Your mom such a slut, that if her pussy had a password, it would be 1234.",
	"Your mom is so skinny she turned sideways and disappeared.",
	"Your mother is so large, she doesn't need a data plan, she's already world wide.",
	"Your mother's such a slut, just like a Popsicle - everybody wants a fuckin lick!",
	"Your mother's such a slut she did the splits and gave the floor a hickey!",
	"Your mother's such a slut she was the subject of the TV movie Fat Insane Whore!",
	"Your mummy's hair is so nappy, her comb had to get dentures. Your hair line so fucked up Jesus found your barber and sent him to hell.",
	"Your mothers like a transformer, she makes ac to dc current look easy.",
	"Your old lady's so old, Jurassic Park brought back memories.",
	"Yo mama's so nasty she got Play-Doh teeth.",
	"Your mother is so bald, I can tell fortunes on her head.",
	"Your mother is so poor her face is on the front of a foodstamp.",
	"Your mom is so thin that if she stands in front of a wall she looks like a crack.",
	"Your mom such a dumb bartender, after a woman ordered a Margarita, she swam out to the Gulf of Mexico to get Margaret!",
	"Your mom is so poor, her front and back door are on the same hinge!",
	"Your mom's feet so big, her sneakers need license plates.",
	"Your mother does too but she charges!",
	"Your mother is so poor she was in K-Mart with a box of Hefty bags. I said, What ya doin'? She said, Buying luggage.",
	"Your mum's so dark, when she puts on yellow lipstick, she looks like a cheese burger.",
	"Your mum's so cross-eyed, she can see the front and the back door at the same time!",
	"Yo mama's so fat, she makes Jonah Hill look superbad at gaining weight!",
	"Your mom is so skinny, if she had a yeast infection she'd be a Quarter Pounder with Cheese.",
	"Yo mama's so nasty, I called her up for phone sex and she gave me an ear infection.",
	"Your old lady's so nasty, the US Government uses her bath water for chemical weapons.",
	"Your mother is so masculine that everytime she grunts, she grows a mustache.",
	"Your mother is so poor she waved her hand around a popsicle and called it air conditioning.",
	"Your mother is so lazy her to do list is one word long nothing!",
	"Your mum's so dark, if they put you in a bottle You'd be a Pepsi!",
	"Your mum's so dark if she had a red light she'd be a beeper.",
	"Your mom is so skinny she can use a bracelet as a hula hoop!",
	"Yo mama's so ghetto, she's got 40oz. tits.",
	"Your mothers pussy is like a can of pringles once you pop u can't stop!",
	"Your mom knows karate cause her body is kickin.",
	"Your mum's so dark, she looks like a picture of outer-space with no stars.",
	"Your mum's so dark that lightening bugs follow her in the daytime!",
	"Your mom pussy so tight, it squeaks when she walks!",
	"Your mother is like Humpty Dumpty - First she gets humped, then she gets dumped.",
	"Your mother is so nasty she went to your school and the principal dropped out!",
	"Your mother is so old she was best friends with Jesus.",
	"Your mother her porta potty is bigger than my house!",
	"Your mothers teeth are so yellow, I thought I was watching despicable.me!",
	"Your mother is so mean the only letters of the alphabet she knows is PMS.",
	"Your mother is a tall glass of water. And im telling youuuu str8 up im thirsty.",
	"Your mom is so skinny, she can dodge rain drops.",
	"Your mummy's hair is nappier than a goats ass.",
	"Your mother is so old she farts dust and shits rust!",
	"Your mother is so chubby when she went out to the strip club, they called her momma shake weight!",
	"Your mother's such a slut, she is like a protractor.....good at every angle.",
	"Your mom is so poor, her front door and back door are the same thing.",
	"Your mum's so dark if she sat in a jacuzy the water turned into coffee!",
	"Your mother is so ghetto she rips off the cover from the phone book and uses it as a dust pan!",
	"Your mother is so greasy her freckles slipped off.",
	"Your mom teeth is so yellow that when god said let their be light, she opened her mouth.",
	"Your mother is so old when she reads the bible, she thinks to herself, these guys stole ma life story.",
	"Your mother is so crazy she asks why can't you be more like Kirk Cameron!",
	"Yo mama's so dumb she returned a puzzle because it was broken!",
	"Your mother is so bald even a wig wouldn't help!",
	"Your mom is so skinny, when she wore her yellow dress, she looked like a #2 pencil.",
	"Your mum's so dark, she was riding a motorcycle and got a ticket for tinted windows.",
	"Your mom is so smart she makes Macgyver look like the village idiot.",
	"Your mum's so bald her hair looks like stitches.",
	"Your mom teeth so dirty when she went to reach for her tooth brush and it did the matrix and said you not picking me!",
	"Your mum's scalp so dirty, her hair started a protest...Hell no we won't grow....Hell No....",
	"Your mom such a slut when a teacher told her to write an essay she went to have sex with a mexican. ESSAY ESAAY ESSAY!",
	"Your old lady's so old, the key on Ben Franklin's kite was to her apartment.",
	"Your mom's ass is so big, she takes up 5 rows of seats in the theater.",
	"Your mom teeth are so yellow when she went to church the priest said I see light!",
	"Your old lady's so old, her memory is in black and white.",
	"Your mum's so dark she bleeds smoke!",
	"Your mother is so ghetto, she wheelies on a ten speed.",
	"Your mother is so old her birth certificate says expired on it.",
	"Your old lady's so skinny they couldn't find her in this joke.",
	"Your mother is so ghetto when you ask her for milk she says pick a boob!",
	"Yo mama's so ghetto, she came to McDonald with a gallon and filled it up with sweet tea!",
	"Your mummy's head is so little, she could wear fruit loops as head rags.",
	"Your mummy's like a race car driver - she burns a lot of rubbers.",
	"Your mothers so fat when she went into space there was no space left.",
	"Your mummy's head is so big, when she tries to tie her shoes, the bitch flips over.",
	"Your mother is so nasty crabs use her as a hide out!",
	"Your mom such a slut, she gained sixty Two pounds just from swallowing!",
	"Your mummy's hair so short she roll her hair up with rice!",
	"Yo mama's so fat,when she went to the theaters everybody yelled, Look King-Kong in 3-D!",
	"Your mom such a slut, I told her I was looking for a job and she started giving me a blowjob.",
	"Your mom is so poor when she heard about the Last Supper she thought she was running out of food stamps!",
	"Your mother is ass is so fat when she sits down she's three feet taller!",
	"Your mother is so old she watched the birth of jesus!",
	"Your mother is so poor she put three peas on the table, I took one and she said Dont be greedy!",
	"Your mother is so nasty she gave your daddy a blow job then came into your room and gave you a kiss goodnight!",
	"Your mum's like an elevator, guys go up and down on her all day.",
	"Your mom is such a slut, her pussy is like Domino's Pizza, if you don't cum in 30 minutes the next one is free!",
	"Your mum's so dirty, I asked her where the junk yard was and she said your looking at it!",
	"Your mom is so skinny,when she got in the bed to have sex, the man said, Where's yo titties!",
	"Your mother is so mean they don't give her happy meals at McDonalds.",
	"Your mom's glasses are so thick she can see into the future.",
	"Your mother is so ghetto she brings ziplock bags in her purse to a buffet.",
	"Your mother is poor when I sat on a skateboard she said (get of my family van)",
	"Your mom then went to Ruben and took his Sandwich.",
	"Your mother is so lazy her constant quandry is should I sit down and do nothing or should I lie down and do nothing!",
	"Your mom such a stupid, when she lost her virginity, she swam all over the world to look for it!",
	"Your mother is so nasty she got a dick transplant just to fuck herself!",
	"Your mother is so old that she dated George Washington in high school.",
	"Your mom is so smart she said Lets get High.........Grades on that test today.",
	"Your motherma so Dumb, she brought a six pack of Budweiser with her to school, hoping she could make her son Bud Wiser.",
	"Your mom is so skinny, I gave her a piece of popcorn and she went into a coma.",
	"Your mum's so dark we use a flash light to see her at night!",
	"Your mom such a slut, she is like a golf course everybody puts there balls in her!",
	"Your mother is so nasty she couldnt even sell her ass for a free sample!",
	"Your old lady's so skinny, both her nipples are touching!",
	"Your mom is such a slut that around her electrons have a positive charge.",
	"Your mother is so mean the only place she visited was the principle Office.",
	"Yo mama's so fat scientists thought Jupiter was her brother.",
	"Yo mama's so nasty, she pours salt water in her drawers to keep the crabs alive.",
	"Your mothers so stupid, she thinks gluteus maximus is a Roman emperor.",
	"Your mother house is so small that when you walk in you already came out!",
	"Yo mama's so nasty lice consider her a great vacation place!",
	"Your mother's such a slut, if her vagina was a video game it would be rated E for Everyone.",
	"Your mother is so greasy she's labeled as an ingredient in Crisco.",
	"Your mom jumped up on the table, spread her legs, and said crabs!",
	"Your mother is so old her license plate number is in roman numerals!",
	"Your mom such a Dumb, when she lost her virginity, she swam all over the world to look for it.",
	"Your mom is so skinny, instead of calling her your parent, you call her transparent.",
	"Your old lady's so old, I told her to act her age and the bitch died.",
	"Yo mama's so ghetto, she was born in a bucket at KFC.",
	"Your mom teeth are so rotten, when she smiles they look like dice.",
	"Your mom is so skinny, I could blind-fold her with dental floss.",
	"Your mom is such a slut a man called her a bitch and she got down on her hands and knees.",
	"Your mother is so old that she knew Gandalf before he had a beard!",
	"Your old lady's so old, her memory is in black and white!",
	"Your mother is so hunchbacked, her husband left her because she couldn't be straight up with him!",
	"Your mom suck so much dick that her voice box is ruined from all the cum!",
	"Your mother is so nasty that when she was born and the doctor slapped her ass she gave him her phone number!",
	"Your mother is so poor her idea of a fortune cookie is a tortilla with a foodstamp.",
	"Your mother is so intelligent, that people don't believe she is a blonde.",
	"Your mother's teeth are so yellow I can't beilieve it's butter!",
	"Your mother's so smart she learned to rollover and beg in one day.",
	"Your mother is so poor she uses a hotdog as a dildo!",
	"Your mother house is so small, her welcome mat only says Well!",
	"Your mom is so skinny, when she sticks her tongue out she looks like a zipper.",
	"Your mom's glasses are so thick she can tell you when your going to get a pimple a week in advance.",
	"Your mummy's legs are so white, they'd disintegrate Dracula.",
	"Your mom stupid she went to mcdonalds, got a hamburger, put a crown on it, and said burger king!",
	"Yo mama's so fat her patronus is a cake.",
	"Your mother's such a slut she's like a chinese finger trap cause men cant get their fingers out of her!",
	"Your mummy's glasses are so thick, when she looks at a map, she can see people waving at her.",
	"Your mother's such a slut, when she was born, the doctor slapped her bottom to make her cry, and she said don't forget to pull my hair!",
	"Your mothers so old her flashbacks are in black and white.",
	"Your mother is so poor she does drive by shootings on the bus.",
	"Yo mama's so nasty she's like a vaccum she sucks blows and gets laid in the closet!",
	"Your mummy's house is so small, when you go in the front door you trip over the back gate.",
	"Your mother is so fricken fat if she could bend over she could disprove the laws of physics!",
	"Your old lady's so skinny, her bra fits better backward.",
	"Your mother is like a gas station, when you pay she pumps!",
	"Your mother is so old,when Jesus said let there be light, she turned on the switch.",
	"Your mom such a slut, that I could've been Yo daddy, but the guy in line behind me had the correct change.",
	"Your mummy's hair is so nappy she needs pain killers to comb her hair.",
	"Your mothers so stupid when the judge said order! she said pie and chips please!",
	"Your mom lips so Big, she got to put her lipstick on with a paint brush!",
	"Your mummy's hair so nappy when she combing, it sound like she's biting an apple!",
	"Your mom such a slut, that i came home and asked what was for dinner. She opened her legs and said tuna suprise!",
	"Your mom such a slut, her legs are just like Librarys. They're always open to the public. You suck!",
	"Your mummy's like a squirrel, she's always got some nuts in her mouth.",
	"Your mummy's like a bowling ball gets fingered three times gets thrown down the ally and comes back for more!",
	"Your mom is so poor when I went over her house and asked what's for dinner she opened her legs and said fish sticks!",
	"Your mum's so dark, she can leave fingerprints on charcoal.",
	"Your mother hair so nappy that whe she went to the jamacians they said oh hell no get out of my shop mon!",
	"Your mom is such a slut, that theres a party in her mouth and everybody's coming!",
	"Your mum's lips are so big, when you smile you wet your hair.",
	"Your mom is so smart she can install an operating system while putting on her makeup.",
	"Your mother is so mean she drinks a bottle of your tears.",
	"Your mother is a convenient proof that the universe is still expanding exponentially!",
	"Your mother is so bald that Vin Diesel thought she was his twin.",
	"Your old lady's so skinny, if she had a sesame seed on her head, she'd look like a push pin.",
	"Yo mama's so ghetto, she washes paper plates.",
	"Your mom is so poor when she heard about the Last Supper she thought she was running out of food stamps!",
	"Yo mama's so ghetto, her idea of a fortune cookie is an Oreo with a food stamp in the middle.",
	"Your mom such a slut when I fingered her my hand came out with Carpal Tunnel.",
	"Your mum's so dark, when she spits, ink comes out her mouth.",
	"Your mom is so sexy, she needs an inhaler, cause she's got that ass ma!",
	"Your mother is so mean that Taylor Swift wrote a song about her.",
	"Your mother hair so nappy, i use that shit to scrub my pans.",
	"Your mother's so skinny, when she holds her arms up she falls through her shirt and hangs herself!",
	"Your mother's teeth are so yellow, when she smiles it looks like a Kraft Singles pack.",
	"Your old lady's so old, she co-wrote the Ten Commandments.",
	"Your mother's so stupid she returned a puzzle because it was broken!",
	"Your mother is so huge, she looked at the Empire State Building, and said, 'Hey look! A Barbie Dream House!'",
	"Your mom is sooooo fat she didnt need to take a flight she was already in China.",
	"Your mother's such a slut, she's been compared to a Big Mac -- full of fat and only worth a buck!",
	"Your mother is so poor she painted the bottom of her shoes red and said, look i got red bottoms!",
	"Your mother is so bald and talented she can blow dry her hair answer yes to all your questions and give head at the same time!",
	"Your mom is so skinny she hid behind a stick during a game of hide and seek!",
	"Your mom is so well-respected within her profession that I bet she'll get another raise and promotion this year despite the downward trajectory of the economy.",
	"Yo mama's so hunchbacked, she hits her head on speed bumps.",
	"Your mother is so ghetto that she saves child support for a new pair of Jordans!",
	"Your mother's such a slut, she doesn't say Cock-a-doodle-do, she says Any cock'll do!",
	"Your mother is like a hockey player, she only showers after three periods.",
	"Your mom is so poor the cockroaches in your home buy the groceries.",
	"Your mother is like a meatlocker every guy wants to store his meat in her!",
	"Your mother's such a Slut, her STD's are running all over in Mexico ilegally.",
	"Your mum's so dark she drinks water and pees coffee.",
	"Your mummy's head is so small, she uses a tea bag for a pillow.",
	"Your mother's such a slut, she listed you as an incedental on her tax returns.",
	"Your mom's glasses are so thick she can tell you how gay the Twilight books are, before ever turning a page.",
	"Your mom is so smart she got into a respectable college.",
	"Your mom such a fat slut, that when people see her giving head on the street, they scream out: THERE SHE BLOWS!",
	"Your mother is so Old She Was A Stripper For Christopher Columbus!",
	"Your mother is so lazy the only two letters of the alphabet she knows is NO.",
	"Your mummy's house is so small, she eats her meals off a dime!",
	"Your mothers so dirty she brings crabs to the beach!",
	"Your old lady's so old she still drives her Model T.",
	"Your mom is so smart people think she's half Asian.",
	"Your mummy's glasses are so thick, she can read all 7 Harry Potter books at the same time.",
	"Your mother is so hungry when she went to the theater to see We Bought A Zoo, she brings barbecue sauce.",
	"Yo mama's so flat, people ask if she's from the second dimension.",
	"Your mother is like a telephone, even a 3 year old can pick her up.",
	"Your mother is so poor on christmas she brought a video tape of other kids opening presents.",
	"Your mummy's head is so big, it shows up on radar.",
	"Your mother is so nasty she made right guard turn left.",
	"Your mum's so dirty, she made Right Guard turn left.",
	"Your old lady's so old, she babysat Adam and Eve!",
	"Your mum's so dark, she got her tattoo done in chalk.",
	"Your mum's so cross-eyed, when she dropped a dime, she thought she picked up two nickels.",
	"Your mother's such a slut, she interned for Bill Clinton!",
	"Your mom is so skinny she hula hoops with a cheerio!",
	"Your mom's ass is so big, her toilet's got a wave machine.",
	"Your mother is like a train, people pay to ride her!",
	"Your mother's teeth so nasty Wiz Khalifa wrote a song about them Black and Yellow.",
	"Your mum's lips are so big, she whispers in her own ear!",
	"Your mom is so poor, burglars break in her house and leave money.",
	"Your mother's such a slut, her sexuality is as straight as a circle.",
	"Your mother is so old when moses parted the red sea she was on the other side fishing!",
	"Your mom teeth so yellow they worth more than 10 gold bars!",
	"Your mummy's hair is so nappy, when she combs her hair her teeth bleed.",
	"Your mother is so old, she dated Marco Polo.",
	"Your mothers so dumb she went to the beach to surf the internet!",
	"Your old lady's so old, her birth-certificate expired.",
	"Your mom is soo stupid, she made an appointment with Dr.Pepper!",
	"Your mother is so lazy she lets serious shit slide......down her leg and into her slippers.",
	"Your mother is so ghetto she uses candy wrappers as wall paper!",
	"Your mothers ass is so thick that Godzilla cant even chew it.",
	"Your mother is so greasy that McDonalds rubbed her body over all the French fries!",
	"Your mum's so dark when she was born her parents said 'oh shit happened'",
	"Your mother is so ghetto that all ya movies and cd's are bootleg!",
	"Your mother is so old she was a waitress at the Last Supper.",
	"Your mom such a slut she got a dick transplant just to fuck herself!",
	"Your mother is so bald that when its cloudy out at night you still see a full moon!",
	"Your mother is so nasty I asked her how to spell PENIS, and she said you should have asked me last night it was at the tip of my tounge.",
	"Your mother is so nasty the fishery be paying her to leave!",
	"Your mother is so greasy Texaco buys Oil from her!",
	"Your mother is so greasy when she slid into second she ended up in Detroit!",
	"Your mom pussy so tight, instead of using a Tampon, she uses a Q-tip.",
	"Your old lady's so old, she got slapped by Eve for blowing Adam.",
	"Your mom is so skinny, she can see through the peephole with both eyes.",
	"Your mother is so nasty when she went to red lobster they kicked her out for bringing her own crabs!",
	"Your old lady's so old, she knew Mr. Clean when he had an afro.",
	"Your mother is so old she knew Captain America before he was frozen in a glacier!",
	"Yo mama sucks cocks in hell!",
	"Yo mama's so fat, she fell off the bed on both sides!",
	"Yo momma's so fat when God said, 'Let there be light,' he asked your mother to move out of the way.",
	"Yo momma so fat, God could not even lift her spirit.",
	"Yo momma so fat, you could slap her but and ride the waves.",
	"yo mama so ugly when she lookes in the mirror Micheal Jackson sings beat it.",
	"Your momma so fat, that when she jumped for joy, she got stuck.",
	"Your momma so fat, she got placed in Gryffindor, Slytherin, Hufflepuff and Ravenclaw.",
	"Yo momma so fat she's got more Chins than a Hong Kong phone book.",
	"Yo momma so fat she broke your family tree.",
	"Yo momma so fat she don't fit in this joke.",
	"Your mamma is so mean, that her standard deviation is negative.",
	"Your mom is so fat that her photograph is in Panaroma mode!!!",
	"Your mom is so fat that she has to wear two watches; as she spans various time zones!",
	"Your mom is so stupid she tried to use substitution to find the definite integral of f(n)=n^2 over the interval 0<2.",
	"Your mother is so ugly, they push her face into dough to make gorrilla cookies.",
	"your momma is so fat, i rolled over to smoke a cigarette and i was still on the bitch!",
	"Your mom is so ugly that your father takes her to work with him so that he doesn't have to kiss her goodbye.",
	"your mom's so fat that she uses a vcr as a beeper.",
	"your momma's so dumb, she thinks that asphalt is rectum trouble!",
	"Your mother is so fat, she plays pool with the planets.",
	"when you were born they slapped your momma instead of your butt!",
	"Yo mama so hairy you almost died of rugburn at birth!",
	"your momma skates down main street with a mattress stapped to her back and she yells curb service!",
	"Your mother is so fat, she went to Sizzlers and the bitch got a group discount!",
	"your mommas so fat that when she went to mcdonalds she got stuck in the golden arch's!",
	"Your mother is so fat shes got three dress sizes, large, x-tra large and OH MY GOD ITS COMING FOR US!",
	"your mommas so fat she has to renew the tags on her underwear at the DMV!",
	"Yo mama so fat you have to grease the door frame and hold a twinkie on the other side just to get her through!",
	"Yo mama's so fat and old that when God said 'Let there be Light', he told her to move her fat ass out of the way.",
	"Yo mama's so fat, a picture of her fell off the wall!",
	"Yo mama's so fat, after she got off the carousel, the horse limped for a week.",
	"Yo mama's so fat, all the restaurants in town have signs that says: 'Maximum Occupancy: 240 Patrons OR Yo Mama'",
	"Yo mama's so fat, her belly button doesn't have lint, it has sweaters.",
	"Yo mama is like a mail box, open day and night.",
	"Your mum is twice the man you are.",
	"Yo momma is so fat that she influences the tides.",
	"Your mother is so old. Whenever she swings her arms up, she flies 3 metres up.",
	"Your mother is so old. If she wanted to see her life flash before her eyes, she would have to fall down the moon!",
	"Your mother is so old. Her breastmilk is powder!",
	"Your mother is so old. Her first christmas, was the first christmas!",
	"Your mother is so old. When I asked her to give me her phone number, she carved a 1 in to a rock and threw it at me!",
	"Your mother is so old. She knew Burger King when he was just a prince.",
	"Your mother is so old. She used to babysit Yoda.",
	"Your mother is so old. Her birth certificate says EXPIRED.",
	"Your mother is so old. When she farts, dust comes out.",
	"Your mother is so old. When I asked her to act her age, she died!",
	"Your mother is so short. You can see her legs on her drivers license photo.",
	"Your mother is so short, she can do the limbo under the door.",
	"Your mother is so short. She ice skates on an ice cube.",
	"Your mother is so short, she hooks up with Legos!",
	"Your mother is so short, she drives Hot Wheels.",
	"Your mother is so short. She committed suicide by jumping off the bed.",
	"Your mother is so short. She broke her leg by getting of the toilet.",
	"Your mother is so short. She is Ray William Johnson's stunt double!",
	"Your mother is so short. She can do backflips under the bed.",
	"Your mother is so short. She can't even play Halo: Reach.",
	"Yo' Momma is so fat that when she dances, she makes the band skip.",
	"Yo' Momma is so fat that when she cut her leg, gravy came out out.",
	"Yo' Momma is so fat that when she was born, the hospital got stretch marks.",
	"Yo' Momma is so fat she aint got cellulite. She's got cellu-heavy.",
	"Yo' Momma is so ugly she had to trick or treat over the phone.",
	"Yo' Momma's mouth is so big that she speaks in surround sound.",
	"Yo' Momma's lips are so big, that chapstick invented a spray.",
	"Yo' Momma is so fat that she went to KFC to get a bucket of chicken and they asked her what size. She said the one on the roof.",
	"Yo' Momma is so fat that she influences the tides.",
	"I cannot comment on the state of your mother because cows are sacred in my country.",
	"Yo' Momma is so stupid that when she heard 90% of all crimes occur around the home, she moved.",
	"Yo' Momma is so stupid that she fell up a flight of stairs.",
	"Yo' Momma is so skinny, her nipples touch.",
	"Yo' Momma is so skinny, she could dive through a fence.",
	"Yo' Momma breath is so bad that her teeth made plans to escape.",
	"Yo' Momma is breath smells so bad that she has prescription strength tic-tacs.",
	"Yo' Momma is teeth are so crooked that her mouth looks like a chic-let parking lot.",
	"Yo mama's teeth are so crooked she can eat corn on the cob through a chain link fence.",
	"Yo' Momma is so stupid she stared at the orange juice container for a half hour because it said concentrate.",
	"They used to call it a jumpoline before your Momma got on it.",
	"Yo mama's so dumb, she couldn't even think up a proper ending to this yo mama joke!",
	"Your momma is the cheapest whore at the local brothel.",
	"Your momma is a notorious self-harmer!",
	"After your momma gave birth to you, when she saw how ugly you were she started cutting for the first time!",

};

/* Insult or intimidate the player */
void
cuss(mtmp)
register struct monst	*mtmp;
{
	int armpro, armprolimit;

	if (mtmp->iswiz) {
	    if (!rn2(5)) { /* typical bad guy action */
		pline("%s laughs fiendishly.", Monnam(mtmp));
		verbalize("%s", random_your_mother[rn2(SIZE(random_your_mother))]);
		u.cnd_mommacount++;

		armpro = magic_negation(&youmonst);
		armprolimit = 75;
		if (!(PlayerCannotUseSkills)) {

			switch (P_SKILL(P_SPIRITUALITY)) {
				default: armprolimit = 75; break;
				case P_BASIC: armprolimit = 78; break;
				case P_SKILLED: armprolimit = 81; break;
				case P_EXPERT: armprolimit = 84; break;
				case P_MASTER: armprolimit = 87; break;
				case P_GRAND_MASTER: armprolimit = 90; break;
				case P_SUPREME_MASTER: armprolimit = 93; break;
			}
		}

		if ((rn2(3) >= armpro) || ((rnd(100) > armprolimit) && ((armpro < 4) || (rnd(armpro) < 4) ) ) ) {
			make_dimmed(HDimmed + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
			if (Role_if(PM_CELLAR_CHILD)) losehp(monster_difficulty(),"Rodney's laughing",KILLED_BY);

		}

	    } else {
		if (u.uhave.amulet && !rn2(SIZE(random_insult)))
		    verbalize("Relinquish the amulet, %s!",
			  random_insult[rn2(SIZE(random_insult))]);
		else if (u.uhp < 5 && !rn2(2))	/* Panic */
		    verbalize(rn2(2) ?
			  "Even now thy life force ebbs, %s!" :
			  "Savor thy breath, %s, it be thy last!",
			  random_insult[rn2(SIZE(random_insult))]);
		else if (mtmp->mhp < 5 && !rn2(2))	/* Parthian shot */
		    verbalize(rn2(2) ?
			      "I shall return." :
			      "I'll be back.");
		else
		    verbalize("%s %s!",
			  random_malediction[rn2(SIZE(random_malediction))],
			  random_insult[rn2(SIZE(random_insult))]);

		armpro = magic_negation(&youmonst);
		armprolimit = 75;
		if (!(PlayerCannotUseSkills)) {

			switch (P_SKILL(P_SPIRITUALITY)) {
				default: armprolimit = 75; break;
				case P_BASIC: armprolimit = 78; break;
				case P_SKILLED: armprolimit = 81; break;
				case P_EXPERT: armprolimit = 84; break;
				case P_MASTER: armprolimit = 87; break;
				case P_GRAND_MASTER: armprolimit = 90; break;
				case P_SUPREME_MASTER: armprolimit = 93; break;
			}
		}

		if ((rn2(3) >= armpro) || ((rnd(100) > armprolimit) && ((armpro < 4) || (rnd(armpro) < 4) ) ) ) {
			make_dimmed(HDimmed + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
			if (Role_if(PM_CELLAR_CHILD)) losehp(monster_difficulty(),"Rodney's taunt",KILLED_BY);
		}
	    }

	} else if(/*is_lminion(mtmp)*/mtmp->data->mlet == S_ANGEL || mtmp->mnum == PM_CHRISTMAS_CHILD || mtmp->mnum == PM_HELLS_ANGEL) /* give angelic maledictions to all angels --Amy */ {
		com_pager(rn2(QTN_ANGELIC - 1 + (Hallucination ? 1 : 0)) +
			      QT_ANGELIC);

		if (!rn2(5)) {
			armpro = magic_negation(&youmonst);
			armprolimit = 75;
			if (!(PlayerCannotUseSkills)) {

				switch (P_SKILL(P_SPIRITUALITY)) {
					default: armprolimit = 75; break;
					case P_BASIC: armprolimit = 78; break;
					case P_SKILLED: armprolimit = 81; break;
					case P_EXPERT: armprolimit = 84; break;
					case P_MASTER: armprolimit = 87; break;
					case P_GRAND_MASTER: armprolimit = 90; break;
					case P_SUPREME_MASTER: armprolimit = 93; break;
				}
			}

			if ((rn2(3) >= armpro) || ((rnd(100) > armprolimit) && ((armpro < 4) || (rnd(armpro) < 4) ) ) ) {
				make_dimmed(HDimmed + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
				if (Role_if(PM_CELLAR_CHILD)) losehp(monster_difficulty(),"angelic maledictions",KILLED_BY);
			}
		}

	} else {
	    if (!rn2(5)) {
		pline("%s casts aspersions on your ancestry.", Monnam(mtmp));
		verbalize("%s", random_your_mother[rn2(SIZE(random_your_mother))]);
		u.cnd_mommacount++;

		armpro = magic_negation(&youmonst);
		armprolimit = 75;
		if (!(PlayerCannotUseSkills)) {

			switch (P_SKILL(P_SPIRITUALITY)) {
				default: armprolimit = 75; break;
				case P_BASIC: armprolimit = 78; break;
				case P_SKILLED: armprolimit = 81; break;
				case P_EXPERT: armprolimit = 84; break;
				case P_MASTER: armprolimit = 87; break;
				case P_GRAND_MASTER: armprolimit = 90; break;
				case P_SUPREME_MASTER: armprolimit = 93; break;
			}
		}

		if ((rn2(3) >= armpro) || ((rnd(100) > armprolimit) && ((armpro < 4) || (rnd(armpro) < 4) ) ) ) {
			make_dimmed(HDimmed + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
			if (Role_if(PM_CELLAR_CHILD)) losehp(monster_difficulty(),"a 'your mother' joke",KILLED_BY);
		}

	    }
	    else {
	      com_pager(rn2(QTN_DEMONIC) + QT_DEMONIC);

		if (!rn2(5)) {
			armpro = magic_negation(&youmonst);
			armprolimit = 75;
			if (!(PlayerCannotUseSkills)) {

				switch (P_SKILL(P_SPIRITUALITY)) {
					default: armprolimit = 75; break;
					case P_BASIC: armprolimit = 78; break;
					case P_SKILLED: armprolimit = 81; break;
					case P_EXPERT: armprolimit = 84; break;
					case P_MASTER: armprolimit = 87; break;
					case P_GRAND_MASTER: armprolimit = 90; break;
					case P_SUPREME_MASTER: armprolimit = 93; break;
				}
			}

			if ((rn2(3) >= armpro) || ((rnd(100) > armprolimit) && ((armpro < 4) || (rnd(armpro) < 4) ) ) ) {
				make_dimmed(HDimmed + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
				if (Role_if(PM_CELLAR_CHILD)) losehp(monster_difficulty(),"demonic maledictions",KILLED_BY);
			}
		}

	    }
	}
}

void
randomcuss()
{
	verbalize("%s", random_your_mother[rn2(SIZE(random_your_mother))]);
	u.cnd_mommacount++;
}

#endif /* OVLB */

/*wizard.c*/
