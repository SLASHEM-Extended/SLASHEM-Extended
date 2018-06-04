/*	SCCS Id: @(#)potion.c	3.4	2002/10/02	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"


/* KMH, intrinsics patch
 * There are many changes here to support >32-bit properties.
 * Also, blessed potions are once again permitted to convey
 * permanent intrinsics.
 */


#ifdef OVLB
boolean notonhead = FALSE;

static NEARDATA int nothing, unkn;
static NEARDATA const char beverages[] = { POTION_CLASS, 0 };

STATIC_DCL long itimeout(long);
STATIC_DCL long itimeout_incr(long,int);
STATIC_DCL void ghost_from_bottle(void);
STATIC_DCL short mixtype(struct obj *,struct obj *);
STATIC_PTR void set_litI(int,int,void *);

STATIC_DCL void healup_mon(struct monst *, int,int,BOOLEAN_P,BOOLEAN_P);
	/* For healing monsters - analogous to healup for players */


/* force `val' to be within valid range for intrinsic timeout value */
STATIC_OVL long
itimeout(val)
long val;
{
    if (val >= TIMEOUT) val = TIMEOUT;
    else if (val < 1) val = 0;

    return val;
}

/* increment `old' by `incr' and force result to be valid intrinsic timeout */
STATIC_OVL long
itimeout_incr(old, incr)
long old;
int incr;
{
    return itimeout((old & TIMEOUT) + (long)incr);
}

/* set the timeout field of intrinsic `which' */
void
set_itimeout(which, val)
long *which, val;
{
    *which &= ~TIMEOUT;
    *which |= itimeout(val);
}

/* increment the timeout field of intrinsic `which' */
void
incr_itimeout(which, incr)
long *which;
int incr;
{
    set_itimeout(which, itimeout_incr(*which, incr));
}

void
make_confused(xtime,talk)
long xtime;
boolean talk;
{
	long old = HConfusion;

	if (!xtime && old) {
		if (talk && (HeavyConfusion < 2) )
		    You_feel("less %s now.",
			Hallucination ? "trippy" : "confused");
	}
	if ((xtime && !old) || (!xtime && old)) flags.botl = TRUE;

	if (xtime && (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "twisted visor helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "shlem vitoy shlema") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "buekuemlue soyabon dubulg'a") ))) xtime *= 5;

	set_itimeout(&HConfusion, xtime);
	if (xtime && !rn2(1000)) {
		pline(Hallucination ? "Huh? Who? Where? What? Is something going on?" : "You're badly confused!");
		set_itimeout(&HeavyConfusion, xtime);
	}
	if (xtime && (StatusTrapProblem || u.uprops[STATUS_FAILURE].extrinsic || have_statusstone()) ) set_itimeout(&HeavyConfusion, xtime);
}

void
make_stunned(xtime,talk)
long xtime;
boolean talk;
{
	long old = HStun;

	if (!xtime && old) {
		if (talk && (HeavyStunned < 2) )
		    You_feel("%s now.",
			Hallucination ? "less wobbly" : "a bit steadier");
	}
	if (xtime && !old) {
		if (talk) {
			if (u.usteed)
				You("wobble in the saddle.");
			else
			You("%s...", stagger(youmonst.data, "stagger"));
		}
	}
	if ((!xtime && old) || (xtime && !old)) flags.botl = TRUE;

	set_itimeout(&HStun, xtime);
	if (xtime && !rn2(1000)) {
		pline(Hallucination ? "It's all wobbly! The world keeps on turning and spinning around..." : "You're badly staggering!");
		set_itimeout(&HeavyStunned, xtime);
	}
	if (xtime && (StatusTrapProblem || u.uprops[STATUS_FAILURE].extrinsic || have_statusstone()) ) set_itimeout(&HeavyStunned, xtime);
}

void
make_numbed(xtime,talk)
long xtime;
boolean talk;
{
	long old = HNumbed;

	if (!xtime && old) {
		if (talk && (HeavyNumbed < 2) )
		    You_feel("%s.",
			Hallucination ? "numbed yer" : "your numbness fading out");
	}
	if (xtime && !old) {
		if (talk) {
			pline(Hallucination ? "You feel numbed! Can't do!" : "You feel numbed!");
		}
	}
	if ((!xtime && old) || (xtime && !old)) flags.botl = TRUE;

	set_itimeout(&HNumbed, xtime);
	if (xtime && !rn2(1000)) {
		pline(Hallucination ? "You can't move! Okay, you can, but it's very difficult..." : "You feel badly numbed!");
		set_itimeout(&HeavyNumbed, xtime);
	}
	if (xtime && (StatusTrapProblem || u.uprops[STATUS_FAILURE].extrinsic || have_statusstone()) ) set_itimeout(&HeavyNumbed, xtime);
}

void
make_feared(xtime,talk)
long xtime;
boolean talk;
{
	long old = HFeared;

	if (!xtime && old) {
		if (talk && (HeavyFeared < 2) )
		    pline("%s",
			Hallucination ? "You're ready to rumble again!" : "You're no longer afraid.");
	}

	if (xtime && old) {
		if (talk)
		    You_feel("%s",
			Hallucination ? "that you just soiled yourself. Crap, where's a toilet if you need one?" : "even more afraid than before!");
	}
	if (xtime && !old) {
		if (talk) {
			pline(Hallucination ? "Suddenly you fear the dungeon around you is going to collapse. Quick! Where is the nearest exit?" : (Role_if(PM_PIRATE) || Role_if(PM_KORSAIR) || (uwep && uwep->oartifact == ART_ARRRRRR_MATEY) ) ? "Ye're lily livered." : "You are stricken with fear!");
		}
	}
	if ((!xtime && old) || (xtime && !old)) flags.botl = TRUE;

	set_itimeout(&HFeared, xtime);
	if (xtime && !rn2(1000)) {
		pline(Hallucination ? "ARRRRRGH! HELP! THERE'S A CRAZY AXE-SWINGING MURDERER CHASING AFTER YOU! RUN!!!" : "You're trembling heavily!");
		set_itimeout(&HeavyFeared, xtime);
	}
	if (xtime && (StatusTrapProblem || u.uprops[STATUS_FAILURE].extrinsic || have_statusstone()) ) set_itimeout(&HeavyFeared, xtime);
}

void
make_frozen(xtime,talk)
long xtime;
boolean talk;
{
	long old = HFrozen;

	if (!xtime && old) {
		if (talk && (HeavyFrozen < 2) )
		    pline(Hallucination ? "Aww... the tasty-looking ice disappeared." : "You are defrosted.");
	}
	if (xtime && !old) {
		if (talk) {
			pline(Hallucination ? "You feel cooling! ??D ??D Y???X!" : "You are frozen solid!");
		}
	}
	if ((!xtime && old) || (xtime && !old)) flags.botl = TRUE;

	if (xtime && Burned) make_burned(0L, TRUE);

	set_itimeout(&HFrozen, xtime);
	if (xtime && !rn2(1000)) {
		pline(Hallucination ? "So many ice-cream cones, and they're all supposed to belong to you... let's eat!" : "The ice is really freezing you rigid!");
		set_itimeout(&HeavyFrozen, xtime);
	}
	if (xtime && (StatusTrapProblem || u.uprops[STATUS_FAILURE].extrinsic || have_statusstone()) ) set_itimeout(&HeavyFrozen, xtime);
}

/* Burn and freezing cancel each other out. --Amy */

void
make_burned(xtime,talk)
long xtime;
boolean talk;
{
	long old = HBurned;

	if (!xtime && old) {
		if (talk && (HeavyBurned < 2) )
		    pline(Hallucination ? "Oh no, someone put out the fire!" : "Your burns disappear.");
	}
	if (xtime && !old) {
		if (talk) {
			pline(Hallucination ? "You're on FIRE! Oh yeah, baby!" : "You were burned!");
		}
	}
	if ((!xtime && old) || (xtime && !old)) flags.botl = TRUE;

	if (xtime && Frozen) make_frozen(0L, TRUE); 
	if (xtime) burn_away_slime();

	set_itimeout(&HBurned, xtime);
	if (xtime && !rn2(1000)) {
		pline(Hallucination ? "Uhh... the fire's getting a little bit too hot, even for your tastes!" : "You're badly burned!");
		set_itimeout(&HeavyBurned, xtime);
	}
	if (xtime && (StatusTrapProblem || (uarmf && uarmf->oartifact == ART_VERA_S_FREEZER) || u.uprops[STATUS_FAILURE].extrinsic || have_statusstone()) ) set_itimeout(&HeavyBurned, xtime);
}

void
make_dimmed(xtime,talk)
long xtime;
boolean talk;
{
	long old = HDimmed;

	if (!xtime && old) {
		if (talk && (HeavyDimmed < 2) )
		    pline(Hallucination ? "Whew, your marriage might be saved after all." : "You are no longer dimmed.");
	}
	if (xtime && !old) {
		if (talk) {
			pline(Hallucination ? "You feel worried about your marriage!" : "You were dimmed!");
		}
	}
	if ((!xtime && old) || (xtime && !old)) flags.botl = TRUE;

	set_itimeout(&HDimmed, xtime);
	if (xtime && !rn2(1000)) {
		pline(Hallucination ? "Life has no more meaning. Your wife has run away, your children are dead and people are setting fire to your home right now." : "You're badly dimmed!");
		set_itimeout(&HeavyDimmed, xtime);
	}
	if (xtime && (StatusTrapProblem || u.uprops[STATUS_FAILURE].extrinsic || have_statusstone()) ) set_itimeout(&HeavyDimmed, xtime);
}

void
make_sick(xtime, cause, talk, type)
long xtime;
const char *cause;	/* sickness cause */
boolean talk;
int type;
{
	long old = Sick;

	if (xtime > 0L) {
	    if (Sick_resistance) return;
	    if (!old) {
		/* newly sick */
		You_feel(Role_if(PM_PIRATE) ? "poxy." : Role_if(PM_KORSAIR) ? "poxy." : (uwep && uwep->oartifact == ART_ARRRRRR_MATEY) ? "poxy." : "deathly sick.");
		if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    } else {
		/* already sick */
		if (talk) You_feel("%s worse.",
			      rn2(2) ? "much" : "even");
		if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    }
	    set_itimeout(&Sick, xtime);
	    if (Sickopathy) pline("You have %d turns to live.", Sick);
	    u.usick_type |= type;
	    flags.botl = TRUE;
		stop_occupation();
	} else if (old && (type & u.usick_type)) {
	    /* was sick, now not */
	    u.usick_type &= ~type;
	    if (u.usick_type) { /* only partly cured */
		if (talk) You_feel("somewhat better.");
		set_itimeout(&Sick, Sick * 2); /* approximation */
	    } else {
		if (talk) pline(Hallucination ? "That cured your overdose!" : "What a relief!");
		Sick = 0L;		/* set_itimeout(&Sick, 0L) */
	    }
	    flags.botl = TRUE;
	}

	if (Sick) {
	    exercise(A_CON, FALSE);
	    if (cause) {
		(void) strncpy(u.usick_cause, cause, sizeof(u.usick_cause));
		u.usick_cause[sizeof(u.usick_cause)-1] = 0;
		}
	    else
		u.usick_cause[0] = 0;
	} else
	    u.usick_cause[0] = 0;
}

void
make_vomiting(xtime, talk)
long xtime;
boolean talk;
{
	long old = Vomiting;

	if(!xtime && old)
	    if(talk) You_feel("much less nauseated now.");

	set_itimeout(&Vomiting, xtime);
}

static const char vismsg[] = "vision seems to %s for a moment but is %s now.";
static const char eyemsg[] = "%s momentarily %s.";

void
make_blinded(xtime, talk)
long xtime;
boolean talk;
{
	long old = Blinded;
	boolean u_could_see, can_see_now;
	int eyecnt;
	char buf[BUFSZ];

	/* we need to probe ahead in case the Eyes of the Overworld
	   are or will be overriding blindness */
	u_could_see = !Blind;
	Blinded = xtime ? 1L : 0L;
	can_see_now = !Blind;
	Blinded = old;		/* restore */

	if (u.usleep) talk = FALSE;

	if (can_see_now && !u_could_see) {	/* regaining sight */
	    if (talk) {
		if (Hallucination)
		    pline("Far out!  Everything is all cosmic again!");
		else
		    You("can see again.");
	    }
	} else if (old && !xtime) {
	    /* clearing temporary blindness without toggling blindness */
	    if (talk) {
		if (!haseyes(youmonst.data)) {
		    strange_feeling((struct obj *)0, (char *)0);
		} else if (Blindfolded) {
		    strcpy(buf, body_part(EYE));
		    eyecnt = eyecount(youmonst.data);
		    Your(eyemsg, (eyecnt == 1) ? buf : makeplural(buf),
			 (eyecnt == 1) ? "itches" : "itch");
		} else {	/* Eyes of the Overworld */
		    Your(vismsg, "brighten",
			 Hallucination ? "sadder" : "normal");
		}
	    }
	}

	if (u_could_see && !can_see_now) {	/* losing sight */
	    if (talk) {
		if (Hallucination)
		    pline("Oh, bummer!  Everything is dark!  Help!");
		else
		    pline("A cloud of darkness falls upon you.");
	    }
	    /* Before the hero goes blind, set the ball&chain variables. */
	    if (Punished) set_bc(0);
	} else if (!old && xtime) {
	    /* setting temporary blindness without toggling blindness */
	    if (talk) {
		if (!haseyes(youmonst.data)) {
		    strange_feeling((struct obj *)0, (char *)0);
		} else if (Blindfolded) {
		    strcpy(buf, body_part(EYE));
		    eyecnt = eyecount(youmonst.data);
		    Your(eyemsg, (eyecnt == 1) ? buf : makeplural(buf),
			 (eyecnt == 1) ? "twitches" : "twitch");
		} else {	/* Eyes of the Overworld */
		    Your(vismsg, "dim",
			 Hallucination ? "happier" : "normal");
		}
	    }
	}

	set_itimeout(&Blinded, xtime);
	if (xtime && !rn2(1000)) {
		pline(Hallucination ? "Aww, even the images in your mind have disappeared!" : "The darkness seems definite and impenetrable!");
		set_itimeout(&HeavyBlind, xtime);
	}
	if (xtime && (StatusTrapProblem || u.uprops[STATUS_FAILURE].extrinsic || have_statusstone()) ) set_itimeout(&HeavyBlind, xtime);

	if (u_could_see ^ can_see_now) {  /* one or the other but not both */
	    flags.botl = 1;
	    vision_full_recalc = 1;	/* blindness just got toggled */
	    if (Blind_telepat || Infravision) see_monsters();
	}
}

boolean
make_hallucinated(xtime, talk, mask)
long xtime;	/* nonzero if this is an attempt to turn on hallucination */
boolean talk;
long mask;	/* nonzero if resistance status should change by mask */
{
	long old = HHallucination;
	boolean changed = 0;
	const char *message, *verb;

	message = (!xtime) ? "Everything %s SO boring now." :
			     "Oh wow!  Everything %s so cosmic!";
	verb = (!Blind) ? "looks" : "feels";

	if (xtime && (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "twisted visor helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "shlem vitoy shlema") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "buekuemlue soyabon dubulg'a") ))) xtime *= 5;

	if (mask) {
	    if (HHallucination) changed = TRUE;

	    if (!xtime) EHalluc_resistance |= mask;
	    else EHalluc_resistance &= ~mask;
	} else {
	    if (!EHalluc_resistance && (!!HHallucination != !!xtime))
		changed = TRUE;
	    set_itimeout(&HHallucination, xtime);
		if (xtime && !rn2(1000)) {
			pline("Now that was some fucked up shit you did there, huh? But who cares? Let's enjoy the colors!");
			set_itimeout(&HeavyHallu, xtime);
		}
		if (xtime && (StatusTrapProblem || u.uprops[STATUS_FAILURE].extrinsic || have_statusstone()) ) set_itimeout(&HeavyHallu, xtime);

	    /* clearing temporary hallucination without toggling vision */
	    if (!changed && !HHallucination && old && talk) {
		if (!haseyes(youmonst.data)) {
		    strange_feeling((struct obj *)0, (char *)0);
		} else if (Blind) {
		    char buf[BUFSZ];
		    int eyecnt = eyecount(youmonst.data);

		    strcpy(buf, body_part(EYE));
		    Your(eyemsg, (eyecnt == 1) ? buf : makeplural(buf),
			 (eyecnt == 1) ? "itches" : "itch");
		} else {	/* Grayswandir */
		    Your(vismsg, "flatten", "normal");
		}
	    }
	}

	if (changed) {
	    if (u.uswallow) {
		swallowed(0);	/* redraw swallow display */
	    } else {
		/* The see_* routines should be called *before* the pline. */
		see_monsters();
		see_objects();
		see_traps();
	    }

	    /* for perm_inv and anything similar
	    (eg. Qt windowport's equipped items display) */
	    update_inventory();

	    flags.botl = 1;
	    if (talk) pline(message, verb);

	    if (!(InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone())) (void) doredraw();

	}
	return changed;
}

/* For game balance we don't always want a bottle made. */
static void make_bottle(boolean certain)
{
	struct obj *otmp;

	if (certain || !rn2(8)) {
		otmp = mksobj(BOTTLE,TRUE,TRUE);
/* We do not transfer curses/blessings, assuming that these a property
** of the actual potion, not the bottle. */
		if (otmp) {
			otmp->cursed = otmp->blessed = FALSE;
			You("are left with the empty bottle.");
			hold_another_object(otmp,"Oops! It drops to the floor!",(const char *) 0, (const char *) 0);
		}
	}
}


STATIC_OVL void
ghost_from_bottle()
{
	struct monst *mtmp = makemon(&mons[PM_GHOST], u.ux, u.uy, NO_MM_FLAGS);

	if (!mtmp) {
		pline("This bottle turns out to be empty.");
		make_bottle(TRUE);
		return;
	}
	if (Blind) {
		pline("As you open the bottle, %s emerges.", something);
		return;
	}
	pline("As you open the bottle, an enormous %s emerges!",
		Hallucination ? rndmonnam() : (const char *)"ghost");
	if(flags.verbose)
	    You("are frightened to death, and unable to move.");
	nomul(-3, "being frightened to death", TRUE);
	make_feared(HFeared + rnd(30 + (monster_difficulty() * 3) ),TRUE);
	nomovemsg = "You regain your composure.";
	make_bottle(FALSE);
}

/* "Quaffing is like drinking, except you spill more."  -- Terry Pratchett
 */

void
badeffect()

{

	struct obj *otmp;
	int nastytrapdur;
	int blackngdur;

	if (uarm && objects[(uarm)->otyp].oc_material == INKA && !rn2(10)) {
		pline("Thanks to your inka armor, you averted misfortune!");
		return;
	}
	if (uarmf && objects[(uarmf)->otyp].oc_material == INKA && !rn2(10)) {
		pline("Thanks to your inka footwear, you averted misfortune!");
		return;
	}
	if (uarmg && objects[(uarmg)->otyp].oc_material == INKA && !rn2(10)) {
		pline("Thanks to your inka gloves, you averted misfortune!");
		return;
	}
	if (uarmh && objects[(uarmh)->otyp].oc_material == INKA && !rn2(10)) {
		pline("Thanks to your inka helmet, you averted misfortune!");
		return;
	}
	if (uarms && objects[(uarms)->otyp].oc_material == INKA && !rn2(10)) {
		pline("Thanks to your inka shield, you averted misfortune!");
		return;
	}
	if (uarmc && objects[(uarmc)->otyp].oc_material == INKA && !rn2(10)) {
		pline("Thanks to your inka cloak, you averted misfortune!");
		return;
	}
	if (uarmu && objects[(uarmu)->otyp].oc_material == INKA && !rn2(10)) {
		pline("Thanks to your inka shirt, you averted misfortune!");
		return;
	}
	if (uwep && uwep->oartifact == ART_LUCK_VERSUS_BAD && !rn2(2)) {
		pline("Thanks to your inka sling, you averted misfortune!");
		return;
	}

	if (!(PlayerCannotUseSkills)) {
		int shiichochance = 0;
		switch (P_SKILL(P_SHII_CHO)) {

			case P_BASIC:	shiichochance =  1; break;
			case P_SKILLED:	shiichochance =  2; break;
			case P_EXPERT:	shiichochance =  3; break;
			case P_MASTER:	shiichochance =  4; break;
			case P_GRAND_MASTER:	shiichochance =  5; break;
			case P_SUPREME_MASTER:	shiichochance =  6; break;
			default: shiichochance += 0; break;
		}

		if (shiichochance > rn2(100)) {
			/* avoid bad effect --Amy */
			return;
		}
	}

	switch (rnd(391)) {

		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		if (Hallucination) You_feel("rather trippy.");
		else You_feel("rather %s.", body_part(LIGHT_HEADED));
		make_confused(HConfusion + rnz(50),FALSE);
		break;

		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
		case 24:
		case 25:
		case 26:
		case 27:
		case 28:
		case 29:
		case 30:
		if (Hallucination) You_feel("uncontrollable.");
		else You_feel("stunned.");
		make_stunned(HStun + rnz(50),FALSE);
		break;

		case 31:
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
		case 38:
		case 39:
		case 40:
		case 41:
		case 42:
		case 43:
		case 44:
		case 45:
		if (Hallucination) You_feel("even weirder!");
		else You_feel("weirded out!");
		make_hallucinated(HHallucination + rnz(100),FALSE,0L);
		break;

		case 46:
		case 47:
		case 48:
		case 49:
		case 50:
		case 51:
		case 52:
		case 53:
		case 54:
		case 55:
		case 56:
		case 57:
		case 58:
		case 59:
		case 60:
		if (!Blind) pline("Everything suddenly goes dark.");
		make_blinded(Blinded+rnz(100),FALSE);
		if (!Blind) Your(vision_clears);
		break;

		case 61:
		case 62:
		case 63:
		case 64:
		case 65:
		case 66:
		case 67:
		case 68:
		case 69:
		case 70:
		case 71:
		case 72:
		case 73:
		case 74:
		case 75:
		if (Hallucination) You_feel("like your body is trying to fall asleep!");
		else You_feel("numb.");
		make_numbed(HNumbed + rnz(150),FALSE);
		break;

		case 76:
		case 77:
		case 78:
		case 79:
		case 80:
		if (Hallucination) You_feel("a giant ice cream cone enclosing you!");
		else pline("You're getting the chills.");
		make_frozen(HFrozen + rnz(150),FALSE);
		break;

		case 81:
		case 82:
		case 83:
		case 84:
		case 85:
		if (Hallucination) You_feel("like you have dementia tremor!"); /* not a real name --Amy */
		else pline("Your hands start trembling violently!");
		incr_itimeout(&Glib, rnz(50) );
		break;

		case 86:
		case 87:
		case 88:
		case 89:
		case 90:
		if (Hallucination) You_feel("totally down! Seems you tried some illegal shit!");
		else You_feel("like you're going to throw up.");
	      make_vomiting(Vomiting+20, TRUE);
		if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
		break;

		case 91:
		case 92:
		case 93:
		case 94:
		case 95:
		case 96:
		case 97:
		case 98:
		case 99:
		case 100:
		pline("The world spins and goes dark.");
		flags.soundok = 0;
		nomul(-rnd(10), "helplessly knocked out", TRUE);
		nomovemsg = "You are conscious again.";
		afternmv = Hear_again;
		break;

		case 101:
	    make_sick(rn1(25,25), "spreading food poisoning", TRUE, SICK_VOMITABLE);
		break;

		case 102:
		if (!Slimed && !flaming(youmonst.data) && !Unchanging && !slime_on_touch(youmonst.data) ) {
			You("don't feel very well.");
		    Slimed = 100L;
		    flags.botl = 1;
			stop_occupation();
		    killer_format = KILLED_BY_AN;
		    delayed_killer = "summoned slime";
		}
		break;

		case 103:

		if ((otmp = mksobj(LOADSTONE, TRUE, FALSE)) != (struct obj *)0) {
		You_feel("burdened");
		if (pickup_object(otmp, 1, FALSE) <= 0) {
		obj_extract_self(otmp);
		place_object(otmp, u.ux, u.uy);
		newsym(u.ux, u.uy); }
		}

		break;

		case 104:
		case 105:
		case 106:
		pline("You float up!");
		HLevitation &= ~I_SPECIAL;
		incr_itimeout(&HLevitation, rnz(50));

		break;

		case 107:
		case 108:
		case 109:
		case 110:
		case 111:
		create_critters(rnz(10), (struct permonst *)0);
		break;

		case 112:
		case 113:
		case 114:
		case 115:
		case 116:
		{coord mm;   
		mm.x = u.ux;   
		mm.y = u.uy;   
		pline("Undead creatures are called forth from the grave!");   
		mkundead(&mm, FALSE, 0);   
		}
		break;

		case 117:
		case 118:
		case 119:
		case 120:
		case 121:
		case 122:
		case 123:
		case 124:
		case 125:
		case 126:
		case 127:
		case 128:
		case 129:
		case 130:
		case 131:
		case 132:
		You_feel("that monsters are aware of your presence.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Dazhe sovetskaya Pyat' Lo obostryayetsya v vashem nizkom igrovom masterstve." : "Woaaaaaah!");
		aggravate();

		break;

		case 133:
		case 134:
		case 135:
		case 136:
		case 137:
		You_feel("as if you need some help.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
		rndcurse();

		break;

		case 138:
			{register struct obj *obj;

			pline("Urgh! You feel a malevolent presence!");
			for(obj = invent; obj ; obj = obj->nobj)
				if (!rn2(5) && !stack_too_big(obj))	curse(obj);
			}
		break;

		case 139:
			if (!Antimagic || !rn2(20)) {
				struct obj *otmp2;

				otmp2 = some_armor(&youmonst);

				if (otmp2 && otmp2->blessed && rn2(5)) pline("Your body shakes violently!");
				else if (otmp2 && (otmp2->spe > 1) && (rn2(otmp2->spe)) ) pline("Your body shakes violently!");
				else if (otmp2 && otmp2->oartifact && rn2(20)) pline("Your body shakes violently!");
				else if (otmp2 && otmp2->greased) {
					pline("Your body shakes violently!");
					 if (!rn2(2)) {
						pline_The("grease wears off.");
						otmp2->greased -= 1;
						update_inventory();
					 }
				}

				else if (!otmp2) pline("Your skin itches.");
			      else if(!destroy_arm(otmp2)) pline("Your skin itches.");

			}

		break;

		case 140:
		u.ugangr++;
		if (!rn2(5)) u.ugangr++;
		if (!rn2(25)) u.ugangr++;
		prayer_done();

		break;

		case 141:
			if (!Antimagic || !rn2(20)) {
			    You("suddenly feel weaker!");
			    losestr(rnz(4));
			    if (u.uhp < 1) {
				u.youaredead = 1;
				u.uhp = 0;
				killer_format = KILLED_BY;
				killer = "a fatally low strength";
				done(DIED);
				u.youaredead = 0;

				}
			}

		break;

		case 142:
		case 143:
		water_damage(invent, FALSE, FALSE);
		if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);
		if (Burned) make_burned(0L, TRUE);

		break;

		case 144:

		withering_damage(invent, FALSE, FALSE);

		break;

		case 145:
		if (!Stoned && !Stone_resistance && !(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM)) ) {
			if (Hallucination && rn2(10)) pline("You are already stoned.");
			else {
				You("start turning to stone!");
				Stoned = 7;
				stop_occupation();
				delayed_killer = "bad petrification effect";
			}
		}

		break;

		case 146:
		{
		int aligntype;
		aligntype = rn2((int)A_LAWFUL+2) - 1;
		 pline("A servant of %s appears!",aligns[1 - aligntype].noun);
		summon_minion(aligntype, TRUE);
		}
		break;

		case 147:
		case 148:
		case 149:
		case 150:
		case 151:
		case 152:
		case 153:
		case 154:
		case 155:
		case 156:
	      attrcurse();

		break;

		case 157:
		case 158:
		case 159:
		case 160:
		case 161:
		case 162:
		case 163:
		case 164:
		case 165:
		case 166:
		case 167:
		case 168:
		case 169:
		case 170:
		case 171:
		pline("It gets dark!");
	    do_clear_areaX(u.ux,u.uy,		/* darkness around player's position */
		15, set_litI, (void *)((char *)0));

		break;

		case 172:
		case 173:
		case 174:
		case 175:
		case 176:
		case 177:
		case 178:
		case 179:
		case 180:
		case 181:
		{	int rtrap;
		    int i, j, bd;
			bd = 1;
			if (!rn2(5)) bd += rnz(1);

		      for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
				if (!isok(u.ux + i, u.uy + j)) continue;
				if (levl[u.ux + i][u.uy + j].typ <= DBWALL) continue;
				if (t_at(u.ux + i, u.uy + j)) continue;

			      rtrap = randomtrap();
				if (!rn2(20)) makerandomtrap();

				(void) maketrap(u.ux + i, u.uy + j, rtrap, 100);
			}
			makerandomtrap();
			if (!rn2(3)) makerandomtrap();
		}
		break;

		case 182:
		case 183:
		case 184:
	      You_feel("yanked in a new direction!");
		(void) safe_teleds(FALSE);

		break;

		case 185:
		case 186:
		case 187:
		case 188:
		case 189:
		case 190:
		case 191:
		case 192:
		case 193:
		case 194:
		case 195:
		case 196:
		case 197:
		case 198:
		case 199:
		pline("You lose  Mana");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vasha magicheskaya energiya udalyayetsya v nastoyashcheye vremya. Skoro on budet raven nulyu, a zatem vy dolzhny igrat' bez zaklinaniy, potomu chto vy sosat', GA GA GA!" : "Due-l-ue-l-ue-l!");
		drain_en(rnz(monster_difficulty() + 1) );
		break;

		case 200:
		case 201:
		case 202:
	      You_feel("out of luck!");
			change_luck(-1);
			if (!rn2(10)) change_luck(-5);
			adjalign(-10);
			if (!rn2(10)) adjalign(-50);

		break;

		case 203:
		case 204:
		case 205:
		case 206:
		case 207:
		punishx();
		break;

		case 208:
		case 209:
		case 210:
		case 211:
		case 212:
		case 213:
		case 214:
		case 215:
		case 216:
		case 217:
		case 218:
		case 219:
		case 220:
		case 221:
		case 222:
		{int copcnt;
		copcnt = rnd(monster_difficulty() ) + 1;
		if (rn2(5)) copcnt = (copcnt / (rnd(4) + 1)) + 1;
		if (rn2(5)) copcnt /= 2;
		if (!rn2(5)) copcnt /= 2; /* don't make too many */
		if (!rn2(10)) copcnt /= 3;
		if (copcnt < 1) copcnt = 1;
	
		if (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "anti-government helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "antipravitel'stvennaya shlem") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "aksil-hukumat dubulg'a") ) ) {
			copcnt = (copcnt / 2) + 1;
		}

		if (RngeAntiGovernment) {
			copcnt = (copcnt / 2) + 1;
		}

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

	      while(--copcnt >= 0) {
			(void) makemon(mkclass(S_KOP,0), u.ux, u.uy, MM_ANGRY);

			if (!rn2(500)) {

				int koptryct = 0;
				int kox, koy;

				for (koptryct = 0; koptryct < 2000; koptryct++) {
					kox = rn1(COLNO-3,2);
					koy = rn2(ROWNO);

					if (kox && koy && isok(kox, koy) && (levl[kox][koy].typ > DBWALL) && !(t_at(kox, koy)) ) {
						(void) maketrap(kox, koy, KOP_CUBE, 0);
						break;
						}
				}

			}

		} /* while */

		u.aggravation = 0;

		}
		break;

		case 223:
			losexp((char *)0, TRUE, FALSE); 

		break;

		case 224:
		    attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse();

		break;

		case 225:
		case 226:
		case 227:
		case 228:
		case 229:
		case 230:
		case 231:
		case 232:

			pline(Hallucination ? "You feel sinful... but do you really care?" : "You have a feeling of separation.");
			u.ublesscnt += rnz(ishaxor ? 150 : 300);

		break;

		case 233:
		case 234:
		case 235:
		case 236:
		case 237:
		case 238:
		case 239:
		case 240:
		case 241:
		case 242:
		if (Hallucination) You_feel("totally hot! Oh yeah, baby!");
		else pline("You're burning!");
		make_burned(HBurned + rnz(150),FALSE);
		break;

		case 243:
		case 244:
		case 245:
		case 246:
		case 247:
		case 248:
		case 249:
		case 250:
		case 251:
		case 252:
		case 253:
		case 254:
		case 255:
		case 256:
		case 257:
		if (Hallucination) You("panic! The alarm bells are ringing and you don't know how to get out!");
		else You_feel("afraid.");
		make_feared(HFeared + rnz(150),FALSE);
		break;

		case 258:
		case 259:
		case 260:
		case 261:
		case 262:
		case 263:
		case 264:
		case 265:
		case 266:
		case 267:
		case 268:
		case 269:
		case 270:
		case 271:
		case 272:
		case 273:
		case 274:
		case 275:
		case 276:
		case 277:
			deacrandomintrinsic(rnz(200));
			break;

		case 278:

		nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
		if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
		blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
		if (!blackngdur ) blackngdur = 500; /* fail safe */

		randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), blackngdur - (monster_difficulty() * 3));

		break;

		case 279:
		case 280:
		case 281:
		case 282:
		case 283:
		case 284:
		case 285:
		case 286:
		case 287:
		case 288:
		    adjalign(-rnd(20));
		    if (flags.soundok) You_hear("a slight rumbling...");
		    if (PlayerHearsSoundEffects) pline(issoviet ? "Pomolis'! Vash bog, bezuslovno, pomozhet vam!" : "Wumm.");

		break;

		case 289:

		    adjalign(-rnd(50));
			u.ualign.sins++;
			u.alignlim--;
		    if (flags.soundok) You_hear("a thunderous rumbling!");
		    if (PlayerHearsSoundEffects) pline(issoviet ? "Seychas ideal'noye vremya dlya molitvy! Vash bog super schastliv s toboy i, bezuslovno, vylechit' vse bolezni i predostavit' vam moshchnyy punkt!" : "Wummmmm! Wummmmm! Grummel!");

		break;

		case 290:

			pline("You are engulfed in flames!");
			(void) burnarmor(&youmonst);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5)) destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5)) destroy_item(SPBOOK_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5)) destroy_item(POTION_CLASS, AD_FIRE);

		break;

		case 291:
		case 292:
		{	
		      int bd = rnd(10);
			if (!rn2(5)) bd += rnz(10);

			while (bd-- >= 0) makerandomtrap();

		}
		break;

		case 293:
		case 294:
		case 295:
		case 296:
		case 297:
		case 298:
		case 299:
		case 300:
		case 301:
		case 302:
		case 303:
		case 304:
		case 305:
		case 306:
		case 307:
			pline("You fall asleep!");
			fall_asleep(-rnd(15 + rnd((monster_difficulty() / 5) + 1) ), TRUE);

		break;

		case 308:
		case 309:

			pline("You are hit by a needle!");
		    poisoned("needle", rn2(6), "poison needle", 30);

		break;

		case 310:
		case 311:
		case 312:
		case 313:
		case 314:
			u_slow_down();

		break;

		case 315:
		case 316:
		case 317:
		case 318:
		case 319:
			pline("A laser beam hits you out of nowhere!");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vy poteryali linii!" : "DWUEUEUET!");
			losehp(monster_difficulty(),"laser beam out of nowhere",KILLED_BY_AN);

		break;

		case 320:
			pline("Suddenly your %s hurts!", body_part(rn2(19) ) );
			losehp(1 + u.chokhmahdamage + rnd(u.ualign.sins + 1),"escalating damage effect",KILLED_BY_AN);
			u.chokhmahdamage++;

		break;

		case 321:
			if (u.ugold) {
				u.ugold -= (u.ugold / 5);
				pline("Your purse feels lighter...");
			}

		break;

		case 322:
			if (!Unchanging && !Antimagic) {
				You("undergo a freakish metamorphosis!");
			      polyself(FALSE);
			}

		break;

		case 323:
			You("need reboot.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Eto poshel na khuy vverkh. No chto zhe vy ozhidali? Igra, v kotoruyu vy mozhete legko vyigrat'? Durak!" : "DUEUEDUET!");
			if (!Race_if(PM_UNGENOMOLD)) newman();
			else polyself(FALSE);
		break;

		case 324:
			if (Hallucination)
				pline("What a groovy feeling!");
			else
				You(Blind ? "%s and get dizzy..." :
					 "%s and your vision blurs...",
					    stagger(youmonst.data, "stagger"));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Imet' delo s effektami statusa ili sdat'sya!" : "Wrueue-ue-e-ue-e-ue-e...");
			make_stunned(HStun + rn1(7,16) + monster_difficulty(), FALSE);
			(void) make_hallucinated(HHallucination + rn1(7,16) + monster_difficulty(),TRUE,0L);

		break;

		case 325:
			if(!Blind)
				Your("vision bugged.");
			(void) make_hallucinated(HHallucination + rn1(10, 25) + rn1(10, 25) + monster_difficulty() + monster_difficulty(),TRUE,0L);

		break;

		case 326:
		case 327:
			if(!Blind)
				Your("vision turns to screen saver.");
			(void) make_hallucinated(HHallucination + rn1(10, 25) + monster_difficulty(),TRUE,0L);
		break;

		case 328:
			{
			    struct obj *objD = some_armor(&youmonst);

			    if (objD && drain_item(objD)) {
				Your("%s less effective.", aobjnam(objD, "seem"));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
			    }
			}

		break;

		case 329:

			if (!Disint_resistance || !rn2(100) ) {
				You_feel("like you're falling apart!");
	
				if (uarms) {
				    /* destroy shield; other possessions are safe */
				    if (!(EDisint_resistance & W_ARMS)) (void) destroy_arm(uarms);
				    break;
				} else if (uarmc) {
				    /* destroy cloak; other possessions are safe */
				    if (!(EDisint_resistance & W_ARMC)) (void) destroy_arm(uarmc);
				    break;
				} else if (uarm) {
				    /* destroy suit */
				    if (!(EDisint_resistance & W_ARM)) (void) destroy_arm(uarm);
				    break;
				} else if (uarmu) {
				    /* destroy shirt */
				    if (!(EDisint_resistance & W_ARMU)) (void) destroy_arm(uarmu);
				    break;
				} else {
					u.youaredead = 1;
					done(DIED);
					u.youaredead = 0;
				}
	
			}

		break;

		case 330:

			pline("Suddenly a lightning flash hits you!");
		    destroy_item(RING_CLASS, AD_ELEC);
		    destroy_item(WAND_CLASS, AD_ELEC);
		    destroy_item(AMULET_CLASS, AD_ELEC);

		break;

		case 331:
			pline("Suddenly it's freezing cold!");
			destroy_item(POTION_CLASS, AD_COLD);

		break;

		case 332:

			MCReduction += rnz(100 * (monster_difficulty() + 1));
			pline("The magic cancellation granted by your armor seems weaker now...");

		break;

		case 333:

		      u_slow_down();
			u.uprops[DEAC_FAST].intrinsic += (( rnd(10) + rnd(monster_difficulty() + 1) ) * 10);
			pline(u.inertia ? "You feel even slower." : "You slow down to a crawl.");
			u.inertia += (rnd(10) + rnd(monster_difficulty() + 1));

		break;

		case 334:

			if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER) && uimplant && uimplant->oartifact == ART_TIMEAGE_OF_REALMS) break;
		{
		int dmg;
		dmg = (rnd(10) + rnd( (monster_difficulty() * 2) + 1));
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
						if(ABASE(A_STR) < ATTRMIN(A_STR)) {dmg *= 3; ABASE(A_STR) = ATTRMIN(A_STR);}
						break;
					case A_DEX:
						pline("You're not as agile as you used to be...");
						ABASE(A_DEX) -= 5;
						if(ABASE(A_DEX) < ATTRMIN(A_DEX)) {dmg *= 3; ABASE(A_DEX) = ATTRMIN(A_DEX);}
						break;
					case A_CON:
						pline("You're not as hardy as you used to be...");
						ABASE(A_CON) -= 5;
						if(ABASE(A_CON) < ATTRMIN(A_CON)) {dmg *= 3; ABASE(A_CON) = ATTRMIN(A_CON);}
						break;
					case A_WIS:
						pline("You're not as wise as you used to be...");
						ABASE(A_WIS) -= 5;
						if(ABASE(A_WIS) < ATTRMIN(A_WIS)) {dmg *= 3; ABASE(A_WIS) = ATTRMIN(A_WIS);}
						break;
					case A_INT:
						pline("You're not as bright as you used to be...");
						ABASE(A_INT) -= 5;
						if(ABASE(A_INT) < ATTRMIN(A_INT)) {dmg *= 3; ABASE(A_INT) = ATTRMIN(A_INT);}
						break;
					case A_CHA:
						pline("You're not as beautiful as you used to be...");
						ABASE(A_CHA) -= 5;
						if(ABASE(A_CHA) < ATTRMIN(A_CHA)) {dmg *= 3; ABASE(A_CHA) = ATTRMIN(A_CHA);}
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
				if(ABASE(A_STR) < ATTRMIN(A_STR)) {dmg *= 2; ABASE(A_STR) = ATTRMIN(A_STR);}
				if(ABASE(A_DEX) < ATTRMIN(A_DEX)) {dmg *= 2; ABASE(A_DEX) = ATTRMIN(A_DEX);}
				if(ABASE(A_CON) < ATTRMIN(A_CON)) {dmg *= 2; ABASE(A_CON) = ATTRMIN(A_CON);}
				if(ABASE(A_WIS) < ATTRMIN(A_WIS)) {dmg *= 2; ABASE(A_WIS) = ATTRMIN(A_WIS);}
				if(ABASE(A_INT) < ATTRMIN(A_INT)) {dmg *= 2; ABASE(A_INT) = ATTRMIN(A_INT);}
				if(ABASE(A_CHA) < ATTRMIN(A_CHA)) {dmg *= 2; ABASE(A_CHA) = ATTRMIN(A_CHA);}
				break;
		}
		if (dmg) losehp(dmg, "being timed", KILLED_BY);
		}

		break;

		case 335:

			(void) cancel_monst(&youmonst, (struct obj *)0, FALSE, TRUE, FALSE);

		break;

		case 336:

			forget(3);
			{
			if (!strncmpi(plname, "Maud", 4))
				pline("Suddenly, your mind turns inward on itself!");
			else 
				pline("Suddenly, you are overwhelmed by a feeling that urges you to think of Maud.");
			}
			exercise(A_WIS, FALSE);

		break;

		case 337:
			if (IncreasedGravity) pline("Your load feels even heavier!");
			else pline("Your load feels heavier!");

			IncreasedGravity += rnz(5 * (monster_difficulty() + 1));

		break;

		case 338:
			if (Upolyd) u.mhmax--; /* lose one hit point */
			else u.uhpmax--; /* lose one hit point */
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			if (u.mh > u.mhmax) u.mh = u.mhmax;
			pline("Your health has been drained!");

		break;

		case 339:
			u.uenmax--; /* lose one mana point */
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Your mana has been drained!");

		break;

		case 340:
		case 341:
		case 342:
		case 343:
		case 344:
		case 345:
		case 346:
		case 347:
		case 348:
		case 349:

		You_feel("bad!");
			if (!rn2(20)) losehp(d(10,8), "a bad damage effect", KILLED_BY);
			else if (!rn2(5)) losehp(d(6,8), "a bad damage effect", KILLED_BY);
			else losehp(d(4,6), "a bad damage effect", KILLED_BY);
		break;

		case 350:
		{    register struct obj *objX, *objX2;
		    for (objX = invent; objX; objX = objX2) {
		      objX2 = objX->nobj;
			if (!rn2(5)) rust_dmg(objX, xname(objX), 3, TRUE, &youmonst);
		    }
		}

		break;

		case 351:

			HFumbling = FROMOUTSIDE | rnd(5);
			incr_itimeout(&HFumbling, rnd(20));
			u.fumbleduration += rnz(1000);

		break;

		case 352:
			You_feel("a hole in your %s!", body_part(STOMACH) );
			morehungry(rnd(1000));

		break;

		case 353:
		case 354:
		case 355:
		case 356:
		case 357:
		case 358:
		case 359:
		case 360:
		case 361:
		case 362:
		case 363:
		case 364:
		case 365:
		case 366:
		case 367:
		case 368:
		case 369:
		case 370:
		case 371:
		case 372:

			pushplayer();

		break;

		case 373:
			if (!Antimagic || !rn2(20)) {
				struct obj *otmp2;

				otmp2 = uwep;
				if (otmp2 && stack_too_big(otmp2)) pline("Your fingers shake violently!");

				else if (otmp2 && otmp2->blessed && rn2(5)) pline("Your fingers shake violently!");
				else if (otmp2 && (otmp2->spe > 1) && (rn2(otmp2->spe)) ) pline("Your fingers shake violently!");
				else if (otmp2 && otmp2->oartifact && rn2(20)) pline("Your fingers shake violently!");
				else if (otmp2 && otmp2->greased) {
					pline("Your fingers shake violently!");
					 if (!rn2(2)) {
						pline_The("grease wears off.");
						otmp2->greased -= 1;
						update_inventory();
					 }
				}

				else if (!otmp2) pline("Your fingers itch.");
			      else {
					useupall(otmp2);
					pline("Your weapon evaporates!");
				}

			}

		break;

		case 374:

			if (!rn2(1000)) skillcaploss();

		break;

		case 375:
			bad_artifact();
		break;

		case 376:
		case 377:
		case 378:
		case 379:
		case 380:
		if (Hallucination) You_feel("totally bad! Your wife is going to abandon you...");
		else pline("You're dimmed!");
		make_dimmed(HDimmed + rnz(150),FALSE);
		break;

		case 381:
		case 382:
		case 383:
		case 384:
		case 385:
		case 386:
		case 387:
		case 388:
		case 389:
		case 390:
			contaminate(rnd(10 + level_difficulty()), FALSE);
		break;
		case 391:
			contaminate(rnz(100 + (level_difficulty() * 10)), FALSE);
		break;

		default:
		break;
	}

}

void
ragnarok()

{
	register int x,y;

	if (rn2(64)) return;

	u.aggravation = 1;
	DifficultyIncreased += 1;
	HighlevelStatus += 1;
	EntireLevelMode += 1;
	if (!rn2(5)) DifficultyIncreased += rnz(100);
	if (!rn2(5)) HighlevelStatus += rnz(100);
	if (!rn2(5)) EntireLevelMode += rnz(100);

	pline("Let's Ragnarok!");

	for (x = 0; x < COLNO; x++)
	  for (y = 0; y < ROWNO; y++) {

		if (isok(x,y) && (IS_STWALL(levl[x][y].typ) || levl[x][y].typ == ROOM || levl[x][y].typ == LAVAPOOL || levl[x][y].typ == CORR) && levl[x][y].typ != SDOOR && ((levl[x][y].wall_info & W_NONDIGGABLE) == 0) && !(*in_rooms(x,y,SHOPBASE)) && !rn2(5) ) {

			levl[x][y].typ = LAVAPOOL;
			unblock_point(x,y);
			if (!(levl[x][y].wall_info & W_HARDGROWTH)) levl[x][y].wall_info |= W_EASYGROWTH;
			newsym(x, y);

			if (!rn2(3)) switch (rnd(10)) {
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
					(void) makemon(mkclass(S_DRAGON,0), x, y, MM_ADJACENTOK|MM_ANGRY);
					break;
				case 6:
				case 7:
				case 8:
					(void) makemon(mkclass(S_GIANT,0), x, y, MM_ADJACENTOK|MM_ANGRY);
					break;
				case 9:
				case 10:
					(void) makemon(mkclass(S_DEMON,0), x, y, MM_ADJACENTOK|MM_ANGRY);
					break;
			}

		}

	}

	u.aggravation = 0;

}

void
evilragnarok(wflevel)
int wflevel;
{
	register struct monst *mtmp, *mtmp2;

	for (mtmp = fmon; mtmp; mtmp = mtmp2) {
		mtmp2 = mtmp->nmon;
		if ((mtmp->m_lev < wflevel) && mtmp->mtame) {
			if (u.usteed && mtmp == u.usteed) dismount_steed(DISMOUNT_GENERIC);
			mondead(mtmp);
			pline("One of your pets has died.");
		}
	}
	if (wflevel > u.ulevel) {
		u.youaredead = 1;
		pline("The world ends and you are dead. Goodbye.");
		killer_format = NO_KILLER_PREFIX;
		killer = "world fall";
		done(DIED);
		u.youaredead = 0;

	}

}

void
datadeleteattack()

{

	Your("data is deleted!");

	switch (rnd(17)) {


		case 1:
			pline("Suddenly you don't remember anything.");
			forget(35);
			forget_levels(35);
			forget_objects(35);
			break;
		case 2:
			pline("Suddenly you don't remember your spells.");
			while (rn2(10)) {
				losespells();
			}
			break;
		case 3:
			pline("You feel a severe attribute loss.");
			{
			int attributelose = rn2(A_MAX);
			if(ABASE(attributelose) > ATTRMIN(attributelose)) {
				ABASE(attributelose) -= 1;
				AMAX(attributelose) -= 1;

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low stats", KILLED_BY);
			}
			if(ABASE(attributelose) > ATTRMIN(attributelose)) {
				ABASE(attributelose) -= 1;
				AMAX(attributelose) -= 1;

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low stats", KILLED_BY);
			}
			if(ABASE(attributelose) > ATTRMIN(attributelose)) {
				ABASE(attributelose) -= 1;
				AMAX(attributelose) -= 1;

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low stats", KILLED_BY);
			}
			if(ABASE(attributelose) > ATTRMIN(attributelose)) {
				ABASE(attributelose) -= 1;
				AMAX(attributelose) -= 1;

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low stats", KILLED_BY);
			}
			if(ABASE(attributelose) > ATTRMIN(attributelose)) {
				ABASE(attributelose) -= 1;
				AMAX(attributelose) -= 1;

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low stats", KILLED_BY);
			}
			if(ABASE(attributelose) > ATTRMIN(attributelose)) {
				ABASE(attributelose) -= 1;
				AMAX(attributelose) -= 1;

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low stats", KILLED_BY);
			}
			if(ABASE(attributelose) > ATTRMIN(attributelose)) {
				ABASE(attributelose) -= 1;
				AMAX(attributelose) -= 1;

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low stats", KILLED_BY);
			}
			if(ABASE(attributelose) > ATTRMIN(attributelose)) {
				ABASE(attributelose) -= 1;
				AMAX(attributelose) -= 1;

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low stats", KILLED_BY);
			}
			if(ABASE(attributelose) > ATTRMIN(attributelose)) {
				ABASE(attributelose) -= 1;
				AMAX(attributelose) -= 1;

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low stats", KILLED_BY);
			}
			if(ABASE(attributelose) > ATTRMIN(attributelose)) {
				ABASE(attributelose) -= 1;
				AMAX(attributelose) -= 1;

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low stats", KILLED_BY);
			}

			}
			break;
		case 4:
			pline("You feel all your attributes draining.");
			if(ABASE(A_STR) > ATTRMIN(A_STR)) {
				ABASE(A_STR) -= 1;
				AMAX(A_STR) -= 1;

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low strength", KILLED_BY);
			}
			if(ABASE(A_DEX) > ATTRMIN(A_DEX)) {
				ABASE(A_DEX) -= 1;
				AMAX(A_DEX) -= 1;

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low dexterity", KILLED_BY);
			}
			if(ABASE(A_WIS) > ATTRMIN(A_WIS)) {
				ABASE(A_WIS) -= 1;
				AMAX(A_WIS) -= 1;

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low wisdom", KILLED_BY);
			}
			if(ABASE(A_INT) > ATTRMIN(A_INT)) {
				ABASE(A_INT) -= 1;
				AMAX(A_INT) -= 1;

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low intelligence", KILLED_BY);
			}
			if(ABASE(A_CHA) > ATTRMIN(A_CHA)) {
				ABASE(A_CHA) -= 1;
				AMAX(A_CHA) -= 1;

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low charisma", KILLED_BY);
			}
			if(ABASE(A_CON) > ATTRMIN(A_CON)) {
				ABASE(A_CON) -= 1;
				AMAX(A_CON) -= 1;

			} else {
				losehp(rnd(10 + (u.ulevel * 5)), "fatally low constitution", KILLED_BY);
			}
			break;
		case 5:
			pline("You cannot pray any longer.");
			u.ublesscnt += 1000000;
			break;
		case 6:
			pline("Your alignment is nuked.");
			u.ualign.sins += 100;
			u.alignlim -= 100;
		      adjalign(-1000);
			break;
		case 7:
			pline("Your level is resetted to 1.");
			while (u.ulevel > 1) losexp("deleterious level drain", TRUE, FALSE);
			if (u.uhpmax < (urole.hpadv.infix + urace.hpadv.infix)) {
				u.uhpmax = urole.hpadv.infix + urace.hpadv.infix;
				if (Role_if(PM_DQ_SLIME) && Race_if(PM_PLAYER_SLIME)) u.uhpmax += 20;
				if (u.uhp < u.uhpmax) u.uhp = u.uhpmax;
			}
			break;
		case 8:
			pline("You feel a loss of intrinsics...");
			attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse();
			break;
		case 9:
			pline("One of your intrinsics is permanently deactivated!");
			deacrandomintrinsic(1000000);

			break;
		case 10:
			pline("You feel much less skilled than before.");
			skillcaploss(); skillcaploss(); skillcaploss(); skillcaploss(); skillcaploss(); skillcaploss(); skillcaploss(); skillcaploss(); skillcaploss(); skillcaploss();
			break;
		case 11:
			pline("Your possessions are severely damaged!");
			withering_damage(invent, FALSE, FALSE);
			withering_damage(invent, FALSE, FALSE);
			break;
		case 12:
			pline("Your magical items turn into mundane ones.");
			lethe_damage(invent, FALSE, FALSE);
			lethe_damage(invent, FALSE, FALSE);
			lethe_damage(invent, FALSE, FALSE);
			lethe_damage(invent, FALSE, FALSE);
			lethe_damage(invent, FALSE, FALSE);
			actual_lethe_damage(invent, FALSE, FALSE);
			actual_lethe_damage(invent, FALSE, FALSE);
			actual_lethe_damage(invent, FALSE, FALSE);
			actual_lethe_damage(invent, FALSE, FALSE);
			actual_lethe_damage(invent, FALSE, FALSE);
			break;
		case 13:
			pline("You lose all of your armor!");
			{
			struct obj *otmpD;
			otmpD = some_armor(&youmonst);
			if(otmpD) useup(otmpD);
			otmpD = some_armor(&youmonst);
			if(otmpD) useup(otmpD);
			otmpD = some_armor(&youmonst);
			if(otmpD) useup(otmpD);
			otmpD = some_armor(&youmonst);
			if(otmpD) useup(otmpD);
			otmpD = some_armor(&youmonst);
			if(otmpD) useup(otmpD);
			otmpD = some_armor(&youmonst);
			if(otmpD) useup(otmpD);
			otmpD = some_armor(&youmonst);
			if(otmpD) useup(otmpD);
			}

			break;
		case 14:
			pline("Your health and mana are severely damaged!");
			if (u.uhpmax > 1) u.uhpmax /= 2;
			else {
				killer_format = KILLED_BY;
				killer = "running out of health";
				done(DIED);
			}
			if (u.uenmax > 1) u.uenmax /= 2;
			else u.uenmax = 0;
			if (Upolyd) {
				u.mhmax /= 2;
				if (u.mh > u.mhmax) u.mh = u.mhmax;
			}
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			break;
		case 15:
			pline("Some of your techniques no longer work...");
			datadeletetechs();
			break;
		case 16:
			pline("Your items are randomly deleted!");

			{
			struct obj *otmpD, *otmpE;

			for (otmpD = invent; otmpD; otmpD = otmpE) {
			      otmpE = otmpD->nobj;

				if (rn2(5)) continue;

				if (evades_destruction(otmpD) ) dropx(otmpD);
				else {

					/* if it's a bag of holding or other container, scatter the contents first --Amy */
					if (Has_contents(otmpD)) {
						dump_container(otmpD, FALSE);
						scatter(u.ux,u.uy,10,VIS_EFFECTS|MAY_HIT|MAY_DESTROY|MAY_FRACTURE,0);
					}
					/* and if it still somehow has contents now, delete them */
					if (Has_contents(otmpD)) delete_contents(otmpD);
					useup(otmpD);
				}
			}

			}

			break;
		case 17:
			pline("Your items are cursed and disenchanted!");
			{
			struct obj *otmpD, *otmpE;

			for (otmpD = invent; otmpD; otmpD = otmpE) {
			      otmpE = otmpD->nobj;

				curse(otmpD);
				if (otmpD->spe > -1) otmpD->spe = -1;
				/* bugsniper said this effect isn't evil enough; let's disenchant stuff down to -50 --Amy */
				else if (otmpD->spe > -50) otmpD->spe--;

			}

			}

			break;

	}

}

int
dodrink()
{
	register struct obj *otmp;
	const char *potion_descr;
	char quaffables[SIZE(beverages) + 2];
	char *qp = quaffables;

	if (Strangled) {
		pline(Hallucination ? "You don't wanna do booze right now." : "If you can't breathe air, how can you drink liquid?");
		if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	}
	if (uarmh && (uarmh->otyp == PLASTEEL_HELM || uarmh->otyp == HELM_OF_STORMS || uarmh->otyp == HELM_OF_DETECT_MONSTERS) ){
		pline("The %s covers your whole face.", xname(uarmh));
		if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	}

	*qp++ = ALLOW_FLOOROBJ;
	if (!u.uswallow && (IS_FOUNTAIN(levl[u.ux][u.uy].typ) ||
			    IS_SINK(levl[u.ux][u.uy].typ) ||
			    IS_TOILET(levl[u.ux][u.uy].typ) ||
			    IS_WELL(levl[u.ux][u.uy].typ) ||
			    IS_POISONEDWELL(levl[u.ux][u.uy].typ) ||
			    Underwater || (IS_POOL(levl[u.ux][u.uy].typ) && !(IS_CRYSTALWATER(levl[u.ux][u.uy].typ))) ))
	    *qp++ = ALLOW_THISPLACE;
	strcpy(qp, beverages);

	otmp = getobj(quaffables, "drink");
	if (otmp == &thisplace) {
	    if (IS_FOUNTAIN(levl[u.ux][u.uy].typ)) {
		drinkfountain();
		if (u.uprops[DEHYDRATION].extrinsic || Dehydration || have_dehydratingstone() ) {
			u.dehydrationtime = moves + 1001;
		}

		if (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "water-pipe helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "shlem kal'yannym") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "suv-quvur dubulg'a") ) ) {
			morehungry(-10);
		}

		if (RngeLiquidDiet) {
			morehungry(-10);
		}

		return 1;
	    }
	    else if (IS_WELL(levl[u.ux][u.uy].typ)) {
		You("draw water from a well.");

		if (level.flags.lethe) {
			pline("Whoops, you forgot that it contains lethe water.");
			if (Hallucination) pline("You also forgot whether lethe water can cause amnesia.");
			forget(ALL_SPELLS | ALL_MAP);
		}

		morehungry(-50);
		if (RngeLiquidDiet) {
			morehungry(-10);
		}
		healup(d(2,6) + rnz(u.ulevel), 0, FALSE, FALSE);
		if (!rn2(10)) {
			levl[u.ux][u.uy].typ = POISONEDWELL;
			pline("Suddenly the well becomes poisoned...");
		} else if (!rn2(100)) {
			levl[u.ux][u.uy].typ = CORR;
			pline("The well dries up!");
		}
		return 1;
	    }
	    else if (IS_POISONEDWELL(levl[u.ux][u.uy].typ)) {
		You("draw water from a well.");

		if (level.flags.lethe) {
			pline("Whoops, you forgot that it contains lethe water.");
			if (Hallucination) pline("You also forgot whether lethe water can cause amnesia.");
			forget(ALL_SPELLS | ALL_MAP);
		}

		morehungry(-50);
		if (RngeLiquidDiet) {
			morehungry(-10);
		}
		pline(Hallucination ? "Urgh - that tastes like cactus juice with full-length thorns in it!" : "Ecch - that must have been poisonous!");
		if(!Poison_resistance) {
		    losestr(rnd(4));
		    losehp(rnd(15), xname(otmp), KILLED_BY_AN);
		} else You("resist the effects but still don't feel so good.");
		if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_STR, -rnd(2), FALSE);
		if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_DEX, -rnd(2), FALSE);
		if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_CON, -rnd(2), FALSE);
		if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_INT, -rnd(2), FALSE);
		if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_WIS, -rnd(2), FALSE);
		if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_CHA, -rnd(2), FALSE);
		poisoned("The water", rn2(A_MAX), "poisoned well", 30);
		if (!rn2(20)) {
			levl[u.ux][u.uy].typ = CORR;
			pline("The well dries up!");
		}
		return 1;

	    }
	    else if (IS_SINK(levl[u.ux][u.uy].typ)) {
		drinksink();
		if (u.uprops[DEHYDRATION].extrinsic || Dehydration || have_dehydratingstone() ) {
			u.dehydrationtime = moves + 1001;
		}

		if (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "water-pipe helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "shlem kal'yannym") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "suv-quvur dubulg'a") ) ) {
			morehungry(-10);
		}

		if (RngeLiquidDiet) {
			morehungry(-10);
		}

		return 1;
	    }
	    else if (IS_TOILET(levl[u.ux][u.uy].typ)) {
		drinktoilet();
		if (u.uprops[DEHYDRATION].extrinsic || Dehydration || have_dehydratingstone() ) {
			u.dehydrationtime = moves + 1001;
		}

		if (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "water-pipe helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "shlem kal'yannym") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "suv-quvur dubulg'a") ) ) {
			morehungry(-10);
		}

		if (RngeLiquidDiet) {
			morehungry(-10);
		}

		return 1;
	    }
	    pline(Hallucination ? "This water seems especially clean. In fact, it's the cleanest water you've ever seen." : "Do you know what lives in this water!");

		if (level.flags.lethe) {

			pline("In any case, you apparently forgot that it causes amnesia.");
			if (Hallucination) pline("You also forgot about Maud.");
			forget(rnd(10));

		}

		if (u.uprops[DEHYDRATION].extrinsic || Dehydration || have_dehydratingstone() ) {
			u.dehydrationtime = moves + 1001;
		}

		if (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "water-pipe helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "shlem kal'yannym") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "suv-quvur dubulg'a") ) ) {
			morehungry(-10);
		}

		if (RngeLiquidDiet) {
			morehungry(-10);
		}

	    return 1;
	}
	if(!otmp) return(0);

	if (InterruptEffect || u.uprops[INTERRUPT_EFFECT].extrinsic || have_interruptionstone()) {
		nomul(-(rnd(5)), "quaffing a potion", TRUE);
	}

	otmp->in_use = TRUE;		/* you've opened the stopper */

#define POTION_OCCUPANT_CHANCE(n) (13 + 2*(n))	/* also in muse.c */

	potion_descr = OBJ_DESCR(objects[otmp->otyp]);

	use_skill(P_DEVICES,1);
	if (Race_if(PM_FAWN)) {
		use_skill(P_DEVICES,1);
	}
	if (Race_if(PM_SATRE)) {
		use_skill(P_DEVICES,1);
		use_skill(P_DEVICES,1);
	}

	if (potion_descr) {
	    if ( (!strcmp(potion_descr, "milky") || !strcmp(potion_descr, "ghostly") || !strcmp(potion_descr, "hallowed") || !strcmp(potion_descr, "spiritual")) &&
		    flags.ghost_count < MAXMONNO &&
		    !rn2(POTION_OCCUPANT_CHANCE(flags.ghost_count))) {
		ghost_from_bottle();
		if (carried(otmp)) useup(otmp);
		else useupf(otmp, 1L);
		return(1);
	    } else if (!strcmp(potion_descr, "smoky") &&
		    (flags.djinni_count < MAXMONNO) &&
		    !rn2(POTION_OCCUPANT_CHANCE(flags.djinni_count))) {
		djinni_from_bottle(otmp, 1);
		if (carried(otmp)) useup(otmp);
		else useupf(otmp, 1L);
		return(1);
	    } else if (!strcmp(potion_descr, "vapor") &&
		    (flags.dao_count < MAXMONNO) &&
		    !rn2(POTION_OCCUPANT_CHANCE(flags.dao_count))) {
		djinni_from_bottle(otmp, 2);
		if (carried(otmp)) useup(otmp);
		else useupf(otmp, 1L);
		return(1);
	    } else if (!strcmp(potion_descr, "fuming") &&
		    (flags.efreeti_count < MAXMONNO) &&
		    !rn2(POTION_OCCUPANT_CHANCE(flags.efreeti_count))) {
		djinni_from_bottle(otmp, 3);
		if (carried(otmp)) useup(otmp);
		else useupf(otmp, 1L);
		return(1);
	    } else if (!strcmp(potion_descr, "sizzling") &&
		    (flags.marid_count < MAXMONNO) &&
		    !rn2(POTION_OCCUPANT_CHANCE(flags.marid_count))) {
		djinni_from_bottle(otmp, 4);
		if (carried(otmp)) useup(otmp);
		else useupf(otmp, 1L);
		return(1);
	    }
	}
	return dopotion(otmp);
}

int
dopotion(otmp)
register struct obj *otmp;
{
	int retval;

	otmp->in_use = TRUE;
	nothing = unkn = 0;

	if (u.uprops[DEHYDRATION].extrinsic || Dehydration || have_dehydratingstone() ) {
		u.dehydrationtime = moves + 1001;
	}

	if (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "water-pipe helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "shlem kal'yannym") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "suv-quvur dubulg'a") ) ) {
		morehungry(-10);
	}

	if (RngeLiquidDiet) {
		morehungry(-10);
	}

	if((retval = peffects(otmp)) >= 0) return(retval);

	if(nothing) {
	    unkn++;
	    You("have a %s feeling for a moment, then it passes.",
		  Hallucination ? "normal" : "peculiar");
	}
	if(otmp->dknown && !objects[otmp->otyp].oc_name_known) {
		if(!unkn) {
			makeknown(otmp->otyp);
			more_experienced(0,10);
		} else if(!objects[otmp->otyp].oc_uname)
			docall(otmp);
	}
	if (carried(otmp)) {useup(otmp);
		make_bottle(FALSE);}
	else if (mcarried(otmp)) m_useup(otmp->ocarry, otmp);
	else if (otmp->where == OBJ_FLOOR) useupf(otmp, 1L);
	else dealloc_obj(otmp);		/* Dummy potion */

	if (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "levuntation cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "levitatsii plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "havo rido kiygan suzadi") )) badeffect();

	return(1);
}

/* return -1 if potion is used up,  0 if error,  1 not used */
int
peffects(otmp)
	register struct obj	*otmp;
{
	register int i, ii, lim;

	if ( (DSTWProblem || u.uprops[DSTW_BUG].extrinsic || (uarmh && uarmh->oartifact == ART_UNIMPLEMENTED_FEATURE) || have_dstwstone() ) && !rn2(5)) {

		pline("The potion doesn't seem to work!"); /* DSTW = abbreviation for "doesn't seem to work" --Amy */
		return(-1);

	}

	if (otmp->selfmade && !(5-rnl(6))) {
		pline("That potion was bad!");
		switch(rnl(5)) {
			case 0:
			case 1:
			case 2:
				break;

			case 3:
				if (Poison_resistance) break;
				You_feel("sick.");
				losehp(rnd(20),"bad chemical knowledge",KILLED_BY);
				break;

			case 4:
				make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON),20),
			"bad potion", TRUE, SICK_VOMITABLE);
				break;
		}
		return(-1);
	}

	if (RngeLevuntation) badeffect();

	if (otmp->otyp == POT_WONDER || otmp->otyp == POT_TERCES_DLU) {

		struct obj *wonderpot;
		wonderpot = mkobj(POTION_CLASS,FALSE);
		if (wonderpot) otmp->otyp = wonderpot->otyp;
		if (otmp->otyp == GOLD_PIECE) otmp->otyp = POT_WATER; /* minimalist fix */
		if (wonderpot) obfree(wonderpot, (struct obj *)0);
		unkn++;

	}

	/* KMH, balance patch -- this is too cruel for novices */
#if 0
	/* sometimes your constitution can be a little _too_ high! */
	if ((Role_if(PM_BARBARIAN) || ACURR(A_CON) > 15) && !rn2(5)) {
		pline("Strange ...");
		nothing++;
		return(-1);
	}
#endif

	switch(otmp->otyp){
	case POT_RESTORE_ABILITY:
	case SPE_RESTORE_ABILITY:
		unkn++;
		if(otmp->cursed) {
		    pline("Ulch!  This makes you feel mediocre!");
		    break;
		} else {
		    pline("Wow!  This makes you feel %s!",
			  (otmp->blessed) ?
				(unfixable_trouble_count(FALSE) ? "better" : "great")
			  : "good");
		    i = rn2(A_MAX);		/* start at a random point */
		    for (ii = 0; ii < A_MAX; ii++) {
			lim = AMAX(i);
			if (i == A_STR && u.uhs >= 3) --lim;	/* WEAK */
			if (ABASE(i) < lim) {
			    if (otmp->otyp == SPE_RESTORE_ABILITY) {
					if (rn2(8)) ABASE(i)++;
					else {
						pline("It actually didn't work though...");
						AMAX(i) -= 1;
					}
			    }
			    else ABASE(i) = lim;
			    flags.botl = 1;
			    /* only first found if not blessed */
			    if (!otmp->blessed) break;
			}
			if(++i >= A_MAX) i = 0;
		    }
		}
		break;
	case POT_HALLUCINATION:
            makeknown (POT_HALLUCINATION);
		if (Hallucination || Halluc_resistance) nothing++;
		else makeknown(otmp->otyp);
		(void) make_hallucinated(itimeout_incr(HHallucination,
					   rn1(200, 600 - 300 * bcsign(otmp))),
				  TRUE, 0L);
		break;
	case POT_ICE:
		if(!Frozen) {
		    if (Hallucination) {
			pline("Giant ice-cream cones... mmmmm!");
			unkn++;
		    } else
			pline("Suddenly, you're frozen solid!");
		} else	nothing++;

		make_frozen(itimeout_incr(HFrozen,
					    rn1(35, 80 - 25 * bcsign(otmp))), FALSE);

		break;
	case POT_FEAR:
		if(!Feared) {
		    if (Hallucination) {
			You_feel("like you're dying from the inside! Waaaaah! Where's my Mommy?");
			unkn++;
		    } else
			pline("Suddenly, you're trembling in fear!");
		} else	nothing++;

		make_feared(itimeout_incr(HFeared,
					    rn1(200, 600 - 300 * bcsign(otmp))), FALSE);

		break;
	case POT_FIRE:
		if(!Burned) {
		    if (Hallucination) {
			pline("Wow! You just swallowed a flamethrower - your tongue can spit fire! Yee-haw!");
			unkn++;
		    } else
			pline("Ooph! Concentrated habanero chili peppers!");
		} else	nothing++;

		make_burned(itimeout_incr(HBurned,
					    rn1(100, 100 - 25 * bcsign(otmp))), FALSE);

		break;
	case POT_DIMNESS:
		if(!Dimmed) {
		    if (Hallucination) {
			pline("It's antidepressiva - no, wait, it's the opposite of it... oh no! Your life is worthless and you want to jump off a bridge!");
			unkn++;
		    } else
			pline("This tastes like bitter medicine, and your senses are dulled!");
		} else	nothing++;

		make_dimmed(itimeout_incr(HDimmed,
					    rn1(100, 100 - 25 * bcsign(otmp))), FALSE);

		break;
	case POT_STUNNING:
		if(!Stunned) {
		    if (Hallucination) {
			pline("You wobble around! How funny!");
			unkn++;
		    } else
			pline("You're staggering!");
		} else	nothing++;

		make_stunned(itimeout_incr(HStun,
					    rn1(35, 80 - 25 * bcsign(otmp))), FALSE);

		break;
	case POT_NUMBNESS:
		if(!Numbed) {
		    if (Hallucination) {
			pline("Oh! Some of your limbs seem to be talking to you!");
			unkn++;
		    } else
			pline("Your limbs start feeling numb!");
		} else	nothing++;

		make_numbed(itimeout_incr(HNumbed,
					    rn1(100, 200 - 75 * bcsign(otmp))), FALSE);
		break;

	case POT_CANCELLATION:

		    attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse();

		unkn++;
		break;

	case POT_SLIME:

		if (!Slimed && !flaming(youmonst.data) && !Unchanging && !slime_on_touch(youmonst.data) ) {
		    You("don't feel very well.");
		    Slimed = 100L;
		    flags.botl = 1;
		}
		unkn++;

		break;

	case POT_URINE:

		pline("Eek! This tastes indescibably bad...");

		if ((monstermoves - otmp->age) < 51) {

		exercise(A_WIS, FALSE);
		make_vomiting(Vomiting + rnd(10) + 5,TRUE);
		if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
		}
		else {
			make_sick(Sick ? Sick/2L + 1L : 10, "urine potion", TRUE, SICK_VOMITABLE);
			losestr(rnd(10));
			losehp(d(otmp->cursed ? 4 : 2, otmp->blessed ? 8 : 16), "drinking poisonous urine", KILLED_BY);

		}

		unkn++;

		break;

	case POT_AMNESIA:
		pline(Hallucination? "This tastes like champagne!" :
			"This liquid bubbles and fizzes as you drink it.");
		forget((!otmp->blessed? ALL_SPELLS : 0) | ALL_MAP);
		if (Hallucination)
		    pline("Hakuna matata!");
		else
		    You_feel("your memories dissolve.");

		/* Blessed amnesia makes you forget lycanthropy, sickness */
		if (otmp->blessed) {
		    if (u.ulycn >= LOW_PM && !Race_if(PM_HUMAN_WEREWOLF) && !Role_if(PM_LUNATIC) && !Race_if(PM_AK_THIEF_IS_DEAD_) ) {
			You("forget your affinity to %s!",
					makeplural(mons[u.ulycn].mname));
			if (youmonst.data == &mons[u.ulycn])
			    you_unwere(FALSE);
			u.ulycn = NON_PM;	/* cure lycanthropy */
		    }
		    make_sick(0L, (char *) 0, TRUE, SICK_ALL);

		    /* You feel refreshed */
		    u.uhunger += 50 + rnd(50);
		    newuhs(FALSE);
		} else
		    exercise(A_WIS, FALSE);
		break;
	case POT_WATER:
		if(!otmp->blessed && !otmp->cursed) {
		    pline("This tastes like water.");
		    u.uhunger += rnd(10);
		    newuhs(FALSE);
		    break;
		}
		unkn++;
		if(is_undead(youmonst.data) || is_demon(youmonst.data) ||
				u.ualign.type == A_CHAOTIC) {
		    if(otmp->blessed) {
			pline("This burns like acid!");
			exercise(A_CON, FALSE);
			if (u.ulycn >= LOW_PM && !Race_if(PM_HUMAN_WEREWOLF) && !Role_if(PM_LUNATIC) && !Race_if(PM_AK_THIEF_IS_DEAD_) ) {
			    Your("affinity to %s disappears!",
				 makeplural(mons[u.ulycn].mname));
			    if (youmonst.data == &mons[u.ulycn])
				you_unwere(FALSE);
			    u.ulycn = NON_PM;	/* cure lycanthropy */
			}
			losehp(d(6,6), "potion of holy water", KILLED_BY_AN);
		    } else if(otmp->cursed) {
			You_feel("quite proud of yourself.");
			healup(d(6,6),0,0,0);
			if (u.ulycn >= LOW_PM && !Upolyd) you_were();
			exercise(A_CON, TRUE);
		    }
		} else {
		    if(otmp->blessed) {
			You_feel("full of awe.");
			if(u.ualign.type == A_LAWFUL) healup(d(6,6),0,0,0);                        
			make_sick(0L, (char *) 0, TRUE, SICK_ALL);
			exercise(A_WIS, TRUE);
			exercise(A_CON, TRUE);
			if (u.ulycn >= LOW_PM && !Race_if(PM_HUMAN_WEREWOLF) && !Role_if(PM_LUNATIC) && !Race_if(PM_AK_THIEF_IS_DEAD_) ) {
			    you_unwere(TRUE);	/* "Purified" */
			}
			/* make_confused(0L,TRUE); */
		    } else {
			if(u.ualign.type == A_LAWFUL) {
			    pline("This burns like acid!");
			    losehp(d(6,6), "potion of unholy water",
				KILLED_BY_AN);
			} else
			    You_feel("full of dread.");
			if (u.ulycn >= LOW_PM && !Upolyd) you_were();
			exercise(A_CON, FALSE);
		    }
		}
		break;
	case POT_BOOZE:

		if (otmp->oartifact == ART_BOOMSHINE) {
			pline("KABOOM!!! You are hit by a massive explosion!");
			losehp(rnd(50), "exploding boomshine", KILLED_BY);
		      u.uprops[CONFUSION].intrinsic |= FROMOUTSIDE;
			HConfusion |= FROMOUTSIDE;
			HConfusion |= FROMRACE;
			HConfusion |= FROMEXPER;
			u.boomshined = 1;
			pline("That was very stupid of you, and your body will never be as it used to be...");
		}

	case POT_WINE:
		unkn++;

		if (Role_if(PM_PIRATE) || Role_if(PM_KORSAIR) || (uwep && uwep->oartifact == ART_ARRRRRR_MATEY) ) pline("Ye splice the mainbrace.");
		else pline("Ooph!  This tastes like %s%s!",
		      otmp->odiluted ? "watered down " : "",
		      Hallucination ? "dandelion wine" : "liquid fire");
		if (!otmp->blessed)
		    make_confused(itimeout_incr(HConfusion, d(3,8)), FALSE);
		/* the whiskey makes us feel better */
		if (!otmp->odiluted) healup(Role_if(PM_DRUNK) ? rnz(20 + u.ulevel) : 1, 0, FALSE, FALSE);
		u.uhunger += 10 * (2 + bcsign(otmp));
		if (Race_if(PM_CLOCKWORK_AUTOMATON)) u.uhunger += 200;
		if (Role_if(PM_DRUNK)) u.uhunger += 100;
		newuhs(FALSE);
		exercise(A_WIS, FALSE);
		if(otmp->cursed) {
			You((Role_if(PM_PIRATE) || Role_if(PM_KORSAIR) || (uwep && uwep->oartifact == ART_ARRRRRR_MATEY) ) ? "are loaded to the gunwhales." : "pass out.");
			multi = -rnd(15);
			nomovemsg = "You awake with a headache.";
		}
		break;
	case POT_ENLIGHTENMENT:
		if(otmp->cursed) {
			unkn++;
			You("have an uneasy feeling...");
			exercise(A_WIS, FALSE);
		} else {
			if (otmp->blessed) {
				if (!rn2(3)) (void) adjattrib(A_INT, 1, FALSE);
				if (!rn2(3)) (void) adjattrib(A_WIS, 1, FALSE);
			}
			You_feel("self-knowledgeable...");
			display_nhwindow(WIN_MESSAGE, FALSE);
			enlightenment(0, 1);
			pline_The("feeling subsides.");
			exercise(A_WIS, TRUE);
		}
		break;
	case SPE_INVISIBILITY:
		/* spell cannot penetrate mummy wrapping */
		if (BInvis && uarmc->otyp == MUMMY_WRAPPING) {
			You_feel("rather itchy under your %s.", xname(uarmc));
			break;
		}
		/* FALLTHRU */
	case POT_INVISIBILITY:
		if (Invis || Blind || BInvis) {
		    nothing++;
		} else {
		    self_invis_message();
		}
		if (otmp->blessed) HInvis |= FROMOUTSIDE;
		else incr_itimeout(&HInvis, rn1(15,31));
		newsym(u.ux,u.uy);	/* update position */
		if(otmp->cursed) {
		    pline(Hallucination ? "Somehow, you get the feeling there's a stalker waiting for you around the corner." : "For some reason, you feel your presence is known.");
		    aggravate();
		}
		break;
	case POT_SEE_INVISIBLE:
		/* tastes like fruit juice in Rogue */
	case POT_FRUIT_JUICE:
	    {
		int msg = Invisible && !Blind;

		unkn++;
		if (otmp->cursed)
		    pline("Yecch!  This tastes %s.",
			  Hallucination ? "overripe" : "rotten");
		else
		    pline(Hallucination ?
		      "This tastes like 10%% real %s%s all-natural beverage." :
				"This tastes like %s%s.",
			  otmp->odiluted ? "reconstituted " : "",
			  fruitname(TRUE));
		if (otmp->otyp == POT_FRUIT_JUICE) {
		    u.uhunger += (otmp->odiluted ? 5 : 10) * (2 + bcsign(otmp));
		    newuhs(FALSE);
		    break;
		}
		if (!otmp->cursed) {
			/* Tell them they can see again immediately, which
			 * will help them identify the potion...
			 */
			make_blinded(0L,TRUE);
		}
		if (otmp->blessed)
			HSee_invisible |= FROMOUTSIDE;
		else
			incr_itimeout(&HSee_invisible, rn1(100,750));
		set_mimic_blocking(); /* do special mimic handling */
		see_monsters();	/* see invisible monsters */
		newsym(u.ux,u.uy); /* see yourself! */
		if (msg && !Blind) { /* Blind possible if polymorphed */
		    You("can see through yourself, but you are visible!");
		    unkn--;
		}
		break;
	    }
	case POT_COFFEE:
		if (otmp->cursed)
		    pline("Yecch!  This tastes %s.",
			  Hallucination ? "like sewage" : "extremely bitter");
		else
		    pline(Hallucination ?
		      "This tastes like bean juice." :
				"This tastes like coffee." );
		    u.uhunger += (otmp->odiluted ? 10 : 20) * (2 + bcsign(otmp));
		    newuhs(FALSE);
		   if (!otmp->cursed) HSleep_resistance += rnd(25);

		break;

	case POT_RED_TEA:
		if (otmp->cursed)
		    pline("Yecch!  This tastes %s.",
			  Hallucination ? "like blood.." : "awful");
		else
		    pline(Hallucination ?
		      "This tastes like cherry juice." :
				"This tastes like red tea." );
		    u.uhunger += (otmp->odiluted ? 20 : 40) * (2 + bcsign(otmp));
		    newuhs(FALSE);
		   if (!otmp->cursed) HFire_resistance += rnd(25);

		break;

	case POT_OOLONG_TEA:
		if (otmp->cursed)
		    pline("Yecch!  This tastes %s.",
			  Hallucination ? "like death" : "very rotten");
		else
		    pline(Hallucination ?
		      "This tastes like peppermint." :
				"This tastes like oolong tea." );
		    u.uhunger += (otmp->odiluted ? 20 : 40) * (2 + bcsign(otmp));
		    newuhs(FALSE);
		   if (!otmp->cursed) HShock_resistance += rnd(25);

		break;

	case POT_GREEN_TEA:
		if (otmp->cursed)
		    pline("Yecch!  This tastes %s.",
			  Hallucination ? "like the contents of a trash can" : "poisonous");
		else
		    pline(Hallucination ?
		      "This tastes like your mother-in-law's tea!" :
				"This tastes like green tea." );
		    u.uhunger += (otmp->odiluted ? 20 : 40) * (2 + bcsign(otmp));
		    newuhs(FALSE);
		   if (!otmp->cursed) HCold_resistance += rnd(25);

		break;

	case POT_GREEN_MATE:
		if (otmp->cursed)
		    pline("Yecch!  This tastes %s.",
			  Hallucination ? "like pee" : "stale");
		else
		    pline(Hallucination ?
		      "This tastes like bitter woodruff!" :
				"This tastes like green mate." );
		    u.uhunger += (otmp->odiluted ? 20 : 40) * (2 + bcsign(otmp));
		    newuhs(FALSE);
		   if (!otmp->cursed) HPoison_resistance += rnd(25);

		break;

	case POT_COCOA:
		if (otmp->cursed)
		    pline("Yecch!  This tastes %s.",
			  Hallucination ? "like shit" : "wretched");
		else
		    pline(Hallucination ?
		      "This tastes like hot chocolate." :
				"This tastes like cocoa." );
		    u.uhunger += (otmp->odiluted ? 100 : 200) * (2 + bcsign(otmp));
		    newuhs(FALSE);
		   if (!otmp->cursed) HDisint_resistance += rnd(25);

		break;

	case POT_TERERE:
		if (otmp->cursed)
		    pline(Hallucination ? "This tastes like ecstasy spiked with poison." : "This tastes like stale alcohol." );
		else
		    pline(Hallucination ? "This tastes like ecstasy." : "This tastes like alcohol." );
		    u.uhunger += (otmp->odiluted ? 250 : 500) * (2 + bcsign(otmp));
		    newuhs(FALSE);

		make_confused(itimeout_incr(HConfusion, rnd(50 - 25 * bcsign(otmp))), FALSE);

		break;

	case POT_AOJIRU:
		if (otmp->cursed)
		    pline(Hallucination ? "This tastes like something that might kill you!" : "This tastes like illegal drugs." );
		else
		    pline(Hallucination ? "This tastes like some illegal shit." : "This tastes like drugs." );
		    u.uhunger += (otmp->odiluted ? 250 : 500) * (2 + bcsign(otmp));
		    newuhs(FALSE);

		make_stunned(itimeout_incr(HStun, rnd(50 - 25 * bcsign(otmp))), FALSE);

		break;

	case POT_ULTIMATE_TSUYOSHI_SPECIAL: {
		int time;
		if (otmp->cursed) {
			pline("Ulch! What in the hell was that???");
			adjattrib(A_CON,-1,-1);
			break;
		}
		You_feel("super-powerful!");
		incr_itimeout(&Invulnerable, 8 + rn2(4) );
		time = 15 + d(otmp->blessed ? 4 : 2, 8);
		incr_itimeout(&HFast, time);
		incr_itimeout(&HSee_invisible, time);
		make_hallucinated(HHallucination + time + d(10, 3), FALSE, 0L);
		u.uhpmax += rnd(5);
		u.uhp = u.uhpmax;
		} break;
	case POT_MEHOHO_BURUSASAN_G:
		pline(fauxmessage());
		if (!rn2(3)) pline(fauxmessage());
		if (otmp->cursed || !rn2(3)) {
			adjattrib(A_CON,-1,-1);
		}
		morehungry(rn1(50, 101));
		make_confused(HConfusion + d(10,2),FALSE);
		if (otmp->blessed || !rn2(3)) {
			You(Hallucination ? "feel slightly the same." : "feel slightly different.");
			if (u.uhp < u.uhpmax) u.uhp++;
		}
		break;

	case POT_TRAINING:
		{

		You("may double your amount of training points in a skill of your choice!");

		int acquiredskill;
		acquiredskill = 0;

		pline("Pick a skill to train. The prompt will loop until you actually make a choice.");

		while (acquiredskill == 0) { /* ask the player what they want --Amy */

			if (P_ADVANCE(P_DAGGER) && !(P_RESTRICTED(P_DAGGER)) && yn("Do you want to train the dagger skill?")=='y') {
				P_ADVANCE(P_DAGGER) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_KNIFE) && !(P_RESTRICTED(P_KNIFE)) && yn("Do you want to train the knife skill?")=='y') {
				P_ADVANCE(P_KNIFE) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_AXE) && !(P_RESTRICTED(P_AXE)) && yn("Do you want to train the axe skill?")=='y') {
				P_ADVANCE(P_AXE) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_PICK_AXE) && !(P_RESTRICTED(P_PICK_AXE)) && yn("Do you want to train the pick-axe skill?")=='y') {
				P_ADVANCE(P_PICK_AXE) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_SHORT_SWORD) && !(P_RESTRICTED(P_SHORT_SWORD)) && yn("Do you want to train the short sword skill?")=='y') {
				P_ADVANCE(P_SHORT_SWORD) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_BROAD_SWORD) && !(P_RESTRICTED(P_BROAD_SWORD)) && yn("Do you want to train the broad sword skill?")=='y') {
				P_ADVANCE(P_BROAD_SWORD) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_LONG_SWORD) && !(P_RESTRICTED(P_LONG_SWORD)) && yn("Do you want to train the long sword skill?")=='y') {
				P_ADVANCE(P_LONG_SWORD) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_TWO_HANDED_SWORD) && !(P_RESTRICTED(P_TWO_HANDED_SWORD)) && yn("Do you want to train the two-handed sword skill?")=='y') {
				P_ADVANCE(P_TWO_HANDED_SWORD) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_SCIMITAR) && !(P_RESTRICTED(P_SCIMITAR)) && yn("Do you want to train the scimitar skill?")=='y') {
				P_ADVANCE(P_SCIMITAR) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_SABER) && !(P_RESTRICTED(P_SABER)) && yn("Do you want to train the saber skill?")=='y') {
				P_ADVANCE(P_SABER) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_CLUB) && !(P_RESTRICTED(P_CLUB)) && yn("Do you want to train the club skill?")=='y') {
				P_ADVANCE(P_CLUB) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_PADDLE) && !(P_RESTRICTED(P_PADDLE)) && yn("Do you want to train the paddle skill?")=='y') {
				P_ADVANCE(P_PADDLE) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_MACE) && !(P_RESTRICTED(P_MACE)) && yn("Do you want to train the mace skill?")=='y') {
				P_ADVANCE(P_MACE) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_MORNING_STAR) && !(P_RESTRICTED(P_MORNING_STAR)) && yn("Do you want to train the morning star skill?")=='y') {
				P_ADVANCE(P_MORNING_STAR) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_FLAIL) && !(P_RESTRICTED(P_FLAIL)) && yn("Do you want to train the flail skill?")=='y') {
				P_ADVANCE(P_FLAIL) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_HAMMER) && !(P_RESTRICTED(P_HAMMER)) && yn("Do you want to train the hammer skill?")=='y') {
				P_ADVANCE(P_HAMMER) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_QUARTERSTAFF) && !(P_RESTRICTED(P_QUARTERSTAFF)) && yn("Do you want to train the quarterstaff skill?")=='y') {
				P_ADVANCE(P_QUARTERSTAFF) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_POLEARMS) && !(P_RESTRICTED(P_POLEARMS)) && yn("Do you want to train the polearms skill?")=='y') {
				P_ADVANCE(P_POLEARMS) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_SPEAR) && !(P_RESTRICTED(P_SPEAR)) && yn("Do you want to train the spear skill?")=='y') {
				P_ADVANCE(P_SPEAR) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_JAVELIN) && !(P_RESTRICTED(P_JAVELIN)) && yn("Do you want to train the javelin skill?")=='y') {
				P_ADVANCE(P_JAVELIN) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_TRIDENT) && !(P_RESTRICTED(P_TRIDENT)) && yn("Do you want to train the trident skill?")=='y') {
				P_ADVANCE(P_TRIDENT) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_LANCE) && !(P_RESTRICTED(P_LANCE)) && yn("Do you want to train the lance skill?")=='y') {
				P_ADVANCE(P_LANCE) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_BOW) && !(P_RESTRICTED(P_BOW)) && yn("Do you want to train the bow skill?")=='y') {
				P_ADVANCE(P_BOW) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_SLING) && !(P_RESTRICTED(P_SLING)) && yn("Do you want to train the sling skill?")=='y') {
				P_ADVANCE(P_SLING) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_FIREARM) && !(P_RESTRICTED(P_FIREARM)) && yn("Do you want to train the firearms skill?")=='y') {
				P_ADVANCE(P_FIREARM) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_CROSSBOW) && !(P_RESTRICTED(P_CROSSBOW)) && yn("Do you want to train the crossbow skill?")=='y') {
				P_ADVANCE(P_CROSSBOW) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_DART) && !(P_RESTRICTED(P_DART)) && yn("Do you want to train the dart skill?")=='y') {
				P_ADVANCE(P_DART) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_SHURIKEN) && !(P_RESTRICTED(P_SHURIKEN)) && yn("Do you want to train the shuriken skill?")=='y') {
				P_ADVANCE(P_SHURIKEN) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_BOOMERANG) && !(P_RESTRICTED(P_BOOMERANG)) && yn("Do you want to train the boomerang skill?")=='y') {
				P_ADVANCE(P_BOOMERANG) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_WHIP) && !(P_RESTRICTED(P_WHIP)) && yn("Do you want to train the whip skill?")=='y') {
				P_ADVANCE(P_WHIP) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_UNICORN_HORN) && !(P_RESTRICTED(P_UNICORN_HORN)) && yn("Do you want to train the unicorn horn skill?")=='y') {
				P_ADVANCE(P_UNICORN_HORN) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_LIGHTSABER) && !(P_RESTRICTED(P_LIGHTSABER)) && yn("Do you want to train the lightsaber skill?")=='y') {
				P_ADVANCE(P_LIGHTSABER) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_ATTACK_SPELL) && !(P_RESTRICTED(P_ATTACK_SPELL)) && yn("Do you want to train the attack spell skill?")=='y') {
				P_ADVANCE(P_ATTACK_SPELL) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_HEALING_SPELL) && !(P_RESTRICTED(P_HEALING_SPELL)) && yn("Do you want to train the healing spell skill?")=='y') {
				P_ADVANCE(P_HEALING_SPELL) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_DIVINATION_SPELL) && !(P_RESTRICTED(P_DIVINATION_SPELL)) && yn("Do you want to train the divination spell skill?")=='y') {
				P_ADVANCE(P_DIVINATION_SPELL) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_ENCHANTMENT_SPELL) && !(P_RESTRICTED(P_ENCHANTMENT_SPELL)) && yn("Do you want to train the enchantment spell skill?")=='y') {
				P_ADVANCE(P_ENCHANTMENT_SPELL) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_PROTECTION_SPELL) && !(P_RESTRICTED(P_PROTECTION_SPELL)) && yn("Do you want to train the protection spell skill?")=='y') {
				P_ADVANCE(P_PROTECTION_SPELL) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_BODY_SPELL) && !(P_RESTRICTED(P_BODY_SPELL)) && yn("Do you want to train the body spell skill?")=='y') {
				P_ADVANCE(P_BODY_SPELL) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_OCCULT_SPELL) && !(P_RESTRICTED(P_OCCULT_SPELL)) && yn("Do you want to train the occult spell skill?")=='y') {
				P_ADVANCE(P_OCCULT_SPELL) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_ELEMENTAL_SPELL) && !(P_RESTRICTED(P_ELEMENTAL_SPELL)) && yn("Do you want to train the elemental spell skill?")=='y') {
				P_ADVANCE(P_ELEMENTAL_SPELL) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_CHAOS_SPELL) && !(P_RESTRICTED(P_CHAOS_SPELL)) && yn("Do you want to train the chaos spell skill?")=='y') {
				P_ADVANCE(P_CHAOS_SPELL) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_MATTER_SPELL) && !(P_RESTRICTED(P_MATTER_SPELL)) && yn("Do you want to train the matter spell skill?")=='y') {
				P_ADVANCE(P_MATTER_SPELL) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_RIDING) && !(P_RESTRICTED(P_RIDING)) && yn("Do you want to train the riding skill?")=='y') {
				P_ADVANCE(P_RIDING) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_HIGH_HEELS) && !(P_RESTRICTED(P_HIGH_HEELS)) && yn("Do you want to train the high heels skill?")=='y') {
				P_ADVANCE(P_HIGH_HEELS) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_GENERAL_COMBAT) && !(P_RESTRICTED(P_GENERAL_COMBAT)) && yn("Do you want to train the general combat skill?")=='y') {
				P_ADVANCE(P_GENERAL_COMBAT) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_SHIELD) && !(P_RESTRICTED(P_SHIELD)) && yn("Do you want to train the shield skill?")=='y') {
				P_ADVANCE(P_SHIELD) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_BODY_ARMOR) && !(P_RESTRICTED(P_BODY_ARMOR)) && yn("Do you want to train the body armor skill?")=='y') {
				P_ADVANCE(P_BODY_ARMOR) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_TWO_HANDED_WEAPON) && !(P_RESTRICTED(P_TWO_HANDED_WEAPON)) && yn("Do you want to train the two-handed weapon skill?")=='y') {
				P_ADVANCE(P_TWO_HANDED_WEAPON) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_TWO_WEAPON_COMBAT) && !(P_RESTRICTED(P_TWO_WEAPON_COMBAT)) && yn("Do you want to train the two-weapon combat skill?")=='y') {
				P_ADVANCE(P_TWO_WEAPON_COMBAT) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_POLYMORPHING) && !(P_RESTRICTED(P_POLYMORPHING)) && yn("Do you want to train the polymorphing skill?")=='y') {
				P_ADVANCE(P_POLYMORPHING) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_DEVICES) && !(P_RESTRICTED(P_DEVICES)) && yn("Do you want to train the devices skill?")=='y') {
				P_ADVANCE(P_DEVICES) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_SEARCHING) && !(P_RESTRICTED(P_SEARCHING)) && yn("Do you want to train the searching skill?")=='y') {
				P_ADVANCE(P_SEARCHING) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_SPIRITUALITY) && !(P_RESTRICTED(P_SPIRITUALITY)) && yn("Do you want to train the spirituality skill?")=='y') {
				P_ADVANCE(P_SPIRITUALITY) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_PETKEEPING) && !(P_RESTRICTED(P_PETKEEPING)) && yn("Do you want to train the petkeeping skill?")=='y') {
				P_ADVANCE(P_PETKEEPING) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_MISSILE_WEAPONS) && !(P_RESTRICTED(P_MISSILE_WEAPONS)) && yn("Do you want to train the missile weapons skill?")=='y') {
				P_ADVANCE(P_MISSILE_WEAPONS) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_TECHNIQUES) && !(P_RESTRICTED(P_TECHNIQUES)) && yn("Do you want to train the techniques skill?")=='y') {
				P_ADVANCE(P_TECHNIQUES) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_IMPLANTS) && !(P_RESTRICTED(P_IMPLANTS)) && yn("Do you want to train the implants skill?")=='y') {
				P_ADVANCE(P_IMPLANTS) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_SHII_CHO) && !(P_RESTRICTED(P_SHII_CHO)) && yn("Do you want to train the form I (Shii-Cho) skill?")=='y') {
				P_ADVANCE(P_SHII_CHO) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_MAKASHI) && !(P_RESTRICTED(P_MAKASHI)) && yn("Do you want to train the form II (Makashi) skill?")=='y') {
				P_ADVANCE(P_MAKASHI) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_SORESU) && !(P_RESTRICTED(P_SORESU)) && yn("Do you want to train the form III (Soresu) skill?")=='y') {
				P_ADVANCE(P_SORESU) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_ATARU) && !(P_RESTRICTED(P_ATARU)) && yn("Do you want to train the form IV (Ataru) skill?")=='y') {
				P_ADVANCE(P_ATARU) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_SHIEN) && !(P_RESTRICTED(P_SHIEN)) && yn("Do you want to train the form V (Shien) skill?")=='y') {
				P_ADVANCE(P_SHIEN) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_DJEM_SO) && !(P_RESTRICTED(P_DJEM_SO)) && yn("Do you want to train the form V (Djem So) skill?")=='y') {
				P_ADVANCE(P_DJEM_SO) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_NIMAN) && !(P_RESTRICTED(P_NIMAN)) && yn("Do you want to train the form VI (Niman) skill?")=='y') {
				P_ADVANCE(P_NIMAN) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_JUYO) && !(P_RESTRICTED(P_JUYO)) && yn("Do you want to train the form VII (Juyo) skill?")=='y') {
				P_ADVANCE(P_JUYO) *= 2;
				acquiredskill = 1; }
			else if (P_ADVANCE(P_VAAPAD) && !(P_RESTRICTED(P_VAAPAD)) && yn("Do you want to train the form VII (Vaapad) skill?")=='y') {
				P_ADVANCE(P_VAAPAD) *= 2;
				acquiredskill = 1; }
			else if (yn("Do you want to train no skill at all?")=='y') {
				acquiredskill = 1;
			}
		} /* ask the player what they want loop */
		pline("Training complete!");

		}

		break;

	case POT_BENEFICIAL_EFFECT:

		switch (rnd(10)) {

			case 1:
				if (ABASE(A_CHA) < ATTRMAX(A_CHA)) {
					You_feel("more %s!", flags.female ? "pretty" : "attractive");
					(void) adjattrib(A_CHA, 1, FALSE);
					break;
				}
			break;
			case 2:
				if (ABASE(A_CON) < ATTRMAX(A_CON)) {
					You_feel("tougher!");
					(void) adjattrib(A_CON, 1, FALSE);
					}
			break;
			case 3:
				u.uenmax += rnd(3);
				You_feel("a mild buzz.");
				flags.botl = 1;
			break;
			case 4:
				if (Upolyd) {
					u.mh++;
					u.mhmax++;
				} else {
					u.uhp++;
					u.uhpmax++;
				}
				You_feel("vitalized.");
				flags.botl = 1;
			break;
			case 5:
				You_feel("clairvoyant!");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vy znayete raspolozheniye, no ne lovushki, a te vse ravno budut poshel na khuy vverkh." : "Wschiiiiiiie!");
				incr_itimeout(&HClairvoyant, rnd(500));
			break;
			case 6:
			      You_feel("that was a smart thing to do.");
				pluslvl(FALSE);
			break;
			case 7:
				if (Upolyd) u.mh = u.mhmax;
				else u.uhp = u.uhpmax;
				You_feel("restored to health!");
				flags.botl = 1;
			break;
			case 8:
				if (!(HInvis & INTRINSIC)) You_feel("hidden!");
				else You_feel("able to see what cannot be seen!");
				HInvis |= FROMOUTSIDE;
				HSee_invisible |= FROMOUTSIDE;
			break;
			case 9:
				gainstr((struct obj *)0, 0);
				pline(Hallucination ? "You feel like ripping out some trees!" : "You feel stronger!");
				break;
			break;
			case 10:
				if (ABASE(A_INT) < ATTRMAX(A_INT)) {
					(void) adjattrib(A_INT, 1, FALSE);
				}
				else {
					pline(Hallucination ? "Eek, that tasted like rotten oversalted seaweed!" : "For some reason, that tasted bland.");
				}
			break;

		}

		break;

	case POT_RANDOM_INTRINSIC:
		Your("intrinsics change.");
		intrinsicgainorloss();
		break;

	case POT_PARALYSIS:
		if (Free_action && rn2(20))
		    You("stiffen momentarily.");
		else {
		    if (Levitation || Is_airlevel(&u.uz)||Is_waterlevel(&u.uz))
			You("are motionlessly suspended.");
		    else if (u.usteed)
			You("are frozen in place!");
		    else
			Your("%s are frozen to the %s!",
			     makeplural(body_part(FOOT)), surface(u.ux, u.uy));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
		    nomul(-(rn1(10, 25 - 12*bcsign(otmp))), "frozen by a potion", TRUE);
		    nomovemsg = You_can_move_again;
		    exercise(A_DEX, FALSE);
		}
		break;
	case POT_SLEEPING:
		if((Sleep_resistance || Free_action) && rn2(20))
		    You("yawn.");
		else {
		    You("suddenly fall asleep!");
		    fall_asleep(-rn1(10, 25 - 12*bcsign(otmp)), TRUE);
		}
		break;
	case POT_MONSTER_DETECTION:
	case SPE_DETECT_MONSTERS:
		if (otmp->blessed) {
		    int x, y;

		    if (Detect_monsters) nothing++;
		    unkn++;
		    /* after a while, repeated uses become less effective */
		    if (HDetect_monsters >= 300L)
			i = 20;
		    else
			i = rn1(40,21);
		    if (otmp->otyp == SPE_DETECT_MONSTERS) {
			if (HDetect_monsters >= 300L) i = rnd(2);
			else i = rnd(5);
		    }
		    incr_itimeout(&HDetect_monsters, i);
		    for (x = 1; x < COLNO; x++) {
			for (y = 0; y < ROWNO; y++) {
			    if (memory_is_invisible(x, y)) {
				unmap_object(x, y);
				newsym(x,y);
			    }
			    if (MON_AT(x,y)) unkn = 0;
			}
		    }
		    see_monsters();
		    if (unkn) You_feel("lonely.");
		    break;
		}
		if (monster_detect(otmp, 0))
			return(1);		/* nothing detected */
		exercise(A_WIS, TRUE);
		break;
	case POT_OBJECT_DETECTION:
	case SPE_DETECT_TREASURE:
		if (object_detect(otmp, 0))
			return(1);		/* nothing detected */
		exercise(A_WIS, TRUE);
		break;
	case POT_SICKNESS:
	case POT_POISON:
		pline("Yecch!  This stuff tastes like poison.");
		if (otmp->blessed) {
		    pline("(But in fact it was mildly stale %s.)",
			  fruitname(TRUE));
		    if (!Role_if(PM_HEALER) && !Race_if(PM_HERBALIST)) {
			/* NB: blessed otmp->fromsink is not possible */
			losehp(1, "mildly contaminated potion", KILLED_BY_AN);
		    }
		} else {
		    if(Poison_resistance)
			pline(
			  "(But in fact it was biologically contaminated %s.)",
			      fruitname(TRUE));
		    if (Role_if(PM_HEALER) || Race_if(PM_HERBALIST))
			pline("Fortunately, you have been immunized.");
		    else {
			int typ = rn2(A_MAX);

			if (!Fixed_abil && !Race_if(PM_SUSTAINER) && !(uarms && uarms->oartifact == ART_SYSTEMATIC_CHAOS) && !(uarms && uarms->oartifact == ART_BONUS_HOLD) && !(uamul && uamul->oartifact == ART_FIX_EVERYTHING) ) {
			    poisontell(typ);
			    (void) adjattrib(typ,
			    		Poison_resistance ? -1 : -rn1(4,3),
			    		TRUE);
			}
			if(!Poison_resistance) {
			    if (otmp->fromsink)
				losehp(rnd(10)+5*!!(otmp->cursed),
				       "contaminated tap water", KILLED_BY);
			    else
				losehp(rnd(10)+5*!!(otmp->cursed),
				       "contaminated potion", KILLED_BY_AN);
			}
			exercise(A_CON, FALSE);
		    }
		}
		if(Hallucination) {
			You("are shocked back to your senses!");
			(void) make_hallucinated(0L,FALSE,0L);
		}
		break;
	case POT_CONFUSION:
		if(!Confusion) {
		    if (Hallucination) {
			pline("What a trippy feeling!");
			unkn++;
		    } else if (Role_if(PM_PIRATE) || Role_if(PM_KORSAIR) || (uwep && uwep->oartifact == ART_ARRRRRR_MATEY) )
			pline("Blimey! Ye're one sheet to the wind!");
			else 
			pline("Huh, What?  Where am I?");
		} else	nothing++;
		make_confused(itimeout_incr(HConfusion,
					    rn1(35, 80 - 25 * bcsign(otmp))),
			      FALSE);
		break;
	case POT_CLAIRVOYANCE:
		/* KMH -- handle cursed, blessed, blocked */
		if (otmp->cursed)
			nothing++;
		else if (!BClairvoyant) {
			if (Hallucination) pline("Dude! See-through walls!");
			do_vicinity_map();
		}
		if (otmp->blessed)
			incr_itimeout(&HClairvoyant, rn1(50, 100));
		break;
	case POT_ESP:
	{
		const char *mod;

		/* KMH -- handle cursed, blessed */
		if (otmp->cursed) {
			if (HTelepat) mod = "less ";
			else {
			    unkn++;
			    mod = NULL;
			}
			HTelepat = 0;
		} else if (otmp->blessed) {
			mod = "fully ";
			incr_itimeout(&HTelepat, rn1(100, 200));
			HTelepat |= FROMOUTSIDE;
		} else {
			mod = "more ";
			incr_itimeout(&HTelepat, rn1(50, 100));
		}
		if (mod)
			You_feel(Hallucination ?
				"%sin touch with the cosmos." :
				"%smentally acute.", mod);
		see_monsters();
		break;
	}
	/* KMH, balance patch -- removed */
	/* but re-inserted by Amy */
	case POT_FIRE_RESISTANCE:
	       if(!(HFire_resistance & FROMOUTSIDE)) {
		if (Hallucination)
		   pline("You feel, like, totally cool!");
		   else You_feel("cooler.");
		   HFire_resistance += rn1(100,50 + 25 * bcsign(otmp) );
		   unkn++;
		   HFire_resistance |= FROMOUTSIDE;
		}
		break;
	case POT_RESISTANCE:
		You_feel("resistant to elemental attacks!");
		   HFire_resistance += rn1(100,50 + 25 * bcsign(otmp) );
		   HCold_resistance += rn1(100,50 + 25 * bcsign(otmp) );
		   HShock_resistance += rn1(100,50 + 25 * bcsign(otmp) );
		break;
	case POT_INVULNERABILITY:
		incr_itimeout(&Invulnerable, rn1(4, 8 + 4 * bcsign(otmp)));
		You_feel(Hallucination ?
				"like a super-duper hero!" : "invulnerable!");
		break;
	case POT_GAIN_ABILITY:
		if(otmp->cursed) {
		    pline("Ulch!  That potion tasted foul!");
		    unkn++;
		} else if (Fixed_abil || Race_if(PM_SUSTAINER) || (uarms && uarms->oartifact == ART_SYSTEMATIC_CHAOS) || (uarms && uarms->oartifact == ART_BONUS_HOLD) || (uamul && uamul->oartifact == ART_FIX_EVERYTHING) ) {
		    nothing++;
		} else {      /* If blessed, increase all; if not, try up to */
		    int itmp; /* 6 times to find one which can be increased. */
		    i = -1;		/* increment to 0 */
		    for (ii = A_MAX; ii > 0; ii--) {
			i = (/*otmp->blessed ? i + 1 :*/ rn2(A_MAX));
			/* only give "your X is already as high as it can get"
			   message on last attempt (except blessed potions) */
			itmp = (otmp->blessed || ii == 1) ? 0 : -1;
			if (adjattrib(i, 1, itmp) && !otmp->blessed)
			    break;
			if (rn2(5)) break; /* now, blessed ones no longer always increase every stat --Amy */
		    } /* but a blessed one has a chance to increase more than one stat, or increase one stat twice */
		}
		break;
	case POT_SPEED:
		if(Wounded_legs && !otmp->cursed
		   && !u.usteed	/* heal_legs() would heal steeds legs */
						) {
			heal_legs();
			unkn++;
			break;
		} /* and fall through */
	case SPE_HASTE_SELF:
		if (!Very_fast)
			You("are suddenly moving %sfaster.",
				Fast ? "" : "much ");
		else {
			Your("%s get new energy.",
				makeplural(body_part(LEG)));
			unkn++;
		}
		exercise(A_DEX, TRUE);
		if (otmp->otyp == SPE_HASTE_SELF) incr_itimeout(&HFast, rn1(10, 10 + 10 * bcsign(otmp)));
		else incr_itimeout(&HFast, rn1(10, 100 + 60 * bcsign(otmp)));
		break;
	case POT_BLINDNESS:
		if(Blind) nothing++;
		make_blinded(itimeout_incr(Blinded,
					   rn1(200, 250 - 125 * bcsign(otmp))),
			     (boolean)!Blind);
		break;

	case POT_GAIN_LEVEL:
		if (otmp->cursed) {
			unkn++;
			/* they went up a level */
			if((ledger_no(&u.uz) == 1 && u.uhave.amulet) ||
				Can_rise_up(u.ux, u.uy, &u.uz)) {
			    const char *riseup ="rise up, through the %s!";
			    /* [ALI] Special handling for quaffing potions
			     * off the floor (otmp won't be valid after
			     * we change levels otherwise).
			     */
			    if (otmp->where == OBJ_FLOOR) {
				if (otmp->quan > 1)
					(void) splitobj(otmp, 1);
				/* Make sure you're charged if in shop */
				otmp->quan++;
				useupf(otmp, 1);
				obj_extract_self(otmp);
			    }
			    if(ledger_no(&u.uz) == 1) {
			        You(riseup, ceiling(u.ux,u.uy));
				goto_level(&earth_level, FALSE, FALSE, FALSE);
				} else {
				/* Skipping levels during the ascension run is a cheap strategy. --Amy */
			        register int newlev = (u.uhave.amulet || (u.uevent.udemigod && rn2(2)) ) ? depth(&u.uz)+1 : depth(&u.uz)-1;
				d_level newlevel;

				/* But using the new system to cheat the necessity to invoke would be even cheaper. */
				if(ledger_no(&u.uz) == 99 && u.uevent.udemigod && !u.uevent.invoked) {
				    pline("You crash into the floor.");
					nomul(-rnd(10), "lying on the floor, unable to get up", TRUE);
					nomovemsg = "You finally get up again.";
				    break;
				}

				get_level(&newlevel, newlev);
				if(on_level(&newlevel, &u.uz)) {
				    pline("It tasted bad.");
				    break;
				} else if (newlev < ledger_no(&u.uz)) You(riseup, ceiling(u.ux,u.uy));
				else pline("You slide downwards...");
				goto_level(&newlevel, FALSE, FALSE, FALSE);
				/* Don't complain. Instead, be glad that the obnoxious mysterious force doesn't exist! */
			    }
			}
			else You("have an uneasy feeling.");
			break;
		}
		pluslvl(FALSE);
		if (otmp->blessed)
			/* blessed potions place you at a random spot in the
			 * middle of the new level instead of the low point
			 */
			u.uexp = rndexp(TRUE);

		break;
	case POT_DOWN_LEVEL:
		if (otmp->cursed) {
			unkn++;
			/* they went up a level */
			if((ledger_no(&u.uz) == 1 && u.uhave.amulet) ||
				Can_rise_up(u.ux, u.uy, &u.uz)) {
			    const char *riseup ="rise up, through the %s!";
			    /* [ALI] Special handling for quaffing potions
			     * off the floor (otmp won't be valid after
			     * we change levels otherwise).
			     */
			    if (otmp->where == OBJ_FLOOR) {
				if (otmp->quan > 1)
					(void) splitobj(otmp, 1);
				/* Make sure you're charged if in shop */
				otmp->quan++;
				useupf(otmp, 1);
				obj_extract_self(otmp);
			    }
			    if(ledger_no(&u.uz) == 1) {
			        You(riseup, ceiling(u.ux,u.uy));
				goto_level(&earth_level, FALSE, FALSE, FALSE);
				} else {
				/* Skipping levels during the ascension run is a cheap strategy. --Amy */
			        register int newlev = depth(&u.uz)+1;
				d_level newlevel;

				/* But using the new system to cheat the necessity to invoke would be even cheaper. */
				if(ledger_no(&u.uz) == 99) {
				    pline("You crash into the floor.");
					nomul(-rnd(10), "lying on the floor, unable to get up", TRUE);
					nomovemsg = "You finally get up again.";
				    break;
				}

				get_level(&newlevel, newlev);
				if(on_level(&newlevel, &u.uz)) {
				    pline("It tasted bad.");
				    break;
				}
				else pline("You slide downwards...");
				goto_level(&newlevel, FALSE, FALSE, FALSE);
			    }
			}
			else You("have an uneasy feeling.");
			break;
		}
		if (Drain_resistance) {
		    You_feel("rejuvenating momentarily.");
		} else {
		    You("restore youth!");
		    losexp("return to the state before being born", FALSE, TRUE);
		}

		break;
	case POT_HEALING:
		You_feel("better.");
		healup(d(5,6) + rnz(u.ulevel) + 5 * bcsign(otmp),
		       otmp->blessed ? 2 : !otmp->cursed ? 1 : 0, 1+1*!!otmp->blessed, !otmp->cursed);
		exercise(A_CON, TRUE);
		break;
	case POT_EXTRA_HEALING:
		You_feel("much better.");
		healup(d(6,8) + rnz(u.ulevel) + 5 * bcsign(otmp),
		       otmp->blessed ? 5 : !otmp->cursed ? 2 : 0,
		       !otmp->cursed, TRUE);
		(void) make_hallucinated(0L,TRUE,0L);
		exercise(A_CON, TRUE);
		exercise(A_STR, TRUE);
		break;
	case POT_FULL_HEALING:
		You_feel("completely healed.");
		healup(400 + rnz(u.ulevel), 4+4*bcsign(otmp), !otmp->cursed, TRUE);
		/* Restore one lost level if blessed */
		if (otmp->blessed && u.ulevel < u.ulevelmax) {
		    /* when multiple levels have been lost, drinking
		       multiple potions will only get half of them back */
		    u.ulevelmax -= 1;
		    pluslvl(FALSE);
		}
		(void) make_hallucinated(0L,TRUE,0L);
		exercise(A_STR, TRUE);
		exercise(A_CON, TRUE);
		break;
	case POT_CURE_WOUNDS:
		You_feel("better.");
		healup(d(5,6) + rnz(u.ulevel) + 5 * bcsign(otmp), 0, 0, 0);
		exercise(A_CON, TRUE);
		break;
	case POT_CURE_SERIOUS_WOUNDS:
		You_feel("much better.");
		healup(d(6,8) + rnz(u.ulevel) + 5 * bcsign(otmp), 0, 0, 0);
		exercise(A_CON, TRUE);
		exercise(A_STR, TRUE);
		break;
	case POT_CURE_CRITICAL_WOUNDS:
		You_feel("completely healed.");
		healup(400 + rnz(u.ulevel),  0, 0, 0);
		exercise(A_STR, TRUE);
		exercise(A_CON, TRUE);
		break;
	case POT_LEVITATION:
	case SPE_LEVITATION:
		if (otmp->cursed) HLevitation &= ~I_SPECIAL;
		if(!Levitation) {
			/* kludge to ensure proper operation of float_up() */
			HLevitation = 1;
			float_up();
			/* reverse kludge */
			HLevitation = 0;
			if (otmp->cursed && !Is_waterlevel(&u.uz)) {
	if((u.ux != xupstair || u.uy != yupstair)
	   && (u.ux != sstairs.sx || u.uy != sstairs.sy || !sstairs.up)
	   && (!xupladder || u.ux != xupladder || u.uy != yupladder)
	) {
					You("hit your %s on the %s.",
						body_part(HEAD),
						ceiling(u.ux,u.uy));
					losehp(uarmh ? 1 : rnd(10),
						"colliding with the ceiling",
						KILLED_BY);
				} else (void) doup();
			}
		} else
			nothing++;
		if (otmp->blessed) {
		    incr_itimeout(&HLevitation, rn1(50,(otmp->otyp == SPE_LEVITATION) ? 50 : 250));
		    HLevitation |= I_SPECIAL;
		} else incr_itimeout(&HLevitation, rn1((otmp->otyp == SPE_LEVITATION) ? 30 : 140,10));
		spoteffects(FALSE);	/* for sinks */
		break;
	case POT_GAIN_ENERGY:			/* M. Stephenson */
		{       register int num , num2;
			if(otmp->cursed)
			    You_feel("lackluster.");
			else
			    pline("Magical energies course through your body.");
			num = rnd(25) + rnz(u.ulevel) + 5 * otmp->blessed + 10;                        
			num2 = rnd(2) + 2 * otmp->blessed + 1;
			u.uenmax += (otmp->cursed) ? -num2 : num2;
			u.uen += (otmp->cursed) ? -num : num;
			if(u.uenmax <= 0) u.uenmax = 0;
			if(u.uen <= 0) u.uen = 0;
			if(u.uen > u.uenmax) {
				u.uenmax += ((u.uen - u.uenmax) / 2);
				u.uen = u.uenmax;
			}
			flags.botl = 1;
			exercise(A_WIS, TRUE);
		}
		break;
	case POT_GAIN_HEALTH:			/* Amy */
		{       register int num , num2;
			if(otmp->cursed)
			    You_feel("drained.");
			else
			    pline("The essence of life flows through your body.");
			num = rnd(25) + 5 * otmp->blessed + 10;                        
			num2 = rnd(2) + 2 * otmp->blessed + 1;
			u.uhpmax += (otmp->cursed) ? -num2 : num2;
			u.uhp += (otmp->cursed) ? -num : num;
			if(u.uhpmax < 1) u.uhpmax = 1;
			if(u.uhp <= 0) u.uhp = 0;
			if(u.uhp > u.uhpmax) {
				u.uhpmax += ((u.uhp - u.uhpmax) / 2);
				u.uhp = u.uhpmax;
			}
			flags.botl = 1;
			exercise(A_WIS, TRUE);
		}
		break;
	case POT_BANISHING_FEAR:
		if (otmp->oartifact == ART_CURSED_PARTS) {
			u.uprops[ITEMCURSING].intrinsic |= FROMOUTSIDE;
			u.uprops[FEAR_RES].intrinsic |= FROMOUTSIDE;
			pline("You worry about cursed items, but you're not afraid at all anymore.");
		}

	case SPE_BANISHING_FEAR:
		{
			if(otmp->cursed) {
				make_feared(HFeared + rnd(100 + (monster_difficulty() * 5) ),TRUE);
			} else {
				if (HFeared) make_feared(0L, TRUE);
				if (otmp->blessed) {
					incr_itimeout(&HFear_resistance, rnd(250) );
					if (Fear_resistance) You_feel("more resistant to fear!");
				}
			}
		}
		break;
	case POT_OIL:				/* P. Winner */
		{
			boolean good_for_you = FALSE;

			if (otmp->lamplit) {
			    if (likes_fire(youmonst.data)) {
				pline("Ahh, a refreshing drink.");
				good_for_you = TRUE;
			    } else {
				You("burn your %s.", body_part(FACE));
				losehp(d(Fire_resistance ? 1 : 3, 4),
				       "burning potion of oil", KILLED_BY_AN);
			    }
			} else if(otmp->cursed)
			    pline("This tastes like castor oil.");
			else
			    pline("That was smooth!");
			exercise(A_WIS, good_for_you);
		}
		if (Race_if(PM_CLOCKWORK_AUTOMATON)) u.uhunger += 500;
		if (Race_if(PM_CLOCKWORK_AUTOMATON)) exercise(A_WIS, TRUE);
		break;
	case POT_ACID:
		if (Acid_resistance)
			/* Not necessarily a creature who _likes_ acid */
			pline("This tastes %s.", Hallucination ? "tangy" : "sour");
		else {
			pline("This burns%s!", otmp->blessed ? " a little" :
					otmp->cursed ? " a lot" : " like acid");
			losehp(d(otmp->cursed ? 2 : 1, otmp->blessed ? 4 : 8),
					"potion of acid", KILLED_BY_AN);
			exercise(A_CON, FALSE);
		}
		if (Stoned) fix_petrification();
		unkn++; /* holy/unholy water can burn like acid too */
		break;
	case POT_POLYMORPH:

		if (otmp->oartifact == ART_PLANECHANGERS) {
			HPolymorph |= FROMOUTSIDE;
			pline("Okay, now you got the intrinsic polymorphitis you wanted.");
		}

	case POT_MUTATION:
		You_feel("a little %s.", Hallucination ? "normal" : "strange");
		if (!Unchanging) polyself(FALSE);
		break;
	case POT_SALT_WATER:

		if (Race_if(PM_ELONA_SNAIL) && !Upolyd) {
			pline("It's salt! you start to melt."); /* capitalization is NOT an error --Amy */
			losehp(((u.uhpmax < 10) ? 9999 : (u.uhpmax - rnd(10))), "a potion of salt solution", KILLED_BY);
			break;
		}

		pline("Yuck... this is salt water!");
		exercise(A_WIS, FALSE);
		make_vomiting(Vomiting + rnd(10) + 5,TRUE);
		if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
		break;
	case POT_BLOOD:
	case POT_VAMPIRE_BLOOD:
		unkn++;
		u.uconduct.unvegan++;

		if (Role_if(PM_BLEEDER) && !otmp->cursed) {

		pline(Hallucination ? "It's the essence of life itself! Damn, you absolutely need more of this stuff! Where's a dealer when you need one?" : "Your tortured body experiences a strange sense of joy as your lips touch the warm red liquid.");
		if (otmp->otyp == POT_VAMPIRE_BLOOD) u.uhpmax += 1;
		u.uhp = u.uhpmax;

		}

		if (Race_if(PM_HEMOPHAGE) && !otmp->cursed) {

		pline(Hallucination ? "It's the essence of life itself! Damn, you absolutely need more of this stuff! Where's a dealer when you need one?" : "Your tortured body experiences a strange sense of joy as your lips touch the warm red liquid.");
		if (otmp->otyp == POT_VAMPIRE_BLOOD) u.uhpmax += 1;
		u.uhp = u.uhpmax;

		}

		if (maybe_polyd(is_vampire(youmonst.data), Race_if(PM_VAMPIRE)) || Role_if(PM_GOFF) ) {
		    violated_vegetarian();
		    if (otmp->cursed)
			pline("Yecch!  This %s.", Hallucination ?
			"liquid could do with a good stir" : "blood has congealed");
		    else pline(Hallucination ?
		      "The %s liquid stirs memories of home." :
		      "The %s blood tastes delicious.",
			  otmp->odiluted ? "watery" : "thick");
		    if (!otmp->cursed)
			lesshungry((otmp->odiluted ? 1 : 2) *
			  (otmp->otyp == POT_VAMPIRE_BLOOD ? 400 :
			  otmp->blessed ? 15 : 10));
		    if (otmp->otyp == POT_VAMPIRE_BLOOD && otmp->blessed) {
			int num = newhp();
			if (Upolyd) {
			    u.mhmax += num;
			    u.mh += num;
			} else {
			    u.uhpmax += num;
			    u.uhp += num;
			}
		    }
		} else if (otmp->otyp == POT_VAMPIRE_BLOOD) {
		    /* [CWC] fix conducts for potions of (vampire) blood -
		       doesn't use violated_vegetarian() to prevent
		       duplicated "you feel guilty" messages */
		    u.uconduct.unvegetarian++;
		    if (u.ualign.type == A_LAWFUL || Role_if(PM_MONK)) {
			You_feel("%sguilty about drinking such a vile liquid.",
				Role_if(PM_MONK) ? "especially " : "");
			u.ugangr++;
			adjalign(-50);
		    } else if (u.ualign.type == A_NEUTRAL)
			adjalign(-3);
		    exercise(A_CON, FALSE);
		    if (!Unchanging && polymon(PM_VAMPIRE))
			u.mtimedone = 0;	/* "Permament" change */
		} else {
		    violated_vegetarian();
		    pline("Ugh.  That was vile.");
		    make_vomiting(Vomiting+d(10,8), TRUE);
			if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
		}
		break;

	case POT_CYANIDE:
		make_sick(Sick ? Sick/2L + 1L : 20, "cyanide potion", TRUE, SICK_VOMITABLE);
		losestr(rnd(10));
		pline(Hallucination ? "This tastes a little bitter; maybe it's some sort of medicine?" : "CN(-) + HCl <==> HCN + Cl(-) ");
			losehp(d(otmp->cursed ? 4 : 2, otmp->blessed ? 8 : 16),
					"drinking cyanide", KILLED_BY);
		break;
	case POT_RADIUM:
		pline(Hallucination ? "For some reason, that potion tastes... orange. Yes, the color orange, not the fruit." : "This was radioactive radium!");
		make_sick(Sick ? Sick/2L + 1L : 50,"radium potion", TRUE, SICK_VOMITABLE);
		break;
	case POT_JOLT_COLA:
		You("are jolted back to your senses.");
		if (otmp->cursed) {
			You_feel("bad.");
			u.uhp -= rn2(10);
			if (u.uhp < 1) u.uhp = 1;	/* can't kill you */
		} else {
			if (Hallucination) make_hallucinated(0L,FALSE,0L);
			if (otmp->blessed && !rn2(10)) {
				pluslvl(FALSE);
			} else {
				u.uhp += 5;
				if (u.uhp > u.uhpmax) {
					if (otmp->blessed) u.uhpmax++;
					u.uhp = u.uhpmax;
				}
			}
		}
		break;

	case POT_EXTREME_POWER:
	{
		int x;

		x = rn2(otmp->blessed ? 20 : 10) + 1;
		if (otmp->cursed) {
			unkn++;
			You("have a bad taste in your mouth.");
			if (x >= u.uhp) x = u.uhp-1;
			losehp(x,"",0);		/* can't kill you */
			u.uhpmax -= x;
			break;
		}
		You_feel("%spowerful!",otmp->blessed ? "very " : "");
		u.uhp += x;
		u.uhpmax += x;
	} break;

	case POT_RECOVERY: 
		if (otmp->cursed) {
			unkn++;
			You("decide to recover yourself.");
			pline("Nothing happens, however.");
			break;
		}	
		if (otmp->blessed) u.uhpmax += rn2(5);
		u.uhp = u.uhpmax;
		You_feel("much, much better.");
		break;


	case POT_HEROISM: {
		/* a total overhaul ... */
		int time;

		if (otmp->cursed) {
			You_feel("cowardly.");

		    set_itimeout(&Invulnerable, 0);
			break;
		}
		You("are surrounded by a strong shimmering aura.");
		time = 10 + rn2(otmp->blessed ? 20 : 10);
		incr_itimeout(&Invulnerable, time);
		incr_itimeout(&HFast, time);
		incr_itimeout(&HSee_invisible, time);
		if (!rnl(3)) make_blinded(Blinded+time,TRUE);	
		u.uhpmax += rn2(5);
		u.uhp = u.uhpmax;
		} break;

	case POT_PORTER:
		if (Hallucination)
			You_feel("like hopping around!");
		else
			You_feel("very jumpy.");
		incr_itimeout(&HTeleportation, rnd(500));
		if (otmp->cursed) {

			if (HTeleport_control & INTRINSIC) {
				HTeleport_control &= ~INTRINSIC;
				You_feel("unable to control where you're going.");
			}
			if (HTeleport_control & TIMEOUT) {
				HTeleport_control &= ~TIMEOUT;
				You_feel("unable to control where you're going.");
			}

		}

		break;

	case POT_KEEN_MEMORY:
		if (Hallucination)
			You_feel("like remembering everything that ever happened to you!");
		else
			pline("Your memory keens.");
		incr_itimeout(&HKeen_memory, rnd(500 + 250 * bcsign(otmp) ));

		break;

	case POT_NIGHT_VISION:
		if (Hallucination)
			pline("Everything is visible! Whoa! Look at all the stuff!");
		else
			pline("Your vision range increases.");
		incr_itimeout(&HSight_bonus, rnd(500 + 250 * bcsign(otmp) ));
		vision_full_recalc = 1;

		break;

 	case POT_PAN_GALACTIC_GARGLE_BLASTE:
		You_feel("like having your brain smashed out by a slice of lemon wrapped");
		pline("around a large gold brick.");
		if (otmp->cursed || !rn2(4)) {
			adjattrib(A_INT,-1,-1);
		}
		make_confused(itimeout_incr(HConfusion, d(6,7)), FALSE);	/* 6d7 is max. 42 */

		if (otmp->blessed || !rn2(4) || u.ulycn != -1) {
			if (u.ulycn != -1 && !Race_if(PM_HUMAN_WEREWOLF) && !Role_if(PM_LUNATIC) && !Race_if(PM_AK_THIEF_IS_DEAD_) ) {
				You_feel("purified.");
				if (!Unchanging && (u.umonnum == u.ulycn) ) {
			    	    if (!Race_if(PM_UNGENOMOLD)) rehumanize();
					else polyself(FALSE);
				}
				u.ulycn = -1;
			}
			You_feel("remembered of %s.",
			Hallucination ? "Zaphod Breeblebrox"
				      : "very, very strong liquor.");
			if (u.uhp < u.uhpmax) u.uhp++;
			pline("Strangely, you feel better that before.");
		}
		break;	
				
	case POT_TERCES_DLU:

		pline(fauxmessage());
		if (!rn2(3)) pline(fauxmessage());

		break;

	case POT_WONDER:

		pline("ide by 0");

		break;

	case POT_HIDING:
	case POT_DECOY_MAKING:

		if(Confusion || Hallucination || metallivorous(youmonst.data)) {
		    pline("This tinfoil is gnarly!");
		    u.uhunger += (otmp->odiluted ? 5 : 10) * (2 + bcsign(otmp));
		    newuhs(FALSE);
		} else {
		    pline("Ecch - It includes tinfoil!");
		    make_vomiting(Vomiting + d(5, (2-bcsign(otmp))), TRUE);
			if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
		}

		break;

	default:
		impossible("What a funny potion! (%u)", otmp->otyp);
		return(0);
	}
	return(-1);
}

void
healup(nhp, nxtra, curesick, cureblind)
	int nhp, nxtra;
	register boolean curesick, cureblind;
{

	if (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "nurse cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "medsestra plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "hamshira plash") )) nhp *= 2;
	if (uarmh && uarmh->oartifact == ART_SEXYNESS_HAS_A_NAME) {
		nhp *= 2;
		if (Role_if(PM_HEALER)) nhp *= 2;
	}
	if (uwep && uwep->oartifact == ART_HEALHEALHEALHEAL) {
		nhp *= 2;
		if (Role_if(PM_HEALER)) nhp *= 2;
	}
	if (uarmf && uarmf->oartifact == ART_KATIE_MELUA_S_FLEECINESS) {
		nhp *= 2;
		if (Role_if(PM_HEALER)) nhp *= 2;
	}
	if (RngeNursery) nhp *= 2;

	if (nhp) {
		if (Upolyd) {
			u.mh += nhp;
			if(u.mh > u.mhmax) u.mh = (u.mhmax += nxtra);
			else if (!rn2(2)) u.mhmax += nxtra;
		} else {
			u.uhp += nhp;
			if(u.uhp > u.uhpmax) u.uhp = (u.uhpmax += nxtra);
			else if (!rn2(2)) u.uhpmax += nxtra;
		}
	}
	if(cureblind)	make_blinded(0L,TRUE);
	if(curesick)	make_sick(0L, (char *) 0, TRUE, SICK_ALL);
	exercise(A_CON, TRUE); /* addition by Amy */
	flags.botl = 1;
	return;
}

void
healup_mon(mtmp, nhp, nxtra, curesick, cureblind)
	struct monst *mtmp;
	int nhp, nxtra;
	register boolean curesick, cureblind;
{

	if (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "nurse cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "medsestra plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "hamshira plash") ) ) nhp *= 2;
	if (uarmh && uarmh->oartifact == ART_SEXYNESS_HAS_A_NAME) {
		nhp *= 2;
		if (Role_if(PM_HEALER)) nhp *= 2;
	}
	if (uwep && uwep->oartifact == ART_HEALHEALHEALHEAL) {
		nhp *= 2;
		if (Role_if(PM_HEALER)) nhp *= 2;
	}
	if (uarmf && uarmf->oartifact == ART_KATIE_MELUA_S_FLEECINESS) {
		nhp *= 2;
		if (Role_if(PM_HEALER)) nhp *= 2;
	}
	if (RngeNursery) nhp *= 2;

	if (nhp) {
		mtmp->mhp += nhp;
		if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = (mtmp->mhpmax += nxtra);
	}
#if 0
	if(cureblind) ; /* NOT DONE YET */
	if(curesick)  ; /* NOT DONE YET */
#endif 
	return;
}

void
strange_feeling(obj,txt)
register struct obj *obj;
register const char *txt;
{
	if (flags.beginner || !txt)
		You("have a %s feeling for a moment, then it passes.",
		Hallucination ? "normal" : "strange");
	else
		pline(txt);

	if(!obj)	/* e.g., crystal ball finds no traps */
		return;

	if(obj->dknown && !objects[obj->otyp].oc_name_known &&
						!objects[obj->otyp].oc_uname)
		docall(obj);
	if (carried(obj) /*&& !(obj->oartifact)*/) useup(obj);
	else /*if (!(obj->oartifact))*/ useupf(obj, 1L);
}

const char *bottlenames[] = {
	"bottle", "phial", "flagon", "carafe", "flask", "jar", "vial"
};

const char *
bottlename()
{
	return bottlenames[rn2(SIZE(bottlenames))];
}

/* WAC -- monsters can throw potions around too! */
void
potionhit(mon, obj, your_fault)
register struct monst *mon; /* Monster that got hit */
register struct obj *obj;
boolean your_fault;
{
	register const char *botlnam = bottlename();
	boolean isyou = (mon == &youmonst);
	int distance;

	if(isyou) {
		distance = 0;
		pline_The("%s crashes on your %s and breaks into shards.",
			botlnam, body_part(HEAD));
		losehp(rnd(2), "thrown potion", KILLED_BY_AN);
	} else {
		distance = distu(mon->mx,mon->my);
		if (!cansee(mon->mx,mon->my)) pline("Crash!");
		else {
		    char *mnam = mon_nam(mon);
		    char buf[BUFSZ];

		    if(has_head(mon->data)) {
			sprintf(buf, "%s %s",
				s_suffix(mnam),
				(notonhead ? "body" : "head"));
		    } else {
			strcpy(buf, mnam);
		    }
		    pline_The("%s crashes on %s and breaks into shards.",
			   botlnam, buf);
		}
		if(rn2(5) && mon->mhp > 1)
			mon->mhp--;
	}

	/* oil doesn't instantly evaporate */
	if (obj->otyp != POT_OIL && cansee(mon->mx,mon->my))
		pline("%s.", Tobjnam(obj, "evaporate"));

    if (isyou) {
	switch (obj->otyp) {
	case POT_OIL:
		if (obj->lamplit)
		    splatter_burning_oil(u.ux, u.uy);
		break;
	case POT_POLYMORPH:
	case POT_MUTATION:
		You_feel("a little %s.", Hallucination ? "normal" : "strange");
		if (!Unchanging && !Antimagic) polyself(FALSE);
		break;
	case POT_ACID:
		if (!Acid_resistance) {
		    pline("This burns%s!", obj->blessed ? " a little" :
				    obj->cursed ? " a lot" : "");
		    losehp(d(obj->cursed ? 2 : 1, obj->blessed ? 4 : 8),
				    "potion of acid", KILLED_BY_AN);
		/* these things are way too weak and don't scale with monster difficulty at all! Let's change this! --Amy */
			if (obj->blessed) losehp(rnd( (monster_difficulty() + 3) / 4), "potion of acid", KILLED_BY_AN);
			else if (obj->cursed) losehp(rnd( (monster_difficulty() + 3) / 2), "potion of acid", KILLED_BY_AN);
			else losehp(rnd( (monster_difficulty() + 3) / 3), "potion of acid", KILLED_BY_AN);

		}
		if (Stoned) fix_petrification();
		break;
	case POT_AMNESIA:
		/* Uh-oh! */
		if (uarmh && is_helmet(uarmh) && 
			rn2(10 - (uarmh->cursed? 8 : 0)))
		    get_wet(uarmh, TRUE);
		break;
	}
    } else {
	boolean angermon = TRUE;

	if (!your_fault) angermon = FALSE;
	switch (obj->otyp) {
	case POT_HEALING:
 do_healing:
		if (mon->data == &mons[PM_PESTILENCE]) goto do_illness;
		angermon = FALSE;
		if (canseemon(mon))
			pline("%s looks better.", Monnam(mon));
		healup_mon(mon, d(5,6) + rnz(u.ulevel) + 5 * bcsign(obj),
			!obj->cursed ? 1 : 0, 1+1*!!obj->blessed, !obj->cursed);
		break;
	case POT_EXTRA_HEALING:
		if (mon->data == &mons[PM_PESTILENCE]) goto do_illness;
		angermon = FALSE;
		if (canseemon(mon))
			pline("%s looks much better.", Monnam(mon));
		healup_mon(mon, d(6,8) + rnz(u.ulevel) + 5 * bcsign(obj),
			obj->blessed ? 5 : !obj->cursed ? 2 : 0,
			!obj->cursed, TRUE);
		break;
	case POT_FULL_HEALING:
		if (mon->data == &mons[PM_PESTILENCE]) goto do_illness;
		/*FALLTHRU*/
	case POT_RESTORE_ABILITY:
	case POT_GAIN_ABILITY:
		angermon = FALSE;
		    if (canseemon(mon))
			pline("%s looks sound and hale again.", Monnam(mon));
		healup_mon(mon, 400 + rnz(u.ulevel), 5+5*!!(obj->blessed), !(obj->cursed), 1);
		break;
	case POT_CURE_WOUNDS:
		if (mon->data == &mons[PM_PESTILENCE]) goto do_illness;
		angermon = FALSE;
		if (canseemon(mon)) pline("%s looks better.", Monnam(mon));
		healup_mon(mon, d(5,6) + rnz(u.ulevel) + 5 * bcsign(obj), 0, 0, 0);
		break;
	case POT_CURE_SERIOUS_WOUNDS:
		if (mon->data == &mons[PM_PESTILENCE]) goto do_illness;
		angermon = FALSE;
		if (canseemon(mon)) pline("%s looks much better.", Monnam(mon));
		healup_mon(mon, d(6,8) + rnz(u.ulevel) + 5 * bcsign(obj), 0, 0, 0);
		break;
	case POT_CURE_CRITICAL_WOUNDS:
		if (mon->data == &mons[PM_PESTILENCE]) goto do_illness;
		angermon = FALSE;
		if (canseemon(mon)) pline("%s looks sound and hale again.", Monnam(mon));
		healup_mon(mon, 400 + rnz(u.ulevel),  0, 0, 0);
		break;
	case POT_SICKNESS:
	case POT_POISON:
		if (mon->data == &mons[PM_PESTILENCE]) goto do_healing;
		if (dmgtype(mon->data, AD_DISE) ||
			   dmgtype(mon->data, AD_PEST) || /* won't happen, see prior goto */
			   resists_poison(mon)) {
		    if (canseemon(mon))
			pline("%s looks unharmed.", Monnam(mon));
		    break;
		}
 do_illness:
		if (canseemon(mon))
		    pline("%s looks rather ill.", Monnam(mon));
		if((mon->mhpmax > 3) && !resist(mon, POTION_CLASS, 0, NOTELL))
			mon->mhpmax /= 2;
		if((mon->mhp > 2) && !resist(mon, POTION_CLASS, 0, NOTELL))
			mon->mhp /= 2;
		if (mon->mhp > mon->mhpmax) mon->mhp = mon->mhpmax;
		break;
	case POT_CYANIDE:
		if (!resist(mon,POTION_CLASS,0,NOTELL)) {
			if (canseemon(mon)) {
				pline("%s looks deathly sick.", Monnam(mon));
			}
			if (mon->mhp < 10) {
				killed(mon);
			} else {
				mon->mhp /= 5;
				if (mon->mhp < 1) killed(mon);
			}

		}
		break;
	case POT_RADIUM:
	case POT_SLIME:	/* too lazy to code something else :D --Amy */
		if (!resist(mon,POTION_CLASS,0,NOTELL)) {
			if (canseemon(mon)) pline("%s looks very sick.",Monnam(mon));
			mon->mhp /= 4;
			if (mon->mhp < 1) killed(mon);
		}
		break;
	case POT_STUNNING:
	case POT_NUMBNESS: /* both of these will stun the monster --Amy */
		if(!resist(mon, POTION_CLASS, 0, NOTELL))  {
			mon->mstun = TRUE;
			if (canseemon(mon)) pline("%s trembles.",Monnam(mon));
		}
		break;

	case POT_CANCELLATION:

		(void) cancel_monst(mon, obj, TRUE, TRUE, FALSE);

		break;

	case POT_ICE: /* there's no frozen monster state, so we'll just slow down the monster --Amy */
		if(!resist(mon, POTION_CLASS, 0, NOTELL))  {
			mon_adjust_speed(mon, -2, (struct obj *)0 );
			if (canseemon(mon)) pline("%s slows to a crawl.",Monnam(mon));
		}
		break;

	case POT_DIMNESS:
	case POT_FEAR: /* monster flees --Amy */
		if(!is_undead(mon->data) && (!mon->egotype_undead) && !resist(mon, POTION_CLASS, 0, NOTELL))  {
		     monflee(mon, rnd(10), FALSE, TRUE);
			if (canseemon(mon)) pline("%s is panicking!",Monnam(mon));
		}
		break;

	case POT_FIRE: /* there's no burned monster state, so we'll take off some max HP instead --Amy */
		if(!resist(mon, POTION_CLASS, 0, NOTELL))  {
		mon->mhp -= rnd(3);
		mon->mhpmax -= rnd(3);
		if (mon->mhp > mon->mhpmax) mon->mhp = mon->mhpmax;
		if (canseemon(mon)) pline("%s is burned a little.",Monnam(mon));
		if (mon->mhp < 1 || mon->mhpmax < 1) monkilled(mon, "", AD_FIRE);;

		}

		break;

	case POT_CONFUSION:
	case POT_BOOZE:
	case POT_WINE:
		if(!resist(mon, POTION_CLASS, 0, NOTELL))  {mon->mconf = TRUE;
			if (canseemon(mon)) pline("%s is confused.",Monnam(mon));

		}
		break;
#if 0 /* NH 3.3.0 version */
	case POT_POLYMORPH:
		(void) bhitm(mon, obj);
		break;
#endif
	case POT_POLYMORPH:
	    /* [Tom] polymorph potion thrown 
	     * [max] added poor victim a chance to resist 
	     * magic resistance protects from polymorph traps, so make
	     * it guard against involuntary polymorph attacks too... 
	     */
	    if (resists_magm(mon)) {
                shieldeff(mon->mx, mon->my);
	    } else if (!resist (mon, POTION_CLASS, 0, NOTELL)) {
                mon_poly(mon, your_fault, "%s changes!");
                if (!Hallucination && canspotmon (mon))
                                makeknown (POT_POLYMORPH);
	    }
  		break;
	case POT_MUTATION:

		mon->isegotype = 1;
		switch (rnd(174)) {
			case 1:
			case 2:
			case 3: mon->egotype_thief = 1; break;
			case 4: mon->egotype_wallwalk = 1; break;
			case 5: mon->egotype_disenchant = 1; break;
			case 6:
			case 7: mon->egotype_rust = 1; break;
			case 8: 
			case 9: mon->egotype_corrosion = 1; break;
			case 10: 
			case 11: mon->egotype_decay = 1; break;
			case 12: mon->egotype_wither = 1; break;
			case 13: 
			case 14: 
			case 15: mon->egotype_grab = 1; break;
			case 16: 
			case 17: mon->egotype_flying = 1; break;
			case 18: 
			case 19: mon->egotype_hide = 1; break;
			case 20: 
			case 21: 
			case 22: mon->egotype_regeneration = 1; break;
			case 23: 
			case 24: 
			case 25: mon->egotype_undead = 1; break;
			case 26: mon->egotype_domestic = 1; break;
			case 27: mon->egotype_covetous = 1; break;
			case 28: 
			case 29: mon->egotype_avoider = 1; break;
			case 30: mon->egotype_petty = 1; break;
			case 31: mon->egotype_pokemon = 1; break;
			case 32: mon->egotype_slows = 1; break;
			case 33: mon->egotype_vampire = 1; break;
			case 34: mon->egotype_teleportself = 1; break;
			case 35: mon->egotype_teleportyou = 1; break;
			case 36: 
			case 37: mon->egotype_wrap = 1; break;
			case 38: mon->egotype_disease = 1; break;
			case 39: mon->egotype_slime = 1; break;
			case 40: 
			case 41: 
			case 42: 
			case 43: mon->egotype_engrave = 1; break;
			case 44: 
			case 45: mon->egotype_dark = 1; break;
			case 46: mon->egotype_luck = 1; break;
			case 47: 
			case 48: 
			case 49: mon->egotype_push = 1; break;
			case 50: mon->egotype_arcane = 1; break;
			case 51: mon->egotype_clerical = 1; break;
			case 52: 
			case 53: mon->egotype_armorer = 1; break;
			case 54: mon->egotype_tank = 1; break;
			case 55: 
			case 56: mon->egotype_speedster = 1; break;
			case 57: mon->egotype_racer = 1; break;
			case 58: mon->egotype_randomizer = 1; break;
			case 59: mon->egotype_blaster = 1; break;
			case 60: mon->egotype_multiplicator = 1; break;
			case 61: mon->egotype_gator = 1; break;
			case 62: mon->egotype_reflecting = 1; break;
			case 63: mon->egotype_hugger = 1; break;
			case 64: mon->egotype_mimic = 1; set_mimic_sym(mon); break;
			case 65: mon->egotype_permamimic = 1; set_mimic_sym(mon); break;
			case 66:
			case 67: mon->egotype_poisoner = 1; break;
			case 68: mon->egotype_elementalist = 1; break;
			case 69: mon->egotype_resistor = 1; break;
			case 70:
			case 71: mon->egotype_acidspiller = 1; break;
			case 72:
			case 73: mon->egotype_watcher = 1; break;
			case 74: mon->egotype_metallivore = 1; break;
			case 75: mon->egotype_lithivore = 1; break;
			case 76: mon->egotype_organivore = 1; break;
			case 77: mon->egotype_breather = 1; break;
			case 78: mon->egotype_beamer = 1; break;
			case 79:
			case 80: mon->egotype_troll = 1; break;
			case 81:
			case 82:
			case 83:
			case 84:
			case 85:
			case 86: mon->egotype_faker = 1; break;
			case 87:
			case 88:
			case 89:
			case 90: mon->egotype_farter = 1; break;
			case 91: mon->egotype_timer = 1; break;
			case 92: mon->egotype_thirster = 1; break;
			case 93: mon->egotype_watersplasher = 1; break;
			case 94: mon->egotype_cancellator = 1; break;
			case 95: mon->egotype_banisher = 1; break;
			case 96: mon->egotype_shredder = 1; break;
			case 97: mon->egotype_abductor = 1; break;
			case 98:
			case 99: mon->egotype_incrementor = 1; break;
			case 100: mon->egotype_mirrorimage = 1; break;
			case 101:
			case 102: mon->egotype_curser = 1; break;
			case 103: mon->egotype_horner = 1; break;
			case 104: mon->egotype_lasher = 1; break;
			case 105: mon->egotype_cullen = 1; break;
			case 106:
			case 107:
			case 108: mon->egotype_webber = 1; break;
			case 109: mon->egotype_itemporter = 1; break;
			case 110: mon->egotype_schizo = 1; break;
			case 111: mon->egotype_nexus = 1; break;
			case 112: mon->egotype_sounder = 1; break;
			case 113: mon->egotype_gravitator = 1; break;
			case 114: mon->egotype_inert = 1; break;
			case 115:
			case 116: mon->egotype_antimage = 1; break;
			case 117: mon->egotype_plasmon = 1; break;
			case 118:
			case 119:
			case 120: mon->egotype_weaponizer = 1; break;
			case 121: mon->egotype_engulfer = 1; break;
			case 122: mon->egotype_bomber = 1; break;
			case 123:
			case 124: mon->egotype_exploder = 1; break;
			case 125: mon->egotype_unskillor = 1; break;
			case 126: mon->egotype_blinker = 1; break;
			case 127: mon->egotype_psychic = 1; break;
			case 128: mon->egotype_abomination = 1; break;
			case 129: mon->egotype_gazer = 1; break;
			case 130: mon->egotype_seducer = 1; break;
			case 131: mon->egotype_flickerer = 1; break;
			case 132:
			case 133: mon->egotype_hitter = 1; break;
			case 134: mon->egotype_piercer = 1; break;
			case 135: mon->egotype_petshielder = 1; break;
			case 136: mon->egotype_displacer = 1; break;
			case 137: mon->egotype_lifesaver = 1; break;
			case 138: mon->egotype_venomizer = 1; break;
			case 139: mon->egotype_nastinator = 1; break;
			case 140: mon->egotype_baddie = 1; break;
			case 141: mon->egotype_dreameater = 1; break;
			case 142: mon->egotype_sludgepuddle = 1; break;
			case 143: mon->egotype_vulnerator = 1; break;
			case 144: mon->egotype_marysue = 1; break;
			case 145: mon->egotype_wallwalk = 1; break;
			case 146: mon->egotype_reflecting = 1; break;
			case 147: mon->egotype_mimic = 1; break;
			case 148: mon->egotype_permamimic = 1; break;
			case 149: mon->egotype_metallivore = 1; break;
			case 150: mon->egotype_lithivore = 1; break;
			case 151: mon->egotype_organivore = 1; break;
			case 152: mon->egotype_armorer = 1; break;
			case 153: mon->egotype_speedster = 1; break;
			case 154: mon->egotype_hitter = 1; break;
			case 155: mon->egotype_shader = 1; break;
			case 156: mon->egotype_amnesiac = 1; break;
			case 157: mon->egotype_trapmaster = 1; break;
			case 158: mon->egotype_midiplayer = 1; break;
			case 159: mon->egotype_rngabuser = 1; break;
			case 160: mon->egotype_mastercaster = 1; break;
			case 161: mon->egotype_aligner = 1; break;
			case 162: mon->egotype_sinner = 1; break;
			case 163: mon->egotype_aggravator = 1; break;
			case 164: mon->egotype_minator = 1; break;
			case 165: mon->egotype_contaminator = 1; break;
			case 166: mon->egotype_radiator = 1; break;
			case 167: mon->egotype_weeper = 1; break;
			case 168: mon->egotype_reactor = 1; break;
			case 169: mon->egotype_aligner = 1; break;
			case 170: mon->egotype_aligner = 1; break;
			case 171: mon->egotype_aggravator = 1; break;
			case 172: mon->egotype_contaminator = 1; break;
			case 173: mon->egotype_contaminator = 1; break;
			case 174: mon->egotype_contaminator = 1; break;
		}

  		break;
	case POT_INVISIBILITY:
		angermon = FALSE;
		mon_set_minvis(mon);
		break;
	case POT_SLEEPING:
		/* wakeup() doesn't rouse victims of temporary sleep */
		if (sleep_monst(mon, rnd(12), POTION_CLASS)) {
		    pline("%s falls asleep.", Monnam(mon));
		    slept_monst(mon);
		}
		break;
	case POT_PARALYSIS:

		/* monsters with paralysis attacks are immune now --Amy */
		if (mon->mcanmove && !(dmgtype(mon->data, AD_PLYS))) {
			mon->mcanmove = 0;
			/* really should be rnd(5) for consistency with players
			 * breathing potions, but...
			 */
			mon->mfrozen = rnd(25);
		}
		break;
	case POT_SPEED:
		angermon = FALSE;
		mon_adjust_speed(mon, 1, obj);
		break;
	case POT_BLINDNESS:
		if(haseyes(mon->data)) {
		    register int btmp = 64 + rn2(32) +
			rn2(32) * !resist(mon, POTION_CLASS, 0, NOTELL);
		    btmp += mon->mblinded;
		    mon->mblinded = min(btmp,127);
		    mon->mcansee = 0;
		}
		break;
	case POT_WATER:
		if (is_undead(mon->data) || is_demon(mon->data) ||
			is_were(mon->data)) {
		    if (obj->blessed) {
			pline("%s %s in pain!", Monnam(mon),
			      is_silent(mon->data) ? "writhes" : "shrieks");
			mon->mhp -= d(2,6);
			if (mon->mhp < 1) {
			    if (your_fault)
				killed(mon);
			    else
				monkilled(mon, "", AD_ACID);
			}
			else if (is_were(mon->data) && !is_human(mon->data))
			    new_were(mon);	/* revert to human */
		    } else if (obj->cursed) {
			angermon = FALSE;
			if (canseemon(mon))
			    pline("%s looks healthier.", Monnam(mon));
			mon->mhp += d(2,6);
			if (mon->mhp > mon->mhpmax) mon->mhp = mon->mhpmax;
			if (is_were(mon->data) && is_human(mon->data) &&
				!Protection_from_shape_changers)
			    new_were(mon);	/* transform into beast */
		    }
		} else if(mon->data == &mons[PM_GREMLIN]) {
		    angermon = FALSE;
		    (void)split_mon(mon, (struct monst *)0);
		} else if(mon->data == &mons[PM_FLAMING_SPHERE] ||
			mon->data == &mons[PM_IRON_GOLEM]) {
		    if (canseemon(mon))
			pline("%s %s.", Monnam(mon),
				mon->data == &mons[PM_IRON_GOLEM] ?
				"rusts" : "flickers");
		    mon->mhp -= d(1,6);
		    if (mon->mhp < 1) {
			if (your_fault)
			    killed(mon);
			else
			    monkilled(mon, "", AD_ACID);
		    }
		}
		break;
	case POT_AMNESIA:
		if (mon->mfrenzied) mon->mfrenzied = 0;
		switch (monsndx(mon->data)) {
		case PM_GREMLIN:
		    /* Gremlins multiply... */
		    mon->mtame = FALSE;	
		    (void)split_mon(mon, (struct monst *)0);
		    break;
		case PM_FLAMING_SPHERE:
		case PM_IRON_GOLEM:
		    if (canseemon(mon)) pline("%s %s.", Monnam(mon),
			    monsndx(mon->data) == PM_IRON_GOLEM ?
			    "rusts" : "flickers");
		    mon->mhp -= d(1,6);
		    if (mon->mhp < 1)
			if (your_fault)
			    killed(mon);
			else
			    monkilled(mon, "", AD_ACID);
		    else
			mon->mtame = FALSE;	
		    break;
		case PM_WIZARD_OF_YENDOR:
		    if (your_fault) {
			if (canseemon(mon)) 
			    pline("%s laughs at you!", Monnam(mon));
			forget(1);
		    }
		    break;

		/* Adding some actually useful effects to all that flavor talk. --Amy */

		case PM_MEDUSA:
		    if (canseemon(mon))
			pline("%s looks like %s's having a bad hair day!", 
					Monnam(mon), mhe(mon));
		    mon->mcan = TRUE;
		    break;
		case PM_CROESUS:
		    if (canseemon(mon))
		        pline("%s says: 'My gold! I must count my gold!'", 
					Monnam(mon));
		    mon->mconf = TRUE;
		    break;
		case PM_DEATH:
 		    if (canseemon(mon))
		        pline("%s pauses, then looks at you thoughtfully!", 
					Monnam(mon));
		    mon->m_lev--;
		    break;
		case PM_FAMINE:
		    if (canseemon(mon))
		        pline("%s looks unusually hungry!", Monnam(mon));
		    mon->m_lev--;
		    break;
		case PM_PESTILENCE:
		    if (canseemon(mon))
		        pline("%s looks unusually well!", Monnam(mon));
		    mon->m_lev--;
		    break;
		case PM_FRUSTRATION:
		    if (canseemon(mon))
		        pline("%s looks like something terrible happened to him!", Monnam(mon));
		    mon->m_lev--;
		    break;
		default:
		    if (mon->data->msound == MS_NEMESIS && canseemon(mon)
				    && your_fault) {
			pline("%s curses your ancestors!", Monnam(mon));
		      mon->m_lev--;
		      mon->mstun = TRUE;
		    } else if (mon->isshk) {
			angermon = FALSE;
			if (canseemon(mon))
			    pline("%s looks at you curiously!", 
					    Monnam(mon));
			make_happy_shk(mon, FALSE);
		    } else if (!is_covetous(mon->data) && !rn2(4) &&
				    !resist(mon, POTION_CLASS, 0, 0)) {
			angermon = FALSE;
			if (canseemon(mon)) {
			    if (mon->msleeping) {
				wakeup(mon);
				pline("%s wakes up looking bewildered!", 
						Monnam(mon));
			    } else
				pline("%s looks bewildered!", Monnam(mon));
			    mon->mpeaceful = TRUE;
			    mon->mtame = FALSE;	
			}
		    }
		    break;
		}
		break;
	case POT_OIL:
		if (obj->lamplit)
			splatter_burning_oil(mon->mx, mon->my);
		break;
/*
	case POT_GAIN_LEVEL:
	case POT_LEVITATION:
	case POT_FRUIT_JUICE:
	case POT_MONSTER_DETECTION:
	case POT_OBJECT_DETECTION:
		break;
*/
	/* KMH, balance patch -- added */
	case POT_ACID:
		if (!resists_acid(mon) && !resist(mon, POTION_CLASS, 0, NOTELL)) {
		    pline("%s %s in pain!", Monnam(mon),
			  is_silent(mon->data) ? "writhes" : "shrieks");
		    mon->mhp -= d(obj->cursed ? 2 : 1, obj->blessed ? 4 : 8);
		    if (mon->mhp < 1) {
			if (your_fault)
			    killed(mon);
			else
			    monkilled(mon, "", AD_ACID);
		    }
		}
		break;
	}
	if (angermon)
	    wakeup(mon);
	else
	    mon->msleeping = 0;
    }

	/* Note: potionbreathe() does its own docall() */
	if ((distance==0 || ((distance < 3) && rn2(5))) &&
	    (!breathless(youmonst.data) || haseyes(youmonst.data)))
		potionbreathe(obj);
	else if (obj->dknown && !objects[obj->otyp].oc_name_known &&
		   !objects[obj->otyp].oc_uname && cansee(mon->mx,mon->my))
		docall(obj);
	if(*u.ushops && obj->unpaid) {
	        register struct monst *shkp =
			shop_keeper(*in_rooms(u.ux, u.uy, SHOPBASE));

		if(!shkp)
		    obj->unpaid = 0;
		else {
		    (void)stolen_value(obj, u.ux, u.uy,
				 (boolean)shkp->mpeaceful, FALSE, TRUE);
		    subfrombill(obj, shkp);
		}
	}
	obfree(obj, (struct obj *)0);
}

/* vapors are inhaled or get in your eyes */
void
potionbreathe(obj)
register struct obj *obj;
{
	register int i, ii, isdone, kn = 0;

	switch(obj->otyp) {
	case POT_RESTORE_ABILITY:
	case POT_GAIN_ABILITY:
		if(obj->cursed) {
		    if (!breathless(youmonst.data))
			pline("Ulch!  That potion smells terrible!");
		    else if (haseyes(youmonst.data)) {
			int numeyes = eyecount(youmonst.data);
			Your("%s sting%s!",
			     (numeyes == 1) ? body_part(EYE) : makeplural(body_part(EYE)),
			     (numeyes == 1) ? "s" : "");
		    }
		    break;
		} else {
		    i = rn2(A_MAX);		/* start at a random point */
		    for(isdone = ii = 0; !isdone && ii < A_MAX; ii++) {
			if(ABASE(i) < AMAX(i)) {
			    ABASE(i)++;
			    /* only first found if not blessed */
			    isdone = !(obj->blessed);
			    flags.botl = 1;
			}
			if(++i >= A_MAX) i = 0;
		    }
		}
		break;
	case POT_FULL_HEALING:
	case POT_CURE_CRITICAL_WOUNDS:
		if (Upolyd && u.mh < u.mhmax) u.mh++, flags.botl = 1;
		if (u.uhp < u.uhpmax) u.uhp++, flags.botl = 1;
		/*FALL THROUGH*/
	case POT_EXTRA_HEALING:
	case POT_CURE_SERIOUS_WOUNDS:
		if (Upolyd && u.mh < u.mhmax) u.mh++, flags.botl = 1;
		if (u.uhp < u.uhpmax) u.uhp++, flags.botl = 1;
		/*FALL THROUGH*/
	case POT_HEALING:
	case POT_CURE_WOUNDS:
		if (Upolyd && u.mh < u.mhmax) u.mh++, flags.botl = 1;
		if (u.uhp < u.uhpmax) u.uhp++, flags.botl = 1;
		exercise(A_CON, TRUE);
		break;
	case POT_SICKNESS:
	case POT_POISON:
		if (!Role_if(PM_HEALER) && !Race_if(PM_HERBALIST)) {
			if (Upolyd) {
			    if (u.mh <= 5) u.mh = 1; else u.mh -= 5;
			} else {
			    if (u.uhp <= 5) u.uhp = 1; else u.uhp -= 5;
			}
			flags.botl = 1;
			exercise(A_CON, FALSE);
		}
		break;
	case POT_CYANIDE:
		if (u.uhp < 10) {
			/* DEATH */
			losehp(10,"a potion of cyanide",KILLED_BY);
		} else {
			u.uhp /= 5;
			if (u.uhp < 1) u.uhp = 1;		/* be generous */
		}
		flags.botl = 1;
		You_feel("very, very sick!");
		break;
	case POT_URINE:
		if (u.uhp < 3) {
			/* DEATH */
			losehp(3,"a potion of urine",KILLED_BY);
		} else {
			u.uhp /= 3;
			if (u.uhp < 1) u.uhp = 1;		/* be generous */
		}
		flags.botl = 1;
		You_feel("very poisoned!");
		break;
	case POT_RADIUM:
		u.uhp /= 4;
		if (u.uhp < 1) u.uhp = 1;		/* be generous */
		flags.botl = 1;
		You_feel("very sick!");
		break;
	case POT_HALLUCINATION:
		You("have a momentary vision.");

		(void) make_hallucinated(itimeout_incr(HHallucination,
					   rnd(30)), TRUE, 0L);
		break;
	case POT_CONFUSION:
	case POT_BOOZE:
	case POT_WINE:
		if(!Confusion)
			You_feel("somewhat dizzy.");
		make_confused(itimeout_incr(HConfusion, rnd(15)), FALSE);
		break;
	case POT_ICE:
		if(!Frozen)
			pline("You're suddenly shockfrosted!");
		make_frozen(itimeout_incr(HFrozen, rnd(15)), FALSE);
		break;
	case POT_FEAR:
		if(!Feared)
			pline("Your knees suddenly become softer!");
		make_feared(itimeout_incr(HFeared, rnd(50)), FALSE);
		break;
	case POT_FIRE:
		if(!Burned)
			pline("You caught fire!");
		make_burned(itimeout_incr(HBurned, rnd(30)), FALSE);
		break;
	case POT_DIMNESS:
		if(!Dimmed)
			pline("You are dimmed!");
		make_dimmed(itimeout_incr(HDimmed, rnd(30)), FALSE);
		break;
	case POT_STUNNING:
		if(!Stunned)
			pline("You stagger a bit...");
		make_stunned(itimeout_incr(HStun, rnd(15)), FALSE);
		break;
	case POT_NUMBNESS:
		if(!Numbed)
			pline("You're feeling a little numb!");
		make_numbed(itimeout_incr(HNumbed, rnd(30)), FALSE);
		break;
	case POT_CANCELLATION:
	      (void) cancel_monst(&youmonst, obj, FALSE, TRUE, FALSE);
		break;
	case POT_SLIME:
		if (!Slimed && !flaming(youmonst.data) && !Unchanging && !slime_on_touch(youmonst.data) ) {
		    You("don't feel very well.");
			stop_occupation();
		    Slimed = 100L;
		    flags.botl = 1;
		}
		break;

	case POT_INVISIBILITY:
		if (!Blind && !Invis) {
		    kn++;
		    pline("For an instant you %s!",
			See_invisible ? "could see right through yourself"
			: "couldn't see yourself");
		}
		break;
	case POT_PARALYSIS:
		kn++;
		if (!Free_action || !rn2(20)) {
		    pline("%s seems to be holding you.", Something);
			if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
		    nomul(-rnd(5), "frozen by breathing a potion", TRUE);
		    nomovemsg = You_can_move_again;
		    exercise(A_DEX, FALSE);
		} else You("stiffen momentarily.");
		break;
	case POT_SLEEPING:
		kn++;
		if ((!Free_action && !Sleep_resistance) || !rn2(20)) {
		    You_feel("rather tired.");
		    nomul(-rnd(5), "sleeping off a magical draught", TRUE);
		    nomovemsg = You_can_move_again;
		    exercise(A_DEX, FALSE);
		} else You("yawn.");
		break;
	case POT_SPEED:
		if (!Fast) Your("knees seem more flexible now.");
		incr_itimeout(&HFast, rnd(5));
		exercise(A_DEX, TRUE);
		break;
	case POT_BLINDNESS:
		if (!Blind && !u.usleep) {
		    kn++;
		    pline("It suddenly gets dark.");
		}
		make_blinded(itimeout_incr(Blinded, rnd(20)), FALSE);
		if (!Blind && !u.usleep) Your(vision_clears);
		break;
	case POT_WATER:
		if(u.umonnum == PM_GREMLIN) {
		    (void)split_mon(&youmonst, (struct monst *)0);
		} else if (u.ulycn >= LOW_PM) {
		    /* vapor from [un]holy water will trigger
		       transformation but won't cure lycanthropy */
		    if (obj->blessed && youmonst.data == &mons[u.ulycn])
			you_unwere(FALSE);
		    else if (obj->cursed && !Upolyd)
			you_were();
		}
		break;
	case POT_AMNESIA:
		if(u.umonnum == PM_GREMLIN)
		    (void)split_mon(&youmonst, (struct monst *)0);
		else if(u.umonnum == PM_FLAMING_SPHERE) {
		    You("flicker!");
		    losehp(d(1,6),"potion of amnesia", KILLED_BY_AN);
		} else if(u.umonnum == PM_IRON_GOLEM) {
		    You("rust!");
		    losehp(d(1,6),"potion of amnesia", KILLED_BY_AN);
		}
		You_feel("dizzy!");
		forget(1 + rn2(5));
		break;
	case POT_ACID:
		if (Stoned) fix_petrification();
	case POT_POLYMORPH:
	case POT_MUTATION:
		exercise(A_CON, FALSE);
		break;
	case POT_BLOOD:
	case POT_VAMPIRE_BLOOD:
		if (maybe_polyd(is_vampire(youmonst.data), Race_if(PM_VAMPIRE))) {
		    exercise(A_WIS, FALSE);
		    You_feel("a %ssense of loss.",
		      obj->otyp == POT_VAMPIRE_BLOOD ? "terrible " : "");
		} else
		    exercise(A_CON, FALSE);
		break;
	case POT_SALT_WATER:

		/* supposed to be thrown by cleaner assholes if you're a snail --Amy */
		if (Race_if(PM_ELONA_SNAIL) && !Upolyd) {
			pline("It's salt! you start to melt."); /* capitalization is NOT an error --Amy */
			losehp(((u.uhpmax < 10) ? 9999 : (u.uhpmax - rnd(10))), "a potion of salt solution", KILLED_BY);
			break;
		}
		break;

/*
	case POT_GAIN_LEVEL:
	case POT_LEVITATION:
	case POT_FRUIT_JUICE:
	case POT_MONSTER_DETECTION:
	case POT_OBJECT_DETECTION:
	case POT_OIL:
		break;
*/
	}
	/* note: no obfree() */
	if (obj->dknown) {
	    if (kn)
		makeknown(obj->otyp);
	    else if (!objects[obj->otyp].oc_name_known &&
						!objects[obj->otyp].oc_uname && !Blind)
		docall(obj);
	}
}

STATIC_OVL short
mixtype(o1, o2)
register struct obj *o1, *o2;
/* returns the potion type when o1 is dipped in o2 */
{
	/* cut down on the number of cases below */
	if (o1->oclass == POTION_CLASS &&
	    (o2->otyp == POT_GAIN_LEVEL ||
	     o2->otyp == POT_GAIN_ENERGY ||
	     o2->otyp == POT_HEALING ||
	     o2->otyp == POT_EXTRA_HEALING ||
	     o2->otyp == POT_FULL_HEALING ||
	     o2->otyp == POT_ENLIGHTENMENT ||
	     o2->otyp == POT_FRUIT_JUICE)) {
		struct obj *swp;

		swp = o1; o1 = o2; o2 = swp;
	}

	switch (o1->otyp) {
		case POT_HEALING:
			switch (o2->otyp) {
			    case POT_SPEED:
			    case POT_GAIN_LEVEL:
			    case POT_GAIN_ENERGY:
				return POT_EXTRA_HEALING;
			}
			break;
		case POT_EXTRA_HEALING:
			switch (o2->otyp) {
			    case POT_GAIN_LEVEL:
			    case POT_GAIN_ENERGY:
				return POT_FULL_HEALING;
			}
			break;
		case POT_FULL_HEALING:
			switch (o2->otyp) {
			    case POT_GAIN_LEVEL:
			    case POT_GAIN_ENERGY:
				return POT_GAIN_ABILITY;
			}
			break;
		case UNICORN_HORN:
			switch (o2->otyp) {
			    case POT_SICKNESS:
				return POT_FRUIT_JUICE;
			    case POT_HALLUCINATION:
			    case POT_BLINDNESS:
			    case POT_NUMBNESS:
			    case POT_STUNNING:
			    case POT_CONFUSION:
			    case POT_BLOOD:
			    case POT_VAMPIRE_BLOOD:
				return POT_WATER;
			}
			break;
		case AMETHYST:		/* "a-methyst" == "not intoxicated" */
			if (o2->otyp == POT_BOOZE)
			    return POT_FRUIT_JUICE;
			break;
		case POT_GAIN_LEVEL:
		case POT_GAIN_ENERGY:
			switch (o2->otyp) {
			    case POT_CONFUSION:
				return (rn2(3) ? POT_BOOZE : POT_ENLIGHTENMENT);
			    case POT_HEALING:
				return POT_EXTRA_HEALING;
			    case POT_EXTRA_HEALING:
				return POT_FULL_HEALING;
			    case POT_FULL_HEALING:
				return POT_GAIN_ABILITY;
			    case POT_FRUIT_JUICE:
				return POT_SEE_INVISIBLE;
			    case POT_BOOZE:
				return POT_HALLUCINATION;
			}
			break;
		case POT_FRUIT_JUICE:
			switch (o2->otyp) {
			    case POT_SICKNESS:
				return POT_SICKNESS;
			    case POT_BLOOD:
				return POT_BLOOD;
			    case POT_VAMPIRE_BLOOD:
				return POT_VAMPIRE_BLOOD;
			    case POT_SPEED:
				return POT_BOOZE;
			    case POT_GAIN_LEVEL:
			    case POT_GAIN_ENERGY:
				return POT_SEE_INVISIBLE;
			}
			break;
		case POT_ENLIGHTENMENT:
			switch (o2->otyp) {
			    case POT_LEVITATION:
				if (rn2(3)) return POT_GAIN_LEVEL;
				break;
			    case POT_FRUIT_JUICE:
				return POT_BOOZE;
			    case POT_BOOZE:
				return POT_CONFUSION;
			}
			break;
	}
	/* MRKR: Extra alchemical effects. */

	if (o2->otyp == POT_ACID && o1->oclass == GEM_CLASS) {
	  const char *potion_descr;

	  /* Note: you can't create smoky, milky or clear potions */

	  switch (o1->otyp) {

	    /* white */

	  case DILITHIUM_CRYSTAL:
	    /* explodes - special treatment in dodip */
	    /* here we just want to return something non-zero */
	    return POT_WATER;
	    break;
	  case DIAMOND:
	    /* won't dissolve */
	    potion_descr = NULL;
	    break;
	  case OPAL:
	    potion_descr = "cloudy";
	    break;

	    /* red */

	  case RUBY:
	    potion_descr = "ruby";
	    break;
	  case GARNET:
	    potion_descr = "pink";
	    break;
	  case JASPER:
	    potion_descr = "purple-red";
	    break;

	    /* orange */

	  case JACINTH:
	    potion_descr = "orange";
	    break;
	  case AGATE:
	    potion_descr = "swirly";
	    break;

	    /* yellow */

	  case CITRINE:
	    potion_descr = "yellow";
	    break;
	  case CHRYSOBERYL:
	    potion_descr = "golden";
	    break;

	    /* yellowish brown */

	  case AMBER:
	    potion_descr = "brown";
	    break;
	  case TOPAZ:
	    potion_descr = "murky";
	    break;

	    /* green */

	  case EMERALD:
	    potion_descr = "emerald";
	    break;
	  case TURQUOISE:
	    potion_descr = "sky blue";
	    break;
	  case AQUAMARINE:
	    potion_descr = "cyan";
	    break;
	  case JADE:
	    potion_descr = "dark green";
	    break;

	    /* blue */

	  case SAPPHIRE:
	    potion_descr = "brilliant blue";
	    break;

	    /* violet */

	  case AMETHYST:
	    potion_descr = "magenta";
	    break;
	  case FLUORITE:
	    potion_descr = "white";
	    break;

	    /* black */

	  case BLACK_OPAL:
	    potion_descr = "black";
	    break;
	  case JET:
	    potion_descr = "dark";
	    break;
	  case OBSIDIAN:
	    potion_descr = "effervescent";
	    break;

		/* more by Amy */
	  case CIRMOCLINE:
	    potion_descr = "thaumaturgic";
	    break;
	  case DIOPTASE:
	    potion_descr = "shining";
	    break;
	  case MORGANITE:
	    potion_descr = "incandescent";
	    break;
	  case ORTHOCLASE:
	    potion_descr = "freezing";
	    break;
	  case ANDALUSITE:
	    potion_descr = "squishy";
	    break;
	  case PERIDOT:
	    potion_descr = "greasy";
	    break;
	  case CHAROITE:
	    potion_descr = "slimy";
	    break;
	  case AMAZONITE:
	    potion_descr = "soapy";
	    break;
	  case SODALITH:
	    potion_descr = "ochre";
	    break;
	  case VIVIANITE:
	    potion_descr = "steamy";
	    break;
	  case EPIDOTE:
	    potion_descr = "gooey";
	    break;
	  case CHALCEDON:
	    potion_descr = "silver";
	    break;
	  case CHRYSOCOLLA:
	    potion_descr = "dangerous";
	    break;
	  case MALACHITE:
	    potion_descr = "creepy";
	    break;
	  case COVELLINE:
	    potion_descr = "distilled";
	    break;
	  case ROSE_QUARTZ:
	    potion_descr = "warping";
	    break;
	  case KUNZITE:
	    potion_descr = "unnatural";
	    break;
	  case SPINEL:
	    potion_descr = "venomous";
	    break;
	  case CYAN_STONE:
	    potion_descr = "colorless";
	    break;
	  case LAPIS_LAZULI:
	    potion_descr = "alchemical";
	    break;
	  case MOONSTONE:
	    potion_descr = "fluorescent";
	    break;
	  case PREHNITE:
	    potion_descr = "illuminated";
	    break;
	  case DISTHENE:
	    potion_descr = "bright";
	    break;
	  case APOPHYLLITE:
	    potion_descr = "carcinogenic";
	    break;
	  case TOURMALINE:
	    potion_descr = "liquid";
	    break;
	  case RHODOCHROSITE:
	    potion_descr = "buzzing";
	    break;
	  case ANHYDRITE:
	    potion_descr = "concocted";
	    break;
	  case HALITE:
	    potion_descr = "blue";
	    break;
	  case MARBLE:
	    potion_descr = "gray";
	    break;
	  case MORION:
	    potion_descr = "plaid";
	    break;

	  default: potion_descr = NULL;
	  }

	  if (potion_descr) {
	    int typ;

	    /* find a potion that matches the description */

	    for (typ = bases[POTION_CLASS];
		 objects[typ].oc_class == POTION_CLASS;
		 typ++) {

	      if (strcmp(potion_descr, OBJ_DESCR(objects[typ])) == 0 && OBJ_NAME(objects[typ]) ) {
		/* preventing potion descriptions that don't match any really existing potion types from generating --Amy */
		return typ;
	      }
	    }
	  }
	}

	return 0;
}

/* Bills an object that's about to be downgraded, assuming that's not already
 * been done */
STATIC_OVL
void
pre_downgrade_obj(obj, used)
register struct obj *obj;
boolean *used;
{
    boolean dummy = FALSE;

    if (!used) used = &dummy;
    if (!*used) Your("%s for a moment.", aobjnam(obj, "sparkle"));
    if(obj->unpaid && costly_spot(u.ux, u.uy) && !*used) {
	You("damage it, you pay for it.");
	bill_dummy_object(obj);
    }
    *used = TRUE;
}

/* Implements the downgrading effect of potions of amnesia and Lethe water */
STATIC_OVL
void
downgrade_obj(obj, nomagic, used)
register struct obj *obj;
int nomagic;	/* The non-magical object to downgrade to */
boolean *used;
{
    pre_downgrade_obj(obj, used);
    obj->otyp = nomagic;
    obj->spe = 0;
    obj->owt = weight(obj);
    flags.botl = TRUE;
}

boolean
get_wet(obj, amnesia)
register struct obj *obj;
boolean amnesia;
/* returns TRUE if something happened (potion should be used up) */
{
	char Your_buf[BUFSZ];
	struct obj* otmp;
	boolean used = FALSE;

	if (stack_too_big(obj)) {
		return(FALSE);
	}

	if (obj && obj->oartifact == ART_ELIANE_S_SHIN_SMASH) {
		pline("The liquid destroys your footwear instantly.");
		useup(obj);
		return(TRUE);
	}

	if (snuff_lit(obj)) return(TRUE);

	if (obj->greased) {
		grease_protect(obj,(char *)0,&youmonst);
		return(FALSE);
	}
	(void) Shk_Your(Your_buf, obj);

	if (obj->finalcancel) return(FALSE);

	/* (Rusting shop goods ought to be charged for.) */
	switch (obj->oclass) {
	    case POTION_CLASS:
		if (obj->otyp == POT_WATER) {
		    if (amnesia) {
			Your("%s to sparkle.", aobjnam(obj,"start"));
			obj->odiluted 	= 0;
			obj->otyp 	= POT_AMNESIA;
			used 		= TRUE;
			break;
		    }
		    return FALSE;
		}

		/* Diluting a !ofAmnesia just gives water... */
		if (obj->otyp == POT_AMNESIA) {
			Your("%s flat.", aobjnam(obj, "become"));
			obj->odiluted = 0;
			obj->otyp = POT_WATER;
			used = TRUE;
			break;
		}

		/* KMH -- Water into acid causes an explosion */
		if (obj->otyp == POT_ACID) {
			pline("It boils vigorously!");
			You("are caught in the explosion!");
			losehp(Acid_resistance ? rnd(5) : rnd(10),
			       "elementary chemistry", KILLED_BY);
			if (amnesia) {
			    You_feel("a momentary lapse of reason!");
			    forget(2 + rn2(3));
			}
			makeknown(obj->otyp);
			used = TRUE;
			break;
		}
		if (amnesia)
		    pline("%s %s completely.", Your_buf, aobjnam(obj,"dilute"));
		else
		    pline("%s %s%s.", Your_buf, aobjnam(obj,"dilute"),
		      		obj->odiluted ? " further" : "");
		if(obj->unpaid && costly_spot(u.ux, u.uy)) {
		    You("dilute it, you pay for it.");
		    bill_dummy_object(obj);
		}
		if (obj->odiluted || amnesia) {
			obj->odiluted = 0;
#ifdef UNIXPC
			obj->blessed = FALSE;
			obj->cursed = FALSE;
#else
			obj->blessed = obj->cursed = FALSE;
#endif
			obj->hvycurse = obj->prmcurse = FALSE;
			obj->morgcurse = obj->evilcurse = obj->bbrcurse = obj->stckcurse = FALSE;

			obj->otyp = POT_WATER;
		} else obj->odiluted++;
		used = TRUE;
		break;
	    case SCROLL_CLASS:
		if (obj->otyp != SCR_BLANK_PAPER  && !obj->oartifact && obj->otyp != SCR_HEALING && obj->otyp != SCR_EXTRA_HEALING && obj->otyp != SCR_STANDARD_ID && obj->otyp != SCR_MANA && obj->otyp != SCR_GREATER_MANA_RESTORATION && obj->otyp != SCR_CURE && obj->otyp != SCR_PHASE_DOOR
#ifdef MAIL
		    && obj->otyp != SCR_MAIL
#endif
		    ) {
			if (!Blind) {
				boolean oq1 = obj->quan == 1L;
				pline_The("scroll%s %s.",
					  oq1 ? "" : "s", otense(obj, "fade"));
			}
			if(obj->unpaid && costly_spot(u.ux, u.uy)) {
			    You("erase it, you pay for it.");
			    bill_dummy_object(obj);
			}
			obj->otyp = SCR_BLANK_PAPER;
			if (Role_if(PM_ARTIST)) {
				You_feel("guilty for erasing a scroll.");
			    adjalign(-10);
			}
			obj->spe = 0;
			used = TRUE;
		} 
		break;
	    case SPBOOK_CLASS:
		if (obj->otyp != SPE_BLANK_PAPER) {
			if (obj->otyp == SPE_BOOK_OF_THE_DEAD) {
	pline("%s suddenly heats up; steam rises and it remains dry.",
				The(xname(obj)));
			} else {
			    if (!Blind) {
				    boolean oq1 = obj->quan == 1L;
				    pline_The("spellbook%s %s.",
					oq1 ? "" : "s", otense(obj, "fade"));
			    }
			    if(obj->unpaid) {
				subfrombill(obj, shop_keeper(*u.ushops));
			        You("erase it, you pay for it.");
			        bill_dummy_object(obj);
			    }
			    obj->otyp = SPE_BLANK_PAPER;
			}
			used = TRUE;
		}
		break;
	    case GEM_CLASS:
		if (amnesia && (obj->otyp == LUCKSTONE ||
			obj->otyp == LOADSTONE || obj->otyp == HEALTHSTONE ||
			obj->otyp == TOUCHSTONE))
		    downgrade_obj(obj, FLINT, &used);
		if (obj->otyp == SALT_CHUNK) {
			pline("The stone dissolves!");
			makeknown(obj->otyp);
			if (obj->oartifact == ART_CUBIC_SODIUM_CHLORIDE) {
				pline("The stone held hidden magical powers, and upon their release, grateful monsters come into existence!");
				struct monst *familiar;
				familiar = makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
				if (familiar) (void) tamedog(familiar, (struct obj *) 0, TRUE);
				familiar = makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
				if (familiar) (void) tamedog(familiar, (struct obj *) 0, TRUE);
				familiar = makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
				if (familiar) (void) tamedog(familiar, (struct obj *) 0, TRUE);
				familiar = makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
				if (familiar) (void) tamedog(familiar, (struct obj *) 0, TRUE);
				familiar = makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
				if (familiar) (void) tamedog(familiar, (struct obj *) 0, TRUE);

			}
			useup(obj);
			otmp = mksobj(POT_SALT_WATER,TRUE,FALSE);
			if (otmp) {
				otmp->blessed = otmp->cursed = otmp->hvycurse = otmp->prmcurse = otmp->morgcurse = otmp->evilcurse = otmp->bbrcurse = otmp->stckcurse = 0;
				addinv(otmp);
				update_inventory();
			}
			return TRUE;
		}
		break;
	    case TOOL_CLASS:
		/* Artifacts aren't downgraded by amnesia */
		if (obj->otyp == BOTTLE) {
			pline("The bottle%s filled with water.",obj->quan > 1 ? "s are" : " is");
			obj->oclass = POTION_CLASS;
			obj->otyp = POT_WATER;
			/* retain curses/blessings etc. */
			return(TRUE);
		}
		if (amnesia && !obj->oartifact) {
		    switch (obj->otyp) {
			case MAGIC_LAMP:
			    /* Magic lamps forget their djinn... */
			    downgrade_obj(obj, OIL_LAMP, &used);
			    break;
			case MAGIC_CANDLE:
			    downgrade_obj(obj, 
					    rn2(2)? WAX_CANDLE : TALLOW_CANDLE,
					    &used);
			    break;
			case DRUM_OF_EARTHQUAKE:
			    downgrade_obj(obj, LEATHER_DRUM, &used);
			    break;
			case MAGIC_WHISTLE:
			    /* Magic whistles lose their powers... */
			    downgrade_obj(obj, TIN_WHISTLE, &used);
			    break;
			case MAGIC_FLUTE:
			    /* Magic flutes sound normal again... */
			    downgrade_obj(obj, WOODEN_FLUTE, &used);
			    break;
			case MAGIC_HARP:
			    /* Magic harps sound normal again... */
			    downgrade_obj(obj, WOODEN_HARP, &used);
			    break;
			case FIRE_HORN:
			case FROST_HORN:
			case TEMPEST_HORN:
			case HORN_OF_PLENTY:
			    downgrade_obj(obj, TOOLED_HORN, &used);
			    break;
			case MAGIC_MARKER:
			    /* Magic markers run... */
			    if (obj->spe > 0) {
				pre_downgrade_obj(obj, &used);
				if ((obj->spe -= (3 + rn2(10))) < 0) 
				    obj->spe = 0;
			    }
			    break;
		    }
		}

		/* The only other tools that can be affected are pick axes and 
		 * unicorn horns... */
		if (!is_weptool(obj)) break;
		/* Drop through for disenchantment and rusting... */
		/* fall through */
	    case ARMOR_CLASS:
	    case WEAPON_CLASS:
	    case WAND_CLASS:
	    case RING_CLASS:
	    /* Just "fall through" to generic rustprone check for now. */
	    /* fall through */
	    default:
		switch(artifact_wet(obj, FALSE)) {
		    case -1: break;
		    default:
			return TRUE;
		}
		/* !ofAmnesia acts as a disenchanter... */
		if (amnesia && obj->spe > 0) {
		    pre_downgrade_obj(obj, &used);
		    drain_item(obj);
		}
		if (!obj->oerodeproof && (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ) && is_rustprone(obj) &&
		    (obj->oeroded < MAX_ERODE) && !rn2(2)) {
			pline("%s %s some%s.",
			      Your_buf, aobjnam(obj, "rust"),
			      obj->oeroded ? " more" : "what");
			obj->oeroded++;
			if(obj->unpaid && costly_spot(u.ux, u.uy) && !used) {
			    You("damage it, you pay for it.");
			    bill_dummy_object(obj);
			}
			used = TRUE;
		} 
		break;
	}
	/* !ofAmnesia might strip away fooproofing... */
	if (amnesia && obj->oerodeproof && !rn2(13)) {
	    pre_downgrade_obj(obj, &used);
	    obj->oerodeproof = FALSE;
	}

	/* !ofAmnesia also strips blessed/cursed status... */

	if (amnesia && (obj->cursed || obj->blessed)) {
	    /* Blessed objects are valuable, cursed objects aren't, unless
	     * they're water.
	     */
	    if (obj->blessed || obj->otyp == POT_WATER)
		pre_downgrade_obj(obj, &used);
	    else if (!used) {
		Your("%s for a moment.", aobjnam(obj, "sparkle"));
		used = TRUE;
	    }
		/* Amy edit: you cannot easily uncurse everything, sorry */
	    if (obj->otyp == POT_WATER) uncurse(obj);
	    unbless(obj);
	}

	if (used) 
	    update_inventory();
	else 
	    pline("%s %s wet.", Your_buf, aobjnam(obj,"get"));

	return used;
}


/* KMH, balance patch -- idea by Dylan O'Donnell <dylanw@demon.net>
 * The poor hacker's polypile.  This includes weapons, armor, and tools.
 * To maintain balance, magical categories (amulets, scrolls, spellbooks,
 * potions, rings, and wands) should NOT be supported.
 * Polearms are not currently implemented.
 */
int
upgrade_obj(obj)
register struct obj *obj;
/* returns 1 if something happened (potion should be used up) 
 * returns 0 if nothing happened
 * returns -1 if object exploded (potion should be used up) 
 */
{
	int chg, otyp = obj->otyp, otyp2;
	xchar ox, oy;
	long owornmask;
	struct obj *otmp;
	boolean explodes;
	char buf[BUFSZ];

	/* Check to see if object is valid */
	if (!obj)
		return 0;
	(void)snuff_lit(obj);
	if (obj->oartifact)
		/* WAC -- Could have some funky fx */
		return 0;

	if (stack_too_big(obj))
		return 0;

	switch (obj->otyp)
	{
		/* weapons */
		case ORCISH_DAGGER:
			obj->otyp = DAGGER;
			break;
		case GREAT_DAGGER:
		case DAGGER:
			if (!rn2(2)) obj->otyp = ELVEN_DAGGER;
			else obj->otyp = DARK_ELVEN_DAGGER;
			break;
		case ELVEN_DAGGER:
		case DARK_ELVEN_DAGGER:
			obj->otyp = GREAT_DAGGER;
			break;
		case KNIFE:
			obj->otyp = STILETTO;
			break;
		case STILETTO:
			obj->otyp = KNIFE;
			break;
		case AXE:
			obj->otyp = BATTLE_AXE;
			break;
		case BATTLE_AXE:
			obj->otyp = AXE;
			break;
		case PICK_AXE:
			obj->otyp = DWARVISH_MATTOCK;
			break;
		case DWARVISH_MATTOCK:
			obj->otyp = PICK_AXE;
			break;
		case ORCISH_SHORT_SWORD:
			obj->otyp = SHORT_SWORD;
			break;
		case ELVEN_SHORT_SWORD:
		case DARK_ELVEN_SHORT_SWORD:
		case SHORT_SWORD:
			obj->otyp = DWARVISH_SHORT_SWORD;
			break;
		case DWARVISH_SHORT_SWORD:
			if (!rn2(2)) obj->otyp = ELVEN_SHORT_SWORD;
			else obj->otyp = DARK_ELVEN_SHORT_SWORD;
			break;
		case IRON_SABER:
			if (!rn2(2)) obj->otyp = SILVER_SABER;
			else obj->otyp = GOLDEN_SABER;
			break;
		case GOLDEN_SABER:
		case SILVER_SABER:
			obj->otyp = IRON_SABER;
		case BROADSWORD:
			obj->otyp = ELVEN_BROADSWORD;
			break;
		case ELVEN_BROADSWORD:
			obj->otyp = BROADSWORD;
			break;
		case MOP:
			obj->otyp = SPECIAL_MOP;
			break;
		case SPECIAL_MOP:
			obj->otyp = MOP;
			break;
		case CLUB:
			obj->otyp = AKLYS;
			break;
		case AKLYS:
			obj->otyp = CLUB;
			break;
		case SICKLE:
			obj->otyp = ELVEN_SICKLE;
			break;
		case ELVEN_SICKLE:
			obj->otyp = SICKLE;
			break;
		case FIRE_HOOK:
			obj->otyp = PLATINUM_FIRE_HOOK;
			break;
		case PLATINUM_FIRE_HOOK:
			obj->otyp = FIRE_HOOK;
			break;
		case ATHAME:
			obj->otyp = MERCURIAL_ATHAME;
			break;
		case MERCURIAL_ATHAME:
			obj->otyp = ATHAME;
			break;
		case SUGUHANOKEN:
			obj->otyp = GREAT_HOUCHOU;
			break;
		case GREAT_HOUCHOU:
			obj->otyp = SUGUHANOKEN;
			break;
		case WAR_HAMMER:
			obj->otyp = HEAVY_HAMMER;
			break;
		case HEAVY_HAMMER:
			obj->otyp = WAR_HAMMER;
			break;
		case ELVEN_BOW:
		case DARK_ELVEN_BOW:
		case YUMI:
		case ORCISH_BOW:
			obj->otyp = BOW;
			break;
		case BOW:
			switch (rn2(3)) {
				case 0: obj->otyp = ELVEN_BOW; break;
				case 1: obj->otyp = DARK_ELVEN_BOW; break;
				case 2: obj->otyp = YUMI; break;
			}
			break;
		case ELVEN_ARROW:
		case DARK_ELVEN_ARROW:
		case YA:
		case ORCISH_ARROW:
			obj->otyp = ARROW;
			break;
		case ARROW:
			switch (rn2(3)) {
				case 0: obj->otyp = ELVEN_ARROW; break;
				case 1: obj->otyp = DARK_ELVEN_ARROW; break;
				case 2: obj->otyp = YA; break;
			}
			break;
		/* armour */
		case ELVEN_MITHRIL_COAT:
			obj->otyp = DARK_ELVEN_MITHRIL_COAT;
			break;
		case DARK_ELVEN_MITHRIL_COAT:
			obj->otyp = ELVEN_MITHRIL_COAT;
			break;
		case ORCISH_CHAIN_MAIL:
			obj->otyp = CHAIN_MAIL;
			break;
		case CHAIN_MAIL:
			obj->otyp = ORCISH_CHAIN_MAIL;
			break;
		case OLIHARCON_SPLINT_MAIL:
			obj->otyp = SPLINT_MAIL;
			break;
		case SPLINT_MAIL:
			obj->otyp = OLIHARCON_SPLINT_MAIL;
			break;
		case STUDDED_LEATHER_ARMOR:
		case LEATHER_JACKET:
			obj->otyp = LEATHER_ARMOR;
			break;
		case LEATHER_ARMOR:
			obj->otyp = STUDDED_LEATHER_ARMOR;
			break;
		case PLATE_MAIL:
			if (!rn2(2)) obj->otyp = CRYSTAL_PLATE_MAIL;
			else obj->otyp = BRONZE_PLATE_MAIL;
			break;
		case BRONZE_PLATE_MAIL:
		case CRYSTAL_PLATE_MAIL:
			obj->otyp = PLATE_MAIL;
			break;
		case CAMOUFLAGED_CLOTHES:
			obj->otyp = SPECIAL_CAMOUFLAGED_CLOTHES;
			break;
		case SPECIAL_CAMOUFLAGED_CLOTHES:
			obj->otyp = CAMOUFLAGED_CLOTHES;
			break;

		/* robes */
		case ROBE:
			if (!rn2(2)) obj->otyp = ROBE_OF_PROTECTION;
			else obj->otyp = ROBE_OF_POWER;
			break;
		case ROBE_OF_PROTECTION:
		case ROBE_OF_POWER:
			obj->otyp = ROBE;
			break;
		/* cloaks */
		case CLOAK_OF_PROTECTION:
		case CLOAK_OF_INVISIBILITY:
		case CLOAK_OF_MAGIC_RESISTANCE:
		case CLOAK_OF_DISPLACEMENT:
		case DWARVISH_CLOAK:
		case ORCISH_CLOAK:
			if (!rn2(2)) obj->otyp = OILSKIN_CLOAK;
			else obj->otyp = ELVEN_CLOAK;
			break;
		case OILSKIN_CLOAK:
		case ELVEN_CLOAK:
			switch (rn2(4)) {
				case 0: obj->otyp = CLOAK_OF_PROTECTION; break;
				case 1: obj->otyp = CLOAK_OF_INVISIBILITY; break;
				case 2: obj->otyp = CLOAK_OF_MAGIC_RESISTANCE; break;
				case 3: obj->otyp = CLOAK_OF_DISPLACEMENT; break;
			}
			break;
		/* helms */
		case FEDORA:
			obj->otyp = ELVEN_LEATHER_HELM;
			break;
		case ELVEN_LEATHER_HELM:
			obj->otyp = FEDORA;
			break;
		case ELVEN_HELM:
			obj->otyp = HIGH_ELVEN_HELM;
			break;
		case HIGH_ELVEN_HELM:
			obj->otyp = ELVEN_HELM;
			break;
		case DENTED_POT:
			obj->otyp = ORCISH_HELM;
			break;
		case ORCISH_HELM:
		case HELM_OF_BRILLIANCE:
		case HELM_OF_TELEPATHY:
			obj->otyp = DWARVISH_IRON_HELM;
			break;
		case DWARVISH_IRON_HELM:
			if (!rn2(2)) obj->otyp = HELM_OF_BRILLIANCE;
			else obj->otyp = HELM_OF_TELEPATHY;
			break;
		case CORNUTHAUM:
			obj->otyp = DUNCE_CAP;
			break;
		case DUNCE_CAP:
			obj->otyp = CORNUTHAUM;
			break;
		/* gloves */
		case LEATHER_GLOVES:
			if (!rn2(2)) obj->otyp = GAUNTLETS_OF_SWIMMING;
			else obj->otyp = GAUNTLETS_OF_DEXTERITY;
			break;
		case GAUNTLETS_OF_SWIMMING:
		case GAUNTLETS_OF_DEXTERITY:
			obj->otyp = LEATHER_GLOVES;
			break;
		case GAUNTLETS:
			obj->otyp = SILVER_GAUNTLETS;
			break;
		case SILVER_GAUNTLETS:
			obj->otyp = GAUNTLETS;
			break;
		/* shields */
		case ELVEN_SHIELD:
			if (!rn2(2)) obj->otyp = URUK_HAI_SHIELD;
			else obj->otyp = ORCISH_SHIELD;
			break;
		case URUK_HAI_SHIELD:
		case ORCISH_SHIELD:
			obj->otyp = ELVEN_SHIELD;
			break;
		case DWARVISH_ROUNDSHIELD:
			obj->otyp = LARGE_SHIELD;
			break;
		case LARGE_SHIELD:
			obj->otyp = DWARVISH_ROUNDSHIELD;
			break;
		/* boots */
		case LOW_BOOTS:
			obj->otyp = HIGH_BOOTS;
			break;
		case HIGH_BOOTS:
			obj->otyp = LOW_BOOTS;
			break;
		case SNEAKERS:
			obj->otyp = SOFT_SNEAKERS;
			break;
		case SOFT_SNEAKERS:
			obj->otyp = SNEAKERS;
			break;
		/* NOTE:  Supposedly,  HIGH_BOOTS should upgrade to any of the
			other magic leather boots (except for fumble).  IRON_SHOES
			should upgrade to the iron magic boots,  unless
			the iron magic boots are fumble */
		/* rings,  amulets */
		case SACK:
			obj->otyp = rn2(5) ? OILSKIN_SACK : BAG_OF_HOLDING;
			break;
		case OILSKIN_SACK:
			obj->otyp = BAG_OF_HOLDING;
			break;
		case BAG_OF_HOLDING:
			obj->otyp = OILSKIN_SACK;
			break;
		case CHEST_OF_HOLDING:
			obj->otyp = CHEST;
			break;
		case CHEST:
			obj->otyp = CHEST_OF_HOLDING;
			break;
		case ICE_BOX:
			obj->otyp = rn2(2) ? ICE_BOX_OF_HOLDING : ICE_BOX_OF_WATERPROOFING;
			break;
		case ICE_BOX_OF_HOLDING:
		case ICE_BOX_OF_WATERPROOFING:
			obj->otyp = ICE_BOX;
			break;
		case TOWEL:
			obj->otyp = BLINDFOLD;
			break;
		case BLINDFOLD:
			obj->otyp = TOWEL;
			break;
		case CREDIT_CARD:
		case LOCK_PICK:
			obj->otyp = SKELETON_KEY;
			break;
		case SKELETON_KEY:
			obj->otyp = LOCK_PICK;
			break;
		case TALLOW_CANDLE:
			obj->otyp = WAX_CANDLE;
			break;
		case WAX_CANDLE:
			obj->otyp = TALLOW_CANDLE;
			break;
		case OIL_LAMP:
			obj->otyp = BRASS_LANTERN;
			break;
		case BRASS_LANTERN:
			obj->otyp = OIL_LAMP;
			break;
		case TIN_WHISTLE:
			obj->otyp = MAGIC_WHISTLE;
			break;
		case MAGIC_WHISTLE:
			obj->otyp = TIN_WHISTLE;
			break;
		case WOODEN_FLUTE:
			obj->otyp = MAGIC_FLUTE;
			obj->spe = rn1(5,10);
			break;
		case MAGIC_FLUTE:
			obj->otyp = WOODEN_FLUTE;
			break;
		case TOOLED_HORN:
			obj->otyp = rn1(HORN_OF_PLENTY - TOOLED_HORN, FROST_HORN);
			obj->spe = rn1(5,10);
			obj->known = 0;
			break;
		case HORN_OF_PLENTY:
		case FIRE_HORN:
		case FROST_HORN:
		case TEMPEST_HORN:
			obj->otyp = TOOLED_HORN;
			break;
		case WOODEN_HARP:
			obj->otyp = MAGIC_HARP;
			obj->spe = rn1(5,10);
			obj->known = 0;
			break;
		case MAGIC_HARP:
			obj->otyp = WOODEN_HARP;
			break;
		case LEATHER_LEASH:
			obj->otyp = LEATHER_SADDLE;
			break;
		case LEATHER_SADDLE:
			obj->otyp = LEATHER_LEASH;
			break;
		case INKA_LEASH:
			obj->otyp = INKA_SADDLE;
			break;
		case INKA_SADDLE:
			obj->otyp = INKA_LEASH;
			break;
		case TIN_OPENER:
			obj->otyp = TINNING_KIT;
			obj->spe = rn1(30,70);
			obj->known = 0;
			break;
		case TINNING_KIT:
			obj->otyp = TIN_OPENER;
			break;
		case EXPENSIVE_CAMERA:
			obj->otyp = MIRROR;
			break;
		case MIRROR:
			obj->otyp = EXPENSIVE_CAMERA;
			obj->spe = rn1(30,70);
			obj->known = 0;
			break;
		case CRYSTAL_BALL:
			/* "ball-point pen" */
			obj->otyp = MAGIC_MARKER;
			/* Keep the charges (crystal ball usually less than marker) */
			break;
		case MAGIC_MARKER:
			obj->otyp = CRYSTAL_BALL;
			chg = rn1(10,3);
			if (obj->spe > chg)
				obj->spe = chg;
			obj->known = 0;
			break;
		case K_RATION:
		case C_RATION:
		case LEMBAS_WAFER:
			if (!rn2(2)) obj->otyp = CRAM_RATION;
			else obj->otyp = FOOD_RATION;
			break;
		case FOOD_RATION:
		case CRAM_RATION:
			obj->otyp = LEMBAS_WAFER;
			break;
		case CHARRED_BREAD:
			obj->otyp = BREAD;
			break;
		case SLICE_OF_PIZZA:
			obj->otyp = PIZZA;
			break;
		case PIZZA:
			obj->otyp = SLICE_OF_PIZZA;
			break;
		case LOADSTONE:
			obj->otyp = FLINT;
			break;
		case FLINT:
			if (!rn2(2)) obj->otyp = LUCKSTONE;
			else obj->otyp = WHETSTONE; /*no more stacks of healthstones for cavedwarfs, sorry --Amy*/
			break;
		default:
			/* This object is not upgradable */
			return 0;
	}

	if (artifact_name(ONAME(obj), &otyp2) && otyp2 == obj->otyp) {
	    int n;
	    char c1, c2;

	    strcpy(buf, ONAME(obj));
	    n = rn2((int)strlen(buf));
	    c1 = lowc(buf[n]);
	    do c2 = 'a' + rn2('z'-'a'); while (c1 == c2);
	    buf[n] = (buf[n] == c1) ? c2 : highc(c2);  /* keep same case */
	    if (oname(obj, buf) != obj)
		panic("upgrade_obj: unhandled realloc");
	}

	if ((!carried(obj) || obj->unpaid) &&
		!is_hazy(obj) &&
		get_obj_location(obj, &ox, &oy, BURIED_TOO|CONTAINED_TOO) &&
		costly_spot(ox, oy)) {
	    char objroom = *in_rooms(ox, oy, SHOPBASE);
	    register struct monst *shkp = shop_keeper(objroom);

	    if ((!obj->no_charge ||
		 (Has_contents(obj) &&
		    (contained_cost(obj, shkp, 0L, FALSE, FALSE) != 0L)))
	       && inhishop(shkp)) {
		if(shkp->mpeaceful) {
		    if(*u.ushops && *in_rooms(u.ux, u.uy, 0) ==
			    *in_rooms(shkp->mx, shkp->my, 0) &&
			    !costly_spot(u.ux, u.uy))
			make_angry_shk(shkp, ox, oy);
		    else {
			pline("%s gets angry!", Monnam(shkp));
			hot_pursuit(shkp);
		    }
		} else Norep("%s is furious!", Monnam(shkp));
		otyp2 = obj->otyp;
		obj->otyp = otyp;
		/*
		 * [ALI] When unpaid containers are upgraded, the
		 * old container is billed as a dummy object, but
		 * it's contents are unaffected and will remain
		 * either unpaid or not as appropriate.
		 */
		otmp = obj->cobj;
		obj->cobj = NULL;
		if (costly_spot(u.ux, u.uy) && objroom == *u.ushops)
		    bill_dummy_object(obj);
		else
		    (void) stolen_value(obj, ox, oy, FALSE, FALSE, FALSE);
		obj->otyp = otyp2;
		obj->cobj = otmp;
	    }
	}

	/* The object was transformed */
	obj->owt = weight(obj);
	obj->oclass = objects[obj->otyp].oc_class;
	if (!objects[obj->otyp].oc_uses_known)
	    obj->known = 1;

	if (carried(obj)) {
	    if (obj == uskin) rehumanize();
	    /* Quietly remove worn item if no longer compatible --ALI */
	    owornmask = obj->owornmask;
	    if (owornmask & W_ARM && !is_suit(obj))
		owornmask &= ~W_ARM;
	    if (owornmask & W_ARMC && !is_cloak(obj))
		owornmask &= ~W_ARMC;
	    if (owornmask & W_ARMH && !is_helmet(obj))
		owornmask &= ~W_ARMH;
	    if (owornmask & W_ARMS && !is_shield(obj))
		owornmask &= ~W_ARMS;
	    if (owornmask & W_ARMG && !is_gloves(obj))
		owornmask &= ~W_ARMG;
	    if (owornmask & W_ARMF && !is_boots(obj))
		owornmask &= ~W_ARMF;
	    if (owornmask & W_ARMU && !is_shirt(obj))
		owornmask &= ~W_ARMU;
	    if (owornmask & W_TOOL && obj->otyp != BLINDFOLD && obj->otyp != EYECLOSER && obj->otyp != DRAGON_EYEPATCH && obj->otyp != CONDOME && obj->otyp != SOFT_CHASTITY_BELT &&
	      obj->otyp != TOWEL && obj->otyp != LENSES && obj->otyp != RADIOGLASSES && obj->otyp != BOSS_VISOR && obj->otyp != BOSS_VISOR)
		owornmask &= ~W_TOOL;
	    otyp2 = obj->otyp;
	    obj->otyp = otyp;
	    if (obj->otyp == LEATHER_LEASH && obj->leashmon) o_unleash(obj);
	    if (obj->otyp == INKA_LEASH && obj->leashmon) o_unleash(obj);
	    remove_worn_item(obj, TRUE);
	    obj->otyp = otyp2;
	    obj->owornmask = owornmask;
	    setworn(obj, obj->owornmask);
	    puton_worn_item(obj);
	}

	if ((obj->otyp == BAG_OF_HOLDING || obj->otyp == CHEST_OF_HOLDING || obj->otyp == ICE_BOX_OF_HOLDING) && Has_contents(obj)) {
	    explodes = FALSE;

	    for (otmp = obj->cobj; otmp; otmp = otmp->nobj)
		if (mbag_explodes(otmp, 0)) { 
		    explodes = TRUE;
		    break;
		}

            if (explodes) {
		pline("As you upgrade your container, you are blasted by a magical explosion!");
		delete_contents(obj);
		if (carried(obj))
		    useup(obj);
		else
		    useupf(obj, obj->quan);
		losehp(d(6,6), "magical explosion", KILLED_BY_AN);
		return -1;
	    }
	}
	return 1;
}

int
dodip()
{
	struct obj *potion, *obj, *singlepotion;
	const char *tmp;
	uchar here;
	char allowall[2], qbuf[QBUFSZ], Your_buf[BUFSZ];
	short mixture;
	int res;

	allowall[0] = ALL_CLASSES; allowall[1] = '\0';
	if(!(obj = getobj(allowall, "dip")))
		return(0);

	here = levl[u.ux][u.uy].typ;
	/* Is there a fountain to dip into here? */
	if (IS_FOUNTAIN(here)) {
		if(yn("Dip it into the fountain?") == 'y') {
			dipfountain(obj);
			return(1);
		}
	} else if (IS_TOILET(here)) {        
		if(yn("Dip it into the toilet?") == 'y') {
			diptoilet(obj);
			return(1);
		}
	} else if (is_waterypool(u.ux,u.uy) && !(is_crystalwater(u.ux, u.uy) && !(Levitation || Flying)) ) {
		tmp = waterbody_name(u.ux,u.uy);
		sprintf(qbuf, "Dip it into the %s?", tmp);
		if (yn(qbuf) == 'y') {
		    if (Levitation) {
			floating_above(tmp);
		    } else if (u.usteed && !(nohands(youmonst.data) && !Race_if(PM_TRANSFORMER) && uimplant && uimplant->oartifact == ART_READY_FOR_A_RIDE) && !is_swimmer(u.usteed->data) && !u.usteed->egotype_watersplasher &&
			    (PlayerCannotUseSkills || P_SKILL(P_RIDING) < P_BASIC) ) {
			rider_cant_reach(); /* not skilled enough to reach */
		    } else {

			if (rn2(2) && !stack_too_big(obj) && !obj->oerodeproof && is_rustprone(obj) && !hard_to_destruct(obj) && obj->oeroded == MAX_ERODE) {

				remove_worn_item(obj, FALSE);
				if (obj == uball) unpunish();
				useupall(obj);
				update_inventory();
				pline("The item rusted away completely!");

				return 1;
			}

			(void) get_wet(obj, level.flags.lethe);
			if (obj->otyp == POT_ACID) useup(obj);
		    }
		    return 1;
		}
	} else if (is_lava(u.ux,u.uy)) {
		sprintf(qbuf, "Dip it into the lava?");
		if (yn(qbuf) == 'y') {
		    if (Levitation) {
			floating_above("lava");
		    } else if (u.usteed && !(nohands(youmonst.data) && !Race_if(PM_TRANSFORMER) && uimplant && uimplant->oartifact == ART_READY_FOR_A_RIDE) && !is_swimmer(u.usteed->data) && !u.usteed->egotype_watersplasher &&
			    (PlayerCannotUseSkills || P_SKILL(P_RIDING) < P_BASIC) ) {
			rider_cant_reach(); /* not skilled enough to reach */
		    } else {

			if (obj && rn2(2) && !stack_too_big(obj) && !obj->oerodeproof && is_flammable(obj) && !hard_to_destruct(obj) && obj->oeroded == MAX_ERODE) {

				remove_worn_item(obj, FALSE);
				if (obj == uball) unpunish();
				useupall(obj);
				update_inventory();
				pline("The item burned away completely!");

				return 1;
			}

			if (obj && !stack_too_big(obj) && !obj->oerodeproof && (obj->oclass == SCROLL_CLASS) ) {

				useupall(obj);
				update_inventory();
				pline("The scroll burns up!");

				return 1;
			}

			else if (obj && !stack_too_big(obj) && !evades_destruction(obj) && !obj->oerodeproof && (obj->oclass == SPBOOK_CLASS) ) {

				useupall(obj);
				update_inventory();
				pline("The book burns up!");

				return 1;
			}

			else if (obj && !stack_too_big(obj) && !obj->oerodeproof && (obj->oclass == POTION_CLASS) ) {

				pline("The potion explodes!");
				potionbreathe(obj);
				useupall(obj);
				update_inventory();

				return 1;
			}

			if (obj && !stack_too_big(obj) && is_flammable(obj) && obj->oeroded < MAX_ERODE && !obj->oerodeproof && !rn2(2)) {
				pline("%s %s%s.", Yname2(obj), otense(obj, "burn"),
			      obj->oeroded+1 == MAX_ERODE ? " completely" :
			      obj->oeroded ? " further" : "");
				obj->oeroded++;
				return 1;
			}
			pline("%s is warmed.", Yname2(obj));

		    }
		    return 1;
		}

	}

	if(!(potion = getobj(beverages, "dip into")))
		return(0);
	if (potion == obj && potion->quan == 1L) {
		pline(Hallucination ? "The liquid inside that potion wobbles around. It's funny." : "That is a potion bottle, not a Klein bottle!");
		return 0;
	}

	if(potion->otyp != POT_WATER && obj->otyp == POT_WATER) {
	  /* swap roles, to ensure symmetry */
	  struct obj *otmp = potion;
	  potion = obj;
	  obj = otmp;
	} 
	potion->in_use = TRUE;          /* assume it will be used up */
	if(potion->otyp == POT_WATER) {
		boolean useeit = !Blind;
		if (useeit) (void) Shk_Your(Your_buf, obj);
		if (potion->blessed) {
			if (obj->cursed) {
				if (useeit)
				    pline("%s %s %s.",
					  Your_buf,
					  aobjnam(obj, "softly glow"),
					  hcolor(NH_AMBER));
				if (!stack_too_big(obj)) uncurse(obj);
				else pline("Unfortunately, the stack was too big, so nothing happened.");
				obj->bknown=1;
	poof:
				if(!(objects[potion->otyp].oc_name_known) &&
				   !(objects[potion->otyp].oc_uname))
					docall(potion);
				useup(potion);
				return(1);
			} else if(!obj->blessed) {
				if (useeit) {
				    tmp = hcolor(NH_LIGHT_BLUE);
				    pline("%s %s with a%s %s aura.",
					  Your_buf,
					  aobjnam(obj, "softly glow"),
					  index(vowels, *tmp) ? "n" : "", tmp);
				}
				if (!stack_too_big(obj)) bless(obj);
				else pline("Unfortunately, the stack was too big, so nothing happened.");
				obj->bknown=1;
				goto poof;
			}
		} else if (potion->cursed) {
			if (obj->blessed) {
				if (useeit)
				    pline("%s %s %s.",
					  Your_buf,
					  aobjnam(obj, "glow"),
					  hcolor((const char *)"brown"));
				if (!stack_too_big(obj)) unbless(obj);
				else pline("Unfortunately, the stack was too big, so nothing happened.");
				obj->bknown=1;
				goto poof;
			} else if(!obj->cursed) {
				if (useeit) {
				    tmp = hcolor(NH_BLACK);
				    pline("%s %s with a%s %s aura.",
					  Your_buf,
					  aobjnam(obj, "glow"),
					  index(vowels, *tmp) ? "n" : "", tmp);
				}
				if (!stack_too_big(obj)) curse(obj);
				else pline("Unfortunately, the stack was too big, so nothing happened.");
				obj->bknown=1;
				goto poof;
			}
		} else {
			switch(artifact_wet(obj,TRUE)) {
				/* Assume ZT_xxx is AD_xxx-1 */
				case -1: break;
				default:
					zap_over_floor(u.ux, u.uy,
					  (artifact_wet(obj,TRUE)-1), NULL);
					break;
			}
			if (get_wet(obj, FALSE))
			    goto poof;
		}
	} else if (potion->otyp == POT_AMNESIA || potion->otyp == POT_CANCELLATION) {
	    if (potion == obj) {
		obj->in_use = FALSE;
		potion = splitobj(obj, 1L);
		potion->in_use = TRUE;
	    }
	    if (get_wet(obj, TRUE)) goto poof;
	}
	/* WAC - Finn Theoderson - make polymorph and gain level msgs similar
	 * 	 Give out name of new object and allow user to name the potion
	 */
	/* KMH, balance patch -- idea by Dylan O'Donnell <dylanw@demon.net> */
	else if (potion->otyp == POT_GAIN_LEVEL && obj->oclass != POTION_CLASS) { /* this should fix it --Amy */
	/* thanks to the guy/girl figuring it out */
	    res = upgrade_obj(obj);

	    if (res != 0) {

		if (res == 1) { 
		     /* The object was upgraded */
		     pline("Hmm!  You don't recall dipping that into the potion.");
		     prinv((char *)0, obj, 0L);
		} /* else potion exploded */
		if (!objects[potion->otyp].oc_name_known &&
			!objects[potion->otyp].oc_uname)
		    docall(potion);
		useup(potion);
		update_inventory();
		exercise(A_WIS, TRUE);
		return(1);
	    }
	    /* no return here, go for Interesting... message */
	} else if (obj->otyp == POT_POLYMORPH || obj->otyp == POT_MUTATION ||
		potion->otyp == POT_POLYMORPH || potion->otyp == POT_MUTATION) {
	    /* some objects can't be polymorphed */
	    if (obj->otyp == potion->otyp ||	/* both POT_POLY */
		    obj->otyp == WAN_POLYMORPH ||
		    obj->otyp == WAN_MUTATION ||
		    obj->otyp == SPE_POLYMORPH ||
		    obj->otyp == SPE_MUTATION ||
		    obj == uball || obj == uskin ||
		    obj_resists( (obj->otyp == POT_POLYMORPH || obj->otyp == POT_MUTATION) ?
				potion : obj, 5, 95)) {
		pline(nothing_happens);
	    } else {
	    	boolean was_wep = FALSE, was_swapwep = FALSE, was_quiver = FALSE;
		short save_otyp = obj->otyp;
		/* KMH, conduct */
		u.uconduct.polypiles++;

		if (obj == uwep) was_wep = TRUE;
		else if (obj == uswapwep) was_swapwep = TRUE;
		else if (obj == uquiver) was_quiver = TRUE;

		obj = poly_obj(obj, STRANGE_OBJECT);

		if (was_wep) setuwep(obj, TRUE);
		else if (was_swapwep) setuswapwep(obj, TRUE);
		else if (was_quiver) setuqwep(obj);

		if (obj->otyp != save_otyp) {
			if (obj->otyp == POT_POLYMORPH) makeknown(POT_POLYMORPH);
			if (obj->otyp == POT_MUTATION) makeknown(POT_MUTATION);
			if (potion->otyp == POT_POLYMORPH) makeknown(POT_POLYMORPH);
			if (potion->otyp == POT_MUTATION) makeknown(POT_MUTATION);
			useup(potion);
			prinv((char *)0, obj, 0L);
			return 1;
		} else {
			pline("Nothing seems to happen.");
			goto poof;
		}
	    }
	    potion->in_use = FALSE;	/* didn't go poof */
	    return(1);
	} else if (potion->otyp == POT_RESTORE_ABILITY && is_hazy(obj)) {
		/* KMH -- Restore ability will stop unpolymorphing */
		if (!stack_too_big(obj)) {stop_timer(UNPOLY_OBJ, (void *) obj);
		obj->oldtyp = STRANGE_OBJECT;
		if (!Blind)
			pline("%s seems less hazy.", Yname2(obj));
		}
		useup(potion);
		return (1);
	} else if(obj->oclass == POTION_CLASS && obj->otyp != potion->otyp) {
		/* Mixing potions is dangerous... */
		pline_The("potions mix...");
		/* KMH, balance patch -- acid is particularly unstable */
		if ((obj->cursed || obj->otyp == POT_ACID ||
		    potion->cursed || potion->otyp == POT_ACID || !rn2(10) || (stack_too_big(obj) && stack_too_big(obj)) || (stack_too_big(potion) && stack_too_big(potion) ) ) && !(uarmc && uarmc->oartifact == ART_NO_MORE_EXPLOSIONS && !(obj->otyp == POT_ACID || potion->otyp == POT_ACID) ) ) {
			pline("BOOM!  They explode!");
			exercise(A_STR, FALSE);
			if (!breathless(youmonst.data) || haseyes(youmonst.data))
				potionbreathe(obj);
			useup(obj);
			useup(potion);
			/* MRKR: an alchemy smock ought to be */
			/* some protection against this: */
			losehp(Acid_resistance ? rnd(5) : rnd(10),
			       "alchemic blast", KILLED_BY_AN);
			return(1);
		}

		obj->blessed = obj->cursed = obj->bknown = 0;
		if (Blind || Hallucination) obj->dknown = 0;

		if ((mixture = mixtype(obj, potion)) != 0) {
			obj->otyp = mixture;
		} else {
		    switch (obj->odiluted ? 1 : rnd(8)) {
			case 1:
				obj->otyp = POT_WATER;
				break;
			case 2:
			case 3:
				obj->otyp = POT_SICKNESS;
				break;
			case 4:
				{
				  struct obj *otmp;
				  otmp = mkobj(POTION_CLASS,FALSE);
				  obj->otyp = otmp->otyp;
				  obfree(otmp, (struct obj *)0);
				}
				break;
			default:
				if (!Blind)
			  pline_The("mixture glows brightly and evaporates.");
				useup(obj);
				useup(potion);
				return(1);
		    }
		}

		obj->odiluted = (obj->otyp != POT_WATER);

		if (obj->otyp == POT_WATER && !Hallucination) {
			pline_The("mixture bubbles%s.",
				Blind ? "" : ", then clears");
		} else if (!Blind) {
			pline_The("mixture looks %s.",
				hcolor(OBJ_DESCR(objects[obj->otyp])));
		}

		useup(potion);
		return(1);
	}
	if (!always_visible(obj)) {
	    if (potion->otyp == POT_INVISIBILITY && !obj->oinvis && !obj->oinvisreal) {
		if (!stack_too_big(obj)) {obj->oinvis = TRUE; if (!rn2(100)) obj->oinvisreal = TRUE;
		if (!Blind)
		    pline(!See_invisible ? "Where did %s go?" :
			  "Gee!  All of a sudden you can see right through %s.",
			  the(xname(obj)));
		}
		goto poof;
	    } else if (potion->otyp == POT_SEE_INVISIBLE && obj->oinvis) {
		if (!stack_too_big(obj)) {obj->oinvis = FALSE; if (obj->oinvisreal) obj->oinvis = TRUE;
		if (!Blind) {
		    if (!See_invisible)
			pline("So that's where %s went!", the(xname(obj)));
		    else
			You("can no longer see through %s.",
				the(xname(obj)));
		}
		}
		goto poof;
	    }
	}

	if(is_poisonable(obj)) {
	    if( (potion->otyp == POT_SICKNESS || potion->otyp == POT_POISON) && !obj->opoisoned) {
		char buf[BUFSZ];
		if (potion->quan > 1L)
		    sprintf(buf, "One of %s", the(xname(potion)));
		else
		    strcpy(buf, The(xname(potion)));
		pline("%s forms a coating on %s.",
		      buf, the(xname(obj)));
		if (!stack_too_big(obj)) obj->opoisoned = TRUE;
		else pline("Unfortunately there wasn't enough poison in there.");
		goto poof;
	    } else if(obj->opoisoned &&
		      (potion->otyp == POT_HEALING ||
		       potion->otyp == POT_EXTRA_HEALING ||
		       potion->otyp == POT_FULL_HEALING)) {
		pline("A coating wears off %s.", the(xname(obj)));
		if (!stack_too_big(obj)) obj->opoisoned = 0;
		else pline("Unfortunately it wasn't enough to completely remove the poison.");
		goto poof;
	    }
	}

	if (potion->otyp == POT_OIL) {
	    boolean wisx = FALSE;
	    if (potion->lamplit) {	/* burning */
		int omat = objects[obj->otyp].oc_material;
		/* the code here should be merged with fire_damage */
		if (catch_lit(obj)) {
		    /* catch_lit does all the work if true */
		} else if (obj->oerodeproof || obj_resists(obj, 5, 95) ||
			   !is_flammable(obj) || obj->oclass == FOOD_CLASS) {
		    pline("%s %s to burn for a moment.",
			  Yname2(obj), otense(obj, "seem"));
		} else {
		    if ((omat == PLASTIC || omat == PAPER) && !obj->oartifact)
			obj->oeroded = MAX_ERODE;
		    pline_The("burning oil %s %s.",
			    obj->oeroded == MAX_ERODE ? "destroys" : "damages",
			    yname(obj));
		    if (obj->oeroded == MAX_ERODE) {
			obj_extract_self(obj);
			obfree(obj, (struct obj *)0);
			obj = (struct obj *) 0;
		    } else {
			/* we know it's carried */
			if (obj->unpaid) {
			    /* create a dummy duplicate to put on bill */
			    verbalize("You burnt it, you bought it!");
			    bill_dummy_object(obj);
			}
			obj->oeroded++;
		    }
		}
	    } else if (potion->cursed) {
		pline_The("potion spills and covers your %s with oil.",
			  makeplural(body_part(FINGER)));
		incr_itimeout(&Glib, d(2,10));
	    } else if (obj->oclass != WEAPON_CLASS && !is_weptool(obj)) {
		/* the following cases apply only to weapons */
		goto more_dips;
	    /* Oil removes rust and corrosion, but doesn't unburn.
	     * Arrows, etc are classed as metallic due to arrowhead
	     * material, but dipping in oil shouldn't repair them.
	     */
	    } else if ((!is_rustprone(obj) && !is_corrodeable(obj)) ||
			is_ammo(obj) || (!obj->oeroded && !obj->oeroded2)) {
		/* uses up potion, doesn't set obj->greased */
		pline("%s %s with an oily sheen.",
		      Yname2(obj), otense(obj, "gleam"));
	    } else {
		pline("%s %s less %s.",
		      Yname2(obj), otense(obj, "are"),
		      (obj->oeroded && obj->oeroded2) ? "corroded and rusty" :
			obj->oeroded ? "rusty" : "corroded");
		if (!stack_too_big(obj) && obj->oeroded > 0) obj->oeroded--;
		if (!stack_too_big(obj) && obj->oeroded2 > 0) obj->oeroded2--;
		wisx = TRUE;
	    }
	    exercise(A_WIS, wisx);
	    makeknown(potion->otyp);
	    useup(potion);
	    return 1;
	} else if (potion->otyp == POT_GAIN_LEVEL) {
	    res = upgrade_obj(obj);
	    if (res != 0) {
		if (res == 1) {
		    /* The object was upgraded */
		    pline("Hmm!  You don't recall dipping that into the potion.");
		    prinv((char *)0, obj, 0L);
		} /* else potion exploded */
		if (!objects[potion->otyp].oc_name_known &&
			!objects[potion->otyp].oc_uname)
		    docall(potion);
		useup(potion);
		update_inventory();
		exercise(A_WIS, TRUE);
		return 1;
	    }
	    /* no return here, go for Interesting... message */
	}

	/* KMH, balance patch -- acid affects damage(proofing) */
	if (potion->otyp == POT_ACID && (obj->oclass == ARMOR_CLASS ||
		obj->oclass == WEAPON_CLASS || is_weptool(obj))) {
	    if (!potion->blessed && obj->oerodeproof && !stack_too_big(obj) ) {
		pline("%s %s golden shield.",  Yname2(obj),
			(obj->quan > 1L) ? "lose their" : "loses its");
		obj->oerodeproof = 0;
		makeknown(potion->otyp);
	    } else {
		pline("%s looks a little dull.", Yname2(obj));
		if (!objects[potion->otyp].oc_name_known &&
			!objects[potion->otyp].oc_uname)
		    docall(potion);
	    }
	    exercise(A_WIS, FALSE);
  	    useup(potion);
	    return 1;
	}
    more_dips:

	/* Allow filling of MAGIC_LAMPs to prevent identification by player */
	if ((obj->otyp == OIL_LAMP || obj->otyp == MAGIC_LAMP) &&
	   (potion->otyp == POT_OIL)) {

	    /* Turn off engine before fueling, turn off fuel too :-)  */
	    if (obj->lamplit || potion->lamplit) {
		useup(potion);
		explode(u.ux, u.uy, ZT_SPELL(ZT_FIRE), d(6,6), 0, EXPL_FIERY);
		exercise(A_WIS, FALSE);
		return 1;
	    }
	    /* Adding oil to an empty magic lamp renders it into an oil lamp */
	    if ((obj->otyp == MAGIC_LAMP) && obj->spe == 0) {
		obj->otyp = OIL_LAMP;
		obj->age = 0;
	    }
	    if (obj->age > 1000L) {
                pline("%s %s full.", Yname2(obj), otense(obj, "are"));
		potion->in_use = FALSE; /* didn't go poof */
	    } else {
                You("fill your %s with oil.", yname(obj));
		check_unpaid(potion);	/* Yendorian Fuel Tax */
		obj->age += 2*potion->age;	/* burns more efficiently */
		if (obj->age > 1500L) obj->age = 1500L;
		useup(potion);
		exercise(A_WIS, TRUE);
	    }
	    makeknown(POT_OIL);
	    obj->spe = 1;
	    update_inventory();
	    return 1;
	}

	potion->in_use = FALSE;         /* didn't go poof */
	if ((obj->otyp == UNICORN_HORN || obj->oclass == GEM_CLASS) &&
	    (mixture = mixtype(obj, potion)) != 0) {
		char oldbuf[BUFSZ], newbuf[BUFSZ];
		short old_otyp = potion->otyp;
		boolean old_dknown = FALSE;
		boolean more_than_one = potion->quan > 1;

		oldbuf[0] = '\0';
		if (potion->dknown) {
		    old_dknown = TRUE;
		    sprintf(oldbuf, "%s ",
			    hcolor(OBJ_DESCR(objects[potion->otyp])));
		}
		/* with multiple merged potions, split off one and
		   just clear it */
		if (potion->quan > 1L) {
		    singlepotion = splitobj(potion, 1L);
		} else singlepotion = potion;
		
		/* MRKR: Gems dissolve in acid to produce new potions */

		if (obj->oclass == GEM_CLASS && potion->otyp == POT_ACID) {
		    struct obj *singlegem = (obj->quan > 1L ? 
					     splitobj(obj, 1L) : obj);

		    singlegem->in_use = TRUE;
		    if (potion->otyp == POT_ACID && 
		      (obj->otyp == DILITHIUM_CRYSTAL || 
		       potion->cursed || !rn2(10))) {
			/* Just to keep them on their toes */

			singlepotion->in_use = TRUE;
			if (Hallucination && obj->otyp == DILITHIUM_CRYSTAL) {
			    /* Thanks to Robin Johnson */
			    pline("Warning, Captain!  The warp core has been breached!");
			}
			pline("BOOM!  %s explodes!", The(xname(singlegem)));
			exercise(A_STR, FALSE);
			if (!breathless(youmonst.data) || haseyes(youmonst.data))
			    potionbreathe(singlepotion);
			useup(singlegem);
			useup(singlepotion);
			/* MRKR: an alchemy smock ought to be */
			/* some protection against this: */
			losehp(Acid_resistance ? rnd(5) : rnd(10), 
			       "alchemic blast", KILLED_BY_AN);
			return(1);	  
		    }

		    pline("%s dissolves in %s.", The(xname(singlegem)), 
			  the(xname(singlepotion)));
		    makeknown(POT_ACID);
		    useup(singlegem);
		}

		if(singlepotion->unpaid && costly_spot(u.ux, u.uy)) {
		    You("use it, you pay for it.");
		    bill_dummy_object(singlepotion);
		}

		if (singlepotion->otyp == mixture) {		  
		    /* no change - merge it back in */
		    if (more_than_one && !merged(&potion, &singlepotion)) {
			/* should never happen */
			impossible("singlepotion won't merge with parent potion.");
		    }
		} else {		  
		singlepotion->otyp = mixture;
		singlepotion->blessed = 0;
		if (mixture == POT_WATER)
		    singlepotion->cursed = singlepotion->odiluted = 0;
		else
		    singlepotion->cursed = obj->cursed;  /* odiluted left as-is */
		singlepotion->bknown = FALSE;
		if (Blind) {
		    singlepotion->dknown = FALSE;
		} else {
		    singlepotion->dknown = !Hallucination;
		    if (mixture == POT_WATER && singlepotion->dknown)
			sprintf(newbuf, "clears");
		    else
			sprintf(newbuf, "turns %s",
				hcolor(OBJ_DESCR(objects[mixture])));
		    pline_The("%spotion%s %s.", oldbuf,
			      more_than_one ? " that you dipped into" : "",
			      newbuf);
		    if(!objects[old_otyp].oc_uname &&
			!objects[old_otyp].oc_name_known && old_dknown) {
			struct obj fakeobj;
			fakeobj = zeroobj;
			fakeobj.dknown = 1;
			fakeobj.otyp = old_otyp;
			fakeobj.oclass = POTION_CLASS;
			docall(&fakeobj);
		    }
		}
		obj_extract_self(singlepotion);
		singlepotion = hold_another_object(singlepotion,
					"You juggle and drop %s!",
					doname(singlepotion), (const char *)0);
		update_inventory();
		}

		return(1);
	}

	pline(Hallucination ? "Colorful..." : "Interesting...");
	return(1);
}


void
djinni_from_bottle(obj, kind)
register struct obj *obj;
int kind;
{
	struct monst *mtmp;
	int genie_type;        
	int chance;

#if 0
	/* KMH -- See comments in monst.c */
	switch (rn2(4)) {
		default:
		case 0: genie_type = PM_DJINNI; break;
		case 1: genie_type = PM_EFREETI; break;
		case 2: genie_type = PM_MARID; break;
		case 3: genie_type = PM_DAO; break;
	}
#else
	if (kind == 1) genie_type = PM_DJINNI;
	else if (kind == 2) genie_type = PM_DAO;
	else if (kind == 3) genie_type = PM_EFREETI;
	else genie_type = PM_MARID;
#endif
	if(!(mtmp = makemon(&mons[genie_type], u.ux, u.uy, NO_MM_FLAGS))){
		pline("It turns out to be empty.");
		make_bottle(TRUE);
		return;
	}

	if (!Blind) {
		pline("In a cloud of smoke, %s emerges!", a_monnam(mtmp));
		pline("%s speaks.", Monnam(mtmp));
	} else {
		You("smell acrid fumes.");
		pline("%s speaks.", Something);
	}

	chance = rn2(5);
	if (obj->blessed) chance = (chance == 4) ? rnd(4) : 0;
	else if (obj->cursed) chance = (chance == 0) ? rn2(4) : 4;
	/* 0,1,2,3,4:  b=80%,5,5,5,5; nc=20%,20,20,20,20; c=5%,5,5,5,80 */

	switch (chance) {
	case 0 : verbalize("I am in your debt.  I will grant a boon!");
		if (!rn2(4)) makewish();
		else othergreateffect();
		mongone(mtmp);
		break;
	case 1 : verbalize("Thank you for freeing me!");
		(void) tamedog(mtmp, (struct obj *)0, FALSE);
		break;
	case 2 : verbalize("You freed me!");
		mtmp->mpeaceful = TRUE;
		set_malign(mtmp);
		break;
	case 3 : verbalize("It is about time!");
		pline("%s vanishes.", Monnam(mtmp));
		mongone(mtmp);
		break;
	default: verbalize("You disturbed me, fool!");
		break;
	}
	make_bottle(FALSE);
}

/* clone a gremlin or mold (2nd arg non-null implies heat as the trigger);
   hit points are cut in half (odd HP stays with original) */
struct monst *
split_mon(mon, mtmp)
struct monst *mon,	/* monster being split */
	     *mtmp;	/* optional attacker whose heat triggered it */
{
	struct monst *mtmp2;
	char reason[BUFSZ];

	reason[0] = '\0';
	if (mtmp) sprintf(reason, " from %s heat",
			  (mtmp == &youmonst) ? (const char *)"your" :
			      (const char *)s_suffix(mon_nam(mtmp)));

	if (mon == &youmonst) {
	    mtmp2 = cloneu();
	    if (mtmp2) {
		mtmp2->mhpmax = u.mhmax / 2;
		u.mhmax -= mtmp2->mhpmax;
		flags.botl = 1;
		You("multiply%s!", reason);
	    }
	} else {
	    mtmp2 = clone_mon(mon, 0, 0);
	    if (mtmp2) {
		mtmp2->mhpmax = mon->mhpmax / 2;
		mon->mhpmax -= mtmp2->mhpmax;
		if (canspotmon(mon))
		    pline("%s multiplies%s!", Monnam(mon), reason);
	    }
	}
	return mtmp2;
}

#endif /* OVLB */

/*potion.c*/

STATIC_PTR void
set_litI(x,y,val)
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


