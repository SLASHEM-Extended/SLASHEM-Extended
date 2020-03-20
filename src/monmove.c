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

		if((Role_if(PM_CONVICT) || Role_if(PM_MURDERER) || Race_if(PM_ALBAE) || Race_if(PM_PLAYER_DYNAMO)) && !Upolyd) {
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

	return (boolean)((sobj_at(SCR_SCARE_MONSTER, x, y) && !(Conflict && rn2(StrongConflict ? 5 : 2)) && !scmresists)
			 || (sengr_at("Elbereth", x, y) && !mresists && !(Conflict && rn2(StrongConflict ? 5 : 2)) && !(EngravingDoesntWork || u.uprops[ENGRAVINGBUG].extrinsic || have_engravingstone() || (uarmf && uarmf->oartifact == ART_VARIANTISH_DESIGN) ) )
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

	/* is the monster bleeding despite not being able to bleed? if so, stop its bleeding --Amy */
	if (mon->bleedout && (!has_blood(mon->data) ) ) {
		mon->bleedout = 0;
	}

	if (mon->healblock || mon->bleedout) return; /* sorry --Amy */

	regenrate = (20 - (mon->m_lev / 3));
	if (regenrate < 6) regenrate = 6;
	if (ishaxor) regenrate /= 2;

	if (mon->mhp < mon->mhpmax && !is_golem(mon->data) &&
	    (!rn2(regenrate) || regenerates(mon->data) || mon->egotype_regeneration )) mon->mhp++;

	if (mon->data == &mons[PM_GROGGY_GUY]) {
		mon->mhp += rnd(5);
		if (mon->mhp > mon->mhpmax) mon->mhp = mon->mhpmax;
	}

	/* super regene */
	if (mon->data == &mons[PM_MESHERA_ALPHA_DEFORMED_ANGEL] || mon->data == &mons[PM_TESTER] || mon->data == &mons[PM_TEA_HUSSY] || mon->data == &mons[PM_OUROBOROS] || mon->data == &mons[PM_JUERE_DEMON_SOLDIER] || mon->data == &mons[PM_JENNIFER_DEMON_SOLDIER] || mon->data == &mons[PM_RENAI_OVER_MESHERA]) {
		mon->mhp += 20;
		if (mon->mhp > mon->mhpmax) mon->mhp = mon->mhpmax;
	}

	if (mon->m_en < mon->m_enmax && 
	    (!rn2(regenrate) || (rn2(mon->m_lev + 5) > 15))) {
	    	mon->m_en += rn1((mon->m_lev % 10 + 1),1);
	    	if (mon->m_en > mon->m_enmax) mon->m_en = mon->m_enmax;
	}

	if (powerfulimplants() && uimplant && uimplant->oartifact == ART_READY_FOR_A_RIDE && u.usteed && (mon == u.usteed)) {
		if (mon->mhp + 1 >= mon->mhpmax)
		      mon->mhp = mon->mhpmax;
		else mon->mhp++;
	}

	if (u.usteed && mon == u.usteed) {
		struct obj *osaeddle = which_armor(u.usteed, W_SADDLE);

		if ((osaeddle = which_armor(u.usteed, W_SADDLE)) && osaeddle->oartifact == ART_CURE_HASSIA_COURSE) {
			if (mon->mhp + 1 >= mon->mhpmax)
			      mon->mhp = mon->mhpmax;
			else mon->mhp++;
		}
	}

	/* good riding skill gives extra regeneration to ridden monster --Amy */

	if (!(PlayerCannotUseSkills)) {

		if (P_SKILL(P_RIDING) == P_BASIC && u.usteed && (mon == u.usteed) && !rn2(10) ) {
			if (powerfulimplants() && uimplant && uimplant->oartifact == ART_READY_FOR_A_RIDE) {
				if (mon->mhp + 1 >= mon->mhpmax)
				      mon->mhp = mon->mhpmax;
				else mon->mhp++;
			}

		}
		if (P_SKILL(P_RIDING) == P_SKILLED && u.usteed && (mon == u.usteed) && !rn2(10) ) {
			if (mon->mhp + 1 >= mon->mhpmax)
			      mon->mhp = mon->mhpmax;
			else mon->mhp++;
			if (powerfulimplants() && uimplant && uimplant->oartifact == ART_READY_FOR_A_RIDE) {
				if (mon->mhp + 2 >= mon->mhpmax)
				      mon->mhp = mon->mhpmax;
				else mon->mhp++;
			}

		}
		if (P_SKILL(P_RIDING) == P_EXPERT && u.usteed && (mon == u.usteed) && !rn2(5) ) {
			if (mon->mhp + 1 >= mon->mhpmax)
			      mon->mhp = mon->mhpmax;
			else mon->mhp++;
			if (powerfulimplants() && uimplant && uimplant->oartifact == ART_READY_FOR_A_RIDE) {
				if (mon->mhp + 3 >= mon->mhpmax)
				      mon->mhp = mon->mhpmax;
				else mon->mhp++;
			}
		}
		if (P_SKILL(P_RIDING) == P_MASTER && u.usteed && (mon == u.usteed) && !rn2(3) ) {
			if (mon->mhp + 1 >= mon->mhpmax)
			      mon->mhp = mon->mhpmax;
			else mon->mhp++;
			if (powerfulimplants() && uimplant && uimplant->oartifact == ART_READY_FOR_A_RIDE) {
				if (mon->mhp + 4 >= mon->mhpmax)
				      mon->mhp = mon->mhpmax;
				else mon->mhp++;
			}
		}
		if (P_SKILL(P_RIDING) == P_GRAND_MASTER && u.usteed && (mon == u.usteed) ) {
			if (mon->mhp + 1 >= mon->mhpmax)
			      mon->mhp = mon->mhpmax;
			else mon->mhp++;
			if (powerfulimplants() && uimplant && uimplant->oartifact == ART_READY_FOR_A_RIDE) {
				if (mon->mhp + 5 >= mon->mhpmax)
				      mon->mhp = mon->mhpmax;
				else mon->mhp++;
			}
		}
		if (P_SKILL(P_RIDING) == P_SUPREME_MASTER && u.usteed && (mon == u.usteed) ) {
			if (mon->mhp + 1 >= mon->mhpmax)
			      mon->mhp = mon->mhpmax;
			else mon->mhp++;
			if (powerfulimplants() && uimplant && uimplant->oartifact == ART_READY_FOR_A_RIDE) {
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
	if(couldsee(mtmp->mx,mtmp->my) && !(Race_if(PM_VIETIS) && rn2(3)) &&
		distu(mtmp->mx,mtmp->my) <= 100 &&
		(!Stealth || (Stealth && !StrongStealth && !rn2(5)) || (Aggravate_monster && !rn2(3) ) || (mtmp->data == &mons[PM_ETTIN] && rn2(10))) &&
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
		  if (isok(u.ux, u.uy) && sengr_at("Elbereth", u.ux, u.uy)) u.cnd_elberethamount++;
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
	if (!mtmp->mcansee || (Invis && (StrongInvis || !rn2(3)) && !perceives(mtmp->data))) {
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
	if (uarmf && uarmf->oartifact == ART_ELEVECULT && !rn2(3)) return 0;

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

	if ((mdat == &mons[PM_BUGBEAM_CUBE] || mdat == &mons[PM_TORSTINA] || mdat == &mons[PM_MARINERV] || mdat == &mons[PM_MARISTIN] || mdat == &mons[PM_MARIVERT] || mdat == &mons[PM_MARISISTER] || mdat == &mons[PM_FUNNY_ITALIAN] || mdat == &mons[PM_EAR_FIG_MACHINE] || mdat == &mons[PM_POLEPOKER] || mdat == &mons[PM_DISTURBMENT_HEAD]) && !rn2(4)) return 0; /* can sometimes not move; this is by design */

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

	if ((mtmp->data->geno & G_UNIQ) && !rn2(20) && (RangCallEffect || u.uprops[RANG_CALL_EFFECT].extrinsic || have_rangcallstone())) {
		pline("A horrible call rang in your head...");
		increasesanity(1);
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

	if ((WakeupCallBug || u.uprops[WAKEUP_CALL_BUG].extrinsic || have_wakeupcallstone() || Race_if(PM_SERB)) && mtmp->mpeaceful && !mtmp->mtame && !rn2(10000)) {
		wakeup(mtmp);
	}

	/* not frozen or sleeping: wipe out texts written in the dust */
	wipe_engr_at(mtmp->mx, mtmp->my, 1);

	/* bleeding monsters will take damage and may die from blood loss --Amy */
	if (mtmp->bleedout) {

		int bleedingdamage = rnd(1 + (mtmp->bleedout / 10));
		if (bleedingdamage > mtmp->bleedout) bleedingdamage = mtmp->bleedout;
		mtmp->mhp -= bleedingdamage;
		mtmp->bleedout -= bleedingdamage;
		if (cansee(mtmp->mx,mtmp->my) && !rn2(10)) pline("%s bleeds.", Monnam(mtmp));
		if (mtmp->bleedout < 0) mtmp->bleedout = 0; /* fail safe */

		if (mtmp->mhp <= 0) {
			if (cansee(mtmp->mx,mtmp->my)) pline("%s dies from loss of blood", Monnam(mtmp));
			/* the lack of a period is intentional because the message is the same in Elona :P --Amy */
			mondied(mtmp);
			return(1);
		}
	}

	/* heal block will time out --Amy */
	if (mtmp->healblock) {
		mtmp->healblock--;

		if (!PlayerCannotUseSkills && mtmp->mtame) {

			switch (P_SKILL(P_PETKEEPING)) {
				default: break;
				case P_BASIC: if (!rn2(10)) mtmp->healblock--; break;
				case P_SKILLED: if (!rn2(5)) mtmp->healblock--; break;
				case P_EXPERT: if (rnd(10) > 7) mtmp->healblock--; break;
				case P_MASTER: if (rnd(10) > 6) mtmp->healblock--; break;
				case P_GRAND_MASTER: if (!rn2(2)) mtmp->healblock--; break;
				case P_SUPREME_MASTER: if (rnd(10) > 4) mtmp->healblock--; break;
			}

		}

		if (mtmp->healblock < 0) mtmp->healblock = 0; /* fail safe */
	}

	/* inertia will also time out, and slows down the monster --Amy */
	if (mtmp->inertia) {
		mtmp->inertia--;

		if (!PlayerCannotUseSkills && mtmp->mtame) {

			switch (P_SKILL(P_PETKEEPING)) {
				default: break;
				case P_BASIC: if (!rn2(5)) mtmp->inertia--; break;
				case P_SKILLED: if (rnd(5) > 3) mtmp->inertia--; break;
				case P_EXPERT: if (rnd(5) > 2) mtmp->inertia--; break;
				case P_MASTER: if (rn2(5)) mtmp->inertia--; break;
				case P_GRAND_MASTER: mtmp->inertia--; break;
				case P_SUPREME_MASTER: if (!rn2(5)) mtmp->inertia--; mtmp->inertia--; break;
			}

		}

		if (mtmp->inertia < 0) mtmp->inertia = 0; /* fail safe */
		if (!rn2(2)) return 0; /* because I'm lazy :P monster loses a turn with 50% chance, instead of every other turn */
	}

	/* confused monsters get unconfused with small probability */
	if (mtmp->mconf) {
		int unconfusechance = 50;

		if (!PlayerCannotUseSkills && mtmp->mtame) {

			switch (P_SKILL(P_PETKEEPING)) {
				default: break;
				case P_BASIC: unconfusechance = 45; break;
				case P_SKILLED: unconfusechance = 40; break;
				case P_EXPERT: unconfusechance = 35; break;
				case P_MASTER: unconfusechance = 30; break;
				case P_GRAND_MASTER: unconfusechance = 25; break;
				case P_SUPREME_MASTER: unconfusechance = 20; break;
			}

		}

		if (!rn2(unconfusechance)) mtmp->mconf = 0;
	}

	/* stunned monsters get un-stunned with larger probability */
	if (mtmp->mstun) {
		int unstunchance = 10;

		if (!PlayerCannotUseSkills && mtmp->mtame) {

			switch (P_SKILL(P_PETKEEPING)) {
				default: break;
				case P_BASIC: unstunchance = 9; break;
				case P_SKILLED: unstunchance = 8; break;
				case P_EXPERT: unstunchance = 7; break;
				case P_MASTER: unstunchance = 6; break;
				case P_GRAND_MASTER: unstunchance = 5; break;
				case P_SUPREME_MASTER: unstunchance = 4; break;
			}

		}

		if (!rn2(unstunchance)) mtmp->mstun = 0;

	}

	/* cancelled monsters get un-cancelled with a VERY low probability --Amy */
	if (mtmp->mcan && !rn2(10000)) mtmp->mcan = 0;

	/* slowed monsters get un-slowed with a low probability --Amy */
	if (mtmp->mspeed == MSLOW && mtmp->permspeed == MSLOW && !mtmp->inertia && !rn2(2000)) mon_adjust_speed(mtmp, 1, (struct obj *)0);

	/* is the monster charging a special laser cannon? */
	if (mtmp->hominglazer) {
		mtmp->hominglazer++;
		/* it can only be charged for so long, and then the monster needs to charge it again */
		if (mtmp->hominglazer > 200) mtmp->hominglazer = 0;
	}

	/* monsters whose butts were bashed by you will slowly recover --Amy */
	if (mtmp->butthurt && !rn2(mdat->msound == MS_FART_QUIET ? 5 : mdat->msound == MS_FART_NORMAL ? 20 : 50) ) mtmp->butthurt -= 1;

	/* some monsters teleport */
	if (mtmp->mflee && !rn2(40) && can_teleport(mdat) && !mtmp->iswiz &&
	    !level.flags.noteleport && !u.antitelespelltimeout) {
		(void) rloc(mtmp, FALSE);
		return(0);
	}
	if (mdat->msound == MS_SHRIEK && !um_dist(mtmp->mx, mtmp->my, 1))
	    m_respond(mtmp);

	if (mtmp->fartbonus > 9) mtmp->fartbonus = 9; /* fail save, gaaaaaah */

	if (FemaleTrapMeltem && mtmp->female && humanoid(mdat) && !rn2(10 + mtmp->butthurt - mtmp->fartbonus) && !um_dist(mtmp->mx, mtmp->my, fartdistance) && !mtmp->mpeaceful) {
		pline("%s produces %s farting noises with %s %s butt.", Monnam(mtmp), rn2(2) ? "beautiful" : "squeaky", mhis(mtmp), mtmp->female ? "sexy" : "ugly" );
		u.cnd_fartingcount++;
		if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
		if (uarmf && uarmf->oartifact == ART_ELIANE_S_SHIN_SMASH) {
			pline("The farting gas destroys your footwear instantly.");
		      useup(uarmf);
		}
		if (mtmp->butthurt) mtmp->butthurt--;
		if (mtmp->butthurt) mtmp->butthurt--;
		if (uarmf && uarmf->oartifact == ART_ELIANE_S_COMBAT_SNEAKERS) {
			pline("Eek! You can't stand farting gas!");
			badeffect();
			badeffect();
			badeffect();
			badeffect();
		}
		if (!extralongsqueak()) badeffect();

		if (uarmh && itemhasappearance(uarmh, APP_BREATH_CONTROL_HELMET) ) {
			pline("Your breath control helmet keeps pumping the farting gas into your %s...", body_part(NOSE));
			badeffect();
			badeffect();
		}

		if (uarmh && uarmh->oartifact == ART_VACUUM_CLEANER_DEATH) {
			pline("The farting gas almost asphyxiates you!");
			badeffect();
			badeffect();
			badeffect();
			badeffect();
			badeffect();
			losehp(rnd(u.ulevel * 3), "suffocating on farting gas", KILLED_BY);
		}

		if (!rn2(20)) increasesanity(1);

		while (FemaleTrapElena && !rn2(3)) {
			pline("You long for more!");
			pline("%s produces %s farting noises with %s %s butt.", Monnam(mtmp), rn2(2) ? "beautiful" : "squeaky", mhis(mtmp), mtmp->female ? "sexy" : "ugly" );
			u.cnd_fartingcount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (uarmf && uarmf->oartifact == ART_ELIANE_S_SHIN_SMASH) {
				pline("The farting gas destroys your footwear instantly.");
			      useup(uarmf);
			}
			if (mtmp->butthurt) mtmp->butthurt--;
			if (mtmp->butthurt) mtmp->butthurt--;
			if (uarmf && uarmf->oartifact == ART_ELIANE_S_COMBAT_SNEAKERS) {
				pline("Eek! You can't stand farting gas!");
				badeffect();
				badeffect();
				badeffect();
				badeffect();
			}
			if (!extralongsqueak()) badeffect();

			if (uarmh && itemhasappearance(uarmh, APP_BREATH_CONTROL_HELMET) ) {
				pline("Your breath control helmet keeps pumping the farting gas into your %s...", body_part(NOSE));
				badeffect();
				badeffect();
			}

			if (uarmh && uarmh->oartifact == ART_VACUUM_CLEANER_DEATH) {
				pline("The farting gas almost asphyxiates you!");
				badeffect();
				badeffect();
				badeffect();
				badeffect();
				badeffect();
				losehp(rnd(u.ulevel * 3), "suffocating on farting gas", KILLED_BY);
			}

			if (!rn2(20)) increasesanity(1);

		}

	}

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

		int blindinc = rnd(20);
		u.ucreamed += blindinc;
		make_blinded(Blinded + (long)blindinc, FALSE);
		if (!rn2(5)) increasesanity(rnd(10));
		u.cnd_crappingcount++;
		if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();

	}
	if (mdat->msound == MS_FART_NORMAL && mtmp->crapbonus && (rn2(1000) < mtmp->crapbonus) && !um_dist(mtmp->mx, mtmp->my, 1) && !mtmp->mpeaceful)
	{
		pline("Using %s %s butt, %s produces beautiful noises and craps right in your %s.", mhis(mtmp), mtmp->female ? "sexy" : "ugly", mon_nam(mtmp), body_part(FACE) );

		int blindinc = rnd(50);
		u.ucreamed += blindinc;
		make_blinded(Blinded + (long)blindinc, FALSE);
		if (!rn2(5)) increasesanity(rnd(10));
		u.cnd_crappingcount++;
		if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();

	}
	if (mdat->msound == MS_FART_LOUD && mtmp->crapbonus && (rn2(400) < mtmp->crapbonus) && !um_dist(mtmp->mx, mtmp->my, 1) && !mtmp->mpeaceful)
	{
		pline("Using %s %s butt, %s produces disgusting noises and craps right in your %s.", mhis(mtmp), mtmp->female ? "sexy" : "ugly", mon_nam(mtmp), body_part(FACE) );

		int blindinc = rnd(100);
		u.ucreamed += blindinc;
		make_blinded(Blinded + (long)blindinc, FALSE);
		if (!rn2(5)) increasesanity(rnd(10));
		u.cnd_crappingcount++;
		if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();

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
	if( (is_covetous(mdat) || mtmp->egotype_covetous) && (!rn2(10) || (mdat == &mons[PM_AT_RAINKID] && !rn2(3)) || (mdat == &mons[PM_TEA_HUSSY] && !rn2(3)) || (CovetousnessBug || u.uprops[COVETOUSNESS_BUG].extrinsic || have_covetousstone() ) ) ) (void) tactics(mtmp);

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
            if (!mtmp->mfrenzied) mtmp->mpeaceful = 1;
            set_malign(mtmp);
        } else {
            pline("I said %ld!", gdemand);
            mtmp->mspec_used = 1000;
        }
    }

	if (monsndx(mdat) == PM_SINGPIR && !rn2(20)) {
		if (!(t_at(mtmp->mx, mtmp->my))) {
			maketrap(mtmp->mx, mtmp->my, SHIT_TRAP, 0);
		}
	}

	/* monster noise trap: some of these noises have effects, might add others in future --Amy */
	if ((MonnoiseEffect || u.uprops[MONNOISE_EFFECT].extrinsic || have_monnoisestone()) && !rn2(250) && !mtmp->mpeaceful && (distu(mtmp->mx, mtmp->my) <= BOLT_LIM * BOLT_LIM) ) {
		switch (mdat->msound) {

			case MS_PRIEST:
				verbalize(rn2(2) ? "Hoyo hoyo!" : "Wololo");
				adjalign(-(mtmp->m_lev + 1));
				You_feel("less faithful!");
				break;
			case MS_VAMPIRE:
				verbalize("I vill suck you dry!");
				if (!rn2(3)) {
					You("are too scared to move.");
					nomul(-5, "scared by a vampire", TRUE);
					nomovemsg = 0;
				}
				break;
			case MS_WERE:
				if (canseemon(mtmp))
					pline("%s throws back %s head and lets out a blood curdling %s!", Monnam(mtmp), mhis(mtmp), mtmp->data == &mons[PM_HUMAN_WERERAT] ? "shriek" : "howl");
				else You_hear("a blood curdling sound!");
				make_numbed(HNumbed + rnz((2 * level_difficulty()) + 1), FALSE);
				break;
			case MS_BARK:
				if (canseemon(mtmp)) pline("%s %s!", Monnam(mtmp), rn2(2) ? "growls" : "barks");
				else You_hear("a barking sound!");
				break;
			case MS_MEW:
				if (canseemon(mtmp)) pline("%s %s!", Monnam(mtmp), rn2(2) ? "growls" : "hisses");
				else You_hear("a growling sound!");
				break;
			case MS_ROAR:
				if (canseemon(mtmp)) pline("%s roars!", Monnam(mtmp));
				else You_hear("a loud roar nearby!");
				if (!rn2(3)) make_feared(HFeared + rnz((2 * level_difficulty()) + 1), TRUE);
				break;
			case MS_SQEEK:
				if (canseemon(mtmp)) pline("%s squeaks!", Monnam(mtmp));
				else You_hear("a squeak nearby!");
				break;
			case MS_SQAWK:
				if (canseemon(mtmp)) pline("%s squawks!", Monnam(mtmp));
				else You_hear("a squawk nearby!");
				break;
			case MS_HISS:
				if (canseemon(mtmp)) pline("%s hisses!", Monnam(mtmp));
				else You_hear("a hiss nearby!");
				break;
			case MS_BUZZ:
				if (canseemon(mtmp)) pline("%s buzzes!", Monnam(mtmp));
				else You_hear("a buzzing nearby!");
				break;
			case MS_GRUNT:
				if (canseemon(mtmp)) pline("%s grunts!", Monnam(mtmp));
				else You_hear("a grunting nearby!");
				break;
			case MS_NEIGH:
				if (canseemon(mtmp)) pline("%s neighs!", Monnam(mtmp));
				else You_hear("a neighing nearby!");
				break;
			case MS_WAIL:
				if (canseemon(mtmp)) pline("%s wails frighteningly.", Monnam(mtmp));
				else You_hear("a nearby wail.");
				break;
			case MS_GURGLE:
				if (canseemon(mtmp)) pline("%s gurgles.", Monnam(mtmp));
				else You_hear("a nearby gurgle.");
				break;
			case MS_BURBLE:
				if (canseemon(mtmp)) pline("%s burbles.", Monnam(mtmp));
				else You_hear("a nearby burble.");
				break;
			case MS_FART_QUIET:
			case MS_FART_NORMAL:
			case MS_FART_LOUD:
				if (canseemon(mtmp)) pline("%s makes woman noises.", Monnam(mtmp));
				else You_hear("nearby woman noises.");
				break;
			case MS_IMITATE:
				if (canseemon(mtmp)) {
					pline("%s seems to imitate you.", Monnam(mtmp));
					make_confused(HConfusion + rnz((2 * level_difficulty()) + 1), FALSE);
				}
				break;
			case MS_SHEEP:
				if (canseemon(mtmp)) pline("%s baaaas!", Monnam(mtmp));
				else You_hear("a nearby 'baaaa!'");
				break;
			case MS_CHICKEN:
				if (canseemon(mtmp)) pline("%s clucks!", Monnam(mtmp));
				else You_hear("a clucking sound!");
				break;
			case MS_COW:
				if (canseemon(mtmp)) pline("%s bellows!", Monnam(mtmp));
				else You_hear("a bellowing sound!");
				break;
			case MS_LAUGH:
				if (canseemon(mtmp)) pline("%s laughs at you!", Monnam(mtmp));
				else You_hear("someone laughing at you!");
				if (!rn2(10)) {
					pline("You are trembling!");
					u.tremblingamount++;
				}
				break;
			case MS_MUMBLE:
				if (canseemon(mtmp)) pline("%s mumbles incomprehensibly.", Monnam(mtmp));
				else You_hear("an incomprehensible mumbling.");
				break;
			case MS_SHOE:
				verbalize("We absolutely want to kick you. Hopefully you enjoy pain!");
				break;
			case MS_STENCH:
				if (canseemon(mtmp)) pline("%s sprays %sself with perfume.", Monnam(mtmp), mhim(mtmp));
				else You_hear("the sound of a spray box.");
				break;
			case MS_BOAST:
				if (canseemon(mtmp)) pline("%s boasts!", Monnam(mtmp));
				else You_hear("someone boasting!");
				break;
			case MS_SEDUCE:
				if (canseemon(mtmp)) pline("%s looks at you seductively!", Monnam(mtmp));
				else You_hear("a seductive voice!");

				if (!rn2(100)) {
					char bufof[BUFSZ];
					bufof[0] = '\0';
					steal(mtmp, bufof, FALSE);
				}

				break;
			case MS_ARREST:
				verbalize("I will hold you, you for asshole!");
				break;
			case MS_SPELL:
				if (canseemon(mtmp)) pline("%s casts furiously!", Monnam(mtmp));
				else You_hear("a furious chant!");
				break;
			case MS_NURSE:
				verbalize("Hold still and let me JAM this needle into you.");
				increasesanity(1);
				break;
			case MS_SOLDIER:
				verbalize("My commander will be very pleased if I kill you!");
				break;
			case MS_RIDER:
				verbalize("You're going down, War.");
				break;
			case MS_VICE:
				verbalize("I shall taint your soul before I send you to Hell.");
				break;
			case MS_DOUGLAS_ADAMS:
				{
					static const char *da_msgs[] = {
						"42",
						"It's a nice day today!",
						"Have a Pan Galactic Gargle Blaster?",
						"Time is relative. Lunchtime doubly so.",
						"This is some strange usage of the word 'safe' that I wasn't aware of.",
					};
					if (mtmp->data == &mons[PM_MARVIN]) {
						verbalize("Life, loathe it or ignore it, you cannot like it.");
					} else if (mtmp->data == &mons[PM_DEEP_THOUGHT]) {
						verbalize("6*9 = 42");
					} else if (mtmp->data == &mons[PM_EDDIE]) {
						verbalize("I'm feeling just great, guys!");
					} else {
						verbalize("%s", da_msgs[rn2(SIZE(da_msgs))]);
					}
				}
				break;
			case MS_PUPIL:
				verbalize("No, I do not have the homework today and you are a shitty teacher anyway.");
				break;
			case MS_TEACHER:
				verbalize("You get 10 hours of detention for your misbehavior!");
				break;
			case MS_PRINCIPAL:
				verbalize("Now come here, little rascal! We're gonna have a SERIOUS talk about the things you did!");
				break;

			default: /* nothing happens */
				break;

		}
	}

	if ((mdat->msound == MS_CONVERT || mtmp->egotype_converter) && !Race_if(PM_TURMENE) && !Race_if(PM_HC_ALIEN) && !mtmp->mpeaceful && (distu(mtmp->mx, mtmp->my) <= BOLT_LIM * BOLT_LIM) && !(uarmh && uarmh->oartifact == ART_JAMILA_S_BELIEF) && !rn2(10)) {

		static const char *conversion_msgs[] = {
			"Kafir! Yuece Tanri sizi sevmez, ve sonra doenuestuermek yoksa cezalandirilacaktir!",
			"Kafir, Allah'a doenuestuermek ya da oel!",
			"Kafir, kutsal Tanri bu boelgede emanetler en kadirdir. Ve bunu degistirmek olmaz!",
			"Kafir, sizin yollariniz sapkin vardir! Islam sizi doenuestuermek zamanidir.",
			"Kafir, sen domuz pirzola yeme suc islemis! Allah sana cok kizgin!",
			"Allahu Akbar! Allahu Akbar!",
			"Kafir, Allah bueyuektuer ve seni yok eder!",
			"Kafir! Kac kere zaten gavur dedin? Sen inanc degistirmek icin ya da Tanri ilahi ceza kesin olacak var!",
			"Allah senin gibi kafirler sevmez ve cok zalim bir sekilde sizi cezalandiracaktir!",
			"Bizim kutsal cami kafirler tarafindan yerle bir olma tehlikesiyle karsi karsiya! Bueyuek Tanri bize yardim ve ilahi asker goendermesi gerekir!",
			"Kafir, Allah bueyuek ve gueclue oldugunu! Ona inanmaya baslarlar yoksa, aci olacak!",
			"Allah onlari oelduererek ve kutsal ateste kendi cesetlerini yakarak buetuen kafirleri cezalandiracaktir.",
			"Kafir, Allah beni tuvalete bok sesleri ueretmek icin izin! Eger o saygi yoksa sonsuza aci olacaktir!",
			"Tek bir gercek dindir ve Tuerkiye cumhurbaskani, onu buetuen duenyaya yayacak bilge bir adamdir! Allah bueyuek!",
			"Kafir! Kutsal topraklarda hicbir sey kaybetmedin! ABD'ye ya da nereye giderseniz gidin!",
			"Inanilmaz olan, senin evin benim tarafimdan yere serilecek.",
			"Inanilmaz, nihayet askerlerini uelkemden mi cekiyorsun yoksa sinirlenmek zorunda miyim?",
			"Inanmasiz, silahim seni vurmak uezere! Sana daha iyi bakarim!",
			"Haha, sen inandin, saldirmami kaldiramazsin!",
			"Inanli olmayan, sana simdi pusu kurup seni uyariyorum. Yani beni yine de durdurma sansin yok.",
			"Kafir! Hemen buradan cik, buguen camiye gitmem ve dua etmeliyim!",
			"Kafir! seni ve iman eden askerlerinizi yok edecegiz!",
			"Inanilmazsin, benden uzak dur yoksa bir talihsizlik olur!",
			"Inkar edenler sadece korkaklar, asla kendinizi adil bir doevuese sokmazsiniz! Simdi sikici el bombalari kullanmaliyim!",
			"Gercekten sadece buguen sinirlenmek istiyor musun, inandin mi? Sanirim simdi agir topcuyla gelecegim.",
			"Inanilmaz olan, Islam'in tek gercek din oldugunu itiraf etme zamani.",
			"Inancsiz, pes et! Islam'in kutsal askerleri simdi sehrinizi istila edecek!",
			"Inanilmaz, yavas yavas oelmesi gerektigini duesuenmueyor musun? Vuecudun yine de degersiz!",
			"Bu bir hacli seferi olacak mi, inandin mi? Kutsal Allah'in gazabini zorla cagirir misin?",
			"Kafirlerin sizin talihsizlikleriniz icin yiyeceksiniz ve Allah sahsen sizi atesin uezerinde bas asagi kizdiracak!",
			"Lanet olasi inanmayan koepekler, kutsal topraklardan disari cik! Hepiniz inkar ediyorsunuz ve Allah sizi sevmiyor, duyuyor musunuz?",
			"Tamam, inandin. Yeterli olan yeter! Herkese, kafirleri hemen yok edin!",
			"Ben cihad, kutsal savas icin cagiriyorum! Kafirler, oelueme savasiyoruz!",
			"Hepinizi inatci koepekler yapacagiz!",
			"Inanilmaz, ama simdi yeterli! Simdi Allah'in gazabi sana vuracak!",
			"Inkar edenler iki seceneginiz var. Hemen Islam'a itiraf edin ya da yueksek savas botlarim ciddi yaralar acacak.",
			"Oyuncu bir guevensiz! Onu Islam'a doenuestuerme zamani.",
			"Inkar edenler yenilir. Derhal Islam'in tek gercek din oldugunu ya da kutsal Allah'in gazabini hissettirdigini itiraf edin!",
			"Oyuncu ve adamlari... pah. Buetuen inanmayanlar. Sadece askerlerim kimin dine inanmalari gerektigini biliyor!",
			"Hahahaha Hahahaha! Tuerk savas botlari bedeninizi yok etti, siz kafirler!",
			"Nihayet uelkemi terk ediyor musun, inanmiyor musun?",
			"Kafir! Ha ha! Simdi ya Islam'a ya da oeluersuen.",
			"Kafirler, senin kokulu fabrikalarin simdi yikilacak!",
			"Inanli olmayan, Allah size uecuencueluek eder ve sonra sizi tehlikede yakar.",
			"Kafir! Simdi kac kere seni guevensiz aradim?",
			"Sonunda inanclarinizi degistirmelisiniz, siz kafirler, aksi takdirde bir firtina olacak!",
			"Gizli silahimi nerede sakladigimi hatirladim! Hehehe, simdi size goesteriyorum, siz kafirler.",
			"Kutsal cami kafirlerin tehdidi altindadir! Allah, bize yardim et ve onlari yuvarlamalarini engelle!",
			"Tuem erkekler ve kadinlar saldirmak icin! Kafirleri derhal uzaklastirin!",
			"Buetuen kafirlere goere kutsal Islam, bu topraklarin kalintilarinin cogunda hakimdir.",
			"Kafirler buguen emanetlerimizi tekrar yakalamayacaklar!",
			"Yalanci medyanin fikri yok! Islam tek gercek din ve kafirler bunu degistirmeyecek!",
			"Inkar edenler beni kandirdi, ama sizi Islam'in tek gercek din olduguna inandiricam.",
			"Aaaaaaaaaaaah! Hayir! Kafirlerimiz camimizi gercekten yok etti! Simdi kim Islam'i yaymali?",
			"Oh hayir, simdi vazgecebilirim, sadece bu kafirler yuezuenden...",
			"Inkar edenler, engizisyonumdan kacamaz, itiraf edemez veya oelebilirler.",
			"Allah seni yok edecek, inandin!",
			"Neden oyuna daha fazla inanmayan asker getiriyorsun?",
			"Siz kafirsiniz, dininiz bos ve gecersizdir!",
			"Kafir, bas oertuesue baskinin bir sembolue degil! Onu kucaklamali ve bir tane de giymelisin yoksa Allah seni cezalandiracak!",
			"Kafir, bir kadinin tuem kafasini kamuya goestermemesi gerektigini asla anlamayacaksiniz. Bueyuek Allah, bir bas oertuesue her zaman giyilmesi gerektigini oegretir.",
			"Kafirlerin beni tuvalete kadar takip etmelerine izin vermeyecegim ve kicimin uerettigi erotik kirma seslerini dinleyemem. Yine de yaparsan, doevues botu topuklari bacaklarini kemige cizecek ve seni oelduerecek.",
			"Benim siyah bayan sandaletler senin gibi bir kafir tarafindan asla hasar goermeyecek!",
			"Senin yayla botlarimin altindaki kafirleri ezecegim!",
			"Bir kafirle evlenmeyecegim, Allah'a iman eden birisini sececegim ve ic camasirlarimi, coraplarimi ve sandaletlerimle savasirken disi catlak seslerimi dinlemesine izin vererek onu memnun edecegim.",
			"Cogu zaman tuvalette saatlerce catlak sesler ueretiyorum, ama sadece Allah'in inanclilari onlari dinleyebilir. Gizlice iceri girerlerse kafirler oelecek.",
			"Hic kafir bir kadini cok guezel bulabilir! Her zaman kafa oertueleri giymeliyiz!",
			"Allah kafa bezinin bize oezguerluek verdigini soeyler, cuenkue saf olmayan kafirlerin pis bakislarini uzak tutar!",
			"Ben seksi yueksek topuklu botlar giyiyorum cuenkue ben bir harlotum, ama ben sadece aydinlanmis olanlar icin calisiyorum, bu yuezden de bir bas kumasina ihtiyacim var! Hic kafir beni beceremez!",
			"Eger herhangi bir aldatmaca kafamin uestuenue cikarmaya calisirsa, onu oelduermek icin erkekleri ise alirim!",
			"Bueyuek Allah, kadinlara basoertuesue takmayan firmalarin ateste oelecegini soeylueyor!",
			"Tabii ki kuecuek okul kizlari da bir bas oertuesue giymelidir! Cok fazla kendilerini goesterirlerse, bazi iyi olmayan cocuklar da onlara bakar ve Allah her ne pahasina olursa olsun bunu engellemelidir!",
			"Kendimizi kafir toplumlara entegre etmek zorunda degiliz, onlari Allah adina sollamak zorundayiz! Cuenkue bu heretiklerin Islam'a doenuesmesi gerekiyor!",
			"Hic kimse kirli domuz eti yiyemez! Allah onu dizanteri ile yemeye cueret eden herkesi cezalandiracak.",
			"Bir kafir adamin Mueslueman bir bayanla konusmasi tamamen duesuenuelemez. Bu, kafa bezine sahip olmamizin nedenlerinden biri, cuenkue Yuece Allah, bizi zarardan koruyor.",
			"Biz, buezguelue ailelerin barok kalelerde yer bulduklari icin bez giyiyoruz, cuenkue kafir aileler ilk geldiler bile beklemek zorundalar. Bueyuek Allah, degersiz bir Alman azinligi bekler!",
			"Biz bez harbots basimiza yueksek topuklularimizla asyali tekmeyi oegrendik, bu sayede kutsal Allah adina kafirlerin ezmelerini ve parcalarini kirabiliriz.",
			"Cok fazla kafir adam var ve bu yuezden kafamizi hicbir zaman halka acik goesteremiyoruz. Ancak, sadece kadinlara yoenelik spor merkezine girersek, bas bezini cikaririz cuenkue Allah, kadinlarin asla tecavuez etmeyecegini bilir. Lezbiyenlik yoktur.",
			"Allah, basoertuesue takmayan kafir Avrupali kadinlardan nefret eder. Muesluemanlarin ayri bir banyosunun yapilmasini istiyoruz cuenkue bizi o boktan fahiselerin tuvalete gitmesini istemiyoruz.",
			"Kafamizi takmiyorken bizi goeren kafir adama vay canina. Yuece Allah, ona asiri oenyargi ile yaklasir.",
			"Bazen inatci erkekleri kandirmak icin paragraf botlari giyerim. Benimle sevisebileceklerini duesuenuerlerse, kiralik suikastcilar onu Allah'in adiyla sessizce kaldiracaklar.",
			"Domuz cocuk bahcesinde servis edilirse yanlistir! Buetuen bu kafir egitmenler yakinda Allah'a iman eden insanlarla degistirilecek.",
			"Evet, kafirler bize yuezuemuezue oerten kiyafetler vermedikce yuezme ya da spora katilmak zorunda olmadigimiz dogrudur. Allah bize, inkarcilarin yaptigi kurallarin bizim kurallarimiz olmadigini ve takip edilmesine gerek olmadigini oegretir.",
			"Aptal Alman uelkesi kendisini yok edecek ve bu kafirleri yavasca kaldirarak Allah'a yardim edecegiz. Daha sonra Berlin, Yeni Istanbul'a yeniden adlandirilacaktir.",
			"Neden kizlari kizdirmaya calisiyorsun? Sizi durdurmak icin herkesi basoertuesue takmaya zorlayacagiz!",
			"Ne, sen kafir, cinsel popoyu ihale popomun tuvalete yaptigi kadin sesine cekti mi? Delirmis olmalisin!",
			"Ne, sen kafir kafami sadece beni kizdirmak mi istiyorsun? Allah, senin icin oeluem cezasi olacagini soeyler!",
			"Sen kafir doenuestueruelecek, ve eger bir kadin iseniz de bir kafa bezi giymek zorunda kalacaksiniz!",
			"Bueyuek Allah, bu kadar genisletilmis egrinin, en iyi varyant oldugunu kabul etmeyen herkesi cezalandiracak! Eger sporkun daha iyi oldugunu duesuenueyorsaniz, sen lanet bir kafirsin ve diri diri diri yakilacaksin!",
			"Bu oyunun aptal bir versiyonunu oynuyorsun, seni kafir! Yuece Allah, benim icin bir oyuncu vur!",
			"Kuefuer edersiniz, Isvecli adamin yapmasi gereken varyanti oynamalisiniz, cuenkue canavarlar soykirimin yazilarini okumalarina izin vererek cok daha dengeli!",
			"Oynayabilecegin cok daha iyi bir cesit var, zindanlar ve ejderhalar hack, ama sen eski heretik bu boku oynamak zorunda. Allah size yildirim carpacaktir.",
			"Neden hala salak oyun oynuyorsun? Doert kana gecin ya da cok gueclue Allah sizi parcalara ayirir!",
			"Allah, varyantini yuezen goezbebeklerini calmayan her kafirin kisisel bilgisayarini kizdirir.",
			"Onun varyanti seytan degildir, sen kafir! Simdi oyna ya da bu senin sonun!",
			"Bana 2000 numara altin parca, yoksa oyunun coekmesine sebep olacaksin, seni kafir!",
			"Sen seytani hain! Bu ne cueret? Intikam seni guevende, kafir!",
			"Hicbir kafirin direnemeyecegi cok kadin tuvalet sesleri ueretebilirim.",
			"Tuvalete yapabilecegim kadin sesler basmak ve sicramak. Korktun mu, sen kafir misin?",
			"Ne, beni cekici buldugunu soeylueyorsun? Bu olamaz. Derhal bas borumu tekrar giyecegim.",
			"Sert cekirdekli uzaylilar uzayli degil! Onlarin bedenlerini kafir erkek bakislarindan saklamak icin pecelerini giyiyorlar!",
			"Kadinlara karsi kadinlari kuran feminist hakli, erkek kafirleri her zaman masum kadinlara bakiyor! Koruma icin basoertuesue ve pece giymeliyiz!",
			"Ne, ben guezel oldugumu mu soeylueyorsun? Bu tahammuel edilemez. Hicbir kafir, bir Islam kadinini guezel olarak algilayamaz! Cildimi suya batiracak ve basoertuesue ve tam oertue giyecegim!",
			"Benim guezel savas botlari bacaklarinizi tam uzunlukta kesecek ve oelueme kadar kanayacak, kafir cehennemden ortaya cikmayacak!",
			"Bana ne diyorsun seytan kadin? Ben oeyle degilim! Ben Islam'in bueyuek sorusturmacisiyim ve kutsal Allah adina seni kafir edecegim!",
			"Plato kiz botlarimin uezerinden gecmemelisin! Senin gibi bir kafir asla evlenmeyecegi icin adanmislikla diz coekmen gerekiyor!",
			"Benim gri spor ayakkabim hicbir sekilde oezel degil, seksi bulursaniz sizi havaya ucurur. Beni cekici buldugum kafirler bas barimla bogulurlar.",
			"Ben farkli degilim cuenkue bir bas oertuesue giyiyorum! Beni ayrimcilik yapmamalisin, kafir demek istiyorsun!",
			"Siz kafirler bizi kabul etmeli ve dininize uygun olmali! Kapsayici olmak istiyorsaniz, oegretmenlerin ve polis kadinlarinin ve digerlerinin halka acik bir bez kullanmasina izin veriyorsunuz!",
			"Beni 'basoertuesue kiz' sluruna duesuerme, seni lanet olasi kafir! Bu benim kimligimin bir parcasi ve bunu kabullenmek icin kanunla zorlanacaksin!",
			"Kafa bezi bize fransiz sigaralari gibi oezguerluek veriyor! Siz kafirler asla anlayamayacaksiniz cuenkue Tanri'ya inanmiyorsunuz!",
			"Yeme tabagim sacmalik degil! Sen pis kafir sadece domuz eti kirli oldugunu ve hic kimsenin tarafindan yenilmeyecegini anlamiyorum! Cuenkue yaparsan kansersin!",
			"Imam, kafir kueltueruenuen hicbir seyin degersiz oldugunu soeylueyor. Bu nedenle aptal kurallariniza uymak zorunda degiliz ve okul da ise yaramiyor, cuenkue birkac yil icinde hepiniz asimile olacaksiniz.",
			"Yuezmeye katilmak zorunda degilim! Kafir guecuen beni zorlamak istiyor ama kutsal Islam her zaman bir bas oertuesue giymem gerektigini soeylueyor ve bu da aptal yuezme derslerine uymuyor! Politikacilarina bogma tilsimindan bogulmalarini soeyle!",
			"Acil bir durum varsa, oence Allah'ta muesluemanlar ve inananlar kurtarilmalidir! Kafirler daha bueyuek iyilik icin kurban edilebilir!",
			"Inanilmaz lanet olasi guezel Islam'in duenyayi yoenetecegini anlamazsin.",
			"Ne, Almanya'nin alternatifi mueslueman avlamak mi istiyor? Yuece Allah adina, bu kafirleri dogrudan cehenneme goenderecek, cocuklarini oelduerecek ve kadinlarini koele tueccarlarina satacagiz.",
			"Kafa bezi sadece bir bez parcasi degildir! Onu takariz cuenkue bizi Allah'in kutsal mueminleri olarak tanimlar! Sadece kafirlerin anlamadigi icin hicbir anlam ifade etmez!",
			"Bir Yahudi dini semboluenue giydigi icin saldiriya ugrarsa, kendi hatasidir. Biz mueslueman bir kafa bezi giymek icin bize saldiran kimseyi asla kabul etmeyecegiz, cuenkue biz kafir topuklularimizla bir asyali tekmelemizi dogrudan kafir somunlarina yerlestirecegiz.",
			"Siz kafirsiniz, Allah'a doenmelisiniz cuenkue o zaman cennette doksan dokuz genc kadin var! Eger yapmazsan, cehennemin kirmizi seytanlari acimasizca iskence edecek!",
			"Halkimizin kutsal topraklara seyahat etmesine izin verilmemesi bir hakaret! Bueyuek Tanri, bu karardan sorumlu olan buetuen kafirleri cezalandirir!",
			"Kongremizdeki kadinlar suclu degil! Ancak baskan kirli bir kuefuer ve Tanri sahsen ona yildirim carpacak!",
			"Bir bildirimin ardindan geri doendueguemuez icin neden uezuelmemiz gerektigini duesuenueyorsunuz? Siz heretik, istediginizi aldiniz, cuenkue yuece Tanrimiz affetmez!",
			"Yasli ihtiyar bizim icadi olan doener kebabi yiyorsun, ama dinimiz hakkinda yalan soeylueyorsun! Tanrimiz seni mahvedecek!",
			"Ceviri makinesi kirli kafirler tarafindan isletiliyor, cuenkue tanri adina varsayilan ceviriyi degistirdiler!",
			"Benim konusmalarimi zaten anlamiyorsunuz, bu yuezden irkinizin tamamen ortadan kaldirilmasi icin cagri yapabilirim, sizi allahimiza inanmayan insafsizsiniz.",
			"Temizlik Allah'in guenue buetuen kafirler cezalandirmak ve ilk bu sapkin kadin olacak!",
			};

		verbalize("%s", conversion_msgs[rn2(SIZE(conversion_msgs))]);
		u.cnd_conversioncount++;
		if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();

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

			if (uarmf && uarmf->oartifact == ART_RUEA_S_FAILED_CONVERSION && rn2(20)) goto convertdone;

			You_feel("less faithful!");

			if (u.ualign.record < -20 && !rn2(Race_if(PM_KORONST) ? 10 : 100) && (sgn(mtmp->data->maligntyp) != u.ualign.type) ) { /* You have been converted! */

				if(u.ualignbase[A_CURRENT] == u.ualignbase[A_ORIGINAL] && mtmp->data->maligntyp != A_NONE) {
					You("have a strong feeling that %s is angry...", u_gname());
					pline("%s accepts your allegiance.", align_gname(sgn(mtmp->data->maligntyp)));

					/* The player wears a helm of opposite alignment? */
					if (uarmh && uarmh->otyp == HELM_OF_OPPOSITE_ALIGNMENT)
						u.ualignbase[A_CURRENT] = sgn(mtmp->data->maligntyp);
					else
						u.ualign.type = u.ualignbase[A_CURRENT] = sgn(mtmp->data->maligntyp);
					u.ublessed = 0;
					flags.botl = 1;

					You("have a sudden sense of a new direction.");
					/* Beware, Conversion is costly */
					change_luck(-3);
					u.ublesscnt += (ishaxor ? 150 : 300);
					adjalign((int)(u.ualignbase[A_ORIGINAL] * (u.alignlim / 2)));

				} else {
					u.ugangr += 3;
					adjalign(-25);
					godvoice(sgn(mtmp->data->maligntyp), "Suffer, infidel!");
					change_luck(-5);
					(void) adjattrib(A_WIS, -2, TRUE, TRUE);
					angrygods(sgn(mtmp->data->maligntyp));

				}
			}
			adjalign(-(mtmp->m_lev + 1));

			if (!rn2(10)) badeffect();

		}

	}

convertdone:

	if ((mdat->msound == MS_HCALIEN || mtmp->egotype_wouwouer) && !Race_if(PM_TURMENE) && !Race_if(PM_HC_ALIEN) && !mtmp->mpeaceful && (distu(mtmp->mx, mtmp->my) <= BOLT_LIM * BOLT_LIM) && !(uarmh && uarmh->oartifact == ART_JAMILA_S_BELIEF) && !rn2(15)) {

		static const char *hcalien_msgs[] = {
			"Wouwou.",
			"Wouuu.",
			"Www-wouwou.",
			"Wwouwwouww.",
			"Wowou.",
			"Wwwouu.",
		};
		verbalize("%s", hcalien_msgs[rn2(SIZE(hcalien_msgs))]);
		u.cnd_wouwoucount++;
		if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();

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

			badeffect();
			stop_occupation();
		    if (!obsidianprotection()) switch (rn2(17)) {
		    case 0:
		    case 1:
		    case 2:
		    case 3: make_confused(HConfusion + rnd(100 + (mtmp->m_lev * 5) ), FALSE);			/* 40% */
			    break;
		    case 4:
		    case 5:
		    case 6: make_confused(HConfusion + (2L * rnd(100 + (mtmp->m_lev * 5) ) / 3L), FALSE);		/* 30% */
			    make_stunned(HStun + (rnd(100 + (mtmp->m_lev * 5) ) / 3L), FALSE);
			    break;
		    case 7:
		    case 8: make_stunned(HStun + (2L * rnd(100 + (mtmp->m_lev * 5) ) / 3L), FALSE);		/* 20% */
			    make_confused(HConfusion + (rnd(100 + (mtmp->m_lev * 5) ) / 3L), FALSE);
			    break;
		    case 9: make_stunned(HStun + rnd(100 + (mtmp->m_lev * 5) ), FALSE);			/* 10% */
			    break;
		    case 10: make_numbed(HNumbed + rnd(100 + (mtmp->m_lev * 5) ), FALSE);			/* 10% */
			    break;
		    case 11: make_frozen(HFrozen + rnd(100 + (mtmp->m_lev * 5) ), FALSE);			/* 10% */
			    break;
		    case 12: make_burned(HBurned + rnd(100 + (mtmp->m_lev * 5) ), FALSE);			/* 10% */
			    break;
		    case 13: make_feared(HFeared + rnd(100 + (mtmp->m_lev * 5) ), FALSE);			/* 10% */
			    break;
		    case 14: make_blinded(Blinded + rnd(100 + (mtmp->m_lev * 5) ), FALSE);			/* 10% */
			    break;
		    case 15: make_hallucinated(HHallucination + rnd(100 + (mtmp->m_lev * 5) ), FALSE, 0L);			/* 10% */
			    break;
		    case 16: make_dimmed(HDimmed + rnd(100 + (mtmp->m_lev * 5) ), FALSE);			/* 10% */
			    break;
		    }
			if (!rn2(20)) increasesanity(rnd(10 + (mtmp->m_lev * 2) ));
		}

	}

	if (mdat->msound == MS_SHOE && !mtmp->mpeaceful && evilfriday && !rn2(50) && (distu(mtmp->mx, mtmp->my) <= 12)) {
		pline("%s uses %s sweaty inlay, causing you to become unconscious from the stench!", Monnam(mtmp), mhis(mtmp));
		nomul(-(rnd(5) + (mtmp->m_lev / 4) ), "smelling sweaty inlays", TRUE);
	}

	if ((mdat->msound == MS_STENCH || mtmp->egotype_perfumespreader) && !Role_if(PM_HUSSY) && !(youmonst.data->msound == MS_STENCH) && !mtmp->mpeaceful && (distu(mtmp->mx, mtmp->my) <= BOLT_LIM * BOLT_LIM) && !rn2((mdat == &mons[PM_NICE_AUNTIE_HILDA]) ? 5 : (mdat == &mons[PM_AUNT_ANITA]) ? 5 : 20)) {
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
		u.cnd_perfumecount++;
		if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();

		if (rn2(10) && uarmh && itemhasappearance(uarmh, APP_GAS_MASK) ) {
			pline("But the gas mask protects you from the effects.");
		} else {

			badeffect();
			if (rn2(2) || Role_if(PM_EMERA)) increasesanity(rnz(20 + mtmp->m_lev));
			stop_occupation();
		}
	}

	/* Monsters with MS_BONES can rattle. If this causes you to snap out of a longer paralysis, more power to you :D */
	if (mdat->msound == MS_BONES && !mtmp->mpeaceful && (distu(mtmp->mx, mtmp->my) <= BOLT_LIM * BOLT_LIM) && !rn2(100)) {
		pline(FunnyHallu ? "%s plays the xylophone!" : "%s rattles noisily!", Monnam(mtmp));

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
			&& !rn2( (uarmh && itemhasappearance(uarmh, APP_NARROW_HELMET) ) ? 4 : 20)) {
		struct monst *m2, *nmon = (struct monst *)0;

		if (canseemon(mtmp))
			pline("%s concentrates.", Monnam(mtmp));
		if (distu(mtmp->mx, mtmp->my) > BOLT_LIM * BOLT_LIM) {
			You(FunnyHallu ? "sense a fantastic wave of psychic energy." : "sense a faint wave of psychic energy.");
			goto toofar;
		}
		pline("A wave of psychic energy pours over you!");
		if ( (mtmp->mpeaceful || (Psi_resist && rn2(StrongPsi_resist ? 100 : 20) ) ) &&
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
				if (StrongHalf_spell_damage && rn2(2) ) dmg = (dmg+1) / 2;
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
					adjattrib(A_INT, -1, 1, TRUE);
					adjattrib(A_WIS, -1, 1, TRUE);
				}
				if (!rn2(100)) {
					(void) make_hallucinated(HHallucination + rnd(10) + rnd(monster_difficulty() + 1), TRUE, 0L);
					if (!Free_action || !rn2(StrongFree_action ? 100 : 20)) {
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
				if (!rn2(25)) increasesanity(rnz(5));

				stop_occupation();
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
	if (dmgtype(mdat, AD_SPC2) && !rn2((uarmh && itemhasappearance(uarmh, APP_NARROW_HELMET) ) ? 40 : 200) && Blind_telepat) {
		You(FunnyHallu ? "sense a fantastic psionic wave." : "sense a faint psionic wave.");
	}

toofar:

	/* try to attack the player's pets at range, if possible (from dnethack) */
	if (!DEADMONSTER(mtmp) && !mtmp->mpeaceful && !mtmp->mtame) {
		register struct monst *mtmp2 = mfind_target(mtmp, FALSE);
		if (mtmp2 && (mtmp2 != &youmonst) && (mtmp2 != mtmp)) {

			int res;
			res = mattackm(mtmp, mtmp2);
			if (res & MM_AGR_DIED) return 1; /* Oops. */
		}
	}

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
		!(mtmp->mtrapped && !nearby && select_rwep(mtmp, FALSE))) {
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
	   (is_wanderer(mdat) && !rn2(4)) || (Conflict && mtmp->mcansee && haseyes(mtmp->data) && (!resist(mtmp, RING_CLASS, 0, 0) || (StrongConflict && !resist(mtmp, RING_CLASS, 0, 0)) ) && !mtmp->iswiz
	   && !Is_blackmarket(&u.uz)
	   ) ||
	   (!mtmp->mcansee && !rn2(iswarper ? 2 : 8)) || mtmp->mpeaceful) {
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

	if (!mtmp->mpeaceful || (mtmp->mnum == PM_FRENZY_KANGAROO) || ((Conflict && !resist(mtmp, RING_CLASS, 0, 0)) || (StrongConflict && !resist(mtmp, RING_CLASS, 0, 0)) && !Is_blackmarket(&u.uz)) ) {

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
			pline("%s", fauxmessage());
			u.cnd_plineamount++;
			if (!rn2(3)) {
				pline("%s", fauxmessage());
				u.cnd_plineamount++;
			}
		}

	    if(inrange && mtmp->egotype_faker && !mtmp->mpeaceful && !rn2(20))
		{
			pline("%s", fauxmessage());
			u.cnd_plineamount++;
			if (!rn2(3)) {
				pline("%s", fauxmessage());
				u.cnd_plineamount++;
			}
		}

	    if(inrange && mtmp->data->msound == MS_BOSS && !mtmp->mpeaceful && !rn2(10))
	    pline("%s %s", Monnam(mtmp), bosstaunt());

	    if(inrange && (mtmp->data->msound == MS_STENCH || mtmp->data->msound == MS_WHORE || mtmp->data->msound == MS_SUPERMAN || mtmp->data->msound == MS_HCALIEN || mtmp->data->msound == MS_CONVERT || mtmp->data->msound == MS_FART_QUIET || mtmp->data->msound == MS_FART_NORMAL || mtmp->data->msound == MS_FART_LOUD) && (mtmp->data->geno & G_UNIQ) && !mtmp->mpeaceful && !rn2(10))
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

		verbalize("%s", pupil_msgs[rn2(SIZE(pupil_msgs))]);
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

		verbalize("%s", whore_msgs[rn2(SIZE(whore_msgs))]);

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

	    if(inrange && mtmp->data->msound == MS_SUPERMAN && !mtmp->mpeaceful && !rn2(mtmp->mnum == PM_BARTOLI_RETARD ? 25 : 5))

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

		verbalize("%s", superman_msgs[rn2(SIZE(superman_msgs))]);
		badeffect();
		increasesanity(rnz(50 + (mtmp->m_lev * 5) ));
		u.cnd_supermancount++;
		stop_occupation();
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

		verbalize("%s", principal_msgs[rn2(SIZE(principal_msgs))]);
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

		verbalize("%s", teacher_msgs[rn2(SIZE(teacher_msgs))]);

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
	boolean astralspecial = FALSE;
	struct permonst *ptr;
	struct monst *mtoo;
	schar mmoved = 0;	/* not strictly nec.: chi >= 0 will do */
	long info[9];
	long flag;
	int  omx = mtmp->mx, omy = mtmp->my;
	struct obj *mw_tmp;

	/* if a frenzied monster somehow becomes peaceful or tame, it immediately becomes hostile again (failsafe) --Amy */
	if (mtmp->mfrenzied && mtmp->mpeaceful) mtmp->mpeaceful = 0;
	if (mtmp->mfrenzied && mtmp->mtame) mtmp->mtame = 0;

	if (mtmp->data->mlet == S_TURRET || stationary(mtmp->data) || ((is_hider(mtmp->data) || mtmp->egotype_hide || mtmp->egotype_mimic) && (mtmp->mundetected || mtmp->m_ap_type == M_AP_FURNITURE || mtmp->m_ap_type == M_AP_OBJECT) ) ) {

		if (mtmp->meating) { /* special case here by Amy, otherwise they'd never finish eating! */
			mtmp->meating--;
			return 3;			/* still eating */
		}

		return(0); /* stationary turrets --Amy */
	}

	if (uwep && uwep->oartifact == ART_FLOWERBANE && !rn2(2) && mtmp->data->mlet == S_BAD_FOOD) return 0;

	/* mcalcmove() in mon.c allows sessile monsters to get turns. We still don't want them to move around though. --Amy */
	if (!mtmp->data->mmove && (!mtmp->egotype_speedster) && (!mtmp->egotype_racer) ) {

		if (mtmp->meating) { /* special case here by Amy, otherwise they'd never finish eating! */
			mtmp->meating--;
			return 3;			/* still eating */
		}

		return(0);
	}

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
	if( (ptr == &mons[PM_TENGU] || ptr == &mons[PM_CORONA_TASK_FORCE] || ptr == &mons[PM_CORONA_LICHEN] || ptr == &mons[PM_STALKING_CORONA_LICHEN] || ptr == &mons[PM_CORONA_SPORE_LICHEN] || ptr == &mons[PM_CORONA_COLONY_LICHEN] || ptr == &mons[PM_SUDDEN_TENGU] || ptr == &mons[PM_FAKE_NEWS_TENGU] || ptr == &mons[PM_LASTING_TENGU] || ptr == &mons[PM_TELEPORTER] || ptr == &mons[PM_POLITICAL_TENGU] || ptr == &mons[PM_EASTERN_TENGU] || ptr == &mons[PM_PHASING_TENGU] || ptr == &mons[PM_CHEERFUL_LEPRECHAUN] || ptr == &mons[PM_BLINK] || ptr == &mons[PM_VORPAL_BUNNY] || ptr == &mons[PM_KING_OF_PORN] || ptr == &mons[PM_PEARDUCK] || ptr == &mons[PM_UAE] || ptr == &mons[PM_CHEATY_SILVER_COIN] || ptr == &mons[PM_SPACEWARP_JELLY] || ptr == &mons[PM_NIGHTMARE_SHEEP] || ptr == &mons[PM_HELL_SHEEP] || ptr == &mons[PM_LOVECRAFT_SHEEP] || ptr == &mons[PM_INDIVIDUAL_WILL_O_THE_WISP] || ptr == &mons[PM_TELEHOBBIT] || ptr == &mons[PM_SPOOPY_GHOST] || ptr == &mons[PM_ANNOYING_SLEX_GHOST] || ptr == &mons[PM_SPRING_WOLF] || ptr == &mons[PM_DIMENSIONAL_SHAMBLER] || ptr == &mons[PM_MAGNET_ELEMENTAL] || ptr == &mons[PM_PHASE_KNIGHT] || ptr == &mons[PM_TELEPORTING_DEMON] || ptr == &mons[PM_BEAMING_UFO_PART] || ptr == &mons[PM_BEAMER] || mtmp->egotype_teleportself) && !rn2(25) && !mtmp->mcan &&
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

	if (ptr == &mons[PM_ROW_PERCENTD_COL_PERCENTD] && rn2(3)) {
		(void) rloc(mtmp, FALSE);
	      mmoved = 1;
	      goto postmov;
	}

	if (ptr == &mons[PM_CHEATY_SILVER_COIN] && rn2(3)) {
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

	if (isevilvariant && mon_has_amulet(mtmp) && !u.freeplaymode && Is_astralevel(&u.uz) ) {

		int altarx = 0, altary = 0;
		while (altarx++ < COLNO) {
			altary = 0;
			while (altary++ < ROWNO) {
				if (isok(altarx, altary) && (IS_ALTAR(levl[altarx][altary].typ))) {

					aligntyp astalign;

					astalign = levl[altarx][altary].altarmask & AM_MASK;
					if (astalign == AM_LAWFUL) astalign = 1;
					else if (astalign == AM_NEUTRAL) astalign = 0;
					else if (astalign == AM_CHAOTIC) astalign = -1;

					if (astalign == sgn(mtmp->data->maligntyp)) {

						gx = altarx;
						gy = altary;
						astralspecial = TRUE;

						if (mtmp->mx == altarx && mtmp->my == altary) { /* game over */
							u.youaredead = 1;
							pline("Oh no! Someone else managed to offer the Amulet of Yendor and obtained immortality. You have failed your mission and the game ends here.");
							killer_format = NO_KILLER_PREFIX;
							killer = "allowed someone else to offer the Amulet of Yendor to the gods";
						      done(DIED);
						      done(DIED);
						      done(QUIT); /* sorry but your game *really* ends here --Amy */

						}

						goto altarfound;

					}
				}
			}
		}

	}
altarfound:

	appr = mtmp->mflee ? -1 : 1;

	if (monsndx(ptr) == PM_SLEEPING_GIANT && !rn2(10)) mtmp->msleeping = 1;
	if (monsndx(ptr) == PM_SARSLEEPER && !rn2(10)) mtmp->msleeping = 1;
	if (monsndx(ptr) == PM_SLEEPY_GIRL && !rn2(10)) mtmp->msleeping = 1;
	if (monsndx(ptr) == PM_SLEEPY_WOMAN && !rn2(10)) mtmp->msleeping = 1;
	if (monsndx(ptr) == PM_SLEEPY_LADY && !rn2(10)) mtmp->msleeping = 1;
	if (monsndx(ptr) == PM_SLEEPING_ASIAN_GIRL && !rn2(10)) mtmp->msleeping = 1;
	if (monsndx(ptr) == PM_NOTHING_CHECKER_WHO_IS_CONFUSED) mtmp->mconf = 1;
	if (monsndx(ptr) == PM_BEER_BELLY) mtmp->mconf = 1;
	if (monsndx(ptr) == PM_LOONIE_BOSS) mtmp->mconf = 1;
	if (monsndx(ptr) == PM_BARTOLI_RETARD) mtmp->mconf = 1;
	if (monsndx(ptr) == PM_ERR_BOSS) mtmp->mconf = 1;
	if (monsndx(ptr) == PM_ERR) mtmp->mconf = 1;
	if (monsndx(ptr) == PM_STUPIDITY_AUTIST) mtmp->mconf = 1;
	if (monsndx(ptr) == PM_SEIZURE_FOOBACUS) mtmp->mconf = 1;
	if (monsndx(ptr) == PM_HIGHEST_PRIESTEST) mtmp->mconf = 1;
	if (monsndx(ptr) == PM_HAMMER_DRIVE) mtmp->mconf = 1;
	if (monsndx(ptr) == PM_PANCAKE_SPIRIT) mtmp->mconf = 1;
	if (mtmp->mconf || (uarmh && !rn2(10) && itemhasappearance(uarmh, APP_INKCOAT_HELMET) ) || (uarmh && uarmh->oartifact == ART_RADAR_NOT_WORKING) || (monsndx(ptr) == PM_DANCING_DRAGON) || (monsndx(ptr) == PM_ERR_BOSS) || (monsndx(ptr) == PM_ERR) || (monsndx(ptr) == PM_NOTHING_CHECKER_WHO_IS_CONFUSED) || (monsndx(ptr) == PM_TREMBLING_POPLAR) || (monsndx(ptr) == PM_DEAR_ENEMY) || (monsndx(ptr) == PM_TOIDI) || (u.uswallow && mtmp == u.ustuck))
		appr = 0;
	else if ((monsndx(ptr) == PM_WIMPDAG_BANGER) || (monsndx(ptr) == PM_SLING_BANGER) || (monsndx(ptr) == PM_FLINT_BANGER) || (monsndx(ptr) == PM_DAT_BANGER) || (monsndx(ptr) == PM_BOW_BANGER) || (monsndx(ptr) == PM_PISTOL_BANGER) || (monsndx(ptr) == PM_SPEA_BANGER) || (monsndx(ptr) == PM_XBOW_BANGER) || (monsndx(ptr) == PM_SHOTTY_BANGER) || (monsndx(ptr) == PM_SHURI_BANGER) || (monsndx(ptr) == PM_SMG_BANGER) || (monsndx(ptr) == PM_LASER_BANGER) || (monsndx(ptr) == PM_ASSAULT_BANGER) || (monsndx(ptr) == PM_JAV_BANGER) || (monsndx(ptr) == PM_AUTOSHOT_BANGER) || (monsndx(ptr) == PM_ARMSTREAM_BANGER) || (monsndx(ptr) == PM_BIG_FUCKING_BANGER) )
		appr = 0;
	else {
#ifdef GOLDOBJ
		struct obj *lepgold, *ygold;
#endif
		boolean should_see = (couldsee(omx, omy) &&
				      (levl[gx][gy].lit ||
				       !levl[omx][omy].lit) &&
				      (dist2(omx, omy, gx, gy) <= (level.flags.shortsighted ? 36 : 100) ));

		if (astralspecial) should_see = TRUE;

		if (!mtmp->mcansee ||
		/* monsters no longer automatically know where you are. That was just incredibly annoying. --Amy */
		( (!Aggravate_monster || !rn2(5)) && !should_see && distu(mtmp->mx,mtmp->my) > 10 && ((Stealth && (StrongStealth || !rn2(3))) ? (can_track(ptr) ? !rn2(4) : rn2(2) ) : (can_track(ptr) ? !rn2(10) : !rn2(4) ) ) ) ||
			 ( (!Aggravate_monster || !rn2(20)) && is_wanderer(mtmp->data) ? ((Stealth && (StrongStealth || !rn2(3))) ? !rn2(3) : !rn2(5) ) : ((Stealth && (StrongStealth || !rn2(3))) ? !rn2(5) : !rn2(25) ) ) ||
		    (should_see && Invis && (StrongInvis || !rn2(3)) && haseyes(ptr) && !perceives(ptr) && rn2(3)) ||
		    (youmonst.m_ap_type == M_AP_OBJECT && youmonst.mappearance == STRANGE_OBJECT) || u.uundetected ||
		    (youmonst.m_ap_type == M_AP_OBJECT && youmonst.mappearance == GOLD_PIECE && !likes_gold(ptr)) ||
		    (mtmp->mpeaceful && !mtmp->isshk) ||  /* allow shks to follow */
		    ((monsndx(ptr) == PM_STALKER || ptr->mlet == S_BAT ||
		      ptr->mlet == S_LIGHT) && !rn2(3)))
			appr = 0;

		/* monsters that are very far away shouldn't know where you are --Amy
		 * but if they're injured, they should (most of the time it's because you tried to kill them) */

		if (mtmp->mhp >= mtmp->mhpmax) {

			int nositerange = 900, siterange = 2500;
			if (Aggravate_monster) {
				nositerange *= 3;
				siterange *= 3;
			}
			if (StrongAggravate_monster) {
				nositerange *= 3;
				siterange *= 3;
			}
			if (Stealth) {
				nositerange /= 2;
				siterange /= 2;
			}
			if (StrongStealth) {
				nositerange /= 2;
				siterange /= 2;
			}

			if (clear_path(u.ux, u.uy, mtmp->mx, mtmp->my) && (distu(mtmp->mx,mtmp->my) > siterange)) appr = 0;
			if (!clear_path(u.ux, u.uy, mtmp->mx, mtmp->my) && (distu(mtmp->mx,mtmp->my) > nositerange)) appr = 0;

		}

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
			if (cp && !astralspecial) {
				gx = cp->x;
				gy = cp->y;
			}
		}
	}

	if (appr == 1 && !rn2(5) && (uarm && itemhasappearance(uarm, APP_CAMO_ROBE)) ) appr = 0;

	if (appr == 1 && uarmf && uarmf->oartifact == ART_SMELL_LIKE_DOG_SHIT && !rn2(10)) appr = 0;

	if (appr == 1 && !rn2(5) && (uarmc && itemhasappearance(uarmc, APP_PINK_CLOAK) )) appr = 0;

	if (ptr == &mons[PM_ANCIENT_BIBLICAL_DRAGON]) appr = -1;
	if (ptr == &mons[PM_BLOWSTREAM_BABY]) appr = -1;
	if (ptr == &mons[PM_BOGUXORN]) appr = -1;
	if (ptr == &mons[PM_POOL_EDGE_SWIMMER]) appr = -1;
	if (ptr == &mons[PM_SOCIAL_DISORDER]) appr = -1;

	if (u.katitrapocc && !mtmp->mpeaceful) appr = -1; /* they're supposed to let you perform your occupation in peace */

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
			objects[otmp->otyp].oc_material != MT_MINERAL) ||
		       (conceals && !cansee(otmp->ox,otmp->oy)) ||
		       (ptr == &mons[PM_GELATINOUS_CUBE] &&
			!index(indigestion, otmp->oclass) &&
			!(otmp->otyp == CORPSE &&
			  touch_petrifies(&mons[otmp->corpsenm])))
		      ) && touch_artifact(otmp,mtmp)) {
			if(can_carry(mtmp,otmp) &&
				otmp->otyp != STATUE && 
				otmp->otyp != CHARGER && 
				otmp->otyp != SYMBIOTE && 
				otmp->otyp != SWITCHER && 
				otmp->otyp != UGH_MEMORY_TO_CREATE_INVENTORY && 
			   (throws_rocks(ptr) ||
				!sobj_at(BOULDER,xx,yy)) &&
			   (!is_unicorn(ptr) ||
			    objects[otmp->otyp].oc_material == MT_GEMSTONE) &&
			   /* Don't get stuck circling an Elbereth */
			   !(onscary(xx, yy, mtmp))) {
			    minr = distmin(omx,omy,xx,yy);
			    oomx = min(COLNO-1, omx+minr);
			    oomy = min(ROWNO-1, omy+minr);
			    lmx = max(1, omx-minr);
			    lmy = max(0, omy-minr);
			    if (!astralspecial) {
				    gx = otmp->ox;
				    gy = otmp->oy;
			    }
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
	    if(!astralspecial && (distmin(omx,omy,mtmp->mux,mtmp->muy) <= 3)) {
		gx = mtmp->mux;
		gy = mtmp->muy;
	    } else
		appr = 1;
	}
      }

	if (astralspecial) appr = 1;

	/* don't tunnel if hostile and close enough to prefer a weapon */
	if (can_tunnel && needspick(ptr) &&
	    ((!mtmp->mpeaceful || Conflict) &&
	     dist2(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) <= 8))
	    can_tunnel = FALSE;

	nix = omx;
	niy = omy;
	flag = 0L;
	if (mtmp->mpeaceful && (!Conflict || (resist(mtmp, RING_CLASS, 0, 0) && (!StrongConflict || resist(mtmp, RING_CLASS, 0, 0)) ) ))
	    flag |= (ALLOW_SANCT | ALLOW_SSM);
	else flag |= ALLOW_U;
	if (is_minion(ptr) || is_rider(ptr) || is_deadlysin(ptr)) flag |= ALLOW_SANCT;
	/* unicorn may not be able to avoid hero on a noteleport level */
	if (is_unicorn(ptr) && !level.flags.noteleport && !u.antitelespelltimeout) flag |= NOTONL;
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
	    if(!mtmp->mpeaceful && (level.flags.shortsighted || (rn2(10) && RngeLightAbsorption) || (rn2(10) && uarmc && itemhasappearance(uarmc, APP_ABSORBING_CLOAK) ) ) &&
	       nidist > (couldsee(nix,niy) ? 144 : 36) && appr == 1) appr = 0;

		/* special coding for "homing" giant wasps from the hunger games --Amy */
		if ((ptr == &mons[PM_TRACKER_JACKER] || ptr == &mons[PM_FULL_REFUGE] || ptr == &mons[PM_DRIVE_TRAIN] || ptr == &mons[PM_XTREME_TRACKER] || ptr == &mons[PM_REFUGE_UHLERT]) && !mtmp->mpeaceful) appr = 1;

	if (uarmh && itemhasappearance(uarmh, APP_BUG_TRACKING_HELMET) && !rn2(3) ) appr = 1; 

	if (uarmf && itemhasappearance(uarmf, APP_RAINBOW_BOOTS) && !rn2(3) ) appr = 1; 

		if (uarmf && uarmf->oartifact == ART_HENRIETTA_S_DOGSHIT_BOOTS) appr = 1;

		if (uarmh && uarmh->oartifact == ART_RADAR_NOT_WORKING) appr = 0;

	    if (is_unicorn(ptr) && (level.flags.noteleport || u.antitelespelltimeout)) {
		/* on noteleport levels, perhaps we cannot avoid hero */
		for(i = 0; i < cnt; i++)
		    if(!(info[i] & NOTONL)) avoid=TRUE;
	    }
		if (avoid_player(ptr) || mtmp->egotype_avoider ) avoid=TRUE; /* making this into a monster flag --Amy */

		if (astralspecial) appr = 1;

	    for(i=0; i < cnt; i++) {
		if (avoid && (info[i] & NOTONL)) continue;
		nx = poss[i].x;
		ny = poss[i].y;

		if (FeelerGauges || u.uprops[FEELER_GAUGES].extrinsic || have_feelergaugesstone() ) appr = 1;

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
			(mw_tmp && (!is_pick(mw_tmp) || !is_axe(mw_tmp))) )
			mtmp->weapon_check = NEED_PICK_OR_AXE;
		} else if (IS_TREE(levl[nix][niy].typ)) {
		    if (!(mw_tmp = MON_WEP(mtmp)) || (mw_tmp && !is_axe(mw_tmp)) )
			mtmp->weapon_check = NEED_AXE;
		} else if (!(mw_tmp = MON_WEP(mtmp)) || (mw_tmp && !is_pick(mw_tmp)) ) {
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

		if ( ((mstatus & MM_HIT) || (mtmp2 && mtmp2->mtame && mtmp && !mtmp->mtame && !rn2(4))) && !(mstatus & MM_DEF_DIED) &&
		    ((rn2(4) && mtmp2 && mtmp2->movement >= NORMAL_SPEED) || (mtmp2 && mtmp2->mtame && mtmp && !mtmp->mtame)) ) {
		    mtmp2->movement -= NORMAL_SPEED;
		    if (mtmp2->movement < 0) mtmp2->movement = 0; /* fail safe */
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

		if ((EaterBugEffect || u.uprops[EATER_BUG].extrinsic || have_eaterstone() || mtmp->egotype_allivore)) {
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
		if (ptr == &mons[PM_GHOUL] || ptr == &mons[PM_GHAST] || ptr == &mons[PM_CORPSEWORM] || ptr == &mons[PM_DOGSHIT_SEARCHER] || ptr == &mons[PM_STINKING_ALIEN] || ptr == &mons[PM_GASTLY] || ptr == &mons[PM_PHANTOM_GHOST]
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

	notseen = (!mtmp->mcansee || (Invis && (StrongInvis || !rn2(3)) && haseyes(mtmp->data) && !perceives(mtmp->data)));
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
	} else if (Displaced && (StrongDisplaced || !rn2(3)) && !(dmgtype(mtmp->data, AD_DISP) ) && !(dmgtype(mtmp->data, AD_MAGM) ) && !(dmgtype(mtmp->data, AD_MCRE) ) && mtmp->data != &mons[PM_BABY_GRAY_DRAGON] && mtmp->data != &mons[PM_YOUNG_GRAY_DRAGON] && mtmp->data != &mons[PM_YOUNG_ADULT_GRAY_DRAGON] &&
		!(dmgtype(mtmp->data, AD_RBRE)) && !(dmgtype(mtmp->data, AD_RNG)) ) {
	    disp = couldsee(mx, my) ? 2 : 1;
	} else disp = 0;
	if (!disp) goto found_you;

	/* without something like the following, invis. and displ.
	   are too powerful */
	gotu = notseen ? rn2(3) : StrongDisplaced ? rn2(2) : Displaced ? rn2(2) : FALSE;

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
		    !(typ >= ARROW && typ <= DARK_BATARANG) &&
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
