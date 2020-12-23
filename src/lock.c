/*	SCCS Id: @(#)lock.c	3.4	2000/02/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

STATIC_PTR int picklock(void);
STATIC_PTR int forcelock(void);
STATIC_PTR int forcedoor(void);

/* at most one of `door' and `box' should be non-null at any given time */
STATIC_VAR NEARDATA struct xlock_s {
	struct rm  *door;
	struct obj *box;
	int picktyp, chance, usedtime;
	/* ALI - Artifact doors */
	int key;			/* Key being used (doors only) */
} xlock;

#ifdef OVLB

STATIC_DCL const char *lock_action(void);
STATIC_DCL boolean obstructed(int,int);
STATIC_DCL void chest_shatter_msg(struct obj *);

boolean
picking_lock(x, y)
	int *x, *y;
{
	if (occupation == picklock || occupation == forcedoor) {
	    *x = u.ux + u.dx;
	    *y = u.uy + u.dy;
	    return TRUE;
	} else {
	    *x = *y = 0;
	    return FALSE;
	}
}

boolean
picking_at(x, y)
int x, y;
{
	return (boolean)(occupation == picklock && xlock.door == &levl[x][y]);
}

/* produce an occupation string appropriate for the current activity */
STATIC_OVL const char *
lock_action()
{
	/* "unlocking"+2 == "locking" */
	static const char *actions[] = {
		/* [0] */	"unlocking the door",
		/* [1] */	"unlocking the chest",
		/* [2] */	"unlocking the box",
		/* [3] */	"picking the lock"
	};

	/* if the target is currently unlocked, we're trying to lock it now */
	if (xlock.door && !(xlock.door->doormask & D_LOCKED))
		return actions[0]+2;	/* "locking the door" */
	else if (xlock.box && !xlock.box->olocked)
		return (xlock.box->otyp == CHEST || xlock.box->otyp == CHEST_OF_HOLDING) ? actions[1]+2 : actions[2]+2;
	/* otherwise we're trying to unlock it */
	else if (xlock.picktyp == LOCK_PICK)
		return actions[3];	/* "picking the lock" */
	else if (xlock.picktyp == HAIRCLIP)
		return actions[3];	/* "picking the lock" */
	else if (xlock.picktyp == CREDIT_CARD)
		return actions[3];	/* same as lock_pick */
	else if (xlock.picktyp == DATA_CHIP)
		return actions[3];	/* same as lock_pick */
	else if (xlock.door)
		return actions[0];	/* "unlocking the door" */
	else
		return (xlock.box->otyp == CHEST || xlock.box->otyp == CHEST_OF_HOLDING) ? actions[1] : actions[2];
}

STATIC_PTR
int
picklock()	/* try to open/close a lock */
{
	register struct obj *trophy;

	if (xlock.box) {
	    if((xlock.box->ox != u.ux) || (xlock.box->oy != u.uy)) {
		return((xlock.usedtime = 0));		/* you or it moved */
	    }
	} else {		/* door */
	    if(xlock.door != &(levl[u.ux+u.dx][u.uy+u.dy])) {
		return((xlock.usedtime = 0));		/* you moved */
	    }
	    switch (xlock.door->doormask) {
		case D_NODOOR:
		    pline("This doorway has no door.");
		    return((xlock.usedtime = 0));
		case D_ISOPEN:
		    You("cannot lock an open door.");
		    return((xlock.usedtime = 0));
		case D_BROKEN:
		    pline("This door is broken.");
		    return((xlock.usedtime = 0));
	    }
	}

	if (xlock.usedtime++ >= 50 ) {
	    You("give up your attempt at %s.", lock_action());
	    exercise(A_DEX, TRUE);	/* even if you don't succeed */
	    return((xlock.usedtime = 0));
	}

	if(rn2(100) >= xlock.chance) return(1);		/* still busy */
	if(isfriday && (rn2(100) >= xlock.chance)) return(1);		/* still busy */

	You("succeed in %s.", lock_action());
	if (xlock.door) {
	    if(xlock.door->doormask & D_TRAPPED) {
		    b_trapped("door", FINGER);
		    xlock.door->doormask = D_NODOOR;
		    unblock_point(u.ux+u.dx, u.uy+u.dy);
		    if (*in_rooms(u.ux+u.dx, u.uy+u.dy, SHOPBASE))
			add_damage(u.ux+u.dx, u.uy+u.dy, 0L);
		    newsym(u.ux+u.dx, u.uy+u.dy);
	    } else if(xlock.door->doormask == D_LOCKED) {
		xlock.door->doormask = D_CLOSED;
		u.cnd_unlockamount++;
	    } else xlock.door->doormask = D_LOCKED;

		/* better handling for special doors in Vlad's: only give the rewards if you actually unlock them --Amy */
	    if (artifact_door(u.ux+u.dx, u.uy+u.dy) && !strcmp(dungeons[u.uz.dnum].dname, "Vlad's Tower")) {

		switch (artifact_door(u.ux+u.dx, u.uy+u.dy)) {

			case ART_MASTER_KEY_OF_THIEVERY:
				if (!u.keythief) {
					u.keythief = TRUE;
					pline("Congratulations, a reward for picking this lock was dropped at your %s!", makeplural(body_part(FOOT)));
					trophy = mksobj(STONE_OF_MAGIC_RESISTANCE, TRUE, FALSE, FALSE);
					if (trophy) {
					    dropy(trophy);
					}
				}
				break;
			case ART_NOCTURNAL_KEY:
				if (!u.keynocturn) {
					u.keynocturn = TRUE;
					pline("Congratulations, a reward for picking this lock was dropped at your %s!", makeplural(body_part(FOOT)));
					trophy = mksobj(SPE_PASSWALL, TRUE, FALSE, FALSE);
					if (trophy) {
					    dropy(trophy);
					}
				}
				break;
			case ART_KEY_OF_ACCESS:
				if (!u.keyaccess) {
					u.keyaccess = TRUE;
					pline("Congratulations, a reward for picking this lock was dropped at your %s!", makeplural(body_part(FOOT)));
					trophy = mksobj(SPE_GODMODE, TRUE, FALSE, FALSE);
					if (trophy) {
					    dropy(trophy);
					}
				}
				break;
			case ART_GAUNTLET_KEY:
				if (!u.keygauntlet) {
					u.keygauntlet = TRUE;
					pline("Congratulations, a reward for picking this lock was dropped at your %s!", makeplural(body_part(FOOT)));
					trophy = mksobj(SCR_WISHING, TRUE, FALSE, FALSE);
					if (trophy) {
					    dropy(trophy);
					}
				}
				break;
			default:
				break;

		}

	    }

	} else {
	    xlock.box->olocked = !xlock.box->olocked;
	    if (!xlock.box->olocked) u.cnd_unlockamount++;
	    if(xlock.box->otrapped) {
		if (Role_if(PM_CYBERNINJA) && rn2(5)) You("also disarm the trap you found on it.");
		else (void) chest_trap(xlock.box, FINGER, FALSE);
	    }
	}
	exercise(A_DEX, TRUE);
	return((xlock.usedtime = 0));
}

STATIC_PTR
int
forcelock()	/* try to force a locked chest */
{

	register struct obj *otmp;

	if((xlock.box->ox != u.ux) || (xlock.box->oy != u.uy))
		return((xlock.usedtime = 0));		/* you or it moved */

	if (xlock.usedtime++ >= 50 || !uwep ) {
	    You("give up your attempt to force the lock.");
	    if(xlock.usedtime >= 50)		/* you made the effort */
	      exercise((xlock.picktyp) ? A_DEX : A_STR, TRUE);
	    return((xlock.usedtime = 0));
	}

	if(xlock.picktyp == 1) {     /* blade */

	    if(rn2(1000 - (int)uwep->spe) > ((isfriday ? 980 : 992) - (greatest_erosionX(uwep) * 10)) &&
	       !uwep->cursed && !obj_resists(uwep, 0, 99)) {
		/* for a +0 weapon, probability that it survives an unsuccessful
		 * attempt to force the lock is (.992)^50 = .67 or on a friday the 13th (.980)^50 = .36
		 */
		pline("%sour %s broke!",
		      (uwep->quan > 1L) ? "One of y" : "Y", xname(uwep));
		useup(uwep);
		You("give up your attempt to force the lock.");
		exercise(A_DEX, TRUE);
		return((xlock.usedtime = 0));
	    }
	} else if(xlock.picktyp == 0)                 /* blunt */
	    wake_nearby();	/* due to hammering on the container */

	if(rn2(100) >= xlock.chance) return(1);		/* still busy */
	if(isfriday && (rn2(100) >= xlock.chance)) return(1);		/* still busy */

	You("succeed in forcing the lock.");

	if (uwep && is_lightsaber(uwep) && uwep->lamplit) {
		use_skill(P_WEDI, 1);
	}

	xlock.box->olocked = 0;
	xlock.box->obroken = 1;
	if((xlock.picktyp == 0 && !rn2(isfriday ? 2 : 3)) || (xlock.picktyp == 2 && !rn2(isfriday ? 3 : 5))) {
	    struct monst *shkp;
	    boolean costly;
	    long loss = 0L;

	    costly = (*u.ushops && costly_spot(u.ux, u.uy));
	    shkp = costly ? shop_keeper(*u.ushops) : 0;

	    pline("In fact, you've totally destroyed %s.",
		  the(xname(xlock.box)));

	    /* Put the contents on ground at the hero's feet. */
	    while ((otmp = xlock.box->cobj) != 0) {
		obj_extract_self(otmp);
		/* [ALI] Allowing containers to be destroyed is complicated
		 * (because they might contain indestructible objects).
		 * Since this is very unlikely to occur in practice simply
		 * avoid the possibility.
		 */
		if (!evades_destruction(otmp) && !Has_contents(otmp) &&
		  (!rn2(isfriday ? 2 : 3) || otmp->oclass == POTION_CLASS)) {
		    chest_shatter_msg(otmp);
		    if (costly)
		        loss += stolen_value(otmp, u.ux, u.uy,
					     (boolean)shkp->mpeaceful, TRUE,
					     TRUE);
		    if (otmp->quan == 1L) {
			obfree(otmp, (struct obj *) 0);
			continue;
		    }
		    useup(otmp);
		}
		if ( (xlock.box->otyp == ICE_BOX || xlock.box->otyp == ICE_BOX_OF_HOLDING || xlock.box->otyp == ICE_BOX_OF_WATERPROOFING || xlock.box->otyp == ICE_BOX_OF_DIGESTION) && otmp->otyp == CORPSE) {
		    otmp->age = monstermoves - otmp->age; /* actual age */
		    otmp->icedobject = TRUE;
		    start_corpse_timeout(otmp);
		}
		place_object(otmp, u.ux, u.uy);
		stackobj(otmp);
	    }

	    if (costly)
		loss += stolen_value(xlock.box, u.ux, u.uy,
				     (boolean)shkp->mpeaceful, TRUE, TRUE);
	    if(loss) You("owe %ld %s for objects destroyed.", loss, currency(loss));
	    delobj(xlock.box);
	}
	exercise((xlock.picktyp) ? A_DEX : A_STR, TRUE);
	return((xlock.usedtime = 0));
}

STATIC_PTR
int
forcedoor()      /* try to break/pry open a door */
{

	if(xlock.door != &(levl[u.ux+u.dx][u.uy+u.dy])) {
	    return((xlock.usedtime = 0));           /* you moved */
	} 
	switch (xlock.door->doormask) {
	    case D_NODOOR:
		pline("This doorway has no door.");
		return((xlock.usedtime = 0));
	    case D_ISOPEN:
		You("cannot lock an open door.");
		return((xlock.usedtime = 0));
	    case D_BROKEN:
		pline("This door is broken.");
		return((xlock.usedtime = 0));
	}
	
	if (xlock.usedtime++ >= 50 ) {
	    You("give up your attempt at %s the door.",
	    	(xlock.picktyp == 2 ? "melting" : xlock.picktyp == 1 ? 
	    		"prying open" : "breaking down"));
	    exercise(A_STR, TRUE);      /* even if you don't succeed */
	    return((xlock.usedtime = 0));
	}

	if(rn2(100) > xlock.chance) return(1);          /* still busy */
	if(isfriday && (rn2(100) > xlock.chance)) return(1);          /* still busy */

	You("succeed in %s the door.",
	    	(xlock.picktyp == 2 ? "melting" : xlock.picktyp == 1 ? 
	    		"prying open" : "breaking down"));

	if(xlock.door->doormask & D_TRAPPED) {
	    b_trapped("door", 0);
	    xlock.door->doormask = D_NODOOR;
	} else if (xlock.picktyp == 1)
	    xlock.door->doormask = D_BROKEN;
	else xlock.door->doormask = D_NODOOR;
	unblock_point(u.ux+u.dx, u.uy+u.dy);
	if (*in_rooms(u.ux+u.dx, u.uy+u.dy, SHOPBASE)) {
	    add_damage(u.ux+u.dx, u.uy+u.dy, 400L);
	    pay_for_damage("break", FALSE);

	    if (in_town(u.ux+u.dx, u.uy+u.dy)) {
		struct monst *mtmp;
		for(mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		    if (DEADMONSTER(mtmp)) continue;
		    if((mtmp->data == &mons[PM_WATCHMAN] ||
			mtmp->data == &mons[PM_WATCH_CAPTAIN]) &&
			couldsee(mtmp->mx, mtmp->my) &&
			mtmp->mpeaceful) {
			if (canspotmon(mtmp))
			    pline("%s yells:", Amonnam(mtmp));
			else
			    You_hear("someone yell:");
			verbalize("Halt, thief!  You're under arrest!");
			(void) angry_guards(FALSE);
			break;
		    }
		}
	    }
	}
	if (Blind)
	    feel_location(u.ux+u.dx, u.uy+u.dy);    /* we know we broke it */
	else
	    newsym(u.ux+u.dx, u.uy+u.dy);
	
	exercise(A_STR, TRUE);
	return((xlock.usedtime = 0));
}

#endif /* OVLB */
#ifdef OVL0

void
reset_pick()
{
	xlock.usedtime = xlock.chance = xlock.picktyp = 0;
	xlock.door = 0;
	xlock.box = 0;
}

#endif /* OVL0 */
#ifdef OVLB

int
pick_lock(pickp) /* pick a lock with a given object */
	struct	obj	**pickp;
{
	int picktyp, c, ch;
	coord cc;
	int key;
	struct rm	*door;
	struct obj	*otmp;
	struct	obj	*pick = *pickp;
	char qbuf[QBUFSZ];

	picktyp = pick->otyp;

	/* check whether we're resuming an interrupted previous attempt */
	if (xlock.usedtime && picktyp == xlock.picktyp) {
	    static char no_longer[] = "Unfortunately, you can no longer %s %s.";

	    if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER)) {
		const char *what = (picktyp == LOCK_PICK) ? "pick" : (picktyp == HAIRCLIP) ? "hairclip" : "key";
		if (picktyp == CREDIT_CARD) what = "card";
		if (picktyp == DATA_CHIP) what = "chip";
		pline(no_longer, "hold the", what);
		reset_pick();
		return 0;
	    } else if (xlock.box && !can_reach_floor()) {
		pline(no_longer, "reach the", "lock");
		reset_pick();
		return 0;
	    } else if (!xlock.door || xlock.key == pick->oartifact) {
		const char *action = lock_action();
		You("resume your attempt at %s.", action);
		set_occupation(picklock, action, 0);
		if (AutoDestruct || u.uprops[AUTO_DESTRUCT].extrinsic || (uarmf && uarmf->oartifact == ART_KHOR_S_REQUIRED_IDEA) || have_autodestructstone()) picklock();
		return(1);
	    }
	}

	if(nohands(youmonst.data) && !Race_if(PM_TRANSFORMER)) {
		You_cant("hold %s -- you have no hands!", doname(pick));
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		if (yn("Attempt it anyway?") == 'y') {
			if (rn2(3) && !polyskillchance()) {
				playerbleed(rnd(2 + (level_difficulty() * 10)));
				pline("Great. Now your %s is squirting everywhere.", body_part(BLOOD));
				if (!rn2(20)) badeffect();
				return 1;
			}

		}
		else return(0);
	}

	if((picktyp != LOCK_PICK &&
	    picktyp != HAIRCLIP &&
	    picktyp != CREDIT_CARD &&
	    picktyp != DATA_CHIP &&
	    picktyp != SECRET_KEY &&
	    picktyp != SKELETON_KEY)) {
		impossible("picking lock with object %d?", picktyp);
		return(0);
	}
	ch = 0;		/* lint suppression */

	if(!get_adjacent_loc((char *)0, "Invalid location!", u.ux, u.uy, &cc)) return 0;
	if (cc.x == u.ux && cc.y == u.uy) {	/* pick lock on a container */
	    const char *verb;
	    boolean it;
	    int count;

	    if (u.dz < 0) {
		There("isn't any sort of lock up %s.",
		      Levitation ? "here" : "there");
		return 0;
	    } else if (is_lava(u.ux, u.uy)) {
		pline("Doing that would probably melt your %s.",
		      xname(pick));
		return 0;
	    } else if (is_waterypool(u.ux, u.uy) && !is_crystalwater(u.ux, u.uy) && !Underwater) {
		pline_The("water has no lock.");
		return 0;
	    }

	    count = 0;
	    c = 'n';			/* in case there are no boxes here */
	    for(otmp = level.objects[cc.x][cc.y]; otmp; otmp = otmp->nexthere)
		if (Is_box(otmp)) {
		    ++count;
		    if (!can_reach_floor()) {
			You_cant("reach %s from up here.", the(xname(otmp)));
			return 0;
		    }
		    it = 0;
		    if (otmp->obroken) verb = "fix";
		    else if (!otmp->olocked) verb = "lock", it = 1;
		    else if (picktyp != LOCK_PICK && picktyp != HAIRCLIP) verb = "unlock", it = 1;
		    else verb = "pick";
		    sprintf(qbuf, "There is %s here, %s %s?",
		    	    safe_qbuf("", sizeof("There is  here, unlock its lock?"),
			    	doname(otmp), an(simple_typename(otmp->otyp)), "a box"),
			    verb, it ? "it" : "its lock");

		    c = ynq(qbuf);
		    if(c == 'q') return(0);
		    if(c == 'n') continue;

		    if (pick->oartifact && (pick->obrittle || pick->obrittle2) ) {
		      Your("key doesn't seem to fit.");
			return 0;
		    }

		    if (otmp->obroken) {
			You_cant("fix its broken lock with %s.", doname(pick));
			return 0;
		    }
		    else if (picktyp == CREDIT_CARD && !otmp->olocked) {
			/* credit cards are only good for unlocking */
			You_cant("do that with %s.", doname(pick));
			return 0;
		    }
		    else if (picktyp == DATA_CHIP && !otmp->olocked) {
			You_cant("do that with %s.", doname(pick));
			return 0;
		    }
		    switch(picktyp) {
			case CREDIT_CARD:
			    if(!rn2(isfriday ? 10 : 20) && (!pick->blessed || !rn2(3)) && !pick->oartifact) {
				Your("credit card breaks in half!");
				useup(pick);
				*pickp = (struct obj *)0;
				return(1);
			    }
			    if(!rn2(isfriday ? 10 : 20) && (!pick->blessed || !rn2(3)) && pick->oartifact) {
				Your("credit card becomes dull and is no longer capable of picking locks!");
				pick->obrittle = pick->obrittle2 = 3;
				return(1);
			    }
			    ch = ACURR(A_DEX) + 20*Role_if(PM_ROGUE) + 40*Role_if(PM_LOCKSMITH) + 20*Role_if(PM_CYBERNINJA);
			    break;
			case DATA_CHIP:
			    if(!rn2(isfriday ? 10 : 20) && (!pick->blessed || !rn2(3)) && !pick->oartifact) {
				Your("data chip breaks in half!");
				useup(pick);
				*pickp = (struct obj *)0;
				return(1);
			    }
			    if(!rn2(isfriday ? 10 : 20) && (!pick->blessed || !rn2(3)) && pick->oartifact) {
				Your("data chip becomes dull and is no longer capable of picking locks!");
				pick->obrittle = pick->obrittle2 = 3;
				return(1);
			    }
			    ch = ACURR(A_DEX) + 20*Role_if(PM_ROGUE) + 40*Role_if(PM_LOCKSMITH) + 20*Role_if(PM_CYBERNINJA);
			    break;
			case LOCK_PICK:
			case HAIRCLIP:
			    if(!rn2(isfriday ? 20 : Role_if(PM_LOCKSMITH) ? 60: (Role_if(PM_ROGUE) || Role_if(PM_CYBERNINJA)) ? 40 : 30) &&
			    		(!pick->blessed || !rn2(3)) && !pick->oartifact) {
				You("break your pick!");
				useup(pick);
				*pickp = (struct obj *)0;
				return(1);
			    }
			    if(!rn2(isfriday ? 20 : Role_if(PM_LOCKSMITH) ? 60: (Role_if(PM_ROGUE) || Role_if(PM_CYBERNINJA)) ? 40 : 30) &&
			    		(!pick->blessed || !rn2(3)) && pick->oartifact) {
				Your("pick becomes brittle and is no longer capable of picking locks!");
				pick->obrittle = pick->obrittle2 = 3;
				return(1);
			    }
			    ch = 4*ACURR(A_DEX) + 25*Role_if(PM_ROGUE) + 50*Role_if(PM_LOCKSMITH) + 30*Role_if(PM_CYBERNINJA);
			    break;
			case SKELETON_KEY:
			case SECRET_KEY:
			    if(!rn2(isfriday ? 7 : 15) && (!pick->blessed || !rn2(3)) && !pick->oartifact) {
				Your("key didn't quite fit the lock and snapped!");
				useup(pick);
				*pickp = (struct obj *)0;
				return(1);
			    }
			    if(!rn2(isfriday ? 7 : 15) && (!pick->blessed || !rn2(3)) && pick->oartifact) {
				Your("key becomes brittle and is no longer capable of picking locks!");
				pick->obrittle = pick->obrittle2 = 3;
				return(1);
			    }
			    ch = 75 + ACURR(A_DEX);
			    break;
			default:	ch = 0;
		    }
		    if(otmp->cursed) ch /= 2;

		    xlock.picktyp = picktyp;
		    xlock.box = otmp;
		    xlock.door = 0;
		    break;
		}
	    if (c != 'y') {
		if (!count)
		    There("doesn't seem to be any sort of lock here.");
		return(0);		/* decided against all boxes */
	    }
	} else {			/* pick the lock in a door */
	    struct monst *mtmp;

	    if (u.utrap && u.utraptype == TT_PIT) {
		You_cant("reach over the edge of the pit.");
		return(0);
	    }

	    door = &levl[cc.x][cc.y];
	    if ((mtmp = m_at(cc.x, cc.y)) && canseemon(mtmp)
			&& mtmp->m_ap_type != M_AP_FURNITURE
			&& mtmp->m_ap_type != M_AP_OBJECT) {
		if (picktyp == CREDIT_CARD &&
		    (mtmp->isshk || mtmp->data == &mons[PM_ORACLE]))
		    verbalize("No checks, no credit, no problem.");
		else
		    pline("I don't think %s would appreciate that.", mon_nam(mtmp));
		return(0);
	    }
	    if(!IS_DOOR(door->typ)) {
		if (is_drawbridge_wall(cc.x,cc.y) >= 0)
		    You("%s no lock on the drawbridge.",
				Blind ? "feel" : "see");
		else
		    You("%s no door there.",
				Blind ? "feel" : "see");
		return(0);
	    }
	    switch (door->doormask) {
		case D_NODOOR:
		    pline("This doorway has no door.");
		    return(0);
		case D_ISOPEN:
		    You("cannot lock an open door.");
		    return(0);
		case D_BROKEN:
		    pline("This door is broken.");
		    return(0);
		default:
		    /* credit cards are only good for unlocking */
		    if(picktyp == CREDIT_CARD && !(door->doormask & D_LOCKED)) {
			You_cant("lock a door with a credit card.");
			return(0);
		    }
		    if(picktyp == DATA_CHIP && !(door->doormask & D_LOCKED)) {
			You_cant("lock a door with a data chip.");
			return(0);
		    }
		    /* ALI - Artifact doors */
		    key = artifact_door(cc.x, cc.y);

		    sprintf(qbuf,"%sock it?",
			(door->doormask & D_LOCKED) ? "Unl" : "L" );

		    c = yn(qbuf);
		    if(c == 'n') return(0);

		    switch(picktyp) {
			case CREDIT_CARD:
			    if(!rn2(isfriday ? 10 : Role_if(PM_LOCKSMITH) ? 40 : (Role_if(PM_TOURIST) || Role_if(PM_CYBERNINJA)) ? 30 : 20) &&
				    (!pick->blessed || !rn2(3)) && !pick->oartifact) {
				You("break your card off in the door!");
				useup(pick);
				*pickp = (struct obj *)0;
				return(0);
			    }
			    if(!rn2(isfriday ? 10 : Role_if(PM_LOCKSMITH) ? 40 : (Role_if(PM_TOURIST) || Role_if(PM_CYBERNINJA)) ? 30 : 20) &&
				    (!pick->blessed || !rn2(3)) && pick->oartifact) {
				Your("credit card becomes dull and is no longer capable of picking locks!");
				pick->obrittle = pick->obrittle2 = 3;
				return(0);
			    }
			    ch = 2*ACURR(A_DEX) + 20*Role_if(PM_ROGUE) + 40*Role_if(PM_LOCKSMITH) + 20*Role_if(PM_CYBERNINJA);
			    break;
			case DATA_CHIP:
			    if(!rn2(isfriday ? 10 : Role_if(PM_LOCKSMITH) ? 40 : (Role_if(PM_TOURIST) || Role_if(PM_CYBERNINJA)) ? 30 : 20) &&
				    (!pick->blessed || !rn2(3)) && !pick->oartifact) {
				You("break your chip off in the door!");
				useup(pick);
				*pickp = (struct obj *)0;
				return(0);
			    }
			    if(!rn2(isfriday ? 10 : Role_if(PM_LOCKSMITH) ? 40 : (Role_if(PM_TOURIST) || Role_if(PM_CYBERNINJA)) ? 30 : 20) &&
				    (!pick->blessed || !rn2(3)) && pick->oartifact) {
				Your("data chip becomes dull and is no longer capable of picking locks!");
				pick->obrittle = pick->obrittle2 = 3;
				return(0);
			    }
			    ch = 2*ACURR(A_DEX) + 20*Role_if(PM_ROGUE) + 40*Role_if(PM_LOCKSMITH) + 20*Role_if(PM_CYBERNINJA);
			    break;
			case LOCK_PICK:
			case HAIRCLIP:
			    if(!rn2(isfriday ? 20 : Role_if(PM_LOCKSMITH) ? 60 : (Role_if(PM_ROGUE) || Role_if(PM_CYBERNINJA)) ? 40 : 30) &&
				    (!pick->blessed || !rn2(3)) && !pick->oartifact) {
				You("break your pick!");
				useup(pick);
				*pickp = (struct obj *)0;
				return(0);
			    }
			    if(!rn2(isfriday ? 20 : Role_if(PM_LOCKSMITH) ? 60 : (Role_if(PM_ROGUE) || Role_if(PM_CYBERNINJA)) ? 40 : 30) &&
				    (!pick->blessed || !rn2(3)) && pick->oartifact) {
				Your("pick becomes brittle and is no longer capable of picking locks!");
				pick->obrittle = pick->obrittle2 = 3;
				return(0);
			    }
			    ch = 3*ACURR(A_DEX) + 30*Role_if(PM_ROGUE) + 60*Role_if(PM_LOCKSMITH) + 30*Role_if(PM_CYBERNINJA);
			    break;
			case SKELETON_KEY:
			case SECRET_KEY:
			    if(!rn2(isfriday ? 7 : Role_if(PM_LOCKSMITH) ? 40 : Role_if(PM_CYBERNINJA) ? 30 : 15) && (!pick->blessed || !rn2(3)) && !pick->oartifact) {
				Your("key wasn't designed for this door and broke!");
				useup(pick);
				*pickp = (struct obj *)0;
				return(0);
			    }
			    if(!rn2(isfriday ? 7 : Role_if(PM_LOCKSMITH) ? 40 : Role_if(PM_CYBERNINJA) ? 30 : 15) && (!pick->blessed || !rn2(3)) && pick->oartifact) {
				Your("key becomes brittle and is no longer capable of picking locks!");
				pick->obrittle = pick->obrittle2 = 3;
				return(0);
			    }
			    ch = 70 + ACURR(A_DEX) + 10*Role_if(PM_LOCKSMITH) + 5*Role_if(PM_CYBERNINJA);
			    break;
			default:    ch = 0;
		    }
		    xlock.door = door;
		    xlock.box = 0;

		    /* ALI - Artifact doors */
		    xlock.key = pick->oartifact;
		    if (key && xlock.key != key) {
			if (picktyp == SKELETON_KEY || picktyp == SECRET_KEY) {
			    Your("key doesn't seem to fit.");
			    return(0);
			}
			else ch = -1;		/* -1 == 0% chance */
		    }

			/* artifact keys shouldn't be overpowered --Amy */

		    if (!key && pick->oartifact && (pick->obrittle || pick->obrittle2) && !issoviet) {
			    Your("key doesn't seem to fit.");
			    return(0);
		    }

	/* "Artifact key/locking tools work properly again. There is next to no reason for this, especially considering keys/lock picks/etc can break in this game." In Soviet Russia, people will tell you that there's no reason for something, and then proceed to tell you the EXACT reason for the same thing but pretend that it's the reason why the "something" shouldn't be! Can you believe that? To clarify: keys being able to break is the exact reason why I'm not allowing artifact keys to unlock everything, cause otherwise that would render the point of breakable keys completely moot. Once you got your artifact key, it just doesn't matter anymore since you're not going to use normal keys. And what *I* want is that it freaking *MATTERS* if you get lucky finding enough keys. The threat of running out should be present for the entire game, not just until you beat the way-too-easy neutral quest! */

	    }
	}
	flags.move = 0;
	xlock.chance = ch;
	xlock.picktyp = picktyp;
	xlock.usedtime = 0;
	set_occupation(picklock, lock_action(), 0);
	if (AutoDestruct || u.uprops[AUTO_DESTRUCT].extrinsic || (uarmf && uarmf->oartifact == ART_KHOR_S_REQUIRED_IDEA) || have_autodestructstone()) picklock();
	return(1);
}

int
doforce()		/* try to force a chest with your weapon */
{

	if (MenuIsBugged) {
	pline("The force command is currently unavailable!");
	if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return 0;
	}

	register struct obj *otmp;
	register int x, y, c, picktyp;
	struct rm       *door;
	char qbuf[QBUFSZ];

	if (!uwep) { /* Might want to make this so you use your shoulder */
	    You_cant("force anything without a weapon.");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	     return(0);
	}

	if (u.utrap && u.utraptype == TT_WEB) {
	    You("are entangled in a web!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    return(0);
	} else if (uwep && is_lightsaber(uwep)) {
	    if (!uwep->lamplit) {
		Your("lightsaber is deactivated!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return(0);
	    }
	} else if(uwep->otyp == LOCK_PICK ||
	    uwep->otyp == HAIRCLIP ||
	    uwep->otyp == CREDIT_CARD ||
	    uwep->otyp == DATA_CHIP ||
	    uwep->otyp == SECRET_KEY ||
	    uwep->otyp == SKELETON_KEY) {
	    	return pick_lock(&uwep);
	/* not a lightsaber or lockpicking device*/
	} else if(!uwep ||     /* proper type test */
	   (uwep->oclass != WEAPON_CLASS && !is_weptool(uwep) &&
	    uwep->oclass != ROCK_CLASS && uwep->oclass != BALL_CLASS && uwep->oclass != CHAIN_CLASS) ||
	   (objects[uwep->otyp].oc_skill < P_DAGGER) ||
	   (objects[uwep->otyp].oc_skill > P_LANCE && uwep->otyp != STEEL_WHIP) ||
	   uwep->otyp == FLAIL || uwep->otyp == AKLYS
	   || uwep->otyp == RUBBER_HOSE
	  ) {
	    You_cant("force anything without a %sweapon.",
		  (uwep) ? "proper " : "");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    return(0);
	}

	/* so we're trying to force something now, which means you touch your weapon; artifact can blast now --Amy */
	if (!touch_artifact(uwep, &youmonst)) return 0;

	if (is_lightsaber(uwep))
	    picktyp = 2;
	else
	picktyp = is_blade(uwep) ? 1 : 0;
	if(xlock.usedtime && picktyp == xlock.picktyp) {
	    if (xlock.box) {
		if (!can_reach_floor()) {
		    pline("Unfortunately, you can no longer reach the lock.");
		    return 0;
		}
		You("resume your attempt to force the lock.");
		set_occupation(forcelock, "forcing the lock", 0);
		if (AutoDestruct || u.uprops[AUTO_DESTRUCT].extrinsic || (uarmf && uarmf->oartifact == ART_KHOR_S_REQUIRED_IDEA) || have_autodestructstone()) forcelock();
		return(1);
	    } else if (xlock.door) {
		You("resume your attempt to force the door.");
		set_occupation(forcedoor, "forcing the door", 0);
		if (AutoDestruct || u.uprops[AUTO_DESTRUCT].extrinsic || (uarmf && uarmf->oartifact == ART_KHOR_S_REQUIRED_IDEA) || have_autodestructstone()) forcedoor();
		return(1);
	    }
	}

	/* A lock is made only for the honest man, the thief will break it. */
	xlock.box = (struct obj *)0;

	if(!getdir((char *)0)) return(0);

	x = u.ux + u.dx;
	y = u.uy + u.dy;
	if (x == u.ux && y == u.uy) {
	    if (u.dz < 0) {
		There("isn't any sort of lock up %s.",
		      Levitation ? "here" : "there");
		return 0;
	    } else if (is_lava(u.ux, u.uy)) {
		pline("Doing that would probably melt your %s.",
		      xname(uwep));
		return 0;
	    } else if (is_waterypool(u.ux, u.uy) && !is_crystalwater(u.ux, u.uy) && !Underwater) {
		pline_The("water has no lock.");
		return 0;
	    }

	    for(otmp = level.objects[u.ux][u.uy]; otmp; otmp = otmp->nexthere)
		if(Is_box(otmp)) {
		    if (!can_reach_floor()) {
			You_cant("reach %s from up here.", the(xname(otmp)));
		    return 0;
		    }

		if (otmp->obroken || !otmp->olocked) {
		    There("is %s here, but its lock is already %s.",
			  doname(otmp), otmp->obroken ? "broken" : "unlocked");
		    continue;
		}
		sprintf(qbuf,"There is %s here, force its lock?",
			safe_qbuf("", sizeof("There is  here, force its lock?"),
				doname(otmp), an(simple_typename(otmp->otyp)),
				"a box"));

		c = ynq(qbuf);
		if(c == 'q') return(0);
		if(c == 'n') continue;

		if(picktyp == 2)
		    You("begin melting it with your %s.", xname(uwep));
		else
		if(picktyp)
		    You("force your %s into a crack and pry.", xname(uwep));
		else
		    You("start bashing it with your %s.", xname(uwep));
		xlock.box = otmp;
		if (is_lightsaber(uwep))
		    xlock.chance = uwep->spe * 2 + 75;
		else
		    xlock.chance = (uwep->spe + objects[uwep->otyp].oc_wldam) * 2;
		xlock.picktyp = picktyp;
		xlock.usedtime = 0;
		break;
	    }
	    if(xlock.box)   {
	    	xlock.door = 0;
	    	set_occupation(forcelock, "forcing the lock", 0);
		if (AutoDestruct || u.uprops[AUTO_DESTRUCT].extrinsic || (uarmf && uarmf->oartifact == ART_KHOR_S_REQUIRED_IDEA) || have_autodestructstone()) forcelock();
	    	return(1);
	    }
	    if (u.dz > 0) {
		if (picktyp != 2) You("cannot find any sort of lock down there.");
		else {
		    	return use_pick_axe2(uwep);
		}
	    }

	} else {		/* break down/open door */
	    struct monst *mtmp;

	    door = &levl[x][y];
	    if ((mtmp = m_at(x, y)) && canseemon(mtmp)
			&& mtmp->m_ap_type != M_AP_FURNITURE
			&& mtmp->m_ap_type != M_AP_OBJECT) {

		if (mtmp->isshk || mtmp->data == &mons[PM_ORACLE])		
		  if (Role_if(PM_JEDI))
		    verbalize("Your puny Jedi tricks won't work on me!"); /* Return of the Jedi */
		  else

		    verbalize("What do you think you are, a Jedi?"); /* Phantom Menace */
		else {
			if (Role_if(PM_JEDI) ? (u.uen < 5) : Race_if(PM_BORG) ? (u.uen < 7) : (u.uen < 10) ) pline("I don't think %s would appreciate that. Besides, you need %d mana in order to use the force.", mon_nam(mtmp), Role_if(PM_JEDI) ? 5 : 10);
			else {

				if (!UseTheForce || rn2(StrongUseTheForce ? 3 : 10)) u.uen -= (Role_if(PM_JEDI) ? 5 : Race_if(PM_BORG) ? 7 : 10);

				int dmg;
				int mdx, mdy;
				dmg = rnd(2) + dbon() + uwep->spe;
				if (UseTheForce) dmg += 5;
				if (StrongUseTheForce) dmg += 5;
				if (tech_inuse(T_USE_THE_FORCE_LUKE)) dmg += techlevX(get_tech_no(T_USE_THE_FORCE_LUKE));
				if (uarmg && uarmg->oartifact == ART_USE_THE_FORCE_LUKE) dmg += 10;
				if (Role_if(PM_JEDI) && UseTheForce) dmg += u.ulevel;
				else if (Race_if(PM_BORG) && UseTheForce) dmg += rnd(u.ulevel);
				if (Role_if(PM_JEDI) && StrongUseTheForce) dmg += u.ulevel;
				else if (Race_if(PM_BORG) && StrongUseTheForce) dmg += rnd(u.ulevel);

				if (Role_if(PM_EMERA) && mtmp->data->msound == MS_SHOE) {
					dmg += rnd(2 * u.ulevel);
				}

				if (!PlayerCannotUseSkills) {
					switch (P_SKILL(P_WEDI)) {

						case P_BASIC:	dmg += (uwep && is_lightsaber(uwep) && uwep->lamplit) ? rnd(2) : 1; break;
						case P_SKILLED:	dmg += (uwep && is_lightsaber(uwep) && uwep->lamplit) ? rnd(4) : rnd(2); break;
						case P_EXPERT:	dmg += (uwep && is_lightsaber(uwep) && uwep->lamplit) ? rnd(6) : rnd(3); break;
						case P_MASTER:	dmg += (uwep && is_lightsaber(uwep) && uwep->lamplit) ? rnd(8) : rnd(4); break;
						case P_GRAND_MASTER:	dmg += (uwep && is_lightsaber(uwep) && uwep->lamplit) ? rnd(10) : rnd(5); break;
						case P_SUPREME_MASTER:	dmg += (uwep && is_lightsaber(uwep) && uwep->lamplit) ? rnd(12) : rnd(6); break;
						default: break;
					}
				}

				boolean trapkilled = FALSE;

				pline("You use the force on %s.", mon_nam(mtmp));
				u.cnd_forcecount++;

				if (Role_if(PM_EMERA) && mtmp->data->msound == MS_SHOE) pline("Your %s furiously rip into %s. You evil bastard.", makeplural(body_part(HAND)), mon_nam(mtmp));

				setmangry(mtmp);

				if (uwep && is_lightsaber(uwep) && uwep->lamplit) {
					u.uwediturns++;
					if (u.uwediturns >= 2) {
						u.uwediturns = 0;
						use_skill(P_WEDI, 1);
					}
				}

				if(mtmp->mtame) {
				    abuse_dog(mtmp);
				    if (mtmp->meating) {
					pline("Startled, %s spits out the food it was eating!", mon_nam(mtmp));
					mtmp->meating = 0;
					}
				    if (mtmp->mfrozen) {
					pline("Being hit by your force, %s is jolted back to its senses.", mon_nam(mtmp));
					mtmp->mfrozen = 0;
					}
				    if (mtmp->msleeping) {
					pline("Being hit by your force, %s suddenly wakes up!", mon_nam(mtmp));
					mtmp->msleeping = 0;
					}
				    mtmp->mcanmove = 1;
				    mtmp->masleep = 0;
				    if (mtmp->mtame)
					monflee(mtmp, (dmg ? rnd(dmg) : 1), FALSE, FALSE);
				    else
					mtmp->mflee = 0;
				}

				if (dmg > 0) {
					mtmp->mhp -= dmg;
#ifdef SHOW_DMG
					showdmg(dmg);
#endif
				}

				if (mtmp->mhp > 0 && ( (UseTheForce && uwep && is_lightsaber(uwep) && uwep->lamplit && rn2(2) ) || (StrongUseTheForce && uwep && is_lightsaber(uwep) && uwep->lamplit && rn2(2) ) || (Role_if(PM_JEDI) ? (rnd(100) < (u.ulevel * 2) ) : (rnd(100) < u.ulevel) ) ) &&
	    mtmp->mcanmove && mtmp != u.ustuck && !mtmp->mtrapped) {
		/* see if the monster has a place to move into */
				mdx = mtmp->mx + u.dx;
				mdy = mtmp->my + u.dy;
				if(goodpos(mdx, mdy, mtmp, 0)) {
					pline("%s is pushed back!", Monnam(mtmp));
					if (m_in_out_region(mtmp, mdx, mdy)) {
					    remove_monster(mtmp->mx, mtmp->my);
					    newsym(mtmp->mx, mtmp->my);
					    place_monster(mtmp, mdx, mdy);
					    newsym(mtmp->mx, mtmp->my);
					    set_apparxy(mtmp);
					    if (mintrap(mtmp) == 2) trapkilled = TRUE;
					    }
					}
				}

				(void) passive(mtmp, TRUE, mtmp->mhp > 0, AT_TUCH, FALSE);
				if (mtmp->mhp <= 0 && !trapkilled) killed(mtmp);

				if (mtmp->mhp > 0 && (mtmp->data->msound == MS_FART_QUIET || (!rn2(5) && mtmp->egotype_farter) ) ) {
					pline("You bash %s's %s butt using %s %s.", mon_nam(mtmp), mtmp->female ? "sexy" : "ugly", !rn2(3) ? "both your left and right" : rn2(2) ? "your left" : "your right", body_part(HAND) );
					if (practicantterror) {
						pline("%s thunders: 'Bashing other people's butts is not permitted! 100 zorkmids!'", noroelaname());
						fineforpracticant(100, 0, 0);

					}
					if (Role_if(PM_BUTT_LOVER)) {
						You_feel("bad for hurting one of your beloved butts!");
						adjalign(-5);
						if (u.negativeprotection > 0 && !rn2(5)) u.negativeprotection--;
					}
					u.cnd_forcebuttcount++;
					if (mtmp->butthurt < 20 && (!rn2(3) || Role_if(PM_EMERA)) ) {
						mtmp->butthurt += rnd(5);
						if (mtmp->butthurt < 5) pline("%s's %s butt is getting %s red bruises.", Monnam(mtmp), mtmp->female ? "sexy" : "ugly", mtmp->female ? "beautiful" : "intense");
						else if (mtmp->butthurt < 9) pline("%s's %s butt is getting sore from your beating.", Monnam(mtmp), mtmp->female ? "sexy" : "ugly");
						else if (mtmp->butthurt < 13) pline("%s's %s butt is hurt badly, and blood is slowly dripping out...", Monnam(mtmp), mtmp->female ? "sexy" : "ugly");
						else if (mtmp->butthurt < 17) pline("%s's %s butt is heavily injured and severely bleeding!", Monnam(mtmp), mtmp->female ? "sexy" : "ugly");
						else {
							pline("You hurt %s so badly that %s ended up crying, and is begging you to spare %s...", mon_nam(mtmp), mhe(mtmp), mhim(mtmp) );
							if (!mtmp->mfrenzied) mtmp->mpeaceful = 1;
							mtmp->butthurt = 0;
						}
					}
				}
				if (mtmp->mhp > 0 && (mtmp->data->msound == MS_FART_NORMAL || (!rn2(5) && mtmp->egotype_farter) ) ) {
					pline("You bash %s's %s butt using %s %s.", mon_nam(mtmp), mtmp->female ? "sexy" : "ugly", !rn2(3) ? "both your left and right" : rn2(2) ? "your left" : "your right", body_part(HAND) );
					if (practicantterror) {
						pline("%s thunders: 'Bashing other people's butts is not permitted! 100 zorkmids!'", noroelaname());
						fineforpracticant(100, 0, 0);

					}
					if (Role_if(PM_BUTT_LOVER)) {
						You_feel("bad for hurting one of your beloved butts!");
						adjalign(-5);
						if (u.negativeprotection > 0 && !rn2(5)) u.negativeprotection--;
					}
					u.cnd_forcebuttcount++;
					if (mtmp->butthurt < 20 && (!rn2(3) || Role_if(PM_EMERA)) ) {
						mtmp->butthurt += rnd(3);
						if (mtmp->butthurt < 5) pline("%s's %s butt is getting %s red bruises.", Monnam(mtmp), mtmp->female ? "sexy" : "ugly", mtmp->female ? "beautiful" : "intense");
						else if (mtmp->butthurt < 9) pline("%s's %s butt is getting sore from your beating.", Monnam(mtmp), mtmp->female ? "sexy" : "ugly");
						else if (mtmp->butthurt < 13) pline("%s's %s butt is hurt badly, and blood is slowly dripping out...", Monnam(mtmp), mtmp->female ? "sexy" : "ugly");
						else if (mtmp->butthurt < 17) pline("%s's %s butt is heavily injured and severely bleeding!", Monnam(mtmp), mtmp->female ? "sexy" : "ugly");
						else {
							pline("You hurt %s so badly that %s gets unconscious from the pain.", mon_nam(mtmp), mhe(mtmp));
							mtmp->mcanmove = 0;
							mtmp->mfrozen = 5 + rnd(15);
							mtmp->mstrategy &= ~STRAT_WAITFORU;
							mtmp->butthurt = 0;
						}
					}
				}
				if (mtmp->mhp > 0 && (mtmp->data->msound == MS_FART_LOUD || (!rn2(5) && mtmp->egotype_farter) ) ) {
					pline("You bash %s's %s butt using %s %s.", mon_nam(mtmp), mtmp->female ? "sexy" : "ugly", !rn2(3) ? "both your left and right" : rn2(2) ? "your left" : "your right", body_part(HAND) );
					if (practicantterror) {
						pline("%s thunders: 'Bashing other people's butts is not permitted! 100 zorkmids!'", noroelaname());
						fineforpracticant(100, 0, 0);

					}
					if (Role_if(PM_BUTT_LOVER)) {
						You_feel("bad for hurting one of your beloved butts!");
						adjalign(-5);
						if (u.negativeprotection > 0 && !rn2(5)) u.negativeprotection--;
					}
					u.cnd_forcebuttcount++;
					if (mtmp->butthurt < 20 && (!rn2(3) || Role_if(PM_EMERA)) ) {
						mtmp->butthurt += 1;
						if (mtmp->butthurt < 5) pline("%s's %s butt is getting %s red bruises.", Monnam(mtmp), mtmp->female ? "sexy" : "ugly", mtmp->female ? "beautiful" : "intense");
						else if (mtmp->butthurt < 9) pline("%s's %s butt is getting sore from your beating.", Monnam(mtmp), mtmp->female ? "sexy" : "ugly");
						else if (mtmp->butthurt < 13) pline("%s's %s butt is hurt badly, and blood is slowly dripping out...", Monnam(mtmp), mtmp->female ? "sexy" : "ugly");
						else if (mtmp->butthurt < 17) pline("%s's %s butt is heavily injured and severely bleeding!", Monnam(mtmp), mtmp->female ? "sexy" : "ugly");
						else {
							pline("You hurt %s so badly that %s is furious, and realize that you're going to learn a lesson in pain!", mon_nam(mtmp), mhe(mtmp));
							mtmp->mpeaceful = mtmp->mtame = 0;
							mon_adjust_speed(mtmp, 2, (struct obj *)0);
							mtmp->butthurt = 0;
							if (!grow_up(mtmp,(struct monst *)0)) return 1;
							if (!grow_up(mtmp,(struct monst *)0)) return 1;
							if (!grow_up(mtmp,(struct monst *)0)) return 1;
						}
					}
				}

				return(1);

			} /* monster forced by player */

		}
		return(0);
	    }

	    if (is_farmland(x,y)) {
			if (u.uen < 5) {
				pline("You have too little energy to force the farmland!");
				return(0);
			} else {
				u.uen -= 5;
				pline("You fire an ammo at the farmland.");
				if (rn2(20)) pline("It doesn't seem to have any effect.");
				else {
					pline("The farmland disappears.");
					u.cnd_farmlandremoved++;
					levl[x][y].typ = CORR;
					newsym(x,y);
					blockorunblock_point(x,y);
					more_experienced(25 * (deepest_lev_reached(FALSE) + 1), 0);
					newexplevel();
				}
				return(1);
			}
		}

	    /* Lightsabers dig through doors and walls via dig.c */
	    if (is_pick(uwep) || is_antibar(uwep) ||
		    is_lightsaber(uwep) ||
		    is_axe(uwep)) 
	    	return use_pick_axe2(uwep);

	    if(!IS_DOOR(door->typ)) { 
		if (is_drawbridge_wall(x,y) >= 0)
		    pline("The drawbridge is too solid to force open.");
		else
		    You("%s no door there.",
				Blind ? "feel" : "see");
		return(0);
	    }
	    /* ALI - artifact doors */
	    if (artifact_door(x, y)) {
		pline("This door is too solid to force open.");
		return 0;
	    }
	    switch (door->doormask) {
		case D_NODOOR:
		    pline("This doorway has no door.");
		    return(0);
		case D_ISOPEN:
		    You("cannot force an open door.");
		    return(0);
		case D_BROKEN:
		    pline("This door is broken.");
		    return(0);
		default:
		    c = yn("Break down the door?");
		    if(c == 'n') return(0);

		    if(picktyp == 1)
			You("force your %s into a crack and pry.", xname(uwep));
		    else
			You("start bashing it with your %s.", xname(uwep));
		    if (is_lightsaber(uwep))
			xlock.chance = uwep->spe + 38;
		    else
			xlock.chance = uwep->spe + objects[uwep->otyp].oc_wldam;
		    xlock.picktyp = picktyp;
		    xlock.usedtime = 0;    
		    xlock.door = door;
		    xlock.box = 0;
		    set_occupation(forcedoor, "forcing the door", 0);
		    if (AutoDestruct || u.uprops[AUTO_DESTRUCT].extrinsic || (uarmf && uarmf->oartifact == ART_KHOR_S_REQUIRED_IDEA) || have_autodestructstone()) forcedoor();
	return(1);
	    }
	}
	You("decide not to force the issue.");
	return(0);
}

int
doopen()		/* try to open a door */
{
	return doopen_indir(0, 0);
}

int
doopen_indir(x, y)		/* try to open a door in direction u.dx/u.dy */
	int x, y;		/* if true, prompt for direction */
{
	coord cc;
	register struct rm *door;
	struct monst *mtmp;

	if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER)) {
	    You_cant("open anything -- you have no hands!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

		if (yn("Try to open it with another part of your body instead?") == 'y') {
			if (rn2(3) && !polyskillchance()) {
	 			make_blinded(Blinded + rnd(50),TRUE);
				pline("Off - you just blinded yourself!");
				if (!rn2(20)) badeffect();
				if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		 		return 1;
			}
		}
		else {return(0);}
	}

	if (u.utrap && u.utraptype == TT_PIT) {
	    You_cant("reach over the edge of the pit.");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    return 0;
	}

	if (x > 0 && y > 0) {
	    cc.x = x;
	    cc.y = y;
	}
	else if(!get_adjacent_loc((char *)0, (char *)0, u.ux, u.uy, &cc)) return(0);

	if((cc.x == u.ux) && (cc.y == u.uy)) return(0);

	if ((mtmp = m_at(cc.x,cc.y))			&&
		mtmp->m_ap_type == M_AP_FURNITURE	&&
		(mtmp->mappearance == S_hcdoor ||
			mtmp->mappearance == S_vcdoor)	&&
		!Protection_from_shape_changers)	 {

	    stumble_onto_mimic(mtmp);
	    return(1);
	}

	door = &levl[cc.x][cc.y];

	if(!IS_DOOR(door->typ)) {
		if (is_db_wall(cc.x,cc.y)) {
		    There("is no obvious way to open the drawbridge.");
		    return(0);
		}
		You("%s no door there.",
				Blind ? "feel" : "see");
		return(0);
	}

	if (!(door->doormask & D_CLOSED)) {
	    const char *mesg;

	    switch (door->doormask) {
	    case D_BROKEN: mesg = " is broken"; break;
	    case D_NODOOR: mesg = "way has no door"; break;
	    case D_ISOPEN: mesg = " is already open"; break;
	    default:	   mesg = " is locked"; break;
	    }
	    pline("This door%s.", mesg);
	    if (Blind) feel_location(cc.x,cc.y);
	    return(0);
	}

	if(verysmall(youmonst.data) && !Race_if(PM_TRANSFORMER) ) {
	    pline("You're too small to pull the door open.");
	    return(0);
	}

	if (uwep && uwep->oartifact == ART_DUURVOID && (door->doormask & D_TRAPPED) && !rn2(5)) {
		pline("There seems to be a trap on this door!");
		if (yn("Stop handling the door?") == 'y') {
			return(0);
		}
	}

	/* door is known to be CLOSED */
	if (rnl(20) < (ACURRSTR+ACURR(A_DEX)+ACURR(A_CON))/3) {
	    pline_The("door opens.");
	    if(door->doormask & D_TRAPPED) {
		b_trapped("door", FINGER);
		door->doormask = D_NODOOR;
		if (*in_rooms(cc.x, cc.y, SHOPBASE)) add_damage(cc.x, cc.y, 0L);
	    } else
		door->doormask = D_ISOPEN;
	    if (Blind)
		feel_location(cc.x,cc.y);	/* the hero knows she opened it  */
	    else
		newsym(cc.x,cc.y);
	    unblock_point(cc.x,cc.y);		/* vision: new see through there */
	} else {
	    exercise(A_STR, TRUE);
	    pline_The("door resists!");
	}

	return(1);
}

STATIC_OVL
boolean
obstructed(x,y)
register int x, y;
{
	register struct monst *mtmp = m_at(x, y);

	if(mtmp && mtmp->m_ap_type != M_AP_FURNITURE) {
		if (mtmp->m_ap_type == M_AP_OBJECT) goto objhere;
		pline("%s stands in the way!", !canspotmon(mtmp) ?
			"Some creature" : Monnam(mtmp));
		if (!canspotmon(mtmp))
		    map_invisible(mtmp->mx, mtmp->my);
		return(TRUE);
	}
	if (OBJ_AT(x, y)) {
objhere:	pline("%s's in the way.", Something);
		return(TRUE);
	}
	return(FALSE);
}

STATIC_OVL
boolean
obstructedX(x,y)
register int x, y;
{
	register struct monst *mtmp = m_at(x, y);

	if(mtmp && mtmp->m_ap_type != M_AP_FURNITURE) {
		if (mtmp->m_ap_type == M_AP_OBJECT) goto objhere;
		if (!canspotmon(mtmp))
		    map_invisible(mtmp->mx, mtmp->my);
		return(TRUE);
	}
	if (OBJ_AT(x, y)) {
objhere:	return(TRUE);
	}
	return(FALSE);
}

int
doclose()		/* try to close a door */
{
	register int x, y;
	register struct rm *door;
	struct monst *mtmp;

	if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER)) {
	    You_cant("close anything -- you have no hands!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

		if (yn("Try to close it with another part of your body instead?") == 'y') {
			if (rn2(3) && !polyskillchance()) {
	 			make_blinded(Blinded + rnd(50),TRUE);
				pline("Something got in your face! You can't see!");
				if (!rn2(20)) badeffect();
				if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
				return 1;
			}
		}
		else {return(0);}
	}

	if (u.utrap && u.utraptype == TT_PIT) {
	    You_cant("reach over the edge of the pit.");
	    if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    return 0;
	}

	if(!getdir((char *)0)) return(0);

	x = u.ux + u.dx;
	y = u.uy + u.dy;
	if((x == u.ux) && (y == u.uy)) {
		You("are in the way!");
		return(1);
	}

	if ((mtmp = m_at(x,y))				&&
		mtmp->m_ap_type == M_AP_FURNITURE	&&
		(mtmp->mappearance == S_hcdoor ||
			mtmp->mappearance == S_vcdoor)	&&
		!Protection_from_shape_changers)	 {

	    stumble_onto_mimic(mtmp);
	    return(1);
	}

	door = &levl[x][y];

	if(!IS_DOOR(door->typ)) {
		if (door->typ == DRAWBRIDGE_DOWN)
		    There("is no obvious way to close the drawbridge.");
		else
		    You("%s no door there.",
				Blind ? "feel" : "see");
		return(0);
	}

	if(door->doormask == D_NODOOR) {
	    pline("This doorway has no door.");
	    return(0);
	}

	if(obstructed(x, y)) return(0);

	if(door->doormask == D_BROKEN) {
	    pline("This door is broken.");
	    return(0);
	}

	if(door->doormask & (D_CLOSED | D_LOCKED)) {
	    pline("This door is already closed.");
	    return(0);
	}

	if(door->doormask == D_ISOPEN) {
	    if(verysmall(youmonst.data) && !Race_if(PM_TRANSFORMER) 
		&& !u.usteed ) {
		 pline("You're too small to push the door closed.");
		 return(0);
	    }
	    if (u.usteed || rn2(25) < (ACURRSTR+ACURR(A_DEX)+ACURR(A_CON))/3) {
		pline_The("door closes.");
		door->doormask = D_CLOSED;
		if (Blind)
		    feel_location(x,y);	/* the hero knows she closed it */
		else
		    newsym(x,y);
		block_point(x,y);	/* vision:  no longer see there */
	    }
	    else {
	        exercise(A_STR, TRUE);
	        pline_The("door resists!");
	    }
	}

	return(1);
}

boolean			/* box obj was hit with spell effect otmp */
boxlock(obj, otmp)	/* returns true if something happened */
register struct obj *obj, *otmp;	/* obj *is* a box */
{
	register boolean res = 0;

	switch(otmp->otyp) {
	case WAN_LOCKING:
	    if (!obj->olocked) {	/* lock it; fix if broken */
		pline("Klunk!");
		obj->olocked = 1;
		obj->obroken = 0;
		res = 1;
	    } /* else already closed and locked */
	    break;
	case SPE_WIZARD_LOCK:
	    if (Role_if(PM_LOCKSMITH) ? !rn2(100) : rn2(2)) {
		if (!rn2(10)) containerkaboom();
		break;
	    }
	    if (!obj->olocked) {	/* lock it; fix if broken */
		pline("Klunk!");
		obj->olocked = 1;
		obj->obroken = 0;
		res = 1;
	    } /* else already closed and locked */
	    break;
	case WAN_OPENING:
	    if (obj->olocked) {		/* unlock; couldn't be broken */
		pline("Klick!");
		obj->olocked = 0;
		res = 1;
	    } else			/* silently fix if broken */
		obj->obroken = 0;
	    break;
	case SPE_KNOCK:
	    if (Role_if(PM_LOCKSMITH) ? !rn2(50) : rn2(3)) {
		if (!rn2(10)) containerkaboom();
		break;
	    }
	    if (obj->olocked) {		/* unlock; couldn't be broken */
		pline("Klick!");
		obj->olocked = 0;
		res = 1;
	    } else			/* silently fix if broken */
		obj->obroken = 0;
	    break;
	case SPE_LOCK_MANIPULATION:

	    if (Role_if(PM_LOCKSMITH) ? !rn2(50) : rn2(3)) {
		if (!rn2(10)) containerkaboom();
		break;
	    }

		if (!rn2(2)) {
		    if (!obj->olocked) {	/* lock it; fix if broken */
			pline("Klunk!");
			obj->olocked = 1;
			obj->obroken = 0;
			res = 1;
		    } /* else already closed and locked */

		} else {

		    if (obj->olocked) {		/* unlock; couldn't be broken */
			pline("Klick!");
			obj->olocked = 0;
			res = 1;
		    } else			/* silently fix if broken */
			obj->obroken = 0;

		}

	    break;
	case WAN_POLYMORPH:
	case WAN_MUTATION:
	case SPE_POLYMORPH:
	case SPE_MUTATION:
	    /* maybe start unlocking chest, get interrupted, then zap it;
	       we must avoid any attempt to resume unlocking it */
	    if (xlock.box == obj)
		reset_pick();
	    break;
	}
	return res;
}

boolean
doorlockX(x,y,update)
int x, y;
boolean update;

{
	register struct rm *door = &levl[x][y];
	boolean res = TRUE;
	int key = artifact_door(x, y);		/* ALI - Artifact doors */

	if (levl[x][y].typ != SDOOR && levl[x][y].typ != DOOR) return FALSE;

		/* Amy edit: sigh. The obstructed check was *also* causing savegame errors. Anyway, there's no real reason
		 * for doors to not close over monsters or objects, so we'll simply allow that to happen. */

	    /*if (obstructedX(x,y)) return FALSE;*/
	    /* Don't allow doors to close over traps.  This is for pits */
	    /* & trap doors, but is it ever OK for anything else? */
	    /*if (t_at(x,y)) {*/
		/* maketrap() clears doormask, so it should be NODOOR */
		/*return FALSE;
	    }*/

	    block_point(x, y);
	    if (key)
		door->doormask = D_CLOSED | (door->doormask & D_TRAPPED);
	    else
	    door->doormask = D_LOCKED | (door->doormask & D_TRAPPED);
	    if (update) newsym(x,y);

	if (update && res && picking_at(x, y)) {
	    /* maybe unseen monster zaps door you're unlocking */
	    stop_occupation();
	    reset_pick();
	}
	return res;
}

boolean			/* Door/secret door was hit with spell effect otmp */
doorlock(otmp,x,y)	/* returns true if something happened */
struct obj *otmp;
int x, y;
{
	register struct rm *door = &levl[x][y];
	boolean res = TRUE;
	int loudness = 0;
	const char *msg = (const char *)0;
	const char *dustcloud = "A cloud of dust";
	const char *quickly_dissipates = "quickly dissipates";
	int key = artifact_door(x, y);		/* ALI - Artifact doors */
	
	if (door->typ == SDOOR) {
	    switch (otmp->otyp) {

	    case WAN_OPENING:
	    case WAN_STRIKING:
	    case WAN_GRAVITY_BEAM:
	    case SPE_GRAVITY_BEAM:
	    case WAN_DISINTEGRATION:
	    case SPE_FORCE_BOLT:
		if (key)	/* Artifact doors are revealed only */
		    cvt_sdoor_to_door(door);
		else {
		door->typ = DOOR;
		door->doormask = D_CLOSED | (door->doormask & D_TRAPPED);
		}
		newsym(x,y);
		if (cansee(x,y)) pline("A door appears in the wall!");
		if (otmp->otyp == WAN_OPENING || otmp->otyp == SPE_LOCK_MANIPULATION)
		    return TRUE;
		break;		/* striking: continue door handling below */

	    case SPE_LOCK_MANIPULATION:
		if (rn2(2)) return FALSE;
		if (Role_if(PM_LOCKSMITH) ? !rn2(50) : rn2(3)) {
			if (!rn2(10)) containerkaboom();
			return TRUE;
		}

		if (key)	/* Artifact doors are revealed only */
		    cvt_sdoor_to_door(door);
		else {
		door->typ = DOOR;
		door->doormask = D_CLOSED | (door->doormask & D_TRAPPED);
		}
		newsym(x,y);
		if (cansee(x,y)) pline("A door appears in the wall!");
		return TRUE;

	    case SPE_KNOCK:
		if (Role_if(PM_LOCKSMITH) ? !rn2(50) : rn2(3)) {
			if (!rn2(10)) containerkaboom();
			return TRUE;
		}

		if (key)	/* Artifact doors are revealed only */
		    cvt_sdoor_to_door(door);
		else {
		door->typ = DOOR;
		door->doormask = D_CLOSED | (door->doormask & D_TRAPPED);
		}
		newsym(x,y);
		if (cansee(x,y)) pline("A door appears in the wall!");
		return TRUE;
	    case WAN_LOCKING:
	    case SPE_WIZARD_LOCK:
	    default:
		return FALSE;
	    }
	}

	switch(otmp->otyp) {

	case SPE_LOCK_MANIPULATION:

		if (!rn2(2)) {
		    if (!key && door->doormask & D_LOCKED) {
			msg = "The door unlocks!";
			door->doormask = D_CLOSED | (door->doormask & D_TRAPPED);
		    } else res = FALSE;
		    break;
		} /* else fall through */
	case WAN_LOCKING:
	case SPE_WIZARD_LOCK:

	    if (otmp->otyp == SPE_WIZARD_LOCK && (Role_if(PM_LOCKSMITH) ? !rn2(150) : !rn2(3))) {
		if (!rn2(10)) containerkaboom();
		return FALSE;
	    }
	    if (otmp->otyp == SPE_LOCK_MANIPULATION && (Role_if(PM_LOCKSMITH) ? !rn2(50) : rn2(3))) {
		if (!rn2(10)) containerkaboom();
		return FALSE;
	    }

	    if (door->doormask == D_BROKEN && otmp->otyp == SPE_WIZARD_LOCK && (Role_if(PM_LOCKSMITH) ? !rn2(50) : rn2(3))) {
		if (!rn2(10)) containerkaboom();
		return FALSE;
	    }
	    if (door->doormask == D_NODOOR && otmp->otyp == SPE_WIZARD_LOCK && (Role_if(PM_LOCKSMITH) ? !rn2(100) : rn2(2))) {
		if (!rn2(10)) containerkaboom();
		return FALSE;
	    }


#ifdef REINCARNATION
	    if (Is_rogue_level(&u.uz)) {
	    	boolean vis = cansee(x,y);
		/* Can't have real locking in Rogue, so just hide doorway */
		if (vis) pline("%s springs up in the older, more primitive doorway.",
			dustcloud);
		else
			You_hear("a swoosh.");
		if (obstructed(x,y)) {
			if (vis) pline_The("cloud %s.",quickly_dissipates);
			return FALSE;
		}
		block_point(x, y);
		door->typ = SDOOR;
		if (vis) pline_The("doorway vanishes!");
		newsym(x,y);
		return TRUE;
	    }
#endif
	    if (obstructed(x,y)) return FALSE;
	    /* Don't allow doors to close over traps.  This is for pits */
	    /* & trap doors, but is it ever OK for anything else? */
	    if (t_at(x,y)) {
		/* maketrap() clears doormask, so it should be NODOOR */
		pline(
		"%s springs up in the doorway, but %s.",
		dustcloud, quickly_dissipates);
		return FALSE;
	    }

	    switch (door->doormask & ~D_TRAPPED) {
	    case D_CLOSED:
		if (key)
		    msg = "The door closes!";
		else
		msg = "The door locks!";
		break;
	    case D_ISOPEN:
		if (key)
		    msg = "The door swings shut!";
		else
		msg = "The door swings shut, and locks!";
		break;
	    case D_BROKEN:
		if (key)
		    msg = "The broken door reassembles!";
		else
		msg = "The broken door reassembles and locks!";
		break;
	    case D_NODOOR:
		msg =
		"A cloud of dust springs up and assembles itself into a door!";
		break;
	    default:
		res = FALSE;
		break;
	    }
	    block_point(x, y);
	    if (key)
		door->doormask = D_CLOSED | (door->doormask & D_TRAPPED);
	    else
	    door->doormask = D_LOCKED | (door->doormask & D_TRAPPED);
	    newsym(x,y);
	    break;
	case WAN_OPENING:
	    if (!key && door->doormask & D_LOCKED) {
		msg = "The door unlocks!";
		door->doormask = D_CLOSED | (door->doormask & D_TRAPPED);
	    } else res = FALSE;
	    break;
	case SPE_KNOCK:
	    if (Role_if(PM_LOCKSMITH) ? !rn2(50) : rn2(3)) {
		if (!rn2(10)) containerkaboom();
		break;
	    }
	    if (!key && door->doormask & D_LOCKED) {
		msg = "The door unlocks!";
		door->doormask = D_CLOSED | (door->doormask & D_TRAPPED);
	    } else res = FALSE;
	    break;
	case WAN_STRIKING:
	case WAN_GRAVITY_BEAM:
	case WAN_DISINTEGRATION:
	case SPE_FORCE_BOLT:
	case SPE_GRAVITY_BEAM:
	case WAN_WIND:
	case SPE_WIND:
	    if (!key && door->doormask & (D_LOCKED | D_CLOSED)) {
		if (door->doormask & D_TRAPPED) {
		    if (MON_AT(x, y))
			(void) mb_trapped(m_at(x,y));
		    else if (flags.verbose) {
			if (cansee(x,y))
			    pline("KABOOM!!  You see a door explode.");
			else if (flags.soundok)
			    You_hear("a distant explosion.");
		    }
		    door->doormask = D_NODOOR;
		    unblock_point(x,y);
		    newsym(x,y);
		    loudness = 40;
		    break;
		}
		door->doormask = D_BROKEN;
		if (flags.verbose) {
		    if (cansee(x,y))
			pline_The("door crashes open!");
		    else if (flags.soundok)
			You_hear("a crashing sound.");
		}
		unblock_point(x,y);
		newsym(x,y);
		/* force vision recalc before printing more messages */
		if (vision_full_recalc) vision_recalc(0);
		loudness = 20;
	    } else res = FALSE;
	    break;
	default: impossible("magic (%ld) attempted on door.", otmp->otyp);
	    break;
	}
	if (msg && cansee(x,y)) pline("%s", msg);
	if (loudness > 0) {
	    /* door was destroyed */
	    wake_nearto(x, y, loudness);
	    if (*in_rooms(x, y, SHOPBASE)) add_damage(x, y, 0L);
	}

	if (res && picking_at(x, y)) {
	    /* maybe unseen monster zaps door you're unlocking */
	    stop_occupation();
	    reset_pick();
	}
	return res;
}

STATIC_OVL void
chest_shatter_msg(otmp)
struct obj *otmp;
{
	const char *disposition;
	const char *thing;
	long save_Blinded;

	if (otmp->oclass == POTION_CLASS) {
		You("%s %s shatter!", Blind ? "hear" : "see", an(bottlename()));
		if (!breathless(youmonst.data) || haseyes(youmonst.data))
			potionbreathe(otmp);
		return;
	}
	/* We have functions for distant and singular names, but not one */
	/* which does _both_... */
	save_Blinded = Blinded;
	Blinded = 1;
	thing = singular(otmp, xname);
	Blinded = save_Blinded;
	/* Amy grepping target: "materialeffect" */
	switch (objects[otmp->otyp].oc_material) {
	case MT_PAPER:	disposition = "is torn to shreds";
		break;
	case MT_WAX:	disposition = "is crushed";
		break;
	case MT_VEGGY:	disposition = "is pulped";
		break;
	case MT_FLESH:	disposition = "is mashed";
		break;
	case MT_FOAM:	disposition = "is mashed";
		break;
	case MT_TAR:	disposition = "breaks apart";
		break;
	case MT_GLASS:	disposition = "shatters";
		break;
	case MT_VIVA:	disposition = "disintegrates";
		break;
	case MT_SECREE:	disposition = "decomposes";
		break;
	case MT_WOOD:	disposition = "splinters to fragments";
		break;
	case MT_SHADOWSTUFF:	disposition = "is swallowed";
		break;
	default:	disposition = "is destroyed";
		break;
	}
	pline("%s %s!", An(thing), disposition);
}

/* ALI - Kevin Hugo's artifact doors.
 * Return the artifact which unlocks the door at (x, y), or
 * zero if it is an ordinary door.
 * Note: Not all doors are listed in the doors array (eg., doors
 * dynamically converted from secret doors). Since only trapped
 * and artifact doors are needed this isn't a problem. If we ever
 * implement trapped secret doors we will have to extend this.
 */

int
artifact_door(x, y)
int x, y;
{
    int i;

    for(i = 0; i < doorindex; i++) {
	if (x == doors[i].x && y == doors[i].y)
	    return doors[i].arti_key;
    }
    return 0;
}

#endif /* OVLB */

/*lock.c*/
