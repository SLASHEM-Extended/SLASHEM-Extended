/*	SCCS Id: @(#)dig.c	3.4	2003/03/23	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "edog.h"
/* #define DEBUG */	/* turn on for diagnostics */

#ifdef OVLB

static NEARDATA boolean did_dig_msg;

STATIC_DCL boolean rm_waslit(void);
STATIC_DCL void mkcavepos(XCHAR_P,XCHAR_P,int,BOOLEAN_P,BOOLEAN_P);
STATIC_DCL void mkcavearea(BOOLEAN_P);
STATIC_DCL int dig_typ(struct obj *,XCHAR_P,XCHAR_P);
STATIC_DCL int dig(void);
STATIC_DCL schar fillholetyp(int, int);
STATIC_DCL void dig_up_grave(void);

/* Indices returned by dig_typ() */
#define DIGTYP_UNDIGGABLE 0
#define DIGTYP_ROCK       1
#define DIGTYP_STATUE     2
#define DIGTYP_BOULDER    3
#define DIGTYP_DOOR       4
#define DIGTYP_TREE       5
#define DIGTYP_IRONBAR    6
#define DIGTYP_FIREAXE    7
/* keep d_action and d_target in sync!!! otherwise, CRASH with illegible backtrace "?? at 07f49320" or something --Amy */

STATIC_OVL boolean
rm_waslit()
{
    register xchar x, y;

    if(levl[u.ux][u.uy].typ == ROOM && levl[u.ux][u.uy].waslit)
	return(TRUE);
    for(x = u.ux-2; x < u.ux+3; x++)
	for(y = u.uy-1; y < u.uy+2; y++)
	    if(isok(x,y) && levl[x][y].waslit) return(TRUE);
    return(FALSE);
}

/* Change level topology.  Messes with vision tables and ignores things like
 * boulders in the name of a nice effect.  Vision will get fixed up again
 * immediately after the effect is complete.
 */
STATIC_OVL void
mkcavepos(x, y, dist, waslit, rockit)
    xchar x,y;
    int dist;
    boolean waslit, rockit;
{
    register struct rm *lev;

    if(!isok(x,y)) return;
    lev = &levl[x][y];

    if(rockit) {
	register struct monst *mtmp;

	if(IS_ROCK(lev->typ)) return;
	if(t_at(x, y)) return; /* don't cover the portal */
	if ((mtmp = m_at(x, y)) != 0)	/* make sure crucial monsters survive */
	    if(!passes_walls(mtmp->data) && (!mtmp->egotype_wallwalk)) (void) rloc(mtmp, FALSE);
    } else if(lev->typ == ROOM) return;

    unblock_point(x,y);	/* make sure vision knows this location is open */
	if (!(levl[x][y].wall_info & W_HARDGROWTH)) levl[x][y].wall_info |= W_EASYGROWTH;

    /* fake out saved state */
    lev->seenv = 0;
    lev->doormask = 0;
    if(dist < 3) lev->lit = (rockit ? FALSE : TRUE);
    if(waslit) lev->waslit = (rockit ? FALSE : TRUE);
    lev->horizontal = FALSE;
    viz_array[y][x] = (dist < 3 ) ?
	(IN_SIGHT|COULD_SEE) : /* short-circuit vision recalc */
	COULD_SEE;
    lev->typ = (rockit ? STONE : ROOM);
    if(dist >= 3)
	impossible("mkcavepos called with dist %d", dist);
    if(Blind)
	feel_location(x, y);
    else newsym(x,y);
}

STATIC_OVL void
mkcavearea(rockit)
register boolean rockit;
{
    int dist;
    xchar xmin = u.ux, xmax = u.ux;
    xchar ymin = u.uy, ymax = u.uy;
    register xchar i;
    register boolean waslit = rm_waslit();

    if(rockit) pline("Crash!  The ceiling collapses around you!");
    else pline("A mysterious force %s cave around you!",
	     (levl[u.ux][u.uy].typ == CORR) ? "creates a" : "extends the");
    display_nhwindow(WIN_MESSAGE, TRUE);

    for(dist = 1; dist <= 2; dist++) {
	xmin--; xmax++;

	/* top and bottom */
	if(dist < 2) { /* the area is wider that it is high */
	    ymin--; ymax++;
	    for(i = xmin+1; i < xmax; i++) {
		mkcavepos(i, ymin, dist, waslit, rockit);
		mkcavepos(i, ymax, dist, waslit, rockit);
	    }
	}

	/* left and right */
	for(i = ymin; i <= ymax; i++) {
	    mkcavepos(xmin, i, dist, waslit, rockit);
	    mkcavepos(xmax, i, dist, waslit, rockit);
	}

	flush_screen(1);	/* make sure the new glyphs shows up */
	delay_output();
    }

    if(!rockit && levl[u.ux][u.uy].typ == CORR) {
	levl[u.ux][u.uy].typ = /*ROOM*/CORR;
	if(waslit) levl[u.ux][u.uy].waslit = TRUE;
	newsym(u.ux, u.uy); /* in case player is invisible */
    }

    vision_full_recalc = 1;	/* everything changed */
}

/* When digging into location <x,y>, what are you actually digging into? */
STATIC_OVL int
dig_typ(otmp, x, y)
struct obj *otmp;
xchar x, y;
{
	boolean ispick = is_pick(otmp);
	boolean isantibar = is_antibar(otmp);
	boolean issaber = is_lightsaber(otmp);

	if (otmp->oartifact == ART_BREAK_EVERYTHING && Role_if(PM_FIREFIGHTER)) {
		if (IS_IRONBAR(levl[x][y].typ) || IS_TREE(levl[x][y].typ) || IS_FARMLAND(levl[x][y].typ) || IS_MOUNTAIN(levl[x][y].typ)) {

			return DIGTYP_FIREAXE;
		}
	}

	return ((ispick
		|| issaber
		) && sobj_at(STATUE, x, y) ? DIGTYP_STATUE :
		(isantibar && IS_IRONBAR(levl[x][y].typ)) ? DIGTYP_IRONBAR :
		(ispick
		|| issaber
		) && sobj_at(BOULDER, x, y) ? DIGTYP_BOULDER :
		closed_door(x, y) ? DIGTYP_DOOR :
		IS_TREE(levl[x][y].typ) ?
			(ispick ? DIGTYP_UNDIGGABLE : DIGTYP_TREE) :
		(ispick
		 || issaber
		) && (IS_ROCK(levl[x][y].typ) || IS_WATERTUNNEL(levl[x][y].typ)) &&
			(!level.flags.arboreal || IS_WALL(levl[x][y].typ)) ?
			DIGTYP_ROCK : DIGTYP_UNDIGGABLE);
}

boolean
is_digging()
{
	if (occupation == dig) {
	    return TRUE;
	}
	return FALSE;
}

#define BY_YOU		(&youmonst)
#define BY_OBJECT	((struct monst *)0)

boolean
dig_check(madeby, verbose, x, y)
	struct monst	*madeby;
	boolean		verbose;
	int		x, y;
{
	struct trap *ttmp = t_at(x, y);
	const char *verb =
	    (madeby != BY_YOU || !uwep || is_pick(uwep)) ? "dig in" :
	    is_lightsaber(uwep) ? "cut" :
	    "chop";

	if (On_stairs(x, y)) {
	    if (x == xdnladder || x == xupladder) {
		if(verbose) pline_The("ladder resists your effort.");
	    } else if(verbose) pline_The("stairs are too hard to %s.", verb);
	    return(FALSE);
	/* ALI - Artifact doors */
	} else if (IS_DOOR(levl[x][y].typ) && artifact_door(x, y)) {
	    if(verbose) pline_The("%s here is too hard to dig in.",
				  surface(x,y));
	    return(FALSE);
	} else if (IS_THRONE(levl[x][y].typ) && madeby != BY_OBJECT) {
	    if(verbose) pline_The("throne is too hard to break apart.");
	    return(FALSE);
	} else if (IS_FARMLAND(levl[x][y].typ) && madeby != BY_OBJECT) {
	    if(verbose) pline("Farmland cannot be dug out.");
	    return(FALSE);
	} else if (IS_MOUNTAIN(levl[x][y].typ) && madeby != BY_OBJECT) {
	    if(verbose) pline_The("mountain is too hard to dig into.");
	    return(FALSE);
	} else if (IS_GRAVEWALL(levl[x][y].typ) && madeby != BY_OBJECT) {
	    if(verbose) pline_The("grave wall needs to be dug by moving into it.");
	    return(FALSE);
	} else if (IS_ALTAR(levl[x][y].typ) && (madeby != BY_OBJECT ||
				Is_astralevel(&u.uz) || Is_sanctum(&u.uz))) {
	    if(verbose) pline_The("altar is too hard to break apart.");
	    return(FALSE);
	} else if (Is_airlevel(&u.uz)) {
	    if(verbose) You("cannot %s thin air.", verb);
	    return(FALSE);
	} else if (Is_waterlevel(&u.uz)) {
	    if(verbose) pline_The("water splashes and subsides.");
	    return(FALSE);
	} else if ((IS_ROCK(levl[x][y].typ) && levl[x][y].typ != SDOOR &&
		      ((levl[x][y].wall_info & W_NONDIGGABLE) != 0 && !(u.dynamitehack)) )
		|| (IS_FARMLAND(levl[x][y].typ)) || (IS_MOUNTAIN(levl[x][y].typ))  || (IS_GRAVEWALL(levl[x][y].typ))
		|| (ttmp &&
		      (ttmp->ttyp == MAGIC_PORTAL || ttmp->ttyp == HEEL_TRAP || ttmp->ttyp == ATTACKING_HEEL_TRAP || ttmp->ttyp == LOUDSPEAKER || ttmp->ttyp == KOP_CUBE || ttmp->ttyp == BOSS_SPAWNER || ttmp->ttyp == ARABELLA_SPEAKER || ttmp->ttyp == FART_TRAP || ttmp->ttyp == PERSISTENT_FART_TRAP || !Can_dig_down(&u.uz)))) {
	    if(verbose) pline_The("%s here is too hard to %s.",
				  surface(x,y), verb);
	    return(FALSE);
	} else if (sobj_at(BOULDER, x, y)) {
	    if(verbose) There("isn't enough room to %s here.", verb);
	    return(FALSE);
	} else if (madeby == BY_OBJECT &&
		    /* the block against existing traps is mainly to
		       prevent broken wands from turning holes into pits */
		    (ttmp || is_waterypool(x,y) || is_styxriver(x,y) || is_lava(x,y))) {
	    /* digging by player handles pools separately */
	    return FALSE;
	}
	return(TRUE);
}

STATIC_OVL int
dig()
{
	register struct rm *lev;
	register xchar dpx = digging.pos.x, dpy = digging.pos.y;
	register boolean ispick = uwep && is_pick(uwep);
	const char *verb =
	    (!uwep || is_pick(uwep)) ? "dig into" :
	    is_lightsaber(uwep) ? "cut through" :
	    "chop through";
	int bonus;

	lev = &levl[dpx][dpy];
	/* perhaps a nymph stole your pick-axe while you were busy digging */
	/* or perhaps you teleported away */
	/* WAC allow lightsabers */
	if (u.uswallow || !uwep || (!ispick &&
		(!is_lightsaber(uwep) || (!uwep->lamplit && !Role_if(PM_SHADOW_JEDI)) ) &&
		!is_axe(uwep) && !is_antibar(uwep)) ||
	    !on_level(&digging.level, &u.uz) ||
	    ((digging.down ? (dpx != u.ux || dpy != u.uy)
			   : (distu(dpx,dpy) > 2))))
		return(0);

	if (digging.down) {
	    if(!dig_check(BY_YOU, TRUE, u.ux, u.uy)) return(0);
	} else { /* !digging.down */
	    if (IS_TREE(lev->typ) && !may_dig(dpx,dpy) &&
			dig_typ(uwep, dpx, dpy) == DIGTYP_TREE) {
		pline("This tree seems to be petrified.");
		return(0);
	    }
	    /* ALI - Artifact doors */
	    if (IS_ROCK(lev->typ) && !may_dig(dpx,dpy) &&
	    		dig_typ(uwep, dpx, dpy) == DIGTYP_ROCK ||
		    IS_DOOR(lev->typ) && artifact_door(dpx, dpy)) {
		pline("This %s is too hard to %s.",
			IS_DOOR(lev->typ) ? "door" : IS_MOUNTAIN(lev->typ) ? "mountain" : IS_FARMLAND(lev->typ) ? "farmland" : "wall", verb);
		return(0);
	    }
	}
	if(Fumbling &&
		/* Can't exactly miss holding a lightsaber to the wall */
		!is_lightsaber(uwep) &&
		!rn2(3)) {
	    switch(rn2(3)) {
	    case 0:
		if(!welded(uwep)) {
		    You("fumble and drop your %s.", xname(uwep));
		    dropx(uwep);
		} else {
		    if (u.usteed)
			Your("%s %s and %s %s!",
			     xname(uwep),
			     otense(uwep, "bounce"), otense(uwep, "hit"),
			     mon_nam(u.usteed));
		    else
			pline("Ouch!  Your %s %s and %s you!",
			      xname(uwep),
			      otense(uwep, "bounce"), otense(uwep, "hit"));
		    set_wounded_legs(RIGHT_SIDE, HWounded_legs + 5 + rnd(5));
		}
		break;
	    case 1:
		pline("Bang!  You hit with the broad side of %s!",
		      the(xname(uwep)));
		break;
	    default: Your("swing misses its mark.");
		break;
	    }
	    return(0);
	}

	/* calculate digging effort */
	bonus = 4 + rn2(5) + abon() + uwep->spe - greatest_erosionX(uwep) + increase_damage_bonus_value() + RngeBloodlust + (Drunken_boxing && Confusion) + (StrongDrunken_boxing && Confusion);
	if (uarms && uarms->oartifact == ART_TEH_BASH_R) bonus += 2;
	if (uarmh && uarmh->oartifact == ART_HELMET_OF_DIGGING) bonus += 5;
	if (uarmg && itemhasappearance(uarmg, APP_DIGGER_GLOVES)) bonus += 5;
	if (uwep && uwep->oartifact == ART_COPPERED_OFF_FROM_ME) bonus += 5;
	if (uwep && uwep->oartifact == ART_STONEBITER) bonus += 5;
	if (uwep && uwep->oartifact == ART_BREAK_OUT) bonus += 5;
	if (uwep && uwep->oartifact == ART_MINE_OUT) bonus += 5;
	if (uarmh && uarmh->oartifact == ART_THIRD_CAR) bonus += 10;
	if (uwep && uwep->oartifact == ART_DIG__OF_COURSE) bonus += 5;
	if (uwep && uwep->oartifact == ART_NOTHING_FOR_IT) bonus += 5;
	if (uarm && uarm->oartifact == ART_CLANGFRIEND) bonus += 5;
	if (uarmf && uarmf->oartifact == ART_GRAVY_HIDE) bonus += 5;
	if (uarmc && uarmc->oartifact == ART_MINING_FOR_FUN_AND_PROFIT) bonus += 5;
	if (tunnels(youmonst.data)) bonus += rn2(3); /* digging monsters really should have a bonus here --Amy */
	if (needspick(youmonst.data)) bonus += rn2(3); /* and more so if they're dedicated miners (e.g. dwarf) */
	if (!PlayerCannotUseSkills) {

		if (uwep && is_lightsaber(uwep)) {
			switch (P_SKILL(P_WEDI)) {

				case P_BASIC:	bonus += 5; break;
				case P_SKILLED:	bonus += 10; break;
				case P_EXPERT:	bonus += 14; break;
				case P_MASTER:	bonus += 17; break;
				case P_GRAND_MASTER:	bonus += 21; break;
				case P_SUPREME_MASTER:	bonus += 25; break;
				default: break;
			}
		} else {
			switch (P_SKILL(P_WEDI)) {

				case P_BASIC:	bonus += 3; break;
				case P_SKILLED:	bonus += 6; break;
				case P_EXPERT:	bonus += 10; break;
				case P_MASTER:	bonus += 13; break;
				case P_GRAND_MASTER:	bonus += 16; break;
				case P_SUPREME_MASTER:	bonus += 20; break;
				default: break;
			}
		}
	}

	if (Race_if(PM_DWARF) || Role_if(PM_MIDGET) )
	    bonus *= 2;
	if (Race_if(PM_IRAHA)) bonus *= rnd(3);
	if (isfriday && bonus > 1) bonus /= 2;

	if (uwep && uwep->oartifact == ART_ETERNALE_DELAY) bonus -= 5;
	if (uwep && uwep->oartifact == ART_DE_SID && uwep->altmode) bonus += rnd(20);

	if (uwep && uwep->otyp == SHOVEL && !(uwep->oartifact == ART_AFTERMINE) )
	    bonus -= rn2(Role_if(PM_UNDERTAKER) ? 5 : 20); /* digging with a shovel takes longer */

	if (uwep && is_lightsaber(uwep) && !(uwep->oartifact == ART_DE_SID) ) /* Melting a hole takes longer */
	    bonus -= (uwep->lamplit && uwep->altmode) ? rn2(8) : rn2(20); /* but is faster if both blades are lit --Amy */
	if (uwep && is_lightsaber(uwep) && !uwep->lamplit)
	    bonus -= rn2(200); /* and if the saber isn't lit (shadow jedi role), it takes like forever --Amy */

	digging.effort += bonus;

	if (digging.down) {
		register struct trap *ttmp;

		if (digging.effort > 250) {
		    (void) dighole(FALSE);
		    (void) memset((void *)&digging, 0, sizeof digging);
		    return(0);	/* done with digging */
		}

		if (digging.effort <= 50 ||
		    is_lightsaber(uwep) ||
		    ((ttmp = t_at(dpx,dpy)) != 0 &&
			(ttmp->ttyp == PIT || ttmp->ttyp == SPIKED_PIT || ttmp->ttyp == GIANT_CHASM || ttmp->ttyp == SHIT_PIT || ttmp->ttyp == MANA_PIT || ttmp->ttyp == ANOXIC_PIT || ttmp->ttyp == ACID_PIT || ttmp->ttyp == SHAFT_TRAP || ttmp->ttyp == CURRENT_SHAFT ||
			 ttmp->ttyp == TRAPDOOR || ttmp->ttyp == HOLE)))
		    return(1);

		if (IS_ALTAR(lev->typ)) {
		    altar_wrath(dpx, dpy);
		    angry_priest();
		}

		if (dighole(TRUE)) {	/* make pit at <u.ux,u.uy> */
		    digging.level.dnum = 0;
		    digging.level.dlevel = -1;
		}
		return(0);
	}

	if (digging.effort > 100) {
		register const char *digtxt, *dmgtxt = (const char*) 0;
		register struct obj *obj;
		register boolean shopedge = *in_rooms(dpx, dpy, SHOPBASE);

		if (Role_if(PM_FIREFIGHTER) && uwep && uwep->oartifact == ART_BREAK_EVERYTHING && (IS_IRONBAR(lev->typ) || IS_TREE(lev->typ) || IS_FARMLAND(lev->typ) || IS_MOUNTAIN(lev->typ) ) ) {
			lev->typ = ROOM;
			digtxt = "You cleared away the obstructing terrain.";
		} else if ((obj = sobj_at(STATUE, dpx, dpy)) != 0) {
			if (break_statue(obj)) {
				digtxt = "The statue shatters.";
				if (uwep && is_lightsaber(uwep) && (uwep->lamplit || Role_if(PM_SHADOW_JEDI)) ) {
					use_skill(P_WEDI, 1);
				}
				if (uwep && uwep->oartifact == ART_DIGSRU) {
					use_skill(P_WEDI, 1);
				}
			} else
				/* it was a statue trap; break_statue()
				 * printed a message and updated the screen
				 */
				digtxt = (char *)0;
		} else if ((obj = sobj_at(BOULDER, dpx, dpy)) != 0) {
			struct obj *bobj;

			fracture_rock(obj);
			if ((bobj = sobj_at(BOULDER, dpx, dpy)) != 0) {
			    /* another boulder here, restack it to the top */
			    obj_extract_self(bobj);
			    place_object(bobj, dpx, dpy);
			}
			digtxt = "The boulder falls apart.";
			if (uwep && is_lightsaber(uwep) && (uwep->lamplit || Role_if(PM_SHADOW_JEDI)) ) {
				use_skill(P_WEDI, 1);
			}
			if (uwep && uwep->oartifact == ART_DIGSRU) {
				use_skill(P_WEDI, 1);
			}
		} else if (lev->typ == STONE || lev->typ == WATERTUNNEL || lev->typ == SCORR || IS_IRONBAR(lev->typ) ||
				IS_TREE(lev->typ)) {
			if(Is_earthlevel(&u.uz)) {
			    if(uwep->blessed && !rn2(3)) {
				mkcavearea(FALSE);
				goto cleanup;
			    } else if((uwep->cursed && !rn2(4)) ||
					  (!uwep->blessed && !rn2(6))) {
				mkcavearea(TRUE);
				goto cleanup;
			    }
			}
			if (IS_TREE(lev->typ)) {
			    digtxt = "You cut down the tree.";
			    u.cnd_treechopamount++;
			    if (u.ualign.type == A_CHAOTIC) adjalign(1);

				/* the tree squads do not want people who kill trees, and will aggressively chase them --Amy */
			    u.treesquadwantedlevel += (100 + rnd(u.cnd_treechopamount * 5));
			    if (Role_if(PM_BOSMER)) { /* you have angered Yavanna... */
				You("violated the sanctity of the grove!");
				adjalign(-10);
				u.ualign.sins++;
				u.alignlim--;
				u.treesquadwantedlevel += 1000;
			    }

			    lev->typ = ROOM;
			    if (!rn2(5)) wake_nearby(); /* felling a tree can make a loud noise, try it in real life! --Amy */

			    if (!rn2(5) && !(lev->looted & TREE_LOOTED) ) (void) rnd_treefruit_at(dpx, dpy);

				if (uwep && is_lightsaber(uwep) && (uwep->lamplit || Role_if(PM_SHADOW_JEDI)) ) {
					use_skill(P_WEDI, 1);
				}
				if (uwep && uwep->oartifact == ART_DIGSRU) {
					use_skill(P_WEDI, 1);
				}

			    if (!(lev->looted & TREE_SWARM) && !rn2(16)) {
			    	int cnt = rnl(4) + 2;
				int made = 0;
			    	coord mm;
			    	mm.x = dpx; mm.y = dpy;

				if (!rn2(20)) {

					while (cnt--) {
						if (enexto(&mm, mm.x, mm.y, &mons[PM_WOOD_NYMPH]) &&
						makemon(mkclass(S_NYMPH,0), mm.x, mm.y, MM_ANGRY))
						made++;
					}
					wake_nearby(); /* make sure they're awake --Amy */

				} else {

					while (cnt--) {
						if (enexto(&mm, mm.x, mm.y, &mons[PM_KILLER_BEE]) &&
						makemon(beehivemon(), mm.x, mm.y, MM_ANGRY))
						made++;
					}
				}

				if ( made )
				    pline("You've attracted the tree's former occupants!");
				else
				    You("smell stale honey.");

			    }

			    if (!rn2(20)) { /* summon the tree squad! --Amy */

				coord cc, dd;
				int cx,cy;
			      cx = rn2(COLNO);
			      cy = rn2(ROWNO);
				int tsdamount = rnd(10);
				if (!rn2(10)) tsdamount += rnd(20);

				u.aggravation = 1;
				reset_rndmonst(NON_PM);

				while (tsdamount) {

					int attempts = 0;
					struct permonst *pm = 0;
					tsdamount--;
					if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

newbossTSD:
					do {
						pm = rndmonst();
						attempts++;
						if (attempts && (attempts % 10000 == 0)) u.mondiffhack++;
						if (!rn2(2000)) reset_rndmonst(NON_PM);

					} while ( (!pm || (pm && !(pm->msound == MS_TREESQUAD ))) && attempts < 50000);

					if (!pm && rn2(50) ) {
						attempts = 0;
						goto newbossTSD;
					}
					if (pm && !(pm->msound == MS_TREESQUAD) && rn2(50) ) {
						attempts = 0;
						goto newbossTSD;
					}

					if (pm) (void) makemon(pm, cx, cy, MM_ANGRY|MM_FRENZIED|MM_XFRENZIED|MM_ADJACENTOK);

					u.mondiffhack = 0;

				}

				u.aggravation = 0;

			    }

			} else if (IS_WATERTUNNEL(lev->typ)) {
			    digtxt = "You smash the solid part of the tunnel apart.  Now it's a moat!";
			    u.cnd_diggingamount++;
			    lev->typ = MOAT;
				if (uwep && is_lightsaber(uwep) && (uwep->lamplit || Role_if(PM_SHADOW_JEDI)) ) {
					use_skill(P_WEDI, 1);
				}
				if (uwep && uwep->oartifact == ART_DIGSRU) {
					use_skill(P_WEDI, 1);
				}
			} else if (uwep && IS_IRONBAR(lev->typ) && is_antibar(uwep) ) {

			    digtxt = "You smash the bars to the ground.";
			    u.cnd_barbashamount++;
			    if (!rn2(100)) {
				int attempts = 0;
				int monstcnt;
				monstcnt = 1 + rnd(3);
				struct permonst *pm = 0;

				if (Aggravate_monster) {
					u.aggravation = 1;
					reset_rndmonst(NON_PM);
				}

newbossMETALMAFIA:
				do {
					pm = rndmonst();
					attempts++;
					if (attempts && (attempts % 10000 == 0)) u.mondiffhack++;
					if (!rn2(2000)) reset_rndmonst(NON_PM);

				} while ( (!pm || (pm && !(pm->msound == MS_METALMAFIA ))) && attempts < 50000);

				if (!pm && rn2(50) ) {
					attempts = 0;
					goto newbossMETALMAFIA;
				}
				if (pm && !(pm->msound == MS_METALMAFIA) && rn2(50) ) {
					attempts = 0;
					goto newbossMETALMAFIA;
				}

				if (pm) (void) makemon(pm, 0, 0, MM_ANGRY);

				u.mondiffhack = 0;

				if (monstcnt > 0) {
					monstcnt--;
					if (monstcnt > 0) goto newbossMETALMAFIA;
				}

				u.aggravation = 0;

			    }

			    if (In_sokoban(&u.uz) && !(uwep && uwep->oartifact == ART_HOPE_OF_SOKOBAN && rn2(3)) )
				{change_luck(-1);
				pline("You cheater!");
				if (evilfriday) u.ugangr++;
				}

			    if (!issoviet || !rn2(10)) lev->typ = ROOM;
			    else {
				pline("V sootvetstvii s tipom bloka l'da eto ne imeyet nikakogo smysla. I poetomu on ne rabotayet. Khar Khar Khar on on on bwar khar khar!");
				digging.quiet = TRUE; /* suppress misleading message */
		/* In Soviet Russia, iron bars are supposed to be nigh impenetrable. After all, someone's gotta keep all those
		 * capitalist prisoners at bay. Therefore, you can try to smash iron bars many times but only rarely it will
		 * work. Actually, it's supposed to be possible to cut through bars with a lightsaber instead according to
		 * Soviet, but seriously, fuck that shit, I'm not gonna bother coding that just for an obscure special mode
		 * that 99.9% of players won't use anyway because they don't understand russian. --Amy */
			    }
				if (uwep->obrittle == 3 || uwep->obrittle2 == 3) {
					Your("weapon was destroyed!");
					useupall(uwep);
				} else {

					if (!rn2(2)) uwep->obrittle++;
					else uwep->obrittle2++;
					Your("weapon has taken damage from smashing the bars!");
				}
				/* it's intentional that the weapon cannot resist, because otherwise you could take one that
				 * is highly resistant to erosion or even immune to being destroyed and simply smash all bars
				 * in the entire dungeon, which isn't what we want --Amy */

				if (!rn2(5)) mkobj_at(CHAIN_CLASS, dpx, dpy, FALSE, FALSE); /* maybe make a chain from the bars --Amy */

			} else {
			    digtxt = "You succeed in cutting away some rock.";
			    u.cnd_diggingamount++;
			    lev->typ = CORR;

				if (uwep && uwep->oartifact == ART_MFFAP && !rn2(20)) {
					struct trap *extramine;
					if (!t_at(dpx, dpy)) {
						extramine = maketrap(dpx, dpy, LANDMINE, 0, FALSE);
						if (extramine) {
							extramine->tseen = TRUE;
							extramine->hiddentrap = FALSE;
							extramine->madeby_u = TRUE;
							pline("CLICK! A mine has been set!");
						}
					}
				}

				if (uwep && is_lightsaber(uwep) && (uwep->lamplit || Role_if(PM_SHADOW_JEDI)) ) {
					use_skill(P_WEDI, 1);
				}
				if (uwep && uwep->oartifact == ART_DIGSRU) {
					use_skill(P_WEDI, 1);
				}
			}
		} else if(IS_WALL(lev->typ)) {
			if(shopedge) {
			    add_damage(dpx, dpy, 10L * ACURRSTR);
			    dmgtxt = "damage";
			}
			if (level.flags.is_maze_lev) {
			    lev->typ = /*ROOM*/CORR;
			} else if (level.flags.is_cavernous_lev &&
				   !in_town(dpx, dpy)) {
			    lev->typ = CORR;
			} else if (IS_WATERTUNNEL(lev->typ)) {
			    lev->typ = MOAT;
			} else if (IS_DIGGABLEWALL(lev->typ)) {
			    lev->typ = CORR;
			} else {
			    lev->typ = DOOR;
			    lev->doormask = D_NODOOR;
			}
			digtxt = "You make an opening in the wall.";
			u.cnd_diggingamount++;

			if (uwep && is_lightsaber(uwep) && (uwep->lamplit || Role_if(PM_SHADOW_JEDI)) ) {
				use_skill(P_WEDI, 1);
			}
			if (uwep && uwep->oartifact == ART_DIGSRU) {
				use_skill(P_WEDI, 1);
			}

		} else if(lev->typ == SDOOR) {
			cvt_sdoor_to_door(lev);	/* ->typ = DOOR */
			digtxt = "You break through a secret door!";
			if (uwep && is_lightsaber(uwep) && (uwep->lamplit || Role_if(PM_SHADOW_JEDI)) ) {
				use_skill(P_WEDI, 1);
			}
			if (uwep && uwep->oartifact == ART_DIGSRU) {
				use_skill(P_WEDI, 1);
			}
			if(!(lev->doormask & D_TRAPPED))
				lev->doormask = D_BROKEN;
		} else if(closed_door(dpx, dpy)) {
			digtxt = "You break through the door.";
			if (uwep && is_lightsaber(uwep) && (uwep->lamplit || Role_if(PM_SHADOW_JEDI)) ) {
				use_skill(P_WEDI, 1);
			}
			if (uwep && uwep->oartifact == ART_DIGSRU) {
				use_skill(P_WEDI, 1);
			}
			if(shopedge) {
			    add_damage(dpx, dpy, 400L);
			    dmgtxt = "break";
			}
			if(!(lev->doormask & D_TRAPPED))
				lev->doormask = D_BROKEN;
		} else return(0); /* statue or boulder got taken */

		if(!does_block(dpx,dpy,&levl[dpx][dpy]))
		    unblock_point(dpx,dpy);	/* vision:  can see through */
			if (!(levl[dpx][dpy].wall_info & W_HARDGROWTH)) levl[dpx][dpy].wall_info |= W_EASYGROWTH;
		if(Blind)
		    feel_location(dpx, dpy);
		else
		    newsym(dpx, dpy);
		if(digtxt && !digging.quiet) pline("%s", digtxt); /* after newsym */
		if(dmgtxt)
		    pay_for_damage(dmgtxt, FALSE);

		if(Is_earthlevel(&u.uz) && !rn2(3)) {
		    register struct monst *mtmp;

		    switch(rn2(2)) {
		      case 0:
			mtmp = makemon(&mons[PM_EARTH_ELEMENTAL],
					dpx, dpy, NO_MM_FLAGS);
			break;
		      default:
			mtmp = makemon(&mons[PM_XORN],
					dpx, dpy, NO_MM_FLAGS);
			break;
		    }
		    if(mtmp) pline_The("debris reassembles and comes to life!");
		}
		if(IS_DOOR(lev->typ) && (lev->doormask & D_TRAPPED)) {
			lev->doormask = D_NODOOR;
			b_trapped("door", 0);
			newsym(dpx, dpy);
		}
cleanup:
		digging.lastdigtime = moves;
		digging.quiet = FALSE;
		digging.level.dnum = 0;
		digging.level.dlevel = -1;
		return(0);
	} else {		/* not enough effort has been spent yet */
		static const char *const d_target[8] = {
			"", "rock", "statue", "boulder", "door", "tree", "bars", "obstruction"
		};
		int dig_target = dig_typ(uwep, dpx, dpy);

		if (IS_WALL(lev->typ) || dig_target == DIGTYP_DOOR) {
		    if(*in_rooms(dpx, dpy, SHOPBASE)) {
			pline("This %s seems too hard to %s.",
			      IS_DOOR(lev->typ) ? "door" : IS_MOUNTAIN(lev->typ) ? "mountain" : IS_FARMLAND(lev->typ) ? "farmland" : "wall", verb);
			return(0);
		    }
		} else if (!IS_ROCK(lev->typ) && !IS_WATERTUNNEL(lev->typ) && dig_target == DIGTYP_ROCK)
		    return(0); /* statue or boulder got taken */
		if(!did_dig_msg) {
		    if (is_lightsaber(uwep)) You("burn steadily through %s.",
			the(d_target[dig_target]));
		    else
		    You("hit the %s with all your might.",
			d_target[dig_target]);
		    did_dig_msg = TRUE;
		}
	}
	return(1);
}

/* When will hole be finished? Very rough indication used by shopkeeper. */
int
holetime()
{
	if(occupation != dig || !*u.ushops) return(-1);
	return ((250 - digging.effort) / 20);
}

/* Return typ of liquid to fill a hole with, or ROOM, if no liquid nearby */
STATIC_OVL
schar
fillholetyp(x,y)
int x, y;
{
    register int x1, y1;
    int lo_x = max(1,x-1), hi_x = min(x+1,COLNO-1),
	lo_y = max(0,y-1), hi_y = min(y+1,ROWNO-1);
    int pool_cnt = 0, moat_cnt = 0, lava_cnt = 0, urine_cnt = 0, styx_cnt = 0, shifting_cnt = 0;

    for (x1 = lo_x; x1 <= hi_x; x1++)
	for (y1 = lo_y; y1 <= hi_y; y1++)
	    if (levl[x1][y1].typ == POOL)
		pool_cnt++;
	    else if (levl[x1][y1].typ == MOAT ||
		    (levl[x1][y1].typ == DRAWBRIDGE_UP &&
			(levl[x1][y1].drawbridgemask & DB_UNDER) == DB_MOAT))
		moat_cnt++;
	    else if (levl[x1][y1].typ == LAVAPOOL ||
		    (levl[x1][y1].typ == DRAWBRIDGE_UP &&
			(levl[x1][y1].drawbridgemask & DB_UNDER) == DB_LAVA))
		lava_cnt++;
	    else if (levl[x1][y1].typ == URINELAKE)
		urine_cnt++;
	    else if (levl[x1][y1].typ == STYXRIVER)
		styx_cnt++;
	    else if (levl[x1][y1].typ == SHIFTINGSAND)
		shifting_cnt++;
    pool_cnt /= 3;		/* not as much liquid as the others */

    if (lava_cnt > moat_cnt + pool_cnt && rn2(lava_cnt + 1))
	return LAVAPOOL;
    else if (moat_cnt > 0 && rn2(moat_cnt + 1))
	return MOAT;
    else if (pool_cnt > 0 && rn2(pool_cnt + 1))
	return POOL;
    else if (urine_cnt > 0 && rn2(urine_cnt + 1))
	return URINELAKE;
    else if (styx_cnt > 0 && rn2(styx_cnt + 1))
	return STYXRIVER;
    else if (shifting_cnt > 0 && rn2(shifting_cnt + 1))
	return SHIFTINGSAND;
    else
	return ROOM;
}

void
digactualhole(x, y, madeby, ttyp)
register int	x, y;
struct monst	*madeby;
int ttyp;
{
	struct obj *oldobjs, *newobjs;
	register struct trap *ttmp;
	char surface_type[BUFSZ];
	struct rm *lev = &levl[x][y];
	boolean shopdoor;
	struct monst *mtmp = m_at(x, y);	/* may be madeby */
	boolean madeby_u = (madeby == BY_YOU);
	boolean madeby_obj = (madeby == BY_OBJECT);
	boolean at_u = (x == u.ux) && (y == u.uy);
	boolean wont_fall = Levitation || Flying;

	if (u.utrap && u.utraptype == TT_INFLOOR) u.utrap = 0;

	/* these furniture checks were in dighole(), but wand
	   breaking bypasses that routine and calls us directly */
	if (IS_FOUNTAIN(lev->typ)) {
	    dogushforth(FALSE);
	    SET_FOUNTAIN_WARNED(x,y);		/* force dryup */
	    dryup(x, y, madeby_u);
	    return;
	} else if (IS_SINK(lev->typ)) {
	    breaksink(x, y);
	    return;
	} else if (IS_TOILET(lev->typ)) {
		breaktoilet(u.ux,u.uy);
	} else if (lev->typ == DRAWBRIDGE_DOWN ||
		   (is_drawbridge_wall(x, y) >= 0)) {
	    int bx = x, by = y;
	    /* if under the portcullis, the bridge is adjacent */
	    (void) find_drawbridge(&bx, &by);
	    destroy_drawbridge(bx, by);
	    return;
	}

	if (ttyp != PIT && !Can_dig_down(&u.uz)) {
	    impossible("digactualhole: can't dig %s on this level.",
		       defsyms[trap_to_defsym(ttyp)].explanation);
	    ttyp = PIT;
	}

	/* maketrap() might change it, also, in this situation,
	   surface() returns an inappropriate string for a grave */
	if (IS_GRAVE(lev->typ))
	    strcpy(surface_type, "grave");
	else
	    strcpy(surface_type, surface(x,y));
	shopdoor = IS_DOOR(lev->typ) && *in_rooms(x, y, SHOPBASE);
	oldobjs = level.objects[x][y];
	ttmp = maketrap(x, y, ttyp, 0, FALSE);
	if (!ttmp) return;
	newobjs = level.objects[x][y];
	ttmp->tseen = ((madeby_u || cansee(x,y)) && !ttmp->hiddentrap);
	ttmp->madeby_u = madeby_u;
	newsym(ttmp->tx,ttmp->ty);

	if (ttyp == PIT) {

	    if(madeby_u) {
		You("dig a pit in the %s.", surface_type);
		u.cnd_diggingamount++;
		if (shopdoor) pay_for_damage("ruin", FALSE);
	    } else if (!madeby_obj && canseemon(madeby))
		pline("%s digs a pit in the %s.", Monnam(madeby), surface_type);
	    else if (cansee(x, y) && flags.verbose)
		pline("A pit appears in the %s.", surface_type);

	    if(at_u) {
		if (!wont_fall) {
		    if (!Passes_walls)
			u.utrap = rn1(4,2);
		    u.utraptype = TT_PIT;
		    vision_full_recalc = 1;	/* vision limits change */
		} else
		    u.utrap = 0;
		if (oldobjs != newobjs)	/* something unearthed */
			(void) pickup(1);	/* detects pit */
	    } else if(mtmp) {
		if(is_flyer(mtmp->data) || mtmp->egotype_flying || is_floater(mtmp->data)) {
		    if(canseemon(mtmp))
			pline("%s %s over the pit.", Monnam(mtmp),
						     (is_flyer(mtmp->data)) ?
						     "flies" : "floats");
		} else if(mtmp != madeby)
		    (void) mintrap(mtmp);
	    }
	} else {	/* was TRAPDOOR now a HOLE*/

	    if(madeby_u) {
		You("dig a hole through the %s.", surface_type);
		u.cnd_diggingamount++;
	    } else if(!madeby_obj && canseemon(madeby))
		pline("%s digs a hole through the %s.",
		      Monnam(madeby), surface_type);
	    else if(cansee(x, y) && flags.verbose)
		pline("A hole appears in the %s.", surface_type);

	    if (at_u) {
		if (!u.ustuck && !wont_fall && !next_to_u()) {
		    You("are jerked back by your pet!");
		    wont_fall = TRUE;
		}

		/* Floor objects get a chance of falling down.  The case where
		 * the hero does NOT fall down is treated here.  The case
		 * where the hero does fall down is treated in goto_level().
		 */
		if (u.ustuck || wont_fall) {
		    if (newobjs)
			impact_drop((struct obj *)0, x, y, 0);
		    if (oldobjs != newobjs)
			(void) pickup(1);
		    if (shopdoor && madeby_u) pay_for_damage("ruin", FALSE);

		} else {
		    d_level newlevel;
		    const char *You_fall = "You fall through...";

		    if (*u.ushops && madeby_u)
			shopdig(1); /* shk might snatch pack */
		    /* handle earlier damage, eg breaking wand of digging */
		    else if (!madeby_u) pay_for_damage("dig into", TRUE);

		    /* Earlier checks must ensure that the destination
		     * level exists and is in the present dungeon.
		     */
		    newlevel.dnum = u.uz.dnum;
		    newlevel.dlevel = u.uz.dlevel + 1;
		    /* Cope with holes caused by monster's actions -- ALI */
		    if (flags.mon_moving) {
			schedule_goto(&newlevel, FALSE, TRUE, FALSE,
			  You_fall, (char *)0);
		    } else {
			pline("%s", You_fall);
		    goto_level(&newlevel, FALSE, TRUE, FALSE);
		    /* messages for arriving in special rooms */
		    spoteffects(FALSE);
		}
		}
	    } else {
		if (shopdoor && madeby_u) pay_for_damage("ruin", FALSE);
		if (newobjs)
		    impact_drop((struct obj *)0, x, y, 0);
		if (mtmp) {
		     /*[don't we need special sokoban handling here?]*/
		    if (is_flyer(mtmp->data) || mtmp->egotype_flying || is_floater(mtmp->data) ||
		        mtmp->data == &mons[PM_WUMPUS] ||
			(mtmp->wormno && count_wsegs(mtmp) > 5) ||
			mtmp->data->msize >= MZ_HUGE) return;
		    if (mtmp == u.ustuck)	/* probably a vortex */
			    return;		/* temporary? kludge */

		    if (teleport_pet(mtmp, FALSE)) {
			d_level tolevel;

			if (Is_stronghold(&u.uz)) {
			    assign_level(&tolevel, &valley_level);
			} else if (Is_botlevel(&u.uz)) {
			    if (canseemon(mtmp))
				pline("%s avoids the trap.", Monnam(mtmp));
			    return;
			} else {
			    get_level(&tolevel, depth(&u.uz) + 1);
			}
			if (mtmp->isshk) make_angry_shk(mtmp, 0, 0);
			migrate_to_level(mtmp, ledger_no(&tolevel),
					 MIGR_RANDOM, (coord *)0);
		    }
		}
	    }
	}
}

/* return TRUE if digging succeeded, FALSE otherwise */
boolean
dighole(pit_only)
boolean pit_only;
{
	register struct trap *ttmp = t_at(u.ux, u.uy);
	struct rm *lev = &levl[u.ux][u.uy];
	struct obj *boulder_here;
	schar typ;
	boolean nohole = !Can_dig_down(&u.uz);

	if ((ttmp && (ttmp->ttyp == MAGIC_PORTAL || ttmp->ttyp == HEEL_TRAP || ttmp->ttyp == ATTACKING_HEEL_TRAP || ttmp->ttyp == LOUDSPEAKER || ttmp->ttyp == KOP_CUBE || ttmp->ttyp == BOSS_SPAWNER || ttmp->ttyp == ARABELLA_SPEAKER || ttmp->ttyp == FART_TRAP || ttmp->ttyp == PERSISTENT_FART_TRAP || nohole)) ||
		In_minotaurmaze(&u.uz) ||
	   /* ALI - artifact doors */
	   IS_DOOR(levl[u.ux][u.uy].typ) && artifact_door(u.ux, u.uy) ||
	   (IS_ROCK(lev->typ) && lev->typ != SDOOR &&
	    (lev->wall_info & W_NONDIGGABLE) != 0)) {
		pline_The("%s here is too hard to dig in.", surface(u.ux,u.uy));

	} else if (is_waterypool(u.ux, u.uy) || is_styxriver(u.ux, u.uy) || is_lava(u.ux, u.uy)) {
		pline_The("%s sloshes furiously for a moment, then subsides.",
			is_lava(u.ux, u.uy) ? "lava" : "water");
		wake_nearby();	/* splashing */

	} else if (lev->typ == DRAWBRIDGE_DOWN ||
		   (is_drawbridge_wall(u.ux, u.uy) >= 0)) {
		/* drawbridge_down is the platform crossing the moat when the
		   bridge is extended; drawbridge_wall is the open "doorway" or
		   closed "door" where the portcullis/mechanism is located */
		if (pit_only) {
		    pline_The("drawbridge seems too hard to dig through.");
		    return FALSE;
	} else if (IS_GRAVE(lev->typ)) {        
	    digactualhole(u.ux, u.uy, BY_YOU, PIT);
	    dig_up_grave();
	    return TRUE;
		} else {
		    int x = u.ux, y = u.uy;
		    /* if under the portcullis, the bridge is adjacent */
		    (void) find_drawbridge(&x, &y);
		    destroy_drawbridge(x, y);
		    return TRUE;
		}

	} else if ((boulder_here = sobj_at(BOULDER, u.ux, u.uy)) != 0) {
		if (ttmp && (ttmp->ttyp == PIT || ttmp->ttyp == SPIKED_PIT || ttmp->ttyp == SHIT_PIT || ttmp->ttyp == MANA_PIT || ttmp->ttyp == ANOXIC_PIT || ttmp->ttyp == ACID_PIT) &&
		    rn2(2)) {
			pline_The("boulder settles into the pit.");
			ttmp->ttyp = PIT;	 /* crush spikes */
		} else if (ttmp && ttmp->ttyp == GIANT_CHASM) {
			pline("KADOOM! The boulder falls in!");
		} else {
			/*
			 * digging makes a hole, but the boulder immediately
			 * fills it.  Final outcome:  no hole, no boulder.
			 */
			pline("KADOOM! The boulder falls in!");
			(void) delfloortrap(ttmp);
		}
		delobj(boulder_here);
		return TRUE;

	} else if (IS_GRAVE(lev->typ)) {        
	    dig_up_grave();
			digactualhole(u.ux, u.uy, BY_YOU, PIT);
	    return TRUE;
	} else if (lev->typ == DRAWBRIDGE_UP) {
		/* must be floor or ice, other cases handled above */
		/* dig "pit" and let fluid flow in (if possible) */
		typ = fillholetyp(u.ux,u.uy);

		if (typ == ROOM) {
			/*
			 * We can't dig a hole here since that will destroy
			 * the drawbridge.  The following is a cop-out. --dlc
			 */
			pline_The("%s here is too hard to dig in.",
			      surface(u.ux, u.uy));
			return FALSE;
		}

		lev->drawbridgemask &= ~DB_UNDER;
		lev->drawbridgemask |= (typ == LAVAPOOL) ? DB_LAVA : DB_MOAT;

 liquid_flow:
		if (ttmp) (void) delfloortrap(ttmp);
		/* if any objects were frozen here, they're released now */
		unearth_objs(u.ux, u.uy);

		pline("As you dig, the hole fills with %s!",
		      typ == LAVAPOOL ? "lava" : typ == STYXRIVER ? "an icky green liquid" : typ == URINELAKE ? "urine" : typ == SHIFTINGSAND ? "quicksand" : "water");
		/* KMH, balance patch -- new intrinsic */
		if (!Levitation && !Flying && typ != STYXRIVER && typ != URINELAKE && typ != SHIFTINGSAND) {
		    if (typ == LAVAPOOL)
			(void) lava_effects();
		    else if (!Wwalking && !Race_if(PM_KORONST) && !Swimming)
			(void) drown();
		}
		return TRUE;

	/* the following two are here for the wand of digging */
	} else if (IS_THRONE(lev->typ)) {
		pline_The("throne is too hard to break apart.");

	} else if (IS_FARMLAND(lev->typ)) {
		pline("Farmland cannot be dug out.");
	} else if (IS_MOUNTAIN(lev->typ)) {
		pline_The("mountain is too hard to dig into.");
	} else if (IS_GRAVEWALL(lev->typ)) {
		pline_The("grave wall needs to be dug by moving into it.");

	} else if (IS_ALTAR(lev->typ)) {
		pline_The("altar is too hard to break apart.");

	} else {
		typ = fillholetyp(u.ux,u.uy);

		if (typ != ROOM) {
			lev->typ = typ;
			goto liquid_flow;
		}

		/* finally we get to make a hole */
		if (nohole || pit_only)
			digactualhole(u.ux, u.uy, BY_YOU, PIT);
		else
			digactualhole(u.ux, u.uy, BY_YOU, HOLE);

		return TRUE;
	}

	return FALSE;
}

STATIC_OVL void
dig_up_grave()
{
	struct obj *otmp;

	/* Grave-robbing is frowned upon... */
	exercise(A_WIS, FALSE);
	if (uwep && uwep->oartifact == ART_JAPANESE_WOMEN) {
	    adjalign(-5);
		u.ualign.sins++;
		u.alignlim--;
		adjalign(-10);
	    You("disturb the honorable dead!");
	}
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_JAPANESE_WOMEN) {
	    adjalign(-5);
		u.ualign.sins++;
		u.alignlim--;
		adjalign(-10);
	    You("disturb the honorable dead!");
	}
	if (uwep && uwep->oartifact == ART_SAKUSHNIR) {
	    adjalign(-5);
		u.ualign.sins++;
		u.alignlim--;
		adjalign(-10);
	    You("disturb the honorable dead!");
	}
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_SAKUSHNIR) {
	    adjalign(-5);
		u.ualign.sins++;
		u.alignlim--;
		adjalign(-10);
	    You("disturb the honorable dead!");
	}
	if (Role_if(PM_ARCHEOLOGIST)) {
	    adjalign(-sgn(u.ualign.type)*3);
		u.ualign.sins++;
		u.alignlim--;
		adjalign(-5);
	    You_feel("like a despicable grave-robber!");
	}
	if (Role_if(PM_SAMURAI)) {
	    adjalign(-sgn(u.ualign.type)*5); /* stiffer penalty */
		u.ualign.sins++;
		u.alignlim--;
		adjalign(-10);
	    You("disturb the honorable dead!");
	}
	if (uwep && uwep->otyp == HONOR_KATANA) {
	    adjalign(-sgn(u.ualign.type)*5); /* stiffer penalty */
		u.ualign.sins++;
		u.alignlim--;
		adjalign(-10);
	    You("disturb the honorable dead!");
	}
	if (u.twoweap && uswapwep && uswapwep->otyp == HONOR_KATANA) {
	    adjalign(-sgn(u.ualign.type)*5); /* stiffer penalty */
		u.ualign.sins++;
		u.alignlim--;
		adjalign(-10);
	    You("disturb the honorable dead!");
	}
	if (u.ualign.type == A_LAWFUL) {
	    adjalign(-sgn(u.ualign.type)*2);
	    You("have violated the sanctity of this grave!");
	}

	switch (rn2(5)) {
	case 0:
	case 1:
	    You("unearth a corpse.");
	    if (!!(otmp = mk_tt_object(CORPSE, u.ux, u.uy)))
	    	otmp->age -= 100;		/* this is an *OLD* corpse */;
	    break;
	case 2:
	    if (!Blind) pline(FunnyHallu ? "Dude!  The living dead!" :
 			"The grave's owner is very upset!");

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

 	    (void) makemon(mkclass(S_ZOMBIE,0), u.ux, u.uy, NO_MM_FLAGS);

		u.aggravation = 0;

	    break;
	case 3:
	    if (!Blind) pline(FunnyHallu ? "I want my mummy!" :
 			"You've disturbed a tomb!");

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

 	    (void) makemon(mkclass(S_MUMMY,0), u.ux, u.uy, NO_MM_FLAGS);

		u.aggravation = 0;

	    break;
	default:
	    /* No corpse */
	    pline_The("grave seems unused.  Strange....");
	    break;
	}
	levl[u.ux][u.uy].typ = ROOM;
	del_engr_at(u.ux, u.uy);
	newsym(u.ux,u.uy);
	return;
}

int
use_pick_axe(obj)
struct obj *obj;
{
	boolean ispick;
	boolean isantibar;
	char dirsyms[12];
	char qbuf[QBUFSZ];
	register char *dsp = dirsyms;
	register int rx, ry;
	int res = 0;
	register const char *sdp, *verb;

	if(iflags.num_pad) sdp = ndir; else sdp = sdir;	/* DICE workaround */

	/* Check tool */
	if (obj != uwep) {
	    if (!wield_tool(obj, "swing")) return 0;
	    else res = 1;
	}
	ispick = is_pick(obj);
	isantibar = is_antibar(obj);
	verb = ispick ? "dig" : isantibar ? "lash out" : "chop";

	if (u.utrap && u.utraptype == TT_WEB) {
	    pline("%s you can't %s while entangled in a web.",
		  /* res==0 => no prior message;
		     res==1 => just got "You now wield a pick-axe." message */
		  !res ? "Unfortunately," : "But", verb);
	    if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    return res;
	}

	if (u.utrap && u.utraptype == TT_GLUE) {
	    pline("%s you can't %s while being held by the glue.",
		  /* res==0 => no prior message;
		     res==1 => just got "You now wield a pick-axe." message */
		  !res ? "Unfortunately," : "But", verb);
	    if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    return res;
	}

	while(*sdp) {
		(void) movecmd(*sdp);	/* sets u.dx and u.dy and u.dz */
		rx = u.ux + u.dx;
		ry = u.uy + u.dy;
		/* Include down even with axe, so we have at least one direction */
		if (u.dz > 0 ||
		    (u.dz == 0 && isok(rx, ry) &&
		     dig_typ(obj, rx, ry) != DIGTYP_UNDIGGABLE))
			*dsp++ = *sdp;
		sdp++;
	}
	*dsp = 0;
	sprintf(qbuf, "In what direction do you want to %s? [%s]", verb, dirsyms);
	if(!getdir(qbuf))
		return(res);

	return (use_pick_axe2(obj));
}

/* general dig through doors/etc. function
 * Handles pickaxes/lightsabers/axes
 * called from doforce and use_pick_axe
 */

/* MRKR: use_pick_axe() is split in two to allow autodig to bypass */
/*       the "In what direction do you want to dig?" query.        */
/*       use_pick_axe2() uses the existing u.dx, u.dy and u.dz    */

int use_pick_axe2(obj)
struct obj *obj;
{
	register int rx, ry;
	register struct rm *lev;
	int dig_target, digtyp;
	boolean ispick = is_pick(obj);
	const char *verbing = ispick ? "digging" :
		is_lightsaber(uwep) ? "cutting" :
		"chopping";

	/* 0 = pick, 1 = lightsaber, 2 = axe */
	digtyp = (is_pick(uwep) ? 0 :
		is_lightsaber(uwep) ? 1 :
		2);

	if (u.uswallow && attack(u.ustuck)) {
		;  /* return(1) */
	} else if (Underwater) {
		pline("Turbulence torpedoes your %s attempts.", verbing);
	} else if(u.dz < 0) {
		if(Levitation)
		    if (digtyp == 1)
			pline_The("ceiling is too hard to cut through.");
		    else
			You("don't have enough leverage.");
		else
			You_cant("reach the %s.",ceiling(u.ux,u.uy));
	} else if(!u.dx && !u.dy && !u.dz) {
		/* NOTREACHED for lightsabers/axes called from doforce */
		
		char buf[BUFSZ];
		int dam;

		dam = rnd(2) + dbon() + obj->spe;
		if (dam <= 0) dam = 1;
		if (obj->otyp == SEXY_LEATHER_PUMP) {
			if (ACURR(A_INT) >= 6) pline("Klock! You hit yourself with your sexy leather pump, producing a beautiful sound.");
			else pline("Klock! You hit yourself with your sexy leather pump, producing a hollow sound.");
		} else if (obj->otyp == BLOCK_HEELED_COMBAT_BOOT) You_feel("a wonderfully painful sensation as you hit yourself with your fleecy block-heeled combat boot.");
		else if (obj->otyp == TORPEDO) pline("You deal yourself a deep gash with your torpedo!");
		else if (obj->otyp == YATAGAN) pline("You terribly hurt yourself with your blade!");
		else if (obj->otyp == DIAMOND_SMASHER) pline("You see lots of little asterisks as the diamond club hits you!");
		else if (obj->otyp == HOMING_TORPEDO) pline("The torpedo strikes right into your vital organs!");
		else if (obj->otyp == RADIOACTIVE_DAGGER) pline("You irradiate yourself with your dagger!");
		else if (obj->otyp == IMPACT_STAFF) pline("Dock! You hit yourself with your staff!");
		else if (obj->otyp == BLOCK_HEELED_SANDAL) pline("Mmmmmmmm, the massive block heel hits your %s so wonderfully painfully!", body_part(HEAD));
		else if (obj->otyp == INKA_BOOT) pline("You come right as the lovely, soft boot heel hits you.");
		else if (obj->otyp == SEXY_MARY_JANE) pline("Wow, the area between your %s gets all wet as the fleecy-soft, massive block heel hits you!", makeplural(body_part(LEG)) );
		else if (obj->otyp == PROSTITUTE_SHOE) pline("You have an orgasm as you hit yourself with the incredibly female prostitute shoe.");
		else if (obj->otyp == SOFT_LADY_SHOE) pline("You feel wonderful intense pain as the incredibly soft lady shoe hits you.");
		else if (obj->otyp == STEEL_WHIP) {
			if (flags.female) You_feel("like a submissive girl as you whip yourself!");
			else pline("You enjoy the feeling of pain as the whip hits your bare skin!");
		}
		else if (obj->otyp == HIGH_HEELED_SANDAL) pline("Ouch - you hit yourself with a massive high heel! It's very painful!");
		else if (obj->otyp == WEDGED_LITTLE_GIRL_SANDAL) pline("You hit yourself with a lovely wedge heel! Even though it looks very innocuous, you can definitely feel intense pain...");
		else if (obj->otyp == HUGGING_BOOT) pline("You bonk yourself with a thick winter boot - seems that this boot is made from a harder material than your %s...", body_part(HEAD));
		else You("hit yourself with %s.", yname(uwep));
		sprintf(buf, "%s own %s", uhis(),
				OBJ_NAME(objects[obj->otyp]));
		losehp(dam, buf, KILLED_BY);
		flags.botl=1;
		return(1);
	} else if(u.dz == 0) {
		if ((Stunned && !rn2(issoviet ? 1 : StrongStun_resist ? 20 : Stun_resist ? 8 : 2)) || (Confusion && !rn2(issoviet ? 2 : StrongConf_resist ? 200 : Conf_resist ? 40 : 8))) confdir();
		rx = u.ux + u.dx;
		ry = u.uy + u.dy;
		if(!isok(rx, ry)) {
			if (digtyp == 1) pline("Your %s bounces off harmlessly.",
				aobjnam(obj, (char *)0));
			else pline("Clash!");
			return(1);
		}
		lev = &levl[rx][ry];
		if(MON_AT(rx, ry) && attack(m_at(rx, ry)))
			return(1);
		dig_target = dig_typ(obj, rx, ry);
		if (dig_target == DIGTYP_UNDIGGABLE) {
			/* ACCESSIBLE or POOL */
			struct trap *trap = t_at(rx, ry);

			if (trap && trap->ttyp == WEB) {
			    if (!trap->tseen && !trap->hiddentrap) {
				seetrap(trap);
				There("is a spider web there!");
			    }
			    Your("%s entangled in the web.",
				aobjnam(obj, "become"));
			    /* you ought to be able to let go; tough luck */
			    /* (maybe `move_into_trap()' would be better) */
			    nomul(-d(2,2), "stuck in a spider web", TRUE);
			    nomovemsg = "You pull free.";
			} else if (lev->typ == IRONBARS) {
			    pline("Clang!");
			    wake_nearby();
			} else if (IS_TREE(lev->typ))
			    You("need an axe to cut down a tree.");
			else if (IS_FARMLAND(lev->typ))
			    You("need to use the force to remove farmland.");
			else if (IS_MOUNTAIN(lev->typ))
			    You("cannot dig out an entire mountain.");
			else if (IS_ROCK(lev->typ))
			    You("need a pick to dig rock.");
			else if (!ispick && (sobj_at(STATUE, rx, ry) ||
					     sobj_at(BOULDER, rx, ry))) {
			    boolean vibrate = !rn2(3);
			    pline("Sparks fly as you whack the %s.%s",
				sobj_at(STATUE, rx, ry) ? "statue" : "boulder",
				vibrate ? " The axe-handle vibrates violently!" : "");
			    if (vibrate) losehp(2, "axing a hard object", KILLED_BY);
			}
			else {
			    You("swing your %s through thin air.", aobjnam(obj, (char *)0));
				if (FunnyHallu) pline("It creates erotic air current noises.");
			}
		} else {
			static const char * const d_action[8][2] = {
			    {"swinging","slicing the air"},
			    {"digging","cutting through the wall"},
			    {"chipping the statue","cutting the statue"},
			    {"hitting the boulder","cutting through the boulder"},
			    {"chopping at the door","burning through the door"},
			    {"cutting the tree","razing the tree"},
			    {"smashing the bars","breaking the bars"},
			    {"smashing the obstruction","breaking the obstruction"}
			};
			did_dig_msg = FALSE;
			digging.quiet = FALSE;
			if (digging.pos.x != rx || digging.pos.y != ry ||
			    !on_level(&digging.level, &u.uz) || digging.down) {
			    if (flags.autodig &&
				dig_target == DIGTYP_ROCK && !digging.down &&
				digging.pos.x == u.ux &&
				digging.pos.y == u.uy &&
				(moves <= digging.lastdigtime+2 &&
				 moves >= digging.lastdigtime)) {
				/* avoid messages if repeated autodigging */
				did_dig_msg = TRUE;
				digging.quiet = TRUE;
			    }
			    digging.down = digging.chew = FALSE;
			    digging.warned = FALSE;
			    digging.pos.x = rx;
			    digging.pos.y = ry;
			    assign_level(&digging.level, &u.uz);
			    digging.effort = 0;
			    if (!digging.quiet)
				You("start %s.", d_action[dig_target][digtyp == 1]);
			} else {
			    You("%s %s.", digging.chew ? "begin" : "continue",
					d_action[dig_target][digtyp == 1]);
			    digging.chew = FALSE;
			}
			set_occupation(dig, verbing, 0);
			if (AutoDestruct || u.uprops[AUTO_DESTRUCT].extrinsic || (uarmf && uarmf->oartifact == ART_KHOR_S_REQUIRED_IDEA) || have_autodestructstone() || (uchain && uchain->oartifact == ART_SIYID) ) dig();
		}
	} else if (Is_airlevel(&u.uz) || Is_waterlevel(&u.uz)) {
		/* it must be air -- water checked above */
		You("swing your %s through thin air.", aobjnam(obj, (char *)0));
		if (FunnyHallu) pline("It creates erotic air current noises.");
	} else if (!can_reach_floor()) {
		You_cant("reach the %s.", surface(u.ux,u.uy));
	} else if (is_waterypool(u.ux, u.uy) || is_lava(u.ux, u.uy)) {
		/* Monsters which swim also happen not to be able to dig */
		You("cannot stay under%s long enough.",
				is_waterypool(u.ux, u.uy) ? "water" : " the lava");
	} else if (digtyp == 2) {
		Your("%s merely scratches the %s.",
				aobjnam(obj, (char *)0), surface(u.ux,u.uy));
		u_wipe_engr(3);
	} else {
		if (digging.pos.x != u.ux || digging.pos.y != u.uy ||
			!on_level(&digging.level, &u.uz) || !digging.down) {
		    digging.chew = FALSE;
		    digging.down = TRUE;
		    digging.warned = FALSE;
		    digging.pos.x = u.ux;
		    digging.pos.y = u.uy;
		    assign_level(&digging.level, &u.uz);
		    digging.effort = 0;
		    You("start %s downward.", verbing);
		    if (*u.ushops) shopdig(0);
		} else
		    You("continue %s downward.", verbing);
		did_dig_msg = FALSE;
		set_occupation(dig, verbing, 0);
		if (AutoDestruct || u.uprops[AUTO_DESTRUCT].extrinsic || (uarmf && uarmf->oartifact == ART_KHOR_S_REQUIRED_IDEA) || have_autodestructstone() || (uchain && uchain->oartifact == ART_SIYID) ) dig();
	}
	return(1);
}

/*
 * Town Watchmen frown on damage to the town walls, trees or fountains.
 * It's OK to dig holes in the ground, however.
 * If mtmp is assumed to be a watchman, a watchman is found if mtmp == 0
 * zap == TRUE if wand/spell of digging, FALSE otherwise (chewing)
 */
void
watch_dig(mtmp, x, y, zap)
    struct monst *mtmp;
    xchar x, y;
    boolean zap;
{
	struct rm *lev = &levl[x][y];

	if (in_town(x, y) &&
	    (closed_door(x, y) || lev->typ == SDOOR ||
	     IS_WALL(lev->typ) || IS_FOUNTAIN(lev->typ) || IS_TREE(lev->typ))) {
	    if (!mtmp) {
		for(mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		    if (DEADMONSTER(mtmp)) continue;
		    if ((mtmp->data == &mons[PM_WATCHMAN] || mtmp->data == &mons[PM_WATCH_LIEUTENANT] || mtmp->data == &mons[PM_WATCH_LEADER] || mtmp->data == &mons[PM_WATCH_CAPTAIN]) &&
			mtmp->mcansee && m_canseeu(mtmp) &&
			couldsee(mtmp->mx, mtmp->my) && mtmp->mpeaceful)
			break;
		}
	    }

	    if (mtmp) {
		if(zap || digging.warned) {
		    verbalize("Halt, vandal!  You're under arrest!");
		    (void) angry_guards(!(flags.soundok));
		} else {
		    const char *str;

		    if (IS_DOOR(lev->typ))
			str = "door";
		    else if (IS_TREE(lev->typ))
			str = "tree";
		    else if (IS_ROCK(lev->typ))
			str = "wall";
		    else
			str = "fountain";
		    verbalize("Hey, stop damaging that %s!", str);
		    digging.warned = TRUE;
		}
		if (is_digging())
		    stop_occupation();
	    }
	}
}

#endif /* OVLB */
#ifdef OVL0

/* Return TRUE if monster died, FALSE otherwise.  Called from m_move(). */
boolean
mdig_tunnel(mtmp)
register struct monst *mtmp;
{
	register struct rm *here;
	int pile = rnd(24); /* nerf by Amy - if this is lower than 5, create a pile, used to be rnd(12) */

	here = &levl[mtmp->mx][mtmp->my];
	if (here->typ == SDOOR)
	    cvt_sdoor_to_door(here);	/* ->typ = DOOR */

	/* Eats away door if present & closed or locked */
	if (closed_door(mtmp->mx, mtmp->my)) {
	    if (*in_rooms(mtmp->mx, mtmp->my, SHOPBASE))
		add_damage(mtmp->mx, mtmp->my, 0L);
	    unblock_point(mtmp->mx, mtmp->my);	/* vision */
		if (!(levl[mtmp->mx][mtmp->my].wall_info & W_HARDGROWTH)) levl[mtmp->mx][mtmp->my].wall_info |= W_EASYGROWTH;
	    if (here->doormask & D_TRAPPED) {
		here->doormask = D_NODOOR;
		if (mb_trapped(mtmp)) {	/* mtmp is killed */
		    newsym(mtmp->mx, mtmp->my);
		    return TRUE;
		}
	    } else {
		if (!rn2(3) && flags.verbose)	/* not too often.. */
		    You_feel("an unexpected draft.");
		here->doormask = D_BROKEN;
	    }
	    newsym(mtmp->mx, mtmp->my);
	    return FALSE;
	} else if ((!IS_ROCK(here->typ) && !IS_WATERTUNNEL(here->typ) && !IS_TREE(here->typ)) || IS_FARMLAND(here->typ) || IS_MOUNTAIN(here->typ) || IS_GRAVEWALL(here->typ)) /* no dig */
	    return FALSE;

	/* Only rock, trees, and walls fall through to this point. */
	if ((here->wall_info & W_NONDIGGABLE) != 0) { /* This is not a bug. So let's get rid of the message. --Amy */
	    /*impossible*//*pline("mdig_tunnel:  %s at (%d,%d) is undiggable",
		       (IS_WALL(here->typ) ? "wall" : IS_MOUNTAIN(here->typ) ? "mountain" : IS_FARMLAND(here->typ) ? "farmland" : "stone"),
		       (int) mtmp->mx, (int) mtmp->my);*/
	    return FALSE;	/* still alive */
	}

	if (IS_WALL(here->typ)) {

	    u.cnd_monsterdigamount++;

	    /* KMH -- Okay on arboreal levels (room walls are still stone) */
	    if (flags.soundok && flags.verbose && !rn2(5)) {
	    /* KMH -- Okay on arboreal levels (room walls are still stone) */
		You_hear("crashing rock.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Stena teper' ushla navsegda." : "Derrsch!");
	    }
	    if (*in_rooms(mtmp->mx, mtmp->my, SHOPBASE))
		add_damage(mtmp->mx, mtmp->my, 0L);
	    if (level.flags.is_maze_lev) {
		here->typ = ROOM;
	    } else if (level.flags.is_cavernous_lev &&
		       !in_town(mtmp->mx, mtmp->my)) {
		here->typ = CORR;
	    } else if (IS_WATERTUNNEL(here->typ)) {
		here->typ = MOAT;
	    } else if (IS_DIGGABLEWALL(here->typ)) {
		here->typ = CORR;
	    } else {
		here->typ = DOOR;
		here->doormask = D_NODOOR;
	    }
	} else if (IS_TREE(here->typ)) {
	    here->typ = ROOM;
	    if (!rn2(50) && !(here->looted & TREE_LOOTED) && pile && pile < 5) /* it shouldn't be that easy to get tree fruits... --Amy */
		(void) rnd_treefruit_at(mtmp->mx, mtmp->my);
	} else if (IS_WATERTUNNEL(here->typ)) {
	    u.cnd_monsterdigamount++;
	    here->typ = MOAT;
	} else {
	    u.cnd_monsterdigamount++;
	    here->typ = CORR;
	    if (!rn2(7) && pile && pile < 5) /* if you dig out rock yourself, you don't get boulders or rock either! --Amy */
	    (void) mksobj_at((pile == 1) ? BOULDER : ROCK,
			     mtmp->mx, mtmp->my, TRUE, FALSE, FALSE);
	}
	newsym(mtmp->mx, mtmp->my);
	if (!sobj_at(BOULDER, mtmp->mx, mtmp->my))
	    unblock_point(mtmp->mx, mtmp->my);	/* vision */
	if (!(levl[mtmp->mx][mtmp->my].wall_info & W_HARDGROWTH)) levl[mtmp->mx][mtmp->my].wall_info |= W_EASYGROWTH;

	return FALSE;
}

void
stardigging()
{
	struct rm *room;
	struct monst *mtmp;
        register struct obj *otmp, *next_obj;
	int zx, zy, digdepth;
	boolean shopdoor, shopwall, maze_dig;

	int diggingiteration = 0;

nextiteration:

	switch (diggingiteration) {

		case 0:
			u.dx = -1;
			u.dy = 0;
			break;
		case 1:
			u.dx = 1;
			u.dy = -1;
			break;
		case 2:
			u.dx = 1;
			u.dy = 0;
			break;
		case 3:
			u.dx = 1;
			u.dy = 1;
			break;
		case 4:
			u.dx = 0;
			u.dy = 1;
			break;
		case 5:
			u.dx = -1;
			u.dy = 1;
			break;
		case 6:
			u.dx = -1;
			u.dy = -1;
			break;
		case 7:
			u.dx = 0;
			u.dy = -1;
			break;

	}

	/* normal case: digging across the level */
	shopdoor = shopwall = FALSE;
	maze_dig = level.flags.is_maze_lev && !Is_earthlevel(&u.uz);
	zx = u.ux + u.dx;
	zy = u.uy + u.dy;
	digdepth = rn1(18, 8);
	if (Race_if(PM_IRAHA)) digdepth *= 2;
	tmp_at(DISP_BEAM, cmap_to_glyph(S_digbeam));
	while (--digdepth >= 0) {
	    if (!isok(zx,zy)) break;
	    room = &levl[zx][zy];
	    tmp_at(zx,zy);
	    delay_output();	/* wait a little bit */

            /* WAC check for monster, boulder */
            if ((mtmp = m_at(zx, zy)) != 0) {
                if (made_of_rock(mtmp->data)) {
                    You("gouge a hole in %s!", mon_nam(mtmp));
                    mtmp->mhp /= 2;
                    if (mtmp->mhp < 1) mtmp->mhp = 1;
		    setmangry(mtmp);
                } else pline("%s is unaffected!", Monnam(mtmp));
            }
            for(otmp = level.objects[zx][zy]; otmp; otmp = next_obj) {
                next_obj = otmp->nexthere;
		/* vaporize boulders */
                if (otmp->otyp == BOULDER) {
		    delobj(otmp);
		    /* A little Sokoban guilt... */
		    if (In_sokoban(&u.uz) && !(uwep && uwep->oartifact == ART_HOPE_OF_SOKOBAN && rn2(3)) )
			{change_luck(-1);
			pline("You cheater!");
			if (evilfriday) u.ugangr++;
			}
		    unblock_point(zx, zy);
		    newsym(zx, zy);
		    pline_The("boulder is vaporized!");
		}
		break;
            }

	    if (closed_door(zx, zy) || room->typ == SDOOR) {
		/* ALI - Artifact doors */
		if (artifact_door(zx, zy)) {
		    if (cansee(zx, zy))
			pline_The("door glows then fades.");
		    break;
		}
		if (*in_rooms(zx,zy,SHOPBASE)) {
		    add_damage(zx, zy, 400L);
		    shopdoor = TRUE;
		}
		if (room->typ == SDOOR)
		    room->typ = DOOR;
		else if (cansee(zx, zy))
		    pline_The("door is razed!");
		watch_dig((struct monst *)0, zx, zy, TRUE);
		room->doormask = D_NODOOR;
		unblock_point(zx,zy); /* vision */
		digdepth -= 2;
		if (issoviet && maze_dig) pline("Vy ne mozhete kopat'! Poskol'ku Sovetskiy Pyat' Lo nenavidit vashi kishki!");
		if (issoviet && maze_dig) break;
	    } else if (maze_dig) {
		if (IS_WATERTUNNEL(room->typ)) {
			room->typ = MOAT;
			unblock_point(zx,zy); /* vision */
			digdepth -= 2; /* fix stupidity --Amy */
			if (issoviet) pline("Vy ne mozhete kopat'! Poskol'ku Sovetskiy Pyat' Lo nenavidit vashi kishki!");
			if (issoviet) break;

		} if (IS_WALL(room->typ) && !(IS_FARMLAND(room->typ)) && !(IS_MOUNTAIN(room->typ)) && !(IS_GRAVEWALL(room->typ)) ) {
		    if (!(room->wall_info & W_NONDIGGABLE)) {
			if (*in_rooms(zx,zy,SHOPBASE)) {
			    add_damage(zx, zy, 200L);
			    shopwall = TRUE;
			}
			room->typ = /*ROOM*/CORR;
			unblock_point(zx,zy); /* vision */
			if (!(levl[zx][zy].wall_info & W_HARDGROWTH)) levl[zx][zy].wall_info |= W_EASYGROWTH;
			digdepth -= 2; /* fix stupidity --Amy */
			if (issoviet) pline("Vy ne mozhete kopat'! Poskol'ku Sovetskiy Pyat' Lo nenavidit vashi kishki!");
			if (issoviet) break;
		    } else if (!Blind) {
			pline_The("wall glows then fades.");
			break;
		    }
		} else if (IS_TREE(room->typ)) { /* check trees before stone */
		    if (!(room->wall_info & W_NONDIGGABLE)) {
			room->typ = ROOM;
			unblock_point(zx,zy); /* vision */
			if (!(levl[zx][zy].wall_info & W_HARDGROWTH)) levl[zx][zy].wall_info |= W_EASYGROWTH;
			digdepth -= 2; /* fix stupidity --Amy */
			if (issoviet) pline("Vy ne mozhete kopat'! Poskol'ku Sovetskiy Pyat' Lo nenavidit vashi kishki!");
			if (issoviet) break;
		    } else if (!Blind) {
			pline_The("tree shudders but is unharmed.");
			break;
		    }
		} else if (room->typ == STONE || room->typ == SCORR) {
		    if (!(room->wall_info & W_NONDIGGABLE)) {
			room->typ = CORR;
			unblock_point(zx,zy); /* vision */
			if (!(levl[zx][zy].wall_info & W_HARDGROWTH)) levl[zx][zy].wall_info |= W_EASYGROWTH;
			digdepth--; /* fix stupidity --Amy */
		/* In Soviet Russia, digging has to be done one block at a time. Faster digging methods are capitalistic
		 * and evil, so they're not allowed. I wonder how long until they decide to make mazes undiggable... --Amy */
			if (issoviet) pline("Vy ne mozhete kopat'! Poskol'ku Sovetskiy Pyat' Lo nenavidit vashi kishki!");
			if (issoviet) break;
		    } else if (!Blind) {
			pline_The("rock glows then fades.");
			break;
		    }
		}
	    } else if (IS_ROCK(room->typ) || IS_WATERTUNNEL(room->typ)) {
		if (!may_dig(zx,zy)) break;
		if (IS_WALL(room->typ) || room->typ == SDOOR) {
		    if (*in_rooms(zx,zy,SHOPBASE)) {
			add_damage(zx, zy, 200L);
			shopwall = TRUE;
		    }
		    watch_dig((struct monst *)0, zx, zy, TRUE);
		    if (level.flags.is_cavernous_lev && !in_town(zx, zy)) {
			room->typ = CORR;
		    } else if (IS_WATERTUNNEL(room->typ)) {
			room->typ = MOAT;
		    } else if (IS_DIGGABLEWALL(room->typ)) {
			room->typ = CORR;
		    } else {
			room->typ = DOOR;
			room->doormask = D_NODOOR;
		    }
		    digdepth -= 2;
		} else if (IS_TREE(room->typ)) {
		    room->typ = ROOM;
		    digdepth -= 2;
		} else if (IS_WATERTUNNEL(room->typ)) {
		    room->typ = MOAT;
		    digdepth -= 2;
		} else {	/* IS_ROCK but not IS_WALL or SDOOR */
		    room->typ = CORR;
		    digdepth--;
		}
		unblock_point(zx,zy); /* vision */
		if (!(levl[zx][zy].wall_info & W_HARDGROWTH)) levl[zx][zy].wall_info |= W_EASYGROWTH;
	    }
	    zx += u.dx;
	    zy += u.dy;
	} /* while */
	tmp_at(DISP_END,0);	/* closing call */
	if (shopdoor || shopwall)
	    pay_for_damage(shopdoor ? "destroy" : "dig into", FALSE);

	diggingiteration++;

	if (diggingiteration < 8) goto nextiteration;

	return;

}

#endif /* OVL0 */
#ifdef OVL3

/* digging via wand zap or spell cast */
void
zap_dig(bigrange)
boolean bigrange;
{
	struct rm *room;
	struct monst *mtmp;
/*        struct obj *otmp;*/
        register struct obj *otmp, *next_obj;
	int zx, zy, digdepth;
	boolean shopdoor, shopwall, maze_dig;
	/*
	 * Original effect (approximately):
	 * from CORR: dig until we pierce a wall
	 * from ROOM: pierce wall and dig until we reach
	 * an ACCESSIBLE place.
	 * Currently: dig for digdepth positions;
	 * also down on request of Lennart Augustsson.
	 */

	if (u.uswallow) {
	    mtmp = u.ustuck;

	    if (!is_whirly(mtmp->data)) {
		if (is_animal(mtmp->data))
		    You("pierce %s %s wall!",
			s_suffix(mon_nam(mtmp)), mbodypart(mtmp, STOMACH));
		mtmp->mhp *= 4;
		mtmp->mhp /= 5; /* prevent easy Jubilex killing --Amy */
		if (bigrange) {
			mtmp->mhp *= 3;
			mtmp->mhp /= 5;
		}
		if (mtmp->mhp < 1) mtmp->mhp = 1; /* fixes the bug where killing gray newts causes error messages */
		expels(mtmp, mtmp->data, !is_animal(mtmp->data));
	    }
	    return;
	} /* swallowed */

	if (u.dz) {
	    if (!Is_airlevel(&u.uz) && !Is_waterlevel(&u.uz) && !Underwater) {
		if (u.dz < 0 || On_stairs(u.ux, u.uy)) {
		    if (On_stairs(u.ux, u.uy))
			pline_The("beam bounces off the %s and hits the %s.",
			      (u.ux == xdnladder || u.ux == xupladder) ?
			      "ladder" : "stairs", ceiling(u.ux, u.uy));
		    You("loosen a rock from the %s.", ceiling(u.ux, u.uy));
		    pline("It falls on your %s!", body_part(HEAD));
		    losehp(rnd((uarmh && is_hardmaterial(uarmh)) ? 2 : 6),
			   "falling rock", KILLED_BY_AN);
		    otmp = mksobj_at(ROCK, u.ux, u.uy, FALSE, FALSE, FALSE);
		    if (otmp) {

			if(!rn2(8)) {
				otmp->spe = rne(2);
				if (rn2(2)) otmp->blessed = rn2(2);
				 else	blessorcurse(otmp, 3);
			} else if(!rn2(10)) {
				if (rn2(10)) curse(otmp);
				 else	blessorcurse(otmp, 3);
				otmp->spe = -rne(2);
			} else	blessorcurse(otmp, 10);

			(void)xname(otmp);	/* set dknown, maybe bknown */
			stackobj(otmp);
		    }
		    newsym(u.ux, u.uy);
		} else {
		    watch_dig((struct monst *)0, u.ux, u.uy, TRUE);
		    (void) dighole(FALSE);
		}
	    }
	    return;
	} /* up or down */

	/* normal case: digging across the level */
	shopdoor = shopwall = FALSE;
	maze_dig = level.flags.is_maze_lev && !Is_earthlevel(&u.uz);
	zx = u.ux + u.dx;
	zy = u.uy + u.dy;
	digdepth = bigrange ? rn1(18, 8) : 1;
	if (Race_if(PM_IRAHA)) digdepth *= 2;
	tmp_at(DISP_BEAM, cmap_to_glyph(S_digbeam));
	while (--digdepth >= 0) {
	    if (!isok(zx,zy)) break;
	    room = &levl[zx][zy];
	    tmp_at(zx,zy);
	    delay_output();	/* wait a little bit */

            /* WAC check for monster, boulder */
            if ((mtmp = m_at(zx, zy)) != 0) {
                if (made_of_rock(mtmp->data)) {
                    You("gouge a hole in %s!", mon_nam(mtmp));
                    mtmp->mhp /= 2;
                    if (mtmp->mhp < 1) mtmp->mhp = 1;
		    setmangry(mtmp);
                } else pline("%s is unaffected!", Monnam(mtmp));
            }
            for(otmp = level.objects[zx][zy]; otmp; otmp = next_obj) {
                next_obj = otmp->nexthere;
		/* vaporize boulders */
                if (otmp->otyp == BOULDER) {
		    delobj(otmp);
		    /* A little Sokoban guilt... */
		    if (In_sokoban(&u.uz) && !(uwep && uwep->oartifact == ART_HOPE_OF_SOKOBAN && rn2(3)) )
			{change_luck(-1);
			pline("You cheater!");
			if (evilfriday) u.ugangr++;
			}
		    unblock_point(zx, zy);
		    newsym(zx, zy);
		    pline_The("boulder is vaporized!");
		}
		break;
            }

	    if (closed_door(zx, zy) || room->typ == SDOOR) {
		/* ALI - Artifact doors */
		if (artifact_door(zx, zy)) {
		    if (cansee(zx, zy))
			pline_The("door glows then fades.");
		    break;
		}
		if (*in_rooms(zx,zy,SHOPBASE)) {
		    add_damage(zx, zy, 400L);
		    shopdoor = TRUE;
		}
		if (room->typ == SDOOR)
		    room->typ = DOOR;
		else if (cansee(zx, zy))
		    pline_The("door is razed!");
		watch_dig((struct monst *)0, zx, zy, TRUE);
		room->doormask = D_NODOOR;
		unblock_point(zx,zy); /* vision */
		digdepth -= 2;
		if (issoviet && maze_dig) pline("Vy ne mozhete kopat'! Poskol'ku Sovetskiy Pyat' Lo nenavidit vashi kishki!");
		if (issoviet && maze_dig) break;
	    } else if (maze_dig) {
		if (IS_WATERTUNNEL(room->typ)) {
			room->typ = MOAT;
			unblock_point(zx,zy); /* vision */
			digdepth -= 2; /* fix stupidity --Amy */
			if (issoviet) pline("Vy ne mozhete kopat'! Poskol'ku Sovetskiy Pyat' Lo nenavidit vashi kishki!");
			if (issoviet) break;

		} if (IS_WALL(room->typ) && !(IS_FARMLAND(room->typ)) && !(IS_MOUNTAIN(room->typ)) && !(IS_GRAVEWALL(room->typ)) ) {
		    if (!(room->wall_info & W_NONDIGGABLE)) {
			if (*in_rooms(zx,zy,SHOPBASE)) {
			    add_damage(zx, zy, 200L);
			    shopwall = TRUE;
			}
			room->typ = /*ROOM*/CORR;
			unblock_point(zx,zy); /* vision */
			if (!(levl[zx][zy].wall_info & W_HARDGROWTH)) levl[zx][zy].wall_info |= W_EASYGROWTH;
			digdepth -= 2; /* fix stupidity --Amy */
			if (issoviet) pline("Vy ne mozhete kopat'! Poskol'ku Sovetskiy Pyat' Lo nenavidit vashi kishki!");
			if (issoviet) break;
		    } else if (!Blind) {
			pline_The("wall glows then fades.");
			break;
		    }
		} else if (IS_TREE(room->typ)) { /* check trees before stone */
		    if (!(room->wall_info & W_NONDIGGABLE)) {
			room->typ = ROOM;
			unblock_point(zx,zy); /* vision */
			if (!(levl[zx][zy].wall_info & W_HARDGROWTH)) levl[zx][zy].wall_info |= W_EASYGROWTH;
			digdepth -= 2; /* fix stupidity --Amy */
			if (issoviet) pline("Vy ne mozhete kopat'! Poskol'ku Sovetskiy Pyat' Lo nenavidit vashi kishki!");
			if (issoviet) break;
		    } else if (!Blind) {
			pline_The("tree shudders but is unharmed.");
			break;
		    }
		} else if (room->typ == STONE || room->typ == SCORR) {
		    if (!(room->wall_info & W_NONDIGGABLE)) {
			room->typ = CORR;
			unblock_point(zx,zy); /* vision */
			if (!(levl[zx][zy].wall_info & W_HARDGROWTH)) levl[zx][zy].wall_info |= W_EASYGROWTH;
			digdepth--; /* fix stupidity --Amy */
		/* In Soviet Russia, digging has to be done one block at a time. Faster digging methods are capitalistic
		 * and evil, so they're not allowed. I wonder how long until they decide to make mazes undiggable... --Amy */
			if (issoviet) pline("Vy ne mozhete kopat'! Poskol'ku Sovetskiy Pyat' Lo nenavidit vashi kishki!");
			if (issoviet) break;
		    } else if (!Blind) {
			pline_The("rock glows then fades.");
			break;
		    }
		}
	    } else if (IS_ROCK(room->typ) || IS_WATERTUNNEL(room->typ)) {
		if (!may_dig(zx,zy)) break;
		if (IS_WALL(room->typ) || room->typ == SDOOR) {
		    if (*in_rooms(zx,zy,SHOPBASE)) {
			add_damage(zx, zy, 200L);
			shopwall = TRUE;
		    }
		    watch_dig((struct monst *)0, zx, zy, TRUE);
		    if (level.flags.is_cavernous_lev && !in_town(zx, zy)) {
			room->typ = CORR;
		    } else if (IS_WATERTUNNEL(room->typ)) {
			room->typ = MOAT;
		    } else if (IS_DIGGABLEWALL(room->typ)) {
			room->typ = CORR;
		    } else {
			room->typ = DOOR;
			room->doormask = D_NODOOR;
		    }
		    digdepth -= 2;
		} else if (IS_TREE(room->typ)) {
		    room->typ = ROOM;
		    digdepth -= 2;
		} else if (IS_WATERTUNNEL(room->typ)) {
		    room->typ = MOAT;
		    digdepth -= 2;
		} else {	/* IS_ROCK but not IS_WALL or SDOOR */
		    room->typ = CORR;
		    digdepth--;
		}
		unblock_point(zx,zy); /* vision */
		if (!(levl[zx][zy].wall_info & W_HARDGROWTH)) levl[zx][zy].wall_info |= W_EASYGROWTH;
	    }
	    zx += u.dx;
	    zy += u.dy;
	} /* while */
	tmp_at(DISP_END,0);	/* closing call */
	if (shopdoor || shopwall)
	    pay_for_damage(shopdoor ? "destroy" : "dig into", FALSE);
	return;
}

/* move objects from fobj/nexthere lists to buriedobjlist, keeping position */
/* information */
struct obj *
bury_an_obj(otmp)
	struct obj *otmp;
{
	struct obj *otmp2;
	boolean under_ice;

#ifdef DEBUG
	pline("bury_an_obj: %s", xname(otmp));
#endif
	if (otmp == uball)
		unpunish();
	/* after unpunish(), or might get deallocated chain */
	otmp2 = otmp->nexthere;
	/*
	 * obj_resists(,0,0) prevents Rider corpses from being buried.
	 * It also prevents The Amulet and invocation tools from being
	 * buried.  Since they can't be confined to bags and statues,
	 * it makes sense that they can't be buried either, even though
	 * the real reason there (direct accessibility when carried) is
	 * completely different.
	 */
	if (otmp == uchain || obj_resists(otmp, 0, 0))
		return(otmp2);

	if (otmp->otyp == LEATHER_LEASH && otmp->leashmon != 0)
		o_unleash(otmp);

	if (otmp->otyp == INKA_LEASH && otmp->leashmon != 0)
		o_unleash(otmp);

	if (otmp->otyp == ADAMANT_LEASH && otmp->leashmon != 0)
		o_unleash(otmp);

	if (otmp == usaddle)
		dismount_steed(DISMOUNT_GENERIC);

	if (otmp->lamplit && otmp->otyp != POT_OIL)
		end_burn(otmp, TRUE);

	obj_extract_self(otmp);

	under_ice = is_ice(otmp->ox, otmp->oy);
	if (otmp->otyp == ROCK && !under_ice) {
		/* merges into burying material */
		obfree(otmp, (struct obj *)0);
		return(otmp2);
	}
	/*
	 * Start a rot on organic material.  Not corpses -- they
	 * are already handled.
	 */
	if (otmp->otyp == CORPSE) {
	    ;		/* should cancel timer if under_ice */
	} else if ((under_ice ? otmp->oclass == POTION_CLASS : is_organic(otmp))
		&& !obj_resists(otmp, 5, 95)) {
	    (void) start_timer((under_ice ? 0L : 250L) + (long)rnd(250),
			       TIMER_OBJECT, ROT_ORGANIC, (void *)otmp);
	}
	add_to_buried(otmp);
	return(otmp2);
}

void
bury_objs(x, y)
int x, y;
{
	struct obj *otmp, *otmp2;

#ifdef DEBUG
	if(level.objects[x][y] != (struct obj *)0)
		pline("bury_objs: at %d, %d", x, y);
#endif
	for (otmp = level.objects[x][y]; otmp; otmp = otmp2)
		otmp2 = bury_an_obj(otmp);

	/* don't expect any engravings here, but just in case */
	del_engr_at(x, y);
	newsym(x, y);
}

/* move objects from buriedobjlist to fobj/nexthere lists */
void
unearth_objs(x, y)
int x, y;
{
	struct obj *otmp, *otmp2;

#ifdef DEBUG
	pline("unearth_objs: at %d, %d", x, y);
#endif
	for (otmp = level.buriedobjlist; otmp; otmp = otmp2) {
		otmp2 = otmp->nobj;
		if (otmp->ox == x && otmp->oy == y) {
		    obj_extract_self(otmp);
		    if (otmp->timed)
			(void) stop_timer(ROT_ORGANIC, (void *)otmp);
		    place_object(otmp, x, y);
		    stackobj(otmp);
		}
	}
	del_engr_at(x, y);
	newsym(x, y);
}

/*
 * The organic material has rotted away while buried.  As an expansion,
 * we could add add partial damage.  A damage count is kept in the object
 * and every time we are called we increment the count and reschedule another
 * timeout.  Eventually the object rots away.
 *
 * This is used by buried objects other than corpses.  When a container rots
 * away, any contents become newly buried objects.
 */
/* ARGSUSED */
void
rot_organic(arg, timeout)
void * arg;
long timeout;	/* unused */
{
#if defined(MAC_MPW)
# pragma unused ( timeout )
#endif
	struct obj *obj = (struct obj *) arg;

	/* nope, you do not get to retrieve the medical kit contents that way, you filthy exploit exploiter --Amy */
	if (obj && obj->otyp == MEDICAL_KIT && Has_contents(obj)) {
		delete_contents(obj);
	}

	while (Has_contents(obj)) {
	    /* We don't need to place contained object on the floor
	       first, but we do need to update its map coordinates. */
	    obj->cobj->ox = obj->ox,  obj->cobj->oy = obj->oy;
	    /* Everything which can be held in a container can also be
	       buried, so bury_an_obj's use of obj_extract_self insures
	       that Has_contents(obj) will eventually become false. */
	    (void)bury_an_obj(obj->cobj);
	}
	obj_extract_self(obj);
	obfree(obj, (struct obj *) 0);
}

/*
 * Called when a corpse has rotted completely away.
 */
void
rot_corpse(arg, timeout)
void * arg;
long timeout;	/* unused */
{
	xchar x = 0, y = 0;
	struct obj *obj = (struct obj *) arg;
	boolean on_floor = obj->where == OBJ_FLOOR,
                in_minvent = obj->where == OBJ_MINVENT,
		in_invent = obj->where == OBJ_INVENT;

	if (on_floor) {
	    x = obj->ox;
	    y = obj->oy;
        } else if (in_minvent) {
            /* WAC unwield if wielded */
            if (MON_WEP(obj->ocarry) && MON_WEP(obj->ocarry) == obj) {
		    obj->owornmask &= ~W_WEP;
                    MON_NOWEP(obj->ocarry);
	    }
	} else if (in_invent) {
	    if (flags.verbose) {
		char *cname = corpse_xname(obj, FALSE);
		Your("%s%s %s away%c",
		     obj == uwep ? "wielded " : nul, cname,
		     otense(obj, "rot"), obj == uwep ? '!' : '.');
	    }
	    if (obj == uwep) {
		uwepgone();	/* now bare handed */
		stop_occupation();
	    } else if (obj == uswapwep) {
		uswapwepgone();
		stop_occupation();
	    } else if (obj == uquiver) {
		uqwepgone();
		stop_occupation();
	    } else if (obj == uswapwep) {
		uswapwepgone();
		stop_occupation();
	    } else if (obj == uquiver) {
		uqwepgone();
		stop_occupation();
	    }
	} else if (obj->where == OBJ_MINVENT && obj->owornmask) {
	    if (obj == MON_WEP(obj->ocarry)) {
		setmnotwielded(obj->ocarry,obj);
		MON_NOWEP(obj->ocarry);
	    }
	}
	rot_organic(arg, timeout);
	if (on_floor) newsym(x, y);
	else if (in_invent) update_inventory();
}

#if 0
void
bury_monst(mtmp)
struct monst *mtmp;
{
#ifdef DEBUG
	pline("bury_monst: %s", mon_nam(mtmp));
#endif
	if(canseemon(mtmp)) {
	    if(is_flyer(mtmp->data) || mtmp->egotype_flying || is_floater(mtmp->data)) {
		pline_The("%s opens up, but %s is not swallowed!",
			surface(mtmp->mx, mtmp->my), mon_nam(mtmp));
		return;
	    } else
	        pline_The("%s opens up and swallows %s!",
			surface(mtmp->mx, mtmp->my), mon_nam(mtmp));
	}

	mtmp->mburied = TRUE;
	wakeup(mtmp);			/* at least give it a chance :-) */
	newsym(mtmp->mx, mtmp->my);
}

void
bury_you()
{
#ifdef DEBUG
	pline("bury_you");
#endif
	/* KMH, balance patch -- new intrinsic */
    if (!Levitation && !Flying) {
	if(u.uswallow)
	    You_feel("a sensation like falling into a trap!");
	else
	    pline_The("%s opens beneath you and you fall in!",
		  surface(u.ux, u.uy));

	u.uburied = TRUE;
	if(!Strangled && !Breathless) Strangled = 11;
	under_ground(1);
    }
}

void
unearth_you()
{
#ifdef DEBUG
	pline("unearth_you");
#endif
	u.uburied = FALSE;
	under_ground(0);
	if(!uamul || uamul->otyp != AMULET_OF_STRANGULATION)
		Strangled = 0;
	vision_recalc(0);
}

void
escape_tomb()
{
#ifdef DEBUG
	pline("escape_tomb");
#endif
	if ((Teleportation || can_teleport(youmonst.data)) &&
	    (Teleport_control || rn2(3) < Luck+2)) {
		You("attempt a teleport spell.");
		(void) dotele_post(FALSE);	/* calls unearth_you() */
	} else if(u.uburied) { /* still buried after 'port attempt */
		boolean good;

		if(amorphous(youmonst.data) || Passes_walls ||
		   noncorporeal(youmonst.data) || unsolid(youmonst.data) || (uarmf && uarmf->oartifact == ART_STONEWALL_CHECKERBOARD_DIS) || (Race_if(PM_SCURRIER) && !Upolyd) ||
		   (tunnels(youmonst.data) && !needspick(youmonst.data))) {

		    You("%s up through the %s.",
			(tunnels(youmonst.data) && !needspick(youmonst.data)) ?
			 "try to tunnel" : (uarmf && uarmf->oartifact == ART_STONEWALL_CHECKERBOARD_DIS) ? "try to tunnel" :  (Race_if(PM_SCURRIER) && !Upolyd) ? "try to tunnel" : (amorphous(youmonst.data)) ?
			 "ooze" : "phase", surface(u.ux, u.uy));

		    if(tunnels(youmonst.data) && !needspick(youmonst.data))
			good = dighole(TRUE);
		    else if (Race_if(PM_SCURRIER) && !Upolyd)
			good = dighole(TRUE);
		    else if (uarmf && uarmf->oartifact == ART_STONEWALL_CHECKERBOARD_DIS)
			good = dighole(TRUE);
		    else good = TRUE;
		    if(good) unearth_you();
		}
	}
}

void
bury_obj(otmp)
struct obj *otmp;
{

#ifdef DEBUG
	pline("bury_obj");
#endif
	if(cansee(otmp->ox, otmp->oy))
	   pline_The("objects on the %s tumble into a hole!",
		surface(otmp->ox, otmp->oy));

	bury_objs(otmp->ox, otmp->oy);
}
#endif

#ifdef DEBUG
int
wiz_debug_cmd() /* in this case, bury everything at your loc and around */
{
	int x, y;

	for (x = u.ux - 1; x <= u.ux + 1; x++)
	    for (y = u.uy - 1; y <= u.uy + 1; y++)
		if (isok(x,y)) bury_objs(x,y);
	return 0;
}

#endif /* DEBUG */
#endif /* OVL3 */

/*dig.c*/
