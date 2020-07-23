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
STATIC_DCL void pumpsminigame(void);

STATIC_PTR void do_megafloodingf(int, int, void *);
STATIC_PTR void do_fjordefloodingf(int, int, void *);

#define decrnknow(spell)	spl_book[spell].sp_know--
#define spellid(spell)		spl_book[spell].sp_id
#define spellknow(spell)	spl_book[spell].sp_know
#define spellname(spell)	OBJ_NAME(objects[spellid(spell)])

static const char all_count[] = { ALLOW_COUNT, ALL_CLASSES, 0 };
static const char allowall[] = { ALL_CLASSES, 0 };

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

static long prev_dgl_extrainfo = 0;
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
	You(FunnyHallu ? "are on the moon tonight!" : "are lucky!  Full moon tonight.");
	change_luck(1);
    } else if(flags.moonphase == NEW_MOON) {
	pline("Be careful!  New moon tonight.");
	u.ualign.record -= 3; 
    } else if(flags.moonphase >= 1 && flags.moonphase <= 3) {
	pline("The moon is waxing tonight.");
    } else if(flags.moonphase >= 5 && flags.moonphase <= 7) {
	pline("The moon is waning tonight.");
    }
    flags.friday13 = friday_13th();
    if (flags.friday13) {
	pline("Watch out!  Bad things can happen on Friday the 13th.");
	change_luck(-1);
	u.ualign.record -= 10; 
    }
    /* KMH -- February 2 */
    flags.groundhogday = groundhog_day();
    if (flags.groundhogday)
	pline("Happy Groundhog Day!");

	if (getmonth() == 5) {
#ifdef PUBLIC_SERVER
		if (flags.uberlostsoul || flags.lostsoul
#ifdef GMMODE
 || flags.gmmode || flags.supergmmode
#endif
		|| flags.wonderland || flags.zapem) {
			pline("WARNING (PLEASE READ): Junethack is running - but you're using a playing mode that is incompatible with the tournament! The following modes are prohibited: lostsoul, uberlostsoul, gmmode, zapem and wonderland. If you want your games to count, quit this one now, disable all the forbidden options, and start a new game. Please refer to junethack.net for more information. Good luck!");
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
	if (TimerunBug || u.uprops[TIMERUN_BUG].extrinsic || have_timerunstone()) {
		didmove = TRUE;
		/* some places in the code use an ugly hack to give you a turn of timerun, which will then not disappear if
		 * your movement speed is faster than normal and you happen to get a double turn, but we assume that the
		 * timerun variable is 1 in this case so since it did what it was supposed to, it now becomes 0 again --Amy */
		if (TimerunBug == 1) TimerunBug = 0;
	}
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

		u.polyprotected = 0;
		u.aggravation = 0;
		u.heavyaggravation = 0;
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

			monclock = 90;

			if ((u.uevent.udemigod && !u.freeplaymode && u.amuletcompletelyimbued) || u.uprops[STORM_HELM].extrinsic) {
				monclock = 30;
			} else {
				if (depth(&u.uz) > depth(&stronghold_level)) {
					monclock = 80;
				}
				past_clock = moves - timeout_start;
				if (past_clock > 0) {
					monclock -= past_clock*40/clock_base;
				}
			}
			/* make sure we don't fall off the bottom */
			if (monclock < 40 && !(u.uevent.udemigod && !u.freeplaymode && u.amuletcompletelyimbued) && !u.uprops[STORM_HELM].extrinsic) { monclock = 40; }
			if (monclock < 30) { monclock = 30; }

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
				if ( ((u.uevent.udemigod && !u.freeplaymode && u.amuletcompletelyimbued) || u.uprops[STORM_HELM].extrinsic) && xupstair && rn2(10)) {
					(void) makemon((struct permonst *)0, xupstair, yupstair, MM_ADJACENTOK|MM_MAYSLEEP);
				} else {
					(void) makemon((struct permonst *)0, 0, 0, MM_MAYSLEEP);
				}
			}

			if (!rn2(monclock) && ishomicider ) makerandomtrap(FALSE);

			xtraclock = 200000;
			if ((u.uevent.udemigod && !u.freeplaymode && u.amuletcompletelyimbued) || u.uprops[STORM_HELM].extrinsic) {
				xtraclock = 60000;
			} else {
				if (depth(&u.uz) > depth(&stronghold_level)) {
					xtraclock = 160000;
				}
				past_clock = moves - timeout_start;
				if (past_clock > 0) {
					xtraclock -= past_clock*100000/clock_base;
				}
			}
			/* make sure we don't fall off the bottom */
			if (xtraclock < 100000 && !(u.uevent.udemigod && !u.freeplaymode && u.amuletcompletelyimbued) && !u.uprops[STORM_HELM].extrinsic) { xtraclock = 100000; }
			if (xtraclock < 60000) { xtraclock = 60000; }

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

				if (wizard || !rn2(10)) pline(FunnyHallu ? "You fear that you'll get a segmentation fault on your next turn!" : "You suddenly feel a surge of tension!");

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

				if (wizard || !rn2(10)) pline(FunnyHallu ? "You fear that you'll get a segmentation fault on your next turn!" : "You suddenly feel a surge of tension!");

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

				if (wizard || !rn2(10)) pline(FunnyHallu ? "You feel that someone reseeded the RNG!" : "You feel that someone was busy hiding treasure!");

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

				if (wizard || !rn2(10)) pline(FunnyHallu ? "You feel that the RNG loves you!" : "You feel that someone was busy hiding useful items!");

				for (i = 0; i < randsp; i++) {

					if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

					if (timebasedlowerchance() && timebasedlowerchance()) (void) makemon(&mons[PM_GOOD_ITEM_MASTER], 0, 0, NO_MM_FLAGS);
				}

			}

			if (u.ualign.sins && !rn2(100) && (rn2(2000) < u.ualign.sins ) ) {

				if (!rn2(20)) u.copwantedlevel += rnz(u.ualign.sins + 1);

				u.cnd_kopsummonamount++;
				int copcnt = rnd(monster_difficulty() ) + 1;
				if (rn2(5)) copcnt = (copcnt / (rnd(4) + 1)) + 1;
				if (Role_if(PM_CAMPERSTRIKER)) copcnt *= (rn2(5) ? 2 : rn2(5) ? 3 : 5);

				if (uarmh && itemhasappearance(uarmh, APP_ANTI_GOVERNMENT_HELMET) ) {
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
						(void) maketrap(x, y, KOP_CUBE, 0, FALSE);
						break;
						}
				}

			      while(--copcnt >= 0) {
					if (xupstair) (void) makemon(mkclass(S_KOP,0), xupstair, yupstair, rn2(3) ? MM_ANGRY|MM_ADJACENTOK : MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
					else (void) makemon(mkclass(S_KOP,0), 0, 0, rn2(3) ? MM_ANGRY|MM_ADJACENTOK : MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
				} /* while */

			}

			if (u.copwantedlevel) {

				u.copwantedlevel--;
				if (u.copwantedlevel < 0) u.copwantedlevel = 0; /* fail safe */

				if ( !(rn2(2) && (uarmh && itemhasappearance(uarmh, APP_ANTI_GOVERNMENT_HELMET))) && !(rn2(2) && RngeAntiGovernment) && !rn2(100)) {

					(void) makemon(mkclass(S_KOP,0), 0, 0, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
					if (!rn2(200)) {
						switch (rnd(5)) {
							case 1:
								if (level_difficulty() > 39) (void) makemon(&mons[PM_EXMINATOR_KOP], 0, 0, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
								else (void) makemon(&mons[PM_WHITLEVEL_KOP], 0, 0, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
								break;
							case 2:
								if (level_difficulty() > 29) (void) makemon(&mons[PM_RNGED_KOP], 0, 0, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
								else (void) makemon(&mons[PM_WHITLEVEL_KOP], 0, 0, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
								break;
							case 3:
								if (level_difficulty() > 19) (void) makemon(&mons[PM_GREYLEVEL_KOP], 0, 0, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
								else (void) makemon(&mons[PM_WHITLEVEL_KOP], 0, 0, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
								break;
							case 4:
								if (level_difficulty() > 9) (void) makemon(&mons[PM_BLAKLEVEL_KOP], 0, 0, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
								else (void) makemon(&mons[PM_WHITLEVEL_KOP], 0, 0, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
								break;
							case 5:
								(void) makemon(&mons[PM_WHITLEVEL_KOP], 0, 0, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
								break;
						}
					}

				}

			}

			if (Race_if(PM_PLAYER_DYNAMO) && u.copwantedlevel < 1000) u.copwantedlevel += 1000;

			if (moves == u.ascensionfirsthint) {

				if (u.freeplaymode) {
					pline("Reminder: there's a time limit, after which the game will start throwing random bad stuff at you.");
					pline("Currently you still have more than %d turns left though, so don't panic.", (u.ascensionfirsthint * 4) );
					pline("However, keep in mind that you're not supposed to be playing forever.");

				} else {

					pline("Reminder: You have a limited amount of time for ascending. This is not a joke.");
					pline("Currently you still have more than %d turns left though, so don't panic.", (u.ascensionfirsthint * 4) );
					pline("However, keep in mind that you're not supposed to be farming forever. Okay?");
				}

			}

			if (moves == u.ascensionsecondhint) {

				if (u.freeplaymode) {
					pline("Remember, there's a time limit and bad things will happen periodically once you exceed it.");
					pline("Currently you have less than %d turns left, so better get ready for the random bad effects.", u.ascensionsecondhint);
					pline("But don't sweat it; you've already ascended, and are just on your victory lap now.");

				} else {

					pline("Remember, you're not supposed to dilly-dally all the time! You're supposed to work towards ascending!");
					pline("Currently you have less than %d turns left, so better move on!", u.ascensionsecondhint);
					pline("If you don't ascend in time, the RNG will get angry, and you don't want that to happen!");

				}

			}

			if (moves == u.ascensiontimelimit) {

				if (u.freeplaymode) {
					pline("Time up! The curtain falls.");
					pline("Now, the RNG has decided that you've been playing enough, and will start throwing random bad stuff at you.");
					pline("Try to survive for as long as possible! Good luck!");

				} else {

					pline("You exceeded the maximum permissible amount of turns for winning the game!");
					pline("Now, the RNG is fed up with your shenanigans, and decides to make the game much more difficult.");
					pline("If you hurry up, you may still be able to ascend, but it will get harder the longer you procrastinate.");

				}
			}

			if (moves > u.ascensiontimelimit) {

				int annoyancefactor = 200 * u.ascensiontimelimit / moves;
				if (ishaxor) annoyancefactor /= 2;
				if (annoyancefactor > 200) annoyancefactor = 200;
				if (annoyancefactor < 2) annoyancefactor = 2;

				if (!rn2(annoyancefactor)) badeffect();

			}

			nastyitemchance = 500000;

			if (moves > 5000) nastyitemchance = 450000;
			if (moves > 10000) nastyitemchance = 400000;
			if (moves > 20000) nastyitemchance = 350000;
			if (moves > 40000) nastyitemchance = 300000;
			if (moves > 80000) nastyitemchance = 270000;
			if (moves > 120000) nastyitemchance = 250000;
			if (moves > 160000) nastyitemchance = 220000;
			if (moves > 240000) nastyitemchance = 200000;
			if (moves > 320000) nastyitemchance = 175000;
			if (moves > 400000) nastyitemchance = 150000;
			if (moves > 600000) nastyitemchance = 125000;
			if (moves > 800000) nastyitemchance = 100000;
			if (moves > 1000000) nastyitemchance = 75000;
			if (moves > 2000000) nastyitemchance = 50000;
			if (moves > 3000000) nastyitemchance = 40000;
			if (moves > 4000000) nastyitemchance = 30000;
			if (moves > 5000000) nastyitemchance = 20000;

			if (ishaxor) nastyitemchance /= 2;

			if (!rn2(nastyitemchance) && !issoviet) {

				randsp = (rn3(14) + 2);
				if (!rn2(10)) randsp *= 2;
				if (!rn2(100)) randsp *= 3;
				if (!rn2(1000)) randsp *= 5;
				if (!rn2(10000)) randsp *= 10;
				if (randsp > 1) randsp = rnd(randsp);

				if (wizard || !rn2(10)) pline(FunnyHallu ? "You feel that the RNG hates you!" : "You feel that the monsters are coming for you with everything they got!");

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

				if (wizard || !rn2(10)) pline(FunnyHallu ? "Someone hacked into the game data files to prevent you from ascending!" : "You feel the arrival of monsters!");

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

				if (wizard || !rn2(10)) pline(FunnyHallu ? "The colors, the colors!" : "You feel a colorful sensation!");

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
				monstercolor = rnd(376);

				if (wizard || !rn2(10)) pline(FunnyHallu ? "Someone got in here! Who could that be?" : "You feel that a group has arrived!");

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

				if (wizard || !rn2(10)) pline(FunnyHallu ? "Someone hacked into the game data files to prevent you from ascending!" : "You feel the arrival of monsters!");

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

				if (wizard || !rn2(10)) pline(FunnyHallu ? "The colors, the colors!" : "You feel a colorful sensation!");

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
				monstercolor = rnd(376);
			      cx = rn2(COLNO);
			      cy = rn2(ROWNO);

				if (wizard || !rn2(10)) pline(FunnyHallu ? "Someone got in here! Who could that be?" : "You feel that a group has arrived!");

				for (i = 0; i < randsp; i++) {

					if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

					(void) makemon(specialtensmon(monstercolor), cx, cy, MM_ADJACENTOK);
				}
			}

			if (uarmf && uarmf->otyp == ZIPPER_BOOTS && !EWounded_legs) EWounded_legs = 1;

			/* small chance of scaring yourself if you stand on Elbereth, even if you engraved it --Amy */
			if (sengr_at("Elbereth", u.ux, u.uy) && !rn2(isfriday ? 50 : 100) && !Blind ) {
				pline("As you see the Elder Sign written on the %s at your %s, you suddenly panic!",surface(u.ux,u.uy), makeplural(body_part(FOOT)) );
				make_feared(HFeared + rnd(10 + (monster_difficulty()) ),TRUE);
				}

		    if(!rn2( ((u.uevent.udemigod && !u.freeplaymode && u.amuletcompletelyimbued) || u.uprops[STORM_HELM].extrinsic) ? 250 :
			    (depth(&u.uz) > depth(&stronghold_level)) ? 450 : 500)) {
			if (!ishomicider) (void) makemon((struct permonst *)0, 0, 0, MM_MAYSLEEP);
			else makerandomtrap(FALSE);
			}

	/* still keeping the old monstermaking routine up, but drastically reducing their spawn rate. --Amy */

		    /* calculate how much time passed. */
		    if (u.usteed && u.umoved) {
			/* your speed doesn't augment steed's speed */
			moveamt = mcalcmove(u.usteed);

			if (Race_if(PM_CARTHAGE) && (mcalcmove(u.usteed) > 12)) moveamt = 12;

			register int steedmultiplier = 5;
			register int speedreduction;

			/* Riding a really fast (speed higher than 17) steed does not necessarily allow you to ride it at
			 * full speed, but depends on your riding skill. The actual speed will never be lower than 17, but
			 * now you need grand master riding skill to ride the steed at its actual speed. --Amy */

			if (!(PlayerCannotUseSkills)) {

				if (P_SKILL(P_RIDING) == P_BASIC) steedmultiplier = 7;
				if (P_SKILL(P_RIDING) == P_SKILLED) steedmultiplier = 9;
				if (P_SKILL(P_RIDING) == P_EXPERT) steedmultiplier = 11;
				if (P_SKILL(P_RIDING) == P_MASTER) steedmultiplier = 13;
				if (P_SKILL(P_RIDING) == P_GRAND_MASTER) steedmultiplier = 15;
				if (P_SKILL(P_RIDING) == P_SUPREME_MASTER) steedmultiplier = 15;

			}
			if (Race_if(PM_PERVERT)) steedmultiplier = 15; /* can always ride at max speed */

			if (uimplant && uimplant->oartifact == ART_READY_FOR_A_RIDE) {
				moveamt *= 6;
				moveamt /= 5;
			}

			if (moveamt > 17) {
				speedreduction = (moveamt - 17);
				speedreduction *= steedmultiplier;
				speedreduction /= 15;
				moveamt = 17 + speedreduction;
			}

			/* if you are slowed, you shouldn't be able to just completely ignore the slowness just because you're
			 * riding; "heavyweight" races (arbitrary) are affected more because they're harder to carry --Amy */

			if ((Race_if(PM_WEAPONIZED_DINOSAUR) || Race_if(PM_TURTLE) || Race_if(PM_LOWER_ENT)) ? rn2(3) : !rn2(3)) {
				if (youmonst.data->mmove < 12 && moveamt > 1) {
					moveamt *= youmonst.data->mmove;
					moveamt /= 12;
				}

				if (Race_if(PM_ASGARDIAN) && !rn2(20) && moveamt > 1)
					moveamt /= 2;

				if (Race_if(PM_PLAYER_FAIRY)) {
					if (uwep && uwep->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
					if (u.twoweap && uswapwep && uswapwep->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
					if (uarm && uarm->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
					if (uarmc && uarmc->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
					if (uarmh && uarmh->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
					if (uarms && uarms->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
					if (uarmg && uarmg->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
					if (uarmf && uarmf->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
					if (uarmu && uarmu->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
					if (uamul && uamul->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
					if (uimplant && uimplant->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
					if (uleft && uleft->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
					if (uright && uright->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
					if (ublindf && ublindf->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
				}

				if (Race_if(PM_SPIRIT) && !rn2(8) && moveamt > 1)
					moveamt /= 2;

				if (Race_if(PM_TONBERRY) && !rn2(8) && moveamt > 1)
					moveamt /= 2;

				if (Race_if(PM_CARTHAGE) && !rn2(8) && moveamt > 1)
					moveamt /= 2;

				if (Race_if(PM_MONGUNG) && !rn2(8) && moveamt > 1)
					moveamt /= 2;

				if (uwep && uwep->oartifact == ART_ETRUSCIAN_SWIMMING_LESSON && !rn2(8) && moveamt > 1)
					moveamt /= 2;

				if (u.twoweap && uswapwep && uswapwep->oartifact == ART_ETRUSCIAN_SWIMMING_LESSON && !rn2(8) && moveamt > 1)
					moveamt /= 2;

				if (Race_if(PM_PLAYER_HULK) && !rn2(8) && moveamt > 1)
					moveamt /= 2;

				if (Race_if(PM_FRO) && !rn2(8) && moveamt > 1)
					moveamt /= 2;

				if (uarmf && uarmf->oartifact == ART_ELEVECULT && !rn2(8) && moveamt > 1)
					moveamt /= 2;

				if (uarmf && !rn2(6) && (moveamt > 1) && itemhasappearance(uarmf, APP_BALLET_HEELS))
					moveamt /= 2;

				if (uamul && uamul->oartifact == ART_APATHY_STRATEGY && (moveamt > 1) && !rn2(2))
					moveamt /= 2;

				if (uarmc && uarmc->otyp == NASTY_CLOAK && !rn2(8) && moveamt > 1)
					moveamt /= 2;
				if (uarm && uarm->otyp == ROBE_OF_NASTINESS && !rn2(8) && moveamt > 1)
					moveamt /= 2;
				if (uarmh && uarmh->otyp == UNWANTED_HELMET && !rn2(8) && moveamt > 1)
					moveamt /= 2;
				if (uarmg && uarmg->otyp == EVIL_GLOVES && !rn2(8) && moveamt > 1)
					moveamt /= 2;
				if (uarmf && uarmf->otyp == UNFAIR_STILETTOS && !rn2(8) && moveamt > 1)
					moveamt /= 2;
				if (uarm && uarm->otyp == EVIL_DRAGON_SCALE_MAIL && !rn2(8) && moveamt > 1)
					moveamt /= 2;
				if (uarm && uarm->otyp == EVIL_DRAGON_SCALES && !rn2(8) && moveamt > 1)
					moveamt /= 2;
				if (uarms && uarms->otyp == EVIL_DRAGON_SCALE_SHIELD && !rn2(8) && moveamt > 1)
					moveamt /= 2;
				if (uarms && uarms->otyp == DIFFICULT_SHIELD && !rn2(8) && moveamt > 1)
					moveamt /= 2;
				if (uarmu && uarmu->otyp == BAD_SHIRT && !rn2(8) && moveamt > 1)
					moveamt /= 2;
				if (uarm && uarm->otyp == EVIL_PLATE_MAIL && !rn2(8) && moveamt > 1)
					moveamt /= 2;
				if (uarm && uarm->otyp == EVIL_LEATHER_ARMOR && !rn2(8) && moveamt > 1)
					moveamt /= 2;

				if (is_sand(u.ux,u.uy) && !(uarmf && itemhasappearance(uarmf, APP_SAND_ALS)) && !(uarmh && itemhasappearance(uarmh, APP_SHEMAGH)) && !(uarmf && uarmf->otyp == STILETTO_SANDALS) && !Race_if(PM_DUTHOL) && !sandprotection() && !Flying && !Levitation && !rn2(4) && moveamt > 1)
					moveamt /= 2;

				if (uarmc && uarmc->oartifact == ART_WEB_OF_THE_CHOSEN && !rn2(8) && moveamt > 1)
					moveamt /= 2;

				if (uarm && (uarm->oartifact == ART_CD_ROME_ARENA) && !rn2(8) && moveamt > 1)
					moveamt /= 2;

				if (uarmf && itemhasappearance(uarmf, APP_ROMAN_SANDALS) && !rn2(8) && moveamt > 1 ) /* Roman sandals aren't made for running. */
					moveamt /= 2;

				if (Race_if(PM_SOVIET) && !rn2(8) && moveamt > 1)
					moveamt /= 2;

				if (Race_if(PM_ARMED_COCKATRICE) && !Upolyd && !rn2(4) && moveamt > 1)
					moveamt /= 2;

				if (Race_if(PM_WEAPON_CUBE) && !Upolyd && !rn2(4) && moveamt > 1)
					moveamt /= 2;

				if (Race_if(PM_CORTEX) && !Upolyd && !rn2(4) && moveamt > 1)
					moveamt /= 2;

				if (Numbed && moveamt > 1) {
					if (!rn2(10))
					moveamt = 0;
				}
				if (Frozen && (!(uarmf && uarmf->oartifact == ART_VERA_S_FREEZER) || !rn2(3)) && moveamt > 1) {
					moveamt /= 2;
				}

				if (is_snow(u.ux, u.uy) && (u.umoved || !rn2(4)) && !Flying && !Levitation) {
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
						    || (uwep && uwep->oartifact == ART_GLACIERDALE)
						    || (uarmf && uarmf->oartifact == ART_BRIDGE_SHITTE)
						    || (uarmf && uarmf->oartifact == ART_IMPOSSIBLE_CATWALK)
						    || (uarmf && uarmf->oartifact == ART_MERLOT_FUTURE)) canwalkonsnow = 1;

					if (powerfulimplants() && uimplant && uimplant->oartifact == ART_WHITE_WHALE_HATH_COME) canwalkonsnow = 1;

					if (!canwalkonsnow)
					moveamt /= 4;

				}

				if ((uwep && uwep->oartifact == ART_KINGS_RANSOM_FOR_YOU) && moveamt > 1) {
					moveamt /= 2;
				}
				if ((u.twoweap && uswapwep && uswapwep->oartifact == ART_KINGS_RANSOM_FOR_YOU) && moveamt > 1) {
					moveamt /= 2;
				}
				if ((uwep && uwep->otyp == COLOSSUS_BLADE) && moveamt > 1) {
					moveamt /= 2;
				}
				if ((u.twoweap && uswapwep && uswapwep->otyp == COLOSSUS_BLADE) && moveamt > 1) {
					moveamt /= 2;
				}
				if (Race_if(PM_DUFFLEPUD) && uarmf && moveamt > 1) {
					moveamt /= 2;
				}
				if (Race_if(PM_WEAPONIZED_DINOSAUR) && uarmf && !PlayerInHighHeels && moveamt > 1) {
					moveamt /= 2;
				}
				if ((uarmc && itemhasappearance(uarmc, APP_SLOWING_GOWN) ) && moveamt > 1) {
					moveamt /= 2;
				}

				if (uarmh && uarmh->oartifact == ART_ELONA_S_SNAIL_TRAIL && !Race_if(PM_ELONA_SNAIL) && moveamt > 1) {
					moveamt /= 2;
				}

				if ((uarmf && itemhasappearance(uarmf, APP_FETISH_HEELS)) && u.umoved && moveamt > 1) {
					moveamt /= 2;
				}
				if ((uarmf && itemhasappearance(uarmf, APP_VELCRO_SANDALS)) && u.umoved && moveamt > 1) {
					moveamt /= 2;
				}
				if (u.inertia && moveamt > 1) {
					moveamt /= 2;
				}
				if (Race_if(PM_TURTLE) && moveamt > 1) {
					moveamt /= 2;
				}
				if (Race_if(PM_ELONA_SNAIL) && moveamt > 1) {
					moveamt /= 2;
				}
				if (!Upolyd && Race_if(PM_SHELL) && moveamt > 1) {
					moveamt /= 2;
				}
				if (Race_if(PM_LOWER_ENT) && moveamt > 1) {
					moveamt /= 2;
				}
				if (Role_if(PM_TRANSSYLVANIAN) && (moveamt > 1) && (!PlayerInHighHeels) ) {
					moveamt /= 2;
				}
				if (uarm && uarm->oartifact == ART_WEB_OF_LOLTH && moveamt > 1) {
					moveamt /= 2;
				}
				if (uarm && uarm->oartifact == ART_ROFLCOPTER_WEB && moveamt > 1) {
					moveamt /= 2;
				}
				if (uwep && uwep->oartifact == ART_ARABELLA_S_WARDING_HOE && moveamt > 1) {
					moveamt /= 2;
				}
				if (u.twoweap && uswapwep && uswapwep->oartifact == ART_ARABELLA_S_WARDING_HOE && moveamt > 1) {
					moveamt /= 2;
				}
				if (uarmf && uarmf->oartifact == ART_GOEFFELBOEFFEL && moveamt > 1) {
					moveamt /= 2;
				}

				if (Double_attack && moveamt > 1) {
					if (rn2(StrongDouble_attack ? 2 : 3))
					moveamt /= 2;
				}
				if (Quad_attack && moveamt > 1) {
					if (!StrongQuad_attack || rn2(3))
					moveamt /= 2;
				}

				if (u.hanguppenalty && moveamt > 1) {
					moveamt /= 2;
				}

				if (YouHaveTheSpeedBug && moveamt > 1) {
					if (rn2(5)) moveamt *= rnd(5);
					moveamt /= rnd(6);
					if (!rn2(5)) moveamt /= 2;
				}

				if (YouHaveTheSpeedBug && moveamt > 1 && StrongFast) {
					if (rn2(5)) moveamt *= rnd(5);
					moveamt /= rnd(6);
					if (!rn2(5)) moveamt /= 2;
				}

				if (Very_fast && YouHaveTheSpeedBug && rn2(4) && rn2(4) && moveamt > 1 ) {
					moveamt /= 2;
				} else if (Fast && YouHaveTheSpeedBug && !rn2(4) && moveamt > 1 ) {
					moveamt /= 2;
				}

			} /* chance to reduce speed end */
			if (moveamt < 1) moveamt = 1; /* don't reduce it too much, no matter what happens --Amy */

			/* clockworks can't become too fast even when riding (sorry) --Amy */
			if (Race_if(PM_CLOCKWORK_AUTOMATON) && rn2(Upolyd ? 2 : 3) && moveamt > 12) {

				moveamt += rnd(9);
				int moveamtdivider = moveamt - 12;
				if (moveamtdivider < 0) moveamtdivider = 0;
				moveamt = 12;
				if (rn2(10)) moveamt += (moveamtdivider / 10);
				else moveamt += (moveamtdivider / (1 + rnd(8)));

			}

		    } else /* not riding */
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

			/* special case: if you're polymorphed into your symbiote's base monster, you can move around --Amy
			 * This is mainly so that the "assume symbiote" technique works without needing to be active */
			if (youmonst.data->mmove == 0 && uactivesymbiosis && Upolyd && (u.umonnum == u.usymbiote.mnum)) {
				moveamt += 12;
			}

			if (youmonst.data->mmove == 0 && !rn2(2)) moveamt += 1; /* be lenient if an ungenomold player is unlucky 		 * enough to poly into a red mold or something like that. Otherwise they would simply die with no chance.
		 * see hack.c code that still prevents movement if polymorphed into something sessile.
		 * Also, you're still slower than a lichen (speed 1), so this should be ok. */

			if (Race_if(PM_ASGARDIAN) && !rn2(20) && moveamt > 1) /* Asgardians are slower sometimes, this is intentional. --Amy */
				moveamt /= 2;

			/* fairy is slowed by heavy gear; many slots don't actually have items with enough weight,
			 * but I'm coding it for every slot anyway just to make sure --Amy */
			if (Race_if(PM_PLAYER_FAIRY)) {
				if (uwep && uwep->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
				if (u.twoweap && uswapwep && uswapwep->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
				if (uarm && uarm->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
				if (uarmc && uarmc->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
				if (uarmh && uarmh->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
				if (uarms && uarms->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
				if (uarmg && uarmg->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
				if (uarmf && uarmf->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
				if (uarmu && uarmu->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
				if (uamul && uamul->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
				if (uimplant && uimplant->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
				if (uleft && uleft->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
				if (uright && uright->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
				if (ublindf && ublindf->owt > 15 && !rn2(6) && moveamt > 1) moveamt /= 2;
			}

			if (Race_if(PM_SPIRIT) && !rn2(8) && moveamt > 1) /* Spirits too are slower sometimes. */
				moveamt /= 2;

			if (Race_if(PM_TONBERRY) && !rn2(8) && moveamt > 1)
				moveamt /= 2;

			if (Race_if(PM_CARTHAGE) && !rn2(8) && moveamt > 1)
				moveamt /= 2;

			if (Race_if(PM_BOVER)) { /* is slowed by lithic armor unless riding */
				if (uarm && is_lithic(uarm) && !rn2(8) && moveamt > 1) moveamt /= 2;
				if (uarmu && is_lithic(uarmu) && !rn2(8) && moveamt > 1) moveamt /= 2;
				if (uarmc && is_lithic(uarmc) && !rn2(8) && moveamt > 1) moveamt /= 2;
				if (uarmg && is_lithic(uarmg) && !rn2(8) && moveamt > 1) moveamt /= 2;
				if (uarmh && is_lithic(uarmh) && !rn2(8) && moveamt > 1) moveamt /= 2;
				if (uarmf && is_lithic(uarmf) && !rn2(8) && moveamt > 1) moveamt /= 2;
				if (uarms && is_lithic(uarms) && !rn2(8) && moveamt > 1) moveamt /= 2;
			}

			if (Race_if(PM_MONGUNG) && !rn2(8) && moveamt > 1)
				moveamt /= 2;

			if (uwep && uwep->oartifact == ART_ETRUSCIAN_SWIMMING_LESSON && !rn2(8) && moveamt > 1)
				moveamt /= 2;

			if (u.twoweap && uswapwep && uswapwep->oartifact == ART_ETRUSCIAN_SWIMMING_LESSON && !rn2(8) && moveamt > 1)
				moveamt /= 2;

			if (Race_if(PM_PLAYER_HULK) && !rn2(8) && moveamt > 1)
				moveamt /= 2;

			if (Race_if(PM_FRO) && !rn2(8) && moveamt > 1)
				moveamt /= 2;

			if (uarmf && uarmf->oartifact == ART_ELEVECULT && !rn2(8) && moveamt > 1)
				moveamt /= 2;

			if (uarmf && !rn2(6) && (moveamt > 1) && itemhasappearance(uarmf, APP_BALLET_HEELS))
				moveamt /= 2;

			if (uamul && uamul->oartifact == ART_APATHY_STRATEGY && (moveamt > 1) && !rn2(2))
				moveamt /= 2;

			/* nasty equipment generally makes you slower because it needs a guaranteed downside, even if the
			 * randomized nastytrap effect is something relatively benign - this equipment is supposed to be
			 * nasty and something that you don't wanna wear casually! --Amy */
			if (uarmc && uarmc->otyp == NASTY_CLOAK && !rn2(8) && moveamt > 1)
				moveamt /= 2;
			if (uarm && uarm->otyp == ROBE_OF_NASTINESS && !rn2(8) && moveamt > 1)
				moveamt /= 2;
			if (uarmh && uarmh->otyp == UNWANTED_HELMET && !rn2(8) && moveamt > 1)
				moveamt /= 2;
			if (uarmg && uarmg->otyp == EVIL_GLOVES && !rn2(8) && moveamt > 1)
				moveamt /= 2;
			if (uarmf && uarmf->otyp == UNFAIR_STILETTOS && !rn2(8) && moveamt > 1)
				moveamt /= 2;
			if (uarm && uarm->otyp == EVIL_DRAGON_SCALE_MAIL && !rn2(8) && moveamt > 1)
				moveamt /= 2;
			if (uarm && uarm->otyp == EVIL_DRAGON_SCALES && !rn2(8) && moveamt > 1)
				moveamt /= 2;
			if (uarms && uarms->otyp == EVIL_DRAGON_SCALE_SHIELD && !rn2(8) && moveamt > 1)
				moveamt /= 2;
			if (uarms && uarms->otyp == DIFFICULT_SHIELD && !rn2(8) && moveamt > 1)
				moveamt /= 2;
			if (uarmu && uarmu->otyp == BAD_SHIRT && !rn2(8) && moveamt > 1)
				moveamt /= 2;
			if (uarm && uarm->otyp == EVIL_PLATE_MAIL && !rn2(8) && moveamt > 1)
				moveamt /= 2;
			if (uarm && uarm->otyp == EVIL_LEATHER_ARMOR && !rn2(8) && moveamt > 1)
				moveamt /= 2;

			if (is_sand(u.ux,u.uy) && !(uarmf && itemhasappearance(uarmf, APP_SAND_ALS)) && !(uarmh && itemhasappearance(uarmh, APP_SHEMAGH)) && !(uarmf && uarmf->otyp == STILETTO_SANDALS) && !Race_if(PM_DUTHOL) && !sandprotection() && !Flying && !Levitation && !rn2(4) && moveamt > 1)
				moveamt /= 2;

			if (uarmc && uarmc->oartifact == ART_WEB_OF_THE_CHOSEN && !rn2(8) && moveamt > 1)
				moveamt /= 2;

			if (uarm && (uarm->oartifact == ART_CD_ROME_ARENA) && !rn2(8) && moveamt > 1) /* roman clothing just generally slows you down */
				moveamt /= 2;

			if (uarmf && itemhasappearance(uarmf, APP_ROMAN_SANDALS) && !rn2(8) && moveamt > 1 ) /* Roman sandals aren't made for running. */
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

			if (is_snow(u.ux, u.uy) && (u.umoved || !rn2(4)) && !Flying && !Levitation) {
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
					    || (uwep && uwep->oartifact == ART_GLACIERDALE)
					    || (uarmf && uarmf->oartifact == ART_BRIDGE_SHITTE)
					    || (uarmf && uarmf->oartifact == ART_IMPOSSIBLE_CATWALK)
					    || (uarmf && uarmf->oartifact == ART_MERLOT_FUTURE)) canwalkonsnow = 1;

				if (powerfulimplants() && uimplant && uimplant->oartifact == ART_WHITE_WHALE_HATH_COME) canwalkonsnow = 1;

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

			/* mushroom moves more slowly, but only when not riding and not polymorphed --Amy */
			if (Race_if(PM_PLAYER_MUSHROOM) && !Upolyd && u.umoved && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2)) {
					moveamt *= 2;
					moveamt /= 3;
				}
			}

			if (Race_if(PM_DUFFLEPUD) && uarmf && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2; /* dufflepud wearing boots moves at half speed --Amy */
			}
			if (Race_if(PM_WEAPONIZED_DINOSAUR) && uarmf && !PlayerInHighHeels && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2; /* dinosaur wearing non-high-heeled boots moves at half speed --Amy */
			}
			if ((uarmc && itemhasappearance(uarmc, APP_SLOWING_GOWN) ) && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2;
			}

			if (uarmh && uarmh->oartifact == ART_ELONA_S_SNAIL_TRAIL && !Race_if(PM_ELONA_SNAIL) && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2;
			}

			if ((uarmf && itemhasappearance(uarmf, APP_FETISH_HEELS)) && u.umoved && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2;
			}
			if ((uarmf && itemhasappearance(uarmf, APP_VELCRO_SANDALS)) && u.umoved && moveamt > 1) {
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
			if (!Upolyd && Race_if(PM_SHELL) && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2;
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
				if ((youmonst.data->mmove > 1 || !rn2(2)) && rn2(StrongDouble_attack ? 2 : 3))
				moveamt /= 2;
			}
			if (Quad_attack && moveamt > 1) {
				if ((youmonst.data->mmove > 1 || !rn2(2)) && (!StrongQuad_attack || rn2(3)) )
				moveamt /= 2;
			}

			if (u.hanguppenalty && moveamt > 1) {
				if (youmonst.data->mmove > 1 || !rn2(2))
				moveamt /= 2; /* punishment for attempting hangup cheat --Amy */
			}

			if (YouHaveTheSpeedBug && moveamt > 1) { /* speed bug messes up the player's speed --Amy */
				if (rn2(5)) moveamt *= rnd(5);
				moveamt /= rnd(6);
				if (!rn2(5)) moveamt /= 2;
			}

			if (YouHaveTheSpeedBug && moveamt > 1 && StrongFast) {
				if (rn2(5)) moveamt *= rnd(5);
				moveamt /= rnd(6);
				if (!rn2(5)) moveamt /= 2;
			}

			/* speed bug reverses speed effects --Amy */
			if (Very_fast && YouHaveTheSpeedBug && rn2(4) && rn2(4) && moveamt > 1 ) {	/* speed boots or potion */
			    /* average movement is 0.5625 times normal */

				moveamt /= 2;

			} else if (Fast && YouHaveTheSpeedBug && !rn2(4) && moveamt > 1 ) {
			    /* average movement is 0.75 times normal */

				moveamt /= 2;
			}

			if (moveamt < 0) moveamt = 0;

			if (Very_fast && !YouHaveTheSpeedBug) {	/* speed boots or potion */
			    /* average movement is 1.67 times normal */
			    if ((StrongFast || rn2(3)) && (!Race_if(PM_FRO) || !rn2(2)) && (!Race_if(PM_MACTHEIST) || !rn2(2)) ) {
				    moveamt += speedbonus(moveamt / 2, NORMAL_SPEED / 2);
				    if (rn2(3) == 0) moveamt += speedbonus(moveamt / 2, NORMAL_SPEED / 2);
			    }
			} else if (Fast && !YouHaveTheSpeedBug && (!Race_if(PM_FRO) || !rn2(2)) && (!Race_if(PM_MACTHEIST) || !rn2(2)) ) {
			    /* average movement is 1.33 times normal */
			    if (rn2(3) != 0) moveamt += speedbonus(moveamt / 2, NORMAL_SPEED / 2);
			}

			if (Fear_factor && Feared) {
			    if (rn2(3) != 0) moveamt += speedbonus(moveamt / 2, NORMAL_SPEED / 2);
			}

			if (Wonderlegs && Wounded_legs) {
				moveamt *= 5;
				moveamt /= 4;
			}

			/* unicorns are ultra fast!!! However, they have enough bullshit downsides to reign them in. --Amy */
			if (Race_if(PM_PLAYER_UNICORN)) {
				moveamt *= 2;
			}

			/* metals are even faster but take greatly increased damage --Amy */
			if (Race_if(PM_METAL)) {
				moveamt *= 3;
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

			if (powerfulimplants() && uimplant && uimplant->oartifact == ART_ETHERATORGARDEN) {
				moveamt *= 6;
				moveamt /= 5;
			}

			if (powerfulimplants() && uimplant && uimplant->oartifact == ART_YOU_SHOULD_SURRENDER) {
				moveamt *= 3;
				moveamt /= 2;
			}

			if (uimplant && uimplant->oartifact == ART_BRRRRRRRRRRRRRMMMMMM) {
				if (is_highway(u.ux, u.uy) || powerfulimplants()) {
					moveamt *= 2;
				}
			}

			if (uarmg && uarmg->oartifact == ART_LINE_CAN_PLAY_BY_YOURSELF) moveamt *= 2;

			if (uarmh && (uarmh->oartifact == ART_REAL_SPEED_DEVIL) && !rn2(10)) moveamt += speedbonus(moveamt / 2, NORMAL_SPEED / 2);
			if (uwep && uwep->oartifact == ART_LULWY_S_TRICK && !rn2(10)) moveamt += speedbonus(moveamt / 2, NORMAL_SPEED / 2);
			if (uarmf && (uarmf->oartifact == ART_VRRRRRRRRRRRR) && !rn2(5)) moveamt += speedbonus(moveamt / 2, NORMAL_SPEED / 2);
			if (uarmh && (uarmh->oartifact == ART_LORSKEL_S_SPEED) && !rn2(10)) moveamt += speedbonus(moveamt / 2, NORMAL_SPEED / 2);
			if (uarmf && (uarmf->oartifact == ART_HIGHEST_FEELING) && !rn2(2)) moveamt += speedbonus(moveamt / 2, NORMAL_SPEED / 2);
			if (uarmc && (uarmc->oartifact == ART_WINDS_OF_CHANGE) && !rn2(10)) moveamt += speedbonus(moveamt / 2, NORMAL_SPEED / 2);
			if (uarm && (uarm->oartifact == ART_FORMULA_ONE_SUIT) && !rn2(10)) moveamt += speedbonus(moveamt / 2, NORMAL_SPEED / 2);
			if (uarmh && !rn2(10) && itemhasappearance(uarmh, APP_FORMULA_ONE_HELMET) ) moveamt += speedbonus(moveamt / 2, NORMAL_SPEED / 2);
			if (uarmf && !rn2(10) && itemhasappearance(uarmf, APP_TURBO_BOOTS) ) moveamt += speedbonus(moveamt / 2, NORMAL_SPEED / 2);
			if (uarmg && !rn2(10) && itemhasappearance(uarmg, APP_RACER_GLOVES) ) moveamt += speedbonus(moveamt / 2, NORMAL_SPEED / 2);
			if (StrongDetect_monsters && !rn2(10)) moveamt += speedbonus(moveamt / 2, NORMAL_SPEED / 2);
			if (StrongFlying && !rn2(20)) moveamt += speedbonus(moveamt / 2, NORMAL_SPEED / 2);

			if (PlayerInHighHeels && !rn2(10) && !(PlayerCannotUseSkills) && (P_SKILL(P_HIGH_HEELS) >= P_MASTER) ) moveamt += speedbonus(moveamt / 2, NORMAL_SPEED / 2);
			if (PlayerInHighHeels && !rn2(10) && !(PlayerCannotUseSkills) && (P_SKILL(P_HIGH_HEELS) >= P_GRAND_MASTER) ) moveamt += speedbonus(moveamt / 2, NORMAL_SPEED / 2);
			if (PlayerInHighHeels && !rn2(10) && !(PlayerCannotUseSkills) && (P_SKILL(P_HIGH_HEELS) >= P_SUPREME_MASTER) ) moveamt += speedbonus(moveamt / 2, NORMAL_SPEED / 2);

			if (!rn2(10) && uarmc && itemhasappearance(uarmc, APP_GREEK_CLOAK) ) moveamt += speedbonus(moveamt / 2, NORMAL_SPEED / 2);

			if (uarmf && uarmf->oartifact == ART_WARP_SPEED && (is_waterypool(u.ux, u.uy) || is_watertunnel(u.ux, u.uy))) moveamt += (speedbonus(moveamt * 5, NORMAL_SPEED * 5));

			if (Race_if(PM_DUTHOL) && is_sand(u.ux, u.uy)) moveamt += speedbonus(moveamt / 2, NORMAL_SPEED / 2);

			if (Race_if(PM_CELTIC) && (rn2(100) < u.ulevel)) moveamt += speedbonus(moveamt, NORMAL_SPEED);

			if (Race_if(PM_SERB) && !rn2(10)) moveamt += speedbonus(moveamt, NORMAL_SPEED);

			if (StrongPasses_walls && !rn2(3) && isok(u.ux, u.uy) && IS_STWALL(levl[u.ux][u.uy].typ) ) moveamt += speedbonus(moveamt / 2, NORMAL_SPEED / 2);
			if (StrongFast && !rn2(10) && (!Race_if(PM_FRO) || !rn2(2)) && (!Race_if(PM_MACTHEIST) || !rn2(2)) && !YouHaveTheSpeedBug) moveamt += speedbonus(moveamt / 2, NORMAL_SPEED / 2);

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
			    moveamt += speedbonus(moveamt * 2 / 3, NORMAL_SPEED * 2 / 3);
			    if (rn2(3) == 0) moveamt += speedbonus(moveamt / 2, NORMAL_SPEED / 2);
			}

			if (uarmc && uarmc->oartifact == ART_LIGHTSPEED_TRAVEL) {
			    if (rn2(3) == 0) moveamt += speedbonus(moveamt / 2, NORMAL_SPEED / 2);
			}

			/* clockwork gets bullshit downside: I know this is heavy-handed, but they're just plain too strong
			 * once they manage to control their food woes. So I decided that "since it would otherwise cause them
			 * to become overwound", they have cruise control that prevents them from being much faster than
			 * normal speed (12). Speed boots will still help a little but certainly not that much.
			 * Originally I was just setting the speed to 12 but that was too harsh. */
			if (Race_if(PM_CLOCKWORK_AUTOMATON) && rn2(Upolyd ? 3 : 10) && moveamt > 12) {

				moveamt += rnd(9);
				int moveamtdivider = moveamt - 12;
				if (moveamtdivider < 0) moveamtdivider = 0;
				moveamt = 12;
				if (rn2(10)) moveamt += (moveamtdivider / 10);
				else moveamt += (moveamtdivider / (1 + rnd(8)));

			}

		    } /* end adjustment for when player is not riding */

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

		    if (uimplant && uimplant->oartifact == ART_RESTROOM_DRENCHING) {
				int drenchfactor = 100;
				if (u.uhp < (u.uhpmax * 4 / 5)) drenchfactor = (flags.female ? 120 : 110);
				if (u.uhp < (u.uhpmax * 3 / 5)) drenchfactor = (flags.female ? 140 : 120);
				if (u.uhp < (u.uhpmax * 2 / 5)) drenchfactor = (flags.female ? 150 : 125);
				if (u.uhp < (u.uhpmax * 3 / 10)) drenchfactor = (flags.female ? 160 : 130);
				if (u.uhp < (u.uhpmax * 1 / 5)) drenchfactor = (flags.female ? 175 : 138);
				if (u.uhp < (u.uhpmax * 1 / 10)) drenchfactor = (flags.female ? 200 : 150);

				moveamt *= drenchfactor;
				moveamt /= 100;

		    }

			/* fluctuating speed - sadly jonadab never fully disclosed how that bug worked in fourk... */
		    if ((FluctuatingSpeed || u.uprops[FLUCTUATING_SPEED].extrinsic || have_fluctuatingspeedstone()) && moveamt > 0) {
			if (uarmf && uarmf->oartifact == ART_JONADAB_S_BUG_MASTERY) {
				if ((moves % 80) < 10) {
					moveamt /= 12;
					if (moveamt < 1) moveamt = 1;
				} else {
					moveamt *= ((moves % 80) - 10);
					moveamt /= 12;	
					if (moveamt < 1) moveamt = 1;
				}
			} else {
				if ((moves % 60) < 10) {
					moveamt /= 12;
					if (moveamt < 1) moveamt = 1;
				} else {
					moveamt *= ((moves % 60) - 10);
					moveamt /= 12;	
					if (moveamt < 1) moveamt = 1;
				}
			}
		    }

		    youmonst.movement += moveamt;
		    if (youmonst.movement < 0) youmonst.movement = 0;
		    settrack();

		    if (!rn2(2) || !(uarmf && itemhasappearance(uarmf, APP_IRREGULAR_BOOTS) ) ) {

			if (!rn2(2) || !((uleft && uleft->oartifact == ART_GOOD_THINGS_WILL_HAPPEN_EV) || (uright && uright->oartifact == ART_GOOD_THINGS_WILL_HAPPEN_EV)) ) {
				if (!rn2(2) || !RngeIrregularity) {
				    monstermoves++;
				    moves++;
				}
			}

		    }

			if (u.uprops[FAST_FORWARD].extrinsic) {

			    if (rn2(10)) {
				    monstermoves++;
				    moves++;
				    nh_timeout();
			    } else if (!rn2(2)) {
				    monstermoves += 2;
				    moves += 2;
				    nh_timeout();
				    nh_timeout();
			    }
			}

			if (have_fastforwardstone()) {

			    if (rn2(10)) {
				    monstermoves++;
				    moves++;
				    nh_timeout();
			    } else if (!rn2(2)) {
				    monstermoves += 2;
				    moves += 2;
				    nh_timeout();
				    nh_timeout();
			    }

			}

			if (TimeGoesByFaster) {

				int veryfasttime;

			    if (rn2(10)) {
				    monstermoves++;
				    moves++;
				    nh_timeout();
			    } else if (!rn2(2)) {
				    monstermoves += 2;
				    moves += 2;
				    nh_timeout();
				    nh_timeout();
			    }

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

		if (Race_if(PM_PERVERT)) {
			u.pervertsex++;
			u.pervertpray++;
			if (!(u.pervertsex % 10000)) {
				You("didn't have sex in 10000 turns and thereby suffer from withdrawal!");
			} else if (!(u.pervertsex % 1000)) {
				pline("Remember that you need to have sex once every 10000 turns or you'll suffer from withdrawal!");
			}
			if (!(u.pervertpray % 10000)) {
				You("didn't pray in 10000 turns and therefore the gods decide to debuff you!");
			} else if (!(u.pervertpray % 1000)) {
				pline("Remember that you need to pray once every 10000 turns or the gods will debuff you!");
			}
		}

		if (Race_if(PM_BOVER) && !rn2(2000)) {
			pline("UGH - you inhaled too much of your own repulsive body odor, and can no longer think straight!");
			make_confused(HConfusion + d(10,10), TRUE);
			turn_allmonsters();
		}

		if (!u.uinwater) u.udrowning = FALSE;
		if (u.udrowning) {
			if (isok(u.ux, u.uy) && is_crystalwater(u.ux, u.uy)) crystaldrown();
			else drown();
			stop_occupation();
		}

		if (u.shutdowntime) {
			u.shutdowntime--;
			if (!uinsymbiosis) u.shutdowntime = 0;
			if (u.shutdowntime < 0) u.shutdowntime = 0; /* fail safe */
			if (!u.shutdowntime) Your(uinsymbiosis ? "symbiote is no longer shut down." : "symbiote is no longer there and therefore its shutdown also ends.");
		}

		/* burden: rarely as a bad effect, more frequently as data delete effect; times out very slowly --Amy */
		if (u.graundweight) {
			if (!rn2(20)) u.graundweight--;
			if (u.graundweight < 0) u.graundweight = 0; /* fail safe */
		}

		if (u.ualign.record > u.cnd_maxalignment) u.cnd_maxalignment = u.ualign.record;
		if (u.ualign.record < u.cnd_minalignment) u.cnd_minalignment = u.ualign.record;
		if (u.usanity > u.cnd_maxsanity) u.cnd_maxsanity = u.usanity;
		if (u.contamination > u.cnd_maxcontamination) u.cnd_maxcontamination = u.contamination;

		if (!rn2(2)) u.funnyhalluroll = 9999999;
		else u.funnyhalluroll = rn2(10000);

		if (u.riderhack) u.riderhack = FALSE;

		if (!occupation) u.katitrapocc = FALSE;

		if (!rn2(100)) u.statuetrapname = rn2(NUMMONS);

		if (!Upolyd && u.polyformed) u.polyformed = 0; /* catch-all, because coding this in polyself.c is horrible --Amy */

		if (AutoDestruct || u.uprops[AUTO_DESTRUCT].extrinsic || (uarmf && uarmf->oartifact == ART_KHOR_S_REQUIRED_IDEA) || have_autodestructstone()) stop_occupation();
 
		if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR) && Feared && !rn2(100)) {
			pline("holy shit this is offensive");
			badeffect();
		}

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

		if (uimplant && uimplant->oartifact == ART_ETERNAL_SORENESS && u.inertia < 5) u.inertia = 15;

		if (uarmf && uarmf->oartifact == ART_KRISTIN_S_NOBILITY) {
			if (HStun > 10) HStun -= 5;
			if (HDimmed > 10) HDimmed -= 5;
		}

		/* beauty charm: tries each turn to pacify monsters around you, but only humanoids or animals --Amy */
		if (tech_inuse(T_BEAUTY_CHARM) && PlayerInHighHeels) {
			struct monst *mtmp3;
			int k, l;

		    for (k = -3; k <= 3; k++) for(l = -3; l <= 3; l++) {
			if (!isok(u.ux + k, u.uy + l)) continue;
			if ( ((mtmp3 = m_at(u.ux + k, u.uy + l)) != 0) && mtmp3->mtame == 0 && mtmp3->isshk == 0 && mtmp3->isgd 			== 0 && mtmp3->ispriest == 0 && mtmp3->isminion == 0 && mtmp3->isgyp == 0
&& mtmp3->data != &mons[PM_SHOPKEEPER] && mtmp3->data != &mons[PM_MASTER_SHOPKEEPER] && mtmp3->data != &mons[PM_ELITE_SHOPKEEPER] && mtmp3->data != &mons[PM_BLACK_MARKETEER] && mtmp3->data != &mons[PM_ALIGNED_PRIEST] && mtmp3->data != &mons[PM_MASTER_PRIEST] && mtmp3->data != &mons[PM_ELITE_PRIEST] && mtmp3->data != &mons[PM_HIGH_PRIEST] && mtmp3->data != &mons[PM_DNETHACK_ELDER_PRIEST_TM_] && mtmp3->data != &mons[PM_GUARD] && mtmp3->data != &mons[PM_MASTER_GUARD] && mtmp3->data != &mons[PM_ELITE_GUARD]
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

		if (tech_inuse(T_EXTRA_LONG_SQUEAK)) {
			struct monst *mtmp3;
			int k, l;

		    for (k = -3; k <= 3; k++) for(l = -3; l <= 3; l++) {
			if (!isok(u.ux + k, u.uy + l)) continue;
			if ( ((mtmp3 = m_at(u.ux + k, u.uy + l)) != 0) && mtmp3->mtame == 0 && mtmp3->mpeaceful == 0) {

				if (humanoid(mtmp3->data) && !resist(mtmp3, RING_CLASS, 0, NOTELL) && !mtmp3->female) {
					mtmp3->mhp -= techlevX(get_tech_no(T_EXTRA_LONG_SQUEAK));
					if (cansee(mtmp3->mx,mtmp3->my)) pline("%s inhales your farting gas and feels bad!", Monnam(mtmp3));
					if (mtmp3->mhp < 1) killed(mtmp3);
				}

			} /* monster is catchable loop */
		    } /* for loop */

			if (practicantterror) {
				pline("%s booms: 'Practicant lass, I collect 500 zorkmids per second for which you're farting in public.'", noroelaname());
				fineforpracticant(500, 0, 0);
			}

		}

		if (Role_if(PM_CELLAR_CHILD) && !rn2(5000)) bad_equipment(0);

		if (CursedParts && !rn2(500)) bad_equipment(0);

		if (uamul && uamul->oartifact == ART_ARABELLA_S_DICINATOR && !rn2(500)) bad_equipment(0);

		if (u.uprops[CURSED_PARTS].extrinsic && !rn2(500)) bad_equipment(0);

		if (have_cursedpartstone() && !rn2(500)) bad_equipment(0);

		if (uwep && uwep->oartifact == ART_EGRID_BUG && !rn2(500)) bad_equipment(0);

		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_EGRID_BUG && !rn2(500)) bad_equipment(0);

		if (AppearanceShuffling && !rn2(2000)) initobjectsamnesia();;

		if (u.uprops[APPEARANCE_SHUFFLING].extrinsic && !rn2(2000)) initobjectsamnesia();;

		if (have_appearanceshufflingstone() && !rn2(2000)) initobjectsamnesia();;

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

		/* from Elona: being too overloaded will periodically cause damage --Amy */
		if (near_capacity() >= OVERLOADED && !rn2(10)) {
			int howmuchistoomuch = 0;
			howmuchistoomuch = ((inv_weight() + weight_cap()) - (weight_cap() * 3));
			if (howmuchistoomuch < 0) howmuchistoomuch = 0;
			howmuchistoomuch /= 100;
			if (isfriday) howmuchistoomuch *= 2;
			if (howmuchistoomuch > 0) {
				Your("backpack is crushing you!");
				losehp(howmuchistoomuch, "crushed underneath the backpack's load", NO_KILLER_PREFIX);

			}
		}

		if (uarmc && (uarmc->oartifact == ART_INA_S_LAB_COAT) && !rn2(100) && multi >= 0) {

			if (!strncmpi(plname, "Ina", 4) || !strncmpi(plalias, "Ina", 4)) {
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

			if (!strncmpi(plname, "Ina", 4) || !strncmpi(plalias, "Ina", 4)) {
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
			pline("%s", soviettaunt());
		}

		if (Role_if(PM_GANG_SCHOLAR) && !rn2(1000)) {
			gangscholarmessage();
		}
		if (practicantterror && !rn2(1000)) {
			practicantmessage();
		}
		if (Role_if(PM_WALSCHOLAR) && !rn2(1000)) {
			walscholarmessage();
		}

		if (practicantterror && In_voiddungeon(&u.uz) && u.pract_void && (moves > u.pract_voidtimer)) {
			if (!u.pract_voidinitial) {
				pline("%s thunders: 'I called the kops. Get out of the void you fucking maggot or the police will give you a life-long sentence in jail!'", noroelaname());

				if (!rn2(20)) u.copwantedlevel += rnz(u.ualign.sins + 1);

				u.cnd_kopsummonamount++;
				int copcnt = rnd(monster_difficulty() ) + 1;
				if (rn2(5)) copcnt = (copcnt / (rnd(4) + 1)) + 1;
				if (Role_if(PM_CAMPERSTRIKER)) copcnt *= (rn2(5) ? 2 : rn2(5) ? 3 : 5);

				if (uarmh && itemhasappearance(uarmh, APP_ANTI_GOVERNMENT_HELMET) ) {
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
						(void) maketrap(x, y, KOP_CUBE, 0, FALSE);
						break;
						}
				}

			      while(--copcnt >= 0) {
					if (xupstair) (void) makemon(mkclass(S_KOP,0), xupstair, yupstair, rn2(3) ? MM_ANGRY|MM_ADJACENTOK : MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
					else (void) makemon(mkclass(S_KOP,0), 0, 0, rn2(3) ? MM_ANGRY|MM_ADJACENTOK : MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
				} /* while */

				u.pract_voidinitial = TRUE;
			}

			if (!rn2(100)) {

				if (!rn2(20)) u.copwantedlevel += rnz(u.ualign.sins + 1);
				if (!rn2(20)) {
					int pm;
					if ((pm = (!rn2(5) ? dprince(rn2((int)A_LAWFUL+2) - 1) : dlord(rn2((int)A_LAWFUL+2) - 1) ) ) != NON_PM) {
						(void) makemon(&mons[pm], u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
					}
				}

				u.cnd_kopsummonamount++;
				int copcnt = rnd(monster_difficulty() ) + 1;
				if (rn2(5)) copcnt = (copcnt / (rnd(4) + 1)) + 1;
				if (Role_if(PM_CAMPERSTRIKER)) copcnt *= (rn2(5) ? 2 : rn2(5) ? 3 : 5);

				if (uarmh && itemhasappearance(uarmh, APP_ANTI_GOVERNMENT_HELMET) ) {
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
						(void) maketrap(x, y, KOP_CUBE, 0, FALSE);
						break;
						}
				}

			      while(--copcnt >= 0) {
					if (xupstair) (void) makemon(mkclass(S_KOP,0), xupstair, yupstair, rn2(3) ? MM_ANGRY|MM_ADJACENTOK : MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
					else (void) makemon(mkclass(S_KOP,0), 0, 0, rn2(3) ? MM_ANGRY|MM_ADJACENTOK : MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
				} /* while */

			}

		}

		if (practicantterror && u.pract_toomanykills4) {
			if (u.pract_conv1timer > 0) u.pract_conv1timer--;
			if (u.pract_conv1timer < 1) {
				u.pract_conv1timer = 5000;
				fineforpracticant(1000, 0, 0);
				pline("%s booms: 'You have to pay 1000 zorkmids again for your past offense of killing too many monsters.'", noroelaname());
			}
		}

		if (practicantterror && u.pract_toomanysacs3) {
			if (u.pract_conv2timer > 0) u.pract_conv2timer--;
			if (u.pract_conv2timer < 1) {
				u.pract_conv2timer = 5000;
				fineforpracticant(1000, 0, 0);
				pline("%s booms: 'You have to pay 1000 zorkmids again for your past offense of sacrificing too many corpses.'", noroelaname());
			}
		}

		if (practicantterror && u.pract_peacedisturb) {
			if (u.pract_conv3timer > 0) u.pract_conv3timer--;
			if (u.pract_conv3timer < 1) {
				u.pract_conv3timer = 5000;
				fineforpracticant(2000, 0, 0);
				pline("%s booms: 'You have to pay 2000 zorkmids again for your past offense of murdering too many innocent townspeople.'", noroelaname());
			}
		}

		if (practicantterror && u.pract_magicresistance) {
			if (u.pract_conv4timer > 0) u.pract_conv4timer--;
			if (u.pract_conv4timer < 1) {
				u.pract_conv4timer = 5000;
				fineforpracticant(2000, 0, 0);
				pline("%s booms: 'You have to pay 2000 zorkmids again for your past offense of gaining an impossible intrinsic.'", noroelaname());
			}

		}

		if (practicantterror && u.pract_idlingtimer) {
			u.pract_idlingtimer--;
			if (u.pract_idlingtimer < 0) {
				u.pract_idlingtimer = 0; /* fail safe */
				u.pract_idling = FALSE;
			}
		}

		if (practicantterror && u.practicantcash) { /* Noroela, the bitch, spends your hard-earned cash */
			if (((u.practicantcash > 100) || !rn2(10)) && !rn2(5)) u.practicantcash--;
			if (u.practicantcash > 500 && !rn2(5000)) {
				int comcost = (u.practicantcash / 2);
				if (comcost > 5000) comcost = 5000;
				comcost = rnd(comcost);
				u.practicantcash -= comcost;
				noroelataunt();
			}
			if (u.practicantcash < 0) u.practicantcash = 0; /* fail safe */
		}

		if (practicantterror) {
			u.pract_finetimer++;
			if (u.pract_finetimer >= 10000) {
				fineforpracticant(1000, 0, 0); /* sets the finetimer var back to 0 */
				pline("%s thunders: 'You now have to pay a fine of 1000 zorkmds as a penalty for not having to pay a fine in such a long time!'", noroelaname());
			}
		}

		if (practicantterror && (u.practicantpenalty || u.practicantstones || u.practicantarrows)) {
			if (u.practicanttime > 0) u.practicanttime--;
			if (u.practicanttime == 500) {
				You("have 500 turns left to pay %d zorkmids to %s!", u.practicantpenalty, noroelaname());
				if (u.practicantstones) You("also still have %d rocks to pay!", u.practicantstones);
				if (u.practicantarrows) You("also still have %d arrows to pay!", u.practicantarrows);
			}
			if (u.practicanttime == 100) {
				You("only have 100 turns left to pay %d zorkmids to %s! Better pay up!", u.practicantpenalty, noroelaname());
				if (u.practicantstones) You("also still have %d rocks to pay!", u.practicantstones);
				if (u.practicantarrows) You("also still have %d arrows to pay!", u.practicantarrows);
			}
			if (u.practicanttime == 10) {
				You("only have 10 turns left to pay %d zorkmids to %s!!! Pay up now, or at least make sure that you have the required amount of zorkmids out in the open!", u.practicantpenalty, noroelaname());
				if (u.practicantstones) You("also still have %d rocks to pay!", u.practicantstones);
				if (u.practicantarrows) You("also still have %d arrows to pay!", u.practicantarrows);
			}

			if (u.practicanttime < 1) { /* payday! */
				practicant_payup(); /* if you e.g. have the menu bug and therefore can't pay manually... */

				if (u.practicantpenalty || u.practicantstones || u.practicantarrows) { /* still have a penalty - now suffer, maggot! */
					pline("%s booms: 'You didn't pay your fine of %d zorkmids in time! Now, you shall be punished.'", noroelaname(), u.practicantpenalty);
					if (u.practicantstones) verbalize("You also didn't pay the %d stones I requested!", u.practicantstones);
					if (u.practicantarrows) verbalize("You also didn't pay the %d arrows I requested!", u.practicantarrows);
					badeffect();
					u.practicantseverity++;
					/* the longer you refuse to pay, the harsher the extra penalties become */
					if (u.practicantseverity < 2) {
						u.practicanttime = 500;
						pline("%s rings out: 'I give you 500 more turns to pay up!'", noroelaname());
					} else if (u.practicantseverity < 5) {
						u.practicanttime = 200;
						pline("%s booms: 'Pay up in 200 turns, or else!'", noroelaname());
					} else if (u.practicantseverity < 10) {
						u.practicanttime = 100;
						u.practicantpenalty += 10;
						pline("%s thunders: 'Since you keep not paying your fine, it is now increased by 10 zorkmids and you only have 100 more turns to finally pay up, maggot!'", noroelaname());
					} else if (u.practicantseverity < 20) {
						u.practicanttime = 50;
						u.practicantpenalty += 20;
						pline("%s thunders: 'You don't get it, huh? Your fine increases by 20 zorkmids now and if you don't pay in 50 turns I'll increase it even more!'", noroelaname());
					} else {
						u.practicanttime = 20;
						u.practicantpenalty += 20;
						pline("%s thunders: 'Hereby I increase your fine by another 20 zorkmids. If you don't pay in 20 turns I'll increase it again. I warn you, maggot, don't test my patience even more.'", noroelaname());
					}





				}
			}
		}

		if ((LongingEffect || u.uprops[LONGING_EFFECT].extrinsic || have_longingstone()) && !rn2(50)) {
			longingtrapeffect();
		}

		if (RngeLoudspeakers && !rn2(100)) {
			pline("%s", fauxmessage());
			u.cnd_plineamount++;
			if (!rn2(3)) {
				pline("%s", fauxmessage());
				u.cnd_plineamount++;
			}
		}

		if (Race_if(PM_SPAMMER) && !rn2(100)) {
			pline("%s", fauxmessage());
			u.cnd_plineamount++;
			if (!rn2(3)) {
				pline("%s", fauxmessage());
				u.cnd_plineamount++;
			}
		}

		for(ttmp = ftrap; ttmp; ttmp = ttmp->ntrap) { /* this function is probably expensive... --Amy */
			if (ttmp && ttmp->ttyp == LOUDSPEAKER && !rn2(100) ) {
				pline("%s", fauxmessage());
				u.cnd_plineamount++;
				if (!rn2(3)) {
					pline("%s", fauxmessage());
					u.cnd_plineamount++;
				}
			}
			if (ttmp && ttmp->ttyp == ARABELLA_SPEAKER && !rn2(50) ) {
				pline("%s", fauxmessage());
				u.cnd_plineamount++;
				if (!rn2(3)) {
					pline("%s", fauxmessage());
					u.cnd_plineamount++;
				}
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

				if (uarmh && itemhasappearance(uarmh, APP_FILTERED_HELMET) && !rn2(2) ) {
					 pline("A cloud of spores surrounds you!");
				} else if (RngeGasFiltering && !rn2(2)) {
					 pline("A cloud of spores surrounds you!");

				 } else if (!Strangled && !Breathless) {
					 You("inhale a cloud of spores!");
					 poisoned("spores", A_STR, "fumarole spores", 30);
			       } else {
					 pline("A cloud of spores surrounds you!");
					 if (!rn2(StrongMagical_breathing ? 5 : 2)) poisoned("spores", A_STR, "fumarole spores", 30);
			       }

			}

			if (ttmp && ttmp->ttyp == VENTILATOR && (distu(ttmp->tx, ttmp->ty) < 4 ) ) {
				if (!rn2(3)) pline("Air currents blow in your %s!", body_part(FACE));
				pushplayer(TRUE);
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
				makerandomtrap(FALSE);
			}

			if (ttmp && ttmp->ttyp == ADJACENT_TRAP && !(t_at(u.ux, u.uy)) && (distu(ttmp->tx, ttmp->ty) < 4 ) ) {
				maketrap(u.ux, u.uy, randomtrap(), 100, FALSE);
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

		/* put things that don't have anything to do with traps below the trapsdone mark --Amy */
trapsdone:

		if (quest_status.touched_artifact) { /* using the artifact a lot improves alignment --Amy */

			if (usingquestarti()) {
				u.artifactaffinity++;
				if (u.artifactaffinity > (u.artifinitythreshold + 1000)) {
					u.artifinitythreshold += 1000;
					if (u.artifinitythreshold >= 10000) u.alignlim += 1;
					adjalign(rnd(10));
				}
			} else {
				u.artifactaffinity -= 10;
			}
		}

		if (uwep && uwep->otyp == YITH_TENTACLE && !rn2(200)) increasesanity(1);
		if (u.twoweap && uswapwep && uswapwep->otyp == YITH_TENTACLE && !rn2(200)) increasesanity(1);

		/* using other roles' quest artifacts (e.g. by wishing for them) gives downsides --Amy */
		if (!rn2(100) && foreignartifactcount() > 0) {
			contaminate(foreignartifactcount(), FALSE);
			u.usanity += (YouGetLotsOfSanity ? (foreignartifactcount() * rnd(20)) : foreignartifactcount());
			adjalign(-(foreignartifactcount()));
		}
		/* I don't want to ban wishing for quest artifacts like some variants do, because that's too arbitrary.
		 * But some of them are really very powerful, and you shouldn't be able to easily become OP if you get a wish */

		if (have_faintingstone() && !rn2(100) && multi >= 0) {

			You("faint from exertion.");
			flags.soundok = 0;
			nomul(-(rnz(5) ), "fainted from exertion", TRUE);
			nomovemsg = "You regain consciousness.";
			afternmv = unfaintX;

		}

		if (uarmf && uarmf->oartifact == ART_ENDORPHIC_SCRATCHING && !rn2(4000)) {
			pline("Your pumps challenge you to a fight!");
			pumpsminigame();
			if (practicantterror) {
				pline("%s rings out: 'Wanking off is not permitted in my laboratory, but you know that. 200 zorkmids.'", noroelaname());
				fineforpracticant(200, 0, 0);
			}
		}

		if (uarmh && uarmh->oartifact == ART_VACUUM_CLEANER_DEATH && !rn2(25000)) {
			register struct monst *blonde;
			blonde = makemon(&mons[PM_SWEET_BLONDE], u.ux, u.uy, 0);
			if (blonde) {
				tamedog(blonde, (struct obj *) 0, FALSE);
				pline("Suddenly, a sweet blonde appears!");
			}
		}

		if (uarmc && uarmc->oartifact == ART_ARABELLA_S_WEAPON_STORAGE && !rn2(1000)) {
			acqo = mkobj_at(WEAPON_CLASS, u.ux, u.uy, FALSE, FALSE);
			if (acqo) pline("Someting appeared on the ground beneath you!");
		}

		if (uarmc && uarmc->oartifact == ART_ARABELLA_S_WEAPON_STORAGE && !rn2(10000)) {
			bad_artifact();
		}

		if (uimplant && uimplant->oartifact == ART_FULLGREASE && !rn2(5000)) {

			pline("Some of your items were greased!");

			register struct obj *grsobj, *grsXXX;
			for(grsobj = invent; grsobj ; grsobj = grsobj->nobj) {
				if (!rn2(10) && grsobj && !stack_too_big(grsobj) && grsobj->greased < 3) grsobj->greased++;
			}

		}

		if (!rn2(5000)) {
			int nanorepaired = 0;
			register struct obj *grsobj, *grsXXX;
			for(grsobj = invent; grsobj ; grsobj = grsobj->nobj) {
				if (grsobj && objects[grsobj->otyp].oc_material == MT_NANOMACHINE && !stack_too_big(grsobj) && grsobj->oeroded) {
					grsobj->oeroded--;
					nanorepaired++;
				}
				if (grsobj && objects[grsobj->otyp].oc_material == MT_NANOMACHINE && !stack_too_big(grsobj) && grsobj->oeroded2) {
					grsobj->oeroded2--;
					nanorepaired++;
				}
			}
			if (nanorepaired) pline("Your nanomachines have repaired some of the damage they sustained!");
		}

		if (uarmc && uarmc->oartifact == ART_ARABELLA_S_WEAPON_STORAGE && !rn2(10000)) {
			register struct obj *crsobj, *crsXXX;
			for(crsobj = invent; crsobj ; crsobj = crsobj->nobj) {
				if (crsobj && Has_contents(crsobj)) {
					for (crsXXX = crsobj->cobj; crsXXX; crsXXX = crsXXX->nobj) {
						curse(crsXXX);
						curse(crsXXX);
						curse(crsXXX);
						if (!rn2(20)) crsXXX->stckcurse = TRUE;
					}
				}
			}

			for(crsobj = invent; crsobj ; crsobj = crsobj->nobj) {
				if (crsobj && !stack_too_big(crsobj)) {
					curse(crsobj);
					curse(crsobj);
					curse(crsobj);
					if (!rn2(20)) crsobj->stckcurse = TRUE;
				}
			}

		}

		if (uamul && uamul->oartifact == ART_APATHY_STRATEGY && !rn2(100) && multi >= 0) {

			You("faint from exertion.");
			flags.soundok = 0;
			nomul(-(rnz(5) ), "fainted from exertion", TRUE);
			nomovemsg = "You regain consciousness.";
			afternmv = unfaintX;

		}

		if (uarmf && uarmf->oartifact == ART_CRUEL_GODDESS_ANA && !rn2(250) && !Vomiting) {
			You_feel("like you're going to throw up.");
		      make_vomiting(Vomiting+20, TRUE);
			if (Sick && Sick < 100) set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
		}

		if (!rn2(200) && uarmf && uarmf->oartifact == ART_WUMSHIN) {
			pline("Wumm! Your lovely boots kick you in the shins.");
			losehp(rnd(u.ulevel), "being kicked by the bum bum boots", KILLED_BY);
			make_stunned(HStun + rnd(10), TRUE);
		}

		if (uarmf && uarmf->oartifact == ART_SMEXY_BERRIES) u.smexyberries++;

		if (uarmg && uarmg->oartifact == ART_VOLCANO_BOOM && !rn2(2000)) {
			pline("Kaboom!");
			explode(u.ux, u.uy, ZT_FIRE, rnd(u.ulevel * 5), WAND_CLASS, EXPL_FIERY);
		}

		if (uarmh && uarmh->oartifact == ART_BAD_LUCK_IN_DROVES && Luck > 0) change_luck(-1);

		/* the manler chases after the player; he often moves randomly but not always */
		if (ManlerIsChasing && (u.manlerx >= 0 && u.manlery >= 0) ) {
			if (u.manlerx == u.ux && u.manlery == u.uy) {
				pline("Daedeldidaet! The manler caught you...");
				u.cnd_manlergetcount++;
				badeffect();
				badeffect();
				badeffect();
				badeffect();
				badeffect();
				losehp(rnd(u.ulevel * 5), "being caught by the manler", KILLED_BY);

				if (u.ux < 39) u.manlerx = 69;
				else u.manlerx = 9;

				if (u.uy < 9) u.manlery = 14;
				else u.manlery = 4;

			}

			int manleroldx = u.manlerx, manleroldy = u.manlery;

			if (!rn2(5)) {
				if (u.ux < u.manlerx && u.ux >= 1) u.manlerx--;
				else if (u.ux > u.manlerx && u.ux <= COLNO-1) u.manlerx++;

				if (u.uy < u.manlery && u.uy >= 0) u.manlery--;
				else if (u.uy > u.manlery && u.uy <= ROWNO-1) u.manlery++;

			} else switch (rnd(8)) {
				case 1:
					if (u.manlerx > 1) u.manlerx--;
					break;
				case 2:
					if (u.manlerx < COLNO-1) u.manlerx++;
					break;
				case 3:
					if (u.manlery > 0) u.manlery--;
					break;
				case 4:
					if (u.manlery < ROWNO-1) u.manlery++;
					break;
				case 5:
					if (u.manlerx > 1) u.manlerx--;
					if (u.manlery > 0) u.manlery--;
					break;
				case 6:
					if (u.manlerx < COLNO-1) u.manlerx++;
					if (u.manlery > 0) u.manlery--;
					break;
				case 7:
					if (u.manlerx > 1) u.manlerx--;
					if (u.manlery < ROWNO-1) u.manlery++;
					break;
				case 8:
					if (u.manlerx < COLNO-1) u.manlerx++;
					if (u.manlery < ROWNO-1) u.manlery++;
					break;
			}
			if (isok(u.manlerx, u.manlery)) newsym(u.manlerx, u.manlery);
			if (isok(manleroldx, manleroldy)) newsym(manleroldx, manleroldy);

			if (u.manlerx == u.ux && u.manlery == u.uy) {
				pline("Daedeldidaet! The manler caught you...");
				u.cnd_manlergetcount++;
				badeffect();
				badeffect();
				badeffect();
				badeffect();
				badeffect();
				losehp(rnd(u.ulevel * 5), "being caught by the manler", KILLED_BY);

				if (u.ux < 39) u.manlerx = 69;
				else u.manlerx = 9;

				if (u.uy < 9) u.manlery = 14;
				else u.manlery = 4;

			}

		}

		if ((ChangingDirectives || u.uprops[CHANGING_DIRECTIVES].extrinsic || have_changingdirectivestone()) && !rn2(100)) {
			switch (rnd(5)) {

				case 1:
					if (u.petcollectitems) {
						u.petcollectitems = 0;
					} else {
						u.petcollectitems = 1;
					}
					break;
				case 2:
					u.petattackenemies = rn2(2) ? 0 : rn2(2) ? 1 : 2;
					break;
				case 3:
					if (u.petcaneat) {
						u.petcaneat = 0;
					} else {
						u.petcaneat = 1;
					}
					break;
				case 4:
					if (u.petcanfollow) {
						u.petcanfollow = 0;
					} else {
						u.petcanfollow = 1;
					}
					break;
				case 5:
					u.steedhitchance = rn2(101);
					break;

			}

		}

		if (u.tarmustrokingturn) {
			u.tarmustrokingturn--;
			if (u.tarmustrokingturn < 0) u.tarmustrokingturn = 0; /* fail safe */
		}

		if (SkillLossEffect || u.uprops[SKILL_LOSS_EFFECT].extrinsic || have_skilllossstone()) {
			skillcaploss_severe();
		}

		if ((DangerousTerrains || u.uprops[DANGEROUS_TERRAINS].extrinsic || have_dangerousterrainstone()) && !(HStun && HConfusion)) {
			int terraindanger = 0;
			int i, j;

			for (i = -1; i <= 1; i++) for(j = -1; j <= 1; j++) {
				if (!isok(u.ux + i, u.uy + j)) continue;
				if (levl[u.ux + i][u.uy + j].typ == POOL || levl[u.ux + i][u.uy + j].typ == MOAT || levl[u.ux + i][u.uy + j].typ == WATER || levl[u.ux + i][u.uy + j].typ == WATERTUNNEL || levl[u.ux + i][u.uy + j].typ == CRYSTALWATER || levl[u.ux + i][u.uy + j].typ == MOORLAND || levl[u.ux + i][u.uy + j].typ == SHIFTINGSAND || levl[u.ux + i][u.uy + j].typ ==  LAVAPOOL || levl[u.ux + i][u.uy + j].typ == STYXRIVER || levl[u.ux + i][u.uy + j].typ == NETHERMIST || levl[u.ux + i][u.uy + j].typ == STALACTITE || levl[u.ux + i][u.uy + j].typ == RAINCLOUD) terraindanger++;
			}

			if (terraindanger > rn2(10)) {
				if (!HStun) make_stunned(5, FALSE);
				if (!HConfusion) make_confused(5, FALSE);
			}
			/* It is not a bug that these aren't giving a message; this is a nasty trap. --Amy */

		}

		if (RecurringSpellLoss && !rn2(1000)) {
			spellmemoryloss((level_difficulty() * rnd(3)) + 1);
		}

		if (u.uprops[RECURRING_SPELL_LOSS].extrinsic && !rn2(1000)) {
			spellmemoryloss((level_difficulty() * rnd(3)) + 1);
		}

		if (have_recurringspelllossstone() && !rn2(1000)) {
			spellmemoryloss((level_difficulty() * rnd(3)) + 1);
		}

		if (TechoutBug && !rn2(1000)) {
			techcapincrease((level_difficulty() + 1) * rnd(150));
		}

		if (u.uprops[TECHOUT_BUG].extrinsic && !rn2(1000)) {
			techcapincrease((level_difficulty() + 1) * rnd(150));
		}

		if (have_techoutstone() && !rn2(1000)) {
			techcapincrease((level_difficulty() + 1) * rnd(150));
		}

		if (StatDecay && !rn2(1000)) {
			statdrain();
		}

		if (u.uprops[STAT_DECAY].extrinsic && !rn2(1000)) {
			statdrain();
		}

		if (have_statdecaystone() && !rn2(1000)) {
			statdrain();
		}

		if (AntitrainingEffect && !rn2(1000)) {
			skilltrainingdecrease((level_difficulty() * rnd(3)) + 1);
		}

		if (u.uprops[ANTI_TRAINING_EFFECT].extrinsic && !rn2(1000)) {
			skilltrainingdecrease((level_difficulty() * rnd(3)) + 1);
		}

		if (have_antitrainingstone() && !rn2(1000)) {
			skilltrainingdecrease((level_difficulty() * rnd(3)) + 1);
		}

		if (FalloutEffect && !rn2(100)) {
			contaminate(rnd(10), FALSE);
		}

		if (uwep && uwep->oartifact == ART_ARABELLA_S_BLACK_PRONG && !rn2(100)) {
			contaminate(rnd(10), FALSE);
		}

		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_ARABELLA_S_BLACK_PRONG && !rn2(100)) {
			contaminate(rnd(10), FALSE);
		}

		if (u.uprops[FALLOUT_EFFECT].extrinsic && !rn2(100)) {
			contaminate(rnd(10), FALSE);
		}

		if (DoorningEffect || u.uprops[DOORNING_EFFECT].extrinsic || have_doorningstone()) {
			if (IS_DOOR(levl[u.ux][u.uy].typ) && !rn2(10) && u.umoved && !(t_at(u.ux, u.uy)) ) {

				int i, j;

				for (i = -1; i <= 1; i++) for(j = -1; j <= 1; j++) {
					if (!isok(u.ux + i, u.uy + j)) continue;
					if (levl[u.ux + i][u.uy + j].typ <= DBWALL) continue;
					if (t_at(u.ux + i, u.uy + j)) continue;

					ttmp = maketrap(u.ux + i, u.uy + j, randomtrap(), 100, FALSE);
					if (ttmp) {
						ttmp->tseen = 0;
						ttmp->hiddentrap = 1;
					}
				}

				int tryct = 0;
				int x, y;

				for (tryct = 0; tryct < 2000; tryct++) {
					x = rn1(COLNO-3,2);
					y = rn2(ROWNO);

					if (x && y && isok(x, y) && (levl[x][y].typ > DBWALL) && !(t_at(x, y)) ) {
							ttmp = maketrap(x, y, randomtrap(), 100, FALSE);
						if (ttmp) {
							ttmp->tseen = 0;
							ttmp->hiddentrap = 1;
						}
						if (rn2(3)) break;
					}
				}

				if (!rn2(10)) b_trapped("door", 0);

			}
		}

		if (have_falloutstone() && !rn2(100)) {
			contaminate(rnd(10), FALSE);
		}

		if (!rn2(2500) && uarmg && itemhasappearance(uarmg, APP_DEMOLITION_GLOVES) ) {
			struct obj *dynamite;
			dynamite = mksobj(STICK_OF_DYNAMITE, TRUE, FALSE, FALSE);
			if (dynamite) {
				if (dynamite->otyp != STICK_OF_DYNAMITE) delobj(dynamite);
				else {
					dynamite->quan = 1;
					dynamite->owt = weight(dynamite);
					dropy(dynamite);
					attach_bomb_blow_timeout(dynamite, 0, 0);
				}
			}

		}

		if (!rn2(2500) && uarmg && itemhasappearance(uarmg, APP_BISE_GLOVES) ) {
			pline("Suddenly, your gauntlets kiss you!");

			if ((rnd(ABASE(A_CHA)) < 11) && ABASE(A_CHA) < 18) {
				(void) adjattrib(A_CHA, 1, -1, TRUE);
			}

			nomul(-5, "being kissed by the bise gauntlets", FALSE);
		}

		if (Numbed && !rn2(isfriday ? 50 : 100) && multi >= 0) {

			pline("You're fully paralyzed!");
			flags.soundok = 0;
			nomul(-2, "fully paralyzed", FALSE);
			nomovemsg = 0;
			afternmv = unfaintX;

		}

		if (uarmh && itemhasappearance(uarmh, APP_BREATH_CONTROL_HELMET) ) {
			if (!rn2(1000) && !Breathless) {
				pline("You're gasping for air!");
				losehp(rnz(u.ulevel + 5), "breath control fetishism", KILLED_BY);
				stop_occupation();

				if (!rn2(3)) {
					switch (rn2(5)) {
						case 0: You_feel("raised to your full potential.");
							exercise(A_CON, TRUE);
							u.uen = (u.uenmax += rnd(5));
							break;
						case 1: You_feel("good enough to do it again.");
							(void) adjattrib(A_CON, 1, TRUE, TRUE);
							exercise(A_CON, TRUE);
							flags.botl = 1;
							break;
						case 2: You("will always remember the feeling of near suffocation...");
							(void) adjattrib(A_WIS, 1, TRUE, TRUE);
							exercise(A_WIS, TRUE);
							flags.botl = 1;
							break;
						case 3: pline("That was a very educational experience.");
							gainlevelmaybe();
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
			}
		}

		if (!rn2(10000) && uarmc && itemhasappearance(uarmc, APP_CURSED_CALLED_CLOAK)) {

			register struct obj *cclld;

			for(cclld = invent; cclld; cclld = cclld->nobj) {
				if (!rn2(20) && !stack_too_big(cclld))
					curse(cclld);
			}

		}

		/* in evilvariant mode you always have at least 100 sanity --Amy */
		if (isevilvariant && u.usanity < 100) u.usanity += 100;

		if (!rn2(isfriday ? 2000 : 5000)) {

			int spellammount = 0;
			for (spellammount = 0; spellammount < MAXSPELL && spellid(spellammount) != NO_SPELL; spellammount++)
				continue;
			if (spellammount > (urole.spelrete * 2)) {

				/* note by Amy: it is not an error that losespells() is called regardless; keen memory and
				 * the mastermind ability are handled directly in losespells() and not here */

				You("have too many spells memorized!");
				if (Keen_memory) Your("mind struggles to keep all the spells in memory.");
				else if (Role_if(PM_MASTERMIND) && mastermindsave()) You("focus on your memorized spells and try to prevent them from being erased.");
				else Your("mind can't keep so many spells in memory, and as a result, some of them are erased.");

				losespells();

			}

		}

		if (Role_if(PM_HUSSY)) {

			if (u.hussyhurtturn < 1) u.hussyhurtturn = (moves + 1000);
			if (u.hussykillturn < 1) u.hussykillturn = (moves + 1000);

			if (moves > u.hussyhurtturn) {
				pline("You neglected your hussy obligations by failing to hit a guy in 1000 turns, and are therefore being punished!");
				badeffect();
				if (!rn2(2)) badeffect();
				if (!rn2(3)) badeffect();
				u.hussyhurtturn = (moves + 1000);
			}

			if (moves > u.hussykillturn) {
				pline("You neglected your hussy obligations by failing to defeat a guy in 1000 turns, and are therefore being punished!");
				badeffect();
				if (!rn2(2)) badeffect();
				if (!rn2(3)) badeffect();
				u.hussykillturn = (moves + 1000);
			}

			if (u.hussyhurtturn == (moves + 500)) {
				pline("Reminder: As a hussy, you must hit a guy once every 1000 turns or be punished for neglecting your obligations! You have 500 turns left!");
			}

			if (u.hussykillturn == (moves + 500)) {
				pline("Reminder: As a hussy, you must defeat a guy once every 1000 turns or be punished for neglecting your obligations! You have 500 turns left!");
			}

			if (u.hussyhurtturn == (moves + 100)) {
				pline("Hey, you hussy! Listen! You didn't hit a guy in 900 turns now, and you only have 100 turns left to fulfill your obligations or else you face a penalty!");
			}

			if (u.hussykillturn == (moves + 100)) {
				pline("Hey, you hussy! Listen! You didn't defeat a guy in 900 turns now, and you only have 100 turns left to fulfill your obligations or else you face a penalty!");
			}

		}

		if (uarmc && uarmc->oartifact == ART_INA_S_SORROW && IS_TOILET(levl[u.ux][u.uy].typ) && u.uhs < HUNGRY ) {
			pline("For some reason, you have to take a shit right now.");

			if (Sick && !rn2(3) ) make_sick(0L, (char *)0, TRUE, SICK_VOMITABLE);
			else if (Sick && !rn2(10) ) make_sick(0L, (char *)0, TRUE, SICK_ALL);
			morehungry(rn2(1200)+600);

			if (uarmu && uarmu->oartifact == ART_KATIA_S_SOFT_COTTON) {
				You("produce very erotic noises.");
				if (!rn2(10)) adjattrib(rn2(A_CHA), 1, -1, TRUE);
			}

			pline("But you were such a bitch and crapped on the lid!");
			use_skill(P_SQUEAKING, 10);
			adjalign(-20);
			u.ualign.sins++;
			u.alignlim--;

		}

		if (FemtrapActiveThai && IS_TOILET(levl[u.ux][u.uy].typ) && u.uhs < HUNGRY ) {
			pline("For some reason, you have to take a shit right now.");

			if (Sick && !rn2(3) ) make_sick(0L, (char *)0, TRUE, SICK_VOMITABLE);
			else if (Sick && !rn2(10) ) make_sick(0L, (char *)0, TRUE, SICK_ALL);
			morehungry(rn2(400)+200);

			if (uarmu && uarmu->oartifact == ART_KATIA_S_SOFT_COTTON) {
				You("produce very erotic noises.");
				if (!rn2(10)) adjattrib(rn2(A_CHA), 1, -1, TRUE);
			}

			use_skill(P_SQUEAKING, 10);
			pline("But in your haste, you forgot to open the lid!");
			adjalign(-20);

		}

		if (FemtrapActiveYvonne && (multi >= 0) && IS_TOILET(levl[u.ux][u.uy].typ) && u.uhs < HUNGRY ) {
			pline("A toilet! You feel that you have to take a shit, and so you do.");

			use_skill(P_SQUEAKING, 10);
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
				if (!rn2(10)) adjattrib(rn2(A_CHA), 1, -1, TRUE);
			}

			nomovemsg = "You are done shitting.";
			nomul(-crapduration, "taking a shit", TRUE);

		}

		if (FemtrapActiveMaurah && !rn2(100)) {

			pline("Suddenly, you produce beautiful farting noises with your sexy butt.");
			u.cnd_fartingcount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (!extralongsqueak()) badeffect();
			stop_occupation();

		}

		if (uarmh && uarmh->oartifact == ART_CLAUDIA_S_SEXY_SCENT && !rn2(100)) {

			pline("Suddenly, you produce beautiful farting noises with your sexy butt.");
			u.cnd_fartingcount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (!extralongsqueak()) badeffect();
			stop_occupation();

		}

		if (FemtrapActiveSarah && !rn2(250)) {

			int x, y;
			x = rn1(COLNO-3,2);
			y = rn2(ROWNO);

			if (isok(x,y)) (void) create_gas_cloud(x, y, 4, 10);
			You_hear("tender air current noises.");
			/* These will anger monsters (not a bug). --Amy */

		}

		if (FemtrapActiveClaudia && !rn2(100)) {

			int x, y;
			x = rn1(COLNO-3,2);
			y = rn2(ROWNO);

			if (isok(x,y)) {
				if ( (levl[x][y].typ == ROOM || levl[x][y].typ == CORR || (IS_STWALL(levl[x][y].typ) && !rn2(10) && ((levl[x][y].wall_info & W_NONDIGGABLE) == 0)) ) && !(*in_rooms(x,y,SHOPBASE)) ) {
					levl[x][y].typ = CLOUD;
					blockorunblock_point(x,y);
					if (!(levl[x][y].wall_info & W_HARDGROWTH)) levl[x][y].wall_info |= W_EASYGROWTH;
					newsym(x, y);

					if (!rn2(10)) {
						if (!rn2(3)) (void) makemon(&mons[PM_EROTICLOUDY], x, y, MM_ANGRY|MM_ADJACENTOK);
						else if (!rn2(2)) (void) makemon(&mons[PM_WOODICLOUDY], x, y, MM_ANGRY|MM_ADJACENTOK);
						else (void) makemon(&mons[PM_SCENTICLOUDY], x, y, MM_ANGRY|MM_ADJACENTOK);
					}

				}
			}

		}

		if (FemtrapActiveClaudia && !rn2(250)) {
			make_confused(HConfusion + rnd(20), FALSE);
			set_itimeout(&HeavyConfusion, HConfusion);
			pline("Whoops, you're getting really dizzy.");
			if (FunnyHallu) pline("Are you a blonde by any chance?");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

		}

		if (FemtrapActiveElif && !rn2(100)) {

			switch (rnd(4)) {

				case 1:
					pline("Elif suddenly kicks your %s with her very soft, female sneakers, and draws %s!", body_part(HAND), body_part(BLOOD));
					incr_itimeout(&Glib, 2); /* just enough to make you drop your weapon */
					losehp(rnd(u.ulevel), "Elif's soft female sneakers", KILLED_BY);
					break;
				case 2:
					pline("Elif suddenly produces %s farting noises with her sexy butt.", rn2(2) ? "tender" : "soft");
					u.cnd_fartingcount++;
					if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
					if (uarmf && uarmf->oartifact == ART_SARAH_S_GRANNY_WEAR) healup((level_difficulty() + 5), 0, FALSE, FALSE);
					else if (!extralongsqueak()) badeffect();
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

		if (u.swappositioncount) {

			u.swappositioncount--;
			if (u.swappositioncount < 0) u.swappositioncount = 0;
			if (u.swappositioncount == 0) pline("Too late, you can no longer displace monsters.");

		}

		if (FemtrapActiveNatalje) {
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

				stop_occupation();

				if ((!SustainAbilityOn || !SustainLossSafe) && ABASE(A_CHA) > 3) (void) adjattrib(A_CHA, -1, TRUE, TRUE);
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
				if (!strncmpi(plname, "Natalje", 8) || !strncmpi(plalias, "Natalje", 8)) {
					pline("Keep dancing, Natalje...");
				} else {
					You("gotta keep dancing...");
				}
				stop_occupation();
			}

			if (moves > (u.nataljetrapturns + 5)) {
				if (!strncmpi(plname, "Natalje", 8) || !strncmpi(plalias, "Natalje", 8)) {
					pline("Careful, Natalje! You gotta dance or you'll get hurt!");
				} else {
					You("missed the beat! Continue dancing or suffer!");
				}
				stop_occupation();
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
			steeling = getobj(allowall, "rustproof");
			if (!steeling) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to rustproof an item.");
				else goto steelingchoice;
				pline("Oh well, if you don't wanna...");
			} else {
				if (!(objects[(steeling)->otyp].oc_material == MT_IRON) ) {
					pline("That is not made of iron!");
				} else if (!stack_too_big(steeling)) {
					steeling->oerodeproof = 1;
					p_glow2(steeling, NH_PURPLE);
					if (steeling && objects[(steeling)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(steeling)) {
						if (!steeling->cursed) bless(steeling);
						else uncurse(steeling, FALSE);
					}
				} else pline("The stack was too big and therefore nothing happens...");
			}
		}

		/* Amy grepping target: "materialeffect" */
		if (uarmc && uarmc->oartifact == ART_PROTECT_WHAT_CANNOT_BE_PRO && !rn2(5000) ) {
			register struct obj *steeling;
			pline("You may erodeproof a nonerodable object.");
protectwhatchoice:
			steeling = getobj(allowall, "erosionproof");
			if (!steeling) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to erosionproof an item.");
				else goto protectwhatchoice;
				pline("Oh well, if you don't wanna...");
			} else {
				if (objects[(steeling)->otyp].oc_material >= MT_WAX && objects[(steeling)->otyp].oc_material <= MT_WOOD) 
					pline("That is erodable, and therefore it doesn't work!");
				else if (objects[(steeling)->otyp].oc_material == MT_IRON)
					pline("That is erodable, and therefore it doesn't work!");
				else if (objects[(steeling)->otyp].oc_material == MT_COPPER)
					pline("That is erodable, and therefore it doesn't work!");
				else if (objects[(steeling)->otyp].oc_material == MT_PLASTIC)
					pline("That is erodable, and therefore it doesn't work!");
				else if (objects[(steeling)->otyp].oc_material >= MT_VIVA && objects[(steeling)->otyp].oc_material <= MT_SAND) 
					pline("That is erodable, and therefore it doesn't work!");
				else if (objects[(steeling)->otyp].oc_material >= MT_CHROME && objects[(steeling)->otyp].oc_material <= MT_ALLOY) 
					pline("That is erodable, and therefore it doesn't work!");
				else if (!stack_too_big(steeling)) {
					steeling->oerodeproof = 1;
					p_glow2(steeling, NH_PURPLE);
					if (steeling && objects[(steeling)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(steeling)) {
						if (!steeling->cursed) bless(steeling);
						else uncurse(steeling, FALSE);
					}
				} else pline("The stack was too big and therefore nothing happens...");
			}
		}

		if (uimplant && uimplant->oartifact == ART_FUKROSION && !rn2(2500) ) {
			register struct obj *steeling;
			pline("You may repair an eroded object. If you're in a form without hands, the object you pick will also become erosionproof.");
fukrosionchoice:
			steeling = getobj(allowall, "repair");
			if (!steeling) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to repair/erosionproof an item.");
				else goto fukrosionchoice;
				pline("Oh well, if you don't wanna...");
			} else if (!stack_too_big(steeling)) {
				steeling->oeroded = steeling->oeroded2 = 0;
				if (powerfulimplants()) steeling->oerodeproof = 1;
				p_glow2(steeling, NH_PURPLE);
				if (steeling && objects[(steeling)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(steeling)) {
					if (!steeling->cursed) bless(steeling);
					else uncurse(steeling, FALSE);
				}
			} else pline("The stack was too big and therefore nothing happens...");

		}

		if (powerfulimplants() && uimplant && uimplant->oartifact == ART_YES_YOU_CAN && !rn2(500)) {
			if (spellid(0) != NO_SPELL)  {

				getlin ("You may use inertia control to choose a spell of your choice, which will automatically be cast every turn. Do it? [y/yes/no]",buf);
				(void) lcase (buf);
				if (!(strcmp (buf, "yes")) || !(strcmp (buf, "y"))) {

controlagain:
					if (!inertiacontrolspell()) { /* see spell.c */
						if (yn("Really exit with no spell selected?") == 'y')
							pline("You just wasted the opportunity to control a spell.");
						else goto controlagain;
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

		if (u.stoogedepth && u.stoogedepth == depth(&u.uz)) {
			u.stoogedepth = 0;
			if (mvitals[PM_STOOGE_CURLY].born == 0) makemon(&mons[PM_STOOGE_CURLY], 0, 0, NO_MM_FLAGS); /* makemon.c will spawn the other two */
		}

		if (u.footererlevel && u.footererlevel == depth(&u.uz)) {

			pline("The footerers were waiting for you here, and will attack now.");

			u.aggravation = 1;
			u.heavyaggravation = 1;
			DifficultyIncreased += 1;
			HighlevelStatus += 1;

			randsp = (rn2(50) + 11);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			monstercolor = rnd(376);

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(monstercolor), 0, 0, MM_ANGRY|MM_FRENZIED);
			}

			u.footererlevel = 0;
			u.aggravation = 0;
			u.heavyaggravation = 0;
			if (DifficultyIncreased > 0) DifficultyIncreased -= 1;
			if (HighlevelStatus > 0) HighlevelStatus -= 1;

		}

		if (Prem_death && !rn2(isfriday ? 5000 : 10000)) { /* evil patch idea by jonadab */

			u.youaredead = 1;
			You("suddenly die.");
			killer_format = KILLED_BY;
			killer = "premature death";
			done(DIED);
			u.youaredead = 0;

		}

		if (iswindinhabitant && !rn2(1000)) {

			u.nastinator01 = u.nastinator02 = u.nastinator03 = u.nastinator04 = u.nastinator05 = u.nastinator06 = u.nastinator07 = u.nastinator08 = u.nastinator09 = u.nastinator10 = u.nastinator11 = u.nastinator12 = u.nastinator13 = u.nastinator14 = u.nastinator15 = u.nastinator16 = u.nastinator17 = u.nastinator18 = u.nastinator19 = u.nastinator20 = u.nastinator21 = u.nastinator22 = u.nastinator23 = u.nastinator24 = u.nastinator25 = u.nastinator26 = u.nastinator27 = u.nastinator28 = u.nastinator29 = u.nastinator30 = u.nastinator31 = u.nastinator32 = u.nastinator33 = u.nastinator34 = u.nastinator35 = u.nastinator36 = u.nastinator37 = u.nastinator38 = u.nastinator39 = u.nastinator40 = u.nastinator41 = u.nastinator42 = u.nastinator43 = u.nastinator44 = u.nastinator45 = u.nastinator46 = u.nastinator47 = u.nastinator48 = u.nastinator49 = u.nastinator50 = u.nastinator51 = u.nastinator52 = u.nastinator53 = u.nastinator54 = u.nastinator55 = u.nastinator56 = u.nastinator57 = u.nastinator58 = u.nastinator59 = u.nastinator60 = u.nastinator61 = u.nastinator62 = u.nastinator63 = u.nastinator64 = u.nastinator65 = u.nastinator66 = u.nastinator67 = u.nastinator68 = u.nastinator69 = u.nastinator70 = u.nastinator71 = u.nastinator72 = u.nastinator73 = u.nastinator74 = u.nastinator75 = u.nastinator76 = u.nastinator77 = u.nastinator78 = u.nastinator79 = u.nastinator80 = u.nastinator81 = u.nastinator82 = u.nastinator83 = u.nastinator84 = u.nastinator85 = u.nastinator86 = u.nastinator87 = u.nastinator88 = u.nastinator89 = u.nastinator90 = u.nastinator91 = u.nastinator92 = u.nastinator93 = u.nastinator94 = u.nastinator95 = u.nastinator96 = u.nastinator97 = u.nastinator98 = u.nastinator99 = u.nastinator100 = u.nastinator101 = u.nastinator102 = u.nastinator103 = u.nastinator104 = u.nastinator105 = u.nastinator106 = u.nastinator107 = u.nastinator108 = u.nastinator109 = u.nastinator110 = u.nastinator111 = u.nastinator112 = u.nastinator113 = u.nastinator114 = u.nastinator115 = u.nastinator116 = u.nastinator117 = u.nastinator118 = u.nastinator119 = u.nastinator120 = u.nastinator121 = u.nastinator122 = u.nastinator123 = u.nastinator124 = u.nastinator125 = u.nastinator126 = u.nastinator127 = u.nastinator128 = u.nastinator129 = u.nastinator130 = u.nastinator131 = u.nastinator132 = u.nastinator133 = u.nastinator134 = u.nastinator135 = u.nastinator136 = u.nastinator137 = u.nastinator138 = u.nastinator139 = u.nastinator140 = u.nastinator141 = u.nastinator142 = u.nastinator143 = u.nastinator144 = u.nastinator145 = u.nastinator146 = u.nastinator147 = u.nastinator148 = u.nastinator149 = u.nastinator150 = u.nastinator151 = u.nastinator152 = u.nastinator153 = u.nastinator154 = u.nastinator155 = u.nastinator156 = u.nastinator157 = u.nastinator158 = u.nastinator159 = u.nastinator160 = u.nastinator161 = u.nastinator162 = u.nastinator163 = u.nastinator164 = u.nastinator165 = u.nastinator166 = u.nastinator167 = u.nastinator168 = u.nastinator169 = u.nastinator170 = u.nastinator171 = u.nastinator172 = u.nastinator173 = u.nastinator174 = u.nastinator175 = u.nastinator176 = u.nastinator177 = u.nastinator178 = u.nastinator179 = u.nastinator180 = u.nastinator181 = u.nastinator182 = u.nastinator183 = u.nastinator184 = u.nastinator185 = u.nastinator186 = u.nastinator187 = u.nastinator188 = u.nastinator189 = u.nastinator190 = u.nastinator191 = u.nastinator192 = u.nastinator193 = u.nastinator194 = u.nastinator195 = u.nastinator196 = u.nastinator197 = u.nastinator198 = u.nastinator199 = u.nastinator200 = u.nastinator201 = u.nastinator202 = u.nastinator203 = u.nastinator204 = u.nastinator205 = u.nastinator206 = u.nastinator207 = u.nastinator208 = u.nastinator209 = u.nastinator210 = u.nastinator211 = u.nastinator212 = u.nastinator213 = u.nastinator214 = u.nastinator215 = u.nastinator216 = u.nastinator217 = u.nastinator218 = u.nastinator219 = u.nastinator220 = u.nastinator221 = u.nastinator222 = u.nastinator223 = u.nastinator224 = u.nastinator225 = u.nastinator226 = u.nastinator227 = u.nastinator228 = u.nastinator229 = u.nastinator230 = u.nastinator231 = u.nastinator232 = u.nastinator233 = u.nastinator234 = u.nastinator235 = 0;

		switch (rnd(235)) {

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
			case 170: u.nastinator170 = 1; break;
			case 171: u.nastinator171 = 1; break;
			case 172: u.nastinator172 = 1; break;
			case 173: u.nastinator173 = 1; break;
			case 174: u.nastinator174 = 1; break;
			case 175: u.nastinator175 = 1; break;
			case 176: u.nastinator176 = 1; break;
			case 177: u.nastinator177 = 1; break;
			case 178: u.nastinator178 = 1; break;
			case 179: u.nastinator179 = 1; break;
			case 180: u.nastinator180 = 1; break;
			case 181: u.nastinator181 = 1; break;
			case 182: u.nastinator182 = 1; break;
			case 183: u.nastinator183 = 1; break;
			case 184: u.nastinator184 = 1; break;
			case 185: u.nastinator185 = 1; break;
			case 186: u.nastinator186 = 1; break;
			case 187: u.nastinator187 = 1; break;
			case 188: u.nastinator188 = 1; break;
			case 189: u.nastinator189 = 1; break;
			case 190: u.nastinator190 = 1; break;
			case 191: u.nastinator191 = 1; break;
			case 192: u.nastinator192 = 1; break;
			case 193: u.nastinator193 = 1; break;
			case 194: u.nastinator194 = 1; break;
			case 195: u.nastinator195 = 1; break;
			case 196: u.nastinator196 = 1; break;
			case 197: u.nastinator197 = 1; break;
			case 198: u.nastinator198 = 1; break;
			case 199: u.nastinator199 = 1; break;
			case 200: u.nastinator200 = 1; break;
			case 201: u.nastinator201 = 1; break;
			case 202: u.nastinator202 = 1; break;
			case 203: u.nastinator203 = 1; break;
			case 204: u.nastinator204 = 1; break;
			case 205: u.nastinator205 = 1; break;
			case 206: u.nastinator206 = 1; break;
			case 207: u.nastinator207 = 1; break;
			case 208: u.nastinator208 = 1; break;
			case 209: u.nastinator209 = 1; break;
			case 210: u.nastinator210 = 1; break;
			case 211: u.nastinator211 = 1; break;
			case 212: u.nastinator212 = 1; break;
			case 213: u.nastinator213 = 1; break;
			case 214: u.nastinator214 = 1; break;
			case 215: u.nastinator215 = 1; break;
			case 216: u.nastinator216 = 1; break;
			case 217: u.nastinator217 = 1; break;
			case 218: u.nastinator218 = 1; break;
			case 219: u.nastinator219 = 1; break;
			case 220: u.nastinator220 = 1; break;
			case 221: u.nastinator221 = 1; break;
			case 222: u.nastinator222 = 1; break;
			case 223: u.nastinator223 = 1; break;
			case 224: u.nastinator224 = 1; break;
			case 225: u.nastinator225 = 1; break;
			case 226: u.nastinator226 = 1; break;
			case 227: u.nastinator227 = 1; break;
			case 228: u.nastinator228 = 1; break;
			case 229: u.nastinator229 = 1; break;
			case 230: u.nastinator230 = 1; break;
			case 231: u.nastinator231 = 1; break;
			case 232: u.nastinator232 = 1; break;
			case 233: u.nastinator233 = 1; break;
			case 234: u.nastinator234 = 1; break;
			case 235: u.nastinator235 = 1; break;

		}

		while (!rn2(3)) {

			switch (rnd(235)) {

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
			case 170: u.nastinator170 = 1; break;
			case 171: u.nastinator171 = 1; break;
			case 172: u.nastinator172 = 1; break;
			case 173: u.nastinator173 = 1; break;
			case 174: u.nastinator174 = 1; break;
			case 175: u.nastinator175 = 1; break;
			case 176: u.nastinator176 = 1; break;
			case 177: u.nastinator177 = 1; break;
			case 178: u.nastinator178 = 1; break;
			case 179: u.nastinator179 = 1; break;
			case 180: u.nastinator180 = 1; break;
			case 181: u.nastinator181 = 1; break;
			case 182: u.nastinator182 = 1; break;
			case 183: u.nastinator183 = 1; break;
			case 184: u.nastinator184 = 1; break;
			case 185: u.nastinator185 = 1; break;
			case 186: u.nastinator186 = 1; break;
			case 187: u.nastinator187 = 1; break;
			case 188: u.nastinator188 = 1; break;
			case 189: u.nastinator189 = 1; break;
			case 190: u.nastinator190 = 1; break;
			case 191: u.nastinator191 = 1; break;
			case 192: u.nastinator192 = 1; break;
			case 193: u.nastinator193 = 1; break;
			case 194: u.nastinator194 = 1; break;
			case 195: u.nastinator195 = 1; break;
			case 196: u.nastinator196 = 1; break;
			case 197: u.nastinator197 = 1; break;
			case 198: u.nastinator198 = 1; break;
			case 199: u.nastinator199 = 1; break;
			case 200: u.nastinator200 = 1; break;
			case 201: u.nastinator201 = 1; break;
			case 202: u.nastinator202 = 1; break;
			case 203: u.nastinator203 = 1; break;
			case 204: u.nastinator204 = 1; break;
			case 205: u.nastinator205 = 1; break;
			case 206: u.nastinator206 = 1; break;
			case 207: u.nastinator207 = 1; break;
			case 208: u.nastinator208 = 1; break;
			case 209: u.nastinator209 = 1; break;
			case 210: u.nastinator210 = 1; break;
			case 211: u.nastinator211 = 1; break;
			case 212: u.nastinator212 = 1; break;
			case 213: u.nastinator213 = 1; break;
			case 214: u.nastinator214 = 1; break;
			case 215: u.nastinator215 = 1; break;
			case 216: u.nastinator216 = 1; break;
			case 217: u.nastinator217 = 1; break;
			case 218: u.nastinator218 = 1; break;
			case 219: u.nastinator219 = 1; break;
			case 220: u.nastinator220 = 1; break;
			case 221: u.nastinator221 = 1; break;
			case 222: u.nastinator222 = 1; break;
			case 223: u.nastinator223 = 1; break;
			case 224: u.nastinator224 = 1; break;
			case 225: u.nastinator225 = 1; break;
			case 226: u.nastinator226 = 1; break;
			case 227: u.nastinator227 = 1; break;
			case 228: u.nastinator228 = 1; break;
			case 229: u.nastinator229 = 1; break;
			case 230: u.nastinator230 = 1; break;
			case 231: u.nastinator231 = 1; break;
			case 232: u.nastinator232 = 1; break;
			case 233: u.nastinator233 = 1; break;
			case 234: u.nastinator234 = 1; break;
			case 235: u.nastinator235 = 1; break;

			}

		}

	if (isnastinator) {

		switch (rnd(235)) {

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
			case 170: u.nastinator170 = 1; break;
			case 171: u.nastinator171 = 1; break;
			case 172: u.nastinator172 = 1; break;
			case 173: u.nastinator173 = 1; break;
			case 174: u.nastinator174 = 1; break;
			case 175: u.nastinator175 = 1; break;
			case 176: u.nastinator176 = 1; break;
			case 177: u.nastinator177 = 1; break;
			case 178: u.nastinator178 = 1; break;
			case 179: u.nastinator179 = 1; break;
			case 180: u.nastinator180 = 1; break;
			case 181: u.nastinator181 = 1; break;
			case 182: u.nastinator182 = 1; break;
			case 183: u.nastinator183 = 1; break;
			case 184: u.nastinator184 = 1; break;
			case 185: u.nastinator185 = 1; break;
			case 186: u.nastinator186 = 1; break;
			case 187: u.nastinator187 = 1; break;
			case 188: u.nastinator188 = 1; break;
			case 189: u.nastinator189 = 1; break;
			case 190: u.nastinator190 = 1; break;
			case 191: u.nastinator191 = 1; break;
			case 192: u.nastinator192 = 1; break;
			case 193: u.nastinator193 = 1; break;
			case 194: u.nastinator194 = 1; break;
			case 195: u.nastinator195 = 1; break;
			case 196: u.nastinator196 = 1; break;
			case 197: u.nastinator197 = 1; break;
			case 198: u.nastinator198 = 1; break;
			case 199: u.nastinator199 = 1; break;
			case 200: u.nastinator200 = 1; break;
			case 201: u.nastinator201 = 1; break;
			case 202: u.nastinator202 = 1; break;
			case 203: u.nastinator203 = 1; break;
			case 204: u.nastinator204 = 1; break;
			case 205: u.nastinator205 = 1; break;
			case 206: u.nastinator206 = 1; break;
			case 207: u.nastinator207 = 1; break;
			case 208: u.nastinator208 = 1; break;
			case 209: u.nastinator209 = 1; break;
			case 210: u.nastinator210 = 1; break;
			case 211: u.nastinator211 = 1; break;
			case 212: u.nastinator212 = 1; break;
			case 213: u.nastinator213 = 1; break;
			case 214: u.nastinator214 = 1; break;
			case 215: u.nastinator215 = 1; break;
			case 216: u.nastinator216 = 1; break;
			case 217: u.nastinator217 = 1; break;
			case 218: u.nastinator218 = 1; break;
			case 219: u.nastinator219 = 1; break;
			case 220: u.nastinator220 = 1; break;
			case 221: u.nastinator221 = 1; break;
			case 222: u.nastinator222 = 1; break;
			case 223: u.nastinator223 = 1; break;
			case 224: u.nastinator224 = 1; break;
			case 225: u.nastinator225 = 1; break;
			case 226: u.nastinator226 = 1; break;
			case 227: u.nastinator227 = 1; break;
			case 228: u.nastinator228 = 1; break;
			case 229: u.nastinator229 = 1; break;
			case 230: u.nastinator230 = 1; break;
			case 231: u.nastinator231 = 1; break;
			case 232: u.nastinator232 = 1; break;
			case 233: u.nastinator233 = 1; break;
			case 234: u.nastinator234 = 1; break;
			case 235: u.nastinator235 = 1; break;

		}

		while ((rnd(7)) < 3) {

			switch (rnd(235)) {

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
			case 170: u.nastinator170 = 1; break;
			case 171: u.nastinator171 = 1; break;
			case 172: u.nastinator172 = 1; break;
			case 173: u.nastinator173 = 1; break;
			case 174: u.nastinator174 = 1; break;
			case 175: u.nastinator175 = 1; break;
			case 176: u.nastinator176 = 1; break;
			case 177: u.nastinator177 = 1; break;
			case 178: u.nastinator178 = 1; break;
			case 179: u.nastinator179 = 1; break;
			case 180: u.nastinator180 = 1; break;
			case 181: u.nastinator181 = 1; break;
			case 182: u.nastinator182 = 1; break;
			case 183: u.nastinator183 = 1; break;
			case 184: u.nastinator184 = 1; break;
			case 185: u.nastinator185 = 1; break;
			case 186: u.nastinator186 = 1; break;
			case 187: u.nastinator187 = 1; break;
			case 188: u.nastinator188 = 1; break;
			case 189: u.nastinator189 = 1; break;
			case 190: u.nastinator190 = 1; break;
			case 191: u.nastinator191 = 1; break;
			case 192: u.nastinator192 = 1; break;
			case 193: u.nastinator193 = 1; break;
			case 194: u.nastinator194 = 1; break;
			case 195: u.nastinator195 = 1; break;
			case 196: u.nastinator196 = 1; break;
			case 197: u.nastinator197 = 1; break;
			case 198: u.nastinator198 = 1; break;
			case 199: u.nastinator199 = 1; break;
			case 200: u.nastinator200 = 1; break;
			case 201: u.nastinator201 = 1; break;
			case 202: u.nastinator202 = 1; break;
			case 203: u.nastinator203 = 1; break;
			case 204: u.nastinator204 = 1; break;
			case 205: u.nastinator205 = 1; break;
			case 206: u.nastinator206 = 1; break;
			case 207: u.nastinator207 = 1; break;
			case 208: u.nastinator208 = 1; break;
			case 209: u.nastinator209 = 1; break;
			case 210: u.nastinator210 = 1; break;
			case 211: u.nastinator211 = 1; break;
			case 212: u.nastinator212 = 1; break;
			case 213: u.nastinator213 = 1; break;
			case 214: u.nastinator214 = 1; break;
			case 215: u.nastinator215 = 1; break;
			case 216: u.nastinator216 = 1; break;
			case 217: u.nastinator217 = 1; break;
			case 218: u.nastinator218 = 1; break;
			case 219: u.nastinator219 = 1; break;
			case 220: u.nastinator220 = 1; break;
			case 221: u.nastinator221 = 1; break;
			case 222: u.nastinator222 = 1; break;
			case 223: u.nastinator223 = 1; break;
			case 224: u.nastinator224 = 1; break;
			case 225: u.nastinator225 = 1; break;
			case 226: u.nastinator226 = 1; break;
			case 227: u.nastinator227 = 1; break;
			case 228: u.nastinator228 = 1; break;
			case 229: u.nastinator229 = 1; break;
			case 230: u.nastinator230 = 1; break;
			case 231: u.nastinator231 = 1; break;
			case 232: u.nastinator232 = 1; break;
			case 233: u.nastinator233 = 1; break;
			case 234: u.nastinator234 = 1; break;
			case 235: u.nastinator235 = 1; break;

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

		if (u.umoved && (uarmf && itemhasappearance(uarmf, APP_RUMBLE_BOOTS) )) {
			wake_nearby();
		}

		if (u.umoved && u.twoweap && uswapwep && uswapwep->oartifact == ART_HENRIETTA_S_MISTAKE && !rn2(10)) {

			doshittrap((struct obj *)0);

		}

		if (uwep && uwep->oartifact == ART_AND_IT_KEEPS_ON_MOVING) pushplayersilently(TRUE);
		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_AND_IT_KEEPS_ON_MOVING) pushplayersilently(TRUE);

		if (uarmh && uarmh->oartifact == ART_ALLURATION && !rn2(1000) ) {
			curse(uarmh);
			if (!rn2(3)) pline("A black glow surrounds your helmet.");
			if (!rn2(20)) NastinessProblem |= FROMOUTSIDE;
		}

		if ((uarmh && itemhasappearance(uarmh, APP_NETRADIATION_HELMET)) && !rn2(5000)) {

			pline("The helmet's radiation damages your health!");
			if (Upolyd && u.mhmax > 1) {
				u.mhmax--;
				if (u.mh > u.mhmax) u.mh = u.mhmax;
			} else if (!Upolyd && u.uhpmax > 1) {
				u.uhpmax--;
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			}
	
		}

		if (u.tsloss_str && !rn2(5000)) {
			u.tsloss_str--;
			if (u.tsloss_str < 0) u.tsloss_str = 0; /* fail safe */
			You_feel("stronger!");
		}
		if (u.tsloss_dex && !rn2(5000)) {
			u.tsloss_dex--;
			if (u.tsloss_dex < 0) u.tsloss_dex = 0; /* fail safe */
			You_feel("nimbler!");
		}
		if (u.tsloss_con && !rn2(5000)) {
			u.tsloss_con--;
			if (u.tsloss_con < 0) u.tsloss_con = 0; /* fail safe */
			You_feel("hardier!");
		}
		if (u.tsloss_wis && !rn2(5000)) {
			u.tsloss_wis--;
			if (u.tsloss_wis < 0) u.tsloss_wis = 0; /* fail safe */
			You_feel("wiser!");
		}
		if (u.tsloss_int && !rn2(5000)) {
			u.tsloss_int--;
			if (u.tsloss_int < 0) u.tsloss_int = 0; /* fail safe */
			You_feel("smarter!");
		}
		if (u.tsloss_cha && !rn2(5000)) {
			u.tsloss_cha--;
			if (u.tsloss_cha < 0) u.tsloss_cha = 0; /* fail safe */
			You_feel("prettier!");
		}

		if (uarmh && uarmh->oartifact == ART_NULL_THE_LIVING_DATABASE && !rn2(5000) ) {
		      (void) cancel_monst(&youmonst, (struct obj *)0, FALSE, TRUE, FALSE);
		}

		if (!rn2(Aggravate_monster ? 4 : 20)) reset_rndmonst(NON_PM);

		if (IntAggravate_monster && !rn2(StrongStealth ? 100000 : Stealth ? 50000 : 5000)) {

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

		if (!rn2(5000) && uarmf && itemhasappearance(uarmf, APP_SPOOKY_BOOTS) ) {
			int aggroamount = rnd(4);
			if (isfriday) aggroamount *= 2;
			u.aggravation = 1;
			u.heavyaggravation = 1;
			while (aggroamount) {

				u.cnd_aggravateamount++;
				makemon(mkclass(S_GHOST,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
				aggroamount--;
				if (aggroamount < 0) aggroamount = 0;
			}
			u.aggravation = 0;
			u.heavyaggravation = 0;
			pline("Several ghosts come out of a portal.");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		}

		if (flags.female && PlayerInSexyFlats && !rn2(10000)) {
			int aggroamount = rnd(6);
			if (isfriday) aggroamount *= 2;
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		      cx = rn2(COLNO);
		      cy = rn2(ROWNO);
			while (aggroamount) {

				aggroamount--;
				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				makemon((struct permonst *)0, cx, cy, MM_ANGRY|MM_FRENZIED);
				if (aggroamount < 0) aggroamount = 0;
			}
			u.aggravation = 0;
			pline("Your sexiness seems to have attracted some monsters...");

		}

		if (ExtAggravate_monster && !rn2(StrongStealth ? 10000 : Stealth ? 5000 : 1000)) {

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

		if (Race_if(PM_HC_ALIEN) && !flags.female && !rn2(StrongStealth ? 3000 : Stealth ? 2000 : 1000)) {

			int aggroamount = rnd(6);
			if (isfriday) aggroamount *= 2;
			u.aggravation = 1;
			u.heavyaggravation = 1;
			reset_rndmonst(NON_PM);
			while (aggroamount) {

				u.cnd_aggravateamount++;
				makemon((struct permonst *)0, u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
				aggroamount--;
				if (aggroamount < 0) aggroamount = 0;
			}
			u.aggravation = 0;
			u.heavyaggravation = 0;
			pline("Several monsters come out of a portal.");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		}

		if (isdemagogue && !rn2(5000)) {

		demagogueparole();
		aggravate();

		if (!rn2(100)) {

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

		      cx = rn2(COLNO);
		      cy = rn2(ROWNO);

			if (!rn2(4)) {

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			randmnst = (rn2(187) + 1);
			randmnsx = (rn2(100) + 1);

				if (wizard || !rn2(10)) pline("You suddenly feel a surge of tension!");

			for (i = 0; i < randsp; i++) {
			/* This function will fill the map with a random amount of monsters of one class. --Amy */

			if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

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

			else if (!rn2(3)) {

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			randmonstforspawn = rndmonst();

			if (wizard || !rn2(10)) You_feel("the arrival of monsters!");

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(randmonstforspawn, cx, cy, MM_ADJACENTOK);
			}

			}

			else if (!rn2(2)) {

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			monstercolor = rnd(15);
			do { monstercolor = rnd(15); } while (monstercolor == CLR_BLUE);

			if (wizard || !rn2(10)) You_feel("a colorful sensation!");

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(colormon(monstercolor), cx, cy, MM_ADJACENTOK);
			}

			}

			else {

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			monstercolor = rnd(376);

			if (wizard || !rn2(10)) You_feel("that a group has arrived!");

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(monstercolor), cx, cy, MM_ADJACENTOK);
			}

			}

			u.aggravation = 0;


		}

		}

		if (Role_if(PM_FEMINIST) && u.ualign.record < 0 && !rn2(StrongStealth ? 100000 : Stealth ? 50000 : 5000)) {
		/* feminist aggravation idea by bugsniper */

			int aggroamount = rnd(6);
			if (isfriday) aggroamount *= 2;
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
			while (aggroamount) {

			int attempts = 0;
			struct permonst *pm = 0;
			aggroamount--;

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
			u.cnd_aggravateamount++;

			} /* while (aggroamount) */

			pline("Several angry females come out of a portal.");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

		}

		if (uarmf && itemhasappearance(uarmf, APP_STANDING_FOOTWEAR) && !rn2(1000)) {
			u.burrowed = 100;
			u.utrap = 100;
			u.utraptype = TT_INFLOOR;
			pline("Suddenly your footwear causes you to sink into the ground!");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		}

		if (uarmh && uarmh->oartifact == ART_JAMILA_S_BELIEF && !rn2(1000)) {
			adjalign(1);
		}

		/* catchall for health/manastones: make sure that if they enter or leave your inventory without the function
		 * making sure that their effect updates, the numbers are corrected periodically */
		if (!rn2(200)) {
			recalc_health();
			recalc_mana();
		}

		if (Race_if(PM_FIXER) && !rn2(1000)) {
			Your("intrinsics change.");
			intrinsicgainorloss();
		}

		if (Race_if(PM_RACE_THAT_DOESN_T_EXIST) && !rn2(100)) {
			attrcurse();
		}

		if (Race_if(PM_RACE_THAT_DOESN_T_EXIST) && !rn2(1000)) {
			eviltechincrease();
		}

		if (Race_if(PM_BODYMORPHER) && !rn2(5000)) {
			Your("body seems to morph...");
			bodymorph();
		}

		if (uarmf && itemhasappearance(uarmf, APP_STANDING_FOOTWEAR) && !rn2(1000)) {
			awaken_monsters(30);
		}

		if (uwep && uwep->oartifact == ART_FULL_LEAD_A_FAST_BEING && !rn2(1000)) {
			You("full-lead a fast-being.");
			incr_itimeout(&HFast, rnd(50));
		}

		/* if you have many forgotten spells, maybe remove one of them entirely --Amy */
		if (!rn2(2000)) {
			removeforgottenspell();
		}

		/* for feminizer hybrid race: re-randomize feminism effect that is active --Amy */
		if (!rn2(5000)) u.feminizeffect = rnd(28); /* amount of feminism trap effects; keyword: "marlena" */

		if (isfeminizer && !rn2(5000)) randomfeminismtrap(rnz( (level_difficulty() + 2) * rnd(50)));

		if (uarmf && uarmf->oartifact == ART_SUBLEVEL_FLOODING && !rn2(10000)) {
			int madepoolQ = 0;

			do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_megafloodingf, (void *)&madepoolQ);
			if (madepoolQ)
				pline("The dungeon is flooded!");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

		}

		if (Role_if(PM_FJORDE) && !rn2(1000)) {
			int madepoolQ = 0;

			do_clear_areaX(u.ux, u.uy, rnd(10), do_fjordefloodingf, (void *)&madepoolQ);
			if (madepoolQ)
				pline("Oh %s, there's %s flooding on this sub level!", rn2(2) ? "damn" : "great", rn2(2) ? "a" : "another");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

		}

		if (uwep && uwep->oartifact == ART_FLAGELLATOR && !rn2(5000)) {
			int attempts = 0;
			register struct permonst *ptrZ;

			do {

				ptrZ = rndmonst();
				attempts++;
				if (!rn2(2000)) reset_rndmonst(NON_PM);

			} while ( (!ptrZ || (ptrZ && !is_neuter(ptrZ))) && attempts < 50000);

			if (is_neuter(ptrZ)) {
				u.wormpolymorph = monsndx(ptrZ);
				You_feel("neuter!");
				polyself(FALSE);
			}

		}

		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_FLAGELLATOR && !rn2(5000)) {
			int attempts = 0;
			register struct permonst *ptrZ;

			do {

				ptrZ = rndmonst();
				attempts++;
				if (!rn2(2000)) reset_rndmonst(NON_PM);

			} while ( (!ptrZ || (ptrZ && !is_neuter(ptrZ))) && attempts < 50000);

			if (is_neuter(ptrZ)) {
				u.wormpolymorph = monsndx(ptrZ);
				You_feel("neuter!");
				polyself(FALSE);
			}

		}

		if (uarmf && itemhasappearance(uarmf, APP_FUNGAL_SANDALS) && !rn2(50000)) {
			u.fungalsandals = TRUE;
			polyself(FALSE);
			u.fungalsandals = FALSE;
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

		if (uwep && uwep->otyp == DEMON_CROSSBOW && !rn2(5000) ) {
			int attempts = 0;
			register struct permonst *ptrZ;

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

newbossO:
			do {
				ptrZ = rndmonst();
				attempts++;
				if (!rn2(2000)) reset_rndmonst(NON_PM);

			} while ( (!ptrZ || (ptrZ && !is_demon(ptrZ))) && attempts < 50000);

			if (ptrZ && is_demon(ptrZ)) {
				(void) makemon(ptrZ, u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
				pline("A demon suddenly appears from nowhere!");
			} else if (rn2(50)) {
				attempts = 0;
				goto newbossO;
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
				if (!rn2(10000)) contaminate(rnd(2), FALSE);
			}
			while (etherquick > 0) {
				etherquick--;
				if (!rn2(5000)) {
					if (!rn2(5)) {
						pline("Your ether equipment causes your contamination to deteriorate.");
						contaminate(rnd(10), TRUE);
					} else {
						contaminate(rnd(10), FALSE);
					}
				}
			}

		}

		if (uwep && uwep->oartifact == ART_ETHER_PENETRATOR && !rn2(100)) contaminate(rnd(10), FALSE);
		if (uswapwep && uswapwep->oartifact == ART_ETHER_PENETRATOR && !rn2(100)) contaminate(rnd(10), FALSE);

		if (In_gammacaves(&u.uz) && !rn2(StrongCont_resist ? 1000 : Cont_resist ? 200 : 100)) {
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

		if (Role_if(PM_WALSCHOLAR) && !rn2(2500)) {
			bad_equipment(1);
		}

		if (Role_if(PM_GRENADONIN) && !rn2(10000)) {
			alter_reality(1);
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		}

		if (Role_if(PM_GRENADONIN) && !rn2(10000)) {
			changehybridization(0);
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		}

		if (Role_if(PM_FEMINIST) && !rn2(5000)) {
			bad_artifact_xtra();
		}

		if (Role_if(PM_TRACER) && !rn2(10000)) {

			boolean havegifts = u.ugifts;

			if (!havegifts) u.ugifts++;

			acqo = mk_artifact((struct obj *)0, !rn2(3) ? A_CHAOTIC : rn2(2) ? A_NEUTRAL : A_LAWFUL, TRUE);
			if (acqo) {
			    dropy(acqo);
				if (P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_ISRESTRICTED) {
					unrestrict_weapon_skill(get_obj_skill(acqo, TRUE));
				} else if (P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_UNSKILLED) {
					unrestrict_weapon_skill(get_obj_skill(acqo, TRUE));
					P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_BASIC;
				} else if (rn2(2) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_BASIC) {
					P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_SKILLED;
				} else if (!rn2(4) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_SKILLED) {
					P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_EXPERT;
				} else if (!rn2(10) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_EXPERT) {
					P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_MASTER;
				} else if (!rn2(100) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_MASTER) {
					P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_GRAND_MASTER;
				} else if (!rn2(200) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_GRAND_MASTER) {
					P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_SUPREME_MASTER;
				}

				if (Race_if(PM_RUSMOT)) {
					if (P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_ISRESTRICTED) {
						unrestrict_weapon_skill(get_obj_skill(acqo, TRUE));
					} else if (P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_UNSKILLED) {
						unrestrict_weapon_skill(get_obj_skill(acqo, TRUE));
						P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_BASIC;
					} else if (rn2(2) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_BASIC) {
						P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_SKILLED;
					} else if (!rn2(4) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_SKILLED) {
						P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_EXPERT;
					} else if (!rn2(10) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_EXPERT) {
						P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_MASTER;
					} else if (!rn2(100) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_MASTER) {
						P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_GRAND_MASTER;
					} else if (!rn2(200) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_GRAND_MASTER) {
						P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_SUPREME_MASTER;
					}
				}

			    discover_artifact(acqo->oartifact);

				if (!havegifts) u.ugifts--;
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
			
			getnastytrapintrinsic();

		}

		if (!rn2(2000) && uwep && uwep->oartifact == ART_RAFSCHAR_S_SUPERWEAPON) {
			
			getnastytrapintrinsic();

		}

		if (!rn2(2000) && uswapwep && uswapwep->oartifact == ART_RAFSCHAR_S_SUPERWEAPON) {

			getnastytrapintrinsic();

		}

		if (!rn2(2000) && u.uprops[EVIL_PATCH_EFFECT].extrinsic) {
			
			getnastytrapintrinsic();

		}

		if (!rn2(2000) && have_evilpatchstone()) {
			
			getnastytrapintrinsic();

		}

		if (!rn2(2000) && u.uprops[ARTIFICER_BUG].extrinsic) {
			bad_artifact();
		}

		if (!rn2(2000) && have_artificialstone()) {
			bad_artifact();
		}

		if (!rn2(200) && HorrorBugEffect) {

			int lcount = rnd(monster_difficulty() ) + 1;

		    if (!obsidianprotection()) switch (rn2(11)) {
		    case 0: make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON),20),
				"horrible sickness", TRUE, SICK_NONVOMITABLE);
			    break;
		    case 1: make_blinded(Blinded + lcount, TRUE);
			    break;
		    case 2: if (!Confusion)
				You("suddenly feel %s.", FunnyHallu ? "trippy" : "confused");
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
		    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
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

		    if (!obsidianprotection()) switch (rn2(11)) {
		    case 0: make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON),20),
				"horrible sickness", TRUE, SICK_NONVOMITABLE);
			    break;
		    case 1: make_blinded(Blinded + lcount, TRUE);
			    break;
		    case 2: if (!Confusion)
				You("suddenly feel %s.", FunnyHallu ? "trippy" : "confused");
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
		    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
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

		    if (!obsidianprotection()) switch (rn2(11)) {
		    case 0: make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON),20),
				"horrible sickness", TRUE, SICK_NONVOMITABLE);
			    break;
		    case 1: make_blinded(Blinded + lcount, TRUE);
			    break;
		    case 2: if (!Confusion)
				You("suddenly feel %s.", FunnyHallu ? "trippy" : "confused");
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
		    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
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

		if (uactivesymbiosis) {
			u.usymbiosisslowturns++;
			if (u.usymbiosisslowturns >= 50) {
				u.usymbiosisslowturns = 0;
				use_skill(P_SYMBIOSIS, 1);
			}
		}

		if ((DeLightBug || u.uprops[DE_LIGHT_BUG].extrinsic || have_delightstone() || (uwep && uwep->oartifact == ART_EGRID_BUG) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_EGRID_BUG) || (uwep && uwep->oartifact == ART_WEAKITE_THRUST) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_WEAKITE_THRUST)) && isok(u.ux, u.uy)) {
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
			else (void) makemon(insidemon(), 0, 0, MM_ANGRY);
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

		if ((uarmc && itemhasappearance(uarmc, APP_ROADMAP_CLOAK)) && !rn2(10000)) {
			if (!HConfusion) HConfusion = 1;
			do_mappingX();
		}

		if (Role_if(PM_KURWA) && !rn2(2000)) { /* re-randomize object appearances */
			initobjectsamnesia();
		}

		if (uarmf && uarmf->oartifact == ART_GRENEUVENIA_S_HUG) {
			randomcuss();
		}

		if ((MommaBugEffect || u.uprops[MOMMA_BUG].extrinsic || (uimplant && uimplant->oartifact == ART_DEINE_MUDDA) || have_mommystone()) && !rn2(100)) {
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

		if ((multi < 0) && (uarmh && itemhasappearance(uarmh, APP_DREAM_HELMET))) {
			if (!Upolyd && u.uhp < u.uhpmax) u.uhp++;
			if (Upolyd && u.mh < u.mhmax) u.mh++;
			if (u.uen < u.uenmax) u.uen++;
			flags.botl = 1;
		}

		/* sleeping heals your symbiote... --Amy */
		if (u.usleep && u.usleep < monstermoves && uactivesymbiosis) {
			if (u.usymbiote.mhp < u.usymbiote.mhpmax) {
				u.usymbiote.mhp++;
				if (is_carvedbed(u.ux, u.uy)) u.usymbiote.mhp += rn2(4);
				if (u.usymbiote.mhp > u.usymbiote.mhpmax) u.usymbiote.mhp = u.usymbiote.mhpmax;
				if (flags.showsymbiotehp) flags.botl = TRUE;
			}

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

			if (!rn2(StrongSleep_resistance ? 1000 : Sleep_resistance ? 200 : 20)) {

				u.cnd_mattressamount++;
				You("suddenly feel an immense need to lie down on the mattress and sleep for a bit.");
				fall_asleep(-rnd(5), TRUE);

			}

			if (isfriday && (multi >= 0) && !rn2(StrongSleep_resistance ? 1000 : Sleep_resistance ? 200 : 20)) {

				u.cnd_mattressamount++;
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
			if (!rn2(2) || !(powerfulimplants())) {
				if (u.uen > 0) u.uen--;
				else if (u.uenmax > 0) u.uenmax--;
				flags.botl = TRUE;
			}
		}

		if ((uarmf && itemhasappearance(uarmf, APP_PINK_SNEAKERS)) && !rn2(1000)) {
			pline("The beguiling stench emanating from your pink sneakers fills the area...");
			badeffect();
			turn_allmonsters();
		}

		if (uarmc && itemhasappearance(uarmc, APP_SPELLSUCKING_CLOAK)) {
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

		if (!In_ZAPM(&u.uz) && iszapem && !(u.zapemescape)) {
			u.zapemescape = TRUE;
			pline("You've escaped from the spaceship. From now on, the level difficulty will be using the actual depth, even if you return to the Space Base. Your ability to level teleport was restored.");
		}

		if (In_sewerplant(&u.uz) && !u.sewerplantcomplete && (dunlev(&u.uz) == dunlevs_in_dungeon(&u.uz)) ) {
			u.sewerplantcomplete = 1;
		      (void) safe_teleds(FALSE);
			pline("Well done, you've reached the bottom of the Sewer Plant! The entrance to the Gamma Caves is now open.");
			if (iszapem && !(u.zapemescape)) {
				pline("The exit of this spaceship was opened and you can go back to the Dungeons of Doom now! However, you might want to finish the Gamma Caves and Mainframe first, because once you leave, the monster difficulty in the entire ZAPM branch will increase.");
			}
		}

		if (In_gammacaves(&u.uz) && !u.gammacavescomplete && (dunlev(&u.uz) == dunlevs_in_dungeon(&u.uz)) ) {
			u.gammacavescomplete = 1;
		      (void) safe_teleds(FALSE);
			pline("Well done, you've reached the bottom of the Gamma Caves! The entrance to the Mainframe is now open.");
		}

		/* freeplay after ascension mode: re-enable passage to the planes once you went to Moloch's sanctum --Amy */
		if (u.freeplaymode && !u.freeplayplanes && on_level(&sanctum_level, &u.uz)) {
			u.freeplayplanes = TRUE;
			pline("The stairs to the Elemental Planes have been permanently re-opened! Well done!");
		}

		/* Imbuing the Bell of Opening must be done before any of the invocation tools work
		 * it will spawn a bunch of quest monsters around the entrance, forcing you to fight your way back out --Amy */

		if (!u.bellimbued && u.uhave.bell && In_bellcaves(&u.uz)) {
			u.bellimbued = 1;
			u.uhpmax += rnd(3);
			u.uenmax += rnd(3);
			if (Upolyd) u.mhmax += rnd(3);

			(void) makemon(&mons[urole.nemesnum], u.ux, u.uy, MM_ANGRY|MM_FRENZIED);

			if (urole.enemy1num != NON_PM) {
				(void) makemon(&mons[urole.enemy1num], u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
				(void) makemon(&mons[urole.enemy1num], u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
				(void) makemon(&mons[urole.enemy1num], u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
				(void) makemon(&mons[urole.enemy1num], u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
				(void) makemon(&mons[urole.enemy1num], u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			}
			if (urole.enemy2num != NON_PM) {
				(void) makemon(&mons[urole.enemy2num], u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
				(void) makemon(&mons[urole.enemy2num], u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			}

			(void) makemon(mkclass(urole.enemy1sym,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			(void) makemon(mkclass(urole.enemy1sym,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);

			(void) makemon(mkclass(urole.enemy2sym,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);

		      (void) safe_teleds(FALSE);

#ifdef RECORD_ACHIEVE

			if (!achieve.imbued_bell) {

				achieve.imbued_bell = TRUE;
				if (uarmc && itemhasappearance(uarmc, APP_TEAM_SPLAT_CLOAK)) pline("TROPHY GET!");
				if (RngeTeamSplat) pline("TROPHY GET!");
				if (Race_if(PM_INHERITOR)) giftartifact();
				if (Race_if(PM_HERALD)) heraldgift();

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
				(void) makemon(&mons[PM_SUPERBEHOLDER], 0, 0, MM_ANGRY|MM_FRENZIED);
			}

		}

		if (!u.amuletimbued2 && u.uhave.amulet && In_ordered(&u.uz)) {
			u.amuletimbued2 = 1;
			qt_pager(QT_AMULETIMBUED2);
		      (void) safe_teleds(FALSE);

			if (!achieve.killed_nightmare) {
				pline("Suddenly, you get the feeling that a nightmarish creature is on the loose...");
				(void) makemon(&mons[PM_UBERNIGHTMARE], 0, 0, MM_ANGRY|MM_FRENZIED);
			}

		}

		if (!u.amuletimbued3 && u.uhave.amulet && In_deadground(&u.uz)) {
			u.amuletimbued3 = 1;
			qt_pager(QT_AMULETIMBUED3);
		      (void) safe_teleds(FALSE);

			if (!achieve.killed_vecna) {
				pline("Suddenly, you get the feeling that an eerie lord of the dead is living here...");
				(void) makemon(&mons[PM_GIGA_VECNA], 0, 0, MM_ANGRY|MM_FRENZIED);
			}

		}

		if (u.amuletimbued1 && u.amuletimbued2 && u.amuletimbued3 && !u.amuletcompletelyimbued && u.uhave.amulet && In_yendorian(&u.uz)) {
			u.amuletcompletelyimbued = 1;
			u.uhpmax += rnd(5);
			u.uenmax += rnd(5);
			if (Upolyd) u.mhmax += rnd(5);

#ifdef RECORD_ACHIEVE

			if (!achieve.imbued_amulet) {

				achieve.imbued_amulet = TRUE;
				if (uarmc && itemhasappearance(uarmc, APP_TEAM_SPLAT_CLOAK)) pline("TROPHY GET!");
				if (RngeTeamSplat) pline("TROPHY GET!");
				if (Race_if(PM_INHERITOR)) giftartifact();
				if (Race_if(PM_HERALD)) heraldgift();

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

		if (u.ucreamed < 0) {
			pline("Fixing a bug that gave you a negative creamed counter...");
			u.ucreamed = 0;
		}
		if (HeavyBlind < 0) HeavyBlind = 0;
		if (Blinded < 0) {
			pline("Fixing a bug that gave you a negative blindness counter...");
			Blinded = 1;
			u.ucreamed = 0;
			make_blinded(0L,TRUE);
		}

		if (HeavyStunned < 0) HeavyStunned = 0;
		if (HStun < 0) {
			pline("Fixing a bug that gave you a negative stun counter...");
			HStun = 1;
			make_stunned(0L,TRUE);
		}

		if (HeavyConfusion < 0) HeavyConfusion = 0;
		if (HConfusion < 0) {
			pline("Fixing a bug that gave you a negative confusion counter...");
			HConfusion = 1;
			make_confused(0L,TRUE);
		}

		if (HeavyNumbed < 0) HeavyNumbed = 0;
		if (HNumbed < 0) {
			pline("Fixing a bug that gave you a negative numbness counter...");
			HNumbed = 1;
			make_numbed(0L,TRUE);
		}

		if (HeavyFeared < 0) HeavyFeared = 0;
		if (HFeared < 0) {
			pline("Fixing a bug that gave you a negative fear counter...");
			HFeared = 1;
			make_feared(0L,TRUE);
		}

		if (HeavyFrozen < 0) HeavyFrozen = 0;
		if (HFrozen < 0) {
			pline("Fixing a bug that gave you a negative freeze counter...");
			HFrozen = 1;
			make_frozen(0L,TRUE);
		}

		if (HeavyBurned < 0) HeavyBurned = 0;
		if (HBurned < 0) {
			pline("Fixing a bug that gave you a negative burn counter...");
			HBurned = 1;
			make_burned(0L,TRUE);
		}

		if (HeavyDimmed < 0) HeavyDimmed = 0;
		if (HDimmed < 0) {
			pline("Fixing a bug that gave you a negative dimness counter...");
			HDimmed = 1;
			make_dimmed(0L,TRUE);
		}

		if (HeavyHallu < 0) HeavyHallu = 0;
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
			u.youarereallydead = 1;
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

		if (FemtrapActiveFemmy && !rn2(500) ) {
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

		if (FemtrapActiveLudgera && !rn2(500) ) {
			struct permonst *pm = 0;
			int attempts = 0;

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

newbossL:
			do {
				pm = rndmonst();
				attempts++;
				if (!rn2(2000)) reset_rndmonst(NON_PM);

			} while ( (!pm || (pm && !(pm->msound == MS_FART_LOUD))) && attempts < 50000);

			if (!pm && rn2(50) ) {
				attempts = 0;
				goto newbossL;
			}
			if (pm && !(pm->msound == MS_FART_LOUD) && rn2(50) ) {
				attempts = 0;
				goto newbossL;
			}

			if (pm) (void) makemon(pm, 0, 0, NO_MM_FLAGS);

			u.aggravation = 0;

		}


		/* the automatic relocation on certain dungeons can make the game almost unwinnable if you end up in a place
		 * surrounded by undiggable walls... so those places give relocatitis now :P --Amy */
		if ((In_bellcaves(&u.uz) || In_deadground(&u.uz) || In_ordered(&u.uz) || In_forging(&u.uz) || (In_netherrealm(&u.uz) && !u.netherrealmcomplete && (dunlev(&u.uz) == dunlevs_in_dungeon(&u.uz))) ) && !rn2(1111)) {

			pline("Suddenly, a void jumpgate appears.");
			getlin ("Do you want to enter the jumpgate and be teleported to a random location on this level? [y/yes/no]",buf);
			(void) lcase (buf);
			if (!(strcmp (buf, "yes")) || !(strcmp (buf, "y"))) {
				pline("Brrrr... it's deathly cold.");
			      (void) safe_teleds(FALSE);
			}
		}
		if ((In_bellcaves(&u.uz) || In_deadground(&u.uz) || In_ordered(&u.uz) || In_forging(&u.uz) || (In_netherrealm(&u.uz) && !u.netherrealmcomplete && (dunlev(&u.uz) == dunlevs_in_dungeon(&u.uz))) ) && !rn2(10000)) {

			pline("Suddenly, a void jumpgate appears and transports you away!");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		      (void) safe_teleds(FALSE);
		}

		/* and just in case you end up stuck somewhere else... this lets the player decline, because it's only meant
		 * as a last resort; can't have the player be relocated against their will on e.g. a Sokoban that has
		 * corridor "rivers" from which there'd be no getting back to the regular map unless you wait for another
		 * 10000 turns! Only teleport the player if they want to, with a full "yes" prompt because it may appear
		 * completely out of the blue and the player shouldn't accidentally skip past it. --Amy */
		if (!(In_endgame(&u.uz)) && !rn2(10000)) {

			pline("Suddenly, a void jumpgate appears.");
			getlin ("Do you want to enter the jumpgate and be teleported to a random location on this level? [y/yes/no]",buf);
			(void) lcase (buf);
			if (!(strcmp (buf, "yes")) || !(strcmp (buf, "y"))) {
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

		if ((uarmh && itemhasappearance(uarmh, APP_BLUY_HELMET)) && !rn2(2000) ) {
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
			maketrap(u.ux + oi, u.uy + oj, SIN_TRAP, 0, FALSE);
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

		if (uwep && uwep->oartifact == ART_YESTERDAY_ASTERISK && !rn2(5000) && !(powerfulimplants() && uimplant && uimplant->oartifact == ART_TIMEAGE_OF_REALMS) ) {

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

		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_YESTERDAY_ASTERISK && !rn2(5000) && !(powerfulimplants() && uimplant && uimplant->oartifact == ART_TIMEAGE_OF_REALMS) ) {

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

		if ( (have_primecurse() || (uinsymbiosis && u.usymbiote.prmcurse)) && !rn2(1000) ) {
			if (!Blind) {
				You("notice a %s glow surrounding you.", hcolor(NH_BLACK));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
			}
			rndcurse();

		}

		/* sticky curses on symbiotes cause periodic damage --Amy */
		if (uinsymbiosis && u.usymbiote.stckcurse && !rn2(50)) {

			u.symbiotedmghack = TRUE;

			if (u.usymbiote.bbcurse || u.usymbiote.evilcurse || u.usymbiote.morgcurse) {
				You("are severely hurt!");
				losehp((u.uhpmax / 2) + 1, "an evil cursed symbiote", KILLED_BY);
				stop_occupation();
			} else if (u.usymbiote.prmcurse) {
				You("writhe in pain.");
				losehp((u.uhpmax / 5) + 1, "a prime cursed symbiote", KILLED_BY);
				stop_occupation();
			} else if (u.usymbiote.hvycurse) {
				You("scream."); /* but not loud enough to actually wake monsters */
				losehp((u.uhpmax / 10) + 1, "a heavily cursed symbiote", KILLED_BY);
				stop_occupation();
			} else {
				You("stop current action."); /* sic from Elona */
				losehp((u.uhpmax / 20) + 1, "a cursed symbiote", KILLED_BY);
				stop_occupation();
			}

			u.symbiotedmghack = FALSE;

		}

		if (uarm && uarm->oartifact == ART_WATER_SHYNESS && !rn2(100) && (levl[u.ux][u.uy].typ == ROOM || levl[u.ux][u.uy].typ == CORR) ) {
			levl[u.ux][u.uy].typ = POOL;
		}

		if (is_sand(u.ux, u.uy) && !(uarmf && itemhasappearance(uarmf, APP_SAND_ALS)) && !(uarmh && itemhasappearance(uarmh, APP_SHEMAGH)) && !(uarmf && uarmf->otyp == STILETTO_SANDALS) && !(Race_if(PM_DUTHOL) && rn2(10)) && !sandprotection() && !rn2(isfriday ? 10 : 20)) {
			You("are caught in a sandstorm, and the sand gets in your %s!", body_part(EYE));
			make_blinded(Blinded + rnd(5),FALSE);
		}

		if (is_nethermist(u.ux, u.uy) && !rn2(isfriday ? 3 : 5)) {

			if ((!Drain_resistance || !rn2(StrongDrain_resistance ? 20 : 5)) && u.uexp > 100) {
				u.uexp -= (u.uexp / 100);
				You_feel("your life slipping away!");
				if (u.uexp < newuexp(u.ulevel - 1)) {
				      losexp("nether mist", TRUE, FALSE);
				}
			}

		}

		if (is_snow(u.ux, u.uy) && !(powerfulimplants() && uimplant && uimplant->oartifact == ART_WHITE_WHALE_HATH_COME) && !rn2(isfriday ? 10 : 20) && (Flying || Levitation)) {
			You("are caught in a snowstorm!");
			make_stunned(Stunned + rnd(5),FALSE);
			stop_occupation();
		}

		if (is_snow(u.ux, u.uy) && !(powerfulimplants() && uimplant && (uimplant->oartifact == ART_WHITE_WHALE_HATH_COME || uimplant->oartifact == ART_DUBAI_TOWER_BREAK)) && !(uarmf && itemhasappearance(uarmf, APP_FLEECY_BOOTS) ) && !(uwep && uwep->oartifact == ART_GLACIERDALE) && !(uarmf && uarmf->oartifact == ART_VERA_S_FREEZER) && !(uarmf && uarmf->oartifact == ART_CORINA_S_SNOWY_TREAD) && !(uarmf && uarmf->oartifact == ART_KATIE_MELUA_S_FLEECINESS) && !rn2(StrongCold_resistance ? 500 : Cold_resistance ? 200 : 50) ) {
			You("freeze!");
			make_frozen(HFrozen + rnz(50),FALSE);
			stop_occupation();
		}

		if (is_styxriver(u.ux, u.uy)) {

			if ((!Flying && !Levitation) || !rn2(5)) {
				Norep("Continued exposure to the Styx River will cause contamination.");
				contaminate(rnd(10 + level_difficulty()), TRUE);
				stop_occupation();
			}

		}

		if (is_burningwagon(u.ux, u.uy)) {
			pline("The wagon burns you!");
			stop_occupation();
			make_burned(HBurned + rnd(10 + level_difficulty()), FALSE);
			if (!Fire_resistance || !rn2(StrongFire_resistance ? 20 : 5)) losehp(rnd(5 + (level_difficulty() / 3)), "a burning wagon", KILLED_BY);

		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 10 : 50))
		      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);
		    burn_away_slime();
		    if (isevilvariant || !rn2(10)) burnarmor(&youmonst);

		}

		if (is_moorland(u.ux, u.uy) && !Flying && !Levitation && !Race_if(PM_BOVER)) {
			Norep("Swimming in moorland causes continuous damage.");
			losehp(rnd(5 + (level_difficulty() / 5)), "swimming in moorland", KILLED_BY);
			stop_occupation();
		}

		if (Underwater && Race_if(PM_BOVER)) {
			if (Upolyd) {
				u.mh++;
				if (u.mh > u.mhmax) u.mh = u.mhmax;
			} else {
				u.uhp++;
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			}
			flags.botl = TRUE;
		}

		if (is_raincloud(u.ux, u.uy)) {

			if (level.flags.lethe) pline("Sparkling rain washes over you.");
			else pline("Rain washes over you.");
			stop_occupation();

			if (multi >= 0 && !rn2(8)) {

				pline("Kaboom!!!  Boom!!  Boom!!");
				stop_occupation();
				nomul(-3, "hiding from thunderstorm", TRUE);
				nomovemsg = 0;

			}

			if ((!StrongSwimming || !rn2(10)) && (!StrongMagical_breathing || !rn2(10))) {
				water_damage(invent, FALSE, FALSE);
				if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);
			}

		}

		if (is_urinelake(u.ux, u.uy) && !Flying && !Levitation) {

			stop_occupation();
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
			stop_occupation();
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
			(void) maketrap(u.ux, u.uy, FIRE_TRAP, 0, FALSE);
		}
		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_OVERHEATER && !rn2(1000) && !(t_at(u.ux, u.uy) ) ) {
			(void) maketrap(u.ux, u.uy, FIRE_TRAP, 0, FALSE);
		}

		if (uarmf && uarmf->oartifact == ART_ANASTASIA_S_PLAYFULNESS && !rn2(1000) ) {
			int tryct = 0;
			int x, y;

			for (tryct = 0; tryct < 2000; tryct++) {
				x = rn1(COLNO-3,2);
				y = rn2(ROWNO);

				if (x && y && isok(x, y) && (levl[x][y].typ > DBWALL) && !(t_at(x, y)) ) {
					(void) maketrap(x, y, SHIT_TRAP, 0, FALSE);
					break;
					}
			}
		}

		if (uarmf && uarmf->oartifact == ART_SMELL_LIKE_DOG_SHIT && !rn2(1000) ) {
			int tryct = 0;
			int x, y;

			for (tryct = 0; tryct < 2000; tryct++) {
				x = rn1(COLNO-3,2);
				y = rn2(ROWNO);

				if (x && y && isok(x, y) && (levl[x][y].typ > DBWALL) && !(t_at(x, y)) ) {
					(void) maketrap(x, y, SHIT_TRAP, 0, FALSE);
					break;
					}
			}
		}

		if (SpellColorBrown && !rn2(500) ) {
			int tryct = 0;
			int x, y;
			register struct trap *shittrap;

			for (tryct = 0; tryct < 2000; tryct++) {
				x = rn1(COLNO-3,2);
				y = rn2(ROWNO);

				if (x && y && isok(x, y) && (levl[x][y].typ > DBWALL) && !(t_at(x, y)) ) {
					shittrap = maketrap(x, y, SHIT_TRAP, 0, FALSE);
					if (shittrap && !(shittrap->hiddentrap)) {
						shittrap->tseen = 1;
					}
					break;
					}
			}
		}

		if (Role_if(PM_HUSSY) && !rn2(500)) {
			int tryct = 0;
			int x, y;

			int hussytraptype;

			for (tryct = 0; tryct < 2000; tryct++) {
				x = rn1(COLNO-3,2);
				y = rn2(ROWNO);

				if (x && y && isok(x, y) && !(t_at(x, y)) ) {

					hussytraptype = rnd(TRAPNUM-1);
					if (hussytraptype == MAGIC_PORTAL) hussytraptype = ROCKTRAP;
					if (hussytraptype == WISHING_TRAP) hussytraptype = BLINDNESS_TRAP;
					if (hussytraptype == S_PRESSING_TRAP) hussytraptype = ROCKTRAP;
					if (In_sokoban(&u.uz) && rn2(10) && (hussytraptype == HOLE || hussytraptype == TRAPDOOR || hussytraptype == SHAFT_TRAP || hussytraptype == CURRENT_SHAFT || hussytraptype == PIT || hussytraptype == SPIKED_PIT || hussytraptype == GIANT_CHASM || hussytraptype == SHIT_PIT || hussytraptype == MANA_PIT || hussytraptype == ANOXIC_PIT || hussytraptype == ACID_PIT)) hussytraptype = ROCKTRAP;
					if (In_sokoban(&u.uz) && rn2(100) && hussytraptype == NUPESELL_TRAP) hussytraptype = FIRE_TRAP;
					if (hussytraptype == ELDER_TENTACLING_TRAP) hussytraptype = FIRE_TRAP;
					if (hussytraptype == DATA_DELETE_TRAP) hussytraptype = RUST_TRAP;
					if (hussytraptype == ARTIFACT_JACKPOT_TRAP) hussytraptype = MAGIC_TRAP;
					if (hussytraptype == GOOD_ARTIFACT_TRAP) hussytraptype = WEB;
					if (hussytraptype == BOON_TRAP) hussytraptype = MAGIC_BEAM_TRAP;
					if (hussytraptype == LEVEL_TELEP && (level.flags.noteleport || Is_knox(&u.uz) || Is_blackmarket(&u.uz) || Is_aligned_quest(&u.uz) || In_endgame(&u.uz) || In_sokoban_real(&u.uz) ) ) hussytraptype = ANTI_MAGIC;
					if (hussytraptype == LEVEL_BEAMER && (level.flags.noteleport || Is_knox(&u.uz) || Is_blackmarket(&u.uz) || Is_aligned_quest(&u.uz) || In_endgame(&u.uz) || In_sokoban_real(&u.uz) ) ) hussytraptype = ANTI_MAGIC;
					if (hussytraptype == NEXUS_TRAP && (level.flags.noteleport || Is_knox(&u.uz) || Is_blackmarket(&u.uz) || Is_aligned_quest(&u.uz) || In_endgame(&u.uz) || In_sokoban_real(&u.uz) ) ) hussytraptype = ANTI_MAGIC;
					if (hussytraptype == TELEP_TRAP && level.flags.noteleport) hussytraptype = SQKY_BOARD;
					if (hussytraptype == BEAMER_TRAP && level.flags.noteleport) hussytraptype = SQKY_BOARD;
					if ((hussytraptype == TRAPDOOR || hussytraptype == HOLE || hussytraptype == SHAFT_TRAP || hussytraptype == CURRENT_SHAFT) && !Can_fall_thru(&u.uz) && !Is_stronghold(&u.uz) ) hussytraptype = ROCKTRAP;
					if (hussytraptype == ACTIVE_SUPERSCROLLER_TRAP) hussytraptype = SUPERSCROLLER_TRAP;
					if (hussytraptype == AUTOMATIC_SWITCHER) hussytraptype = UNKNOWN_TRAP;

					(void) maketrap(x, y, hussytraptype, 100, FALSE);
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
					(void) maketrap(x, y, SHIT_TRAP, 0, FALSE);
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
			if (powerfulimplants()) use_skill(P_HEALING_SPELL, 1);
		}

		if (!PlayerCannotUseSkills && PlayerInSexyFlats && HStun) {

			int sexyflatchance = 0;
			switch (P_SKILL(P_SEXY_FLATS)) {
				case P_BASIC: sexyflatchance = 1; break;
				case P_SKILLED: sexyflatchance = 2; break;
				case P_EXPERT: sexyflatchance = 3; break;
				case P_MASTER: sexyflatchance = 4; break;
				case P_GRAND_MASTER: sexyflatchance = 5; break;
				case P_SUPREME_MASTER: sexyflatchance = 6; break;

			}
			if (sexyflatchance > rn2(100)) make_stunned(0L,TRUE);

		}

		if (!PlayerCannotUseSkills && PlayerInSexyFlats && HDimmed) {

			int sexyflatchance = 0;
			switch (P_SKILL(P_SEXY_FLATS)) {
				case P_BASIC: sexyflatchance = 1; break;
				case P_SKILLED: sexyflatchance = 2; break;
				case P_EXPERT: sexyflatchance = 3; break;
				case P_MASTER: sexyflatchance = 4; break;
				case P_GRAND_MASTER: sexyflatchance = 5; break;
				case P_SUPREME_MASTER: sexyflatchance = 6; break;

			}
			if (sexyflatchance > rn2(100)) make_dimmed(0L,TRUE);

		}

		if (uarmf && itemhasappearance(uarmf, APP_PLOF_HEELS) ) {
			if (HStun > 2 && !rn2(5)) {
				HStun -= rnd(20);
				if (HStun < 2) HStun = 2;
			}
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
				FunnyHallu ? pline("The rancid goo is gone! Yay!") : pline_The("slime disappears.");
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
					(void) maketrap(x, y, SHIT_TRAP, 0, FALSE);
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
					(void) maketrap(x, y, NATALJE_TRAP, 0, FALSE);
					break;
					}
			}
		}

		if (FemtrapActiveFemmy && !rn2(200)) {

			int tryct = 0;
			int x, y;

			for (tryct = 0; tryct < 2000; tryct++) {
				x = rn1(COLNO-3,2);
				y = rn2(ROWNO);

				if (x && y && isok(x, y) && (levl[x][y].typ > DBWALL) && !(t_at(x, y)) ) {
					(void) maketrap(x, y, FART_TRAP, 0, TRUE);
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
					(void) maketrap(x, y, FART_TRAP, 0, TRUE);
					break;
					}
			}

		}

		if ( (uarmf && itemhasappearance(uarmf, APP_FEMMY_BOOTS)) && !rn2(1000) ) {

			int tryct = 0;
			int x, y;

			for (tryct = 0; tryct < 2000; tryct++) {
				x = rn1(COLNO-3,2);
				y = rn2(ROWNO);

				if (x && y && isok(x, y) && (levl[x][y].typ > DBWALL) && !(t_at(x, y)) ) {
					(void) maketrap(x, y, FART_TRAP, 0, TRUE);
					break;
					}
			}

		}

		if (FemtrapActiveFemmy && !rn2(200)) {

			int tryct = 0;
			int x, y;

			for (tryct = 0; tryct < 2000; tryct++) {
				x = rn1(COLNO-3,2);
				y = rn2(ROWNO);

				if (x && y && isok(x, y) && (levl[x][y].typ > DBWALL) && !(t_at(x, y)) ) {
					(void) maketrap(x, y, HEEL_TRAP, 0, FALSE);
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
					(void) maketrap(x, y, SHIT_TRAP, 0, FALSE);
					break;
					}
			}
		}

		if (FemtrapActiveAnastasia && !rn2(250) ) {
			int tryct = 0;
			int x, y;

			for (tryct = 0; tryct < 2000; tryct++) {
				x = rn1(COLNO-3,2);
				y = rn2(ROWNO);

				if (x && y && isok(x, y) && (levl[x][y].typ > DBWALL) && !(t_at(x, y)) ) {
					(void) maketrap(x, y, SHIT_TRAP, 0, FALSE);
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
					(void) maketrap(x, y, SHIT_TRAP, 0, FALSE);
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

			makerandomtrap(FALSE);

		}

		if (StrongConflict && !rn2(500)) { /* bullshit downside --Amy :P */

			makerandomtrap(FALSE);

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
					(void) maketrap(x, y, SHIT_TRAP, 0, FALSE);
					break;
					}
			}
		}

		if ( (have_morgothiancurse() || (uinsymbiosis && u.usymbiote.morgcurse) || (uamul && uamul->oartifact == ART_NOW_YOU_HAVE_LOST) || (uarmc && uarmc->oartifact == ART_BLACK_VEIL_OF_BLACKNESS) || (uarmc && uarmc->oartifact == ART_ARABELLA_S_WAND_BOOSTER) || (uarmf && uarmf->oartifact == ART_KYLIE_LUM_S_SNAKESKIN_BOOT && !Role_if(PM_TOPMODEL) ) || (uarmh && uarmh->oartifact == ART_MASSIVE_IRON_CROWN_OF_MORG) || (uwep && uwep->oartifact == ART_GUN_CONTROL_LAWS) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_GUN_CONTROL_LAWS) ) && !rn2(isfriday ? 200 : 500) ) { /* was 1 in 50 in ToME */
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
					if (StrongFree_action) nomul(-rnd(2), "paralyzed by the ancient morgotian curse", TRUE);
					else if (Free_action) nomul(-rnd(3), "paralyzed by the ancient morgotian curse", TRUE);
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
					(void) adjattrib(rn2(A_MAX), -5, FALSE, TRUE);
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
			 	    (void) makemon(mkclass(S_DEMON,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			 	    (void) makemon(mkclass(S_DEMON,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			 	    (void) makemon(mkclass(S_DEMON,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			 	    (void) makemon(mkclass(S_DEMON,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			 	    (void) makemon(mkclass(S_DEMON,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			 	    (void) makemon(mkclass(S_DEMON,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			 	    (void) makemon(mkclass(S_DEMON,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			 	    (void) makemon(mkclass(S_DEMON,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);

					u.aggravation = 0;

					You("suddenly have company.");
					break;
				case 29:
				case 30: /* drain all stats by one */
					(void) adjattrib(A_STR, -1, FALSE, TRUE);
					(void) adjattrib(A_INT, -1, FALSE, TRUE);
					(void) adjattrib(A_DEX, -1, FALSE, TRUE);
					(void) adjattrib(A_CHA, -1, FALSE, TRUE);
					(void) adjattrib(A_CON, -1, FALSE, TRUE);
					(void) adjattrib(A_WIS, -1, FALSE, TRUE);
					break;
			}
		}

		if ( (have_topiylinencurse() || (uinsymbiosis && u.usymbiote.evilcurse) || (uamul && uamul->oartifact == ART_SURTERSTAFF && uwep && (weapon_type(uwep) == P_QUARTERSTAFF)) || (uarmh && uarmh->oartifact == ART_IRON_HELM_OF_GORLIM) ) && !rn2(isfriday ? 300 : 1000) ) { /* was 1 in 100 in ToME */
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
			 	    (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			 	    (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			 	    (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			 	    (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			 	    if (!rn2(2)) (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			 	    if (!rn2(5)) (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			 	    if (!rn2(12)) (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			 	    if (!rn2(27)) (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
					u.aggravation = 0;
					break;
				case 13:
				case 14:
				case 15: /* level drain */
					if(!Drain_resistance || !rn2(StrongDrain_resistance ? 15 : 4) )
					    losexp("topi ylinen drainage", FALSE, TRUE);
					break;
				case 16:
				case 17:
				case 18:
				case 19:
				case 20: /* paralysis: up to 3 turns with free action, up to 13 without */
					You_feel("like a statue!");
					if (StrongFree_action) nomul(-rnd(2), "paralyzed by topi ylinen's curse", TRUE);
					else if (Free_action) nomul(-rnd(3), "paralyzed by topi ylinen's curse", TRUE);
					else nomul(-rnd(13), "paralyzed by topi ylinen's curse", TRUE);
					break;
				case 21:
				case 22:
				case 23: /* drain a random stat by 3 points */
					(void) adjattrib(rn2(A_MAX), -3, FALSE, TRUE);
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
					if (rn2(2)) (void) adjattrib(A_STR, -1, FALSE, TRUE);
					if (rn2(2)) (void) adjattrib(A_INT, -1, FALSE, TRUE);
					if (rn2(2)) (void) adjattrib(A_DEX, -1, FALSE, TRUE);
					if (rn2(2)) (void) adjattrib(A_CHA, -1, FALSE, TRUE);
					if (rn2(2)) (void) adjattrib(A_CON, -1, FALSE, TRUE);
					if (rn2(2)) (void) adjattrib(A_WIS, -1, FALSE, TRUE);
					break;
			}
		}

		if ( (have_blackbreathcurse() || (uinsymbiosis && u.usymbiote.bbcurse) || (uamul && uamul->oartifact == ART_SURTERSTAFF && !(uwep && (weapon_type(uwep) == P_QUARTERSTAFF))) ) && !rn2( (Race_if(PM_HOBBIT) || Role_if(PM_RINGSEEKER) ) ? 500 : 200) ) {
			/* was 1 in 20 in ToME, or 1 in 50 if you were a hobbit */
			if (!rn2(5)) { /* level drain */
				if(!Drain_resistance || !rn2(StrongDrain_resistance ? 15 : 4) )
				    losexp("black breath drainage", FALSE, TRUE);
				break;
			} else { /* drain a random stat */
				(void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
			}
		}

		if ( (have_blackbreathcurse() || (uinsymbiosis && u.usymbiote.bbcurse) || (uamul && uamul->oartifact == ART_SURTERSTAFF && !(uwep && (weapon_type(uwep) == P_QUARTERSTAFF))) ) && isfriday && !rn2( (Race_if(PM_HOBBIT) || Role_if(PM_RINGSEEKER) ) ? 500 : 200) ) {
			if (!rn2(5)) { /* level drain */
				if(!Drain_resistance || !rn2(StrongDrain_resistance ? 15 : 4) )
				    losexp("black breath drainage", FALSE, TRUE);
				break;
			} else { /* drain a random stat */
				(void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
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
			u.cnd_disenchantamount++;
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		}

		if (FemtrapActiveSolvejg && !rn2(200)) {

			aggravate();
			Your("loud voice aggravates the denizens of the dungeon.");

		}

		if (uarm && uarm->oartifact == ART_MITHRAL_CANCELLATION && !rn2(1000)) {

			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(10)) (void) drain_item(otmpE);
			}
			Your("equipment seems less effective.");
			u.cnd_disenchantamount++;
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		}

		if (uwep && uwep->oartifact == ART_KUSANAGI_NO_TSURUGI && !rn2(1000)) {

			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(10)) (void) drain_item(otmpE);
			}
			Your("equipment seems less effective.");
			u.cnd_disenchantamount++;
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		}

		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_KUSANAGI_NO_TSURUGI && !rn2(1000)) {

			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(10)) (void) drain_item(otmpE);
			}
			Your("equipment seems less effective.");
			u.cnd_disenchantamount++;
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		}

		if (uwep && uwep->oartifact == ART_ARABELLA_S_ARTIFACT_CREATI && !rn2(1000)) {

			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(10)) (void) drain_item(otmpE);
			}
			Your("equipment seems less effective.");
			u.cnd_disenchantamount++;
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		}

		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_ARABELLA_S_ARTIFACT_CREATI && !rn2(1000)) {

			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(10)) (void) drain_item(otmpE);
			}
			Your("equipment seems less effective.");
			u.cnd_disenchantamount++;
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		}

		if (u.uprops[RECURRING_DISENCHANT].extrinsic && !rn2(1000)) {

			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(10)) (void) drain_item(otmpE);
			}
			Your("equipment seems less effective.");
			u.cnd_disenchantamount++;
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		}

		if (uarmg && uarmg->oartifact == ART_DISENCHANTING_BLACKNESS && !rn2(1000)) {

			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(10)) (void) drain_item_severely(otmpE);
			}
			Your("equipment seems less effective.");
			u.cnd_disenchantamount++;
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		}

		if (uarmh && uarmh->oartifact == ART_DRELITT && !rn2(1000)) {

			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(10)) (void) drain_item(otmpE);
			}
			Your("equipment seems less effective.");
			u.cnd_disenchantamount++;
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		}

		if (have_disenchantmentstone() && !rn2(1000)) {

			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(10)) (void) drain_item(otmpE);
			}
			Your("equipment seems less effective.");
			u.cnd_disenchantamount++;
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		}

		if (ChaosTerrain && !rn2(5) && (!In_sokoban(&u.uz) || !rn2(5) ) ) {

			int chaosx, chaosy;
			chaosx = rn1(COLNO-3,2);
			chaosy = rn2(ROWNO);
			if (chaosx && chaosy && isok(chaosx, chaosy) && (levl[chaosx][chaosy].typ == ROOM || levl[chaosx][chaosy].typ == CORR) ) {
				levl[chaosx][chaosy].typ = randomwalltype();
				if (!(levl[chaosx][chaosy].wall_info & W_EASYGROWTH)) levl[chaosx][chaosy].wall_info |= W_HARDGROWTH;
				blockorunblock_point(chaosx,chaosy);
				del_engr_at(chaosx,chaosy);
				newsym(chaosx,chaosy);
			}

		}

		if (BadPartBug && !rn2(20) && (!In_sokoban(&u.uz) || !rn2(5) ) ) {
			int chaosx, chaosy;
			chaosx = rn1(COLNO-3,2);
			chaosy = rn2(ROWNO);

			if (chaosx && chaosy && isok(chaosx, chaosy) && levl[chaosx][chaosy].typ <= ROCKWALL && ((levl[chaosx][chaosy].wall_info & W_NONDIGGABLE) == 0) ) {

				boolean neighborbad = 0;
				int bpx, bpy;

				for (bpx= -1; bpx<=1; bpx++) for(bpy= -1; bpy<=1; bpy++) {
					if (!bpx && !bpy) continue;
					if (!isok(chaosx+bpx, chaosy+bpy)) continue;
					if (levl[chaosx+bpx][chaosy+bpy].typ == DOOR || levl[chaosx+bpx][chaosy+bpy].typ == CORR || levl[chaosx+bpx][chaosy+bpy].typ == ROOM || levl[chaosx+bpx][chaosy+bpy].typ == STAIRS || levl[chaosx+bpx][chaosy+bpy].typ == LADDER) neighborbad = 1;
				}

				if (!neighborbad) {
					levl[chaosx][chaosy].typ = randomwalltype();
					del_engr_at(chaosx,chaosy);
					newsym(chaosx,chaosy);
					blockorunblock_point(chaosx,chaosy);

					if (!rn2(40) && !t_at(chaosx, chaosy)) (void) maketrap(chaosx, chaosy, randominsidetrap(), 100, FALSE);
				}

			}

		}

		if (u.uprops[BAD_PARTS].extrinsic && !rn2(20) && (!In_sokoban(&u.uz) || !rn2(5) ) ) {
			int chaosx, chaosy;
			chaosx = rn1(COLNO-3,2);
			chaosy = rn2(ROWNO);

			if (chaosx && chaosy && isok(chaosx, chaosy) && levl[chaosx][chaosy].typ <= ROCKWALL && ((levl[chaosx][chaosy].wall_info & W_NONDIGGABLE) == 0) ) {

				boolean neighborbad = 0;
				int bpx, bpy;

				for (bpx= -1; bpx<=1; bpx++) for(bpy= -1; bpy<=1; bpy++) {
					if (!bpx && !bpy) continue;
					if (!isok(chaosx+bpx, chaosy+bpy)) continue;
					if (levl[chaosx+bpx][chaosy+bpy].typ == DOOR || levl[chaosx+bpx][chaosy+bpy].typ == CORR || levl[chaosx+bpx][chaosy+bpy].typ == ROOM || levl[chaosx+bpx][chaosy+bpy].typ == STAIRS || levl[chaosx+bpx][chaosy+bpy].typ == LADDER) neighborbad = 1;
				}

				if (!neighborbad) {
					levl[chaosx][chaosy].typ = randomwalltype();
					del_engr_at(chaosx,chaosy);
					newsym(chaosx,chaosy);
					blockorunblock_point(chaosx,chaosy);

					if (!rn2(40) && !t_at(chaosx, chaosy)) (void) maketrap(chaosx, chaosy, randominsidetrap(), 100, FALSE);
				}

			}

		}

		if (have_badpartstone() && !rn2(20) && (!In_sokoban(&u.uz) || !rn2(5) ) ) {
			int chaosx, chaosy;
			chaosx = rn1(COLNO-3,2);
			chaosy = rn2(ROWNO);

			if (chaosx && chaosy && isok(chaosx, chaosy) && levl[chaosx][chaosy].typ <= ROCKWALL && ((levl[chaosx][chaosy].wall_info & W_NONDIGGABLE) == 0) ) {

				boolean neighborbad = 0;
				int bpx, bpy;

				for (bpx= -1; bpx<=1; bpx++) for(bpy= -1; bpy<=1; bpy++) {
					if (!bpx && !bpy) continue;
					if (!isok(chaosx+bpx, chaosy+bpy)) continue;
					if (levl[chaosx+bpx][chaosy+bpy].typ == DOOR || levl[chaosx+bpx][chaosy+bpy].typ == CORR || levl[chaosx+bpx][chaosy+bpy].typ == ROOM || levl[chaosx+bpx][chaosy+bpy].typ == STAIRS || levl[chaosx+bpx][chaosy+bpy].typ == LADDER) neighborbad = 1;
				}

				if (!neighborbad) {
					levl[chaosx][chaosy].typ = randomwalltype();
					del_engr_at(chaosx,chaosy);
					newsym(chaosx,chaosy);
					blockorunblock_point(chaosx,chaosy);

					if (!rn2(40) && !t_at(chaosx, chaosy)) (void) maketrap(chaosx, chaosy, randominsidetrap(), 100, FALSE);
				}

			}

		}

		if (CompletelyBadPartBug && !rn2(5) && (!In_sokoban(&u.uz) || !rn2(5) ) ) {
			int chaosx, chaosy;
			chaosx = rn1(COLNO-3,2);
			chaosy = rn2(ROWNO);

			if (chaosx && chaosy && isok(chaosx, chaosy) && levl[chaosx][chaosy].typ <= ROCKWALL && ((levl[chaosx][chaosy].wall_info & W_NONDIGGABLE) == 0) ) {

				levl[chaosx][chaosy].typ = randomwalltype();
				del_engr_at(chaosx,chaosy);
				newsym(chaosx,chaosy);
				blockorunblock_point(chaosx,chaosy);

				if (!rn2(15) && !t_at(chaosx, chaosy)) (void) maketrap(chaosx, chaosy, randominsidetrap(), 100, FALSE);

			}

		}

		if (u.uprops[COMPLETELY_BAD_PARTS].extrinsic && !rn2(5) && (!In_sokoban(&u.uz) || !rn2(5) ) ) {
			int chaosx, chaosy;
			chaosx = rn1(COLNO-3,2);
			chaosy = rn2(ROWNO);

			if (chaosx && chaosy && isok(chaosx, chaosy) && levl[chaosx][chaosy].typ <= ROCKWALL && ((levl[chaosx][chaosy].wall_info & W_NONDIGGABLE) == 0) ) {

				levl[chaosx][chaosy].typ = randomwalltype();
				del_engr_at(chaosx,chaosy);
				newsym(chaosx,chaosy);
				blockorunblock_point(chaosx,chaosy);

				if (!rn2(15) && !t_at(chaosx, chaosy)) (void) maketrap(chaosx, chaosy, randominsidetrap(), 100, FALSE);

			}

		}

		if (have_completelybadpartstone() && !rn2(5) && (!In_sokoban(&u.uz) || !rn2(5) ) ) {
			int chaosx, chaosy;
			chaosx = rn1(COLNO-3,2);
			chaosy = rn2(ROWNO);

			if (chaosx && chaosy && isok(chaosx, chaosy) && levl[chaosx][chaosy].typ <= ROCKWALL && ((levl[chaosx][chaosy].wall_info & W_NONDIGGABLE) == 0) ) {

				levl[chaosx][chaosy].typ = randomwalltype();
				del_engr_at(chaosx,chaosy);
				newsym(chaosx,chaosy);
				blockorunblock_point(chaosx,chaosy);

				if (!rn2(15) && !t_at(chaosx, chaosy)) (void) maketrap(chaosx, chaosy, randominsidetrap(), 100, FALSE);

			}

		}

		if (FemtrapActiveYvonne && !rn2(250)) {

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

		if (powerfulimplants() && uimplant && uimplant->oartifact == ART_MIGHTY_MOLASS && !rn2(500)) {
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
		if (isok(u.ux, u.uy) && isok(u.ux + 1, u.uy) && artifact_door(u.ux + 1, u.uy) == ART_GAUNTLET_KEY && Is_lawful_quest(&u.uz)) {
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
				blockorunblock_point(chaosx,chaosy);
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
				blockorunblock_point(chaosx,chaosy);
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
				blockorunblock_point(chaosx,chaosy);
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
				blockorunblock_point(chaosx,chaosy);
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
				blockorunblock_point(chaosx,chaosy);
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

		if (uarmg && itemhasappearance(uarmg, APP_GRAFFITI_GLOVES) && !rn2(2000) ) {
		    incr_itimeout(&Glib, 2); /* just enough to make you drop your weapon */
		}

		if (uarmg && itemhasappearance(uarmg, APP_FATAL_GLOVES) && !rn2(10000) ) {
			pline("Fatal attraction!");

		{
			for(otmpii = otmpi =invent; otmpii ; otmpi = otmpii) {
				otmpii = otmpi->nobj;
				if (objects[(otmpi)->otyp].oc_material == MT_IRON ) {

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
				if (objects[(otmpi)->otyp].oc_material == MT_IRON ) {

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

		if (uarmg && itemhasappearance(uarmg, APP_VAMPIRIC_GLOVES) && (u.uexp > 100) && !rn2(1000) ) {
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

		if (uarmh && itemhasappearance(uarmh, APP_RNG_HELMET) && !rn2(100000) ) {
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

		if (YouAreDeaf) flags.soundok = 0;
		else if (!multi) flags.soundok = 1;

		/* Let's throw a bone to permablind races. --Amy */
		if (!Unidentify && !u.uprops[UNIDENTIFY].extrinsic && !have_unidentifystone() && !(uarmh && uarmh->oartifact == ART_YOU_DON_T_KNOW_SHIT) ) {

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

				if (!rn2(100000) && uarmc && itemhasappearance(uarmc, APP_MYSTERIOUS_CLOAK) && !otmpi->dknown) {
					otmpi->dknown = TRUE;
					You_feel("that you know more about the contents of your inventory...");
				}
				if (!rn2(100000) && uarmc && itemhasappearance(uarmc, APP_MYSTERIOUS_CLOAK) && !otmpi->known) {
					otmpi->known = TRUE;
					You_feel("that you know more about the contents of your inventory...");
				}
				if (!rn2(100000) && uarmc && itemhasappearance(uarmc, APP_MYSTERIOUS_CLOAK) && !otmpi->bknown) {
					otmpi->bknown = TRUE;
					You_feel("that you know more about the contents of your inventory...");
				}
				if (!rn2(100000) && uarmc && itemhasappearance(uarmc, APP_MYSTERIOUS_CLOAK) && !otmpi->rknown) {
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
					u.cnd_unidentifycount++;
				}
				if (!rn2(4000)) {
					otmpi->dknown = FALSE;
					u.cnd_unidentifycount++;
				}
				if (!rn2(4000)) {
					otmpi->rknown = FALSE;
					u.cnd_unidentifycount++;
				}
				if (!rn2(4000)) {
					otmpi->known = FALSE;
					u.cnd_unidentifycount++;
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
					u.cnd_unidentifycount++;
				}
				if (!rn2(4000)) {
					otmpi->dknown = FALSE;
					u.cnd_unidentifycount++;
				}
				if (!rn2(4000)) {
					otmpi->rknown = FALSE;
					u.cnd_unidentifycount++;
				}
				if (!rn2(4000)) {
					otmpi->known = FALSE;
					u.cnd_unidentifycount++;
				}
			    }
			}

		}

		if (uarmh && uarmh->oartifact == ART_YOU_DON_T_KNOW_SHIT) {

			if (invent) {
			    for (otmpi = invent; otmpi; otmpi = otmpii) {
			      otmpii = otmpi->nobj;
	
				if (!rn2(4000)) {
					otmpi->bknown = FALSE;
					u.cnd_unidentifycount++;
				}
				if (!rn2(4000)) {
					otmpi->dknown = FALSE;
					u.cnd_unidentifycount++;
				}
				if (!rn2(4000)) {
					otmpi->rknown = FALSE;
					u.cnd_unidentifycount++;
				}
				if (!rn2(4000)) {
					otmpi->known = FALSE;
					u.cnd_unidentifycount++;
				}
			    }
			}

		}

		if (!rn2(100) && CaptchaProblem) {
			u.captchahack = 1;
			strcpy(buf, rn2(20) ? rndmonnam() : fauxmessage() );
			if ((int) strlen(buf) > 75) buf[75] = '\0';

			pline("Captcha! Please type in the following word(s) to continue: %s", buf);
			getlin("Your input:",ebuf);
			if (((int) strlen(ebuf) != (int) strlen(buf) ) || (strncmpi(buf, ebuf, (int) strlen(ebuf)) != 0)) {
				pline("WRONG! You will be punished. I will paralyze you, slow you and reduce your max HP and Pw.");
				u.cnd_captchafail++;

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
			else {
				pline("Alright. Please move on.");
				u.cnd_captchaamount++;
			}
			u.captchahack = 0;
		}

		if (!rn2(2000)) {
			polyinitors();
			if (Race_if(PM_DESTABILIZER) || Race_if(PM_POLYINITOR)) {
				init_uasmon();
				You("mutate, and your body changes...");
				if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			}
		}

		if (!rn2(100) && QuizTrapEffect) {
			u.captchahack = 1;
			boolean rumoristrue = rn2(2);

			pline("NetHack Quiz! You will now tell me whether the following rumor is true or not!");

			if (rumoristrue) outrumor(1, BY_OTHER, TRUE);
			else outrumor(-1, BY_OTHER, TRUE);

			if (yn("Now tell me if this rumor was true!") != 'y') { /* player said it's false */

				if (rumoristrue) {

					pline("Haha, you guessed wrong! Tough luck, player, it seems you're just not good enough and now there will be a bad effect to punish you, ha-ha!");
					u.cnd_quizfail++;
					badeffect();

				} else {

					pline("Damn it, you guessed correctly! I can't believe it! This rumor was actually false!");
					u.cnd_quizamount++;

				}

			} else { /* player said it's true */

				if (rumoristrue) {

					pline("Oh no, how did you know that this rumor was true? You cheated! You're playing with a spoiler sheet, admit it!");
					u.cnd_quizamount++;

				} else {

					You("really believe everything you read, huh? Well, sucks to be you. This rumor was obviously not true! Everyone except you would've noticed that! Enjoy the punishment.");
					u.cnd_quizfail++;
					badeffect();

				}

			}
			u.captchahack = 0;

		}

		if (!rn2(100) && uarmf && uarmf->oartifact == ART_RIDDLE_ME_THIS) {
			u.captchahack = 1;
			boolean rumoristrue = rn2(2);

			pline("Alright, riddle me this: Is the following rumor true or not?");

			if (rumoristrue) outrumor(1, BY_OTHER, TRUE);
			else outrumor(-1, BY_OTHER, TRUE);

			if (yn("Now tell me if this rumor was true!") != 'y') { /* player said it's false */

				if (rumoristrue) {

					pline("Haha, you guessed wrong! Tough luck, player, it seems you're just not good enough and now there will be a bad effect to punish you, ha-ha!");
					u.cnd_quizfail++;
					badeffect();

				} else {

					pline("Damn it, you guessed correctly! I can't believe it! This rumor was actually false!");
					u.cnd_quizamount++;

				}

			} else { /* player said it's true */

				if (rumoristrue) {

					pline("Oh no, how did you know that this rumor was true? You cheated! You're playing with a spoiler sheet, admit it!");
					u.cnd_quizamount++;

				} else {

					You("really believe everything you read, huh? Well, sucks to be you. This rumor was obviously not true! Everyone except you would've noticed that! Enjoy the punishment.");
					u.cnd_quizfail++;
					badeffect();

				}

			}
			u.captchahack = 0;

		}

		if (!rn2(100) && u.uprops[QUIZZES].extrinsic) {
			u.captchahack = 1;
			boolean rumoristrue = rn2(2);

			pline("NetHack Quiz! You will now tell me whether the following rumor is true or not!");

			if (rumoristrue) outrumor(1, BY_OTHER, TRUE);
			else outrumor(-1, BY_OTHER, TRUE);

			if (yn("Now tell me if this rumor was true!") != 'y') { /* player said it's false */

				if (rumoristrue) {

					pline("Haha, you guessed wrong! Tough luck, player, it seems you're just not good enough and now there will be a bad effect to punish you, ha-ha!");
					u.cnd_quizfail++;
					badeffect();

				} else {

					pline("Damn it, you guessed correctly! I can't believe it! This rumor was actually false!");
					u.cnd_quizamount++;

				}

			} else { /* player said it's true */

				if (rumoristrue) {

					pline("Oh no, how did you know that this rumor was true? You cheated! You're playing with a spoiler sheet, admit it!");
					u.cnd_quizamount++;

				} else {

					You("really believe everything you read, huh? Well, sucks to be you. This rumor was obviously not true! Everyone except you would've noticed that! Enjoy the punishment.");
					u.cnd_quizfail++;
					badeffect();

				}

			}
			u.captchahack = 0;

		}

		if (!rn2(100) && have_quizstone()) {
			u.captchahack = 1;
			boolean rumoristrue = rn2(2);

			pline("NetHack Quiz! You will now tell me whether the following rumor is true or not!");

			if (rumoristrue) outrumor(1, BY_OTHER, TRUE);
			else outrumor(-1, BY_OTHER, TRUE);

			if (yn("Now tell me if this rumor was true!") != 'y') { /* player said it's false */

				if (rumoristrue) {

					pline("Haha, you guessed wrong! Tough luck, player, it seems you're just not good enough and now there will be a bad effect to punish you, ha-ha!");
					u.cnd_quizfail++;
					badeffect();

				} else {

					pline("Damn it, you guessed correctly! I can't believe it! This rumor was actually false!");
					u.cnd_quizamount++;

				}

			} else { /* player said it's true */

				if (rumoristrue) {

					pline("Oh no, how did you know that this rumor was true? You cheated! You're playing with a spoiler sheet, admit it!");
					u.cnd_quizamount++;

				} else {

					You("really believe everything you read, huh? Well, sucks to be you. This rumor was obviously not true! Everyone except you would've noticed that! Enjoy the punishment.");
					u.cnd_quizfail++;
					badeffect();

				}

			}
			u.captchahack = 0;

		}

		if (!rn2(100) && u.uprops[CAPTCHA].extrinsic) {
			u.captchahack = 1;
			strcpy(buf, rn2(20) ? rndmonnam() : fauxmessage() );
			if ((int) strlen(buf) > 75) buf[75] = '\0';

			pline("Captcha! Please type in the following word(s) to continue: %s", buf);
			getlin("Your input:",ebuf);
			if (((int) strlen(ebuf) != (int) strlen(buf) ) || (strncmpi(buf, ebuf, (int) strlen(ebuf)) != 0)) {
				pline("WRONG! You will be punished. I will paralyze you, slow you and reduce your max HP and Pw.");
				u.cnd_captchafail++;

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
			else {
				pline("Alright. Please move on.");
				u.cnd_captchaamount++;
			}
			u.captchahack = 0;
		}

		if (!rn2(100) && have_captchastone()) {
			u.captchahack = 1;
			strcpy(buf, rn2(20) ? rndmonnam() : fauxmessage() );
			if ((int) strlen(buf) > 75) buf[75] = '\0';

			pline("Captcha! Please type in the following word(s) to continue: %s", buf);
			getlin("Your input:",ebuf);
			if (((int) strlen(ebuf) != (int) strlen(buf) ) || (strncmpi(buf, ebuf, (int) strlen(ebuf)) != 0)) {
				pline("WRONG! You will be punished. I will paralyze you, slow you and reduce your max HP and Pw.");
				u.cnd_captchafail++;

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
			else {
				pline("Alright. Please move on.");
				u.cnd_captchaamount++;
			}
			u.captchahack = 0;
		}

		if (Race_if(PM_BATMAN) && u.ualign.record > 0 && (rnd(300000) < u.ualign.record) ) {
			struct obj *ubatarang;
			ubatarang = mksobj(rn2(10) ? BATARANG : DARK_BATARANG, TRUE, FALSE, FALSE);
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
					u.cnd_unidentifycount++;
				}
				if (!rn2(4000)) {
					otmpi->dknown = FALSE;
					u.cnd_unidentifycount++;
				}
				if (!rn2(4000)) {
					otmpi->rknown = FALSE;
					u.cnd_unidentifycount++;
				}
				if (!rn2(4000)) {
					otmpi->known = FALSE;
					u.cnd_unidentifycount++;
				}
			    }
			}

		}

		if (u.uprops[DEHYDRATION].extrinsic || Dehydration || have_dehydratingstone() ) {
			if (u.dehydrationtime < 1) u.dehydrationtime = moves + 1001;
			if ((u.dehydrationtime - moves) == 100) You("are beginning to feel thirsty.");
			if ((u.dehydrationtime - moves) == 0) {
				You("are dehydrated, your vision begins to blur...");
				u.cnd_dehydratedcount++;
			}
		} else {
			u.dehydrationtime = 0;
		}

		if (!rn2(10000) && uarmc && itemhasappearance(uarmc, APP_GHOSTLY_CLOAK) ) {
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

		if (!rn2(1000) && uarmc && itemhasappearance(uarmc, APP_CHILLING_CLOAK) ) {
			make_frozen(HFrozen + rnd(50),TRUE);
		}

		if (!rn2(2000) && uarmc && itemhasappearance(uarmc, APP_HOMICIDAL_CLOAK) ) {
			makerandomtrap(FALSE);
		}

		if (!rn2(2000) && uarmc && itemhasappearance(uarmc, APP_GRAVITY_CLOAK) ) {
			pline("Gravity warps around you...");
			phase_door(0);
			pushplayer(TRUE);
			u.uprops[DEAC_FAST].intrinsic += rnd(10);
			make_stunned(HStun + rnd(10), TRUE);
		}

		if (!rn2(1000) && RngeGravity) {
			pline("Gravity warps around you...");
			phase_door(0);
			pushplayer(TRUE);
			u.uprops[DEAC_FAST].intrinsic += rnd(10);
			make_stunned(HStun + rnd(10), TRUE);
		}

		if (!rn2(1000) && uarmc && itemhasappearance(uarmc, APP_FLASH_CLOAK) ) {
			make_blinded(Blinded + rnd(10), TRUE);
		}

		if (uwep && uwep->oartifact == ART_EVERYTHING_MUST_BURN && !rn2(100)) {

			make_burned(HBurned + rnd(10), TRUE);

		}

		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_EVERYTHING_MUST_BURN && !rn2(100)) {

			make_burned(HBurned + rnd(10), TRUE);

		}

		if (uarmc && itemhasappearance(uarmc, APP_ELECTROSTATIC_CLOAK) ) {
			if (!rn2(500)) {
				You("receive an electric shock from your cloak!");
				make_confused(HConfusion + rnd(10),TRUE);
			}
			if (!rn2(500)) {
				You("receive a static shock from your cloak!");
				make_numbed(HNumbed + rnd(10),TRUE);
			}
		}

		if (uarmc && !rn2(5000) && itemhasappearance(uarmc, APP_IRRADIATION_CLOAK) ) {
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

		if (uarmc && !rn2(2000) && itemhasappearance(uarmc, APP_HUNGRY_CLOAK) ) {
			pline("Suddenly you notice the smell of food...");
			morehungry(rnd(1000));
		}

		if (have_hungerhealer()) {
			morehungry(1);
			if (uactivesymbiosis && Role_if(PM_SYMBIANT)) {
				if (u.usymbiote.mhp < u.usymbiote.mhpmax) {
					u.usymbiote.mhp++;
					if (flags.showsymbiotehp) flags.botl = TRUE;
					if (u.usymbiote.mhp > u.usymbiote.mhpmax) u.usymbiote.mhp = u.usymbiote.mhpmax;
				}
			}
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

		if (uarmh && itemhasappearance(uarmh, APP_BUG_TRACKING_HELMET) && !rn2(10000) ) {
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

		if (uarm && uarm->oartifact == ART_REJUVENATION_BY_GATE && u.inertia > 3) {
			u.inertia /= 2;
		}

		if (u.inertia) {

			/* silk helps against inertia --Amy */
			if (uwep && objects[uwep->otyp].oc_material == MT_SILK && !rn2(12)) {
				u.inertia--;
			}
			if (u.twoweap && uswapwep && objects[uswapwep->otyp].oc_material == MT_SILK && !rn2(12)) {
				u.inertia--;
			}
			if (uarm && objects[uarm->otyp].oc_material == MT_SILK && !rn2(12)) {
				u.inertia--;
			}
			if (uarmc && objects[uarmc->otyp].oc_material == MT_SILK && !rn2(12)) {
				u.inertia--;
			}
			if (uarmh && objects[uarmh->otyp].oc_material == MT_SILK && !rn2(12)) {
				u.inertia--;
			}
			if (uarms && objects[uarms->otyp].oc_material == MT_SILK && !rn2(12)) {
				u.inertia--;
			}
			if (uarmg && objects[uarmg->otyp].oc_material == MT_SILK && !rn2(12)) {
				u.inertia--;
			}
			if (uarmf && objects[uarmf->otyp].oc_material == MT_SILK && !rn2(12)) {
				u.inertia--;
			}
			if (uarmu && objects[uarmu->otyp].oc_material == MT_SILK && !rn2(12)) {
				u.inertia--;
			}
			if (uamul && objects[uamul->otyp].oc_material == MT_SILK && !rn2(12)) {
				u.inertia--;
			}
			if (uimplant && objects[uimplant->otyp].oc_material == MT_SILK && !rn2(12)) {
				u.inertia--;
			}
			if (uleft && objects[uleft->otyp].oc_material == MT_SILK && !rn2(12)) {
				u.inertia--;
			}
			if (uright && objects[uright->otyp].oc_material == MT_SILK && !rn2(12)) {
				u.inertia--;
			}
			if (ublindf && objects[ublindf->otyp].oc_material == MT_SILK && !rn2(12)) {
				u.inertia--;
			}

			if (u.inertia < 0) u.inertia = 0; /* fail safe */

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

		if ((u.uprops[CRAP_EFFECT].extrinsic || (uwep && uwep->oartifact == ART_LUISA_S_CHARMING_BEAUTY) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_LUISA_S_CHARMING_BEAUTY) || (uarmc && uarmc->oartifact == ART_FEMMY_FATALE) || (uwep && uwep->oartifact == ART_GIRLFUL_BONKING) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_GIRLFUL_BONKING) || CrapEffect || (uimplant && uimplant->oartifact == ART_BUCKET_HOUSE) || have_shitstone() || (uarmh && uarmh->oartifact == ART_CLAUDIA_S_SEXY_SCENT) ) && (u.uhs == 0) && !rn2(100) ) {
			You("suddenly have to take a shit!");
			use_skill(P_SQUEAKING, 5);
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

		if (( (u.uhave.amulet && !u.freeplaymode && (u.amuletcompletelyimbued || !rn2(5)) && !rn2(5)) || Clairvoyant) &&
		    !In_endgame(&u.uz) && !BClairvoyant && !rn2(StrongClairvoyant ? 15 : 40) && !rn2(2))
			do_vicinity_map();

		/* farting webs place you at the mercy of whoever is the farting girl */
		if (u.utrap && (ttmp = t_at(u.ux, u.uy)) && ttmp && ttmp->ttyp == FARTING_WEB) {
			fartingweb();
		}		

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

		if (u.utrap && (ttmp = t_at(u.ux, u.uy)) && ttmp && ttmp->ttyp == ACID_PIT) {
			You("are covered with acid!");
		    register struct obj *objX, *objX2;
		    for (objX = invent; objX; objX = objX2) {
		      objX2 = objX->nobj;
			if (!rn2(5)) rust_dmg(objX, xname(objX), 3, TRUE, &youmonst);
		    }

			if (!Acid_resistance || !rn2(StrongAcid_resistance ? 20 : 5)) {
				pline_The("acid inside the pit burns you!");
				losehp((rnd(10) + rnd(monster_difficulty() + 1)), "being stuck in an acid pit", KILLED_BY);
			}
		}
	
		if(u.utrap && u.utraptype == TT_LAVA) {
		    if(!is_lava(u.ux,u.uy))
			u.utrap = 0;
		    else if (!u.uinvulnerable) {
			if (!(uarmc && itemhasappearance(uarmc, APP_VOLCANIC_CLOAK) && rn2(2))) u.utrap -= 1<<8;
			if(u.utrap < 1<<8) {
			    u.youaredead = 1;
			    killer_format = KILLED_BY;
			    killer = "molten lava";
			    You(FunnyHallu ? "dissolve completely, warping to another plane of existence." : "sink below the surface and die.");
			    done(DISSOLVED);
			    u.youaredead = 0;
			} else /*if(didmove && !u.umoved)*/ {
			    /*Norep*/pline(FunnyHallu ? "Your body is dissolving... maybe the Grim Reaper is waiting for you?" : "You sink deeper into the lava.");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			    u.utrap += rnd(4);
			    stop_occupation(); /* BULLSHIT! why doesn't such a thing stop you */
			}
		    }
		}

		    if (flags.bypasses) clear_bypasses();
		    if(IsGlib) glibr();

		/* symbiote HP regeneration - rather slow, but depends on your symbiosis skill and charisma --Amy */
		if (uactivesymbiosis) {
			int symregenrate = 50;
			int symmuchregrate = 500;

			if (ACURR(A_CHA) < 15) {
				symregenrate += ((15 - ACURR(A_CHA)) * 5);
			}
			if (ACURR(A_CHA) > 15) {
				symregenrate -= ACURR(A_CHA);
			}

			if (!(PlayerCannotUseSkills)) {
				switch (P_SKILL(P_SYMBIOSIS)) {
					default: break;
					case P_BASIC:
						symregenrate *= 4;
						symregenrate /= 5;
						symmuchregrate = 450;
						break;
					case P_SKILLED:
						symregenrate *= 3;
						symregenrate /= 5;
						symmuchregrate = 400;
						break;
					case P_EXPERT:
						symregenrate *= 2;
						symregenrate /= 5;
						symmuchregrate = 350;
						break;
					case P_MASTER:
						symregenrate /= 5;
						symmuchregrate = 300;
						break;
					case P_GRAND_MASTER:
						symregenrate /= 6;
						symmuchregrate = 250;
						break;
					case P_SUPREME_MASTER:
						symregenrate /= 8;
						symmuchregrate = 200;
						break;
				}
			}
			if (symregenrate < 3) symregenrate = 3; /* don't fall off the bottom */

			if (Role_if(PM_SYMBIANT)) {
				if (symregenrate > 1) symregenrate /= 2;
				if (symmuchregrate > 1) symmuchregrate /= 2;
			}

			if (regenerates(&mons[u.usymbiote.mnum]) || !rn2(symregenrate)) {
				if (u.usymbiote.mhp < u.usymbiote.mhpmax) {
					u.usymbiote.mhp++;
					if (u.usymbiote.mhp > u.usymbiote.mhpmax) u.usymbiote.mhp = u.usymbiote.mhpmax;
					if (flags.showsymbiotehp) flags.botl = TRUE;
				}
				if (Role_if(PM_SYMBIANT)) {
					if (u.usymbiote.mhp < u.usymbiote.mhpmax) {
						u.usymbiote.mhp++;
						if (u.usymbiote.mhp > u.usymbiote.mhpmax) u.usymbiote.mhp = u.usymbiote.mhpmax;
						if (flags.showsymbiotehp) flags.botl = TRUE;
					}
				}
			}

			/* occasionally regenerate more, but only with high charisma */
			if (!rn2(symmuchregrate) && (rnd(ACURR(A_CHA)) > 14) ) {
				if (rn2(10)) u.usymbiote.mhp += rnd(20 + GushLevel);
				else u.usymbiote.mhp += rnz(20 + GushLevel);
				if (u.usymbiote.mhp > u.usymbiote.mhpmax) u.usymbiote.mhp = u.usymbiote.mhpmax;
				if (flags.showsymbiotehp) flags.botl = TRUE;
			}

		}

		    if (!rn2(2) || !(uarmf && itemhasappearance(uarmf, APP_IRREGULAR_BOOTS) ) ) {

			if (!rn2(2) || !((uleft && uleft->oartifact == ART_GOOD_THINGS_WILL_HAPPEN_EV) || (uright && uright->oartifact == ART_GOOD_THINGS_WILL_HAPPEN_EV)) ) {
				if (!rn2(2) || !RngeIrregularity) {
				    nh_timeout();
				}
			}
		    }

		    run_regions();

#ifdef DUNGEON_GROWTH
		    dgn_growths(TRUE, TRUE, TRUE);
#endif

		if (NonprayerBug || u.uprops[NON_PRAYER_BUG].extrinsic || have_antiprayerstone()) u.ublesscnt++;
		else if (u.ublesscnt) {
			u.ublesscnt--;
		}
		if (u.ublesscnt < 0) u.ublesscnt = 0; /* fail safe */

		if (uarmg && u.ublesscnt && itemhasappearance(uarmg, APP_COMFORTABLE_GLOVES) ) u.ublesscnt--;

		if (u.ublesscnt && RngePrayer) u.ublesscnt--;

		if (u.ublesscnt && Race_if(PM_BOVER)) u.ublesscnt--;

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

                    if ((prev_dgl_extrainfo == 0) || (prev_dgl_extrainfo < (moves + 250))) {
                        prev_dgl_extrainfo = moves;
                        mk_dgl_extrainfo();
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
		    } else if (Upolyd && youmonst.data->mlet == S_EEL && !is_waterypool(u.ux,u.uy) && !is_watertunnel(u.ux,u.uy) && !is_shiftingsand(u.ux,u.uy) && !is_crystalwater(u.ux,u.uy) && !Is_waterlevel(&u.uz)) {
			if (u.mh > 1) {
			    u.mh--;
			    flags.botl = 1;
			} else if (u.mh < 1)
			    rehumanize();
		    } else if (Upolyd && u.mh < u.mhmax && (rn2(2) || (!sengr_at("Elbereth", u.ux, u.uy) ) ) ) {

			/* faster regeneration --Amy */
			regenrate = (20 - (GushLevel / 3));
			if (regenrate < 6) regenrate = 6;
			if (Race_if(PM_HAXOR)) regenrate /= 2;
			if (is_grassland(u.ux, u.uy)) regenrate *= 2;

			if (u.mh < 1)
			    rehumanize();
			else if (Regeneration ||
				    (wtcap < MOD_ENCUMBER && !(moves%/*20*/regenrate))) {
			    flags.botl = 1;
				if (!Burned && !Race_if(PM_ETHEREALOID) && !PlayerBleeds && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) ) {
					u.mh++;
					if (Race_if(PM_PIERCER)) u.mh++;
					if (u.mh > u.mhmax) u.mh = u.mhmax;
				}
				if (StrongRegeneration && !Burned && !Race_if(PM_ETHEREALOID) && !PlayerBleeds && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) ) {
					u.mh++;
					if (Race_if(PM_PIERCER)) u.mh++;
					if (u.mh > u.mhmax) u.mh = u.mhmax;
				}
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
 			int efflev = rnd(GushLevel) + (u.uhealbonus);
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

			} /* player cannot use skills */

			if (u.usteed) {
				struct obj *osaeddle = which_armor(u.usteed, W_SADDLE);

				if ((osaeddle = which_armor(u.usteed, W_SADDLE)) && osaeddle->oartifact == ART_CURE_HASSIA_COURSE) {
					effcon += 5;
					efflev += 5;
				}

			}

			if (isfriday) {
				if (effcon > 1) effcon /= 2;
				if (efflev > 1) efflev /= 2;
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
			    if (!Burned && !Race_if(PM_ETHEREALOID) && !PlayerBleeds && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) ) {
					u.uhp += heal;
					if (Race_if(PM_PIERCER)) u.uhp += heal;
					if(u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			    }
			} else if (Regeneration ||
			     (efflev <= 9 &&
			      !(moves % ((MAXULEV+12) / (GushLevel+2) + 1)))) {
			    flags.botl = 1;
			    if (!Burned && !Race_if(PM_ETHEREALOID) && !PlayerBleeds && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) ) {
					u.uhp++;
					if (Race_if(PM_PIERCER)) u.uhp++;
					if(u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				}
			    if (StrongRegeneration && !Burned && !Race_if(PM_ETHEREALOID) && !PlayerBleeds && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) ) {
					u.uhp++;
					if (Race_if(PM_PIERCER)) u.uhp++;
					if(u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				}
			}

			if (Race_if(PM_BACTERIA) && u.uhpmax > 4 && !Upolyd && u.uhp <= ((u.uhpmax / 5) + 1)) {
				pline("*Super Regene*");
				u.uhp += 5;
				if(u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				flags.botl = 1;
			}

		    }

			if (!Burned && !Race_if(PM_ETHEREALOID) && !PlayerBleeds && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && Race_if(PM_HAXOR) && !rn2(20) && (rn2(2) || (!sengr_at("Elbereth", u.ux, u.uy) ) ) ) {
				u.uhp += rnd(5 + (GushLevel / 5));
				if (Race_if(PM_PIERCER)) u.uhp += rnd(5 + (GushLevel / 5));
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				flags.botl = 1;
			}
			if (!Burned && !Race_if(PM_ETHEREALOID) && !PlayerBleeds && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && Race_if(PM_HAXOR) && Upolyd && !rn2(20) && (rn2(2) || (!sengr_at("Elbereth", u.ux, u.uy) ) ) ) {
				u.mh += rnd(5 + (GushLevel / 5));
				if (Race_if(PM_PIERCER)) u.mh += rnd(5 + (GushLevel / 5));
				if (u.mh > u.mhmax) u.mh = u.mhmax;
				flags.botl = 1;
			}
			if (!Burned && !Race_if(PM_ETHEREALOID) && !PlayerBleeds && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH)) && !rn2(60 / GushLevel) ) {
				u.uhp++;
				if (Race_if(PM_PIERCER)) u.uhp++;
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				flags.botl = 1;
			}
			if (!Burned && !Race_if(PM_ETHEREALOID) && !PlayerBleeds && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH)) && !rn2(60 / GushLevel) && Upolyd ) {
				u.mh++;
				if (Race_if(PM_PIERCER)) u.mh++;
				if (u.mh > u.mhmax) u.mh = u.mhmax;
				flags.botl = 1;
			}

			/* nice patch addition by Amy - sometimes regenerate more */
			if (!Burned && !Race_if(PM_ETHEREALOID) && !PlayerBleeds && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && !rn2(150) && (rn2(2) || (!sengr_at("Elbereth", u.ux, u.uy) ) ) ) {
				u.uhp += rnz(2 + GushLevel);
				if (Race_if(PM_PIERCER)) u.uhp += rnz(2 + GushLevel);
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				flags.botl = 1;
			}
			if (!Burned && !Race_if(PM_ETHEREALOID) && !PlayerBleeds && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && !rn2(150) && Upolyd && (rn2(2) || (!sengr_at("Elbereth", u.ux, u.uy) ) ) ) {
				u.mh += rnz(2 + GushLevel);
				if (Race_if(PM_PIERCER)) u.mh += rnz(2 + GushLevel);
				if (u.mh > u.mhmax) u.mh = u.mhmax;
				flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && Race_if(PM_HAXOR) && !rn2(20) ) {
				u.uen += rnd(5 + (GushLevel / 5));
				if (Race_if(PM_PIERCER)) u.uen += rnd(5 + (GushLevel / 5));
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
				You(FunnyHallu ? "are too trippy to stand on two legs, so you fall down." : "pass out from exertion!");
				exercise(A_CON, FALSE);
				fall_asleep(-10, FALSE);
			    }
			}
		    }

		    
		    /* KMH -- OK to regenerate if you don't move */
		    if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && (recalc_mana() >= 0 || (!rn2(-(recalc_mana() - 1) ) ) ) && (u.uen < u.uenmax) && 
				((Energy_regeneration && !rn2(StrongEnergy_regeneration ? 2 : 3)) || /* greatly nerfed overpowered wizard artifact --Amy */
				(Role_if(PM_ALTMER) && !rn2(5)) || /* altmer have extra mana regeneration --Amy */
				((wtcap < MOD_ENCUMBER || !flags.mv) &&
				(!(moves%((MAXULEV + 15 - GushLevel) *                                    
				(Role_if(PM_WIZARD) ? 3 : 4) / 6)))))) {
			u.uen += rn1((int)(ACURR(A_WIS) + ACURR(A_INT)) / 15 + 1,1);
			if (Race_if(PM_PIERCER)) u.uen += rn1((int)(ACURR(A_WIS) + ACURR(A_INT)) / 15 + 1,1);

#ifdef WIZ_PATCH_DEBUG
                pline("mana was = %d now = %d",temp,u.uen);
#endif

			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
		    }

			/* nice patch addition by Amy - sometimes regenerate more */
			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && !issoviet && (rn2(2) || !Race_if(PM_SYLPH) ) && !rn2(250) && (u.uen < u.uenmax)) {

				u.uen += rnz(2 + GushLevel);
				if (Race_if(PM_PIERCER)) u.uen += rnz(2 + GushLevel);
				if (u.uen > u.uenmax)  u.uen = u.uenmax;
				flags.botl = 1;

			}
			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && !issoviet && (rn2(2) || !Race_if(PM_SYLPH) ) && !rn2(250) && (u.uen < u.uenmax) && Energy_regeneration) {

				u.uen += rnz(2 + GushLevel);
				if (Race_if(PM_PIERCER)) u.uen += rnz(2 + GushLevel);
				if (StrongEnergy_regeneration) u.uen += rnz(2 + GushLevel);
				if (Race_if(PM_PIERCER) && StrongEnergy_regeneration) u.uen += rnz(2 + GushLevel);
				if (u.uen > u.uenmax)  u.uen = u.uenmax;
				flags.botl = 1;

			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && !issoviet && (rn2(2) || !Race_if(PM_SYLPH) ) && !rn2(50) && (u.uen < u.uenmax) && recalc_mana() > 0) {

				u.uen += rnd(recalc_mana());
				if (Race_if(PM_PIERCER)) u.uen += rnd(recalc_mana());
				if (u.uen > u.uenmax)  u.uen = u.uenmax;
				flags.botl = 1;

			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && Race_if(PM_RODNEYAN)) { /* rodney has special built-in energy regeneration --Amy */
				u.uen++;
				if (Race_if(PM_PIERCER)) u.uen++;
				if (u.uen > u.uenmax)  u.uen = u.uenmax;
				flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !rn2(3) && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && Role_if(PM_PSYKER)) { /* psyker has special built-in energy regeneration --Amy */
				u.uen++;
				if (Race_if(PM_PIERCER)) u.uen++;
				if (u.uen > u.uenmax)  u.uen = u.uenmax;
				flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && !issoviet && (rn2(2) || !Race_if(PM_SYLPH)) && !rn2(90 / GushLevel) ) {
				u.uen++;
				if (Race_if(PM_PIERCER)) u.uen++;
				if (u.uen > u.uenmax)  u.uen = u.uenmax;
				flags.botl = 1;
			}

		/* leveling up will give a small boost to mana regeneration now --Amy */
		    if ( !Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && !issoviet && (rn2(2) || !Race_if(PM_SYLPH) ) && u.uen < u.uenmax && ( 
			(GushLevel >= 5 && !rn2(200)) ||
			(GushLevel >= 10 && !rn2(100)) ||
			(GushLevel >= 14 && !rn2(100)) ||
			(GushLevel >= 15 && !rn2(50)) ||
			(GushLevel >= 20 && !rn2(30)) ||
			(GushLevel >= 25 && !rn2(20)) ||
			(GushLevel >= 30 && !rn2(10)) ||
			(u.menoraget && !rn2(200)) ||
			(u.bookofthedeadget && !rn2(200)) ||
			(u.silverbellget && !rn2(200)) ||
			(u.chaoskeyget && !rn2(500)) ||
			(u.neutralkeyget && !rn2(500)) ||
			(u.lawfulkeyget && !rn2(500)) ||
			(u.medusaremoved && !rn2(250)) ||
			(u.luckstoneget && !rn2(1000)) ||
			(u.deepminefinished && !rn2(1000)) ||
			(u.sokobanfinished && !rn2(1000)) ) ) {
				u.uen += 1;
				if (Race_if(PM_PIERCER)) u.uen++;
				if (u.uen > u.uenmax)  u.uen = u.uenmax;
				flags.botl = 1;
			}

			/* Having a spell school at skilled will improve mana regeneration.
			 * Having a spell school at expert will improve it by even more. --Amy */

			if (!(issoviet || PlayerCannotUseSkills)) {

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ATTACK_SPELL) == P_SKILLED && !rn2(200)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ATTACK_SPELL) == P_EXPERT && !rn2(100)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ATTACK_SPELL) == P_MASTER && !rn2(50)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ATTACK_SPELL) == P_GRAND_MASTER && !rn2(25)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ATTACK_SPELL) == P_SUPREME_MASTER && !rn2(15)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_DIVINATION_SPELL) == P_SKILLED && !rn2(200)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_DIVINATION_SPELL) == P_EXPERT && !rn2(100)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_DIVINATION_SPELL) == P_MASTER && !rn2(50)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_DIVINATION_SPELL) == P_GRAND_MASTER && !rn2(25)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_DIVINATION_SPELL) == P_SUPREME_MASTER && !rn2(15)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_MATTER_SPELL) == P_SKILLED && !rn2(200)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_MATTER_SPELL) == P_EXPERT && !rn2(100)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_MATTER_SPELL) == P_MASTER && !rn2(50)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_MATTER_SPELL) == P_GRAND_MASTER && !rn2(25)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_MATTER_SPELL) == P_SUPREME_MASTER && !rn2(15)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_OCCULT_SPELL) == P_SKILLED && !rn2(200)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_OCCULT_SPELL) == P_EXPERT && !rn2(100)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_OCCULT_SPELL) == P_MASTER && !rn2(50)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_OCCULT_SPELL) == P_GRAND_MASTER && !rn2(25)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_OCCULT_SPELL) == P_SUPREME_MASTER && !rn2(15)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ELEMENTAL_SPELL) == P_SKILLED && !rn2(200)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ELEMENTAL_SPELL) == P_EXPERT && !rn2(100)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ELEMENTAL_SPELL) == P_MASTER && !rn2(50)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ELEMENTAL_SPELL) == P_GRAND_MASTER && !rn2(25)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ELEMENTAL_SPELL) == P_SUPREME_MASTER && !rn2(15)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_CHAOS_SPELL) == P_SKILLED && !rn2(200)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_CHAOS_SPELL) == P_EXPERT && !rn2(100)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_CHAOS_SPELL) == P_MASTER && !rn2(50)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_CHAOS_SPELL) == P_GRAND_MASTER && !rn2(25)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_CHAOS_SPELL) == P_SUPREME_MASTER && !rn2(15)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_BODY_SPELL) == P_SKILLED && !rn2(200)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_BODY_SPELL) == P_EXPERT && !rn2(100)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_BODY_SPELL) == P_MASTER && !rn2(50)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_BODY_SPELL) == P_GRAND_MASTER && !rn2(25)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_BODY_SPELL) == P_SUPREME_MASTER && !rn2(15)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_PROTECTION_SPELL) == P_SKILLED && !rn2(200)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_PROTECTION_SPELL) == P_EXPERT && !rn2(100)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_PROTECTION_SPELL) == P_MASTER && !rn2(50)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_PROTECTION_SPELL) == P_GRAND_MASTER && !rn2(25)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_PROTECTION_SPELL) == P_SUPREME_MASTER && !rn2(15)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ENCHANTMENT_SPELL) == P_SKILLED && !rn2(200)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ENCHANTMENT_SPELL) == P_EXPERT && !rn2(100)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ENCHANTMENT_SPELL) == P_MASTER && !rn2(50)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ENCHANTMENT_SPELL) == P_GRAND_MASTER && !rn2(25)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_ENCHANTMENT_SPELL) == P_SUPREME_MASTER && !rn2(15)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_HEALING_SPELL) == P_SKILLED && !rn2(200)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_HEALING_SPELL) == P_EXPERT && !rn2(100)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_HEALING_SPELL) == P_MASTER && !rn2(50)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_HEALING_SPELL) == P_GRAND_MASTER && !rn2(25)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

			if (!Burned && !contaminationcheck() && !(Race_if(PM_PLAYER_GREMLIN) && levl[u.ux][u.uy].lit) && (rn2(2) || !Race_if(PM_SYLPH) ) && P_SKILL(P_HEALING_SPELL) == P_SUPREME_MASTER && !rn2(15)) {
			u.uen += 1;
			if (Race_if(PM_PIERCER)) u.uen += 1;
			if (u.uen > u.uenmax)  u.uen = u.uenmax;
			flags.botl = 1;
			}

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
				pline("%s", fauxmessage());
				u.cnd_plineamount++;
				if (!rn2(3)) {
					pline("%s", fauxmessage());
					u.cnd_plineamount++;
				}
			}

			if (u.stairscumslowing) {
				u.stairscumslowing--;
				if (u.stairscumslowing < 0) u.stairscumslowing = 0;
			}

		    if(!u.uinvulnerable) {
			if(Teleportation && (ishaxor ? !rn2(150) : !rn2(250)) ) {
			    xchar old_ux = u.ux, old_uy = u.uy;
				You(FunnyHallu ? "open a warp gate!" : "suddenly get teleported!");
			    tele();
				if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			    if (u.ux != old_ux || u.uy != old_uy) {
				if (!next_to_u()) {
				    check_leash(old_ux, old_uy);
				}
				/* clear doagain keystrokes */
				pushch(0);
				savech(0);
			    }
			}

			if(StrongTeleportation && (ishaxor ? !rn2(150) : !rn2(250)) ) {
			    xchar old_ux = u.ux, old_uy = u.uy;
				You(FunnyHallu ? "open a warp gate!" : "suddenly get teleported!");
			    tele();
				if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			    if (u.ux != old_ux || u.uy != old_uy) {
				if (!next_to_u()) {
				    check_leash(old_ux, old_uy);
				}
				/* clear doagain keystrokes */
				pushch(0);
				savech(0);
			    }
			}

			if (tech_inuse(T_REFUGE)) {

				register struct monst *refmon, *refmon2;

				for(refmon = fmon; refmon; refmon = refmon2) {
				    refmon2 = refmon->nmon;
				if (DEADMONSTER(refmon)) continue;
				if (distu(refmon->mx,refmon->my) > 25) continue;

				if (!refmon->mpeaceful && (is_undead(refmon->data) || refmon->egotype_undead)) {
					if (!rn2(10) && !refmon->mflee) {
						refmon->mflee = 1;
						if (cansee(refmon->mx,refmon->my)) pline("%s turns to flee!", Monnam(refmon));
					}
					refmon->mhp -= rnd(6 + techlevX(get_tech_no(T_REFUGE))); /* ideally this should be made to scale with techlevel --Amy */
					if (cansee(refmon->mx,refmon->my)) pline("%s is affected by your refuge!", Monnam(refmon));
					if (refmon->mhp < 1) killed(refmon);
				} /* monster can be affected check */

				} /* for check */

			}

			if (tech_inuse(T_GLOWHORN)) {
				use_unicorn_horn((struct obj *)0);
			}

			if (powerfulimplants() && uimplant && uimplant->oartifact == ART_HEALENERATION) {
				use_unicorn_horn((struct obj *)0);
			}

			if (Race_if(PM_RODNEYAN) && !rn2(1000)) {	/* levelteleportitis --Amy */

				make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */
				pline("A mysterious force surrounds you...");
			      if (!flags.lostsoul && !flags.uberlostsoul && !(flags.wonderland && !(u.wonderlandescape)) && !(iszapem && !(u.zapemescape)) && !(u.uprops[STORM_HELM].extrinsic) && !(In_bellcaves(&u.uz)) && !(In_subquest(&u.uz)) && !(In_voiddungeon(&u.uz)) && !(In_netherrealm(&u.uz)) ) level_tele();
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

			if (Role_if(PM_CELLAR_CHILD) && !rn2(20000)) {

				if (((u.uevent.udemigod || u.uhave.amulet) && !u.freeplaymode) || CannotTeleport || (u.usteed && mon_has_amulet(u.usteed))) {
					goto cellarnope;
				}

				if (flags.lostsoul || flags.uberlostsoul || (flags.wonderland && !(u.wonderlandescape)) || (iszapem && !(u.zapemescape)) || u.uprops[STORM_HELM].extrinsic || In_bellcaves(&u.uz) || In_subquest(&u.uz) || In_voiddungeon(&u.uz) || In_netherrealm(&u.uz)) { 
					goto cellarnope;
				}

				make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */
				pline("A mysterious force surrounds you...");
			      level_tele();

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
cellarnope:
			if (!rn2(10000) && uarmc && itemhasappearance(uarmc, APP_CHINESE_CLOAK) ) {

				if (((u.uevent.udemigod || u.uhave.amulet) && !u.freeplaymode) || CannotTeleport || (u.usteed && mon_has_amulet(u.usteed))) {
					NastinessProblem += rnd(1000);
					You("can hear Arabella giggling.");
					goto past1;
				}

				if (flags.lostsoul || flags.uberlostsoul || (flags.wonderland && !(u.wonderlandescape)) || (iszapem && !(u.zapemescape)) || u.uprops[STORM_HELM].extrinsic || In_bellcaves(&u.uz) || In_subquest(&u.uz) || In_voiddungeon(&u.uz) || In_netherrealm(&u.uz)) { 
					NastinessProblem += rnd(1000);
					You("can hear Arabella announce: 'Sorry, but the time of your demise is drawing near.'");
					goto past1;
				}

				make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

				u.cnd_banishmentcount++;
				if (rn2(2)) {(void) safe_teleds(FALSE); goto_level(&medusa_level, TRUE, FALSE, FALSE); }
				else {(void) safe_teleds(FALSE); goto_level(&portal_level, TRUE, FALSE, FALSE); }

				register int newlev = rnd(99);
				d_level newlevel;
				get_level(&newlevel, newlev);
				goto_level(&newlevel, TRUE, FALSE, FALSE);
				You("were banished!");

			}
past1:
			if (!rn2(10000) && RngeChina) {

				if (((u.uevent.udemigod || u.uhave.amulet) && !u.freeplaymode) || CannotTeleport || (u.usteed && mon_has_amulet(u.usteed))) {
					NastinessProblem += rnd(1000);
					You("can hear Arabella giggling.");
					goto past2;
				}

				if (flags.lostsoul || flags.uberlostsoul || (flags.wonderland && !(u.wonderlandescape)) || (iszapem && !(u.zapemescape)) || u.uprops[STORM_HELM].extrinsic || In_bellcaves(&u.uz) || In_subquest(&u.uz) || In_voiddungeon(&u.uz) || In_netherrealm(&u.uz)) { 
					NastinessProblem += rnd(1000);
					You("can hear Arabella announce: 'Sorry, but the time of your demise is drawing near.'");
					goto past2;
				}

				make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

				u.cnd_banishmentcount++;
				if (rn2(2)) {(void) safe_teleds(FALSE); goto_level(&medusa_level, TRUE, FALSE, FALSE); }
				else {(void) safe_teleds(FALSE); goto_level(&portal_level, TRUE, FALSE, FALSE); }

				register int newlev = rnd(99);
				d_level newlevel;
				get_level(&newlevel, newlev);
				goto_level(&newlevel, TRUE, FALSE, FALSE);
				You("were banished!");

			}
past2:
			if (!rn2(10000) && uarmc && uarmc->oartifact == ART_ARABELLA_S_LIGHTNINGROD) {

				if (((u.uevent.udemigod || u.uhave.amulet) && !u.freeplaymode) || CannotTeleport || (u.usteed && mon_has_amulet(u.usteed))) {
					NastinessProblem += rnd(1000);
					You("can hear Arabella giggling.");
					goto past3;
				}

				if (flags.lostsoul || flags.uberlostsoul || (flags.wonderland && !(u.wonderlandescape)) || (iszapem && !(u.zapemescape)) || u.uprops[STORM_HELM].extrinsic || In_bellcaves(&u.uz) || In_subquest(&u.uz) || In_voiddungeon(&u.uz) || In_netherrealm(&u.uz)) { 
					NastinessProblem += rnd(1000);
					You("can hear Arabella announce: 'Sorry, but the time of your demise is drawing near.'");
					goto past3;
				}

				make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

				u.cnd_banishmentcount++;
				if (rn2(2)) {(void) safe_teleds(FALSE); goto_level(&medusa_level, TRUE, FALSE, FALSE); }
				else {(void) safe_teleds(FALSE); goto_level(&portal_level, TRUE, FALSE, FALSE); }

				register int newlev = rnd(99);
				d_level newlevel;
				get_level(&newlevel, newlev);
				goto_level(&newlevel, TRUE, FALSE, FALSE);
				You("were banished!");

			}
past3:
			if (!rn2(10000) && uarmc && itemhasappearance(uarmc, APP_POLYFORM_CLOAK) ) {
				if (!HPolymorph_control) HPolymorph_control = 2;
				You_feel("polyform.");
				if (!Unchanging) polyself(FALSE);
			}

			if (!rn2(10000) && uarmc && itemhasappearance(uarmc, APP_CONTAMINATED_COAT)) {
				if (IntSick_resistance || (ExtSick_resistance && rn2(20)) ) {
					You_feel("a slight illness.");
				} else {
					make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON), 40),
				"contaminated coat", TRUE, SICK_NONVOMITABLE);
				}
			    stop_occupation();

			}

			if (!rn2(2000) && RngeSickness) {
				if (IntSick_resistance || (ExtSick_resistance && rn2(20)) ) {
					You_feel("a slight illness.");
				} else {
					make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON), 40),
				"cursed sickness", TRUE, SICK_NONVOMITABLE);
				}
			    stop_occupation();

			}

			if (uarmh && !rn2(1000) && itemhasappearance(uarmh, APP_WEEPING_HELMET) ) {

				make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */
				pline("A mysterious force surrounds you...");
			      if (!flags.lostsoul && !flags.uberlostsoul && !(flags.wonderland && !(u.wonderlandescape)) && !(iszapem && !(u.zapemescape)) && !(u.uprops[STORM_HELM].extrinsic) && !(In_bellcaves(&u.uz)) && !(In_subquest(&u.uz)) && !(In_voiddungeon(&u.uz)) && !(In_netherrealm(&u.uz))) level_tele();
				else You_feel("very disoriented but decide to move on.");

			}

			if (RngeWeeping && !rn2(1000)) {

				make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */
				pline("A mysterious force surrounds you...");
			      if (!flags.lostsoul && !flags.uberlostsoul && !(flags.wonderland && !(u.wonderlandescape)) && !(iszapem && !(u.zapemescape)) && !(u.uprops[STORM_HELM].extrinsic) && !(In_bellcaves(&u.uz)) && !(In_subquest(&u.uz)) && !(In_voiddungeon(&u.uz)) && !(In_netherrealm(&u.uz))) level_tele();
				else You_feel("very disoriented but decide to move on.");

			}

			if (uarmg && uarmg->oartifact == ART_ARABELLA_S_BANK_OF_CROSSRO && !rn2(2000) ) {

				make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */
				pline("A mysterious force surrounds you...");
			      if (!flags.lostsoul && !flags.uberlostsoul && !(flags.wonderland && !(u.wonderlandescape)) && !(iszapem && !(u.zapemescape)) && !(u.uprops[STORM_HELM].extrinsic) && !(In_bellcaves(&u.uz)) && !(In_subquest(&u.uz)) && !(In_voiddungeon(&u.uz)) && !(In_netherrealm(&u.uz))) level_tele();
				else You_feel("very disoriented but decide to move on.");

			}

			if (uarmg && uarmg->oartifact == ART_ARABELLA_S_GREAT_BANISHER && !rn2(2000) ) {

				make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */
				pline("A mysterious force surrounds you...");
			      if (!flags.lostsoul && !flags.uberlostsoul && !(flags.wonderland && !(u.wonderlandescape)) && !(iszapem && !(u.zapemescape)) && !(u.uprops[STORM_HELM].extrinsic) && !(In_bellcaves(&u.uz)) && !(In_subquest(&u.uz)) && !(In_voiddungeon(&u.uz)) && !(In_netherrealm(&u.uz))) level_tele();
				else You_feel("very disoriented but decide to move on.");

			}

			/* the uberquasit REALLY doesn't want you to ride it --Amy */
			if (u.usteed && u.usteed->mnum == PM_ULTRA_EVIL_QUASIT) {
				dismount_steed(DISMOUNT_FELL);
			}

			if (u.usteed && !rn2(5000) ) {

				struct obj *osaeddle = which_armor(u.usteed, W_SADDLE);

				if ((osaeddle = which_armor(u.usteed, W_SADDLE)) && osaeddle->oartifact == ART_WESTERN_FRANKISH_COURSE) {
					pline("A mysterious force surrounds you...");
					HTeleport_control++;
				      if (!flags.lostsoul && !flags.uberlostsoul && !(flags.wonderland && !(u.wonderlandescape)) && !(iszapem && !(u.zapemescape)) && !(u.uprops[STORM_HELM].extrinsic) && !(In_bellcaves(&u.uz)) && !(In_subquest(&u.uz)) && !(In_voiddungeon(&u.uz)) && !(In_netherrealm(&u.uz))) level_tele();
					else You_feel("very disoriented but decide to move on.");
				}

			}

			if (u.usteed && !rn2(200) ) {

				struct obj *osaeddle = which_armor(u.usteed, W_SADDLE);

				if ((osaeddle = which_armor(u.usteed, W_SADDLE)) && osaeddle->oartifact == ART_WESTERN_FRANKISH_COURSE) {
					xchar old_ux = u.ux, old_uy = u.uy;
					You(FunnyHallu ? "open a warp gate!" : "suddenly get teleported!");
					HTeleport_control++;
					tele();
					if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
					if (u.ux != old_ux || u.uy != old_uy) {
					if (!next_to_u()) {
						check_leash(old_ux, old_uy);
					}
					/* clear doagain keystrokes */
					pushch(0);
					savech(0);
					}

				}

			}

			if (uwep && uwep->oartifact == ART_RAFSCHAR_S_SUPERWEAPON && !rn2(2000) ) {

				make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */
				pline("A mysterious force surrounds you...");

				if ((((u.uevent.udemigod || u.uhave.amulet) && !u.freeplaymode) || In_endgame(&u.uz) || (Role_if(PM_CAMPERSTRIKER) && In_quest(&u.uz)) || (u.usteed && mon_has_amulet(u.usteed)) ) ) {
					u.datadeletedefer = 1;
					datadeleteattack();
				}
				else if (!flags.lostsoul && !flags.uberlostsoul && !(flags.wonderland && !(u.wonderlandescape)) && !(iszapem && !(u.zapemescape)) && !(u.uprops[STORM_HELM].extrinsic) && !(In_bellcaves(&u.uz)) && !(In_subquest(&u.uz)) && !(In_voiddungeon(&u.uz)) && !(In_netherrealm(&u.uz))) {

					make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

					u.cnd_banishmentcount++;
					if (rn2(2)) {(void) safe_teleds(FALSE); goto_level(&medusa_level, TRUE, FALSE, FALSE); }
					else {(void) safe_teleds(FALSE); goto_level(&portal_level, TRUE, FALSE, FALSE); }

					register int newlev = rnd(99);
					d_level newlevel;
					get_level(&newlevel, newlev);
					goto_level(&newlevel, TRUE, FALSE, FALSE);
					You("were banished!");

				}
				else {
					u.datadeletedefer = 1;
					datadeleteattack();
				}

			}

			if (uswapwep && uswapwep->oartifact == ART_RAFSCHAR_S_SUPERWEAPON && !rn2(2000) ) {

				make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */
				pline("A mysterious force surrounds you...");

				if ((((u.uevent.udemigod || u.uhave.amulet) && !u.freeplaymode) || In_endgame(&u.uz) || (Role_if(PM_CAMPERSTRIKER) && In_quest(&u.uz)) || (u.usteed && mon_has_amulet(u.usteed)) ) ) {
					u.datadeletedefer = 1;
					datadeleteattack();
				}
				else if (!flags.lostsoul && !flags.uberlostsoul && !(flags.wonderland && !(u.wonderlandescape)) && !(iszapem && !(u.zapemescape)) && !(u.uprops[STORM_HELM].extrinsic) && !(In_bellcaves(&u.uz)) && !(In_subquest(&u.uz)) && !(In_voiddungeon(&u.uz)) && !(In_netherrealm(&u.uz))) {

					make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

					u.cnd_banishmentcount++;
					if (rn2(2)) {(void) safe_teleds(FALSE); goto_level(&medusa_level, TRUE, FALSE, FALSE); }
					else {(void) safe_teleds(FALSE); goto_level(&portal_level, TRUE, FALSE, FALSE); }

					register int newlev = rnd(99);
					d_level newlevel;
					get_level(&newlevel, newlev);
					goto_level(&newlevel, TRUE, FALSE, FALSE);
					You("were banished!");

				}

				else {
					u.datadeletedefer = 1;
					datadeleteattack();
				}

			}

			if ((WereformBug || u.uprops[WEREFORM_BUG].extrinsic || have_wereformstone() || (uarmf && uarmf->oartifact == ART_USE_THE_NORMALNESS_TURNS)) && !rn2(1200 - (200 * night()))) {

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
			else if(StrongPolymorph && (ishaxor ? !rn2(500) : !rn2(1000)) )
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

		if(Searching && multi >= 0 && (!Role_if(PM_CAMPERSTRIKER) || !rn2(3) ) ) {
				if (StrongSearching && rn2(5)) (void) dosearch0(1);
				else if (!StrongSearching && !rn2(3)) (void) dosearch0(1);
		}

		    dosounds();
		    do_storms();
		    gethungry();
		    age_spells();
		    exerchk();
		    invault();
		    if (u.uhave.amulet && !u.freeplaymode) amulet();

		if (!rn2(40+(int)(ACURR(A_DEX)*3))) u_wipe_engr(rnd(3));
		    if ((u.uevent.udemigod && !u.freeplaymode && !rn2(5) && (u.amuletcompletelyimbued || !rn2(10))) && !u.uinvulnerable) {
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

	u.captchahack = 0;
	u.roommatehack = 0;
	u.mushroompoleused = 0;
	u.explodewandhack = 0;
	u.symbiotedmghack = FALSE;

	u.dungeongrowthhack = 0; /* should always be 0 except during saving and loading */

	/* etherealoid should have xray vision; doesn't stack with artifacts */
	if (Race_if(PM_ETHEREALOID)) u.xray_range = 3;

	if (Upolyd && youmonst.data == &mons[PM_SLITHER]) { /* laaaaaaaaaag! :D --Amy */
		int lagamount = rno(10);
		while (lagamount > 0) {
			delay_output();
			lagamount--;
		}
	}

	/* depending on the player's speed, you may go back and forth and still end up on the same square when the next
	 * check on the natalje trap effect happens; make sure to catch those moves and reset the timer --Amy */
	if (FemtrapActiveNatalje) {
		if ((u.ux != u.nataljetrapx) || (u.uy != u.nataljetrapy)) {
			u.nataljetrapturns = moves;
			u.nataljetrapx = u.ux;
			u.nataljetrapy = u.uy;
		}
	}

	if (practicantterror) {
		if (u.uconduct.killer >= 1000 && !u.pract_toomanykills) {
			pline("%s booms: 'You killed too many monsters, you maggot. That's a fine of 1000 zorkmids.'", noroelaname());
			fineforpracticant(1000, 0, 0);
			u.pract_toomanykills = TRUE;
		}
		if (u.uconduct.killer >= 2000 && !u.pract_toomanykills2) {
			pline("%s booms: 'You maggot again killed too many monsters, and since you did that once already, it costs 2000 zorkmids now.'", noroelaname());
			fineforpracticant(2000, 0, 0);
			u.pract_toomanykills2 = TRUE;
		}
		if (u.uconduct.killer >= 3000 && !u.pract_toomanykills3) {
			pline("%s thunders: 'You don't get it, huh? Well, no matter, now it costs 4000 zorkmids. Soon I can afford a Porsche.'", noroelaname());
			fineforpracticant(4000, 0, 0);
			u.pract_toomanykills3 = TRUE;
		}
		if (u.uconduct.killer >= 4000 && !u.pract_toomanykills4) {
			pline("%s thunders: 'Now there's a conventional penalty for you!'", noroelaname());
			u.pract_conv1timer = 5000;
			u.pract_toomanykills4 = TRUE;
		}
		if (u.ugold >= 10000 && !u.pract_toomuchmoney) {
			pline("%s rings out: 'You didn't obey the rule that you may not enter here with more than 10000 zorkmids! Now you have to pay half of them!'", noroelaname());
			fineforpracticant(5000, 0, 0);
			u.pract_toomuchmoney = TRUE;
		}
		if (u.cnd_offercount >= 10 && !u.pract_toomanysacs) {
			pline("%s booms: 'You sacrificed a corpse without permission for the tenth time in a row and therefore have to pay a fine of 1000 zorkmids to me immediately.'", noroelaname());
			fineforpracticant(1000, 0, 0);
			u.pract_toomanysacs = TRUE;
		}
		if (u.cnd_offercount >= 20 && !u.pract_toomanysacs2) {
			pline("%s thunders: 'Since you still don't obey the rule that you may not sacrifice corpses, you now pay 2000 zorkmids to me!'", noroelaname());
			fineforpracticant(2000, 0, 0);
			u.pract_toomanysacs2 = TRUE;
		}
		if (u.cnd_offercount >= 30 && !u.pract_toomanysacs3) {
			pline("%s thunders: 'That's enough! Now there's the conventional penalty for you!'", noroelaname());
			u.pract_conv2timer = 5000;
			u.pract_toomanysacs3 = TRUE;
		}
		if (u.cnd_altarconvertamount && !u.pract_altarconvert) {
			pline("%s rings out: 'I just caught you converting an altar without permission, and therefore you have to pay 2000 zorkmids now.'", noroelaname());
			fineforpracticant(2000, 0, 0);
			u.pract_altarconvert = TRUE;

		}
		if (!u.pract_toomucharmor && uarm && uarm->spe >= 2 && uarmu && uarmu->spe >= 2 && uarmc && uarmc->spe >= 2 && uarms && uarms->spe >= 2 && uarmh && uarmh->spe >= 2 && uarmf && uarmf->spe >= 2 && uarmg && uarmg->spe >= 2 ) {
			pline("%s thunders: 'You may not wear that much armor, and therefore all of it is disenchanted now and additionally you have to pay 3000 zorkmids to me as a penalty!'", noroelaname());
			fineforpracticant(3000, 0, 0);
			uarm->spe--; /* we made sure that these exist */
			uarmu->spe--;
			uarmc->spe--;
			uarms->spe--;
			uarmh->spe--;
			uarmf->spe--;
			uarmg->spe--;
			u.pract_toomucharmor = TRUE;
		}
		if (!u.pract_fastform && ((Upolyd && moveamt > 24) || (u.usteed && (mcalcmove(u.usteed) > 24) )) ) {
			pline("%s rings out: 'Such fast forms aren't allowed because they constantly exceed the maximum permissible speed! Therefore I'm getting a trebuchet now!'", noroelaname());
			makemon(&mons[PM_TREBUCHET_DRAGON], 0, 0, MM_ANGRY|MM_FRENZIED|MM_XFRENZIED);
			u.pract_fastform = TRUE;
		}
		if (StrongRegeneration && !u.pract_fastregen) {
			pline("%s thunders: 'There may be no practicant who regenerates more than one hit point per turn! You violated this rule and therefore you have hemophilia now.'", noroelaname());
			BloodLossProblem |= FROMOUTSIDE;
			u.pract_fastregen = TRUE;
		}
		if (uwep && uwep->otyp == SNIPER_RIFLE && !u.pract_toomuchrange) {
			pline("%s thunders: 'This weapon has too much range, and therefore you have to pay all your money to me now.'", noroelaname());
			if (u.ugold || hidden_gold()) fineforpracticant(u.ugold + hidden_gold(), 0, 0);
			else fineforpracticant(10000, 0, 0); /* wise guy, eh? */
			u.pract_toomuchrange = TRUE;
		}
		if (Is_nemesis(&u.uz) && !u.pract_gottoonear4) {
			u.pract_tooneartimer++;
			if (u.pract_tooneartimer >= 500 && !u.pract_gottoonear) {
				pline("%s rings out: 'You pay 50 zorkmids to me, reason: obtrusive assistant decomposition!'", noroelaname());
				fineforpracticant(50, 0, 0);
				u.pract_gottoonear = TRUE;
			}
			if (u.pract_tooneartimer >= 1000 && !u.pract_gottoonear2) {
				pline("%s rings out: 'Since you're still pestering me, I take off 100 zorkmids from you now. Every additional such offense will cost twice as much.'", noroelaname());
				fineforpracticant(100, 0, 0);
				u.pract_gottoonear2 = TRUE;
			}
			if (u.pract_tooneartimer >= 1500 && !u.pract_gottoonear3) {
				pline("%s booms: 'You're still coming too close to me: 200 zorkmids are to be paid as a penalty, immediately. I'll think of some other penalty that I can hit you with.'", noroelaname());
				fineforpracticant(200, 0, 0);
				u.pract_gottoonear3 = TRUE;
			}
			if (u.pract_tooneartimer >= 2000 && !u.pract_gottoonear4) {
				pline("%s thunders: 'That's a big crime, called obtrusive assistant decomposition! That makes 1000 zorkmids - many thanks for the money!'", noroelaname());
				fineforpracticant(1000, 0, 0);
				u.pract_gottoonear4 = TRUE;
			}
		}
		if (!u.pract_idling && !u.pract_idlingtimer && occupation && occtxt && (!strcmp(occtxt, "waiting") || !strcmp(occtxt, "searching")) && ((multi > 50) || (multi < -50))) {
			pline("%s thunders: 'Sitting idly is harshly penalized - now 400 of your zorkmids are collected for safe keeping!'", noroelaname());
			fineforpracticant(400, 0, 0);
			u.pract_idlingtimer = 1000;
			u.pract_idling = TRUE;
		}
		if (((P_SKILL(P_TWO_HANDED_SWORD) >= P_BASIC) || (P_SKILL(P_PICK_AXE) >= P_BASIC)) && !u.pract_forbiddenskill) {
			pline("%s thunders: 'You trained skills that your role can't even have, and therefore it costs 2000 zorkmids and 2000 nutrition now, because I can't be arsed.'", noroelaname());
			fineforpracticant(2000, 0, 0);
			morehungry(2000);
			u.pract_forbiddenskill = TRUE;
		}
		if (Detect_monsters && !u.pract_espionage) {
			pline("%s thunders: 'You've committed a huge crime: Industrial espionage is strictly forbidden! That costs 20000 zorkmids!'", noroelaname());
			fineforpracticant(20000, 0, 0);
			u.pract_espionage = TRUE;
		}
		if (HAntimagic && !u.pract_magicresistance) {
			pline("%s thunders: 'You got a intrinsic that you don't even have! That's fraud! The fine for that is a nice juicy conventional penalty, you maggot.'", noroelaname());
			u.pract_conv4timer = 5000;
			u.pract_magicresistance = TRUE;
		}
		if (u.cnd_gunpowderused >= 100 && !u.pract_bullets) {
			pline("%s rings out: 'You're not supposed to use so much gunpowder, and therefore you pay 2000 zorkmids to me now!'", noroelaname());
			fineforpracticant(2000, 0, 0);
			u.pract_bullets = TRUE;
		}
		if (u.cnd_gunpowderused >= 200 && !u.pract_bullets2) {
			pline("%s booms: 'I've told you not to use too much gunpowder! That's 4000 zorkmids now. Be warned, for if you do that again I'll give you a particularly nasty fine.'", noroelaname());
			fineforpracticant(4000, 0, 0);
			u.pract_bullets2 = TRUE;
		}
		if (u.cnd_gunpowderused >= 300 && !u.pract_bullets3) {
			pline("%s thunders: 'Okay, that's it. Your guns shall jam for a while now. Also, 8000 zorkmids have to be paid immediately.'", noroelaname());
			fineforpracticant(8000, 0, 0);
			AllSkillsUnskilled += rnz(5000);
			u.pract_bullets3 = TRUE;
		}
		if (uwep && uwep->otyp == HEAVY_MACHINE_GUN && !u.pract_heavymg) {
			pline("%s booms: 'This gun isn't meant for scrawny little practicants like you! Therefore you have to pay 1000 zorkmids and 1000 stones now!'", noroelaname());
			fineforpracticant(1000, 1000, 0);
			u.pract_heavymg = TRUE;
		}
		if (u.contamination >= 1000 && !u.pract_fatalcontamination) {
			pline("%s booms: 'That's a violation of radiation safety protocols right there if I've ever seen one! You there, little practicant maggot! For this transgression you will pay 10000 zorkmids to me, and your lab coat won't protect you from poison for a week. Keep your hands off of radioactive materials, you hear?'", noroelaname());
			fineforpracticant(10000, 0, 0);
			u.uprops[DEAC_POISON_RES].intrinsic += 20000;
			u.pract_fatalcontamination = TRUE;
		}
		if (u.usanity >= 900 && !u.pract_sanity) {
			pline("%s booms: 'Are you on drugs? You're not allowed to enter the lab in that condition! Now you pay 2000 zorkmids and leave the lab, and don't come back before you get sober!'", noroelaname());
			fineforpracticant(2000, 0, 0);
			u.pract_sanity = TRUE;
		}
		if (u.usanity >= 9000 && !u.pract_sanity2) {
			pline("%s thunders: 'Now look at that stoned practicant maggot... You there! Pay 20000 zorkmids to me or that is your end! If you didn't understand that, I won't say it a second time - pay up or there'll be more fines!'", noroelaname());
			fineforpracticant(20000, 0, 0);
			u.pract_sanity2 = TRUE;
		}
		if (uwep && uwep->oinvis) {
			uwep->oinvis = uwep->oinvisreal = FALSE;
			pline("%s rings out: 'I see your invisible weapon, maggot! Now I'll turn it visible again so as to prevent you from secretly stabbing the other practicants, and then you still pay 5000 zorkmids to me. And of course you're banned from being invisible yourself!'", noroelaname());
			u.uprops[DEAC_INVIS].intrinsic += rnz(5000);
			fineforpracticant(5000, 0, 0);
		}
		if (moves > 10000 && u.urexp < 10000 && !u.pract_lowscore) {
			pline("%s thunders: 'Ah! You're obviously idling instead of doing the work you've been assigned to do! Now you get your purse and number out 2000 zorkmids to me, and then you go back to the lab and do your goddamn work!'", noroelaname());
			fineforpracticant(2000, 0, 0);
			u.pract_lowscore = TRUE;
		}
		if (u.uhpmax > 500 && !u.pract_lotsofhp) {
			pline("%s thunders: 'You've gained too many hit points! How I'll punish you for that, I will not say, but be aware that you will experience my displeasure sometime in the future...'", noroelaname());
			register struct monst *offmon;
			if ((offmon = makemon(&mons[PM_NOROELA_TRAPPER], 0, 0, MM_ANGRY|MM_FRENZIED|MM_XFRENZIED)) != 0) {
				u_teleport_monB(offmon, FALSE);
			}
			u.pract_lotsofhp = TRUE;
		}
		if (u.uenmax > 500 && !u.pract_lotsofmp && !u.ragnaroktimer) {
			pline("%s thunders: 'Now you made me push the end-of-the-world switch, because I won't accept you to have so much mana. Know this: the apocalypse will come, and you can't do anything about it.'", noroelaname());
			u.ragnaroktimer = rnz(100000);
			u.pract_lotsofmp = TRUE;
		}

		if (moves > u.pract_procrastinatetimer && !u.pract_procrastinate) {
			ragnarok(TRUE);
			if (evilfriday) evilragnarok(TRUE,level_difficulty());
			pline("%s thunders: 'I'm fed up with you, practicant. So I decided that the world ends now. Goodbye.'", noroelaname());
			u.pract_procrastinate = TRUE;
		}
		if (P_SKILL(P_GUN_CONTROL) >= P_EXPERT && !u.pract_expertguncontrol) {
			pline("%s thunders: 'That overuse of guns isn't permitted. Am I supposed to get the Thai, or do you give up? You have 30 seconds to decide.'", noroelaname());
			getlin ("Do you give up? [yes/no]",buf);
			(void) lcase (buf);
			if (!(strcmp (buf, "yes"))) {
				dataskilldecrease(); /* whoops! you just lost ALL skill training */
				pline("%s booms: 'Wise choice. All of your skills have been reset to zero. Don't dare using guns again, you hear?'", noroelaname());
			} else {
				(void) makemon(&mons[PM_THAI], 0, 0, MM_ANGRY|MM_FRENZIED|MM_XFRENZIED);
				pline("%s thunders: 'Thai will break all your bones with her black block heels and you'll end up in the hospital. Better call an ambulance, maggot.'", noroelaname());
			}

			u.pract_expertguncontrol = TRUE;
		}
		if (In_voiddungeon(&u.uz) && !u.pract_void) {
			pline("%s thunders: 'The void is off-limits for little practicants like you! Leave now, or I'll call the police! You have 5 seconds.'", noroelaname());
			u.pract_voidtimer = (moves + 5);
			u.pract_void = TRUE;
		}

	} /* practicant terror check */

	if ((Race_if(PM_MAYMES) || Race_if(PM_AZTPOK) || Race_if(PM_PLAYER_ATLANTEAN)) && P_MAX_SKILL(P_RIDING) >= P_BASIC) {
		skillcaploss_specific(P_RIDING);
	}

	if (HardcoreAlienMode && P_MAX_SKILL(P_HIGH_HEELS) >= P_BASIC) {
		skillcaploss_specific(P_HIGH_HEELS);
	}

	if (uarmc && uarmc->oartifact == ART_ULTRAGGRAVATE) {
		u.heavyaggravation = 1;
	}

	if (WingYellowChange || u.uprops[WING_YELLOW_GLYPHS].extrinsic || have_wingyellowstone()) {
		iflags.winggraphics = TRUE;
#ifdef CURSES_GRAPHICS
		iflags.cursesgraphics = FALSE;
#endif
		assignwinggraphics();
		u.wingyellowhack = TRUE;
	} else {
		iflags.winggraphics = FALSE;
		if (u.wingyellowhack == TRUE) {
			u.wingyellowhack = FALSE;
			switch_graphics(iflags.DECgraphics ? DEC_GRAPHICS : iflags.IBMgraphics ? IBM_GRAPHICS : ASCII_GRAPHICS);
		}
	}

	if (ManlerIsChasing) {
		if (u.manlerx < 0) {
			if (u.ux < 39) u.manlerx = 69;
			else u.manlerx = 9;
		}
		if (u.manlery < 0) {
			if (u.uy < 9) u.manlery = 14;
			else u.manlery = 4;
		}
	} else {
		u.manlerx = u.manlery = -1;
	}

	if (PokelieEffect || u.uprops[POKELIE_EFFECT].extrinsic || have_pokeliestone()) {
		if (!u.pokelieresistances) {
			u.pokelieresistances = rnd(17);
			u.pokeliegeneration = 0;
			u.pokelieflags = 0;
			u.pokeliespeed = rnd(50);
			u.pokelieattacktype = rnd(22);
			u.pokeliedamagetype = rnd(158);
		}
	} else {
		u.pokelieresistances = 0;
		u.pokeliegeneration = 0;
		u.pokelieflags = 0;
		u.pokeliespeed = 0;
		u.pokelieattacktype = 0;
		u.pokeliedamagetype = 0;
	}


	/* Frequentation spawn should be a different trait every time you get the effect --Amy */
	if (FrequentationSpawns || u.uprops[FREQUENTATION_SPAWNS].extrinsic || have_frequentationspawnstone()) {
		u.frequentationtrait = rnd(376); /* same as monstercolor function */
	} else {
		u.frequentationtrait = 0;
	}

	if (u.shiftingsandsinking && !(is_shiftingsand(u.ux, u.uy))) {
		u.shiftingsandsinking = 0;
		You("escaped the shifting sand.");
		if (FunnyHallu) pline("The Grim Reaper was waiting to take you away, too.");
	}

	if (TheGameLaaaaags) {

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
		pline("%s", generate_garbage_string());
	}

#if 0
		/* for making it impossible to start new games in old slex versions */
		if (moves < 50) {
			pline("You are playing an old version of SLEX, which is no longer supported and therefore your game ends. The version is only kept on the server so that people can finish old savegames. If you had a far-progressed savegame that is somehow gone, complain to the server administrators and they might be able to restore it. --Amy");
			u.youaredead = 1;
			u.youarereallydead = 1;
			done(ESCAPED);
			/* still game over if you somehow get here */
			done(DIED);
		}
#endif

	if (uarmh && uarmh->oartifact == ART_UBB_RUPTURE && !rn2(10)) {
		pline("%s", generate_garbage_string());
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

		if (!rn2(isfriday ? 10 : 20)) docrt();
		vision_recalc(0);

	}

	if (RngeTrapAlert && t_at(u.ux, u.uy)) {
		pline("Alert! You are standing on a trap!");
	}

	if (uarmg && t_at(u.ux, u.uy) && itemhasappearance(uarmg, APP_SENSOR_GLOVES) ) {
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

	if ((BankTrapEffect || (uarmf && uarmf->oartifact == ART_SONJA_S_TORN_SOUL) || (uleft && uleft->oartifact == ART_ARABELLA_S_RESIST_COLD) || (uright && uright->oartifact == ART_ARABELLA_S_RESIST_COLD) || (uamul && uamul->oartifact == ART_LOW_ZERO_NUMBER) || (uamul && uamul->oartifact == ART_ARABELLA_S_PRECIOUS_GADGET) || u.uprops[BANKBUG].extrinsic || have_bankstone()) && u.ugold) {

		if (!u.bankcashlimit) u.bankcashlimit = rnz(1000 * (monster_difficulty() + 1));

		u.bankcashamount += u.ugold;

		if (uarmf && uarmf->oartifact == ART_SONJA_S_TORN_SOUL) {
			adjalign(u.ugold / 100);
			if (u.ugold > 999) u.alignlim += (u.ugold / 1000);
		}

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
	if (iflags.sanity_check || iflags.debug_fuzzer)
	    sanity_check();
#elif defined(OBJ_SANITY)
	if (iflags.sanity_check || iflags.debug_fuzzer)
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
		if (((u.uevent.udemigod || u.uhave.amulet) && !u.freeplaymode) || CannotTeleport || (u.usteed && mon_has_amulet(u.usteed))) {
			You("shudder for a moment."); (void) safe_teleds(FALSE); u.banishmentbeam = 0; break;
		}

		if (flags.lostsoul || flags.uberlostsoul || (flags.wonderland && !(u.wonderlandescape)) || (iszapem && !(u.zapemescape)) || u.uprops[STORM_HELM].extrinsic || In_bellcaves(&u.uz) || In_subquest(&u.uz) || In_voiddungeon(&u.uz) || In_netherrealm(&u.uz)) { 
			pline("Somehow, the banishment beam doesn't do anything."); u.banishmentbeam = 0; break;
		}

		u.cnd_banishmentcount++;
		if (rn2(2)) {(void) safe_teleds(FALSE); goto_level(&medusa_level, TRUE, FALSE, FALSE); }
		else { (void) safe_teleds(FALSE); goto_level(&portal_level, TRUE, FALSE, FALSE); }
		u.banishmentbeam = 0; /* player got warped, now clear the flag even if it crashes afterwards */

		register int newlev = rnd(99);
		d_level newlevel;
		get_level(&newlevel, newlev);
		goto_level(&newlevel, TRUE, FALSE, FALSE);
	}

	if (u.levelporting) { /* something attacked you with nexus or weeping */

		if ((!u.uevent.udemigod || u.freeplaymode) && !(flags.lostsoul || flags.uberlostsoul || (flags.wonderland && !(u.wonderlandescape)) || (iszapem && !(u.zapemescape)) || u.uprops[STORM_HELM].extrinsic || In_bellcaves(&u.uz) || In_subquest(&u.uz) || In_voiddungeon(&u.uz) || In_netherrealm(&u.uz)) ) {
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

	if (uarmc && itemhasappearance(uarmc, APP_QUICKTRAVEL_CLOAK) ) return;

	if (uarmf && uarmf->oartifact == ART_TOO_FAST__TOO_FURIOUS) return;

	if (RngePermanentOccupation) return;

	if(occupation) {

		if (u.katitrapocc) {
			pline("Something tries to interrupt your attempt to clean the Kati shoes! If you stop now, the sexy girl will hate you!");
			if (yn("Really stop cleaning them?") == 'y') {

			      register struct monst *mtmp2;

				for (mtmp2 = fmon; mtmp2; mtmp2 = mtmp2->nmon) {

					if (!mtmp2->mtame) {
						mtmp2->mpeaceful = 0;
						mtmp2->mfrenzied = 1;
						mtmp2->mhp = mtmp2->mhpmax;
					}
				}
				pline("The beautiful girl in the sexy Kati shoes is very sad that you didn't finish cleaning her lovely boots, and urges everyone in her vicinity to bludgeon you.");

			} else return;
		}

		if (!maybe_finished_meal(TRUE))
		    You("stop %s.", occtxt);
		occupation = 0;
		u.katitrapocc = FALSE;
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

	monst_globals_init();

	for (i = 0; i < NUMMONS; i++)
		mvitals[i].mvflags = mons[i].geno & G_NOCORPSE;

	init_objects(TRUE);		/* must be before u_init() */

	randommaterials();	/* only done here - do not call this during a running game! --Amy */

	flags.pantheon = -1;	/* role_init() will reset this */
	role_init();		/* must be before init_dungeons(), u_init(),
				 * and init_artifacts() */

	if (flags.askforalias) {
		char aliasbuf[2048];	/* Buffer for alias name */
		char eliasbuf[2048];
		int aliaslength;
		int testx;

		pline("Your character: %s %s %s %s", (aligns[flags.initalign].value == A_CHAOTIC) ? "chaotic" : (aligns[flags.initalign].value == A_NEUTRAL) ? "neutral" : "lawful", flags.initgend ? "female" : "male", urace.adj, (flags.initgend && urole.name.f) ? urole.name.f : urole.name.m);
aliasagain:

		sprintf(aliasbuf,"What is your alias name?");
		getlin(aliasbuf, eliasbuf);
		aliaslength = strlen(eliasbuf);

		if (aliaslength > 30) {
			pline("That name is too long. Maximum 30 characters. Sorry.");
			goto aliasagain;
		}

		for (testx = 0; testx >= 0; testx++) {

			if (testx > aliaslength) break;
			if (eliasbuf[testx]) {
				if (eliasbuf[testx] == ' ') continue;
				if (eliasbuf[testx] == '.') continue;
				if (eliasbuf[testx] == ',') continue;
				if (eliasbuf[testx] == '-') continue;
				if (eliasbuf[testx] >= 'A' && eliasbuf[testx] <= 'Z') continue;
				if (eliasbuf[testx] >= 'a' && eliasbuf[testx] <= 'z') continue;
				if (eliasbuf[testx] >= '0' && eliasbuf[testx] <= '9') continue;
				pline("You can only use spaces, alphanumeric characters or .,- characters. Sorry.");
				goto aliasagain;
			}
		}

		if (eliasbuf[0] && aliaslength < 31) { /* We do NOT want a buffer overflow. --Amy */
			if (eliasbuf && !(strncmpi(eliasbuf, "Glorious Dead", 14) ) ) strcpy(eliasbuf, "Cheator");
			strcpy(plalias, eliasbuf);
			(void) strncpy(u.aliasname, eliasbuf, sizeof(u.aliasname));
		}
	}

	if (!strncmpi(plname, "Glorious Dead", 14)) {
		strcpy(plalias, "Uber Cheator");
		strcpy(u.aliasname, "Uber Cheator");
	}

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
              setworn(mkobj(CHAIN_CLASS, TRUE, FALSE), W_CHAIN);
              setworn(mkobj(BALL_CLASS, TRUE, FALSE), W_BALL);
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
        } else if (Role_if(PM_WALSCHOLAR)) {
		    com_pager(194);
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

	u.lightsabermax1 = (P_SKILL(P_SHII_CHO) >= P_BASIC) ? P_SKILL(P_SHII_CHO) : 1;
	u.lightsabermax2 = (P_SKILL(P_MAKASHI) >= P_BASIC) ? P_SKILL(P_MAKASHI) : 1;
	u.lightsabermax3 = (P_SKILL(P_SORESU) >= P_BASIC) ? P_SKILL(P_SORESU) : 1;
	u.lightsabermax4 = (P_SKILL(P_ATARU) >= P_BASIC) ? P_SKILL(P_ATARU) : 1;
	u.lightsabermax5 = (P_SKILL(P_SHIEN) >= P_BASIC) ? P_SKILL(P_SHIEN) : 1;
	u.lightsabermax6 = (P_SKILL(P_DJEM_SO) >= P_BASIC) ? P_SKILL(P_DJEM_SO) : 1;
	u.lightsabermax7 = (P_SKILL(P_NIMAN) >= P_BASIC) ? P_SKILL(P_NIMAN) : 1;
	u.lightsabermax8 = (P_SKILL(P_JUYO) >= P_BASIC) ? P_SKILL(P_JUYO) : 1;
	u.lightsabermax9 = (P_SKILL(P_VAAPAD) >= P_BASIC) ? P_SKILL(P_VAAPAD) : 1;
	u.lightsabermax10 = (P_SKILL(P_WEDI) >= P_BASIC) ? P_SKILL(P_WEDI) : 1;

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

	if (WingYellowChange || u.uprops[WING_YELLOW_GLYPHS].extrinsic || have_wingyellowstone()) {
		iflags.winggraphics = TRUE;
#ifdef CURSES_GRAPHICS
		iflags.cursesgraphics = FALSE;
#endif
		assignwinggraphics();
	} else {
		iflags.winggraphics = FALSE;
	}

	/* matrayser hybrid race: all of the player's starting inventory items are levelported away
	 * after that has happened, all the item materials are shuffled --Amy */
	if (ismatrayser && new_game) {
		int mattries = 0;
		register struct monst *offmon;
		while ((inv_cnt() > 0) && mattries++ < 50000) {
			if ((offmon = makemon(&mons[PM_HURO_TROVE], 0, 0, MM_ANGRY)) != 0) {
				char bufof[BUFSZ];
				bufof[0] = '\0';
				steal(offmon, bufof, TRUE, TRUE);
				u_teleport_monB(offmon, FALSE);
			}
		}
		matraysershuffle();
	}

	if (TheInfoIsFucked) {
		pline("You've forgotten who you are, but you are back.");
		return;
	}

	if (new_game && (Movemork || u.uprops[MOVEMORKING].extrinsic || have_movemorkstone())) {
		nomul(-2, "acclimating in the dungeon", FALSE);
		nomovemsg = "You are now ready to explore the dungeon.";
	}

	/* prevent hangup cheating when special game modes haven't teleported you yet --Amy */
	if ((flags.wonderland || iszapem || flags.lostsoul || flags.uberlostsoul || Role_if(PM_SOFTWARE_ENGINEER) || Role_if(PM_CRACKER) || Role_if(PM_JANITOR) || Role_if(PM_SPACE_MARINE) || Role_if(PM_STORMBOY) || Role_if(PM_YAUTJA) || Role_if(PM_QUARTERBACK) || Role_if(PM_PSYKER) || Role_if(PM_EMPATH) || Role_if(PM_MASTERMIND) || Role_if(PM_WEIRDBOY) || Role_if(PM_ASTRONAUT) || Role_if(PM_CYBERNINJA) || Role_if(PM_DISSIDENT) || Race_if(PM_RETICULAN) || Race_if(PM_OUTSIDER) || Role_if(PM_XELNAGA)) && new_game) {
		u.youaredead = 1;
		u.youarereallydead = 1;
	}

    if (!new_game) {

	/* restore the player's alias name, if existing - LENGTH MUST BE BELOW 32 OR THERE IS A BUFFER OVERRUN --Amy */
	if (u.aliasname[0] && (strlen(u.aliasname) < 31)) {
		strcpy(plalias, u.aliasname);
	}

	/* restore heretic gods */
	reinitgods();

	/* in case healthstones and stuff got out of sync somehow... */
	recalc_health();
	recalc_mana();

    }

    if (new_game || u.ualignbase[A_ORIGINAL] != u.ualignbase[A_CURRENT])
	sprintf(eos(buf), " %s", align_str(u.ualignbase[A_ORIGINAL]));
    if (!urole.name.f &&
	    (new_game ? (urole.allow & ROLE_GENDMASK) == (ROLE_MALE|ROLE_FEMALE) :
	     currentgend != flags.initgend))
	sprintf(eos(buf), " %s", genders[currentgend].adj);

    *xtrabuf = '\0';

	int hybridcount = 0;

	if (flags.hybridangbander && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "angbander ");
	if (flags.hybridaquarian && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "aquarian ");
	if (flags.hybridcurser && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "curser ");
	if (flags.hybridhaxor && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "haxor ");
	if (flags.hybridhomicider && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "homicider ");
	if (flags.hybridsuxxor && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "suxxor ");
	if (flags.hybridwarper && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "warper ");
	if (flags.hybridrandomizer && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "randomizer ");
	if (flags.hybridnullrace && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "null ");
	if (flags.hybridmazewalker && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "mazewalker ");
	if (flags.hybridsoviet && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "soviet ");
	if (flags.hybridxrace && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "x-race ");
	if (flags.hybridheretic && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "heretic ");
	if (flags.hybridsokosolver && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "sokosolver ");
	if (flags.hybridspecialist && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "specialist ");
	if (flags.hybridamerican && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "american ");
	if (flags.hybridminimalist && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "minimalist ");
	if (flags.hybridnastinator && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "nastinator ");
	if (flags.hybridrougelike && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "rougelike ");
	if (flags.hybridsegfaulter && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "segfaulter ");
	if (flags.hybridironman && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "ironman ");
	if (flags.hybridamnesiac && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "amnesiac ");
	if (flags.hybridproblematic && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "problematic ");
	if (flags.hybridwindinhabitant && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "windinhabitant ");
	if (flags.hybridaggravator && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "aggravator ");
	if (flags.hybridevilvariant && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "evilvariant ");
	if (flags.hybridlevelscaler && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "levelscaler ");
	if (flags.hybriderosator && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "erosator ");
	if (flags.hybridroommate && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "roommate ");
	if (flags.hybridextravator && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "extravator ");
	if (flags.hybridhallucinator && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "hallucinator ");
	if (flags.hybridbossrusher && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "bossrusher ");
	if (flags.hybriddorian && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "dorian ");
	if (flags.hybridtechless && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "techless ");
	if (flags.hybridblait && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "blait ");
	if (flags.hybridgrouper && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "grouper ");
	if (flags.hybridscriptor && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "scriptor ");
	if (flags.hybridunbalancor && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "unbalancor ");
	if (flags.hybridbeacher && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "beacher ");
	if (flags.hybridstairseeker && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "stairseeker ");
	if (flags.hybridmatrayser && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "matrayser ");
	if (flags.hybridfeminizer && (hybridcount++ < 20)) sprintf(eos(xtrabuf), "feminizer ");
	if (hybridcount >= 20) sprintf(eos(xtrabuf), "(%d hybrids) ", hybridcount);

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
	  Hello((struct monst *) 0), playeraliasname, buf, xtrabuf, urace.adj,
	  (currentgend && urole.name.f) ? urole.name.f : urole.name.m);
#endif
    if (new_game) pline("%s %s, welcome to %s!  You are a%s %s%s %s.",
	  Hello((struct monst *) 0), playeraliasname, issoviet ? "SlashTHEM Extended" : DEF_GAME_NAME, buf, xtrabuf, urace.adj,
	  (currentgend && urole.name.f) ? urole.name.f : urole.name.m);
    else pline("%s %s, the%s %s%s %s, welcome back to %s!",
	  Hello((struct monst *) 0), playeraliasname, buf, xtrabuf, urace.adj,
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
#ifdef BIGSLEX
	pline("Attention: You're playing BIGslex, where the dungeon levels are bigger than normal. Recommended terminal size is 125x45. Also, savebreaks will happen without warning in this version. If you have a far-progressed savegame that you want to finish, contact me on the IRC. If your savegame seems to be gone, contact me on the IRC too. Have fun!");
#endif /* BIGSLEX */
	if (new_game) pline("Message of the day: This is a bleeding-edge development version of SLEX. Playtesters are welcome. You can help me in particular by playing the new roles and races that have been added lately. If you encounter any bugs or weirdness while playing, please notify me, the best way of contacting me is via the #em.slashem.me IRC channel (Freenode). --Amy");
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

	obj_descr[SPE_GRAVE].oc_name = "mogila";
	obj_descr[SPE_TUNNELS].oc_name = "tunneli";
	obj_descr[SPE_FARMING].oc_name = "sel'skoye khozyaystvo";
	obj_descr[SPE_MOUNTAINS].oc_name = "gory";
	obj_descr[SPE_DIVING].oc_name = "dayving";
	obj_descr[SPE_CRYSTALLIZATION].oc_name = "kristallizatsiya";
	obj_descr[SPE_MOORLAND].oc_name = "vereshchatnik";
	obj_descr[SPE_URINE].oc_name = "mocha";
	obj_descr[SPE_QUICKSAND].oc_name = "plyvun";
	obj_descr[SPE_STYX].oc_name = "stiks";
	obj_descr[SPE_ATTUNE_MAGIC].oc_name = "magiya nastroyki";
	obj_descr[SPE_SNOW].oc_name = "sneg";
	obj_descr[SPE_ASH].oc_name = "yasen'";
	obj_descr[SPE_SAND].oc_name = "pesok";
	obj_descr[SPE_PAVING].oc_name = "moshcheniye";
	obj_descr[SPE_HIGHWAY].oc_name = "shosse";
	obj_descr[SPE_GRASSLAND].oc_name = "lug";
	obj_descr[SPE_NETHER_MIST].oc_name = "nizhniy tuman";
	obj_descr[SPE_STALACTITE].oc_name = "stalaktit";
	obj_descr[SPE_CRYPT].oc_name = "kripta";
	obj_descr[SPE_BUBBLE_BOBBLE].oc_name = "puzyr'";
	obj_descr[SPE_RAIN].oc_name = "dozhd'";
	obj_descr[SPE_GAIN_SPACT].oc_name = "poluchit' tolchok";
	obj_descr[SPE_METAL_GUARD].oc_name = "metallicheskaya zashchita";
	obj_descr[SPE_MAGIC_WHISTLING].oc_name = "volshebnyy svist";

	obj_descr[SPE_ORE_MINING].oc_name = "dobycha rudy";
	obj_descr[SPE_BOILER_KABOOM].oc_name = "zvuk vzryva kotla";
	obj_descr[SPE_DEFOG].oc_name = "predotvrashcheniye zapotevaniya";
	obj_descr[SPE_SWAP_POSITION].oc_name = "pozitsiya svop";
	obj_descr[SPE_SHUFFLE_MONSTER].oc_name = "sluchaynyy monstr";
	obj_descr[SPE_PET_SYRINGE].oc_name = "domashneye zhivotnoye shprits";
	obj_descr[SPE_BUC_KNOWLEDGE].oc_name = "blagoslovennoye proklyatoye znaniye";
	obj_descr[SPE_PREACHING].oc_name = "propovedovaniye";
	obj_descr[SPE_RESIST_PARALYSIS].oc_name = "soprotivlyat'sya paralichu";
	obj_descr[SPE_KEEP_SATIATION].oc_name = "prodolzhay nasyshchat'sya";
	obj_descr[SPE_TECH_BOOST].oc_name = "tekhnicheskaya podderzhka";
	obj_descr[SPE_CONTINGENCY].oc_name = "nepredvidennyye obstoyatel'stva";
	obj_descr[SPE_AULE_SMITHING].oc_name = "kuznets";
	obj_descr[SPE_HORSE_HOP].oc_name = "konnyy pryzhok";
	obj_descr[SPE_LINE_LOSS].oc_name = "poterya linii";
	obj_descr[SPE_TACTICAL_NUKE].oc_name = "takticheskoye yadernoye oruzhiye";
	obj_descr[SPE_RAGNAROK].oc_name = "bozhiy rassvet";
	obj_descr[SPE_ARMOR_SMASH].oc_name = "razrusheniye dospekhov";
	obj_descr[SPE_STRANGLING].oc_name = "zadykhayushchiysya";
	obj_descr[SPE_PARTICLE_CANNON].oc_name = "pushka chastits";
	obj_descr[SPE_ONE_POINT_SHOOT].oc_name = "odna tochka strelyat'";
	obj_descr[SPE_BLOOD_STREAM].oc_name = "krovotok";
	obj_descr[SPE_SHINING_WAVE].oc_name = "siyayushchaya volna";
	obj_descr[SPE_GROUND_STOMP].oc_name = "topat' zemlyu";
	obj_descr[SPE_DIRECTIVE].oc_name = "direktiva";
	obj_descr[SPE_NERVE_POISON].oc_name = "nervnyy yad";
	obj_descr[SPE_POWDER_SPRAY].oc_name = "poroshkovyy sprey";
	obj_descr[SPE_FIREWORKS].oc_name = "feyyerverk";
	obj_descr[SPE_AIMBOT_LIGHTNING].oc_name = "pritsel molnii";
	obj_descr[SPE_ENHANCE_BREATH].oc_name = "uluchshit' dykhaniye";
	obj_descr[SPE_GOUGE_DICK].oc_name = "vybit' chlen";
	obj_descr[SPE_BODYFLUID_STRENGTHENING].oc_name = "ukrepleniye telesnoy zhidkosti";
	obj_descr[SPE_GEYSER].oc_name = "geyzer";
	obj_descr[SPE_BUBBLING_HOLE].oc_name = "puzyrchataya dyra";
	obj_descr[SPE_PURIFICATION].oc_name = "ochistka";
	obj_descr[SPE_ADD_SPELL_MEMORY].oc_name = "dobavit' pamyat' zaklinaniy";
	obj_descr[SPE_NEXUSPORT].oc_name = "port svyazi";
	obj_descr[SPE_GIANT_FOOT].oc_name = "gigantskaya noga";
	obj_descr[SPE_ANTI_TELEPORTATION].oc_name = "anti-teleportatsiya";
	obj_descr[SPE_SYMHEAL].oc_name = "sim lechit'";

	/* todo area */

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

		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "old-fashioned cloak")) OBJ_DESCR(objects[i]) = "staromodnyy plashch";

		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "rifling power cloak")) OBJ_DESCR(objects[i]) = "naplechnyy shchit sily";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cursed called cloak")) OBJ_DESCR(objects[i]) = "proklyatyy pod nazvaniyem plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "failuncap cloak")) OBJ_DESCR(objects[i]) = "mantiya s provalom";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "unostentatious cloak")) OBJ_DESCR(objects[i]) = "nenavyazchivyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fourchan cloak")) OBJ_DESCR(objects[i]) = "chetyrekhchasovoy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "erudite cloak")) OBJ_DESCR(objects[i]) = "erudirovannyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "inalish cloak")) OBJ_DESCR(objects[i]) = "glupyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "zombified cloak")) OBJ_DESCR(objects[i]) = "zombirovannyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "transversal helmet")) OBJ_DESCR(objects[i]) = "poperechnyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "grunter helmet")) OBJ_DESCR(objects[i]) = "shturmovoy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "precious helmet")) OBJ_DESCR(objects[i]) = "dragotsennyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cloudy helmet")) OBJ_DESCR(objects[i]) = "oblachnyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "post-nuclear helmet")) OBJ_DESCR(objects[i]) = "post-yadernyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "speechpipe helmet")) OBJ_DESCR(objects[i]) = "rechevoy trubnyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "failuncap helmet")) OBJ_DESCR(objects[i]) = "shlem s provalom";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "breath control helmet")) OBJ_DESCR(objects[i]) = "shlem upravleniya dykhaniyem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gas mask")) OBJ_DESCR(objects[i]) = "protivogaz";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cold-blooded helmet")) OBJ_DESCR(objects[i]) = "khladnokrovnyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sages helmet")) OBJ_DESCR(objects[i]) = "mudryy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cuddly gloves")) OBJ_DESCR(objects[i]) = "priyatnyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "multilinguitis gloves")) OBJ_DESCR(objects[i]) = "perchatki mul'tilingvita";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "demolition gloves")) OBJ_DESCR(objects[i]) = "perchatki dlya snosa";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "failuncap gloves")) OBJ_DESCR(objects[i]) = "perchatki s perforatsiyey";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "rayductnay gloves")) OBJ_DESCR(objects[i]) = "ruchnyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "flower gloves")) OBJ_DESCR(objects[i]) = "tsvetochnyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gameble gloves")) OBJ_DESCR(objects[i]) = "geymperskiye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "bise gloves")) OBJ_DESCR(objects[i]) = "bi sebe perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "orgasm pumps")) OBJ_DESCR(objects[i]) = "nasosy dlya orgazma";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "worn-out sneakers")) OBJ_DESCR(objects[i]) = "iznoshennyye krossovki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "imaginary heels")) OBJ_DESCR(objects[i]) = "voobrazhayemyye kabluki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "noble sandals")) OBJ_DESCR(objects[i]) = "blagorodnyye sandalii";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "failuncap shoes")) OBJ_DESCR(objects[i]) = "s provalom obuv'";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "flipflops")) OBJ_DESCR(objects[i]) = "shlepki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "rumble boots")) OBJ_DESCR(objects[i]) = "gul botinki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "high iron boots")) OBJ_DESCR(objects[i]) = "vysokiye zheleznyye botinki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "doctor claw boots")) OBJ_DESCR(objects[i]) = "kolgotki dlya sapog";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "spooky boots")) OBJ_DESCR(objects[i]) = "zhutkiy botinok";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "feelgood heels")) OBJ_DESCR(objects[i]) = "chuvstvennyye kabluki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gentle sneakers")) OBJ_DESCR(objects[i]) = "nezhnyy krossovki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "plof heels")) OBJ_DESCR(objects[i]) = "ploskiye kabluki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "princess pumps")) OBJ_DESCR(objects[i]) = "nasosy printsessy";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ballet heels")) OBJ_DESCR(objects[i]) = "baletnyye kabluki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "steel toed boots")) OBJ_DESCR(objects[i]) = "stal'nyye kosolapyy sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "marji shoes")) OBJ_DESCR(objects[i]) = "obuv' marzhi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "mary janes")) OBJ_DESCR(objects[i]) = "meri dzheyn";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "frayed cloak")) OBJ_DESCR(objects[i]) = "iznoshen plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "moist towelette")) OBJ_DESCR(objects[i]) = "vlazhnaya salfetka";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ratty fleece")) OBJ_DESCR(objects[i]) = "rutina";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "horrible christmas sweater")) OBJ_DESCR(objects[i]) = "uzhasnyy rozhdestvenskiy sviter";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "giant throwing star")) OBJ_DESCR(objects[i]) = "gigantskaya zvezda";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fearsome helmet")) OBJ_DESCR(objects[i]) = "groznyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fleshy wrap")) OBJ_DESCR(objects[i]) = "myasistaya plenka";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "feathery cloak")) OBJ_DESCR(objects[i]) = "pernatyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "boiled gloves")) OBJ_DESCR(objects[i]) = "varenyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hairnet helmet")) OBJ_DESCR(objects[i]) = "setka dlya volos";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "glibskin helmet")) OBJ_DESCR(objects[i]) = "shlem iz tolstoy kozhi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "feminism dragonhide shield")) OBJ_DESCR(objects[i]) = "feminizm shchit drakon'yey shkury";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "negative dragonhide shield")) OBJ_DESCR(objects[i]) = "otritsatel'nyy shchit iz drakon'yey shkury";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "heroic dragonhide shield")) OBJ_DESCR(objects[i]) = "geroicheskiy shchit iz drakon'yey shkury";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sand cloak")) OBJ_DESCR(objects[i]) = "pesochnyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "shadow cloak")) OBJ_DESCR(objects[i]) = "tenevoy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "grainy cloak")) OBJ_DESCR(objects[i]) = "zernistyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "obsidian cloak")) OBJ_DESCR(objects[i]) = "obsidianovyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "volcanic cloak")) OBJ_DESCR(objects[i]) = "vulkanicheskiy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "lead cloak")) OBJ_DESCR(objects[i]) = "svintsovyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "different cloak")) OBJ_DESCR(objects[i]) = "drugoy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fuel cloak")) OBJ_DESCR(objects[i]) = "toplivnyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "chrome cloak")) OBJ_DESCR(objects[i]) = "khromovyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "silvermoon cloak")) OBJ_DESCR(objects[i]) = "plashch serebryanoy luny";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "pure cloak")) OBJ_DESCR(objects[i]) = "chistyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ceramic cloak")) OBJ_DESCR(objects[i]) = "keramicheskiy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cloister cloak")) OBJ_DESCR(objects[i]) = "plashch monastyrya";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "nano cloak")) OBJ_DESCR(objects[i]) = "nanorazmernyy plashch";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sand helmet")) OBJ_DESCR(objects[i]) = "pesochnyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "shemagh")) OBJ_DESCR(objects[i]) = "arabskiy golovnoy ubor";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "shadow helmet")) OBJ_DESCR(objects[i]) = "tenevoy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "obsidian helmet")) OBJ_DESCR(objects[i]) = "obsidianovyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "lead helmet")) OBJ_DESCR(objects[i]) = "vedushchiy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "chrome helmet")) OBJ_DESCR(objects[i]) = "khromirovannyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "driver helmet")) OBJ_DESCR(objects[i]) = "shlem voditelya";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ceramic helmet")) OBJ_DESCR(objects[i]) = "keramicheskiy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "serrated helmet")) OBJ_DESCR(objects[i]) = "zubchatyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "nano helmet")) OBJ_DESCR(objects[i]) = "nanorazmernyy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "technical helmet")) OBJ_DESCR(objects[i]) = "tekhnicheskiy shlem";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "beach gloves")) OBJ_DESCR(objects[i]) = "plyazhnyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "paperweight gloves")) OBJ_DESCR(objects[i]) = "bumaga s vesovymi perchatkami";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "nano gloves")) OBJ_DESCR(objects[i]) = "nanorazmernyye perchatki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "metallic shield")) OBJ_DESCR(objects[i]) = "metallicheskiy shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "black shield")) OBJ_DESCR(objects[i]) = "chernyy shchit";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "lovely boots")) OBJ_DESCR(objects[i]) = "prekrasnyye botinki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "feminine sandals")) OBJ_DESCR(objects[i]) = "zhenskiye sandalii";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "everlasting boots")) OBJ_DESCR(objects[i]) = "vechnyye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ether boots")) OBJ_DESCR(objects[i]) = "efirnyye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "elean boots")) OBJ_DESCR(objects[i]) = "elin sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "thick boots")) OBJ_DESCR(objects[i]) = "tolstyye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "brittle boots")) OBJ_DESCR(objects[i]) = "khrupkiye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sand-als")) OBJ_DESCR(objects[i]) = "pesok kak";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "shadowy heels")) OBJ_DESCR(objects[i]) = "temnyye kabluki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "reflective slippers")) OBJ_DESCR(objects[i]) = "svetootrazhayushchiye tapochki";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "lead boots")) OBJ_DESCR(objects[i]) = "svintsovyye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "weight attachment boots")) OBJ_DESCR(objects[i]) = "sapogi navesnyye";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "chrome boots")) OBJ_DESCR(objects[i]) = "khromirovannyye sapogi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fungal sandals")) OBJ_DESCR(objects[i]) = "gribnyye sandalii";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "porcelain sandals")) OBJ_DESCR(objects[i]) = "farforovyye sandalii";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "3D-printed shoes")) OBJ_DESCR(objects[i]) = "trekhmernaya obuv' s printom";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "3D-printed sandals")) OBJ_DESCR(objects[i]) = "trekhmernyye pechatnyye sandalii";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "3D-printed wedges")) OBJ_DESCR(objects[i]) = "trekhmernyye pechatnyye klin'ya";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "standing footwear")) OBJ_DESCR(objects[i]) = "stoyachaya obuv'";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cancel dragonhide shield")) OBJ_DESCR(objects[i]) = "otmenit' shchit iz drakon'yey shkury";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "corona dragonhide shield")) OBJ_DESCR(objects[i]) = "korona iz drakon'yey shkury";

		/* todo area */


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

	obj_descr[SPE_GRAVE].oc_name = "qabr";
	obj_descr[SPE_TUNNELS].oc_name = "tunnellar";
	obj_descr[SPE_FARMING].oc_name = "qishloq xo'jaligi";
	obj_descr[SPE_MOUNTAINS].oc_name = "tog'lar";
	obj_descr[SPE_DIVING].oc_name = "sho'ng'in";
	obj_descr[SPE_CRYSTALLIZATION].oc_name = "kristallanish";
	obj_descr[SPE_MOORLAND].oc_name = "tog'li erlar";
	obj_descr[SPE_URINE].oc_name = "siydik";
	obj_descr[SPE_QUICKSAND].oc_name = "bilqillama qumloq";
	obj_descr[SPE_STYX].oc_name = "stiks";
	obj_descr[SPE_ATTUNE_MAGIC].oc_name = "hamohang qilmoq sehrli";
	obj_descr[SPE_SNOW].oc_name = "qor";
	obj_descr[SPE_ASH].oc_name = "kul";
	obj_descr[SPE_SAND].oc_name = "qum";
	obj_descr[SPE_PAVING].oc_name = "slaydni yopish";
	obj_descr[SPE_HIGHWAY].oc_name = "avtomobil yo'li";
	obj_descr[SPE_GRASSLAND].oc_name = "chorvachilik";
	obj_descr[SPE_NETHER_MIST].oc_name = "o'tkir tuman";
	obj_descr[SPE_STALACTITE].oc_name = "sarkitit";
	obj_descr[SPE_CRYPT].oc_name = "yer ostidagi sag'ana";
	obj_descr[SPE_BUBBLE_BOBBLE].oc_name = "kabarcikli noto'g'ri ziplatma";
	obj_descr[SPE_RAIN].oc_name = "yomg'ir";
	obj_descr[SPE_GAIN_SPACT].oc_name = "kuch-qudratga ega bo'lish";
	obj_descr[SPE_METAL_GUARD].oc_name = "metall muhofazachi";
	obj_descr[SPE_MAGIC_WHISTLING].oc_name = "sehrli hushtak";

	obj_descr[SPE_ORE_MINING].oc_name = "ruda qazib olish";
	obj_descr[SPE_BOILER_KABOOM].oc_name = "qozon kabeli";
	obj_descr[SPE_DEFOG].oc_name = "ham tuman";
	obj_descr[SPE_SWAP_POSITION].oc_name = "almashtirish holati";
	obj_descr[SPE_SHUFFLE_MONSTER].oc_name = "aralashuvchi hayvon";
	obj_descr[SPE_PET_SYRINGE].oc_name = "uyda ishlatiladigan shprits";
	obj_descr[SPE_BUC_KNOWLEDGE].oc_name = "muborak la'nati la'natlangan bilim";
	obj_descr[SPE_PREACHING].oc_name = "voizlik qilish";
	obj_descr[SPE_RESIST_PARALYSIS].oc_name = "falajga qarshi turish";
	obj_descr[SPE_KEEP_SATIATION].oc_name = "to'yinishni saqlang";
	obj_descr[SPE_TECH_BOOST].oc_name = "texnologiyani kuchaytirish";
	obj_descr[SPE_CONTINGENCY].oc_name = "ehtimollik";
	obj_descr[SPE_AULE_SMITHING].oc_name = "juda kuchli soqchi ishi";
	obj_descr[SPE_HORSE_HOP].oc_name = "ot otdi";
	obj_descr[SPE_LINE_LOSS].oc_name = "yo'nalish yo'qolishi";
	obj_descr[SPE_TACTICAL_NUKE].oc_name = "taktik yadro raketasi";
	obj_descr[SPE_RAGNAROK].oc_name = "xudo tongi";
	obj_descr[SPE_ARMOR_SMASH].oc_name = "qurol-yarog'";
	obj_descr[SPE_STRANGLING].oc_name = "bo'g'ilib qolgan";
	obj_descr[SPE_PARTICLE_CANNON].oc_name = "zarracha to'pi";
	obj_descr[SPE_ONE_POINT_SHOOT].oc_name = "bitta nuqta olov";
	obj_descr[SPE_BLOOD_STREAM].oc_name = "qon oqimi";
	obj_descr[SPE_SHINING_WAVE].oc_name = "to'lqin to'lqinlari";
	obj_descr[SPE_GROUND_STOMP].oc_name = "er osti suvlari";
	obj_descr[SPE_DIRECTIVE].oc_name = "direktiv";
	obj_descr[SPE_NERVE_POISON].oc_name = "asab zahari";
	obj_descr[SPE_POWDER_SPRAY].oc_name = "chang spreyi";
	obj_descr[SPE_FIREWORKS].oc_name = "havo fisekleri";
	obj_descr[SPE_AIMBOT_LIGHTNING].oc_name = "bot botishini maqsad";
	obj_descr[SPE_ENHANCE_BREATH].oc_name = "nafasni kuchaytirish";
	obj_descr[SPE_GOUGE_DICK].oc_name = "o'ymok dik";
	obj_descr[SPE_BODYFLUID_STRENGTHENING].oc_name = "jasadni mustahkamlash";
	obj_descr[SPE_GEYSER].oc_name = "geyzer";
	obj_descr[SPE_BUBBLING_HOLE].oc_name = "qabariq teshik";
	obj_descr[SPE_PURIFICATION].oc_name = "tozalash";
	obj_descr[SPE_ADD_SPELL_MEMORY].oc_name = "josus xotira qo'shing";
	obj_descr[SPE_NEXUSPORT].oc_name = "teleportatsiya elementining porti";
	obj_descr[SPE_GIANT_FOOT].oc_name = "ulkan oyoq";
	obj_descr[SPE_ANTI_TELEPORTATION].oc_name = "qarshi teleportatsiya";
	obj_descr[SPE_SYMHEAL].oc_name = "simbiot shifo";

	/* todo area */

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

		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "old-fashioned cloak")) OBJ_DESCR(objects[i]) = "qadimgi plash";

		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "rifling power cloak")) OBJ_DESCR(objects[i]) = "miltig'idan tortib, kuch-quvvat plashi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cursed called cloak")) OBJ_DESCR(objects[i]) = "la'natlangan la'nati";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "failuncap cloak")) OBJ_DESCR(objects[i]) = "noto'g'ri plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "unostentatious cloak")) OBJ_DESCR(objects[i]) = "nostandart plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fourchan cloak")) OBJ_DESCR(objects[i]) = "to'rtburchak plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "erudite cloak")) OBJ_DESCR(objects[i]) = "eduit plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "inalish cloak")) OBJ_DESCR(objects[i]) = "ichki plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "zombified cloak")) OBJ_DESCR(objects[i]) = "zombi plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "transversal helmet")) OBJ_DESCR(objects[i]) = "ko'ndalangiga tushgan dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "grunter helmet")) OBJ_DESCR(objects[i]) = "go'shtli dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "precious helmet")) OBJ_DESCR(objects[i]) = "qimmatli dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cloudy helmet")) OBJ_DESCR(objects[i]) = "bulutli dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "post-nuclear helmet")) OBJ_DESCR(objects[i]) = "atom yadrosi zarbasi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "speechpipe helmet")) OBJ_DESCR(objects[i]) = "nutq kaskasi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "failuncap helmet")) OBJ_DESCR(objects[i]) = "noto'g'ri zarbdan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "breath control helmet")) OBJ_DESCR(objects[i]) = "nafasni boshqarish dubulg'asi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gas mask")) OBJ_DESCR(objects[i]) = "gaz niqobi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cold-blooded helmet")) OBJ_DESCR(objects[i]) = "sovuq qonli dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sages helmet")) OBJ_DESCR(objects[i]) = "do'stlar dubulg'asi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cuddly gloves")) OBJ_DESCR(objects[i]) = "jingalak qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "multilinguitis gloves")) OBJ_DESCR(objects[i]) = "ko'p tilli qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "demolition gloves")) OBJ_DESCR(objects[i]) = "buzilgan qo'lqoplar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "failuncap gloves")) OBJ_DESCR(objects[i]) = "noto'g'ri qo'lqop";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "rayductnay gloves")) OBJ_DESCR(objects[i]) = "nurli qo'lqoplar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "flower gloves")) OBJ_DESCR(objects[i]) = "gul qo'lqoplari";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gameble gloves")) OBJ_DESCR(objects[i]) = "o'yinchoq qo'lqoplari";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "bise gloves")) OBJ_DESCR(objects[i]) = "bosh o'pish sportchisi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "orgasm pumps")) OBJ_DESCR(objects[i]) = "orgazm nasoslari";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "worn-out sneakers")) OBJ_DESCR(objects[i]) = "eskirib qolgan sport poyafzali";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "imaginary heels")) OBJ_DESCR(objects[i]) = "xayoliy to'pi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "noble sandals")) OBJ_DESCR(objects[i]) = "oqlangan sandallar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "failuncap shoes")) OBJ_DESCR(objects[i]) = "poyafzal poyafzallari";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "flipflops")) OBJ_DESCR(objects[i]) = "sohil shippaklari";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "rumble boots")) OBJ_DESCR(objects[i]) = "yirtqich chiziqlar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "high iron boots")) OBJ_DESCR(objects[i]) = "yuqori temir chiziqlar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "doctor claw boots")) OBJ_DESCR(objects[i]) = "shifokor chiziqlari";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "spooky boots")) OBJ_DESCR(objects[i]) = "jingalak botinkalar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "feelgood heels")) OBJ_DESCR(objects[i]) = "his-tuyg'ulari baland";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gentle sneakers")) OBJ_DESCR(objects[i]) = "nozik poyafzallar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "plof heels")) OBJ_DESCR(objects[i]) = "buzilgan yurish ovozi to'piqlari";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "princess pumps")) OBJ_DESCR(objects[i]) = "malika nasoslari";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ballet heels")) OBJ_DESCR(objects[i]) = "balet poshnali";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "steel toed boots")) OBJ_DESCR(objects[i]) = "po'latdan yasalgan poyafzal";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "marji shoes")) OBJ_DESCR(objects[i]) = "oz maryam poyafzallari";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "mary janes")) OBJ_DESCR(objects[i]) = "meri janes";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "frayed cloak")) OBJ_DESCR(objects[i]) = "eskirgan plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "moist towelette")) OBJ_DESCR(objects[i]) = "nam suvog'i";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ratty fleece")) OBJ_DESCR(objects[i]) = "kalamush uchun belgilangan jundan";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "horrible christmas sweater")) OBJ_DESCR(objects[i]) = "qo'rqinchli cherepitsa";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "giant throwing star")) OBJ_DESCR(objects[i]) = "ulkan otish yulduzi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fearsome helmet")) OBJ_DESCR(objects[i]) = "qo'rqinchli dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fleshy wrap")) OBJ_DESCR(objects[i]) = "go'shtli sariq";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "feathery cloak")) OBJ_DESCR(objects[i]) = "tuklar plashi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "boiled gloves")) OBJ_DESCR(objects[i]) = "qaynatilgan qo'lqoplar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hairnet helmet")) OBJ_DESCR(objects[i]) = "sochni dubulg'asi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "glibskin helmet")) OBJ_DESCR(objects[i]) = "jo'shqin terisi zarbasi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "feminism dragonhide shield")) OBJ_DESCR(objects[i]) = "feminizm ajdarho qalqoni";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "negative dragonhide shield")) OBJ_DESCR(objects[i]) = "salbiy ajdaho qalqoni";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "heroic dragonhide shield")) OBJ_DESCR(objects[i]) = "qahramon ajdaho qalqoni";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sand cloak")) OBJ_DESCR(objects[i]) = "qum plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "shadow cloak")) OBJ_DESCR(objects[i]) = "soyali plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "grainy cloak")) OBJ_DESCR(objects[i]) = "donli plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "obsidian cloak")) OBJ_DESCR(objects[i]) = "qora shisha plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "volcanic cloak")) OBJ_DESCR(objects[i]) = "vulkanik plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "lead cloak")) OBJ_DESCR(objects[i]) = "qo'rg'oshin plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "different cloak")) OBJ_DESCR(objects[i]) = "har xil plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fuel cloak")) OBJ_DESCR(objects[i]) = "yonilg'i plashi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "chrome cloak")) OBJ_DESCR(objects[i]) = "xrom plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "silvermoon cloak")) OBJ_DESCR(objects[i]) = "kumushrang plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "pure cloak")) OBJ_DESCR(objects[i]) = "toza plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ceramic cloak")) OBJ_DESCR(objects[i]) = "sopol plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cloister cloak")) OBJ_DESCR(objects[i]) = "monastir plashi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "nano cloak")) OBJ_DESCR(objects[i]) = "nano ko'lamli plash";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sand helmet")) OBJ_DESCR(objects[i]) = "qum dubulg'asi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "shemagh")) OBJ_DESCR(objects[i]) = "arabcha bosh kiyim";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "shadow helmet")) OBJ_DESCR(objects[i]) = "soyali dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "obsidian helmet")) OBJ_DESCR(objects[i]) = "vulqon dubulg'asi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "lead helmet")) OBJ_DESCR(objects[i]) = "qo'rg'oshin dubulg'asi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "chrome helmet")) OBJ_DESCR(objects[i]) = "xrom dubulg'asi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "driver helmet")) OBJ_DESCR(objects[i]) = "haydovchi dubulg'asi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ceramic helmet")) OBJ_DESCR(objects[i]) = "seramika dubulg'asi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "serrated helmet")) OBJ_DESCR(objects[i]) = "jag'li dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "nano helmet")) OBJ_DESCR(objects[i]) = "nano o'lchovli dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "technical helmet")) OBJ_DESCR(objects[i]) = "texnik dubulg'a";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "beach gloves")) OBJ_DESCR(objects[i]) = "plyaj qo'lqoplari";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "paperweight gloves")) OBJ_DESCR(objects[i]) = "qog'ozli qo'lqoplar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "nano gloves")) OBJ_DESCR(objects[i]) = "nano o'lchovli qo'lqoplar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "metallic shield")) OBJ_DESCR(objects[i]) = "metall qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "black shield")) OBJ_DESCR(objects[i]) = "qora qalqon";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "lovely boots")) OBJ_DESCR(objects[i]) = "yoqimli etiklar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "feminine sandals")) OBJ_DESCR(objects[i]) = "nazokatli sandal";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "everlasting boots")) OBJ_DESCR(objects[i]) = "abadiy etiklar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ether boots")) OBJ_DESCR(objects[i]) = "etik etiklari";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "elean boots")) OBJ_DESCR(objects[i]) = "elflarning yaponcha etiklari";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "thick boots")) OBJ_DESCR(objects[i]) = "qalin etiklar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "brittle boots")) OBJ_DESCR(objects[i]) = "mo'rt etiklar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sand-als")) OBJ_DESCR(objects[i]) = "qum kabi";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "shadowy heels")) OBJ_DESCR(objects[i]) = "soyali poshnalar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "reflective slippers")) OBJ_DESCR(objects[i]) = "yansitici terlik";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "lead boots")) OBJ_DESCR(objects[i]) = "qo'rg'oshin etiklari";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "weight attachment boots")) OBJ_DESCR(objects[i]) = "vazn biriktiruvchi etiklar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "chrome boots")) OBJ_DESCR(objects[i]) = "krom botinkalari";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fungal sandals")) OBJ_DESCR(objects[i]) = "qo'ziqorin sandallari";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "porcelain sandals")) OBJ_DESCR(objects[i]) = "chinni sandal";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "3D-printed shoes")) OBJ_DESCR(objects[i]) = "uch o'lchamli bosma poyabzal";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "3D-printed sandals")) OBJ_DESCR(objects[i]) = "uch o'lchamli bosma sandal";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "3D-printed wedges")) OBJ_DESCR(objects[i]) = "uch o'lchamli bosilgan takozlar";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "standing footwear")) OBJ_DESCR(objects[i]) = "tik turgan poyafzal";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cancel dragonhide shield")) OBJ_DESCR(objects[i]) = "ajdarhid qalqonini bekor qiling";
		if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "corona dragonhide shield")) OBJ_DESCR(objects[i]) = "ajdaho qalqoni";

		/* todo area */


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

	if (new_game && Race_if(PM_MAGYAR)) u.weapon_slots += 1000;
	if (new_game && Race_if(PM_YUGGER)) u.weapon_slots += 15;
	if (new_game && Race_if(PM_RUSMOT)) u.weapon_slots += 5;
	if (new_game) u.weapon_slots += 2;

	if (new_game) u.zapem_mode = 0;
	if ((flags.zapem || Role_if(PM_SOFTWARE_ENGINEER) || Role_if(PM_CRACKER) || Role_if(PM_JANITOR) || Role_if(PM_SPACE_MARINE) || Role_if(PM_STORMBOY) || Role_if(PM_YAUTJA) || Role_if(PM_QUARTERBACK) || Role_if(PM_PSYKER) || Role_if(PM_EMPATH) || Role_if(PM_MASTERMIND) || Role_if(PM_WEIRDBOY) || Role_if(PM_ASTRONAUT) || Race_if(PM_RETICULAN) || Race_if(PM_OUTSIDER) || Role_if(PM_CYBERNINJA) || Role_if(PM_DISSIDENT) || Role_if(PM_XELNAGA)) && new_game && !flags.wonderland && !flags.lostsoul && !flags.uberlostsoul) {

		if (!flags.zapem) u.zapem_mode = 1;

		d_level zapemlevel;

		zapemlevel.dnum = dname_to_dnum("Space Base");
		zapemlevel.dlevel = dungeons[zapemlevel.dnum].entry_lev;

		flags.lostsoul = flags.uberlostsoul = flags.wonderland = FALSE;

		goto_level(&zapemlevel, TRUE, FALSE, FALSE);

		u.youaredead = 0;
		u.youarereallydead = 0;
		save_currentstate();
		pline("You find yourself in a derelict spaceship. In order to get out, you have to make it at least to the bottom of the Sewer Plant. But watch out: once you leave, the monster difficulty inside the Space Base and its sub-branches will increase to reflect its actual depth.");

	}

	if (flags.wonderland && new_game) {
		flags.lostsoul = flags.uberlostsoul = FALSE;
		goto_level(&elderpriest_level, TRUE, FALSE, FALSE);
		u.youaredead = 0;
		u.youarereallydead = 0;
		save_currentstate();
		pline("Welcome to Wonderland. You have to get to the bottom of the Yendorian Tower to escape.");
		pline("If you manage to do that, you regain your ability to levelport. Good luck, and don't get near the elder priest or he will tentacle to tentacle you!");

		/* Apparently the game is so stupid and doesn't know what a teleport region is, and still occasionally
		 * spawns you inside the temple! This should not be, and therefore you're teleported out if it happens. */
		if (*in_rooms(u.ux, u.uy, TEMPLE) || (u.ux > 74) ) {
			pline("S'Wonderful!");
			(void) safe_teleds(FALSE);
			/* still in the temple? try again, but prevent infinite loops */
			while (rn2(1000) && (*in_rooms(u.ux, u.uy, TEMPLE) || (u.ux > 74)) ) (void) safe_teleds(FALSE);
		}

	}

	if (flags.lostsoul && !flags.uberlostsoul && new_game) { 

		goto_level(&medusa_level, TRUE, FALSE, FALSE); /* inspired by Tome, an Angband mod --Amy */
		u.youaredead = 0;
		u.youarereallydead = 0;
		save_currentstate();
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
		u.youaredead = 0;
		u.youarereallydead = 0;
		save_currentstate();
		pline("Enjoy your stay, and try to get out if you can.");


	}

	if (Race_if(PM_HC_ALIEN) && new_game && !flags.female) {
		makeknown(AMULET_OF_CHANGE);
		pline("The gods don't allow you to be male.");
		change_sex();
		flags.botl = 1;

	}

	if (Role_if(PM_TRANSVESTITE) && !Race_if(PM_HC_ALIEN) && new_game && flags.female) {
		makeknown(AMULET_OF_CHANGE);
		You("don't feel like being female!");
		change_sex();
		flags.botl = 1;

	}

	if (Role_if(PM_EMERA) && !Race_if(PM_HC_ALIEN) && new_game && flags.female) {
		makeknown(AMULET_OF_CHANGE);
		You("don't feel like being female!");
		change_sex();
		flags.botl = 1;

	}

	if (Role_if(PM_LADIESMAN) && !Race_if(PM_HC_ALIEN) && new_game && flags.female) {
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

	if (Role_if(PM_HUSSY) && new_game && !flags.female) {
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

	if (new_game && (Role_if(PM_PRACTICANT) || isbeacher)) pline("%s thunders: 'Alright %s you little practicant maggot, get ready for another hard day of work! You're late by 5 minutes but that's your loss! If you do your work properly this time you'll not get any problems with me, but if you step out of line you'll pay zorkmids for each of your offenses!'", noroelaname(), playeraliasname);

	u.stethocheat = moves;
	init_uasmon();

	/* Amy switch to invalidate older versions */
#if 0
	if (moves < 100) {
		pline("Sorry, but this version of SLASH'EM Extended is outdated. Your current game ends now. Please start a new game on the up-to-date version, because this old version exists only to allow players to finish old runs and you don't seem to have an old savegame. If you feel that this is in error, get on the #em.slashem.me IRC channel on Freenode and complain to Amy.");
		done(QUIT);
	}
#endif

	if (!new_game && (u.uhp <= 0 && (!Upolyd || u.mh <= 0))) {

		u.youaredead = 1;
		You("were not healthy enough to survive restoration.");
		killer_format = NO_KILLER_PREFIX;
		killer = "restored the game while having no hit points left";
		done(DIED);
		u.youaredead = 0;
	}

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

	if (!new_game && uinsymbiosis && u.usymbiote.mhp < 0) {
		u.usymbiote.active = 0;
		u.usymbiote.mnum = PM_PLAYERMON;
		u.usymbiote.mhp = 0;
		u.usymbiote.mhpmax = 0;
		u.usymbiote.cursed = u.usymbiote.hvycurse = u.usymbiote.prmcurse = u.usymbiote.bbcurse = u.usymbiote.morgcurse = u.usymbiote.evilcurse = u.usymbiote.stckcurse = 0;
		u.cnd_symbiotesdied++;
		You("filthy hangup cheater tried to cheat past the fact that your symbiote died, and therefore it gets declared dead now. I hope you're ashamed of yourself, cheater :P --Amy");
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

	if (!new_game && u.youarereallydead) {

		pline("You are dead. Filthy cheater. Don't try to abuse the hangup function again, you hear?");
		killer = "trying to circumvent an instadeath via hangup cheating";
		killer_format = KILLED_BY;
		done(DIED);
		/* lifesaved */
		killer = "trying to be a wiseguy";
		killer_format = KILLED_BY;
		done(DIED);

		if (!wizard) done(ESCAPED);

		/* should only get here if wizard */
		You_feel("like a very cheaty wizard.");

		u.youaredead = 0;
		u.youarereallydead = 0;

	}

	if (!new_game && u.datadeletedefer) {
		pline("Oh no, you're not gonna hangup cheat past a data delete effect.");
		datadeleteattack();
		u.datadeletedefer = 0;
	}

	if (u.segfaultpanic) {
		u.youaredead = 1;
		u.youarereallydead = 1;
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

	if (!new_game && u.freeplaytransit) { /* ch3at0r! */
		u.freeplaymode = TRUE;
		u.freeplaytransit = TRUE;
		u.freeplayplanes = FALSE;

		if (u.uhave.amulet) { /* no longer need the amulet, now that you've won */
			struct obj *otmpi, *otmpii;
			if (invent) {
				for (otmpi = invent; otmpi; otmpi = otmpii) {
				      otmpii = otmpi->nobj;
					if (otmpi->otyp == AMULET_OF_YENDOR) {							
						if (otmpi->owornmask) {
							setnotworn(otmpi);
						}
						dropx(otmpi);
					}
				}
			}
		}
		goto_level(&medusa_level, TRUE, FALSE, FALSE);

		register int newlevX = 1;
		d_level newlevelX;
		get_level(&newlevelX, newlevX);
		goto_level(&newlevelX, TRUE, FALSE, FALSE);
		u.freeplaytransit = FALSE;
		pline("You find yourself back in the dungeon. Since you've officially won the game, you can freely explore now. If you want to go back to the Elemental Planes, you have to visit Moloch's Sanctum first. You can also retire (commit suicide) when you are ready.");
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
	if (isfriday && !rn2(10)) return FALSE; /* unconditional failure on the unlucky day */

	int chance = 133;
	chance -= (moves * 100 / u.monstertimefinish);
	/* make sure we don't fall off the bottom */
	if (chance < 33) chance = 33;

	if (moves > 20000 && !rn2(2)) {
		chance *= 19;
		chance /= 20;
	}

	if (moves > 40000 && !rn2(2)) {
		chance *= 19;
		chance /= 20;
	}

	if (moves > 60000 && !rn2(2)) {
		chance *= 19;
		chance /= 20;
	}

	if (moves > 80000 && !rn2(2)) {
		chance *= 19;
		chance /= 20;
	}

	if (moves > 100000 && !rn2(2)) {
		chance *= 19;
		chance /= 20;
	}

	if (moves > 120000 && !rn2(2)) {
		chance *= 19;
		chance /= 20;
	}

	if (moves > 140000 && !rn2(2)) {
		chance *= 19;
		chance /= 20;
	}

	if (moves > 160000 && !rn2(2)) {
		chance *= 19;
		chance /= 20;
	}

	if (moves > 180000 && !rn2(2)) {
		chance *= 19;
		chance /= 20;
	}

	if (moves > 200000 && !rn2(2)) {
		chance *= 19;
		chance /= 20;
	}

	if (In_quest(&u.uz)) {
		chance /= rnd(2);
	}

	if ((In_sheol(&u.uz) || In_angmar(&u.uz) || In_swimmingpool(&u.uz) || In_hellbathroom(&u.uz) || In_gehennom(&u.uz) || In_frnkn(&u.uz)) && !rn2(2)) {
		chance *= 2;
		chance /= rn1(2,2);
	}

	if (In_yendorian(&u.uz) && !(flags.wonderland && !(u.wonderlandescape)) && !rn2(2) ) {
		chance *= 3;
		chance /= rn1(3,3);
	}

	if ((In_forging(&u.uz) || In_ordered(&u.uz) || In_deadground(&u.uz) || In_voiddungeon(&u.uz) || In_netherrealm(&u.uz)) && !rn2(2) ) {
		chance *= 3;
		chance /= rn1(3,3);
	}

	if (In_subquest(&u.uz) || In_bellcaves(&u.uz)) {
		chance /= rnd(5);
	}

	if (chance < 10) chance = 10; /* always at least a 10% chance of getting it --Amy */

	if (chance > rnd(100)) return(TRUE); /* the effect will happen despite the lower chance */
	else return(FALSE);
}

/* contamination check: reduced HP/Pw regeneration depending on how contaminated the player is --Amy */
boolean
contaminationcheck()
{
	/* piercer only regenerates when hidden */
	if (Race_if(PM_PIERCER) && !u.uundetected) return TRUE;

	if (u.contamination) {
		if (u.contamination >= 1000 && rn2(10)) return 1;
		else if (u.contamination >= 800 && u.contamination < 1000 && (rnd(10) > 3)) return 1;
		else if (u.contamination >= 600 && u.contamination < 800 && rn2(2)) return 1;
		else if (u.contamination >= 400 && u.contamination < 600 && (rnd(10) > 7)) return 1;
		else if (u.contamination >= 200 && u.contamination < 400 && !rn2(10)) return 1;
	}

	/* "nasty" equipment also reduces regeneration, more if you have several pieces equipped --Amy */
	if (uarmc && uarmc->otyp == NASTY_CLOAK && !rn2(5)) return 1;
	if (uarm && uarm->otyp == ROBE_OF_NASTINESS && !rn2(5)) return 1;
	if (uarmh && uarmh->otyp == UNWANTED_HELMET && !rn2(5)) return 1;
	if (uarmg && uarmg->otyp == EVIL_GLOVES && !rn2(5)) return 1;
	if (uarmf && uarmf->otyp == UNFAIR_STILETTOS && !rn2(5)) return 1;
	if (uarm && uarm->otyp == EVIL_DRAGON_SCALE_MAIL && !rn2(5)) return 1;
	if (uarm && uarm->otyp == EVIL_DRAGON_SCALES && !rn2(5)) return 1;
	if (uarms && uarms->otyp == EVIL_DRAGON_SCALE_SHIELD && !rn2(5)) return 1;
	if (uarms && uarms->otyp == DIFFICULT_SHIELD && !rn2(5)) return 1;
	if (uarmu && uarmu->otyp == BAD_SHIRT && !rn2(5)) return 1;
	if (uarm && uarm->otyp == EVIL_PLATE_MAIL && !rn2(5)) return 1;
	if (uarm && uarm->otyp == EVIL_LEATHER_ARMOR && !rn2(5)) return 1;

	/* if you're in the Sewer Plant and have to breathe, your regeneration is also reduced */
	if (In_sewerplant(&u.uz) && rn2(2) && !Breathless) return 1;

	return 0;
}

/* cyan spell trap: highlight tiles that have at least one adjacent monster but don't have a monster themselves --Amy */
boolean
cyanspellok(x, y)
int x, y;
{
	if (MON_AT(x, y)) return FALSE;

	int i, j;
	for (i = -1; i <= 1; i++) for(j = -1; j <= 1; j++) {
		if (!isok(x + i, y + j)) continue;
		if (MON_AT(x + i, y + j)) return TRUE;
	}

	return FALSE;

}

int
speedbonus(amount, max)
int amount, max;
{
	if (amount > max) amount = max;
	return amount;
}

STATIC_PTR void
do_megafloodingf(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	if (Aggravate_monster) {
		u.aggravation = 1;
		reset_rndmonst(NON_PM);
	}

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			if (rn2(4)) {
				levl[randomx][randomy].typ = MOAT;
				makemon(mkclass(S_EEL,0), randomx, randomy, NO_MM_FLAGS);
			} else {
				levl[randomx][randomy].typ = LAVAPOOL;
				makemon(mkclass(S_FLYFISH,0), randomx, randomy, NO_MM_FLAGS);
			}

			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */

		if (rn2(4)) {
			levl[x][y].typ = MOAT;
			makemon(mkclass(S_EEL,0), x, y, NO_MM_FLAGS);
		} else {
			levl[x][y].typ = LAVAPOOL;
			makemon(mkclass(S_FLYFISH,0), x, y, NO_MM_FLAGS);
		}

		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

	u.aggravation = 0;

}

STATIC_PTR void
do_fjordefloodingf(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	if (Aggravate_monster) {
		u.aggravation = 1;
		reset_rndmonst(NON_PM);
	}

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = MOAT;
			if (!rn2(3)) makemon(mkclass(S_EEL,0), randomx, randomy, NO_MM_FLAGS);

			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */

		levl[x][y].typ = MOAT;
		if (!rn2(3)) makemon(mkclass(S_EEL,0), x, y, NO_MM_FLAGS);

		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

	u.aggravation = 0;

}

STATIC_OVL void
pumpsminigame()
{
	int minigameturns = 0;
	boolean yourturn = FALSE;
	int pumpsstate = 0;
	int pumpslikeyou = 0;
	int pumpshealth = 100;

	int yourdamagedeal;
	int yourstrength;
	if (ACURR(A_STR) <= 18) yourstrength = 18;
	else if (ACURR(A_STR) <= STR19(19)) yourstrength = 19;
	else if (ACURR(A_STR) <= STR19(20)) yourstrength = 20;
	else if (ACURR(A_STR) <= STR19(21)) yourstrength = 21;
	else if (ACURR(A_STR) <= STR19(22)) yourstrength = 22;
	else if (ACURR(A_STR) <= STR19(23)) yourstrength = 23;
	else if (ACURR(A_STR) <= STR19(24)) yourstrength = 24;
	else yourstrength = 25;

#define PUMPSCRATCHING	1
#define PUMPBASHING	2
#define PUMPKICKINNUTS	3
#define PUMPTOESTOMP	4
#define PUMPIDLE	5
#define PUMPINLAP	6

	pline("In this minigame, you and the pair of lady pumps will take turns alternately. There are various conditions that you can reach to end the game. Good luck!");

newturn:
	if (!yourturn) { /* it's the pumps' turn */

		if (pumpsstate == 0 || !rn2(5)) pumpsstate = rnd(minigameturns >= 10 ? 6 : 5);
		if (pumpsstate == PUMPKICKINNUTS && flags.female) {
			pumpsstate = rnd(3); /* females don't have nuts (DUH) */
			if (pumpsstate == 3) pumpsstate = 4;
		}

		if (rn2(25) < ((pumpslikeyou > 10) ? 10 : pumpslikeyou) ) pumpsstate = PUMPINLAP;

		switch (pumpsstate) {

			case PUMPSCRATCHING:

				pline("The sexy leather pumps scratch up and down your legs with their heels!");

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
				losehp(u.legscratching, "endorphic leg scratches", KILLED_BY);
				u.legscratching++;
				pumpslikeyou--;
				if (u.legscratching >= 20) pumpslikeyou--;

				break;
			case PUMPBASHING:

				pline("Klock! The heel slams on your %s, producing a beautiful sound.", body_part(HEAD));
				losehp(rnd(20),"being bashed on the head by an orgasm pump",KILLED_BY);
				if (!rn2(3)) pumpslikeyou++;

				break;
			case PUMPKICKINNUTS:

				if (rnd(30) > ACURR(A_CHA)) {

					pline("The sexy leather pumps painfully drive the lovely cone heel into your nuts, and you moan in agony!");

					losehp(rnd(monster_difficulty() + 20),"being kicked in the nuts by a sexy leather pump",KILLED_BY);
					pumpslikeyou -= 3;

				} else {

					pline("The sexy leather pumps kick you in the nuts with their lovely cone heel, and you moan in lust due to the intense pain!");

					losehp(rnd(10),"being kicked in the nuts by a sexy leather pump",KILLED_BY);
					pumpslikeyou += rnd(2);

				}

				break;
			case PUMPTOESTOMP:

				pline("The sexy leather pumps stomp your toes with their lovely heels!");
				losehp(rnd(5),"having their toes stomped by sexy leather pumps",KILLED_BY);

				if (!rn2(10) && (rnd(30) > ACURR(A_CON)) ) {
					pline("Your defenseless %s was crushed underneath the very sexy heel!", body_part(TOE));
					if (u.uhpmax < 2) {
						u.youaredead = 1;
						pline("You break down unconscious, and the pumps proceed to stomp you to death even though they look so lovely.");
						killer = "being crushed underneath high-heeled leather pumps";
						killer_format = KILLED_BY;
						done(DIED);
						u.youaredead = 0;
						return;
					} else {
						u.uhpmax--;
						if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
					}
				}

				break;
			case PUMPIDLE:

				pline("The sexy leather pumps just stand there and look pretty.");
				break;
			case PUMPINLAP:
				pline("The sexy lady pumps are resting in your lap comfortably.");

				if (Upolyd) u.mh += rnd(5); /* heal some hit points */
				else u.uhp += rnd(5); /* heal some hit points */
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				if (u.mh > u.mhmax) u.mh = u.mhmax;

				pumpslikeyou++;

				if ((pumpslikeyou >= rn1(10, 10)) && !rn2(5)) {

					if (Upolyd) u.mh += 200; /* heal some hit points */
					else u.uhp += 200; /* heal some hit points */
					if (Upolyd) u.mhmax++;
					u.uhpmax++;
					if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
					if (u.mh > u.mhmax) u.mh = u.mhmax;
					pline("Both you and the sexy leather pumps reached their climax, and your maximum health increases!");
					pline("The sexy leather pumps congratulate you, and are looking forward to playing with you again.");

					return;
				}

				break;

			default:
				impossible("unknown action for sexy leather pumps minigame %d", pumpsstate);
				pumpsstate = PUMPIDLE;
				break;

		}
		yourturn = TRUE;

	} else { /* it's your turn */
		winid tmpwin;
		anything any;
		menu_item *selected;
		int n;

		any.a_void = 0;         /* zero out all bits */
		tmpwin = create_nhwindow(NHW_MENU);
		start_menu(tmpwin);
		any.a_int = 1;
		add_menu(tmpwin, NO_GLYPH, &any , 'g', 0, ATR_NONE, "Gently caress", MENU_UNSELECTED);
		any.a_int = 2;
		add_menu(tmpwin, NO_GLYPH, &any , 'p', 0, ATR_NONE, "Punch", MENU_UNSELECTED);
		any.a_int = 3;
		add_menu(tmpwin, NO_GLYPH, &any , 't', 0, ATR_NONE, "Try to put them on again", MENU_UNSELECTED);
		any.a_int = 4;
		add_menu(tmpwin, NO_GLYPH, &any , 'r', 0, ATR_NONE, "Run in circles", MENU_UNSELECTED);
		if (pumpslikeyou >= 5) {
			any.a_int = 5;
			add_menu(tmpwin, NO_GLYPH, &any , 'k', 0, ATR_NONE, "Kiss", MENU_UNSELECTED);
		}

		end_menu(tmpwin, "What do you do?");
		n = select_menu(tmpwin, PICK_ONE, &selected);
		destroy_nhwindow(tmpwin);

		if (n > 0) {
			switch (selected[0].item.a_int) {
				case 1:
					pline("You gently caress the wonderful high heels using %s %s.", !rn2(3) ? "both your left and right" : rn2(2) ? "your left" : "your right", body_part(HAND) );
					if ((pumpslikeyou > rnd(5) && !rn2(2)) || (ACURR(A_CHA) > rnd(40)) ) {
						pumpslikeyou++;
						if (rn2(5)) pumpsstate = PUMPINLAP;
						pline("The sexy leather pumps seem to love you!");
					} else if (pumpslikeyou < 0) {
						pline("Unfortunately the sexy leather pumps still hate you passionately...");
					} else {
						pline("The sexy leather pumps seem to like your caressing touch.");
						if (!rn2(10)) pumpsstate = PUMPIDLE;
					}
					break;
				case 2:
					if (ACURR(A_DEX) < rnd(25) && ACURR(A_DEX) < rnd(25)) {
						pline("Ouch - you punched the hard, unyielding cone heel!");
						losehp(rnd(4),"punching a massive cone heel",KILLED_BY);
					} else if (rnd(yourstrength) > 10) {
						yourdamagedeal = rnd(yourstrength);
						pumpshealth -= yourdamagedeal;
						if (yourdamagedeal < 6) pline("It's not very effective...");
						else if (yourdamagedeal < 16) pline("You landed a regular hit.");
						else pline("It's super effective!");

						if (pumpshealth < 0) pumpshealth = 0;
						if (pumpshealth < 20) pline("The sexy leather pumps are weak! Go get 'em!");
						pumpslikeyou--;
						if (!rn2(3)) pumpsstate = rnd(4);

					} else {
						pline("The soft leather does not seem to budge at all...");
						if (!rn2(3)) pumpslikeyou--;
						pumpsstate = rnd(4);
					}
					break;
				case 3:
					if (rnd(yourstrength) > rn1(10,10) && rnd(yourstrength) > rn1(10,10) && rnd(yourstrength) > rn1(10,10) && rnd(yourstrength) > rn1(10,10) && rnd(yourstrength) > rn1(10,10) && rnd(yourstrength) > rn1(10,10) && rnd(yourstrength) > rn1(10,10) && rnd(yourstrength) > rn1(10,10) && rnd(yourstrength) > rn1(10,10) && rnd(yourstrength) > rn1(10,10)) { /* you win */
						pline("You completely surprised the sexy leather pumps and managed to slip your %s into them!", makeplural(body_part(FOOT)));
						pline("As a reward, the sexy leather pumps magically boost your strength!");
						gainstr((struct obj *)0, 0);
						return;
					} else if (rnd(yourstrength) > pumpshealth) { /* they're out of health, you win */
						pline("Your %s quickly snatch the lovely lady pumps and you manage to slip into them before they can fight back.", makeplural(body_part(HAND)));
						pline("Congratulations, you won! Your dexterity increases.");
						(void) adjattrib(A_DEX, 1, -1, TRUE);
						return;
					} else {
						if (!rn2(3)) pumpslikeyou--;
						pline("The sexy leather pumps quickly evade your grasp and stomp on your %s with their lovely high heel.", body_part(FINGER));
						if (!rn2(4)) losehp(rnd(4),"having their fingers crushed underneath cone-heeled lady pumps",KILLED_BY);
						if (!rn2(5) && rnd(30) > ACURR(A_CON)) {
							if (u.uhpmax < 2) {
								u.youaredead = 1;
								pline("The pain is unbearable... apparently the incredibly cute heel broke your bones. While you're groaning in pain, the high heel proceeds to successively crush all of your remaining body parts.");
								killer = "having their fingers stomped by sexy leather pumps";
								killer_format = KILLED_BY;
								done(DIED);
								u.youaredead = 0;
								return;

							} else {
								u.uhpmax--;
								if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
							}
						}
					}
					break;
				case 4:
					if (rnd(30) < ACURR(A_DEX) && rnd(30) < ACURR(A_DEX)) {
						pline("It seems that your constant movement makes the sexy leather pumps slightly dizzy.");
						if (pumpslikeyou < 0) pumpslikeyou++;
						if (rn2(3)) pumpsstate = PUMPIDLE;
					} else {
						pline("Your fancy footwork didn't fool the lovely leather pumps.");
						pumpsstate = rnd(4);
					}
					break;
				case 5:
					if (pumpsstate == PUMPINLAP) {
						pline("Aww, the lovely leather pumps seem to really like being kissed by you!");
						pumpslikeyou++;
					} else if (rnd(30) < ACURR(A_CHA)) {
						pline("The sexy leather pumps seem to become calmer as you kiss them!");
						pumpsstate = PUMPINLAP;
						pumpslikeyou++;
					} else pline("The sexy leather pumps brush your mouth away.");
					break;
				default:
					pline("You decide to do nothing.");
					break;
			}
		}
		yourturn = FALSE;
		minigameturns++;

	}
	if (yourturn) goto newturn;

	if ((pumpslikeyou > 0) && rnd(pumpslikeyou) > 9) {
		pline("The sexy leather pumps are satisfied, and offer you to end the fight.");
		if (yn("Do you accept the offer and end the fight?") == 'y') {
			pline("You are gently stroked by the tender cone heels, and as you put them on again, you feel very pretty!");
			(void) adjattrib(A_CHA, 1, -1, TRUE);
			return;
		}
	}

	if (pumpslikeyou < -9 && !rn2(5)) pumpsstate = PUMPSCRATCHING;

	if (minigameturns >= 15 && !rn2(pumpsstate == PUMPSCRATCHING ? 100 : 5)) {
		if (pumpslikeyou < 0) {
			pline("The sexy leather pumps hate you bitterly... You can try to run away from the fight.");
			if (yn("Do you try to run?") == 'y') {
				if (rnd(20) > ACURR(A_DEX)) {
					pline("Unfortunately the very lovely heels catch you, and slam on your head with full force.");
					if (ABASE(A_INT) <= ATTRMIN(A_INT)) {
						u.youaredead = 1;
						pline("The incredibly tender cone heels split your skull. You die.");
						killer = "having their head crushed by a high-heeled leather pump";
						killer_format = KILLED_BY;
						done(DIED);
						u.youaredead = 0; /* lifesaving allowed, unlike mind flayer instadeath */
						return;

					} else {
						adjattrib(A_INT, -rnd(2), FALSE, TRUE);
						if (!rn2(issoviet ? 2 : 3)) forget_levels(rnd(issoviet ? 25 : 10));
						if (!rn2(issoviet ? 3 : 5)) forget_objects(rnd(issoviet ? 25 : 10));
						exercise(A_WIS, FALSE);
					}

				} else {
					pline("Got away safely!");
					return;
				}
			}

		} else {
			pline("You can try to escape the fight.");
			if (yn("Do you want to escape?") == 'y') {
				pline("The fight with the sexy leather pumps has ended.");
				return;

			}
		}
	}

	goto newturn;

}

#endif /* OVLB */

/*allmain.c*/
