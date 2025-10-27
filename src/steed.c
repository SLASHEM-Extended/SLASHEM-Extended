/*	SCCS Id: @(#)steed.c	3.4	2003/01/10	*/
/* Copyright (c) Kevin Hugo, 1998-1999. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"


/* Monsters that might be ridden */
static NEARDATA const char steeds[] = {
	S_QUADRUPED, S_UNICORN, S_ANGEL, S_DEMON, S_CENTAUR, S_DRAGON, S_JABBERWOCK, S_COCKATRICE, S_HUMANOID, S_NYMPH,
S_SPIDER, S_ZOUTHERN, S_BAT, S_GIANT, S_KOP, S_LICH, S_MUMMY, S_NAGA, S_VAMPIRE, S_WRAITH, S_YETI, S_ZOMBIE, S_GOLEM, 
S_HUMAN, S_EEL, S_LIZARD, S_BAD_FOOD, S_BAD_COINS, S_FLYFISH,  '\0' 
/* added demons and some other stuff --Amy */
};

STATIC_DCL boolean landing_spot(coord *, int, int);

/* caller has decided that hero can't reach something while mounted */
void
rider_cant_reach()
{
     You("aren't skilled enough to reach from %s.", y_monnam(u.usteed));
}

/* to check for your current steed's saddle, because we might be looking for a specific artifact --Amy */
boolean
bmwride(artid)
int artid;
{
	if (!u.usteed) return FALSE;

	struct obj *osaeddle = which_armor(u.usteed, W_SADDLE);
	if ((osaeddle = which_armor(u.usteed, W_SADDLE)) && osaeddle->oartifact == artid) return TRUE;

	return FALSE;

}

/* to check for your current steed's saddle, when looking for a specific base type of saddle --Amy */
boolean
opelride(saddleid)
int saddleid;
{
	if (!u.usteed) return FALSE;

	struct obj *osaeddle = which_armor(u.usteed, W_SADDLE);
	if ((osaeddle = which_armor(u.usteed, W_SADDLE)) && osaeddle->otyp == saddleid) return TRUE;

	return FALSE;

}

/* to check for some random monster's saddle, because we might be looking for a specific artifact --Amy */
boolean
mercedesride(artid, mtmp)
int artid;
struct monst *mtmp;
{
	if (!mtmp) return FALSE;

	if (mtmp == &youmonst) return FALSE;

	struct obj *osaeddle = which_armor(mtmp, W_SADDLE);
	if ((osaeddle = which_armor(mtmp, W_SADDLE)) && osaeddle->oartifact == artid) return TRUE;

	return FALSE;

}

/* to check for some random monster's saddle, when looking for a specific base type of saddle --Amy */
boolean
fordride(saddleid, mtmp)
int saddleid;
struct monst *mtmp;
{
	if (!mtmp) return FALSE;

	if (mtmp == &youmonst) return FALSE;

	struct obj *osaeddle = which_armor(mtmp, W_SADDLE);
	if ((osaeddle = which_armor(mtmp, W_SADDLE)) && osaeddle->otyp == saddleid) return TRUE;

	return FALSE;

}

/* is the monster mtmp a dedicated steed (gives better speed returns, see allmain.c)? --Amy */
boolean
dedicatedsteed(mtmp)
struct monst *mtmp;
{
	if (mercedesride(ART_MOUNT_OF_THE_MAD_POTATO, mtmp)) return TRUE;
	if (index(steeds, mtmp->data->mlet)) return TRUE;
	if (mtmp->egotype_steed) return TRUE;
	if (monstersoundtype(mtmp) == MS_NEIGH || monstersoundtype(mtmp) == MS_SHOE || monstersoundtype(mtmp) == MS_CAR) return TRUE;

	return FALSE;
}

/* is the monster mtmp not suitable for riding (makes you very slow if you attempt to ride, see allmain.c)? --Amy */
boolean
nogoodsteed(mtmp)
struct monst *mtmp;
{
	if (monstersoundtype(mtmp) == MS_BULLETATOR) return TRUE;

	return FALSE;
}

/*** Putting the saddle on ***/

/* Can this monster wear a saddle? */
boolean
can_saddle(mtmp)
	struct monst *mtmp;
{
	if (!issoviet) return 1; /* just remove all those annoying restrictions and allow everything to be saddled --Amy */
	struct permonst *ptr = mtmp->data;

	return (index(steeds, ptr->mlet) && (ptr->msize >= MZ_MEDIUM) &&
			(!humanoid(ptr) || ptr->mlet == S_CENTAUR) &&
			!amorphous(ptr) && !noncorporeal(ptr) &&
			!is_whirly(ptr) && !unsolid(ptr));
}

/* high skill should allow the player to have a saving throw against falling off --Amy
 * not all instances of falling off the steed will allow you to do this though, and since I follow the rule that no skills
 * may make things harder for you, you'll always have a chance of falling off anyway just in case you specifically WANT
 * to be thrown off the steed (hint: cursed saddle and player would want to unmount but cannot).
 * returns 0 if the player failed the check and will fall off, 1 if the saving throw was successful and you stay mounted */
boolean
mayfalloffsteed()
{
	int ridesavingthrow = 0;
	int spcsavingthrow = 0;
	char buf[BUFSZ];

	if (uarm && uarm->oartifact == ART_MOTA_ROLA) {
		if (spcsavingthrow < 75) spcsavingthrow = 75;
	}

	if (opelride(SEATBELT_SADDLE)) {
		spcsavingthrow = 75;
		if (bmwride(ART_RULES_MAKE_SENSE)) spcsavingthrow = 90;
		if (bmwride(ART_CAR_SAFETY) && u.usteed && monstersoundtype(u.usteed) == MS_CAR) spcsavingthrow = 100;
	}

	if (!PlayerCannotUseSkills) {
		if (Race_if(PM_PERVERT)) {
			switch (P_SKILL(P_RIDING)) {
				case P_SKILLED: ridesavingthrow = 56; break;
				case P_EXPERT: ridesavingthrow = 67; break;
				case P_MASTER: ridesavingthrow = 89; break;
				case P_GRAND_MASTER: ridesavingthrow = 96; break;
				case P_SUPREME_MASTER: ridesavingthrow = 101; break;
			}
		} else {
			switch (P_SKILL(P_RIDING)) {
				case P_SKILLED: ridesavingthrow = 11; break;
				case P_EXPERT: ridesavingthrow = 34; break;
				case P_MASTER: ridesavingthrow = 76; break;
				case P_GRAND_MASTER: ridesavingthrow = 91; break;
				case P_SUPREME_MASTER: ridesavingthrow = 101; break;
			}
		}
	}

skillcheckdone:

	if (Role_if(PM_JOCKEY)) ridesavingthrow += ((100 - ridesavingthrow) / 2);

	if ((ridesavingthrow > 0) || (spcsavingthrow > 0)) {
		getlin ("Uh-oh! You're about to fall off your steed! Attempt a saving throw? [y/yes/no]",buf);
		(void) lcase (buf);
		if (!(strcmp (buf, "yes")) || !(strcmp (buf, "y")) || !(strcmp (buf, "ye")) || !(strcmp (buf, "ys"))) {
			if ((ridesavingthrow > rnd(100)) || (spcsavingthrow > rnd(100)) ) {
				pline("Success! You've managed to stay mounted.");
				return TRUE;
			} else {
				pline("Unfortunately your saving throw failed...");
				return FALSE;
			}
		}
	}

	return FALSE;

}

/* it was SOOOOOOO annoying that a confused steed would always walk randomly, when a confused player would only be
 * subjected to random-direction walking some of the time. Change that, and make it use riding skill. --Amy
 * Returns TRUE if you will walk randomly, FALSE otherwise */
boolean
confsteeddir()
{
	if (PlayerCannotUseSkills) return rn2(Race_if(PM_PERVERT) ? 2 : 4);
	if (!Race_if(PM_PERVERT)) {
		switch (P_SKILL(P_RIDING)) {

			default: return rn2(4);
			case P_BASIC: return rn2(2);
			case P_SKILLED: return !rn2(3);
			case P_EXPERT: return !rn2(4);
			case P_MASTER: return !rn2(5);
			case P_GRAND_MASTER: return !rn2(7);
			case P_SUPREME_MASTER: return !rn2(10);
		}
	} else {
		switch (P_SKILL(P_RIDING)) {
			default: return rn2(2);
			case P_BASIC: return !rn2(4);
			case P_SKILLED: return !rn2(6);
			case P_EXPERT: return !rn2(10);
			case P_MASTER: return !rn2(15);
			case P_GRAND_MASTER: return !rn2(25);
			case P_SUPREME_MASTER: return !rn2(50);
		}
	}

}

int
use_saddle(otmp)
	struct obj *otmp;
{
	struct monst *mtmp;
	struct permonst *ptr;
	int chance;
	const char *s;


	/* Can you use it? */
	if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER) ) {
		You("have no hands!");	/* not `body_part(HAND)' */
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

		if (yn("Try to use the saddle with another part of your body instead? Warning: this can fail and cause blindness.") == 'y') {
			if (rn2(3) && !polyskillchance()) {
	 			make_blinded(Blinded + rnd(50),TRUE);
				pline("You got something in your face!");
				if (!rn2(20)) badeffect();
				if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
				return 1;
			}
		}
		else {return(0);}

	} else if (!freehandX()) {
		You("have no free %s.", body_part(HAND));
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	}

	/* Select an animal */
	if (u.uswallow || Underwater || !getdir((char *)0)) {
	    pline("%s", Never_mind);
		if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
			pline("Oh wait, actually I do mind...");
			badeffect();
		}
	    return 0;
	}
	if (!u.dx && !u.dy) {
	    pline("Saddle yourself?  Very funny...");
	    return 0;
	}
	if (!isok(u.ux+u.dx, u.uy+u.dy) ||
			!(mtmp = m_at(u.ux+u.dx, u.uy+u.dy)) ||
			!canspotmon(mtmp)) {
	    pline("I see nobody there.");
	    return 1;
	}

	/* Is this a valid monster? */
	if (mtmp->misc_worn_check & W_SADDLE ||
			which_armor(mtmp, W_SADDLE)) {
	    pline("%s doesn't need another one.", Monnam(mtmp));
	    return 1;
	}
	ptr = mtmp->data;
	if (touch_petrifies(ptr) && (!uarmg || FingerlessGloves) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) )) {
	    static char kbuf[BUFSZ];

	    You("touch %s.", mon_nam(mtmp));
 	    if (!(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {
		sprintf(kbuf, "attempting to saddle a petrifying monster");
		instapetrify(kbuf);
 	    }
	}
	if (ptr == &mons[PM_INCUBUS] || ptr == &mons[PM_SUCCUBUS]) {
	    pline("Shame on you!");
	    exercise(A_WIS, FALSE);
	    /*return 1;*/ /* removed that stupid restriction --Amy */
	}
	if (mtmp->isminion || mtmp->isshk || mtmp->ispriest ||
			mtmp->isgd || mtmp->iswiz) {
	    pline("I think %s would mind.", mon_nam(mtmp));
	    return 1;
	}
	if (!can_saddle(mtmp)) {
		if (!issoviet) You_cant("saddle such a creature.");
		else pline("Ublyudka, kotoryy nazyvayet sebya sovetskiy ne khochet, chtoby vy yezdit' eto sushchestvo.");
		return 1;
	}

	/* Calculate your chance */
	chance = ACURR(A_DEX) + ACURR(A_CHA)/2 + 2*mtmp->mtame;
	chance += GushLevel * (mtmp->mtame ? 20 : 5);
	if (!mtmp->mtame) { /* humpers and steed egotypes should be easier... --Amy */

		if (mtmp->egotype_steed || canalwaysride(mtmp->data) || ((mercedesride(ART_GRAND_THEFT_AUTO, mtmp)) && (monstersoundtype(mtmp) == MS_CAR) ) || ((mercedesride(ART_UTTER_USELESSNESS, mtmp)) && (monstersoundtype(mtmp) == MS_FART_NORMAL || monstersoundtype(mtmp) == MS_FART_QUIET || monstersoundtype(mtmp) == MS_FART_LOUD || monstersoundtype(mtmp) == MS_STENCH || monstersoundtype(mtmp) == MS_SUPERMAN ) ) ) {
			if (mtmp->mpeaceful) chance -= mtmp->m_lev;
			else chance -= 2*mtmp->m_lev;
		} else {
			if (mtmp->mpeaceful) chance -= 5*mtmp->m_lev;
			else chance -= 10*mtmp->m_lev;
		}

	}
	if (Role_if(PM_KNIGHT) || Role_if(PM_CHEVALIER))
	    chance += 20;
	if (Role_if(PM_JOCKEY))
	    chance += 25;
	if (Role_if(PM_TRANSVESTITE) || Role_if(PM_TOPMODEL))
	    chance += 50;

	if (FemtrapActiveSabrina && mtmp->data == &mons[PM_SABRINA_S_PLATEAU_BOOT]) {
		chance += 100;
	}

	if (otmp && otmp->otyp == INKA_SADDLE) chance += 100;

	if (PlayerCannotUseSkills) chance -= 20;
	else {

	switch (P_SKILL(P_RIDING)) {
	case P_ISRESTRICTED:
	case P_UNSKILLED:
	default:
	    chance -= 20;	break;
	case P_BASIC:
	    break;
	case P_SKILLED:
	    chance += 25;	break;
	case P_EXPERT:
	    chance += 50;	break;
	case P_MASTER:
	    chance += 75;	break;
	case P_GRAND_MASTER:
	    chance += 100;	break;
	case P_SUPREME_MASTER:
	    chance += 150;	break;
	}

	}

	if ( (Confusion && !Conf_resist) || Fumbling || IsGlib)
	    chance -= 20;
	/*else */if (uarmg &&
		(s = OBJ_DESCR(objects[uarmg->otyp])) != (char *)0 &&
		( !strcmp(s, "riding gloves") || !strcmp(s, "yezda perchatki") || !strcmp(s, "kopgina qo'lqop") ) )
	    /* Bonus for wearing "riding" (but not fumbling) gloves */
	    chance += 10;
	/*else */if (uarmf &&
		(s = OBJ_DESCR(objects[uarmf->otyp])) != (char *)0 &&
		( !strcmp(s, "riding boots") || !strcmp(s, "sapogi dlya verkhovoy yezdy") || !strcmp(s, "kopgina chizilmasin") ) )
	    /* ... or for "riding boots" */
	    chance += 10; /* Amy edit: allowed those bonuses to stack. */
	if (otmp->cursed && otmp->otyp == LEATHER_SADDLE)
	    chance -= 50;

	/* Make the attempt */
	if (rn2(100) < chance) {
	    You("put the saddle on %s.", mon_nam(mtmp));
	    if (otmp && otmp->oartifact == ART_SADDLE_OF_REFLECTION) {
		You("reflect upon your life choices when applying the saddle.");
		adjattrib(A_WIS, -1, FALSE, TRUE);
	    }
	    if (otmp->owornmask) remove_worn_item(otmp, FALSE);
	    freeinv(otmp);
	    /* mpickobj may free otmp it if merges, but we have already
	       checked for a saddle above, so no merger should happen */
	    (void) mpickobj(mtmp, otmp, FALSE);
	    mtmp->misc_worn_check |= W_SADDLE;
	    otmp->owornmask = W_SADDLE;
	    otmp->leashmon = mtmp->m_id;
	    update_mon_intrinsics(mtmp, otmp, TRUE, FALSE);
	} else
	    pline("%s resists!", Monnam(mtmp));
	return 1;
}


/*** Riding the monster ***/

/* Can we ride this monster?  Caller should also check can_saddle() */
boolean
can_ride(mtmp)
	struct monst *mtmp;
{
	if (!issoviet) return (mtmp->mtame || mtmp->egotype_steed || (FemtrapActiveLarissa && monstersoundtype(mtmp) == MS_SHOE) || canalwaysride(mtmp->data) || ((mercedesride(ART_GRAND_THEFT_AUTO, mtmp)) && (monstersoundtype(mtmp) == MS_CAR) ) || ((mercedesride(ART_UTTER_USELESSNESS, mtmp)) && (monstersoundtype(mtmp) == MS_FART_NORMAL || monstersoundtype(mtmp) == MS_FART_QUIET || monstersoundtype(mtmp) == MS_FART_LOUD || monstersoundtype(mtmp) == MS_STENCH || monstersoundtype(mtmp) == MS_SUPERMAN ) ) || (Race_if(PM_SHOE) && monstersoundtype(mtmp) == MS_SHOE) );

	return ((mtmp->mtame || mtmp->egotype_steed || (FemtrapActiveLarissa && monstersoundtype(mtmp) == MS_SHOE) || canalwaysride(mtmp->data) || ((mercedesride(ART_GRAND_THEFT_AUTO, mtmp)) && (monstersoundtype(mtmp) == MS_CAR) ) || ((mercedesride(ART_UTTER_USELESSNESS, mtmp)) && (monstersoundtype(mtmp) == MS_FART_NORMAL || monstersoundtype(mtmp) == MS_FART_QUIET || monstersoundtype(mtmp) == MS_FART_LOUD || monstersoundtype(mtmp) == MS_STENCH || monstersoundtype(mtmp) == MS_SUPERMAN ) ) || (Race_if(PM_SHOE) && monstersoundtype(mtmp) == MS_SHOE)) && humanoid(youmonst.data) &&
			!verysmall(youmonst.data) && !bigmonst(youmonst.data) &&
			(!Underwater || is_swimmer(mtmp->data)) );

}
/* Removed a lot of annoying restrictions that don't serve any purpose anyway other than annoying the player. --Amy */
/* In Soviet Russia, you cannot ride freely because seriously, the player isn't supposed to be able to ride. --Amy */

int
doride()
{
	boolean forcemount = FALSE;

	if (u.usteed)
	    dismount_steed(DISMOUNT_BYCHOICE);
	else if (getdir((char *)0) && isok(u.ux+u.dx, u.uy+u.dy)) {
#ifdef WIZARD
	if (wizard && yn("Force the mount to succeed?") == 'y')
		forcemount = TRUE;
#endif
	    return (mount_steed(m_at(u.ux+u.dx, u.uy+u.dy), forcemount));
	} else
	    return 0;
	return 1;
}


/* Start riding, with the given monster */
boolean
mount_steed(mtmp, force)
	struct monst *mtmp;	/* The animal */
	boolean force;		/* Quietly force this animal */
{
	struct obj *otmp;
	char buf[BUFSZ];
	struct permonst *ptr;

	/* Sanity checks */
	if (u.usteed) {
	    You("are already riding %s.", mon_nam(u.usteed));
	    return (FALSE);
	}

	/* Is the player in the right form? */
	if (FunnyHallu && !force) {
	    pline("Maybe you should find a designated driver.");
	    /*return (FALSE); well, if the horse is saddled, a hallucinating player should be able to ride it --Amy*/
	}
	/* While riding Wounded_legs refers to the steed's,
	 * not the hero's legs.
	 * That opens up a potential abuse where the player
	 * can mount a steed, then dismount immediately to
	 * heal leg damage, because leg damage is always
	 * healed upon dismount (Wounded_legs context switch).
	 * By preventing a hero with Wounded_legs from
	 * mounting a steed, the potential for abuse is
	 * minimized, if not eliminated altogether.
	 */
	if (Wounded_legs) {
	    Your("%s are in no shape for riding.", makeplural(body_part(LEG)));
#ifdef WIZARD
	    if (force && wizard && yn("Heal your legs?") == 'y')
		HWounded_legs = 0;
	    else
#endif
	    if (yn("But you can try to get on your steed anyway. Do it?") == 'y') {
		if (rn2(3)) {
			losehp(rn1(10,20), "trying an illegal ride", KILLED_BY);
			pline("Ouch! You slip and hurt yourself a lot!");
			if (rn2(3)) {
				pline("Due to your leg injury, you don't manage to swing yourself onto your steed.");
				return(FALSE);
			}
		}
	    }
	    else return (FALSE);
	}

	if (Upolyd && !Race_if(PM_TRANSFORMER) && (!humanoid(youmonst.data) || verysmall(youmonst.data) ||
			bigmonst(youmonst.data) || slithy(youmonst.data))) {
		You("won't fit on a saddle.");

		if (yn("But you can try to get on your steed anyway. Do it? Warning: this can do up to 30 points of damage!") == 'y') {
			if (rn2(3) && !polyskillchance()) {
				losehp(rn1(10,20), "trying an illegal ride", NO_KILLER_PREFIX);
				pline("Ouch! You slip and hurt yourself a lot!");
				if (!rn2(20)) badeffect();
				return(FALSE); /* failed, need to try again */
			}
		}
		else {
			return(FALSE);
		}

	}
	if(!force && (near_capacity() > SLT_ENCUMBER)) {
	    You_cant("do that while carrying so much stuff.");
	    return (FALSE);
	}

	/* Can the player reach and see the monster? */
	/* Amy edit: boah the vanilla code for that was really shitty... permamimics should be rideable :P */
	if (!mtmp || (!force && !canspotmon(mtmp))) {
	    pline("I see nobody there.");
	    return (FALSE);
	}
	if (u.uswallow || u.ustuck || u.utrap ||
	    !test_move(u.ux, u.uy, mtmp->mx-u.ux, mtmp->my-u.uy, TEST_MOVE)) {
	    if (!(u.uswallow || u.ustuck || u.utrap))
		You("are unable to swing your %s over.", body_part(LEG)); 
	    else
		You("are stuck here for now.");
	    return (FALSE);
	}

	if (Punished) {
		You("are unable to swing your %s over.", body_part(LEG)); 
		if (yn("But you can try to get on your steed anyway. Do it?") == 'y') {
			if (rn2(3)) {
				losehp(rn1(10,20), "trying an illegal ride", NO_KILLER_PREFIX);
				pline("Ouch! You slip and hurt yourself a lot!");
			}
		}
		else {
			return(FALSE);
		}
	}

	/* Is this a valid monster? */
	otmp = which_armor(mtmp, W_SADDLE);
	if (!otmp) {
	    pline("%s is not saddled.", Monnam(mtmp));
	    return (FALSE);
	}
	ptr = mtmp->data;
	/* Amy edit: you should be able to ride a cockatrice as long as your body is sufficiently covered in clothing
	 * or also if you're highly skilled (making high riding skill more useful in the process) */
	if (touch_petrifies(ptr) && (PlayerCannotUseSkills || (P_SKILL(P_RIDING) < P_EXPERT)) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && !(uarmg && !FingerlessGloves && uarmu && uarm && uarmc) ) {
	    static char kbuf[BUFSZ];

	    You("touch %s.", mon_nam(mtmp));
	    sprintf(kbuf, "attempting to ride a petrifying monster");
	    instapetrify(kbuf);
	}
	if (!(mtmp->mtame || mtmp->egotype_steed || (FemtrapActiveLarissa && monstersoundtype(mtmp) == MS_SHOE) || canalwaysride(mtmp->data) || ((mercedesride(ART_GRAND_THEFT_AUTO, mtmp)) && (monstersoundtype(mtmp) == MS_CAR) ) || ((mercedesride(ART_UTTER_USELESSNESS, mtmp)) && (monstersoundtype(mtmp) == MS_FART_NORMAL || monstersoundtype(mtmp) == MS_FART_QUIET || monstersoundtype(mtmp) == MS_FART_LOUD || monstersoundtype(mtmp) == MS_STENCH || monstersoundtype(mtmp) == MS_SUPERMAN ) ) || (Race_if(PM_SHOE) && monstersoundtype(mtmp) == MS_SHOE)) || mtmp->isminion) {
	    pline("I think %s would mind.", mon_nam(mtmp));
	    return (FALSE);
	}
	if (mtmp->mtrapped) {
	    struct trap *t = t_at(mtmp->mx, mtmp->my);

	    You_cant("mount %s while %s's trapped in %s.",
		     mon_nam(mtmp), mhe(mtmp),
		     an(defsyms[trap_to_defsym(t->ttyp)].explanation));
	    return (FALSE);
	}

	if (!force && !(otmp && otmp->otyp == INKA_SADDLE) && !(FemtrapActiveSabrina && mtmp->data == &mons[PM_SABRINA_S_PLATEAU_BOOT]) && !(Role_if(PM_JOCKEY) && !rn2(5)) && !(uwep && uwep->oartifact == ART_DARK_CLAYMORE) && !Role_if(PM_KNIGHT) && !Role_if(PM_CHEVALIER) && mtmp->mtame && !(--mtmp->mtame)) {
	    /* no longer tame */
	    newsym(mtmp->mx, mtmp->my);
	    pline("%s resists%s!", Monnam(mtmp),
		  mtmp->mleashed ? " and its leash comes off" : "");
	    if (mtmp->mleashed) m_unleash(mtmp, FALSE, 0);
	    return (FALSE);
	}
	if (!force && Underwater && !is_swimmer(ptr)) {
	    You_cant("ride that creature while under water.");
	    return (FALSE);
	}
	if (!can_saddle(mtmp) || !can_ride(mtmp)) {
	    if (!issoviet) You_cant("ride such a creature.");
	    else pline("Ublyudka, kotoryy nazyvayet sebya sovetskiy ne khochet, chtoby vy yezdit' eto sushchestvo.");
	    return (0);
	}

	/* Is the player impaired? */
	if (!force && !is_floater(ptr) && !is_flyer(ptr) &&
			Levitation && !Lev_at_will) {
	    You("cannot reach %s.", mon_nam(mtmp));
	    return (FALSE);
	}
	if (!force && !(otmp && otmp->otyp == INKA_SADDLE) && uarm && is_metallic(uarm) &&
			greatest_erosionX(uarm)) {
	    Your("%s armor is too stiff to be able to mount %s.",
			uarm->oeroded ? "rusty" : "corroded",
			mon_nam(mtmp));
	    return (FALSE);
	}
	if (!force && !(otmp && otmp->otyp == INKA_SADDLE) && ((Confusion && !Conf_resist && rn2(3)) || (Fumbling && rn2(4)) || (IsGlib && rn2(2)) || (Wounded_legs && rn2(3)) ||
		otmp->cursed || (GushLevel+mtmp->mtame < rnd(MAXULEV/2+5) && ( (!Role_if(PM_KNIGHT) || !rn2(5)) && (!Role_if(PM_CHEVALIER) || !rn2(5)) && (!Role_if(PM_JOCKEY) || !rn2(5)) && (!Role_if(PM_YEOMAN) || !rn2(5)) && ((!Role_if(PM_TRANSVESTITE) && !Role_if(PM_TOPMODEL)) || !rn2(5)) ) ) )) {
	    if (Levitation) {
		pline("%s slips away from you.", Monnam(mtmp));
		return FALSE;
	    }
	    You("slip while trying to get on %s.", mon_nam(mtmp));

	    sprintf(buf, "slipped while mounting a monster (%s)",
		    /* "a saddled mumak" or "a saddled pony called Dobbin" */
		    x_monnam(mtmp, ARTICLE_A, (char *)0,
			SUPPRESS_IT|SUPPRESS_INVISIBLE|SUPPRESS_HALLUCINATION,
			     TRUE));
	    losehp(rn1(5,10), buf, NO_KILLER_PREFIX);
	    return (FALSE);
	}

	/* Success */
	if (!force) {
	    if (Levitation && !is_floater(ptr) && !is_flyer(ptr))
	    	/* Must have Lev_at_will at this point */
	    	pline("%s magically floats up!", Monnam(mtmp));
	    You("mount %s.", mon_nam(mtmp));

	    if (nogoodsteed(mtmp)) pline("This creature is too weak to carry you.");
	    else if (dedicatedsteed(mtmp)) You_feel("comfortable.");

	    if (otmp && otmp->oartifact == ART_SADDLE_OF_REFLECTION) {
		You("reflect upon your life choices when climbing the saddle.");
		adjattrib(A_WIS, -1, FALSE, TRUE);
	    }

	}
	/* setuwep handles polearms differently when you're mounted */
	if (uwep && is_applypole(uwep)) unweapon = FALSE;
	u.usteed = mtmp;
	remove_monster(mtmp->mx, mtmp->my);
	teleds(mtmp->mx, mtmp->my, TRUE);
	return (TRUE);
}


/* You and your steed have moved */
void
exercise_steed()
{
	if (!u.usteed)
		return;

	/* It takes many turns of riding to exercise skill */
	/* but not THAT godawfully many - used to be 100, now it's 33 --Amy */
	if (u.urideturns++ >= 33) {
	    u.urideturns = 0;
	    use_skill(P_RIDING, 1);
	}
	return;
}

/*
 * Try to find a dismount point adjacent to the steed's location.
 * If all else fails, try enexto().  Use enexto() as a last resort because
 * enexto() chooses its point randomly, possibly even outside the
 * room's walls, which is not what we want.
 * Adapted from mail daemon code.
 */
STATIC_OVL boolean
landing_spot(spot, reason, forceit)
coord *spot;	/* landing position (we fill it in) */
int reason;
int forceit;
{
    int i = 0, x, y, distance, min_distance = -1;
    boolean found = FALSE;
    struct trap *t;

    /* avoid known traps (i == 0) and boulders, but allow them as a backup */
    if (reason != DISMOUNT_BYCHOICE || Stunned || Numbed || Feared || Confusion || Fumbling) i = 1;
    for (; !found && i < 2; ++i) {
	for (x = u.ux-1; x <= u.ux+1; x++)
	    for (y = u.uy-1; y <= u.uy+1; y++) {
		if (!isok(x, y) || (x == u.ux && y == u.uy)) continue;

		if (ACCESSIBLE(levl[x][y].typ) &&
			    !MON_AT(x,y) && !closed_door(x,y)) {
		    distance = distu(x,y);
		    if (min_distance < 0 || distance < min_distance ||
			    (distance == min_distance && rn2(2))) {
			if (i > 0 || (((t = t_at(x, y)) == 0 || !t->tseen) &&
				      (!sobj_at(BOULDER, x, y) ||
				       throws_rocks(youmonst.data) || (uarmg && uarmg->oartifact == ART_MOUNTAIN_FISTS) ))) {
			    spot->x = x;
			    spot->y = y;
			    min_distance = distance;
			    found = TRUE;
			}
		    }
		}
	    }
    }

    /* If we didn't find a good spot and forceit is on, try enexto(). */
    if (forceit && min_distance < 0 &&
		!enexto(spot, u.ux, u.uy, youmonst.data))
	return FALSE;

    return found;
}

/* The player kicks or whips the steed */
void
kick_steed()
{
	char He[10];
	int gallopamount;
	if (!u.usteed)
	    return;

	/* [ALI] Various effects of kicking sleeping/paralyzed steeds */
	if (u.usteed->msleeping || !u.usteed->mcanmove) {
	    /* We assume a message has just been output of the form
	     * "You kick <steed>."
	     */
	    strcpy(He, mhe(u.usteed));
	    *He = highc(*He);
	    if ((u.usteed->mcanmove || u.usteed->mfrozen) && !rn2(2)) {
		if (u.usteed->mcanmove)
		    u.usteed->msleeping = 0;
		else if (u.usteed->mfrozen > 2)
		    u.usteed->mfrozen -= 2;
		else {
		    u.usteed->mfrozen = 0;
		    u.usteed->mcanmove = 1;
		    u.usteed->masleep = 0;
		}
		if (u.usteed->msleeping || !u.usteed->mcanmove)
		    pline("%s stirs.", He);
		else
		    pline("%s rouses %sself!", He, mhim(u.usteed));
	    } else
		pline("%s does not respond.", He);
	    return;
	}

	/* Make the steed less tame and check if it resists */
	if (u.usteed->mtame && (!Role_if(PM_JOCKEY) || !rn2(3)) ) u.usteed->mtame--;
	if (!u.usteed->mtame && u.usteed->mleashed) m_unleash(u.usteed, TRUE, 0);
	if (!u.usteed->mtame || (u.ulevel+u.usteed->mtame < rnd(MAXULEV/2+5))) {

		if (!mayfalloffsteed()) {
			newsym(u.usteed->mx, u.usteed->my);
			dismount_steed(DISMOUNT_THROWN);
			return;
		}
	}

	pline("%s gallops!", Monnam(u.usteed));
	/* here's another thing that should IMHO be better if you have more skill; in vanilla, high riding skill is
	 * borderline useless, but I decided that high skill will make your horse gallop for a loooooong time --Amy */

	gallopamount = rn1(20, 30);
	if (!PlayerCannotUseSkills) {
		switch (P_SKILL(P_RIDING)) {
			case P_SKILLED: gallopamount *= 2; break;
			case P_EXPERT: gallopamount *= 4; break;
			case P_MASTER: gallopamount *= 6; break;
			case P_GRAND_MASTER: gallopamount *= 8; break;
			case P_SUPREME_MASTER: gallopamount *= 10; break;
		}
	}
	if (uarmf && uarmf->oartifact == ART_ARVOGENIA_S_BIKER_HEELS) gallopamount *= 2;

	u.ugallop += gallopamount;
	return;
}

/* Stop riding the current steed */
void
dismount_steed(reason)
	int reason;		/* Player was thrown off etc. */
{
	struct monst *mtmp;
	struct obj *otmp;
	coord cc;
	const char *verb = "fall";
	boolean repair_leg_damage = TRUE;
	unsigned save_utrap = u.utrap;
	boolean have_spot = landing_spot(&cc,reason,0);
	register struct trap *ttmp;

	mtmp = u.usteed;		/* make a copy of steed pointer */
	/* Sanity check */
	if (!mtmp)		/* Just return silently */
	    return;

	/* Check the reason for dismounting */
	otmp = which_armor(mtmp, W_SADDLE);
	switch (reason) {
	    case DISMOUNT_THROWN:
		verb = "are thrown";
	    case DISMOUNT_FELL:
		You("%s off of %s!", verb, mon_nam(mtmp));
		if (!have_spot) have_spot = landing_spot(&cc,reason,1);
		losehp(rn1(10,10), "riding accident", KILLED_BY_AN);
		set_wounded_legs(BOTH_SIDES, HWounded_legs + (int)HWounded_legs + rn1(5,5));
		repair_leg_damage = FALSE;
		break;
	    case DISMOUNT_POLY:
		You("can no longer ride %s.", mon_nam(u.usteed));
		if (!have_spot) have_spot = landing_spot(&cc,reason,1);
		break;
	    case DISMOUNT_ENGULFED:
		/* caller displays message */
		break;
	    case DISMOUNT_BONES:
		/* hero has just died... */
		break;
	    case DISMOUNT_GENERIC:
		/* no messages, just make it so */
		break;
	    case DISMOUNT_BYCHOICE:
	    default:
		if (u.utrap && (ttmp = t_at(u.ux, u.uy)) && ttmp && ttmp->ttyp == FARTING_WEB) {
		    You("can't escape from the farting web.");
		    return;
		}
		if (otmp && otmp->cursed) {
		    You("can't.  The saddle %s cursed.",
			otmp->bknown ? "is" : "seems to be");
		    otmp->bknown = TRUE;
		    return;
		}
		if (!have_spot) {
		    You("can't. There isn't anywhere for you to stand.");
		    return;
		}
		if (!mtmp->mnamelth) {
			pline("You've been through the dungeon on %s with no name.",
				an(mtmp->data->mname));
			if (FunnyHallu)
				pline("It felt good to get out of the rain.");
		} else
			You("dismount %s.", mon_nam(mtmp));
	}
	if (!program_state.gameover && otmp && otmp->otyp == INKA_SADDLE) {
		pline("The inka saddle tries unsuccessfully to prevent you from getting off your steed, and rips off your %s in the process...", body_part(BODY_SKIN));
		pline("You and your steed are severely hurt!");
		losehp(rnd(u.ulevel * 5), "inka saddle", KILLED_BY_AN);
		if (mtmp && mtmp->mhp > 1) mtmp->mhp /= 2;
	}

	/* While riding these refer to the steed's legs
	 * so after dismounting they refer to the player's
	 * legs once again.
	 */
	if (repair_leg_damage) HWounded_legs = 0;

	/* Release the steed and saddle */
	u.usteed = 0;
	u.ugallop = 0L;

	/* Set player and steed's position.  Try moving the player first
	   unless we're in the midst of creating a bones file. */
	if (reason == DISMOUNT_BONES) {
	    /* move the steed to an adjacent square */
	    if (enexto(&cc, u.ux, u.uy, mtmp->data))
		rloc_to(mtmp, cc.x, cc.y);
	    else	/* evidently no room nearby; move steed elsewhere */
		(void) rloc(mtmp, FALSE);
	    return;
	}
	if (!DEADMONSTER(mtmp)) {
	    place_monster(mtmp, u.ux, u.uy);
	    if (!u.uswallow && !u.ustuck && have_spot) {
		struct permonst *mdat = mtmp->data;

		/* The steed may drop into water/lava */
		if (!is_flyer(mdat) && (!mtmp->egotype_flying) && !is_floater(mdat) && !is_clinger(mdat)) {
		    if (is_waterypool(u.ux, u.uy) || is_watertunnel(u.ux, u.uy)) {
			if (!Underwater)
			    pline("%s falls into the %s!", Monnam(mtmp),
							surface(u.ux, u.uy));
			if (!is_swimmer(mdat) && !mtmp->egotype_watersplasher && !amphibious(mdat)) {
			    killed(mtmp);
			    adjalign(-5);
			}
		    } else if (is_lava(u.ux, u.uy)) {
			pline("%s is pulled into the lava!", Monnam(mtmp));
			if (!likes_lava(mdat)) {
			    killed(mtmp);
			    adjalign(-5);
			}
		    }
		}
	    /* Steed dismounting consists of two steps: being moved to another
	     * square, and descending to the floor.  We have functions to do
	     * each of these activities, but they're normally called
	     * individually and include an attempt to look at or pick up the
	     * objects on the floor:
	     * teleds() --> spoteffects() --> pickup()
	     * float_down() --> pickup()
	     * We use this kludge to make sure there is only one such attempt.
	     *
	     * Clearly this is not the best way to do it.  A full fix would
	     * involve having these functions not call pickup() at all, instead
	     * calling them first and calling pickup() afterwards.  But it
	     * would take a lot of work to keep this change from having any
	     * unforseen side effects (for instance, you would no longer be
	     * able to walk onto a square with a hole, and autopickup before
	     * falling into the hole).
	     */
		/* [ALI] No need to move the player if the steed died. */
		if (!DEADMONSTER(mtmp)) {
		    /* Keep steed here, move the player to cc;
		     * teleds() clears u.utrap
		     */
		    in_steed_dismounting = TRUE;
		    teleds(cc.x, cc.y, TRUE);
		    in_steed_dismounting = FALSE;

		    /* Put your steed in your trap */
		    if (save_utrap)
			(void) mintrap(mtmp);
		}
	    /* Couldn't... try placing the steed */
	    } else if (enexto(&cc, u.ux, u.uy, mtmp->data)) {
		/* Keep player here, move the steed to cc */
		rloc_to(mtmp, cc.x, cc.y);
		/* Player stays put */
	    /* Otherwise, kill the steed */
	    } else {
		killed(mtmp);
		adjalign(-5);
	    }
	}

	/* Return the player to the floor */
	if (reason != DISMOUNT_ENGULFED) {
	    in_steed_dismounting = TRUE;
	    (void) float_down(0L, W_SADDLE);
	    in_steed_dismounting = FALSE;
	    flags.botl = 1;
	    (void)encumber_msg();
	    vision_full_recalc = 1;
	} else
	    flags.botl = 1;
	/* polearms behave differently when not mounted */
	if (uwep && is_pole(uwep)) unweapon = TRUE;
	return;
}

void
place_monster(mon, x, y)
struct monst *mon;
int x, y;
{
    if (!mon) {
	impossible("place_monster: no monster?");
    }

    if (mon && mon == u.usteed ||
	    /* special case is for convoluted vault guard handling */
	    (DEADMONSTER(mon) && !(mon->isgd && x == 0 && y == 0))) {
	/*impossible("placing bugged monster onto map?");*/
	return;
    }
    mon->mx = x, mon->my = y;
    level.monsters[x][y] = mon;
}

/* will something hit your steed instead of you? --Amy */
boolean
will_hit_steed()
{
	/* if your steed is low on health, attacks should be redirected to it much less often, otherwise they die constantly */
	if (u.usteed && (u.usteed->mhp < 5 || (u.usteed->mhp <= (u.usteed->mhpmax / 5) ) ) && rn2(5)) return FALSE;

	if (u.usteed) {

		if (bmwride(ART_NO_DAMAGE)) return FALSE;

	}

	if (rn2(100) < u.steedhitchance) return TRUE;
	else return FALSE;
}

/* does your steed have the saddle that lets you fly? --Amy */
boolean
flysaddle()
{
	if (u.usteed) {

		if (bmwride(ART_CLOWN_CAR)) return TRUE;

	}
	return FALSE;
}

/*steed.c*/
