/*	SCCS Id: @(#)monmove.c	3.4	2002/04/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "mfndpos.h"
#include "artifact.h"
#include "epri.h"

extern boolean notonhead;

#ifdef OVL0

STATIC_DCL int disturb(struct monst *);
STATIC_DCL void distfleeck(struct monst *,int *,int *,int *);
STATIC_DCL int m_arrival(struct monst *);
STATIC_DCL void watch_on_duty(struct monst *);
/* WAC for breath door busting */
static int bust_door_breath(struct monst *);

#endif /* OVL0 */
#ifdef OVLB

boolean /* TRUE : mtmp died */
mb_trapped(mtmp)
register struct monst *mtmp;
{
	if (flags.verbose) {
	    if (cansee(mtmp->mx, mtmp->my))
		pline("KABOOM!!  You see a door explode.");
	    else if (flags.soundok)
		You_hear("a distant explosion.");
	}
	wake_nearto(mtmp->mx, mtmp->my, 7*7);
	mtmp->mstun = 1;
	mtmp->mhp -= rnd(15);
	if(mtmp->mhp <= 0) {
		mondied(mtmp);
		if (mtmp->mhp > 0) /* lifesaved */
			return(FALSE);
		else
			return(TRUE);
	}
	return(FALSE);
}

#endif /* OVLB */
#ifdef OVL0

STATIC_OVL void
watch_on_duty(mtmp)
register struct monst *mtmp;
{
	int	x, y;

	if(mtmp->mpeaceful && in_town(u.ux+u.dx, u.uy+u.dy) &&
	   mtmp->mcansee && m_canseeu(mtmp) && !rn2(3)) {

		if((Role_if(PM_CONVICT) || Role_if(PM_MURDERER) || Race_if(PM_ALBAE)) && !Upolyd) {
            verbalize("%s yells: Hey!  You are the one from the wanted poster!",
             Amonnam(mtmp));
            (void) angry_guards(!(flags.soundok));
            stop_occupation();
            return;
        }
	    if(picking_lock(&x, &y) && IS_DOOR(levl[x][y].typ) &&
	       (levl[x][y].doormask & D_LOCKED)) {

		if(couldsee(mtmp->mx, mtmp->my)) {

		  pline("%s yells:", Amonnam(mtmp));
		  if(levl[x][y].looted & D_WARNED) {
			verbalize("Halt, thief!  You're under arrest!");
			(void) angry_guards(!(flags.soundok));
		  } else {
			int i;
			verbalize("Hey, stop picking that lock!");
			/* [ALI] Since marking a door as warned will have
			 * the side effect of trapping the door, it must be
			 * included in the doors[] array in order that trap
			 * detection will find it.
			 */
			for(i = doorindex - 1; i >= 0; i--)
			    if (x == doors[i].x && y == doors[i].y)
				break;
			if (i < 0)
			    i = add_door(x, y, (struct mkroom *)0);
			if (i >= 0)
			    levl[x][y].looted |= D_WARNED;
		  }
		  stop_occupation();
		}
	    } else if (is_digging()) {
		/* chewing, wand/spell of digging are checked elsewhere */
		watch_dig(mtmp, digging.pos.x, digging.pos.y, FALSE);
	    }
	}
}

#endif /* OVL0 */
#ifdef OVL1

int
dochugw(mtmp)
register struct monst *mtmp;
{
	register int x = mtmp->mx, y = mtmp->my;
	boolean already_saw_mon = !occupation ? 0 : canspotmon(mtmp);
	int rd = dochug(mtmp);
#if 0
	/* part of the original warning code which was replaced in 3.3.1 */
	register struct permonst *mdat = mtmp->data;        
	int dd;
	if(Warning && !rd && !mtmp->mpeaceful &&
			(dd = distu(mtmp->mx,mtmp->my)) < distu(x,y) &&
			dd < 100 && !canseemon(mtmp)) {
	    /* Note: this assumes we only want to warn against the monster to
	     * which the weapon does extra damage, as there is no "monster
	     * which the weapon warns against" field.
	     */
	    if (spec_ability(uwep, SPFX_WARN) && spec_dbon(uwep, mtmp, 1))
		warnlevel = 100;
	    else if ((int) (mtmp->m_lev / 6) > warnlevel)
		warnlevel = (mtmp->m_lev / 6);
	/* STEPHEN WHITE'S NEW CODE */
	} else if(Undead_warning && !rd && !mtmp->mpeaceful &&
		  (dd = distu(mtmp->mx,mtmp->my)) < distu(x,y) &&
		   dd < 100 && !canseemon(mtmp) && (is_undead(mdat) || mtmp->egotype_undead) ) {
			/* 
			 * The value of warnlevel coresponds to the 8 
			 * cardinal directions, see mon.c.
			 */
			if(((mtmp->mx - u.ux) < 0) && ((mtmp->my - u.uy) < 0))
				warnlevel = 101;
			if(((mtmp->mx - u.ux) == 0) && ((mtmp->my - u.uy) < 0))
				warnlevel = 102;
			if(((mtmp->mx - u.ux) > 0) && ((mtmp->my - u.uy) < 0))
				warnlevel = 103;
			if(((mtmp->mx - u.ux) < 0) && ((mtmp->my - u.uy) == 0))
				warnlevel = 104;
			if(((mtmp->mx - u.ux) > 0) && ((mtmp->my - u.uy) == 0))
				warnlevel = 105;
			if(((mtmp->mx - u.ux) < 0) && ((mtmp->my - u.uy) > 0))
				warnlevel = 106;
			if(((mtmp->mx - u.ux) == 0) && ((mtmp->my - u.uy) > 0))
				warnlevel = 107;
			if(((mtmp->mx - u.ux) > 0) && ((mtmp->my - u.uy) > 0))
				warnlevel = 108;
	}
#endif /* 0 */

	/* a similar check is in monster_nearby() in hack.c */
	/* check whether hero notices monster and stops current activity */
	if (occupation && !rd && !Confusion &&
	    (!mtmp->mpeaceful || Hallucination) &&
	    /* it's close enough to be a threat */
	    distu(mtmp->mx,mtmp->my) <= (BOLT_LIM+1)*(BOLT_LIM+1) &&
	    /* and either couldn't see it before, or it was too far away */
	    (!already_saw_mon || !couldsee(x,y) ||
		distu(x,y) > (BOLT_LIM+1)*(BOLT_LIM+1)) &&
	    /* can see it now, or sense it and would normally see it */
	    (canseemon(mtmp) ||
		(sensemon(mtmp) && couldsee(mtmp->mx,mtmp->my))) &&
	    mtmp->mcanmove &&
	    !noattacks(mtmp->data) && !onscary(u.ux, u.uy, mtmp))
		stop_occupation();
	return(rd);
}

#endif /* OVL1 */
#ifdef OVL2

boolean
onscary(x, y, mtmp)
int x, y;
struct monst *mtmp;
{
	int resist_percentage;
	int scresist_percentage;
	boolean mresists;
	boolean scmresists;


	if (mtmp->isshk || mtmp->isgd || mtmp->iswiz || !mtmp->mcansee ||
			mtmp->mpeaceful || (mtmp->data->mlet == S_HUMAN && (rn2(5) || mtmp->data->geno & G_UNIQ )) || (mtmp->data->mlet == S_DEMON && (rn2(3) || mtmp->data->geno & G_UNIQ )) ||
			(mtmp->data->mlet == S_NEMESE && (rn2(10) || mtmp->data->geno & G_UNIQ )) || (mtmp->data->mlet == S_ARCHFIEND && (rn2(25) || mtmp->data->geno & G_UNIQ )) || mtmp->data->mlet == S_RUBMONST ||
	    is_lminion(mtmp) || (mtmp->data->mlet == S_ANGEL && (rn2(20) || mtmp->data->geno & G_UNIQ )) || (mtmp->data->mlet == S_JELLY && (rn2(3) || mtmp->data->geno & G_UNIQ )) ||
	    mtmp->data == &mons[PM_CTHULHU] || (mtmp->data->mlet == S_LIGHT && (rn2(2) || mtmp->data->geno & G_UNIQ )) || (mtmp->data->mlet == S_FUNGUS && (rn2(10) || mtmp->data->geno & G_UNIQ )) ||
	    is_rider(mtmp->data) || is_deadlysin(mtmp->data) || (mtmp->data == &mons[PM_MINOTAUR] && (rn2(5) || mtmp->data->geno & G_UNIQ )) || (mtmp->data->msound == MS_BOSS && rn2(10) ) || (mtmp->data->msound == MS_FART_QUIET && rn2(50) ) || (mtmp->data->msound == MS_FART_NORMAL && rn2(30) ) || (mtmp->data->msound == MS_FART_LOUD && rn2(20) ) || 
		 mtmp->mnum == quest_info(MS_NEMESIS) || mtmp->mnum == PM_VLAD_THE_IMPALER)
		return(FALSE);

	if (mtmp->mhp < 2) return FALSE;

	/* the smallest monsters always respect Elbereth;
	 * more powerful things less so */
	/* also nerfed scare monster scrolls a bit */

	resist_percentage = (int)(mtmp->m_lev * 3 / 2);
	scresist_percentage = (int)(mtmp->m_lev / 2);

	mresists = rn2(100) < resist_percentage;
	scmresists = rn2(100) < resist_percentage;

	return (boolean)((sobj_at(SCR_SCARE_MONSTER, x, y) && !(Conflict && rn2(2)) && !scmresists)
			 || (sengr_at("Elbereth", x, y) && !mresists && !(Conflict && rn2(2)) && !(EngravingDoesntWork || u.uprops[ENGRAVINGBUG].extrinsic || have_engravingstone()) )
			 || (is_vampire(mtmp->data)
			     && IS_ALTAR(levl[x][y].typ)));
}

#endif /* OVL2 */
#ifdef OVL0

/* regenerate lost hit points */
void
mon_regen(mon, digest_meal)
struct monst *mon;
boolean digest_meal;
{

	int regenrate; /* A level 30 pet large cat would otherwise regenerate waaaaaay too slowly. --Amy */

	regenrate = (20 - (mon->m_lev / 3));
	if (regenrate < 6) regenrate = 6;
	if (ishaxor) regenrate /= 2;

	if (mon->mhp < mon->mhpmax && !is_golem(mon->data) &&
	    (moves % /*20*/regenrate == 0 || regenerates(mon->data) || mon->egotype_regeneration )) mon->mhp++;
	if (mon->m_en < mon->m_enmax && 
	    (moves % /*20*/regenrate == 0 || (rn2(mon->m_lev + 5) > 15))) {
	    	mon->m_en += rn1((mon->m_lev % 10 + 1),1);
	    	if (mon->m_en > mon->m_enmax) mon->m_en = mon->m_enmax;
	}

	if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER) && uimplant && uimplant->oartifact == ART_READY_FOR_A_RIDE && u.usteed && (mon == u.usteed)) {
		if (mon->mhp + 1 >= mon->mhpmax)
		      mon->mhp = mon->mhpmax;
		else mon->mhp++;
	}

	/* good riding skill gives extra regeneration to ridden monster --Amy */

	if (!(PlayerCannotUseSkills)) {

		if (P_SKILL(P_RIDING) == P_BASIC && u.usteed && (mon == u.usteed) && !rn2(10) ) {
			if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER) && uimplant && uimplant->oartifact == ART_READY_FOR_A_RIDE) {
				if (mon->mhp + 1 >= mon->mhpmax)
				      mon->mhp = mon->mhpmax;
				else mon->mhp++;
			}

		}
		if (P_SKILL(P_RIDING) == P_SKILLED && u.usteed && (mon == u.usteed) && !rn2(10) ) {
			if (mon->mhp + 1 >= mon->mhpmax)
			      mon->mhp = mon->mhpmax;
			else mon->mhp++;
			if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER) && uimplant && uimplant->oartifact == ART_READY_FOR_A_RIDE) {
				if (mon->mhp + 2 >= mon->mhpmax)
				      mon->mhp = mon->mhpmax;
				else mon->mhp++;
			}

		}
		if (P_SKILL(P_RIDING) == P_EXPERT && u.usteed && (mon == u.usteed) && !rn2(5) ) {
			if (mon->mhp + 1 >= mon->mhpmax)
			      mon->mhp = mon->mhpmax;
			else mon->mhp++;
			if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER) && uimplant && uimplant->oartifact == ART_READY_FOR_A_RIDE) {
				if (mon->mhp + 3 >= mon->mhpmax)
				      mon->mhp = mon->mhpmax;
				else mon->mhp++;
			}
		}
		if (P_SKILL(P_RIDING) == P_MASTER && u.usteed && (mon == u.usteed) && !rn2(3) ) {
			if (mon->mhp + 1 >= mon->mhpmax)
			      mon->mhp = mon->mhpmax;
			else mon->mhp++;
			if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER) && uimplant && uimplant->oartifact == ART_READY_FOR_A_RIDE) {
				if (mon->mhp + 4 >= mon->mhpmax)
				      mon->mhp = mon->mhpmax;
				else mon->mhp++;
			}
		}
		if (P_SKILL(P_RIDING) == P_GRAND_MASTER && u.usteed && (mon == u.usteed) ) {
			if (mon->mhp + 1 >= mon->mhpmax)
			      mon->mhp = mon->mhpmax;
			else mon->mhp++;
			if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER) && uimplant && uimplant->oartifact == ART_READY_FOR_A_RIDE) {
				if (mon->mhp + 5 >= mon->mhpmax)
				      mon->mhp = mon->mhpmax;
				else mon->mhp++;
			}
		}
		if (P_SKILL(P_RIDING) == P_SUPREME_MASTER && u.usteed && (mon == u.usteed) ) {
			if (mon->mhp + 1 >= mon->mhpmax)
			      mon->mhp = mon->mhpmax;
			else mon->mhp++;
			if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER) && uimplant && uimplant->oartifact == ART_READY_FOR_A_RIDE) {
				if (mon->mhp + 6 >= mon->mhpmax)
				      mon->mhp = mon->mhpmax;
				else mon->mhp++;
			}
		}

		if (mon->mtame) {

			if ((rnd(30 - ACURR(A_CHA))) < 4) {
				if (mon->mhp + 1 >= mon->mhpmax)
				      mon->mhp = mon->mhpmax;
				else mon->mhp++;
			}

			switch (P_SKILL(P_PETKEEPING)) {
				default: break;
				case P_BASIC: 
					if (!rn2(6)) {
						if (mon->mhp + 1 >= mon->mhpmax)
						      mon->mhp = mon->mhpmax;
						else mon->mhp++;
					}
					break;
				case P_SKILLED:
					if (!rn2(5)) {
						if (mon->mhp + 1 >= mon->mhpmax)
						      mon->mhp = mon->mhpmax;
						else mon->mhp++;
					}
					break;
				case P_EXPERT:
					if (!rn2(4)) {
						if (mon->mhp + 1 >= mon->mhpmax)
						      mon->mhp = mon->mhpmax;
						else mon->mhp++;
					}
					break;
				case P_MASTER:
					if (!rn2(3)) {
						if (mon->mhp + 1 >= mon->mhpmax)
						      mon->mhp = mon->mhpmax;
						else mon->mhp++;
					}
					break;
				case P_GRAND_MASTER:
					if (!rn2(2)) {
						if (mon->mhp + 1 >= mon->mhpmax)
						      mon->mhp = mon->mhpmax;
						else mon->mhp++;
					}
					break;
				case P_SUPREME_MASTER:
					if (mon->mhp + 1 >= mon->mhpmax)
					      mon->mhp = mon->mhpmax;
					else mon->mhp++;
					break;
			}

		}

	}

	if (mon->mspec_used) mon->mspec_used--;
	if (digest_meal) {
	    if (mon->meating) mon->meating--;
	}
}

/*
 * Possibly awaken the given monster.  Return a 1 if the monster has been
 * jolted awake.
 */
STATIC_OVL int
disturb(mtmp)
	register struct monst *mtmp;
{
	/*
	 * + Ettins are hard to surprise.
	 * + Nymphs, jabberwocks, and leprechauns do not easily wake up.
	 *
	 * Wake up if:
	 *	in direct LOS						AND
	 *	within 10 squares					AND
	 *	not stealthy or (mon is an ettin and 9/10)		AND
	 *	(mon is not a nymph, jabberwock, or leprechaun) or 1/50	AND
	 *	Aggravate or mon is (dog or human) or
	 *	    (1/7 and mon is not mimicing furniture or object)
	 */
	if(couldsee(mtmp->mx,mtmp->my) &&
		distu(mtmp->mx,mtmp->my) <= 100 &&
		(!Stealth || (Aggravate_monster && !rn2(3) ) || (mtmp->data == &mons[PM_ETTIN] && rn2(10))) &&
		(!(mtmp->data->mlet == S_NYMPH
			|| mtmp->data == &mons[PM_JABBERWOCK]
			|| mtmp->data == &mons[PM_VORPAL_JABBERWOCK]
#if 0	/* DEFERRED */
			|| mtmp->data == &mons[PM_VORPAL_JABBERWOCK]
#endif
			|| mtmp->data->mlet == S_LEPRECHAUN) || !rn2(50)) &&
		(Aggravate_monster
			|| (mtmp->data->mlet == S_DOG ||
				mtmp->data->mlet == S_HUMAN)
			|| (!rn2(7) && mtmp->m_ap_type != M_AP_FURNITURE &&
				mtmp->m_ap_type != M_AP_OBJECT) )) {
		mtmp->msleeping = 0;
		return(1);
	}
	return(0);
}

/* monster begins fleeing for the specified time, 0 means untimed flee
 * if first, only adds fleetime if monster isn't already fleeing
 * if fleemsg, prints a message about new flight, otherwise, caller should */
void
monflee(mtmp, fleetime, first, fleemsg)
struct monst *mtmp;
int fleetime;
boolean first;
boolean fleemsg;
{
	if (u.ustuck == mtmp) {
	    if (u.uswallow)
		expels(mtmp, mtmp->data, TRUE);
	    else if (!sticks(youmonst.data)) {
		unstuck(mtmp);	/* monster lets go when fleeing */
		You("get released!");
	    }
	}

	if (!first || !mtmp->mflee) {
	    /* don't lose untimed scare */
	    if (!fleetime)
		mtmp->mfleetim = 0;
	    else if (!mtmp->mflee || mtmp->mfleetim) {
		fleetime += mtmp->mfleetim;
		/* ensure monster flees long enough to visibly stop fighting */
		if (fleetime == 1) fleetime++;
		mtmp->mfleetim = min(fleetime, 127);
	    }
	    if (!mtmp->mflee && fleemsg && canseemon(mtmp) && !mtmp->mfrozen) {
		if (rn2(3)) {
		  pline("%s turns to flee!", (Monnam(mtmp)));
		  mtmp->mflee = 1;
		}
		else {
		  pline("%s is startled for a moment.", (Monnam(mtmp)));
		}
	    } else if (rn2(3)) mtmp->mflee = 1;

		/* pline("%s turns to flee!", (Monnam(mtmp))); */
	    /*mtmp->mflee = 1;*/
	}
}

STATIC_OVL void
distfleeck(mtmp,inrange,nearby,scared)
register struct monst *mtmp;
int *inrange, *nearby, *scared;
{
	int seescaryx, seescaryy;

	*inrange = (dist2(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) <=
							(BOLT_LIM * BOLT_LIM));
	*nearby = *inrange && monnear(mtmp, mtmp->mux, mtmp->muy);

	/* Note: if your image is displaced, the monster sees the Elbereth
	 * at your displaced position, thus never attacking your displaced
	 * position, but possibly attacking you by accident.  If you are
	 * invisible, it sees the Elbereth at your real position, thus never
	 * running into you by accident but possibly attacking the spot
	 * where it guesses you are.
	 */
	if (!mtmp->mcansee || (Invis && !perceives(mtmp->data))) {
		seescaryx = mtmp->mux;
		seescaryy = mtmp->muy;
	} else {
		seescaryx = u.ux;
		seescaryy = u.uy;
	}

	/* "super bonus evil patch idea" by jonadab - monsters with only 1HP ignore Elbereth and similar scary stuff */
	*scared = ( (mtmp->mhp > 1) && *nearby && (onscary(seescaryx, seescaryy, mtmp) && (rnd(20) > 1) ||
			       (!mtmp->mpeaceful && (rnd(5) > 2) &&
				    in_your_sanctuary(mtmp, 0, 0))));
	/* note by Amy: I always felt permanent Elbereths were waaaaaaaay too strong.
	It's much more interesting if Elbereth has a chance to fail, too.
	After all, where's the challenge in burning an Elbereth, then whacking at soldier ants for two hours straight? */

	if(*scared) {
		if (rn2(7))
		    monflee(mtmp, rnd(10), TRUE, TRUE);
		else
		    monflee(mtmp, rnd(100), TRUE, TRUE);
	}

}

/* perform a special one-time action for a monster; returns -1 if nothing
   special happened, 0 if monster uses up its turn, 1 if monster is killed */
STATIC_OVL int
m_arrival(mon)
struct monst *mon;
{
	mon->mstrategy &= ~STRAT_ARRIVE;	/* always reset */

	return -1;
}

static NEARDATA const char practical[] = { 
	WEAPON_CLASS, ARMOR_CLASS, GEM_CLASS, FOOD_CLASS, 0 };
static NEARDATA const char magical[] = {
	AMULET_CLASS, IMPLANT_CLASS, POTION_CLASS, SCROLL_CLASS, WAND_CLASS, RING_CLASS,
	SPBOOK_CLASS, 0 };
static NEARDATA const char indigestion[] = { BALL_CLASS, ROCK_CLASS, 0 };
static NEARDATA const char boulder_class[] = { ROCK_CLASS, 0 };
static NEARDATA const char gem_class[] = { GEM_CLASS, 0 };


/* returns 1 if monster died moving, 0 otherwise */
/* The whole dochugw/m_move/distfleeck/mfndpos section is serious spaghetti
 * code. --KAA
 */
int
dochug(mtmp)
register struct monst *mtmp;
{
	register struct permonst *mdat, *mdat2;
	register int tmp=0;

	int armpro, armprolimit;

	int inrange, nearby, scared;
#ifdef GOLDOBJ
        struct obj *ygold = 0, *lepgold = 0;
#endif

/*	Pre-movement adjustments	*/

	boolean likegold=0, likegems=0, likeobjs=0, likemagic=0;
	boolean uses_items=0, likerock=0;

	mdat = mtmp->data;

	int fartdistance = 1;
	if (FemaleTrapKatharina) fartdistance = 15;
	if (FemaleTrapKatharina && mtmp->crapbonus) fartdistance = 100;

	if (TimeStopped) return 0;	/* time stop completely prevents monsters from doing anything --Amy */
	if (u.stasistime) return 0;	/* stasis does the same --Amy */

	if (mtmp->mstrategy & STRAT_ARRIVE) {
	    int res = m_arrival(mtmp);
	    if (res >= 0) return res;
	}

	/* check for waitmask status change */
	if ((mtmp->mstrategy & STRAT_WAITFORU) &&
		(m_canseeu(mtmp) || mtmp->mhp < mtmp->mhpmax))
	    mtmp->mstrategy &= ~STRAT_WAITFORU;

	/* update quest status flags */
	quest_stat_check(mtmp);

	if (!mtmp->mcanmove || (mtmp->mstrategy & STRAT_WAITMASK)) {
	    if (Hallucination) newsym(mtmp->mx,mtmp->my);
	    if (mtmp->mcanmove && (mtmp->mstrategy & STRAT_CLOSE) &&
	       !mtmp->msleeping && monnear(mtmp, u.ux, u.uy))
		quest_talk(mtmp);	/* give the leaders a chance to speak */
	    return(0);	/* other frozen monsters can't do anything */
	}

	/* there is a chance we will wake it */
	if (mtmp->msleeping && !disturb(mtmp)) {
		if (Hallucination) newsym(mtmp->mx,mtmp->my);
		return(0);
	}

      if(OBJ_AT(mtmp->mx, mtmp->my) && !mtmp->mpeaceful) {

		register int pctload = (curr_mon_load(mtmp) * 100) / max_mon_load(mtmp);

		likegold = (likes_gold(mtmp->data) && pctload < 95);
		likegems = (likes_gems(mtmp->data) && pctload < 85);
		uses_items = (pctload < 75);
		likeobjs = (likes_objs(mtmp->data) && pctload < 75);
		likemagic = (likes_magic(mtmp->data) && pctload < 85);
		likerock = (throws_rocks(mtmp->data) && pctload < 50 && !In_sokoban(&u.uz));

		if(g_at(mtmp->mx,mtmp->my) && likegold) mpickgold(mtmp);

		if(!*in_rooms(mtmp->mx, mtmp->my, SHOPBASE) || !rn2(25)) {

		    if(likeobjs) mpickstuff(mtmp, practical);
		    if(likemagic) mpickstuff(mtmp, magical);
		    if(likerock) mpickstuff(mtmp, boulder_class);
		    if(likegems) mpickstuff(mtmp, gem_class);
		    if(uses_items) mpickstuff(mtmp, (char *)0);
		}

		if(mtmp->minvis) {
		    newsym(mtmp->mx, mtmp->my);
		    if (mtmp->wormno) see_wsegs(mtmp);
		}
      }

	if (mdat == &mons[PM_LAG_MONSTER] || mdat == &mons[PM_LAG_DEFENSE_TOWER]) { /* laaaaaaaaaag! :D --Amy */
		int lagamount = rno(10);
		while (lagamount > 0) {
			delay_output();
			lagamount--;
		}
	}

	if (mdat == &mons[PM_BUNDLE_MONSTER] && !rn2(10) && (distu(mtmp->mx,mtmp->my) <= BOLT_LIM*BOLT_LIM) ) {
		You_hear("'bundlebundlebundle!!'");
	}
	if (mdat == &mons[PM_CUDDLE_MONSTER] && !rn2(10) && (distu(mtmp->mx,mtmp->my) <= BOLT_LIM*BOLT_LIM) ) {
		You_hear("'cuddlecuddlecuddle!!'");
	}
	if (mdat == &mons[PM_FLEECE_MONSTER] && !rn2(10) && (distu(mtmp->mx,mtmp->my) <= BOLT_LIM*BOLT_LIM) ) {
		You_hear("'fleecelfleecelfleecel!!'");
	}

	if (mdat == &mons[PM_UBERNAZGUL] && rn2(2)) { /* sporkhack nazgul --Amy */
		if (canseemon(mtmp)) {
			pline("%s screams!",Monnam(mtmp));
		} else {
			You_hear("a horrific scream!");
		}
		if (u.usleep) { unmul("You are shocked awake!"); }
		Your("mind reels from the noise!");
		make_stunned(HStun + rnd(20),FALSE);

	}

	if ((WakeupCallBug || u.uprops[WAKEUP_CALL_BUG].extrinsic || have_wakeupcallstone()) && mtmp->mpeaceful && !mtmp->mtame && !rn2(10000)) {
		wakeup(mtmp);
	}

	/* not frozen or sleeping: wipe out texts written in the dust */
	wipe_engr_at(mtmp->mx, mtmp->my, 1);

	/* confused monsters get unconfused with small probability */
	if (mtmp->mconf && !rn2(50)) mtmp->mconf = 0;

	/* stunned monsters get un-stunned with larger probability */
	if (mtmp->mstun && !rn2(10)) mtmp->mstun = 0;

	/* monsters whose butts were bashed by you will slowly recover --Amy */
	if (mtmp->butthurt && !rn2(mdat->msound == MS_FART_QUIET ? 5 : mdat->msound == MS_FART_NORMAL ? 20 : 50) ) mtmp->butthurt -= 1;

	/* some monsters teleport */
	if (mtmp->mflee && !rn2(40) && can_teleport(mdat) && !mtmp->iswiz &&
	    !level.flags.noteleport) {
		(void) rloc(mtmp, FALSE);
		return(0);
	}
	if (mdat->msound == MS_SHRIEK && !um_dist(mtmp->mx, mtmp->my, 1))
	    m_respond(mtmp);
	if (mdat->msound == MS_FART_QUIET && !rn2(10 + mtmp->butthurt - mtmp->fartbonus) && !um_dist(mtmp->mx, mtmp->my, fartdistance) && !mtmp->mpeaceful) {
	    m_respond(mtmp);
		while (FemaleTrapElena && !rn2(3)) {
			pline("You long for more!");
			m_respond(mtmp);
		}
	}
	if (mdat->msound == MS_FART_NORMAL && !rn2(10 + mtmp->butthurt - mtmp->fartbonus) && !um_dist(mtmp->mx, mtmp->my, fartdistance) && !mtmp->mpeaceful) {
	    m_respond(mtmp);
		while (FemaleTrapElena && !rn2(3)) {
			pline("You long for more!");
			m_respond(mtmp);
		}
	}
	if (mdat->msound == MS_FART_LOUD && !rn2(10 + mtmp->butthurt - mtmp->fartbonus) && !um_dist(mtmp->mx, mtmp->my, fartdistance) && !mtmp->mpeaceful) {
	    m_respond(mtmp);
		while (FemaleTrapElena && !rn2(3)) {
			pline("You long for more!");
			m_respond(mtmp);
		}
	}

	if (mdat->msound == MS_FART_QUIET && mtmp->crapbonus && (rn2(2000) < mtmp->crapbonus) && !um_dist(mtmp->mx, mtmp->my, 1) && !mtmp->mpeaceful)
	{
		pline("Using %s %s butt, %s produces tender noises and craps right in your %s.", mhis(mtmp), mtmp->female ? "sexy" : "ugly", mon_nam(mtmp), body_part(FACE) );
		u.ucreamed += rnd(20);
		make_blinded(Blinded + (long)u.ucreamed, FALSE);

	}
	if (mdat->msound == MS_FART_NORMAL && mtmp->crapbonus && (rn2(1000) < mtmp->crapbonus) && !um_dist(mtmp->mx, mtmp->my, 1) && !mtmp->mpeaceful)
	{
		pline("Using %s %s butt, %s produces beautiful noises and craps right in your %s.", mhis(mtmp), mtmp->female ? "sexy" : "ugly", mon_nam(mtmp), body_part(FACE) );
		u.ucreamed += rnd(50);
		make_blinded(Blinded + (long)u.ucreamed, FALSE);

	}
	if (mdat->msound == MS_FART_LOUD && mtmp->crapbonus && (rn2(400) < mtmp->crapbonus) && !um_dist(mtmp->mx, mtmp->my, 1) && !mtmp->mpeaceful)
	{
		pline("Using %s %s butt, %s produces disgusting noises and craps right in your %s.", mhis(mtmp), mtmp->female ? "sexy" : "ugly", mon_nam(mtmp), body_part(FACE) );
		u.ucreamed += rnd(100);
		make_blinded(Blinded + (long)u.ucreamed, FALSE);

	}

	if (!(mdat->msound == MS_FART_LOUD || mdat->msound == MS_FART_NORMAL || mdat->msound == MS_FART_QUIET) && mtmp->egotype_farter && !rn2(10 + mtmp->butthurt) && !um_dist(mtmp->mx, mtmp->my, fartdistance) && !mtmp->mpeaceful) {
	    m_respond(mtmp);
		while (FemaleTrapElena && !rn2(3)) {
			pline("You long for more!");
			m_respond(mtmp);
		}
	}
	if ( (mdat->msound == MS_SOUND || mtmp->egotype_sounder) && !rn2(20) && !um_dist(mtmp->mx, mtmp->my, 1) && !mtmp->mpeaceful)
	    m_respond(mtmp);
	if (mdat == &mons[PM_MEDUSA] && couldsee(mtmp->mx, mtmp->my))
	    m_respond(mtmp);
	if (mtmp->mhp <= 0) return(1); /* m_respond gaze can kill medusa */

	/* fleeing monsters might regain courage */
	if (mtmp->mflee && !mtmp->mfleetim
	   && mtmp->mhp == mtmp->mhpmax && !rn2(25)) mtmp->mflee = 0;

	set_apparxy(mtmp);
	/* Must be done after you move and before the monster does.  The
	 * set_apparxy() call in m_move() doesn't suffice since the variables
	 * inrange, etc. all depend on stuff set by set_apparxy().
	 */

	/* Monsters that want to acquire things */
	/* may teleport, so do it before inrange is set */
	if( (is_covetous(mdat) || mtmp->egotype_covetous) && (!rn2(10) || (CovetousnessBug || u.uprops[COVETOUSNESS_BUG].extrinsic || have_covetousstone() ) ) ) (void) tactics(mtmp);

	/* check distance and scariness of attacks */
	distfleeck(mtmp,&inrange,&nearby,&scared);

	if(find_defensive(mtmp)) {
		if (use_defensive(mtmp) != 0)
			return 1;
	} else if(find_misc(mtmp)) {
		if (use_misc(mtmp) != 0)
			return 1;
	}

	/* Demonic Blackmail! */
	if(nearby && mdat->msound == MS_BRIBE &&
       (monsndx(mdat) != PM_PRISON_GUARD) &&
	   mtmp->mpeaceful && !mtmp->mtame && !u.uswallow) {
		if (mtmp->mux != u.ux || mtmp->muy != u.uy) {
			pline("%s whispers at thin air.",
			    cansee(mtmp->mux, mtmp->muy) ? Monnam(mtmp) : "It");

			if (is_demon(youmonst.data)) {
			  /* "Good hunting, brother" */
			    if (!tele_restrict(mtmp)) (void) rloc(mtmp, FALSE);
			} else {
			    mtmp->minvis = mtmp->perminvis = 0;
			    /* Why?  For the same reason in real demon talk */
			    pline("%s gets angry!", Amonnam(mtmp));
			    mtmp->mpeaceful = 0;
			    /* since no way is an image going to pay it off */
			}
		} else if(demon_talk(mtmp)) return(1);	/* you paid it off */
	}

	/* Prison guard extortion */
    if(nearby && (monsndx(mdat) == PM_PRISON_GUARD) && !mtmp->mpeaceful
	 && !mtmp->mtame && !u.uswallow && (!mtmp->mspec_used)) {
        long gdemand = 500 * u.ulevel;
        long goffer = 0;

        pline("%s demands %ld %s to avoid re-arrest.", Amonnam(mtmp),
         gdemand, currency(gdemand));
        if ((goffer = bribe(mtmp)) >= gdemand) {
            verbalize("Good.  Now beat it, scum!");
            mtmp->mpeaceful = 1;
            set_malign(mtmp);
        } else {
            pline("I said %ld!", gdemand);
            mtmp->mspec_used = 1000;
        }
    }

	if (mdat->msound == MS_STENCH && !Role_if(PM_HUSSY) && !(youmonst.data->msound == MS_STENCH) && !mtmp->mpeaceful && (distu(mtmp->mx, mtmp->my) <= BOLT_LIM * BOLT_LIM) && !rn2(20)) {
		switch (rnd(9)) {

			case 1:
				pline("Urgh! You inhale the vile stench that emanates from %s!", mon_nam(mtmp));
				break;
			case 2:
				pline("%s's perfume is beguiling, and you have trouble concentrating!", Monnam(mtmp));
				break;
			case 3:
				pline("You deeply inhale %s's feminine scent.", mon_nam(mtmp));
				break;
			case 4:
				pline("%s's odor cloud made of concentrated perfume infiltrates your %s!", Monnam(mtmp), body_part(NOSE));
				break;
			case 5:
				pline("Your %s are having trouble dealing with the asphyxiating stench that comes from %s!", makeplural(body_part(LUNG)), mon_nam(mtmp));
				break;
			case 6:
				pline("%s attacks you with a fragrance cloud!", Monnam(mtmp));
				break;
			case 7:
				pline("%s is close enough that you can smell %s perfume... but it's way too concentrated, and inhaling the aroma makes you dizzy!", Monnam(mtmp), mhis(mtmp));
				break;
			case 8:
				pline("The lovely scent of femininity floods your nostrils... until you realize that it's getting ever stronger, and you are having trouble when breathing!");
				break;
			case 9:
				pline("%s's perfume is so scentful that %s reminds you of what your aunt smells like when she comes for a visit on Christmas! Ugh!", Monnam(mtmp), mhe(mtmp));
				break;

		}
		badeffect();
	}

	/* Monsters with MS_BONES can rattle. If this causes you to snap out of a longer paralysis, more power to you :D */
	if (mdat->msound == MS_BONES && !mtmp->mpeaceful && (distu(mtmp->mx, mtmp->my) <= BOLT_LIM * BOLT_LIM) && !rn2(100)) {
		pline(Hallucination ? "%s plays the xylophone!" : "%s rattles noisily!", Monnam(mtmp));

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
			You("freeze for a moment.");
			nomul(-2, "scared by rattling", TRUE);
			nomovemsg = 0;
		}
	}

	/* the watch will look around and see if you are up to no good :-) */
	if (mdat == &mons[PM_WATCHMAN] || mdat == &mons[PM_WATCH_CAPTAIN] || mdat == &mons[PM_WATCH_LEADER] || mdat == &mons[PM_WATCH_LIEUTENANT])
		watch_on_duty(mtmp);

	/* [DS] Cthulhu also uses psychic blasts */
	else if ((is_mind_flayer(mdat) || mdat == &mons[PM_CTHULHU] || mdat == &mons[PM_FLYING_ASSHOLE] ) 
			&& !rn2( (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "narrow helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "uzkiy shlem") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "tor dubulg'a") ) ) ? 4 : 20)) {
		struct monst *m2, *nmon = (struct monst *)0;

		if (canseemon(mtmp))
			pline("%s concentrates.", Monnam(mtmp));
		if (distu(mtmp->mx, mtmp->my) > BOLT_LIM * BOLT_LIM) {
			You(Hallucination ? "sense a fantastic wave of psychic energy." : "sense a faint wave of psychic energy.");
			goto toofar;
		}
		pline("A wave of psychic energy pours over you!");
		if ( (mtmp->mpeaceful || (Psi_resist && rn2(20) ) ) &&
		    (!Conflict || resist(mtmp, RING_CLASS, 0, 0)))
			pline("It feels quite soothing.");
		else {
			register boolean m_sen = sensemon(mtmp);

			if (m_sen || (Blind_telepat && rn2(2)) || !rn2(10)) {
				int dmg;
				pline("It locks on to your %s!",
					m_sen ? "telepathy" :
					Blind_telepat ? "latent telepathy" : "mind");
				dmg = (mdat == &mons[PM_CTHULHU])?
					rn1(10, 10) :
					rn1(4, 4);
				if (Half_spell_damage && rn2(2) ) dmg = (dmg+1) / 2;
				if (!rn2(100)) { /* evil patch idea by jonadab: 1% chance of causing amnesia */
					forget(1 + rn2(5));
				}
				/* evil patch idea by Amy: chance of causing other adverse effects, like ToME eldritch horror */
				if (!rn2(100)) {
					losexp("eldritch life force drain", FALSE, TRUE);
				}
				if (!rn2(100)) {
					make_stunned(HStun + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
					make_confused(HConfusion + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
					pline("You're confused!");
				}
				if (!rn2(100)) {
					adjattrib(A_INT, -1, 1);
					adjattrib(A_WIS, -1, 1);
				}
				if (!rn2(100)) {
					(void) make_hallucinated(HHallucination + rnd(10) + rnd(monster_difficulty() + 1), TRUE, 0L);
					if (!Free_action || !rn2(20)) {
					    pline("You are frozen in place!");
					    nomul(-rnz(10), "frozen by an eldritch blast", TRUE);
						if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
					    nomovemsg = You_can_move_again;
					    exercise(A_DEX, FALSE);
					}

				}
				if (!rn2(100)) {
					pline("You let out a bloodcurdling scream!");
					wake_nearby();
				}
				if (!rn2(100)) {
					make_feared(HFeared + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
				}
				if (!rn2(100)) {
					badeffect();
				}
				if (!rn2(100)) {
					badeffect();
				}
				if (!rn2(100)) {
					badeffect();
				}
				losehp(dmg, "psychic blast", KILLED_BY_AN);
			}
		}
		for(m2=fmon; m2; m2 = nmon) {
			nmon = m2->nmon;
			if (DEADMONSTER(m2)) continue;
			if (m2->mpeaceful == mtmp->mpeaceful) continue;
			if (mindless(m2->data) || m2->egotype_undead) continue;
			if (m2 == mtmp) continue;
			if ((telepathic(m2->data) &&
			    (rn2(2) || m2->mblinded)) || !rn2(10)) {
				if (cansee(m2->mx, m2->my))
				    pline("It locks on to %s.", mon_nam(m2));
				m2->mhp -= rnd(15);
				if (m2->mhp <= 0)
				    if (mtmp->uexp)
					mon_xkilled(m2, "", AD_DRIN);
				    else
				    monkilled(m2, "", AD_DRIN);
				else
				    m2->msleeping = 0;
			}
		}
	}

	/* occasionally hint at the presence of psychic beings, if you are telepathic --Amy */
	if (dmgtype(mdat, AD_SPC2) && !rn2((uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "narrow helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "uzkiy shlem") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "tor dubulg'a") ) ) ? 40 : 200) && Blind_telepat) {
		You(Hallucination ? "sense a fantastic psionic wave." : "sense a faint psionic wave.");
	}

toofar:

	/* If monster is nearby you, and has to wield a weapon, do so.   This
	 * costs the monster a move, of course.
	 */
	if((!mtmp->mpeaceful || Conflict) && inrange &&
	   dist2(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) <= 8
	   && attacktype(mdat, AT_WEAP)) {
	    struct obj *mw_tmp;

	    /* The scared check is necessary.  Otherwise a monster that is
	     * one square near the player but fleeing into a wall would keep	
	     * switching between pick-axe and weapon.  If monster is stuck
	     * in a trap, prefer ranged weapon (wielding is done in thrwmu).
	     * This may cost the monster an attack, but keeps the monster
	     * from switching back and forth if carrying both.
	     */
	    mw_tmp = MON_WEP(mtmp);
	    if (!(scared && mw_tmp && is_pick(mw_tmp)) &&
		mtmp->weapon_check == NEED_WEAPON &&
		!(mtmp->mtrapped && !nearby && select_rwep(mtmp))) {
		mtmp->weapon_check = NEED_HTH_WEAPON;
		if (mon_wield_item(mtmp) != 0) return(0);
	    }
	}

/*	Now the actual movement phase	*/

#ifndef GOLDOBJ
	if(!nearby || mtmp->mflee || scared ||
	   mtmp->mconf || mtmp->mstun || (mtmp->minvis && !rn2(3)) ||
	   (mdat->mlet == S_LEPRECHAUN && !u.ugold && (mtmp->mgold || rn2(2))) ||

#else
        if (mdat->mlet == S_LEPRECHAUN) {
	    ygold = findgold(invent);
	    lepgold = findgold(mtmp->minvent);
	}

	if(!nearby || mtmp->mflee || scared ||
	   mtmp->mconf || mtmp->mstun || (mtmp->minvis && !rn2(3)) ||
	   (mdat->mlet == S_LEPRECHAUN && !ygold && (lepgold || rn2(2))) ||
#endif
	   (is_wanderer(mdat) && !rn2(4)) || (Conflict && mtmp->mcansee && haseyes(mtmp->data) && !resist(mtmp, RING_CLASS, 0, 0) && !mtmp->iswiz
	   && !Is_blackmarket(&u.uz)
	   ) ||
	   (!mtmp->mcansee && !rn2(iswarper ? 2 : 4)) || mtmp->mpeaceful) {
		/* Possibly cast an undirected spell if not attacking you */
		/* note that most of the time castmu() will pick a directed
		   spell and do nothing, so the monster moves normally */
		/* arbitrary distance restriction to keep monster far away
		   from you from having cast dozens of sticks-to-snakes
		   or similar spells by the time you reach it */
		if (dist2(mtmp->mx, mtmp->my, u.ux, u.uy) <= 49 && !mtmp->mspec_used && !rn2(iswarper ? 2 : 4) ) {
		    struct attack *a;

		    for (a = &mdat->mattk[0]; a < &mdat->mattk[NATTK]; a++) {
			if (a->aatyp == AT_MAGC && (a->adtyp == AD_SPEL || a->adtyp == AD_CLRC || a->adtyp == AD_CAST)) {
			    if (castmu(mtmp, a, FALSE, FALSE)) {
				tmp = 3;
				break;
			    }
			}
		    }
		    if (mtmp->egotype_arcane) {
			mdat2 = &mons[PM_CAST_DUMMY];
			a = &mdat2->mattk[0];
			if (castmu(mtmp, a, FALSE, FALSE)) {
				tmp = 3;
			    }
		    }
		    if (mtmp->egotype_clerical) {
			mdat2 = &mons[PM_CAST_DUMMY];
			a = &mdat2->mattk[1];
			if (castmu(mtmp, a, FALSE, FALSE)) {
				tmp = 3;
			    }
		    }
		    if (mtmp->egotype_mastercaster) {
			mdat2 = &mons[PM_CAST_DUMMY];
			a = &mdat2->mattk[2];
			if (castmu(mtmp, a, FALSE, FALSE)) {
				tmp = 3;
			    }
		    }
		}

		tmp = m_move(mtmp, 0);
		distfleeck(mtmp,&inrange,&nearby,&scared);	/* recalc */
		switch (tmp) {
		    case 0:	/* no movement, but it can still attack you */
		    case 3:	/* absolutely no movement */
				/* for pets, case 0 and 3 are equivalent */
			/* vault guard might have vanished */
			if (mtmp->isgd && (mtmp->mhp < 1 ||
					    (mtmp->mx == 0 && mtmp->my == 0)))
			    return 1;	/* behave as if it died */
			/* During hallucination, monster appearance should
			 * still change - even if it doesn't move.
			 */
			if(Hallucination) newsym(mtmp->mx,mtmp->my);
			break;
		    case 1:	/* monster moved */
			/* Maybe it stepped on a trap and fell asleep... */
			if (mtmp->msleeping || !mtmp->mcanmove) return(0);
			if(!nearby &&
			  (ranged_attk(mdat) || mtmp->egotype_weaponizer || mtmp->egotype_breather || find_offensive(mtmp)))
			    break;
 			else if(u.uswallow && mtmp == u.ustuck) {
			    /* a monster that's digesting you can move at the
			     * same time -dlc
			     */
			    return(mattacku(mtmp));
			} else
				return(0);
			/*NOTREACHED*/
			break;
		    case 2:	/* monster died */
			return(1);
		}
	}

/*	Now, attack the player if possible - one attack set per monst	*/

	if (!mtmp->mpeaceful ||
	    (Conflict && !resist(mtmp, RING_CLASS, 0, 0)
		&& !Is_blackmarket(&u.uz)
	)) {

		/* FIQ found out that self-genocide while polymorphed can make monsters stop attacking entirely. Fixed. */

	    if(/*inrange && */ /* !noattacks(mdat) && */ (Upolyd || u.uhp > 0) && !scared && tmp != 3)
		/* noattacks check removed, since now there's things like egotype shriekers... --Amy */
		if(mattacku(mtmp)) return(1); /* monster died (e.g. exploded) */

	    if(mtmp->wormno) wormhitu(mtmp);
	}
	/* special speeches for quest monsters */
	if (!mtmp->msleeping && mtmp->mcanmove && nearby)
	    quest_talk(mtmp);
	/* extra emotional attack for vile monsters */
	/* Come on, monsters can still cuss at you if you can't see them. Seriously, what the heck. --Amy */

	    if(inrange && dmgtype(mtmp->data, AD_FAKE) && !mtmp->mpeaceful && !rn2(20))
		{
			pline(fauxmessage());
			if (!rn2(3)) pline(fauxmessage());
		}

	    if(inrange && mtmp->egotype_faker && !mtmp->mpeaceful && !rn2(20))
		{
			pline(fauxmessage());
			if (!rn2(3)) pline(fauxmessage());
		}

	    if(inrange && mtmp->data->msound == MS_BOSS && !mtmp->mpeaceful && !rn2(10))
	    pline("%s %s", Monnam(mtmp), bosstaunt());

	    if(inrange && (mtmp->data->msound == MS_FART_QUIET || mtmp->data->msound == MS_FART_NORMAL || mtmp->data->msound == MS_FART_LOUD) && (mtmp->data->geno & G_UNIQ) && !mtmp->mpeaceful && !rn2(10))
	    pline("%s %s", Monnam(mtmp), bosstaunt());

	    if(inrange && mtmp->data->msound == MS_CUSS && !mtmp->mpeaceful &&
		/*couldsee(mtmp->mx, mtmp->my) && !mtmp->minvis &&*/ !rn2(5))
	    cuss(mtmp);

	    if(inrange && mtmp->data->msound == MS_PUPIL && !mtmp->mpeaceful &&
		/*couldsee(mtmp->mx, mtmp->my) && !mtmp->minvis &&*/ !rn2(5))

		{
		static const char *pupil_msgs[] = {
			"Today no homework ... *please*",
			"six times nine is ... um ... uh ... ... forty-two",
			"you ... Strange word",	/* You're not expected to understand this ... */
			"Bugger off!",
			"*uck off!",
			"What are the newest news about the Super Information Highway?",
			"What do you want?",
			"Do the world a favour---jump out of the 20th story of the Uni-Center!",
		};

		verbalize(pupil_msgs[rn2(SIZE(pupil_msgs))]);
		}

	    if(inrange && mtmp->data->msound == MS_WHORE && !mtmp->mpeaceful &&
		/*couldsee(mtmp->mx, mtmp->my) && !mtmp->minvis &&*/ !rn2(5))

		{
		static const char *whore_msgs[] = { /* These are obviously inspired by the GTA series --Amy */
			"Come to Momma.",
			"I'm gonna call my pimp.",
			"You picked the wrong bitch.",
			"You're just another sorry ass!",
			"Hey, this is my first night!",
			"You know, I used to be a marine before the OP.", /* Vice City */
			"Cocksucker!",
			"I'll kick your nuts flat!", /* yes the PC can be female, but who cares? */
			"I'm no slut, I just need the money!",
			"I'll be sitting on you in a second.",
			"You think I can't mess you up?",
			"Die in a pool of your own blood.",
			"Get ready for an ass-kicking.",
			"You want me to whoop you?",
			"You want some? I'll give you some!",
			"Enjoy this stiletto.",
			"If I don't kill you, my parents will.",
			"I know kickboxing.",
			"I'm a black belt in karate.",
			"My hands are lethal weapons.",
			"I'll kick your teeth in.",
			"Would you really hit a woman?",
			"I've killed hundreds of pigs like you!",
			"I'm gonna open up a can of whoopass on you now!",
			"Girls can fight too!",
			"Beating on girls, right?",
			"I have no problem kicking you in the nuts.",
			"I'll slap you silly.",
			"My pimp will take care of you.",
			"You're messing with an angry bitch!",
			"Another asshole with a problem!",
			"You think cause I'm a girl I can't fight?",
			"You call that 'fighting'?",
			"I'm gonna stomp your balls!",
			"I'm a lady but I can fight.",
			"I'm an innocent virgin!",
			"You just made me break a nail!",
			"I'm expecting an apology!",
			"You insult my womanhood.",
			"You disgust me.",
		};

		verbalize(whore_msgs[rn2(SIZE(whore_msgs))]);

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
		}

		}

	    if(inrange && mtmp->data->msound == MS_SUPERMAN && !mtmp->mpeaceful && !rn2(5))

		{
		static const char *superman_msgs[] = {
			"FRONTAL ATTACK!!!",
			"YOU DON'T STAND A CHANCE!",
			"YOU WILL DIE A SLOW, SLOW, DEATH...",
			"COME OUT! WE WILL HURT YOU!",
			"GRRRRRRRRRAAAAAAAAAAHHH!",
			"CHARRRRRRRRRGE!",
			"FEAR ME!!!",
			"DIE YOU SON OF A BITCH!", /* too lazy to check for female PC --Amy */
			"I AM YOUR DOOM!",
			"YOUR LIFE IS GONNA END NOW!",
			"YOU WILL CEASE TO EXIST!",
			"I'M GOING TO EAT YOU!",
			"RAAAAAAAAAAAARGH!",
			"ATTACK PATTERN ALPHA!",
			"YOU CAN'T HIDE!",
			"THERE'S NO ESCAPE!",
			"BE AFRAID OF ME!",
			"ATTAAAAAAAAAAAAACK!",
		};

		verbalize(superman_msgs[rn2(SIZE(superman_msgs))]);
		badeffect();
		}

	    if(inrange && mtmp->data->msound == MS_PRINCIPAL && !mtmp->mpeaceful &&
		/*couldsee(mtmp->mx, mtmp->my) && !mtmp->minvis &&*/ !rn2(5))

		{
		static const char *principal_msgs[] = {
		"What's up?",
		"I really feel sick - there are so many things to do!",
		"Help me, I faint!",
		"We'll do that in groups of one person!",
		};

		verbalize(principal_msgs[rn2(SIZE(principal_msgs))]);
		}

	    if(inrange && mtmp->data->msound == MS_TEACHER && !mtmp->mpeaceful &&
		/*couldsee(mtmp->mx, mtmp->my) && !mtmp->minvis &&*/ !rn2(5))

		{
	   	 static const char *teacher_msgs[] = {
			"No chance! Every day you'll get homework!",
			"Is it really true? Does really _everybody_ have the homework?",
			"That usage of the word 'goes' does harm to my ears!",
			"Your attitude is really unacceptable!",
			"The \"Stigel-Brauerei\" was founded 1492. Well, in that year was that affair with that guy, Columbus, but that really isn't important.",
			"Why are you going? I'm only 20 minutes late!",
			"Where's your problem? I'll be happy to help you",
			"You didn't understand? Then let's begin again ... (*sigh*)",
			"No homework yet? - This can be changed!",
			"Overbecks - das Ueberbier",
			"How about dehydrating carbonhydrates today?",
			"Back when I was a pupil, the following thing happened ...",
			"Back when I was studying chemistry, the following thing happened ...",
			"... dann ist die Scheisse am dampfen",
			"NIKI forever!",
			"Pascal forever!",
			"Yes ... I know that everything is easier in C, but I simply love Pascal ...",
			"You have Str:0 (at most), so bugger off!",
			"Do it - who cares about the odd broken bone?",
			"You are sick because you were running for 30 minutes? So run another hour!",
			"Shall I help you? (takes the whip)",
			"We'll do that diagonally. *grin* (wipes sweat off head)",
			"*grin*",
			"You know, (*grin*) we'll have to do something now! (*grin*)",
			"How about a pupil experiment - cut off your ears?",
			"Yet another pupil experiment: the consequences of KCN ingested.",
			"Don't expect to get away without homework!",
			"No homework in the holidays? You must have something to do, after all!",
			"The low level of you all is really beyond acception!",
			"There was endless work in the supervision and administration of the farm ...",
			/* it's really a shame that I can't think of more messages for him */
			"I expect you to do your homework _regularly_ and _carefully_!",
			"The level of work is really very low nowadays!",
			"In _our_ times pupils were real pupils and teachers were real teachers!",
			"Back when pupils where real pupils and teachers were real teachers, everything was better!",
		};

		verbalize(teacher_msgs[rn2(SIZE(teacher_msgs))]);

		}

	return(tmp == 2);
}

boolean
itsstuck(mtmp)
register struct monst *mtmp;
{
	if (sticks(youmonst.data) && mtmp==u.ustuck && !u.uswallow) {
		pline("%s cannot escape from you!", Monnam(mtmp));
		return(TRUE);
	}
	return(FALSE);
}

/* Return values:
 * 0: did not move, but can still attack and do other stuff.
 * 1: moved, possibly can attack.
 * 2: monster died.
 * 3: did not move, and can't do anything else either.
 */
int
m_move(mtmp, after)
register struct monst *mtmp;
register int after;
{
	register int appr;
	xchar gx,gy,nix,niy,chcnt;
	int chi;	/* could be schar except for stupid Sun-2 compiler */
	boolean likegold=0, likegems=0, likeobjs=0, likemagic=0, conceals=0;
	boolean likerock=0, can_tunnel=0;
	boolean can_open=0, can_unlock=0, doorbuster=0;
	boolean uses_items=0, setlikes=0;
	boolean avoid=FALSE;
	struct permonst *ptr;
	struct monst *mtoo;
	schar mmoved = 0;	/* not strictly nec.: chi >= 0 will do */
	long info[9];
	long flag;
	int  omx = mtmp->mx, omy = mtmp->my;
	struct obj *mw_tmp;

	if (mtmp->data->mlet == S_TURRET || stationary(mtmp->data) ) return(0); /* stationary turrets --Amy */

	/* mcalcmove() in mon.c allows sessile monsters to get turns. We still don't want them to move around though. --Amy */
	if (!mtmp->data->mmove && (!mtmp->egotype_speedster) && (!mtmp->egotype_racer) ) return(0);

	if(mtmp->mtrapped) {
	    int i = mintrap(mtmp);
	    if(i >= 2) { newsym(mtmp->mx,mtmp->my); return(2); }/* it died */
	    if(i == 1) return(0);	/* still in trap, so didn't move */
	}

	ptr = mtmp->data; /* mintrap() can change mtmp->data -dlc */

	if (mtmp->meating) {
	    mtmp->meating--;
	    return 3;			/* still eating */
	}

	if (hides_under(ptr) && OBJ_AT(mtmp->mx, mtmp->my) && rn2(10))
	    return 0;		/* do not leave hiding place */

	set_apparxy(mtmp);
	/* where does mtmp think you are? */
	/* Not necessary if m_move called from this file, but necessary in
	 * other calls of m_move (ex. leprechauns dodging)
	 */
#ifdef REINCARNATION
	if (!Is_rogue_level(&u.uz))
#endif
	    can_tunnel = tunnels(ptr);
	can_open = !(nohands(ptr) || verysmall(ptr));
	can_unlock = ((can_open && m_carrying(mtmp, SKELETON_KEY)) || (can_open && m_carrying(mtmp, SECRET_KEY)) ||
		      mtmp->iswiz || is_rider(ptr) || is_deadlysin(ptr));
/*        doorbuster = is_giant(ptr);*/

	/* WAC add dragon breath */
	doorbuster = is_giant(ptr) || (bust_door_breath(mtmp) != -1);

	if(mtmp->wormno) goto not_special;
	/* my dog gets special treatment */
	if(mtmp->mtame) {
	    mmoved = dog_move(mtmp, after);
	    goto postmov;
	}

	/* likewise for shopkeeper */
	if(mtmp->isshk) {
	    mmoved = shk_move(mtmp);
	    if(mmoved == -2) return(2);
	    if(mmoved >= 0) goto postmov;
	    mmoved = 0;		/* follow player outside shop */
	}

	/* and for the guard */
	if(mtmp->isgd) {
	    mmoved = gd_move(mtmp);
	    if(mmoved == -2) return(2);
	    if(mmoved >= 0) goto postmov;
	    mmoved = 0;
	}

	/* and the acquisitive monsters get special treatment */
	if( (is_covetous(ptr) || mtmp->egotype_covetous) && !rn2(10)) {
	    xchar tx = STRAT_GOALX(mtmp->mstrategy),
		  ty = STRAT_GOALY(mtmp->mstrategy);
	    struct monst *intruder = m_at(tx, ty);
	    /*
	     * if there's a monster on the object or in possesion of it,
	     * attack it.
	     */
	    if((dist2(mtmp->mx, mtmp->my, tx, ty) < 2) &&
	       intruder && (intruder != mtmp)) {

		notonhead = (intruder->mx != tx || intruder->my != ty);
		if(mattackm(mtmp, intruder) == 2) return(2);
		mmoved = 1;
	    } else mmoved = 0;
	    goto postmov;
	}

	/* and for the priest */
	if(mtmp->ispriest) {

		/* if the elder priest is hurt below 75%, regardless of HOW, he becomes covetous --Amy */
	    if (mtmp->data == &mons[PM_DNETHACK_ELDER_PRIEST_TM_] && (mtmp->mhp <= (mtmp->mhpmax * 3 / 4)) ) {
		mtmp->isegotype = 1;
		mtmp->egotype_covetous = 1;
	    }
	    mmoved = pri_move(mtmp);
	    if(mmoved == -2) return(2);
	    if(mmoved >= 0) goto postmov;
	    mmoved = 0;
	}

#ifdef MAIL
	if(ptr == &mons[PM_MAIL_DAEMON]) {
	    if(flags.soundok && canseemon(mtmp))
		verbalize("I'm late!");
	    mongone(mtmp);
	    return(2);
	}
#endif

	/* teleport if that lies in our nature */
	if( (ptr == &mons[PM_TENGU] || ptr == &mons[PM_TELEPORTER] || ptr == &mons[PM_EASTERN_TENGU] || ptr == &mons[PM_PHASING_TENGU] || ptr == &mons[PM_CHEERFUL_LEPRECHAUN] || ptr == &mons[PM_BLINK] || ptr == &mons[PM_VORPAL_BUNNY] || ptr == &mons[PM_KING_OF_PORN] || ptr == &mons[PM_PEARDUCK] || ptr == &mons[PM_TELEHOBBIT] || ptr == &mons[PM_SPOOPY_GHOST] || ptr == &mons[PM_ANNOYING_SLEX_GHOST] || ptr == &mons[PM_SPRING_WOLF] || ptr == &mons[PM_DIMENSIONAL_SHAMBLER] || ptr == &mons[PM_MAGNET_ELEMENTAL] || ptr == &mons[PM_PHASE_KNIGHT] || ptr == &mons[PM_TELEPORTING_DEMON] || ptr == &mons[PM_BEAMING_UFO_PART] || ptr == &mons[PM_BEAMER] || mtmp->egotype_teleportself) && !rn2(25) && !mtmp->mcan &&
	   !tele_restrict(mtmp)) {
	    if(mtmp->mhp < 7 || (ptr == &mons[PM_SPOOPY_GHOST]) || mtmp->mpeaceful || rn2(2))
		(void) rloc(mtmp, FALSE);
	    else
		mnexto(mtmp);
	    mmoved = 1;
	    goto postmov;
	}

	if (mtmp->egotype_blinker && !mtmp->mcan && !rn2(5) ) {
		(void) rloc(mtmp, FALSE);
	      mmoved = 1;
	      goto postmov;
	}

not_special:
	if(u.uswallow && !mtmp->mflee && u.ustuck != mtmp) return(1);
	omx = mtmp->mx;
	omy = mtmp->my;
	gx = mtmp->mux;
	gy = mtmp->muy;
	appr = mtmp->mflee ? -1 : 1;
	if (monsndx(ptr) == PM_NOTHING_CHECKER_WHO_IS_CONFUSED) mtmp->mconf = 1;
	if (monsndx(ptr) == PM_PANCAKE_SPIRIT) mtmp->mconf = 1;
	if (mtmp->mconf || (uarmh && !rn2(10) && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "inkcoat helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "shlem pal'to chernil") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "siyoh palto dubulg'a") ) ) || (uarmh && uarmh->oartifact == ART_RADAR_NOT_WORKING) || (monsndx(ptr) == PM_DANCING_DRAGON) || (monsndx(ptr) == PM_NOTHING_CHECKER_WHO_IS_CONFUSED) || (monsndx(ptr) == PM_TREMBLING_POPLAR) || (u.uswallow && mtmp == u.ustuck))
		appr = 0;
	else {
#ifdef GOLDOBJ
		struct obj *lepgold, *ygold;
#endif
		boolean should_see = (couldsee(omx, omy) &&
				      (levl[gx][gy].lit ||
				       !levl[omx][omy].lit) &&
				      (dist2(omx, omy, gx, gy) <= (level.flags.shortsighted ? 36 : 100) ));

		if (!mtmp->mcansee ||
		/* monsters no longer automatically know where you are. That was just incredibly annoying. --Amy */
		( (!Aggravate_monster || !rn2(5)) && !should_see && distu(mtmp->mx,mtmp->my) > 10 && (Stealth ? (can_track(ptr) ? !rn2(4) : rn2(2) ) : (can_track(ptr) ? !rn2(10) : !rn2(4) ) ) ) ||
			 ( (!Aggravate_monster || !rn2(20)) && is_wanderer(mtmp->data) ? (Stealth ? !rn2(3) : !rn2(5) ) : (Stealth ? !rn2(5) : !rn2(25) ) ) ||
		    (should_see && Invis && haseyes(ptr) && !perceives(ptr) && rn2(3)) ||
		    (youmonst.m_ap_type == M_AP_OBJECT && youmonst.mappearance == STRANGE_OBJECT) || u.uundetected ||
		    (youmonst.m_ap_type == M_AP_OBJECT && youmonst.mappearance == GOLD_PIECE && !likes_gold(ptr)) ||
		    (mtmp->mpeaceful && !mtmp->isshk) ||  /* allow shks to follow */
		    ((monsndx(ptr) == PM_STALKER || ptr->mlet == S_BAT ||
		      ptr->mlet == S_LIGHT) && !rn2(3)))
			appr = 0;

/*		if(monsndx(ptr) == PM_LEPRECHAUN && (appr == 1) &&
#ifndef GOLDOBJ
		   (mtmp->mgold > u.ugold))
#else
		   ( (lepgold = findgold(mtmp->minvent)) && 
                   (lepgold->quan > ((ygold = findgold(invent)) ? ygold->quan : 0L)) ))
#endif
			appr = -1;*/ /* commented out - they should still be attacking you --Amy */

		if (!should_see && can_track(ptr)) {
			register coord *cp;

			cp = gettrack(omx,omy);
			if (cp) {
				gx = cp->x;
				gy = cp->y;
			}
		}
	}

	if (appr == 1 && !rn2(5) && (uarm && OBJ_DESCR(objects[uarm->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarm->otyp]), "camo robe") || !strcmp(OBJ_DESCR(objects[uarm->otyp]), "kamuflyazhnaya roba") || !strcmp(OBJ_DESCR(objects[uarm->otyp]), "kamuflaj to'n") )) ) appr = 0;

	if (appr == 1 && !rn2(5) && (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "pink cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "bakh-rozovyy plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "portlash-pushti plash") ) )) appr = 0;

	if (ptr == &mons[PM_ANCIENT_BIBLICAL_DRAGON]) appr = -1;
	if (ptr == &mons[PM_BOGUXORN]) appr = -1;
	if (ptr == &mons[PM_POOL_EDGE_SWIMMER]) appr = -1;
	if (ptr == &mons[PM_SOCIAL_DISORDER]) appr = -1;

	if ((!mtmp->mpeaceful || !rn2(10))
#ifdef REINCARNATION
				    && (!Is_rogue_level(&u.uz))
#endif
							    ) {
	    boolean in_line = lined_up(mtmp) &&
		(distmin(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) <=
		    (throws_rocks(youmonst.data) ? 20 : ACURRSTR/2+1)
		);

	    if (appr != 1 || !in_line) {
		/* Monsters in combat won't pick stuff up, avoiding the
		 * situation where you toss arrows at it and it has nothing
		 * better to do than pick the arrows up.
		 */
		register int pctload = (curr_mon_load(mtmp) * 100) /
			max_mon_load(mtmp);

		/* look for gold or jewels nearby */
		likegold = (likes_gold(ptr) && pctload < 95);
		likegems = (likes_gems(ptr) && pctload < 85);
		uses_items = (/*!mindless(ptr) && !is_animal(ptr)
			&& */pctload < 75); /* I just decided that "mindless" isn't the same as "stupid". --Amy */
		likeobjs = (likes_objs(ptr) && pctload < 75);
		likemagic = (likes_magic(ptr) && pctload < 85);
		likerock = (throws_rocks(ptr) && pctload < 50 && !In_sokoban(&u.uz));
		conceals = hides_under(ptr);
		setlikes = TRUE;
	    }
	}

#define SQSRCHRADIUS	5

      { register int minr = SQSRCHRADIUS;	/* not too far away */
	register struct obj *otmp;
	register int xx, yy;
	int oomx, oomy, lmx, lmy;

	/* cut down the search radius if it thinks character is closer. */
	if(distmin(mtmp->mux, mtmp->muy, omx, omy) < SQSRCHRADIUS &&
	    !mtmp->mpeaceful) minr--;
	/* guards shouldn't get too distracted */
	if(!mtmp->mpeaceful && is_mercenary(ptr)) minr = 1;

	if((likegold || likegems || likeobjs || likemagic || likerock || conceals)
	      && (!*in_rooms(omx, omy, SHOPBASE) || (!rn2(25) && !mtmp->isshk))) {
	look_for_obj:
	    oomx = min(COLNO-1, omx+minr);
	    oomy = min(ROWNO-1, omy+minr);
	    lmx = max(1, omx-minr);
	    lmy = max(0, omy-minr);
	    for(otmp = fobj; otmp; otmp = otmp->nobj) {
		/* monsters may pick rocks up, but won't go out of their way
		   to grab them; this might hamper sling wielders, but it cuts
		   down on move overhead by filtering out most common item */
		if (otmp->otyp == ROCK) continue;
		xx = otmp->ox;
		yy = otmp->oy;
		/* Nymphs take everything.  Most other creatures should not
		 * pick up corpses except as a special case like in
		 * searches_for_item().  We need to do this check in
		 * mpickstuff() as well.
		 */
		if(xx >= lmx && xx <= oomx && yy >= lmy && yy <= oomy) {
		    /* don't get stuck circling around an object that's underneath
		       an immobile or hidden monster; paralysis victims excluded */
		    if ((mtoo = m_at(xx,yy)) != 0 &&
			(mtoo->msleeping || mtoo->mundetected ||
			 (mtoo->mappearance && !mtoo->iswiz) ||
			 !mtoo->data->mmove)) continue;

		    if(((likegold && otmp->oclass == COIN_CLASS) ||
		       (likeobjs && index(practical, otmp->oclass) &&
			(otmp->otyp != CORPSE || (ptr->mlet == S_NYMPH
			   && !is_rider(&mons[otmp->corpsenm]) && !is_deadlysin(&mons[otmp->corpsenm]) ))) ||
		       (likemagic && index(magical, otmp->oclass)) ||
		       (uses_items && searches_for_item(mtmp, otmp)) ||
		       (likerock && otmp->otyp == BOULDER) ||
		       (likegems && otmp->oclass == GEM_CLASS &&
			objects[otmp->otyp].oc_material != MINERAL) ||
		       (conceals && !cansee(otmp->ox,otmp->oy)) ||
		       (ptr == &mons[PM_GELATINOUS_CUBE] &&
			!index(indigestion, otmp->oclass) &&
			!(otmp->otyp == CORPSE &&
			  touch_petrifies(&mons[otmp->corpsenm])))
		      ) && touch_artifact(otmp,mtmp)) {
			if(can_carry(mtmp,otmp) &&
			   (throws_rocks(ptr) ||
				!sobj_at(BOULDER,xx,yy)) &&
			   (!is_unicorn(ptr) ||
			    objects[otmp->otyp].oc_material == GEMSTONE) &&
			   /* Don't get stuck circling an Elbereth */
			   !(onscary(xx, yy, mtmp))) {
			    minr = distmin(omx,omy,xx,yy);
			    oomx = min(COLNO-1, omx+minr);
			    oomy = min(ROWNO-1, omy+minr);
			    lmx = max(1, omx-minr);
			    lmy = max(0, omy-minr);
			    gx = otmp->ox;
			    gy = otmp->oy;
			    /*if (gx == omx && gy == omy) {
				mmoved = 3; actually unnecessary
				goto postmov;
			    }*/
			}
		    }
		}
	    }
	} else if(likegold) {
	    /* don't try to pick up anything else, but use the same loop */
	    uses_items = 0;
	    likegems = likeobjs = likemagic = likerock = conceals = 0;
	    goto look_for_obj;
	}

	if(minr < SQSRCHRADIUS && appr == -1) {
	    if(distmin(omx,omy,mtmp->mux,mtmp->muy) <= 3) {
		gx = mtmp->mux;
		gy = mtmp->muy;
	    } else
		appr = 1;
	}
      }

	/* don't tunnel if hostile and close enough to prefer a weapon */
	if (can_tunnel && needspick(ptr) &&
	    ((!mtmp->mpeaceful || Conflict) &&
	     dist2(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) <= 8))
	    can_tunnel = FALSE;

	nix = omx;
	niy = omy;
	flag = 0L;
	if (mtmp->mpeaceful && (!Conflict || resist(mtmp, RING_CLASS, 0, 0)))
	    flag |= (ALLOW_SANCT | ALLOW_SSM);
	else flag |= ALLOW_U;
	if (is_minion(ptr) || is_rider(ptr) || is_deadlysin(ptr)) flag |= ALLOW_SANCT;
	/* unicorn may not be able to avoid hero on a noteleport level */
	if (is_unicorn(ptr) && !level.flags.noteleport) flag |= NOTONL;
	if (passes_walls(ptr) || (mtmp->egotype_wallwalk) ) flag |= (ALLOW_WALL | ALLOW_ROCK);
	if (passes_bars(ptr) && !In_sokoban(&u.uz)) flag |= ALLOW_BARS;
	if (can_tunnel) flag |= ALLOW_DIG;
	if (is_human(ptr) || ptr == &mons[PM_MINOTAUR]) flag |= ALLOW_SSM;
	if ( (is_undead(ptr) || mtmp->egotype_undead) && ptr->mlet != S_GHOST) flag |= NOGARLIC;
	if (throws_rocks(ptr) || passes_walls(ptr) || (mtmp->egotype_wallwalk) || amorphous(ptr) || is_whirly(ptr) || ptr->mlet == S_NEMESE || ptr->mlet == S_ARCHFIEND || ptr->msound == MS_NEMESIS || ptr->geno & G_UNIQ ||
				verysmall(ptr) || slithy(ptr) || ptr == &mons[PM_BLACK_MARKETEER]) flag |= ALLOW_ROCK;
/* Boulder forts will be a lot less effective at holding dangerous monsters at bay. --Amy */
	if (can_open) flag |= OPENDOOR;
	if (can_unlock) flag |= UNLOCKDOOR;
	if (doorbuster) flag |= BUSTDOOR;
	{
	    register int i, j, nx, ny, nearer;
	    int jcnt, cnt;
	    int ndist, nidist;
	    register coord *mtrk;
	    coord poss[9];

	    cnt = mfndpos(mtmp, poss, info, flag);
	    chcnt = 0;
	    jcnt = min(MTSZ, cnt-1);
	    chi = -1;
	    nidist = dist2(nix,niy,gx,gy);
	    /* allow monsters be shortsighted on some levels for balance */
	    if(!mtmp->mpeaceful && (level.flags.shortsighted || (rn2(10) && RngeLightAbsorption) || (rn2(10) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "absorbing cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "pogloshchayushchiy plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "yutucu plash") ) ) ) &&
	       nidist > (couldsee(nix,niy) ? 144 : 36) && appr == 1) appr = 0;

		/* special coding for "homing" giant wasps from the hunger games --Amy */
		if ((ptr == &mons[PM_TRACKER_JACKER] || ptr == &mons[PM_FULL_REFUGE] || ptr == &mons[PM_REFUGE_UHLERT]) && !mtmp->mpeaceful) appr = 1;

	if (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "bug-tracking helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "oshibka otslezhivaniya shlem") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "hasharotlar-kuzatish dubulg'a") ) && !rn2(3) ) appr = 1; 

	if (uarmf && OBJ_DESCR(objects[uarmf->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "rainbow boots") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "raduga sapogi") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "kamalak chizilmasin") ) && !rn2(3) ) appr = 1; 

		if (uarmf && uarmf->oartifact == ART_HENRIETTA_S_DOGSHIT_BOOTS) appr = 1;

		if (uarmh && uarmh->oartifact == ART_RADAR_NOT_WORKING) appr = 0;

	    if (is_unicorn(ptr) && level.flags.noteleport) {
		/* on noteleport levels, perhaps we cannot avoid hero */
		for(i = 0; i < cnt; i++)
		    if(!(info[i] & NOTONL)) avoid=TRUE;
	    }
		if (avoid_player(ptr) || mtmp->egotype_avoider ) avoid=TRUE; /* making this into a monster flag --Amy */

	    for(i=0; i < cnt; i++) {
		if (avoid && (info[i] & NOTONL)) continue;
		nx = poss[i].x;
		ny = poss[i].y;

		if (appr != 0) {
		    mtrk = &mtmp->mtrack[0];
		    for(j=0; j < jcnt; mtrk++, j++)
			if(nx == mtrk->x && ny == mtrk->y)
			    if(rn2(4*(cnt-j)))
				goto nxti;
		}

		nearer = ((ndist = dist2(nx,ny,gx,gy)) < nidist);

		if((appr == 1 && nearer) || (appr == -1 && !nearer) ||
		   (!appr && !rn2(++chcnt)) || !mmoved) {
		    nix = nx;
		    niy = ny;
		    nidist = ndist;
		    chi = i;
		    mmoved = 1;
		}
	    nxti:	;
	    }
	}

	if(mmoved) {
	    register int j;

	    if (mmoved==1 && (u.ux != nix || u.uy != niy) && itsstuck(mtmp))
		return(3);

	    if (((IS_ROCK(levl[nix][niy].typ) && !(IS_FARMLAND(levl[nix][niy].typ) || IS_MOUNTAIN(levl[nix][niy].typ) || IS_GRAVEWALL(levl[nix][niy].typ)) && may_dig(nix,niy)) ||
		 closed_door(nix, niy)) &&
		mmoved==1 && can_tunnel && needspick(ptr)) {
		if (closed_door(nix, niy)) {
		    if (!(mw_tmp = MON_WEP(mtmp)) ||
			!is_pick(mw_tmp) || !is_axe(mw_tmp))
			mtmp->weapon_check = NEED_PICK_OR_AXE;
		} else if (IS_TREE(levl[nix][niy].typ)) {
		    if (!(mw_tmp = MON_WEP(mtmp)) || !is_axe(mw_tmp))
			mtmp->weapon_check = NEED_AXE;
		} else if (!(mw_tmp = MON_WEP(mtmp)) || !is_pick(mw_tmp)) {
		mtmp->weapon_check = NEED_PICK_AXE;
		}
		if (mtmp->weapon_check >= NEED_PICK_AXE && mon_wield_item(mtmp))
		    return(3);
	    }
	    /* If ALLOW_U is set, either it's trying to attack you, or it
	     * thinks it is.  In either case, attack this spot in preference to
	     * all others.
	     */
	/* Actually, this whole section of code doesn't work as you'd expect.
	 * Most attacks are handled in dochug().  It calls distfleeck(), which
	 * among other things sets nearby if the monster is near you--and if
	 * nearby is set, we never call m_move unless it is a special case
	 * (confused, stun, etc.)  The effect is that this ALLOW_U (and
	 * mfndpos) has no effect for normal attacks, though it lets a confused
	 * monster attack you by accident.
	 */
	    if(info[chi] & ALLOW_U) {
		nix = mtmp->mux;
		niy = mtmp->muy;
	    }
	    if (nix == u.ux && niy == u.uy) {
		mtmp->mux = u.ux;
		mtmp->muy = u.uy;
		return(0);
	    }
	    /* The monster may attack another based on 1 of 2 conditions:
	     * 1 - It may be confused.
	     * 2 - It may mistake the monster for your (displaced) image.
	     * Pets get taken care of above and shouldn't reach this code.
	     * Conflict gets handled even farther away (movemon()).
	     */
	    if((info[chi] & ALLOW_M) ||
		   (nix == mtmp->mux && niy == mtmp->muy)) {
		struct monst *mtmp2;
		int mstatus;
		mtmp2 = m_at(nix,niy);

		notonhead = mtmp2 && (nix != mtmp2->mx || niy != mtmp2->my);
		/* note: mstatus returns 0 if mtmp2 is nonexistent */
		mstatus = mattackm(mtmp, mtmp2);

		if (mstatus & MM_AGR_DIED)		/* aggressor died */
		    return 2;

		if ((mstatus & MM_HIT) && !(mstatus & MM_DEF_DIED)  &&
		    rn2(4) && mtmp2->movement >= NORMAL_SPEED) {
		    mtmp2->movement -= NORMAL_SPEED;
		    notonhead = 0;
		    mstatus = mattackm(mtmp2, mtmp);	/* return attack */
		    if (mstatus & MM_DEF_DIED)
			return 2;
		}
		return 3;
	    }

	    if (!m_in_out_region(mtmp,nix,niy))
	        return 3;
	    remove_monster(omx, omy);
	    place_monster(mtmp, nix, niy);

	    for(j = MTSZ-1; j > 0; j--)
		mtmp->mtrack[j] = mtmp->mtrack[j-1];
	    mtmp->mtrack[0].x = omx;
	    mtmp->mtrack[0].y = omy;
	    /* Place a segment at the old position. */
	    if (mtmp->wormno) worm_move(mtmp);
	} else {
	    if(is_unicorn(ptr) && ptr != &mons[PM_YOUNG_UNICORN] && rn2(2) && !tele_restrict(mtmp)) {
		(void) rloc(mtmp, FALSE);
		return(1);
	    }
	    if(mtmp->wormno) worm_nomove(mtmp);
	}
postmov:
	if(mmoved == 1 || mmoved == 3) {
	    boolean canseeit = cansee(mtmp->mx, mtmp->my);

	    if(mmoved == 1) {
		newsym(omx,omy);		/* update the old position */
		if (mintrap(mtmp) >= 2) {
		    if(mtmp->mx) newsym(mtmp->mx,mtmp->my);
		    return(2);	/* it died */
		}
		ptr = mtmp->data;

		/* open a door, or crash through it, if you can */
		if(IS_DOOR(levl[mtmp->mx][mtmp->my].typ)
			&& !passes_walls(ptr) && (!mtmp->egotype_wallwalk) /* doesn't need to open doors */
			&& !can_tunnel /* taken care of below */
		      ) {
		    struct rm *here = &levl[mtmp->mx][mtmp->my];
		    boolean btrapped = (here->doormask & D_TRAPPED);

		    if(here->doormask & (D_LOCKED|D_CLOSED) && (amorphous(ptr)  ) ) {
			if (flags.verbose && canseemon(mtmp))
			    pline("%s %s under the door.", Monnam(mtmp),
				  (ptr == &mons[PM_FOG_CLOUD] ||
				   ptr == &mons[PM_YELLOW_LIGHT])
				  ? "flows" : "oozes");
		    } else if(here->doormask & D_LOCKED && can_unlock) {
			if(btrapped) {
			    here->doormask = D_NODOOR;
			    newsym(mtmp->mx, mtmp->my);
			    unblock_point(mtmp->mx,mtmp->my); /* vision */
			    if(mb_trapped(mtmp)) return(2);
			} else {
			    if (flags.verbose) {
				if (canseeit) {
				   You("see a door unlock and open.");
				   if (PlayerHearsSoundEffects) pline(issoviet ? "Kto-to klyuch k pobede pervogo igroka. Bud'te ochen' boyatsya." : "Kloeck-wong!");
				}
				else if (flags.soundok)
				   You_hear("a door unlock and open.");
			    }
			    here->doormask = D_ISOPEN;
			    /* newsym(mtmp->mx, mtmp->my); */
			    unblock_point(mtmp->mx,mtmp->my); /* vision */
			}
		    } else if (here->doormask == D_CLOSED && (can_open ) ) {
			if(btrapped) {
			    here->doormask = D_NODOOR;
			    newsym(mtmp->mx, mtmp->my);
			    unblock_point(mtmp->mx,mtmp->my); /* vision */
			    if(mb_trapped(mtmp)) return(2);
			} else {
			    if (flags.verbose) {
				if (canseeit) {
				     You("see a door open.");
				     if (PlayerHearsSoundEffects) pline(issoviet ? "Mirskiye veshchi, kak dveri ne mozhet derzhat' vas v bezopasnosti, nub!" : "Wong!");
				}
				else if (flags.soundok)
				     You_hear("a door open.");
			    }
			    here->doormask = D_ISOPEN;
			    /* newsym(mtmp->mx, mtmp->my); */  /* done below */
			    unblock_point(mtmp->mx,mtmp->my); /* vision */
			}
		    } else if (here->doormask & (D_LOCKED|D_CLOSED)) {
			/* mfndpos guarantees this must be a doorbuster */
				/* WAC do dragons and breathers */
				if (bust_door_breath(mtmp) != -1) {
				        (void) breamspot(mtmp,
				                 &ptr->mattk[bust_door_breath(mtmp)],
				                 (nix-omx), (niy-omy));
				} else
			if(btrapped) {
			    here->doormask = D_NODOOR;
			    newsym(mtmp->mx, mtmp->my);
			    unblock_point(mtmp->mx,mtmp->my); /* vision */
			    if(mb_trapped(mtmp)) return(2);
			} else {
			    if (flags.verbose) {
				if (canseeit) {
				    You("see a door crash open.");
				    if (PlayerHearsSoundEffects) pline(issoviet ? "Tam net bar'yera mezhdu vami i burlyashchey ordy monstrov. Vse oni budut obdavat' vas, kak bibleyskiy potop!" : "Wongwongwongwongwongwongwongbooooooooooooooom!");
				}
				else if (flags.soundok)
				    You_hear("a door crash open.");
			    }
			    if (here->doormask & D_LOCKED && !rn2(2))
				    here->doormask = D_NODOOR;
			    else here->doormask = D_BROKEN;
			    /* newsym(mtmp->mx, mtmp->my); */ /* done below */
			    unblock_point(mtmp->mx,mtmp->my); /* vision */
			}
			/* if it's a shop door, schedule repair */
			if (*in_rooms(mtmp->mx, mtmp->my, SHOPBASE))
			    add_damage(mtmp->mx, mtmp->my, 0L);
		    }
		} else if (levl[mtmp->mx][mtmp->my].typ == IRONBARS) {
			if (flags.verbose && canseemon(mtmp))
			    Norep("%s %s %s the iron bars.", Monnam(mtmp),
				  /* pluralization fakes verb conjugation */
				  makeplural(locomotion(ptr, "pass")),
				  passes_walls(ptr) ? "through" : "between");
		}

		/* possibly dig */
		if (can_tunnel && mdig_tunnel(mtmp))
			return(2);  /* mon died (position already updated) */

		/* set also in domove(), hack.c */
		if (u.uswallow && mtmp == u.ustuck &&
					(mtmp->mx != omx || mtmp->my != omy)) {
		    /* If the monster moved, then update */
		    u.ux0 = u.ux;
		    u.uy0 = u.uy;
		    u.ux = mtmp->mx;
		    u.uy = mtmp->my;
		    swallowed(0);
		} else
		newsym(mtmp->mx,mtmp->my);
	    }
	    if(OBJ_AT(mtmp->mx, mtmp->my) && mtmp->mcanmove) {
		/* recompute the likes tests, in case we polymorphed
		 * or if the "likegold" case got taken above */
		if (setlikes) {
		    register int pctload = (curr_mon_load(mtmp) * 100) /
			max_mon_load(mtmp);

		    /* look for gold or jewels nearby */
		    likegold = (likes_gold(ptr) && pctload < 95);
		    likegems = (likes_gems(ptr) && pctload < 85);
		    uses_items = (/*!mindless(ptr) && !is_animal(ptr)
				  && */pctload < 75);
		    likeobjs = (likes_objs(ptr) && pctload < 75);
		    likemagic = (likes_magic(ptr) && pctload < 85);
		    likerock = (throws_rocks(ptr) && pctload < 50 &&
				!In_sokoban(&u.uz));
		    conceals = hides_under(ptr);
		}

		if ((EaterBugEffect || u.uprops[EATER_BUG].extrinsic || have_eaterstone())) {
		    if (meatanything(mtmp) == 2) return 2;	/* it died */
		}

		/* Maybe a rock mole just ate some metal object */
		if (metallivorous(ptr) || mtmp->egotype_metallivore) {
		    if (meatmetal(mtmp) == 2) return 2;	/* it died */
		}

		/* or a lithic object */
		if (lithivorous(ptr) || mtmp->egotype_lithivore) {
		    if (meatlithic(mtmp) == 2) return 2;	/* it died */
		}

		if(g_at(mtmp->mx,mtmp->my) && likegold) mpickgold(mtmp);

		/* Maybe a cube ate just about anything */
		/* KMH -- Taz likes organics, too! */
		if (organivorous(ptr) || mtmp->egotype_organivore) {
		    if (meatobj(mtmp) == 2) return 2;	/* it died */
		}
		if (ptr == &mons[PM_GHOUL] || ptr == &mons[PM_GHAST] || ptr == &mons[PM_GASTLY] || ptr == &mons[PM_PHANTOM_GHOST]
		|| ptr == &mons[PM_HAUNTER] || ptr == &mons[PM_GENGAR] || ptr == &mons[PM_CORPULENT_DOG] || ptr == &mons[PM_SPIT_DEMON] || ptr == &mons[PM_THESTRAL] || ptr == &mons[PM_THICK_POTATO] || ptr == &mons[PM_BLACK_MUZZLE] || ptr == &mons[PM_CORPSE_SPITTER] || ptr == &mons[PM_MUZZLE_FIEND] || ptr == &mons[PM_MAW_FIEND] || ptr == &mons[PM_ROCKET_MUZZLE]) meatcorpse(mtmp);

		if(!*in_rooms(mtmp->mx, mtmp->my, SHOPBASE) || !rn2(25)) {
		    boolean picked = FALSE;

		    if(likeobjs) picked |= mpickstuff(mtmp, practical);
		    if(likemagic) picked |= mpickstuff(mtmp, magical);
		    if(likerock) picked |= mpickstuff(mtmp, boulder_class);
		    if(likegems) picked |= mpickstuff(mtmp, gem_class);
		    if(uses_items) picked |= mpickstuff(mtmp, (char *)0);
		    /*if(picked) mmoved = 3;*/
		}

		if(mtmp->minvis) {
		    newsym(mtmp->mx, mtmp->my);
		    if (mtmp->wormno) see_wsegs(mtmp);
		}
	    }

	    if(hides_under(ptr) || (ptr->mlet == S_EEL && !(ptr == &mons[PM_DEFORMED_FISH]) ) ) {
		/* Always set--or reset--mundetected if it's already hidden
		   (just in case the object it was hiding under went away);
		   usually set mundetected unless monster can't move.  */
		if (mtmp->mundetected ||
			(mtmp->mcanmove && !mtmp->msleeping && rn2(5)))
		    mtmp->mundetected = (ptr->mlet != S_EEL) ?
			OBJ_AT(mtmp->mx, mtmp->my) :
			((is_waterypool(mtmp->mx, mtmp->my) || is_watertunnel(mtmp->mx, mtmp->my) || is_shiftingsand(mtmp->mx, mtmp->my)) && !Is_waterlevel(&u.uz));
		newsym(mtmp->mx, mtmp->my);
	    }
		if (ptr == &mons[PM_LAVA_SPIDER] && is_lava(mtmp->mx, mtmp->my)) {
			mtmp->mundetected = 1;
			newsym(mtmp->mx, mtmp->my);
		}
	    if (mtmp->isshk) {
		after_shk_move(mtmp);
	    }
	}
	return(mmoved);
}

#endif /* OVL0 */
#ifdef OVL2

boolean
closed_door(x, y)
register int x, y;
{
	return((boolean)(IS_DOOR(levl[x][y].typ) &&
			(levl[x][y].doormask & (D_LOCKED | D_CLOSED))));
}

boolean
accessible(x, y)
register int x, y;
{
	return((boolean)(ACCESSIBLE(levl[x][y].typ) && !closed_door(x, y)));
}

#endif /* OVL2 */
#ifdef OVL0

/* decide where the monster thinks you are standing */
void
set_apparxy(mtmp)
register struct monst *mtmp;
{
	boolean notseen, gotu;
	register int disp, mx = mtmp->mux, my = mtmp->muy;
#ifdef GOLDOBJ
	long umoney = money_cnt(invent);
#endif

	/*
	 * do cheapest and/or most likely tests first
	 */

	/* pet knows your smell; grabber still has hold of you */
	if (mtmp->mtame || mtmp == u.ustuck) goto found_you;

	/* monsters which know where you are don't suddenly forget,
	   if you haven't moved away */
	if (mx == u.ux && my == u.uy) goto found_you;

	notseen = (!mtmp->mcansee || (Invis && haseyes(mtmp->data) && !perceives(mtmp->data)));
	/* add cases as required.  eg. Displacement ... */
	if (notseen || Underwater) {
	    /* Xorns can smell valuable metal like gold, treat as seen */
	    if ((mtmp->data == &mons[PM_XORN]) &&
#ifndef GOLDOBJ
			u.ugold
#else
			umoney
#endif
			&& !Underwater)
		disp = 0;
	    else
		disp = 1;
	} else if (Displaced && !(dmgtype(mtmp->data, AD_DISP) ) && !(dmgtype(mtmp->data, AD_MAGM) ) && mtmp->data != &mons[PM_BABY_GRAY_DRAGON] && mtmp->data != &mons[PM_YOUNG_GRAY_DRAGON] && mtmp->data != &mons[PM_YOUNG_ADULT_GRAY_DRAGON] &&
		!(dmgtype(mtmp->data, AD_RBRE) ) ) {
	    disp = couldsee(mx, my) ? 2 : 1;
	} else disp = 0;
	if (!disp) goto found_you;

	/* without something like the following, invis. and displ.
	   are too powerful */
	gotu = notseen ? rn2(3) : Displaced ? rn2(2) : FALSE;

#if 0		/* this never worked as intended & isn't needed anyway */
	/* If invis but not displaced, staying around gets you 'discovered' */
	gotu |= (!Displaced && u.dx == 0 && u.dy == 0);
#endif

	if (!gotu) {
	    register int try_cnt = 0;
	    do {
		if (++try_cnt > 200) goto found_you;		/* punt */
		mx = u.ux - disp + rn2(2*disp+1);
		my = u.uy - disp + rn2(2*disp+1);
	    } while (!isok(mx,my)
		  || (disp != 2 && mx == mtmp->mx && my == mtmp->my)
		  || ((mx != u.ux || my != u.uy) &&
		      !passes_walls(mtmp->data) && (!mtmp->egotype_wallwalk) &&
		      (!ACCESSIBLE(levl[mx][my].typ) ||
		       (closed_door(mx, my) && !can_ooze(mtmp))))
		  || !couldsee(mx, my));
	} else {
found_you:
	    mx = u.ux;
	    my = u.uy;
	}

	mtmp->mux = mx;
	mtmp->muy = my;
}

boolean
can_ooze(mtmp)
struct monst *mtmp;
{
	struct obj *chain, *obj;

	if (!amorphous(mtmp->data)) return FALSE;

	/* If you carry really little stuff, you should be able to ooze even if the items are bulky. --Amy */

	if (mtmp == &youmonst && ((inv_weight() + weight_cap()) < 100) ) return TRUE;

	if (mtmp == &youmonst && ((inv_weight() + weight_cap()) < (weight_cap() / 10) ) ) return TRUE;


	if (mtmp == &youmonst) {
#ifndef GOLDOBJ
		if (u.ugold > 100L) return FALSE;
#endif
		chain = invent;
	} else {
#ifndef GOLDOBJ
		if (mtmp->mgold > 100L) return FALSE;
#endif
		chain = mtmp->minvent;
	}
	for (obj = chain; obj; obj = obj->nobj) {
		int typ = obj->otyp;

#ifdef GOLDOBJ
                if (typ == COIN_CLASS && obj->quan > 100L) return FALSE;
#endif
		if (obj->oclass != GEM_CLASS &&
		    !(typ >= ARROW && typ <= BATARANG) &&
		    !(typ >= DAGGER && typ <= CRYSKNIFE) &&
		    typ != SLING &&
		    !is_cloak(obj) && typ != FEDORA &&
		    !is_gloves(obj) && typ != LEATHER_JACKET &&
		    typ != CREDIT_CARD && typ != DATA_CHIP && !is_shirt(obj) &&
		    !(typ == CORPSE && verysmall(&mons[obj->corpsenm])) &&
		    typ != FORTUNE_COOKIE && typ != CANDY_BAR &&
		    typ != PANCAKE && typ != LEMBAS_WAFER &&
		    typ != LUMP_OF_ROYAL_JELLY &&
		    obj->oclass != AMULET_CLASS &&
		    obj->oclass != IMPLANT_CLASS &&
		    obj->oclass != RING_CLASS &&
#ifdef WIZARD
		    obj->oclass != VENOM_CLASS &&
#endif
		    typ != SACK && typ != BAG_OF_HOLDING &&
		    typ != BAG_OF_TRICKS && !Is_candle(obj) &&
		    typ != OILSKIN_SACK && typ != LEATHER_LEASH && typ != INKA_LEASH &&
		    typ != STETHOSCOPE && typ != BLINDFOLD && typ != EYECLOSER && typ != DRAGON_EYEPATCH && typ != TOWEL &&
		    typ != TIN_WHISTLE && typ != GRASS_WHISTLE && typ != MAGIC_WHISTLE &&
		    typ != MAGIC_MARKER && typ != TIN_OPENER && typ != BUDO_NO_SASU &&
		    typ != SKELETON_KEY && typ != SECRET_KEY && typ != LOCK_PICK && typ != HAIRCLIP
		) return FALSE;
		if (Is_container(obj) && obj->cobj) return FALSE;
		    
	}
	return TRUE;
}

static int
bust_door_breath(mtmp)
	register struct monst        *mtmp;
{
	struct permonst *ptr = mtmp->data;
	int     i;


	if (mtmp->mcan || mtmp->mspec_used) return (-1); /* Cancelled/used up */

	for(i = 0; i < NATTK; i++)
            if ((ptr->mattk[i].aatyp == AT_BREA) &&
                (ptr->mattk[i].adtyp == AD_ACID ||
                ptr->mattk[i].adtyp == AD_MAGM ||
                ptr->mattk[i].adtyp == AD_DISN ||
                ptr->mattk[i].adtyp == AD_ELEC ||
                ptr->mattk[i].adtyp == AD_FIRE ||
                ptr->mattk[i].adtyp == AD_COLD)) return(i);

        return(-1);
}


#endif /* OVL0 */

/*monmove.c*/
