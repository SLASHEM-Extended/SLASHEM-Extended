/*	SCCS Id: @(#)allmain.c	3.4	2003/04/02	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* various code that was replicated in *main.c */

#include "hack.h"
#ifndef NO_SIGNAL
#include <signal.h>
#endif
#include "patchlevel.h"

#include "qtext.h"

#ifdef POSITIONBAR
STATIC_DCL void do_positionbar(void);
#endif

STATIC_PTR int unfaintX(void);

#define decrnknow(spell)	spl_book[spell].sp_know--
#define spellid(spell)		spl_book[spell].sp_id
#define spellknow(spell)	spl_book[spell].sp_know
#define spellname(spell)	OBJ_NAME(objects[spellid(spell)])

static const char all_count[] = { ALLOW_COUNT, ALL_CLASSES, 0 };

static void p_glow2(struct obj *,const char *);

/* hunger texts used on bottom line (each 8 chars long) */
#define SATIATED	0
#define NOT_HUNGRY	1
#define HUNGRY		2
#define WEAK		3
#define FAINTING	4
#define FAINTED		5
#define STARVED		6

#ifdef OVL0

STATIC_PTR
int
unfaintX()
{
	(void) Hear_again();
	stop_occupation();
	return 0;
}

static void
p_glow2(otmp,color)
register struct obj	*otmp;
register const char *color;
{
	Your("%s %s%s%s for a moment.",
		xname(otmp),
		otense(otmp, Blind ? "vibrate" : "glow"),
		Blind ? "" : " ",
		Blind ? nul : hcolor(color));
}

void
moveloop()
{
#if defined(MICRO) || defined(WIN32)
    char ch;
    int abort_lev;
#endif
	struct obj *pobj; /* buildfix by EternalEye: pobj is used on all platforms */

	int regenrate; /* A level 30 player polymorphed into a large cat would otherwise regenerate waaaaaay too slowly. --Amy */

    int moveamt = 0, wtcap = 0, change = 0;
	int randsp;
	int randmnst;
	struct permonst *randmonstforspawn;
	int blackngdur;
	int monstercolor;
	int randmnsx;
	int i;
	int nastyitemchance;
	coord cc;
    int cx,cy;
	register struct obj *acqo;

    char buf[BUFSZ];
	char ebuf[BUFSZ];
    boolean didmove = FALSE, monscanmove = FALSE;
	register struct trap *ttmp;
    /* don't make it obvious when monsters will start speeding up */
    int monclock;
    int xtraclock;
    /*int timeout_start = rnz(10000)+rnz(15000);*/
    /*int clock_base = rnz(10000)+rnz(20000)+timeout_start;*/
	int timeout_start = u.monstertimeout;
	int clock_base = u.monstertimefinish; /* values set in u_init */
    int past_clock;
	/*u.monstertimeout = timeout_start;*/
	/*u.monstertimefinish = clock_base;*/

	struct obj *otmpi, *otmpii;

    flags.moonphase = phase_of_the_moon();
    if(flags.moonphase == FULL_MOON) {
	You(Hallucination ? "are on the moon tonight!" : "are lucky!  Full moon tonight.");
	change_luck(1);
    } else if(flags.moonphase == NEW_MOON) {
	pline("Be careful!  New moon tonight.");
	adjalign(-3); 
    } else if(flags.moonphase >= 1 && flags.moonphase <= 3) {
	pline("The moon is waxing tonight.");
    } else if(flags.moonphase >= 5 && flags.moonphase <= 7) {
	pline("The moon is waning tonight.");
    }
    flags.friday13 = friday_13th();
    if (flags.friday13) {
	pline("Watch out!  Bad things can happen on Friday the 13th.");
	change_luck(-1);
	adjalign(-10); 
    }
    /* KMH -- February 2 */
    flags.groundhogday = groundhog_day();
    if (flags.groundhogday)
	pline("Happy Groundhog Day!");

	if (getmonth() == 5) {
#ifdef PUBLIC_SERVER
		if (flags.uberlostsoul || flags.lostsoul || flags.gmmode || flags.supergmmode || flags.wonderland) {
			pline("Junethack is running - but you're using a playing mode that is incompatible with the tournament! The following modes are prohibited: lostsoul, uberlostsoul, gmmode and wonderland. If you want your games to count, quit this one now, disable all the forbidden options, and start a new game. Please refer to junethack.net for more information. Good luck!");
		} else {
			pline("Junethack is running! Please refer to junethack.net for more information. Give it your best shot, and try to score as many trophies as you can! Good luck!");
		}
#else
		pline("It is June! Why are you not participating in the Junethack tournament which traditionally runs this month? Quick, go to junethack.net in your web browser where you can play SLASH'EM Extended and other NetHack variants online and compare your scores with other players! And what's more, certain achievements will net you trophies!");
#endif
	}

    initrack();


    /* Note:  these initializers don't do anything except guarantee that
	    we're linked properly.
    */
    decl_init();
    monst_init();
    monstr_init();	/* monster strengths */
    objects_init();

    commands_init();

    (void) encumber_msg(); /* in case they auto-picked up something */
    if (defer_see_monsters) {
	defer_see_monsters = FALSE;
	see_monsters();
    }

    u.uz0.dlevel = u.uz.dlevel;
    youmonst.movement = NORMAL_SPEED;	/* give the hero some movement points */

    for(;;) {
	get_nh_event();
#ifdef POSITIONBAR
	do_positionbar();
#endif

	didmove = flags.move;
	if (TimerunBug || u.uprops[TIMERUN_BUG].extrinsic || have_timerunstone()) didmove = TRUE;
	if(didmove) {

		if (tech_inuse(T_COMBO_STRIKE)) {
			if (u.comboactive) u.comboactive = FALSE;
			else {
				u.comboactive = u.combostrike = 0;
				stopsingletechnique(T_COMBO_STRIKE);
				pline("Since you failed to hit something in your last action, your combo ends prematurely!");
			}
		} else { /* combo strike inactive - clear any combo strike flags that might still be present */
			u.comboactive = u.combostrike = 0;
		}

		u.aggravation = 0;
		/* note by Amy: It is not a bug that we're setting this variable to zero regardless of whether you currently
		 * have aggravate monster. The variable is used in several other places to ensure that summoned monsters are
		 * more dangerous, but we do not want aggravate monster to become a crippling status effect, and therefore
		 * randomly spawned monsters aren't supposed to be higher level only because you aggravate. */

		u.outtadepthtrap = 0;

	    /* actual time passed */
	    youmonst.movement -= NORMAL_SPEED;

	    do { /* hero can't move this turn loop */
		wtcap = encumber_msg();

		flags.mon_moving = TRUE;
		do {
		    monscanmove = movemon();
		    if (youmonst.movement > NORMAL_SPEED)
			break;	/* it's now your turn */
		} while (monscanmove);
		flags.mon_moving = FALSE;

		if (!monscanmove && youmonst.movement < NORMAL_SPEED) {
		    /* both you and the monsters are out of steam this round */
		    /* set up for a new turn */
		    struct monst *mtmp;
		    mcalcdistress();	/* adjust monsters' trap, blind, etc */

		    /* reallocate movement rations to monsters */
		    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
			mtmp->movement += mcalcmove(mtmp);

			 /* Vanilla generates a critter every 70-ish turns.
			  * The rate accelerates to every 50 or so below the Castle,
			  * and 'round every 25 turns once you've done the Invocation.
			  *
			  * We will push it even further.  Monsters post-Invocation
			  * will almost always appear on the stairs (if present), and 
			  * much more frequently; this, along with the extra intervene()
			  * calls, should certainly make it seem like you're wading back
			  * through the teeming hordes.
			  *
			  * Aside from that, a more general clock should be put on things;
			  * after about 30,000 turns, the frequency rate of appearance
			  * and (TODO) difficulty of monsters generated will slowly increase until
			  * it reaches the point it will be at as if you were post-Invocation.
			  *
			  * 80,000 turns should be adequate as a target mark for this effect;
			  * if you haven't ascended in 80,000 turns, you're intentionally
			  * fiddling around somewhere and will certainly be strong enough
			  * to handle anything that comes your way, so this won't be 
			  * dropping newbies off the edge of the planet.  -- DSR 12/2/07
			  */

			monclock = 70;

			if ((u.uevent.udemigod && u.amuletcompletelyimbued) || u.uprops[STORM_HELM].extrinsic) {
				monclock = 15;
			} else {
				if (depth(&u.uz) > depth(&stronghold_level)) {
					monclock = 60;
				}
				past_clock = moves - timeout_start;
				if (past_clock > 0) {
					monclock -= past_clock*40/clock_base;
				}
			}
			/* make sure we don't fall off the bottom */
			if (monclock < 30 && !(u.uevent.udemigod && u.amuletcompletelyimbued) && !u.uprops[STORM_HELM].extrinsic) { monclock = 30; }
			if (monclock < 15) { monclock = 15; }

			if (u.sterilized) monclock *= (5 + spell_damage_bonus(SPE_STERILIZE));

			if (verisiertEffect || u.uprops[VERISIERTEFFECT].extrinsic || have_verisiertstone()) monclock /= 5;
			if (uimplant && uimplant->oartifact == ART_YOU_SHOULD_SURRENDER) monclock /= 5;
			if (uarms && uarms->oartifact == ART_GOLDEN_DAWN) monclock /= 5;
			if (uarms && uarms->oartifact == ART_GREXIT_IS_NEAR) monclock /= 5;
			if (uarmf && uarmf->oartifact == ART_BLACK_DIAMOND_ICON) monclock /= 4;
			if (ishaxor) monclock /= 2;
			if (Race_if(PM_LICH_WARRIOR)) monclock /= 2;
			if (Race_if(PM_RODNEYAN)) monclock /= 4;
			if (uarmg && uarmg->oartifact == ART_DIFFICULTY__) monclock /= 2;
			if (issuxxor) monclock *= 2;

			if (Race_if(PM_DEVELOPER) && ((u.ulevel > 9) || (moves > 10000)) ) monclock /= 3;

			/* here, we really need a fail safe --Amy */
			if (monclock < 2) monclock = 2;

			/* TODO: adj difficulty in makemon */
			if (!rn2(monclock) && !ishomicider ) {
				if ( ((u.uevent.udemigod && u.amuletcompletelyimbued) || u.uprops[STORM_HELM].extrinsic) && xupstair && rn2(10)) {
					(void) makemon((struct permonst *)0, xupstair, yupstair, MM_ADJACENTOK);
				} else {
					(void) makemon((struct permonst *)0, 0, 0, NO_MM_FLAGS);
				}
			}

			if (!rn2(monclock) && ishomicider ) makerandomtrap();

			xtraclock = 100000;
			if ((u.uevent.udemigod && u.amuletcompletelyimbued) || u.uprops[STORM_HELM].extrinsic) {
				xtraclock = 30000;
			} else {
				if (depth(&u.uz) > depth(&stronghold_level)) {
					xtraclock = 80000;
				}
				past_clock = moves - timeout_start;
				if (past_clock > 0) {
					xtraclock -= past_clock*50000/clock_base;
				}
			}
			/* make sure we don't fall off the bottom */
			if (xtraclock < 50000 && !(u.uevent.udemigod && u.amuletcompletelyimbued) && !u.uprops[STORM_HELM].extrinsic) { xtraclock = 50000; }
			if (xtraclock < 30000) { xtraclock = 30000; }

			if (u.sterilized) xtraclock *= (5 + spell_damage_bonus(SPE_STERILIZE));

			if (verisiertEffect || u.uprops[VERISIERTEFFECT].extrinsic || have_verisiertstone()) xtraclock /= 5;
			if (uimplant && uimplant->oartifact == ART_YOU_SHOULD_SURRENDER) xtraclock /= 5;
			if (uarms && uarms->oartifact == ART_GOLDEN_DAWN) xtraclock /= 5;
			if (uarms && uarms->oartifact == ART_GREXIT_IS_NEAR) xtraclock /= 5;
			if (uarmf && uarmf->oartifact == ART_BLACK_DIAMOND_ICON) xtraclock /= 4;
			if (ishaxor) xtraclock /= 2;
			if (Race_if(PM_LICH_WARRIOR)) xtraclock /= 2;
			if (Race_if(PM_RODNEYAN)) xtraclock /= 4;
			if (uarmg && uarmg->oartifact == ART_DIFFICULTY__) monclock /= 2;
			if (issuxxor) xtraclock *= 2;

			if (Race_if(PM_DEVELOPER) && ((u.ulevel > 9) || (moves > 10000)) ) xtraclock /= 3;

			/* new group spawn system by Amy */
			if (!rn2(xtraclock) && !rn2(2) && !issoviet ) {

				randsp = (rn2(14) + 2);
				if (!rn2(10)) randsp *= 2;
				if (!rn2(100)) randsp *= 3;
				if (!rn2(1000)) randsp *= 5;
				if (!rn2(10000)) randsp *= 10;
				randmnst = (rn2(187) + 1);
				randmnsx = (rn2(100) + 1);

				if (wizard || !rn2(10)) pline(Hallucination ? "You fear that you'll get a segmentation fault on your next turn!" : "You suddenly feel a surge of tension!");

			for (i = 0; i < randsp; i++) {
			/* This function will fill the map with a random amount of monsters of one class. --Amy */

			if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

			if (randmnst < 6)
		 	    (void) makemon(mkclass(S_ANT,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 9)
		 	    (void) makemon(mkclass(S_BLOB,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 11)
		 	    (void) makemon(mkclass(S_COCKATRICE,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 15)
		 	    (void) makemon(mkclass(S_DOG,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 18)
		 	    (void) makemon(mkclass(S_EYE,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 22)
		 	    (void) makemon(mkclass(S_FELINE,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 24)
		 	    (void) makemon(mkclass(S_GREMLIN,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 29)
		 	    (void) makemon(mkclass(S_HUMANOID,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 33)
		 	    (void) makemon(mkclass(S_IMP,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 36)
		 	    (void) makemon(mkclass(S_JELLY,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 41)
		 	    (void) makemon(mkclass(S_KOBOLD,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 44)
		 	    (void) makemon(mkclass(S_LEPRECHAUN,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 47)
		 	    (void) makemon(mkclass(S_MIMIC,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 50)
		 	    (void) makemon(mkclass(S_NYMPH,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 54)
		 	    (void) makemon(mkclass(S_ORC,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 55)
		 	    (void) makemon(mkclass(S_PIERCER,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 58)
		 	    (void) makemon(mkclass(S_QUADRUPED,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 62)
		 	    (void) makemon(mkclass(S_RODENT,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 65)
		 	    (void) makemon(mkclass(S_SPIDER,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 66)
		 	    (void) makemon(mkclass(S_TRAPPER,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 69)
		 	    (void) makemon(mkclass(S_UNICORN,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 71)
		 	    (void) makemon(mkclass(S_VORTEX,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 73)
		 	    (void) makemon(mkclass(S_WORM,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 75)
		 	    (void) makemon(mkclass(S_XAN,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 76)
		 	    (void) makemon(mkclass(S_LIGHT,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 77)
		 	    (void) makemon(mkclass(S_ZOUTHERN,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 78)
		 	    (void) makemon(mkclass(S_ANGEL,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 81)
		 	    (void) makemon(mkclass(S_BAT,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 83)
		 	    (void) makemon(mkclass(S_CENTAUR,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 86)
		 	    (void) makemon(mkclass(S_DRAGON,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 89)
		 	    (void) makemon(mkclass(S_ELEMENTAL,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 94)
		 	    (void) makemon(mkclass(S_FUNGUS,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 99)
		 	    (void) makemon(mkclass(S_GNOME,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 102)
		 	    (void) makemon(mkclass(S_GIANT,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 103)
		 	    (void) makemon(mkclass(S_JABBERWOCK,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 104)
		 	    (void) makemon(mkclass(S_KOP,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 105)
		 	    (void) makemon(mkclass(S_LICH,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 108)
		 	    (void) makemon(mkclass(S_MUMMY,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 110)
		 	    (void) makemon(mkclass(S_NAGA,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 113)
		 	    (void) makemon(mkclass(S_OGRE,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 115)
		 	    (void) makemon(mkclass(S_PUDDING,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 116)
		 	    (void) makemon(mkclass(S_QUANTMECH,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 118)
		 	    (void) makemon(mkclass(S_RUSTMONST,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 121)
		 	    (void) makemon(mkclass(S_SNAKE,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 123)
		 	    (void) makemon(mkclass(S_TROLL,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 124)
		 	    (void) makemon(mkclass(S_UMBER,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 125)
		 	    (void) makemon(mkclass(S_VAMPIRE,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 127)
		 	    (void) makemon(mkclass(S_WRAITH,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 128)
		 	    (void) makemon(mkclass(S_XORN,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 130)
		 	    (void) makemon(mkclass(S_YETI,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 135)
		 	    (void) makemon(mkclass(S_ZOMBIE,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 145)
		 	    (void) makemon(mkclass(S_HUMAN,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 147)
		 	    (void) makemon(mkclass(S_GHOST,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 149)
		 	    (void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 152)
		 	    (void) makemon(mkclass(S_DEMON,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 155)
		 	    (void) makemon(mkclass(S_EEL,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 160)
		 	    (void) makemon(mkclass(S_LIZARD,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 162)
		 	    (void) makemon(mkclass(S_BAD_FOOD,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 165)
		 	    (void) makemon(mkclass(S_BAD_COINS,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 166) {
				if (randmnsx < 96)
		 	    (void) makemon(mkclass(S_HUMAN,0), 0, 0, NO_MM_FLAGS);
				else
		 	    (void) makemon(mkclass(S_NEMESE,0), 0, 0, NO_MM_FLAGS);
				}
			else if (randmnst < 171)
		 	    (void) makemon(mkclass(S_GRUE,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 176)
		 	    (void) makemon(mkclass(S_WALLMONST,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 180)
		 	    (void) makemon(mkclass(S_RUBMONST,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 181) {
				if (randmnsx < 99)
		 	    (void) makemon(mkclass(S_HUMAN,0), 0, 0, NO_MM_FLAGS);
				else
		 	    (void) makemon(mkclass(S_ARCHFIEND,0), 0, 0, NO_MM_FLAGS);
				}
			else if (randmnst < 186)
		 	    (void) makemon(mkclass(S_TURRET,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 187)
		 	    (void) makemon(mkclass(S_FLYFISH,0), 0, 0, NO_MM_FLAGS);
			else
		 	    (void) makemon((struct permonst *)0, 0, 0, NO_MM_FLAGS);

				}
			}

			if (!rn2(xtraclock) && !rn2(2) && !issoviet ) {

				randsp = (rn2(14) + 2);
				if (!rn2(10)) randsp *= 2;
				if (!rn2(100)) randsp *= 3;
				if (!rn2(1000)) randsp *= 5;
				if (!rn2(10000)) randsp *= 10;
				randmnst = (rn2(187) + 1);
				randmnsx = (rn2(100) + 1);
			      cx = rn2(COLNO);
			      cy = rn2(ROWNO);

				if (wizard || !rn2(10)) pline(Hallucination ? "You fear that you'll get a segmentation fault on your next turn!" : "You suddenly feel a surge of tension!");

			for (i = 0; i < randsp; i++) {
			/* This function will fill the map with a random amount of monsters of one class. --Amy */

			if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

			if (randmnst < 6)
		 	    (void) makemon(mkclass(S_ANT,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 9)
		 	    (void) makemon(mkclass(S_BLOB,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 11)
		 	    (void) makemon(mkclass(S_COCKATRICE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 15)
		 	    (void) makemon(mkclass(S_DOG,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 18)
		 	    (void) makemon(mkclass(S_EYE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 22)
		 	    (void) makemon(mkclass(S_FELINE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 24)
		 	    (void) makemon(mkclass(S_GREMLIN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 29)
		 	    (void) makemon(mkclass(S_HUMANOID,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 33)
		 	    (void) makemon(mkclass(S_IMP,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 36)
		 	    (void) makemon(mkclass(S_JELLY,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 41)
		 	    (void) makemon(mkclass(S_KOBOLD,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 44)
		 	    (void) makemon(mkclass(S_LEPRECHAUN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 47)
		 	    (void) makemon(mkclass(S_MIMIC,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 50)
		 	    (void) makemon(mkclass(S_NYMPH,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 54)
		 	    (void) makemon(mkclass(S_ORC,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 55)
		 	    (void) makemon(mkclass(S_PIERCER,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 58)
		 	    (void) makemon(mkclass(S_QUADRUPED,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 62)
		 	    (void) makemon(mkclass(S_RODENT,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 65)
		 	    (void) makemon(mkclass(S_SPIDER,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 66)
		 	    (void) makemon(mkclass(S_TRAPPER,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 69)
		 	    (void) makemon(mkclass(S_UNICORN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 71)
		 	    (void) makemon(mkclass(S_VORTEX,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 73)
		 	    (void) makemon(mkclass(S_WORM,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 75)
		 	    (void) makemon(mkclass(S_XAN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 76)
		 	    (void) makemon(mkclass(S_LIGHT,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 77)
		 	    (void) makemon(mkclass(S_ZOUTHERN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 78)
		 	    (void) makemon(mkclass(S_ANGEL,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 81)
		 	    (void) makemon(mkclass(S_BAT,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 83)
		 	    (void) makemon(mkclass(S_CENTAUR,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 86)
		 	    (void) makemon(mkclass(S_DRAGON,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 89)
		 	    (void) makemon(mkclass(S_ELEMENTAL,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 94)
		 	    (void) makemon(mkclass(S_FUNGUS,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 99)
		 	    (void) makemon(mkclass(S_GNOME,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 102)
		 	    (void) makemon(mkclass(S_GIANT,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 103)
		 	    (void) makemon(mkclass(S_JABBERWOCK,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 104)
		 	    (void) makemon(mkclass(S_KOP,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 105)
		 	    (void) makemon(mkclass(S_LICH,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 108)
		 	    (void) makemon(mkclass(S_MUMMY,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 110)
		 	    (void) makemon(mkclass(S_NAGA,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 113)
		 	    (void) makemon(mkclass(S_OGRE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 115)
		 	    (void) makemon(mkclass(S_PUDDING,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 116)
		 	    (void) makemon(mkclass(S_QUANTMECH,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 118)
		 	    (void) makemon(mkclass(S_RUSTMONST,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 121)
		 	    (void) makemon(mkclass(S_SNAKE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 123)
		 	    (void) makemon(mkclass(S_TROLL,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 124)
		 	    (void) makemon(mkclass(S_UMBER,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 125)
		 	    (void) makemon(mkclass(S_VAMPIRE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 127)
		 	    (void) makemon(mkclass(S_WRAITH,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 128)
		 	    (void) makemon(mkclass(S_XORN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 130)
		 	    (void) makemon(mkclass(S_YETI,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 135)
		 	    (void) makemon(mkclass(S_ZOMBIE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 145)
		 	    (void) makemon(mkclass(S_HUMAN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 147)
		 	    (void) makemon(mkclass(S_GHOST,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 149)
		 	    (void) makemon(mkclass(S_GOLEM,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 152)
		 	    (void) makemon(mkclass(S_DEMON,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 155)
		 	    (void) makemon(mkclass(S_EEL,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 160)
		 	    (void) makemon(mkclass(S_LIZARD,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 162)
		 	    (void) makemon(mkclass(S_BAD_FOOD,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 165)
		 	    (void) makemon(mkclass(S_BAD_COINS,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 166) {
				if (randmnsx < 96)
		 	    (void) makemon(mkclass(S_HUMAN,0), cx, cy, MM_ADJACENTOK);
				else
		 	    (void) makemon(mkclass(S_NEMESE,0), cx, cy, MM_ADJACENTOK);
				}
			else if (randmnst < 171)
		 	    (void) makemon(mkclass(S_GRUE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 176)
		 	    (void) makemon(mkclass(S_WALLMONST,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 180)
		 	    (void) makemon(mkclass(S_RUBMONST,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 181) {
				if (randmnsx < 99)
		 	    (void) makemon(mkclass(S_HUMAN,0), cx, cy, MM_ADJACENTOK);
				else
		 	    (void) makemon(mkclass(S_ARCHFIEND,0), cx, cy, MM_ADJACENTOK);
				}
			else if (randmnst < 186)
		 	    (void) makemon(mkclass(S_TURRET,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 187)
		 	    (void) makemon(mkclass(S_FLYFISH,0), cx, cy, MM_ADJACENTOK);
			else
		 	    (void) makemon((struct permonst *)0, cx, cy, MM_ADJACENTOK);

				}
			}

	/* "Put AmyBSOD's extra spawn system into a define. Because some people aren't a fan of having liches on dlvl1." In Soviet Russia, some people aren't a fan of games where the player can actually lose. They want to win all the time, so they go through the entirety of the game's code and remove every little bit of possible difficulty. --Amy */

			if (!rn2(ishaxor ? 5000 : 10000) && (moves < 10000 || rn2(3)) && !issoviet) {

				randsp = (rn2(14) + 2);
				if (!rn2(10)) randsp *= 2;
				if (!rn2(100)) randsp *= 3;
				if (!rn2(1000)) randsp *= 5;
				if (!rn2(10000)) randsp *= 10;
				if (randsp > 1) randsp = rnd(randsp);

				if (wizard || !rn2(10)) pline(Hallucination ? "You feel that someone reseeded the RNG!" : "You feel that someone was busy hiding treasure!");

				for (i = 0; i < randsp; i++) {

					if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

					if (timebasedlowerchance()) (void) makemon(&mons[PM_ITEM_MASTER], 0, 0, NO_MM_FLAGS);
				}

			}

			if (!rn2(ishaxor ? 125000 : 250000) && (moves < 10000 || rn2(3)) && (moves < 50000 || !rn2(3)) && !issoviet) {

				randsp = (rn2(14) + 2);
				if (!rn2(10)) randsp *= 2;
				if (!rn2(100)) randsp *= 3;
				if (!rn2(1000)) randsp *= 5;
				if (!rn2(10000)) randsp *= 10;
				if (randsp > 1) randsp = rnd(randsp);

				if (wizard || !rn2(10)) pline(Hallucination ? "You feel that the RNG loves you!" : "You feel that someone was busy hiding useful items!");

				for (i = 0; i < randsp; i++) {

					if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

					if (timebasedlowerchance() && timebasedlowerchance()) (void) makemon(&mons[PM_GOOD_ITEM_MASTER], 0, 0, NO_MM_FLAGS);
				}

			}

			if (u.ualign.sins && !rn2(100) && (rn2(2000) < u.ualign.sins ) ) {

				if (!rn2(20)) u.copwantedlevel += rnz(u.ualign.sins + 1);

				int copcnt = rnd(monster_difficulty() ) + 1;
				if (rn2(5)) copcnt = (copcnt / (rnd(4) + 1)) + 1;
				if (Role_if(PM_CAMPERSTRIKER)) copcnt *= (rn2(5) ? 2 : rn2(5) ? 3 : 5);

				if (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "anti-government helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "antipravitel'stvennaya shlem") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "aksil-hukumat dubulg'a") ) ) {
					copcnt = (copcnt / 2) + 1;
				}

				if (RngeAntiGovernment) {
					copcnt = (copcnt / 2) + 1;
				}

				int tryct = 0;
				int x, y;

				for (tryct = 0; tryct < 2000; tryct++) {
					x = rn1(COLNO-3,2);
					y = rn2(ROWNO);

					if (x && y && isok(x, y) && (levl[x][y].typ > DBWALL) && !(t_at(x, y)) ) {
						(void) maketrap(x, y, KOP_CUBE, 0);
						break;
						}
				}

			      while(--copcnt >= 0) {
					if (xupstair) (void) makemon(mkclass(S_KOP,0), xupstair, yupstair, MM_ANGRY|MM_ADJACENTOK);
					else (void) makemon(mkclass(S_KOP,0), 0, 0, MM_ANGRY|MM_ADJACENTOK);
				} /* while */

			}

			if (u.copwantedlevel) {

				u.copwantedlevel--;
				if (u.copwantedlevel < 0) u.copwantedlevel = 0; /* fail safe */

				if ( !(rn2(2) && (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "anti-government helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "antipravitel'stvennaya shlem") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "aksil-hukumat dubulg'a") ))) && !(rn2(2) && RngeAntiGovernment) && !rn2(100)) {

					(void) makemon(mkclass(S_KOP,0), 0, 0, MM_ANGRY|MM_ADJACENTOK);
					if (!rn2(200)) {
						switch (rnd(5)) {
							case 1:
								if (level_difficulty() > 39) (void) makemon(&mons[PM_EXMINATOR_KOP], 0, 0, MM_ANGRY|MM_ADJACENTOK);
								else (void) makemon(&mons[PM_WHITLEVEL_KOP], 0, 0, MM_ANGRY|MM_ADJACENTOK);
								break;
							case 2:
								if (level_difficulty() > 29) (void) makemon(&mons[PM_RNGED_KOP], 0, 0, MM_ANGRY|MM_ADJACENTOK);
								else (void) makemon(&mons[PM_WHITLEVEL_KOP], 0, 0, MM_ANGRY|MM_ADJACENTOK);
								break;
							case 3:
								if (level_difficulty() > 19) (void) makemon(&mons[PM_GREYLEVEL_KOP], 0, 0, MM_ANGRY|MM_ADJACENTOK);
								else (void) makemon(&mons[PM_WHITLEVEL_KOP], 0, 0, MM_ANGRY|MM_ADJACENTOK);
								break;
							case 4:
								if (level_difficulty() > 9) (void) makemon(&mons[PM_BLAKLEVEL_KOP], 0, 0, MM_ANGRY|MM_ADJACENTOK);
								else (void) makemon(&mons[PM_WHITLEVEL_KOP], 0, 0, MM_ANGRY|MM_ADJACENTOK);
								break;
							case 5:
								(void) makemon(&mons[PM_WHITLEVEL_KOP], 0, 0, MM_ANGRY|MM_ADJACENTOK);
								break;
						}
					}

				}

			}

			if (moves == u.ascensionfirsthint) {
				pline("Reminder: You have a limited amount of time for ascending. This is not a joke.");
				pline("Currently you still have more than %d turns left though, so don't panic.", (u.ascensionfirsthint * 4) );
				pline("However, keep in mind that you're not supposed to be farming forever. Okay?");

			}

			if (moves == u.ascensionsecondhint) {
				pline("Remember, you're not supposed to dilly-dally all the time! You're supposed to work towards ascending!");
				pline("Currently you have less than %d turns left, so better move on!", u.ascensionsecondhint);
				pline("If you don't ascend in time, the RNG will get angry, and you don't want that to happen!");

			}

			if (moves == u.ascensiontimelimit) {
				pline("You exceeded the maximum permissible amount of turns for winning the game!");
				pline("Now, the RNG is fed up with your shenanigans, and decides to make the game much more difficult.");
				pline("If you hurry up, you may still be able to ascend, but it will get harder the longer you procrastinate.");
			}

			if (moves > u.ascensiontimelimit) {

				int annoyancefactor = 200 * u.ascensiontimelimit / moves;
				if (ishaxor) annoyancefactor /= 2;
				if (annoyancefactor > 200) annoyancefactor = 200;
				if (annoyancefactor < 2) annoyancefactor = 2;

				if (!rn2(annoyancefactor)) badeffect();

			}

			nastyitemchance = 250000;

			if (moves > 5000) nastyitemchance = 225000;
			if (moves > 10000) nastyitemchance = 200000;
			if (moves > 20000) nastyitemchance = 175000;
			if (moves > 40000) nastyitemchance = 150000;
			if (moves > 80000) nastyitemchance = 135000;
			if (moves > 120000) nastyitemchance = 125000;
			if (moves > 160000) nastyitemchance = 110000;
			if (moves > 240000) nastyitemchance = 100000;
			if (moves > 320000) nastyitemchance = 87500;
			if (moves > 400000) nastyitemchance = 75000;
			if (moves > 600000) nastyitemchance = 62500;
			if (moves > 800000) nastyitemchance = 50000;
			if (moves > 1000000) nastyitemchance = 37500;
			if (moves > 2000000) nastyitemchance = 25000;
			if (moves > 3000000) nastyitemchance = 10000;
			if (moves > 4000000) nastyitemchance = 7500;
			if (moves > 5000000) nastyitemchance = 5000;

			if (ishaxor) nastyitemchance /= 2;

			if (!rn2(nastyitemchance) && !issoviet) {

				randsp = (rn3(14) + 2);
				if (!rn2(10)) randsp *= 2;
				if (!rn2(100)) randsp *= 3;
				if (!rn2(1000)) randsp *= 5;
				if (!rn2(10000)) randsp *= 10;
				if (randsp > 1) randsp = rnd(randsp);

				if (wizard || !rn2(10)) pline(Hallucination ? "You feel that the RNG hates you!" : "You feel that the monsters are coming for you with everything they got!");

				for (i = 0; i < randsp; i++) {

					if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

					(void) makemon(&mons[PM_BAD_ITEM_MASTER], 0, 0, NO_MM_FLAGS);

					if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

			 	      (void) makemon((struct permonst *)0, 0, 0, MM_ADJACENTOK);

					if (!rn2(2)) {

						if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				 	      (void) makemon((struct permonst *)0, 0, 0, MM_ADJACENTOK);

					}
				}

			}

			if (!rn2(xtraclock) && !rn2(2) && !issoviet) { /* group of one single monster species --Amy */

				randsp = (rn2(14) + 2);
				if (!rn2(10)) randsp *= 2;
				if (!rn2(100)) randsp *= 3;
				if (!rn2(1000)) randsp *= 5;
				if (!rn2(10000)) randsp *= 10;
				randmonstforspawn = rndmonst();

				if (wizard || !rn2(10)) pline(Hallucination ? "Someone hacked into the game data files to prevent you from ascending!" : "You feel the arrival of monsters!");

				for (i = 0; i < randsp; i++) {

					if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

					(void) makemon(randmonstforspawn, 0, 0, NO_MM_FLAGS);
				}

			}

			if (!rn2(xtraclock) && !rn2(2) && !issoviet) { /* group of colored monster species --Amy */

				randsp = (rn2(14) + 2);
				if (!rn2(10)) randsp *= 2;
				if (!rn2(100)) randsp *= 3;
				if (!rn2(1000)) randsp *= 5;
				if (!rn2(10000)) randsp *= 10;
				monstercolor = rnd(15);
				do { monstercolor = rnd(15); } while (monstercolor == CLR_BLUE);

				if (wizard || !rn2(10)) pline(Hallucination ? "The colors, the colors!" : "You feel a colorful sensation!");

				for (i = 0; i < randsp; i++) {

					if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

					(void) makemon(colormon(monstercolor), 0, 0, NO_MM_FLAGS);
				}
			}

			if (!rn2(xtraclock) && !rn2(2) && !issoviet) { /* group of themed monster species --Amy */

				randsp = (rn2(14) + 2);
				if (!rn2(10)) randsp *= 2;
				if (!rn2(100)) randsp *= 3;
				if (!rn2(1000)) randsp *= 5;
				if (!rn2(10000)) randsp *= 10;
				monstercolor = rnd(343);

				if (wizard || !rn2(10)) pline(Hallucination ? "Someone got in here! Who could that be?" : "You feel that a group has arrived!");

				for (i = 0; i < randsp; i++) {

					if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

					(void) makemon(specialtensmon(monstercolor), 0, 0, NO_MM_FLAGS);
				}
			}

			if (!rn2(xtraclock) && !rn2(2) && !issoviet) { /* group of one single monster species --Amy */

				randsp = (rn2(14) + 2);
				if (!rn2(10)) randsp *= 2;
				if (!rn2(100)) randsp *= 3;
				if (!rn2(1000)) randsp *= 5;
				if (!rn2(10000)) randsp *= 10;
				randmonstforspawn = rndmonst();
			      cx = rn2(COLNO);
			      cy = rn2(ROWNO);

				if (wizard || !rn2(10)) pline(Hallucination ? "Someone hacked into the game data files to prevent you from ascending!" : "You feel the arrival of monsters!");

				for (i = 0; i < randsp; i++) {

					if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

					(void) makemon(randmonstforspawn, cx, cy, MM_ADJACENTOK);
				}

			}

			if (!rn2(xtraclock) && !rn2(2) && !issoviet) { /* group of colored monster species --Amy */

				randsp = (rn2(14) + 2);
				if (!rn2(10)) randsp *= 2;
				if (!rn2(100)) randsp *= 3;
				if (!rn2(1000)) randsp *= 5;
				if (!rn2(10000)) randsp *= 10;
				monstercolor = rnd(15);
				do { monstercolor = rnd(15); } while (monstercolor == CLR_BLUE);
			      cx = rn2(COLNO);
			      cy = rn2(ROWNO);

				if (wizard || !rn2(10)) pline(Hallucination ? "The colors, the colors!" : "You feel a colorful sensation!");

				for (i = 0; i < randsp; i++) {

					if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

					(void) makemon(colormon(monstercolor), cx, cy, MM_ADJACENTOK);
				}
			}

			if (!rn2(xtraclock) && !rn2(2) && !issoviet) { /* group of themed monster species --Amy */

				randsp = (rn2(14) + 2);
				if (!rn2(10)) randsp *= 2;
				if (!rn2(100)) randsp *= 3;
				if (!rn2(1000)) randsp *= 5;
				if (!rn2(10000)) randsp *= 10;
				monstercolor = rnd(343);
			      cx = rn2(COLNO);
			      cy = rn2(ROWNO);

				if (wizard || !rn2(10)) pline(Hallucination ? "Someone got in here! Who could that be?" : "You feel that a group has arrived!");

				for (i = 0; i < randsp; i++) {

					if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

					(void) makemon(specialtensmon(monstercolor), cx, cy, MM_ADJACENTOK);
				}
			}

			if (uarmf && uarmf->otyp == ZIPPER_BOOTS && !EWounded_legs) EWounded_legs = 1;

			/* small chance of scaring yourself if you stand on Elbereth, even if you engraved it --Amy */
			if (sengr_at("Elbereth", u.ux, u.uy) && !rn2(100) && !Blind ) {
				pline("As you see the Elder Sign written on the %s at your %s, you suddenly panic!",surface(u.ux,u.uy), makeplural(body_part(FOOT)) );
				make_feared(HFeared + rnd(10 + (monster_difficulty()) ),TRUE);
				}

		    if(!rn2( ((u.uevent.udemigod && u.amuletcompletelyimbued) || u.uprops[STORM_HELM].extrinsic) ? 250 :
			    (depth(&u.uz) > depth(&stronghold_level)) ? 450 : 500)) {
			if (!ishomicider) (void) makemon((struct permonst *)0, 0, 0, NO_MM_FLAGS);
			else makerandomtrap();
			}

	/* still keeping the old monstermaking routine up, but drastically reducing their spawn rate. --Amy */

		    /* calculate how much time passed. */
		    if (u.usteed && u.umoved) {
			/* your speed doesn't augment steed's speed */
			moveamt = mcalcmove(u.usteed);
			register int steedmultiplier = 5;
			register int speedreduction;

			/* Riding a really fast (speed higher than 24) steed does not necessarily allow you to ride it at
			 * full speed, but depends on your riding skill. The actual speed will never be lower than 24, but
			 * now you need grand master riding skill to ride the steed at its actual speed. --Amy */

			if (!(PlayerCannotUseSkills)) {

				if (P_SKILL(P_RIDING) == P_BASIC) steedmultiplier = 7;
				if (P_SKILL(P_RIDING) == P_SKILLED) steedmultiplier = 9;
				if (P_SKILL(P_RIDING) == P_EXPERT) steedmultiplier = 11;
				if (P_SKILL(P_RIDING) == P_MASTER) steedmultiplier = 13;
				if (P_SKILL(P_RIDING) == P_GRAND_MASTER) steedmultiplier = 15;
				if (P_SKILL(P_RIDING) == P_SUPREME_MASTER) steedmultiplier = 15;

			}

			if (uimplant && uimplant->oartifact == ART_READY_FOR_A_RIDE) {
				moveamt *= 6;
				moveamt /= 5;
			}

			if (moveamt > 24) {
				speedreduction = (moveamt - 24);
				speedreduction *= steedmultiplier;
				speedreduction /= 15;
				moveamt = 24 + speedreduction;
			}

		    } else
		    {
			moveamt = youmonst.data->mmove;

			register int polymultiplier = 5;
			register int speedreduction;

			/* Polymorphing into a really fast (speed higher than 24) monster does not necessarily allow you to
			 * move at full speed, but depends on your polymorphing skill. The actual speed will never be lower
			 * than 24, but now you need grand master polymorphing skill to move at the actual speed of whatever
			 * monster you polymorphed into. --Amy */

			if (!(PlayerCannotUseSkills)) {

				if (P_SKILL(P_POLYMORPHING) == P_BASIC) polymultiplier = 7;
				if (P_SKILL(P_POLYMORPHING) == P_SKILLED) polymultiplier = 9;
				if (P_SKILL(P_POLYMORPHING) == P_EXPERT) polymultiplier = 11;
				if (P_SKILL(P_POLYMORPHING) == P_MASTER) polymultiplier = 13;
				if (P_SKILL(P_POLYMORPHING) == P_GRAND_MASTER) polymultiplier = 15;
				if (P_SKILL(P_POLYMORPHING) == P_SUPREME_MASTER) polymultiplier = 15;

			}

			if (Upolyd && (moveamt > 24)) {
				speedreduction = (moveamt - 24);
				speedreduction *= polymultiplier;
				speedreduction /= 15;
				moveamt = 24 + speedreduction;
			}


			if (youmonst.data->mmove == 0 && !rn2(2)) moveamt += 1; /* be lenient if an ungenomold player is unlucky 		 * enough to poly into a red mold or something like that. Otherwise they would simply die with no chance.
		 * see hack.c code that still prevents movement if polymorphed into something sessile.
		 * Also, you're still slower than a lichen (speed 1), so this should be ok. */

			if (Race_if(PM_ASGARDIAN) && !rn2(20) && moveamt > 1) /* Asgardians are slower sometimes, this is intentional. --Amy */
				moveamt /= 2;

			if (Race_if(PM_SPIRIT) && !rn2(8) && moveamt > 1) /* Spirits too. */
				moveamt /= 2;

			if (is_sand(u.ux,u.uy) && !Flying && !Levitation && !rn2(4) && moveamt > 1)
				moveamt /= 2;

			if (uarmc && uarmc->oartifact == ART_WEB_OF_THE_CHOSEN && !rn2(8) && moveamt > 1)
				moveamt /= 2;

			if (uarm && (uarm->oartifact == ART_CD_ROME_ARENA) && !rn2(8) && moveamt > 1) /* roman clothing just generally slows you down */
				moveamt /= 2;

			if (uarmf && OBJ_DESCR(objects[uarmf->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "roman sandals") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "rimskiye sandalii") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "rim fuqarosi kavushlari") ) && !rn2(8) && moveamt > 1 ) /* Roman sandals aren't made for running. */
				moveamt /= 2;

			if (Race_if(PM_SOVIET) && !rn2(8) && moveamt > 1) /* And soviets, since they get enough features that make the game easier than it's supposed to be. */
				moveamt /= 2;

			if (Race_if(PM_ARMED_COCKATRICE) && !Upolyd && !rn2(4) && moveamt > 1) /* Cockatrices even more. */
				moveamt /= 2;

			if (Race_if(PM_WEAPON_CUBE) && !Upolyd && !rn2(4) && moveamt > 1)
				moveamt /= 2;

			if (Race_if(PM_CORTEX) && !Upolyd && !rn2(4) && moveamt > 1)
				moveamt /= 2;

		/* The new numbed and frozen properties seem to dislike rn2 calls for some reason.
		 * So I need to make a subloop to prevent numbed or frozen players from being completely immobile. */

			if (Numbed && moveamt > 1) {
				if ( (youmonst.data->mmove > 1 || !rn2(2)) && !rn2(10))
				moveamt = 0; /* numbed characters sometimes miss turns --Amy */
			}
			if (Frozen && (!(uarmf && uarmf->oartifact == ART_VERA_S_FREEZER) || !rn2(3)) && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2; /* frozen characters move at half speed --Amy */
			}

			/* salamander race by Kwahn, speeds up in lava */
			if (is_lava(u.ux, u.uy) && Race_if(PM_PLAYER_SALAMANDER)) {
				moveamt *= 3;
			}

			if (is_snow(u.ux, u.uy) && !Flying && !Levitation) {
					static boolean canwalkonsnow = 0;
				    static int skates = 0;
				    if (!skates) skates = find_skates();
				    static int skates2 = 0;
				    if (!skates2) skates2 = find_skates2();
				    static int skates3 = 0;
				    if (!skates3) skates3 = find_skates3();
				    static int skates4 = 0;
				    if (!skates4) skates4 = find_skates4();
				    if ((uarmf && uarmf->otyp == skates)
					    || (uarmf && uarmf->otyp == skates2)
					    || (uarmf && uarmf->otyp == skates3)
					    || (uarmf && uarmf->otyp == skates4)
					    || (uarmf && uarmf->oartifact == ART_BRIDGE_SHITTE)
					    || (uarmf && uarmf->oartifact == ART_MERLOT_FUTURE)) canwalkonsnow = 1;

				if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER) && uimplant && uimplant->oartifact == ART_WHITE_WHALE_HATH_COME) canwalkonsnow = 1;

				if ((youmonst.data->mmove > 1 || !rn2(2)) && !canwalkonsnow)
				moveamt /= 4;

				if (canwalkonsnow && ((uarmf && uarmf->otyp == skates4) || (uarmf && uarmf->oartifact == ART_BRIDGE_SHITTE) || (uarmf && uarmf->oartifact == ART_CORINA_S_SNOWY_TREAD)) && !rn2(2)) {
					moveamt *= 2;
				}
			}

			if ((uwep && uwep->oartifact == ART_KINGS_RANSOM_FOR_YOU) && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2;
			}
			if ((u.twoweap && uswapwep && uswapwep->oartifact == ART_KINGS_RANSOM_FOR_YOU) && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2;
			}
			if ((uwep && uwep->otyp == COLOSSUS_BLADE) && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2;
			}
			if ((u.twoweap && uswapwep && uswapwep->otyp == COLOSSUS_BLADE) && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2;
			}
			if (Race_if(PM_DUFFLEPUD) && uarmf && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2; /* dufflepud wearing boots moves at half speed --Amy */
			}
			if (Race_if(PM_WEAPONIZED_DINOSAUR) && uarmf && !PlayerInHighHeels && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2; /* dinosaur wearing non-high-heeled boots moves at half speed --Amy */
			}
			if ((uarmc && OBJ_DESCR(objects[uarmc->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "slowing gown") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "zamedlennoye plat'ye") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "sekinlashuvi libos") ) ) && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2;
			}

			if (uarmh && uarmh->oartifact == ART_ELONA_S_SNAIL_TRAIL && !Race_if(PM_SNAIL) && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2;
			}

			if ((uarmf && OBJ_DESCR(objects[uarmf->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "fetish heels") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "idol kabluki") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "but poshnalar") )) && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2;
			}
			if ((uarmf && OBJ_DESCR(objects[uarmf->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "velcro sandals") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "sandalii na lipuchkakh") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "cirt kavushlari") )) && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2;
			}
			if (u.inertia && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2; /* inert characters move at half speed --Amy */
			}
			if (Race_if(PM_TURTLE) && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2; /* turtles are very slow too --Amy */
			}
			if (Race_if(PM_ELONA_SNAIL) && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2; /* ever played Elona? Snail is the best race! :D --Amy */
			}
			if (Race_if(PM_LOWER_ENT) && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2; /* ents are very slow too --Amy */
			}
			if (Role_if(PM_TRANSSYLVANIAN) && (moveamt > 1) && (!PlayerInHighHeels) ) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2; /* transsylvanian has to wear heels at all times --Amy */
			}
			if (uarm && uarm->oartifact == ART_WEB_OF_LOLTH && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2;
			}
			if (uarm && uarm->oartifact == ART_ROFLCOPTER_WEB && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2;
			}
			if (uwep && uwep->oartifact == ART_ARABELLA_S_WARDING_HOE && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2;
			}
			if (u.twoweap && uswapwep && uswapwep->oartifact == ART_ARABELLA_S_WARDING_HOE && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2;
			}
			if (uarmf && uarmf->oartifact == ART_GOEFFELBOEFFEL && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2;
			}

			/* double and quad attack are teh pwnz0r, so they need to have a downside --Amy */
			if (Double_attack && moveamt > 1) {
				if ((youmonst.data->mmove > 1 || !rn2(2)) && rn2(3))
				moveamt /= 2;
			}
			if (Quad_attack && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2;
			}

			if (u.hanguppenalty && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2; /* punishment for attempting hangup cheat --Amy */
			}

			if ( (SpeedBug || u.uprops[SPEED_BUG].extrinsic || (uarmf && uarmf->oartifact == ART_UNEVEN_ENGINE) || (uarmf && uarmf->oartifact == ART_ERROR_IN_PLAY_ENCHANTMENT) || have_speedbugstone()) && moveamt > 1) { /* speed bug messes up the player's speed --Amy */
				if (rn2(5)) moveamt *= rnd(5);
				moveamt /= rnd(6);
				if (!rn2(5)) moveamt /= 2;
			}

			/* speed bug reverses speed effects --Amy */
			if (Very_fast && (SpeedBug || u.uprops[SPEED_BUG].extrinsic || (uarmf && uarmf->oartifact == ART_UNEVEN_ENGINE) || (uarmf && uarmf->oartifact == ART_ERROR_IN_PLAY_ENCHANTMENT) || have_speedbugstone()) && rn2(4) && rn2(4) && moveamt > 1 ) {	/* speed boots or potion */
			    /* average movement is 0.5625 times normal */

				moveamt /= 2;

			} else if (Fast && (SpeedBug || u.uprops[SPEED_BUG].extrinsic || (uarmf && uarmf->oartifact == ART_UNEVEN_ENGINE) || (uarmf && uarmf->oartifact == ART_ERROR_IN_PLAY_ENCHANTMENT) || have_speedbugstone()) && !rn2(4) && moveamt > 1 ) {
			    /* average movement is 0.75 times normal */

				moveamt /= 2;
			}

			if (moveamt < 0) moveamt = 0;

			if (Very_fast && !SpeedBug && !u.uprops[SPEED_BUG].extrinsic && !(uarmf && uarmf->oartifact == ART_UNEVEN_ENGINE) && !(uarmf && uarmf->oartifact == ART_ERROR_IN_PLAY_ENCHANTMENT) && !have_speedbugstone()) {	/* speed boots or potion */
			    /* average movement is 1.67 times normal */
			    moveamt += NORMAL_SPEED / 2;
			    if (rn2(3) == 0) moveamt += NORMAL_SPEED / 2;
			} else if (Fast && !SpeedBug && !u.uprops[SPEED_BUG].extrinsic && !(uarmf && uarmf->oartifact == ART_UNEVEN_ENGINE) && !(uarmf && uarmf->oartifact == ART_ERROR_IN_PLAY_ENCHANTMENT) && !have_speedbugstone()) {
			    /* average movement is 1.33 times normal */
			    if (rn2(3) != 0) moveamt += NORMAL_SPEED / 2;
			}

			if (Fear_factor && Feared) {
			    if (rn2(3) != 0) moveamt += NORMAL_SPEED / 2;
			}

			if (Wonderlegs && Wounded_legs) {
				moveamt *= 5;
				moveamt /= 4;
			}

			/* unicorns are ultra fast!!! However, they have enough bullshit downsides to reign them in. --Amy */
			if (Race_if(PM_PLAYER_UNICORN)) {
				moveamt *= 2;
			}

			if (numberofwornetheritems() > rn2(20)) {
				moveamt *= 2;
			}

			if (is_highway(u.ux, u.uy)) {
				moveamt *= 2;
			}

			if (uarmh && uarmh->oartifact == ART_LUXIDREAM_S_ASCENSION) {
				moveamt *= 11;
				moveamt /= 10;
			}

			if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER) && uimplant && uimplant->oartifact == ART_ETHERATORGARDEN) {
				moveamt *= 6;
				moveamt /= 5;
			}

			if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER) && uimplant && uimplant->oartifact == ART_YOU_SHOULD_SURRENDER) {
				moveamt *= 3;
				moveamt /= 2;
			}

			if (uimplant && uimplant->oartifact == ART_BRRRRRRRRRRRRRMMMMMM) {
				if (is_highway(u.ux, u.uy) || (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER))) {
					moveamt *= 2;
				}
			}

			if (uarmg && uarmg->oartifact == ART_LINE_CAN_PLAY_BY_YOURSELF) moveamt *= 2;

			if (uarmh && (uarmh->oartifact == ART_REAL_SPEED_DEVIL) && !rn2(10)) moveamt += NORMAL_SPEED / 2;
			if (uarmf && (uarmf->oartifact == ART_VRRRRRRRRRRRR) && !rn2(5)) moveamt += NORMAL_SPEED / 2;
			if (uarmh && (uarmh->oartifact == ART_LORSKEL_S_SPEED) && !rn2(10)) moveamt += NORMAL_SPEED / 2;
			if (uarmf && (uarmf->oartifact == ART_HIGHEST_FEELING) && !rn2(2)) moveamt += NORMAL_SPEED / 2;
			if (uarmc && (uarmc->oartifact == ART_WINDS_OF_CHANGE) && !rn2(10)) moveamt += NORMAL_SPEED / 2;
			if (uarm && (uarm->oartifact == ART_FORMULA_ONE_SUIT) && !rn2(10)) moveamt += NORMAL_SPEED / 2;
			if (uarmh && !rn2(10) && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "formula one helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "formula odin shlem") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "formula bir zarbdan") ) ) moveamt += NORMAL_SPEED / 2;
			if (uarmf && !rn2(10) && OBJ_DESCR(objects[uarmf->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "turbo boots") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "turbo sapogi") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "qidiruvi va turbo chizilmasin") ) ) moveamt += NORMAL_SPEED / 2;
			if (uarmg && !rn2(10) && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "racer gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "gonshchik perchatki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "poygachi qo'lqop") ) ) moveamt += NORMAL_SPEED / 2;

			if (PlayerInHighHeels && !rn2(10) && !(PlayerCannotUseSkills) && (P_SKILL(P_HIGH_HEELS) >= P_MASTER) ) moveamt += NORMAL_SPEED / 2;
			if (PlayerInHighHeels && !rn2(10) && !(PlayerCannotUseSkills) && (P_SKILL(P_HIGH_HEELS) >= P_GRAND_MASTER) ) moveamt += NORMAL_SPEED / 2;
			if (PlayerInHighHeels && !rn2(10) && !(PlayerCannotUseSkills) && (P_SKILL(P_HIGH_HEELS) >= P_SUPREME_MASTER) ) moveamt += NORMAL_SPEED / 2;

			if (!rn2(10) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "greek cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "grecheskiy plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "yunon plash") ) ) moveamt += NORMAL_SPEED / 2;

			if (uarmf && uarmf->oartifact == ART_WARP_SPEED && (is_waterypool(u.ux, u.uy) || is_watertunnel(u.ux, u.uy))) moveamt += (NORMAL_SPEED * 5);

			if (tech_inuse(T_BLINK)) { /* TECH: Blinking! */
			    /* Case    Average  Variance
			     * -------------------------
			     * Normal    12         0
			     * Fast      16        12
			     * V fast    20        12
			     * Blinking  24        12
			     * F & B     28        18
			     * V F & B   30        18
			     */
			    moveamt += NORMAL_SPEED * 2 / 3;
			    if (rn2(3) == 0) moveamt += NORMAL_SPEED / 2;
			}

			if (uarmc && uarmc->oartifact == ART_LIGHTSPEED_TRAVEL) {
			    if (rn2(3) == 0) moveamt += NORMAL_SPEED / 2;
			}
		    }

		    switch (wtcap) { /* tweaked so the player is slowed down less --Amy */
			case UNENCUMBERED: break;
			case SLT_ENCUMBER: moveamt -= (moveamt / 5); break;
			case MOD_ENCUMBER: moveamt -= (moveamt / 3); break;
			case HVY_ENCUMBER: moveamt -= (moveamt / 2); break;
			case EXT_ENCUMBER: moveamt -= ((moveamt * 3) / 4); break;
			default: break;
		    }

			/* being satiated makes you slower... --Amy */
		    if (u.uhunger >= 3200) moveamt -= (moveamt / 6);
		    if (u.uhunger >= 4000) moveamt -= (moveamt / 5);
		    if (u.uhunger >= 5000) moveamt -= (moveamt / 5);
		    if (u.uhunger >= 6000) moveamt -= (moveamt / 5);
		    if (u.uhunger >= 7000) moveamt -= (moveamt / 5);
		    if (u.uhunger >= 8000) moveamt -= (moveamt / 5);
		    if (u.uhunger >= 9000) moveamt -= (moveamt / 5);
		    if (u.uhunger >= 10000) moveamt -= (moveamt / 5);

		    youmonst.movement += moveamt;
		    if (youmonst.movement < 0) youmonst.movement = 0;
		    settrack();

		    if (!rn2(2) || !(uarmf && OBJ_DESCR(objects[uarmf->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "irregular boots") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "neregulyarnyye sapogi") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "tartibsizlik chizilmasin") ) ) ) {

			if (!rn2(2) || !((uleft && uleft->oartifact == ART_GOOD_THINGS_WILL_HAPPEN_EV) || (uright && uright->oartifact == ART_GOOD_THINGS_WILL_HAPPEN_EV)) ) {
				if (!rn2(2) || !RngeIrregularity) {
				    monstermoves++;
				    moves++;
				}
			}

		    }

			if (u.uprops[FAST_FORWARD].extrinsic) {

			    monstermoves++;
			    moves++;
			    nh_timeout();

			}

			if (have_fastforwardstone()) {

			    monstermoves++;
			    moves++;
			    nh_timeout();

			}

			if (TimeGoesByFaster) {

				int veryfasttime;

			    monstermoves++;
			    moves++;
			    nh_timeout();

				veryfasttime = TimeGoesByFaster / 5000;
				if (veryfasttime) {

					moves += veryfasttime;
					monstermoves += veryfasttime;
					nh_timeout();

				}

			}

		    /********************************/
		    /* once-per-turn things go here */
		    /********************************/

		if (!rn2(100)) u.statuetrapname = rn2(NUMMONS);

		if (!Upolyd && u.polyformed) u.polyformed = 0; /* catch-all, because coding this in polyself.c is horrible --Amy */

		if (AutoDestruct || u.uprops[AUTO_DESTRUCT].extrinsic || (uarmf && uarmf->oartifact == ART_KHOR_S_REQUIRED_IDEA) || have_autodestructstone()) stop_occupation();
 
		if (PlayerInBlockHeels) {

			if (HConfusion > 10 && !rn2(10)) HConfusion--;

			if (HStun > 10) {

				register int dmgreductor = 11;
				if (!(PlayerCannotUseSkills)) switch (P_SKILL(P_HIGH_HEELS)) {
					case P_BASIC: dmgreductor = 16; break;
					case P_SKILLED: dmgreductor = 21; break;
					case P_EXPERT: dmgreductor = 26; break;
					case P_MASTER: dmgreductor = 31; break;
					case P_GRAND_MASTER: dmgreductor = 36; break;
					case P_SUPREME_MASTER: dmgreductor = 41; break;
				}
				if (rnd(100) < dmgreductor) HStun--;

			}

		}

		/* beauty charm: tries each turn to pacify monsters around you, but only humanoids or animals --Amy */
		if (tech_inuse(T_BEAUTY_CHARM) && PlayerInHighHeels) {
			struct monst *mtmp3;
			int k, l;

		    for (k = -3; k <= 3; k++) for(l = -3; l <= 3; l++) {
			if (!isok(u.ux + k, u.uy + l)) continue;
			if ( ((mtmp3 = m_at(u.ux + k, u.uy + l)) != 0) && mtmp3->mtame == 0 && mtmp3->isshk == 0 && mtmp3->isgd 			== 0 && mtmp3->ispriest == 0 && mtmp3->isminion == 0 && mtmp3->isgyp == 0
&& mtmp3->data != &mons[PM_SHOPKEEPER] && mtmp3->data != &mons[PM_BLACK_MARKETEER] && mtmp3->data != &mons[PM_ALIGNED_PRIEST] && mtmp3->data != &mons[PM_HIGH_PRIEST] && mtmp3->data != &mons[PM_DNETHACK_ELDER_PRIEST_TM_] && mtmp3->data != &mons[PM_GUARD]
			&& mtmp3->mnum != quest_info(MS_NEMESIS) && !(rn2(5) && (mtmp3->data->geno & G_UNIQ))) {

				if (mtmp3->mfrenzied) continue;
				if (mtmp3->mpeaceful) continue;
				if (!(humanoid(mtmp3->data) && !rn2(5)) && !(is_animal(mtmp3->data) && !rn2(10)) ) continue;
				if (resist(mtmp3, RING_CLASS, 0, NOTELL) && !(((rnd(30 - ACURR(A_CHA))) < 4) && !resist(mtmp3, RING_CLASS, 0, NOTELL)) ) continue;

				pline("%s is charmed by your sexy pose!", mon_nam(mtmp3));
				mtmp3->mpeaceful = TRUE;

			} /* monster is catchable loop */
		    } /* for loop */

		}

		if (FaintActive && !rn2(100) && multi >= 0) {

			You("faint from exertion.");
			flags.soundok = 0;
			nomul(-(rnz(5) ), "fainted from exertion", TRUE);
			nomovemsg = "You regain consciousness.";
			afternmv = unfaintX;

		}

		if (uimplant && uimplant->oartifact == ART_KATRIN_S_SUDDEN_APPEARANCE && !rn2(100) && multi >= 0) {

			You("faint from exertion.");
			flags.soundok = 0;
			nomul(-(rnz(5) ), "fainted from exertion", TRUE);
			nomovemsg = "You regain consciousness.";
			afternmv = unfaintX;

		}

		if ((uarmh && uarmh->oartifact == ART_UBB_RUPTURE) && !rn2(100) && multi >= 0) {

			You("faint from the terrible sounds.");
			flags.soundok = 0;
			nomul(-(rnz(5) ), "fainted from terrible sounds", TRUE);
			nomovemsg = "You regain consciousness.";
			afternmv = unfaintX;

		}

		if ((uleft && uleft->oartifact == ART_BLIND_PILOT) && !rn2(100) && multi >= 0) {

			You("faint from exertion.");
			flags.soundok = 0;
			nomul(-(rnz(5) ), "fainted from exertion", TRUE);
			nomovemsg = "You regain consciousness.";
			afternmv = unfaintX;

		}

		if ((uright && uright->oartifact == ART_BLIND_PILOT) && !rn2(100) && multi >= 0) {

			You("faint from exertion.");
			flags.soundok = 0;
			nomul(-(rnz(5) ), "fainted from exertion", TRUE);
			nomovemsg = "You regain consciousness.";
			afternmv = unfaintX;

		}

		if (uarmc && (uarmc->oartifact == ART_INA_S_LAB_COAT) && !rn2(100) && multi >= 0) {

			if (!strncmpi(plname, "Ina", 3)) {
			/* There is of course no corresponding real-life Ina. --Amy */

			Your("terrible eating disorder causes you to faint.");
			flags.soundok = 0;
			nomul(-(rnz(5) ), "fainted from anorexia", TRUE);
			nomovemsg = "Dear Ina, you need to eat!!! If you don't, you'll die!";
			afternmv = unfaintX;

			} else {

			You("faint from anorexia.");
			flags.soundok = 0;
			nomul(-(rnz(5) ), "fainted from anorexia", TRUE);
			nomovemsg = "If you don't want to end like Ina did, eat!!!";
			afternmv = unfaintX;

			}
	
		}

		if (uarmc && (uarmc->oartifact == ART_INA_S_OVERCOAT) && !rn2(100) && multi >= 0) {

			if (!strncmpi(plname, "Ina", 3)) {
			/* There is of course no corresponding real-life Ina. --Amy */

			Your("terrible eating disorder causes you to faint.");
			flags.soundok = 0;
			nomul(-(rnz(5) ), "fainted from anorexia", TRUE);
			nomovemsg = "Dear Ina, you need to eat!!! If you don't, you'll die!";
			afternmv = unfaintX;

			} else {

			You("faint from anorexia.");
			flags.soundok = 0;
			nomul(-(rnz(5) ), "fainted from anorexia", TRUE);
			nomovemsg = "If you don't want to end like Ina did, eat!!!";
			afternmv = unfaintX;

			}
	
		}

		/* safety check in case the hero somehow manages to survive genociding themselves --Amy */
		if (!Upolyd && ((mvitals[urole.malenum].mvflags & G_GENOD) ||
				(urole.femalenum != NON_PM &&
				(mvitals[urole.femalenum].mvflags & G_GENOD)) ||
				(mvitals[urace.malenum].mvflags & G_GENOD) ||
				(urace.femalenum != NON_PM &&
				(mvitals[urace.femalenum].mvflags & G_GENOD)))) {

		    killer_format = KILLED_BY;
		    killer = "genocidal existence failure";
		    done(GENOCIDED);

		}

		/* safety check in case the hero becomes an ungenomold by recursion --Amy */
		if (Upolyd && Race_if(PM_UNGENOMOLD) && !(mvitals[PM_UNGENOMOLD].mvflags & G_GENOD) ) {
			mvitals[PM_UNGENOMOLD].mvflags |= (G_GENOD|G_NOCORPSE);
		}

		if (issoviet && !rn2(1000)) { /* mocking messages :-P --Amy */
			pline(soviettaunt());
		}

		if (Role_if(PM_GANG_SCHOLAR) && !rn2(1000)) {
			gangscholarmessage();
		}

		if (RngeLoudspeakers && !rn2(100)) {
			pline(fauxmessage());
			if (!rn2(3)) pline(fauxmessage());
		}

		for(ttmp = ftrap; ttmp; ttmp = ttmp->ntrap) { /* this function is probably expensive... --Amy */
			if (ttmp && ttmp->ttyp == LOUDSPEAKER && !rn2(100) ) {
				pline(fauxmessage());
				if (!rn2(3)) pline(fauxmessage());
			}
			if (ttmp && ttmp->ttyp == ARABELLA_SPEAKER && !rn2(50) ) {
				pline(fauxmessage());
				if (!rn2(3)) pline(fauxmessage());
			}

			if (ttmp && ttmp->ttyp == KOP_CUBE && !rn2(2000) && !(m_at(ttmp->tx, ttmp->ty)) ) {

				u.aggravation = 1;
				reset_rndmonst(NON_PM);
				(void) makemon(mkclass(S_KOP,0), ttmp->tx, ttmp->ty, MM_ANGRY);
				u.aggravation = 0;
				if (!rn2(20)) {
					deltrap(ttmp);
					goto trapsdone; /* we check for ttmp below, but just to be on the safe side... --Amy */
				}

			}

			if (ttmp && ttmp->ttyp == BOSS_SPAWNER) {

				int bossfrequency = 1000;
				switch (ttmp->launch_otyp) {

					default: /* 1-10 */
						break;
					case 11:
						bossfrequency = 2000;
						break;
					case 12:
						bossfrequency = 4000;
						break;
					case 13:
						bossfrequency = 6000;
						break;
					case 14:
						bossfrequency = 8000;
						break;
					case 15:
						bossfrequency = 10000;
						break;
					case 16:
						bossfrequency = 20000;
						break;
					case 17:
						bossfrequency = 30000;
						break;
					case 18:
						bossfrequency = 40000;
						break;
					case 19:
						bossfrequency = 50000;
						break;
					case 20:
						bossfrequency = 100000;
						break;

				}

				if (bossfrequency < 1000) bossfrequency = 1000; /* fail safe */

				if (!rn2(bossfrequency)) {

					if (!rn2(4)) {

						u.aggravation = 1;
						reset_rndmonst(NON_PM);

						int attempts = 0;
						register struct permonst *ptrZ;
newbossS:
						do {

							ptrZ = rndmonst();
							attempts++;
							if (!rn2(2000)) reset_rndmonst(NON_PM);

						} while ( (!ptrZ || (ptrZ && !(ptrZ->geno & G_UNIQ))) && attempts < 50000);

						if (ptrZ && ptrZ->geno & G_UNIQ) {
							if (wizard) pline("monster generation: %s", ptrZ->mname);
							(void) makemon(ptrZ, ttmp->tx, ttmp->ty, MM_ANGRY|MM_ADJACENTOK);
						}
						else if (rn2(50)) {
							attempts = 0;
							goto newbossS;
						}

						switch (rnd(8)) {

							case 1:
								You_hear("a dramatic sound that tells of imminent danger!");
								break;
							case 2:
								You_hear("an innocuous tune that seems to decrease in pitch...");
								break;
							case 3:
								You_hear("a terrible melody, and realize that there is no escape!");
								break;
							case 4:
								You_hear("a wavy, repeating sound!");
								break;
							case 5:
								You_hear("a deep metallic tune.");
								break;
							case 6:
								You_hear("a standard combat jingle, except in the end it somehow sounds nonstandard...");
								break;
							case 7:
								You_hear("a frantic melody that quickly rises and falls in pitch!");
								break;
							case 8:
								You_hear("an extremely high-pitch jingle and instantly know that something completely abnormal must have happened!");
								break;

						}

						u.aggravation = 0;
						deltrap(ttmp);
						goto trapsdone;
					} 
				}

			}

			if (ttmp && ttmp->ttyp == FUMAROLE && (distu(ttmp->tx, ttmp->ty) < 4 ) ) {

				if (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "filtered helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "fil'truyut shlem") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "filtrlangan zarbdan") ) && !rn2(2) ) {
					 pline("A cloud of spores surrounds you!");
				} else if (RngeGasFiltering && !rn2(2)) {
					 pline("A cloud of spores surrounds you!");

				 } else if (!Strangled && !Breathless) {
					 You("inhale a cloud of spores!");
					 poisoned("spores", A_STR, "fumarole spores", 30);
			       } else {
					 pline("A cloud of spores surrounds you!");
					 if (rn2(2)) poisoned("spores", A_STR, "fumarole spores", 30);
			       }

			}

			if (ttmp && ttmp->ttyp == VENTILATOR && (distu(ttmp->tx, ttmp->ty) < 4 ) ) {
				if (!rn2(3)) pline("Air currents blow in your %s!", body_part(FACE));
				pushplayer();
			}

			if (ttmp && ttmp->ttyp == MONSTER_CUBE && !rn2(500)) {
				if (!enexto(&cc, ttmp->tx, ttmp->ty, (struct permonst *)0) ) continue;
				if (Aggravate_monster) {
					u.aggravation = 1;
					reset_rndmonst(NON_PM);
				}
				(void) makemon((struct permonst *)0, ttmp->tx, ttmp->ty, MM_ADJACENTOK);
				u.aggravation = 0;
				if (!rn2(20)) pline("Chaeaet!");
				if (!rn2(50)) ttmp->ttyp = ANIMATION_TRAP;
			}

			if (ttmp && ttmp->ttyp == SPREADING_TRAP && !rn2(500)) {
				makerandomtrap();
			}

			if (ttmp && ttmp->ttyp == ADJACENT_TRAP && !(t_at(u.ux, u.uy)) && (distu(ttmp->tx, ttmp->ty) < 4 ) ) {
				maketrap(u.ux, u.uy, randomtrap(), 100 );
			}

			if (ttmp && ttmp->ttyp == SUPERTHING_TRAP && (multi >= 0) && (distu(ttmp->tx, ttmp->ty) < 4 ) ) {
				nomul(-rnd(5), "standing next to a superthing", FALSE);
			}

			if (ttmp && ttmp->ttyp == CURSED_GRAVE && !rn2(500)) {
				if (!enexto(&cc, ttmp->tx, ttmp->ty, (struct permonst *)0) ) continue;

				if (Aggravate_monster) {
					u.aggravation = 1;
					reset_rndmonst(NON_PM);
				}

				    switch (rnd(10)) {
				    case 1:
					(void) makemon(mkclass(S_VAMPIRE,0), ttmp->tx, ttmp->ty, MM_ADJACENTOK);
					break;
				    case 2:
				    case 3:
				    case 4:
				    case 5:
					(void) makemon(mkclass(S_ZOMBIE,0), ttmp->tx, ttmp->ty, MM_ADJACENTOK);
					break;
				    case 6:
				    case 7:
				    case 8:
					(void) makemon(mkclass(S_MUMMY,0), ttmp->tx, ttmp->ty, MM_ADJACENTOK);
					break;
				    case 9:
					(void) makemon(mkclass(S_GHOST,0), ttmp->tx, ttmp->ty, MM_ADJACENTOK);
					break;
				    case 10:
					(void) makemon(mkclass(S_WRAITH,0), ttmp->tx, ttmp->ty, MM_ADJACENTOK);
					break;
				    }

				u.aggravation = 0;

				if (midnight()) You_feel("a ghastly chill running down your %s!", body_part(SPINE) );
				else if (!rn2(20)) pline("A monster rises from the grave!");
				if (!rn2(50)) ttmp->ttyp = SUMMON_UNDEAD_TRAP;
			}

		}
trapsdone:

		if (have_faintingstone() && !rn2(100) && multi >= 0) {

			You("faint from exertion.");
			flags.soundok = 0;
			nomul(-(rnz(5) ), "fainted from exertion", TRUE);
			nomovemsg = "You regain consciousness.";
			afternmv = unfaintX;

		}

		if (FemaleTrapThai && IS_TOILET(levl[u.ux][u.uy].typ) && u.uhs < HUNGRY ) {
			pline("For some reason, you have to take a shit right now.");

			if (Sick && !rn2(3) ) make_sick(0L, (char *)0, TRUE, SICK_VOMITABLE);
			else if (Sick && !rn2(10) ) make_sick(0L, (char *)0, TRUE, SICK_ALL);
			morehungry(rn2(400)+200);

			if (uarmu && uarmu->oartifact == ART_KATIA_S_SOFT_COTTON) {
				You("produce very erotic noises.");
				if (!rn2(10)) adjattrib(rn2(A_CHA), 1, -1);
			}

			pline("But in your haste, you forgot to open the lid!");
			adjalign(-20);

		}

		if (FemaleTrapYvonne && (multi >= 0) && IS_TOILET(levl[u.ux][u.uy].typ) && u.uhs < HUNGRY ) {
			pline("A toilet! You feel that you have to take a shit, and so you do.");

			int crapduration = 5;
			if (uarm && objects[uarm->otyp].oc_delay) {
				pline("Taking off your armor is going to take a while...");
				crapduration += objects[uarm->otyp].oc_delay;
			}
			if (uarmc && objects[uarmc->otyp].oc_delay) {
				You("need to remove your cloak...");
				crapduration += objects[uarmc->otyp].oc_delay;
			}
			if (Sick && !rn2(3) ) make_sick(0L, (char *)0, TRUE, SICK_VOMITABLE);
			else if (Sick && !rn2(10) ) make_sick(0L, (char *)0, TRUE, SICK_ALL);
			morehungry(rn2(400)+200);

			if (uarmu && uarmu->oartifact == ART_KATIA_S_SOFT_COTTON) {
				You("produce very erotic noises.");
				if (!rn2(10)) adjattrib(rn2(A_CHA), 1, -1);
			}

			nomovemsg = "You are done shitting.";
			nomul(-crapduration, "taking a shit", TRUE);

		}

		if (FemaleTrapMaurah && !rn2(100)) {

			pline("Suddenly, you produce beautiful farting noises with your sexy butt.");
			badeffect();

		}

		if (FemaleTrapElif && !rn2(100)) {

			switch (rnd(4)) {

				case 1:
					pline("Elif suddenly kicks your %s with her very soft, female sneakers, and draws %s!", body_part(HAND), body_part(BLOOD));
					incr_itimeout(&Glib, 2); /* just enough to make you drop your weapon */
					losehp(rnd(u.ulevel), "Elif's soft female sneakers", KILLED_BY);
					break;
				case 2:
					pline("Elif suddenly produces %s farting noises with her sexy butt.", rn2(2) ? "tender" : "soft");
					badeffect();
					break;
				case 3:
					pline("Elif suddenly uses her very sharp-edged female fingernails and cuts your unprotected skin!");
					if (Upolyd && u.mhmax > 1) {
						u.mhmax--;
						if (u.mh > u.mhmax) u.mh = u.mhmax;
					} else if (!Upolyd && u.uhpmax > 1) {
						u.uhpmax--;
						if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
					}
					break;
				case 4:
					pline("Elif suddenly strokes you gently with her very soft, fleecy hands!");
					if (Upolyd) u.mhmax++;
					else u.uhpmax++;
					break;

			}

		}

		if (FemaleTrapNatalje) {
			if ((u.ux != u.nataljetrapx) || (u.uy != u.nataljetrapy)) {
				u.nataljetrapturns = moves;
				u.nataljetrapx = u.ux;
				u.nataljetrapy = u.uy;
			}

			if (moves > (u.nataljetrapturns + 6)) {
				u.nataljetrapturns = moves;
				u.nataljetrapx = u.ux;
				u.nataljetrapy = u.uy;
				pline("Oh no! You were standing still for too long, and are horribly scarred by a bunch of female painted asian toenails. Your sexy high heels are also damaged.");

				if (ABASE(A_CHA) > 3) (void) adjattrib(A_CHA, -1, TRUE);
				else {
				    if (Upolyd) {
					u.mh -= 5;
					u.mhmax -= 5;
				    } else {
					u.uhp -= 5;
					u.uhpmax -= 5;
					if (u.uhp < 1) {
						u.youaredead = 1;
						pline("The nails cut you fatally and you die.");
						killer_format = KILLED_BY;
						killer = "painted asian toenails";
						done(DIED);
						u.youaredead = 0;

					}
				    }

				}

				antimatter_damage(invent, FALSE, FALSE);
			}

			if (moves > (u.nataljetrapturns + 3)) {
				if (!strncmpi(plname, "Natalje", 7)) {
					pline("Keep dancing, Natalje...");
				} else {
					You("gotta keep dancing...");
				}
			}

			if (moves > (u.nataljetrapturns + 5)) {
				if (!strncmpi(plname, "Natalje", 7)) {
					pline("Careful, Natalje! You gotta dance or you'll get hurt!");
				} else {
					You("missed the beat! Continue dancing or suffer!");
				}
			}

		}

		if (RngeCoquetry && !rn2(1000) && multi >= 0) {

			You("faint from exertion.");
			flags.soundok = 0;
			nomul(-(rnd(10) ), "fainted from exertion", TRUE);
			nomovemsg = "You regain consciousness.";
			afternmv = unfaintX;

		}

		if (uarmf && uarmf->oartifact == ART_JOSEFINE_S_EVILNESS && !rn2(100) && multi >= 0) {

			You("faint from exertion.");
			flags.soundok = 0;
			nomul(-(rnz(5) ), "fainted from exertion", TRUE);
			nomovemsg = "You regain consciousness.";
			afternmv = unfaintX;

		}

		if (u.uprops[FAINT_ACTIVE].extrinsic && !rn2(100) && multi >= 0) {

			You("faint from exertion.");
			flags.soundok = 0;
			nomul(-(rnz(5) ), "fainted from exertion", TRUE);
			nomovemsg = "You regain consciousness.";
			afternmv = unfaintX;

		}

		if (uarmg && uarmg->oartifact == ART_CLAWS_OF_THE_REVENANCER && !rn2(100) && multi >= 0) {

			You("are clawed by your gauntlets and faint.");
			flags.soundok = 0;
			nomul(-(rnz(5) ), "fainted due to the revenancer", TRUE);
			nomovemsg = "You regain consciousness.";
			afternmv = unfaintX;

		}

		if (uarmc && uarmc->oartifact == ART_LAST_STEELING && !rn2(5000) ) {
			register struct obj *steeling;
			pline("You may rustproof an iron object.");
steelingchoice:
			steeling = getobj(all_count, "rustproof");
			if (!steeling) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to rustproof an item.");
				else goto steelingchoice;
				pline("Oh well, if you don't wanna...");
			} else {
				if (!(objects[(steeling)->otyp].oc_material == IRON) ) {
					pline("That is not made of iron!");
				} else if (!stack_too_big(steeling)) {
					steeling->oerodeproof = 1;
					p_glow2(steeling, NH_PURPLE);
				} else pline("The stack was too big and therefore nothing happens...");
			}
		}

		if (uarmc && uarmc->oartifact == ART_PROTECT_WHAT_CANNOT_BE_PRO && !rn2(5000) ) {
			register struct obj *steeling;
			pline("You may erodeproof a nonerodable object.");
protectwhatchoice:
			steeling = getobj(all_count, "erosionproof");
			if (!steeling) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to erosionproof an item.");
				else goto protectwhatchoice;
				pline("Oh well, if you don't wanna...");
			} else {
				if (objects[(steeling)->otyp].oc_material >= WAX && objects[(steeling)->otyp].oc_material <= WOOD) 
					pline("That is erodable, and therefore it doesn't work!");
				else if (objects[(steeling)->otyp].oc_material == IRON)
					pline("That is erodable, and therefore it doesn't work!");
				else if (objects[(steeling)->otyp].oc_material == COPPER)
					pline("That is erodable, and therefore it doesn't work!");
				else if (objects[(steeling)->otyp].oc_material == PLASTIC)
					pline("That is erodable, and therefore it doesn't work!");
				else if (objects[(steeling)->otyp].oc_material >= VIVA && objects[(steeling)->otyp].oc_material <= BRICK) 
					pline("That is erodable, and therefore it doesn't work!");
				else if (!stack_too_big(steeling)) {
					steeling->oerodeproof = 1;
					p_glow2(steeling, NH_PURPLE);
				} else pline("The stack was too big and therefore nothing happens...");
			}
		}

		if (uimplant && uimplant->oartifact == ART_FUKROSION && !rn2(2500) ) {
			register struct obj *steeling;
			pline("You may repair a nonerodable object. If you're in a form without hands, the object you pick will also become erosionproof.");
fukrosionchoice:
			steeling = getobj(all_count, "repair");
			if (!steeling) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to repair/erosionproof an item.");
				else goto fukrosionchoice;
				pline("Oh well, if you don't wanna...");
			} else if (!stack_too_big(steeling)) {
				steeling->oeroded = steeling->oeroded2 = 0;
				if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER)) steeling->oerodeproof = 1;
				p_glow2(steeling, NH_PURPLE);
			} else pline("The stack was too big and therefore nothing happens...");

		}

		if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER) && uimplant && uimplant->oartifact == ART_YES_YOU_CAN && !rn2(500)) {
			if (spellid(0) != NO_SPELL)  {
				int numspells;

				getlin ("You may use inertia control to choose a spell of your choice, which will automatically be cast every turn. Do it? [yes/no]",buf);
				(void) lcase (buf);
				if (!(strcmp (buf, "yes"))) {

					for (numspells = 0; numspells < MAXSPELL && spellid(numspells) != NO_SPELL; numspells++) {
						if (spellid(numspells) == SPE_INERTIA_CONTROL) continue;

						pline("You know the %s spell.", spellname(numspells));
						if (yn("Control the flow of this spell?") == 'y') {
							u.inertiacontrolspell = spellid(numspells);
							u.inertiacontrolspellno = numspells;
							u.inertiacontrol = 50;
							break;
						}
					}
				}	
			}

		}

		/* sustainer should have additional protection against contamination instadeath --Amy */
		if (Race_if(PM_SUSTAINER) && !rn2(10000) && ABASE(A_WIS) < 5) {
			ABASE(A_WIS) += 1;
			AMAX(A_WIS) += 1;
			flags.botl = 1;
			pline("Suddenly, your wisdom increases.");
		}

		if (Prem_death && !rn2(10000)) { /* evil patch idea by jonadab */

			u.youaredead = 1;
			You("suddenly die.");
			killer_format = KILLED_BY;
			killer = "premature death";
			done(DIED);
			u.youaredead = 0;

		}

		if (iswindinhabitant && !rn2(1000)) {

			u.nastinator01 = u.nastinator02 = u.nastinator03 = u.nastinator04 = u.nastinator05 = u.nastinator06 = u.nastinator07 = u.nastinator08 = u.nastinator09 = u.nastinator10 = u.nastinator11 = u.nastinator12 = u.nastinator13 = u.nastinator14 = u.nastinator15 = u.nastinator16 = u.nastinator17 = u.nastinator18 = u.nastinator19 = u.nastinator20 = u.nastinator21 = u.nastinator22 = u.nastinator23 = u.nastinator24 = u.nastinator25 = u.nastinator26 = u.nastinator27 = u.nastinator28 = u.nastinator29 = u.nastinator30 = u.nastinator31 = u.nastinator32 = u.nastinator33 = u.nastinator34 = u.nastinator35 = u.nastinator36 = u.nastinator37 = u.nastinator38 = u.nastinator39 = u.nastinator40 = u.nastinator41 = u.nastinator42 = u.nastinator43 = u.nastinator44 = u.nastinator45 = u.nastinator46 = u.nastinator47 = u.nastinator48 = u.nastinator49 = u.nastinator50 = u.nastinator51 = u.nastinator52 = u.nastinator53 = u.nastinator54 = u.nastinator55 = u.nastinator56 = u.nastinator57 = u.nastinator58 = u.nastinator59 = u.nastinator60 = u.nastinator61 = u.nastinator62 = u.nastinator63 = u.nastinator64 = u.nastinator65 = u.nastinator66 = u.nastinator67 = u.nastinator68 = u.nastinator69 = u.nastinator70 = u.nastinator71 = u.nastinator72 = u.nastinator73 = u.nastinator74 = u.nastinator75 = u.nastinator76 = u.nastinator77 = u.nastinator78 = u.nastinator79 = u.nastinator80 = u.nastinator81 = u.nastinator82 = u.nastinator83 = u.nastinator84 = u.nastinator85 = u.nastinator86 = u.nastinator87 = u.nastinator88 = u.nastinator89 = u.nastinator90 = u.nastinator91 = u.nastinator92 = u.nastinator93 = u.nastinator94 = u.nastinator95 = u.nastinator96 = u.nastinator97 = u.nastinator98 = u.nastinator99 = u.nastinator100 = u.nastinator101 = u.nastinator102 = u.nastinator103 = u.nastinator104 = u.nastinator105 = u.nastinator106 = u.nastinator107 = u.nastinator108 = u.nastinator109 = u.nastinator110 = u.nastinator111 = u.nastinator112 = u.nastinator113 = u.nastinator114 = u.nastinator115 = u.nastinator116 = u.nastinator117 = u.nastinator118 = u.nastinator119 = u.nastinator120 = u.nastinator121 = u.nastinator122 = u.nastinator123 = u.nastinator124 = u.nastinator125 = u.nastinator126 = u.nastinator127 = u.nastinator128 = u.nastinator129 = u.nastinator130 = u.nastinator131 = u.nastinator132 = u.nastinator133 = u.nastinator134 = u.nastinator135 = u.nastinator136 = u.nastinator137 = u.nastinator138 = u.nastinator139 = u.nastinator140 = u.nastinator141 = u.nastinator142 = u.nastinator143 = u.nastinator144 = u.nastinator145 = u.nastinator146 = u.nastinator147 = u.nastinator148 = u.nastinator149 = u.nastinator150 = u.nastinator151 = u.nastinator152 = u.nastinator153 = u.nastinator154 = u.nastinator155 = u.nastinator156 = u.nastinator157 = u.nastinator158 = u.nastinator159 = u.nastinator160 = u.nastinator161 = u.nastinator162 = u.nastinator163 = u.nastinator164 = u.nastinator165 = u.nastinator166 = u.nastinator167 = u.nastinator168 = u.nastinator169 = 0;

		switch (rnd(169)) {

			case 1: u.nastinator01 = 1; break;
			case 2: u.nastinator02 = 1; break;
			case 3: u.nastinator03 = 1; break;
			case 4: u.nastinator04 = 1; break;
			case 5: u.nastinator05 = 1; break;
			case 6: u.nastinator06 = 1; break;
			case 7: u.nastinator07 = 1; break;
			case 8: u.nastinator08 = 1; break;
			case 9: u.nastinator09 = 1; break;
			case 10: u.nastinator10 = 1; break;
			case 11: u.nastinator11 = 1; break;
			case 12: u.nastinator12 = 1; break;
			case 13: u.nastinator13 = 1; break;
			case 14: u.nastinator14 = 1; break;
			case 15: u.nastinator15 = 1; break;
			case 16: u.nastinator16 = 1; break;
			case 17: u.nastinator17 = 1; break;
			case 18: u.nastinator18 = 1; break;
			case 19: u.nastinator19 = 1; break;
			case 20: u.nastinator20 = 1; break;
			case 21: u.nastinator21 = 1; break;
			case 22: u.nastinator22 = 1; break;
			case 23: u.nastinator23 = 1; break;
			case 24: u.nastinator24 = 1; break;
			case 25: u.nastinator25 = 1; break;
			case 26: u.nastinator26 = 1; break;
			case 27: u.nastinator27 = 1; break;
			case 28: u.nastinator28 = 1; break;
			case 29: u.nastinator29 = 1; break;
			case 30: u.nastinator30 = 1; break;
			case 31: u.nastinator31 = 1; break;
			case 32: u.nastinator32 = 1; break;
			case 33: u.nastinator33 = 1; break;
			case 34: u.nastinator34 = 1; break;
			case 35: u.nastinator35 = 1; break;
			case 36: u.nastinator36 = 1; break;
			case 37: u.nastinator37 = 1; break;
			case 38: u.nastinator38 = 1; break;
			case 39: u.nastinator39 = 1; break;
			case 40: u.nastinator40 = 1; break;
			case 41: u.nastinator41 = 1; break;
			case 42: u.nastinator42 = 1; break;
			case 43: u.nastinator43 = 1; break;
			case 44: u.nastinator44 = 1; break;
			case 45: u.nastinator45 = 1; break;
			case 46: u.nastinator46 = 1; break;
			case 47: u.nastinator47 = 1; break;
			case 48: u.nastinator48 = 1; break;
			case 49: u.nastinator49 = 1; break;
			case 50: u.nastinator50 = 1; break;
			case 51: u.nastinator51 = 1; break;
			case 52: u.nastinator52 = 1; break;
			case 53: u.nastinator53 = 1; break;
			case 54: u.nastinator54 = 1; break;
			case 55: u.nastinator55 = 1; break;
			case 56: u.nastinator56 = 1; break;
			case 57: u.nastinator57 = 1; break;
			case 58: u.nastinator58 = 1; break;
			case 59: u.nastinator59 = 1; break;
			case 60: u.nastinator60 = 1; break;
			case 61: u.nastinator61 = 1; break;
			case 62: u.nastinator62 = 1; break;
			case 63: u.nastinator63 = 1; break;
			case 64: u.nastinator64 = 1; break;
			case 65: u.nastinator65 = 1; break;
			case 66: u.nastinator66 = 1; break;
			case 67: u.nastinator67 = 1; break;
			case 68: u.nastinator68 = 1; break;
			case 69: u.nastinator69 = 1; break;
			case 70: u.nastinator70 = 1; break;
			case 71: u.nastinator71 = 1; break;
			case 72: u.nastinator72 = 1; break;
			case 73: u.nastinator73 = 1; break;
			case 74: u.nastinator74 = 1; break;
			case 75: u.nastinator75 = 1; break;
			case 76: u.nastinator76 = 1; break;
			case 77: u.nastinator77 = 1; break;
			case 78: u.nastinator78 = 1; break;
			case 79: u.nastinator79 = 1; break;
			case 80: u.nastinator80 = 1; break;
			case 81: u.nastinator81 = 1; break;
			case 82: u.nastinator82 = 1; break;
			case 83: u.nastinator83 = 1; break;
			case 84: u.nastinator84 = 1; break;
			case 85: u.nastinator85 = 1; break;
			case 86: u.nastinator86 = 1; break;
			case 87: u.nastinator87 = 1; break;
			case 88: u.nastinator88 = 1; break;
			case 89: u.nastinator89 = 1; break;
			case 90: u.nastinator90 = 1; break;
			case 91: u.nastinator91 = 1; break;
			case 92: u.nastinator92 = 1; break;
			case 93: u.nastinator93 = 1; break;
			case 94: u.nastinator94 = 1; break;
			case 95: u.nastinator95 = 1; break;
			case 96: u.nastinator96 = 1; break;
			case 97: u.nastinator97 = 1; break;
			case 98: u.nastinator98 = 1; break;
			case 99: u.nastinator99 = 1; break;
			case 100: u.nastinator100 = 1; break;
			case 101: u.nastinator101 = 1; break;
			case 102: u.nastinator102 = 1; break;
			case 103: u.nastinator103 = 1; break;
			case 104: u.nastinator104 = 1; break;
			case 105: u.nastinator105 = 1; break;
			case 106: u.nastinator106 = 1; break;
			case 107: u.nastinator107 = 1; break;
			case 108: u.nastinator108 = 1; break;
			case 109: u.nastinator109 = 1; break;
			case 110: u.nastinator110 = 1; break;
			case 111: u.nastinator111 = 1; break;
			case 112: u.nastinator112 = 1; break;
			case 113: u.nastinator113 = 1; break;
			case 114: u.nastinator114 = 1; break;
			case 115: u.nastinator115 = 1; break;
			case 116: u.nastinator116 = 1; break;
			case 117: u.nastinator117 = 1; break;
			case 118: u.nastinator118 = 1; break;
			case 119: u.nastinator119 = 1; break;
			case 120: u.nastinator120 = 1; break;
			case 121: u.nastinator121 = 1; break;
			case 122: u.nastinator122 = 1; break;
			case 123: u.nastinator123 = 1; break;
			case 124: u.nastinator124 = 1; break;
			case 125: u.nastinator125 = 1; break;
			case 126: u.nastinator126 = 1; break;
			case 127: u.nastinator127 = 1; break;
			case 128: u.nastinator128 = 1; break;
			case 129: u.nastinator129 = 1; break;
			case 130: u.nastinator130 = 1; break;
			case 131: u.nastinator131 = 1; break;
			case 132: u.nastinator132 = 1; break;
			case 133: u.nastinator133 = 1; break;
			case 134: u.nastinator134 = 1; break;
			case 135: u.nastinator135 = 1; break;
			case 136: u.nastinator136 = 1; break;
			case 137: u.nastinator137 = 1; break;
			case 138: u.nastinator138 = 1; break;
			case 139: u.nastinator139 = 1; break;
			case 140: u.nastinator140 = 1; break;
			case 141: u.nastinator141 = 1; break;
			case 142: u.nastinator142 = 1; break;
			case 143: u.nastinator143 = 1; break;
			case 144: u.nastinator144 = 1; break;
			case 145: u.nastinator145 = 1; break;
			case 146: u.nastinator146 = 1; break;
			case 147: u.nastinator147 = 1; break;
			case 148: u.nastinator148 = 1; break;
			case 149: u.nastinator149 = 1; break;
			case 150: u.nastinator150 = 1; break;
			case 151: u.nastinator151 = 1; break;
			case 152: u.nastinator152 = 1; break;
			case 153: u.nastinator153 = 1; break;
			case 154: u.nastinator154 = 1; break;
			case 155: u.nastinator155 = 1; break;
			case 156: u.nastinator156 = 1; break;
			case 157: u.nastinator157 = 1; break;
			case 158: u.nastinator158 = 1; break;
			case 159: u.nastinator159 = 1; break;
			case 160: u.nastinator160 = 1; break;
			case 161: u.nastinator161 = 1; break;
			case 162: u.nastinator162 = 1; break;
			case 163: u.nastinator163 = 1; break;
			case 164: u.nastinator164 = 1; break;
			case 165: u.nastinator165 = 1; break;
			case 166: u.nastinator166 = 1; break;
			case 167: u.nastinator167 = 1; break;
			case 168: u.nastinator168 = 1; break;
			case 169: u.nastinator169 = 1; break;

		}

		while (!rn2(3)) {

			switch (rnd(169)) {

				case 1: u.nastinator01 = 1; break;
				case 2: u.nastinator02 = 1; break;
				case 3: u.nastinator03 = 1; break;
				case 4: u.nastinator04 = 1; break;
				case 5: u.nastinator05 = 1; break;
				case 6: u.nastinator06 = 1; break;
				case 7: u.nastinator07 = 1; break;
				case 8: u.nastinator08 = 1; break;
				case 9: u.nastinator09 = 1; break;
				case 10: u.nastinator10 = 1; break;
				case 11: u.nastinator11 = 1; break;
				case 12: u.nastinator12 = 1; break;
				case 13: u.nastinator13 = 1; break;
				case 14: u.nastinator14 = 1; break;
				case 15: u.nastinator15 = 1; break;
				case 16: u.nastinator16 = 1; break;
				case 17: u.nastinator17 = 1; break;
				case 18: u.nastinator18 = 1; break;
				case 19: u.nastinator19 = 1; break;
				case 20: u.nastinator20 = 1; break;
				case 21: u.nastinator21 = 1; break;
				case 22: u.nastinator22 = 1; break;
				case 23: u.nastinator23 = 1; break;
				case 24: u.nastinator24 = 1; break;
				case 25: u.nastinator25 = 1; break;
				case 26: u.nastinator26 = 1; break;
				case 27: u.nastinator27 = 1; break;
				case 28: u.nastinator28 = 1; break;
				case 29: u.nastinator29 = 1; break;
				case 30: u.nastinator30 = 1; break;
				case 31: u.nastinator31 = 1; break;
				case 32: u.nastinator32 = 1; break;
				case 33: u.nastinator33 = 1; break;
				case 34: u.nastinator34 = 1; break;
				case 35: u.nastinator35 = 1; break;
				case 36: u.nastinator36 = 1; break;
				case 37: u.nastinator37 = 1; break;
				case 38: u.nastinator38 = 1; break;
				case 39: u.nastinator39 = 1; break;
				case 40: u.nastinator40 = 1; break;
				case 41: u.nastinator41 = 1; break;
				case 42: u.nastinator42 = 1; break;
				case 43: u.nastinator43 = 1; break;
				case 44: u.nastinator44 = 1; break;
				case 45: u.nastinator45 = 1; break;
				case 46: u.nastinator46 = 1; break;
				case 47: u.nastinator47 = 1; break;
				case 48: u.nastinator48 = 1; break;
				case 49: u.nastinator49 = 1; break;
				case 50: u.nastinator50 = 1; break;
				case 51: u.nastinator51 = 1; break;
				case 52: u.nastinator52 = 1; break;
				case 53: u.nastinator53 = 1; break;
				case 54: u.nastinator54 = 1; break;
				case 55: u.nastinator55 = 1; break;
				case 56: u.nastinator56 = 1; break;
				case 57: u.nastinator57 = 1; break;
				case 58: u.nastinator58 = 1; break;
				case 59: u.nastinator59 = 1; break;
				case 60: u.nastinator60 = 1; break;
				case 61: u.nastinator61 = 1; break;
				case 62: u.nastinator62 = 1; break;
				case 63: u.nastinator63 = 1; break;
				case 64: u.nastinator64 = 1; break;
				case 65: u.nastinator65 = 1; break;
				case 66: u.nastinator66 = 1; break;
				case 67: u.nastinator67 = 1; break;
				case 68: u.nastinator68 = 1; break;
				case 69: u.nastinator69 = 1; break;
				case 70: u.nastinator70 = 1; break;
				case 71: u.nastinator71 = 1; break;
				case 72: u.nastinator72 = 1; break;
				case 73: u.nastinator73 = 1; break;
				case 74: u.nastinator74 = 1; break;
				case 75: u.nastinator75 = 1; break;
				case 76: u.nastinator76 = 1; break;
				case 77: u.nastinator77 = 1; break;
				case 78: u.nastinator78 = 1; break;
				case 79: u.nastinator79 = 1; break;
				case 80: u.nastinator80 = 1; break;
				case 81: u.nastinator81 = 1; break;
				case 82: u.nastinator82 = 1; break;
				case 83: u.nastinator83 = 1; break;
				case 84: u.nastinator84 = 1; break;
				case 85: u.nastinator85 = 1; break;
			case 86: u.nastinator86 = 1; break;
			case 87: u.nastinator87 = 1; break;
			case 88: u.nastinator88 = 1; break;
			case 89: u.nastinator89 = 1; break;
			case 90: u.nastinator90 = 1; break;
			case 91: u.nastinator91 = 1; break;
			case 92: u.nastinator92 = 1; break;
			case 93: u.nastinator93 = 1; break;
			case 94: u.nastinator94 = 1; break;
			case 95: u.nastinator95 = 1; break;
			case 96: u.nastinator96 = 1; break;
			case 97: u.nastinator97 = 1; break;
			case 98: u.nastinator98 = 1; break;
			case 99: u.nastinator99 = 1; break;
			case 100: u.nastinator100 = 1; break;
			case 101: u.nastinator101 = 1; break;
			case 102: u.nastinator102 = 1; break;
			case 103: u.nastinator103 = 1; break;
			case 104: u.nastinator104 = 1; break;
			case 105: u.nastinator105 = 1; break;
			case 106: u.nastinator106 = 1; break;
			case 107: u.nastinator107 = 1; break;
			case 108: u.nastinator108 = 1; break;
			case 109: u.nastinator109 = 1; break;
			case 110: u.nastinator110 = 1; break;
			case 111: u.nastinator111 = 1; break;
			case 112: u.nastinator112 = 1; break;
			case 113: u.nastinator113 = 1; break;
			case 114: u.nastinator114 = 1; break;
			case 115: u.nastinator115 = 1; break;
			case 116: u.nastinator116 = 1; break;
			case 117: u.nastinator117 = 1; break;
			case 118: u.nastinator118 = 1; break;
			case 119: u.nastinator119 = 1; break;
			case 120: u.nastinator120 = 1; break;
			case 121: u.nastinator121 = 1; break;
			case 122: u.nastinator122 = 1; break;
			case 123: u.nastinator123 = 1; break;
			case 124: u.nastinator124 = 1; break;
			case 125: u.nastinator125 = 1; break;
			case 126: u.nastinator126 = 1; break;
			case 127: u.nastinator127 = 1; break;
			case 128: u.nastinator128 = 1; break;
			case 129: u.nastinator129 = 1; break;
			case 130: u.nastinator130 = 1; break;
			case 131: u.nastinator131 = 1; break;
			case 132: u.nastinator132 = 1; break;
			case 133: u.nastinator133 = 1; break;
			case 134: u.nastinator134 = 1; break;
			case 135: u.nastinator135 = 1; break;
			case 136: u.nastinator136 = 1; break;
			case 137: u.nastinator137 = 1; break;
			case 138: u.nastinator138 = 1; break;
			case 139: u.nastinator139 = 1; break;
			case 140: u.nastinator140 = 1; break;
			case 141: u.nastinator141 = 1; break;
			case 142: u.nastinator142 = 1; break;
			case 143: u.nastinator143 = 1; break;
			case 144: u.nastinator144 = 1; break;
			case 145: u.nastinator145 = 1; break;
			case 146: u.nastinator146 = 1; break;
			case 147: u.nastinator147 = 1; break;
			case 148: u.nastinator148 = 1; break;
			case 149: u.nastinator149 = 1; break;
			case 150: u.nastinator150 = 1; break;
			case 151: u.nastinator151 = 1; break;
			case 152: u.nastinator152 = 1; break;
			case 153: u.nastinator153 = 1; break;
			case 154: u.nastinator154 = 1; break;
			case 155: u.nastinator155 = 1; break;
			case 156: u.nastinator156 = 1; break;
			case 157: u.nastinator157 = 1; break;
			case 158: u.nastinator158 = 1; break;
			case 159: u.nastinator159 = 1; break;
			case 160: u.nastinator160 = 1; break;
			case 161: u.nastinator161 = 1; break;
			case 162: u.nastinator162 = 1; break;
			case 163: u.nastinator163 = 1; break;
			case 164: u.nastinator164 = 1; break;
			case 165: u.nastinator165 = 1; break;
			case 166: u.nastinator166 = 1; break;
			case 167: u.nastinator167 = 1; break;
			case 168: u.nastinator168 = 1; break;
			case 169: u.nastinator169 = 1; break;

			}

		}

	if (isnastinator) {

		switch (rnd(169)) {

			case 1: u.nastinator01 = 1; break;
			case 2: u.nastinator02 = 1; break;
			case 3: u.nastinator03 = 1; break;
			case 4: u.nastinator04 = 1; break;
			case 5: u.nastinator05 = 1; break;
			case 6: u.nastinator06 = 1; break;
			case 7: u.nastinator07 = 1; break;
			case 8: u.nastinator08 = 1; break;
			case 9: u.nastinator09 = 1; break;
			case 10: u.nastinator10 = 1; break;
			case 11: u.nastinator11 = 1; break;
			case 12: u.nastinator12 = 1; break;
			case 13: u.nastinator13 = 1; break;
			case 14: u.nastinator14 = 1; break;
			case 15: u.nastinator15 = 1; break;
			case 16: u.nastinator16 = 1; break;
			case 17: u.nastinator17 = 1; break;
			case 18: u.nastinator18 = 1; break;
			case 19: u.nastinator19 = 1; break;
			case 20: u.nastinator20 = 1; break;
			case 21: u.nastinator21 = 1; break;
			case 22: u.nastinator22 = 1; break;
			case 23: u.nastinator23 = 1; break;
			case 24: u.nastinator24 = 1; break;
			case 25: u.nastinator25 = 1; break;
			case 26: u.nastinator26 = 1; break;
			case 27: u.nastinator27 = 1; break;
			case 28: u.nastinator28 = 1; break;
			case 29: u.nastinator29 = 1; break;
			case 30: u.nastinator30 = 1; break;
			case 31: u.nastinator31 = 1; break;
			case 32: u.nastinator32 = 1; break;
			case 33: u.nastinator33 = 1; break;
			case 34: u.nastinator34 = 1; break;
			case 35: u.nastinator35 = 1; break;
			case 36: u.nastinator36 = 1; break;
			case 37: u.nastinator37 = 1; break;
			case 38: u.nastinator38 = 1; break;
			case 39: u.nastinator39 = 1; break;
			case 40: u.nastinator40 = 1; break;
			case 41: u.nastinator41 = 1; break;
			case 42: u.nastinator42 = 1; break;
			case 43: u.nastinator43 = 1; break;
			case 44: u.nastinator44 = 1; break;
			case 45: u.nastinator45 = 1; break;
			case 46: u.nastinator46 = 1; break;
			case 47: u.nastinator47 = 1; break;
			case 48: u.nastinator48 = 1; break;
			case 49: u.nastinator49 = 1; break;
			case 50: u.nastinator50 = 1; break;
			case 51: u.nastinator51 = 1; break;
			case 52: u.nastinator52 = 1; break;
			case 53: u.nastinator53 = 1; break;
			case 54: u.nastinator54 = 1; break;
			case 55: u.nastinator55 = 1; break;
			case 56: u.nastinator56 = 1; break;
			case 57: u.nastinator57 = 1; break;
			case 58: u.nastinator58 = 1; break;
			case 59: u.nastinator59 = 1; break;
			case 60: u.nastinator60 = 1; break;
			case 61: u.nastinator61 = 1; break;
			case 62: u.nastinator62 = 1; break;
			case 63: u.nastinator63 = 1; break;
			case 64: u.nastinator64 = 1; break;
			case 65: u.nastinator65 = 1; break;
			case 66: u.nastinator66 = 1; break;
			case 67: u.nastinator67 = 1; break;
			case 68: u.nastinator68 = 1; break;
			case 69: u.nastinator69 = 1; break;
			case 70: u.nastinator70 = 1; break;
			case 71: u.nastinator71 = 1; break;
			case 72: u.nastinator72 = 1; break;
			case 73: u.nastinator73 = 1; break;
			case 74: u.nastinator74 = 1; break;
			case 75: u.nastinator75 = 1; break;
			case 76: u.nastinator76 = 1; break;
			case 77: u.nastinator77 = 1; break;
			case 78: u.nastinator78 = 1; break;
			case 79: u.nastinator79 = 1; break;
			case 80: u.nastinator80 = 1; break;
			case 81: u.nastinator81 = 1; break;
			case 82: u.nastinator82 = 1; break;
			case 83: u.nastinator83 = 1; break;
			case 84: u.nastinator84 = 1; break;
			case 85: u.nastinator85 = 1; break;
			case 86: u.nastinator86 = 1; break;
			case 87: u.nastinator87 = 1; break;
			case 88: u.nastinator88 = 1; break;
			case 89: u.nastinator89 = 1; break;
			case 90: u.nastinator90 = 1; break;
			case 91: u.nastinator91 = 1; break;
			case 92: u.nastinator92 = 1; break;
			case 93: u.nastinator93 = 1; break;
			case 94: u.nastinator94 = 1; break;
			case 95: u.nastinator95 = 1; break;
			case 96: u.nastinator96 = 1; break;
			case 97: u.nastinator97 = 1; break;
			case 98: u.nastinator98 = 1; break;
			case 99: u.nastinator99 = 1; break;
			case 100: u.nastinator100 = 1; break;
			case 101: u.nastinator101 = 1; break;
			case 102: u.nastinator102 = 1; break;
			case 103: u.nastinator103 = 1; break;
			case 104: u.nastinator104 = 1; break;
			case 105: u.nastinator105 = 1; break;
			case 106: u.nastinator106 = 1; break;
			case 107: u.nastinator107 = 1; break;
			case 108: u.nastinator108 = 1; break;
			case 109: u.nastinator109 = 1; break;
			case 110: u.nastinator110 = 1; break;
			case 111: u.nastinator111 = 1; break;
			case 112: u.nastinator112 = 1; break;
			case 113: u.nastinator113 = 1; break;
			case 114: u.nastinator114 = 1; break;
			case 115: u.nastinator115 = 1; break;
			case 116: u.nastinator116 = 1; break;
			case 117: u.nastinator117 = 1; break;
			case 118: u.nastinator118 = 1; break;
			case 119: u.nastinator119 = 1; break;
			case 120: u.nastinator120 = 1; break;
			case 121: u.nastinator121 = 1; break;
			case 122: u.nastinator122 = 1; break;
			case 123: u.nastinator123 = 1; break;
			case 124: u.nastinator124 = 1; break;
			case 125: u.nastinator125 = 1; break;
			case 126: u.nastinator126 = 1; break;
			case 127: u.nastinator127 = 1; break;
			case 128: u.nastinator128 = 1; break;
			case 129: u.nastinator129 = 1; break;
			case 130: u.nastinator130 = 1; break;
			case 131: u.nastinator131 = 1; break;
			case 132: u.nastinator132 = 1; break;
			case 133: u.nastinator133 = 1; break;
			case 134: u.nastinator134 = 1; break;
			case 135: u.nastinator135 = 1; break;
			case 136: u.nastinator136 = 1; break;
			case 137: u.nastinator137 = 1; break;
			case 138: u.nastinator138 = 1; break;
			case 139: u.nastinator139 = 1; break;
			case 140: u.nastinator140 = 1; break;
			case 141: u.nastinator141 = 1; break;
			case 142: u.nastinator142 = 1; break;
			case 143: u.nastinator143 = 1; break;
			case 144: u.nastinator144 = 1; break;
			case 145: u.nastinator145 = 1; break;
			case 146: u.nastinator146 = 1; break;
			case 147: u.nastinator147 = 1; break;
			case 148: u.nastinator148 = 1; break;
			case 149: u.nastinator149 = 1; break;
			case 150: u.nastinator150 = 1; break;
			case 151: u.nastinator151 = 1; break;
			case 152: u.nastinator152 = 1; break;
			case 153: u.nastinator153 = 1; break;
			case 154: u.nastinator154 = 1; break;
			case 155: u.nastinator155 = 1; break;
			case 156: u.nastinator156 = 1; break;
			case 157: u.nastinator157 = 1; break;
			case 158: u.nastinator158 = 1; break;
			case 159: u.nastinator159 = 1; break;
			case 160: u.nastinator160 = 1; break;
			case 161: u.nastinator161 = 1; break;
			case 162: u.nastinator162 = 1; break;
			case 163: u.nastinator163 = 1; break;
			case 164: u.nastinator164 = 1; break;
			case 165: u.nastinator165 = 1; break;
			case 166: u.nastinator166 = 1; break;
			case 167: u.nastinator167 = 1; break;
			case 168: u.nastinator168 = 1; break;
			case 169: u.nastinator169 = 1; break;

		}

		while ((rnd(7)) < 3) {

			switch (rnd(169)) {

				case 1: u.nastinator01 = 1; break;
				case 2: u.nastinator02 = 1; break;
				case 3: u.nastinator03 = 1; break;
				case 4: u.nastinator04 = 1; break;
				case 5: u.nastinator05 = 1; break;
				case 6: u.nastinator06 = 1; break;
				case 7: u.nastinator07 = 1; break;
				case 8: u.nastinator08 = 1; break;
				case 9: u.nastinator09 = 1; break;
				case 10: u.nastinator10 = 1; break;
				case 11: u.nastinator11 = 1; break;
				case 12: u.nastinator12 = 1; break;
				case 13: u.nastinator13 = 1; break;
				case 14: u.nastinator14 = 1; break;
				case 15: u.nastinator15 = 1; break;
				case 16: u.nastinator16 = 1; break;
				case 17: u.nastinator17 = 1; break;
				case 18: u.nastinator18 = 1; break;
				case 19: u.nastinator19 = 1; break;
				case 20: u.nastinator20 = 1; break;
				case 21: u.nastinator21 = 1; break;
				case 22: u.nastinator22 = 1; break;
				case 23: u.nastinator23 = 1; break;
				case 24: u.nastinator24 = 1; break;
				case 25: u.nastinator25 = 1; break;
				case 26: u.nastinator26 = 1; break;
				case 27: u.nastinator27 = 1; break;
				case 28: u.nastinator28 = 1; break;
				case 29: u.nastinator29 = 1; break;
				case 30: u.nastinator30 = 1; break;
				case 31: u.nastinator31 = 1; break;
				case 32: u.nastinator32 = 1; break;
				case 33: u.nastinator33 = 1; break;
				case 34: u.nastinator34 = 1; break;
				case 35: u.nastinator35 = 1; break;
				case 36: u.nastinator36 = 1; break;
				case 37: u.nastinator37 = 1; break;
				case 38: u.nastinator38 = 1; break;
				case 39: u.nastinator39 = 1; break;
				case 40: u.nastinator40 = 1; break;
				case 41: u.nastinator41 = 1; break;
				case 42: u.nastinator42 = 1; break;
				case 43: u.nastinator43 = 1; break;
				case 44: u.nastinator44 = 1; break;
				case 45: u.nastinator45 = 1; break;
				case 46: u.nastinator46 = 1; break;
				case 47: u.nastinator47 = 1; break;
				case 48: u.nastinator48 = 1; break;
				case 49: u.nastinator49 = 1; break;
				case 50: u.nastinator50 = 1; break;
				case 51: u.nastinator51 = 1; break;
				case 52: u.nastinator52 = 1; break;
				case 53: u.nastinator53 = 1; break;
				case 54: u.nastinator54 = 1; break;
				case 55: u.nastinator55 = 1; break;
				case 56: u.nastinator56 = 1; break;
				case 57: u.nastinator57 = 1; break;
				case 58: u.nastinator58 = 1; break;
				case 59: u.nastinator59 = 1; break;
				case 60: u.nastinator60 = 1; break;
				case 61: u.nastinator61 = 1; break;
				case 62: u.nastinator62 = 1; break;
				case 63: u.nastinator63 = 1; break;
				case 64: u.nastinator64 = 1; break;
				case 65: u.nastinator65 = 1; break;
				case 66: u.nastinator66 = 1; break;
				case 67: u.nastinator67 = 1; break;
				case 68: u.nastinator68 = 1; break;
				case 69: u.nastinator69 = 1; break;
				case 70: u.nastinator70 = 1; break;
				case 71: u.nastinator71 = 1; break;
				case 72: u.nastinator72 = 1; break;
				case 73: u.nastinator73 = 1; break;
				case 74: u.nastinator74 = 1; break;
				case 75: u.nastinator75 = 1; break;
				case 76: u.nastinator76 = 1; break;
				case 77: u.nastinator77 = 1; break;
				case 78: u.nastinator78 = 1; break;
				case 79: u.nastinator79 = 1; break;
				case 80: u.nastinator80 = 1; break;
				case 81: u.nastinator81 = 1; break;
				case 82: u.nastinator82 = 1; break;
				case 83: u.nastinator83 = 1; break;
				case 84: u.nastinator84 = 1; break;
				case 85: u.nastinator85 = 1; break;
			case 86: u.nastinator86 = 1; break;
			case 87: u.nastinator87 = 1; break;
			case 88: u.nastinator88 = 1; break;
			case 89: u.nastinator89 = 1; break;
			case 90: u.nastinator90 = 1; break;
			case 91: u.nastinator91 = 1; break;
			case 92: u.nastinator92 = 1; break;
			case 93: u.nastinator93 = 1; break;
			case 94: u.nastinator94 = 1; break;
			case 95: u.nastinator95 = 1; break;
			case 96: u.nastinator96 = 1; break;
			case 97: u.nastinator97 = 1; break;
			case 98: u.nastinator98 = 1; break;
			case 99: u.nastinator99 = 1; break;
			case 100: u.nastinator100 = 1; break;
			case 101: u.nastinator101 = 1; break;
			case 102: u.nastinator102 = 1; break;
			case 103: u.nastinator103 = 1; break;
			case 104: u.nastinator104 = 1; break;
			case 105: u.nastinator105 = 1; break;
			case 106: u.nastinator106 = 1; break;
			case 107: u.nastinator107 = 1; break;
			case 108: u.nastinator108 = 1; break;
			case 109: u.nastinator109 = 1; break;
			case 110: u.nastinator110 = 1; break;
			case 111: u.nastinator111 = 1; break;
			case 112: u.nastinator112 = 1; break;
			case 113: u.nastinator113 = 1; break;
			case 114: u.nastinator114 = 1; break;
			case 115: u.nastinator115 = 1; break;
			case 116: u.nastinator116 = 1; break;
			case 117: u.nastinator117 = 1; break;
			case 118: u.nastinator118 = 1; break;
			case 119: u.nastinator119 = 1; break;
			case 120: u.nastinator120 = 1; break;
			case 121: u.nastinator121 = 1; break;
			case 122: u.nastinator122 = 1; break;
			case 123: u.nastinator123 = 1; break;
			case 124: u.nastinator124 = 1; break;
			case 125: u.nastinator125 = 1; break;
			case 126: u.nastinator126 = 1; break;
			case 127: u.nastinator127 = 1; break;
			case 128: u.nastinator128 = 1; break;
			case 129: u.nastinator129 = 1; break;
			case 130: u.nastinator130 = 1; break;
			case 131: u.nastinator131 = 1; break;
			case 132: u.nastinator132 = 1; break;
			case 133: u.nastinator133 = 1; break;
			case 134: u.nastinator134 = 1; break;
			case 135: u.nastinator135 = 1; break;
			case 136: u.nastinator136 = 1; break;
			case 137: u.nastinator137 = 1; break;
			case 138: u.nastinator138 = 1; break;
			case 139: u.nastinator139 = 1; break;
			case 140: u.nastinator140 = 1; break;
			case 141: u.nastinator141 = 1; break;
			case 142: u.nastinator142 = 1; break;
			case 143: u.nastinator143 = 1; break;
			case 144: u.nastinator144 = 1; break;
			case 145: u.nastinator145 = 1; break;
			case 146: u.nastinator146 = 1; break;
			case 147: u.nastinator147 = 1; break;
			case 148: u.nastinator148 = 1; break;
			case 149: u.nastinator149 = 1; break;
			case 150: u.nastinator150 = 1; break;
			case 151: u.nastinator151 = 1; break;
			case 152: u.nastinator152 = 1; break;
			case 153: u.nastinator153 = 1; break;
			case 154: u.nastinator154 = 1; break;
			case 155: u.nastinator155 = 1; break;
			case 156: u.nastinator156 = 1; break;
			case 157: u.nastinator157 = 1; break;
			case 158: u.nastinator158 = 1; break;
			case 159: u.nastinator159 = 1; break;
			case 160: u.nastinator160 = 1; break;
			case 161: u.nastinator161 = 1; break;
			case 162: u.nastinator162 = 1; break;
			case 163: u.nastinator163 = 1; break;
			case 164: u.nastinator164 = 1; break;
			case 165: u.nastinator165 = 1; break;
			case 166: u.nastinator166 = 1; break;
			case 167: u.nastinator167 = 1; break;
			case 168: u.nastinator168 = 1; break;
			case 169: u.nastinator169 = 1; break;

			}

		}
	  }


	}

		if ( Itemcursing && !rn2(1000) ) {
			if (!Blind) {
				You("notice a %s glow surrounding you.", hcolor(NH_BLACK));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
			}
			rndcurse();

		}

		if (uarmc && uarmc->oartifact == ART_ARABELLA_S_LIGHTNINGROD && !rn2(1000) ) {
			if (!Blind) {
				You("notice a %s glow surrounding you.", hcolor(NH_BLACK));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
			}
			rndcurse();

		}

		if (uarmc && uarmc->oartifact == ART_ARABELLA_S_WAND_BOOSTER && !rn2(1000) ) {
			if (!Blind) {
				You("notice a %s glow surrounding you.", hcolor(NH_BLACK));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
			}
			rndcurse();
		}

		if (uwep && uwep->oartifact == ART_ALASSEA_TELEMNAR && !rn2(20000) ) {
			useupall(uwep);
			Your("weapon spontaneously disintegrates!");
		}
		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_ALASSEA_TELEMNAR && !rn2(20000) ) {
			useupall(uswapwep);
			Your("weapon spontaneously disintegrates!");
		}

		if (uwep && uwep->oartifact == ART_THRANDUIL_LOSSEHELIN && !rn2(20000) ) {
			useupall(uwep);
			Your("weapon spontaneously disintegrates!");
		}

		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_THRANDUIL_LOSSEHELIN && !rn2(20000) ) {
			useupall(uswapwep);
			Your("weapon spontaneously disintegrates!");
		}

		if (u.umoved && uwep && uwep->oartifact == ART_HENRIETTA_S_MISTAKE && !rn2(10)) {

			doshittrap((struct obj *)0);

		}

		if (u.umoved && u.twoweap && uswapwep && uswapwep->oartifact == ART_HENRIETTA_S_MISTAKE && !rn2(10)) {

			doshittrap((struct obj *)0);

		}

		if (uwep && uwep->oartifact == ART_AND_IT_KEEPS_ON_MOVING) pushplayersilently();
		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_AND_IT_KEEPS_ON_MOVING) pushplayersilently();

		if (uarmh && uarmh->oartifact == ART_ALLURATION && !rn2(1000) ) {
			curse(uarmh);
			if (!rn2(3)) pline("A black glow surrounds your helmet.");
			if (!rn2(20)) NastinessProblem |= FROMOUTSIDE;
		}

		if ((uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "netradiation helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "obluchonnyy shlem") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "sof radiatsiya dubulg'a") )) && !rn2(5000)) {

			pline("The helmet's radiation damages your health!");
			if (Upolyd && u.mhmax > 1) {
				u.mhmax--;
				if (u.mh > u.mhmax) u.mh = u.mhmax;
			} else if (!Upolyd && u.uhpmax > 1) {
				u.uhpmax--;
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			}
	
		}

		if (uarmh && uarmh->oartifact == ART_NULL_THE_LIVING_DATABASE && !rn2(5000) ) {
		      (void) cancel_monst(&youmonst, (struct obj *)0, FALSE, TRUE, FALSE);
		}

		if (!rn2(Aggravate_monster ? 4 : 20)) reset_rndmonst(NON_PM);

		if (Aggravate_monster && !rn2(Stealth ? 50000 : 5000)) {

			int aggroamount = rnd(6);
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
			while (aggroamount) {

				makemon((struct permonst *)0, u.ux, u.uy, MM_ANGRY);
				aggroamount--;
				if (aggroamount < 0) aggroamount = 0;
			}
			u.aggravation = 0;
			pline("Several monsters come out of a portal.");
			if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		}

		if (Role_if(PM_FEMINIST) && u.ualign.record < 0 && !rn2(Stealth ? 50000 : 5000)) {
		/* feminist aggravation idea by bugsniper */

			int aggroamount = rnd(6);
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
			while (aggroamount) {

			int attempts = 0;
			struct permonst *pm = 0;

newbossF:
			do {
				pm = rndmonst();
				attempts++;
				if (!rn2(2000)) reset_rndmonst(NON_PM);

			} while ( (!pm || (pm && !(pm->msound == MS_FART_LOUD || pm->msound == MS_FART_NORMAL || pm->msound == MS_FART_QUIET || pm->msound == MS_STENCH ))) && attempts < 50000);

			if (!pm && rn2(50) ) {
				attempts = 0;
				goto newbossF;
			}
			if (pm && !(pm->msound == MS_FART_LOUD || pm->msound == MS_FART_NORMAL || pm->msound == MS_FART_QUIET || pm->msound == MS_STENCH) && rn2(50) ) {
				attempts = 0;
				goto newbossF;
			}

			if (pm) (void) makemon(pm, u.ux, u.uy, NO_MM_FLAGS);

			} /* while (aggroamount) */

			pline("Several angry females come out of a portal.");
			if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

		}

		if (RngeBossEncounters && !rn2(10000) ) {
			int attempts = 0;
			register struct permonst *ptrZ;

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

newbossA:
			do {
				ptrZ = rndmonst();
				attempts++;
				if (!rn2(2000)) reset_rndmonst(NON_PM);

			} while ( (!ptrZ || (ptrZ && !(ptrZ->geno & G_UNIQ))) && attempts < 50000);

			if (ptrZ && ptrZ->geno & G_UNIQ) {
				if (wizard) pline("monster generation: %s", ptrZ->mname);
				(void) makemon(ptrZ, 0, 0, NO_MM_FLAGS);
			}
			else if (rn2(50)) {
				attempts = 0;
				goto newbossA;
			}

			u.aggravation = 0;

		}

		if (RngeImmobility && !rn2(5000) ) {

			int monstcnt;
			monstcnt = 8 + rnd(10);
			int sessileattempts;
			int sessilemnum;

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

		      while(--monstcnt >= 0) {
				for (sessileattempts = 0; sessileattempts < 100; sessileattempts++) {
					sessilemnum = rndmonnum();
					if (sessilemnum != -1 && (mons[sessilemnum].mlet != S_TROVE) && is_nonmoving(&mons[sessilemnum]) ) sessileattempts = 100;
				}
				if (sessilemnum != -1) (void) makemon( &mons[sessilemnum], u.ux, u.uy, NO_MM_FLAGS);
			}
			pline("You're immobilized by stationary monsters!");

			u.aggravation = 0;

		}

		if (RngePunishment && !rn2(1000) && !Punished) {
			punish((struct obj *)0);
		}

		if (RngeVortices && !rn2(2000)) {

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

	 	    (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY);
	 	    (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY);
	 	    (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY);
	 	    if (!rn2(3)) (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY);
	 	    if (!rn2(5)) (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY);
	 	    if (!rn2(15)) (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY);

			u.aggravation = 0;
		}

		if (RngeExplosions && !rn2(2000)) {

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

	 	    (void) makemon(mkclass(S_LIGHT,0), u.ux, u.uy, MM_ANGRY);
	 	    (void) makemon(mkclass(S_LIGHT,0), u.ux, u.uy, MM_ANGRY);
	 	    (void) makemon(mkclass(S_LIGHT,0), u.ux, u.uy, MM_ANGRY);
	 	    if (!rn2(3)) (void) makemon(mkclass(S_LIGHT,0), u.ux, u.uy, MM_ANGRY);
	 	    if (!rn2(5)) (void) makemon(mkclass(S_LIGHT,0), u.ux, u.uy, MM_ANGRY);
	 	    if (!rn2(15)) (void) makemon(mkclass(S_LIGHT,0), u.ux, u.uy, MM_ANGRY);

			u.aggravation = 0;
		}

		/* "ether disease" similar to Elona - any ether items in your inventory will cause this --Amy */
		if (numberofetheritems() > 0) {

			int etherslow = numberofetheritems();
			int etherquick = numberofwornetheritems();
			/* ones that are actually worn are much worse */

			while (etherslow > 0) {
				etherslow--;
				if (!rn2(1000)) contaminate(rnd(2), FALSE);
			}
			while (etherquick > 0) {
				etherquick--;
				if (!rn2(1000)) {
					if (!rn2(5)) {
						pline("Your ether equipment causes your contamination to deteriorate.");
						contaminate(rnd(10), TRUE);
					} else {
						contaminate(rnd(10), FALSE);
					}
				}
			}

		}

		if (In_gammacaves(&u.uz) && !rn2(100)) {
			if (!rn2(5)) {
				pline("The background radiation in the Gamma Caves contaminates you.");
				contaminate(rnz(25), TRUE);
			} else {
				contaminate(rnz(25), FALSE);
			}
		}

		if (In_sewerplant(&u.uz) && !rn2(250) && !Breathless) {
			poisoned("sewer air", rn2(A_MAX), "the poisonous air in the sewer plant", 30);
		}

		if (In_hellbathroom(&u.uz) && !rn2(1000) && !Breathless) {
			poisoned("bad air", rn2(A_MAX), "the bad air in Hell's bathroom", 30);
		}

		if (uarmc && uarmc->oartifact == ART_PROZACELF_S_AUTOHEALER && !rn2(1000) ) {

			u.uhpmax++;
			if (Upolyd) u.mhmax++;
			Your("health increases, at the cost of nasty side effects.");
			NastinessProblem += rnd(1000);

		}

		if (Role_if(PM_FEMINIST) && !rn2(5000)) {
			bad_artifact_xtra();
		}

		if (Role_if(PM_TRACER) && !rn2(10000)) {

			acqo = mk_artifact((struct obj *)0, !rn2(3) ? A_CHAOTIC : rn2(2) ? A_NEUTRAL : A_LAWFUL);
			if (acqo) {
			    dropy(acqo);
				if (P_MAX_SKILL(get_obj_skill(acqo)) == P_ISRESTRICTED) {
					unrestrict_weapon_skill(get_obj_skill(acqo));
				} else if (P_MAX_SKILL(get_obj_skill(acqo)) == P_UNSKILLED) {
					unrestrict_weapon_skill(get_obj_skill(acqo));
					P_MAX_SKILL(get_obj_skill(acqo)) = P_BASIC;
				} else if (rn2(2) && P_MAX_SKILL(get_obj_skill(acqo)) == P_BASIC) {
					P_MAX_SKILL(get_obj_skill(acqo)) = P_SKILLED;
				} else if (!rn2(4) && P_MAX_SKILL(get_obj_skill(acqo)) == P_SKILLED) {
					P_MAX_SKILL(get_obj_skill(acqo)) = P_EXPERT;
				} else if (!rn2(10) && P_MAX_SKILL(get_obj_skill(acqo)) == P_EXPERT) {
					P_MAX_SKILL(get_obj_skill(acqo)) = P_MASTER;
				} else if (!rn2(100) && P_MAX_SKILL(get_obj_skill(acqo)) == P_MASTER) {
					P_MAX_SKILL(get_obj_skill(acqo)) = P_GRAND_MASTER;
				} else if (!rn2(200) && P_MAX_SKILL(get_obj_skill(acqo)) == P_GRAND_MASTER) {
					P_MAX_SKILL(get_obj_skill(acqo)) = P_SUPREME_MASTER;
				}

			    discover_artifact(acqo->oartifact);

				if (!u.ugifts) u.ugifts = 1;
				pline("An artifact appeared beneath you!");

			}

		}

		if (!rn2(2000) && ArtificerBug) {
			bad_artifact();
		}

		if (!rn2(2000) && uimplant && uimplant->oartifact == ART_HENRIETTA_S_TENACIOUSNESS) {
			bad_artifact();
		}

		if (!rn2(2000) && EvilPatchEffect) {
			
			switch (rnd(169)) {

				case 1: 
				    SpeedBug |= FROMOUTSIDE; break;
				case 2: 
				    MenuBug |= FROMOUTSIDE; break;
				case 3: 
				    RMBLoss |= FROMOUTSIDE; break;
				case 4: 
				    DisplayLoss |= FROMOUTSIDE; break;
				case 5: 
				    SpellLoss |= FROMOUTSIDE; break;
				case 6: 
				    YellowSpells |= FROMOUTSIDE; break;
				case 7: 
				    AutoDestruct |= FROMOUTSIDE; break;
				case 8: 
				    MemoryLoss |= FROMOUTSIDE; break;
				case 9: 
				    InventoryLoss |= FROMOUTSIDE; break;
				case 10: 
				    BlackNgWalls |= FROMOUTSIDE; break;
				case 11: 
				    Superscroller |= FROMOUTSIDE; break;
				case 12: 
				    FreeHandLoss |= FROMOUTSIDE; break;
				case 13: 
				    Unidentify |= FROMOUTSIDE; break;
				case 14: 
				    Thirst |= FROMOUTSIDE; break;
				case 15: 
				    LuckLoss |= FROMOUTSIDE; break;
				case 16: 
				    ShadesOfGrey |= FROMOUTSIDE; break;
				case 17: 
				    FaintActive |= FROMOUTSIDE; break;
				case 18: 
				    Itemcursing |= FROMOUTSIDE; break;
				case 19: 
				    DifficultyIncreased |= FROMOUTSIDE; break;
				case 20: 
				    Deafness |= FROMOUTSIDE; break;
				case 21: 
				    CasterProblem |= FROMOUTSIDE; break;
				case 22: 
				    WeaknessProblem |= FROMOUTSIDE; break;
				case 23: 
				    RotThirteen |= FROMOUTSIDE; break;
				case 24: 
				    BishopGridbug |= FROMOUTSIDE; break;
				case 25: 
				    ConfusionProblem |= FROMOUTSIDE; break;
				case 26: 
				    NoDropProblem |= FROMOUTSIDE; break;
				case 27: 
				    DSTWProblem |= FROMOUTSIDE; break;
				case 28: 
				    StatusTrapProblem |= FROMOUTSIDE; break;
				case 29: 
				    AlignmentProblem |= FROMOUTSIDE; break;
				case 30: 
				    StairsProblem |= FROMOUTSIDE; break;
				case 31: 
				    UninformationProblem |= FROMOUTSIDE; break;
				case 32: 
				    IntrinsicLossProblem |= FROMOUTSIDE; break;
				case 33: 
				    BloodLossProblem |= FROMOUTSIDE; break;
				case 34: 
				    BadEffectProblem |= FROMOUTSIDE; break;
				case 35: 
				    TrapCreationProblem |= FROMOUTSIDE; break;
				case 36: 
				    AutomaticVulnerabilitiy |= FROMOUTSIDE; break;
				case 37: 
				    TeleportingItems |= FROMOUTSIDE; break;
				case 38: 
				    NastinessProblem |= FROMOUTSIDE; break;
				case 39: 
				    RecurringAmnesia |= FROMOUTSIDE; break;
				case 40: 
				    BigscriptEffect |= FROMOUTSIDE; break;
				case 41: 
				    BankTrapEffect |= FROMOUTSIDE; break;
				case 42: 
				    MapTrapEffect |= FROMOUTSIDE; break;
				case 43: 
				    TechTrapEffect |= FROMOUTSIDE; break;
				case 44: 
				    RecurringDisenchant |= FROMOUTSIDE; break;
				case 45: 
				    verisiertEffect |= FROMOUTSIDE; break;
				case 46: 
				    ChaosTerrain |= FROMOUTSIDE; break;
				case 47: 
				    Muteness |= FROMOUTSIDE; break;
				case 48: 
				    EngravingDoesntWork |= FROMOUTSIDE; break;
				case 49: 
				    MagicDeviceEffect |= FROMOUTSIDE; break;
				case 50: 
				    BookTrapEffect |= FROMOUTSIDE; break;
				case 51: 
				    LevelTrapEffect |= FROMOUTSIDE; break;
				case 52: 
				    QuizTrapEffect |= FROMOUTSIDE; break;
				case 53: 
				    CaptchaProblem |= FROMOUTSIDE; break;
				case 54: 
				    FarlookProblem |= FROMOUTSIDE; break;
				case 55: 
				    RespawnProblem |= FROMOUTSIDE; break;
				case 56: 
				    FastMetabolismEffect |= FROMOUTSIDE; break;
				case 57: 
				    NoReturnEffect |= FROMOUTSIDE; break;
				case 58: 
				    AlwaysEgotypeMonsters |= FROMOUTSIDE; break;
				case 59: 
				    TimeGoesByFaster |= FROMOUTSIDE; break;
				case 60: 
				    FoodIsAlwaysRotten |= FROMOUTSIDE; break;
				case 61: 
				    AllSkillsUnskilled |= FROMOUTSIDE; break;
				case 62: 
				    AllStatsAreLower |= FROMOUTSIDE; break;
				case 63: 
				    PlayerCannotTrainSkills |= FROMOUTSIDE; break;
				case 64: 
				    PlayerCannotExerciseStats |= FROMOUTSIDE; break;
				case 65: 
				    TurnLimitation |= FROMOUTSIDE; break;
				case 66: 
				    WeakSight |= FROMOUTSIDE; break;
				case 67: 
				    RandomMessages |= FROMOUTSIDE; break;
				case 68: 
				    Desecration |= FROMOUTSIDE; break;
				case 69: 
				    StarvationEffect |= FROMOUTSIDE; break;
				case 70: 
				    NoDropsEffect |= FROMOUTSIDE; break;
				case 71: 
				    LowEffects |= FROMOUTSIDE; break;
				case 72: 
				    InvisibleTrapsEffect |= FROMOUTSIDE; break;
				case 73: 
				    GhostWorld |= FROMOUTSIDE; break;
				case 74: 
				    Dehydration |= FROMOUTSIDE; break;
				case 75: 
				    HateTrapEffect |= FROMOUTSIDE; break;
				case 76: 
				    TotterTrapEffect |= FROMOUTSIDE; break;
				case 77: 
				    Nonintrinsics |= FROMOUTSIDE; break;
				case 78: 
				    Dropcurses |= FROMOUTSIDE; break;
				case 79: 
				    Nakedness |= FROMOUTSIDE; break;
				case 80: 
				    Antileveling |= FROMOUTSIDE; break;
				case 81: 
				    ItemStealingEffect |= FROMOUTSIDE; break;
				case 82: 
				    Rebellions |= FROMOUTSIDE; break;
				case 83: 
				    CrapEffect |= FROMOUTSIDE; break;
				case 84: 
				    ProjectilesMisfire |= FROMOUTSIDE; break;
				case 85: 
				    WallTrapping |= FROMOUTSIDE; break;
				case 86: 
				    DisconnectedStairs |= FROMOUTSIDE; break;
				case 87: 
				    InterfaceScrewed |= FROMOUTSIDE; break;
				case 88: 
				    Bossfights |= FROMOUTSIDE; break;
				case 89: 
				    EntireLevelMode |= FROMOUTSIDE; break;
				case 90: 
				    BonesLevelChange |= FROMOUTSIDE; break;
				case 91: 
				    AutocursingEquipment |= FROMOUTSIDE; break;
				case 92: 
				    HighlevelStatus |= FROMOUTSIDE; break;
				case 93: 
				    SpellForgetting |= FROMOUTSIDE; break;
				case 94: 
				    SoundEffectBug |= FROMOUTSIDE; break;
				case 95: 
				    TimerunBug |= FROMOUTSIDE; break;
				case 96:
				    LootcutBug |= FROMOUTSIDE; break;
				case 97:
				    MonsterSpeedBug |= FROMOUTSIDE; break;
				case 98:
				    ScalingBug |= FROMOUTSIDE; break;
				case 99:
				    EnmityBug |= FROMOUTSIDE; break;
				case 100:
				    WhiteSpells |= FROMOUTSIDE; break;
				case 101:
				    CompleteGraySpells |= FROMOUTSIDE; break;
				case 102:
				    QuasarVision |= FROMOUTSIDE; break;
				case 103:
				    MommaBugEffect |= FROMOUTSIDE; break;
				case 104:
				    HorrorBugEffect |= FROMOUTSIDE; break;
				case 105:
				    ArtificerBug |= FROMOUTSIDE; break;
				case 106:
				    WereformBug |= FROMOUTSIDE; break;
				case 107:
				    NonprayerBug |= FROMOUTSIDE; break;
				case 108:
				    EvilPatchEffect |= FROMOUTSIDE; break;
				case 109:
				    HardModeEffect |= FROMOUTSIDE; break;
				case 110:
				    SecretAttackBug |= FROMOUTSIDE; break;
				case 111:
				    EaterBugEffect |= FROMOUTSIDE; break;
				case 112:
				    CovetousnessBug |= FROMOUTSIDE; break;
				case 113:
				    NotSeenBug |= FROMOUTSIDE; break;
				case 114:
				    DarkModeBug |= FROMOUTSIDE; break;
				case 115:
				    AntisearchEffect |= FROMOUTSIDE; break;
				case 116:
				    HomicideEffect |= FROMOUTSIDE; break;
				case 117:
				    NastynationBug |= FROMOUTSIDE; break;
				case 118:
				    WakeupCallBug |= FROMOUTSIDE; break;
				case 119:
				    GrayoutBug |= FROMOUTSIDE; break;
				case 120:
				    GrayCenterBug |= FROMOUTSIDE; break;
				case 121:
				    CheckerboardBug |= FROMOUTSIDE; break;
				case 122:
				    ClockwiseSpinBug |= FROMOUTSIDE; break;
				case 123:
				    CounterclockwiseSpin |= FROMOUTSIDE; break;
				case 124:
				    LagBugEffect |= FROMOUTSIDE; break;
				case 125:
				    BlesscurseEffect |= FROMOUTSIDE; break;
				case 126:
				    DeLightBug |= FROMOUTSIDE; break;
				case 127:
				    DischargeBug |= FROMOUTSIDE; break;
				case 128:
				    TrashingBugEffect |= FROMOUTSIDE; break;
				case 129:
				    FilteringBug |= FROMOUTSIDE; break;
				case 130:
				    DeformattingBug |= FROMOUTSIDE; break;
				case 131:
				    FlickerStripBug |= FROMOUTSIDE; break;
				case 132:
				    UndressingEffect |= FROMOUTSIDE; break;
				case 133:
				    Hyperbluewalls |= FROMOUTSIDE; break;
				case 134:
				    NoliteBug |= FROMOUTSIDE; break;
				case 135:
				    ParanoiaBugEffect |= FROMOUTSIDE; break;
				case 136:
				    FleecescriptBug |= FROMOUTSIDE; break;
				case 137:
				    InterruptEffect |= FROMOUTSIDE; break;
				case 138:
				    DustbinBug |= FROMOUTSIDE; break;
				case 139:
				    ManaBatteryBug |= FROMOUTSIDE; break;
				case 140:
				    Monsterfingers |= FROMOUTSIDE; break;
				case 141:
				    MiscastBug |= FROMOUTSIDE; break;
				case 142:
				    MessageSuppression |= FROMOUTSIDE; break;
				case 143:
				    StuckAnnouncement |= FROMOUTSIDE; break;
				case 144:
				    BloodthirstyEffect |= FROMOUTSIDE; break;
				case 145:
				    MaximumDamageBug |= FROMOUTSIDE; break;
				case 146:
				    LatencyBugEffect |= FROMOUTSIDE; break;
				case 147:
				    StarlitBug |= FROMOUTSIDE; break;
				case 148:
				    KnowledgeBug |= FROMOUTSIDE; break;
				case 149:
				    HighscoreBug |= FROMOUTSIDE; break;
				case 150:
				    PinkSpells |= FROMOUTSIDE; break;
				case 151:
				    GreenSpells |= FROMOUTSIDE; break;
				case 152:
				    EvencoreEffect |= FROMOUTSIDE; break;
				case 153:
				    UnderlayerBug |= FROMOUTSIDE; break;
				case 154:
				    DamageMeterBug |= FROMOUTSIDE; break;
				case 155:
				    ArbitraryWeightBug |= FROMOUTSIDE; break;
				case 156:
				    FuckedInfoBug |= FROMOUTSIDE; break;
				case 157:
				    BlackSpells |= FROMOUTSIDE; break;
				case 158:
				    CyanSpells |= FROMOUTSIDE; break;
				case 159:
				    HeapEffectBug |= FROMOUTSIDE; break;
				case 160:
				    BlueSpells |= FROMOUTSIDE; break;
				case 161:
				    TronEffect |= FROMOUTSIDE; break;
				case 162:
				    RedSpells |= FROMOUTSIDE; break;
				case 163:
				    TooHeavyEffect |= FROMOUTSIDE; break;
				case 164:
				    ElongationBug |= FROMOUTSIDE; break;
				case 165:
				    WrapoverEffect |= FROMOUTSIDE; break;
				case 166:
				    DestructionEffect |= FROMOUTSIDE; break;
				case 167:
				    MeleePrefixBug |= FROMOUTSIDE; break;
				case 168:
				    AutomoreBug |= FROMOUTSIDE; break;
				case 169:
				    UnfairAttackBug |= FROMOUTSIDE; break;
			}

		}

		if (!rn2(2000) && u.uprops[EVIL_PATCH_EFFECT].extrinsic) {
			
			switch (rnd(169)) {

				case 1: 
				    SpeedBug |= FROMOUTSIDE; break;
				case 2: 
				    MenuBug |= FROMOUTSIDE; break;
				case 3: 
				    RMBLoss |= FROMOUTSIDE; break;
				case 4: 
				    DisplayLoss |= FROMOUTSIDE; break;
				case 5: 
				    SpellLoss |= FROMOUTSIDE; break;
				case 6: 
				    YellowSpells |= FROMOUTSIDE; break;
				case 7: 
				    AutoDestruct |= FROMOUTSIDE; break;
				case 8: 
				    MemoryLoss |= FROMOUTSIDE; break;
				case 9: 
				    InventoryLoss |= FROMOUTSIDE; break;
				case 10: 
				    BlackNgWalls |= FROMOUTSIDE; break;
				case 11: 
				    Superscroller |= FROMOUTSIDE; break;
				case 12: 
				    FreeHandLoss |= FROMOUTSIDE; break;
				case 13: 
				    Unidentify |= FROMOUTSIDE; break;
				case 14: 
				    Thirst |= FROMOUTSIDE; break;
				case 15: 
				    LuckLoss |= FROMOUTSIDE; break;
				case 16: 
				    ShadesOfGrey |= FROMOUTSIDE; break;
				case 17: 
				    FaintActive |= FROMOUTSIDE; break;
				case 18: 
				    Itemcursing |= FROMOUTSIDE; break;
				case 19: 
				    DifficultyIncreased |= FROMOUTSIDE; break;
				case 20: 
				    Deafness |= FROMOUTSIDE; break;
				case 21: 
				    CasterProblem |= FROMOUTSIDE; break;
				case 22: 
				    WeaknessProblem |= FROMOUTSIDE; break;
				case 23: 
				    RotThirteen |= FROMOUTSIDE; break;
				case 24: 
				    BishopGridbug |= FROMOUTSIDE; break;
				case 25: 
				    ConfusionProblem |= FROMOUTSIDE; break;
				case 26: 
				    NoDropProblem |= FROMOUTSIDE; break;
				case 27: 
				    DSTWProblem |= FROMOUTSIDE; break;
				case 28: 
				    StatusTrapProblem |= FROMOUTSIDE; break;
				case 29: 
				    AlignmentProblem |= FROMOUTSIDE; break;
				case 30: 
				    StairsProblem |= FROMOUTSIDE; break;
				case 31: 
				    UninformationProblem |= FROMOUTSIDE; break;
				case 32: 
				    IntrinsicLossProblem |= FROMOUTSIDE; break;
				case 33: 
				    BloodLossProblem |= FROMOUTSIDE; break;
				case 34: 
				    BadEffectProblem |= FROMOUTSIDE; break;
				case 35: 
				    TrapCreationProblem |= FROMOUTSIDE; break;
				case 36: 
				    AutomaticVulnerabilitiy |= FROMOUTSIDE; break;
				case 37: 
				    TeleportingItems |= FROMOUTSIDE; break;
				case 38: 
				    NastinessProblem |= FROMOUTSIDE; break;
				case 39: 
				    RecurringAmnesia |= FROMOUTSIDE; break;
				case 40: 
				    BigscriptEffect |= FROMOUTSIDE; break;
				case 41: 
				    BankTrapEffect |= FROMOUTSIDE; break;
				case 42: 
				    MapTrapEffect |= FROMOUTSIDE; break;
				case 43: 
				    TechTrapEffect |= FROMOUTSIDE; break;
				case 44: 
				    RecurringDisenchant |= FROMOUTSIDE; break;
				case 45: 
				    verisiertEffect |= FROMOUTSIDE; break;
				case 46: 
				    ChaosTerrain |= FROMOUTSIDE; break;
				case 47: 
				    Muteness |= FROMOUTSIDE; break;
				case 48: 
				    EngravingDoesntWork |= FROMOUTSIDE; break;
				case 49: 
				    MagicDeviceEffect |= FROMOUTSIDE; break;
				case 50: 
				    BookTrapEffect |= FROMOUTSIDE; break;
				case 51: 
				    LevelTrapEffect |= FROMOUTSIDE; break;
				case 52: 
				    QuizTrapEffect |= FROMOUTSIDE; break;
				case 53: 
				    CaptchaProblem |= FROMOUTSIDE; break;
				case 54: 
				    FarlookProblem |= FROMOUTSIDE; break;
				case 55: 
				    RespawnProblem |= FROMOUTSIDE; break;
				case 56: 
				    FastMetabolismEffect |= FROMOUTSIDE; break;
				case 57: 
				    NoReturnEffect |= FROMOUTSIDE; break;
				case 58: 
				    AlwaysEgotypeMonsters |= FROMOUTSIDE; break;
				case 59: 
				    TimeGoesByFaster |= FROMOUTSIDE; break;
				case 60: 
				    FoodIsAlwaysRotten |= FROMOUTSIDE; break;
				case 61: 
				    AllSkillsUnskilled |= FROMOUTSIDE; break;
				case 62: 
				    AllStatsAreLower |= FROMOUTSIDE; break;
				case 63: 
				    PlayerCannotTrainSkills |= FROMOUTSIDE; break;
				case 64: 
				    PlayerCannotExerciseStats |= FROMOUTSIDE; break;
				case 65: 
				    TurnLimitation |= FROMOUTSIDE; break;
				case 66: 
				    WeakSight |= FROMOUTSIDE; break;
				case 67: 
				    RandomMessages |= FROMOUTSIDE; break;
				case 68: 
				    Desecration |= FROMOUTSIDE; break;
				case 69: 
				    StarvationEffect |= FROMOUTSIDE; break;
				case 70: 
				    NoDropsEffect |= FROMOUTSIDE; break;
				case 71: 
				    LowEffects |= FROMOUTSIDE; break;
				case 72: 
				    InvisibleTrapsEffect |= FROMOUTSIDE; break;
				case 73: 
				    GhostWorld |= FROMOUTSIDE; break;
				case 74: 
				    Dehydration |= FROMOUTSIDE; break;
				case 75: 
				    HateTrapEffect |= FROMOUTSIDE; break;
				case 76: 
				    TotterTrapEffect |= FROMOUTSIDE; break;
				case 77: 
				    Nonintrinsics |= FROMOUTSIDE; break;
				case 78: 
				    Dropcurses |= FROMOUTSIDE; break;
				case 79: 
				    Nakedness |= FROMOUTSIDE; break;
				case 80: 
				    Antileveling |= FROMOUTSIDE; break;
				case 81: 
				    ItemStealingEffect |= FROMOUTSIDE; break;
				case 82: 
				    Rebellions |= FROMOUTSIDE; break;
				case 83: 
				    CrapEffect |= FROMOUTSIDE; break;
				case 84: 
				    ProjectilesMisfire |= FROMOUTSIDE; break;
				case 85: 
				    WallTrapping |= FROMOUTSIDE; break;
				case 86: 
				    DisconnectedStairs |= FROMOUTSIDE; break;
				case 87: 
				    InterfaceScrewed |= FROMOUTSIDE; break;
				case 88: 
				    Bossfights |= FROMOUTSIDE; break;
				case 89: 
				    EntireLevelMode |= FROMOUTSIDE; break;
				case 90: 
				    BonesLevelChange |= FROMOUTSIDE; break;
				case 91: 
				    AutocursingEquipment |= FROMOUTSIDE; break;
				case 92: 
				    HighlevelStatus |= FROMOUTSIDE; break;
				case 93: 
				    SpellForgetting |= FROMOUTSIDE; break;
				case 94: 
				    SoundEffectBug |= FROMOUTSIDE; break;
				case 95: 
				    TimerunBug |= FROMOUTSIDE; break;
				case 96:
				    LootcutBug |= FROMOUTSIDE; break;
				case 97:
				    MonsterSpeedBug |= FROMOUTSIDE; break;
				case 98:
				    ScalingBug |= FROMOUTSIDE; break;
				case 99:
				    EnmityBug |= FROMOUTSIDE; break;
				case 100:
				    WhiteSpells |= FROMOUTSIDE; break;
				case 101:
				    CompleteGraySpells |= FROMOUTSIDE; break;
				case 102:
				    QuasarVision |= FROMOUTSIDE; break;
				case 103:
				    MommaBugEffect |= FROMOUTSIDE; break;
				case 104:
				    HorrorBugEffect |= FROMOUTSIDE; break;
				case 105:
				    ArtificerBug |= FROMOUTSIDE; break;
				case 106:
				    WereformBug |= FROMOUTSIDE; break;
				case 107:
				    NonprayerBug |= FROMOUTSIDE; break;
				case 108:
				    EvilPatchEffect |= FROMOUTSIDE; break;
				case 109:
				    HardModeEffect |= FROMOUTSIDE; break;
				case 110:
				    SecretAttackBug |= FROMOUTSIDE; break;
				case 111:
				    EaterBugEffect |= FROMOUTSIDE; break;
				case 112:
				    CovetousnessBug |= FROMOUTSIDE; break;
				case 113:
				    NotSeenBug |= FROMOUTSIDE; break;
				case 114:
				    DarkModeBug |= FROMOUTSIDE; break;
				case 115:
				    AntisearchEffect |= FROMOUTSIDE; break;
				case 116:
				    HomicideEffect |= FROMOUTSIDE; break;
				case 117:
				    NastynationBug |= FROMOUTSIDE; break;
				case 118:
				    WakeupCallBug |= FROMOUTSIDE; break;
				case 119:
				    GrayoutBug |= FROMOUTSIDE; break;
				case 120:
				    GrayCenterBug |= FROMOUTSIDE; break;
				case 121:
				    CheckerboardBug |= FROMOUTSIDE; break;
				case 122:
				    ClockwiseSpinBug |= FROMOUTSIDE; break;
				case 123:
				    CounterclockwiseSpin |= FROMOUTSIDE; break;
				case 124:
				    LagBugEffect |= FROMOUTSIDE; break;
				case 125:
				    BlesscurseEffect |= FROMOUTSIDE; break;
				case 126:
				    DeLightBug |= FROMOUTSIDE; break;
				case 127:
				    DischargeBug |= FROMOUTSIDE; break;
				case 128:
				    TrashingBugEffect |= FROMOUTSIDE; break;
				case 129:
				    FilteringBug |= FROMOUTSIDE; break;
				case 130:
				    DeformattingBug |= FROMOUTSIDE; break;
				case 131:
				    FlickerStripBug |= FROMOUTSIDE; break;
				case 132:
				    UndressingEffect |= FROMOUTSIDE; break;
				case 133:
				    Hyperbluewalls |= FROMOUTSIDE; break;
				case 134:
				    NoliteBug |= FROMOUTSIDE; break;
				case 135:
				    ParanoiaBugEffect |= FROMOUTSIDE; break;
				case 136:
				    FleecescriptBug |= FROMOUTSIDE; break;
				case 137:
				    InterruptEffect |= FROMOUTSIDE; break;
				case 138:
				    DustbinBug |= FROMOUTSIDE; break;
				case 139:
				    ManaBatteryBug |= FROMOUTSIDE; break;
				case 140:
				    Monsterfingers |= FROMOUTSIDE; break;
				case 141:
				    MiscastBug |= FROMOUTSIDE; break;
				case 142:
				    MessageSuppression |= FROMOUTSIDE; break;
				case 143:
				    StuckAnnouncement |= FROMOUTSIDE; break;
				case 144:
				    BloodthirstyEffect |= FROMOUTSIDE; break;
				case 145:
				    MaximumDamageBug |= FROMOUTSIDE; break;
				case 146:
				    LatencyBugEffect |= FROMOUTSIDE; break;
				case 147:
				    StarlitBug |= FROMOUTSIDE; break;
				case 148:
				    KnowledgeBug |= FROMOUTSIDE; break;
				case 149:
				    HighscoreBug |= FROMOUTSIDE; break;
				case 150:
				    PinkSpells |= FROMOUTSIDE; break;
				case 151:
				    GreenSpells |= FROMOUTSIDE; break;
				case 152:
				    EvencoreEffect |= FROMOUTSIDE; break;
				case 153:
				    UnderlayerBug |= FROMOUTSIDE; break;
				case 154:
				    DamageMeterBug |= FROMOUTSIDE; break;
				case 155:
				    ArbitraryWeightBug |= FROMOUTSIDE; break;
				case 156:
				    FuckedInfoBug |= FROMOUTSIDE; break;
				case 157:
				    BlackSpells |= FROMOUTSIDE; break;
				case 158:
				    CyanSpells |= FROMOUTSIDE; break;
				case 159:
				    HeapEffectBug |= FROMOUTSIDE; break;
				case 160:
				    BlueSpells |= FROMOUTSIDE; break;
				case 161:
				    TronEffect |= FROMOUTSIDE; break;
				case 162:
				    RedSpells |= FROMOUTSIDE; break;
				case 163:
				    TooHeavyEffect |= FROMOUTSIDE; break;
				case 164:
				    ElongationBug |= FROMOUTSIDE; break;
				case 165:
				    WrapoverEffect |= FROMOUTSIDE; break;
				case 166:
				    DestructionEffect |= FROMOUTSIDE; break;
				case 167:
				    MeleePrefixBug |= FROMOUTSIDE; break;
				case 168:
				    AutomoreBug |= FROMOUTSIDE; break;
				case 169:
				    UnfairAttackBug |= FROMOUTSIDE; break;
			}

		}

		if (!rn2(2000) && have_evilpatchstone()) {
			
			switch (rnd(169)) {

				case 1: 
				    SpeedBug |= FROMOUTSIDE; break;
				case 2: 
				    MenuBug |= FROMOUTSIDE; break;
				case 3: 
				    RMBLoss |= FROMOUTSIDE; break;
				case 4: 
				    DisplayLoss |= FROMOUTSIDE; break;
				case 5: 
				    SpellLoss |= FROMOUTSIDE; break;
				case 6: 
				    YellowSpells |= FROMOUTSIDE; break;
				case 7: 
				    AutoDestruct |= FROMOUTSIDE; break;
				case 8: 
				    MemoryLoss |= FROMOUTSIDE; break;
				case 9: 
				    InventoryLoss |= FROMOUTSIDE; break;
				case 10: 
				    BlackNgWalls |= FROMOUTSIDE; break;
				case 11: 
				    Superscroller |= FROMOUTSIDE; break;
				case 12: 
				    FreeHandLoss |= FROMOUTSIDE; break;
				case 13: 
				    Unidentify |= FROMOUTSIDE; break;
				case 14: 
				    Thirst |= FROMOUTSIDE; break;
				case 15: 
				    LuckLoss |= FROMOUTSIDE; break;
				case 16: 
				    ShadesOfGrey |= FROMOUTSIDE; break;
				case 17: 
				    FaintActive |= FROMOUTSIDE; break;
				case 18: 
				    Itemcursing |= FROMOUTSIDE; break;
				case 19: 
				    DifficultyIncreased |= FROMOUTSIDE; break;
				case 20: 
				    Deafness |= FROMOUTSIDE; break;
				case 21: 
				    CasterProblem |= FROMOUTSIDE; break;
				case 22: 
				    WeaknessProblem |= FROMOUTSIDE; break;
				case 23: 
				    RotThirteen |= FROMOUTSIDE; break;
				case 24: 
				    BishopGridbug |= FROMOUTSIDE; break;
				case 25: 
				    ConfusionProblem |= FROMOUTSIDE; break;
				case 26: 
				    NoDropProblem |= FROMOUTSIDE; break;
				case 27: 
				    DSTWProblem |= FROMOUTSIDE; break;
				case 28: 
				    StatusTrapProblem |= FROMOUTSIDE; break;
				case 29: 
				    AlignmentProblem |= FROMOUTSIDE; break;
				case 30: 
				    StairsProblem |= FROMOUTSIDE; break;
				case 31: 
				    UninformationProblem |= FROMOUTSIDE; break;
				case 32: 
				    IntrinsicLossProblem |= FROMOUTSIDE; break;
				case 33: 
				    BloodLossProblem |= FROMOUTSIDE; break;
				case 34: 
				    BadEffectProblem |= FROMOUTSIDE; break;
				case 35: 
				    TrapCreationProblem |= FROMOUTSIDE; break;
				case 36: 
				    AutomaticVulnerabilitiy |= FROMOUTSIDE; break;
				case 37: 
				    TeleportingItems |= FROMOUTSIDE; break;
				case 38: 
				    NastinessProblem |= FROMOUTSIDE; break;
				case 39: 
				    RecurringAmnesia |= FROMOUTSIDE; break;
				case 40: 
				    BigscriptEffect |= FROMOUTSIDE; break;
				case 41: 
				    BankTrapEffect |= FROMOUTSIDE; break;
				case 42: 
				    MapTrapEffect |= FROMOUTSIDE; break;
				case 43: 
				    TechTrapEffect |= FROMOUTSIDE; break;
				case 44: 
				    RecurringDisenchant |= FROMOUTSIDE; break;
				case 45: 
				    verisiertEffect |= FROMOUTSIDE; break;
				case 46: 
				    ChaosTerrain |= FROMOUTSIDE; break;
				case 47: 
				    Muteness |= FROMOUTSIDE; break;
				case 48: 
				    EngravingDoesntWork |= FROMOUTSIDE; break;
				case 49: 
				    MagicDeviceEffect |= FROMOUTSIDE; break;
				case 50: 
				    BookTrapEffect |= FROMOUTSIDE; break;
				case 51: 
				    LevelTrapEffect |= FROMOUTSIDE; break;
				case 52: 
				    QuizTrapEffect |= FROMOUTSIDE; break;
				case 53: 
				    CaptchaProblem |= FROMOUTSIDE; break;
				case 54: 
				    FarlookProblem |= FROMOUTSIDE; break;
				case 55: 
				    RespawnProblem |= FROMOUTSIDE; break;
				case 56: 
				    FastMetabolismEffect |= FROMOUTSIDE; break;
				case 57: 
				    NoReturnEffect |= FROMOUTSIDE; break;
				case 58: 
				    AlwaysEgotypeMonsters |= FROMOUTSIDE; break;
				case 59: 
				    TimeGoesByFaster |= FROMOUTSIDE; break;
				case 60: 
				    FoodIsAlwaysRotten |= FROMOUTSIDE; break;
				case 61: 
				    AllSkillsUnskilled |= FROMOUTSIDE; break;
				case 62: 
				    AllStatsAreLower |= FROMOUTSIDE; break;
				case 63: 
				    PlayerCannotTrainSkills |= FROMOUTSIDE; break;
				case 64: 
				    PlayerCannotExerciseStats |= FROMOUTSIDE; break;
				case 65: 
				    TurnLimitation |= FROMOUTSIDE; break;
				case 66: 
				    WeakSight |= FROMOUTSIDE; break;
				case 67: 
				    RandomMessages |= FROMOUTSIDE; break;
				case 68: 
				    Desecration |= FROMOUTSIDE; break;
				case 69: 
				    StarvationEffect |= FROMOUTSIDE; break;
				case 70: 
				    NoDropsEffect |= FROMOUTSIDE; break;
				case 71: 
				    LowEffects |= FROMOUTSIDE; break;
				case 72: 
				    InvisibleTrapsEffect |= FROMOUTSIDE; break;
				case 73: 
				    GhostWorld |= FROMOUTSIDE; break;
				case 74: 
				    Dehydration |= FROMOUTSIDE; break;
				case 75: 
				    HateTrapEffect |= FROMOUTSIDE; break;
				case 76: 
				    TotterTrapEffect |= FROMOUTSIDE; break;
				case 77: 
				    Nonintrinsics |= FROMOUTSIDE; break;
				case 78: 
				    Dropcurses |= FROMOUTSIDE; break;
				case 79: 
				    Nakedness |= FROMOUTSIDE; break;
				case 80: 
				    Antileveling |= FROMOUTSIDE; break;
				case 81: 
				    ItemStealingEffect |= FROMOUTSIDE; break;
				case 82: 
				    Rebellions |= FROMOUTSIDE; break;
				case 83: 
				    CrapEffect |= FROMOUTSIDE; break;
				case 84: 
				    ProjectilesMisfire |= FROMOUTSIDE; break;
				case 85: 
				    WallTrapping |= FROMOUTSIDE; break;
				case 86: 
				    DisconnectedStairs |= FROMOUTSIDE; break;
				case 87: 
				    InterfaceScrewed |= FROMOUTSIDE; break;
				case 88: 
				    Bossfights |= FROMOUTSIDE; break;
				case 89: 
				    EntireLevelMode |= FROMOUTSIDE; break;
				case 90: 
				    BonesLevelChange |= FROMOUTSIDE; break;
				case 91: 
				    AutocursingEquipment |= FROMOUTSIDE; break;
				case 92: 
				    HighlevelStatus |= FROMOUTSIDE; break;
				case 93: 
				    SpellForgetting |= FROMOUTSIDE; break;
				case 94: 
				    SoundEffectBug |= FROMOUTSIDE; break;
				case 95: 
				    TimerunBug |= FROMOUTSIDE; break;
				case 96:
				    LootcutBug |= FROMOUTSIDE; break;
				case 97:
				    MonsterSpeedBug |= FROMOUTSIDE; break;
				case 98:
				    ScalingBug |= FROMOUTSIDE; break;
				case 99:
				    EnmityBug |= FROMOUTSIDE; break;
				case 100:
				    WhiteSpells |= FROMOUTSIDE; break;
				case 101:
				    CompleteGraySpells |= FROMOUTSIDE; break;
				case 102:
				    QuasarVision |= FROMOUTSIDE; break;
				case 103:
				    MommaBugEffect |= FROMOUTSIDE; break;
				case 104:
				    HorrorBugEffect |= FROMOUTSIDE; break;
				case 105:
				    ArtificerBug |= FROMOUTSIDE; break;
				case 106:
				    WereformBug |= FROMOUTSIDE; break;
				case 107:
				    NonprayerBug |= FROMOUTSIDE; break;
				case 108:
				    EvilPatchEffect |= FROMOUTSIDE; break;
				case 109:
				    HardModeEffect |= FROMOUTSIDE; break;
				case 110:
				    SecretAttackBug |= FROMOUTSIDE; break;
				case 111:
				    EaterBugEffect |= FROMOUTSIDE; break;
				case 112:
				    CovetousnessBug |= FROMOUTSIDE; break;
				case 113:
				    NotSeenBug |= FROMOUTSIDE; break;
				case 114:
				    DarkModeBug |= FROMOUTSIDE; break;
				case 115:
				    AntisearchEffect |= FROMOUTSIDE; break;
				case 116:
				    HomicideEffect |= FROMOUTSIDE; break;
				case 117:
				    NastynationBug |= FROMOUTSIDE; break;
				case 118:
				    WakeupCallBug |= FROMOUTSIDE; break;
				case 119:
				    GrayoutBug |= FROMOUTSIDE; break;
				case 120:
				    GrayCenterBug |= FROMOUTSIDE; break;
				case 121:
				    CheckerboardBug |= FROMOUTSIDE; break;
				case 122:
				    ClockwiseSpinBug |= FROMOUTSIDE; break;
				case 123:
				    CounterclockwiseSpin |= FROMOUTSIDE; break;
				case 124:
				    LagBugEffect |= FROMOUTSIDE; break;
				case 125:
				    BlesscurseEffect |= FROMOUTSIDE; break;
				case 126:
				    DeLightBug |= FROMOUTSIDE; break;
				case 127:
				    DischargeBug |= FROMOUTSIDE; break;
				case 128:
				    TrashingBugEffect |= FROMOUTSIDE; break;
				case 129:
				    FilteringBug |= FROMOUTSIDE; break;
				case 130:
				    DeformattingBug |= FROMOUTSIDE; break;
				case 131:
				    FlickerStripBug |= FROMOUTSIDE; break;
				case 132:
				    UndressingEffect |= FROMOUTSIDE; break;
				case 133:
				    Hyperbluewalls |= FROMOUTSIDE; break;
				case 134:
				    NoliteBug |= FROMOUTSIDE; break;
				case 135:
				    ParanoiaBugEffect |= FROMOUTSIDE; break;
				case 136:
				    FleecescriptBug |= FROMOUTSIDE; break;
				case 137:
				    InterruptEffect |= FROMOUTSIDE; break;
				case 138:
				    DustbinBug |= FROMOUTSIDE; break;
				case 139:
				    ManaBatteryBug |= FROMOUTSIDE; break;
				case 140:
				    Monsterfingers |= FROMOUTSIDE; break;
				case 141:
				    MiscastBug |= FROMOUTSIDE; break;
				case 142:
				    MessageSuppression |= FROMOUTSIDE; break;
				case 143:
				    StuckAnnouncement |= FROMOUTSIDE; break;
				case 144:
				    BloodthirstyEffect |= FROMOUTSIDE; break;
				case 145:
				    MaximumDamageBug |= FROMOUTSIDE; break;
				case 146:
				    LatencyBugEffect |= FROMOUTSIDE; break;
				case 147:
				    StarlitBug |= FROMOUTSIDE; break;
				case 148:
				    KnowledgeBug |= FROMOUTSIDE; break;
				case 149:
				    HighscoreBug |= FROMOUTSIDE; break;
				case 150:
				    PinkSpells |= FROMOUTSIDE; break;
				case 151:
				    GreenSpells |= FROMOUTSIDE; break;
				case 152:
				    EvencoreEffect |= FROMOUTSIDE; break;
				case 153:
				    UnderlayerBug |= FROMOUTSIDE; break;
				case 154:
				    DamageMeterBug |= FROMOUTSIDE; break;
				case 155:
				    ArbitraryWeightBug |= FROMOUTSIDE; break;
				case 156:
				    FuckedInfoBug |= FROMOUTSIDE; break;
				case 157:
				    BlackSpells |= FROMOUTSIDE; break;
				case 158:
				    CyanSpells |= FROMOUTSIDE; break;
				case 159:
				    HeapEffectBug |= FROMOUTSIDE; break;
				case 160:
				    BlueSpells |= FROMOUTSIDE; break;
				case 161:
				    TronEffect |= FROMOUTSIDE; break;
				case 162:
				    RedSpells |= FROMOUTSIDE; break;
				case 163:
				    TooHeavyEffect |= FROMOUTSIDE; break;
				case 164:
				    ElongationBug |= FROMOUTSIDE; break;
				case 165:
				    WrapoverEffect |= FROMOUTSIDE; break;
				case 166:
				    DestructionEffect |= FROMOUTSIDE; break;
				case 167:
				    MeleePrefixBug |= FROMOUTSIDE; break;
				case 168:
				    AutomoreBug |= FROMOUTSIDE; break;
				case 169:
				    UnfairAttackBug |= FROMOUTSIDE; break;
			}

		}

		if (!rn2(2000) && u.uprops[ARTIFICER_BUG].extrinsic) {
			bad_artifact();
		}

		if (!rn2(2000) && have_artificialstone()) {
			bad_artifact();
		}

		if (!rn2(200) && HorrorBugEffect) {

			int lcount = rnd(monster_difficulty() ) + 1;

		    switch (rn2(11)) {
		    case 0: make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON),20),
				"horrible sickness", TRUE, SICK_NONVOMITABLE);
			    break;
		    case 1: make_blinded(Blinded + lcount, TRUE);
			    break;
		    case 2: if (!Confusion)
				You("suddenly feel %s.", Hallucination ? "trippy" : "confused");
			    make_confused(HConfusion + lcount, TRUE);
			    break;
		    case 3: make_stunned(HStun + lcount, TRUE);
			    break;
		    case 4: make_numbed(HNumbed + lcount, TRUE);
			    break;
		    case 5: make_frozen(HFrozen + lcount, TRUE);
			    break;
		    case 6: make_burned(HBurned + lcount, TRUE);
			    break;
		    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE);
			    break;
		    case 8: (void) make_hallucinated(HHallucination + lcount, TRUE, 0L);
			    break;
		    case 9: make_feared(HFeared + lcount, TRUE);
			    break;
		    case 10: make_dimmed(HDimmed + lcount, TRUE);
			    break;
		    }

		}

		if (!rn2(200) && u.uprops[HORROR_BUG].extrinsic) {

			int lcount = rnd(monster_difficulty() ) + 1;

		    switch (rn2(11)) {
		    case 0: make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON),20),
				"horrible sickness", TRUE, SICK_NONVOMITABLE);
			    break;
		    case 1: make_blinded(Blinded + lcount, TRUE);
			    break;
		    case 2: if (!Confusion)
				You("suddenly feel %s.", Hallucination ? "trippy" : "confused");
			    make_confused(HConfusion + lcount, TRUE);
			    break;
		    case 3: make_stunned(HStun + lcount, TRUE);
			    break;
		    case 4: make_numbed(HNumbed + lcount, TRUE);
			    break;
		    case 5: make_frozen(HFrozen + lcount, TRUE);
			    break;
		    case 6: make_burned(HBurned + lcount, TRUE);
			    break;
		    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE);
			    break;
		    case 8: (void) make_hallucinated(HHallucination + lcount, TRUE, 0L);
			    break;
		    case 9: make_feared(HFeared + lcount, TRUE);
			    break;
		    case 10: make_dimmed(HDimmed + lcount, TRUE);
			    break;
		    }

		}

		if (!rn2(200) && have_horrorstone()) {

			int lcount = rnd(monster_difficulty() ) + 1;

		    switch (rn2(11)) {
		    case 0: make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON),20),
				"horrible sickness", TRUE, SICK_NONVOMITABLE);
			    break;
		    case 1: make_blinded(Blinded + lcount, TRUE);
			    break;
		    case 2: if (!Confusion)
				You("suddenly feel %s.", Hallucination ? "trippy" : "confused");
			    make_confused(HConfusion + lcount, TRUE);
			    break;
		    case 3: make_stunned(HStun + lcount, TRUE);
			    break;
		    case 4: make_numbed(HNumbed + lcount, TRUE);
			    break;
		    case 5: make_frozen(HFrozen + lcount, TRUE);
			    break;
		    case 6: make_burned(HBurned + lcount, TRUE);
			    break;
		    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE);
			    break;
		    case 8: (void) make_hallucinated(HHallucination + lcount, TRUE, 0L);
			    break;
		    case 9: make_feared(HFeared + lcount, TRUE);
			    break;
		    case 10: make_dimmed(HDimmed + lcount, TRUE);
			    break;
		    }

		}

		if (uimplant) {
			u.uimplantturns++;
			if (u.uimplantturns >= 200) {
				u.uimplantturns = 0;
				use_skill(P_IMPLANTS, 1);
			}
		}

		if ((DeLightBug || u.uprops[DE_LIGHT_BUG].extrinsic || have_delightstone()) && isok(u.ux, u.uy)) {
			levl[u.ux][u.uy].lit = FALSE;
		}

		if (UndressingEffect || u.uprops[UNDRESSING_EFFECT].extrinsic || have_undressingstone()) {

			if (!rn2(10000) && uwep && !(uwep->cursed)) {
				setnotworn(uwep);
				uswapwepgone();
			}
			if (!rn2(10000) && ublindf && !(ublindf->cursed)) {
				remove_worn_item(ublindf, TRUE);
			}
			if (!rn2(10000) && uright && !(uright->cursed)) {
				remove_worn_item(uright, TRUE);
			}
			if (!rn2(10000) && uleft && !(uleft->cursed)) {
				remove_worn_item(uleft, TRUE);
			}
			if (!rn2(10000) && uamul && !(uamul->cursed)) {
				remove_worn_item(uamul, TRUE);
			}
			if (!rn2(10000) && uimplant && !(uimplant->cursed)) {
				remove_worn_item(uimplant, TRUE);
			}
			if (!rn2(10000) && uarmf && !(uarmf->cursed)) {
				remove_worn_item(uarmf, TRUE);
			}
			if (!rn2(10000) && uarmg && !(uarmg->cursed)) {
				remove_worn_item(uarmg, TRUE);
			}
			if (!rn2(10000) && uarmh && !(uarmh->cursed)) {
				remove_worn_item(uarmh, TRUE);
			}
			if (!rn2(10000) && uarms && !(uarms->cursed)) {
				remove_worn_item(uarms, TRUE);
			}
			if (!rn2(10000) && uarmc && !(uarmc->cursed)) {
				remove_worn_item(uarmc, TRUE);
			}
			if (!rn2(10000) && uarmu && !(uarmu->cursed)) {
				remove_worn_item(uarmu, TRUE);
			}
			if (!rn2(10000) && uarm && !(uarm->cursed)) {
				remove_worn_item(uarm, TRUE);
			}

		}

		if ((HighscoreBug || u.uprops[HIGHSCORE_BUG].extrinsic || have_highscorestone()) && !rn2(300) ) {
			if (!rn2(2)) makespacewarstrap();
			else (void) makemon(insidemon(), 0, 0, NO_MM_FLAGS);
		}

		if (SpellColorPink) {
			u.pinkspelldirection = rnd(8);
		}

		if (TronEffect || u.uprops[TRON_EFFECT].extrinsic || have_tronstone()) {
			if (u.trontrapdirection > 0 && (u.trontrapturn + 1) < moves) u.trontrapdirection = -1;
		}

		if (DestructionEffect && !rn2(100)) {
			switch (rnd(4)) {
				case 1:
					(void) burnarmor(&youmonst);
					destroy_item(SCROLL_CLASS, AD_FIRE);
					destroy_item(SPBOOK_CLASS, AD_FIRE);
					destroy_item(POTION_CLASS, AD_FIRE);
					break;
				case 2:
					destroy_item(POTION_CLASS, AD_COLD);
					break;
				case 3:
					destroy_item(RING_CLASS, AD_ELEC);
					destroy_item(WAND_CLASS, AD_ELEC);
					destroy_item(AMULET_CLASS, AD_ELEC);
					break;
				case 4:
					(void) destroy_item(POTION_CLASS, AD_VENO);
					(void) destroy_item(FOOD_CLASS, AD_VENO);
					break;
			}
		}

		if (u.uprops[DESTRUCTION_EFFECT].extrinsic && !rn2(100)) {
			switch (rnd(4)) {
				case 1:
					(void) burnarmor(&youmonst);
					destroy_item(SCROLL_CLASS, AD_FIRE);
					destroy_item(SPBOOK_CLASS, AD_FIRE);
					destroy_item(POTION_CLASS, AD_FIRE);
					break;
				case 2:
					destroy_item(POTION_CLASS, AD_COLD);
					break;
				case 3:
					destroy_item(RING_CLASS, AD_ELEC);
					destroy_item(WAND_CLASS, AD_ELEC);
					destroy_item(AMULET_CLASS, AD_ELEC);
					break;
				case 4:
					(void) destroy_item(POTION_CLASS, AD_VENO);
					(void) destroy_item(FOOD_CLASS, AD_VENO);
					break;
			}
		}

		if (have_destructionstone() && !rn2(100)) {
			switch (rnd(4)) {
				case 1:
					(void) burnarmor(&youmonst);
					destroy_item(SCROLL_CLASS, AD_FIRE);
					destroy_item(SPBOOK_CLASS, AD_FIRE);
					destroy_item(POTION_CLASS, AD_FIRE);
					break;
				case 2:
					destroy_item(POTION_CLASS, AD_COLD);
					break;
				case 3:
					destroy_item(RING_CLASS, AD_ELEC);
					destroy_item(WAND_CLASS, AD_ELEC);
					destroy_item(AMULET_CLASS, AD_ELEC);
					break;
				case 4:
					(void) destroy_item(POTION_CLASS, AD_VENO);
					(void) destroy_item(FOOD_CLASS, AD_VENO);
					break;
			}
		}

		if ((uarmc && OBJ_DESCR(objects[uarmc->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "roadmap cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "plashch dorozhnoy karty") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "yo'l xaritasi plash") )) && !rn2(10000)) {
			if (!HConfusion) HConfusion = 1;
			do_mappingX();
		}

		if (Role_if(PM_KURWA) && !rn2(2000)) { /* re-randomize object appearances */
			initobjectsamnesia();
		}

		if (uarmf && uarmf->oartifact == ART_GRENEUVENIA_S_HUG) {
			randomcuss();
		}

		if ((MommaBugEffect || u.uprops[MOMMA_BUG].extrinsic || have_mommystone()) && !rn2(100)) {
			randomcuss();
		}

		if (uarmg && uarmg->oartifact == ART_WHINY_MARY && uwep && !(uwep->cursed)) {
			curse(uwep);
			Your("weapon welds itself to your %s!", makeplural(body_part(HAND)));
		}

		if (uarmh && uarmh->oartifact == ART_GIRLFUL_FARTING_NOISES && !rn2(1000) ) {
			struct permonst *pm = 0;
			int attempts = 0;

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

newbossX:
			do {
				pm = rndmonst();
				attempts++;
				if (!rn2(2000)) reset_rndmonst(NON_PM);

			} while ( (!pm || (pm && !(pm->msound == MS_FART_LOUD || pm->msound == MS_FART_NORMAL || pm->msound == MS_FART_QUIET ))) && attempts < 50000);

			if (!pm && rn2(50) ) {
				attempts = 0;
				goto newbossX;
			}
			if (pm && !(pm->msound == MS_FART_LOUD || pm->msound == MS_FART_NORMAL || pm->msound == MS_FART_QUIET) && rn2(50) ) {
				attempts = 0;
				goto newbossX;
			}

			if (pm) (void) makemon(pm, u.ux, u.uy, NO_MM_FLAGS);

			u.aggravation = 0;

		}

		if ((multi < 0) && (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "dream helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "shlem mechty") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "dubulg'a orzu") ))) {
			if (!Upolyd && u.uhp < u.uhpmax) u.uhp++;
			if (Upolyd && u.mh < u.mhmax) u.mh++;
			if (u.uen < u.uenmax) u.uen++;
			flags.botl = 1;
		}

		if (is_carvedbed(u.ux, u.uy) && (multi < 0)) {
			if (!Upolyd) {
				u.uhp += rnd(10);
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			}
			if (Upolyd) {
				u.mh += rnd(10);
				if (u.mh > u.mhmax) u.mh = u.mhmax;
			}
			flags.botl = 1;

		}

		if (is_mattress(u.ux, u.uy) && (multi >= 0)) {

			if (!rn2(Sleep_resistance ? 200 : 20)) {

				You("suddenly feel an immense need to lie down on the mattress and sleep for a bit.");
				fall_asleep(-rnd(5), TRUE);

			}

		}

		if (is_shiftingsand(u.ux, u.uy) && !Flying && !Levitation) {
			u.shiftingsandsinking++;
			switch (u.shiftingsandsinking) {
				/* make it VERY clear to the player that they will die unless they get out immediately --Amy */
				case 1:
					You("start sinking in the shifting sand. This will quickly kill you, better try to get out ASAP.");
					break;
				case 2:
					You("are struggling against the shifting sand, but you're almost completely submerged now. Pull yourself out immediately or you'll end up in a sandy grave.");
					break;
				case 3:
				default:
					u.youaredead = 1;
					You("sink below the surface and die.");
					/* Unbreathing doesn't help, similar to lava (intentional) --Amy */
					killer = "sinking in shifting sand";
					killer_format = KILLED_BY;
					done(DIED);
					u.youaredead = 0;
					u.shiftingsandsinking = 0; /* if lifesaved, have a few turns to save your hide */
					break;
			}
		}

		if (uarmf && uarmf->oartifact == ART_SUCH_A_WONDERFUL_ROOMMATE && (multi < 0) && (u.uhs >= HUNGRY)) {
			Your("stomach fills.");
			u.uhunger += 100;
		}

		if (uimplant && uimplant->oartifact == ART_BRRRRRRRRRRRRRMMMMMM) {
			if (!rn2(2) || !(nohands(youmonst.data) && !Race_if(PM_TRANSFORMER))) {
				if (u.uen > 0) u.uen--;
				else if (u.uenmax > 0) u.uenmax--;
				flags.botl = TRUE;
			}
		}

		if ((uarmf && OBJ_DESCR(objects[uarmf->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "pink sneakers") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "rozovyye krossovki") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "pushti shippak") )) && !rn2(1000)) {
			pline("The beguiling stench emanating from your pink sneakers fills the area...");
			badeffect();
			turn_allmonsters();
		}

		if (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "spellsucking cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "plashch zaklinaniy") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "so'rib imlo plash") )) {
			if (rn2(2)) {
				u.uen += rnd(5);
				if (u.uen > u.uenmax) u.uen = u.uenmax;
			} else {
				u.uen -= rnd(5);
				if (u.uen < 0) {
					u.uenmax -= rnd(5);
					if (u.uenmax < 0) u.uenmax = 0;
					u.uen = 0;
				}
			}
		}

		if (In_sewerplant(&u.uz) && !u.sewerplantcomplete && (dunlev(&u.uz) == dunlevs_in_dungeon(&u.uz)) ) {
			u.sewerplantcomplete = 1;
			tele();
			pline("Well done, you've reached the bottom of the Sewer Plant! The entrance to the Gamma Caves is now open.");
		}

		if (In_gammacaves(&u.uz) && !u.gammacavescomplete && (dunlev(&u.uz) == dunlevs_in_dungeon(&u.uz)) ) {
			u.gammacavescomplete = 1;
			tele();
			pline("Well done, you've reached the bottom of the Gamma Caves! The entrance to the Mainframe is now open.");
		}

		/* Imbuing the Bell of Opening must be done before any of the invocation tools work
		 * it will spawn a bunch of quest monsters around the entrance, forcing you to fight your way back out --Amy */

		if (!u.bellimbued && u.uhave.bell && In_bellcaves(&u.uz)) {
			u.bellimbued = 1;
			u.uhpmax += rnd(10);
			u.uenmax += rnd(10);
			if (Upolyd) u.mhmax += rnd(10);

			(void) makemon(&mons[urole.nemesnum], u.ux, u.uy, MM_ANGRY);

			if (urole.enemy1num != NON_PM) {
				(void) makemon(&mons[urole.enemy1num], u.ux, u.uy, MM_ANGRY);
				(void) makemon(&mons[urole.enemy1num], u.ux, u.uy, MM_ANGRY);
				(void) makemon(&mons[urole.enemy1num], u.ux, u.uy, MM_ANGRY);
				(void) makemon(&mons[urole.enemy1num], u.ux, u.uy, MM_ANGRY);
				(void) makemon(&mons[urole.enemy1num], u.ux, u.uy, MM_ANGRY);
			}
			if (urole.enemy2num != NON_PM) {
				(void) makemon(&mons[urole.enemy2num], u.ux, u.uy, MM_ANGRY);
				(void) makemon(&mons[urole.enemy2num], u.ux, u.uy, MM_ANGRY);
			}

			(void) makemon(mkclass(urole.enemy1sym,0), u.ux, u.uy, MM_ANGRY);
			(void) makemon(mkclass(urole.enemy1sym,0), u.ux, u.uy, MM_ANGRY);

			(void) makemon(mkclass(urole.enemy2sym,0), u.ux, u.uy, MM_ANGRY);

		      (void) safe_teleds(FALSE);

#ifdef RECORD_ACHIEVE

			if (!achieve.imbued_bell) {

				achieve.imbued_bell = TRUE;
				if (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "team splat cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "vosklitsatel'nyy znak plashch komanda") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "jamoasi xavfsizlik plash") )) pline("TROPHY GET!");
				if (RngeTeamSplat) pline("TROPHY GET!");

				if (uarmc && uarmc->oartifact == ART_JUNETHACK______WINNER) {
					u.uhpmax += 10;
					u.uenmax += 10;
					if (Upolyd) u.mhmax += 10;
					pline("Well done! Your maximum health and mana were increased to make sure you'll get even more trophies! Go for it!");
				}
			}

#ifdef LIVELOGFILE
			livelog_achieve_update();
			livelog_report_trophy("imbued the Bell of Opening");
#endif
#endif

			qt_pager(QT_BELLIMBUED);

		}

		/* Imbuing the amulet is done in three phases. ziratha had the idea that if you didn't defeat the
		 * corresponding alignment key boss, a more powerful version of it spawns to take revenge. --Amy */

		if (!u.amuletimbued1 && u.uhave.amulet && In_forging(&u.uz)) {
			u.amuletimbued1 = 1;
			qt_pager(QT_AMULETIMBUED1);
		      (void) safe_teleds(FALSE);

			if (!achieve.killed_beholder) {
				pline("Suddenly, you get the feeling that a sinister presence is watching you intently...");
				(void) makemon(&mons[PM_SUPERBEHOLDER], 0, 0, MM_ANGRY);
			}

		}

		if (!u.amuletimbued2 && u.uhave.amulet && In_ordered(&u.uz)) {
			u.amuletimbued2 = 1;
			qt_pager(QT_AMULETIMBUED2);
		      (void) safe_teleds(FALSE);

			if (!achieve.killed_nightmare) {
				pline("Suddenly, you get the feeling that a nightmarish creature is on the loose...");
				(void) makemon(&mons[PM_UBERNIGHTMARE], 0, 0, MM_ANGRY);
			}

		}

		if (!u.amuletimbued3 && u.uhave.amulet && In_deadground(&u.uz)) {
			u.amuletimbued3 = 1;
			qt_pager(QT_AMULETIMBUED3);
		      (void) safe_teleds(FALSE);

			if (!achieve.killed_vecna) {
				pline("Suddenly, you get the feeling that an eerie lord of the dead is living here...");
				(void) makemon(&mons[PM_GIGA_VECNA], 0, 0, MM_ANGRY);
			}

		}

		if (u.amuletimbued1 && u.amuletimbued2 && u.amuletimbued3 && !u.amuletcompletelyimbued && u.uhave.amulet && In_yendorian(&u.uz)) {
			u.amuletcompletelyimbued = 1;
			u.uhpmax += rnd(25);
			u.uenmax += rnd(25);
			if (Upolyd) u.mhmax += rnd(25);

#ifdef RECORD_ACHIEVE

			if (!achieve.imbued_amulet) {

				achieve.imbued_amulet = TRUE;
				if (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "team splat cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "vosklitsatel'nyy znak plashch komanda") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "jamoasi xavfsizlik plash") )) pline("TROPHY GET!");
				if (RngeTeamSplat) pline("TROPHY GET!");

				if (uarmc && uarmc->oartifact == ART_JUNETHACK______WINNER) {
					u.uhpmax += 10;
					u.uenmax += 10;
					if (Upolyd) u.mhmax += 10;
					pline("Well done! Your maximum health and mana were increased to make sure you'll get even more trophies! Go for it!");
				}
			}

#ifdef LIVELOGFILE
			livelog_achieve_update();
			livelog_report_trophy("finished imbuing the Amulet of Yendor");
#endif
#endif
			qt_pager(QT_AMULETIMBUEDCOMPLETE);

			if (u.umortality < 2) {
				u.extralives++;
				pline("Thanks to your flawless performance so far, you gain an extra life (1-UP)!");
			}

			register int newlev = 100;
			d_level newlevel;
			get_level(&newlevel, newlev);
			goto_level(&newlevel, TRUE, FALSE, FALSE);

		}

		if (Blinded < 0) {
			pline("Fixing a bug that gave you a negative blindness counter...");
			Blinded = 1;
			u.ucreamed = 0;
			make_blinded(0L,TRUE);
		}

		if (HStun < 0) {
			pline("Fixing a bug that gave you a negative stun counter...");
			HStun = 1;
			make_stunned(0L,TRUE);
		}

		if (HConfusion < 0) {
			pline("Fixing a bug that gave you a negative confusion counter...");
			HConfusion = 1;
			make_confused(0L,TRUE);
		}

		if (HNumbed < 0) {
			pline("Fixing a bug that gave you a negative numbness counter...");
			HNumbed = 1;
			make_numbed(0L,TRUE);
		}

		if (HFeared < 0) {
			pline("Fixing a bug that gave you a negative fear counter...");
			HFeared = 1;
			make_feared(0L,TRUE);
		}

		if (HFrozen < 0) {
			pline("Fixing a bug that gave you a negative freeze counter...");
			HFrozen = 1;
			make_frozen(0L,TRUE);
		}

		if (HBurned < 0) {
			pline("Fixing a bug that gave you a negative burn counter...");
			HBurned = 1;
			make_burned(0L,TRUE);
		}

		if (HDimmed < 0) {
			pline("Fixing a bug that gave you a negative dimness counter...");
			HDimmed = 1;
			make_dimmed(0L,TRUE);
		}

		if (HHallucination < 0) {
			pline("Fixing a bug that gave you a negative hallucination counter...");
			HHallucination = 1;
			make_hallucinated(0L,TRUE,0L);
		}

		if (Wounded_legs && ((EWounded_legs & LEFT_SIDE) || (EWounded_legs & RIGHT_SIDE)) && (HWounded_legs < 1) ) {
			pline("Fixing a bug that would prevent your legs from healing...");
			heal_legs();
		}

		if (In_netherrealm(&u.uz) && !u.netherrealmcomplete && (dunlev(&u.uz) == dunlevs_in_dungeon(&u.uz)) ) {
			u.netherrealmcomplete = TRUE;
			create_critters(rnd(25), (struct permonst *)0);
		      (void) safe_teleds(FALSE);
			pline("You've reached the bottom of the Nether Realm. Tiksrvzllat has noticed this, and was spawned on the bottom of the Void. Defeat her and you'll get a very useful reward!");
		}

		if (In_voiddungeon(&u.uz) && u.netherrealmcomplete && !u.tiksrvzllatspawn && (dunlev(&u.uz) == dunlevs_in_dungeon(&u.uz)) ) {
			u.tiksrvzllatspawn = TRUE;
			(void) makemon(&mons[PM_TIKSRVZLLAT], 0, 0, NO_MM_FLAGS);
			pline("Get ready for the fight with Tiksrvzllat! Defeat her and you'll get a very useful reward!");
		}

		if (u.segfaultpanic && !rn2(100)) {
			u.youaredead = 1;
			pline("Oops... Suddenly, the dungeon collapses.");
			pline("Report error to 'flauschie' and it might be possible to rebuild.");
			pline("obj_is_local:Segmentation fault -- core dumped.");
			killer = "the dreaded segfault panic";
			killer_format = KILLED_BY;
			done(DIED);
			/* lifesaved */
			pline("Somebody is trying some trickery here... This game is void, because the trickery code fires only when the player didn't actually try to cheat (10/10 design, big thumbs up to whoever invented that).");
			pline("EOF on file #5.");
			pline("Error restoring old game.");
			done(DIED);
			/* lifesaved again */
			pline("You cannot avoid this death! This is segfaulter mode, and your game crashed! Game over, man! Game over!");
			done(ESCAPED);
			u.youaredead = 0;
		}

		if (FemaleTrapFemmy && !rn2(500) ) {
			struct permonst *pm = 0;
			int attempts = 0;

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

newbossZ:
			do {
				pm = rndmonst();
				attempts++;
				if (!rn2(2000)) reset_rndmonst(NON_PM);

			} while ( (!pm || (pm && !(pm->msound == MS_FART_LOUD || pm->msound == MS_FART_NORMAL || pm->msound == MS_FART_QUIET ))) && attempts < 50000);

			if (!pm && rn2(50) ) {
				attempts = 0;
				goto newbossZ;
			}
			if (pm && !(pm->msound == MS_FART_LOUD || pm->msound == MS_FART_NORMAL || pm->msound == MS_FART_QUIET) && rn2(50) ) {
				attempts = 0;
				goto newbossZ;
			}

			if (pm) (void) makemon(pm, 0, 0, NO_MM_FLAGS);

			u.aggravation = 0;

		}

		/* the automatic relocation on certain dungeons can make the game almost unwinnable if you end up in a place
		 * surrounded by undiggable walls... so those places give relocatitis now :P --Amy */
		if ((In_bellcaves(&u.uz) || In_deadground(&u.uz) || In_ordered(&u.uz) || In_forging(&u.uz) || (In_netherrealm(&u.uz) && !u.netherrealmcomplete && (dunlev(&u.uz) == dunlevs_in_dungeon(&u.uz))) ) && !rn2(1000)) {

			pline("Suddenly, a void jumpgate appears and transports you away!");
			if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		      (void) safe_teleds(FALSE);
		}

		/* and just in case you end up stuck somewhere else... this lets the player decline, because it's only meant
		 * as a last resort; can't have the player be relocated against their will on e.g. a Sokoban that has
		 * corridor "rivers" from which there'd be no getting back to the regular map unless you wait for another
		 * 10000 turns! Only teleport the player if they want to, with a full "yes" prompt because it may appear
		 * completely out of the blue and the player shouldn't accidentally skip past it. --Amy */
		if (!(In_endgame(&u.uz)) && !rn2(10000)) {

			pline("Suddenly, a void jumpgate appears.");
			getlin ("Do you want to enter the jumpgate and be teleported to a random location on this level? [yes/no]",buf);
			(void) lcase (buf);
			if (!(strcmp (buf, "yes"))) {
				pline("Brrrr... it's deathly cold.");
			      (void) safe_teleds(FALSE);
			}
		}

		if (uarmc && uarmc->oartifact == ART_GREEB && !rn2(2000) ) {
			struct permonst *pm = 0;
			int attempts = 0;

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

newbossY:
			do {
				pm = rndmonst();
				attempts++;
				if (!rn2(2000)) reset_rndmonst(NON_PM);

			} while ( (!pm || (pm && !(pm->mcolor == CLR_GREEN || pm->mcolor == CLR_BRIGHT_GREEN ))) && attempts < 50000);

			if (!pm && rn2(50) ) {
				attempts = 0;
				goto newbossY;
			}
			if (pm && !(pm->mcolor == CLR_GREEN || pm->mcolor == CLR_BRIGHT_GREEN) && rn2(50) ) {
				attempts = 0;
				goto newbossY;
			}

			if (pm) (void) makemon(pm, u.ux, u.uy, NO_MM_FLAGS);

			if (rn2(3)) goto newbossY;

			u.aggravation = 0;

		}

		if ((uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "bluy helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "siniy shlem") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "bluy dubulg'a") )) && !rn2(2000) ) {
			struct permonst *pm = 0;
			int attempts = 0;

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

newbossB:
			do {
				pm = rndmonst();
				attempts++;
				if (!rn2(2000)) reset_rndmonst(NON_PM);

			} while ( (!pm || (pm && !(pm->mcolor == CLR_BRIGHT_BLUE ))) && attempts < 50000);

			if (!pm && rn2(50) ) {
				attempts = 0;
				goto newbossB;
			}
			if (pm && !(pm->mcolor == CLR_BRIGHT_BLUE) && rn2(50) ) {
				attempts = 0;
				goto newbossB;
			}

			if (enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) {
				if (pm) (void) makemon(pm, 0, 0, NO_MM_FLAGS);
			}

			if (rn2(3)) goto newbossB;

			u.aggravation = 0;

		}

		if (uwep && uwep->oartifact == ART_POISON_PEN_LETTER) {
		    poisoned("letter", rn2(6), "wielding a poisoned weapon", 30);
		}
		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_POISON_PEN_LETTER) {
		    poisoned("letter", rn2(6), "wielding a poisoned weapon", 30);
		}

		if (uarmc && uarmc->oartifact == ART_MORE_HIGHER && !rn2(2000) ) {
			u.chokhmahdamage += 1;
			pline("Escalation!");
		}

		if (uamul && uamul->oartifact == ART_EIGHTH_DEADLY_SIN && !rn2(10000) ) {

		    int oi, oj, bd = 1;
		    for (oi = -bd; oi <= bd; oi++) for(oj = -bd; oj <= bd; oj++) {
				if (!isok(u.ux + oi, u.uy + oj)) continue;
				if (levl[u.ux + oi][u.uy + oj].typ <= DBWALL) continue;
				if (t_at(u.ux + oi, u.uy + oj)) continue;
			maketrap(u.ux + oi, u.uy + oj, SIN_TRAP, 0);
		    }

		}

		if (uarmg && uarmg->oartifact == ART_GRABBER_MASTER && !rn2(1000) ) {
			gold_detect(uarmg);
		}

		if (uarmh && uarmh->oartifact == ART_IF_THE_RIGHT_MOUSE_BUTTON_ && !rn2(2000) ) {
		    (void) makemon(&mons[PM_GUNNHILD_S_GENERAL_STORE], 0, 0, NO_MM_FLAGS);
		}

		if (uwep && uwep->oartifact == ART_ARABELLA_S_MELEE_POWER && !rn2(1000) ) {
		    (void) makemon(&mons[PM_GUNNHILD_S_GENERAL_STORE], 0, 0, NO_MM_FLAGS);
		}
		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_ARABELLA_S_MELEE_POWER && !rn2(1000) ) {
		    (void) makemon(&mons[PM_GUNNHILD_S_GENERAL_STORE], 0, 0, NO_MM_FLAGS);
		}

		if (uarmh && uarmh->oartifact == ART_SOON_THERE_WILL_BE_AN_ERRO && !rn2(5000) ) {
			NastinessProblem |= FROMOUTSIDE; /* no message */
		}

		if (uwep && uwep->oartifact == ART_YESTERDAY_ASTERISK && !rn2(5000) && !(nohands(youmonst.data) && !Race_if(PM_TRANSFORMER) && uimplant && uimplant->oartifact == ART_TIMEAGE_OF_REALMS) ) {

		Your("morning star takes you back in time...");

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

		stop_occupation();

		}

		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_YESTERDAY_ASTERISK && !rn2(5000) && !(nohands(youmonst.data) && !Race_if(PM_TRANSFORMER) && uimplant && uimplant->oartifact == ART_TIMEAGE_OF_REALMS) ) {

		Your("morning star takes you back in time...");

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

		stop_occupation();

		}

		if (uarmf && uarmf->oartifact == ART_CURSING_ANOMALY && !rn2(1000) ) {
			if (!Blind) {
				You("notice a %s glow surrounding you.", hcolor(NH_BLACK));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
			}
			rndcurse();

		}

		if (uarmf && uarmf->oartifact == ART_AMY_LOVES_AUTOCURSING_ITEM && !rn2(1000) ) {
			if (!Blind) {
				You("notice a %s glow surrounding you.", hcolor(NH_BLACK));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
			}
			rndcurse();

		}

		if ( Role_if(PM_TRANSSYLVANIAN) && !rn2(1000) ) {
			if (!Blind) {
				You("notice a %s glow surrounding you.", hcolor(NH_BLACK));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
			}
			rndcurse();

		}

		if ( have_cursingstone() && !rn2(1000) ) {
			if (!Blind) {
				You("notice a %s glow surrounding you.", hcolor(NH_BLACK));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
			}
			rndcurse();

		}

		if ( uleft && uleft->oartifact == ART_ARABELLA_S_RADAR && !rn2(1000) ) {
			if (!Blind) {
				You("notice a %s glow surrounding you.", hcolor(NH_BLACK));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
			}
			rndcurse();

		}

		if ( uright && uright->oartifact == ART_ARABELLA_S_RADAR && !rn2(1000) ) {
			if (!Blind) {
				You("notice a %s glow surrounding you.", hcolor(NH_BLACK));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
			}
			rndcurse();

		}

		if ( have_primecurse() && !rn2(1000) ) {
			if (!Blind) {
				You("notice a %s glow surrounding you.", hcolor(NH_BLACK));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
			}
			rndcurse();

		}

		if (uarm && uarm->oartifact == ART_WATER_SHYNESS && !rn2(100) && (levl[u.ux][u.uy].typ == ROOM || levl[u.ux][u.uy].typ == CORR) ) {
			levl[u.ux][u.uy].typ = POOL;
		}

		if (is_sand(u.ux, u.uy) && !rn2(20)) {
			You("are caught in a sandstorm, and the sand gets in your %s!", body_part(EYE));
			make_blinded(Blinded + rnd(5),FALSE);
		}

		if (is_nethermist(u.ux, u.uy) && !rn2(5)) {

			if ((!Drain_resistance || !rn2(5)) && u.uexp > 100) {
				u.uexp -= (u.uexp / 100);
				You_feel("your life slipping away!");
				if (u.uexp < newuexp(u.ulevel - 1)) {
				      losexp("nether mist", TRUE, FALSE);
				}
			}

		}

		if (is_snow(u.ux, u.uy) && !(nohands(youmonst.data) && !Race_if(PM_TRANSFORMER) && uimplant && uimplant->oartifact == ART_WHITE_WHALE_HATH_COME) && !rn2(20) && (Flying || Levitation)) {
			You("are caught in a snowstorm!");
			make_stunned(Stunned + rnd(5),FALSE);
			    stop_occupation();
		}

		if (is_styxriver(u.ux, u.uy)) {

			if ((!Flying && !Levitation) || !rn2(5)) {
				Norep("Continued exposure to the Styx River will cause contamination.");
				contaminate(rnd(10 + level_difficulty()), TRUE);
			}

		}

		if (is_burningwagon(u.ux, u.uy)) {
			pline("The wagon burns you!");
			make_burned(HBurned + rnd(10 + level_difficulty()), FALSE);
			if (!Fire_resistance || !rn2(20)) losehp(rnd(5 + (level_difficulty() / 3)), "a burning wagon", KILLED_BY);

		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 10 : 50))
		      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);
		    burn_away_slime();
		    if (isevilvariant || !rn2(10)) burnarmor(&youmonst);

		}

		if (is_moorland(u.ux, u.uy) && !Flying && !Levitation) {
			Norep("Swimming in moorland causes continuous damage.");
			losehp(rnd(5 + (level_difficulty() / 5)), "swimming in moorland", KILLED_BY);
		}

		if (is_raincloud(u.ux, u.uy)) {

			if (level.flags.lethe) pline("Sparkling rain washes over you.");
			else pline("Rain washes over you.");

			if (multi >= 0 && !rn2(8)) {

				pline("Kaboom!!!  Boom!!  Boom!!");
				stop_occupation();
				nomul(-3, "hiding from thunderstorm", TRUE);
				nomovemsg = 0;

			}

			water_damage(invent, FALSE, FALSE);
			if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);

		}

		if (is_urinelake(u.ux, u.uy) && !Flying && !Levitation) {

			if (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record > 0) {
				pline("Ulch - your divine body is tainted by that filthy yellow liquid!");
				u.ualign.record = -20;
			}
			/* This isn't much of a downside, since usually angels with positive alignment fly anyway :D --Amy */

			if (u.ualign.record > 0) {
				Norep("The yellow liquid actually feels comfortable on your skin.");
			}

			else if (u.ualign.record == 0) {
				Norep("The yellow liquid tickles your skin.");
				losehp(1, "swimming in urine while nominally aligned", KILLED_BY);
			}

			else if (u.ualign.record < 0) {
				pline("The yellow liquid %scorrodes your unprotected skin!", !Acid_resistance ? "severely " : "");
				losehp(rnd(5 + (level_difficulty() / 2)), "foolishly swimming in urine", KILLED_BY);
				if (!Acid_resistance) losehp(rnd(10 + level_difficulty()), "foolishly swimming in urine", KILLED_BY);
				if (!rn2(10)) badeffect();

				if (!rn2(10)) {
					register struct obj *objU, *objU2;
					for (objU = invent; objU; objU = objU2) {
					      objU2 = objU->nobj;
						if (!rn2(5)) rust_dmg(objU, xname(objU), 3, TRUE, &youmonst);
					}
				}

			}

		}

		if (is_stalactite(u.ux, u.uy) && (Flying || Levitation)) {
			pline("The stalactite pierces you!");
			losehp(rnd(10 + level_difficulty()), "being impaled on a stalactite", KILLED_BY);
		}

		if (u.umoved && is_pavedfloor(u.ux, u.uy) && !Flying && !Levitation) {
			Norep("Walking on paved floor makes lots of noise.");
			wake_nearby();

			if (PlayerInHighHeels && !rn2(40) && ((rnd(4) > P_SKILL(P_HIGH_HEELS)) || (PlayerCannotUseSkills) )) {
				nomul(-(1 + rnd(5)), "crashing into a paved floor", TRUE);
				set_wounded_legs(rn2(2) ? RIGHT_SIDE : LEFT_SIDE, HWounded_legs + rnz(200));
				pline("Since you're not proficient at walking in high heels, you sprain your %s very painfully and crash into the floor.", body_part(LEG));

			}

		}

		if (uwep && uwep->oartifact == ART_OVERHEATER && !rn2(1000) && !(t_at(u.ux, u.uy) ) ) {
			(void) maketrap(u.ux, u.uy, FIRE_TRAP, 0);
		}
		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_OVERHEATER && !rn2(1000) && !(t_at(u.ux, u.uy) ) ) {
			(void) maketrap(u.ux, u.uy, FIRE_TRAP, 0);
		}

		if (uarmf && uarmf->oartifact == ART_ANASTASIA_S_PLAYFULNESS && !rn2(1000) ) {
			int tryct = 0;
			int x, y;

			for (tryct = 0; tryct < 2000; tryct++) {
				x = rn1(COLNO-3,2);
				y = rn2(ROWNO);

				if (x && y && isok(x, y) && (levl[x][y].typ > DBWALL) && !(t_at(x, y)) ) {
					(void) maketrap(x, y, SHIT_TRAP, 0);
					break;
					}
			}
		}

		if (Role_if(PM_GANG_SCHOLAR) && !rn2(500) ) {
			int tryct = 0;
			int x, y;

			for (tryct = 0; tryct < 2000; tryct++) {
				x = rn1(COLNO-3,2);
				y = rn2(ROWNO);

				if (x && y && isok(x, y) && (levl[x][y].typ > DBWALL) && !(t_at(x, y)) ) {
					(void) maketrap(x, y, SHIT_TRAP, 0);
					break;
					}
			}
		}

		if (uimplant && uimplant->oartifact == ART_DUBAI_TOWER_BREAK && HFrozen && !rn2(5) ) {
			verbalize("Mighty fire magic thaws your ice!");
			make_frozen(0L,FALSE);
		}

		if (uimplant && uimplant->oartifact == ART_ARRGH_OUCH && !rn2(10) ) {
			losehp(10, "pain", KILLED_BY);
			if (u.uhp < 20 || (u.uhp < 50 && !rn2(3)) || !rn2(10)) You("scream in pain.");
			if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER)) use_skill(P_HEALING_SPELL, 1);
		}

		/* Gang Scholar gods are really nice: unless you're in Gehennom, they will occasionally fix status effects
		 * negative alignment or luck lowers the chance, angry gods are very reluctant to help --Amy */
		if (Role_if(PM_GANG_SCHOLAR) && !(u.ualign.record < 0 && rn2(3)) && !(u.ugangr && rn2(10 * u.ugangr)) && !(Luck < 0 && rn2(5)) && !((Inhell && !Race_if(PM_HERETIC) ) || flags.gehenna) ) {

			if (HStun && !rn2(100)) {
				verbalize("Thou shalt be relieved of that stun!");
				make_stunned(0L,TRUE);
			}

			if (Hallucination && !rn2(100)) {
				verbalize("Thou shalt not be addled by drugs!");
				(void) make_hallucinated(0L,FALSE,0L);
			}

			if (HNumbed && !rn2(50)) {
				verbalize("I cannot let thee be numbed forever.");
				make_numbed(0L,FALSE);
			}

			if (HFrozen && !rn2(200)) {
				verbalize("Thine ice shalt thaw!");
				make_frozen(0L,FALSE);
			}

			if (HBurned && !rn2(100)) {
				verbalize("Thine burning shalt stop!");
				make_burned(0L,FALSE);
			}

			if (HDimmed && !rn2(100)) {
				verbalize("Thy dimness ceases now!");
				make_dimmed(0L,FALSE);
			}

			if (HFeared && !rn2(100)) {
				verbalize("Shake off thine fear!");
				make_feared(0L,FALSE);
			}

			if (Stoned && !rn2(20)) {
				verbalize("Thou wouldst be of no use as a statue!");
				You_feel("more limber.");
				Stoned = 0;
				flags.botl = 1;
				delayed_killer = 0;
			}

			if (Slimed && !rn2(400)) {
				verbalize("That foul green goo is to be devoured by holy fire!");
				Hallucination ? pline("The rancid goo is gone! Yay!") : pline_The("slime disappears.");
				Slimed = 0;
				flags.botl = 1;
				delayed_killer = 0;
			}

			if (Sick && !rn2(100)) {
				verbalize("Begone, thou foul sickness, and don't dare affecting my precious scholar again!");
				make_sick(0L, (char *) 0, FALSE, SICK_ALL);
			}

			/* confusion is not cured, because the player may try to confuse themselves on purpose */

		}

		if (uarmf && uarmf->oartifact == ART_BRIDGE_SHITTE && !rn2(500) ) {
			int tryct = 0;
			int x, y;

			for (tryct = 0; tryct < 2000; tryct++) {
				x = rn1(COLNO-3,2);
				y = rn2(ROWNO);

				if (x && y && isok(x, y) && (levl[x][y].typ > DBWALL) && !(t_at(x, y)) ) {
					(void) maketrap(x, y, SHIT_TRAP, 0);
					break;
					}
			}
		}

		if (uarmf && uarmf->oartifact == ART_BLUEDE && !rn2(1000) ) {
			int tryct = 0;
			int x, y;

			for (tryct = 0; tryct < 2000; tryct++) {
				x = rn1(COLNO-3,2);
				y = rn2(ROWNO);

				if (x && y && isok(x, y) && (levl[x][y].typ > DBWALL) && !(t_at(x, y)) ) {
					(void) maketrap(x, y, NATALJE_TRAP, 0);
					break;
					}
			}
		}

		if (FemaleTrapFemmy && !rn2(200)) {

			int tryct = 0;
			int x, y;

			for (tryct = 0; tryct < 2000; tryct++) {
				x = rn1(COLNO-3,2);
				y = rn2(ROWNO);

				if (x && y && isok(x, y) && (levl[x][y].typ > DBWALL) && !(t_at(x, y)) ) {
					(void) maketrap(x, y, FART_TRAP, 0);
					break;
					}
			}

		}

		if (uarmf && uarmf->oartifact == ART_LORSKEL_S_INTEGRITY && !rn2(500)) {

			int tryct = 0;
			int x, y;

			for (tryct = 0; tryct < 2000; tryct++) {
				x = rn1(COLNO-3,2);
				y = rn2(ROWNO);

				if (x && y && isok(x, y) && (levl[x][y].typ > DBWALL) && !(t_at(x, y)) ) {
					(void) maketrap(x, y, FART_TRAP, 0);
					break;
					}
			}

		}

		if ( (uarmf && OBJ_DESCR(objects[uarmf->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "femmy boots") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "zhenskiye sapogi") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "nazokat etigi") )) && !rn2(1000) ) {

			int tryct = 0;
			int x, y;

			for (tryct = 0; tryct < 2000; tryct++) {
				x = rn1(COLNO-3,2);
				y = rn2(ROWNO);

				if (x && y && isok(x, y) && (levl[x][y].typ > DBWALL) && !(t_at(x, y)) ) {
					(void) maketrap(x, y, FART_TRAP, 0);
					break;
					}
			}

		}

		if (FemaleTrapFemmy && !rn2(200)) {

			int tryct = 0;
			int x, y;

			for (tryct = 0; tryct < 2000; tryct++) {
				x = rn1(COLNO-3,2);
				y = rn2(ROWNO);

				if (x && y && isok(x, y) && (levl[x][y].typ > DBWALL) && !(t_at(x, y)) ) {
					(void) maketrap(x, y, HEEL_TRAP, 0);
					break;
					}
			}

		}

		if (uarmg && uarmg->oartifact == ART_MADELINE_S_STUPID_GIRL && !rn2(500) ) {
			int tryct = 0;
			int x, y;

			for (tryct = 0; tryct < 2000; tryct++) {
				x = rn1(COLNO-3,2);
				y = rn2(ROWNO);

				if (x && y && isok(x, y) && (levl[x][y].typ > DBWALL) && !(t_at(x, y)) ) {
					(void) maketrap(x, y, SHIT_TRAP, 0);
					break;
					}
			}
		}

		if (FemaleTrapAnastasia && !rn2(250) ) {
			int tryct = 0;
			int x, y;

			for (tryct = 0; tryct < 2000; tryct++) {
				x = rn1(COLNO-3,2);
				y = rn2(ROWNO);

				if (x && y && isok(x, y) && (levl[x][y].typ > DBWALL) && !(t_at(x, y)) ) {
					(void) maketrap(x, y, SHIT_TRAP, 0);
					break;
					}
			}
		}

		if (uwep && uwep->oartifact == ART_LUISA_S_CHARMING_BEAUTY && !rn2(200) ) {
			int tryct = 0;
			int x, y;

			for (tryct = 0; tryct < 2000; tryct++) {
				x = rn1(COLNO-3,2);
				y = rn2(ROWNO);

				if (x && y && isok(x, y) && (levl[x][y].typ > DBWALL) && !(t_at(x, y)) ) {
					(void) maketrap(x, y, SHIT_TRAP, 0);
					break;
					}
			}
		}

		if (uwep && uwep->oartifact == ART_LUISA_S_CHARMING_BEAUTY && !rn2(500) ) {
			pline("Wielding such a beautiful, erotic lady boot for so long makes your %s turn.", body_part(STOMACH));
		      make_vomiting(Vomiting+20, TRUE);
			if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10);

		}

		if (Conflict && !rn2(500)) { /* bullshit downside --Amy :P */

			makerandomtrap();

		}

		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_LUISA_S_CHARMING_BEAUTY && !rn2(500) ) {
			pline("Wielding such a beautiful, erotic lady boot for so long makes your %s turn.", body_part(STOMACH));
		      make_vomiting(Vomiting+20, TRUE);
			if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10);

		}

		if (uwep && uwep->oartifact == ART_AMY_S_FIRST_GIRLFRIEND && !rn2(500) ) {
			pline("The incredibly soft girl shoe suddenly bonks you, and your optical nerve is damaged...");
			make_blinded(Blinded + rnd(30),FALSE);
		}

		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_AMY_S_FIRST_GIRLFRIEND && !rn2(500) ) {
			pline("The incredibly soft girl shoe suddenly bonks you, and your optical nerve is damaged...");
			make_blinded(Blinded + rnd(30),FALSE);
		}

		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_LUISA_S_CHARMING_BEAUTY && !rn2(200) ) {
			int tryct = 0;
			int x, y;

			for (tryct = 0; tryct < 2000; tryct++) {
				x = rn1(COLNO-3,2);
				y = rn2(ROWNO);

				if (x && y && isok(x, y) && (levl[x][y].typ > DBWALL) && !(t_at(x, y)) ) {
					(void) maketrap(x, y, SHIT_TRAP, 0);
					break;
					}
			}
		}

		if ( (have_morgothiancurse() || (uamul && uamul->oartifact == ART_NOW_YOU_HAVE_LOST) || (uarmc && uarmc->oartifact == ART_BLACK_VEIL_OF_BLACKNESS) || (uarmc && uarmc->oartifact == ART_ARABELLA_S_WAND_BOOSTER) || (uarmf && uarmf->oartifact == ART_KYLIE_LUM_S_SNAKESKIN_BOOT && !Role_if(PM_TOPMODEL) ) || (uarmh && uarmh->oartifact == ART_MASSIVE_IRON_CROWN_OF_MORG) || (uwep && uwep->oartifact == ART_GUN_CONTROL_LAWS) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_GUN_CONTROL_LAWS) ) && !rn2(500) ) { /* was 1 in 50 in ToME */
			switch (rnd(30)) {

				case 1:
				case 2:
				case 3:
				case 4:
				case 5: /* aggravate monster, long duration */
					You_feel("that monsters are aware of your presence.");
					aggravate();
					aggravate();
					aggravate();
					wake_nearby();
					wake_nearby();
					wake_nearby();
					break;
				case 6:
				case 7:
				case 8: /* replication */
					pline("Oh! You feel that the curse is replicating itself!");

					struct obj *otmpE;
				      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
						if (otmpE && !stack_too_big(otmpE) && !rn2(10)) {
							curse(otmpE);
							otmpE->cursed = otmpE->hvycurse = otmpE->prmcurse = otmpE->morgcurse = 1;
						}
					}

					break;
				case 9:
				case 10:
				case 11:
				case 12: /* curse items */
					You_feel("as if you need some help.");
					if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
					rndcurse();
					break;
				case 13:
				case 14:
				case 15: /* level drain, ignoring resists */
				      losexp("ancient morgothian drainage", FALSE, FALSE);
					break;
				case 16:
				case 17:
				case 18: /* paralysis: up to 3 turns with free action, up to 13 without */
					You_feel("like a statue!");
					if (Free_action) nomul(-rnd(3), "paralyzed by the ancient morgotian curse", TRUE);
					else nomul(-rnd(13), "paralyzed by the ancient morgotian curse", TRUE);
					break;
				case 19:
				case 20: /* confusion */
					pline("Woah! You see 10 little Morgoths dancing before you!");
					make_confused(HConfusion + rnz(1000), FALSE);
					break;
				case 21:
				case 22:
				case 23: /* drain a random stat by 5 points */
					(void) adjattrib(rn2(A_MAX), -5, FALSE);
					break;
				case 24: /* amnesia, magnitude 3 */
					forget(3);
					break;
				case 25:
				case 26:
				case 27: /* evaporate weapon */
					if (uwep) {
						Your("weapon seems useless now and disintegrates!");
						useupall(uwep);
					}
					break;
				case 28: /* summon "thunderlords" */

					if (Aggravate_monster) {
						u.aggravation = 1;
						reset_rndmonst(NON_PM);
					}
			 	    (void) makemon(mkclass(S_DEMON,0), u.ux, u.uy, MM_ANGRY);
			 	    (void) makemon(mkclass(S_DEMON,0), u.ux, u.uy, MM_ANGRY);
			 	    (void) makemon(mkclass(S_DEMON,0), u.ux, u.uy, MM_ANGRY);
			 	    (void) makemon(mkclass(S_DEMON,0), u.ux, u.uy, MM_ANGRY);
			 	    (void) makemon(mkclass(S_DEMON,0), u.ux, u.uy, MM_ANGRY);
			 	    (void) makemon(mkclass(S_DEMON,0), u.ux, u.uy, MM_ANGRY);
			 	    (void) makemon(mkclass(S_DEMON,0), u.ux, u.uy, MM_ANGRY);
			 	    (void) makemon(mkclass(S_DEMON,0), u.ux, u.uy, MM_ANGRY);

					u.aggravation = 0;

					You("suddenly have company.");
					break;
				case 29:
				case 30: /* drain all stats by one */
					(void) adjattrib(A_STR, -1, FALSE);
					(void) adjattrib(A_INT, -1, FALSE);
					(void) adjattrib(A_DEX, -1, FALSE);
					(void) adjattrib(A_CHA, -1, FALSE);
					(void) adjattrib(A_CON, -1, FALSE);
					(void) adjattrib(A_WIS, -1, FALSE);
					break;
			}
		}

		if ( (have_topiylinencurse() || (uamul && uamul->oartifact == ART_SURTERSTAFF && uwep && (weapon_type(uwep) == P_QUARTERSTAFF)) || (uarmh && uarmh->oartifact == ART_IRON_HELM_OF_GORLIM) ) && !rn2(1000) ) { /* was 1 in 100 in ToME */
			switch (rnd(27)) {
				case 1:
				case 2:
				case 3:
				case 4:
				case 5: /* aggravate monster */
					You_feel("that monsters are aware of your presence.");
					if (PlayerHearsSoundEffects) pline(issoviet ? "Dazhe sovetskaya Pyat' Lo obostryayetsya v vashem nizkom igrovom masterstve." : "Woaaaaaah!");
					aggravate();
					break;
				case 6:
				case 7:
				case 8: /* summon nasties */
					(void)nasty((struct monst *)0);
					break;
				case 9:
				case 10:
				case 11:
				case 12: /* summon vortices */
					if (Aggravate_monster) {
						u.aggravation = 1;
						reset_rndmonst(NON_PM);
					}
			 	    (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY);
			 	    (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY);
			 	    (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY);
			 	    (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY);
			 	    if (!rn2(2)) (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY);
			 	    if (!rn2(5)) (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY);
			 	    if (!rn2(12)) (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY);
			 	    if (!rn2(27)) (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY);
					u.aggravation = 0;
					break;
				case 13:
				case 14:
				case 15: /* level drain */
					if(!Drain_resistance || !rn2(4) )
					    losexp("topi ylinen drainage", FALSE, TRUE);
					break;
				case 16:
				case 17:
				case 18:
				case 19:
				case 20: /* paralysis: up to 3 turns with free action, up to 13 without */
					You_feel("like a statue!");
					if (Free_action) nomul(-rnd(3), "paralyzed by topi ylinen's curse", TRUE);
					else nomul(-rnd(13), "paralyzed by topi ylinen's curse", TRUE);
					break;
				case 21:
				case 22:
				case 23: /* drain a random stat by 3 points */
					(void) adjattrib(rn2(A_MAX), -3, FALSE);
					break;
				case 24: /* amnesia, magnitude 1-3 */
					forget(rnd(3));
					break;
				case 25: /* summon "cyberdemon" */
					{
					int attempts = 0;
					register struct permonst *ptrZ;

					if (Aggravate_monster) {
						u.aggravation = 1;
						reset_rndmonst(NON_PM);
					}
newboss:
					do {

						ptrZ = rndmonst();
						attempts++;
						if (!rn2(2000)) reset_rndmonst(NON_PM);

					} while ( (!ptrZ || (ptrZ && !(ptrZ->geno & G_UNIQ))) && attempts < 50000);

					if (ptrZ && ptrZ->geno & G_UNIQ) {
						if (wizard) pline("monster generation: %s", ptrZ->mname);
						(void) makemon(ptrZ, u.ux, u.uy, NO_MM_FLAGS);
					}
					else if (rn2(50)) {
						attempts = 0;
						goto newboss;
					}
					pline("Boss monsters appear from nowhere!");

					}
					u.aggravation = 0;
					break;
				case 26:
				case 27: /* drain random stats by one, 50% chance for each of being affected */
					if (rn2(2)) (void) adjattrib(A_STR, -1, FALSE);
					if (rn2(2)) (void) adjattrib(A_INT, -1, FALSE);
					if (rn2(2)) (void) adjattrib(A_DEX, -1, FALSE);
					if (rn2(2)) (void) adjattrib(A_CHA, -1, FALSE);
					if (rn2(2)) (void) adjattrib(A_CON, -1, FALSE);
					if (rn2(2)) (void) adjattrib(A_WIS, -1, FALSE);
					break;
			}
		}

		if ( (have_blackbreathcurse() || (uamul && uamul->oartifact == ART_SURTERSTAFF && !(uwep && (weapon_type(uwep) == P_QUARTERSTAFF))) ) && !rn2( (Race_if(PM_HOBBIT) || Role_if(PM_RINGSEEKER) ) ? 500 : 200) ) {
			/* was 1 in 20 in ToME, or 1 in 50 if you were a hobbit */
			if (!rn2(5)) { /* level drain */
				if(!Drain_resistance || !rn2(4) )
				    losexp("black breath drainage", FALSE, TRUE);
				break;
			} else { /* drain a random stat */
				(void) adjattrib(rn2(A_MAX), -1, FALSE);
			}
		}

		if (WrapoverEffect && !rn2(200)) {

			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(200)) (void) drain_item_reverse(otmpE);
			}

		}

		if (u.uprops[WRAPOVER_EFFECT].extrinsic && !rn2(200)) {

			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(200)) (void) drain_item_reverse(otmpE);
			}

		}

		if (have_wrapoverstone() && !rn2(200)) {

			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(200)) (void) drain_item_reverse(otmpE);
			}

		}

		if (RecurringDisenchant && !rn2(1000)) {

			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(10)) (void) drain_item(otmpE);
			}
			Your("equipment seems less effective.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		}

		if (FemaleTrapSolvejg && !rn2(200)) {

			aggravate();
			Your("loud voice aggravates the denizens of the dungeon.");

		}

		if (uarm && uarm->oartifact == ART_MITHRAL_CANCELLATION && !rn2(1000)) {

			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(10)) (void) drain_item(otmpE);
			}
			Your("equipment seems less effective.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		}

		if (uwep && uwep->oartifact == ART_KUSANAGI_NO_TSURUGI && !rn2(1000)) {

			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(10)) (void) drain_item(otmpE);
			}
			Your("equipment seems less effective.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		}

		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_KUSANAGI_NO_TSURUGI && !rn2(1000)) {

			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(10)) (void) drain_item(otmpE);
			}
			Your("equipment seems less effective.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		}

		if (uwep && uwep->oartifact == ART_ARABELLA_S_ARTIFACT_CREATI && !rn2(1000)) {

			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(10)) (void) drain_item(otmpE);
			}
			Your("equipment seems less effective.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		}

		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_ARABELLA_S_ARTIFACT_CREATI && !rn2(1000)) {

			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(10)) (void) drain_item(otmpE);
			}
			Your("equipment seems less effective.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		}

		if (u.uprops[RECURRING_DISENCHANT].extrinsic && !rn2(1000)) {

			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(10)) (void) drain_item(otmpE);
			}
			Your("equipment seems less effective.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		}

		if (uarmg && uarmg->oartifact == ART_DISENCHANTING_BLACKNESS && !rn2(1000)) {

			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(10)) (void) drain_item_severely(otmpE);
			}
			Your("equipment seems less effective.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		}

		if (uarmh && uarmh->oartifact == ART_DRELITT && !rn2(1000)) {

			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(10)) (void) drain_item(otmpE);
			}
			Your("equipment seems less effective.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		}

		if (have_disenchantmentstone() && !rn2(1000)) {

			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(10)) (void) drain_item(otmpE);
			}
			Your("equipment seems less effective.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		}

		if (ChaosTerrain && !rn2(5) && (!In_sokoban(&u.uz) || !rn2(5) ) ) {

			int chaosx, chaosy;
			chaosx = rn1(COLNO-3,2);
			chaosy = rn2(ROWNO);
			if (chaosx && chaosy && isok(chaosx, chaosy) && (levl[chaosx][chaosy].typ == ROOM || levl[chaosx][chaosy].typ == CORR) ) {
				levl[chaosx][chaosy].typ = randomwalltype();
				if (!(levl[chaosx][chaosy].wall_info & W_EASYGROWTH)) levl[chaosx][chaosy].wall_info |= W_HARDGROWTH;
				block_point(chaosx,chaosy);
				del_engr_at(chaosx,chaosy);
				newsym(chaosx,chaosy);
			}

		}

		if (FemaleTrapYvonne && !rn2(250)) {

			int chaosx, chaosy;
			chaosx = rn1(COLNO-3,2);
			chaosy = rn2(ROWNO);
			if (chaosx && chaosy && isok(chaosx, chaosy) && (levl[chaosx][chaosy].typ == ROOM || levl[chaosx][chaosy].typ == CORR) ) {
				levl[chaosx][chaosy].typ = TOILET;
			}

		}

		if (uimplant && uimplant->oartifact == ART_RUBBER_SHOALS && !rn2(200)) {

			int chaosx, chaosy;
			chaosx = rn1(COLNO-3,2);
			chaosy = rn2(ROWNO);
			if (chaosx && chaosy && isok(chaosx, chaosy) && (levl[chaosx][chaosy].typ == ROOM || levl[chaosx][chaosy].typ == CORR) ) {
				levl[chaosx][chaosy].typ = ASH;
			}

		}

		if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER) && uimplant && uimplant->oartifact == ART_MIGHTY_MOLASS && !rn2(500)) {
			struct monst *mtmp3;
			int k, l;

			for (k = -1; k <= 1; k++) for(l = -1; l <= 1; l++) {
				if (!isok(u.ux + k, u.uy + l)) continue;
				if ( ((mtmp3 = m_at(u.ux + k, u.uy + l)) != 0) && mtmp3->mpeaceful == 0) {
					if (!resist(mtmp3, RING_CLASS, 0, NOTELL)) {
						mon_adjust_speed(mtmp3, -1, (struct obj *)0);
						m_dowear(mtmp3, FALSE); /* might want speed boots */
					}
				}
			}

		}

		/* If you somehow get relocated on the Nightmare's Gauntlet level, the game can become nearly unwinnable.
		 * Therefore we will unlock that door so you can get out. --Amy */
		if (isok(u.ux, u.uy) && isok(u.ux + 1, u.uy) && artifact_door(u.ux + 1, u.uy) == ART_GAUNTLET_KEY) {
			register struct rm *door = &levl[u.ux + 1][u.uy];
			if (door && door->doormask == D_LOCKED) {
				pline("Suddenly the door to the east is unlocked!");
				door->doormask = D_CLOSED | (door->doormask & D_TRAPPED);
			}
		}

		if (u.uprops[CHAOS_TERRAIN].extrinsic && !rn2(5) && (!In_sokoban(&u.uz) || !rn2(5) ) ) {

			int chaosx, chaosy;
			chaosx = rn1(COLNO-3,2);
			chaosy = rn2(ROWNO);
			if (chaosx && chaosy && isok(chaosx, chaosy) && (levl[chaosx][chaosy].typ == ROOM || levl[chaosx][chaosy].typ == CORR) ) {
				levl[chaosx][chaosy].typ = randomwalltype();
				if (!(levl[chaosx][chaosy].wall_info & W_EASYGROWTH)) levl[chaosx][chaosy].wall_info |= W_HARDGROWTH;
				block_point(chaosx,chaosy);
				del_engr_at(chaosx,chaosy);
				newsym(chaosx,chaosy);
			}

		}

		if (uarm && uarm->oartifact == ART_ARMOR_OF_EREBOR && !rn2(5) && (!In_sokoban(&u.uz) || !rn2(5) )) {

			int chaosx, chaosy;
			chaosx = rn1(COLNO-3,2);
			chaosy = rn2(ROWNO);
			if (chaosx && chaosy && isok(chaosx, chaosy) && (levl[chaosx][chaosy].typ == ROOM || levl[chaosx][chaosy].typ == CORR) ) {
				levl[chaosx][chaosy].typ = randomwalltype();
				if (!(levl[chaosx][chaosy].wall_info & W_EASYGROWTH)) levl[chaosx][chaosy].wall_info |= W_HARDGROWTH;
				block_point(chaosx,chaosy);
				del_engr_at(chaosx,chaosy);
				newsym(chaosx,chaosy);
			}

		}

		if (have_chaosterrainstone() && !rn2(5) && (!In_sokoban(&u.uz) || !rn2(5) )) {

			int chaosx, chaosy;
			chaosx = rn1(COLNO-3,2);
			chaosy = rn2(ROWNO);
			if (chaosx && chaosy && isok(chaosx, chaosy) && (levl[chaosx][chaosy].typ == ROOM || levl[chaosx][chaosy].typ == CORR) ) {
				levl[chaosx][chaosy].typ = randomwalltype();
				if (!(levl[chaosx][chaosy].wall_info & W_EASYGROWTH)) levl[chaosx][chaosy].wall_info |= W_HARDGROWTH;
				block_point(chaosx,chaosy);
				del_engr_at(chaosx,chaosy);
				newsym(chaosx,chaosy);
			}

		}

		if (uwep && uwep->oartifact == ART_PROZACELF_S_POOPDECK && !rn2(5) && (!In_sokoban(&u.uz) || !rn2(5) )) {

			int chaosx, chaosy;
			chaosx = rn1(COLNO-3,2);
			chaosy = rn2(ROWNO);
			if (chaosx && chaosy && isok(chaosx, chaosy) && (levl[chaosx][chaosy].typ == ROOM || levl[chaosx][chaosy].typ == CORR) ) {
				levl[chaosx][chaosy].typ = randomwalltype();
				if (!(levl[chaosx][chaosy].wall_info & W_EASYGROWTH)) levl[chaosx][chaosy].wall_info |= W_HARDGROWTH;
				block_point(chaosx,chaosy);
				del_engr_at(chaosx,chaosy);
				newsym(chaosx,chaosy);
			}

		}

		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_PROZACELF_S_POOPDECK && !rn2(5) && (!In_sokoban(&u.uz) || !rn2(5) )) {

			int chaosx, chaosy;
			chaosx = rn1(COLNO-3,2);
			chaosy = rn2(ROWNO);
			if (chaosx && chaosy && isok(chaosx, chaosy) && (levl[chaosx][chaosy].typ == ROOM || levl[chaosx][chaosy].typ == CORR) ) {
				levl[chaosx][chaosy].typ = randomwalltype();
				if (!(levl[chaosx][chaosy].wall_info & W_EASYGROWTH)) levl[chaosx][chaosy].wall_info |= W_HARDGROWTH;
				block_point(chaosx,chaosy);
				del_engr_at(chaosx,chaosy);
				newsym(chaosx,chaosy);
			}

		}

		if (RecurringAmnesia && !rn2(1000)) {
			You_feel("dizzy!");
			forget(1 + rn2(5));
		}

		if (uarmh && uarmh->oartifact == ART_DIADEM_OF_AMNESIA && !rn2(1000)) {
			You_feel("dizzy!");
			forget(1 + rn2(5));
		}

		if (uarmh && uarmh->oartifact == ART_TIARA_OF_AMNESIA && !rn2(1000)) {
			You_feel("dizzy!");
			forget(1 + rn2(5));
		}

		if (have_amnesiastone() && !rn2(1000)) {
			You_feel("dizzy!");
			forget(1 + rn2(5));
		}

		if (u.uprops[RECURRING_AMNESIA].extrinsic && !rn2(1000)) {
			You_feel("dizzy!");
			forget(1 + rn2(5));
		}

		if (uarmh && uarmh->oartifact == ART_DRELITT && !rn2(1000)) {
			You_feel("dizzy!");
			forget(1 + rn2(5));
		}

		if ((uwep && uwep->oartifact == ART_SANDRA_S_EVIL_MINDDRILL) && has_head(youmonst.data) && !Role_if(PM_COURIER) && !rn2(200)) {
			Your("evil female battle boot drills into your mind with its spikes!");
			forget(1 + rn2(5));
		}

		if ((u.twoweap && uswapwep && uswapwep->oartifact == ART_SANDRA_S_EVIL_MINDDRILL) && has_head(youmonst.data) && !Role_if(PM_COURIER) && !rn2(200)) {
			Your("evil female battle boot drills into your mind with its spikes!");
			forget(1 + rn2(5));
		}

		if ((uwep && uwep->oartifact == ART_SANDRA_S_SECRET_WEAPON) && has_head(youmonst.data) && !Role_if(PM_COURIER) && !rn2(200)) {
			Your("mind clears unexpectedly!");
			forget(1 + rn2(5));
		}

		if ((u.twoweap && uswapwep && uswapwep->oartifact == ART_SANDRA_S_SECRET_WEAPON) && has_head(youmonst.data) && !Role_if(PM_COURIER) && !rn2(200)) {
			Your("mind clears unexpectedly!");
			forget(1 + rn2(5));
		}

		if (uarmc && uarmc->oartifact == ART_WATERS_OF_OBLIVION && !rn2(1000)) {
			You("suddenly forget what you were doing. Maybe your thoughts ended up in the realms of Oblivion, who knows?");
			forget(1 + rn2(5));
		}

		if (u.uprops[ITEMCURSING].extrinsic && !rn2(1000) ) {
			if (!Blind) {
				You("notice a %s glow surrounding you.", hcolor(NH_BLACK));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
			}
			rndcurse();

		}

		if (uwep && uwep->oartifact == ART_KUSANAGI_NO_TSURUGI && !rn2(1000) ) {
			if (!Blind) {
				You("notice a %s glow surrounding you.", hcolor(NH_BLACK));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
			}
			rndcurse();

		}

		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_KUSANAGI_NO_TSURUGI && !rn2(1000) ) {
			if (!Blind) {
				You("notice a %s glow surrounding you.", hcolor(NH_BLACK));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
			}
			rndcurse();

		}

		if (uwep && uwep->oartifact == ART_ARABELLA_S_ARTIFACT_CREATI && !rn2(1000) ) {
			if (!Blind) {
				You("notice a %s glow surrounding you.", hcolor(NH_BLACK));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
			}
			rndcurse();

		}

		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_ARABELLA_S_ARTIFACT_CREATI && !rn2(1000) ) {
			if (!Blind) {
				You("notice a %s glow surrounding you.", hcolor(NH_BLACK));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
			}
			rndcurse();

		}

		if (uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "graffiti gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "graffiti perchatki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "qo'lqop purkash") ) && !rn2(2000) ) {
		    incr_itimeout(&Glib, 2); /* just enough to make you drop your weapon */
		}

		if (uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "fatal gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "fatal'nyye perchatki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "halokatli qo'lqop") ) && !rn2(10000) ) {
			pline("Fatal attraction!");

		{
			for(otmpii = otmpi =invent; otmpii ; otmpi = otmpii) {
				otmpii = otmpi->nobj;
				if (objects[(otmpi)->otyp].oc_material == IRON ) {

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

				}
			}
		}
			scatter(u.ux,u.uy,4,VIS_EFFECTS|MAY_HIT|MAY_DESTROY|MAY_FRACTURE,(struct obj*)0);

		}

		if (RngeFatalAttraction && !rn2(10000)) {
			pline("Fatal attraction!");

		{
			for(otmpii = otmpi =invent; otmpii ; otmpi = otmpii) {
				otmpii = otmpi->nobj;
				if (objects[(otmpi)->otyp].oc_material == IRON ) {

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

				}
			}
		}
			scatter(u.ux,u.uy,4,VIS_EFFECTS|MAY_HIT|MAY_DESTROY|MAY_FRACTURE,(struct obj*)0);

		}

		if (uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "vampiric gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "vampiry perchatki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "sindirishi qo'lqop") ) && (u.uexp > 100) && !rn2(1000) ) {
			Your("vampiric gloves drain your experience!");
			u.uexp -= (u.uexp / 100);
			if (u.uexp < newuexp(u.ulevel - 1)) {
			      losexp("vampiric experience drain", TRUE, FALSE);
			}
		}

		if (RngeVampiricDrain && (u.uexp > 100) && !rn2(1000) ) {
			Your("experience is drained!");
			u.uexp -= (u.uexp / 100);
			if (u.uexp < newuexp(u.ulevel - 1)) {
			      losexp("vampiric experience drain", TRUE, FALSE);
			}
		}

		if (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "RNG helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "shlem gsch") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "RNG dubulg'a") ) && !rn2(100000) ) {
			badeffect();
		}

		if ( (have_blackystone() || u.uprops[BLACK_NG_WALLS].extrinsic || (uarmc && uarmc->oartifact == ART_VEIL_OF_LATONA) || (uarmg && uarmg->oartifact == ART_BLACKY_S_BACK_WITHOUT_L) || (uarmc && uarmc->oartifact == ART_VEIL_OF_MINISTRY) ) && !BlackNgWalls && !rn2(100) ) {

			blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
			if (!blackngdur ) blackngdur = 500; /* fail safe */

			BlackNgWalls = (blackngdur - (monster_difficulty() * 3));
			(void) makemon(&mons[PM_BLACKY], 0, 0, NO_MM_FLAGS);
			pline((uarmh && uarmh->oartifact == ART_BLUE_SCREEN_OF_DEATH) ? "Blue Screen of Death!" : "Blackness..."); /* otherwise players would just have no chance --Amy */

			if (uarmh && uarmh->oartifact == ART_BLUE_SCREEN_OF_DEATH) {

				if (Aggravate_monster) {
					u.aggravation = 1;
					reset_rndmonst(NON_PM);
				}

				for (i = 0; i < 8; i++) {

					if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;
					(void) makemon(colormon(CLR_BRIGHT_BLUE), 0, 0, NO_MM_FLAGS);
				}

				u.aggravation = 0;

			}

			break;
		}

		if (Deafness || (uwep && uwep->oartifact == ART_MEMETAL) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_MEMETAL) || (uwep && uwep->oartifact == ART_BANG_BANG) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_BANG_BANG) || u.uprops[DEAFNESS].extrinsic || have_deafnessstone() ) flags.soundok = 0;
		else if (!multi) flags.soundok = 1;

		/* Let's throw a bone to permablind races. --Amy */
		if (!Unidentify && !u.uprops[UNIDENTIFY].extrinsic && !have_unidentifystone() ) {

			if (invent) {
			    for (otmpi = invent; otmpi; otmpi = otmpii) {
			      otmpii = otmpi->nobj;
	
				if (!rn2(10000) && !otmpi->dknown) {
					otmpi->dknown = TRUE;
					You_feel("that you know more about the contents of your inventory...");
				}
				if (!rn2(100) && Race_if(PM_JELLY) && !(otmpi->oclass == SPBOOK_CLASS) && !otmpi->dknown) {
					otmpi->dknown = TRUE;
					You_feel("that you know more about the contents of your inventory...");
				}
				if (!rn2(10000) && isangbander && !otmpi->dknown) {
					otmpi->dknown = TRUE;
					You_feel("that you know more about the contents of your inventory...");
				}
				if (!rn2(10000) && isangbander && !otmpi->known) {
					otmpi->known = TRUE;
					You_feel("that you know more about the contents of your inventory...");
				}
				if (!rn2(10000) && isangbander && !otmpi->bknown) {
					otmpi->bknown = TRUE;
					You_feel("that you know more about the contents of your inventory...");
				}
				if (!rn2(10000) && isangbander && !otmpi->rknown) {
					otmpi->rknown = TRUE;
					You_feel("that you know more about the contents of your inventory...");
				}
				if (!rn2(2000) && Race_if(PM_WEAPON_CUBE) && !otmpi->dknown) {
					otmpi->dknown = TRUE;
					You_feel("that you know more about the contents of your inventory...");
				}
				if (!rn2(1000) && Race_if(PM_CORTEX) && !otmpi->dknown) {
					otmpi->dknown = TRUE;
					You_feel("that you know more about the contents of your inventory...");
				}
				if (!rn2(500) && Race_if(PM_VORTEX) && !otmpi->dknown) {
					otmpi->dknown = TRUE;
					You_feel("that you know more about the contents of your inventory...");
				}

				if (!rn2(100000) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "mysterious cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "tainstvennyy plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "sirli plash") ) && !otmpi->dknown) {
					otmpi->dknown = TRUE;
					You_feel("that you know more about the contents of your inventory...");
				}
				if (!rn2(100000) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "mysterious cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "tainstvennyy plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "sirli plash") ) && !otmpi->known) {
					otmpi->known = TRUE;
					You_feel("that you know more about the contents of your inventory...");
				}
				if (!rn2(100000) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "mysterious cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "tainstvennyy plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "sirli plash") ) && !otmpi->bknown) {
					otmpi->bknown = TRUE;
					You_feel("that you know more about the contents of your inventory...");
				}
				if (!rn2(100000) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "mysterious cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "tainstvennyy plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "sirli plash") ) && !otmpi->rknown) {
					otmpi->rknown = TRUE;
					You_feel("that you know more about the contents of your inventory...");
				}
				if (!rn2(100000) && RngePseudoIdentification && !otmpi->dknown) {
					otmpi->dknown = TRUE;
					You_feel("that you know more about the contents of your inventory...");
				}
				if (!rn2(100000) && RngePseudoIdentification && !otmpi->known) {
					otmpi->known = TRUE;
					You_feel("that you know more about the contents of your inventory...");
				}
				if (!rn2(100000) && RngePseudoIdentification && !otmpi->bknown) {
					otmpi->bknown = TRUE;
					You_feel("that you know more about the contents of your inventory...");
				}
				if (!rn2(100000) && RngePseudoIdentification && !otmpi->rknown) {
					otmpi->rknown = TRUE;
					You_feel("that you know more about the contents of your inventory...");
				}

			    }
			}

		}


		if (Unidentify ) {

			if (invent) {
			    for (otmpi = invent; otmpi; otmpi = otmpii) {
			      otmpii = otmpi->nobj;
	
				if (!rn2(4000)) {
					otmpi->bknown = FALSE;
				}
				if (!rn2(4000)) {
					otmpi->dknown = FALSE;
				}
				if (!rn2(4000)) {
					otmpi->rknown = FALSE;
				}
				if (!rn2(4000)) {
					otmpi->known = FALSE;
				}
			    }
			}

		}

		if (have_unidentifystone() ) {

			if (invent) {
			    for (otmpi = invent; otmpi; otmpi = otmpii) {
			      otmpii = otmpi->nobj;
	
				if (!rn2(4000)) {
					otmpi->bknown = FALSE;
				}
				if (!rn2(4000)) {
					otmpi->dknown = FALSE;
				}
				if (!rn2(4000)) {
					otmpi->rknown = FALSE;
				}
				if (!rn2(4000)) {
					otmpi->known = FALSE;
				}
			    }
			}

		}

		if (!rn2(100) && CaptchaProblem) {
			strcpy(buf, rndmonnam() );
			pline("Captcha! Please type in the following word(s) to continue: %s", buf);
			getlin("Your input:",ebuf);
			if (((int) strlen(ebuf) != (int) strlen(buf) ) || (strncmpi(buf, ebuf, (int) strlen(ebuf)) != 0)) {
				pline("WRONG! You will be punished. I will paralyze you, slow you and reduce your max HP and Pw.");

				if (multi >= 0) nomul(-2, "paralyzed by a captcha", FALSE);

				u.ublesscnt += rnz(300);
				change_luck(-1);

				u.ualign.sins++;
				u.alignlim--;
			      adjalign(-10);

				u.uhpmax -= rnd(5);
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				if (u.uhpmax < 1) {
				    u.youaredead = 1;
				    killer = "failing to solve a captcha";
				    killer_format = KILLED_BY;
				    done(DIED);
				    u.youaredead = 0;
				}
				u.uenmax -= rnd(5);
				if (u.uenmax < 0) {
					u.uenmax = u.uen = 0;
				}
				if (Upolyd) {
					u.mhmax -= rnd(10);
					if (u.mh > u.mhmax) u.mh = u.mhmax;
					if (u.mhmax < 1) rehumanize();
				}

			}
			else pline("Alright. Please move on.");
		}

		if (!rn2(2000)) {
			polyinitors();
			if (Race_if(PM_DESTABILIZER) || Race_if(PM_POLYINITOR)) {
				init_uasmon();
				You("mutate, and your body changes...");
				if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			}
		}

		if (!rn2(100) && QuizTrapEffect) {
			boolean rumoristrue = rn2(2);

			pline("NetHack Quiz! You will now tell me whether the following rumor is true or not!");

			if (rumoristrue) outrumor(1, BY_OTHER, TRUE);
			else outrumor(-1, BY_OTHER, TRUE);

			if (yn("Now tell me if this rumor was true!") != 'y') { /* player said it's false */

				if (rumoristrue) {

					pline("Haha, you guessed wrong! Tough luck, player, it seems you're just not good enough and now there will be a bad effect to punish you, ha-ha!");
					badeffect();

				} else {

					pline("Damn it, you guessed correctly! I can't believe it! This rumor was actually false!");

				}

			} else { /* player said it's true */

				if (rumoristrue) {

					pline("Oh no, how did you know that this rumor was true? You cheated! You're playing with a spoiler sheet, admit it!");

				} else {

					You("really believe everything you read, huh? Well, sucks to be you. This rumor was obviously not true! Everyone except you would've noticed that! Enjoy the punishment.");
					badeffect();

				}

			}

		}

		if (!rn2(100) && uarmf && uarmf->oartifact == ART_RIDDLE_ME_THIS) {
			boolean rumoristrue = rn2(2);

			pline("Alright, riddle me this: Is the following rumor true or not?");

			if (rumoristrue) outrumor(1, BY_OTHER, TRUE);
			else outrumor(-1, BY_OTHER, TRUE);

			if (yn("Now tell me if this rumor was true!") != 'y') { /* player said it's false */

				if (rumoristrue) {

					pline("Haha, you guessed wrong! Tough luck, player, it seems you're just not good enough and now there will be a bad effect to punish you, ha-ha!");
					badeffect();

				} else {

					pline("Damn it, you guessed correctly! I can't believe it! This rumor was actually false!");

				}

			} else { /* player said it's true */

				if (rumoristrue) {

					pline("Oh no, how did you know that this rumor was true? You cheated! You're playing with a spoiler sheet, admit it!");

				} else {

					You("really believe everything you read, huh? Well, sucks to be you. This rumor was obviously not true! Everyone except you would've noticed that! Enjoy the punishment.");
					badeffect();

				}

			}

		}

		if (!rn2(100) && u.uprops[QUIZZES].extrinsic) {
			boolean rumoristrue = rn2(2);

			pline("NetHack Quiz! You will now tell me whether the following rumor is true or not!");

			if (rumoristrue) outrumor(1, BY_OTHER, TRUE);
			else outrumor(-1, BY_OTHER, TRUE);

			if (yn("Now tell me if this rumor was true!") != 'y') { /* player said it's false */

				if (rumoristrue) {

					pline("Haha, you guessed wrong! Tough luck, player, it seems you're just not good enough and now there will be a bad effect to punish you, ha-ha!");
					badeffect();

				} else {

					pline("Damn it, you guessed correctly! I can't believe it! This rumor was actually false!");

				}

			} else { /* player said it's true */

				if (rumoristrue) {

					pline("Oh no, how did you know that this rumor was true? You cheated! You're playing with a spoiler sheet, admit it!");

				} else {

					You("really believe everything you read, huh? Well, sucks to be you. This rumor was obviously not true! Everyone except you would've noticed that! Enjoy the punishment.");
					badeffect();

				}

			}

		}

		if (!rn2(100) && have_quizstone()) {
			boolean rumoristrue = rn2(2);

			pline("NetHack Quiz! You will now tell me whether the following rumor is true or not!");

			if (rumoristrue) outrumor(1, BY_OTHER, TRUE);
			else outrumor(-1, BY_OTHER, TRUE);

			if (yn("Now tell me if this rumor was true!") != 'y') { /* player said it's false */

				if (rumoristrue) {

					pline("Haha, you guessed wrong! Tough luck, player, it seems you're just not good enough and now there will be a bad effect to punish you, ha-ha!");
					badeffect();

				} else {

					pline("Damn it, you guessed correctly! I can't believe it! This rumor was actually false!");

				}

			} else { /* player said it's true */

				if (rumoristrue) {

					pline("Oh no, how did you know that this rumor was true? You cheated! You're playing with a spoiler sheet, admit it!");

				} else {

					You("really believe everything you read, huh? Well, sucks to be you. This rumor was obviously not true! Everyone except you would've noticed that! Enjoy the punishment.");
					badeffect();

				}

			}

		}

		if (!rn2(100) && u.uprops[CAPTCHA].extrinsic) {
			strcpy(buf, rndmonnam() );
			pline("Captcha! Please type in the following word(s) to continue: %s", buf);
			getlin("Your input:",ebuf);
			if (((int) strlen(ebuf) != (int) strlen(buf) ) || (strncmpi(buf, ebuf, (int) strlen(ebuf)) != 0)) {
				pline("WRONG! You will be punished. I will paralyze you, slow you and reduce your max HP and Pw.");

				if (multi >= 0) nomul(-2, "paralyzed by a captcha", FALSE);

				u.ublesscnt += rnz(300);
				change_luck(-1);

				u.ualign.sins++;
				u.alignlim--;
			      adjalign(-10);

				u.uhpmax -= rnd(5);
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				if (u.uhpmax < 1) {
				    u.youaredead = 1;
				    killer = "failing to solve a captcha";
				    killer_format = KILLED_BY;
				    done(DIED);
				    u.youaredead = 0;
				}
				u.uenmax -= rnd(5);
				if (u.uenmax < 0) {
					u.uenmax = u.uen = 0;
				}
				if (Upolyd) {
					u.mhmax -= rnd(10);
					if (u.mh > u.mhmax) u.mh = u.mhmax;
					if (u.mhmax < 1) rehumanize();
				}

			}
			else pline("Alright. Please move on.");
		}

		if (!rn2(100) && have_captchastone()) {
			strcpy(buf, rndmonnam() );
			pline("Captcha! Please type in the following word(s) to continue: %s", buf);
			getlin("Your input:",ebuf);
			if (((int) strlen(ebuf) != (int) strlen(buf) ) || (strncmpi(buf, ebuf, (int) strlen(ebuf)) != 0)) {
				pline("WRONG! You will be punished. I will paralyze you, slow you and reduce your max HP and Pw.");

				if (multi >= 0) nomul(-2, "paralyzed by a captcha", FALSE);

				u.ublesscnt += rnz(300);
				change_luck(-1);

				u.ualign.sins++;
				u.alignlim--;
			      adjalign(-10);

				u.uhpmax -= rnd(5);
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				if (u.uhpmax < 1) {
				    u.youaredead = 1;
				    killer = "failing to solve a captcha";
				    killer_format = KILLED_BY;
				    done(DIED);
				    u.youaredead = 0;
				}
				u.uenmax -= rnd(5);
				if (u.uenmax < 0) {
					u.uenmax = u.uen = 0;
				}
				if (Upolyd) {
					u.mhmax -= rnd(10);
					if (u.mh > u.mhmax) u.mh = u.mhmax;
					if (u.mhmax < 1) rehumanize();
				}

			}
			else pline("Alright. Please move on.");
		}

		if (Race_if(PM_BATMAN) && u.ualign.record > 0 && (rnd(300000) < u.ualign.record) ) {
			struct obj *ubatarang;
			ubatarang = mksobj(BATARANG, TRUE, FALSE);
			if (ubatarang) {
				ubatarang->known = ubatarang->dknown = ubatarang->bknown = ubatarang->rknown = 1;
				ubatarang->owt = weight(ubatarang);
				dropy(ubatarang);
			}
			pline("The HQ grants you a batarang for your valiant efforts.");
		}

		if (u.uprops[UNIDENTIFY].extrinsic ) {

			if (invent) {
			    for (otmpi = invent; otmpi; otmpi = otmpii) {
			      otmpii = otmpi->nobj;
	
				if (!rn2(4000)) {
					otmpi->bknown = FALSE;
				}
				if (!rn2(4000)) {
					otmpi->dknown = FALSE;
				}
				if (!rn2(4000)) {
					otmpi->rknown = FALSE;
				}
				if (!rn2(4000)) {
					otmpi->known = FALSE;
				}
			    }
			}

		}

		if (u.uprops[DEHYDRATION].extrinsic || Dehydration || have_dehydratingstone() ) {
			if (u.dehydrationtime < 1) u.dehydrationtime = moves + 1001;
			if ((u.dehydrationtime - moves) == 100) You("are beginning to feel thirsty.");
			if ((u.dehydrationtime - moves) == 0) You("are dehydrated, your vision begins to blur...");
		} else {
			u.dehydrationtime = 0;
		}

		if (!rn2(10000) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "ghostly cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "prizrachnyy plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "diniy plash") ) ) {
			coord mm;   
			mm.x = u.ux;   
			mm.y = u.uy;   

			tt_mname(&mm, FALSE, 0);
			pline("An enormous ghost appears next to you!");
			You("are frightened to death, and unable to move.");
		    nomul(-3, "frightened to death", TRUE);
			make_feared(HFeared + rnd(30 + (monster_difficulty() * 3) ),TRUE);
		    nomovemsg = "You regain your composure.";
		}

		if (!rn2(10000) && RngeGhostSummoning) {
			coord mm;   
			mm.x = u.ux;   
			mm.y = u.uy;   

			tt_mname(&mm, FALSE, 0);
			pline("An enormous ghost appears next to you!");
			You("are frightened to death, and unable to move.");
		    nomul(-3, "frightened to death", TRUE);
			make_feared(HFeared + rnd(30 + (monster_difficulty() * 3) ),TRUE);
		    nomovemsg = "You regain your composure.";
		}

		if (!rn2(1000) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "chilling cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "pugayushchim plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "sovutgichli plash") ) ) {
			make_frozen(HFrozen + rnd(50),TRUE);
		}

		if (!rn2(2000) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "homicidal cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "smertonosnyy plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "halokatli plash") ) ) {
			makerandomtrap();
		}

		if (!rn2(2000) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "gravity cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "gravitatsionnyy plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "agar tortishish kuchi plash") ) ) {
			pline("Gravity warps around you...");
			phase_door(0);
			pushplayer();
			u.uprops[DEAC_FAST].intrinsic += rnd(10);
			make_stunned(HStun + rnd(10), TRUE);
		}

		if (!rn2(1000) && RngeGravity) {
			pline("Gravity warps around you...");
			phase_door(0);
			pushplayer();
			u.uprops[DEAC_FAST].intrinsic += rnd(10);
			make_stunned(HStun + rnd(10), TRUE);
		}

		if (!rn2(1000) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "flash cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "flesh-plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "bir flesh plash") ) ) {
			make_blinded(Blinded + rnd(10), TRUE);
		}

		if (uwep && uwep->oartifact == ART_EVERYTHING_MUST_BURN && !rn2(100)) {

			make_burned(HBurned + rnd(10), TRUE);

		}

		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_EVERYTHING_MUST_BURN && !rn2(100)) {

			make_burned(HBurned + rnd(10), TRUE);

		}

		if (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "electrostatic cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "elektrostaticheskoye plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "elektrofizikaviy kompyuteringizda ornatilgan plash") ) ) {
			if (!rn2(500)) {
				You("receive an electric shock from your cloak!");
				make_confused(HConfusion + rnd(10),TRUE);
			}
			if (!rn2(500)) {
				You("receive a static shock from your cloak!");
				make_numbed(HNumbed + rnd(10),TRUE);
			}
		}

		if (uarmc && !rn2(5000) && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "irradiation cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "oblucheniye plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "nurlanish plash") ) ) {
			You("are irradiated by your cloak!");
			u.uhpmax -= 1;
			if (u.uhp > u.uhpmax) u.uhp--;
			if (Upolyd) {
				u.mhmax -= 1;
				if (u.mh > u.mhmax) u.mh--;
			}
			u.uenmax -= 1;
			if (u.uen > u.uenmax) u.uen--;
			if (u.uhp < 1) {
				u.youaredead = 1;
				killer = "an irradiation cloak";
				killer_format = KILLED_BY;
				done(DIED);
				u.youaredead = 0;
			}

		}

		if (uarmc && !rn2(2000) && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "hungry cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "golodnymi plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "chanqoq plash") ) ) {
			pline("Suddenly you notice the smell of food...");
			morehungry(rnd(1000));
		}

		if (RngeVoltage) {
			if (!rn2(500)) {
				You("receive an electric shock out of nowhere!");
				make_confused(HConfusion + rnd(10),TRUE);
			}
			if (!rn2(500)) {
				You("receive a static shock out of nowhere!");
				make_numbed(HNumbed + rnd(10),TRUE);
			}
		}

		if (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "bug-tracking helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "oshibka otslezhivaniya shlem") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "hasharotlar-kuzatish dubulg'a") ) && !rn2(10000) ) {
			pline("Bugs are alerted to your position.");
			int ammount;
			ammount = rnd(15);

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			while (ammount--) {
		 	    (void) makemon(mkclass(S_XAN,0), u.ux, u.uy, NO_MM_FLAGS);
			}

			u.aggravation = 0;

		}

		if (u.drippingtread) {

			u.drippingtread--;

			if (isok(u.ux, u.uy) && (levl[u.ux][u.uy].typ == ROOM || levl[u.ux][u.uy].typ == CORR)) {
				switch (u.drippingtreadtype) {
					case 1: levl[u.ux][u.uy].typ = WATER; break;
					case 2: levl[u.ux][u.uy].typ = LAVAPOOL; break;
					case 3: levl[u.ux][u.uy].typ = ICE; break;
					case 4: levl[u.ux][u.uy].typ = CLOUD; break;
					default: impossible("Dripping tread terrain type uninitialized");
				}
			}

			if (u.drippingtread < 0) u.drippingtread = 0;
			if (!u.drippingtread) {
				u.drippingtreadtype = 0;
				You("stop dripping elements.");
			}

		}

		if (u.geolysis) {

			u.geolysis--;
			if (u.geolysis < 0) u.geolysis = 0;
			if (!u.geolysis) {
				You("can no longer eat through rock.");
			}

		}

		if (u.inertiacontrol) {

			castinertiaspell();

			u.inertiacontrol--;
			if (u.inertiacontrol < 0) u.inertiacontrol = 0;
			if (!u.inertiacontrol) {
				pline("Inertia control has timed out.");
				u.inertiacontrolspell = -1;
				u.inertiacontrolspellno = -1;
			}

		}

		if (u.spellbinder) u.spellbinder = 0;

		if (u.halresdeactivated) {

			u.halresdeactivated--;
			if (u.halresdeactivated < 0) u.halresdeactivated = 0;
			if (!u.halresdeactivated) {
				You("are no longer prevented from having hallucination resistance.");
			}

		}

		if (u.sterilized) {

			u.sterilized--;
			if (u.sterilized < 0) u.sterilized = 0;
			if (!u.sterilized) {
				You("no longer feel an anti-sexual aura.");
			}

		}

		if (u.disruptionshield) {

			u.disruptionshield--;
			if (u.disruptionshield < 0) u.disruptionshield = 0;
			if (!u.disruptionshield) {
				Your("disruption shield dissipates.");
			}

		}

		if (u.holyshield) {

			u.holyshield--;
			if (u.holyshield < 0) u.holyshield = 0;
			if (!u.holyshield) {
				Your("holy shield dissipates.");
			}

		}

		if ((u.uprops[CRAP_EFFECT].extrinsic || (uwep && uwep->oartifact == ART_LUISA_S_CHARMING_BEAUTY) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_LUISA_S_CHARMING_BEAUTY) || (uarmc && uarmc->oartifact == ART_FEMMY_FATALE) || (uwep && uwep->oartifact == ART_GIRLFUL_BONKING) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_GIRLFUL_BONKING) || CrapEffect || have_shitstone() ) && (u.uhs == 0) && !rn2(100) ) {
			You("suddenly have to take a shit!");
			int crapduration = 5;
			if (uarm && objects[uarm->otyp].oc_delay) {
				pline("Taking off your armor is going to take a while...");
				crapduration += objects[uarm->otyp].oc_delay;
			}
			if (uarmc && objects[uarmc->otyp].oc_delay) {
				You("need to remove your cloak...");
				crapduration += objects[uarmc->otyp].oc_delay;
			}
			if (Sick && !rn2(3) ) make_sick(0L, (char *)0, TRUE, SICK_VOMITABLE);
			else if (Sick && !rn2(10) ) make_sick(0L, (char *)0, TRUE, SICK_ALL);
			if (u.uhs == 0) morehungry(rn2(400)+200);
			nomovemsg = "You are done shitting.";
			nomul(-crapduration, "taking a shit", TRUE);

			if (!rn2(5)) {
				You("did not watch out, and stepped into your own shit.");
			    if (uarmf && !rn2(5)) (void)rust_dmg(uarmf, xname(uarmf), 0, TRUE, &youmonst);
			    if (uarmf && !rn2(5)) (void)rust_dmg(uarmf, xname(uarmf), 1, TRUE, &youmonst);
			    if (uarmf && !rn2(5)) (void)rust_dmg(uarmf, xname(uarmf), 2, TRUE, &youmonst);
			    if (uarmf && !rn2(5)) (void)rust_dmg(uarmf, xname(uarmf), 3, TRUE, &youmonst);
				HFumbling = FROMOUTSIDE | rnd(5);
				incr_itimeout(&HFumbling, rnd(2));
				u.fumbleduration += rnz(30);
				if (!uarmf) losehp(rnd(u.ulevel * 5), "diarrhea", KILLED_BY);
			}
		}

		if (( (u.uhave.amulet && (u.amuletcompletelyimbued || !rn2(5)) && !rn2(5)) || Clairvoyant) &&
		    !In_endgame(&u.uz) && !BClairvoyant &&
		    !(moves % 15) && !rn2(2))
			do_vicinity_map();

		if (u.utrap && (ttmp = t_at(u.ux, u.uy)) && ttmp && ttmp->ttyp == ANOXIC_PIT && !Breathless) {
			pline("The air in the anoxic pit does not contain oxygen! You can't breathe!");
			losehp(u.ulevel * 3, "being stuck in an anoxic pit", KILLED_BY);
		}
		/* jonadab invented the anoxic pit, and later changed the name to hypoxic pit for whatever reason, which
		 * sounds much less badass than anoxic pit. I (Amy) learned Ancient Greek in school, so I know what those names
		 * mean. Anoxic means "does not contain oxygen AT ALL", and will therefore suffocate you very quickly, while
		 * hypoxic only means "contains a little less oxygen than regular air". While that will still suffocate you
		 * after a while (I seem to recall humans need about 15% oxygen content in air to breathe), it certainly
		 * makes the trap sound less powerful and dangerous than it really is, so I'll call it anoxic pit. */
	
		if(u.utrap && u.utraptype == TT_LAVA) {
		    if(!is_lava(u.ux,u.uy))
			u.utrap = 0;
		    else if (!u.uinvulnerable) {
			u.utrap -= 1<<8;
			if(u.utrap < 1<<8) {
			    u.youaredead = 1;
			    killer_format = KILLED_BY;
			    killer = "molten lava";
			    You(Hallucination ? "dissolve completely, warping to another plane of existence." : "sink below the surface and die.");
			    done(DISSOLVED);
			    u.youaredead = 0;
			} else if(didmove && !u.umoved) {
			    /*Norep*/pline(Hallucination ? "Your body is dissolving... maybe the Grim Reaper is waiting for you?" : "You sink deeper into the lava.");
		if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			    u.utrap += rnd(4);
			}
		    }
		}


		    if (flags.bypasses) clear_bypasses();
		    if(IsGlib) glibr();

		    if (!rn2(2) || !(uarmf && OBJ_DESCR(objects[uarmf->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "irregular boots") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "neregulyarnyye sapogi") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "tartibsizlik chizilmasin") ) ) ) {

			if (!rn2(2) || !((uleft && uleft->oartifact == ART_GOOD_THINGS_WILL_HAPPEN_EV) || (uright && uright->oartifact == ART_GOOD_THINGS_WILL_HAPPEN_EV)) ) {
				if (!rn2(2) || !RngeIrregularity) {
				    nh_timeout();
				}
			}
		    }

		    run_regions();

#ifdef DUNGEON_GROWTH
		    dgn_growths(TRUE, TRUE);
#endif

		    if (u.ublesscnt) {
				if (NonprayerBug || u.uprops[NON_PRAYER_BUG].extrinsic || have_antiprayerstone()) u.ublesscnt++;
				else u.ublesscnt--;
			}
		    if (u.ublesscnt < 0) u.ublesscnt = 0; /* fail safe */

		if (uarmg && u.ublesscnt && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "comfortable gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "udobnyye perchatki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "qulay qo'lqop") ) ) u.ublesscnt--;

		if (u.ublesscnt && RngePrayer) u.ublesscnt--;

		if (u.ublesscnt && !(PlayerCannotUseSkills)) {

			if ((P_SKILL(P_SPIRITUALITY) >= P_BASIC) && u.ublesscnt && !rn2(10)) u.ublesscnt--;
			if ((P_SKILL(P_SPIRITUALITY) >= P_SKILLED) && u.ublesscnt && !rn2(10)) u.ublesscnt--;
			if ((P_SKILL(P_SPIRITUALITY) >= P_EXPERT) && u.ublesscnt && !rn2(10)) u.ublesscnt--;
			if ((P_SKILL(P_SPIRITUALITY) >= P_MASTER) && u.ublesscnt && !rn2(10)) u.ublesscnt--;
			if ((P_SKILL(P_SPIRITUALITY) >= P_GRAND_MASTER) && u.ublesscnt && !rn2(10)) u.ublesscnt--;
			if ((P_SKILL(P_SPIRITUALITY) >= P_SUPREME_MASTER) && u.ublesscnt && !rn2(10)) u.ublesscnt--;

		}

		    if (u.ublesscnt < 0) u.ublesscnt = 0; /* fail safe */
		    
		    if(flags.time && !flags.run)
			flags.botl = 1;

			if(u.ukinghill){
				if(u.protean > 0) u.protean--;
				else{
					for(pobj = invent; pobj; pobj=pobj->nobj)
						if(pobj->oartifact == ART_TREASURY_OF_PROTEUS)
							break;
					if(!pobj) pline("Treasury not actually in inventory??");
					else if(pobj->cobj){
						arti_poly_contents(pobj);
					}
					u.protean = rnz(100)+d(3,10);
					update_inventory();
				}
			}

		    /* One possible result of prayer is healing.  Whether or
		     * not you get healed depends on your current hit points.
		     * If you are allowed to regenerate during the prayer, the
		     * end-of-prayer calculation messes up on this.
		     * Another possible result is rehumanization, which requires
		     * that encumbrance and movement rate be recalculated.
		     */
		    if (u.uinvulnerable) {
			/* for the moment at least, you're in tiptop shape */
			wtcap = UNENCUMBERED;
		    } else if (Upolyd && youmonst.data->mlet == S_EEL && !is_waterypool(u.ux,u.uy) && !is_crystalwater(u.ux,u.uy) && !Is_waterlevel(&u.uz)) {
			if (u.mh > 1) {
			    u.mh--;
			    flags.botl = 1;
			} else if (u.mh < 1)
			    rehumanize();
		    } else if (Upolyd && u.mh < u.mhmax && (rn2(2) || (!sengr_at("Elbereth", u.ux, u.uy) ) ) ) {

			/* faster regeneration --Amy */
			regenrate = (20 - (u.ulevel / 3));
			if (regenrate < 6) regenrate = 6;
			if (Race_if(PM_HAXOR)) regenrate /= 2;
			if (is_grassland(u.ux, u.uy)) regenrate *= 2;

			if (u.mh < 1)
			    rehumanize();
			else if (Regeneration ||
				    (wtcap < MOD_ENCUMBER && !(moves%/*20*/regenrate))) {
			    flags.botl = 1;
			    if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) ) u.mh++;
			}

		/* evil patch idea by b_jonas: slower HP regeneration while standing on Elbereth
               he actually proposes regeneration to be completely eliminated, but let's not be too cruel here... --Amy */

		    } else if (u.uhp < u.uhpmax &&
			 (wtcap < MOD_ENCUMBER || !u.umoved || Regeneration) && (rn2(2) || (!sengr_at("Elbereth", u.ux, u.uy) ) ) ) {
/*
 * KMH, balance patch -- New regeneration code
 * Healthstones have been added, which alter your effective
 * experience level and constitution (-2 cursed, +1 uncursed,
 * +2 blessed) for the basis of regeneration calculations.
 */

			/* adjustments by Amy - make it slower, because otherwise intrinsic regeneration is useless later on */
 			int efflev = rnd(u.ulevel) + (u.uhealbonus);
 			int effcon = rnd(ACURR(A_CON)) + (u.uhealbonus);

			if (!(PlayerCannotUseSkills)) {

			if (!issoviet) {
				if (P_SKILL(P_RIDING) == P_SKILLED) efflev += 2;
				if (P_SKILL(P_RIDING) == P_EXPERT) efflev += 5;
				if (P_SKILL(P_RIDING) == P_MASTER) efflev += 7;
				if (P_SKILL(P_RIDING) == P_GRAND_MASTER) efflev += 10;
				if (P_SKILL(P_RIDING) == P_SUPREME_MASTER) efflev += 12;
				if (P_SKILL(P_RIDING) == P_SKILLED) effcon += 2;
				if (P_SKILL(P_RIDING) == P_EXPERT) effcon += 5;
				if (P_SKILL(P_RIDING) == P_MASTER) effcon += 7;
				if (P_SKILL(P_RIDING) == P_GRAND_MASTER) effcon += 10;
				if (P_SKILL(P_RIDING) == P_SUPREME_MASTER) effcon += 12;
			} else {
				if (P_SKILL(P_RIDING) == P_SKILLED) efflev -= 2;
				if (P_SKILL(P_RIDING) == P_EXPERT) efflev -= 5;
				if (P_SKILL(P_RIDING) == P_MASTER) efflev -= 7;
				if (P_SKILL(P_RIDING) == P_GRAND_MASTER) efflev -= 10;
				if (P_SKILL(P_RIDING) == P_SUPREME_MASTER) efflev -= 12;
				if (P_SKILL(P_RIDING) == P_SKILLED) effcon -= 2;
				if (P_SKILL(P_RIDING) == P_EXPERT) effcon -= 5;
				if (P_SKILL(P_RIDING) == P_MASTER) effcon -= 7;
				if (P_SKILL(P_RIDING) == P_GRAND_MASTER) effcon -= 10;
				if (P_SKILL(P_RIDING) == P_SUPREME_MASTER) effcon -= 12;
			}

			}

	/* Yeah I know this makes no sense at all, but it improves the usefulness of the riding skill. --Amy */
			int heal = 1;


			if ( efflev > 9 && !(moves % 3)) {
			    if (effcon <= 12) {
				heal = 1;
			    } else {
				heal = rnd(effcon / 5) + 1;
  				if (heal > efflev-9) heal = efflev-9;
			    }
			    flags.botl = 1;
			    if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) ) u.uhp += heal;
			    if(u.uhp > u.uhpmax)
				u.uhp = u.uhpmax;
			} else if (Regeneration ||
			     (efflev <= 9 &&
			      !(moves % ((MAXULEV+12) / (u.ulevel+2) + 1)))) {
			    flags.botl = 1;
			    if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) ) u.uhp++;
			}
		    }

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && Race_if(PM_HAXOR) && !rn2(20) && (rn2(2) || (!sengr_at("Elbereth", u.ux, u.uy) ) ) ) {
				u.uhp += rnd(5 + (u.ulevel / 5));
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				flags.botl = 1;
			}
			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && Race_if(PM_HAXOR) && Upolyd && !rn2(20) && (rn2(2) || (!sengr_at("Elbereth", u.ux, u.uy) ) ) ) {
				u.mh += rnd(5 + (u.ulevel / 5));
				if (u.mh > u.mhmax) u.mh = u.mhmax;
				flags.botl = 1;
			}
			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH)) && !rn2(60 / u.ulevel) ) {
				u.uhp++;
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				flags.botl = 1;
			}
			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH)) && !rn2(60 / u.ulevel) && Upolyd ) {
				u.mh++;
				if (u.mh > u.mhmax) u.mh = u.mhmax;
				flags.botl = 1;
			}

			/* nice patch addition by Amy - sometimes regenerate more */
			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && !rn2(150) && (rn2(2) || (!sengr_at("Elbereth", u.ux, u.uy) ) ) ){
				u.uhp += rnz(2 + u.ulevel);
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				flags.botl = 1;
			}
			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && !rn2(150) && Upolyd && (rn2(2) || (!sengr_at("Elbereth", u.ux, u.uy) ) ) ){
				u.mh += rnz(2 + u.ulevel);
				if (u.mh > u.mhmax) u.mh = u.mhmax;
				flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && Race_if(PM_HAXOR) && !rn2(20) ) {
				u.uen += rnd(5 + (u.ulevel / 5));
				if (u.uen > u.uenmax) u.uen = u.uenmax;
				flags.botl = 1;
			}

		    if (!u.uinvulnerable && u.uen > 0 && u.uhp < u.uhpmax &&
			    tech_inuse(T_CHI_HEALING)) {
			u.uen--;
			u.uhp++;
			flags.botl = 1;
		    }

		    /* moving around while encumbered is hard work */
		    if (wtcap > MOD_ENCUMBER && u.umoved) {
			if(!(wtcap < EXT_ENCUMBER ? moves%30 : moves%10)) {
			    if (Upolyd && u.mh > 1) {
				u.mh--;
			    } else if (!Upolyd && u.uhp > 1) {
				u.uhp--;
			    } else {
				You(Hallucination ? "are too trippy to stand on two legs, so you fall down." : "pass out from exertion!");
				exercise(A_CON, FALSE);
				fall_asleep(-10, FALSE);
			    }
			}
		    }

		    
		    /* KMH -- OK to regenerate if you don't move */
		    if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && (recalc_mana() >= 0 || (!rn2(-(recalc_mana() - 1) ) ) ) && (u.uen < u.uenmax) && 
				((Energy_regeneration && !rn2(3)) || /* greatly nerfed overpowered wizard artifact --Amy */
				(Role_if(PM_ALTMER) && !rn2(5)) || /* altmer have extra mana regeneration --Amy */
				((wtcap < MOD_ENCUMBER || !flags.mv) &&
				(!(moves%((MAXULEV + 15 - u.ulevel) *                                    
				(Role_if(PM_WIZARD) ? 3 : 4) / 6)))))) {
			u.uen += rn1((int)(ACURR(A_WIS) + ACURR(A_INT)) / 15 + 1,1);

#ifdef WIZ_PATCH_DEBUG
                pline("mana was = %d now = %d",temp,u.uen);
#endif

			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
		    }

			/* nice patch addition by Amy - sometimes regenerate more */
			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && !issoviet && (rn2(2) || !Race_if(PM_SYLPH) ) && !rn2(250) && (u.uen < u.uenmax)) {

				u.uen += rnz(2 + u.ulevel);
				if (u.uen > u.uenmax)  u.uen = u.uenmax;
				flags.botl = 1;

			}
			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && !issoviet && (rn2(2) || !Race_if(PM_SYLPH) ) && !rn2(250) && (u.uen < u.uenmax) && Energy_regeneration) {

				u.uen += rnz(2 + u.ulevel);
				if (u.uen > u.uenmax)  u.uen = u.uenmax;
				flags.botl = 1;

			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && !issoviet && (rn2(2) || !Race_if(PM_SYLPH) ) && !rn2(50) && (u.uen < u.uenmax) && recalc_mana() > 0) {

				u.uen += rnd(recalc_mana());
				if (u.uen > u.uenmax)  u.uen = u.uenmax;
				flags.botl = 1;

			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && Race_if(PM_RODNEYAN)) { /* rodney has special built-in energy regeneration --Amy */
				u.uen++;
				if (u.uen > u.uenmax)  u.uen = u.uenmax;
				flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && !issoviet && (rn2(2) || !Race_if(PM_SYLPH)) && !rn2(90 / u.ulevel) ) {
				u.uen++;
				if (u.uen > u.uenmax)  u.uen = u.uenmax;
				flags.botl = 1;
			}

		/* leveling up will give a small boost to mana regeneration now --Amy */
		    if ( !Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && !issoviet && (rn2(2) || !Race_if(PM_SYLPH) ) && u.uen < u.uenmax && ( 
			(u.ulevel >= 5 && !rn2(200)) ||
			(u.ulevel >= 10 && !rn2(100)) ||
			(u.ulevel >= 14 && !rn2(100)) ||
			(u.ulevel >= 15 && !rn2(50)) ||
			(u.ulevel >= 20 && !rn2(30)) ||
			(u.ulevel >= 25 && !rn2(20)) ||
			(u.ulevel >= 30 && !rn2(10)) ||
			(u.menoraget && !rn2(200)) ||
			(u.bookofthedeadget && !rn2(200)) ||
			(u.silverbellget && !rn2(200)) ||
			(u.chaoskeyget && !rn2(500)) ||
			(u.neutralkeyget && !rn2(500)) ||
			(u.lawfulkeyget && !rn2(500)) ||
			(u.medusaremoved && !rn2(250)) ||
			(u.luckstoneget && !rn2(1000)) ||
			(u.deepminefinished && !rn2(1000)) ||
			(u.sokobanfinished && !rn2(1000))
			)
			)
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			/* Having a spell school at skilled will improve mana regeneration.
			 * Having a spell school at expert will improve it by even more. --Amy */

			if (!(issoviet || PlayerCannotUseSkills)) {

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ATTACK_SPELL) == P_SKILLED && !rn2(200))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ATTACK_SPELL) == P_EXPERT && !rn2(100))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ATTACK_SPELL) == P_MASTER && !rn2(50))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ATTACK_SPELL) == P_GRAND_MASTER && !rn2(25))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ATTACK_SPELL) == P_SUPREME_MASTER && !rn2(15))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_DIVINATION_SPELL) == P_SKILLED && !rn2(200))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_DIVINATION_SPELL) == P_EXPERT && !rn2(100))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_DIVINATION_SPELL) == P_MASTER && !rn2(50))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_DIVINATION_SPELL) == P_GRAND_MASTER && !rn2(25))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_DIVINATION_SPELL) == P_SUPREME_MASTER && !rn2(15))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_MATTER_SPELL) == P_SKILLED && !rn2(200))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_MATTER_SPELL) == P_EXPERT && !rn2(100))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_MATTER_SPELL) == P_MASTER && !rn2(50))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_MATTER_SPELL) == P_GRAND_MASTER && !rn2(25))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_MATTER_SPELL) == P_SUPREME_MASTER && !rn2(15))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_OCCULT_SPELL) == P_SKILLED && !rn2(200))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_OCCULT_SPELL) == P_EXPERT && !rn2(100))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_OCCULT_SPELL) == P_MASTER && !rn2(50))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_OCCULT_SPELL) == P_GRAND_MASTER && !rn2(25))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_OCCULT_SPELL) == P_SUPREME_MASTER && !rn2(15))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ELEMENTAL_SPELL) == P_SKILLED && !rn2(200))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ELEMENTAL_SPELL) == P_EXPERT && !rn2(100))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ELEMENTAL_SPELL) == P_MASTER && !rn2(50))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ELEMENTAL_SPELL) == P_GRAND_MASTER && !rn2(25))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ELEMENTAL_SPELL) == P_SUPREME_MASTER && !rn2(15))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_CHAOS_SPELL) == P_SKILLED && !rn2(200))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_CHAOS_SPELL) == P_EXPERT && !rn2(100))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_CHAOS_SPELL) == P_MASTER && !rn2(50))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_CHAOS_SPELL) == P_GRAND_MASTER && !rn2(25))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_CHAOS_SPELL) == P_SUPREME_MASTER && !rn2(15))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_BODY_SPELL) == P_SKILLED && !rn2(200))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_BODY_SPELL) == P_EXPERT && !rn2(100))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_BODY_SPELL) == P_MASTER && !rn2(50))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_BODY_SPELL) == P_GRAND_MASTER && !rn2(25))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_BODY_SPELL) == P_SUPREME_MASTER && !rn2(15))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_PROTECTION_SPELL) == P_SKILLED && !rn2(200))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_PROTECTION_SPELL) == P_EXPERT && !rn2(100))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_PROTECTION_SPELL) == P_MASTER && !rn2(50))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_PROTECTION_SPELL) == P_GRAND_MASTER && !rn2(25))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_PROTECTION_SPELL) == P_SUPREME_MASTER && !rn2(15))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ENCHANTMENT_SPELL) == P_SKILLED && !rn2(200))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ENCHANTMENT_SPELL) == P_EXPERT && !rn2(100))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ENCHANTMENT_SPELL) == P_MASTER && !rn2(50))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ENCHANTMENT_SPELL) == P_GRAND_MASTER && !rn2(25))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ENCHANTMENT_SPELL) == P_SUPREME_MASTER && !rn2(15))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_HEALING_SPELL) == P_SKILLED && !rn2(200))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_HEALING_SPELL) == P_EXPERT && !rn2(100))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_HEALING_SPELL) == P_MASTER && !rn2(50))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_HEALING_SPELL) == P_GRAND_MASTER && !rn2(25))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_HEALING_SPELL) == P_SUPREME_MASTER && !rn2(15))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			}

			/* Spooky faux error messages on the Spacewars Fighter goal level --Amy */
			if ((Role_if(PM_SPACEWARS_FIGHTER) && !rn2(200) && Is_nemesis(&u.uz) ) || (Role_if(PM_CAMPERSTRIKER) && !rn2(200) && In_quest(&u.uz)) ) {
			pline("Warning: Low Local Memory. Freeing description strings.");
			/* These --More-- messages are forced by design, no matter what flags.forcemore is set to! --Amy */
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			pline(" ");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			pline(" ");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			pline(" ");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			pline(" ");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			pline(" ");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			pline(" ");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			pline(" ");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			pline(" ");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			pline(" ");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			pline(" ");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			pline(" ");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			pline(" ");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			pline(" ");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			pline(" ");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			pline(" ");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			pline(" ");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			pline(" ");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			pline(" ");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			pline(" ");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			pline(" ");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			pline(" ");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			pline(" ");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			pline(" ");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			pline(" ");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			pline(" ");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			if (!rn2(25)) pline("nt|| - Not a valid save file");
			else if (!rn2(25)) pline("NETHACK.EXE caused a General Protection Fault at address 000D:001D.");
			else if (!rn2(25)) pline("APPLICATION ERROR - integer divide by 0");
			else if (!rn2(25)) {pline("Runtime error! Program: NETHACK.EXE");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
				pline("R6025 - pure virtual function call");
			}
			else if (!rn2(25)) {pline("Buffer overrun detected! Program: NETHACK.EXE");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
				pline("A buffer overrun has been detected which has corrupted the program's internal state.");
				pline("The program cannot safely continue execution and must now be terminated.");
			}
			else if (!rn2(25)) {pline("Runtime error! Program: NETHACK.EXE");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
				pline("This application has requested the Runtime to terminate it in an unusual way.");
				pline("Please contact the application's support team for more information.");
			}
			else if (!rn2(25)) pline("Not enough memory to create inventory window");
			else if (!rn2(25)) pline("Error: Nethack will only run in Protect mode");
			else if (!rn2(25)) {pline("Oops...");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
				pline("Suddenly, the dungeon collapses.");
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
				pline("NETHACK.EXE has stopped working. Unsaved data may have been lost.");
			}
			else if (!rn2(25)) pline("ERROR: SIGNAL 11 WAS RAISED");
			else if (!rn2(25)) pline("UNHANDLED EXCEPTION: ACCESS_VIOLATION (C0000005)");
			else if (!rn2(25)) pline("An error has occurred in your application. If you choose Close, your application will be terminated. If you choose Ignore, you should save your work in a new file.");
			else if (!rn2(25)) pline("Do you want your possessions identified? DYWYPI?");
			else if (!rn2(25)) pline("Windows Subsystem service has stopped unexpectedly.");
			else if (!rn2(25)) pline("nv4_disp.dll device driver is stuck in an infinite loop.");
			else if (!rn2(25)) pline("The win16 subsystem has insufficient resources to continue running. Click on OK, close your application and restart your machine.");
			else if (!rn2(25)) pline("System resources depleted. Please restart your computer.");

				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			}

			/* more faux messages */
			if ((Role_if(PM_SPACEWARS_FIGHTER) && !rn2(200) && Is_nemesis(&u.uz) ) || (Role_if(PM_CAMPERSTRIKER) && !rn2(200) && In_quest(&u.uz)) ) {
				pline(fauxmessage());
				if (!rn2(3)) pline(fauxmessage());
			}

			if (u.stairscumslowing) {
				u.stairscumslowing--;
				if (u.stairscumslowing < 0) u.stairscumslowing = 0;
			}

		    if(!u.uinvulnerable) {
			if(Teleportation && (ishaxor ? !rn2(150) : !rn2(250)) ) {
			    xchar old_ux = u.ux, old_uy = u.uy;
				You(Hallucination ? "open a warp gate!" : "suddenly get teleported!");
			    tele();
				if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			    if (u.ux != old_ux || u.uy != old_uy) {
				if (!next_to_u()) {
				    check_leash(old_ux, old_uy);
				}
				/* clear doagain keystrokes */
				pushch(0);
				savech(0);
			    }
			}

			if (tech_inuse(T_GLOWHORN)) {
				use_unicorn_horn((struct obj *)0);
			}

			if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER) && uimplant && uimplant->oartifact == ART_HEALENERATION) {
				use_unicorn_horn((struct obj *)0);
			}

			if (Race_if(PM_RODNEYAN) && !rn2(1000)) {	/* levelteleportitis --Amy */

				make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */
				pline("A mysterious force surrounds you...");
			      if (!flags.lostsoul && !flags.uberlostsoul && !(flags.wonderland && !(u.wonderlandescape)) && !(u.uprops[STORM_HELM].extrinsic) && !(In_bellcaves(&u.uz)) && !(In_subquest(&u.uz)) && !(In_voiddungeon(&u.uz)) && !(In_netherrealm(&u.uz)) ) level_tele();
				else You_feel("very disoriented but decide to move on.");

			}

			/* For developer race, Rodney wakes up prematurely after 20000 turns! --Amy */
			if (Race_if(PM_DEVELOPER) && !u.uevent.udemigod && moves > 20000) {
				u.uevent.udemigod = TRUE;
				u.udg_cnt = rn1(250, 50);
			}

			if (Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit && !rn2(1000)) {
				if (!Blind) pline("The bright light blinds you!");
				make_blinded(Blinded+rnz(100),FALSE);
				if (!rn2(5)) set_itimeout(&HeavyBlind, Blinded);
			}

			if (!rn2(10000) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "chinese cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "kitayskiy plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "xitoy plash") ) ) {

				if (u.uevent.udemigod || u.uhave.amulet || (uarm && uarm->oartifact == ART_CHECK_YOUR_ESCAPES) || NoReturnEffect || u.uprops[NORETURN].extrinsic || have_noreturnstone() || (u.usteed && mon_has_amulet(u.usteed))) {
					NastinessProblem += rnd(1000);
					You("can hear Arabella giggling.");
					break;
				}

				if (flags.lostsoul || flags.uberlostsoul || (flags.wonderland && !(u.wonderlandescape)) || u.uprops[STORM_HELM].extrinsic || In_bellcaves(&u.uz) || In_subquest(&u.uz) || In_voiddungeon(&u.uz) || In_netherrealm(&u.uz)) { 
					NastinessProblem += rnd(1000);
					You("can hear Arabella announce: 'Sorry, but the time of your demise is drawing near.'");
					break;
				}

				make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

				if (rn2(2)) {(void) safe_teleds(FALSE); goto_level(&medusa_level, TRUE, FALSE, FALSE); }
				else {(void) safe_teleds(FALSE); goto_level(&portal_level, TRUE, FALSE, FALSE); }

				register int newlev = rnd(99);
				d_level newlevel;
				get_level(&newlevel, newlev);
				goto_level(&newlevel, TRUE, FALSE, FALSE);
				You("were banished!");

			}

			if (!rn2(10000) && RngeChina) {

				if (u.uevent.udemigod || u.uhave.amulet || (uarm && uarm->oartifact == ART_CHECK_YOUR_ESCAPES) || NoReturnEffect || u.uprops[NORETURN].extrinsic || have_noreturnstone() || (u.usteed && mon_has_amulet(u.usteed))) {
					NastinessProblem += rnd(1000);
					You("can hear Arabella giggling.");
					break;
				}

				if (flags.lostsoul || flags.uberlostsoul || (flags.wonderland && !(u.wonderlandescape)) || u.uprops[STORM_HELM].extrinsic || In_bellcaves(&u.uz) || In_subquest(&u.uz) || In_voiddungeon(&u.uz) || In_netherrealm(&u.uz)) { 
					NastinessProblem += rnd(1000);
					You("can hear Arabella announce: 'Sorry, but the time of your demise is drawing near.'");
					break;
				}

				make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

				if (rn2(2)) {(void) safe_teleds(FALSE); goto_level(&medusa_level, TRUE, FALSE, FALSE); }
				else {(void) safe_teleds(FALSE); goto_level(&portal_level, TRUE, FALSE, FALSE); }

				register int newlev = rnd(99);
				d_level newlevel;
				get_level(&newlevel, newlev);
				goto_level(&newlevel, TRUE, FALSE, FALSE);
				You("were banished!");

			}

			if (!rn2(10000) && uarmc && uarmc->oartifact == ART_ARABELLA_S_LIGHTNINGROD) {

				if (u.uevent.udemigod || u.uhave.amulet || (uarm && uarm->oartifact == ART_CHECK_YOUR_ESCAPES) || NoReturnEffect || u.uprops[NORETURN].extrinsic || have_noreturnstone() || (u.usteed && mon_has_amulet(u.usteed))) {
					NastinessProblem += rnd(1000);
					You("can hear Arabella giggling.");
					break;
				}

				if (flags.lostsoul || flags.uberlostsoul || (flags.wonderland && !(u.wonderlandescape)) || u.uprops[STORM_HELM].extrinsic || In_bellcaves(&u.uz) || In_subquest(&u.uz) || In_voiddungeon(&u.uz) || In_netherrealm(&u.uz)) { 
					NastinessProblem += rnd(1000);
					You("can hear Arabella announce: 'Sorry, but the time of your demise is drawing near.'");
					break;
				}

				make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

				if (rn2(2)) {(void) safe_teleds(FALSE); goto_level(&medusa_level, TRUE, FALSE, FALSE); }
				else {(void) safe_teleds(FALSE); goto_level(&portal_level, TRUE, FALSE, FALSE); }

				register int newlev = rnd(99);
				d_level newlevel;
				get_level(&newlevel, newlev);
				goto_level(&newlevel, TRUE, FALSE, FALSE);
				You("were banished!");

			}

			if (!rn2(10000) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "polyform cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "sopolimer forma plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "belgigacha bo'lgan poli shakli plash") ) ) {
				if (!HPolymorph_control) HPolymorph_control = 2;
				You_feel("polyform.");
				if (!Unchanging) polyself(FALSE);
			}

			if (!rn2(10000) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "contaminated coat") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "zagryaznennoye pal'to") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "ifloslangan palto") )) {
				if (Sick_resistance) {
					You_feel("a slight illness.");
				} else {
					make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON), 40),
				"contaminated coat", TRUE, SICK_NONVOMITABLE);
				}
			    stop_occupation();

			}

			if (!rn2(2000) && RngeSickness) {
				if (Sick_resistance) {
					You_feel("a slight illness.");
				} else {
					make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON), 40),
				"cursed sickness", TRUE, SICK_NONVOMITABLE);
				}
			    stop_occupation();

			}

			if (uarmh && !rn2(1000) && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "weeping helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "placha shlem") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "yig'lab dubulg'a") ) ) {

				make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */
				pline("A mysterious force surrounds you...");
			      if (!flags.lostsoul && !flags.uberlostsoul && !(flags.wonderland && !(u.wonderlandescape)) && !(u.uprops[STORM_HELM].extrinsic) && !(In_bellcaves(&u.uz)) && !(In_subquest(&u.uz)) && !(In_voiddungeon(&u.uz)) && !(In_netherrealm(&u.uz))) level_tele();
				else You_feel("very disoriented but decide to move on.");

			}

			if (RngeWeeping && !rn2(1000)) {

				make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */
				pline("A mysterious force surrounds you...");
			      if (!flags.lostsoul && !flags.uberlostsoul && !(flags.wonderland && !(u.wonderlandescape)) && !(u.uprops[STORM_HELM].extrinsic) && !(In_bellcaves(&u.uz)) && !(In_subquest(&u.uz)) && !(In_voiddungeon(&u.uz)) && !(In_netherrealm(&u.uz))) level_tele();
				else You_feel("very disoriented but decide to move on.");

			}

			if (uarmg && uarmg->oartifact == ART_ARABELLA_S_BANK_OF_CROSSRO) {

				make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */
				pline("A mysterious force surrounds you...");
			      if (!flags.lostsoul && !flags.uberlostsoul && !(flags.wonderland && !(u.wonderlandescape)) && !(u.uprops[STORM_HELM].extrinsic) && !(In_bellcaves(&u.uz)) && !(In_subquest(&u.uz)) && !(In_voiddungeon(&u.uz)) && !(In_netherrealm(&u.uz))) level_tele();
				else You_feel("very disoriented but decide to move on.");

			}

			if (uarmg && uarmg->oartifact == ART_ARABELLA_S_GREAT_BANISHER) {

				make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */
				pline("A mysterious force surrounds you...");
			      if (!flags.lostsoul && !flags.uberlostsoul && !(flags.wonderland && !(u.wonderlandescape)) && !(u.uprops[STORM_HELM].extrinsic) && !(In_bellcaves(&u.uz)) && !(In_subquest(&u.uz)) && !(In_voiddungeon(&u.uz)) && !(In_netherrealm(&u.uz))) level_tele();
				else You_feel("very disoriented but decide to move on.");

			}

			if ((WereformBug || u.uprops[WEREFORM_BUG].extrinsic || have_wereformstone()) && !rn2(1200 - (200 * night()))) {

				int wereformattempts = 0;

				int monstZ;
				do {

					monstZ = rn2(NUMMONS);
					wereformattempts++;

				} while (!(is_were(&mons[monstZ])) || (mons[monstZ].mlet == S_HUMAN) && wereformattempts < 50000);

				if (is_were(&mons[monstZ]) && !(mons[monstZ].mlet == S_HUMAN) ) {
					u.wormpolymorph = monstZ;
					polyself(FALSE);
				}

			}

			/* delayed change may not be valid anymore */
			if ((change == 1 && !Polymorph) ||
			    (change == 2 && u.ulycn == NON_PM))
			    change = 0;
			if(Polymorph && (ishaxor ? !rn2(500) : !rn2(1000)) )
			    change = 1;
	/* let's allow the moulds to stop sucking so much. Make them polymorph more often. --Amy */
			else if(Polymorph && !rn2(200) && !Upolyd && (Race_if(PM_MOULD) || Race_if(PM_DEATHMOLD)) )
			    change = 1;
			else if (u.ulycn >= LOW_PM && !Upolyd &&
				 !rn2(1200 - (200 * night())))
			    change = 2;
			if (change && !Unchanging) {
			    if (multi >= 0) {
				if (occupation)
				    stop_occupation();
				else
				    nomul(0, 0, FALSE);
				if (change == 1) polyself(FALSE);
				else you_were();
				change = 0;
			    }
			}
		}	/* !u.uinvulnerable */

		    if(Searching && multi >= 0 && (!Role_if(PM_CAMPERSTRIKER) || !rn2(3) ) ) (void) dosearch0(1);
		    dosounds();
		    do_storms();
		    gethungry();
		    age_spells();
		    exerchk();
		    invault();
		    if (u.uhave.amulet) amulet();
		if (!rn2(40+(int)(ACURR(A_DEX)*3))) u_wipe_engr(rnd(3));
		    if ((u.uevent.udemigod && (u.amuletcompletelyimbued || !rn2(10))) && !u.uinvulnerable) {
			if (u.udg_cnt) u.udg_cnt--;
			if (u.udg_cnt < 0) u.udg_cnt = 0; /* fail safe by Amy */
			if (!u.udg_cnt) {
			    intervene();
			    u.udg_cnt = rn1(200, 50);
			}
		    }
		    restore_attrib();

		    /* underwater and waterlevel vision are done here */
		    if (Is_waterlevel(&u.uz) && !(tech_inuse(T_SILENT_OCEAN)))
			movebubbles();
		    else if (Underwater)
			under_water(0);
		    /* vision while buried done here */
		    else if (u.uburied) under_ground(0);

		    /* when immobile, count is in turns */
		    if(multi < 0) {
			if (++multi == 0) {	/* finished yet? */
			    unmul((char *)0);
			    /* if unmul caused a level change, take it now */
			    if (u.utotype) deferred_goto();
			}
		    }
		}
	    } while (youmonst.movement<NORMAL_SPEED); /* hero can't move loop */

	    /******************************************/
	    /* once-per-hero-took-time things go here */
	    /******************************************/


	} /* actual time passed */

	/****************************************/
	/* once-per-player-input things go here */
	/****************************************/

	if (u.shiftingsandsinking && !(is_shiftingsand(u.ux, u.uy))) {
		u.shiftingsandsinking = 0;
		You("escaped the shifting sand.");
		if (Hallucination) pline("The Grim Reaper was waiting to take you away, too.");
	}

	if (LatencyBugEffect || u.uprops[LATENCY_BUG].extrinsic || have_latencystone()) {

		int lagamount = rno(10);
		if (!rn2(10)) lagamount += rnd(10);
		if (!rn2(100)) lagamount += rnz(15);
		if (!rn2(3)) {
			lagamount = rn2(9) ? 0 : 1;
		}
		while (lagamount > 0) {
			delay_output();
			lagamount--;
		}
	}

	if (SpellColorRed && !rn2(10)) {
		pline(generate_garbage_string());
	}

	if (uarmh && uarmh->oartifact == ART_UBB_RUPTURE && !rn2(10)) {
		pline(generate_garbage_string());
	}

	if (EvencoreEffect || u.uprops[EVC_EFFECT].extrinsic || have_evcstone()) {

		int x, y;

		x = rn1(COLNO-3,2);
		y = rn2(ROWNO);

		if (isok(x, y)) map_invisible(x, y);

	}

	if (isamnesiac || Map_amnesia) {
		register int zx, zy;
		for(zx = 0; zx < COLNO; zx++) for(zy = 0; zy < ROWNO; zy++) {
			/* Zonk all memory of this location. */
			levl[zx][zy].seenv = 0;
			levl[zx][zy].waslit = 0;
			clear_memory_glyph(zx, zy, S_stone);
		}

		if (!rn2(20)) docrt();
		vision_recalc(0);

	}

	if (RngeTrapAlert && t_at(u.ux, u.uy)) {
		pline("Alert! You are standing on a trap!");
	}

	if (uarmg && t_at(u.ux, u.uy) && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "sensor gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "sensornyye perchatki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "tayinlangan qurilmani qo'lqop") ) ) {
		pline("Alert! You are standing on a trap!");
	}

	if (u.burrowed) {
		if (!u.utrap || !u.utraptype || (u.utraptype != TT_INFLOOR)) {
			u.burrowed = 0;
			You("are no longer burrowed.");
		}
	}

	if (u.enchantspell) {
		if (!uwep) {
			u.enchantspell = 0;
			You("are no longer wielding a weapon, and therefore the enchantment fades.");
		}
	}

	if ((BankTrapEffect || (uleft && uleft->oartifact == ART_ARABELLA_S_RESIST_COLD) || (uright && uright->oartifact == ART_ARABELLA_S_RESIST_COLD) || (uamul && uamul->oartifact == ART_LOW_ZERO_NUMBER) || (uamul && uamul->oartifact == ART_ARABELLA_S_PRECIOUS_GADGET) || u.uprops[BANKBUG].extrinsic || have_bankstone()) && u.ugold) {

		if (!u.bankcashlimit) u.bankcashlimit = rnz(1000 * (monster_difficulty() + 1));

		u.bankcashamount += u.ugold;
		u.ugold = 0;
		Your("money was stored, thanks.");
		if (u.bankcashamount > u.bankcashlimit) {
			(void) makemon(&mons[PM_ARABELLA_THE_MONEY_THIEF], 0, 0, NO_MM_FLAGS);
		}
	}

	/* copy undead warning code over from attrib.c - this needs to update in real time due to items --Amy */
	HWarn_of_mon = (Undead_warning);
	if (Undead_warning) {
	    flags.warntype |= M2_UNDEAD;
	}
	else {
	    flags.warntype &= ~M2_UNDEAD;
	}

	find_ac();
	see_monsters();
	if(!flags.mv || Blind) {
	    /* redo monsters if hallu or wearing a helm of telepathy */
	    if (Hallucination) {	/* update screen randomly */
		see_objects();
		see_traps();
		if (u.uswallow) swallowed(0);
	    }

	    if (vision_full_recalc) vision_recalc(0);	/* vision! */
	}

#ifdef REALTIME_ON_BOTL
        if(iflags.showrealtime) {
            /* Update the bottom line if the number of minutes has
             * changed */
            if(get_realtime() / 60 != realtime_data.last_displayed_time / 60)
                flags.botl = 1;
        }
#endif
  
	if(flags.botl || flags.botlx) bot();

	flags.move = 1;

	if(multi >= 0 && occupation) {
#if defined(MICRO) || defined(WIN32)
	    abort_lev = 0;
	    if (kbhit()) {
		if ((ch = Getchar()) == ABORT)
		    abort_lev++;
		else
		    pushch(ch);
	    }
	    if (!abort_lev && (*occupation)() == 0)
#else
	    if ((*occupation)() == 0)
#endif
		occupation = 0;
	    if(
#if defined(MICRO) || defined(WIN32)
		   abort_lev ||
#endif
		   monster_nearby()) {
		stop_occupation();
		reset_eat();
	    }
#if defined(MICRO) || defined(WIN32)
	    if (!(++occtime % 7))
		display_nhwindow(WIN_MAP, FALSE);
#endif
	    continue;
	}

#ifdef WIZARD
	if (iflags.sanity_check)
	    sanity_check();
#elif defined(OBJ_SANITY)
	if (iflags.sanity_check)
	    obj_sanity_check();
#endif

#ifdef CLIPPING
	/* just before rhack */
	cliparound(u.ux, u.uy);
#endif

	u.umoved = FALSE;

   if (u.hangupcheat && !multi) u.hangupcheat = 0;

	if (multi > 0) {
	    lookaround();
	    if (!multi) {
		/* lookaround may clear multi */
		flags.move = 0;
		if (flags.time) flags.botl = 1;
		continue;
	    }
	    if (flags.mv) {
		if(multi < COLNO && !--multi)
		    flags.travel = iflags.travel1 = flags.mv = flags.run = 0;
		domove();
	    } else {
		--multi;
		rhack(save_cm);
	    }
	} else if (multi == 0) {
#ifdef MAIL
	    ckmailstatus();
#endif
	    rhack((char *)0);
	}
	if (u.utotype)		/* change dungeon level */
	    deferred_goto();	/* after rhack() */
	/* !flags.move here: multiple movement command stopped */
	else if (flags.time && (!flags.move || !flags.mv))
	    flags.botl = 1;

	if (vision_full_recalc) vision_recalc(0);	/* vision! */
	/* when running in non-tport mode, this gets done through domove() */
	if ((!flags.run || iflags.runmode == RUN_TPORT) &&
		(multi && (!flags.travel ? !(multi % 7) : !(moves % 7L)))) {
	    if (flags.time && flags.run) flags.botl = 1;
	    display_nhwindow(WIN_MAP, FALSE);
	}

	/* Autosave option by Amy. While this does not actually prevent phantom crash bugs and similar crap,
	 * it at least means you'll get thrown back at most 100 turns if it does crash. */
	/* edit: set to prime numbers so you'll still get the autosave while pulling an iron ball et. al. */
#ifdef INSURANCE
	if (flags.xtimed_autosave && (moves > 1) && (moves % 19 == 0) ) save_currentstate();
	else if (flags.etimed_autosave && (moves > 1) && (moves % 97 == 0) ) save_currentstate();
#endif

	if (u.riennevaplus) { /* delayed paralysis --Amy */

		nomul(-rnd(u.riennevaplus), "nothing went anymore", FALSE);
		u.riennevaplus = 0;

	}

	if (u.banishmentbeam) { /* uh-oh... something zapped you with a wand of banishment */
		/* this replaces the code in muse.c that always caused segfaults --Amy */

		make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

		/* failsafes in case the player somehow manages to quickly snatch the amulet or something... */
		if (u.uevent.udemigod || u.uhave.amulet || (uarm && uarm->oartifact == ART_CHECK_YOUR_ESCAPES) || NoReturnEffect || u.uprops[NORETURN].extrinsic || have_noreturnstone() || (u.usteed && mon_has_amulet(u.usteed))) {
			You("shudder for a moment."); (void) safe_teleds(FALSE); u.banishmentbeam = 0; break;
		}

		if (flags.lostsoul || flags.uberlostsoul || (flags.wonderland && !(u.wonderlandescape)) || u.uprops[STORM_HELM].extrinsic || In_bellcaves(&u.uz) || In_subquest(&u.uz) || In_voiddungeon(&u.uz) || In_netherrealm(&u.uz)) { 
			pline("Somehow, the banishment beam doesn't do anything."); u.banishmentbeam = 0; break;
		}

		if (rn2(2)) {(void) safe_teleds(FALSE); goto_level(&medusa_level, TRUE, FALSE, FALSE); }
		else { (void) safe_teleds(FALSE); goto_level(&portal_level, TRUE, FALSE, FALSE); }
		u.banishmentbeam = 0; /* player got warped, now clear the flag even if it crashes afterwards */

		register int newlev = rnd(99);
		d_level newlevel;
		get_level(&newlevel, newlev);
		goto_level(&newlevel, TRUE, FALSE, FALSE);
	}

	if (u.levelporting) { /* something attacked you with nexus or weeping */

		if (!u.uevent.udemigod && !(flags.lostsoul || flags.uberlostsoul || (flags.wonderland && !(u.wonderlandescape)) || u.uprops[STORM_HELM].extrinsic || In_bellcaves(&u.uz) || In_subquest(&u.uz) || In_voiddungeon(&u.uz) || In_netherrealm(&u.uz)) ) {
			make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */
			level_tele(); /* will take care of u.uhave.amulet and similar stuff --Amy */
		}
		u.levelporting = 0; /* player got teleported, now clear the flag even if it crashes afterwards */
	}

    }
}


#endif /* OVL0 */
#ifdef OVL1

void
stop_occupation()
{

	if (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "quicktravel cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "bystryy plashch puteshestviya") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "tez safar plash") ) ) return;

	if (uarmf && uarmf->oartifact == ART_TOO_FAST__TOO_FURIOUS) return;

	if (RngePermanentOccupation) return;

	if(occupation) {
		if (!maybe_finished_meal(TRUE))
		    You("stop %s.", occtxt);
		occupation = 0;
		flags.botl = 1; /* in case u.uhs changed */
/* fainting stops your occupation, there's no reason to sync.
		sync_hunger();
*/
		forcenomul(0, 0);
		pushch(0);
	}
}

#endif /* OVL1 */
#ifdef OVLB

void
display_gamewindows()
{
    WIN_MESSAGE = create_nhwindow(NHW_MESSAGE);
    WIN_STATUS = create_nhwindow(NHW_STATUS);
    WIN_MAP = create_nhwindow(NHW_MAP);
    WIN_INVEN = create_nhwindow(NHW_MENU);

#ifdef MAC
    /*
     * This _is_ the right place for this - maybe we will
     * have to split display_gamewindows into create_gamewindows
     * and show_gamewindows to get rid of this ifdef...
     */
	if ( ! strcmp ( windowprocs . name , "mac" ) ) {
	    SanePositions ( ) ;
	}
#endif

    /*
     * The mac port is not DEPENDENT on the order of these
     * displays, but it looks a lot better this way...
     */
    display_nhwindow(WIN_STATUS, FALSE);
    display_nhwindow(WIN_MESSAGE, FALSE);
    clear_glyph_buffer();
    display_nhwindow(WIN_MAP, FALSE);
}

void
newgame()
{
	int i;

#ifdef MFLOPPY
	gameDiskPrompt();
#endif

	flags.ident = 1;

	for (i = 0; i < NUMMONS; i++)
		mvitals[i].mvflags = mons[i].geno & G_NOCORPSE;

	init_objects();		/* must be before u_init() */

	randommaterials();	/* only done here - do not call this during a running game! --Amy */

	flags.pantheon = -1;	/* role_init() will reset this */
	role_init();		/* must be before init_dungeons(), u_init(),
				 * and init_artifacts() */

	init_dungeons();	/* must be before u_init() to avoid rndmonst()
				 * creating odd monsters for any tins and eggs
				 * in hero's initial inventory */

	init_artifacts();	/* before u_init() in case $WIZKIT specifies
				 * any artifacts */
	u_init();
	init_artifacts1();	/* must be after u_init() */

#ifndef NO_SIGNAL
	(void) signal(SIGINT, (SIG_RET_TYPE) done1);
#endif
#ifdef NEWS
	if(iflags.news) display_file_area(NEWS_AREA, NEWS, FALSE);
#endif

	load_qtlist();	/* load up the quest text info */
/*	quest_init();*/	/* Now part of role_init() */

	mklev();
	u_on_upstairs();
	vision_reset();		/* set up internals for level (after mklev) */
	check_special_room(FALSE);

	flags.botlx = 1;

	/* Move the monster from under you or else
	 * makedog() will fail when it calls makemon().
	 *			- ucsfcgl!kneller
	 */

	if(MON_AT(u.ux, u.uy)) mnexto(m_at(u.ux, u.uy));
	(void) makedog();

	docrt();

	/* Yes I know, nymphs usually remove iron balls. The playable nymph race doesn't, since I don't want them
	   to be totally overpowered - I mean, they start with teleportitis *and* can get teleport control! --Amy */
       if (Role_if(PM_CONVICT) || Role_if(PM_MURDERER) || Race_if(PM_NYMPH) ) {
              setworn(mkobj(CHAIN_CLASS, TRUE), W_CHAIN);
              setworn(mkobj(BALL_CLASS, TRUE), W_BALL);
              /*uball->spe = 1;*/
              placebc();
              newsym(u.ux,u.uy);
       }

	if (flags.legacy) {
		flush_screen(1);
        if (Role_if(PM_CONVICT)) {
		    com_pager(199);
        } else if (Role_if(PM_GANG_SCHOLAR)) {
		    com_pager(198);
        } else {
		    com_pager(1);
        }
	}
#ifdef INSURANCE
	save_currentstate();
#endif
	program_state.something_worth_saving++;	/* useful data now exists */

#if defined(RECORD_REALTIME) || defined(REALTIME_ON_BOTL)

        /* Start the timer here */
        realtime_data.realtime = (time_t)0L;

#if defined(BSD) && !defined(POSIX_TYPES)
        (void) time((long *)&realtime_data.restoretime);
#else
        (void) time(&realtime_data.restoretime);
#endif

#endif /* RECORD_REALTIME || REALTIME_ON_BOTL */

	/* Success! */
	welcome(TRUE);

#ifdef WHEREIS_FILE
	touch_whereis();
#endif

	return;
}

/* show "welcome [back] to nethack" message at program startup */
void
welcome(new_game)
boolean new_game;	/* false => restoring an old game */
{
    char buf[BUFSZ];
    char xtrabuf[BUFSZ];
    boolean currentgend = Upolyd ? u.mfemale : flags.female;

    /*
     * The "welcome back" message always describes your innate form
     * even when polymorphed or wearing a helm of opposite alignment.
     * Alignment is shown unconditionally for new games; for restores
     * it's only shown if it has changed from its original value.
     * Sex is shown for new games except when it is redundant; for
     * restores it's only shown if different from its original value.
     */
    *buf = '\0';

	if (FuckedInfoBug || u.uprops[FUCKED_INFO_BUG].extrinsic || have_infofuckstone()) {
		pline("You've forgotten who you are, but you are back.");
		return;
	}

    if (new_game || u.ualignbase[A_ORIGINAL] != u.ualignbase[A_CURRENT])
	sprintf(eos(buf), " %s", align_str(u.ualignbase[A_ORIGINAL]));
    if (!urole.name.f &&
	    (new_game ? (urole.allow & ROLE_GENDMASK) == (ROLE_MALE|ROLE_FEMALE) :
	     currentgend != flags.initgend))
	sprintf(eos(buf), " %s", genders[currentgend].adj);

    *xtrabuf = '\0';
	if (flags.hybridangbander) sprintf(eos(xtrabuf), "angbander ");
	if (flags.hybridaquarian) sprintf(eos(xtrabuf), "aquarian ");
	if (flags.hybridcurser) sprintf(eos(xtrabuf), "curser ");
	if (flags.hybridhaxor) sprintf(eos(xtrabuf), "haxor ");
	if (flags.hybridhomicider) sprintf(eos(xtrabuf), "homicider ");
	if (flags.hybridsuxxor) sprintf(eos(xtrabuf), "suxxor ");
	if (flags.hybridwarper) sprintf(eos(xtrabuf), "warper ");
	if (flags.hybridrandomizer) sprintf(eos(xtrabuf), "randomizer ");
	if (flags.hybridnullrace) sprintf(eos(xtrabuf), "null ");
	if (flags.hybridmazewalker) sprintf(eos(xtrabuf), "mazewalker ");
	if (flags.hybridsoviet) sprintf(eos(xtrabuf), "soviet ");
	if (flags.hybridxrace) sprintf(eos(xtrabuf), "x-race ");
	if (flags.hybridheretic) sprintf(eos(xtrabuf), "heretic ");
	if (flags.hybridsokosolver) sprintf(eos(xtrabuf), "sokosolver ");
	if (flags.hybridspecialist) sprintf(eos(xtrabuf), "specialist ");
	if (flags.hybridamerican) sprintf(eos(xtrabuf), "american ");
	if (flags.hybridminimalist) sprintf(eos(xtrabuf), "minimalist ");
	if (flags.hybridnastinator) sprintf(eos(xtrabuf), "nastinator ");
	if (flags.hybridrougelike) sprintf(eos(xtrabuf), "rougelike ");
	if (flags.hybridsegfaulter) sprintf(eos(xtrabuf), "segfaulter ");
	if (flags.hybridironman) sprintf(eos(xtrabuf), "ironman ");
	if (flags.hybridamnesiac) sprintf(eos(xtrabuf), "amnesiac ");
	if (flags.hybridproblematic) sprintf(eos(xtrabuf), "problematic ");
	if (flags.hybridwindinhabitant) sprintf(eos(xtrabuf), "windinhabitant ");
	if (flags.hybridaggravator) sprintf(eos(xtrabuf), "aggravator ");
	if (flags.hybridevilvariant) sprintf(eos(xtrabuf), "evilvariant ");
	if (flags.hybridlevelscaler) sprintf(eos(xtrabuf), "levelscaler ");

	if (new_game) { /* for recursion trap */
		ustartrace = urace;
		ustartrole = urole;
		flags.startingrole = flags.initrole;
		flags.startingrace = flags.initrace;

	}

	if (isrougelike) assign_rogue_graphics(TRUE);

#if 0
    pline(new_game ? "%s %s, welcome to NetHack!  You are a%s %s%s %s."
		   : "%s %s, the%s %s %s, welcome back to NetHack!",
	  Hello((struct monst *) 0), plname, buf, xtrabuf, urace.adj,
	  (currentgend && urole.name.f) ? urole.name.f : urole.name.m);
#endif
    if (new_game) pline("%s %s, welcome to %s!  You are a%s %s%s %s.",
	  Hello((struct monst *) 0), plname, issoviet ? "SlashTHEM Extended" : DEF_GAME_NAME, buf, xtrabuf, urace.adj,
	  (currentgend && urole.name.f) ? urole.name.f : urole.name.m);
    else pline("%s %s, the%s %s%s %s, welcome back to %s!",
	  Hello((struct monst *) 0), plname, buf, xtrabuf, urace.adj,
	  (currentgend && urole.name.f) ? urole.name.f : urole.name.m, 
	  issoviet ? "SlashTHEM Extended" : DEF_GAME_NAME);

	if (issoviet) pline("Tip bloka l'da zhelayet udachi vam... on on on.");

	/*if (Race_if(PM_MISSINGNO)) pline("Caution! The missingno might still be an unstable race. I tried to fix all the crashes but some may still remain. You can disable the missing_safety option if you deliberately want the game to be crashy. --Amy");*/

	if (issegfaulter) pline("ATTENTION!!! You are playing the segfaulter race, which can cause certain actions to produce a fake segfault panic that erases your character. If you intend to go on playing, you do so at your own peril, and getting an ascension will basically be impossible! Consider using #quit if you rather want to play a balanced game. (You can make fun of me now, because I know you will if you're from /rlg/. --Amy)");

#ifdef PUBLIC_SERVER

#ifdef PHANTOM_CRASH_BUG
	if (new_game) pline("Welcome to SLASH'EM Extended! For game discussion, bug reports etc. join the #slashemextended or #em.slashem.me IRC channel on Freenode. :-) --Amy");
#else
	if (new_game) pline("You are playing SLASH'EM Extended on a public server. For game discussion, bug reports etc. join the #em.slashem.me IRC channel on Freenode. You should absolutely do that, unless you want to figure out this complex game on your own. Amy and other players will be glad to give you advice!");

#endif /* PHANTOM_CRASH_BUG */

#endif /* PUBLIC_SERVER */

	if (issoviet) { /* horrible kludge for spell.c, which unfortunately makes wishing for books *very* difficult... --Amy */
	obj_descr[SPE_FORCE_BOLT].oc_name = "sila zadvizhka";
	obj_descr[SPE_CREATE_MONSTER].oc_name = "vyzov monstra";
	obj_descr[SPE_DRAIN_LIFE].oc_name = "pokhishcheniye zhizni";
	obj_descr[SPE_COMMAND_UNDEAD].oc_name = "komanda nezhit'";
	obj_descr[SPE_SUMMON_UNDEAD].oc_name = "prizvat' nezhit'";
	obj_descr[SPE_DISINTEGRATION].oc_name = "raspad";
	obj_descr[SPE_STONE_TO_FLESH].oc_name = "kamen' ploti";
	obj_descr[SPE_HEALING].oc_name = "istseleniye";
	obj_descr[SPE_CURE_BLINDNESS].oc_name = "lecheniye slepoty";
	obj_descr[SPE_CURE_NUMBNESS].oc_name = "lecheniye onemeniya";
	obj_descr[SPE_BLIND_SELF].oc_name = "slepoy samoupravleniya";
	obj_descr[SPE_CURE_SICKNESS].oc_name = "lecheniye bolezni";
	obj_descr[SPE_CURE_HALLUCINATION].oc_name = "lecheniye gallyutsinatsii";
	obj_descr[SPE_CURE_CONFUSION].oc_name = "lecheniye putanitsa";
	obj_descr[SPE_CURE_BURN].oc_name = "lecheniye ozhogov";
	obj_descr[SPE_CONFUSE_SELF].oc_name = "zaputat' sebya";
	obj_descr[SPE_CURE_STUN].oc_name = "lecheniye oglushayet";
	obj_descr[SPE_STUN_SELF].oc_name = "oglushayet samoupravleniya";
	obj_descr[SPE_EXTRA_HEALING].oc_name = "dopolnitel'naya istseleniye";
	obj_descr[SPE_FULL_HEALING].oc_name = "polnogo zazhivleniya";
	obj_descr[SPE_RESTORE_ABILITY].oc_name = "vosstanovit' sposobnost'";
	obj_descr[SPE_BANISHING_FEAR].oc_name = "izgonyaya strakh";
	obj_descr[SPE_CURE_FREEZE].oc_name = "lecheniye zamorazhivaniya";
	obj_descr[SPE_CREATE_FAMILIAR].oc_name = "sozdat' znakomy";
	obj_descr[SPE_LIGHT].oc_name = "svet";
	obj_descr[SPE_DARKNESS].oc_name = "t'ma";
	obj_descr[SPE_DETECT_MONSTERS].oc_name = "obnaruzhit' monstrov";
	obj_descr[SPE_DETECT_FOOD].oc_name = "obnaruzhit' pishchu";
	obj_descr[SPE_DISSOLVE_FOOD].oc_name = "rastvorit' pishchu";
	obj_descr[SPE_DETECT_ARMOR_ENCHANTMENT].oc_name = "obnaruzhit' broni chary";
	obj_descr[SPE_CLAIRVOYANCE].oc_name = "yasnovideniye";
	obj_descr[SPE_DETECT_UNSEEN].oc_name = "obnaruzhit' nevidimyy";
	obj_descr[SPE_IDENTIFY].oc_name = "identifitsirovat'";
	obj_descr[SPE_DETECT_TREASURE].oc_name = "obnaruzhit' sokrovishcha";
	obj_descr[SPE_MAGIC_MAPPING].oc_name = "magiya otobrazheniye";
	obj_descr[SPE_ENTRAPPING].oc_name = "obnaruzhit' lovushki";
	obj_descr[SPE_FINGER].oc_name = "palets";
	obj_descr[SPE_CHEMISTRY].oc_name = "khimiya";
	obj_descr[SPE_DETECT_FOOT].oc_name = "obnaruzhit' nogu";
	obj_descr[SPE_FORBIDDEN_KNOWLEDGE].oc_name = "zapreshcheno znaniya";
	obj_descr[SPE_CONFUSE_MONSTER].oc_name = "putayut chudovishche";
	obj_descr[SPE_SLOW_MONSTER].oc_name = "medlenno monstr";
	obj_descr[SPE_CAUSE_FEAR].oc_name = "strakh";
	obj_descr[SPE_CHARM_MONSTER].oc_name = "ukroshcheniye";
	obj_descr[SPE_ENCHANT_WEAPON].oc_name = "chary dlya oruzhiya";
	obj_descr[SPE_ENCHANT_ARMOR].oc_name = "ocharovat' bronyu";
	obj_descr[SPE_CHARGING].oc_name = "zaryadka";
	obj_descr[SPE_GENOCIDE].oc_name = "iskoreneniye";
	obj_descr[SPE_PROTECTION].oc_name = "zashchita";
	obj_descr[SPE_RESIST_POISON].oc_name = "soprotivleniye otravleniyu";
	obj_descr[SPE_RESIST_SLEEP].oc_name = "protivostoyat' son";
	obj_descr[SPE_ENDURE_COLD].oc_name = "terpet' kholod";
	obj_descr[SPE_ENDURE_HEAT].oc_name = "terpet' zharu";
	obj_descr[SPE_INSULATE].oc_name = "izolirovat'";
	obj_descr[SPE_REMOVE_CURSE].oc_name = "udalit' proklyatiye";
	obj_descr[SPE_REMOVE_BLESSING].oc_name = "udalit' blagosloveniye";
	obj_descr[SPE_TURN_UNDEAD].oc_name = "povorot nezhit'";
	obj_descr[SPE_ANTI_DISINTEGRATION].oc_name = "ni raspad";
	obj_descr[SPE_BOTOX_RESIST].oc_name = "botoks soprotivlyat'sya";
	obj_descr[SPE_ACIDSHIELD].oc_name = "kisloty shchit";
	obj_descr[SPE_GODMODE].oc_name = "rezhim boga";
	obj_descr[SPE_RESIST_PETRIFICATION].oc_name = "protivostoyat' okameneniya";
	obj_descr[SPE_JUMPING].oc_name = "pryzhki";
	obj_descr[SPE_HASTE_SELF].oc_name = "pospeshnost' samoupravleniya";
	obj_descr[SPE_ENLIGHTEN].oc_name = "prosvetit'";
	obj_descr[SPE_INVISIBILITY].oc_name = "nevidimost'";
	obj_descr[SPE_AGGRAVATE_MONSTER].oc_name = "usugubit' vragov";
	obj_descr[SPE_LEVITATION].oc_name = "svobodnoye pareniye";
	obj_descr[SPE_TELEPORT_AWAY].oc_name = "teleportirovat'sya";
	obj_descr[SPE_PASSWALL].oc_name = "prokhod stena";
	obj_descr[SPE_POLYMORPH].oc_name = "prevrashchat'";
	obj_descr[SPE_MUTATION].oc_name = "mutatsiya";
	obj_descr[SPE_LEVELPORT].oc_name = "uroven' siyayushcheye";
	obj_descr[SPE_KNOCK].oc_name = "stuchat'";
	obj_descr[SPE_FLAME_SPHERE].oc_name = "sfera plameni";
	obj_descr[SPE_FREEZE_SPHERE].oc_name = "zamorozit' sfera";
	obj_descr[SPE_SHOCKING_SPHERE].oc_name = "shokiruyet sfera";
	obj_descr[SPE_ACID_SPHERE].oc_name = "kislota sfera";
	obj_descr[SPE_WIZARD_LOCK].oc_name = "master blokirovki";
	obj_descr[SPE_DIG].oc_name = "kopat'";
	obj_descr[SPE_CANCELLATION].oc_name = "annulirovaniye";
	obj_descr[SPE_REFLECTION].oc_name = "otrazheniye";
	obj_descr[SPE_PARALYSIS].oc_name = "paralich";
	obj_descr[SPE_REPAIR_ARMOR].oc_name = "remont broni";
	obj_descr[SPE_CORRODE_METAL].oc_name = "korroziyu metalla";
	obj_descr[SPE_PETRIFY].oc_name = "stolbenet'";
	obj_descr[SPE_MAGIC_MISSILE].oc_name = "magicheskaya raketa";
	obj_descr[SPE_FIREBALL].oc_name = "ognennyy shar";
	obj_descr[SPE_CONE_OF_COLD].oc_name = "konus kholoda";
	obj_descr[SPE_SLEEP].oc_name = "son";
	obj_descr[SPE_FINGER_OF_DEATH].oc_name = "luch smerti";
	obj_descr[SPE_LIGHTNING].oc_name = "molniya";
	obj_descr[SPE_POISON_BLAST].oc_name = "yad vzryv";
	obj_descr[SPE_ACID_STREAM].oc_name = "potok kisloty";
	obj_descr[SPE_SOLAR_BEAM].oc_name = "solnechnaya luch";
	obj_descr[SPE_BLANK_PAPER].oc_name = "chistyy list bumagi";
	obj_descr[SPE_STINKING_CLOUD].oc_name = "vonyuchiy oblako";
	obj_descr[SPE_TIME_STOP].oc_name = "vremya ostanovki";
	obj_descr[SPE_MAP_LEVEL].oc_name = "uroven' otobrazheniye";
	obj_descr[SPE_GAIN_LEVEL].oc_name = "uroven' povyshen";
	obj_descr[SPE_AMNESIA].oc_name = "poterya pamyati";
	obj_descr[SPE_REPAIR_WEAPON].oc_name = "remont oruzhiya";
	obj_descr[SPE_KNOW_ENCHANTMENT].oc_name = "znayete chary";
	obj_descr[SPE_MAGICTORCH].oc_name = "magiya fakel";
	obj_descr[SPE_DISPLACEMENT].oc_name = "smeshcheniye";
	obj_descr[SPE_MASS_HEALING].oc_name = "massa istseleniye";
	obj_descr[SPE_TIME_SHIFT].oc_name = "sdvig vremeni";
	obj_descr[SPE_ALTER_REALITY].oc_name = "izmenit' real'nost'";
	obj_descr[SPE_DISINTEGRATION_BEAM].oc_name = "raspad puchka";
	obj_descr[SPE_FLYING].oc_name = "letayushchiy";
	obj_descr[SPE_CHROMATIC_BEAM].oc_name = "khromaticheskoy sveta";
	obj_descr[SPE_FUMBLING].oc_name = "nelovkiy";
	obj_descr[SPE_MAKE_VISIBLE].oc_name = "sdelat' vidimym";
	obj_descr[SPE_WARPING].oc_name = "iskrivleniye";
	obj_descr[SPE_TRAP_CREATION].oc_name = "sozdayut lovushki";
	obj_descr[SPE_STUN_MONSTER].oc_name = "oglushayet monstr";
	obj_descr[SPE_CURSE_ITEMS].oc_name = "proklyatiye predmety";
	obj_descr[SPE_CHARACTER_RECURSION].oc_name = "kharakter rekursii";
	obj_descr[SPE_CLONE_MONSTER].oc_name = "umnozhit' monstr";
	obj_descr[SPE_DESTROY_ARMOR].oc_name = "unichtozhit' dospekhi";
	obj_descr[SPE_INERTIA].oc_name = "inertsiya";
	obj_descr[SPE_TIME].oc_name = "vremya";
	obj_descr[SPE_PSYBEAM].oc_name = "psikho luch";
	obj_descr[SPE_HYPER_BEAM].oc_name = "giper luch";
	obj_descr[SPE_FIRE_BOLT].oc_name = "ogon' snaryadom";
	obj_descr[SPE_INFERNO].oc_name = "ad";
	obj_descr[SPE_ICE_BEAM].oc_name = "ledyanoy luch";
	obj_descr[SPE_THUNDER].oc_name = "grom";
	obj_descr[SPE_SLUDGE].oc_name = "otstoy";
	obj_descr[SPE_TOXIC].oc_name = "toksichnyy";
	obj_descr[SPE_NETHER_BEAM].oc_name = "luch pustoty";
	obj_descr[SPE_AURORA_BEAM].oc_name = "polyarnyye siyaniya lucha";
	obj_descr[SPE_GRAVITY_BEAM].oc_name = "gravitatsionnyy luch";
	obj_descr[SPE_CHLOROFORM].oc_name = "khloroform";
	obj_descr[SPE_DREAM_EATER].oc_name = "pozhiratel' snov";
	obj_descr[SPE_BUBBLEBEAM].oc_name = "puzyr' luch";
	obj_descr[SPE_GOOD_NIGHT].oc_name = "dobroy nochi";
	obj_descr[SPE_FIXING].oc_name = "fiksatsiya";

	obj_descr[SPE_CHAOS_TERRAIN].oc_name = "khaos mestnosti";
	obj_descr[SPE_RANDOM_SPEED].oc_name = "sluchaynaya skorost'";
	obj_descr[SPE_VANISHING].oc_name = "ischezayushchiy";
	obj_descr[SPE_WISHING].oc_name = "zhelayushchikh";
	obj_descr[SPE_ACQUIREMENT].oc_name = "priobreteniye";
	obj_descr[SPE_CHAOS_BOLT].oc_name = "boltom khaos";
	obj_descr[SPE_HELLISH_BOLT].oc_name = "adskaya boltom";
	obj_descr[SPE_EARTHQUAKE].oc_name = "zemletryaseniye";
	obj_descr[SPE_LYCANTHROPY].oc_name = "likantropiyu";
	obj_descr[SPE_BUC_RANDOMIZATION].oc_name = "buc randomizatsii";
	obj_descr[SPE_LOCK_MANIPULATION].oc_name = "manipulyatsiya zamok";
	obj_descr[SPE_POLYFORM].oc_name = "poliform";
	obj_descr[SPE_MESSAGE].oc_name = "soobshcheniye";
	obj_descr[SPE_RUMOR].oc_name = "slukh";
	obj_descr[SPE_CURE_RANDOM_STATUS].oc_name = "vylechit' sluchaynyy status";
	obj_descr[SPE_RESIST_RANDOM_ELEMENT].oc_name = "soprotivlyayutsya sluchaynyy element";
	obj_descr[SPE_RUSSIAN_ROULETTE].oc_name = "russkaya ruletka";
	obj_descr[SPE_POSSESSION].oc_name = "vladeniye";
	obj_descr[SPE_TOTEM_SUMMONING].oc_name = "totem vyzova";
	obj_descr[SPE_MIMICRY].oc_name = "mimikriya";
	obj_descr[SPE_HORRIFY].oc_name = "shokirovat'";
	obj_descr[SPE_TERROR].oc_name = "uzhas";
	obj_descr[SPE_PHASE_DOOR].oc_name = "faza dveri";
	obj_descr[SPE_TRAP_DISARMING].oc_name = "lovushka snyatiya s okhrany";
	obj_descr[SPE_NEXUS_FIELD].oc_name = "svyazuyushchey pole";
	obj_descr[SPE_COMMAND_DEMON].oc_name = "komanda demonov";
	obj_descr[SPE_FIRE_GOLEM].oc_name = "ogon' golemov";
	obj_descr[SPE_DISRUPTION_SHIELD].oc_name = "razrusheniye shchita";
	obj_descr[SPE_SPELLBINDER].oc_name = "orator, uvlekayushchiy svoyu auditoriyu";
	obj_descr[SPE_TRACKER].oc_name = "treker";
	obj_descr[SPE_INERTIA_CONTROL].oc_name = "kontrol' inertsii";
	obj_descr[SPE_CODE_EDITING].oc_name = "redaktirovaniye koda";
	obj_descr[SPE_FORGOTTEN_SPELL].oc_name = "zabyli zaklinaniye";
	obj_descr[SPE_FLOOD].oc_name = "navodneniye";
	obj_descr[SPE_LAVA].oc_name = "lavovyy";
	obj_descr[SPE_IRON_PRISON].oc_name = "zheleznaya tyur'ma";
	obj_descr[SPE_LOCKOUT].oc_name = "lokaut";
	obj_descr[SPE_CLOUDS].oc_name = "oblaka";
	obj_descr[SPE_ICE].oc_name = "led";
	obj_descr[SPE_GROW_TREES].oc_name = "rastut derev'ya";
	obj_descr[SPE_DRIPPING_TREAD].oc_name = "kapayet protektora";
	obj_descr[SPE_GEOLYSIS].oc_name = "geologicheskoye resheniye";
	obj_descr[SPE_ELEMENTAL_BEAM].oc_name = "elementarnyy puchok";
	obj_descr[SPE_STERILIZE].oc_name = "sterilizovat'";
	obj_descr[SPE_WIND].oc_name = "veter";
	obj_descr[SPE_FIRE].oc_name = "ogon'";
	obj_descr[SPE_ELEMENTAL_MINION].oc_name = "elementnyy min'on";
	obj_descr[SPE_WATER_BOLT].oc_name = "boltov vody";
	obj_descr[SPE_AIR_CURRENT].oc_name = "potok vozdukha";
	obj_descr[SPE_DASHING].oc_name = "likhoy";
	obj_descr[SPE_MELTDOWN].oc_name = "rasplavleniye";
	obj_descr[SPE_POISON_BRAND].oc_name = "yad brend";
	obj_descr[SPE_STEAM_VENOM].oc_name = "para otrava";
	obj_descr[SPE_HOLD_AIR].oc_name = "uderzhivat' vozdukh";
	obj_descr[SPE_SWIMMING].oc_name = "plavaniye";
	obj_descr[SPE_VOLT_ROCK].oc_name = "vol'tovyy rok";
	obj_descr[SPE_WATER_FLAME].oc_name = "plamya vody";
	obj_descr[SPE_AVALANCHE].oc_name = "lavina";
	obj_descr[SPE_MANA_BOLT].oc_name = "boltov many";
	obj_descr[SPE_ENERGY_BOLT].oc_name = "boltov energii";
	obj_descr[SPE_ACID_INGESTION].oc_name = "kislota proglatyvaniye";
	obj_descr[SPE_INDUCE_VOMITING].oc_name = "vyzvat' rvotu";
	obj_descr[SPE_REBOOT].oc_name = "perezagruzhat'";
	obj_descr[SPE_HOLY_SHIELD].oc_name = "svyatoy shchit";

	obj_descr[SPE_FROST].oc_name = "moroz";
	obj_descr[SPE_TRUE_SIGHT].oc_name = "istinnoye zreniye";
	obj_descr[SPE_BERSERK].oc_name = "neistovyy";
	obj_descr[SPE_BLINDING_RAY].oc_name = "oslepitel'nogo lucha";
	obj_descr[SPE_MAGIC_SHIELD].oc_name = "zashchita ot magii";
	obj_descr[SPE_WORLD_FALL].oc_name = "mir osen'yu";
	obj_descr[SPE_ESP].oc_name = "ekstra-sensornoye vospriyatiye";
	obj_descr[SPE_RADAR].oc_name = "radiolokatsionnyy";
	obj_descr[SPE_SEARCHING].oc_name = "poisk";
	obj_descr[SPE_INFRAVISION].oc_name = "infravideniye";
	obj_descr[SPE_STEALTH].oc_name = "khitrost'";
	obj_descr[SPE_CONFLICT].oc_name = "protivorechiye";
	obj_descr[SPE_REGENERATION].oc_name = "vosstanovleniye";
	obj_descr[SPE_FREE_ACTION].oc_name = "soprotivleniye paralich";
	obj_descr[SPE_MULTIBEAM].oc_name = "na neskol'kikh luch";
	obj_descr[SPE_NO_EFFECT].oc_name = "net effekta";
	obj_descr[SPE_SELFDESTRUCT].oc_name = "samorazrusheniye";
	obj_descr[SPE_THUNDER_WAVE].oc_name = "grom volna";
	obj_descr[SPE_BATTERING_RAM].oc_name = "taran";
	obj_descr[SPE_BURROW].oc_name = "nora";
	obj_descr[SPE_GAIN_CORRUPTION].oc_name = "poluchit' korruptsiyu";
	obj_descr[SPE_SWITCHEROO].oc_name = "obmanchiv razvorot";
	obj_descr[SPE_THRONE_GAMBLE].oc_name = "tron avantyura";
	obj_descr[SPE_BACKFIRE].oc_name = "obratnaya vspyshka";
	obj_descr[SPE_DEMEMORIZE].oc_name = "bol'she ne zapominat'";
	obj_descr[SPE_CALL_THE_ELEMENTS].oc_name = "nazyvat' elementy";
	obj_descr[SPE_NATURE_BEAM].oc_name = "priroda sveta";
	obj_descr[SPE_WHISPERS_FROM_BEYOND].oc_name = "shepoty iz-za";
	obj_descr[SPE_STASIS].oc_name = "staz";
	obj_descr[SPE_CRYOGENICS].oc_name = "fizika nizkikh temperatur";
	obj_descr[SPE_REDEMPTION].oc_name = "vykup";
	obj_descr[SPE_HYPERSPACE_SUMMON].oc_name = "giperprostranstvo prizvat'";
	obj_descr[SPE_SATISFY_HUNGER].oc_name = "utolit' golod";
	obj_descr[SPE_RAIN_CLOUD].oc_name = "dozhdevyye oblaka";
	obj_descr[SPE_POWER_FAILURE].oc_name = "sboy pitaniya";
	obj_descr[SPE_VAPORIZE].oc_name = "isparyat'sya";
	obj_descr[SPE_TUNNELIZATION].oc_name = "tunnel'naya";
	obj_descr[SPE_BOMBING].oc_name = "bombardirovka";
	obj_descr[SPE_DRAGON_BLOOD].oc_name = "drakon krovi";
	obj_descr[SPE_ANTI_MAGIC_FIELD].oc_name = "antimagicheskiy pole";
	obj_descr[SPE_ANTI_MAGIC_SHELL].oc_name = "antimagicheskiy obolochki";
	obj_descr[SPE_CURE_WOUNDED_LEGS].oc_name = "vylechit' ranenyye nogi";
	obj_descr[SPE_ANGER_PEACEFUL_MONSTER].oc_name = "gnev mirnyy monstr";
	obj_descr[SPE_UNTAME_MONSTER].oc_name = "dikiy monstr";
	obj_descr[SPE_UNLEVITATE].oc_name = "ostanovit' levitatsiyu";
	obj_descr[SPE_DETECT_WATER].oc_name = "obnaruzheniya vody";
	obj_descr[SPE_APPLY_NAIL_POLISH].oc_name = "primenit' lak dlya nogtey";
	obj_descr[SPE_ENCHANT].oc_name = "ocharovyvat'";
	obj_descr[SPE_DRY_UP_FOUNTAIN].oc_name = "vysykhayut fontan";
	obj_descr[SPE_TAKE_SELFIE].oc_name = "prinyat' selfi";
	obj_descr[SPE_SNIPER_BEAM].oc_name = "snayperskiy luch";
	obj_descr[SPE_CURE_GLIB].oc_name = "vylechit' boykim";
	obj_descr[SPE_CURE_MONSTER].oc_name = "vylechit' monstra";
	obj_descr[SPE_MANA_BATTERY].oc_name = "batareya many";
	obj_descr[SPE_THORNS].oc_name = "shipy";
	obj_descr[SPE_REROLL_ARTIFACT].oc_name = "randomizatsii artefakt";
	obj_descr[SPE_FINAL_EXPLOSION].oc_name = "okonchatel'nyy vzryv";
	obj_descr[SPE_CUTTING].oc_name = "rezka";
	obj_descr[SPE_CURE_DIM].oc_name = "lecheniye tusklost'";

	{

	register int i;
	register const char *s;

	for (i = 0; i < NUM_OBJECTS; i++) {
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "crude dagger")) OBJ_DESCR(objects[i]) = "syroy kinzhal";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "obsidian dagger")) OBJ_DESCR(objects[i]) = "vulkanicheskoye steklo kinzhal";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "runed dagger")) OBJ_DESCR(objects[i]) = "runicheskiy kinzhal";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "black runed dagger")) OBJ_DESCR(objects[i]) = "chernyy runicheskiy kinzhal";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "very sharp tooth")) OBJ_DESCR(objects[i]) = "khishchnik zub";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "double-headed axe")) OBJ_DESCR(objects[i]) = "dvuglavyy topor";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "broad pick")) OBJ_DESCR(objects[i]) = "shirokiy vybor";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "crude short sword")) OBJ_DESCR(objects[i]) = "syroy korotkiy mech";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "broad short sword")) OBJ_DESCR(objects[i]) = "shirokiy korotkiy mech";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "runed short sword")) OBJ_DESCR(objects[i]) = "runicheskiy korotkiy mech";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "obsidian short sword")) OBJ_DESCR(objects[i]) = "vulkanicheskoye steklo korotkiy mech";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "black runed short sword")) OBJ_DESCR(objects[i]) = "chernyy runicheskiy korotkiy mech";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "runic broadsword")) OBJ_DESCR(objects[i]) = "runicheskaya palash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "runed broadsword")) OBJ_DESCR(objects[i]) = "runicheskiy palash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "samurai sword")) OBJ_DESCR(objects[i]) = "yaponskiy mech";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "large obsidian sword")) OBJ_DESCR(objects[i]) = "bol'shoy mech vulkanicheskoye steklo";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "long samurai sword")) OBJ_DESCR(objects[i]) = "yaponskiy mech dolgo";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "curved sword")) OBJ_DESCR(objects[i]) = "krivoy mech";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "bent sword")) OBJ_DESCR(objects[i]) = "sognuty mech";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "thonged club")) OBJ_DESCR(objects[i]) = "luchshe klub";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "heavy club")) OBJ_DESCR(objects[i]) = "tyazhelaya klub";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "staff")) OBJ_DESCR(objects[i]) = "sotrudniki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "vulgar polearm")) OBJ_DESCR(objects[i]) = "vul'garnym drevkom";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "single-edged polearm")) OBJ_DESCR(objects[i]) = "odnolezviynyye drevkom";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "forked polearm")) OBJ_DESCR(objects[i]) = "razdvoyennyy drevkom";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hilted polearm")) OBJ_DESCR(objects[i]) = "rukoyat'yu drevkom";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "long poleaxe")) OBJ_DESCR(objects[i]) = "dolgo sekira";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "pole cleaver")) OBJ_DESCR(objects[i]) = "polyus tesak";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "angled poleaxe")) OBJ_DESCR(objects[i]) = "pod uglom sekira";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "pole sickle")) OBJ_DESCR(objects[i]) = "polyus serp";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "pruning hook")) OBJ_DESCR(objects[i]) = "obrezka kryuk";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hooked polearm")) OBJ_DESCR(objects[i]) = "kryuchok drevkom";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "pronged polearm")) OBJ_DESCR(objects[i]) = "napravleniyam drevkom";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "beaked polearm")) OBJ_DESCR(objects[i]) = "klyuv drevkom";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "crude spear")) OBJ_DESCR(objects[i]) = "syroy kop'ya";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "long obsidian spear")) OBJ_DESCR(objects[i]) = "dolgo vulkanicheskoye steklo kop'ye";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "runed spear")) OBJ_DESCR(objects[i]) = "runicheskiy kop'ye";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "stout spear")) OBJ_DESCR(objects[i]) = "tolstyy kop'ye";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "throwing spear")) OBJ_DESCR(objects[i]) = "metatel'noye oruzhiye";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "long gray spear")) OBJ_DESCR(objects[i]) = "dlinnyye sedyye kop'ye";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "obsidian lance")) OBJ_DESCR(objects[i]) = "vulkanicheskoye steklo kop'ye";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "crude bow")) OBJ_DESCR(objects[i]) = "syroy luk";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "runed bow")) OBJ_DESCR(objects[i]) = "runicheskiy luk";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "black runed bow")) OBJ_DESCR(objects[i]) = "chernyy runicheskiy luk";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "long bow")) OBJ_DESCR(objects[i]) = "dlinnyy luk";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "spider-legged bow")) OBJ_DESCR(objects[i]) = "pauk-turetski luk";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "crude arrow")) OBJ_DESCR(objects[i]) = "syroy strelka";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "runed arrow")) OBJ_DESCR(objects[i]) = "runicheskiy strelka";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "black runed arrow")) OBJ_DESCR(objects[i]) = "chernyy runicheskiy strelka";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "bamboo arrow")) OBJ_DESCR(objects[i]) = "bambuk strelki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "obsidian arrow")) OBJ_DESCR(objects[i]) = "vulkanicheskoye steklo strelka";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "red stick")) OBJ_DESCR(objects[i]) = "krasnyy palochka";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "spider-legged crossbow")) OBJ_DESCR(objects[i]) = "pauk-turetski arbalet";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "obsidian bolt")) OBJ_DESCR(objects[i]) = "vulkanicheskoye steklo arbalet boyepripasy";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "throwing star")) OBJ_DESCR(objects[i]) = "vostochnaya zvezda";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "crested black plate")) OBJ_DESCR(objects[i]) = "khokhlataya chernyy plita";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "little blue vest")) OBJ_DESCR(objects[i]) = "malen'kiy siniy zhilet";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "crested black mail")) OBJ_DESCR(objects[i]) = "khokhlataya chernyy pochty";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "crude chain mail")) OBJ_DESCR(objects[i]) = "syroy kol'chuga";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "crude ring mail")) OBJ_DESCR(objects[i]) = "syroy kol'tso pochta";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "red robe")) OBJ_DESCR(objects[i]) = "krasnyy khalat";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "blue robe")) OBJ_DESCR(objects[i]) = "goluboy khalat";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "orange robe")) OBJ_DESCR(objects[i]) = "oranzhevyy khalat";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "green robe")) OBJ_DESCR(objects[i]) = "zelenyy khalat";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "coarse mantelet")) OBJ_DESCR(objects[i]) = "grubaya mantiya";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hooded cloak")) OBJ_DESCR(objects[i]) = "plashch s kapyushonom";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "slippery cloak")) OBJ_DESCR(objects[i]) = "skol'zkiy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "faded pall")) OBJ_DESCR(objects[i]) = "ischez priyelos'";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cobwebbed cloak")) OBJ_DESCR(objects[i]) = "melkomshistyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "white coat")) OBJ_DESCR(objects[i]) = "beloye pal'to";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tattered cape")) OBJ_DESCR(objects[i]) = "pobityy mys";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "dirty rag")) OBJ_DESCR(objects[i]) = "gryaznuyu tryapku";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "deadly cloak")) OBJ_DESCR(objects[i]) = "smertel'noy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "opera cloak")) OBJ_DESCR(objects[i]) = "nakidka";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "shaking cloak")) OBJ_DESCR(objects[i]) = "pozhimaya plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ornamental cope")) OBJ_DESCR(objects[i]) = "dekorativnyye spravit'sya";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "vampiric cloak")) OBJ_DESCR(objects[i]) = "vampir plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "funeral cloak")) OBJ_DESCR(objects[i]) = "pokhorony plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "dragonhide cloak")) OBJ_DESCR(objects[i]) = "drakon'yey plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ripped cloak")) OBJ_DESCR(objects[i]) = "razorval plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "metal cloak")) OBJ_DESCR(objects[i]) = "metall plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "granite cloak")) OBJ_DESCR(objects[i]) = "granit plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "asbestos cloak")) OBJ_DESCR(objects[i]) = "asbesta plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "weaving cloak")) OBJ_DESCR(objects[i]) = "tkachestvo plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "second-hand cloak")) OBJ_DESCR(objects[i]) = "sekond-khend plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "phantom cloak")) OBJ_DESCR(objects[i]) = "fantom plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "icky cloak")) OBJ_DESCR(objects[i]) = "nepriglyadnoye plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "petrified cloak")) OBJ_DESCR(objects[i]) = "okamenela plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "flax cloak")) OBJ_DESCR(objects[i]) = "len plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "glass cloak")) OBJ_DESCR(objects[i]) = "bokal plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hammered cloak")) OBJ_DESCR(objects[i]) = "zabil plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "voluminous cloak")) OBJ_DESCR(objects[i]) = "ob''yemnyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "mysterious cloak")) OBJ_DESCR(objects[i]) = "tainstvennyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "prolo cloak")) OBJ_DESCR(objects[i]) = "nizshego klassa plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "weird cloak")) OBJ_DESCR(objects[i]) = "stranno plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "piece of cloth")) OBJ_DESCR(objects[i]) = "kusok tkani";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "leather hat")) OBJ_DESCR(objects[i]) = "kozha shlyapa";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "little red hat")) OBJ_DESCR(objects[i]) = "malen'kiy krasnyy shlyapa";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "iron skull cap")) OBJ_DESCR(objects[i]) = "zheleza cherep shapka";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hard hat")) OBJ_DESCR(objects[i]) = "kaska";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "spider shaped helm")) OBJ_DESCR(objects[i]) = "pauk forme shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "conical hat")) OBJ_DESCR(objects[i]) = "konicheskaya shlyapa";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "plumed helmet")) OBJ_DESCR(objects[i]) = "pernatyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "etched helmet")) OBJ_DESCR(objects[i]) = "travleniya shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "crested helmet")) OBJ_DESCR(objects[i]) = "khokhlataya shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "golden helmet")) OBJ_DESCR(objects[i]) = "zolotoy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "padded helmet")) OBJ_DESCR(objects[i]) = "myagkiy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "steel helmet")) OBJ_DESCR(objects[i]) = "stal'noy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "black helmet")) OBJ_DESCR(objects[i]) = "chernyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "horned helmet")) OBJ_DESCR(objects[i]) = "rogatyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "winged helmet")) OBJ_DESCR(objects[i]) = "krylatyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "viking helmet")) OBJ_DESCR(objects[i]) = "lyzhnoye shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "red helmet")) OBJ_DESCR(objects[i]) = "krasnyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "resounding helmet")) OBJ_DESCR(objects[i]) = "gromkoye shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "echo helmet")) OBJ_DESCR(objects[i]) = "ekho shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "middle-earth helmet")) OBJ_DESCR(objects[i]) = "sredizem'ye shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gemmed helmet")) OBJ_DESCR(objects[i]) = "ukrashennyye shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "spired helmet")) OBJ_DESCR(objects[i]) = "shpilem shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "basin helmet")) OBJ_DESCR(objects[i]) = "basseyn shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hydra helmet")) OBJ_DESCR(objects[i]) = "gidra shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "wok-shaped helmet")) OBJ_DESCR(objects[i]) = "vok v forme shlema";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "masked helmet")) OBJ_DESCR(objects[i]) = "maske shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tribal helmet")) OBJ_DESCR(objects[i]) = "plemennyye shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "internet helmet")) OBJ_DESCR(objects[i]) = "vsemirnaya pautina shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "crazy helmet")) OBJ_DESCR(objects[i]) = "sumasshedshiy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tight helmet")) OBJ_DESCR(objects[i]) = "plotno shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "great helmet")) OBJ_DESCR(objects[i]) = "bol'shoy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "deformed helmet")) OBJ_DESCR(objects[i]) = "deformirovannogo shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "rotating helmet")) OBJ_DESCR(objects[i]) = "vrashchayushchikhsya shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "covered helmet")) OBJ_DESCR(objects[i]) = "pokryty shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "filtered helmet")) OBJ_DESCR(objects[i]) = "fil'truyut shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "visored helmet")) OBJ_DESCR(objects[i]) = "zabralom shlema";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "white gloves")) OBJ_DESCR(objects[i]) = "belyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "old gloves")) OBJ_DESCR(objects[i]) = "staryye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "padded gloves")) OBJ_DESCR(objects[i]) = "myagkiye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "slit gloves")) OBJ_DESCR(objects[i]) = "shcheli perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ice-cold gloves")) OBJ_DESCR(objects[i]) = "ledyanyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "slippery gloves")) OBJ_DESCR(objects[i]) = "skol'zkiye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "riding gloves")) OBJ_DESCR(objects[i]) = "yezda perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "brand-new gloves")) OBJ_DESCR(objects[i]) = "sovershenno novyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "steel gloves")) OBJ_DESCR(objects[i]) = "stal'nyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "black gloves")) OBJ_DESCR(objects[i]) = "chernyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "mirrored gloves")) OBJ_DESCR(objects[i]) = "zerkal'nyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "moth-bitten gloves")) OBJ_DESCR(objects[i]) = "moli ukusila perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "graffiti gloves")) OBJ_DESCR(objects[i]) = "graffiti perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "warped gloves")) OBJ_DESCR(objects[i]) = "deformirovannyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "concrete gloves")) OBJ_DESCR(objects[i]) = "konkretnyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "nondescript gloves")) OBJ_DESCR(objects[i]) = "neopredelennyy perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "spiky gloves")) OBJ_DESCR(objects[i]) = "kolyuchiye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "grey-shaded gloves")) OBJ_DESCR(objects[i]) = "sero-zatenennykh perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "spacey gloves")) OBJ_DESCR(objects[i]) = "speysi perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "paper gloves")) OBJ_DESCR(objects[i]) = "bumazhnyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "vampiric gloves")) OBJ_DESCR(objects[i]) = "vampiry perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "transparent gloves")) OBJ_DESCR(objects[i]) = "prozrachnyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fencing gloves")) OBJ_DESCR(objects[i]) = "ograzhdeniya perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "blue and green shield")) OBJ_DESCR(objects[i]) = "siniy i zelenyy shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "white-handed shield")) OBJ_DESCR(objects[i]) = "belyy rukami shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "red-eyed shield")) OBJ_DESCR(objects[i]) = "krasnyye glaza shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "large round shield")) OBJ_DESCR(objects[i]) = "bol'shoy kruglyy shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "dark red shield")) OBJ_DESCR(objects[i]) = "temno-krasnyy shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "dark blue shield")) OBJ_DESCR(objects[i]) = "temno-siniy shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "dull yellow shield")) OBJ_DESCR(objects[i]) = "skuchno zheltogo shchita";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "dark green shield")) OBJ_DESCR(objects[i]) = "temno-zelenyy shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "shiny shield")) OBJ_DESCR(objects[i]) = "blestyashchiye shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "slippery shield")) OBJ_DESCR(objects[i]) = "skol'zkoy shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gray dragonhide shield")) OBJ_DESCR(objects[i]) = "seryy vazhno shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "silver dragonhide shield")) OBJ_DESCR(objects[i]) = "serebro vazhno shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "mercurial dragonhide shield")) OBJ_DESCR(objects[i]) = "rtutnyy vazhno shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "shimmering dragonhide shield")) OBJ_DESCR(objects[i]) = "mertsayushchiy shchit vazhno";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "deep dragonhide shield")) OBJ_DESCR(objects[i]) = "gluboko vazhno shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "red dragonhide shield")) OBJ_DESCR(objects[i]) = "krasnyy shchit vazhno";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "white dragonhide shield")) OBJ_DESCR(objects[i]) = "belyy shchit vazhno";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "orange dragonhide shield")) OBJ_DESCR(objects[i]) = "oranzhevyy vazhno shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "black dragonhide shield")) OBJ_DESCR(objects[i]) = "chernyy shchit vazhno";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "blue dragonhide shield")) OBJ_DESCR(objects[i]) = "siniy shchit vazhno";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "green dragonhide shield")) OBJ_DESCR(objects[i]) = "zelenyy shchit vazhno";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "golden dragonhide shield")) OBJ_DESCR(objects[i]) = "zolotoy shchit vazhno";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "stone dragonhide shield")) OBJ_DESCR(objects[i]) = "kamen' vazhno shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cyan dragonhide shield")) OBJ_DESCR(objects[i]) = "goluboy shchit vazhno";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "yellow dragonhide shield")) OBJ_DESCR(objects[i]) = "zheltyy shchit vazhno";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "platinum dragonhide shield")) OBJ_DESCR(objects[i]) = "platina drakon'yey shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "brass dragonhide shield")) OBJ_DESCR(objects[i]) = "latun' drakon'yey shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "copper dragonhide shield")) OBJ_DESCR(objects[i]) = "med' drakon'yey shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "emerald dragonhide shield")) OBJ_DESCR(objects[i]) = "izumrud drakon'yey shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ruby dragonhide shield")) OBJ_DESCR(objects[i]) = "rubin drakon'yey shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sapphire dragonhide shield")) OBJ_DESCR(objects[i]) = "sapfir drakon'yey shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "diamond dragonhide shield")) OBJ_DESCR(objects[i]) = "almaznyy drakon'yey shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "amethyst dragonhide shield")) OBJ_DESCR(objects[i]) = "ametist drakon'yey shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "purple dragonhide shield")) OBJ_DESCR(objects[i]) = "fioletovyy drakon'yey shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "polished silver shield")) OBJ_DESCR(objects[i]) = "polirovannyy serebryanyy shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "white boots")) OBJ_DESCR(objects[i]) = "belyye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "walking shoes")) OBJ_DESCR(objects[i]) = "obuv' dlya khod'by";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hard shoes")) OBJ_DESCR(objects[i]) = "zhestkiye botinki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "little black boots")) OBJ_DESCR(objects[i]) = "malen'kiye chernyye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "jackboots")) OBJ_DESCR(objects[i]) = "botforty";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "heeled sandals")) OBJ_DESCR(objects[i]) = "bosonozhki na kabluke";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "soft footwear")) OBJ_DESCR(objects[i]) = "myagkaya obuv'";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "womens footwear")) OBJ_DESCR(objects[i]) = "zhenskaya obuv'";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "light footwear")) OBJ_DESCR(objects[i]) = "svet obuv'";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "female footwear")) OBJ_DESCR(objects[i]) = "obuv' zhenskaya";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "asian footwear")) OBJ_DESCR(objects[i]) = "aziatskaya obuv'";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "red overknees")) OBJ_DESCR(objects[i]) = "krasnyy nad kolenyami";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "heeled boots")) OBJ_DESCR(objects[i]) = "kabluke";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "combat boots")) OBJ_DESCR(objects[i]) = "boyevyye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "thigh boots")) OBJ_DESCR(objects[i]) = "bedra sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "jungle boots")) OBJ_DESCR(objects[i]) = "dzhunglyakh sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hiking boots")) OBJ_DESCR(objects[i]) = "turisticheskiye botinki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "battle boots")) OBJ_DESCR(objects[i]) = "bitvy sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "buckled boots")) OBJ_DESCR(objects[i]) = "pryazhkami sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "mud boots")) OBJ_DESCR(objects[i]) = "gryazevyye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "steel boots")) OBJ_DESCR(objects[i]) = "stal'nyye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "riding boots")) OBJ_DESCR(objects[i]) = "sapogi dlya verkhovoy yezdy";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "persian boots")) OBJ_DESCR(objects[i]) = "persidskiye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hot boots")) OBJ_DESCR(objects[i]) = "goryachiye botinki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "explosive boots")) OBJ_DESCR(objects[i]) = "vzryvnyye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "profiled boots")) OBJ_DESCR(objects[i]) = "profilirovannyye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "irregular boots")) OBJ_DESCR(objects[i]) = "neregulyarnyye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cast iron boots")) OBJ_DESCR(objects[i]) = "chugunnyye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "winter boots")) OBJ_DESCR(objects[i]) = "sapogi zimniye";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "torn boots")) OBJ_DESCR(objects[i]) = "rvanyye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "chess boots")) OBJ_DESCR(objects[i]) = "shakhmatnyye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "plateau boots")) OBJ_DESCR(objects[i]) = "plato sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "jade boots")) OBJ_DESCR(objects[i]) = "nefrita sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "snow boots")) OBJ_DESCR(objects[i]) = "zimniye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "bag")) OBJ_DESCR(objects[i]) = "sumka";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "key")) OBJ_DESCR(objects[i]) = "klyuch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "candle")) OBJ_DESCR(objects[i]) = "svecha";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "lamp")) OBJ_DESCR(objects[i]) = "lampa";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "whistle")) OBJ_DESCR(objects[i]) = "svistok";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "flute")) OBJ_DESCR(objects[i]) = "svireli";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "horn")) OBJ_DESCR(objects[i]) = "rog";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "harp")) OBJ_DESCR(objects[i]) = "orkestrovaya orudiyem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "drum")) OBJ_DESCR(objects[i]) = "baraban";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "iron hook")) OBJ_DESCR(objects[i]) = "zheleznyy kryuk";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "lightsaber")) OBJ_DESCR(objects[i]) = "lazernyy mech";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "double lightsaber")) OBJ_DESCR(objects[i]) = "dvoynoy svetovoy mech";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "looking glass")) OBJ_DESCR(objects[i]) = "ishchu steklo";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "glass orb")) OBJ_DESCR(objects[i]) = "steklo shar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "leather bag")) OBJ_DESCR(objects[i]) = "kozhanaya sumka";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "highly-complicated electronic device")) OBJ_DESCR(objects[i]) = "vysoko slozhnym elektronnym ustroystvom";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "two-wired thing")) OBJ_DESCR(objects[i]) = "dva provodnoy veshch'";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "three-wired thing")) OBJ_DESCR(objects[i]) = "tri provodnoy veshch'";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "many-wired thing")) OBJ_DESCR(objects[i]) = "mnogiye-provodnoy veshch'";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "box containing little plastic cards")) OBJ_DESCR(objects[i]) = "korobka, soderzhashchaya malen'kiye plastikovyye karty";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "four-wired thing")) OBJ_DESCR(objects[i]) = "chetyrekh provodnoy veshch'";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "box of obscure-looking glass devices")) OBJ_DESCR(objects[i]) = "korobka neyasnykh-zazerkal'ye ustroystv";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "candelabrum")) OBJ_DESCR(objects[i]) = "kandelyabr";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "silver bell")) OBJ_DESCR(objects[i]) = "serebryanyy kolokol'chik";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "splash of venom")) OBJ_DESCR(objects[i]) = "vsplesk yada";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "large silver axe")) OBJ_DESCR(objects[i]) = "bol'shoy serebryanoy topor";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gray short sword")) OBJ_DESCR(objects[i]) = "seryy korotkiy mech";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "silver monk's staff")) OBJ_DESCR(objects[i]) = "sotrudniki serebro monakha";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "wide conical hat")) OBJ_DESCR(objects[i]) = "whirokiy konicheskiy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "metal disk")) OBJ_DESCR(objects[i]) = "metall kruglyy ob''yekt";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "green bowel")) OBJ_DESCR(objects[i]) = "zelenyy kishechnika";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fish bowl")) OBJ_DESCR(objects[i]) = "ryba shar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "armored black dress")) OBJ_DESCR(objects[i]) = "bronirovannyy chernoye plat'ye";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "loud foppish suit")) OBJ_DESCR(objects[i]) = "gromko modno kostyum";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gemstone-adorned clothing")) OBJ_DESCR(objects[i]) = "dragotsennyy kamen' ukrashennyye odezhdy";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "clean white clothes")) OBJ_DESCR(objects[i]) = "chistyye belyye odezhdy";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "silver clothes")) OBJ_DESCR(objects[i]) = "serebryanyye odezhdy";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tight black clothes")) OBJ_DESCR(objects[i]) = "plotno chernyye odezhdy";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "apron")) OBJ_DESCR(objects[i]) = "fartuk";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "lion skin")) OBJ_DESCR(objects[i]) = "lev kozhi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "glass shield")) OBJ_DESCR(objects[i]) = "steklo shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fighting gloves")) OBJ_DESCR(objects[i]) = "boyevyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "glass boots")) OBJ_DESCR(objects[i]) = "steklyannyye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "beaked axe")) OBJ_DESCR(objects[i]) = "klyuv topor";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "flower-pommeled broadsword")) OBJ_DESCR(objects[i]) = "tsvetok luka palash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "huge sword")) OBJ_DESCR(objects[i]) = "ogromnyy mech";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "black polearm")) OBJ_DESCR(objects[i]) = "chernyy drevkom";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "rod")) OBJ_DESCR(objects[i]) = "sterzhen'";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "beaked staff")) OBJ_DESCR(objects[i]) = "sotrudniki klyuv";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "headpiece")) OBJ_DESCR(objects[i]) = "zastavka";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "round helmet")) OBJ_DESCR(objects[i]) = "kruglyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "yellow helmet")) OBJ_DESCR(objects[i]) = "zheltyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "orange visored helmet")) OBJ_DESCR(objects[i]) = "oranzhevyy shlem zabralom";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tasseled helmet")) OBJ_DESCR(objects[i]) = "kistochkami shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "dim colored helmet")) OBJ_DESCR(objects[i]) = "tusklyy tsvetnoy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "chain-aventailed helmet")) OBJ_DESCR(objects[i]) = "tsepi spetsial'nyy kozyrek shlema";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "scuba helmet")) OBJ_DESCR(objects[i]) = "podvodnoye shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "radio helmet")) OBJ_DESCR(objects[i]) = "translyatsii shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "orange suit")) OBJ_DESCR(objects[i]) = "oranzhevyy kostyum";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "red leather armor")) OBJ_DESCR(objects[i]) = "krasnyy kozhanyy dospekh";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "little black dress")) OBJ_DESCR(objects[i]) = "malen'koye chernoye plat'ye";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "white dress")) OBJ_DESCR(objects[i]) = "beloye plat'ye";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "black men's suit")) OBJ_DESCR(objects[i]) = "chernyye muzhskoy kostyum";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "camouflage patterned clothes")) OBJ_DESCR(objects[i]) = "kamuflyazh s risunkom odezhda";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "strange rings")) OBJ_DESCR(objects[i]) = "strannyye kol'tsa";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tightly fitting suit")) OBJ_DESCR(objects[i]) = "plotno prilegayushchiye kostyum";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "red mantle")) OBJ_DESCR(objects[i]) = "krasnyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "wings")) OBJ_DESCR(objects[i]) = "kryl'ya";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "brown burnoose")) OBJ_DESCR(objects[i]) = "korichnevyy burnuse";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "vestiary")) OBJ_DESCR(objects[i]) = "riznitsa";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "long overcoat")) OBJ_DESCR(objects[i]) = "dlinnoye pal'to";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gaberdine")) OBJ_DESCR(objects[i]) = "gabardin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "livid worped-moon shield")) OBJ_DESCR(objects[i]) = "yarosti deformirovannyye luna shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "polished shield")) OBJ_DESCR(objects[i]) = "polirovannyy shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "rainbow dragonhide shield")) OBJ_DESCR(objects[i]) = "raduga drakon'yey shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "blood dragonhide shield")) OBJ_DESCR(objects[i]) = "drakon'yey krovi shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "plain dragonhide shield")) OBJ_DESCR(objects[i]) = "ravniny drakon'yey shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sky dragonhide shield")) OBJ_DESCR(objects[i]) = "nebo drakon'yey shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "water dragonhide shield")) OBJ_DESCR(objects[i]) = "voda iz drakon'yey shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "orange gloves")) OBJ_DESCR(objects[i]) = "oranzhevyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tightly fitting gloves")) OBJ_DESCR(objects[i]) = "plotno prilegayushchiye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "light gloves")) OBJ_DESCR(objects[i]) = "legkiye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "camouflage patterned gloves")) OBJ_DESCR(objects[i]) = "kamuflyazh s risunkom perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "odd pair of gloves")) OBJ_DESCR(objects[i]) = "stranno para perchatok";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "even pair of gloves")) OBJ_DESCR(objects[i]) = "dazhe para perchatok";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "covered pair of gloves")) OBJ_DESCR(objects[i]) = "pokryty para perchatok";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "boxing gloves")) OBJ_DESCR(objects[i]) = "boks para perchatok";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "turning shoes")) OBJ_DESCR(objects[i]) = "prevrashchaya obuv'";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "rainbow boots")) OBJ_DESCR(objects[i]) = "raduga sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tightly fitting boots")) OBJ_DESCR(objects[i]) = "plotno prilegayushchiye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "roller boots")) OBJ_DESCR(objects[i]) = "rolikovyye botinki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "camouflage patterned boots")) OBJ_DESCR(objects[i]) = "kamuflyazh s risunkom sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "orange boots")) OBJ_DESCR(objects[i]) = "oranzhevyye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "platform boots")) OBJ_DESCR(objects[i]) = "sapogi na platforme";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "side gore boots")) OBJ_DESCR(objects[i]) = "bokovyye gor sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "side zip boots")) OBJ_DESCR(objects[i]) = "storona molnii sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cone-shaped helmet")) OBJ_DESCR(objects[i]) = "konusoobraznyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "dark helmet")) OBJ_DESCR(objects[i]) = "temno-shlem";

		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "velcro boots")) OBJ_DESCR(objects[i]) = "lipuchki sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "complete helmet")) OBJ_DESCR(objects[i]) = "polnaya shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "weeb cloak")) OBJ_DESCR(objects[i]) = "zese plashch";

		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "silver circular blade")) OBJ_DESCR(objects[i]) = "serebro krugloye lezviye";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "runed curved sword")) OBJ_DESCR(objects[i]) = "runicheskiy krivoy mech";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "wide metal helm")) OBJ_DESCR(objects[i]) = "shirokiy metallicheskiy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "runed helm")) OBJ_DESCR(objects[i]) = "runicheskiy rulya";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "expensive clothes")) OBJ_DESCR(objects[i]) = "dorogaya odezhda";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "expensive dress")) OBJ_DESCR(objects[i]) = "dorogoye plat'ye";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "long-sleeved jacket")) OBJ_DESCR(objects[i]) = "kurtki s dlinnymi rukavami";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "flowery shorts and lei")) OBJ_DESCR(objects[i]) = "tsvetochnyye shorty i ley";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "wooden box")) OBJ_DESCR(objects[i]) = "derevyannaya korobka";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "wooden container")) OBJ_DESCR(objects[i]) = "derevyannyy konteyner";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cooling box")) OBJ_DESCR(objects[i]) = "korobka okhlazhdeniya";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "psychic dragonhide shield")) OBJ_DESCR(objects[i]) = "psikhicheskaya drakon'yey shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "spinning helmet")) OBJ_DESCR(objects[i]) = "spina shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "disgusting cloak")) OBJ_DESCR(objects[i]) = "otvratitel'noye plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "comfortable gloves")) OBJ_DESCR(objects[i]) = "udobnyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "yellow gloves")) OBJ_DESCR(objects[i]) = "zheltyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fleecy boots")) OBJ_DESCR(objects[i]) = "flis sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "space helmet")) OBJ_DESCR(objects[i]) = "kosmicheskiy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "godless cloak")) OBJ_DESCR(objects[i]) = "bezbozhnaya plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "guzzle helmet")) OBJ_DESCR(objects[i]) = "shlem obzhora";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ghostly cloak")) OBJ_DESCR(objects[i]) = "prizrachnyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "water-pipe helmet")) OBJ_DESCR(objects[i]) = "shlem kal'yannym";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "thick-soled boots")) OBJ_DESCR(objects[i]) = "tolstoy podoshve sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "mechanical helmet")) OBJ_DESCR(objects[i]) = "mekhanicheskiy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "wafer-thin cloak")) OBJ_DESCR(objects[i]) = "tonchayshaya plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "polnish gloves")) OBJ_DESCR(objects[i]) = "pol'skiye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "clunky heels")) OBJ_DESCR(objects[i]) = "neuklyuzhiye kabluki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "wedge boots")) OBJ_DESCR(objects[i]) = "klin sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "translucent helmet")) OBJ_DESCR(objects[i]) = "poluprozrachnyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "faded mittens")) OBJ_DESCR(objects[i]) = "vytsvetshiye varezhki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "jarring cloak")) OBJ_DESCR(objects[i]) = "sotryaseniye plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "politician cloak")) OBJ_DESCR(objects[i]) = "politik plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "meteoric helmet")) OBJ_DESCR(objects[i]) = "meteoricheskoye shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hugging boots")) OBJ_DESCR(objects[i]) = "obnimat'sya sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "void cloak")) OBJ_DESCR(objects[i]) = "nedeystvitel'nym plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "anachorononononaut helmet")) OBJ_DESCR(objects[i]) = "anakhronizmom shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fin boots")) OBJ_DESCR(objects[i]) = "plavnik sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "straitjacket cloak")) OBJ_DESCR(objects[i]) = "smiritel'naya rubashka plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "angelic cloak")) OBJ_DESCR(objects[i]) = "angel'skoye plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "full helmet")) OBJ_DESCR(objects[i]) = "polnyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "evil cloak")) OBJ_DESCR(objects[i]) = "zlo plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "winter stilettos")) OBJ_DESCR(objects[i]) = "zima stilety";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "aluminium helmet")) OBJ_DESCR(objects[i]) = "shlem iz alyuminiya";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "demonic cloak")) OBJ_DESCR(objects[i]) = "demonicheskaya plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "anti-government helmet")) OBJ_DESCR(objects[i]) = "antipravitel'stvennaya shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "clumsy gloves")) OBJ_DESCR(objects[i]) = "neuklyuzhiye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ankle boots")) OBJ_DESCR(objects[i]) = "botil'ony";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "frequent helmet")) OBJ_DESCR(objects[i]) = "chastyy shlem";

		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "RNG helmet")) OBJ_DESCR(objects[i]) = "shlem gsch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "block-heeled boots")) OBJ_DESCR(objects[i]) = "blok kablukakh sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "awesome gloves")) OBJ_DESCR(objects[i]) = "udivitel'nyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "shrouded cloak")) OBJ_DESCR(objects[i]) = "okutana plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fingerless gloves")) OBJ_DESCR(objects[i]) = "mitenki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "energizer cloak")) OBJ_DESCR(objects[i]) = "antidepressant plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "mantle of coat")) OBJ_DESCR(objects[i]) = "mantiya pal'to";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "chilling cloak")) OBJ_DESCR(objects[i]) = "pugayushchim plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "bug-tracking helmet")) OBJ_DESCR(objects[i]) = "oshibka otslezhivaniya shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fatal gloves")) OBJ_DESCR(objects[i]) = "fatal'nyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "beautiful heels")) OBJ_DESCR(objects[i]) = "krasivyye kabluki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "electrostatic cloak")) OBJ_DESCR(objects[i]) = "elektrostaticheskoye plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "weeping helmet")) OBJ_DESCR(objects[i]) = "placha shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "runic gloves")) OBJ_DESCR(objects[i]) = "runa rukovitsakh";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "roman sandals")) OBJ_DESCR(objects[i]) = "rimskiye sandalii";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "homicidal cloak")) OBJ_DESCR(objects[i]) = "smertonosnyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "narrow helmet")) OBJ_DESCR(objects[i]) = "uzkiy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "spanish gloves")) OBJ_DESCR(objects[i]) = "ispanskiy perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "castlevania boots")) OBJ_DESCR(objects[i]) = "zamok vaney sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "greek cloak")) OBJ_DESCR(objects[i]) = "grecheskiy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "celtic helmet")) OBJ_DESCR(objects[i]) = "kel'tskaya shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "english gloves")) OBJ_DESCR(objects[i]) = "angliyskiye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "korean sandals")) OBJ_DESCR(objects[i]) = "koreyskiye sandalii";

		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "forgetful cloak")) OBJ_DESCR(objects[i]) = "zabyvchiv plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "spider boots")) OBJ_DESCR(objects[i]) = "pauk sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "yellow cloak")) OBJ_DESCR(objects[i]) = "zheltyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "changing cloak")) OBJ_DESCR(objects[i]) = "izmeneniye plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "black cloak")) OBJ_DESCR(objects[i]) = "chernyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "blue cloak")) OBJ_DESCR(objects[i]) = "siniy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "red cloak")) OBJ_DESCR(objects[i]) = "krasnyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "green cloak")) OBJ_DESCR(objects[i]) = "zelenyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "mauve cloak")) OBJ_DESCR(objects[i]) = "rozovato-lilovyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "purple cloak")) OBJ_DESCR(objects[i]) = "fioletovyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "dark cloak")) OBJ_DESCR(objects[i]) = "neyasnymi plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "shell cloak")) OBJ_DESCR(objects[i]) = "plashch obolochki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "chinese cloak")) OBJ_DESCR(objects[i]) = "kitayskiy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gray cloak")) OBJ_DESCR(objects[i]) = "seryy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "polyform cloak")) OBJ_DESCR(objects[i]) = "sopolimer forma plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "absorbing cloak")) OBJ_DESCR(objects[i]) = "pogloshchayushchiy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cyan cloak")) OBJ_DESCR(objects[i]) = "goluboy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "deep cloak")) OBJ_DESCR(objects[i]) = "glubokiy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "blinking cloak")) OBJ_DESCR(objects[i]) = "migayet plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tone cloak")) OBJ_DESCR(objects[i]) = "ton plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "running cloak")) OBJ_DESCR(objects[i]) = "rabotayet plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "pink cloak")) OBJ_DESCR(objects[i]) = "bakh-rozovyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "birthcloth")) OBJ_DESCR(objects[i]) = "rozhdeniye tkan'";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "colorless cloak")) OBJ_DESCR(objects[i]) = "bestsvetnaya plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "frade mantle")) OBJ_DESCR(objects[i]) = "fantazii mantii";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "heavy cloak")) OBJ_DESCR(objects[i]) = "tyazhelyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "grass cloak")) OBJ_DESCR(objects[i]) = "plashch trava";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "contaminated coat")) OBJ_DESCR(objects[i]) = "zagryaznennoye pal'to";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "withered cloak")) OBJ_DESCR(objects[i]) = "uvyadshiye plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "white cloak")) OBJ_DESCR(objects[i]) = "belyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ignorant cloak")) OBJ_DESCR(objects[i]) = "nevezhestvennyye plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "avenger cloak")) OBJ_DESCR(objects[i]) = "mstitel' plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gravity cloak")) OBJ_DESCR(objects[i]) = "gravitatsionnyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "wishful cloak")) OBJ_DESCR(objects[i]) = "zhelayemoye za deystvitel'noye plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "poke mongo cloak")) OBJ_DESCR(objects[i]) = "sovat' mongo plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "levuntation cloak")) OBJ_DESCR(objects[i]) = "levitatsii plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "enduring cloak")) OBJ_DESCR(objects[i]) = "vyderzhivayushchiy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "minded cloak")) OBJ_DESCR(objects[i]) = "myslyashchikh plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "quicktravel cloak")) OBJ_DESCR(objects[i]) = "bystryy plashch puteshestviya";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "geek cloak")) OBJ_DESCR(objects[i]) = "komp'yutershchik plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "orange cloak")) OBJ_DESCR(objects[i]) = "oranzhevyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "pastel cloak")) OBJ_DESCR(objects[i]) = "pastel'nyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "nurse cloak")) OBJ_DESCR(objects[i]) = "medsestra plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "slexual cloak")) OBJ_DESCR(objects[i]) = "polovoy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "angband cloak")) OBJ_DESCR(objects[i]) = "plashch sredizem'ye krepost'";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fleecy-colored cloak")) OBJ_DESCR(objects[i]) = "vorsistyye tsvetnoy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "concrete cloak")) OBJ_DESCR(objects[i]) = "betonnyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "anorexia cloak")) OBJ_DESCR(objects[i]) = "yedyat plashch rasstroystvo";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "flash cloak")) OBJ_DESCR(objects[i]) = "flesh-plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "magenta cloak")) OBJ_DESCR(objects[i]) = "purpurnogo plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "icy cloak")) OBJ_DESCR(objects[i]) = "ledyanoy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "starwars cloak")) OBJ_DESCR(objects[i]) = "zvezdnyye voyny plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tunnel cloak")) OBJ_DESCR(objects[i]) = "tunnel'naya plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "dnethack cloak")) OBJ_DESCR(objects[i]) = "podzemeliy i vnezemnyye plashch vzlomat'";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "barley cloak")) OBJ_DESCR(objects[i]) = "yachmen' plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "taiga cloak")) OBJ_DESCR(objects[i]) = "tayga plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "boxing gown")) OBJ_DESCR(objects[i]) = "plat'ye boks";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "team splat cloak")) OBJ_DESCR(objects[i]) = "vosklitsatel'nyy znak plashch komanda";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "eldritch cloak")) OBJ_DESCR(objects[i]) = "sverkh'yestestvennyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "yellow robe")) OBJ_DESCR(objects[i]) = "zheltyy khalat";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "purple robe")) OBJ_DESCR(objects[i]) = "bagryanitse";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "white robe")) OBJ_DESCR(objects[i]) = "belyy khalat";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "black robe")) OBJ_DESCR(objects[i]) = "chernyy khalat";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gray robe")) OBJ_DESCR(objects[i]) = "seryy khalat";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cyan robe")) OBJ_DESCR(objects[i]) = "zelenovato-goluboy khalat";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "octarine robe")) OBJ_DESCR(objects[i]) = "vos'moy tsvet khalata";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "brown robe")) OBJ_DESCR(objects[i]) = "korichnevyy khalat";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "camo robe")) OBJ_DESCR(objects[i]) = "kamuflyazhnaya roba";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "pink robe")) OBJ_DESCR(objects[i]) = "rozovyy khalat";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "inka cloak")) OBJ_DESCR(objects[i]) = "inka plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "viva cloak")) OBJ_DESCR(objects[i]) = "vivat plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "saddle")) OBJ_DESCR(objects[i]) = "yezda instrument";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "leash")) OBJ_DESCR(objects[i]) = "instrument domashneye zhivotnoye";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "living boots")) OBJ_DESCR(objects[i]) = "zhivyye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "rain boots")) OBJ_DESCR(objects[i]) = "dozhd' sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "silk helmet")) OBJ_DESCR(objects[i]) = "shelk shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tar gloves")) OBJ_DESCR(objects[i]) = "bituminoznykh perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "musical helmet")) OBJ_DESCR(objects[i]) = "muzykal'nyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "poor helmet")) OBJ_DESCR(objects[i]) = "bednyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gray helmet")) OBJ_DESCR(objects[i]) = "seryy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "radiant helmet")) OBJ_DESCR(objects[i]) = "siyayushchiy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "secret helmet")) OBJ_DESCR(objects[i]) = "sekret shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "inkcoat helmet")) OBJ_DESCR(objects[i]) = "shlem pal'to chernil";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "knowledgeable helmet")) OBJ_DESCR(objects[i]) = "znayushchikh shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "organic helmet")) OBJ_DESCR(objects[i]) = "organicheskiy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "formula one helmet")) OBJ_DESCR(objects[i]) = "formula odin shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "arduous helmet")) OBJ_DESCR(objects[i]) = "trudnyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "long-hair helmet")) OBJ_DESCR(objects[i]) = "shlem s dlinnymi volosami";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "meso helmet")) OBJ_DESCR(objects[i]) = "shlem mezo";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "difficult cloak")) OBJ_DESCR(objects[i]) = "trudnyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ugly cloak")) OBJ_DESCR(objects[i]) = "nekrasivo plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "armored cloak")) OBJ_DESCR(objects[i]) = "bronirovannyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "inky cloak")) OBJ_DESCR(objects[i]) = "chernil'nyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gentle cloak")) OBJ_DESCR(objects[i]) = "nezhnyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "living cloak")) OBJ_DESCR(objects[i]) = "zhivoy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "irradiation cloak")) OBJ_DESCR(objects[i]) = "oblucheniye plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "silk cloak")) OBJ_DESCR(objects[i]) = "shelkovyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "heavenly cloak")) OBJ_DESCR(objects[i]) = "nebesnyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "soft cloak")) OBJ_DESCR(objects[i]) = "myagkiy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "bituminous mantle")) OBJ_DESCR(objects[i]) = "bitumnaya mantii";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "stone cloak")) OBJ_DESCR(objects[i]) = "kamen' plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "secretion cloak")) OBJ_DESCR(objects[i]) = "plashch sekretsii";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "excrement cloak")) OBJ_DESCR(objects[i]) = "ekskrementy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hungry cloak")) OBJ_DESCR(objects[i]) = "golodnymi plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "science cloak")) OBJ_DESCR(objects[i]) = "nauka plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "guild cloak")) OBJ_DESCR(objects[i]) = "gil'dii plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tower cloak")) OBJ_DESCR(objects[i]) = "bashnya plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "arcane cloak")) OBJ_DESCR(objects[i]) = "arkan plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "organic cloak")) OBJ_DESCR(objects[i]) = "organicheskiy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "natural cloak")) OBJ_DESCR(objects[i]) = "yestestvennyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "compost cloak")) OBJ_DESCR(objects[i]) = "kompost plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "erotic boots")) OBJ_DESCR(objects[i]) = "eroticheskiye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sputa boots")) OBJ_DESCR(objects[i]) = "mokrota sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "arcane boots")) OBJ_DESCR(objects[i]) = "skrytyy botinki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "street boots")) OBJ_DESCR(objects[i]) = "ulichnyye botinki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "heavy boots")) OBJ_DESCR(objects[i]) = "tyazhelyye botinki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gentle boots")) OBJ_DESCR(objects[i]) = "nezhnyye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "nature boots")) OBJ_DESCR(objects[i]) = "priroda sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "radiant heels")) OBJ_DESCR(objects[i]) = "izluchayushchiye kabluki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "turbo boots")) OBJ_DESCR(objects[i]) = "turbo sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "area boots")) OBJ_DESCR(objects[i]) = "oblast' sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sexy heels")) OBJ_DESCR(objects[i]) = "seksual'nyye kabluki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "stroking boots")) OBJ_DESCR(objects[i]) = "poglazhivaya sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "velvet gloves")) OBJ_DESCR(objects[i]) = "barkhatnyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "nonexistant gloves")) OBJ_DESCR(objects[i]) = "nesushchestvuyushchiy perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "racer gloves")) OBJ_DESCR(objects[i]) = "gonshchik perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "inka gloves")) OBJ_DESCR(objects[i]) = "inka perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "viva gloves")) OBJ_DESCR(objects[i]) = "viva perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "poor gloves")) OBJ_DESCR(objects[i]) = "bednyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "shitty gloves")) OBJ_DESCR(objects[i]) = "der'movyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "natural gloves")) OBJ_DESCR(objects[i]) = "prirodnyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sensor gloves")) OBJ_DESCR(objects[i]) = "sensornyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "evil dragonhide shield")) OBJ_DESCR(objects[i]) = "zloy shchit iz shkury drakona";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "magic dragonhide shield")) OBJ_DESCR(objects[i]) = "magicheskiy drakon skryt' shchit";

		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "blue sneakers")) OBJ_DESCR(objects[i]) = "siniye krossovki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "dusty gloves")) OBJ_DESCR(objects[i]) = "pyl'nyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "woven cloak")) OBJ_DESCR(objects[i]) = "tkanyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "colorfade cloak")) OBJ_DESCR(objects[i]) = "plashch tsveta";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "femmy boots")) OBJ_DESCR(objects[i]) = "zhenskiye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "kitchen hat")) OBJ_DESCR(objects[i]) = "kukhonnaya shlyapa";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "dream helmet")) OBJ_DESCR(objects[i]) = "shlem mechty";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "red sneakers")) OBJ_DESCR(objects[i]) = "krasnyye krossovki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "residential cloak")) OBJ_DESCR(objects[i]) = "zhiloy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "yellow sneakers")) OBJ_DESCR(objects[i]) = "zheltyye krossovki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fertile cloak")) OBJ_DESCR(objects[i]) = "plodorodnyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "pink sneakers")) OBJ_DESCR(objects[i]) = "rozovyye krossovki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "grimace helmet")) OBJ_DESCR(objects[i]) = "grimasovyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sharp-edged sandals")) OBJ_DESCR(objects[i]) = "ostrokonechnyye sandalii";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ruined helmet")) OBJ_DESCR(objects[i]) = "razrushennyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ski heels")) OBJ_DESCR(objects[i]) = "lyzhnyye kabluki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "slowing gown")) OBJ_DESCR(objects[i]) = "zamedlennoye plat'ye";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "bleached cloak")) OBJ_DESCR(objects[i]) = "otbelennyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "foundry cloak")) OBJ_DESCR(objects[i]) = "liteynyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fetish heels")) OBJ_DESCR(objects[i]) = "idol kabluki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "reliance cloak")) OBJ_DESCR(objects[i]) = "plashch doveriya";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "rubynus helmet")) OBJ_DESCR(objects[i]) = "rubinovyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "thinking helmet")) OBJ_DESCR(objects[i]) = "myslyashchiy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "silk fingerlings")) OBJ_DESCR(objects[i]) = "shelkovyye mal'ki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "netradiation helmet")) OBJ_DESCR(objects[i]) = "obluchonnyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "skinny cloak")) OBJ_DESCR(objects[i]) = "uzkiy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "rural helmet")) OBJ_DESCR(objects[i]) = "sel'skiy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "velvet pumps")) OBJ_DESCR(objects[i]) = "barkhatnyye nasosy";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hearing cloak")) OBJ_DESCR(objects[i]) = "plashch dlya slukha";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "calf-leather sandals")) OBJ_DESCR(objects[i]) = "sandalii iz telyach'yey kozhi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hacker helmet")) OBJ_DESCR(objects[i]) = "shlem khakera";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "velcro sandals")) OBJ_DESCR(objects[i]) = "sandalii na lipuchkakh";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "spellsucking cloak")) OBJ_DESCR(objects[i]) = "plashch zaklinaniy";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "princess gloves")) OBJ_DESCR(objects[i]) = "perchatki printsessy";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "uncanny gloves")) OBJ_DESCR(objects[i]) = "sverkh''yestestvennyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "strip bandana")) OBJ_DESCR(objects[i]) = "polosa bandanu";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "roadmap cloak")) OBJ_DESCR(objects[i]) = "plashch dorozhnoy karty";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "storm coat")) OBJ_DESCR(objects[i]) = "shtorm";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "pitch cloak")) OBJ_DESCR(objects[i]) = "smolyanoy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "buffalo boots")) OBJ_DESCR(objects[i]) = "buyvolovyye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fleeceling cloak")) OBJ_DESCR(objects[i]) = "pushistyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "heroine mocassins")) OBJ_DESCR(objects[i]) = "mokasiny dlya geroini";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "glaring cloak")) OBJ_DESCR(objects[i]) = "yarkiy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "greenery helmet")) OBJ_DESCR(objects[i]) = "zelonyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "odd hull cloak")) OBJ_DESCR(objects[i]) = "plashch s nechetnym korpusom";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "up-down cloak")) OBJ_DESCR(objects[i]) = "plashch s verkhnim plashchem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "parked gloves")) OBJ_DESCR(objects[i]) = "priparkovannyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "lead gloves")) OBJ_DESCR(objects[i]) = "svintsovyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "twisted visor helmet")) OBJ_DESCR(objects[i]) = "shlem vitoy shlema";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "occultism gloves")) OBJ_DESCR(objects[i]) = "perchatki okkul'tizma";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cyanism cloak")) OBJ_DESCR(objects[i]) = "plashch s tsianom";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "heap of shit boots")) OBJ_DESCR(objects[i]) = "kucha der'movykh sapog";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "bluy helmet")) OBJ_DESCR(objects[i]) = "siniy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "balanced boots")) OBJ_DESCR(objects[i]) = "sbalansirovannyye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "lolita boots")) OBJ_DESCR(objects[i]) = "botinki s lolitoy";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "digger gloves")) OBJ_DESCR(objects[i]) = "kopatel'skiye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "long-range cloak")) OBJ_DESCR(objects[i]) = "dlinnyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "inverse gloves")) OBJ_DESCR(objects[i]) = "obratnyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "weapon light boots")) OBJ_DESCR(objects[i]) = "legkiye botinki dlya oruzhiya";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "slaying gloves")) OBJ_DESCR(objects[i]) = "ubiystvennyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "less helmet")) OBJ_DESCR(objects[i]) = "men'she shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "flier cloak")) OBJ_DESCR(objects[i]) = "plashch letchika";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "engraved helmet")) OBJ_DESCR(objects[i]) = "gravirovannyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ill cloak")) OBJ_DESCR(objects[i]) = "bol'noy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "etheric helmet")) OBJ_DESCR(objects[i]) = "efirnyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "eternal helmet")) OBJ_DESCR(objects[i]) = "vechnyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "eternal cloak")) OBJ_DESCR(objects[i]) = "vechnyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "brick cloak")) OBJ_DESCR(objects[i]) = "kirpichnyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "etheric cloak")) OBJ_DESCR(objects[i]) = "efirnyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "brick helmet")) OBJ_DESCR(objects[i]) = "kirpichnyy shlem";

	}
	}

	}

	if (!issoviet && Race_if(PM_ANCIENT)) {

	obj_descr[SPE_FORCE_BOLT].oc_name = "kuch murvat";
	obj_descr[SPE_CREATE_MONSTER].oc_name = "hayvon yaratish";
	obj_descr[SPE_DRAIN_LIFE].oc_name = "drenaj hayot";
	obj_descr[SPE_COMMAND_UNDEAD].oc_name = "buyruq bo'lmagan o'lik";
	obj_descr[SPE_SUMMON_UNDEAD].oc_name = "bo'lmagan-o'lik chaqirish";
	obj_descr[SPE_DISINTEGRATION].oc_name = "parchalanish";
	obj_descr[SPE_STONE_TO_FLESH].oc_name = "go'sht uchun tosh";
	obj_descr[SPE_HEALING].oc_name = "davolash";
	obj_descr[SPE_CURE_BLINDNESS].oc_name = "qattiqlashishi ko'rlik";
	obj_descr[SPE_CURE_NUMBNESS].oc_name = "qattiqlashishi qattiq uyqu";
	obj_descr[SPE_BLIND_SELF].oc_name = "ko'r o'zini o'zi";
	obj_descr[SPE_CURE_SICKNESS].oc_name = "qattiqlashishi kasallik";
	obj_descr[SPE_CURE_HALLUCINATION].oc_name = "qattiqlashishi gallutsinatsiya";
	obj_descr[SPE_CURE_CONFUSION].oc_name = "qattiqlashishi tartibsizlik";
	obj_descr[SPE_CURE_BURN].oc_name = "qattiqlashishi kuygan";
	obj_descr[SPE_CONFUSE_SELF].oc_name = "o'zini o'zi aralashtirmang";
	obj_descr[SPE_CURE_STUN].oc_name = "qattiqlashishi bayiltici";
	obj_descr[SPE_STUN_SELF].oc_name = "bayiltici o'zini o'zi";
	obj_descr[SPE_EXTRA_HEALING].oc_name = "qo'shimcha davolash";
	obj_descr[SPE_FULL_HEALING].oc_name = "to'liq shifo";
	obj_descr[SPE_RESTORE_ABILITY].oc_name = "qobiliyatini tiklash";
	obj_descr[SPE_BANISHING_FEAR].oc_name = "voz kechishning qo'rquv";
	obj_descr[SPE_CURE_FREEZE].oc_name = "qattiqlashishi ishga solmay";
	obj_descr[SPE_CREATE_FAMILIAR].oc_name = "tanish yaratish";
	obj_descr[SPE_LIGHT].oc_name = "engil";
	obj_descr[SPE_DARKNESS].oc_name = "qorong'ilik";
	obj_descr[SPE_DETECT_MONSTERS].oc_name = "hayvonlar aniqlash";
	obj_descr[SPE_DETECT_FOOD].oc_name = "oziq-ovqat aniqlash";
	obj_descr[SPE_DISSOLVE_FOOD].oc_name = "oziq-ovqat tarqatib";
	obj_descr[SPE_DETECT_ARMOR_ENCHANTMENT].oc_name = "zirh sehr aniqlash";
	obj_descr[SPE_CLAIRVOYANCE].oc_name = "oldindan bilish";
	obj_descr[SPE_DETECT_UNSEEN].oc_name = "g'aybni aniqlash";
	obj_descr[SPE_IDENTIFY].oc_name = "aniqlash";
	obj_descr[SPE_DETECT_TREASURE].oc_name = "xazina aniqlash";
	obj_descr[SPE_MAGIC_MAPPING].oc_name = "sehrli xaritalash";
	obj_descr[SPE_ENTRAPPING].oc_name = "kabarciklarinin";
	obj_descr[SPE_FINGER].oc_name = "barmoq";
	obj_descr[SPE_CHEMISTRY].oc_name = "kimyo";
	obj_descr[SPE_DETECT_FOOT].oc_name = "oyoq aniqlash";
	obj_descr[SPE_FORBIDDEN_KNOWLEDGE].oc_name = "man ilm";
	obj_descr[SPE_CONFUSE_MONSTER].oc_name = "hayvon aralashtirmang";
	obj_descr[SPE_SLOW_MONSTER].oc_name = "sekin hayvon";
	obj_descr[SPE_CAUSE_FEAR].oc_name = "sabab qo'rquv";
	obj_descr[SPE_CHARM_MONSTER].oc_name = "jozibasi hayvon";
	obj_descr[SPE_ENCHANT_WEAPON].oc_name = "qurol sehrlamoq";
	obj_descr[SPE_ENCHANT_ARMOR].oc_name = "sovuti sehrlamoq";
	obj_descr[SPE_CHARGING].oc_name = "zaryad";
	obj_descr[SPE_GENOCIDE].oc_name = "irqiy qirg'inchilikni";
	obj_descr[SPE_PROTECTION].oc_name = "himoya";
	obj_descr[SPE_RESIST_POISON].oc_name = "zahar qarshilik";
	obj_descr[SPE_RESIST_SLEEP].oc_name = "uyqu qarshilik";
	obj_descr[SPE_ENDURE_COLD].oc_name = "sovuq sabr-toqat";
	obj_descr[SPE_ENDURE_HEAT].oc_name = "issiqlik bardosh";
	obj_descr[SPE_INSULATE].oc_name = "yakkalamoq";
	obj_descr[SPE_REMOVE_CURSE].oc_name = "la'natni olib tashlash";
	obj_descr[SPE_REMOVE_BLESSING].oc_name = "ne'matini olib tashlash";
	obj_descr[SPE_TURN_UNDEAD].oc_name = "bo'lmagan-o'lik o'girib";
	obj_descr[SPE_ANTI_DISINTEGRATION].oc_name = "piyodalarga parchalanish";
	obj_descr[SPE_BOTOX_RESIST].oc_name = "Oziq-ovqat zaharlanishi qarshilik";
	obj_descr[SPE_ACIDSHIELD].oc_name = "kislota qalqon";
	obj_descr[SPE_GODMODE].oc_name = "alloh xil";
	obj_descr[SPE_RESIST_PETRIFICATION].oc_name = "toshbo'ron qarshilik";
	obj_descr[SPE_JUMPING].oc_name = "o'tish";
	obj_descr[SPE_HASTE_SELF].oc_name = "shoshilish o'zini o'zi";
	obj_descr[SPE_ENLIGHTEN].oc_name = "ma'lumotlar";
	obj_descr[SPE_INVISIBILITY].oc_name = "ko'rinmas";
	obj_descr[SPE_AGGRAVATE_MONSTER].oc_name = "hayvon og'irlashtiradigan";
	obj_descr[SPE_LEVITATION].oc_name = "ko'rib ularni";
	obj_descr[SPE_TELEPORT_AWAY].oc_name = "nur uzoqda";
	obj_descr[SPE_PASSWALL].oc_name = "o'tkazgan devor";
	obj_descr[SPE_POLYMORPH].oc_name = "bir polimorfunun";
	obj_descr[SPE_MUTATION].oc_name = "o'zgarish";
	obj_descr[SPE_LEVELPORT].oc_name = "seminarlar darajasi";
	obj_descr[SPE_KNOCK].oc_name = "taqillatmoq";
	obj_descr[SPE_FLAME_SPHERE].oc_name = "olov shar";
	obj_descr[SPE_FREEZE_SPHERE].oc_name = "ishga solmay soha";
	obj_descr[SPE_SHOCKING_SPHERE].oc_name = "yomon soha";
	obj_descr[SPE_ACID_SPHERE].oc_name = "kislota soha";
	obj_descr[SPE_WIZARD_LOCK].oc_name = "ustasi qulf";
	obj_descr[SPE_DIG].oc_name = "chopmoq";
	obj_descr[SPE_CANCELLATION].oc_name = "bekor qilish";
	obj_descr[SPE_REFLECTION].oc_name = "aks";
	obj_descr[SPE_PARALYSIS].oc_name = "falaj";
	obj_descr[SPE_REPAIR_ARMOR].oc_name = "ta'mirlash zirh";
	obj_descr[SPE_CORRODE_METAL].oc_name = "metall yemoq";
	obj_descr[SPE_PETRIFY].oc_name = "toshga aylanmoq";
	obj_descr[SPE_MAGIC_MISSILE].oc_name = "sehrli raketa";
	obj_descr[SPE_FIREBALL].oc_name = "yong'in to'p";
	obj_descr[SPE_CONE_OF_COLD].oc_name = "sovuq konusning";
	obj_descr[SPE_SLEEP].oc_name = "uyqu";
	obj_descr[SPE_FINGER_OF_DEATH].oc_name = "o'lim nuridir";
	obj_descr[SPE_LIGHTNING].oc_name = "chaqmoq";
	obj_descr[SPE_POISON_BLAST].oc_name = "zahar portlash";
	obj_descr[SPE_ACID_STREAM].oc_name = "kislota oqimi";
	obj_descr[SPE_SOLAR_BEAM].oc_name = "quyosh nur";
	obj_descr[SPE_BLANK_PAPER].oc_name = "bo'sh qog'oz";
	obj_descr[SPE_STINKING_CLOUD].oc_name = "titilgan bulut";
	obj_descr[SPE_TIME_STOP].oc_name = "vaqt to'xtatish";
	obj_descr[SPE_MAP_LEVEL].oc_name = "xarita darajasi";
	obj_descr[SPE_GAIN_LEVEL].oc_name = "daromad darajasi";
	obj_descr[SPE_AMNESIA].oc_name = "xotira halok";
	obj_descr[SPE_REPAIR_WEAPON].oc_name = "ta'mirlash qurol";
	obj_descr[SPE_KNOW_ENCHANTMENT].oc_name = "sehr bilaman";
	obj_descr[SPE_MAGICTORCH].oc_name = "sehr mash'alasi";
	obj_descr[SPE_DISPLACEMENT].oc_name = "olish";
	obj_descr[SPE_MASS_HEALING].oc_name = "ommaviy shifo";
	obj_descr[SPE_TIME_SHIFT].oc_name = "vaqt o'zgarish";
	obj_descr[SPE_ALTER_REALITY].oc_name = "haqiqatni o'zgartirish";
	obj_descr[SPE_DISINTEGRATION_BEAM].oc_name = "parchalanish nur";
	obj_descr[SPE_FLYING].oc_name = "uchib";
	obj_descr[SPE_CHROMATIC_BEAM].oc_name = "rang nur";
	obj_descr[SPE_FUMBLING].oc_name = "paypaslab izlay oxirgi";
	obj_descr[SPE_MAKE_VISIBLE].oc_name = "paydo qilish";
	obj_descr[SPE_WARPING].oc_name = "coezgue";
	obj_descr[SPE_TRAP_CREATION].oc_name = "makr yaratish";
	obj_descr[SPE_STUN_MONSTER].oc_name = "bayiltici hayvon";
	obj_descr[SPE_CURSE_ITEMS].oc_name = "la'nat mahsulot";
	obj_descr[SPE_CHARACTER_RECURSION].oc_name = "belgilar oezyineleme";
	obj_descr[SPE_CLONE_MONSTER].oc_name = "klonudur hayvon";
	obj_descr[SPE_DESTROY_ARMOR].oc_name = "sovuti halok";
	obj_descr[SPE_INERTIA].oc_name = "dangasalik";
	obj_descr[SPE_TIME].oc_name = "vaqt";
	obj_descr[SPE_PSYBEAM].oc_name = "psixologik nur";
	obj_descr[SPE_HYPER_BEAM].oc_name = "hiper ish nur";
	obj_descr[SPE_FIRE_BOLT].oc_name = "yong'in mermilerinin";
	obj_descr[SPE_INFERNO].oc_name = "do'zax";
	obj_descr[SPE_ICE_BEAM].oc_name = "muz nur";
	obj_descr[SPE_THUNDER].oc_name = "momaqaldiroq";
	obj_descr[SPE_SLUDGE].oc_name = "baliqli ko'lining";
	obj_descr[SPE_TOXIC].oc_name = "zaharli";
	obj_descr[SPE_NETHER_BEAM].oc_name = "juda past nur";
	obj_descr[SPE_AURORA_BEAM].oc_name = "aurora o'rganish nur";
	obj_descr[SPE_GRAVITY_BEAM].oc_name = "agar tortishish kuchi nur";
	obj_descr[SPE_CHLOROFORM].oc_name = "xloroform";
	obj_descr[SPE_DREAM_EATER].oc_name = "egan orzu";
	obj_descr[SPE_BUBBLEBEAM].oc_name = "qabariq nur";
	obj_descr[SPE_GOOD_NIGHT].oc_name = "xayrli tun";
	obj_descr[SPE_FIXING].oc_name = "ekranga";

	obj_descr[SPE_CHAOS_TERRAIN].oc_name = "betartiblik yerlarni";
	obj_descr[SPE_RANDOM_SPEED].oc_name = "tasodifiy tezligi";
	obj_descr[SPE_VANISHING].oc_name = "barham topish davri";
	obj_descr[SPE_WISHING].oc_name = "xohlovchi";
	obj_descr[SPE_ACQUIREMENT].oc_name = "olish";
	obj_descr[SPE_CHAOS_BOLT].oc_name = "betartiblik murvat";
	obj_descr[SPE_HELLISH_BOLT].oc_name = "jahannamiy dardning asoratidan murvat";
	obj_descr[SPE_EARTHQUAKE].oc_name = "zilzila";
	obj_descr[SPE_LYCANTHROPY].oc_name = "bo'ri inson shakli";
	obj_descr[SPE_BUC_RANDOMIZATION].oc_name = "buc randomizasyon";
	obj_descr[SPE_LOCK_MANIPULATION].oc_name = "qulf hiyla";
	obj_descr[SPE_POLYFORM].oc_name = "belgigacha bo'lgan poli shakli";
	obj_descr[SPE_MESSAGE].oc_name = "xabar";
	obj_descr[SPE_RUMOR].oc_name = "mish-mish gaplar";
	obj_descr[SPE_CURE_RANDOM_STATUS].oc_name = "tasodifiy maqomini davolash";
	obj_descr[SPE_RESIST_RANDOM_ELEMENT].oc_name = "tasodifiy elementi qarshi";
	obj_descr[SPE_RUSSIAN_ROULETTE].oc_name = "rus ruletka";
	obj_descr[SPE_POSSESSION].oc_name = "egalik";
	obj_descr[SPE_TOTEM_SUMMONING].oc_name = "totemlarga chaqirishni";
	obj_descr[SPE_MIMICRY].oc_name = "mimika taqlid";
	obj_descr[SPE_HORRIFY].oc_name = "qo'rqitmoq";
	obj_descr[SPE_TERROR].oc_name = "kishi terror hujumini";
	obj_descr[SPE_PHASE_DOOR].oc_name = "bosqichi eshik";
	obj_descr[SPE_TRAP_DISARMING].oc_name = "qaytsin qurolsizlantirish";
	obj_descr[SPE_NEXUS_FIELD].oc_name = "aloqa faoliyat";
	obj_descr[SPE_COMMAND_DEMON].oc_name = "buyrug'i jin";
	obj_descr[SPE_FIRE_GOLEM].oc_name = "yong'in gol";
	obj_descr[SPE_DISRUPTION_SHIELD].oc_name = "buzilish qalqon";
	obj_descr[SPE_SPELLBINDER].oc_name = "majburiy imlo";
	obj_descr[SPE_TRACKER].oc_name = "kuzatishdan";
	obj_descr[SPE_INERTIA_CONTROL].oc_name = "inertsiya nazorat";
	obj_descr[SPE_CODE_EDITING].oc_name = "kodi tahrir";
	obj_descr[SPE_FORGOTTEN_SPELL].oc_name = "unutilgan afsun";
	obj_descr[SPE_FLOOD].oc_name = "suv toshqini";
	obj_descr[SPE_LAVA].oc_name = "lava oqimlarida";
	obj_descr[SPE_IRON_PRISON].oc_name = "temir qamoqxona";
	obj_descr[SPE_LOCKOUT].oc_name = "qulflash";
	obj_descr[SPE_CLOUDS].oc_name = "bulutlar";
	obj_descr[SPE_ICE].oc_name = "muz";
	obj_descr[SPE_GROW_TREES].oc_name = "daraxtlar o'sadi";
	obj_descr[SPE_DRIPPING_TREAD].oc_name = "terlaganda qadam";
	obj_descr[SPE_GEOLYSIS].oc_name = "geologik hal";
	obj_descr[SPE_ELEMENTAL_BEAM].oc_name = "tabiiy nur";
	obj_descr[SPE_STERILIZE].oc_name = "sterillamoq";
	obj_descr[SPE_WIND].oc_name = "shamol";
	obj_descr[SPE_FIRE].oc_name = "yong'in";
	obj_descr[SPE_ELEMENTAL_MINION].oc_name = "asosiy arzanda";
	obj_descr[SPE_WATER_BOLT].oc_name = "suv murvat";
	obj_descr[SPE_AIR_CURRENT].oc_name = "havo joriy";
	obj_descr[SPE_DASHING].oc_name = "qo'li bilan qo'ygan jimjimador";
	obj_descr[SPE_MELTDOWN].oc_name = "erish";
	obj_descr[SPE_POISON_BRAND].oc_name = "zahar brendi";
	obj_descr[SPE_STEAM_VENOM].oc_name = "bug 'zahar";
	obj_descr[SPE_HOLD_AIR].oc_name = "havo o'tkazadi";
	obj_descr[SPE_SWIMMING].oc_name = "suzish";
	obj_descr[SPE_VOLT_ROCK].oc_name = "sarma jinslar";
	obj_descr[SPE_WATER_FLAME].oc_name = "suv olov";
	obj_descr[SPE_AVALANCHE].oc_name = "qor ko'chimi";
	obj_descr[SPE_MANA_BOLT].oc_name = "sehrli murvat";
	obj_descr[SPE_ENERGY_BOLT].oc_name = "energiya murvat";
	obj_descr[SPE_ACID_INGESTION].oc_name = "kislota biriktirish";
	obj_descr[SPE_INDUCE_VOMITING].oc_name = "kusturun";
	obj_descr[SPE_REBOOT].oc_name = "qayta ishga tushirish";
	obj_descr[SPE_HOLY_SHIELD].oc_name = "muqaddas qalqon";

	obj_descr[SPE_FROST].oc_name = "ayoz";
	obj_descr[SPE_TRUE_SIGHT].oc_name = "haqiqiy ko'rish";
	obj_descr[SPE_BERSERK].oc_name = "g'azablanib";
	obj_descr[SPE_BLINDING_RAY].oc_name = "ko'r nurlarning";
	obj_descr[SPE_MAGIC_SHIELD].oc_name = "sehrli qalqon";
	obj_descr[SPE_WORLD_FALL].oc_name = "qiyomat";
	obj_descr[SPE_ESP].oc_name = "qo'shimcha hissiy idrok qilish";
	obj_descr[SPE_RADAR].oc_name = "aniqlovchi asbob";
	obj_descr[SPE_SEARCHING].oc_name = "puxta";
	obj_descr[SPE_INFRAVISION].oc_name = "infraqizil ko'rish";
	obj_descr[SPE_STEALTH].oc_name = "ayyorlik";
	obj_descr[SPE_CONFLICT].oc_name = "mojaro";
	obj_descr[SPE_REGENERATION].oc_name = "yangilanishi";
	obj_descr[SPE_FREE_ACTION].oc_name = "bepul aksiyalar";
	obj_descr[SPE_MULTIBEAM].oc_name = "ko'p nur";
	obj_descr[SPE_NO_EFFECT].oc_name = "hech ta'sir";
	obj_descr[SPE_SELFDESTRUCT].oc_name = "o'zini vayron";
	obj_descr[SPE_THUNDER_WAVE].oc_name = "momaqaldiroq to'lqin";
	obj_descr[SPE_BATTERING_RAM].oc_name = "quvvatli qo'chqor";
	obj_descr[SPE_BURROW].oc_name = "in";
	obj_descr[SPE_GAIN_CORRUPTION].oc_name = "korruptsiyaga qozonish";
	obj_descr[SPE_SWITCHEROO].oc_name = "aldamchi bekor qilish";
	obj_descr[SPE_THRONE_GAMBLE].oc_name = "taxt qimor";
	obj_descr[SPE_BACKFIRE].oc_name = "o'ziga qarshi ishlay boshlashi";
	obj_descr[SPE_DEMEMORIZE].oc_name = "endi yod";
	obj_descr[SPE_CALL_THE_ELEMENTS].oc_name = "elementlarni qo'ng'iroq";
	obj_descr[SPE_NATURE_BEAM].oc_name = "tabiat nur";
	obj_descr[SPE_WHISPERS_FROM_BEYOND].oc_name = "narigi vasvasa";
	obj_descr[SPE_STASIS].oc_name = "staz";
	obj_descr[SPE_CRYOGENICS].oc_name = "kriyojenikler";
	obj_descr[SPE_REDEMPTION].oc_name = "sotib";
	obj_descr[SPE_HYPERSPACE_SUMMON].oc_name = "hiper kosmik chaqirish";
	obj_descr[SPE_SATISFY_HUNGER].oc_name = "ochlik qondirish";
	obj_descr[SPE_RAIN_CLOUD].oc_name = "yomg'ir bulut";
	obj_descr[SPE_POWER_FAILURE].oc_name = "elektr etishmovchiligi";
	obj_descr[SPE_VAPORIZE].oc_name = "bug'lanmoq";
	obj_descr[SPE_TUNNELIZATION].oc_name = "harakati";
	obj_descr[SPE_BOMBING].oc_name = "bombardimon";
	obj_descr[SPE_DRAGON_BLOOD].oc_name = "ajdaho qon";
	obj_descr[SPE_ANTI_MAGIC_FIELD].oc_name = "qarshisehrli faoliyat";
	obj_descr[SPE_ANTI_MAGIC_SHELL].oc_name = "qarshisehrli qobiq";
	obj_descr[SPE_CURE_WOUNDED_LEGS].oc_name = "yaralangan oyoqlari davolash";
	obj_descr[SPE_ANGER_PEACEFUL_MONSTER].oc_name = "g'azab tinch maxluq";
	obj_descr[SPE_UNTAME_MONSTER].oc_name = "dovdirashlariga maxluq";
	obj_descr[SPE_UNLEVITATE].oc_name = "suzuvchi to'xtatish";
	obj_descr[SPE_DETECT_WATER].oc_name = "suv aniqlash";
	obj_descr[SPE_APPLY_NAIL_POLISH].oc_name = "tirnoq jilo amal";
	obj_descr[SPE_ENCHANT].oc_name = "sehrlamoqchi";
	obj_descr[SPE_DRY_UP_FOUNTAIN].oc_name = "bulog'ini quritib";
	obj_descr[SPE_TAKE_SELFIE].oc_name = "o'zingiz rasmini olib";
	obj_descr[SPE_SNIPER_BEAM].oc_name = "snayper nur";
	obj_descr[SPE_CURE_GLIB].oc_name = "davolash yengil";
	obj_descr[SPE_CURE_MONSTER].oc_name = "davolash maxluq";
	obj_descr[SPE_MANA_BATTERY].oc_name = "sehrli energiya batareya";
	obj_descr[SPE_THORNS].oc_name = "tikanlar";
	obj_descr[SPE_REROLL_ARTIFACT].oc_name = "parazit tasodifiy";
	obj_descr[SPE_FINAL_EXPLOSION].oc_name = "oxirgi portlash";
	obj_descr[SPE_CUTTING].oc_name = "kesish";
	obj_descr[SPE_CURE_DIM].oc_name = "qattiqlashishi xira";

	{

	register int i;
	register const char *s;

	for (i = 0; i < NUM_OBJECTS; i++) {
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "crude dagger")) OBJ_DESCR(objects[i]) = "xom xanjar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "obsidian dagger")) OBJ_DESCR(objects[i]) = "teaneck xanjar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "runed dagger")) OBJ_DESCR(objects[i]) = "yozilgan xanjar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "black runed dagger")) OBJ_DESCR(objects[i]) = "qora yozilgan xanjar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "very sharp tooth")) OBJ_DESCR(objects[i]) = "yirtqich tish";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "double-headed axe")) OBJ_DESCR(objects[i]) = "ikki boshli ax";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "broad pick")) OBJ_DESCR(objects[i]) = "keng nuqtada";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "crude short sword")) OBJ_DESCR(objects[i]) = "xom qisqa qilich";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "broad short sword")) OBJ_DESCR(objects[i]) = "keng qisqa qilich";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "runed short sword")) OBJ_DESCR(objects[i]) = "yozilgan qisqa qilich";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "obsidian short sword")) OBJ_DESCR(objects[i]) = "teaneck qisqa qilich";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "black runed short sword")) OBJ_DESCR(objects[i]) = "qora yozilgan qisqa qilich";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "runic broadsword")) OBJ_DESCR(objects[i]) = "runi keng dudama qilich";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "runed broadsword")) OBJ_DESCR(objects[i]) = "yozilgan keng dudama qilich";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "samurai sword")) OBJ_DESCR(objects[i]) = "yaponiya qilich";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "large obsidian sword")) OBJ_DESCR(objects[i]) = "katta teaneck qilich";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "long samurai sword")) OBJ_DESCR(objects[i]) = "uzoq yaponiya qilich";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "curved sword")) OBJ_DESCR(objects[i]) = "egri qilich";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "bent sword")) OBJ_DESCR(objects[i]) = "egilgan qilich";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "thonged club")) OBJ_DESCR(objects[i]) = "yaxshiroq klubi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "heavy club")) OBJ_DESCR(objects[i]) = "og'ir klubi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "staff")) OBJ_DESCR(objects[i]) = "xodimlar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "vulgar polearm")) OBJ_DESCR(objects[i]) = "qo'pol kutuplu qurol";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "single-edged polearm")) OBJ_DESCR(objects[i]) = "yagona xanjarday kutuplu qurol";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "forked polearm")) OBJ_DESCR(objects[i]) = "shoxlagan kutuplu qurol";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hilted polearm")) OBJ_DESCR(objects[i]) = "kutuplu qurol dasta";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "long poleaxe")) OBJ_DESCR(objects[i]) = "uzoq kutuplu bolta";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "pole cleaver")) OBJ_DESCR(objects[i]) = "kutuplu yoruvchi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "angled poleaxe")) OBJ_DESCR(objects[i]) = "ochiladigan kutuplu bolta";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "pole sickle")) OBJ_DESCR(objects[i]) = "kutuplu o'roq";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "pruning hook")) OBJ_DESCR(objects[i]) = "azizillo tuzoq";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hooked polearm")) OBJ_DESCR(objects[i]) = "hodisalar kutuplu qurol";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "pronged polearm")) OBJ_DESCR(objects[i]) = "tishli kutuplu qurol";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "beaked polearm")) OBJ_DESCR(objects[i]) = "tumshuqli kutuplu qurol";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "crude spear")) OBJ_DESCR(objects[i]) = "xom nayza";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "long obsidian spear")) OBJ_DESCR(objects[i]) = "uzoq teaneck nayza";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "runed spear")) OBJ_DESCR(objects[i]) = "yozilgan nayza";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "stout spear")) OBJ_DESCR(objects[i]) = "mustahkam nayza";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "throwing spear")) OBJ_DESCR(objects[i]) = "otib nayza";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "long gray spear")) OBJ_DESCR(objects[i]) = "uzoq kulrang nayza";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "obsidian lance")) OBJ_DESCR(objects[i]) = "teaneck lens";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "crude bow")) OBJ_DESCR(objects[i]) = "xom yoy";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "runed bow")) OBJ_DESCR(objects[i]) = "yozilgan yoy";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "black runed bow")) OBJ_DESCR(objects[i]) = "qora yozilgan yoy";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "long bow")) OBJ_DESCR(objects[i]) = "uzoq yoy";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "spider-legged bow")) OBJ_DESCR(objects[i]) = "o'rgimchak-oyoqli yoy";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "crude arrow")) OBJ_DESCR(objects[i]) = "xom o'q";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "runed arrow")) OBJ_DESCR(objects[i]) = "yozilgan o'q";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "black runed arrow")) OBJ_DESCR(objects[i]) = "qora yozilgan o'q";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "bamboo arrow")) OBJ_DESCR(objects[i]) = "bambuk o'q";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "obsidian arrow")) OBJ_DESCR(objects[i]) = "teaneck o'q";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "red stick")) OBJ_DESCR(objects[i]) = "qizil tayoq";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "spider-legged crossbow")) OBJ_DESCR(objects[i]) = "o'rgimchak-oyoqli yayli miltiq";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "obsidian bolt")) OBJ_DESCR(objects[i]) = "teaneck murvat";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "throwing star")) OBJ_DESCR(objects[i]) = "sharq yulduzi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "crested black plate")) OBJ_DESCR(objects[i]) = "tepeli qora plastinka";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "little blue vest")) OBJ_DESCR(objects[i]) = "oz ko'k libos";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "crested black mail")) OBJ_DESCR(objects[i]) = "tepeli qora pochta";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "crude chain mail")) OBJ_DESCR(objects[i]) = "xom zanjir pochta";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "crude ring mail")) OBJ_DESCR(objects[i]) = "xom ring pochta";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "red robe")) OBJ_DESCR(objects[i]) = "qizil to'n";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "blue robe")) OBJ_DESCR(objects[i]) = "ko'k libos";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "orange robe")) OBJ_DESCR(objects[i]) = "apelsin egnida xalat";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "green robe")) OBJ_DESCR(objects[i]) = "yashil rido";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "coarse mantelet")) OBJ_DESCR(objects[i]) = "qo'pol mantilya";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hooded cloak")) OBJ_DESCR(objects[i]) = "qaytarma qalpoqli plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "slippery cloak")) OBJ_DESCR(objects[i]) = "sirg'anchiq plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "faded pall")) OBJ_DESCR(objects[i]) = "nafas tobut latta";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cobwebbed cloak")) OBJ_DESCR(objects[i]) = "o'rgimchak to'rlari qoplab yotgan plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "white coat")) OBJ_DESCR(objects[i]) = "oq ko'ylagi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tattered cape")) OBJ_DESCR(objects[i]) = "yirtiq plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "dirty rag")) OBJ_DESCR(objects[i]) = "iflos latta";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "deadly cloak")) OBJ_DESCR(objects[i]) = "o'ldiradigan plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "opera cloak")) OBJ_DESCR(objects[i]) = "operasi plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "shaking cloak")) OBJ_DESCR(objects[i]) = "chayqab plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ornamental cope")) OBJ_DESCR(objects[i]) = "manzarali engish";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "vampiric cloak")) OBJ_DESCR(objects[i]) = "sindirishi plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "funeral cloak")) OBJ_DESCR(objects[i]) = "janoza plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "dragonhide cloak")) OBJ_DESCR(objects[i]) = "ajdaho yashirish plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ripped cloak")) OBJ_DESCR(objects[i]) = "yirtilib plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "metal cloak")) OBJ_DESCR(objects[i]) = "metall plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "granite cloak")) OBJ_DESCR(objects[i]) = "granit plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "asbestos cloak")) OBJ_DESCR(objects[i]) = "asbest plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "weaving cloak")) OBJ_DESCR(objects[i]) = "to'quv plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "second-hand cloak")) OBJ_DESCR(objects[i]) = "ikkinchi-qo'l plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "phantom cloak")) OBJ_DESCR(objects[i]) = "sharpa plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "icky cloak")) OBJ_DESCR(objects[i]) = "yoqimsiz plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "petrified cloak")) OBJ_DESCR(objects[i]) = "qotib plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "flax cloak")) OBJ_DESCR(objects[i]) = "zig'ir plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "glass cloak")) OBJ_DESCR(objects[i]) = "barometr plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hammered cloak")) OBJ_DESCR(objects[i]) = "bolg'alangan plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "voluminous cloak")) OBJ_DESCR(objects[i]) = "katta hajmli plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "mysterious cloak")) OBJ_DESCR(objects[i]) = "sirli plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "prolo cloak")) OBJ_DESCR(objects[i]) = "past-sinf plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "weird cloak")) OBJ_DESCR(objects[i]) = "g'alati plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "piece of cloth")) OBJ_DESCR(objects[i]) = "mato qism";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "leather hat")) OBJ_DESCR(objects[i]) = "charm shapka";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "little red hat")) OBJ_DESCR(objects[i]) = "oz qizil shapka";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "iron skull cap")) OBJ_DESCR(objects[i]) = "temir boshsuyagi idish";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hard hat")) OBJ_DESCR(objects[i]) = "qiyin shapka";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "spider shaped helm")) OBJ_DESCR(objects[i]) = "o'rgimchak shaklida rul";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "conical hat")) OBJ_DESCR(objects[i]) = "konusning shapka";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "plumed helmet")) OBJ_DESCR(objects[i]) = "qilishga zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "etched helmet")) OBJ_DESCR(objects[i]) = "kazinmis zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "crested helmet")) OBJ_DESCR(objects[i]) = "tepeli zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "golden helmet")) OBJ_DESCR(objects[i]) = "oltin dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "padded helmet")) OBJ_DESCR(objects[i]) = "yostiqli zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "steel helmet")) OBJ_DESCR(objects[i]) = "po'lat dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "black helmet")) OBJ_DESCR(objects[i]) = "qorong'u zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "horned helmet")) OBJ_DESCR(objects[i]) = "shoxli zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "winged helmet")) OBJ_DESCR(objects[i]) = "qanotli zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "viking helmet")) OBJ_DESCR(objects[i]) = "shimoliy zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "red helmet")) OBJ_DESCR(objects[i]) = "qizil zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "resounding helmet")) OBJ_DESCR(objects[i]) = "tovushning qaytishi zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "echo helmet")) OBJ_DESCR(objects[i]) = "sadosi zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "middle-earth helmet")) OBJ_DESCR(objects[i]) = "o'rta-yer dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gemmed helmet")) OBJ_DESCR(objects[i]) = "marvarid zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "spired helmet")) OBJ_DESCR(objects[i]) = "minora zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "basin helmet")) OBJ_DESCR(objects[i]) = "havzasi zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hydra helmet")) OBJ_DESCR(objects[i]) = "gidra zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "wok-shaped helmet")) OBJ_DESCR(objects[i]) = "wok shaklidagi dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "masked helmet")) OBJ_DESCR(objects[i]) = "niqobli zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tribal helmet")) OBJ_DESCR(objects[i]) = "qabila zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "internet helmet")) OBJ_DESCR(objects[i]) = "keng dunyo veb-zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "crazy helmet")) OBJ_DESCR(objects[i]) = "aqldan zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tight helmet")) OBJ_DESCR(objects[i]) = "qattiq zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "great helmet")) OBJ_DESCR(objects[i]) = "katta dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "deformed helmet")) OBJ_DESCR(objects[i]) = "quyon zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "rotating helmet")) OBJ_DESCR(objects[i]) = "qaytib zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "covered helmet")) OBJ_DESCR(objects[i]) = "qoplangan zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "filtered helmet")) OBJ_DESCR(objects[i]) = "filtrlangan zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "visored helmet")) OBJ_DESCR(objects[i]) = "soyabon zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "white gloves")) OBJ_DESCR(objects[i]) = "oq qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "old gloves")) OBJ_DESCR(objects[i]) = "eski qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "padded gloves")) OBJ_DESCR(objects[i]) = "yostiqli qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "slit gloves")) OBJ_DESCR(objects[i]) = "yoriq qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ice-cold gloves")) OBJ_DESCR(objects[i]) = "muz-sovuq qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "slippery gloves")) OBJ_DESCR(objects[i]) = "sirg'anchiq qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "riding gloves")) OBJ_DESCR(objects[i]) = "kopgina qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "brand-new gloves")) OBJ_DESCR(objects[i]) = "yangi qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "steel gloves")) OBJ_DESCR(objects[i]) = "po'lat qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "black gloves")) OBJ_DESCR(objects[i]) = "qora qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "mirrored gloves")) OBJ_DESCR(objects[i]) = "akslantirish qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "moth-bitten gloves")) OBJ_DESCR(objects[i]) = "kuya-chaqqan qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "graffiti gloves")) OBJ_DESCR(objects[i]) = "qo'lqop purkash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "warped gloves")) OBJ_DESCR(objects[i]) = "qiyshaygan qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "concrete gloves")) OBJ_DESCR(objects[i]) = "aniq qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "nondescript gloves")) OBJ_DESCR(objects[i]) = "zerikarli qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "spiky gloves")) OBJ_DESCR(objects[i]) = "qirrali qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "grey-shaded gloves")) OBJ_DESCR(objects[i]) = "kulrang-soyali qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "spacey gloves")) OBJ_DESCR(objects[i]) = "kosmik qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "paper gloves")) OBJ_DESCR(objects[i]) = "qog'oz qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "vampiric gloves")) OBJ_DESCR(objects[i]) = "sindirishi qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "transparent gloves")) OBJ_DESCR(objects[i]) = "shaffof qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fencing gloves")) OBJ_DESCR(objects[i]) = "qilichbozlik qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "blue and green shield")) OBJ_DESCR(objects[i]) = "ko'k va yashil qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "white-handed shield")) OBJ_DESCR(objects[i]) = "to'g'ri qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "red-eyed shield")) OBJ_DESCR(objects[i]) = "qizil ko'zli qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "large round shield")) OBJ_DESCR(objects[i]) = "katta dumaloq qalqoni";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "dark red shield")) OBJ_DESCR(objects[i]) = "quyuq qizil qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "dark blue shield")) OBJ_DESCR(objects[i]) = "quyuq ko'k qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "dull yellow shield")) OBJ_DESCR(objects[i]) = "zerikarli sariq qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "dark green shield")) OBJ_DESCR(objects[i]) = "quyuq yashil qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "shiny shield")) OBJ_DESCR(objects[i]) = "yorqin qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "slippery shield")) OBJ_DESCR(objects[i]) = "sirg'anchiq qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gray dragonhide shield")) OBJ_DESCR(objects[i]) = "kulrang muhim qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "silver dragonhide shield")) OBJ_DESCR(objects[i]) = "kumush muhim qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "mercurial dragonhide shield")) OBJ_DESCR(objects[i]) = "simob muhim qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "shimmering dragonhide shield")) OBJ_DESCR(objects[i]) = "jimirlagan muhim qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "deep dragonhide shield")) OBJ_DESCR(objects[i]) = "chuqur muhim qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "red dragonhide shield")) OBJ_DESCR(objects[i]) = "qizil muhim qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "white dragonhide shield")) OBJ_DESCR(objects[i]) = "oq muhim qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "orange dragonhide shield")) OBJ_DESCR(objects[i]) = "apelsin muhim qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "black dragonhide shield")) OBJ_DESCR(objects[i]) = "qora muhim qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "blue dragonhide shield")) OBJ_DESCR(objects[i]) = "ko'k muhim qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "green dragonhide shield")) OBJ_DESCR(objects[i]) = "yashil muhim qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "golden dragonhide shield")) OBJ_DESCR(objects[i]) = "oltin muhim qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "stone dragonhide shield")) OBJ_DESCR(objects[i]) = "tosh muhim qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cyan dragonhide shield")) OBJ_DESCR(objects[i]) = "osmon muhim qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "yellow dragonhide shield")) OBJ_DESCR(objects[i]) = "sariq muhim qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "platinum dragonhide shield")) OBJ_DESCR(objects[i]) = "platina ajdaho yashir qalqoni";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "brass dragonhide shield")) OBJ_DESCR(objects[i]) = "jez ajdaho yashir qalqoni";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "copper dragonhide shield")) OBJ_DESCR(objects[i]) = "mis ajdaho yashir qalqoni";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "emerald dragonhide shield")) OBJ_DESCR(objects[i]) = "zumrad ajdaho yashir qalqoni";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ruby dragonhide shield")) OBJ_DESCR(objects[i]) = "yoqut ajdaho yashir qalqoni";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sapphire dragonhide shield")) OBJ_DESCR(objects[i]) = "ko'kish yoqut ajdaho yashir qalqoni";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "diamond dragonhide shield")) OBJ_DESCR(objects[i]) = "olmos ajdaho yashir qalqoni";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "amethyst dragonhide shield")) OBJ_DESCR(objects[i]) = "ametist ajdaho yashir qalqoni";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "purple dragonhide shield")) OBJ_DESCR(objects[i]) = "safsar ajdaho yashir qalqoni";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "polished silver shield")) OBJ_DESCR(objects[i]) = "sayqallangan kumush qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "white boots")) OBJ_DESCR(objects[i]) = "oq chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "walking shoes")) OBJ_DESCR(objects[i]) = "poyabzal yurish";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hard shoes")) OBJ_DESCR(objects[i]) = "qattiq poyafzali";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "little black boots")) OBJ_DESCR(objects[i]) = "oz qora chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "jackboots")) OBJ_DESCR(objects[i]) = "kundalik erkak iskarpin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "heeled sandals")) OBJ_DESCR(objects[i]) = "yuqori choriq";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "soft footwear")) OBJ_DESCR(objects[i]) = "yumshoq poyafzal";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "womens footwear")) OBJ_DESCR(objects[i]) = "ayollar poyafzal";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "light footwear")) OBJ_DESCR(objects[i]) = "engil poyafzal";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "female footwear")) OBJ_DESCR(objects[i]) = "ayol poyafzal";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "asian footwear")) OBJ_DESCR(objects[i]) = "osiyolik poyafzal";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "red overknees")) OBJ_DESCR(objects[i]) = "tiz ustida qizil";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "heeled boots")) OBJ_DESCR(objects[i]) = "to'pi chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "combat boots")) OBJ_DESCR(objects[i]) = "jangovar chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "thigh boots")) OBJ_DESCR(objects[i]) = "son chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "jungle boots")) OBJ_DESCR(objects[i]) = "o'rmon chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hiking boots")) OBJ_DESCR(objects[i]) = "yurish chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "battle boots")) OBJ_DESCR(objects[i]) = "urush chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "buckled boots")) OBJ_DESCR(objects[i]) = "quyon chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "mud boots")) OBJ_DESCR(objects[i]) = "loy chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "steel boots")) OBJ_DESCR(objects[i]) = "po'lat chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "riding boots")) OBJ_DESCR(objects[i]) = "kopgina chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "persian boots")) OBJ_DESCR(objects[i]) = "fors chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hot boots")) OBJ_DESCR(objects[i]) = "issiq chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "explosive boots")) OBJ_DESCR(objects[i]) = "portlovchi chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "profiled boots")) OBJ_DESCR(objects[i]) = "profilli chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "irregular boots")) OBJ_DESCR(objects[i]) = "tartibsizlik chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cast iron boots")) OBJ_DESCR(objects[i]) = "temir etiklar tashlash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "winter boots")) OBJ_DESCR(objects[i]) = "qish chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "torn boots")) OBJ_DESCR(objects[i]) = "yirtiq chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "chess boots")) OBJ_DESCR(objects[i]) = "shaxmat chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "plateau boots")) OBJ_DESCR(objects[i]) = "platosi chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "jade boots")) OBJ_DESCR(objects[i]) = "yashma chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "snow boots")) OBJ_DESCR(objects[i]) = "qor chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "bag")) OBJ_DESCR(objects[i]) = "yostig'i";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "key")) OBJ_DESCR(objects[i]) = "kalit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "candle")) OBJ_DESCR(objects[i]) = "sham";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "lamp")) OBJ_DESCR(objects[i]) = "chiroq";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "whistle")) OBJ_DESCR(objects[i]) = "hushtak";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "flute")) OBJ_DESCR(objects[i]) = "nay";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "horn")) OBJ_DESCR(objects[i]) = "shox";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "harp")) OBJ_DESCR(objects[i]) = "harb";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "drum")) OBJ_DESCR(objects[i]) = "do'mbira";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "iron hook")) OBJ_DESCR(objects[i]) = "temir tuzoq";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "lightsaber")) OBJ_DESCR(objects[i]) = "nur qilichi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "double lightsaber")) OBJ_DESCR(objects[i]) = "ikki kishilik nur qilichi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "looking glass")) OBJ_DESCR(objects[i]) = "qarab shisha";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "glass orb")) OBJ_DESCR(objects[i]) = "shisha gumbaz";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "leather bag")) OBJ_DESCR(objects[i]) = "teri xalta";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "highly-complicated electronic device")) OBJ_DESCR(objects[i]) = "yuqori murakkab elektron qurilma";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "two-wired thing")) OBJ_DESCR(objects[i]) = "ikki-simli narsa";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "three-wired thing")) OBJ_DESCR(objects[i]) = "uch-simli narsa";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "many-wired thing")) OBJ_DESCR(objects[i]) = "ko'p-simli narsa";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "box containing little plastic cards")) OBJ_DESCR(objects[i]) = "oz plastik kartochkalar o'z ichiga olgan qutisi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "four-wired thing")) OBJ_DESCR(objects[i]) = "to'rt-simli narsa";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "box of obscure-looking glass devices")) OBJ_DESCR(objects[i]) = "qorong'i-qarab shisha asboblar qutisi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "candelabrum")) OBJ_DESCR(objects[i]) = "qandil";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "silver bell")) OBJ_DESCR(objects[i]) = "kumush qo'ng'iroq";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "splash of venom")) OBJ_DESCR(objects[i]) = "zahari sachratmoq";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "large silver axe")) OBJ_DESCR(objects[i]) = "katta kumush bolta";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gray short sword")) OBJ_DESCR(objects[i]) = "kul qisqa qilich";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "silver monk's staff")) OBJ_DESCR(objects[i]) = "kumush rohiblik xodimlari";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "wide conical hat")) OBJ_DESCR(objects[i]) = "keng konusning shapka";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "metal disk")) OBJ_DESCR(objects[i]) = "metall diskda";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "green bowel")) OBJ_DESCR(objects[i]) = "yashil ichak";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fish bowl")) OBJ_DESCR(objects[i]) = "baliq piyola";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "armored black dress")) OBJ_DESCR(objects[i]) = "zirhli qora libos";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "loud foppish suit")) OBJ_DESCR(objects[i]) = "baland havoyi kostyum";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gemstone-adorned clothing")) OBJ_DESCR(objects[i]) = "nodir tosh-bezatilgan kiyim";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "clean white clothes")) OBJ_DESCR(objects[i]) = "toza oq kiyim";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "silver clothes")) OBJ_DESCR(objects[i]) = "kumush kiyim";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tight black clothes")) OBJ_DESCR(objects[i]) = "qattiq qora libos";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "apron")) OBJ_DESCR(objects[i]) = "etak";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "lion skin")) OBJ_DESCR(objects[i]) = "sher teri";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "glass shield")) OBJ_DESCR(objects[i]) = "shisha qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fighting gloves")) OBJ_DESCR(objects[i]) = "jang qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "glass boots")) OBJ_DESCR(objects[i]) = "shisha chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "beaked axe")) OBJ_DESCR(objects[i]) = "tumshuqli bolta";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "flower-pommeled broadsword")) OBJ_DESCR(objects[i]) = "gul-o'yma keng dudama qilich";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "huge sword")) OBJ_DESCR(objects[i]) = "katta qilich";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "black polearm")) OBJ_DESCR(objects[i]) = "qora kutuplu qo'l";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "rod")) OBJ_DESCR(objects[i]) = "tayog'i";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "beaked staff")) OBJ_DESCR(objects[i]) = "tumshuqli xodimlari";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "headpiece")) OBJ_DESCR(objects[i]) = "ustki qismi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "round helmet")) OBJ_DESCR(objects[i]) = "dumaloq zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "yellow helmet")) OBJ_DESCR(objects[i]) = "sariq zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "orange visored helmet")) OBJ_DESCR(objects[i]) = "apelsin soyabon zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tasseled helmet")) OBJ_DESCR(objects[i]) = "popukli zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "dim colored helmet")) OBJ_DESCR(objects[i]) = "rangli zarbdan xira";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "chain-aventailed helmet")) OBJ_DESCR(objects[i]) = "zanjir maxsus soyabon dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "scuba helmet")) OBJ_DESCR(objects[i]) = "tueplue zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "radio helmet")) OBJ_DESCR(objects[i]) = "uzatuvchi zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "orange suit")) OBJ_DESCR(objects[i]) = "apelsin kostyum";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "red leather armor")) OBJ_DESCR(objects[i]) = "qizil charm zirh";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "little black dress")) OBJ_DESCR(objects[i]) = "oz qora libos";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "white dress")) OBJ_DESCR(objects[i]) = "oq libos";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "black men's suit")) OBJ_DESCR(objects[i]) = "qora erkaklar kostyum";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "camouflage patterned clothes")) OBJ_DESCR(objects[i]) = "kamuflyaj naqshli kiyim";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "strange rings")) OBJ_DESCR(objects[i]) = "g'alati uzuk";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tightly fitting suit")) OBJ_DESCR(objects[i]) = "mahkam uydirma kostyum";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "red mantle")) OBJ_DESCR(objects[i]) = "qizil mantiya";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "wings")) OBJ_DESCR(objects[i]) = "qanotlari";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "brown burnoose")) OBJ_DESCR(objects[i]) = "jigarrang tozluklar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "vestiary")) OBJ_DESCR(objects[i]) = "kiyim";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "long overcoat")) OBJ_DESCR(objects[i]) = "uzoq palto";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gaberdine")) OBJ_DESCR(objects[i]) = "uchun gabardin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "livid worped-moon shield")) OBJ_DESCR(objects[i]) = "g'azablangan qiyshaygan-oy qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "polished shield")) OBJ_DESCR(objects[i]) = "sayqallangan qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "rainbow dragonhide shield")) OBJ_DESCR(objects[i]) = "kamalak ajdaho teri qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "blood dragonhide shield")) OBJ_DESCR(objects[i]) = "qon ajdaho teri qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "plain dragonhide shield")) OBJ_DESCR(objects[i]) = "tekis ajdaho teri qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sky dragonhide shield")) OBJ_DESCR(objects[i]) = "havo ajdaho teri qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "water dragonhide shield")) OBJ_DESCR(objects[i]) = "suv ajdaho teri qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "orange gloves")) OBJ_DESCR(objects[i]) = "apelsin qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tightly fitting gloves")) OBJ_DESCR(objects[i]) = "mahkam uydirma qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "light gloves")) OBJ_DESCR(objects[i]) = "engil qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "camouflage patterned gloves")) OBJ_DESCR(objects[i]) = "kamuflyaj naqshli qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "odd pair of gloves")) OBJ_DESCR(objects[i]) = "qo'lqop g'alati juftligi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "even pair of gloves")) OBJ_DESCR(objects[i]) = "qo'lqop ham juft";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "covered pair of gloves")) OBJ_DESCR(objects[i]) = "qo'lqop qoplangan juft";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "boxing gloves")) OBJ_DESCR(objects[i]) = "boks qo'lqoplari";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "turning shoes")) OBJ_DESCR(objects[i]) = "poyabzal o'girilib";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "rainbow boots")) OBJ_DESCR(objects[i]) = "kamalak chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tightly fitting boots")) OBJ_DESCR(objects[i]) = "mahkam uydirma chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "roller boots")) OBJ_DESCR(objects[i]) = "rolikli chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "camouflage patterned boots")) OBJ_DESCR(objects[i]) = "kamuflyaj naqshli chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "orange boots")) OBJ_DESCR(objects[i]) = "apelsin chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "platform boots")) OBJ_DESCR(objects[i]) = "platformalar chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "side gore boots")) OBJ_DESCR(objects[i]) = "yon gor chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "side zip boots")) OBJ_DESCR(objects[i]) = "yon bir zip chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cone-shaped helmet")) OBJ_DESCR(objects[i]) = "konusning-pokiza zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "dark helmet")) OBJ_DESCR(objects[i]) = "qorong'u zarbdan";

		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "velcro boots")) OBJ_DESCR(objects[i]) = "cirt chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "complete helmet")) OBJ_DESCR(objects[i]) = "to'liq dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "weeb cloak")) OBJ_DESCR(objects[i]) = "yaponiya ucube rido";

		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "silver circular blade")) OBJ_DESCR(objects[i]) = "kumush dairesel bir pichoq";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "runed curved sword")) OBJ_DESCR(objects[i]) = "yodimga tushyapti gvardiyasi qilich";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "wide metal helm")) OBJ_DESCR(objects[i]) = "keng metall elkan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "runed helm")) OBJ_DESCR(objects[i]) = "yodimga tushyapti elkan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "expensive clothes")) OBJ_DESCR(objects[i]) = "qimmat kiyim";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "expensive dress")) OBJ_DESCR(objects[i]) = "qimmat liboslar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "long-sleeved jacket")) OBJ_DESCR(objects[i]) = "uzoq qisma ko'ylagi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "flowery shorts and lei")) OBJ_DESCR(objects[i]) = "gullarni shortilar va ley";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "wooden box")) OBJ_DESCR(objects[i]) = "yog'och qutisini";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "wooden container")) OBJ_DESCR(objects[i]) = "yog'och idish";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cooling box")) OBJ_DESCR(objects[i]) = "sovutish qutisini";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "psychic dragonhide shield")) OBJ_DESCR(objects[i]) = "ruhiy ajdar yashirish qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "spinning helmet")) OBJ_DESCR(objects[i]) = "yigiruv dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "disgusting cloak")) OBJ_DESCR(objects[i]) = "jirkanch plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "comfortable gloves")) OBJ_DESCR(objects[i]) = "qulay qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "yellow gloves")) OBJ_DESCR(objects[i]) = "sariq qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fleecy boots")) OBJ_DESCR(objects[i]) = "tozalamoq chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "space helmet")) OBJ_DESCR(objects[i]) = "kosmik dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "godless cloak")) OBJ_DESCR(objects[i]) = "xudosiz plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "guzzle helmet")) OBJ_DESCR(objects[i]) = "zarbdan ko'p yemoq";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ghostly cloak")) OBJ_DESCR(objects[i]) = "diniy plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "water-pipe helmet")) OBJ_DESCR(objects[i]) = "suv-quvur dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "thick-soled boots")) OBJ_DESCR(objects[i]) = "qalin-asoslangan chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "mechanical helmet")) OBJ_DESCR(objects[i]) = "mexanik dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "wafer-thin cloak")) OBJ_DESCR(objects[i]) = "juda ham yupqa plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "polnish gloves")) OBJ_DESCR(objects[i]) = "polsha qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "clunky heels")) OBJ_DESCR(objects[i]) = "qisqa ko'chirish to'piqlarni";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "wedge boots")) OBJ_DESCR(objects[i]) = "xanjar chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "translucent helmet")) OBJ_DESCR(objects[i]) = "shaffof dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "faded mittens")) OBJ_DESCR(objects[i]) = "so'nib sportchisi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "jarring cloak")) OBJ_DESCR(objects[i]) = "g'azablantiradigan plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "politician cloak")) OBJ_DESCR(objects[i]) = "siyosatchi plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "meteoric helmet")) OBJ_DESCR(objects[i]) = "meteorik dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hugging boots")) OBJ_DESCR(objects[i]) = "havola etdi chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "void cloak")) OBJ_DESCR(objects[i]) = "haqiqiy emas plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "anachorononononaut helmet")) OBJ_DESCR(objects[i]) = "eskilik sarqiti dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fin boots")) OBJ_DESCR(objects[i]) = "kanatcik chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "straitjacket cloak")) OBJ_DESCR(objects[i]) = "tor kamzul plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "angelic cloak")) OBJ_DESCR(objects[i]) = "farishtalarning plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "full helmet")) OBJ_DESCR(objects[i]) = "bilan to'la dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "evil cloak")) OBJ_DESCR(objects[i]) = "ham yomon plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "winter stilettos")) OBJ_DESCR(objects[i]) = "qish sandal chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "aluminium helmet")) OBJ_DESCR(objects[i]) = "alyuminiy dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "demonic cloak")) OBJ_DESCR(objects[i]) = "jinlarning plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "anti-government helmet")) OBJ_DESCR(objects[i]) = "aksil-hukumat dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "clumsy gloves")) OBJ_DESCR(objects[i]) = "qo'pol qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ankle boots")) OBJ_DESCR(objects[i]) = "bilagi zo'r chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "frequent helmet")) OBJ_DESCR(objects[i]) = "tez-tez zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "RNG helmet")) OBJ_DESCR(objects[i]) = "RNG dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "block-heeled boots")) OBJ_DESCR(objects[i]) = "blok-o'tish chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "awesome gloves")) OBJ_DESCR(objects[i]) = "ajoyib qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "shrouded cloak")) OBJ_DESCR(objects[i]) = "kafan plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fingerless gloves")) OBJ_DESCR(objects[i]) = "kam qo'lqop barmoq";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "energizer cloak")) OBJ_DESCR(objects[i]) = "energiya plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "mantle of coat")) OBJ_DESCR(objects[i]) = "ko'ylagi mantiya";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "chilling cloak")) OBJ_DESCR(objects[i]) = "sovutgichli plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "bug-tracking helmet")) OBJ_DESCR(objects[i]) = "hasharotlar-kuzatish dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fatal gloves")) OBJ_DESCR(objects[i]) = "halokatli qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "beautiful heels")) OBJ_DESCR(objects[i]) = "chiroyli ko'chirish to'piqlarni";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "electrostatic cloak")) OBJ_DESCR(objects[i]) = "elektrofizikaviy kompyuteringizda ornatilgan plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "weeping helmet")) OBJ_DESCR(objects[i]) = "yig'lab dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "runic gloves")) OBJ_DESCR(objects[i]) = "runi qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "roman sandals")) OBJ_DESCR(objects[i]) = "rim fuqarosi kavushlari";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "homicidal cloak")) OBJ_DESCR(objects[i]) = "halokatli plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "narrow helmet")) OBJ_DESCR(objects[i]) = "tor dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "spanish gloves")) OBJ_DESCR(objects[i]) = "ispaniya qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "castlevania boots")) OBJ_DESCR(objects[i]) = "qal'a vania chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "greek cloak")) OBJ_DESCR(objects[i]) = "yunon plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "celtic helmet")) OBJ_DESCR(objects[i]) = "seltik dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "english gloves")) OBJ_DESCR(objects[i]) = "ingliz tili qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "korean sandals")) OBJ_DESCR(objects[i]) = "janubiy koreyaning kavushlari";

		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "forgetful cloak")) OBJ_DESCR(objects[i]) = "unutuvchan plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "spider boots")) OBJ_DESCR(objects[i]) = "o'rgimchak chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "yellow cloak")) OBJ_DESCR(objects[i]) = "sariq plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "changing cloak")) OBJ_DESCR(objects[i]) = "o'zgaruvchan plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "black cloak")) OBJ_DESCR(objects[i]) = "qora rido";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "blue cloak")) OBJ_DESCR(objects[i]) = "ko'k plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "red cloak")) OBJ_DESCR(objects[i]) = "qizil plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "green cloak")) OBJ_DESCR(objects[i]) = "yashil rido";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "mauve cloak")) OBJ_DESCR(objects[i]) = "qizg'ish binafsharang plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "purple cloak")) OBJ_DESCR(objects[i]) = "safsar plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "dark cloak")) OBJ_DESCR(objects[i]) = "tushunarsiz plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "shell cloak")) OBJ_DESCR(objects[i]) = "qobiq plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "chinese cloak")) OBJ_DESCR(objects[i]) = "xitoy plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gray cloak")) OBJ_DESCR(objects[i]) = "kulrang plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "polyform cloak")) OBJ_DESCR(objects[i]) = "belgigacha bo'lgan poli shakli plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "absorbing cloak")) OBJ_DESCR(objects[i]) = "yutucu plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cyan cloak")) OBJ_DESCR(objects[i]) = "camgoebegi plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "deep cloak")) OBJ_DESCR(objects[i]) = "chuqur plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "blinking cloak")) OBJ_DESCR(objects[i]) = "miltillovchi plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tone cloak")) OBJ_DESCR(objects[i]) = "ohang plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "running cloak")) OBJ_DESCR(objects[i]) = "ishlayotgan plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "pink cloak")) OBJ_DESCR(objects[i]) = "portlash-pushti plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "birthcloth")) OBJ_DESCR(objects[i]) = "tug'ilgan mato";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "colorless cloak")) OBJ_DESCR(objects[i]) = "rangsiz plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "frade mantle")) OBJ_DESCR(objects[i]) = "farasingiz mantiya";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "heavy cloak")) OBJ_DESCR(objects[i]) = "og'ir plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "grass cloak")) OBJ_DESCR(objects[i]) = "o't plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "contaminated coat")) OBJ_DESCR(objects[i]) = "ifloslangan palto";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "withered cloak")) OBJ_DESCR(objects[i]) = "shol plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "white cloak")) OBJ_DESCR(objects[i]) = "oq rido";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ignorant cloak")) OBJ_DESCR(objects[i]) = "johil plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "avenger cloak")) OBJ_DESCR(objects[i]) = "qasoskor plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gravity cloak")) OBJ_DESCR(objects[i]) = "agar tortishish kuchi plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "wishful cloak")) OBJ_DESCR(objects[i]) = "istalgan plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "poke mongo cloak")) OBJ_DESCR(objects[i]) = "soktudun mongo plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "levuntation cloak")) OBJ_DESCR(objects[i]) = "havo rido kiygan suzadi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "enduring cloak")) OBJ_DESCR(objects[i]) = "bardoshli plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "minded cloak")) OBJ_DESCR(objects[i]) = "fikrlovchi plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "quicktravel cloak")) OBJ_DESCR(objects[i]) = "tez safar plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "geek cloak")) OBJ_DESCR(objects[i]) = "qani plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "orange cloak")) OBJ_DESCR(objects[i]) = "apelsin plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "pastel cloak")) OBJ_DESCR(objects[i]) = "rang soya plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "nurse cloak")) OBJ_DESCR(objects[i]) = "hamshira plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "slexual cloak")) OBJ_DESCR(objects[i]) = "jinsiy plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "angband cloak")) OBJ_DESCR(objects[i]) = "o'rta yer qal'a plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fleecy-colored cloak")) OBJ_DESCR(objects[i]) = "serjun rangli plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "concrete cloak")) OBJ_DESCR(objects[i]) = "aniq plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "anorexia cloak")) OBJ_DESCR(objects[i]) = "eb buzilishi plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "flash cloak")) OBJ_DESCR(objects[i]) = "bir flesh plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "magenta cloak")) OBJ_DESCR(objects[i]) = "qirmizi plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "icy cloak")) OBJ_DESCR(objects[i]) = "muzli plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "starwars cloak")) OBJ_DESCR(objects[i]) = "yulduz urushlar plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tunnel cloak")) OBJ_DESCR(objects[i]) = "harakati plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "dnethack cloak")) OBJ_DESCR(objects[i]) = "hamzindon va dunyo bo'lmagan doirasi so'yish plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "barley cloak")) OBJ_DESCR(objects[i]) = "arpa plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "taiga cloak")) OBJ_DESCR(objects[i]) = "shimoliy o'rmon plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "boxing gown")) OBJ_DESCR(objects[i]) = "boks libosi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "team splat cloak")) OBJ_DESCR(objects[i]) = "jamoasi xavfsizlik plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "eldritch cloak")) OBJ_DESCR(objects[i]) = "aql bovar qilmaydigan plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "yellow robe")) OBJ_DESCR(objects[i]) = "sariq to'n";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "purple robe")) OBJ_DESCR(objects[i]) = "safsar to'n";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "white robe")) OBJ_DESCR(objects[i]) = "oq to'n";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "black robe")) OBJ_DESCR(objects[i]) = "qora to'n";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gray robe")) OBJ_DESCR(objects[i]) = "kulrang to'n";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cyan robe")) OBJ_DESCR(objects[i]) = "feruza rang to'n";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "octarine robe")) OBJ_DESCR(objects[i]) = "sakkizinchi rang to'n";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "brown robe")) OBJ_DESCR(objects[i]) = "jigarrang to'n";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "camo robe")) OBJ_DESCR(objects[i]) = "kamuflaj to'n";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "pink robe")) OBJ_DESCR(objects[i]) = "pushti to'n";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "inka cloak")) OBJ_DESCR(objects[i]) = "inka plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "viva cloak")) OBJ_DESCR(objects[i]) = "viva plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "saddle")) OBJ_DESCR(objects[i]) = "binicilik vositasi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "leash")) OBJ_DESCR(objects[i]) = "uyda oziqlangan vositasi";

		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "living boots")) OBJ_DESCR(objects[i]) = "turmush chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "rain boots")) OBJ_DESCR(objects[i]) = "yomg'ir chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "silk helmet")) OBJ_DESCR(objects[i]) = "ipak dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tar gloves")) OBJ_DESCR(objects[i]) = "sol qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "musical helmet")) OBJ_DESCR(objects[i]) = "musiqiy dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "poor helmet")) OBJ_DESCR(objects[i]) = "kambag'al dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gray helmet")) OBJ_DESCR(objects[i]) = "kulrang dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "radiant helmet")) OBJ_DESCR(objects[i]) = "yorqin dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "secret helmet")) OBJ_DESCR(objects[i]) = "yashirin dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "inkcoat helmet")) OBJ_DESCR(objects[i]) = "siyoh palto dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "knowledgeable helmet")) OBJ_DESCR(objects[i]) = "bilimdon dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "organic helmet")) OBJ_DESCR(objects[i]) = "organik dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "formula one helmet")) OBJ_DESCR(objects[i]) = "formula bir zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "arduous helmet")) OBJ_DESCR(objects[i]) = "qiyinchiliklar bilan to'la dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "long-hair helmet")) OBJ_DESCR(objects[i]) = "uzoq soch dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "meso helmet")) OBJ_DESCR(objects[i]) = "mezo dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "difficult cloak")) OBJ_DESCR(objects[i]) = "qiyin plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ugly cloak")) OBJ_DESCR(objects[i]) = "chirkin bir plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "armored cloak")) OBJ_DESCR(objects[i]) = "zirhli plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "inky cloak")) OBJ_DESCR(objects[i]) = "tim qora rido";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gentle cloak")) OBJ_DESCR(objects[i]) = "muloyim plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "living cloak")) OBJ_DESCR(objects[i]) = "tirik plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "irradiation cloak")) OBJ_DESCR(objects[i]) = "nurlanish plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "silk cloak")) OBJ_DESCR(objects[i]) = "ipak rido";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "heavenly cloak")) OBJ_DESCR(objects[i]) = "samoviy plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "soft cloak")) OBJ_DESCR(objects[i]) = "yumshoq plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "bituminous mantle")) OBJ_DESCR(objects[i]) = "suvab mantiya";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "stone cloak")) OBJ_DESCR(objects[i]) = "tashlagan tosh plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "secretion cloak")) OBJ_DESCR(objects[i]) = "yuqumli plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "excrement cloak")) OBJ_DESCR(objects[i]) = "chiqindi plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hungry cloak")) OBJ_DESCR(objects[i]) = "chanqoq plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "science cloak")) OBJ_DESCR(objects[i]) = "ilm-fan plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "guild cloak")) OBJ_DESCR(objects[i]) = "birlik plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tower cloak")) OBJ_DESCR(objects[i]) = "minora plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "arcane cloak")) OBJ_DESCR(objects[i]) = "urug'dan plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "organic cloak")) OBJ_DESCR(objects[i]) = "organik plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "natural cloak")) OBJ_DESCR(objects[i]) = "tabiiy plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "compost cloak")) OBJ_DESCR(objects[i]) = "kompost plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "erotic boots")) OBJ_DESCR(objects[i]) = "erotik chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sputa boots")) OBJ_DESCR(objects[i]) = "sputa chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "arcane boots")) OBJ_DESCR(objects[i]) = "urug'dan chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "street boots")) OBJ_DESCR(objects[i]) = "ko'cha chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "heavy boots")) OBJ_DESCR(objects[i]) = "og'ir etiklari";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gentle boots")) OBJ_DESCR(objects[i]) = "yumshoq chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "nature boots")) OBJ_DESCR(objects[i]) = "tabiat chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "radiant heels")) OBJ_DESCR(objects[i]) = "yorqin ko'chirish to'piqlarni";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "turbo boots")) OBJ_DESCR(objects[i]) = "qidiruvi va turbo chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "area boots")) OBJ_DESCR(objects[i]) = "maydoni chizilmasin";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sexy heels")) OBJ_DESCR(objects[i]) = "belgila sexy ko'chirish to'piqlarni";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "stroking boots")) OBJ_DESCR(objects[i]) = "etiklar silay";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "velvet gloves")) OBJ_DESCR(objects[i]) = "baxmal qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "nonexistant gloves")) OBJ_DESCR(objects[i]) = "yo'q qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "racer gloves")) OBJ_DESCR(objects[i]) = "poygachi qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "inka gloves")) OBJ_DESCR(objects[i]) = "inka qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "viva gloves")) OBJ_DESCR(objects[i]) = "viva qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "poor gloves")) OBJ_DESCR(objects[i]) = "kambag'al qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "shitty gloves")) OBJ_DESCR(objects[i]) = "boktan qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "natural gloves")) OBJ_DESCR(objects[i]) = "tabiiy qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sensor gloves")) OBJ_DESCR(objects[i]) = "tayinlangan qurilmani qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "evil dragonhide shield")) OBJ_DESCR(objects[i]) = "yomon ajdaho yashir qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "magic dragonhide shield")) OBJ_DESCR(objects[i]) = "sehr ajdaho yashir qalqon";

		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "blue sneakers")) OBJ_DESCR(objects[i]) = "ko'k shippak";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "dusty gloves")) OBJ_DESCR(objects[i]) = "chang qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "woven cloak")) OBJ_DESCR(objects[i]) = "to'qilgan plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "colorfade cloak")) OBJ_DESCR(objects[i]) = "ranglash plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "femmy boots")) OBJ_DESCR(objects[i]) = "nazokat etigi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "kitchen hat")) OBJ_DESCR(objects[i]) = "oshxona xet";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "dream helmet")) OBJ_DESCR(objects[i]) = "dubulg'a orzu";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "red sneakers")) OBJ_DESCR(objects[i]) = "qizil shippak";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "residential cloak")) OBJ_DESCR(objects[i]) = "uy-joy plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "yellow sneakers")) OBJ_DESCR(objects[i]) = "sariq shippak";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fertile cloak")) OBJ_DESCR(objects[i]) = "unumdor plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "pink sneakers")) OBJ_DESCR(objects[i]) = "pushti shippak";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "grimace helmet")) OBJ_DESCR(objects[i]) = "face dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sharp-edged sandals")) OBJ_DESCR(objects[i]) = "o'tkir xanjarday kavushlari";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ruined helmet")) OBJ_DESCR(objects[i]) = "vayron dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ski heels")) OBJ_DESCR(objects[i]) = "chang'i poshnalar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "slowing gown")) OBJ_DESCR(objects[i]) = "sekinlashuvi libos";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "bleached cloak")) OBJ_DESCR(objects[i]) = "oqartirilgan plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "foundry cloak")) OBJ_DESCR(objects[i]) = "quyish plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fetish heels")) OBJ_DESCR(objects[i]) = "but poshnalar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "reliance cloak")) OBJ_DESCR(objects[i]) = "ishonch plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "rubynus helmet")) OBJ_DESCR(objects[i]) = "yoqut asosiy dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "thinking helmet")) OBJ_DESCR(objects[i]) = "fikr dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "silk fingerlings")) OBJ_DESCR(objects[i]) = "ipak va ling, barmoqlar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "netradiation helmet")) OBJ_DESCR(objects[i]) = "sof radiatsiya dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "skinny cloak")) OBJ_DESCR(objects[i]) = "oriq plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "rural helmet")) OBJ_DESCR(objects[i]) = "qishloq dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "velvet pumps")) OBJ_DESCR(objects[i]) = "duxoba nasoslar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hearing cloak")) OBJ_DESCR(objects[i]) = "eshitish plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "calf-leather sandals")) OBJ_DESCR(objects[i]) = "buzoq-charm kavushlari";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hacker helmet")) OBJ_DESCR(objects[i]) = "kompyuter dolandirici dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "velcro sandals")) OBJ_DESCR(objects[i]) = "cirt kavushlari";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "spellsucking cloak")) OBJ_DESCR(objects[i]) = "so'rib imlo plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "princess gloves")) OBJ_DESCR(objects[i]) = "malika qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "uncanny gloves")) OBJ_DESCR(objects[i]) = "dahshatli qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "strip bandana")) OBJ_DESCR(objects[i]) = "bir ip yengil bosh kiyim";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "roadmap cloak")) OBJ_DESCR(objects[i]) = "yo'l xaritasi plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "storm coat")) OBJ_DESCR(objects[i]) = "bo'ron palto";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "pitch cloak")) OBJ_DESCR(objects[i]) = "hotva plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "buffalo boots")) OBJ_DESCR(objects[i]) = "qo'tos botlarni";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fleeceling cloak")) OBJ_DESCR(objects[i]) = "serjunrangli plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "heroine mocassins")) OBJ_DESCR(objects[i]) = "qahramoni mokasen";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "glaring cloak")) OBJ_DESCR(objects[i]) = "qo'pol plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "greenery helmet")) OBJ_DESCR(objects[i]) = "o'simliklar dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "odd hull cloak")) OBJ_DESCR(objects[i]) = "g'alati po'st plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "up-down cloak")) OBJ_DESCR(objects[i]) = "up-pastga plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "parked gloves")) OBJ_DESCR(objects[i]) = "to'xtaganda qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "lead gloves")) OBJ_DESCR(objects[i]) = "qo'rg'oshin qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "twisted visor helmet")) OBJ_DESCR(objects[i]) = "buekuemlue soyabon dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "occultism gloves")) OBJ_DESCR(objects[i]) = "folbinlik qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cyanism cloak")) OBJ_DESCR(objects[i]) = "ko'k zaharlanish plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "heap of shit boots")) OBJ_DESCR(objects[i]) = "boktan etik to'p";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "bluy helmet")) OBJ_DESCR(objects[i]) = "bluy dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "balanced boots")) OBJ_DESCR(objects[i]) = "muvozanatli etigi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "lolita boots")) OBJ_DESCR(objects[i]) = "bosh ketish etigi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "digger gloves")) OBJ_DESCR(objects[i]) = "kazici qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "long-range cloak")) OBJ_DESCR(objects[i]) = "uzoq masofaga plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "inverse gloves")) OBJ_DESCR(objects[i]) = "teskari qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "weapon light boots")) OBJ_DESCR(objects[i]) = "qurol engil etigi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "slaying gloves")) OBJ_DESCR(objects[i]) = "o'ldirish qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "less helmet")) OBJ_DESCR(objects[i]) = "kam dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "flier cloak")) OBJ_DESCR(objects[i]) = "uchuvchi plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "engraved helmet")) OBJ_DESCR(objects[i]) = "o'yilgan dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ill cloak")) OBJ_DESCR(objects[i]) = "kasal plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "etheric helmet")) OBJ_DESCR(objects[i]) = "eter kaskasi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "eternal helmet")) OBJ_DESCR(objects[i]) = "abadiy dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "eternal cloak")) OBJ_DESCR(objects[i]) = "abadiy plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "brick cloak")) OBJ_DESCR(objects[i]) = "g'isht plashi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "etheric cloak")) OBJ_DESCR(objects[i]) = "eterik plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "brick helmet")) OBJ_DESCR(objects[i]) = "g'isht kaskasi";

	}
	}

	}

	if ((Role_if(PM_ACTIVISTOR) || Race_if(PM_PEACEMAKER) || Role_if(PM_MYSTIC)) && new_game) {

		int ammount;
		ammount = 0;

		while (ammount < 5) {

		switch (rnd(51)) {

		case 1: 
		case 2: 
		case 3: 
		    HFire_resistance |= FROMOUTSIDE; break;
		case 4: 
		case 5: 
		case 6: 
		    HCold_resistance |= FROMOUTSIDE; break;
		case 7: 
		case 8: 
		case 9: 
		    HSleep_resistance |= FROMOUTSIDE; break;
		case 10: 
		case 11: 
		    HDisint_resistance |= FROMOUTSIDE; break;
		case 12: 
		case 13: 
		case 14: 
		    HShock_resistance |= FROMOUTSIDE; break;
		case 15: 
		case 16: 
		case 17: 
		    HPoison_resistance |= FROMOUTSIDE; break;
		case 18: 
		    HDrain_resistance |= FROMOUTSIDE; break;
		case 19: 
		    HSick_resistance |= FROMOUTSIDE; break;
		case 20: 
		    HAcid_resistance |= FROMOUTSIDE; break;
		case 21: 
		case 22: 
		    HHunger |= FROMOUTSIDE; break;
		case 23: 
		case 24: 
		    HSee_invisible |= FROMOUTSIDE; break;
		case 25: 
		    HTelepat |= FROMOUTSIDE; break;
		case 26: 
		case 27: 
		    HWarning |= FROMOUTSIDE; break;
		case 28: 
		case 29: 
		    HSearching |= FROMOUTSIDE; break;
		case 30: 
		case 31: 
		    HStealth |= FROMOUTSIDE; break;
		case 32: 
		case 33: 
		    HAggravate_monster |= FROMOUTSIDE; break;
		case 34: 
		    HConflict |= FROMOUTSIDE; break;
		case 35: 
		case 36: 
		    HTeleportation |= FROMOUTSIDE; break;
		case 37: 
		    HTeleport_control |= FROMOUTSIDE; break;
		case 38: 
		    HFlying |= FROMOUTSIDE; break;
		case 39: 
		    HSwimming |= FROMOUTSIDE; break;
		case 40: 
		    HMagical_breathing |= FROMOUTSIDE; break;
		case 41: 
		    HSlow_digestion |= FROMOUTSIDE; break;
		case 42: 
		case 43: 
		    HRegeneration |= FROMOUTSIDE; break;
		case 44: 
		    HPolymorph |= FROMOUTSIDE; break;
		case 45: 
		    HPolymorph_control |= FROMOUTSIDE; break;
		case 46: 
		case 47: 
		    HFast |= FROMOUTSIDE; break;
		case 48: 
		    HInvis |= FROMOUTSIDE; break;
		case 49: 
		    HManaleech |= FROMOUTSIDE; break;
		default:
			break;
			}
		ammount++;

		}
	}

	if (flags.wonderland && new_game) {
		flags.lostsoul = flags.uberlostsoul = FALSE;
		goto_level(&elderpriest_level, TRUE, FALSE, FALSE);
		pline("Welcome to Wonderland. You have to get to the bottom of the Yendorian Tower to escape.");
		pline("If you manage to do that, you regain your ability to levelport. Good luck, and don't get near the elder priest or he will tentacle to tentacle you!");

		/* Apparently the game is so stupid and doesn't know what a teleport region is, and still occasionally
		 * spawns you inside the temple! This should not be, and therefore you're teleported out if it happens. */
		if (*in_rooms(u.ux, u.uy, TEMPLE)) {
			pline("S'Wonderful!");
			(void) safe_teleds(FALSE);
			/* still in the temple? try again, but prevent infinite loops */
			while (rn2(1000) && *in_rooms(u.ux, u.uy, TEMPLE)) (void) safe_teleds(FALSE);
		}

	}

	if (flags.lostsoul && !flags.uberlostsoul && new_game) { 
	goto_level(&medusa_level, TRUE, FALSE, FALSE); /* inspired by Tome, an Angband mod --Amy */
	pline("These are the halls of Mandos... err, Medusa. Good luck making your way back up!");
	}

	if (flags.uberlostsoul && new_game) { 

		flags.lostsoul = FALSE;

	goto_level((&sanctum_level - 1), TRUE, FALSE, FALSE);
	pline("These are the halls of Mandos... err, Gehennom. Looks nice, huh?");

			        register int newlev = 74;
				d_level newlevel;
				get_level(&newlevel, newlev);
				goto_level(&newlevel, TRUE, FALSE, FALSE);
				pline("Enjoy your stay, and try to get out if you can.");


	}

	if (Role_if(PM_TRANSVESTITE) && new_game && flags.female) {
		    makeknown(AMULET_OF_CHANGE);
		    You("don't feel like being female!");
			change_sex();
		    flags.botl = 1;

	}

	if (Role_if(PM_LADIESMAN) && new_game && flags.female) {
		    makeknown(AMULET_OF_CHANGE);
		    You("don't feel like being female!");
			change_sex();
		    flags.botl = 1;

	}

	if (Role_if(PM_TOPMODEL) && new_game && !flags.female) {
		    makeknown(AMULET_OF_CHANGE);
		    You("don't feel like being male!");
			change_sex();
		    flags.botl = 1;

	}

	if (Role_if(PM_DOLL_MISTRESS) && new_game && !flags.female) {
		    makeknown(AMULET_OF_CHANGE);
		    You("don't feel like being male!");
			change_sex();
		    flags.botl = 1;

	}

	if (Race_if(PM_UNGENOMOLD) && new_game) {
		  makeknown(SCR_GENOCIDE);
	    polyself(FALSE);
		mvitals[PM_UNGENOMOLD].mvflags |= (G_GENOD|G_NOCORPSE);
	    pline("Wiped out all ungenomolds.");
 		You_feel("dead inside.");

	}

	u.stethocheat = moves;
	init_uasmon();

	if (!new_game && issoviet) {

		/* In Soviet Russia, modders simply assume that everything the Amy does is bullshit. They do not actually
		 * think before reverting her changes, they just do it. And so they reintroduce the old bug that caused
		 * monster spawn increase timers to be re-initialized every time the player saves and restores. It is simply
		 * unbelievable. --Amy */

	      u.monstertimeout = rnz(10000)+rnz(15000);
		while (u.monstertimeout < 10) u.monstertimeout = rnz(10000)+rnz(15000);
	      u.monstertimefinish = rnz(10000)+rnz(20000)+u.monstertimeout;
		while (u.monstertimefinish < 20) u.monstertimefinish = rnz(10000)+rnz(20000)+u.monstertimeout;

	}

	if (!new_game && !wizard && !discover && (u.hangupcheat >= 666)) {

		discover = TRUE;
		pline("Filthy hangup cheater! The ability to enter explore mode after death is not meant to be abused! Be glad that I'm lenient and allow you to play on in explore mode instead of simply killing you outright... --Amy");

	}

	if (!new_game && u.hangupcheat) {

		u.hangupamount++;

	}

#ifdef HANGUPPENALTY
	if (!new_game && (u.hangupcheat > 2)) { /* filthy cheater! */

		u.ublesscnt += rnz(2000);
		change_luck(-3);

		u.ualign.sins += 5;
		u.alignlim -= 5;
	      adjalign(-100);

		nomul(-(5 + u.hangupparalysis), "paralyzed by severe hangup cheating", FALSE);

		u.uhpmax -= rnd(20);
		if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		if (u.uhpmax < 1) {
		    u.youaredead = 1;
		    killer = "critical existence failure";
		    killer_format = KILLED_BY;
		    done(DIED);
		    u.youaredead = 0;
		}
		u.uenmax -= rnd(20);
		if (u.uenmax < 0) {
			u.uenmax = u.uen = 0;
		}
		if (Upolyd) {
			u.mhmax -= rnd(50);
			if (u.mh > u.mhmax) u.mh = u.mhmax;
			if (u.mhmax < 1) rehumanize();
		}

		u.hangupcheat--;
		u.hangupparalysis = 0;

	}
#endif /* HANGUPPENALTY */

/* Hanging up at an instadeath should always kill you upon restore --Amy */
	if (!new_game && u.youaredead) {

		pline("Apparently you decided to be a wise-guy and hang up to prevent an instakill from affecting you. Unfortunately for you though, the Amy decided to fix that oversight, and since you should actually have died already, it's game over for you now.");
		killer = "trying to circumvent an instadeath via hangup cheating";
		killer_format = KILLED_BY;
		done(DIED);

		/* lifesaved */
		You_feel("like a filthy cheater.");
		/* I could add luck penalties or whatnot, but meh. You've hanged up anyway, and are thus most probably
		 * gonna eat the hangup penalties below :P */
		u.youaredead = 0;

	}

	if (u.segfaultpanic) {
		u.youaredead = 1;
		pline("Stack corruption panic dumped to desktop SIGSEGV c0000005.");
		killer = "the dreaded segfault panic";
		killer_format = KILLED_BY;
		done(DIED);

		/* lifesaved */
		pline("There's no escape - you'll just crash again!");
		u.youaredead = 0;

	}

	if (!new_game && u.hangupcheat) {

#ifdef HANGUPPENALTY
		pline("You hanged up during your last session! Since I can't determine whether you did that to cheat, you will now be paralyzed, slowed and have your max HP/Pw reduced. Please save your game normally next time! --Amy");
		if (multi >= 0) nomul(-(2 + u.hangupparalysis), "paralyzed by trying to hangup cheat", FALSE);

		u.ublesscnt += rnz(300);
		change_luck(-1);

		u.ualign.sins++;
		u.alignlim--;
	      adjalign(-10);

		u.uhpmax -= rnd(5);
		if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		if (u.uhpmax < 1) {
		    u.youaredead = 1;
		    killer = "critical existence failure";
		    killer_format = KILLED_BY;
		    done(DIED);
		    u.youaredead = 0;
		}
		u.uenmax -= rnd(5);
		if (u.uenmax < 0) {
			u.uenmax = u.uen = 0;
		}
		if (Upolyd) {
			u.mhmax -= rnd(10);
			if (u.mh > u.mhmax) u.mh = u.mhmax;
			if (u.mhmax < 1) rehumanize();
		}

		u.hangupcheat = 0;
		u.hangupparalysis = 0;
#else
		pline("You hanged up during your last session! As an anti-cheat measure, you're paralyzed for a few turns, and your act of hanging up is being tracked. Your dumplog file will show how many times you've hanged up and if your ascension dumplog shows 200 hangups, everyone will know what you've been up to... But as long as you didn't actually try to cheat, there will be no other consequences because I give you the benefit of the doubt (after all, your internet connection might just have died, and it would be unfair to penalize you for that).");
		pline("But if I discover, by watching your ttyrec, that you were actually cheating, I can also recompile the game with the HANGUPPENALTY flag defined, and then you'll actually start getting severe penalties for every hangup. In really severe cases I might even put your username on a blacklist, meaning that specifically you would get penalties while others would not. So, better be a honest player and then I don't have to take such measures. Anyway, have fun playing!  --Amy");
		if (multi >= 0) nomul(-(2 + u.hangupparalysis), "paralyzed by trying to hangup cheat", FALSE);
		u.hangupcheat = 0;
		u.hangupparalysis = 0;
#endif
	}

	#ifdef LIVELOGFILE
	/* Start live reporting */
		  livelog_start();
	#endif
}

#ifdef POSITIONBAR
STATIC_DCL void
do_positionbar()
{
	static char pbar[COLNO];
	char *p;
	
	p = pbar;
	/* up stairway */
	if (upstair.sx &&
#ifdef DISPLAY_LAYERS
	   (level.locations[upstair.sx][upstair.sy].mem_bg == S_upstair ||
	    level.locations[upstair.sx][upstair.sy].mem_bg == S_upladder)) {
#else
	   (glyph_to_cmap(level.locations[upstair.sx][upstair.sy].glyph) ==
	    S_upstair ||
 	    glyph_to_cmap(level.locations[upstair.sx][upstair.sy].glyph) ==
	    S_upladder)) {
#endif
		*p++ = '<';
		*p++ = upstair.sx;
	}
	if (sstairs.sx &&
#ifdef DISPLAY_LAYERS
	   (level.locations[sstairs.sx][sstairs.sy].mem_bg == S_upstair ||
	    level.locations[sstairs.sx][sstairs.sy].mem_bg == S_upladder)) {
#else
	   (glyph_to_cmap(level.locations[sstairs.sx][sstairs.sy].glyph) ==
	    S_upstair ||
 	    glyph_to_cmap(level.locations[sstairs.sx][sstairs.sy].glyph) ==
	    S_upladder)) {
#endif
		*p++ = '<';
		*p++ = sstairs.sx;
	}

	/* down stairway */
	if (dnstair.sx &&
#ifdef DISPLAY_LAYERS
	   (level.locations[dnstair.sx][dnstair.sy].mem_bg == S_dnstair ||
	    level.locations[dnstair.sx][dnstair.sy].mem_bg == S_dnladder)) {
#else
	   (glyph_to_cmap(level.locations[dnstair.sx][dnstair.sy].glyph) ==
	    S_dnstair ||
 	    glyph_to_cmap(level.locations[dnstair.sx][dnstair.sy].glyph) ==
	    S_dnladder)) {
#endif
		*p++ = '>';
		*p++ = dnstair.sx;
	}
	if (sstairs.sx &&
#ifdef DISPLAY_LAYERS
	   (level.locations[sstairs.sx][sstairs.sy].mem_bg == S_dnstair ||
	    level.locations[sstairs.sx][sstairs.sy].mem_bg == S_dnladder)) {
#else
	   (glyph_to_cmap(level.locations[sstairs.sx][sstairs.sy].glyph) ==
	    S_dnstair ||
 	    glyph_to_cmap(level.locations[sstairs.sx][sstairs.sy].glyph) ==
	    S_dnladder)) {
#endif
		*p++ = '>';
		*p++ = sstairs.sx;
	}

	/* hero location */
	if (u.ux) {
		*p++ = '@';
		*p++ = u.ux;
	}
	/* fence post */
	*p = 0;

	update_positionbar(pbar);
}
#endif

#if defined(REALTIME_ON_BOTL) || defined (RECORD_REALTIME)
time_t
get_realtime(void)
{
    time_t curtime;

    /* Get current time */
#if defined(BSD) && !defined(POSIX_TYPES)
    (void) time((long *)&curtime);
#else
    (void) time(&curtime);
#endif

    /* Since the timer isn't set until the game starts, this prevents us
     * from displaying nonsense on the bottom line before it does. */
    if(realtime_data.restoretime == 0) {
        curtime = realtime_data.realtime;
    } else {
        curtime -= realtime_data.restoretime;
        curtime += realtime_data.realtime;
    }
 
    return curtime;
}
#endif /* REALTIME_ON_BOTL || RECORD_REALTIME */

/* if enough game time has elapsed, things in the player's favor happen less often --Amy */
boolean
timebasedlowerchance()
{
	int chance = 115;
	chance -= (moves * 100 / u.monstertimefinish);
	/* make sure we don't fall off the bottom */
	if (chance < 15) chance = 15;

	if (moves > 10000) {
		chance *= 9;
		chance /= 10;
	}

	if (moves > 20000) {
		chance *= 9;
		chance /= 10;
	}

	if (moves > 30000) {
		chance *= 9;
		chance /= 10;
	}

	if (moves > 40000) {
		chance *= 9;
		chance /= 10;
	}

	if (moves > 50000) {
		chance *= 9;
		chance /= 10;
	}

	if (moves > 60000) {
		chance *= 9;
		chance /= 10;
	}

	if (moves > 70000) {
		chance *= 9;
		chance /= 10;
	}

	if (moves > 80000) {
		chance *= 9;
		chance /= 10;
	}

	if (moves > 90000) {
		chance *= 9;
		chance /= 10;
	}

	if (moves > 100000) {
		chance *= 9;
		chance /= 10;
	}

	if (chance < 5) chance = 5;

	if (chance > rnd(100)) return(TRUE);
	else return(FALSE);
}

/* contamination check: reduced HP/Pw regeneration depending on how contaminated the player is --Amy */
boolean
contaminationcheck()
{
	if (u.contamination) {
		if (u.contamination >= 1000 && rn2(10)) return 1;
		else if (u.contamination >= 800 && u.contamination < 1000 && (rnd(10) > 3)) return 1;
		else if (u.contamination >= 600 && u.contamination < 800 && rn2(2)) return 1;
		else if (u.contamination >= 400 && u.contamination < 600 && (rnd(10) > 7)) return 1;
		else if (u.contamination >= 200 && u.contamination < 400 && !rn2(10)) return 1;
	}

	/* if you're in the Sewer Plant and have to breathe, your regeneration is also reduced */
	if (In_sewerplant(&u.uz) && rn2(2) && !Breathless) return 1;

	return 0;
}

#endif /* OVLB */

/*allmain.c*/
