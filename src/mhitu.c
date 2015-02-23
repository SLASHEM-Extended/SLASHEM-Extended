/*	SCCS Id: @(#)mhitu.c	3.4	2003/11/26	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "artifact.h"
#include "edog.h"

STATIC_VAR NEARDATA struct obj *otmp;

STATIC_DCL void FDECL(urustm, (struct monst *, struct obj *));
# ifdef OVL1
STATIC_DCL boolean FDECL(u_slip_free, (struct monst *,struct attack *));
STATIC_DCL int FDECL(passiveum, (struct permonst *,struct monst *,struct attack *));
# endif /* OVL1 */

#ifdef OVLB
# ifdef SEDUCE
STATIC_DCL void FDECL(mayberem, (struct obj *, const char *));
# endif
#endif /* OVLB */

STATIC_DCL boolean FDECL(diseasemu, (struct permonst *));
STATIC_DCL int FDECL(hitmu, (struct monst *,struct attack *));
STATIC_DCL int FDECL(gulpmu, (struct monst *,struct attack *));
STATIC_DCL int FDECL(explmu, (struct monst *,struct attack *,BOOLEAN_P));
STATIC_DCL void FDECL(missmu,(struct monst *,int,int,struct attack *));
STATIC_DCL void FDECL(mswings,(struct monst *,struct obj *));
STATIC_DCL void FDECL(wildmiss, (struct monst *,struct attack *));

STATIC_DCL void FDECL(hurtarmor,(int));
STATIC_DCL void NDECL(witherarmor);
STATIC_DCL void FDECL(hitmsg,(struct monst *,struct attack *));

/* See comment in mhitm.c.  If we use this a lot it probably should be */
/* changed to a parameter to mhitu. */
static int dieroll;

#ifdef OVL1

STATIC_OVL void
hitmsg(mtmp, mattk)
register struct monst *mtmp;
register struct attack *mattk;
{
	int compat;
	int monsterlev;
	int armproX = 0;
	int randomkick;

	/* Note: if opposite gender, "seductively" */
	/* If same gender, "engagingly" for nymph, normal msg for others */
	if((compat = could_seduce(mtmp, &youmonst, mattk)) && !mtmp->mcan &&
	    !mtmp->mspec_used) {
	        pline("%s %s you %s.", Monnam(mtmp), Blind ? "talks to" :
	              "smiles at", compat == 2 ? "engagingly" :
	              "seductively");
	} /*else*/ switch (mattk->aatyp) {
		case AT_CLAW:
			pline("%s claws you!", Monnam(mtmp));

			if (flags.female && !rn2((u.ualign.type == A_LAWFUL) ? 10 : (u.ualign.type == A_NEUTRAL) ? 7 : 5) ) { 
				pline("%s rips into your breast with maniacal fervor!", Monnam(mtmp));

			monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;

				losehp(d(2,monsterlev), "cowardly attack on her breasts", KILLED_BY_AN);

			}
			break;
		case AT_BITE:
			pline("%s bites you!", Monnam(mtmp));
			armproX = magic_negation(&youmonst);
			if (!rn2((u.ualign.type == A_LAWFUL) ? 100 : (u.ualign.type == A_NEUTRAL) ? 150 : 250) && ((rn2(3) >= armproX) || !rn2(20)) ) {
			if (!Drain_resistance || !rn2(20)) {
			pline("%s sinks %s teeth deep into your skin and drinks your %s!", Monnam(mtmp), mhis(mtmp), body_part(BLOOD));
		      losexp("life drainage", FALSE);
			}
			}
			break;
		case AT_KICK:
			pline("%s kicks you%c", Monnam(mtmp),
				    thick_skinned(youmonst.data) ? '.' : '!');

			if (!flags.female && !rn2((u.ualign.type == A_LAWFUL) ? 10 : (u.ualign.type == A_NEUTRAL) ? 7 : 5) ) { 
				pline("%s's kick painfully slams into your nuts!", Monnam(mtmp));

			monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;

				losehp(d(2,monsterlev), "kick in the nuts", KILLED_BY_AN);

			}

			struct obj *footwear = which_armor(mtmp, W_ARMF);
			if (!rn2(3) && ((footwear && footwear->otyp == WEDGE_SANDALS) || mtmp->data == &mons[PM_ANIMATED_WEDGE_SANDAL]) ) {
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("%s slams %s sandals against your shins! It hurts!", Monnam(mtmp), mhis(mtmp) );
				losehp(d(1,monsterlev), "sandal to the shin bone", KILLED_BY_AN);
			}

			if (!rn2(3) && (footwear && footwear->otyp == DANCING_SHOES) ) {
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("%s slams %s dancing shoes against your shins! You stagger...", Monnam(mtmp), mhis(mtmp) );
				make_stunned(HStun + monsterlev, FALSE);
				losehp(1, "soft dancing shoe", KILLED_BY_AN);
			}

			if (!rn2(3) && (footwear && footwear->otyp == SWEET_MOCASSINS) ) {
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("%s slides %s mocassins over your %ss, opening your arteries and squirting %s everywhere!", Monnam(mtmp), mhis(mtmp), body_part(HAND), body_part(BLOOD) );
				    incr_itimeout(&Glib, monsterlev);
			}

			if (!rn2(3) && (footwear && footwear->otyp == SOFT_SNEAKERS) ) {
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("%s hits your %s with %s sneakers!", Monnam(mtmp), body_part(HAND), mhis(mtmp) );
				    incr_itimeout(&Glib, 2); /* just enough to make you drop your weapon */
				losehp(d(1,monsterlev), "soft sneaker to the hand", KILLED_BY_AN);
			}

			if (!rn2(3) && (footwear && footwear->otyp == HIPPIE_HEELS) ) {
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				randomkick = rnd(15);
				if (randomkick == 1) {
				pline("%s steps on your %s with %s plateau heels!", Monnam(mtmp), body_part(HAND), mhis(mtmp) );
				    incr_itimeout(&Glib, 20); /* painfully jamming your fingers */
				losehp(d(1,monsterlev), "hippie plateau boot", KILLED_BY_AN);
				}
				if (randomkick == 2) {
				pline("%s jams your %ss with %s red whore boots!", Monnam(mtmp), body_part(TOE), mhis(mtmp) );

				register long side = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
				  const char *sidestr = (side == RIGHT_SIDE) ? "right" : "left";
			    set_wounded_legs(side, rnd(60-ACURR(A_DEX)));
			    exercise(A_STR, FALSE);
			    exercise(A_DEX, FALSE);

				losehp(d(1,monsterlev), "red whore boot", KILLED_BY_AN);
				}

				if (randomkick == 3) {
					if (!flags.female) { 
					pline("%s kicks right into your nuts with %s prostitute shoes!", Monnam(mtmp), mhis(mtmp) );

					losehp(d(2,monsterlev), "prostitute shoe to the nuts", KILLED_BY_AN);
					}

					else {
					pline("%s gently strokes you with %s prostitute shoes.", Monnam(mtmp), mhis(mtmp) );
					if (Upolyd) u.mh += monsterlev; /* heal some hit points */
					else u.uhp += monsterlev; /* heal some hit points */
					pline("It feels very soothing.");
					if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
					if (u.mh > u.mhmax) u.mh = u.mhmax;
					}

				}

				if (randomkick == 4) {
				pline("%s painfully stomps your body with %s erotic lady boots!", Monnam(mtmp), mhis(mtmp) );
				u.uhp -= 1;
				u.uhpmax -= 1;
				u.uen -= 1;
				u.uenmax -= 1;
				losehp(d(3,monsterlev), "being stomped by an erotic lady boot", KILLED_BY);
				}

				if (randomkick == 5) {
				pline("You fall in love with %s's incredibly erotic footwear, and are unable to fight back.", mon_nam(mtmp) );
					nomovemsg = "You finally decide to stop admiring the sexy leather boots.";
					nomul(-rnd(5));

				}

				if (randomkick == 6) {
				pline("Think of the sweet red leather your sputa will flow down.");
				morehungry(1000);
	      make_vomiting(Vomiting+20, TRUE);
		if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
				}

				if (randomkick == 7) {
				pline("You decide to close your %ss for a while, thinking about the lovely %s and %s sexy red overknees.", body_part(EYE), m_monnam(mtmp), mhis(mtmp) );
		    make_blinded(Blinded+monsterlev,FALSE);
				}

				if (randomkick == 8) {
				pline("Wow... those wonderful high heels are soooooo mesmerizing and beautiful...");
		make_hallucinated(HHallucination + monsterlev,FALSE,0L);
				}

				if (randomkick == 9) {
				pline("You wonder where %s got %s plateau heels from. Who the hell wears those in a dungeon, anyway? Your %s spins in bewilderment.", mon_nam(mtmp), mhis(mtmp), body_part(HEAD));
		    make_confused(HConfusion+monsterlev,FALSE);
				}

				if (randomkick == 10) {
				pline("Argh! The massive heel strained a vital muscle!");
		    make_stunned(HStun+monsterlev,FALSE);
				}

				if (randomkick == 11) {
				if (!rn2(25)) {
					pline("Fuck! The high heel was contaminated with spores!" );
					    make_sick(rn1(25,25), "contaminated high heel", TRUE, SICK_VOMITABLE);
					losehp(d(1,monsterlev), "unclean high heel", KILLED_BY_AN);
					}
				else {
				pline("The devious %s rubbed %s female boots with venom! How unfair!", m_monnam(mtmp), mhis(mtmp) );
						poisoned("block heel", A_STR, "poisoned block heel", 8);
					losehp(d(1,monsterlev), "impregnated block heel", KILLED_BY_AN);
					}
				}

				if (randomkick == 12) {
				pline("Ow, %s is stomping you repeatedly with %s slutty boots!", m_monnam(mtmp), mhis(mtmp));
		    make_numbed(HNumbed+monsterlev,FALSE);
				}

				if (randomkick == 13) {
				pline("The fact that %s is wearing such incredibly high heels is giving you the creeps.", m_monnam(mtmp));
		    make_frozen(HFrozen+monsterlev,FALSE);
				}

				if (randomkick == 14) {
				pline("Your skin is burned by %s's block-heeled lady boots!", m_monnam(mtmp));
		    make_burned(HBurned+monsterlev,FALSE);
				}

				if (randomkick == 15) {
				pline("You shudder in fear as you see the violent %s using %s high-heeled plateau boots as lethal weapons.", l_monnam(mtmp), mhis(mtmp));
		    make_feared(HFeared+monsterlev,FALSE);
				}

			}

			if (!rn2(3) && ((footwear && footwear->otyp == LEATHER_PEEP_TOES) || mtmp->data == &mons[PM_ANIMATED_LEATHER_PEEP_TOE]) ) {
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("%s slams %s leather peep-toes against your shins!", Monnam(mtmp), mhis(mtmp) );
				losehp(d(1,monsterlev), "asian peep-toe", KILLED_BY_AN);

				if (multi >= 0 && !rn2(2)) {
				    if (flags.female) {
					pline("You congratulate %s for %s valiant attempt to hurt you.", mon_nam(mtmp), mhis(mtmp) );
				    } else if (Free_action) {
					pline("It hurts like hell, but you bear it like a man.");            
				    } else {
					pline("It hurts like hell! You pass out from the intense pain.");            
					nomovemsg = "Still groaning with pain, you get back up, ready to fight again.";
					monsterlev = ((mtmp->m_lev) + 1);
					monsterlev /= 2;
					if (monsterlev <= 0) monsterlev = 1;
					nomul(-monsterlev);
					exercise(A_DEX, FALSE);
				    }
				}
			}

			if (!rn2(3) && ((footwear && footwear->otyp == FEMININE_PUMPS) || mtmp->data == &mons[PM_ANIMATED_SEXY_LEATHER_PUMP] || mtmp->data == &mons[PM_BLOODY_BEAUTIES]) ) {
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("%s scratches up and down your %ss with %s heels!", Monnam(mtmp), body_part(LEG), mhis(mtmp) );
				losehp(d(2,monsterlev), "leg-scratch from feminine pumps", KILLED_BY_AN);

			if (u.legscratching <= 5)
		    	    pline("It stings a little.");
			else if (u.legscratching <= 10)
		    	    pline("It hurts quite a bit as some of your skin is scraped off!");
			else if (u.legscratching <= 20)
			    pline("Blood drips from your %s as the heel scratches over your open wounds!", body_part(LEG));
			else if (u.legscratching <= 40)
			    pline("You can feel the heel scratching on your shin bone! It hurts and bleeds a lot!");
			else
			    pline("You watch in shock as your blood is squirting everywhere, all the while feeling the razor-sharp high heel mercilessly opening your %ss!", body_part(LEG));

			/* Need to add a timeout function that will lower the legscratching variable. --Amy
			   It's in timeout.c and will just lower u.legscratching by one every 1000 ticks for now. */

				losehp(u.legscratching, "bleedout from leg scratches", KILLED_BY_AN);
				u.legscratching++;
				register long side = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
				  const char *sidestr = (side == RIGHT_SIDE) ? "right" : "left";
			    set_wounded_legs(side, rnd(60-ACURR(A_DEX)));
			    exercise(A_STR, FALSE);
			    exercise(A_DEX, FALSE);
			}
			if (!rn2(3) && ((footwear && footwear->otyp == COMBAT_STILETTOS) || mtmp->data == &mons[PM_ANIMATED_COMBAT_STILETTO]) ) {
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("%s does a round-house and painfully hits you with %s combat boots!", Monnam(mtmp), mhis(mtmp) );
				losehp(d(4,monsterlev), "kung-fu attack", KILLED_BY_AN);
			}

			if (!rn2(250)) pushplayer();

			break;
		case AT_STNG:
			pline("%s stings you!", Monnam(mtmp));
			if (!rn2((u.ualign.type == A_LAWFUL) ? 300 : (u.ualign.type == A_NEUTRAL) ? 250 : 300)) {
			pline("You are bleeding out from your stinging injury!");
			monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;

			if (Upolyd) {u.mhmax -= monsterlev; if (u.mh > u.mhmax) u.mh = u.mhmax;}
			else {u.uhpmax -= monsterlev; if (u.uhp > u.uhpmax) u.uhp = u.uhpmax; }


			}
			break;
		case AT_BUTT:
			pline("%s butts you!", Monnam(mtmp));

			if (multi >= 0 && !rn2((u.ualign.type == A_LAWFUL) ? 40 : (u.ualign.type == A_NEUTRAL) ? 33 : 50)) {
			    if (Free_action) {
				You("feel a slight shaking.");            
			    } else {
				You("flinch!");
				nomovemsg = 0;	/* default: "you can move again" */
				monsterlev = ((mtmp->m_lev) + 1);
				monsterlev /= 3;
				if (monsterlev <= 0) monsterlev = 1;
				nomul(-monsterlev);
				exercise(A_DEX, FALSE);
			    }
			}

			if (!rn2(50)) pushplayer();

			break;
		case AT_SCRA:
			pline("%s scratches you!", Monnam(mtmp));
			if (!rn2((u.ualign.type == A_LAWFUL) ? 100 : (u.ualign.type == A_NEUTRAL) ? 150 : 125)) {
			pline("One of your arteries bursts open! You suffer from %s loss!", body_part(BLOOD));
			monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
			losehp((monsterlev), "scratching attack", KILLED_BY_AN);

			if (Upolyd) {u.mhmax -= monsterlev/2; if (u.mh > u.mhmax) u.mh = u.mhmax;}
			else {u.uhpmax -= monsterlev/2; if (u.uhp > u.uhpmax) u.uhp = u.uhpmax; }

			}
			break;
		case AT_LASH:
			pline("%s lashes you!", Monnam(mtmp));
			if (!rn2((u.ualign.type == A_LAWFUL) ? 15 : (u.ualign.type == A_NEUTRAL) ? 20 : 10)) {
			monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("Your %s spins in confusion.", body_part(HEAD));
			make_confused(HConfusion + monsterlev, FALSE);
			}

			if (!rn2(200)) pushplayer();
			break;
		case AT_TRAM:
			pline("%s tramples over you!", Monnam(mtmp));
			if (!rn2(5)) {
			monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("You can't think straight as your every muscle is aching!");
			make_stunned(HStun + monsterlev, FALSE);
			}
			if (!rn2(25)) pushplayer();
			break;
		case AT_TUCH:
			pline("%s touches you!", Monnam(mtmp));
			losehp(1, "icy touch", KILLED_BY_AN);

			if (mtmp->data == &mons[PM_BLACK_DEATH]) { /* lose one maximum HP --Amy */

				if (Upolyd) {u.mhmax -= 1; if (u.mh > u.mhmax) u.mh = u.mhmax;}
				else {u.uhpmax -= 1; if (u.uhp > u.uhpmax) u.uhp = u.uhpmax; }

			}

			break;
		case AT_TENT:
			pline("%s tentacles suck you!",
				        s_suffix(Monnam(mtmp)));
			monsterlev = ((mtmp->m_lev) + 1);
			monsterlev /= 5;
			if (monsterlev <= 0) monsterlev = 1;
			losehp((monsterlev), "sucking tentacle attack", KILLED_BY_AN);
			break;
		case AT_EXPL:
		case AT_BOOM:
			pline("%s explodes!", Monnam(mtmp));
			break;
		case AT_MULTIPLY:
			/* No message. */
		break;
		default:
			pline("%s hits you!", Monnam(mtmp));
	    }
}


STATIC_OVL void
missmu(mtmp, target, roll, mattk)           /* monster missed you */
register struct monst *mtmp;
register int target;
register int roll;
register struct attack *mattk;
{
	register boolean nearmiss = (target == roll);
	register struct obj *blocker = (struct obj *)0;	
		/* 3 values for blocker
		 *	No blocker:  (struct obj *) 0  
		 * 	Piece of armour:  object
		 *	magical: &zeroobj
		 */

	if (target < roll) {
		/* get object responsible 
		 * Work from the closest to the skin outwards
		 */
#ifdef TOURIST
		/* Try undershirt if tourist */
		if (uarmu && target <= roll) {
			target += ARM_BONUS(uarmu);
			if (target > roll) blocker = uarmu;
		}
#endif
		/* Try body armour */
		if (uarm && target <= roll) {
			target += ARM_BONUS(uarm);
			if (target > roll) blocker = uarm;
		}

		if (uarmg && !rn2(10)) {
			/* Try gloves */
			target += ARM_BONUS(uarmg);
			if (target > roll) blocker = uarmg;
		}
		if (uarmf && !rn2(10)) {
			/* Try boots */
			target += ARM_BONUS(uarmf);
			if (target > roll) blocker = uarmf;
		}
		if (uarmh && !rn2(5)) {
			/* Try helm */
			target += ARM_BONUS(uarmh);
			if (target > roll) blocker = uarmh;
		}
		if (uarmc && target <= roll) {
			/* Try cloak */
			target += ARM_BONUS(uarmc);
			if (target > roll) blocker = uarmc;
		}
		if (uarms && target <= roll) {
			/* Try shield */
			target += ARM_BONUS(uarms);
			if (target > roll) blocker = uarms;
		}
		if (target <= roll) {
			/* Try spell protection */
			target += u.uspellprot;
			if (target > roll) blocker = &zeroobj;
		}			
	}

	if (!canspotmon(mtmp))
	    map_invisible(mtmp->mx, mtmp->my);

	if(could_seduce(mtmp, &youmonst, mattk) && !mtmp->mcan)
	    pline("%s pretends to be friendly.", Monnam(mtmp));
	else {
	    if (!flags.verbose || !nearmiss && !blocker)
		pline("%s misses.", Monnam(mtmp));
	    else if (!blocker)
		pline("%s just misses!", Monnam(mtmp));
	    else if (blocker == &zeroobj)
		pline("%s is stopped by the golden haze.", Monnam(mtmp));
	    else
		Your("%s %s%s %s attack.", 
			simple_typename(blocker->otyp),
			rn2(2) ? "block" : "deflect",
			(blocker == uarmg || blocker == uarmf) ? "" : "s",
			s_suffix(mon_nam(mtmp)));

	    if (MON_WEP(mtmp)) {
		struct obj *obj = MON_WEP(mtmp);
		obj->owornmask &= ~W_WEP;
		if (rnd(100) < (obj->oeroded * 5 / 2)) {
		    if (obj->spe > -5) {    
			obj->spe--;
			pline("%s %s is damaged further!",
				s_suffix(Monnam(mtmp)), xname(obj));
		    } else
			pline("%s %s is badly battered!", 
				s_suffix(Monnam(mtmp)), xname(obj));
		}
	    }
	}
	stop_occupation();
}

STATIC_OVL void
mswings(mtmp, otemp)		/* monster swings obj */
register struct monst *mtmp;
register struct obj *otemp;
{
        if (!flags.verbose || Blind || !mon_visible(mtmp)) return;
	pline("%s %s %s %s.", Monnam(mtmp),
	      (objects[otemp->otyp].oc_dir & PIERCE) ? "thrusts" : "swings",
	      mhis(mtmp), singular(otemp, xname));
}

/* return how a poison attack was delivered */
const char *
mpoisons_subj(mtmp, mattk)
struct monst *mtmp;
struct attack *mattk;
{
	if (mattk->aatyp == AT_WEAP) {
	    struct obj *mwep = (mtmp == &youmonst) ? uwep : MON_WEP(mtmp);
	    /* "Foo's attack was poisoned." is pretty lame, but at least
	       it's better than "sting" when not a stinging attack... */
	    return (!mwep || !mwep->opoisoned) ? "attack" : "weapon";
	} else {
	    return (mattk->aatyp == AT_TUCH) ? "contact" :
		   (mattk->aatyp == AT_GAZE) ? "gaze" :
		   (mattk->aatyp == AT_BITE) ? "bite" : "sting";
	}
}

/* called when your intrinsic speed is taken away */
void
u_slow_down()
{
	HFast = 0L;
	if (!Fast) You("slow down.");
	   /* speed boots */
	else Your("quickness feels less natural.");
	exercise(A_DEX, FALSE);
}

#endif /* OVL1 */
#ifdef OVLB

STATIC_OVL void
wildmiss(mtmp, mattk)		/* monster attacked your displaced image */
	register struct monst *mtmp;
	register struct attack *mattk;
{
	int compat;

	/* no map_invisible() -- no way to tell where _this_ is coming from */

	if (!flags.verbose) return;
	if (!cansee(mtmp->mx, mtmp->my)) return;
		/* maybe it's attacking an image around the corner? */

	compat = (mattk->adtyp == AD_SEDU || mattk->adtyp == AD_SSEX) &&
		 could_seduce(mtmp, &youmonst, (struct attack *)0);

	if (!mtmp->mcansee || (Invis && !perceives(mtmp->data))) {
	    const char *swings =
		mattk->aatyp == AT_BITE ? "snaps" :
		mattk->aatyp == AT_KICK ? "kicks" :
		mattk->aatyp == AT_LASH ? "lashes" :
		mattk->aatyp == AT_TRAM ? "stomps" :
		(mattk->aatyp == AT_STNG ||
		 mattk->aatyp == AT_BUTT ||
		 nolimbs(mtmp->data)) ? "lunges" : "swings";

	    if (compat)
		pline("%s tries to touch you and misses!", Monnam(mtmp));
	    else
		switch(rn2(3)) {
		case 0: pline("%s %s wildly and misses!", Monnam(mtmp),
			      swings);
		    break;
		case 1: pline("%s attacks a spot beside you.", Monnam(mtmp));
		    break;
		case 2: pline("%s strikes at %s!", Monnam(mtmp),
				levl[mtmp->mux][mtmp->muy].typ == WATER
				    ? "empty water" : "thin air");
		    break;
		default:pline("%s %s wildly!", Monnam(mtmp), swings);
		    break;
		}
	} else if (Displaced) {
	    if (compat)
		pline("%s smiles %s at your %sdisplaced image...",
			Monnam(mtmp),
			compat == 2 ? "engagingly" : "seductively",
			Invis ? "invisible " : "");
	    else
		pline("%s strikes at your %sdisplaced image and misses you!",
			/* Note: if you're both invisible and displaced,
			 * only monsters which see invisible will attack your
			 * displaced image, since the displaced image is also
			 * invisible.
			 */
                        Monnam(mtmp),Invis ? "invisible " : "");

	} else if (Underwater) {
	    /* monsters may miss especially on water level where
	       bubbles shake the player here and there */
	    if (compat)
		pline("%s reaches towards your distorted image.",Monnam(mtmp));
	    else
		pline("%s is fooled by water reflections and misses!",Monnam(mtmp));

	} else impossible("%s attacks you without knowing your location?",
		Monnam(mtmp));
}

void
expels(mtmp, mdat, message)
register struct monst *mtmp;
register struct permonst *mdat; /* if mtmp is polymorphed, mdat != mtmp->data */
boolean message;
{
	if (message) {
		if (is_animal(mdat))
			You("get regurgitated!");
		else {
			char blast[40];
			register int i;

			blast[0] = '\0';
			for(i = 0; i < NATTK; i++)
				if(mdat->mattk[i].aatyp == AT_ENGL)
					break;
			if (mdat->mattk[i].aatyp != AT_ENGL)
			      impossible("Swallower has no engulfing attack?");
			else {
				if (is_whirly(mdat)) {
					switch (mdat->mattk[i].adtyp) {
						case AD_ELEC:
							Strcpy(blast,
						      " in a shower of sparks");
							break;
						case AD_COLD:
							Strcpy(blast,
							" in a blast of frost");
							break;
					}
				} else
					Strcpy(blast, " with a squelch");
                                You("get expelled from %s%s!",mon_nam(mtmp), blast);
			}
		}
	}
	unstuck(mtmp);	/* ball&chain returned in unstuck() */
	mnexto(mtmp);
	newsym(u.ux,u.uy);
	spoteffects(TRUE);
	/* to cover for a case where mtmp is not in a next square */
	if(um_dist(mtmp->mx,mtmp->my,1))
		pline("Brrooaa...  You land hard at some distance.");
}

#endif /* OVLB */
#ifdef OVL0

/* select a monster's next attack, possibly substituting for its usual one */
struct attack *
getmattk(mptr, indx, prev_result, alt_attk_buf)
struct permonst *mptr;
int indx, prev_result[];
struct attack *alt_attk_buf;
{
    struct attack *attk = &mptr->mattk[indx];

    /* prevent a monster with two consecutive disease or hunger attacks
       from hitting with both of them on the same turn; if the first has
       already hit, switch to a stun attack for the second */
    if (indx > 0 && prev_result[indx - 1] > 0 &&
	    (attk->adtyp == AD_DISE ||
		attk->adtyp == AD_PEST ||
		attk->adtyp == AD_FAMN) &&
	    attk->adtyp == mptr->mattk[indx - 1].adtyp && rn2(5) /* let's be a sadistic programmer --Amy */ ) {
	*alt_attk_buf = *attk;
	attk = alt_attk_buf;
	attk->adtyp = AD_STUN;
    }
    return attk;
}

/* Intelligent monsters try and avoid "blue on blue" incidents.
 */
STATIC_OVL int
blue_on_blue(mtmp)
struct monst *mtmp;
{
    int x, y;
    struct monst *mon;
    if (!mtmp->mconf && !Conflict && !mtmp->mflee && !mindless(mtmp->data)) {
	if (!lined_up(mtmp))
	    return FALSE;	/* Irrelevant; monster won't attack anyway */
	x = mtmp->mx + sgn(tbx);
	y = mtmp->my + sgn(tby);
	while(x != mtmp->mux || y != mtmp->muy) {
	    mon = m_at(x, y);
	    if (mon && m_cansee(mtmp, x, y) && !mon->mundetected &&
		    (!mon->minvis || perceives(mtmp->data)))
		return TRUE;
	    x += sgn(tbx);
	    y += sgn(tby);
	}
    }
    return FALSE;
}

/*
 * mattacku: monster attacks you
 *	returns 1 if monster dies (e.g. "yellow light"), 0 otherwise
 *	Note: if you're displaced or invisible the monster might attack the
 *		wrong position...
 *	Assumption: it's attacking you or an empty square; if there's another
 *		monster which it attacks by mistake, the caller had better
 *		take care of it...
 */
int
mattacku(mtmp)
	register struct monst *mtmp;
{
	struct	attack	*mattk, alt_attk;
	int	i, j, tmp, sum[NATTK];
	struct	permonst *mdat = mtmp->data;
	boolean ranged = (distu(mtmp->mx, mtmp->my) > 3);
		/* Is it near you?  Affects your actions */
	boolean range2 = !monnear(mtmp, mtmp->mux, mtmp->muy);
		/* Does it think it's near you?  Affects its actions */
	boolean foundyou = (mtmp->mux==u.ux && mtmp->muy==u.uy);
		/* Is it attacking you or your image? */
	boolean youseeit = canseemon(mtmp);
		/* Might be attacking your image around the corner, or
		 * invisible, or you might be blind....
		 */

	/* you can attack land-based monsters while underwater, so why should YOU be protected from THEIR attacks??? --Amy */
	if(!ranged) nomul(0);
	if(mtmp->mhp <= 0 /*|| (Underwater && !is_swimmer(mtmp->data))*/)
	    return(0);

	/* If swallowed, can only be affected by u.ustuck */
	if(u.uswallow) {
		if(mtmp != u.ustuck) return(0);
	    u.ustuck->mux = u.ux;
	    u.ustuck->muy = u.uy;
	    range2 = 0;
	    foundyou = 1;
	    if(u.uinvulnerable) return (0); /* stomachs can't hurt you! */
	}

#ifdef STEED
	else if (u.usteed) {
		if (mtmp == u.usteed)
			/* Your steed won't attack you */
			return (0);
		/* Orcs like to steal and eat horses and the like */
		if (!rn2(is_orc(mtmp->data) ? 2 : 4) &&
				distu(mtmp->mx, mtmp->my) <= 2) {
			/* Attack your steed instead */
			i = mattackm(mtmp, u.usteed);
			if ((i & MM_AGR_DIED))
				return (1);
			if (i & MM_DEF_DIED || u.umoved)
				return (0);
			/* Let your steed retaliate */
			return (!!(mattackm(u.usteed, mtmp) & MM_DEF_DIED));
		}
	}
#endif

	if (u.uundetected && !range2 && foundyou && !u.uswallow) {
		u.uundetected = 0;
		if (is_hider(youmonst.data)) {
		    coord cc; /* maybe we need a unexto() function? */
		    struct obj *obj;

		    You("fall from the %s!", ceiling(u.ux,u.uy));
		    if (enexto(&cc, u.ux, u.uy, youmonst.data)) {
			remove_monster(mtmp->mx, mtmp->my);
			newsym(mtmp->mx,mtmp->my);
			place_monster(mtmp, u.ux, u.uy);
			if(mtmp->wormno) worm_move(mtmp);
			teleds(cc.x, cc.y, TRUE);
			set_apparxy(mtmp);
			newsym(u.ux,u.uy);
		    } else {
			pline("%s is killed by a falling %s (you)!",
					Monnam(mtmp), youmonst.data->mname);
			killed(mtmp);
			newsym(u.ux,u.uy);
			if (mtmp->mhp > 0) return 0;
			else return 1;
		    }
		    if (youmonst.data->mlet != S_PIERCER)
			return(0);	/* trappers don't attack */

		    obj = which_armor(mtmp, WORN_HELMET);
		    if (obj && is_metallic(obj)) {
			Your("blow glances off %s helmet.",
			               s_suffix(mon_nam(mtmp)));
		    } else {
			if (3 + find_mac(mtmp) <= rnd(20)) {
			    pline("%s is hit by a falling piercer (you)!",
								Monnam(mtmp));
			    if ((mtmp->mhp -= d(3,6)) < 1)
				killed(mtmp);
			} else
			  pline("%s is almost hit by a falling piercer (you)!",
								Monnam(mtmp));
		    }
		} else {
		    if (!youseeit)
			pline("It tries to move where you are hiding.");
		    else {
			/* Ugly kludge for eggs.  The message is phrased so as
			 * to be directed at the monster, not the player,
			 * which makes "laid by you" wrong.  For the
			 * parallelism to work, we can't rephrase it, so we
			 * zap the "laid by you" momentarily instead.
			 */
			struct obj *obj = level.objects[u.ux][u.uy];

			if (obj ||
			      (youmonst.data->mlet == S_EEL && is_pool(u.ux, u.uy))) {
			    int save_spe = 0; /* suppress warning */
			    if (obj) {
				save_spe = obj->spe;
				if (obj->otyp == EGG) obj->spe = 0;
			    }
			    if (youmonst.data->mlet == S_EEL)
		pline("Wait, %s!  There's a hidden %s named %s there!",
				m_monnam(mtmp), youmonst.data->mname, plname);
			    else
	     pline("Wait, %s!  There's a %s named %s hiding under %s!",
				m_monnam(mtmp), youmonst.data->mname, plname,
				doname(level.objects[u.ux][u.uy]));
			    if (obj) obj->spe = save_spe;
			} else
			    impossible("hiding under nothing?");
		    }
		    newsym(u.ux,u.uy);
		}
		return(0);
	}
	if (youmonst.data->mlet == S_MIMIC && youmonst.m_ap_type &&
		    !range2 && foundyou && !u.uswallow) {
		if (!youseeit) pline("It gets stuck on you.");
		else pline("Wait, %s!  That's a %s named %s!",
			   m_monnam(mtmp), youmonst.data->mname, plname);
		setustuck(mtmp);
		youmonst.m_ap_type = M_AP_NOTHING;
		youmonst.mappearance = 0;
		newsym(u.ux,u.uy);
		return(0);
	}

	/* player might be mimicking an object */
	if (youmonst.m_ap_type == M_AP_OBJECT && !range2 && foundyou && !u.uswallow) {
	    if (!youseeit)
		 pline("%s %s!", Something,
			(likes_gold(mtmp->data) && youmonst.mappearance == GOLD_PIECE) ?
			"tries to pick you up" : "disturbs you");
	    else pline("Wait, %s!  That %s is really %s named %s!",
			m_monnam(mtmp),
			mimic_obj_name(&youmonst),
			an(mons[u.umonnum].mname),
			plname);
	    if (multi < 0) {	/* this should always be the case */
		char buf[BUFSZ];
		Sprintf(buf, "You appear to be %s again.",
			Upolyd ? (const char *) an(youmonst.data->mname) :
			    (const char *) "yourself");
		unmul(buf);	/* immediately stop mimicking */
	    }
	    return 0;
	}

/*	Work out the armor class differential	*/
	tmp = AC_VALUE(u.uac) + 10;		/* tmp ~= 0 - 20 */
	tmp += mtmp->m_lev;
	if(multi < 0) tmp += 4;
        if((Invis && !perceives(mdat)) || !mtmp->mcansee) tmp -= 2;
	if(mtmp->mtrapped) tmp -= 2;
	if(tmp <= 0) tmp = 1;

	/* make eels visible the moment they hit/miss us */
	if(mdat->mlet == S_EEL && mtmp->minvis && cansee(mtmp->mx,mtmp->my) && rn2(5) ) {
		mtmp->minvis = 0;
		newsym(mtmp->mx,mtmp->my);
	}
	/* but not always --Amy */

	if(mdat->mlet == S_FLYFISH && mtmp->minvis && cansee(mtmp->mx,mtmp->my) && !rn2(5) ) {
		mtmp->minvis = 0;
		newsym(mtmp->mx,mtmp->my);
	}

	/* Make Star Vampires visible the moment they hit/miss us */
	if(mtmp->data == &mons[PM_STAR_VAMPIRE] && mtmp->minvis
	   && cansee(mtmp->mx, mtmp->my)) {
	    mtmp->minvis = 0;
	    newsym(mtmp->mx, mtmp->my);
	}

/*	Special demon handling code */
	if(!mtmp->cham && is_demon(mdat) && !range2
	   && mtmp->data != &mons[PM_BALROG]
	   && mtmp->data != &mons[PM_SUCCUBUS]
	   && mtmp->data != &mons[PM_INCUBUS]
 	   && mtmp->data != &mons[PM_NEWS_DAEMON]
 	   && mtmp->data != &mons[PM_PRINTER_DAEMON])
	    if(!mtmp->mcan && !rn2(23))	{msummon(mtmp); pline("%s opens a gate!",Monnam(mtmp) );}


/*	Special arbitrator handling code --Amy */

	if (mtmp->data == &mons[PM_ARBITRATOR] && !rn2(25) ) {

		wake_nearby();
		pline("%s blows a whistle!",Monnam(mtmp) ); 
		/* even if the player stole it; arbitrators can somehow whistle anyway */
	}

/*	Special lycanthrope handling code */
	if(!mtmp->cham && is_were(mdat) && !range2) {
	    if(is_human(mdat)) {
		if(!rn2(15 - (night() * 5)) && !mtmp->mcan) new_were(mtmp);
	    } else if(!rn2(30) && !mtmp->mcan) new_were(mtmp);
	    mdat = mtmp->data;

	    if(!rn2(10) && !mtmp->mcan) {
	    	int numseen, numhelp;
		char buf[BUFSZ], genericwere[BUFSZ];

		Strcpy(genericwere, "creature");
		numhelp = were_summon(mdat, FALSE, &numseen, genericwere);
		if (youseeit) {
			pline("%s summons help!", Monnam(mtmp));
			if (numhelp > 0) {
			    if (numseen == 0)
				You_feel("hemmed in.");
			} else pline("But none comes.");
		} else {
			const char *from_nowhere;

			if (flags.soundok) {
				pline("%s %s!", Something,
					makeplural(growl_sound(mtmp)));
				from_nowhere = "";
			} else from_nowhere = " from nowhere";
			if (numhelp > 0) {
			    if (numseen < 1) You_feel("hemmed in.");
			    else {
				if (numseen == 1)
			    		Sprintf(buf, "%s appears",
							an(genericwere));
			    	else
			    		Sprintf(buf, "%s appear",
							makeplural(genericwere));
				pline("%s%s!", upstart(buf), from_nowhere);
			    }
			} /* else no help came; but you didn't know it tried */
		}
	    }
	}

	if(u.uinvulnerable) {
	    /* monsters won't attack you */
	    if(mtmp == u.ustuck)
		pline("%s loosens its grip slightly.", Monnam(mtmp));
	    else if(!range2) {
		if (youseeit || sensemon(mtmp))
		    pline("%s starts to attack you, but pulls back.",
			  Monnam(mtmp));
		else
		    You_feel("%s move nearby.", something);
	    }
	    return (0);
	}

	/* Unlike defensive stuff, don't let them use item _and_ attack. */
	if(!blue_on_blue(mtmp) && find_offensive(mtmp)) {
		int foo = use_offensive(mtmp);

		if (foo != 0) return(foo==1);
	}

	for(i = 0; i < NATTK; i++) {

	    sum[i] = 0;
	    mattk = getmattk(mdat, i, sum, &alt_attk);
	    if (u.uswallow && (mattk->aatyp != AT_ENGL))
		continue;
	    switch(mattk->aatyp) {
		case AT_CLAW:	/* "hand to hand" attacks */
		case AT_KICK:
		case AT_BITE:
		case AT_LASH:
		case AT_TRAM:
		case AT_SCRA:
		case AT_STNG:
		case AT_TUCH:
		case AT_BUTT:
		case AT_TENT:
			if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
					!touch_petrifies(youmonst.data))) {
			    if (foundyou) {
				if(tmp > (j = rnd(20+i))) {
				    if (mattk->aatyp != AT_KICK ||
					    !thick_skinned(youmonst.data))
					sum[i] = hitmu(mtmp, mattk);
				} else
				    missmu(mtmp, tmp, j, mattk);
			    } else wildmiss(mtmp, mattk);
			}
			break;
		case AT_HUGS:	/* automatic if prev two attacks succeed */
			/* Note: if displaced, prev attacks never succeeded */
		/* Note by Amy: come on, allow it to hit sometimes even if there are no previous attacks (shambling horror)! */
		                if((!range2 && i>=2 && sum[i-1] && sum[i-2]) || mtmp == u.ustuck || !rn2(20) )
				sum[i]= hitmu(mtmp, mattk);
		/* This has the side effect of AT_HUGS hitting from far away. I decided to declare this "bug" a feature. */
			break;
		case AT_GAZE:	/* can affect you either ranged or not */
			/* Medusa gaze already operated through m_respond in
			 * dochug(); don't gaze more than once per round.
			 */
			if (mdat != &mons[PM_MEDUSA])
				sum[i] = gazemu(mtmp, mattk);
			break;
		case AT_EXPL:	/* automatic hit if next to, and aimed at you */
			if(!range2) sum[i] = explmu(mtmp, mattk, foundyou);
			break;
		case AT_ENGL:
			if (!range2) {
			    if(foundyou) {
				if((u.uswallow || tmp > (j = rnd(20+i))) && rn2(10)) { /* 10% chance to miss --Amy */
				    /* Force swallowing monster to be
				     * displayed even when player is
				     * moving away */
				    flush_screen(1);
				    sum[i] = gulpmu(mtmp, mattk);
				} else {
		                                missmu(mtmp, tmp, j, mattk);
				}
			    } else if (is_animal(mtmp->data)) {
				pline("%s gulps some air!", Monnam(mtmp));
			    } else {
				if (youseeit)
				    pline("%s lunges forward and recoils!",
					  Monnam(mtmp));
				else
				    You_hear("a %s nearby.",
					     is_whirly(mtmp->data) ?
						"rushing noise" : "splat");
			   }
			}
			break;
		case AT_BREA:
			if (range2 && !blue_on_blue(mtmp))
			    sum[i] = breamu(mtmp, mattk);
			/* Note: breamu takes care of displacement */
			break;
		case AT_SPIT:
			if (range2 && !blue_on_blue(mtmp))
			    sum[i] = spitmu(mtmp, mattk);
			/* Note: spitmu takes care of displacement */
			break;
		case AT_MULTIPLY:
			/*
			 * Monster multiplying is an AT_ for the following
			 * reasons:
			 *   1. Monsters will only multiply when they're close
			 *      to you.  The whole level will not become clogged
			 *      up with giant lice from monsters multiplying
			 *      where you can't see them.
			 *   2. Tame monsters won't multiply.  Too bad! (unless
			 *      they are conflicted or confused from hunger.
			 *      A bit of a "tactic" -- but then you'll have to
			 *      let them bite you, and anyway who really wants
			 *      a dozen pet fleas to feed?)
			 *   3. Monsters have to be next to you to multiply.
			 *      This makes the inevitable altar abuse a little
			 *      harder.
			 *   4. Elbereth will stop monsters multiplying.
			 *      Otherwise a ring of conflict would crowd out a
			 *      whole level in no time.
			 *   5. It is a hack.  (Shrug)
			 *
			 * Multiplying monsters must be low-level and
			 * low-frequency, so as to minimise altar/experience
			 * abuse.  Any multiplying monsters above about
			 * level 5 should be G_NOCORPSE.
			 *
			 * RJ
			 */
			if (!range2 && (!rn2(5)) ) { /* greatly reduced chance --Amy */

			    pline("%s multiplies!",Monnam(mtmp) );
			    clone_mon(mtmp, 0, 0);
				}
			break;
		case AT_WEAP:
			if(range2) {
#ifdef REINCARNATION
				if (!Is_rogue_level(&u.uz))
#endif
				    if (!blue_on_blue(mtmp))
					thrwmu(mtmp);
			} else {
			    int hittmp = 0;

			    /* Rare but not impossible.  Normally the monster
			     * wields when 2 spaces away, but it can be
			     * teleported or whatever....
			     */
			     if (mtmp->weapon_check == NEED_WEAPON || !MON_WEP(mtmp)) {
				mtmp->weapon_check = NEED_HTH_WEAPON;
				/* mon_wield_item resets weapon_check as
				 * appropriate */
				if (mon_wield_item(mtmp) != 0) break;
			    }
			    if (foundyou) {
				otmp = MON_WEP(mtmp);
				if (otmp) {
				    hittmp = hitval(otmp, &youmonst);
				    tmp += hittmp;
				    mswings(mtmp, otmp);
					if (!rn2(3) && otmp->otyp == WEDGED_LITTLE_GIRL_SANDAL) {
					pline("The massive wedge heel thunders painfully on your %s!", body_part(HEAD));
					losehp(rnd(4),"a wedged little-girl sandal",KILLED_BY);
					}
					if (!rn2(3) && otmp->otyp == SOFT_GIRL_SNEAKER) {
					pline("The soft leather sneaker actually feels quite soothing.");

					if (Upolyd) u.mh++; /* heal one hit point */
					else u.uhp++; /* heal one hit point */

					if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
					if (u.mh > u.mhmax) u.mh = u.mhmax;
					}
					if (!rn2(3) && otmp->otyp == STURDY_PLATEAU_BOOT_FOR_GIRLS) {
					pline("The unyielding plateau boot bonks your %s!", body_part(HEAD));
					losehp(rnd(10),"a sturdy plateau boot for girls",KILLED_BY);
					}

					if (!rn2(3) && otmp->otyp == BLOCK_HEELED_COMBAT_BOOT) {

					if (flags.female) {
						pline("The massive heel hits your %s. Wow, this feels soothing and lovely!", body_part(HEAD));

						if (Upolyd) u.mh++; /* heal one hit point */
						else u.uhp++; /* heal one hit point */

						if (!rn2(5)) {
							if (Upolyd) u.mh += 5; /* heal one hit point */
							else u.uhp += 5; /* heal one hit point */
						}

						if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
						if (u.mh > u.mhmax) u.mh = u.mhmax;

						pline("You feel a strange sensation.");
						make_confused(HConfusion + rnd(4), FALSE);
					}
					else {

					pline("The massive heel painfully hits your %s!", body_part(HEAD));
					losehp(rnd(12),"a block-heeled combat boot",KILLED_BY);
						pline("You're seeing little asterisks everywhere.");
						make_confused(HConfusion + rnd(10), FALSE);
						}
					}

					if (!rn2(3) && otmp->otyp == HUGGING_BOOT) {
					pline("Uff! Your %s got hit hard!", body_part(HEAD));
					losehp(rnd(12),"a hugging boot",KILLED_BY);
						if (Upolyd) u.mhmax--; /* lose one hit point */
						else u.uhpmax--; /* lose one hit point */
						if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
						if (u.mh > u.mhmax) u.mh = u.mhmax;
					}

					if (!rn2(3) && otmp->otyp == WOODEN_GETA) {
					pline("Argh! The unyielding piece of wood painfully lands on your %s!", body_part(HEAD));
					losehp(rnd(15),"a wooden Japanese sandal",KILLED_BY);

					if (multi >= 0 && !rn2(2)) {
					    if (Free_action) {
						pline("You struggle to stay on your %s.", makeplural(body_part(FOOT)));
					    } else {
						pline("You're knocked out and helplessly drop to the floor.");
						nomovemsg = 0;	/* default: "you can move again" */
						nomul(-rnd(5));
						exercise(A_DEX, FALSE);
						    }
						}
					}

					if (!rn2(3) && otmp->otyp == LACQUERED_DANCING_SHOE) {

					if (Role_if(PM_COURIER)) pline("The lacquered dancing shoe harmlessly scratches you.");
					else {pline("The lacquered dancing shoe scratches your %s!", body_part(HEAD));

						if (!uarmh || uarmh->otyp != DUNCE_CAP) {

					    /* No such thing as mindless players... */
					    if (ABASE(A_INT) <= ATTRMIN(A_INT)) {
						int lifesaved = 0;
						struct obj *wore_amulet = uamul;
			
						while(1) {
						    /* avoid looping on "die(y/n)?" */
						    if (lifesaved && (discover || wizard)) {
							if (wore_amulet && !uamul) {
							    /* used up AMULET_OF_LIFE_SAVING; still
							       subject to dying from brainlessness */
							    wore_amulet = 0;
							} else {
							    /* explicitly chose not to die;
							       arbitrarily boost intelligence */
							    ABASE(A_INT) = ATTRMIN(A_INT) + 2;
							    You_feel("like a scarecrow.");
							    break;
							}
						    }

						    if (lifesaved)
							pline("Unfortunately your brain is still gone.");
						    else
							Your("last thought fades away.");
						    killer = "brainlessness";
						    killer_format = KILLED_BY;
						    done(DIED);
						    lifesaved++;
						}
					    }
					}
					/* adjattrib gives dunce cap message when appropriate */
					if (!rn2(10)) (void) adjattrib(A_INT, -rnd(2), FALSE);
					else if (!rn2(2)) (void) adjattrib(A_INT, -1, FALSE);
					forget_levels(5);	/* lose memory of 25% of levels */
					forget_objects(5);	/* lose memory of 25% of objects */
					exercise(A_WIS, FALSE);

						}
					}

					if (!rn2(3) && otmp->otyp == HIGH_HEELED_SANDAL) {
					pline("Your %s is hit painfully by the high heel!", body_part(HEAD));
					losehp(rnd(12),"a high-heeled sandal",KILLED_BY);
					}
					if (!rn2(3) && otmp->otyp == SEXY_LEATHER_PUMP) {
					pline("Klock! The heel slams on your %s, producing a beautiful sound.", body_part(HEAD));
					losehp(rnd(20),"a sexy leather pump",KILLED_BY);
					}
					if (!rn2(3) && otmp->otyp == SPIKED_BATTLE_BOOT) {
					pline("Ouch! The spiked boot soles bore themselves into your skin!");
					losehp(rnd(10),"a spiked battle boot",KILLED_BY);
				    if (!rn2(6))
					poisoned("spikes", A_STR, "poisoned boot spike", 8);
					}

/* The gnome swings her spiked battle boot! Ouch! The spiked boot soles bore themselves into your skin! The spikes were poisoned! The poison was deadly... Do you want your possessions identified? */

				}
				if(tmp > (j = dieroll = rnd(20+i)))
				    sum[i] = hitmu(mtmp, mattk);
					if (!rn2(75)) pushplayer();
				else
				    missmu(mtmp, tmp , j, mattk);
				/* KMH -- Don't accumulate to-hit bonuses */
				if (otmp)
					tmp -= hittmp;
			     } else wildmiss(mtmp, mattk);
			}
			break;
		case AT_MAGC:
			if (!rn2(4)) /* yeah they need to be toned down a lot */{

			if (range2) {
			    if (!blue_on_blue(mtmp))
				sum[i] = buzzmu(mtmp, mattk);
			} else {
			    if (foundyou)
				sum[i] = castmu(mtmp, mattk, TRUE, TRUE);
			    else
				sum[i] = castmu(mtmp, mattk, TRUE, FALSE);
			}

			}
			break;

		default:		/* no attack */
			break;
	    }
	    if(flags.botl) bot();
	/* give player a chance of waking up before dying -kaa */
	    if(sum[i] == 1) {	    /* successful attack */
		if (u.usleep && u.usleep < monstermoves && !rn2(10)) {
		    multi = -1;
		    nomovemsg = "The combat suddenly awakens you.";
		}
	    }
	    if(sum[i] == 2) return 1;		/* attacker dead */
	    if(sum[i] == 3) break;  /* attacker teleported, no more attacks */
	    /* sum[i] == 0: unsuccessful attack */
	}
	return(0);
}

#endif /* OVL0 */
#ifdef OVLB

/*
 * helper function for some compilers that have trouble with hitmu
 */

STATIC_OVL void
hurtarmor(attk)
int attk;
{
	int	hurt;

	switch(attk) {
	    /* 0 is burning, which we should never be called with */
	    case AD_RUST: hurt = 1; break;
	    case AD_CORR: hurt = 3; break;
	    default: hurt = 2; break;
	}

	/* What the following code does: it keeps looping until it
	 * finds a target for the rust monster.
	 * Head, feet, etc... not covered by metal, or covered by
	 * rusty metal, are not targets.  However, your body always
	 * is, no matter what covers it.
	 *
	 * WAC fixed code so that it keeps looping until it either hits
	 * your body or finds a rustable item
	 * changed the last parm of !rust_dmg for non-body targets to FALSE
	 */
	while (1) {
	    switch(rn2(5)) {
	    case 0:
		if (!uarmh || !rust_dmg(uarmh, xname(uarmh), hurt, FALSE, &youmonst))
			continue;
		break;
	    case 1:
		if (uarmc) {
		    (void)rust_dmg(uarmc, xname(uarmc), hurt, TRUE, &youmonst);
		    break;
		}
		/* Note the difference between break and continue;
		 * break means it was hit and didn't rust; continue
		 * means it wasn't a target and though it didn't rust
		 * something else did.
		 */
		if (uarm)
		    (void)rust_dmg(uarm, xname(uarm), hurt, TRUE, &youmonst);
#ifdef TOURIST
		else if (uarmu)
		    (void)rust_dmg(uarmu, xname(uarmu), hurt, TRUE, &youmonst);
#endif
		break;
	    case 2:
		if (!uarms || !rust_dmg(uarms, xname(uarms), hurt, FALSE, &youmonst))
		    continue;
		break;
	    case 3:
		if (!uarmg || !rust_dmg(uarmg, xname(uarmg), hurt, FALSE, &youmonst))
		    continue;
		break;
	    case 4:
		if (!uarmf || !rust_dmg(uarmf, xname(uarmf), hurt, FALSE, &youmonst))
		    continue;
		break;
	    }
	    
	    break; /* Out of while loop */
	}
}

STATIC_OVL void
witherarmor()
{

	/* What the following code does: it keeps looping until it
	 * finds a target for the rust monster.
	 * Head, feet, etc... not covered by metal, or covered by
	 * rusty metal, are not targets.  However, your body always
	 * is, no matter what covers it.
	 *
	 * WAC fixed code so that it keeps looping until it either hits
	 * your body or finds a rustable item
	 * changed the last parm of !rust_dmg for non-body targets to FALSE
	 */
	while (1) {
	    switch(rn2(5)) {
	    case 0:
		if (!uarmh || !wither_dmg(uarmh, xname(uarmh), rn2(4), FALSE, &youmonst))
			continue;
		break;
	    case 1:
		if (uarmc) {
		    (void)wither_dmg(uarmc, xname(uarmc), rn2(4), TRUE, &youmonst);
		    break;
		}
		/* Note the difference between break and continue;
		 * break means it was hit and didn't rust; continue
		 * means it wasn't a target and though it didn't rust
		 * something else did.
		 */
		if (uarm)
		    (void)wither_dmg(uarm, xname(uarm), rn2(4), TRUE, &youmonst);
#ifdef TOURIST
		else if (uarmu)
		    (void)wither_dmg(uarmu, xname(uarmu), rn2(4), TRUE, &youmonst);
#endif
		break;
	    case 2:
		if (!uarms || !wither_dmg(uarms, xname(uarms), rn2(4), FALSE, &youmonst))
		    continue;
		break;
	    case 3:
		if (!uarmg || !wither_dmg(uarmg, xname(uarmg), rn2(4), FALSE, &youmonst))
		    continue;
		break;
	    case 4:
		if (!uarmf || !wither_dmg(uarmf, xname(uarmf), rn2(4), FALSE, &youmonst))
		    continue;
		break;
	    }
	    
	    break; /* Out of while loop */
	}
}

#endif /* OVLB */
#ifdef OVL1

STATIC_OVL boolean
diseasemu(mdat)
struct permonst *mdat;
{
	if (Sick_resistance || !rn2(10) ) { /* small chance to not get infected even if not resistant --Amy */
		You_feel("a slight illness.");
		return FALSE;
	} else {
		make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON), 40),
			mdat->mname, TRUE, SICK_NONVOMITABLE);
		return TRUE;
	}
}

STATIC_OVL boolean
digeasemu(mtmp)
struct permonst *mtmp;
{
	if (Sick_resistance || !rn2(10) ) {
		You_feel("a slight illness.");
		return FALSE;
	} else {
		make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON), 40),
			mtmp->mname, TRUE, SICK_NONVOMITABLE);
		return TRUE;
	}
}

/* check whether slippery clothing protects from hug or wrap attack */
STATIC_OVL boolean
u_slip_free(mtmp, mattk)
struct monst *mtmp;
struct attack *mattk;
{
	struct obj *obj = (uarmc ? uarmc : uarm);

#ifdef TOURIST
	if (!obj) obj = uarmu;
#endif
	if (mattk->adtyp == AD_DRIN) obj = uarmh;

	/* if your cloak/armor is greased, monster slips off; this
	   protection might fail (33% chance) when the armor is cursed */
	if (obj && (obj->greased || obj->otyp == OILSKIN_CLOAK) && rn2(50) && /* low chance to fail anyway --Amy */
		(!obj->cursed || rn2(3))) {
	    pline("%s %s your %s %s!",
		  Monnam(mtmp),
		  (mattk->adtyp == AD_WRAP) ?
			"slips off of" : "grabs you, but cannot hold onto",
		  obj->greased ? "greased" : "slippery",
		  /* avoid "slippery slippery cloak"
		     for undiscovered oilskin cloak */
		  (obj->greased || objects[obj->otyp].oc_name_known) ?
			xname(obj) : cloak_simple_name(obj));

	    if (obj->greased && !rn2(2)) {
		pline_The("grease wears off.");
		obj->greased = 0;
		update_inventory();
	    }
	    return TRUE;
	/* 50% chance (with a luck bonus) of slipping free with free action */
	} else if (Free_action && (rnl(10) < 5)) {
                pline("%s %s you, but you quickly free yourself!",
                       Monnam(mtmp),
                       (mattk->adtyp == AD_WRAP) ?
                      "swings itself around of" : "grabs");
                return TRUE;
	}
	return FALSE;
}

/* armor that sufficiently covers the body might be able to block magic */
int
magic_negation(mon)
struct monst *mon;
{
	struct obj *armor;
	int armpro = 0;

	armor = (mon == &youmonst) ? uarm : which_armor(mon, W_ARM);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
	armor = (mon == &youmonst) ? uarmc : which_armor(mon, W_ARMC);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
	armor = (mon == &youmonst) ? uarmh : which_armor(mon, W_ARMH);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;

	/* armor types for shirt, gloves, shoes, and shield don't currently
	   provide any magic cancellation but we might as well be complete */
#ifdef TOURIST
	armor = (mon == &youmonst) ? uarmu : which_armor(mon, W_ARMU);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
#endif
	armor = (mon == &youmonst) ? uarmg : which_armor(mon, W_ARMG);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
	armor = (mon == &youmonst) ? uarmf : which_armor(mon, W_ARMF);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
	armor = (mon == &youmonst) ? uarms : which_armor(mon, W_ARMS);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;

#ifdef STEED
	/* this one is really a stretch... */
	armor = (mon == &youmonst) ? 0 : which_armor(mon, W_SADDLE);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
#endif

	return armpro;
}

/*
 * hitmu: monster hits you
 *	  returns 2 if monster dies (e.g. "yellow light"), 1 otherwise
 *	  3 if the monster lives but teleported/paralyzed, so it can't keep
 *	       attacking you
 */
STATIC_OVL int
hitmu(mtmp, mattk)
	register struct monst *mtmp;
	register struct attack  *mattk;
{
	register struct permonst *mdat = mtmp->data;
	register int tmp = d((int)mattk->damn, (int)mattk->damd*10);
	register boolean not_affected = defends((int)mattk->adtyp, uwep);
	register int uncancelled, ptmp;
	register struct engr *ep = engr_at(u.ux,u.uy);
	int dmg, armpro, permdmg;
	char	 buf[BUFSZ];
	struct permonst *olduasmon = youmonst.data;
	int res;
	boolean burnmsg = FALSE;

	/*int randattack = 0;*/
	uchar atttyp;

	if (!canspotmon(mtmp))
	    map_invisible(mtmp->mx, mtmp->my);

/*	If the monster is undetected & hits you, you should know where
 *	the attack came from.
 */
	if(mtmp->mundetected && (hides_under(mdat) || mdat->mlet == S_EEL || mdat->mlet == S_FLYFISH)) {
	    mtmp->mundetected = 0;
	    if (!(Blind ? Blind_telepat : Unblind_telepat)) {
		struct obj *obj;
		const char *what;

		if ((obj = level.objects[mtmp->mx][mtmp->my]) != 0) {
		    if (Blind && !obj->dknown)
			what = something;
		    else if (is_pool(mtmp->mx, mtmp->my) && !Underwater)
			what = "the water";
		    else
			what = doname(obj);

		    pline("%s was hidden under %s!", Amonnam(mtmp), what);
		}
		newsym(mtmp->mx, mtmp->my);
	    }
	}

/*	First determine the base damage done */
	dmg = d((int)mattk->damn, (int)mattk->damd);
	if(is_undead(mdat) && midnight())
		dmg += d((int)mattk->damn, (int)mattk->damd); /* extra damage */
/*	Next a cancellation factor	*/

/*	Use uncancelled when the cancellation factor takes into account certain
 *	armor's special magic protection.  Otherwise just use !mtmp->mcan.
 */
	armpro = magic_negation(&youmonst);
	uncancelled = !mtmp->mcan && ((rn2(3) >= armpro) || !rn2(20)); /* mc3 no longer protects that much --Amy */

	permdmg = 0;
/*	Now, adjust damages via resistances or specific attacks */

	/* Monsters with AD_RBRE will choose a random attack instead. --Amy */

	atttyp = mattk->adtyp;

	if (atttyp == AD_RBRE) {
		while (atttyp == AD_ENDS ||atttyp == AD_RBRE || atttyp == AD_SPC2 || atttyp == AD_WERE) {
			atttyp = randattack(); }
		/*randattack = 1;*/
	}

	switch(atttyp) {
	    case AD_PHYS:
		if (mattk->aatyp == AT_HUGS && !sticks(youmonst.data)) {
		    if(!u.ustuck && rn2(2)) {
			if (u_slip_free(mtmp, mattk)) {
			    dmg = 0;
			} else {
			    setustuck(mtmp);
			    pline("%s grabs you!", Monnam(mtmp));
			}
		    } else if(u.ustuck == mtmp) {
			exercise(A_STR, FALSE);
			if (mtmp->data == &mons[PM_ROPE_GOLEM] && Breathless) {
			    You("are being strangled.");
			    dmg = (dmg+1) / 2;
			} else
			    You("are being %s.",
				    (mtmp->data == &mons[PM_ROPE_GOLEM])
				    ? "choked" : "crushed");
		    }
		} else {			  /* hand to hand weapon */
		    if(mattk->aatyp == AT_WEAP && otmp) {
			int nopoison = (10/* - (otmp->owt/10)*/);
			if (otmp->otyp == CORPSE &&
				touch_petrifies(&mons[otmp->corpsenm])) {
			    dmg = 1;
			    pline("%s hits you with the %s corpse.",
				Monnam(mtmp), mons[otmp->corpsenm].mname);
			    if (!Stoned) goto do_stone;
			}

			/* MRKR: If hit with a burning torch,     */
			/*       then do an extra point of damage */
			/*       but save the message till after  */
			/*       the hitmsg()                     */

			if (otmp->otyp == TORCH && otmp->lamplit &&
			    !Fire_resistance) {
			  burnmsg = TRUE;
			  dmg++;
			}

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
			    if (u.umonnum == PM_SHADE && otmp->otyp != SILVER_ARROW)
				dmg = 0;
			    else
				dmg += rnd(2); /* don't lose the base damage from monst.txt --Amy */

#if 0 /* Monsters don't wield boomerangs */
			    if (otmp->otyp == BOOMERANG /* && !rnl(3) */) {
				pline("As %s hits you, %s breaks into splinters.",
				      mon_nam(mtmp), the(xname(otmp)));
				useup(otmp);
				otmp = (struct obj *) 0;
				possibly_unwield(mtmp);
				if (u.umonnum != PM_SHADE)
				    dmg++;
			    }
#endif
			} else dmg += dmgval(otmp, &youmonst);

			if (objects[otmp->otyp].oc_material == SILVER &&
				hates_silver(youmonst.data)) {
			    pline("The silver sears your flesh!");
			}
			/* Stakes do extra dmg agains vamps */
			if ((otmp->otyp == WOODEN_STAKE || otmp->oartifact == ART_VAMPIRE_KILLER) &&
				is_vampire(youmonst.data)) {
			    if (otmp->oartifact == ART_STAKE_OF_VAN_HELSING) {
				if (!rn2(10)) {
				    pline("%s plunges the stake into your heart.",
					    Monnam(mtmp));
				    killer = "a wooden stake in the heart.";
				    killer_format = KILLED_BY_AN;
				    u.ugrave_arise = NON_PM; /* No corpse */
				    done(DIED);
				} else {
				    pline("%s drives the stake into you.",
					    Monnam(mtmp));
				    dmg += rnd(6) + 2;
				}
			    }else if (otmp->oartifact == ART_VAMPIRE_KILLER) {
				pline("%s whips you good!",
					Monnam(mtmp));
				dmg += rnd(6);
			    } else {
				pline("%s drives the stake into you.",
					Monnam(mtmp));
				dmg += rnd(6);
			    }
			}

			if (otmp->opoisoned) {
			    poisoned(simple_typename(otmp->otyp), A_STR,
				    killer_xname(otmp), 10);
			    if (nopoison < 2) nopoison = 2;
			    if (!rn2(nopoison)) {
				otmp->opoisoned = FALSE;
				pline("%s %s no longer poisoned.",
				       s_suffix(Monnam(mtmp)),
				       aobjnam(otmp, "are"));
			    }
			}
			if (dmg <= 0) dmg = 1;
			if (!otmp->oartifact || !artifact_hit(mtmp, &youmonst,
				otmp, &dmg, dieroll))
			     hitmsg(mtmp, mattk);

			if (burnmsg) {
			  boolean plural = (Blind ? FALSE : otmp->quan > 1L);
			  boolean water = (youmonst.data ==
					   &mons[PM_WATER_ELEMENTAL]);

			  pline("%s %s%s %syou!",
				(Blind ? "It" : Yname2(otmp)),
				(water ? "vaporize" : "burn"),
				(plural ? "" : "s"),
				(water ? "part of " : ""));

			  if (!rn2(2) && burnarmor(&youmonst)) {
			    dmg++;

			    /* Torch flame is not hot enough to guarantee */
			    /* burning away slime */

			    if (!rn2(4)) burn_away_slime();
			    if (!rn2(33))
			      (void)destroy_item(POTION_CLASS, AD_FIRE);
			    if (!rn2(33))
			      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
			    if (!rn2(50))
			      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);
			  }
			  burn_faster(otmp, 1);
			}

			if (!dmg) break;
			if (u.mh > 1 && u.mh > ((u.uac>0) ? dmg : dmg+u.uac) &&
				   objects[otmp->otyp].oc_material == IRON &&
					(u.umonnum==PM_BLACK_PUDDING
					|| u.umonnum==PM_BROWN_PUDDING || u.umonnum==PM_BLACK_PIERCER)) {
			    /* This redundancy necessary because you have to
			     * take the damage _before_ being cloned.
			     */
			    if (u.uac < 0) dmg += u.uac;
			    if (dmg < 1) dmg = 1;
			    if (dmg > 1) exercise(A_STR, FALSE);
			    u.mh -= dmg;
			    flags.botl = 1;
			    dmg = 0;
			    if (!rn2(50)) { if(cloneu()) /* greatly reduce ability to farm puddings --Amy */
			    You("divide as %s hits you!",mon_nam(mtmp));
				}
			}
			urustm(mtmp, otmp);
		    } else if (mattk->aatyp != AT_TUCH || dmg != 0 ||
			    mtmp != u.ustuck)
			hitmsg(mtmp, mattk);
		}
		break;
	    case AD_DISE:
		hitmsg(mtmp, mattk);
		if (rn2(3)) break;
                if (!diseasemu(mdat) || Invulnerable) dmg = 0;
		break;
	    case AD_NGRA:
		hitmsg(mtmp, mattk);

		      if (ep && sengr_at("Elbereth", u.ux, u.uy) ) {
		/* This attack can remove any Elbereth engraving, even burned ones. --Amy */
			pline("The engraving underneath you vanishes!");
		    del_engr(ep);
		    ep = (struct engr *)0;
		}

		break;
	    case AD_GLIB:
		hitmsg(mtmp, mattk);

		/* hurt the player's hands --Amy */
		pline("Your hands got hit hard!");
		incr_itimeout(&Glib, dmg);

		break;
	    case AD_DARK:
		hitmsg(mtmp, mattk);

		/* create darkness around the player --Amy */
		pline("That felt evil and sinister!");
			litroomlite(FALSE);
		break;
	    case AD_FIRE:
		hitmsg(mtmp, mattk);
		if (uncancelled) {
		    pline("You're %s!", on_fire(youmonst.data, mattk));
		    if (youmonst.data == &mons[PM_STRAW_GOLEM] ||
		        youmonst.data == &mons[PM_PAPER_GOLEM]) {
			    You("roast!");
			    /* KMH -- this is okay with unchanging */
			    rehumanize();
			    break;
		    } else if (Fire_resistance && rn2(20)) {
			pline_The("fire doesn't feel hot!");
			dmg = 0;
                        } else if (u.umonnum == PM_STRAW_GOLEM ||
				   u.umonnum == PM_PAPER_GOLEM ||
				   u.umonnum == PM_WAX_GOLEM) {
				/* This code ASSUMES that you are polymorphed 
				 * Code will need to be changed if we ever implement
				 * Golems as a class.
				 */
				You("burn up!");
				u.uhp -= mons[u.umonnum].mlevel;
				u.uhpmax -= mons[u.umonnum].mlevel;
				if (u.uhpmax < 1) u.uhpmax = 1;
				/* KMH, balance patch -- this is okay with unchanging */
				u.mh = 0; /* Kill monster form */
				rehumanize();
				break;
		    }
/*		    if((int) mtmp->m_lev > rn2(20))
			destroy_item(SCROLL_CLASS, AD_FIRE);
		    if((int) mtmp->m_lev > rn2(20))
			destroy_item(POTION_CLASS, AD_FIRE);
		    if((int) mtmp->m_lev > rn2(25))
			destroy_item(SPBOOK_CLASS, AD_FIRE);*/
		    if (!rn2(33)) /* new calculations --Amy */
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		    if (!rn2(33))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (!rn2(50))
		      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);
		    burn_away_slime();
		} /*else dmg = 0;*/
		break;
	    case AD_COLD:
		hitmsg(mtmp, mattk);
		if (uncancelled) {
		    pline("You're covered in frost!");
		    if (Cold_resistance && rn2(20)) {
			pline_The("frost doesn't seem cold!");
			dmg = 0;
		    }
		    /*if((int) mtmp->m_lev > rn2(20))*/
		    if (!rn2(33)) /* new calculations --Amy */
			destroy_item(POTION_CLASS, AD_COLD);
		} /*else dmg = 0;*/
		break;
	    case AD_ELEC:
		hitmsg(mtmp, mattk);
		if (uncancelled) {
		    You("get zapped!");
		    if (Shock_resistance && rn2(20)) {
			pline_The("zap doesn't shock you!");
			dmg = 0;
		    }
		    /*if((int) mtmp->m_lev > rn2(20))*/
		    if (!rn2(33)) /* new calculations --Amy */
			destroy_item(WAND_CLASS, AD_ELEC);
		    /*if((int) mtmp->m_lev > rn2(20))*/
		    if (!rn2(33)) /* new calculations --Amy */
			destroy_item(RING_CLASS, AD_ELEC);
		} /*else dmg = 0;*/
		break;
	    case AD_SLEE:
		hitmsg(mtmp, mattk);
		if (uncancelled && multi >= 0 && !rn2(5)) {
		    if (Sleep_resistance && rn2(20)) break;
		    fall_asleep(-rnd(10), TRUE);
		    if (Blind) You("are put to sleep!");
		    else You("are put to sleep by %s!", mon_nam(mtmp));
		}
		break;
	    case AD_BLND:
		hitmsg(mtmp, mattk);
		if (can_blnd(mtmp, &youmonst, mattk->aatyp, (struct obj*)0) && !rn2(3) ) {
		    if (!Blind) pline("%s blinds you!", Monnam(mtmp));
		    make_blinded(Blinded+(long)dmg,FALSE);
		    if (!Blind) Your(vision_clears);
		}
		dmg = 0;
		break;
	    case AD_DRST:
		ptmp = A_STR;
		goto dopois;
	    case AD_DRDX:
		ptmp = A_DEX;
		goto dopois;
	    case AD_DRCO:
		ptmp = A_CON;
dopois:
		hitmsg(mtmp, mattk);
		if (uncancelled && !rn2(8)) {
		    Sprintf(buf, "%s %s",
			    s_suffix(Monnam(mtmp)), mpoisons_subj(mtmp, mattk));
		    poisoned(buf, ptmp, mdat->mname, 30);
		}
		break;
	    case AD_DRIN:
		hitmsg(mtmp, mattk);
		if (defends(AD_DRIN, uwep) || !has_head(youmonst.data) || Role_if(PM_COURIER)) {
		    You("don't seem harmed.");
		    /* Not clear what to do for green slimes */
		    break;
		}
		if (u_slip_free(mtmp,mattk)) break;

		if (uarmh && rn2(8)) {
		    /* not body_part(HEAD) */
		    Your("helmet blocks the attack to your head.");
		    break;
		}
		
                /* conflicted dog, perhaps? */
		if (mtmp->mtame && !mtmp->isminion) {
		    EDOG(mtmp)->hungrytime += rnd(60);
		    mtmp->mconf = 0;
		}

		if (Half_physical_damage && rn2(2) ) dmg = (dmg+1) / 2;
		mdamageu(mtmp, dmg);

		if (!uarmh || uarmh->otyp != DUNCE_CAP) {
		    Your("brain is eaten!");
		    /* No such thing as mindless players... */
		    if (ABASE(A_INT) <= ATTRMIN(A_INT)) {
			int lifesaved = 0;
			struct obj *wore_amulet = uamul;

			while(1) {
			    /* avoid looping on "die(y/n)?" */
			    if (lifesaved && (discover || wizard)) {
				if (wore_amulet && !uamul) {
				    /* used up AMULET_OF_LIFE_SAVING; still
				       subject to dying from brainlessness */
				    wore_amulet = 0;
				} else {
				    /* explicitly chose not to die;
				       arbitrarily boost intelligence */
				    ABASE(A_INT) = ATTRMIN(A_INT) + 2;
				    You_feel("like a scarecrow.");
				    break;
				}
			    }

			    if (lifesaved)
				pline("Unfortunately your brain is still gone.");
			    else
				Your("last thought fades away.");
			    killer = "brainlessness";
			    killer_format = KILLED_BY;
			    done(DIED);
			    lifesaved++;
			}
		    }
		}
		/* adjattrib gives dunce cap message when appropriate */
		if (!rn2(10)) (void) adjattrib(A_INT, -rnd(2), FALSE);
		else if (!rn2(2)) (void) adjattrib(A_INT, -1, FALSE);
		forget_levels(5);	/* lose memory of 25% of levels */
		forget_objects(5);	/* lose memory of 25% of objects */
		exercise(A_WIS, FALSE);
		break;
	    case AD_PLYS:
		hitmsg(mtmp, mattk);
		if (uncancelled && multi >= 0 && !rn2(3)) {
		    if (Free_action) {
			You("momentarily stiffen.");            
		    } else {
			if (Blind) You("are frozen!");
			else You("are frozen by %s!", mon_nam(mtmp));
			nomovemsg = 0;	/* default: "you can move again" */
			nomul(-rnd(10));
			exercise(A_DEX, FALSE);
		    }
		}
		break;
	    case AD_TCKL:
		hitmsg(mtmp, mattk);
		if (uncancelled && multi >= 0 && !rn2(3)) {
		    if (Free_action)
			You_feel("horrible tentacles probing your flesh!");
		    else {
			if (Blind) You("are mercilessly tickled!");
			else You("are mercilessly tickled by %s!", mon_nam(mtmp));
			nomovemsg = 0;	/* default: "you can move again" */
			nomul(-rnd(10));
			exercise(A_DEX, FALSE);
			exercise(A_CON, FALSE);
		    }
		}
		break;
	    case AD_DRLI:
		hitmsg(mtmp, mattk);
		/* if vampire biting (and also a pet) */
		if (is_vampire(mtmp->data) && mattk->aatyp == AT_BITE &&
			has_blood(youmonst.data)) {
			   Your("blood is being drained!");
			   /* Get 1/20th of full corpse value
			    * Therefore 4 bites == 1 drink
			    */
			    if (mtmp->mtame && !mtmp->isminion)
			    	EDOG(mtmp)->hungrytime += ((int)((youmonst.data)->cnutrit / 20) + 1);
		}
		
		if (uncancelled && !rn2(3) && (!Drain_resistance || !rn2(20) )  ) {
		    losexp("life drainage", FALSE);
		}
		break;
	    case AD_LEGS:
		{ register long side = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
		  const char *sidestr = (side == RIGHT_SIDE) ? "right" : "left";

		/* This case is too obvious to ignore, but Nethack is not in
		 * general very good at considering height--most short monsters
		 * still _can_ attack you when you're flying or mounted.
		 * [FIXME: why can't a flying attacker overcome this?]
		 */
		  if (
#ifdef STEED
			u.usteed ||
#endif
				    Levitation || Flying) {
		    pline("%s tries to reach your %s %s!", Monnam(mtmp),
			  sidestr, body_part(LEG));
		    dmg = 0;
		  } else if (mtmp->mcan) {
		    pline("%s nuzzles against your %s %s!", Monnam(mtmp),
			  sidestr, body_part(LEG));
		    dmg = 0;
		  } else {
		    if (uarmf) {
			if (rn2(2) && (uarmf->otyp == LOW_BOOTS ||
					     uarmf->otyp == IRON_SHOES))
			    pline("%s scratches the exposed part of your %s %s!",
				Monnam(mtmp), sidestr, body_part(LEG));
			else if (!rn2(5))
			    pline("%s scratches through your %s boot!",
				Monnam(mtmp), sidestr);
			else {
			    pline("%s scratches your %s boot!", Monnam(mtmp),
				sidestr);
			    dmg = 0;
			    break;
			}
		    } else pline("%s scratches your %s %s!", Monnam(mtmp),
			  sidestr, body_part(LEG));
		    set_wounded_legs(side, rnd(60-ACURR(A_DEX)));
		    exercise(A_STR, FALSE);
		    exercise(A_DEX, FALSE);
		  }
		  break;
		}
	    case AD_STON:	/* cockatrice */
		hitmsg(mtmp, mattk);
		if(!rn2(3)) {
		    if (mtmp->mcan) {
			if (flags.soundok)
			    You_hear("a cough from %s!", mon_nam(mtmp));
		    } else {
			if (flags.soundok)
			    You_hear("%s hissing!", s_suffix(mon_nam(mtmp)));
			if(!rn2(10) ||
			    (flags.moonphase == NEW_MOON && !have_lizard())) {
 do_stone:
			    if (!Stoned && !Stone_resistance
				    && !(poly_when_stoned(youmonst.data) &&
					polymon(PM_STONE_GOLEM))) {
				Stoned = 7;
				delayed_killer = mtmp->data->mname;
				if (mtmp->data->geno & G_UNIQ) {
				    if (!type_is_pname(mtmp->data)) {
					static char kbuf[BUFSZ];

					/* "the" buffer may be reallocated */
					Strcpy(kbuf, the(delayed_killer));
					delayed_killer = kbuf;
				    }
				    killer_format = KILLED_BY;
				} else killer_format = KILLED_BY_AN;
				return(1);
				/* You("turn to stone..."); */
				/* done_in_by(mtmp); */
			    }
			}
		    }
		}
		break;
	    case AD_STCK:
		hitmsg(mtmp, mattk);
		if (uncancelled && !u.ustuck && !sticks(youmonst.data))
			setustuck(mtmp);
		break;
	    case AD_WRAP:
		if ((!mtmp->mcan || u.ustuck == mtmp) && !sticks(youmonst.data)) {
		    if (!u.ustuck && !rn2(10)) {
			if (u_slip_free(mtmp, mattk)) {
			    dmg = 0;
			} else {
			    pline("%s swings itself around you!",
				  Monnam(mtmp));
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			    setustuck(mtmp);
			}
		    } else if(u.ustuck == mtmp) {

			if (is_pool(mtmp->mx,mtmp->my)) {

		/* Being drowned should ALWAYS involve your stuff getting wet. 
		 * Even if you're unbreathing, swimming or whatever. Your stuff isn't immune just because you are.  --Amy	*/

			    pline("%s pulls you into the water!", Monnam(mtmp));
				water_damage(invent, FALSE, FALSE);
				if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);
				if (Burned) make_burned(0L, TRUE);

			}

			if (is_lava(mtmp->mx,mtmp->my)) {

		/* drowning in lava  --Amy	*/
		/* This can only be done by monsters that can go on lava (flying couatls for example) and have wrap attacks */

			    pline("%s pulls you into the lava!", Monnam(mtmp));

				if (Slimed) {        
			      pline("The slime that covers you is burned away!");
			      Slimed = 0;
				}
			    if (Frozen) {
				pline("The ice thaws!");
				make_frozen(0L, FALSE);
			    }

			    destroy_item(SCROLL_CLASS, AD_FIRE);
			    destroy_item(SPBOOK_CLASS, AD_FIRE);
			    destroy_item(POTION_CLASS, AD_FIRE);
				burnarmor(&youmonst);

			}

			if (is_lava(mtmp->mx,mtmp->my) && !rn2(3)) {

				if (!Fire_resistance) {
					You("burn to ashes...");
					killer_format = KILLED_BY_AN;
					    Sprintf(buf, "pool of lava by %s", an(mtmp->data->mname));
					    killer = buf;
					done(BURNING);

				}
				else {
					display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
					You("scream "); verbalize("HAHAHAHAHAHAHAAAAAAAA!"); /* Super Mario 64 */
					u.uhpmax -= rnd(10);
					if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				    losehp(10, "lava dive", KILLED_BY_AN);
				}

			}

			if (is_pool(mtmp->mx,mtmp->my) && !Swimming
			    && !Amphibious && !rn2(3) ) { /* greatly lowered chance of getting drowned --Amy */
			    boolean moat =
				(levl[mtmp->mx][mtmp->my].typ != POOL) &&
				(levl[mtmp->mx][mtmp->my].typ != WATER) &&
				!Is_medusa_level(&u.uz) &&
				!Is_waterlevel(&u.uz);

			    pline("%s drowns you...", Monnam(mtmp));
			    killer_format = KILLED_BY_AN;
			    Sprintf(buf, "%s by %s",
				    moat ? "moat" : "pool of water",
				    an(mtmp->data->mname));
			    killer = buf;
			    done(DROWNING);
			} /*else*/ if(mattk->aatyp == AT_HUGS)
			    You("are being crushed.");
		    } else {
			dmg = 0;
			if(flags.verbose)
			    pline("%s brushes against your %s.", Monnam(mtmp),
				   body_part(LEG));
		    }
		} else dmg = 0;
		break;
	    case AD_WERE:
		hitmsg(mtmp, mattk);
		if (uncancelled && !rn2(4) && u.ulycn == NON_PM &&
			!Protection_from_shape_changers &&
			!is_were(youmonst.data) &&
			!defends(AD_WERE,uwep)) {
		    You_feel("feverish.");
		    exercise(A_CON, FALSE);
		    u.ulycn = monsndx(mdat);
			if (u.ulycn == PM_HUMAN_WERERAT) u.ulycn = PM_WERERAT;
			if (u.ulycn == PM_HUMAN_WEREJACKAL) u.ulycn = PM_WEREJACKAL;
			if (u.ulycn == PM_HUMAN_WEREWOLF) u.ulycn = PM_WEREWOLF;
			if (u.ulycn == PM_HUMAN_WEREPANTHER) u.ulycn = PM_WEREPANTHER;
			if (u.ulycn == PM_HUMAN_WERETIGER) u.ulycn = PM_WERETIGER;
			if (u.ulycn == PM_HUMAN_WERESNAKE) u.ulycn = PM_WERESNAKE;
			if (u.ulycn == PM_HUMAN_WERESPIDER) u.ulycn = PM_WERESPIDER;
			if (u.ulycn == PM_HUMAN_WERELICHEN) u.ulycn = PM_WERELICHEN;
			if (u.ulycn == PM_HUMAN_WEREPIERCER) u.ulycn = PM_WEREPIERCER;
			if (u.ulycn == PM_HUMAN_WEREPENETRATOR) u.ulycn = PM_WEREPENETRATOR;
			if (u.ulycn == PM_HUMAN_WERESMASHER) u.ulycn = PM_WERESMASHER;
			if (u.ulycn == PM_HUMAN_WERENYMPH) u.ulycn = PM_WERENYMPH;
			if (u.ulycn == PM_HUMAN_WEREMIMIC) u.ulycn = PM_WEREMIMIC;
			if (u.ulycn == PM_HUMAN_WERECOW) u.ulycn = PM_WERECOW;
			if (u.ulycn == PM_HUMAN_WEREBEAR) u.ulycn = PM_WEREBEAR;
			if (u.ulycn == PM_HUMAN_WEREVORTEX) u.ulycn = PM_WEREVORTEX;
			if (u.ulycn == PM_HUMAN_WEREGIANT) u.ulycn = PM_WEREGIANT;
			if (u.ulycn == PM_HUMAN_WEREGHOST) u.ulycn = PM_WEREGHOST;
			if (u.ulycn == PM_HUMAN_WEREMINDFLAYER) u.ulycn = PM_WEREMINDFLAYER;
			if (u.ulycn == PM_HUMAN_WERECOCKATRICE) u.ulycn = PM_WERECOCKATRICE;
			if (u.ulycn == PM_HUMAN_WERESOLDIERANT) u.ulycn = PM_WERESOLDIERANT;
			if (u.ulycn == PM_HUMAN_WEREPIRANHA) u.ulycn = PM_WEREPIRANHA;
			if (u.ulycn == PM_HUMAN_WEREEEL) u.ulycn = PM_WEREEEL;
			if (u.ulycn == PM_HUMAN_WEREFLYFISH) u.ulycn = PM_WEREFLYFISH;
			if (u.ulycn == PM_HUMAN_WEREKRAKEN) u.ulycn = PM_WEREKRAKEN;
		    upermonst.mflags2 |= (M2_WERE);
		}
		break;
	    case AD_SGLD:
		hitmsg(mtmp, mattk);
		if (youmonst.data->mlet == mdat->mlet) break;
		if(mtmp->mcan) break;
		if (rn2(10)) {stealgold(mtmp);
		break;
		}
		/* fall through --Amy */

	    case AD_SITM:	/* for now these are the same */
	    case AD_SEDU:
		if (is_animal(mtmp->data)) {
			hitmsg(mtmp, mattk);
			if (mtmp->mcan) break;
			/* Continue below */
		} else if (rn2(5) && (dmgtype(youmonst.data, AD_SEDU)
#ifdef SEDUCE
			|| dmgtype(youmonst.data, AD_SSEX)
#endif
						) ) {
			pline("%s %s.", Monnam(mtmp), mtmp->minvent ?
		    "brags about the goods some dungeon explorer provided" :
		    "makes some remarks about how difficult theft is lately");
			if (!tele_restrict(mtmp)) (void) rloc(mtmp, FALSE);
			return 3;
		} else if (mtmp->mcan) {
		    if (!Blind) {
			/*
			 * We use flags.female here on the basis that the
			 * monster chooses whether to charm or to seduce
			 * based on your visible gender.  --ALI
			 */
			int do_charm = is_neuter(mdat) || \
			  flags.female == mtmp->female;
			pline("%s tries to %s you, but you seem %s.",
			    Adjmonnam(mtmp, "plain"),
			    do_charm ? "charm" : "seduce",
			    do_charm ? "unaffected" : "uninterested");
		    }
		    if(rn2(3)) {
			if (!tele_restrict(mtmp)) (void) rloc(mtmp, FALSE);
			return 3;
		    }
		    break;
		}

		if (!rn2(25)) { /* Nymphs want to have some fun! :-) --Amy */

			if(could_seduceX(mtmp, &youmonst, mattk) == 1 && !mtmp->mcan)
			    if (doseduce(mtmp)) return 3;
		/* What kind of male person wouldn't want to be seduced by such a wonderfully lovely, sweet lady? */

		}

		buf[0] = '\0';
		if (  (rnd(100) > ACURR(A_CHA)) && ((mtmp->female) && !flags.female && rn2(5) ) || ((!mtmp->female) && flags.female && rn2(3) ) || 
			((mtmp->female) && flags.female && rn2(2) ) || ((!mtmp->female) && !flags.female && rn2(2) ) )
/* male characters are more susceptible to nymphs --Amy */
			{ switch (steal(mtmp, buf)) {
		  case -1:
			return 2;
		  case 0:
			break;
		  default:
			if (!is_animal(mtmp->data) && !tele_restrict(mtmp))
			    (void) rloc(mtmp, FALSE);
			if (is_animal(mtmp->data) && *buf) {
			    if (canseemon(mtmp))
				pline("%s tries to %s away with %s.",
				      Monnam(mtmp),
				      locomotion(mtmp->data, "run"),
				      buf);
			}
			monflee(mtmp, 0, FALSE, FALSE);
			return 3;
			};
		}
		break;
#ifdef SEDUCE
	    case AD_SSEX:
		if(could_seduceX(mtmp, &youmonst, mattk) == 1
			&& !mtmp->mcan && rn2(2) ) /* 50% chance --Amy */
		    if (doseduce(mtmp))
			return 3;
		break;
#endif
	    case AD_SAMU:
		hitmsg(mtmp, mattk);
		/* when the Wiz hits, 1/20 steals the amulet */
		if (u.uhave.amulet ||
		     u.uhave.bell || u.uhave.book || u.uhave.menorah
		     || u.uhave.questart) /* carrying the Quest Artifact */
		    if (!rn2(20)) stealamulet(mtmp);
		break;

	    case AD_TLPT:
		hitmsg(mtmp, mattk);
		if (uncancelled) {
		    if(flags.verbose)
			Your("position suddenly seems very uncertain!");
		    teleX();
		}
		break;
	    case AD_DISP:
		hitmsg(mtmp, mattk);
		pushplayer();
		break;

	    case AD_RUST:
		hitmsg(mtmp, mattk);
		if (mtmp->mcan) break;
		if (u.umonnum == PM_IRON_GOLEM) {
			You("rust!");
			u.uhp -= mons[u.umonnum].mlevel;
			u.uhpmax -= mons[u.umonnum].mlevel;
			if (u.uhpmax < 1) u.uhpmax = 1;
			/* KMH, balance patch -- this is okay with unchanging */
			u.mh = 0;
			rehumanize();
			break;
		}
		if (rn2(3)) hurtarmor(AD_RUST);
		break;
	    case AD_CORR:
		hitmsg(mtmp, mattk);
		if (mtmp->mcan) break;
		if (rn2(3)) hurtarmor(AD_CORR);
		break;
	    case AD_WTHR:
		hitmsg(mtmp, mattk);
		if (mtmp->mcan) break;
		if (rn2(3)) witherarmor();
		break;
	    case AD_LUCK:
		hitmsg(mtmp, mattk);
		if (mtmp->mcan) break;
		if (!rn2(3)) {change_luck(-1);
				pline("You feel unlucky.");
				}
		break;
	    case AD_HALU:
		hitmsg(mtmp, mattk);
		not_affected |= Blind ||
			(u.umonnum == PM_BLACK_LIGHT ||
			 u.umonnum == PM_VIOLET_FUNGUS ||
			 dmgtype(youmonst.data, AD_STUN));
		if (!not_affected && rn2(2) ) {
		    boolean chg;
		    if (!Hallucination)
			You("are caught in a blast of kaleidoscopic light!");
		    chg = make_hallucinated(HHallucination + (long)tmp,FALSE,0L);
		    You("%s", chg ? "inhale a cloud of psychedelic drugs!" : "took another hit of the good stuff!");
		}
		break;
	    case AD_DISN:
		hitmsg(mtmp, mattk);

		if (!rn2(10))  {
		if (Disint_resistance && rn2(100)) {
		    You("are not disintegrated.");
		    break;
            } else if (Invulnerable) {
                pline("You are unharmed!");
                break;
		} else if (uarms) {
		    /* destroy shield; other possessions are safe */
		    (void) destroy_arm(uarms);
		    break;
		} else if (uarmc) {
		    /* destroy cloak; other possessions are safe */
		    (void) destroy_arm(uarmc);
		    break;
		} else if (uarm) {
		    /* destroy suit */
		    (void) destroy_arm(uarm);
		    break;
#ifdef TOURIST
		} else if (uarmu) {
		    /* destroy shirt */
		    (void) destroy_arm(uarmu);
		    break;
#endif
		}
	      else if (nonliving(youmonst.data) || is_demon(youmonst.data)) {
		You("seem unaffected.");
		break;
	    } else if (Antimagic && rn2(20)) {
		You("aren't affected.");
		break;
	    }
	    done(DIED);
	    return; /* lifesaved */

	}
	      break;
	    case AD_DCAY:
		hitmsg(mtmp, mattk);
		if (mtmp->mcan) break;
		if (u.umonnum == PM_WOOD_GOLEM ||
		    u.umonnum == PM_LEATHER_GOLEM) {
			You("rot!");
			u.uhp -= mons[u.umonnum].mlevel;
			u.uhpmax -= mons[u.umonnum].mlevel;
			if (u.uhpmax < 1) u.uhpmax = 1;
			u.mh = 0;
			/* KMH, balance patch -- this is okay with unchanging */
			rehumanize();
			break;
		}
		if (rn2(3)) hurtarmor(AD_DCAY);
		break;
	    case AD_HEAL:
		/* a cancelled nurse is just an ordinary monster */
		if (mtmp->mcan) {
		    hitmsg(mtmp, mattk);
		    break;
		}
		if(!uwep
#ifdef TOURIST
		   && !uarmu
#endif
		   && !uarm && !uarmh && !uarms && !uarmg && !uarmc && !uarmf) {
		    boolean goaway = FALSE;
		    pline("%s hits!  (I hope you don't mind.)", Monnam(mtmp));
		    if (Upolyd) {
			u.mh += rnd(7);
/* STEPHEN WHITE'S NEW CODE */                                            
			if (!rn2(7)) {
			    /* no upper limit necessary; effect is temporary */
			    u.mhmax++;
			    if (!rn2(13)) goaway = TRUE;
			}
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		    } else {
			u.uhp += rnd(7);
			if (!rn2(10)) {
			    /* hard upper limit via nurse care: 25 * ulevel */
			    if (u.uhpmax < 5 * u.ulevel + d(2 * u.ulevel, 10)) {
				u.uhpmax++;
			    }
			    if (!rn2(10)) goaway = TRUE;
			}
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		    }
		    if (!rn2(3)) exercise(A_STR, TRUE);
		    if (!rn2(3)) exercise(A_CON, TRUE);
		    if (Sick) make_sick(0L, (char *) 0, FALSE, SICK_ALL);
		    flags.botl = 1;
		    if (goaway) {
			mongone(mtmp);
			return 2;
		    } else if (!rn2(15)) {
			if (!tele_restrict(mtmp) || !rn2(5) ) (void) rloc(mtmp, FALSE); /* sometimes ignore noteleport --Amy */
			monflee(mtmp, d(3, 6), TRUE, FALSE);
			return 3;
		    }
		    dmg = 0;
		} else {
		    if (Role_if(PM_HEALER)) {
			if (flags.soundok && !(moves % 5))
		      verbalize("Doc, I can't help you unless you cooperate.");
			dmg = 0;
		    } else hitmsg(mtmp, mattk);
		}
		break;
	    case AD_CURS:
	    case AD_LITE:
		hitmsg(mtmp, mattk);
		/* if(!night() && mdat == &mons[PM_GREMLIN]) break; */

		/* Yeah I know, I just made gremlins and other AD_CURS using monsters a lot more dangerous.
		They're supposed to appear late in the game, adding a bit of risk to high-level-characters.
		I mean come on, early game is hell but late game is cake? Now you can lose your intrinsics at any time!
		If you lose poison resistance, try eating some corpses to get it back.
		If you lose sickness resistance, well, tough luck - it's not coming back. Ever. --Amy*/
		if((!mtmp->mcan && !rn2(10)) || (night() && !rn2(3)) ) {
		    if (flags.soundok) {
			if (Blind) You_hear("laughter.");
			else       pline("%s chuckles.", Monnam(mtmp));
		    }
		    if (u.umonnum == PM_CLAY_GOLEM) {
			pline("Some writing vanishes from your head!");
			u.uhp -= mons[u.umonnum].mlevel;
			u.uhpmax -= mons[u.umonnum].mlevel;
			if (u.uhpmax < 1) u.uhpmax = 1;
			/* KMH, balance patch -- this is okay with unchanging */
			u.mh = 0;
			rehumanize();
			break;
		    }
		    attrcurse();
		}
		break;
	    case AD_STUN:
		hitmsg(mtmp, mattk);
		if(!mtmp->mcan && !rn2(4)) {
		    make_stunned(HStun + dmg, TRUE);
		    dmg /= 2;
		}
		break;
	    case AD_NUMB:
		hitmsg(mtmp, mattk);
		if(!mtmp->mcan && !rn2(4)) {
		    make_numbed(HNumbed + dmg, TRUE);
		    dmg /= 2;
		}
		break;
	    case AD_FRZE:
		hitmsg(mtmp, mattk);
		if(!mtmp->mcan && !rn2(10)) {
		    make_frozen(HFrozen + dmg, TRUE);
		    dmg /= 2;
		}
		break;
	    case AD_BURN:
		hitmsg(mtmp, mattk);
		if(!mtmp->mcan && !rn2(5)) {
		    make_burned(HBurned + dmg, TRUE);
		    dmg /= 2;
		}
		break;
	    case AD_FEAR:
		hitmsg(mtmp, mattk);
		if(!mtmp->mcan && !rn2(3)) {
		    make_feared(HFeared + dmg, TRUE);
		    dmg /= 2;
		}
		break;
	    case AD_ACID:
		hitmsg(mtmp, mattk);
		if(!mtmp->mcan && !rn2(3)) {
		    if (Acid_resistance && rn2(20)) {
			pline("You're covered in acid, but it seems harmless.");
			dmg = 0;
		    } else {
			pline("You're covered in acid! It burns!");
			exercise(A_STR, FALSE);
		    }
			if(rn2(30)) erode_armor(&youmonst, TRUE);

		if (Stoned) fix_petrification();

		} /*else		dmg = 0;*/
		break;
	    case AD_SLOW:
		hitmsg(mtmp, mattk);
		if (uncancelled && HFast &&
					!defends(AD_SLOW, uwep) && !rn2(4))
		    u_slow_down();
		break;
	    case AD_DREN:
		hitmsg(mtmp, mattk);
		if (uncancelled && !rn2(4))
		    drain_en(dmg);
		dmg = 0;
		break;
	    case AD_CONF:
		hitmsg(mtmp, mattk);
		if(!mtmp->mcan && !rn2(4) && !mtmp->mspec_used) {
		    mtmp->mspec_used = mtmp->mspec_used + (dmg + rn2(6));
		    if(Confusion)
			 You("are getting even more confused.");
		    else You("are getting confused.");
		    make_confused(HConfusion + dmg, FALSE);
		}
		/*dmg = 0;*/
		break;
	    case AD_DETH:
		pline("%s reaches out with its deadly touch.", Monnam(mtmp));
		if (is_undead(youmonst.data)) {
		    /* Still does normal damage */
		    pline("Was that the touch of death?");
		    break;
		}
		switch (rn2(20)) {
		case 19: /* case 18: case 17: */
		    if (!Antimagic) {
			killer_format = KILLED_BY_AN;
			killer = "touch of death";
			done(DIED);
			dmg = 0;
			break;
		    } /* else FALLTHRU */
		default: /* case 16: ... case 5: */
		    You_feel("your life force draining away...");
		    permdmg = 1;	/* actual damage done below */
		    break;
		case 4: case 3: case 2: case 1: case 0:
		    if (Antimagic) shieldeff(u.ux, u.uy);
		    pline("Lucky for you, it didn't work!");
		    dmg = 0;
		    break;
		}
		break;
	    case AD_PEST:
		pline("%s reaches out, and you feel fever and chills.",
			Monnam(mtmp));
		(void) diseasemu(mdat); /* plus the normal damage */
		/* No damage if invulnerable; setting dmg zero prevents
		 * "You are unharmed!" after a sickness inducing attack */
		if (Invulnerable) dmg = 0;
		break;
	    case AD_FAMN:
		pline("%s reaches out, and your body shrivels.",
			Monnam(mtmp));
		exercise(A_CON, FALSE);
		if (!is_fainted() && rn2(10) ) morehungry(rnz(40));
		morehungry(dmg); /* This attack was way too weak. --Amy */
		/* plus the normal damage */
		break;
	    case AD_CALM:	/* KMH -- koala attack */
		hitmsg(mtmp, mattk);
		if (uncancelled)
		    docalm();
		break;
	    case AD_POLY:
		hitmsg(mtmp, mattk);
		if (uncancelled && !Unchanging && !Antimagic) {
		    if (flags.verbose)
			You("undergo a freakish metamorphosis!");
		    polyself(FALSE);
		}
		break;
	    case AD_MAGM:
		hitmsg(mtmp, mattk);
		    if(Antimagic && rn2(5)) {
			shieldeff(u.ux, u.uy);
			dmg = 0;
			pline("A hail of magic missiles narrowly misses you!");
		    } else {
			You("are hit by magic missiles appearing from thin air!");
	    }
	    break;
		break;
	    case AD_SLIM:    
		hitmsg(mtmp, mattk);
		if (!uncancelled) break;
		if (flaming(youmonst.data)) {
		    pline_The("slime burns away!");
		    dmg = 0;
		} else if (Unchanging ||
				youmonst.data == &mons[PM_GREEN_SLIME]) {
		    You("are unaffected.");
		    dmg = 0;
		} else if (!Slimed) {
		    You("don't feel very well.");
		    Slimed = 100L;
		    flags.botl = 1;
		    killer_format = KILLED_BY_AN;
		    delayed_killer = mtmp->data->mname;
		} else
		    pline("Yuck!");
		break;
	    case AD_ENCH:	/* KMH -- remove enchantment (disenchanter) */
		hitmsg(mtmp, mattk);
		/* uncancelled is sufficient enough; please
		   don't make this attack less frequent */
		if (uncancelled) {
		    struct obj *obj = some_armor(&youmonst);

		    if (drain_item(obj)) {
			Your("%s less effective.", aobjnam(obj, "seem"));
		    }
		}
		break;
	    default:	dmg = 0;
			break;
	}

	/*if (randattack == 1) {
		mattk->adtyp = AD_RBRE;
		randattack = 0;
	}*/

		/* weapon attacks should be done even if they don't use AD_PHYS --Amy */
		if(mattk->aatyp == AT_WEAP && otmp && atttyp > AD_PHYS) {
			int nopoison = (10/* - (otmp->owt/10)*/);
			if (otmp->otyp == CORPSE &&
				touch_petrifies(&mons[otmp->corpsenm])) {
			    dmg += 1;
			    pline("%s hits you with the %s corpse.",
				Monnam(mtmp), mons[otmp->corpsenm].mname);
			    if (!Stoned) goto do_stone;
			}

			/* MRKR: If hit with a burning torch,     */
			/*       then do an extra point of damage */
			/*       but save the message till after  */
			/*       the hitmsg()                     */

			if (otmp->otyp == TORCH && otmp->lamplit &&
			    !Fire_resistance) {
			  burnmsg = TRUE;
			  dmg++;
			}

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
				dmg += rnd(2); /* don't lose the base damage from monst.txt --Amy */

			} else dmg += dmgval(otmp, &youmonst);

			if (objects[otmp->otyp].oc_material == SILVER &&
				hates_silver(youmonst.data)) {
			    pline("The silver sears your flesh!");
			}
			/* Stakes do extra dmg agains vamps */
			if ((otmp->otyp == WOODEN_STAKE || otmp->oartifact == ART_VAMPIRE_KILLER) &&
				is_vampire(youmonst.data)) {
			    if (otmp->oartifact == ART_STAKE_OF_VAN_HELSING) {
				if (!rn2(10)) {
				    pline("%s plunges the stake into your heart.",
					    Monnam(mtmp));
				    killer = "a wooden stake in the heart.";
				    killer_format = KILLED_BY_AN;
				    u.ugrave_arise = NON_PM; /* No corpse */
				    done(DIED);
				} else {
				    pline("%s drives the stake into you.",
					    Monnam(mtmp));
				    dmg += rnd(6) + 2;
				}
			    }else if (otmp->oartifact == ART_VAMPIRE_KILLER) {
				pline("%s whips you good!",
					Monnam(mtmp));
				dmg += rnd(6);
			    } else {
				pline("%s drives the stake into you.",
					Monnam(mtmp));
				dmg += rnd(6);
			    }
			}

			if (otmp->opoisoned) {
			    poisoned(obj_typename(otmp->otyp), A_STR,
				    killer_xname(otmp), 10);
			    if (nopoison < 2) nopoison = 2;
			    if (!rn2(nopoison)) {
				otmp->opoisoned = FALSE;
				pline("%s %s no longer poisoned.",
				       s_suffix(Monnam(mtmp)),
				       aobjnam(otmp, "are"));
			    }
			}
			if (dmg <= 0) dmg = 1;
			if (!otmp->oartifact || !artifact_hit(mtmp, &youmonst,
				otmp, &dmg, dieroll))
			     hitmsg(mtmp, mattk);

			if (burnmsg) {
			  boolean plural = (Blind ? FALSE : otmp->quan > 1L);
			  boolean water = (youmonst.data ==
					   &mons[PM_WATER_ELEMENTAL]);

			  pline("%s %s%s %syou!",
				(Blind ? "It" : Yname2(otmp)),
				(water ? "vaporize" : "burn"),
				(plural ? "" : "s"),
				(water ? "part of " : ""));

			  if (!rn2(2) && burnarmor(&youmonst)) {
			    dmg++;

			    /* Torch flame is not hot enough to guarantee */
			    /* burning away slime */

			    if (!rn2(4)) burn_away_slime();
			    if (!rn2(33))
			      (void)destroy_item(POTION_CLASS, AD_FIRE);
			    if (!rn2(33))
			      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
			    if (!rn2(50))
			      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);
			  }
			  burn_faster(otmp, 1);
			}

			if (u.mh > 1 && u.mh > ((u.uac>0) ? dmg : dmg+u.uac) &&
				   objects[otmp->otyp].oc_material == IRON &&
					(u.umonnum==PM_BLACK_PUDDING
					|| u.umonnum==PM_BROWN_PUDDING || u.umonnum==PM_BLACK_PIERCER)) {
			    /* This redundancy necessary because you have to
			     * take the damage _before_ being cloned.
			     */
			    if (u.uac < 0) dmg += u.uac;
			    if (dmg < 1) dmg = 1;
			    if (dmg > 1) exercise(A_STR, FALSE);
			    u.mh -= dmg;
			    flags.botl = 1;
			    dmg = 0;
			    if (!rn2(50)) { if(cloneu()) /* greatly reduce ability to farm puddings --Amy */
			    You("divide as %s hits you!",mon_nam(mtmp));
				}
			}
		}


	if(u.uhp < 1) done_in_by(mtmp);

/*	Negative armor class reduces damage done instead of fully protecting
 *	against hits.
 */
	if (dmg && u.uac < /*-1*/0) {  /* damage reduction will start at -1 rather than -11 AC now --Amy */
		int tempval;
		tempval = rnd(-(u.uac)/5+1);
		if (tempval < 1)  tempval = 1;
		if (tempval > 20) tempval = 20; /* max limit increased --Amy */
		dmg -= tempval;
		if (dmg < 1) dmg = 1;
	}

	if(dmg) {
	    if ( (Half_physical_damage && rn2(2)) 
					/* Mitre of Holiness */
		|| (Role_if(PM_PRIEST) && uarmh && is_quest_artifact(uarmh) &&
		    (is_undead(mtmp->data) || is_demon(mtmp->data))))
		dmg = (dmg+1) / 2;

	    if (permdmg) {	/* Death's life force drain */
		int lowerlimit, *hpmax_p;
		/*
		 * Apply some of the damage to permanent hit points:
		 *	polymorphed	    100% against poly'd hpmax
		 *	hpmax > 25*lvl	    100% against normal hpmax
		 *	hpmax > 10*lvl	50..100%
		 *	hpmax >  5*lvl	25..75%
		 *	otherwise	 0..50%
		 * Never reduces hpmax below 1 hit point per level.
		 */
		permdmg = rn2(dmg / 2 + 1);
		if (Upolyd || u.uhpmax > 25 * u.ulevel) permdmg = dmg;
		else if (u.uhpmax > 10 * u.ulevel) permdmg += dmg / 2;
		else if (u.uhpmax > 5 * u.ulevel) permdmg += dmg / 4;

		if (Upolyd) {
		    hpmax_p = &u.mhmax;
		    /* [can't use youmonst.m_lev] */
		    lowerlimit = min((int)youmonst.data->mlevel, u.ulevel);
		} else {
		    hpmax_p = &u.uhpmax;
		    lowerlimit = u.ulevel;
		}
		if (*hpmax_p - permdmg > lowerlimit)
		    *hpmax_p -= permdmg;
		else if (*hpmax_p > lowerlimit)
		    *hpmax_p = lowerlimit;
		else	/* unlikely... */
		    ;	/* already at or below minimum threshold; do nothing */
		flags.botl = 1;

		if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
	    }

	    mdamageu(mtmp, dmg);
	}

	if (DEADMONSTER(mtmp))
	    res = 2;
	else if (dmg)
	    res = passiveum(olduasmon, mtmp, mattk);
	else
	    res = 1;
	stop_occupation();
	return res;
}

#endif /* OVL1 */
#ifdef OVLB

STATIC_OVL int
gulpmu(mtmp, mattk)	/* monster swallows you, or damage if u.uswallow */
	register struct monst *mtmp;
	register struct attack  *mattk;
{
	char	 buf[BUFSZ];
	struct trap *t = t_at(u.ux, u.uy);
	int	tmp = d((int)mattk->damn, (int)mattk->damd);
	int	tim_tmp;
	register struct obj *otmp2;
	int	i;
	/*int randattackA = 0;*/
	uchar atttypA;

	if (!u.uswallow) {	/* swallows you */
		if (youmonst.data->msize >= MZ_HUGE) return(0);
		if ((t && ((t->ttyp == PIT) || (t->ttyp == SPIKED_PIT))) &&
		    sobj_at(BOULDER, u.ux, u.uy))
			return(0);

		if (Punished) unplacebc();	/* ball&chain go away */
		remove_monster(mtmp->mx, mtmp->my);
		mtmp->mtrapped = 0;		/* no longer on old trap */
		place_monster(mtmp, u.ux, u.uy);
		newsym(mtmp->mx,mtmp->my);
#ifdef STEED
		if (is_animal(mtmp->data) && u.usteed) {
			char buf[BUFSZ];
			/* Too many quirks presently if hero and steed
			 * are swallowed. Pretend purple worms don't
			 * like horses for now :-)
			 */
			Strcpy(buf, mon_nam(u.usteed));
			pline ("%s lunges forward and plucks you off %s!",
				Monnam(mtmp), buf);
			dismount_steed(DISMOUNT_ENGULFED);
		} else
#endif
		pline("%s engulfs you!", Monnam(mtmp));
		stop_occupation();
		reset_occupations();	/* behave as if you had moved */

		if (u.utrap) {
			You("are released from the %s!",
				u.utraptype==TT_WEB ? "web" : "trap");
			u.utrap = 0;
		}

		i = number_leashed();
		if (i > 0) {
		    const char *s = (i > 1) ? "leashes" : "leash";
		    pline_The("%s %s loose.", s, vtense(s, "snap"));
		    unleash_all();
		}

		if (touch_petrifies(youmonst.data) && !resists_ston(mtmp) && !rn2(4)) {
			minstapetrify(mtmp, TRUE);
			if (mtmp->mhp > 0) return 0;
			else return 2;
		}

		display_nhwindow(WIN_MESSAGE, FALSE);
		vision_recalc(2);	/* hero can't see anything */
		u.uswallow = 1;
		setustuck(mtmp);
		/* u.uswldtim always set > 1 */
		tim_tmp = 25 - (int)mtmp->m_lev;
		if (tim_tmp > 0) tim_tmp = rnd(tim_tmp) / 2;
		else if (tim_tmp < 0) tim_tmp = -(rnd(-tim_tmp) / 2);
		tim_tmp += -u.uac + 10;
		u.uswldtim = (unsigned)((tim_tmp < 2) ? 2 : tim_tmp);
		swallowed(1);
		for (otmp2 = invent; otmp2; otmp2 = otmp2->nobj)
		    (void) snuff_lit(otmp2);
	}

	if (mtmp != u.ustuck) return(0);
	if (u.uswldtim > 0) u.uswldtim -= 1;

	/* Monsters with AD_RBRE have random engulfing attacks. --Amy */

	atttypA = mattk->adtyp;

	if (atttypA == AD_RBRE) {
		while (atttypA == AD_ENDS ||atttypA == AD_RBRE || atttypA == AD_SPC2 || atttypA == AD_WERE) {
			atttypA = randattack(); }
		/*randattack = 1;*/
	}

	switch(atttypA) {

		case AD_DGST:
		    if (Slow_digestion) {
			/* Messages are handled below */
			u.uswldtim = 0;
			tmp = 0;
		    } else if (u.uswldtim == 0) {
			pline("%s totally digests you!", Monnam(mtmp));
			tmp = u.uhp;
			if (Half_physical_damage) tmp *= 2; /* sorry */
		    } else {
			pline("%s%s digests you!", Monnam(mtmp),
			      (u.uswldtim == 2) ? " thoroughly" :
			      (u.uswldtim == 1) ? " utterly" : "");
			exercise(A_STR, FALSE);
		    }
		    break;
	      case AD_CURS:
	    case AD_LITE:
			pline("It curses you!");

			if(!rn2(10) || (night() && !rn2(3)) )  {
			    if (u.umonnum == PM_CLAY_GOLEM) {
				pline("Some writing vanishes from your head!");
				u.uhp -= mons[u.umonnum].mlevel;
				u.uhpmax -= mons[u.umonnum].mlevel;
				if (u.uhpmax < 1) u.uhpmax = 1;
				/* KMH, balance patch -- this is okay with unchanging */
				u.mh = 0;
				rehumanize();
				break;
			    }
				pline("You hear a chuckling laughter.");
			    attrcurse();
			}
			break;

		case AD_SLEE:
			pline("It slaps you!");
			if (!rn2(5) && multi >= 0) {
			    if (Sleep_resistance && rn2(20)) break;
			    fall_asleep(-rnd(10), TRUE);
			    You("suddenly fall asleep!");
			}
			break;
		case AD_DRST:
				You("feel your strength drain away!");
			if (!rn2(8)) {
			    poisoned("The attack", A_STR, "strength drain", 30);
			}
			break;
		case AD_DRDX:
				You("feel your muscles cramping!");
			if (!rn2(8)) {
			    poisoned("The attack", A_DEX, "dexterity drain", 30);
			}
			break;
		case AD_DRCO:
				You("feel a lack of force!");
			if (!rn2(8)) {
			    poisoned("The attack", A_CON, "constitution drain", 30);
			}
			break;
	      case AD_STUN:
			{
				You("seem less steady!");
			    make_stunned(HStun + tmp, TRUE);
			}
			break;
	      case AD_NUMB:
			{
				You("feel your body parts getting numb!");
			    make_numbed(HNumbed + tmp, TRUE);
			}
			break;
	      case AD_FRZE:
				You("feel ice cold!");
			if (!rn2(3)) {
			    make_frozen(HFrozen + tmp, TRUE);
			}
			break;
	      case AD_BURN:
				You("feel an overwhelming heat!");
			if (!rn2(2)) {
			    make_burned(HBurned + tmp, TRUE);
			}
			break;
	      case AD_FEAR:
				You("feel a tight squeezing!");
			if (!rn2(2)) {
			    make_feared(HFeared + tmp, TRUE);
			}
			break;
	      case AD_SLOW:
				You("feel a force field!");
			if (HFast && !defends(AD_SLOW, uwep) && !rn2(4))
			    u_slow_down();
			break;
	      case AD_PLYS:
			pline("It whacks you!");
			if (multi >= 0 && !rn2(3)) {
			    if (Free_action) {
				You("cramp for a moment.");            
			    } else {
				You("can't move!");
				nomovemsg = 0;	/* default: "you can move again" */
				nomul(-rnd(10));
				exercise(A_DEX, FALSE);
			    }
			}
			break;
	      case AD_DRLI:
			pline("It sucks you!");
/* Imagine the facial expression of a player who thinks this is the mind flayer's amnesia attack. --Amy */
			if (!rn2(3) && (!Drain_resistance || !rn2(20) )  ) {
			    losexp("life drainage", FALSE);
			}
			break;
	      case AD_DREN:
			pline("It drains you!");
			if (!rn2(4)) drain_en(tmp);
			break;

	    case AD_GLIB:
			pline("A disgusting substance pours all over your hands!");

			/* hurt the player's hands --Amy */
			incr_itimeout(&Glib, tmp);
			break;

	    case AD_DARK:

			pline("You feel a constricting darkness...");

			/* create darkness around the player --Amy */
			litroomlite(FALSE);
			break;

	    case AD_LEGS:
			{ register long sideX = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
	
			if (tmp <= 5)
		    	    Your("legs itch badly for a moment.");
			else if (tmp <= 10)
			    pline("Wounds appear on your legs!");
			else if (tmp <= 20)
			    pline("Severe wounds appear on your legs!");
			else if (tmp <= 40)
			    Your("legs are covered with painful wounds!");
			else
			    Your("legs are covered with bloody wounds! It hurts like hell! Auuuggghhhh!");
			set_wounded_legs(sideX, rnd(60-ACURR(A_DEX)));
			exercise(A_STR, FALSE);
			exercise(A_DEX, FALSE);
			}
			break;
	    case AD_STON:
	    pline("It strikes you hard!");
		if(!rn2(3)) {
			if (flags.soundok)
			    You_hear("a hissing noise!");
			if(!rn2(10) ||
			    (flags.moonphase == NEW_MOON && !have_lizard())) {
			    if (!Stoned && !Stone_resistance
				    && !(poly_when_stoned(youmonst.data) &&
					polymon(PM_STONE_GOLEM))) {
				Stoned = 7;
				delayed_killer = mtmp->data->mname;
				if (mtmp->data->geno & G_UNIQ) {
				    if (!type_is_pname(mtmp->data)) {
					static char kbuf[BUFSZ];

					/* "the" buffer may be reallocated */
					Strcpy(kbuf, the(delayed_killer));
					delayed_killer = kbuf;
				    }
				    killer_format = KILLED_BY;
				} else killer_format = KILLED_BY_AN;
				return(1);
				/* You("turn to stone..."); */
				/* done_in_by(mtmp); */
			    }
			}
		}
		break;
	    case AD_STCK:
	    pline("You are covered with some sticky substance!");
		if (!u.ustuck && !sticks(youmonst.data))
			setustuck(mtmp);
		break;
	    case AD_SGLD:
		    pline("It shakes you!");
		if (rn2(10)) {stealgold(mtmp);
		break;
		}
		/* fall thru --Amy */

	    case AD_SITM:	/* for now these are the same */
	    case AD_SEDU:
	    case AD_SSEX:
		    pline("It thrusts you!");

			buf[0] = '\0';
		if ( (rnd(100) > ACURR(A_CHA)) && ((mtmp->female) && !flags.female && rn2(5) ) || ((!mtmp->female) && flags.female && rn2(3) ) || 
			((mtmp->female) && flags.female && rn2(2) ) || ((!mtmp->female) && !flags.female && rn2(2) ) )
			{ 
			switch (steal(mtmp, buf)) {
		  case -1:
			return 2;
		  case 0:
			break;
		  default:
			if ( !tele_restrict(mtmp))
			    (void) rloc(mtmp, FALSE);
			monflee(mtmp, 0, FALSE, FALSE);
			return 3;
			};
		}
		break;

	    case AD_TLPT:
			pline("A stream of energy irradiates you!");
		    if(flags.verbose)
			pline("You are teleported away!");
		    teleX();
		break;
	    case AD_DISP:
		pline("You're shaken around!");
		pushplayer();
		break;

	    case AD_RUST:
		pline("You are covered with rust!");
		if (u.umonnum == PM_IRON_GOLEM) {
			You("rust!");
			u.uhp -= mons[u.umonnum].mlevel;
			u.uhpmax -= mons[u.umonnum].mlevel;
			if (u.uhpmax < 1) u.uhpmax = 1;
			/* KMH, balance patch -- this is okay with unchanging */
			u.mh = 0;
			rehumanize();
			break;
		}
		if (rn2(3)) hurtarmor(AD_RUST);
		break;
	    case AD_CORR:
		pline("You are covered with acid!");
		if (Stoned) fix_petrification();
		if (rn2(3)) hurtarmor(AD_CORR);
		break;
	    case AD_WTHR:
		pline("You are covered with some aggressive substance!");
		if (rn2(3)) witherarmor();
		break;
	    case AD_LUCK:
		pline("It's sucking away all of your good feelings!");
		if (!rn2(3)) change_luck(-1);
		break;
	    case AD_CONF:
		    pline("Your head spins wildly!");
		    make_confused(HConfusion + tmp, FALSE);
		break;
	    case AD_DCAY:
		pline("You are covered with decaying waste!");
		if (u.umonnum == PM_WOOD_GOLEM ||
		    u.umonnum == PM_LEATHER_GOLEM) {
			You("rot!");
			u.uhp -= mons[u.umonnum].mlevel;
			u.uhpmax -= mons[u.umonnum].mlevel;
			if (u.uhpmax < 1) u.uhpmax = 1;
			u.mh = 0;
			/* KMH, balance patch -- this is okay with unchanging */
			rehumanize();
			break;
		}
		if (rn2(3)) hurtarmor(AD_DCAY);
		break;
	    case AD_HALU:
		    pline("You inhale some great stuff!");
		    make_hallucinated(HHallucination + tmp,FALSE,0L);
		break;
	    case AD_DETH:
		pline("You see your life flash in front of your eyes...");
		if (is_undead(youmonst.data)) {
		    /* Still does normal damage */
		    pline("But since you're undead already, this doesn't matter to you.");
		    break;
		}
		switch (rn2(20)) {
		case 19: /* case 18: case 17: */
		    if (!Antimagic) {
			killer_format = KILLED_BY_AN;
			killer = "engulf of death";
			done(DIED);
			tmp = 0;
			break;
		    } /* else FALLTHRU */
		default: /* case 16: ... case 5: */
		    You_feel("your life force draining away...");
			if (Antimagic || (Half_spell_damage && rn2(2) )) {
				shieldeff(u.ux, u.uy);
				tmp /= 2;
			}
			u.uhpmax -= tmp/2;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		    break;
		case 4: case 3: case 2: case 1: case 0:
		    if (Antimagic) shieldeff(u.ux, u.uy);
		    pline("Nothing happens.");
		    tmp = 0;
		    break;
		}
		break;
	    case AD_FAMN:
		pline("You are being eaten!");
		exercise(A_CON, FALSE);
		if (!is_fainted()) morehungry(rnz(40));
		morehungry(tmp);
		/* plus the normal damage */
		break;
	    case AD_SLIM:    
		pline("A disgusting green goo pours all over you!");
		if (rn2(3)) break;
		if (flaming(youmonst.data)) {
		    pline_The("slime burns away!");
		    tmp = 0;
		} else if (Unchanging ||
				youmonst.data == &mons[PM_GREEN_SLIME]) {
		    You("are unaffected.");
		    tmp = 0;
		} else if (!Slimed) {
		    You("don't feel very well.");
		    Slimed = 100L;
		    flags.botl = 1;
		    killer_format = KILLED_BY_AN;
		    delayed_killer = mtmp->data->mname;
		} else
		    pline("Yuck!");
		break;
	    case AD_CALM:	/* KMH -- koala attack */
		pline("You feel something sapping your energy!");
		    docalm();
		break;
	    case AD_ENCH:	/* KMH -- remove enchantment (disenchanter) */
		pline("Mysterious waves pulsate through your body!");
		if (rn2(3)) break;
		    struct obj *obj = some_armor(&youmonst);

		    if (drain_item(obj)) {
			Your("%s less effective.", aobjnam(obj, "seem"));
		    }
		break;
	    case AD_POLY:
		pline("A weird green light pierces you!");
		if (!Unchanging && !Antimagic) {
		    if (flags.verbose)
			You("undergo a freakish metamorphosis!");
		    polyself(FALSE);
		}
		break;
	    case AD_TCKL:
		pline("A bunch of feelers suddenly reaches out for you!");
		if (multi >= 0 && !rn2(3)) {
		    if (Free_action)
			pline("They don't seem to do all that much, though...");
		    else {
			pline("Wahahahaha! Arrgh! It tickles!");
			nomovemsg = 0;	/* default: "you can move again" */
			nomul(-rnd(10));
			exercise(A_DEX, FALSE);
			exercise(A_CON, FALSE);
		    }
		}
		break;

		case AD_PHYS:
		    if (mtmp->data == &mons[PM_FOG_CLOUD]) {
			You("are laden with moisture and %s",
			    flaming(youmonst.data) ? "are smoldering out!" :
			    Breathless ? "find it mildly uncomfortable." :
			    amphibious(youmonst.data) ? "feel comforted." :
			    "can barely breathe!");
			/* NB: Amphibious includes Breathless */
			if (Amphibious && !flaming(youmonst.data)) tmp = 0;
		    } else {
			You("are pummeled with debris!");
			exercise(A_STR, FALSE);
		    }
		    break;
		case AD_ACID:
		    if (Acid_resistance && rn2(20)) {
			You("are covered with a seemingly harmless goo.");
			tmp = 0;
		    } else {
		      if (Hallucination) pline("Ouch!  You've been slimed!");
		      else You("are covered in slime!  It burns!");
		      exercise(A_STR, FALSE);
		    }

		if (Stoned) fix_petrification();

		    /* Mik: Go corrode a few things... */
			/*for (otmp2 = invent; otmp2; otmp2 = otmp2->nobj)
			    if (is_corrodeable(otmp2) && !rn2(9))
		    		(void) rust_dmg(otmp2, xname(otmp2), 3, FALSE, 
					&youmonst);*/
			if(rn2(30)) erode_armor(&youmonst, TRUE);
		    break;
		case AD_BLND:
		    if (can_blnd(mtmp, &youmonst, mattk->aatyp, (struct obj*)0)) {
			if(!Blind) {
			    You_cant("see in here!");
			    make_blinded((long)tmp,FALSE);
			    if (!Blind) Your(vision_clears);
			} else
			    /* keep him blind until disgorged */
				/* Amy edit - but also add the actual damage */
			    make_blinded(Blinded+1+(long)tmp,FALSE);
		    }
		    tmp = 0;
		    break;
		case AD_ELEC:
		    if(!mtmp->mcan && rn2(2)) {
			pline_The("air around you crackles with electricity.");
			if (Shock_resistance && rn2(20)) {
				shieldeff(u.ux, u.uy);
				You("seem unhurt.");
				ugolemeffects(AD_ELEC,tmp);
				tmp = 0;
			}
		    } else tmp = 0;
		    break;
		case AD_COLD:
		    if(!mtmp->mcan && rn2(2)) {
			if (Cold_resistance && rn2(20)) {
				shieldeff(u.ux, u.uy);
				You_feel("mildly chilly.");
				ugolemeffects(AD_COLD,tmp);
				tmp = 0;
			} else You("are freezing to death!");
		    } else tmp = 0;
		    break;
		case AD_DRIN:
			pline("It messes with your mind!");
		    if(!mtmp->mcan && !rn2(4)) {
		    if (ABASE(A_INT) <= ATTRMIN(A_INT)) {
			int lifesaved = 0;
			struct obj *wore_amulet = uamul;

			while(1) {
			    /* avoid looping on "die(y/n)?" */
			    if (lifesaved && (discover || wizard)) {
				if (wore_amulet && !uamul) {
				    /* used up AMULET_OF_LIFE_SAVING; still
				       subject to dying from brainlessness */
				    wore_amulet = 0;
				} else {
				    /* explicitly chose not to die;
				       arbitrarily boost intelligence */
				    ABASE(A_INT) = ATTRMIN(A_INT) + 2;
				    You_feel("like a scarecrow.");
				    break;
				}
			    }

			    if (lifesaved)
				pline("Unfortunately your brain is still gone.");
			    else
				Your("last thought fades away.");
			    killer = "brainlessness";
			    killer_format = KILLED_BY;
			    done(DIED);
			    lifesaved++;
			}
		    }

			if (!rn2(10)) (void) adjattrib(A_INT, -rnd(2), FALSE);
			else if (!rn2(2)) (void) adjattrib(A_INT, -1, FALSE);
			forget_levels(5);	/* lose memory of 25% of levels */
			forget_objects(5);	/* lose memory of 25% of objects */
			exercise(A_WIS, FALSE);
		    } else tmp = 0;
		    break;
		case AD_FIRE:
		    if(!mtmp->mcan && rn2(2)) {
			if (Fire_resistance && rn2(20)) {
				shieldeff(u.ux, u.uy);
				You_feel("mildly hot.");
				ugolemeffects(AD_FIRE,tmp);
				tmp = 0;
			} else You("are burning to a crisp!");
			burn_away_slime();
		    } else tmp = 0;
		    break;
		case AD_DISE:
			if (rn2(3)) break; /* lower chance of actually being diseased --Amy */
		case AD_PEST:
		    if (!diseasemu(mtmp->data)) tmp = 0;
		    break;
		case AD_MAGM:
		    if(Antimagic && rn2(5)) {
			tmp = 0;
		    } else {
			You("are irradiated with energy!");
		    }
		    break;
		case AD_DISN:
		    You("feel like a drill is tearing you apart!");
		if (!rn2(10))  {
		if (Disint_resistance && rn2(100)) {
		    You("are not disintegrated.");
		    break;
            } else if (Invulnerable) {
                pline("You are unharmed!");
                break;
		}
		 else if (uarms) {
		    /* destroy shield; other possessions are safe */
		    (void) destroy_arm(uarms);
		    break;
		} else if (uarmc) {
		    /* destroy cloak; other possessions are safe */
		    (void) destroy_arm(uarmc);
		    break;
		} else if (uarm) {
		    /* destroy suit */
		    (void) destroy_arm(uarm);
		    break;
#ifdef TOURIST
		} else if (uarmu) {
		    /* destroy shirt */
		    (void) destroy_arm(uarmu);
		    break;
#endif
		}
	      else if (nonliving(youmonst.data) || is_demon(youmonst.data)) {
		You("seem unaffected.");
		break;
	    } else if (Antimagic && rn2(20)) {
		You("aren't affected.");
		break;
	    }
	    done(DIED);
	    return; /* lifesaved */

	}
		    break;
		default:
		    /*tmp = 0;*/
			pline(rn2(2) ? "It hits you!" : "It hits!");
		    break; /* There is absolutely no reason why this wouldn't do any damage. --Amy */
	}

	/*if (randattackA == 1) {
		mattk->adtyp = AD_RBRE;
		randattackA = 0;
	}*/

	if (Half_physical_damage && rn2(2) ) tmp = (tmp+1) / 2;

	mdamageu(mtmp, tmp);
	if (tmp) stop_occupation();

	if (touch_petrifies(youmonst.data) && !resists_ston(mtmp)) {
	    pline("%s very hurriedly %s you!", Monnam(mtmp),
		  is_animal(mtmp->data)? "regurgitates" : "expels");
	    expels(mtmp, mtmp->data, FALSE);
	} else if (!u.uswldtim || youmonst.data->msize >= MZ_HUGE) {
	    You("get %s!", is_animal(mtmp->data)? "regurgitated" : "expelled");
	    if (flags.verbose && (is_animal(mtmp->data) ||
		    (dmgtype(mtmp->data, AD_DGST) && Slow_digestion)))
		pline("Obviously %s doesn't like your taste.", mon_nam(mtmp));
	    expels(mtmp, mtmp->data, FALSE);
	}
	return(1);
}

STATIC_OVL int
explmu(mtmp, mattk, ufound)	/* monster explodes in your face */
register struct monst *mtmp;
register struct attack  *mattk;
boolean ufound;
{

    if (mtmp->mcan) return(0);

    if (!ufound) {
	if (rn2(20)) return(0); /* make it a lot less likely for them to blow up in vain --Amy */
	pline("%s explodes at a spot in %s!",
	    canseemon(mtmp) ? Monnam(mtmp) : "It",
	    levl[mtmp->mux][mtmp->muy].typ == WATER
		? "empty water" : "thin air");
	}
    else {
	register int tmp = d((int)mattk->damn, (int)mattk->damd);
	register boolean not_affected = defends((int)mattk->adtyp, uwep);

	hitmsg(mtmp, mattk);

	switch (mattk->adtyp) {
	    case AD_CONF:
		    if (mon_visible(mtmp) || (rnd(tmp /= 2) > u.ulevel)) {
			if (!Hallucination) You("are confused by a blast of light!");
			else pline("%s TO ORDER! %s MESSED!",urole.name.m,urole.name.m);
			make_confused(HConfusion + (long)tmp, FALSE); }
		break;

	    case AD_ACID:
		not_affected |= Acid_resistance;
		if (Stoned) fix_petrification();
		goto common;
	    case AD_COLD:
		not_affected |= Cold_resistance;
		goto common;
	    case AD_FIRE:
		not_affected |= Fire_resistance;
		goto common;
	    case AD_ELEC:
		not_affected |= Shock_resistance;
	    case AD_PHYS: /* just do some plain physical damage, for golem's selfdestruct attack et. al. --Amy */
common:

		if (!not_affected || !rn2(10) ) {
		    if (ACURR(A_DEX) > rnd(200)) {
			You("duck some of the blast.");
			tmp = (tmp+1) / 2;
		    } else {
		        if (flags.verbose) You("get blasted!");
		    }
		    if (mattk->adtyp == AD_FIRE) burn_away_slime();
		    if (Half_physical_damage && rn2(2) ) tmp = (tmp+1) / 2;
		    mdamageu(mtmp, tmp);
		}
		break;

	    case AD_BLND:
		not_affected = resists_blnd(&youmonst);
		if (!not_affected) {
		    /* sometimes you're affected even if it's invisible */
		    if (mon_visible(mtmp) || (rnd(tmp /= 2) > u.ulevel)) {
			You("are blinded by a blast of light!");
			make_blinded((long)tmp, FALSE);
			if (!Blind) Your(vision_clears);
		    } else if (flags.verbose)
			You("get the impression it was not terribly bright.");
		}
		break;

	    case AD_RUST:

		water_damage(invent, FALSE, FALSE);

		break;

	    case AD_DRST:

		if (!Poison_resistance || !rn2(5) ) {
		poisoned("blast", A_STR, "poison explosion", 30);
		}
	      mdamageu(mtmp, tmp); /* still does damage even if you resist the poison --Amy */
		break;

	    case AD_DRDX:

		if (!Poison_resistance || !rn2(5) ) {
		poisoned("blast", A_DEX, "poison explosion", 30);
		}
	      mdamageu(mtmp, tmp); /* still does damage even if you resist the poison --Amy */
		break;

	    case AD_DRCO:

		if (!Poison_resistance || !rn2(5) ) {
		poisoned("blast", A_CON, "poison explosion", 30);
		}
	      mdamageu(mtmp, tmp); /* still does damage even if you resist the poison --Amy */
		break;

	    case AD_WTHR:

		withering_damage(invent, FALSE, FALSE);

		break;

	    case AD_STUN:
		not_affected = (Blind && rn2(2));
		if (!not_affected) {
			if (!Hallucination) You("are stunned by a blast of light!");
			else pline("%s NUMBED CAN'T DO",urole.name.m);
			make_stunned(HStun + (long)tmp, FALSE);
		}
		break;

	    case AD_NUMB:
		not_affected = (Blind && rn2(2));
		if (!not_affected) {
			if (!Hallucination) You("are numbed by a blast of light!");
			else pline("%s NUMBED YER",urole.name.m);
			make_numbed(HNumbed + (long)tmp, FALSE);
		}
		break;

	    case AD_FRZE:
		not_affected = (Blind && rn2(2));
		if (!not_affected) {
			if (!Hallucination) You("are frozen solid by a blast of light!");
			else pline("%s COOLING",urole.name.m);
			make_frozen(HFrozen + (long)tmp, FALSE);
		}
		break;

	    case AD_BURN:
		not_affected = (Blind && rn2(2));
		if (!not_affected) {
			if (!Hallucination) You("are burned by a blast of light!");
			else pline("%s BURNED",urole.name.m);
			make_burned(HBurned + (long)tmp, FALSE);
		}
		break;

	    case AD_FEAR:
		not_affected = (Blind && rn2(2));
		if (!not_affected) {
			if (!Hallucination) You("are stricken with fear by a blast of light!");
			else pline("%s FEARED",urole.name.m);
			make_feared(HFeared + (long)tmp, FALSE);
		}
		break;

	    case AD_HALU:
		not_affected |= (Blind && rn2(2)) ||
			(u.umonnum == PM_BLACK_LIGHT ||
			 u.umonnum == PM_VIOLET_FUNGUS ||
			 dmgtype(youmonst.data, AD_STUN));
		if (!not_affected) {
		    boolean chg;
		    if (!Hallucination)
			You("are caught in a blast of kaleidoscopic light!");
		    chg = make_hallucinated(HHallucination + (long)tmp,FALSE,0L);
		    You("%s.", chg ? "are freaked out" : "seem unaffected");
		}
		break;

	    default:
		break;
	}
	if (not_affected) {
	    You("seem unaffected by it.");
	    ugolemeffects((int)mattk->adtyp, tmp);
	}
    }
    mondead(mtmp);
    wake_nearto(mtmp->mx, mtmp->my, 7*7);
    if (mtmp->mhp > 0) return(0);
    return(2);	/* it dies */
}

int
gazemu(mtmp, mattk)	/* monster gazes at you */
	register struct monst *mtmp;
	register struct attack  *mattk;
{
	register struct engr *ep = engr_at(u.ux,u.uy);
	char	 buf[BUFSZ];

	/*int randattackB = 0;*/
	uchar atttypB;

	/* Monsters with AD_RBRE can use any random gaze. --Amy */

	atttypB = mattk->adtyp;

	if (atttypB == AD_RBRE) {
		while (atttypB == AD_ENDS ||atttypB == AD_RBRE || atttypB == AD_SPC2 || atttypB == AD_WERE) {
			atttypB = randattack(); }
		/*randattack = 1;*/
	}

	switch(atttypB) {
	    case AD_STON:
		if (mtmp->mcan || !mtmp->mcansee) {
		    if (!canseemon(mtmp)) break;	/* silently */
		    pline("%s %s.", Monnam(mtmp),
			  (mtmp->data == &mons[PM_MEDUSA] && mtmp->mcan) ?
				"doesn't look all that ugly" :
				"gazes ineffectually");
		    break;
		}
		if (Reflecting && couldsee(mtmp->mx, mtmp->my) &&
			mtmp->data == &mons[PM_MEDUSA]) {
		    /* hero has line of sight to Medusa and she's not blind */
		    boolean useeit = canseemon(mtmp);

		    if (useeit)
			(void) ureflects("%s gaze is reflected by your %s.",
					 s_suffix(Monnam(mtmp)));
		    if (mon_reflects(mtmp, !useeit ? (char *)0 :
				     "The gaze is reflected away by %s %s!"))
			break;
		    if (!m_canseeu(mtmp)) { /* probably you're invisible */
			if (useeit)
			    pline(
		      "%s doesn't seem to notice that %s gaze was reflected.",
				  Monnam(mtmp), mhis(mtmp));
			break;
		    }
		    if (useeit)
			pline("%s is turned to stone!", Monnam(mtmp));
		    stoned = TRUE;
		    killed(mtmp);

		    if (mtmp->mhp > 0) break;
		    return 2;
		}
		if (canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my) &&
		    !Stone_resistance && !rn2(15) ) {
		    You("meet %s gaze.", s_suffix(mon_nam(mtmp)));
		    stop_occupation();
		    if(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
			break;
		    /*You("turn to stone...");
		    killer_format = KILLED_BY;
		    killer = mtmp->data->mname;
		    done(STONING);*/
		    You("start turning to stone...");
			if (!Stoned) Stoned = 7; delayed_killer = "petrifying gaze";
		}
		break;
	    case AD_CURS:
	    case AD_LITE:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && rn2(5))
 		{
		pline("%s gives you a mean look!", Monnam(mtmp));

			if(!rn2(10) || (night() && !rn2(3)) )  {
			    if (u.umonnum == PM_CLAY_GOLEM) {
				pline("Some writing vanishes from your head!");
				u.uhp -= mons[u.umonnum].mlevel;
				u.uhpmax -= mons[u.umonnum].mlevel;
				if (u.uhpmax < 1) u.uhpmax = 1;
				/* KMH, balance patch -- this is okay with unchanging */
				u.mh = 0;
				rehumanize();
				break;
			    }
				pline("%s laughs fiendishly!", Monnam(mtmp));
			    attrcurse();
			}
		}
		break;
	    case AD_MAGM:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && rn2(5))
 		{
		pline("%s's eye color suddenly changes!", Monnam(mtmp));
		    if(Antimagic && !rn2(3)) {
			shieldeff(u.ux, u.uy);
			pline("A hail of magic missiles narrowly misses you!");
		    } else {
			You("are hit by magic missiles appearing from thin air!");
	            mdamageu(mtmp, d(4,6));
		    }
		}
	    break;
	    case AD_DISN:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(4))
 		{
		if (!rn2(20))  {
		pline("%s's gaze seems to drill right into you!", Monnam(mtmp));
		if (Disint_resistance && rn2(100)) {
		    You("are not disintegrated.");
		    break;
            } else if (Invulnerable) {
                pline("You are unharmed!");
                break;
		} else if (uarms) {
		    /* destroy shield; other possessions are safe */
		    (void) destroy_arm(uarms);
		    break;
		} else if (uarmc) {
		    /* destroy cloak; other possessions are safe */
		    (void) destroy_arm(uarmc);
		    break;
		} else if (uarm) {
		    /* destroy suit */
		    (void) destroy_arm(uarm);
		    break;
#ifdef TOURIST
		} else if (uarmu) {
		    /* destroy shirt */
		    (void) destroy_arm(uarmu);
		    break;
#endif
		}
	      else if (nonliving(youmonst.data) || is_demon(youmonst.data)) {
		You("seem unaffected.");
		break;
	    } else if (Antimagic && rn2(20)) {
		You("aren't affected.");
		break;
	    }
	    done(DIED);
	    return; /* lifesaved */

		}
		}
	      break;
	    case AD_ACID:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && rn2(5))
 		{

		if(!rn2(3)) {
		pline("%s sends a terrifying gaze at you!", Monnam(mtmp));
		    if (Acid_resistance && rn2(20)) {
			pline("You're covered in acid, but it seems harmless.");
		    } else {
			pline("You're covered in acid! It burns!");
			exercise(A_STR, FALSE);
		if (Stoned) fix_petrification();
		    int dmg = d(2,6);
		    if (dmg) mdamageu(mtmp, dmg);
		    }
			if(rn2(30)) erode_armor(&youmonst, TRUE);
		}
		}
		break;
	      case AD_DRLI:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && rn2(5))
 		{
			if (!rn2(7) && (!Drain_resistance || !rn2(20) )  ) {
				pline("%s seems to drain your life with its gaze!", Monnam(mtmp));
			    losexp("life drainage", FALSE);
			}
		}
		break;

	      case AD_DREN:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && rn2(5))
 		{
			pline("%s seems to drain your energy with its gaze!", Monnam(mtmp));
			if (!rn2(4)) {drain_en(10);
			}
		}
		break;
	    case AD_NGRA:
		if (!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && rn2(5))
 		{

		      if (ep && sengr_at("Elbereth", u.ux, u.uy) ) {
		/* This attack can remove any Elbereth engraving, even burned ones. --Amy */
			pline("%s seems to suck in the words engraved on the surface below you!", Monnam(mtmp));
		    del_engr(ep);
		    ep = (struct engr *)0;
			}
		}

		break;

	    case AD_GLIB:
		if (!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(10))
 		{

		/* hurt the player's hands --Amy */
		pline("%s telepathically twists your hands!", Monnam(mtmp));
		incr_itimeout(&Glib, (int)mattk->damd );

		}
		break;

	    case AD_DARK:
		if (!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(4))
 		{

		/* create darkness around the player --Amy */
		pline("%s's sinister gaze fills your mind with dreadful, evil thoughts!", Monnam(mtmp));
		litroomlite(FALSE);
		}
		break;

	    case AD_LEGS:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && rn2(5))
 		{
			{ register long sideX = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
	
			pline("%s's gaze makes your legs turn to jelly!", Monnam(mtmp));
			set_wounded_legs(sideX, rnd(60-ACURR(A_DEX)));
			exercise(A_STR, FALSE);
			exercise(A_DEX, FALSE);
			}
		}
		break;

	    case AD_SLIM:    
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(15))
 		{

		pline("%s hurls some disgusting green goo at you!", Monnam(mtmp));

		if (flaming(youmonst.data)) {
		    pline_The("slime burns away!");
		} else if (Unchanging ||
				youmonst.data == &mons[PM_GREEN_SLIME]) {
		    You("are unaffected.");
		} else if (!Slimed) {
		    You("don't feel very well.");
		    Slimed = 100L;
		    flags.botl = 1;
		    killer_format = KILLED_BY_AN;
		    delayed_killer = mtmp->data->mname;
		} else
		    pline("Yuck!");
		}
		break;
	    case AD_CALM:	/* KMH -- koala attack */
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && rn2(5))
 		{
		pline("%s gazes at you softly.", Monnam(mtmp));
		    docalm();
		}
		break;
	    case AD_ENCH:	/* KMH -- remove enchantment (disenchanter) */
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(25))
 		{
		pline("%s gazes at your belongings!", Monnam(mtmp));
		/* uncancelled is sufficient enough; please
		   don't make this attack less frequent */
		    struct obj *obj = some_armor(&youmonst);

		    if (drain_item(obj)) {
			Your("%s less effective.", aobjnam(obj, "seem"));
		    }
		}
		break;
	    case AD_POLY:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(20))
 		{
		if (!Unchanging && !Antimagic) {
		    if (flags.verbose)
			pline("%s throws a changing gaze at you!", Monnam(mtmp));
		    polyself(FALSE);
			}
		}
		break;

	    case AD_CONF:
		if(!mtmp->mcan && canseemon(mtmp) &&
		   couldsee(mtmp->mx, mtmp->my) &&
		   mtmp->mcansee && !mtmp->mspec_used && !rn2(5)) {
		    int conf = d(3,4);

		    mtmp->mspec_used = mtmp->mspec_used + (conf + rn2(6));
		    if(!Confusion)
			pline("%s gaze confuses you!",
			                  s_suffix(Monnam(mtmp)));
		    else
			You("are getting more and more confused.");
		    make_confused(HConfusion + conf, FALSE);
		    stop_occupation();
		}
		break;
	    case AD_FAMN:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(4)) 		{
		pline("%s gazes at you with its hungry eyes!",
			Monnam(mtmp));
		exercise(A_CON, FALSE);
		if (!is_fainted()) morehungry(rnz(40));
		/* plus the normal damage */
		}
		break;

	    case AD_SGLD:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && rn2(5))
 		{
			pline("%s uses a telepathic gaze!", Monnam(mtmp));
			if (rn2(10)) stealgold(mtmp);
			else if( (rnd(100) > ACURR(A_CHA)) &&  !mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && /*!rn2(25)*/ 
		( ((mtmp->female) && !flags.female && !rn2(5) ) || ((!mtmp->female) && flags.female && !rn2(15) ) || 
			((mtmp->female) && flags.female && !rn2(25) ) || ((!mtmp->female) && !flags.female && !rn2(25) ) ) )
			{
				buf[0] = '\0';
				switch (steal(mtmp, buf)) {
			  case -1:
				return 2;
			  case 0:
				break;
			  default:
				if ( !tele_restrict(mtmp))
				    (void) rloc(mtmp, FALSE);
				monflee(mtmp, 0, FALSE, FALSE);
				return 3;
				};

			}
		}
		break;

	    case AD_SITM:	/* for now these are the same */
	    case AD_SEDU:
	    case AD_SSEX:
	      if( (rnd(100) > ACURR(A_CHA)) &&  !mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && /*!rn2(25)*/ 
		( ((mtmp->female) && !flags.female && !rn2(5) ) || ((!mtmp->female) && flags.female && !rn2(15) ) || 
			((mtmp->female) && flags.female && !rn2(25) ) || ((!mtmp->female) && !flags.female && !rn2(25) ) )

		) 		{
		pline("%s gazes at you with its demanding eyes!",
			Monnam(mtmp));
		buf[0] = '\0';
			switch (steal(mtmp, buf)) {
		  case -1:
			return 2;
		  case 0:
			break;
		  default:
			if ( !tele_restrict(mtmp))
			    (void) rloc(mtmp, FALSE);
			monflee(mtmp, 0, FALSE, FALSE);
			return 3;
			};
		}
		break;
	    case AD_RUST:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(5)) 		{
		pline("%s squirts water at you!", Monnam(mtmp));

		if (u.umonnum == PM_IRON_GOLEM) {
			You("rust!");
			u.uhp -= mons[u.umonnum].mlevel;
			u.uhpmax -= mons[u.umonnum].mlevel;
			if (u.uhpmax < 1) u.uhpmax = 1;
			/* KMH, balance patch -- this is okay with unchanging */
			u.mh = 0;
			rehumanize();
			break;
		}
		hurtarmor(AD_RUST);
		}
		break;
	    case AD_DCAY:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(5)) 		{
		pline("%s flings organic matter at you!", Monnam(mtmp));

		if (u.umonnum == PM_WOOD_GOLEM ||
		    u.umonnum == PM_LEATHER_GOLEM) {
			You("rot!");
			u.uhp -= mons[u.umonnum].mlevel;
			u.uhpmax -= mons[u.umonnum].mlevel;
			if (u.uhpmax < 1) u.uhpmax = 1;
			u.mh = 0;
			/* KMH, balance patch -- this is okay with unchanging */
			rehumanize();
			break;
		}
		hurtarmor(AD_DCAY);
		}
		break;
	    case AD_CORR:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(5)) 		{
		pline("%s throws corrosive stuff at you!", Monnam(mtmp));
		hurtarmor(AD_CORR);
		}
		break;
	    case AD_WTHR:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(5)) 		{
		pline("%s telepathically messes with your clothes!", Monnam(mtmp));
		witherarmor();
		}
		break;
	    case AD_LUCK:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(20)) 		{
		pline("%s's terrifying gaze makes you feel like you'll never be able to experience luck again!", Monnam(mtmp));
		change_luck(-1);
		}
		break;
	    case AD_STUN:
		if(!mtmp->mcan && canseemon(mtmp) &&
		   couldsee(mtmp->mx, mtmp->my) &&
		   mtmp->mcansee && !mtmp->mspec_used && !rn2(5)) {
		    int stun = d(2,6);

		    mtmp->mspec_used = mtmp->mspec_used + (stun + rn2(6));
		    pline("%s stares piercingly at you!", Monnam(mtmp));
		    make_stunned(HStun + stun, TRUE);
		    stop_occupation();
		}
		break;
	    case AD_NUMB:
		if(!mtmp->mcan && canseemon(mtmp) &&
		   couldsee(mtmp->mx, mtmp->my) &&
		   mtmp->mcansee && !mtmp->mspec_used && !rn2(5)) {
		    int numb = d(2,6);

		    mtmp->mspec_used = mtmp->mspec_used + (numb + rn2(6));
		    pline("%s stares numbingly at you!", Monnam(mtmp));
		    make_numbed(HNumbed + numb, TRUE);
		    stop_occupation();
		}
		break;

	    case AD_FRZE:
		if(!mtmp->mcan && canseemon(mtmp) &&
		   couldsee(mtmp->mx, mtmp->my) &&
		   mtmp->mcansee && !mtmp->mspec_used && !rn2(25)) {
		    int frze = d(2,6);

		    mtmp->mspec_used = mtmp->mspec_used + (frze + rn2(6));
		    pline("%s stares freezingly at you!", Monnam(mtmp));
		    make_frozen(HFrozen + frze, TRUE);
		    stop_occupation();
		}
		break;

	    case AD_BURN:
		if(!mtmp->mcan && canseemon(mtmp) &&
		   couldsee(mtmp->mx, mtmp->my) &&
		   mtmp->mcansee && !mtmp->mspec_used && !rn2(5)) {
		    int burn = d(2,6);

		    mtmp->mspec_used = mtmp->mspec_used + (burn + rn2(6));
		    pline("%s stares burningly at you!", Monnam(mtmp));
		    make_burned(HBurned + burn, TRUE);
		    stop_occupation();
		}
		break;

	    case AD_FEAR:
		if(!mtmp->mcan && canseemon(mtmp) &&
		   couldsee(mtmp->mx, mtmp->my) &&
		   mtmp->mcansee && !mtmp->mspec_used && !rn2(5)) {
		    int fearing = d(2,6);

		    mtmp->mspec_used = mtmp->mspec_used + (fearing + rn2(6));
		    pline("%s stares terrifyingly at you!", Monnam(mtmp));
		    make_feared(HFeared + fearing, TRUE);
		    stop_occupation();
		}
		break;

	    case AD_BLND:
		if (!mtmp->mcan && canseemon(mtmp) && !resists_blnd(&youmonst)
			&& distu(mtmp->mx,mtmp->my) <= BOLT_LIM*BOLT_LIM && !rn2(6) ) {
		    int blnd = d((int)mattk->damn, (int)mattk->damd);

		    You("are blinded by %s radiance!",
			              s_suffix(mon_nam(mtmp)));
		    make_blinded((long)blnd,FALSE);
		    stop_occupation();
		    /* not blind at this point implies you're wearing
		       the Eyes of the Overworld; make them block this
		       particular stun attack too */
		    if (!Blind) Your(vision_clears);
		    else make_stunned((long)d(1,3),TRUE);
		}
		break;
	    case AD_FIRE:
		if (!mtmp->mcan && canseemon(mtmp) &&
			couldsee(mtmp->mx, mtmp->my) &&
			mtmp->mcansee && !mtmp->mspec_used && !rn2(5)) {
		    int dmg = d(2,6);

		    pline("%s attacks you with a fiery gaze!", Monnam(mtmp));
		    stop_occupation();
		    if (Fire_resistance && rn2(20)) {
			pline_The("fire doesn't feel hot!");
			dmg = 0;
		    }
		    burn_away_slime();
		    /*if ((int) mtmp->m_lev > rn2(20))
			destroy_item(SCROLL_CLASS, AD_FIRE);
		    if ((int) mtmp->m_lev > rn2(20))
			destroy_item(POTION_CLASS, AD_FIRE);
		    if ((int) mtmp->m_lev > rn2(25))
			destroy_item(SPBOOK_CLASS, AD_FIRE);*/
		    if (!rn2(33)) /* new calculations --Amy */
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		    if (!rn2(33))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (!rn2(50))
		      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);

		    if (dmg) mdamageu(mtmp, dmg);
		}
		break;
	    case AD_COLD:
		if (!mtmp->mcan && canseemon(mtmp) &&
			couldsee(mtmp->mx, mtmp->my) &&
			mtmp->mcansee && !mtmp->mspec_used && !rn2(5)) {
		    int dmg = d(2,6);

		    pline("%s attacks you with an icy gaze!", Monnam(mtmp));
		    stop_occupation();
		    if (Cold_resistance && rn2(20)) {
			pline_The("cold doesn't freeze you!");
			dmg = 0;
		    }
		    /*if ((int) mtmp->m_lev > rn2(20))*/
		    if (!rn2(33)) /* new calculations --Amy */
			destroy_item(POTION_CLASS, AD_COLD);
		    if (dmg) mdamageu(mtmp, dmg);
		}
		break;
	    case AD_ELEC:
		if (!mtmp->mcan && canseemon(mtmp) &&
			couldsee(mtmp->mx, mtmp->my) &&
			mtmp->mcansee && !mtmp->mspec_used && !rn2(5)) {
		    int dmg = d(2,6);

		    pline("%s attacks you with a shocking gaze!", Monnam(mtmp));
		    stop_occupation();
		    if (Shock_resistance && rn2(20)) {
			pline_The("gaze doesn't shock you!");
			dmg = 0;
		    }
		    /*if((int) mtmp->m_lev > rn2(20))*/
		    if (!rn2(33)) /* new calculations --Amy */
			destroy_item(WAND_CLASS, AD_ELEC);
		    /*if((int) mtmp->m_lev > rn2(20))*/
		    if (!rn2(33)) /* new calculations --Amy */
			destroy_item(RING_CLASS, AD_ELEC);
		    if (dmg) mdamageu(mtmp, dmg);
		}
		break;
       case AD_DRIN:
     if(!mtmp->mcan && canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my) && mtmp->mcansee && !rn2(10) &&
        (!ublindf || ublindf->otyp != TOWEL)  &&
        !mtmp->mspec_used){
       pline("%s screeches at you!", Monnam(mtmp));
       if (u.usleep){
         multi = -1;
         nomovemsg = "You wake.";
       }
         if (ABASE(A_INT) > ATTRMIN(A_INT) && !rn2(10)) {
           /* adjattrib gives dunce cap message when appropriate */
           (void) adjattrib(A_INT, -1, FALSE);
           losespells();
           forget_map(0);
           docrt();
         }
         mtmp->mspec_used += ABASE(A_INT) * rn1(1,3);
     }
     break;
#ifdef PM_BEHOLDER /* work in progress */
#if 0
	    case AD_SLEE:
		if(!mtmp->mcan && canseemon(mtmp) &&
		   couldsee(mtmp->mx, mtmp->my) && mtmp->mcansee &&
		   multi >= 0 && !rn2(5) && !Sleep_resistance) {

		    fall_asleep(-rnd(10), TRUE);
		    pline("%s gaze makes you very sleepy...",
			  s_suffix(Monnam(mtmp)));
		}
		break;
#endif
	    case AD_SLOW:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee &&
		   (HFast & (INTRINSIC|TIMEOUT)) &&
		   !defends(AD_SLOW, uwep) && !rn2(4)) {

		    pline("%s uses a slowing gaze!",Monnam(mtmp));

		    u_slow_down();
		    stop_occupation();
			}
		break;
#endif
	    case AD_SLEE:
		if(!mtmp->mcan && canseemon(mtmp) &&
				mtmp->mcansee && !mtmp->mspec_used && !rn2(5)) {
		    if (Displaced && rn2(3)) {
			if (!Blind) pline("%s gazes at your displaced image!",Monnam(mtmp));
			    break;
		    }
		    if ((Invisible && rn2(3)) || rn2(4)) {
			if (!Blind) pline("%s gazes around, but misses you!",Monnam(mtmp));
			break;
		    }
		    if (!Blind) pline("%s gazes directly at you!",Monnam(mtmp));
		    if(Reflecting && m_canseeu(mtmp) && !mtmp->mcan) {
			if(!Blind) {
		    	    (void) ureflects("%s gaze is reflected by your %s.",
		    			s_suffix(Monnam(mtmp)));
		    	    if (mon_reflects(mtmp,
		    			"The gaze is reflected away by %s %s!"))
				break;
			}
			if (sleep_monst(mtmp, rnd(10), -1) && !Blind)
			    pline("%s is put to sleep!", Monnam(mtmp));
			break;
		    } else if (Sleep_resistance && rn2(20)) {
			pline("You yawn.");
		    } else {
			nomul(-rnd(10));
			u.usleep = 1;
			nomovemsg = "You wake up.";
			if (Blind)  You("are put to sleep!");
			else You("are put to sleep by %s!",mon_nam(mtmp));
		    }
		}
		break;
	    case AD_DETH:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && !rn2(18)) {
		    if (Displaced && rn2(3)) {
			if (!Blind) pline("%s gazes at your displaced image!",Monnam(mtmp));
			    break;
		    }
		    if ((Invisible && rn2(3)) || rn2(4)) {
			if (!Blind) pline("%s gazes around, but misses you!",Monnam(mtmp));
			break;
		    }
		    if ((!Blind) && !rn2(8)) pline("%s gazes directly at you!",Monnam(mtmp));
		    if(Reflecting && m_canseeu(mtmp) && !mtmp->mcan && !resists_death(mtmp) && mtmp->data->msound != MS_NEMESIS  ) {
			if(!Blind) {
		    	    (void) ureflects("%s gaze is reflected by your %s.",
		    			s_suffix(Monnam(mtmp)));
		    	    if (mon_reflects(mtmp,
		    			"The gaze is reflected away by %s %s!"))
				break;
			    pline("%s is killed by its own gaze of death!",
							Monnam(mtmp));
			}
			killed(mtmp);
			if (mtmp->mhp > 0) break;
			return 2;
		    } else if (is_undead(youmonst.data)) {
			/* Still does normal damage */
			pline("Was that the gaze of death?");
			break;
		    } else if (rn2(5) ) {
			/* Still does normal damage */
			pline("It is pitch black...");
			losehp(15, "black gaze", KILLED_BY_AN);
			u.uhpmax -= 2;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			break;
		    } else if (Antimagic) {
			You("shudder momentarily...");
		    } else {
			You(Race_if(PM_ANGBANDER) ? "have died." : "die...");
			killer_format = KILLED_BY_AN;
			killer = "gaze of death";
			done(DIED);
		    }
		}
		break;
	    case AD_PHYS:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && rn2(3)) {
	                if (Displaced && rn2(3)) {
	                        if (!Blind) pline("%s gazes at your displaced image!",Monnam(mtmp));
	                        break;
	                }
	                if ((Invisible && rn2(3)) || rn2(4)) {
	                        if (!Blind) pline("%s gazes around, but misses you!",Monnam(mtmp));
	                        break;
	                }
	                if (!Blind) pline("%s gazes directly at you!",Monnam(mtmp));
	                pline("You are wracked with pains!");
	                mdamageu(mtmp, d(3,8));
	        }
	        break;
	    case AD_DRST:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && !rn2(5)) {
	                pline("%s stares into your eyes...", Monnam(mtmp));
	                poisoned("The gaze", A_STR, mtmp->data->mname, 30);
	        }
	        break;
	    case AD_DRDX:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && !rn2(5)) {
	                pline("%s stares into your eyes...", Monnam(mtmp));
	                poisoned("The gaze", A_DEX, mtmp->data->mname, 30);
	        }
	        break;
	    case AD_DRCO:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && !rn2(5)) {
	                pline("%s stares into your eyes...", Monnam(mtmp));
	                poisoned("The gaze", A_CON, mtmp->data->mname, 30);
	        }
	        break;
          case AD_DISE:
		if (rn2(3)) break; /* lower chance for normal disease, so pestilence attack is unique --Amy */
          case AD_PEST:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(12)) {
	                pline("%s leers down on you!", Monnam(mtmp));
			(void) digeasemu(mtmp); /* plus the normal damage */
	        }
	        break;
	    case AD_HALU:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(5)) 		{
		    boolean chg;
		    if (!Hallucination)
			You("suddenly see a mess of colors!");
		    chg = make_hallucinated(HHallucination + (int)mattk->damd*10,FALSE,0L);
		    You("%s.", chg ? "are getting very trippy" : "seem to get even more trippy");
		}
		break;
	    case AD_PLYS:
	        if(!mtmp->mcan && multi >= 0 && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && !rn2(5)) {
	                pline("%s stares at you!", Monnam(mtmp));
	                if (Free_action) You("stiffen momentarily.");
	                else {
	                        You("are frozen by %s!", mon_nam(mtmp));
				nomovemsg = 0;
	                        nomul(-rnd(4));
	                        exercise(A_DEX, FALSE);
	                }
	        }
	        break;
	    case AD_TLPT:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && !rn2(15)) {
	                pline("%s stares blinkingly at you!", Monnam(mtmp));
	                if(flags.verbose)
	                        Your("position suddenly seems very uncertain!");
	                teleX();
		}
		break;
	    case AD_DISP:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && !rn2(5)) {
	                pline("%s telepathically tries to move you around!", Monnam(mtmp));
		pushplayer();
		}
		break;
	    default: /*impossible*/pline("Gaze attack %d?", mattk->adtyp);
		break;
	}

	/*if (randattackB == 1) {
		mattk->adtyp = AD_RBRE;
		randattackB = 0;
	}*/

	return(0);
}

#endif /* OVLB */
#ifdef OVL1

void
mdamageu(mtmp, n)	/* mtmp hits you for n points damage */
register struct monst *mtmp;
register int n;
{
	if (!strncmpi(plname, "IWBTG", 5)) {

		You("explode in a fountain of red pixels!");
		pline("GAME OVER - press R to try again");
		done_in_by(mtmp);

	}

	/* sometimes you take less damage. The game is deadly enough already. High constitution helps. --Amy */
	if (rn2(ABASE(A_CON))) {
	if (!rn2(3) && n >= 1) {n = n / 2; if (n < 1) n = 1;}
	if (!rn2(10) && n >= 1 && u.ulevel >= 10) {n = n / 3; if (n < 1) n = 1;}
	if (!rn2(20) && n >= 1 && u.ulevel >= 20) {n = n / 5; if (n < 1) n = 1;}
	if (!rn2(50) && n >= 1 && u.ulevel >= 30) {n = n / 10; if (n < 1) n = 1;}
	}

	if (Role_if(PM_BLEEDER)) n = n * 2; /* bleeders are harder than hard mode */

	if (Invulnerable) n=0;
	if (n == 0) {
		pline("You are unharmed.");
		return;
	}

	/* WAC For consistency...DO be careful using techniques ;B */
	if (mtmp->mtame != 0 && tech_inuse(T_PRIMAL_ROAR)) {
		n *= 2; /* Double Damage! */
	}

#ifdef SHOW_DMG
	if (flags.showdmg) {

		pline("[-%d -> %d]", n, (Upolyd ? (u.mh - n) : (u.uhp - n) ) );  /* WAC see damage */
		if (!Upolyd && (( (u.uhp - n) * 5) < u.uhpmax)) pline(Race_if(PM_ANGBANDER) ? "***LOW HITPOINT WARNING***" : "Warning: HP low!");

	}
#endif
	flags.botl = 1; /* This needs to be AFTER the pline for botl to be 
	 		 * updated correctly -- Kelly Bailey
	 		 */

	if (Upolyd) {
		u.mh -= n;
		if (u.mh < 1) {                
			if (Polymorph_control || !rn2(3)) {
			    u.uhp -= mons[u.umonnum].mlevel;
			    u.uhpmax -= mons[u.umonnum].mlevel;
			    if (u.uhpmax < 1) u.uhpmax = 1;
			}
			rehumanize();
		}
	} else {
		u.uhplast = u.uhp;
		u.uhp -= n;
		if (u.uhp >= 1 && n > 0 && u.uhp*10 < u.uhpmax)	maybe_wail(); /* Wizard is about to die. --Amy */
		if(u.uhp < 1) done_in_by(mtmp);
	}
}

#endif /* OVL1 */
#ifdef OVLB

STATIC_OVL void
urustm(mon, obj)
register struct monst *mon;
register struct obj *obj;
{
	boolean vis;
	boolean is_acid;

	if (!mon || !obj) return; /* just in case */
	if (dmgtype(youmonst.data, AD_CORR))
	    is_acid = TRUE;
	else if (dmgtype(youmonst.data, AD_RUST))
	    is_acid = FALSE;
	else
	    return;

	vis = cansee(mon->mx, mon->my);

	if ((is_acid ? is_corrodeable(obj) : is_rustprone(obj)) &&
	    (is_acid ? obj->oeroded2 : obj->oeroded) < MAX_ERODE) {
		if (obj->greased || obj->oerodeproof || (obj->blessed && rn2(3))) {
		        if (vis) pline("Somehow, %s weapon is not affected.",
						s_suffix(mon_nam(mon)));
		    if (obj->greased && !rn2(2)) obj->greased = 0;
		} else {
		        if (vis) pline("%s %s%s!",
			        s_suffix(Monnam(mon)),
				aobjnam(obj, (is_acid ? "corrode" : "rust")),
			        (is_acid ? obj->oeroded2 : obj->oeroded)
				    ? " further" : "");
		    if (is_acid) obj->oeroded2++;
		    else obj->oeroded++;
		}
	}
}

#endif /* OVLB */
#ifdef OVL1

int
could_seduce(magr,mdef,mattk)
struct monst *magr, *mdef;
struct attack *mattk;
/* returns 0 if seduction impossible,
 *	   1 if fine,
 *	   2 if wrong gender for nymph */
{
	register struct permonst *pagr;
	boolean agrinvis, defperc;
	xchar genagr, gendef;

	if (is_animal(magr->data)) return (0);
	if(magr == &youmonst) {
		pagr = youmonst.data;
		agrinvis = (Invis != 0);
		genagr = poly_gender();
	} else {
		pagr = magr->data;
		agrinvis = magr->minvis;
		genagr = gender(magr);
	}
	if(mdef == &youmonst) {
		defperc = (See_invisible != 0);
		gendef = poly_gender();
	} else {
		defperc = perceives(mdef->data);
		gendef = gender(mdef);
	}

	if(agrinvis && !defperc
#ifdef SEDUCE
		&& mattk && mattk->adtyp != AD_SSEX
#endif
		)
		return 0;

/*	if(pagr->mlet != S_NYMPH
		&& ((pagr != &mons[PM_INCUBUS] && pagr != &mons[PM_SUCCUBUS])
#ifdef SEDUCE
		    || (mattk && mattk->adtyp != AD_SSEX)
#endif
		   ))
		return 0; */

	if (mattk && mattk->adtyp != AD_SSEX && mattk->adtyp != AD_SEDU && mattk->adtyp != AD_SITM && pagr->mlet != S_NYMPH
&& pagr != &mons[PM_INCUBUS] && pagr != &mons[PM_SUCCUBUS]) return 0;
	
	if(genagr == 1 - gendef)
		return 1;
	else
		return (pagr->mlet == S_NYMPH) ? 2 : 0;
}

int
could_seduceX(magr,mdef,mattk)
struct monst *magr, *mdef;
struct attack *mattk;
/* returns 0 if seduction impossible,
 *	   1 if fine,
 *	   2 if wrong gender for nymph */
{
	register struct permonst *pagr;
	boolean agrinvis, defperc;
	xchar genagr, gendef;

	if (is_animal(magr->data)) return (0);
	if(magr == &youmonst) {
		pagr = youmonst.data;
		agrinvis = (Invis != 0);
		genagr = poly_gender();
	} else {
		pagr = magr->data;
		agrinvis = magr->minvis;
		genagr = gender(magr);
	}
	if(mdef == &youmonst) {
		defperc = (See_invisible != 0);
		gendef = poly_gender();
	} else {
		defperc = perceives(mdef->data);
		gendef = gender(mdef);
	}

	if(agrinvis && !defperc
#ifdef SEDUCE
		&& mattk && mattk->adtyp != AD_SSEX
#endif
		)
		return 0;

/*	if(pagr->mlet != S_NYMPH
		&& ((pagr != &mons[PM_INCUBUS] && pagr != &mons[PM_SUCCUBUS])
#ifdef SEDUCE
		    || (mattk && mattk->adtyp != AD_SSEX)
#endif
		   ))
		return 0; */

	/*if (mattk && mattk->adtyp != AD_SSEX && mattk->adtyp != AD_SEDU && mattk->adtyp != AD_SITM && pagr->mlet != S_NYMPH
&& pagr != &mons[PM_INCUBUS] && pagr != &mons[PM_SUCCUBUS]) return 0;*/
	
	if(genagr == 1 - gendef)
		return 1;
	else
		return (pagr->mlet == S_NYMPH) ? 2 : 0;
}

#endif /* OVL1 */
#ifdef OVLB

#ifdef SEDUCE
/* Returns 1 if monster teleported */
int
doseduce(mon)
register struct monst *mon;
{
	int monsterlev;
	register struct obj *ring, *nring;
	boolean fem = /*(mon->data == &mons[PM_SUCCUBUS])*/ (mon->female); /* otherwise incubus */
	char qbuf[QBUFSZ];

	if (mon->mcan || mon->mspec_used) {
		pline("%s acts as though %s has got a %sheadache.",
		      Monnam(mon), mhe(mon),
		      mon->mcan ? "severe " : "");
		return 0;
	}

	if (unconscious()) {
		pline("%s seems dismayed at your lack of response.",
		      Monnam(mon));
		return 0;
	}

	if (Blind) pline("It caresses you...");
	else You_feel("very attracted to %s.", mon_nam(mon));

	for(ring = invent; ring; ring = nring) {
	    nring = ring->nobj;
	    if (ring->otyp != RIN_ADORNMENT) continue;
	    if (fem) {
		if (rn2(20) < ACURR(A_CHA)) {
		    Sprintf(qbuf, "\"That %s looks pretty.  May I have it?\"",
			safe_qbuf("",sizeof("\"That  looks pretty.  May I have it?\""),
			xname(ring), simple_typename(ring->otyp), "ring"));
		    makeknown(RIN_ADORNMENT);
		    if (yn(qbuf) == 'n') continue;
		} else pline("%s decides she'd like your %s, and takes it.",
			Blind ? "She" : Monnam(mon), xname(ring));
		makeknown(RIN_ADORNMENT);
		if (ring==uleft || ring==uright) Ring_gone(ring);
		if (ring==uwep) setuwep((struct obj *)0, FALSE);
		if (ring==uswapwep) setuswapwep((struct obj *)0, FALSE);
		if (ring==uquiver) setuqwep((struct obj *)0);
		freeinv(ring);
		(void) mpickobj(mon,ring);
	    } else {
		char buf[BUFSZ];

		if (uleft && uright && uleft->otyp == RIN_ADORNMENT
				&& uright->otyp==RIN_ADORNMENT)
			break;
		if (ring==uleft || ring==uright) continue;
		if (rn2(20) < ACURR(A_CHA)) {
		    Sprintf(qbuf,"\"That %s looks pretty.  Would you wear it for me?\"",
			safe_qbuf("",
			    sizeof("\"That  looks pretty.  Would you wear it for me?\""),
			    xname(ring), simple_typename(ring->otyp), "ring"));
		    makeknown(RIN_ADORNMENT);
		    if (yn(qbuf) == 'n') continue;
		} else {
		    pline("%s decides you'd look prettier wearing your %s,",
			Blind ? "He" : Monnam(mon), xname(ring));
		    pline("and puts it on your finger.");
		}
		makeknown(RIN_ADORNMENT);
		if (!uright) {
		    pline("%s puts %s on your right %s.",
			Blind ? "He" : Monnam(mon), the(xname(ring)), body_part(HAND));
		    setworn(ring, RIGHT_RING);
		} else if (!uleft) {
		    pline("%s puts %s on your left %s.",
			Blind ? "He" : Monnam(mon), the(xname(ring)), body_part(HAND));
		    setworn(ring, LEFT_RING);
		} else if (uright && uright->otyp != RIN_ADORNMENT) {
		    Strcpy(buf, xname(uright));
		    pline("%s replaces your %s with your %s.",
			Blind ? "He" : Monnam(mon), buf, xname(ring));
		    Ring_gone(uright);
		    setworn(ring, RIGHT_RING);
		} else if (uleft && uleft->otyp != RIN_ADORNMENT) {
		    Strcpy(buf, xname(uleft));
		    pline("%s replaces your %s with your %s.",
			Blind ? "He" : Monnam(mon), buf, xname(ring));
		    Ring_gone(uleft);
		    setworn(ring, LEFT_RING);
		} else impossible("ring replacement");
		Ring_on(ring);
		prinv((char *)0, ring, 0L);
	    }
	}

	if (!uarmc && !uarmf && !uarmg && !uarms && !uarmh
#ifdef TOURIST
								&& !uarmu
#endif
									)
		pline("%s murmurs sweet nothings into your ear.",
			Blind ? (fem ? "She" : "He") : Monnam(mon));
	else
		pline("%s murmurs in your ear, while helping you undress.",
			Blind ? (fem ? "She" : "He") : Monnam(mon));
	mayberem(uarmc, cloak_simple_name(uarmc));
	if(!uarmc)
		mayberem(uarm, "suit");
	mayberem(uarmf, "boots");
	if(!uwep || !welded(uwep))
		mayberem(uarmg, "gloves");
	/* 
	 * STEPHEN WHITE'S NEW CODE
	 *
	 * This will cause a game crash should the if statment be removed.
	 * It will try to de-referance a pointer that doesn't exist should 
	 * the player not have a shield
	 */

	if (uarms) mayberem(uarms, "shield");
	mayberem(uarmh, "helmet");
#ifdef TOURIST
	if(!uarmc && !uarm)
		mayberem(uarmu, "shirt");
#endif

	if (uarm || uarmc) {
		verbalize("You're such a %s; I wish...",
				flags.female ? "sweet lady" : "nice guy");
		if (!tele_restrict(mon)) (void) rloc(mon, FALSE);
		return 1;
	}
	if (u.ualign.type == A_CHAOTIC)
		adjalign(1);

	/* yay graphical descriptions! --Amy */
	if (!flags.female) pline("%s starts to gently pull down your pants with her soft, fleecy hands...", Monnam(mon));
	else pline("%s softly caresses your fleecy bra, and gently pulls it off to reveal your breasts...", Monnam(mon));

	if (rnd(ACURR(A_CHA)) < 3) { /* random chance of being betrayed by your love interest... */

	monsterlev = ((mon->m_lev) + 1);
	if (monsterlev <= 0) monsterlev = 1;

	if (!flags.female) { pline("But %s suddenly rams her sexy knees right into your nuts! OUCH!", Monnam(mon));
				losehp(d(3,monsterlev), "treacherous lady", KILLED_BY_AN);
			}
	if (flags.female) { pline("But all of a sudden, %s clenches his burly hands to a fist and violently punches your breasts! AIIIEEEEEGGGGGHHHHH!", Monnam(mon));
				losehp(d(3,monsterlev), "filthy traitor", KILLED_BY_AN);
			}

	return 1;
	}

	/* by this point you have discovered mon's identity, blind or not... */
	pline("Time stands still while you and %s lie in each other's arms...",
		noit_mon_nam(mon));
	/* Well,  IT happened ... */
	u.uconduct.celibacy++;
	
	if (rn2(135) > ACURR(A_CHA) + ACURR(A_INT)) /*much higher chance of negative outcome now --Amy */ {
		/* Don't bother with mspec_used here... it didn't get tired! */
		pline("%s seems to have enjoyed it more than you...",
			noit_Monnam(mon));
		switch (rn2(5)) {
			case 0: You_feel("drained of energy.");
				u.uen = 0;
				u.uenmax -= rnd(Half_physical_damage ? 5 : 10);
			        exercise(A_CON, FALSE);
				if (u.uenmax < 0) u.uenmax = 0;
				break;
			case 1: You("are down in the dumps.");
				(void) adjattrib(A_CON, -1, TRUE);
			        exercise(A_CON, FALSE);
				flags.botl = 1;
				break;
			case 2: Your("senses are dulled.");
				(void) adjattrib(A_WIS, -1, TRUE);
			        exercise(A_WIS, FALSE);
				flags.botl = 1;
				break;
			case 3:
				if (!Drain_resistance || !rn2(20) ) {
				    You_feel("out of shape.");
				    losexp("overexertion", FALSE);
				} else {
				    You("have a curious feeling...");
				}
				break;
			case 4: {
				int tmp;
				You_feel("exhausted.");
			        exercise(A_STR, FALSE);
				tmp = rn1(10, 6);
				if(Half_physical_damage && rn2(2) ) tmp = (tmp+1) / 2;
				losehp(tmp, "exhaustion", KILLED_BY);
				break;
			}
		}
	} else {
		mon->mspec_used = rnd(100); /* monster is worn out */
		You("seem to have enjoyed it more than %s...",
		    noit_mon_nam(mon));
		switch (rn2(5)) {
		case 0: You_feel("raised to your full potential.");
			exercise(A_CON, TRUE);
			u.uen = (u.uenmax += rnd(5));
			break;
		case 1: You_feel("good enough to do it again.");
			(void) adjattrib(A_CON, 1, TRUE);
			exercise(A_CON, TRUE);
			flags.botl = 1;
			break;
		case 2: You("will always remember %s...", noit_mon_nam(mon));
			(void) adjattrib(A_WIS, 1, TRUE);
			exercise(A_WIS, TRUE);
			flags.botl = 1;
			break;
		case 3: pline("That was a very educational experience.");
			pluslvl(FALSE);
			exercise(A_WIS, TRUE);
			break;
		case 4: You_feel("restored to health!");
			u.uhp = u.uhpmax;
			if (Upolyd) u.mh = u.mhmax;
			exercise(A_STR, TRUE);
			flags.botl = 1;
			break;
		}
	}

	if (mon->mtame) /* don't charge */ ;
	else if (rn2(20) < ACURR(A_CHA)) {
		pline("%s demands that you pay %s, but you refuse...",
			noit_Monnam(mon),
			Blind ? (fem ? "her" : "him") : mhim(mon));
	} else if (u.umonnum == PM_LEPRECHAUN)
		pline("%s tries to take your money, but fails...",
				noit_Monnam(mon));
	else {
#ifndef GOLDOBJ
		long cost;

		if (u.ugold > (long)LARGEST_INT - 10L)
			cost = (long) rnd(LARGEST_INT) + 500L;
		else
			cost = (long) rnd((int)u.ugold + 10) + 500L;
		if (mon->mpeaceful) {
			cost /= 5L;
			if (!cost) cost = 1L;
		}
		if (cost > u.ugold) cost = u.ugold;
		if (Role_if(PM_LADIESMAN) && rn2(5)) cost = 0;
		if (!cost) verbalize("It's on the house!");
		else {
		    pline("%s takes %ld %s for services rendered!",
			    noit_Monnam(mon), cost, currency(cost));
		    u.ugold -= cost;
		    mon->mgold += cost;
		    flags.botl = 1;
		}
#else
		long cost;
                long umoney = money_cnt(invent);

		if (umoney > (long)LARGEST_INT - 10L)
			cost = (long) rnd(LARGEST_INT) + 500L;
		else
			cost = (long) rnd((int)umoney + 10) + 500L;
		if (mon->mpeaceful) {
			cost /= 5L;
			if (!cost) cost = 1L;
		}
		if (cost > umoney) cost = umoney;
		if (!cost) verbalize("It's on the house!");
		else { 
		    pline("%s takes %ld %s for services rendered!",
			    noit_Monnam(mon), cost, currency(cost));
                    money2mon(mon, cost);
		    flags.botl = 1;
		}
#endif
	}

	if (!rn2(50)) {

	/* Yes, real-life pregnancy doesn't work like this. But I want to avoid having to make complicated functions,
	   so the player will just get an egg that immediately hatches and may be tame. --Amy */

		struct obj *uegg;

		if (flags.female) { pline("Uh-oh - you're pregnant!"); verbalize("Be a good mother, sweetheart!");
		}
		else { pline("Oh! %s is pregnant!",noit_Monnam(mon)); verbalize("Please take good care of my baby, %s!",plname);
		}

		uegg = mksobj(EGG, FALSE, FALSE);
		uegg->spe = (flags.female ? 1 : 0);
		uegg->quan = 1;
		uegg->owt = weight(uegg);
		if (!rn2(2)) uegg->corpsenm = mon->mnum;
		else if (Upolyd) uegg->corpsenm = u.umonnum;
		else if (urole.femalenum != NON_PM && !rn2(2)) uegg->corpsenm = urole.femalenum;
		else uegg->corpsenm = urole.malenum;
		uegg->known = uegg->dknown = 1;
		attach_egg_hatch_timeout(uegg);
		(void) start_timer(1, TIMER_OBJECT, HATCH_EGG, (genericptr_t)uegg);
		pickup_object(uegg, 1, FALSE);

	}

	if (!rn2(Role_if(PM_LADIESMAN) ? 100 : 10)) mon->mcan = 1; /* monster is worn out; chance is much higher now --Amy */
	if (!tele_restrict(mon)) (void) rloc(mon, FALSE);
	return 1;
}

STATIC_OVL void
mayberem(obj, str)
register struct obj *obj;
const char *str;
{
	char qbuf[QBUFSZ];

	if (!obj || !obj->owornmask) return;

	if (rn2(120) < ACURR(A_CHA)) { /*much lower chance for the player to resist --Amy*/
		Sprintf(qbuf,"\"Shall I remove your %s, %s?\"",
			str,
			(!rn2(2) ? "lover" : !rn2(2) ? "dear" : "sweetheart"));
		if (yn(qbuf) == 'n') return;
	} else {
		char hairbuf[BUFSZ];

		Sprintf(hairbuf, "let me run my fingers through your %s",
			body_part(HAIR));
		verbalize("Take off your %s; %s.", str,
			(obj == uarm)  ? "let's get a little closer" :
			(obj == uarmc || obj == uarms) ? "it's in the way" :
			(obj == uarmf) ? "let me rub your feet" :
			(obj == uarmg) ? "they're too clumsy" :
#ifdef TOURIST
			(obj == uarmu) ? "let me massage you" :
#endif
			/* obj == uarmh */
			hairbuf);
	}
	remove_worn_item(obj, TRUE);
}
#endif  /* SEDUCE */

#endif /* OVLB */

#ifdef OVL1

STATIC_OVL int
passiveum(olduasmon,mtmp,mattk)
struct permonst *olduasmon;
register struct monst *mtmp;
register struct attack *mattk;
{
	int i, tmp;

	for(i = 0; ; i++) {
	    if(i >= NATTK) return 1;
	    if (olduasmon->mattk[i].aatyp == AT_NONE ||
	    		olduasmon->mattk[i].aatyp == AT_BOOM) break;
	}
	if (olduasmon->mattk[i].damn)
	    tmp = d((int)olduasmon->mattk[i].damn,
				    (int)olduasmon->mattk[i].damd);
	else if(olduasmon->mattk[i].damd)
	    tmp = d( ((int)olduasmon->mlevel / 5)+1, (int)olduasmon->mattk[i].damd); /* Players polymorphed into blue slimes or similar stuff aren't supposed to be OP. --Amy */
	else
	    tmp = 0;

	/* These affect the enemy even if you were "killed" (rehumanized) */
	switch(olduasmon->mattk[i].adtyp) {
	    case AD_ACID:
		if (!rn2(2)) {
		    pline("%s is splashed by your acid!", Monnam(mtmp));
		    if (resists_acid(mtmp)) {
			pline("%s is not affected.", Monnam(mtmp));
			tmp = 0;
		    }
		} else tmp = 0;
		if (!rn2(30)) erode_armor(mtmp, TRUE);
		if (!rn2(6)) erode_obj(MON_WEP(mtmp), TRUE, TRUE);
		goto assess_dmg;
	    case AD_STON: /* cockatrice */
	    {
		long protector = attk_protection((int)mattk->aatyp),
		     wornitems = mtmp->misc_worn_check;

		/* wielded weapon gives same protection as gloves here */
		if (MON_WEP(mtmp) != 0) wornitems |= W_ARMG;

		if (!resists_ston(mtmp) && !rn2(4) && (protector == 0L ||
			(protector != ~0L &&
			    (wornitems & protector) != protector))) {
		    if (poly_when_stoned(mtmp->data)) {
			mon_to_stone(mtmp);
			return (1);
		    }
		    pline("%s turns to stone!", Monnam(mtmp));
		    stoned = 1;
		    xkilled(mtmp, 0);
		    if (mtmp->mhp > 0) return 1;
		    return 2;
		}
		return 1;
	    }
	    case AD_ENCH:	/* KMH -- remove enchantment (disenchanter) */
	    	if (otmp) {
	    	    (void) drain_item(otmp);
	    	    /* No message */
	    	}
	    	return (1);
	    default:
		break;
	}
	if (!Upolyd) return 1;

	/* These affect the enemy only if you are still a monster */
	if (rn2(3)) switch(youmonst.data->mattk[i].adtyp) {
	    case AD_PHYS:
	    	if (youmonst.data->mattk[i].aatyp == AT_BOOM ) {
	    	    You("explode!");
	    	    /* KMH, balance patch -- this is okay with unchanging */
	    	    if (!Race_if(PM_UNGENOMOLD)) rehumanize(); /* we don't want ungenomolds to die from being a graveler */
			else polyself(FALSE);
	    	    goto assess_dmg;
	    	}
	    	break;
	    case AD_PLYS: /* Floating eye */
		if (tmp > 127) tmp = 127;
		if (u.umonnum == PM_FLOATING_EYE) {
		    /*if (!rn2(4)) tmp = 127;*/
		    if (mtmp->mcansee && haseyes(mtmp->data) && rn2(3) &&
				(perceives(mtmp->data) || !Invis)) {
			if (Blind)
			    pline("As a blind %s, you cannot defend yourself.",
							youmonst.data->mname);
		        else {
			    if (mon_reflects(mtmp,
					    "Your gaze is reflected by %s %s."))
				return 1;
			    pline("%s is frozen by your gaze!", Monnam(mtmp));
			    mtmp->mcanmove = 0;
			    mtmp->mfrozen = tmp;
			    return 3;
			}
		    }
		} else { /* gelatinous cube */
		    pline("%s is frozen by you.", Monnam(mtmp));
		    mtmp->mcanmove = 0;
		    mtmp->mfrozen = tmp;
		    return 3;
		}
		return 1;
	    case AD_COLD: /* Brown mold or blue jelly */
		if (resists_cold(mtmp)) {
		    shieldeff(mtmp->mx, mtmp->my);
		    pline("%s is mildly chilly.", Monnam(mtmp));
		    golemeffects(mtmp, AD_COLD, tmp);
		    tmp = 0;
		    break;
		}
		pline("%s is suddenly very cold!", Monnam(mtmp));
		u.mh += tmp / 2;
		if (u.mhmax < u.mh) u.mhmax = u.mh;
		if (u.mhmax > ((youmonst.data->mlevel+1) * 8) && !rn2(25) ) /* slow down farming --Amy */
		    (void)split_mon(&youmonst, mtmp);
		break;
	    case AD_STUN: /* Yellow mold */
		if (!mtmp->mstun) {
		    mtmp->mstun = 1;
		    pline("%s %s.", Monnam(mtmp),
			  makeplural(stagger(mtmp->data, "stagger")));
		}
		tmp = 0;
		break;
	    case AD_FIRE: /* Red mold */
		if (resists_fire(mtmp)) {
		    shieldeff(mtmp->mx, mtmp->my);
		    pline("%s is mildly warm.", Monnam(mtmp));
		    golemeffects(mtmp, AD_FIRE, tmp);
		    tmp = 0;
		    break;
		}
		pline("%s is suddenly very hot!", Monnam(mtmp));
		break;
	    case AD_ELEC:
		if (resists_elec(mtmp)) {
		    shieldeff(mtmp->mx, mtmp->my);
		    pline("%s is slightly tingled.", Monnam(mtmp));
		    golemeffects(mtmp, AD_ELEC, tmp);
		    tmp = 0;
		    break;
		}
		pline("%s is jolted with your electricity!", Monnam(mtmp));
		break;
	    default: tmp = 0;
		break;
	}
	else tmp = 0;

    assess_dmg:
	if((mtmp->mhp -= tmp) <= 0) {
		pline("%s dies!", Monnam(mtmp));
		xkilled(mtmp,0);
		if (mtmp->mhp > 0) return 1;
		return 2;
	}
	return 1;
}

#endif /* OVL1 */
#ifdef OVLB

#include "edog.h"
struct monst *
cloneu()
{
	register struct monst *mon;
	int mndx = monsndx(youmonst.data);

	if (u.mh <= 1) return(struct monst *)0;
	if (mvitals[mndx].mvflags & G_EXTINCT) return(struct monst *)0;
	mon = makemon(youmonst.data, u.ux, u.uy, NO_MINVENT|MM_EDOG);
	mon = christen_monst(mon, plname);
	initedog(mon);
	mon->m_lev = youmonst.data->mlevel;
	mon->mhpmax = u.mhmax;
	mon->mhp = u.mh / 2;
	u.mh -= mon->mhp;
	flags.botl = 1;
	return(mon);
}

#endif /* OVLB */

/*mhitu.c*/
