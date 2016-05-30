/*	SCCS Id: @(#)allmain.c	3.4	2003/04/02	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* various code that was replicated in *main.c */

#include "hack.h"
#ifndef NO_SIGNAL
#include <signal.h>
#endif
#ifdef SHORT_FILENAMES
#include "patchlev.h"
#else
#include "patchlevel.h"
#endif

#ifdef POSITIONBAR
STATIC_DCL void NDECL(do_positionbar);
#endif

STATIC_PTR int NDECL(unfaintX);

#define decrnknow(spell)	spl_book[spell].sp_know--
#define spellid(spell)		spl_book[spell].sp_id
#define spellknow(spell)	spl_book[spell].sp_know

#ifdef OVL0

STATIC_PTR
int
unfaintX()
{
	(void) Hear_again();
	stop_occupation();
	return 0;
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
	You("are lucky!  Full moon tonight.");
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
	if(didmove) {
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
			if (u.uevent.udemigod || u.uprops[STORM_HELM].extrinsic) {
				monclock = 15;
			} else {
				if (depth(&u.uz) > depth(&stronghold_level)) {
					monclock = 50;
				}
				past_clock = moves - timeout_start;
				if (past_clock > 0) {
					monclock -= past_clock*55/clock_base;
				}
			}
			/* make sure we don't fall off the bottom */
			if (monclock < 15) { monclock = 15; }
			if (verisiertEffect || u.uprops[VERISIERTEFFECT].extrinsic || have_verisiertstone()) monclock /= 5;
			if (ishaxor) monclock /= 2;
			if (Race_if(PM_LICH_WARRIOR)) monclock /= 2;
			if (Race_if(PM_RODNEYAN)) monclock /= 4;
			if (issuxxor) monclock *= 2;

			/* here, we really need a fail safe --Amy */
			if (monclock < 2) monclock = 2;

			/* TODO: adj difficulty in makemon */
			if (!rn2(monclock) && !ishomicider ) {
				if ( (u.uevent.udemigod || u.uprops[STORM_HELM].extrinsic) && xupstair && rn2(10)) {
					(void) makemon((struct permonst *)0, xupstair, yupstair, MM_ADJACENTOK);
				} else {
					(void) makemon((struct permonst *)0, 0, 0, NO_MM_FLAGS);
				}
			}

			if (!rn2(monclock) && ishomicider ) makerandomtrap();

			xtraclock = 40000;
			if (u.uevent.udemigod || u.uprops[STORM_HELM].extrinsic) {
				xtraclock = 12000;
			} else {
				if (depth(&u.uz) > depth(&stronghold_level)) {
					xtraclock = 28000;
				}
				past_clock = moves - timeout_start;
				if (past_clock > 0) {
					xtraclock -= past_clock*28000/clock_base;
				}
			}
			/* make sure we don't fall off the bottom */
			if (xtraclock < 12000) { xtraclock = 12000; }
			if (verisiertEffect || u.uprops[VERISIERTEFFECT].extrinsic || have_verisiertstone()) xtraclock /= 5;
			if (ishaxor) xtraclock /= 2;
			if (Race_if(PM_LICH_WARRIOR)) xtraclock /= 2;
			if (Race_if(PM_RODNEYAN)) xtraclock /= 4;
			if (issuxxor) xtraclock *= 2;

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

			if (!rn2(ishaxor ? 1500 : 3000) && !issoviet) {

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

			if (!rn2(ishaxor ? 50000 : 100000) && !issoviet) {

				randsp = (rn2(14) + 2);
				if (!rn2(10)) randsp *= 2;
				if (!rn2(100)) randsp *= 3;
				if (!rn2(1000)) randsp *= 5;
				if (!rn2(10000)) randsp *= 10;
				if (randsp > 1) randsp = rnd(randsp);

				if (wizard || !rn2(10)) pline(Hallucination ? "You feel that the RNG loves you!" : "You feel that someone was busy hiding useful items!");

				for (i = 0; i < randsp; i++) {

					if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

					if (timebasedlowerchance()) (void) makemon(&mons[PM_GOOD_ITEM_MASTER], 0, 0, NO_MM_FLAGS);
				}

			}

			if (u.ualign.sins && (rn2(100000) < u.ualign.sins ) ) {
				int copcnt = rnd(monster_difficulty() ) + 1;
				if (Role_if(PM_CAMPERSTRIKER)) copcnt *= (rn2(5) ? 2 : rn2(5) ? 3 : 5);

				if (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "anti-government helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "antipravitel'stvennaya shlem") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "aksil-hukumat dubulg'a") ) ) {
					copcnt = (copcnt / 2) + 1;
				}

			      while(--copcnt >= 0) {
					if (xupstair) (void) makemon(mkclass(S_KOP,0), xupstair, yupstair, MM_ANGRY|MM_ADJACENTOK);
					else (void) makemon(mkclass(S_KOP,0), 0, 0, MM_ANGRY|MM_ADJACENTOK);
				} /* while */

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

			nastyitemchance = 50000;

			if (moves > 2000) nastyitemchance = 45000;
			if (moves > 4000) nastyitemchance = 40000;
			if (moves > 10000) nastyitemchance = 35000;
			if (moves > 20000) nastyitemchance = 30000;
			if (moves > 40000) nastyitemchance = 27500;
			if (moves > 60000) nastyitemchance = 25000;
			if (moves > 80000) nastyitemchance = 22500;
			if (moves > 120000) nastyitemchance = 20000;
			if (moves > 160000) nastyitemchance = 17500;
			if (moves > 200000) nastyitemchance = 15000;
			if (moves > 300000) nastyitemchance = 12500;
			if (moves > 400000) nastyitemchance = 10000;
			if (moves > 500000) nastyitemchance = 7500;
			if (moves > 1000000) nastyitemchance = 5000;
			if (moves > 1500000) nastyitemchance = 2500;
			if (moves > 2000000) nastyitemchance = 1500;
			if (moves > 2500000) nastyitemchance = 1000;

			if (ishaxor) nastyitemchance /= 2;

			if (!rn2(nastyitemchance) && !issoviet) {

				randsp = (rn2(14) + 2);
				if (!rn2(10)) randsp *= 2;
				if (!rn2(100)) randsp *= 3;
				if (!rn2(1000)) randsp *= 5;
				if (!rn2(10000)) randsp *= 10;
				if (randsp > 1) randsp = rnd(randsp);

				if (wizard || !rn2(10)) pline(Hallucination ? "You feel that the RNG hates you!" : "You feel that the monsters are coming for you with everything they got!");

				for (i = 0; i < randsp; i++) {

					if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

					(void) makemon(&mons[PM_BAD_ITEM_MASTER], 0, 0, NO_MM_FLAGS);
			 	      (void) makemon((struct permonst *)0, 0, 0, MM_ADJACENTOK);
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
				monstercolor = rnd(288);

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
				monstercolor = rnd(288);
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

		    if(!rn2( (u.uevent.udemigod || u.uprops[STORM_HELM].extrinsic) ? 125 :
			    (depth(&u.uz) > depth(&stronghold_level)) ? 250 : 340)) {
			if (!ishomicider) (void) makemon((struct permonst *)0, 0, 0, NO_MM_FLAGS);
			else makerandomtrap();
			}

	/* still keeping the old monstermaking routine up, but drastically reducing their spawn rate. --Amy */

		    /* calculate how much time passed. */
#ifdef STEED
		    if (u.usteed && u.umoved) {
			/* your speed doesn't augment steed's speed */
			moveamt = mcalcmove(u.usteed);
		    } else
#endif
		    {
			moveamt = youmonst.data->mmove;
			if (youmonst.data->mmove == 0 && !rn2(2)) moveamt += 1; /* be lenient if an ungenomold player is unlucky 		 * enough to poly into a red mold or something like that. Otherwise they would simply die with no chance.
		 * see hack.c code that still prevents movement if polymorphed into something sessile.
		 * Also, you're still slower than a lichen (speed 1), so this should be ok. */

			if (Race_if(PM_ASGARDIAN) && !rn2(20) && moveamt > 1) /* Asgardians are slower sometimes, this is intentional. --Amy */
				moveamt /= 2;

			if (Race_if(PM_SPIRIT) && !rn2(8) && moveamt > 1) /* Spirits too. */
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
			if (Frozen && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2; /* frozen characters move at half speed --Amy */
			}
			if (u.inertia && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2; /* inert characters move at half speed --Amy */
			}
			if (Race_if(PM_TURTLE) && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2; /* turtles are very slow too --Amy */
			}
			if (Race_if(PM_LOWER_ENT) && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2; /* ents are very slow too --Amy */
			}
			if (Role_if(PM_TRANSSYLVANIAN) && (moveamt > 1) && (!PlayerInHighHeels) ) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2; /* transsylvanian has to wear heels at all times --Amy */
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

			if ( (SpeedBug || u.uprops[SPEED_BUG].extrinsic || have_speedbugstone()) && moveamt > 1) { /* speed bug messes up the player's speed --Amy */
				if (rn2(5)) moveamt *= rnd(5);
				moveamt /= rnd(6);
				if (!rn2(5)) moveamt /= 2;
			}

			/* speed bug reverses speed effects --Amy */
			if (Very_fast && (SpeedBug || u.uprops[SPEED_BUG].extrinsic || have_speedbugstone()) && rn2(4) && rn2(4) && moveamt > 1 ) {	/* speed boots or potion */
			    /* average movement is 0.5625 times normal */

				moveamt /= 2;

			} else if (Fast && (SpeedBug || u.uprops[SPEED_BUG].extrinsic || have_speedbugstone()) && !rn2(4) && moveamt > 1 ) {
			    /* average movement is 0.75 times normal */

				moveamt /= 2;
			}

			if (moveamt < 0) moveamt = 0;

			if (Very_fast && !SpeedBug && !u.uprops[SPEED_BUG].extrinsic && !have_speedbugstone()) {	/* speed boots or potion */
			    /* average movement is 1.67 times normal */
			    moveamt += NORMAL_SPEED / 2;
			    if (rn2(3) == 0) moveamt += NORMAL_SPEED / 2;
			} else if (Fast && !SpeedBug && !u.uprops[SPEED_BUG].extrinsic && !have_speedbugstone()) {
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

		    monstermoves++;
		    moves++;

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

		if (AutoDestruct || u.uprops[AUTO_DESTRUCT].extrinsic || have_autodestructstone()) stop_occupation();
 
		if (FaintActive && !rn2(100) && multi >= 0) {

			pline("You faint from exertion.");
			flags.soundok = 0;
			nomul(-(rnz(5) ), "fainted from exertion");
			nomovemsg = "You regain consciousness.";
			afternmv = unfaintX;

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

		for(ttmp = ftrap; ttmp; ttmp = ttmp->ntrap) { /* this function is probably expensive... --Amy */
			if (ttmp && ttmp->ttyp == LOUDSPEAKER && !rn2(100) ) {
				pline(fauxmessage());
				if (!rn2(3)) pline(fauxmessage());
			}

			if (ttmp && ttmp->ttyp == FUMAROLE && (distu(ttmp->tx, ttmp->ty) < 4 ) ) {

				if (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "filtered helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "fil'truyut shlem") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "filtrlangan zarbdan") ) && !rn2(2) ) {
					 pline("A cloud of spores surrounds you!");

				 } else if (!Strangled && !Breathless) {
					 pline("You inhale a cloud of spores!");
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

			if (ttmp && ttmp->ttyp == MONSTER_CUBE && !rn2(50)) {
				if (!enexto(&cc, ttmp->tx, ttmp->ty, (struct permonst *)0) ) continue;
				(void) makemon((struct permonst *)0, ttmp->tx, ttmp->ty, MM_ADJACENTOK);
				if (!rn2(20)) pline("Chaeaet!");
				if (!rn2(50)) ttmp->ttyp = ANIMATION_TRAP;
			}

			if (ttmp && ttmp->ttyp == CURSED_GRAVE && !rn2(50)) {
				if (!enexto(&cc, ttmp->tx, ttmp->ty, (struct permonst *)0) ) continue;
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

				if (midnight()) pline("You feel a ghastly chill running down your %s!", body_part(SPINE) );
				else if (!rn2(20)) pline("A monster rises from the grave!");
				if (!rn2(50)) ttmp->ttyp = SUMMON_UNDEAD_TRAP;
			}

		}

		if (have_faintingstone() && !rn2(100) && multi >= 0) {

			pline("You faint from exertion.");
			flags.soundok = 0;
			nomul(-(rnz(5) ), "fainted from exertion");
			nomovemsg = "You regain consciousness.";
			afternmv = unfaintX;

		}

		if (u.uprops[FAINT_ACTIVE].extrinsic && !rn2(100) && multi >= 0) {

			pline("You faint from exertion.");
			flags.soundok = 0;
			nomul(-(rnz(5) ), "fainted from exertion");
			nomovemsg = "You regain consciousness.";
			afternmv = unfaintX;

		}

		if (Prem_death && !rn2(10000)) { /* evil patch idea by jonadab */

			pline("You suddenly die.");
			killer_format = KILLED_BY;
			killer = "premature death";
			done(DIED);

		}

		if ( Itemcursing && !rn2(1000) ) {
			if (!Blind) 
				You("notice a %s glow surrounding you.", hcolor(NH_BLACK));
			rndcurse();

		}

		if ( Role_if(PM_TRANSSYLVANIAN) && !rn2(1000) ) {
			if (!Blind) 
				You("notice a %s glow surrounding you.", hcolor(NH_BLACK));
			rndcurse();

		}

		if ( have_cursingstone() && !rn2(1000) ) {
			if (!Blind) 
				You("notice a %s glow surrounding you.", hcolor(NH_BLACK));
			rndcurse();

		}

		if ( have_primecurse() && !rn2(1000) ) {
			if (!Blind) 
				You("notice a %s glow surrounding you.", hcolor(NH_BLACK));
			rndcurse();

		}

		if (RecurringDisenchant && !rn2(1000)) {

			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(10)) (void) drain_item(otmpE);
			}
			pline("Your equipment seems less effective.");
		}

		if (u.uprops[RECURRING_DISENCHANT].extrinsic && !rn2(1000)) {

			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(10)) (void) drain_item(otmpE);
			}
			pline("Your equipment seems less effective.");
		}

		if (have_disenchantmentstone() && !rn2(1000)) {

			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(10)) (void) drain_item(otmpE);
			}
			pline("Your equipment seems less effective.");
		}

		if (ChaosTerrain && !rn2(5)) {

			int chaosx, chaosy;
			chaosx = rn1(COLNO-3,2);
			chaosy = rn2(ROWNO);
			if (chaosx && chaosy && isok(chaosx, chaosy) && (levl[chaosx][chaosy].typ == ROOM || levl[chaosx][chaosy].typ == CORR) ) {
				levl[chaosx][chaosy].typ = randomwalltype();
				block_point(chaosx,chaosy);
				del_engr_at(chaosx,chaosy);
				newsym(chaosx,chaosy);
			}

		}

		if (u.uprops[CHAOS_TERRAIN].extrinsic && !rn2(5)) {

			int chaosx, chaosy;
			chaosx = rn1(COLNO-3,2);
			chaosy = rn2(ROWNO);
			if (chaosx && chaosy && isok(chaosx, chaosy) && (levl[chaosx][chaosy].typ == ROOM || levl[chaosx][chaosy].typ == CORR) ) {
				levl[chaosx][chaosy].typ = randomwalltype();
				block_point(chaosx,chaosy);
				del_engr_at(chaosx,chaosy);
				newsym(chaosx,chaosy);
			}

		}

		if (have_chaosterrainstone() && !rn2(5)) {

			int chaosx, chaosy;
			chaosx = rn1(COLNO-3,2);
			chaosy = rn2(ROWNO);
			if (chaosx && chaosy && isok(chaosx, chaosy) && (levl[chaosx][chaosy].typ == ROOM || levl[chaosx][chaosy].typ == CORR) ) {
				levl[chaosx][chaosy].typ = randomwalltype();
				block_point(chaosx,chaosy);
				del_engr_at(chaosx,chaosy);
				newsym(chaosx,chaosy);
			}

		}

		if (RecurringAmnesia && !rn2(1000)) {
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

		if (u.uprops[ITEMCURSING].extrinsic && !rn2(1000) ) {
			if (!Blind) 
				You("notice a %s glow surrounding you.", hcolor(NH_BLACK));
			rndcurse();

		}

		if (uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "graffiti gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "graffiti perchatki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "qo'lqop purkash") ) && !rn2(2000) ) {
		    incr_itimeout(&Glib, 2); /* just enough to make you drop your weapon */
		}

		if (uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "vampiric gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "vampiry perchatki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "sindirishi qo'lqop") ) && (u.uexp > 100) && !rn2(1000) ) {
			pline("Your vampiric gloves drain your experience!");
			u.uexp -= (u.uexp / 100);
			if (u.uexp < newuexp(u.ulevel - 1)) {
			      losexp("vampiric experience drain", TRUE, FALSE);
			}
		}

		if (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "RNG helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "shlem gsch") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "RNG dubulg'a") ) && !rn2(100000) ) {
			badeffect();
		}

		if ( (have_blackystone() || u.uprops[BLACK_NG_WALLS].extrinsic) && !BlackNgWalls && !rn2(100) ) {

			blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
			if (!blackngdur ) blackngdur = 500; /* fail safe */

			BlackNgWalls = (blackngdur - (monster_difficulty() * 3));
			(void) makemon(&mons[PM_BLACKY], 0, 0, NO_MM_FLAGS);
			break;
		}

		if (Deafness || u.uprops[DEAFNESS].extrinsic || have_deafnessstone() ) flags.soundok = 0;

		/* Let's throw a bone to permablind races. --Amy */
		if (!Unidentify && !u.uprops[UNIDENTIFY].extrinsic && !have_unidentifystone() ) {

			if (invent) {
			    for (otmpi = invent; otmpi; otmpi = otmpii) {
			      otmpii = otmpi->nobj;
	
				if (!rn2(10000) && !otmpi->dknown) {
					otmpi->dknown = TRUE;
					pline("You feel that you know more about the contents of your inventory...");
				}
				if (!rn2(100) && Race_if(PM_JELLY) && !(otmpi->oclass == SPBOOK_CLASS) && !otmpi->dknown) {
					otmpi->dknown = TRUE;
					pline("You feel that you know more about the contents of your inventory...");
				}
				if (!rn2(10000) && isangbander && !otmpi->dknown) {
					otmpi->dknown = TRUE;
					pline("You feel that you know more about the contents of your inventory...");
				}
				if (!rn2(10000) && isangbander && !otmpi->known) {
					otmpi->known = TRUE;
					pline("You feel that you know more about the contents of your inventory...");
				}
				if (!rn2(10000) && isangbander && !otmpi->bknown) {
					otmpi->bknown = TRUE;
					pline("You feel that you know more about the contents of your inventory...");
				}
				if (!rn2(10000) && isangbander && !otmpi->rknown) {
					otmpi->rknown = TRUE;
					pline("You feel that you know more about the contents of your inventory...");
				}
				if (!rn2(2000) && Race_if(PM_WEAPON_CUBE) && !otmpi->dknown) {
					otmpi->dknown = TRUE;
					pline("You feel that you know more about the contents of your inventory...");
				}
				if (!rn2(1000) && Race_if(PM_CORTEX) && !otmpi->dknown) {
					otmpi->dknown = TRUE;
					pline("You feel that you know more about the contents of your inventory...");
				}
				if (!rn2(500) && Race_if(PM_VORTEX) && !otmpi->dknown) {
					otmpi->dknown = TRUE;
					pline("You feel that you know more about the contents of your inventory...");
				}

				if (!rn2(100000) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "mysterious cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "tainstvennyy plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "sirli plash") ) && !otmpi->dknown) {
					otmpi->dknown = TRUE;
					pline("You feel that you know more about the contents of your inventory...");
				}
				if (!rn2(100000) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "mysterious cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "tainstvennyy plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "sirli plash") ) && !otmpi->known) {
					otmpi->known = TRUE;
					pline("You feel that you know more about the contents of your inventory...");
				}
				if (!rn2(100000) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "mysterious cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "tainstvennyy plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "sirli plash") ) && !otmpi->bknown) {
					otmpi->bknown = TRUE;
					pline("You feel that you know more about the contents of your inventory...");
				}
				if (!rn2(100000) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "mysterious cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "tainstvennyy plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "sirli plash") ) && !otmpi->rknown) {
					otmpi->rknown = TRUE;
					pline("You feel that you know more about the contents of your inventory...");
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
			Strcpy(buf, rndmonnam() );
			pline("Captcha! Please type in the following word(s) to continue: %s", buf);
			getlin("Your input:",ebuf);
			if (strncmpi(buf, ebuf, (int) strlen(ebuf)) != 0) {
				pline("WRONG! You will be punished. I will paralyze you, slow you and reduce your max HP and Pw.");

				if (multi >= 0) nomul(-2, "paralyzed by a captcha");

				u.ublesscnt += rnz(300);
				change_luck(-1);

				u.ualign.sins++;
				u.alignlim--;
			      adjalign(-10);

				u.uhpmax -= rnd(5);
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				if (u.uhpmax < 1) {
				    killer = "failing to solve a captcha";
				    killer_format = KILLED_BY;
				    done(DIED);
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

		if (!rn2(100) && QuizTrapEffect) {
			boolean rumoristrue = rn2(2);

			pline("NetHack Quiz! You will now tell me whether the following rumor is true or not!");

			if (rumoristrue) outrumor(1, BY_OTHER);
			else outrumor(-1, BY_OTHER);

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

					pline("You really believe everything you read, huh? Well, sucks to be you. This rumor was obviously not true! Everyone except you would've noticed that! Enjoy the punishment.");
					badeffect();

				}

			}

		}

		if (!rn2(100) && u.uprops[QUIZZES].extrinsic) {
			boolean rumoristrue = rn2(2);

			pline("NetHack Quiz! You will now tell me whether the following rumor is true or not!");

			if (rumoristrue) outrumor(1, BY_OTHER);
			else outrumor(-1, BY_OTHER);

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

					pline("You really believe everything you read, huh? Well, sucks to be you. This rumor was obviously not true! Everyone except you would've noticed that! Enjoy the punishment.");
					badeffect();

				}

			}

		}

		if (!rn2(100) && have_quizstone()) {
			boolean rumoristrue = rn2(2);

			pline("NetHack Quiz! You will now tell me whether the following rumor is true or not!");

			if (rumoristrue) outrumor(1, BY_OTHER);
			else outrumor(-1, BY_OTHER);

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

					pline("You really believe everything you read, huh? Well, sucks to be you. This rumor was obviously not true! Everyone except you would've noticed that! Enjoy the punishment.");
					badeffect();

				}

			}

		}

		if (!rn2(100) && u.uprops[CAPTCHA].extrinsic) {
			Strcpy(buf, rndmonnam() );
			pline("Captcha! Please type in the following word(s) to continue: %s", buf);
			getlin("Your input:",ebuf);
			if (strncmpi(buf, ebuf, (int) strlen(ebuf)) != 0) {
				pline("WRONG! You will be punished. I will paralyze you, slow you and reduce your max HP and Pw.");

				if (multi >= 0) nomul(-2, "paralyzed by a captcha");

				u.ublesscnt += rnz(300);
				change_luck(-1);

				u.ualign.sins++;
				u.alignlim--;
			      adjalign(-10);

				u.uhpmax -= rnd(5);
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				if (u.uhpmax < 1) {
				    killer = "failing to solve a captcha";
				    killer_format = KILLED_BY;
				    done(DIED);
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
			Strcpy(buf, rndmonnam() );
			pline("Captcha! Please type in the following word(s) to continue: %s", buf);
			getlin("Your input:",ebuf);
			if (strncmpi(buf, ebuf, (int) strlen(ebuf)) != 0) {
				pline("WRONG! You will be punished. I will paralyze you, slow you and reduce your max HP and Pw.");

				if (multi >= 0) nomul(-2, "paralyzed by a captcha");

				u.ublesscnt += rnz(300);
				change_luck(-1);

				u.ualign.sins++;
				u.alignlim--;
			      adjalign(-10);

				u.uhpmax -= rnd(5);
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				if (u.uhpmax < 1) {
				    killer = "failing to solve a captcha";
				    killer_format = KILLED_BY;
				    done(DIED);
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
			ubatarang->known = ubatarang->dknown = ubatarang->bknown = ubatarang->rknown = 1;
			ubatarang->owt = weight(ubatarang);
			dropy(ubatarang);
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
			if ((u.dehydrationtime - moves) == 100) pline("You are beginning to feel thirsty.");
			if ((u.dehydrationtime - moves) == 0) pline("You are dehydrated, your vision begins to blur...");
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
		    nomul(-3, "frightened to death");
			make_feared(HFeared + rnd(30 + (monster_difficulty() * 3) ),TRUE);
		    nomovemsg = "You regain your composure.";
		}

		if ((u.uprops[CRAP_EFFECT].extrinsic || CrapEffect || have_shitstone() ) && (u.uhs == 0) && !rn2(100) ) {
			pline("You suddenly have to take a shit!");
			int crapduration = 5;
			if (uarm && objects[uarm->otyp].oc_delay) {
				pline("Taking off your armor is going to take a while...");
				crapduration += objects[uarm->otyp].oc_delay;
			}
			if (uarmc && objects[uarmc->otyp].oc_delay) {
				pline("You need to remove your cloak...");
				crapduration += objects[uarmc->otyp].oc_delay;
			}
			if (Sick && !rn2(3) ) make_sick(0L, (char *)0, TRUE, SICK_VOMITABLE);
			else if (Sick && !rn2(10) ) make_sick(0L, (char *)0, TRUE, SICK_ALL);
			if (u.uhs == 0) morehungry(rn2(400)+200);
			nomovemsg = "You are done shitting.";
			nomul(-crapduration, "while taking a shit");

			if (!rn2(5)) {
				pline("You did not watch out, and stepped into your own shit.");
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

		if (( (u.uhave.amulet && !rn2(5)) || Clairvoyant) &&
		    !In_endgame(&u.uz) && !BClairvoyant &&
		    !(moves % 15) && !rn2(2))
			do_vicinity_map();
	
		if(u.utrap && u.utraptype == TT_LAVA) {
		    if(!is_lava(u.ux,u.uy))
			u.utrap = 0;
		    else if (!u.uinvulnerable) {
			u.utrap -= 1<<8;
			if(u.utrap < 1<<8) {
			    killer_format = KILLED_BY;
			    killer = "molten lava";
			    You(Hallucination ? "dissolve completely, warping to another plane of existence." : "sink below the surface and die.");
			    done(DISSOLVED);
			} else if(didmove && !u.umoved) {
			    /*Norep*/pline(Hallucination ? "Your body is dissolving... maybe the Grim Reaper is waiting for you?" : "You sink deeper into the lava.");
		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			    u.utrap += rnd(4);
			}
		    }
		}


		    if (flags.bypasses) clear_bypasses();
		    if(IsGlib) glibr();

		    if (!rn2(2) || !(uarmf && OBJ_DESCR(objects[uarmf->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "irregular boots") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "neregulyarnyye sapogi") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "tartibsizlik chizilmasin") ) ) ) {

		    nh_timeout();
		    }

		    run_regions();

#ifdef DUNGEON_GROWTH
		    dgn_growths(TRUE, TRUE);
#endif

		    if (u.ublesscnt)  u.ublesscnt--;

		if (uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "comfortable gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "udobnyye perchatki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "qulay qo'lqop") ) ) u.ublesscnt--;
		    
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
		    } else if (Upolyd && youmonst.data->mlet == S_EEL && !is_pool(u.ux,u.uy) && !Is_waterlevel(&u.uz)) {
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

			if (u.mh < 1)
			    rehumanize();
			else if (Regeneration ||
				    (wtcap < MOD_ENCUMBER && !(moves%/*20*/regenrate))) {
			    flags.botl = 1;
			    if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) ) u.mh++;
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


 			int efflev = u.ulevel + (u.uhealbonus);
 			int effcon = ACURR(A_CON) + (u.uhealbonus);

			if (!(AllSkillsUnskilled || u.uprops[SKILL_DEACTIVATED].extrinsic || have_unskilledstone())) {

			if (!issoviet) {
				if (P_SKILL(P_RIDING) == P_SKILLED) efflev += 2;
				if (P_SKILL(P_RIDING) == P_EXPERT) efflev += 5;
				if (P_SKILL(P_RIDING) == P_MASTER) efflev += 7;
				if (P_SKILL(P_RIDING) == P_GRAND_MASTER) efflev += 10;
				if (P_SKILL(P_RIDING) == P_SKILLED) effcon += 2;
				if (P_SKILL(P_RIDING) == P_EXPERT) effcon += 5;
				if (P_SKILL(P_RIDING) == P_MASTER) effcon += 7;
				if (P_SKILL(P_RIDING) == P_GRAND_MASTER) effcon += 10;
			} else {
				if (P_SKILL(P_RIDING) == P_SKILLED) efflev -= 2;
				if (P_SKILL(P_RIDING) == P_EXPERT) efflev -= 5;
				if (P_SKILL(P_RIDING) == P_MASTER) efflev -= 7;
				if (P_SKILL(P_RIDING) == P_GRAND_MASTER) efflev -= 10;
				if (P_SKILL(P_RIDING) == P_SKILLED) effcon -= 2;
				if (P_SKILL(P_RIDING) == P_EXPERT) effcon -= 5;
				if (P_SKILL(P_RIDING) == P_MASTER) effcon -= 7;
				if (P_SKILL(P_RIDING) == P_GRAND_MASTER) effcon -= 10;
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
			    if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) ) u.uhp += heal;
			    if(u.uhp > u.uhpmax)
				u.uhp = u.uhpmax;
			} else if (Regeneration ||
			     (efflev <= 9 &&
			      !(moves % ((MAXULEV+12) / (u.ulevel+2) + 1)))) {
			    flags.botl = 1;
			    if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) ) u.uhp++;
			}
		    }

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && Race_if(PM_HAXOR) && !rn2(20) && (rn2(2) || (!sengr_at("Elbereth", u.ux, u.uy) ) ) ) {
				u.uhp += rnd(5 + (u.ulevel / 5));
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				flags.botl = 1;
			}
			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && Race_if(PM_HAXOR) && Upolyd && !rn2(20) && (rn2(2) || (!sengr_at("Elbereth", u.ux, u.uy) ) ) ) {
				u.mh += rnd(5 + (u.ulevel / 5));
				if (u.mh > u.mhmax) u.mh = u.mhmax;
				flags.botl = 1;
			}
			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH)) && !rn2(60 / u.ulevel) ) {
				u.uhp++;
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				flags.botl = 1;
			}
			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH)) && !rn2(60 / u.ulevel) && Upolyd ) {
				u.mh++;
				if (u.mh > u.mhmax) u.mh = u.mhmax;
				flags.botl = 1;
			}

			/* nice patch addition by Amy - sometimes regenerate more */
			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && !rn2(150) && (rn2(2) || (!sengr_at("Elbereth", u.ux, u.uy) ) ) ){
				u.uhp += rnz(2 + u.ulevel);
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				flags.botl = 1;
			}
			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && !rn2(150) && Upolyd && (rn2(2) || (!sengr_at("Elbereth", u.ux, u.uy) ) ) ){
				u.mh += rnz(2 + u.ulevel);
				if (u.mh > u.mhmax) u.mh = u.mhmax;
				flags.botl = 1;
			}

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && Race_if(PM_HAXOR) && !rn2(20) ) {
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
		    if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && (recalc_mana() >= 0 || (!rn2(-(recalc_mana() - 1) ) ) ) && (u.uen < u.uenmax) && 
				((Energy_regeneration && !rn2(5)) || /* greatly nerfed overpowered wizard artifact --Amy */
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
			if (!Burned && !issoviet && (rn2(2) || !Race_if(PM_SYLPH) ) && !rn2(250) && (u.uen < u.uenmax)) {

				u.uen += rnz(2 + u.ulevel);
				if (u.uen > u.uenmax)  u.uen = u.uenmax;
				flags.botl = 1;

			}
			if (!Burned && !issoviet && (rn2(2) || !Race_if(PM_SYLPH) ) && !rn2(250) && (u.uen < u.uenmax) && Energy_regeneration) {

				u.uen += rnz(2 + u.ulevel);
				if (u.uen > u.uenmax)  u.uen = u.uenmax;
				flags.botl = 1;

			}

			if (!Burned && !issoviet && (rn2(2) || !Race_if(PM_SYLPH) ) && !rn2(50) && (u.uen < u.uenmax) && recalc_mana() > 0) {

				u.uen += rnd(recalc_mana());
				if (u.uen > u.uenmax)  u.uen = u.uenmax;
				flags.botl = 1;

			}

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && Race_if(PM_RODNEYAN)) { /* rodney has special built-in energy regeneration --Amy */
				u.uen++;
				if (u.uen > u.uenmax)  u.uen = u.uenmax;
				flags.botl = 1;
			}

			if (!Burned && !issoviet && (rn2(2) || !Race_if(PM_SYLPH)) && !rn2(90 / u.ulevel) ) {
				u.uen++;
				if (u.uen > u.uenmax)  u.uen = u.uenmax;
				flags.botl = 1;
			}

		/* leveling up will give a small boost to mana regeneration now --Amy */
		    if ( !Burned && !issoviet && (rn2(2) || !Race_if(PM_SYLPH) ) && u.uen < u.uenmax && ( 
			(u.ulevel >= 5 && !rn2(200)) ||
			(u.ulevel >= 10 && !rn2(100)) ||
			(u.ulevel >= 15 && !rn2(50)) ||
			(u.ulevel >= 20 && !rn2(30)) ||
			(u.ulevel >= 25 && !rn2(20)) ||
			(u.ulevel >= 30 && !rn2(10))
			)
			)
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			/* Having a spell school at skilled will improve mana regeneration.
			 * Having a spell school at expert will improve it by even more. --Amy */

			if (!(issoviet || AllSkillsUnskilled || u.uprops[SKILL_DEACTIVATED].extrinsic || have_unskilledstone())) {

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ATTACK_SPELL) == P_SKILLED && !rn2(200))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ATTACK_SPELL) == P_EXPERT && !rn2(100))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ATTACK_SPELL) == P_MASTER && !rn2(50))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ATTACK_SPELL) == P_GRAND_MASTER && !rn2(25))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_DIVINATION_SPELL) == P_SKILLED && !rn2(200))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_DIVINATION_SPELL) == P_EXPERT && !rn2(100))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_DIVINATION_SPELL) == P_MASTER && !rn2(50))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_DIVINATION_SPELL) == P_GRAND_MASTER && !rn2(25))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_MATTER_SPELL) == P_SKILLED && !rn2(200))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_MATTER_SPELL) == P_EXPERT && !rn2(100))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_MATTER_SPELL) == P_MASTER && !rn2(50))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_MATTER_SPELL) == P_GRAND_MASTER && !rn2(25))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_BODY_SPELL) == P_SKILLED && !rn2(200))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_BODY_SPELL) == P_EXPERT && !rn2(100))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_BODY_SPELL) == P_MASTER && !rn2(50))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_BODY_SPELL) == P_GRAND_MASTER && !rn2(25))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_PROTECTION_SPELL) == P_SKILLED && !rn2(200))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_PROTECTION_SPELL) == P_EXPERT && !rn2(100))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_PROTECTION_SPELL) == P_MASTER && !rn2(50))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_PROTECTION_SPELL) == P_GRAND_MASTER && !rn2(25))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ENCHANTMENT_SPELL) == P_SKILLED && !rn2(200))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ENCHANTMENT_SPELL) == P_EXPERT && !rn2(100))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ENCHANTMENT_SPELL) == P_MASTER && !rn2(50))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ENCHANTMENT_SPELL) == P_GRAND_MASTER && !rn2(25))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_HEALING_SPELL) == P_SKILLED && !rn2(200))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_HEALING_SPELL) == P_EXPERT && !rn2(100))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_HEALING_SPELL) == P_MASTER && !rn2(50))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			if (!Burned && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_HEALING_SPELL) == P_GRAND_MASTER && !rn2(25))
			u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;

			}

			/* Spooky faux error messages on the Spacewars Fighter goal level --Amy */
			if ((Role_if(PM_SPACEWARS_FIGHTER) && !rn2(200) && Is_nemesis(&u.uz) ) || (Role_if(PM_CAMPERSTRIKER) && !rn2(200) && In_quest(&u.uz)) ) {
			pline("Warning: Low Local Memory. Freeing description strings.");
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

				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			}

			/* more faux messages */
			if ((Role_if(PM_SPACEWARS_FIGHTER) && !rn2(200) && Is_nemesis(&u.uz) ) || (Role_if(PM_CAMPERSTRIKER) && !rn2(200) && In_quest(&u.uz)) ) {
				pline(fauxmessage());
				if (!rn2(3)) pline(fauxmessage());
			}

		    if(!u.uinvulnerable) {
			if(Teleportation && (ishaxor ? !rn2(150) : !rn2(250)) ) {
			    xchar old_ux = u.ux, old_uy = u.uy;
				You(Hallucination ? "open a warp gate!" : "suddenly get teleported!");
			    tele();
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			    if (u.ux != old_ux || u.uy != old_uy) {
				if (!next_to_u()) {
				    check_leash(old_ux, old_uy);
				}
#ifdef REDO
				/* clear doagain keystrokes */
				pushch(0);
				savech(0);
#endif
			    }
			}

			if (Race_if(PM_RODNEYAN) && !rn2(1000)) {	/* levelteleportitis --Amy */

				make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */
				pline("A mysterious force surrounds you...");
			      if (!flags.lostsoul && !flags.uberlostsoul && !(u.uprops[STORM_HELM].extrinsic)) level_tele();
				else pline("You feel very disoriented but decide to move on.");

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
				    nomul(0, 0);
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
		    if (u.uevent.udemigod && !u.uinvulnerable) {
			if (u.udg_cnt) u.udg_cnt--;
			if (!u.udg_cnt) {
			    intervene();
			    u.udg_cnt = rn1(200, 50);
			}
		    }
		    restore_attrib();

		    /* underwater and waterlevel vision are done here */
		    if (Is_waterlevel(&u.uz))
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

	if ((BankTrapEffect || u.uprops[BANKBUG].extrinsic || have_bankstone()) && u.ugold) {

		if (!u.bankcashlimit) u.bankcashlimit = rnz(1000 * (monster_difficulty() + 1));

		u.bankcashamount += u.ugold;
		u.ugold = 0;
		pline("Your money was stored, thanks.");
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
# ifdef REDO
		else
		    pushch(ch);
# endif /* REDO */
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
#ifdef INSURANCE
	if (flags.xtimed_autosave && (moves > 1) && (moves % 20 == 0) ) save_currentstate();
	else if (flags.etimed_autosave && (moves > 1) && (moves % 100 == 0) ) save_currentstate();
#endif

	if (u.banishmentbeam) { /* uh-oh... something zapped you with a wand of banishment */
		/* this replaces the code in muse.c that always caused segfaults --Amy */

		make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

		/* failsafes in case the player somehow manages to quickly snatch the amulet or something... */
		if (u.uevent.udemigod || u.uhave.amulet || NoReturnEffect || u.uprops[NORETURN].extrinsic || have_noreturnstone() || (u.usteed && mon_has_amulet(u.usteed))) {
			pline("You shudder for a moment."); (void) safe_teleds(FALSE); u.banishmentbeam = 0; break;
		}

		if (flags.lostsoul || flags.uberlostsoul || u.uprops[STORM_HELM].extrinsic) { 
			pline("Somehow, the banishment beam doesn't do anything."); u.banishmentbeam = 0; break;
		}

		if (rn2(2)) {(void) safe_teleds(FALSE); goto_level(&medusa_level, TRUE, FALSE, FALSE); }
		else { (void) safe_teleds(FALSE); goto_level(&portal_level, TRUE, FALSE, FALSE); }
		u.banishmentbeam = 0; /* player got warped, now clear the flag even if it crashes afterwards */

		register int newlev = rnd(71);
		d_level newlevel;
		get_level(&newlevel, newlev);
		goto_level(&newlevel, TRUE, FALSE, FALSE);
	}

	if (u.levelporting) { /* something attacked you with nexus or weeping */

		if (!u.uevent.udemigod && !(flags.lostsoul || flags.uberlostsoul || u.uprops[STORM_HELM].extrinsic) ) {
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
	if(occupation) {
		if (!maybe_finished_meal(TRUE))
		    You("stop %s.", occtxt);
		occupation = 0;
		flags.botl = 1; /* in case u.uhs changed */
/* fainting stops your occupation, there's no reason to sync.
		sync_hunger();
*/
#ifdef REDO
		nomul(0, 0);
		pushch(0);
#endif
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
#ifdef CONVICT
       if (Role_if(PM_CONVICT) || Role_if(PM_MURDERER) || Race_if(PM_NYMPH) ) {
              setworn(mkobj(CHAIN_CLASS, TRUE), W_CHAIN);
              setworn(mkobj(BALL_CLASS, TRUE), W_BALL);
              /*uball->spe = 1;*/
              placebc();
              newsym(u.ux,u.uy);
       }
#endif /* CONVICT */

	if (flags.legacy) {
		flush_screen(1);
#ifdef CONVICT
        if (Role_if(PM_CONVICT)) {
		    com_pager(199);
        } else {
		    com_pager(1);
        }
#else
		com_pager(1);
#endif /* CONVICT */
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
    if (new_game || u.ualignbase[A_ORIGINAL] != u.ualignbase[A_CURRENT])
	Sprintf(eos(buf), " %s", align_str(u.ualignbase[A_ORIGINAL]));
    if (!urole.name.f &&
	    (new_game ? (urole.allow & ROLE_GENDMASK) == (ROLE_MALE|ROLE_FEMALE) :
	     currentgend != flags.initgend))
	Sprintf(eos(buf), " %s", genders[currentgend].adj);

    *xtrabuf = '\0';
	if (flags.hybridangbander) Sprintf(eos(xtrabuf), "angbander ");
	if (flags.hybridaquarian) Sprintf(eos(xtrabuf), "aquarian ");
	if (flags.hybridcurser) Sprintf(eos(xtrabuf), "curser ");
	if (flags.hybridhaxor) Sprintf(eos(xtrabuf), "haxor ");
	if (flags.hybridhomicider) Sprintf(eos(xtrabuf), "homicider ");
	if (flags.hybridsuxxor) Sprintf(eos(xtrabuf), "suxxor ");
	if (flags.hybridwarper) Sprintf(eos(xtrabuf), "warper ");
	if (flags.hybridrandomizer) Sprintf(eos(xtrabuf), "randomizer ");
	if (flags.hybridnullrace) Sprintf(eos(xtrabuf), "null ");
	if (flags.hybridmazewalker) Sprintf(eos(xtrabuf), "mazewalker ");
	if (flags.hybridsoviet) Sprintf(eos(xtrabuf), "soviet ");
	if (flags.hybridxrace) Sprintf(eos(xtrabuf), "x-race ");
	if (flags.hybridheretic) Sprintf(eos(xtrabuf), "heretic ");
	if (flags.hybridsokosolver) Sprintf(eos(xtrabuf), "sokosolver ");
	if (flags.hybridspecialist) Sprintf(eos(xtrabuf), "specialist ");
	if (flags.hybridamerican) Sprintf(eos(xtrabuf), "american ");
	if (flags.hybridminimalist) Sprintf(eos(xtrabuf), "minimalist ");
	if (flags.hybridnastinator) Sprintf(eos(xtrabuf), "nastinator ");
	if (flags.hybridrougelike) Sprintf(eos(xtrabuf), "rougelike ");

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

	if (Race_if(PM_MISSINGNO)) pline("Caution! The missingno might still be an unstable race. I tried to fix all the crashes but some may still remain. You can disable the missing_safety option if you deliberately want the game to be crashy. --Amy");

#ifdef PUBLIC_SERVER

#ifdef PHANTOM_CRASH_BUG
	if (new_game) pline("Welcome to SLASH'EM Extended! For game discussion, bug reports etc. join the #slashemextended IRC channel. :-) --Amy");
#else
	if (new_game) pline("You are playing SLASH'EM Extended on a public server. In the very rare case that your previous game crashed by sending you back to the dgamelaunch screen, please contact the server administrator for a restore.");
	if (new_game) pline("Panics and segfaults aren't the end of the world - in roughly 99%% of all cases, these can be restored into a working savegame file. Have fun playing! You can find me and other players on the IRC channel #slashemextended :-) --Amy");
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


	}
	}

	}

	if ((Role_if(PM_ACTIVISTOR) || Race_if(PM_PEACEMAKER) || Role_if(PM_MYSTIC)) && new_game) {

		int ammount;
		ammount = 0;

		while (ammount < 5) {

		switch (rnd(50)) {

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
		default:
			break;
			}
		ammount++;

		}
	}

	if (flags.lostsoul && !flags.uberlostsoul && new_game) { 
	goto_level(&medusa_level, TRUE, FALSE, FALSE); /* inspired by Tome, an Angband mod --Amy */
	pline("These are the halls of Mandos... err, Medusa. Good luck making your way back up!");
	}

	if (flags.uberlostsoul && new_game) { 

	goto_level((&sanctum_level - 1), TRUE, FALSE, FALSE);
	pline("These are the halls of Mandos... err, Gehennom. Looks nice, huh?");

			        register int newlev = 71;
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

	if (!new_game && (u.hangupcheat > 2)) { /* filthy cheater! */

		u.ublesscnt += rnz(2000);
		change_luck(-3);

		u.ualign.sins += 5;
		u.alignlim -= 5;
	      adjalign(-100);

		nomul(-(5 + u.hangupparalysis), "paralyzed by severe hangup cheating");

		u.uhpmax -= rnd(20);
		if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		if (u.uhpmax < 1) {
		    killer = "critical existence failure";
		    killer_format = KILLED_BY;
		    done(DIED);
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

	if (!new_game && u.hangupcheat) {

		pline("You hanged up during your last session! Since I can't determine whether you did that to cheat, you will now be paralyzed, slowed and have your max HP/Pw reduced. Please save your game normally next time! --Amy");
		if (multi >= 0) nomul(-(2 + u.hangupparalysis), "paralyzed by trying to hangup cheat");

		u.ublesscnt += rnz(300);
		change_luck(-1);

		u.ualign.sins++;
		u.alignlim--;
	      adjalign(-10);

		u.uhpmax -= rnd(5);
		if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		if (u.uhpmax < 1) {
		    killer = "critical existence failure";
		    killer_format = KILLED_BY;
		    done(DIED);
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

	if (chance > rnd(100)) return(TRUE);
	else return(FALSE);
}

#endif /* OVLB */

/*allmain.c*/
