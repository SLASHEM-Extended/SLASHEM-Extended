/*	SCCS Id: @(#)uhitm.c	3.4	2003/02/18	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

STATIC_DCL boolean known_hitum(struct monst *,int,int *,struct attack *,int);
STATIC_DCL void steal_it(struct monst *, struct attack *);
#if 0
STATIC_DCL boolean hitum(struct monst *,int,int,struct attack *);
#endif
STATIC_DCL boolean hmon_hitmon(struct monst *,struct obj *,int,int);
STATIC_DCL int joust(struct monst *,struct obj *);
STATIC_DCL void demonpet(void);
STATIC_DCL boolean m_slips_free(struct monst *mtmp,struct attack *mattk);
STATIC_DCL int explum(struct monst *,struct attack *);
STATIC_DCL void start_engulf(struct monst *);
STATIC_DCL void end_engulf(void);
STATIC_DCL int gulpum(struct monst *,struct attack *);
STATIC_DCL boolean hmonas(struct monst *,int);
STATIC_DCL void nohandglow(struct monst *);
STATIC_DCL boolean shade_aware(struct obj *);

static int martial_dmg(void);

static const char allnoncount[] = { ALL_CLASSES, 0 };

STATIC_PTR void set_lit(int,int,void *);

extern boolean notonhead;	/* for long worms */
/* The below might become a parameter instead if we use it a lot */
/* and now it actually did */
/* static int dieroll; */
static int rolls[2][2];
#define dice(x) rolls[0][x]
#define tohit(x) rolls[1][x]
#define UWEP_ROLL	0
#define USWAPWEP_ROLL	1

#define get_artifact(o) \
		(((o)&&(o)->oartifact) ? &artilist[(int) ((unsigned int) (o)->oartifact)] : 0)

/* Used to flag attacks caused by Stormbringer's maliciousness. */
static boolean override_confirmation = 0;

/* Used to control whether Drow's sleep attack should succeed. */
static boolean barehanded_hit = 0;

/* WAC for mhit,  two weapon attacking */
#define HIT_UWEP 	1
#define HIT_USWAPWEP 	2
#define HIT_BODY 	4		/* Hit with other body part */
#define HIT_OTHER 	8		/* Hit without touching */
#define HIT_FATAL 	16

#define PROJECTILE(obj)	((obj) && is_ammo(obj))

/* modified from hurtarmor() in mhitu.c */
/* This is not static because it is also used for monsters rusting monsters */
void
hurtmarmor(mdef, attk)
struct monst *mdef;
int attk;
{
	int	hurt;
	struct obj *target;

	switch(attk) {
	    /* 0 is burning, which we should never be called with */
		/* Amy edit: AD_LAVA does now. */
	    case AD_RUST: hurt = 1; break;
	    case AD_CORR: hurt = 3; break;
	    case AD_LAVA: hurt = 0; break;
	    case AD_FLAM: hurt = 0; break;
	    default: hurt = 2; break;
	}
	/* What the following code does: it keeps looping until it
	 * finds a target for the rust monster.
	 * Head, feet, etc... not covered by metal, or covered by
	 * rusty metal, are not targets.  However, your body always
	 * is, no matter what covers it.
	 */
	while (1) {
	    switch(rn2(5)) {
	    case 0:
		target = which_armor(mdef, W_ARMH);
		if (!target || !rust_dmg(target, xname(target), hurt, FALSE, mdef))
		    continue;
		break;
	    case 1:
		target = which_armor(mdef, W_ARMC);
		if (target) {
		    (void)rust_dmg(target, xname(target), hurt, TRUE, mdef);
		    break;
		}
		if ((target = which_armor(mdef, W_ARM)) != (struct obj *)0) {
		    (void)rust_dmg(target, xname(target), hurt, TRUE, mdef);
		} else if ((target = which_armor(mdef, W_ARMU)) != (struct obj *)0) {
		    (void)rust_dmg(target, xname(target), hurt, TRUE, mdef);
		}
		break;
	    case 2:
		target = which_armor(mdef, W_ARMS);
		if (!target || !rust_dmg(target, xname(target), hurt, FALSE, mdef))
		    continue;
		break;
	    case 3:
		target = which_armor(mdef, W_ARMG);
		if (!target || !rust_dmg(target, xname(target), hurt, FALSE, mdef))
		    continue;
		break;
	    case 4:
		target = which_armor(mdef, W_ARMF);
		if (!target || !rust_dmg(target, xname(target), hurt, FALSE, mdef))
		    continue;
		break;
	    }
	    break; /* Out of while loop */
	}
}

/*
 * Now returns a bit mask of attacks that may proceed. Note that barehanded
 * returns HIT_UWEP. -ALI
 */

int
attack_checks(mtmp, barehanded)
register struct monst *mtmp;
boolean barehanded;
{
	int retval;
	char qbuf[QBUFSZ];

	if (barehanded || !u.twoweap || !uswapwep)
		retval = HIT_UWEP;
	else
		retval = HIT_UWEP | HIT_USWAPWEP;

	/* if you're close enough to attack, alert any waiting monster */
	mtmp->mstrategy &= ~STRAT_WAITMASK;

	if (u.uswallow && mtmp == u.ustuck) return retval;

	if (flags.forcefight) {
		/* Do this in the caller, after we checked that the monster
		 * didn't die from the blow.  Reason: putting the 'I' there
		 * causes the hero to forget the square's contents since
		 * both 'I' and remembered contents are stored in .glyph.
		 * If the monster dies immediately from the blow, the 'I' will
		 * not stay there, so the player will have suddenly forgotten
		 * the square's contents for no apparent reason.
		if (!canspotmon(mtmp) && !(mtmp->data->msound == MS_DEEPSTATE) && !(mtmp->egotype_deepstatemember) &&
		    !memory_is_invisible(u.ux+u.dx, u.uy+u.dy))
			map_invisible(u.ux+u.dx, u.uy+u.dy);
		 */
		return retval;
	}

	/* Put up an invisible monster marker, but with exceptions for
	 * monsters that hide and monsters you've been warned about.
	 * The former already prints a warning message and
	 * prevents you from hitting the monster just via the hidden monster
	 * code below; if we also did that here, similar behavior would be
	 * happening two turns in a row.  The latter shows a glyph on
	 * the screen, so you know something is there.
	 */

	if (MeleePrefixBug || u.uprops[MELEE_PREFIX_BUG].extrinsic || autismweaponcheck(ART_ULTRA_ANNOYANCE) || autismweaponcheck(ART_RIDGET_PHASTO) || have_meleeprefixstone()) {

		pline("You bump into %s.", mon_nam(mtmp));
		return 0;
	}

	if (!canspotmon(mtmp) &&
		    !glyph_is_warning(glyph_at(u.ux+u.dx,u.uy+u.dy)) &&
		    !(memory_is_invisible(u.ux+u.dx, u.uy+u.dy) && !(UnderlayerBug || u.uprops[UNDERLAYER_BUG].extrinsic || have_underlaidstone()) ) &&
		    !(!Blind && mtmp->mundetected && hides_under(mtmp->data))) {

		if (uarmc && uarmc->oartifact == ART_JANA_S_SECRET_CAR && !rn2(100) ) {
			u.youaredead = 1;
			pline("NETHACK caused a General Protection Fault at address 0003:0CED.");
			killer_format = KILLED_BY;
			killer = "Jana's secret car";
			done(DIED);
			u.youaredead = 0;
		} else
		pline("Wait!  There's %s there you can't see!",
			something);

		if (!rn2(2) && !mtmp->mpeaceful && !mtmp->mtame) (void) passive(mtmp, TRUE, mtmp->mhp > 0, AT_TUCH, FALSE);

		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		if (!(mtmp->data->msound == MS_DEEPSTATE) && !(mtmp->egotype_deepstatemember)) map_invisible(u.ux+u.dx, u.uy+u.dy);
		/* if it was an invisible mimic, treat it as if we stumbled
		 * onto a visible mimic
		 */
		if(mtmp->m_ap_type && !Protection_from_shape_changers) {
		    if(!u.ustuck && !mtmp->mflee && dmgtype(mtmp->data,AD_STCK))
			setustuck(mtmp);
		}
		wakeup(mtmp); /* always necessary; also un-mimics mimics */
		return 0;
	}

	if (mtmp->m_ap_type && !Protection_from_shape_changers &&
	   !sensemon(mtmp) &&
	   !glyph_is_warning(glyph_at(u.ux+u.dx,u.uy+u.dy))) {
		/* If a hidden mimic was in a square where a player remembers
		 * some (probably different) unseen monster, the player is in
		 * luck--he attacks it even though it's hidden.
		 */
		if (memory_is_invisible(mtmp->mx, mtmp->my)) {
		    seemimic(mtmp);
		    return retval;
		}
		if (uarmc && uarmc->oartifact == ART_JANA_S_GRAVE_WALL && !rn2(100) ) {
			u.youaredead = 1;
			pline("NETHACK caused a General Protection Fault at address 000B:0122.");
			killer_format = KILLED_BY;
			killer = "Jana's grave wall";
			done(DIED);
			u.youaredead = 0;
		} else
		stumble_onto_mimic(mtmp);
		return 0;
	}

	/* it used to say "canseemon" here but that was bullshit, because you'd constantly lose turns if you were
	 * detecting monsters via some other weird method! --Amy */
	if (mtmp->mundetected && !canspotmon(mtmp) &&
		!glyph_is_warning(glyph_at(u.ux+u.dx,u.uy+u.dy)) &&
		(hides_under(mtmp->data) || mtmp->data->mlet == S_FLYFISH || ((mtmp->data->mlet == S_EEL) && !(mtmp->data == &mons[PM_DEFORMED_FISH]) && !(mtmp->data == &mons[PM_MOAT_PIRAHNA]) && !(mtmp->data == &mons[PM_CHRABCHRABCHRAB_PIRAHNA]) ) ) ) {
	    mtmp->mundetected = mtmp->msleeping = 0;
	    newsym(mtmp->mx, mtmp->my);
	    if (memory_is_invisible(mtmp->mx, mtmp->my)) {
		seemimic(mtmp);
		return retval;
	    }
	    if (!((Blind ? Blind_telepat : Unblind_telepat) || Detect_monsters)) {
		struct obj *obj;

		if (Blind || ((is_waterypool(mtmp->mx,mtmp->my) || is_watertunnel(mtmp->mx, mtmp->my) || is_shiftingsand(mtmp->mx, mtmp->my) ) && !Underwater))
		    pline("Wait!  There's a hidden monster there!");
		else if ((obj = level.objects[mtmp->mx][mtmp->my]) != 0) {
			if (uarmc && uarmc->oartifact == ART_JANA_S_EXTREME_HIDE_AND_SE && !rn2(100) ) {
				u.youaredead = 1;
				pline("NETHACK caused a General Protection Fault at address 000D:001D.");
				killer_format = KILLED_BY;
				killer = "Jana's devious cloak";
				done(DIED);
				u.youaredead = 0;
			} else
			pline("Wait!  There's %s hiding under %s!", an(l_monnam(mtmp)), doname(obj));
		}

		if (!rn2(2) && !mtmp->mpeaceful && !mtmp->mtame) (void) passive(mtmp, TRUE, mtmp->mhp > 0, AT_TUCH, FALSE);

		return 0;
	    }
	}

	/*
	 * make sure to wake up a monster from the above cases if the
	 * hero can sense that the monster is there.
	 */
	if ((mtmp->mundetected || mtmp->m_ap_type) && sensemon(mtmp)) {
	    mtmp->mundetected = 0;
	    wakeup(mtmp);
	}
	if (permamimic(mtmp->data) || mtmp->egotype_permamimic ) {
	    /*mtmp->mundetected = 0;*/
	    wakeup(mtmp);
	}

	if (flags.confirm && !(ParanoiaBugEffect || u.uprops[PARANOIA_BUG].extrinsic || have_paranoiastone()) && mtmp->mpeaceful
	    && !(Confusion && !Conf_resist) && !Hallucination && !(Stunned && !Stun_resist) ) {
		/* Intelligent chaotic weapons (Stormbringer) want blood */
		if ((BloodthirstyAttacking || (uwep && weapon_is_bloodthirsty(uwep)) )) {
			override_confirmation = HIT_UWEP;
			return retval;
		}
		if (canspotmon(mtmp)) {

			/* from the variant that calls itself NetHack 3.70: sometimes, silently scare the priest --Amy */
			if (isevilvariant && mtmp->ispriest && !rn2(7)) monflee(mtmp, rnd(6), FALSE, FALSE);

			sprintf(qbuf, "Really attack %s?", mon_nam(mtmp));
			if (yn(qbuf) != 'y') {
				/* Stormbringer is not tricked so easily */
				if (BloodthirstyAttacking || (u.twoweap && uswapwep && weapon_is_bloodthirsty(uswapwep)) ) {
					override_confirmation = HIT_USWAPWEP;
					/* Lose primary attack */
					return HIT_USWAPWEP;
				}
				flags.move = 0;
				return 0;
			}
			char bufX[BUFSZ];
			getlin ("Are you really sure [yes/no]?",bufX);
			if (strcmp (bufX, "yes")) {
				/* Stormbringer is not tricked so easily */
				if (BloodthirstyAttacking || (u.twoweap && uswapwep && weapon_is_bloodthirsty(uswapwep)) ) {
					override_confirmation = HIT_USWAPWEP;
					/* Lose primary attack */
					return HIT_USWAPWEP;
				}
				flags.move = 0;
				return 0;
			}
		}
	}

	return retval;
}

/*
 * It is unchivalrous for a knight to attack the defenseless or from behind.
 */
void
check_caitiff(mtmp)
struct monst *mtmp;
{
	if (Role_if(PM_PALADIN) && mtmp->mpeaceful && mtmp->isshk && !strncmpi(shkname(mtmp), "Izchak", 6) ) {
	/* Attacking Izchak is grounds for immediate disintegration. */

		adjalign(-200);

		You_feel("the air around you grow charged...");
		pline("Suddenly, you realize that %s has noticed you...", u_gname());
		/* Throw everything we have at the player */
		god_zaps_you(u.ualign.type);

	}

	if (Role_if(PM_KNIGHT) && u.ualign.type == A_LAWFUL &&
	    (!mtmp->mcanmove || mtmp->msleeping ||
	     (mtmp->mflee && !mtmp->mavenge)) ) {
	    You("caitiff!");
	    adjalign(-5);

	}

	if (uwep && uwep->otyp == HONOR_KATANA && u.ualign.type == A_LAWFUL &&
	    (!mtmp->mcanmove || mtmp->msleeping ||
	     (mtmp->mflee && !mtmp->mavenge)) ) {
	    You("caitiff!");
	    adjalign(-5);

	}

	if (u.twoweap && uswapwep && uswapwep->otyp == HONOR_KATANA && u.ualign.type == A_LAWFUL &&
	    (!mtmp->mcanmove || mtmp->msleeping ||
	     (mtmp->mflee && !mtmp->mavenge)) ) {
	    You("caitiff!");
	    adjalign(-5);

	}

	if (Role_if(PM_CHEVALIER) &&
	    (!mtmp->mcanmove || mtmp->msleeping ||
	     (mtmp->mflee && !mtmp->mavenge)) ) {
	    You("caitiff!");
	    adjalign(-5);

	}

	if (Role_if(PM_HEDDERJEDI) &&
	    (!mtmp->mcanmove || mtmp->msleeping ||
	     (mtmp->mflee && !mtmp->mavenge)) ) {
	    You("caitiff!");
	    adjalign(-5);

	}

	if (Role_if(PM_CELLAR_CHILD) &&
	    (!mtmp->mcanmove || mtmp->msleeping ||
	     (mtmp->mflee && !mtmp->mavenge)) ) {
	    You("caitiff!");
	    adjalign(-5);

	}

	if (Race_if(PM_VEELA) &&
	    (!mtmp->mcanmove || mtmp->msleeping ||
	     (mtmp->mflee && !mtmp->mavenge)) ) {
	    You("crumple-horned snorkack!");
	    adjalign(-5);
		badeffect();

	}

	if (Role_if(PM_PALADIN) &&
	    (!mtmp->mcanmove || mtmp->msleeping ||
	     (mtmp->mflee && !mtmp->mavenge)) ) {
	    You("caitiff!");
	    adjalign(-5);

	}

	if (mtmp->data == &mons[PM_DNETHACK_ELDER_PRIEST_TM_]) {
		mtmp->isegotype = 1;
		mtmp->egotype_covetous = 1;
	}

	if ( Role_if(PM_LADIESMAN) && !flags.female && mtmp->female && humanoid(mtmp->data) ) {

	    You_feel("bad for hitting a defenseless woman.");
	    adjalign(-1);

	}

	/* Batman may not hit women, but you can be a female batman and it would be unfair to only punish males. --Amy */
	if ( Race_if(PM_BATMAN) && ((!flags.female && mtmp->female) || (flags.female && !mtmp->female)) && humanoid(mtmp->data) ) {

	    You_feel("bad for hitting a defenseless %s.", flags.female ? "man" : "woman");
	    adjalign(-1);

	}

	if (FemtrapActiveNadja && mtmp->female && humanoid(mtmp->data)) {

		u.cnd_nadjatrapcnt++;

		pline("%s is really angry about the fact that you tried to hit her, and uses a pair of buckled shoes to scratch up and down your %s, ripping a lot of skin to shreds.", Monnam(mtmp), body_part(LEG));

		/* this can now increase the player's legscratching variable. Since the damage you take depends on how much
		 * legscratching you already have, and you might get hit by a long-lasting effect of this trap, we need to
		 * make absolutely sure that the increases don't happen too quickly or it becomes unplayable; this is achieved
		 * by having only 1 in (legscratching squared) chance for an increase --Amy */
		if (!u.legscratching) u.legscratching++;
		else if (u.legscratching > 60000) {
			if (!rn2(60000) && !rn2(60000)) u.legscratching++;
		} else if (!rn2(u.legscratching * u.legscratching)) u.legscratching++;
		losehp(rnd(u.legscratching), "the wrath of Nadja's buckled lady shoes", KILLED_BY);
		if (u.legscratching > 20) {
			pline("She notices that you're bleeding, which seems to make her even more angry as she continues slitting your %s full length with the metal buckle!", body_part(LEG));
			losehp(rnd(u.legscratching), "the wrath of Nadja's buckled lady shoes", KILLED_BY);
		}
		if (u.legscratching > 40) {
			pline("You are severely hurt, but %s just doesn't want to stop punishing you with her very female buckled shoes. She continues destroying your unprotected skin and announces that if you hit her one more time, she will kill you.", mon_nam(mtmp));
			losehp(rnd(u.legscratching), "the wrath of Nadja's buckled lady shoes", KILLED_BY);
		}
	}

}

int
find_roll_to_hit(mtmp)
register struct monst *mtmp;
{
	/*schar*/int tmp; /* fail safe at the bottom ensures it returns something valid --Amy */
	int tmp2;

	/* idea gotten from watching Chris's to-hit discussion: high luck gave too big boosts --Amy */

	tmp = 1 + ( (rn2(2) && Luck > 0) ? rnd(Luck) : Luck) + abon() + find_mac(mtmp) + ((increase_accuracy_bonus_value() > 1) ? rnd(increase_accuracy_bonus_value()) : increase_accuracy_bonus_value()) +
		(rn2(2) ? (maybe_polyd(rnd(youmonst.data->mlevel + 1), rnd(GushLevel))) : (maybe_polyd(youmonst.data->mlevel + 1, GushLevel)) );

	/* another extra boost --Amy */
	/* In Soviet Russia, to-hit bonuses generally get trashed. Nobody needs to be able to hit a -40 AC monster anyway,
	 * and if you do, well, tough luck! Communism isn't prepared to handle such rare situations! --Amy */
	if (!issoviet && !Upolyd && !rn2(20 - (GushLevel / 2) )) tmp += rnd(GushLevel);

	if (Feared) tmp -= rn2(21); /* being feared reduces to-hit by something between 0 and 20 --Amy */

	if (!(PlayerCannotUseSkills)) {
		switch (P_SKILL(P_GENERAL_COMBAT)) {
			default: break;
			case P_BASIC: tmp += 1; break;
			case P_SKILLED: tmp += rnd(2); break;
			case P_EXPERT: tmp += rnd(3); break;
			case P_MASTER: tmp += rnd(4); break;
			case P_GRAND_MASTER: tmp += rnd(5); break;
			case P_SUPREME_MASTER: tmp += rnd(6); break;
		}
	}

	if (!rn2(3)) {
		if (!(PlayerCannotUseSkills)) {
			switch (P_SKILL(P_GENERAL_COMBAT)) {
				default: break;
				case P_BASIC: tmp += 1; break;
				case P_SKILLED: tmp += rn2(2); break;
				case P_EXPERT: tmp += rn2(3); break;
				case P_MASTER: tmp += rn2(4); break;
				case P_GRAND_MASTER: tmp += rn2(5); break;
				case P_SUPREME_MASTER: tmp += rn2(6); break;
			}
		}
	}

	if (Upolyd && !(PlayerCannotUseSkills)) {
		switch (P_SKILL(P_POLYMORPHING)) {

	      	case P_BASIC:	tmp +=  1; break;
	      	case P_SKILLED:	tmp +=  2; break;
	      	case P_EXPERT:	tmp +=  3; break;
	      	case P_MASTER:	tmp +=  4; break;
	      	case P_GRAND_MASTER:tmp +=  5; break;
	      	case P_SUPREME_MASTER:tmp +=  6; break;
	      	default: tmp += 0; break;
	      }

	}

	if (!rn2(3)) {
		if (Upolyd && !(PlayerCannotUseSkills)) {
			switch (P_SKILL(P_POLYMORPHING)) {
		      	case P_BASIC:	tmp +=  rn2(3); break;
		      	case P_SKILLED:	tmp +=  rn2(6); break;
		      	case P_EXPERT:	tmp +=  rn2(9); break;
		      	case P_MASTER:	tmp +=  rn2(12); break;
		      	case P_GRAND_MASTER:tmp +=  rn2(15); break;
		      	case P_SUPREME_MASTER:tmp +=  rn2(18); break;
		      	default: tmp += 0; break;
		      }
		}
	}

	check_caitiff(mtmp);

/*	attacking peaceful creatures is bad for the samurai's giri */
	if (Role_if(PM_SAMURAI) && mtmp->mpeaceful) {
	    You("dishonorably attack the innocent!");
		increasesincounter(1);
		u.alignlim--;
	    adjalign(-5);
	}

	if (uwep && uwep->otyp == HONOR_KATANA && mtmp->mpeaceful) {
	    You("dishonorably attack the innocent!");
		increasesincounter(1);
		u.alignlim--;
	    adjalign(-5);
	}

	if (u.twoweap && uswapwep && uswapwep->otyp == HONOR_KATANA && mtmp->mpeaceful) {
	    You("dishonorably attack the innocent!");
		increasesincounter(1);
		u.alignlim--;
	    adjalign(-5);
	}

	if (uwep && uwep->oartifact == ART_JAPANESE_WOMEN && mtmp->mpeaceful) {
	    You("dishonorably attack the innocent!");
		increasesincounter(1);
		u.alignlim--;
	    adjalign(-5);
	}

	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_JAPANESE_WOMEN && mtmp->mpeaceful) {
	    You("dishonorably attack the innocent!");
		increasesincounter(1);
		u.alignlim--;
	    adjalign(-5);
	}

	if (uwep && uwep->oartifact == ART_SAKUSHNIR && mtmp->mpeaceful) {
	    You("dishonorably attack the innocent!");
		increasesincounter(1);
		u.alignlim--;
	    adjalign(-5);
	}

	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_SAKUSHNIR && mtmp->mpeaceful) {
	    You("dishonorably attack the innocent!");
		increasesincounter(1);
		u.alignlim--;
	    adjalign(-5);
	}

/* as well as for the way of the Jedi */
	if (Role_if(PM_JEDI) && mtmp->mpeaceful) {
	    You("violate the way of the Jedi!");
		increasesincounter(1);
		u.alignlim--;
	    adjalign(-5);
	}
	if (Role_if(PM_SHADOW_JEDI) && mtmp->mpeaceful) {
	    You("violate the way of the Jedi!");
		increasesincounter(1);
		u.alignlim--;
	    adjalign(-5);
	}
	if (Role_if(PM_HEDDERJEDI) && mtmp->mpeaceful) {
	    You("violate the way of the Jedi!");
		increasesincounter(1);
		u.alignlim--;
	    adjalign(-5);
	}
	if (Race_if(PM_BORG) && mtmp->mpeaceful) {
	    You("violate the way of the Jedi!");
		increasesincounter(1);
		u.alignlim--;
	    adjalign(-5);
	}

/*	Adjust vs. (and possibly modify) monster state.		*/

	if(mtmp->mstun) tmp += 2;
	if(mtmp->mflee) tmp += 2;

	if (mtmp->msleeping) {
		mtmp->msleeping = 0;
		tmp += 2;
	}
	if(!mtmp->mcanmove) {
		tmp += 4;
		if(!rn2(mtmp->masleep ? 3 : 10)) {
			mtmp->mcanmove = 1;
			mtmp->mfrozen = 0;
			mtmp->masleep = 0;
		}
	}

	if (is_orc(mtmp->data) && maybe_polyd(is_elf(youmonst.data),
			Race_if(PM_ELF)))
	    tmp++;

    /* Adding iron ball as a weapon skill gives a -4 penalty for
    unskilled vs no penalty for non-weapon objects.  Add 4 to
    compensate. */
    /* if (uwep && (uwep->otyp == HEAVY_IRON_BALL)) {
        tmp += 4;   Compensate for iron ball weapon skill -4
                    penalty for unskilled vs no penalty for non-
                    weapon objects.
    } */

	if (Race_if(PM_INKA) && uwep && objects[uwep->otyp].oc_material == MT_INKA) {
		tmp += 5;
	}

	if (uwep && objects[uwep->otyp].oc_material == MT_METEOSTEEL) {
		tmp++;
	}

	/* In Soviet Russia, convicts are treated with disdain and indifference. They are intentionally chained to
	 * special "unwieldy" balls and chains, which don't make good weapons and are therefore less likely to be used
	 * by the convicts in an attempt to break out of prison and/or kill the guards. Or, in other words, the convict
	 * is supposed to suck for some reason, but only in the early game because later on you'll max out your skill
	 * anyway and therefore won't be missing anymore. --Amy */
    if (uwep && (uwep->oclass == CHAIN_CLASS || uwep->oclass == BALL_CLASS) && issoviet) tmp -= 4;
	/* mini-nerf outside of soviet mode too, because some of these things are possibly overpowered */
    if (uwep && (uwep->oclass == CHAIN_CLASS || uwep->oclass == BALL_CLASS)) tmp -= rnd(5);

#if 0
	if(Role_if(PM_MONK) && !Upolyd) {
	    if (uarm) {
		Your("armor is rather cumbersome...");
		tmp -= urole.spelarmr;
	    } else if (!uwep && !uarms) {
		tmp += (u.ulevel / 3) + 2;
	}
	}
#endif

	if(Role_if(PM_MONK) && !Upolyd) {
		if(!uwep && (!u.twoweap || !uswapwep) && !uarms && 
		  (!uarm || (uarm->oartifact == ART_HA_MONK) || (uarm->oartifact == ART_BOBAIS) || (uarm->oartifact == ART_AMMY_S_RETRIBUTION) || (uarm && uarm->otyp >= ELVEN_TOGA && 
		  	uarm->otyp <= ROBE_OF_WEAKNESS)))
		  	
		  tmp += (GushLevel / 3) + 2;
		else if (!uwep && (!u.twoweap || !uswapwep)) {
		   pline("Your armor is rather cumbersome...");
		   tmp += (GushLevel / 9) + 1;
		}
	}

	if(Role_if(PM_HALF_BAKED) && uarm && ((uarm->otyp < ROBE) || (uarm->otyp > ROBE_OF_WEAKNESS)) ) {
		pline("Your armor is rather cumbersome...");
		tmp -= 20;
	}

	if(Role_if(PM_HEDDERJEDI) && uarm && !(uarm->oartifact == ART_HA_MONK) && !(uarm->oartifact == ART_BOBAIS) && !(uarm->oartifact == ART_AMMY_S_RETRIBUTION) && ((uarm->otyp < ELVEN_TOGA) || (uarm->otyp > ROBE_OF_WEAKNESS)) ) {
		pline("Your armor is rather cumbersome...");
		tmp -= 20;
	}

	if (Race_if(PM_SWIKNI)) {
		if (uwep) {
			if (uwep->oeroded) tmp -= ((uwep->oeroded) * 2);
			if (uwep->oeroded2) tmp -= ((uwep->oeroded2) * 2);
		}
		if (u.twoweap && uswapwep) {
			if (uswapwep->oeroded) tmp -= ((uswapwep->oeroded) * 2);
			if (uswapwep->oeroded2) tmp -= ((uswapwep->oeroded2) * 2);
		}
	}

	if (Role_if(PM_GUNNER)) {

		if (uwep && uwep_skill_type() != P_DAGGER && uwep_skill_type() != P_KNIFE && uwep_skill_type() != P_FIREARM) {

	/* Code of Conduct- the gunner may not strike anything with any melee weapon, except daggers and knives. */
			You_feel("bad about using such a weapon.");
			adjalign(-1);
		    tmp-=20; // sorry

		}

	}

	if (u.martialstyle == MARTIALSTYLE_CAPOEIRA && (moves > (u.capoeiraturns + 9)) ) tmp -= 5;

	if (uwep && uwep->oartifact == ART_TYPICAL_ORC && u.twoweap) {
		tmp += 4;
		if (uswapwep && uswapwep->oartifact == ART_TYPICAL_ORC) tmp += 10;
	}

	if (uwep && uwep->oartifact == ART_TEH_HUNK && !uwep->lamplit && tmp > 0) tmp += 5;

	if (uwep && uwep->oartifact == ART_CORROSER_LANCE) {
		if (u.usteed) tmp += 5;
		else tmp -= 5;
	}

	if (u.twoweap && uwep && uswapwep && uswapwep->oartifact == ART_TEH_HUNK && (is_launcher(uwep) && !(uwep->otyp == LASERXBOW && uwep->lamplit))) tmp += 5;
	if (u.twoweap && uwep && uswapwep && uswapwep->oartifact == ART_TEH_HUNK && (is_missile(uwep) || is_ammo(uwep))) tmp += 5;
	if (u.twoweap && uwep && uswapwep && uswapwep->oartifact == ART_TEH_HUNK && (!u.usteed && !(tech_inuse(T_POLE_MELEE)) && is_pole(uwep))) tmp += 5;
	if (u.twoweap && uwep && uswapwep && uswapwep->oartifact == ART_TEH_HUNK && (uwep->oclass != WEAPON_CLASS && !is_weptool(uwep) && uwep->oclass != GEM_CLASS && uwep->oclass != BALL_CLASS && uwep->oclass != CHAIN_CLASS && uwep->oclass != VENOM_CLASS)) tmp += 5;

	if (!PlayerCannotUseSkills && ((uarm && uarm->oartifact == ART_HUNKSTERMAN) || (uwep && uwep->oartifact == ART_DE_SID) || Role_if(PM_HEDDERJEDI)) && uwep && (is_launcher(uwep) && !(uwep->otyp == LASERXBOW && uwep->lamplit))) {
		if (u.hunkskill >= 20) tmp++;
		if (u.hunkskill >= 160) tmp++;
		if (u.hunkskill >= 540) tmp++;
		if (u.hunkskill >= 1280) tmp++;
		if (u.hunkskill >= 2500) tmp++;
		if (u.hunkskill >= 4320) tmp++;
	}

	if (!PlayerCannotUseSkills && uwep && uwep->otyp == LIGHTTORCH && !uwep->lamplit) {
		if (u.hunkskill >= 20) tmp++;
		if (u.hunkskill >= 160) tmp++;
		if (u.hunkskill >= 540) tmp++;
		if (u.hunkskill >= 1280) tmp++;
		if (u.hunkskill >= 2500) tmp++;
		if (u.hunkskill >= 4320) tmp++;
	}

	if (!PlayerCannotUseSkills && ((uarm && uarm->oartifact == ART_HUNKSTERMAN) || (uwep && uwep->oartifact == ART_DE_SID) || Role_if(PM_HEDDERJEDI)) && uwep && (is_missile(uwep) || is_ammo(uwep))) {
		if (u.hunkskill >= 20) tmp++;
		if (u.hunkskill >= 160) tmp++;
		if (u.hunkskill >= 540) tmp++;
		if (u.hunkskill >= 1280) tmp++;
		if (u.hunkskill >= 2500) tmp++;
		if (u.hunkskill >= 4320) tmp++;
	}

	if (!PlayerCannotUseSkills && ((uarm && uarm->oartifact == ART_HUNKSTERMAN) || (uwep && uwep->oartifact == ART_DE_SID) || Role_if(PM_HEDDERJEDI)) && uwep && (!u.usteed && !(tech_inuse(T_POLE_MELEE)) && is_pole(uwep))) {
		if (u.hunkskill >= 20) tmp++;
		if (u.hunkskill >= 160) tmp++;
		if (u.hunkskill >= 540) tmp++;
		if (u.hunkskill >= 1280) tmp++;
		if (u.hunkskill >= 2500) tmp++;
		if (u.hunkskill >= 4320) tmp++;
	}

	if (!PlayerCannotUseSkills && ((uarm && uarm->oartifact == ART_HUNKSTERMAN) || (uwep && uwep->oartifact == ART_DE_SID) || Role_if(PM_HEDDERJEDI)) && uwep && (uwep->oclass != WEAPON_CLASS && !is_weptool(uwep) && uwep->oclass != GEM_CLASS && uwep->oclass != BALL_CLASS && uwep->oclass != CHAIN_CLASS && uwep->oclass != VENOM_CLASS)) {
		if (u.hunkskill >= 20) tmp++;
		if (u.hunkskill >= 160) tmp++;
		if (u.hunkskill >= 540) tmp++;
		if (u.hunkskill >= 1280) tmp++;
		if (u.hunkskill >= 2500) tmp++;
		if (u.hunkskill >= 4320) tmp++;
	}

	if( (Role_if(PM_JEDI) || Role_if(PM_SHADOW_JEDI) || Role_if(PM_HEDDERJEDI) || Race_if(PM_BORG)) && !Upolyd) {
		if (((uwep && is_lightsaber(uwep) && uwep->lamplit) ||
		    (uswapwep && u.twoweap && is_lightsaber(uswapwep) && uswapwep->lamplit)) &&
		   (uarm && !(uarm->oartifact == ART_HA_MONK) && !(uarm->oartifact == ART_BOBAIS) && !(uarm->oartifact == ART_AMMY_S_RETRIBUTION) &&
		   (uarm->otyp < ELVEN_TOGA || uarm->otyp > ROBE_OF_WEAKNESS))){
		    char yourbuf[BUFSZ];
		    You("can't use %s %s effectively in this armor...", shk_your(yourbuf, uwep), xname(uwep));
		    tmp-=20; // sorry
		}
	}

	if (uwep && is_lightsaber(uwep) && uwep->lamplit && u.twoweap && uswapwep && is_lightsaber(uswapwep) && uswapwep->lamplit && tech_inuse(T_WILD_SLASHING)) {
		if (tmp > 1) tmp -= (tmp * 2 / 3);
		tmp -= 10;
		tmp -= rn2(21);
	}

	if (uwep && uwep->otyp == GROM_AXE) tmp -= 5;
	if (u.twoweap && uswapwep && uswapwep->otyp == GROM_AXE) tmp -= 5;
	if (uwep && uwep->otyp == ORANGE_LIGHTSABER) tmp -= 5;
	if (u.twoweap && uswapwep && uswapwep->otyp == ORANGE_LIGHTSABER) tmp -= 5;

	/* special class effect uses... */
	if (tech_inuse(T_KIII) && !(tech_inuse(T_WILD_SLASHING)) ) tmp += 4;
	if (tech_inuse(T_BERSERK) && !(tech_inuse(T_WILD_SLASHING)) ) tmp += 2;

	/* if you're fearless, your attacks are also fearless and will strike true more often --Amy */
	if (StrongFear_resistance) tmp += rnd(4);

/*	with a lot of luggage, your agility diminishes */
	if ((tmp2 = near_capacity()) != 0) tmp -= (tmp2*2) - 1;
	if (u.utrap) tmp -= 3;
/*	Some monsters have a combination of weapon attacks and non-weapon
 *	attacks.  It is therefore wrong to add hitval to tmp; we must add
 *	it only for the specific attack (in hmonas()).
 */
/* WAC This is now taken care of later in player's case - for twoweapon */
/*
	if (uwep && !Upolyd) {
		tmp += hitval(uwep, mtmp);
		tmp += weapon_hit_bonus(uwep);
	}
*/
	if (u.twoweap && (P_RESTRICTED(P_TWO_WEAPON_COMBAT) )) tmp -= (issoviet ? 20 : rnd(20) );
	/* In Soviet Russia, you're absolutely not supposed to be able to use twoweaponing if you can't learn the skill. --Amy */

	/* using dual-wielding with a shield (e.g. grim shield) greatly reduces to-hit for balancing reasons --Amy */
	if (u.twoweap && uarms) tmp -= rnd(10);

	if (u.twoweap && !rn2(10)) tmp -= rnd(20);

	if (uwep && tech_inuse(T_UNARMED_FOCUS)) tmp -= rnd(20);
	if (u.twoweap && uswapwep && tech_inuse(T_UNARMED_FOCUS)) tmp -= rnd(20);

	if (u.martialstyle == MARTIALSTYLE_KUNGFU && (uwep || (u.twoweap && uswapwep)) ) tmp -= rnd(20);

	if (u.twoweap && !PlayerCannotUseSkills) {
		switch (P_SKILL(P_ATARU)) {

			case P_BASIC:	tmp +=  1; break;
			case P_SKILLED:	tmp +=  2; break;
			case P_EXPERT:	tmp +=  3; break;
			case P_MASTER:	tmp +=  4; break;
			case P_GRAND_MASTER:	tmp +=  5; break;
			case P_SUPREME_MASTER:	tmp +=  6; break;
			default: tmp += 0; break;
		}
	}

	if (uarmh && uarmh->oartifact == ART_DARK_NADIR) tmp += 5;
	if (uarmh && uarmh->oartifact == ART_RUTH_S_DARK_FORCE) tmp += 5;
	if (uarmh && uarmh->oartifact == ART_NADJA_S_DARKNESS_GENERATOR) tmp += 5;

	if (uarmc && itemhasappearance(uarmc, APP_DNETHACK_CLOAK) ) tmp -= 5;
	if (uarms && uarms->oartifact == ART_DNETHACKC_NOONISHNESS) tmp -= 5;
	if (RngeDnethack) tmp -= 5;
	if (Race_if(PM_INHERITOR)) tmp -= 5;
	if (RngeUnnethack) tmp -= 10;
	if (u.twoweap && RngeNethackFourk) tmp -= rn1(10, 10);

	if (!uwep && !PlayerCannotUseSkills && (P_SKILL(P_MARTIAL_ARTS) >= P_UNSKILLED) && uarmc && itemhasappearance(uarmc, APP_BOXING_GOWN)) tmp += 4;
	/* the P_UNSKILLED is not an error; it means that you have the skill, and are therefore eligible for a bonus --Amy */

	if (!uwep && !PlayerCannotUseSkills && (P_SKILL(P_MARTIAL_ARTS) >= P_UNSKILLED) && RngeMaritalArts) tmp += 5;

	if (uarmc && uarmc->oartifact == ART_DEATHCLAW_HIDE) tmp += 10;

	if (uarmh && uarmh->oartifact == ART_REMOTE_GAMBLE) tmp += 2;
	if (uarm && uarm->oartifact == ART_MOTHERFUCKER_TROPHY) tmp += 5;
	if (u.tiksrvzllatdown) tmp += 5;

	if (uarmg && itemhasappearance(uarmg, APP_UNCANNY_GLOVES)) tmp += 1;
	if (uarmg && itemhasappearance(uarmg, APP_SLAYING_GLOVES)) tmp += 1;

	if (!uwep && StrongGlib_combat && Glib) tmp += 5;

	if (uarmh && uarmh->oartifact == ART_IRON_HELM_OF_GORLIM) tmp += 10;
	if (uarmh && uarmh->oartifact == ART_SUDUNSEL) tmp += 2;
	if (uarm && uarm->otyp == DARK_DRAGON_SCALES) tmp += 1;
	if (uarm && uarm->otyp == DARK_DRAGON_SCALE_MAIL) tmp += 1;
	if (uarms && uarms->otyp == DARK_DRAGON_SCALE_SHIELD) tmp += 1;
	if (uarmg && uarmg->oartifact == ART_FLOEMMELFLOEMMELFLOEMMELFL) tmp += 1;
	if (uwep && uwep->oartifact == ART_WILD_HEAVY_SWINGS) tmp -= 10;
	if (uwep && uwep->oartifact == ART_JUSTICE_FOR_GARLIC) tmp += 5;
	if (uwep && uwep->oartifact == ART_RAFSCHAR_S_SUPERWEAPON) tmp += 1;
	if (uarmc && uarmc->oartifact == ART_ENEMIES_SHALL_LAUGH_TOO) tmp += 10;
	if (uimplant && uimplant->oartifact == ART_ACTUAL_PRECISION) tmp += 5;
	if (uimplant && uimplant->oartifact == ART_RHEA_S_MISSING_EYESIGHT) tmp -= rnd(20);
	if (uleft && uleft->oartifact == ART_CERBERUS_BAND) tmp += 3;
	if (uright && uright->oartifact == ART_CERBERUS_BAND) tmp += 3;
	if (uwep && uwep->oartifact == ART_DOUBLE_BESTARD) tmp -= rnd(20);
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_DOUBLE_BESTARD) tmp -= rnd(20);
	if (uwep && uwep->oartifact == ART_DESANN_S_WRATH) tmp -= 8;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_DESANN_S_WRATH) tmp -= 8;
	if (uwep && uwep->oartifact == ART_WILD_WHIRLING) tmp -= 8;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_WILD_WHIRLING) tmp -= 8;
	if (uwep && uwep->oartifact == ART_MAGE_STAFF_OF_ETERNITY) tmp -= rnd(20);
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_MAGE_STAFF_OF_ETERNITY) tmp -= rnd(20);
	if (uwep && uwep->oartifact == ART_SIGIX_BROADSWORD) tmp -= 5;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_SIGIX_BROADSWORD) tmp -= 5;
	if (uwep && uwep->oartifact == ART_KLOBB) tmp -= 6;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_KLOBB) tmp -= 6;
	if (uwep && uwep->oartifact == ART_EXCALIPOOR) tmp -= 9;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_EXCALIPOOR) tmp -= 9;
	if (uwep && uwep->oartifact == ART_BAD_HITTER_BOY) tmp -= rnd(20);
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_BAD_HITTER_BOY) tmp -= rnd(20);
	if (powerfulimplants() && uimplant && uimplant->oartifact == ART_ACTUAL_PRECISION) tmp += 5;
	if (uleft && uleft->oartifact == ART_BLIND_PILOT) tmp -= 10;
	if (uright && uright->oartifact == ART_BLIND_PILOT) tmp -= 10;
	if (Role_if(PM_ARCHEOLOGIST) && uamul && uamul->oartifact == ART_ARCHEOLOGIST_SONG) tmp += 2;
	if (ublindf && ublindf->oartifact == ART_EYEHANDER) tmp += 5;
	if (uarmf && uarmf->oartifact == ART_CAT_ROCKZ) tmp += 5;
	if (uwep && uwep->oartifact == ART_RIP_STRATEGY) tmp -= 5;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_RIP_STRATEGY) tmp -= 5;
	if (uarmg && uarmg->oartifact == ART_MAJOR_PRESENCE) tmp += 2;
	if (uarmf && uarmf->oartifact == ART_CRASHING_YOUR_SISTER_S_WED) tmp -= 5;
	if (uarmf && uarmf->oartifact == ART_MAY_BRITT_S_ADULTHOOD) tmp -= 2;
	if (uwep && uwep->oartifact == ART_SINSWORD && u.ualign.record < 0) tmp += 1;
	if (uwep && uwep->oartifact == ART_SINSWORD && u.ualign.record < 49) tmp += 1;
	if (uwep && uwep->oartifact == ART_SINSWORD && u.ualign.record < 99) tmp += 1;
	if (uwep && uwep->oartifact == ART_SINSWORD && u.ualign.record < 149) tmp += 1;
	if (uwep && uwep->oartifact == ART_SINSWORD && u.ualign.record < 199) tmp += 1;
	if (uwep && uwep->oartifact == ART_SINSWORD && u.ualign.record < 249) tmp += 1;
	if (uwep && uwep->oartifact == ART_LUCKY_MELEE_ATTACKS) tmp += 10;
	if (StrongBlind_resistance) tmp += rn1(5, 5);
	if (uarmh && uarmh->oartifact == ART_WAITING_FOR_MELEE) tmp -= 2;
	if (bmwride(ART_KERSTIN_S_COWBOY_BOOST)) tmp += 2;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_DUAL_MASTERY) tmp += 5;
	if (uarmf && uarmf->oartifact == ART_PROPERTY_GRUMBLE) tmp -= 5;
	if (uarmg && uarmg->oartifact == ART_UNKNOWINGNESS_AS_A_WEAPON && !(objects[uarmg->otyp].oc_name_known)) tmp += 5;
	if (uwep && uwep->oartifact == ART_FALCO_S_ORB) tmp += 1;
	if (uwep && uwep->oartifact == ART_VERY_SPECIFICNESS) tmp += 1;
	if (uarmg && uarmg->oartifact == ART_PLUS_TO_HIT) tmp += 5;
	if (uarm && uarm->oartifact == ART_WU_WU && u.twoweap) tmp += 4;
	if (uarmh && uarmh->oartifact == ART_BE_THE_LITE) tmp += 1;
	if (u.boosttimer) tmp += 5;
	if (uarms && uarms->oartifact == ART_RONDITSCH) tmp += 1;
	if (powerfulimplants() && uimplant && uimplant->oartifact == ART_NIOBE_S_ANGER) tmp += 4;
	if (uwep && objects[uwep->otyp].oc_material == MT_ADAMANTIUM) tmp += 2;
	if (u.twoweap && uswapwep && objects[uswapwep->otyp].oc_material == MT_ADAMANTIUM) tmp += 2;
	if (uimplant && uimplant->oartifact == ART_I_M_GONNA_CRUSH_YA_) tmp += 4;
	if (bmwride(ART_ZIN_BA)) tmp += 4;
	if (uleft && uleft->oartifact == ART_KRATSCHEM_HARD) tmp += 3;
	if (uarmg && uarmg->oartifact == ART_SOFT_TO_THE_TOUCH) tmp += 5;
	if (uright && uright->oartifact == ART_KRATSCHEM_HARD) tmp += 3;
	if (u.twoweap && uswapwep && uswapwep->otyp == TONFA) tmp += 6;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_SACRIFICE_TONFA) tmp += 5;
	if (uarm && uarm->oartifact == ART_I_AM_YOUR_FALL) tmp += 10;
	if (uarm && uarm->otyp == JEDI_ROBE && uwep && is_lightsaber(uwep) && uwep->lamplit ) tmp += 1;

	if (u.martialstyle == MARTIALSTYLE_TAEKWONDO) tmp -= rnd(10);
	if (u.martialstyle == MARTIALSTYLE_SILAT && !uwep && (!u.twoweap || !uswapwep)) tmp += rn1(6, 6);
	if (u.martialstyle == MARTIALSTYLE_KUNGFU && !uwep && (!u.twoweap || !uswapwep)) tmp += 5;

	if (u.martialstyle == MARTIALSTYLE_HAIDONGGUMDO && uwep && uwep->otyp == JEONTU_GEOM) {
		tmp += 5;
		if (!PlayerCannotUseSkills) {
			switch (P_SKILL(P_MARTIAL_ARTS)) {

				case P_BASIC:	tmp +=  2; break;
				case P_SKILLED:	tmp +=  4; break;
				case P_EXPERT:	tmp +=  6; break;
				case P_MASTER:	tmp +=  8; break;
				case P_GRAND_MASTER:	tmp +=  10; break;
				case P_SUPREME_MASTER:	tmp +=  12; break;
				default: tmp += 0; break;
			}

		}
	}

	if (u.martialstyle == MARTIALSTYLE_CAPOEIRA && u.capoeiracheck && !uwep && (!u.twoweap || !uswapwep)) {
		tmp += rnd(10);
	}

	if (PlayerInWedgeHeels && !PlayerCannotUseSkills) {
		switch (P_SKILL(P_WEDGE_HEELS)) {
			default: break;
			case P_BASIC: tmp += 1; break;
			case P_SKILLED: tmp += 2; break;
			case P_EXPERT: tmp += 3; break;
			case P_MASTER: tmp += 4; break;
			case P_GRAND_MASTER: tmp += 5; break;
			case P_SUPREME_MASTER: tmp += 6; break;
		}
	}

	if (u.martialstyle == MARTIALSTYLE_HAIDONGGUMDO && !(uwep && uwep->otyp == JEONTU_GEOM)) tmp -= 10;

	if (uwep && uwep->oartifact == ART_AK_____) {
		if (!PlayerCannotUseSkills) {
			if (P_SKILL(P_FIREARM) < P_BASIC) tmp += 6;
			else if (P_SKILL(P_FIREARM) == P_BASIC) tmp += 4;
			else if (P_SKILL(P_FIREARM) == P_SKILLED) tmp += 2;
		}
		if (P_SKILL(P_FIREARM) == P_MASTER) tmp -= 2;
		if (P_SKILL(P_FIREARM) == P_GRAND_MASTER) tmp -= 4;
		if (P_SKILL(P_FIREARM) == P_SUPREME_MASTER) tmp -= 6;
	}
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_AK_____) {
		if (P_SKILL(P_FIREARM) == P_MASTER) tmp -= 2;
		if (P_SKILL(P_FIREARM) == P_GRAND_MASTER) tmp -= 4;
		if (P_SKILL(P_FIREARM) == P_SUPREME_MASTER) tmp -= 6;
	}

	if (uwep && uwep->oartifact == ART_LONGLOSS) tmp -= rnd(10);
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_LONGLOSS) tmp -= rnd(10);
	if (uwep && uwep->oartifact == ART_MISS_DOES_IT_REAL) tmp -= 10;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_MISS_DOES_IT_REAL) tmp -= 10;
	if (uwep && uwep->oartifact == ART_UNWIELDYTINE) tmp -= rn1(5, 5);
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_UNWIELDYTINE) tmp -= rn1(5, 5);
	if (uarmc && uarmc->oartifact == ART_OLD_PERSON_TALK) tmp -= 5;
	if (uwep && uwep->oartifact == ART_BLACK_MARK) tmp -= 1;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_BLACK_MARK) tmp -= 1;
	if (uarm && uarm->oartifact == ART_POWASPEL) tmp -= 3;
	if (uwep && uwep->oartifact == ART_HOW_IS_THE_CAR_ROWED) tmp -= 6;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_HOW_IS_THE_CAR_ROWED) tmp -= 6;
	if (uwep && uwep->oartifact == ART_BEEEEEEEP) tmp -= 3;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_BEEEEEEEP) tmp -= 3;
	if (uwep && uwep->oartifact == ART_UZ_I) tmp -= rnd(10);
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_UZ_I) tmp -= rnd(10);
	if (uwep && uwep->oartifact == ART_FOR_THE_REAL_GAMING_EXPERI) tmp -= 5;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_FOR_THE_REAL_GAMING_EXPERI) tmp -= 5;

	if (uarmf && uarmf->oartifact == ART_MELISSA_S_BEAUTY) tmp += 5;
	if (uarmg && uarmg->oartifact == ART_SI_OH_WEE) tmp += 2;
	if (uimplant && uimplant->oartifact == ART_SOME_LITTLE_AID) tmp += 1;
	if (Race_if(PM_SERB)) tmp += 1;

	if (Role_if(PM_OTAKU) && uarmc && itemhasappearance(uarmc, APP_FOURCHAN_CLOAK)) tmp += 1;

	/* being on the menstrual phase means you hit a lot less but if you do hit, you deal more damage --Amy */
	if (FemtrapActiveNatalia && flags.female && (u.nataliacycletimer >= u.nataliafollicularend) && (u.nataliacycletimer < (u.nataliafollicularend + u.natalialutealstart)) ) tmp -= 5;

	if (Role_if(PM_EMERA) && mtmp->female && humanoid(mtmp->data)) tmp += rnd(u.ulevel);

	if (uwep && uwep->oartifact == ART_ORE_DEPOSIT && lithivorous(mtmp->data)) tmp += 5;

	if (tech_inuse(T_CONCENTRATING) && !(tech_inuse(T_WILD_SLASHING)) ) tmp += 50;
	if (Race_if(PM_MONGUNG)) tmp += 10;

	if (Race_if(PM_BOVER)) {
		if (uarm && is_metallic(uarm)) tmp -= rnd(3);
		if (uarmu && is_metallic(uarmu)) tmp -= rnd(3);
		if (uarmc && is_metallic(uarmc)) tmp -= rnd(3);
		if (uarms && is_metallic(uarms)) tmp -= rnd(3);
		if (uarmh && is_metallic(uarmh)) tmp -= rnd(3);
		if (uarmg && is_metallic(uarmg)) tmp -= rnd(3);
		if (uarmf && is_metallic(uarmf)) tmp -= rnd(3);
	}

	if (uwep && uwep->oartifact == ART_VLADSBANE) tmp -= 5;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_VLADSBANE) tmp -= 5;
	if (uwep && uwep->oartifact == ART_CHARGING_MADE_EASY) tmp -= 5;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_CHARGING_MADE_EASY) tmp -= 5;

	if (Race_if(PM_PLAYER_SKELETON)) tmp -= u.ulevel; /* sorry */

	if (Race_if(PM_DEVELOPER)) tmp += 3;

	if (is_table(u.ux, u.uy)) tmp += 3;

	if (is_grassland(u.ux, u.uy) && !(uarm && uarm->oartifact == ART_DORL_TSCH) && !(uarmf && itemhasappearance(uarmf, APP_GARDEN_SLIPPERS))) tmp -= rnd(5);
	if (Race_if(PM_VIETIS)) tmp -= rnd(10);

	if (humanoid(mtmp->data) && is_female(mtmp->data) && FemtrapActiveWendy) tmp -= rnd(SuperFemtrapWendy ? 20 : 10);

	if (uwep && uwep->oartifact == ART_LUCKLESS_FOLLY && (Luck > 0)) tmp -= Luck;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_LUCKLESS_FOLLY && (Luck > 0)) tmp -= Luck;

	if (!u.twoweap && uwep && uwep->oartifact == ART_LUCKLESS_FOLLY && (Luck < 0)) tmp -= Luck;

	/* early-game bonuses to make starting characters not suck too badly --Amy */
	if (u.ulevel < 6) tmp += 1;
	if (u.ulevel < 2) tmp += 1;
	if (u.ulevel < 5 && rn2(2)) tmp += 1;
	if (u.ulevel < 3 && rn2(2)) tmp += 1;

	if (GushLevel > 5) tmp += 1;
	if (GushLevel > 9) tmp += 1;
	if (GushLevel > 12) tmp += 1;
	if (GushLevel > 15) tmp += 1;
	if (GushLevel > 19) tmp += 1;
	if (GushLevel > 23) tmp += 1;
	if (GushLevel > 26) tmp += 1;
	if (GushLevel > 29) tmp += 1;

	if (tech_inuse(T_STEADY_HAND) && !(tech_inuse(T_WILD_SLASHING)) ) tmp += 5;

	if (!issoviet && !rn2(3)) tmp += rno(GushLevel);

	if (InvertedState) {
		if (uwep && (uwep->spe > 0)) tmp -= (uwep->spe * 2);
		if (u.twoweap && uswapwep && (uswapwep->spe > 0)) tmp -= (uswapwep->spe * 2);
	}

	if (Numbed) {
		if (tmp > 1) {
			tmp *= 9;
			tmp /= 10;
		}
		tmp -= 2;
	}

	/* heavy weapons are inaccurate versus tiny monsters (e.g. rats or insects) --Amy */
	if (uwep && is_heavyweapon(uwep) && verysmall(mtmp->data) && tmp > 1) {

		int heavyreduction = 50;

		if (!PlayerCannotUseSkills) {
			switch (P_SKILL(P_TWO_HANDED_WEAPON)) {
				case P_BASIC: heavyreduction = 58; break;
				case P_SKILLED: heavyreduction = 66; break;
				case P_EXPERT: heavyreduction = 75; break;
				case P_MASTER: heavyreduction = 83; break;
				case P_GRAND_MASTER: heavyreduction = 91; break;
				case P_SUPREME_MASTER: heavyreduction = 100; break;
				default: heavyreduction = 50; break;
			}
		}

		if (heavyreduction < rnd(100)) tmp /= 2;
	}
	/* but they're particularly good at hitting huge monsters */
	if (uwep && is_heavyweapon(uwep) && hugemonst(mtmp->data)) {
		tmp += rnd(5);
	}

	if (u.tremblingamount) tmp -= rnd(u.tremblingamount);

	if (!rn2(20)) tmp -= 20; /* catastrophic failure on a "natural 20", similar to D&D --Amy */
	if (Race_if(PM_INHERITOR) && !rn2(100)) tmp -= 20;

	if (Race_if(PM_NEMESIS) && !is_swimmer(mtmp->data) && !is_waterypool(mtmp->mx, mtmp->my) ) tmp -= rnd(10);

	if (Role_if(PM_FAILED_EXISTENCE) && rn2(2)) tmp = -100; /* 50% chance of automiss --Amy */
	if (uarmc && uarmc->oartifact == ART_ARTIFICIAL_FAKE_DIFFICULTY && !rn2(6)) tmp = -100;

	/*if (tmp < -127) tmp = -127;*/ /* fail safe, and to ensure that the end result is a schar */
	/*if (tmp > 127) tmp = 127;*/ /* however, why is it a schar anyway??? --Amy */
	/* trying to make it an int instead --Amy */

	return tmp;
}

/* try to attack; return FALSE if monster evaded */
/* u.dx and u.dy must be set */
boolean
attack(mtmp)
register struct monst *mtmp;
{
	int tmp; /* used to be an schar for a reason not known to me, after all hmonas() uses an int --Amy */
	register struct permonst *mdat = mtmp->data;
	int mhit;
	struct trap *trap;

	/* Double and quad attacks gives extra attacks per round --Amy
	 * This means you do all of your normal attacks two or four times. */
	int attackamount = 0;
	if (Double_attack || (u.twoweap && uswapwep && uswapwep->oartifact == ART_SMASH_TONFA) || (uwep && uwep->oartifact == ART_MELISSA_S_PEACEBRINGER && !u.twoweap) || (uwep && uwep->oartifact == ART_CRUSHING_IMPACT && !u.twoweap) ) attackamount += rn2(2);
	if (Quad_attack) attackamount += rn2(4); /* don't always give the full amount (balance reasons) --Amy */

	/* whirlstaff technique... this is very powerful when polymorphed into a form with several other attacks --Amy */
	if (uwep && weapon_type(uwep) == P_QUARTERSTAFF && tech_inuse(T_WHIRLSTAFF)) {
		attackamount++;
		if (!PlayerCannotUseSkills && P_SKILL(P_QUARTERSTAFF) >= P_GRAND_MASTER) attackamount++;
		if (!PlayerCannotUseSkills && P_SKILL(P_QUARTERSTAFF) >= P_SUPREME_MASTER) attackamount++;
	}

	if (uwep && is_lightsaber(uwep) && uwep->lamplit && u.twoweap && uswapwep && is_lightsaber(uswapwep) && uswapwep->lamplit && tech_inuse(T_WILD_SLASHING)) {
		attackamount++;
	}

	/* This section of code provides protection against accidentally
	 * hitting peaceful (like '@') and tame (like 'd') monsters.
	 * Protection is provided as long as player is not: blind, confused,
	 * hallucinating or stunned.
	 * changes by wwp 5/16/85
	 * More changes 12/90, -dkh-. if its tame and safepet, (and protected
	 * 07/92) then we assume that you're not trying to attack. Instead,
	 * you'll usually just swap places if this is a movement command
	 */
	/* Intelligent chaotic weapons (Stormbringer) want blood */
	if (is_safepet(mtmp) && !flags.forcefight) {
	    if (!(BloodthirstyAttacking || (uwep && weapon_is_bloodthirsty(uwep)) || (u.twoweap && uswapwep && weapon_is_bloodthirsty(uswapwep)) ) ) {
		/* there are some additional considerations: this won't work
		 * if in a shop or Punished or you miss a random roll or
		 * if you can walk thru walls and your pet cannot (KAA) or
		 * if your pet is a long worm (unless someone does better).
		 * there's also a chance of displacing a "frozen" monster.
		 * sleeping monsters might magically walk in their sleep.
		 */

		/* the stuuuuuuuuuupid pet should be less likely to fail displacing if your skill is high --Amy */
		int petdisplacechance = 7;
		if (!PlayerCannotUseSkills) {
			switch (P_SKILL(P_PETKEEPING)) {
		      	case P_BASIC:	petdisplacechance = 8; break;
		      	case P_SKILLED:	petdisplacechance = 9; break;
		      	case P_EXPERT:	petdisplacechance = 10; break;
		      	case P_MASTER:	petdisplacechance = 12; break;
		      	case P_GRAND_MASTER:	petdisplacechance = 15; break;
		      	case P_SUPREME_MASTER:	petdisplacechance = 20; break;
		      	default: break;
			}

		}

		boolean foo = (Punished || !rn2(petdisplacechance) || is_longworm(mtmp->data)),
			inshop = FALSE;
		char *p;

		for (p = in_rooms(mtmp->mx, mtmp->my, SHOPBASE); *p; p++)
		    if (tended_shop(&rooms[*p - ROOMOFFSET])) {
			inshop = TRUE;
			break;
		    }

		if (inshop || foo ||
			(IS_ROCKWFL(levl[u.ux][u.uy].typ) && (!mtmp->egotype_wallwalk) &&
					!passes_walls(mtmp->data))) {
		    char buf[BUFSZ];

		    monflee(mtmp, rnd(6), FALSE, FALSE);
		    strcpy(buf, y_monnam(mtmp));
		    buf[0] = highc(buf[0]);
		    You("stop.  %s is in the way!", buf);
		    return(TRUE);
		} else if ((mtmp->mfrozen || (! mtmp->mcanmove)
				|| (mtmp->data->mmove == 0)) && rn2(6)) {
		    pline("%s doesn't seem to move!", Monnam(mtmp));
		    return(TRUE);
		} else return(FALSE);
	    }
	}

	/* possibly set in attack_checks;
	   examined in known_hitum, called via hitum or hmonas below */
	override_confirmation = 0;
	mhit = attack_checks(mtmp, !uwep);
	if (!mhit) return(TRUE);

	if (trap = t_at(u.ux, u.uy)) {
		if (trap->ttyp == VIVISECTION_TRAP) {
			You("are in vivisection, and therefore unable to attack!");
			mtmp->mstrategy &= ~STRAT_WAITMASK;
			goto atk_done;
		}
	}

	if (Upolyd) {
		/* certain "pacifist" monsters don't attack */
		if(noattacks(youmonst.data) && (!uactivesymbiosis || noattacks(&mons[u.usymbiote.mnum])) ) {
			You("have no way to attack monsters physically.");
			mtmp->mstrategy &= ~STRAT_WAITMASK;
			goto atk_done;
		}
	}

	if (mtmp->handytime) {
		You("aren't allowed to attack monsters who are currently on the phone.");
		return FALSE;
	}

	if(check_capacity("You cannot fight while so heavily loaded."))
	    goto atk_done;

	if (u.twoweap && !can_twoweapon())
		untwoweapon();

	if(unweapon) {
	    unweapon = FALSE;
	    if(flags.verbose) {
		if(uwep)
		    You("begin bashing monsters with your %s.",
			aobjnam(uwep, (char *)0));
		else if (tech_inuse(T_EVISCERATE))
		    You("begin slashing monsters with your claws.");
		else if (!cantwield(youmonst.data)) {
		    if (!(PlayerCannotUseSkills) && P_SKILL(P_MARTIAL_ARTS) >= P_EXPERT)
			You("assume a martial arts stance.");
		    else You("begin %sing monsters with your %s %s.",
			(Role_if(PM_MONK) || Role_if(PM_HALF_BAKED)) ? "strik" : "bash",
			uarmg ? "gloved" : "bare",	/* Del Lamb */
			makeplural(body_part(HAND)));
	    }
	}
	}

	exercise(A_STR, TRUE);		/* you're exercising muscles */
	/* andrew@orca: prevent unlimited pick-axe attacks */
	u_wipe_engr(3);

	/* Is the "it died" check actually correct? */
	if(mdat->mlet == S_LEPRECHAUN && !mtmp->mfrozen && !mtmp->msleeping &&
	   !mtmp->mconf && mtmp->mcansee && !rn2(7) &&
	   (m_move(mtmp, 0) == 2 ||			    /* it died */
	   mtmp->mx != u.ux+u.dx || mtmp->my != u.uy+u.dy)) { /* it moved */

		if (FunnyHallu) pline("%s uses a pogo stick!", Monnam(mtmp));
		else You("miss wildly and stumble forwards.");

		/* evil variant by Amy: paralysis */
		if (evilfriday) {
			if (isstunfish) nomul(-(rnz(5) ), "stumbling forwards and falling down", TRUE);
			else nomul(-(rnd(5) ), "stumbling forwards and falling down", TRUE);
			nomovemsg = "You manage to get up again.";
		}

		return(FALSE);
	}

	if (FemtrapActiveJulia && !rn2(5) && mtmp->female && humanoid(mtmp->data) ) {
		u.cnd_juliatrapcnt++;
		You("are held back by the referee, who states that it's unfair of you to try to attack %s!", mon_nam(mtmp));
		return(FALSE);
	}

	tmp = find_roll_to_hit(mtmp);
	
newroundofattacks:
	(void) hmonas(mtmp, tmp); /* hmonas handles all attacks now */
	
	/* berserk lycanthropes calm down after the enemy is dead */
	if (mtmp->mhp <= 0) repeat_hit = 0;
	if (mtmp->mhp > 0 && (attackamount > 0)) {
		attackamount--;
		goto newroundofattacks;
	}
/*
	if (Upolyd)
		(void) hmonas(mtmp, tmp);
	else
		(void) hitum(mtmp, tmp, mhit, youmonst.data->mattk);
*/		
	mtmp->mstrategy &= ~STRAT_WAITMASK;

atk_done:
	/* see comment in attack_checks() */
	/* we only need to check for this if we did an attack_checks()
	 * and it returned 0 (it's okay to attack), and the monster didn't
	 * evade.
	 */
	if (flags.forcefight && mtmp->mhp > 0 && !canspotmon(mtmp) && !(mtmp->data->msound == MS_DEEPSTATE) && !(mtmp->egotype_deepstatemember) &&
	    !memory_is_invisible(u.ux+u.dx, u.uy+u.dy) &&
	    !(u.uswallow && mtmp == u.ustuck))
		map_invisible(u.ux+u.dx, u.uy+u.dy);

	return(TRUE);
}

STATIC_OVL boolean
known_hitum(mon, mattack, mhit, uattk, dieroll)   /* returns TRUE if monster still lives */
register struct monst *mon;
int mattack;			/* Which weapons you attacked with -ALI */
register int *mhit;
struct attack *uattk;
int dieroll;
{
	register boolean malive = TRUE;

	if (override_confirmation || BloodthirstyAttacking) {
	    /* this may need to be generalized if weapons other than Stormbringer acquire similar anti-social behavior...
	     * Amy edit: not just Stormbringer, but plenty of other weapons, and also a general nastytrap effect */
	    if (flags.verbose)
		if (override_confirmation == HIT_UWEP)
		    Your("bloodthirsty weapon attacks!");
		else
		    Your("bloodthirsty attack will not be thwarted!");
	}

	if(!*mhit) {
	    if (mattack & HIT_UWEP)
		missum(mon, tohit(UWEP_ROLL), dice(UWEP_ROLL), uattk);
	    if (mattack & HIT_USWAPWEP)
	    	missum(mon, tohit(USWAPWEP_ROLL), dice(USWAPWEP_ROLL), uattk);
	} else {
	    int oldhp = mon->mhp,
		x = u.ux + u.dx, y = u.uy + u.dy;

	    /* we hit the monster; be careful: it might die or
	       be knocked into a different location */
	    notonhead = (mon->mx != x || mon->my != y);
	    if (*mhit & HIT_UWEP) {
		/* KMH, conduct */
		if (uwep && (uwep->oclass == WEAPON_CLASS || uwep->oclass == BALL_CLASS || uwep->oclass == CHAIN_CLASS || uwep->oclass == VENOM_CLASS || is_weptool(uwep)))
		    u.uconduct.weaphit++;
		dieroll = dice(UWEP_ROLL);
		malive = hmon(mon, uwep, 0, dieroll);
	    } else if (mattack & HIT_UWEP)
		missum(mon, tohit(UWEP_ROLL), dice(UWEP_ROLL), uattk);
	    if ((mattack & HIT_USWAPWEP) && malive && m_at(x, y) == mon) {
		/* KMH, ethics */
	    	if (*mhit & HIT_USWAPWEP) {
		    if (uswapwep) u.uconduct.weaphit++;
		    dieroll = dice(USWAPWEP_ROLL);
		    malive = hmon(mon, uswapwep, 0, dieroll);
	    	} else
		    missum(mon, tohit(USWAPWEP_ROLL), dice(USWAPWEP_ROLL), uattk);
	    }
	    if (malive) {
		/* monster still alive */

		int juyofleechance = 25;

		/* roles and races that have caitiff conducts might dislike the increased fleeing chance, so it's now possible
		 * to switch back and forth via #monster (see also cmd.c) --Amy */
		if (!PlayerCannotUseSkills && u.juyofleeing) {
			switch (P_SKILL(P_JUYO)) {

				case P_BASIC:	juyofleechance =  20; break;
				case P_SKILLED:	juyofleechance =  15; break;
				case P_EXPERT:	juyofleechance =  10; break;
				case P_MASTER:	juyofleechance =  5; break;
				case P_GRAND_MASTER:	juyofleechance =  3; break;
				case P_SUPREME_MASTER:	juyofleechance =  2; break;
				default: juyofleechance = 25; break;
			}

		}

		if(!rn2(juyofleechance) && mon->mhp < mon->mhpmax/2
			    && !(u.uswallow && mon == u.ustuck)) {
		    /* maybe should regurgitate if swallowed? */
		    if(!rn2(3)) {
			monflee(mon, rnd(100), FALSE, TRUE);
		    } else monflee(mon, rnd(10), FALSE, TRUE);

		    if(u.ustuck == mon && !u.uswallow && !sticks(youmonst.data))
			setustuck(0);
		}
		/* Vorpal Blade hit converted to miss */
		/* could be headless monster or worm tail */
		if (mon->mhp == oldhp) {
		    *mhit = 0;
		    /* a miss does not break conduct */
		    if (uwep &&
			(uwep->oclass == WEAPON_CLASS || uwep->oclass == BALL_CLASS || uwep->oclass == CHAIN_CLASS || uwep->oclass == VENOM_CLASS || is_weptool(uwep)))
			--u.uconduct.weaphit;
		}
		if (mon->wormno && *mhit) {
		    int dohit = *mhit;
		    if (!u.twoweap || (dohit & HIT_UWEP)) {
			if (cutworm(mon, x, y, uwep))
			    dohit = 0;	/* Don't try and cut a worm twice */
		    }
		    if (u.twoweap && (dohit & HIT_USWAPWEP))
			(void) cutworm(mon, x, y, uswapwep);
		}
	    }

	    /* Lycanthropes sometimes go a little berserk! 
	     * If special is on,  they will multihit and stun!
	     */
	    if (( (Race_if(PM_HUMAN_WEREWOLF) || Role_if(PM_YAUTJA) || Role_if(PM_LUNATIC) ) && (mon->mhp > 0)) ||
				tech_inuse(T_EVISCERATE)) {
		if (tech_inuse(T_EVISCERATE)) {
		    /*make slashing message elsewhere*/
		    if (repeat_hit == 0) {
			/* [max] limit to 4 (0-3) */
			repeat_hit = (tech_inuse(T_EVISCERATE) > 5) ?
						4 : (tech_inuse(T_EVISCERATE) - 2);
			/* [max] limit to 4 */
			mon->mfrozen = (tech_inuse(T_EVISCERATE) > 5) ?
						4 : (tech_inuse(T_EVISCERATE) - 2); 
		    }
		    mon->mstun = 1;
		    mon->mcanmove = 0;
		} else if (!rn2(24)) {
		    repeat_hit += rn2(4)+1;
		    mon->mfrozen = repeat_hit; /* Lycanthropes suck badly enough already, k? --Amy */
		    mon->mcanmove = 0;
		    /* Length of growl depends on how angry you get */
		    switch (repeat_hit) {
		    	case 0: /* This shouldn't be possible, but... */
			case 1: pline("Grrrrr!"); break;
			case 2: pline("Rarrrgh!"); break;
			case 3: pline("Grrarrgh!"); break;
			case 4: pline("Rarggrrgh!"); break;
			case 5: pline("Raaarrrrrr!"); break;
			case 6: 
			default:pline("Grrrrrrarrrrg!"); break;
		    }
		}
	    }
	}
	return(malive);
}

#if 0 /* Obsolete */
STATIC_OVL boolean
hitum(mon, tmp, mhit, uattk)          /* returns TRUE if monster still lives */
struct monst *mon;
int tmp;
int mhit;
struct attack *uattk;
{
	boolean malive;
	int mattack = mhit;
	int tmp1 = tmp, tmp2 = tmp;

	if (mhit & HIT_UWEP)
	{
		if (uwep) tmp1 += hitval(uwep, mon);
	
		tohit(UWEP_ROLL) = tmp1;
	
		if (tmp1 <= (dice(UWEP_ROLL) = rnd(20)) && !u.uswallow)
			mhit &= ~HIT_UWEP;

		if (tmp1 > dice(UWEP_ROLL)) exercise(A_DEX, TRUE);
#ifdef DEBUG
		pline("(%i/20)", tmp1);
#endif
	}
	
	if (mhit & HIT_USWAPWEP && u.twoweap) {
		if (uswapwep) tmp2 += hitval(uswapwep, mon) - 2;

		tohit(USWAPWEP_ROLL) = tmp2;

		if (tmp2 <= (dice(USWAPWEP_ROLL) = rnd(20)) && !u.uswallow)
			mhit &= ~HIT_USWAPWEP;

		if (tmp2 > dice(USWAPWEP_ROLL)) exercise(A_DEX, TRUE);
#ifdef DEBUG
		pline("((%i/20))", tmp2);
#endif
	}
	
	malive = known_hitum(mon, mattack, &mhit, uattk, dieroll);
	(void) passive(mon, mhit, malive, AT_WEAP, FALSE);
	/* berserk lycanthropes calm down after the enemy is dead */
	if (!malive) repeat_hit = 0;
	return(malive);
}
#endif

/* WAC Seperate martial arts damage function */
int
martial_dmg()
{
        int damage;
	  int damagetodouble = 0;
        /* WAC   plateau at 16 if Monk and Grand Master (6d4)
                            13 if Grand Master
                            11 if Master
                             9 if Expert
                             7 if Skilled
                             5 if Basic  (1d4)
         */

	  if (Role_if(PM_HALF_BAKED) && !Upolyd) {
		damage = rnd(2);
		if (!(PlayerCannotUseSkills) && (P_SKILL(P_MARTIAL_ARTS) >= P_BASIC)) {
			damage += rnd(P_SKILL(P_MARTIAL_ARTS) - P_UNSKILLED);
			if (!rn2(2)) damage += rnd(P_SKILL(P_MARTIAL_ARTS) - P_UNSKILLED);
		}

        } else if ((Role_if(PM_MONK) && !Upolyd && !(PlayerCannotUseSkills) )
                && (P_SKILL(P_MARTIAL_ARTS) >= P_GRAND_MASTER)
                && (GushLevel > 16)) damage = d(6,2) + (P_SKILL(P_MARTIAL_ARTS) == P_SUPREME_MASTER ? rnd(10) : 0) ;                                
        else if (!(PlayerCannotUseSkills) && (P_SKILL(P_MARTIAL_ARTS) >= P_BASIC) && GushLevel > (2*(P_SKILL(P_MARTIAL_ARTS) - P_BASIC) + 5))
                damage = d((int) (P_SKILL(P_MARTIAL_ARTS) - P_UNSKILLED),2);
        else
                damage = rnd(2);

	  if (!Role_if(PM_HALF_BAKED)) {

	        if(( (!uarm && rn2(2)) || (uarm && (uarm->otyp >= ROBE && uarm->otyp <= ROBE_OF_WEAKNESS))) && (!uarms) && (damage > 1))
	                damage += rnd(damage);
	        else {
			if (damage > 1) damage += rnd(2);
			else damage++;
		  }

	  }

	damagetodouble = damage;

	/* base damage calculation end; unarmed focus tech doubles this, but doesn't double the bonuses below --Amy */
	if (tech_inuse(T_UNARMED_FOCUS)) {
		damage += (damagetodouble / 2);
	}
	if (uarmg && uarmg->otyp == GAUNTLETS_OF_FISTFIGHTING) {
		damage += damagetodouble;
	}
	if (uarmg && uarmg->oartifact == ART_PU_PU) {
		damage += damagetodouble;
	}

	if (uarmg && itemhasappearance(uarmg, APP_BOXING_GLOVES) ) damage += 1;

	if (u.martialstyle == MARTIALSTYLE_KRAVMAGA) damage += rn1(5, 5);

	if (uarmg && uarmg->oartifact == ART_BOX_FIST) damage += 5;
	if (uarmg && uarmg->oartifact == ART_BOXING_LESSON) damage += 5;
	if (uarmg && uarmg->oartifact == ART_FIFTY_SHADES_OF_FUCKED_UP) damage += 5;
	if (uarm && uarm->oartifact == ART_GRANDMASTER_S_ROBE) damage += 10;
	if (uarm && uarm->oartifact == ART_MONKSTERMAN) damage += 5;
	if (RngeMaritalArts) damage += 5;
	if (u.martialstyle == MARTIALSTYLE_CAPOEIRA && u.capoeiracheck) {
		damage += rnd((u.ulevel / 3) + 1);
	}

	if (Glib_combat && IsGlib) {

		damage += rnd(GushLevel);
		if (StrongGlib_combat) damage += 5;

	}

	if (u.combostrike) {
		damage += u.combostrike;
		pline("+%d combo!", u.combostrike);
	}
	if (tech_inuse(T_COMBO_STRIKE)) {
		u.combostrike++;
		u.comboactive = TRUE;
	}

	if (uarmc && itemhasappearance(uarmc, APP_BOXING_GOWN)) damage += 2;

	/* In Soviet Russia, people LOVE bugs. They love them so much, they even go out of their way to put them back into
	 * the game, because the Amy was the one to remove them and by definition the Amy is the antichrist. --Amy */

	  if (issoviet && (damage > 1) && !rn2(4)) damage = 1;

        return (damage);
}

boolean			/* general "damage monster" routine */
hmon(mon, obj, thrown, dieroll)		/* return TRUE if mon still alive */
struct monst *mon;
struct obj *obj;
int thrown;	/* 0: not thrown, 1: launched with uwep,
		   2: launched with uswapwep, 3: thrown by some other means */
int dieroll;
{
	boolean result, anger_guards;
	struct obj *obj2;

	anger_guards = (mon->mpeaceful &&
			    (mon->ispriest || mon->isshk ||
			     mon->data == &mons[PM_WATCHMAN] ||
			     mon->data == &mons[PM_WATCH_LIEUTENANT] ||
			     mon->data == &mons[PM_WATCH_LEADER] ||
			     mon->data == &mons[PM_WATCH_CAPTAIN]));
	result = hmon_hitmon(mon, obj, thrown, dieroll);
	if (mon->ispriest && !rn2(2)) ghod_hitsu(mon);

	if (mon->data == &mons[PM_HUGE_OGRE_THIEF]) {

/* hitting this monster thrice means potential instadeath, unless those hits are enough to kill it by damage. --Amy */

		if (result && mon->ogrethief < 3) mon->ogrethief++;
		else if (result) {
			monkilled(mon, "", AD_PHYS);
			pline("You kill the smirking sneak thief!");

			if (!rn2(20) && (!Antimagic && !is_undead(youmonst.data) )) {
				u.youaredead = 1;
				killer_format = KILLED_BY;
				killer = "hitting a huge ogre thief thrice";
				done(DIED);
				u.youaredead = 0;
			}
			else {
				if (Upolyd) losehp(rnz(u.mhmax), "hitting a huge ogre thief thrice", KILLED_BY);
				losehp(rnz(u.uhpmax), "hitting a huge ogre thief thrice", KILLED_BY);

			}
		}
	}

	if (anger_guards) (void)angry_guards(!flags.soundok);
	return result;
}

/* guts of hmon() */
STATIC_OVL boolean
hmon_hitmon(mon, obj, thrown, dieroll)
struct monst *mon;
struct obj *obj;
int thrown;
int dieroll;
{
	int tmp, canhitmon = 0, objenchant;
	struct permonst *mdat = mon->data;
	int barehand_silver_rings = 0;
	/* The basic reason we need all these booleans is that we don't want
	 * a "hit" message when a monster dies, so we have to know how much
	 * damage it did _before_ outputting a hit message, but any messages
	 * associated with the damage don't come out until _after_ outputting
	 * a hit message.
	 */
	boolean hittxt = FALSE, destroyed = FALSE, already_killed = FALSE;
	boolean get_dmg_bonus = TRUE;
	boolean ispoisoned = FALSE, needpoismsg = FALSE, poiskilled = FALSE;
	boolean silvermsg = FALSE, silverobj = FALSE;
	boolean vivaobj = FALSE;
	boolean copperobj = FALSE;
	boolean mercurialobj = FALSE;
	boolean platinumobj = FALSE;
	int cursedobj = 0;
	boolean inkaobj = FALSE;
	boolean odorobj = FALSE;
	boolean valid_weapon_attack = FALSE;
	boolean unarmed = !uwep && !uarm && !uarms;
	int jousting = 0;
	boolean vapekilled = FALSE; /* WAC added boolean for vamps vaporize */
	boolean burnmsg = FALSE;
	boolean no_obj = !obj;	/* since !obj can change if weapon breaks, etc. */
	boolean noeffect;
	int wtype;
	struct obj *monwep;
	struct obj *launcher;
	char yourbuf[BUFSZ];
	char unconventional[BUFSZ];	/* substituted for word "attack" in msg */
	char saved_oname[BUFSZ];
	char cutbuf[BUFSZ];
	char buf[BUFSZ];

	boolean sanitymessage = (u.usanity > rn2(1000));

	boolean stupidrock = 0;
	if (thrown && obj->otyp == ROCK) stupidrock = 1;

	boolean pieks = 0;
	if (obj && thrown == 1 && objects[obj->otyp].oc_skill == P_POLEARMS) pieks = 1;
	if (obj && thrown == 1 && objects[obj->otyp].oc_skill == P_LANCE) pieks = 1;
	if (obj && thrown == 1 && objects[obj->otyp].oc_skill == P_GRINDER) pieks = 1;
	if (obj && thrown == 1 && obj->otyp == GRAPPLING_HOOK) pieks = 1;
	if (obj && thrown == 1 && obj->otyp == JACK_KNIFE) pieks = 1;
	if (obj && thrown == 1 && obj->otyp == LAJATANG) pieks = 1;

	if (thrown == 1) launcher = uwep;
	else if (thrown == 2) launcher = uswapwep;
	else if (thrown == 666 && uarmf && itemhasappearance(uarmf, APP_PISTOL_BOOTS)) launcher = uarmf;
	else launcher = 0;

	boolean gunused = 0;
	boolean crossbowused = 0;
	if (obj && launcher && ammo_and_launcher(obj, launcher) && objects[launcher->otyp].oc_skill == P_FIREARM) gunused = 1;
	if (obj && launcher && ammo_and_launcher(obj, launcher) && objects[launcher->otyp].oc_skill == P_CROSSBOW) crossbowused = 1;

	objenchant = !thrown && no_obj || obj->spe < 0 ? 0 : obj->spe;

	if (obj && obj->otyp == BROKEN_SWORD && objenchant > 0) objenchant = 0;

	if (need_one(mon))    canhitmon = 1;
	if (need_two(mon))    canhitmon = 2;
	if (need_three(mon))  canhitmon = 3;
	if (need_four(mon))   canhitmon = 4;
	if (uarmf && uarmf->oartifact == ART_KILLCAP) canhitmon = 0;
	if (uwep && uwep->oartifact == ART_AP_) canhitmon = 0;

	/*
	 * If you are a creature that can hit as a +2 weapon, then YOU can
	 * hit as a +2 weapon. - SW
	 */

	if (Upolyd) {       /* Is Upolyd correct? */
	    /* a monster that needs a +1 weapon to hit it hits as a +1 weapon... */
	    if (need_one(&youmonst))		objenchant = 1;
	    if (need_two(&youmonst))		objenchant = 2;
	    if (need_three(&youmonst))		objenchant = 3;
	    if (need_four(&youmonst))		objenchant = 4;
	    /* overridden by specific flags */
	    if (hit_as_one(&youmonst))		objenchant = 1;
	    if (hit_as_two(&youmonst))		objenchant = 2;
	    if (hit_as_three(&youmonst))	objenchant = 3;
	    if (hit_as_four(&youmonst))		objenchant = 4;
	}

	unconventional[0] = '\0';
	saved_oname[0] = '\0';

	wakeup(mon);

	if(!thrown && no_obj) {      /* attack with bare hands */
	    if (Role_if(PM_MONK) && !Upolyd && GushLevel/4 > objenchant)
		objenchant = GushLevel/4;
	    /* if you have gloves and they have enough enchantment, you should be able to hit the monster --Amy */
	    noeffect = objenchant < canhitmon && !(uarmg && uarmg->spe >= canhitmon) && (issoviet || rn2(isfriday ? 5 : 3));

	    if (martial_bonus()) {
		if ((is_shade(mdat) || mon->egotype_shader) && !(uwep && uwep->oartifact == ART_AP_)) {
		    tmp = rn2(3);
		} else {
		    tmp = martial_dmg();
		}

		if (!(PlayerCannotUseSkills)) {
			switch (P_SKILL(P_GENERAL_COMBAT)) {
		
				case P_BASIC:		tmp +=  1; break;
				case P_SKILLED:	tmp +=  rnd(2); break;
				case P_EXPERT:	tmp +=  rnd(3); break;
				case P_MASTER:	tmp +=  rnd(4); break;
				case P_GRAND_MASTER:	tmp +=  rnd(5); break;
				case P_SUPREME_MASTER:	tmp +=  rnd(6); break;
				default: tmp += 0; break;
			}
		}

		if (!(PlayerCannotUseSkills) && uarm && (uarm->otyp >= ROBE && uarm->otyp <= ROBE_OF_WEAKNESS) ) {
			switch (P_SKILL(P_SORESU)) {
		
				case P_BASIC:		tmp +=  rn2(2); break;
				case P_SKILLED:	tmp +=  rnd(2); break;
				case P_EXPERT:	tmp +=  rnd(3); break;
				case P_MASTER:	tmp +=  rnd(4); break;
				case P_GRAND_MASTER:	tmp +=  rnd(5); break;
				case P_SUPREME_MASTER:	tmp +=  rnd(6); break;
				default: tmp += 0; break;
			}

		}

	    } else { /* bare-handed combat skill */
	    if ((is_shade(mdat) || mon->egotype_shader) && !(uwep && uwep->oartifact == ART_AP_))
		tmp = 0;
		else {
			tmp = rnd(2);
			int damagetodouble = 0;

			if (!(PlayerCannotUseSkills)) {

				switch (P_SKILL(P_BARE_HANDED_COMBAT)) {
					default:
					case P_ISRESTRICTED:
					case P_UNSKILLED:	tmp += 0; break;
					case P_BASIC:	tmp += 1; break;
					case P_SKILLED:	tmp += rnd(3); break;
					case P_EXPERT:	tmp += rnd(6); break;
					case P_MASTER:	tmp += rnd(10); break;
					case P_GRAND_MASTER:	tmp += rnd(15); break;
					case P_SUPREME_MASTER:	tmp += rnd(20); break;
				}
			}

			damagetodouble = tmp;

			/* base damage calculation is finished now, unarmed focus increases that amount --Amy */
			if (tech_inuse(T_UNARMED_FOCUS)) {
				tmp += (damagetodouble / 2);
			}
			if (uarmg && uarmg->otyp == GAUNTLETS_OF_FISTFIGHTING) {
				tmp += damagetodouble;
			}
			if (uarmg && uarmg->oartifact == ART_PU_PU) {
				tmp += damagetodouble;
			}

			if (uarmg && itemhasappearance(uarmg, APP_BOXING_GLOVES) ) tmp += 1;

			if (u.martialstyle == MARTIALSTYLE_KRAVMAGA) tmp += rn1(5, 5);

			if (uarmg && uarmg->oartifact == ART_BOX_FIST) tmp += 5;
			if (uarmg && uarmg->oartifact == ART_BOXING_LESSON) tmp += 5;
			if (uarmg && uarmg->oartifact == ART_FIFTY_SHADES_OF_FUCKED_UP) tmp += 5;
			if (uarm && uarm->oartifact == ART_GRANDMASTER_S_ROBE) tmp += 10;
			if (uarm && uarm->oartifact == ART_MONKSTERMAN) tmp += 5;

			if (u.martialstyle == MARTIALSTYLE_CAPOEIRA && u.capoeiracheck) {
				tmp += rnd((u.ulevel / 3) + 1);
			}

			if (uarmc && itemhasappearance(uarmc, APP_BOXING_GOWN)) tmp += 2;

			if (!(PlayerCannotUseSkills)) {
				switch (P_SKILL(P_GENERAL_COMBAT)) {
			
					case P_BASIC:		tmp +=  1; break;
					case P_SKILLED:	tmp +=  rnd(2); break;
					case P_EXPERT:	tmp +=  rnd(3); break;
					case P_MASTER:	tmp +=  rnd(4); break;
					case P_GRAND_MASTER:	tmp +=  rnd(5); break;
					case P_SUPREME_MASTER:	tmp +=  rnd(6); break;
					default: tmp += 0; break;
				}
			}

			if (Glib_combat && IsGlib) {
				tmp += rnd(GushLevel);
			}

			if (tech_inuse(T_JIU_JITSU)) {
				int jiuextradmg = 0;
				if (find_mac(mon) <= -5) jiuextradmg += 2;
				if (find_mac(mon) <= -10) jiuextradmg += 2;
				if (find_mac(mon) <= -15) jiuextradmg += 2;
				if (find_mac(mon) <= -20) jiuextradmg += 2;
				if (find_mac(mon) <= -25) jiuextradmg += 2;
				if (find_mac(mon) <= -30) jiuextradmg += 2;
				if (find_mac(mon) <= -35) jiuextradmg += 2;
				if (find_mac(mon) <= -40) jiuextradmg += 2;
				if (find_mac(mon) <= -45) jiuextradmg += 2;
				if (find_mac(mon) <= -50) jiuextradmg += 2;
				if (find_mac(mon) <= -55) jiuextradmg += 2;
				if (find_mac(mon) <= -60) jiuextradmg += 2;
				if (find_mac(mon) <= -65) jiuextradmg += 2;
				if (find_mac(mon) <= -70) jiuextradmg += 2;
				if (find_mac(mon) <= -75) jiuextradmg += 2;
				if (find_mac(mon) <= -80) jiuextradmg += 2;
				if (find_mac(mon) <= -85) jiuextradmg += 2;
				if (find_mac(mon) <= -90) jiuextradmg += 2;
				if (find_mac(mon) <= -95) jiuextradmg += 2;
				if (find_mac(mon) <= -100) jiuextradmg += 2;
				if (find_mac(mon) <= -105) jiuextradmg += 2;
				if (find_mac(mon) <= -110) jiuextradmg += 2;
				if (find_mac(mon) <= -115) jiuextradmg += 2;
				if (find_mac(mon) <= -120) jiuextradmg += 2;

				if (!(PlayerCannotUseSkills)) {
					if (P_SKILL(P_BARE_HANDED_COMBAT) >= P_SUPREME_MASTER) jiuextradmg *= 2;
					else if (P_SKILL(P_BARE_HANDED_COMBAT) >= P_GRAND_MASTER) {
						jiuextradmg *= 3;
						jiuextradmg /= 2;
					}
				}
				if (jiuextradmg) pline("You deal %d extra points of damage!", jiuextradmg);
				tmp += jiuextradmg;
			}
			if (tech_inuse(T_ESCROBISM) && uarm && (uarm->otyp >= ROBE && uarm->otyp <= ROBE_OF_WEAKNESS)) {
				if (uarm && uarm->spe > 0) tmp += (uarm->spe + rnd(5));
				else tmp += rnd(5);
			}

		}

	    } /* end bare-handed combat skill */

		/* multipliers should come first --Amy */

		if (u.martialstyle == MARTIALSTYLE_MUAYTHAI && tmp > 0) {
			tmp *= 13;
			tmp /= 10;
		}

		if (u.martialstyle == MARTIALSTYLE_KRAVMAGA && tmp > 0 && !rn2(5)) {
			tmp *= 2;
		}

		if (u.martialstyle == MARTIALSTYLE_KUNGFU && tmp > 0) {
			tmp *= 11;
			tmp /= 10;
		}

		if (Role_if(PM_XELNAGA)) tmp += rnd(4);
		if (Race_if(PM_KHAJIIT)) tmp += rnd(4);
		if (Race_if(PM_FENEK)) tmp += rnd(2);

		if (uarmg && uarmg->oartifact == ART_PRICKBUFF) {
			mon->bleedout += rnd(5);
		}
		if (uarmg && uarmg->oartifact == ART_GHAND) {
			if (haseyes(mon->data) && !resist(mon, WEAPON_CLASS, 0, NOTELL) && !mon->mblinded ) {
				mon->mblinded = rnd(10);
				You("slammed the dazzling light to %s!", mon_nam(mon));
			}
		}

		if (u.martialstyle == MARTIALSTYLE_KRAVMAGA && !rn2(20)) {
			if (!mon->mfrenzied) {
				mon->mtame = mon->mpeaceful = FALSE;
				mon->mfrenzied = TRUE;
				pline("%s is frenzied!", Monnam(mon));
			}
		}

		if (u.martialstyle == MARTIALSTYLE_SILAT && !rn2(10)) {
			pline_The("hard strike stuns %s!", mon_nam(mon));
			mon->mstun = TRUE;
		}

		if (uamul && uamul->oartifact == ART_BAKURETU_KEN && !rn2(5)) {
			int sx = u.ux, sy = u.uy, i;

			pline("Bakuretu-Ken!"); /* similar to "Kamehame-ha!", from Elona --Amy */

			for( i = 0; i < 2; i++) {		
			    if (!isok(sx,sy) || !cansee(sx,sy) || 
			    		IS_STWALL(levl[sx][sy].typ) || u.uswallow)
			    	break;

			    /* Display the center of the explosion */
			    tmp_at(DISP_FLASH, explosion_to_glyph(EXPL_FIERY, S_explode5));
			    tmp_at(sx, sy);
			    delay_output();
			    tmp_at(DISP_END, 0);

			    sx += u.dx;
			    sy += u.dy;
			}
			/* low explosion damage, since it triggers fairly often --Amy */
			explode(sx, sy, 10, d(3,4), WAND_CLASS, EXPL_FIERY);

			if (DEADMONSTER(mon)) return FALSE;

		}

		if (uleft && uleft->oartifact == ART_SPIKED_KNUCKLES) {
			mon->bleedout += rnd(5);
		}
		if (uright && uright->oartifact == ART_SPIKED_KNUCKLES) {
			mon->bleedout += rnd(5);
		}

		if (uleft && objects[(uleft)->otyp].oc_material == MT_MERCURIAL && !uarmg) {
			if (!rn2(10) && !resists_poison(mon)) {
				pline("%s is poisoned by your mercurial ring!", Monnam(mon));
				tmp += rnd(10);
				if (!rn2(10000)) {
					tmp += 9999;
					pline("The poison was deadly...");
				}
			}
		}
		if (uright && objects[(uright)->otyp].oc_material == MT_MERCURIAL && !uarmg) {
			if (!rn2(10) && !resists_poison(mon)) {
				pline("%s is poisoned by your mercurial ring!", Monnam(mon));
				tmp += rnd(10);
				if (!rn2(10000)) {
					tmp += 9999;
					pline("The poison was deadly...");
				}
			}
		}

		if (u.nailpolish && (!uarmg || FingerlessGloves) ) {
			tmp += (u.nailpolish * 2);
			if (rnd(10) <= u.nailpolish) {
				u.nailpolish--;
				pline(u.nailpolish ? "One of your nails loses its polish." : "Your nail loses its polish.");
			}
		}

		if (uarm && itemhasappearance(uarm, APP_ARENA_ROBE)) tmp++;
		if (Role_if(PM_GLADIATOR) && uarm && itemhasappearance(uarm, APP_ARENA_ROBE)) tmp++;
		if (uarm && uarm->oartifact == ART_MAEDHROS_SARALONDE) tmp += 2;
		if (uarmc && uarmc->oartifact == ART_DISBELIEVING_POWERLORD) tmp += rnd(5);
		if (uarmc && uarmc->oartifact == ART_MANTLE_OF_BEAST) tmp += 2;

		if (uleft && uleft->otyp == RIN_IMPACT) tmp += rnd(5);
		if (uright && uright->otyp == RIN_IMPACT) tmp += rnd(5);

		if (uarm && uarm->otyp == ROBE_OF_PUGILISM && !(PlayerCannotUseSkills)) {
			if (martial_bonus()) {
				switch (P_SKILL(P_MARTIAL_ARTS)) {
					case P_BASIC:		tmp +=  1; break;
					case P_SKILLED:	tmp +=  rnd(3); break;
					case P_EXPERT:	tmp +=  rnd(5); break;
					case P_MASTER:	tmp +=  rnd(6); break;
					case P_GRAND_MASTER:	tmp +=  rnd(8); break;
					case P_SUPREME_MASTER:	tmp +=  rnd(9); break;
					default: tmp += 0; break;
				}

			} else {
				switch (P_SKILL(P_BARE_HANDED_COMBAT)) {
			
					case P_BASIC:		tmp +=  1; break;
					case P_SKILLED:	tmp +=  rnd(2); break;
					case P_EXPERT:	tmp +=  rnd(3); break;
					case P_MASTER:	tmp +=  rnd(4); break;
					case P_GRAND_MASTER:	tmp +=  rnd(5); break;
					case P_SUPREME_MASTER:	tmp +=  rnd(6); break;
					default: tmp += 0; break;
				}

			}
		}

		if (uarm && uarm->oartifact == ART_JOHN_CENA_SPLIT_MAIL && !(PlayerCannotUseSkills)) {
			if (martial_bonus()) {
				switch (P_SKILL(P_MARTIAL_ARTS)) {
					case P_BASIC:		tmp +=  1; break;
					case P_SKILLED:	tmp +=  rnd(3); break;
					case P_EXPERT:	tmp +=  rnd(5); break;
					case P_MASTER:	tmp +=  rnd(6); break;
					case P_GRAND_MASTER:	tmp +=  rnd(8); break;
					case P_SUPREME_MASTER:	tmp +=  rnd(9); break;
					default: tmp += 0; break;
				}

			} else {
				switch (P_SKILL(P_BARE_HANDED_COMBAT)) {
			
					case P_BASIC:		tmp +=  1; break;
					case P_SKILLED:	tmp +=  rnd(2); break;
					case P_EXPERT:	tmp +=  rnd(3); break;
					case P_MASTER:	tmp +=  rnd(4); break;
					case P_GRAND_MASTER:	tmp +=  rnd(5); break;
					case P_SUPREME_MASTER:	tmp +=  rnd(6); break;
					default: tmp += 0; break;
				}

			}
		}

		if (uwep && uwep->oartifact == ART_BAEFF) tmp += 8;
		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_BAEFF) tmp += 8;

		if (uwep && uwep->oartifact == ART_CRUDE_SENSEI) {

			tmp += rnd(2);
			if (martial_bonus()) {
				switch (P_SKILL(P_MARTIAL_ARTS)) {
					case P_BASIC:		tmp +=  rnd(3); break;
					case P_SKILLED:	tmp +=  rnd(9); break;
					case P_EXPERT:	tmp +=  rnd(15); break;
					case P_MASTER:	tmp +=  rnd(18); break;
					case P_GRAND_MASTER:	tmp +=  rnd(24); break;
					case P_SUPREME_MASTER:	tmp +=  rnd(27); break;
					default: tmp += 0; break;
				}

			} else {
				switch (P_SKILL(P_BARE_HANDED_COMBAT)) {
			
					case P_BASIC:		tmp +=  rnd(2); break;
					case P_SKILLED:	tmp +=  rnd(6); break;
					case P_EXPERT:	tmp +=  rnd(9); break;
					case P_MASTER:	tmp +=  rnd(12); break;
					case P_GRAND_MASTER:	tmp +=  rnd(15); break;
					case P_SUPREME_MASTER:	tmp +=  rnd(18); break;
					default: tmp += 0; break;

				}
			}
		}

		if (uarm && uarm->oartifact == ART_EXTRAPUGELN && !(PlayerCannotUseSkills)) {
			if (martial_bonus()) {
				switch (P_SKILL(P_MARTIAL_ARTS)) {
					case P_BASIC:		tmp +=  1; break;
					case P_SKILLED:	tmp +=  rnd(3); break;
					case P_EXPERT:	tmp +=  rnd(5); break;
					case P_MASTER:	tmp +=  rnd(6); break;
					case P_GRAND_MASTER:	tmp +=  rnd(8); break;
					case P_SUPREME_MASTER:	tmp +=  rnd(9); break;
					default: tmp += 0; break;
				}

			} else {
				switch (P_SKILL(P_BARE_HANDED_COMBAT)) {
			
					case P_BASIC:		tmp +=  1; break;
					case P_SKILLED:	tmp +=  rnd(2); break;
					case P_EXPERT:	tmp +=  rnd(3); break;
					case P_MASTER:	tmp +=  rnd(4); break;
					case P_GRAND_MASTER:	tmp +=  rnd(5); break;
					case P_SUPREME_MASTER:	tmp +=  rnd(6); break;
					default: tmp += 0; break;
				}

			}
		}

		if (uarmf && uarmf->oartifact == ART_FINGERNAIL_FRONT && (!uarmg || FingerlessGloves) ) tmp += 3;

		if (u.martialstyle == MARTIALSTYLE_JUDO && tmp > 1) {
			tmp *= 4;
			tmp /= 5;
			if (tmp < 1) tmp = 1;
		}

	    valid_weapon_attack = (tmp > 0);

		/* stuff like prem heart or claws of the revenancer, implemented by stealing code from Chris (dnethack) --Amy */
		if (uarmg && uarmg->oartifact) {
			if (artifact_hit(&youmonst, mon, uarmg, &tmp, rnd(20))) {
				if(mon->mhp <= 0) /* artifact killed monster */
					return FALSE;
				if (tmp == 0) return TRUE;
				hittxt = TRUE;
			}
		}

	    /* blessed gloves give bonuses when fighting 'bare-handed' */
	    if (uarmg && uarmg->blessed && (is_undead(mdat) || is_demon(mdat))) {
		tmp += rnd(4);
	    }

		/* wtf vanilla devs! why didn't you account for the possibility of silver gloves??? --Amy */
	    if (uarmg && ((objects[uarmg->otyp].oc_material == MT_SILVER) || (uarmg->otyp == SILVER_GAUNTLETS)) ) {
		if (hates_silver(mdat)) {
			tmp += rnd(20);
			silvermsg = TRUE;
		}
	    }

		/* actually we should add other material-based damage bonuses here, too --Amy */

	    if (uarmg && uarmg->spe) tmp += uarmg->spe; /* WAC plusses from gloves */

	    /* So do silver rings.  Note: rings are worn under gloves, so you
	     * don't get both bonuses.
	     */
	    if (!uarmg) {
		if (uleft && objects[uleft->otyp].oc_material == MT_SILVER)
		    barehand_silver_rings++;
		if (uright && objects[uright->otyp].oc_material == MT_SILVER)
		    barehand_silver_rings++;
		if (barehand_silver_rings && hates_silver(mdat)) {
		    tmp += rnd(20);
		    silvermsg = TRUE;
		}
	    }

	    /* WAC - Hand-to-Hand Combat Techniques */


	    if ((tech_inuse(T_CHI_STRIKE))  && (u.uen > 0)) {
		You_feel("a surge of force.");
		tmp += (u.uen > (10 + (u.ulevel / 5)) ? 
			 (10 + (u.ulevel / 5)) : u.uen);
		u.uen -= (10 + (u.ulevel / 5));
		if (u.uen < 0) u.uen = 0;
	    }
	    
	    if (tech_inuse(T_E_FIST)) {
	    	int dmgbonus = 0;
		hittxt = TRUE;
		dmgbonus = noeffect ? 0 : d(2,4);
		switch (rn2(4)) {
		    case 0: /* Fire */
			if (!Blind) pline("%s is on fire!", Monnam(mon));
			if (!rn2(33)) dmgbonus += destroy_mitem(mon, SCROLL_CLASS, AD_FIRE);
			if (!rn2(33)) dmgbonus += destroy_mitem(mon, SPBOOK_CLASS, AD_FIRE);
			if (noeffect || (resists_fire(mon) && !player_will_pierce_resistance() ) ) {
			    if (!noeffect)
				shieldeff(mon->mx, mon->my);
			    if (!Blind) 
				pline_The("fire doesn't heat %s!", mon_nam(mon));
			    golemeffects(mon, AD_FIRE, dmgbonus);
			    if (!noeffect)
				dmgbonus = 0;
			    else
				noeffect = 0;
			}
			/* only potions damage resistant players in destroy_item */
			if (!rn2(33)) dmgbonus += destroy_mitem(mon, POTION_CLASS, AD_FIRE);
			break;
		    case 1: /* Cold */
		    	if (!Blind) pline("%s is covered in frost!", Monnam(mon));
			if (noeffect || (resists_cold(mon) && !player_will_pierce_resistance()) ) {
			    if (!noeffect)
				shieldeff(mon->mx, mon->my);
			    if (!Blind) 
				pline_The("frost doesn't chill %s!", mon_nam(mon));
			    golemeffects(mon, AD_COLD, dmgbonus);
			    dmgbonus = 0;
			    noeffect = 0;
			}
			if (!rn2(33)) dmgbonus += destroy_mitem(mon, POTION_CLASS, AD_COLD);
			break;
		    case 2: /* Elec */
			if (!Blind) pline("%s is zapped!", Monnam(mon));
			if (!rn2(33)) dmgbonus += destroy_mitem(mon, WAND_CLASS, AD_ELEC);
			if (noeffect || (resists_elec(mon) && !player_will_pierce_resistance()) ) {
			    if (!noeffect)
				shieldeff(mon->mx, mon->my);
			    if (!Blind)
				pline_The("zap doesn't shock %s!", mon_nam(mon));
			    golemeffects(mon, AD_ELEC, dmgbonus);
			    if (!noeffect)
				dmgbonus = 0;
			    else
				noeffect = 0;
			}
			/* only rings damage resistant players in destroy_item */
			if (!rn2(33)) dmgbonus += destroy_mitem(mon, RING_CLASS, AD_ELEC);
			break;
		    case 3: /* Acid */
			if (!Blind)
			    pline("%s is covered in acid!", Monnam(mon));
			if (noeffect || (resists_acid(mon) && !player_will_pierce_resistance()) ) {
			    if (!Blind)
				pline_The("acid doesn't burn %s!", Monnam(mon));
			    dmgbonus = 0;
			    noeffect = 0;
			}
			break;
		}
		if (dmgbonus > 0)
		    tmp += dmgbonus;
	    } /* Techinuse Elemental Fist */		

	} else { /* bare hands end, start weapons code */

	    if (obj->oartifact == ART_MAGICBANE) objenchant = 4;
	    else if (obj->oartifact) objenchant += 2;

	    if (is_lightsaber(obj)) objenchant = 4;

	    if (obj->opoisoned) ispoisoned = TRUE;

		if (Race_if(PM_POISONER)) ispoisoned = TRUE;
		if (obj->oartifact == ART_ASBESTOS_MATERIAL) ispoisoned = TRUE;
		if (obj->oartifact == ART_BLACK_POISON_INSIDE) ispoisoned = TRUE;
		if (obj->oartifact == ART_ALSO_MATTE_MASK) ispoisoned = TRUE;
		if (obj->otyp == ASBESTOS_JAVELIN) ispoisoned = TRUE;

	    noeffect = objenchant < canhitmon && !ispoisoned && (issoviet || rn2(isfriday ? 5 : 3) );
	    if (obj && obj->oartifact == ART_CLEAN_MAULER) noeffect = FALSE;

	    strcpy(saved_oname, cxname(obj));
	    if(obj->oclass == WEAPON_CLASS || is_weptool(obj) ||
	       obj->oclass == GEM_CLASS || obj->oclass == BALL_CLASS || obj->oclass == CHAIN_CLASS || obj->oclass == VENOM_CLASS) {

		/* is it not a melee weapon? */
		/* KMH, balance patch -- new macros */
		if (/* if you strike with a bow... */
		    (is_launcher(obj) && !(obj->otyp == LASERXBOW && obj->lamplit) && !(obj->otyp == KLIUSLING && obj->lamplit)) ||
		    /* or strike with a missile in your hand... */
		    (!thrown && (is_missile(obj) || is_ammo(obj))) ||
		    /* or use a pole at short range and not mounted... */
		    (!thrown && !u.usteed && !(tech_inuse(T_POLE_MELEE)) && is_pole(obj)) ||
		    /* lightsaber that isn't lit ;) */
		    (is_lightsaber(obj) && !obj->lamplit) ||
		    /* or throw a missile without the proper bow... */
		    (thrown == 1 && is_ammo(obj) && launcher && launcher->otyp == LASERXBOW && !launcher->lamplit) ||
		    (thrown == 1 && is_ammo(obj) && launcher && launcher->otyp == KLIUSLING && !launcher->lamplit) ||
		    (thrown == 1 && is_ammo(obj) && 
		    	!ammo_and_launcher(obj, launcher)) || 
		    /* This case isn't actually needed so far since 
		     * you can only throw in two-weapon mode when both
		     * launchers take the same ammo
		     */
		    (thrown == 2 && is_ammo(obj) && launcher && launcher->otyp == LASERXBOW && !launcher->lamplit) ||
		    (thrown == 2 && is_ammo(obj) && launcher && launcher->otyp == KLIUSLING && !launcher->lamplit) ||
		    (thrown == 2 && is_ammo(obj) && 
		    	!ammo_and_launcher(obj, launcher))) {
		    /* then do only 1-2 points of damage */
		    if ((is_shade(mdat) || mon->egotype_shader) && !(uwep && uwep->oartifact == ART_AP_) && objects[obj->otyp].oc_material != MT_SILVER && objects[obj->otyp].oc_material != MT_ARCANIUM)
			tmp = 0;
		    else
			tmp = rnd(2);

			if (!thrown && obj && obj->oartifact == ART_ZACKSCHRACK) {
				tmp += 5;
			}

		/* Bashing with bows, darts, ranseurs or inactive lightsabers might not be completely useless... --Amy */

		    if (( (is_launcher(obj) && obj->otyp != WEAPON_SIGN && !(obj->otyp == LASERXBOW && obj->lamplit) && !(obj->otyp == KLIUSLING && obj->lamplit)) || is_missile(obj) || is_ammo(obj) || (is_pole(obj) && !(tech_inuse(T_POLE_MELEE)) && !u.usteed) || (is_lightsaber(obj) && !obj->lamplit) ) && !thrown) {


			if (!(PlayerCannotUseSkills) && !rn2(2)) {

				int basherskill = objects[obj->otyp].oc_skill;
				if (basherskill < 0) basherskill = -(basherskill);

				switch (P_SKILL(basherskill)) {

					case P_SKILLED: tmp += 1; break;
					case P_EXPERT: tmp += rno(2); break;
					case P_MASTER: tmp += rno(3); break;
					case P_GRAND_MASTER: tmp += rno(4); break;
					case P_SUPREME_MASTER: tmp += rno(5); break;
					default: break;

				}

			}

			if (Role_if(PM_SHADOW_JEDI) && is_lightsaber(obj) &&
			   ((wtype = uwep_skill_type()) != P_NONE) &&
			   ((monwep = MON_WEP(mon)) != 0 && monwep && !is_lightsaber(monwep) &&
			   !monwep->oartifact && !stack_too_big(monwep) &&
			   !obj_resists(monwep, 50 + 15 * greatest_erosionX(obj), 100))) {

				int juyochance = 1;
				if (!PlayerCannotUseSkills) {
					switch (P_SKILL(P_JUYO)) {

						case P_BASIC:	juyochance +=  1; break;
						case P_SKILLED:	juyochance +=  2; break;
						case P_EXPERT:	juyochance +=  3; break;
						case P_MASTER:	juyochance +=  4; break;
						case P_GRAND_MASTER:	juyochance +=  5; break;
						case P_SUPREME_MASTER:	juyochance +=  6; break;
						default: juyochance += 0; break;
					}

				}

				if (juyochance > rn2(100)) {

					pline("Your lightsaber is capable of cutting %s %s!", s_suffix(Monnam(mon)), xname(monwep));
					getlin ("Cut the opponent's weapon in half? [y/yes/no]",cutbuf);
					(void) lcase (cutbuf);
					if (!(strcmp (cutbuf, "yes")) || !(strcmp (cutbuf, "y")) || !(strcmp (cutbuf, "ye")) || !(strcmp (cutbuf, "ys"))) {

						setmnotwielded(mon,monwep);
						MON_NOWEP(mon);
						mon->weapon_check = NEED_WEAPON;
						Your("%s cuts %s %s in half!", xname(obj), s_suffix(mon_nam(mon)), xname(monwep));
						m_useup(mon, monwep);
						u.cnd_weaponbreakcount++;
						mightbooststat(A_CON);
						/* If someone just shattered MY weapon, I'd flee! */
						if (rn2(4)) {
						    monflee(mon, d(2,3), TRUE, TRUE);
						}

						hittxt = TRUE;
						use_skill(P_JUYO, rnd(7)); /* has to be faster because hard to train otherwise --Amy */

					}
				}

			}

			if (obj && obj->spe > 0 && obj->otyp != BROKEN_SWORD) tmp += obj->spe;

			if (obj && obj->oartifact == ART_KLOBB && !rn2(10) && mon->mcanmove) {
				mon->mfrozen = rnd(10);
				mon->mcanmove = 0;
				mon->mstrategy &= ~STRAT_WAITFORU;
				You("klobbed %s, who cannot move for the time being.", mon_nam(mon));
			}

			/* reminder: this is only for ineffective weapons, not stuff like shields, whistles etc. !!! --Amy
			 * artifacts that deal their bonus damage even when bashing have to go here */

			if (obj->oartifact) {

				boolean willartibash = FALSE;

				switch (obj->oartifact) {

					case ART_JONADAB_S_GREAT_BASH:
					case ART_EVERCONSUMING_HELLFIRE:
					case ART_FIRE_L_E_A_D_E_R:
					case ART_VITRIOL_FROM_THE_LAB:
					case ART_CHATGPT_S_FREEZE:
					case ART_CHOCKERSHOCKER:
					case ART_BIBLICAL_PLAGUE:
					case ART_DARTH_S_VOREMUZZLE:
					case ART_LORD_SIDIOUS__SECRET_WEAPO:
					case ART_RISE_OF_VENTRESS:
						willartibash = TRUE;
						break;

					default: break;
				}

				if (willartibash && artifact_hit(&youmonst, mon, obj, &tmp, dieroll)) {
					if(mon->mhp <= 0) /* artifact killed monster */
						return FALSE;
					if (tmp == 0) return TRUE;
					hittxt = TRUE;
				}
			}

			if (obj && obj->oartifact == ART_PING_EM_AWAY && tmp > 0) {
				tmp += 12;
			}
			if (obj && obj->oartifact == ART_OZYZEVPDWTVP && tmp > 0) {
				tmp += rn1(7, 4);
			}
			if (obj && obj->oartifact == ART_BASHCRASH && tmp > 0) {
				tmp *= 2;
			}
			if (obj && obj->oartifact == ART_STRIKE_ONE_RUN_THER && tmp > 0) {
				tmp += 5;
				if (!PlayerCannotUseSkills) {
					switch (P_SKILL(P_SLING)) {

						case P_BASIC:	tmp += 1; break;
						case P_SKILLED:	tmp += 2; break;
						case P_EXPERT:	tmp += 3; break;
						case P_MASTER:	tmp += 4; break;
						case P_GRAND_MASTER:	tmp += 5; break;
						case P_SUPREME_MASTER:	tmp += 6; break;
						default: break;
					}

				}
			}
			if (obj && obj->oartifact == ART_TIME_FOR_A_CALL) tmp += 6;

			if (obj && obj->oartifact == ART_GAMMASABER) tmp += rnd(10);
			if (obj && obj->oartifact == ART_TEH_HUNK && !obj->lamplit && tmp > 0) tmp += 5;
			if (obj && obj->oartifact == ART_GAYGUN && (u.homosexual == 1)) tmp += 5;

			if (!SkillTrainingImpossible) {

				if (is_lightsaber(obj)) {
					u.hunkturns++;
					if (u.hunkturns >= 5) {
						u.hunkturns = 0;
						u.hunkskill++;
						if ((obj && obj->oartifact == ART_TEH_HUNK) || (uwep && uwep->oartifact == ART_DE_SID) || Role_if(PM_HEDDERJEDI) || (uarm && uarm->oartifact == ART_HUNKSTERMAN)) {
							if (u.hunkskill == 20) You("are now more skilled in form IX (Hunk).");
							if (u.hunkskill == 160) You("are now more skilled in form IX (Hunk).");
							if (u.hunkskill == 540) You("are now more skilled in form IX (Hunk).");
							if (u.hunkskill == 1280) You("are now more skilled in form IX (Hunk).");
							if (u.hunkskill == 2560) You("are now more skilled in form IX (Hunk).");
							if (u.hunkskill == 4320) You("are now most skilled in form IX (Hunk).");
						}
					}

				}
			}

			if (!PlayerCannotUseSkills && is_lightsaber(obj)) {
				if ((uarm && uarm->oartifact == ART_HUNKSTERMAN) || (uwep && uwep->oartifact == ART_DE_SID) || (uwep && uwep->otyp == LIGHTTORCH) || Role_if(PM_HEDDERJEDI)) {
					if (u.hunkskill >= 20) tmp++;
					if (u.hunkskill >= 160) tmp++;
					if (u.hunkskill >= 540) tmp++;
					if (u.hunkskill >= 1280) tmp++;
					if (u.hunkskill >= 2500) tmp++;
					if (u.hunkskill >= 4320) tmp++;
				}
			}

			valid_weapon_attack = (tmp > 0);
			if (flags.bash_reminder && !rn2(20)) {
				switch (rnd(5)) {

					case 1: pline("A helpful reminder: your weapon could be used more effectively."); break;
					case 2: pline("A helpful reminder: bashing with that weapon is not the most effective way of using it."); break;
					case 3: pline("A helpful reminder: there is a different way of using your current weapon..."); break;
					case 4: pline("A helpful reminder: did you try applying your weapon or firing projectiles from it yet?"); break;
					case 5: pline("A helpful reminder: your current weapon is a lightsaber that is not lit, a polearm that's meant to be applied, or a ranged weapon! In any case it's not very effective to bash monsters with it!"); break;
				}
			}

		}

		/* not gonna do that stupidity (sorry) where everything unconditionally misses 25%. --Amy 
		 * All that we want is to periodically remind the player that they aren't using their weapon correctly. */

			/* Slings were more powerful in melee than war hammers, which clearly wasn't intended. --Amy */

			if (rn2(3)) {
				if (tmp == 3) tmp = 2;
				else if (tmp == 4) tmp = 3;
				else if (tmp == 5) tmp = 3;
				else if (tmp >= 6) tmp /= 2;
			}

			/* bashing demons with silver arrows did NOTHING??? what a bug... --Amy */

			if (objects[obj->otyp].oc_material == MT_SILVER && hates_silver(mdat)) {
				tmp += rnd(20);
				silvermsg = TRUE; silverobj = TRUE;
			}
			if (objects[obj->otyp].oc_material == MT_VIVA && hates_viva(mdat)) {
				tmp += 20;
				vivaobj = TRUE;
			}
			if (objects[obj->otyp].oc_material == MT_COPPER && hates_copper(mdat)) {
				tmp += 20;
				copperobj = TRUE;
			}
			if (objects[obj->otyp].oc_material == MT_MERCURIAL && !rn2(10) && !(resists_poison(mon) )) {
				tmp += rnd(4);
				mercurialobj = TRUE;
			}
			if (objects[obj->otyp].oc_material == MT_PLATINUM && hates_platinum(mdat)) {
				tmp += 20;
				platinumobj = TRUE;
			}
			if (obj->cursed && hates_cursed(mdat)) {
				tmp += 4;
				if (obj->hvycurse) tmp += 4;
				if (obj->prmcurse) tmp += 7;
				if (obj->bbrcurse) tmp += 15;
				if (obj->evilcurse) tmp += 15;
				if (obj->morgcurse) tmp += 15;
				cursedobj = 1;
				if (obj->hvycurse) cursedobj++;
				if (obj->prmcurse) cursedobj++;
				if (obj->bbrcurse) cursedobj++;
				if (obj->evilcurse) cursedobj++;
				if (obj->morgcurse) cursedobj++;
			}
			if (objects[obj->otyp].oc_material == MT_INKA && hates_inka(mdat)) {
				tmp += 5;
				inkaobj = TRUE;
			}
			if (obj->otyp == ODOR_SHOT && hates_odor(mdat)) {
				tmp += rnd(10);
				odorobj = TRUE;
			}

		    if (!thrown && (obj == uwep || obj == uswapwep) && 
				(obj->otyp == BOOMERANG || obj->otyp == ALU_BOOMERANG || obj->otyp == SILVER_CHAKRAM || obj->otyp == BATARANG || obj->otyp == DARK_BATARANG) && (rnl(4) != 3) ) {
			boolean more_than_1 = (obj->quan > 1L);

			pline("As you hit %s, %s%s %s breaks into splinters.",
			      mon_nam(mon), more_than_1 ? "one of " : "",
			      shk_your(yourbuf, obj), xname(obj));
			if (!more_than_1) {
			    if (obj == uwep)
				uwepgone();   /* set unweapon */
			    else
				setuswapwep((struct obj *)0, FALSE);
			}
			useup(obj);
			if (!more_than_1) obj = (struct obj *) 0;
			hittxt = TRUE;
			if (!is_shade(mdat) && !(mon->egotype_shader))
			    tmp++;
		   }

		} else {
		    tmp = dmgvalX(obj, mon);
		    /* a minimal hit doesn't exercise proficiency */
		    valid_weapon_attack = (tmp > 0);
#if 0
		    if (!valid_weapon_attack || mon == u.ustuck || u.twoweap) {
#endif

		    if (!thrown && tech_inuse(T_SPECTRAL_SWORD)) {
		    	int dmgbonus = 0;
			hittxt = TRUE;
			dmgbonus = noeffect ? 0 : d(6,4);
			switch (rn2(4)) {
			    case 0: /* Fire */
				if (!Blind) pline("%s is on fire!", Monnam(mon));
				if (!rn2(33)) dmgbonus += destroy_mitem(mon, SCROLL_CLASS, AD_FIRE);
				if (!rn2(33)) dmgbonus += destroy_mitem(mon, SPBOOK_CLASS, AD_FIRE);
				if (noeffect || (resists_fire(mon) && !player_will_pierce_resistance() ) ) {
				    if (!noeffect)
					shieldeff(mon->mx, mon->my);
				    if (!Blind) 
					pline_The("fire doesn't heat %s!", mon_nam(mon));
				    golemeffects(mon, AD_FIRE, dmgbonus);
				    if (!noeffect)
					dmgbonus = 0;
				    else
					noeffect = 0;
				}
				/* only potions damage resistant players in destroy_item */
				if (!rn2(33)) dmgbonus += destroy_mitem(mon, POTION_CLASS, AD_FIRE);
				break;
			    case 1: /* Cold */
			    	if (!Blind) pline("%s is covered in frost!", Monnam(mon));
				if (noeffect || (resists_cold(mon) && !player_will_pierce_resistance()) ) {
				    if (!noeffect)
					shieldeff(mon->mx, mon->my);
				    if (!Blind) 
					pline_The("frost doesn't chill %s!", mon_nam(mon));
				    golemeffects(mon, AD_COLD, dmgbonus);
				    dmgbonus = 0;
				    noeffect = 0;
				}
				if (!rn2(33)) dmgbonus += destroy_mitem(mon, POTION_CLASS, AD_COLD);
				break;
			    case 2: /* Elec */
				if (!Blind) pline("%s is zapped!", Monnam(mon));
				if (!rn2(33)) dmgbonus += destroy_mitem(mon, WAND_CLASS, AD_ELEC);
				if (noeffect || (resists_elec(mon) && !player_will_pierce_resistance()) ) {
				    if (!noeffect)
					shieldeff(mon->mx, mon->my);
				    if (!Blind)
					pline_The("zap doesn't shock %s!", mon_nam(mon));
				    golemeffects(mon, AD_ELEC, dmgbonus);
				    if (!noeffect)
					dmgbonus = 0;
				    else
					noeffect = 0;
				}
				/* only rings damage resistant players in destroy_item */
				if (!rn2(33)) dmgbonus += destroy_mitem(mon, RING_CLASS, AD_ELEC);
				break;
			    case 3: /* Acid */
				if (!Blind)
				    pline("%s is covered in acid!", Monnam(mon));
				if (noeffect || (resists_acid(mon) && !player_will_pierce_resistance()) ) {
				    if (!Blind)
					pline_The("acid doesn't burn %s!", Monnam(mon));
				    dmgbonus = 0;
				    noeffect = 0;
				}
				break;
			}
			if (dmgbonus > 0)
			    tmp += dmgbonus;
		    } /* Techinuse Spectral Sword */

			int juyohelpchance = 0;

			if (!PlayerCannotUseSkills) {
				switch (P_SKILL(P_JUYO)) {

					case P_BASIC:	juyohelpchance =  1; break;
					case P_SKILLED:	juyohelpchance =  2; break;
					case P_EXPERT:	juyohelpchance =  3; break;
					case P_MASTER:	juyohelpchance =  4; break;
					case P_GRAND_MASTER:	juyohelpchance =  5; break;
					case P_SUPREME_MASTER:	juyohelpchance =  6; break;
					default: juyohelpchance = 0; break;
				}

			}

		    if (!valid_weapon_attack || mon == u.ustuck) {
			;	/* no special bonuses */
		    } else if (mon->mflee && (Role_if(PM_ROGUE) || (uwep && uwep->oartifact == ART_SUPERSTAB) || (uarmc && uarmc->oartifact == ART_BEHIND_CUNTINGNESS) || (uarmc && uarmc->oartifact == ART_STRIPED_SHIRT_OF_THE_MURDE) || (uarmf && uarmf->oartifact == ART_BACKGROUND_HOLDING) || Race_if(PM_VIETIS) || Role_if(PM_MURDERER) || Role_if(PM_DISSIDENT) || Role_if(PM_ASSASSIN) ) && !Upolyd) {
			if (!issoviet) You("strike %s from behind!", mon_nam(mon));
			else pline("K schast'yu, vy ne chuvstvuyete sebya vo vsem, chto vasha spina koloto odolevayet!");
			tmp += issoviet ? GushLevel : rno(GushLevel); /* nerf by Amy */
			hittxt = TRUE;
		    } else if ((dieroll == 2 || (juyohelpchance >= rnd(100))) && obj == uwep &&
			  !u.twoweap && obj->oclass == WEAPON_CLASS && (bimanual(obj) || obj->oartifact == ART_OUTJUYOING ||
			    (Role_if(PM_SAMURAI) && obj->otyp == KATANA && !uarms)) &&
			  (((wtype = uwep_skill_type()) != P_NONE && !(PlayerCannotUseSkills) && P_SKILL(wtype) >= P_SKILLED) || (obj->oartifact == ART_OUTJUYOING) ) && 
			  ((monwep = MON_WEP(mon)) != 0 && monwep &&
			   !is_flimsy(monwep) && !stack_too_big(monwep) &&
			   !obj_resists(monwep, 50 + 15 * greatest_erosionX(obj), 100))) {
			/*
			 * 2.5% chance of shattering defender's weapon when
			 * using a two-handed weapon; less if uwep is rusted.
			 * [dieroll == 2 is most successful non-beheading or
			 * -bisecting hit, in case of special artifact damage;
			 * the percentage chance is (1/20)*(50/100).]
			 * WAC.	Bimanual, or samurai and Katana without shield.
			 *	No twoweapon.
			 */

			pline("Your blow is powerful enough to destroy %s %s!", s_suffix(Monnam(mon)), xname(monwep));
			getlin ("Smash the opponent's weapon? [y/yes/no]",cutbuf);
			(void) lcase (cutbuf);
			if (!(strcmp (cutbuf, "yes")) || !(strcmp (cutbuf, "y")) || !(strcmp (cutbuf, "ye")) || !(strcmp (cutbuf, "ys"))) {

				setmnotwielded(mon,monwep);
				MON_NOWEP(mon);
				mon->weapon_check = NEED_WEAPON;
				pline("%s %s %s from the force of your blow!",
				      s_suffix(Monnam(mon)), xname(monwep),
				      otense(monwep, "shatter"));
				m_useup(mon, monwep);
				u.cnd_weaponbreakcount++;
				mightbooststat(A_CON);
				/* If someone just shattered MY weapon, I'd flee! */
				if (rn2(4)) {
				    monflee(mon, d(2,3), TRUE, TRUE);
				}
				if (obj && obj->oartifact == ART_OUTJUYOING) use_skill(P_JUYO, rnd(7));

				hittxt = TRUE;
			}

		    }

		    else if (obj == uwep && is_lightsaber(obj) &&
			   ((wtype = uwep_skill_type()) != P_NONE) &&
			   ((monwep = MON_WEP(mon)) != 0 && monwep && !is_lightsaber(monwep) && // no cutting other lightsabers :)
			   !monwep->oartifact && !stack_too_big(monwep) && // no cutting artifacts either
			   !obj_resists(monwep, 50 + 15 * greatest_erosionX(obj), 100))) {

				int juyochance = 5;

				if (!PlayerCannotUseSkills) {
					switch (P_SKILL(P_JUYO)) {

						case P_BASIC:	juyochance +=  10; break;
						case P_SKILLED:	juyochance +=  20; break;
						case P_EXPERT:	juyochance +=  30; break;
						case P_MASTER:	juyochance +=  40; break;
						case P_GRAND_MASTER:	juyochance +=  50; break;
						case P_SUPREME_MASTER:	juyochance +=  60; break;
						default: juyochance += 0; break;
					}

					/* juyo and vaapad so are both "form VII" so they boost each other --Amy
					 * increased chance to cut enemy weapons if you've enhanced both */
					if (P_SKILL(P_JUYO) >= P_BASIC && P_SKILL(P_VAAPAD) >= P_BASIC) juyochance++;
					if (P_SKILL(P_JUYO) >= P_SKILLED && P_SKILL(P_VAAPAD) >= P_SKILLED) juyochance++;
					if (P_SKILL(P_JUYO) >= P_EXPERT && P_SKILL(P_VAAPAD) >= P_EXPERT) juyochance++;
					if (P_SKILL(P_JUYO) >= P_MASTER && P_SKILL(P_VAAPAD) >= P_MASTER) juyochance++;
					if (P_SKILL(P_JUYO) >= P_GRAND_MASTER && P_SKILL(P_VAAPAD) >= P_GRAND_MASTER) juyochance++;
					if (P_SKILL(P_JUYO) >= P_SUPREME_MASTER && P_SKILL(P_VAAPAD) >= P_SUPREME_MASTER) juyochance++;

				}
				if (P_SKILL(weapon_type(uwep)) >= P_SKILLED && !(PlayerCannotUseSkills) ) {
					juyochance += 30;
					if (Role_if(PM_JEDI)) {
						juyochance += ((100 - juyochance) / 2);
					}
					if (Role_if(PM_SHADOW_JEDI)) {
						juyochance += ((100 - juyochance) / 2);
					}
					if (Role_if(PM_HEDDERJEDI)) {
						juyochance += ((100 - juyochance) / 2);
					}
					if (Race_if(PM_BORG)) {
						juyochance += ((100 - juyochance) / 5);
					}
				}

				if (juyochance > rn2(100)) {

					pline("Your lightsaber is capable of cutting %s %s!", s_suffix(Monnam(mon)), xname(monwep));
					getlin ("Cut the opponent's weapon in half? [y/yes/no]",cutbuf);
					(void) lcase (cutbuf);
					if (!(strcmp (cutbuf, "yes")) || !(strcmp (cutbuf, "y")) || !(strcmp (cutbuf, "ye")) || !(strcmp (cutbuf, "ys"))) {

						setmnotwielded(mon,monwep);
						MON_NOWEP(mon);
						mon->weapon_check = NEED_WEAPON;
						Your("%s cuts %s %s in half!", xname(obj), s_suffix(mon_nam(mon)), xname(monwep));
						m_useup(mon, monwep);
						u.cnd_weaponbreakcount++;
						mightbooststat(A_CON);
						/* If someone just shattered MY weapon, I'd flee! */
						if (rn2(4)) {
						    monflee(mon, d(2,3), TRUE, TRUE);
						}

						if (tech_inuse(T_DECAPABILITY)) {

							if (!mon->msleeping && sleep_monst(mon, rnd(10), -1)) {
								if (!Blind) pline("%s is put to sleep by you!", Monnam(mon));
								slept_monst(mon);
							}

						}

						hittxt = TRUE;
						use_skill(P_JUYO, rnd(7)); /* has to be faster because hard to train otherwise --Amy */

						if (tech_inuse(T_SURRENDER_OR_DIE)) {
							if (!mon->mfrenzied && !resist(mon, WEAPON_CLASS, 0, NOTELL) ) {
								mon->mpeaceful = TRUE;
								pline("%s surrenders!", Monnam(mon));
								return FALSE;
							} else if (obj && obj->spe < 0) {
								obj->spe = 0;
								pline("Your lightsaber is no longer negatively enchanted!");
							} else if (obj && obj->spe < 2) {
								obj->spe++;
								pline("Your lightsaber gains a point of enchantment!");
							} else if (obj && obj->spe < 7 && !rn2(obj->spe) && !rn2(obj->spe)) {
								obj->spe++;
								pline("Your lightsaber vibrates and is highly enchanted now!");
							} else {
								obj->age += rn1(100, 100);
								if (obj->otyp == ORANGE_LIGHTSABER) obj->age += (rn1(100, 100) * rnd(2));
								if (obj->oartifact == ART_DESANN_S_WRATH) obj->age += (rn1(100, 100) * rnd(2));
								pline("Your lightsaber gains additional energy!");
							}
						}

					}

				}
			}

			if (obj == uwep && is_lightsaber(obj) && obj->lamplit && tech_inuse(T_PERILOUS_WHIRL) && !rn2(10) && !resists_drli(mon) && !resist(mon, WEAPON_CLASS, 0, NOTELL) ) {
				pline("%s suddenly seems weaker!", mon_nam(mon));
				mon->mhpmax -= rnd(8);
				if (mon->mhp > mon->mhpmax) mon->mhp = mon->mhpmax;
				if (mon->mhp <= 0 || !mon->m_lev) {
					pline("%s dies!", Monnam(mon));
					xkilled(mon,0);
					return FALSE;
				} else {
					mon->m_lev--;
				}

			}

			if (obj && objects[obj->otyp].oc_skill == P_CLAW) {
				if (mon->mflee || !(mon->mcanmove)) tmp += rnd(10);
				if (obj->otyp == AMBUSH_QATAR) tmp += rnd(10);
			}

			if (obj && (obj->otyp == BRASS_KNUCKLES || obj->otyp == SUPER_KNUCKLES || obj->otyp == ULTRA_KNUCKLES || obj->otyp == MASTER_KNUCKLES || obj->otyp == ELITE_KNUCKLES || obj->otyp == POWERFIST || obj->otyp == SATURNITE_FIST || obj->otyp == LASERFIST) && tech_inuse(T_JIU_JITSU)) {
				int jiuextradmg = 0;
				if (find_mac(mon) <= -5) jiuextradmg += 2;
				if (find_mac(mon) <= -10) jiuextradmg += 2;
				if (find_mac(mon) <= -15) jiuextradmg += 2;
				if (find_mac(mon) <= -20) jiuextradmg += 2;
				if (find_mac(mon) <= -25) jiuextradmg += 2;
				if (find_mac(mon) <= -30) jiuextradmg += 2;
				if (find_mac(mon) <= -35) jiuextradmg += 2;
				if (find_mac(mon) <= -40) jiuextradmg += 2;
				if (find_mac(mon) <= -45) jiuextradmg += 2;
				if (find_mac(mon) <= -50) jiuextradmg += 2;
				if (find_mac(mon) <= -55) jiuextradmg += 2;
				if (find_mac(mon) <= -60) jiuextradmg += 2;
				if (find_mac(mon) <= -65) jiuextradmg += 2;
				if (find_mac(mon) <= -70) jiuextradmg += 2;
				if (find_mac(mon) <= -75) jiuextradmg += 2;
				if (find_mac(mon) <= -80) jiuextradmg += 2;
				if (find_mac(mon) <= -85) jiuextradmg += 2;
				if (find_mac(mon) <= -90) jiuextradmg += 2;
				if (find_mac(mon) <= -95) jiuextradmg += 2;
				if (find_mac(mon) <= -100) jiuextradmg += 2;
				if (find_mac(mon) <= -105) jiuextradmg += 2;
				if (find_mac(mon) <= -110) jiuextradmg += 2;
				if (find_mac(mon) <= -115) jiuextradmg += 2;
				if (find_mac(mon) <= -120) jiuextradmg += 2;

				if (!(PlayerCannotUseSkills)) {
					if (P_SKILL(P_BARE_HANDED_COMBAT) >= P_SUPREME_MASTER) jiuextradmg *= 2;
					else if (P_SKILL(P_BARE_HANDED_COMBAT) >= P_GRAND_MASTER) {
						jiuextradmg *= 3;
						jiuextradmg /= 2;
					}
				}
				if (jiuextradmg) pline("You deal %d extra points of damage!", jiuextradmg);
				tmp += jiuextradmg;
			}

			/* empath can feel the monster's psyche sometimes --Amy */
			if (Role_if(PM_EMPATH) && !rn2(20)) {
				You("probe %s!", mon_nam(mon));
				probe_monster(mon);
			}
			/* and calm down angry ones */
			if (Role_if(PM_EMPATH) && mon->mfrenzied && !rn2(10)) {
				mon->mfrenzied = FALSE;
				pline("%s is shaken out of %s frenzy by your attack!", Monnam(mon), mhis(mon));
			}

			/* tin opener special-casing */
			if (obj && obj->oartifact == ART_TIN_FU) tmp += 20;
			if (obj->oartifact == ART_SUPERMARKET_FU) {
				tmp += rnd(10);
				if (Role_if(PM_SUPERMARKET_CASHIER)) tmp += rnd(10);
			}

			if (obj && (obj->otyp == LASER_TIN_OPENER || obj->otyp == TIN_OPENER || obj->otyp == BUDO_NO_SASU || obj->otyp == JEONTU_GEOM) && Role_if(PM_SUPERMARKET_CASHIER)) {
				if (obj->otyp == BUDO_NO_SASU) tmp += 5;
				tmp += 2;
				if (GushLevel >= 18) tmp += rnd(10);
				if (GushLevel >= 24) tmp += rnd(4);
				if (GushLevel >= 27) tmp += rnd(2);
				if (GushLevel >= 30) tmp += 1;

				if (!(PlayerCannotUseSkills)) {
					switch (P_SKILL(P_MARTIAL_ARTS)) {
						case P_BASIC: tmp += 2; break;
						case P_SKILLED: tmp += 4; break;
						case P_EXPERT: tmp += 6; break;
						case P_MASTER: tmp += 8; break;
						case P_GRAND_MASTER: tmp += 10; break;
						case P_SUPREME_MASTER: tmp += 12; break;
					}
				}

			    if ((tech_inuse(T_CHI_STRIKE))  && (u.uen > 0)) {
				You_feel("a surge of force.");
				tmp += (u.uen > (10 + (u.ulevel / 5)) ? 
					 (10 + (u.ulevel / 5)) : u.uen);
				u.uen -= (10 + (u.ulevel / 5));
				if (u.uen < 0) u.uen = 0;
			    }

			    if (tech_inuse(T_E_FIST)) {
			    	int dmgbonus = 0;
				hittxt = TRUE;
				dmgbonus = d(2,4);
				switch (rn2(4)) {
				    case 0: /* Fire */
					if (!Blind) pline("%s is on fire!", Monnam(mon));
					if (!rn2(33)) dmgbonus += destroy_mitem(mon, SCROLL_CLASS, AD_FIRE);
					if (!rn2(33)) dmgbonus += destroy_mitem(mon, SPBOOK_CLASS, AD_FIRE);
					if (noeffect || (resists_fire(mon) && !player_will_pierce_resistance() ) ) {
					    if (!noeffect)
						shieldeff(mon->mx, mon->my);
					    if (!Blind) 
						pline_The("fire doesn't heat %s!", mon_nam(mon));
					    golemeffects(mon, AD_FIRE, dmgbonus);
					    if (!noeffect)
						dmgbonus = 0;
					    else
						noeffect = 0;
					}
					/* only potions damage resistant players in destroy_item */
					if (!rn2(33)) dmgbonus += destroy_mitem(mon, POTION_CLASS, AD_FIRE);
					break;
				    case 1: /* Cold */
				    	if (!Blind) pline("%s is covered in frost!", Monnam(mon));
					if (noeffect || (resists_cold(mon) && !player_will_pierce_resistance()) ) {
					    if (!noeffect)
						shieldeff(mon->mx, mon->my);
					    if (!Blind) 
						pline_The("frost doesn't chill %s!", mon_nam(mon));
					    golemeffects(mon, AD_COLD, dmgbonus);
					    dmgbonus = 0;
					    noeffect = 0;
					}
					if (!rn2(33)) dmgbonus += destroy_mitem(mon, POTION_CLASS, AD_COLD);
					break;
				    case 2: /* Elec */
					if (!Blind) pline("%s is zapped!", Monnam(mon));
					if (!rn2(33)) dmgbonus += destroy_mitem(mon, WAND_CLASS, AD_ELEC);
					if (noeffect || (resists_elec(mon) && !player_will_pierce_resistance()) ) {
					    if (!noeffect)
						shieldeff(mon->mx, mon->my);
					    if (!Blind)
						pline_The("zap doesn't shock %s!", mon_nam(mon));
					    golemeffects(mon, AD_ELEC, dmgbonus);
					    if (!noeffect)
						dmgbonus = 0;
					    else
						noeffect = 0;
					}
					/* only rings damage resistant players in destroy_item */
					if (!rn2(33)) dmgbonus += destroy_mitem(mon, RING_CLASS, AD_ELEC);
					break;
				    case 3: /* Acid */
					if (!Blind)
					    pline("%s is covered in acid!", Monnam(mon));
					if (noeffect || (resists_acid(mon) && !player_will_pierce_resistance()) ) {
					    if (!Blind)
						pline_The("acid doesn't burn %s!", Monnam(mon));
					    dmgbonus = 0;
					    noeffect = 0;
					}
					break;
				}
				if (dmgbonus > 0)
				    tmp += dmgbonus;
			    } /* Techinuse Elemental Fist */	

			}

			/* software engineer can occasionally instakill bugs */
			if (Role_if(PM_SOFTWARE_ENGINEER) && !mon->mtame && !mon->mpeaceful && mon->data->mlet == S_XAN && !rn2(100)) {
				/* can't have all the ZAPM stuff in the game without using the word "derezzed" at least once! */
				pline("%s is derezzed!", Monnam(mon));
				xkilled(mon,0);
				return FALSE;
			}

			/* dissidents want to leak information about their enemy, and they can't do that very well when the
			 * enemy is hiding in the shadows... so they go ahead and remove the enemy's camouflage! --Amy */
			if (Role_if(PM_DISSIDENT) && mon->minvisreal) {
				mon->minvisreal = FALSE;
				You("remove %s's disguise!", mon_nam(mon));
			}

			if (Role_if(PM_DISSIDENT) && !mon->minvisreal && mon->minvis && !rn2(20)) {
				mon->perminvis = 0;
				mon->minvis = 0;
				pline("%s's real %s is revealed!", Monnam(mon), mbodypart(mon, FACE));
			}

			if (obj->oartifact &&
			artifact_hit(&youmonst, mon, obj, &tmp, dieroll)) {
				if(mon->mhp <= 0) /* artifact killed monster */
					return FALSE;
				if (tmp == 0) return TRUE;
				hittxt = TRUE;
			}
		    if (objects[obj->otyp].oc_material == MT_SILVER && hates_silver(mdat)) {
			silvermsg = TRUE; silverobj = TRUE;
		    }
		    if (objects[obj->otyp].oc_material == MT_VIVA && hates_viva(mdat)) {
			vivaobj = TRUE;
		    }
		    if (objects[obj->otyp].oc_material == MT_COPPER && hates_copper(mdat)) {
			copperobj = TRUE;
		    }
		    if (objects[obj->otyp].oc_material == MT_PLATINUM && hates_platinum(mdat)) {
			platinumobj = TRUE;
		    }
		    if (obj->cursed && hates_cursed(mdat)) {
			cursedobj = 1;
			if (obj->hvycurse) cursedobj++;
			if (obj->prmcurse) cursedobj++;
			if (obj->bbrcurse) cursedobj++;
			if (obj->evilcurse) cursedobj++;
			if (obj->morgcurse) cursedobj++;
		    }
		    if (objects[obj->otyp].oc_material == MT_INKA && hates_inka(mdat)) {
			inkaobj = TRUE;
		    }
		    if (obj->otyp == ODOR_SHOT && hates_odor(mdat)) {
			odorobj = TRUE;
		    }
		    if (u.usteed && !thrown && tmp > 0 &&
			    weapon_type(obj) == (tech_inuse(T_GRAP_SWAP) ? P_GRINDER : P_LANCE) && mon != u.ustuck) {
			jousting = joust(mon, obj);
			/* exercise skill even for minimal damage hits */
			if (jousting) valid_weapon_attack = TRUE;
		    }
		    if (thrown && (is_ammo(obj) || is_missile(obj))) {
			if (obj->oartifact == ART_HOUCHOU) {
			    pline("There is a bright flash as it hits %s.",
				the(mon_nam(mon)));
			    tmp = dmgvalX(obj, mon);
			}
			if (ammo_and_launcher(obj, launcher)) {

			    if (obj->otyp == ANTIMATTER_PISTOL_BULLET) {
					tmp += 20;
			    }
			    if (obj->otyp == ANTIMATTER_FIVE_SEVEN_BULLET) {
					tmp += 20;
			    }
			    if (obj->otyp == ANTIMATTER_SMG_BULLET) {
					tmp += 20;
			    }
			    if (obj->otyp == ANTIMATTER_MG_BULLET) {
					tmp += 20;
			    }
			    if (obj->otyp == ANTIMATTER_RIFLE_BULLET) {
					tmp += 20;
			    }
			    if (obj->otyp == ANTIMATTER_SNIPER_BULLET) {
					tmp += 20;
			    }
			    if (obj->otyp == ANTIMATTER_ASSAULT_RIFLE_BULLE) {
					tmp += 20;
			    }

			    if (launcher && launcher->oartifact == ART_BANG_THE_HEAD_AWAY && !rn2(20) && !noncorporeal(mon->data) && !amorphous(mon->data) && has_head(mon->data) && !(mon->data->geno & G_UNIQ) ) {
				pline("Head shot!");
				tmp += 9999;
			    }

			    if (launcher && launcher->otyp == LASERXBOW && launcher->lamplit && launcher->altmode) {
					tmp += 5;
			    }

			    if (launcher->oartifact)
				tmp += spec_dbon(launcher, mon, tmp);
			    /* Elves and Samurai do extra damage using
			     * their bows&arrows; they're highly trained.
			     * WAC Only elves get dmg bonus from flurry. Change?
			     */
			    if (Role_if(PM_SAMURAI) && obj->otyp == YA && launcher->otyp == YUMI)
				tmp++;
			    if (Role_if(PM_SAMURAI) && obj->otyp == FAR_EAST_ARROW && launcher->otyp == YUMI)
				tmp++;

				if (Race_if(PM_ELF) || Race_if(PM_PLAYER_MYRKALFR)) {
				if (obj->otyp == ELVEN_ARROW &&
					launcher->otyp == ELVEN_BOW) {
				tmp++;
				    /* WAC Extra damage if in special ability*/
				    if (tech_inuse(T_FLURRY)) tmp += 2;
				} else if (objects[obj->otyp].oc_skill == P_BOW
					&& tech_inuse(T_FLURRY)) {
				tmp++;
				}
			    }
				if (Role_if(PM_ELPH)) {
				if (obj->otyp == ELVEN_ARROW &&
					launcher->otyp == ELVEN_BOW) {
				tmp++;
				    /* WAC Extra damage if in special ability*/
				    if (tech_inuse(T_FLURRY)) tmp += 2;
				} else if (objects[obj->otyp].oc_skill == P_BOW
					&& tech_inuse(T_FLURRY)) {
				tmp++;
				}
			    }

				if (Role_if(PM_TWELPH)) {
				if (obj->otyp == DARK_ELVEN_ARROW &&
					launcher->otyp == DARK_ELVEN_BOW) {
				tmp++;
				    /* WAC Extra damage if in special ability*/
				    if (tech_inuse(T_FLURRY)) tmp += 2;
				} else if (objects[obj->otyp].oc_skill == P_BOW
					&& tech_inuse(T_FLURRY)) {
				tmp++;
				}
			    }

			      if (Role_if(PM_ROCKER)) {
				if ((obj->otyp == SLING) && tech_inuse(T_FLURRY)) tmp += 2;
				if ((obj->otyp == INKA_SLING) && tech_inuse(T_FLURRY)) tmp += 2;
				if ((obj->otyp == ATLATL) && tech_inuse(T_FLURRY)) tmp += 2;
				if ((obj->otyp == SNIPESLING) && tech_inuse(T_FLURRY)) tmp += 2;
				if ((obj->otyp == METAL_SLING) && tech_inuse(T_FLURRY)) tmp += 2;
				if ((obj->otyp == BAGGY_SLING) && tech_inuse(T_FLURRY)) tmp += 2;
				if ((obj->otyp == SHOVEL) && tech_inuse(T_FLURRY)) tmp += 2;
				if ((obj->otyp == CATAPULT) && tech_inuse(T_FLURRY)) tmp += 5;
				tmp++;
				
			    }
				if (Race_if(PM_DROW)) {
				if (obj->otyp == DARK_ELVEN_ARROW &&
					launcher->otyp == DARK_ELVEN_BOW) {
				    tmp += 2;
				    /* WAC Mucho damage if in special ability*/
				    if (tech_inuse(T_FLURRY)) tmp *= 2;
				} else if (objects[obj->otyp].oc_skill == P_BOW
					&& tech_inuse(T_FLURRY)) {
				    tmp++;
				}
			    }

			} /* ranged attack check */
		    }
		    /* MRKR: Hitting with a lit torch does extra */
		    /*       fire damage, but uses up the torch  */
		    /*       more quickly.                       */

		    if(obj && obj->otyp == TORCH && obj->lamplit && (!resists_fire(mon) || player_will_pierce_resistance() ) ) {

		      burnmsg = TRUE;

		      tmp++;
		      if (resists_cold(mon)) tmp += rnd(3);

		      /* Additional damage due to burning armor */
		      /* & equipment is delayed to below, after */
		      /* the hit messages are printed. */
		    }

		    if(obj && obj->otyp == LIGHTTORCH && obj->lamplit && (!resists_fire(mon) || player_will_pierce_resistance() ) ) {

		      burnmsg = TRUE;

		      tmp++;
		      if (resists_cold(mon)) tmp += rnd(3);

		      /* Additional damage due to burning armor */
		      /* & equipment is delayed to below, after */
		      /* the hit messages are printed. */
		    }

		    if (thrown && obj && obj->otyp == FLAMETHROWER && (!resists_fire(mon) || player_will_pierce_resistance() ) ) {
		      burnmsg = TRUE; /* special-cased because flamethrowers aren't lit */
			tmp += rnd(6);
		      if (resists_cold(mon)) tmp += rnd(6);
		    }

			if (thrown && obj && obj->otyp == LASER_FLYAXE && obj->lamplit) tmp += 2;
			if (thrown && obj && obj->otyp == DISKOS) {
				tmp += 5;
			}
			if (thrown && obj && obj->otyp == TOMAHAWK && !PlayerCannotUseSkills) {
				switch (P_SKILL(P_AXE)) {
					default: break;
					case P_BASIC: tmp += 2; break;
					case P_SKILLED: tmp += 4; break;
					case P_EXPERT: tmp += 6; break;
					case P_MASTER: tmp += 8; break;
					case P_GRAND_MASTER: tmp += 10; break;
					case P_SUPREME_MASTER: tmp += 12; break;
				}

			}

			if (thrown && obj && obj->otyp == HEAVY_SPEAR) {
				if (tmp > 0) tmp *= 2;
			}
			if (thrown && obj && obj->otyp == SUPERHEAVY_SPEAR) {
				if (tmp > 0) {
					tmp *= 5;
					tmp /= 2;
				}
			}
			if (thrown && obj && obj->oartifact == ART_BURDENSOME_LOG) { /* intentionally below the doubling */
				tmp += 15;
			}

			if (obj && obj->otyp == BLINDING_VENOM) {

				if (can_blnd(&youmonst, mon, AT_SPIT, obj)) {
				    int blindnessdur = rn1(25, 21);
				    if (Blind) {
					pline("Splash!");
				    } else {
					pline_The("venom blinds %s%s!", mon_nam(mon), mon->mcansee ? "" : " further");
				    }
				    setmangry(mon);
				    mon->mcansee = 0;
				    if(((int) mon->mblinded + blindnessdur) > 127)
					mon->mblinded = 127;
				    else mon->mblinded += blindnessdur;
				} else {
				    pline("Splash!");
				    setmangry(mon);
				}
	
			}

			if (obj && (obj->otyp == SEGFAULT_VENOM || obj->otyp == FAERIE_FLOSS_RHING || obj->otyp == TAIL_SPIKES) ) {
				if (obj->otyp == FAERIE_FLOSS_RHING && !resists_drain(mon) ) {
					if (mon->mhpmax > 1) {
						mon->mhpmax--;
						if (mon->mhp > mon->mhpmax) mon->mhpmax--;
						pline("%s was hit by the faerie floss rhing and is now small.", Monnam(mon));
					}
				}
			}

			if (obj && obj->otyp == ACID_VENOM) {
				if (resists_acid(mon) && !player_will_pierce_resistance()) {
					Your("venom hits %s harmlessly.", mon_nam(mon));
					tmp = 0;
				} else {
					Your("venom burns %s!", mon_nam(mon));
				}

			}

		    if (obj && obj->oclass == VENOM_CLASS) {
			if (!thrown) useup(obj);
			/* if (thrown) obfree(obj, (struct obj *)0); */
		    }

		}
	    } else if(obj->oclass == POTION_CLASS) {
		if (!u.twoweap || obj == uwep) {
		if (obj->quan > 1L)
		    obj = splitobj(obj, 1L);
		else
		    setuwep((struct obj *)0, FALSE, TRUE);
		} else if (u.twoweap && obj == uswapwep) {
		    if (obj->quan > 1L)
			setworn(splitobj(obj, 1L), W_SWAPWEP);
		    else
			setuswapwep((struct obj *)0, FALSE);
		}
		freeinv(obj);
		potionhit(mon, obj, TRUE);
		if (mon->mhp <= 0) return FALSE;	/* killed */
		hittxt = TRUE;
		/* in case potion effect causes transformation */
		mdat = mon->data;
		tmp = (is_shade(mdat) || mon->egotype_shader) ? 0 : 1;
	    } else {
		if (flags.bash_reminder && obj->otyp != BOULDER && obj->otyp != WEAPON_SIGN && obj->oclass != VENOM_CLASS && !rn2(10)) pline("A helpful reminder: you attack with a non-weapon!");
		if ((is_shade(mdat) || mon->egotype_shader) && !shade_aware(obj)) {
		    tmp = 0;
		    strcpy(unconventional, cxname(obj));
		} else {
		switch(obj->otyp) {
		    case BOULDER:		/* 1d20 */
			tmp = dmgvalX(obj, mon);
			if (thrown && obj->oartifact == ART_BLOHIT) tmp += 10;
			if (thrown) use_skill(P_BOULDER_THROWING, 1);
			if (thrown && !PlayerCannotUseSkills) {
				switch (P_SKILL(P_BOULDER_THROWING)) {
					default: break;
					case P_BASIC: tmp += rnd(3); break;
					case P_SKILLED: tmp += rnd(6); break;
					case P_EXPERT: tmp += rnd(9); break;
					case P_MASTER: tmp += rnd(12); break;
					case P_GRAND_MASTER: tmp += rnd(15); break;
					case P_SUPREME_MASTER: tmp += rnd(18); break;
				}
			}
			break;
		    case HEAVY_IRON_BALL:	/* 1d25 */
		    case REALLY_HEAVY_IRON_BALL:	/* 1d25 */
		    case EXTREMELY_HEAVY_IRON_BALL:	/* 1d25 */
		    case QUITE_HEAVY_IRON_BALL:	/* 1d25 */
		    case IMPOSSIBLY_HEAVY_IRON_BALL:	/* 1d25 */
		    case HEAVY_STONE_BALL:	/* 1d25 */
		    case HEAVY_GLASS_BALL:	/* 1d25 */
		    case HEAVY_GOLD_BALL:	/* 1d25 */
		    case HEAVY_ELYSIUM_BALL:	/* 1d25 */
		    case HEAVY_CLAY_BALL: 
		    case HEAVY_GRANITE_BALL: 
		    case HEAVY_CONUNDRUM_BALL: 
		    case HEAVY_LASER_BALL: 
		    case HEAVY_TINSEL_BALL: 
		    case AMBER_BALL: 
		    case HEAVY_CONCRETE_BALL: 
		    case HEAVY_BRICK_BALL: 
		    case HEAVY_PAPER_BALL:
		    case HEAVY_CHROME_BALL:
		    case HEAVY_NANO_BALL:
		    case IMPOSSIBLY_HEAVY_SAND_BALL:
		    case HEAVY_METAL_BALL:
		    case HEAVY_PLASTIC_BALL:
		    case HEAVY_SHADOW_BALL:
		    case HEAVY_LEAD_BALL:
		    case IMPOSSIBLY_HEAVY_ETHER_BALL:
		    case HEAVY_WAX_BALL:
		    case HEAVY_ALLOY_BALL:
		    case HEAVY_SCRAP_BALL:
		    case HEAVY_ANTIDOTE_BALL:
		    case HEAVY_METEORIC_BALL:
		    case IMPOSSIBLY_HEAVY_ALLOY_BALL:
		    case HEAVY_MESH_BALL:
		    case HEAVY_FIRMAMENT_BALL:
		    case HEAVY_SINNUM_BALL:
		    case HEAVY_CORAL_BALL:
		    case HEAVY_CHITIN_BALL:
		    case HEAVY_BRONZE_BALL:
		    case HEAVY_TITANIUM_BALL:
		    case HEAVY_STEEL_BALL:
		    case HEAVY_MERCURIAL_BALL:
		    case HEAVY_BAMBOO_BALL:
		    case HEAVY_ALU_BALL:
		    case HEAVY_COBALT_BALL:
		    case GREEN_STEEL_BALL:
		    case MESH_CHAIN:
		    case FIRMAMENT_CHAIN:
		    case SINNUM_SCOURGE:
		    case CORAL_NUNCHIAKU:
		    case CHITIN_CHAIN:
		    case BRONZE_CHAIN:
		    case TITANIUM_SCOURGE:
		    case STEEL_NUNCHIAKU:
		    case MERCURIAL_CHAIN:
		    case BAMBOO_CHAIN:
		    case ALU_SCOURGE:
		    case COBALT_NUNCHIAKU:
		    case GREEN_STEEL_CHAIN:
		    case HEAVY_BUBBLE_BALL:
		    case HEAVY_FOAM_BALL:
		    case HEAVY_CELESTIAL_BALL:
		    case HEAVY_ZEBETITE_BALL:
		    case IMPOSSIBLY_HEAVY_FOAM_BALL:
		    case HEAVY_WOOD_BALL:
		    case HEAVY_COPPER_BALL:
		    case HEAVY_SILVER_BALL:
		    case IMPOSSIBLY_HEAVY_OBSIDIAN_BALL:
		    case BULKY_POURPOOR_BALL:
		    case BULKY_GEMSTONE_BALL:
		    case BULKY_FLESH_BALL:
		    case BULKY_MITHRIL_BALL:
		    case HEAVY_CLOTH_BALL:
		    case HEAVY_TAR_BALL:
		    case HEAVY_SILK_BALL:
		    case HEAVY_LEATHER_BALL:
		    case WONDER_BALL:
		    case NULL_BALL:
		    case LIQUID_BALL:
		    case BRICK_CHAIN:
		    case PAPER_CHAIN:
		    case CHROME_SCOURGE:
		    case NANO_NUNCHIAKU:
		    case SAND_HOSTAGE_CHAIN:
		    case METAL_CHAIN:
		    case PLASTIC_CHAIN:
		    case SHADOW_SCOURGE:
		    case LEAD_NUNCHIAKU:
		    case ETHER_HOSTAGE_CHAIN:
		    case WAX_CHAIN:
		    case ALLOY_CHAIN:
		    case SCRAP_CHAIN:
		    case ANTIDOTE_SCOURGE:
		    case METEORIC_NUNCHIAKU:
		    case ALLOY_HOSTAGE_CHAIN:
		    case BUBBLE_CHAIN:
		    case FOAM_CHAIN:
		    case CELESTIAL_SCOURGE:
		    case ZEBETITE_NUNCHIAKU:
		    case FOAM_HOSTAGE_CHAIN:
		    case WOOD_CHAIN:
		    case COPPER_SCOURGE:
		    case SILVER_NUNCHIAKU:
		    case OBSIDIAN_HOSTAGE_CHAIN:
		    case POURPOOR_LASH:
		    case GEMSTONE_LASH:
		    case FLESH_LASH:
		    case MITHRIL_LASH:
		    case CLOTH_CHAIN:
		    case TAR_CHAIN:
		    case SILK_SCOURGE:
		    case LEATHER_NUNCHIAKU:
		    case WONDER_CHAIN:
		    case NULL_CHAIN:
		    case TINSEL_CHAIN:
		    case AMBER_CHAIN:
		    case LASER_CHAIN:
		    case LIQUID_CHAIN:
		    case IMPOSSIBLY_HEAVY_GLASS_BALL:
		    case IMPOSSIBLY_HEAVY_ELYSIUM_BALL:
		    case IMPOSSIBLY_HEAVY_MINERAL_BALL:
		    case IRON_CHAIN:		/* 1d4+1 */
		    case STONE_CHAIN:		/* 1d4+1 */
		    case GLASS_CHAIN:		/* 1d4+1 */
		    case ROTATING_CHAIN:		/* 1d4+1 */
		    case GOLD_CHAIN:		/* 1d4+1 */
		    case CLAY_CHAIN:		/* 1d4+1 */
		    case SCOURGE:		/* 1d4+1 */
		    case ELYSIUM_SCOURGE:		/* 1d4+1 */
		    case GRANITE_SCOURGE:		/* 1d4+1 */
		    case NUNCHIAKU:		/* 1d4+1 */
		    case CONUNDRUM_NUNCHIAKU:		/* 1d4+1 */
		    case CONCRETE_NUNCHIAKU:		/* 1d4+1 */
		    case HOSTAGE_CHAIN:		/* 1d4+1 */
		    case GLASS_HOSTAGE_CHAIN:		/* 1d4+1 */
		    case MINERAL_HOSTAGE_CHAIN:		/* 1d4+1 */
		    case ELYSIUM_HOSTAGE_CHAIN:		/* 1d4+1 */
		    case VERY_HEAVY_BALL:
		    case HEAVY_CHAIN:
		    case HEAVY_COMPOST_BALL:
		    case COMPOST_CHAIN:
		    case DISGUSTING_BALL:
		    case DISGUSTING_CHAIN:
		    case HEAVY_ELASTHAN_BALL:
		    case ELASTHAN_CHAIN:
		    case IMPOSSIBLY_HEAVY_NUCLEAR_BALL:
		    case NUCLEAR_HOSTAGE_CHAIN:
			tmp = dmgvalX(obj, mon);
			break;
		    case MIRROR:
			if (breaktest(obj)) {
			    You("break %s mirror.  That's bad luck!",
				shk_your(yourbuf, obj));
			    change_luck(-2);
			    useup(obj);
			    obj = (struct obj *) 0;
			    unarmed = FALSE;	/* avoid obj==0 confusion */
			    get_dmg_bonus = FALSE;
			    hittxt = TRUE;
			}
			tmp = 1;
			break;
		    case EXPENSIVE_CAMERA:
			You("succeed in destroying %s camera.  Congratulations!",
			    shk_your(yourbuf, obj));
			useup(obj);
			return(TRUE);
			/*NOTREACHED*/
			break;
		    case CORPSE:		/* fixed by polder@cs.vu.nl */

			if (!rn2(100) || nocorpsedecay(&mons[obj->corpsenm])) {
			/* kludge, mainly for cursed lizards but also because of general c corpse overpoweredness --Amy */

				if (thrown) obfree(obj, (struct obj *)0);
				else useup(obj);
				pline("The corpse rotted away completely.");
				return(TRUE);

			} else if (touch_petrifies(&mons[obj->corpsenm])) {
			    static const char withwhat[] = "corpse";
			    tmp = 1;
			    hittxt = TRUE;
			    You("hit %s with %s %s.", mon_nam(mon),
				obj->dknown ? the(mons[obj->corpsenm].mname) :
				an(mons[obj->corpsenm].mname),
				(obj->quan > 1) ? makeplural(withwhat) : withwhat);
			    if (!munstone(mon, TRUE) && !rn2(4))
				minstapetrify(mon, TRUE);
			    if (resists_ston(mon)) break;
			    /* note: hp may be <= 0 even if munstoned==TRUE */
			    return (boolean) (mon->mhp > 0);
#if 0
			} else if (touch_petrifies(mdat)) {
			    /* maybe turn the corpse into a statue? */
#endif
			}
			tmp = (obj->corpsenm >= LOW_PM ?
					mons[obj->corpsenm].msize : 0) + 1;
			break;
		    case EGG:
		      {
#define useup_eggs(o)	{ if (thrown) obfree(o,(struct obj *)0); \
			  else useupall(o); \
			  o = (struct obj *)0; }	/* now gone */
			long cnt = obj->quan;

			tmp = 1;		/* nominal physical damage */
			get_dmg_bonus = FALSE;
			hittxt = TRUE;		/* message always given */
			/* egg is always either used up or transformed, so next
			   hand-to-hand attack should yield a "bashing" mesg */
			if (obj == uwep) unweapon = TRUE;
			if (obj->spe && obj->corpsenm >= LOW_PM) {
			    if (obj->quan < 5)
				change_luck((schar) -(obj->quan));
			    else
				change_luck(-5);
			}

			if (touch_petrifies(&mons[obj->corpsenm]) && obj->corpsenm != PM_PLAYERMON) {
			    /*learn_egg_type(obj->corpsenm);*/
			    pline("Splat! You hit %s with %s %s egg%s!",
				mon_nam(mon),
				obj->known ? "the" : cnt > 1L ? "some" : "a",
				obj->known ? mons[obj->corpsenm].mname : "petrifying",
				plur(cnt));
#if 0
			    obj->known = 1;	/* (not much point...) */
#endif
			    useup_eggs(obj);
			    if (!munstone(mon, TRUE) && (!rn2(4) || thrown) ) /* always petrify if thrown --Amy */
				minstapetrify(mon, TRUE);
			    if (resists_ston(mon)) break;
			    return (boolean) (mon->mhp > 0);
			} else {	/* ordinary egg(s) */
			    const char *eggp =
				     (obj->corpsenm != NON_PM && obj->known) ?
					      the(mons[obj->corpsenm].mname) :
					      (cnt > 1L) ? "some" : "an";
			    You("hit %s with %s egg%s.",
				mon_nam(mon), eggp, plur(cnt));
			    if (touch_petrifies(mdat) && !stale_egg(obj)) {
				pline_The("egg%s %s alive any more...",
				      plur(cnt),
				      (cnt == 1L) ? "isn't" : "aren't");
				if (obj->timed) obj_stop_timers(obj);
				obj->otyp = ROCK;
				obj->oclass = GEM_CLASS;
				obj->oartifact = 0;
				obj->spe = 0;
				obj->known = obj->dknown = obj->bknown = 0;
				obj->owt = weight(obj);
				if (thrown) place_object(obj, mon->mx, mon->my);
			    } else {
				pline("Splat!");
				useup_eggs(obj);
				exercise(A_WIS, FALSE);
			    }
			}
			break;
#undef useup_eggs
		      }
		    case CLOVE_OF_GARLIC:	/* no effect against demons */
			if (is_undead(mdat) || mon->egotype_undead) {

				/* nerf by Amy - shouldn't be infinite, non-vampiric undead are highly resistant */
				if (!rn2(obj->oartifact ? 1000 : 100)) {

					/* amateurhour wants garlicators to punish players who constantly use garlic */
					if (!rn2(100)) { /* but they should be really rare, because garlic isn't that strong --Amy */
						int garlicatormon = PM_LOWER_GARLICATOR;
						if (level_difficulty() > 9) garlicatormon = PM_GARLICATOR;
						if (level_difficulty() > 19) garlicatormon = PM_STRONGER_GARLICATOR;
						if (level_difficulty() > 29) garlicatormon = PM_VERY_STRONG_GARLICATOR;
						if (level_difficulty() > 49) garlicatormon = PM_EXTRA_STRONG_GARLICATOR;
						register struct monst *garlicone;

						garlicone = makemon(&mons[garlicatormon], 0, 0, MM_ADJACENTOK|MM_ANGRY);
						if (garlicone) {
							makemon(&mons[garlicatormon], garlicone->mx, garlicone->my, MM_ADJACENTOK|MM_ANGRY);
							makemon(&mons[garlicatormon], garlicone->mx, garlicone->my, MM_ADJACENTOK|MM_ANGRY);
						}

						if (!rn2(20) && (mvitals[PM_ROSY__PRINCESS_OF_GARLIC].born == 0) && level_difficulty() > 29) {
							(void) makemon(&mons[PM_ROSY__PRINCESS_OF_GARLIC], 0, 0, MM_ADJACENTOK|MM_ANGRY);
						}
					}

					if (!rn2(2)) {
						if (obj->oeroded < MAX_ERODE) {
							obj->oeroded++;
							Your("clove of garlic degrades.");
						} else {
							useup(obj);
							Your("clove of garlic has been used up.");
							return(TRUE);
						}
					} else {
						if (obj->oeroded2 < MAX_ERODE) {
							obj->oeroded2++;
							Your("clove of garlic degrades.");
						} else {
							useup(obj);
							Your("clove of garlic has been used up.");
							return(TRUE);
						}

					}

				}

				if (obj->oartifact == ART_DIVINE_GARLIC) {
					if ( (!resist(mon, WEAPON_CLASS, 0, NOTELL) || is_vampire(mdat)) && (is_vampire(mdat) || !rn2(3)) ) {
						monflee(mon, d(2, 4), FALSE, TRUE);
						mon->mhpmax--;
						mon->mhp -= 5;
						if (mon->mhp < 1) mon->mhp = 1;
						if (mon->mhpmax < 1) mon->mhpmax = 1;
						if (mon->mhp > mon->mhpmax) mon->mhp = mon->mhpmax;
					}

				} else {
					if (!resist(mon, WEAPON_CLASS, 0, NOTELL) && (is_vampire(mdat) || !rn2(3)) ) {
						monflee(mon, d(2, 4), FALSE, TRUE);
					}

				}

			}
			tmp = 1;
			break;
		    case CREAM_PIE:
		    case BLINDING_VENOM:
			mon->msleeping = 0;
			if (can_blnd(&youmonst, mon, (uchar)
				    (obj->otyp == BLINDING_VENOM
				     ? AT_SPIT : AT_WEAP), obj)) {
			    if (Blind) {
				pline(obj->otyp == CREAM_PIE ?
				      "Splat!" : "Splash!");
			    } else if (obj->otyp == BLINDING_VENOM) {
				pline_The("venom blinds %s%s!", mon_nam(mon),
					  mon->mcansee ? "" : " further");
			    } else {
				char *whom = mon_nam(mon);
				char *what = The(xname(obj));
				if (!thrown && obj->quan > 1)
				    what = An(singular(obj, xname));
				/* note: s_suffix returns a modifiable buffer */
				if (haseyes(mdat)
				    && mdat != &mons[PM_FLOATING_EYE])
				    whom = strcat(strcat(s_suffix(whom), " "),
						  mbodypart(mon, FACE));
				pline("%s %s over %s!",
				      what, vtense(what, "splash"), whom);
			    }
			    setmangry(mon);
			    mon->mcansee = 0;
			    tmp = rn1(25, 21);
			    if(((int) mon->mblinded + tmp) > 127)
				mon->mblinded = 127;
			    else mon->mblinded += tmp;
			} else {
			    pline(obj->otyp==CREAM_PIE ? "Splat!" : "Splash!");
			    setmangry(mon);
			}
			if (thrown) obfree(obj, (struct obj *)0);
			else useup(obj);
			hittxt = TRUE;
			get_dmg_bonus = FALSE;
			tmp = 0;
			break;
		    case ACID_VENOM: /* thrown (or spit) */
			if (resists_acid(mon) && !player_will_pierce_resistance()) {
				Your("venom hits %s harmlessly.",
					mon_nam(mon));
				tmp = 0;
			} else {
				Your("venom burns %s!", mon_nam(mon));
				tmp = dmgvalX(obj, mon);
			}
			if (thrown) obfree(obj, (struct obj *)0);
			else useup(obj);
			hittxt = TRUE;
			get_dmg_bonus = FALSE;
			break;
		    default:
			/* non-weapons can damage because of their weight */
			/* (but not too much) */
			tmp = obj->owt/40;
			if(tmp < 1) tmp = 1;
			else tmp = rnd(tmp);
			if(tmp > 10) tmp = 10; /* slight increase --Amy */

			if (obj && obj->otyp == WEAPON_SIGN) {
				int weaponsign = 10;
				if (obj->spe > 0) weaponsign += obj->spe;

				if (!(PlayerCannotUseSkills)) {
					switch (P_SKILL(P_SHIELD)) {
						case P_BASIC: weaponsign += 2; break;
						case P_SKILLED: weaponsign += 4; break;
						case P_EXPERT: weaponsign += 6; break;
						case P_MASTER: weaponsign += 8; break;
						case P_GRAND_MASTER: weaponsign += 10; break;
						case P_SUPREME_MASTER: weaponsign += 12; break;
					}
				}

				if (tech_inuse(T_SHIELD_BASH)) {
					weaponsign += (3 + obj->spe);
					if (!(PlayerCannotUseSkills)) {
						switch (P_SKILL(P_SHIELD)) {
							case P_BASIC: weaponsign += 1; break;
							case P_SKILLED: weaponsign += 2; break;
							case P_EXPERT: weaponsign += 3; break;
							case P_MASTER: weaponsign += 4; break;
							case P_GRAND_MASTER: weaponsign += 5; break;
							case P_SUPREME_MASTER: weaponsign += 6; break;
						}
					}
					if (obj->oartifact == ART_SPECTRATE_ETTECKOR) weaponsign += 10;
				}
				if (Role_if(PM_PALADIN) && weaponsign > 0) weaponsign *= 2;

				if (weaponsign > 0) tmp += rnd(weaponsign);
			}

			if (obj && itemhasappearance(obj, APP_HAMMER_SHOES)) {
				tmp += 10;
				if (!(PlayerCannotUseSkills)) {
					switch (P_SKILL(P_BLOCK_HEELS)) {
						case P_BASIC: tmp += rnd(2); break;
						case P_SKILLED: tmp += rnd(4); break;
						case P_EXPERT: tmp += rnd(6); break;
						case P_MASTER: tmp += rnd(8); break;
						case P_GRAND_MASTER: tmp += rnd(10); break;
						case P_SUPREME_MASTER: tmp += rnd(12); break;
					}
				}
			}

			if (obj && obj->oartifact == ART_EXCALISHIELD) {
				tmp += 6;
			}
			if (obj && obj->oartifact == ART_BLAMMER_OF_DOOM) {
				tmp += 10;
			}
			if (obj && obj->oartifact == ART_INVUNI && obj->otyp == BUGLE) {
				tmp += 5;
			}

			if (obj && obj->oclass == SPBOOK_CLASS && obj->oartifact) {
				tmp += rnd(10);
				if (obj->spe > 0) tmp += obj->spe;
				if (obj->oartifact == ART_DEADLY_GAMBLING) tmp += rnd(30);
			}

			if (obj && obj->oclass == SPBOOK_CLASS && Role_if(PM_LIBRARIAN)) {
				if (obj->spe > 0) tmp += obj->spe;
			}

			if (obj && obj->oartifact == ART_FOR_THE_PERMABLIND_POTATO && !obj->dknown) {
				tmp += 20;
			}

			if (obj && obj->oclass == WAND_CLASS && obj->oartifact && obj->spe > 0) {
				tmp += rno(obj->spe);
			}
			if (obj && obj->oclass == WAND_CLASS && Race_if(PM_STICKER) && obj->spe > 0) {
				tmp += rno(obj->spe);
			}

			if (obj && obj->oartifact == ART_ICE_BLOCK_HARHARHARHARHAR) {

				struct obj *curr;
				int cnt = 0;

				for (curr = obj->cobj; curr; curr = curr->nobj)
				    cnt++;
				if (cnt > 15) cnt = 15;
				if (cnt < 0) cnt = 0; /* should never happen; fail safe */
				tmp += (cnt * 2);

			}

			/* artifact spellbooks etc., by Amy */

			if (obj->oartifact) {

				boolean willartihit = FALSE;
				register const struct artifact *weap = get_artifact(obj);

				if (weap) {
					if (weap->attk.adtyp != AD_PHYS) willartihit = TRUE;
					if (weap->attk.damn != 0) willartihit = TRUE;
					if (weap->attk.damd != 0) willartihit = TRUE;
				}

				if (willartihit) {
					if (artifact_hit(&youmonst, mon, obj, &tmp, dieroll)) {
						if(mon->mhp <= 0) /* artifact killed monster */
							return FALSE;
						if (tmp == 0) return TRUE;
						hittxt = TRUE;
					}
				}

			}

			/*
			 * Things like silver wands can arrive here so
			 * so we need another silver check.
			 */
			if (objects[obj->otyp].oc_material == MT_SILVER && hates_silver(mdat)) {
				tmp += rnd(20);
				silvermsg = TRUE; silverobj = TRUE;
			}
			if (objects[obj->otyp].oc_material == MT_COPPER && hates_copper(mdat)) {
				tmp += 20;
				copperobj = TRUE;
			}
			if (objects[obj->otyp].oc_material == MT_MERCURIAL && !rn2(10) && !(resists_poison(mon)) ) {
				tmp += rnd(4);
				mercurialobj = TRUE;
			}
			if (objects[obj->otyp].oc_material == MT_PLATINUM && hates_platinum(mdat)) {
				tmp += 20;
				platinumobj = TRUE;
			}
			if (obj->cursed && hates_cursed(mdat)) {
				tmp += 4;
				if (obj->hvycurse) tmp += 4;
				if (obj->prmcurse) tmp += 7;
				if (obj->bbrcurse) tmp += 15;
				if (obj->evilcurse) tmp += 15;
				if (obj->morgcurse) tmp += 15;
				cursedobj = 1;
				if (obj->hvycurse) cursedobj++;
				if (obj->prmcurse) cursedobj++;
				if (obj->bbrcurse) cursedobj++;
				if (obj->evilcurse) cursedobj++;
				if (obj->morgcurse) cursedobj++;
			}
			if (objects[obj->otyp].oc_material == MT_VIVA && hates_viva(mdat)) {
				tmp += 20;
				vivaobj = TRUE;
			}
			if (objects[obj->otyp].oc_material == MT_INKA && hates_inka(mdat)) {
				tmp += 5;
				inkaobj = TRUE;
			}
			if (obj->otyp == ODOR_SHOT && hates_odor(mdat)) {
				tmp += rnd(10);
				odorobj = TRUE;
			}
		    }
		}
	    }
	}

	/* dancer has a combo system, inspired by splicehack, but here I used my own implementation --Amy */

	if (Role_if(PM_DANCER) && !thrown && !pieks) {
		tmp += u.dancercombostrike;
		if (u.dancercombostrike > 0) {
			You("continue dancing.");
			pline("+%d bonus!", u.dancercombostrike);
			u.dancercombostrike++;
			u.dancercomboactive = TRUE;
		} else {
			You("start dancing.");
			u.dancercombostrike++;
			u.dancercomboactive = TRUE;
		}
	}

	if (obj && obj->oartifact == ART_MANA_SLASHER) {
		mon->m_en -= 5;
		if (mon->m_en < 0) {
			mon->m_en = 0;
			mon->m_enmax -= rnd(5);
			if (mon->m_enmax < 0) mon->m_enmax = 0;
		}
	}

	if (uwep && uwep->oartifact == ART_DACHA_DACHA_DACHA) {
		tmp += u.dachacombostrike;
		if (u.dachacombostrike) {
			pline("+%d combo!", u.dachacombostrike);
			u.dachacombostrike++;
			u.dachacomboactive = TRUE;
		} else {
			You("start your combo.");
			u.dachacombostrike++;
			u.dachacomboactive = TRUE;
		}
	}

	/* negative effects go here --Amy */

	if (obj && obj->oartifact == ART_LUCKLESS_FOLLY && Luck > 0) tmp -= Luck;

	if (thrown && obj && (obj->oartifact == ART_TRAPPERATE) && isok(mon->mx, mon->my) && !(t_at(mon->mx, mon->my)) ) {
		(void) maketrap(mon->mx, mon->my, rndtrap(), 100, TRUE);
	}

	if (thrown && obj && (obj->oartifact == ART_FEMMY_LOVES_YOU) ) {
		if (!FemaleTrapFemmy) pline("Femmy loves you!");
		FemaleTrapFemmy += rnd(1000);
	}

	if (obj && obj->otyp == COLLUSION_KNIFE && !(Race_if(PM_PLAYER_NIBELUNG) && rn2(5))) {
		pline("Collusion!");
		litroomlite(FALSE);
	}

	if (obj && obj->oartifact == ART_CRUCIFIX_OF_THE_MAD_KING && !(Race_if(PM_PLAYER_NIBELUNG) && rn2(5))) {
		pline("Collusion!");
		litroomlite(FALSE);
	}

	if (obj && obj->otyp == DARKNESS_CLUB && !(Race_if(PM_PLAYER_NIBELUNG) && rn2(5))) {
		pline("Collusion!");
		litroomlite(FALSE);
	}

	if (obj && obj->oartifact == ART_FIREBIRD && obj->spe > -20 && !rn2(10)) {
		obj->spe--;
		pline("The boomerang degrades.");
	}

	/****** NOTE: perhaps obj is undefined!! (if !thrown && BOOMERANG)
	 *      *OR* if attacking bare-handed!! */

	if (thrown && stupidrock && rn2(4)) get_dmg_bonus = 0;

	if (thrown && !stupidrock && !gunused && !pieks && rn2(2)) get_dmg_bonus = 0;

	if (thrown && launcher && (launcher->otyp != ART_DOLORES__WINNING_STRAT) && obj && ammo_and_launcher(obj, launcher) && !gunused && (distmin(u.ux, u.uy, mon->mx, mon->my) == 1)) {
		get_dmg_bonus = 0;
		if (tmp > 1) tmp /= 2;
		if (flags.bash_reminder && !rn2(10)) pline("Firing that weapon at point blank range is not very effective...");
	}

	if (tech_inuse(T_DECAPABILITY) && mon && pieks && obj && objects[obj->otyp].oc_skill == P_POLEARMS && !rn2(3)) {

		if (!mon->msleeping && sleep_monst(mon, rnd(10), -1)) {
			if (!Blind) pline("%s is put to sleep by you!", Monnam(mon));
			slept_monst(mon);
		}
	}

	if (obj && obj->oartifact == ART_KABOOOOOM_ && pieks && mon && isok(mon->mx, mon->my) ) {
		explode(mon->mx, mon->my, ZT_SPELL(ZT_FIRE), d(2,6), WEAPON_CLASS, EXPL_FIERY);
		if (DEADMONSTER(mon)) return FALSE;
	}

	if (thrown && launcher && obj && ammo_and_launcher(obj, launcher) && !gunused && u.uswallow) {
		get_dmg_bonus = 0;
		if (tmp > 1) tmp /= 2;
		if (flags.bash_reminder && !rn2(10)) You("can't fire that weapon effectively while engulfed...");
	}

	if (thrown && obj && is_ammo(obj) && launcher && launcher->otyp == LASERXBOW && !launcher->lamplit) {
		if (flags.bash_reminder && !rn2(5)) pline("The laser-based crossbow is ineffective while it's not lit! You need to turn it on or it won't deal meaningful damage!");
	}

	if (thrown && obj && is_ammo(obj) && launcher && launcher->otyp == KLIUSLING && !launcher->lamplit) {
		if (flags.bash_reminder && !rn2(5)) pline("The laser-based sling is ineffective while it's not lit! You need to turn it on or it won't deal meaningful damage!");
	}

	if (thrown && obj && is_ammo(obj) && launcher && obj->otyp != FRAG_GRENADE && obj->otyp != GAS_GRENADE && !ammo_and_launcher(obj, launcher)) {
		if (flags.bash_reminder && !rn2(10)) You("are throwing projectiles that are meant to be fired, which isn't very effective! You're wielding an inappropriate launcher in your main hand, better switch to a better one!");
	}

	if (thrown && obj && is_ammo(obj) && !launcher) {
		if (flags.bash_reminder && !rn2(10)) You("are throwing projectiles that are meant to be fired, which isn't very effective! Better wield an appropriate launcher in your main hand!");
	}

	if (get_dmg_bonus && tmp > 0) {

		if (increase_damage_bonus_value() > 1) tmp += rnd(increase_damage_bonus_value());
		else tmp += increase_damage_bonus_value();

		if (uarmh && uarmh->oartifact == ART_REMOTE_GAMBLE) tmp += 2;
		if (uarm && uarm->oartifact == ART_MOTHERFUCKER_TROPHY) tmp += 5;
		if (u.tiksrvzllatdown) tmp += 1;

		if (uarmg && itemhasappearance(uarmg, APP_UNCANNY_GLOVES)) tmp += 1;
		if (uarmg && itemhasappearance(uarmg, APP_SLAYING_GLOVES)) tmp += 1;

		if (uarmc && uarmc->oartifact == ART_INA_S_SORROW && u.uhunger < 0) tmp += 3;
		if (uwep && uwep->oartifact == ART_SPAMBAIT_FIRE) tmp += 2;
		if (uwep && uwep->oartifact == ART_GARY_S_RIVALRY) tmp += 2;
		if (uarmf && uarmf->oartifact == ART_KATI_S_IRRESISTIBLE_STILET) tmp += 2;
		if (uarmf && uarmf->oartifact == ART_STREET_ROCKZ) tmp += 2;
		if (uarmf && uarmf->oartifact == ART_MAY_BRITT_S_ADULTHOOD) tmp += 1;
		if (uwep && uwep->oartifact == ART_THOR_S_STRIKE && ACURR(A_STR) >= STR19(25)) tmp += 5;
		if (uarmh && uarmh->oartifact == ART_IRON_HELM_OF_GORLIM) tmp += 10;
		if (uarmh && uarmh->oartifact == ART_SUDUNSEL) tmp += 2;
		if (uarmh && uarmh->oartifact == ART_DOUVONED) tmp += 1;
		if (uarmg && uarmg->oartifact == ART_FLOEMMELFLOEMMELFLOEMMELFL) tmp += 1;
		if (uarm && uarm->otyp == DARK_DRAGON_SCALES) tmp += 1;
		if (uarm && uarm->otyp == DARK_DRAGON_SCALE_MAIL) tmp += 1;
		if (uarms && uarms->otyp == DARK_DRAGON_SCALE_SHIELD) tmp += 1;
		if (uleft && uleft->oartifact == ART_BLIND_PILOT) tmp += 10;
		if (uright && uright->oartifact == ART_BLIND_PILOT) tmp += 10;
		if (uamul && uamul->oartifact == ART_NOW_YOU_HAVE_LOST) tmp += 10;
		if (Role_if(PM_ARCHEOLOGIST) && uamul && uamul->oartifact == ART_ARCHEOLOGIST_SONG) tmp += 2;
		if (ublindf && ublindf->oartifact == ART_EYEHANDER) tmp += 5;
		if (uarmg && uarmg->oartifact == ART_MADELINE_S_STUPID_GIRL) tmp += 3;
		if (uwep && uwep->oartifact == ART_KLOCKING_NOISE) tmp += 2;
		tmp += (Drunken_boxing && Confusion);
		if (u.boosttimer) tmp += 2;
		tmp += (StrongDrunken_boxing && Confusion);
		if (StrongFear_factor && Feared) tmp += rnd(5);
		if (RngeBloodlust) tmp += 1;
		if (uarms && uarms->oartifact == ART_TEH_BASH_R) tmp += 2;
		if (uarmc && uarmc->oartifact == ART_DUFFDUFFDUFF) tmp += 3;
		if (uarmg && uarmg->oartifact == ART_RAAAAAAAARRRRRRGH) tmp += 5;
		if (uarmg && uarmg->oartifact == ART_SI_OH_WEE) tmp += 2;
		if (powerfulimplants() && uimplant && uimplant->oartifact == ART_RHEA_S_MISSING_EYESIGHT) tmp += rnd(5);
		if (powerfulimplants() && uimplant && uimplant->oartifact == ART_SOME_LITTLE_AID) tmp += 1;
		if (Race_if(PM_VIKING)) tmp += 1;
		if (Race_if(PM_ITAQUE)) tmp -= 1;
		if (uwep && uwep->oartifact == ART_RIP_STRATEGY) tmp -= 5;
		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_RIP_STRATEGY) tmp -= 5;
		if (!thrown && Race_if(PM_TURMENE) && tmp > 0) tmp -= 2;
		if (Race_if(PM_SERB)) tmp += 1;
		if (Race_if(PM_MONGUNG)) tmp += 3;
		if (Race_if(PM_RUSMOT)) tmp += 2;
		if (uarmg && uarmg->oartifact == ART_MAJOR_PRESENCE) tmp += 2;
		if (uarmf && uarmf->oartifact == ART_SNAILHUNT) tmp += 1;
		if (uarmf && uarmf->oartifact == ART_CRASHING_YOUR_SISTER_S_WED) tmp += 2;
		if (uwep && uwep->oartifact == ART_SINSWORD && u.ualign.record < 0) tmp += 1;
		if (uwep && uwep->oartifact == ART_SINSWORD && u.ualign.record < 49) tmp += 1;
		if (uwep && uwep->oartifact == ART_SINSWORD && u.ualign.record < 99) tmp += 1;
		if (uwep && uwep->oartifact == ART_SINSWORD && u.ualign.record < 149) tmp += 1;
		if (uwep && uwep->oartifact == ART_SINSWORD && u.ualign.record < 199) tmp += 1;
		if (uwep && uwep->oartifact == ART_SINSWORD && u.ualign.record < 249) tmp += 1;
		if (uarmf && uarmf->oartifact == ART_EROTICLAMP && u.ustuck && !u.uswallow && !sticks(youmonst.data)) tmp += 2;
		if (!thrown && FemtrapActiveNatalia && flags.female && (u.nataliacycletimer >= u.nataliafollicularend) && (u.nataliacycletimer < (u.nataliafollicularend + u.natalialutealstart)) ) tmp += 2;
		if (!thrown && uarmc && uarmc->oartifact == ART_SIECHELALUER) tmp += 5;
		if (thrown && uarmh && uarmh->oartifact == ART_WAITING_FOR_MELEE) tmp -= 2;
		if (!thrown && uarmh && uarmh->oartifact == ART_WAITING_FOR_MELEE) tmp += 2;
		if (bmwride(ART_KERSTIN_S_COWBOY_BOOST)) tmp += 2;
		if (uwep && uwep->oartifact == ART_KLOBB) tmp -= 6;
		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_KLOBB) tmp -= 6;
		if (uwep && uwep->oartifact == ART_EXCALIPOOR) tmp -= 9;
		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_EXCALIPOOR) tmp -= 9;
		if (uwep && uwep->oartifact == ART_MAILIE_S_SELF_CENTRATION) tmp -= 3;
		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_MAILIE_S_SELF_CENTRATION) tmp -= 3;
		if (uwep && uwep->oartifact == ART_VLADSBANE) tmp -= 5;
		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_VLADSBANE) tmp -= 5;
		if (uwep && uwep->oartifact == ART_CHARGING_MADE_EASY) tmp -= 5;
		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_CHARGING_MADE_EASY) tmp -= 5;
		if (uarmf && uarmf->oartifact == ART_PROPERTY_GRUMBLE) tmp += 8;
		if (uarmh && uarmh->oartifact == ART_HABIBA_S_MATRONAGE) tmp += 2;
		if (!thrown && uarmg && uarmg->oartifact == ART_SUPERHEAVYKLONK) tmp += 4;
		if (uarmg && uarmg->oartifact == ART_UNKNOWINGNESS_AS_A_WEAPON && !(objects[uarmg->otyp].oc_name_known)) tmp += 5;
		if (uwep && uwep->oartifact == ART_BUCK_SHOT && !uwep->bknown) tmp += 2;
		if (uwep && uwep->oartifact == ART_FALCO_S_ORB) tmp += 1;
		if (uarm && uarm->oartifact == ART_EITHER_INTELLIGENT_OR_FAIR) tmp += 2;
		if (uarmg && uarmg->oartifact == ART_PLUS_TO_DAM) tmp += 2;
		if (uarm && uarm->oartifact == ART_YOU_ARE_UGLY) tmp += 1;
		if (uarm && uarm->oartifact == ART_THERE_GOES_SHE_TO) tmp += 4;
		if (uarm && uarm->oartifact == ART_WU_WU && u.twoweap) tmp += 2;
		if (uarms && uarms->oartifact == ART_RONDITSCH) tmp += 1;
		if (uwep && uwep->oartifact == ART_NOOBY_BONUS_STYLE && !bimanual(uwep)) tmp += 2;
		if (powerfulimplants() && uimplant && uimplant->oartifact == ART_NIOBE_S_ANGER) tmp += 2;
		if (powerfulimplants() && uimplant && uimplant->oartifact == ART_I_M_GONNA_CRUSH_YA_) tmp += 4;
		if (uarms && uarms->oartifact == ART_UNUSUAL_ENCH) tmp += 1;
		if (bmwride(ART_ZIN_BA)) tmp += 1;
		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_SACRIFICE_TONFA) tmp += 5;
		if (uarm && uarm->oartifact == ART_I_AM_YOUR_FALL) tmp += 2;
		if (uarm && uarm->oartifact == ART_ETH_ITH) tmp += 3;
		if (uarms && uarms->oartifact == ART_YOUTH_UNWORD) tmp += 2;
		if (uleft && uleft->oartifact == ART_RING_OF_THROR) tmp += 2;
		if (uright && uright->oartifact == ART_RING_OF_THROR) tmp += 2;
		if (uleft && uleft->oartifact == ART_KRATSCHEM_HARD) tmp += 2;
		if (uright && uright->oartifact == ART_KRATSCHEM_HARD) tmp += 2;
		if (uarm && uarm->otyp == JEDI_ROBE && uwep && is_lightsaber(uwep) && uwep->lamplit ) tmp += 1;
		if (!thrown && obj && obj->otyp == SUPERWEAPON) tmp += 5;

		if (pieks && obj && obj->otyp == LAJATANG) {
			tmp += 5;
			if (!PlayerCannotUseSkills) {
				switch (P_SKILL(P_QUARTERSTAFF)) {

					case P_BASIC:	tmp +=  1; break;
					case P_SKILLED:	tmp +=  rnd(3); break;
					case P_EXPERT:	tmp +=  rnd(5); break;
					case P_MASTER:	tmp +=  rnd(8); break;
					case P_GRAND_MASTER:	tmp +=  rnd(11); break;
					case P_SUPREME_MASTER:	tmp +=  rnd(14); break;
					default: tmp += 0; break;
				}

			}
		}

		if (u.martialstyle == MARTIALSTYLE_HAIDONGGUMDO && uwep && uwep->otyp == JEONTU_GEOM) {
			tmp += 5;
			if (!PlayerCannotUseSkills) {
				switch (P_SKILL(P_MARTIAL_ARTS)) {

					case P_BASIC:	tmp +=  1; break;
					case P_SKILLED:	tmp +=  2; break;
					case P_EXPERT:	tmp +=  3; break;
					case P_MASTER:	tmp +=  4; break;
					case P_GRAND_MASTER:	tmp +=  5; break;
					case P_SUPREME_MASTER:	tmp +=  6; break;
					default: tmp += 0; break;
				}

			}
		}

		if (PlayerInStilettoHeels && tech_inuse(T_HEEL_STAB) && obj && weapon_type(obj) == P_KNIFE && !thrown) {
			tmp += 5;
			if (uarmf && uarmf->spe > 0) tmp += uarmf->spe;
		}

		if (uwep && uwep->oartifact == ART_AK_____) {
			if (!PlayerCannotUseSkills) {
				if (P_SKILL(P_FIREARM) < P_BASIC) tmp += 3;
				else if (P_SKILL(P_FIREARM) == P_BASIC) tmp += 2;
				else if (P_SKILL(P_FIREARM) == P_SKILLED) tmp += 1;
			}
			if (P_SKILL(P_FIREARM) == P_MASTER) tmp -= 2;
			if (P_SKILL(P_FIREARM) == P_GRAND_MASTER) tmp -= 4;
			if (P_SKILL(P_FIREARM) == P_SUPREME_MASTER) tmp -= 6;
		}
		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_AK_____) {
			if (P_SKILL(P_FIREARM) == P_MASTER) tmp -= 2;
			if (P_SKILL(P_FIREARM) == P_GRAND_MASTER) tmp -= 4;
			if (P_SKILL(P_FIREARM) == P_SUPREME_MASTER) tmp -= 6;
		}

		if (uwep && uwep->oartifact == ART_BLACK_MARK) tmp -= 1;
		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_BLACK_MARK) tmp -= 1;
		if (uarm && uarm->oartifact == ART_POWASPEL) tmp -= 3;
		if (uwep && uwep->oartifact == ART_BEEEEEEEP) tmp -= 3;
		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_BEEEEEEEP) tmp -= 3;

		if (Role_if(PM_OTAKU) && uarmc && itemhasappearance(uarmc, APP_FOURCHAN_CLOAK)) tmp += 1;

		if (Race_if(PM_RODNEYAN)) tmp += (1 + (GushLevel / 3) );
		/* If you throw using a propellor, you don't get a strength
		 * bonus but you do get an increase-damage bonus.
		 */
		if(!thrown || !obj || !uwep || !ammo_and_launcher(obj, launcher) || uslinging() )
		    tmp += dbon();
	}

	if (!thrown && (!Upolyd || !no_obj) && tech_inuse(T_SHIELD_BASH) && uarms && (uarms->spe > -4)) {
		pline("Schrack!");
		tmp += (3 + uarms->spe);
		if (uarms->oartifact == ART_EXCALISHIELD) tmp += 6;
		if (uarms->oartifact == ART_SPECTRATE_ETTECKOR) tmp += rnd(10);
		if (!(PlayerCannotUseSkills)) {
			switch (P_SKILL(P_SHIELD)) {
				case P_BASIC: tmp += 1; break;
				case P_SKILLED: tmp += 2; break;
				case P_EXPERT: tmp += 3; break;
				case P_MASTER: tmp += 4; break;
				case P_GRAND_MASTER: tmp += 5; break;
				case P_SUPREME_MASTER: tmp += 6; break;
			}
		}
	}

	/*
	 * Ki special ability, see cmd.c in function special_ability.
	 * In this case, we do twice damage! Wow!
	 *
	 * Berserk special ability only does +4 damage. - SW
	 */
	/*Lycanthrope claws do +level bare hands dmg
                (multi-hit, stun/freeze)..- WAC*/

	if (!thrown) {

		if (tech_inuse(T_KIII)) tmp *= 2;
		if (u.berserktime) tmp *= 2;
		if (tech_inuse(T_BERSERK)) tmp += 4;
		if (tech_inuse(T_EVISCERATE)) {
			tmp += rnd((int) (u.ulevel/2 + 1)) + (u.ulevel/2); /* [max] was only + u.ulevel */
	                You("slash %s!", mon_nam(mon));
			hittxt = TRUE;
		}
	}

	/* yet another twoweaponing nerf --Amy */
	if (u.twoweap && !thrown && !rn2(3) && tmp > 1) tmp /= 2;

	if (WinceState && tmp > 1) {
		tmp *= 4;
		tmp /= 5;
		if (StrongWinceState) {
			tmp *= 4;
			tmp /= 5;
		}
		if (tmp < 1) tmp = 1;
	}

	/* throwing launchers or ammo (if no suitable launcher is wielded) should still train the skill, but give no bonus */
	if (tmp > 0 && thrown && obj && is_launcher(obj) && !valid_weapon_attack) {
		use_skill(weapon_type(obj), 1);
	}
	if (tmp > 0 && thrown && obj && is_ammo(obj) && (!launcher || !ammo_and_launcher(obj, launcher)) && !valid_weapon_attack) {
		use_skill(weapon_type(obj), 1);
	}

	if (valid_weapon_attack) {
	    struct obj *wep;
	    struct obj *thrwwep = (struct obj *)0;

	    /* to be valid a projectile must have had the correct projector */
	    wep = PROJECTILE(obj) ? launcher : obj;
	    if (!thrown && !launcher) wep = obj; /* bashing with ammo --Amy */

	    if (thrown && !wep) {
		thrwwep = obj; /* throwing ammo or launchers - very ineffective, but trains skill --Amy */
	    }

		/* bashing with launchers or other "bad" weapons shouldn't give insane bonuses --Amy */
		if (wep && !(( (is_launcher(wep) && !(wep->otyp == LASERXBOW && wep->lamplit) && !(wep->otyp == KLIUSLING && wep->lamplit)) || is_missile(wep) || is_ammo(wep) || (is_pole(wep) && !(tech_inuse(T_POLE_MELEE)) && !u.usteed) || (is_lightsaber(wep) && !wep->lamplit) ) && !thrown)) tmp += weapon_dam_bonus(wep);

		if (wep && !thrown && !(( (is_launcher(wep) && !(wep->otyp == LASERXBOW && wep->lamplit) && !(wep->otyp == KLIUSLING && wep->lamplit)) || is_missile(wep) || is_ammo(wep) || (is_pole(wep) && !(tech_inuse(T_POLE_MELEE)) && !u.usteed) || (is_lightsaber(wep) && !wep->lamplit) )) ) tmp += melee_dam_bonus(wep);	/* extra damage bonus added by Amy */

		if (wep && thrown) tmp += ranged_dam_bonus(wep);	/* ditto */

		/* various artifacts and other specific things with *beneficial* effects go here --Amy
		 * negative effects, like collusion knives, should run even if it wasn't a "valid weapon attack" */

		if (obj && obj->oartifact == ART_EMMA_S_SYMPATHY && !mon->mpeaceful && !mon->mtame && !rn2(25)) {
			if (!resist(mon, WEAPON_CLASS, 0, NOTELL)) {
				mon->mpeaceful = TRUE;
				pline("%s becomes peaceful!", Monnam(mon));
				return FALSE;
			}
		}

		if (obj && obj->oartifact == ART_RESOLE_THE_WOMAN_S_ASS) {
			if (mon->data->msound == MS_FART_NORMAL || mon->data->msound == MS_FART_QUIET || mon->data->msound == MS_FART_LOUD) {
				if (mon->butthurt < 19) {
					mon->butthurt++;
					pline("%s's butt is hurt!", Monnam(mon));
				}
				if (mon->butthurt >= 19) {
					pline("%s's butt is severely hurt! You should use the #force to finish %s off!", Monnam(mon), mhim(mon));
				}
			}
		}

		if (obj && obj->oartifact == ART_SHOE_BRAND && mon->data->msound == MS_SHOE) {

			if (!rn2(20)) {
				pline("Shoe Brand tries to tame the shoe...");
				(void) tamedog(mon, (struct obj *)0, FALSE);
				return FALSE;
			} else tmp += rnd(20);
		}

		if (uarmf && uarmf->oartifact == ART_STOLP && !resists_blnd(mon) && mon->mcansee && humanoid(mon->data) && is_female(mon->data)) {
			mon->mcansee = 0;
			mon->mblinded = rnd(10);
			pline("%s is stolped by you!", Monnam(mon));

		}

		if (obj && obj->oartifact == ART_VERSUS_ELECTRICALLY_BASED_ && (dmgtype(mon->data, AD_ELEC) || dmgtype(mon->data, AD_MALK) ) ) {
			tmp += d(3, 10);
		}

		if (!thrown && obj && obj->oartifact == ART_UP_DOWN_STAB && isok(mon->mx, mon->my) && ((levl[mon->mx][mon->my].typ == WOODENTABLE) || (levl[mon->mx][mon->my].typ == POOL) || (levl[mon->mx][mon->my].typ == MOAT) || (levl[mon->mx][mon->my].typ == WATER) ) ) {
			tmp += 20;
		}

		if (thrown && obj && obj->oartifact == ART_MESHERABANE && is_elonamonster(mon->data)) {
			tmp += rnd(40);
		}

		if (!thrown && uwep && uwep->oartifact == ART_TOWEL_OF_THE_INTERSTELLAR_ && obj && objects[obj->otyp].oc_skill == P_WHIP) {
			tmp += 5;
		}

		if (!thrown && obj && obj->oartifact == ART_EVIL_MOTHER_CELINE) {
			if (has_head(mon->data) && !mindless(mon->data) && !mon->egotype_undead && !((mon->misc_worn_check & W_ARMH) && rn2(8)) ) {
				tmp += 10;
			}
		}

		if (ublindf && ublindf->oartifact == ART_IUSTITIA_BRINGER && mon->data->maligntyp < 0) {
			tmp += 10;
		}

		if (obj && obj->oartifact == ART_ORE_DEPOSIT && lithivorous(mon->data)) {
			tmp += rnd(20);
		}

		if (thrown && obj && obj->oartifact == ART_ARROW_OF_SLAY_GOOD && mon->data->maligntyp > 0) {
			tmp += rnd(15);
		}

		if (obj && obj->oartifact == ART_MOLDSWANDIR && mon->data->mlet == S_FUNGUS) {
			tmp += rnd(10);
		}

		if (Role_if(PM_EMERA) && !thrown && mon->female && humanoid(mon->data)) tmp += rnd(u.ulevel);

		if (thrown && obj && obj->oartifact == ART_DARTHADART && obj->spe < 15) {
			obj->spe++;
		}

		if (thrown && obj && obj->oartifact == ART_NEZ_SPECIAL_OFFER) {
			mon->bleedout += rnd(10);
			pline("%s gets a cut.", Monnam(mon));
		}

		if (gunused && tech_inuse(T_SHUT_THAT_BITCH_UP) && mon && mon->female && humanoid(mon->data)) {
			if (!TimeStopped || !rn2(TimeStopped)) {
				You("managed to shut the %s bitch up!", l_monnam(mon));
				TimeStopped++;
			}
		}

		if (gunused && !PlayerCannotUseSkills) { /* firearm boosts by Amy */
			if (launcher && (launcher->otyp == PISTOL || launcher->otyp == HAND_BLASTER || launcher->otyp == FLINTLOCK || launcher->otyp == BEAM_REFLECTOR_GUN || launcher->otyp == AUTO_SHOTGUN)) {
				switch (P_SKILL(P_FIREARM)) {
					case P_BASIC: tmp += rn2(2); break;
					case P_SKILLED: tmp += 1; break;
					case P_EXPERT: tmp += 1; break;
					case P_MASTER: tmp += rnd(2); break;
					case P_GRAND_MASTER: tmp += rnd(2); break;
					case P_SUPREME_MASTER: tmp += rnd(3); break;
					default: break;
				}
				switch (P_SKILL(P_GUN_CONTROL)) {
					case P_BASIC: tmp += rn2(2); break;
					case P_SKILLED: tmp += 1; break;
					case P_EXPERT: tmp += 1; break;
					case P_MASTER: tmp += rnd(2); break;
					case P_GRAND_MASTER: tmp += rnd(2); break;
					case P_SUPREME_MASTER: tmp += rnd(3); break;
					default: break;
				}

			}
			if (launcher && (launcher->otyp == RIFLE || launcher->otyp == HUNTING_RIFLE || launcher->otyp == PROCESS_CARD || launcher->otyp == SHOTGUN || launcher->otyp == PAPER_SHOTGUN || launcher->otyp == CUTTING_LASER)) {
				switch (P_SKILL(P_FIREARM)) {
					case P_BASIC: tmp += 1; break;
					case P_SKILLED: tmp += rnd(2); break;
					case P_EXPERT: tmp += rnd(3); break;
					case P_MASTER: tmp += rnd(4); break;
					case P_GRAND_MASTER: tmp += rnd(5); break;
					case P_SUPREME_MASTER: tmp += rnd(6); break;
					default: break;
				}
				switch (P_SKILL(P_GUN_CONTROL)) {
					case P_BASIC: tmp += 1; break;
					case P_SKILLED: tmp += rnd(2); break;
					case P_EXPERT: tmp += rnd(3); break;
					case P_MASTER: tmp += rnd(4); break;
					case P_GRAND_MASTER: tmp += rnd(5); break;
					case P_SUPREME_MASTER: tmp += rnd(6); break;
					default: break;
				}

			}
			if (launcher && (launcher->otyp == SNIPER_RIFLE || launcher->otyp == SAWED_OFF_SHOTGUN)) {

				switch (P_SKILL(P_FIREARM)) {
					case P_BASIC: tmp += 1; break;
					case P_SKILLED: tmp += rnd(3); break;
					case P_EXPERT: tmp += rnd(4); break;
					case P_MASTER: tmp += rnd(6); break;
					case P_GRAND_MASTER: tmp += rnd(7); break;
					case P_SUPREME_MASTER: tmp += rnd(9); break;
					default: break;
				}
				switch (P_SKILL(P_GUN_CONTROL)) {
					case P_BASIC: tmp += 1; break;
					case P_SKILLED: tmp += rnd(3); break;
					case P_EXPERT: tmp += rnd(4); break;
					case P_MASTER: tmp += rnd(6); break;
					case P_GRAND_MASTER: tmp += rnd(7); break;
					case P_SUPREME_MASTER: tmp += rnd(9); break;
					default: break;
				}

			}
		}
		if (crossbowused && !PlayerCannotUseSkills) { /* crossbow boosts by Amy */
			switch (P_SKILL(P_CROSSBOW)) {
				case P_SKILLED: tmp += 1; break;
				case P_EXPERT: tmp += rnd(2); break;
				case P_MASTER: tmp += rnd(4); break;
				case P_GRAND_MASTER: tmp += rnd(6); break;
				case P_SUPREME_MASTER: tmp += rnd(8); break;
				default: break;
			}

		}

		/* luckless folly: increase damage with low luck, decrease with high luck --Amy */
		if (wep && wep->oartifact == ART_LUCKLESS_FOLLY && Luck < 0) tmp -= Luck;

		if (wep && wep->oartifact == ART_PUCKOCK && thrown && uball && (wep == uball)) tmp += 30;
		if (wep && wep->oartifact == ART_SKULL_SWORD && thrown) tmp += 5;
		if (wep && wep->oartifact == ART_VIOLENT_SKULL_SWORD && thrown) tmp += 10;

		if (wep && wep->oartifact == ART_WU_TSCHI_ && thrown) tmp += 10;
		if (wep && wep->oartifact == ART_STRUCK_ON && thrown) tmp += 10;
		if (wep && wep->oartifact == ART_WUMMP && thrown) tmp += 12;
		if (wep && wep->oartifact == ART_DOHLOW && thrown) tmp += 12;
		if (wep && wep->oartifact == ART_OLYMPIDE && thrown) tmp += 8;
		if (wep && wep->oartifact == ART_GOLDSTRUCK && thrown) tmp += 10;

		if (uwep && uwep->oartifact == ART_HYPER_INTELLIGENCE && wep && (objects[wep->otyp].w_ammotyp == WP_SHELL) ) {
			mightbooststat(A_INT);
		}

		if (uwep && uwep->oartifact == ART_AY_PEE_RIFLE && wep && (objects[wep->otyp].w_ammotyp == WP_BULLET_RIFLE) ) {
			hurtmarmor(mon, AD_CORR);
		}

		if (obj && obj->oartifact == ART_SHREDSHOT && uwep && ammo_and_launcher(obj, uwep)) {

			int astries = 200;
			int diceroll;
			int shredtype;
			struct obj *otmpS;
armorsmashrepeat:

			diceroll = rnd(5);
			shredtype = rnd(2);

			switch (diceroll) {
				case 1:
					if (mon->misc_worn_check & W_ARMS) {
					    otmpS = which_armor(mon, W_ARMS);

					    if (otmpS) {
						if (shredtype == 1 && otmpS->oeroded < MAX_ERODE) otmpS->oeroded++;
						else if (shredtype == 2 && otmpS->oeroded2 < MAX_ERODE) otmpS->oeroded2++;
						else if (otmpS->spe > -20) otmpS->spe--;
					      pline("%s %s is damaged!", s_suffix(Monnam(mon)), distant_name(otmpS, xname));
					    }

					    goto armorsmashdone;
					}
					break;
				case 2:
					if (mon->misc_worn_check & W_ARMG) {
					    otmpS = which_armor(mon, W_ARMG);

					    if (otmpS) {
						if (shredtype == 1 && otmpS->oeroded < MAX_ERODE) otmpS->oeroded++;
						else if (shredtype == 2 && otmpS->oeroded2 < MAX_ERODE) otmpS->oeroded2++;
						else if (otmpS->spe > -20) otmpS->spe--;
					      pline("%s %s is damaged!", s_suffix(Monnam(mon)), distant_name(otmpS, xname));
					    }

					    goto armorsmashdone;
					}
					break;
				case 3:
					if (mon->misc_worn_check & W_ARMF) {
					    otmpS = which_armor(mon, W_ARMF);

					    if (otmpS) {
						if (shredtype == 1 && otmpS->oeroded < MAX_ERODE) otmpS->oeroded++;
						else if (shredtype == 2 && otmpS->oeroded2 < MAX_ERODE) otmpS->oeroded2++;
						else if (otmpS->spe > -20) otmpS->spe--;
					      pline("%s %s is damaged!", s_suffix(Monnam(mon)), distant_name(otmpS, xname));
					    }

					    goto armorsmashdone;
					}
					break;
				case 4:
					if (mon->misc_worn_check & W_ARMH) {
					    otmpS = which_armor(mon, W_ARMH);

					    if (otmpS) {
						if (shredtype == 1 && otmpS->oeroded < MAX_ERODE) otmpS->oeroded++;
						else if (shredtype == 2 && otmpS->oeroded2 < MAX_ERODE) otmpS->oeroded2++;
						else if (otmpS->spe > -20) otmpS->spe--;
					      pline("%s %s is damaged!", s_suffix(Monnam(mon)), distant_name(otmpS, xname));
					    }

					    goto armorsmashdone;
					}
					break;
				case 5:
					if (mon->misc_worn_check & W_ARMC) {
					    otmpS = which_armor(mon, W_ARMC);

					    if (otmpS) {
						if (shredtype == 1 && otmpS->oeroded < MAX_ERODE) otmpS->oeroded++;
						else if (shredtype == 2 && otmpS->oeroded2 < MAX_ERODE) otmpS->oeroded2++;
						else if (otmpS->spe > -20) otmpS->spe--;
					      pline("%s %s is damaged!", s_suffix(Monnam(mon)), distant_name(otmpS, xname));
					    }

					    goto armorsmashdone;
					} else if (mon->misc_worn_check & W_ARM) {
					    otmpS = which_armor(mon, W_ARM);

					    if (otmpS) {
						if (shredtype == 1 && otmpS->oeroded < MAX_ERODE) otmpS->oeroded++;
						else if (shredtype == 2 && otmpS->oeroded2 < MAX_ERODE) otmpS->oeroded2++;
						else if (otmpS->spe > -20) otmpS->spe--;
					      pline("%s %s is damaged!", s_suffix(Monnam(mon)), distant_name(otmpS, xname));
					    }

					    goto armorsmashdone;
					} else if (mon->misc_worn_check & W_ARMU) {
					    otmpS = which_armor(mon, W_ARMU);

					    if (otmpS) {
						if (shredtype == 1 && otmpS->oeroded < MAX_ERODE) otmpS->oeroded++;
						else if (shredtype == 2 && otmpS->oeroded2 < MAX_ERODE) otmpS->oeroded2++;
						else if (otmpS->spe > -20) otmpS->spe--;
					      pline("%s %s is damaged!", s_suffix(Monnam(mon)), distant_name(otmpS, xname));
					    }

					    goto armorsmashdone;
					}
					break;
			}

			if (astries > 0) {
				astries--;
				goto armorsmashrepeat;
			} else pline("%s doesn't seem to be wearing any armor that can be destroyed...", Monnam(mon));

		}

armorsmashdone:

		if (wep && wep->otyp == JUMPING_FLAMER) {
			if (!rn2(33)) (burnarmor(mon));
			if (!rn2(33)) (void)destroy_mitem(mon, POTION_CLASS, AD_FIRE);
			if (!rn2(33)) (void)destroy_mitem(mon, SCROLL_CLASS, AD_FIRE);
			if (!rn2(50)) (void)destroy_mitem(mon, SPBOOK_CLASS, AD_FIRE);
			if (thrown && wep->oartifact == ART_ORE_EVIL_WIGHT && mon->mcanmove) {
				mon->mfrozen = 3;
				mon->mcanmove = 0;
				mon->mstrategy &= ~STRAT_WAITFORU;
			}
		}

		if (wep && wep->oartifact == ART_MAGISTUS && thrown) {
			if (mon->mhpmax > 1) {
				mon->mhpmax--;
				if (mon->mhp > mon->mhpmax) mon->mhp = mon->mhpmax;
			}
		}
		if (wep && wep->oartifact == ART_TRISMAGISTUS && thrown) {
			if (mon->mhpmax > 1) {
				mon->mhpmax -= 3;
				if (mon->mhpmax < 1) mon->mhpmax = 1;
				if (mon->mhp > mon->mhpmax) mon->mhp = mon->mhpmax;
			}
		}
		if (wep && wep->oartifact == ART_VIOLENT_SKULL_SWORD && thrown) {
			if (!resists_drli(mon)) {
				pline("%s suddenly seems weaker!", mon_nam(mon));
				mon->mhpmax -= rnd(8);
				if (mon->mhp > mon->mhpmax) mon->mhp = mon->mhpmax;
				if (mon->mhp <= 0 || !mon->m_lev) {
					pline("%s dies!", Monnam(mon));
					xkilled(mon,0);
					return FALSE;
				} else {
					mon->m_lev--;
				}
				if (!rn2(5) && wep->spe > -20) wep->spe--;
			}
		}

		if (wep && wep->oartifact == ART_THIS_TRENCH_WAR_HAS_BEEN_F) {
			if (mon->mhp < (mon->mhpmax / 10)) {
				mon->mhp = -1;
				pline("%s is instantly killed!", Monnam(mon));
				xkilled(mon,0);
				return FALSE;
			} else if (mon->mhp < (mon->mhpmax / 3)) {
				if (mon->mhp > 1) mon->mhp /= 2;
				pline("%s is greatly weakened!", Monnam(mon));
			}
		}

		if (wep && wep->oartifact == ART_NO_PRISONERS_TAKEN) {
			if (mon->mhp < (mon->mhpmax * 3 / 4)) tmp += 6;
			if (mon->mhp < (mon->mhpmax / 2)) tmp += 6;
			if (mon->mhp < (mon->mhpmax / 4)) tmp += 6;
		}

		if (tech_inuse(T_DOCKLOCK) && wep && wep->otyp == SEXY_LEATHER_PUMP && !thrown && has_head(mon->data)) {
			tmp += rn1(5, 5);
			pline(!rn2(2) ? "Dock!" : "Klock!");

			if (!rn2(10) && mon->mcanmove) {
				mon->mfrozen = rn1(3, 3);
				mon->mcanmove = 0;
				mon->mstrategy &= ~STRAT_WAITFORU;
				pline("%s can't fight back!", Monnam(mon));
			}

		}

		if (wep && wep->oartifact == ART_HACKNSLASH) {
			mon->bleedout += 5;
			pline("%s is bleeding!", Monnam(mon));
		}
		if (wep && wep->oartifact == ART_BLOODLETTER) {
			mon->bleedout += rnd(12);
			pline("%s is bleeding!", Monnam(mon));
		}
		if (wep && wep->oartifact == ART_REALLY_WANNA_SEE_BLOOD_) {
			mon->bleedout += rn1(11,11);
			pline("%s is bleeding!", Monnam(mon));
		}

		if (wep && wep->oartifact == ART_KATI_GAVE_YOU_THE_ENGLISH_) {
			hurtmarmor(mon, AD_DCAY);
		}

		if (wep && wep->oartifact == ART_STAFF_OF_ROT) {
			hurtmarmor(mon, AD_DCAY);
		}

		if (wep && wep->oartifact == ART_TONA_S_GAMES) {
			hurtmarmor(mon, AD_DCAY);
		}

		if (wep && wep->oartifact == ART_LARISSA_S_REVENGE) {
			hurtmarmor(mon, AD_DCAY);
		}

		if (wep && wep->oartifact == ART_DROWSING_ROD && !rn2(3)) {
			if (!resist(mon, WEAPON_CLASS, 0, NOTELL)) {
				int parlyzdur = rnd(5);

				if (!mon->msleeping && sleep_monst(mon, parlyzdur, -1)) {
					pline("%s is put to sleep by you!", Monnam(mon));
					slept_monst(mon);
				}

			}

		}

		if (wep && wep->oartifact == ART_PRICKTRICK) {
			mon->bleedout += 6;
			pline("%s is bleeding!", Monnam(mon));
		}

		if (wep && wep->oartifact == ART_NAIL_IMPACT) {
			mon->bleedout += 10;
			pline("%s is bleeding!", Monnam(mon));
		}

		if (wep && wep->oartifact == ART_UT_RULER) {
			mon->bleedout += rnd(10);
			pline("%s is bleeding!", Monnam(mon));
		}

		if (wep && wep->oartifact == ART_VORPAL_RULER) {
			mon->bleedout += rnd(10);
			pline("%s is bleeding!", Monnam(mon));
		}

		if (wep && wep->oartifact == ART_CUTNERVE && thrown) {
			mon->bleedout += rnd(6);
			pline("%s is bleeding!", Monnam(mon));
		}

		if (wep && wep->oartifact == ART_KRURUINK && !thrown) {
			if (HFast < 2) HFast = 2;
		}

		if (wep && wep->oartifact == ART_GAE_BUIDHE) {
			mon->bleedout += rnd(10);
			pline("%s is bleeding!", Monnam(mon));
		}

		if (wep && wep->oartifact == ART_SCHWIUQUIEK) {
			mon->bleedout += 8;
			pline("%s is bleeding!", Monnam(mon));
		}
		if (wep && wep->oartifact == ART_MEAT_SCRAPER) {
			mon->bleedout += 6;
			pline("%s is bleeding!", Monnam(mon));
		}
		if (wep && wep->oartifact == ART_LONG_STRALE) {
			mon->bleedout += 7;
			pline("%s is bleeding!", Monnam(mon));
		}

		if (wep && wep->oartifact == ART_CRABBOMAT) {
			mon->bleedout += rnd(15);
			pline("%s is bleeding!", Monnam(mon));
		}

		if (wep && wep->oartifact == ART_INCORRECT_HEALING_ADJUST && !rn2(10)) {
			mon->mhp += 50;
			if (mon->mhp > mon->mhpmax) mon->mhp = mon->mhpmax;
			healup(50, 0, FALSE, FALSE);
			pline("Dingdoeng! Both you and %s were healed.", mon_nam(mon));
		}

		if (wep && wep->oartifact == ART_AINTGETIN && !rn2(3)) {

			if (!resist(mon, WEAPON_CLASS, 0, NOTELL)) {

				if (u_teleport_mon(mon, FALSE)) pline("%s is teleported away!", Monnam(mon));
			}

		}

		if (!thrown && wep && wep->oartifact == ART_SWORDBREAKER) {

			if ( (monwep = MON_WEP(mon)) != 0) {
				monwep = MON_WEP(mon);

				if (monwep && (monwep->spe > -10) && !obj_resists(monwep, 0, 95) ) {
					monwep->spe--;
					pline("%s's weapon degrades.", Monnam(mon));
				}
			}
		}

		if (wep && wep->oartifact == ART_SVEN_S_GARBAGE_BOOSTER) {
			mon->bleedout += rnd(10);
			pline("%s is bleeding!", Monnam(mon));
		}

		if (wep && wep->oartifact == ART_ILJA_S_ASSHOLERY && has_head(mon->data) && !(mon->misc_worn_check & W_ARMH) && !thrown) {
			mon->bleedout += rnd(10);
			pline("%s is bleeding!", Monnam(mon));
		}

		if (wep && wep->oartifact == ART_RED_TREAD && !thrown) {
			mon->bleedout += rnd(17);
			pline("%s is bleeding!", Monnam(mon));
		}

		if (wep && wep->oartifact == ART_ONE_CATHLETTE && !thrown && !rn2(200)) goodeffect();

		if (wep && wep->oartifact == ART_ONE_CATHLETTE && !thrown && !rn2(20)) {
			if (!resist(mon, WEAPON_CLASS, 0, NOTELL)) {
				mon->mconf = TRUE;
				pline("%s is pained!", Monnam(mon));
			}
		}

		if (wep && wep->oartifact == ART_DUEUEUEUEU && !thrown && mon->mcanmove && !rn2(10)) {
			if (!resist(mon, WEAPON_CLASS, 0, NOTELL)) {
				mon->mfrozen = rn1(2,2);
				mon->mcanmove = 0;
				mon->mstrategy &= ~STRAT_WAITFORU;
				pline("%s drops helplessly to the floor!", Monnam(mon));
			}
		}

		if (wep && wep->oartifact == ART_HAMSTRUNG_FOUR_SURE && mon->mcanmove && !rn2(3)) {
			mon->mfrozen = 2;
			mon->mcanmove = 0;
			mon->mstrategy &= ~STRAT_WAITFORU;
			pline("%s is unable to move!", Monnam(mon));
		}

		if (wep && wep->otyp == SPRAY_BOX && !rn2(3) && mon->mcansee && !resists_blnd(mon)) {
			if (!resist(mon, WEAPON_CLASS, 0, NOTELL)) {
				mon->mcansee = 0;
				mon->mblinded = rnd(10);
				pline("%s is blinded by your spray!", Monnam(mon));
			}
		}
		if (wep && wep->oartifact == ART_YOU_LIL_PUSSY && mon->mcanmove && !rn2(3)) {
			if (!resist(mon, WEAPON_CLASS, 0, NOTELL)) {
				mon->mfrozen = rnd(10);
				mon->mcanmove = 0;
				mon->mstrategy &= ~STRAT_WAITFORU;
				pline("%s is maced!", Monnam(mon));
			}
		}

		if (wep && wep->oartifact == ART_BOHEM_FUELKANAL && !rn2(100)) {
			if (!resist(mon, WEAPON_CLASS, 0, NOTELL)) mon_adjust_speed(mon, -1, (struct obj *)0);
		}

		if (wep && wep->oartifact == ART_MELATED_METAL && !thrown && wep->spe > -20 && !rn2(1000) && !CannotSelectItemsInPrompts) {
			register struct obj *melating;
			pline("You may change the material of a worn armor piece to copper.");
melatechoice:
			melating = getobj(allnoncount, "magically enchant");
			if(!melating) {
				if (yn("Really exit with no object selected? Doing so will disenchant your weapon!") == 'y') {
					if (wep && wep->spe > -20) wep->spe--;
					pline("Well, your fault, now your weapon was disenchanted.");
				}
				else goto melatechoice;
			} else if (melating && !(melating->owornmask & W_ARMOR) ) {
				if (wep && wep->spe > -20) wep->spe--;
				pline("You didn't select a worn armor piece, and therefore your weapon was disenchanted.");
			} else if (melating && objects[(melating)->otyp].oc_material == MT_COPPER) {
				if (wep && wep->spe > -20) wep->spe--;
				pline("You selected an armor piece that was already made of copper, and therefore your weapon was disenchanted.");
			} else if (melating) {
				objects[(melating)->otyp].oc_material = MT_COPPER;
				pline_The("armor piece is made of copper now.");
			}

		}

		if (thrown && obj && obj->oartifact == ART_RAZORSHARD) {
			mon->bleedout += 10;
			pline("%s is bleeding!", Monnam(mon));
		}

		if (thrown && obj && obj->oartifact == ART_FOMMP && uwep && uwep->otyp == PISTOL_PAIR) {
			if (!rn2(20)) {
				pline("Fommp!");
				goodeffect();
			}
		}

		if (obj && obj->oartifact == ART_DONK_O_BONK) {
			reducesanity(1);
			decontaminate(1);
		}

		if (thrown && obj && obj->oartifact == ART_BLINKBLINKBLINK__BLINKBLIN) {
			incr_itimeout(&HInvis, 50);
		}

		if (thrown && obj && obj->oartifact == ART_FJELL_BARB) {
			if (isok(mon->mx, mon->my)) levl[mon->mx][mon->my].lit = TRUE;
		}

		if (obj && obj->oartifact == ART_NUKE_OPTION) {
			ragnarok(FALSE);
			if (evilfriday && u.ulevel > 1) evilragnarok(FALSE,u.ulevel);
		}

		if (thrown && obj && obj->oartifact == ART_MAP_END) {
			register int mapendx, mapendy;
			int dirx, diry;
			mapendx = rnd(COLNO-1);
			mapendy = rn2(ROWNO);
			dirx = rn2(3) - 1;
			diry = rn2(3) - 1;
			if(dirx != 0 || diry != 0)
				buzz(-11, 8, mapendx, mapendy, dirx, diry);
		}

		if (thrown && obj && (obj->oartifact == ART_SELF_SUFFICIENCE)) {
			if (obj->cursed) uncurse(obj, TRUE);
			if (obj->spe < 0) obj->spe++;
		}

		if (thrown && obj && (obj->oartifact == ART_TURN_INTO_ASH_HEAP)) {
			(void) cancel_monst(mon, obj, TRUE, TRUE, FALSE);
		}

		if (thrown && obj && (obj->oartifact == ART_PARTIAL_MANTLE_PROJECTILE)) {
			if (mon->mhp > 9) mon->mhp -= (mon->mhp / 10);
		}

		if (thrown && obj && (obj->oartifact == ART_DAMN_I_M_GOOD)) {
			if (mon->mspeed != MSLOW && !resist(mon, WEAPON_CLASS, 0, NOTELL)) {
			    unsigned int oldspeed = mon->mspeed;
	
			    mon_adjust_speed(mon, -1, (struct obj *)0);
			    if (mon->mspeed != oldspeed && canseemon(mon))
				pline("%s slows down.", Monnam(mon));
			}

		}

		if (thrown && obj && (obj->oartifact == ART_HEAVYDRAIN)) {
			if (mon->mhpmax > 1) mon->mhpmax--;
			if (mon->mhpmax < mon->mhp) mon->mhp = mon->mhpmax;
			pline("%s's health is drained!", Monnam(mon));
		}


		if (thrown && obj && (obj->oartifact == ART_MINDTURNER) && !rn2(20)) {
			if (mon->female) mon->female = 0;
			else mon->female = 1;
			pline("%s undergoes gender surgery!", Monnam(mon));
		}

		if (obj && obj->oartifact == ART_CHRISMISS && !PlayerCannotUseSkills) {
			switch (P_SKILL(P_VAAPAD)) {
				case P_BASIC: tmp += 4; break;
				case P_SKILLED: tmp += 8; break;
				case P_EXPERT: tmp += 12; break;
				case P_MASTER: tmp += 16; break;
				case P_GRAND_MASTER: tmp += 20; break;
				case P_SUPREME_MASTER: tmp += 24; break;
				default: break;
			}

		}

		if (thrown && obj && (obj->oartifact == ART_PAUERED_BY_THE_CAP)) {

			int paueredskill = objects[obj->otyp].oc_skill;
			if (paueredskill < 0) paueredskill = -(paueredskill);

			if (P_MAX_SKILL(paueredskill) >= P_BASIC) tmp += 5;
			if (P_MAX_SKILL(paueredskill) >= P_SKILLED) tmp += 5;
			if (P_MAX_SKILL(paueredskill) >= P_EXPERT) tmp += 5;
			if (P_MAX_SKILL(paueredskill) >= P_MASTER) tmp += 5;
			if (P_MAX_SKILL(paueredskill) >= P_GRAND_MASTER) tmp += 5;
			if (P_MAX_SKILL(paueredskill) >= P_SUPREME_MASTER) tmp += 5;

			if (!rn2(10) && !P_RESTRICTED(paueredskill) ) {
				int tryct;
				int i = 0;

				if (P_MAX_SKILL(paueredskill) == P_BASIC) {
					P_MAX_SKILL(paueredskill) = P_ISRESTRICTED;
					pline("You lose all knowledge of the %s skill!", wpskillname(paueredskill));
					P_ADVANCE(paueredskill) = 0;
				} else if (P_MAX_SKILL(paueredskill) == P_SKILLED) {
					P_MAX_SKILL(paueredskill) = P_BASIC;
					pline("You lose some knowledge of the %s skill!", wpskillname(paueredskill));
				} else if (P_MAX_SKILL(paueredskill) == P_EXPERT) {
					P_MAX_SKILL(paueredskill) = P_SKILLED;
					pline("You lose some knowledge of the %s skill!", wpskillname(paueredskill));
				} else if (P_MAX_SKILL(paueredskill) == P_MASTER) {
					P_MAX_SKILL(paueredskill) = P_EXPERT;
					pline("You lose some knowledge of the %s skill!", wpskillname(paueredskill));
				} else if (P_MAX_SKILL(paueredskill) == P_GRAND_MASTER) {
					P_MAX_SKILL(paueredskill) = P_MASTER;
					pline("You lose some knowledge of the %s skill!", wpskillname(paueredskill));
				} else if (P_MAX_SKILL(paueredskill) == P_SUPREME_MASTER) {
					P_MAX_SKILL(paueredskill) = P_GRAND_MASTER;
					pline("You lose some knowledge of the %s skill!", wpskillname(paueredskill));
				}

				skill_sanity_check(paueredskill);

			}
		}

		if (thrown && obj && (obj->oartifact == ART_CHANGE_THE_PLAY)) {
			if (P_RESTRICTED(P_DART) && !P_RESTRICTED(P_SHURIKEN)) {
				int changetheplaycap = 20;
				if (P_MAX_SKILL(P_SHURIKEN) == P_SKILLED) changetheplaycap = 160;
				if (P_MAX_SKILL(P_SHURIKEN) == P_EXPERT) changetheplaycap = 540;
				if (P_MAX_SKILL(P_SHURIKEN) == P_MASTER) changetheplaycap = 1280;
				if (P_MAX_SKILL(P_SHURIKEN) == P_GRAND_MASTER) changetheplaycap = 2500;
				if (P_MAX_SKILL(P_SHURIKEN) == P_SUPREME_MASTER) changetheplaycap = 4320;

				if (P_ADVANCE(P_DART) >= changetheplaycap) {
					skillcaploss_specific(P_SHURIKEN);
					unrestrict_weapon_skill(P_DART);
					P_ADVANCE(P_DART) = 0;
					P_SKILL(P_DART) = P_UNSKILLED;
					if (changetheplaycap == 4320) P_MAX_SKILL(P_DART) = P_SUPREME_MASTER;
					else if (changetheplaycap == 2500) P_MAX_SKILL(P_DART) = P_GRAND_MASTER;
					else if (changetheplaycap == 1280) P_MAX_SKILL(P_DART) = P_MASTER;
					else if (changetheplaycap == 540) P_MAX_SKILL(P_DART) = P_EXPERT;
					else if (changetheplaycap == 160) P_MAX_SKILL(P_DART) = P_SKILLED;
					else P_MAX_SKILL(P_DART) = P_BASIC;
					You("lose all knowledge of the shuriken skill, and learn the dart skill instead!");
				}
			}
		}

		if (wep && wep->oartifact == ART_ENCHANTEASY && !rn2(1000) && wep->spe < 7) {
			wep->spe++;
			Your("weapon was enchanted!");
		}

		if (thrown && obj && (obj->oartifact == ART_BECOME_NORMAL) ) {
			struct obj *uammo;
			uammo = mksobj(GRAPHITE, TRUE, 2, FALSE);
			if (uammo) {
				uammo->quan = 1;
				uammo->owt = weight(uammo);
				dropy(uammo);
				stackobj(uammo);
			}
		}

		if (thrown && obj && (obj->oartifact == ART_TRACKSTOP) && mon->mcanmove ) {
			mon->mfrozen = 2;
			mon->mcanmove = 0;
			mon->mstrategy &= ~STRAT_WAITFORU;
			pline("%s is stopped in %s tracks!", Monnam(mon), mhis(mon));
		}

		if (thrown && launcher && obj && (obj->oartifact == ART_MORETRAIN) ) {
			use_skill(P_FIREARM, 5);
			use_skill(P_GUN_CONTROL, 1);
		}

		if (thrown && obj && (obj->oartifact == ART_STELSHOT) ) {
			incr_itimeout(&HStealth, 200);
		}

		if (wep && wep->oartifact == ART_MARTHA_S_FOREIGN_GOER) {
			mon->bleedout += rnd(10);
			pline("%s is bleeding!", Monnam(mon));
		}

		if (wep && wep->oartifact == ART_PRICK_BEARER_S_RANSOM) {
			mon->bleedout += rnd(10);
			pline("%s is bleeding!", Monnam(mon));
		}

		if (wep && wep->otyp == ARCANE_HORN && !thrown) {
			if (use_unicorn_horn(wep)) { /* wep has now been REMOVED - make sure we don't segfault! --Amy */
				nomul(0, 0, FALSE);
				return FALSE;
			}
		}

		if (tech_inuse(T_JOKERBANE) && mon->mcanmove && wep && thrown && (weapon_type(wep) == P_BOOMERANG || weapon_type(wep) == -P_BOOMERANG) ) {
			if (!resist(mon, WEAPON_CLASS, 0, NOTELL)) {
				mon->mfrozen = 2 + rnd(5);
				mon->mcanmove = 0;
				mon->mstrategy &= ~STRAT_WAITFORU;
				pline("%s is paralyzed!", Monnam(mon));
			} else {
				mon->mfrozen = 2;
				mon->mcanmove = 0;
				mon->mstrategy &= ~STRAT_WAITFORU;
				pline("%s pauses momentarily!", Monnam(mon));
			}

		}

	    /* [this assumes that `!thrown' implies wielded...] */
	    wtype = weapon_type(wep);
	    if (thrwwep) wtype = weapon_type(thrwwep);

	    /* dark lightsaber uses energy slowly while lit but faster if you hit with it --Amy */
	    if (wep && (wep->otyp == DARK_LIGHTSABER || wep->oartifact == ART_CHRIS_S_TWIT_APOSTROPH || wep->oartifact == ART_BURN_FASTER) && wep->lamplit) {
		wep->age -= 1;
		if (wep->age < 0) wep->age = 0;
	    }
	    if (wep && wep->oartifact == ART_BURN_FASTER && wep->lamplit) {
		wep->age -= 4;
		if (wep->age < 0) wep->age = 0;
		u.uhunger -= 5;
		newuhs(TRUE);
	    }

	    if (!(mon->egotype_flickerer) && !noeffect && !(mon->data == &mons[PM_LITTLE_POISON_IVY] || mon->data == &mons[PM_CRITICALLY_INJURED_PERCENTS] || mon->data == &mons[PM_SUPERDEEP_TYPE] || mon->data == &mons[PM_AGULA] || mon->data == &mons[PM_DTTN_ERROR] || mon->data == &mons[PM_FLUIDATOR_IVE] || mon->data == &mons[PM_MISTER_GRIBBS] || mon->data == &mons[PM_AMBER_FEMMY] || mon->data == &mons[PM_IMMUNITY_VIRUS] || mon->data == &mons[PM_UNGENOCIDABLE_VAMPSHIFTER] || mon->data == &mons[PM_TERRIFYING_POISON_IVY] || mon->data == &mons[PM_GIRL_WITH_THE_MOST_BEAUTIFUL_SHOES_IN_THE_WORLD] || mon->data == &mons[PM_IMMOVABLE_OBSTACLE] || mon->data == &mons[PM_INVINCIBLE_HAEN] || mon->data == &mons[PM_CHAREY] || mon->data == &mons[PM_INVENTOR_OF_THE_SISTER_COMBAT_BOOTS] || mon->data == &mons[PM_SWEET_ASIAN_POISON_IVY] || mon->data == &mons[PM_ARABELLA_SHOE] || mon->data == &mons[PM_ANASTASIA_SHOE] || mon->data == &mons[PM_HENRIETTA_SHOE] || mon->data == &mons[PM_KATRIN_SHOE] || mon->data == &mons[PM_JANA_SHOE] || mon->data == &mons[PM_FIRST_DUNVEGAN] || mon->data == &mons[PM_PERCENTI_HAS_LOST___] || mon->data == &mons[PM_PERCENTI_IS_IMMUNE_TO_THE_ATTACK_]) ) {
		    if (thrown || !u.twoweap || !rn2(2)) {
			use_skill(wtype, 1);
		    }
		    else if (u.twoweap) use_skill(P_TWO_WEAPON_COMBAT,1);

		    if (!thrown) { /* general combat skill is trained by using melee weapons --Amy */
				u.ugeneralcombatturns++;
				if (u.ugeneralcombatturns >= 10) {
					u.ugeneralcombatturns = 0;
					use_skill(P_GENERAL_COMBAT, 1);
				}

				if (uwep && uwep->oartifact == ART_ATARU_ONE && u.twoweap && uswapwep && uswapwep->oartifact == ART_ATARU_TWO) {
					u.uataruturns++;
					if (u.uataruturns >= 4) {
						u.uataruturns = 0;
						use_skill(P_ATARU, 1);
					}
				}

				if (uwep && uwep->oartifact == ART_ATARU_TWO && u.twoweap && uswapwep && uswapwep->oartifact == ART_ATARU_ONE) {
					u.uataruturns++;
					if (u.uataruturns >= 4) {
						u.uataruturns = 0;
						use_skill(P_ATARU, 1);
					}
				}

				if (uwep && uwep->oartifact == ART_THIS_IS_VAAPAD) {
						u.uvaapadturns++;
						if (u.uvaapadturns >= 4) {
							u.uvaapadturns = 0;
							use_skill(P_VAAPAD, 1);
						}
				}

				if (uwep && uwep->oartifact == ART_CONCENTRATOR) {
					if (!u.twoweap || !rn2(2)) {
						u.ushiichoturns++;
						if (u.ushiichoturns >= 5) {
							u.ushiichoturns = 0;
							use_skill(P_SHII_CHO, 1);
						}
					}
				}

				if (!uarms && !u.twoweap && uwep && uwep->oartifact == ART_MA_STRIKE) {
					u.umakashiturns++;
					if (u.umakashiturns >= 4) {
						u.umakashiturns = 0;
						use_skill(P_MAKASHI, 1);
					}
				}

				if (uwep && is_lightsaber(uwep) && (uwep->lamplit || Role_if(PM_SHADOW_JEDI)) ) {

					if (!u.twoweap || !rn2(2)) {
						u.ushiichoturns++;
						if (u.ushiichoturns >= 5) {
							u.ushiichoturns = 0;
							use_skill(P_SHII_CHO, 1);
						}
					}

					if (!uarms && !u.twoweap && uwep && !bimanual(uwep)) {
						u.umakashiturns++;
						if (u.umakashiturns >= 4) {
							u.umakashiturns = 0;
							use_skill(P_MAKASHI, 1);
						}
					}

					if (u.twoweap && uswapwep && is_lightsaber(uswapwep) && uswapwep->lamplit) {
						u.uataruturns++;
						if (u.uataruturns >= 4) {
							u.uataruturns = 0;
							use_skill(P_ATARU, 1);
						}
					}

					if (uwep && bimanual(uwep) && uwep->altmode) {
						u.uvaapadturns++;
						if (u.uvaapadturns >= 4) {
							u.uvaapadturns = 0;
							use_skill(P_VAAPAD, 1);
						}
					}

				} /* end lightsaber-specific code */

				/* For some reason, "wep" isn't always defined, yet the checks above don't crash... --Amy */
				if (wep && !uarms && !is_missile(wep) && !is_ammo(wep) && !(is_launcher(wep) && !(wep->otyp == LASERXBOW && wep->lamplit) && !(wep->otyp == KLIUSLING && wep->lamplit)) && !(is_pole(wep) && !(tech_inuse(T_POLE_MELEE)) && !u.usteed) && bimanual(wep)) {
					u.utwohandedcombatturns++;
					if (u.utwohandedcombatturns >= 3) {
						u.utwohandedcombatturns = 0;
						use_skill(P_TWO_HANDED_WEAPON, 1);
					}
				}

			}

		/* but general combat was training so slowly... ranged attacks may also train it --Amy */
		    if (thrown) {
				u.ugeneralcombatturns++;
				if (u.ugeneralcombatturns >= 10) {
					u.ugeneralcombatturns = 0;
					use_skill(P_GENERAL_COMBAT, 1);
				}

				u.umissileweaponturns++;
				if (u.umissileweaponturns >= 3) {
					u.umissileweaponturns = 0;
					use_skill(P_MISSILE_WEAPONS, 1);
				}

				if (obj && is_bullet(obj)) {
					u.uguncontrolturns++;
					if (u.uguncontrolturns >= 2) {
						u.uguncontrolturns = 0;
						use_skill(P_GUN_CONTROL, 1);
					}
				}
				/* djem so was also training ultra slowly, so here's a multiplier */
				if (wep && is_lightsaber(wep) && (wep->lamplit || Role_if(PM_SHADOW_JEDI)) && obj && (wep == obj)) {
					use_skill(P_DJEM_SO, rnd(4));
					if (wep->otyp == PINK_LIGHTSWORD || wep->otyp == PINK_DOUBLE_LIGHTSWORD) use_skill(P_DJEM_SO, rnd(4));
					if (wep->oartifact == ART_ROSH_TRAINOR) use_skill(P_DJEM_SO, rnd(4));
					mightbooststat(A_DEX);
				}

				if (wep && wep->oartifact == ART_RUSMA_SRO && obj && (wep == obj)) {
					use_skill(P_DJEM_SO, rnd(4));
					if (wep->otyp == PINK_LIGHTSWORD || wep->otyp == PINK_DOUBLE_LIGHTSWORD) use_skill(P_DJEM_SO, rnd(4));
					if (wep->oartifact == ART_ROSH_TRAINOR) use_skill(P_DJEM_SO, rnd(4));
					mightbooststat(A_DEX);
				}

				if (uwep && uwep->oartifact == ART_DJARWETHEREYET && uwep->lamplit && obj && objects[obj->otyp].oc_skill == -P_CROSSBOW) {
					use_skill(P_DJEM_SO, 1);
					if (uwep->altmode) use_skill(P_DJEM_SO, 1);

					if (obj->otyp == PINK_LIGHTSWORD || obj->otyp == PINK_DOUBLE_LIGHTSWORD) 
{
						use_skill(P_DJEM_SO, 1);
						if (uwep->altmode) use_skill(P_DJEM_SO, 1);
					}
					if (obj->oartifact == ART_ROSH_TRAINOR) {
						use_skill(P_DJEM_SO, 1);
						if (uwep->altmode) use_skill(P_DJEM_SO, 1);
					}

				}

				if (!SkillTrainingImpossible) {
					if (uwep && uwep->otyp == KLIUSLING && uwep->lamplit && obj && objects[obj->otyp].oc_skill == -P_SLING) {
						u.kliuturns++;
						if (u.kliuturns >= 5) {
							u.kliuturns = 0;
							u.kliuskill++;
							if (u.kliuskill == 20) You("are now more skilled in form X (Kliu).");
							if (u.kliuskill == 160) You("are now more skilled in form X (Kliu).");
							if (u.kliuskill == 540) You("are now more skilled in form X (Kliu).");
							if (u.kliuskill == 1280) You("are now more skilled in form X (Kliu).");
							if (u.kliuskill == 2560) You("are now more skilled in form X (Kliu).");
							if (u.kliuskill == 4320) You("are now most skilled in form X (Kliu).");
						}
					}
				}

			}

	    }
	}

	if (ispoisoned) {
	    int nopoison = (10/* - (obj->owt/10)*/);
	    if (Race_if(PM_IRAHA)) nopoison *= 10;
	    if(nopoison < 2) nopoison = 2;

	    if (uwep && uwep->oartifact == ART_SCHOSCHO_BARBITUER) {
		adjalign(-1);
	    }
		/* no message because it would get spammy, also it's your fault if you don't read the item description --Amy */
	    if (u.twoweap && uswapwep && uswapwep->oartifact == ART_SCHOSCHO_BARBITUER) {
		adjalign(-1);
	    }

	    if (Role_if(PM_SAMURAI) && !Race_if(PM_IRAHA) && !Race_if(PM_POISONER)) {
		You("dishonorably use a poisoned weapon!");
		adjalign(-sgn(u.ualign.type));
		adjalign(-5);
		increasesincounter(1);
		u.alignlim--;
	    }
	    if (uwep && uwep->otyp == HONOR_KATANA && !Race_if(PM_IRAHA) && !Race_if(PM_POISONER)) {
		You("dishonorably use a poisoned weapon!");
		adjalign(-sgn(u.ualign.type));
		adjalign(-5);
		increasesincounter(1);
		u.alignlim--;
	    }
	    if (u.twoweap && uswapwep && uswapwep->otyp == HONOR_KATANA && !Race_if(PM_IRAHA) && !Race_if(PM_POISONER)) {
		You("dishonorably use a poisoned weapon!");
		adjalign(-sgn(u.ualign.type));
		adjalign(-5);
		increasesincounter(1);
		u.alignlim--;
	    }
	    if (uwep && uwep->oartifact == ART_JAPANESE_WOMEN && !Race_if(PM_IRAHA) && !Race_if(PM_POISONER)) {
		You("dishonorably use a poisoned weapon!");
		adjalign(-1);
		adjalign(-5);
		increasesincounter(1);
		u.alignlim--;
	    }
	    if (u.twoweap && uswapwep && uswapwep->oartifact == ART_JAPANESE_WOMEN && !Race_if(PM_IRAHA) && !Race_if(PM_POISONER)) {
		You("dishonorably use a poisoned weapon!");
		adjalign(-1);
		adjalign(-5);
		increasesincounter(1);
		u.alignlim--;
	    }
	    if (uwep && uwep->oartifact == ART_SAKUSHNIR && !Race_if(PM_IRAHA) && !Race_if(PM_POISONER)) {
		You("dishonorably use a poisoned weapon!");
		adjalign(-1);
		adjalign(-5);
		increasesincounter(1);
		u.alignlim--;
	    }
	    if (u.twoweap && uswapwep && uswapwep->oartifact == ART_SAKUSHNIR && !Race_if(PM_IRAHA) && !Race_if(PM_POISONER)) {
		You("dishonorably use a poisoned weapon!");
		adjalign(-1);
		adjalign(-5);
		increasesincounter(1);
		u.alignlim--;
	    }
	    if ((u.ualign.type == A_LAWFUL) && !Race_if(PM_POISONER) && !Race_if(PM_IRAHA)) {
		You_feel("like an evil coward for using a poisoned weapon.");
		adjalign(-5);
	    }
	    if (obj && obj->opoisoned && !rn2(nopoison) && (!obj->superpoison || !rn2(10)) && !stack_too_big(obj) ) {
		obj->opoisoned = FALSE;
		obj->superpoison = FALSE;
		Your("%s %s no longer poisoned.", xname(obj),
		     otense(obj, "are"));
	    }
	    if (resists_poison(mon))
		needpoismsg = TRUE;
	    else if (rn2(100))
		tmp += rnd(6);
	    else poiskilled = TRUE;
	}
	  
	if (tmp < 1) { /* if (tmp < 0), but it is possible to deal no damage (not a bug) */
	    /* make sure that negative damage adjustment can't result
	       in inadvertently boosting the victim's hit points */
	    tmp = 0;
	    if (is_shade(mdat) || mon->egotype_shader) {
		if (!hittxt) {
		    const char *what = unconventional[0] ? unconventional : "attack";
		    Your("%s %s harmlessly through %s.",
		    	what, vtense(what, "pass"),
			mon_nam(mon));
		    hittxt = TRUE;
		}
	    } else {
		if (get_dmg_bonus) tmp = 1;
	    }
	}

	if (jousting) {
	    tmp += d(2, (obj == uwep) ? 10 : 2);        /* [was in dmgval()] */
	    if (obj && obj->oartifact == ART_JOUSTER_POWER) tmp += rnd(20);
	    if (obj && obj->oartifact == ART_CRASH_JOUST) {
			tmp += 10;
			if (!rn2(3) && mon->mcanmove) {
				mon->mcanmove = 0;
				mon->mfrozen = rnd(5);
				mon->mstrategy &= ~STRAT_WAITFORU;
			}
	    }
	    You("joust %s%s",
			 mon_nam(mon), canseemon(mon) ? exclam(tmp) : ".");
	    if (jousting < 0 && !(itemsurvivedestruction(obj, 8) && !issoviet) && !(Race_if(PM_CARTHAGE) && rn2(100)) ) {
		Your("%s shatters on impact!", xname(obj));

		if (obj->oartifact == ART_LUCKY_SHARDS) {

			if (P_MAX_SKILL(P_LANCE) == P_ISRESTRICTED) {
				unrestrict_weapon_skill(P_LANCE);
				pline("You can now learn the lance skill!");
			} else if (P_MAX_SKILL(P_LANCE) == P_UNSKILLED) {
				unrestrict_weapon_skill(P_LANCE);
				pline("You can now learn the lance skill!");
				P_MAX_SKILL(P_LANCE) = P_BASIC;
			} else if (P_MAX_SKILL(P_LANCE) == P_BASIC) {
				P_MAX_SKILL(P_LANCE) = P_SKILLED;
				pline("You can now become skilled with lances!");
			} else if (P_MAX_SKILL(P_LANCE) == P_SKILLED) {
				P_MAX_SKILL(P_LANCE) = P_EXPERT;
				pline("You can now become expert with lances!");
			} else if (P_MAX_SKILL(P_LANCE) == P_EXPERT) {
				P_MAX_SKILL(P_LANCE) = P_MASTER;
				pline("You can now become master with lances!");
			} else if (P_MAX_SKILL(P_LANCE) == P_MASTER) {
				P_MAX_SKILL(P_LANCE) = P_GRAND_MASTER;
				pline("You can now become grand master with lances!");
			} else if (P_MAX_SKILL(P_LANCE) == P_GRAND_MASTER) {
				P_MAX_SKILL(P_LANCE) = P_SUPREME_MASTER;
				pline("You can now become supreme master with lances!");
			} else pline("Sadly your knowledge of the lance skill is already maxed.");

		}

		if (obj->oartifact == ART_PENUMBRAL_LASSO && obj->spe >= 0) {
			pline("Except that it doesn't; instead, it just became very dull.");
			obj->spe = -10;

		} else { /* really shatter it */

			/* (must be either primary or secondary weapon to get here) */
			u.twoweap = FALSE;      /* untwoweapon() is too verbose here */
			if (obj == uwep) uwepgone();            /* set unweapon */
			/* minor side-effect: broken lance won't split puddings */
			useup(obj);
			obj = 0;
		}
	
	    }
	    /* avoid migrating a dead monster */
	    if (mon->mhp > tmp) {
		mhurtle(mon, u.dx, u.dy, 1);
		mdat = mon->data; /* in case of a polymorph trap */
		if (DEADMONSTER(mon)) already_killed = TRUE;
	    }
	    hittxt = TRUE;
	} else

	/* VERY small chance of stunning opponent if unarmed. */
	if (unarmed && tmp > 1 && !thrown && !obj && !Upolyd && !(PlayerCannotUseSkills) ) {
	    if (rnd(100) < P_SKILL(P_BARE_HANDED_COMBAT) &&
			!bigmonst(mdat) && !thick_skinned(mdat)) {
		if (canspotmon(mon))
		    pline("%s %s from your powerful strike!", Monnam(mon),
			  makeplural(stagger(mon->data, "stagger")));
		/* avoid migrating a dead monster */
		if (mon->mhp > tmp) {
		    mhurtle(mon, u.dx, u.dy, 1);
		    mdat = mon->data; /* in case of a polymorph trap */
		    if (DEADMONSTER(mon)) already_killed = TRUE;
		}
		hittxt = TRUE;
	    }
	}

	if (tmp && noeffect) {
	    if (silvermsg)
		tmp = 8;
	    else {
		if (!issoviet) Your("attack doesn't seem to harm %s.", mon_nam(mon));
		else pline("Etot monstr ne mozhet byt' povrezhden, potomu chto Sovetskiy khochet nesmotrya vas.");
		hittxt = TRUE;
		tmp = 0;
	    }
	}

	if (Race_if(PM_INHERITOR) && !rn2(100) && tmp > 1) tmp = 1;

	if (mon->egotype_flickerer) {
		pline("%s flickers and is impervious to melee and missile attacks!", Monnam(mon));
		if (!rn2(30)) pline("Reminder: you must use something else in order to damage this monster!");
		else if (!rn2(30)) pline("You read that right: your attacks are doing no damage at all!");
		else if (!rn2(30)) pline("Hint: try offensive wands or spells.");
	      tmp = 0;
		return FALSE;
	}

	if (mon->data == &mons[PM_DTTN_ERROR] && rn2(10)) {

		pline("%s absorbed the attack and took no damage!", Monnam(mon));
	      tmp = 0;
		return FALSE;
	}

	if (mon->data == &mons[PM_LITTLE_POISON_IVY] || mon->data == &mons[PM_CRITICALLY_INJURED_PERCENTS] || mon->data == &mons[PM_SUPERDEEP_TYPE] || mon->data == &mons[PM_AGULA] || mon->data == &mons[PM_MISTER_GRIBBS] || mon->data == &mons[PM_FLUIDATOR_IVE] || mon->data == &mons[PM_IMMUNITY_VIRUS] || mon->data == &mons[PM_AMBER_FEMMY] || mon->data == &mons[PM_UNGENOCIDABLE_VAMPSHIFTER] || mon->data == &mons[PM_TERRIFYING_POISON_IVY] || mon->data == &mons[PM_GIRL_WITH_THE_MOST_BEAUTIFUL_SHOES_IN_THE_WORLD] || mon->data == &mons[PM_IMMOVABLE_OBSTACLE] || mon->data == &mons[PM_INVINCIBLE_HAEN] || mon->data == &mons[PM_CHAREY] || mon->data == &mons[PM_INVENTOR_OF_THE_SISTER_COMBAT_BOOTS] || mon->data == &mons[PM_SWEET_ASIAN_POISON_IVY] || mon->data == &mons[PM_ARABELLA_SHOE] || mon->data == &mons[PM_ANASTASIA_SHOE] || mon->data == &mons[PM_HENRIETTA_SHOE] || mon->data == &mons[PM_KATRIN_SHOE] || mon->data == &mons[PM_JANA_SHOE] || mon->data == &mons[PM_FIRST_DUNVEGAN] || mon->data == &mons[PM_PERCENTI_HAS_LOST___] || mon->data == &mons[PM_PERCENTI_IS_IMMUNE_TO_THE_ATTACK_]) {

		pline("%s is IMMUNE to the attack!", Monnam(mon));
		if (FunnyHallu) You("curse at Konami for designing it like that.");
		if (!rn2(30)) pline("Reminder: you must use something else in order to damage this monster!");
		else if (!rn2(30)) pline("You read that right: your attacks are doing no damage at all!");
		else if (!rn2(30)) pline("Hint: try offensive wands or spells.");

	      tmp = 0;
		return FALSE;
	}

        /* WAC Added instant kill from wooden stakes vs vampire */
        /* based off Poison Code */
        /* fixed stupid mistake - check that obj exists before comparing...*/
        if (obj && (obj->otyp == WOODEN_STAKE || obj->oartifact == ART_VAMPIRE_KILLER) && is_vampire(mdat)) {
            if (Role_if(PM_UNDEAD_SLAYER) 
              || (!(PlayerCannotUseSkills) && (P_SKILL(weapon_type(obj)) >= P_EXPERT))
              || obj->oartifact == ART_STAKE_OF_VAN_HELSING) {
                if (!rn2(10)) {
                    You("plunge your stake into the heart of %s.", mon_nam(mon));
                    vapekilled = TRUE;
                } else {
                    You("drive your stake into %s.", mon_nam(mon));
                    tmp += rnd(6) + 2;
                    hittxt = TRUE;
                }
            } else if (obj->oartifact == ART_VAMPIRE_KILLER) {
                You("whip %s good!", mon_nam(mon));
                tmp += rnd(6);
                hittxt = TRUE;

		} else {
                You("drive your stake into %s.", mon_nam(mon));
                tmp += rnd(6);
                hittxt = TRUE;

            }

		if (obj->oartifact == ART_VAMPDOAING && rn2(2)) {
			You("plunge your stake into the heart of %s.", mon_nam(mon));
			vapekilled = TRUE;
		}

        }

	/* Special monk strikes */
	if ( (Role_if(PM_MONK) || (u.martialstyle == MARTIALSTYLE_JUDO && !uwep && (!u.twoweap || !uswapwep)) ) && !Upolyd && !thrown && no_obj &&
		(!uarm || (uarm && uarm->oartifact == ART_HA_MONK) || (uarm && uarm->oartifact == ART_BOBAIS) || (uarm->oartifact == ART_AMMY_S_RETRIBUTION) || (uarm && uarm->otyp >= ELVEN_TOGA &&
		 uarm->otyp <= ROBE_OF_WEAKNESS)) && !uarms &&
		 distu(mon->mx, mon->my) <= 2) {
	    /* just so we don't need another variable ... */
	    canhitmon = rnd(500);
	    if (u.martialstyle == MARTIALSTYLE_JUDO && !uwep && (!u.twoweap || !uswapwep)) {
		canhitmon /= 5;
		if (canhitmon < 1) canhitmon = 1;
	    }
	    if (canhitmon < (GushLevel / 8) && !thick_skinned(mdat)) {
		if (canspotmon(mon))
		    You("strike %s extremely hard!", mon_nam(mon));
		tmp *= 2;
		hittxt = TRUE;
	    } else if (canhitmon < (GushLevel / 4) && !thick_skinned(mdat)) {
		if (canspotmon(mon))
		    You("strike %s very hard!", mon_nam(mon));
		tmp += tmp / 2;
		hittxt = TRUE;
	    } else if (canhitmon < (GushLevel / 2) && !bigmonst(mon->data) &&
		    !thick_skinned(mdat)) {
		if (canspotmon(mon))
		    pline("%s %s from your powerful strike!", Monnam(mon),
			  makeplural(stagger(mon->data, "stagger")));
		/* avoid migrating a dead monster */
		if (mon->mhp > tmp) {
		    mhurtle(mon, u.dx, u.dy, 1);
		    mdat = mon->data; /* in case of a polymorph trap */
		    if (DEADMONSTER(mon)) already_killed = TRUE;
		}
		hittxt = TRUE;
	    }
	}

	if (Role_if(PM_HALF_BAKED) && !Upolyd && !thrown && no_obj &&
		(!uarm || (uarm && uarm->otyp >= ROBE &&
		 uarm->otyp <= ROBE_OF_WEAKNESS)) && !uarms &&
		 distu(mon->mx, mon->my) <= 2) {
	    /* just so we don't need another variable ... */
	    canhitmon = rnd(500);
		if (canhitmon < (GushLevel / 2) && !bigmonst(mon->data) && !thick_skinned(mdat) && !rn2(10)) {
		if (canspotmon(mon))
		    pline("%s %s from your powerful strike!", Monnam(mon),
			  makeplural(stagger(mon->data, "stagger")));
		/* avoid migrating a dead monster */
		if (mon->mhp > tmp) {
		    mhurtle(mon, u.dx, u.dy, 1);
		    mdat = mon->data; /* in case of a polymorph trap */
		    if (DEADMONSTER(mon)) already_killed = TRUE;
		}
		hittxt = TRUE;
	    }
	}

	/* FIQ wants the player to do fire damage to monsters while burned.
	 * I'm granting him that wish, but the player needs to have burnopathy too. --Amy */

	if (Burnopathy && Burned && (!resists_fire(mon) || player_will_pierce_resistance()) && !thrown ) {

	      pline("%s is burning!", Monnam(mon));

		tmp += rnd(u.ulevel);

	}

	if (u.enchantspell && (!resists_fire(mon) || player_will_pierce_resistance()) && !thrown ) {

	      pline("%s is burning!", Monnam(mon));

		tmp += rnd(u.ulevel);

	}

	if (RngeWeakness && tmp > 1) tmp /= 2;

	/* javelin is meant to be thrown, and therefore less effective in melee --Amy */
	if (!thrown && !Role_if(PM_TOSSER) && obj && obj->oclass == WEAPON_CLASS && (objects[obj->otyp].oc_skill == P_JAVELIN) && tmp > 1) {

		int javreduction = 0;

		if (!PlayerCannotUseSkills) {
			switch (P_SKILL(P_JAVELIN)) {
				case P_BASIC: javreduction = 20; break;
				case P_SKILLED: javreduction = 40; break;
				case P_EXPERT: javreduction = 60; break;
				case P_MASTER: javreduction = 80; break;
				case P_GRAND_MASTER: javreduction = 95; break;
				case P_SUPREME_MASTER: javreduction = 100; break;
				default: javreduction = 0; break;
			}
		}

		if (javreduction < rnd(100)) tmp /= 2;

	}

	/* skull swords don't want to be used in melee */
	if (!thrown && obj && obj->oartifact == ART_SKULL_SWORD) tmp /= 2;
	if (!thrown && obj && obj->oartifact == ART_VIOLENT_SKULL_SWORD) tmp /= 2;

	if (u.martialstyle == MARTIALSTYLE_BOJUTSU && obj && weapon_type(obj) == P_QUARTERSTAFF && !thrown) {
		tmp *= 2;
		tmp /= 3;
	}

	if (uarms && uarms->otyp == COMPLETE_BLOCKAGE_SHIELD && !thrown && tmp > 1) {
		tmp /= 2;
	}

	/* heavy two-handed weapons are bad versus tiny enemies (hard to effectively hit a tiny monster with a huge weapon) */
	if (!thrown && obj && is_heavyweapon(obj) && verysmall(mon->data) && tmp > 1) {

		int heavyreduction = 50;

		if (!PlayerCannotUseSkills) {
			switch (P_SKILL(P_TWO_HANDED_WEAPON)) {
				case P_BASIC: heavyreduction = 58; break;
				case P_SKILLED: heavyreduction = 66; break;
				case P_EXPERT: heavyreduction = 75; break;
				case P_MASTER: heavyreduction = 83; break;
				case P_GRAND_MASTER: heavyreduction = 91; break;
				case P_SUPREME_MASTER: heavyreduction = 100; break;
				default: heavyreduction = 50; break;
			}
		}

		if (heavyreduction < rnd(100)) tmp /= 2;
	}

	if (FemtrapActiveMagdalena && humanoid(mon->data) && is_female(mon->data) && tmp > 1) tmp /= 2;

	if (obj && obj->oartifact == ART_RHALALALALALAAAAR && (mon->mhp > tmp) ) {
		if ((mon->mhp - tmp) < (mon->mhpmax / 10)) {
			tmp += 10000; /* instant death for the monster */
			pline("Rhalalalalalaaaar!");
		}
	}

	if (!already_killed) mon->mhp -= tmp;
	/* adjustments might have made tmp become less than what
	   a level draining artifact has already done to max HP */
	if (mon->mhp > mon->mhpmax) mon->mhp = mon->mhpmax;
	if (mon->mhp < 1)
		destroyed = TRUE;
	/* fixed bug with hitting tame monster with non-magic weapon */        
	if (mon->mtame && (!mon->mflee || mon->mfleetim) && tmp > 0) {

		abuse_dog(mon);
		monflee(mon, 10 * rnd(tmp), FALSE, FALSE);
	}
	if((mdat == &mons[PM_SHOCK_PUDDING] || mdat == &mons[PM_VOLT_PUDDING] || mdat == &mons[PM_BLACK_PUDDING] || mdat == &mons[PM_DIVISIBLE_ICE_CREAM_BAR] || mdat == &mons[PM_GEMINICROTTA] || mdat == &mons[PM_GREY_PUDDING] || mdat == &mons[PM_STICKY_PUDDING] || mdat == &mons[PM_DRUDDING] || mdat == &mons[PM_BLACK_DRUDDING] || mdat == &mons[PM_BLACKSTEEL_PUDDING] || mdat == &mons[PM_BLOOD_PUDDING] || mdat == &mons[PM_MORAL_HAZARD] || mdat == &mons[PM_MORAL_EVENT_HORIZON] || mdat == &mons[PM_BLACK_PIERCER] || mdat == &mons[PM_BROWN_PUDDING])
		   && obj /* && obj == uwep -- !thrown and obj == weapon */
		   && !thrown
		   && objects[obj->otyp].oc_material == MT_IRON
		   && mon->mhp > 1 && !thrown && !mon->mcan && !rn2(10) /*slowing down pudding farming --Amy*/
		   /* && !destroyed  -- guaranteed by mhp > 1 */ ) {
		if (clone_mon(mon, 0, 0)) {
			pline("%s divides as you hit it!", Monnam(mon));
			/* make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON), 40),
			0, TRUE, SICK_NONVOMITABLE); */
		      (void) difeasemu(mon->data); /* hopefully stopping those annoying pudding farmers! */
			increasesincounter(1); /* adding even more punishment for lame farmers */
			u.alignlim--;
			hittxt = TRUE;
		}
	}
	if((mdat == &mons[PM_ARCH_LICHEN]) /* hit by any weapon and not destroyed, splits */
		   && obj && obj == uwep && mon->mhp > 1 && !rn2(10) ) { /*sorry I had to reduce the chance a bit --Amy*/
		if (clone_mon(mon, 0, 0)){
			pline("RNG decides to clone %s in front of your eyes!", Monnam(mon));
			hittxt = TRUE;
		}
	}

	if ((!hittxt || (mon->data->msound == MS_COMBAT) || (youmonst.data->msound == MS_COMBAT) || (Role_if(PM_SPACEWARS_FIGHTER) || sanitymessage || Role_if(PM_CAMPERSTRIKER) || Role_if(PM_HUSSY) || FemtrapActiveAriane || Role_if(PM_GANG_SCHOLAR) || Role_if(PM_WALSCHOLAR) || ishaxor || Hallucination) ) &&			/*( thrown => obj exists )*/
	  (!destroyed || (thrown && m_shot.n > 1 && m_shot.o == obj->otyp))) {

		if (!thrown && Role_if(PM_HUSSY) && !mon->female && !is_neuter(mon->data)) {
			u.hussyhurtturn = (moves + 1000);
		}

		if (thrown) hit(mshot_xname(obj), mon, exclam(tmp));
		else if (!flags.verbose) You("hit it.");

		else if ((Role_if(PM_SPACEWARS_FIGHTER) || (mon->data->msound == MS_COMBAT) || (youmonst.data->msound == MS_COMBAT) || Role_if(PM_CAMPERSTRIKER) || Role_if(PM_HUSSY) || Role_if(PM_GANG_SCHOLAR) || FemtrapActiveAriane || Role_if(PM_WALSCHOLAR) || ishaxor || Hallucination || sanitymessage) && !rn2(20) && (PlayerHearsSoundEffects) ) {

			switch (rnd(15)) {
	
			case 1: pline("Daennaennaennaennaennaennaennaennaenn!"); break;
			case 2: pline("Skrrrrrrrrrrr!"); break;
			case 3: pline("Waedaewaedaewaennwaennwaenn!"); break;
			case 4: pline("Daediedaedae daedaedaedae!"); break;
			case 5: pline("Eeeep!"); break;
			case 6: pline("Krrk"); break;
			case 7: pline("Wiiiiiiel!"); break;
			case 8: pline("Wlallalalallp!"); break;
			case 9: pline("Wooooooooar!"); break;
			case 10: pline("Waedawinnindiuu!"); break;
			case 11: pline("Defff!"); break;
			case 12: pline("Wloe!"); break;
			case 13: pline("Dschiimdschiim!"); break;
			case 14: pline("Baeiibaeiibaeiibaeiibaeiibaeiibaeii!"); break;
			case 15: pline("Ditdae!"); break;

			}

		}

		else if ((Role_if(PM_SPACEWARS_FIGHTER) || (mon->data->msound == MS_COMBAT) || (youmonst.data->msound == MS_COMBAT) || Role_if(PM_CAMPERSTRIKER) || Role_if(PM_HUSSY) || Role_if(PM_GANG_SCHOLAR) || FemtrapActiveAriane || Role_if(PM_WALSCHOLAR) || ishaxor || Hallucination || sanitymessage) && (!rn2(5) || (mon->data->msound == MS_COMBAT) || (youmonst.data->msound == MS_COMBAT) ) ) {

			switch (rnd(647)) {
	
			case 1: pline("%s staggers from your furious assault.", Monnam(mon)); break;
			case 2: pline("Your cut barely scratches %s's scales.", mon_nam(mon)); break;
			case 3: pline("%s gasps as your weapon strikes home.", Monnam(mon)); break;
			case 4: pline("Your strike barely mars %s's scales.", mon_nam(mon)); break;
			case 5: pline("%s flinches as you score a hit.", Monnam(mon)); break;
			case 6: pline("%s stoically ignores your shallow cut.", Monnam(mon)); break;
			case 7: pline("You merely bruise %s as it dodges to one side.", mon_nam(mon)); break;
			case 8: pline("You thrust deep into %s!", mon_nam(mon)); break;
			case 9: pline("You strike %s a glancing blow.", mon_nam(mon)); break;
			case 10: pline("You hit %s in the %s!", mon_nam(mon), mbodypart(mon,ARM)); break;
			case 11: pline("You hit %s in the %s!", mon_nam(mon), mbodypart(mon,LEG)); break;
			case 12: pline("You scratch %s", mon_nam(mon)); break;
			case 13: pline("%s reels from the fury of your attack.", Monnam(mon)); break;
			case 14: pline("Your thrust scores a shallow wound through %s's scales.", mon_nam(mon)); break;
			case 15: pline("You barely hit %s.", mon_nam(mon)); break;
			case 16: pline("%s roars in pain as you draw %s!", Monnam(mon), mbodypart(mon,BLOOD)); break;
			case 17: pline("You strike a ringing blow against %s, which stumbles under your assault.", mon_nam(mon)); break;
			case 18: pline("You smite %s, driving it back a step.", mon_nam(mon)); break;
			case 19: pline("You deal %s a solid blow!", mon_nam(mon)); break;
			case 20: pline("You slip past %s's guard and hit!", mon_nam(mon)); break;
			case 21: pline("You slash %s, opening a %sless cut.", mon_nam(mon), mbodypart(mon,BLOOD)); break;
			case 22: pline("You hit %s on the flank!", mon_nam(mon)); break;
			case 23: pline("%s is fully hit by you.", Monnam(mon)); break;
			case 24: pline("You wound %s's scales with your axe.", mon_nam(mon)); break;
			case 25: pline("%s is hit by your weapon.", Monnam(mon)); break;
			case 26: pline("You wound %s's scales.", mon_nam(mon)); break;
			case 27: pline("%s loses quite some energy.", Monnam(mon)); break;
			case 28: pline("%s ignores your superhit.", Monnam(mon)); break;
			case 29: pline("You flog %s as it tries to defend.", mon_nam(mon)); break;
			case 30: pline("You wound %s's scales with the machine gun", mon_nam(mon)); break;
			case 31: pline("%s's scales lost lines.", Monnam(mon)); break;
			case 32: pline("%s lost quite some lines.", Monnam(mon)); break;
			case 33: pline("You hit %s glancingly.", mon_nam(mon)); break;
			case 34: pline("%s has lost quite some lines", Monnam(mon)); break;
			case 35: pline("You take off lines from %s's scales but are forced to give up eventually.", mon_nam(mon)); break;
			case 36: pline("You heavily injured %s.", mon_nam(mon)); break;
			case 37: pline("You strike the %s.", l_monnam(mon)); break;
			case 38: pline("%s is rammed away by you.", Monnam(mon)); break;
			case 39: pline("You fend!"); break;
			case 40: pline("Random hit."); break;
			case 41: pline("%s lost lines for once.", Monnam(mon)); break;
			case 42: pline("The %s its scales have fewer lines remaining!", l_monnam(mon)); break;
			case 43: pline("Barely hit."); break;
			case 44: pline("The over-driving pulls off lines of %s!", mon_nam(mon)); break;
			case 45: pline("You fully ram into %s but it gets up again, the old bag!", mon_nam(mon)); break;
			case 46: pline("You blind %s with a headlight.", mon_nam(mon)); break;
			case 47: pline("%s did not watch out: Hit!", Monnam(mon)); break;
			case 48: pline("Well finally you scored a hit for once!"); break;
			case 49: pline("%s allows you to hit it!", Monnam(mon)); break;
			case 50: pline("You hit %s in the %s!", mon_nam(mon), mbodypart(mon,FINGER)); break;
			case 51: pline("You hit %s in the %s!", mon_nam(mon), mbodypart(mon,FINGERTIP)); break;
			case 52: pline("You hit %s in the %s!", mon_nam(mon), mbodypart(mon,FOOT)); break;
			case 53: pline("You hit %s in the %s!", mon_nam(mon), mbodypart(mon,HAND)); break;
			case 54: pline("You hit %s in the %s!", mon_nam(mon), mbodypart(mon,TOE)); break;
			case 55: pline("You mess up %s's %s!", mon_nam(mon), mbodypart(mon,HAIR)); break;
			case 56: pline("You hit %s by a league!", mon_nam(mon)); break;
			case 57: pline("You beat up the annoying %s.", l_monnam(mon)); break;
			case 58: pline("%s is fully flogged by you.", Monnam(mon)); break;
			case 59: pline("%s loses some lines now!", Monnam(mon)); break;
			case 60: pline("You start attacking the superopponent %s.", l_monnam(mon)); break;
			case 61: pline("You perform an energy impact"); break;
			case 62: pline("You pull off lines of %s.", mon_nam(mon)); break;
			case 63: pline("%s is hit.", Monnam(mon)); break;
			case 64: pline("%s hits itself!", Monnam(mon)); break;
			case 65: pline("%s loses something!", Monnam(mon)); break;
			case 66: pline("You hit for a change!"); break;
			case 67: pline("You pull off lines of %s's scales with the machine gun!", mon_nam(mon)); break;
			case 68: pline("%s allows you to fully hit it, somehow!", Monnam(mon)); break;
			case 69: pline("Your sword ringed %s, so that lost lines.", mon_nam(mon)); break;
			case 70: pline("Direct hit! %s allowed you to hit it", Monnam(mon)); break;
			case 71: pline("Hit!"); break;
			case 72: pline("You hit %s even though that is hard", mon_nam(mon)); break;
			case 73: pline("%s loses some lines to you.", Monnam(mon)); break;
			case 74: pline("Hit on the flank!"); break;
			case 75: pline("Hit! %s loses something", Monnam(mon)); break;
			case 76: pline("You cover for %s one.", mon_nam(mon)); break;
			case 77: pline("You strike %s on the muzzle.", mon_nam(mon)); break;
			case 78: pline("Unbelievable! You can actually fight!"); break;
			case 79: pline("Direct hit, bwoong-wongwongwong"); break;
			case 80: pline("Your weapon hits %s!", mon_nam(mon)); break;
			case 81: pline("You fully kick %s into the nuts.", mon_nam(mon)); break;
			case 82: pline("Foul! You get a red card and lose!"); break;
			case 83: pline("Well, for that offense you're getting a red card."); break;
			case 84: pline("Foul! %s is injured and gets a penalty kick!", Monnam(mon)); break;
			case 85: pline("You fire off %s and get a red card!", mon_nam(mon)); break;
			case 86: pline("%s is struck fully over by you.", Monnam(mon)); break;
			case 87: pline("You outplayed %s.", mon_nam(mon)); break;
			case 88: pline("You passed!"); break;
			case 89: pline("Oh! Good trick!"); break;
			case 90: pline("You give the crazy %s a beating. That is injured.", l_monnam(mon)); break;
			case 91: pline("%s is hit by your superimpact.", Monnam(mon)); break;
			case 92: pline("%s gasps and has lost lines, unfortunately.", Monnam(mon)); break;
			case 93: pline("You sting the %s baddie.", l_monnam(mon)); break;
			case 94: pline("Reeled! %s lost lines!", Monnam(mon)); break;
			case 95: pline("Roared! %s lost quite some lines", Monnam(mon)); break;
			case 96: pline("%s stumbles and gave away some lines for no purpose.", Monnam(mon)); break;
			case 97: pline("Smited! %s goes back a few steps.", Monnam(mon)); break;
			case 98: pline("Slashed! %s loses a few lines once again", Monnam(mon)); break;
			case 99: pline("You hit the %s. That loses lines.", l_monnam(mon)); break;
			case 100: pline("%s is hit by a rocket.", Monnam(mon)); break;
			case 101: pline("%s takes a hit from you", Monnam(mon)); break;
			case 102: pline("You kick the %s, that loses lines.", l_monnam(mon)); break;
			case 103: pline("You hit %s a couple of times, but it still lives!", mon_nam(mon)); break;
			case 104: pline("You hit %s by a world-championship!", mon_nam(mon)); break;
			case 105: pline("You just fully lambasted %s!", mon_nam(mon)); break;
			case 106: pline("You try to remove the superopponent %s which is ultra hard.", l_monnam(mon)); break;
			case 107: pline("Bamm! You shot."); break;
			case 108: pline("You open fire at %s.", mon_nam(mon)); break;
			case 109: pline("%s's scales get rid of lines!", Monnam(mon)); break;
			case 110: pline("You lambast %s, but it still lives!", mon_nam(mon)); break;
			case 111: pline("You're capable of getting something right after all!"); break;
			case 112: pline("You're exceptionally doing things right! I didn't expect that at all!"); break;
			case 113: pline("After a long streak of misses, you land another hit."); break;
			case 114: pline("Luck-based hit!"); break;
			case 115: pline("Exceptionally you scored a hit."); break;
			case 116: pline("You hit %s! Unbelievable!", mon_nam(mon)); break;
			case 117: pline("You throw over %s, but that opponent baddie quickly got up again!", mon_nam(mon)); break;
			case 118: pline("You performed a shield bash!"); break;
			case 119: pline("You ringed %s with your sword!", mon_nam(mon)); break;
			case 120: pline("You pull lines off from the opponent."); break;
			case 121: pline("Direct hit, but totally!"); break;
			case 122: pline("You did hit after all!"); break;
			case 123: pline("Your weapon strifes the %s baddie!", l_monnam(mon)); break;
			case 124: pline("You cleared %s one.", mon_nam(mon)); break;
			case 125: pline("You shoot sniper rounds at %s!", mon_nam(mon)); break;
			case 126: pline("You ram your blade into %s.", mon_nam(mon)); break;
			case 127: pline("%s's armor was useless!", Monnam(mon)); break;
			case 128: pline("Barely!"); break;
			case 129: pline("You ram your spear into the front portion of %s!", mon_nam(mon)); break;
			case 130: pline("You slam %s against a wall.", mon_nam(mon)); break;
			case 131: pline("Schrack! Your shield bash fully hit %s!", mon_nam(mon)); break;
			case 132: pline("%s gets acquainted with your axe!", Monnam(mon)); break;
			case 133: pline("%s cuts itself at your lightsaber.", Monnam(mon)); break;
			case 134: pline("Ringed! %s was wounded by your superweapon.", Monnam(mon)); break;
			case 135: pline("You strike %s fully over!", mon_nam(mon)); break;
			case 136: pline("%s's scales allowed themselves to get hit!", Monnam(mon)); break;
			case 137: pline("%s says: 'Ow-wow-wow!'", Monnam(mon)); break;
			case 138: pline("%s was stupid enough to get hit!", Monnam(mon)); break;
			case 139: pline("You succeeded in hitting %s!", mon_nam(mon)); break;
			case 140: pline("You strike %s. That is hurting now.", mon_nam(mon)); break;
			case 141: pline("You lambast %s as it tries to defend.", mon_nam(mon)); break;
			case 142: pline("%s says: 'Ooooooooouch!'", Monnam(mon)); break;
			case 143: pline("You're striking %s!", mon_nam(mon)); break;
			case 144: pline("%s was finally hit for once!", Monnam(mon)); break;
			case 145: pline("You hit %s, which happens rarely enough.", mon_nam(mon)); break;
			case 146: pline("Hit, %s doesn't watch out!", mon_nam(mon)); break;
			case 147: pline("You ram %s.", mon_nam(mon)); break;
			case 148: pline("You hit %s in the elbow!", mon_nam(mon)); break;
			case 149: pline("The suicide attack worked!"); break;
			case 150: pline("%s loses some energy lines!", Monnam(mon)); break;
			case 151: pline("You didn't completely defeat %s yet.", mon_nam(mon)); break;
			case 152: pline("You fully strike %s!", mon_nam(mon)); break;
			case 153: pline("%s was strifed by your ammo.", Monnam(mon)); break;
			case 154: pline("%s is hit by your machine gun rounds!", Monnam(mon)); break;
			case 155: pline("You hit %s around a corner!", mon_nam(mon)); break;
			case 156: pline("%s lost arms lines!", Monnam(mon)); break;
			case 157: pline("You unload a Colt magazine into %s.", mon_nam(mon)); break;
			case 158: pline("You land a hit in the %s", mbodypart(mon,HEAD)); break;
			case 159: pline("You puncture %s!", mon_nam(mon)); break;
			case 160: pline("You hit %s with the pistol.", mon_nam(mon)); break;
			case 161: pline("Bang! Hit!"); break;
			case 162: pline("%s is hit into the belly by your ammo!", Monnam(mon)); break;
			case 163: pline("You shot a %sy wound into %s's %s!", mbodypart(mon,BLOOD), mon_nam(mon), mbodypart(mon,HAND)); break;
			case 164: pline("%s is heavily hit by you!", Monnam(mon)); break;
			case 165: pline("You shoot %s a %sy wound to the %s.", mon_nam(mon), mbodypart(mon,BLOOD), mbodypart(mon,LEG)); break;
			case 166: pline("You keep %s away from you by using some ammo.", mon_nam(mon)); break;
			case 167: pline("%s partly deflects!", Monnam(mon)); break;
			case 168: pline("You shoot an %s off of %s.", mbodypart(mon,ARM), mon_nam(mon)); break;
			case 169: pline("Bamm! You struck %s.", mon_nam(mon)); break;
			case 170: pline("That was even a direct hit!"); break;
			case 171: pline("%s says: 'I will catch you!'", Monnam(mon)); break;
			case 172: pline("Paff! Dock! Kapow! You lambasted %s.", mon_nam(mon)); break;
			case 173: pline("You bash into %s's visage!", mon_nam(mon)); break;
			case 174: pline("You scratch open %s's %s, causing %s to flow out.", mon_nam(mon), mbodypart(mon,LEG), mbodypart(mon,BLOOD)); break;
			case 175: pline("%s was hit by a pistol magazine from you!", Monnam(mon)); break;
			case 176: pline("You fully raced into %s.", mon_nam(mon)); break;
			case 177: pline("%s allows you to fully away-ram it!", Monnam(mon)); break;
			case 178: pline("%s catches fire!", Monnam(mon)); break;
			case 179: pline("That really hurt!"); break;
			case 180: pline("You use the rifle against %s.", mon_nam(mon)); break;
			case 181: pline("%s races into another car.", Monnam(mon)); break;
			case 182: pline("%s is singed by a Krishna's flamethrower!", Monnam(mon)); break;
			case 183: pline("You stop %s in its tracks, forcing it to restart.", mon_nam(mon)); break;
			case 184: pline("You kick %s. It loses lines.", mon_nam(mon)); break;
			case 185: pline("%s's scales can't stop your weapon!", Monnam(mon)); break;
			case 186: pline("You weakened %s.", mon_nam(mon)); break;
			case 187: pline("Your sword hurts %s.", mon_nam(mon)); break;
			case 188: pline("You shot some bullets at %s.", mon_nam(mon)); break;
			case 189: pline("You measured %s with your measurer.", mon_nam(mon)); break;
			case 190: pline("%s is hit by your sword.", Monnam(mon)); break;
			case 191: pline("You cut %s with your axe, causing it to bleed.", mon_nam(mon)); break;
			case 192: pline("You pull off lines of %s's hide!", mon_nam(mon)); break;
			case 193: pline("You detach one of %s's %ss!", mon_nam(mon), mbodypart(mon,ARM)); break;
			case 194: pline("You strike %s %sily!", mon_nam(mon), mbodypart(mon,BLOOD)); break;
			case 195: pline("You go berserk and hit!"); break;
			case 196: pline("You hit %s through the armor!", mon_nam(mon)); break;
			case 197: pline("You struck off a %s of %s.", mbodypart(mon,ARM), mon_nam(mon) ); break;
			case 198: pline("You kick %s in the belly!", mon_nam(mon)); break;
			case 199: pline("You head-butt %s.", mon_nam(mon)); break;
			case 200: pline("You push %s over.", mon_nam(mon)); break;
			case 201: pline("You broke %s's %s!", mon_nam(mon), mbodypart(mon,FINGER)); break;
			case 202: pline("You broke %s's %s!", mon_nam(mon), mbodypart(mon,TOE)); break;
			case 203: pline("You kick %s in the shins.", mon_nam(mon)); break;
			case 204: pline("You stab %s in the %s.", mon_nam(mon), mbodypart(mon,ARM)); break;
			case 205: pline("You stab %s in the %s.", mon_nam(mon), mbodypart(mon,LEG)); break;
			case 206: pline("You hit %s over the %s.", mon_nam(mon), mbodypart(mon,HEAD)); break;
			case 207: pline("You knee %s in the groin!", mon_nam(mon)); break;
			case 208: pline("You aim a high kick at %s's %s.", mon_nam(mon), mbodypart(mon,HEAD)); break;
			case 209: pline("You stab %s in the ribs.", mon_nam(mon)); break;
			case 210: pline("You chop at %s's %s.", mon_nam(mon), mbodypart(mon,NECK)); break;
			case 211: pline("You put a tight choke-hold on %s.", mon_nam(mon)); break;
			case 212: pline("You attempt to topple %s over.", mon_nam(mon)); break;
			case 213: pline("You punch %s.", mon_nam(mon)); break;
			case 214: pline("You attempt to poke %s in the %s!", mon_nam(mon), mbodypart(mon,EYE)); break;
			case 215: pline("You twist %s's %s.", mon_nam(mon), mbodypart(mon,LEG)); break;
			case 216: pline("You twist %s's %s.", mon_nam(mon), mbodypart(mon,ARM)); break;
			case 217: pline("You bend %s's %s.", mon_nam(mon), makeplural(mbodypart(mon,FINGER)) ); break;
			case 218: pline("You punch %s in the kidneys!", mon_nam(mon)); break;
			case 219: pline("You smash %s with your elbow.", mon_nam(mon)); break;
			case 220: pline("You smash %s with your knee.", mon_nam(mon)); break;
			case 221: pline("You scratch %s.", mon_nam(mon)); break;
			case 222: pline("You give %s a nasty bruise.", mon_nam(mon)); break;
			case 223: pline("You stub %s's %s.", mon_nam(mon), mbodypart(mon,TOE)); break;
			case 224: pline("You jab %s in the ribs.", mon_nam(mon)); break;
			case 225: pline("You made %s's %s bleed.", mon_nam(mon), mbodypart(mon,NOSE)); break;
			case 226: pline("You slap %s.", mon_nam(mon)); break;
			case 227: pline("%s slips and falls.", Monnam(mon)); break;
			case 228: pline("You pull at %s's %s.", mon_nam(mon), mbodypart(mon,HAIR)); break;
			case 229: pline("You punch %s in the %s.", mon_nam(mon), mbodypart(mon,NOSE)); break;
			case 230: pline("You pull at %s's ear.", mon_nam(mon)); break;
			case 231: pline("You scream an insult at %s.", mon_nam(mon)); break;
			case 232: pline("You scowl at %s.", mon_nam(mon)); break;
			case 233: pline("%s says: 'Now you can however which expierience!'", Monnam(mon)); break;
			case 234: pline("%s was scratched by your sword and is really angry now.", Monnam(mon)); break;
			case 235: pline("Now %s becomes however correctly sourly!", mon_nam(mon)); break;
			case 236: pline("You stab %s fully into the belly!", mon_nam(mon)); break;
			case 237: pline("Your sexy leather pumps scratch up and down %s's %s.", mon_nam(mon), makeplural(mbodypart(mon, LEG))); break;
			case 238: pline("You draw %s by sliding your female mocassins over %s's %s.", mbodypart(mon, BLOOD), mon_nam(mon), makeplural(mbodypart(mon, HAND))); break;
			case 239: pline("With your Nike(TM) sneakers, you place a strong kick into %s's eggs.", mon_nam(mon)); break;
			case 240: pline("You scratch open %s's skin with your velcro shoes.", mon_nam(mon)); break;
			case 241: pline("You blast %s with a fire bolt.", mon_nam(mon)); break;
			case 242: pline("You chill %s with a frost bolt.", mon_nam(mon)); break;
			case 243: pline("You dig into %s's body with your pick-axe.", mon_nam(mon)); break;
			case 244: pline("Keep attacking %s and you may earn a trophy!", mon_nam(mon)); break;
			case 245: pline("You slap %s, generating a ringing sound.", mon_nam(mon)); break;
			case 246: pline("You claw %s's %s with your fingernails.", mon_nam(mon), mbodypart(mon, FACE)); break;
			case 247: pline("You sting %s in the %s with your scalpel.", mon_nam(mon), mbodypart(mon, FINGERTIP)); break;
			case 248: pline("%s gets %s cancer from your attack!", Monnam(mon), mbodypart(mon, LUNG)); break;
			case 249: pline("You whack %s's %s with your high heel.", mon_nam(mon), mbodypart(mon, HEAD)); break;
			case 250: pline("You kick %s in the shin with your plateau sandals.", mon_nam(mon)); break;
			case 251: pline("You hit %s over the %s with your massive wedge-heeled sandal.", mon_nam(mon), mbodypart(mon, HEAD)); break;
			case 252: pline("Being a member of %s allows you to hit!", urace.coll); break;
			case 253: pline("And yet again you hit!"); break;
			case 254: pline("Unlike Eyehack, this variant allows you to actually hit monsters, and so you do."); break;
			case 255: pline("Thankfully you're not playing terrahack with its screwy to-hit calculations, so you hit %s.", mon_nam(mon)); break;
			case 256: pline("Your character is more skilled than it could be if this were K-Measurer, and so your weapon passes the to-hit check!"); break;
			case 257: pline("Vanilla Nethack might have made you miss this attack, but here, you hit %s.", mon_nam(mon)); break;
			case 258: pline("If this were Rodney's Tower, %s would totally own you. But you're playing something else, and so %s loses health.", mon_nam(mon), mon_nam(mon)); break;
			case 259: pline("You sting/strike/slit/<insert other attack verb here> %s!", mon_nam(mon)); break;
			case 260: pline("You hit %s with a thrown rock!", mon_nam(mon)); break;
			case 261: pline("Clash! You rammed %s.", mon_nam(mon)); break;
			case 262: pline("You damage %s's clothing!", mon_nam(mon)); break;
			case 263: pline("You break %s's eyeglasses!", mon_nam(mon)); break;
			case 264: pline("%s fails to duck under your swing!", Monnam(mon)); break;
			case 265: pline("You fart %s in the %s!", mon_nam(mon), mbodypart(mon, FACE)); break;
			case 266: pline("You smash a heavy object into %s's upper body.", mon_nam(mon)); break;
			case 267: pline("You throw a chair at %s and hit!", mon_nam(mon)); break;
			case 268: pline("You grab %s's %s and smash it against a wall!", mon_nam(mon), mbodypart(mon, HEAD)); break;
			case 269: pline("You cause a bit of pain to %s.", mon_nam(mon)); break;
			case 270: pline("You spit %s in the %s.", mon_nam(mon), mbodypart(mon, FACE)); break;
			case 271: pline("You poke a sharp weapon into %s's ass.", mon_nam(mon)); break;
			case 272: pline("Your furious attack damages %s's underwear!", mon_nam(mon)); break;
			case 273: pline("Klock! You hit %s's %s with the heel of your sexy leather pump.", mon_nam(mon), mbodypart(mon, HEAD)); break;
			case 274: pline("The hit connected!"); break;
			case 275: pline("%s tries to dodge, but you hit it anyway!", Monnam(mon)); break;
			case 276: pline("You bash %s, leaving a bruise.", mon_nam(mon)); break;
			case 277: pline("You kick a heavy object and manage to hit %s's knee, making a huge dent.", mon_nam(mon)); break;
			case 278: pline("%s's knuckles deftly block your blow, screaming in pain in the process.", Monnam(mon)); break;
			case 279: pline("You smash your hammer into %s's %s, who hops around trying to soothe the pain.", mon_nam(mon), mbodypart(mon, FOOT)); break;
			case 280: pline("You mercilessly clobber %s's groin, who doubles over in pain.", mon_nam(mon)); break;
			case 281: pline("Your blow slams into %s's %s with overwhelming impact.", mon_nam(mon), mbodypart(mon, ARM)); break;
			case 282: pline("Your crushing swipe slams right into %s's %s, fracturing a bone.", mon_nam(mon), mbodypart(mon, LEG)); break;
			case 283: pline("Your vicious hit tears %s's skin open.", mon_nam(mon)); break;
			case 284: pline("You nicked %s, leaving a small scar.", mon_nam(mon)); break;
			case 285: pline("The point of your weapon glances off %s's knuckle bone, opening a small wound.", mon_nam(mon)); break;
			case 286: pline("You stab %s's %s, causing it to lose balance.", mon_nam(mon), mbodypart(mon, LEG)); break;
			case 287: pline("%s ducks and almost manages to avoid your thrust, but ends up getting hit in the %s.", Monnam(mon), mbodypart(mon, HEAD)); break;
			case 288: pline("You aim a blow straight at %s's heart, but end up scraping the ribs instead.", mon_nam(mon)); break;
			case 289: pline("You sting %s, who receives a new scar.", mon_nam(mon)); break;
			case 290: pline("You leave a small burn scar on %s's skin.", mon_nam(mon)); break;
			case 291: pline("You blast across %s's %s, whose skull is encased in energy and dazed.", mon_nam(mon), mbodypart(mon, HEAD)); break;
			case 292: pline("You spray hideous acrid fumes causing %s to gasp and choke.", mon_nam(mon)); break;
			case 293: pline("You kick %s in the chest, who is knocked off its %s by the force of your blast.", mon_nam(mon), makeplural(mbodypart(mon, FOOT))); break;
			case 294: pline("You disfigure %s's skin, granting it a horrific scar.", mon_nam(mon)); break;
			case 295: pline("You clip %s with your tooth, leaving a small bite mark.", mon_nam(mon)); break;
			case 296: pline("Your quick snap breaks the skin on %s's %s.", mon_nam(mon), mbodypart(mon, HAND)); break;
			case 297: pline("You scrape across %s's %s causing it to stumble.", mon_nam(mon), mbodypart(mon, LEG)); break;
			case 298: pline("Your rake catches %s in the %s leaving it dazed.", mon_nam(mon), mbodypart(mon, HEAD)); break;
			case 299: pline("You grab %s's %s and try to yank it off its %s.", mon_nam(mon), mbodypart(mon, LEG), makeplural(mbodypart(mon, FOOT))); break;
			case 300: pline("Your stout fetid breath causes %s to gasp and choke.", mon_nam(mon)); break;
			case 301: pline("You chilled %s, leaving a small frostburn scar.", mon_nam(mon)); break;
			case 302: pline("The skin on %s's %s shrinks and cracks.", mon_nam(mon), mbodypart(mon, HEAD)); break;
			case 303: pline("Thankfully %s is not a %s, and therefore you hit.", mon_nam(mon), rndmonnam()); break;
			case 304: pline("Yet again your left fist finds its way to %s's %s!", mon_nam(mon), mbodypart(mon, FACE)); break;
			case 305: pline("%s becomes all %s from your vicious attack!", Monnam(mon), rndcolor()); break;
			case 306: pline("You open pretty wounds on %s's skin!", mon_nam(mon)); break;
			case 307: pline("Your sharp-edged brown combat boot heels scratch very pretty lovely huggable wounds on %s's %s!", mon_nam(mon), makeplural(mbodypart(mon, LEG))); break;
			case 308: pline("You tried fawn on %s.", mon_nam(mon)); break;
			case 309: pline("You strike %s with dark force.", mon_nam(mon)); break;
			case 310: pline("You try to doom %s, but it only feels mildly uncomfortable.", mon_nam(mon)); break;
			case 311: pline("You briefly turn invisible and strike %s from behind!", mon_nam(mon)); break;
			case 312: pline("You shock %s with a shrill sound.", mon_nam(mon)); break;
			case 313: pline("%s gets a cut from your thrown shuriken.", Monnam(mon)); break;
			case 314: pline("You poke %s with a stick.", mon_nam(mon)); break;
			case 315: pline("Your blade scratches %s's %s.", mon_nam(mon), makeplural(mbodypart(mon, LEG))); break;
			case 316: pline("You throw an uppercut at %s's %s!", mon_nam(mon), mbodypart(mon, HEAD)); break;
			case 317: pline("You manage to scar %s's %s!", mon_nam(mon), mbodypart(mon, FACE)); break;
			case 318: pline("You batter %s with a series of punches!", mon_nam(mon)); break;
			case 319: pline("%s sustains a paper cut!", Monnam(mon)); break;
			case 320: pline("Your 9mm rounds hit %s but only cause a flesh wound.", mon_nam(mon)); break;
			case 321: pline("You successfully land a groin attack on %s!", mon_nam(mon)); break;
			case 322: pline("Yay, you got another hit! Yay!"); break;
			case 323: pline("%s takes cover but gets clipped by your attack anyway.", Monnam(mon)); break;
			case 324: pline("Your to-hit is good enough and therefore you hit %s!", mon_nam(mon)); break;
			case 325: pline("You squeezed s %s.", mon_nam(mon)); break; /* not a spelling error --Amy */
			case 326: pline("You land a series of jiu-jitsu strikes on %s.", mon_nam(mon)); break;
			case 327: pline("%s's zipper goes up slightly.", Monnam(mon)); break;
			case 328: pline("You open one of %s's buckles.", mon_nam(mon)); break;
			case 329: pline("You start to untie %s's lacings.", mon_nam(mon)); break;
			case 330: pline("One of %s's velcro lashes is now open!", mon_nam(mon)); break;
			case 331: pline("You furiously claw at %s's skin!", mon_nam(mon)); break;
			case 332: pline("You prick %s with a needle.", mon_nam(mon)); break;
			case 333: pline("You sneeze into %s's %s.", mon_nam(mon), mbodypart(mon, FACE)); break;
			case 334: pline("You ratch over %s's back, drawing %s.", mon_nam(mon), mbodypart(mon, BLOOD)); break;
			case 335: pline("You hit %s with a full %s shot.", mon_nam(mon), mbodypart(mon, STOMACH)); break;
			case 336: pline("You poison %s with a mix of spiddal and harrada, but it's not very effective...", mon_nam(mon)); break;
			case 337: pline("You land an effective hit in %s's %s.", mon_nam(mon), mbodypart(mon, FACE)); break;
			case 338: pline("%s sustains minor lacerations from your attack.", Monnam(mon)); break;
			case 339: pline("You bonk %s's %s.", mon_nam(mon), mbodypart(mon, HEAD)); break;
			case 340: pline("You try to steal %s's virginity!", mon_nam(mon)); break;
			case 341: pline("You merely daze %s.", mon_nam(mon)); break;
			case 342: pline("%s is clipped by your wild swing, but ignores the wound and keeps fighting.", Monnam(mon)); break;
			case 343: pline("%s is heavily wounded, but not dead yet, and will keep fighting you to the last breath.", Monnam(mon)); break;
			case 344: pline("You fire your shotgun at %s, who is hit by a few pellets and gets really annoyed.", mon_nam(mon)); break;
			case 345: pline("Since you don't have anything better, you throw a pencil sharpener at %s and hit.", mon_nam(mon)); break;
			case 346: pline("You fire some prickly wooden balls at %s.", mon_nam(mon)); break;
			case 347: pline("You kick the ball and hit %s!", mon_nam(mon)); break;
			case 348: pline("%s's nerves are hurt!", Monnam(mon)); break;
			case 349: pline("%s is chilled by infernal squall!", Monnam(mon)); break;
			case 350: pline("Your psychokinetic forces damage %s's brain!", mon_nam(mon)); break;
			case 351: pline("You whack %s's knee with a glass bottle.", mon_nam(mon)); break;
			case 352: pline("You perform an underhand drop that reduces %s's gauge.", mon_nam(mon)); break;
			case 353: pline("You use a high damage attack that takes off most, but not all of %s's health.", mon_nam(mon)); break;
			case 354: pline("You shoot your graviton buster at %s.", mon_nam(mon)); break;
			case 355: pline("You shoot a dazzling light from your %s at %s, who vomits and loses weight.", body_part(EYE), mon_nam(mon)); break;
			case 356: pline("You unleashed own mana, and %s is severely hurt.", mon_nam(mon)); break;
			case 357: pline("Your polearm penetrates %s's %s!", mon_nam(mon), mbodypart(mon, STOMACH)); break;
			case 358: pline("You attempt to strike the weapon out of %s's %s!", mon_nam(mon), mbodypart(mon, HAND)); break;
			case 359: pline("You tell %s that its %s will eventually develop cancer from all the shitty smoking.", mon_nam(mon), mbodypart(mon, LUNG)); break;
			case 360: pline("You apply some tweezers to %s's %s.", mon_nam(mon), mbodypart(mon, NOSE)); break;
			case 361: pline("You place a painful little-girl kick to %s's %s.", mon_nam(mon), mbodypart(mon, LEG)); break;
			case 362: pline("You step on %s's %s with your lovely platform boots.", mon_nam(mon), makeplural(mbodypart(mon, TOE))); break;
			case 363: pline("Your soft dancing shoes land a shin kick of love on %s.", mon_nam(mon)); break;
			case 364: pline("You try to scratch %s's %s with the heel of your ankle boot. %s attempts to dodge but you manage to draw %s anyway.", mon_nam(mon), mbodypart(mon, LEG), mhe(mon), mbodypart(mon, BLOOD)); break;
			case 365: pline("%s's %s somehow doesn't break underneath the blow of your hammer!", mon_nam(mon), mbodypart(mon, SPINE)); break;
			case 366: pline("Your Shii-Cho lightsaber attack hits %s.", mon_nam(mon)); break;
			case 367: pline("%s is injured and calls the kops.", Monnam(mon)); break;
			case 368: pline("%s is only slightly injured by your attack.", Monnam(mon)); break;
			case 369: pline("You hit %s 5 times in a row but it's only barely scratched!", mon_nam(mon)); break;
			case 370: pline("You angrily shout that you won't be cleaning the %s shoes.", l_monnam(mon)); break;
			case 371: pline("You clamp %s's %s.", mon_nam(mon), makeplural(mbodypart(mon, FINGER))); break;
			case 372: pline("You try to crush %s's %s.", mon_nam(mon), mbodypart(mon, FINGERTIP)); break;
			case 373: pline("Your grinding machine slightly damages %s's %s.", mon_nam(mon), makeplural(mbodypart(mon, FINGERTIP))); break;
			case 374: pline("Your claws draw %s's %s, and then you pull back your claws and lick it off.", mon_nam(mon), mbodypart(mon, BLOOD)); break;
			case 375: pline("Thanks to the helping hand of %s, your inaccurate attack hits anyway!", u_gname()); break;
			case 376: pline("%s decides to be nice and guides you, allowing your attack to hit %s!", (!rn2(3) ? align_gname(A_LAWFUL) : rn2(2) ? align_gname(A_NEUTRAL) : align_gname(A_CHAOTIC)), mon_nam(mon)); break;
			case 377: pline("%s complains that you didn't address it with the correct pronoun, but you give it a big slap in the %s in retaliation.", mon_nam(mon), mbodypart(mon, FACE)); break;
			case 378: pline("You slap %s's butt a couple of times with your left %s.", mon_nam(mon), body_part(HAND)); break;
			case 379: pline("You sting your fingernails into various skin-covered body parts of %s.", mon_nam(mon)); break;
			case 380: pline("Yeah! You cursed %s! Now your next attack will deal even more damage!", mon_nam(mon)); break;
			case 381: pline("You listen to the wonderful scratching sound as your stiletto heel scratches over the full length of %s's %s.", mon_nam(mon), mbodypart(mon, LEG)); break;
			case 382: pline("The lashes of your cute velcro shoes scratch open both of %s's %s, and %s is flowing like rivers.", mon_nam(mon), makeplural(mbodypart(mon, LEG)), mbodypart(mon, BLOOD)); break;
			case 383: pline("Your sexy leather pumps continuously scratch up and down %s's %s, causing %s to squirt everywhere, but the tender heel absolutely wants to deal even more damage.", mon_nam(mon), mbodypart(mon, LEG), mbodypart(mon, BLOOD)); break;
			case 384: pline("The tender heels of your fleecy combat boots are starting to get very %s-smeared as you keep scratching over the open wounds on %s's %s.", mbodypart(mon, BLOOD), mon_nam(mon), mbodypart(mon, LEG)); break;
			case 385: pline("Your buckles slit %s's %s very painfully, but somehow no %s is flowing. Maybe %s has let it coagulate?", mon_nam(mon), mbodypart(mon, LEG), mbodypart(mon, BLOOD), mon_nam(mon)); break;
			case 386: pline("Your sneakers fully slam into %s's nuts, but not strongly enough for a knock-out.", mon_nam(mon)); break;
			case 387: pline("%s is trying to tease you with his sexy underwear, but you can't be fooled and place a powerful kick into his groin with your boots.", Monnam(mon)); break;
			case 388: pline("You stick some fly paper onto %s's %s.", mon_nam(mon), mbodypart(mon, HAIR)); break;
			case 389: pline("Your blade removes some of %s's %s.", mon_nam(mon), mbodypart(mon, HAIR)); break;
			case 390: pline("Your hit lays bare the muscles of %s's %s!", mon_nam(mon), mbodypart(mon, ARM)); break;
			case 391: pline("You kick %s's %s with your cuddly-gentle sneakers, trying to draw %s.", mon_nam(mon), makeplural(mbodypart(mon, HAND)), mbodypart(mon, BLOOD)); break;
			case 392: pline("%s tries to mock you that your stinking boots cannot hurt %s, but you prove %s wrong by scratching both of %s legs spilling %s everywhere.", Monnam(mon), mhim(mon), mhim(mon), mhis(mon), mbodypart(mon, BLOOD)); break;
			case 393: pline("You pivot and roundhouse kick %s!", mon_nam(mon)); break;
			case 394: pline("You leap up and butterfly kick %s!", mon_nam(mon)); break;
			case 395: pline("You spin around and back kick %s!", mon_nam(mon)); break;
			case 396: pline("You lightly caress %s.", mon_nam(mon)); break;
			case 397: pline("You gently stroke %s.", mon_nam(mon)); break;
			case 398: pline("Your honchos at %s are cheering in front of the TV as they see you batter %s!", urole.homebase, mon_nam(mon)); break;
			case 399: pline("You might in fact be capable of surviving at %s, seeing how you're mixing up %s right now.", urole.intermed, mon_nam(mon)); break;
			case 400: pline("You make it darker for %s!", mon_nam(mon)); break;
			case 401: pline("You miss but some guy comes and helps you."); break;
			case 402: pline("Since you ate from the hellfire devil fruit, you sling some bolts of fire at %s and hit.", mon_nam(mon)); break;
			case 403: pline("You body-check %s.", mon_nam(mon)); break;
			case 404: pline("You hit %s with a full-speed head-on collision, taking about three quarters of %s health off.", mon_nam(mon), mhis(mon)); break;
			case 405: pline("You throw a coconut at %s, hitting %s in the belly.", mon_nam(mon), mhim(mon)); break;
			case 406: pline("Your extreme body odor causes %s to choke.", mon_nam(mon)); break;
			case 407: pline("You shoot %s with a football.", mon_nam(mon)); break;
			case 408: pline("%s tries to burn you with a cigarette, but you slap %s in the %s for that.", Monnam(mon), mhim(mon), mbodypart(mon, FACE)); break;
			case 409: pline("Your purple heel sandals clear %s one.", mon_nam(mon)); break;
			case 410: pline("You ring %s's %s to punish %s for always making bad jokes.", mon_nam(mon), mbodypart(mon, HEAD), mhim(mon)); break;
			case 411: pline("Your painted fingernails painfully sting %s.", mon_nam(mon)); break;
			case 412: pline("You rip off %s's ugly face mask.", mon_nam(mon)); break;
			case 413: pline("You bite %s in the %s and suck some delicious red nectar.", mon_nam(mon), mbodypart(mon, NECK)); break;
			case 414: pline("You use your fingernails to scratch over %s's back.", mon_nam(mon)); break;
			case 415: pline("You kick %s in the shins like a little girl.", mon_nam(mon)); break;
			case 416: pline("%s manages to knock you down, but while laying prone, you perform an upward kick into %s belly!", Monnam(mon), mhis(mon)); break;
			case 417: pline("Your axe almost manages to decapitate %s, but ends up hitting %s lower body instead.", mon_nam(mon), mhis(mon)); break;
			case 418: pline("%s realizes that your hard wooden sandals can kick %s shins very painfully!", Monnam(mon), mhis(mon)); break;
			case 419: pline("Yes, that's how you do it. Well done, you actually managed to land a hit!"); break;
			case 420: pline("I've not expected you to be able to hit %s at all, but apparently you can do it!", mon_nam(mon)); break;
			case 421: pline("Your sister combat boots with very pretty black block heels stomp %s's %s.", mon_nam(mon), makeplural(mbodypart(mon, TOE))); break;
			case 422: pline("You land a very lovely little-girl kick against %s's shin with your black calf-leather sandals!", mon_nam(mon)); break;
			case 423: pline("You slam shut and %s lost lines.", mon_nam(mon)); break;
			case 424: pline("%s is caught by your homosexual spin kick, but keeps fighting.", Monnam(mon)); break;
			case 425: pline("Your jump kick catches %s, who is dazed for a moment.", mon_nam(mon)); break;
			case 426: pline("You hit %s, whose identity number is %d by the way!", mon_nam(mon), mon->m_id); break;
			case 427: pline("%s has only a movement rate of %d, so you can easily land another strike.", Monnam(mon), mon->movement); break;
			case 428: pline("%s has %d hit points remaining - but now it's a little less after your hit connected!", Monnam(mon), mon->mhp); break;
			case 429: pline("%s's special attacks are currently on a timeout of %d, so they failed to parry your strike!", Monnam(mon), mon->mspec_used); break;
			case 430: pline("%s tries to taunt you, and catches a big slap in the face from you.", Monnam(mon)); break;
			case 431: pline("%s fails to dodge your swing due to being occupied with %s smartphone!", Monnam(mon), mhis(mon)); break;
			case 432: pline("Klililililing! %s's phone rings in the middle of the fight, but you don't care and land a powerful hit.", Monnam(mon)); break;
			case 433: pline("As %s tries to dodge, %s fully steps into a heap of dog shit, and swears like a tube sparrow. You use the opportunity to slash %s with your sword.", mon_nam(mon), mhe(mon), mhim(mon)); break;
			case 434: pline("%s's butt soreness of %d has just increased by one after your palm connected with it!", Monnam(mon), mon->butthurt); break;
			case 435: pline("%s is bleeding heavily as a result of your slashing attacks. In fact, the amount of bleeding damage equals %d right now.", Monnam(mon), mon->bleedout); break;
			case 436: pline("Since you've eaten meat %ld times already, you're pretty strong now and smash %s again.", u.uconduct.unvegetarian, mon_nam(mon)); break;
			case 437: pline("Well done, your amount of weapon hits was %ld and has just increased by one again.", u.uconduct.weaphit); break;
			case 438: pline("%s starts blabbering 'Hey, my pronouns are %s and %s!' You don't want to listen to that bullshit and aim a big fat slap at its %s.", Monnam(mon), mhe(mon), mhim(mon), mbodypart(mon, FACE)); break;
			case 439: pline("You're getting angry because you only have %d points of nutrition remaining, and furiously batter %s.", u.uhunger, mon_nam(mon)); break;
			case 440: pline("You decide to speed up the fight with %s, since you feel that the monster spawn rate may well start speeding up at turn number %d.", mon_nam(mon), (u.ascensionfirsthint * 4)); break;
			case 441: pline("Quick, you need to finish off %s. After all, if you've still not ascended by turn %d, the game will become a whole lot harder.", mon_nam(mon), u.ascensionsecondhint); break;
			case 442: pline("Since your protection has been reduced by %d, you decide for a particularly offensive strategy. Neglecting your already weakened defenses, you start to mess up %s with your whirling attack.", u.negativeprotection, mon_nam(mon)); break;
			case 443: pline("Your contamination of %d powers up your melee just enough that you manage to hit %s.", u.contamination, mon_nam(mon)); break;
			case 444: pline("It was a good idea to enhance %d skills after all. Look how your blade slashes its way through %s!", u.skills_advanced, mon_nam(mon)); break;
			case 445: pline("You use the parting attack ID %d on %s, which proves reasonably effective.", u.adpartattack, mon_nam(mon)); break;
			case 446: pline("%s's light does not go any longer!", Monnam(mon)); break;
			case 447: pline("%s's chassis is damaged!", Monnam(mon)); break;
			case 448: pline("%s has a FAILURE: CLOCK, and no longer knows what time it is!", Monnam(mon)); break;
			case 449: pline("Well, at least you did hit at all, even if you didn't deal much damage."); break;
			case 450: pline("You attempt to thrust %s into the iron bars.", mon_nam(mon)); break;
			case 451: pline("You fire your water gun at %s.", mon_nam(mon)); break;
			case 452: pline("You popped one of %s's tires!", mon_nam(mon)); break;
			case 453: pline("You thwacked %s with the iron chain.", mon_nam(mon)); break;
			case 454: pline("You fire your FLUDD at %s, which doesn't seem to be very effective.", mon_nam(mon)); break;
			case 455: pline("%s is nothing but a hounddog, just crying all the time.", Monnam(mon)); break;
			case 456: pline("%s lost a little bit of health, which %s has to regenerate first, so please stop attacking for a while.", Monnam(mon), mhe(mon)); break;
			case 457: pline("Your research on %s's weaknesses pays off.", mon_nam(mon)); break;
			case 458: pline("You're a firm believer in equal opportunity sexism, and therefore counter %s's previous sexist remark with your own one.", mon_nam(mon)); break;
			case 459: pline("You land a reasonably good hit against %s.", mon_nam(mon)); break;
			case 460: pline("%s tries to use the madeleine strategy but gets hit by you anyway.", Monnam(mon)); break;
			case 461: pline("%s made the mistake to stand still rather than dodging your blow.", Monnam(mon)); break;
			case 462: pline("Yes, keep going, you might be able to defeat %s yet!", mon_nam(mon)); break;
			case 463: pline("You use the frenzy attack, meaning this hit dealt more damage to %s than your last one.", mon_nam(mon)); break;
			case 464: pline("You batter %s with your branch.", mon_nam(mon)); break;
			case 465: pline("Your weapon hits %s, but its enchantment level is too low to kill %s in one hit.", mon_nam(mon), mhim(mon)); break;
			case 466: pline("Your weapon skill isn't high enough to kill %s outright, but at least you have some weapon skill at all, otherwise you would have missed entirely.", mon_nam(mon)); break;
			case 467: pline("You spank %s's ructious little ass.", mon_nam(mon)); break;
			case 468: pline("You punch and kick %s.", mon_nam(mon)); break;
			case 469: pline("%s's game lags, causing %s evade command to not execute, and therefore your attack hits.", Monnam(mon), mhis(mon)); break;
			case 470: pline("After your slap hits %s, %s starts to cry like a little girl.", mon_nam(mon), mhe(mon)); break;
			case 471: pline("%s intentionally allows your weapon to hit.", Monnam(mon)); break;
			case 472: pline("It seems that %s is a masochist, just standing there receiving your melee hits.", mon_nam(mon)); break;
			case 473: pline("You offer a shin kick of love to %s with your very cuuuuute girl platform boots.", mon_nam(mon)); break;
			case 474: pline("You park your lovely black sandals on %s's %s.", mon_nam(mon), mbodypart(mon, HAND)); break;
			case 475: pline("%s grumbles incessantly after being kicked by your boots, complaining that you should have cleaned the dog shit from your treaded soles.", Monnam(mon)); break;
			case 476: pline("Lightning strikes %s!", mon_nam(mon)); break;
			case 477: pline("Your fiery weapon burns %s!", mon_nam(mon)); break;
			case 478: pline("The ice-cold blade chills %s!", mon_nam(mon)); break;
			case 479: pline("Your artifact weapon douses %s with acid!", mon_nam(mon)); break;
			case 480: pline("You activate Adrenaline Rush mode and powerfully punch %s.", mon_nam(mon)); break;
			case 481: pline("You are playing with quaractar rules, and rolled a %d which means you land a moderately strong hit.", rn1(4, 2)); break;
			case 482: pline("You strike the disturbed %s one down.", l_monnam(mon)); break;
			case 483: pline("You took off just barely more HP from %s than %s can regenerate per turn.", mon_nam(mon), mhe(mon)); break;
			case 484: pline("You hit %s, and now %s is barely scratched.", mon_nam(mon), mhe(mon)); break;
			case 485: pline("You hit %s, and now %s is slightly injured.", mon_nam(mon), mhe(mon)); break;
			case 486: pline("You hit %s, and now %s is injured.", mon_nam(mon), mhe(mon)); break;
			case 487: pline("You hit %s, and now %s is heavily injured.", mon_nam(mon), mhe(mon)); break;
			case 488: pline("You hit %s, and now %s is critically injured.", mon_nam(mon), mhe(mon)); break;
			case 489: pline("You hit %s, and now %s is almost dead.", mon_nam(mon), mhe(mon)); break;
			case 490: pline("You hit %s, and now %s is damaged.", mon_nam(mon), mhe(mon)); break;
			case 491: pline("You hit %s, and now %s is burning.", mon_nam(mon), mhe(mon)); break;
			case 492: pline("%s was already critically injured, and after this hit %s's still critically injured.", Monnam(mon), mhe(mon)); break;
			case 493: pline("You concentrate for once, and thus manage to hit %s despite your bad to-hit rating.", mon_nam(mon)); break;
			case 494: pline("Thankfully you have a flail, so you can actually hit %s.", mon_nam(mon)); break;
			case 495: pline("You hit %s even though your weapon is cursed, but it's rather ineffective.", mon_nam(mon)); break;
			case 496: pline("You keep battering %s.", mon_nam(mon)); break;
			case 497: pline("%s made another error.", Monnam(mon)); break;
			case 498: pline("%s injury - that should slow %s down!", mbodypart(mon, LEG), mon_nam(mon)); break;
			case 499: pline("%s stepped into your caltrop and is slower now.", Monnam(mon)); break;
			case 500: pline("You attack several times but because of the stupid combo system %s still has some health remaining!", mon_nam(mon)); break;
			case 501: pline("Your button mashing strategy lets you land a series of hits on %s.", mon_nam(mon)); break;
			case 502: pline("%s is wounded by your grenade's explosion.", Monnam(mon)); break;
			case 503: pline("You hit %s with some of your multiple attacks but not with all of them.", mon_nam(mon)); break;
			case 504: pline("You spill your coffee mug on %s's shirt.", mon_nam(mon)); break;
			case 505: pline("You rush %s and make a scratch.", mon_nam(mon)); break;
			case 506: pline("You drain a level from %s, who becomes an easier target.", mon_nam(mon)); break;
			case 507: pline("%s touched your fairy floss ring and is now small.", Monnam(mon)); break;
			case 508: pline("%s lost a life, but it wasn't the last one!", Monnam(mon)); break;
			case 509: pline("You rip off a button from %s's jacket.", mon_nam(mon)); break;
			case 510: pline("%s has to lick %s wounds for a bit after your attack.", Monnam(mon), mhis(mon)); break;
			case 511: pline("Your paddle spanks %s's ass a little.", mon_nam(mon)); break;
			case 512: pline("%s seems to enjoy being flogged by your riding crop.", Monnam(mon)); break;
			case 513: pline("%s is now infected with your corona viri, and becomes physically weaker.", Monnam(mon)); break;
			case 514: pline("You threw a cursed potion that reduces %s's stats.", mon_nam(mon)); break;
			case 515: pline("You rip off %s's helmet, so you can remove %s skull with your next attack!", mon_nam(mon), mhis(mon)); break;
			case 516: pline("Your stone edge attack scrapes off a maximum hit point from %s's total.", mon_nam(mon)); break;
			case 517: pline("You push %s through the area with your very fluffy (or fleecy) combat boot heels.", mon_nam(mon)); break;
			case 518: pline("In this story, %s is often bashed by %s.", mon_nam(mon), playeraliasname); break;
			case 519: pline("The tip of your poleaxe just barely hits %s.", mon_nam(mon)); break;
			case 520: pline("You start squirting your pee into %s's feminine dancing shoes.", mon_nam(mon)); break;
			case 521: pline("You attempt to rip the soft leather of %s's women's shoes to shreds even though they look so lovely.", mon_nam(mon)); break;
			case 522: pline("You start farting at %s's lovely female sneakers.", mon_nam(mon)); break;
			case 523: pline("You shoot a smoke cloud at %s, who can't move for a while now.", mon_nam(mon)); break;
			case 524: pline("You use a very slow attack, but %s is too stupid to dodge.", mon_nam(mon)); break;
			case 525: pline("You use a very fast attack, and %s tries to dodge but fails.", mon_nam(mon)); break;
			case 526: pline("You fling a bunch of zorkmids at %s and hit.", mon_nam(mon)); break;
			case 527: pline("You hit %s with your thrown hammer, but %s survived and stands on the hammer now so you can't easily get it back!", mon_nam(mon), mhe(mon)); break;
			case 528: pline("Your thrown lightsaber hits %s, but %s starts using it against you now because it didn't come back!", mon_nam(mon), mhe(mon)); break;
			case 529: pline("Your thrown lightsaber hits %s, but %s survived so you should throw again!", mon_nam(mon), mhe(mon)); break;
			case 530: pline("You hit %s with a superpowered attack that should have killed %s outright, but %s has the annoying property to take reduced damage and therefore survived!", mon_nam(mon), mhim(mon), mhe(mon)); break;
			case 531: pline("You hit %s's %ss with your cane a couple of times.", mon_nam(mon), mbodypart(mon, FINGER)); break;
			case 532: pline("%s is hit by your jumping flamer and lost a whole bunch of health.", Monnam(mon)); break;
			case 533: pline("%s caught your rocket with %s body, but still has some HP remaining!", Monnam(mon), mhis(mon)); break;
			case 534: pline("You were sure that %s would die with your next hit, but apparently not!", mon_nam(mon)); break;
			case 535: pline("You blast %s with an elemental spell, which %s turns out to be highly resistant to.", mon_nam(mon), mhe(mon)); break;
			case 536: pline("You throw a potion of poison at %s, who is now green.", mon_nam(mon)); break;
			case 537: pline("%s starts to lose some %s after your attack, and you can't wait to attack again because you want to make %s suffer.", Monnam(mon), mbodypart(mon, BLOOD), mhim(mon)); break;
			case 538: pline("%s starts to lose some %s after your attack, and you are shocked because you didn't want to hurt %s that badly.", Monnam(mon), mbodypart(mon, BLOOD), mhim(mon)); break;
			case 539: pline("Your platform girl boots kick %s in the shins very painfully, but you hope that %s fights back and defeats you.", mon_nam(mon), mhe(mon)); break;
			case 540: pline("Your platform girl boots kick %s in the shins very painfully, and now %s has to grapple with the intense pain.", mon_nam(mon), mhe(mon)); break;
			case 541: pline("%s simply stands there and stops the kick of your female block-heeled boot with his nuts, which he really seems to enjoy.", Monnam(mon)); break; /* no mhe */
			case 542: pline("You get the feeling that %s is new to the game, and decide to not be so rough so you only lightly hit %s with your weapon.", mon_nam(mon), mhim(mon)); break;
			case 543: pline("You hit the opponent."); break;
			case 544: pline("Yet another hit."); break;
			case 545: pline("The opponent loses health."); break;
			case 546: pline("The opponent has less hit points remaining."); break;
			case 547: pline("Yes, that was a hit."); break;
			case 548: pline("Almost missed, but no, you landed a hit."); break;
			case 549: pline("You throw a used FFP-2 mask at %s, who starts screaming like a mad%sman.", mon_nam(mon), mon->female ? "wo" : ""); break;
			case 550: pline("Oh no, you just realized that %s is a self-harmer who has started to hurt %sself with a dangerously sharp-edged object!", mon_nam(mon), mhim(mon)); break;
			case 551: pline("Since your strength is only %d, %s survives the hit.", ACURR(A_STR), mon_nam(mon)); break;
			case 552: pline("Good thing you raised your dexterity to %d, so you reliably hit %s.", ACURR(A_DEX), mon_nam(mon)); break;
			case 553: pline("%s loses a chunk of health, but gains a bunch of gauge!", Monnam(mon)); break;
			case 554: pline("%s is almost dead, but has a full gauge meter so you need to be wary of %s counterattack!", Monnam(mon), mhis(mon)); break;
			case 555: pline("You sprinkle the soy sauce to %s of %s as mise en place, 'Stop...'", mbodypart(mon, HEAD), mon_nam(mon)); break;
			case 556: pline("You sprinkle the pepper to %s as mise en place, 'Achoo!'", mon_nam(mon)); break;
			case 557: pline("You dusted the sugar to %s as mise en place, 'Sweet!'", mon_nam(mon)); break;
			case 558: pline("You rubbed into %s the salt as mise en place... %s is stumped.", mhim(mon), Monnam(mon)); break;
			case 559: pline("You painfully slam your girl peep-toes into %s's shins.", mon_nam(mon)); break;
			case 560: pline("You throw a high potion of soma, and %s takes a lot of sound damage.", mon_nam(mon)); break;
			case 561: pline("You throw a high potion of disaster, permanently damaging %s's stats.", mon_nam(mon)); break;
			case 562: pline("%s triggers a nasty trap and should be easier to defeat now, because %s has to deal with an annoying interface screw!", Monnam(mon), mhe(mon)); break;
			case 563: pline("Your %snails pinch %s.", body_part(FINGER), mon_nam(mon)); break;
			case 564: pline("%s loses one single drop of blood as a result of your stinging attack.", Monnam(mon)); break;
			case 565: pline("You use your high heels as a blunt weapon to batter %s's bones.", mon_nam(mon)); break;
			case 566: pline("You use your high heels as a sharp weapon to remove the skin from %s's %s.", mon_nam(mon), mbodypart(mon, LEG)); break;
			case 567: pline("Your weapon hits %s, but since it's so dull, it only deals a single point of damage.", mon_nam(mon)); break;
			case 568: pline("Your d20 only rolled a 2, so %s loses very little health.", mon_nam(mon)); break;
			case 569: pline("%s's %s look quite attractive, but you can't be fooled and land a hit.", Monnam(mon), pantsdescription(mon)); break;
			case 570: pline("You pull down %s's %s, which means that %s butt cheeks are unprotected now.", mon_nam(mon), pantsdescription(mon), mhis(mon)); break;
			case 571: pline("Regular hit."); break;
			case 572: pline("That hit seems to have been relatively effective."); break;
			case 573: pline("%s's heel is slightly damaged!", Monnam(mon)); break;
			case 574: pline("Shreds of %s's leather are whirling through the air!", mon_nam(mon)); break;
			case 575: pline("You grab %s and squeeze %s.", mon_nam(mon), mhim(mon)); break;
			case 576: pline("It'll take a long time with that weapon, but at least you hit %s at all.", mon_nam(mon)); break;
			case 577: pline("%s's morale seems to be dropping, so you should keep attacking!", Monnam(mon)); break;
			case 578: pline("You keep getting past %s's defenses, so %s will probably surrender soon!", mon_nam(mon), mhe(mon)); break;
			case 579: pline("You keep bashing away at %s.", mon_nam(mon)); break;
			case 580: pline("Yay, %s is almost dead now! One more hit and %s should drop dead!", mon_nam(mon), mhe(mon)); break;
			case 581: pline("Your character plays exactly like a samurai, and therefore your melee weapon keeps hitting %s.", mon_nam(mon)); break;
			case 582: pline("You whack the annoying %s.", l_monnam(mon)); break;
			case 583: pline("The stability of %s's cone heels is decreasing!", mon_nam(mon)); break;
			case 584: pline("Well done! One more hit and %s's wedge heels should crumble!", mon_nam(mon)); break;
			case 585: pline("Even though %s is wearing thick platform soles, you keep landing effective hits.", mon_nam(mon)); break;
			case 586: pline("Annoyedly, you fully bash into %s's %s, dealing quite some damage.", mon_nam(mon), mbodypart(mon, FACE)); break;
			case 587: pline("As %s looks at the health bar, %s notices that the displayed value has decreased!", mon_nam(mon), mhe(mon)); break;
			case 588: pline("%s's health is in the red now! Keep attacking!", Monnam(mon)); break;
			case 589: pline("Man, you were so sure that %s would die from that hit, but somehow %s still has a little sliver of health left!", mon_nam(mon), mhe(mon)); break;
			case 590: pline("Oh come on, %s should be dead by now, considering how many hits you've landed!", mon_nam(mon)); break;
			case 591: pline("It seems that your hits are always dealing just 1 damage to %s, otherwise this fight wouldn't be taking so long.", mon_nam(mon)); break;
			case 592: pline("You deal a nonspecified amount of damage to %s, and hope it's enough to overcome %s health regeneration rate.", mon_nam(mon), mhis(mon)); break;
			case 593: pline("You intentionally use a weaker attack on %s to ensure that %s doesn't die.", mon_nam(mon), mhe(mon)); break;
			case 594: pline("%s is driven back by your blow.", Monnam(mon)); break;
			case 595: pline("At last, you landed a hit. %s will require several more of those if you want to win this encounter, though.", Monnam(mon)); break;
			case 596: pline("%s was shot but didn't die!", Monnam(mon)); break;
			case 597: pline("%s decides to eat a mochi, but suddenly starts choking. You perform the Heimlich maneuver and bash %s up with full force, which causes the mochi to rocket back out of %s mouth, and %s survives with slight injuries.", Monnam(mon), mhim(mon), mhis(mon), mhe(mon)); break;
			case 598: pline("%s is stupid enough to allow you to deal a sharp cut to %s with your %snails.", Monnam(mon), mhim(mon), body_part(TOE)); break;
			case 599: pline("%s steps on your %s, but didn't expect you to violently retaliate with your %snails, and a lot of %s is flowing!", Monnam(mon), body_part(FOOT), body_part(TOE), mbodypart(mon, BLOOD)); break;
			case 600: pline("%s stands in front of you in such a way that you can easily kick %s in the delicate balls, and you use that opportunity.", Monnam(mon), mhim(mon)); break;
			case 601: pline("%s begs you to slam your massive high heels on %s %s, and so you do that, which seems to be very painful.", Monnam(mon), mhis(mon), mbodypart(mon, HEAD)); break;
			case 602: pline("You use the chance to dance on %s's %s with your pretty cone heels for a bit.", mon_nam(mon), makeplural(mbodypart(mon, FINGER)) ); break;
			case 603: pline("You bite %s because you fight like a woman.", mon_nam(mon)); break;
			case 604: pline("You don't feel like fighting fair, and so you claw and scratch %s furiously.", mon_nam(mon)); break;
			case 605: pline("You strike on %s like a dust bowl.", mon_nam(mon)); break;
			case 606: pline("Your kick powerfully slams against %s's knee.", mon_nam(mon)); break;
			case 607: pline("%s tries to threaten you with a knife, but you use a skillful fighting move, and suddenly you are holding the knife in your %s, ready to attack %s with it!", Monnam(mon), body_part(HAND), mhim(mon)); break;
			case 608: pline("%s complains '%s %s has attacked me!'", Monnam(mon), urace.noun, playeraliasname); break;
			case 609: pline("%s's chin is hit by your fist.", Monnam(mon)); break;
			case 610: pline("%s failed to predict your strike and gets hit!", Monnam(mon)); break;
			case 611: pline("One of %s's bones breaks, so %s should be an easier target now!", mon_nam(mon), mhe(mon)); break;
			case 612: pline("%s starts leaking!", Monnam(mon)); break;
			case 613: pline("%s looks quite a bit more damaged now.", Monnam(mon)); break;
			case 614: pline("You feel that you've landed a reasonably effective hit."); break;
			case 615: pline("%s survives the explosion of your grenade, taking only slight damage.", Monnam(mon)); break;
			case 616: pline("%s survives what should have been a deadly hit!", Monnam(mon)); break;
			case 617: pline("Oh shit, %s survived the attack and is really angry now.", mon_nam(mon)); break;
			case 618: pline("You foul %s, who is dazed for a moment.", mon_nam(mon)); break;
			case 619: pline("You foul %s, who falls to the floor but quickly gets up again!", mon_nam(mon)); break;
			case 620: pline("You're playing on a high difficulty level, but you do manage to successfully tackle %s.", mon_nam(mon)); break;
			case 621: pline("%s trips over a stone and is injured even without you having to attack at all!", Monnam(mon)); break;
			case 622: pline("The details of the attack you just used would be too disturbing, so we're leaving that bit out, but nonetheless %s survived.", mon_nam(mon)); break;
			case 623: pline("Well, you landed a good hit with your right %s there, keep going...", body_part(HAND)); break;
			case 624: pline("And a powerful hit with the left %s! A few more of those and this fight should be over!", body_part(HAND)); break;
			case 625: pline("And left! And right! %s keeps taking hits!", Monnam(mon)); break;
			case 626: pline("You wield a formidable %s as your weapon of choice, and land a series of strikes on %s.", playerweaponname(), mon_nam(mon)); break;
			case 627: pline("You counter %s's lightning-fast strikes, parry %s relentless onslaught with your %s, and counter with your own calculated blows.", mon_nam(mon), mhis(mon), playerweaponname()); break;
			case 628: pline("Your %s knocks %s down below half health.", playerweaponname(), mon_nam(mon)); break;
			case 629: pline("You bite a large chunk out of %s's %s. Repeat that a few times and %s body should stop functioning.", mon_nam(mon), mbodypart(mon, FACE), mhis(mon)); break;
			case 630: pline("You thrust %s against a metal bar, but %s refuses to succumb to that attack!", mon_nam(mon), mhe(mon)); break;
			case 631: pline("You land an effective hit in %s's %s, but when you want to follow up with a finishing move, some stupid sjw woman goes in between, separating you from %s even though you would have killed the asshole!", mon_nam(mon), mbodypart(mon, FACE), mon_nam(mon)); break;
			case 632: pline("%s seems to enjoy the pain you cause to %s.", Monnam(mon), mhim(mon)); break;
			case 633: pline("You discover that %s apparently gets all hot when you step on %s %s with your stiletto heels. What a sicko.", mon_nam(mon), mhis(mon), makeplural(mbodypart(mon, FINGER)) ); break;
			case 634: pline("%s tries to grope you, so you slide your women's shoes over %s %s and open a %sy wound, not knowing that the fetishist was hoping you'd do exactly that.", Monnam(mon), mhis(mon), mbodypart(mon, HAND), mbodypart(mon, BLOOD)); break;
			case 635: pline("The sexual predator of %s is constantly groping you, so you slap and kick him, but he doesn't seem deterred...", mon_nam(mon)); break; /* not "mhim" */
			case 636: pline("You try to break the choke hold that %s is using on you by biting %s in the %s.", mon_nam(mon), mhim(mon), mbodypart(mon, ARM)); break;
			case 637: pline("You use your teeth to deal an ugly, festering wound to %s.", mon_nam(mon)); break;
			case 638: pline("Your bite ripped off plenty of skin from %s!", mon_nam(mon)); break;
			case 639: pline("As you pull back your teeth from %s's lower %s, %s is bleeding profusely.", mon_nam(mon), mbodypart(mon, LEG), mhe(mon)); break;
			case 640: pline("You bite %s viciously but suddenly taste %s, and hurriedly stop biting %s because you're not a vampire.", mon_nam(mon), mbodypart(mon, BLOOD), mhim(mon)); break;
			case 641: pline("Even though you slashed your blade across %s's body and %s's bleeding all over, %s doesn't stop fighting!", mon_nam(mon), mhe(mon), mhe(mon)); break;
			case 642: pline("Your heavy hammer smashed %s's skull so powerfully that you expected %s to drop dead, but somehow %s survived!", mon_nam(mon), mhim(mon), mhe(mon)); break;
			case 643: pline("You smashed %s in the %s with a force that should have knocked %s lights out, but %s narrowly deflected the blow and is only seriously injured.", mon_nam(mon), mbodypart(mon, FACE), mhis(mon), mhe(mon)); break;
			case 644: {

				strcpy(buf, mon_nam(mon)); /* stupid bug is stupid --Amy */

				pline("%s is knocked out, and the referee starts counting, 'One! Two! Three!' But when he reaches 7, %s gets back up and the fight continues.", Monnam(mon), buf); break;
			}
			case 645: pline("You cherry-tap %s with your super wimpy weapon while your character is underwater, abusing the fact that monsters can't fight back from dry land like the filthy exploit abuser you are.", mon_nam(mon)); break;
			case 646: pline("Since you have the time stop spell active, you automatically hit %s.", mon_nam(mon)); break;
			case 647: pline("You're currently at the 'whack' part of your 'alternate between move and whack' kiting strategy that you're employing against %s.", mon_nam(mon)); break;

			default: pline("You hit %s!", mon_nam(mon)); break;
	
			}

		} else You("%s %s%s", Role_if(PM_BARBARIAN) ? "smite" : "hit",
			 mon_nam(mon), canseemon(mon) ? exclam(tmp) : ".");
	}

	if (burnmsg) {
	  /* A chance of setting the monster's */
	  /* armour + equipment on fire */
	  /* (this does not do any extra damage) */

	  if (!Blind) {
	    Your("%s %s %s.", xname(obj),
		 (mon->data == &mons[PM_WATER_ELEMENTAL]) ?
		 "vaporizes part of" : "burns", mon_nam(mon));
	  }

	  if (!rn2(33)) burnarmor(mon);
	    if (!rn2(33)) 
	      (void)destroy_mitem(mon, POTION_CLASS, AD_FIRE);
	    if (!rn2(33)) 
	      (void)destroy_mitem(mon, SCROLL_CLASS, AD_FIRE);
	    if (!rn2(50)) 
	      (void)destroy_mitem(mon, SPBOOK_CLASS, AD_FIRE);

	  if (!(obj && (obj->otyp == FLAMETHROWER) )) {

		  if (mon->data == &mons[PM_WATER_ELEMENTAL]) {
		    if (!Blind) {
		      Your("%s goes out.", xname(obj));
		    }
		    end_burn(obj, TRUE);
		  }
		  else {
		    /* use up the torch more quickly */	    
		    burn_faster(obj, 1);
		}	
	  }
	}
	
	if (silvermsg) {
		const char *fmt;
		char *whom = mon_nam(mon);
		char silverobjbuf[BUFSZ];

		if (canspotmon(mon)) {
		    if (barehand_silver_rings == 1)
			fmt = "Your silver ring sears %s!";
		    else if (barehand_silver_rings == 2)
			fmt = "Your silver rings sear %s!";
		    else if (silverobj && saved_oname[0]) {
		    	sprintf(silverobjbuf, "Your %s%s %s %%s!",
		    		strstri(saved_oname, "silver") ?
					"" : "silver ",
				saved_oname, vtense(saved_oname, "sear"));
		    	fmt = silverobjbuf;
		    } else
			fmt = "The silver sears %s!";
		} else {
		    *whom = highc(*whom);	/* "it" -> "It" */
		    fmt = "%s is seared!";
		}
		/* note: s_suffix returns a modifiable buffer */
		if (!noncorporeal(mdat))
		    whom = strcat(s_suffix(whom), " flesh");
		pline(fmt, whom);
	}

	if (vivaobj) pline("The irradiation severely hurts %s!", mon_nam(mon));
	if (inkaobj) pline("The inka string hurts %s!", mon_nam(mon));
	if (odorobj) pline("The odor beguils %s!", mon_nam(mon));
	if (copperobj) pline("The copper decomposes %s!", mon_nam(mon));
	if (mercurialobj) pline("The mercury poisons %s!", mon_nam(mon));
	if (platinumobj) pline("The platinum smashes %s!", mon_nam(mon));
	if (cursedobj) pline("A black aura blasts %s!", mon_nam(mon));

	if (needpoismsg) {
		pline_The("poison doesn't seem to affect %s.", mon_nam(mon));
	}
	if (poiskilled) {
		pline_The("poison was deadly...");
		if (!already_killed) xkilled(mon, 0);
		return FALSE;
/* For vamps */
        } else if (vapekilled) {
                if (cansee(mon->mx, mon->my))
                    pline("%s%ss body vaporizes!", Monnam(mon),
                            canseemon(mon) ? "'" : "");                
                if (!already_killed) xkilled(mon, 2);
		return FALSE;
	} else if (destroyed) {
		if (!already_killed) {

			if (Role_if(PM_HUSSY) && !mon->female && !is_neuter(mon->data)) {
				u.hussykillturn = (moves + 1000);
			}

			if ((Role_if(PM_SPACEWARS_FIGHTER) || (mon->data->msound == MS_COMBAT) || (youmonst.data->msound == MS_COMBAT) || Role_if(PM_CAMPERSTRIKER) || Role_if(PM_HUSSY) || Role_if(PM_GANG_SCHOLAR) || FemtrapActiveAriane || Role_if(PM_WALSCHOLAR) || ishaxor || Hallucination || (u.usanity > rn2(1000)) ) && !thrown && !rn2(20) && (PlayerHearsSoundEffects) ) {

				switch (rnd(8)) {
	
				case 1: pline("Skjuaaaaa!"); break;
				case 2: pline("Baemmmmm!"); break;
				case 3: pline("Aeiaeaeaeaeaeae!"); break;
				case 4: pline("Baemmaemmaemmaemmaemmaemmaeaeaeaeae!"); break;
				case 5: pline("Fooooooouch!"); break;
				case 6: pline("Woepp!"); break;
				case 7: pline("Paaaaaaaaaund!"); break;
				case 8: pline("Baeau!"); break;
				}

			}

			else if ((Role_if(PM_SPACEWARS_FIGHTER) || (mon->data->msound == MS_COMBAT) || (youmonst.data->msound == MS_COMBAT) || Role_if(PM_CAMPERSTRIKER) || Role_if(PM_HUSSY) || Role_if(PM_GANG_SCHOLAR) || FemtrapActiveAriane || Role_if(PM_WALSCHOLAR) || ishaxor || Hallucination || (u.usanity > rn2(1000)) ) && (!rn2(5) || (mon->data->msound == MS_COMBAT) || (youmonst.data->msound == MS_COMBAT) ) && !thrown) {

				switch (rnd(773)) {

				case 1: pline("You crush %s's skull into jelly.", mon_nam(mon)); break;
				case 2: pline("You decapitate %s with a backhand stroke.", mon_nam(mon)); break;
				case 3: pline("You skewer %s, which collapses, spitting %s.", mon_nam(mon), mbodypart(mon,BLOOD)); break;
				case 4: pline("You disembowel %s with a final cut.", mon_nam(mon)); break;
				case 5: pline("You slash through %s's throat with a neat lunge and slice.", mon_nam(mon)); break;
				case 6: pline("You impale %s with a lightning stroke.", mon_nam(mon)); break;
				case 7: pline("You crush %s, splintering bone and spraying gore.", mon_nam(mon)); break;
				case 8: pline("You hit %s in the %s!", mon_nam(mon), mbodypart(mon,HEAD)); break;
				case 9: pline("You rip into %s with maniacal fervor.", mon_nam(mon)); break;
				case 10: pline("You cleave %s wide open!", mon_nam(mon)); break;
				case 11: pline("Your mighty blow smashes through %s's scales!", mon_nam(mon)); break;
				case 12: pline("Your weapon lays open %s, which suddenly recognizes its own mortality.", mon_nam(mon)); break;
				case 13: pline("You deal %s a crushing blow!", mon_nam(mon)); break;
				case 14: pline("You chop open %s's chest, splintering ribs and shredding viscera.", mon_nam(mon)); break;
				case 15: pline("You hit %s in the chest!", mon_nam(mon)); break;
				case 16: pline("You deal %s a final murderous cut.", mon_nam(mon)); break;
				case 17: pline("You pound %s until it stops moving.", mon_nam(mon)); break;
				case 18: pline("You crush %s's %s into useless pulp.", mon_nam(mon), mbodypart(mon,SPINE)); break;
				case 19: pline("You defeat %s with a super impact.", mon_nam(mon)); break;
				case 20: pline("You skewer %s, who is dead now, spitting %s.", mon_nam(mon), mbodypart(mon,BLOOD)); break;
				case 21: pline("You smash %s's throat and it's dead at last.", mon_nam(mon)); break;
				case 22: pline("You give %s a beating, who is kay-oh now.", mon_nam(mon)); break;
				case 23: pline("You won against %s!", mon_nam(mon)); break;
				case 24: pline("You skewer %s, who is k.o. now, spitting %s.", mon_nam(mon), mbodypart(mon,BLOOD)); break;
				case 25: pline("You flatten %s's scales.", mon_nam(mon)); break;
				case 26: pline("You smash %s's throat and it's k.o. at last.", mon_nam(mon)); break;
				case 27: pline("Impaled! %s has finally been removed!", Monnam(mon)); break;
				case 28: pline("You hit %s, making it unable to fight on, so you can proceed.", mon_nam(mon)); break;
				case 29: pline("You quickly get past %s.", mon_nam(mon)); break;
				case 30: pline("You smashed through %s's scales!", mon_nam(mon)); break;
				case 31: pline("You defeat %s which thought it was immortal.", mon_nam(mon)); break;
				case 32: pline("You fully finished off %s.", mon_nam(mon)); break;
				case 33: pline("You smash through %s's hide and it's finally finished off.", mon_nam(mon)); break;
				case 34: pline("You smash through %s's hide.", mon_nam(mon)); break;
				case 35: pline("Pounded! %s is removed now.", Monnam(mon)); break;
				case 36: pline("You quickly run past %s.", mon_nam(mon)); break;
				case 37: pline("You defeat %s with a super ramming.", mon_nam(mon)); break;
				case 38: pline("%s did not make it!", Monnam(mon)); break;
				case 39: pline("You ran over %s which is finally dead.", mon_nam(mon)); break;
				case 40: pline("You rammed %s down an abyss.", mon_nam(mon)); break;
				case 41: pline("You fully roll over %s.", mon_nam(mon)); break;
				case 42: pline("You remove %s from your roadway.", mon_nam(mon)); break;
				case 43: pline("You take down %s!", mon_nam(mon)); break;
				case 44: pline("The %s has no more fuel and is out!", l_monnam(mon)); break;
				case 45: pline("Your car's machine gun shot down %s.", mon_nam(mon)); break;
				case 46: pline("You defeated %s, you baddie-type!", mon_nam(mon)); break;
				case 47: pline("You blow away %s.", mon_nam(mon)); break;
				case 48: pline("You finish off %s in such a way that it thinks its name was Gospodarik.", mon_nam(mon)); break;
				case 49: pline("You neutralize %s.", mon_nam(mon)); break;
				case 50: pline("Pounded! %s is finally removed.", Monnam(mon)); break;
				case 51: pline("You smash apart the scrap iron mobile named %s.", l_monnam(mon)); break;
				case 52: pline("You give %s a good flogging.", mon_nam(mon)); break;
				case 53: pline("You hit %s in the %s!", mon_nam(mon), mbodypart(mon,EYE)); break;
				case 54: pline("You hit %s in the %s!", mon_nam(mon), mbodypart(mon,FACE)); break;
				case 55: pline("You hit %s in the %s!", mon_nam(mon), mbodypart(mon,NECK)); break;
				case 56: pline("You hit %s in the %s!", mon_nam(mon), mbodypart(mon,SPINE)); break;
				case 57: pline("You hit %s in the %s!", mon_nam(mon), mbodypart(mon,LUNG)); break;
				case 58: pline("You hit %s in the %s!", mon_nam(mon), mbodypart(mon,NOSE)); break;
				case 59: pline("You hit %s in the %s!", mon_nam(mon), mbodypart(mon,STOMACH)); break;
				case 60: pline("You give %s a free %s-lift.", mon_nam(mon), mbodypart(mon,FACE)); break;
				case 61: pline("You break %s's %s.", mon_nam(mon), mbodypart(mon,NECK)); break;
				case 62: pline("You rip out %s's %s!", mon_nam(mon), mbodypart(mon,LUNG)); break;
				case 63: pline("You smash %s's %s so hard that it explodes into a fountain of %s!", mon_nam(mon), mbodypart(mon,NOSE), mbodypart(mon,BLOOD)); break;
				case 64: pline("You ram your knee into %s's %s!", mon_nam(mon), mbodypart(mon,STOMACH)); break;
				case 65: pline("Your cut slices open %s's %s, causing %s to squirt everywhere!", mon_nam(mon), mbodypart(mon,STOMACH), mbodypart(mon,BLOOD)); break;
				case 66: pline("You show the left-%s %s who's boss!", mbodypart(mon,HANDED), l_monnam(mon)); break;
				case 67: pline("%s is so %s right now that you can easily kill it!", Monnam(mon), mbodypart(mon,LIGHT_HEADED)); break;
				case 68: pline("Now you defeated %s.", mon_nam(mon)); break;
				case 69: pline("You defeat %s which was really difficult.", mon_nam(mon)); break;
				case 70: pline("You shot down %s with your machine gun.", mon_nam(mon)); break;
				case 71: pline("You fully finish off %s because that did not watch out.", mon_nam(mon)); break;
				case 72: pline("You defeat %s who is out of commission for the time being!", mon_nam(mon)); break;
				case 73: pline("Bang, %s is shot by you", mon_nam(mon)); break;
				case 74: pline("You punched out %s's lights and defeated it.", mon_nam(mon)); break;
				case 75: pline("%s is teleported away.", Monnam(mon)); break;
				case 76: pline("You shoot down %s with missiles (which you don't have) and collect some points.", mon_nam(mon)); break;
				case 77: pline("You really hurt %s.", mon_nam(mon)); break;
				case 78: pline("Bang! You shot down %s with the machine gun.", mon_nam(mon)); break;
				case 79: pline("You show it to %s. That is defeated.", mon_nam(mon)); break;
				case 80: pline("You fully finished %s and that is dead.", mon_nam(mon)); break;
				case 81: pline("%s surrenders! What a coward!", Monnam(mon)); break;
				case 82: pline("You cracked open %s's skull!", mon_nam(mon)); break;
				case 83: pline("You mowed down %s, thus banishing it from this world.", mon_nam(mon)); break;
				case 84: pline("GOAL! Bad for the keeper!"); break;
				case 85: pline("It's a goal!"); break;
				case 86: pline("First to the post and then into the net - a supergoal."); break;
				case 87: pline("You shoot right through %s - Goal!", mon_nam(mon)); break;
				case 88: pline("%s is standing in offside, and you transformed the free kick.", Monnam(mon)); break;
				case 89: pline("Fully clean-breaded!"); break;
				case 90: pline("GOAL! I don't think the keeper saw that one."); break;
				case 91: pline("Bang, %s allows your ball to score.", mon_nam(mon)); break;
				case 92: pline("%s scores an own goal!", Monnam(mon)); break;
				case 93: pline("Seen from above, directly into the goal's center - super."); break;
				case 94: pline("You get past the super-opponent %s, which is ultra hard.", l_monnam(mon)); break;
				case 95: pline("You run right through %s.", mon_nam(mon)); break;
				case 96: pline("You run past %s.", mon_nam(mon)); break;
				case 97: pline("%s is fully cold-made by you!", Monnam(mon)); break;
				case 98: pline("You unfortunately won against %s. You don't deserve that at all!", mon_nam(mon)); break;
				case 99: pline("You finish off %s who can't do anything for the time being!", mon_nam(mon)); break;
				case 100: pline("You lambast %s, who is dead.", mon_nam(mon)); break;
				case 101: pline("You strike %s to components.", mon_nam(mon)); break;
				case 102: pline("You have finished %s.", mon_nam(mon)); break;
				case 103: pline("Boom! %s is dead.", mon_nam(mon)); break;
				case 104: pline("You fully lambasted %s. That is dead.", mon_nam(mon)); break;
				case 105: pline("You fully finish %s.", mon_nam(mon)); break;
				case 106: pline("Kablamm! %s is blown to bits.", Monnam(mon)); break;
				case 107: pline("You use your Nike(TM) sneakers to defeat the crazy %s once and for all.", l_monnam(mon)); break;
				case 108: pline("You fully showed it to %s.", mon_nam(mon)); break;
				case 109: pline("You meanwhile know how to do a ground attack, so you stomp %s.", mon_nam(mon)); break;
				case 110: pline("You baddie defeated %s!", mon_nam(mon)); break;
				case 111: pline("%s didn't stand no chance!", Monnam(mon)); break;
				case 112: pline("You bashed %s's skull.", mon_nam(mon)); break;
				case 113: pline("You clear %s from the way.", mon_nam(mon)); break;
				case 114: pline("You pound %s from the way with your superweapon so you can continue.", mon_nam(mon)); break;
				case 115: pline("Man, you sure took quite some time to defeat %s.", mon_nam(mon)); break;
				case 116: pline("You pulverize %s!", mon_nam(mon)); break;
				case 117: pline("You defeated %s even though that is quite a difficult opponent", mon_nam(mon)); break;
				case 118: pline("You bludgeon the %s nerve %s.", l_monnam(mon), mbodypart(mon,HEAD) ); break;
				case 119: pline("You smash %s right into the wall.", mon_nam(mon)); break;
				case 120: pline("You thunder %s fully into the wall.", mon_nam(mon)); break;
				case 121: pline("You quartered %s.", mon_nam(mon)); break;
				case 122: pline("You defeat the superopponent %s!", mon_nam(mon)); break;
				case 123: pline("You get past the superopponent %s.", mon_nam(mon)); break;
				case 124: pline("You unfortunately won against %s, you wimp!", mon_nam(mon)); break;
				case 125: pline("Open-cleaved! %s is kay-oh!", Monnam(mon)); break;
				case 126: pline("You steamrollered %s.", mon_nam(mon)); break;
				case 127: pline("Decapitated! %s was defeated by you.", Monnam(mon)); break;
				case 128: pline("Skewered! %s was unfortunately defeated by you.", Monnam(mon)); break;
				case 129: pline("Boom, %s surrenders for some reason.", mon_nam(mon)); break;
				case 130: pline("You make %s fully away.", mon_nam(mon)); break;
				case 131: pline("You strike %s fully together, but genuinly...", mon_nam(mon)); break;
				case 132: pline("You annihilated %s fully.", mon_nam(mon)); break;
				case 133: pline("You ended the fight against %s victoriously!", mon_nam(mon)); break;
				case 134: pline("You strike %s fully together.", mon_nam(mon)); break;
				case 135: pline("You finished off %s in such a way that it thinks its name was Graveleene.", mon_nam(mon)); break;
				case 136: pline("Bamm! %s is dead now.", Monnam(mon)); break;
				case 137: pline("Bang, %s is dead-made of course!", mon_nam(mon)); break;
				case 138: pline("%s is finally defeated now, that sure took quite some time!", Monnam(mon)); break;
				case 139: pline("You make %s so what from all...", mon_nam(mon)); break;
				case 140: pline("%s was shot!", Monnam(mon)); break;
				case 141: pline("You gun down %s fully!", mon_nam(mon)); break;
				case 142: pline("You cause %s to explode, who is dead.", mon_nam(mon)); break;
				case 143: pline("Headshot! Now %s is dead-made.", mon_nam(mon)); break;
				case 144: pline("%s suffocated at your shotgun's buckshot!", Monnam(mon)); break;
				case 145: pline("You freezed %s, defeating it.", mon_nam(mon)); break;
				case 146: pline("You fraged %s, who is defeated now.", mon_nam(mon)); break;
				case 147: pline("You blow out %s's brains.", mon_nam(mon)); break;
				case 148: pline("You shoot down %s!", mon_nam(mon)); break;
				case 149: pline("You shot %s into nirvana!", mon_nam(mon)); break;
				case 150: pline("Boooom! With a rocket you showed %s who's boss.", mon_nam(mon)); break;
				case 151: pline("You rip off %s's %s", mon_nam(mon), mbodypart(mon,HEAD) ); break;
				case 152: pline("%s stupidly looked at a house, allowing you to kill it!", Monnam(mon)); break;
				case 153: pline("You blow open %s's chest: Booooom!", mon_nam(mon)); break;
				case 154: pline("You defeat %s and violate its corpse!", mon_nam(mon)); break;
				case 155: pline("Boom! That really hurt %s!", mon_nam(mon)); break;
				case 156: pline("Now you've given it to %s.", mon_nam(mon)); break;
				case 157: pline("Whoa, you're really super! How amusing!"); break;
				case 158: pline("You make %s away, who suddenly recognizes its own mortally.", mon_nam(mon)); break;
				case 159: pline("You hurled away %s's skull.", mon_nam(mon)); break;
				case 160: pline("You make %s at the ass!", mon_nam(mon)); break;
				case 161: pline("Boooooooooom! %s exploded!", Monnam(mon)); break;
				case 162: pline("You ram %s into a cop car!", mon_nam(mon)); break;
				case 163: pline("%s is out of fuel and thus defeated!", Monnam(mon)); break;
				case 164: pline("You fatally tazered %s!", mon_nam(mon)); break;
				case 165: pline("You ram %s into the water, defeating it.", mon_nam(mon)); break;
				case 166: pline("You blew up %s with the grenade launcher.", mon_nam(mon)); break;
				case 167: pline("%s caused an accident and is out of commission!", Monnam(mon)); break;
				case 168: pline("%s rams a special-agent auto and explodes!", Monnam(mon)); break;
				case 169: pline("You smash %s's car to bits!", mon_nam(mon)); break;
				case 170: pline("You jump onto %s!", mon_nam(mon)); break;
				case 171: pline("You used the rocket launcher!"); break;
				case 172: pline("%s ran out of time.", Monnam(mon)); break;
				case 173: pline("You vaporize %s!", mon_nam(mon)); break;
				case 174: pline("%s impaled itself with its own sword.", Monnam(mon)); break;
				case 175: pline("You shot %s with the laser.", mon_nam(mon)); break;
				case 176: pline("You rip %s to pieces.", mon_nam(mon)); break;
				case 177: pline("You use your finishing move on %s.", mon_nam(mon)); break;
				case 178: pline("You struck off %s's %s.", mon_nam(mon), mbodypart(mon,HEAD) ); break;
				case 179: pline("You speared %s out of your way!", mon_nam(mon)); break;
				case 180: pline("You fraged %s because that didn't have no chance.", mon_nam(mon)); break;
				case 181: pline("%s couldn't do anything to stop you!", Monnam(mon)); break;
				case 182: pline("You do a deadly surgery on %s.", mon_nam(mon)); break;
				case 183: pline("You land a buggical hit!"); break;
				case 184: pline("You took %s apart and cleared it away.", mon_nam(mon)); break;
				case 185: pline("Bamm! %s has to go to the hospital!", Monnam(mon)); break;
				case 186: pline("%s is hurled into a wall.", Monnam(mon)); break;
				case 187: pline("You throw %s fully against the wall!", mon_nam(mon)); break;
				case 188: pline("You hurl %s away!", mon_nam(mon)); break;
				case 189: pline("You hurt %s a lot, so it will surely remember it for a long time!", mon_nam(mon)); break;
				case 190: pline("You strike out one of %s's ears.", mon_nam(mon)); break;
				case 191: pline("Head shot! %s lost!", Monnam(mon)); break;
				case 192: pline("You defeated %s with a ground attack.", mon_nam(mon)); break;
				case 193: pline("You stabbed %s.", mon_nam(mon)); break;
				case 194: pline("You broke all of %s's bones!", mon_nam(mon)); break;
				case 195: pline("You ripped %s to pieces.", mon_nam(mon)); break;
				case 196: pline("Your sword makes %s a %s shorter.", mon_nam(mon), mbodypart(mon,HEAD) ); break;
				case 197: pline("You remove %s from this world.", mon_nam(mon)); break;
				case 198: pline("You decapitate %s!", mon_nam(mon)); break;
				case 199: pline("You impale %s on your weapon.", mon_nam(mon)); break;
				case 200: pline("Your weapon slices %s in half!", mon_nam(mon)); break;
				case 201: pline("%s's %s caves in!", Monnam(mon), mbodypart(mon,HEAD)); break;
				case 202: pline("You broke %s's %s!", mon_nam(mon), mbodypart(mon,SPINE)); break;
				case 203: pline("Your weapon slices into %s's heart!", mon_nam(mon)); break;
				case 204: pline("You smashed %s's ribcage!", mon_nam(mon)); break;
				case 205: pline("You stab %s in the %s.", mon_nam(mon), mbodypart(mon,STOMACH)); break;
				case 206: pline("You cut off %s's %s.", mon_nam(mon), mbodypart(mon,HAND)); break;
				case 207: pline("You chop down on %s's shins.", mon_nam(mon)); break;
				case 208: pline("You gave %s a deep gash.", mon_nam(mon)); break;
				case 209: pline("You gave %s a gigantic bruise.", mon_nam(mon)); break;
				case 210: pline("You cut off %s's %s!", mon_nam(mon), mbodypart(mon,ARM)); break;
				case 211: pline("You cut off %s's %s!", mon_nam(mon), mbodypart(mon,LEG)); break;
				case 212: pline("You stab %s in the heart.", mon_nam(mon)); break;
				case 213: pline("You slash at %s's %s.", mon_nam(mon), mbodypart(mon,FACE)); break;
				case 214: pline("You throw %s down at the ground.", mon_nam(mon)); break;
				case 215: pline("You attempt to strangle %s.", mon_nam(mon)); break;
				case 216: pline("You grab %s's %s and twist it.", mon_nam(mon), mbodypart(mon,HEAD)); break;
				case 217: pline("You knocked out several of %s's teeth!", mon_nam(mon)); break;
				case 218: pline("You broke some of %s's ribs!", mon_nam(mon)); break;
				case 219: pline("%s spins around dizzily after your blow.", Monnam(mon)); break;
				case 220: pline("%s sputters at your tight, choking hold!", Monnam(mon)); break;
				case 221: pline("%s grunts under the force of your blows.", Monnam(mon)); break;
				case 222: pline("%s screams shrilly in fear.", Monnam(mon)); break;
				case 223: pline("%s disintegrates into a fine mist!", Monnam(mon)); break;
				case 224: pline("%s splatters all over the floor.", Monnam(mon)); break;
				case 225: pline("%s explodes into tiny chunks.", Monnam(mon)); break;
				case 226: pline("%s's bones are crushed at the force of your blow!", Monnam(mon)); break;
				case 227: pline("You squash %s into a pancake!", mon_nam(mon)); break;
				case 228: pline("Your weapons neatly slice %s into many little pieces.", mon_nam(mon)); break;
				case 229: pline("You cleave %s in half.", mon_nam(mon)); break;
				case 230: pline("%s's %s flies off in a wide trajectory.", Monnam(mon), mbodypart(mon,HEAD)); break;
				case 231: pline("%s is driven several feet into the ground under your blow!", Monnam(mon)); break;
				case 232: pline("You make %s flatly.", mon_nam(mon)); break;
				case 233: pline("You hack %s to pieces.", mon_nam(mon)); break;
				case 234: pline("%s was completely annihilated!", Monnam(mon)); break;
				case 235: pline("You make %s fully finished!", mon_nam(mon)); break;
				case 236: pline("Your dagger cuts through %s's %s!", mon_nam(mon), mbodypart(mon, NECK)); break;
				case 237: pline("You use the bastard sword to kill %s.", mon_nam(mon)); break;
				case 238: pline("You instakill %s by teleporting into it.", mon_nam(mon)); break;
				case 239: pline("You split %s's skull with your high-heeled footwear!", mon_nam(mon)); break;
				case 240: pline("Your leather peep-toes broke %s's %s!", mon_nam(mon), mbodypart(mon, LEG)); break;
				case 241: pline("You shoot %s with a death ray!", mon_nam(mon)); break;
				case 242: pline("You fire a wide-angle disintegration beam at %s!", mon_nam(mon)); break;
				case 243: pline("You invoke a mana storm and kill %s.", mon_nam(mon)); break;
				case 244: pline("By defeating %s you earned a trophy! Well done!", mon_nam(mon)); break;
				case 245: pline("%s's nuts are flattened by your combat stilettos!", Monnam(mon)); break;
				case 246: pline("Gategoal! Goalgate!"); break;
				case 247: pline("You purged the world of the filthy heretic that is %s.", mon_nam(mon)); break;
				case 248: pline("Your hammer breaks %s's %s.", mon_nam(mon), mbodypart(mon, SPINE)); break;
				case 249: pline("With the power of %s, you defeat %s!", urace.coll, mon_nam(mon)); break;
				case 250: pline("KAMEHAMEHA! %s is finished.", Monnam(mon)); break;
				case 251: pline("%s jumps out the window and plummets to death!", Monnam(mon)); break;
				case 252: pline("You push %s into a lake of lava.", mon_nam(mon)); break;
				case 253: pline("You perform a DDOS attack on %s, who disconnects from the server.", mon_nam(mon)); break;
				case 254: pline("%s gets a segmentation fault!", Monnam(mon)); break;
				case 255: pline("The %s its program crashed!", l_monnam(mon)); break;
				case 256: pline("%s encounters an access violation and is removed from the game!", Monnam(mon)); break;
				case 257: pline("%s forgot to watch its hit points and now they're empty!", Monnam(mon)); break;
				case 258: pline("%s ran out of health and tries to discuss with the game master but is ultimately eliminated.", Monnam(mon)); break;
				case 259: pline("%s gets banned for breaking the game rules!", Monnam(mon)); break;
				case 260: pline("The game is running low on memory. We're removing %s to free some space.", mon_nam(mon)); break;
				case 261: pline("Your Tsurugi of Muramasa bisects %s!", mon_nam(mon)); break;
				case 262: pline("%s catches your knife with its temple!", Monnam(mon)); break;
				case 263: pline("1-hit KO!"); break;
				case 264: pline("And the winner is... %s!", playeraliasname); break;
				case 265: pline("Dazhe tip bloka l'da pozdravlyayet vas, khar khar."); break;
				case 266: pline("Vy ne tip bloka l'da, no vy vse ravno pobedili %s!", mon_nam(mon)); break;
				case 267: pline("You send %s back to where it belongs!", mon_nam(mon)); break;
				case 268: pline("You zap %s with a wand of banishment!", mon_nam(mon)); break;
				case 269: pline("%s is sent to vanilla in order to bother players other than you!", Monnam(mon)); break;
				case 270: pline("Bzzzzzt! %s is warped to the dnethack game that's running on nethack.dank.ninja right now!", Monnam(mon)); break;
				case 271: pline("Your wand of cross-variant teleportation transfers %s to nethack.alt.org!", mon_nam(mon)); break;
				case 272: pline("You take down %s and immerse its %s in a heap of dog shit!", mon_nam(mon), mbodypart(mon, FACE)); break;
				case 273: pline("You resole %s's ass!", mon_nam(mon)); break;
				case 274: pline("You put a mine into %s's trouser pocket and wait for it to explode.", mon_nam(mon)); break;
				case 275: pline("%s fails to block your finishing move!", Monnam(mon)); break;
				case 276: pline("As it turns out, %s is the biggest weakling of this dungeon.", mon_nam(mon)); break;
				case 277: pline("You transform %s into several pieces of meat.", mon_nam(mon)); break;
				case 278: pline("You mince %s.", mon_nam(mon)); break;
				case 279: pline("%s will no longer stand in your way!", Monnam(mon)); break;
				case 280: pline("%s's evil schemes have been thwarted by you!", Monnam(mon)); break;
				case 281: pline("You use your lovely plateau boots to bludgeon %s to death.", mon_nam(mon)); break;
				case 282: pline("You send %s to where it belongs: Sovngarde.", mon_nam(mon)); break;
				case 283: pline("In your mission to purge the dungeon of all stupid monsters, you remove %s from this world.", mon_nam(mon)); break;
				case 284: pline("You separate %s's jaw from its %s, who is thrashing about spraying blood on every bystander and then dies.", mon_nam(mon), mbodypart(mon, FACE)); break;
				case 285: pline("Your hit to %s's midsection cleaves clear to its groin. The small flap of flesh cannot support its body anymore and as a result its collapses and dies.", mon_nam(mon)); break;
				case 286: pline("Your extremely sharp blade decapitates %s, who lives for three more seconds and then expires.", mon_nam(mon)); break;
				case 287: pline("With your tsurugi, you neatly split %s in half from %s to groin, and the halves slide uncomfortably down to the %s.", mon_nam(mon), mbodypart(mon, HEAD), surface(mon->mx, mon->my)); break;
				case 288: pline("There is a solid crunch as you club %s's %s and break it.", mon_nam(mon), mbodypart(mon, LEG)); break;
				case 289: pline("%s's %s snaps backward as the kneecap is crushed.", Monnam(mon), mbodypart(mon, LEG)); break;
				case 290: pline("%s deftly parries your blow with its %s, and blood gushes from its smashed %s.", Monnam(mon), mbodypart(mon, FACE), mbodypart(mon, NOSE)); break;
				case 291: pline("With a tremendous swipe, you crush %s's jaw, who spits out bone and blood as its jawbone is broken and teeth are knocked out.", mon_nam(mon)); break;
				case 292: pline("There is a sickening crack as your mallet strikes %s's %s and smashes the %s to pulp.", mon_nam(mon), mbodypart(mon, HAND), makeplural(mbodypart(mon, FINGER))); break;
				case 293: pline("Your seemingly weak blow happened to break %s's collarbone.", mon_nam(mon)); break;
				case 294: pline("With a crushing strike, you hit %s's abdomen fracturing ribs and damaging organs. %s tastes bitter copper as %s vomits %s and dies slowly.", mon_nam(mon), mhe(mon), mhe(mon), mbodypart(mon, BLOOD)); break;
				case 295: pline("Your hammer detaches %s's %s from its socket.", mon_nam(mon), mbodypart(mon, EYE)); break;
				case 296: pline("With a devastating blow, you crush %s's %s at the shoulder.", mon_nam(mon), mbodypart(mon, ARM)); break;
				case 297: pline("%s spins and falls as its %s is crushed and bone splinters blast everywhere.", Monnam(mon), mbodypart(mon, LEG)); break;
				case 298: pline("You destroy %s's brain with a mighty crunch!", mon_nam(mon)); break;
				case 299: pline("That blow really knocked the wind out of %s, who has severe trouble breathing because of the broken rib that just punctured its %s.", mon_nam(mon), mbodypart(mon, LUNG)); break;
				case 300: pline("You break %s's %s, and realize that it would live so you decide to be merciful and kill it with your next strike.", mon_nam(mon), mbodypart(mon, SPINE)); break;
				case 301: pline("%s's skull shatters under the force of your blow.", Monnam(mon)); break;
				case 302: pline("%s's hip is crushed and bone shards puncture its femoral artery, bringing a quick death.", Monnam(mon)); break;
				case 303: pline("%s spins as its %s is broken, struggles to breathe, and expires.", Monnam(mon), mbodypart(mon, NECK)); break;
				case 304: {
					strcpy(buf, mon_nam(mon));

					pline("%s stumbles as its %s and shoulder are crushed to mid-torso while bones and wreckage are driven into it. Since all of %s's internal organs are destroyed, it falls over dead.", Monnam(mon), mbodypart(mon, ARM), buf); break;
				}
				case 305: pline("You crush %s's lower torso and hip, who drops to its knees with %s gushing out of its mouth.", mon_nam(mon), mbodypart(mon, BLOOD)); break;
				case 306: pline("Your hit didn't seem like much, but the bone driven into %s's kidney begs to differ. After a few seconds, %s dies of shock.", mon_nam(mon), mon_nam(mon)); break;
				case 307: pline("You hit %s's %s from above and drive it into its torso, causing its to waddle in tearing pain before finally dying in agony.", mon_nam(mon), mbodypart(mon, HEAD)); break;
				case 308: pline("A thump to %s's chest explodes its heart. Its %s bulge in surprise as it drops to the knee and then falls over to the side, dead.", mon_nam(mon), makeplural(mbodypart(mon, EYE))); break;
				case 309: pline("Your hit to %s's %s breaks it in half. It falls forward shattering its wrist and bashing its %s against a rock, caving its skull in. %s pools around %s's corpse.", mon_nam(mon), mbodypart(mon, LEG), mbodypart(mon, HEAD), mbodypart(mon, BLOOD), mon_nam(mon)); break;
				case 310: pline("Your sledgehammer strikes %s's skull, which explodes like a pumpkin. You're covered in brain, %s and bone.", mon_nam(mon), mbodypart(mon, BLOOD)); break;
				case 311: pline("%s's %s is knocked clean off.", Monnam(mon), mbodypart(mon, HEAD)); break;
				case 312: pline("With a titanic swing you break close to 50 of %s's bones, whose body is pulverized as a result.", mon_nam(mon)); break;
				case 313: pline("You viciously skewer %s's thigh.", mon_nam(mon)); break;
				case 314: pline("Your powerful thrust rips past %s's garment, tearing into the abdominal wall.", mon_nam(mon)); break;
				case 315: pline("With a mighty thrust you strike %s's %s, piercing the skin and fracturing the bones with a snap.", mon_nam(mon), mbodypart(mon, ARM)); break;
				case 316: pline("Your terrible strike hits %s's %s, digging deep into the flesh and fracturing bones.", mon_nam(mon), mbodypart(mon, LEG)); break;
				case 317: pline("You puncture %s's waist, dislocating its hip.", mon_nam(mon)); break;
				case 318: pline("You cut off %s's %s.", mon_nam(mon), mbodypart(mon, NOSE)); break;
				case 319: pline("%s smoothly blocks your thrust with its cheek, which is shanked right through the jaw, knocking it loose. Teeth shatter and %s sprays everywhere.", mon_nam(mon), mbodypart(mon, BLOOD)); break;
				case 320: pline("The point of your weapon slides along the edge of %s's hilt, slicing as it goes and separating all the %s.", mon_nam(mon), makeplural(mbodypart(mon, FINGER))); break;
				case 321: pline("The tip of your blade strikes deep into %s's %s past armor, skin and muscle, and then you violently rip it out tearing the %s apart and puncturing an artery.", mon_nam(mon), mbodypart(mon, LEG), mbodypart(mon, LEG)); break;
				case 322: pline("Your clipped strike lands on %s's collarbone, shattering it.", mon_nam(mon)); break;
				case 323: pline("You shank %s between the ribs.", mon_nam(mon)); break;
				case 324: pline("%s's %s explodes like a stuck grape and is totally ruined.", Monnam(mon), mbodypart(mon, EYE)); break;
				case 325: pline("%s's %s is sliced with a deep gash and torn apart, hanging from its body by a flap of skin.", Monnam(mon), mbodypart(mon, ARM)); break;
				case 326: pline("You puncture %s's %s, cutting clear through the bone and ripping the flesh on either side.", mon_nam(mon), mbodypart(mon, LEG)); break;
				case 327: pline("%s's carotid artery is nicked, with arterial %s spraying out of the wound in a wide area creating a fine red mist.", Monnam(mon), mbodypart(mon, BLOOD)); break;
				case 328: pline("You stab %s's %s like a ripe melon, and are startled by the contents as %s collapses in agony.", mon_nam(mon), mbodypart(mon, STOMACH), mon_nam(mon)); break;
				case 329: pline("Your spear's point rips through %s's %s, touching the brain.", mon_nam(mon), mbodypart(mon, NOSE)); break;
				case 330: pline("You gouge a %s vessel out of %s's %s which flips around like a hose spraying %s everywhere. As %s's body hits the ground, it dies of shock and %s loss.", mbodypart(mon, BLOOD), mon_nam(mon), mbodypart(mon, LEG), mbodypart(mon, BLOOD), mon_nam(mon), mbodypart(mon, BLOOD)); break;
				case 331: pline("Everything below %s's waist goes numb as its %s is fractured by your pointy weapon.", mon_nam(mon), mbodypart(mon, SPINE)); break;
				case 332: pline("%s's body goes limp as you stab your weapon severing its %s column.", Monnam(mon), mbodypart(mon, SPINE)); break;
				case 333: pline("You rip %s's %s off the body, exposing its heart, which sprays %s in its %s. It is the last thing %s sees.", mon_nam(mon), mbodypart(mon, ARM), mbodypart(mon, BLOOD), mbodypart(mon, FACE), mon_nam(mon)); break;
				case 334: pline("You stabbed %s through the heart, who realizes that it's too late now, and dies.", mon_nam(mon)); break;
				case 335: pline("You gouge %s in the groin, making a %sy mess of intestines and genitalia.", mon_nam(mon), mbodypart(mon, BLOOD)); break;
				case 336: pline("%s keeps trying to reach and remove the shaft of your weapon from its %s, but for some reason its %s aren't responding. It ceases to care in a few moments.", Monnam(mon), mbodypart(mon, FACE), makeplural(mbodypart(mon, ARM))); break;
				case 337: pline("After your stab to the gut, %s can't manage to move anymore. It struggles to hold its intestines in while it slowly dies.", mon_nam(mon)); break;
				case 338: pline("%s's %s is sliced off by your blade. It tries to scream but only makes a gurgling rattle and dies.", Monnam(mon), mbodypart(mon, FACE)); break;
				case 339: pline("Your weapon's point doesn't stop at %s's %s socket. Any bystander standing behind %s could tell what brain tastes like.", mon_nam(mon), mbodypart(mon, EYE), mon_nam(mon)); break;
				case 340: pline("There is a clang as %s's %s strikes the ground with the %s severed. It spins around in agony spraying %s on everyone before dying.", mon_nam(mon), mbodypart(mon, HAND), mbodypart(mon, ARM), mbodypart(mon, BLOOD)); break;
				case 341: pline("%s flings its %s up in an attempt to block your attack, but they end up stripped of flesh and horribly burnt.", Monnam(mon), makeplural(mbodypart(mon, HAND))); break;
				case 342: pline("%s's %s are engulfed in energy, scorching its lower body.", Monnam(mon), makeplural(mbodypart(mon, FOOT))); break;
				case 343: pline("%s is struck full on in the %s by your blast, burning its %s to cinders.", Monnam(mon), mbodypart(mon, FACE), makeplural(mbodypart(mon, EYE))); break;
				case 344: pline("Your hellish blast encompasses %s, whose agony is unreal as it suffers serious burns on all exposed skin.", mon_nam(mon)); break;
				case 345: pline("You engulf %s's %s and render all the %s useless.", mon_nam(mon), mbodypart(mon, HAND), makeplural(mbodypart(mon, FINGER))); break;
				case 346: pline("%s's skin and %s begin to melt.", Monnam(mon), mbodypart(mon, FACE)); break;
				case 347: pline("You sear %s's %s, leaving nothing but a charred stump.", mon_nam(mon), mbodypart(mon, ARM)); break;
				case 348: pline("Your blast burns %s horribly, causing second degree burns to covered areas and more serious ones to exposed areas.", mon_nam(mon)); break;
				case 349: pline("Your blast sears %s's flesh, causing second and third degree burns all over.", mon_nam(mon)); break;
				case 350: pline("Your furnace rends %s's flesh with energy, causing hideous burns all over.", mon_nam(mon)); break;
				case 351: pline("You strike %s in the %s, which melts in an entertaining fashion. Fire erupts from its mouth as it falls over and dies soon later.", mon_nam(mon), mbodypart(mon, NECK)); break;
				case 352: pline("%s's clothing and armor is vaporized and its skin roasted off. %s's blackened skeleton and musculature flail about the battlefield for another moment before collapsing.", Monnam(mon), Monnam(mon)); break;
				case 353: pline("%s's torso is blasted open, knocking its %s and shoulders to the ground.", Monnam(mon), mbodypart(mon, HEAD)); break;
				case 354: pline("You see %s's %s instantly vaporize.", mon_nam(mon), mbodypart(mon, HEAD)); break;
				case 355: pline("%s's upper body is consumed in flames and melted. All that is left is a burnt stump on two twitching %s.", Monnam(mon), makeplural(mbodypart(mon, LEG))); break;
				case 356: pline("The heat causes %s's body to cook itself, who vomits up the slush that was its internal organs while its consciousness fades.", mon_nam(mon)); break;
				case 357: pline("%s catches fire and melts in such a way that it takes a long time to die. It screams and twitches for almost ten minutes before its body finally succumbs to the pain.", Monnam(mon)); break;
				case 358: pline("%s erupts in flame, staggering around for a few feet, before falling over dead.", Monnam(mon)); break;
				case 359: pline("%s is melted to a pile of ash and sludge.", Monnam(mon)); break;
				case 360: pline("Everything except %s's bones burn to a crisp. Its skeleton twitches before collapsing.", mon_nam(mon)); break;
				case 361: pline("There is a blinding flash, and nothing remains of %s, except perhaps a smoking pair of boots.", mon_nam(mon)); break;
				case 362: pline("Your snap connects at %s's groin, who doubles over in pain and terror.", mon_nam(mon)); break;
				case 363: pline("You viciously gouge %s, ripping chunks of skin from its %s.", mon_nam(mon), mbodypart(mon, ARM)); break;
				case 364: pline("You latch on to %s's %s, ripping flesh and bone.", mon_nam(mon), mbodypart(mon, LEG)); break;
				case 365: pline("You get under %s and trip its %s, then take the opportunity to shred the back of its thigh.", mon_nam(mon), makeplural(mbodypart(mon, LEG))); break;
				case 366: pline("You strike %s's abdomen, destroying the kidney in the process.", mon_nam(mon)); break;
				case 367: pline("%s uses its %s to block your attack, allowing you to viciously bite shredding its scalp with %s running down into its %s.", Monnam(mon), mbodypart(mon, HEAD), mbodypart(mon, BLOOD), makeplural(mbodypart(mon, EYE))); break;
				case 368: pline("You hideously mangle %s's %s and bite off the %s.", mon_nam(mon), mbodypart(mon, HAND), makeplural(mbodypart(mon, FINGER))); break;
				case 369: pline("%s experiences a burst of inconceivable agony as its %s is torn apart by your attack.", Monnam(mon), mbodypart(mon, LEG)); break;
				case 370: pline("You viciously bite %s's %s, shredding it to pieces. There is nothing left but shreds of skin.", mon_nam(mon), mbodypart(mon, ARM)); break;
				case 371: pline("You tear out %s's throat with your claws while also breaking its shoulder and collarbone.", mon_nam(mon)); break;
				case 372: pline("You tear %s's %s apart and its %s are ripped out like a piece of fruit.", mon_nam(mon), mbodypart(mon, FACE), makeplural(mbodypart(mon, EYE))); break;
				case 373: pline("%s's gut burns and twists as you crush its internal organs.", Monnam(mon)); break;
				case 374: pline("You maul %s's back, rupturing the %s in the process.", mon_nam(mon), mbodypart(mon, SPINE)); break;
				case 375: pline("With your claws, you nick %s's %s and open up the carotid artery. %s spectacularly fountains everywhere.", mon_nam(mon), mbodypart(mon, NECK), mbodypart(mon, BLOOD)); break;
				case 376: pline("There is a crack as %s's broken ribs puncture its %s.", mon_nam(mon), mbodypart(mon, LUNG)); break;
				case 377: pline("You bite %s in the %s, and as you pull back, a red curtain bursts forth as %s collapses on the ground with its femoral artery shredded.", mon_nam(mon), mbodypart(mon, LEG), mon_nam(mon)); break;
				case 378: pline("%s's %s is removed from a tear that reaches from %s to abdomen. %s's %s falls down to the side and lands in its guts as it staggers and falls down dead.", Monnam(mon), mbodypart(mon, ARM), mbodypart(mon, NECK), Monnam(mon), mbodypart(mon, HEAD)); break;
				case 379: pline("Your crushing bite cracks %s's skull.", mon_nam(mon)); break;
				case 380: pline("%s suddenly finds it hard to breathe with its throat lying on the ground. It collapses next to it, dead.", Monnam(mon)); break;
				case 381: pline("You hold %s's still beating heart in your feral claw... wait, never mind, it's stopped now.", mon_nam(mon)); break;
				case 382: {
					strcpy(buf, mon_nam(mon));

					pline("%s's %s snaps as you snatch it and shake it back and forth. Then you drop %s on the ground and begin to feast.", Monnam(mon), mbodypart(mon, NECK), buf); break;
				}
				case 383: pline("You manage to bite both of %s's %s apart. Then your teeth reach its %s cracking it, allowing you to eat %s whole.", mon_nam(mon), makeplural(mbodypart(mon, LEG)), mbodypart(mon, SPINE), mon_nam(mon)); break;
				case 384: pline("You crush %s's %s including every bone from the calf to the ribs. Bone shards play billiards with its organs and %s vessels and it is dead before it slushes to the ground.", mon_nam(mon), mbodypart(mon, LEG), mbodypart(mon, BLOOD)); break;
				case 385: pline("There's a release of tension in %s's midsection, who is unable to catch its guts as they spill out onto the ground.", mon_nam(mon)); break;
				case 386: pline("You bite off %s's %s, who falls over cracking its skull open like an egg on the nearby surface and dies instantly.", mon_nam(mon), mbodypart(mon, LEG)); break;
				case 387: pline("%s gets a real good look at your tongue and teeth before you bite off its %s.", Monnam(mon), mbodypart(mon, HEAD)); break;
				case 388: pline("%s is hit and lands 20 feet from its %s.", Monnam(mon), makeplural(mbodypart(mon, LEG))); break;
				case 389: pline("Suddenly, chunks of %s's destroyed body fly everywhere.", mon_nam(mon)); break;
				case 390: pline("%s's body is engulfed in a maelstrom of cold.", Monnam(mon)); break;
				case 391: pline("You shoot a breath of frozen air, clutching %s's throat.", mon_nam(mon)); break;
				case 392: pline("You catch %s in a devastating blast of cold, forming ice around it, and it slips and falls.", mon_nam(mon)); break;
				case 393: pline("Large swaths of exposed skin crack and turn bright red as %s is severely frost burnt by you!", mon_nam(mon)); break;
				case 394: pline("The abyssal cold freezes %s's exposed skin, which cracks and blackens.", mon_nam(mon)); break;
				case 395: pline("%s is battered about by freezing cold, hypothermia setting in. It loses its mind as its body begins to shut down.", Monnam(mon)); break;
				case 396: pline("You drain %s's %s of vital energy and reduce them to blackened frostburn stumps.", mon_nam(mon), makeplural(mbodypart(mon, ARM))); break;
				case 397: pline("Swirling cold engulfs %s's %s, freezing its lower body.", mon_nam(mon), makeplural(mbodypart(mon, LEG))); break;
				case 398: pline("You catch %s's %s in the freezing cold, who is unable to shut its %s in time, and they freeze solid.", mon_nam(mon), mbodypart(mon, HEAD), makeplural(mbodypart(mon, EYE))); break;
				case 399: pline("%s is caught in the center of the cold, all heat and energy draining from its body and suffering serious burns on all exposed skin.", Monnam(mon)); break;
				case 400: pline("You encase %s's %s in ice, whose %s turn into ice sculptures and shatter.", mon_nam(mon), mbodypart(mon, HAND), makeplural(mbodypart(mon, FINGER))); break;
				case 401: pline("Your blast freezes %s seriously, damaging all exposed skin and causing third and fourth degree burns.", mon_nam(mon)); break;
				case 402: pline("Your blast sears %s's flesh, causing third and fourth degree burns over exposed and unexposed skin.", mon_nam(mon)); break;
				case 403: pline("Your icy torrent blackens and freezes %s's flesh, nerve and muscle.", mon_nam(mon)); break;
				case 404: pline("You sap the very vital energy of %s's spirit, whose life begins to slip away.", mon_nam(mon)); break;
				case 405: pline("You engulf %s in intense cold causing its heart to stop, leaving a very well preserved corpse.", mon_nam(mon)); break;
				case 406: pline("Your breath literally freezes in %s's throat as its %s is frozen. %s drops to the knees and the shock shatters its frozen %s, causing it to die of suffocation shortly later.", mon_nam(mon), mbodypart(mon, NECK), Monnam(mon), mbodypart(mon, NECK)); break;
				case 407: pline("Frost climbs up %s's %s moments before it freezes solid and shatters. %s falls impaling its %s on its own weapon.", mon_nam(mon), mbodypart(mon, LEG), mon_nam(mon), mbodypart(mon, FACE)); break;
				case 408: pline("%s thrashes around in horrible pain before dropping down dead.", Monnam(mon)); break;
				case 409: pline("The front of %s's %s becomes frozen. It stumbles around in pain and strikes a wall, breaking the front half of its skull apart, showing frozen brain to all bystanders.", mon_nam(mon), mbodypart(mon, FACE)); break;
				case 410: pline("%s's waist becomes stiff, and a moment later it falls over backwards as its torso separates from the lower body.", Monnam(mon)); break;
				case 411: pline("You froze the %s in %s's veins, who clutches its chest as it slowly dies from suffocation.", mbodypart(mon, BLOOD), mon_nam(mon)); break;
				case 412: pline("You encase %s's %s in ice, who is unable to free it before it suffocates.", mon_nam(mon), mbodypart(mon, HEAD)); break;
				case 413: pline("You coat %s's lower body and the ground in ice, causing it to slip and shatter its frozen shins. Additionally you thrust a frozen icicle through %s's %s that kills it instantly.", mon_nam(mon), mon_nam(mon), mbodypart(mon, NECK)); break;
				case 414: pline("As %s tries to dodge your icy blast, the living tissue separates from the frozen tissue. Its living half of the body is in exquisite pain in the moments before it dies.", mon_nam(mon)); break;
				case 415: pline("You completely cover %s's body in ice, who freezes to death long before it can be freed.", mon_nam(mon)); break;
				case 416: pline("You freeze %s solid as a statue, dead and perfectly preserved.", mon_nam(mon)); break;
				case 417: pline("You freeze %s solid, who falls over shattering into several large pieces.", mon_nam(mon)); break;
				case 418: pline("You freeze %s's body completely solid straight through to the core, who then shatters into a million pieces.", mon_nam(mon)); break;
				case 419: pline("You introduce %s to the chamber of anathema, which exposes it to a torment that exceeds any possible imagination.", mon_nam(mon)); break;
				case 420: pline("You summon a huge monster that eats %s whole.", mon_nam(mon)); break;
				case 421: pline("You pierce %s's ribs with your spear!", mon_nam(mon)); break;
				case 422: pline("Your cannon is loaded, and you fire a hundred gobs of pus at %s, who ends up drowning in the icky liquid.", mon_nam(mon)); break;
				case 423: pline("You realize that the cowardly cunt of %s tried to attack you from behind. Furious, you grab its %s and firmly smash it into a set of iron bars.", mon_nam(mon), mbodypart(mon, HEAD)); break;
				case 424: pline("You just know that %s is a sexual predator who has groped a woman, and now finally your lawyer has arrived and takes %s away to a court trial.", mon_nam(mon), mon_nam(mon)); break;
				case 425: pline("Suddenly, the police appears and shouts 'Put your %s in the air!' At first, you think they're coming for you, but then your surprised %s watch %s get put in %scuffs and abducted.", makeplural(mbodypart(mon, HAND)), makeplural(body_part(EYE)), mon_nam(mon), mbodypart(mon, HAND)); break;
				case 426: pline("You somehow manage to melt %s's body completely by peeing on it. Apparently your pee is really acidic or something.", mon_nam(mon)); break;
				case 427: pline("You throw %s into a heap of cow dung, and for some reason that causes %s's body to dissolve completely.", mon_nam(mon), mon_nam(mon)); break;
				case 428: pline("You steal %s's briar heart, causing it to die instantly!", mon_nam(mon)); break;
				case 429: pline("Your minddrill tears apart %s's brain, who screams in agony and dies.", mon_nam(mon)); break;
				case 430: pline("With your sexy butt, you blow some weaponized farting gas into %s's %s, causing its body to swell up and explode into chunks.", mon_nam(mon), mbodypart(mon, FACE)); break;
				case 431: pline("%s dies and unlocks the 'Killed by %s' achievement in the process!", Monnam(mon), playeraliasname); break;
				case 432: pline("%s has been killed, but may respawn later!", Monnam(mon)); break;
				case 433: pline("%s has lost already again! How many times did you kill it already in this game?", Monnam(mon)); break;
				case 434: pline("%s is pulled into the abyss and has to pay 2000 zorkmids to you!", Monnam(mon)); break;
				case 435: pline("You sabotage %s's attempts to win the property challenge!", mon_nam(mon)); break;
				case 436: pline("You make true on your announcement that you'd kill %s.", mon_nam(mon)); break;
				case 437: pline("You use your telephoning ball to lure %s out of the dungeon!", mon_nam(mon)); break;
				case 438: pline("%s fires a function. Now %s has one less function remaining.", Monnam(mon), mhe(mon)); break;
				case 439: pline("%s accidentally surrenders.", Monnam(mon)); break;
				case 440: pline("%s's internet connection drops due to a netsplit.", Monnam(mon)); break;
				case 441: pline("The admin realized that %s is a bot, and bots aren't allowed on this server. So %s is banned from the game now and can't come back.", mon_nam(mon), mon_nam(mon)); break;
				case 442: pline("You somehow manage to break %s's %s so badly with your soft yellow suede sneakers that it loses several gallons of %s and dies.", mon_nam(mon), mbodypart(mon, LEG), mbodypart(mon, BLOOD)); break;
				case 443: pline("You stop playing like an idiot and finally kill the annoying %s.", l_monnam(mon)); break;
				case 444: pline("You swat the pesky %s gnat with your fly swatter.", l_monnam(mon)); break;
				case 445: pline("%s decides to be nice today, and teleports itself away to spare you the hassle of having to fight it.", Monnam(mon)); break;
				case 446: pline("%s says 'All right, %s, I know you'd stand no chance against me in a serious fight but I'm feeling generous, so I'm letting you win this time.'", Monnam(mon), playeraliasname); break;
				case 447: pline("%s was suddenly summoned by another SLEX player on this server and therefore has to leave your game to join the other one instead!", Monnam(mon)); break;
				case 448: pline("%s hissed off - one less.", Monnam(mon)); break;
				case 449: pline("%s is hit by the timeout bug and quits the game.", Monnam(mon)); break;
				case 450: pline("%s tries to laugh fiendishly in an attempt to mock you, but you quickly throw a knife and hit %s right in that ugly %s of %s.", Monnam(mon), mhim(mon), mbodypart(mon, FACE), mhis(mon)); break;
				case 451: pline("You murderer, you killed the beautiful %s!", l_monnam(mon)); break;
				case 452: pline("You decide that all beauty must fade, and therefore ruin the wonderful %s. You sick bastard.", l_monnam(mon)); break;
				case 453: pline("You asshole! %s just wanted to live a peaceful life and now you killed it in cold blood! Hopefully you'll die of a heart attack!", Monnam(mon)); break;
				case 454: pline("You vow to genocide the entire %s race as you kill this particular specimen.", l_monnam(mon)); break;
				case 455: pline("%s has an appointment with Doctor Ben Dovah!", Monnam(mon)); break;
				case 456: pline("You manage to win the 'Beat up %s' game! But in order to win the SLEX game, you need to defeat many other opponents as well!", mon_nam(mon)); break;
				case 457: pline("%s is defeated now... but don't get too cocky, for the next enemy may be a %s.", Monnam(mon), rndmonnam()); break;
				case 458: pline("You manage to stab %s, but with its dying breath it mutters: 'I spawned a %s one level down from here.'", mon_nam(mon), rndmonnam()); break;
				case 459: pline("You transform %s into a %s corpse.", mon_nam(mon), rndcolor()); break;
				case 460: pline("%s loses its mind and commits suicide.", Monnam(mon)); break;
				case 461: pline("Your shot hits %s and completely disables it.", mon_nam(mon)); break;
				case 462: pline("You burn %s to ashes.", mon_nam(mon)); break;
				case 463: pline("You electrocute %s to death.", mon_nam(mon)); break;
				case 464: pline("%s resonates and break up.", Monnam(mon)); break; /* sic --Amy */
				case 465: pline("%s dies from loss of %s! But because you didn't score the kill yourself, you don't get AP!", Monnam(mon), mbodypart(mon, BLOOD)); break;
				case 466: pline("%s is poisoned to death and utters a final 'fuck you' because it knows that you got robbed of the AP for this kill.", Monnam(mon)); break;
				case 467: pline("Since %s doesn't want you to gain experience points, it suicides to prevent you from getting credit for the kill!", mon_nam(mon)); break;
				case 468: pline("You stopped biding your time, and unleash energy to instakill %s in one overwhelming shot.", mon_nam(mon)); break;
				case 469: pline("You quickly enter the maximum damage cheat just before your blade is about to strike %s, and as a result the enemy dies even though you'd normally just do scratch damage.", mon_nam(mon)); break;
				case 470: pline("You use your super ape to annihilate %s.", mon_nam(mon)); break;
				case 471: pline("You ram your stick very far up %s's ass!", mon_nam(mon)); break;
				case 472: pline("%s's %s breaks from your torture.", Monnam(mon), mbodypart(mon, NECK)); break;
				case 473: pline("You completely vaporized %s.", mon_nam(mon)); break;
				case 474: pline("You smash the %s of the damn smoker bitch called %s, who collapses and drowns in %s.", mbodypart(mon, HEAD), l_monnam(mon), mbodypart(mon, BLOOD)); break;
				case 475: pline("You knock out %s and dump %s fucking cigarettes in a very deep hole.", mon_nam(mon), mhis(mon)); break;
				case 476: pline("Another opponent has died to you!"); break;
				case 477: pline("You fired %s in the ass hole!", mon_nam(mon)); break;
				case 478: pline("You exploit %s's weak point!", mon_nam(mon)); break;
				case 479: pline("You empty several gallons of acid over %s, who melts down completely.", mon_nam(mon)); break;
				case 480: pline("You have to attack with destruct wave. %s shatters to atoms.", Monnam(mon)); break;
				case 481: pline("You use your menstruation as a weapon and completely drown %s in your %s.", mon_nam(mon), body_part(BLOOD)); break;
				case 482: pline("Congratulations, you fully opened %s's zipper and thereby defeated it!", mon_nam(mon)); break;
				case 483: pline("Wow, you managed to defeat %s's dangerous buckles! Well done!", mon_nam(mon)); break;
				case 484: pline("Now that %s's lacings are untied, it has to admit defeat.", mon_nam(mon)); break;
				case 485: pline("With its velcro lashes open, %s can no longer fight you and the jury therefore declares it defeated.", mon_nam(mon)); break;
				case 486: pline("Your %s bash %s's butt cheeks very %sily.", makeplural(body_part(HAND)), mon_nam(mon), mbodypart(mon, BLOOD)); break;
				case 487: pline("You ram a syringe containing a narcotic that could daze an elephant into %s's %s, who immediately enters a very deep sleep.", mon_nam(mon), mbodypart(mon, ARM)); break;
				case 488: pline("You infect %s with malaria and because it doesn't have sickle cell anemia, it dies.", mon_nam(mon)); break;
				case 489: pline("You perform open heart surgery on %s, who ends up bleeding to death.", mon_nam(mon)); break;
				case 490: pline("You manage to strangle %s to death!", mon_nam(mon)); break;
				case 491: pline("You run over %s with your car.", mon_nam(mon)); break;
				case 492: pline("You completely mangle %s's %s.", mon_nam(mon), mbodypart(mon, FACE)); break;
				case 493: pline("%s dies to a major fracture!", Monnam(mon)); break;
				case 494: pline("%s is crushed underneath a speeding train!", Monnam(mon)); break;
				case 495: pline("You perform a full axe kick on %s's genitals and break its pubic bone.", mon_nam(mon)); break;
				case 496: pline("Your dump truck hits %s with a full-speed head-on collision and squashes it into a thin mass of dead meat.", mon_nam(mon)); break;
				case 497: pline("At first %s tries to laugh off your direct hit, but then realizes in shock that you cut off the vein that pumps %s to its %s, and enters death spasms.", mon_nam(mon), mbodypart(mon, BLOOD), mbodypart(mon, HEAD)); break;
				case 498: pline("You decide to help %s overcome its death throes by killing it for good.", mon_nam(mon)); break;
				case 499: pline("You take revenge by punishing %s for its evil deeds!", mon_nam(mon)); break;
				case 500: pline("%s has been violated by you!", Monnam(mon)); break;
				case 501: pline("You pay back %s for all the grief %s's given you.", mon_nam(mon), mhe(mon)); break;
				case 502: pline("%s is sucked into a space-time vortex.", Monnam(mon)); break;
				case 503: pline("%s has caused an access violation and no longer participates in the game.", Monnam(mon)); break;
				case 504: pline("%s's computer encountered a blue screen of death!", Monnam(mon)); break;
				case 505: pline("%s gets banished to the void!", Monnam(mon)); break;
				case 506: pline("%s goes to hell.", Monnam(mon)); break;
				case 507: pline("You destroy %s's nerves.", mon_nam(mon)); break;
				case 508: pline("You entrap %s in the inferno.", mon_nam(mon)); break;
				case 509: pline("You smash a wine glass into %s's %s, which gets completely cut up by the shards!", mon_nam(mon), mbodypart(mon, FACE)); break;
				case 510: pline("You put %s into a saw mill, who ends up getting cut into thousands of thin shreds.", mon_nam(mon)); break;
				case 511: pline("%s explode with 0.", Monnam(mon)); break;
				case 512: pline("You let the depths swallow %s.", mon_nam(mon)); break;
				case 513: pline("Since your gauge is full, you unleash a six-fold converge breath at %s that results in a major overkill.", mon_nam(mon)); break;
				case 514: pline("M-M-M-M-Monster kill!!!"); break;
				case 515: pline("You squash %s's %s under a very large box.", mon_nam(mon), makeplural(mbodypart(mon, FOOT))); break;
				case 516: pline("As %s steps underneath, you operate the guillotine and cut off its %s.", mon_nam(mon), mbodypart(mon, HEAD)); break;
				case 517: pline("You put a tightrope around %s's %s and pull it firmly.", mon_nam(mon), mbodypart(mon, NECK)); break;
				case 518: pline("Your sexy leather pumps scratched %s so badly that it dies of severe bleedout.", mon_nam(mon)); break;
				case 519: pline("Your kick gave %s a fatal distortion fracture (don't google what that is, you don't want to know).", mon_nam(mon)); break;
				case 520: pline("Now you've however given an abrasion to %s.", mon_nam(mon)); break;
				case 521: pline("You crush %s's %s underneath your boots, whose %s turns blue as it suffocates.", mon_nam(mon), mbodypart(mon, STOMACH), mbodypart(mon, FACE)); break;
				case 522: pline("You hack up %s's body with a knife, neatly transforming it into mincemeat.", mon_nam(mon)); break;
				case 523: pline("You ensured that you don't have to clean the %s shoes!", l_monnam(mon)); break;
				case 524: {
					strcpy(buf, l_monnam(mon));

					pline("You lock %s into a room with a wild vulture and watch joyfully as the poor %s sap is hacked to death by the very sharp claws.", mon_nam(mon), buf); break;
				}
				case 525: pline("%s has prayed too often, causing the gods to become really angry and hit it with a wide-angle disintegration beam.", Monnam(mon)); break;
				case 526: pline("%s tries to pray to its god to be protected from the attack, but forgot that praying in Gehennom doesn't work. As a result, it is killed by the wrath of the angry deity.", Monnam(mon)); break;
				case 527: pline("%s guides your strike and allows you to defeat %s!", u_gname(), mon_nam(mon)); break;
				case 528: pline("%s happens to have some personal beef with %s and therefore decides to remove it from this dungeon!", (!rn2(3) ? align_gname(A_LAWFUL) : rn2(2) ? align_gname(A_NEUTRAL) : align_gname(A_CHAOTIC)), mon_nam(mon)); break;
				case 529: pline("You tell %s to stick its gender-neutral pronouns up its ass and then knock it out with a firm punch of your right %s.", mon_nam(mon), body_part(HAND)); break;
				case 530: pline("You deal %s a final scratch with your very sexy leather pumps, then watch as it slowly bleeds to death.", mon_nam(mon)); break;
				case 531: pline("%s has died to the %s scratches you caused. But now your beautiful lady pumps have completely %s-smeared heels...", mon_nam(mon), mbodypart(mon, LEG), mbodypart(mon, BLOOD)); break;
				case 532: pline("Your tender combat boot heels are very sad, because they just killed %s and there's %s everywhere...", mon_nam(mon), mbodypart(mon, BLOOD)); break;
				case 533: pline("Your cute velcro shoes are done playing with %s! Who cares that %s actually died from this little children's game? As long as you're having fun, everything's alright!", mon_nam(mon), mon_nam(mon)); break;
				case 534: pline("You shoot the critical particle cannon, thereby instakilling %s and three other monsters.", mon_nam(mon)); break;
				case 535: pline("Thankfully, you are wearing your Depends(TM) today, and therefore sum up the courage to kill %s.", mon_nam(mon)); break;
				case 536: pline("You rip off %s's nuts and feed them to your dog.", mon_nam(mon)); break;
				case 537: pline("You introduce %s to the meat shredder.", mon_nam(mon)); break;
				case 538: pline("Your mouth has finally finished nibbling %s to death.", mon_nam(mon)); break;
				case 539: pline("%s encountered a panic, causing the dungeon to collapse on it!", Monnam(mon)); break;
				case 540: pline("The game has detected that %s used cheats, and therefore declares a trickery.", mon_nam(mon)); break;
				case 541: pline("%s entered the #quit extended command and stops existing!", Monnam(mon)); break;
				case 542: pline("You manage to cut %s's %s right off!", mon_nam(mon), mbodypart(mon, ARM)); break;
				case 543: pline("You sever %s's hamstring with your sword.", mon_nam(mon)); break;
				case 544: pline("Your sword rips %s's fore%s wide open!", mon_nam(mon), mbodypart(mon, HEAD)); break;
				case 545: pline("You break %s's jawbone and knock out several teeth.", mon_nam(mon)); break;
				case 546: pline("With a wide swing, you cut off several of %s's %s.", mon_nam(mon), makeplural(mbodypart(mon, FINGER))); break;
				case 547: pline("%s is caught completely off guard by your overhead swipe and loses its %s.", mon_nam(mon), mbodypart(mon, HEAD)); break;
				case 548: pline("You perform a downward thrust and break %s's collarbone.", mon_nam(mon)); break;
				case 549: pline("You sink your sword deep into %s's chest and break several ribs.", mon_nam(mon)); break;
				case 550: pline("With a well-placed chop, you puncture %s's %s, who falls over unconscious and then dies.", mon_nam(mon), mbodypart(mon, LUNG)); break;
				case 551: pline("You manage to cut open %s's abdomen, and know that it will slowly die.", mon_nam(mon)); break;
				case 552: {

					strcpy(buf, Monnam(mon));

					pline("You severed %s's carotid artery and a fountain of %s drenches the room. %s bleeds spectacularly to death.", mon_nam(mon), mbodypart(mon, BLOOD), buf); break;
				}
				case 553: pline("Your mighty swing severs %s's %s across the breastbone, who stumbles backwards and dies.", mon_nam(mon), mbodypart(mon, ARM)); break;
				case 554: pline("You cut open %s's skull and cleave its brain.", mon_nam(mon)); break;
				case 555: pline("%s stops your blade with its torso, and has its ribs pierced and heart punctured, dying instantly.", Monnam(mon)); break;
				case 556: pline("You cut %s's femoral artery wide open, causing instant death, but you're also showered with %s in the process.", mon_nam(mon), mbodypart(mon, BLOOD)); break;
				case 557: pline("You cut %s across the abdomen, whose guts rejoice as they flee the body.", mon_nam(mon)); break;
				case 558: pline("You cover %s's %s in a white leaden dust, and then it shatters, falling to the ground.", mon_nam(mon), mbodypart(mon, ARM)); break;
				case 559: pline("You lie on the ground and as %s approaches, you bludgeon %s with a devastating bicycle kick.", mon_nam(mon), mhim(mon)); break;
				case 560: pline("Now %s gets however skin-fully.", mon_nam(mon)); break;
				case 561: pline("%s fallow together.", Monnam(mon)); break;
				case 562: pline("You firstoh %s with your knife.", mon_nam(mon)); break;
				case 563: pline("You stab %s in the crotch, severing a major artery! Blood spurts all over!", mon_nam(mon)); break;
				case 564: pline("You kick %s, crunching his left testicle!", mon_nam(mon)); break; /* no mhis */
				case 565: pline("You slash %s's sword %s! Three %s fall to the floor!", mon_nam(mon), mbodypart(mon, HAND), makeplural(mbodypart(mon, FINGER))); break;
				case 566: pline("You made sure that %s can never take over %s!", mon_nam(mon), urole.homebase); break;
				case 567: pline("Even your worst enemy at %s would be trembling in fear if he could see how brutally you just killed %s.", urole.intermed, mon_nam(mon)); break;
				case 568: pline("%s makes the error to pull down your airtight pants, and chokes to death in the resulting cloud of farting gas!", Monnam(mon)); break;
				case 569: pline("%s crashes into the accomodating traffic!", Monnam(mon)); break;
				case 570: pline("%s has a film rupture with sharp ss!", Monnam(mon)); break;
				case 571: pline("%s is so stupid and gives Colonel B. Astard the correct code, and therefore ends up being shot.", Monnam(mon)); break;
				case 572: pline("%s gets a hell travel command!", Monnam(mon)); break;
				case 573: pline("Client %s sent a bogus command packet and is removed from the game.", m_monnam(mon)); break;
				case 574: pline("Client %s sent an unreadable command packet and can no longer participate in this game.", m_monnam(mon)); break;
				case 575: pline("You've learned how to use conqueror's haki, even though one has to be born with that, and make %s fall down unconscious.", mon_nam(mon)); break;
				case 576: {
					strcpy(buf, mon_nam(mon));

					pline("%s is someone from the revolutinos army, who don't like marine guys like you at all, but the reverse is also true and therefore you shoot down %s now.", Monnam(mon), buf); break;
				}
				case 577: pline("Your cannonball hits %s fully in the %s.", mon_nam(mon), mbodypart(mon, STOMACH)); break;
				case 578: pline("Since you know that a guillotine trap is nearby, you lure %s to it, who is so stupid and actually triggers the trap, losing %s %s.", mon_nam(mon), mhis(mon), mbodypart(mon, HEAD)); break;
				case 579: pline("As %s tries to push you into a bottomless pit, you dodge and thereby cause %s to fall in %sself.", mon_nam(mon), mhim(mon), mhim(mon)); break;
				case 580: pline("%s clicked on a cursed called and sees a game over screen!", Monnam(mon)); break;
				case 581: pline("%s has a level bug.", Monnam(mon)); break;
				case 582: pline("%s isn't resistant to your elemental breath and therefore succumbs to your continued breathing!", Monnam(mon)); break;
				case 583: pline("You tie %s to the railroad tracks and wait until the train comes.", mon_nam(mon)); break;
				case 584: pline("You've constructed an electric trap with a lot of money as a bait, and %s is so greedy and actually touches the metal bars you set up. %s receives an electric shock and dies instantly.", mon_nam(mon), mhe(mon)); break;
				case 585: pline("Suddenly %s is crushed by the collapsing ceiling.", mon_nam(mon)); break;
				case 586: pline("You catch %s in a revolving spiked door.", mon_nam(mon)); break;
				case 587: pline("You managed to throw %s out of the ring, thereby defeating %s!", mon_nam(mon), mhim(mon)); break;
				case 588: pline("Bang, your gunboat sinks %s's ship.", mon_nam(mon)); break;
				case 589: pline("You pull %s underwater and wait for %s to drown.", mon_nam(mon), mhim(mon)); break;
				case 590: pline("%s has overdosed on drugs, and suddenly falls over dead.", Monnam(mon)); break;
				case 591: pline("You tell %s to 'kys yourself', and %s is actually stupid enough to comply.", mon_nam(mon), mhe(mon)); break;
				case 592: pline("You smash %s's %s with your wedding ring because you're such a cowardly cunt.", mon_nam(mon), mbodypart(mon, FACE)); break;
				case 593: pline("With your block heels, you fully finish off %s in melee combat.", mon_nam(mon)); break;
				case 594: pline("It turns out that %s was infected with covid-19 all the time, and has just passed away due to the disease.", mon_nam(mon)); break;
				case 595: pline("%s tries to push you into a wall of fire, but you dodge and thereby cause %s to fall in %sself.", Monnam(mon), mhim(mon), mhim(mon)); break;
				case 596: pline("Suddenly the police comes and arrests %s for not adhering to the social distancing protocols.", mon_nam(mon)); break;
				case 597: pline("%s accidentally falls into a pool of water, only to discover that the water is really acid, and ends up dissolving completely.", Monnam(mon)); break;
				case 598: pline("You ruined %s's day.", mon_nam(mon)); break;
				case 599: pline("You crush %s's hopes.", mon_nam(mon)); break;
				case 600: pline("%s has triggered too many nasty traps and therefore decides to ragequit the game.", Monnam(mon)); break;
				case 601: pline("You throw a totally homosexual spin kick, and this time %s actually gets knocked out by it.", mon_nam(mon)); break;
				case 602: pline("With your jump kick, you bludgeon the dirty ladder camper bastard of %s.", mon_nam(mon)); break;
				case 603: pline("You rapidly batter %s with punches, beating the crap out of %s.", mon_nam(mon), mhim(mon)); break;
				case 604: pline("%s loses an %s as a result of your thrown knife!", Monnam(mon), mbodypart(mon, ARM)); break;
				case 605: pline("You start kicking %s in the nuts repeatedly, and because the noob is wearing pants with holes, you always aim for the holes and thereby manage to bludgeon him.", mon_nam(mon)); break;
				case 606: pline("%s life flashes before %s %s as your sword connects with %s %s.", s_suffix(Monnam(mon)), mhis(mon), makeplural(mbodypart(mon, EYE)), mhis(mon), mbodypart(mon, rn2(2) ? FACE : NECK)); break;
				case 607: pline("First you fire at %s, then you fire again, then you smash %s with your hammer and finally take a look at the identity card to figure out that it was number %d.", mon_nam(mon), mhim(mon), mon->m_id); break;
				case 608: pline("You mow down %s because %s is only level %d!", mon_nam(mon), mhe(mon), mon->m_lev); break;
				case 609: pline("You call in an airstrike on the coordinates %d,%d.", mon->mx, mon->my); break;
				case 610: pline("Finally, you managed to remove %s's remaining %d hit points.", mon_nam(mon), mon->mhp); break;
				case 611: pline("Unbelievable! You somehow managed to reduce %s's maximum %d hit points to zero!", mon_nam(mon), mon->mhpmax); break;
				case 612: pline("Despite having a maximum mana of %d Pw, %s didn't stand no chance!", mon->m_enmax, mon_nam(mon)); break;
				case 613: pline("%s doesn't want to put in the effort of fighting you, and is cut down by your axe.", Monnam(mon)); break;
				case 614: pline("Your scythe decapitates %s, who somehow just stood there instead of trying to dodge.", mon_nam(mon)); break;
				case 615: pline("%s is on a heal block for %d turns, and unlike Pokemon Soul Silver, it also prevents the use of healing potions in this game so you manage to remove its last hit point!", Monnam(mon), mon->healblock); break;
				case 616: pline("%s has been slowed by inertia for the next %d turns, and you're so cruel to abuse that momentary weakness.", Monnam(mon), mon->inertia); break;
				case 617: pline("You've prayed %ld times already, and your last prayer included beseeching the gods to let you defeat %s. Apparently, they complied.", u.uconduct.praydone, mon_nam(mon)); break;
				case 618: pline("You're really a serial killer! %ld monsters have been killed by you already, and you just killed another!", u.uconduct.killer); break;
				case 619: pline("You've wished for %ld objects by now, and for good purpose, because they allowed you to win the fight with %s.", u.uconduct.wishes, mon_nam(mon)); break;
				case 620: pline("You tell %s to fuck %sself because it certainly wasn't your fault that your savegame file got erased. If you had been able to make the decision, the game would still be there.", mon_nam(mon), mhim(mon)); break;
				case 621: pline("You use the FATAL attack sheet for your melee attacks, and just rolled the rape attack, which has a devastating effect on %s.", mon_nam(mon)); break;
				case 622: pline("%s is so weird and requests the game master to let %s use the FATAL spellcasting table even though this game is actually SLEX. The game master complies, and the attempted spell results in a catastrophic failure which then rolls the 1 in 2000 chance to cast Fatal.", Monnam(mon), mhim(mon)); break;
				case 623: pline("Your char is capable of remembering a whopping %d spells, so you cast one of them to kill %s.", urole.spelrete, mon_nam(mon)); break;
				case 624: pline("You firmly stand your ground at %d,%d, and don't allow %s to pass.", u.ux, u.uy, mon_nam(mon)); break;
				case 625: pline("Since you've recently leveled up to level %d, you're now capable of taking down %s!", u.ulevel, mon_nam(mon)); break;
				case 626: pline("Your plan to defend your %d remaining HP was successful!", u.uhp); break;
				case 627: pline("You use all the %d gifts you've received from the gods, leaving the poor %s with no chance.", u.ugifts, l_monnam(mon)); break;
				case 628: pline("%s looks like a statue of monster number %d - oh wait, now it became one. Apparently your Petrificus Totalus curse worked.", Monnam(mon), u.statuetrapname); break;
				case 629: pline("You may be a cheater who has used cheats %d times already, but you don't feel bad about doing it again, and therefore open the console, click on %s, and enter the kill command.", u.hangupamount, mon_nam(mon)); break;
				case 630: pline("Your cop wanted level is %d right now, but %s is one cop asshole that won't be busting you, that's for sure.", u.copwantedlevel, mon_nam(mon)); break;
				case 631: pline("Yeah, it seems that your specific attack with ID %d is super-effective against %s!", u.adrunsattack, mon_nam(mon)); break;
				case 632: pline("You're capable of telling good weapons apart from crappy ones due to your weapon enchantment knowledge of %d, and just used one of the good weapons to pound %s into submission!", u.weapchantrecskill, mon_nam(mon)); break;
				case 633: pline("With your sanity of %d, your sight of %s is perfectly clear. Every millisecond of your attack plays out before your %s, letting you see exactly how your blade cuts through flesh.", u.usanity, mon_nam(mon), makeplural(body_part(EYE)) ); break;
				case 634: pline("You've used your quest artifact for %d turns straight, and are now really proficient with it. Case in point, %s fell in a single hit.", u.artifinitythreshold, mon_nam(mon)); break;
				case 635: pline("There's the danger of you meeting Mr. Conclusio, in fact the counter has already increased to %d. So you decide to not waste any more time, and cut down the hapless %s.", u.conclusiocount, l_monnam(mon)); break;
				case 636: pline("You use the hidden power number %d to get rid of %s!", u.hiddenpowerskill, mon_nam(mon)); break;
				case 637: pline("%s calls for a timeout, stating that %s has to go to the toilet, and you pretend to comply... only to shoot %s in the back as %s's walking away.", Monnam(mon), mhe(mon), mhim(mon), mhe(mon)); break;
				case 638: pline("You throw the bottle at %s, it explodes, and the fragments fly everywhere.", mon_nam(mon)); break;
				case 639: pline("%s is no longer capable of protecting the castle!", Monnam(mon)); break;
				case 640: pline("You hope that the wilderness will devour %s's corpse.", mon_nam(mon)); break;
				case 641: pline("You drown %s in the underground river.", mon_nam(mon)); break;
				case 642: pline("After thrusting %s into the nearby pond, you realize that %s cannot swim, decide that %s was an asshole anyway, and watch %s slowly turn into a watery corpse.", mon_nam(mon), mhe(mon), mhe(mon), mhim(mon)); break;
				case 643: pline("%s suddenly steers %s car into a tree at full speed, and dies in the resulting collision.", Monnam(mon), mhis(mon)); break;
				case 644: pline("Since there's the stupid rule that %s is immune to your physical attacks, you push %s into a spiked pit instead, which %s turns out to not be immune to.", mon_nam(mon), mhim(mon), mhe(mon)); break;
				case 645: pline("You smash %s's %s to a pulp with your massive guitar.", mon_nam(mon), mbodypart(mon, FACE)); break;
				case 646: pline("Finally, you managed to land the killing blow against %s, after getting annoyed by the stupid 'last hitpoint is always the hardest to remove' 'feature' that caused you to miss five times in a row.", mon_nam(mon)); break;
				case 647: pline("%s was living a good life, which you just decided to terminate.", Monnam(mon)); break;
				case 648: pline("You mercilessly slaughter %s!", mon_nam(mon)); break;
				case 649: pline("Taking down %s was like shooting fish in a barrel.", mon_nam(mon)); break;
				case 650: pline("Your sword slits %s completely from top to bottom.", mon_nam(mon)); break;
				case 651: pline("You hit %s so hard that %s is no longer in one piece.", mon_nam(mon), mhe(mon)); break;
				case 652: pline("Yeah! You rolled a natural 20 and instakill %s!", mon_nam(mon)); break;
				case 653: pline("%s spontaneously breaks down!", Monnam(mon)); break;
				case 654: pline("Since %s has been awake for 25 hours straight, %s suddenly falls down unconscious from exertion.", mon_nam(mon), mhe(mon)); break;
				case 655: pline("You decide to act as if %s has stopped existing - and suddenly that thought becomes reality.", mon_nam(mon)); break;
				case 656: pline("%s's deity decides to remove %s from the dungeon.", Monnam(mon), mhim(mon)); break;
				case 657: pline("You push %s onto a railroad track just as the train approaches.", mon_nam(mon)); break;
				case 658: pline("%s falls from a high bridge.", Monnam(mon)); break;
				case 659: pline("Suddenly %s reads a cursed scroll of teleportation and accidentally levelports to level -9, and ends up falling to %s death.", mon_nam(mon), mhis(mon)); break;
				case 660: pline("%s dies due to genocidal confusion!", Monnam(mon)); break;
				case 661: pline("Suddenly %s is turned to slime by a scroll of genocide.", mon_nam(mon)); break;
				case 662: pline("You don't know why, but %s somehow managed to be petrified by a green slime.", mon_nam(mon)); break;
				case 663: pline("%s was killed by a died.", Monnam(mon)); break;
				case 664: pline("*click click click click click* %s died.", Monnam(mon)); break;
				case 665: pline("The game master decides to cheat in your favor and makes you roll a super-double-critical hit that vaporizes %s instantly.", mon_nam(mon)); break;
				case 666: pline("You smashed %s so hard, your weapon almost broke.", mon_nam(mon)); break;
				case 667: pline("Your character is way too overpowered! Look how %s stood absolutely no chance!", mon_nam(mon)); break;
				case 668: pline("Man, you should play a bit worse, and give %s a chance for once!", mon_nam(mon)); break;
				case 669: pline("There was absolutely nothing that %s could have done to stop your wrath.", mon_nam(mon)); break;
				case 670: pline("%s tries to stop your pistol bullet with %s %s.", Monnam(mon), mhis(mon), mbodypart(mon, HEAD)); break;
				case 671: pline("And again you defeated an enemy!"); break;
				case 672: pline("You're simply unstoppable!"); break;
				case 673: pline("%s ended up on the NL list, and you just executed order %d to punish %s for being not likable.", Monnam(mon), rnz(1000000), mhim(mon)); break;
				case 674: pline("Your megaton punch sends %s flying through the air.", mon_nam(mon)); break;
				case 675: pline("Ueueuep! %s falls from the picture.", Monnam(mon)); break;
				case 676: pline("Now the asshole of %s is eliminated!", mon_nam(mon)); break;
				case 677: pline("You are playing with quaractar rules, and rolled a 6 which means that %s dies instantly.", mon_nam(mon)); break;
				case 678: pline("Now %s's kinuesili station is destroyed!", mon_nam(mon)); break;
				case 679: pline("%s was already critically injured, and after this hit %s's completely out of health.", Monnam(mon), mhe(mon)); break;
				case 680: pline("%s has to enter the circles of hell now, for %s had sinned.", Monnam(mon), mhe(mon)); break;
				case 681: pline("%s lost %s last life and sees a game over screen.", Monnam(mon), mhis(mon)); break;
				case 682: pline("%s was too slow to escape the wrath of your heavy hammer.", Monnam(mon)); break;
				case 683: pline("%s suddenly remembers that %s cannot see blood, and falls down unconscious from seeing the wound you gave to %s.", Monnam(mon), mhe(mon), mhim(mon)); break;
				case 684: pline("You lamer used the screen-wide total annihilation ability, and %s simply dies. Great, maybe you should just use the autoplaythegameforyou command if you don't want to put in the effort for playing the game properly.", mon_nam(mon)); break;
				case 685: pline("%s ignored a red traffic light, and is chased off the dungeon by the police.", Monnam(mon)); break;
				case 686: {
					strcpy(buf, mon_nam(mon));

					pline("You deactivate the %s-floor collision detection, which results in %s falling through the floor to %s death.", l_monnam(mon), buf, mhis(mon)); break;
				}
				case 687: pline("The double wanker of %s can suck cocks in hell now.", mon_nam(mon)); break;
				case 688: pline("%s is crying %sy tears because you're so mean to %s.", Monnam(mon), mbodypart(mon, BLOOD), mhim(mon)); break;
				case 689: pline("Good job, now you made %s sad because you're such an evil person.", mon_nam(mon)); break;
				case 690: pline("Lol, look at the fullpost of %s, who can't even defeat the wimpy %s.", mon_nam(mon), playeraliasname); break;
				case 691: pline("Finally you don't have to see %s's hacking kisser any longer.", mon_nam(mon)); break;
				case 692: pline("%s is really the retard chief of the service!", Monnam(mon)); break;
				case 693: pline("And the story ends for %s, because %s just passed away.", mon_nam(mon), mhe(mon)); break;
				case 694: pline("You use your considerable body weight to simply crush %s.", mon_nam(mon)); break;
				case 695: pline("%s says the wrong line, namely the one that allows you to pee into %s pretty high-heeled lady boots, and you evil person actually go ahead and defile them irreversibly.", Monnam(mon), mhis(mon)); break;
				case 696: pline("You produced squeaking farting noises for a long time, and now %s's feminine sneakers are destroyed even though they were so lovely.", mon_nam(mon)); break;
				case 697: pline("You swat %s like an annoying insect.", mon_nam(mon)); break;
				case 698: pline("You shit on %s until %s is dead. Oh whoops, that was supposed to read 'shoot', not 'shit'...", mon_nam(mon), mhe(mon)); break;
				case 699: pline("%s's system hangs, and %s cannot reboot the PC in time so %s disappears from the game.", Monnam(mon), mhe(mon), mhe(mon)); break;
				case 700: pline("%s fell asleep from reading your novel, and you simply stab %s to death in %s sleep.", Monnam(mon), mhim(mon), mhis(mon)); break;
				case 701: pline("Your leather belt clamps %s's %s, who quickly develops a deep vein thrombosis and dies in a horrifying fashion.", mon_nam(mon), mbodypart(mon, LEG)); break;
				case 702: pline("Somehow, you manage to pull off a one-hit-kill against %s by hitting %s in the %s with your screwdriver.", mon_nam(mon), mhim(mon), mbodypart(mon, HEAD)); break;
				case 703: pline("You officially appoint yourself as the King of this dungeon as you finish off %s.", mon_nam(mon)); break;
				case 704: pline("Yay, %s was actually stupid enough to eat your poisonous apple and dies!", mon_nam(mon)); break;
				case 705: pline("You rubbed your weapon with deadly welkynd essence, and poison %s to death in one hit.", mon_nam(mon)); break;
				case 706: pline("Your female ski wedges rip off %s's %snails in a rather %sy way.", mon_nam(mon), mbodypart(mon, FINGER), mbodypart(mon, BLOOD)); break;
				case 707: pline("You incapacitate %s and let your girlfriend Sing take %s to her torture chamber.", mon_nam(mon), mhim(mon)); break;
				case 708: pline("You killed another monster."); break;
				case 709: pline("Enemy down."); break;
				case 710: pline("Another monster has died."); break;
				case 711: pline("You get rid of the opponent."); break;
				case 712: pline("The enemy is dead."); break;
				case 713: pline("%s is so dead!", Monnam(mon)); break;
				case 714: pline("An enemy has died!"); break;
				case 715: pline("You have an astounding strength of %d, and smash %s to a pulp.", ACURR(A_STR), mon_nam(mon)); break;
				case 716: pline("Thankfully you are very wise, with %d wisdom in fact, and make the wise decision to off %s.", ACURR(A_WIS), mon_nam(mon)); break;
				case 717: pline("Barely managed to off %s before %s could unleash the gauge attack on you!", mon_nam(mon), mhe(mon)); break;
				case 718: pline("You tell %s to jump off a bridge, and %s actually does so and plummets to %s death.", mon_nam(mon), mhe(mon), mhis(mon)); break;
				case 719: pline("%s has died, and needs to be buried.", Monnam(mon)); break;
				case 720: pline("Now you can sing the elegy, for %s has just passed away.", mon_nam(mon)); break;
				case 721: pline("%s has an E, which is shorthand for epileptic seizure!", Monnam(mon)); break;
				case 722: pline("You slam the massive translucent heel on %s's %s with overwhelming force.", mon_nam(mon), mbodypart(mon, HEAD)); break;
				case 723: pline("Despite %s's attempts to tease you with %s %s, you land the killing blow!", mon_nam(mon), mhis(mon), pantsdescription(mon)); break;
				case 724: pline("Since %s's butt cheeks are no longer protected by the %s, you end up bashing them so badly that a fountain of %s shoots out... urgh...", mon_nam(mon), pantsdescription(mon), mbodypart(mon, BLOOD)); break;
				case 725: pline("%s admits defeat after your powerful hit connects!", Monnam(mon)); break;
				case 726: pline("Boom, %s's block heel shatters into a thousand fragments. I guess %s can't fight on now.", mon_nam(mon), mhe(mon)); break;
				case 727: pline("Oh no, you ripped %s's leather apart! How could you? That's such an evil thing of you to do!", mon_nam(mon)); break;
				case 728: pline("After grabbing %s, you quickly put %s on and are therefore declared the winner of this confrontation!", mon_nam(mon), mhim(mon)); break;
				case 729: pline("%s has breathed %s last breath!", Monnam(mon), mhis(mon)); break;
				case 730: pline("You snuffed out %s's life.", mon_nam(mon)); break;
				case 731: pline("%s got rekt by you.", Monnam(mon)); break;
				case 732: pline("You take %s down a peg or two.", mon_nam(mon)); break;
				case 733: pline("%s falls over dead for no good reason.", Monnam(mon)); break;
				case 734: pline("Due to a game bug, %s simply disappears even though %s had full health left.", mon_nam(mon), mhe(mon)); break;
				case 735: pline("The time has come for %s to die.", mon_nam(mon)); break;
				case 736: pline("%s fell into the lava, and tries to pull %sself back out but you intentionally block the space where %s'd land, so %s ends up melting away.", Monnam(mon), mhim(mon), mhe(mon), mhe(mon)); break;
				case 737: pline("You fully score a goal because %s ain't got no chance.", mon_nam(mon)); break;
				case 738: pline("You evil person, you broke off %s's stiletto heels!", mon_nam(mon)); break;
				case 739: pline("Finally, %s ain't be getting on your nerves no more!", mon_nam(mon)); break;
				case 740: pline("After being shot in the %s, %s dies (expectably).", mbodypart(mon, HEAD), mon_nam(mon)); break;
				case 741: pline("%s dies from with rupture of brain.", Monnam(mon)); break; /* sic from elona */
				case 742: pline("%s decides to eat a mochi, but suddenly starts choking. Instead of helping %s, you just stand by and wait until %s runs out of air and explodes into tiny chunks.", Monnam(mon), mhim(mon), mhe(mon)); break;
				case 743: pline("%s decides to eat a mochi, but suddenly starts choking. You perform the Heimlich maneuver and bash %s up with full force; while that causes the mochi to rocket back out of %s mouth, %s also sustains a severe gastrointestinal hemorrhage and dies shortly after.", Monnam(mon), mhim(mon), mhis(mon), mhe(mon)); break;
				case 744: pline("%s fails the Turing test and is derezzed!", Monnam(mon)); break;
				case 745: pline("After your teeth bit %s for the umpteenth time, %s surrenders.", mon_nam(mon), mhe(mon)); break;
				case 746: pline("You pee at %s, who turns out to be instantly incapacitated by that type of attack.", mon_nam(mon)); break;
				case 747: pline("%s stood no chance against your whirlwind of blows!", Monnam(mon)); break;
				case 748: pline("You call for the teacher like the little rat you are, and he comes, giving %s a whopping 10 hours of detention.", mon_nam(mon)); break;
				case 749: pline("You put the little snitch of %s in a ditch.", mon_nam(mon)); break;
				case 750: pline("%s's stupid mouth has been shut forever.", Monnam(mon)); break;
				case 751: pline("You push %s off a rooftop, with the expected deadly results.", mon_nam(mon)); break;
				case 752: pline("Your Nike(TM) sneakers kick %s to death!", mon_nam(mon)); break;
				case 753: pline("%s catches your grenade, but it explodes in %s %s, which is torn straight off.", Monnam(mon), mhis(mon), mbodypart(mon, HAND)); break;
				case 754: pline("You send the double-crosser of %s to the grave.", mon_nam(mon)); break;
				case 755: pline("You foul %s, who falls to the floor... and doesn't seem able to get up again.", mon_nam(mon)); break;
				case 756: pline("Thank God it's possible to set the difficulty of this game to a very low value, which is in fact what you did, and so you slide into %s, hitting %s so badly with your %s that %s breaks down seriously wounded and isn't able to continue fighting.", mon_nam(mon), mhim(mon), makeplural(body_part(FOOT)), mhe(mon)); break;
				case 757: pline("%s trips over a stone and drops to the floor, so you quickly rush over and stomp %s repeatedly until you're sure %s doesn't get up again!", Monnam(mon), mhim(mon), mhe(mon)); break;
				case 758: pline("Suddenly %s commits hari kari or ikebana, and dies with a fountain of %s shooting out of %s %s.", mon_nam(mon), mbodypart(mon, BLOOD), mhis(mon), mbodypart(mon, STOMACH)); break;
				case 759: pline("We're not going into too much detail today, so all we say is that you defeated %s.", mon_nam(mon)); break;
				case 760: pline("Another hit! And another! Ohh, boys, %s is really losing... yet again, and - ah, %s's knocked out. %s is victorious!", mon_nam(mon), mhe(mon), playeraliasname); break;
				case 761: pline("As %s insults you again, you viciously slam your fist into %s ugly %s, shattering %s jawbone. Now %s should never be able to insult you again.", mon_nam(mon), mhis(mon), mbodypart(mon, FACE), mhis(mon), mhe(mon)); break;
				case 762: pline("A thunderous crack echoes through the dungeon as your %s connects with %s's skull.", playerweaponname(), mon_nam(mon)); break;
				case 763: pline("With a decisive strike, your %s shatters %s's skull and banishes %s from existence.", playerweaponname(), mon_nam(mon), mhim(mon)); break;
				case 764: pline("The once fearsome %s lays vanquished, defeated by your indomitable spirit.", mon_nam(mon)); break;
				case 765: pline("%s is sent to the other world.", Monnam(mon)); break;
				case 766: pline("%s is put to sleep for all eternity.", Monnam(mon)); break;
				case 767: pline("You swat %s from here to kingdom come.", mon_nam(mon)); break;
				case 768: pline("You fully thunder %s's %s into a concrete wall, causing %s stupid facial expression to become stuck for all eternity.", mon_nam(mon), mbodypart(mon, HEAD), mhis(mon)); break;
				case 769: pline("You just murdered %s, and drop %s corpse on the floor to wither away.", mon_nam(mon), mhis(mon)); break;
				case 770: pline("Congratulations, after cherry-tapping %s with your weapon 500 times in a row from your lame underwater square that you created with your OP scroll of flood, %s died. You're really a pro player who truly knows how to play the game, and you're absolutely capable of winning without abusing stupid loopholes.", mon_nam(mon), mhe(mon)); break;
				case 771: pline("Lamer, with time stop spells everyone could have defeated %s!", mon_nam(mon)); break;
				case 772: pline("You've been kiting %s for a long time, and now you landed the finishing blow without taking even a single hit yourself.", mon_nam(mon)); break;
				case 773: pline("Kaboooooom, nothing remains of %s after you fired your fatman at it.", mon_nam(mon)); break;

				default: pline("You hit %s very hard!", mon_nam(mon)); break;
	
				}
			}

		    killed(mon);	/* takes care of most messages */

		}
	} else if(u.umconf && !thrown) {
		nohandglow(mon);
		if (!mon->mconf && !resist(mon, SPBOOK_CLASS, 0, NOTELL)) {
			mon->mconf = 1;
			if (!mon->mstun && mon->mcanmove && !mon->msleeping &&
				canseemon(mon))
			    pline("%s appears confused.", Monnam(mon));
		}
	}
	if (!destroyed) {
		monster_pain(mon);
		wounds_message(mon);
	}

#ifdef SHOW_DMG
	if (!destroyed) showdmg(tmp);
#endif

	if (tech_inuse(T_PIRATE_BROTHERING) && uwep && is_lightsaber(uwep) && uwep->lamplit && uswapwep && weapon_type(uswapwep) == P_SCIMITAR && u.twoweap && obj && obj == uswapwep) {
		uwep->age += tmp;
		if (uwep->otyp == ORANGE_LIGHTSABER) uwep->age += (tmp * rnd(2));
		if (uwep->oartifact == ART_DESANN_S_WRATH) uwep->age += (tmp * rnd(2));
		pline("Your lightsaber is recharged a bit!");
	}

	return((boolean)(destroyed ? FALSE : TRUE));
}

STATIC_OVL boolean
shade_aware(obj)
struct obj *obj;
{
	if (!obj) return FALSE;
	/*
	 * The things in this list either
	 * 1) affect shades.
	 *  OR
	 * 2) are dealt with properly by other routines
	 *    when it comes to shades.
	 */
	if (obj->otyp == BOULDER || obj->otyp == HEAVY_IRON_BALL
	    || obj->otyp == IRON_CHAIN		/* dmgval handles those first three */
	    || obj->otyp == MIRROR		/* silver in the reflective surface */
	    || obj->otyp == CLOVE_OF_GARLIC	/* causes shades to flee */
	    || objects[obj->otyp].oc_material == MT_ARCANIUM
	    || objects[obj->otyp].oc_material == MT_SILVER)
		return TRUE;

	if (uwep && uwep->oartifact == ART_AP_) return TRUE; /* artifact that ignores the shade's immunity to non-silver */

	return FALSE;
}

/* check whether slippery clothing protects from hug or wrap attack */
/* [currently assumes that you are the attacker] */
STATIC_OVL boolean
m_slips_free(mdef, mattk)
struct monst *mdef;
struct attack *mattk;
{
	struct obj *obj;

	if (mattk->adtyp == AD_DRIN) {
	    /* intelligence drain attacks the head */
	    obj = which_armor(mdef, W_ARMH);
	} else {
	    /* grabbing attacks the body */
	    obj = which_armor(mdef, W_ARMC);		/* cloak */
	    if (!obj) obj = which_armor(mdef, W_ARM);	/* suit */
	    if (!obj) obj = which_armor(mdef, W_ARMU);	/* shirt */
	}

	/* if your cloak/armor is greased, monster slips off; this
	   protection might fail (33% chance) when the armor is cursed */
	if (obj && (obj->greased || obj->otyp == OILSKIN_CLOAK || itemhasappearance(obj, APP_TARPAULIN_CLOAK)) &&
		(!obj->cursed || rn2(3))) {
	    You("%s %s %s %s!",
		mattk->adtyp == AD_WRAP ?
			"slip off of" : "grab, but cannot hold onto",
		s_suffix(mon_nam(mdef)),
		obj->greased ? "greased" : "slippery",
		/* avoid "slippery slippery cloak"
		   for undiscovered oilskin cloak */
		(obj->greased || objects[obj->otyp].oc_name_known) ?
			xname(obj) : cloak_simple_name(obj));

	    if (obj->greased && (!rn2(2) || (isfriday && !rn2(2)))) {
		pline_The("grease wears off.");
		obj->greased -= 1;
	    }
	    return TRUE;
	}
	return FALSE;
}

/* used when hitting a monster with a lance while mounted */
STATIC_OVL int	/* 1: joust hit; 0: ordinary hit; -1: joust but break lance */
joust(mon, obj)
struct monst *mon;	/* target */
struct obj *obj;	/* weapon */
{
    int skill_rating, joust_dieroll;
    int bypassrating = 0;

    if (Fumbling || (Stunned && !Stun_resist) ) return 0;
    /* sanity check; lance must be wielded in order to joust */
    if (obj != uwep && (obj != uswapwep || !u.twoweap)) return 0;

    /* if using two weapons, use worse of lance and two-weapon skills */
    skill_rating = P_SKILL(weapon_type(obj));	/* lance skill */
    if (u.twoweap && P_SKILL(P_TWO_WEAPON_COMBAT) < skill_rating)
	skill_rating = P_SKILL(P_TWO_WEAPON_COMBAT);
	if (PlayerCannotUseSkills) skill_rating = P_UNSKILLED;
    if (skill_rating == P_ISRESTRICTED) skill_rating = P_UNSKILLED; /* 0=>1 */

    if (!PlayerCannotUseSkills) {

	if (skill_rating == P_MASTER) bypassrating = 1;
	if (skill_rating == P_GRAND_MASTER) bypassrating = 2;
	if (skill_rating == P_SUPREME_MASTER) bypassrating = 3;

    }

    /* odds to joust are expert:80%, skilled:60%, basic:40%, unskilled:20%
     * but Amy edit: too easy to achieve high values... after all, expert isn't the max now */
    if (((joust_dieroll = rn2(5)) < skill_rating) && (rn2(3) || (bypassrating >= rnd(4)) ) ) {
	if (joust_dieroll == 0 && rnl(50) == (50-1) &&
		!unsolid(mon->data) && !obj_resists(obj, 0, 95)) /* Amy edit: artifacts no longer immune, only highly resistant */
	    return -1;	/* hit that breaks lance */
	return 1;	/* successful joust */
    }
    return 0;	/* no joust bonus; revert to ordinary attack */
}

/*
 * Send in a demon pet for the hero.  Exercise wisdom.
 *
 * This function used to be inline to damageum(), but the Metrowerks compiler
 * (DR4 and DR4.5) screws up with an internal error 5 "Expression Too Complex."
 * Pulling it out makes it work.
 */
STATIC_OVL void
demonpet()
{
	int i;
	struct permonst *pm;
	struct monst *dtmp;
	int tries = 0;

	pline("Some hell-p has arrived!");
	i = !rn2(6) ? ndemon(u.ualign.type) : NON_PM;
	if ((Race_if(PM_GAVIL) && !Upolyd) || (Race_if(PM_PLAYER_SHEEP) && !Upolyd && u.ulevel >= 20)) {
		i = ndemon(u.ualign.type);
		while (tries++ < 50000 && i == NON_PM) i = ndemon(u.ualign.type);
	}
	pm = i != NON_PM ? &mons[i] : youmonst.data;
	if ((dtmp = makemon(pm, u.ux, u.uy, MM_NOSPECIALS)) != 0)
	    (void)tamedog(dtmp, (struct obj *)0, FALSE);
	exercise(A_WIS, TRUE);
	use_skill(P_SQUEAKING, 1);
}

/*
 * Player uses theft attack against monster.
 *
 * If the target is wearing body armor, take all of its possesions;
 * otherwise, take one object.  [Is this really the behavior we want?]
 *
 * This routine implicitly assumes that there is no way to be able to
 * resist petfication (ie, be polymorphed into a xorn or golem) at the
 * same time as being able to steal (poly'd into nymph or succubus).
 * If that ever changes, the check for touching a cockatrice corpse
 * will need to be smarter about whether to break out of the theft loop.
 */
STATIC_OVL void
steal_it(mdef, mattk)
struct monst *mdef;
struct attack *mattk;
{
	struct obj *otmp, *stealoid, **minvent_ptr;
	long unwornmask;

	/*pline("player steal attack");*/

	if (!mdef->minvent) return;		/* nothing to take */

	/* look for worn body armor */
	stealoid = (struct obj *)0;
	/* extra check for nymph race because of the could_seduce function's weirdness --Amy */
	if (could_seduce(&youmonst, mdef, mattk) /* || Race_if(PM_NYMPH)*/ ) {
	    /* find armor, and move it to end of inventory in the process */
	    minvent_ptr = &mdef->minvent;
	    while ((otmp = *minvent_ptr) != 0)
		if (otmp->owornmask & W_ARM) {
		    if (stealoid) panic("steal_it: multiple worn suits");
		    *minvent_ptr = otmp->nobj;	/* take armor out of minvent */
		    stealoid = otmp;
		    stealoid->nobj = (struct obj *)0;
		} else {
		    minvent_ptr = &otmp->nobj;
		}
	    *minvent_ptr = stealoid;	/* put armor back into minvent */
	}

	if (stealoid) {		/* we will be taking everything */
	    if (gender(mdef) == (int) u.mfemale &&
			youmonst.data->mlet == S_NYMPH)
		You("charm %s.  She gladly hands over her possessions.",
		    mon_nam(mdef));
	    else
		You("seduce %s and %s starts to take off %s clothes.",
		    mon_nam(mdef), mhe(mdef), mhis(mdef));
	}

	while ((otmp = mdef->minvent) != 0) {
	    if (!Upolyd && !Race_if(PM_NYMPH) && (!Race_if(PM_HUMAN_MONKEY) || rn2(3) ) && !Role_if(PM_BULLY) ) break;		/* no longer have ability to steal, unless nymph or bully --Amy */
	    /* take the object away from the monster */

	    if (MON_WEP(mdef) && (MON_WEP(mdef) == otmp) ) {
		mdef->misc_worn_check &= ~W_WEP;
		setmnotwielded(mdef,otmp);
		MON_NOWEP(mdef);
		otmp->owornmask = 0L;
		update_mon_intrinsics(mdef, otmp, FALSE, FALSE);

	    }

	    obj_extract_self(otmp);
	    if ((unwornmask = otmp->owornmask) != 0L) {
		mdef->misc_worn_check &= ~unwornmask;
		if (otmp->owornmask & W_WEP) {
		    setmnotwielded(mdef,otmp);
		    MON_NOWEP(mdef);
		}
		otmp->owornmask = 0L;
		update_mon_intrinsics(mdef, otmp, FALSE, FALSE);

		if (otmp == stealoid)	/* special message for final item */
		    pline("%s finishes taking off %s suit.",
			  Monnam(mdef), mhis(mdef));
	    }
	    /* give the object to the character, new function by Amy - see invent.c */

	    if (otmp && otmp->mstartinvent && !(otmp->oartifact) && !(otmp->fakeartifact && timebasedlowerchance()) && (!rn2(3) || (rn2(100) < u.musableremovechance) || (rn2(4) && (otmp->otyp == POT_BLOOD || otmp->otyp == POT_VAMPIRE_BLOOD) ) || LootcutBug || u.uprops[LOOTCUT_BUG].extrinsic || have_lootcutstone() || !timebasedlowerchance() ) ) {
			delobj(otmp);
			return;
	    }
	    if (otmp && otmp->mstartinventB && !(otmp->oartifact) && !(otmp->fakeartifact && timebasedlowerchance()) && (!rn2(4) || (rn2(100) < u.equipmentremovechance) || !timebasedlowerchance() ) ) {
			delobj(otmp);
			return;
	    }
	    if (otmp && otmp->mstartinventC && !(otmp->oartifact) && !(otmp->fakeartifact && !rn2(10)) && rn2(10) ) {
			delobj(otmp);
			return;
	    }
	    if (otmp && otmp->mstartinventE && !(otmp->oartifact) && !(otmp->fakeartifact && !rn2(20)) && rn2(20) ) {
			delobj(otmp);
			return;
	    }
	    if (otmp && otmp->mstartinventD && !(otmp->oartifact) && !(otmp->fakeartifact && !rn2(4)) && rn2(4) ) {
			delobj(otmp);
			return;
	    }
	    if (otmp && otmp->mstartinventX) {
			delobj(otmp);
			return;
	    }

	    otmp = hold_another_objectX(otmp, "You snatched but dropped %s.",
				       doname(otmp), "You steal: ");
	    if (otmp->where != OBJ_INVENT) /*continue*/return; /* otherwise you could steal everything the monster has! */
	    if (otmp->otyp == CORPSE &&
		    touch_petrifies(&mons[otmp->corpsenm]) && (!uarmg || FingerlessGloves)) {
		static char kbuf[BUFSZ];

		sprintf(kbuf, "a stolen petrifying corpse");
		instapetrify(kbuf);
		break;		/* stop the theft even if hero survives */
	    }
	    /* more take-away handling, after theft message */
	    if (unwornmask & W_WEP) {		/* stole wielded weapon */
		possibly_unwield(mdef, FALSE);
	    } else if (unwornmask & W_ARMG) {	/* stole worn gloves */
		mselftouch(mdef, (const char *)0, TRUE);
		if (mdef->mhp <= 0)	/* it's now a statue */
		    return;		/* can't continue stealing */
	    }

	    /*if (!stealoid)*/ break;	/* only taking one item */
	}
}

int
damageum(mdef, mattk)
register struct monst *mdef;
register struct attack *mattk;
{
	register struct permonst *pd = mdef->data;
	register int	tmp = d((int)mattk->damn, (int)mattk->damd);
	int armpro;
	boolean negated;
	register int    enchantlvl = 0;
	boolean noeffect = FALSE;

	int youdamagebonus;

	armpro = magic_negation(mdef);
	if (mdef->data->mr >= 49) armpro++; /* highly magic resistant monsters should have magic cancellation --Amy */
	if (mdef->data->mr >= 69) armpro++;
	if (mdef->data->mr >= 99) armpro++;
	/* since hero can't be cancelled, only defender's armor applies */
	negated = !((rn2(3) >= armpro) || !rn2(50));

	if (hit_as_one(&youmonst))    enchantlvl = 1; 
	if (hit_as_two(&youmonst))    enchantlvl = 2;         
	if (hit_as_three(&youmonst))  enchantlvl = 3; 
	if (hit_as_four(&youmonst))   enchantlvl = 4;         

	if (!(uarmf && uarmf->oartifact == ART_KILLCAP) && !(uwep && uwep->oartifact == ART_AP_) ) {
		if (need_one(mdef)   && enchantlvl < 1 && rn2(isfriday ? 5 : 3)) noeffect = TRUE;
		if (need_two(mdef)   && enchantlvl < 2 && rn2(isfriday ? 5 : 3)) noeffect = TRUE;
		if (need_three(mdef) && enchantlvl < 3 && rn2(isfriday ? 5 : 3)) noeffect = TRUE;
		if (need_four(mdef)  && enchantlvl < 4 && rn2(isfriday ? 5 : 3)) noeffect = TRUE;
	}

	/* summoning demons should only happen while polymorphed or being certain races, otherwise there's bugs --Amy */
	if (( (is_demon(youmonst.data) && Upolyd) || (Race_if(PM_PLAYER_SHEEP) && !Upolyd && u.ulevel >= 20) || (Race_if(PM_GAVIL) && !Upolyd) )
		&& !rn2(23)
		&& u.umonnum != PM_SUCCUBUS && u.umonnum != PM_INCUBUS
		&& u.umonnum != PM_BALROG && u.umonnum != PM_NEWS_DAEMON
		&& u.umonnum != PM_PRINTER_DAEMON) {

	    demonpet();
	    return(0);
	}

	if (mattk->aatyp == AT_CLAW && u.nailpolish) {
		tmp += (u.nailpolish * 2);
		if (rnd(10) <= u.nailpolish) {
			u.nailpolish--;
			pline(u.nailpolish ? "One of your nails loses its polish." : "Your nail loses its polish.");
		}
	}

	youdamagebonus = 100;

	if (Upolyd && !PlayerCannotUseSkills) { /* bonus damage for chars who are good at polymorphing --Amy */
		switch (P_SKILL(P_POLYMORPHING)) {

	      	case P_BASIC:	youdamagebonus += 16; break;
	      	case P_SKILLED:	youdamagebonus += 32; break;
	      	case P_EXPERT:	youdamagebonus += 50; break;
	      	case P_MASTER:	youdamagebonus += 75; break;
	      	case P_GRAND_MASTER:	youdamagebonus += 100; break;
	      	case P_SUPREME_MASTER:	youdamagebonus += 150; break;
			default: break;

		}

		if (powerfulimplants() && uimplant) {
			switch (P_SKILL(P_IMPLANTS)) { /* more bonus when using a good implant without hands --Amy */

		      	case P_BASIC:	youdamagebonus += 16; break;
		      	case P_SKILLED:	youdamagebonus += 32; break;
		      	case P_EXPERT:	youdamagebonus += 50; break;
		      	case P_MASTER:	youdamagebonus += 75; break;
		      	case P_GRAND_MASTER:	youdamagebonus += 100; break;
		      	case P_SUPREME_MASTER:	youdamagebonus += 150; break;
				default: break;

			}

		} else if (uimplant) {

			switch (P_SKILL(P_IMPLANTS)) { /* less bonus when using implant with hands --Amy */

		      	case P_BASIC:	youdamagebonus += 10; break;
		      	case P_SKILLED:	youdamagebonus += 20; break;
		      	case P_EXPERT:	youdamagebonus += 30; break;
		      	case P_MASTER:	youdamagebonus += 40; break;
		      	case P_GRAND_MASTER:	youdamagebonus += 50; break;
		      	case P_SUPREME_MASTER:	youdamagebonus += 60; break;
				default: break;

			}

		}

	}

	if (Upolyd) {
		/* and a little help if your experience level is very high, to not make polyselfing obsolete later on --Amy */
		int overlevelled = 0;
		if (u.ulevel > mons[u.umonnum].mlevel) overlevelled = (u.ulevel - mons[u.umonnum].mlevel) * 2;
		if (overlevelled > 0) {
			youdamagebonus += overlevelled;
		}

		if (ACURR(A_STR) > 12) { /* strength shouldn't be completely irrelevant for non-weapon attacks! --Amy */
			if (ACURR(A_STR) == 13) youdamagebonus += 1;
			else if (ACURR(A_STR) == 14) youdamagebonus += 2;
			else if (ACURR(A_STR) == 15) youdamagebonus += 3;
			else if (ACURR(A_STR) == 16) youdamagebonus += 4;
			else if (ACURR(A_STR) == 17) youdamagebonus += 5;
			else if (ACURR(A_STR) == 18) youdamagebonus += 6;
			else if (ACURR(A_STR) <= STR18(20)) youdamagebonus += 8;
			else if (ACURR(A_STR) <= STR18(40)) youdamagebonus += 10;
			else if (ACURR(A_STR) <= STR18(60)) youdamagebonus += 12;
			else if (ACURR(A_STR) <= STR18(80)) youdamagebonus += 14;
			else if (ACURR(A_STR) <= STR18(100)) youdamagebonus += 16;
			else if (ACURR(A_STR) == STR18(100)) youdamagebonus += 18;
			else if (ACURR(A_STR) == STR19(19)) youdamagebonus += 20;
			else if (ACURR(A_STR) == STR19(20)) youdamagebonus += 22;
			else if (ACURR(A_STR) == STR19(21)) youdamagebonus += 24;
			else if (ACURR(A_STR) == STR19(22)) youdamagebonus += 26;
			else if (ACURR(A_STR) == STR19(23)) youdamagebonus += 28;
			else if (ACURR(A_STR) == STR19(24)) youdamagebonus += 30;
			else if (ACURR(A_STR) >= STR19(25)) youdamagebonus += 33;
		}

	}

	if (youdamagebonus > 100 && (tmp > 1 || (tmp == 1 && youdamagebonus >= 150) )) {

		tmp *= youdamagebonus;
		tmp /= 100;

	}

	switch(mattk->adtyp) {
	    case AD_STUN:
	    case AD_FUMB:
	    case AD_DROP:
	    case AD_TREM:
	    case AD_SOUN:
		if(!Blind && !rn2(3))
		    pline("%s %s for a moment.", Monnam(mdef),
			  makeplural(stagger(mdef->data, "stagger")));
		mdef->mstun = 1;
		goto physical;
	    case AD_LEGS:
	     /* if (u.ucancelled) { */
	     /*    tmp = 0;	    */
	     /*    break;	    */
	     /* }		    */
		goto physical;
	    case AD_WERE:	    /* no special effect on monsters */
	    case AD_HEAL:	    /* likewise */
	    case AD_PHYS:
 physical:
		if(mattk->aatyp == AT_WEAP) {
		    if(uwep) tmp = 0;
		} else if(mattk->aatyp == AT_KICK) {
		    if(thick_skinned(mdef->data) && tmp) tmp = 1;
		    if((is_shade(mdef->data) || mdef->egotype_shader) && !(uwep && uwep->oartifact == ART_AP_) && !(uarmf && (objects[uarmf->otyp].oc_material == MT_SILVER || objects[uarmf->otyp].oc_material == MT_ARCANIUM)) ) {
			if (!(uarmf && uarmf->blessed)) {
			    impossible("bad shade attack function flow?");
			    tmp = 0;
			} else
			    tmp = rnd(4); /* bless damage */
		    }
		} else if(mattk->aatyp == AT_HUGS &&
			u.umonnum == PM_ROPE_GOLEM) {
		    if (breathless(mdef->data) || mdef->egotype_undead ) tmp = (tmp + 1) / 2;
		}

		if (Race_if(PM_PLAYER_JABBERWOCK) && !Upolyd && mattk->aatyp != AT_WEAP) {
			int jabdamage = (u.ulevel * 2 / 3);
			if (jabdamage < 1) jabdamage = 1;
			if (jabdamage > 20) jabdamage = 20;
			tmp = rnd(jabdamage);
		}

		break;
	    case AD_FIRE:
		if (negated) {
		    tmp = 0;
		    break;
		}
		if (rn2(3)) break;
		if (!Blind)
		    pline("%s is %s!", Monnam(mdef),
			  on_fire(mdef->data, mattk));
		if (pd == &mons[PM_STRAW_GOLEM] ||
		    pd == &mons[PM_PAPER_GOLEM]) {
		    if (!Blind)
		    	pline("%s burns completely!", Monnam(mdef));
		    xkilled(mdef,2);
		    tmp = 0;
		    break;
		    /* Don't return yet; keep hp<1 and tmp=0 for pet msg */
		}
		if (pd == &mons[PM_STRAW_GOLEM] ||
			pd == &mons[PM_PAPER_GOLEM] ||
		    pd == &mons[PM_WAX_GOLEM]) {
		    if (!Blind)
			pline("%s falls to pieces!", Monnam(mdef));
			xkilled(mdef,3);
			return(2);
		}
		if (!rn2(33)) tmp += destroy_mitem(mdef, SCROLL_CLASS, AD_FIRE);
		if (!rn2(33)) tmp += destroy_mitem(mdef, SPBOOK_CLASS, AD_FIRE);
		if (resists_fire(mdef) && !player_will_pierce_resistance()) {
		    if (!Blind)
			pline_The("fire doesn't heat %s!", mon_nam(mdef));
		    golemeffects(mdef, AD_FIRE, tmp);
		    shieldeff(mdef->mx, mdef->my);
		    tmp = 0;
		}
		/* only potions damage resistant players in destroy_item */
		if (!rn2(33)) tmp += destroy_mitem(mdef, POTION_CLASS, AD_FIRE);
		break;
	    case AD_COLD:
		if (negated) {
		    tmp = 0;
		    break;
		}
		if (rn2(3)) break;
		if (!Blind) pline("%s is covered in frost!", Monnam(mdef));
		if (resists_cold(mdef) && !player_will_pierce_resistance()) {
		    shieldeff(mdef->mx, mdef->my);
		    if (!Blind)
			pline_The("frost doesn't chill %s!", mon_nam(mdef));
		    golemeffects(mdef, AD_COLD, tmp);
		    tmp = 0;
		}
		if (!rn2(33)) tmp += destroy_mitem(mdef, POTION_CLASS, AD_COLD);
		break;
	    case AD_ELEC:
		if (negated) {
		    tmp = 0;
		    break;
		}
		if (rn2(3)) break;
		if (!Blind) pline("%s is zapped!", Monnam(mdef));
		if (!rn2(33)) tmp += destroy_mitem(mdef, WAND_CLASS, AD_ELEC);
		if (resists_elec(mdef) && !player_will_pierce_resistance()) {
		    if (!Blind)
			pline_The("zap doesn't shock %s!", mon_nam(mdef));
		    golemeffects(mdef, AD_ELEC, tmp);
		    shieldeff(mdef->mx, mdef->my);
		    tmp = 0;
		}
		/* only rings damage resistant players in destroy_item */
		if (!rn2(33)) tmp += destroy_mitem(mdef, RING_CLASS, AD_ELEC);
		break;
	    case AD_ACID:
		if (resists_acid(mdef) && !player_will_pierce_resistance()) tmp = 0;
		break;
	    case AD_STON:
	    case AD_EDGE:
		if (!munstone(mdef, TRUE) && !rn2(4)) {
		    minstapetrify(mdef, TRUE);
		tmp = 0;
		}
		break;
	    case AD_SSEX:
	    case AD_SEDU:
	    case AD_SITM:
	    case AD_STTP:
		    /*Your("stealing attack goes off"); for debug purposes */
		if (!rn2(4)) steal_it(mdef, mattk);
		tmp = 0;
		break;
	    case AD_SGLD:
#ifndef GOLDOBJ
		if (mdef->mgold) {
		    u.ugold += mdef->mgold;
		    if (practicantterror && (mdef->mgold > 0)) {
			pline("%s thunders: 'Theft detected! You have to pay twice the amount that you stole.'", noroelaname());
			fineforpracticant(mdef->mgold * 2, 0, 0);

		    }
		    mdef->mgold = 0;
		    Your("purse feels heavier.");
		}
#else
                /* This you as a leprechaun, so steal
                   real gold only, no lesser coins */
	        {
		    struct obj *mongold = findgold(mdef->minvent);
	            if (mongold) {
		        obj_extract_self(mongold);  
		        if (merge_choice(invent, mongold) || inv_cnt() < 52) {
			    addinv(mongold);
			    Your("purse feels heavier.");
			} else {
                            You("grab %s's gold, but find no room in your knapsack.", mon_nam(mdef));
			    dropy(mongold);
		        }
		    }
	        }
#endif
		exercise(A_DEX, TRUE);
		tmp = 0;
		break;
	    case AD_TLPT:
	    case AD_NEXU:
	    case AD_BANI:
	    case AD_ABDC:
		if (tmp <= 0) tmp = 1;
		if (!negated && tmp < mdef->mhp && !rn2(4)) {
		    char nambuf[BUFSZ];
		    boolean u_saw_mon = canseemon(mdef) ||
					(u.uswallow && u.ustuck == mdef);
		    /* record the name before losing sight of monster */
		    strcpy(nambuf, Monnam(mdef));
		    if (u_teleport_mon(mdef, FALSE) &&
			    u_saw_mon && !canseemon(mdef))
			pline("%s suddenly disappears!", nambuf);
		}
		break;
	    case AD_BLND:
		if (can_blnd(&youmonst, mdef, mattk->aatyp, (struct obj*)0) && !rn2(3) ) {
		    if(!Blind && mdef->mcansee)
			pline("%s is blinded.", Monnam(mdef));
		    mdef->mcansee = 0;
		    tmp += mdef->mblinded;
		    if (tmp > 127) tmp = 127;
		    mdef->mblinded = tmp;
		}
		tmp = 0;
		break;
	    case AD_CURS:
	    case AD_ICUR:
	    case AD_NACU:

		if (night() && !rn2(10) && !mdef->mcan && (rnd(100) > mdef->data->mr) ) {

		    if (mdef->data == &mons[PM_CLAY_GOLEM]) {
			if (!Blind)
			    pline("Some writing vanishes from %s head!",
				s_suffix(mon_nam(mdef)));
			xkilled(mdef, 0);
			/* Don't return yet; keep hp<1 and tmp=0 for pet msg */
		    } else {
			cancelmonsterlite(mdef);
			You("chuckle.");
		    }
		}
		tmp = 0;
		break;
	    case AD_PAIN:
		if (mdef->mhp > 9) tmp += (mdef->mhp / 10);
		pline("%s shrieks in pain!", Monnam(mdef));
		break;

	    case AD_DRLI:
	    case AD_TIME:
	    case AD_DFOO:
	    case AD_WEEP:
	    case AD_VAMP:
		if (!negated && !rn2(3) && !resists_drli(mdef)) {
			int xtmp = d(2,6);
			if (mdef->mhp < xtmp) xtmp = mdef->mhp;
			if (maybe_polyd(is_vampire(youmonst.data), 
			    (Race_if(PM_VAMPIRE) || Race_if(PM_SUCKING_FIEND) || Race_if(PM_VAMGOYLE)) ) && mattk->aatyp == AT_BITE &&
			    has_blood(pd) && (yn_function("Drink blood?", ynchars, 'y') == 'y') ) {
				/* fixing stupid overeating issues --Amy */
				/* For the life of a creature is in the blood
				   (Lev 17:11) */
				if (flags.verbose)
				    You("feed on the lifeblood.");
				/* [ALI] Biting monsters does not count against
				   eating conducts. The draining of life is
				   considered to be primarily a non-physical
				   effect */
				lesshungry(xtmp * 12);
				if (Role_if(PM_BLEEDER)) { /* Bleeders get better from drinking blood */

					pline("This tastes good.");
					healup(d(5,5) + rnz(u.ulevel), 0, FALSE, FALSE);
					exercise(A_CON, TRUE);
				}
				if (Race_if(PM_HEMOPHAGE)) { /* Hemophages too */

					pline("This tastes good.");
					healup(d(5,5) + rnz(u.ulevel), 0, FALSE, FALSE);
					exercise(A_CON, TRUE);
				}
			}
			pline("%s suddenly seems weaker!", Monnam(mdef));
			mdef->mhpmax -= xtmp;
#ifdef SHOW_DMG
			if (xtmp < mdef->mhp) showdmg(xtmp);
#endif
			if ((mdef->mhp -= xtmp) <= 0 || !mdef->m_lev) {
				pline("%s dies!", Monnam(mdef));
				xkilled(mdef,0);
			} else
				mdef->m_lev--;
		tmp = 0;
		}
		break;
	    case AD_RUST:
		if (pd == &mons[PM_IRON_GOLEM]) {
			pline("%s falls to pieces!", Monnam(mdef));
			xkilled(mdef,0);
		}
		if (!rn2(3)) hurtmarmor(mdef, AD_RUST);
		if (pd == &mons[PM_IRON_GOLEM]) tmp = 0;
		break;
	    case AD_CORR:
		if (!rn2(3)) hurtmarmor(mdef, AD_CORR);
		break;
	    case AD_DCAY:
		if (pd == &mons[PM_WOOD_GOLEM] ||
		    pd == &mons[PM_LEATHER_GOLEM]) {
			pline("%s falls to pieces!", Monnam(mdef));
			xkilled(mdef,0);
		}
		if (!rn2(3)) hurtmarmor(mdef, AD_DCAY);
		if (pd == &mons[PM_WOOD_GOLEM] || pd == &mons[PM_LEATHER_GOLEM]) tmp = 0;
		break;
	    case AD_FLAM:
	    case AD_WNCE:
		if (!rn2(3)) hurtmarmor(mdef, AD_FLAM);
		break;
	    case AD_DRST:
	    case AD_DRDX:
	    case AD_DRCO:
	    case AD_POIS:
	    case AD_STAT:
	    case AD_WISD:
	    case AD_DRCH:
		if (!negated && !rn2(8)) {
		    Your("%s was poisoned!", mpoisons_subj(&youmonst, mattk));
		    if (resists_poison(mdef) && !player_will_pierce_resistance())
			pline_The("poison doesn't seem to affect %s.",
				mon_nam(mdef));
		    else {
			if (!rn2(100) && !resists_poison(mdef)) {
			    Your("poison was deadly...");
			    tmp = mdef->mhp;
			} else tmp += rn1(10,6);
		    }
		}
		break;
	    case AD_VENO:
		if (!negated && !rn2(3)) {
		    if (resists_poison(mdef) && !player_will_pierce_resistance())
			pline_The("poison doesn't seem to affect %s.",
				mon_nam(mdef));
		    else {
			pline("%s is badly poisoned!", Monnam(mdef));
			if (!rn2(10) && !resists_poison(mdef)) {
			    Your("poison was deadly...");
			    tmp = mdef->mhp;
			} else tmp += rn1(20,12);
		    }
		}
		break;
	    case AD_LITE:
		if (is_vampire(mdef->data)) {
			tmp *= 2; /* vampires take more damage from sunlight --Amy */
			pline("%s is irradiated!", Monnam(mdef));
		}
		break;
	    case AD_DARK:
		do_clear_area(mdef->mx,mdef->my, 7, set_lit, (void *)((char *)0));
		pline("You generate a sinister darkness!");
		if (mdef->data->mlet == S_ANGEL) tmp *= 2;
		break;
	    case AD_THIR:
	    case AD_NTHR:
		healup(tmp + (u.ulevel / 2), 0, FALSE, FALSE);
		pline("You suck %s's %s!", mon_nam(mdef), mbodypart(mdef, BLOOD) );
		if (Race_if(PM_BURNINATOR)) {
			u.uen += (tmp + (u.ulevel / 2));
			if (u.uen > u.uenmax) u.uen = u.uenmax;
		}
		break;

	    case AD_RAGN:
		ragnarok(FALSE);
		if (evilfriday && u.ulevel > 1) evilragnarok(FALSE,u.ulevel);
		break;

	    case AD_AGGR:

		incr_itimeout(&HAggravate_monster, tmp);
		You_feel("that monsters are aware of your presence.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Dazhe sovetskaya Pyat' Lo obostryayetsya v vashem nizkom igrovom masterstve." : "Woaaaaaah!");
		aggravate();
		if (!rn2(20)) {

			int aggroamount = rnd(6);
			if (isfriday) aggroamount *= 2;
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
			while (aggroamount) {

				u.cnd_aggravateamount++;
				makemon((struct permonst *)0, u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
				aggroamount--;
				if (aggroamount < 0) aggroamount = 0;
			}
			u.aggravation = 0;
			pline("Several monsters come out of a portal.");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

		}

		break;

	    case AD_CONT:

		if (!rn2(30)) {
			mdef->isegotype = 1;
			mdef->egotype_contaminator = 1;
		}
		if (!rn2(100)) {
			mdef->isegotype = 1;
			mdef->egotype_weeper = 1;
		}
		if (!rn2(250)) {
			mdef->isegotype = 1;
			mdef->egotype_radiator = 1;
		}
		if (!rn2(250)) {
			mdef->isegotype = 1;
			mdef->egotype_reactor = 1;
		}

		break;

	    case AD_FRZE:
		if (!resists_cold(mdef) && resists_fire(mdef)) {
			tmp *= 2;
			pline("%s is freezing!", Monnam(mdef));
		}

		break;

	    case AD_ICEB:
		if (!resists_cold(mdef)) {
			tmp *= 2;
			pline("%s feels ice-cold!", Monnam(mdef));
		}

		break;

	    case AD_CHKH:
		if (u.ulevel > mdef->m_lev) tmp += (u.ulevel - mdef->m_lev);
		break;

	    case AD_HODS:
		tmp += mdef->m_lev;
		break;

	    case AD_DIMN:
		tmp += u.ulevel;
		break;

	    case AD_MALK:
		if (!resists_elec(mdef)) {
			tmp *= 2;
			pline("%s is shocked!", Monnam(mdef));
		}

		break;

	    case AD_CHRN:
		if ((tmp > 0) && (mdef->mhpmax > 1)) {
			mdef->mhpmax--;
			pline("%s feels bad!", Monnam(mdef));
		}
		break;

	    case AD_UVUU:
		if (has_head(mdef->data)) {
			tmp *= 2;
			if (!rn2(1000)) {
				tmp *= 100;
				pline("You split %s's %s in two!", mon_nam(mdef), mbodypart(mdef, HEAD));
			} else pline("You spike %s's %s!", mon_nam(mdef), mbodypart(mdef, HEAD));
		}
		break;

	    case AD_GRAV:
		if (!is_flyer(mdef->data)) {
			tmp *= 2;
			pline("You slam %s into the ground!", mon_nam(mdef));
		}
		break;

	    case AD_BURN:
		if (resists_cold(mdef) && !resists_fire(mdef)) {
			tmp *= 2;
			pline("%s is burning!", Monnam(mdef));
		}

		break;

	    case AD_PLAS:
		if (!resists_fire(mdef)) {
			tmp *= 2;
			pline("%s is enveloped by searing plasma radiation!", Monnam(mdef));
		}

		break;

	    case AD_SLUD:
		if (!resists_acid(mdef)) {
			tmp *= 2;
			pline("%s is covered with sludge!", Monnam(mdef));
		}

		break;

	    case AD_LAVA:
		if (resists_cold(mdef) && !resists_fire(mdef)) {
			tmp *= 4;
			pline("%s is scorched!", Monnam(mdef));
		} else if (!resists_fire(mdef)) {
			tmp *= 2;
			pline("%s is severely burned!", Monnam(mdef));
		}

		break;

	    case AD_FAKE:
		pline("%s", fauxmessage());
		u.cnd_plineamount++;
		if (!rn2(3)) {
			pline("%s", fauxmessage());
			u.cnd_plineamount++;
		}

		break;

	    case AD_WEBS:
		(void) maketrap(mdef->mx, mdef->my, WEB, 0, FALSE);
		if (!rn2(issoviet ? 2 : 8)) makerandomtrap(FALSE);

		break;

	    case AD_TRAP:
		if (t_at(mdef->mx, mdef->my) == 0) (void) maketrap(mdef->mx, mdef->my, rndtrap(), 0, FALSE);
		else makerandomtrap(FALSE);

		break;

	    case AD_CNCL:
		if (rnd(100) > mdef->data->mr) {
			cancelmonsterlite(mdef);
			pline("%s is covered in sparkling lights!", Monnam(mdef));
		}

		break;

	    case AD_FEAR:
		if (rnd(100) > mdef->data->mr) {
		     monflee(mdef, rnd(1 + tmp), FALSE, TRUE);
			pline("%s screams in fear!",Monnam(mdef));
		}

		break;

	    case AD_SANI:
		if (!rn2(10)) {
			mdef->mconf = 1;
			switch (rnd(4)) {

				case 1:
					pline("%s sees you chow dead bodies.", Monnam(mdef)); break;
				case 2:
					pline("%s shudders at your terrifying %s.", Monnam(mdef), makeplural(body_part(EYE)) ); break;
				case 3:
					pline("%s feels sick at entrails caught in your tentacles.", Monnam(mdef)); break;
				case 4:
					pline("%s sees maggots breed in your rent %s.", Monnam(mdef), body_part(STOMACH)); break;

			}

		}

		break;

	    case AD_INSA:
		if (rnd(100) > mdef->data->mr) {
		     monflee(mdef, rnd(1 + tmp), FALSE, TRUE);
			pline("%s screams in fear!",Monnam(mdef));
		}
		if (!mdef->mconf && !rn2(3)) {
		    if (canseemon(mdef))
			pline("%s looks confused.", Monnam(mdef));
		    mdef->mconf = 1;
		}
		if(!Blind && !rn2(3))
		    pline("%s %s for a moment.", Monnam(mdef),
			  makeplural(stagger(mdef->data, "stagger")));
		mdef->mstun = 1;

		break;

	    case AD_DREA:
		if (!mdef->mcanmove) {
			tmp *= 4;
			pline("You eat %s's dream!", mon_nam(mdef));
			u.uconduct.food++;
			if (FemtrapActiveNora) {
				You("vomit.");
				vomit();
				morehungry(20);
			}
			morehungry(-rnd(10)); /* cannot choke */
		}

		break;

	    case AD_SPC2:
		if (!mdef->mconf) {
		    if (canseemon(mdef))
			pline("%s looks confused.", Monnam(mdef));
		    mdef->mconf = 1;
		}
		else
		{
		    if (canseemon(mdef))
			pline("%s is getting more and more confused.",
				Monnam(mdef));
		    mdef->mconf++;
		}
		break;

	    case AD_DRIN:
		if (notonhead || !has_head(mdef->data)) {
		    pline("%s doesn't seem harmed.", Monnam(mdef));
		    tmp = 0;
		    if (!Unchanging && slime_on_touch(mdef->data) ) {
			if (!Slimed) {
			    You("suck in some slime and don't feel very well.");
				make_slimed(100);
			    killer_format = KILLED_BY_AN;
			    delayed_killer = "slimed by sucking some slime through their nose";
			}
		    }
		    break;
		}
		if (m_slips_free(mdef, mattk)) break;

		if ((mdef->misc_worn_check & W_ARMH) && rn2(8)) {
		    pline("%s helmet blocks your attack to %s head.",
			  s_suffix(Monnam(mdef)), mhis(mdef));
		    break;
		}
		if (rn2(5)) break; /* would otherwise be way too overpowered --Amy */

		You("eat %s brain!", s_suffix(mon_nam(mdef)));
		u.uconduct.food++;
		if (FemtrapActiveNora) {
			You("vomit.");
			vomit();
			morehungry(20);
		}
		if (touch_petrifies(mdef->data) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && !Stoned) {
			if (Hallucination && rn2(10)) pline("You are already stoned.");
			else {
				You("start turning to stone.");
				Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
				u.cnd_stoningcount++;
				killer_format = KILLED_BY_AN;
				delayed_killer = mdef->data->mname;
			}
		}
		if (!vegan(mdef->data))
		    u.uconduct.unvegan++;
		if (!vegetarian(mdef->data))
		    violated_vegetarian();
		if (mindless(mdef->data) || mdef->egotype_undead) {
		    pline("%s doesn't notice.", Monnam(mdef));
		    break;
		}
		tmp += rnd(10);
		morehungry(-rnd(30)); /* cannot choke */
		if (ABASE(A_INT) < AMAX(A_INT)) {
			ABASE(A_INT) += rnd(4);
			if (ABASE(A_INT) > AMAX(A_INT))
				ABASE(A_INT) = AMAX(A_INT);
			flags.botl = 1;
		}
		exercise(A_WIS, TRUE);
		break;
	    case AD_STCK:
		if (!negated && !sticks(mdef->data))
		    setustuck(mdef); /* it's now stuck to you */
		break;
	    case AD_WRAP:
		if (!sticks(mdef->data)) {
		    if (!u.ustuck && !rn2(10)) {
			if (m_slips_free(mdef, mattk)) {
			    tmp = 0;
			} else {
			    You("swing yourself around %s!",
				  mon_nam(mdef));
			    setustuck(mdef);
			}
		    } else if(u.ustuck == mdef) {
			/* Monsters don't wear amulets of magical breathing */
			if (!rn2(20) && is_drowningpool(u.ux,u.uy) && !is_swimmer(mdef->data) && !mdef->egotype_watersplasher &&
			    !amphibious(mdef->data)) {
			    You("drown %s...", mon_nam(mdef));
			    tmp = mdef->mhp;
			} else if(mattk->aatyp == AT_HUGS)
			    pline("%s is being crushed.", Monnam(mdef));
		    } else {
			tmp = 0;
			if (flags.verbose)
			    You("brush against %s %s.",
				s_suffix(mon_nam(mdef)),
				mbodypart(mdef, LEG));
		    }
		} else tmp = 0;
		break;
	    case AD_PLYS:
		if (!negated && mdef->mcanmove && !(mdef->m_lev > 1 && (rnd(mdef->m_lev) > u.ulevel)) && !(dmgtype(mdef->data, AD_PLYS)) && !rn2(3) && tmp < mdef->mhp) {
		    if (!Blind) pline("%s is frozen by you!", Monnam(mdef));
		    int parlyzdur = rnd(4);
		    if (tmp > 4 && !rn2(3)) parlyzdur = rnd(tmp);
		    if (parlyzdur > 1) parlyzdur = rnd(parlyzdur);
		    if (parlyzdur > 127) parlyzdur = 127;
		    mdef->mcanmove = 0;
		    mdef->mfrozen = parlyzdur;
		}
		break;
	    case AD_TCKL:
		if (!negated && mdef->mcanmove && !(mdef->m_lev > 1 && (rnd(mdef->m_lev) > u.ulevel)) && !(dmgtype(mdef->data, AD_PLYS)) && !rn2(5) && tmp < mdef->mhp) {
		    if (!Blind) You("mercilessly tickle %s!", mon_nam(mdef));
		    int parlyzdur = rnd(3);
		    if (tmp > 3 && !rn2(5)) parlyzdur = rnd(tmp);
		    if (parlyzdur > 1) parlyzdur = rnd(parlyzdur);
		    if (parlyzdur > 127) parlyzdur = 127;
		    mdef->mcanmove = 0;
		    mdef->mfrozen = parlyzdur;
		}
		break;
	    case AD_BLEE:
		if (tmp > 0) {
			mdef->bleedout += tmp;
			pline("%s sustains a cut.", Monnam(mdef));
		}
		break;
	    case AD_TERR:
		terrainterror(0);
		break;
	    case AD_SLEE:
		if (mattk->aatyp == AT_GAZE && mon_reflects(mdef, (char *)0)) {
		    tmp = 0;
		    (void) mon_reflects(mdef, "But it reflects from %s %s!");
		    if ((Sleep_resistance || Free_action) && rn2(StrongSleep_resistance ? 20 : 5)) {
			pline("You yawn.");
			break;
		    } else {
			nomul(-rnd(10), "sleeping from your own reflected gaze", TRUE);
			u.usleep = 1;
			nomovemsg = "You wake up.";
			if (Blind)  You("are put to sleep!");
			else You("are put to sleep by your reflected gaze!");
			break;
		    }
		}

		{
		int parlyzdur = rnd(5);
		if (tmp > 5 && !rn2(3)) parlyzdur = rnd(tmp);
		if (parlyzdur > 1) parlyzdur = rnd(parlyzdur);
		if (parlyzdur > 127) parlyzdur = 127;

		if (rn2(2) && !(mdef->m_lev > 1 && (rnd(mdef->m_lev) > u.ulevel)) && (rn2(100) > mdef->data->mr) && !negated && !mdef->msleeping && /* drow nerf --Amy */
			(mattk->aatyp != AT_WEAP || barehanded_hit) && sleep_monst(mdef, parlyzdur, -1)) {
		    if (!Blind)
			pline("%s is put to sleep by you!", Monnam(mdef));
		    slept_monst(mdef);
		}
		else
		    tmp = 0;

		}

		break;
	    case AD_SLIM: /* no easy sliming Death or Famine --Amy */
	    case AD_LITT:
		if (negated || (rn2(100) < mdef->data->mr) ) break;	/* physical damage only */
		if (!rn2(400) && !flaming(mdef->data) && !slime_on_touch(mdef->data) ) {
		    You("turn %s into slime.", mon_nam(mdef));
		    (void) newcham(mdef, &mons[PM_GREEN_SLIME], FALSE, !Blind);
		    tmp = 0;
		}
		break;
	    case AD_ENCH:	/* KMH -- remove enchantment (disenchanter) */
		/* There's no msomearmor() function, so just do damage */
	     /* if (negated) break; */
		break;
	    case AD_SLOW:
	    case AD_WGHT:
		if (!negated && (rn2(100) > mdef->data->mr) && !rn2(4) && mdef->mspeed != MSLOW) {
		    unsigned int oldspeed = mdef->mspeed;

		    mon_adjust_speed(mdef, -1, (struct obj *)0);
		    if (mdef->mspeed != oldspeed && canseemon(mdef))
			pline("%s slows down.", Monnam(mdef));
		}
		break;
	    case AD_INER:
		if (!negated && !rn2(4) && mdef->mspeed != MSLOW) {
		    unsigned int oldspeed = mdef->mspeed;

		    mon_adjust_speed(mdef, -1, (struct obj *)0);
		    if (mdef->mspeed != oldspeed && canseemon(mdef))
			pline("%s slows down.", Monnam(mdef));
		}
		break;
	    case AD_LAZY:
		if (!negated && !rn2(4) && mdef->mspeed != MSLOW) {
		    unsigned int oldspeed = mdef->mspeed;

		    mon_adjust_speed(mdef, -1, (struct obj *)0);
		    if (mdef->mspeed != oldspeed && canseemon(mdef))
			pline("%s slows down.", Monnam(mdef));
		}
		if (!negated && mdef->mcanmove && !(mdef->m_lev > 1 && (rnd(mdef->m_lev) > u.ulevel)) && !(dmgtype(mdef->data, AD_PLYS)) && !rn2(10) && tmp < mdef->mhp) {

		    int parlyzdur = rnd(4);
		    if (tmp > 4 && !rn2(3)) parlyzdur = rnd(tmp);
		    if (parlyzdur > 1) parlyzdur = rnd(parlyzdur);
		    if (parlyzdur > 127) parlyzdur = 127;

		    if (!Blind) pline("%s is frozen by you!", Monnam(mdef));
		    mdef->mcanmove = 0;
		    mdef->mfrozen = parlyzdur;
		}
		break;
	    case AD_NUMB:
		if (!negated && !rn2(40) && mdef->mspeed != MSLOW) {
		    unsigned int oldspeed = mdef->mspeed;

		    mon_adjust_speed(mdef, -1, (struct obj *)0);
		    if (mdef->mspeed != oldspeed && canseemon(mdef))
			pline("%s is numbed.", Monnam(mdef));
		}
		break;
	    case AD_CONF:
	    case AD_HALU:
	    case AD_DEPR:
		if (!mdef->mconf && !rn2(3)) {
		    if (canseemon(mdef))
			pline("%s looks confused.", Monnam(mdef));
		    mdef->mconf = 1;
		}
		else if (!rn2(3) && mdef->mconf)
		{
		    if (canseemon(mdef))
			pline("%s is getting more and more confused.",
				Monnam(mdef));
		    mdef->mconf++;
		}
		break;
	    case AD_POLY:
		if (tmp < mdef->mhp && !rn2(20)) {
		    if (resists_magm(mdef) || (rn2(100) < mdef->data->mr) ) { /* no easy taming Death or Famine! --Amy */
			/* magic resistance protects from polymorph traps,
			 * so make it guard against involuntary polymorph
			 * attacks too... */
			shieldeff(mdef->mx, mdef->my);
#if 0
		    } else if (!rn2(25) || !mon_poly(mdef)) {
			if (canseemon(mdef)) {
			    pline("%s shudders!", Monnam(mdef));
			}
			/* no corpse after system shock */
			tmp = rnd(30);
#endif
		    } else if (!mon_poly(mdef, TRUE,
			    "%s undergoes a freakish metamorphosis!"))
			/* prevent killing the monster again - 
			 * could be killed in mon_poly */
			tmp = 0;
		}
		break;

	    case AD_CHAO:
		if (tmp < mdef->mhp && !rn2(20)) {
		    if (resists_magm(mdef) || (rn2(100) < mdef->data->mr) ) { /* no easy taming Death or Famine! --Amy */
			/* magic resistance protects from polymorph traps,
			 * so make it guard against involuntary polymorph
			 * attacks too... */
			shieldeff(mdef->mx, mdef->my);
		    } else if (!mon_poly(mdef, TRUE,
			    "%s undergoes a freakish metamorphosis!"))
			/* prevent killing the monster again - 
			 * could be killed in mon_poly */
			tmp = 0;
		}

		if ((tmp > 0) && (mdef && mdef->mhpmax > 1)) {
			mdef->mhpmax--;
			pline("%s feels bad!", Monnam(mdef));
		}

		break;

		/* WAC -- for death gazes - but all messages should be generic */
	    case AD_DETH:
		if (rn2(16)) {
		    /* Just damage */
		    break;
		}
		if (mattk->aatyp == AT_GAZE) 
		    You("look directly at %s!", mon_nam(mdef));
		if ((mattk->aatyp == AT_GAZE) && (mon_reflects(mdef, (char *)0))) {
		    /* WAC reflected gaze 
		     * Oooh boy...that was a bad move :B 
		     */
		    tmp = 0;
		    shieldeff(mdef->mx, mdef->my);
		    (void) mon_reflects(mdef, "But it reflects from %s %s!");
		    if (PlayerResistsDeathRays) {
			You("shudder momentarily...");
			break;
		    }
			u.youaredead = 1;
		    You(isangbander ? "have died." : "die...");
		    killer_format = KILLED_BY;
		    killer = "a reflected gaze of death";
		    done(DIED);
			u.youaredead = 0;
		} else if (is_undead(mdef->data) || mdef->egotype_undead) {
		    /* Still does normal damage */
		    if (!Blind) pline("Something didn't work...");
		    break;
		} else if (resists_magm(mdef)) {
		    if (!Blind)
			pline("%s shudders momentarily...", Monnam(mdef));
		} else {
		    tmp = mdef->mhp;
		}
		break;
	    case AD_WRAT:
	    case AD_MANA:
	    case AD_TECH:
	    case AD_MEMO:
	    case AD_TRAI:
	    	    mon_drain_en(mdef, ((mdef->m_lev > 0) ? (rnd(mdef->m_lev)) : 0) + 1 + tmp);
		break;
	    case AD_FAMN:
		if (mdef->mtame) {
			makedoghungry(mdef, tmp * rnd(50));
			pline("%s suddenly looks hungry.", Monnam(mdef));
		}
		break;
	    case AD_DREN:
	    	if (resists_magm(mdef)) {
		    if (!Blind) {
			shieldeff(mdef->mx,mdef->my);
			pline("%s is unaffected.", Monnam(mdef));
		    }
	    	} else {
	    	    mon_drain_en(mdef, 
				((mdef->m_lev > 0) ? (rnd(mdef->m_lev)) : 0) + 1);
	    	}
		break;
	    case AD_CALM:	/* KMH -- koala attack */
		/* Certain monsters aren't even made peaceful. */
		if (!rn2(10) && !mdef->iswiz && mdef->data != &mons[PM_MEDUSA] &&
				!(mdef->data->mflags3 & M3_COVETOUS) &&
				!(mdef->data->geno & G_UNIQ)) {
		    pline("You calm %s.", mon_nam(mdef));
		    if (!mdef->mfrenzied) mdef->mpeaceful = 1;
		    mdef->mtame = 0;
		    tmp = 0;
		}
		break;

	    case AD_FREN:
		    if (!mdef->mfrenzied) pline("You frenzied %s!", mon_nam(mdef));
		    mdef->mfrenzied = 1;
		    mdef->mtame = mdef->mpeaceful = 0;

		break;

	    case AD_SAMU:
	    case AD_SPEL:
		/* obvious rule patch because the rodneyan race is way too overpowered otherwise --Amy */
		if (Race_if(PM_RODNEYAN) && !Upolyd && (rnd(GushLevel + 100) < 100)) tmp = 0;

		break;

	    default:	/*tmp = 0;*/ /*making uncommon polymorph forms with weird attacks like AD_DARK stronger. --Amy*/
		break;
	}

	if ( (Race_if(PM_HUMAN_WEREWOLF) || Role_if(PM_LUNATIC) || Race_if(PM_AK_THIEF_IS_DEAD_) ) && Upolyd && tmp) tmp += rnd(u.ulevel); /* come on, werewolves need some love too! --Amy */

	if (Upolyd && tmp && !(PlayerCannotUseSkills)) {
		switch (P_SKILL(P_POLYMORPHING)) {

	      	case P_BASIC:	tmp +=  1; break;
	      	case P_SKILLED:	tmp +=  rnd(2); break;
	      	case P_EXPERT:	tmp +=  rnd(3); break;
	      	case P_MASTER:	tmp +=  rnd(4); break;
	      	case P_GRAND_MASTER:tmp +=  rnd(5); break;
	      	case P_SUPREME_MASTER:tmp +=  rnd(6); break;
	      	default: tmp += 0; break;
	      }

	}

	mdef->mstrategy &= ~STRAT_WAITFORU; /* in case player is very fast */
	if ( (rn2(3) || issoviet) && tmp && noeffect && !DEADMONSTER(mdef)) {
	     if (!issoviet) You("don't seem to harm %s.", mon_nam(mdef));
	     else pline("Etot monstr ne mozhet byt' povrezhden, potomu chto Sovetskiy khochet nesmotrya vas.");
	     tmp = 0;
	     return 1;
	}

	if (mdef->egotype_flickerer && tmp && !DEADMONSTER(mdef)) {
		pline("%s flickers and is impervious to melee and missile attacks!", Monnam(mdef));
		if (!rn2(30)) pline("Reminder: you must use something else in order to damage this monster!");
		else if (!rn2(30)) pline("You read that right: your attacks are doing no damage at all!");
		else if (!rn2(30)) pline("Hint: try offensive wands or spells.");
	      tmp = 0;
	      return 1;
	}

	if (mdef->data == &mons[PM_LITTLE_POISON_IVY] || mdef->data == &mons[PM_CRITICALLY_INJURED_PERCENTS] || mdef->data == &mons[PM_SUPERDEEP_TYPE] || mdef->data == &mons[PM_AGULA] || mdef->data == &mons[PM_MISTER_GRIBBS] || mdef->data == &mons[PM_FLUIDATOR_IVE] || mdef->data == &mons[PM_IMMUNITY_VIRUS] || mdef->data == &mons[PM_AMBER_FEMMY] || mdef->data == &mons[PM_UNGENOCIDABLE_VAMPSHIFTER] || mdef->data == &mons[PM_TERRIFYING_POISON_IVY] || mdef->data == &mons[PM_GIRL_WITH_THE_MOST_BEAUTIFUL_SHOES_IN_THE_WORLD] || mdef->data == &mons[PM_IMMOVABLE_OBSTACLE] || mdef->data == &mons[PM_INVINCIBLE_HAEN] || mdef->data == &mons[PM_CHAREY] || mdef->data == &mons[PM_INVENTOR_OF_THE_SISTER_COMBAT_BOOTS] || mdef->data == &mons[PM_SWEET_ASIAN_POISON_IVY] || mdef->data == &mons[PM_ARABELLA_SHOE] || mdef->data == &mons[PM_ANASTASIA_SHOE] || mdef->data == &mons[PM_HENRIETTA_SHOE] || mdef->data == &mons[PM_KATRIN_SHOE] || mdef->data == &mons[PM_JANA_SHOE] || mdef->data == &mons[PM_FIRST_DUNVEGAN] || mdef->data == &mons[PM_PERCENTI_HAS_LOST___] || mdef->data == &mons[PM_PERCENTI_IS_IMMUNE_TO_THE_ATTACK_]) {

		pline("%s is IMMUNE to the attack!", Monnam(mdef));
		if (FunnyHallu) You("curse at Konami for designing it like that.");
		if (!rn2(30)) pline("Reminder: you must use something else in order to damage this monster!");
		else if (!rn2(30)) pline("You read that right: your attacks are doing no damage at all!");
		else if (!rn2(30)) pline("Hint: try offensive wands or spells.");

	      tmp = 0;
		return 1;
	}

	if (RngeWeakness && tmp > 1) tmp /= 2;

#ifdef SHOW_DMG
	if (tmp < mdef->mhp) showdmg(tmp);
#endif
	
	/* if tmp == 0, DON'T xkilled/killed the monster even if hp < 1 
	 *	- xkilled/killed via other method... */

	if((mdef->mhp -= tmp) < 1) {
	    if (mdef->mtame && !cansee(mdef->mx,mdef->my)) {
		You_feel("embarrassed for a moment.");
		if (tmp) xkilled(mdef, 0); /* !tmp but hp<1: already killed */
	    } else if (!flags.verbose) {
		You("destroy it!");
		if (tmp) xkilled(mdef, 0);
	    } else
		if (tmp) killed(mdef);
	    return(2);
	}
	return(1);
}

STATIC_OVL int
explum(mdef, mattk)
register struct monst *mdef;
register struct attack *mattk;
{
	register int tmp = d((int)mattk->damn, (int)mattk->damd);
	register struct permonst *pd = mdef->data;

	You("explode!");
	switch(mattk->adtyp) {
	    boolean resistance; /* only for cold/fire/elec */

	    case AD_BLND:
		if (!resists_blnd(mdef)) {
		    pline("%s is blinded by your flash of light!", Monnam(mdef));
		    mdef->mblinded = min((int)mdef->mblinded + tmp, 127);
		    mdef->mcansee = 0;
		}
		break;
	    case AD_STUN:
	    case AD_FUMB:
	    case AD_DROP:
	    case AD_TREM:
		if (haseyes(mdef->data) && mdef->mcansee) {
		    pline("%s is stunned by your flash of light!", Monnam(mdef));
		    mdef->mstun = 1;
		}
		break;
	    case AD_SOUN:
		pline("%s is stunned by the concussion!", Monnam(mdef));
		mdef->mstun = 1;
		goto common;

	    case AD_LEGS: /* no effect at all, this is by design --Amy */
		break;

	    case AD_HALU:
	    case AD_CONF:
		if (haseyes(mdef->data) && mdef->mcansee) {
		    pline("%s is affected by your flash of light!",
			  Monnam(mdef));
		    mdef->mconf = 1;
		}
		break;
	    case AD_DEPR:
		if (haseyes(mdef->data) && mdef->mcansee) {
		    pline("%s is affected by your flash of light!",
			  Monnam(mdef));
		    mdef->mconf = 1;
		}
		goto common;
	    case AD_POLY:
		if (!rn2(4)) {
		    if (resists_magm(mdef) || (rn2(100) < mdef->data->mr) ) { /* no easy taming Death or Famine! --Amy */
			/* magic resistance protects from polymorph traps,
			 * so make it guard against involuntary polymorph
			 * attacks too... */
			shieldeff(mdef->mx, mdef->my);
		    } else mon_poly(mdef, TRUE, "%s undergoes a freakish metamorphosis!");
		}
		break;
	    case AD_CHAO:
		if (!rn2(4)) {
		    if (resists_magm(mdef) || (rn2(100) < mdef->data->mr) ) { /* no easy taming Death or Famine! --Amy */
			/* magic resistance protects from polymorph traps,
			 * so make it guard against involuntary polymorph
			 * attacks too... */
			shieldeff(mdef->mx, mdef->my);
		    } else mon_poly(mdef, TRUE, "%s undergoes a freakish metamorphosis!");
		}
		if (mdef && mdef->mhpmax > 1) {
			mdef->mhpmax--;
			pline("%s feels bad!", Monnam(mdef));
		}

		break;
	    case AD_DETH:
		if (rn2(16)) {
		    /* Just damage */
		    goto common;
		}
		if (is_undead(mdef->data) || mdef->egotype_undead) {
		    if (!Blind) pline("%s looks no deader than before.", Monnam(mdef));
		    break;
		} else if (resists_magm(mdef)) {
		    if (!Blind)
			pline("%s shudders momentarily...", Monnam(mdef));
		} else {
		    tmp = mdef->mhp;
		}
		goto common;
	    case AD_FAMN:
		if (mdef->mtame) {
			makedoghungry(mdef, tmp * rnd(50));
			pline("%s suddenly looks hungry.", Monnam(mdef));
		}
		break;
	    case AD_WRAT:
	    case AD_MANA:
	    case AD_TECH:
	    case AD_MEMO:
	    case AD_TRAI:
	    	mon_drain_en(mdef, ((mdef->m_lev > 0) ? (rnd(mdef->m_lev)) : 0) + 1 + tmp);
    	      mon_drain_en(mdef, tmp);
		goto common;
	    case AD_DREN:
	    	if (resists_magm(mdef)) {
		    if (!Blind) {
			shieldeff(mdef->mx,mdef->my);
			pline("%s is unaffected.", Monnam(mdef));
		    }
	    	} else {
	    	    mon_drain_en(mdef, ((mdef->m_lev > 0) ? (rnd(mdef->m_lev)) : 0) + 1);
	    	    mon_drain_en(mdef, tmp);
	    	}
		break;
	    case AD_CALM:
		/* Certain monsters aren't even made peaceful. */
		if (!mdef->iswiz && mdef->data != &mons[PM_MEDUSA] &&
				!(mdef->data->mflags3 & M3_COVETOUS) &&
				!(mdef->data->geno & G_UNIQ)) {
		    pline("You calm %s.", mon_nam(mdef));
		    if (!mdef->mfrenzied) mdef->mpeaceful = 1;
		    mdef->mtame = 0;
		}
		break;

	    case AD_FREN:
		    if (!mdef->mfrenzied) pline("You frenzied %s!", mon_nam(mdef));
		    mdef->mfrenzied = 1;
		    mdef->mtame = mdef->mpeaceful = 0;

		break;

	    case AD_STON:
	    case AD_EDGE:
		if (!munstone(mdef, TRUE) && !rn2(4)) {
		    minstapetrify(mdef, TRUE);
		}
		break;
	    case AD_TLPT:
	    case AD_NEXU:
	    case AD_BANI:
	    case AD_ABDC:
		if (!rn2(4)) {
		    if (u_teleport_mon(mdef, FALSE))
			pline("Your opponent suddenly disappears!"); /* I'm lazy. Sue me. --Amy */
		}
		break;
	    case AD_CURS:
	    case AD_ICUR:
	    case AD_NACU:
		if (night() && !rn2(10) && !mdef->mcan && (rnd(100) > mdef->data->mr) ) {

		    if (mdef->data == &mons[PM_CLAY_GOLEM]) {
			if (!Blind)
			    pline("Some writing vanishes from %s head!",
				s_suffix(mon_nam(mdef)));
			xkilled(mdef, 0);
			/* Don't return yet; keep hp<1 and tmp=0 for pet msg */
		    } else {
			cancelmonsterlite(mdef);
			You("chuckle.");
		    }
		}
		break;
	    case AD_DRLI:
	    case AD_TIME:
	    case AD_DFOO:
	    case AD_WEEP:
	    case AD_VAMP:
		resistance = resists_drli(mdef);
		if (!resistance) {
			pline("%s suddenly seems weaker!", Monnam(mdef));
			mdef->mhpmax -= rnd(12);
			if (mdef->mhp <= 0 || !mdef->m_lev) {
				pline("%s dies!", Monnam(mdef));
				xkilled(mdef,0);
			} else
				mdef->m_lev--;

		}
		goto common;
	    case AD_RUST:
		if (pd == &mons[PM_IRON_GOLEM]) {
			pline("%s falls to pieces!", Monnam(mdef));
			xkilled(mdef,0);
		}
		hurtmarmor(mdef, AD_RUST);
		break;
	    case AD_CORR:
		hurtmarmor(mdef, AD_CORR);
		break;
	    case AD_DCAY:
		if (pd == &mons[PM_WOOD_GOLEM] ||
		    pd == &mons[PM_LEATHER_GOLEM]) {
			pline("%s falls to pieces!", Monnam(mdef));
			xkilled(mdef,0);
		}
		hurtmarmor(mdef, AD_DCAY);
		break;
	    case AD_FLAM:
	    case AD_WNCE:
		hurtmarmor(mdef, AD_FLAM);
		break;
	    case AD_DRST:
	    case AD_DRDX:
	    case AD_DRCO:
	    case AD_POIS:
	    case AD_WISD:
	    case AD_DRCH:
		resistance = (resists_poison(mdef) && !player_will_pierce_resistance()) ;
		if (resistance)
			pline_The("poison doesn't seem to affect %s.", mon_nam(mdef));
		else {
			if (!rn2(100) && !resists_poison(mdef)) {
			    Your("poison was deadly...");
			    tmp = mdef->mhp;
			}
		}
		goto common;
	    case AD_VENO:
		if (resists_poison(mdef) && !player_will_pierce_resistance())
			pline_The("poison doesn't seem to affect %s.", mon_nam(mdef));
		else {
			pline("%s is badly poisoned!", Monnam(mdef));
			if (!rn2(10) && !resists_poison(mdef)) {
			    Your("poison was deadly...");
			    tmp = mdef->mhp;
			} else tmp *= 2;
		}
		goto common;
	    case AD_LITE:
		if (is_vampire(mdef->data)) {
			tmp *= 2; /* vampires take more damage from sunlight --Amy */
			pline("%s is irradiated!", Monnam(mdef));
		}
		goto common;
	    case AD_DARK:
		do_clear_area(mdef->mx,mdef->my, 7, set_lit, (void *)((char *)0));
		pline("You generate a sinister darkness!");
		if (mdef->data->mlet == S_ANGEL) tmp *= 2;
		break;
	    case AD_THIR:
	    case AD_NTHR:
		healup(tmp + (u.ulevel / 2), 0, FALSE, FALSE);
		pline("You suck %s's %s!", mon_nam(mdef), mbodypart(mdef, BLOOD) );
		if (Race_if(PM_BURNINATOR)) {
			u.uen += (tmp + (u.ulevel / 2));
			if (u.uen > u.uenmax) u.uen = u.uenmax;
		}
		goto common;
	    case AD_CHKH:
		if (u.ulevel > mdef->m_lev) tmp += (u.ulevel - mdef->m_lev);
		goto common;
	    case AD_HODS:
		tmp += mdef->m_lev;
		goto common;
	    case AD_DIMN:
		tmp /= 20;
		tmp += u.ulevel;
		goto common;
	    case AD_COLD:
		resistance = (resists_cold(mdef) && !player_will_pierce_resistance());
		goto common;
	    case AD_ACID:
		resistance = (resists_acid(mdef) && !player_will_pierce_resistance()) ;
		goto common;
	    case AD_FIRE:
		resistance = (resists_fire(mdef) && !player_will_pierce_resistance());
		goto common;
	    case AD_MALK:
		if (!resists_elec(mdef)) {
			tmp *= 2;
			pline("%s is shocked!", Monnam(mdef));
		}
		goto common;
	    case AD_CHRN:
	    case AD_BURN:
	    case AD_FRZE:
	    case AD_ICEB:
		if (mdef->mhpmax > 1) {
			mdef->mhpmax--;
			pline("%s feels bad!", Monnam(mdef));
		}
		break;

	    case AD_RAGN:
		ragnarok(FALSE);
		if (evilfriday && u.ulevel > 1) evilragnarok(FALSE,u.ulevel);
		break;

	    case AD_AGGR:

		incr_itimeout(&HAggravate_monster, tmp);
		You_feel("that monsters are aware of your presence.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Dazhe sovetskaya Pyat' Lo obostryayetsya v vashem nizkom igrovom masterstve." : "Woaaaaaah!");
		aggravate();
		if (!rn2(20)) {

			int aggroamount = rnd(6);
			if (isfriday) aggroamount *= 2;
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
			while (aggroamount) {

				u.cnd_aggravateamount++;
				makemon((struct permonst *)0, u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
				aggroamount--;
				if (aggroamount < 0) aggroamount = 0;
			}
			u.aggravation = 0;
			pline("Several monsters come out of a portal.");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

		}
		tmp /= 20;
		goto common;

		break;

	    case AD_CONT:

		if (!rn2(30)) {
			mdef->isegotype = 1;
			mdef->egotype_contaminator = 1;
		}
		if (!rn2(100)) {
			mdef->isegotype = 1;
			mdef->egotype_weeper = 1;
		}
		if (!rn2(250)) {
			mdef->isegotype = 1;
			mdef->egotype_radiator = 1;
		}
		if (!rn2(250)) {
			mdef->isegotype = 1;
			mdef->egotype_reactor = 1;
		}

		break;

	    case AD_UVUU:
		if (has_head(mdef->data)) {
			tmp *= 2;
			if (!rn2(1000)) {
				tmp *= 100;
				pline("Your explosion rips off %s's %s completely!", mon_nam(mdef), mbodypart(mdef, HEAD));
			} else pline("You spike %s's %s!", mon_nam(mdef), mbodypart(mdef, HEAD));
		}
		goto common;
	    case AD_GRAV:
		if (!is_flyer(mdef->data)) {
			tmp *= 2;
			pline("Your explosion slams %s into the ground!", mon_nam(mdef));
		}
		goto common;
	    case AD_PLAS:
		if (!resists_fire(mdef)) {
			tmp *= 2;
			pline("%s is enveloped by searing plasma radiation!", Monnam(mdef));
		}
		goto common;
	    case AD_SLUD:
		if (!resists_acid(mdef)) {
			tmp *= 2;
			pline("%s is covered with sludge!", Monnam(mdef));
		}
		goto common;
	    case AD_LAVA:
		if (resists_cold(mdef) && !resists_fire(mdef)) {
			tmp *= 4;
			pline("%s is scorched!", Monnam(mdef));
		} else if (!resists_fire(mdef)) {
			tmp *= 2;
			pline("%s is severely burned!", Monnam(mdef));
		}
		goto common;
	    case AD_FAKE:
		pline("%s", fauxmessage());
		u.cnd_plineamount++;
		if (!rn2(3)) {
			pline("%s", fauxmessage());
			u.cnd_plineamount++;
		}
		goto common;
	    case AD_WEBS:
		(void) maketrap(mdef->mx, mdef->my, WEB, 0, FALSE);
		if (!rn2(issoviet ? 2 : 8)) makerandomtrap(FALSE);
		goto common;

	    case AD_TRAP:
		if (t_at(mdef->mx, mdef->my) == 0) (void) maketrap(mdef->mx, mdef->my, rndtrap(), 0, FALSE);
		else makerandomtrap(FALSE);
		goto common;
		break;

	    case AD_CNCL:
		if (rnd(100) > mdef->data->mr) {
			cancelmonsterlite(mdef);
			pline("%s is covered in sparkling lights!", Monnam(mdef));
		}
		goto common;
	    case AD_FEAR:
		if (rnd(100) > mdef->data->mr) {
		     monflee(mdef, rnd(1 + tmp), FALSE, TRUE);
			pline("%s screams in fear!",Monnam(mdef));
		}
		break;
	    case AD_SANI:
		if (!rn2(10)) {
			mdef->mconf = 1;
			switch (rnd(4)) {

				case 1:
					pline("%s sees you chow dead bodies.", Monnam(mdef)); break;
				case 2:
					pline("%s shudders at your terrifying %s.", Monnam(mdef), makeplural(body_part(EYE)) ); break;
				case 3:
					pline("%s feels sick at entrails caught in your tentacles.", Monnam(mdef)); break;
				case 4:
					pline("%s sees maggots breed in your rent %s.", Monnam(mdef), body_part(STOMACH)); break;

			}

		}

		break;

	    case AD_INSA:

		if (rnd(100) > mdef->data->mr) {
		     monflee(mdef, rnd(1 + tmp), FALSE, TRUE);
			pline("%s screams in fear!",Monnam(mdef));
		}
		if (!mdef->mconf && !rn2(3)) {
		    if (canseemon(mdef))
			pline("%s looks confused.", Monnam(mdef));
		    mdef->mconf = 1;
		}
		if(!Blind && !rn2(3))
		    pline("%s %s for a moment.", Monnam(mdef),
			  makeplural(stagger(mdef->data, "stagger")));
		mdef->mstun = 1;
		break;

	    case AD_DREA:
		if (!mdef->mcanmove) {
			tmp *= 4;
			pline("You eat %s's dream!", mon_nam(mdef));
			u.uconduct.food++;
			if (FemtrapActiveNora) {
				You("vomit.");
				vomit();
				morehungry(20);
			}
			morehungry(-rnd(10)); /* cannot choke */
		}
		goto common;

	    case AD_DRIN:
		if (!has_head(mdef->data)) {
		    pline("%s doesn't seem harmed.", Monnam(mdef));
		    tmp = 0;
		    break;
		}
		if (m_slips_free(mdef, mattk)) break;

		if ((mdef->misc_worn_check & W_ARMH) && rn2(3)) {
		    pline("%s helmet blocks your attack to %s head.",
			  s_suffix(Monnam(mdef)), mhis(mdef));
		    break;
		}
		You("eat %s brain!", s_suffix(mon_nam(mdef)));
		u.uconduct.food++;
		if (FemtrapActiveNora) {
			You("vomit.");
			vomit();
			morehungry(20);
		}
		if (touch_petrifies(mdef->data) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && !Stoned) {
			if (Hallucination && rn2(10)) pline("You are already stoned.");
			else {
				You("are turning to stone.");
				Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
				u.cnd_stoningcount++;
				killer_format = KILLED_BY_AN;
				delayed_killer = mdef->data->mname;
			}
		}
		if (!vegan(mdef->data))
		    u.uconduct.unvegan++;
		if (!vegetarian(mdef->data))
		    violated_vegetarian();
		if (mindless(mdef->data) || mdef->egotype_undead) {
		    pline("%s doesn't notice.", Monnam(mdef));
		    break;
		}
		tmp += rnd(10);
		morehungry(-rnd(30)); /* cannot choke */
		if (ABASE(A_INT) < AMAX(A_INT)) {
			ABASE(A_INT) += rnd(4);
			if (ABASE(A_INT) > AMAX(A_INT))
				ABASE(A_INT) = AMAX(A_INT);
			flags.botl = 1;
		}
		exercise(A_WIS, TRUE);
		goto common;

	    case AD_SPC2:
		if (!mdef->mconf) {
		    if (canseemon(mdef))
			pline("%s looks confused.", Monnam(mdef));
		    mdef->mconf = 1;
		}
		else
		{
		    if (canseemon(mdef))
			pline("%s is getting more and more confused.",
				Monnam(mdef));
		    mdef->mconf++;
		}
		break;

	    case AD_BLEE:
		if (tmp > 0) {
			mdef->bleedout += tmp;
			pline("%s is hit by debris and loses a lot of %s!", Monnam(mdef), mbodypart(mdef, BLOOD));
		}
		goto common;

	    case AD_TERR:
		terrainterror(0);
		break;

	    case AD_PLYS:
	    case AD_TCKL:
		if (mdef->mcanmove && !(dmgtype(mdef->data, AD_PLYS))) {
		    if (!Blind) pline("%s is frozen by you!", Monnam(mdef));
		    mdef->mcanmove = 0;
		    mdef->mfrozen = rnd(10);
		}
		break;

	    case AD_SLEE:
		if (!mdef->msleeping && sleep_monst(mdef, rnd(10), -1)) {
		    if (!Blind)
			pline("%s is put to sleep by you!", Monnam(mdef));
		    slept_monst(mdef);
		}
		break;
	    case AD_SLIM: /* no easy sliming Death or Famine --Amy */
	    case AD_LITT:
		if ((rn2(100) < mdef->data->mr) ) goto common;	/* physical damage only */
		if (!rn2(400) && !flaming(mdef->data) && !slime_on_touch(mdef->data) ) {
		    You("turn %s into slime.", mon_nam(mdef));
		    (void) newcham(mdef, &mons[PM_GREEN_SLIME], FALSE, !Blind);
		    tmp = 0;
		    break;
		}
		goto common;
	    case AD_SLOW:
	    case AD_WGHT:
		if (mdef->mspeed != MSLOW) {
		    unsigned int oldspeed = mdef->mspeed;

		    mon_adjust_speed(mdef, -1, (struct obj *)0);
		    if (mdef->mspeed != oldspeed && canseemon(mdef))
			pline("%s slows down.", Monnam(mdef));
		}
		break;
	    case AD_INER:
		if (mdef->mspeed != MSLOW) {
		    unsigned int oldspeed = mdef->mspeed;

		    mon_adjust_speed(mdef, -1, (struct obj *)0);
		    if (mdef->mspeed != oldspeed && canseemon(mdef))
			pline("%s slows down.", Monnam(mdef));
		}
		goto common;

	    case AD_LAZY:
		if (mdef->mspeed != MSLOW) {
		    unsigned int oldspeed = mdef->mspeed;

		    mon_adjust_speed(mdef, -1, (struct obj *)0);
		    if (mdef->mspeed != oldspeed && canseemon(mdef))
			pline("%s slows down.", Monnam(mdef));
		}
		if (mdef->mcanmove && !(dmgtype(mdef->data, AD_PLYS))) {
		    if (!Blind) pline("%s is frozen by you!", Monnam(mdef));
		    mdef->mcanmove = 0;
		    mdef->mfrozen = rnd(10);
		}
		goto common;
	    case AD_NUMB:
		if (!rn2(3) && mdef->mspeed != MSLOW) {
		    unsigned int oldspeed = mdef->mspeed;

		    mon_adjust_speed(mdef, -1, (struct obj *)0);
		    if (mdef->mspeed != oldspeed && canseemon(mdef))
			pline("%s is numbed.", Monnam(mdef));
		}
		break;

	    case AD_ELEC:
		resistance = (resists_elec(mdef) && !player_will_pierce_resistance());
common:
		if (!resistance) {
		    pline("%s gets blasted!", Monnam(mdef));
		    mdef->mhp -= tmp;
		    if (mdef->mhp <= 0) {
			 killed(mdef);
			 return(2);
		    }
		} else {
		    shieldeff(mdef->mx, mdef->my);
		    if (is_golem(mdef->data))
			golemeffects(mdef, (int)mattk->adtyp, tmp);
		    else
			pline_The("blast doesn't seem to affect %s.",
				mon_nam(mdef));
		}
		break;
	    default:
		tmp /= 20; /* fail safe */
		    pline("%s gets blasted!", Monnam(mdef));
		    mdef->mhp -= tmp;
		    if (mdef->mhp <= 0) {
			 killed(mdef);
			 return(2);
		    }
		break;
	}

	if (uactivesymbiosis && attacktype(&mons[u.usymbiote.mnum], AT_EXPL)) {

		if (uarmf && itemhasappearance(uarmf, APP_REMORA_HEELS) && u.usymbiote.mnum == PM_REMORA) {
			if (uarmf->spe > -1) uarmf->spe = -1;
		}

		if (uamul && uamul->otyp == AMULET_OF_SYMBIOTE_SAVING) {
			makeknown(AMULET_OF_SYMBIOTE_SAVING);
			useup(uamul);
			u.usymbiote.mhp = u.usymbiote.mhpmax;
			Your("symbiote glows, and your amulet crumbles to dust!");
		} else {
			u.usymbiote.active = 0;
			u.usymbiote.mnum = PM_PLAYERMON;
			u.usymbiote.mhp = 0;
			u.usymbiote.mhpmax = 0;
			u.usymbiote.cursed = u.usymbiote.hvycurse = u.usymbiote.prmcurse = u.usymbiote.bbcurse = u.usymbiote.morgcurse = u.usymbiote.evilcurse = u.usymbiote.stckcurse = 0;
			if (flags.showsymbiotehp) flags.botl = TRUE;
			u.cnd_symbiotesdied++;
			Your("symbiote was blown to smithereens.");
		}
	}

	return(1);
}

STATIC_OVL void
start_engulf(mdef)
struct monst *mdef;
{
	if (!Invisible) {
		map_location(u.ux, u.uy, TRUE);
		tmp_at(DISP_ALWAYS, mon_to_glyph(&youmonst));
		tmp_at(mdef->mx, mdef->my);
	}
	You("engulf %s!", mon_nam(mdef));
	delay_output();
	delay_output();
}

STATIC_OVL void
end_engulf()
{
	if (!Invisible) {
		tmp_at(DISP_END, 0);
		newsym(u.ux, u.uy);
	}
}

STATIC_OVL int
gulpum(mdef,mattk)
register struct monst *mdef;
register struct attack *mattk;
{
	register int tmp;
	register int dam = d((int)mattk->damn, (int)mattk->damd);
	struct obj *otmp;
	/* Not totally the same as for real monsters.  Specifically, these
	 * don't take multiple moves.  (It's just too hard, for too little
	 * result, to program monsters which attack from inside you, which
	 * would be necessary if done accurately.)  Instead, we arbitrarily
	 * kill the monster immediately for AD_DGST and we regurgitate them
	 * after exactly 1 round of attack otherwise.  -KAA
	 */

	if(mdef->data->msize >= MZ_HUGE) return 0;

	if(u.uhunger < 4000 && !u.uswallow) {
	    for (otmp = mdef->minvent; otmp; otmp = otmp->nobj)
		(void) snuff_lit(otmp);

	    if(!touch_petrifies(mdef->data) || Stone_resistance) {
#ifdef LINT	/* static char msgbuf[BUFSZ]; */
		char msgbuf[BUFSZ];
#else
		static char msgbuf[BUFSZ];
#endif
		start_engulf(mdef);
		switch(mattk->adtyp) {
		    case AD_DGST:
		if (!rn2(50)) { /*muuuuuch lower chance because this attack is totally unbalanced anyway --Amy*/
			/* eating a Rider or its corpse is fatal */
			if (is_rider(mdef->data) || is_deadlysin(mdef->data)) {
				u.youaredead = 1;
			 pline("Unfortunately, digesting any of it is fatal.");
			    end_engulf();
			    sprintf(msgbuf, "unwisely tried to eat a monster (%s)",
				    mdef->data->mname);
			    killer = msgbuf;
			    killer_format = NO_KILLER_PREFIX;
			    done(DIED);
				u.youaredead = 0;
			    return 0;		/* lifesaved */
			}

			if (Slow_digestion) {
			    dam = 0;
			    break;
			}

			/* KMH, conduct */
			u.uconduct.food++;
			if (FemtrapActiveNora) {
				You("vomit.");
				vomit();
				morehungry(20);
			}
			if (!vegan(mdef->data))
			     u.uconduct.unvegan++;
			if (!vegetarian(mdef->data))
			     violated_vegetarian();

			/* Use up amulet of life saving */
			if (!!(otmp = mlifesaver(mdef))) m_useup(mdef, otmp);

			newuhs(FALSE);
			xkilled(mdef,2);
			if (mdef->mhp > 0) { /* monster lifesaved */
			    You("hurriedly regurgitate the sizzling in your %s.",
				body_part(STOMACH));
			} else {
			    tmp = 1 + (mdef->data->cwt >> 8);
			    if (corpse_chance(mdef, &youmonst, TRUE) &&
				!(mvitals[monsndx(mdef->data)].mvflags &
				  G_NOCORPSE)) {
				/* nutrition only if there can be a corpse */
				u.uhunger += (mdef->data->cnutrit+1) / 2;
			    } else tmp = 0;
			    sprintf(msgbuf, "You totally digest %s.",
					    mon_nam(mdef));
			    if (tmp != 0) {
				/* setting afternmv = end_engulf is tempting,
				 * but will cause problems if the player is
				 * attacked (which uses his real location) or
				 * if his See_invisible wears off
				 */
				int paralysistime = tmp;
				if (!isstunfish && (paralysistime > 1)) paralysistime = rnd(paralysistime);
				if (paralysistime > 10) {
					while (rn2(5) && (paralysistime > 10)) {
						paralysistime--;
					}
				}

				You("digest %s.", mon_nam(mdef));
				if (Slow_digestion) paralysistime *= 2;
				nomul(-paralysistime, "digesting a monster", TRUE);
				nomovemsg = msgbuf;
			    } else pline("%s", msgbuf);
			    if (slime_on_touch(mdef->data)) {
				sprintf(msgbuf, "%s isn't sitting well with you.",
					The(mdef->data->mname));
				if (!Unchanging) {
					make_slimed(50);
					flags.botl = 1;
				    killer_format = KILLED_BY_AN;
				    delayed_killer = "slimed by digesting a slimer";
				}
			    } else
			    exercise(A_CON, TRUE);
			}
			end_engulf();
			return(2);
		}
			break;
		    case AD_PHYS:
			if (youmonst.data == &mons[PM_FOG_CLOUD]) {
			    pline("%s is laden with your moisture.",
				  Monnam(mdef));
			    if (amphibious(mdef->data) &&
				!flaming(mdef->data)) {
				dam = 0;
				pline("%s seems unharmed.", Monnam(mdef));
			    }
			} else
			    pline("%s is pummeled with your debris!",
				  Monnam(mdef));
			break;
		    case AD_ACID:
			pline("%s is covered with your goo!", Monnam(mdef));
			if (resists_acid(mdef) && !player_will_pierce_resistance()) {
			    pline("It seems harmless to %s.", mon_nam(mdef));
			    dam = 0;
			}
			break;
		    case AD_BLND:
			if (can_blnd(&youmonst, mdef, mattk->aatyp, (struct obj *)0)) {
			    if (mdef->mcansee)
				pline("%s can't see in there!", Monnam(mdef));
			    mdef->mcansee = 0;
			    dam += mdef->mblinded;
			    if (dam > 127) dam = 127;
			    mdef->mblinded = dam;
			}
			dam = 0;
			break;
		    case AD_ELEC:
			if (rn2(2)) {
			    pline_The("air around %s crackles with electricity.", mon_nam(mdef));
			    if (resists_elec(mdef) && !player_will_pierce_resistance()) {
				pline("%s seems unhurt.", Monnam(mdef));
				dam = 0;
			    }
			    golemeffects(mdef,(int)mattk->adtyp,dam);
			} else dam = 0;
			break;
		    case AD_COLD:
			if (rn2(2)) {
			    if (resists_cold(mdef) && !player_will_pierce_resistance()) {
				pline("%s seems mildly chilly.", Monnam(mdef));
				dam = 0;
			    } else
				pline("%s is freezing to death!",Monnam(mdef));
			    golemeffects(mdef,(int)mattk->adtyp,dam);
			} else dam = 0;
			break;
		    case AD_STON: /* for cortex race */
		    case AD_EDGE:
			if (!munstone(mdef, TRUE) && !rn2(4)) {
			    minstapetrify(mdef, TRUE);
			    return(2);
			}
			dam = 0;
			break;
		    case AD_DARK: /* for etherealoid race */
			do_clear_area(mdef->mx,mdef->my, 7, set_lit, (void *)((char *)0));
			pline("You generate a sinister darkness!");
			if (mdef->data->mlet == S_ANGEL) dam *= 2;
			break;
		    case AD_FIRE:
			if (rn2(2)) {
			    if (resists_fire(mdef) && !player_will_pierce_resistance()) {
				pline("%s seems mildly hot.", Monnam(mdef));
				dam = 0;
			    } else
				pline("%s is burning to a crisp!",Monnam(mdef));
			    golemeffects(mdef,(int)mattk->adtyp,dam);
			} else dam = 0;
			break;
		}
		end_engulf();
		if ((mdef->mhp -= dam) <= 0) {
		    killed(mdef);
		    if (mdef->mhp <= 0)	/* not lifesaved */
			return(2);
		}
		You("%s %s!", is_animal(youmonst.data) ? "regurgitate"
			: "expel", mon_nam(mdef));
		if (Slow_digestion || is_animal(youmonst.data)) {
		    pline("Obviously, you didn't like %s taste.",
			  s_suffix(mon_nam(mdef)));
		}
	    } else {
		static char kbuf[BUFSZ];

		You("bite into %s.", mon_nam(mdef));
		sprintf(kbuf, "swallowing a petrifying monster whole");
		instapetrify(kbuf);
	    }
	}
	return(0);
}

void
missum(mdef, target, roll, mattk)
register struct monst *mdef;
register struct attack *mattk;
register int target;
register int roll;
{
	register boolean nearmiss = (target == roll);
	register boolean totalmiss = ((roll - target) > 20); /* tell the player if their to-hit is really sucky --Amy */
	register struct obj *blocker = (struct obj *)0;
	long mwflags = mdef->misc_worn_check;
	char buf[BUFSZ];

		/* 3 values for blocker
		 *	No blocker:  (struct obj *) 0  
		 * 	Piece of armour:  object
		 */

	/* This is a hack,  since there is no fast equivalent for uarm, uarms, etc.  
	 * Technically, we really should check from the inside out...
	 */
	if (target < roll) {
	    for (blocker = mdef->minvent; blocker; blocker = blocker->nobj) {
		if (blocker->owornmask & mwflags) {
			target += ARM_BONUS(blocker);
			if (target > roll) break;
		}
	    }
	}
	
	if (could_seduce(&youmonst, mdef, mattk) && rn2(5) ) {
		You("pretend to be friendly to %s.", mon_nam(mdef));
	} else if ((Role_if(PM_SPACEWARS_FIGHTER) || (mdef->data->msound == MS_COMBAT) || (youmonst.data->msound == MS_COMBAT) || Role_if(PM_CAMPERSTRIKER) || Role_if(PM_HUSSY) || Role_if(PM_GANG_SCHOLAR) || FemtrapActiveAriane || Role_if(PM_WALSCHOLAR) || ishaxor || Hallucination || (u.usanity > rn2(1000)) ) && !rn2(20) && canspotmon(mdef) && flags.verbose && (PlayerHearsSoundEffects) ) {

		switch (rnd(6)) {
	
		case 1: pline("Waff!"); break;
		case 2: pline("Haehaehaehaehaehaehaeaeae-aeaeae-aeaeae-aeaeae-aeaeae."); break;
		case 3: pline("Laaaaaaaaansch!"); break;
		case 4: pline("Woeoeoeing!"); break;
		case 5: pline("Waeddaewied-di waend dae dae."); break;
		case 6: pline("Bimmselimmelimm!"); break;
		}

	} else if ((Role_if(PM_SPACEWARS_FIGHTER) || (mdef->data->msound == MS_COMBAT) || (youmonst.data->msound == MS_COMBAT) || Role_if(PM_CAMPERSTRIKER) || Role_if(PM_HUSSY) || Role_if(PM_GANG_SCHOLAR) || FemtrapActiveAriane || Role_if(PM_WALSCHOLAR) || ishaxor || Hallucination || (u.usanity > rn2(1000)) ) && (!rn2(5) || (mdef->data->msound == MS_COMBAT) || (youmonst.data->msound == MS_COMBAT) ) && canspotmon(mdef) && flags.verbose) {

		switch (rnd(839)) {

		case 1: pline("%s cringes from your strike behind its %sshield.", Monnam(mdef), which_armor(mdef, W_ARMS) ? "" : "nonexistant "); break;
		case 2: pline("You smash into %s's %sshield, striking sparks.", mon_nam(mdef), which_armor(mdef, W_ARMS) ? "" : "nonexistant "); break;
		case 3: pline("Your thrust glances from %s's scales.", mon_nam(mdef)); break;
		case 4: pline("%s cowers under its %sshield, barely avoiding your blows.", Monnam(mdef), which_armor(mdef, W_ARMS) ? "" : "nonexistant "); break;
		case 5: pline("%s catches your blow on its %sshield.", Monnam(mdef), which_armor(mdef, W_ARMS) ? "" : "nonexistant "); break;
		case 6: pline("%s's scaly hide deflects your blow.", Monnam(mdef)); break;
		case 7: pline("You stumble and miss %s, cleaving the air.", mon_nam(mdef)); break;
		case 8: pline("%s blocks your swing!", Monnam(mdef)); break;
		case 9: pline("%s laughs fiendishly at your futile attack.", Monnam(mdef)); break;
		case 10: pline("You lunge at %s, but fumble the thrust.", mon_nam(mdef)); break;
		case 11: pline("Your blow bounces off %s's %sarmor!", mon_nam(mdef), which_armor(mdef, W_ARM) ? "" : "nonexistant "); break;
		case 12: pline("%s blocks your clumsy blow!", Monnam(mdef)); break;
		case 13: pline("%s ignores your puny attack.", Monnam(mdef)); break;
		case 14: pline("%s ignores your feeble flailings.", Monnam(mdef)); break;
		case 15: pline("You slash at air as %s dances back.", mon_nam(mdef)); break;
		case 16: pline("You miss %s by a league!", mon_nam(mdef)); break;
		case 17: pline("%s dodges your attack!", Monnam(mdef)); break;
		case 18: pline("%s narrowly escapes your slashing attack.", Monnam(mdef)); break;
		case 19: pline("You strike sparks from %s's %sarmor.", mon_nam(mdef), which_armor(mdef, W_ARM) ? "" : "nonexistant "); break;
		case 20: pline("Your swing goes wide!"); break;
		case 21: pline("%s dodges your swing!", Monnam(mdef)); break;
		case 22: pline("%s barely deflects your strike.", Monnam(mdef)); break;
		case 23: pline("You smash into %s's %sshield, sending splinters flying.", mon_nam(mdef), which_armor(mdef, W_ARMS) ? "" : "nonexistant "); break;
		case 24: pline("%s dodges your clumsy blow!", Monnam(mdef)); break;
		case 25: pline("%s avoids your blow.", Monnam(mdef)); break;
		case 26: pline("Your weapon glances off of %s, striking sparks.", mon_nam(mdef)); break;
		case 27: pline("%s evades your attack!", Monnam(mdef)); break;
		case 28: pline("%s parries your strike.", Monnam(mdef)); break;
		case 29: pline("%s evades your swing!", Monnam(mdef)); break;
		case 30: pline("%s easily parries your strike.", Monnam(mdef)); break;
		case 31: pline("%s evades your clumsy blow!", Monnam(mdef)); break;
		case 32: pline("You pierce through %s, which ignores the wound.", mon_nam(mdef)); break;
		case 33: pline("Your blow bounces off %s's %sshield!", mon_nam(mdef), which_armor(mdef, W_ARMS) ? "" : "nonexistant "); break;
		case 34: pline("%s blocks your attack!", Monnam(mdef)); break;
		case 35: pline("Your blow bounces off %s's hide!", mon_nam(mdef)); break;
		case 36: pline("You fully hit %s's %sshield, hahahahahahaha.", mon_nam(mdef), which_armor(mdef, W_ARMS) ? "" : "nonexistant "); break;
		case 37: pline("Your strike glances off %s's scales.", mon_nam(mdef)); break;
		case 38: pline("You didn't hit cause you were drinking liquor! Ha ha ha ha ha ha ha!"); break;
		case 39: pline("%s lost only zero lines!", Monnam(mdef)); break;
		case 40: pline("%s's scaly hide deflects.", Monnam(mdef)); break;
		case 41: pline("You stumbled and missed %s.", mon_nam(mdef)); break;
		case 42: pline("%s laughs about your weak attack.", Monnam(mdef)); break;
		case 43: pline("Haha, you missed! Ha ha!"); break;
		case 44: pline("Haha, you didn't hit!"); break;
		case 45: pline("You did not hit, ha-ha-ha"); break;
		case 46: pline("Your weapon glances off %s, hahaha haha.", mon_nam(mdef)); break;
		case 47: pline("Haha, you missed %s haha", mon_nam(mdef)); break;
		case 48: pline("Your strike glances off %s's Eastpak.", mon_nam(mdef)); break;
		case 49: pline("You glanced off %s's scales.", mon_nam(mdef)); break;
		case 50: pline("Lunged! %s lost no lines", Monnam(mdef)); break;
		case 51: pline("Hit the %sarmor! That doesn't harm %s.", which_armor(mdef, W_ARM) ? "" : "nonexistant ", mon_nam(mdef)); break;
		case 52: pline("%s finishes you off fully.", Monnam(mdef)); break;
		case 53: pline("%s ignores your strike!", Monnam(mdef)); break;
		case 54: pline("Missed by a league!"); break;
		case 55: pline("You missed %s by a league.", Monnam(mdef)); break;
		case 56: pline("%shit - that doesn't harm %s.", which_armor(mdef, W_ARM) ? "Armor " : "Nonexistant armor ", mon_nam(mdef)); break;
		case 57: pline("Splintering hit! But %s is totally unphased.", mon_nam(mdef)); break;
		case 58: pline("You try to fully hit %s with your fist but to no avail.", mon_nam(mdef)); break;
		case 59: pline("Pierced! That doesn't phase %s in the least.", mon_nam(mdef)); break;
		case 60: pline("Hahahahahahahaha! Not hit, ha!"); break;
		case 61: pline("Haha, you didn't hit, you dumb %s! Hehehe!", flags.female ? "woman" : "man"); break;
		case 62: pline("%s blocks! Hahahaha!", Monnam(mdef)); break;
		case 63: pline("It's never gonna work this way, you!"); break;
		case 64: pline("You fully crash into the wall. Hahaha!"); break;
		case 65: pline("Ha, you only hit %s's armament!", Monnam(mdef)); break;
		case 66: pline("%s is not hit!", Monnam(mdef)); break;
		case 67: pline("%s is missed by you, ha.", Monnam(mdef)); break;
		case 68: pline("%s ignores your attack, haha.", Monnam(mdef)); break;
		case 69: pline("Missed by a league, ha"); break;
		case 70: pline("%s is not hit of course.", Monnam(mdef)); break;
		case 71: pline("%s ducked away!", Monnam(mdef)); break;
		case 72: pline("%s jumps away!", Monnam(mdef)); break;
		case 73: pline("%s uses hidden passages!", Monnam(mdef)); break;
		case 74: pline("%s jumps over you!", Monnam(mdef)); break;
		case 75: pline("%s runs away.", Monnam(mdef)); break;
		case 76: pline("Completely off, you type!"); break;
		case 77: pline("Fully hit the %sshield.", which_armor(mdef, W_ARMS) ? "" : "nonexistant "); break;
		case 78: pline("%s defends, hahahaha!", Monnam(mdef)); break;
		case 79: pline("Only hit the hide, hahaha"); break;
		case 80: pline("%s struck you over!", Monnam(mdef)); break;
		case 81: pline("%s is not hit, ha-ha-ha", Monnam(mdef)); break;
		case 82: pline("Haha, you did not hit %s!", mon_nam(mdef)); break;
		case 83: pline("You miss %s cause you hit yourself", mon_nam(mdef)); break;
		case 84: pline("%s blocks your strike", Monnam(mdef)); break;
		case 85: pline("%s flogs you and you ran away from it.", Monnam(mdef)); break;
		case 86: pline("You didn't hit cause you were drinking liquor! Hahaha!"); break;
		case 87: pline("%s is luckily missed, bwarharhar!", Monnam(mdef)); break;
		case 88: pline("%s doesn't allow you to hit!", Monnam(mdef)); break;
		case 89: pline("%s slaps you silly.", Monnam(mdef)); break;
		case 90: pline("%s's got a whole lot of talent!", Monnam(mdef)); break;
		case 91: pline("You miss by a league!"); break;
		case 92: pline("%s flogs you.", Monnam(mdef)); break;
		case 93: pline("%s disables your ability to walk. You lose.", Monnam(mdef)); break;
		case 94: pline("You missed %s.", mon_nam(mdef)); break;
		case 95: pline("You really hit absolutely nothing at all, you failure..."); break;
		case 96: pline("%s kicked you over.", Monnam(mdef)); break;
		case 97: pline("%s goes out of your way", Monnam(mdef)); break;
		case 98: pline("Glanced off! But that didn't hurt %s!", mon_nam(mdef)); break;
		case 99: pline("%s loses 0 lines!", Monnam(mdef)); break;
		case 100: pline("You shoot the wall!"); break;
		case 101: pline("%s runs away! Bwarharhar!", Monnam(mdef)); break;
		case 102: pline("%s is missed by you.", Monnam(mdef)); break;
		case 103: pline("%s defends easily, you type!", Monnam(mdef)); break;
		case 104: pline("Slat hit!"); break;
		case 105: pline("%s is fully missed.", Monnam(mdef)); break;
		case 106: pline("Only hit the hide, ha-ha!"); break;
		case 107: pline("You missed %s cause you were acting stupid!", mon_nam(mdef)); break;
		case 108: pline("%s evades, bwarharharhar!", Monnam(mdef)); break;
		case 109: pline("%s loses nothing at all!", Monnam(mdef)); break;
		case 110: pline("You seem to never hit at all!"); break;
		case 111: pline("Shielded! %s loses nothing.", Monnam(mdef)); break;
		case 112: pline("And beside it again"); break;
		case 113: pline("And of course the goalkeeper has that one!"); break;
		case 114: pline("And the ball goes inches wide!"); break;
		case 115: pline("You shoot the ball out of the playing field, so %s gets a corner!", mon_nam(mdef)); break;
		case 116: pline("%s simply takes the ball away from you!", Monnam(mdef)); break;
		case 117: pline("%s easily parries your shot!", Monnam(mdef)); break;
		case 118: pline("Haha, you stumble and miss the goal!"); break;
		case 119: pline("%s cleared off.", Monnam(mdef)); break;
		case 120: pline("%s holds your ball!", Monnam(mdef)); break;
		case 121: pline("%s clears off!", Monnam(mdef)); break;
		case 122: pline("%s clears off against you.", Monnam(mdef)); break;
		case 123: pline("%s takes the ball away!", Monnam(mdef)); break;
		case 124: pline("%s has taken the ball away from you.", Monnam(mdef)); break;
		case 125: pline("%s places itself in front of ye!", Monnam(mdef)); break;
		case 126: pline("Hit the post!"); break;
		case 127: pline("%s is getting bored. Missed!", Monnam(mdef)); break;
		case 128: pline("You are rammed away! Har har!"); break;
		case 129: pline("You missed the goal by a league"); break;
		case 130: pline("You glance off %s's Eastpak.", mon_nam(mdef)); break;
		case 131: pline("Glanced off the scales! That doesn't hurt %s", mon_nam(mdef)); break;
		case 132: pline("%s hidden itself behind its sign and is thus not met.", Monnam(mdef)); break;
		case 133: pline("%s easily catches your strike!", Monnam(mdef)); break;
		case 134: pline("Cringed! %s still has full lines.", Monnam(mdef)); break;
		case 135: pline("Heheheheheheeeeee! %s lost 0 lines!", Monnam(mdef)); break;
		case 136: pline("%s narrowly lost no lines.", Monnam(mdef)); break;
		case 137: pline("%s parried!", Monnam(mdef)); break;
		case 138: pline("%s uses a weapon blocker.", Monnam(mdef)); break;
		case 139: pline("%s doesn't lose a line!", Monnam(mdef)); break;
		case 140: pline("Boom! Fully hit the %sshield.", which_armor(mdef, W_ARMS) ? "" : "nonexistant "); break;
		case 141: pline("%s defends really well against your attack!", Monnam(mdef)); break;
		case 142: pline("%s calls for its buddy. You're scared and run away.", Monnam(mdef)); break;
		case 143: pline("%s defends and starts a counterattack.", Monnam(mdef)); break;
		case 144: pline("%s defends against you with superthings!", Monnam(mdef)); break;
		case 145: pline("%s laughs at you because you hit the wall!", Monnam(mdef)); break;
		case 146: pline("The referee says: 'You don't hit %s!'", mon_nam(mdef)); break;
		case 147: pline("%s is missed fully. Haa haa!", Monnam(mdef)); break;
		case 148: pline("%s teleports behind you.", Monnam(mdef)); break;
		case 149: pline("%s uses a hiding place!", Monnam(mdef)); break;
		case 150: pline("You missed %s by a world-championship.", mon_nam(mdef)); break;
		case 151: pline("%s doesn't want to be hurt by you, so it runs off.", Monnam(mdef)); break;
		case 152: pline("%s uses a secret door!", Monnam(mdef)); break;
		case 153: pline("%s simply runs off.", Monnam(mdef)); break;
		case 154: pline("%s says: 'You failure!'", Monnam(mdef)); break;
		case 155: pline("You hit %s's hide!", mon_nam(mdef)); break;
		case 156: pline("%s hides from you.", Monnam(mdef)); break;
		case 157: pline("%s is laughing at you wimp.", Monnam(mdef)); break;
		case 158: pline("%s helped itself!", Monnam(mdef)); break;
		case 159: pline("Bwar har har har har har har!"); break;
		case 160: pline("Ha, hahahahahahahaiiiiiiiiiii!"); break;
		case 161: pline("%s says: 'Har har!'", Monnam(mdef)); break;
		case 162: pline("%s laughs at you because you totally miss!", Monnam(mdef)); break;
		case 163: pline("%s uses its supercar - VROOM!", Monnam(mdef)); break;
		case 164: pline("%s jumps aside.", Monnam(mdef)); break;
		case 165: pline("You obviously don't hit at all!"); break;
		case 166: pline("%s ducks and you don't know how to do a ground attack!", Monnam(mdef)); break;
		case 167: pline("Fully smashed into the %sshield.", which_armor(mdef, W_ARMS) ? "" : "nonexistant "); break;
		case 168: pline("%s says: 'Nothing gives it!'", Monnam(mdef)); break;
		case 169: pline("%s says: 'I'm not buying whatever you are selling.'", Monnam(mdef)); break;
		case 170: pline("You never hit at all, you wimp-type!"); break;
		case 171: pline("%s is quickly away-coagulated!", Monnam(mdef)); break;
		case 172: pline("%s got away again!", Monnam(mdef)); break;
		case 173: pline("%s says: 'Boo!'", Monnam(mdef)); break;
		case 174: pline("%s has a mega-strong shield", Monnam(mdef)); break;
		case 175: pline("%s has fully the good %sshield and deflects you.", Monnam(mdef), which_armor(mdef, W_ARMS) ? "" : "nonexistant "); break;
		case 176: pline("You open fire but don't hit %s.", mon_nam(mdef)); break;
		case 177: pline("%s is not hit somehow!", Monnam(mdef)); break;
		case 178: pline("%s tricks you!", Monnam(mdef)); break;
		case 179: pline("%s tricked you.", Monnam(mdef)); break;
		case 180: pline("Missed! Haa haa!"); break;
		case 181: pline("You only hit the air."); break;
		case 182: pline("Whoosh! %s's lightsaber deflects you.", Monnam(mdef)); break;
		case 183: pline("You only hit the scales of %s.", mon_nam(mdef)); break;
		case 184: pline("And you call yourself a Jedi?"); break;
		case 185: pline("Now finally start trying at least!"); break;
		case 186: pline("Blocked!"); break;
		case 187: pline("You whiffed at %s!", mon_nam(mdef)); break;
		case 188: pline("Your weapon whiffs at %s.", mon_nam(mdef)); break;
		case 189: pline("Stop whiffing all the time, %s!", flags.female ? "woman" : "man"); break;
		case 190: pline("you never hit at all, it seems..."); break;
		case 191: pline("Can you even hit at all? I don't think so!"); break;
		case 192: pline("%s's %sshield is like an impenetrable wall.", Monnam(mdef), which_armor(mdef, W_ARMS) ? "" : "nonexistant "); break;
		case 193: pline("%s's AC is too high for you!", Monnam(mdef)); break;
		case 194: pline("Bragger! You're incapable of getting anything right!"); break;
		case 195: pline("Was probably nothing."); break;
		case 196: pline("%s runs away from YOU.", Monnam(mdef)); break;
		case 197: pline("%s falls asleep while you miss", Monnam(mdef)); break;
		case 198: pline("%s didn't let you hit it!", Monnam(mdef)); break;
		case 199: pline("%s is the best!", Monnam(mdef)); break;
		case 200: pline("%s is way too good for you.", Monnam(mdef)); break;
		case 201: pline("%s does a sommersault!", Monnam(mdef)); break;
		case 202: pline("Yeah, you missed! Yeah!"); break;
		case 203: pline("You only hit the air instead of %s!", mon_nam(mdef)); break;
		case 204: pline("You strike the wall! Ouch!"); break;
		case 205: pline("You hammer into %s's %sarmor!", mon_nam(mdef), which_armor(mdef, W_ARM) ? "" : "nonexistant "); break;
		case 206: pline("%s managed to evade!", Monnam(mdef)); break;
		case 207: pline("You perform a suicide attack against %s's %sshield.", mon_nam(mdef), which_armor(mdef, W_ARMS) ? "" : "nonexistant "); break;
		case 208: pline("%s is super, am I right?", Monnam(mdef)); break;
		case 209: pline("You stung %s, but that loses 0.", mon_nam(mdef)); break;
		case 210: pline("%s goes to the school abort!", Monnam(mdef)); break;
		case 211: pline("The Super-%s is missed of course. Hahaha!", l_monnam(mdef)); break;
		case 212: pline("%s hides behind a column!", Monnam(mdef)); break;
		case 213: pline("Your ammo glances off %s's scales.", mon_nam(mdef)); break;
		case 214: pline("%s goes to your ammo out of the way!", Monnam(mdef)); break;
		case 215: pline("%s deflects with a %sshield!", mon_nam(mdef), which_armor(mdef, W_ARMS) ? "" : "nonexistant "); break;
		case 216: pline("%s evades your shot!", Monnam(mdef)); break;
		case 217: pline("%s converted your ammo into lines!", Monnam(mdef)); break;
		case 218: pline("%s deflected with its energy converter!", Monnam(mdef)); break;
		case 219: pline("Your shot goes completely off!"); break;
		case 220: pline("You didn't aim right and missed %s!", mon_nam(mdef)); break;
		case 221: pline("%s uses a kevlar vest!", Monnam(mdef)); break;
		case 222: pline("%s used armoring!", Monnam(mdef)); break;
		case 223: pline("Your little-girl attack obviously doesn't hit!"); break;
		case 224: pline("Rattle/clink! %s lost nothing.", Monnam(mdef)); break;
		case 225: pline("%s has %sarmor!", Monnam(mdef), which_armor(mdef, W_ARM) ? "an " : "a nonexistant "); break;
		case 226: pline("%s is missed by your ammo.", Monnam(mdef)); break;
		case 227: pline("Clank! You hit a window by mistake!"); break;
		case 228: pline("%s avoids your ammo.", Monnam(mdef)); break;
		case 229: pline("Your ammo glances off %s because you're wimpy.", mon_nam(mdef)); break;
		case 230: pline("%s parries your ammo.", Monnam(mdef)); break;
		case 231: pline("Your shot passes under %s!", mon_nam(mdef)); break;
		case 232: pline("%s swipes your ammo away.", Monnam(mdef)); break;
		case 233: pline("You shoot off %s's hide!", mon_nam(mdef)); break;
		case 234: pline("You strike past %s!", mon_nam(mdef)); break;
		case 235: pline("%s says: 'Catch me, wiuuu!'", Monnam(mdef)); break;
		case 236: pline("The Super-%s is not hit, haha!", l_monnam(mdef)); break;
		case 237: pline("%s is really super!", Monnam(mdef)); break;
		case 238: pline("Yeah, you did not hit! Yeah!"); break;
		case 239: pline("You got out-cheated!"); break;
		case 240: pline("You recoil from %s!", mon_nam(mdef)); break;
		case 241: pline("%s has driven to you out of the way!", Monnam(mdef)); break;
		case 242: pline("Of course %s has, like everyone else, a %sshield.", mon_nam(mdef), which_armor(mdef, W_ARMS) ? "" : "nonexistant "); break;
		case 243: pline("%s drives off: Vroom!", Monnam(mdef)); break;
		case 244: pline("%s sounds its horn - eeeeeeeeeeep!", Monnam(mdef)); break;
		case 245: pline("%s simply drives away!", Monnam(mdef)); break;
		case 246: pline("%s laughs at you because you're such a wimp.", Monnam(mdef)); break;
		case 247: pline("You missed with your flamethrower!"); break;
		case 248: pline("%s drives up a set of stairs.", Monnam(mdef)); break;
		case 249: pline("%s collects new lines!", Monnam(mdef)); break;
		case 250: pline("%s abuses a flight glitch with its grenade launcher!", Monnam(mdef)); break;
		case 251: pline("You crashed into a house by mistake!"); break;
		case 252: pline("Your auto glances off of %s, striking sparks.", mon_nam(mdef)); break;
		case 253: pline("%s evades your ramming!", Monnam(mdef)); break;
		case 254: pline("%s resists your attack!", Monnam(mdef)); break;
		case 255: pline("%s uses the reverse gear!", Monnam(mdef)); break;
		case 256: pline("%s says: 'Check up!'", Monnam(mdef)); break;
		case 257: pline("%s says: 'Jerk off!'", Monnam(mdef)); break;
		case 258: pline("You jumped over %s!", mon_nam(mdef)); break;
		case 259: pline("%s goes behind a wall!", Monnam(mdef)); break;
		case 260: pline("%s protects itself!", Monnam(mdef)); break;
		case 261: pline("%s has a %sshield of course. You expected that, right?", Monnam(mdef), which_armor(mdef, W_ARMS) ? "" : "nonexistant "); break;
		case 262: pline("%s used a hiding place, that old baddie!", Monnam(mdef)); break;
		case 263: pline("%s is totally missed by you, wimp!", Monnam(mdef)); break;
		case 264: pline("%s jumps aside.", Monnam(mdef)); break;
		case 265: pline("%s does a draw backdown!", Monnam(mdef)); break;
		case 266: pline("%s diverted your attack with its ECM system.", Monnam(mdef)); break;
		case 267: pline("The scales of %s deflect.", mon_nam(mdef)); break;
		case 268: pline("%s uses a sign as a shield!", Monnam(mdef)); break;
		case 269: pline("You're too clumsy and hit the air."); break;
		case 270: pline("Haha, beside it! Hectar hectar!"); break;
		case 271: pline("You actually never hit %s!", mon_nam(mdef)); break;
		case 272: pline("%s has a special shield!", Monnam(mdef)); break;
		case 273: pline("%s uses a %sshield, hahaha", Monnam(mdef), which_armor(mdef, W_ARMS) ? "" : "nonexistant "); break;
		case 274: pline("You stumbled and %s is laughing at you.", mon_nam(mdef)); break;
		case 275: pline("%s self-teleports!", Monnam(mdef)); break;
		case 276: pline("Of course your weapon doesn't hit!"); break;
		case 277: pline("%s barely escapes!", Monnam(mdef)); break;
		case 278: pline("You fully hit the enemy's %sshield.", which_armor(mdef, W_ARMS) ? "" : "nonexistant "); break;
		case 279: pline("Your weapon passes through %s, with no effect.", mon_nam(mdef)); break;
		case 280: pline("You almost poked your %s out while fighting %s!", body_part(EYE), mon_nam(mdef)); break;
		case 281: pline("You almost slipped while fighting %s.", mon_nam(mdef)); break;
		case 282: pline("%s doesn't even flinch!", Monnam(mdef)); break;
		case 283: pline("You accidentally hurt yourself while fighting %s.", mon_nam(mdef)); break;
		case 284: pline("%s laughs at your wild swings.", Monnam(mdef)); break;
		case 285: pline("%s growls at you.", Monnam(mdef)); break;
		case 286: pline("%s is very, very annoyed!", Monnam(mdef)); break;
		case 287: pline("%s makes a nasty %s.", Monnam(mdef), mbodypart(mdef, FACE)); break;
		case 288: pline("%s gnashes his teeth.", Monnam(mdef)); break;
		case 289: pline("%s lets out a gurgling laugh.", Monnam(mdef)); break;
		case 290: pline("%s laughs at your low to-hit rating.", Monnam(mdef)); break;
		case 291: pline("You swing and swing but miss the mark every time!"); break;
		case 292: pline("Are you drunk or why do you never land a hit?"); break;
		case 293: pline("You should really try to aim better!"); break;
		case 294: pline("A wimp like you will never hit %s in melee.", mon_nam(mdef)); break;
		case 295: pline("%s blocks your attack and prepares a vicious counter.", Monnam(mdef)); break;
		case 296: pline("%s will make you fixed and finished, you!", Monnam(mdef)); break;
		case 297: pline("The Amy gave too much AC to %s and therefore you don't hit! Hahaha!", mon_nam(mdef)); break;
		case 298: pline("You are the greatest failure under the sun!"); break;
		case 299: pline("%s says: 'Nyah-nyah, you'll never hit me!'", Monnam(mdef)); break;
		case 300: pline("%s hides underneath an item, preventing you from hitting!", Monnam(mdef)); break;
		case 301: pline("%s decides that play-time is over and you will be killed now.", Monnam(mdef)); break;
		case 302: pline("A boulder (thrown by you) bounces off %s's %shelmet and deals no damage.", mon_nam(mdef), which_armor(mdef, W_ARMH) ? "" : "nonexistant "); break;
		case 303: pline("You'll never get the Junethack trophies if you continue playing that badly!"); break;
		case 304: pline("%s says: 'You N'wah!'", Monnam(mdef)); break;
		case 305: pline("%s shoots a gate against you!", Monnam(mdef)); break;
		case 306: pline("%s quickly builds a wall between you and it, so your attack fails!", Monnam(mdef)); break;
		case 307: pline("%s is the real identity of Donald Trump!", Monnam(mdef)); break;
		case 308: pline("%s says: 'Allahu Akbar!'", Monnam(mdef)); break;
		case 309: pline("%s will fuck you up, you bad player!", Monnam(mdef)); break;
		case 310: pline("%s knows for a fact that you will not win this game!", Monnam(mdef)); break;
		case 311: pline("Your weapon rusts and fails to damage %s!", mon_nam(mdef)); break;
		case 312: pline("%s smashes your sword apart! Hahahahahahaha!", Monnam(mdef)); break;
		case 313: pline("%s's armor gets stronger from deflecting your strike!", Monnam(mdef)); break;
		case 314: pline("%s's shield of reflection sends your attack right back at you!", Monnam(mdef)); break;
		case 315: pline("LOL, you missed again!"); break;
		case 316: pline("I laugh at your puny attempts to hit %s!", mon_nam(mdef)); break;
		case 317: pline("Ha, ha, ha, you really think such an attack could hit..."); break;
		case 318: pline("You missed again! If I were you I'd really train my to-hit!"); break;
		case 319: pline("You made yourself into the trousers because you're so afraid of %s.", mon_nam(mdef)); break;
		case 320: pline("%s laughs: 'Bwarharhar you won't win bwarharhar!'", Monnam(mdef)); break;
		case 321: pline("You try to kick %s but hit the wall. Ouch!", mon_nam(mdef)); break;
		case 322: pline("You try to assault %s but slip on a heap of dog shit.", mon_nam(mdef)); break;
		case 323: pline("A weakling %s cannot hit in melee, and you know that!", urole.name.m); break;
		case 324: pline("Do you fight like a girl on purpose?"); break;
		case 325: pline("Do you even want to hit anything? It sure doesn't look that way!"); break;
		case 326: pline("Your %s remains hanging at %s's high heel, and you sustain %sy scratches!", body_part(LEG), mon_nam(mdef), body_part(BLOOD)); break;
		case 327: pline("Your attack attempts are stopped in their tracks as %s kicks you in the nuts.", mon_nam(mdef)); break;
		case 328: pline("Your boss suddenly enters the room. Startled, you hit the wrong direction key and waste a turn."); break;
		case 329: pline("If your math teacher knew that 9 out of 10 of your attacks are missing..."); break;
		case 330: pline("Did you really not notice yet that you're never hitting %s? ROFL.", mon_nam(mdef)); break;
		case 331: pline("%s is a deluxe troll!", Monnam(mdef)); break;
		case 332: pline("%s pulls down your trousers, ha-ha!", Monnam(mdef)); break;
		case 333: pline("%s says: 'Catch me nevertheless, you old hole!'", Monnam(mdef)); break;
		case 334: pline("%s is an asshole because it always evades your attacks!", Monnam(mdef)); break;
		case 335: pline("The cowardly woman of %s runs away instead of letting you hit it!", mon_nam(mdef)); break;
		case 336: pline("%s preaches some boring sermon to you, causing you to fall asleep instead of attacking!", Monnam(mdef)); break;
		case 337: pline("%s is laughing all the time, and the distraction causes you to miss!", Monnam(mdef)); break;
		case 338: pline("You attack %s 6 times and do not meet!", mon_nam(mdef)); break;
		case 339: pline("%s punches you into the %s.", Monnam(mdef), body_part(STOMACH)); break;
		case 340: pline("%s stomps your %s with its plateau boots.", Monnam(mdef), makeplural(body_part(TOE))); break;
		case 341: pline("%s slams a wooden sandal against your %s, and the beautiful pain prevents you from retaliating.", Monnam(mdef), body_part(LEG)); break;
		case 342: pline("%s is much better than you!", Monnam(mdef)); break;
		case 343: pline("Unlike you, %s is really strong! Didn't you know that?", mon_nam(mdef)); break;
		case 344: pline("You game lags for a moment, and so you don't hit %s but it hits you in reverse.", mon_nam(mdef)); break;
		case 345: pline("Your %s slips, like it always does.", body_part(HAND)); break;
		case 346: pline("Hahaha, it's really fun to watch you futilely swing that little sword around."); break;
		case 347: pline("Is your sword made of paper or what? It would explain why you're unable to hurt %s!", mon_nam(mdef)); break;
		case 348: pline("Now come on, you need a better weapon. Your current one is even less effective than a wet trout!"); break;
		case 349: pline("You fumble around with your dagger, missing %s's %s.", mon_nam(mdef), mbodypart(mdef, FACE)); break;
		case 350: pline("You shoot %s with your pistol, but the bullet glances off its bullet-proof vest!", mon_nam(mdef)); break;
		case 351: pline("You try to stab your spear in %s's %s, but cannot penetrate the steel plate covering it!", mon_nam(mdef), mbodypart(mdef, LEG)); break;
		case 352: pline("You are fully the failure!"); break;
		case 353: pline("And you seriously think you're gonna ascend if you keep missing like that?"); break;
		case 354: pline("%s skillfully evades you.", Monnam(mdef)); break;
		case 355: pline("%s seems more experienced from evading your puny attack!", Monnam(mdef)); break;
		case 356: pline("%s mocks you by producing farting noises with its butt.", Monnam(mdef)); break;
		case 357: pline("You break off one of your nails while trying to fight %s!", mon_nam(mdef)); break;
		case 358: pline("Klock! %s slammed a sexy leather pump on your %s, producing a beautiful sound.", Monnam(mdef), body_part(HEAD)); break;
		case 359: pline("You're lucky you didn't trip over your own sword."); break;
		case 360: pline("Your cumbersome armour gets in the way of the attack."); break;
		case 361: pline("You clumsily miss with the unweildy weapon"); break;
		case 362: pline("The small creature nimbly dodges out of the way of your strike."); break;
		case 363: pline("Your half assed attack failed to hit %s!", mon_nam(mdef)); break;
		case 364: pline("Your fancy footwork did not fool %s!", mon_nam(mdef)); break;
		case 365: pline("You could not hit the broadside of a barn with that pathetic attempt."); break;
		case 366: pline("You perform a decent attack but was perfectly parried by %s!", mon_nam(mdef)); break;
		case 367: pline("Your fumbled attack completely missed %s!", mon_nam(mdef)); break;
		case 368: pline("You did not hit because you are such a loser!"); break;
		case 369: pline("You feel as if the screwy terrahack to-hit calculations have been enabled, since you unexpectedly miss %s.", mon_nam(mdef)); break;
		case 370: pline("It appears that someone has turned the game into K-Measurer, because you missed %s despite your to-hit actually being rather good.", mon_nam(mdef)); break;
		case 371: pline("Are you sure you're not playing Rodney's Tower by mistake? Here in Slash'EM Extended, you should hit %s... but you didn't!", mon_nam(mdef)); break;
		case 372: pline("Apparently you're playing Russiack, because your to-hit bonus doesn't seem to work and therefore you miss."); break;
		case 373: pline("Your penis-shaped sword doesn't hit of course!"); break;
		case 374: pline("Are you scared of %s or are you really incapable of hitting the broad side of a barn?", mon_nam(mdef)); break;
		case 375: pline("What, you keep attacking monsters in melee even though you have the Fear status effect? No wonder you never hit!"); break;
		case 376: pline("You miss like always, because you're simply very bad at this game."); break;
		case 377: pline("Why do you keep trying to hit with such a wimpy weapon?"); break;
		case 378: pline("You hit yourself because your dexterity is way too low!"); break;
		case 379: pline("%s laughs all the time while your strikes only hit the air!", Monnam(mdef)); break;
		case 380: pline("Rattle/clink! You accidentally smashed your weapon into the wall!"); break;
		case 381: pline("Hooooooooo he he he he he!"); break;
		case 382: pline("%s says: 'Harharharharharharharrrrr!'", Monnam(mdef)); break;
		case 383: pline("Wow, you really are the greatest. The greatest failure of the world!"); break;
		case 384: pline("You fire your assault rifle 24 times and miss 25 times!"); break;
		case 385: pline("That's typical. Always brag about your deeds but unable to even hit the wimpy %s!", l_monnam(mdef)); break;
		case 386: pline("Your stick isn't long enough!"); break;
		case 387: pline("You try to bludgeon %s with your penis but the attempt fails.", mon_nam(mdef)); break;
		case 388: pline("%s constructed a nasty trap that causes you to miss!", Monnam(mdef)); break;
		case 389: pline("%s was really just a displaced image of a monster standing on an adjacent tile!", Monnam(mdef)); break;
		case 390: pline("%s tickles you, and you're unable to get an attack off!", Monnam(mdef)); break;
		case 391: pline("You probably forgot that you're infected with the Russiack disease, which causes you to automiss."); break;
		case 392: pline("Some evil eye reprogrammed the game, and it is now Eyehack where your weapon automatically misses 1 out of 4 times. This means that your current attack, which would otherwise have hit, does not."); break;
		case 393: pline("%s turned on the Kneelhack switch, causing it (and also all other monsters) to gain massive amounts of AC and of course your wimpy little-girl attack is too weak to penetrate that.", Monnam(mdef)); break;
		case 394: pline("The Bonuseyehack error-bug mode has been activated, and as a result the top status line doesn't tell you whether your attack hit or not."); break;
		case 395: pline("Because of the server lag, your attack misses."); break;
		case 396: pline("You are playing in real-time mode, and you took too long to press that button. Therefore, %s was able to parry your attack.", mon_nam(mdef)); break;
		case 397: pline("%s says: 'Sorry %s, but I don't have a photo for you today.'", Monnam(mdef), playeraliasname); break;
		case 398: pline("You fire your pistol at %s but the bullet just barely misses!", mon_nam(mdef)); break;
		case 399: pline("%s produces erotic air current noises, and you forget that you actually wanted to attack it.", Monnam(mdef)); break;
		case 400: pline("%s steals all your Junethack trophies because you didn't watch out.", Monnam(mdef)); break;
		case 401: pline("You pushed the wrong button and therefore wasted a turn instead of attacking."); break;
		case 402: pline("If you keep missing like this, you will end up abusing your dexterity and then you'll miss even more!"); break;
		case 403: pline("You should really realize that melee combat is not what your current character is good at. Why don't you try to cast some spells instead?"); break;
		case 404: pline("You fucked it up again, and %s is laughing all the time.", mon_nam(mdef)); break;
		case 405: pline("If you think you're missing a lot now, wait until %s installs K-Measurer on your PC where you'll REALLY miss almost every time!", mon_nam(mdef)); break;
		case 406: pline("%s is really an ancient red dragon from K-Measurer, which has so much AC that it's almost impossible to hit!", mon_nam(mdef)); break;
		case 407: pline("You expected your melee attacks to hit without using buff spells first? Ha!"); break;
		case 408: pline("You're putting %s to shame.", urace.coll); break;
		case 409: pline("You're just a wimpy %s! You can't do anything right!", urace.noun); break;
		case 410: pline("Useless %s scum like you can't even hope to stand a chance against %s.", urace.adj, mon_nam(mdef)); break;
		case 411: pline("%s will call the Kops to punish you if you keep missing.", align_gname(A_LAWFUL)); break;
		case 412: pline("%s watches your futile attempts with disdain.", align_gname(A_NEUTRAL)); break;
		case 413: pline("Do you want to anger %s? Then by all means continue.", align_gname(A_CHAOTIC)); break;
		case 414: pline("Your ancestors are cursing the name '%s' from their grave as they see you actually manage to miss %s!", playeraliasname, mon_nam(mdef)); break;
		case 415: pline("%s, you are such a loser!", playeraliasname); break;
		case 416: pline("%s escapes from you by using a pogo stick!", Monnam(mdef)); break;
		case 417: pline("%s chloroforms you just as your weapon is about to strike.", Monnam(mdef)); break;
		case 418: pline("%s is actually a monster from the Evil Variant(TM), and instakills you. Do you want your possessions identified? DYWYPI? (ynq) (y) _", Monnam(mdef)); break;
		case 419: pline("Suddenly the variant you're playing turns into GameOverHack. Do you want your possessions identified? DYWYPI? (ynq) (y) _"); break;
		case 420: pline("Suddenly the variant you're playing turns into ScrewHack. You miss %s! Oh no, it uses a passive touch of death! Do you want your possessions identified? DYWYPI? (ynq) (y) _", mon_nam(mdef)); break;
		case 421: pline("%s summons 5 titanotheres and 10 large cats. The titanothere bites! The large cat hits! The large cat hits! The titanothere bites! The large cat hits! You die...", Monnam(mdef)); break;
		case 422: pline("You obtain a master's degree in missing. Well done, now you can miss even the easiest monsters in existence (%s for example).", mon_nam(mdef)); break;
		case 423: pline("%s hangs %s long bundle into your %s, and the soothing feeling prevents you from attacking.", Monnam(mdef), mhis(mdef), body_part(FACE)); break;
		case 424: pline("You are dispirited and therefore unable to attack %s.", mon_nam(mdef)); break;
		case 425: pline("You are confused! You accidentally hurt yourself in your confusion!"); break;
		case 426: pline("You're fully paralyzed and can't attack!"); break;
		case 427: pline("Your attack goes way off!"); break;
		case 428: pline("%s slips in between your %s.", Monnam(mdef), makeplural(body_part(LEG))); break;
		case 429: pline("The audience cheers for %s, who keeps hitting you while you keep missing!", mon_nam(mdef)); break;
		case 430: pline("Ah, the incapable %s whiffing at %s again, although even a little child could hit such a weak monster!", playeraliasname, mon_nam(mdef)); break;
		case 431: pline("%s yawns: 'Why don't you just surrender... I've got better things to do today than fighting with a punching bag like you.'", Monnam(mdef)); break;
		case 432: pline("You accidentally drop your weapon just as you try to strike %s.", mon_nam(mdef)); break;
		case 433: pline("%s snatches your weapon!", mon_nam(mdef)); break;
		case 434: pline("%s mumbles an incantation, and your weapon becomes cursed.", mon_nam(mdef)); break;
		case 435: pline("%s mumbles an incantation, and your weapon seems less effective.", mon_nam(mdef)); break;
		case 436: pline("In order to keep things fair, the RNG decides that you miss because otherwise the poor %s would have no chance at all.", mon_nam(mdef)); break;
		case 437: pline("%s laughs: 'Neener-neener, catch me if you caa-an!'", Monnam(mdef)); break;
		case 438: pline("%s suddenly decides that the game you're playing is not Slash'EM Extended but SatanHack instead, and opens a gate to summon a couple major demons including Demogorgon.", Monnam(mdef)); break;
		case 439: pline("%s presses a direction button on your keyboard, and as a result your character doesn't walk in the direction you intended.", Monnam(mdef)); break;
		case 440: pline("You held down the key. As a punishment, none of your attacks against %s are going to hit until you let go.", mon_nam(mdef)); break;
		case 441: pline("You crash out of the game just as it's trying to show the message telling you about your attack against %s!", mon_nam(mdef)); break;
		case 442: pline("%s is a glitch monster with impossibly good AC, so you have no chance of hitting, ever!", Monnam(mdef)); break;
		case 443: pline("You strike %s, but %s is immune to melee strikes.", mon_nam(mdef), mhe(mdef)); break;
		case 444: pline("Hahahahaha, you can't even hit a %s... what are you gonna do if you encounter a %s later?", l_monnam(mdef), rndmonnam()); break;
		case 445: pline("Be warned: if you miss %s one more time, a %s will spawn and own you.", mon_nam(mdef), rndmonnam()); break;
		case 446: pline("Your %s turns %s in shame as your attack misses its mark.", body_part(FACE), rndcolor()); break;
		case 447: pline("Well, I guess your luck ran out, because you're not hitting %s anymore.", mon_nam(mdef)); break;
		case 448: pline("You don't hit the miserable hussy of %s!", mon_nam(mdef)); break;
		case 449: pline("%s's feminine scent repels your attack!", Monnam(mdef)); break;
		case 450: pline("%s encloses you in a stinking cloud that blocks your sight and prevents you from aiming properly!", Monnam(mdef)); break;
		case 451: pline("%s makes a peace offering, and you're so stupid to accept it.", Monnam(mdef)); break;
		case 452: pline("%s turns on the challenge mode that makes all your attacks automiss half of the time.", Monnam(mdef)); break;
		case 453: pline("You're too encumbered, and that hampers your to-hit chances!"); break;
		case 454: pline("%s requires a really skilled character to defeat, and you're not playing such a character! Flee and die another day!", Monnam(mdef)); break;
		case 455: pline("You get the airstrike bug: even though you aimed perfectly, your sword still only strikes the air instead of %s.", mon_nam(mdef)); break;
		case 456: pline("You perform another blank zeal swipe."); break;
		case 457: pline("You fend 17 times in a row and miss with all attacks every time. Considering that a fend cycle consists of 5 attacks, that means you just missed %s 85 times in a row! WOW!", mon_nam(mdef)); break;
		case 458: pline("%s tells you to suck a dick and fuck off.", Monnam(mdef)); break;
		case 459: pline("%s pushes you away from the analysis port, calling you nasty names in the process.", Monnam(mdef)); break;
		case 460: pline("%s condemns you to an expensive hall punishment.", Monnam(mdef)); break;
		case 461: pline("Suddenly %s fires a teleporting ball at you, which forces you to play through an auxiliary level before you can continue fighting.", mon_nam(mdef)); break;
		case 462: pline("You try to bar %s but miss completely.", mon_nam(mdef)); break;
		case 463: pline("As your weapon strikes %s, it falls apart without doing any damage to the enemy whatsoever.", mon_nam(mdef)); break;
		case 464: pline("Your weapon merely strokes %s but doesn't cause any damage.", mon_nam(mdef)); break;
		case 465: pline("The filthy %s uses your %s as a cigarette extinguisher.", l_monnam(mdef), body_part(FACE)); break;
		case 466: pline("The %s asshole blows %s cigarette smoke into your %s, and you get a severe coughing fit.", l_monnam(mdef), mhis(mdef), body_part(FACE)); break;
		case 467: pline("%s's fleecy leather jacket deflects your blow.", Monnam(mdef)); break;
		case 468: pline("%s is looking very sexy and charms you, preventing you from getting an efficient attack off.", Monnam(mdef)); break;
		case 469: pline("Suddenly %s constructs a wall between you and %s, and your weapon fully smashes into it.", mon_nam(mdef), mhim(mdef)); break;
		case 470: pline("You're such a weakling and miss! Ha-ha!"); break;
		case 471: pline("You forgot to take the point and therefore your attack goes off."); break;
		case 472: pline("You failed to look at the pokedex and thus didn't realize that %s's armor class is impossible for you to hit.", mon_nam(mdef)); break;
		case 473: pline("%s pours a bottle of salt over your %s.", Monnam(mdef), body_part(HEAD)); break;
		case 474: pline("%s does not budge!", Monnam(mdef)); break;
		case 475: pline("%s slides its buckle over your skin and you fail to get your attack off!", Monnam(mdef)); break;
		case 476: pline("%s's zipper doesn't come up!", Monnam(mdef)); break;
		case 477: pline("%s uses its lacings as a shackle and ties you up!", Monnam(mdef)); break;
		case 478: pline("Your attempt to open %s's velcro lash fails!", mon_nam(mdef)); break;
		case 479: pline("Whoops, you accidentally smashed your %s into a wall!", body_part(ARM)); break;
		case 480: pline("Your left %s remains hanging at a thorny rose, and you scream in pain.", body_part(ARM)); break;
		case 481: pline("%s dunks your %s into a pool of infectious waste!", Monnam(mdef), body_part(HEAD)); break;
		case 482: pline("%s presses a log of shit on your %s.", Monnam(mdef), makeplural(body_part(HAND))); break;
		case 483: pline("Your hit connects but glances from %s's %s! Apparently %s is made of steel (maybe a relative of Superman)!", mon_nam(mdef), mbodypart(mdef, STOMACH), mon_nam(mdef)); break;
		case 484: pline("You helplessly swing at %s with no chance of hitting.", mon_nam(mdef)); break;
		case 485: pline("You cry like a little baby because you can't handle the fact that %s is 0wning you.", mon_nam(mdef)); break;
		case 486: pline("Engulfed in fury and seething rage you try to smash %s's stupid %s in, but %s is just laughing off your futile attempts.", mon_nam(mdef), mbodypart(mdef, FACE), mhe(mdef)); break;
		case 487: pline("%s sticks out %s tongue to mock you for your weak attacks.", Monnam(mdef), mhis(mdef)); break;
		case 488: pline("%s spits at your %s.", Monnam(mdef), makeplural(body_part(FOOT))); break;
		case 489: pline("%s evades your attack, then tells you that your mother is a devil's whore.", Monnam(mdef)); break;
		case 490: pline("%s dances back and forth like a clown, and you can't seem to land a hit.", Monnam(mdef)); break;
		case 491: pline("%s swerves back and forth, narrowly avoiding your furious attack.", Monnam(mdef)); break;
		case 492: pline("You're completely losing the fight against %s. Case in point: just now you missed again.", mon_nam(mdef)); break;
		case 493: pline("%s interlocks your weapon and thereby prevents the hit!", Monnam(mdef)); break;
		case 494: pline("%s's thorns spell causes you to hurt yourself instead of it!", Monnam(mdef)); break;
		case 495: pline("You just know that you can't ever hope to compete with %s.", mon_nam(mdef)); break;
		case 496: pline("%s makes a 'click' sound that reminds you of a timebomb, and you decide to back off.", Monnam(mdef)); break;
		case 497: pline("For a moment, your conscience bothers you... and as a result, you don't follow through on your plan to attack %s.", mon_nam(mdef)); break;
		case 498: pline("You cucumber wimp! You can't even hit the easy-to-hit %s!", l_monnam(mdef)); break;
		case 499: pline("Whoa your attacks are so nooblike, no wonder you don't hit."); break;
		case 500: pline("You fall down yet again."); break;
		case 501: pline("Can you even get one single attack off? Sure doesn't seem like it!"); break;
		case 502: pline("You strike %s with your shield, but %s is immune to the attack.", mon_nam(mdef), mon_nam(mdef)); break;
		case 503: pline("You shoot at %s, but it's out of range and therefore you don't hit.", mon_nam(mdef)); break;
		case 504: pline("You manage to miss %s even though you're attacking at point blank range!", mon_nam(mdef)); break;
		case 505: pline("Your weapon actually hits %s, but the stupid collision detection failed to register the hit.", mon_nam(mdef)); break;
		case 506: pline("The RNG just doesn't feel like letting you hit the monsters today, and therefore you miss %s.", mon_nam(mdef)); break;
		case 507: pline("Your weapon mortally hits %s, but suddenly the referee comes, declares the hit invalid, and fully heals %s.", mon_nam(mdef), mon_nam(mdef)); break;
		case 508: pline("You would have killed %s with this attack, but killing monsters is not allowed right now and therefore your slash stops short of %s's %s.", mon_nam(mdef), mon_nam(mdef), mbodypart(mdef, NECK)); break;
		case 509: pline("An annoying rule says that you may not hurt %s right now, but %s is fully allowed to constantly fart you in the face, which is really unfair.", mon_nam(mdef), mhe(mdef)); break;
		case 510: pline("You try to slam your cute calf-leather sandals into %s's leg, but it quickly evades your kick!", mon_nam(mdef)); break;
		case 511: pline("You suddenly get a sexual orgasm from the sight of %s's beauty, and find yourself unable to continue fighting.", mon_nam(mdef)); break;
		case 512: pline("You're so %s that you can't hit the broad side of a barn!", body_part(LIGHT_HEADED)); break;
		case 513: pline("%s's sexiness deflects the attack!", Monnam(mdef)); break;
		case 514: pline("%s intoxicates you with its perfume, causing you to accidentally sheathe your weapon!", Monnam(mdef)); break;
		case 515: pline("%s steals your weapon and you don't even notice!", Monnam(mdef)); break;
		case 516: pline("%s breaks the fourth wall by announcing 'The person in front of the monitor is a very bad SLEX player'.", Monnam(mdef)); break;
		case 517: pline("%s laughs: 'I ascended SLEX, unlike you! It's because I can actually hit things!'", Monnam(mdef)); break;
		case 518: pline("%s realizes that you're no threat and gets ready to retrieve the Amulet of Yendor %sself and ascend before you do.", Monnam(mdef), mhim(mdef)); break;
		case 519: pline("%s is praying to Moloch right now and therefore invulnerable! You must wait with your attack until it finished the prayer!", Monnam(mdef)); break;
		case 520: pline("%s cannot be harmed by your melee attacks! Ha-ha!", Monnam(mdef)); break;
		case 521: pline("You were so stupid and smashed a sink with your weapon when trying to hit %s, and now you have to buy a new one for 500 zorkmids.", mon_nam(mdef)); break;
		case 522: pline("You must somehow have contracted a weakness, because %s is losing no lines!", mon_nam(mdef)); break;
		case 523: pline("You are such a failure and therefore have to clean the dog shit off of the %s shoes.", l_monnam(mdef)); break;
		case 524: pline("%s extends %s beautiful claws and pinches your skin drawing %s, then retracts the cute claws again.", Monnam(mdef), mhis(mdef), body_part(BLOOD)); break;
		case 525: pline("%s doesn't feel like helping you and therefore decides that this time your attack misses.", u_gname()); break;
		case 526: pline("%s has just decided to send bad luck your way, and here's the first sign of it: you missed %s even though normally you should have hit.", (!rn2(3) ? align_gname(A_LAWFUL) : rn2(2) ? align_gname(A_NEUTRAL) : align_gname(A_CHAOTIC)), mon_nam(mdef)); break;
		case 527: pline("%s demands that you use 'xe/xir' pronouns when addressing it, and while you ponder the meaning of that, your opening for an attack has ceased to exist.", Monnam(mdef)); break;
		case 528: pline("You use your %s to feel up the tender %s. It feels very fleecy, but that doesn't actually help you win the fight...", makeplural(body_part(HAND)), mon_nam(mdef)); break;
		case 529: pline("You spontaneously decide to kiss %s in the middle of the fight, but %s disgustedly brushes you away.", mon_nam(mdef), mhe(mdef)); break;
		case 530: pline("You can't resist %s's tender butt cheeks, and start showering them with kisses. But %s actually wants to kill you and abuses the fact that you forgot to watch your defenses.", mon_nam(mdef), mon_nam(mdef)); break;
		case 531: pline("You're like the bride on the wedding night - can't seem to find the hole!"); break;
		case 532: pline("You hit %s with a cotton ball. As expected, that attack deals no damage.", mon_nam(mdef)); break;
		case 533: pline("You kick %s in the nuts, but he's wearing blue pants that protect his most sensitive body parts very well!", mon_nam(mdef)); break;
		case 534: pline("You massage %s's pants and marvel about how you could squash his delicate nuts with your %s... but then don't do it after all, because you don't want to cause pain to him.", mon_nam(mdef), makeplural(body_part(HAND))); break;
		case 535: pline("You suddenly discover that you really like pain, and beg %s to kick you powerfully with %s soft footwear.", mon_nam(mdef), mhis(mdef)); break;
		case 536: pline("You left the bunsen burner on! Now you have to stop fighting %s and think about where you left it!", mon_nam(mdef)); break;
		case 537: pline("You're not wearing your Depends(TM) today, and therefore you accidentally shit your trousers."); break;
		case 538: pline("%s threatens that your data may be deleted, and you hold off on attacking.", Monnam(mdef)); break;
		case 539: pline("%s ducks under your swing because %s's such a little ass.", Monnam(mdef), mhe(mdef)); break;
		case 540: pline("%s squeaks like a little girl, and you're so fascinated that you just stand there instead of fighting.", Monnam(mdef)); break;
		case 541: pline("%s converts you to %s religion just as you're about to attack.", Monnam(mdef), mhis(mdef)); break;
		case 542: pline("Your stinking boots cannot hurt %s.", mon_nam(mdef)); break;
		case 543: pline("%s grabs your nuts and squeezes them tight!", Monnam(mdef)); break;
		case 544: pline("Now you look old, because %s fully pwns you.", mon_nam(mdef)); break;
		case 545: pline("You try to sell %s, but fail.", mon_nam(mdef)); break;
		case 546: pline("You'll never be able to rescue %s that way.", urole.homebase); break;
		case 547: pline("If you can't even hit the wimpy %s, you can completely forget about your mission to purge %s.", l_monnam(mdef), urole.intermed); break;
		case 548: pline("All your friends at %s are booing at the TV screen that shows you missing constantly with that little sword.", urole.homebase); break;
		case 549: pline("You feel the voice of your worst enemy penetrate your mind from %s. It says: 'Ha ha ha, you tin can are no match for me.'", urole.intermed); break;
		case 550: pline("Some guy comes and diverts you, so you forget to attack."); break;
		case 551: pline("%s clamps your %s with %s lovely pink pumps.", Monnam(mdef), makeplural(body_part(FINGER)), mhis(mdef)); break;
		case 552: pline("%s is a master of observation haki and thereby predicted your attack, allowing %s to evade easily!", Monnam(mdef), mhim(mdef)); break;
		case 553: pline("You fail to penetrate %s's armament haki!", mon_nam(mdef)); break;
		case 554: pline("%s ate from the vindication devil fruit, and immobilizes you.", Monnam(mdef)); break;
		case 555: pline("%s places a bounty on your head.", Monnam(mdef)); break;
		case 556: pline("Well you're obviously unable to get past %s's haki. You have to figure out yourself how to hit %s!", mon_nam(mdef), mhim(mdef)); break;
		case 557: pline("As you try to fight %s, you find out to your shock that %s is controlling two devil fruits, which is actually impossible but somehow %s managed to do it. As a result, your attack completely fails.", mon_nam(mdef), mhe(mdef), mon_nam(mdef)); break;
		case 558: pline("%s is encased in some sort of cocoon and your sword fails to penetrate it!", Monnam(mdef)); break;
		case 559: pline("You're chasing %s and are about to kill %s, when suddenly some weird light envelopes %s and saves %s in front of your %s!", mon_nam(mdef), mhim(mdef), mhim(mdef), mon_nam(mdef), makeplural(body_part(EYE))); break;
		case 560: pline("You set fire to %s's house with %s inside, and wait for it to burn down... but then %s starts praying to %s god, and gets teleported out unharmed. You call bullshit.", mon_nam(mdef), mhim(mdef), mon_nam(mdef), mhis(mdef)); break;
		case 561: pline("%s stole all of your attacks and therefore owns you.", Monnam(mdef)); break;
		case 562: pline("%s blew the whistle a third time, which means that you ran out of time and lose the game!", Monnam(mdef)); break;
		case 563: pline("%s tramples all over your body with %s thick block-heeled boots.", Monnam(mdef), mhis(mdef)); break;
		case 564: pline("%s suddenly dresses up as Pocahontas and points a %s in a specific direction. You're actually stupid enough to start walking in that direction.", Monnam(mdef), mbodypart(mdef, FINGER)); break;
		case 565: pline("Suddenly %s sinks your ship!", mon_nam(mdef)); break;
		case 566: pline("%s has eaten from the floating devil fruit and hovers just out of reach of your weapon!", Monnam(mdef)); break;
		case 567: pline("Your attempt to incinerate %s fails. After all, %s's Japanese, and those are fireproof.", mon_nam(mdef), mhe(mdef)); break;
		case 568: pline("%s reminds you that you've forgotten to wear a face mask.", Monnam(mdef)); break;
		case 569: pline("Suddenly %s sneezes into your %s! Oh no! Now you're infected with covid-19!", mon_nam(mdef), body_part(FACE)); break;
		case 570: pline("As you attempt to attack %s, %s suddenly hangs a used facial tissue into your %s. You don't even want to know how many corona viri you inhaled.", mon_nam(mdef), mhe(mdef), body_part(FACE)); break;
		case 571: pline("%s goes to your cannonball out of the way!", Monnam(mdef)); break;
		case 572: pline("%s uses the Emergency Evasion ability to avoid getting hit!", Monnam(mdef)); break;
		case 573: pline("Hahaha, %s is immune to such attacks.", mon_nam(mdef)); break;
		case 574: pline("Since you only increased your damage output, but not your to-hit, your attack didn't hit."); break;
		case 575: pline("It seems that with your low to-hit rating, a well-armored enemy like %s is next to impossible to hit.", mon_nam(mdef)); break;
		case 576: pline("%s requires a +4 weapon to hit, which you don't have, so all of your attacks are ineffective!", Monnam(mdef)); break;
		case 577: pline("Whoops - it seems you accidentally misplaced your weapon!"); break;
		case 578: pline("%s suddenly shouts 'CORONA!' Frightened, you don't manage to get an attack off.", Monnam(mdef)); break;
		case 579: pline("You almost fell into the tar pit!"); break;
		case 580: pline("If you continue like this, %s can soon attend your funeral.", Monnam(mdef)); break;
		case 581: pline("As you charge at %s, you get acquainted with %s spear.", mon_nam(mdef), mhis(mdef)); break;
		case 582: pline("%s tells you to go smell your own socks.", Monnam(mdef)); break;
		case 583: pline("You slowly close in on %s with the intention of attacking in melee, but %s stays just out of range while peppering you with arrows.", mon_nam(mdef), mhe(mdef)); break;
		case 584: pline("You've inhaled too much of %s's Chanel No. 25 perfume, which causes you to suddenly become blind and be unable to fight!", mon_nam(mdef)); break;
		case 585: pline("%s decides to teach you a lesson by firing lightning spikes at you.", Monnam(mdef)); break;
		case 586: pline("%s polishes your visage.", Monnam(mdef)); break;
		case 587: pline("You accidentally charge past %s, who swiftly kicks you in the calf with %s high heels.", mon_nam(mdef), mhis(mdef)); break;
		case 588: pline("The entire lower part of your arm slides along %s's fingernails, and your %s is squirting in all directions.", mon_nam(mdef), body_part(BLOOD)); break;
		case 589: pline("%s is really washed with all landing on water!", Monnam(mdef)); break;
		case 590: pline("You try to throw a totally homosexual spin kick at %s, but miss (predictably).", mon_nam(mdef)); break;
		case 591: pline("You charge %s and try a jump kick, but %s narrowly escapes!", mon_nam(mdef), mhe(mdef)); break;
		case 592: pline("You attempt to batter %s with punches, but your first punch misses and because of some stupid rule you now have to wait for an entire second before you can try again!", mon_nam(mdef)); break;
		case 593: pline("Your thrown knife doesn't hit %s!", mon_nam(mdef)); break;
		case 594: pline("%s's brightly colored sneaker soles look so exciting that you just stand there staring at them instead of fighting!", Monnam(mdef)); break;
		case 595: pline("%s is species number %d, which just so happens to be the one you don't know how to fight!", Monnam(mdef), mdef->mnum); break;
		case 596: pline("%s has a whopping %d movement points left, and uses them to evade your laughable attack!", Monnam(mdef), mdef->movement); break;
		case 597: pline("You noobie! Why are you expecting to be able to hit a level %d monster?", mdef->m_lev); break;
		case 598: pline("Hahaha! %s is unhittable while on square %d,%d! Didn't you know that?", Monnam(mdef), mdef->mx, mdef->my); break;
		case 599: pline("Lol, %s still has %d hit points remaining because you just can't seem to hit!", mon_nam(mdef), mdef->mhp); break;
		case 600: pline("A total failure like you will never be able to take off all of %s's maximum %d hit points.", mon_nam(mdef), mdef->mhpmax); break;
		case 601: pline("%s has a mana supply of %d Pw, and casts a shielding spell that deflects your attack.", Monnam(mdef), mdef->m_en); break;
		case 602: pline("%s is polymorphed from the monster species %d, and since you don't want it to turn back into that dangerous form, you miss on purpose.", Monnam(mdef), mdef->oldmonnm); break;
		case 603: pline("Your character just doesn't feel like hitting %s today, and swings at thin air instead.", mon_nam(mdef)); break;
		case 604: pline("Somehow, your char doesn't have the required badge to be fighting %s, and dozes off instead of fighting properly, like in the Pokemon game series.", mon_nam(mdef)); break;
		case 605: pline("%s has a farting bonus of %d, and therefore produces beautiful farting noises constantly which divert you and cause your swing to miss!", Monnam(mdef), mdef->fartbonus); break;
		case 606: pline("%s uses a crapping bonus of %d to press a log of shit right into your %s, incapacitating you until you clean it off.", Monnam(mdef), mdef->crapbonus, body_part(FACE)); break;
		case 607: pline("%s's homing lazer currently has a charge count of %d, and you shit your pants as you realize that you'll be shot with the laser cannon soon. Perhaps you should flee.", Monnam(mdef), mdef->hominglazer); break;
		case 608: pline("%s decides that enough is enough. After all, you've killed %ld monsters already; you'll not manage to kill another, you evil mass murderer.", Monnam(mdef), u.uconduct.killer); break;
		case 609: pline("%s laughs: 'You've had sex %ld times since you entered the dungeon! It seems that sex is the only thing on your mind! No wonder you can't hit with that wimpy sword, hahaha!'", Monnam(mdef), u.uconduct.celibacy); break;
		case 610: pline("%s prepares to rape your sorry ass.", Monnam(mdef)); break;
		case 611: pline("%s simply deletes your savegame file that you've put dozens of hours into, claiming that it's because the game supposedly contains pedophilia, which it actually doesn't.", Monnam(mdef)); break;
		case 612: pline("%s bans you from the gaming community, labels you a pariah, and then mocks you by stating that you supposedly exiled yourself when it was really the community exiling you.", Monnam(mdef)); break;
		case 613: pline("Your base spellcasting penalty is only %d... don't you want to attempt spells instead of melee?", urole.spelbase); break;
		case 614: pline("Not only does your shield give you an additional %d penalty to casting your spells, it additionally encumbers you, causing your attacks to miss! Why don't you just take it off?", urole.spelshld); break;
		case 615: pline("Look, here's a hint. Your character isn't a good fighter. You're playing a spellcaster! And in particular, your special spell is the one numbered %d - you'd be wise to cast it!", urole.spelspec); break;
		case 616: pline("You tell %s that your pronouns are %s and %s. This has no effect on the current battle, obviously, but results in %s mocking you by answering 'For me, your pronouns are 'screw you' and 'get the fuck outta here'. Now kindly remove yourself from my sights.'", mon_nam(mdef), uhe(), uhim(), mon_nam(mdef)); break;
		case 617: pline("%s laughs at you because you're only level %d.", Monnam(mdef), u.ulevel); break;
		case 618: pline("%s snickers: 'Getting hungry, eh? Well I'd be hungry too if I only had %d nutrition left. Too bad you'll never be able to kill me and eat my corpse, wimp!'", Monnam(mdef), u.uhunger); break;
		case 619: pline("Suddenly you shiver in fear because you only have %d hit points remaining. You decide that it would be wise to flee from this fight.", u.uhp); break;
		case 620: pline("Man, you have %d Pw remaining... why aren't you using it? Some noob you are!", u.uen); break;
		case 621: pline("You keep swinging and swinging to no avail, I really don't get why you're never casting from your pool of %d max Pw! Maybe you should learn how to play this game properly?", u.uenmax); break;
		case 622: pline("The gods are getting really angry at you. They don't want to have to watch over someone who plays that badly, and their divine anger amounts to %d already.", u.ugangr); break;
		case 623: pline("%s laughs: 'You trousershitter are running around with %d points of divine protection? Too scared to fight me without that help? Wimp.'", Monnam(mdef), u.ublessed); break;
		case 624: pline("%s laughs about the fact that you need an AC of %d for this fight, and dares you to remove some of your armor.", Monnam(mdef), u.uac); break;
		case 625: pline("%s scratches very beautiful wounds on your legs with a pair of cone-heeled lady pumps, increasing your leg damage amount to %d.", Monnam(mdef), u.legscratching); break;
		case 626: pline("Maybe it's time to use the #youpoly command, for which you still have %d uses left, because you obviously don't have enough to-hit in your current form.", u.youpolyamount); break;
		case 627: pline("You know that the chance for traps to be randomized is %d, and are suddenly very afraid that %s has a trap underneath, so you hold back on your intended attack.", u.traprandomizing, mon_nam(mdef)); break;
		case 628: pline("You fumble for %d more turns, and therefore %s evades your thrust with ease.", u.fumbleduration, mon_nam(mdef)); break;
		case 629: pline("You're trembling with a severity of %d! It was to be expected that %s would evade your inaccurate melee swing!", u.tremblingamount, mon_nam(mdef)); break;
		case 630: pline("Every time you miss %s, %s attack will get stronger. Currently, it already does %d points worth of extra damage, and if you miss again, it'll increase by one yet again.", mon_nam(mdef), mhis(mdef), u.chokhmahdamage); break;
		case 631: pline("You're moving in slow motion. This is likely a result of your %d remaining turns of inertia, and therefore %s sees exactly where your sword would strike, allowing %s to easily parry the attack.", u.inertia, mon_nam(mdef), mhim(mdef)); break;
		case 632: pline("%s laughs: 'Want to get your %d zorkmids back from the bank? Ha ha ha. Dream on.'", Monnam(mdef), u.bankcashamount); break;
		case 633: pline("All your %ld experience points are for naught, because you still miss the broad side of a barn.", u.uexp); break;
		case 634: pline("You're such a scrub, why don't you actually use the %d skill slots you have available. Hint: Type #enhance to spend them! Then maybe you'll get somewhere!", u.weapon_slots); break;
		case 635: pline("Even after %ld turns, you still haven't enhanced more than %d skills. Are you REALLY surprised that you miss more often than you hit?", moves, u.skills_advanced); break;
		case 636: pline("Maybe you should lower your sanity, since an amount of %d doesn't let you see %s for what it really is. Your mind causes you to think you're fighting a weak monster, which isn't actually the case!", u.usanity, mon_nam(mdef)); break;
		case 637: pline("You've used your quest artifact for only %d turns! That's way too little, you should probably re-equip it and then maybe %s wouldn't be completely impervious to your attacks!", u.artifactaffinity, mon_nam(mdef)); break;
		case 638: pline("%s calls for a timeout, stating that %s has to go to the toilet, and you're actually stupid enough to cease attacking.", Monnam(mdef), mhe(mdef)); break;
		case 639: pline("%s decides to dam your cathedral, and there is nothing you can do about it.", Monnam(mdef)); break;
		case 640: pline("%s starts singing an Abba song. Terrified, you put your %s into your ears.", Monnam(mdef), makeplural(body_part(FINGER))); break;
		case 641: pline("Your wife will not be pleased to hear that you're losing to %s!", mon_nam(mdef)); break;
		case 642: pline("Hmm, you called the police 15 minutes ago in an attempt to get %s arrested, you're beginning to wonder whether they will ever come...", mon_nam(mdef)); break;
		case 643: pline("It turns out that the policemen you called to get %s arrested are useless, since they're just standing there instead of doing anything.", mon_nam(mdef)); break;
		case 644: pline("%s douses you with water!", Monnam(mdef)); break;
		case 645: pline("Your game lags, and therefore skips right past the attack command you gave, deciding to execute a wait command instead."); break;
		case 646: pline("%s messes up your humeroradial joint, making you incapable of swinging that weapon.", Monnam(mdef)); break;
		case 647: pline("Is that 'tactic' of yours some kind of joke? Just punch everything that moves or looks like it might move? Pathetic. Just pathetic."); break;
		case 648: pline("You must be really desperate, resorting to punching a monster that's clearly meant to be fought with ranged attacks."); break;
		case 649: pline("%s is impervious to melee attacks and you'd better wise up and start using some offensive wands or spells!", Monnam(mdef)); break;
		case 650: pline("You're playing like a complete noob! A real player would be smart enough to cast a spell to get rid of %s, but obviously you're not such a player!", mon_nam(mdef)); break;
		case 651: pline("%s crashes into you, and you're too shaken to continue fighting.", Monnam(mdef)); break;
		case 652: pline("You're out of water and therefore cannot fire your FLUDD at %s!", mon_nam(mdef)); break;
		case 653: pline("%s's armament repelled the impact.", Monnam(mdef)); break;
		case 654: pline("Oh no, %s stepped on your blue suede shoes!", mon_nam(mdef)); break;
		case 655: pline("%s is the dancing queen, and easily evades every attack you throw at her!", Monnam(mdef)); break;
		case 656: pline("%s reminds you that you're in arrears with your tax bills for 4 months, and if you don't pay up soon, the cops will be after you!", Monnam(mdef)); break;
		case 657: pline("%s makes you come to a stop and demands that you pay 700 zorkmids as a fine for exceeding the permissible maximum speed with your car.", Monnam(mdef)); break;
		case 658: pline("You curse at the fact that the last hitpoint is always the hardest to remove, because that bullshit feature is causing you to miss %s way more often than you should!", mon_nam(mdef)); break;
		case 659: pline("%s shouts '%ss out!'", Monnam(mdef), urace.noun); break;
		case 660: pline("%s screams some racist bullshit at you.", Monnam(mdef)); break;
		case 661: pline("%s tells you that your race sucks because it gets so few techniques upon leveling up.", Monnam(mdef)); break;
		case 662: pline("%s laughs: 'You're playing a %s? And you expect to be able to hit me? lol bro'", Monnam(mdef), urole.name.m); break;
		case 663: pline("%s suddenly screams like a woman, and you drop your weapon in shock.", Monnam(mdef)); break;
		case 664: pline("%s tells you that the last line was sexist, and you're busy pondering the meaning of that.", Monnam(mdef)); break;
		case 665: pline("Suddenly a mosquito starts buzzing right next to your ear, and you're so scared that you completely forget what you were doing."); break;
		case 666: pline("%s simply pushes you away.", Monnam(mdef)); break;
		case 667: pline("%s blocks with %s own weapon.", Monnam(mdef), mhis(mdef)); break;
		case 668: pline("%s is too small and therefore your clumsy attack missed!", Monnam(mdef)); break;
		case 669: pline("%s just slightly changes position and that was enough to make your attack hit thin air!", Monnam(mdef)); break;
		case 670: pline("You rolled a 1 with your d20 and so your strike automatically misses."); break;
		case 671: pline("You need more coffee! You're too tired and therefore cannot concentrate!"); break;
		case 672: pline("It seems that you forgot to take your accuracy pills today!"); break;
		case 673: pline("You're so high on drugs right now that you forgot how to use a weapon!"); break;
		case 674: pline("You missed, and thereby blew your last chance to ever defeat %s.", mon_nam(mdef)); break;
		case 675: pline("You miss so often, you won't be able to overcome %s's HP regeneration rate if you continue like this!", mon_nam(mdef)); break;
		case 676: pline("%s body-shames you for being such a fatass.", Monnam(mdef)); break;
		case 677: pline("You miss %s despite your high weapon skill because you forgot to enhance your secondary combat skills!", mon_nam(mdef)); break;
		case 678: pline("Why don't you noob try to enchant your weapon, you should have realized that without enchantment it just whiffs every time!"); break;
		case 679: pline("Your gun is out of ammo, so you can't shoot this turn and need to reload first."); break;
		case 680: pline("Suddenly you realize that the monster you're fighting is one that crashes the game as soon as its name is displayed, so you decide to not attack after all."); break;
		case 681: pline("%s tries to display a panic message, but it has a bugged string and therefore your game crashes.", Monnam(mdef)); break;
		case 682: pline("Oh no, due to a bug that has been fixed thrice already, %s has much more AC than intended! How many times does the incapable dev have to fix this bug before it's finally gone for good?", mon_nam(mdef)); break;
		case 683: pline("Yeah, maybe you should relearn the basic lesson of 'How much to-hit do I need relative to the amount of AC a monster has'?"); break;
		case 684: pline("You're playing like the last noob, using a weapon that %s is highly resistant to. Switch to another one or you won't have no chance.", mon_nam(mdef)); break;
		case 685: pline("Maybe you shouldn't have slept through the basic combat lessons."); break;
		case 686: pline("%s's lightsaber stops burning, and your game crashes. You have no idea why that happened, even with the backtrace. Maybe you should ask Amy why that happens, she might be able to fix it...", Monnam(mdef)); break;
		case 687: pline("%s uses bullet time and easily dodges your machine gun rounds!", Monnam(mdef)); break;
		case 688: pline("Klililililing! %s's phone rings, and you are nice enough to stop attacking so %s can pick up the call.", Monnam(mdef), mhe(mdef)); break;
		case 689: pline("You are playing with quaractar rules, and rolled a 1 which means you automatically miss."); break;
		case 690: pline("You won't defeat %s anyway, so you might as well not even try.", mon_nam(mdef)); break;
		case 691: pline("%s invokes some special rule that allows %s to dodge what would otherwise have been a guaranteed hit.", Monnam(mdef), mhim(mdef)); break;
		case 692: pline("You are using a crappy short sword, which constantly misses %s!", mon_nam(mdef)); break;
		case 693: pline("Your weapon is cursed and therefore usually misses %s.", mon_nam(mdef)); break;
		case 694: pline("You're losing to the small penis of %s because you're playing like a complete retard.", mon_nam(mdef)); break;
		case 695: pline("Are you trying to get your butt kicked by the gay fag of %s on purpose?", mon_nam(mdef)); break;
		case 696: pline("%s stays out of reach of your small weapon!", Monnam(mdef)); break;
		case 697: pline("%s thinks that %s is a stupid name.", Monnam(mdef), playeraliasname); break;
		case 698: pline("If you can't handle even the easy %s, how are you supposed to deal with the stronger monsters that will come later?", l_monnam(mdef)); break;
		case 699: pline("You are using a heavy weapon, which strikes so slowly that %s can easily interrupt your attack cycle!", mon_nam(mdef)); break;
		case 700: pline("%s's block-heeled combat boots look so pretty that your sputa will flow down.", Monnam(mdef)); break;
		case 701: pline("%s suddenly pulls out a tampon, and the mere thought of seeing menstruational liquid makes you back off and run.", Monnam(mdef)); break;
		case 702: pline("You were careless and underestimated %s's cuddle-soft female sneakers, and ended up being kicked powerfully by them.", mon_nam(mdef)); break;
		case 703: pline("%s simply pees at your belly.", Monnam(mdef)); break;
		case 704: pline("You wait for %s to make a mistake, but it doesn't seem to be happening...", mon_nam(mdef)); break;
		case 705: pline("%s is using some gay juju magic, which deflects your melee attacks!", Monnam(mdef)); break;
		case 706: pline("Suddenly %s says something to you in a language you don't understand, and you ponder the meaning of that instead of fighting.", mon_nam(mdef)); break;
		case 707: pline("%s simply disables the collision detection that checks whether your weapon hits %s, and you cannot hit %s at all now!", Monnam(mdef), mhim(mdef), mhim(mdef)); break;
		case 708: pline("%s uses the madeleine strategy, and keeps narrowly avoiding your attacks!", Monnam(mdef)); break;
		case 709: pline("Graeaeaeaeb! You stepped into the s-pressing trap that has been laid by %s!", mon_nam(mdef)); break;
		case 710: pline("You fire a salvo of SMG ammo at %s, but %s just blocks all the bullets with %s lightsaber!", mon_nam(mdef), mhe(mdef), mhis(mdef)); break;
		case 711: pline("You hit %s with a superpowered attack that should have killed %s outright, but %s has the annoying property to occasionally nullify incoming damage!", mon_nam(mdef), mhim(mdef), mhe(mdef)); break;
		case 712: pline("It seems that %s does not respect you!", mon_nam(mdef)); break;
		case 713: pline("You are stupid enough to cast a spell of an element that %s is immune to, and end up dealing no damage!", mon_nam(mdef)); break;
		case 714: {
			strcpy(buf, mon_nam(mdef));

			pline("%s suddenly offers some food to you which you happily eat, but then you become infatuated with %s because %s apparently spiked the comestible with an aphrodisiac...", Monnam(mdef), buf, mhe(mdef)); break;
		}
		case 715: pline("%s fell into the lava, but simply jumps back out unharmed!", Monnam(mdef)); break;
		case 716: pline("You use your mundane iron weapon against %s, which does nothing because %s can't be harmed by non-silver weapons.", mon_nam(mdef), mhe(mdef)); break;
		case 717: pline("%s hacks into the game code and reprograms it such that the player's melee attacks automatically miss every time.", Monnam(mdef)); break;
		case 718: pline("You do not hit your enemy."); break;
		case 719: pline("You miss."); break;
		case 720: pline("You miss again."); break;
		case 721: pline("You fail to penetrate the enemy's armor."); break;
		case 722: pline("You might want to switch to a different weapon."); break;
		case 723: pline("You keep missing because you're probably not playing very well."); break;
		case 724: pline("You should use some better strategy."); break;
		case 725: pline("You play like a complete noob."); break;
		case 726: pline("You wasted a turn because your attack didn't hit."); break;
		case 727: pline("Your to-hit is so low that you automatically miss. Use some other action or you won't win."); break;
		case 728: pline("Listen, if you keep plugging away like a complete dorf, you'll never hit but the monster will eventually beat the crap out of you."); break;
		case 729: pline("Timewaster! Play better!"); break;
		case 730: pline("You tell %s to jump off a bridge, and %s actually does so but since %s can fly, %s simply comes back up and keeps attacking you.", mon_nam(mdef), mhe(mdef), mhe(mdef), mhe(mdef)); break;
		case 731: pline("Suddenly your phone makes a sound that tells you about the battery being low, so you have to stop fighting %s and find a way to recharge the device.", mon_nam(mdef)); break;
		case 732: pline("%s says some pseudogerman word, and you scratch your %s.", Monnam(mdef), body_part(HEAD)); break;
		case 733: pline("%s says 'Look over there, it's a fire!' and you are actually stupid enough to fall for it.", Monnam(mdef)); break;
		case 734: pline("Did you know that with your shitty %d dexterity, you miss all the time?", ACURR(A_DEX)); break;
		case 735: pline("It's fairly obvious that you're a fool with only %d wisdom.", ACURR(A_WIS)); break;
		case 736: pline("You are a stupid person with a pitiful %d intelligence, which explains why you are using the worst weapon imaginable that can never get past %s's AC!", ACURR(A_INT), mon_nam(mdef)); break;
		case 737: pline("Since you're not very good at this game, you decide to throw a glass weapon at %s. Not only does it result in the weapon breaking to pieces, you didn't even hit in the first place, so the item was wasted!", mon_nam(mdef)); break;
		case 738: pline("%s's thick butt cheeks cushion the impact of your weapon, meaning you ended up dealing no damage.", Monnam(mdef)); break;
		case 739: pline("You fail to get past %s's thick belly with your useless weapon!", mon_nam(mdef)); break;
		case 740: pline("%s is thick-skinned, and therefore takes no damage from your flimsy weapon!", Monnam(mdef)); break;
		case 741: pline("%s can only be harmed by Donkey Kong, and because you're not Donkey Kong, you are completely unable to deal any damage to %s!", Monnam(mdef), mhim(mdef)); break;
		case 742: pline("%s will imprison you in Sing's torture chamber if you keep fighting that badly!", Monnam(mdef)); break;
		case 743: pline("%s cannot be snatched by you!", Monnam(mdef)); break;
		case 744: pline("%s narrowly manages to escape your grabbing attempt.", Monnam(mdef)); break;
		case 745: pline("%s swerves out of your reach.", Monnam(mdef)); break;
		case 746: pline("%s teases you by floating to the ceiling where you cannot reach it.", Monnam(mdef)); break;
		case 747: pline("%s scratches over your %s as you try to grab it.", Monnam(mdef), body_part(HAND)); break;
		case 748: pline("Even though %s is laying prone, %s keeps fighting back and hits you with a painful kick!", mon_nam(mdef), mhe(mdef)); break;
		case 749: pline("Your thick block heel boots stomp %s, but since the heels are so incredibly soft, they cannot actually hurt anyone.", mon_nam(mdef)); break;
		case 750: pline("%s is wearing %s, which look so hot that you just can't bring yourself to attack.", Monnam(mdef), pantsdescription(mdef)); break;
		case 751: pline("%s's %s pad your kick very well!", Monnam(mdef), pantsdescription(mdef)); break;
		case 752: pline("%s steps on your %s.", Monnam(mdef), body_part(HAND)); break;
		case 753: pline("Your punch hits %s's block heel, which has no effect except that you hurt yourself.", mon_nam(mdef)); break;
		case 754: pline("You attempt to rip %s's leather to shreds even though it's so soft, but thankfully you fail.", mon_nam(mdef)); break;
		case 755: pline("%s swerves out of the way of your grasp!", Monnam(mdef)); break;
		case 756: pline("You fail to snatch %s's buckles!", mon_nam(mdef)); break;
		case 757: pline("Since you're not a node master, you fail to untie %s's lacings!", mon_nam(mdef)); break;
		case 758: pline("%s's zipper constantly slides along your %s, and your attempts to pull it open aren't very fruitful.", Monnam(mdef), makeplural(body_part(HAND)) ); break;
		case 759: pline("Since you're unable to fight properly, you just stand there while %s's velcro lash constantly scratches up and down your unprotected %s, and the %s is squirting in all directions.", mon_nam(mdef), body_part(LEG), body_part(BLOOD)); break;
		case 760: pline("You're losing hope because you've been fighting %s for such a long time and still haven't made even the slightest bit of progress!", mon_nam(mdef)); break;
		case 761: pline("Since you're exhausted from all of your previous failed attack attempts, you decide to just stand there and allow %s to continuously attack you, which is obviously not the best strategy.", mon_nam(mdef)); break;
		case 762: pline("Should read Sun Tzu, player. Greater enemy - retreat. Smaller enemy - attack. And most importantly, you should realize that %s is the former type!", mon_nam(mdef)); break;
		case 763: pline("That was very close, but it wasn't a hit."); break;
		case 764: pline("You keep barely missing %s.", mon_nam(mdef)); break;
		case 765: pline("Your attack goes in the completely wrong direction."); break;
		case 766: pline("You fully hit %s, but somehow %s takes no damage at all!", mon_nam(mdef), mhe(mdef)); break;
		case 767: pline("Maybe you should use some proper weapon, not that wimpy water squirting gun you're currently using."); break;
		case 768: pline("You're obviously not using all you have, or you'd be capable of taking down %s.", mon_nam(mdef)); break;
		case 769: pline("Why aren't you using a weapon that can damage %s?", mon_nam(mdef)); break;
		case 770: pline("Why are you apparently too bad to play this game?"); break;
		case 771: pline("%s makes fun of the fact that you're still under the delusion that there was a Junethack tournament.", Monnam(mdef)); break;
		case 772: pline("%s tells you that Junethack is dead and you can stop trying to play as if you were in a tournament, because no one is watching anyway.", Monnam(mdef)); break;
		case 773: pline("%s laughs about the huge amount of time you're wasting playing stupid video games.", Monnam(mdef)); break;
		case 774: pline("%s tells you that all of your efforts will be in vain.", Monnam(mdef)); break;
		case 775: pline("%s reminds you about your last lecture, which told you that at the end of the day, none of your achievements will be of any value.", Monnam(mdef)); break;
		case 776: pline("%s tells you that one of the real-life presidents will eventually push the big red button, and then the world will end, along with the game NetHack.", Monnam(mdef)); break;
		case 777: pline("%s says in a calm voice, 'Stop your combat activities. Nothing that you do still matters at this point. Haven't you listened to the oracle? She said... Tomorrow, %s is going to die.'", Monnam(mdef), playeraliasname); break;
		case 778: pline("%s knows that you suck not only at the game, but at real life too.", Monnam(mdef)); break;
		case 779: pline("%s snickers 'Don't you have a job? Got all day to waste living off social welfare and playing stupid video games?'", Monnam(mdef)); break;
		case 780: pline("%s is the server administrator and decides that your game gets banned from the annual NetHack tournament for no reason, other than the admin wanting to demonstrate his power, of which he has too much!", Monnam(mdef)); break;
		case 781: pline("Your character plays exactly like a samurai... who didn't enhance the melee weapon skills yet, and therefore your attack misses."); break;
		case 782: pline("You are trying to play your character like it was a samurai, but it actually isn't one, and so your melee attack misses!"); break;
		case 783: pline("You are somehow unconcentrated and shoot your ball highly over the goal."); break;
		case 784: pline("You scream 'Bii-iarr!' Oh no, you missed."); break;
		case 785: pline("You stand there helplessly while %s scratches %s stiletto heels up and down your %sied shins constantly.", mon_nam(mdef), mhis(mdef), body_part(BLOOD)); break;
		case 786: pline("You start to get really annoyed by %s who simply doesn't seem to get hit by you.", mon_nam(mdef)); break;
		case 787: pline("You have no chance against %s's sexy leather pumps with cone heels.", mon_nam(mdef)); break;
		case 788: pline("%s's wedge heel deflects your blow.", Monnam(mdef)); break;
		case 789: pline("%s kicks your weapon away by using %s platform soles.", Monnam(mdef), mhis(mdef)); break;
		case 790: pline("You draw back, but then somehow your conscience bothers you and you end up not attacking after all."); break;
		case 791: pline("Your morale is so low, you simply decide to play the pacifist conduct from now on and therefore refuse to attack the enemy."); break;
		case 792: pline("Afraid that you might accidentally kill %s, you sheathe your weapon.", mon_nam(mdef)); break;
		case 793: pline("Oh, better don't attack %s, who has a deadly passive attack as you remembered just in time.", mon_nam(mdef)); break;
		case 794: pline("Idiot! Didn't you realize that %s was peaceful to you? If you were to attack now, that 'peaceful' status would change to 'hostile'! Be glad that the game prevented you from executing the attack!", mon_nam(mdef)); break;
		case 795: pline("The game reminds you that %s is actually your pet, and therefore you cancel your attack.", mon_nam(mdef)); break;
		case 796: pline("Terrified by the fact that violence seems to be the only option in this dungeon, you decide to be nice to %s, so you run up to %s, hug %s, and start to heart and kiss %s.", mon_nam(mdef), mhim(mdef), mhim(mdef), mhim(mdef)); break;
		case 797: pline("You realize that the reason why %s is ripping off %sy shreds of skin from you isn't hate, but love, and decide to return that love by kissing %s %s.", mon_nam(mdef), body_part(BLOOD), mhis(mdef), mbodypart(mdef, FACE)); break;
		case 798: pline("%s steps on your %s, and when you try to fight back with your razor-sharp %snails, you find out to your dismay that you cannot move them at all!", Monnam(mdef), body_part(FOOT), body_part(TOE)); break;
		case 799: pline("You try repeatedly to bite %s's ugly skin but the little bastard constantly deflects your attacks!", mon_nam(mdef)); break;
		case 800: pline("Suddenly %s calls for a particularly strict teacher woman, and she says that if you don't stop fighting, you will get a stern talking to!", mon_nam(mdef)); break;
		case 801: pline("You little victim, sucks to be constantly bullied by %s, eh?", mon_nam(mdef)); break;
		case 802: pline("%s shouts 'Ey wolloh fuck you!'", Monnam(mdef)); break;
		case 803: pline("%s laughs 'Ha-ha, did the little %s shit %s pants? Ha, ha, ha!'", Monnam(mdef), playeraliasname, flags.female ? "her" : "his"); break;
		case 804: pline("%s tries to threaten you with a knife, and since you don't have a weapon, you decide to turn tail and run off.", Monnam(mdef)); break;
		case 805: pline("%s laughs 'Ha ha ha, little baby gonna run to mommy?'", Monnam(mdef)); break;
		case 806: pline("%s steals your pencil sharpener, and you try to throw a flower pot at %s in revenge but don't hit. Now you need to replace the broken pot, which will cost you 60 euros.", Monnam(mdef), mhim(mdef)); break;
		case 807: pline("%s threatens to tell your boss about your bad behavior at work if you don't stop attacking immediately.", Monnam(mdef)); break;
		case 808: pline("%s calls you a tranny, and your confidence melts away completely.", Monnam(mdef)); break;
		case 809: pline("%s laughs at you and mocks your style of clothing.", Monnam(mdef)); break;
		case 810: pline("%s predicted your strike and avoids the hit!", Monnam(mdef)); break;
		case 811: pline("%s simply catches your grenade and throws it far away!", Monnam(mdef)); break;
		case 812: pline("All this time, you were hoping to marry %s one day but now %s tells you that you'll only be a friend at best, and the revelation causes you to burst into tears.", mon_nam(mdef), mhe(mdef)); break;
		case 813: pline("%s shouts 'Pissing carnation!'", Monnam(mdef)); break;
		case 814: pline("Since you set the game difficulty to the highest value by mistake, you cannot stop %s at all.", mon_nam(mdef)); break;
		case 815: pline("You try to fully foul %s, but somehow it has no effect at all!", mon_nam(mdef)); break;
		case 816: pline("You try to fully foul %s, but somehow you are the one who drops to the floor, and now you have to get up again, quick!", mon_nam(mdef)); break;
		case 817: pline("As you rush to attack %s, you trip over a stone and lose balance, dropping to the ground helplessly.", mon_nam(mdef)); break;
		case 818: pline("Even our narrator is getting bored of having to constantly come up with combat messages for all the different ways of you missing %s, so now we just say: 'You missed %s!'", mon_nam(mdef), mon_nam(mdef)); break;
		case 819: pline("You're using fancy attack patterns, yet they're not going to be of much use if you keep missing %s...", mon_nam(mdef)); break;
		case 820: pline("%s shouts 'You got shit on your %s!'", Monnam(mdef), body_part(HAND)); break;
		case 821: pline("%s laughs: 'Ah, you must be poor, being able to only afford a lowly %s! Pity!'", Monnam(mdef), playerweaponname()); break;
		case 822: pline("With %s flaming %s and razor-sharp hooves, %s seems invincible, and you surrender to fear and run off.", mhis(mdef), makeplural(mbodypart(mdef, EYE)), mon_nam(mdef)); break;
		case 823: pline("%s's breath reeks of sulfur, and since you can't stand that stench, you decide to run rather than fight.", Monnam(mdef)); break;
		case 824: pline("%s is certainly not scared by your wimpy %s!", Monnam(mdef), playerweaponname()); break;
		case 825: pline("%s knows that you'll never land a hit with that %s because you're just such a wimp.", Monnam(mdef), playerweaponname()); break;
		case 826: pline("You get the fucked-ass corpse bug: because %s is standing on a corpse and the programmers are REALLY stupid, it registered as you striking the corpse instead of the alive monster, so your attack did no damage but still reduced your weapon's durability.", mon_nam(mdef)); break;
		case 827: pline("You try to fire your gun straight at %s's %s but the aimbot, which cannot even be turned off because of the stupid programmer morons, thinks you are aiming for a different monster standing five feet away. The result is that the bullet doesn't hit anything at all.", mon_nam(mdef), mbodypart(mdef, FACE)); break;
		case 828: {
			strcpy(buf, mon_nam(mdef));

			pline("You try to fully hammer the annoying %s on the %s but unfortunately you're standing in waist-high water. As you draw back to attack, suddenly your body is just a tad too deep in the water and the STUPID programming aborts your attack entirely, while %s keeps fighting without having to face the same problems.", l_monnam(mdef), mbodypart(mdef, HEAD), buf); break;
		}
		case 829: pline("You furiously try to smash %s's stupid %s in, but %s last hit knocked you back just far enough that your swing passes harmlessly through the air in front of %s.", mon_nam(mdef), mbodypart(mdef, FACE), mhis(mdef), mhim(mdef)); break;
		case 830: pline("You want to rip apart every little bit of skin from the asshole that is %s, but you just don't seem to have the strength to land an effective hit!", mon_nam(mdef)); break;
		case 831: pline("Even though you absolutely want to kill %s and devour %s corpse, you seem unable to hit %s at all!", mon_nam(mdef), mhis(mdef), mhim(mdef)); break;
		case 832: pline("%s is so unfair and blocks your attack, making you wish that blocking was banned from this game.", Monnam(mdef)); break;
		case 833: pline("%s uses hit and run tactics to pepper you with arrows while you cannot get into melee attack range, and when you complain to the game master about it, he tells you to suck it up and learn how to play the game properly.", Monnam(mdef)); break;
		case 834: pline("A mysterious force causes you to put your weapon away, and you can't re-equip it either."); break;
		case 835: pline("You're losing all of your lines because of %s!", mon_nam(mdef)); break;
		case 836: pline("Due to your strategy of kiting %s, you're not taking any hits, but in this case you didn't land one either.", mon_nam(mdef)); break;
		case 837: pline("Since you're a lamer who has abused various game-breaking exploits in the past, you automiss %s now. Serves you right.", mon_nam(mdef)); break;
		case 838: pline("Until you tell Amy about your lame autowin exploit that involves abusing a loophole in the game which is certainly not meant to be there, you'll always miss %s no matter what, so better confess your sins to the developer so she can patch out the bad bug.", mon_nam(mdef)); break;
		case 839: pline("With your fatman, you fire a mini-nuke at %s... but since you're such a horribly bad shot, you miss, meaning you've just wasted your super precious ammo for nothing at all.", mon_nam(mdef)); break;

		default: pline("You missed %s!", mon_nam(mdef)); break;

		}

	} else if(canspotmon(mdef) && flags.verbose) {
		if (nearmiss || !blocker) {
		    You("%smiss %s%s.", (nearmiss ? "just " : ""),mon_nam(mdef), (totalmiss ? (!rn2(6) ? " completely" : !rn2(5) ? " by a wide margin" : !rn2(4) ? " totally" : !rn2(3) ? " hopelessly" : rn2(2) ? " by more than 20 points" : " by a league") : ""));
			/*pline("%d",(target - roll) );*/
		} else {
        	    /* Blocker */
        	    pline("%s %s %s your attack.", 
        		s_suffix(Monnam(mdef)),
        		aobjnam(blocker, (char *)0),
        		(rn2(2) ? "blocks" : "deflects"));    
		}
	} else {
		You("%smiss it%s.", ((flags.verbose && nearmiss) ? "just " : ""), (totalmiss ? (!rn2(6) ? " completely" : !rn2(5) ? " by a wide margin" : !rn2(4) ? " totally" : !rn2(3) ? " hopelessly" : rn2(2) ? " by more than 20 points" : " by a league") : "") );
	}
	if (!mdef->msleeping && mdef->mcanmove)
		wakeup(mdef);
}

/*
 * [WAC] This code now handles twoweapon in the following way:
 *	-monster with one or 2 AT_WEAP can get primary and secondary attacks
 *	-monster with AT_WEAP and another hand attack(s) will lose one
 *		of the other hand attacks
 *	-monster with several hand attacks will do primary weapon and secondary
 *		weapon attacks with the first 2 hand attacks,  then claw attacks
 *		for the rest
 *	-Code assumes that a single AT_WEAP or 2 AT_WEAP entries still mean 
 *		at most 1 or 2 hands attack.  i.e. 1 handed monsters with AT_WEAP
 *		or monsters with 3+ handed monsters where more than 2 hands are
 *		AT_WEAP are not handled properly
 *		(I don't think any exist yet)
 * This code now handles ALL hand to hand whether you are poly'ed or not
 * (uses your current race as the monster type)
 *
 * [ALI] Returns TRUE if you hit (and maybe killed) the monster.
 */
STATIC_OVL boolean
hmonas(mon, tmp)		/* attack monster as a monster. */
register struct monst *mon;
register int tmp;
{
	struct attack *mattk, alt_attk;
	int	i, sum[NATTK];

	struct permonst *mdat2;
      struct attack *carthageattk;

#if 0
	int	hittmp = 0;
#endif
	int	nsum = 0;
	int	dhit = 0;
	int 	mhit = 0; /* Used to pass the attacks used */
	int 	tmp1, tmp2;
	int dieroll = rnd(20);
	boolean Old_Upolyd = Upolyd;
	static const int hit_touch[] = {0, HIT_BODY, HIT_BODY|HIT_FATAL};
	static const int hit_notouch[] = {0, HIT_OTHER, HIT_OTHER|HIT_FATAL};

	boolean willsymattack = FALSE;
	if (symbiotemelee()) willsymattack = TRUE;
	boolean symbioteprocess = FALSE;
	boolean symbiotedouble = FALSE;
	boolean symbiotenomore = FALSE;
	boolean carthageattack = FALSE;
	if (Race_if(PM_CARTHAGE) && u.usteed) carthageattack = TRUE;
	boolean carthageprocess = FALSE;

	/* don't give the extra weapon attacks every time if your natural form has more than two --Amy */
	int weaponiteration = 0;
	int weaponamount = 1;
	if (rnd(5) > 3) {
		weaponamount++;
		if (rnd(5) > 3) weaponamount++;
	}

	/* Keeps track of which weapon hands have been used */
	boolean used_uwep = FALSE;

symbiotejump:
carthagejump:
	for(i = 0; i < NATTK; i++) {
	    mhit = 0; /* Clear all previous attacks */

	    sum[i] = 0;

	    /* failsafe if your symbiote dies while doing attacks --Amy */
	    if (symbioteprocess && !symbiotenomore && !uactivesymbiosis) continue;
	    if (carthageprocess && !(Race_if(PM_CARTHAGE) && u.usteed)) continue;

	    mattk = getmattk(carthageprocess ? &mons[PM_CARTHAGE_DUMMY] : (symbioteprocess && !symbiotenomore) ? &mons[u.usymbiote.mnum] : youmonst.data, i, sum, &alt_attk);

	    switch(mattk->aatyp) {
		case AT_WEAP:

	if (!rn2(8) && Role_if(PM_BULLY) ) { steal_it(mon, mattk); }

	if (!rn2(8) && Role_if(PM_PICKPOCKET) ) {

		if (mon->mgold) {
		    u.ugold += mon->mgold;
		    mon->mgold = 0;
		    Your("purse feels heavier.");
		}
	}

use_weapon:	

	/* Certain monsters don't use weapons when encountered as enemies,
	 * but players who polymorph into them have hands or claws and thus
	 * should be able to use weapons.  This shouldn't prohibit the use
	 * of most special abilities, either.
	 */
	/* Potential problem: if the monster gets multiple weapon attacks,
	 * we currently allow the player to get each of these as a weapon
	 * attack.  Is this really desirable?
	 * [WAC] See Above ...  anyways,  this was changed in 3.3.0 so that
	 * only attack 0 would give a weapon attack...
	 * [ALI] Most monsters should get multiple weapon attacks since they
	 * only have two hands. There are exceptions such as mariliths which
	 * should get two weapon attacks and four barehanded attacks. Such
	 * monsters should be special cased in AT_CLAW below.
	 */
			mhit = used_uwep ? HIT_USWAPWEP : HIT_UWEP;
			used_uwep = !used_uwep;

			/* clockworks, haxors etc. - 60% chance of one attack, 24% two attacks, 16% three attacks --Amy */
			weaponiteration++;
			if (!Upolyd && weaponiteration > 2 && weaponamount < 2) continue;
			if (!Upolyd && weaponiteration > 4 && weaponamount < 3) continue;

			if (mhit == HIT_USWAPWEP && !u.twoweap)
			    continue;	/* Skip this attack */

			/* WAC if attacking cockatrice/etc, player is smart
			   if wielding a weapon.  So don't let him
			   touch the monster */

			/* Amy edit: but if you're immune to petrification, it's okay */

			if ((uwep || u.twoweap && uswapwep) &&
				(mhit == HIT_UWEP && !uwep ||
				 mhit == HIT_USWAPWEP && !uswapwep) && !Stone_resistance &&
				(touch_petrifies(mon->data) ||
				 mon->data == &mons[PM_MEDUSA]))
			    break;

			dhit = mhit; /* Clear the miss counter as attacks miss */
			tmp1 = tmp2 = tmp;

#ifdef DEBUG
			pline("%i/20", tmp);
#endif

			if (mhit & HIT_UWEP) {
			    if (uwep) tmp1 = tmp + hitval(uwep, mon);
			    tohit(UWEP_ROLL) = tmp1;
			    if (tmp1 <= (dice(UWEP_ROLL) = rnd(20)) &&
				    (!u.uswallow || !rn2(3)))
				dhit &= ~HIT_UWEP; /* missed */
				
			    if (tmp1 > dice(UWEP_ROLL)) exercise(A_DEX, TRUE);
#ifdef DEBUG
			    pline("(%i/20)", tmp1);
#endif
			}

			if (mhit & HIT_USWAPWEP) {
			    if (uswapwep)
				tmp2 = tmp + hitval(uswapwep, mon) - 2;

			    tohit(USWAPWEP_ROLL) = tmp2;

			    if (tmp2 <= (dice(USWAPWEP_ROLL) = rnd(20)) &&
				    (!u.uswallow || !rn2(3)))
				dhit &= ~HIT_USWAPWEP;

			    if (tmp2 > dice(USWAPWEP_ROLL))
				exercise(A_DEX, TRUE);
#ifdef DEBUG
			    pline("((%i/20))", tmp2);
#endif
			}

			if (dhit && mattk->adtyp == AD_SLEE)
			    barehanded_hit = (dhit & HIT_UWEP) && !uwep ||
			      (dhit & HIT_USWAPWEP) && !uswapwep;

#if 0 /* Old code */
			if (uwep) {
			    hittmp = hitval(uwep, mon);
			    hittmp += weapon_hit_bonus(uwep);
			    tmp += hittmp;
			}
			if (tmp > (dice(UWEP_ROLL) = rnd(20)) || (u.uswallow && rn2(3)))
			    dhit = HIT_UWEP;
			else dhit = 0;
			/* KMH -- Don't accumulate to-hit bonuses */
			if (uwep) tmp -= hittmp;
#endif

			if (dhit) {

				int savechance = 0;

				if (!(PlayerCannotUseSkills)) {
					switch (P_SKILL(P_GENERAL_COMBAT)) {

					    case P_BASIC:		savechance = 1; break;
					    case P_SKILLED:	savechance = 2; break;
					    case P_EXPERT:	savechance = 3; break;
					    case P_MASTER:	savechance = 4; break;
					    case P_GRAND_MASTER:savechance = 5; break;
					    case P_SUPREME_MASTER:savechance = 6; break;
					    default: savechance += 0; break;
					}
				}

				/* evil patch idea: if a weapon is used very often, it eventually degrades --Amy */
				if (uwep && weaponwilldull(uwep) && (rnd(7) > savechance) && !issoviet) {
					if (uwep->greased) {
						uwep->greased--;
						pline("Your weapon loses its grease.");
					} else {
						uwep->spe--;
						pline("Your weapon dulls.");
						u.cnd_weapondull++;
					}
				}

				if (uwep && uwep->oartifact == ART_ANNOYING_DOG_WHISTLE) {
					if (uwep->cursed && !rn2(2)) {
						You(FunnyHallu ? "produce a grating, annoying sound." : "produce a high-pitched humming noise.");
						if (PlayerHearsSoundEffects) pline(issoviet ? "Potomu chto vy ne mozhete igrat' der'mo." : "Dueueueueue!");
						wake_nearby();
					} else {
						register struct monst *wisselmon, *nextmon;
						You("produce a %s whistling sound.", FunnyHallu ? "normal" : "strange");
						if (PlayerHearsSoundEffects) pline(issoviet ? "Dazhe vy mozhete sdelat' chto-to pravil'no, v redkikh sluchayakh, eto kazhetsya." : "dueueueueue");
						for(wisselmon = fmon; wisselmon; wisselmon = nextmon) {
						    nextmon = wisselmon->nmon; /* trap might kill mon */
						    if (DEADMONSTER(wisselmon)) continue;
						    if (wisselmon->mtame) {
							if (wisselmon->mtrapped) {
							    /* no longer in previous trap (affects mintrap) */
							    wisselmon->mtrapped = 0;
							    fill_pit(wisselmon->mx, wisselmon->my);
							}
							mnexto(wisselmon);
							if (mintrap(wisselmon) == 2) change_luck(-1);
						    }
						}
					}
				}
				if (u.twoweap && uswapwep && uswapwep->oartifact == ART_ANNOYING_DOG_WHISTLE) {
					if (uwep->cursed && !rn2(2)) {
						You(FunnyHallu ? "produce a grating, annoying sound." : "produce a high-pitched humming noise.");
						if (PlayerHearsSoundEffects) pline(issoviet ? "Potomu chto vy ne mozhete igrat' der'mo." : "Dueueueueue!");
						wake_nearby();
					} else {
						register struct monst *wisselmon, *nextmon;
						You("produce a %s whistling sound.", FunnyHallu ? "normal" : "strange");
						if (PlayerHearsSoundEffects) pline(issoviet ? "Dazhe vy mozhete sdelat' chto-to pravil'no, v redkikh sluchayakh, eto kazhetsya." : "dueueueueue");
						for(wisselmon = fmon; wisselmon; wisselmon = nextmon) {
						    nextmon = wisselmon->nmon; /* trap might kill mon */
						    if (DEADMONSTER(wisselmon)) continue;
						    if (wisselmon->mtame) {
							if (wisselmon->mtrapped) {
							    /* no longer in previous trap (affects mintrap) */
							    wisselmon->mtrapped = 0;
							    fill_pit(wisselmon->mx, wisselmon->my);
							}
							mnexto(wisselmon);
							if (mintrap(wisselmon) == 2) change_luck(-1);
						    }
						}
					}
				}

				if (uwep && objects[uwep->otyp].oc_material == MT_COMPOST && uwep->spe < 0 && !rn2(500)) {
					uwep->spe++;
					pline("Your weapon repairs itself a bit!");
				}

				if (uwep && uwep->oartifact == ART_BEST_OFFENSE_IS_A_GOOD_DEF && !rn2(1000)) {
					if (uarm && uarm->spe < 7) {
						uarm->spe++;
						Your("armor seems harder.");
					}
				}

				if (uwep && uwep->oartifact == ART_ELVIN_S_PRESS && !u.twoweap && !rn2(100)) {
					TimeStopped += rnd(3);
					pline((Role_if(PM_SAMURAI) || Role_if(PM_NINJA)) ? "Jikan ga teishi shimashita." : "Time has stopped.");

				}

				if (uwep && uwep->oartifact == ART_GORMALER && !u.twoweap && u.gormalerturns >= 1000) {

					int parlyzdur = rnd(5);

					if (!rn2(3) && (rn2(100) > mon->data->mr) && !mon->msleeping && sleep_monst(mon, parlyzdur, -1)) {
					    if (!Blind)
						pline("%s is put to sleep by you!", Monnam(mon));
					    slept_monst(mon);
					}

				}

				if (uwep && uwep->oartifact == ART_SKOGLO && !rn2(200)) {

					pushplayerfar(FALSE, 100);
				}

				if (uwep && uwep->oartifact == ART_SHARPENING_SLAT && !rn2(100) && uwep->spe < 0) {
					uwep->spe++;
					pline("Your weapon repairs itself a bit!");
				}

				if (uwep && uwep->oartifact == ART_KRASCHDE_GLOMAN && !resist(mon, WEAPON_CLASS, 0, NOTELL)) {
					mon->healblock += 100;
				}

				if (uwep && uwep->oartifact == ART_GEB_ME_ALL_YOUR_MONEY && !resist(mon, WEAPON_CLASS, 0, NOTELL)) {
					monflee(mon, rnd(10), FALSE, FALSE);
					pline("%s looks scared.", Monnam(mon));
				}

				if (uwep && uwep->oartifact == ART_TERROR_DROP && !resist(mon, WEAPON_CLASS, 0, NOTELL)) {
					monflee(mon, 2, FALSE, FALSE);
					pline("%s looks scared.", Monnam(mon));
				}

				if (uwep && uwep->oartifact == ART_GAUGE_REDUCE) {
					mon->mspec_used += 2;
				}

				if (uwep && uwep->oartifact == ART_DESTRUCTION_BALL && !rn2(3) && uwep->spe > -20) {
					uwep->spe--;
					pline("Your ball sustains damage.");
				}
				if (uwep && uwep->oartifact == ART_DONNNNNNNNNNNNG && !rn2(3) && uwep->spe > -20) {
					uwep->spe--;
					pline("Your weapon sustains damage.");
				}
				if (uwep && uwep->oartifact == ART_DULL_METAL && !rn2(50) && uwep->spe > -20) {
					uwep->spe--;
					pline("Your weapon sustains damage.");
				}
				if (uwep && uwep->oartifact == ART_SKULL_SWORD && uwep->spe > -20) {
					uwep->spe--;
				}
				if (uwep && uwep->oartifact == ART_VIOLENT_SKULL_SWORD && uwep->spe > -20) {
					uwep->spe--;
				}
				if (uwep && uwep->otyp == SECRET_SOUND_WHIP) {
					increasesanity(1);
				}
				if (uwep && uwep->oartifact == ART_CHA_SHATTER && !rn2(3) && uwep->spe > -20) {
					uwep->spe--;
					pline("Your weapon sustains damage.");
				}
				if (uwep && uwep->oartifact == ART_SIGIX_BROADSWORD && !rn2(20)) {
					uwep->spe--;
					pline("Your broadsword sustains damage.");
					if (uwep->spe < -20) {
						useupall(uwep);
						pline("Your broadsword is destroyed.");
						return FALSE;
					}
				}
				if (uwep && uwep->oartifact == ART_NEED_ELITE_UPGRADE) {
					int eliteupgradechance = 100;
					if (uwep->spe > 1) eliteupgradechance = (uwep->spe * 100);
					if (uwep->spe < 12 && !rn2(eliteupgradechance)) {
						uwep->spe++;
						pline("Your weapon seems more effective.");
					}
				}

				if (uarmh && uarmh->oartifact == ART_DOUVONED) {
					lesshungry(5);
				}

				if (uwep && uwep->oartifact == ART_SCHWILLSCHWILLSCHWILLSCHWI && uwep->lamplit && (u.dx || u.dy) && !u.dz) {
					u.linkmasterswordhack = 1;
					struct obj *pseudo;
					pseudo = mksobj(SPE_BEAMSWORD, FALSE, 2, FALSE);
					if (!pseudo) goto bladeangerdone;
					if (pseudo->otyp == GOLD_PIECE) pseudo->otyp = SPE_BEAMSWORD; /* minimalist fix */
					pseudo->blessed = pseudo->cursed = 0;
					pseudo->quan = 20L;			/* do not let useup get it */
					pseudo->spe = uwep->spe;
					weffects(pseudo);
					if (pseudo) obfree(pseudo, (struct obj *)0);	/* now, get rid of it */
					if (!mon) return FALSE;
					if (DEADMONSTER(mon)) return FALSE;

				}
bladeangerdone:

				if (uarm && uarm->oartifact == ART_NOTHINGATALLBANE && (u.dx || u.dy) && !u.dz) {
					u.linkmasterswordhack = 1;
					struct obj *pseudo;
					pseudo = mksobj(SPE_BEAMSWORD, FALSE, 2, FALSE);
					if (!pseudo) goto bladeangerdone2;
					if (pseudo->otyp == GOLD_PIECE) pseudo->otyp = SPE_BEAMSWORD; /* minimalist fix */
					pseudo->blessed = pseudo->cursed = 0;
					pseudo->quan = 20L;			/* do not let useup get it */
					pseudo->spe = uwep->spe;
					weffects(pseudo);
					if (pseudo) obfree(pseudo, (struct obj *)0);	/* now, get rid of it */
					if (!mon) return FALSE;
					if (DEADMONSTER(mon)) return FALSE;

				}
bladeangerdone2:

				if (uwep && objects[uwep->otyp].oc_skill == (tech_inuse(T_GRAP_SWAP) ? P_LANCE : P_GRINDER)) {
					int grindirection = 0;
					if (u.dx > 0 && u.dy == 0) grindirection = 1; /* east */
					if (u.dx > 0 && u.dy > 0) grindirection = 2; /* southeast */
					if (u.dx == 0 && u.dy < 0) grindirection = 3; /* north */
					if (u.dx < 0 && u.dy < 0) grindirection = 4; /* northwest */
					if (u.dx < 0 && u.dy == 0) grindirection = 5; /* west */
					if (u.dx < 0 && u.dy > 0) grindirection = 6; /* southwest */
					if (u.dx == 0 && u.dy > 0) grindirection = 7; /* south */
					if (u.dx > 0 && u.dy < 0) grindirection = 8; /* northeast */
					if (grindirection >= 1 && grindirection <= 8) grinderattack(grindirection);
					if (!mon) return FALSE;
					if (DEADMONSTER(mon)) return FALSE;

				}

				if (uwep && ((objects[uwep->otyp].oc_skill == P_ORB) || (uwep->oartifact == ART_KHALIM_S_FEMUR)) ) {
					int suckingchance = 12;
					if (uwep && uwep->otyp == JARED_STONE) suckingchance = 11;
					if (uwep && uwep->otyp == DRAMA_ORB) suckingchance = 11;
					if (uwep && uwep->otyp == CIGARETTE) suckingchance = 11;
					if (uwep && uwep->otyp == ELECTRIC_CIGARETTE) suckingchance = 10;
					if (uwep && uwep->otyp == CIGAR) suckingchance = 10;
					if (uwep && uwep->otyp == LIGHTBULB) suckingchance = 10;
					if (uwep && uwep->otyp == HEATH_BALL) suckingchance = 9;
					if (uwep && uwep->otyp == HEAVENLY_BALL) suckingchance = 8;
					if (uwep && uwep->otyp == DIMENSIONAL_SHARD) suckingchance = 7;
					if (uwep && uwep->otyp == CIGARETTE && !rn2(250)) {
						You("inhale some cancerogenous smoke!");
						if (FunnyHallu) pline("Why are you such an idiot and smoke, anyway?");
						badeffect();
					}
					if (uwep && uwep->otyp == CIGAR && !rn2(100)) {
						switch (rnd(3)) {
							case 1:
								pline_The("smoke prevents you from seeing!");
								make_blinded(Blinded + rnz(500), FALSE);
								break;
							case 2:
								You("have a coughing fit!");
								make_numbed(HNumbed + rnz(500), FALSE);
								break;
							case 3:
								pline("The world spins and goes dark.");
								flags.soundok = 0;
								if (isstunfish) nomul(-rnz(10), "unconscious from smoking a cigar", TRUE);
								else nomul(-rnd(10), "unconscious from smoking a cigar", TRUE);
								nomovemsg = "You are conscious again.";
								afternmv = Hear_again;
								break;
						}
					}
					if (uwep && uwep->otyp == ELECTRIC_CIGARETTE && !rn2(250)) {
						pline("Kaboom! Your cigarette suddenly causes an explosion.");
						struct obj *dynamite;
						dynamite = mksobj(STICK_OF_DYNAMITE, TRUE, FALSE, FALSE);
						if (dynamite) {
							if (dynamite->otyp != STICK_OF_DYNAMITE) delobj(dynamite);
							else {
								dynamite->dynamitekaboom = 1;
								dynamite->quan = 1;
								dynamite->owt = weight(dynamite);
								dropy(dynamite);
								attach_bomb_blow_timeout(dynamite, 0, 0);
								run_timers();
							}
							if (!rn2(10)) {
								You("inhale some cancerogenous smoke!");
								if (FunnyHallu) pline("Why are you such an idiot and smoke, anyway?");
								badeffect();
							}
						}
					}
					if (!rn2(suckingchance)) {
						You("suck some health from the target.");
						healup(rnd(objects[uwep->otyp].oc_wldam), 0, FALSE, FALSE);
						flags.botl = TRUE;
					} else if (!rn2(suckingchance)) {
						You("suck some mana from the target.");
						u.uen += rnd(objects[uwep->otyp].oc_wldam);
						if (u.uen > u.uenmax) u.uen = u.uenmax;
						flags.botl = TRUE;
					}
				}
				if (uwep && uwep->otyp == STEEL_CAPPED_SANDAL && !rn2(uwep->oartifact == ART_PATRICIA_S_FEMININITY ? 150 : 30)) {
					uwep->spe--;
					pline("Your steel-capped sandal degrades.");
					if (uwep->spe < -15) {
						useupall(uwep);
						pline("Your steel-capped sandal is destroyed.");
						return FALSE;
					}
				}

				if (FemtrapActivePatricia && uwep && (objects[uwep->otyp].oc_dir == 0 ) && !rn2(uwep->oartifact ? 150 : 30) ) {
					uwep->spe--;
					if (SuperFemtrapPatricia) uwep->spe--;
					pline("Your weapon degrades.");
					if (uwep->spe < -20) {
						useupall(uwep);
						pline("Your weapon is destroyed.");
						return FALSE;
					}

				}

				if (uchain && uchain->oartifact == ART_STREEEEEAM && !rn2(20) && (u.dx || u.dy) && !u.dz) {
					buzz(20, 6, u.ux, u.uy, u.dx, u.dy);
					if (!mon) return FALSE;
					if (DEADMONSTER(mon)) return FALSE;
				}

				if (uleft && uleft->otyp == RIN_RUIN && !rn2(1000) && (u.dx || u.dy) && !u.dz) {
					buzz(29, GushLevel, u.ux, u.uy, u.dx, u.dy);
					buzz(20, GushLevel, u.ux, u.uy, u.dx, u.dy);
					if (!mon) return FALSE;
					if (DEADMONSTER(mon)) return FALSE;
				}
				if (uright && uright->otyp == RIN_RUIN && !rn2(1000) && (u.dx || u.dy) && !u.dz) {
					buzz(29, GushLevel, u.ux, u.uy, u.dx, u.dy);
					buzz(20, GushLevel, u.ux, u.uy, u.dx, u.dy);
					if (!mon) return FALSE;
					if (DEADMONSTER(mon)) return FALSE;
				}

				if (uleft && uleft->oartifact == ART_NARYA && !rn2(20) && (u.dx || u.dy) && !u.dz) {
					buzz(21, rnd(4), u.ux, u.uy, u.dx, u.dy);
					if (!mon) return FALSE;
					if (DEADMONSTER(mon)) return FALSE;
				}

				if (uright && uright->oartifact == ART_NARYA && !rn2(20) && (u.dx || u.dy) && !u.dz) {
					buzz(21, rnd(4), u.ux, u.uy, u.dx, u.dy);
					if (!mon) return FALSE;
					if (DEADMONSTER(mon)) return FALSE;
				}

				if (uwep && uwep->oartifact == ART_SKYSTORMER && !rn2(10) && (u.dx || u.dy) && !u.dz) {
					buzz(22, rn1(10, 10), u.ux, u.uy, u.dx, u.dy);
					if (!mon) return FALSE;
					if (DEADMONSTER(mon)) return FALSE;
				}

				if (uwep && uwep->oartifact == ART_FLOATING_PARTICLE && (u.dx || u.dy) && !u.dz) {
					buzz(20, rnd(4), u.ux, u.uy, u.dx, u.dy);
					if (!mon) return FALSE;
					if (DEADMONSTER(mon)) return FALSE;
				}

				if (uwep && uwep->oartifact == ART_LIGHTBLOOM && !rn2(100) && (u.dx || u.dy) && !u.dz) {
					buzz(18, 3, u.ux, u.uy, u.dx, u.dy);
					if (!mon) return FALSE;
					if (DEADMONSTER(mon)) return FALSE;
				}
				if (uwep && uwep->oartifact == ART_DIABLO_S_LBOD && !rn2(20) && (u.dx || u.dy) && !u.dz) {
					buzz(15, rnd(10), u.ux, u.uy, u.dx, u.dy);
					if (!mon) return FALSE;
					if (DEADMONSTER(mon)) return FALSE;
				}

				if (uwep && uwep->oartifact == ART_H__S_BRITTLE_REPLICA && !rn2(10)) {
					useupall(uwep);
					pline("Your broadsword replica shatters into a thousand fragments.");
					return FALSE;
				}

				if (u.twoweap && uswapwep && weaponwilldull(uswapwep) && (rnd(7) > savechance) && !issoviet) {
					if (uswapwep->greased) {
						uswapwep->greased--;
						pline("Your weapon loses its grease.");
					} else {
						uswapwep->spe--;
						pline("Your weapon dulls.");
						u.cnd_weapondull++;
					}
				}

				if (u.twoweap && uswapwep && objects[uswapwep->otyp].oc_material == MT_COMPOST && uswapwep->spe < 0 && !rn2(500)) {
					uswapwep->spe++;
					pline("Your off-hand weapon repairs itself a bit!");
				}

				if (u.twoweap && uswapwep && uswapwep->oartifact == ART_DESTRUCTION_BALL && !rn2(3) && uswapwep->spe > -20) {
					uswapwep->spe--;
					pline("Your ball sustains damage.");
				}
				if (u.twoweap && uswapwep && uswapwep->oartifact == ART_SKOGLO && !rn2(200)) {
					pushplayerfar(FALSE, 100);
				}

				if (u.twoweap && uswapwep && uswapwep->oartifact == ART_DONNNNNNNNNNNNG && !rn2(3) && uswapwep->spe > -20) {
					uswapwep->spe--;
					pline("Your weapon sustains damage.");
				}
				if (u.twoweap && uswapwep && uswapwep->oartifact == ART_DULL_METAL && !rn2(50) && uswapwep->spe > -20) {
					uswapwep->spe--;
					pline("Your weapon sustains damage.");
				}
				if (u.twoweap && uswapwep && uswapwep->otyp == SECRET_SOUND_WHIP) {
					increasesanity(1);
				}
				if (u.twoweap && uswapwep && uswapwep->oartifact == ART_CHA_SHATTER && !rn2(3) && uswapwep->spe > -20) {
					uswapwep->spe--;
					pline("Your weapon sustains damage.");
				}
				if (u.twoweap && uswapwep && uswapwep->oartifact == ART_SIGIX_BROADSWORD && !rn2(20)) {
					uswapwep->spe--;
					pline("Your broadsword sustains damage.");
					if (uswapwep->spe < -20) {
						useupall(uswapwep);
						pline("Your broadsword is destroyed.");
						return FALSE;
					}
				}

				if (u.twoweap && uswapwep && uswapwep->otyp == STEEL_CAPPED_SANDAL && !rn2(uswapwep->oartifact == ART_PATRICIA_S_FEMININITY ? 150 : 30)) {
					uswapwep->spe--;
					pline("Your steel-capped sandal degrades.");
					if (uswapwep->spe < -15) {
						useupall(uswapwep);
						pline("Your steel-capped sandal is destroyed.");
						return FALSE;
					}
				}

				if (FemtrapActivePatricia && u.twoweap && uswapwep && (objects[uswapwep->otyp].oc_dir == 0 ) && !rn2(uswapwep->oartifact ? 150 : 30) ) {
					uswapwep->spe--;
					if (SuperFemtrapPatricia) uswapwep->spe--;
					pline("Your weapon degrades.");
					if (uswapwep->spe < -20) {
						useupall(uswapwep);
						pline("Your weapon is destroyed.");
						return FALSE;
					}

				}

				if (u.twoweap && uswapwep && uswapwep->oartifact == ART_H__S_BRITTLE_REPLICA && !rn2(10)) {
					useupall(uswapwep);
					pline("Your broadsword replica shatters into a thousand fragments.");
					return FALSE;
				}

			}

			/* Enemy dead, before any special abilities used */
			if (!known_hitum(mon,mhit,&dhit,mattk,dieroll)) {
			    sum[i] = dhit | HIT_FATAL;
			    break;
			} else sum[i] = dhit;
			/* might be a worm that gets cut in half */
			if (m_at(u.ux+u.dx, u.uy+u.dy) != mon) return((boolean)(nsum != 0));
			/* Do not print "You hit" message, since known_hitum
			 * already did it.
			 */
			if (dhit && mattk->adtyp != AD_SPEL
				&& mattk->adtyp != AD_PHYS )
			    if (damageum(mon,mattk) == 2)
				sum[i] |= HIT_FATAL;

			break;
		case AT_CLAW:
			/*if (!cantwield(youmonst.data) &&
				u.umonnum != PM_MARILITH)
			    goto use_weapon;*/
#if 0	/* Shouldn't matter where the first AT_CLAW is anymore */
			/* succubi/incubi are humanoid, but their _second_
			 * attack is AT_CLAW, not their first...
			 */
			/*if (i==1 && uwep && (u.umonnum == PM_SUCCUBUS ||
				u.umonnum == PM_INCUBUS)) goto use_weapon;*/
#endif
		case AT_BITE:
			/* [ALI] Vampires are also smart. They avoid biting
			   monsters if doing so would be fatal */
			if ((uwep || u.twoweap && uswapwep) &&
				is_vampire(youmonst.data) &&
				(is_rider(mon->data) || is_deadlysin(mon->data) || slime_on_touch(mon->data) ))
			    break;
		case AT_STNG:
		case AT_TUCH:
		case AT_BUTT:
		case AT_LASH:
		case AT_TRAM:
		case AT_SCRA:
		case AT_BEAM:
		case AT_TENT:
		case AT_MAGC:
			/*if (i==0 && uwep && (youmonst.data->mlet==S_LICH)) goto use_weapon;*/
			if ((uwep || u.twoweap && uswapwep) && !Stone_resistance &&
				(touch_petrifies(mon->data) ||
				 mon->data == &mons[PM_MEDUSA]))
			    break;
		case AT_KICK:
			if ((dhit = ((tmp > dieroll) || (u.uswallow && rn2(3)) )) != 0) {
			    int compat;

			    if (!u.uswallow &&
				(compat=could_seduce(&youmonst, mon, mattk))) {
				You("%s %s %s.",
				    mon->mcansee && haseyes(mon->data)
				    ? "smile at" : "talk to",
				    mon_nam(mon),
				    compat == 2 ? "engagingly":"seductively");
				/* doesn't anger it; no wakeup() */
				sum[i] = hit_notouch[damageum(mon, mattk)];
				break;
			    }
			    wakeup(mon);
			    /* maybe this check should be in damageum()? */
			    if ((is_shade(mon->data) || mon->egotype_shader) && !(uwep && uwep->oartifact == ART_AP_) && !(uarmf && (objects[uarmf->otyp].oc_material == MT_SILVER || objects[uarmf->otyp].oc_material == MT_ARCANIUM)) &&
					!(mattk->aatyp == AT_KICK &&
					    uarmf && uarmf->blessed)) {
				Your("attack passes harmlessly through %s.",
				    mon_nam(mon));
				break;
			    }
			    if (mattk->aatyp == AT_KICK)
				    You("kick %s.", mon_nam(mon));
			    else if (mattk->aatyp == AT_BITE)
				    You("bite %s.", mon_nam(mon));
			    else if (mattk->aatyp == AT_CLAW)
				    You("claw %s.", mon_nam(mon));
			    else if (mattk->aatyp == AT_STNG)
				    You("sting %s.", mon_nam(mon));
			    else if (mattk->aatyp == AT_BUTT)
				    You("butt %s.", mon_nam(mon));
			    else if (mattk->aatyp == AT_LASH)
				    You("lash %s.", mon_nam(mon));
			    else if (mattk->aatyp == AT_TRAM)
				    You("trample over %s.", mon_nam(mon));
			    else if (mattk->aatyp == AT_SCRA)
				    You("scratch %s.", mon_nam(mon));
			    else if (mattk->aatyp == AT_BEAM)
				    You("blast %s.", mon_nam(mon));
			    else if (mattk->aatyp == AT_TUCH)
				    You("touch %s.", mon_nam(mon));
			    else if (mattk->aatyp == AT_MAGC)
				    You("curse at %s.", mon_nam(mon));
			    else if (mattk->aatyp == AT_TENT)
				    Your("tentacles suck %s.", mon_nam(mon));
			    else You("hit %s.", mon_nam(mon));
			    sum[i] = hit_touch[damageum(mon, mattk)];
			} else
			    missum(mon, tmp, dieroll, mattk);
			break;

		case AT_HUGS:
			/* automatic if prev two attacks succeed, or if
			 * already grabbed in a previous attack
			 */
			dhit = 1;
			wakeup(mon);
			if ((is_shade(mon->data) || mon->egotype_shader) && !(uwep && uwep->oartifact == ART_AP_) )
			    Your("hug passes harmlessly through %s.",
				mon_nam(mon));
			else if (!sticks(mon->data) && !u.uswallow) {
			    if (mon==u.ustuck) {
				pline("%s is being %s.", Monnam(mon),
				    u.umonnum==PM_ROPE_GOLEM ?
				    breathless(mon->data) ? "strangled" :
				    "choked" : "crushed");
				sum[i] = hit_touch[damageum(mon, mattk)];
			    } else if((i >= 2 && sum[i-1] && sum[i-2]) || !rn2(40)) {
		/* some polymorph forms might not have two previous attacks... --Amy */
				You("grab %s!", mon_nam(mon));
				setustuck(mon);
				sum[i] = hit_touch[damageum(mon, mattk)];
			    }
			}
			break;

		case AT_EXPL:	/* automatic hit if next to */
			dhit = -1;
			wakeup(mon);
			sum[i] = hit_notouch[explum(mon, mattk)];
			break;

		case AT_ENGL:
			if((dhit = (tmp > (dieroll = rnd(20+i))))) {
				wakeup(mon);
				if ((is_shade(mon->data) || mon->egotype_shader) && !(uwep && uwep->oartifact == ART_AP_) )
				    Your("attempt to surround %s is harmless.",
					mon_nam(mon));
				else {
				    sum[i]= hit_touch[gulpum(mon,mattk)];
				    if (sum[i] & HIT_FATAL &&
					    (mon->data->mlet == S_ZOMBIE ||
						mon->data->mlet == S_MUMMY) &&
					    rn2(5) && !Sick_resistance) {
						You_feel("%ssick.", (Sick) ? "very " : "");
						mdamageu(mon, rnd(8));
				    }
				}
			} else
				missum(mon, tmp, dieroll, mattk);
			break;

		/*case AT_MAGC:*/
			/* No check for uwep; if wielding nothing we want to
			 * do the normal 1-2 points bare hand damage...
			 */
			/*if (i == 0 && (youmonst.data->mlet==S_KOBOLD
				|| youmonst.data->mlet==S_ORC
				|| youmonst.data->mlet==S_GNOME
				)) goto use_weapon;*/

		case AT_NONE:
		case AT_RATH:
		case AT_BOOM:
			continue;
			/* Not break--avoid passive attacks from enemy */

		case AT_BREA:
		case AT_SPIT:
			dhit = 0;
			break;

		case AT_GAZE:   /* WAC -- can be either ranged attack OR close */
			if (Blind) {
				dhit = 0;
				break;
			}
			if (!canseemon(mon) && rn2(3)) {
				You("gaze around,  but miss!");
				dhit = 0;
				break;
			}
			You("gaze at %s...", mon_nam(mon));

			if ((mon->data==&mons[PM_MEDUSA]) && !mon->mcan) {
				pline("Gazing at the awake Medusa is not a very good idea.");
				/* as if gazing at a sleeping anything is fruitful... */
				/*You("turn to stone...");
				killer_format = KILLED_BY;
				killer = "deliberately gazing at Medusa's hideous countenance";
				done(STONING);*/
				You("start turning to stone...");
				if (!Stoned) {
					if (Hallucination && rn2(10)) pline("But you are already stoned.");
					else {
						Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
						u.cnd_stoningcount++;
						delayed_killer = "deliberately gazing at Medusa's hideous countenance";
					}
				}
			} else if (!mon->mcansee || mon->msleeping) {
				pline("But nothing happens.");
				dhit = 0;
				break;
			} else if (Invis && !perceives(mon->data)) {
				pline("%s seems not to notice your gaze.", Monnam(mon));
				break;
			}
			sum[i] = hit_notouch[damageum(mon, mattk)];
			break;

		case AT_MULTIPLY:
			/* Not a #monster ability -- this is something that the
			 * player must figure out -RJ */
			if (!u.sterilized && !rn2(20)) {
				pline("You multiply!");
				cloneu(); /* greatly lowered chance --Amy */
			}
			break;

		default: /* Strange... */
			impossible("strange attack of yours (%d)",
				 mattk->aatyp);
	    }
	    if (dhit == -1) {
		u.mh = -1;	/* dead in the current form */
    	      if (!Race_if(PM_UNGENOMOLD)) rehumanize(); /* we don't want ungenomolds polyd into lights to suck */
		else polyself(FALSE);
	    }

	    if (sum[i] & HIT_FATAL)
		return((boolean)passive(mon, sum[i], 0, mattk->aatyp, FALSE));
							/* defender dead */
	    else {

	     /* bullshit downside to jabberwock: they have so many melee attacks that are so powerful, they need to be nerfed
	     * this is achieved by making the monster be able to retaliate faster if it survives your attacks --Amy */
	     if (mon && Race_if(PM_PLAYER_JABBERWOCK)) {
			mon->movement++;
			if (mon->data->mmove >= 3) {
				mon->movement += rn2((mon->data->mmove / 3) + 1);
				if (!rn2(2)) mon->movement += rn2((mon->data->mmove / 3) + 1);
			}
	     }

		(void) passive(mon, sum[i], 1, mattk->aatyp, FALSE);
		nsum |= sum[i];
	    }
	    if (Upolyd != Old_Upolyd)
		break; /* No extra attacks if no longer a monster */
	    if (multi < 0)
		break; /* If paralyzed while attacking, i.e. floating eye */
	}
	if (willsymattack && !symbioteprocess && uactivesymbiosis) {
		if (!noattacks(&mons[u.usymbiote.mnum])) {
			Your("%s symbiote attacks!", mons[u.usymbiote.mnum].mname);
			u.symbioteattacking = TRUE;
			u.usymbiosisfastturns++;
			if (u.usymbiosisfastturns >= 3) {
				u.usymbiosisfastturns = 0;
				use_skill(P_SYMBIOSIS, 1);
			}
		}
		symbioteprocess = TRUE;
		goto symbiotejump;
	}

	/* powerbiosis is supposed to double the damage, but that's so annoying to code... let it attack twice instead --Amy */
	if (tech_inuse(T_POWERBIOSIS) && willsymattack && symbioteprocess && uactivesymbiosis && !symbiotedouble) {
		symbiotedouble = TRUE;
		goto symbiotejump;
	}

	if (carthageattack && !carthageprocess && Race_if(PM_CARTHAGE) && u.usteed) {
		carthageprocess = TRUE;
		symbiotenomore = TRUE;

		mdat2 = &mons[PM_CARTHAGE_DUMMY];
		carthageattk = &mdat2->mattk[0];
		carthageattk->damd = u.ulevel;

		goto carthagejump;
	}

	u.symbioteattacking = FALSE;

	return((boolean)(nsum != 0));
}

/*	Special (passive) attacks on you by monsters done here.		*/

int
passive(mon, mhit, malive, aatyp, ranged)
register struct monst *mon;
register int mhit;
register int malive;
uchar aatyp;
boolean ranged;
{
	register struct permonst *ptr = mon->data;
	register int i, tmp;
	int tmptemp;
	register struct engr *ep = engr_at(u.ux,u.uy);
	struct obj *target = mhit & HIT_UWEP ? uwep :
		mhit & HIT_USWAPWEP ? uswapwep : (struct obj *)0;
	char buf[BUFSZ];
	struct obj *optr;
	struct obj *otmpi, *otmpii;
	int hallutime;
	struct permonst *mdat2;
      struct attack *attspc;
	boolean rathback = FALSE;

	/*int randattackC = 0;*/
	int atttypC;

	if (RngeIronMaiden) {
		pline("Ouch - you hurt yourself!");
		losehp(rnd(u.ulevel), "the iron maiden curse", KILLED_BY);
	}

	if (mhit && aatyp == AT_BITE && is_vampire(youmonst.data)) {
	    if (bite_monster(mon))
		return 2;			/* lifesaved */
	}

	/* constitution is a relatively useless attribute, so I decide to make it more useful: act as saving throw --Amy */
	if (rnd(100) < ACURR(A_CON)) return(malive | mhit);

	for(i = 0; ; i++) {
	    if(i >= NATTK) return(malive | mhit);	/* no passive attacks */
	    if((!ranged && (ptr->mattk[i].aatyp == AT_NONE || (!malive && ptr->mattk[i].aatyp == AT_BOOM)) ) ||
		  (ranged && (ptr->mattk[i].aatyp == AT_RATH || (!malive && monnear(mon, u.ux, u.uy) && ptr->mattk[i].aatyp == AT_BOOM)) ) ) { /* try this one */

		if (ptr->mattk[i].aatyp == AT_RATH) rathback = TRUE;
		else rathback = FALSE;

	/*}*/ /* the above allows multiple passive attacks on a single monster; code from FHS --Amy */

	/* Note: tmp not always used */
	if (ptr->mattk[i].damn)
	    tmp = d((int)ptr->mattk[i].damn, (int)ptr->mattk[i].damd);
	else if(ptr->mattk[i].damd)
	    tmp = d( ((int)mon->m_lev / 3)+1, (int)ptr->mattk[i].damd); /* Yes guys and girls, blue slimes were OP. --Amy */
	else
	    tmp = 0;

	if (YouTakeMaximumDamage) {
		if (ptr->mattk[i].damn)
		    tmp = (int)ptr->mattk[i].damn * (int)ptr->mattk[i].damd;
		else if(ptr->mattk[i].damd)
		    tmp = (((int)mon->m_lev / 3)+1) * (int)ptr->mattk[i].damd;
		else
		    tmp = 0;

	}

	if (Race_if(PM_PLAYER_JABBERWOCK) && tmp > 0) tmp *= 2;

	/* Monsters with AD_RBRE are supposed to have a random passive attack every time they are hit. --Amy */

	atttypC = ptr->mattk[i].adtyp;

	if ((RealLieEffect || u.uprops[REAL_LIE_EFFECT].extrinsic || have_realliestone()) && !rn2(2) && (atttypC != AD_PHYS) ) atttypC = reallie(atttypC);

	if (atttypC == AD_RBRE) {
		while (atttypC == AD_ENDS || atttypC == AD_RBRE || atttypC == AD_WERE) {
			atttypC = randattack(); }
		/*randattack = 1;*/
	}

	if (atttypC == AD_DAMA) {
		atttypC = randomdamageattack();
	}

	if (atttypC == AD_ILLU) {
		atttypC = randomillusionattack();
	}

	if (atttypC == AD_THIE) {
		atttypC = randomthievingattack();
	}

	if (atttypC == AD_RNG) {
		while (atttypC == AD_ENDS || atttypC == AD_RNG || atttypC == AD_WERE) {
			atttypC = rn2(AD_ENDS); }
	}

	if (atttypC == AD_PART) atttypC = u.adpartattack;

	if (atttypC == AD_MIDI) {
		atttypC = mon->m_id;
		if (atttypC < 0) atttypC *= -1;
		while (atttypC >= AD_ENDS) atttypC -= AD_ENDS;
		if (!(atttypC >= AD_PHYS && atttypC < AD_ENDS)) atttypC = AD_PHYS; /* fail safe --Amy */
		if (atttypC == AD_WERE) atttypC = AD_PHYS;
	}

/*	These affect you even if they just died */
	switch(atttypC) {

	  case AD_ACID:
	    if((mhit || rathback) && rn2(2)) {
		if (Blind || !flags.verbose) You("are splashed!");
		else	You("are splashed by %s acid!",
			                s_suffix(mon_nam(mon)));

		if (Stoned) fix_petrification();

		if ( (!Acid_resistance || !rn2(StrongAcid_resistance ? 20 : 5)) && !AcidImmunity)
			mdamageu(mon, tmp);
		if(!rn2(30)) erode_armor(&youmonst, TRUE);
	    }
	    if (mhit) {
		if (aatyp == AT_KICK) {
		    if (uarmf && !rn2(6))
			(void)rust_dmg(uarmf, xname(uarmf), 3, TRUE, &youmonst);
		} else if (aatyp == AT_WEAP || aatyp == AT_CLAW ||
			   aatyp == AT_MAGC || aatyp == AT_TUCH)
		    passive_obj(mon, target, &(ptr->mattk[i]));
	    }
	    exercise(A_STR, FALSE);
	    break;
	  case AD_TCKL:
		if (mhit || rathback) {
			You("get hurt by %s spikes!", s_suffix(mon_nam(mon)));	
			mdamageu(mon, tmp);
		}
		break;

	  case AD_DREA:
		if (multi < 0) {
			tmp *= 4;
			pline("Your dream is eaten!");
			mdamageu(mon, tmp);
		}
		break;

	  case AD_BADE:

		badeffect();

		break;

	  case AD_RBAD:

		reallybadeffect();

		break;

	    case AD_BLEE:

		if (tmp > 0) playerbleed(tmp);

		break;

	    case AD_SHAN:

		shank_player();

		break;

	    case AD_UNPR:

		if (ptr->mattk[i].aatyp == AT_RATH) {
			struct obj *obj = some_armor(&youmonst);
			if (obj && obj->oerodeproof) {
				obj->oerodeproof = FALSE;
				Your("%s brown for a moment.", aobjnam(obj, "glow"));
			}
		} else {

		    if(mhit && !mon->mcan) {
			if (aatyp == AT_KICK) {
				if (uarmf && uarmf->oerodeproof) {
					uarmf->oerodeproof = FALSE;
					Your("%s brown for a moment.", aobjnam(uarmf, "glow"));
				}
			} else if (aatyp == AT_WEAP || aatyp == AT_CLAW || aatyp == AT_MAGC || aatyp == AT_TUCH)
				passive_obj(mon, target, &(ptr->mattk[i]));
		    }
		}

		break;

	    case AD_NIVE:

		nivellate();
		break;

	    case AD_DEBU:

		pline("You're being sapped!");
		statdebuff();

		break;

	    case AD_SCOR:

		u.urexp -= (tmp * 50);
		if (u.urexp < 0) u.urexp = 0;
		Your("score is drained!");

		break;

	    case AD_TERR:

		if (mon->data == &mons[PM_BUILDER]) terrainterror(1);
		else terrainterror(0);

		break;

	    case AD_FEMI:

		randomfeminismtrap(rnz( (tmp + 2) * rnd(100)));
		if (Role_if(PM_CLIMACTERIAL)) climtrainsqueaking(tmp);

		break;

	    case AD_LEVI:

		You("float up, against an obstacle! Ouch!");
		HLevitation &= ~I_SPECIAL;
		incr_itimeout(&HLevitation, tmp);
		mdamageu(mon, tmp);

		break;

	    case AD_MCRE:

		MCReduction += (100 * tmp);
		pline("The magic cancellation granted by your armor seems weaker now...");
		mdamageu(mon, tmp);

		break;

	  case AD_FUMB:

		pline("You stumble!");
		HFumbling = FROMOUTSIDE | rnd(5);
		incr_itimeout(&HFumbling, rnd(20));
		u.fumbleduration += rnz(10 * (tmp + 1) );

		break;

	  case AD_TREM:

		pline("You tremble!");
		u.tremblingamount++;

		break;

	    case AD_IDAM:

		pline("Attacking this monster causes damage to your inventory...");
		{
		    register struct obj *objX, *objX2;
		    for (objX = invent; objX; objX = objX2) {
		      objX2 = objX->nobj;
			if (!rn2(20)) rust_dmg(objX, xname(objX), rn2(4), TRUE, &youmonst);
		    }
		}

		break;

	    case AD_ANTI:

		pline("Attacking this monster causes severe damage to your inventory...");
		{
		    register struct obj *objX, *objX2;
		    for (objX = invent; objX; objX = objX2) {
		      objX2 = objX->nobj;
			if (!rn2(20)) wither_dmg(objX, xname(objX), rn2(4), TRUE, &youmonst);
		    }
		}

		break;

	    case AD_PAIN:

		pline("Ouch - you've hurt yourself!");
		losehp(Upolyd ? ((u.mh / 10) + 1) : ((u.uhp / 10) + 1), "a painful attack", KILLED_BY);

		break;

	    case AD_TECH:

		techcapincrease(tmp * rnd(50));

		break;

	    case AD_MEMO:

		spellmemoryloss(tmp);

		break;

	    case AD_TRAI:

		skilltrainingdecrease(tmp);
		if (!rn2(100)) skillcaploss();

		break;

	    case AD_STAT:

		if (!rn2(3)) {
			statdrain();
		}

		break;

	    case AD_VULN:

		deacrandomintrinsic(rnz( (tmp * rnd(30) ) + 1));
		break;

	    case AD_ICUR:
		if (!rn2(3)) {
			You_feel("as if you need some help.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
			rndcurse();
		}
		break;

	    case AD_NACU:

		if (!rn2(4)) nastytrapcurse();
		break;

	    case AD_SLUD:
		{
		    register struct obj *objX, *objX2;
		    for (objX = invent; objX; objX = objX2) {
		      objX2 = objX->nobj;
			if (!rn2(5)) rust_dmg(objX, xname(objX), 3, TRUE, &youmonst);
		    }
		}

		break;

	    case AD_NAST:

		if (!rn2(5)) {
			pline("Nasty!");

			randomnastytrapeffect(rnz( (tmp + 2) * rnd(100)), 1000 - (tmp * 3));

		}

		break;

	  case AD_CHKH:
		if (mhit || rathback) {
			pline("WHACK! You feel like you just got whipped!");
			tmp += u.chokhmahdamage;
			tmp += rnd(u.ualign.sins > 0 ? (isqrt(u.ualign.sins) + 1) : (1));
			u.chokhmahdamage++;
			mdamageu(mon, tmp);
		}
		break;

	  case AD_HODS:
		pline("Your attack is mirrored and hits you!");
		 if(uwep){
			if (uwep->otyp == CORPSE
				&& touch_petrifies(&mons[uwep->corpsenm])) {
			    tmp = 1;
			    pline("%s hits you with the %s corpse.",
				Monnam(mon), mons[uwep->corpsenm].mname);

			    if ((!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) &&
				!(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {

				if (!Stoned) {
					if (Hallucination && rn2(10)) pline("Thankfully you are already stoned.");
					else {
						Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
						u.cnd_stoningcount++;
						sprintf(killer_buf, "being hit by a mirrored petrifying corpse");
						delayed_killer = killer_buf;
					}
				}
		
			    }

			}
			tmp += dmgval(uwep, &youmonst);
			
			if (uwep->opoisoned){
				sprintf(buf, "%s mirror attack",
					s_suffix(Monnam(mon)));
				poisoned(buf, A_CON, mon->data->mname, 30);
			}
			int dieroll = rnd(20);

			if (tmp <= 0) tmp = 1;
			tmptemp = tmp;
			if (!(uwep->oartifact && artifact_hit(mon, &youmonst, uwep, &tmptemp,dieroll))) pline("Rattle/clink!");
			if (!tmp) break;
		 }
		mdamageu(mon, tmp);
		break;

	    case AD_CHRN:
		pline("That was a bad idea.");

		    if (!obsidianprotection()) switch (rn2(11)) {
		    case 0: difeasemu(mon->data);
			    break;
		    case 1: make_blinded(Blinded + tmp, TRUE);
			    break;
		    case 2: if (!Confusion)
				You("suddenly feel %s.",
				    FunnyHallu ? "trippy" : "confused");
			    make_confused(HConfusion + tmp, TRUE);
			    break;
		    case 3: make_stunned(HStun + tmp, TRUE);
			    break;
		    case 4: make_numbed(HNumbed + tmp, TRUE);
			    break;
		    case 5: make_frozen(HFrozen + tmp, TRUE);
			    break;
		    case 6: make_burned(HBurned + tmp, TRUE);
			    break;
		    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
			    break;
		    case 8: (void) make_hallucinated(HHallucination + tmp, TRUE, 0L);
			    break;
		    case 9: make_feared(HFeared + tmp, TRUE);
			    break;
		    case 10: make_dimmed(HDimmed + tmp, TRUE);
			    break;
		    }

		break;

	    case AD_WEEP:
		if (!rn2(3) && !playerlevelportdisabled() ) {
			make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */
			if (!u.levelporting) {
				u.levelporting = 1;
				nomul(-2, "being levelwarped", FALSE); /* because it's not called until you get another turn... */
			}
			return(0);
		}
		else if (!rn2(3) && (!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4) )  ) {
		    losexp("loss of potential", FALSE, TRUE);
		}
		break;

	  case AD_THIR:
		if(malive && (mhit || rathback) && rn2(3)) {
			pline("Your %s is sucked!", body_part(BLOOD) );
			mon->mhp += tmp;
			if (mon->mhp > mon->mhpmax) mon->mhp = mon->mhpmax;
			mdamageu(mon, tmp);
		}
		break;

	  case AD_NTHR:
		if(malive && (mhit || rathback) && rn2(3)) {
			pline("Your %s is sucked!", body_part(BLOOD) );
			mon->mhp += tmp;
			if (mon->mhp > mon->mhpmax) mon->mhp = mon->mhpmax;
			mdamageu(mon, tmp);
		}
		if ((!Drain_resistance || !rn2(StrongDrain_resistance ? 20 : 5)) && u.uexp > 100) {
			u.uexp -= (u.uexp / 100);
			You_feel("your life slipping away!");
			if (u.uexp < newuexp(u.ulevel - 1)) {
			      losexp("nether forces", TRUE, FALSE);
			}
		}
		break;

	    case AD_RAGN:

		ragnarok(FALSE);
		if (evilfriday && mon->m_lev > 1) evilragnarok(FALSE,mon->m_lev);
		break;

	    case AD_AGGR:

		incr_itimeout(&HAggravate_monster, tmp);
		You_feel("that monsters are aware of your presence.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Dazhe sovetskaya Pyat' Lo obostryayetsya v vashem nizkom igrovom masterstve." : "Woaaaaaah!");
		aggravate();
		if (!rn2(20)) {

			int aggroamount = rnd(6);
			if (isfriday) aggroamount *= 2;
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
			while (aggroamount) {

				u.cnd_aggravateamount++;
				makemon((struct permonst *)0, u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
				aggroamount--;
				if (aggroamount < 0) aggroamount = 0;
			}
			u.aggravation = 0;
			pline("Several monsters come out of a portal.");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

		}

		break;

	    case AD_DATA:

		u.datadeletedefer = 1;
		datadeleteattack();
		break;

	    case AD_DEST:

		destroyarmorattack();
		break;

	    case AD_MINA:

		{
			register int midentity = mon->m_id;
			if (midentity < 0) midentity *= -1;
			while (midentity > 275) midentity -= 275; /* timerun! */

			register int nastyduration = ((tmp + 2) * rnd(10));
			if (YouAreScrewedEternally) nastyduration *= 20;
			if (LongScrewupXtra) nastyduration *= 10;
			register int blackngvar = 1000 - (tmp * 3);

			specificnastytrapeffect(midentity, nastyduration, blackngvar);

		}

		break;

	    case AD_RUNS:

		{
			register int nastyduration = ((tmp + 2) * rnd(10));
			if (YouAreScrewedEternally) nastyduration *= 20;
			if (LongScrewupXtra) nastyduration *= 10;
			register int blackngvar = 1000 - (tmp * 3);

			specificnastytrapeffect(u.adrunsattack, nastyduration, blackngvar);

		}

		break;

	    case AD_SIN:

		You("committed the sin of attacking %s!", mon_nam(mon));
		increasesincounter(1);
		u.alignlim--;
		adjalign(-5);

		break;

	    case AD_ALIN:

		if (tmp) {
			You_feel("displeased!");
			adjalign(-tmp);
		}

		break;

	    case AD_CONT:

		You("are struck by several ether thorns.");
		contaminate(tmp, TRUE);
		mdamageu(mon, tmp);

		break;

	    case AD_NEXU:
		if (level.flags.noteleport || Race_if(PM_STABILISATOR) || u.antitelespelltimeout || (u.uhave.amulet && !u.freeplaymode) || CannotTeleport || On_W_tower_level(&u.uz) || (u.usteed && mon_has_amulet(u.usteed)) ) tmp *= (1 + rnd(2));
		mdamageu(mon, tmp);
		switch (rnd(7)) {

			case 1:
			case 2:
			case 3:
				pline("You are beamed far away!");
				teleX();
				break;
			case 4:
			case 5:
				pline("You are beamed away!");
				phase_door(0);
				break;
			case 6:

				if (!playerlevelportdisabled() ) {
					make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */
					level_tele();
					nomul(-2, "being levelwarped", FALSE);
					return(0);
				}
				break;
			case 7:
				{
					nexus_swap();

					if (!rn2(3)) {

						int reducedstat = rn2(A_MAX);
						if(ABASE(reducedstat) <= ATTRABSMIN(reducedstat)) {
							pline("Your health was damaged!");
							u.uhpmax -= rnd(5);
							if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
							if (u.uhp < 1) {
								u.youaredead = 1;
								killer = "nexus scrambling";
								killer_format = KILLED_BY;
								done(DIED);
								u.youaredead = 0;
							}

						} else {
							ABASE(reducedstat) -= 1;
							AMAX(reducedstat) -= 1;
							flags.botl = 1;
							pline("Your attributes were damaged!");
						}
					}
				}
				break;
		}
		break;

	    case AD_SOUN:
		pline("%s screams terribly at your attack, and the noise seems to blow your ears!", Monnam(mon) );
		if (YouAreDeaf) tmp /= 2;
		make_stunned(HStun + tmp, TRUE);
		if (isevilvariant || !rn2(issoviet ? 2 : 5)) (void)destroy_item(POTION_CLASS, AD_COLD);
		wake_nearby();
		break;

	    case AD_GRAV:
		pline("As you try to hit %s, you're hurled through the air and slam onto the floor with a crash.", mon_nam(mon) );
		if (level.flags.noteleport || Race_if(PM_STABILISATOR) || u.antitelespelltimeout || (u.uhave.amulet && !u.freeplaymode) || CannotTeleport || On_W_tower_level(&u.uz) || (u.usteed && mon_has_amulet(u.usteed)) ) tmp *= 2;
		phase_door(0);
		pushplayer(FALSE);
		u.uprops[DEAC_FAST].intrinsic += (tmp + 2);
		make_stunned(HStun + tmp, TRUE);
		mdamageu(mon, tmp);
		break;

	    case AD_WGHT:
		pline("Suddenly your load gets heavier!");
		IncreasedGravity += (1 + (tmp * rnd(20)));

		break;

	    case AD_INVE:

		u.uprops[INVERT_STATE].intrinsic += ((tmp + 2) * 10);
		pline("Whoops, you're inverted!");

		break;

	    case AD_DEBT:

		u.moneydebt += ((tmp + 2) * rnd(100));
		You("have to pay %d zorkmids to the bank.", u.moneydebt);
		break;

	    case AD_WNCE:

		u.uprops[WINCE_STATE].intrinsic += ((tmp + 2) * 10);
		pline("That was painful!");

		break;

	    case AD_INER:
		if (inertiaprotection()) break;
	      u_slow_down();
		u.uprops[DEAC_FAST].intrinsic += ((tmp + 2) * 10);
		pline(u.inertia ? "That was a bad idea - your body struggles at your attempts to get it to move again..." : "That was a bad idea - your body lost the will to listen to your instructions...");
		u.inertia += (tmp + 2);
		break;

	    case AD_TIME:

		if (powerfulimplants() && uimplant && uimplant->oartifact == ART_TIMEAGE_OF_REALMS) break;
		switch (rnd(10)) {

			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				You_feel("life has clocked back.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Zhizn' razgonyal nazad, potomu chto vy ne smotreli, i teper' vy dolzhny poluchit', chto poteryannyy uroven' nazad." : "Kloeck!");
			      losexp("time", FALSE, FALSE); /* resistance is futile :D */
				break;
			case 6:
			case 7:
			case 8:
			case 9:
				switch (rnd(A_MAX)) {
					case A_STR:
						pline("You're not as strong as you used to be...");
						ABASE(A_STR) -= 5;
						if(ABASE(A_STR) < ATTRABSMIN(A_STR)) {tmp *= 3; ABASE(A_STR) = ATTRABSMIN(A_STR);}
						break;
					case A_DEX:
						pline("You're not as agile as you used to be...");
						ABASE(A_DEX) -= 5;
						if(ABASE(A_DEX) < ATTRABSMIN(A_DEX)) {tmp *= 3; ABASE(A_DEX) = ATTRABSMIN(A_DEX);}
						break;
					case A_CON:
						pline("You're not as hardy as you used to be...");
						ABASE(A_CON) -= 5;
						if(ABASE(A_CON) < ATTRABSMIN(A_CON)) {tmp *= 3; ABASE(A_CON) = ATTRABSMIN(A_CON);}
						break;
					case A_WIS:
						pline("You're not as wise as you used to be...");
						ABASE(A_WIS) -= 5;
						if(ABASE(A_WIS) < ATTRABSMIN(A_WIS)) {tmp *= 3; ABASE(A_WIS) = ATTRABSMIN(A_WIS);}
						break;
					case A_INT:
						pline("You're not as bright as you used to be...");
						ABASE(A_INT) -= 5;
						if(ABASE(A_INT) < ATTRABSMIN(A_INT)) {tmp *= 3; ABASE(A_INT) = ATTRABSMIN(A_INT);}
						break;
					case A_CHA:
						pline("You're not as beautiful as you used to be...");
						ABASE(A_CHA) -= 5;
						if(ABASE(A_CHA) < ATTRABSMIN(A_CHA)) {tmp *= 3; ABASE(A_CHA) = ATTRABSMIN(A_CHA);}
						break;
				}
				break;
			case 10:
				pline("You're not as powerful as you used to be...");
				ABASE(A_STR)--;
				ABASE(A_DEX)--;
				ABASE(A_CON)--;
				ABASE(A_WIS)--;
				ABASE(A_INT)--;
				ABASE(A_CHA)--;
				if(ABASE(A_STR) < ATTRABSMIN(A_STR)) {tmp *= 2; ABASE(A_STR) = ATTRABSMIN(A_STR);}
				if(ABASE(A_DEX) < ATTRABSMIN(A_DEX)) {tmp *= 2; ABASE(A_DEX) = ATTRABSMIN(A_DEX);}
				if(ABASE(A_CON) < ATTRABSMIN(A_CON)) {tmp *= 2; ABASE(A_CON) = ATTRABSMIN(A_CON);}
				if(ABASE(A_WIS) < ATTRABSMIN(A_WIS)) {tmp *= 2; ABASE(A_WIS) = ATTRABSMIN(A_WIS);}
				if(ABASE(A_INT) < ATTRABSMIN(A_INT)) {tmp *= 2; ABASE(A_INT) = ATTRABSMIN(A_INT);}
				if(ABASE(A_CHA) < ATTRABSMIN(A_CHA)) {tmp *= 2; ABASE(A_CHA) = ATTRABSMIN(A_CHA);}
				break;
		}
		break;

	    case AD_PLAS:

			pline("You are suddenly extremely hot!");
			if (Race_if(PM_LOWER_ENT)) tmp *= 2;
			if (!Fire_resistance) tmp *= 2;
			if (StrongFire_resistance && tmp > 1) tmp /= 2;
			if (FireImmunity && tmp > 1) tmp = 1;

		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5)) /* extremely hot - very high chance to burn items! --Amy */
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5))
		      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);
		    burn_away_slime();
			make_stunned(HStun + tmp, TRUE);
		mdamageu(mon, tmp);

		break;

	    case AD_MANA:
		pline("You're irradiated by pure unresistable mana energy!");
		drain_en(tmp);
		mdamageu(mon, tmp);
		break;

	    case AD_SKIL:
		if (!rn2(10)) skillcaploss();
		break;

	    case AD_TDRA:
		if (!rn2(10)) techdrain();
		break;

	    case AD_DROP:
		dropitemattack();
		break;

	    case AD_BLAS:
		if (!rn2(25)) {
			u.ugangr++;
		      You("get the feeling that %s is angry...", u_gname());
		}
		break;

	  case AD_NGRA:

		      if (ep && sengr_at("Elbereth", u.ux, u.uy) ) {
		/* This attack can remove any Elbereth engraving, even burned ones. --Amy */
			pline("The engraving underneath you vanishes!");
		    del_engr(ep);
		    ep = (struct engr *)0;
		}

		break;

	    case AD_FAKE:
		{
			pline("%s", fauxmessage());
			u.cnd_plineamount++;
			if (!rn2(3)) {
				pline("%s", fauxmessage());
				u.cnd_plineamount++;
			}
		}
		break;

	    case AD_AMNE:

		pline("Suddenly you realize that you forgot to think about Maud recently.");
		maprot();
		break;

	    case AD_LETH:
		if (!rn2(3)) {
			pline("Sparkling water splashes over you!");
			lethe_damage(invent, FALSE, FALSE);
			if (!rn2(3)) actual_lethe_damage(invent, FALSE, FALSE);
			if (!rn2(issoviet ? 2 : 3)) forget_levels(rnd(issoviet ? 25 : 10), FALSE);	/* lose memory of 25% of levels */
			if (!rn2(issoviet ? 3 : 5)) forget_objects(rnd(issoviet ? 25 : 10), FALSE);	/* lose memory of 25% of objects */
		}
		break;
	    case AD_WET:
		if (!rn2(3)) {
			pline("Water splashes over you!");
			if ((!StrongSwimming || !rn2(10)) && (!StrongMagical_breathing || !rn2(10))) {
				water_damage(invent, FALSE, FALSE);
				if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);
			}
		}
		break;

	    case AD_SUCK:
			if (noncorporeal(youmonst.data) || amorphous(youmonst.data)) tmp = 0;
			else{
				if( has_head(youmonst.data) && !Role_if(PM_COURIER) && !uarmh && !rn2(20) && 
					((!Upolyd && u.uhp < (u.uhpmax / 10) ) || (Upolyd && u.mh < (u.mhmax / 10) ))
				){
					tmp = 2 * (Upolyd ? u.mh : u.uhp)
						  + 400; //FATAL_DAMAGE_MODIFIER;
					pline("%s sucks your %s off!",
					      Monnam(mon), body_part(HEAD));
				}
				else{
					You_feel("%s trying to suck your extremities off!",mon_nam(mon));
					if(!rn2(10)){
						Your("%s twist from the suction!", makeplural(body_part(LEG)));
					    set_wounded_legs(RIGHT_SIDE, HWounded_legs + rnd(60-ACURR(A_DEX)));
					    set_wounded_legs(LEFT_SIDE, HWounded_legs + rnd(60-ACURR(A_DEX)));
					    exercise(A_STR, FALSE);
					    exercise(A_DEX, FALSE);
					}
					if(uwep && !rn2(6)){
						You_feel("%s pull on your weapon!",mon_nam(mon));
						if( rnd(130) > ACURR(A_STR)){
							Your("weapon is sucked out of your grasp!");
							optr = uwep;
							uwepgone();
							freeinv(optr);
							(void) mpickobj(mon,optr,FALSE);
						}
						else{
							You("keep a tight grip on your weapon!");
						}
					}
					if(!rn2(10) && uarmf){
						Your("boots are sucked off!");
						optr = uarmf;
						if (donning(optr)) cancel_don();
						(void) Boots_off();
						freeinv(optr);
						(void) mpickobj(mon,optr,FALSE);
					}
					if(!rn2(6) && uarmg && !uwep){
						You_feel("%s pull on your gloves!",mon_nam(mon));
						if( rnd(130) > ACURR(A_STR)){
							Your("gloves are sucked off!");
							optr = uarmg;
							if (donning(optr)) cancel_don();
							(void) Gloves_off();
							freeinv(optr);
							(void) mpickobj(mon,optr,FALSE);
						}
						else You("keep your %s closed.", makeplural(body_part(HAND)));
					}
					if(!rn2(8) && uarms){
						You_feel("%s pull on your shield!",mon_nam(mon));
						if( rnd(150) > ACURR(A_STR)){
							Your("shield is sucked out of your grasp!");
							optr = uarms;
							if (donning(optr)) cancel_don();
							Shield_off();
							freeinv(optr);
							(void) mpickobj(mon,optr,FALSE);
						 }
						 else{
							You("keep a tight grip on your shield!");
						 }
					}
					if(!rn2(4) && uarmh){
						Your("helmet is sucked off!");
						optr = uarmh;
						if (donning(optr)) cancel_don();
						(void) Helmet_off();
						freeinv(optr);
						(void) mpickobj(mon,optr,FALSE);
					}
				}
			}
			mdamageu(mon, tmp);
		break;

	    case AD_CNCL:
		if (!rn2(3)) {
			(void) cancel_monst(&youmonst, (struct obj *)0, FALSE, TRUE, FALSE);
		}
		break;

	    case AD_BANI:
		if (!rn2(3)) {
			if (((u.uhave.amulet) && !u.freeplaymode) || CannotTeleport || (u.usteed && mon_has_amulet(u.usteed))) { pline("You shudder for a moment."); (void) safe_teleds_normalterrain(FALSE); break;}
			if (playerlevelportdisabled()) {
				pline("For some reason you resist the banishment!");
				break;
			}

			make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

			if (!u.banishmentbeam) {
				u.banishmentbeam = 1;
				nomul(-2, "being banished", FALSE); /* because it's not called until you get another turn... */
			}
			return(0);
		}
		break;

	  case AD_GLIB:

		/* hurt the player's hands --Amy */
		You("rip open your hands as you scrape along %s body!", s_suffix(mon_nam(mon)));
		incr_itimeout(&Glib, (tmp + 2) );

		break;
	  case AD_LUCK:

		pline("It feels like hurting this monster with that type of attack was a bad idea.");
		change_luck(-1);
		/* Yes I know it's different from ADOM, but NetHack is way better than ADOM anyway, so who cares? --Amy */

	    break;
	  case AD_DARK:

		if (Race_if(PM_PLAYER_NIBELUNG) && rn2(5)) break;
		if (uarms && uarms->oartifact == ART_DA_PELTA) break;
		/* darken your surroundings --Amy */
		pline("A maleficient darkness comes over you.");
		litroomlite(FALSE);

		break;
	  case AD_TLPT:
	  case AD_ABDC:

		    if(flags.verbose)
			Your("position suddenly seems very uncertain!");
		    teleX();
		break;

	    case AD_SWAP:

		if (mon->mspec_used) break;

		if (!rn2(5)) {

			int oldmx, oldmy;
			oldmx = mon->mx;
			oldmy = mon->my;
			mon->mspec_used = rnz(100);

			u.utrap = 0; /* A lucky escape */
			remove_monster(mon->mx, mon->my);
			place_monster(mon, u.ux, u.uy);
			teleds(oldmx, oldmy, FALSE);
			pline("The monster swapped positions with you!");
		}

		break;

	    case AD_TPTO:

		if ((level.flags.noteleport || Race_if(PM_STABILISATOR) || u.antitelespelltimeout) && !Race_if(PM_RODNEYAN) ) {
			pline("A mysterious force prevents you from teleporting!");
			break;
		}

		if ( ( (u.uhave.amulet && !u.freeplaymode && (u.amuletcompletelyimbued || !rn2(3))) || CannotTeleport || On_W_tower_level(&u.uz) || (u.usteed && mon_has_amulet(u.usteed)) ) ) {
			You_feel("disoriented for a moment.");
			break;
		}

		{
			int i, j, bd = 1;

			for(i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
				if (!isok(mon->mx + i, mon->my + j)) continue;

				if (teleok(mon->mx + i, mon->my + j, FALSE)) {
					teleds(mon->mx + i, mon->my + j, FALSE);
					pline("%s pulls you near!", Monnam(mon));
					i = 99; j = 99;
					break;
				}
			}
		}

		break;

	  case AD_DISP:
		You_feel("a strong force!");
			mdamageu(mon, tmp);
		pushplayer(FALSE);
		break;
	  case AD_DRIN:
	    if(!rn2(3)) {
		    if (ABASE(A_INT) <= ATTRABSMIN(A_INT)) {
			int lifesaved = 0;
			struct obj *wore_amulet = uamul;

			while(1) {
			    /* avoid looping on "die(y/n)?" */
			    if (lifesaved && (discover || wizard)) {
				if (wore_amulet && !uamul) {
				    /* used up AMULET_OF_LIFE_SAVING; still
				       subject to dying from brainlessness */
				    wore_amulet = 0;
				} else if (wizard) {
				    /* explicitly chose not to die;
				       arbitrarily boost intelligence */
				    ABASE(A_INT) = ATTRABSMIN(A_INT) + 2;
				    You_feel("like a scarecrow.");
				    break;
				}
			    }

				u.youaredead = 1;
				u.youarereallydead = 1;
			    if (lifesaved)
				pline("Unfortunately your brain is still gone.");
			    else
				Your("last thought fades away.");
			    killer = "brainlessness";
			    killer_format = KILLED_BY;
			    done(DIED);
			    if (wizard) {
				    u.youaredead = 0;
				    u.youarereallydead = 0;
			    }
			    lifesaved++;
			}
		    }

			if (!rn2(10)) (void) adjattrib(A_INT, -rnd(2), FALSE, TRUE);
			else if (!rn2(2)) (void) adjattrib(A_INT, -1, FALSE, TRUE);
			if (!rn2(issoviet ? 2 : 3)) forget_levels(rnd(issoviet ? 25 : 10), FALSE);	/* lose memory of 25% of levels */
			if (!rn2(issoviet ? 3 : 5)) forget_objects(rnd(issoviet ? 25 : 10), FALSE);	/* lose memory of 25% of objects */
			exercise(A_WIS, FALSE);
		    } else tmp = 0;

		break;
	  case AD_STON:
	    if (mhit || rathback) {		/* successful attack */
		long protector = attk_protection((int)aatyp);
		boolean barehanded = mhit & HIT_BODY ||
			mhit & HIT_UWEP && !uwep ||
			mhit & HIT_USWAPWEP && !uswapwep;

		/* hero using monsters' AT_MAGC attack is hitting hand to
		   hand rather than casting a spell */
		if (!rathback && (aatyp == AT_MAGC)) protector = W_ARMG;

		if (protector == 0L ||		/* no protection */
			(protector == W_ARMG && (!uarmg || FingerlessGloves) && barehanded) ||
			(protector == W_ARMF && !uarmf) ||
			(protector == W_ARMH && !uarmh) ||
			(protector == (W_ARMC|W_ARMG) && (!uarmc || !uarmg))) {
		if ((!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) &&
			    !(poly_when_stoned(youmonst.data) &&
				polymon(PM_STONE_GOLEM))) {
			/* You("turn to stone...");
			done_in_by(mon);
			return 2; */
			if (!Stoned) {
				if (Hallucination && rn2(10)) pline("Thankfully you are already stoned.");
				else {
					You("start turning to stone.");
					u.cnd_stoningcount++;
					Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
					delayed_killer = "bashing a petrifying monster";
				}
			}

/* damn it! You should be able to do SOMEthing against those things! Insta-stoning on touch just plain sucks, okay?*/
/* One wrongly pressed key is supposed to end it all? And the travelto command rarely finds the way?*/
/* I mean, come on! Nobody has the patience to sit through eons of using the travelto command! --Amy*/
		}
	      }
	    }
	    break;

	  case AD_EDGE:
	    if (mhit || rathback) {		/* successful attack */

		if (!Stone_resistance || !rn2(StrongStone_resistance ? 100 : 20)) {
			pline("The sharp-edged stone slits your entire body!");
			if (Upolyd) {u.mhmax--; if (u.mh > u.mhmax) u.mh = u.mhmax;}
			else {u.uhpmax--; if (u.uhp > u.uhpmax) u.uhp = u.uhpmax; }
		}

		long protector = attk_protection((int)aatyp);
		boolean barehanded = mhit & HIT_BODY ||
			mhit & HIT_UWEP && !uwep ||
			mhit & HIT_USWAPWEP && !uswapwep;

		/* hero using monsters' AT_MAGC attack is hitting hand to
		   hand rather than casting a spell */
		if (!rathback && (aatyp == AT_MAGC)) protector = W_ARMG;

		if (protector == 0L ||		/* no protection */
			(protector == W_ARMG && (!uarmg || FingerlessGloves) && barehanded) ||
			(protector == W_ARMF && !uarmf) ||
			(protector == W_ARMH && !uarmh) ||
			(protector == (W_ARMC|W_ARMG) && (!uarmc || !uarmg))) {
		if ((!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) &&
			    !(poly_when_stoned(youmonst.data) &&
				polymon(PM_STONE_GOLEM))) {
			/* You("turn to stone...");
			done_in_by(mon);
			return 2; */
			if (!Stoned) {
				if (Hallucination && rn2(10)) pline("Thankfully you are already stoned.");
				else {
					You("start turning to stone.");
					Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
					u.cnd_stoningcount++;
					delayed_killer = "bashing a petrifying monster";
				}
			}

		}
	      }
	    }
	    break;

	  case AD_RUST:

		if (ptr->mattk[i].aatyp == AT_RATH) {
			hurtarmor(AD_RUST);
		} else {

		    if(mhit && !mon->mcan) {
			if (aatyp == AT_KICK) {
			    if (uarmf)
				(void)rust_dmg(uarmf, xname(uarmf), 1, TRUE, &youmonst);
			} else if (aatyp == AT_WEAP || aatyp == AT_CLAW ||
				   aatyp == AT_MAGC || aatyp == AT_TUCH)
			    passive_obj(mon, target, &(ptr->mattk[i]));
		    }
		}
	    break;
	  case AD_CORR:

		if (ptr->mattk[i].aatyp == AT_RATH) {
			hurtarmor(AD_CORR);
		} else {

		    if(mhit && !mon->mcan) {
			if (aatyp == AT_KICK) {
			    if (uarmf)
				(void)rust_dmg(uarmf, xname(uarmf), 3, TRUE, &youmonst);
			} else if (aatyp == AT_WEAP || aatyp == AT_CLAW ||
				   aatyp == AT_MAGC || aatyp == AT_TUCH)
			    passive_obj(mon, target, &(ptr->mattk[i]));
		    }
		}
	    break;
	  case AD_WTHR:

		if (ptr->mattk[i].aatyp == AT_RATH) {
			witherarmor();
		} else {
		    if(mhit && !mon->mcan) {
			if (aatyp == AT_KICK) {
			    if (uarmf)
				(void)wither_dmg(uarmf, xname(uarmf), rn2(4), TRUE, &youmonst);
			} else if (aatyp == AT_WEAP || aatyp == AT_CLAW ||
				   aatyp == AT_MAGC || aatyp == AT_TUCH)
			    passive_obj(mon, target, &(ptr->mattk[i]));
		    }
		}
	    break;
	  case AD_DCAY:

		if (ptr->mattk[i].aatyp == AT_RATH) {
			hurtarmor(AD_DCAY);
		} else {

		    if(mhit && !mon->mcan) {
			if (aatyp == AT_KICK) {
			    if (uarmf)
				(void)rust_dmg(uarmf, xname(uarmf), 2, TRUE, &youmonst);
			} else if (aatyp == AT_WEAP || aatyp == AT_CLAW ||
				   aatyp == AT_MAGC || aatyp == AT_TUCH)
			    passive_obj(mon, target, &(ptr->mattk[i]));
		    }
		}
	    break;

	  case AD_FLAM:

		if (ptr->mattk[i].aatyp == AT_RATH) {
			hurtarmor(AD_FLAM);
		} else {

		    if(mhit && !mon->mcan) {
			if (aatyp == AT_KICK) {
			    if (uarmf)
				(void)rust_dmg(uarmf, xname(uarmf), 0, TRUE, &youmonst);
			} else if (aatyp == AT_WEAP || aatyp == AT_CLAW ||
				   aatyp == AT_MAGC || aatyp == AT_TUCH)
			    passive_obj(mon, target, &(ptr->mattk[i]));
		    }
		}
	    break;

	    case AD_HEAL:
		if (mon->mcan) {
		    break;
		}
		if(!uwep && (!uarmu || (uarmu && uarmu->oartifact == ART_GIVE_ME_STROKE__JO_ANNA)) && !uarm && !uarmh && !uarms && !uarmg && !uarmc && !uarmf) {
		    boolean goaway = FALSE;
		    pline("You are healed!");
		    reducesanity(1);
		    if (Upolyd) {
			u.mh += rnd(7);
			if (!rn2(7)) {
			    /* no upper limit necessary; effect is temporary */
			    u.mhmax++;
			    u.cnd_nursehealamount++;
			    if (!rn2(4)) goaway = TRUE;
			}
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		    } else {
			u.uhp += rnd(7);
			if (!rn2(10)) {
			    /* hard upper limit via nurse care: 25 * ulevel */
			    if (u.uhpmax < 5 * u.ulevel + d(2 * u.ulevel, 10)) {
				u.uhpmax++;
				u.cnd_nursehealamount++;
			    }
			    if (!rn2(3)) goaway = TRUE;
			}
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		    }
		    if (!rn2(3)) exercise(A_STR, TRUE);
		    if (!rn2(3)) exercise(A_CON, TRUE);
		    if (Sick) make_sick(0L, (char *) 0, FALSE, SICK_ALL);
		    flags.botl = 1;
		    if (goaway) {
			mon->mcan = TRUE; /* not "mongone" because of segfault danger --Amy */
			return 2;
		    } else if (!rn2(5)) {
			if (!tele_restrict(mon) || !rn2(2) ) (void) rloc(mon, FALSE); /* sometimes ignore noteleport --Amy */
			monflee(mon, d(3, 6), TRUE, FALSE);
			return 3;
		    }
		    tmp = 0;
		} else {
		    if (Role_if(PM_HEALER) || Race_if(PM_HERBALIST)) {
			tmp = 0;
		    } else {
			pline("Ouch!");
			mdamageu(mon, tmp);
		    }
		}
		break;

        case AD_CURS:
	  case AD_LITE:

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
				You_feel("a dark void in your head!");
			    attrcurse();
			}
			break;

	  case AD_SPC2:

			if (Psi_resist && rn2(StrongPsi_resist ? 100 : 20)) break;
			if (obsidianprotection()) break;
			pline("%s's corrupted mind backlashes your attack!", Monnam(mon) );

			switch (rnd(10)) {

				case 1:
				case 2:
				case 3:
					make_confused(HConfusion + tmp, FALSE);
					break;
				case 4:
				case 5:
				case 6:
					make_stunned(HStun + tmp, FALSE);
					break;
				case 7:
					make_confused(HConfusion + tmp, FALSE);
					make_stunned(HStun + tmp, FALSE);
					break;
				case 8:
					make_hallucinated(HHallucination + tmp, FALSE, 0L);
					break;
				case 9:
					make_feared(HFeared + tmp, FALSE);
					break;
				case 10:
					make_numbed(HNumbed + tmp, FALSE);
					break;
	
			}
			if (!rn2(200)) {
				forget(rnd(5), FALSE);
				pline("You forget some important things...");
			}
			if (!rn2(200)) {
				losexp("psionic drain", FALSE, TRUE);
			}
			if (!rn2(200)) {
				adjattrib(A_INT, -1, 1, TRUE);
				adjattrib(A_WIS, -1, 1, TRUE);
			}
			if (!rn2(200)) {
				pline("You scream in pain!");
				wake_nearby();
			}
			if (!rn2(200)) {
				badeffect();
			}
			if (!rn2(5)) increasesanity(rnz(5));

			mdamageu(mon, tmp);

		break;

	  case AD_SLOW:
		if (HFast && !defends(AD_SLOW, uwep) && !rn2(4)) {
		    u_slow_down();

		    if (Race_if(PM_SPIRIT) && !inertiaprotection() && !rn2(3)) {
			u.uprops[DEAC_FAST].intrinsic += ((tmp + 2) * 10);
			pline(u.inertia ? "That was a bad idea - your body struggles at your attempts to get it to move again..." : "That was a bad idea - your body lost the will to listen to your instructions...");
			u.inertia += (tmp + 2);
		    }

		}
		break;
        case AD_DRLI:
			if (!rn2(3) && (!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4) )  ) {
			    losexp("life drainage", FALSE, TRUE);
			}
			break;
        case AD_VAMP:
			if (!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4) ) {
			    losexp("life drainage", FALSE, TRUE);
			}
			break;

	  case AD_WEBS: 
		{
			struct trap *ttmp2 = maketrap(u.ux, u.uy, WEB, 0, FALSE);
			if (ttmp2) {
				pline("You're caught in a web!");
				dotrap(ttmp2, NOWEBMSG);
				if (u.usteed && u.utrap && !mayfalloffsteed()) {
				/* you, not steed, are trapped */
				dismount_steed(DISMOUNT_FELL);
				}
			}
		}
		/* Amy addition: sometimes, also make a random trap somewhere on the level :D */
		if (!rn2(issoviet ? 2 : 8)) makerandomtrap(FALSE);
		break;

	  case AD_TRAP:
		if (t_at(u.ux, u.uy) == 0) (void) maketrap(u.ux, u.uy, rndtrap(), 0, FALSE);
		else makerandomtrap(FALSE);

	  break;

	  case AD_DREN:
			if (!rn2(4)) drain_en(tmp);
			break;
	  case AD_LEGS:
			{ register long sideX = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
	
			pline("Your legs get grazed!");
			set_wounded_legs(sideX, HWounded_legs + rnd(60-ACURR(A_DEX)));
			exercise(A_STR, FALSE);
			exercise(A_DEX, FALSE);
			}
			break;
	  case AD_DETH:
		pline("Oh no, it uses the touch of death!");
		if (is_undead(youmonst.data)) {
		    /* Still does normal damage */
		    pline("Nothing seems to happen.");
		    break;
		}
		switch (rn2(20)) {
		case 19: /* case 18: case 17: */
		    if (!PlayerResistsDeathRays) {
			u.youaredead = 1;
			killer_format = KILLED_BY_AN;
			killer = "touch of death";
			done(DIED);
			u.youaredead = 0;
			tmp = 0;
			break;
		    } /* else FALLTHRU */
		default: /* case 16: ... case 5: */
		    You_feel("your life force draining away...");
			if (Antimagic || (Half_spell_damage && rn2(2) ) || (StrongHalf_spell_damage && rn2(2)) ) {
				shieldeff(u.ux, u.uy);
				tmp /= 2;
				if (StrongAntimagic && tmp > 1) tmp /= 2;
			}
			u.uhpmax -= tmp/2;
			if (u.uhpmax < 1) { /* discovered by maxlunar */
				u.youaredead = 1;
				u.uhpmax = 1;
				killer_format = KILLED_BY_AN;
				killer = "touch of death";
				done(DIED);
				u.youaredead = 0;
			}
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		    break;
		case 4: case 3: case 2: case 1: case 0:
		    if (Antimagic) shieldeff(u.ux, u.uy);
		    pline("Lucky for you, it didn't work!");
		    tmp = 0;
		    break;
		}
		break;
	  case AD_FAMN:
		pline("Your body shrivels!");
		exercise(A_CON, FALSE);
		if (!is_fainted()) morehungry(rnz(40));
		if (!is_fainted()) morehungry(rnz(40));
		morehungry(tmp+2);
		morehungry(tmp+2);
		/* plus the normal damage */
		break;

	    case AD_NPRO:
		if (!rn2(3)) {
			u.negativeprotection++;
			if (evilfriday && u.ublessed > 0) {
				u.ublessed -= 1;
				if (u.ublessed < 0) u.ublessed = 0;
			}
			You_feel("less protected!");
		}
		break;

	  case AD_SLIM:    
		if (!flaming(youmonst.data) && !Unchanging && !slime_on_touch(youmonst.data) ) {

		 if (!Slimed) {
		    You("don't feel very well.");
			make_slimed(100);
		    killer_format = KILLED_BY_AN;
		    static char kbuf[BUFSZ];
		    sprintf(kbuf, "slimed by attacking the wrong monster");
		    delayed_killer = kbuf;
		} else
		    pline("Yuck!");
		}

		break;
	  case AD_LITT:
		if (!flaming(youmonst.data) && !Unchanging && !slime_on_touch(youmonst.data) ) {

		 if (!Slimed) {
		    You("don't feel very well.");
			make_slimed(20);
		    killer_format = KILLED_BY_AN;
		    static char kbuf[BUFSZ];
		    sprintf(kbuf, "slimed by attacking the wrong monster");
		    delayed_killer = kbuf;
		} else
		    pline("Yuck!");
		}

		{
		    register struct obj *littX, *littX2;
		    for (littX = invent; littX; littX = littX2) {
		      littX2 = littX->nobj;
			if (!rn2(StrongAcid_resistance ? 1000 : Acid_resistance ? 100 : 10)) rust_dmg(littX, xname(littX), 3, TRUE, &youmonst);
		    }
		}

		break;
	  case AD_CALM:	/* KMH -- koala attack */
		    docalm();
		break;

	    case AD_FREN:
		pline("You feel angry at yourself for making the stupid mistake of attacking such a monster!");
			if (u.berserktime) {
			    if (!obsidianprotection()) switch (rn2(11)) {
			    case 0: difeasemu(mon->data);
				    break;
			    case 1: make_blinded(Blinded + tmp, TRUE);
				    break;
			    case 2: if (!Confusion)
					You("suddenly feel %s.",
					    FunnyHallu ? "trippy" : "confused");
				    make_confused(HConfusion + tmp, TRUE);
				    break;
			    case 3: make_stunned(HStun + tmp, TRUE);
				    break;
			    case 4: make_numbed(HNumbed + tmp, TRUE);
				    break;
			    case 5: make_frozen(HFrozen + tmp, TRUE);
				    break;
			    case 6: make_burned(HBurned + tmp, TRUE);
				    break;
			    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
				    break;
			    case 8: (void) make_hallucinated(HHallucination + tmp, TRUE, 0L);
				    break;
			    case 9: make_feared(HFeared + tmp, TRUE);
				    break;
			    case 10: make_dimmed(HDimmed + tmp, TRUE);
				    break;
			    }

			} else u.berserktime = tmp;

		break;

	  case AD_POLY:
		if (!Unchanging && !Antimagic && !(Upolyd && rn2(5))) {
		    if (flags.verbose)
			You("suddenly feel very unstable!");
			u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += 1;
			u.polyattackhack = TRUE;
		    polyself(FALSE);
			u.polyattackhack = FALSE;
		}
		break;
	  case AD_CHAO:
		You("are hit by chaotic forces!");
		if (!Unchanging && !Antimagic && !(Upolyd && rn2(5))) {
		    if (flags.verbose)
			You("suddenly feel very unstable!");
			u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += 1;
			u.polyattackhack = TRUE;
		    polyself(FALSE);
			u.polyattackhack = FALSE;
		}

		    if (!obsidianprotection()) switch (rn2(11)) {
		    case 0: difeasemu(mon->data);
			    break;
		    case 1: make_blinded(Blinded + tmp, TRUE);
			    break;
		    case 2: if (!Confusion)
				You("suddenly feel %s.",
				    FunnyHallu ? "trippy" : "confused");
			    make_confused(HConfusion + tmp, TRUE);
			    break;
		    case 3: make_stunned(HStun + tmp, TRUE);
			    break;
		    case 4: make_numbed(HNumbed + tmp, TRUE);
			    break;
		    case 5: make_frozen(HFrozen + tmp, TRUE);
			    break;
		    case 6: make_burned(HBurned + tmp, TRUE);
			    break;
		    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
			    break;
		    case 8: (void) make_hallucinated(HHallucination + tmp, TRUE, 0L);
			    break;
		    case 9: make_feared(HFeared + tmp, TRUE);
			    break;
		    case 10: make_dimmed(HDimmed + tmp, TRUE);
			    break;
		    }

		break;
	  case AD_MAGM:
	    /* wrath of gods for attacking Oracle */
	    if(Antimagic && !Race_if(PM_KUTAR) && rn2(StrongAntimagic ? 5 : 3)) {
		shieldeff(u.ux, u.uy);
		pline("A hail of magic missiles narrowly misses you!");
	    } else {
		You("are hit by magic missiles appearing from thin air!");
		mdamageu(mon, tmp);
	    }
	    break;
        case AD_DISE:
		if (rn2(2)) break; /* 50% chance if AD_DISE, 100% if AD_PEST --Amy */
        case AD_PEST:
		  pline("You got hit by botox spores!");
		  (void) difeasemu(mon->data); /* plus the normal damage */
	        break;

        case AD_CAST:
        case AD_CLRC:
        case AD_SPEL:
		if (malive) castmu(mon, &ptr->mattk[i], TRUE, TRUE);
		break;

        case AD_VOMT:

		if (!rn2(StrongSick_resistance ? 100 : 10) || !Sick_resistance) {
			if (!Vomiting) {
				make_vomiting(Vomiting+d(10,4), TRUE);
				pline("You feel nauseated.");
				if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10);
			} else if (!rn2(2)) difeasemu(mon->data);
		}

	  	break;

	  default:
	    break;
	}

/*	These only affect you if they still live */
/* well, since most of them are sessile, and it's actually quite cowardly to attack a helpless sessile monster... */
/* it was changed so these effects will still apply if the monster got killed, but not if it was cancelled --Amy */

	if(/*malive && */!mon->mcan && rn2(3)) {

	    switch(atttypC) {

	      case AD_DRST:

		if (uarmh && itemhasappearance(uarmh, APP_FILTERED_HELMET) && !rn2(2) ) break;

		if (RngeGasFiltering && !rn2(2)) break;

	       if (!Strangled && !Breathless) {
		 pline("You inhale a cloud of spores!");
		 poisoned("spores", A_STR, "spore cloud", 30);
	       } else {
		 pline("A cloud of spores surrounds you!");
		 if (!rn2(StrongMagical_breathing ? 5 : 2)) poisoned("spores", A_STR, "spore cloud", 30);
	       }
	      break;
	      case AD_DRDX:

		if (uarmh && itemhasappearance(uarmh, APP_FILTERED_HELMET) && !rn2(2) ) break;

		if (RngeGasFiltering && !rn2(2)) break;

	       if (!Strangled && !Breathless) {
		 pline("You inhale a cloud of spores!");
		 poisoned("spores", A_DEX, "spore cloud", 30);
	       } else {
		 pline("A cloud of spores surrounds you!");
		 if (!rn2(StrongMagical_breathing ? 5 : 2)) poisoned("spores", A_DEX, "spore cloud", 30);
	       }
	      break;
	      case AD_DRCO:

		if (uarmh && itemhasappearance(uarmh, APP_FILTERED_HELMET) && !rn2(2) ) break;

		if (RngeGasFiltering && !rn2(2)) break;

	       if (!Strangled && !Breathless) {
		 pline("You inhale a cloud of spores!");
		 poisoned("spores", A_CON, "spore cloud", 30);
	       } else {
		 pline("A cloud of spores surrounds you!");
		 if (!rn2(StrongMagical_breathing ? 5 : 2)) poisoned("spores", A_CON, "spore cloud", 30);
	       }
	      break;
	      case AD_WISD:

		if (uarmh && itemhasappearance(uarmh, APP_FILTERED_HELMET) && !rn2(2) ) break;

		if (RngeGasFiltering && !rn2(2)) break;

	       if (!Strangled && !Breathless) {
		 pline("You inhale a cloud of spores!");
		 poisoned("spores", A_WIS, "spore cloud", 30);
	       } else {
		 pline("A cloud of spores surrounds you!");
		 if (!rn2(StrongMagical_breathing ? 5 : 2)) poisoned("spores", A_WIS, "spore cloud", 30);
	       }
	      break;
	      case AD_DRCH:

		if (uarmh && itemhasappearance(uarmh, APP_FILTERED_HELMET) && !rn2(2) ) break;

		if (RngeGasFiltering && !rn2(2)) break;

	       if (!Strangled && !Breathless) {
		 pline("You inhale a cloud of spores!");
		 poisoned("spores", A_CHA, "spore cloud", 30);
	       } else {
		 pline("A cloud of spores surrounds you!");
		 if (!rn2(StrongMagical_breathing ? 5 : 2)) poisoned("spores", A_CHA, "spore cloud", 30);
	       }
	      break;
	      case AD_POIS:

		if (uarmh && itemhasappearance(uarmh, APP_FILTERED_HELMET) && !rn2(2) ) break;

		if (RngeGasFiltering && !rn2(2)) break;

	       if (!Strangled && !Breathless) {
		 pline("You inhale a cloud of spores!");
		 poisoned("spores", rn2(A_MAX), "spore cloud", 30);
	       } else {
		 pline("A cloud of spores surrounds you!");
		 if (!rn2(StrongMagical_breathing ? 5 : 2)) poisoned("spores", rn2(A_MAX), "spore cloud", 30);
	       }
	      break;
	    case AD_VENO:
		if (uarmh && itemhasappearance(uarmh, APP_FILTERED_HELMET) && !rn2(3) ) break;
		if (chromeprotection()) break;

		if (RngeGasFiltering && !rn2(3)) break;

	       if (!Strangled && !Breathless) {
			pline("You inhale a cloud of superpoisonous gas!");
		} else {
			pline("A cloud of superpoisonous gas surrounds you!");
		}

	       if ((!Strangled && !Breathless) || rn2(StrongMagical_breathing ? 2 : 3)) {
		if (!Poison_resistance) pline("You're badly poisoned!");
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_STR, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_DEX, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_CON, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_INT, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_WIS, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_CHA, -rnd(2), FALSE, TRUE);
		 poisoned("gas", rn2(A_MAX), "superpoisonous gas", 30);
		if (isevilvariant || !rn2(issoviet ? 2 : 20)) (void)destroy_item(POTION_CLASS, AD_VENO);
		if (isevilvariant || !rn2(issoviet ? 2 : 20)) (void)destroy_item(FOOD_CLASS, AD_VENO);
		}

		break;

	      case AD_SAMU:
			You("get hurt by %s retaliation!", s_suffix(mon_nam(mon)));	
			mdamageu(mon, tmp);
			if (malive && !rn2(10)) stealamulet(mon);
			break;

	      case AD_STCK:
	      case AD_WRAP:
			if (malive && !u.ustuck && !sticks(youmonst.data)) {
				setustuck(mon);
				pline("%s sticks to you!", Monnam(mon));
				if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			}
			break;
	      case AD_SGLD:
			if (!malive) break;
			if (rn2(10)) {stealgold(mon);
				break;
			}
			/* fall through --Amy */

	      case AD_SITM:	/* for now these are the same */
	      case AD_SEDU:
	      case AD_SSEX:
			if (!malive) break;

			if (StealersActive) {
				You_feel("a tug on your backpack!");
				buf[0] = '\0';
				switch (steal(mon, buf, atttypC == AD_SEDU ? TRUE : FALSE, FALSE)) {
			  case -1:
				return 2;
			  case 0:
				break;
			  default:
				if ( !tele_restrict(mon))
				    (void) rloc(mon, FALSE);
				monflee(mon, rnd(10), FALSE, FALSE);
				return 3;
				};

			} else if ( issoviet || (  (rnd(100) > ACURR(A_CHA)) && malive && ( ((mon->female) && !flags.female && !rn2(2) ) || ((!mon->female) && flags.female && !rn2(3) ) || 
				((mon->female) && flags.female && !rn2(5) ) || ((!mon->female) && !flags.female && !rn2(5) ) ) )
	
			) 		{
			You_feel("a tug on your backpack!");
			buf[0] = '\0';
				switch (steal(mon, buf, atttypC == AD_SEDU ? TRUE : FALSE, FALSE)) {
			  case -1:
				return 2;
			  case 0:
				break;
			  default:
				if ( !tele_restrict(mon))
				    (void) rloc(mon, FALSE);
				monflee(mon, rnd(10), FALSE, FALSE);
				return 3;
				};
			}
			break;

	    case AD_STTP:

		if (!rn2(3)) {
		if (flags.soundok) You_hear("a scattering sound!");
		if (invent) {
		    int itemportchance = 10 + rn2(21);
		    for (otmpi = invent; otmpi; otmpi = otmpii) {

		      otmpii = otmpi->nobj;

			if (!rn2(itemportchance) && !(objects[otmpi->otyp].oc_material == MT_BONE && rn2(10)) && !stack_too_big(otmpi) ) {

				if (otmpi->owornmask & W_ARMOR) {
				    if (otmpi == uskin) {
					skinback(TRUE);		/* uarm = uskin; uskin = 0; */
				    }
				    if (otmpi == uarm) (void) Armor_off();
				    else if (otmpi == uarmc) (void) Cloak_off();
				    else if (otmpi == uarmf) (void) Boots_off();
				    else if (otmpi == uarmg) (void) Gloves_off();
				    else if (otmpi == uarmh) (void) Helmet_off();
				    else if (otmpi == uarms) (void) Shield_off();
				    else if (otmpi == uarmu) (void) Shirt_off();
				    /* catchall -- should never happen */
				    else setworn((struct obj *)0, otmpi ->owornmask & W_ARMOR);
				} else if (otmpi ->owornmask & W_AMUL) {
				    Amulet_off();
				} else if (otmpi ->owornmask & W_IMPLANT) {
				    Implant_off();
				} else if (otmpi ->owornmask & W_RING) {
				    Ring_gone(otmpi);
				} else if (otmpi ->owornmask & W_TOOL) {
				    Blindf_off(otmpi);
				} else if (otmpi ->owornmask & (W_WEP|W_SWAPWEP|W_QUIVER)) {
				    if (otmpi == uwep)
					uwepgone();
				    if (otmpi == uswapwep)
					uswapwepgone();
				    if (otmpi == uquiver)
					uqwepgone();
				}

				if (otmpi->owornmask & (W_BALL|W_CHAIN)) {
				    unpunish();
				} else if (otmpi->owornmask) {
				/* catchall */
				    setnotworn(otmpi);
				}

				dropx(otmpi);
			      if (otmpi->where == OBJ_FLOOR) rloco(otmpi);
				u.cnd_itemportcount++;
			}

		    }
		}

		}
		break;

	    case AD_DEPR:
		You_feel("depressed...");
		if (!rn2(2)) {

		    switch(rnd(20)) {
		    case 1:
			if (!Unchanging && !Antimagic) {
				You("undergo a freakish metamorphosis!");
				u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += 1;
				u.polyattackhack = TRUE;
			      polyself(FALSE);
				u.polyattackhack = FALSE;
			}
			break;
		    case 2:
			You("need reboot.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Eto poshel na khuy vverkh. No chto zhe vy ozhidali? Igra, v kotoruyu vy mozhete legko vyigrat'? Durak!" : "DUEUEDUET!");
			if (!Race_if(PM_UNGENOMOLD)) newman();
			else {
				u.polyattackhack = TRUE;
				polyself(FALSE);
				u.polyattackhack = FALSE;
			}
			break;
		    case 3: case 4:
			if(!rn2(4) && u.ulycn == NON_PM &&
				!Protection_from_shape_changers &&
				!is_were(youmonst.data) &&
				!defends(AD_WERE,uwep)) {
			    You_feel("feverish.");
			    exercise(A_CON, FALSE);
			    u.ulycn = PM_WERECOW;
			    u.cnd_lycanthropecount++;
			} else {
				if (multi >= 0) {
				    if (Sleep_resistance && rn2(StrongSleep_resistance ? 20 : 5)) break;
				    fall_asleep(-rnd(10), TRUE);
				    if (Blind) You("are put to sleep!");
				    else You("are put to sleep by %s!", mon_nam(mon));
				}
			}
			break;
		    case 5: case 6:
			if (!u.ustuck && !sticks(youmonst.data)) {
				setustuck(mon);
				pline("%s grabs you!", Monnam(mon));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Tam net vykhoda! Ty predatel' russkogo naroda i, sledovatel'no, budut zaderzhany navsegda!" : "Wroa!");
				if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			}
			break;
		    case 7:
		    case 8:
			Your("position suddenly seems very uncertain!");
			teleX();
			break;
		    case 9:
			u_slow_down();
			break;
		    case 10:

			if (ptr->mattk[i].aatyp == AT_RATH) {
				hurtarmor(AD_RUST);
			}
		    if(mhit) {
			if (aatyp == AT_KICK) {
			    if (uarmf)
				(void)rust_dmg(uarmf, xname(uarmf), 1, TRUE, &youmonst);
			} else if (aatyp == AT_WEAP || aatyp == AT_CLAW ||
				   aatyp == AT_MAGC || aatyp == AT_TUCH) {
				mdat2 = &mons[PM_RUST_MONSTER];
				attspc = &mdat2->mattk[2];
			    passive_obj(mon, target, attspc);
				}
		    }
		    break;

		    case 11:

			if (ptr->mattk[i].aatyp == AT_RATH) {
				hurtarmor(AD_DCAY);
			}
		    if(mhit) {
			if (aatyp == AT_KICK) {
			    if (uarmf)
				(void)rust_dmg(uarmf, xname(uarmf), 1, TRUE, &youmonst);
			} else if (aatyp == AT_WEAP || aatyp == AT_CLAW ||
				   aatyp == AT_MAGC || aatyp == AT_TUCH) {
				mdat2 = &mons[PM_ROT_MONSTER];
				attspc = &mdat2->mattk[2];
			    passive_obj(mon, target, attspc);
				}
		    }
		    break;

		    case 12:

			if (ptr->mattk[i].aatyp == AT_RATH) {
				hurtarmor(AD_CORR);
			}
		    if(mhit) {
			if (aatyp == AT_KICK) {
			    if (uarmf)
				(void)rust_dmg(uarmf, xname(uarmf), 1, TRUE, &youmonst);
			} else if (aatyp == AT_WEAP || aatyp == AT_CLAW ||
				   aatyp == AT_MAGC || aatyp == AT_TUCH) {
				mdat2 = &mons[PM_ACID_MONSTER];
				attspc = &mdat2->mattk[2];
			    passive_obj(mon, target, attspc);
				}
		    }
		    break;

		    case 13:
			if (multi >= 0) {
			    if (Free_action && rn2(StrongFree_action ? 100 : 20)) {
				You("momentarily stiffen.");            
			    } else {
				if (Blind) You("are frozen!");
				else You("are frozen by %s!", mon_nam(mon));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
				nomovemsg = 0;	/* default: "you can move again" */
				if (isstunfish) nomul(-rnz(5), "paralyzed by a monster attack", TRUE);
				else nomul(-rnd(5), "paralyzed by a monster attack", TRUE);
				exercise(A_DEX, FALSE);
			    }
			}
			break;
		    case 14:
			if (FunnyHallu)
				pline("What a groovy feeling!");
			else
				You(Blind ? "%s and get dizzy..." :
					 "%s and your vision blurs...",
					    stagger(youmonst.data, "stagger"));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Imet' delo s effektami statusa ili sdat'sya!" : "Wrueue-ue-e-ue-e-ue-e...");
			hallutime = rn1(7, 16);
			make_stunned(HStun + hallutime + tmp, FALSE);
			(void) make_hallucinated(HHallucination + hallutime + tmp,TRUE,0L);
			break;
		    case 15:
			if(!Blind)
				Your("vision bugged.");
			hallutime += rn1(10, 25);
			hallutime += rn1(10, 25);
			(void) make_hallucinated(HHallucination + hallutime + tmp + tmp,TRUE,0L);
			break;
		    case 16:
			if(!Blind)
				Your("vision turns to screen saver.");
			hallutime += rn1(10, 25);
			(void) make_hallucinated(HHallucination + hallutime + tmp,TRUE,0L);
			break;
		    case 17:
			{
			    struct obj *obj = some_armor(&youmonst);

			    if (obj && drain_item(obj)) {
				Your("%s less effective.", aobjnam(obj, "seem"));
				u.cnd_disenchantamount++;
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
			    }
			}
			break;
		    default:
			    if(Confusion)
				 You("are getting even more confused.");
			    else You("are getting confused.");
			    make_confused(HConfusion + tmp, FALSE);
			break;
		    }
		    exercise(A_INT, FALSE);

		}
		break;

	    case AD_WRAT:
		You_feel("your life draining away!");

		if(u.uen < 1) {
		    You_feel("less energised!");
		    u.uenmax -= rn1(10,10);
		    if(u.uenmax < 0) u.uenmax = 0;
		} else if(u.uen <= 10) {
		    You_feel("your magical energy dwindle to nothing!");
		    u.uen = 0;
		} else {
		    You_feel("your magical energy dwindling rapidly!");
		    u.uen /= 2;
		}

		break;

	    case AD_LAZY: /* laziness attack; do lots of nasty things at random */
		if(!rn2(2)) {
		    You_feel("momentarily lethargic.");
		    break;
		}
		You_feel("very apathetic...");
		switch(rn2(7)) {
		    case 0: /* destroy certain things */

			if (ptr->mattk[i].aatyp == AT_RATH) {
				witherarmor();
			}
		    if(mhit) {
			if (aatyp == AT_KICK) {
			    if (uarmf)
				(void)rust_dmg(uarmf, xname(uarmf), 1, TRUE, &youmonst);
			} else if (aatyp == AT_WEAP || aatyp == AT_CLAW ||
				   aatyp == AT_MAGC || aatyp == AT_TUCH) {
				mdat2 = &mons[PM_WITHER_MONSTER];
				attspc = &mdat2->mattk[2];
			    passive_obj(mon, target, attspc);
				}
		    }
		    break;

		    case 1: /* sleep */
			if (multi >= 0) {
			    if (Sleep_resistance && rn2(StrongSleep_resistance ? 20 : 5)) {pline("You yawn."); break;}
			    fall_asleep(-rnd(10), TRUE);
			    if (Blind) You("are put to sleep!");
			    else You("are put to sleep by %s!", mon_nam(mon));
			}
			break;
		    case 2: /* paralyse */
			if (multi >= 0) {
			    if (Free_action && rn2(StrongFree_action ? 100 : 20)) {
				You("momentarily stiffen.");            
			    } else {
				if (Blind) You("are frozen!");
				else You("are frozen by %s!", mon_nam(mon));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
				nomovemsg = 0;	/* default: "you can move again" */
				if (isstunfish) nomul(-rnz(5), "paralyzed by a monster attack", TRUE);
				else nomul(-rnd(5), "paralyzed by a monster attack", TRUE);
				exercise(A_DEX, FALSE);
			    }
			}
			break;
		    case 3: /* slow */
			if(HFast)  u_slow_down();
			else You("pause momentarily.");
			break;
		    case 4: /* drain Dex */
			adjattrib(A_DEX, -rn1(1,1), 0, TRUE);
			break;
		    case 5: /* steal teleportitis */
			if(HTeleportation & INTRINSIC) {
			      HTeleportation &= ~INTRINSIC;
			}
	 		if (HTeleportation & TIMEOUT) {
				HTeleportation &= ~TIMEOUT;
			}
			if(HTeleport_control & INTRINSIC) {
			      HTeleport_control &= ~INTRINSIC;
			}
	 		if (HTeleport_control & TIMEOUT) {
				HTeleport_control &= ~TIMEOUT;
			}
		      You("don't feel in the mood for jumping around.");
			break;
		    case 6: /* steal sleep resistance */
			if(HSleep_resistance & INTRINSIC) {
				HSleep_resistance &= ~INTRINSIC;
			} 
			if(HSleep_resistance & TIMEOUT) {
				HSleep_resistance &= ~TIMEOUT;
			} 
			You_feel("like you could use a nap.");
			break;
		}
		break;

	    case AD_DFOO:
	      You_feel("physically and mentally weaker!");
		if (!rn2(3)) {
		    sprintf(buf, "%s body",
			    s_suffix(Monnam(mon)));
		    poisoned(buf, rn2(A_MAX), mon->data->mname, 30);
		}
		if (!rn2(4)) {
			You_feel("drained...");
			u.uhpmax -= rn1(10,10);
			if (u.uhpmax < 1) u.uhpmax = 1;
			if(u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		}
		if (!rn2(4)) {
			You_feel("less energised!");
			u.uenmax -= rn1(10,10);
			if (u.uenmax < 0) u.uenmax = 0;
			if(u.uen > u.uenmax) u.uen = u.uenmax;
		}
		if (!rn2(4)) {
			if(!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4) )
			    losexp("life drainage", FALSE, TRUE);
			else You_feel("woozy for an instant, but shrug it off.");
		}

		break;

	      case AD_SLEE:

		    if (Sleep_resistance && rn2(StrongSleep_resistance ? 20 : 5)) {
 			    You("yawn.");
			break;
			}
		    fall_asleep(-rnd(10), TRUE);
		    You("suddenly feel very tired!");

	      break;
	    case AD_BLND:
		    if (!Blind) pline("You are blinded by a flash of light!");
		    make_blinded(Blinded+(long)tmp,FALSE);
		break;
	    case AD_DISN:
		if (!rn2(10))  {
		if (Disint_resistance && rn2(StrongDisint_resistance ? 1000 : 100) && !(evilfriday && (uarms || uarmc || uarm || uarmu))) {
		    You("are mildly shaked.");
		    break;
            } else if (Invulnerable || ((PlayerInBlockHeels || PlayerInWedgeHeels) && tech_inuse(T_EXTREME_STURDINESS) && !rn2(2) ) || (Stoned_chiller && Stoned && !(u.stonedchilltimer) && !rn2(3)) ) {
                pline("You are unharmed!");
                break;
		} else if (uarms) {
			/* destroy shield; other possessions are safe */
			if (!(EDisint_resistance & W_ARMS) && !(itemsurvivedestruction(uarms, 12)) ) (void) destroy_arm(uarms);
			break;
		} else if (uarmc) {
			/* destroy cloak; other possessions are safe */
			if (!(EDisint_resistance & W_ARMC) && !(itemsurvivedestruction(uarmc, 12)) ) (void) destroy_arm(uarmc);
			break;
		} else if (uarm) {
			/* destroy suit */
			if (!(EDisint_resistance & W_ARM) && !(itemsurvivedestruction(uarm, 12)) ) (void) destroy_arm(uarm);
			break;
		} else if (uarmu) {
			/* destroy shirt */
			if (!(EDisint_resistance & W_ARMU) && !(itemsurvivedestruction(uarmu, 12)) ) (void) destroy_arm(uarmu);
			break;
		} else if (u.uhpmax > 20) {
			u.uhpmax -= rnd(20);
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			losehp(rnz(100 + level_difficulty()), "being so stupid and attacking the wrong monster", KILLED_BY);
			break;
		} else {
			u.youaredead = 1;
			done(DIED);
			u.youaredead = 0;
		}
	    return (malive | mhit); /* lifesaved */

		}
	      break;

	    case AD_VAPO:

			pline("Attention, %s - if you continue attacking this monster you will be vaporized.", playeraliasname);
			if (!Disint_resistance) tmp *= 3;
		    mdamageu(mon, tmp);

		if (!rn2(10))  {
		if (Disint_resistance && rn2(StrongDisint_resistance ? 1000 : 100) && !(evilfriday && (uarms || uarmc || uarm || uarmu))) {
		    You("are mildly shaked.");
		    break;
            } else if (Invulnerable || ((PlayerInBlockHeels || PlayerInWedgeHeels) && tech_inuse(T_EXTREME_STURDINESS) && !rn2(2) ) || (Stoned_chiller && Stoned && !(u.stonedchilltimer) && !rn2(3)) ) {
                pline("You are unharmed!");
                break;
		} else if (uarms) {
			/* destroy shield; other possessions are safe */
			if (!(EDisint_resistance & W_ARMS) && !(itemsurvivedestruction(uarms, 12)) ) (void) destroy_arm(uarms);
			break;
		} else if (uarmc) {
			/* destroy cloak; other possessions are safe */
			if (!(EDisint_resistance & W_ARMC) && !(itemsurvivedestruction(uarmc, 12)) ) (void) destroy_arm(uarmc);
			break;
		} else if (uarm) {
			/* destroy suit */
			if (!(EDisint_resistance & W_ARM) && !(itemsurvivedestruction(uarm, 12)) ) (void) destroy_arm(uarm);
			break;
		} else if (uarmu) {
			/* destroy shirt */
			if (!(EDisint_resistance & W_ARMU) && !(itemsurvivedestruction(uarmu, 12)) ) (void) destroy_arm(uarmu);
			break;
		} else if (u.uhpmax > 20) {
			u.uhpmax -= rnd(20);
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			losehp(rnz(100 + level_difficulty()), "stupidly self-vaporizing", KILLED_BY);
			break;
		} else {
			u.youaredead = 1;
			done(DIED);
			u.youaredead = 0;
		}
	    return (malive | mhit); /* lifesaved */

		}
	      break;

	      case AD_PLYS:
		if(ptr == &mons[PM_FLOATING_EYE]) {
		    if (!canseemon(mon)) {
			break;
		    }
		    if(mon->mcansee) {
			if (ureflects("%s gaze is reflected by your %s.",
				    s_suffix(Monnam(mon))))
			    ;
			else if (Free_action && rn2(StrongFree_action ? 100 : 20))
			    You("momentarily stiffen under %s gaze!",
				    s_suffix(mon_nam(mon)));
			else {
			    You("are frozen by %s gaze!",
				  s_suffix(mon_nam(mon)));
			    nomul(-tmp, "paralyzed by a monster's passive gaze", TRUE);
			}
		    } else {
			pline("%s cannot defend itself.",
				Adjmonnam(mon,"blind"));
			if(!rn2(500)) change_luck(-1);
		    }
		} else if (Free_action && rn2(StrongFree_action ? 100 : 20)) {
		    You("momentarily stiffen.");
		} else { /* gelatinous cube */
		    You("are frozen by %s!", mon_nam(mon));
	    	    nomovemsg = 0;	/* default: "you can move again" */
		    nomul(-tmp, "paralyzed by a monster's passive attack", TRUE);
		    exercise(A_DEX, FALSE);
		}
		break;
	      case AD_COLD:		/* brown mold or blue jelly */

		if (Race_if(PM_GAVIL)) tmp *= 2;
		if (Race_if(PM_HYPOTHERMIC)) tmp *= 3;

		if((Cold_resistance && rn2(StrongCold_resistance ? 20 : 5)) || ColdImmunity) {
			shieldeff(u.ux, u.uy);
			You_feel("a mild chill.");
			ugolemeffects(AD_COLD, tmp);
			break;
		}
		You("are suddenly very cold!");
		mdamageu(mon, tmp);
		/* monster gets stronger with your heat! */
		if(malive) {
		    mon->mhp += tmp / 2;
		    if (mon->mhpmax < mon->mhp) mon->mhpmax = mon->mhp;
			/* at a certain point, the monster will reproduce! */
		    if(mon->mhpmax > ((int) (mon->m_lev+1) * 8) && !rn2(5) ) /* prevent easy farming --Amy */
			(void)split_mon(mon, &youmonst);
		}
		break;
	      case AD_STUN:		/* specifically yellow mold */
		    make_stunned(HStun + (long)tmp, TRUE);
		break;
	      case AD_NUMB:
		    make_numbed(HNumbed + (long)tmp, TRUE);
		break;
	      case AD_FRZE:
		    make_frozen(HFrozen + (long)tmp, TRUE);
		break;
	      case AD_ICEB:
			You("are hit by a barrage of ice blocks!");
			if (issoviet) pline("BWAR KHAR (gryaznyy smekh) on on on kha kha kha!");
			if (Race_if(PM_GAVIL)) tmp *= 2;
			if (Race_if(PM_HYPOTHERMIC)) tmp *= 3;
			make_frozen(HFrozen + (long)tmp, TRUE);
			if (isevilvariant || !rn2(issoviet ? 2 : Race_if(PM_GAVIL) ? 2 : Race_if(PM_HYPOTHERMIC) ? 2 : 10)) {
				destroy_item(POTION_CLASS, AD_COLD);
			}
			if ((!rn2(StrongCold_resistance ? 20 : 5) || !Cold_resistance) && !ColdImmunity) mdamageu(mon, tmp);
		break;
	      case AD_FEAR:
		    make_feared(HFeared + (long)tmp, TRUE);
		break;
		case AD_SANI:
			increasesanity(tmp);
		break;

	      case AD_INSA:
		    make_feared(HFeared + (long)tmp, TRUE);
		    make_stunned(HStun + (long)tmp, TRUE);
		    make_confused(HConfusion + (long)tmp, TRUE);
		    increasesanity(1);
		break;
	      case AD_BURN:
		    make_burned(HBurned + (long)tmp, TRUE);
		break;
	      case AD_DIMN:
		    make_dimmed(HDimmed + (long)tmp, TRUE);
		break;
	      case AD_CONF:
			 You_feel("confused!");
		    make_confused(HConfusion + (long)tmp, TRUE);
		break;
	      case AD_HALU:
			 You("sniff some psychoactive substances!");
		    make_hallucinated(HHallucination + (long)tmp, TRUE, 0L);
		break;
	      case AD_FIRE:
			if (Race_if(PM_LOWER_ENT)) tmp *= 2;
		    if((Fire_resistance && rn2(StrongFire_resistance ? 20 : 5)) || FireImmunity) {
			shieldeff(u.ux, u.uy);
			You_feel("mildly warm.");
			ugolemeffects(AD_FIRE, tmp);
			break;
		    }
		    You("are suddenly very hot!");
		    mdamageu(mon, tmp);
		break;
	      case AD_LAVA:

			if (ptr->mattk[i].aatyp == AT_RATH) {
				hurtarmor(AD_LAVA);
			}

		    if(mhit && !mon->mcan) {
			if (aatyp == AT_KICK) {
			    if (uarmf)
				(void)rust_dmg(uarmf, xname(uarmf), 0, TRUE, &youmonst);
			} else if (aatyp == AT_WEAP || aatyp == AT_CLAW ||
				   aatyp == AT_MAGC || aatyp == AT_TUCH)
			    passive_obj(mon, target, &(ptr->mattk[i]));
		    }

			if (Race_if(PM_LOWER_ENT)) tmp *= 2;

		    if((Fire_resistance && rn2(StrongFire_resistance ? 20 : 5)) || FireImmunity) {
			shieldeff(u.ux, u.uy);
			You_feel("quite warm.");
			ugolemeffects(AD_FIRE, tmp);
			break;
		    }
		    You("are suddenly extremely hot!");
		    mdamageu(mon, tmp);
		break;
	      case AD_ELEC:
		if((Shock_resistance && rn2(StrongShock_resistance ? 20 : 5)) || ShockImmunity) {
		    shieldeff(u.ux, u.uy);
		    You_feel("a mild tingle.");
		    ugolemeffects(AD_ELEC, tmp);
		    break;
		}
		You("are jolted with electricity!");
		mdamageu(mon, tmp);
		break;

	      case AD_AXUS:
		    if((Fire_resistance && rn2(StrongFire_resistance ? 20 : 5)) || FireImmunity) {
			shieldeff(u.ux, u.uy);
			You_feel("mildly warm.");
			ugolemeffects(AD_FIRE, tmp);
			if (tmp >= 4) tmp -= (tmp / 4);
		    } else You("are suddenly very hot!");

		    if((Shock_resistance && rn2(StrongShock_resistance ? 20 : 5)) || ShockImmunity) {
			shieldeff(u.ux, u.uy);
			You_feel("a mild tingle.");
			ugolemeffects(AD_ELEC, tmp);
			if (tmp >= 4) tmp -= (tmp / 4);
		    } else You("are jolted with electricity!");

		    if((Cold_resistance && rn2(StrongCold_resistance ? 20 : 5)) || ColdImmunity) {
			shieldeff(u.ux, u.uy);
			You_feel("a mild chill.");
			ugolemeffects(AD_COLD, tmp);
			if (tmp >= 4) tmp -= (tmp / 4);
		    } else You("are suddenly very cold!");

		    mdamageu(mon, tmp);

			if (!rn2(3) && (!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4) )  ) {
			    losexp("life drainage", FALSE, TRUE);
			}

		break;

	      case AD_MALK:
		if (malive) {
			setustuck(mon);
			pline("%s sticks to you!", Monnam(mon));
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		}

		if((Shock_resistance && rn2(StrongShock_resistance ? 20 : 5)) || ShockImmunity) {
		    shieldeff(u.ux, u.uy);
		    You_feel("a strong tingle.");
		    ugolemeffects(AD_ELEC, tmp);
		    break;
		}
		You("are jolted with high voltage!");

		mdamageu(mon, tmp);
		break;

	    case AD_UVUU:{
		int wdmg = (int)(tmp/6) + 1;
		pline("A big spike painfully hits your %s!", body_part(HEAD));
		sprintf(buf, "%s spike", s_suffix(Monnam(mon)));
		poisoned(buf, A_CON, mon->data->mname, 60);
		if(Poison_resistance) wdmg -= ACURR(A_CON)/2;
		if (StrongPoison_resistance && wdmg > 1) wdmg /= 2;
		if(wdmg > 0){
		
			while( ABASE(A_WIS) > ATTRABSMIN(A_WIS) && wdmg > 0){
				wdmg--;
				(void) adjattrib(A_WIS, -1, TRUE, TRUE);
				forget_levels(1, FALSE);	/* lose memory of 1% of levels per point lost*/
				forget_objects(1, FALSE);	/* lose memory of 1% of objects per point lost*/
				exercise(A_WIS, FALSE);
			}
			if(AMAX(A_WIS) > ATTRABSMIN(A_WIS) && 
				ABASE(A_WIS) < AMAX(A_WIS)/2) AMAX(A_WIS) -= 1; //permanently drain wisdom
			if(wdmg){
				boolean chg;
				chg = make_hallucinated(HHallucination + (long)(wdmg*5),FALSE,0L);
			}
		}
		drain_en( (int)(tmp/2) );
		if(!rn2(20)){
			if (!has_head(youmonst.data) || Role_if(PM_COURIER) ) {
				tmp *= 2;
			}
			else if (noncorporeal(youmonst.data) || amorphous(youmonst.data)) {
				pline("%s passes through your %s.",
				      mon_nam(mon), body_part(HEAD));
				tmp *= 2;
			}
			else {
				if(!uarmh){
					tmp = (ABASE(A_WIS) <= ATTRABSMIN(A_WIS)) ? ( 2 * (Upolyd ? u.mh : u.uhp) + 400) : (tmp * 2); 
					pline("%s smashes your %s!",
						mon_nam(mon), body_part(HEAD));
				} else pline("%s's headspike hits your %s!",
						mon_nam(mon), xname(uarmh) );
			}
		 }
 		}
		mdamageu(mon, tmp);
	    break;

	      case AD_ENCH:	/* KMH -- remove enchantment (disenchanter) */
		case AD_NGEN:
		if (ptr->mattk[i].aatyp == AT_RATH) {
			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(10)) (void) drain_item_severely(otmpE);
			}
			Your("equipment seems less effective.");
			u.cnd_disenchantamount++;
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");

		}
		if (mhit) {
		    struct obj *obj = target;

		    if (aatyp == AT_KICK) {
			obj = uarmf;
			if (!obj) break;
		    } else if (aatyp == AT_BITE || aatyp == AT_BUTT ||
			       (aatyp >= AT_STNG && aatyp < AT_WEAP)) {
			break;		/* no object involved */
		    } else if (!obj && mhit & (HIT_UWEP | HIT_USWAPWEP))
			obj = uarmg;
		    passive_obj(mon, obj, &(ptr->mattk[i]));
	    	}
	    	break;

	      case AD_SHRD:	/* KMH -- remove enchantment (disenchanter) */

		if (ptr->mattk[i].aatyp == AT_RATH) {
			witherarmor();
		}

		if (mhit) {
		    struct obj *obj = target;

		    if (aatyp == AT_KICK) {
			obj = uarmf;
			if (!obj) break;
		    } else if (aatyp == AT_BITE || aatyp == AT_BUTT ||
			       (aatyp >= AT_STNG && aatyp < AT_WEAP)) {
			break;		/* no object involved */
		    } else if (!obj && mhit & (HIT_UWEP | HIT_USWAPWEP))
			obj = uarmg;
		    passive_obj(mon, obj, &(ptr->mattk[i]));
	    	}
	    	break;

	      default:
		break;
	    }	/* attack switch */
	  }	/*if(ptr->mattk[i].aatyp == AT_NONE) {*/
	} 	/*for(i = 0; ; i++) {*/

	}

	return(malive | mhit);


}

/*
 * Special (passive) attacks on an attacking object by monsters done here.
 * Assumes the attack was successful.
 */
void
passive_obj(mon, obj, mattk)
register struct monst *mon;
register struct obj *obj;	/* null means pick uwep, uswapwep or uarmg */
struct attack *mattk;		/* null means we find one internally */
{
	register struct permonst *ptr = mon->data;
	register int i;

#if 0
	/* if caller hasn't specified an object, use uwep, uswapwep or uarmg */
	if (!obj) {
	    obj = (u.twoweap && uswapwep && !rn2(2)) ? uswapwep : uwep;
	    if (!obj && (mattk->adtyp == AD_ENCH || mattk->adtyp == AD_NGEN || mattk->adtyp == AD_SHRD) )
		obj = uarmg;		/* no weapon? then must be gloves */
	    if (!obj) return;		/* no object to affect */
	}
#else
	/* In Slash'EM, the caller always specifies the object */
	if (!obj) return;		/* no object to affect */
#endif

	if (stack_too_big(obj)) return;

	/* if caller hasn't specified an attack, find one */
	if (!mattk) {
	    for(i = 0; ; i++) {
		if(i >= NATTK) return;	/* no passive attacks */
		if(ptr->mattk[i].aatyp == AT_NONE) break; /* try this one */
	    }
	    mattk = &(ptr->mattk[i]);
	}

	if (objects[(obj)->otyp].oc_material == MT_CHITIN && organivorous(mon->data) && !rn2(100)) {
		if (obj->spe > -20) {
			obj->spe--;
			pline_The("chitin was gnawed a bit!");
		}
	}

	switch(mattk->adtyp) {

	case AD_ACID:
	    if(!rn2(6)) {
		erode_obj(obj, TRUE, FALSE);
	    }
	    break;
	case AD_RUST:
	    if(!mon->mcan) {
		erode_obj(obj, FALSE, FALSE);
	    }
	    break;
	case AD_CORR:
	    if(!mon->mcan) {
		erode_obj(obj, TRUE, FALSE);
	    }
	    break;
	case AD_UNPR:
	    if (!mon->mcan && obj->oerodeproof) obj->oerodeproof = FALSE;
	    break;
	case AD_LAVA:
	case AD_FLAM:

burnagain:
	    if(!mon->mcan && !(Race_if(PM_CHIQUAI) && rn2(4)) && !stack_too_big(obj) && is_flammable(obj) && !(objects[obj->otyp].oc_material == MT_WOOD && rn2(4)) && !(objects[obj->otyp].oc_material == MT_LEATHER && rn2(2)) && !(objects[obj->otyp].oc_material == MT_ALKALINE && rn2(2)) && !obj->oerodeproof && obj->otyp != SPE_BOOK_OF_THE_DEAD && obj->otyp != AMULET_OF_YENDOR && obj->otyp != CANDELABRUM_OF_INVOCATION && obj->otyp != BELL_OF_OPENING && obj->oartifact != ART_KEY_OF_LAW && obj->oartifact != ART_KEY_OF_CHAOS && obj->oartifact != ART_KEY_OF_NEUTRALITY && obj->oartifact != ART_GAUNTLET_KEY ) {

			if (obj->oeroded < MAX_ERODE && !(obj->oartifact && rn2(4)) && (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ) && !((obj->blessed && !rnl(4)))) {
				obj->oeroded++;
				pline("%s weapon is damaged by fire!", carried(obj) ? "Your" : "A");
				if (obj && objects[obj->otyp].oc_material == MT_PAPER && !rn2(2)) goto burnagain;
				}
			else if (obj->oeroded == MAX_ERODE && !(obj->oartifact && rn2(4)) && !hard_to_destruct(obj) && (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ) && carried(obj) )
			{
				pline("Your weapon burned to ashes!");
				if (obj == uball) unpunish();
				delobj(obj);
				uwepgone();
				update_inventory();
	    
			}

	    }
	    break;

	case AD_DCAY:

	    if(!mon->mcan && !(Race_if(PM_CHIQUAI) && rn2(4)) && !stack_too_big(obj) && is_rottable(obj) && !(objects[obj->otyp].oc_material == MT_ALKALINE && rn2(2)) && !obj->oerodeproof && obj->otyp != SPE_BOOK_OF_THE_DEAD && obj->otyp != AMULET_OF_YENDOR && obj->otyp != CANDELABRUM_OF_INVOCATION && obj->otyp != BELL_OF_OPENING && obj->oartifact != ART_KEY_OF_LAW && obj->oartifact != ART_KEY_OF_CHAOS && obj->oartifact != ART_KEY_OF_NEUTRALITY && obj->oartifact != ART_GAUNTLET_KEY ) {

			if (obj->oeroded2 < MAX_ERODE && !(obj->oartifact && rn2(4)) && (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ) && !((obj->blessed && !rnl(4)))) {
				obj->oeroded2++;
				pline("%s weapon rots!", carried(obj) ? "Your" : "A");
				}
			else if (obj->oeroded2 == MAX_ERODE && !(obj->oartifact && rn2(4)) && !hard_to_destruct(obj) && (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ) && carried(obj) )
			{
				pline("Your weapon rotted away to nothing!");
				if (obj == uball) unpunish();
				delobj(obj);
				uwepgone();
				update_inventory();
	    
			}

	    }
	    break;

	case AD_WTHR:

	    if(!mon->mcan && !(Race_if(PM_CHIQUAI) && rn2(4)) && !stack_too_big(obj) && !is_unwitherable(obj) && obj->otyp != SPE_BOOK_OF_THE_DEAD && obj->otyp != AMULET_OF_YENDOR && obj->otyp != CANDELABRUM_OF_INVOCATION && obj->otyp != BELL_OF_OPENING && obj->oartifact != ART_KEY_OF_LAW && obj->oartifact != ART_KEY_OF_CHAOS && obj->oartifact != ART_KEY_OF_NEUTRALITY && obj->oartifact != ART_GAUNTLET_KEY ) {
		/*erode_obj(obj, TRUE, FALSE);*/

		if (rn2(2)) {
				if (obj->oeroded < MAX_ERODE && !(obj->oartifact && rn2(4)) && (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ) && !((obj->blessed && !rnl(4)))) 
				{obj->oeroded++;
				pline("%s weapon has taken damage!", carried(obj) ? "Your" : "A");
				}
			else if (obj->oeroded == MAX_ERODE && !(obj->oartifact && rn2(4)) && !hard_to_destruct(obj) && (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ) && carried(obj) )
			{
	/* Unfortunately I can't seem to make it work for non-player inventory objects without crashing. --Amy */
		    
			pline("Your weapon withered away!");
			if (obj == uball) unpunish();
			delobj(obj);
			uwepgone();
			update_inventory();
	    
			}
		}
		else {
				if (obj->oeroded2 < MAX_ERODE && !(obj->oartifact && rn2(4)) && (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ) && !((obj->blessed && !rnl(4))))
				{obj->oeroded2++;
				pline("%s weapon has taken damage!", carried(obj) ? "Your" : "A");
				}
			else if (obj->oeroded2 == MAX_ERODE && !(obj->oartifact && rn2(4)) && !hard_to_destruct(obj) && (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ) && carried(obj) )
			{
		    
			pline("Your weapon withered away!");
			if (obj == uball) unpunish();
			delobj(obj);
			uwepgone();
			update_inventory();
	    
			}
		}

	    }
	    break;
	case AD_ENCH:
	    if (!mon->mcan) {
		if (drain_item(obj)) {
			u.cnd_disenchantamount++;
			if (obj && carried(obj) && (obj->known || obj->oclass == ARMOR_CLASS)) {
				Your("%s less effective.", aobjnam(obj, "seem"));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		    	}
		} 
	    	break;
	    }
	case AD_NGEN:
	    if (!mon->mcan) {
		if (drain_item_severely(obj)) {
			u.cnd_disenchantamount++;
			if (obj && carried(obj) && (obj->known || obj->oclass == ARMOR_CLASS)) {
				Your("%s less effective.", aobjnam(obj, "seem"));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
			}
	    	}
	    	break;
	    }
	case AD_SHRD:
	    if (!mon->mcan) {
		if (drain_item(obj)) {
			u.cnd_disenchantamount++;
			if(carried(obj) && (obj->known || obj->oclass == ARMOR_CLASS)) {
				Your("%s less effective.", aobjnam(obj, "seem"));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
			}
	    	}

		else if(obj->otyp != SPE_BOOK_OF_THE_DEAD && !is_unwitherable(obj) && obj->otyp != AMULET_OF_YENDOR && obj->otyp != CANDELABRUM_OF_INVOCATION && obj->otyp != BELL_OF_OPENING && obj->oartifact != ART_KEY_OF_LAW && obj->oartifact != ART_KEY_OF_CHAOS && obj->oartifact != ART_KEY_OF_NEUTRALITY && obj->oartifact != ART_GAUNTLET_KEY ) {

		if (rn2(2)) {
				if (obj->oeroded < MAX_ERODE && !(Race_if(PM_CHIQUAI) && rn2(4)) && !(obj->oartifact && rn2(4)) && (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ) && !((obj->blessed && !rnl(4)))) 
				{obj->oeroded++;
				pline("%s weapon has taken damage!", carried(obj) ? "Your" : "A");
				}
			else if (obj->oeroded == MAX_ERODE && !(Race_if(PM_CHIQUAI) && rn2(4)) && !(obj->oartifact && rn2(4)) && !hard_to_destruct(obj) && (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ) && carried(obj) )
			{
	/* Unfortunately I can't seem to make it work for non-player inventory objects without crashing. --Amy */
		    
			pline("Your weapon withered away!");
			if (obj == uball) unpunish();
			delobj(obj);
			uwepgone();
			update_inventory();
	    
			}
		}
		else {
				if (obj->oeroded2 < MAX_ERODE && !(Race_if(PM_CHIQUAI) && rn2(4)) && !(obj->oartifact && rn2(4)) && (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ) && !((obj->blessed && !rnl(4))))
				{obj->oeroded2++;
				pline("%s weapon has taken damage!", carried(obj) ? "Your" : "A");
				}
			else if (obj->oeroded2 == MAX_ERODE && !(Race_if(PM_CHIQUAI) && rn2(4)) && !(obj->oartifact && rn2(4)) && !hard_to_destruct(obj) && (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ) && carried(obj) )
			{
		    
			pline("Your weapon withered away!");
			if (obj == uball) unpunish();
			delobj(obj);
			uwepgone();
			update_inventory();
	    
			}
		}

	    }


	    	break;
	    }
	  default:
	    break;
	}

	if (obj && carried(obj)) update_inventory();
}

int difeasemu(mon)
struct permonst *mon;
{
	if (IntSick_resistance || (ExtSick_resistance && rn2(20)) ) {
		You_feel("a slight illness.");
		return FALSE;
	} else {
		make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON), 40),
			mon->mname, TRUE, SICK_NONVOMITABLE);
		return TRUE;
	}
}


/* Note: caller must ascertain mtmp is mimicking... */
void
stumble_onto_mimic(mtmp)
struct monst *mtmp;
{
	const char *fmt = (Role_if(PM_PIRATE) || Role_if(PM_KORSAIR) || PirateSpeakOn) ? "Arrrrr!  Suddenly %s shows its true colors!" : "Wait!  That's %s!",
		   *generic = "a monster",
		   *what = 0;

	if(!u.ustuck && !mtmp->mflee && dmgtype(mtmp->data,AD_STCK))
	    setustuck(mtmp);

	if (Blind) {
	    if (!Blind_telepat)
		what = generic;		/* with default fmt */
	    else if (mtmp->m_ap_type == M_AP_MONSTER)
		what = a_monnam(mtmp);	/* differs from what was sensed */
	} else {
#ifdef DISPLAY_LAYERS
	    if (levl[u.ux+u.dx][u.uy+u.dy].mem_bg == S_hcdoor ||
		    levl[u.ux+u.dx][u.uy+u.dy].mem_bg == S_vcdoor)
		fmt = "The door actually was %s!";
	    else if (levl[u.ux+u.dx][u.uy+u.dy].mem_obj == GOLD_PIECE)
		fmt = "That gold was %s!";
#else
	    int glyph = levl[u.ux+u.dx][u.uy+u.dy].glyph;

	    if (glyph_is_cmap(glyph) &&
		    (glyph_to_cmap(glyph) == S_hcdoor ||
		     glyph_to_cmap(glyph) == S_vcdoor))
		fmt = "The door actually was %s!";
	    else if (glyph_is_object(glyph) &&
		    glyph_to_obj(glyph) == GOLD_PIECE)
		fmt = "That gold was %s!";
#endif

	    /* cloned Wiz starts out mimicking some other monster and
	       might make himself invisible before being revealed */
	    if ((mtmp->minvis && (!See_invisible || (!StrongSee_invisible && !mtmp->seeinvisble) ) ) || mtmp->minvisreal)
		what = generic;
	    else
		what = a_noit_monnam(mtmp);
	}

	if (mtmp && mtmp->data == &mons[PM_CAKE_MONSTER]) fmt = "The cake is alife!";

	if (what) pline(fmt, what);

	if (!rn2(2) && !mtmp->mpeaceful && !mtmp->mtame) (void) passive(mtmp, TRUE, mtmp->mhp > 0, AT_TUCH, FALSE);

	wakeup(mtmp);	/* clears mimicking */
}

STATIC_OVL void
nohandglow(mon)
struct monst *mon;
{
	char *hands=makeplural(body_part(HAND));

	if (!u.umconf || mon->mconf) return;
	if (u.umconf == 1) {
		if (Blind)
			Your("%s stop tingling.", hands);
		else
			Your("%s stop glowing %s.", hands, hcolor(NH_RED));
	} else {
		if (Blind)
			pline_The("tingling in your %s lessens.", hands);
		else
			Your("%s no longer glow so brightly %s.", hands,
				hcolor(NH_RED));
	}
	u.umconf--;
}

int
flash_hits_mon(mtmp, otmp)
struct monst *mtmp;
struct obj *otmp;	/* source of flash */
{
	int tmp, amt, res = 0, useeit = canseemon(mtmp);

	if (mtmp->msleeping) {
	    mtmp->msleeping = 0;
	    if (useeit) {
		pline_The("flash awakens %s.", mon_nam(mtmp));
		res = 1;
	    }
	} else if (mtmp->data->mlet != S_LIGHT) {
	    if (!resists_blnd(mtmp)) {
		tmp = dist2(otmp->ox, otmp->oy, mtmp->mx, mtmp->my);
		if (useeit) {
		    pline("%s is blinded by the flash!", Monnam(mtmp));
		    res = 1;
		}
		if (splittinggremlin(mtmp->data)) {
		    /* Rule #1: Keep them out of the light. */
		    amt = otmp->otyp == WAN_LIGHT ? d(1 + otmp->spe, 4) :
		          rn2(min(mtmp->mhp,4));
		    pline("%s %s!", Monnam(mtmp), amt > mtmp->mhp / 2 ?
			  "wails in agony" : "cries out in pain");
		    if ((mtmp->mhp -= amt) <= 0) {
			if (flags.mon_moving)
			    monkilled(mtmp, (char *)0, AD_BLND);
			else
			    killed(mtmp);
		    } else if (cansee(mtmp->mx,mtmp->my) && !canspotmon(mtmp) && !(mtmp->data->msound == MS_DEEPSTATE) && !(mtmp->egotype_deepstatemember)) {
			map_invisible(mtmp->mx, mtmp->my);
		    }
		}
		if (mtmp->mhp > 0) {
		    if (!flags.mon_moving) setmangry(mtmp);
		    if (tmp < 9 && !mtmp->isshk && rn2(4)) {
			if (rn2(4))
			    monflee(mtmp, rnd(100), FALSE, TRUE);
			else
			    monflee(mtmp, rnd(10), FALSE, TRUE);
		    }
		    mtmp->mcansee = 0;
		    mtmp->mblinded = (tmp < 3) ? 0 : rnd(1 + 50/tmp);
		}
	    }
	}
	return res;
}

/* AT_RATH: monster retaliates if you try a ranged action against it --Amy */
void
ranged_thorns(mon)
register struct monst *mon;
{

	if (!attacktype(mon->data, AT_RATH) && !attacktype(mon->data, AT_BOOM)) return;

	int mithrilitemcount = 0;

	if (uwep && objects[uwep->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;
	if (u.twoweap && uswapwep && objects[uswapwep->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;
	if (uarm && objects[uarm->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;
	if (uarmc && objects[uarmc->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;
	if (uarmh && objects[uarmh->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;
	if (uarms && objects[uarms->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;
	if (uarmg && objects[uarmg->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;
	if (uarmf && objects[uarmf->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;
	if (uarmu && objects[uarmu->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;
	if (uamul && objects[uamul->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;
	if (uimplant && objects[uimplant->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;
	if (uleft && objects[uleft->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;
	if (uright && objects[uright->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;
	if (ublindf && objects[ublindf->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;

	if (mithrilitemcount > 0 && (mithrilitemcount >= rnd(20))) {
		pline("Your mithril equipment prevents %s's retaliation!", mon_nam(mon));
		return;
	}

	if (!rn2(4)) return; /* some randomness --Amy */
	(void) passive(mon, FALSE, TRUE, AT_WEAP, TRUE);

}

STATIC_PTR void
set_lit(x,y,val)
int x, y;
void * val;
{
	if (val)
	    levl[x][y].lit = 1;
	else {
	    levl[x][y].lit = 0;
	    snuff_light_source(x, y);
	}
}

/*uhitm.c*/
