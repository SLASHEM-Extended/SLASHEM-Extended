/*	SCCS Id: @(#)trap.c	3.4	2003/10/20	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"


/* If you are a modderling looking to deconstruct SLEX, I hope that you'll have to waste lots of free time looking for
 * all the traps you're going to remove, because they're meant to stay :P --Amy */


extern const char * const destroy_strings[];	/* from zap.c */

STATIC_DCL void rocks_fall(int, int);

STATIC_DCL void doplasmatrap(struct obj *);
STATIC_DCL void doicetrap(struct obj *);
STATIC_DCL void doshocktrap(struct obj *);
STATIC_DCL void dovolttrap(struct obj *);
STATIC_DCL void domagictrap(void);
STATIC_DCL boolean emergency_disrobe(boolean *);
STATIC_DCL int untrap_prob(struct trap *ttmp);
STATIC_DCL void cnv_trap_obj(int, int, struct trap *);
STATIC_DCL void move_into_trap(struct trap *);
STATIC_DCL int try_disarm(struct trap *,BOOLEAN_P,BOOLEAN_P);
STATIC_DCL void reward_untrap(struct trap *, struct monst *);
/*
STATIC_DCL int disarm_holdingtrap(struct trap *);
STATIC_DCL int disarm_unknowntrap(struct trap *);
STATIC_DCL int disarm_fartingtrap(struct trap *);
STATIC_DCL int disarm_active_superscroller(struct trap *);
STATIC_DCL int disarm_trap_percents(struct trap *);
STATIC_DCL int disarm_rust_trap(struct trap *);
STATIC_DCL int disarm_acid_trap(struct trap *);
STATIC_DCL int disarm_water_trap(struct trap *);
STATIC_DCL int disarm_heel_trap(struct trap *);
STATIC_DCL int disarm_glyph_trap(struct trap *, int);
STATIC_DCL int disarm_blade_trap(struct trap *);
STATIC_DCL int disarm_spined_ball(struct trap *);
STATIC_DCL int disarm_pendulum(struct trap *);
STATIC_DCL int disarm_difficult_trap(struct trap *);
STATIC_DCL int disarm_mace_trap(struct trap *);
STATIC_DCL int disarm_dagger_trap(struct trap *);
STATIC_DCL int disarm_fire_trap(struct trap *);
STATIC_DCL int disarm_landmine(struct trap *);
STATIC_DCL int disarm_squeaky_board(struct trap *);
STATIC_DCL int disarm_shooting_trap(struct trap *, int);
STATIC_DCL int disarm_hard_shooting_trap(struct trap *, int);
*/
STATIC_DCL int try_lift(struct monst *, struct trap *, int, BOOLEAN_P);
STATIC_DCL int help_monster_out(struct monst *, struct trap *);
STATIC_DCL boolean thitm(int,struct monst *,struct obj *,int,BOOLEAN_P);
STATIC_DCL int mkroll_launch(struct trap *,XCHAR_P,XCHAR_P,SHORT_P,long);
STATIC_DCL boolean isclearpath(coord *, int, SCHAR_P, SCHAR_P);
STATIC_OVL int steedintrap(struct trap *, struct obj *);
STATIC_OVL boolean keep_saddle_with_steedcorpse(unsigned, struct obj *, struct obj *);

#ifndef OVLB
STATIC_VAR const char *a_your[2];
STATIC_VAR const char *A_Your[2];
STATIC_VAR const char tower_of_flame[];
STATIC_VAR const char *A_gush_of_water_hits;
STATIC_VAR const char * const blindgas[6];

#else

STATIC_VAR const char * const a_your[2] = { "a", "your" };
STATIC_VAR const char * const A_Your[2] = { "A", "Your" };
STATIC_VAR const char tower_of_flame[] = "tower of flame";
STATIC_VAR const char * const A_gush_of_water_hits = "A gush of water hits";
STATIC_VAR const char * const blindgas[6] = 
	{"humid", "odorless", "pungent", "chilling", "acrid", "biting"};

#endif /* OVLB */

static const char all_count[] = { ALLOW_COUNT, ALL_CLASSES, 0};
static const char allnoncount[] = { ALL_CLASSES, 0};

static const char * const farttrapnames[] = {
/* always 5 names per line, because we'll need the index numbers later --Amy
 * if this changes, keep mkmaze.c bound_digging() in sync for no fun walls trap */

/* quiet */
"Nadja", "Mailie", "Elif", "Solvejg", "Sueschen", 
"Jessica", "Yvonne", "Patricia", "Jennifer", "Inge", 
"Sarah", "Birgit", /* 0-11 */

/* normal */
"Kati", "Maurah", "Eveline", "Larissa", "Sandra", 
"Meltem", "Kerstin", "Karin", "Ina", "Lou", 
"Lisa", "Miriam", "Elena", "Katharina", "Simone", 
"Jasieen", "Marike", "Sue Lyn", "Marleen", "Claudia",
"Ksenia", /* 12-32 */

/* loud */
"Sunali", "Thai", "Klara", "Ludgera", "Johanetta", 
"Antje", "Ruea", "Mariya", "Wendy", "Katia", /* 33-42 */

};

#ifdef OVLB

STATIC_PTR void
do_lockfloodd(x, y, poolcnt)
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

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && ((levl[randomx][randomy].wall_info & W_NONDIGGABLE) == 0) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR || (levl[randomx][randomy].typ == DOOR && levl[randomx][randomy].doormask == D_NODOOR) ) ) {

			if (rn2(3)) doorlockX(randomx, randomy, TRUE);
			else {
				if (levl[randomx][randomy].typ != DOOR) levl[randomx][randomy].typ = STONE;
				else levl[randomx][randomy].typ = CROSSWALL;
				blockorunblock_point(randomx,randomy);
				if (!(levl[randomx][randomy].wall_info & W_EASYGROWTH)) levl[randomx][randomy].wall_info |= W_HARDGROWTH;
				del_engr_at(randomx, randomy);

				if ((mtmp = m_at(randomx, randomy)) != 0) {
					(void) minliquid(mtmp);
				} else {
					newsym(randomx,randomy);
				}

			}
		}
	}
	if (rn2(3)) doorlockX(x, y, TRUE);

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].wall_info & W_NONDIGGABLE) != 0 || (levl[x][y].typ != CORR && levl[x][y].typ != ROOM && (levl[x][y].typ != DOOR || levl[x][y].doormask != D_NODOOR) ))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a wall at x, y */
		if (levl[x][y].typ != DOOR) levl[x][y].typ = STONE;
		else levl[x][y].typ = CROSSWALL;
		blockorunblock_point(x,y);
		if (!(levl[x][y].wall_info & W_EASYGROWTH)) levl[x][y].wall_info |= W_HARDGROWTH;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_treefloodd(x, y, poolcnt)
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

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = TREE;
			blockorunblock_point(randomx,randomy);
			if (!(levl[randomx][randomy].wall_info & W_EASYGROWTH)) levl[randomx][randomy].wall_info |= W_HARDGROWTH;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = TREE;
		blockorunblock_point(x,y);
		if (!(levl[x][y].wall_info & W_EASYGROWTH)) levl[x][y].wall_info |= W_HARDGROWTH;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_gravefloode(x, y, poolcnt)
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

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = GRAVEWALL;
			blockorunblock_point(randomx,randomy);
			if (!(levl[randomx][randomy].wall_info & W_EASYGROWTH)) levl[randomx][randomy].wall_info |= W_HARDGROWTH;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = GRAVEWALL;
		blockorunblock_point(x,y);
		if (!(levl[x][y].wall_info & W_EASYGROWTH)) levl[x][y].wall_info |= W_HARDGROWTH;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_tunnelfloode(x, y, poolcnt)
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

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = TUNNELWALL;
			blockorunblock_point(randomx,randomy);
			if (!(levl[randomx][randomy].wall_info & W_EASYGROWTH)) levl[randomx][randomy].wall_info |= W_HARDGROWTH;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = TUNNELWALL;
		blockorunblock_point(x,y);
		if (!(levl[x][y].wall_info & W_EASYGROWTH)) levl[x][y].wall_info |= W_HARDGROWTH;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_farmfloode(x, y, poolcnt)
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

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = FARMLAND;
			blockorunblock_point(randomx,randomy);
			if (!(levl[randomx][randomy].wall_info & W_EASYGROWTH)) levl[randomx][randomy].wall_info |= W_HARDGROWTH;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = FARMLAND;
		blockorunblock_point(x,y);
		if (!(levl[x][y].wall_info & W_EASYGROWTH)) levl[x][y].wall_info |= W_HARDGROWTH;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_mountainfloode(x, y, poolcnt)
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

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = MOUNTAIN;
			blockorunblock_point(randomx,randomy);
			if (!(levl[randomx][randomy].wall_info & W_EASYGROWTH)) levl[randomx][randomy].wall_info |= W_HARDGROWTH;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = MOUNTAIN;
		blockorunblock_point(x,y);
		if (!(levl[x][y].wall_info & W_EASYGROWTH)) levl[x][y].wall_info |= W_HARDGROWTH;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_watertunnelfloode(x, y, poolcnt)
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

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = WATERTUNNEL;
			blockorunblock_point(randomx,randomy);
			if (!(levl[randomx][randomy].wall_info & W_EASYGROWTH)) levl[randomx][randomy].wall_info |= W_HARDGROWTH;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = WATERTUNNEL;
		blockorunblock_point(x,y);
		if (!(levl[x][y].wall_info & W_EASYGROWTH)) levl[x][y].wall_info |= W_HARDGROWTH;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_megafloodingd(x, y, poolcnt)
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
do_icefloodd(x, y, poolcnt)
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

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = ICE;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = ICE;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_crystalwaterfloode(x, y, poolcnt)
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

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = CRYSTALWATER;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = CRYSTALWATER;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_moorfloode(x, y, poolcnt)
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

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = MOORLAND;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = MOORLAND;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_urinefloode(x, y, poolcnt)
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

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = URINELAKE;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = URINELAKE;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_shiftingsandfloode(x, y, poolcnt)
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

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = SHIFTINGSAND;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = SHIFTINGSAND;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_styxfloode(x, y, poolcnt)
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

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = STYXRIVER;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = STYXRIVER;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_snowfloode(x, y, poolcnt)
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

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = SNOW;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = SNOW;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_ashfloode(x, y, poolcnt)
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

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = ASH;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = ASH;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_sandfloode(x, y, poolcnt)
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

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = SAND;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = SAND;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_pavementfloode(x, y, poolcnt)
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

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = PAVEDFLOOR;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = PAVEDFLOOR;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_highwayfloode(x, y, poolcnt)
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

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = HIGHWAY;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = HIGHWAY;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_grassfloode(x, y, poolcnt)
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

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = GRASSLAND;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = GRASSLAND;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_nethermistfloode(x, y, poolcnt)
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

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = NETHERMIST;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = NETHERMIST;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_stalactitefloode(x, y, poolcnt)
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

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = STALACTITE;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = STALACTITE;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_cryptfloode(x, y, poolcnt)
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

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = CRYPTFLOOR;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = CRYPTFLOOR;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_bubblefloode(x, y, poolcnt)
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

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = BUBBLES;
			blockorunblock_point(randomx,randomy);
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = BUBBLES;
		blockorunblock_point(x,y);
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_raincloudfloode(x, y, poolcnt)
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

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = RAINCLOUD;
			blockorunblock_point(randomx,randomy);
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = RAINCLOUD;
		blockorunblock_point(x,y);
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_cloudfloodd(x, y, poolcnt)
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

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = CLOUD;
			blockorunblock_point(randomx,randomy);
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = CLOUD;
		blockorunblock_point(x,y);
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_terrainfloodd(x, y, poolcnt)
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

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = randomwalltype();
			blockorunblock_point(randomx,randomy);
			if (!(levl[randomx][randomy].wall_info & W_EASYGROWTH)) levl[randomx][randomy].wall_info |= W_HARDGROWTH;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = randomwalltype();
		blockorunblock_point(x,y);
		if (!(levl[x][y].wall_info & W_EASYGROWTH)) levl[x][y].wall_info |= W_HARDGROWTH;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_barfloodd(x, y, poolcnt)
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

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = IRONBARS;
			blockorunblock_point(randomx,randomy);
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = IRONBARS;
		blockorunblock_point(x,y);
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

/* called when you're hit by fire (dofiretrap,buzz,zapyourself,explode) */
boolean			/* returns TRUE if hit on torso */
burnarmor(victim)
struct monst *victim;
{
    struct obj *item;
    char buf[BUFSZ];
    int mat_idx;

	if (victim == &youmonst && steelbreak()) return 0;
	if (victim == &youmonst && FireImmunity) return 0;
	if (victim == &youmonst && uarm && uarm->otyp == SATANIC_ARMOR) return 0;
	if (victim == &youmonst && uarm && uarm->oartifact == ART_DARK_L) return 0;

	if ((victim == &youmonst) && !rn2(2) ) make_burned(HBurned + rnd(20 + (monster_difficulty() * 5) ),TRUE);
    
    if (!victim) return 0;
#define burn_dmg(obj,descr) rust_dmg(obj, descr, 0, FALSE, victim)
    while (1) {
	switch (rn2(5)) {
	case 0:
	    item = (victim == &youmonst) ? uarmh : which_armor(victim, W_ARMH);
	    if (item) {
		mat_idx = objects[item->otyp].oc_material;
	    	sprintf(buf,"%s helmet", materialnm[mat_idx] );
	    }
	    if (!burn_dmg(item, item ? buf : "helmet")) continue;
	    break;
	case 1:
	    item = (victim == &youmonst) ? uarmc : which_armor(victim, W_ARMC);
	    if (item) {
		(void) burn_dmg(item, cloak_simple_name(item));
		return TRUE;
	    }
	    item = (victim == &youmonst) ? uarm : which_armor(victim, W_ARM);
	    if (item) {
		(void) burn_dmg(item, xname(item));
		return TRUE;
	    }
	    item = (victim == &youmonst) ? uarmu : which_armor(victim, W_ARMU);
	    if (item)
		(void) burn_dmg(item, "shirt");
	    return TRUE;
	case 2:
	    item = (victim == &youmonst) ? uarms : which_armor(victim, W_ARMS);
	    if (!burn_dmg(item, "shield")) continue;
	    break;
	case 3:
	    item = (victim == &youmonst) ? uarmg : which_armor(victim, W_ARMG);
	    if (!burn_dmg(item, "gloves")) continue;
	    break;
	case 4:
	    item = (victim == &youmonst) ? uarmf : which_armor(victim, W_ARMF);
	    if (!burn_dmg(item, "boots")) continue;
	    break;
	}
	break; /* Out of while loop */
    }
    return FALSE;
#undef burn_dmg
}

STATIC_PTR void
do_lavafloode(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;

	if (In_sokoban(&u.uz) && rn2(5)) return;

	if (/*nexttodoor(x, y) || */(rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = LAVAPOOL;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_floode(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;

	if (In_sokoban(&u.uz) && rn2(5)) return;

	if (/*nexttodoor(x, y) || */(rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = POOL;
		del_engr_at(x, y);
		water_damage(level.objects[x][y], FALSE, TRUE);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

/* Generic rust-armor function.  Returns TRUE if a message was printed;
 * "print", if set, means to print a message (and thus to return TRUE) even
 * if the item could not be rusted; otherwise a message is printed and TRUE is
 * returned only for rustable items.
 */
boolean
rust_dmg(otmp, ostr, type, print, victim)
register struct obj *otmp;
register const char *ostr;
int type;
boolean print;
struct monst *victim;
{
	boolean youdefend = (victim == &youmonst);
	boolean mondefend = !youdefend && victim;
	static NEARDATA const char * const action[] = { "smoulder", "rust", "rot", "corrode" };
	static NEARDATA const char * const msg[] =  { "burnt", "rusted", "rotten", "corroded" };
	boolean vulnerable = FALSE;
	boolean is_primary = TRUE;
	boolean vismon = (victim != &youmonst) && canseemon(victim);
	int erosion;

	if (!otmp) return(FALSE);
	if (stack_too_big(otmp)) return (FALSE);

	if (youdefend && AcidImmunity && type == 3) return(FALSE);

	switch(type) {
		case 0: vulnerable = is_flammable(otmp);

			/* Amy edit: leather and especially wood are hard to burn. They do burn of course, but not as good
			 * as cloth or paper, and therefore get a saving throw versus burning */
			if (objects[otmp->otyp].oc_material == MT_WOOD && rn2(4)) vulnerable = FALSE;
			if (objects[otmp->otyp].oc_material == MT_LEATHER && rn2(2)) vulnerable = FALSE;
			if (objects[otmp->otyp].oc_material == MT_ALKALINE && rn2(2)) vulnerable = FALSE;
			break;
		case 1: vulnerable = is_rustprone(otmp);
			if (objects[otmp->otyp].oc_material == MT_COBALT && rn2(2)) vulnerable = FALSE;
			if (objects[otmp->otyp].oc_material == MT_BRONZE && rn2(2)) vulnerable = FALSE;
			break;
		case 2: vulnerable = is_rottable(otmp);
			if (objects[otmp->otyp].oc_material == MT_ALKALINE && rn2(2)) vulnerable = FALSE;
			is_primary = FALSE;
			break;
		case 3: vulnerable = is_corrodeable(otmp);
			if (objects[otmp->otyp].oc_material == MT_GREEN_STEEL && rn2(2)) vulnerable = FALSE;
			is_primary = FALSE;
			break;
	}
burnagain:

	erosion = is_primary ? otmp->oeroded : otmp->oeroded2;

	if (!print && (!vulnerable || otmp->oerodeproof /* || erosion == MAX_ERODE*/ ))
		return FALSE;

	if (itemhasappearance(otmp, APP_BRAND_NEW_GLOVES) && rn2(4) ) vulnerable = FALSE;

	if (itemhasappearance(otmp, APP_IMAGINARY_HEELS) ) vulnerable = FALSE;

	if (otmp->oartifact == ART_SARTIRO ) vulnerable = FALSE;
	if (otmp->oartifact == ART_CUMBERSOME_DESC ) vulnerable = FALSE;
	if (otmp->oartifact == ART_GOODRES_ELVEN ) vulnerable = FALSE;
	if (otmp->oartifact == ART_PARTICULARLY_SOLID_SKULL ) vulnerable = FALSE;

	if (otmp->oartifact == ART_EXTENDED_DURABILITY && rn2(4)) vulnerable = FALSE;

	if (uarm && uarm->oartifact == ART_CANNOT_BE_HARMED_BLA_BLA && (otmp->owornmask & W_ARMOR) ) vulnerable = FALSE;

	if (uwep && uwep->oartifact == ART_SLAM_ && rn2(10) && (otmp->owornmask & W_ARMOR) ) vulnerable = FALSE;

	if (otmp->oartifact == ART_PROOFINGNESS_POOFS && !otmp->rknown) vulnerable = FALSE;

	if (otmp->oartifact == ART_RATCH_CLOSURE_SCRATCHING && rn2(4) ) vulnerable = FALSE;

	if (itemhasappearance(otmp, APP_WITHERED_CLOAK) ) vulnerable = FALSE;

	if (Race_if(PM_CHIQUAI) && rn2(4)) vulnerable = FALSE;

	if (uarmf && rn2(2) && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) vulnerable = FALSE;

	if (!vulnerable) {
	    if (flags.verbose) {
		if (victim == &youmonst)
		    Your("%s %s not affected.", ostr, vtense(ostr, "are"));
		else if (vismon)
		    pline("%s's %s %s not affected.", Monnam(victim), ostr,
			  vtense(ostr, "are"));
	    }
	} else if (erosion < MAX_ERODE) {
	    if (otmp->greased && (!issoviet || !rn2(2)) ) {
		grease_protect(otmp,ostr,victim);
	    } else if (otmp->oerodeproof || (otmp->oartifact && rn2(4)) || (otmp->blessed && !rnl(4))) {
		if (flags.verbose) {
		    if (victim == &youmonst)
			pline("Somehow, your %s %s not affected.",
			      ostr, vtense(ostr, "are"));
		    else if (vismon)
			pline("Somehow, %s's %s %s not affected.",
			      mon_nam(victim), ostr, vtense(ostr, "are"));
		}
	    } else {
		if (victim == &youmonst) {
		    Your("%s %s%s!", ostr,
			 vtense(ostr, action[type]),
			 erosion+1 == MAX_ERODE ? " completely" :
			    erosion ? " further" : "");
			if (issoviet && otmp->greased) pline("Sovetskiy khochet vash detal' byt' povrezhden, nesmotrya na smazku, potomu chto on takoy mudak!");
		} else if (vismon)
		    pline("%s's %s %s%s!", Monnam(victim), ostr,
			vtense(ostr, action[type]),
			erosion+1 == MAX_ERODE ? " completely" :
			  erosion ? " further" : "");
		if (is_primary)
		    otmp->oeroded++;
		else
		    otmp->oeroded2++;
		update_inventory();
	    }

		/* Amy edit: paper is particularly flammable and should therefore burn even faster */
	    if (otmp && objects[otmp->otyp].oc_material == MT_PAPER && type == 0 && !rn2(2)) goto burnagain;

	} else if (!hard_to_destruct(otmp) && !(otmp->oartifact == ART_EXCITING_SPFLOTCH) && (!otmp->oartifact || !rn2(4))) {
		    if (youdefend) {
		    Your("%s got vaporized!", ostr);
			remove_worn_item(otmp, FALSE);
			if (otmp == uball) unpunish();
			useupall(otmp);
			update_inventory();
		    } else if (mondefend) {
			long unwornmask;
			if ((unwornmask = otmp->owornmask) != 0L) {
			    victim->misc_worn_check &= ~unwornmask;
			    if (otmp->owornmask & W_WEP)
				setmnotwielded(victim,otmp);
			    otmp->owornmask = 0L;
			    update_mon_intrinsics(victim, otmp, FALSE, FALSE);
			}
			if (unwornmask & W_WEP)		/* wielded weapon is broken */
			    possibly_unwield(victim, FALSE);
			else if (unwornmask & W_ARMG)	/* worn gloves are broken */
			    mselftouch(victim, (const char *)0, TRUE);
			m_useup(victim, otmp);
		    }
		/*	    if (flags.verbose) {
		if (victim == &youmonst)
		    Your("%s %s completely %s.", ostr,
			 vtense(ostr, Blind ? "feel" : "look"),
			 msg[type]);
		else if (vismon)
		    pline("%s's %s %s completely %s.",
			  Monnam(victim), ostr,
			  vtense(ostr, "look"), msg[type]);
	    } */
	}
	return(TRUE);
}

/* Withering effects will erode objects even if they are erodeproof or of a non-vulnerable type. --Amy */
boolean
wither_dmg(otmp, ostr, type, print, victim)
register struct obj *otmp;
register const char *ostr;
int type;
boolean print;
struct monst *victim;
{
	boolean youdefend = (victim == &youmonst);
	boolean mondefend = !youdefend && victim;
	static NEARDATA const char * const action[] = { "smoulder", "rust", "rot", "corrode" };
	static NEARDATA const char * const msg[] =  { "burnt", "rusted", "rotten", "corroded" };
	boolean vulnerable = TRUE;
	boolean is_primary = TRUE;
	boolean vismon = (victim != &youmonst) && canseemon(victim);
	int erosion;

	if (!otmp) return(FALSE);
	if (stack_too_big(otmp)) return (FALSE);
	switch(type) {
		case 0: vulnerable = TRUE;
			break;
		case 1: vulnerable = TRUE;
			break;
		case 2: vulnerable = TRUE;
			is_primary = FALSE;
			break;
		case 3: vulnerable = TRUE;
			is_primary = FALSE;
			break;
	}
	erosion = is_primary ? otmp->oeroded : otmp->oeroded2;

	if (itemhasappearance(otmp, APP_BRAND_NEW_GLOVES) && rn2(4) ) vulnerable = FALSE;

	if (itemhasappearance(otmp, APP_IMAGINARY_HEELS) ) vulnerable = FALSE;

	if (otmp->oartifact == ART_SARTIRO ) vulnerable = FALSE;
	if (otmp->oartifact == ART_CUMBERSOME_DESC ) vulnerable = FALSE;
	if (otmp->oartifact == ART_GOODRES_ELVEN ) vulnerable = FALSE;
	if (otmp->oartifact == ART_PARTICULARLY_SOLID_SKULL ) vulnerable = FALSE;

	if (otmp->oartifact == ART_EXTENDED_DURABILITY && rn2(4)) vulnerable = FALSE;

	if (uarm && uarm->oartifact == ART_CANNOT_BE_HARMED_BLA_BLA && (otmp->owornmask & W_ARMOR) ) vulnerable = FALSE;

	if (uwep && uwep->oartifact == ART_SLAM_ && rn2(10) && (otmp->owornmask & W_ARMOR) ) vulnerable = FALSE;

	if (otmp->oartifact == ART_PROOFINGNESS_POOFS && !otmp->rknown) vulnerable = FALSE;

	if (otmp->oartifact == ART_RATCH_CLOSURE_SCRATCHING && rn2(4) ) vulnerable = FALSE;

	if (itemhasappearance(otmp, APP_WITHERED_CLOAK) ) vulnerable = FALSE;

	if (Race_if(PM_CHIQUAI) && rn2(4)) vulnerable = FALSE;

	if (is_unwitherable(otmp)) vulnerable = FALSE;

	if (uarmf && rn2(2) && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) vulnerable = FALSE;

	if (!print && (!vulnerable /* || erosion == MAX_ERODE*/ ))
		return FALSE;

	if (!vulnerable) {
	    if (flags.verbose) {
		if (victim == &youmonst)
		    Your("%s %s not affected.", ostr, vtense(ostr, "are"));
		else if (vismon)
		    pline("%s's %s %s not affected.", Monnam(victim), ostr,
			  vtense(ostr, "are"));
	    }
	} else if (erosion < MAX_ERODE) {
	    if (otmp->greased && (!issoviet || !rn2(2)) ) {
		grease_protect(otmp,ostr,victim);
	    } else if ((otmp->oartifact && rn2(4)) || (otmp->blessed && !rnl(4))) {
		if (flags.verbose) {
		    if (victim == &youmonst)
			pline("Somehow, your %s %s not affected.",
			      ostr, vtense(ostr, "are"));
		    else if (vismon)
			pline("Somehow, %s's %s %s not affected.",
			      mon_nam(victim), ostr, vtense(ostr, "are"));
		}
	    } else {
		if (victim == &youmonst) {
		    Your("%s %s%s!", ostr,
			 vtense(ostr, action[type]),
			 erosion+1 == MAX_ERODE ? " completely" :
			    erosion ? " further" : "");
			if (issoviet && otmp->greased) pline("Sovetskiy khochet vash detal' byt' povrezhden, nesmotrya na smazku, potomu chto on takoy mudak!");
		} else if (vismon)
		    pline("%s's %s %s%s!", Monnam(victim), ostr,
			vtense(ostr, action[type]),
			erosion+1 == MAX_ERODE ? " completely" :
			  erosion ? " further" : "");
		if (is_primary)
		    otmp->oeroded++;
		else
		    otmp->oeroded2++;
		update_inventory();
	    }
	} else /*if (!otmp->oartifact)*/ {
		    if (youdefend && !hard_to_destruct(otmp) && !(otmp->oartifact == ART_EXCITING_SPFLOTCH) && (!otmp->oartifact || !rn2(4)) ) {
		    Your("%s got vaporized!", ostr);
			remove_worn_item(otmp, FALSE);
			if (otmp == uball) unpunish();
			useupall(otmp);
			update_inventory();
		    } else if (mondefend) {
			long unwornmask;
			if ((unwornmask = otmp->owornmask) != 0L) {
			    victim->misc_worn_check &= ~unwornmask;
			    if (otmp->owornmask & W_WEP)
				setmnotwielded(victim,otmp);
			    otmp->owornmask = 0L;
			    update_mon_intrinsics(victim, otmp, FALSE, FALSE);
			}
			if (unwornmask & W_WEP)		/* wielded weapon is broken */
			    possibly_unwield(victim, FALSE);
			else if (unwornmask & W_ARMG)	/* worn gloves are broken */
			    mselftouch(victim, (const char *)0, TRUE);
			m_useup(victim, otmp);
		    }
		/*	    if (flags.verbose) {
		if (victim == &youmonst)
		    Your("%s %s completely %s.", ostr,
			 vtense(ostr, Blind ? "feel" : "look"),
			 msg[type]);
		else if (vismon)
		    pline("%s's %s %s completely %s.",
			  Monnam(victim), ostr,
			  vtense(ostr, "look"), msg[type]);
	    } */
	}
	return(TRUE);
}

void
grease_protect(otmp,ostr,victim)
register struct obj *otmp;
register const char *ostr;
struct monst *victim;
{
	static const char txt[] = "protected by the layer of grease!";
	boolean vismon = victim && (victim != &youmonst) && canseemon(victim);

	if (ostr) {
	    if (victim == &youmonst)
		Your("%s %s %s", ostr, vtense(ostr, "are"), txt);
	    else if (vismon)
		pline("%s's %s %s %s", Monnam(victim),
		    ostr, vtense(ostr, "are"), txt);
	} else {
	    if (victim == &youmonst)
		Your("%s %s",aobjnam(otmp,"are"), txt);
	    else if (vismon)
		pline("%s's %s %s", Monnam(victim), aobjnam(otmp,"are"), txt);
	}
	if ((!rn2(2) || (isfriday && !rn2(2)) ) && !stack_too_big(otmp) ) {
	    otmp->greased -= 1;
	    if (carried(otmp)) {
		pline_The("grease dissolves.");
		update_inventory();
	    }
	}
}

void
makerandomtrap_at(x,y,givehp)
register int x, y;
boolean givehp;
{

	int rtrap;
	rtrap = rndtrap();
	maketrap(x,y,rtrap,100,givehp);
	return;

}

struct trap *
maketrap(x,y,typ,replacechance,givehp)
register int x, y, typ, replacechance;
boolean givehp;
{
	register struct trap *ttmp;
	register struct rm *lev;
	register boolean oldplace;

	if (typ != MAGIC_PORTAL && (rn2(100) < replacechance) && (rnd(u.freqtrapbonus + 1000) > 1000)) {
		typ = u.frequenttrap;
		if (typ == MAGIC_PORTAL) typ = ROCKTRAP;
		if (typ == S_PRESSING_TRAP) typ = ROCKTRAP;
		if (typ == WISHING_TRAP) typ = BLINDNESS_TRAP;
		if (In_sokoban(&u.uz) && rn2(10) && (typ == HOLE || typ == TRAPDOOR || typ == SHAFT_TRAP || typ == CURRENT_SHAFT || typ == PIT || typ == SPIKED_PIT || typ == GIANT_CHASM || typ == SHIT_PIT || typ == MANA_PIT || typ == ANOXIC_PIT || typ == HYPOXIC_PIT || typ == ACID_PIT)) typ = ROCKTRAP;
		if (In_sokoban(&u.uz) && rn2(100) && typ == NUPESELL_TRAP) typ = FIRE_TRAP;
		if (typ == ELDER_TENTACLING_TRAP) typ = FIRE_TRAP;
		if (typ == DATA_DELETE_TRAP) typ = RUST_TRAP;
		if (typ == ARTIFACT_JACKPOT_TRAP) typ = MAGIC_TRAP;
		if (typ == GOOD_ARTIFACT_TRAP) typ = WEB;
		if (typ == BOON_TRAP) typ = MAGIC_BEAM_TRAP;
		if (typ == LEVEL_TELEP && (level.flags.noteleport || Race_if(PM_STABILISATOR) || Is_knox(&u.uz) || Is_blackmarket(&u.uz) || Is_aligned_quest(&u.uz) || In_endgame(&u.uz) || In_sokoban(&u.uz) ) ) typ = ANTI_MAGIC;
		if (typ == LEVEL_BEAMER && (level.flags.noteleport || Race_if(PM_STABILISATOR) || Is_knox(&u.uz) || Is_blackmarket(&u.uz) || Is_aligned_quest(&u.uz) || In_endgame(&u.uz) || In_sokoban(&u.uz) ) ) typ = ANTI_MAGIC;
		if (typ == BRANCH_TELEPORTER && (level.flags.noteleport || Race_if(PM_STABILISATOR) || Is_knox(&u.uz) || Is_blackmarket(&u.uz) || Is_aligned_quest(&u.uz) || In_endgame(&u.uz) || In_sokoban(&u.uz) ) ) typ = ANTI_MAGIC;
		if (typ == BRANCH_BEAMER && (level.flags.noteleport || Race_if(PM_STABILISATOR) || Is_knox(&u.uz) || Is_blackmarket(&u.uz) || Is_aligned_quest(&u.uz) || In_endgame(&u.uz) || In_sokoban(&u.uz) ) ) typ = ANTI_MAGIC;
		if (typ == NEXUS_TRAP && (level.flags.noteleport || Race_if(PM_STABILISATOR) || Is_knox(&u.uz) || Is_blackmarket(&u.uz) || Is_aligned_quest(&u.uz) || In_endgame(&u.uz) || In_sokoban(&u.uz) ) ) typ = ANTI_MAGIC;
		if (typ == TELEP_TRAP && (level.flags.noteleport || Race_if(PM_STABILISATOR)) ) typ = SQKY_BOARD;
		if (typ == PHASEPORTER && (level.flags.noteleport || Race_if(PM_STABILISATOR)) ) typ = SQKY_BOARD;
		if (typ == PHASE_BEAMER && (level.flags.noteleport || Race_if(PM_STABILISATOR)) ) typ = SQKY_BOARD;
		if (typ == BEAMER_TRAP && (level.flags.noteleport || Race_if(PM_STABILISATOR)) ) typ = SQKY_BOARD;
		if ((typ == TRAPDOOR || typ == HOLE || typ == SHAFT_TRAP || typ == CURRENT_SHAFT) && !Can_fall_thru(&u.uz) && !Is_stronghold(&u.uz) ) typ = ROCKTRAP;
		if (typ == ACTIVE_SUPERSCROLLER_TRAP) typ = SUPERSCROLLER_TRAP;
		
	} else if (typ != MAGIC_PORTAL && (rn2(100) < replacechance) && !rn2(u.traprandomizing)) {
		typ = rnd(TRAPNUM-1);
		if (In_sokoban(&u.uz) && rn2(10) && (typ == HOLE || typ == TRAPDOOR || typ == SHAFT_TRAP || typ == CURRENT_SHAFT || typ == PIT || typ == SPIKED_PIT || typ == GIANT_CHASM || typ == SHIT_PIT || typ == MANA_PIT || typ == ANOXIC_PIT || typ == HYPOXIC_PIT || typ == ACID_PIT)) typ = ROCKTRAP;
		if (In_sokoban(&u.uz) && rn2(100) && typ == NUPESELL_TRAP) typ = FIRE_TRAP;
		while (typ == MAGIC_PORTAL || typ == ACTIVE_SUPERSCROLLER_TRAP || typ == WISHING_TRAP || typ == S_PRESSING_TRAP || typ == DATA_DELETE_TRAP || typ == ELDER_TENTACLING_TRAP || typ == ARTIFACT_JACKPOT_TRAP || typ == GOOD_ARTIFACT_TRAP || typ == BOON_TRAP) typ = rnd(TRAPNUM-1);
	} else if (typ != MAGIC_PORTAL && (rn2(100) < replacechance) && uarmh && uarmh->oartifact == ART_BOMB_BLOW && !rn2(100)) {
		typ = CATACLYSM_TRAP;
	}

	/* the caller doesn't care if we're on a level that cannot have holes, so we need an extra failsafe --Amy */
	if ((typ == TRAPDOOR || typ == HOLE || typ == SHAFT_TRAP || typ == CURRENT_SHAFT) && !Can_fall_thru(&u.uz) && !Is_stronghold(&u.uz) ) typ = ROCKTRAP;

	if ((ttmp = t_at(x,y)) != 0) {
	    if (ttmp->ttyp == MAGIC_PORTAL) return (struct trap *)0;
	    oldplace = TRUE;
	    if (u.utrap && (x == u.ux) && (y == u.uy) &&
	      ((u.utraptype == TT_BEARTRAP && typ != BEAR_TRAP && typ != LEG_TRAP) ||
	      (u.utraptype == TT_WEB && typ != WEB && typ != FARTING_WEB) ||
	      (u.utraptype == TT_PIT && typ != PIT && typ != SPIKED_PIT && typ != GIANT_CHASM && typ != SHIT_PIT && typ != MANA_PIT && typ != ANOXIC_PIT && typ != HYPOXIC_PIT && typ != ACID_PIT)))
		    u.utrap = 0;
	} else {
	    oldplace = FALSE;
	    ttmp = newtrap();
	    ttmp->tx = x;
	    ttmp->ty = y;
	    ttmp->launch.x = -1;	/* force error if used before set */
	    ttmp->launch.y = -1;
	}
	ttmp->ttyp = typ;

	if (ttmp && FemtrapActiveInge && !rn2(SuperFemtrapInge ? 25 : 100)) {
			struct permonst *pm = 0;
			int attempts = 0;

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

newbossING:
			do {
				pm = rndmonst();
				attempts++;
				if (attempts && (attempts % 10000 == 0)) u.mondiffhack++;
				if (!rn2(2000)) reset_rndmonst(NON_PM);

			} while ( (!pm || (pm && !(pm->msound == MS_FART_QUIET))) && attempts < 50000);

			if (!pm && rn2(50) ) {
				attempts = 0;
				goto newbossING;
			}
			if (pm && !(pm->msound == MS_FART_QUIET) && rn2(50) ) {
				attempts = 0;
				goto newbossING;
			}

			if (pm) (void) makemon(pm, ttmp->tx, ttmp->ty, MM_ANGRY|MM_ADJACENTOK);

			u.aggravation = 0;
			u.mondiffhack = 0;

	}

	switch(typ) {

	    case INSTAFEMINISM_TRAP:
		ttmp->ttyp = UNKNOWN_TRAP;
		if (!FemtrapActiveRuth) pline("Ha ha ha...");
		randomfeminismtrap(rnz( (level_difficulty() + 2) * rnd(50)));
		if (!FemtrapActiveRuth) pline("The woman is laughing loudly because you were unable to avoid that effect.");

		break;

	    case INSTANASTY_TRAP:
		{
			ttmp->ttyp = UNKNOWN_TRAP;
			/* we use that in win/tty/topl.c too --Amy */
			int nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
			if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
			int blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
			if (!blackngdur ) blackngdur = 500; /* fail safe */

			randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), blackngdur - (monster_difficulty() * 3));

		}
		break;

	    case WEB: /* always generate a spider unless directly beneath you --Amy */
		if (u.ux != x || u.uy != y) (void) makemon( mkclass(S_SPIDER,0), x, y, NO_MM_FLAGS);
		break;

	    case STATUE_TRAP:	    /* create a "living" statue */
	      { struct monst *mtmp;
		struct obj *otmp, *statue;
		int sessileattempts;
		int sessilemnum;

		statue = mkcorpstat(STATUE, (struct monst *)0,
					&mons[rndmonnum()], x, y, FALSE);
		mtmp = makemon(&mons[statue->corpsenm], 0, 0, NO_MM_FLAGS);
		if (!mtmp) break; /* should never happen */
		while(mtmp->minvent) {

		    otmp = mtmp->minvent;
		    otmp->owornmask = 0;
		    obj_extract_self(otmp);

		    /* in order to properly set the items to not always drop, a new flag is needed... --Amy */
		    (void) add_to_container(statue, otmp, FALSE);
		}
		statue->owt = weight(statue);
		if (!rn2(5)) statue->oinvis = 1; /* player needs see invisible to detect this thing --Amy */
		if (!rn2(500)) {statue->oinvis = 1; statue->oinvisreal = 1;} /* not detectable at all */

	/* "Stop Fungi/Objects from hiding statue traps. It's a bit of a balance issue and created clutter, and had a good chance of spawning out of depth monsters." In Soviet Russia, people want to be able to see that statue of a slimy vortex in advance. They don't like games being difficult, and they don't want any nasty surprises, so they make sure that statue traps are never obscured by anything. I wonder if the next thing they'll do is to remove all the other traps because you can't see them in advance, or even make sure all of them are visible before you trigger them, too? Completely missing the point of traps, but oh well... --Amy */

		else if (!rn2(10) && !issoviet) (void) mkobj_at(COIN_CLASS, x, y, TRUE, FALSE); /* hidden underneath a zorkmid */
		else if (!rn2(10) && !issoviet) (void) mksobj_at(BOULDER, x, y, TRUE, FALSE, FALSE); /* hidden underneath a boulder */
		else if (!rn2(10) && !issoviet) (void) mkobj_at(timebasedlowerchance() ? 0 : COIN_CLASS, x, y, TRUE, FALSE); /* hidden underneath a random item */
		else if (!rn2(12) && !issoviet) (void) makemon( mkclass(S_MIMIC,0), x, y, NO_MM_FLAGS); /* hidden beneath a mimic */
		else if (!rn2(7) && !issoviet) {
				for (sessileattempts = 0; sessileattempts < 20; sessileattempts++) {
					sessilemnum = pm_mkclass(S_FUNGUS, 0);
					if (sessilemnum != -1 && is_nonmoving(&mons[sessilemnum]) ) sessileattempts = 20;
					}
		if (sessilemnum != -1) (void) makemon( &mons[sessilemnum], x, y, NO_MM_FLAGS); /* hidden beneath a fungus */
		}
		else if (!rn2(10) && !issoviet) {
				for (sessileattempts = 0; sessileattempts < 10; sessileattempts++) {
					sessilemnum = pm_mkclass(S_JELLY, 0);
					if (sessilemnum != -1 && is_nonmoving(&mons[sessilemnum]) ) sessileattempts = 10;
					}
		if (sessilemnum != -1) (void) makemon( &mons[sessilemnum], x, y, NO_MM_FLAGS); /* hidden beneath a jelly */
		}
		else if (!rn2(15) && !issoviet) {
				for (sessileattempts = 0; sessileattempts < 100; sessileattempts++) {
					sessilemnum = rndmonnum();
					if (sessilemnum != -1 && is_nonmoving(&mons[sessilemnum]) ) sessileattempts = 100;
					}
		if (sessilemnum != -1) (void) makemon( &mons[sessilemnum], x, y, NO_MM_FLAGS); /* hidden beneath a monster */
		}

		mongone(mtmp);
		break;
	      }

	    case SATATUE_TRAP:	    /* inspired by ais523: more dangerous monster for the statue trap */
	      { struct monst *mtmp;
		struct obj *otmp, *statue;
		int sessileattempts;
		int sessilemnum;

		u.aggravation = 1;
		if (!rn2(2)) u.heavyaggravation = 1;
		reset_rndmonst(NON_PM);

		statue = mkcorpstat(STATUE, (struct monst *)0,
					&mons[rndmonnum()], x, y, FALSE);
		mtmp = makemon(&mons[statue->corpsenm], 0, 0, NO_MM_FLAGS);
		if (!mtmp) break; /* should never happen */
		while(mtmp->minvent) {
		    otmp = mtmp->minvent;
		    otmp->owornmask = 0;
		    obj_extract_self(otmp);
		    (void) add_to_container(statue, otmp, FALSE);
		}
		statue->owt = weight(statue);
		if (!rn2(5)) statue->oinvis = 1; /* player needs see invisible to detect this thing --Amy */
		if (!rn2(500)) {statue->oinvis = 1; statue->oinvisreal = 1;} /* not detectable at all */

	/* "Stop Fungi/Objects from hiding statue traps. It's a bit of a balance issue and created clutter, and had a good chance of spawning out of depth monsters." In Soviet Russia, people want to be able to see that statue of a slimy vortex in advance. They don't like games being difficult, and they don't want any nasty surprises, so they make sure that statue traps are never obscured by anything. I wonder if the next thing they'll do is to remove all the other traps because you can't see them in advance, or even make sure all of them are visible before you trigger them, too? Completely missing the point of traps, but oh well... --Amy */

		else if (!rn2(10) && !issoviet) (void) mkobj_at(COIN_CLASS, x, y, TRUE, FALSE); /* hidden underneath a zorkmid */
		else if (!rn2(10) && !issoviet) (void) mksobj_at(BOULDER, x, y, TRUE, FALSE, FALSE); /* hidden underneath a boulder */
		else if (!rn2(10) && !issoviet) (void) mkobj_at(timebasedlowerchance() ? 0 : COIN_CLASS, x, y, TRUE, FALSE); /* hidden underneath a random item */
		else if (!rn2(12) && !issoviet) (void) makemon( mkclass(S_MIMIC,0), x, y, NO_MM_FLAGS); /* hidden beneath a mimic */
		else if (!rn2(7) && !issoviet) {
				for (sessileattempts = 0; sessileattempts < 20; sessileattempts++) {
					sessilemnum = pm_mkclass(S_FUNGUS, 0);
					if (sessilemnum != -1 && is_nonmoving(&mons[sessilemnum]) ) sessileattempts = 20;
					}
		if (sessilemnum != -1) (void) makemon( &mons[sessilemnum], x, y, NO_MM_FLAGS); /* hidden beneath a fungus */
		}
		else if (!rn2(10) && !issoviet) {
				for (sessileattempts = 0; sessileattempts < 10; sessileattempts++) {
					sessilemnum = pm_mkclass(S_JELLY, 0);
					if (sessilemnum != -1 && is_nonmoving(&mons[sessilemnum]) ) sessileattempts = 10;
					}
		if (sessilemnum != -1) (void) makemon( &mons[sessilemnum], x, y, NO_MM_FLAGS); /* hidden beneath a jelly */
		}
		else if (!rn2(15) && !issoviet) {
				for (sessileattempts = 0; sessileattempts < 100; sessileattempts++) {
					sessilemnum = rndmonnum();
					if (sessilemnum != -1 && is_nonmoving(&mons[sessilemnum]) ) sessileattempts = 100;
					}
		if (sessilemnum != -1) (void) makemon( &mons[sessilemnum], x, y, NO_MM_FLAGS); /* hidden beneath a monster */
		}

		mongone(mtmp);

		u.aggravation = 0;
		u.heavyaggravation = 0;

		break;
	      }

	    case LANDMINE:
		ttmp->launch_otyp = 0;
		break;

	    case HEEL_TRAP:
	    case ATTACKING_HEEL_TRAP:
		{

		ttmp->launch_otyp = rnd(28);

		break;
	      }

	    case FART_TRAP:
	    case PERSISTENT_FART_TRAP:
	    case FARTING_WEB:
		{

		ttmp->launch_otyp = rn2(SIZE(farttrapnames));

		break;
	      }

	    case BOSS_SPAWNER:

		{

		ttmp->launch_otyp = rnd(20);

		break;
	      }

	    case S_PRESSING_TRAP:

		{
		int spressingstrength = 10;
		spressingstrength += techlevX(get_tech_no(T_S_PRESSING));
		if (!(PlayerCannotUseSkills)) {
			switch (P_SKILL(P_SQUEAKING)) {
				default: break;
				case P_BASIC: spressingstrength += 10; break;
				case P_SKILLED: spressingstrength += 20; break;
				case P_EXPERT: spressingstrength += 30; break;
				case P_MASTER: spressingstrength += 40; break;
				case P_GRAND_MASTER: spressingstrength += 50; break;
				case P_SUPREME_MASTER: spressingstrength += 60; break;
			}
		}
		if (spressingstrength > 125) spressingstrength = 125; /* fail safe */
		if (spressingstrength < 10) spressingstrength = 10;
		ttmp->launch_otyp = spressingstrength;

		}
		break;

	    case MONSTER_GENERATOR:
		{
			ttmp->launch.x = rnd(3);
			switch (ttmp->launch.x) {
				case 1:
					ttmp->launch_otyp = rnd(15); /* random color */
					break;
				case 2:
					ttmp->launch_otyp = rnd(186); /* monster glyph (weighted chance) */
					break;
				case 3:
					ttmp->launch_otyp = rnd(434); /* monstercolor function */
					break;
				default:
					impossible("monster generator with bad launch type %d!", ttmp->launch.x);
					break;
			}
		}
		break;

	    case MAGIC_BEAM_TRAP:
	    case PIERCING_BEAM_TRAP:
		{
		    int d,startdir = rn2(8);
		    int dist;
		    int lx, ly;
		    int ok = 0;
		    for (d = 0; ((d < 8) && !ok); d++)
			for (dist = 1; ((dist < 8) && !ok); dist++) {
			    lx = x;
			    ly = y;
			    switch ((startdir + d) % 8) {
			    case 0: lx += dist; break;
			    case 1: lx += dist; ly += dist; break;
			    case 2: ly += dist; break;
			    case 3: lx -= dist; ly += dist; break;
			    case 4: lx -= dist; break;
			    case 5: lx -= dist; ly -= dist; break;
			    case 6: ly -= dist; break;
			    case 7: lx += dist; ly -= dist; break;
			    }
			    if (isok(lx,ly) && IS_STWALL(levl[lx][ly].typ)) {
				ttmp->launch.x = lx;
				ttmp->launch.y = ly;
				/* no AD_DISN, thanks */
				/* edit by Amy - wimp! I'll allow this trap to zap death and disintegration rays. :P */

				switch (rnd(100)) {

				case 1: 
				case 2: 
				case 3: 
				case 4: 
				case 5: 
				ttmp->launch_otyp = -10-(AD_MAGM-1);
				break;
				case 6: 
				case 7: 
				case 8: 
				case 9: 
				case 10: 
				ttmp->launch_otyp = -20-(AD_MAGM-1);
				break;
				case 11: 
				case 12: 
				case 13: 
				case 14: 
				case 15: 
				ttmp->launch_otyp = -30-(AD_MAGM-1);
				break;
				case 16: 
				case 17: 
				case 18: 
				case 19: 
				case 20: 
				ttmp->launch_otyp = -40-(AD_MAGM-1);
				break;
				case 21: 
				case 22: 
				case 23: 
				case 24: 
				ttmp->launch_otyp = -10-(AD_SLEE-1);
				break;
				case 25: 
				case 26: 
				case 27: 
				case 28: 
				ttmp->launch_otyp = -20-(AD_SLEE-1);
				break;
				case 29: 
				case 30: 
				case 31: 
				case 32: 
				ttmp->launch_otyp = -30-(AD_SLEE-1);
				break;
				case 33: 
				case 34: 
				case 35: 
				case 36: 
				ttmp->launch_otyp = -40-(AD_SLEE-1);
				break;
				case 37: 
				case 38: 
				case 39: 
				ttmp->launch_otyp = -10-(AD_ELEC-1);
				break;
				case 40: 
				case 41: 
				case 42: 
				ttmp->launch_otyp = -20-(AD_ELEC-1);
				break;
				case 43: 
				case 44: 
				case 45: 
				ttmp->launch_otyp = -30-(AD_ELEC-1);
				break;
				case 46: 
				case 47: 
				case 48: 
				ttmp->launch_otyp = -40-(AD_ELEC-1);
				break;
				case 49: 
				case 50: 
				case 51: 
				ttmp->launch_otyp = -10-(AD_FIRE-1);
				break;
				case 52: 
				case 53: 
				case 54: 
				ttmp->launch_otyp = -20-(AD_FIRE-1);
				break;
				case 55: 
				case 56: 
				case 57: 
				ttmp->launch_otyp = -30-(AD_FIRE-1);
				break;
				case 58: 
				case 59: 
				case 60: 
				ttmp->launch_otyp = -40-(AD_FIRE-1);
				break;
				case 61: 
				case 62: 
				case 63: 
				ttmp->launch_otyp = -10-(AD_COLD-1);
				break;
				case 64: 
				case 65: 
				case 66: 
				ttmp->launch_otyp = -20-(AD_COLD-1);
				break;
				case 67: 
				case 68: 
				case 69: 
				ttmp->launch_otyp = -30-(AD_COLD-1);
				break;
				case 70: 
				case 71: 
				case 72: 
				ttmp->launch_otyp = -40-(AD_COLD-1);
				break;
				case 73: 
				case 74: 
				ttmp->launch_otyp = -10-(AD_DRST-1);
				break;
				case 75: 
				case 76: 
				ttmp->launch_otyp = -20-(AD_DRST-1);
				break;
				case 77: 
				case 78: 
				ttmp->launch_otyp = -30-(AD_DRST-1);
				break;
				case 79: 
				case 80: 
				ttmp->launch_otyp = -40-(AD_DRST-1);
				break;
				case 81: 
				case 82: 
				ttmp->launch_otyp = -10-(AD_ACID-1);
				break;
				case 83: 
				case 84: 
				ttmp->launch_otyp = -20-(AD_ACID-1);
				break;
				case 85: 
				case 86: 
				ttmp->launch_otyp = -30-(AD_ACID-1);
				break;
				case 87: 
				case 88: 
				ttmp->launch_otyp = -40-(AD_ACID-1);
				break;
				case 89: 
				ttmp->launch_otyp = -10-(AD_LITE-1);
				break;
				case 90: 
				ttmp->launch_otyp = -20-(AD_LITE-1);
				break;
				case 91:
				ttmp->launch_otyp = -30-(AD_LITE-1);
				break;
				case 92: 
				ttmp->launch_otyp = -40-(AD_LITE-1);
				break;
				case 93: 
				ttmp->launch_otyp = -10-(AD_DISN-1);
				break;
				case 94: 
				ttmp->launch_otyp = -20-(AD_DISN-1);
				break;
				case 95:
				ttmp->launch_otyp = -30-(AD_DISN-1);
				break;
				case 96: 
				ttmp->launch_otyp = -40-(AD_DISN-1);
				break;

				case 97: 
				ttmp->launch_otyp = -10-(AD_SPC2-1);
				break;
				case 98: 
				ttmp->launch_otyp = -20-(AD_SPC2-1);
				break;
				case 99:
				ttmp->launch_otyp = -30-(AD_SPC2-1);
				break;
				case 100: 
				ttmp->launch_otyp = -40-(AD_SPC2-1);
				break;

				default:
				ttmp->launch_otyp = -10-(AD_MAGM-1);
				break;
				}

				ok = 1;
			    }
			}
		}
		break;

	    case ROLLING_BOULDER_TRAP:	/* boulder will roll towards trigger */
		(void) mkroll_launch(ttmp, x, y, BOULDER, 1L);
		break;
	    case HOLE:
	    case PIT:
	    case SPIKED_PIT:
	    case GIANT_CHASM:
	    case SHIT_PIT:
	    case MANA_PIT:
	    case ANOXIC_PIT:
	    case HYPOXIC_PIT:
	    case ACID_PIT:
	    case SHAFT_TRAP:
	    case CURRENT_SHAFT:
	    case TRAPDOOR:

		/* Amy: transforms terrain into a regular walkable tile upon creation. We have to ensure several things:
		 * - furniture that is mandatory for winning the game (high altars, are there any others?) is protected
		 * - undiggable walls may not be turned into walkable floor this way
		 * I don't get why these traps can't just be on other types of terrain anyway... */

		lev = &levl[x][y];
		if (*in_rooms(x, y, SHOPBASE) &&
			((typ == HOLE || typ == TRAPDOOR || typ == SHAFT_TRAP || typ == CURRENT_SHAFT) ||
			 IS_DOOR(lev->typ) || IS_WALL(lev->typ)))
		    add_damage(x, y,		/* schedule repair */
			       ((IS_DOOR(lev->typ) || IS_WALL(lev->typ))
				&& !flags.mon_moving) ? 200L : 0L);
		lev->doormask = 0;	/* subsumes altarmask, icedpool... */
		if (IS_ROOM(lev->typ) && lev->typ != STAIRS && lev->typ != LADDER && !(lev->typ == ALTAR && (Is_astralevel(&u.uz) || Is_sanctum(&u.uz)) ) ) /* && !IS_AIR(lev->typ) */
		    lev->typ = ROOM;

		/*
		 * some cases which can happen when digging
		 * down while phazing thru solid areas
		 */
		else if ((lev->typ == STONE || lev->typ == SCORR) && (!(lev->wall_info & W_NONDIGGABLE) || u.dynamitehack) ) {
		    lev->typ = CORR;
			if (u.dynamitehack) lev->wall_info &= ~W_NONDIGGABLE;
		}
		else if ((IS_WALL(lev->typ) || lev->typ == SDOOR) && (!(lev->wall_info & W_NONDIGGABLE) || u.dynamitehack) ) {
		    lev->typ = level.flags.is_maze_lev ? ROOM :
			       level.flags.is_cavernous_lev ? CORR : DOOR;
			if (u.dynamitehack) lev->wall_info &= ~W_NONDIGGABLE;
		}

		unearth_objs(x, y);
		blockorunblock_point(x,y); /* might have transformed wall into floor */
		break;
	}
	ttmp->hiddentrap = 0;
	if (!rn2(u.invistrapchance)) ttmp->hiddentrap = 1;
	if (u.uprops[INVIS_TRAPS_EFFECT].extrinsic || InvisibleTrapsEffect || have_invisostone() ) ttmp->hiddentrap = 1;

	if (ttmp->ttyp == u.invisotrap) ttmp->hiddentrap = 1;
	if (ttmp->ttyp == INVISIBLE_TRAP) ttmp->hiddentrap = 1;
	if (ttmp->ttyp == NOWNSIBLE_TRAP) ttmp->hiddentrap = 1;
	if (ttmp->ttyp == KOP_CUBE) ttmp->hiddentrap = 1;
	if (ttmp->ttyp == BOSS_SPAWNER) ttmp->hiddentrap = 1;

	/* can the trap give max HP when untrapped? make sure it's not farmable --Amy */
	ttmp->giveshp = 0;
	if (givehp && !rn2(3)) ttmp->giveshp = 1;

	if (ttmp->ttyp == HOLE && !In_sokoban(&u.uz) && !ttmp->hiddentrap ) ttmp->tseen = 1;  /* You can't hide a hole */
	else if (ttmp->ttyp == SUPERTHING_TRAP && !ttmp->hiddentrap ) ttmp->tseen = 1;
	else if (ttmp->ttyp == ARABELLA_SPEAKER && !ttmp->hiddentrap ) ttmp->tseen = 1;
	else if (ttmp->ttyp == WRONG_STAIRS && !ttmp->hiddentrap ) ttmp->tseen = 1;
	else ttmp->tseen = 0;
	ttmp->once = 0;
	ttmp->artionce = 0;
	ttmp->tdetected = 0;
	ttmp->trapdiff = 0; /* difficulty: higher values make it less likely for you to find it */
	if (ttmp->ttyp != MAGIC_PORTAL) {
		ttmp->trapdiff = level_difficulty();
		if (u.trapxtradiff) ttmp->trapdiff += rnd(u.trapxtradiff);
		if (u.xdifftrapchance > rn2(100)) {
			if (rn2(10)) ttmp->trapdiff += rnd(100);
			else if (rn2(5)) ttmp->trapdiff += rnd(200);
			else ttmp->trapdiff += rnd(500);
		}
		/* we used to have a fail safe for this difficulty level, but I decided we don't need it :P
		 * after all, most traps will reveal themselves when triggered, and if some nasty ones don't, oh well! --Amy */
	}
	ttmp->madeby_u = 0;
	ttmp->dst.dnum = -1;
	ttmp->dst.dlevel = -1;
	if (!oldplace) {
	    ttmp->ntrap = ftrap;
	    ftrap = ttmp;
	}
	return(ttmp);
}

void
fall_through(td)
boolean td;	/* td == TRUE : trap door or hole */
{
	d_level dtmp;
	char msgbuf[BUFSZ];
	const char *dont_fall = 0;
	register int newlevel = dunlev(&u.uz);

	/* KMH -- You can't escape the Sokoban level traps */
	if(Blind && Levitation && !(SoiltypeEffect || u.uprops[SOILTYPE].extrinsic || have_soiltypestone() || (uarmf && uarmf->oartifact == ART_ARABELLA_S_GIRL_KICK)) && !In_sokoban(&u.uz)) return;

	do {
	    newlevel++;
	} while(!rn2(4) && newlevel < dunlevs_in_dungeon(&u.uz));

	if (wizard && !Can_fall_thru(&u.uz) && (yn("Go down here?") == 'y')) newlevel++;

	if(td) {
	    struct trap *t=t_at(u.ux,u.uy);
	    seetrap(t);
	    if (!In_sokoban(&u.uz)) {
		if (t->ttyp == TRAPDOOR)
			pline("A trap door opens up under you!");
		else if (t->ttyp == SHAFT_TRAP)
			pline("A shaft opens up under you!");
		else if (t->ttyp == CURRENT_SHAFT)
			pline("Air currents pull you into a shaft!");
		else 
			pline("There's a gaping hole under you!");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Dobro pozhalovat' v kakoy-to gorazdo boleye opasnyy uroven'! Udachi vam, vy, veroyatno, ne budet zdes' vyzhit' v lyubom sluchaye." : "Schlueing!");
	    }
	} else pline_The("%s opens up under you!", surface(u.ux,u.uy));

	if (In_sokoban(&u.uz) && Can_fall_thru(&u.uz))
	    ;	/* KMH -- You can't escape the Sokoban level traps */
	else if((Levitation || u.ustuck || (!Can_fall_thru(&u.uz) && !wizard)
	   || Flying || is_clinger(youmonst.data)
	   || (Role_if(PM_ARCHEOLOGIST) && uwep && uwep->otyp >= BULLWHIP && uwep->otyp <= SECRET_WHIP && uwep->otyp != RUBBER_HOSE)
	   || (In_gehennom(&u.uz) && !u.uevent.invoked && newlevel == dunlevs_in_dungeon(&u.uz)))
		&& !(SoiltypeEffect || u.uprops[SOILTYPE].extrinsic || have_soiltypestone() || (uarmf && uarmf->oartifact == ART_ARABELLA_S_GIRL_KICK)) ) {
		if (Role_if(PM_ARCHEOLOGIST) && uwep && uwep->otyp >= BULLWHIP && uwep->otyp <= SECRET_WHIP && uwep->otyp != RUBBER_HOSE)            
		pline("But thanks to your trusty whip ...");
	    dont_fall = "don't fall in.";
	} else if (youmonst.data->msize >= MZ_HUGE) {
	    dont_fall = "don't fit through.";
	} else if (!next_to_u()) {
	    dont_fall = "are jerked back by your pet!";
	}
	if (dont_fall) {
	    You("%s", dont_fall);
	    /* hero didn't fall through, but any objects here might */
	    impact_drop((struct obj *)0, u.ux, u.uy, 0);
	    if (!td) {
		display_nhwindow(WIN_MESSAGE, FALSE);
		pline_The("opening under you closes up.");
	    }
	    return;
	}

	if(*u.ushops) shopdig(1);
	if (Is_stronghold(&u.uz)) {
	    find_hell(&dtmp);
	} else {
	    dtmp.dnum = u.uz.dnum;
	    dtmp.dlevel = newlevel;
	}

	if (wizard && !Can_fall_thru(&u.uz)) {
		extern int n_dgns; /* from dungeon.c */
		int duncounter, num_ok_dungeons, last_ok_dungeon = 0;
		int randomnumber;

		for (duncounter = num_ok_dungeons = 0; duncounter < n_dgns; duncounter++) {
			if (!dungeons[duncounter].dunlev_ureached) continue;
			if (flags.wonderland && !achieve.perform_invocation) {
				if (!strcmp(dungeons[duncounter].dname, "Yendorian Tower")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Forging Chamber")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Dead Grounds")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Ordered Chaos")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TA")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TB")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TC")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TD")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TE")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TF")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TG")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TH")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TI")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TJ")) continue;
			} 
			num_ok_dungeons++;
			last_ok_dungeon = duncounter;

		}
		if (num_ok_dungeons > 1) randomnumber = rnd(num_ok_dungeons);
		else randomnumber = 1;

		dtmp.dnum = u.uz.dnum;

		while (randomnumber > 0) {

			randomnumber--;

			dtmp.dnum++;
			if (dtmp.dnum >= n_dgns) dtmp.dnum = 0;

			while (!dungeons[dtmp.dnum].dunlev_ureached || (flags.wonderland && !achieve.perform_invocation && ( !strcmp(dungeons[dtmp.dnum].dname, "Yendorian Tower") || !strcmp(dungeons[dtmp.dnum].dname, "Forging Chamber") || !strcmp(dungeons[dtmp.dnum].dname, "Dead Grounds") || !strcmp(dungeons[dtmp.dnum].dname, "Ordered Chaos") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TA") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TB") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TC") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TD") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TE") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TF") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TG") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TH") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TI") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TJ") ) ) ) {

				dtmp.dnum++;
				if (dtmp.dnum >= n_dgns) dtmp.dnum = 0;
			}
		}

		dtmp.dlevel = 1;
		if (dungeons[dtmp.dnum].dunlev_ureached > 1) dtmp.dlevel = rnd(dungeons[dtmp.dnum].dunlev_ureached);
	}

	if (!td)
	    sprintf(msgbuf, "The hole in the %s above you closes up.",
		    ceiling(u.ux,u.uy));
	schedule_goto(&dtmp, FALSE, TRUE, 0,
		      (char *)0, !td ? msgbuf : (char *)0);
}

void
fall_throughX(td)
boolean td;	/* td == TRUE : trap door or hole */
{
	d_level dtmp;
	char msgbuf[BUFSZ];
	const char *dont_fall = 0;
	register int newlevel = dunlev(&u.uz);

	/* KMH -- You can't escape the Sokoban level traps */
	if(Blind && Levitation && !(SoiltypeEffect || u.uprops[SOILTYPE].extrinsic || have_soiltypestone() || (uarmf && uarmf->oartifact == ART_ARABELLA_S_GIRL_KICK)) && !In_sokoban(&u.uz)) return;

	do {
	    newlevel++;
	} while(rn2(4) && newlevel < dunlevs_in_dungeon(&u.uz));

	if (wizard && !Can_fall_thru(&u.uz) && (yn("Go down here?") == 'y')) newlevel++;

	if(td) {
	    struct trap *t=t_at(u.ux,u.uy);
	    seetrap(t);
	    if (!In_sokoban(&u.uz)) {
		if (t->ttyp == TRAPDOOR)
			pline("A trap door opens up under you!");
		else if (t->ttyp == SHAFT_TRAP)
			pline("A shaft opens up under you!");
		else if (t->ttyp == CURRENT_SHAFT)
			pline("Air currents pull you into a shaft!");
		else 
			pline("There's a gaping hole under you!");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Dobro pozhalovat' v kakoy-to gorazdo boleye opasnyy uroven'! Udachi vam, vy, veroyatno, ne budet zdes' vyzhit' v lyubom sluchaye." : "Schlueing!");
	    }
	} else pline_The("%s opens up under you!", surface(u.ux,u.uy));

	if (In_sokoban(&u.uz) && Can_fall_thru(&u.uz))
	    ;	/* KMH -- You can't escape the Sokoban level traps */
	else if((Levitation || u.ustuck || (!Can_fall_thru(&u.uz) && !wizard)
	   || Flying || is_clinger(youmonst.data)
	   || (Role_if(PM_ARCHEOLOGIST) && uwep && uwep->otyp >= BULLWHIP && uwep->otyp <= SECRET_WHIP && uwep->otyp != RUBBER_HOSE)
	   || (In_gehennom(&u.uz) && !u.uevent.invoked && newlevel == dunlevs_in_dungeon(&u.uz)))
		&& !(SoiltypeEffect || u.uprops[SOILTYPE].extrinsic || have_soiltypestone() || (uarmf && uarmf->oartifact == ART_ARABELLA_S_GIRL_KICK)) ) {
		if (Role_if(PM_ARCHEOLOGIST) && uwep && uwep->otyp >= BULLWHIP && uwep->otyp <= SECRET_WHIP && uwep->otyp != RUBBER_HOSE)
		pline("But thanks to your trusty whip ...");
	    dont_fall = "don't fall in.";
	} else if (youmonst.data->msize >= MZ_HUGE) {
	    dont_fall = "don't fit through.";
	} else if (!next_to_u()) {
	    dont_fall = "are jerked back by your pet!";
	}
	if (dont_fall) {
	    You("%s", dont_fall);
	    /* hero didn't fall through, but any objects here might */
	    impact_drop((struct obj *)0, u.ux, u.uy, 0);
	    if (!td) {
		display_nhwindow(WIN_MESSAGE, FALSE);
		pline_The("opening under you closes up.");
	    }
	    return;
	}

	if(*u.ushops) shopdig(1);
	if (Is_stronghold(&u.uz)) {
	    find_hell(&dtmp);
	} else {
	    dtmp.dnum = u.uz.dnum;
	    dtmp.dlevel = newlevel;
	}

	if (wizard && !Can_fall_thru(&u.uz)) {
		extern int n_dgns; /* from dungeon.c */
		int duncounter, num_ok_dungeons, last_ok_dungeon = 0;
		int randomnumber;

		for (duncounter = num_ok_dungeons = 0; duncounter < n_dgns; duncounter++) {
			if (!dungeons[duncounter].dunlev_ureached) continue;
			if (flags.wonderland && !achieve.perform_invocation) {
				if (!strcmp(dungeons[duncounter].dname, "Yendorian Tower")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Forging Chamber")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Dead Grounds")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Ordered Chaos")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TA")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TB")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TC")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TD")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TE")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TF")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TG")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TH")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TI")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TJ")) continue;
			} 
			num_ok_dungeons++;
			last_ok_dungeon = duncounter;

		}
		if (num_ok_dungeons > 1) randomnumber = rnd(num_ok_dungeons);
		else randomnumber = 1;

		dtmp.dnum = u.uz.dnum;

		while (randomnumber > 0) {

			randomnumber--;

			dtmp.dnum++;
			if (dtmp.dnum >= n_dgns) dtmp.dnum = 0;

			while (!dungeons[dtmp.dnum].dunlev_ureached || (flags.wonderland && !achieve.perform_invocation && ( !strcmp(dungeons[dtmp.dnum].dname, "Yendorian Tower") || !strcmp(dungeons[dtmp.dnum].dname, "Forging Chamber") || !strcmp(dungeons[dtmp.dnum].dname, "Dead Grounds") || !strcmp(dungeons[dtmp.dnum].dname, "Ordered Chaos") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TA") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TB") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TC") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TD") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TE") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TF") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TG") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TH") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TI") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TJ") ) ) ) {

				dtmp.dnum++;
				if (dtmp.dnum >= n_dgns) dtmp.dnum = 0;
			}
		}

		dtmp.dlevel = 1;
		if (dungeons[dtmp.dnum].dunlev_ureached > 1) dtmp.dlevel = rnd(dungeons[dtmp.dnum].dunlev_ureached);
	}

	if (!td)
	    sprintf(msgbuf, "The hole in the %s above you closes up.",
		    ceiling(u.ux,u.uy));
	schedule_goto(&dtmp, FALSE, TRUE, 0,
		      (char *)0, !td ? msgbuf : (char *)0);
}

/* current shaft - like an ordinary shaft, but levitation or flying don't help --Amy */
void
fall_throughY(td)
boolean td;	/* td == TRUE : trap door or hole */
{
	d_level dtmp;
	char msgbuf[BUFSZ];
	const char *dont_fall = 0;
	register int newlevel = dunlev(&u.uz);

	do {
	    newlevel++;
	} while(rn2(4) && newlevel < dunlevs_in_dungeon(&u.uz));

	if (wizard && !Can_fall_thru(&u.uz) && (yn("Go down here?") == 'y')) newlevel++;

	if(td) {
	    struct trap *t=t_at(u.ux,u.uy);
	    seetrap(t);
	    if (!In_sokoban(&u.uz)) {
		if (t->ttyp == TRAPDOOR)
			pline("A trap door opens up under you!");
		else if (t->ttyp == SHAFT_TRAP)
			pline("A shaft opens up under you!");
		else if (t->ttyp == CURRENT_SHAFT)
			pline("Air currents pull you into a shaft!");
		else 
			pline("There's a gaping hole under you!");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Dobro pozhalovat' v kakoy-to gorazdo boleye opasnyy uroven'! Udachi vam, vy, veroyatno, ne budet zdes' vyzhit' v lyubom sluchaye." : "Schlueing!");
	    }
	} else pline_The("%s opens up under you!", surface(u.ux,u.uy));

	if (In_sokoban(&u.uz) && Can_fall_thru(&u.uz))
	    ;	/* KMH -- You can't escape the Sokoban level traps */
	else if(u.ustuck || (!Can_fall_thru(&u.uz) && !wizard)
	   || (Role_if(PM_ARCHEOLOGIST) && uwep && uwep->otyp >= BULLWHIP && uwep->otyp <= SECRET_WHIP && uwep->otyp != RUBBER_HOSE)
	   || (In_gehennom(&u.uz) && !u.uevent.invoked && newlevel == dunlevs_in_dungeon(&u.uz))
		) {
		if (Role_if(PM_ARCHEOLOGIST) && uwep && uwep->otyp >= BULLWHIP && uwep->otyp <= SECRET_WHIP && uwep->otyp != RUBBER_HOSE)
		pline("But thanks to your trusty whip ...");
	    dont_fall = "don't fall in.";
	} else if (youmonst.data->msize >= MZ_HUGE) {
	    dont_fall = "don't fit through.";
	} else if (!next_to_u()) {
	    dont_fall = "are jerked back by your pet!";
	}
	if (dont_fall) {
	    You("%s", dont_fall);
	    /* hero didn't fall through, but any objects here might */
	    impact_drop((struct obj *)0, u.ux, u.uy, 0);
	    if (!td) {
		display_nhwindow(WIN_MESSAGE, FALSE);
		pline_The("opening under you closes up.");
	    }
	    return;
	}

	if(*u.ushops) shopdig(1);
	if (Is_stronghold(&u.uz)) {
	    find_hell(&dtmp);
	} else {
	    dtmp.dnum = u.uz.dnum;
	    dtmp.dlevel = newlevel;
	}

	if (wizard && !Can_fall_thru(&u.uz)) {
		extern int n_dgns; /* from dungeon.c */
		int duncounter, num_ok_dungeons, last_ok_dungeon = 0;
		int randomnumber;

		for (duncounter = num_ok_dungeons = 0; duncounter < n_dgns; duncounter++) {
			if (!dungeons[duncounter].dunlev_ureached) continue;
			if (flags.wonderland && !achieve.perform_invocation) {
				if (!strcmp(dungeons[duncounter].dname, "Yendorian Tower")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Forging Chamber")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Dead Grounds")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Ordered Chaos")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TA")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TB")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TC")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TD")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TE")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TF")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TG")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TH")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TI")) continue;
				if (!strcmp(dungeons[duncounter].dname, "Resting Zone TJ")) continue;
			} 
			num_ok_dungeons++;
			last_ok_dungeon = duncounter;

		}
		if (num_ok_dungeons > 1) randomnumber = rnd(num_ok_dungeons);
		else randomnumber = 1;

		dtmp.dnum = u.uz.dnum;

		while (randomnumber > 0) {

			randomnumber--;

			dtmp.dnum++;
			if (dtmp.dnum >= n_dgns) dtmp.dnum = 0;

			while (!dungeons[dtmp.dnum].dunlev_ureached || (flags.wonderland && !achieve.perform_invocation && ( !strcmp(dungeons[dtmp.dnum].dname, "Yendorian Tower") || !strcmp(dungeons[dtmp.dnum].dname, "Forging Chamber") || !strcmp(dungeons[dtmp.dnum].dname, "Dead Grounds") || !strcmp(dungeons[dtmp.dnum].dname, "Ordered Chaos") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TA") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TB") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TC") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TD") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TE") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TF") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TG") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TH") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TI") || !strcmp(dungeons[dtmp.dnum].dname, "Resting Zone TJ") ) ) ) {

				dtmp.dnum++;
				if (dtmp.dnum >= n_dgns) dtmp.dnum = 0;
			}
		}

		dtmp.dlevel = 1;
		if (dungeons[dtmp.dnum].dunlev_ureached > 1) dtmp.dlevel = rnd(dungeons[dtmp.dnum].dunlev_ureached);
	}

	if (!td)
	    sprintf(msgbuf, "The hole in the %s above you closes up.",
		    ceiling(u.ux,u.uy));
	schedule_goto(&dtmp, FALSE, TRUE, 0,
		      (char *)0, !td ? msgbuf : (char *)0);
}

/*
 * Animate the given statue.  May have been via shatter attempt, trap,
 * or stone to flesh spell.  Return a monster if successfully animated.
 * If the monster is animated, the object is deleted.  If fail_reason
 * is non-null, then fill in the reason for failure (or success).
 *
 * The cause of animation is:
 *
 *	ANIMATE_NORMAL  - hero "finds" the monster
 *	ANIMATE_SHATTER - hero tries to destroy the statue
 *	ANIMATE_SPELL   - stone to flesh spell hits the statue
 *
 * Perhaps x, y is not needed if we can use get_obj_location() to find
 * the statue's location... ???
 */
struct monst *
animate_statue(statue, x, y, cause, fail_reason)
struct obj *statue;
xchar x, y;
int cause;
int *fail_reason;
{
	struct permonst *mptr;
	struct monst *mon = 0;
	struct obj *item;
	coord cc;
	boolean historic = (Role_if(PM_ARCHEOLOGIST) && !flags.mon_moving && (statue->spe & STATUE_HISTORIC));
	char statuename[BUFSZ];

	strcpy(statuename,the(xname(statue)));

	if (statue->oxlth && statue->oattached == OATTACHED_MONST) {
	    cc.x = x,  cc.y = y;
	    mon = montraits(statue, &cc, FALSE);
	    if (mon && mon->mtame && !mon->isminion)
		wary_dog(mon, TRUE);
	} else {
	    /* statue of any golem hit with stone-to-flesh becomes flesh golem */
	    if (is_golem(&mons[statue->corpsenm]) && cause == ANIMATE_SPELL)
	    	mptr = &mons[PM_FLESHY_GOLEM];
	    else
		mptr = &mons[statue->corpsenm];
	    /*
	     * Guard against someone wishing for a statue of a unique monster
	     * (which is allowed in normal play) and then tossing it onto the
	     * [detected or guessed] location of a statue trap.  Normally the
	     * uppermost statue is the one which would be activated.
	     */
	    if ((mptr->geno & G_UNIQ) && cause != ANIMATE_SPELL) {
	        if (fail_reason) *fail_reason = AS_MON_IS_UNIQUE;
	        return (struct monst *)0;
	    }
	    if (cause == ANIMATE_SPELL &&
		((mptr->geno & G_UNIQ) || mptr->msound == MS_GUARDIAN)) {
		/* Statues of quest guardians or unique monsters
		 * will not stone-to-flesh as the real thing.
		 */
		mon = makemon(&mons[PM_DOPPELGANGER], x, y,
			NO_MINVENT|MM_NOCOUNTBIRTH|MM_ADJACENTOK);
		if (mon) {
			/* makemon() will set mon->cham to
			 * CHAM_ORDINARY if hero is wearing
			 * ring of protection from shape changers
			 * when makemon() is called, so we have to
			 * check the field before calling newcham().
			 */
			if (mon->cham == CHAM_DOPPELGANGER)
				(void) newcham(mon, mptr, FALSE, FALSE);
		}
	    } else
		mon = makemon(mptr, x, y, (cause == ANIMATE_SPELL) ?
			(NO_MINVENT | MM_ADJACENTOK) : NO_MINVENT);
	}

	if (!mon) {
	    if (fail_reason) *fail_reason = AS_NO_MON;
	    return (struct monst *)0;
	}

	/* in case statue is wielded and hero zaps stone-to-flesh at self */
	if (statue->owornmask) remove_worn_item(statue, TRUE);

	/* allow statues to be of a specific gender */
	if (statue->spe & STATUE_MALE)
	    mon->female = FALSE;
	else if (statue->spe & STATUE_FEMALE)
	    mon->female = TRUE;
	/* if statue has been named, give same name to the monster */
	if (statue->onamelth)
	    mon = christen_monst(mon, ONAME(statue));
	/* transfer any statue contents to monster's inventory */
	while ((item = statue->cobj) != 0) {
	    obj_extract_self(item);
	    (void) add_to_minv(mon, item);
	}
	m_dowear(mon, TRUE);
	delobj(statue);

	/* mimic statue becomes seen mimic; other hiders won't be hidden */
	if (mon->m_ap_type) seemimic(mon);
	else mon->mundetected = FALSE;
	if ((x == u.ux && y == u.uy) || cause == ANIMATE_SPELL) {
	    const char *comes_to_life = nonliving(mon->data) ?
					"moves" : "comes to life"; 
	    if (cause == ANIMATE_SPELL)
	    	pline("%s %s!", upstart(statuename),
	    		canspotmon(mon) ? comes_to_life : "disappears");
	    else
		pline_The("statue %s!",
			canspotmon(mon) ? comes_to_life : "disappears");
	    if (historic) {
		    You_feel("guilty that the historic statue is now gone.");
		    adjalign(-10);
			u.ualign.sins++;
			u.alignlim--;
	    }
	} else if (cause == ANIMATE_SHATTER)
	    pline("Instead of shattering, the statue suddenly %s!",
		canspotmon(mon) ? "comes to life" : "disappears");
	else { /* cause == ANIMATE_NORMAL */
	    You("find %s posing as a statue.",
		canspotmon(mon) ? a_monnam(mon) : something);
	    stop_occupation();
	}
	/* avoid hiding under nothing */
	if (x == u.ux && y == u.uy &&
		Upolyd && (hides_under(youmonst.data) || (uarmh && itemhasappearance(uarmh, APP_SECRET_HELMET) ) || (!night() && uarmg && uarmg->oartifact == ART_NIGHTLY_HIGHWAY) || (uarmc && uarmc->oartifact == ART_JANA_S_EXTREME_HIDE_AND_SE) ) && !OBJ_AT(x, y))
	    u.uundetected = 0;

	if (fail_reason) *fail_reason = AS_OK;
	return mon;
	}

/* keyword "newtraps", is a given trap considered a "common trap"? --Amy
 * important: this should be synced against rndtrap() in sp_lev.c, i.e. all traps that have a freq bonus go here */
boolean
is_common_trap(traptype)
int traptype;
{
	switch (traptype) {

		case ARROW_TRAP:
		case DART_TRAP:
		case ROCKTRAP:
		case SQKY_BOARD:
		case BEAR_TRAP:
		case LANDMINE:
		case ROLLING_BOULDER_TRAP:
		case SLP_GAS_TRAP:
		case RUST_TRAP:
		case FIRE_TRAP:
		case PIT:
		case SPIKED_PIT:
		case HOLE:
		case TRAPDOOR:
		case TELEP_TRAP:
		case LEVEL_TELEP:
		case WEB:
		case STATUE_TRAP:
		case MAGIC_TRAP:
		case ANTI_MAGIC:
		case POLY_TRAP:
		case ICE_TRAP:
		case SPEAR_TRAP:
		case SHIT_TRAP:
		case GLYPH_OF_WARDING:
		case SCYTHING_BLADE:
		case BOLT_TRAP:
		case ACID_POOL:
		case WATER_POOL:
		case POISON_GAS_TRAP:
		case SLOW_GAS_TRAP:
		case SHOCK_TRAP:
		case UNKNOWN_TRAP:
		case THROWING_STAR_TRAP:
		case LOUDSPEAKER:
		case LASER_TRAP:
		case CONFUSE_TRAP:
		case STUN_TRAP:
		case HALLUCINATION_TRAP:
		case NUMBNESS_TRAP:
		case FREEZING_TRAP:
		case BURNING_TRAP:
		case FEAR_TRAP:
		case BLINDNESS_TRAP:
		case SPINED_BALL_TRAP:
		case BANANA_TRAP:
		case SLINGSHOT_TRAP:
		case TRIP_ONCE_TRAP:
		case WALL_TRAP:
		case MACE_TRAP:
		case DAGGER_TRAP:
		case PHASEPORTER:
		case SCORE_DRAIN_TRAP:
		case DEBUFF_TRAP:
		case STAT_DAMAGE_TRAP:

			return TRUE;

		default:
			return FALSE;
	}

	return FALSE;

}

/* is a given trap considered a "nasty trap"? --Amy */
boolean
is_nasty_trap(traptype)
int traptype;
{
	switch (traptype) {

		case RMB_LOSS_TRAP:
		case UNINFORMATION_TRAP:
		case BIGSCRIPT_TRAP:
		case BAD_EFFECT_TRAP:
		case CAPTCHA_TRAP:
		case WEAKNESS_TRAP:
		case TRAINING_TRAP:
		case ALIGNMENT_TRAP:
		case DISPLAY_TRAP:
		case SPELL_LOSS_TRAP:
		case STEALER_TRAP:
		case REBELLION_TRAP:
		case EXERCISE_TRAP:
		case NUPESELL_TRAP:
		case CASTER_TRAP:
		case QUIZ_TRAP:
		case INTRINSIC_LOSS_TRAP:
		case NONINTRINSICAL_TRAP:
		case LOW_STATS_TRAP:
		case CRAP_TRAP:
		case YELLOW_SPELL_TRAP:
		case MISFIRE_TRAP:
		case SOUND_TRAP:
		case LOW_EFFECT_TRAP:
		case EGOTRAP:
		case LEVEL_TRAP:
		case WEAK_SIGHT_TRAP:
		case DEHYDRATION_TRAP:
		case RANDOM_MESSAGE_TRAP:
		case MULTIPLY_TRAP:
		case TRAP_OF_ROTTENNESS:
		case DSTW_TRAP:
		case STATUS_TRAP:
		case MENU_TRAP:
		case AUTO_DESTRUCT_TRAP:
		case DIFFICULTY_TRAP:
		case BANK_TRAP:
		case STAIRS_TRAP:
		case FREE_HAND_TRAP:
		case AUTO_VULN_TRAP:
		case STARVATION_TRAP:
		case HATE_TRAP:
		case UNSKILLED_TRAP:
		case MAGIC_DEVICE_TRAP:
		case DROPCURSE_TRAP:
		case ROT_THIRTEEN_TRAP:
		case TRAP_OF_NO_RETURN:
		case GHOST_WORLD_TRAP:
		case DROP_TRAP:
		case ENGRAVING_TRAP:
		case BLOOD_LOSS_TRAP:
		case BOOK_TRAP:
		case MEMORY_TRAP:
		case RESPAWN_TRAP:
		case MAP_TRAP:
		case METABOLIC_TRAP:
		case UNIDENTIFY_TRAP:
		case NTLL_TRAP:
		case INVENTORY_TRAP:
		case THIRST_TRAP:
		case VERISIERT:
		case DROPLESS_TRAP:
		case NAKEDNESS_TRAP:
		case FAINT_TRAP:
		case MUTENESS_TRAP:
		case TRAP_OF_WALLS:
		case CURSE_TRAP:
		case CHAOS_TRAP:
		case FARLOOK_TRAP:
		case SHADES_OF_GREY_TRAP:
		case TELE_ITEMS_TRAP:
		case TECH_TRAP:
		case FAST_FORWARD_TRAP:
		case LIMITATION_TRAP:
		case DESECRATION_TRAP:
		case BISHOP_TRAP:
		case SPEED_TRAP:
		case CONFUSION_TRAP:
		case INVISIBLE_TRAP:
		case LUCK_TRAP:
		case ONLY_TRAP:
		case DISENCHANT_TRAP:
		case BLACK_NG_WALL_TRAP:
		case ANTILEVEL_TRAP:
		case SUPERSCROLLER_TRAP:
		case TOTTER_TRAP:
		case NASTINESS_TRAP:
		case DISCONNECT_TRAP:
		case INTERFACE_SCREW_TRAP:
		case BOSSFIGHT_TRAP:
		case ENTIRE_LEVEL_TRAP:
		case BONES_TRAP:
		case AUTOCURSE_TRAP:
		case HIGHLEVEL_TRAP:
		case SPELL_FORGETTING_TRAP:
		case SOUND_EFFECT_TRAP:
		case TIMERUN_TRAP:
		case MEAN_BURDEN_TRAP:
		case CARRCAP_TRAP:
		case UMENG_TRAP:
		case AEFDE_TRAP:
		case EPVI_TRAP:
		case FUCKFUCKFUCK_TRAP:
		case OPTION_TRAP:
		case MISCOLOR_TRAP:
		case ONE_RAINBOW_TRAP:
		case COLORSHIFT_TRAP:
		case TOP_LINE_TRAP:
		case CAPS_TRAP:
		case UN_KNOWLEDGE_TRAP:
		case DARKHANCE_TRAP:
		case DSCHUEUEUET_TRAP:
		case NOPESKILL_TRAP:
		case REPEATING_NASTYCURSE_TRAP:
		case REALLY_BAD_TRAP:
		case COVID_TRAP:
		case ARTIBLAST_TRAP:
		case REAL_LIE_TRAP:
		case ESCAPE_PAST_TRAP:
		case PETHATE_TRAP:
		case PET_LASHOUT_TRAP:
		case PETSTARVE_TRAP:
		case PETSCREW_TRAP:
		case TECH_LOSS_TRAP:
		case PROOFLOSS_TRAP:
		case UN_INVIS_TRAP:
		case DETECTATION_TRAP:

		case LOOTCUT_TRAP:
		case MONSTER_SPEED_TRAP:
		case SCALING_TRAP:
		case ENMITY_TRAP:
		case WHITE_SPELL_TRAP:
		case COMPLETE_GRAY_SPELL_TRAP:
		case QUASAR_TRAP:
		case MOMMA_TRAP:
		case HORROR_TRAP:
		case ARTIFICER_TRAP:
		case WEREFORM_TRAP:
		case NON_PRAYER_TRAP:
		case EVIL_PATCH_TRAP:
		case HARD_MODE_TRAP:
		case SECRET_ATTACK_TRAP:
		case EATER_TRAP:
		case COVETOUSNESS_TRAP:
		case NOT_SEEN_TRAP:
		case DARK_MODE_TRAP:
		case ANTISEARCH_TRAP:
		case HOMICIDE_TRAP:
		case NASTY_NATION_TRAP:
		case WAKEUP_CALL_TRAP:
		case GRAYOUT_TRAP:
		case GRAY_CENTER_TRAP:
		case CHECKERBOARD_TRAP:
		case CLOCKWISE_SPIN_TRAP:
		case COUNTERCLOCKWISE_SPIN_TRAP:
		case LAG_TRAP:
		case BLESSCURSE_TRAP:
		case DE_LIGHT_TRAP:
		case DISCHARGE_TRAP:
		case TRASHING_TRAP:
		case FILTERING_TRAP:
		case DEFORMATTING_TRAP:
		case FLICKER_STRIP_TRAP:
		case UNDRESSING_TRAP:
		case HYPERBLUEWALL_TRAP:
		case NOLITE_TRAP:
		case PARANOIA_TRAP:
		case FLEECESCRIPT_TRAP:
		case INTERRUPT_TRAP:
		case DUSTBIN_TRAP:
		case MANA_BATTERY_TRAP:
		case MONSTERFINGERS_TRAP:
		case MISCAST_TRAP:
		case MESSAGE_SUPPRESSION_TRAP:
		case STUCK_ANNOUNCEMENT_TRAP:
		case BLOODTHIRSTY_TRAP:
		case MAXIMUM_DAMAGE_TRAP:
		case LATENCY_TRAP:
		case STARLIT_TRAP:
		case KNOWLEDGE_TRAP:
		case HIGHSCORE_TRAP:
		case PINK_SPELL_TRAP:
		case GREEN_SPELL_TRAP:
		case EVC_TRAP:
		case UNDERLAYER_TRAP:
		case DAMAGE_METER_TRAP:
		case ARBITRARY_WEIGHT_TRAP:
		case FUCKED_INFO_TRAP:
		case BLACK_SPELL_TRAP:
		case CYAN_SPELL_TRAP:
		case HEAP_TRAP:
		case BLUE_SPELL_TRAP:
		case TRON_TRAP:
		case RED_SPELL_TRAP:
		case TOO_HEAVY_TRAP:
		case ELONGATION_TRAP:
		case WRAPOVER_TRAP:
		case DESTRUCTION_TRAP:
		case MELEE_PREFIX_TRAP:
		case AUTOMORE_TRAP:
		case UNFAIR_ATTACK_TRAP:
		case ORANGE_SPELL_TRAP:
		case VIOLET_SPELL_TRAP:
		case TRAP_OF_LONGING:
		case CURSED_PART_TRAP:
		case QUAVERSAL_TRAP:
		case APPEARANCE_SHUFFLING_TRAP:
		case BROWN_SPELL_TRAP:
		case CHOICELESS_TRAP:
		case GOLDSPELL_TRAP:
		case DEPROVEMENT_TRAP:
		case INITIALIZATION_TRAP:
		case GUSHLUSH_TRAP:
		case SOILTYPE_TRAP:
		case DANGEROUS_TERRAIN_TRAP:
		case FALLOUT_TRAP:
		case MOJIBAKE_TRAP:
		case GRAVATION_TRAP:
		case UNCALLED_TRAP:
		case EXPLODING_DICE_TRAP:
		case PERMACURSE_TRAP:
		case SHROUDED_IDENTITY_TRAP:
		case FEELER_GAUGES_TRAP:
		case LONG_SCREWUP_TRAP:
		case WING_YELLOW_CHANGER:
		case LIFE_SAVING_TRAP:
		case CURSEUSE_TRAP:
		case CUT_NUTRITION_TRAP:
		case SKILL_LOSS_TRAP:
		case AUTOPILOT_TRAP:
		case FORCE_TRAP:
		case MONSTER_GLYPH_TRAP:
		case CHANGING_DIRECTIVE_TRAP:
		case CONTAINER_KABOOM_TRAP:
		case STEAL_DEGRADE_TRAP:
		case LEFT_INVENTORY_TRAP:
		case FLUCTUATING_SPEED_TRAP:
		case TARMUSTROKINGNORA_TRAP:
		case FAILURE_TRAP:
		case BRIGHT_CYAN_SPELL_TRAP:
		case FREQUENTATION_SPAWN_TRAP:
		case PET_AI_TRAP:
		case SATAN_TRAP:
		case REMEMBERANCE_TRAP:
		case POKELIE_TRAP:
		case AUTOPICKUP_TRAP:
		case DYWYPI_TRAP:
		case SILVER_SPELL_TRAP:
		case METAL_SPELL_TRAP:
		case PLATINUM_SPELL_TRAP:
		case MANLER_TRAP:
		case DOORNING_TRAP:
		case NOWNSIBLE_TRAP:
		case ELM_STREET_TRAP:
		case MONNOISE_TRAP:
		case RANG_CALL_TRAP:
		case RECURRING_SPELL_LOSS_TRAP:
		case ANTITRAINING_TRAP:
		case TECHOUT_TRAP:
		case STAT_DECAY_TRAP:
		case MOVEMORK_TRAP:
		case BAD_PART_TRAP:
		case COMPLETELY_BAD_PART_TRAP:
		case EVIL_VARIANT_TRAP:
		case SANITY_TREBLE_TRAP:
		case STAT_DECREASE_TRAP:
		case SIMEOUT_TRAP:
		case GIANT_EXPLORER_TRAP:
		case TRAPWARP_TRAP:
		case YAWM_TRAP:
		case CRADLE_OF_CHAOS_TRAP:
		case TEZCATLIPOCA_TRAP:
		case ENTHUMESIS_TRAP:
		case MIKRAANESIS_TRAP:
		case GOTS_TOO_GOOD_TRAP:
		case KILLER_ROOM_TRAP:
		case NO_FUN_WALLS_TRAP:

			return TRUE;

		default:
			return FALSE;

	}

	return FALSE;

}

/*
 * You've either stepped onto a statue trap's location or you've triggered a
 * statue trap by searching next to it or by trying to break it with a wand
 * or pick-axe.
 */
struct monst *
activate_statue_trap(trap, x, y, shatter)
struct trap *trap;
xchar x, y;
boolean shatter;
{
	struct monst *mtmp = (struct monst *)0;
	struct obj *otmp = sobj_at(STATUE, x, y);
	int fail_reason;

	/*
	 * Try to animate the first valid statue.  Stop the loop when we
	 * actually create something or the failure cause is not because
	 * the mon was unique.
	 */
	deltrap(trap);
	while (otmp) {
	    mtmp = animate_statue(otmp, x, y,
		    shatter ? ANIMATE_SHATTER : ANIMATE_NORMAL, &fail_reason);
	    if (mtmp || fail_reason != AS_MON_IS_UNIQUE) break;

	    while ((otmp = otmp->nexthere) != 0)
		if (otmp->otyp == STATUE) break;
	}

	if (Blind) feel_location(x, y);
	else newsym(x, y);
	return mtmp;
}

STATIC_OVL boolean
keep_saddle_with_steedcorpse(steed_mid, objchn, saddle)
unsigned steed_mid;
struct obj *objchn, *saddle;
{
	if (!saddle) return FALSE;
	while(objchn) {
		if(objchn->otyp == CORPSE &&
		   objchn->oattached == OATTACHED_MONST && objchn->oxlth) {
			struct monst *mtmp = (struct monst *)objchn->oextra;
			if (mtmp->m_id == steed_mid) {
				/* move saddle */
				xchar x,y;
				if (get_obj_location(objchn, &x, &y, 0)) {
					obj_extract_self(saddle);
					place_object(saddle, x, y);
					stackobj(saddle);
				}
				return TRUE;
			}
		}
		if (Has_contents(objchn) &&
		    keep_saddle_with_steedcorpse(steed_mid, objchn->cobj, saddle))
			return TRUE;
		objchn = objchn->nobj;
	}
	return FALSE;
}

void
dotrap(trap, trflags)
register struct trap *trap;
unsigned trflags;
{
	register int ttype = trap->ttyp;
	register struct obj *otmp;
	boolean already_seen = trap->tseen;
	boolean webmsgok = (!(trflags & NOWEBMSG));
	boolean forcebungle = (trflags & FORCEBUNGLE);
	boolean dontreveal = (trflags & DONTREVEAL);
	boolean skipgarbage = (trflags & SKIPGARBAGE);

	int monstcnt; /* for animation trap */
	int ptmp;	/* for poison gas trap */
	int nastytrapdur;	/* for nasty traps */
	int femmytrapdur;	/* for femininity traps */
	int blackngdur;	/* timeout for black ng wall trap after which you die */

	register int randsp;
	register int randmnst;
	struct permonst *randmonstforspawn;
	register int monstercolor;
	register int randmnsx;
	int i;

	int randomamount = 0;
	int randomx, randomy;

	struct obj *otmpi, *otmpii;

	nastytrapdur = (Role_if(PM_GRADUATE) ? 12 : Role_if(PM_GEEK) ? 25 : 50);
	if (!nastytrapdur) nastytrapdur = 50; /* fail safe */

	if (YouAreScrewedEternally) nastytrapdur *= 20;
	if (LongScrewupXtra) nastytrapdur *= 10;

	if (uleft && uleft->otyp == RIN_NASTINESS_RESISTANCE) {
		nastytrapdur *= 9;
		nastytrapdur /= 10;
	}
	if (uright && uright->otyp == RIN_NASTINESS_RESISTANCE) {
		nastytrapdur *= 9;
		nastytrapdur /= 10;
	}
	if (RngeNastyReduction && nastytrapdur > 1) nastytrapdur /= 2;
	if (uarmh && uarmh->oartifact == ART_HAHAREDUCTION && nastytrapdur > 1) nastytrapdur /= 2;
	if (autismringcheck(ART_ARABELLA_S_NASTYGUARD)) nastytrapdur /= 2; /* having two of them does not stack --Amy */

	if (nastytrapdur < 1) nastytrapdur = 1; /* fail safe */

	femmytrapdur = (Role_if(PM_LADIESMAN) ? 5 : Role_if(PM_SEXYMATE) ? 10 : 20);

	if (FemtrapActiveRosa) {
		femmytrapdur *= 5;
		if (SuperFemtrapRosa) femmytrapdur *= 2;
	}
	if (!rn2(2)) femmytrapdur /= 2;
	if (!rn2(5)) femmytrapdur /= 3;
	if (!rn2(20)) femmytrapdur /= 5;

	if (femmytrapdur < 1) femmytrapdur = 1; /* fail safe */

	blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
	if (!blackngdur ) blackngdur = 500; /* fail safe */

	/* ATTENTION: anything below this point, but above the part where it checks which trap you've triggered, is meant
	 * for when the player actually walked into a trap. This is SLEX, monsters (e.g. while under the effect of a
	 * rita trap) as well as other effects (giant explorer, triggers nasty traps close to you) can cause traps to
	 * act as if you triggered them, but we don't want them to become visible in that case or otherwise run their
	 * standard procedures because that could result in odd behavior --Amy
	 *
	 * Anything that is required to be set up so that the trap works properly (e.g. nasty trap duration) has to be set
	 * prior to this, since it should be set up when something else triggers the trap for the player too. */

	if (skipgarbage) goto dothetrap;

	if (trap && RngeDenastification) pline("A %s has been sprung.", defsyms[trap_to_defsym(ttype)].explanation);

	if (trap && Yawming) u.yawmtime = 0;

	/* Traps are 50% more likely to fail for a pickpocket */
	if (!In_sokoban(&u.uz) && Role_if(PM_PICKPOCKET) && ttype != MAGIC_PORTAL && rn2(2)) return;

	if (!In_sokoban(&u.uz) && uwep && uwep->oartifact == ART_CUTE_JAPANESE_FEET && rn2(2) && ttype != MAGIC_PORTAL) {
		if ((trap && trap->tseen) || rn2(4)) You("avoid a trap!");
		return;
	}

	/* Players could deduce the position of a nasty trap by running in a corridor. This would probably come into effect
	 * rarely, but the fact that it was possible at all was unintentional, so I'm closing this loophole just to cover
	 * my butt. Nasty traps are supposed to be really difficult to spot! --Amy */
	if (!is_nasty_trap(ttype) && ttype != AUTOMATIC_SWITCHER && ttype != ACTIVE_SUPERSCROLLER_TRAP && ttype != PET_TRAP && ttype != SPREADING_TRAP && ttype != ADJACENT_TRAP && ttype != SUPERTHING_TRAP && ttype != ONLY_TRAP && ttype != NTLL_TRAP && ttype != LOUDSPEAKER && ttype != ARABELLA_SPEAKER && ttype != KOP_CUBE && ttype != BOSS_SPAWNER)
		nomul(0, 0, FALSE);

	if (NownsibleEffect || u.uprops[NOWNSIBLE_EFFECT].extrinsic || have_nownsiblestone() || (uarmh && uarmh->oartifact == ART_MASK_OF_TLALOC) ) {
		trap->tseen = 0;
		trap->hiddentrap = 1;
	/* dissidents want to uncover the evil plans of the government, and that includes the nasty traps they set up :P
	 * therefore they'll have a small chance of uncovering such traps --Amy */
	} else if (Role_if(PM_DISSIDENT) && !dontreveal && !rn2(20) && (trap->hiddentrap || !trap->tseen)) {
		trap->hiddentrap = 0;
		pline("Suddenly a trap is revealed underneath you!");
		trap->tseen = 1;
	} else if ((uarm && uarm->otyp == EILISTRAN_ARMOR) && !dontreveal && !rn2(20) && (trap->hiddentrap || !trap->tseen)) {
		trap->hiddentrap = 0;
		pline("Suddenly a trap is revealed underneath you!");
		trap->tseen = 1;
	}

	/* KMH -- You can't escape the Sokoban level traps */
	if (In_sokoban(&u.uz) &&
			(ttype == PIT || ttype == SPIKED_PIT || ttype == GIANT_CHASM || ttype == SHIT_PIT || ttype == MANA_PIT || ttype == ANOXIC_PIT || ttype == HYPOXIC_PIT || ttype == SHAFT_TRAP || ttype == ACID_PIT || ttype == HOLE || ttype == CURRENT_SHAFT ||
			ttype == TRAPDOOR)) {
	    /* The "air currents" message is still appropriate -- even when
	     * the hero isn't flying or levitating -- because it conveys the
	     * reason why the player cannot escape the trap with a dexterity
	     * check, clinging to the ceiling, etc.
	     */
	    pline("Air currents pull you down into %s %s!",
	    	a_your[trap->madeby_u],
	    	defsyms[trap_to_defsym(ttype)].explanation);
	    /* then proceed to normal trap effect */
	} else if (already_seen) {
	    if ((Levitation || Flying) && !(SoiltypeEffect || u.uprops[SOILTYPE].extrinsic || have_soiltypestone() || (uarmf && uarmf->oartifact == ART_ARABELLA_S_GIRL_KICK)) &&
		    (ttype == PIT || ttype == SPIKED_PIT || ttype == GIANT_CHASM || ttype == HOLE ||
		    ttype == BEAR_TRAP)) {
		You("%s over %s %s.",
		    Levitation ? "float" : "fly",
		    a_your[trap->madeby_u],
		    defsyms[trap_to_defsym(ttype)].explanation);
		return;
	    }
	    if(!Fumbling && !is_nasty_trap(ttype) && ttype != MAGIC_PORTAL && ttype != AUTOMATIC_SWITCHER && ttype != ACTIVE_SUPERSCROLLER_TRAP && ttype != PET_TRAP && ttype != SPREADING_TRAP && ttype != ADJACENT_TRAP && ttype != ONLY_TRAP && ttype != NTLL_TRAP && ttype != LOUDSPEAKER && ttype != ARABELLA_SPEAKER && !forcebungle &&
		(!rn2(5) ||
	    ((ttype == PIT || ttype == SPIKED_PIT || ttype == GIANT_CHASM || ttype == SHIT_PIT || ttype == MANA_PIT || ttype == ANOXIC_PIT || ttype == HYPOXIC_PIT || ttype == ACID_PIT) && is_clinger(youmonst.data)))) {
		You("escape %s %s.",
		    (ttype == ARROW_TRAP && !trap->madeby_u) ? "an" :
			a_your[trap->madeby_u],
		    defsyms[trap_to_defsym(ttype)].explanation);
		if (ttype == FART_TRAP || ttype == PERSISTENT_FART_TRAP) {
			pline("But you're longing for a sexy girl so much, so...");
		} else
			return;
	    }
	}

	/*if (u.usteed) u.usteed->mtrapseen |= (1 << (ttype-1));*/

	if (uwep && uwep->oartifact == ART_KA_BLAMMO) {
		boolean goup = rn2(2);
		if (uwep->blessed && !rn2(4)) goup = 1;
		if (uwep->cursed && !rn2(4)) goup = 0;

		if (goup && trap && !(trap->artionce) && uwep->spe < 10) uwep->spe++;
		if (!goup && uwep->spe > -20) uwep->spe--;

		/* No message, because you're not supposed to know that you just triggered a trap. --Amy */
	}

	if (uwep && uwep->oartifact == ART_BANGCOCK) {
		boolean goup = rn2(2);
		if (uwep->blessed && !rn2(4)) goup = 1;
		if (uwep->cursed && !rn2(4)) goup = 0;

		if (goup && trap && !(trap->artionce) && uwep->spe < 10) uwep->spe++;
		if (!goup && uwep->spe > -20) uwep->spe--;

		/* No message, because you're not supposed to know that you just triggered a trap. --Amy */
	}

	if (uwep && uwep->oartifact == ART_INTELLIGENT_POPE) {
		boolean goup = rn2(2);
		if (uwep->blessed && !rn2(4)) goup = 1;
		if (uwep->cursed && !rn2(4)) goup = 0;

		if (goup && trap && !(trap->artionce) && uwep->spe < 10) uwep->spe++;
		if (!goup && uwep->spe > -20) uwep->spe--;

		/* No message, because you're not supposed to know that you just triggered a trap. --Amy */
	}

	if (uarmc && uarmc->oartifact == ART_JANA_S_FAIRNESS_CUP && !rn2(100)) {
		u.youaredead = 1;
		pline("NETHACK caused a General Protection Fault at address 0001:0001.");
		killer_format = KILLED_BY;
		killer = "Jana's (un)fairness";
		done(DIED);
		u.youaredead = 0;
	}

	if (uarmf && uarmf->oartifact == ART_SOFTSTEP && ttype == SHIT_TRAP && !(trap->artionce)) {
		pline("Spflotch! Your dancing shoes fully stepped into a heap of dog shit.");
		adjattrib(A_CHA, 1, 0, TRUE);
	}

	if (uarm && uarm->oartifact == ART_SHIT_HAPPENZ && ttype == SHIT_TRAP && !(trap->artionce)) {
		gain_alla(1);
	}

	if (trap) trap->artionce = TRUE;

dothetrap:

	switch(ttype) {
	    case ARROW_TRAP:
		if (trap->once && !rn2(15)) {
		    You_hear("a loud click!");
		    deltrap(trap);
		    newsym(u.ux,u.uy);
		    break;
		}
		trap->once = 1;
		seetrap(trap);
		pline("An arrow shoots out at you!");
		otmp = mksobj(ARROW, TRUE, FALSE, FALSE);
		if (otmp) {
			int projectiledamage = dmgval(otmp, &youmonst);
			if (projectiledamage > 1) {
				if (u.ulevel == 1) projectiledamage /= 2;
				else if (u.ulevel == 2) {
					projectiledamage *= 2;
					projectiledamage /= 3;
				} else if (u.ulevel == 3) {
					projectiledamage *= 3;
					projectiledamage /= 4;
				} else if (u.ulevel == 4) {
					projectiledamage *= 4;
					projectiledamage /= 5;
				}
			}

			otmp->quan = 1L;
			otmp->owt = weight(otmp);
			otmp->opoisoned = 0;
			if (u.usteed && will_hit_steed() && steedintrap(trap, otmp)) /* nothing */;
			else
			if (thitu(8 + rnd((monster_difficulty() / 2) + 1), projectiledamage + rnd((monster_difficulty() / 2) + 1) , otmp, "arrow")) {
			    obfree(otmp, (struct obj *)0);
			} else if (!timebasedlowerchance()) {
			    obfree(otmp, (struct obj *)0);
			} else {
			    place_object(otmp, u.ux, u.uy);
			    if (!Blind) otmp->dknown = 1;
			    stackobj(otmp);
			    newsym(u.ux, u.uy);
			}
		}
		break;

	    case POISON_ARROW_TRAP:
		if (trap->once && !rn2(15)) {
		    You_hear("a loud click!");
		    deltrap(trap);
		    newsym(u.ux,u.uy);
		    break;
		}
		trap->once = 1;
		seetrap(trap);
		pline("An arrow shoots out at you!");
		otmp = mksobj(ARROW, TRUE, FALSE, FALSE);
		if (otmp) {
			int projectiledamage = dmgval(otmp, &youmonst);
			if (projectiledamage > 1) {
				if (u.ulevel == 1) projectiledamage /= 2;
				else if (u.ulevel == 2) {
					projectiledamage *= 2;
					projectiledamage /= 3;
				} else if (u.ulevel == 3) {
					projectiledamage *= 3;
					projectiledamage /= 4;
				} else if (u.ulevel == 4) {
					projectiledamage *= 4;
					projectiledamage /= 5;
				}
			}

			otmp->quan = 1L;
			otmp->owt = weight(otmp);
			otmp->opoisoned = TRUE;
			if (u.usteed && will_hit_steed() && steedintrap(trap, otmp)) /* nothing */;
			else
			if (thitu(8 + rnd((monster_difficulty() / 2) + 1), projectiledamage + rnd((monster_difficulty() / 2) + 1) , otmp, "arrow")) {
			    obfree(otmp, (struct obj *)0);
			} else if (!timebasedlowerchance()) {
			    obfree(otmp, (struct obj *)0);
			} else {
			    place_object(otmp, u.ux, u.uy);
			    if (!Blind) otmp->dknown = 1;
			    stackobj(otmp);
			    newsym(u.ux, u.uy);
			}
		}
		break;

	    case BOLT_TRAP:
		if (trap->once && !rn2(15)) {
		    You_hear("a loud click!");
		    deltrap(trap);
		    newsym(u.ux,u.uy);
		    break;
		}
		trap->once = 1;
		seetrap(trap);
		pline("A bolt shoots out at you!");
		otmp = mksobj(CROSSBOW_BOLT, TRUE, FALSE, FALSE);
		if (otmp) {
			int projectiledamage = dmgval(otmp, &youmonst);
			if (projectiledamage > 1) {
				if (u.ulevel == 1) projectiledamage /= 2;
				else if (u.ulevel == 2) {
					projectiledamage *= 2;
					projectiledamage /= 3;
				} else if (u.ulevel == 3) {
					projectiledamage *= 3;
					projectiledamage /= 4;
				} else if (u.ulevel == 4) {
					projectiledamage *= 4;
					projectiledamage /= 5;
				}
			}

			otmp->quan = 1L;
			otmp->owt = weight(otmp);
			otmp->opoisoned = 0;
			if (u.usteed && will_hit_steed() && steedintrap(trap, otmp)) /* nothing */;
			else
			if (thitu(8 + rnd((monster_difficulty() * 2 / 3) + 1), projectiledamage + rnd((monster_difficulty() * 2 / 3) + 1) , otmp, "bolt")) {
			    obfree(otmp, (struct obj *)0);
			} else if (!timebasedlowerchance()) {
			    obfree(otmp, (struct obj *)0);
			} else {
			    place_object(otmp, u.ux, u.uy);
			    if (!Blind) otmp->dknown = 1;
			    stackobj(otmp);
			    newsym(u.ux, u.uy);
			}
		}
		break;

	    case POISON_BOLT_TRAP:
		if (trap->once && !rn2(15)) {
		    You_hear("a loud click!");
		    deltrap(trap);
		    newsym(u.ux,u.uy);
		    break;
		}
		trap->once = 1;
		seetrap(trap);
		pline("A bolt shoots out at you!");
		otmp = mksobj(CROSSBOW_BOLT, TRUE, FALSE, FALSE);
		if (otmp) {
			int projectiledamage = dmgval(otmp, &youmonst);
			if (projectiledamage > 1) {
				if (u.ulevel == 1) projectiledamage /= 2;
				else if (u.ulevel == 2) {
					projectiledamage *= 2;
					projectiledamage /= 3;
				} else if (u.ulevel == 3) {
					projectiledamage *= 3;
					projectiledamage /= 4;
				} else if (u.ulevel == 4) {
					projectiledamage *= 4;
					projectiledamage /= 5;
				}
			}

			otmp->quan = 1L;
			otmp->owt = weight(otmp);
			otmp->opoisoned = TRUE;
			if (u.usteed && will_hit_steed() && steedintrap(trap, otmp)) /* nothing */;
			else
			if (thitu(8 + rnd((monster_difficulty() * 2 / 3) + 1), projectiledamage + rnd((monster_difficulty() * 2 / 3) + 1) , otmp, "bolt")) {
			    obfree(otmp, (struct obj *)0);
			} else if (!timebasedlowerchance()) {
			    obfree(otmp, (struct obj *)0);
			} else {
			    place_object(otmp, u.ux, u.uy);
			    if (!Blind) otmp->dknown = 1;
			    stackobj(otmp);
			    newsym(u.ux, u.uy);
			}
		}
		break;

	    case BULLET_TRAP:
		if (trap->once && !rn2(15)) {
		    You_hear("a soft click.");
		    deltrap(trap);
		    newsym(u.ux,u.uy);
		    break;
		}
		trap->once = 1;
		seetrap(trap);
		pline("BANG! A gun shoots at you!");
		otmp = mksobj(PISTOL_BULLET, TRUE, FALSE, FALSE);
		if (otmp) {
			int projectiledamage = dmgval(otmp, &youmonst);
			if (projectiledamage > 1) {
				if (u.ulevel == 1) projectiledamage /= 2;
				else if (u.ulevel == 2) {
					projectiledamage *= 2;
					projectiledamage /= 3;
				} else if (u.ulevel == 3) {
					projectiledamage *= 3;
					projectiledamage /= 4;
				} else if (u.ulevel == 4) {
					projectiledamage *= 4;
					projectiledamage /= 5;
				}
			}

			otmp->quan = 1L;
			otmp->owt = weight(otmp);
			otmp->opoisoned = 0;
			if (u.usteed && will_hit_steed() && steedintrap(trap, otmp)) /* nothing */;
			else
			if (thitu(8 + rnd(monster_difficulty() + 1), projectiledamage + rnd((monster_difficulty() * 2 / 3) + 1) , otmp, "bullet")) {
			    obfree(otmp, (struct obj *)0);
			} else {
			    obfree(otmp, (struct obj *)0);
			}
		}
		break;

	    case GLASS_ARROW_TRAP:
		if (trap->once && !rn2(15)) {
		    You_hear("a loud click!");
		    deltrap(trap);
		    newsym(u.ux,u.uy);
		    break;
		}
		trap->once = 1;
		seetrap(trap);
		pline("A glass arrow shoots out at you!");
		otmp = mksobj(DROVEN_ARROW, TRUE, FALSE, FALSE);
		if (otmp) {
			int projectiledamage = dmgval(otmp, &youmonst);
			if (projectiledamage > 1) {
				if (u.ulevel == 1) projectiledamage /= 2;
				else if (u.ulevel == 2) {
					projectiledamage *= 2;
					projectiledamage /= 3;
				} else if (u.ulevel == 3) {
					projectiledamage *= 3;
					projectiledamage /= 4;
				} else if (u.ulevel == 4) {
					projectiledamage *= 4;
					projectiledamage /= 5;
				}
			}

			otmp->quan = 1L;
			otmp->owt = weight(otmp);
			otmp->opoisoned = 0;
			if (u.usteed && will_hit_steed() && steedintrap(trap, otmp)) /* nothing */;
			else
			if (thitu(8 + rnd((monster_difficulty() / 2) + 1), projectiledamage + rnd((monster_difficulty() / 2) + 1) , otmp, "glass arrow")) {
			    obfree(otmp, (struct obj *)0);
			} else {
			    obfree(otmp, (struct obj *)0);
			}
		}
		break;

	    case GLASS_BOLT_TRAP:
		if (trap->once && !rn2(15)) {
		    You_hear("a loud click!");
		    deltrap(trap);
		    newsym(u.ux,u.uy);
		    break;
		}
		trap->once = 1;
		seetrap(trap);
		pline("A glass bolt shoots out at you!");
		otmp = mksobj(DROVEN_BOLT, TRUE, FALSE, FALSE);
		if (otmp) {
			int projectiledamage = dmgval(otmp, &youmonst);
			if (projectiledamage > 1) {
				if (u.ulevel == 1) projectiledamage /= 2;
				else if (u.ulevel == 2) {
					projectiledamage *= 2;
					projectiledamage /= 3;
				} else if (u.ulevel == 3) {
					projectiledamage *= 3;
					projectiledamage /= 4;
				} else if (u.ulevel == 4) {
					projectiledamage *= 4;
					projectiledamage /= 5;
				}
			}

			otmp->quan = 1L;
			otmp->owt = weight(otmp);
			otmp->opoisoned = 0;
			if (u.usteed && will_hit_steed() && steedintrap(trap, otmp)) /* nothing */;
			else
			if (thitu(8 + rnd((monster_difficulty() * 2 / 3) + 1), projectiledamage + rnd((monster_difficulty() * 2 / 3) + 1) , otmp, "glass bolt")) {
			    obfree(otmp, (struct obj *)0);
			} else {
			    obfree(otmp, (struct obj *)0);
			}
		}
		break;

	    case FART_TRAP:
	    case PERSISTENT_FART_TRAP:

		if (!trap->tseen) pline("You discover a construction, behind which there is a sexy girl waiting... but you can only see her butt. It seems that her name is %s.", farttrapnames[trap->launch_otyp]);
		else pline("You can't resist the temptation to caress %s's sexy butt.", farttrapnames[trap->launch_otyp]);

		seetrap(trap);

		if (trap->launch_otyp < 12) pline("%s produces %s farting noises with her sexy butt.", farttrapnames[trap->launch_otyp], rn2(2) ? "tender" : "soft");
		else if (trap->launch_otyp < 33) pline("%s produces %s farting noises with her sexy butt.", farttrapnames[trap->launch_otyp], rn2(2) ? "beautiful" : "squeaky");
		else pline("%s produces %s farting noises with her sexy butt.", farttrapnames[trap->launch_otyp], rn2(2) ? "disgusting" : "loud");
		u.cnd_fartingcount++;
		if (Role_if(PM_CLIMACTERIAL)) climtrainsqueaking(1);
		if (Role_if(PM_BUTT_LOVER) && !rn2(20)) buttlovertrigger();
		if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();

		if (trap->launch_otyp < 12 && uarmf && uarmf->oartifact == ART_SARAH_S_GRANNY_WEAR) {
			healup((level_difficulty() + 5), 0, FALSE, FALSE);
			if (!rn2(100)) deltrap(trap);
			break;
		}

		if (uarmf && uarmf->oartifact == ART_ELIANE_S_SHIN_SMASH) {
			pline("The farting gas destroys your footwear instantly.");
		      useup(uarmf);
		}

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

		break;

	    case MAGIC_BEAM_TRAP:
		You_hear("a soft click.");
		seetrap(trap);
		if (isok(trap->launch.x,trap->launch.y) && IS_STWALL(levl[trap->launch.x][trap->launch.y].typ)) {
		    buzz(trap->launch_otyp, 5 + rnd((monster_difficulty() / 10) + 1) ,
			 trap->launch.x,trap->launch.y,
			 sgn(trap->tx - trap->launch.x),sgn(trap->ty - trap->launch.y));
		    trap->once = 1;
		} else {
		    deltrap(trap);
		    newsym(u.ux,u.uy);
			break; /* reduce risk of segfaulting */
		}
		if (!rn2(20)) deltrap(trap);
		break;
	    case PIERCING_BEAM_TRAP:
		You_hear("a soft click.");
		seetrap(trap);
		u.uprops[DEAC_REFLECTING].intrinsic += rnd(5);
		if (isok(trap->launch.x,trap->launch.y) && IS_STWALL(levl[trap->launch.x][trap->launch.y].typ)) {
		    buzz(trap->launch_otyp, 5 + rnd((monster_difficulty() / 10) + 1) ,
			 trap->launch.x,trap->launch.y,
			 sgn(trap->tx - trap->launch.x),sgn(trap->ty - trap->launch.y));
		    trap->once = 1;
		} else {
		    deltrap(trap);
		    newsym(u.ux,u.uy);
			break; /* reduce risk of segfaulting */
		}
		if (!rn2(20)) deltrap(trap);
		break;
	    case DART_TRAP:
		if (trap->once && !rn2(15)) {
		    You_hear("a soft click.");
		    deltrap(trap);
		    newsym(u.ux,u.uy);
		    break;
		}
		trap->once = 1;
		seetrap(trap);
		pline("A little dart shoots out at you!");
		otmp = mksobj(DART, TRUE, FALSE, FALSE);
		if (otmp) {
			int projectiledamage = dmgval(otmp, &youmonst);
			if (projectiledamage > 1) {
				if (u.ulevel == 1) projectiledamage /= 2;
				else if (u.ulevel == 2) {
					projectiledamage *= 2;
					projectiledamage /= 3;
				} else if (u.ulevel == 3) {
					projectiledamage *= 3;
					projectiledamage /= 4;
				} else if (u.ulevel == 4) {
					projectiledamage *= 4;
					projectiledamage /= 5;
				}
			}

			otmp->quan = 1L;
			otmp->owt = weight(otmp);
			if (!rn2(6) && !(uarmf && uarmf->oartifact == ART_DAROH_NO) ) otmp->opoisoned = 1;
			if (u.usteed && will_hit_steed() && steedintrap(trap, otmp)) /* nothing */;
			else
			if (thitu(7 + rnd((monster_difficulty() / 3) + 1), projectiledamage + rnd((monster_difficulty() / 3) + 1), otmp, "little dart")) {
			    if (otmp->opoisoned)
				poisoned("dart", A_CON, "little dart", -10);
			    obfree(otmp, (struct obj *)0);
			} else if (!timebasedlowerchance()) {
			    obfree(otmp, (struct obj *)0);
			} else {
			    place_object(otmp, u.ux, u.uy);
			    if (!Blind) otmp->dknown = 1;
			    stackobj(otmp);
			    newsym(u.ux, u.uy);
			}
		}
		break;
	    case THROWING_STAR_TRAP:
		if (trap->once && !rn2(15)) {
		    You_hear("a soft click.");
		    deltrap(trap);
		    newsym(u.ux,u.uy);
		    break;
		}
		trap->once = 1;
		seetrap(trap);
		pline("A shuriken shoots out at you!");
		otmp = mksobj(SHURIKEN, TRUE, FALSE, FALSE);
		if (otmp) {
			int projectiledamage = dmgval(otmp, &youmonst);
			if (projectiledamage > 1) {
				if (u.ulevel == 1) projectiledamage /= 2;
				else if (u.ulevel == 2) {
					projectiledamage *= 2;
					projectiledamage /= 3;
				} else if (u.ulevel == 3) {
					projectiledamage *= 3;
					projectiledamage /= 4;
				} else if (u.ulevel == 4) {
					projectiledamage *= 4;
					projectiledamage /= 5;
				}
			}

			otmp->quan = 1L;
			otmp->owt = weight(otmp);
			if (!rn2(6)) otmp->opoisoned = 1;
			if (u.usteed && will_hit_steed() && steedintrap(trap, otmp)) /* nothing */;
			else
			if (thitu(8 + rnd((monster_difficulty() / 2) + 1), projectiledamage + rnd((monster_difficulty() / 2) + 1), otmp, "shuriken")) {
			    if (otmp->opoisoned)
				poisoned("shuriken", A_CON, "shuriken", -10);
			    obfree(otmp, (struct obj *)0);
			} else if (!timebasedlowerchance()) {
			    obfree(otmp, (struct obj *)0);
			} else {
			    place_object(otmp, u.ux, u.uy);
			    if (!Blind) otmp->dknown = 1;
			    stackobj(otmp);
			    newsym(u.ux, u.uy);
			}
		}
		break;
	    case ROCKTRAP:
		if (trap->once && !rn2(15)) {
		    pline("A trap door in %s opens, but nothing falls out!",
			  the(ceiling(u.ux,u.uy)));
		    deltrap(trap);
		    newsym(u.ux,u.uy);
		} else {
		    int dmg = d(2,6) + rnd((monster_difficulty() * 2 / 3) + 1); /* should be std ROCK dmg? */
			if (dmg > 1) {
				if (u.ulevel == 1) dmg /= 2;
				else if (u.ulevel == 2) {
					dmg *= 2;
					dmg /= 3;
				} else if (u.ulevel == 3) {
					dmg *= 3;
					dmg /= 4;
				} else if (u.ulevel == 4) {
					dmg *= 4;
					dmg /= 5;
				}
			}

		    trap->once = 1;
		    seetrap(trap);
		    otmp = mksobj_at(ROCK, u.ux, u.uy, TRUE, FALSE, FALSE);
		    if (otmp) {
			    otmp->quan = 1L;
			    otmp->owt = weight(otmp);

			    pline("A trap door in %s opens and %s falls on your %s!",
				  the(ceiling(u.ux,u.uy)),
				  an(xname(otmp)),
				  body_part(HEAD));

			    if (uarmh) {
				if(is_hardmaterial(uarmh)) {
				    pline("Fortunately, you are wearing a hard helmet.");
				    dmg /= 4;
				} else if (flags.verbose) {
				    Your("%s does not protect you.", xname(uarmh));
				}
			    }

			    if (!Blind) otmp->dknown = 1;
			    stackobj(otmp);
			    newsym(u.ux,u.uy);	/* map the rock */

			    losehp(dmg, "falling rock", KILLED_BY_AN);
			    exercise(A_STR, FALSE);
		    }
		}
		break;

	    case FALLING_LOADSTONE_TRAP:
		{
		    int dmg = rnd(30) + rnd(monster_difficulty() + 1);
			if (dmg > 1) {
				if (u.ulevel == 1) {
					dmg *= 3;
					dmg /= 10;
				} else if (u.ulevel == 2) {
					dmg /= 2;
				} else if (u.ulevel == 3) {
					dmg *= 2;
					dmg /= 3;
				} else if (u.ulevel == 4) {
					dmg *= 3;
					dmg /= 4;
				}
			}

		    otmp = mksobj_at(LOADSTONE, u.ux, u.uy, TRUE, FALSE, FALSE);
		    if (otmp) {
			    otmp->quan = 1L;
			    otmp->owt = weight(otmp);

			    pline("A trap door in %s opens and %s falls on your %s!",
				  the(ceiling(u.ux,u.uy)),
				  an(xname(otmp)),
				  body_part(HEAD));

			    if (uarmh) {
				if(is_hardmaterial(uarmh)) {
				    pline("Fortunately, you are wearing a hard helmet.");
				    dmg /= 2;
				} else if (flags.verbose) {
				    Your("%s does not protect you.", xname(uarmh));
				}
			    }

			    if (!Blind) otmp->dknown = 1;
			    if (otmp) {
			      pline("%s lands in your knapsack!", Doname2(otmp));
				(void) pickup_object(otmp, 1L, TRUE, TRUE);
			    }

			    losehp(dmg, "falling loadstone", KILLED_BY_AN);
			    exercise(A_STR, FALSE);
			}
		}
		deltrap(trap); /* only triggers once */
		break;

	    case FALLING_NASTYSTONE_TRAP:
		{
		    int dmg = rnd(20) + rnd(monster_difficulty() + 1);
			if (dmg > 1) {
				if (u.ulevel == 1) {
					dmg *= 3;
					dmg /= 10;
				} else if (u.ulevel == 2) {
					dmg /= 2;
				} else if (u.ulevel == 3) {
					dmg *= 2;
					dmg /= 3;
				} else if (u.ulevel == 4) {
					dmg *= 3;
					dmg /= 4;
				}
			}

		    otmp = mksobj_at(rnd_class(RIGHT_MOUSE_BUTTON_STONE,NASTY_STONE), u.ux, u.uy, TRUE, FALSE, FALSE);
		    if (otmp) {
			    otmp->quan = 1L;
			    otmp->owt = weight(otmp);

			    pline("A trap door in %s opens and %s falls on your %s!",
				  the(ceiling(u.ux,u.uy)),
				  an(xname(otmp)),
				  body_part(HEAD));

			    if (uarmh) {
				if(is_hardmaterial(uarmh)) {
				    pline("Fortunately, you are wearing a hard helmet.");
				    dmg /= 2;
				} else if (flags.verbose) {
				    Your("%s does not protect you.", xname(uarmh));
				}
			    }

			    if (!Blind) otmp->dknown = 1;
			    if (otmp) {
			      pline("%s lands in your knapsack!", Doname2(otmp));
				(void) pickup_object(otmp, 1L, TRUE, TRUE);
			    }

			    losehp(dmg, "falling loadstone", KILLED_BY_AN);
			    exercise(A_STR, FALSE);
			}
		}
		deltrap(trap); /* only triggers once */
		break;

	    case FALLING_BOULDER_TRAP:
		if (trap->once && !rn2(15)) {
		    pline("A trap door in %s opens, but nothing falls out!",
			  the(ceiling(u.ux,u.uy)));
		    deltrap(trap);
		    newsym(u.ux,u.uy);
		} else {
		    int dmg = d(6,6) + rnd(monster_difficulty() + 1); /* should be std BOULDER dmg? */
			if (dmg > 1) {
				if (u.ulevel == 1) {
					dmg *= 3;
					dmg /= 10;
				} else if (u.ulevel == 2) {
					dmg /= 2;
				} else if (u.ulevel == 3) {
					dmg *= 2;
					dmg /= 3;
				} else if (u.ulevel == 4) {
					dmg *= 3;
					dmg /= 4;
				}
			}

		    trap->once = 1;
		    seetrap(trap);
		    otmp = mksobj_at(BOULDER, u.ux, u.uy, TRUE, FALSE, FALSE);
		    if (otmp) {
			    otmp->quan = 1L;
			    otmp->owt = weight(otmp);

			    pline("A trap door in %s opens and %s falls on your %s!",
				  the(ceiling(u.ux,u.uy)),
				  an(xname(otmp)),
				  body_part(HEAD));

			    if (uarmh) {
				if(is_hardmaterial(uarmh)) {
				    pline("Fortunately, you are wearing a hard helmet.");
				    dmg /= 2;
				} else if (flags.verbose) {
				    Your("%s does not protect you.", xname(uarmh));
				}
			    }

			    if (!Blind) otmp->dknown = 1;
			    stackobj(otmp);
			    newsym(u.ux,u.uy);	/* map the rock */

			    losehp(dmg, "falling boulder", KILLED_BY_AN);
			    exercise(A_STR, FALSE);
			}
		}
		break;

	    case SQKY_BOARD:	    /* stepped on a squeaky board */
		if ((Levitation || Flying) && !(SoiltypeEffect || u.uprops[SOILTYPE].extrinsic || have_soiltypestone() || (uarmf && uarmf->oartifact == ART_ARABELLA_S_GIRL_KICK)) ) {
		    if (!Blind) {
			seetrap(trap);
			if (FunnyHallu)
				You("notice a crease in the linoleum.");
			else
				You("notice a loose board below you.");
		    }
		} else {
		    seetrap(trap);
		    pline("A board beneath you squeaks loudly.");
		    wake_nearby();
		}
		break;

	    case NOISE_TRAP:
		{
		    pline("You stepped on a trigger!");
		    wake_nearby();
			register struct monst *mtmp3;
			for(mtmp3 = fmon; mtmp3; mtmp3 = mtmp3->nmon) {
			    if (!DEADMONSTER(mtmp3) && !rn2(3)) {
				mtmp3->msleeping = 0;
			    }
			}
			pline("Your ears are blasted by hellish noise!");
			int dmg = rnd(16) + rnd( (monster_difficulty() * 2 / 3) + 1);
			if (YouAreDeaf) dmg /= 2;
			make_stunned(HStun + dmg, TRUE);
			if (isevilvariant || !rn2(issoviet ? 2 : 5)) (void)destroy_item(POTION_CLASS, AD_COLD);
	      	if (dmg) losehp(dmg, "noise trap", KILLED_BY_AN);

		    seetrap(trap);
		}
		break;

	    case BEAR_TRAP:
		if ((Levitation || Flying) && !(SoiltypeEffect || u.uprops[SOILTYPE].extrinsic || have_soiltypestone() || (uarmf && uarmf->oartifact == ART_ARABELLA_S_GIRL_KICK)) ) break;
		seetrap(trap);
		if(amorphous(youmonst.data) || is_whirly(youmonst.data) ||
						    unsolid(youmonst.data)) {
		    pline("%s bear trap closes harmlessly through you.",
			    A_Your[trap->madeby_u]);
		    break;
		}
		if(!u.usteed && youmonst.data->msize <= MZ_SMALL) {
		    pline("%s bear trap closes harmlessly over you.",
			    A_Your[trap->madeby_u]);
		    break;
		}
		u.utrap = rn1(4, 4);
		u.utraptype = TT_BEARTRAP;
		if (u.usteed) {
		    pline("%s bear trap closes on %s %s!",
			A_Your[trap->madeby_u], s_suffix(mon_nam(u.usteed)),
			mbodypart(u.usteed, FOOT));
		} else
		{
		    pline("%s bear trap closes on your %s!",
			    A_Your[trap->madeby_u], body_part(FOOT));
		    if(u.umonnum == PM_OWLBEAR || u.umonnum == PM_BUGBEAR)
			You("howl in anger!");
		}
		exercise(A_DEX, FALSE);
		break;

	    case LEVITATION_TRAP:

		if (Levitation) break;
		seetrap(trap);

		pline("Uh-oh, should have watched your step...");
		pline(FunnyHallu ? "Wow... you're suddenly walking on air!" : "You float up!");

		HLevitation &= ~I_SPECIAL;
		incr_itimeout(&HLevitation, rnz(50));

		break;

	    case LEG_TRAP:
		seetrap(trap);
		u.utrap = rn1(4, 4);
		u.utraptype = TT_BEARTRAP;
		if (u.usteed) {
		    pline("%s leg trap closes on %s %s!",
			A_Your[trap->madeby_u], s_suffix(mon_nam(u.usteed)),
			mbodypart(u.usteed, FOOT));
		} else
		{
		    pline("%s leg trap closes on your %s!",
			    A_Your[trap->madeby_u], body_part(FOOT));
		    if(u.umonnum == PM_OWLBEAR || u.umonnum == PM_BUGBEAR)
			You("howl in anger!");
		}
		exercise(A_DEX, FALSE);
		set_wounded_legs(RIGHT_SIDE, HWounded_legs + rnz(50));
		break;

	    case CHLOROFORM_TRAP:
		seetrap(trap);

		if (uarmh && uarmh->oartifact == ART_VACUUM_CLEANER_DEATH) {
			pline("The gas asphyxiates you!");
			losehp(rnd(u.ulevel * 3), "suffocating in a gas trap", KILLED_BY);
		}

		if (uarmh && itemhasappearance(uarmh, APP_FILTERED_HELMET) && !rn2(2) ) {
		    You("are enveloped in a cloud of gas!");
		    break;
		}

		if (RngeGasFiltering && !rn2(2)) {
		    You("are enveloped in a cloud of gas!");
		    break;
		}

		if((Sleep_resistance && rn2(StrongSleep_resistance ? 5 : 3)) || breathless(youmonst.data)) {
		    You("are enveloped in a cloud of gas!");
		    break;
		}
		pline("A cloud of gas puts you to sleep!");
		fall_asleep(-rnd(45 + rnd((monster_difficulty() / 2) + 1) ), TRUE);
		(void) steedintrap(trap, (struct obj *)0);

		break;

	    case SLP_GAS_TRAP:
		seetrap(trap);

		if (uarmh && uarmh->oartifact == ART_VACUUM_CLEANER_DEATH) {
			pline("The gas asphyxiates you!");
			losehp(rnd(u.ulevel * 3), "suffocating in a gas trap", KILLED_BY);
		}

		if (uarmh && itemhasappearance(uarmh, APP_FILTERED_HELMET) && !rn2(2) ) {
		    You("are enveloped in a cloud of gas!");
		    break;
		}

		if (RngeGasFiltering && !rn2(2)) {
		    You("are enveloped in a cloud of gas!");
		    break;
		}

		if((Sleep_resistance && rn2(StrongSleep_resistance ? 20 : 5)) || breathless(youmonst.data)) {
		    You("are enveloped in a cloud of gas!");
		    break;
		}
		pline("A cloud of gas puts you to sleep!");
		fall_asleep(-rnd(25 + rnd((monster_difficulty() / 4) + 1) ), TRUE);
		(void) steedintrap(trap, (struct obj *)0);
		break;

	    case PARALYSIS_TRAP:
	      pline("You stepped on a trigger!");
		seetrap(trap);
		if (!Free_action || !rn2(StrongFree_action ? 100 : 10)) {
		    pline("You are frozen in place!");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
		    if (isstunfish) nomul(-rnz(10), "frozen by a paralysis trap", TRUE);
		    else nomul(-rnd(10), "frozen by a paralysis trap", TRUE);
		    nomovemsg = You_can_move_again;
		    exercise(A_DEX, FALSE);
		} else You("stiffen momentarily.");
		(void) steedintrap(trap, (struct obj *)0);
		break;

	    case POISON_GAS_TRAP:
		seetrap(trap);

		if (uarmh && uarmh->oartifact == ART_VACUUM_CLEANER_DEATH) {
			pline("The gas asphyxiates you!");
			losehp(rnd(u.ulevel * 3), "suffocating in a gas trap", KILLED_BY);
		}

		if (uarmh && itemhasappearance(uarmh, APP_FILTERED_HELMET) && !rn2(2) ) {
		    pline("A cloud of gas surrounds you!");
		    break;
		}

		if (RngeGasFiltering && !rn2(2)) {
		    pline("A cloud of gas surrounds you!");
		    break;
		}

		if(breathless(youmonst.data)) {
		    pline("A cloud of gas surrounds you!");
		    break;
		}
		pline("A cloud of gas surrounds you!");
		ptmp = rn2(6); /* A_STR ... A_CHA */
		    poisoned("gas", ptmp, "poison gas trap", 30);
		(void) steedintrap(trap, (struct obj *)0);

		break;

	    case SLOW_GAS_TRAP:
		seetrap(trap);

		if (uarmh && uarmh->oartifact == ART_VACUUM_CLEANER_DEATH) {
			pline("The gas asphyxiates you!");
			losehp(rnd(u.ulevel * 3), "suffocating in a gas trap", KILLED_BY);
		}

		if (uarmh && itemhasappearance(uarmh, APP_FILTERED_HELMET) && !rn2(2) ) {
		    pline("A cloud of foggy gas shoots out at you!");
		    break;
		}

		if (RngeGasFiltering && !rn2(2)) {
		    pline("A cloud of foggy gas shoots out at you!");
		    break;
		}

		if(breathless(youmonst.data)) {
		    pline("A cloud of foggy gas shoots out at you!");
		    break;
		}
		pline("A cloud of foggy gas shoots out at you!");
		u_slow_down();

		if (!rn2(3)) make_frozen(HFrozen + ( (monster_difficulty() + 1) * 5), TRUE); /* randomly freeze the player --Amy */

		(void) steedintrap(trap, (struct obj *)0);
		break;

	    case ACID_POOL:

		if ((Levitation || Flying) && !(SoiltypeEffect || u.uprops[SOILTYPE].extrinsic || have_soiltypestone() || (uarmf && uarmf->oartifact == ART_ARABELLA_S_GIRL_KICK)) ) break; /* this trap is ground-based */

		seetrap(trap);

	        int num = 0;
		struct obj *target;
	        num = d(4, 4) + rnd((monster_difficulty() / 2) + 1);

			if (num > 1) {
				if (u.ulevel == 1) num /= 2;
				else if (u.ulevel == 2) {
					num *= 2;
					num /= 3;
				} else if (u.ulevel == 3) {
					num *= 3;
					num /= 4;
				} else if (u.ulevel == 4) {
					num *= 4;
					num /= 5;
				}
			}

	        pline("You are sprayed with acid!");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vasha glupaya bronya budet korroziyey, potomu chto vy ne sledite glupyy igrok durak." : "Tsch-tschhh-hhh!");
		if (Stoned) fix_petrification();
	        if (Acid_resistance) {
                shieldeff(u.ux, u.uy);
                num = d(1, 2) + rnd((monster_difficulty() / 8) + 1);
	        }

			if (!rn2(5)) {target = some_armor(&youmonst);
			if (target) (void)rust_dmg(target, xname(target), 3, TRUE, &youmonst);
			}
			if (!rn2(5)) {target = some_armor(&youmonst);
			if (target) (void)rust_dmg(target, xname(target), 3, TRUE, &youmonst);
			}
			if (!rn2(5)) {target = some_armor(&youmonst);
			if (target) (void)rust_dmg(target, xname(target), 3, TRUE, &youmonst);
			}

	        if (num) losehp(num, "acidpool", KILLED_BY_AN);

		break;

	    case WATER_POOL:

		if ((Levitation || Flying) && !(SoiltypeEffect || u.uprops[SOILTYPE].extrinsic || have_soiltypestone() || (uarmf && uarmf->oartifact == ART_ARABELLA_S_GIRL_KICK)) ) break; /* this trap is ground-based */

		seetrap(trap);

			if (Levitation) {
				pline("You float over a deep pool of water.");
				break;
			}
			if (Flying) {
				pline("You fly over a deep pool of water.");
				break;
			}
			if (Wwalking || Race_if(PM_KORONST)) {
				pline("There is a deep pool of water right below your feet.");
				break;
			}

	        int numX = 0;
	        numX = d(3, 3) + rnd((monster_difficulty() / 3) + 1);

			if (num > 1) {
				if (u.ulevel == 1) num /= 2;
				else if (u.ulevel == 2) {
					num *= 2;
					num /= 3;
				} else if (u.ulevel == 3) {
					num *= 3;
					num /= 4;
				} else if (u.ulevel == 4) {
					num *= 4;
					num /= 5;
				}
			}

		  if (Swimming) {pline("You fell into a pool of water, but thankfully you can swim."); numX = 0;}
		  else if (Amphibious || Breathless) {pline("You fell into a pool of water, but you aren't drowning."); numX = 0;}
	        else pline("You fell into a pool of water!");

			if (!rn2(20)) { /* A higher chance would be incredibly annoying. --Amy */
				if ((!StrongSwimming || !rn2(10)) && (!StrongMagical_breathing || !rn2(10))) {
					water_damage(invent, FALSE, FALSE);
					if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);
				}
			}

	        if (numX) losehp(numX, "waterpool", KILLED_BY_AN);

		break;

	    case CORROSION_TRAP:

		seetrap(trap);
		pline("A gush of acid hits you!");

		switch (rn2(5)) {
		    case 0:
			pline("The acid lands on your %s!", body_part(HEAD));
			(void) rust_dmg(uarmh, "helmet", 3, TRUE, &youmonst);
			break;
		    case 1:
			pline("The acid lands on your left %s!", body_part(ARM));
			if (rust_dmg(uarms, "shield", 3, TRUE, &youmonst))
			    break;
			if (u.twoweap || (uwep && bimanual(uwep))) {
			    otmp = u.twoweap ? uswapwep : uwep;
			    if (otmp && !snuff_lit(otmp))
				(void) rust_dmg(otmp, "weapon", 3, TRUE, &youmonst);
			}
glovecheck:		(void) rust_dmg(uarmg, "gauntlets", 3, TRUE, &youmonst);
			/* Not "metal gauntlets" since it gets called
			 * even if it's leather for the message
			 */
			break;
		    case 2:
			pline("The acid lands on your right %s!", body_part(ARM));
			if (uwep && !snuff_lit(uwep))
				(void) rust_dmg(uwep, "weapon", 3, TRUE, &youmonst);
			goto glovecheck;
		    default:
			if (uarmc)
			    (void) rust_dmg(uarmc, cloak_simple_name(uarmc),
						3, TRUE, &youmonst);
			else if (uarm)
			    (void) rust_dmg(uarm, "armor", 3, TRUE, &youmonst);
			else if (uarmu)
			    (void) rust_dmg(uarmu, "shirt", 3, TRUE, &youmonst);
		}
		update_inventory();

		break;

	    case FLAME_TRAP:

		seetrap(trap);
		You("are engulfed in flames!");

		switch (rn2(5)) {
		    case 0:
			(void) rust_dmg(uarmh, "helmet", 0, TRUE, &youmonst);
			break;
		    case 1:
			if (rust_dmg(uarms, "shield", 0, TRUE, &youmonst))
			    break;
			if (u.twoweap || (uwep && bimanual(uwep))) {
			    otmp = u.twoweap ? uswapwep : uwep;
			    if (otmp && !snuff_lit(otmp))
				(void) rust_dmg(otmp, "weapon", 0, TRUE, &youmonst);
			}
glovecheckX:		(void) rust_dmg(uarmg, "gauntlets", 0, TRUE, &youmonst);
			/* Not "metal gauntlets" since it gets called
			 * even if it's leather for the message
			 */
			break;
		    case 2:
			if (uwep && !snuff_lit(uwep))
				(void) rust_dmg(uwep, "weapon", 0, TRUE, &youmonst);
			goto glovecheckX;
		    default:
			if (uarmc)
			    (void) rust_dmg(uarmc, cloak_simple_name(uarmc),
						0, TRUE, &youmonst);
			else if (uarm)
			    (void) rust_dmg(uarm, "armor", 0, TRUE, &youmonst);
			else if (uarmu)
			    (void) rust_dmg(uarmu, "shirt", 0, TRUE, &youmonst);
		}
		update_inventory();

		break;

	    case WITHER_TRAP:

		seetrap(trap);
		You("are covered with antimatter!");

		switch (rn2(5)) {
		    case 0:
			(void) wither_dmg(uarmh, "helmet", rn2(4), TRUE, &youmonst);
			break;
		    case 1:
			if (wither_dmg(uarms, "shield", rn2(4), TRUE, &youmonst))
			    break;
			if (u.twoweap || (uwep && bimanual(uwep))) {
			    otmp = u.twoweap ? uswapwep : uwep;
			    if (otmp && !snuff_lit(otmp))
				(void) wither_dmg(otmp, "weapon", rn2(4), TRUE, &youmonst);
			}
glovecheckY:		(void) wither_dmg(uarmg, "gauntlets", rn2(4), TRUE, &youmonst);
			/* Not "metal gauntlets" since it gets called
			 * even if it's leather for the message
			 */
			break;
		    case 2:
			if (uwep && !snuff_lit(uwep))
				(void) wither_dmg(uwep, "weapon", rn2(4), TRUE, &youmonst);
			goto glovecheckY;
		    default:
			if (uarmc)
			    (void) wither_dmg(uarmc, cloak_simple_name(uarmc),
						0, TRUE, &youmonst);
			else if (uarm)
			    (void) wither_dmg(uarm, "armor", rn2(4), TRUE, &youmonst);
			else if (uarmu)
			    (void) wither_dmg(uarmu, "shirt", rn2(4), TRUE, &youmonst);
		}
		update_inventory();

		break;

	    case RUST_TRAP:
		seetrap(trap);
		if (u.umonnum == PM_IRON_GOLEM) {
		    int dam = u.mhmax;

		    pline("%s you!", A_gush_of_water_hits);
		    You("are covered with rust!");
		    if (Half_physical_damage && (rn2(2) || (uwep && uwep->oartifact == ART_SOOTHE_)) ) dam = (dam+1) / 2;
		    if (StrongHalf_physical_damage && (rn2(2) || (uwep && uwep->oartifact == ART_SOOTHE_)) ) dam = (dam+1) / 2;
		    losehp(dam, "rusting away", KILLED_BY);
		    break;
		} else if (u.umonnum == PM_FLAMING_SPHERE || u.umonnum == PM_SUMMONED_FLAMING_SPHERE) {
		    int dam = u.mhmax;

		    pline("%s you!", A_gush_of_water_hits);
		    You("are extinguished!");
		    if (Half_physical_damage && (rn2(2) || (uwep && uwep->oartifact == ART_SOOTHE_)) ) dam = (dam+1) / 2;
		    if (StrongHalf_physical_damage && (rn2(2) || (uwep && uwep->oartifact == ART_SOOTHE_)) ) dam = (dam+1) / 2;
		    losehp(dam, "drenching", KILLED_BY);
		    break;
		} else if (splittinggremlin(youmonst.data) && rn2(3)) {
		    pline("%s you!", A_gush_of_water_hits);
		    (void)split_mon(&youmonst, (struct monst *)0);
		    break;
		}

	    /* Unlike monsters, traps cannot aim their rust attacks at
	     * you, so instead of looping through and taking either the
	     * first rustable one or the body, we take whatever we get,
	     * even if it is not rustable.
	     */
		switch (rn2(5)) {
		    case 0:
			pline("%s you on the %s!", A_gush_of_water_hits,
				    body_part(HEAD));
			(void) rust_dmg(uarmh, "helmet", 1, TRUE, &youmonst);
			break;
		    case 1:
			pline("%s your left %s!", A_gush_of_water_hits,
				    body_part(ARM));
			if (rust_dmg(uarms, "shield", 1, TRUE, &youmonst))
			    break;
			if (u.twoweap || (uwep && bimanual(uwep))) {
			    otmp = u.twoweap ? uswapwep : uwep;
			    if (otmp && !snuff_lit(otmp))
				erode_obj(otmp, FALSE, TRUE);
			}
glovecheckZ:		(void) rust_dmg(uarmg, "gauntlets", 1, TRUE, &youmonst);
			/* Not "metal gauntlets" since it gets called
			 * even if it's leather for the message
			 */
			break;
		    case 2:
			pline("%s your right %s!", A_gush_of_water_hits,
				    body_part(ARM));
			if (uwep && !snuff_lit(uwep))
			    erode_obj(uwep, FALSE, TRUE);
			goto glovecheckZ;
		    default:
			pline("%s you!", A_gush_of_water_hits);
			for (otmp=invent; otmp; otmp = otmp->nobj)
				    (void) snuff_lit(otmp);
			if (uarmc)
			    (void) rust_dmg(uarmc, cloak_simple_name(uarmc),
						1, TRUE, &youmonst);
			else if (uarm)
			    (void) rust_dmg(uarm, "armor", 1, TRUE, &youmonst);
			else if (uarmu)
			    (void) rust_dmg(uarmu, "shirt", 1, TRUE, &youmonst);
		}
		update_inventory();
		break;

	    case FIRE_TRAP:
		seetrap(trap);

		if (Role_if(PM_COOK) && !rn2(2)) {

			pline("You stomp and extinguish a fire trap.");
		    deltrap(trap);
			break;
		}

		if (uarmc && uarmc->oartifact == ART_COOKING_COURSE && !rn2(2)) {

			pline("You stomp and extinguish a fire trap.");
		    deltrap(trap);
			break;
		}

		dofiretrap((struct obj *)0);
		break;

	    case ICE_TRAP:
		seetrap(trap);
		doicetrap((struct obj *)0);
		break;

	    case PLASMA_TRAP:
		seetrap(trap);
		doplasmatrap((struct obj *)0);
		break;

	    case SHOCK_TRAP:
		seetrap(trap);
		doshocktrap((struct obj *)0);
		break;

	    case VOLT_TRAP:
		seetrap(trap);
		dovolttrap((struct obj *)0);
		break;

	    case ELEMENTAL_TRAP:
		seetrap(trap);
		if (rn2(2)) {
			dofiretrap((struct obj *)0);
			doicetrap((struct obj *)0);
			doshocktrap((struct obj *)0);
		} else {
			doicetrap((struct obj *)0);
			dofiretrap((struct obj *)0);
			doshocktrap((struct obj *)0);
		}
		break;

	    case LASER_TRAP:
		seetrap(trap);
		pline("You stepped on a trigger!");
		pline("A laser beam hits you!");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vy poteryali linii!" : "DWUEUEUET!");
		losehp(monster_difficulty(),"laser trap",KILLED_BY_AN);
		break;

	    case ESCALATING_TRAP:
		seetrap(trap);
		pline("You stepped on a trigger!");
		pline("Something hits your %s.", body_part(rn2(19) ) );
		losehp(1 + u.chokhmahdamage + rnd(u.ualign.sins > 0 ? (isqrt(u.ualign.sins) + 1) : (1)),"escalating trap",KILLED_BY_AN);
		u.chokhmahdamage++;
		break;

	    case ANIMATION_TRAP:

		monstcnt = 1;
		if (!rn2(5)) monstcnt += 1;
		if (!rn2(25)) monstcnt += 2;
		if (!rn2(125)) monstcnt += 3;
		if (!rn2(725)) monstcnt += rnd(20);

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

	      while(--monstcnt >= 0) {
		(void) makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
		}

		u.aggravation = 0;

	pline("es come to life!"); /* garbled string from Castle of the Winds. This trap summons random monsters. --Amy */

		deltrap(trap); /* only triggers once */
		break;

	    case SUMMON_UNDEAD_TRAP:

		monstcnt = rnd(3);
		if (!rn2(3)) monstcnt += rnd(2);
		if (!rn2(10)) monstcnt += rnd(5);
		if (!rn2(50)) monstcnt += rnd(10);
		if (!rn2(200)) monstcnt += rnd(20);

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

	      while(--monstcnt >= 0) {

		    switch (rnd(10)) {
		    case 1:
			(void) makemon(mkclass(S_VAMPIRE,0), u.ux, u.uy, NO_MM_FLAGS);
			break;
		    case 2:
		    case 3:
		    case 4:
		    case 5:
			(void) makemon(mkclass(S_ZOMBIE,0), u.ux, u.uy, NO_MM_FLAGS);
			break;
		    case 6:
		    case 7:
		    case 8:
			(void) makemon(mkclass(S_MUMMY,0), u.ux, u.uy, NO_MM_FLAGS);
			break;
		    case 9:
			(void) makemon(mkclass(S_GHOST,0), u.ux, u.uy, NO_MM_FLAGS);
			break;
		    case 10:
			(void) makemon(mkclass(S_WRAITH,0), u.ux, u.uy, NO_MM_FLAGS);
			break;
		    }
		}

		u.aggravation = 0;

		pline("ge broadcast by Arabella to confuse unwary players.");

		deltrap(trap); /* only triggers once */
		break;

	    case HORDE_TRAP:

		monstcnt = 6 + rnd(12);
		if (!rn2(5)) monstcnt += rnd(5);
		if (!rn2(25)) monstcnt += rnd(10);
		if (!rn2(125)) monstcnt += rnd(20);
		if (!rn2(725)) monstcnt += rnd(50);

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

	      while(--monstcnt >= 0) {
		(void) makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
		}

		u.aggravation = 0;

		pline("e Pale Wraith with a lightning stroke.");

		deltrap(trap); /* only triggers once */
		break;

	    case IMMOBILITY_TRAP:

		monstcnt = 8;
		while (rn2(10)) monstcnt++;

		{
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

		}

		u.aggravation = 0;

		pline("vailable, exit anyway?");

		deltrap(trap); /* only triggers once */
		break;

	    case BOSS_TRAP:

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		{
			int attempts = 0;
			register struct permonst *ptrZ;
newboss:
			do {

				ptrZ = rndmonst();
				attempts++;
				if (attempts && (attempts % 10000 == 0)) u.mondiffhack++;
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
			u.mondiffhack = 0;

		}

		u.aggravation = 0;

		pline("A boss monster suddenly appears!");

		deltrap(trap); /* only triggers once */
		break;

	    case GIRLINESS_TRAP:

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		{
			int attempts = 0;
			int ammount = rnd(9);
			register struct permonst *ptrZ;
newgirl:
			do {

				ptrZ = rndmonst();
				attempts++;
				if (attempts && (attempts % 10000 == 0)) u.mondiffhack++;
				if (!rn2(2000)) reset_rndmonst(NON_PM);

			} while ( (!ptrZ || (ptrZ && !(ptrZ->msound == MS_FART_LOUD || ptrZ->msound == MS_FART_NORMAL || ptrZ->msound == MS_FART_QUIET))) && attempts < 50000);

			if (ptrZ && (ptrZ->msound == MS_FART_LOUD || ptrZ->msound == MS_FART_NORMAL || ptrZ->msound == MS_FART_QUIET) ) {
				(void) makemon(ptrZ, u.ux, u.uy, NO_MM_FLAGS);

				u.mondiffhack = 0;

				if (ammount > 0) {
					ammount--;
					attempts = 0;
					goto newgirl;
				}
			}
			else if (rn2(50)) {
				attempts = 0;
				goto newgirl;
			}

		}

		u.mondiffhack = 0;
		u.aggravation = 0;

		You_feel("a very feminine aura!");

		deltrap(trap); /* only triggers once */
		break;

	    case EGOMONSTER_TRAP:

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		{
			int attempts = 0;
			int ammount = rnd(9);
			register struct permonst *ptrZ;
newegomon:
			do {

				ptrZ = rndmonst();
				attempts++;
				if (attempts && (attempts % 10000 == 0)) u.mondiffhack++;
				if (!rn2(2000)) reset_rndmonst(NON_PM);

			} while ( (!ptrZ || (ptrZ && !(always_egotype(ptrZ)))) && attempts < 50000);

			if (ptrZ && (always_egotype(ptrZ)) ) {
				(void) makemon(ptrZ, u.ux, u.uy, NO_MM_FLAGS);

				u.mondiffhack = 0;

				if (ammount > 0) {
					ammount--;
					attempts = 0;
					goto newegomon;
				}
			}
			else if (rn2(50)) {
				attempts = 0;
				goto newegomon;
			}

		}

		u.mondiffhack = 0;
		u.aggravation = 0;

		pline("hwer!");

		deltrap(trap); /* only triggers once */
		break;

	    case FLOODING_TRAP:

		pline("Uh-oh, should have watched your step...");
		{
			int madepoolQ = 0;

			do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_megafloodingd, (void *)&madepoolQ);
			if (madepoolQ)
				pline("The dungeon is flooded!");
			else pline("The trap doesn't seem to have any effect.");

		}
		deltrap(trap); /* only triggers once */
		break;

	    case WISHING_TRAP:

		deltrap(trap); /* only triggers once, and before giving the wish to make sure you can't hangup cheat :P */
		pline("You stepped on a trap of wishing!");
		if ((Luck+rn2(5) < 0) && !RngeWishImprovement && !(uarmc && itemhasappearance(uarmc, APP_WISHFUL_CLOAK)) ) {
			makenonworkingwish();
			break;
		}
		makewish(TRUE);
		break;

	    case BOON_TRAP:

		deltrap(trap); /* only triggers once, and before giving the wish to make sure you can't hangup cheat :P */
		pline("You stepped on a boon trap!");

		if (!rn2(4)) makewish(evilfriday ? FALSE : TRUE);
		else othergreateffect();

		break;

	    case FOUNTAIN_TRAP:
		seetrap(trap);
		pline("You trip over a strange fountain!");

		if (u.ualign.type == A_NEUTRAL) adjalign(1);

		switch (rnd(42)) {
			case 1:
			case 2:
			case 3:
				break;
			case 4:
				if (!Unchanging) {
					You_feel("a change coming over you.");
					if (moves < 1000) u.polyprotected = TRUE;
					polyself(FALSE);
					u.polyprotected = FALSE;
				}
				break;

			case 5:
				if (!rn2(10)) pline("The water is bad, because of course it is!");
				else pline("The water is bad!");
				if (!rn2(10)) pline("Reminder: do not quaff from fountains!");
				badeffect();
				break;

			case 6:
				pline("Something comes out of the fountain!");

				int aggroamount = rnd(6);
				if (isfriday) aggroamount *= 2;
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
				while (aggroamount) {

					makemon((struct permonst *)0, u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
					aggroamount--;
					if (aggroamount < 0) aggroamount = 0;
				}
				u.aggravation = 0;

				break;

			case 7:

				if (rn2(10)) {
					pline("Ulch - the water was radioactive!");
					contaminate(rnd(10 + level_difficulty()), TRUE);
				} else {
					pline("Urrgh, this water must be extremely radioactive, and you wish you hadn't made the error of quaffing it.");
					contaminate(rnz(100 + (level_difficulty() * 5)), TRUE);
				}
				break;

			case 8:
			{
				int pm = rn2(5) ? dprince(rn2((int)A_LAWFUL+2) - 1) : dlord(rn2((int)A_LAWFUL+2) - 1);
				if (pm >= PM_ORCUS && pm <= PM_DEMOGORGON) u.conclusiocount++;
				if (pm && (pm != NON_PM)) {
					(void) makemon(&mons[pm], u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
					pline("An angry demon climbs out of the fountain...");
					if (!rn2(5)) {
						pline("Now you're probably dead. If you hadn't quaffed from the fountain, you might have lived.");
						if (!rn2(3)) pline("If you play really well now, you may still survive, but chances are you'll bash at the level 98 demon lord with your puny level %d char like a complete dorf and get all surprised when you (expectably) die.", u.ulevel);
					}
				}
			}
			break;

			case 9:
				pline_The("cool draught refreshes you.");
				u.uhunger += rnd(40); /* don't choke on water */
				newuhs(FALSE);
				break;
			case 10:
				You_feel("self-knowledgeable...");
				display_nhwindow(WIN_MESSAGE, FALSE);
				enlightenment(0, 1);
				exercise(A_WIS, TRUE);
				pline_The("feeling subsides.");
				break;
			case 11:
				pline_The("water is foul!  You gag and vomit.");
				morehungry(rn1(20, 11));
				vomit();
				break;
			case 12:
				pline_The("water is contaminated!");
				if (Poison_resistance && (StrongPoison_resistance || rn2(10)) ) {
				   pline("Perhaps it is runoff from the nearby %s farm.", fruitname(FALSE));
				   losehp(rnd(4),"unrefrigerated sip of juice", KILLED_BY_AN);
				   break;
				}
				losestr(rnd(4), TRUE);
				if (!rn2(20)) losestr(rnd(3), TRUE);
				losehp(rnd(10),"contaminated water", KILLED_BY);
				exercise(A_CON, FALSE);
				break;
			case 13:
				pline("An endless stream of snakes pours forth!");
				makemon(&mons[PM_RIVER_MOCCASIN], u.ux, u.uy, NO_MM_FLAGS);
				makemon(&mons[PM_RIVER_MOCCASIN], u.ux, u.uy, NO_MM_FLAGS);
				makemon(&mons[PM_RIVER_MOCCASIN], u.ux, u.uy, NO_MM_FLAGS);
				makemon(&mons[PM_RIVER_MOCCASIN], u.ux, u.uy, NO_MM_FLAGS);
				makemon(&mons[PM_RIVER_MOCCASIN], u.ux, u.uy, NO_MM_FLAGS);
				break;
			case 14:
				pline("You sense the presence of evil.");
				makemon(&mons[PM_WATER_DEMON], u.ux, u.uy, NO_MM_FLAGS);
				break;
			case 15: {
				register struct obj *crsobj;

				pline("This water's no good!");
				if (!rn2(5)) pline("Told ya that only noobs quaff from fountains.");
				morehungry(rn1(20, 11));
				exercise(A_CON, FALSE);
				for(crsobj = invent; crsobj ; crsobj = crsobj->nobj)
					if (!rn2(5) && !stack_too_big(crsobj))	curse(crsobj);
				}

				break;
			case 16:
				if (Blind) {
				    if (Invisible) {
					You_feel("transparent.");
				    } else {
				    	You_feel("very self-conscious.");
				    	pline("Then it passes.");
				    }
				} else {
				   You("see an image of someone stalking you.");
				   pline("But it disappears.");
				}
				HSee_invisible |= FROMOUTSIDE;
				newsym(u.ux,u.uy);
				exercise(A_WIS, TRUE);

				break;
			case 17:
				(void) monster_detect((struct obj *)0, 0);
				exercise(A_WIS, TRUE);

				break;
			case 18:
				You_feel("a gem here!");
				(void) mksobj_at(rnd_class(DILITHIUM_CRYSTAL, LUCKSTONE-1), u.ux, u.uy, TRUE, FALSE, FALSE);
				break;
			case 19:
				pline("A water nymph is summoned!");
				makemon(&mons[PM_WATER_NYMPH], u.ux, u.uy, NO_MM_FLAGS);
				break;
			case 20: {
				register struct monst *brmtmp;

				pline("This water gives you bad breath!");
				for(brmtmp = fmon; brmtmp; brmtmp = brmtmp->nmon)
				    if(!DEADMONSTER(brmtmp))
					monflee(brmtmp, rnd(10), FALSE, FALSE);
				}

				break;
			case 21: {
				int madepoolX = 0;
				do_clear_areaX(u.ux, u.uy, 5, do_floode, (void *)&madepoolX);
				pline("Water sprays all over the area.");
				}
				break;
			case 22:
				pline("A strange tingling runs up your %s.", body_part(ARM));
				break;
			case 23:
				You_feel("a sudden chill.");
				break;
			case 24:
				pline("An urge to take a bath overwhelms you.");
				break;
			case 25:
				gainlevelmaybe();
				break;
			case 26:
				if (!rn2(10)) {
					pline("Wow, this is healing water!");
					if (u.usanity > 0) {
						if (u.usanity > 50) reducesanity(u.usanity / 2);
						else reducesanity(u.usanity);
					}
				} else {
					You("cannot remember quaffing from the fountain.");
					if (FunnyHallu) You("also cannot remember who Maud is supposed to be.");
					forget(1 + rn2(5), FALSE);
				}
				break;

			case 27:

				if (!rn2(10)) {
					if (!u.badfcursed) {
						u.badfcursed = rnz(10000);
						You("start a trip on the road to nowhere.");
					} else {
						u.badfcursed += rnz(10000);
						u.badfdoomed += rnz(10000);
						if (u.badfcursed < u.badfdoomed) u.badfcursed += rnz(10000);
						if (u.badfcursed < u.badfdoomed) u.badfcursed = (u.badfdoomed * 2);
						You("continue a trip on the road to nowhere...");
					}
					break;
				}

				pline("This tepid water is tasteless.");

				u.uhunger += rnd(5); /* don't choke on water */
				newuhs(FALSE);
				break;
			case 28:
				if (!rn2(5)) {
					forget(rnd(20), FALSE);
					You_feel("a loss of memory. Maybe you shouldn't have quaffed from the fountain.");
					if (!rn2(10)) pline("In fact, maybe you shouldn't be quaffing from fountains at all if you want to live.");
				} else {
					int ulx, uly;
					for (ulx = 1; ulx < (COLNO); ulx++)
				        for (uly = 0; uly < (ROWNO); uly++) {
						levl[ulx][uly].lit = 0;
					}

					int heavyfountain = rnz(5000);
					make_hallucinated(HHallucination + heavyfountain,FALSE,0L);
					set_itimeout(&HeavyHallu, HHallucination);
					u.uprops[DEAC_HALLUC_RES].intrinsic += heavyfountain;
					pline("All the lights go out, in this disco hell...");
				}
				break;
			case 29:
				switch (rnd(7)) {

					case 1:
						make_inverted(rnz(5000));
						pline("Well uhh, seems you're walking upside down. Shoulda stayed away from the stupid fountain.");
						break;
					case 2:
						make_wincing(rnz(50000)); /* no, this number is not a typo --Amy */
						pline("A feeling of pain shoots into your body, and doesn't seem to be going away any time soon! Why did you quaff from a fountain in the first place? Some genius you are!");
						break;
					case 3:
						if (!inertiaprotection()) {
							u.inertia += rnz(1000);
							pline("E v e r y t h i n g   i s   m o v i n g   i n   s l o w   m o t i o n . . .   Y o u   g e t   t h e   f e e l i n g   t h a t   f o u n t a i n   q u a f f i n g   i s   b a d .");
						}
						break;
					case 4:
						incr_itimeout(&HMap_amnesia, rnz(100 * (monster_difficulty() + 1) ) );
						pline("Somehow, your memory is lost...");
						if (!rn2(5)) pline("An inner voice tells you that you should finally stop the bad habit of fountain quaffing or your characters will keep dying.");
						break;
					case 5:
						make_magicvacuum(rnz(5000));
						pline("A dark void enters your %s. Whatever was in the water, it wasn't healthy, and if your intelligence is above 3, you will now stop quaffing from fountains for the rest of your SLEX playing career.", body_part(HEAD));
						if (ACURR(A_INT) < 4) pline("Sadly, you seem to be very stupid though, so you'll continue quaffing and wonder why your chars keep dying.");
						break;
					case 6:
						make_burdened(rnz(5000));
						pline("Now you're burdened because you didn't get the memo that you *do not quaff from fountains*. Your fault.");
						break;
					case 7:
						if(!(HFuckOverEffect & FROMOUTSIDE)) {
							pline("Yeah, now you're being fucked over. Your fault, you shouldn't be quaffing from fountains, you noob.");
							HFuckOverEffect |= FROMOUTSIDE;
						} else pline("Seems you got lucky this time, but you should still stop quaffing from fountains, you noob.");
						break;
				}
				break;

			case 30:
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
			default:
				pline("This tepid water is tasteless.");
				u.uhunger += rnd(5); /* don't choke on water */
				newuhs(FALSE);
				break;

		}
		if (trap && !rn2(6)) {
			deltrap(trap);
			pline("The fountain dries up!");
		}

		break;

	    case THRONE_TRAP:
		seetrap(trap);
		pline("You trip over a strange throne!");

		if (u.ualign.type == A_CHAOTIC) adjalign(1);

		if (rn2(2)) {
			if (is_prince(youmonst.data))
			    You_feel("very comfortable here.");
			else
			    You_feel("somehow out of place...");

		} else if (uarmg && uarmg->oartifact == ART_FUMBLEFINGERS_QUEST) {

			{register int cnt = rnd(10);
			int randmonstforspawn = rnd(68);
			if (randmonstforspawn == 35) randmonstforspawn = 53;

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			while(cnt--)
			    (void) makemon(mkclass(randmonstforspawn,0), u.ux, u.uy, NO_MM_FLAGS);

			u.aggravation = 0;

			pline("A voice echoes:");
			verbalize("Oh, please help me! A horrible %s stole my sword! I'm nothing without it.", monexplain[randmonstforspawn]);
			}

		} else if (uarmg && uarmg->oartifact == ART_PRINCESS_BITCH) {

			{register int cnt = rnd(10);
			struct permonst *randmonstforspawn = rndmonst();

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			while(cnt--)
			    (void) makemon(randmonstforspawn, u.ux, u.uy, NO_MM_FLAGS);

			u.aggravation = 0;

			pline("A voice echoes:");
			verbalize("Leave me alone, stupid %s", randmonstforspawn->mname);
			}

		} else switch (rnd(32)) {

		    case 1:
			(void) adjattrib(rn2(A_MAX), -rno(5), FALSE, TRUE);
			losehp(rnd(10), "cursed throne", KILLED_BY_AN);
			break;
		    case 2:
			(void) adjattrib(rn2(A_MAX), 1, FALSE, TRUE);
			break;
		    case 3:
			pline("A%s electric shock shoots through your body!",
			      (Shock_resistance) ? "n" : " massive");
			if (!ShockImmunity) losehp(StrongShock_resistance ? rnd(2) : Shock_resistance ? rnd(6) : rnd(30),
			       "electric chair", KILLED_BY_AN);
			exercise(A_CON, FALSE);
			break;
		    case 4:
			You_feel("much, much better!");
			if (Upolyd) {
			    if (u.mh >= (u.mhmax - 5))  u.mhmax += 4;
			    u.mh = u.mhmax;
			}
			if(u.uhp >= (u.uhpmax - 5))  u.uhpmax += 4;
			u.uhp = u.uhpmax;
			if (uinsymbiosis) {
				u.usymbiote.mhpmax += 4;
				maybe_evolve_symbiote();
				if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
			}
			make_blinded(0L,TRUE);
			make_sick(0L, (char *) 0, FALSE, SICK_ALL);
			heal_legs();
			flags.botl = 1;
			break;
		    case 5:
			take_gold();
			break;
		    case 6:

			You_feel("your luck is changing.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Kha, vy ne poluchite zhelaniye, potomu chto eto Sovetskaya Rossiya, gde kazhdyy poluchayet odinakovoye kolichestvo zhelaniy! I vy uzhe boleye chem dostatochno, teper' ochered' drugikh personazhey'!" : "DSCHUEueUEueUEueUEueUEue...");
			change_luck(5);

			break;
		    case 7:
			{
			register int cnt = rnd(10);

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			pline("A voice echoes:");
			verbalize("Thy audience hath been summoned, %s!",
				  flags.female ? "Dame" : "Sire");
			while(cnt--)
			    (void) makemon(courtmon(), u.ux, u.uy, NO_MM_FLAGS);

			u.aggravation = 0;

			break;
			}
		    case 8:
			pline("A voice echoes:");
			verbalize("By thy Imperious order, %s...",
				  flags.female ? "Dame" : "Sire");
			do_genocide(5);	/* REALLY|ONTHRONE, see do_genocide() */
			break;
		    case 9:
			pline("A voice echoes:");
			verbalize("A curse upon thee for sitting upon this most holy throne!");
			if (Luck > 0)  {
			    make_blinded(Blinded + rn1(100,250),TRUE);
			} else	    rndcurse();
			break;
		    case 10:
			if (Luck < 0 || (HSee_invisible & INTRINSIC))  {
				if (level.flags.nommap) {
					pline(
					"A terrible drone fills your head!");
					make_confused(HConfusion + rnd(30),
									FALSE);
				} else {
					pline("An image forms in your mind.");
					do_mapping();
				}
			} else  {
				Your("vision becomes clear.");
				HSee_invisible |= FROMOUTSIDE;
				newsym(u.ux, u.uy);
			}
			break;
		    case 11:
			if (Luck < 0)  {
			    You_feel("threatened.");
			    aggravate();
			} else  {

			    You_feel("a wrenching sensation.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Tam net nikakoy zashchity. Tam net nikakoy nadezhdy. Yedinstvennoye, chto yest'? Uverennost' v tom, chto vy, igrok, budet umeret' uzhasnoy i muchitel'noy smert'yu." : "SCHRING!");
			    tele();		/* teleport him */
			}
			break;
		    case 12:
			You("are granted an insight!");
			if (invent) {
			    /* rn2(5) agrees w/seffects() */
			    identify_pack(rn2(5), 0, 0);
			}
			break;
		    case 13:
			Your("mind turns into a pretzel!");
			make_confused(HConfusion + rn1(7,16),FALSE);
			break;
		    case 14:
			You("are granted some new skills!"); /* new effect that unrestricts skills --Amy */
			unrestrictskillchoice();

			break;
		    case 15:
			pline("A voice echoes:");
			verbalize("Thou be cursed!");
			attrcurse();
			break;
		    case 16:
			pline("A voice echoes:");
			verbalize("Thou shall be punished!");
			punishx();
			break;
		    case 17:
			You_feel("like someone has touched your forehead...");

			int skillimprove = randomgoodskill();

			if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
				unrestrict_weapon_skill(skillimprove);
				pline("You can now learn the %s skill.", wpskillname(skillimprove));
			} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
				unrestrict_weapon_skill(skillimprove);
				P_MAX_SKILL(skillimprove) = P_BASIC;
				pline("You can now learn the %s skill.", wpskillname(skillimprove));
			} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
				P_MAX_SKILL(skillimprove) = P_SKILLED;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
				P_MAX_SKILL(skillimprove) = P_EXPERT;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
				P_MAX_SKILL(skillimprove) = P_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
				P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(200) && P_MAX_SKILL(skillimprove) == P_GRAND_MASTER) {
				P_MAX_SKILL(skillimprove) = P_SUPREME_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else gainlevelmaybe();

			if (Race_if(PM_RUSMOT)) {
				if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
					unrestrict_weapon_skill(skillimprove);
					pline("You can now learn the %s skill.", wpskillname(skillimprove));
				} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
					unrestrict_weapon_skill(skillimprove);
					P_MAX_SKILL(skillimprove) = P_BASIC;
					pline("You can now learn the %s skill.", wpskillname(skillimprove));
				} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
					P_MAX_SKILL(skillimprove) = P_SKILLED;
					pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
				} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
					P_MAX_SKILL(skillimprove) = P_EXPERT;
					pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
				} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
					P_MAX_SKILL(skillimprove) = P_MASTER;
					pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
				} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
					P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
					pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
				} else if (!rn2(200) && P_MAX_SKILL(skillimprove) == P_GRAND_MASTER) {
					P_MAX_SKILL(skillimprove) = P_SUPREME_MASTER;
					pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
				}
			}

			gainlevelmaybe();

			break;
		    case 18:
			{register int cnt = rnd(10);
			struct permonst *randmonstforspawn = rndmonst();

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			while(cnt--)
			    (void) makemon(randmonstforspawn, u.ux, u.uy, NO_MM_FLAGS);

			u.aggravation = 0;

			pline("A voice echoes:");
			verbalize("Leave me alone, stupid %s", randmonstforspawn->mname);
			break;
			}
		    case 19:
			{register int cnt = rnd(10);
			int randmonstforspawn = rnd(68);
			if (randmonstforspawn == 35) randmonstforspawn = 53;

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			while(cnt--)
			    (void) makemon(mkclass(randmonstforspawn,0), u.ux, u.uy, NO_MM_FLAGS);

			u.aggravation = 0;

			pline("A voice echoes:");
			verbalize("Oh, please help me! A horrible %s stole my sword! I'm nothing without it.", monexplain[randmonstforspawn]);
			break;
			}
		    case 20:
			{
			register struct obj *identotmp;
			if (CannotSelectItemsInPrompts) break;
			pline("You may fully identify an object!");

secureidchoice:
			identotmp = getobj(allnoncount, "secure identify");

			if (!identotmp) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to secure identify your objects.");
				else goto secureidchoice;
				pline("A feeling of loss comes over you.");
				break;
			}
			if (identotmp) {
				makeknown(identotmp->otyp);
				if (identotmp->oartifact) discover_artifact((int)identotmp->oartifact);
				identotmp->known = identotmp->dknown = identotmp->bknown = identotmp->rknown = 1;
				if (identotmp->otyp == EGG && identotmp->corpsenm != NON_PM)
				learn_egg_type(identotmp->corpsenm);
				prinv((char *)0, identotmp, 0L);
			}
			}
			break;
		    case 21:
			{
				int nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
				if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
				int blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
				if (!blackngdur ) blackngdur = 500; /* fail safe */
				randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), (blackngdur - (monster_difficulty() * 3)));
				You_feel("uncomfortable.");
			}
			break;
		    case 22:
			morehungry(500);
			pline("Whoops... suddenly you feel hungry.");
			break;
		    case 23:
			pline("Suddenly you feel a healing touch!");
			reducesanity(100);
			break;
		    case 24:
			poisoned("throne", rn2(6) /* A_STR ... A_CHA*/, "poisoned throne", 30);
			break;
		    case 25:
			{
				int thronegold = rnd(200);
				u.ugold += thronegold;
				pline("Some coins come loose! You pick up %d zorkmids.", thronegold);
			}
			break;
		    case 26:
			{

				if (Aggravate_monster) {
					u.aggravation = 1;
					reset_rndmonst(NON_PM);
				}

				pline("A voice echoes:");
				verbalize("Thou hath been summoned to appear before royalty, %s!", playeraliasname);
				(void) makemon(specialtensmon(rn2(2) ? 105 : 106), u.ux, u.uy, MM_ANGRY); /* M2_LORD, M2_PRINCE */

				u.aggravation = 0;

			}
			break;
		    case 27:
			badeffect();
			break;
		    case 28:
			u.uhp++;
			u.uhpmax++;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			You_feel("a health boost!");
			break;
		    case 29:
			pline("A pretty ethereal woman appears and offers: 'For only 10000 zorkmids, I will give you a very rare trinket!");
			if (u.ugold < 10000) {
				pline("But you don't have enough money! Frustrated, she places a terrible curse on you and disappears.");
				randomfeminismtrap(rnz( (level_difficulty() + 2) * rnd(50)));
			}
			else {
				char femhandlebuf[BUFSZ];
				getlin ("Do you want to buy her goods? [y/yes/no]",femhandlebuf);
				(void) lcase (femhandlebuf);
				if (!(strcmp (femhandlebuf, "yes")) || !(strcmp (femhandlebuf, "y")) || !(strcmp (femhandlebuf, "ye")) || !(strcmp (femhandlebuf, "ys"))) {
					u.ugold -= 10000;
					register struct obj *acqo;
					acqo = mksobj(makegreatitem(), TRUE, TRUE, FALSE);
					if (acqo) {
						dropy(acqo);
						verbalize("Thanks a lot! You'll find your prize on the ground.");
					} else {
						verbalize("Oh sorry, I must have misplaced it. Here you have your money back. Maybe next time I'll have something for you.");
						u.ugold += 10000;
					}
				} else {
					verbalize("You will regret that decision!");
					randomfeminismtrap(rnz( (level_difficulty() + 2) * rnd(50)));

				}
			}
			break;
		    case 30:
			pline("A shady merchant appears and offers: 'Sale! Sale! I'm selling you this useful item for 2000 zorkmids!");
			if (u.ugold < 2000) {
				pline("But you don't have enough money! The merchant disappears.");
			}
			else {
				char femhandlebuf[BUFSZ];
				getlin ("Do you want to buy his item? [y/yes/no]",femhandlebuf);
				(void) lcase (femhandlebuf);
				if (!(strcmp (femhandlebuf, "yes")) || !(strcmp (femhandlebuf, "y")) || !(strcmp (femhandlebuf, "ye")) || !(strcmp (femhandlebuf, "ys"))) {
					u.ugold -= 2000;
					register struct obj *acqo;
					acqo = mksobj(usefulitem(), TRUE, TRUE, FALSE);
					if (acqo) {
						dropy(acqo);
						verbalize("Thank you! I've dropped the item at your feet.");
					} else {
						verbalize("Nyah-nyah, thanks for the money, sucker!");
					}
				} else {
					verbalize("Are you sure? Well, it's your decision. I'll find someone else to sell it to, then.");
				}
			}
			break;
		    case 31:
			{
				struct obj *stupidstone;
				stupidstone = mksobj_at(rnd_class(RIGHT_MOUSE_BUTTON_STONE,NASTY_STONE), u.ux, u.uy, TRUE, FALSE, FALSE);
				if (stupidstone) {
					stupidstone->quan = 1L;
					stupidstone->owt = weight(stupidstone);
					if (!Blind) stupidstone->dknown = 1;
					if (stupidstone) {
						pline("%s lands in your knapsack!", Doname2(stupidstone));
						(void) pickup_object(stupidstone, 1L, TRUE, TRUE);
					}
				}
			}
			break;
		    case 32:
			(void) mksobj_at(rnd_class(DILITHIUM_CRYSTAL, ROCK), u.ux, u.uy, TRUE, TRUE, FALSE);
			pline("Some stones come loose!");
			break;

		}

		if (trap && !rn2(6)) {
			deltrap(trap);
			pline("The throne vanishes in a puff of logic.");
		}

		break;

	    case MAP_AMNESIA_TRAP:

		pline("You stepped on a trigger!");
		seetrap(trap);

		forget_map(ALL_MAP);
		docrt();

		pline("Suddenly, you realize that you forgot to think about Maud.");
		if (FunnyHallu) pline("You also forgot that you were suffering from amnesia.");

		break;

	    case ARTIFACT_JACKPOT_TRAP:

		deltrap(trap); /* only triggers once, and before giving the artifact to make sure you can't hangup cheat :P */
		pline("It's your lucky day - you triggered an artifact jackpot trap!");

		{

		register struct obj *acqo;

		boolean havegifts = u.ugifts;

		if (!havegifts) u.ugifts++;

		acqo = mk_artifact((struct obj *)0, !rn2(3) ? A_CHAOTIC : rn2(2) ? A_NEUTRAL : A_LAWFUL, TRUE);
		if (acqo) {

		switch (acqo->oclass) {
			case WEAPON_CLASS:

				{

					int wpntype; /* 1 = launcher, 2 = ammo, 3 = melee */
					if (is_launcher(acqo)) wpntype = 1;
					else if (is_ammo(acqo) || is_missile(acqo)) wpntype = 2;
					else wpntype = 3;
reroll:
					acqo->otyp = rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1);
					if (wpntype == 1 && !is_launcher(acqo)) goto reroll;
					if (wpntype == 2 && !is_ammo(acqo) && !is_missile(acqo)) goto reroll;
					if (wpntype == 3 && (is_launcher(acqo) || is_ammo(acqo) || is_missile(acqo))) goto reroll;
				}

				break;
			case ARMOR_CLASS:

				{

					int armortype;
					/* 1 = shield, 2 = helmet, 3 = boots, 4 = gloves, 5 = cloak, 6 = shirt, 7 = suit */
					if (is_shield(acqo)) armortype = 1;
					else if (is_helmet(acqo)) armortype = 2;
					else if (is_boots(acqo)) armortype = 3;
					else if (is_gloves(acqo)) armortype = 4;
					else if (is_cloak(acqo)) armortype = 5;
					else if (is_shirt(acqo)) armortype = 6;
					else armortype = 7;
rerollX:
					acqo->otyp = rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS);
					if (armortype == 1 && !is_shield(acqo)) goto rerollX;
					if (armortype == 2 && !is_helmet(acqo)) goto rerollX;
					if (armortype == 3 && !is_boots(acqo)) goto rerollX;
					if (armortype == 4 && !is_gloves(acqo)) goto rerollX;
					if (armortype == 5 && !is_cloak(acqo)) goto rerollX;
					if (armortype == 6 && !is_shirt(acqo)) goto rerollX;
					if (armortype == 7 && !is_suit(acqo)) goto rerollX;

				}
				break;
			case RING_CLASS:
				acqo->otyp = rnd_class(RIN_ADORNMENT,RIN_TELEPORT_CONTROL);
				break;
			case AMULET_CLASS:
				acqo->otyp = rnd_class(AMULET_OF_CHANGE,AMULET_OF_VULNERABILITY);
				break;
			case IMPLANT_CLASS:
				acqo->otyp = rnd_class(IMPLANT_OF_ABSORPTION,IMPLANT_OF_VIRTUE);
				break;
		}

		    dropy(acqo);

			int acqoskill = get_obj_skill(acqo, TRUE);

			if (P_MAX_SKILL(acqoskill) == P_ISRESTRICTED) {
			    unrestrict_weapon_skill(acqoskill);
			} else if (P_MAX_SKILL(acqoskill) == P_UNSKILLED) {
				unrestrict_weapon_skill(acqoskill);
				P_MAX_SKILL(acqoskill) = P_BASIC;
			} else if (rn2(2) && P_MAX_SKILL(acqoskill) == P_BASIC) {
				P_MAX_SKILL(acqoskill) = P_SKILLED;
			} else if (!rn2(4) && P_MAX_SKILL(acqoskill) == P_SKILLED) {
				P_MAX_SKILL(acqoskill) = P_EXPERT;
			} else if (!rn2(10) && P_MAX_SKILL(acqoskill) == P_EXPERT) {
				P_MAX_SKILL(acqoskill) = P_MASTER;
			} else if (!rn2(100) && P_MAX_SKILL(acqoskill) == P_MASTER) {
				P_MAX_SKILL(acqoskill) = P_GRAND_MASTER;
			} else if (!rn2(200) && P_MAX_SKILL(acqoskill) == P_GRAND_MASTER) {
				P_MAX_SKILL(acqoskill) = P_SUPREME_MASTER;
			}
			if (Race_if(PM_RUSMOT)) {
				if (P_MAX_SKILL(acqoskill) == P_ISRESTRICTED) {
				    unrestrict_weapon_skill(acqoskill);
				} else if (P_MAX_SKILL(acqoskill) == P_UNSKILLED) {
					unrestrict_weapon_skill(acqoskill);
					P_MAX_SKILL(acqoskill) = P_BASIC;
				} else if (rn2(2) && P_MAX_SKILL(acqoskill) == P_BASIC) {
					P_MAX_SKILL(acqoskill) = P_SKILLED;
				} else if (!rn2(4) && P_MAX_SKILL(acqoskill) == P_SKILLED) {
					P_MAX_SKILL(acqoskill) = P_EXPERT;
				} else if (!rn2(10) && P_MAX_SKILL(acqoskill) == P_EXPERT) {
					P_MAX_SKILL(acqoskill) = P_MASTER;
				} else if (!rn2(100) && P_MAX_SKILL(acqoskill) == P_MASTER) {
					P_MAX_SKILL(acqoskill) = P_GRAND_MASTER;
				} else if (!rn2(200) && P_MAX_SKILL(acqoskill) == P_GRAND_MASTER) {
					P_MAX_SKILL(acqoskill) = P_SUPREME_MASTER;
				}
			}

		    discover_artifact(acqo->oartifact);

			if (!havegifts) u.ugifts--;
			pline("An artifact appeared beneath you!");
		}
		else pline("Opportunity knocked, but nobody was home.  Bummer.");

		}

		break;

	    case GOOD_ARTIFACT_TRAP:

		deltrap(trap); /* only triggers once, and before giving the artifact to make sure you can't hangup cheat :P */
		pline("You feel very lucky, because you triggered a good artifact trap!");

		{

		register struct obj *acqo;

		boolean havegifts = u.ugifts;

		if (!havegifts) u.ugifts++;

		acqo = mk_artifact((struct obj *)0, !rn2(3) ? A_CHAOTIC : rn2(2) ? A_NEUTRAL : A_LAWFUL, TRUE);
		if (acqo) {

		    dropy(acqo);

			int acqoskill = get_obj_skill(acqo, TRUE);

			if (P_MAX_SKILL(acqoskill) == P_ISRESTRICTED) {
			    unrestrict_weapon_skill(acqoskill);
			} else if (P_MAX_SKILL(acqoskill) == P_UNSKILLED) {
				unrestrict_weapon_skill(acqoskill);
				P_MAX_SKILL(acqoskill) = P_BASIC;
			} else if (rn2(2) && P_MAX_SKILL(acqoskill) == P_BASIC) {
				P_MAX_SKILL(acqoskill) = P_SKILLED;
			} else if (!rn2(4) && P_MAX_SKILL(acqoskill) == P_SKILLED) {
				P_MAX_SKILL(acqoskill) = P_EXPERT;
			} else if (!rn2(10) && P_MAX_SKILL(acqoskill) == P_EXPERT) {
				P_MAX_SKILL(acqoskill) = P_MASTER;
			} else if (!rn2(100) && P_MAX_SKILL(acqoskill) == P_MASTER) {
				P_MAX_SKILL(acqoskill) = P_GRAND_MASTER;
			} else if (!rn2(200) && P_MAX_SKILL(acqoskill) == P_GRAND_MASTER) {
				P_MAX_SKILL(acqoskill) = P_SUPREME_MASTER;
			}
			if (Race_if(PM_RUSMOT)) {
				if (P_MAX_SKILL(acqoskill) == P_ISRESTRICTED) {
				    unrestrict_weapon_skill(acqoskill);
				} else if (P_MAX_SKILL(acqoskill) == P_UNSKILLED) {
					unrestrict_weapon_skill(acqoskill);
					P_MAX_SKILL(acqoskill) = P_BASIC;
				} else if (rn2(2) && P_MAX_SKILL(acqoskill) == P_BASIC) {
					P_MAX_SKILL(acqoskill) = P_SKILLED;
				} else if (!rn2(4) && P_MAX_SKILL(acqoskill) == P_SKILLED) {
					P_MAX_SKILL(acqoskill) = P_EXPERT;
				} else if (!rn2(10) && P_MAX_SKILL(acqoskill) == P_EXPERT) {
					P_MAX_SKILL(acqoskill) = P_MASTER;
				} else if (!rn2(100) && P_MAX_SKILL(acqoskill) == P_MASTER) {
					P_MAX_SKILL(acqoskill) = P_GRAND_MASTER;
				} else if (!rn2(200) && P_MAX_SKILL(acqoskill) == P_GRAND_MASTER) {
					P_MAX_SKILL(acqoskill) = P_SUPREME_MASTER;
				}
			}

		    discover_artifact(acqo->oartifact);

			if (!havegifts) u.ugifts--;
			pline("An artifact appeared beneath you!");
		}
		else pline("Opportunity knocked, but nobody was home.  Bummer.");

		}

		break;

	    case GROWING_TRAP:

		pline("Uh-oh, should have watched your step...");
		{
			int madepoolQ = 0;

			do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_treefloodd, (void *)&madepoolQ);
			if (madepoolQ)
				pline("Suddenly, greenery grows around you!");
			else pline("The trap doesn't seem to have any effect.");

		}
		deltrap(trap); /* only triggers once */

		break;

	    case COOLING_TRAP:

		pline("Uh-oh, should have watched your step...");
		{
			int madepoolQ = 0;

			do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_icefloodd, (void *)&madepoolQ);
			if (madepoolQ)
				pline("Suddenly, the area around you gets very icy!");
			else pline("The trap doesn't seem to have any effect.");

		}
		deltrap(trap); /* only triggers once */

		break;

	    case BAR_TRAP:

		pline("Uh-oh, should have watched your step...");
		{
			int madepoolQ = 0;

			do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_barfloodd, (void *)&madepoolQ);
			if (madepoolQ)
				pline("Suddenly, iron bars shoot out of the ground all around you!");
			else pline("The trap doesn't seem to have any effect.");

		}
		deltrap(trap); /* only triggers once */

		break;

	    case LOCKING_TRAP:

		pline("Uh-oh, should have watched your step...");
		{
			int madepoolQ = 0;

			do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_lockfloodd, (void *)&madepoolQ);
			if (madepoolQ)
				pline("Suddenly, you hear grating noises and the locking of doors!");
			else pline("The trap doesn't seem to have any effect.");

		}
		deltrap(trap); /* only triggers once */

		break;

	    case AIR_TRAP:

		pline("Uh-oh, should have watched your step...");
		{
			int madepoolQ = 0;

			do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_cloudfloodd, (void *)&madepoolQ);
			if (madepoolQ)
				pline("Suddenly, clouds are obscuring your vision!");
			else pline("The trap doesn't seem to have any effect.");

		}
		deltrap(trap); /* only triggers once */

		break;

	    case TERRAIN_TRAP:

		pline("Uh-oh, should have watched your step...");
		{
			int madepoolQ = 0;

			do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_terrainfloodd, (void *)&madepoolQ);
			if (madepoolQ)
				pline("Suddenly, the entire dungeon around you seems to slightly change its structure!");
			else pline("The trap doesn't seem to have any effect.");

		}
		deltrap(trap); /* only triggers once */

		break;

	    case GATEWAY_FROM_HELL: /* evil patch idea by jonadab */

		monstcnt = d(3, 5);	
		if (!rn2(5)) monstcnt += rnd(3);
		if (!rn2(25)) monstcnt += rnd(7);
		if (!rn2(125)) monstcnt += rnd(15);
		if (!rn2(725)) monstcnt += rnd(50);

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

	      while(--monstcnt >= 0) {
		(void) makemon(mkclass(S_DEMON,0), u.ux, u.uy, NO_MM_FLAGS);
		}

		u.aggravation = 0;

		pline(FunnyHallu ? "And she's buying a stairway to heaven... er, hell." : "From the dark stairway to hell, demons appear to surround you!");

		deltrap(trap); /* only triggers once */
		break;

	    case ELEMENTAL_PORTAL:

		monstcnt = d(3, 5);	
		if (!rn2(5)) monstcnt += rnd(3);
		if (!rn2(25)) monstcnt += rnd(7);
		if (!rn2(125)) monstcnt += rnd(15);
		if (!rn2(725)) monstcnt += rnd(50);

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

	      while(--monstcnt >= 0) {
		(void) makemon(mkclass(S_ELEMENTAL,0), u.ux, u.uy, NO_MM_FLAGS);
		}

		u.aggravation = 0;

		pline(FunnyHallu ? "Alien alert! I will kick you in the god damn ass! Shit these alien creeps are hot!" : "The barriers between the dungeon and the elemental planes have been breached!");

		deltrap(trap); /* only triggers once */
		break;

	    case LAVA_TRAP:

		pline("Uh-oh, should have watched your step...");
		pline("The thin crust of rock gives way, revealing lava underneath!");

		deltrap(trap); /* only triggers once */

		int madepool = 0;

		do_clear_areaX(u.ux, u.uy, 5, do_lavafloode, (void *)&madepool);

		if (levl[u.ux][u.uy].typ == ROOM || levl[u.ux][u.uy].typ == CORR || levl[u.ux][u.uy].typ == ICE)
			levl[u.ux][u.uy].typ = LAVAPOOL; /* don't fall in, that would be too evil. --Amy */

		break;

	    case FLOOD_TRAP:

		pline("Uh-oh, should have watched your step...");
		pline("The thin crust of rock gives way, revealing water underneath!");

		deltrap(trap); /* only triggers once */

		int madepoolX = 0;

		do_clear_areaX(u.ux, u.uy, 5, do_floode, (void *)&madepoolX);

		if (levl[u.ux][u.uy].typ == ROOM || levl[u.ux][u.uy].typ == CORR || levl[u.ux][u.uy].typ == ICE) {
			levl[u.ux][u.uy].typ = POOL;
			if (!Wwalking && !Race_if(PM_KORONST) && !Flying && !Levitation && !(uarmf && uarmf->oartifact == ART_RUBBER_LOVE) && !(uarmc && itemhasappearance(uarmc, APP_FLIER_CLOAK)) ) drown();
		}
		break;

		case GRAVE_WALL_TRAP:

		pline("Uh-oh, should have watched your step...");
		pline("Hans Walt erects grave walls!");

		deltrap(trap); /* only triggers once */

		int madepoolX2 = 0;

		do_clear_areaX(u.ux, u.uy, 5, do_gravefloode, (void *)&madepoolX2);

		break;
		case TUNNEL_TRAP:

		pline("Uh-oh, should have watched your step...");
		pline("It seems that construction workers have carved tunnels through the rock.");

		deltrap(trap); /* only triggers once */

		int madepoolX3 = 0;

		do_clear_areaX(u.ux, u.uy, 5, do_tunnelfloode, (void *)&madepoolX3);

		break;
		case FARMLAND_TRAP:

		pline("Uh-oh, should have watched your step...");
		pline("All the local farmers are claiming land on this dungeon level.");

		deltrap(trap); /* only triggers once */

		int madepoolX4 = 0;

		do_clear_areaX(u.ux, u.uy, 5, do_farmfloode, (void *)&madepoolX4);

		break;
		case MOUNTAIN_TRAP:

		pline("Uh-oh, should have watched your step...");
		pline("Strange... the underground dungeon seems to become mountainous.");

		deltrap(trap); /* only triggers once */

		int madepoolX5 = 0;

		do_clear_areaX(u.ux, u.uy, 5, do_mountainfloode, (void *)&madepoolX5);

		break;
		case WATER_TUNNEL_TRAP:

		pline("Uh-oh, should have watched your step...");
		pline("Watery tunnels are erected!");

		deltrap(trap); /* only triggers once */

		int madepoolX6 = 0;

		do_clear_areaX(u.ux, u.uy, 5, do_watertunnelfloode, (void *)&madepoolX6);

		break;
		case CRYSTAL_FLOOD_TRAP:

		pline("Uh-oh, should have watched your step...");
		pline("Crystallized water appears on the ceiling.");

		deltrap(trap); /* only triggers once */

		int madepoolX7 = 0;

		do_clear_areaX(u.ux, u.uy, 5, do_crystalwaterfloode, (void *)&madepoolX7);

		break;
		case MOORLAND_TRAP:

		pline("Uh-oh, should have watched your step...");
		pline("The dungeon gets swampy.");

		deltrap(trap); /* only triggers once */

		int madepoolX8 = 0;

		do_clear_areaX(u.ux, u.uy, 5, do_moorfloode, (void *)&madepoolX8);

		break;
		case URINE_TRAP:

		pline("Uh-oh, should have watched your step...");
		pline("Mira pees all over the dungeon.");

		deltrap(trap); /* only triggers once */

		int madepoolX9 = 0;

		do_clear_areaX(u.ux, u.uy, 5, do_urinefloode, (void *)&madepoolX9);

		break;
		case SHIFTING_SAND_TRAP:

		pline("Uh-oh, should have watched your step...");
		pline("Deadly sandholes appear.");

		deltrap(trap); /* only triggers once */

		int madepoolX10 = 0;

		do_clear_areaX(u.ux, u.uy, 5, do_shiftingsandfloode, (void *)&madepoolX10);

		break;
		case STYX_TRAP:

		pline("Uh-oh, should have watched your step...");
		pline("Hellish green water flows into the dungeon!");

		deltrap(trap); /* only triggers once */

		int madepoolX11 = 0;

		do_clear_areaX(u.ux, u.uy, 5, do_styxfloode, (void *)&madepoolX11);

		break;
		case PENTAGRAM_TRAP:

			if (u.ualign.type == A_CHAOTIC) adjalign(1);

			pline("You trip over a strange pentagram!");
			seetrap(trap);

			if (rn2(3)) {
				pline("Your mana increases.");
				u.uenmax++;
			} else switch (rnd(29)) {

				case 1:
					HTeleport_control += 2;
					tele();
					break;
				case 2:
					{

					register struct obj *acqo;

					acqo = mkobj_at(SPBOOK_CLASS, u.ux, u.uy, FALSE, FALSE);
					if (acqo) {
						acqo->bknown = acqo->known = TRUE;
						pline("A book appeared at your %s!", makeplural(body_part(FOOT)));
					} else {
						pline("Nothing happens...");
						if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
							pline("Oh wait, actually something bad happens...");
							badeffect();
						}
					  }
					}
					break;
				case 3:
					(void) monster_detect((struct obj *)0, 0);
					exercise(A_WIS, TRUE);
					break;
				case 4:
					trap_detect((struct obj *)0);
					break;
				case 5:
					object_detect((struct obj *)0, 0);
					break;
				case 6:
					{
					boolean havegifts = u.ugifts;

					if (!havegifts) u.ugifts++;

					register struct obj *acqo;

					acqo = mk_artifact((struct obj *)0, !rn2(3) ? A_CHAOTIC : rn2(2) ? A_NEUTRAL : A_LAWFUL, TRUE);
					if (acqo) {
					    dropy(acqo);
						int acqoskill = get_obj_skill(acqo, TRUE);

						if (P_MAX_SKILL(acqoskill) == P_ISRESTRICTED) {
						    unrestrict_weapon_skill(acqoskill);
						} else if (P_MAX_SKILL(acqoskill) == P_UNSKILLED) {
							unrestrict_weapon_skill(acqoskill);
							P_MAX_SKILL(acqoskill) = P_BASIC;
						} else if (rn2(2) && P_MAX_SKILL(acqoskill) == P_BASIC) {
							P_MAX_SKILL(acqoskill) = P_SKILLED;
						} else if (!rn2(4) && P_MAX_SKILL(acqoskill) == P_SKILLED) {
							P_MAX_SKILL(acqoskill) = P_EXPERT;
						} else if (!rn2(10) && P_MAX_SKILL(acqoskill) == P_EXPERT) {
							P_MAX_SKILL(acqoskill) = P_MASTER;
						} else if (!rn2(100) && P_MAX_SKILL(acqoskill) == P_MASTER) {
							P_MAX_SKILL(acqoskill) = P_GRAND_MASTER;
						} else if (!rn2(200) && P_MAX_SKILL(acqoskill) == P_GRAND_MASTER) {
							P_MAX_SKILL(acqoskill) = P_SUPREME_MASTER;
						}

						if (Race_if(PM_RUSMOT)) {
							if (P_MAX_SKILL(acqoskill) == P_ISRESTRICTED) {
							    unrestrict_weapon_skill(acqoskill);
							} else if (P_MAX_SKILL(acqoskill) == P_UNSKILLED) {
								unrestrict_weapon_skill(acqoskill);
								P_MAX_SKILL(acqoskill) = P_BASIC;
							} else if (rn2(2) && P_MAX_SKILL(acqoskill) == P_BASIC) {
								P_MAX_SKILL(acqoskill) = P_SKILLED;
							} else if (!rn2(4) && P_MAX_SKILL(acqoskill) == P_SKILLED) {
								P_MAX_SKILL(acqoskill) = P_EXPERT;
							} else if (!rn2(10) && P_MAX_SKILL(acqoskill) == P_EXPERT) {
								P_MAX_SKILL(acqoskill) = P_MASTER;
							} else if (!rn2(100) && P_MAX_SKILL(acqoskill) == P_MASTER) {
								P_MAX_SKILL(acqoskill) = P_GRAND_MASTER;
							} else if (!rn2(200) && P_MAX_SKILL(acqoskill) == P_GRAND_MASTER) {
								P_MAX_SKILL(acqoskill) = P_SUPREME_MASTER;
							}
						}

					    discover_artifact(acqo->oartifact);

						if (!havegifts) u.ugifts--;
						pline("An artifact appeared beneath you!");

					}	

					else pline("Opportunity knocked, but nobody was home.  Bummer.");

					}

					break;
				case 7:
					pline("The RNG decides to curse-weld an item to you.");
					bad_artifact_xtra();
					break;
				case 8:
					{
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

					}

					break;
				case 9:
					pline("Your body suddenly becomes all stiff!");
					if (isstunfish) nomul(-rnz(15), "paralyzed by a pentagram", TRUE);
					else nomul(-rnd(15), "paralyzed by a pentagram", TRUE);
					break;
				case 10:

					pline("The dungeon is getting more chaotic!");
					{
					int madepoolPEP = 0;
					do_clear_areaX(u.ux, u.uy, 12, do_terrainfloodd, (void *)&madepoolPEP);
					}

					break;
				case 11:
					You_feel("powered up!");
					u.uenmax += rnd(5);
					u.uen = u.uenmax;
					break;
				case 12:
					pline("Suddenly, you gain a new companion!");
					(void) make_familiar((struct obj *)0, u.ux, u.uy, FALSE, FALSE);
					break;
				case 13:
					{

					if (Aggravate_monster) {
						u.aggravation = 1;
						reset_rndmonst(NON_PM);
					}

					    coord dd;
					    coord cc;
					    int cx,cy;
						int i;
						int randsp, randmnst, randmnsx;
						struct permonst *randmonstforspawn;
						int monstercolor;

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

					pline("You suddenly feel a surge of tension!");

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

					You_feel("the arrival of monsters!");

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

					You_feel("a colorful sensation!");

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
					monstercolor = rnd(434);

					You_feel("that a group has arrived!");

					for (i = 0; i < randsp; i++) {

						if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

						(void) makemon(specialtensmon(monstercolor), cx, cy, MM_ADJACENTOK);
					}

					}

					u.aggravation = 0;

					}
					break;
				case 14:

					if (u.uhunger < 1500) {
						pline("Your %s fills.", body_part(STOMACH));
						u.uhunger = 1500;
						u.uhs = 1; /* NOT_HUNGRY */
						flags.botl = 1;
					} else {
						pline("Nothing happens...");
						if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
							pline("Oh wait, actually something bad happens...");
							badeffect();
						}
					}
					break;
				case 15:
					if (u.ualign.record < -1) {
						adjalign(-(u.ualign.record / 2));
						You_feel("partially absolved.");
					} else {
						u.alignlim++;
						adjalign(10);
						You_feel("appropriately %s.", align_str(u.ualign.type));
					}
					break;
				case 16:
					{

					int nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
					if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
					int blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
					if (!blackngdur ) blackngdur = 500; /* fail safe */

					pline("Your mana increases.");
					u.uenmax++;
					/* nasty trap effect - no extra message because, well, nastiness! --Amy */
					randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), (blackngdur - (monster_difficulty() * 3)));

					}
					break;
				case 17:
					{
					int i = rn2(A_MAX);
					adjattrib(i, 1, 0, TRUE);
					adjattrib(i, 1, 0, TRUE);
					adjattrib(i, 1, 0, TRUE);
					adjattrib(i, 1, 0, TRUE);
					adjattrib(i, 1, 0, TRUE);
					}
					break;
				case 18:
					Your("intrinsics change.");
					intrinsicgainorloss(0);
					break;
				case 19:
					{
					struct obj *pseudo;
					pseudo = mksobj(SCR_ITEM_GENOCIDE, FALSE, 2, FALSE);
					if (!pseudo) {
						pline("Nothing happens...");
						if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
							pline("Oh wait, actually something bad happens...");
							badeffect();
						}
						break;
					}
					if (pseudo->otyp == GOLD_PIECE) pseudo->otyp = SCR_ITEM_GENOCIDE;
					(void) seffects(pseudo);
					obfree(pseudo, (struct obj *)0);	/* now, get rid of it */

					}

					break;
				case 20:
					doubleskilltraining();
					break;
				case 21:
					if (!(HAggravate_monster & INTRINSIC) && !(HAggravate_monster & TIMEOUT)) {

						int maxtrainingamount = 0;
						int skillnumber = 0;
						int actualskillselection = 0;
						int amountofpossibleskills = 1;
						int i;

						for (i = 0; i < P_NUM_SKILLS; i++) {
							if (P_SKILL(i) != P_ISRESTRICTED) continue;

							if (P_ADVANCE(i) > 0 && P_ADVANCE(i) >= maxtrainingamount) {
								if (P_ADVANCE(i) > maxtrainingamount) {
									amountofpossibleskills = 1;
									skillnumber = i;
									maxtrainingamount = P_ADVANCE(i);
								} else if (!rn2(amountofpossibleskills + 1)) {
									amountofpossibleskills++;
									skillnumber = i;
								} else {
									amountofpossibleskills++;
								}
							}
						}

						if (skillnumber > 0 && maxtrainingamount > 0) {
							unrestrict_weapon_skill(skillnumber);

							register int maxcap = P_BASIC;
							if (!rn2(2)) {
								maxcap = P_SKILLED;
								if (!rn2(2)) {
									maxcap = P_EXPERT;
									if (maxtrainingamount >= 20 && !rn2(2)) {
										maxcap = P_MASTER;
										if (maxtrainingamount >= 160 && !rn2(2)) {
											maxcap = P_GRAND_MASTER;
											if (maxtrainingamount >= 540 && !rn2(2)) {
												maxcap = P_SUPREME_MASTER;
											}
										}
									}
								}
							}

							P_MAX_SKILL(skillnumber) = maxcap;
							pline("You can now learn the %s skill, with a new cap of %s.", wpskillname(skillnumber), maxcap == P_SUPREME_MASTER ? "supreme master" : maxcap == P_GRAND_MASTER ? "grand master" : maxcap == P_MASTER ? "master" : maxcap == P_EXPERT ? "expert" : maxcap == P_SKILLED ? "skilled" : "basic");
						} else {
							pline("Nothing happens...");
							if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
								pline("Oh wait, actually something bad happens...");
								badeffect();
							}
						}

					}

					if (HAggravate_monster & INTRINSIC) {
						HAggravate_monster &= ~INTRINSIC;
						You_feel("more acceptable!");
					}
					if (HAggravate_monster & TIMEOUT) {
						HAggravate_monster &= ~TIMEOUT;
						You_feel("more acceptable!");
					}
					break;
				case 22:
					{
						u.aggravation = 1;
						reset_rndmonst(NON_PM);
						int attempts = 0;
						register struct permonst *ptrZ;
newbossPENT:
					do {

						ptrZ = rndmonst();
						attempts++;
						if (attempts && (attempts % 10000 == 0)) u.mondiffhack++;
						if (!rn2(2000)) reset_rndmonst(NON_PM);

					} while ( (!ptrZ || (ptrZ && !(ptrZ->geno & G_UNIQ))) && attempts < 50000);

					if (ptrZ && ptrZ->geno & G_UNIQ) {
						if (wizard) pline("monster generation: %s", ptrZ->mname);
						(void) makemon(ptrZ, u.ux, u.uy, MM_ANGRY);
					}
					else if (rn2(50)) {
						attempts = 0;
						goto newbossPENT;
					}

					u.mondiffhack = 0;

					if (!rn2(10) ) {
						attempts = 0;
						goto newbossPENT;
					}
					pline("Boss monsters appear from nowhere!");

					}
					u.aggravation = 0;

					break;
				case 23:
					if (!rn2(6400)) {
						ragnarok(TRUE);
						if (evilfriday) evilragnarok(TRUE,level_difficulty());

					}

					u.aggravation = 1;
					u.heavyaggravation = 1;
					DifficultyIncreased += 1;
					HighlevelStatus += 1;
					EntireLevelMode += 1;

					(void) makemon(mkclass(S_NEMESE,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);

					u.aggravation = 0;
					u.heavyaggravation = 0;

					break;
				case 24:
					wonderspell(-1);
					break;
				case 25:

					{
					int tryct = 0;
					int x, y;
					register struct trap *ttmp;
					boolean canbeinawall = FALSE;
					if (!rn2(Passes_walls ? 5 : 25)) canbeinawall = TRUE;
					for (tryct = 0; tryct < 2000; tryct++) {
						x = rn1(COLNO-3,2);
						y = rn2(ROWNO);

						if (isok(x, y) && ((levl[x][y].typ > DBWALL) || canbeinawall) && !(t_at(x, y)) ) {
								ttmp = maketrap(x, y, rndtrap(), 0, TRUE);
							if (ttmp) {
								ttmp->tseen = 0;
								ttmp->hiddentrap = 1;
							}
							if (!rn2(5)) break;
						}
					}

					You_feel("in grave danger...");
					}
					break;
				case 26:
					badeffect();
					break;
				case 27:
					if (!uinsymbiosis) {
						getrandomsymbiote(FALSE, FALSE);
						pline("Suddenly you have a symbiote!");
					} else {
						u.usymbiote.mhpmax += rnd(10);
						maybe_evolve_symbiote();
						if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
						flags.botl = TRUE;
						Your("symbiote seems much stronger now.");
					}
					break;
				case 28:
					decontaminate(100);
					You_feel("decontaminated.");
					break;
				case 29:
					pline("Wow!  This makes you feel good!");
					{
					int i, ii, lim;
					i = rn2(A_MAX);
					for (ii = 0; ii < A_MAX; ii++) {
						lim = AMAX(i);
						if (i == A_STR && u.uhs >= 3) --lim;	/* WEAK */
						if (ABASE(i) < lim) {
							ABASE(i) = lim;
							flags.botl = 1;
							break;
						}
						if(++i >= A_MAX) i = 0;
					}

					}
					break;
				default:
					impossible("undefined pentagram effect");
					break;

			}

			if (trap && !rn2(6)) {
				deltrap(trap);
				pline_The("pentagram fades away completely.");
			}


		break;
		case SNOW_TRAP:

		pline("Uh-oh, should have watched your step...");
		pline("It starts snowing!");

		deltrap(trap); /* only triggers once */

		int madepoolX12 = 0;

		do_clear_areaX(u.ux, u.uy, 5, do_snowfloode, (void *)&madepoolX12);

		break;
		case ASH_TRAP:

		pline("Uh-oh, should have watched your step...");
		pline("The floor becomes red.");

		deltrap(trap); /* only triggers once */

		int madepoolX13 = 0;

		do_clear_areaX(u.ux, u.uy, 5, do_ashfloode, (void *)&madepoolX13);

		break;
		case SAND_TRAP:

		pline("Uh-oh, should have watched your step...");
		pline("Soft sand appears in the dungeon.");

		deltrap(trap); /* only triggers once */

		int madepoolX14 = 0;

		do_clear_areaX(u.ux, u.uy, 5, do_sandfloode, (void *)&madepoolX14);

		break;
		case PAVEMENT_TRAP:

		pline("Uh-oh, should have watched your step...");
		pline("Suddenly there are paved roads everywhere!");

		deltrap(trap); /* only triggers once */

		int madepoolX15 = 0;

		do_clear_areaX(u.ux, u.uy, 5, do_pavementfloode, (void *)&madepoolX15);

		break;
		case HIGHWAY_TRAP:

		pline("Uh-oh, should have watched your step...");
		pline("Highways are being built all over the dungeon!");

		deltrap(trap); /* only triggers once */

		int madepoolX16 = 0;

		do_clear_areaX(u.ux, u.uy, 5, do_highwayfloode, (void *)&madepoolX16);

		break;
		case GRASSLAND_TRAP:

		pline("Uh-oh, should have watched your step...");
		pline("You feel the grass grow.");

		deltrap(trap); /* only triggers once */

		int madepoolX17 = 0;

		do_clear_areaX(u.ux, u.uy, 5, do_grassfloode, (void *)&madepoolX17);

		break;
		case NETHER_MIST_TRAP:

		pline("Uh-oh, should have watched your step...");
		pline("Purple mist appears.");

		deltrap(trap); /* only triggers once */

		int madepoolX18 = 0;

		do_clear_areaX(u.ux, u.uy, 5, do_nethermistfloode, (void *)&madepoolX18);

		break;
		case STALACTITE_TRAP:

		pline("Uh-oh, should have watched your step...");
		pline("Stalactites shoot out of the ceiling!");

		deltrap(trap); /* only triggers once */

		int madepoolX19 = 0;

		do_clear_areaX(u.ux, u.uy, 5, do_stalactitefloode, (void *)&madepoolX19);

		break;
		case CRYPTFLOOR_TRAP:

		pline("Uh-oh, should have watched your step...");
		pline("Seems you're in the crypt now.");

		deltrap(trap); /* only triggers once */

		int madepoolX20 = 0;

		do_clear_areaX(u.ux, u.uy, 5, do_cryptfloode, (void *)&madepoolX20);

		break;
		case BUBBLE_TRAP:

		pline("Uh-oh, should have watched your step...");
		pline("Floating bubbles appear!");

		deltrap(trap); /* only triggers once */

		int madepoolX21 = 0;

		do_clear_areaX(u.ux, u.uy, 5, do_bubblefloode, (void *)&madepoolX21);

		break;
		case RAIN_CLOUD_TRAP:

		pline("Uh-oh, should have watched your step...");
		pline("It starts to rain.");

		deltrap(trap); /* only triggers once */

		int madepoolX22 = 0;

		do_clear_areaX(u.ux, u.uy, 5, do_raincloudfloode, (void *)&madepoolX22);

		break;

		case ITEM_NASTIFICATION_TRAP:

			deltrap(trap); /* only triggers once */
			pline("CLICK! You have triggered a trap!");
			nastytrapcurse();

		break;
		case SANITY_INCREASE_TRAP:
			seetrap(trap);
			pline("You stepped on a trigger!");
			increasesanity(rnz((monster_difficulty() * 5) + 1));
		break;
		case PSI_TRAP:

			seetrap(trap);
			pline("You stepped on a trigger!");

			if (Psi_resist && rn2(StrongPsi_resist ? 100 : 20)) {
				pline("But you resist the effects.");
				break;
			}
			if (obsidianprotection()) {
				pline("But you resist the effects.");
				break;
			}

			pline("Your mind is blasted by psionic energy.");

			switch (rnd(10)) {

				case 1:
				case 2:
				case 3:
					make_confused(HConfusion + rnd(10) + rnd(monster_difficulty()), FALSE);
					break;
				case 4:
				case 5:
				case 6:
					make_stunned(HStun + rnd(10) + rnd(monster_difficulty()), FALSE);
					break;
				case 7:
					make_confused(HConfusion + rnd(10) + rnd(monster_difficulty()), FALSE);
					make_stunned(HStun + rnd(10) + rnd(monster_difficulty()), FALSE);
					break;
				case 8:
					make_hallucinated(HHallucination + rnd(10) + rnd(monster_difficulty()), FALSE, 0L);
					break;
				case 9:
					make_feared(HFeared + rnd(10) + rnd(monster_difficulty()), FALSE);
					break;
				case 10:
					make_numbed(HNumbed + rnd(10) + rnd(monster_difficulty()), FALSE);
					break;
	
			}
			if (!rn2(200)) {
				forget(rnd(5), FALSE);
				pline("You forget some important things...");
			}
			if (!rn2(200) && (u.urmaxlvlUP >= 2) ) {
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

		break;
		case GAY_TRAP: /* by Demo */
			deltrap(trap); /* only triggers once */
			pline("CLICK! You have triggered a trap!");
			u.homosexual = rn2(3);
			if (u.homosexual == 0) pline("You forget your sexual orientation!");
			else if (u.homosexual == 1) pline("Your sexual orientation is straight now!");
			else pline("Your sexual orientation is homosexual now!");

		break;

		case SARAH_TRAP:

			if (FemaleTrapSarah) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Sarah.");
				pline("You can already imagine the farting noises you're gonna hear.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapSarah = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapSarah += 100;
			if (!rn2(3)) FemaleTrapSarah += rnz(500);

		break;
		case CLAUDIA_TRAP:

			if (FemaleTrapClaudia) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Claudia.");
				pline("Suddenly you feel a little confused, and also feel like stroking the sexy butt cheeks of a woman in wooden sandals.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapClaudia = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapClaudia += 100;
			if (!rn2(3)) FemaleTrapClaudia += rnz(500);

		break;
		case LUDGERA_TRAP:

			if (FemaleTrapLudgera) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Ludgera.");
				pline("You'll certainly like to listen to the disgusting toilet noises.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapLudgera = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapLudgera += 100;
			if (!rn2(3)) FemaleTrapLudgera += rnz(500);

		break;
		case KATI_TRAP:

			if (FemaleTrapKati) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Kati.");
				pline("You feel like being kicked by sexy girls and cleaning their shoes.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapKati = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapKati += 100;
			if (!rn2(3)) FemaleTrapKati += rnz(500);

		break;

	    case QUICKSAND_TRAP:

		if ((Levitation || Flying) && !(SoiltypeEffect || u.uprops[SOILTYPE].extrinsic || have_soiltypestone() || (uarmf && uarmf->oartifact == ART_ARABELLA_S_GIRL_KICK)) ) {

			if (rn2(10)) break;
			else pline("You are pulled downwards...");

		}
		else pline("Uh-oh, should have watched your step...");
		pline("Suddenly, you're in quicksand! You frantically try to get out.");

		/*
		Unbreathing doesn't help; once you're completely buried there's nothing you can do.
		Teleportitis doesn't help either; the quicksand prevents you from doing teleport incantations.
		And if anyone still finds something that should protect you I'll make up similar reasons for it. :-) --Amy
		*/

		seetrap(trap);

		int quicksanddamage;
		quicksanddamage = 1;

		while (rn2(2 + monster_difficulty() ) ) {

			quicksanddamage++;

			if (quicksanddamage == 5) pline("You still try pulling yourself out...");
			if (quicksanddamage == 10) pline("Damn, this is some nasty stuff!");
			if (quicksanddamage == 20) pline("It just keeps pulling you down! Help!");
			if (quicksanddamage == 50) pline("Your whole body is stuck and being pulled underneath!");
			if (quicksanddamage == 100) pline("Uh-oh, this might be the end...");

		}

		losehp(quicksanddamage,"being pulled into quicksand",KILLED_BY);

		pline("Wheeeew! At last you managed to pull yourself out of the quicksand.");

		break;

	    case ITEM_TELEP_TRAP:
		seetrap(trap);
		pline("A vivid purple glow surrounds you...");

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

		break;

	    case STONE_TO_FLESH_TRAP:

		pline("You are momentarily illuminated by a flash of light!");

		seetrap(trap);

		    {
		    struct obj *otemp, *onext;
		    struct obj *pseudo;
		    boolean didmerge;

			/* pseudo is a temporary "false" object containing the spell stats. */
			pseudo = mksobj(SPE_STONE_TO_FLESH, FALSE, 2, FALSE);
			if (!pseudo) break;
			if (pseudo->otyp == GOLD_PIECE) pseudo->otyp = SPE_STONE_TO_FLESH; /* minimalist fix */
			pseudo->blessed = pseudo->cursed = 0;
			pseudo->quan = 20L;			/* do not let useup get it */

		    if (u.umonnum == PM_STONE_GOLEM)
			(void) polymon(PM_FLESHY_GOLEM);
		    if (Stoned) fix_petrification();	/* saved! */
		    /* but at a cost.. */
		    for (otemp = invent; otemp; otemp = onext) {
			onext = otemp->nobj;
			if (!rn2(10)) (void) bhito(otemp, pseudo);
			}
		    /*
		     * It is possible that we can now merge some inventory.
		     * Do a higly paranoid merge.  Restart from the beginning
		     * until no merges.
		     */
		    do {
			didmerge = FALSE;
			for (otemp = invent; !didmerge && otemp; otemp = otemp->nobj)
			    for (onext = otemp->nobj; onext; onext = onext->nobj)
			    	if (merged(&otemp, &onext)) {
			    		didmerge = TRUE;
			    		break;
			    		}
		    } while (didmerge);

			obfree(pseudo, (struct obj *)0);	/* now, get rid of it */

		    }

		break;

	    case GENDER_TRAP:

		deltrap(trap);
		pline("CLICK! You have triggered a trap!");
		change_sex();
		You("don't feel like yourself.");

		break;

	    case WRENCHING_TRAP:

		seetrap(trap);
		pline("Uh-oh, should have watched your step...");

		switch (rnd(8)) {

			case 1:
				You_feel("a wrenching sensation.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Tam net nikakoy zashchity. Tam net nikakoy nadezhdy. Yedinstvennoye, chto yest'? Uverennost' v tom, chto vy, igrok, budet umeret' uzhasnoy i muchitel'noy smert'yu." : "SCHRING!");
				tele();		/* teleport him */

				break;
			case 2:
				You_feel("threatened.");
				aggravate();
				break;
			case 3:
				make_blinded(Blinded + rnz(75),TRUE);
				break;
			case 4:
				take_gold();
				break;
			case 5:
				pline("Pondering why you are running around in a dungeon looking for an amulet was just too much to comprehend.");
				make_confused(HConfusion + rnz(75),FALSE);
				break;
			case 6:

				pline("Poison washes over you!");
				if (uarmg) {
				    if (uarmg->oerodeproof || (Race_if(PM_CHIQUAI) && rn2(4)) || (uarmg->oartifact && rn2(4)) || (objects[uarmg->otyp].oc_material == MT_GREEN_STEEL && rn2(2)) || !is_corrodeable(uarmg)) {
					Your("gloves seem unaffected.");
				    } else if (uarmg->oeroded2 < MAX_ERODE) {
					if (uarmg->greased) {
					    grease_protect(uarmg, "gloves", &youmonst);
					} else if (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ) {
					    Your("gloves corrode%s!",
						 uarmg->oeroded2+1 == MAX_ERODE ?
						 " completely" : uarmg->oeroded2 ?
						 " further" : "");
					    uarmg->oeroded2++;
					}
				    } else
					Your("gloves %s completely corroded.",
					     Blind ? "feel" : "look");
				}
				losestr(StrongPoison_resistance ? 1 : Poison_resistance ? rno(3) : rnd(5), TRUE);
				losehp(rnd(Poison_resistance ? 6 : 10), "wrenching poison", KILLED_BY_AN);

				break;
			case 7:
				{
				int projectiledamage = 2*rnd(10) + 5;
				if (projectiledamage > 1) {
					if (u.ulevel == 1) projectiledamage /= 2;
					else if (u.ulevel == 2) {
						projectiledamage *= 2;
						projectiledamage /= 3;
					} else if (u.ulevel == 3) {
						projectiledamage *= 3;
						projectiledamage /= 4;
					} else if (u.ulevel == 4) {
						projectiledamage *= 4;
						projectiledamage /= 5;
					}
				}
				pline("Something explodes in your %s!", body_part(FACE));
				losehp(projectiledamage, "exploding rune", KILLED_BY_AN);
				}

				break;
			case 8:
				rndcurse();
				break;

		}

		break;

	    case NURSE_TRAP:
		deltrap(trap);
		pline("A feeling of benevolence washes over you, and your health increases!");

		if (Upolyd) {
			u.mhmax += u.ulevel;
			u.mh = u.mhmax;
		} else {
			if (u.uhpmax < (u.ulevel * 10)) u.uhpmax += u.ulevel;
			else u.uhpmax++;
			u.uhp = u.uhpmax;
		}
		if (uinsymbiosis) {
			u.usymbiote.mhpmax++;
			maybe_evolve_symbiote();
			if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
		}

		break;

	    case TRACKER_TRAP:

		deltrap(trap);
		pline("You stepped on a trigger!");

		{
			register struct monst *nexusmon;
			boolean teleportdone = FALSE;

			if ((level.flags.noteleport || Race_if(PM_STABILISATOR) || u.antitelespelltimeout) && !Race_if(PM_RODNEYAN) ) {
				pline("A mysterious force prevents you from teleporting!");
				break;
			}

			if ( ((u.uhave.amulet && !u.freeplaymode && (u.amuletcompletelyimbued || !rn2(3))) || CannotTeleport || On_W_tower_level(&u.uz) || (u.usteed && mon_has_amulet(u.usteed)) ) ) {
				You_feel("disoriented for a moment.");
				break;
			}

			for(nexusmon = fmon; nexusmon; nexusmon = nexusmon->nmon) {
				if (nexusmon) break;
			}

			if (!nexusmon) {
				pline("But nothing happens...");
				break;
			}

			int ii, ij, bd = 1;

			for(ii = -bd; ii <= bd; ii++) for(ij = -bd; ij <= bd; ij++) {
				if (!isok(nexusmon->mx + ii, nexusmon->my + ij)) continue;

				if (teleok(nexusmon->mx + ii, nexusmon->my + ij, FALSE)) {
					teleportdone = TRUE;
					teleds(nexusmon->mx + ii, nexusmon->my + ij, FALSE);
					Norep("Suddenly your surroundings change.");
					break;
				}
			}
			if (!teleportdone) pline("But nothing happens...");

		}

		break;

	    case SINCOUNT_TRAP:

		seetrap(trap);
		pline("CLICK! You have triggered a trap!");

		u.ualign.sins++;
		u.alignlim--;
		adjalign(-5);

		You_feel("sinful...");
		if (FunnyHallu) pline("But why would you care?");

		break;

	    case TRAP_OF_OPPOSITE_ALIGNMENT:

		deltrap(trap);
		pline("CLICK! You have triggered a trap!");

		if (u.ualign.type == A_NEUTRAL)
			u.ualign.type = rn2(2) ? A_CHAOTIC : A_LAWFUL;
		else u.ualign.type = -(u.ualign.type);
		u.ublessed = 0; /* lose your god's protection */
		Your("mind oscillates briefly.");

		break;

	    case DRAIN_TRAP:

		seetrap(trap);
		pline("CLICK! You have triggered a trap!");

		if (moves < 1000 && (u.urmaxlvlUP == 1) && rn2(25)) {
			pline("But it didn't do anything.");
			break;
		}

		if (!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4) ) {
			pline("A malevolent black glow suddenly surrounds you...");
			losexp("life drainage", FALSE, TRUE);
		}
		else {
			pline("A dark black glow suddenly surrounds you.");
			pline("It doesn't seem to harm you though.");
		}

		break;

	    case DEATH_TRAP:

		seetrap(trap);
		pline("CLICK! You have triggered a trap!");

		if (!is_undead(youmonst.data) ) {
			pline("A definite, impenetrable black glow suddenly surrounds you...");

			if (!rn2(20) && !PlayerResistsDeathRays ) {
				u.youaredead = 1;
				killer_format = KILLED_BY_AN;
				killer = "instadeath trap";
				done(DIED);
				u.youaredead = 0;
			} else {

		      int dmgnum = 0;
		      dmgnum = d(2, 6) + rnd((monster_difficulty() * 2) + 1);
			if (Antimagic || (Half_spell_damage && rn2(2)) || (StrongHalf_spell_damage && rn2(2)) ) dmgnum /= 2;
			if (StrongAntimagic && dmgnum > 1) dmgnum /= 2;
			You_feel("drained...");
			u.uhpmax -= dmgnum/2;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			losehp(dmgnum,"death trap",KILLED_BY_AN);
			}

		}
		else {
			pline("You are surrounded by an apparently harmless black glow.");
		}

		break;

	    case PESTILENCE_TRAP:

		seetrap(trap);
		pline("CLICK! You have triggered a trap!");

		if (IntSick_resistance || (ExtSick_resistance && rn2(20)) ) {
			You_feel("fever and chills for a moment, but you seem unharmed.");
		} else {
			You_feel("fever and chills...");
			make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON), 40),
			"illness trap", TRUE, rn2(2) ? SICK_VOMITABLE : SICK_NONVOMITABLE);
		}

		break;

	    case FAMINE_TRAP:

		seetrap(trap);
		pline("CLICK! You have triggered a trap!");

		morehungry(rnz(40));

		pline("Your body shrivels.");

		morehungry(rnz(5 * (monster_difficulty() + 1)) );

		break;

	    case DESTROY_ARMOR_TRAP:

		seetrap(trap);
		if (!rn2(5)) deltrap(trap); /* CAUTION: "trap" might have been dereferenced now */
		pline("CLICK! You have triggered a trap!");

		{
			struct obj *otmpD;
			otmpD = some_armor(&youmonst);
			if(otmpD) {

				if (otmpD->blessed && rn2(5)) pline("Your body shakes violently!");
				else if ((otmpD->spe > 1) && (rn2(otmpD->spe)) ) pline("Your body shakes violently!");
				else if (otmpD->oartifact && rn2(20)) pline("Your body shakes violently!");
				else if (otmpD->greased) {
					pline("Your body shakes violently!");
					 if (!rn2(2) || (isfriday && !rn2(2))) {
						pline_The("grease wears off.");
						otmpD->greased -= 1;
						update_inventory();
					 }
				}
				else if(!destroy_arm(otmpD)) {
					pline("Your skin itches.");
				}
			} 
			else pline("Your skin itches.");
		}

		break;

	    case DIVINE_ANGER_TRAP:

		seetrap(trap);
		pline("CLICK! You have triggered a trap!");
		u.ugangr++;
	      You("get the feeling that %s is angry...", u_gname());

		break;

	    case SIN_TRAP:

		seetrap(trap);
		pline("CLICK! You have triggered a trap!");

		{
			int dmg = 0;

		switch (rnd(8)) {

			case 1: /* gluttony */
				u.negativeprotection++;
				if (evilfriday && u.ublessed > 0) {
					u.ublessed -= 1;
					if (u.ublessed < 0) u.ublessed = 0;
				}
				You_feel("less protected!");
				break;
			case 2: /* wrath */
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
			case 3: /* sloth */
				You_feel("a little apathetic...");

				switch(rn2(7)) {
				    case 0: /* destroy certain things */
					lethe_damage(invent, FALSE, FALSE);
					break;
				    case 1: /* sleep */
					if (multi >= 0) {
					    if (Sleep_resistance && rn2(StrongSleep_resistance ? 20 : 5)) {pline("You yawn."); break;}
					    fall_asleep(-rnd(10), TRUE);
					    You("are put to sleep!");
					}
					break;
				    case 2: /* paralyse */
					if (multi >= 0) {
					    if (Free_action && rn2(StrongFree_action ? 100 : 20)) {
						You("momentarily stiffen.");            
					    } else {
						You("are frozen!");
						if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
						nomovemsg = 0;	/* default: "you can move again" */
						if (isstunfish) nomul(-rnz(5), "paralyzed by a sin trap", TRUE);
						else nomul(-rnd(5), "paralyzed by a sin trap", TRUE);
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
			case 4: /* greed */
				if (u.ugold) pline("Your purse feels lighter...");
				u.ugold /= 2;
				break;
			case 5: /* lust */
				if (invent) {
					pline("Your belongings leave your body!");
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
				break;
			case 6: /* envy */
				if (flags.soundok) You_hear("a chuckling laughter.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Kha-kha-kha-kha-kha-KDZH KDZH, tip bloka l'da smeyetsya yego tortsa, potomu chto vy teryayete vse vashi vstroyennyye funktsii!" : "Hoehoehoehoe!");
			      attrcurse();
			      attrcurse();
				break;
			case 7: /* pride */
			      pline("The RNG determines to take you down a peg or two...");
				if (!rn2(3)) {
				    poisoned("air", rn2(A_MAX), "sin trap", 30);
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
					if((!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4)) && (u.urmaxlvlUP >= 2) )
					    losexp("life drainage", FALSE, TRUE);
					else You_feel("woozy for an instant, but shrug it off.");
				}
				break;
			case 8: /* depression */

			    switch(rnd(20)) {
			    case 1:
				if (!Unchanging && !Antimagic) {
					You("undergo a freakish metamorphosis!");
					if (moves < 1000) u.polyprotected = TRUE;
					polyself(FALSE);
					u.polyprotected = FALSE;
				}
				break;
			    case 2:
				You("need reboot.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Eto poshel na khuy vverkh. No chto zhe vy ozhidali? Igra, v kotoruyu vy mozhete legko vyigrat'? Durak!" : "DUEUEDUET!");
				if (!Race_if(PM_UNGENOMOLD)) newman();
				else {
					if (moves < 1000) u.polyprotected = TRUE;
					polyself(FALSE);
					u.polyprotected = FALSE;
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
					    You("are put to sleep!");
					}
				}
				break;
			    case 5: case 6:
				pline("Suddenly, there's glue all over you!");
				u.utraptype = TT_GLUE;
				u.utrap = 25 + rnd(monster_difficulty());

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
			    case 11:
			    case 12:
				if ((!StrongSwimming || !rn2(10)) && (!StrongMagical_breathing || !rn2(10))) {
					water_damage(invent, FALSE, FALSE);
				}
				break;
			    case 13:
				if (multi >= 0) {
				    if (Free_action && rn2(StrongFree_action ? 100 : 20)) {
					You("momentarily stiffen.");            
				    } else {
					You("are frozen!");
					if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
					nomovemsg = 0;	/* default: "you can move again" */
					if (isstunfish) nomul(-rnz(5), "paralyzed by a sin trap", TRUE);
					else nomul(-rnd(5), "paralyzed by a sin trap", TRUE);
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
				dmg = rn1(7, 16);
				make_stunned(HStun + dmg + monster_difficulty(), FALSE);
				(void) make_hallucinated(HHallucination + dmg + monster_difficulty(),TRUE,0L);
				break;
			    case 15:
				if(!Blind)
					Your("vision bugged.");
				dmg += rn1(10, 25);
				dmg += rn1(10, 25);
				(void) make_hallucinated(HHallucination + dmg + monster_difficulty() + monster_difficulty(),TRUE,0L);
				break;
			    case 16:
				if(!Blind)
					Your("vision turns to screen saver.");
				dmg += rn1(10, 25);
				(void) make_hallucinated(HHallucination + dmg + monster_difficulty(),TRUE,0L);
				break;
			    case 17:
				{
				    struct obj *objD = some_armor(&youmonst);
	
				    if (objD && drain_item(objD)) {
					Your("%s less effective.", aobjnam(objD, "seem"));
					u.cnd_disenchantamount++;
					if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
				    }
				}
				break;
			    default:
				    if(Confusion)
					 You("are getting even more confused.");
				    else You("are getting confused.");
				    make_confused(HConfusion + monster_difficulty() + 1, FALSE);
				break;
			    }

				break;

		}

		}

		break;

	    case DISINTEGRATION_TRAP:

		seetrap(trap);
		pline("CLICK! You have triggered a trap!");

		if (!Disint_resistance || !rn2(StrongDisint_resistance ? 1000 : 100) || (evilfriday && (uarms || uarmc || uarm || uarmu)) ) {
			You_feel("like you're falling apart!");

			if (uarms) {
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
			} else {
				if (u.uhpmax > 20) {
					u.uhpmax -= rnd(20);
					if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
					losehp(rnz(100 + level_difficulty()), "lol you died because you stepped on the wrong trap", KILLED_BY);
					break;
				} else {
					u.youaredead = 1;
					done(DIED);
					u.youaredead = 0;
				}
			}

		}
		else {
			pline("You seem unharmed.");
		}

		break;

	    case GLYPH_OF_WARDING:
			{
			pline("You trigger a hidden glyph!");
		    seetrap(trap);

		    int dmg = (d(4, 4) + rnd((monster_difficulty() / 2) + 1));
			int dmgtype = rnd(4);
			if (dmg > 1) {
				if (u.ulevel == 1) dmg /= 2;
				else if (u.ulevel == 2) {
					dmg *= 2;
					dmg /= 3;
				} else if (u.ulevel == 3) {
					dmg *= 3;
					dmg /= 4;
				} else if (u.ulevel == 4) {
					dmg *= 4;
					dmg /= 5;
				}
			}

			switch (dmgtype) {

			case 1: /* ordinary physical damage */
			if (dmg) {losehp(dmg, "hidden glyph", KILLED_BY_AN); }
				break;
			case 2: /* shock */
			if (Shock_resistance) dmg = 0;
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) /* new calculations --Amy */	destroy_item(RING_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) /* new calculations --Amy */	destroy_item(WAND_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 30 : 165)) /* new calculations --Amy */	destroy_item(AMULET_CLASS, AD_ELEC);
			if (dmg) {pline("You are hit by lightning!"); losehp(dmg, "lightning", KILLED_BY); }
			break;
			case 3: /* ice */
			if(Cold_resistance) dmg = 0;
			if (!rn2(10)) make_frozen(HFrozen + (dmg * 5), TRUE); /* randomly freeze the player --Amy */
		    if (isevilvariant || !rn2(issoviet ? 6 : Race_if(PM_GAVIL) ? 6 : Race_if(PM_HYPOTHERMIC) ? 6 : 33)) /* new calculations --Amy */  destroy_item(POTION_CLASS, AD_COLD);
			if (dmg) {pline("You are hit by cold!"); losehp(dmg, "cold", KILLED_BY); }
			break;
			case 4: /* fire */
			if (Race_if(PM_LOWER_ENT)) dmg *= 2;
			if(Fire_resistance) dmg = 0;
			if ( /* burnarmor(&youmonst) || */ isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33)) /* new calculation -- Amy */ {
			    destroy_item(SCROLL_CLASS, AD_FIRE);
			    destroy_item(SPBOOK_CLASS, AD_FIRE);
			    destroy_item(POTION_CLASS, AD_FIRE);
				burnarmor(&youmonst);
				}
			if (dmg) {pline("You are hit by fire!"); losehp(dmg, "fire", KILLED_BY); }
			break;

			}
		badeffect();
		}

		if (!rn2(10)) deltrap(trap);
		break;

	    case PURPLE_GLYPH:

		pline("You trigger a hidden glyph!");
		seetrap(trap);

		badeffect();
		badeffect();
		badeffect();
		badeffect();
		badeffect();
		badeffect();
		badeffect();

		if (!rn2(10)) deltrap(trap);
		break;

	    case BLACK_GLYPH:
			{
			pline("You trigger a hidden glyph!");
		    seetrap(trap);

		    int dmg = (d(4, 4) + rnd((monster_difficulty() * 3) + 1));
			int dmgtype = rnd(4);
			if (dmg > 1) {
				if (u.ulevel == 1) dmg /= 2;
				else if (u.ulevel == 2) {
					dmg *= 2;
					dmg /= 3;
				} else if (u.ulevel == 3) {
					dmg *= 3;
					dmg /= 4;
				} else if (u.ulevel == 4) {
					dmg *= 4;
					dmg /= 5;
				}
			}

			switch (dmgtype) {

			case 1: /* ordinary physical damage */
			if (dmg) {losehp(dmg, "hidden glyph", KILLED_BY_AN); }
				break;
			case 2: /* shock */
			if (Shock_resistance) dmg = 0;
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) /* new calculations --Amy */	destroy_item(RING_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) /* new calculations --Amy */	destroy_item(WAND_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 30 : 165)) /* new calculations --Amy */	destroy_item(AMULET_CLASS, AD_ELEC);
			if (dmg) {pline("You are hit by lightning!"); losehp(dmg, "lightning", KILLED_BY); }
			break;
			case 3: /* ice */
			if(Cold_resistance) dmg = 0;
			if (!rn2(10)) make_frozen(HFrozen + (dmg * 5), TRUE); /* randomly freeze the player --Amy */
		    if (isevilvariant || !rn2(issoviet ? 6 : Race_if(PM_GAVIL) ? 6 : Race_if(PM_HYPOTHERMIC) ? 6 : 33)) /* new calculations --Amy */  destroy_item(POTION_CLASS, AD_COLD);
			if (dmg) {pline("You are hit by cold!"); losehp(dmg, "cold", KILLED_BY); }
			break;
			case 4: /* fire */
			if (Race_if(PM_LOWER_ENT)) dmg *= 2;
			if(Fire_resistance) dmg = 0;
			if ( /* burnarmor(&youmonst) || */ isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33)) /* new calculation -- Amy */ {
			    destroy_item(SCROLL_CLASS, AD_FIRE);
			    destroy_item(SPBOOK_CLASS, AD_FIRE);
			    destroy_item(POTION_CLASS, AD_FIRE);
				burnarmor(&youmonst);
				}
			if (dmg) {pline("You are hit by fire!"); losehp(dmg, "fire", KILLED_BY); }
			break;

			}
		}
		if (!rn2(10)) deltrap(trap);
		break;

	    case ORANGE_GLYPH:
			{
			pline("You trigger a hidden glyph!");
		    seetrap(trap);

		    int dmg = (d(4, 4) + rnd((monster_difficulty() / 2) + 1));
			int dmgtype = rnd(4);
			if (dmg > 1) {
				if (u.ulevel == 1) dmg /= 2;
				else if (u.ulevel == 2) {
					dmg *= 2;
					dmg /= 3;
				} else if (u.ulevel == 3) {
					dmg *= 3;
					dmg /= 4;
				} else if (u.ulevel == 4) {
					dmg *= 4;
					dmg /= 5;
				}
			}

			switch (dmgtype) {

			case 1: /* ordinary physical damage */
			if (dmg) {losehp(dmg, "hidden glyph", KILLED_BY_AN); }
				break;
			case 2: /* shock */
			if (Shock_resistance) dmg = 0;
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) /* new calculations --Amy */	destroy_item(RING_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) /* new calculations --Amy */	destroy_item(WAND_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 30 : 165)) /* new calculations --Amy */	destroy_item(AMULET_CLASS, AD_ELEC);
			if (dmg) {pline("You are hit by lightning!"); losehp(dmg, "lightning", KILLED_BY); }
			break;
			case 3: /* ice */
			if(Cold_resistance) dmg = 0;
			if (!rn2(10)) make_frozen(HFrozen + (dmg * 5), TRUE); /* randomly freeze the player --Amy */
		    if (isevilvariant || !rn2(issoviet ? 6 : Race_if(PM_GAVIL) ? 6 : Race_if(PM_HYPOTHERMIC) ? 6 : 33)) /* new calculations --Amy */  destroy_item(POTION_CLASS, AD_COLD);
			if (dmg) {pline("You are hit by cold!"); losehp(dmg, "cold", KILLED_BY); }
			break;
			case 4: /* fire */
			if (Race_if(PM_LOWER_ENT)) dmg *= 2;
			if(Fire_resistance) dmg = 0;
			if ( /* burnarmor(&youmonst) || */ isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33)) /* new calculation -- Amy */ {
			    destroy_item(SCROLL_CLASS, AD_FIRE);
			    destroy_item(SPBOOK_CLASS, AD_FIRE);
			    destroy_item(POTION_CLASS, AD_FIRE);
				burnarmor(&youmonst);
				}
			if (dmg) {pline("You are hit by fire!"); losehp(dmg, "fire", KILLED_BY); }
			break;

			}
		}
		badeffect();
		badeffect();
		badeffect();
		badeffect();

		monstcnt = 6 + rnd(12);
		if (!rn2(5)) monstcnt += rnd(5);
		if (!rn2(25)) monstcnt += rnd(10);
		if (!rn2(125)) monstcnt += rnd(20);
		if (!rn2(725)) monstcnt += rnd(50);

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

	      while(--monstcnt >= 0) {
		(void) makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
		}

		u.aggravation = 0;

		pline("e Pale Wraith with a lightning stroke.");

		if (!rn2(10)) deltrap(trap);
		break;

	    case GREEN_GLYPH:
			{
			pline("You trigger a hidden glyph!");
		    seetrap(trap);

		    int dmg = (d(4, 4) + rnd(monster_difficulty() + 1));
			int dmgtype = rnd(4);
			if (dmg > 1) {
				if (u.ulevel == 1) dmg /= 2;
				else if (u.ulevel == 2) {
					dmg *= 2;
					dmg /= 3;
				} else if (u.ulevel == 3) {
					dmg *= 3;
					dmg /= 4;
				} else if (u.ulevel == 4) {
					dmg *= 4;
					dmg /= 5;
				}
			}

			switch (dmgtype) {

			case 1: /* ordinary physical damage */
			if (dmg) {losehp(dmg, "hidden glyph", KILLED_BY_AN); }
				break;
			case 2: /* shock */
			if (Shock_resistance) dmg = 0;
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) /* new calculations --Amy */	destroy_item(RING_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) /* new calculations --Amy */	destroy_item(WAND_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 30 : 165)) /* new calculations --Amy */	destroy_item(AMULET_CLASS, AD_ELEC);
			if (dmg) {pline("You are hit by lightning!"); losehp(dmg, "lightning", KILLED_BY); }
			break;
			case 3: /* ice */
			if(Cold_resistance) dmg = 0;
			if (!rn2(10)) make_frozen(HFrozen + (dmg * 5), TRUE); /* randomly freeze the player --Amy */
		    if (isevilvariant || !rn2(issoviet ? 6 : Race_if(PM_GAVIL) ? 6 : Race_if(PM_HYPOTHERMIC) ? 6 : 33)) /* new calculations --Amy */  destroy_item(POTION_CLASS, AD_COLD);
			if (dmg) {pline("You are hit by cold!"); losehp(dmg, "cold", KILLED_BY); }
			break;
			case 4: /* fire */
			if (Race_if(PM_LOWER_ENT)) dmg *= 2;
			if(Fire_resistance) dmg = 0;
			if ( /* burnarmor(&youmonst) || */ isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33)) /* new calculation -- Amy */ {
			    destroy_item(SCROLL_CLASS, AD_FIRE);
			    destroy_item(SPBOOK_CLASS, AD_FIRE);
			    destroy_item(POTION_CLASS, AD_FIRE);
				burnarmor(&youmonst);
				}
			if (dmg) {pline("You are hit by fire!"); losehp(dmg, "fire", KILLED_BY); }
			break;

			}
		}
		badeffect();
		badeffect();
		badeffect();

		if (!rn2(10)) deltrap(trap);
		break;

	    case YELLOW_GLYPH:
			{
			pline("You trigger a hidden glyph!");
		    seetrap(trap);

		    int dmg = (d(4, 4) + rnd(monster_difficulty() + 1));
			int dmgtype = rnd(4);
			if (dmg > 1) {
				if (u.ulevel == 1) dmg /= 2;
				else if (u.ulevel == 2) {
					dmg *= 2;
					dmg /= 3;
				} else if (u.ulevel == 3) {
					dmg *= 3;
					dmg /= 4;
				} else if (u.ulevel == 4) {
					dmg *= 4;
					dmg /= 5;
				}
			}

			switch (dmgtype) {

			case 1: /* ordinary physical damage */
			if (dmg) {losehp(dmg, "hidden glyph", KILLED_BY_AN); }
				break;
			case 2: /* shock */
			if (Shock_resistance) dmg = 0;
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) /* new calculations --Amy */	destroy_item(RING_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) /* new calculations --Amy */	destroy_item(WAND_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 30 : 165)) /* new calculations --Amy */	destroy_item(AMULET_CLASS, AD_ELEC);
			if (dmg) {pline("You are hit by lightning!"); losehp(dmg, "lightning", KILLED_BY); }
			break;
			case 3: /* ice */
			if(Cold_resistance) dmg = 0;
			if (!rn2(10)) make_frozen(HFrozen + (dmg * 5), TRUE); /* randomly freeze the player --Amy */
		    if (isevilvariant || !rn2(issoviet ? 6 : Race_if(PM_GAVIL) ? 6 : Race_if(PM_HYPOTHERMIC) ? 6 : 33)) /* new calculations --Amy */  destroy_item(POTION_CLASS, AD_COLD);
			if (dmg) {pline("You are hit by cold!"); losehp(dmg, "cold", KILLED_BY); }
			break;
			case 4: /* fire */
			if (Race_if(PM_LOWER_ENT)) dmg *= 2;
			if(Fire_resistance) dmg = 0;
			if ( /* burnarmor(&youmonst) || */ isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33)) /* new calculation -- Amy */ {
			    destroy_item(SCROLL_CLASS, AD_FIRE);
			    destroy_item(SPBOOK_CLASS, AD_FIRE);
			    destroy_item(POTION_CLASS, AD_FIRE);
				burnarmor(&youmonst);
				}
			if (dmg) {pline("You are hit by fire!"); losehp(dmg, "fire", KILLED_BY); }
			break;

			}
		badeffect();
		badeffect();
		}
		monstcnt = 1;
		if (!rn2(5)) monstcnt += 1;
		if (!rn2(25)) monstcnt += 2;
		if (!rn2(125)) monstcnt += 3;
		if (!rn2(725)) monstcnt += rnd(20);

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

	      while(--monstcnt >= 0) {
		(void) makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
		}

		u.aggravation = 0;

		pline("es come to life!");

		if (!rn2(10)) deltrap(trap);
		break;

	    case BLUE_GLYPH:
			{
			pline("You trigger a hidden glyph!");
		    seetrap(trap);

		    int dmg = (d(4, 4) + rnd((monster_difficulty() / 4) + 1));
			int dmgtype = rnd(4);
			if (dmg > 1) {
				if (u.ulevel == 1) dmg /= 2;
				else if (u.ulevel == 2) {
					dmg *= 2;
					dmg /= 3;
				} else if (u.ulevel == 3) {
					dmg *= 3;
					dmg /= 4;
				} else if (u.ulevel == 4) {
					dmg *= 4;
					dmg /= 5;
				}
			}

			switch (dmgtype) {

			case 1: /* ordinary physical damage */
			if (dmg) {losehp(dmg, "hidden glyph", KILLED_BY_AN); }
				break;
			case 2: /* shock */
			if (Shock_resistance) dmg = 0;
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) /* new calculations --Amy */	destroy_item(RING_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) /* new calculations --Amy */	destroy_item(WAND_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 30 : 165)) /* new calculations --Amy */	destroy_item(AMULET_CLASS, AD_ELEC);
			if (dmg) {pline("You are hit by lightning!"); losehp(dmg, "lightning", KILLED_BY); }
			break;
			case 3: /* ice */
			if(Cold_resistance) dmg = 0;
			if (!rn2(10)) make_frozen(HFrozen + (dmg * 5), TRUE); /* randomly freeze the player --Amy */
		    if (isevilvariant || !rn2(issoviet ? 6 : Race_if(PM_GAVIL) ? 6 : Race_if(PM_HYPOTHERMIC) ? 6 : 33)) /* new calculations --Amy */  destroy_item(POTION_CLASS, AD_COLD);
			if (dmg) {pline("You are hit by cold!"); losehp(dmg, "cold", KILLED_BY); }
			break;
			case 4: /* fire */
			if (Race_if(PM_LOWER_ENT)) dmg *= 2;
			if(Fire_resistance) dmg = 0;
			if ( /* burnarmor(&youmonst) || */ isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33)) /* new calculation -- Amy */ {
			    destroy_item(SCROLL_CLASS, AD_FIRE);
			    destroy_item(SPBOOK_CLASS, AD_FIRE);
			    destroy_item(POTION_CLASS, AD_FIRE);
				burnarmor(&youmonst);
				}
			if (dmg) {pline("You are hit by fire!"); losehp(dmg, "fire", KILLED_BY); }
			break;

			}
		}
		badeffect();
		badeffect();
		badeffect();
		badeffect();
		badeffect();

		if (!rn2(10)) deltrap(trap);
		break;

	    case SHIT_TRAP:

		{
			boolean larissashoes = (uarmf ? TRUE : FALSE);

			if ((Levitation || Flying || (uarmf && itemhasappearance(uarmf, APP_YELLOW_SNEAKERS) ) ) && !(Role_if(PM_GANG_SCHOLAR)) && !(SoiltypeEffect || u.uprops[SOILTYPE].extrinsic || have_soiltypestone() || (uarmf && uarmf->oartifact == ART_ARABELLA_S_GIRL_KICK)) && !SpellColorBrown && !FemtrapActiveAnastasia && !FemtrapActiveBridghitte && !FemtrapActiveLarissa && !FemtrapActiveHenrietta && !FemtrapActiveAnna && !(uarmg && uarmg->oartifact == ART_MADELINE_S_STUPID_GIRL) && !(uarmf && itemhasappearance(uarmf, APP_SKI_HEELS)) && !(uarmf && itemhasappearance(uarmf, APP_TREADED_HEELS)) && !(autismweaponcheck(ART_LUISA_S_CHARMING_BEAUTY) && !rn2(200) ) && !(uarmf && uarmf->oartifact == ART_ANASTASIA_S_PLAYFULNESS) && !(uarmf && uarmf->oartifact == ART_BRIDGE_SHITTE) && !(uarmf && (itemhasappearance(uarmf, APP_HUGGING_BOOTS) || itemhasappearance(uarmf, APP_HEELED_HUGGING_BOOTS) || itemhasappearance(uarmf, APP_BUFFALO_BOOTS)) ) ) { /* ground-based trap, obviously */
			    if (!already_seen && rn2(3)) break;
			    seetrap(trap);
			    pline("%s %s on the ground below you.",
				    already_seen ? "There is" : "You discover", "a stinking heap of shit");
				break;
			} 

			seetrap(trap);
			doshittrap((struct obj *)0);

			if (u.larissatimer) {
				u.larissatimer = 0;
				u.cnd_larissatrapcnt++; /* counts only if you actually obey the command to step into shit --Amy */
				if (!larissashoes) {
					statdrain();
					You("were told that you should be wearing shoes when stepping into shit, but you just didn't want to listen...");
				} else if (PlayerInHighHeels) {
					pline("It was fun to step into dog shit with your high heels.");
					goodeffect();
				} else {
					pline("Okay, you stepped into dog shit with your shoes, although you're not really sure why you had to do that.");
				}
			}

			if (!rn2(50) || (uarmf && uarmf->oartifact == ART_ELIANE_S_SHIN_SMASH) ) deltrap(trap);
			break;

		}

	    case PIT:
	    case SPIKED_PIT:
	    case GIANT_CHASM:
	    case SHIT_PIT:
	    case MANA_PIT:
	    case ANOXIC_PIT:
	    case HYPOXIC_PIT:
	    case ACID_PIT:

		if (uarmf && uarmf->oartifact == ART_BEWARE_OF_THE_PITFALL) break; /* yes, even in sokoban! */

		/* KMH -- You can't escape the Sokoban level traps */
		if (!In_sokoban(&u.uz) && !(SoiltypeEffect || u.uprops[SOILTYPE].extrinsic || have_soiltypestone() || (uarmf && uarmf->oartifact == ART_ARABELLA_S_GIRL_KICK)) && (Levitation || Flying)) break;

		if (ttype == SHIT_PIT && (uarmf && itemhasappearance(uarmf, APP_YELLOW_SNEAKERS))) break;

		seetrap(trap);
		if (!In_sokoban(&u.uz) && is_clinger(youmonst.data)) {
		    if(trap->tseen) {
			You("see %s %spit below you.", a_your[trap->madeby_u],
			    ttype == SPIKED_PIT ? "spiked " : ttype == GIANT_CHASM ? "gigantic " : "");
		    } else {
			pline("%s pit %sopens up under you!",
			    A_Your[trap->madeby_u],
			    ttype == SPIKED_PIT ? "full of spikes " : ttype == GIANT_CHASM ? "of enormous depth " : "");
			You("don't fall in!");
		    }
		    break;
		}
		if (!In_sokoban(&u.uz)) {
		   char verbbuf[BUFSZ];
		    if (u.usteed) {
		    	if ((trflags & RECURSIVETRAP) != 0)
			    sprintf(verbbuf, "and %s fall",
				x_monnam(u.usteed,
				    u.usteed->mnamelth ? ARTICLE_NONE : ARTICLE_THE,
				    (char *)0, SUPPRESS_SADDLE, FALSE));
			else
			    sprintf(verbbuf,"lead %s",
				x_monnam(u.usteed,
					 u.usteed->mnamelth ? ARTICLE_NONE : ARTICLE_THE,
				 	 "poor", SUPPRESS_SADDLE, FALSE));
		    } else
		    strcpy(verbbuf,"fall");
		   You("%s into %s pit!", verbbuf, a_your[trap->madeby_u]);
		}
		/* wumpus reference */
		if (Role_if(PM_RANGER) && !trap->madeby_u && !trap->once &&
			In_quest(&u.uz) && Is_qlocate(&u.uz)) {
		    pline("Fortunately it has a bottom after all...");
		    trap->once = 1;
		} else if (u.umonnum == PM_PIT_VIPER ||
			u.umonnum == PM_PIT_FIEND)
		    pline("How pitiful.  Isn't that the pits?");

		if (ttype == SPIKED_PIT && uarmf && uarmf->oartifact == ART_SPIKEMASH) {
			if (trap) trap->ttyp = PIT;
			ttype = PIT;
			pline("Thanks to your spike-mashing boots, the spikes in the pits are destroyed and cannot harm you.");
		}

		if (ttype == SPIKED_PIT) {
		    const char *predicament = "on a set of sharp iron spikes";
		    if (u.usteed) {
			pline("%s lands %s!",
				upstart(x_monnam(u.usteed,
					 u.usteed->mnamelth ? ARTICLE_NONE : ARTICLE_THE,
					 "poor", SUPPRESS_SADDLE, FALSE)),
			      predicament);
		    } else
		    You("land %s!", predicament);
		}
		if (!Passes_walls)
		    u.utrap = rn1(6,2);
		    if (ttype == SHIT_PIT) {
				u.utrap += rnd((rnd(6)) );
				if (trap && uarmf && uarmf->oartifact == ART_ELIANE_S_SHIN_SMASH) trap->ttyp = PIT;
			}
		    if (ttype == GIANT_CHASM) u.utrap += rnd(75);
		u.utraptype = TT_PIT;
		if (!steedintrap(trap, (struct obj *)0)) {

		{
			int pitdamage;

			if (ttype == SPIKED_PIT) {
				pitdamage = 10;
				if (pitdamage > 1) {
					if (u.ulevel == 1) pitdamage /= 2;
					else if (u.ulevel == 2) {
						pitdamage *= 2;
						pitdamage /= 3;
					} else if (u.ulevel == 3) {
						pitdamage *= 3;
						pitdamage /= 4;
					} else if (u.ulevel == 4) {
						pitdamage *= 4;
						pitdamage /= 5;
					}
				}
			    losehp(rnd(pitdamage) + rnd((monster_difficulty() / 2) + 1) ,"fell into a pit of iron spikes",
				NO_KILLER_PREFIX);
			    if (!rn2(6)) {

				 int inhalechance = 100;
				 if (u.urmaxlvlUP == 1) inhalechance = 30;
				 else if (u.urmaxlvlUP == 2) inhalechance = 50;
				 else if (u.urmaxlvlUP == 3) inhalechance = 75;

				if (rn2(100) < inhalechance) {
					poisoned("spikes", A_STR, "fall onto poison spikes", 8);
				}
			    }
			} else if (ttype == GIANT_CHASM) {
				pitdamage = 25;
				if (u.urmaxlvlUP < 5) pitdamage = 12;
				if (pitdamage > 1) {
					if (u.ulevel == 1) pitdamage /= 2;
					else if (u.ulevel == 2) {
						pitdamage *= 2;
						pitdamage /= 3;
					} else if (u.ulevel == 3) {
						pitdamage *= 3;
						pitdamage /= 4;
					} else if (u.ulevel == 4) {
						pitdamage *= 4;
						pitdamage /= 5;
					}
				}
			    losehp(rnd(pitdamage) + rnd(monster_difficulty() + 1) ,"fell into a giant chasm", NO_KILLER_PREFIX);
			} else {
				pitdamage = 6;
				if (pitdamage > 1) {
					if (u.ulevel == 1) pitdamage /= 2;
					else if (u.ulevel == 2) {
						pitdamage *= 2;
						pitdamage /= 3;
					} else if (u.ulevel == 3) {
						pitdamage *= 3;
						pitdamage /= 4;
					} else if (u.ulevel == 4) {
						pitdamage *= 4;
						pitdamage /= 5;
					}
				}
			    losehp(rnd(pitdamage) + rnd((monster_difficulty() / 3) + 1) ,"fell into a pit", NO_KILLER_PREFIX);
			}
		}

		if (ttype == SHIT_PIT) {
			doshittrap((struct obj *)0);
		}

		if (ttype == MANA_PIT) {
		    drain_en(rnz(monster_difficulty() + 1));
		}

		if (Punished && !carried(uball)) {
		    unplacebc();
		    ballfall();
		    placebc();
		}
		selftouch("Falling, you");
		vision_full_recalc = 1;	/* vision limits change */
		exercise(A_STR, FALSE);
		exercise(A_DEX, FALSE);
		}
		break;
	    case HOLE:
	    case TRAPDOOR:
	    case SHAFT_TRAP:
	    case CURRENT_SHAFT:
		if (!Can_fall_thru(&u.uz)) {

		    if (wizard && yn("Trigger the trap that shouldn't exist here?") == 'y') {
			; /* do nothing, for testing purposes --Amy */
		    } else {

			    seetrap(trap);	/* normally done in fall_through */
			    impossible("dotrap: %ss cannot exist on this level.",
				       defsyms[trap_to_defsym(ttype)].explanation);
			    deltrap(trap);
			    break;		/* don't activate it after all */
		    }
		}
		if (ttype != SHAFT_TRAP && ttype != CURRENT_SHAFT) fall_through(TRUE);
		else if (ttype != CURRENT_SHAFT) fall_throughX(TRUE);
		else fall_throughY(TRUE);
		break;

	    case TELEP_TRAP:
		seetrap(trap);
		tele_trap(trap);
		break;

	    case BEAMER_TRAP:
		seetrap(trap);
		tele_trapX(trap);
		break;

	    case RELOCATION_TRAP:
		seetrap(trap);
		You_feel("yourself yanked in a direction you didn't know existed!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	      (void) safe_teleds_normalterrain(FALSE);
		deltrap(trap);
		break;

	    case WARP_ZONE:
		deltrap(trap);

		if (((u.uhave.amulet) && !u.freeplaymode) || CannotTeleport || (u.usteed && mon_has_amulet(u.usteed))) { pline("You shudder for a moment."); (void) safe_teleds_normalterrain(FALSE); break;}

		if (playerlevelportdisabled()) { 
			pline("For some reason you resist the banishment!");
			break;
		}

		banishplayer();
		pline("Welcome to warp zone!");

		break;

	    case BACK_TO_START_TRAP:
		deltrap(trap);

		if (((u.uhave.amulet) && !u.freeplaymode) || CannotTeleport || (u.usteed && mon_has_amulet(u.usteed))) { pline("You shudder for a moment."); (void) safe_teleds_normalterrain(FALSE); break;}

		if (playerlevelportdisabled()) { 
			pline("For some reason you resist the banishment!");
			break;
		}

		make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

		(void) safe_teleds(FALSE);
		goto_level(&medusa_level, TRUE, FALSE, FALSE);

		register int newlevX = 1;
		d_level newlevelX;
		get_level(&newlevelX, newlevX);
		goto_level(&newlevelX, TRUE, FALSE, FALSE);
		pline("It's back to square one for you!");

		break;

	    case LEVEL_TELEP:
		seetrap(trap);
		level_tele_trap(trap);
		break;

	    case NEMESIS_TRAP:
		deltrap(trap);
		pline("CLICK! You have triggered a trap!");

		{
		register int nemesenum = urole.nemesnum;

		(void) makemon(&mons[nemesenum], u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
		verbalize("Ha ha ha ha! Wa ha ha ha! You are so doomed!");

		}

		break;

	    case PHASE_BEAMER:
		seetrap(trap);
		safe_teledsPD(0);
		You("triggered a phase teleport trap!");

		break;

	    case PHASEPORTER:
		seetrap(trap);
		phase_door(0);
		You("triggered a phase teleport trap!");

		break;

	    case LEVEL_BEAMER:
		seetrap(trap);
		level_tele_trapX(trap);
		break;

	    case BRANCH_TELEPORTER:

		You("triggered a branch teleporter!");
		if (Antimagic) {
			shieldeff(u.ux, u.uy);
		}
		if (Antimagic || In_endgame(&u.uz)) {
			You_feel("a wrenching sensation.");
			break;
		}

	      if (!playerlevelportdisabled()) {
			deltrap(trap);
			newsym(u.ux,u.uy);	/* get rid of trap symbol */
			randombranchtele();
		} else pline("The trap doesn't seem to have any effect on you.");

		break;

	    case BRANCH_BEAMER:

		You("triggered a branch beamer!");
	      if (!playerlevelportdisabled()) {
			deltrap(trap);
			newsym(u.ux,u.uy);	/* get rid of trap symbol */
			randombranchtele();
		} else pline("The trap doesn't seem to have any effect on you.");

		break;

	    case MAGIC_CANCELLATION_TRAP:
		pline("You're surrounded by a cyan glow!");
		seetrap(trap);
		MCReduction += rnz(100 * (monster_difficulty() + 1));
		pline("The magic cancellation granted by your armor seems weaker now...");
		break;

	    case LOCK_TRAP:
		seetrap(trap);
		pline("Uh-oh, should have watched your step...");
	    switch(rn2(26)) {
		case 25:
		case 24:
		case 23:
		case 22:
		case 21:
			pline("You're hit by a massive explosion!");
			wake_nearby();
			losehp( (d(6,6) + rnd((monster_difficulty()) + 1) ), "massive explosion", KILLED_BY_AN);
			exercise(A_STR, FALSE);
			break;
		case 20:
		case 19:
		case 18:
		case 17:
			pline("A cloud of noxious gas billows out at you.");
			poisoned("gas cloud", A_STR, "cloud of poison gas",15);
			exercise(A_CON, FALSE);
			break;
		case 16:
		case 15:
		case 14:
		case 13:
			You_feel("a needle prick your %s.",body_part(ARM));
			poisoned("needle", A_CON, "poisoned needle",10);
			exercise(A_CON, FALSE);
			break;
		case 12:
		case 11:
		case 10:
		case 9:
			dofiretrap((struct obj *)0);
			break;
		case 8:
		case 7:
		case 6: {
			int dmg;

			You("are jolted by a surge of electricity!");
			if ((Shock_resistance && (StrongShock_resistance || rn2(10))) || ShockImmunity ) {
			    shieldeff(u.ux, u.uy);
			    You("don't seem to be affected.");
			    break;
			} else
			    losehp(d(4, 4) + rnd((monster_difficulty() / 2) + 1), "electric shock", KILLED_BY_AN);
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) /* new calculations --Amy */	destroy_item(RING_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) /* new calculations --Amy */	destroy_item(WAND_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 30 : 165)) /* new calculations --Amy */	destroy_item(AMULET_CLASS, AD_ELEC);
			break;
		      }
		case 5:
		case 4:
		case 3:
			if (!Free_action || !rn2(StrongFree_action ? 100 : 20)) {
			pline("Suddenly you are frozen in place!");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
			if (isstunfish) nomul(-rnz(11), "frozen by a lock trap", TRUE);
			else nomul(-rn1(5, 6), "frozen by a lock trap", TRUE);
			exercise(A_DEX, FALSE);
			nomovemsg = You_can_move_again;
			} else You("momentarily stiffen.");
			break;
		case 2:
		case 1:
		case 0:
			pline("A cloud of %s gas billows out at you.",
				Blind ? blindgas[rn2(SIZE(blindgas))] :
				rndcolor() );
			if(!Stunned) {
			    if (Blind)
				You("%s and get dizzy...",
				    stagger(youmonst.data, "stagger"));
			    else
				You("%s and your vision blurs...",
				    stagger(youmonst.data, "stagger"));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Imet' delo s effektami statusa ili sdat'sya!" : "Wrueue-ue-e-ue-e-ue-e...");
			}
			make_stunned(HStun + rn1(7, 16) + rnd((monster_difficulty() / 2) + 1),FALSE);
			(void) make_hallucinated(HHallucination + rn1(5, 16) + rnd((monster_difficulty() / 2) + 1),FALSE,0L);
			break;
		default: impossible("bad lock trap");
			break;
		}
		break;

	    case FUMBLING_TRAP:
		pline("A green glow surrounds you...");
		HFumbling = FROMOUTSIDE | rnd(5);
		incr_itimeout(&HFumbling, rnd(20));
		u.fumbleduration += rnz(10 * (monster_difficulty() + 1) );
		seetrap(trap);
		You_feel("like a whiny Mary-Sue!"); /* yes Anabella Swansteele, I'm looking at you!!! --Amy */
		break;

	    case CONFUSE_TRAP:
		pline("A cyan glow surrounds you...");
		seetrap(trap);
		make_confused(HConfusion + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
		pline("You're confused!");
		break;

	    case STUN_TRAP:
		pline("A yellow glow surrounds you...");
		seetrap(trap);
		make_stunned(HStun + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
		break;

	    case HALLUCINATION_TRAP:
		pline("A multicolored glow surrounds you...");
		seetrap(trap);
		(void) make_hallucinated(HHallucination + rnd(10) + rnd(monster_difficulty() + 1), TRUE, 0L);
		break;

	    case TOXIC_VENOM_TRAP:
		pline("CLICK! You have triggered a trap!");
		seetrap(trap);
		if (chromeprotection()) break;

		if (!Poison_resistance) pline("You're badly poisoned!");
		else pline("You're poisoned!");
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_STR, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_DEX, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_CON, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_INT, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_WIS, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_CHA, -rnd(2), FALSE, TRUE);

		if (isevilvariant || !rn2(issoviet ? 2 : 20)) (void)destroy_item(POTION_CLASS, AD_VENO);
		if (isevilvariant || !rn2(issoviet ? 2 : 20)) (void)destroy_item(FOOD_CLASS, AD_VENO);

		poisoned("The air", rn2(A_MAX), "toxic venom trap", 30);

		break;

	    case NUMBNESS_TRAP:
		pline("An orange glow surrounds you...");
		seetrap(trap);
		make_numbed(HNumbed + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
		break;

	    case FREEZING_TRAP:
		pline("An icy glow surrounds you...");
		seetrap(trap);
		make_frozen(HFrozen + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
		break;

	    case BURNING_TRAP:
		pline("A red glow surrounds you...");
		seetrap(trap);
		make_burned(HBurned + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
		break;

	    case FEAR_TRAP:
		pline("A violet glow surrounds you...");
		seetrap(trap);
		make_feared(HFeared + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
		break;

	    case BLINDNESS_TRAP:
		pline("You are blinded by a flash of light!");
		seetrap(trap);
		make_blinded(Blinded + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
		break;

	    case INERTIA_TRAP:
		pline("You stepped on a trigger!");
		seetrap(trap);
		if (inertiaprotection()) break;
	      u_slow_down();
		u.uprops[DEAC_FAST].intrinsic += (( rnd(10) + rnd(monster_difficulty() + 1) ) * 10);
		pline(u.inertia ? "You feel even slower." : "You slow down to a crawl.");
		u.inertia += (rnd(10) + rnd(monster_difficulty() + 1));
		break;

	    case TIME_TRAP:
		{
		int dmg;
		dmg = (rnd(10) + rnd( (monster_difficulty() * 2) + 1));
		pline("You stepped on a trigger!");
		seetrap(trap);
		if (powerfulimplants() && uimplant && uimplant->oartifact == ART_TIMEAGE_OF_REALMS) {
			You_feel("not as powerful as you used to be, but the feeling passes.");
			break;
		}
		switch (rnd(10)) {

			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				You_feel("life has clocked back.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Zhizn' razgonyal nazad, potomu chto vy ne smotreli, i teper' vy dolzhny poluchit', chto poteryannyy uroven' nazad." : "Kloeck!");
			      if (u.urmaxlvlUP >= 2) losexp("time", FALSE, FALSE); /* resistance is futile :D */
				break;
			case 6:
			case 7:
			case 8:
			case 9:
				switch (rnd(A_MAX)) {
					case A_STR:
						pline("You're not as strong as you used to be...");
						ABASE(A_STR) -= 5;
						if(ABASE(A_STR) < ATTRABSMIN(A_STR)) {dmg *= 3; ABASE(A_STR) = ATTRABSMIN(A_STR);}
						break;
					case A_DEX:
						pline("You're not as agile as you used to be...");
						ABASE(A_DEX) -= 5;
						if(ABASE(A_DEX) < ATTRABSMIN(A_DEX)) {dmg *= 3; ABASE(A_DEX) = ATTRABSMIN(A_DEX);}
						break;
					case A_CON:
						pline("You're not as hardy as you used to be...");
						ABASE(A_CON) -= 5;
						if(ABASE(A_CON) < ATTRABSMIN(A_CON)) {dmg *= 3; ABASE(A_CON) = ATTRABSMIN(A_CON);}
						break;
					case A_WIS:
						pline("You're not as wise as you used to be...");
						ABASE(A_WIS) -= 5;
						if(ABASE(A_WIS) < ATTRABSMIN(A_WIS)) {dmg *= 3; ABASE(A_WIS) = ATTRABSMIN(A_WIS);}
						break;
					case A_INT:
						pline("You're not as bright as you used to be...");
						ABASE(A_INT) -= 5;
						if(ABASE(A_INT) < ATTRABSMIN(A_INT)) {dmg *= 3; ABASE(A_INT) = ATTRABSMIN(A_INT);}
						break;
					case A_CHA:
						pline("You're not as beautiful as you used to be...");
						ABASE(A_CHA) -= 5;
						if(ABASE(A_CHA) < ATTRABSMIN(A_CHA)) {dmg *= 3; ABASE(A_CHA) = ATTRABSMIN(A_CHA);}
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
				if(ABASE(A_STR) < ATTRABSMIN(A_STR)) {dmg *= 2; ABASE(A_STR) = ATTRABSMIN(A_STR);}
				if(ABASE(A_DEX) < ATTRABSMIN(A_DEX)) {dmg *= 2; ABASE(A_DEX) = ATTRABSMIN(A_DEX);}
				if(ABASE(A_CON) < ATTRABSMIN(A_CON)) {dmg *= 2; ABASE(A_CON) = ATTRABSMIN(A_CON);}
				if(ABASE(A_WIS) < ATTRABSMIN(A_WIS)) {dmg *= 2; ABASE(A_WIS) = ATTRABSMIN(A_WIS);}
				if(ABASE(A_INT) < ATTRABSMIN(A_INT)) {dmg *= 2; ABASE(A_INT) = ATTRABSMIN(A_INT);}
				if(ABASE(A_CHA) < ATTRABSMIN(A_CHA)) {dmg *= 2; ABASE(A_CHA) = ATTRABSMIN(A_CHA);}
				break;
		}
		if (dmg) losehp(dmg, "a time trap", KILLED_BY);
		}
		break;

	    case NEGATIVE_TRAP:
		pline("You stepped on a trigger!");
		seetrap(trap);
		if (!obsidianprotection()) switch (rnd(11)) {
			case 1:
				make_blinded(Blinded + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
				break;
			case 2:
				make_feared(HFeared + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
				break;
			case 3:
				make_burned(HBurned + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
				break;
			case 4:
				make_frozen(HFrozen + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
				break;
			case 5:
				make_numbed(HNumbed + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
				break;
			case 6:
				(void) make_hallucinated(HHallucination + rnd(10) + rnd(monster_difficulty() + 1), TRUE, 0L);
				break;
			case 7:
				make_stunned(HStun + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
				break;
			case 8:
				make_confused(HConfusion + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
				pline("You're confused!");
				break;
			case 9:
				(void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
				break;
			case 10:
				make_sick(rn1(25,25), "negative food poisoning", TRUE, SICK_VOMITABLE);
				break;
			case 11:
				make_dimmed(HDimmed + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
				break;
		}
		break;

	    case MADNESS_TRAP:
		pline("You stepped on a trigger!");
		seetrap(trap);

madnesseffect:
		switch (rnd(9)) {
			case 1:
				make_blinded(Blinded + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
				break;
			case 2:
				make_feared(HFeared + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
				break;
			case 3:
				make_burned(HBurned + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
				break;
			case 4:
				make_frozen(HFrozen + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
				break;
			case 5:
				make_numbed(HNumbed + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
				break;
			case 6:
				(void) make_hallucinated(HHallucination + rnd(10) + rnd(monster_difficulty() + 1), TRUE, 0L);
				break;
			case 7:
				make_stunned(HStun + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
				break;
			case 8:
				make_confused(HConfusion + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
				pline("You're confused!");
				break;
			case 9:
				make_dimmed(HDimmed + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
				break;
		}
		if (rn2(6)) goto madnesseffect;

		break;

	    case PETRIFICATION_TRAP:
		pline("CLICK! You have triggered a trap!");
		seetrap(trap);
		    if ((!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) &&
			!(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {
			if (!Stoned) {
				if (Hallucination && rn2(10)) pline("Thankfully you are already stoned.");
				else {
					Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
					u.cnd_stoningcount++;
					pline("You start turning to stone!");
				}
			}
			sprintf(killer_buf, "petrification trap");
			delayed_killer = killer_buf;
		
		    }
		break;

	    case CANCELLATION_TRAP:
		pline("CLICK! You have triggered a trap!");
		seetrap(trap);
		(void) cancel_monst(&youmonst, (struct obj *)0, FALSE, TRUE, FALSE);
		break;

	    case GLIB_TRAP:
		pline("You stepped on a trigger!");
		seetrap(trap);
		pline("Glibbery stuff pours all over your %s!", makeplural(body_part(HAND)) );
		incr_itimeout(&Glib, rnd(15) + rnd(monster_difficulty() + 1) );
		break;

	    case SLIME_TRAP:
		pline("CLICK! You have triggered a trap!");
		seetrap(trap);
		if (!Slimed && !flaming(youmonst.data) && !Unchanging && !slime_on_touch(youmonst.data) ) {
			You("don't feel very well.");
			make_slimed(100);
		    killer_format = KILLED_BY_AN;
		    delayed_killer = "slimed by a slime trap";
		}
		break;

	    case LYCANTHROPY_TRAP:
		pline("CLICK! You have triggered a trap!");
		seetrap(trap);
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREWOLF;
			You_feel("feverish.");
			u.cnd_lycanthropecount++;
		}
		break;

	    case EARTHQUAKE_TRAP:
		pline("CLICK! You have triggered a trap!");
		deltrap(trap);
		pline_The("entire dungeon is shaking around you!");
		do_earthquake((GushLevel - 1) / 3 + 1);
		break;

	    case CONTAMINATION_TRAP:

		pline("You stepped on a trigger!");
		seetrap(trap);
		pline("Suddenly you're exposed to the contamination.");
		contaminate(rnd(10 + level_difficulty()), TRUE);

		break;

	    case ADJACENT_TRAP:
	    case SPREADING_TRAP:
	    case SUPERTHING_TRAP: /* these three don't do anything if you trigger them */
	    case KOP_CUBE:
	    case BOSS_SPAWNER:

		break;

	    case PET_TRAP: /* does not affect player, and does not become visible; however, can affect your steed */

		(void) steedintrap(trap, (struct obj *)0);

		break;

	    case UNLIGHT_TRAP:
		pline("Uh-oh, should have watched your step...");
		seetrap(trap);
		{
			int ulx, uly;
			for (ulx = 1; ulx < (COLNO); ulx++)
		        for (uly = 0; uly < (ROWNO); uly++) {
				levl[ulx][uly].lit = 0;
			}

		}
		pline("Eternal darkness seems to consume your surroundings...");
		break;

	    case HOSTILITY_TRAP: /* enrage spell effect */
		pline("Uh-oh, should have watched your step...");
		seetrap(trap);

		{

		struct monst *mtmp2;

		for (mtmp2 = fmon; mtmp2; mtmp2 = mtmp2->nmon) {

			if (mtmp2->mtame <= rnd(21) ) {

				int untamingchance = 10;

				if (!(PlayerCannotUseSkills)) {
					switch (P_SKILL(P_PETKEEPING)) {
						default: untamingchance = 10; break;
						case P_BASIC: untamingchance = 9; break;
						case P_SKILLED: untamingchance = 8; break;
						case P_EXPERT: untamingchance = 7; break;
						case P_MASTER: untamingchance = 6; break;
						case P_GRAND_MASTER: untamingchance = 5; break;
						case P_SUPREME_MASTER: untamingchance = 4; break;
					}
				}

				if (untamingchance > rnd(10) && !(Role_if(PM_DRAGONMASTER) && uarms && Is_dragon_shield(uarms) && mtmp2->data->mlet == S_DRAGON) && !((rnd(30 - ACURR(A_CHA))) < 4) ) {

					mtmp2->mtame = mtmp2->mpeaceful = 0;

				}

			} else if (!mtmp2->mtame && !is_infrastructure_monster(mtmp2)) {

				mtmp2->mtame = mtmp2->mpeaceful = 0;

			}

			if (!mtmp2->mtame && !is_infrastructure_monster(mtmp2) && !rn2(5)) mtmp2->mfrenzied = 1;

		}

		pline("It seems a little more dangerous here now...");
		if (!(InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone())) doredraw();

		}

		break;

	    case MIND_WIPE_TRAP:
		pline("CLICK! You have triggered a trap!");
		seetrap(trap);

		forget(3, FALSE);
		{
		if (!strncmpi(plname, "Maud", 4) || !strncmpi(plalias, "Maud", 4))
			pline("As your mind turns inward on itself, you forget everything else.");
		else if (rn2(2))
			pline("Who was that Maud person anyway?");
		else
			pline("Thinking of Maud you forget everything else.");
		}
		exercise(A_WIS, FALSE);

		break;

	    case GLUE_TRAP:
		seetrap(trap);
		if (webmsgok) pline("Eek - you stepped into a heap of sticky glue!");
		u.utraptype = TT_GLUE;
		u.utrap = 25 + rnd(monster_difficulty());

		break;

	    case WEB: /* Our luckless player has stumbled into a web. */
		seetrap(trap);
		if (amorphous(youmonst.data) || is_whirly(youmonst.data) ||
				unsolid(youmonst.data) || acidic(youmonst.data) || u.umonnum == PM_GELATINOUS_CUBE ||
			u.umonnum == PM_FIRE_ELEMENTAL) { /* bugfix --Amy */
		    if (acidic(youmonst.data) || u.umonnum == PM_GELATINOUS_CUBE ||
			u.umonnum == PM_FIRE_ELEMENTAL) {
			if (webmsgok)
			    You("%s %s spider web!",
				(u.umonnum == PM_FIRE_ELEMENTAL) ? "burn" : "dissolve",
				a_your[trap->madeby_u]);
			deltrap(trap);
			newsym(u.ux,u.uy);
			break;
		    }
		    if (webmsgok) You("flow through %s spider web.",
			    a_your[trap->madeby_u]);
		    break;
		}
		if (webmaker(youmonst.data) || (uarm && uarm->otyp == EILISTRAN_ARMOR) || Race_if(PM_SPIDERMAN) || (uarmf && itemhasappearance(uarmf, APP_SPIDER_BOOTS) ) ) {
		    if (webmsgok)
		    	pline(trap->madeby_u ? "You take a walk on your web."
					 : "There is a spider web here.");
		    break;
		}
		if (webmsgok) {
		   char verbbuf[BUFSZ];
		   verbbuf[0] = '\0';
		   if (u.usteed)
		   	sprintf(verbbuf,"lead %s",
				x_monnam(u.usteed,
					 u.usteed->mnamelth ? ARTICLE_NONE : ARTICLE_THE,
				 	 "poor", SUPPRESS_SADDLE, FALSE));
		   else
			
		    sprintf(verbbuf, "%s", Levitation ? (const char *)"float" :
		      		locomotion(youmonst.data, "stumble"));
		    You("%s into %s spider web!",
			verbbuf, a_your[trap->madeby_u]);
		}
		u.utraptype = TT_WEB;

		/* Time stuck in the web depends on your/steed strength. */
		{
		    register int str = ACURR(A_STR);

		    /* If mounted, the steed gets trapped.  Use mintrap
		     * to do all the work.  If mtrapped is set as a result,
		     * unset it and set utrap instead.  In the case of a
		     * strongmonst and mintrap said it's trapped, use a
		     * short but non-zero trap time.  Otherwise, monsters
		     * have no specific strength, so use player strength.
		     * This gets skipped for webmsgok, which implies that
		     * the steed isn't a factor.
		     */
		    if (u.usteed && webmsgok) {
			/* mtmp location might not be up to date */
			u.usteed->mx = u.ux;
			u.usteed->my = u.uy;

			/* mintrap currently does not return 2(died) for webs */
			if (mintrap(u.usteed)) {
			    u.usteed->mtrapped = 0;
			    if (strongmonst(u.usteed->data)) str = 17;
			} else {
			    break;
			}

			webmsgok = FALSE; /* mintrap printed the messages */
		    }
		    if (str <= 1) u.utrap = rn1(24,6);
		    else if (str <= 2) u.utrap = rn1(12,6);
		    else if (str <= 3) u.utrap = rn1(6,6);
		    else if (str < 6) u.utrap = rn1(6,4);
		    else if (str < 9) u.utrap = rn1(4,4);
		    else if (str < 12) u.utrap = rn1(4,2);
		    else if (str < 15) u.utrap = rn1(2,2);
		    else if (str < 18) u.utrap = rnd(2);
		    else if (str < 69) u.utrap = 1;
		    else {
			u.utrap = 0;
			if (webmsgok)
				You("tear through %s web!", a_your[trap->madeby_u]);
			deltrap(trap);
			newsym(u.ux,u.uy);	/* get rid of trap symbol */
		    }
		}
		break;

	    case STATUE_TRAP:
		activate_statue_trap(trap, u.ux, u.uy, FALSE);
		break;

	    case MAGIC_TRAP:	    /* A magic trap. */
		seetrap(trap);
		if (!rn2(30)) {
		    deltrap(trap);
		    newsym(u.ux,u.uy);	/* update position */
		    You("are caught in a magical explosion!");
		    losehp(rnd(10), "magical explosion", KILLED_BY_AN);
		    Your("body absorbs some of the magical energy!");
		    u.uen = (u.uenmax += 2);
		} else domagictrap();
		(void) steedintrap(trap, (struct obj *)0);
		break;

	    case ANTI_MAGIC:
		seetrap(trap);
		if(Antimagic && rn2(2) ) {
		    shieldeff(u.ux, u.uy);
		    You_feel("momentarily lethargic.");
		} else if(StrongAntimagic && rn2(2) ) {
		    shieldeff(u.ux, u.uy);
		    You_feel("momentarily lethargic.");
		} else drain_en(rnd(u.ulevel) + 1 + rnd(monster_difficulty() + 1));
		break;

	    case MANA_TRAP:
		pline("You are irradiated by pure mana!");
		seetrap(trap);
		drain_en(rnd(u.ulevel) + 1 + rnd(monster_difficulty() + 1));
		losehp(rnd(u.ulevel) + 1 + rnd(monster_difficulty() + 1), "mana trap", KILLED_BY_AN);
		break;

	    case OUT_OF_MAGIC_TRAP:
		seetrap(trap);
		if(Antimagic && !rn2(5) ) {
		    shieldeff(u.ux, u.uy);
		    You_feel("momentarily lethargic.");
		} else if(StrongAntimagic && !rn2(3) ) {
		    shieldeff(u.ux, u.uy);
		    You_feel("momentarily lethargic.");
		} else if (u.uen > ((u.uenmax / 9) + 1)) {
			u.uen = 0;
			You_feel("drained of energy!");
		}
		else {
			u.uen = 0;
			You_feel("drained of energy!");
			drain_en(rnd(u.ulevel) + 1 + rnd(monster_difficulty() + 1));
		}
		break;

	    case POLY_TRAP: {
	        char verbbuf[BUFSZ];
		seetrap(trap);
		if (u.usteed)
			sprintf(verbbuf, "lead %s",
				x_monnam(u.usteed,
					 u.usteed->mnamelth ? ARTICLE_NONE : ARTICLE_THE,
				 	 (char *)0, SUPPRESS_SADDLE, FALSE));
		else
		 sprintf(verbbuf,"%s",
		    Levitation ? (const char *)"float" :
		    locomotion(youmonst.data, "step"));
		You("%s onto a polymorph trap!", verbbuf);
		if(Antimagic || Unchanging) {
		    shieldeff(u.ux, u.uy);
		    You_feel("momentarily different.");
		    /* Trap did nothing; don't remove it --KAA */
		} else {

			if (uarmf && uarmf->oartifact == ART_DEEPMINE_SAFETY) {
				char deepminebuf[BUFSZ];
				getlin ("Do you want to be polymorped? [y/yes/no]",deepminebuf);
				(void) lcase (deepminebuf);
				if (!(strcmp (deepminebuf, "yes")) || !(strcmp (deepminebuf, "y")) || !(strcmp (deepminebuf, "ye")) || !(strcmp (deepminebuf, "ys"))) {
					pline("Okay...");
				} else {
					pline("Alright, you won't be polymorphed this time.");
					break;
				}
			}

		    (void) steedintrap(trap, (struct obj *)0);
		    deltrap(trap);	/* delete trap before polymorph */
		    newsym(u.ux,u.uy);	/* get rid of trap symbol */
		    You_feel("a change coming over you.");
			if (moves < 1000) u.polyprotected = TRUE;
			polyself(FALSE);
			u.polyprotected = FALSE;
		}
		break;
	    }

	    case GENETIC_TRAP: {
	        char verbbuf[BUFSZ];
		seetrap(trap);
		 sprintf(verbbuf,"%s",
		    Levitation ? (const char *)"float" :
		    locomotion(youmonst.data, "step"));
		You("%s onto a genetic trap!", verbbuf);
		if(Antimagic || Unchanging) {
		    shieldeff(u.ux, u.uy);
		    You_feel("momentarily different.");
		    /* Trap did nothing; don't remove it --KAA */
		} else {

		    deltrap(trap);	/* delete trap before polymorph */
		    newsym(u.ux,u.uy);	/* get rid of trap symbol */
		    You_feel("a change coming over you.");

			do {
				u.wormpolymorph = rn2(NUMMONS);
			} while(( (notake(&mons[u.wormpolymorph]) && rn2(4) ) || ((mons[u.wormpolymorph].mlet == S_BAT) && rn2(2)) || ((mons[u.wormpolymorph].mlet == S_EYE) && rn2(2) ) || ((mons[u.wormpolymorph].mmove == 1) && rn2(4) ) || ((mons[u.wormpolymorph].mmove == 2) && rn2(3) ) || ((mons[u.wormpolymorph].mmove == 3) && rn2(2) ) || ((mons[u.wormpolymorph].mmove == 4) && !rn2(3) ) || ( (mons[u.wormpolymorph].mlevel < 10) && ((mons[u.wormpolymorph].mlevel + 1) < rnd(u.ulevel)) ) || (!haseyes(&mons[u.wormpolymorph]) && rn2(2) ) || ( is_nonmoving(&mons[u.wormpolymorph]) && rn2(5) ) || ( is_eel(&mons[u.wormpolymorph]) && rn2(5) ) || ( is_nonmoving(&mons[u.wormpolymorph]) && rn2(20) ) || (is_jonadabmonster(&mons[u.wormpolymorph]) && rn2(20)) || ( uncommon2(&mons[u.wormpolymorph]) && !rn2(4) ) || ( uncommon3(&mons[u.wormpolymorph]) && !rn2(3) ) || ( uncommon5(&mons[u.wormpolymorph]) && !rn2(2) ) || ( uncommon7(&mons[u.wormpolymorph]) && rn2(3) ) || ( uncommon10(&mons[u.wormpolymorph]) && rn2(5) ) || ( is_eel(&mons[u.wormpolymorph]) && rn2(20) ) ) );

			if (moves < 1000) u.polyprotected = TRUE;
			polyself(FALSE);
			u.polyprotected = FALSE;
		}

		break;
		}

	    case MISSINGNO_TRAP: {
	        char verbbuf[BUFSZ];
		seetrap(trap);
		 sprintf(verbbuf,"%s",
		    Levitation ? (const char *)"float" :
		    locomotion(youmonst.data, "step"));
		You("%s onto a missingno trap!", verbbuf);
		if(Antimagic || Unchanging) {
		    shieldeff(u.ux, u.uy);
		    You_feel("momentarily different.");
		    /* Trap did nothing; don't remove it --KAA */
		} else {

		    deltrap(trap);	/* delete trap before polymorph */
		    newsym(u.ux,u.uy);	/* get rid of trap symbol */
		    You_feel("a change coming over you.");

			u.wormpolymorph = PM_POLYMORPHED_MISSINGNO;
			reinitmissingno();

			if (moves < 1000) u.polyprotected = TRUE;
			polyself(FALSE);
			u.polyprotected = FALSE;
		}

		break;
		}

	    case LANDMINE: {

		int projectiledamage = rnd(16) + rnd(monster_difficulty() + 1);
		if (projectiledamage > 1) {
			if (u.ulevel == 1) projectiledamage /= 2;
			else if (u.ulevel == 2) {
				projectiledamage *= 2;
				projectiledamage /= 3;
			} else if (u.ulevel == 3) {
				projectiledamage *= 3;
				projectiledamage /= 4;
			} else if (u.ulevel == 4) {
				projectiledamage *= 4;
				projectiledamage /= 5;
			}
		}

		if (u.urmaxlvlUP < 5) projectiledamage /= 2;
		if (projectiledamage < 1) projectiledamage = 1; /* fail safe */

		unsigned steed_mid = 0;
		struct obj *saddle = 0;
		if ((Levitation || Flying) && !(SoiltypeEffect || u.uprops[SOILTYPE].extrinsic || have_soiltypestone() || (uarmf && uarmf->oartifact == ART_ARABELLA_S_GIRL_KICK)) ) {
		    if (!already_seen && rn2(3)) break;
		    seetrap(trap);
		    pline("%s %s in a pile of soil below you.",
			    already_seen ? "There is" : "You discover",
			    trap->madeby_u ? "the trigger of your mine" :
					     "a trigger");
		    if (already_seen && rn2(3)) break;
		    pline("KAABLAMM!!!  %s %s%s off!",
			  forcebungle ? "Your inept attempt sets" :
					"The air currents set",
			    already_seen ? a_your[trap->madeby_u] : "",
			    already_seen ? " land mine" : "it");
		} else {
		    /* prevent landmine from killing steed, throwing you to
		     * the ground, and you being affected again by the same
		     * mine because it hasn't been deleted yet
		     */
		    static boolean recursive_mine = FALSE;

		    if (recursive_mine) break;
		    seetrap(trap);
		    pline("KAABLAMM!!!  You triggered %s land mine!",
					    a_your[trap->madeby_u]);
		    if (u.usteed) steed_mid = u.usteed->m_id;
		    recursive_mine = TRUE;
		    (void) steedintrap(trap, (struct obj *)0);
		    recursive_mine = FALSE;
		    saddle = sobj_at(LEATHER_SADDLE,u.ux, u.uy);
		    if (!saddle) saddle = sobj_at(INKA_SADDLE,u.ux, u.uy);
		    if (!saddle) saddle = sobj_at(TANK_SADDLE,u.ux, u.uy);
		    if (!saddle) saddle = sobj_at(BARDING,u.ux, u.uy);
		    if (!saddle) saddle = sobj_at(MESH_SADDLE,u.ux, u.uy);
		    set_wounded_legs(LEFT_SIDE, HWounded_legs + rn1(35, 41));
		    set_wounded_legs(RIGHT_SIDE, HWounded_legs + rn1(35, 41));
		    exercise(A_DEX, FALSE);
		}
		blow_up_landmine(trap);
		if (steed_mid && saddle && !u.usteed)
			(void)keep_saddle_with_steedcorpse(steed_mid, fobj, saddle);
		newsym(u.ux,u.uy);		/* update trap symbol */
		losehp(projectiledamage, "land mine", KILLED_BY_AN);
		/* fall recursively into the pit... */
		if ((trap = t_at(u.ux, u.uy)) != 0) dotrap(trap, RECURSIVETRAP);
		fill_pit(u.ux, u.uy);
		break;
	    }

	    case BOMB_TRAP:
		{
			int projectiledamage = rnd(36) + rnd( (monster_difficulty() * 3) + 1);

			if (u.urmaxlvlUP < 5) projectiledamage = rnd(16) + rnd(monster_difficulty() + 1);

			if (projectiledamage > 1) {
				if (u.ulevel == 1) projectiledamage /= 2;
				else if (u.ulevel == 2) {
					projectiledamage *= 2;
					projectiledamage /= 3;
				} else if (u.ulevel == 3) {
					projectiledamage *= 3;
					projectiledamage /= 4;
				} else if (u.ulevel == 4) {
					projectiledamage *= 4;
					projectiledamage /= 5;
				}
			}

			int i, j, bd = (1 + (monster_difficulty() / 10) );
			pline("KAABLAMM!!!  You triggered a bomb!");
			newsym(u.ux,u.uy);		/* update trap symbol */
			losehp(projectiledamage, "hidden bomb", KILLED_BY_AN);
			deltrap(trap);
			/* fall recursively into the pit... */

		    for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
				if (!isok(u.ux + i, u.uy + j)) continue;
				if (levl[u.ux + i][u.uy + j].typ != ROOM && levl[u.ux + i][u.uy + j].typ != CORR) continue;					if (t_at(u.ux + i, u.uy + j)) continue;
			maketrap(u.ux + i, u.uy + j, PIT, 0, FALSE);
		    }
		if ((trap = t_at(u.ux, u.uy)) != 0) dotrap(trap, RECURSIVETRAP);

		break;
		}

	    case ROLLING_BOULDER_TRAP: {
		int style = ROLL | (trap->tseen ? LAUNCH_KNOWN : 0);

		seetrap(trap);
		pline("Click! You trigger a rolling boulder trap!");
		if(!launch_obj(BOULDER, trap->launch.x, trap->launch.y,
		      trap->launch2.x, trap->launch2.y, style)) {
		    deltrap(trap);
		    newsym(u.ux,u.uy);	/* get rid of trap symbol */
		    pline("Fortunately for you, no boulder was released.");
		}
		break;
	    }
	    case MAGIC_PORTAL:

		{
		boolean greencrossworks = TRUE;

		seetrap(trap);

		if (u.stairscumslowing && !(u.uhave.amulet && In_endgame(&u.uz))) {
			pline("This portal is currently deactivated and will become active in %d turn%s.", u.stairscumslowing, u.stairscumslowing > 1 ? "s" : "");
			break;
		}

		if (flags.wonderland && !(u.wonderlandescape) && In_yendorian(&u.uz) && depth(&u.uz) == 100) {
			pline("Congratulations!!! You escaped the Yendorian Tower and regain the ability to level teleport!");

			make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

			(void) safe_teleds(FALSE);
			goto_level(&medusa_level, TRUE, FALSE, FALSE);

			register int newlevX = 1;
			d_level newlevelX;
			get_level(&newlevelX, newlevX);
			goto_level(&newlevelX, TRUE, FALSE, FALSE);

			u.wonderlandescape = 1;
			break;
		}

		if (at_dgn_entrance("Green Cross") && !u.greencrossopen) {
			switch (Role_switch) {
				case PM_PREVERSIONER:
				case PM_SPACEWARS_FIGHTER:
				case PM_CAMPERSTRIKER:
					break; /* always open for those roles */
				case PM_GANG_SCHOLAR:
				case PM_WALSCHOLAR:
					if (u.greencrosschance > 4) {
						greencrossworks = FALSE;
					}
					break;
				default:
					if (u.greencrosschance > 1) {
						greencrossworks = FALSE;
					}
					break;
			}
		}

		if (u.preversionmode && !(u.preversionescape) && In_greencross(&u.uz) && (dunlev(&u.uz) == 1)) {
			pline("You trigger a magic portal.");
			pline("This exit portal is closed until you've been to the very bottom of the Green Cross dungeon. Finish the pre-alpha version of this game first before you can play the full game!");
			break;
		}

		if (uwep && uwep->oartifact == ART_ATTIC_CODE) greencrossworks = TRUE;
		if (uarm && uarm->oartifact == ART_SECRET_COURSE_VACUUM_CLEAN) greencrossworks = TRUE;
		if (uarmf && uarmf->otyp == OVER_SHOES) greencrossworks = TRUE;

		if (!greencrossworks) {
			pline("You trigger a magic portal.");
			pline("But apparently, the secret entrance isn't open today. :(");
			break;
		}

		if (at_dgn_entrance("The Subquest") && !u.prematuresubquest && !u.silverbellget && !(uarmf && uarmf->otyp == OVER_SHOES)) {
			pline("You trigger a magic portal.");
			pline("The power of your nemesis is keeping this portal closed...");
			break;
		}

		if (at_dgn_entrance("Rival Quest") && !u.silverbellget && !(uarmf && uarmf->otyp == OVER_SHOES)) {
			pline("You trigger a magic portal.");
			pline("The power of your nemesis is keeping this portal closed...");
			break;
		}

		if (at_dgn_entrance("Yendorian Tower") && !u.prematureyendortower && !(uarmf && uarmf->otyp == OVER_SHOES) && !achieve.get_amulet) {
			pline("You trigger a magic portal.");
			pline("As long as the Amulet of Yendor has never been in your possession, this portal will not function.");
			break;
		}

		if (u.usteed && (Is_blackmarket(&trap->dst) || Is_blackmarket(&u.uz)))
		    pline("%s seems to shimmer for a moment.",
			  Monnam(u.usteed));
		else

		/* In Soviet Russia, magic portals have to always work. The government does not tolerate faulty portals.
		 * And of course the government does not think portal-hopping (which is analogous to stairdancing, but that
		 * is reigned in by my stairs trap code) is cheating in any way, no, for them it's completely legal to
		 * lure out the Ludios soldiers one by one. Sigh. --Amy */

		if (!rn2(10) && !issoviet && (In_Devnull(&u.uz) || In_greencross(&u.uz) || Is_blackmarket(&u.uz) || !strcmp(dungeons[u.uz.dnum].dname, "Fort Ludios") || !strcmp(dungeons[u.uz.dnum].dname, "Lawful Quest") || !strcmp(dungeons[u.uz.dnum].dname, "Neutral Quest") || !strcmp(dungeons[u.uz.dnum].dname, "Chaotic Quest") || !strcmp(dungeons[u.uz.dnum].dname, "Yendorian Tower") || !strcmp(dungeons[u.uz.dnum].dname, "The Subquest") || !strcmp(dungeons[u.uz.dnum].dname, "Rival Quest") || In_quest(&u.uz)) ) {
			pline("You trigger a magic portal, but it malfunctions!");
			pushplayer(TRUE);
		} else if (rn2(3) && !issoviet && (In_Devnull(&u.uz) || In_greencross(&u.uz) || Is_blackmarket(&u.uz) || !strcmp(dungeons[u.uz.dnum].dname, "Fort Ludios") || !strcmp(dungeons[u.uz.dnum].dname, "Lawful Quest") || !strcmp(dungeons[u.uz.dnum].dname, "Neutral Quest") || !strcmp(dungeons[u.uz.dnum].dname, "Chaotic Quest") || !strcmp(dungeons[u.uz.dnum].dname, "Yendorian Tower") || !strcmp(dungeons[u.uz.dnum].dname, "The Subquest") || !strcmp(dungeons[u.uz.dnum].dname, "Rival Quest") || In_quest(&u.uz)) ) {
			pline("You trigger a magic portal, but it doesn't seem to work!");
		} else
		domagicportal(trap);
		break;

		 case SPEAR_TRAP:
		seetrap(trap);
		pline("A spear stabs up from a hole in the ground at you!");
		if (thick_skinned(youmonst.data) || (uwep && uwep->oartifact == ART_ETRUSCIAN_SWIMMING_LESSON) || (uwep && uwep->oartifact == ART_PATRICIA_S_FEMININITY) || (uarms && uarms->oartifact == ART_FETTIS_SLOT) || FemtrapActivePatricia || (uarmf && uarmf->oartifact == ART_ANTJE_S_POWERSTRIDE) || (uarmf && uarmf->oartifact == ART_THICK_FARTING_GIRL) || Race_if(PM_DUTHOL) ) {
			pline("But it breaks off against your body.");
			deltrap(trap);
		} else if (Levitation && !(SoiltypeEffect || u.uprops[SOILTYPE].extrinsic || have_soiltypestone() || (uarmf && uarmf->oartifact == ART_ARABELLA_S_GIRL_KICK)) ) {
			pline("The spear isn't long enough to reach you.");
		} else if (unsolid(youmonst.data)) {
			pline("It passes right through you!");
		} else {
			int projectiledamage = rnd(10)+ rnd(monster_difficulty() + 1);
			if (projectiledamage > 1) {
				if (u.ulevel == 1) projectiledamage /= 2;
				else if (u.ulevel == 2) {
					projectiledamage *= 2;
					projectiledamage /= 3;
				} else if (u.ulevel == 3) {
					projectiledamage *= 3;
					projectiledamage /= 4;
				} else if (u.ulevel == 4) {
					projectiledamage *= 4;
					projectiledamage /= 5;
				}
			}

			pline("Ouch! That hurts!");
			losehp(projectiledamage,"sharpened bamboo stick",KILLED_BY_AN);
		}
		break;

		}

		 case SCYTHING_BLADE:
		seetrap(trap);

		if (unsolid(youmonst.data)) {
			pline("A blade swings through your body.");
		} else if (!rn2(4)) {
			pline("You are barely missed by a scything blade!");
		} else {
			int projectiledamage = rnd(9)+ rnd( (monster_difficulty() / 2) + 1 + ((has_head(youmonst.data) && !Role_if(PM_COURIER)) ? 10 : 0));
			if (projectiledamage > 1) {
				if (u.ulevel == 1) projectiledamage /= 2;
				else if (u.ulevel == 2) {
					projectiledamage *= 2;
					projectiledamage /= 3;
				} else if (u.ulevel == 3) {
					projectiledamage *= 3;
					projectiledamage /= 4;
				} else if (u.ulevel == 4) {
					projectiledamage *= 4;
					projectiledamage /= 5;
				}
			}

			pline("You are hit by a scything blade!");
			losehp(projectiledamage,"scything blade",KILLED_BY_AN);
		}
		break;

		 case SPINED_BALL_TRAP:
		seetrap(trap);

		if (unsolid(youmonst.data)) {
			pline("A spined ball swings through your body.");
		} else {
			int projectiledamage = rnd(12)+ rnd( (monster_difficulty() / 2) + 1);
			if (projectiledamage > 1) {
				if (u.ulevel == 1) projectiledamage /= 2;
				else if (u.ulevel == 2) {
					projectiledamage *= 2;
					projectiledamage /= 3;
				} else if (u.ulevel == 3) {
					projectiledamage *= 3;
					projectiledamage /= 4;
				} else if (u.ulevel == 4) {
					projectiledamage *= 4;
					projectiledamage /= 5;
				}
			}

			pline("You are hit by a spined ball!");
			losehp(projectiledamage,"spined ball",KILLED_BY_AN);
		}
		break;

		 case PENDULUM_TRAP:
		seetrap(trap);

		if (unsolid(youmonst.data)) {
			pline("A pendulum swings through your body.");
		} else {
			int projectiledamage = rnd(6)+ rnd( (monster_difficulty() / 2) + 1);
			if (projectiledamage > 1) {
				if (u.ulevel == 1) projectiledamage /= 2;
				else if (u.ulevel == 2) {
					projectiledamage *= 2;
					projectiledamage /= 3;
				} else if (u.ulevel == 3) {
					projectiledamage *= 3;
					projectiledamage /= 4;
				} else if (u.ulevel == 4) {
					projectiledamage *= 4;
					projectiledamage /= 5;
				}
			}

			pline("You are hit by a pendulum!");
			losehp(projectiledamage,"pendulum",KILLED_BY_AN);
			pushplayer(TRUE);
		}
		break;

	    case TURN_TABLE:
		if ( ((Levitation || Flying) && !(SoiltypeEffect || u.uprops[SOILTYPE].extrinsic || have_soiltypestone() || (uarmf && uarmf->oartifact == ART_ARABELLA_S_GIRL_KICK)) ) || unsolid(youmonst.data)) break;

		i = rn2(15);
		if (trap->once && i < 1) {
			You("sense words of ouija board...");
			display_nhwindow(WIN_MESSAGE, FALSE);
			enlightenment(0, 1);
			exercise(A_WIS, TRUE);
			pline_The("feeling subsides.");
		    deltrap(trap);
		    newsym(u.ux, u.uy);
		    break;
	      }

		trap->once = 1;
		seetrap(trap);

		{
		    const char *message;
			const char *message2;
			static const char *confmsg1[] = {
			"feel that you ride x128 speed coffee cup!",
			"announced triple Axel. It's regrettable -- landing failure.",
			"are put on the turning umbrella!",
			"change your whole body to drill!",
			};
			static const char *confmsg2[] = {
			"The world turns and turns and turns but it turns too much? Ah-Ha...",
			"The judgment is shocking to you, and you are crowded.",
			"You are crowded feebly.",
			"has the belt pulled! Oh-No-(turn and turn)",
			};
			if (Confusion || FunnyHallu) {
			    int mess_num = rn2(SIZE(confmsg1));
			    message = confmsg1[mess_num];
			    message2 = confmsg2[mess_num];
			} else {
			    message = "spin high speed accidentally!";
			    message2 = "You stagger...";
			}
			You("%s", message);
			pline("%s", message2);
			if (i < 6) pushplayer(TRUE);
		    make_stunned(HStun + rn1(12, 5), FALSE);
		}

		break;

	    case SCENT_TRAP:

		i = rn2(15);
		if (trap->once && i < 1) {
		    You_hear("a soft blowing.");	/*"a soft blowing."*/
		    deltrap(trap);
		    newsym(u.ux, u.uy);
		    break;
		}

		seetrap(trap);

	      {

		int projectiledamage = rnd(20)+ rnd( (monster_difficulty() ) + 1);
		if (projectiledamage > 1) {
			if (u.ulevel == 1) projectiledamage /= 2;
			else if (u.ulevel == 2) {
				projectiledamage *= 2;
				projectiledamage /= 3;
			} else if (u.ulevel == 3) {
				projectiledamage *= 3;
				projectiledamage /= 4;
			} else if (u.ulevel == 4) {
				projectiledamage *= 4;
				projectiledamage /= 5;
			}
		}

		boolean youbreath = !breathless(youmonst.data);
		boolean youghoul = (saprovorousnumber(u.umonnum) || (Race_if(PM_GASTLY) && !Upolyd) || (Race_if(PM_PLAYER_SKELETON) && !Upolyd) || (Race_if(PM_PHANTOM_GHOST) && !Upolyd) );
		trap->once = 1;
		if (youbreath)
		    You("smell sweet scent...");
		switch (i) {

			case 1: /* tainted */

				if (youghoul) {
					pline("Yum - something well aged.");
				} else if (youbreath) {
					pline("Ulch - something %s!", rn2(2) ? "like excrement" : "tainted");
				      make_vomiting(Vomiting+20, TRUE);
					if (Sick && Sick < 100)	set_itimeout(&Sick, (Sick * 2) + 10);
				}
				break;
			case 2: /* incense */

				if (youbreath && !youghoul) {
					pline_The("incense dyed your body.");
				} else if (youghoul) {
					pline("Ulch - terrible smell!");
				      make_vomiting(Vomiting+20, TRUE);
					if (Sick && Sick < 100)	set_itimeout(&Sick, (Sick * 2) + 10);
				}
			      (void) adjattrib(A_CHA, !youghoul ? 1 : -1, FALSE, TRUE);

				break;
			case 3: /* holy incense */

				if(is_undead(youmonst.data) || hates_silver(youmonst.data) || (uarmf && uarmf->oartifact == ART_IRIS_S_HIDDEN_ALLERGY) || (uarmh && uarmh->oartifact == ART_IRIS_S_SECRET_VULNERABILIT) || (uarmc && uarmc->oartifact == ART_IRIS_S_UNREVEALED_LOVE) || (uarmg && uarmg->oartifact == ART_IRIS_S_FAVORED_MATERIAL) || autismweaponcheck(ART_PORKMAN_S_BALLS_OF_STEEL)) {

					pline("Eek - this smells like %s!", FunnyHallu ? "priest's fart" : "exocism incense");
					losehp(projectiledamage,"holy incense",KILLED_BY);

				} else if(youbreath) {
					if (!rn2(3)) {
						You_feel("relaxed with the holy incense.");
						adjalign(5);
					} else {
						pline("Oh, it's incense of ritual.");
					}
				}

				break;
			case 4: /* mosquito incense */

				if (youbreath) {
				    You("are suffering from smoke of mosquito-incense!");
					losehp(projectiledamage,"mosquito incense",KILLED_BY);
				      make_vomiting(Vomiting+20, TRUE);
					if (Sick && Sick < 100)	set_itimeout(&Sick, (Sick * 2) + 10);

				} else {
					pline("It's mosquito-incense.");

				}

				break;
			case 5: /* holyflowers */

				if (u.ualign.type == A_CHAOTIC) {

					pline("Eek - this smells like holy flowers!");
					losehp(projectiledamage,"holy flowers",KILLED_BY);

				} else if (youbreath) {
				    You_feel("relaxed with the smell of flowers...");
					make_stunned(0L, TRUE);

				} else {
					You_feel("so relaxed that you decide to take a nap.");
					nomul(-5, "relaxed with a smell", TRUE);
					nomovemsg = "You are conscious again.";

				}

				break;
			case 6: /* poisonous flowers */
				if (youbreath) {
				    You_feel("relaxed with the smell of flowers...");
				    pline("No, it's poisonous flowers!");
				    poisoned("smell", A_STR, "smell of poisonous flowers", 5);
				}

				break;
			case 7: /* onion */

				pline_The("onion infiltrates your %s.", body_part(EYE));
				    make_blinded(Blinded+rnd(10 + monster_difficulty() ) ,FALSE);

				break;
			case 8: /* Chanel No. 5 */

				pline("Whoa! Sweet fragrant perfume! You get ready for sex...");
				if (uarmc) remove_worn_item(uarmc, TRUE);
				if (uarm) remove_worn_item(uarm, TRUE);
				if (uarmu) remove_worn_item(uarmu, TRUE);
				if (uarms) remove_worn_item(uarms, TRUE);
				if (uarmf) remove_worn_item(uarmf, TRUE);
				if (uarmg) remove_worn_item(uarmg, TRUE);
				if (uarmh) remove_worn_item(uarmh, TRUE);
				nomul(-5, "dreaming of lovely girls", TRUE);
				nomovemsg = "You wake up and discover you're naked.";

				break;
			case 9: /* dinner */

				if (youbreath) {
				    pline("It is sweet smell of gorgeous dinners.");
				    if (u.uhunger <= 0) {
					u.youaredead = 1;
					u.uhs = 6; /* STARVED */
					flags.botl = 1;
					bot();
				    You(FunnyHallu ? "fired the last match, and ascended with your grandmother..." : "faint from starvation, and died...");
					killer_format = KILLED_BY;
					killer = "being caught in smell of gorgeous dinners which can't get";
					done(DIED);
					u.youaredead = 0;
				    } else {
					You_feel("hungry%s.", (u.uhunger > 500) ? " a bit" : "");
					if (u.uhunger > 500) u.uhunger = 500;
					else if (u.uhunger > 200) u.uhunger = 200;
					else u.uhunger -= rnd(StrongFull_nutrient ? 100 : Full_nutrient ? 200 : 400);

				    }

				}

				break;
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
			default:
				/* do nothing */
				break;

		} /* switch i */

		}

		break;

	    case BANANA_TRAP:
		if ((Levitation || Flying) && !(SoiltypeEffect || u.uprops[SOILTYPE].extrinsic || have_soiltypestone()) ) break;
		trap->once = 1;
		seetrap(trap);

		if (amorphous(youmonst.data) || is_whirly(youmonst.data) || unsolid(youmonst.data)) {
		    if (Blind)
			You("trample on %s.", something);
		    else
			You("trample on a banana peel, but don't slip.");
		    break;
		}
		else
		    You("trample on a banana peel and slip your %s!", body_part(FOOT));
		exercise(A_DEX, FALSE);

		break;

	    case FALLING_TUB_TRAP:

		if (trap->once && (rn2(10) > 7)) {
			You_hear("a loud click!");	/*"a loud click!"*/
			deltrap(trap);
			newsym(u.ux, u.uy);
			break;
		}
		trap->once = 1;
		seetrap(trap);
		newsym(u.ux, u.uy);

		if ( Confusion )
			pline("Achoo!");
		pline("%s drops from above!", Blind ? "Something" : "A bathtub");

		if ( is_whirly(youmonst.data) ) {
			You("blow the bathtub away.");
			break;
		} else if ( unsolid(youmonst.data) ) {
			pline("%s passes through your body.", Blind ? "Something" : "A bathtub");
			break;
		} else if ( amorphous(youmonst.data) ) {
			pline("%s stucks onto your body.", Blind ? "Something" : "A bathtub");
			break;
		} else if ( rn2(10) > 3 ) {
			pline("Whang! It hits you%s%s!",
				has_head(youmonst.data) ? "r " : "",
				has_head(youmonst.data) ? body_part(HEAD) : "");
			if ( Confusion )
				You_hear("someone laughing.");
			if (FunnyHallu) {
				You(rn2(2) ? "saw stars!" : "can see stars yet.");
				if (Confusion)
					You_feel("cosmic lifestream.");
			}

			int projectiledamage = rnd(3)+ rnd( (monster_difficulty() / 5) + 1);
			if (projectiledamage > 1) {
				if (u.ulevel == 1) projectiledamage /= 2;
				else if (u.ulevel == 2) {
					projectiledamage *= 2;
					projectiledamage /= 3;
				} else if (u.ulevel == 3) {
					projectiledamage *= 3;
					projectiledamage /= 4;
				} else if (u.ulevel == 4) {
					projectiledamage *= 4;
					projectiledamage /= 5;
				}
			}

			losehp(projectiledamage,"falling bathtub",KILLED_BY_AN);
	
			/*from NetHack brass 08.03.25, and arrange [Sakusha]*/
			if(!rn2(5)) {
				pline("The water splashes all over you!");
				if ((!StrongSwimming || !rn2(10)) && (!StrongMagical_breathing || !rn2(10))) {
					water_damage(invent, FALSE, FALSE);
				}
			}
			make_stunned(HStun + rn1(14, 32), TRUE);

		} else {
			if (Blind)
				You_hear("metallic sound nearby.");
			else
				You("avoid the falling bathtub.");
		}

		break;

	    case ALARM:
		pline((FunnyHallu || Confusion) ?	"Oops! Alarm:" : "Oops! Here is alarm!");
		trap->once = 1;
		seetrap(trap);
		aggravate();

		break;

	    case CALTROPS_TRAP:
		pline("You stepped into a caltrop!");
		seetrap(trap);

		if (uarmf && !rn2(3)) {
			pline("But it isn't long enough to pierce through your boot.");
			break;
		}

		if (!rn2(2)) set_wounded_legs(LEFT_SIDE, HWounded_legs + rn1(35, 41));
		else set_wounded_legs(RIGHT_SIDE, HWounded_legs + rn1(35, 41));

		{
			int projectiledamage = rnd(2)+ rnd( (monster_difficulty() / 10) + 1);
			if (projectiledamage > 1) {
				if (u.ulevel == 1) projectiledamage /= 2;
				else if (u.ulevel == 2) {
					projectiledamage *= 2;
					projectiledamage /= 3;
				} else if (u.ulevel == 3) {
					projectiledamage *= 3;
					projectiledamage /= 4;
				} else if (u.ulevel == 4) {
					projectiledamage *= 4;
					projectiledamage /= 5;
				}
			}

			losehp(projectiledamage,"caltrop trap",KILLED_BY_AN);

		}

		if (!inertiaprotection()) {
			u.inertia += (rnd(4) + rnd( (monster_difficulty() / 6) + 1));
		}

		break;

	    case BLADE_WIRE:
		pline("You trip over a wire!");
		seetrap(trap);

		if (rn2(4)) {
			int paralysistime = (monster_difficulty() / 10) + 1;
			if (!isstunfish && (paralysistime > 1)) paralysistime = rnd(paralysistime);
			if (paralysistime > 10) {
				while (rn2(5) && (paralysistime > 10)) {
					paralysistime--;
				}
			}

			pline("You fall down.");
			nomul(-(2 + paralysistime) , "helpless from tripping over a wire", TRUE);
			nomovemsg = "You get up again.";
		}
		if (rn2(4)) {
			pline("It slits your %s!", makeplural(body_part(LEG)) );

			int projectiledamage = rnd(7)+ rnd( (monster_difficulty() / 5) + 1);
			if (projectiledamage > 1) {
				if (u.ulevel == 1) projectiledamage /= 2;
				else if (u.ulevel == 2) {
					projectiledamage *= 2;
					projectiledamage /= 3;
				} else if (u.ulevel == 3) {
					projectiledamage *= 3;
					projectiledamage /= 4;
				} else if (u.ulevel == 4) {
					projectiledamage *= 4;
					projectiledamage /= 5;
				}
			}

			losehp(projectiledamage,"blade wire",KILLED_BY_AN);
		}

		break;

	    case MAGNET_TRAP:

		pline("CLICK! You have triggered a trap!");
		pline("A magnet attracts your metallic equipment!");
		seetrap(trap);

		{
			register struct obj *otmp, *otmp2;

			for(otmp2 = otmp =invent; otmp2 ; otmp = otmp2) {
				otmp2 = otmp->nobj;
				if (objects[(otmp)->otyp].oc_material == MT_IRON ) {

					if (otmp->owornmask & W_ARMOR) {
					    if (otmp == uskin) {
						skinback(TRUE);		/* uarm = uskin; uskin = 0; */
					    }
					    if (otmp == uarm) (void) Armor_off();
					    else if (otmp == uarmc) (void) Cloak_off();
					    else if (otmp == uarmf) (void) Boots_off();
					    else if (otmp == uarmg) (void) Gloves_off();
					    else if (otmp == uarmh) (void) Helmet_off();
					    else if (otmp == uarms) (void) Shield_off();
					    else if (otmp == uarmu) (void) Shirt_off();
					    /* catchall -- should never happen */
					    else setworn((struct obj *)0, otmp ->owornmask & W_ARMOR);
					} else if (otmp ->owornmask & W_AMUL) {
					    Amulet_off();
					} else if (otmp ->owornmask & W_IMPLANT) {
					    Implant_off();
					} else if (otmp ->owornmask & W_RING) {
					    Ring_gone(otmp);
					} else if (otmp ->owornmask & W_TOOL) {
					    Blindf_off(otmp);
					} else if (otmp ->owornmask & (W_WEP|W_SWAPWEP|W_QUIVER)) {
					    if (otmp == uwep)
						uwepgone();
					    if (otmp == uswapwep)
						uswapwepgone();
					    if (otmp == uquiver)
						uqwepgone();
					}
	
					if (otmp->owornmask & (W_BALL|W_CHAIN)) {
					    unpunish();
					} else if (otmp->owornmask) {
					/* catchall */
					    setnotworn(otmp);
					}

				dropx(otmp);

				}
			}
		}

		scatter(u.ux,u.uy,4,VIS_EFFECTS|MAY_HIT|MAY_DESTROY|MAY_FRACTURE,(struct obj*)0);

		break;

	    case OUTTA_DEPTH_TRAP:
		deltrap(trap);
		pline("CLICK! You have triggered a trap!");
		u.outtadepthtrap = 1;

		if (Aggravate_monster) {
			u.aggravation = 1;
		}

		reset_rndmonst(NON_PM);
		int aggroamount = rnd(6);
		if (isfriday) aggroamount *= 2;

		while (aggroamount) {
			u.cnd_aggravateamount++;
			makemon((struct permonst *)0, u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			aggroamount--;
			if (aggroamount < 0) aggroamount = 0;
		}

		pline("Several monsters come out of a portal.");

		u.outtadepthtrap = 0;
		u.aggravation = 0;

		break;

	    case PUNISHMENT_TRAP:

		deltrap(trap);
		pline("CLICK! You have triggered a trap!");
		punishx();
		break;

	    case STREW_TRAP:

		pline("CLICK! You have triggered a trap!");
		pline("You are greatly startled by a sudden sound.");
		seetrap(trap);
		if (!rn2(10)) deltrap(trap);

		{
			register struct obj *otmp, *otmp2;

			for(otmp2 = otmp =invent; otmp2 ; otmp = otmp2) {
				otmp2 = otmp->nobj;
				if (!rn2(5) ) {

					if (otmp->owornmask & W_ARMOR) {
					    if (otmp == uskin) {
						skinback(TRUE);		/* uarm = uskin; uskin = 0; */
					    }
					    if (otmp == uarm) (void) Armor_off();
					    else if (otmp == uarmc) (void) Cloak_off();
					    else if (otmp == uarmf) (void) Boots_off();
					    else if (otmp == uarmg) (void) Gloves_off();
					    else if (otmp == uarmh) (void) Helmet_off();
					    else if (otmp == uarms) (void) Shield_off();
					    else if (otmp == uarmu) (void) Shirt_off();
					    /* catchall -- should never happen */
					    else setworn((struct obj *)0, otmp ->owornmask & W_ARMOR);
					} else if (otmp ->owornmask & W_AMUL) {
					    Amulet_off();
					} else if (otmp ->owornmask & W_IMPLANT) {
					    Implant_off();
					} else if (otmp ->owornmask & W_RING) {
					    Ring_gone(otmp);
					} else if (otmp ->owornmask & W_TOOL) {
					    Blindf_off(otmp);
					} else if (otmp ->owornmask & (W_WEP|W_SWAPWEP|W_QUIVER)) {
					    if (otmp == uwep)
						uwepgone();
					    if (otmp == uswapwep)
						uswapwepgone();
					    if (otmp == uquiver)
						uqwepgone();
					}
	
					if (otmp->owornmask & (W_BALL|W_CHAIN)) {
					    unpunish();
					} else if (otmp->owornmask) {
					/* catchall */
					    setnotworn(otmp);
					}

				dropx(otmp);

				}
			}
		}

		scatter(u.ux,u.uy,4,VIS_EFFECTS|MAY_HIT|MAY_DESTROY|MAY_FRACTURE,(struct obj*)0);

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		(void) makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
		(void) makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
		(void) makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
		(void) makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
		(void) makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
		(void) makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
		(void) makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
		(void) makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);

		u.aggravation = 0;

		break;

	    case SLINGSHOT_TRAP:

		if (trap->once && !rn2(15)) {
		    You_hear("a loud click!");
		    deltrap(trap);
		    newsym(u.ux,u.uy);
		    break;
		}
		trap->once = 1;
		seetrap(trap);
		pline("A stone shoots out at you!");
		otmp = mksobj(ROCK, TRUE, FALSE, FALSE);
		if (otmp) {
			int projectiledamage = dmgval(otmp, &youmonst);
			if (projectiledamage > 1) {
				if (u.ulevel == 1) projectiledamage /= 2;
				else if (u.ulevel == 2) {
					projectiledamage *= 2;
					projectiledamage /= 3;
				} else if (u.ulevel == 3) {
					projectiledamage *= 3;
					projectiledamage /= 4;
				} else if (u.ulevel == 4) {
					projectiledamage *= 4;
					projectiledamage /= 5;
				}
			}

			otmp->quan = 1L;
			otmp->owt = weight(otmp);

			if (thitu(5 + rnd((monster_difficulty() / 3) + 1), projectiledamage + rnd((monster_difficulty() / 3) + 1), otmp, "stone")) {
			    obfree(otmp, (struct obj *)0);
			} else if (!timebasedlowerchance()) {
			    obfree(otmp, (struct obj *)0);
			} else {
			    place_object(otmp, u.ux, u.uy);
			    if (!Blind) otmp->dknown = 1;
			    stackobj(otmp);
			    newsym(u.ux, u.uy);
			}
		}

		break;

	    case CANNON_TRAP:

		seetrap(trap);

		pline("CLICK! You have triggered a trap!");
		pline("Suddenly, you're hit by an enormous cannonball!");

		{
			int projectiledamage = rnd(24) + rnd( (monster_difficulty() * 2) + 1);
			if (u.urmaxlvlUP < 5) projectiledamage = rnd(12) + rnd( (monster_difficulty()) + 1);
			if (projectiledamage > 1) {
				if (u.ulevel == 1) projectiledamage /= 2;
				else if (u.ulevel == 2) {
					projectiledamage *= 2;
					projectiledamage /= 3;
				} else if (u.ulevel == 3) {
					projectiledamage *= 3;
					projectiledamage /= 4;
				} else if (u.ulevel == 4) {
					projectiledamage *= 4;
					projectiledamage /= 5;
				}
			}

			losehp(projectiledamage,"cannonball",KILLED_BY_AN);
		}

		break;

	    case VENOM_SPRINKLER:

		seetrap(trap);

		if (!rn2(2)) {
			int blindtime = rnd(25 + rnd(monster_difficulty() ) );
			pline("You hear a splash, and something hits you right in the %s.", body_part(FACE));
			u.ucreamed += blindtime;
			make_blinded(Blinded + (long)blindtime, FALSE);

		} else {
			pline("You hear a splash, and are covered in acid!");
			if ((Acid_resistance && rn2(StrongAcid_resistance ? 20 : 5)) || AcidImmunity) {
				pline("But it seems harmless.");
			} else {
				int projectiledamage = rnd(6)+ rnd( (monster_difficulty() / 3) + 1);
				if (projectiledamage > 1) {
					if (u.ulevel == 1) projectiledamage /= 2;
					else if (u.ulevel == 2) {
						projectiledamage *= 2;
						projectiledamage /= 3;
					} else if (u.ulevel == 3) {
						projectiledamage *= 3;
						projectiledamage /= 4;
					} else if (u.ulevel == 4) {
						projectiledamage *= 4;
						projectiledamage /= 5;
					}
				}

				losehp(projectiledamage,"sprinkled acid venom",KILLED_BY_AN);
				if(!rn2(3)) erode_armor(&youmonst, TRUE);
			}
		}

		break;

	    case BOWEL_CRAMPS_TRAP:

		seetrap(trap);

		pline("You stepped on a trigger!");

		if (!Vomiting) {
			make_vomiting(Vomiting+d(10,4), TRUE);
			pline("You feel nauseated.");
			if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10);
		} else {
			pline("Suddenly your tummy aches terribly!");
			if (isstunfish) nomul(-rnz(5), "having bowel cramps", TRUE);
			else nomul(-rnd(5), "having bowel cramps", TRUE);
		}

		break;

	    case UNEQUIPPING_TRAP:

		pline("CLICK! You have triggered a trap!");
		deltrap(trap); /* only triggers once, because otherwise you could easily get rid of cursed stuff --Amy */

		pline("You are completely startled by a sudden sound.");

		if (uwep) setnotworn(uwep);
		if (uswapwep) uswapwepgone();
		if (ublindf) remove_worn_item(ublindf, TRUE);
		if (uright) remove_worn_item(uright, TRUE);
		if (uleft) remove_worn_item(uleft, TRUE);
		if (uamul) remove_worn_item(uamul, TRUE);
		if (uimplant) remove_worn_item(uimplant, TRUE);
		if (uarmf) remove_worn_item(uarmf, TRUE);
		if (uarmg) remove_worn_item(uarmg, TRUE);
		if (uarmh) remove_worn_item(uarmh, TRUE);
		if (uarmc) remove_worn_item(uarmc, TRUE);
		if (uarms) remove_worn_item(uarms, TRUE);
		if (uarmu) remove_worn_item(uarmu, TRUE);
		if (uskin) skinback(FALSE);
		if (uarm) remove_worn_item(uarm, TRUE);

		break;

	    case NEXUS_TRAP:

		pline("Uh-oh, should have watched your step...");

		seetrap(trap);
		if (!rn2(15)) deltrap(trap);

		switch (rnd(7)) {

			case 1:
			case 2:
			case 3:
				pline("You are teleported by nexus forces!");
				teleX();
				break;
			case 4:
			case 5:
				pline("You are pushed around by nexus forces!");
				phase_door(0);
				break;
			case 6:

				if (!playerlevelportdisabled() ) {
					make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

					if (!u.levelporting) {
						u.levelporting = 1;
						nomul(-2, "being levelported", FALSE); /* because it's not called until you get another turn... */
					}
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

	    case FUMAROLE:
	    case MONSTER_CUBE:
	    case CURSED_GRAVE:

		break;

		 case GUILLOTINE_TRAP:
		seetrap(trap);

		if (!rn2(20) && has_head(youmonst.data) && !Role_if(PM_COURIER) ) {
			pline("You are decapitated by a guillotine!");
			losehp(2 * (Upolyd ? u.mh : u.uhp) + 200, "guillotine trap",KILLED_BY_AN);
		} else {
			int projectiledamage = rnd(20)+ rnd( (monster_difficulty() * 2) + 1 + ((has_head(youmonst.data) && !Role_if(PM_COURIER)) ? 40 : 0));
			if (projectiledamage > 1) {
				if (u.ulevel == 1) projectiledamage /= 2;
				else if (u.ulevel == 2) {
					projectiledamage *= 2;
					projectiledamage /= 3;
				} else if (u.ulevel == 3) {
					projectiledamage *= 3;
					projectiledamage /= 4;
				} else if (u.ulevel == 4) {
					projectiledamage *= 4;
					projectiledamage /= 5;
				}
			}

			pline("You are hit by a guillotine!");
			losehp(projectiledamage,"guillotine trap",KILLED_BY_AN);
		}
		break;

		 case BISECTION_TRAP:
		seetrap(trap);

		if (!rn2(20) && !bigmonst(youmonst.data) && !StrongDiminishedBleeding && !Invulnerable && !((PlayerInBlockHeels || PlayerInWedgeHeels) && tech_inuse(T_EXTREME_STURDINESS) && !rn2(2) ) && !(Stoned_chiller && Stoned && !(u.stonedchilltimer) && !rn2(3)) ) {
			pline("You are bisected by a razor-sharp blade!");
			losehp(2 * (Upolyd ? u.mh : u.uhp) + 200, "bisection trap",KILLED_BY);
		} else {
			int projectiledamage = rnd(30)+ rnd( (monster_difficulty() * 3) + 1);
			if (projectiledamage > 1) {
				if (u.ulevel == 1) projectiledamage /= 2;
				else if (u.ulevel == 2) {
					projectiledamage *= 2;
					projectiledamage /= 3;
				} else if (u.ulevel == 3) {
					projectiledamage *= 3;
					projectiledamage /= 4;
				} else if (u.ulevel == 4) {
					projectiledamage *= 4;
					projectiledamage /= 5;
				}
			}

			pline("You are hit by a razor-sharp blade!");
			losehp(projectiledamage,"bisection trap",KILLED_BY_AN);
		}
		break;

		 case COLLAPSE_TRAP:
			pline("Rocks fall all around you!");
			deltrap(trap);
			newsym(u.ux,u.uy);
			rocks_fall(u.ux,u.uy);
		 break;

		 case UNKNOWN_TRAP: /* randomly transforms into some other trap */

			{
			boolean cangivehp = trap->giveshp;
			deltrap(trap);

			int rtrap;
		      rtrap = rndtrap();

			(void) maketrap(u.ux, u.uy, rtrap, 100, cangivehp);

			if ((trap = t_at(u.ux, u.uy)) != 0) dotrap(trap, RECURSIVETRAP);
			}

		 break;

		 case TRAP_PERCENTS: /* gives out random rumors */

			{const char *line;
			char buflin[BUFSZ];
			if (rn2(2)) line = getrumor(-1, buflin, TRUE);
			else line = getrumor(0, buflin, TRUE);
			if (!*line) line = "Slash'EM rumors file closed for renovation.";
			pline("%s", line);
			}

			if (!rn2(10)) {
			deltrap(trap);
			(void) maketrap(u.ux, u.uy, ROCKTRAP, 0, FALSE); /* no recursive trap here */
			}

		 break;

		 case LOUDSPEAKER: /* fake message */
			{
				pline("%s", fauxmessage());
				u.cnd_plineamount++;
				if (!rn2(3)) {
					pline("%s", fauxmessage());
					u.cnd_plineamount++;
				}
			}
		 break;

		 case ARABELLA_SPEAKER: /* fake message */
			{
				pline("%s", fauxmessage());
				u.cnd_plineamount++;
				if (!rn2(3)) {
					pline("%s", fauxmessage());
					u.cnd_plineamount++;
				}
			}
		 break;

		 case RMB_LOSS_TRAP:

			if (RMBLoss) break;
			u.cnd_nastytrapamount++;

			RMBLoss = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case DROP_TRAP:

			if (NoDropProblem) break;
			u.cnd_nastytrapamount++;

			NoDropProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case DSTW_TRAP:

			if (DSTWProblem) break;
			u.cnd_nastytrapamount++;

			DSTWProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case STATUS_TRAP:

			if (StatusTrapProblem) break;
			u.cnd_nastytrapamount++;

			StatusTrapProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

			if (HConfusion) set_itimeout(&HeavyConfusion, HConfusion);
			if (HStun) set_itimeout(&HeavyStunned, HStun);
			if (HNumbed) set_itimeout(&HeavyNumbed, HNumbed);
			if (HFeared) set_itimeout(&HeavyFeared, HFeared);
			if (HFrozen) set_itimeout(&HeavyFrozen, HFrozen);
			if (HBurned) set_itimeout(&HeavyBurned, HBurned);
			if (HDimmed) set_itimeout(&HeavyDimmed, HDimmed);
			if (Blinded) set_itimeout(&HeavyBlind, Blinded);
			if (HHallucination) set_itimeout(&HeavyHallu, HHallucination);

		 break;

		 case NUPESELL_TRAP: /* supposed to be impossible */

		 break;

		 case ACTIVE_SUPERSCROLLER_TRAP:

			if (Superscroller) break;

			else {
			u.cnd_nastytrapamount++;
			deltrap(trap);
			(void) maketrap(u.ux, u.uy, SUPERSCROLLER_TRAP, 0, FALSE);
			/* no recursive trap - just give the nasty effect here */
			Superscroller = rnz(nastytrapdur * (Role_if(PM_GRADUATE) ? 2 : Role_if(PM_GEEK) ? 5 : 10) * (monster_difficulty() + 1));
			(void) makemon(&mons[PM_SCROLLER_MASTER], 0, 0, NO_MINVENT);
			}

		 break;

		 case SUPERSCROLLER_TRAP:

			if (Superscroller) break;
			u.cnd_nastytrapamount++;

			Superscroller = rnz(nastytrapdur * (Role_if(PM_GRADUATE) ? 2 : Role_if(PM_GEEK) ? 5 : 10) * (monster_difficulty() + 1));
			(void) makemon(&mons[PM_SCROLLER_MASTER], 0, 0, NO_MINVENT);

		 break;

		 case MENU_TRAP:

			if (MenuBug) break;
			u.cnd_nastytrapamount++;

			MenuBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case FREE_HAND_TRAP:

			if (FreeHandLoss) break;
			u.cnd_nastytrapamount++;

			FreeHandLoss = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case UNIDENTIFY_TRAP:

			if (Unidentify) break;
			u.cnd_nastytrapamount++;

			Unidentify = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case METABOLIC_TRAP:

			if (FastMetabolismEffect) break;
			u.cnd_nastytrapamount++;

			FastMetabolismEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case TRAP_OF_NO_RETURN:

			if (NoReturnEffect) break;
			u.cnd_nastytrapamount++;

			NoReturnEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case EGOTRAP:

			if (AlwaysEgotypeMonsters) break;
			u.cnd_nastytrapamount++;

			AlwaysEgotypeMonsters = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case FAST_FORWARD_TRAP:

			if (TimeGoesByFaster) break;
			u.cnd_nastytrapamount++;

			TimeGoesByFaster = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case TRAP_OF_ROTTENNESS:

			if (FoodIsAlwaysRotten) break;
			u.cnd_nastytrapamount++;

			FoodIsAlwaysRotten = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case UNSKILLED_TRAP:

			if (AllSkillsUnskilled) break;
			u.cnd_nastytrapamount++;

			AllSkillsUnskilled = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case DESECRATION_TRAP:

			if (Desecration) break;
			u.cnd_nastytrapamount++;

			Desecration = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case VENTILATOR:

			if (FunnyHallu)
				You_hear("air current noises, and a remark by Amy about how sexy they are.");
			else
				You_hear("air current noises.");

		 break;

		 case SPACEWARS_TRAP:

			makespacewarstrap();
			while (rn2(4)) makespacewarstrap();

			(void) makemon(insidemon(), 0, 0, MM_ANGRY);
			while (rn2(5)) makemon(insidemon(), 0, 0, MM_ANGRY);

			if (!rn2(100)) pline("You see:");

			deltrap(trap); /* only triggers once */

		 break;

		 case STARVATION_TRAP:

			if (StarvationEffect) break;
			u.cnd_nastytrapamount++;

			StarvationEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case DROPLESS_TRAP:

			if (NoDropsEffect) break;
			u.cnd_nastytrapamount++;

			NoDropsEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case LOW_EFFECT_TRAP:

			if (LowEffects) break;
			u.cnd_nastytrapamount++;

			LowEffects = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case INVISIBLE_TRAP:

			if (InvisibleTrapsEffect) break;
			u.cnd_nastytrapamount++;

			InvisibleTrapsEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case GHOST_WORLD_TRAP:

			if (GhostWorld) break;
			u.cnd_nastytrapamount++;

			GhostWorld = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case DEHYDRATION_TRAP:

			if (Dehydration) break;
			u.cnd_nastytrapamount++;

			Dehydration = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case HATE_TRAP:

			if (HateTrapEffect) break;
			u.cnd_nastytrapamount++;

			HateTrapEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case TOTTER_TRAP:

			if (TotterTrapEffect) break;
			u.cnd_nastytrapamount++;

			TotterTrapEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case NONINTRINSICAL_TRAP:

			if (Nonintrinsics) break;
			u.cnd_nastytrapamount++;

			Nonintrinsics = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case DROPCURSE_TRAP:

			if (Dropcurses) break;
			u.cnd_nastytrapamount++;

			Dropcurses = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case NAKEDNESS_TRAP:

			if (Nakedness) break;
			u.cnd_nastytrapamount++;

			Nakedness = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case ANTILEVEL_TRAP:

			if (Antileveling) break;
			u.cnd_nastytrapamount++;

			Antileveling = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case STEALER_TRAP:

			if (ItemStealingEffect) break;
			u.cnd_nastytrapamount++;

			ItemStealingEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case REBELLION_TRAP:

			if (Rebellions) break;
			u.cnd_nastytrapamount++;

			Rebellions = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case CRAP_TRAP:

			if (CrapEffect) break;
			u.cnd_nastytrapamount++;

			CrapEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case MISFIRE_TRAP:

			if (ProjectilesMisfire) break;
			u.cnd_nastytrapamount++;

			ProjectilesMisfire = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case TRAP_OF_WALLS:

			if (WallTrapping) break;
			u.cnd_nastytrapamount++;

			WallTrapping = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case LOW_STATS_TRAP:

			if (AllStatsAreLower) break;
			u.cnd_nastytrapamount++;

			AllStatsAreLower = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case TRAINING_TRAP:

			if (PlayerCannotTrainSkills) break;
			u.cnd_nastytrapamount++;

			PlayerCannotTrainSkills = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case EXERCISE_TRAP:

			if (PlayerCannotExerciseStats) break;
			u.cnd_nastytrapamount++;

			PlayerCannotExerciseStats = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case LIMITATION_TRAP:

			if (TurnLimitation) break;
			u.cnd_nastytrapamount++;

			TurnLimitation = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case WEAK_SIGHT_TRAP:

			if (WeakSight) break;
			u.cnd_nastytrapamount++;

			WeakSight = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case RANDOM_MESSAGE_TRAP:

			if (RandomMessages) break;
			u.cnd_nastytrapamount++;

			RandomMessages = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case RECURRING_AMNESIA_TRAP:

			if (RecurringAmnesia) break;
			u.cnd_nastytrapamount++;

			RecurringAmnesia = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case BIGSCRIPT_TRAP:

			if (BigscriptEffect) break;
			u.cnd_nastytrapamount++;

			BigscriptEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case BANK_TRAP:

			if (BankTrapEffect) break;
			u.cnd_nastytrapamount++;

			BankTrapEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

			if (u.bankcashlimit == 0) u.bankcashlimit = rnz(1000 * (monster_difficulty() + 1 + (long)mvitals[PM_ARABELLA_THE_MONEY_THIEF].born));
			u.bankcashamount += u.ugold;
			u.ugold = 0;

		 break;

		 case MAP_TRAP:

			if (MapTrapEffect) break;
			u.cnd_nastytrapamount++;

			MapTrapEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case TECH_TRAP:

			if (TechTrapEffect) break;
			u.cnd_nastytrapamount++;

			TechTrapEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case DISENCHANT_TRAP:

			if (RecurringDisenchant) break;
			u.cnd_nastytrapamount++;

			RecurringDisenchant = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case VERISIERT:

			if (verisiertEffect) break;
			u.cnd_nastytrapamount++;

			verisiertEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case CHAOS_TRAP:

			if (ChaosTerrain) break;
			u.cnd_nastytrapamount++;

			ChaosTerrain = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case MUTENESS_TRAP:

			if (Muteness) break;
			u.cnd_nastytrapamount++;

			Muteness = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case ENGRAVING_TRAP:

			if (EngravingDoesntWork) break;
			u.cnd_nastytrapamount++;

			EngravingDoesntWork = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case MAGIC_DEVICE_TRAP:

			if (MagicDeviceEffect) break;
			u.cnd_nastytrapamount++;

			MagicDeviceEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case BOOK_TRAP:

			if (BookTrapEffect) break;
			u.cnd_nastytrapamount++;

			BookTrapEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case LEVEL_TRAP:

			if (LevelTrapEffect) break;
			u.cnd_nastytrapamount++;

			LevelTrapEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case QUIZ_TRAP:

			if (QuizTrapEffect) break;
			u.cnd_nastytrapamount++;

			QuizTrapEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case NTLL_TRAP:

		 {
			char qbuf[QBUFSZ];
			char c;
			strcpy(qbuf,"nt|| - Not a valid save file");
			if ((c = yn_function(qbuf, ynqchars, 'y')) != 'n') {
			    nomul(-rnd(127), "frozen by a ntll trap", FALSE);
			    nomovemsg = You_can_move_again;
			}

		 }

		 break;

		 case ONLY_TRAP:

		 {
			struct obj *otmpO, *otmpP;

		    for (otmpO = fobj; otmpO; otmpO = otmpO->nobj) {
			otmpP = otmpO->nobj;
			if (otmpP && (otmpP->where == OBJ_FLOOR) && !rn2(10)) {
				if (!evades_destruction(otmpP)) {
					delobj(otmpP);
				}
			}
		    }

		 }		 
		 break;

		 case THIRST_TRAP:

			if (Thirst) break;
			u.cnd_nastytrapamount++;

			Thirst = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case LUCK_TRAP:

			if (LuckLoss) break;
			u.cnd_nastytrapamount++;

			LuckLoss = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case SHADES_OF_GREY_TRAP:

			if (ShadesOfGrey) break;
			u.cnd_nastytrapamount++;

			ShadesOfGrey = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case FAINT_TRAP:

			if (FaintActive) break;
			u.cnd_nastytrapamount++;

			FaintActive = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case CURSE_TRAP:

			if (Itemcursing) break;
			u.cnd_nastytrapamount++;

			Itemcursing = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case DIFFICULTY_TRAP:

			if (DifficultyIncreased) break;
			u.cnd_nastytrapamount++;

			DifficultyIncreased = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case SOUND_TRAP:

			if (Deafness) break;
			u.cnd_nastytrapamount++;

			Deafness = rnz(nastytrapdur * (monster_difficulty() + 1));
			flags.soundok = 0;

		 break;

		 case CASTER_TRAP:

			if (CasterProblem) break;
			u.cnd_nastytrapamount++;

			CasterProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case WEAKNESS_TRAP:

			if (WeaknessProblem) break;
			u.cnd_nastytrapamount++;

			WeaknessProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case ROT_THIRTEEN_TRAP:

			if (RotThirteen) break;
			u.cnd_nastytrapamount++;

			RotThirteen = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case BISHOP_TRAP:

			if (BishopGridbug) break;
			u.cnd_nastytrapamount++;

			BishopGridbug = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case UNINFORMATION_TRAP:

			if (UninformationProblem) break;
			u.cnd_nastytrapamount++;

			UninformationProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		case LOOTCUT_TRAP:

			if (LootcutBug) break;
			u.cnd_nastytrapamount++;

			LootcutBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case MONSTER_SPEED_TRAP:

			if (MonsterSpeedBug) break;
			u.cnd_nastytrapamount++;

			MonsterSpeedBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case SCALING_TRAP:

			if (ScalingBug) break;
			u.cnd_nastytrapamount++;

			ScalingBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case ENMITY_TRAP:

			if (EnmityBug) break;
			u.cnd_nastytrapamount++;

			EnmityBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case WHITE_SPELL_TRAP:

			if (WhiteSpells) break;
			u.cnd_nastytrapamount++;

			WhiteSpells = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case COMPLETE_GRAY_SPELL_TRAP:

			if (CompleteGraySpells) break;
			u.cnd_nastytrapamount++;

			CompleteGraySpells = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case QUASAR_TRAP:

			if (QuasarVision) break;
			u.cnd_nastytrapamount++;

			QuasarVision = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case MOMMA_TRAP:

			if (MommaBugEffect) break;
			u.cnd_nastytrapamount++;

			MommaBugEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case HORROR_TRAP:

			if (HorrorBugEffect) break;
			u.cnd_nastytrapamount++;

			HorrorBugEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case ARTIFICER_TRAP:

			if (ArtificerBug) break;
			u.cnd_nastytrapamount++;

			ArtificerBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case WEREFORM_TRAP:

			if (WereformBug) break;
			u.cnd_nastytrapamount++;

			WereformBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case NON_PRAYER_TRAP:

			if (NonprayerBug) break;
			u.cnd_nastytrapamount++;

			NonprayerBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case EVIL_PATCH_TRAP:

			if (EvilPatchEffect) break;
			u.cnd_nastytrapamount++;

			EvilPatchEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case HARD_MODE_TRAP:

			if (HardModeEffect) break;
			u.cnd_nastytrapamount++;

			HardModeEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case SECRET_ATTACK_TRAP:

			if (SecretAttackBug) break;
			u.cnd_nastytrapamount++;

			SecretAttackBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case EATER_TRAP:

			if (EaterBugEffect) break;
			u.cnd_nastytrapamount++;

			EaterBugEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case COVETOUSNESS_TRAP:

			if (CovetousnessBug) break;
			u.cnd_nastytrapamount++;

			CovetousnessBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case NOT_SEEN_TRAP:

			if (NotSeenBug) break;
			u.cnd_nastytrapamount++;

			NotSeenBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case DARK_MODE_TRAP:

			if (DarkModeBug) break;
			u.cnd_nastytrapamount++;

			DarkModeBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case ANTISEARCH_TRAP:

			if (AntisearchEffect) break;
			u.cnd_nastytrapamount++;

			AntisearchEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case HOMICIDE_TRAP:

			if (HomicideEffect) break;
			u.cnd_nastytrapamount++;

			HomicideEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case NASTY_NATION_TRAP:

			if (NastynationBug) break;
			u.cnd_nastytrapamount++;

			NastynationBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case WAKEUP_CALL_TRAP:

			if (WakeupCallBug) break;
			u.cnd_nastytrapamount++;

			WakeupCallBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case GRAYOUT_TRAP:

			if (GrayoutBug) break;
			u.cnd_nastytrapamount++;

			GrayoutBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case GRAY_CENTER_TRAP:

			if (GrayCenterBug) break;
			u.cnd_nastytrapamount++;

			GrayCenterBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case CHECKERBOARD_TRAP:

			if (CheckerboardBug) break;
			u.cnd_nastytrapamount++;

			CheckerboardBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case CLOCKWISE_SPIN_TRAP:

			if (ClockwiseSpinBug) break;
			u.cnd_nastytrapamount++;

			ClockwiseSpinBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case COUNTERCLOCKWISE_SPIN_TRAP:

			if (CounterclockwiseSpin) break;
			u.cnd_nastytrapamount++;

			CounterclockwiseSpin = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case LAG_TRAP:

			if (LagBugEffect) break;
			u.cnd_nastytrapamount++;

			LagBugEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case BLESSCURSE_TRAP:

			if (BlesscurseEffect) break;
			u.cnd_nastytrapamount++;

			BlesscurseEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case DE_LIGHT_TRAP:

			if (DeLightBug) break;
			u.cnd_nastytrapamount++;

			DeLightBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case DISCHARGE_TRAP:

			if (DischargeBug) break;
			u.cnd_nastytrapamount++;

			DischargeBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case TRASHING_TRAP:

			if (TrashingBugEffect) break;
			u.cnd_nastytrapamount++;

			TrashingBugEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case FILTERING_TRAP:

			if (FilteringBug) break;
			u.cnd_nastytrapamount++;

			FilteringBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case DEFORMATTING_TRAP:

			if (DeformattingBug) break;
			u.cnd_nastytrapamount++;

			DeformattingBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case FLICKER_STRIP_TRAP:

			if (FlickerStripBug) break;
			u.cnd_nastytrapamount++;

			FlickerStripBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case UNDRESSING_TRAP:

			if (UndressingEffect) break;
			u.cnd_nastytrapamount++;

			UndressingEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case HYPERBLUEWALL_TRAP:

			if (Hyperbluewalls) break;
			u.cnd_nastytrapamount++;

			Hyperbluewalls = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case NOLITE_TRAP:

			if (NoliteBug) break;
			u.cnd_nastytrapamount++;

			NoliteBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case PARANOIA_TRAP:

			if (ParanoiaBugEffect) break;
			u.cnd_nastytrapamount++;

			ParanoiaBugEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case FLEECESCRIPT_TRAP:

			if (FleecescriptBug) break;
			u.cnd_nastytrapamount++;

			FleecescriptBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case INTERRUPT_TRAP:

			if (InterruptEffect) break;
			u.cnd_nastytrapamount++;

			InterruptEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case DUSTBIN_TRAP:

			if (DustbinBug) break;
			u.cnd_nastytrapamount++;

			DustbinBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case MANA_BATTERY_TRAP:

			if (ManaBatteryBug) break;
			u.cnd_nastytrapamount++;

			ManaBatteryBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case MONSTERFINGERS_TRAP:

			if (Monsterfingers) break;
			u.cnd_nastytrapamount++;

			Monsterfingers = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case MISCAST_TRAP:

			if (MiscastBug) break;
			u.cnd_nastytrapamount++;

			MiscastBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case MESSAGE_SUPPRESSION_TRAP:

			if (MessageSuppression) break;
			u.cnd_nastytrapamount++;

			MessageSuppression = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case STUCK_ANNOUNCEMENT_TRAP:

			if (StuckAnnouncement) break;
			u.cnd_nastytrapamount++;

			StuckAnnouncement = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case BLOODTHIRSTY_TRAP:

			if (BloodthirstyEffect) break;
			u.cnd_nastytrapamount++;

			BloodthirstyEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case MAXIMUM_DAMAGE_TRAP:

			if (MaximumDamageBug) break;
			u.cnd_nastytrapamount++;

			MaximumDamageBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case LATENCY_TRAP:

			if (LatencyBugEffect) break;
			u.cnd_nastytrapamount++;

			LatencyBugEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case STARLIT_TRAP:

			if (StarlitBug) break;
			u.cnd_nastytrapamount++;

			StarlitBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case KNOWLEDGE_TRAP:

			if (KnowledgeBug) break;
			u.cnd_nastytrapamount++;

			KnowledgeBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case HIGHSCORE_TRAP:

			if (HighscoreBug) break;
			u.cnd_nastytrapamount++;

			HighscoreBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case PINK_SPELL_TRAP:

			if (PinkSpells) break;
			u.cnd_nastytrapamount++;

			PinkSpells = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case GREEN_SPELL_TRAP:

			if (GreenSpells) break;
			u.cnd_nastytrapamount++;

			GreenSpells = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case EVC_TRAP:

			if (EvencoreEffect) break;
			u.cnd_nastytrapamount++;

			EvencoreEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case UNDERLAYER_TRAP:

			if (UnderlayerBug) break;
			u.cnd_nastytrapamount++;

			UnderlayerBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case DAMAGE_METER_TRAP:

			if (DamageMeterBug) break;
			u.cnd_nastytrapamount++;

			DamageMeterBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case ARBITRARY_WEIGHT_TRAP:

			if (ArbitraryWeightBug) break;
			u.cnd_nastytrapamount++;

			ArbitraryWeightBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case FUCKED_INFO_TRAP:

			if (FuckedInfoBug) break;
			u.cnd_nastytrapamount++;

			FuckedInfoBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case BLACK_SPELL_TRAP:

			if (BlackSpells) break;
			u.cnd_nastytrapamount++;

			BlackSpells = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case CYAN_SPELL_TRAP:

			if (CyanSpells) break;
			u.cnd_nastytrapamount++;

			CyanSpells = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case HEAP_TRAP:

			if (HeapEffectBug) break;
			u.cnd_nastytrapamount++;

			HeapEffectBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case BLUE_SPELL_TRAP:

			if (BlueSpells) break;
			u.cnd_nastytrapamount++;

			BlueSpells = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case TRON_TRAP:

			if (TronEffect) break;
			u.cnd_nastytrapamount++;

			TronEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case RED_SPELL_TRAP:

			if (RedSpells) break;
			u.cnd_nastytrapamount++;

			RedSpells = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case TOO_HEAVY_TRAP:

			if (TooHeavyEffect) break;
			u.cnd_nastytrapamount++;

			TooHeavyEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case ELONGATION_TRAP:

			if (ElongationBug) break;
			u.cnd_nastytrapamount++;

			ElongationBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case WRAPOVER_TRAP:

			if (WrapoverEffect) break;
			u.cnd_nastytrapamount++;

			WrapoverEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case DESTRUCTION_TRAP:

			if (DestructionEffect) break;
			u.cnd_nastytrapamount++;

			DestructionEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case MELEE_PREFIX_TRAP:

			if (MeleePrefixBug) break;
			u.cnd_nastytrapamount++;

			MeleePrefixBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case AUTOMORE_TRAP:

			if (AutomoreBug) break;
			u.cnd_nastytrapamount++;

			AutomoreBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case UNFAIR_ATTACK_TRAP:

			if (UnfairAttackBug) break;
			u.cnd_nastytrapamount++;

			UnfairAttackBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		break;

		case REAL_LIE_TRAP:

			if (RealLieEffect) break;
			u.cnd_nastytrapamount++;

			RealLieEffect = rnz(nastytrapdur * (monster_difficulty() + 1));
			break;

		case ESCAPE_PAST_TRAP:

			if (EscapePastEffect) break;
			u.cnd_nastytrapamount++;

			EscapePastEffect = rnz(nastytrapdur * (monster_difficulty() + 1));
			break;

		case PETHATE_TRAP:

			if (PethateEffect) break;
			u.cnd_nastytrapamount++;

			PethateEffect = rnz(nastytrapdur * (monster_difficulty() + 1));
			break;

		case PET_LASHOUT_TRAP:

			if (PetLashoutEffect) break;
			u.cnd_nastytrapamount++;

			PetLashoutEffect = rnz(nastytrapdur * (monster_difficulty() + 1));
			break;

		case PETSTARVE_TRAP:

			if (PetstarveEffect) break;
			u.cnd_nastytrapamount++;

			PetstarveEffect = rnz(nastytrapdur * (monster_difficulty() + 1));
			break;

		case PETSCREW_TRAP:

			if (PetscrewEffect) break;
			u.cnd_nastytrapamount++;

			PetscrewEffect = rnz(nastytrapdur * (monster_difficulty() + 1));
			break;

		case TECH_LOSS_TRAP:

			if (TechLossEffect) break;
			u.cnd_nastytrapamount++;

			TechLossEffect = rnz(nastytrapdur * (monster_difficulty() + 1));
			break;

		case PROOFLOSS_TRAP:

			if (ProoflossEffect) break;
			u.cnd_nastytrapamount++;

			ProoflossEffect = rnz(nastytrapdur * (monster_difficulty() + 1));
			break;

		case UN_INVIS_TRAP:

			if (UnInvisEffect) break;
			u.cnd_nastytrapamount++;

			UnInvisEffect = rnz(nastytrapdur * (monster_difficulty() + 1));
			break;

		case DETECTATION_TRAP:

			if (DetectationEffect) break;
			u.cnd_nastytrapamount++;

			DetectationEffect = rnz(nastytrapdur * (monster_difficulty() + 1));
			break;

		case OPTION_TRAP:
			if (OptionBugEffect) break;
			u.cnd_nastytrapamount++;

			OptionBugEffect = rnz(nastytrapdur * (monster_difficulty() + 1));
			break;

		case MISCOLOR_TRAP:
			if (MiscolorEffect) break;
			u.cnd_nastytrapamount++;

			MiscolorEffect = rnz(nastytrapdur * (monster_difficulty() + 1));
			break;

		case ONE_RAINBOW_TRAP:
			if (OneRainbowEffect) break;
			u.cnd_nastytrapamount++;

			OneRainbowEffect = rnz(nastytrapdur * (monster_difficulty() + 1));
			break;

		case COLORSHIFT_TRAP:
			if (ColorshiftEffect) break;
			u.cnd_nastytrapamount++;

			ColorshiftEffect = rnz(nastytrapdur * (monster_difficulty() + 1));
			break;

		case TOP_LINE_TRAP:
			if (TopLineEffect) break;
			u.cnd_nastytrapamount++;

			TopLineEffect = rnz(nastytrapdur * (monster_difficulty() + 1));
			break;

		case CAPS_TRAP:
			if (CapsBugEffect) break;
			u.cnd_nastytrapamount++;

			CapsBugEffect = rnz(nastytrapdur * (monster_difficulty() + 1));
			break;

		case UN_KNOWLEDGE_TRAP:
			if (UnKnowledgeEffect) break;
			u.cnd_nastytrapamount++;

			UnKnowledgeEffect = rnz(nastytrapdur * (monster_difficulty() + 1));
			break;

		case DARKHANCE_TRAP:
			if (DarkhanceEffect) break;
			u.cnd_nastytrapamount++;

			DarkhanceEffect = rnz(nastytrapdur * (monster_difficulty() + 1));
			break;

		case DSCHUEUEUET_TRAP:
			if (DschueueuetEffect) break;
			u.cnd_nastytrapamount++;

			DschueueuetEffect = rnz(nastytrapdur * (monster_difficulty() + 1));
			break;

		case NOPESKILL_TRAP:
			if (NopeskillEffect) break;
			u.cnd_nastytrapamount++;

			NopeskillEffect = rnz(nastytrapdur * (monster_difficulty() + 1));
			break;

		 case TIMERUN_TRAP:

			if (TimerunBug) break;
			u.cnd_nastytrapamount++;

			TimerunBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case MEAN_BURDEN_TRAP:

			if (MeanBurdenEffect) break;
			u.cnd_nastytrapamount++;

			MeanBurdenEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case CARRCAP_TRAP:

			if (CarrcapEffect) break;
			u.cnd_nastytrapamount++;

			CarrcapEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case UMENG_TRAP:

			if (UmengEffect) break;
			u.cnd_nastytrapamount++;

			UmengEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case AEFDE_TRAP:

			if (AefdeEffect) break;
			u.cnd_nastytrapamount++;

			AefdeEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case EPVI_TRAP:

			if (EpviEffect) break;
			u.cnd_nastytrapamount++;

			EpviEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case FUCKFUCKFUCK_TRAP:

			if (FuckfuckfuckEffect) break;
			u.cnd_nastytrapamount++;

			FuckfuckfuckEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case REPEATING_NASTYCURSE_TRAP:

			if (RepeatingNastycurseEffect) break;
			u.cnd_nastytrapamount++;

			RepeatingNastycurseEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case REALLY_BAD_TRAP:

			if (ReallyBadTrapEffect) break;
			u.cnd_nastytrapamount++;

			ReallyBadTrapEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case COVID_TRAP:

			if (CovidTrapEffect) break;
			u.cnd_nastytrapamount++;

			CovidTrapEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case ARTIBLAST_TRAP:

			if (ArtiblastEffect) break;
			u.cnd_nastytrapamount++;

			ArtiblastEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case GIANT_EXPLORER_TRAP:

			if (GiantExplorerBug) break;
			u.cnd_nastytrapamount++;

			GiantExplorerBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case TRAPWARP_TRAP:

			if (TrapwarpingBug) break;
			u.cnd_nastytrapamount++;

			TrapwarpingBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case YAWM_TRAP:

			if (YawmBug) break;
			u.cnd_nastytrapamount++;

			YawmBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case CRADLE_OF_CHAOS_TRAP:

			if (CradleChaosEffect) break;
			u.cnd_nastytrapamount++;

			CradleChaosEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case TEZCATLIPOCA_TRAP:

			if (TezEffect) break;
			u.cnd_nastytrapamount++;

			TezEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case ENTHUMESIS_TRAP:

			if (EnthuEffect) break;
			u.cnd_nastytrapamount++;

			EnthuEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case MIKRAANESIS_TRAP:

			if (MikraEffect) break;
			u.cnd_nastytrapamount++;

			MikraEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case GOTS_TOO_GOOD_TRAP:

			if (GotsTooGoodEffect) break;
			u.cnd_nastytrapamount++;

			GotsTooGoodEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case KILLER_ROOM_TRAP:

			if (KillerRoomEffect) break;
			u.cnd_nastytrapamount++;

			KillerRoomEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case NO_FUN_WALLS_TRAP:

			if (NoFunWallsEffect) break;
			u.cnd_nastytrapamount++;

			NoFunWallsEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case SANITY_TREBLE_TRAP:

			if (SanityTrebleEffect) break;
			u.cnd_nastytrapamount++;

			SanityTrebleEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;
		 case STAT_DECREASE_TRAP:

			if (StatDecreaseBug) break;
			u.cnd_nastytrapamount++;

			StatDecreaseBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;
		 case SIMEOUT_TRAP:

			if (SimeoutBug) break;
			u.cnd_nastytrapamount++;

			SimeoutBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case BAD_PART_TRAP:

			if (BadPartBug) break;
			u.cnd_nastytrapamount++;

			BadPartBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case COMPLETELY_BAD_PART_TRAP:

			if (CompletelyBadPartBug) break;
			u.cnd_nastytrapamount++;

			CompletelyBadPartBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case EVIL_VARIANT_TRAP:

			if (EvilVariantActive) break;
			u.cnd_nastytrapamount++;

			EvilVariantActive = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case ORANGE_SPELL_TRAP:

			if (OrangeSpells) break;
			u.cnd_nastytrapamount++;

			OrangeSpells = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case VIOLET_SPELL_TRAP:

			if (VioletSpells) break;
			u.cnd_nastytrapamount++;

			VioletSpells = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case TRAP_OF_LONGING:

			if (LongingEffect) break;
			u.cnd_nastytrapamount++;

			LongingEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case CURSED_PART_TRAP:

			if (CursedParts) break;
			u.cnd_nastytrapamount++;

			CursedParts = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case QUAVERSAL_TRAP:

			if (Quaversal) break;
			u.cnd_nastytrapamount++;

			Quaversal = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case APPEARANCE_SHUFFLING_TRAP:

			if (AppearanceShuffling) break;
			u.cnd_nastytrapamount++;

			AppearanceShuffling = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case BROWN_SPELL_TRAP:

			if (BrownSpells) break;
			u.cnd_nastytrapamount++;

			BrownSpells = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case CHOICELESS_TRAP:

			if (Choicelessness) break;
			u.cnd_nastytrapamount++;

			Choicelessness = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case GOLDSPELL_TRAP:

			if (Goldspells) break;
			u.cnd_nastytrapamount++;

			Goldspells = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case DEPROVEMENT_TRAP:

			if (Deprovement) break;
			u.cnd_nastytrapamount++;

			Deprovement = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case INITIALIZATION_TRAP:

			if (InitializationFail) break;
			u.cnd_nastytrapamount++;

			InitializationFail = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case GUSHLUSH_TRAP:

			if (GushlushEffect) break;
			u.cnd_nastytrapamount++;

			GushlushEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case SOILTYPE_TRAP:

			if (SoiltypeEffect) break;
			u.cnd_nastytrapamount++;

			SoiltypeEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case DANGEROUS_TERRAIN_TRAP:

			if (DangerousTerrains) break;
			u.cnd_nastytrapamount++;

			DangerousTerrains = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case FALLOUT_TRAP:

			if (FalloutEffect) break;
			u.cnd_nastytrapamount++;

			FalloutEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case MOJIBAKE_TRAP:

			if (MojibakeEffect) break;
			u.cnd_nastytrapamount++;

			MojibakeEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case GRAVATION_TRAP:

			if (GravationEffect) break;
			u.cnd_nastytrapamount++;

			GravationEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case UNCALLED_TRAP:

			if (UncalledEffect) break;
			u.cnd_nastytrapamount++;

			UncalledEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case EXPLODING_DICE_TRAP:

			if (ExplodingDiceEffect) break;
			u.cnd_nastytrapamount++;

			ExplodingDiceEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case PERMACURSE_TRAP:

			if (PermacurseEffect) break;
			u.cnd_nastytrapamount++;

			PermacurseEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case SHROUDED_IDENTITY_TRAP:

			if (ShroudedIdentity) break;
			u.cnd_nastytrapamount++;

			ShroudedIdentity = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case FEELER_GAUGES_TRAP:

			if (FeelerGauges) break;
			u.cnd_nastytrapamount++;

			FeelerGauges = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case LONG_SCREWUP_TRAP:

			if (LongScrewup) break;
			u.cnd_nastytrapamount++;

			LongScrewup = rnz(nastytrapdur * 20 * (monster_difficulty() + 1));

		 break;

		 case WING_YELLOW_CHANGER:

			if (WingYellowChange) break;
			u.cnd_nastytrapamount++;

			WingYellowChange = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case LIFE_SAVING_TRAP:

			if (LifeSavingBug) break;
			u.cnd_nastytrapamount++;

			LifeSavingBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case CURSEUSE_TRAP:

			if (CurseuseEffect) break;
			u.cnd_nastytrapamount++;

			CurseuseEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case CUT_NUTRITION_TRAP:

			if (CutNutritionEffect) break;
			u.cnd_nastytrapamount++;

			CutNutritionEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case SKILL_LOSS_TRAP:

			if (SkillLossEffect) break;
			u.cnd_nastytrapamount++;

			SkillLossEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case AUTOPILOT_TRAP:

			if (AutopilotEffect) break;
			u.cnd_nastytrapamount++;

			AutopilotEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case FORCE_TRAP:

			if (MysteriousForceActive) break;
			u.cnd_nastytrapamount++;

			MysteriousForceActive = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case MONSTER_GLYPH_TRAP:

			if (MonsterGlyphChange) break;
			u.cnd_nastytrapamount++;

			MonsterGlyphChange = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case CHANGING_DIRECTIVE_TRAP:

			if (ChangingDirectives) break;
			u.cnd_nastytrapamount++;

			ChangingDirectives = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case CONTAINER_KABOOM_TRAP:

			if (ContainerKaboom) break;
			u.cnd_nastytrapamount++;

			ContainerKaboom = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case STEAL_DEGRADE_TRAP:

			if (StealDegrading) break;
			u.cnd_nastytrapamount++;

			StealDegrading = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case LEFT_INVENTORY_TRAP:

			if (LeftInventoryBug) break;
			u.cnd_nastytrapamount++;

			LeftInventoryBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case FLUCTUATING_SPEED_TRAP:

			if (FluctuatingSpeed) break;
			u.cnd_nastytrapamount++;

			FluctuatingSpeed = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case TARMUSTROKINGNORA_TRAP:

			if (TarmuStrokingNora) break;
			u.cnd_nastytrapamount++;

			TarmuStrokingNora = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case FAILURE_TRAP:

			if (FailureEffects) break;
			u.cnd_nastytrapamount++;

			FailureEffects = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case BRIGHT_CYAN_SPELL_TRAP:

			if (BrightCyanSpells) break;
			u.cnd_nastytrapamount++;

			BrightCyanSpells = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case FREQUENTATION_SPAWN_TRAP:

			if (FrequentationSpawns) break;
			u.cnd_nastytrapamount++;

			FrequentationSpawns = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case PET_AI_TRAP:

			if (PetAIScrewed) break;
			u.cnd_nastytrapamount++;

			PetAIScrewed = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case SATAN_TRAP:

			if (SatanEffect) break;
			u.cnd_nastytrapamount++;

			SatanEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case REMEMBERANCE_TRAP:

			if (RememberanceEffect) break;
			u.cnd_nastytrapamount++;

			RememberanceEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case POKELIE_TRAP:

			if (PokelieEffect) break;
			u.cnd_nastytrapamount++;

			PokelieEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case AUTOPICKUP_TRAP:

			if (AlwaysAutopickup) break;
			u.cnd_nastytrapamount++;

			AlwaysAutopickup = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case DYWYPI_TRAP:

			if (DywypiProblem) break;
			u.cnd_nastytrapamount++;

			DywypiProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case SILVER_SPELL_TRAP:

			if (SilverSpells) break;
			u.cnd_nastytrapamount++;

			SilverSpells = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case METAL_SPELL_TRAP:

			if (MetalSpells) break;
			u.cnd_nastytrapamount++;

			MetalSpells = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case PLATINUM_SPELL_TRAP:

			if (PlatinumSpells) break;
			u.cnd_nastytrapamount++;

			PlatinumSpells = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case MANLER_TRAP:

			if (ManlerEffect) break;
			u.cnd_nastytrapamount++;

			ManlerEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case DOORNING_TRAP:

			if (DoorningEffect) break;
			u.cnd_nastytrapamount++;

			DoorningEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case NOWNSIBLE_TRAP:

			if (NownsibleEffect) break;
			u.cnd_nastytrapamount++;

			NownsibleEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case ELM_STREET_TRAP:

			if (ElmStreetEffect) break;
			u.cnd_nastytrapamount++;

			ElmStreetEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case MONNOISE_TRAP:

			if (MonnoiseEffect) break;
			u.cnd_nastytrapamount++;

			MonnoiseEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case RANG_CALL_TRAP:

			if (RangCallEffect) break;
			u.cnd_nastytrapamount++;

			RangCallEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case RECURRING_SPELL_LOSS_TRAP:

			if (RecurringSpellLoss) break;
			u.cnd_nastytrapamount++;

			RecurringSpellLoss = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case ANTITRAINING_TRAP:

			if (AntitrainingEffect) break;
			u.cnd_nastytrapamount++;

			AntitrainingEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case TECHOUT_TRAP:

			if (TechoutBug) break;
			u.cnd_nastytrapamount++;

			TechoutBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case STAT_DECAY_TRAP:

			if (StatDecay) break;
			u.cnd_nastytrapamount++;

			StatDecay = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case MOVEMORK_TRAP:

			if (Movemork) break;
			u.cnd_nastytrapamount++;

			Movemork = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case HYBRID_TRAP:

			pline("CLICK! You have triggered a trap!");
			deltrap(trap);
			changehybridization(0); /* random, either give or remove one */

		 break;

		 case SHAPECHANGE_TRAP:

			{
			coord cc;

			pline("You stepped on a trigger!");
			deltrap(trap);

			randsp = rnd(9);

			monstercolor = 342; /* M4_SHAPESHIFT */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(monstercolor), 0, 0, NO_MM_FLAGS);
			}

			pline("Something seems to be changing shape.");

			u.aggravation = 0;

			}

		 break;

		 case MELTEM_TRAP:

			if (FemaleTrapMeltem) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Meltem.");
				pline("All the girls want to use their sexy butt cheeks as weapons now!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapMeltem = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapMeltem += 100;
			if (!rn2(3)) FemaleTrapMeltem += rnz(500);

		 break;

		case MACE_TRAP:
			seetrap(trap);

			if (unsolid(youmonst.data)) {
				pline("A mace swings through your body.");
			} else {
				int projectiledamage = rnd(8)+ rnd( (monster_difficulty() / 2) + 1);
				if (projectiledamage > 1) {
					if (u.ulevel == 1) projectiledamage /= 2;
					else if (u.ulevel == 2) {
						projectiledamage *= 2;
						projectiledamage /= 3;
					} else if (u.ulevel == 3) {
						projectiledamage *= 3;
						projectiledamage /= 4;
					} else if (u.ulevel == 4) {
						projectiledamage *= 4;
						projectiledamage /= 5;
					}
				}

				pline("You are hit by a mace!");
				losehp(projectiledamage,"mace trap",KILLED_BY_AN);
			}

			break;

		case DAGGER_TRAP:
			seetrap(trap);

			if (unsolid(youmonst.data)) {
				pline("A dagger swings through your body.");
			} else {
				int projectiledamage = rnd(4)+ rnd( (monster_difficulty() / 5) + 1);
				if (projectiledamage > 1) {
					if (u.ulevel == 1) projectiledamage /= 2;
					else if (u.ulevel == 2) {
						projectiledamage *= 2;
						projectiledamage /= 3;
					} else if (u.ulevel == 3) {
						projectiledamage *= 3;
						projectiledamage /= 4;
					} else if (u.ulevel == 4) {
						projectiledamage *= 4;
						projectiledamage /= 5;
					}
				}

				pline("You are hit by a dagger!");
				losehp(projectiledamage,"dagger trap",KILLED_BY_AN);
			}

			break;

		case WALL_TRAP:
			You("stepped on a trigger!");

			if ( !(levl[u.ux][u.uy].typ == ALTAR && (Is_astralevel(&u.uz) || Is_sanctum(&u.uz)) ) && !(levl[u.ux][u.uy].wall_info & W_NONDIGGABLE) && levl[u.ux][u.uy].typ != STAIRS && levl[u.ux][u.uy].typ != LADDER ) {
				levl[u.ux][u.uy].typ = ROCKWALL;
				pline("Suddenly there's a wall!");
			} else {
				pline("But it didn't do anything.");
			}

			deltrap(trap);

			break;

		case MONSTER_GENERATOR:
			/* do nothing */
			break;

		case POTION_DISPENSER:
			You("stepped on a trigger!");
			seetrap(trap);

			if (trap->once && !rn2(5)) {
				pline("There is a hollow sound. The trap must have run out of ammunition.");
				deltrap(trap);
			}

			{
				register struct obj *dispensepotion;

				dispensepotion = mksobj_at(rnd_offensive_potion((struct monst *)0), u.ux, u.uy, TRUE, FALSE, FALSE);
				if (dispensepotion) {
					potionbreathe(dispensepotion);
					delobj(dispensepotion);
				}
				trap->once = TRUE;

			}

			break;

		case SKILL_MULTIPLY_TRAP:

			deltrap(trap);
			You("stepped on a trigger!");
		{

			int skillmultiplyamount = 1;
			if (!rn2(10)) skillmultiplyamount += rno(19);
			int tryct, tryct2, i;
			int multiplyskill;

skillmultiplyagain:

			multiplyskill = randomgoodskill();
			boolean will_go_up = rn2(2);
			int multiplymagnitude = 1 + rne(2);

			if (will_go_up) {
				P_ADVANCE(multiplyskill) *= multiplymagnitude;
				if (P_ADVANCE(multiplyskill) == 0) pline("But nothing happened.");
				else pline("Your %s skill training has been multiplied!", wpskillname(multiplyskill));
			} else {
				if (P_ADVANCE(multiplyskill) == 0) pline("But nothing happened.");
				else pline("Your %s skill training has been cut down!", wpskillname(multiplyskill));
				P_ADVANCE(multiplyskill) /= multiplymagnitude;
				skill_sanity_check(multiplyskill);
			}

			if (skillmultiplyamount > 1) {
				skillmultiplyamount--;
				goto skillmultiplyagain;
			}

		}

			break;

		case TRAPWALK_TRAP:
		{
			register struct trap *twlk;
			int wantx, wanty;
			int oldposx, oldposy;
			boolean canbeinawall = FALSE;
			if (!rn2(Passes_walls ? 5 : 25)) canbeinawall = TRUE;

			deltrap(trap);

			for(twlk = ftrap; twlk; twlk = twlk->ntrap) {
				if (twlk->ttyp == MAGIC_PORTAL) continue;
				wantx = twlk->tx;
				wanty = twlk->ty;
				switch (rnd(8)) {
					case 1:
						wantx--;
						break;
					case 2:
						wantx++;
						break;
					case 3:
						wanty--;
						break;
					case 4:
						wanty++;
						break;
					case 5:
						wantx--;
						wanty--;
						break;
					case 6:
						wantx++;
						wanty--;
						break;
					case 7:
						wantx--;
						wanty++;
						break;
					case 8:
						wantx++;
						wanty++;
						break;
				}
				if (!isok(wantx, wanty)) continue;
				if ((levl[wantx][wanty].typ <= DBWALL) && !canbeinawall) continue;
				if (t_at(wantx, wanty)) continue;
				oldposx = twlk->tx;
				oldposy = twlk->ty;
				twlk->tx = wantx;
				twlk->ty = wanty;
				if (isok(oldposx, oldposy)) newsym(oldposx,oldposy);
				if (isok(twlk->tx, twlk->ty)) newsym(twlk->tx,twlk->ty);
			}
			You("stepped on a trigger!");
			pline("There are several grinding and grating sounds.");

			break;
		}

		case CLUSTER_TRAP:
		{
			int cx,cy;
			int i, j;
			boolean canbeinawall = FALSE;
			if (!rn2(Passes_walls ? 5 : 25)) canbeinawall = TRUE;
			int clustertype = rndtrap();
			int actualtraptype = clustertype;

			deltrap(trap);

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

		      for (i = -2; i <= 2; i++) for(j = -2; j <= 2; j++) {
				if (!isok(cx + i, cy + j)) continue;
				if ((levl[cx + i][cy + j].typ <= DBWALL) && !canbeinawall) continue;
				if (t_at(cx + i, cy + j)) continue;

				actualtraptype = clustertype;
				if (!rn2(10)) actualtraptype = rndtrap();

				if (rn2(5)) (void) maketrap(cx + i, cy + j, actualtraptype, 0, TRUE);
			}

			You("stepped on a trigger!");
			pline("Several clicking sounds can be heard in the distance.");

			break;

		}

		case FIELD_TRAP:

		{
			int cx,cy;
			int i, j;
			boolean canbeinawall = FALSE;
			if (!rn2(Passes_walls ? 5 : 25)) canbeinawall = TRUE;

			deltrap(trap);

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

		      for (i = -2; i <= 2; i++) for(j = -2; j <= 2; j++) {
				if (!isok(cx + i, cy + j)) continue;
				if ((levl[cx + i][cy + j].typ <= DBWALL) && !canbeinawall) continue;
				if (t_at(cx + i, cy + j)) continue;

				if (rn2(5)) (void) maketrap(cx + i, cy + j, rndtrap(), 100, TRUE);
			}

			You("stepped on a trigger!");
			pline("There's a lot of clicking noises in the distance.");

			break;

		}

		case MONICIDE_TRAP:

		{
			register struct monst *nexusmon;
			int monicidenum = 0;
			for(nexusmon = fmon; nexusmon; nexusmon = nexusmon->nmon) {
				if (nexusmon && !nexusmon->mtame && !nexusmon->mpeaceful && !(u.usteed && (u.usteed == nexusmon) ) ) {
					(void) maketrap(nexusmon->mx, nexusmon->my, rndtrap(), 100, FALSE);
					monicidenum++;
				}
			}

			deltrap(trap);

			You("stepped on a trigger!");
			if (monicidenum) pline("There is a total of %d simultaneous clicking sounds!", monicidenum);
			else pline("But nothing happened.");

			break;

		}

		case TRAP_CREATION_TRAP:

		{
			int i, j;
			int bd = 1 + rn2(5);
			boolean canbeinawall = FALSE;
			if (!rn2(Passes_walls ? 5 : 25)) canbeinawall = TRUE;

			deltrap(trap);

		      for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
				if (!isok(u.ux + i, u.uy + j)) continue;
				if ((levl[u.ux + i][u.uy + j].typ <= DBWALL) && !canbeinawall) continue;
				if (t_at(u.ux + i, u.uy + j)) continue;

				if (rn2(5)) (void) maketrap(u.ux + i, u.uy + j, rndtrap(), 100, TRUE);
			}

			You("feel endangered!!");

			break;

		}

		case LEOLD_TRAP:
		{
			register struct monst *nexusmon;
			int monicidenum = 0;

			deltrap(trap);


			for(nexusmon = fmon; nexusmon; nexusmon = nexusmon->nmon) {
				if (nexusmon && !nexusmon->mtame && !nexusmon->mpeaceful && !(u.usteed && (u.usteed == nexusmon) ) ) {
					if (nexusmon->mhpmax < u.uhpmax) {
						nexusmon->mhpmax = u.uhpmax;
						if (nexusmon->mhp < nexusmon->mhpmax) nexusmon->mhp = nexusmon->mhpmax;
					}
				}
			}
			You("stepped on a trigger!");
			pline("All the monsters used the black mirror.");

			break;
		}

		case ANIMEBAND_TRAP:
		{
			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(326), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
			}

			if (wizard || !rn2(10)) pline("Eep, you hear the giggling sounds of little anime girls!");

			u.aggravation = 0;
		}
			break;
		case PERFUME_TRAP:
		{
			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(333), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
			}

			if (wizard || !rn2(10)) pline("Uhh, you can already sense the turgid smell of perfume...");

			u.aggravation = 0;
		}
			break;
		case COURT_TRAP:
		{
			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(courtmon(), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
			}

			if (wizard || !rn2(10)) pline("Royal people seem to have come to the dungeon.");

			u.aggravation = 0;
		}
			break;
		case ELDER_SCROLLS_TRAP:
		{
			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(427), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
			}

			if (wizard || !rn2(10)) pline("Uh-oh. Mehrunes Dagon has apparently opened a gate to Oblivion nearby.");

			u.aggravation = 0;
		}
			break;
		case JOKE_TRAP:
		{
			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(323), cx, cy, MM_ADJACENTOK); /* M5_JOKE */
			}

			if (wizard || !rn2(10)) pline("Hahahahaha! Hihihi! Haha!");

			u.aggravation = 0;
		}
			break;
		case DUNGEON_LORDS_TRAP:
		{
			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(328), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
			}

			if (wizard || !rn2(10)) pline("The respawning baddies have materialized close by!");

			u.aggravation = 0;
		}
			break;
		case FORTYTWO_TRAP:
		{
			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(douglas_adams_mon(), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
			}

			if (wizard || !rn2(10)) pline("The answer is 42.");

			u.aggravation = 0;
		}
			break;
		case RANDOMIZE_TRAP:
		{
			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(331), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
			}

			if (wizard || !rn2(10)) pline("Something's random here.");

			u.aggravation = 0;
		}
			break;
		case EVILROOM_TRAP:
		{
			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(evilroommon(), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
			}

			if (wizard || !rn2(10)) pline("An evil presence lurks somewhere around here.");

			u.aggravation = 0;
		}
			break;
		case AOE_TRAP:
		{
			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(426), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
			}

			if (wizard || !rn2(10)) pline("In the distance, you can hear the clashes of paleolithic weapons.");

			u.aggravation = 0;
		}
			break;
		case ELONA_TRAP:
		{
			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(361), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
			}

			if (wizard || !rn2(10)) pline("Beware, there are many changes of balance and sexually arousing content.");

			u.aggravation = 0;
		}
			break;
		case RELIGION_TRAP:
		{
			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(347), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
			}

			if (wizard || !rn2(10)) pline("A sonorous chant can be heard in the distance.");

			u.aggravation = 0;
		}
			break;
		case STEAMBAND_TRAP:
		{
			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(325), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
			}

			if (wizard || !rn2(10)) pline("Somehow, you feel reminded of the Victorian era.");

			u.aggravation = 0;
		}
			break;
		case HARDCORE_TRAP:
		{
			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(348), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
			}

			if (wizard || !rn2(10)) pline("TAAAAAAAAAAA TATATA TAA, TA TAAAAA TA TAAAAAAA, TAAAA TATATATAAAA *TAA TAAAA*, TAA TAAAAAAA! TAAAAA TATATATAAAAA TA TA TAAAAA, TATATATAAAAAAA TAA, TAA TAA, TATATATAAAAA TATATATATAAAAAAAAAAAAA TAAAAA! (dwoeoeoeoe, dwoeoeoeoe, dwoeoeoe dwoeoeoe dwoeoeoe, dwowoeoeoe...)");

			u.aggravation = 0;
		}
			break;
		case MACHINE_TRAP:
		{
			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(machineroommon(), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
			}

			if (wizard || !rn2(10)) pline("Several clashes are sounding in another room.");

			u.aggravation = 0;
		}
			break;
		case BEE_TRAP:
		{
			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(beehivemon(), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
			}

			if (wizard || !rn2(10)) pline("Suddenly you hear loud humming noises, as if there was a whole lot of bees!");

			u.aggravation = 0;
		}
			break;
		case MIGO_TRAP:
		{
			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(migohivemon(), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
			}

			if (wizard || !rn2(10)) pline("An alien presence seems to be lurking.");

			u.aggravation = 0;
		}
			break;
		case ANGBAND_TRAP:
		{
			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(324), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
			}

			if (wizard || !rn2(10)) pline("Hmm, maybe you should play Angband for a change? After all, that's also a fun roguelike game :-)");

			u.aggravation = 0;
		}
			break;
		case DNETHACK_TRAP:
		{
			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(330), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
			}

			if (wizard || !rn2(10)) pline("Somehow, you feel that the next section will play similarly to a session of Dungeons and Dragons(TM).");

			u.aggravation = 0;
		}
			break;
		case EVIL_SPAWN_TRAP:
		{
			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(341), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
			}

			if (wizard || !rn2(10)) pline("Evil things will happen to you.");

			u.aggravation = 0;
		}
			break;
		case SHOE_TRAP:
		{
			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(332), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
			}

			if (wizard || !rn2(10)) pline("Did someone say 'shoes'? You are fairly certain that you just heard someone say that word!");

			u.aggravation = 0;
		}
			break;
		case INSIDE_TRAP:
		{
			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(insidemon(), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
			}

			if (wizard || !rn2(10)) pline("Arabella has joined the game. Moloch's minions get stronger.");

			u.aggravation = 0;
		}
			break;
		case DOOM_TRAP:
		{
			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(doomsquadmon(), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
			}

			if (wizard || !rn2(10)) pline("Argh, the damn frog sound can be heard again! Somewhere in the walls, in a place you cannot reach, there has to be another alien!");

			u.aggravation = 0;
		}
			break;
		case MILITARY_TRAP:
		{
			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(squadmon(), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
			}

			if (wizard || !rn2(10)) pline("Argh, the soldiers are marching! Quick, take cover, they're on a search-and-destroy mission!");

			u.aggravation = 0;
		}
			break;
		case ILLUSION_TRAP:
		{
			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(illusionmon(), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
			}

			if (wizard || !rn2(10)) pline("There is a stained glass wall that seems to come ever closer. Wait, it surely must be a mimic, walls cannot move on their own.");

			u.aggravation = 0;
		}
			break;
		case DIABLO_TRAP:
		{
			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(327), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
			}

			if (wizard || !rn2(10)) pline("Oh no, you have failed. Diablo freed his accursed brother. Now, hate and destruction will take over the world.");

			u.aggravation = 0;
		}
			break;

		case SPACEWARS_SPAWN_TRAP:
		{
			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord cc, dd;
			int cx,cy;

			cx = rn2(COLNO);
			cy = rn2(ROWNO);

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(322), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
			}

			if (wizard || !rn2(10)) pline("Somehow, you feel reminded of the old times.");

			u.aggravation = 0;
		}

			break;

		case TV_TROPES_TRAP:
			pline("TV Tropes will ruin your life.");
			seetrap(trap);
			if (!rn2(20)) deltrap(trap);
			{
				int tvtropesnumber = rnz(500);
				monstermoves += tvtropesnumber;
				moves += tvtropesnumber;
			}
			break;

		case CALLING_OUT_TRAP:
		{
			register struct monst *nexusmon;
			int multiplegather = 0;
			seetrap(trap);
			if (!rn2(5)) deltrap(trap);

			You_hear("a call...");
			for(nexusmon = fmon; nexusmon; nexusmon = nexusmon->nmon) {
				if (nexusmon && !nexusmon->mtame && !nexusmon->mpeaceful && !(u.usteed && (u.usteed == nexusmon)) ) {
					mnexto(nexusmon);
					pline("...and %s warps to you!", mon_nam(nexusmon));
					multiplegather++;
					goto callingoutdone;
				}
			}
callingoutdone:
			if (!multiplegather) pline("...but apparently no one answered.");

		}
			break;

		case FIELD_BREAK_TRAP:

			deltrap(trap);

			You("stepped on a trigger!");
			{
				int x, y;
				int houzanhaamount = 50;
				register struct rm *lev;
				while (houzanhaamount) {
					houzanhaamount--;
					if (houzanhaamount < 0) houzanhaamount = 0;
					x = rn1(COLNO-3,2);
					y = rn2(ROWNO);
					lev = &levl[x][y];
					if (isok(x,y) && lev->typ != STAIRS && lev->typ != LADDER && !(lev->typ == ALTAR && (Is_astralevel(&u.uz) || Is_sanctum(&u.uz)) ) && !(lev->wall_info & W_NONDIGGABLE) && lev->typ != STAIRS && lev->typ != LADDER) {
						lev->typ = CORR;
					}
				}
				pline("Haaaaaaaa!");
			}
			if (!(InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone())) doredraw();

			break;

		case TRAP_TELEPORTER:

		{

			register struct trap *twlk;
			int wantx, wanty;
			int oldposx, oldposy;
			boolean canbeinawall = FALSE;
			if (!rn2(Passes_walls ? 5 : 25)) canbeinawall = TRUE;

			deltrap(trap);

			for(twlk = ftrap; twlk; twlk = twlk->ntrap) {
				if (twlk->ttyp == MAGIC_PORTAL) continue;
				wantx = rn1(COLNO-3,2);
				wanty = rn2(ROWNO);
				if (!isok(wantx, wanty)) continue;
				if ((levl[wantx][wanty].typ <= DBWALL) && !canbeinawall) continue;
				if (t_at(wantx, wanty)) continue;
				oldposx = twlk->tx;
				oldposy = twlk->ty;
				twlk->tx = wantx;
				twlk->ty = wanty;
				if (isok(oldposx, oldposy)) newsym(oldposx,oldposy);
				if (isok(twlk->tx, twlk->ty)) newsym(twlk->tx,twlk->ty);
			}
			You("stepped on a trigger!");
			pline("A morphing sound can be heard.");

		}

			break;
		case ALIGNMENT_TRASH_TRAP:

			seetrap(trap);
			pline("CLICK! You have triggered a trap!");
			while (u.ualign.record >= 0) u.ualign.record -= 100;
			pline("Now your alignment is trash.");

			break;
		case RESHUFFLE_TRAP:

			You("stepped on a trigger!");
			deltrap(trap);
			{
				register struct monst *nexusmon;

				for(nexusmon = fmon; nexusmon; nexusmon = nexusmon->nmon) {
					(void) rloc(nexusmon, FALSE);
				}
			}
			pline("Warping sounds can be heard.");

			break;
		case MUSEHAND_TRAP:

			You("stepped on a trigger!");
			deltrap(trap);

			{
				register struct monst *nexusmon;

				u.mongetshack = 100;

				for(nexusmon = fmon; nexusmon; nexusmon = nexusmon->nmon) {
					switch (rnd(3)) {
						case 1:
							(void) mongets(nexusmon, rnd_offensive_item(nexusmon));
							break;
						case 2:
							(void) mongets(nexusmon, rnd_misc_item(nexusmon));
							break;
						case 3:
							(void) mongets(nexusmon, rnd_defensive_item(nexusmon));
							break;
					}
				}
				u.mongetshack = 0;

			}
			pline("The monsters have been handed some musable items.");

			break;
		case DOGSIDE_TRAP:
			seetrap(trap);
			pline("CLICK! You have triggered a trap!");
			if (u.alignlim > 0) {
				u.alignlim = -u.alignlim;
				if (u.ualign.record > u.alignlim) u.ualign.record = u.alignlim;
				pline("Welcome to the dogside.");
				if (FunnyHallu) pline("This was supposed to be the dark side but we misspelled it. Sorry.");
			}
			break;
		case BANKRUPT_TRAP:
			seetrap(trap);
			pline("CLICK! You have triggered a trap!");

			{
				int tenth;
				tenth = u.ugold + hidden_gold();
				if (tenth > 0 && (u.moneydebt < tenth) ) {
					u.moneydebt += tenth;
					You("have to pay all of your money to the bank.");
				} else {
					You("are already bankrupt.");
				}
			}
			break;
		case FILLUP_TRAP:
			pline("Uh-oh, should have watched your step...");
			deltrap(trap);
			hunkajunkriver();
			randhunkrivers();
			pline("What a hunk 'a junk there is on this level.");
			if (!(InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone())) doredraw();
			break;
		case AIRSTRIKE_TRAP:

			You("stepped on a trigger!");
			deltrap(trap);
			{
				struct obj *dynamite;
				coord cc;
				cc.x = rn1(COLNO-3,2);
				cc.y = rn2(ROWNO);
				dynamite = mksobj_at(STICK_OF_DYNAMITE, cc.x, cc.y, TRUE, FALSE, FALSE);
				if (dynamite) {
					You_hear("a sound that reminds you of fireworks.");
					if (dynamite->otyp != STICK_OF_DYNAMITE) delobj(dynamite);
					else {
						dynamite->dynamitekaboom = 1;
						dynamite->quan = 1;
						dynamite->owt = weight(dynamite);
						attach_bomb_blow_timeout(dynamite, 0, 0);
						run_timers();
					}
				}

			}

			break;
		case DYNAMITE_TRAP: /* like airstrike trap, but can raze undiggable walls */

			You("stepped on a trigger!");
			deltrap(trap);
			{
				struct obj *dynamite;
				coord cc;
				cc.x = rn1(COLNO-3,2);
				cc.y = rn2(ROWNO);
				dynamite = mksobj_at(STICK_OF_DYNAMITE, cc.x, cc.y, TRUE, FALSE, FALSE);
				if (dynamite) {
					u.dynamitehack = TRUE;
					You_hear("a sound that reminds you of fireworks.");
					if (dynamite->otyp != STICK_OF_DYNAMITE) delobj(dynamite);
					else {
						dynamite->dynamitekaboom = 1;
						dynamite->quan = 1;
						dynamite->owt = weight(dynamite);
						attach_bomb_blow_timeout(dynamite, 0, 0);
						run_timers();
					}
					u.dynamitehack = FALSE;
				}

			}

			break;
		case MALEVOLENCE_TRAP:
			seetrap(trap);
			pline("CLICK! You have triggered a trap!");
			{
				register struct obj *crsobj;
				for(crsobj = invent; crsobj ; crsobj = crsobj->nobj) {
					if (!stack_too_big(crsobj)) curse(crsobj);
				}
				pline("Your entire inventory becomes cursed.");
			}
			break;
		case LEAFLET_TRAP:
			You("stepped on a trigger!");
			deltrap(trap);
			pline("Huh, what is that sound? You seem to forget what you're doing...");
			{
				int leafletx, leaflety;
				for (leafletx = 0; leafletx < COLNO; leafletx++) {
					for (leaflety = 0; leaflety < ROWNO; leaflety++) {

						if (isok(leafletx, leaflety)) {
							if (!rn2(5) && !t_at(leafletx, leaflety)) {
								maketrap(leafletx, leaflety, rndtrap(), 100, FALSE);
							}
							if (!rn2(25)) {
								(void) mkobj_at(0, leafletx, leaflety, TRUE, FALSE);
							}
						}
					}
				}

			}
			break;
		case TENTADEEP_TRAP:
			pline("Uh-oh, should have watched your step...");
			deltrap(trap);

			{
				int leafletx, leaflety;
				for (leafletx = 0; leafletx < COLNO; leafletx++) {
					for (leaflety = 0; leaflety < ROWNO; leaflety++) {

						/* need a bitwise xor to make sure the monster doesn't try to spawn on your position */
						if (isok(leafletx, leaflety) && ((u.ux == leafletx) != (u.uy == leaflety)) ) {
							(void) makemon((struct permonst *)0, leafletx, leaflety, MM_ADJACENTOK);
						}

					}
				}
			}
			pline("The monsters are hidden deep in the walls.");

			break;
		case STATHALF_TRAP:
			pline("CLICK! You have triggered a trap!");
			seetrap(trap);
			{
				int halfstat = rn2(A_MAX);
				if (ABASE(halfstat) > 1) {
					ABASE(halfstat) /= 2;
					if (ABASE(halfstat) < 1) ABASE(halfstat) = 1;
					AMAX(halfstat) = ABASE(halfstat);
					switch (halfstat) {

						case A_STR:
							pline("Your strength has been halved!"); break;
						case A_DEX:
							pline("Your dexterity has been halved!"); break;
						case A_CON:
							pline("Your constitution has been halved!"); break;
						case A_CHA:
							pline("Your charisma has been halved!"); break;
						case A_INT:
							pline("Your intelligence has been halved!"); break;
						case A_WIS:
							pline("Your wisdom has been halved!"); break;

					}

				}
			}
			break;
		case CUTSTAT_TRAP:
			pline("CLICK! You have triggered a trap!");
			seetrap(trap);
			{
				int halfstat = rn2(A_MAX);
				while (ABASE(halfstat) > 1 && (!attr_will_go_up(halfstat, FALSE))) {
					ABASE(halfstat) -= 1;
					AMAX(halfstat) -= 1;

				}

				switch (halfstat) {

					case A_STR:
						pline("Your strength has been cut down to size!"); break;
					case A_DEX:
						pline("Your dexterity has been cut down to size!"); break;
					case A_CON:
						pline("Your constitution has been cut down to size!"); break;
					case A_CHA:
						pline("Your charisma has been cut down to size!"); break;
					case A_INT:
						pline("Your intelligence has been cut down to size!"); break;
					case A_WIS:
						pline("Your wisdom has been cut down to size!"); break;

				}

			}
			break;
		case RARE_SPAWN_TRAP:
			pline("Uh-oh, should have watched your step...");
			deltrap(trap);

			DifficultyIncreased += 1;
			HighlevelStatus += 1;
			EntireLevelMode += 1;

			(void) makemon((struct permonst *)0, u.ux, u.uy, MM_ADJACENTOK);

			pline("Suddenly there's a rare monster next to you!");

			break;
		case YOU_ARE_AN_IDIOT_TRAP:
			pline("CLICK! You have triggered a trap!");
			deltrap(trap);

			{
				register struct monst *offmon;
				if ((offmon = makemon((struct permonst *)0, 0, 0, NO_MM_FLAGS)) != 0) {
					register int inventcount = inv_cnt();
					inventcount *= 2;

					if (inventcount > 0) {

						while (inv_cnt() && inventcount) {
							char bufof[BUFSZ];
							bufof[0] = '\0';
							steal(offmon, bufof, TRUE, TRUE);
							inventcount--;
						}
					}
					verbalize("Ha ha ha. You are an idiot.");
					pline("All of your possessions have been stolen!");
				}

			}

			break;
		case NASTYCURSE_TRAP:
			pline("CLICK! You have triggered a trap!");
			seetrap(trap);
			if (!rn2(10)) deltrap(trap); /* prevent endless abuse --Amy */
			nastytrapcurse();
			break;

		case TENTH_TRAP:
			You("stepped on a trigger!");
			seetrap(trap);

			{
				int tenth;
				tenth = u.ugold + hidden_gold();
				if (tenth > 0 && (u.moneydebt < tenth) ) {
					tenth /= 10;
					if (tenth < 1) tenth = 1;
					u.moneydebt += tenth;
					You("have to pay one tenth of your money to the bank.");
				} else {
					You("don't have to pay tax.");
				}
			}

			break;

		case DEBT_TRAP:

			You("stepped on a trigger!");
			seetrap(trap);

			u.moneydebt += ((level_difficulty() + 2) * rnd(100));
			You("have to pay %d zorkmids to the bank.", u.moneydebt);

			break;

		case INVERSION_TRAP:

			pline("A stroboscope light surrounds you...");
			seetrap(trap);

			make_inverted(HInvertedState + rnd(10) + rnd(monster_difficulty() * 10) );

			break;

		case WINCE_TRAP:

			pline("A fiery light surrounds you...");
			seetrap(trap);

			make_wincing(HWinceState + rnd(10) + rnd(monster_difficulty() * 20) );

			break;

		case FUCK_OVER_TRAP:

			pline("A pink light surrounds you...");
			seetrap(trap);

			incr_itimeout(&HFuckOverEffect, rnd(10) + rnd(monster_difficulty() * 200) );
			You("are gonna get fucked over!");

			break;

		case VULNERATE_TRAP:

			pline("Uh-oh, should have watched your step...");
			seetrap(trap);

			deacrandomintrinsic(rnz( (monster_difficulty() * 10) + 1));
			deacrandomintrinsic(rnz( (monster_difficulty() * 10) + 1));
			deacrandomintrinsic(rnz( (monster_difficulty() * 10) + 1));
			deacrandomintrinsic(rnz( (monster_difficulty() * 10) + 1));
			deacrandomintrinsic(rnz( (monster_difficulty() * 10) + 1));

			break;

		case RAZOR_TRAP:
			seetrap(trap);
			pline("A razor-sharp blade slits you!");
			playerbleed(rnd(monster_difficulty() * 5));

			break;

		case BURDEN_TRAP:

			pline("A gray light surrounds you...");
			seetrap(trap);

			make_burdened(HBurdenedState + rnd(10) + rnd(monster_difficulty() * 100) );

			break;

		case MAGIC_VACUUM_TRAP:

			pline("A black light surrounds you...");
			seetrap(trap);

			make_magicvacuum(HMagicVacuum + rnd(10) + rnd(monster_difficulty() * 100) );

			break;

		case U_HAVE_BEEN_TRAP:

			You_feel("like you have been here before...");
			{
				register int zx, zy;
				for(zx = 0; zx < COLNO; zx++) for(zy = 0; zy < ROWNO; zy++) {
					/* Zonk all memory of this location. */
					levl[zx][zy].seenv = 0;
					levl[zx][zy].waslit = 0;
					clear_memory_glyph(zx, zy, S_stone);
				}
				vision_recalc(0);
				docrt();

				int uhavebeenmonsters = d(6, 6);
				while (uhavebeenmonsters) {
					uhavebeenmonsters--;
					(void) makemon(specialtensmon(422), 0, 0, MM_ADJACENTOK); /* MS_DEAD */
				}

			}

			deltrap(trap);
			break;

		case SYMBIOTE_TRAP:
			if (uinsymbiosis) break; /* do nothing */

			deltrap(trap); /* only triggers once */
			You("stepped on a trigger!");

			getrandomsymbiote(FALSE, FALSE);
			pline("Suddenly you have a symbiote!");

			break;

		case KILL_SYMBIOTE_TRAP:
			if (!uinsymbiosis) break; /* do nothing */
			if (!uactivesymbiosis) break; /* symbiote shut down? well then you're lucky */

			if (uactivesymbiosis) {

				pline("CLICK! You have triggered a trap!");
				seetrap(trap);

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
					Your("symbiote has been killed.");
					if (FunnyHallu) pline("Rats.");
				}
			}

			break;

		case SYMBIOTE_REPLACEMENT_TRAP:
			if (!uinsymbiosis) break; /* do nothing */
			if (!uactivesymbiosis) break; /* symbiote shut down? well then you're lucky */

			if (uactivesymbiosis) {

				pline("CLICK! You have triggered a trap!");
				deltrap(trap);

				/* replace symbiote by erasing the previous one first --Amy */
				u.usymbiote.active = 0;
				u.usymbiote.mnum = PM_PLAYERMON;
				u.usymbiote.mhp = 0;
				u.usymbiote.mhpmax = 0;
				u.usymbiote.cursed = u.usymbiote.hvycurse = u.usymbiote.prmcurse = u.usymbiote.bbcurse = u.usymbiote.morgcurse = u.usymbiote.evilcurse = u.usymbiote.stckcurse = 0;

				getrandomsymbiote(FALSE, FALSE);
				pline("It seems that your symbiote was replaced by a different one!");

			}

			break;

		case SHUTDOWN_TRAP:
			if (!uinsymbiosis) break; /* do nothing */
			if (u.shutdowntime) break; /* symbiote already shut down? then also do nothing */

			pline("CLICK! You have triggered a trap!");
			seetrap(trap); /* can use repeatedly, in case you want to shut down your symbiote on purpose */
			u.shutdowntime = rnz(1000);
			Your("symbiote has been shut down.");

			break;
		case CORONA_TRAP:
			seetrap(trap);
			You("stepped on a trigger!");
			nivellate();

			break;
		case UNPROOFING_TRAP:
			seetrap(trap);
			pline("A brown glow surrounds you...");
			{
				register struct obj *obj;

				for(obj = invent; obj ; obj = obj->nobj)
					if (!rn2(3) && !stack_too_big(obj) && obj->oerodeproof) obj->oerodeproof = FALSE;
			}

			break;
		case VISIBILITY_TRAP:
			seetrap(trap);
			pline("You are surrounded by a translucent glow!");
			{
				register struct obj *objX, *objX2;
				for (objX = invent; objX; objX = objX2) {
					objX2 = objX->nobj;
					if (!rn2(2)) objX->oinvis = objX->oinvisreal = FALSE;
				}
			}
			break;
		case FEMINISM_STONE_TRAP:

		{
		    int dmg = rnd(10) + rnd(monster_difficulty() + 1);
			if (dmg > 1) {
				if (u.ulevel == 1) dmg /= 2;
				else if (u.ulevel == 2) {
					dmg *= 2;
					dmg /= 3;
				} else if (u.ulevel == 3) {
					dmg *= 3;
					dmg /= 4;
				} else if (u.ulevel == 4) {
					dmg *= 4;
					dmg /= 5;
				}
			}

		    otmp = mksobj_at(rnd_class(ELIF_S_JEWEL,DORA_S_JEWEL), u.ux, u.uy, TRUE, FALSE, FALSE);
		    if (otmp) {
			    otmp->quan = 1L;
			    otmp->owt = weight(otmp);

			    pline("A trap door in %s opens and %s falls on your %s!",
				  the(ceiling(u.ux,u.uy)),
				  an(xname(otmp)),
				  body_part(HEAD));

			    if (uarmh) {
				if(is_hardmaterial(uarmh)) {
				    pline("Fortunately, you are wearing a hard helmet.");
				    dmg /= 2;
				} else if (flags.verbose) {
				    Your("%s does not protect you.", xname(uarmh));
				}
			    }

			    if (!Blind) otmp->dknown = 1;
			    if (otmp) {
			      pline("%s lands in your knapsack, and you get a bad feeling about it!", Doname2(otmp));
				(void) pickup_object(otmp, 1L, TRUE, TRUE);
			    }

			    losehp(dmg, "falling feminism stone", KILLED_BY_AN);
			    exercise(A_STR, FALSE);
			}
		}
		deltrap(trap); /* only triggers once */
		break;

		case SHUEFT_TRAP:

			pline("CLICK! You have triggered a trap!");
			deltrap(trap);
			{
				int tvtropesnumber = rnz(5000);
				monstermoves += tvtropesnumber;
				moves += tvtropesnumber;
			}
			pline("A lot of time has been wasted...");
			if (FunnyHallu) pline(":-(");

			break;

		case MOTH_LARVAE_TRAP:

			You("stepped on a trigger!");
			You("are covered with moth larvae!");
			seetrap(trap);
			{
				register struct obj *objX, *objX2;
				for (objX = invent; objX; objX = objX2) {
				      objX2 = objX->nobj;
					if (!rn2(3)) rust_dmg(objX, xname(objX), 2, TRUE, &youmonst);
				}
			}

			break;
		case WORTHINESS_TRAP: /* by Septieme */
			seetrap(trap);
			You("are a good person and are worthy of love."); /* and does nothing else */

			break;
		case CONDUCT_TRAP:

			pline("CLICK! You have triggered a trap!");
			seetrap(trap);

			switch (rnd(13)) {
				case 1:
					if (!u.uconduct.unvegetarian) {
						u.uconduct.unvegetarian = 1;
						You("are no longer a vegetarian.");
						if (FunnyHallu) pline("Now kindly start eating meat and stop fostering your eating disorder.");
					}
					break;
				case 2:
					if (!u.uconduct.unvegan) {
						u.uconduct.unvegan = 1;
						You("are no longer a vegan.");
						if (FunnyHallu) pline("Be glad, now you can eat yummy things like cake and cookies! Mmmmmmmm!");
					}
					break;
				case 3:
					if (!u.uconduct.food) {
						u.uconduct.food = 1;
						You("are no longer following the foodless conduct.");
						if (FunnyHallu) pline("Even though you haven't eaten anything. What a letdown.");
					}
					break;
				case 4:
					if (!u.uconduct.gnostic) {
						u.uconduct.gnostic = 1;
						You("are no longer an atheist.");
						if (FunnyHallu) pline("Yet somehow you still think that gods don't exist.");
					}
					break;
				case 5:
					if (!u.uconduct.praydone) {
						u.uconduct.praydone = 1;
						You("are no longer following the prayerless conduct.");
						if (FunnyHallu) pline("Someone else must have prayed for you and the stupid gods seem to think that you were the one praying!");
					}
					break;
				case 6:
					if (!u.uconduct.weaphit) {
						u.uconduct.weaphit = 1;
						You("are no longer following the wieldedweaponless conduct.");
						if (FunnyHallu) pline("But let's face it, bare-handed combat sucks anyway, so now you're free to use whatever weapon you can get your hands on.");
					}
					break;
				case 7:
					if (!u.uconduct.killer) {
						u.uconduct.killer = 1;
						You("are no longer a pacifist.");
						if (FunnyHallu) pline("Actually, you already weren't, because the weird-ass vanilla dev team made it so that hurting a monster doesn't break the conduct even though it realistically should.");
					}
					break;
				case 8:
					if (!u.uconduct.literate) {
						u.uconduct.literate = 1;
						You("are no longer illiterate.");
						if (FunnyHallu) pline("Now you no longer have to treat scrolls and spellbooks like toilet paper, but can actually READ them! Yay!");
					}
					break;
				case 9:
					if (!u.uconduct.polypiles) {
						u.uconduct.polypiles = 1;
						You("are no longer following the polyobjectless conduct.");
						if (FunnyHallu) pline("Shame, that conduct was actually easy to keep and now you lost it through no fault of your own...");
					}
					break;
				case 10:
					if (!u.uconduct.polyselfs) {
						u.uconduct.polyselfs = 1;
						You("are no longer following the polyselfless conduct.");
						if (FunnyHallu) pline("But at least you didn't break your armor and cloak.");
					}
					break;
				case 11:
					if (!u.uconduct.wishes) {
						u.uconduct.wishes = 1;
						You("are no longer following the wishless conduct.");
						if (FunnyHallu) pline("Damn, and you didn't even get a wish to compensate for losing the conduct!");
					}
					break;
				case 12:
					if (!u.uconduct.wisharti) {
						u.uconduct.wisharti = 1;
						You("are no longer following the artiwishless conduct.");
						if (FunnyHallu) pline("Maybe you shouldn't have stepped on that particular tile, then the conduct would still be intact.");
					}
					break;
				case 13:
					if (!u.uconduct.celibacy) {
						u.uconduct.celibacy = 1;
						You("are no longer celibate.");
						if (FunnyHallu) pline("But you probably would have lost that conduct anyway, since foocubi are rather common later on.");
					}
					break;
			}

			break;

		case STRIKETHROUGH_TRAP:
			pline("Uh-oh, should have watched your step...");
			seetrap(trap);

			switch (rnd(3)) {
				case 1:
					if (u.enchantrecskill > 0) {
						u.enchantrecskill /= 2;
						You_feel("much less knowledgable about item enchantments.");
					}
					break;
				case 2:
					if (u.weapchantrecskill > 0) {
						u.weapchantrecskill /= 2;
						You_feel("much less knowledgable about weapon enchantments.");
					}
					break;
				case 3:
					if (u.bucskill > 0) {
						u.bucskill /= 2;
						You_feel("much less knowledgable about the BUC state of items.");
					}
					break;
			}

			break;

		case MULTIPLE_GATHER_TRAP:
			pline("Uh-oh, should have watched your step...");
			deltrap(trap);

			{
				int multiplegather = 0;
				register struct monst *nexusmon;
				for(nexusmon = fmon; nexusmon; nexusmon = nexusmon->nmon) {
					if (nexusmon && !nexusmon->mtame && !nexusmon->mpeaceful && !(u.usteed && (u.usteed == nexusmon) ) ) {
						mnexto(nexusmon);
						multiplegather++;
					}
				}
				if (multiplegather) pline("%d monsters were teleported to you!", multiplegather);
				else pline("Weird, nothing seems to have happened.");
			}

			break;

		case VIVISECTION_TRAP:
			seetrap(trap);
			pline("Oh no!!! You have stepped into a vivisection trap, and now the monsters can attack you with impunity until you free yourself.");

			break;
		case INSTAFEMINISM_TRAP:

			break;

		case INSTANASTY_TRAP:

			break;
		case SKILL_POINT_LOSS_TRAP:

			pline("CLICK! You have triggered a trap!");
			seetrap(trap);
			lose_weapon_skill(1);
			You("permanently lost a skill slot.");
			break;
		case PERFECT_MATCH_TRAP:
			deltrap(trap);
			pline("Hmm... apparently two monsters have a perfect match here...");

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			{
				int monstercolor = rnd(434);
				(void) makemon(specialtensmon(monstercolor), u.ux, u.uy, MM_ADJACENTOK);
				monstercolor = rnd(434);
				(void) makemon(specialtensmon(monstercolor), u.ux, u.uy, MM_ADJACENTOK);
			}
			u.aggravation = 0;

			break;
		case DUMBIE_LIGHTSABER_TRAP:

			pline("Uh-oh, should have watched your step...");
			deltrap(trap);
			bad_equipment_lightsaber();

			break;
		case WRONG_STAIRS:

			if (((u.uhave.amulet) && !u.freeplaymode) || CannotTeleport || (u.usteed && mon_has_amulet(u.usteed))) break;

			if (playerlevelportdisabled()) { 
				break;
			}

			deltrap(trap);
			{
				d_level dtmp;
				dtmp.dnum = dname_to_dnum("Gehennom");
				dtmp.dlevel = 5; /* level 55, which is the 5th level of Gehennom */
				schedule_goto(&dtmp, FALSE, FALSE, 0, (char *)0, (char *)0);
				pline("Lol, you took the wrong stairs and now you're on level 55.");
				if (FunnyHallu) pline("But to your surprise, you discover that this dungeon does indeed have a genuine level 55...");
			}

			break;
		case TECHSTOP_TRAP:
			You("stepped on a trigger!");
			seetrap(trap);
			docalm();

			break;
		case AMNESIA_SWITCH_TRAP:
			pline("CLICK! You have triggered a trap!");
			deltrap(trap);
			forget(ALL_SPELLS, FALSE);
			forget(ALL_SPELLS, FALSE);
			wonderspell(-1);
			break;

		case SKILL_SWAP_TRAP:
			deltrap(trap);
			You("stepped on a trigger!");


			{
				int skillswapamount = 1;
				int tempswapvalue;
				int tempswapvalue2;
				if (!rn2(10)) skillswapamount += rno(19);
				int tryct, tryct2, i;
				int swapskill1, swapskill2;

skillswapagain:

				swapskill1 = randomgoodskill();
skillswapredo:
				swapskill2 = randomgoodskill();
				if (swapskill1 == swapskill2) goto skillswapredo;

				if (P_MAX_SKILL(swapskill1) == P_MAX_SKILL(swapskill2)) {
					tempswapvalue = P_ADVANCE(swapskill1);
					tempswapvalue2 = P_ADVANCE(swapskill2);
					P_ADVANCE(swapskill1) = tempswapvalue2;
					P_ADVANCE(swapskill2) = tempswapvalue;

					pline("Your training for the %s and %s skills was swapped.", wpskillname(swapskill1), wpskillname(swapskill2));

					skill_sanity_check(swapskill1);
					skill_sanity_check(swapskill2);

				} else {
					tempswapvalue = P_MAX_SKILL(swapskill1);
					tempswapvalue2 = P_MAX_SKILL(swapskill2);
					P_MAX_SKILL(swapskill1) = tempswapvalue2;
					P_MAX_SKILL(swapskill2) = tempswapvalue;

					if (P_SKILL(swapskill1) == P_ISRESTRICTED && P_MAX_SKILL(swapskill1) >= P_BASIC) {
						P_SKILL(swapskill1) = P_UNSKILLED;
						P_ADVANCE(swapskill1) = 0;
					}
					if (P_SKILL(swapskill2) == P_ISRESTRICTED && P_MAX_SKILL(swapskill2) >= P_BASIC) {
						P_SKILL(swapskill2) = P_UNSKILLED;
						P_ADVANCE(swapskill2) = 0;
					}

					pline("Your caps for the %s and %s skills were swapped.", wpskillname(swapskill1), wpskillname(swapskill2));

					skill_sanity_check(swapskill1);
					skill_sanity_check(swapskill2);

				}

				if (skillswapamount > 1) {
					skillswapamount--;
					goto skillswapagain;
				}

			}

			break;
		case SKILL_UPORDOWN_TRAP:
			deltrap(trap);
			You("stepped on a trigger!");

			{
				int skillupordownamount = 1;
				if (!rn2(10)) skillupordownamount += rno(19);
				int tryct, tryct2, i;
				int upordownskill;

skillupordownagain:

				upordownskill = randomgoodskill();
				boolean will_go_up = rn2(2);

				if (will_go_up) {
					if (P_MAX_SKILL(upordownskill) == P_SUPREME_MASTER) {
						pline("But nothing happened.");
					} else if (P_MAX_SKILL(upordownskill) == P_GRAND_MASTER) {
						P_MAX_SKILL(upordownskill) = P_SUPREME_MASTER;
						pline("Your knowledge of the %s skill increases.", wpskillname(upordownskill));
					} else if (P_MAX_SKILL(upordownskill) == P_MASTER) {
						P_MAX_SKILL(upordownskill) = P_GRAND_MASTER;
						pline("Your knowledge of the %s skill increases.", wpskillname(upordownskill));
					} else if (P_MAX_SKILL(upordownskill) == P_EXPERT) {
						P_MAX_SKILL(upordownskill) = P_MASTER;
						pline("Your knowledge of the %s skill increases.", wpskillname(upordownskill));
					} else if (P_MAX_SKILL(upordownskill) == P_SKILLED) {
						P_MAX_SKILL(upordownskill) = P_EXPERT;
						pline("Your knowledge of the %s skill increases.", wpskillname(upordownskill));
					} else if (P_MAX_SKILL(upordownskill) == P_BASIC) {
						P_MAX_SKILL(upordownskill) = P_SKILLED;
						pline("Your knowledge of the %s skill increases.", wpskillname(upordownskill));
					} else if (P_MAX_SKILL(upordownskill) == P_ISRESTRICTED) {
						unrestrict_weapon_skill(upordownskill);
						pline("You can now learn the %s skill.", wpskillname(upordownskill));
					}
				} else {

					if (P_MAX_SKILL(upordownskill) == P_ISRESTRICTED) {
						P_ADVANCE(upordownskill) = 0;
						You("lost some hidden skill training.");
					} else if (P_MAX_SKILL(upordownskill) == P_BASIC) {
						P_MAX_SKILL(upordownskill) = P_ISRESTRICTED;
						pline("You lose all knowledge of the %s skill!", wpskillname(upordownskill));
						P_ADVANCE(upordownskill) = 0;
					} else if (P_MAX_SKILL(upordownskill) == P_SKILLED) {
						P_MAX_SKILL(upordownskill) = P_BASIC;
						pline("You lose some knowledge of the %s skill!", wpskillname(upordownskill));
					} else if (P_MAX_SKILL(upordownskill) == P_EXPERT) {
						P_MAX_SKILL(upordownskill) = P_SKILLED;
						pline("You lose some knowledge of the %s skill!", wpskillname(upordownskill));
					} else if (P_MAX_SKILL(upordownskill) == P_MASTER) {
						P_MAX_SKILL(upordownskill) = P_EXPERT;
						pline("You lose some knowledge of the %s skill!", wpskillname(upordownskill));
					} else if (P_MAX_SKILL(upordownskill) == P_GRAND_MASTER) {
						P_MAX_SKILL(upordownskill) = P_MASTER;
						pline("You lose some knowledge of the %s skill!", wpskillname(upordownskill));
					} else if (P_MAX_SKILL(upordownskill) == P_SUPREME_MASTER) {
						P_MAX_SKILL(upordownskill) = P_GRAND_MASTER;
						pline("You lose some knowledge of the %s skill!", wpskillname(upordownskill));
					}

					skill_sanity_check(upordownskill);

				}

				if (skillupordownamount > 1) {
					skillupordownamount--;
					goto skillupordownagain;
				}

			}

			break;
		case SKILL_RANDOMIZE_TRAP:
 			deltrap(trap);
			You("stepped on a trigger!");

			{

				int skillrandomizeamount = 1;
				if (!rn2(10)) skillrandomizeamount += rno(19);
				int tryct, tryct2, i;
				int randomizeskill;

skillrandomizeagain:

				tryct = 2000;

skillrandomizeredo:
				randomizeskill = randomgoodskill();

				/* if the skill's cap is lower than expert, then its new cap can't be higher than skilled
				 * if it's at least expert, the new cap can't be lower than expert */

				if (P_MAX_SKILL(randomizeskill) < P_EXPERT) {
					if (rn2(2)) {
						if (P_MAX_SKILL(randomizeskill) >= P_BASIC) {
							P_MAX_SKILL(randomizeskill) = P_ISRESTRICTED;
							P_ADVANCE(randomizeskill) = 0;
						}
						P_MAX_SKILL(randomizeskill) = P_ISRESTRICTED;
						pline("Your %s skill is now restricted!", wpskillname(randomizeskill));
					} else if (rn2(3)) {
						if (P_MAX_SKILL(randomizeskill) == P_ISRESTRICTED) {
							unrestrict_weapon_skill(randomizeskill);
							P_MAX_SKILL(randomizeskill) = P_BASIC;
						}
						P_MAX_SKILL(randomizeskill) = P_BASIC;
						pline("Your %s skill is now capped at basic!", wpskillname(randomizeskill));
					} else {
						if (P_MAX_SKILL(randomizeskill) == P_ISRESTRICTED) {
							unrestrict_weapon_skill(randomizeskill);
							P_MAX_SKILL(randomizeskill) = P_SKILLED;
						}
						P_MAX_SKILL(randomizeskill) = P_SKILLED;
						pline("Your %s skill is now capped at skilled!", wpskillname(randomizeskill));
					}
				} else {
					if (!rn2(100)) {
						P_MAX_SKILL(randomizeskill) = P_SUPREME_MASTER;
						pline("Your %s skill is now capped at supreme master!", wpskillname(randomizeskill));
					} else if (!rn2(10)) {
						P_MAX_SKILL(randomizeskill) = P_GRAND_MASTER;
						pline("Your %s skill is now capped at grand master!", wpskillname(randomizeskill));
					} else if (!rn2(2)) {
						P_MAX_SKILL(randomizeskill) = P_MASTER;
						pline("Your %s skill is now capped at master!", wpskillname(randomizeskill));
					} else {
						P_MAX_SKILL(randomizeskill) = P_EXPERT;
						pline("Your %s skill is now capped at expert!", wpskillname(randomizeskill));
					}
				}

				skill_sanity_check(randomizeskill);

				if (skillrandomizeamount > 1) {
					skillrandomizeamount--;
					goto skillrandomizeagain;
				}

			}

			break;

		 case MIGUC_TRAP:
			/* evil patch idea, inspired by ais523 IIRC? spawn monster and paralyze player --Amy */

			pline("Oh no, you stepped on a miguc trap!");
			seetrap(trap);
			if (!rn2(15)) deltrap(trap);

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			(void) makemon((struct permonst *) 0, u.ux, u.uy, NO_MM_FLAGS);
			if (isstunfish) nomul(-(rnz(5) ), "paralyzed by a miguc trap", TRUE);
			else nomul(-(rnd(5) ), "paralyzed by a miguc trap", TRUE);

			u.aggravation = 0;

		 break;

		 case DIRECTIVE_TRAP:

			pline("You stepped on a trigger!");
			deltrap(trap);

			switch (rnd(5)) {

				case 1:
					if (u.petcollectitems) {
						u.petcollectitems = 0;
						pline("Your pets can no longer pick up items.");
					} else {
						u.petcollectitems = 1;
						pline("Your pets can pick up items now.");
					}
					break;
				case 2:
					if (u.petattackenemies == 2) {
						u.petattackenemies = !rn2(3) ? 0 : rn2(2) ? -1 : 1;
					} else if (u.petattackenemies == 1) {
						u.petattackenemies = !rn2(3) ? 0 : rn2(2) ? -1 : 2;
					} else if (u.petattackenemies == 0) {
						u.petattackenemies = !rn2(3) ? 2 : rn2(2) ? -1 : 1;
					} else if (u.petattackenemies == -1) {
						u.petattackenemies = !rn2(3) ? 0 : rn2(2) ? 2 : 1;
					}
					if (u.petattackenemies == 2) pline("Your pets can attack all monsters now.");
					else if (u.petattackenemies == 1) pline("Your pets can attack hostile monsters now, but will leave peaceful ones alone.");
					else if (u.petattackenemies == -1) pline("Your pets can't attack monsters now and also aren't attacked by enemies anymore.");
					else pline("Your pets can't attack monsters now.");


					break;
				case 3:
					if (u.petcaneat) {
						u.petcaneat = 0;
						pline("Your pets can no longer eat food off the ground.");
					} else {
						u.petcaneat = 1;
						pline("Your pets can eat food off the ground now.");
					}
					break;
				case 4:
					if (u.petcanfollow) {
						u.petcanfollow = 0;
						pline("Your pets can no longer try to follow you.");
					} else {
						u.petcanfollow = 1;
						pline("Your pets can try to follow you now.");
					}
					break;
				case 5:
					u.steedhitchance = rn2(101);
					pline("The chance that attacks target your steed is %d%% now.", u.steedhitchance);
					break;

			}

		 break;

		 case SATATUE_TRAP:

			activate_statue_trap(trap, u.ux, u.uy, FALSE);

		 break;

		 case FARTING_WEB:

		seetrap(trap);

		if (webmsgok) {
		   char verbbuf[BUFSZ];
		   verbbuf[0] = '\0';
		   if (u.usteed)
		   	sprintf(verbbuf,"lead %s",
				x_monnam(u.usteed,
					 u.usteed->mnamelth ? ARTICLE_NONE : ARTICLE_THE,
				 	 "poor", SUPPRESS_SADDLE, FALSE));
		   else

		    sprintf(verbbuf, "%s", Levitation ? (const char *)"float" :
		      		locomotion(youmonst.data, "stumble"));
		    You("%s into %s farting web!",
			verbbuf, a_your[trap->madeby_u]);

			pline("The web was apparently set by %s, who can now freely fart into your face.", farttrapnames[trap->launch_otyp]);
		}
		u.utraptype = TT_WEB;

		/* Time stuck in the web depends on your/steed strength.
		 * Amy note: unlike regular webs, this one still traps you, not the steed */

		{
		    register int str = ACURR(A_STR);

		    if (str <= 1) u.utrap = rn1(24,6);
		    else if (str <= 2) u.utrap = rn1(12,6);
		    else if (str <= 3) u.utrap = rn1(6,6);
		    else if (str < 6) u.utrap = rn1(6,4);
		    else if (str < 9) u.utrap = rn1(4,4);
		    else if (str < 12) u.utrap = rn1(4,2);
		    else if (str < 15) u.utrap = rn1(3,2);
		    else if (str < 18) u.utrap = rn1(2,2);
		    else if (str < 69) u.utrap = rnd(2);
		    else u.utrap = 1;
		}

		 break;

		case S_PRESSING_TRAP:
			break;

		 case CATACLYSM_TRAP:

			if (u.ragnaroktimer) break; /* don't reveal or do anything for that matter */

			pline("CLICK! You have triggered a trap!");
			deltrap(trap);
			u.ragnaroktimer = rnz(100000);
			pline("Ragnarok is drawing near...");

		 break;

		 case DATA_DELETE_TRAP:

			u.datadeletedefer = 1;
			pline("CLICK! You have triggered a trap!");
			deltrap(trap);
			datadeleteattack();

		 break;

		 case ELDER_TENTACLING_TRAP:

			{
			register struct monst *elderpriest;

			pline("CLICK! You have triggered a trap!");
			deltrap(trap);

			if (elderpriest = makemon(&mons[PM_DNETHACK_ELDER_PRIEST_TM_], u.ux, u.uy, MM_ANGRY|MM_FRENZIED)) {
				pline("The most dangerous monster in existence was just generated next to you, which means you're in big trouble.");
			} else {
				pline("You feel an immense sense of relief as you realize that the trap must have failed to trigger.");
			}

			}

		 break;

		 case FOOTERER_TRAP:

			if (u.footererlevel) break; /* don't reveal or do anything for that matter */

			pline("CLICK! You have triggered a trap!");
			deltrap(trap);

			u.footererlevel = rnd(100);
			while (u.footererlevel == depth(&u.uz)) u.footererlevel = rnd(100);

			pline("The footerers are waiting for you on some level...");

		 break;

		 case DISCONNECT_TRAP:

			if (DisconnectedStairs) break;
			u.cnd_nastytrapamount++;

			DisconnectedStairs = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case INTERFACE_SCREW_TRAP:

			if (InterfaceScrewed) break;
			u.cnd_nastytrapamount++;

			InterfaceScrewed = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case BOSSFIGHT_TRAP:

			if (Bossfights) break;
			u.cnd_nastytrapamount++;

			Bossfights = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case ENTIRE_LEVEL_TRAP:

			if (EntireLevelMode) break;
			u.cnd_nastytrapamount++;

			EntireLevelMode = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case BONES_TRAP:

			if (BonesLevelChange) break;
			u.cnd_nastytrapamount++;

			BonesLevelChange = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case AUTOCURSE_TRAP:

			if (AutocursingEquipment) break;
			u.cnd_nastytrapamount++;

			AutocursingEquipment = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case HIGHLEVEL_TRAP:

			if (HighlevelStatus) break;
			u.cnd_nastytrapamount++;

			HighlevelStatus = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case SPELL_FORGETTING_TRAP:

			if (SpellForgetting) break;
			u.cnd_nastytrapamount++;

			SpellForgetting = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case SOUND_EFFECT_TRAP:

			if (SoundEffectBug) break;
			u.cnd_nastytrapamount++;

			SoundEffectBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case FARLOOK_TRAP:

			if (FarlookProblem) break;
			u.cnd_nastytrapamount++;

			FarlookProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case RESPAWN_TRAP:

			if (RespawnProblem) break;
			u.cnd_nastytrapamount++;

			RespawnProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case CAPTCHA_TRAP:

			if (CaptchaProblem) break;
			u.cnd_nastytrapamount++;

			CaptchaProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case FEMMY_TRAP:

			if (FemaleTrapFemmy) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Femmy.");
				pline("Now, the dungeon will be more feminine for a while!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapFemmy = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapFemmy += 100;
			if (!rn2(3)) FemaleTrapFemmy += rnz(500);

		 break;

		 case MADELEINE_TRAP:

			if (FemaleTrapMadeleine) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Madeleine.");
				pline("Your shins can expect to get kicked repeatedly by all the girls and women now!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapMadeleine = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapMadeleine += 100;
			if (!rn2(3)) FemaleTrapMadeleine += rnz(500);

		 break;

		case NADINE_TRAP:

			if (FemaleTrapNadine) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Nadine.");
				pline("Some dark feminists are coming to the dungeon.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapNadine = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapNadine += 100;
			if (!rn2(3)) FemaleTrapNadine += rnz(500);

		 break;

		case LUISA_TRAP:

			if (FemaleTrapLuisa) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Luisa.");
				pline("The girl shoes are starting to cheat in an attempt to kick you!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapLuisa = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapLuisa += 100;
			if (!rn2(3)) FemaleTrapLuisa += rnz(500);

		 break;

		case IRINA_TRAP:

			if (FemaleTrapIrina) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Irina.");
				pline("You'll have to fight a whole lot of women's shoes.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapIrina = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapIrina += 100;
			if (!rn2(3)) FemaleTrapIrina += rnz(500);

		 break;

		case LISELOTTE_TRAP:

			if (FemaleTrapLiselotte) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Liselotte.");
				pline("The little girl wants to kick you in the shins and then retreat rapidly so that you can't attack her back!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapLiselotte = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapLiselotte += 100;
			if (!rn2(3)) FemaleTrapLiselotte += rnz(500);

		 break;

		case GRETA_TRAP:

			if (FemaleTrapGreta) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Greta.");
				pline("The Hussies are waiting for you on certain dungeon levels, because they have been hired by the Bang Gang bosses.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapGreta = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapGreta += 100;
			if (!rn2(3)) FemaleTrapGreta += rnz(500);

		 break;

		case JANE_TRAP:

			if (FemaleTrapJane) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Jane.");
				pline("Don't underestimate the women just because they have a small stature.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapJane = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapJane += 100;
			if (!rn2(3)) FemaleTrapJane += rnz(500);

		 break;

		case SUE_LYN_TRAP:

			if (FemaleTrapSueLyn) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Sue Lyn.");
				pline("The Asian girls want to fart you in the face, and if you allow it, they'll be nice to you, but treat them badly and they'll slit your skin with very sharp female fingernails!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapSueLyn = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapSueLyn += 100;
			if (!rn2(3)) FemaleTrapSueLyn += rnz(500);

		 break;

		case CHARLOTTE_TRAP:

			if (FemaleTrapCharlotte) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Charlotte.");
				pline("Watch out, the Bang Gang is considering you a target. They're coming for you, and they have some tricks and traps ready.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapCharlotte = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapCharlotte += 100;
			if (!rn2(3)) FemaleTrapCharlotte += rnz(500);

		 break;

		case HANNAH_TRAP:

			if (FemaleTrapHannah) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Hannah.");
				pline("Careful, the girls may steal your stuff and vanish before your eyes!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapHannah = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapHannah += 100;
			if (!rn2(3)) FemaleTrapHannah += rnz(500);

		 break;

		case LITTLE_MARIE_TRAP:

			if (FemaleTrapLittleMarie) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Little Marie.");
				pline("Oh no! Apparently the girls and women have prepared a really nasty counterattack to make your life miserable!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapLittleMarie = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapLittleMarie += 100;
			if (!rn2(3)) FemaleTrapLittleMarie += rnz(500);

		 break;

		 case MARLENA_TRAP:

			if (FemaleTrapMarlena) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Marlena.");
				pline("Green is the new favorite color, it seems!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapMarlena = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapMarlena += 100;
			if (!rn2(3)) FemaleTrapMarlena += rnz(500);

		 break;

		 case SABRINA_TRAP:

			if (FemaleTrapSabrina) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Sabrina.");
				pline("Wear Sabrina's platform boots! Now! You just know that if you don't, some angry women will try to kill you.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapSabrina = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapSabrina += 100;
			if (!rn2(3)) FemaleTrapSabrina += rnz(500);

		 break;

		 case TANJA_TRAP:

			if (FemaleTrapTanja) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Tanja.");
				pline("The girls learned takewondo, and will knock the crap out of you with their female sneakers.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapTanja = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapTanja += 100;
			if (!rn2(3)) FemaleTrapTanja += rnz(500);

		 break;

		 case SONJA_TRAP:

			if (FemaleTrapSonja) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Sonja.");
				pline("Seems that the girls will call for help if you attack them.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapSonja = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapSonja += 100;
			if (!rn2(3)) FemaleTrapSonja += rnz(500);

		 break;

		 case RHEA_TRAP:

			if (FemaleTrapRhea) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Rhea.");
				pline("Ugh, the girls and women are really going to beguile you with the horrible odor of their perfume!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapRhea = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapRhea += 100;
			if (!rn2(3)) FemaleTrapRhea += rnz(500);

		 break;

		 case LARA_TRAP:

			if (FemaleTrapLara) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Lara.");
				pline("You look forward to being attacked by treaded block heel boots while you're bound to a stick.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapLara = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapLara += 100;
			if (!rn2(3)) FemaleTrapLara += rnz(500);

		 break;

		 case RUTH_TRAP:

			if (FemaleTrapRuth) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			/* no message
			 * pline("Whoops... you seem to have stumbled into a trap that was set by Ruth.");
			 * pline("The dark girl has pulled the switch, and now feminism traps will work like nasty traps. Except that this message can never be displayed, because the trap in question works like a nasty trap itself."); */
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapRuth = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapRuth += 100;
			if (!rn2(3)) FemaleTrapRuth += rnz(500);

		 break;

		 case MAGDALENA_TRAP:

			if (FemaleTrapMagdalena) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Magdalena.");
				pline("Oh, apparently you have to fight some particularly burly women. That's gonna be difficult.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapMagdalena = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapMagdalena += 100;
			if (!rn2(3)) FemaleTrapMagdalena += rnz(500);

		 break;

		 case MARLEEN_TRAP:

			if (FemaleTrapMarleen) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Marleen.");
				pline("It seems the women are strengthening their defenses.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapMarleen = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapMarleen += 100;
			if (!rn2(3)) FemaleTrapMarleen += rnz(500);

		 break;

		 case KLARA_TRAP:

			if (FemaleTrapKlara) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Klara.");
				pline("You become sensitive to the sound of high heels.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapKlara = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapKlara += 100;
			if (!rn2(3)) FemaleTrapKlara += rnz(500);

		 break;

		 case FRIEDERIKE_TRAP:

			if (FemaleTrapFriederike) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Friederike.");
				pline("Yay, a pretty girl with a long blond bundle starts following you around! She's certainly very nice and will have a lot of fun with you!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapFriederike = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapFriederike += 100;
			if (!rn2(3)) FemaleTrapFriederike += rnz(500);

		 break;

		 case NAOMI_TRAP:

			if (FemaleTrapNaomi) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Naomi.");
				pline("You have to wear your stiletto heels like a model, and if you fall, you have to smile anyway! That's the obligation of a model...");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapNaomi = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapNaomi += 100;
			if (!rn2(3)) FemaleTrapNaomi += rnz(500);

		 break;

		 case UTE_TRAP:

			if (FemaleTrapUte) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Ute.");
				pline("Uh-oh, the patriarch is coming to take your pretty high heels away. Make sure he doesn't catch you!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapUte = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapUte += 100;
			if (!rn2(3)) FemaleTrapUte += rnz(500);

		 break;

		 case JASIEEN_TRAP:

			if (FemaleTrapJasieen) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Jasieen.");
				pline("The sexy ladies are gonna challenge you again and again and again.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapJasieen = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapJasieen += 100;
			if (!rn2(3)) FemaleTrapJasieen += rnz(500);

		 break;

		 case YASAMAN_TRAP:

			if (FemaleTrapYasaman) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Yasaman.");
				pline("It seems that monsters may spotaneously turn into sexy women, as if there weren't enough of them running around the dungeon anyway.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapYasaman = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapYasaman += 100;
			if (!rn2(3)) FemaleTrapYasaman += rnz(500);

		 break;

		 case MAY_BRITT_TRAP:

			if (FemaleTrapMayBritt) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by May-Britt.");
				pline("Those little girls want to tease you by trying to steal things from your backpack for fun! You certainly know that this 'fun' will happen at your expense...");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapMayBritt = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapMayBritt += 100;
			if (!rn2(3)) FemaleTrapMayBritt += rnz(500);

		 break;

		 case KSENIA_TRAP:

			if (FemaleTrapKsenia) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Ksenia.");
				pline("The incredibly cute female platform sandals are looking forward to kicking you in the shins like a little girl.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapKsenia = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapKsenia += 100;
			if (!rn2(3)) FemaleTrapKsenia += rnz(500);

		 break;

		 case LYDIA_TRAP:

			if (FemaleTrapLydia) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Lydia.");
				pline("You feel that you're gonna fight ladies who wear highly elegant cone-heeled pumps.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapLydia = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapLydia += 100;
			if (!rn2(3)) FemaleTrapLydia += rnz(500);

		 break;

		 case CONNY_TRAP:

			if (FemaleTrapConny) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Conny.");
				pline("Uh-oh, you're not sure that you can escape from the thick women...");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapConny = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapConny += 100;
			if (!rn2(3)) FemaleTrapConny += rnz(500);

		 break;

		 case KATIA_TRAP:

			if (FemaleTrapKatia) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Katia.");
				pline("Some little girl really wants to take a crap while you are nearby.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapKatia = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapKatia += 100;
			if (!rn2(3)) FemaleTrapKatia += rnz(500);

		 break;

		 case MARIYA_TRAP:

			if (FemaleTrapMariya) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Mariya.");
				pline("You feel that some stinking girl is waiting until you're defenseless so that she can fart you into the face without you fighting back!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapMariya = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapMariya += 100;
			if (!rn2(3)) FemaleTrapMariya += rnz(500);

		 break;

		 case ELISE_TRAP:

			if (FemaleTrapElise) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Elise.");
				pline("The females are everywhere, it seems!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapElise = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapElise += 100;
			if (!rn2(3)) FemaleTrapElise += rnz(500);

		 break;

		 case RONJA_TRAP:

			if (FemaleTrapRonja) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Ronja.");
				pline("Somehow, the women have learned to heal their wounds quickly.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapRonja = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapRonja += 100;
			if (!rn2(3)) FemaleTrapRonja += rnz(500);

		 break;

		 case ARIANE_TRAP:

			if (FemaleTrapAriane) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Ariane.");
				pline("You'll have to fight the girls from the Bang Gang as well as Anna's hussies, just like in the old times.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapAriane = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapAriane += 100;
			if (!rn2(3)) FemaleTrapAriane += rnz(500);

		 break;

		 case JOHANNA_TRAP:

			if (FemaleTrapJohanna) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Johanna.");
				pline("Admit it, you fetishize female footwear, especially if they're block-heeled combat boots with long zippers and fleecy colors.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapJohanna = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapJohanna += 100;
			if (!rn2(3)) FemaleTrapJohanna += rnz(500);

		 break;

		 case INGE_TRAP:

			if (FemaleTrapInge) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Inge.");
				pline("All the elegant ladies will produce very tender farting noises that you just cannot resist.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapInge = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapInge += 100;
			if (!rn2(3)) FemaleTrapInge += rnz(500);

		 break;

		 case ROSA_TRAP:

			if (FemaleTrapRosa) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Rosa.");
				pline("You'll have to struggle with feminism for a long time...");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapRosa = rnz(femmytrapdur * 5 * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapRosa += 100;
			if (!rn2(3)) FemaleTrapRosa += rnz(500);

		 break;

		 case JANINA_TRAP:

			if (FemaleTrapJanina) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Janina.");
				pline("Those women are gonna hang their worn pants in front of your %s, forcing you to smell them!", body_part(NOSE));
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapJanina = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapJanina += 100;
			if (!rn2(3)) FemaleTrapJanina += rnz(500);

		 break;

		case ANNEMARIE_TRAP:
			if (FemaleTrapAnnemarie) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Annemarie.");
				pline("It seems that the women are doing athletic exercises.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapAnnemarie = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapAnnemarie += 100;
			if (!rn2(3)) FemaleTrapAnnemarie += rnz(500);

		 break;

		case JIL_TRAP:
			if (FemaleTrapJil) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Jil.");
				pline("The girls are planning to attack you with their sweaty socks!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapJil = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapJil += 100;
			if (!rn2(3)) FemaleTrapJil += rnz(500);

		 break;

		case JANA_TRAP:
			if (FemaleTrapJana) break;
			if (!dontreveal && !FemtrapActiveRuth && !(in_rooms(u.ux, u.uy, INSIDEROOM)) ) seetrap(trap);

			if (!FemtrapActiveRuth && !(in_rooms(u.ux, u.uy, INSIDEROOM)) ) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Jana.");
				pline("Someone's hiding underneath a grave wall, but some of the grave walls have teleporters or cursed calleds underneath, so be careful!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapJana = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapJana += 100;
			if (!rn2(3)) FemaleTrapJana += rnz(500);

		 break;

		case KATRIN_TRAP:
			if (FemaleTrapKatrin) break;
			if (!dontreveal && !FemtrapActiveRuth && !(in_rooms(u.ux, u.uy, INSIDEROOM)) ) seetrap(trap);

			if (!FemtrapActiveRuth && !(in_rooms(u.ux, u.uy, INSIDEROOM)) ) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Katrin.");
				pline("Some girls are planning to use their sticky chewing gum as a trap to prevent you from getting away!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapKatrin = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapKatrin += 100;
			if (!rn2(3)) FemaleTrapKatrin += rnz(500);

		 break;

		case GUDRUN_TRAP:
			if (FemaleTrapGudrun) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Gudrun.");
				pline("A stringy feminist named Gudrun is challenging you...");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapGudrun = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapGudrun += 100;
			if (!rn2(3)) FemaleTrapGudrun += rnz(500);

		 break;

		case ELLA_TRAP:
			if (FemaleTrapElla) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Ella.");
				pline("A fearless feminist named Ella is challenging you...");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapElla = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapElla += 100;
			if (!rn2(3)) FemaleTrapElla += rnz(500);

		 break;

		case MANUELA_TRAP:
			if (FemaleTrapManuela) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Manuela.");
				pline("Some unfair woman wants to crush you with her block-heeled combat boots!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapManuela = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapManuela += 100;
			if (!rn2(3)) FemaleTrapManuela += rnz(500);

		 break;

		case JENNIFER_TRAP:
			if (FemaleTrapJennifer) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Jennifer.");
				pline("The girls are going to fart you in the face without emitting any kind of sound.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapJennifer = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapJennifer += 100;
			if (!rn2(3)) FemaleTrapJennifer += rnz(500);

		 break;

		case PATRICIA_TRAP:
			if (FemaleTrapPatricia) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Patricia.");
				pline("You feel like a beautifully thick girl, capable of splitting enemies' skulls with blunt weapons!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapPatricia = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapPatricia += 100;
			if (!rn2(3)) FemaleTrapPatricia += rnz(500);

		 break;

		case ANTJE_TRAP:
			if (FemaleTrapAntje) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Antje.");
				pline("You feel like walking the dyke with your extra thick block heels.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapAntje = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapAntje += 100;
			if (!rn2(3)) FemaleTrapAntje += rnz(500);

		 break;

		case ANTJE_TRAP_X:
			if (FemaleTrapAntjeX) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a special trap that was set by Antje.");
				pline("Something tells you that you'd like to visit the public toilet.");
			}
			u.antjetwotriggered = TRUE;
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapAntjeX = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapAntjeX += 100;
			if (!rn2(3)) FemaleTrapAntjeX += rnz(500);

		 break;

		case KERSTIN_TRAP:
			if (FemaleTrapKerstin) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Kerstin.");
				pline("Those farmer girls plan to use block-heeled lady boots, wooden sandals and scentful sneakers on you!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapKerstin = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapKerstin += 100;
			if (!rn2(3)) FemaleTrapKerstin += rnz(500);

		 break;

		case LAURA_TRAP:
			if (FemaleTrapLaura) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Laura.");
				pline("Every woman is going to attack you with their clothing.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapLaura = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapLaura += 100;
			if (!rn2(3)) FemaleTrapLaura += rnz(500);

		 break;

		case LARISSA_TRAP:
			if (FemaleTrapLarissa) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Larissa.");
				pline("You are commanded to seek out heaps of dog shit and step into it with your high heels. Something tells you that there are bad consequences if you don't obey this instruction.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapLarissa = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapLarissa += 100;
			if (!rn2(3)) FemaleTrapLarissa += rnz(500);

		 break;

		case NORA_TRAP:
			if (FemaleTrapNora) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Nora.");
				pline("You feel that eating is a sin and should be avoided at all costs, even though that may end up killing you.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapNora = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapNora += 100;
			if (!rn2(3)) FemaleTrapNora += rnz(500);

		 break;

		case NATALIA_TRAP:
			if (FemaleTrapNatalia) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Natalia.");
				if (flags.female) pline("Ack! You suddenly have to deal with the effects of your menstruational period!");
				else pline("Some russian girl decided that you're a very naughty boy, and will continuously bash your head with her high-heeled sandals.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapNatalia = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapNatalia += 100;
			if (!rn2(3)) FemaleTrapNatalia += rnz(500);

		 break;

		case SUSANNE_TRAP:
			if (FemaleTrapSusanne) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Susanne.");
				pline("You might have to fight some particularly powerful woman!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapSusanne = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapSusanne += 100;
			if (!rn2(3)) FemaleTrapSusanne += rnz(500);

		 break;

		case LISA_TRAP:
			if (FemaleTrapLisa) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Lisa.");
				pline("You're interested in seeing what kind of underwear the girls are wearing.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapLisa = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapLisa += 100;
			if (!rn2(3)) FemaleTrapLisa += rnz(500);

		 break;

		case BRIDGHITTE_TRAP:
			if (FemaleTrapBridghitte) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Bridghitte.");
				pline("Watch out, walking over the bridge has a large chance of you stepping into a heap of dog shit unexpectedly!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapBridghitte = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapBridghitte += 100;
			if (!rn2(3)) FemaleTrapBridghitte += rnz(500);

		 break;

		case JULIA_TRAP:
			if (FemaleTrapJulia) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Julia.");
				pline("Since it's unfair that you can simply hit women whenever you want to, you can no longer freely do so.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapJulia = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapJulia += 100;
			if (!rn2(3)) FemaleTrapJulia += rnz(500);

		 break;

		case NICOLE_TRAP:
			if (FemaleTrapNicole) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Nicole.");
				pline("The women are apparently surrounded by a dark aura! Is it some kind of black light, maybe?");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapNicole = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapNicole += 100;
			if (!rn2(3)) FemaleTrapNicole += rnz(500);

		 break;

		case RITA_TRAP:
			if (FemaleTrapRita) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Rita.");
				pline("Hahahahaha, now the women can just trigger feminism traps at will and when they do, the trap effect in question will be activated for YOU! Hahaha!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapRita = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapRita += 100;
			if (!rn2(3)) FemaleTrapRita += rnz(500);

		 break;


		 case KRISTIN_TRAP:

			if (FemaleTrapKristin) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Kristin.");
				pline("You feel that there are women around who really like various high heels.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapKristin = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapKristin += 100;
			if (!rn2(3)) FemaleTrapKristin += rnz(500);

		 break;

		 case ANNA_TRAP:

			if (FemaleTrapAnna) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Anna.");
				pline("Oh, it seems that the hussies are on the loose!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapAnna = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapAnna += 100;
			if (!rn2(3)) FemaleTrapAnna += rnz(500);

		 break;

		 case RUEA_TRAP:

			if (FemaleTrapRuea) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Ruea.");
				pline("You get the feeling that some women are trying to convert you.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapRuea = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapRuea += 100;
			if (!rn2(3)) FemaleTrapRuea += rnz(500);

		 break;

		 case DORA_TRAP:

			if (FemaleTrapDora) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Dora.");
				pline("Ack! There's birds on the loose, and they want to eat your shoes!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapDora = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapDora += 100;
			if (!rn2(3)) FemaleTrapDora += rnz(500);

		 break;

		 case MARIKE_TRAP:

			if (FemaleTrapMarike) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Marike.");
				pline("You want to endlessly listen to squeaking farting noises.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapMarike = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapMarike += 100;
			if (!rn2(3)) FemaleTrapMarike += rnz(500);

		 break;

		 case JETTE_TRAP:

			if (FemaleTrapJette) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Jette.");
				pline("The power of feminism compels you.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapJette = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapJette += 100;
			if (!rn2(3)) FemaleTrapJette += rnz(500);

		 break;

		 case INA_TRAP:

			if (FemaleTrapIna) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Ina.");
				pline("Oh my god, you are now afflicted with the life-threatening disease known as anorexia!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapIna = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapIna += 100;
			if (!rn2(3)) FemaleTrapIna += rnz(500);

		 break;

		 case SING_TRAP:

			if (FemaleTrapSing) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Sing.");
				pline("A heinously evil woman plans to force you to clean the shit from all kinds of female shoes...");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapSing = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapSing += 100;
			if (!rn2(3)) FemaleTrapSing += rnz(500);

		 break;

		 case VICTORIA_TRAP:

			if (FemaleTrapVictoria) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Victoria.");
				pline("There's some karate women who want to demonstrate their combat capabilities to you.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapVictoria = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapVictoria += 100;
			if (!rn2(3)) FemaleTrapVictoria += rnz(500);

		 break;

		 case MELISSA_TRAP:

			if (FemaleTrapMelissa) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Melissa.");
				pline("You suddenly feel that the women in this dungeon are quite attractive...");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapMelissa = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapMelissa += 100;
			if (!rn2(3)) FemaleTrapMelissa += rnz(500);

		 break;

		 case ANITA_TRAP:

			if (FemaleTrapAnita) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Anita.");
				pline("The women want to slit your legs with razor-sharp high heels! Be afraid of them!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapAnita = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapAnita += 100;
			if (!rn2(3)) FemaleTrapAnita += rnz(500);

		 break;

		 case HENRIETTA_TRAP:

			if (FemaleTrapHenrietta) break;
			if (!dontreveal && !FemtrapActiveRuth && !(in_rooms(u.ux, u.uy, INSIDEROOM)) ) seetrap(trap);

			if (!FemtrapActiveRuth && !(in_rooms(u.ux, u.uy, INSIDEROOM)) ) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Henrietta.");
				pline("You feel that someone is going to open the zippers of your boots, making you fumble into a heap of dog shit.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapHenrietta = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapHenrietta += 100;
			if (!rn2(3)) FemaleTrapHenrietta += rnz(500);

		 break;

		 case VERENA_TRAP:

			if (FemaleTrapVerena) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Verena.");
				pline("Oh no, some annoying blonde girl starts to follow you around. Sigh. Why can't you get a clever companion instead?!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapVerena = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapVerena += 100;
			if (!rn2(3)) FemaleTrapVerena += rnz(500);

		 break;

		 case ARABELLA_TRAP:

			if (FemaleTrapArabella) break;
			if (!dontreveal && !FemtrapActiveRuth && !(in_rooms(u.ux, u.uy, INSIDEROOM)) ) seetrap(trap);

			if (!FemtrapActiveRuth && !(in_rooms(u.ux, u.uy, INSIDEROOM)) ) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Arabella.");
				pline("Oh no, the master of nasty traps is laying out her snares to get you!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapArabella = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapArabella += 100;
			if (!rn2(3)) FemaleTrapArabella += rnz(500);

		 break;

		 case NELLY_TRAP:

			if (FemaleTrapNelly) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Nelly.");
				pline("You fear that the women are going to crush you with a hug.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapNelly = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapNelly += 100;
			if (!rn2(3)) FemaleTrapNelly += rnz(500);

		 break;

		 case EVELINE_TRAP:

			if (FemaleTrapEveline) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Eveline.");
				pline("It seems that the women are running faster.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapEveline = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapEveline += 100;
			if (!rn2(3)) FemaleTrapEveline += rnz(500);

		 break;

		 case KARIN_TRAP:

			if (FemaleTrapKarin) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Karin.");
				pline("All the women want to knee you in the delicate nuts now!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapKarin = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapKarin += 100;
			if (!rn2(3)) FemaleTrapKarin += rnz(500);

		 break;

		 case JUEN_TRAP:

			if (FemaleTrapJuen) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Juen.");
				pline("Oh no, your shins are almost broken!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapJuen = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapJuen += 100;
			if (!rn2(3)) FemaleTrapJuen += rnz(500);

		 break;

		 case KRISTINA_TRAP:

			if (FemaleTrapKristina) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Kristina.");
				pline("Urgh, you can already smell the stench of stinking cigarettes!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapKristina = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapKristina += 100;
			if (!rn2(3)) FemaleTrapKristina += rnz(500);

		 break;

		 case LOU_TRAP:

			if (FemaleTrapLou) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Lou.");
				pline("These damn people want to use your precious clothing to brush off their dirty shoes!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapLou = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapLou += 100;
			if (!rn2(3)) FemaleTrapLou += rnz(500);

		 break;

		 case ALMUT_TRAP:

			if (FemaleTrapAlmut) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Almut.");
				pline("The girls feel like kicking your hands bloodily with their sneakers!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapAlmut = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapAlmut += 100;
			if (!rn2(3)) FemaleTrapAlmut += rnz(500);

		 break;

		 case JULIETTA_TRAP:

			if (FemaleTrapJulietta) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Julietta.");
				pline("Now you'll be subjected to Julietta's torture, and she'll subject you to random punishments from time to time.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapJulietta = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapJulietta += 100;
			if (!rn2(3)) FemaleTrapJulietta += rnz(500);

			if (!FemtrapActiveRuth) pline("Julietta rolls the dice to randomly select a punishment for you...");
			randomfeminismtrap(rnz( (level_difficulty() + 2) * rnd(50)));

		 break;

		 case ANASTASIA_TRAP:

			if (FemaleTrapAnastasia) break;
			if (!dontreveal && !FemtrapActiveRuth && !(in_rooms(u.ux, u.uy, INSIDEROOM)) ) seetrap(trap);

			if (!FemtrapActiveRuth && !(in_rooms(u.ux, u.uy, INSIDEROOM)) ) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Anastasia.");
				pline("Suddenly, you feel that you're going to step into a heap of shit.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapAnastasia = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapAnastasia += 100;
			if (!rn2(3)) FemaleTrapAnastasia += rnz(500);

		 break;

		 case JESSICA_TRAP:

			if (FemaleTrapJessica) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Jessica.");
				pline("Your butt cheeks suddenly feel very tender, and in fact, a similar thing is happening to your entire body!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapJessica = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapJessica += 100;
			if (!rn2(3)) FemaleTrapJessica += rnz(500);

		 break;

		 case SOLVEJG_TRAP:

			if (FemaleTrapSolvejg) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Solvejg.");
				pline("You suddenly have a very grating, aggravating voice, and you start to emit a beguiling odor! In fact, you're super sexy and sweet now!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapSolvejg = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapSolvejg += 100;
			if (!rn2(3)) FemaleTrapSolvejg += rnz(500);

		 break;

		 case WENDY_TRAP:

			if (FemaleTrapWendy) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Wendy.");
				pline("All the female denizens of the dungeon will show you their true power, and it will happen very soon!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapWendy = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapWendy += 100;
			if (!rn2(3)) FemaleTrapWendy += rnz(500);

		 break;

		 case KATHARINA_TRAP:

			if (FemaleTrapKatharina) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Katharina.");
				pline("You feel that the girls and women are getting ready to use their sexy butts as weapons.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapKatharina = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapKatharina += 100;
			if (!rn2(3)) FemaleTrapKatharina += rnz(500);

		 break;

		 case ELENA_TRAP:

			if (FemaleTrapElena) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Elena.");
				pline("You long for beautiful sexy women with tender butt cheeks and lovely high heels.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapElena = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapElena += 100;
			if (!rn2(3)) FemaleTrapElena += rnz(500);

		 break;

		 case THAI_TRAP:

			if (FemaleTrapThai) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Thai.");
				pline("You feel that you'll want to use the toilet more often. Also, somehow your physique seems weaker now...");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapThai = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapThai += 100;
			if (!rn2(3)) FemaleTrapThai += rnz(500);

		 break;

		 case ELIF_TRAP:

			if (FemaleTrapElif) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Elif.");
				pline("A ghostly girl (named Elif) starts following you around, and apparently she wants to play with you!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapElif = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapElif += 100;
			if (!rn2(3)) FemaleTrapElif += rnz(500);

		 break;

		 case NADJA_TRAP:

			if (FemaleTrapNadja) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Nadja.");
				pline("You feel that you angered the womanhood. If you now hit a woman, you will be hit with retribution!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapNadja = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapNadja += 100;
			if (!rn2(3)) FemaleTrapNadja += rnz(500);

		 break;

		 case SANDRA_TRAP:

			if (FemaleTrapSandra) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Sandra.");
				pline("You just know that your legs are going to be ripped open by very sharp-edged combat boot heels.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapSandra = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapSandra += 100;
			if (!rn2(3)) FemaleTrapSandra += rnz(500);

		 break;

		 case NATALJE_TRAP:

			if (FemaleTrapNatalje) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Natalje.");
				pline("You gotta dance! You've suddenly become super sexy and attractive, and neither sleep nor paralysis can stop you in your tracks. And you can kick your enemies to stomp their toes flat. But if you ever stand still for too long, a bunch of bloodthirsty female painted toenails is going to hurt your beautiful skin, so make sure you keep moving!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapNatalje = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapNatalje += 100;
			if (!rn2(3)) FemaleTrapNatalje += rnz(500);
			u.nataljetrapturns = moves;
			u.nataljetrapx = u.ux;
			u.nataljetrapy = u.uy;

		 break;

		 case JEANETTA_TRAP:

			if (FemaleTrapJeanetta) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Jeanetta.");
				pline("Lots of shreds of skin will be scraped off your shins, and the girls will enjoy it.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapJeanetta = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapJeanetta += 100;
			if (!rn2(3)) FemaleTrapJeanetta += rnz(500);

		 break;

		 case YVONNE_TRAP:

			if (FemaleTrapYvonne) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Yvonne.");
				pline("You feel that people are building toilets for you to use.");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapYvonne = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapYvonne += 100;
			if (!rn2(3)) FemaleTrapYvonne += rnz(500);

		 break;

		 case MAURAH_TRAP:

			if (FemaleTrapMaurah) break;
			if (!dontreveal && !FemtrapActiveRuth) seetrap(trap);

			if (!FemtrapActiveRuth) {
				pline("Whoops... you seem to have stumbled into a trap that was set by Maurah.");
				pline("Your sexy butt signals that it wants to produce beautiful farting noises!");
			}
			u.cnd_feminismtrapamount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (Role_if(PM_EMERA)) emerafrenzy();

			FemaleTrapMaurah = rnz(femmytrapdur * (monster_difficulty() + 1));
			if (rn2(3)) FemaleTrapMaurah += 100;
			if (!rn2(3)) FemaleTrapMaurah += rnz(500);

		 break;

		 case EVIL_HEEL_TRAP:

			pline("Uh-oh, should have watched your step...");
			deltrap(trap);
			bad_equipment_heel();

		 break;

		 case BAD_EQUIPMENT_TRAP:

			pline("Uh-oh, should have watched your step...");
			deltrap(trap);
			bad_equipment(0);

		 break;

		 case TEMPOCONFLICT_TRAP:

			seetrap(trap);

		    pline("You stepped on a trigger!");

			if(!(HConflict & INTRINSIC) && !(HConflict & TIMEOUT)) {
				pline("You start generating conflict!");
				incr_itimeout(&HConflict, rnz(20 * (monster_difficulty() + 1) ) );

			} else pline("But nothing happens...");

		 break;

		 case TEMPOHUNGER_TRAP:

			seetrap(trap);

		    pline("You stepped on a trigger!");

			if(!(HHunger & INTRINSIC) && !(HHunger & TIMEOUT)) {
				pline("You start hungering rapidly!");
				incr_itimeout(&HHunger, rnz(20 * (monster_difficulty() + 1) ) );

			} else pline("But nothing happens...");

		 break;

		 case TELEPORTITIS_TRAP:

			seetrap(trap);

		    pline("You stepped on a trigger!");

			if(!(HTeleportation & INTRINSIC) && !(HTeleportation & TIMEOUT)) {
				pline("You feel very jumpy!");
				incr_itimeout(&HTeleportation, rnz(20 * (monster_difficulty() + 1) ) );

			} else pline("But nothing happens...");

		 break;

		 case POLYMORPHITIS_TRAP:

			seetrap(trap);

		    pline("You stepped on a trigger!");

			if(!(HPolymorph & INTRINSIC) && !(HPolymorph & TIMEOUT)) {
				pline("You feel unstable!");
				incr_itimeout(&HPolymorph, rnz(20 * (monster_difficulty() + 1) ) );

			} else pline("But nothing happens...");


		 break;

		 case PREMATURE_DEATH_TRAP:

			seetrap(trap);
			pline("CLICK! You have triggered a trap!");
			pline("Suddenly, you fear that you might die soon...");
			incr_itimeout(&HPrem_death, rnz(100 * (monster_difficulty() + 1) ) );

		 break;

		 case LASTING_AMNESIA_TRAP:

			seetrap(trap);
			pline("You stepped on a trigger!");
			pline("Somehow, your memory is lost...");
			incr_itimeout(&HMap_amnesia, rnz(100 * (monster_difficulty() + 1) ) );

		 break;

		 case RAGNAROK_TRAP:

			deltrap(trap);
			pline("CLICK! You have triggered a trap!");
			ragnarok(FALSE);
			if (evilfriday) evilragnarok(FALSE,level_difficulty());

		 break;

		 case SINGLE_DISENCHANT_TRAP:

			pline("CLICK! You have triggered a trap!");
			deltrap(trap);

			{
				struct obj *otmpE;
			      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
					if (otmpE && !rn2(10)) (void) drain_item(otmpE);
				}
				Your("equipment seems less effective.");
				u.cnd_disenchantamount++;
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");

			}

		 break;

		case SEVERE_DISENCHANT_TRAP:

			pline("CLICK! You have triggered a trap!");
			deltrap(trap);

			{
				struct obj *otmpE;
			      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
					if (otmpE && !rn2(10)) (void) drain_item_severely(otmpE);
				}
				Your("equipment seems less effective.");
				u.cnd_disenchantamount++;
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");

			}

		 break;

		case PAIN_TRAP:

			seetrap(trap);
			pline("You stepped on a trigger!");
			You_feel("a painful sensation!");
			losehp(Upolyd ? ((u.mh / 10) + 1) : ((u.uhp / 10) + 1), "a pain trap", KILLED_BY);

		 break;

		case TECHCAP_TRAP:

			seetrap(trap);
			pline("You stepped on a trigger!");
			techcapincrease((level_difficulty() + 1) * rnd(50));

		 break;

		case TREMBLING_TRAP:

			seetrap(trap);
			pline("Uh-oh, should have watched your step...");
			pline("Your %s are trembling!", makeplural(body_part(HAND)));
			u.tremblingamount++;

		 break;

		case SPELL_MEMORY_TRAP:

			seetrap(trap);
			pline("You stepped on a trigger!");
			spellmemoryloss(level_difficulty() + 1);

		 break;

		case SKILL_REDUCTION_TRAP:

			seetrap(trap);
			pline("You stepped on a trigger!");
			skilltrainingdecrease(level_difficulty() + 1);

		 break;

		case SKILLCAP_TRAP:

			seetrap(trap);
			pline("CLICK! You have triggered a trap!");
			skillcaploss();

		 break;

		case PERMANENT_STAT_DAMAGE_TRAP:

			seetrap(trap);
			pline("CLICK! You have triggered a trap!");
			statdrain();

		 break;

		 case DIMNESS_TRAP:

			pline("A dim glow surrounds you...");
			seetrap(trap);
			make_dimmed(HDimmed + rnd(10) + rnd(monster_difficulty() + 1), TRUE);

		 break;

		 case EVIL_ARTIFACT_TRAP:

			pline("CLICK! You have triggered a trap!");
			bad_artifact();
			deltrap(trap);

		 break;

		 case RODNEY_TRAP:

			pline("CLICK! You have triggered a trap!");
			seetrap(trap);
			if (!u.uevent.udemigod) {
				u.uevent.udemigod = TRUE;
				u.udg_cnt = rn1(250, 50);
				pline("You can listen to Rodney's taunts inside your head...");
			} else pline("But nothing happens.");

		 break;

		case RECURSION_TRAP:
			pline("CLICK! You have triggered a trap!");
			deltrap(trap);
			recursioneffect();
		break;

		case TEMPORARY_RECURSION_TRAP:
			if (!u.temprecursion) {
			pline("CLICK! You have triggered a trap!");
			deltrap(trap);
			temprecursioneffect();
			}
		break;

		 case INTRINSIC_LOSS_TRAP:

			if (!rn2(5)) {
				You_hear("maniacal laughter!");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Kha-kha-kha-kha-kha-KDZH KDZH, tip bloka l'da smeyetsya yego tortsa, potomu chto vy teryayete vse vashi vstroyennyye funktsii!" : "Hoehoehoehoe!");
				attrcurse();
			}

			if (IntrinsicLossProblem) break;
			u.cnd_nastytrapamount++;

			IntrinsicLossProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case BLOOD_LOSS_TRAP:

			if (BloodLossProblem) break;
			u.cnd_nastytrapamount++;

			BloodLossProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case BAD_EFFECT_TRAP:

			if (!rn2(5)) badeffect();

			if (BadEffectProblem) break;
			u.cnd_nastytrapamount++;

			BadEffectProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case MULTIPLY_TRAP:


			{
				int rtrap;
			    int i, j, bd = 1;
				boolean canbeinawall = FALSE;
				if (!rn2(Passes_walls ? 5 : 25)) canbeinawall = TRUE;

			      for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
					if (!isok(u.ux + i, u.uy + j)) continue;
					if ((levl[u.ux + i][u.uy + j].typ <= DBWALL) && !canbeinawall) continue;
					if (t_at(u.ux + i, u.uy + j)) continue;
	
				      rtrap = rndtrap();
	
					(void) maketrap(u.ux + i, u.uy + j, rtrap, 100, FALSE);
				}
			}

			makerandomtrap(FALSE);

			if (TrapCreationProblem) break;
			u.cnd_nastytrapamount++;

			TrapCreationProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case AUTO_VULN_TRAP:

			if (!rn2(5)) {

				deacrandomintrinsic(rnz( (monster_difficulty() * 10) + 1));
			}

			if (AutomaticVulnerabilitiy) break;
			u.cnd_nastytrapamount++;

			AutomaticVulnerabilitiy = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case TELE_ITEMS_TRAP:

			if (TeleportingItems) break;
			u.cnd_nastytrapamount++;

			TeleportingItems = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case NASTINESS_TRAP:

			switch (rnd(275)) {

				case 1: RMBLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 2: NoDropProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 3: DSTWProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 4: StatusTrapProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); 
					if (HConfusion) set_itimeout(&HeavyConfusion, HConfusion);
					if (HStun) set_itimeout(&HeavyStunned, HStun);
					if (HNumbed) set_itimeout(&HeavyNumbed, HNumbed);
					if (HFeared) set_itimeout(&HeavyFeared, HFeared);
					if (HFrozen) set_itimeout(&HeavyFrozen, HFrozen);
					if (HBurned) set_itimeout(&HeavyBurned, HBurned);
					if (HDimmed) set_itimeout(&HeavyDimmed, HDimmed);
					if (Blinded) set_itimeout(&HeavyBlind, Blinded);
					if (HHallucination) set_itimeout(&HeavyHallu, HHallucination);
					break;
				case 5: Superscroller += rnz(nastytrapdur * (Role_if(PM_GRADUATE) ? 2 : Role_if(PM_GEEK) ? 5 : 10) * (monster_difficulty() + 1)); 
					(void) makemon(&mons[PM_SCROLLER_MASTER], 0, 0, NO_MINVENT);
					break;
				case 6: MenuBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 7: FreeHandLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 8: Unidentify += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 9: Thirst += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 10: LuckLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 11: ShadesOfGrey += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 12: FaintActive += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 13: Itemcursing += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 14: DifficultyIncreased += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 15: Deafness += rnz(nastytrapdur * (monster_difficulty() + 1)); flags.soundok = 0; break;
				case 16: CasterProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 17: WeaknessProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 18: RotThirteen += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 19: BishopGridbug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 20: UninformationProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 21: StairsProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 22: AlignmentProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 23: ConfusionProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 24: SpeedBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 25: DisplayLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 26: SpellLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 27: YellowSpells += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 28: AutoDestruct += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 29: MemoryLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 30: InventoryLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 31: {
	
					if (BlackNgWalls) break;
	
					BlackNgWalls = (blackngdur - (monster_difficulty() * 3));
					(void) makemon(&mons[PM_BLACKY], 0, 0, NO_MM_FLAGS);
					break;
				}
				case 32: IntrinsicLossProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 33: BloodLossProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 34: BadEffectProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 35: TrapCreationProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 36: AutomaticVulnerabilitiy += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 37: TeleportingItems += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 38: NastinessProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 39: CaptchaProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 40: FarlookProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 41: RespawnProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 42: RecurringAmnesia += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 43: BigscriptEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 44: {
					BankTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1));
					if (u.bankcashlimit == 0) u.bankcashlimit = rnz(1000 * (monster_difficulty() + 1 + (long)mvitals[PM_ARABELLA_THE_MONEY_THIEF].born));
					u.bankcashamount += u.ugold;
					u.ugold = 0;
	
					break;
				}
				case 45: MapTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 46: TechTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 47: RecurringDisenchant += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 48: verisiertEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 49: ChaosTerrain += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 50: Muteness += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 51: EngravingDoesntWork += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 52: MagicDeviceEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 53: BookTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 54: LevelTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 55: QuizTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 56: FastMetabolismEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 57: NoReturnEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 58: AlwaysEgotypeMonsters += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 59: TimeGoesByFaster += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 60: FoodIsAlwaysRotten += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 61: AllSkillsUnskilled += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 62: AllStatsAreLower += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 63: PlayerCannotTrainSkills += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 64: PlayerCannotExerciseStats += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 65: TurnLimitation += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 66: WeakSight += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 67: RandomMessages += rnz(nastytrapdur * (monster_difficulty() + 1)); break;

				case 68: Desecration += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 69: StarvationEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 70: NoDropsEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 71: LowEffects += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 72: InvisibleTrapsEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 73: GhostWorld += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 74: Dehydration += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 75: HateTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 76: TotterTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 77: Nonintrinsics += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 78: Dropcurses += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 79: Nakedness += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 80: Antileveling += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 81: ItemStealingEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 82: Rebellions += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 83: CrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 84: ProjectilesMisfire += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 85: WallTrapping += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 86: DisconnectedStairs += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 87: InterfaceScrewed += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 88: Bossfights += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 89: EntireLevelMode += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 90: BonesLevelChange += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 91: AutocursingEquipment += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 92: HighlevelStatus += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 93: SpellForgetting += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 94: SoundEffectBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 95: TimerunBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 96: LootcutBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 97: MonsterSpeedBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 98: ScalingBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 99: EnmityBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 100: WhiteSpells += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 101: CompleteGraySpells += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 102: QuasarVision += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 103: MommaBugEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 104: HorrorBugEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 105: ArtificerBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 106: WereformBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 107: NonprayerBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 108: EvilPatchEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 109: HardModeEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 110: SecretAttackBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 111: EaterBugEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 112: CovetousnessBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 113: NotSeenBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 114: DarkModeBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 115: AntisearchEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 116: HomicideEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 117: NastynationBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 118: WakeupCallBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 119: GrayoutBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 120: GrayCenterBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 121: CheckerboardBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 122: ClockwiseSpinBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 123: CounterclockwiseSpin += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 124: LagBugEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 125: BlesscurseEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 126: DeLightBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 127: DischargeBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 128: TrashingBugEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 129: FilteringBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 130: DeformattingBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 131: FlickerStripBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 132: UndressingEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 133: Hyperbluewalls += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 134: NoliteBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 135: ParanoiaBugEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 136: FleecescriptBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 137: InterruptEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 138: DustbinBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 139: ManaBatteryBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 140: Monsterfingers += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 141: MiscastBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 142: MessageSuppression += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 143: StuckAnnouncement += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 144: BloodthirstyEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 145: MaximumDamageBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 146: LatencyBugEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 147: StarlitBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 148: KnowledgeBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 149: HighscoreBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 150: PinkSpells += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 151: GreenSpells += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 152: EvencoreEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 153: UnderlayerBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 154: DamageMeterBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 155: ArbitraryWeightBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 156: FuckedInfoBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 157: BlackSpells += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 158: CyanSpells += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 159: HeapEffectBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 160: BlueSpells += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 161: TronEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 162: RedSpells += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 163: TooHeavyEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 164: ElongationBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 165: WrapoverEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 166: DestructionEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 167: MeleePrefixBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 168: AutomoreBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
				case 169: UnfairAttackBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 170: OrangeSpells += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 171: VioletSpells += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 172: LongingEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 173: CursedParts += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 174: Quaversal += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 175: AppearanceShuffling += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 176: BrownSpells += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 177: Choicelessness += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 178: Goldspells += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 179: Deprovement += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 180: InitializationFail += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 181: GushlushEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 182: SoiltypeEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 183: DangerousTerrains += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 184: FalloutEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 185: MojibakeEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 186: GravationEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 187: UncalledEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 188: ExplodingDiceEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 189: PermacurseEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 190: ShroudedIdentity += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 191: FeelerGauges += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 192: LongScrewup += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 193: WingYellowChange += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 194: LifeSavingBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 195: CurseuseEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 196: CutNutritionEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 197: SkillLossEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 198: AutopilotEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 199: MysteriousForceActive += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 200: MonsterGlyphChange += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 201: ChangingDirectives += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 202: ContainerKaboom += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 203: StealDegrading += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 204: LeftInventoryBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 205: FluctuatingSpeed += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 206: TarmuStrokingNora += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 207: FailureEffects += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 208: BrightCyanSpells += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 209: FrequentationSpawns += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 210: PetAIScrewed += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 211: SatanEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 212: RememberanceEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 213: PokelieEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 214: AlwaysAutopickup += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 215: DywypiProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 216: SilverSpells += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 217: MetalSpells += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 218: PlatinumSpells += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 219: ManlerEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 220: DoorningEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 221: NownsibleEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 222: ElmStreetEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 223: MonnoiseEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 224: RangCallEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 225: RecurringSpellLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 226: AntitrainingEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 227: TechoutBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 228: StatDecay += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 229: Movemork += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 230: BadPartBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 231: CompletelyBadPartBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 232: EvilVariantActive += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 233: SanityTrebleEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 234: StatDecreaseBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 235: SimeoutBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 236: GiantExplorerBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 237: YawmBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 238: TrapwarpingBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 239: EnthuEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 240: MikraEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 241: GotsTooGoodEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 242: NoFunWallsEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 243: CradleChaosEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 244: TezEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 245: KillerRoomEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 246: ReallyBadTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 247: CovidTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 248: ArtiblastEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 249: RepeatingNastycurseEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 250: RealLieEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 251: EscapePastEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 252: PethateEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 253: PetLashoutEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 254: PetstarveEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 255: PetscrewEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 256: TechLossEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 257: ProoflossEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 258: UnInvisEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 259: DetectationEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 260: OptionBugEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 261: MiscolorEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 262: OneRainbowEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 263: ColorshiftEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 264: TopLineEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 265: CapsBugEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 266: UnKnowledgeEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 267: DarkhanceEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 268: DschueueuetEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 269: NopeskillEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 270: FuckfuckfuckEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 271: EpviEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 272: AefdeEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 273: MeanBurdenEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 274: CarrcapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 275: UmengEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;

			}

			if (NastinessProblem) break;
			u.cnd_nastytrapamount++;

			NastinessProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case STAIRS_TRAP:

			if (StairsProblem) break;
			u.cnd_nastytrapamount++;

			StairsProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case ALIGNMENT_TRAP:

			if (AlignmentProblem) break;
			u.cnd_nastytrapamount++;

			AlignmentProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case CONFUSION_TRAP:

			if (ConfusionProblem) break;
			u.cnd_nastytrapamount++;

			ConfusionProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case GRAVITY_TRAP:

			seetrap(trap);

			pline("You stepped on a trigger!");

			if (IncreasedGravity) pline("Your load feels even heavier!");
			else pline("Your load feels heavier!");

			IncreasedGravity += rnz(5 * (monster_difficulty() + 1));

		 break;

		 case VULN_TRAP: /* temporarily disables player's intrinsics --Amy */

			seetrap(trap);
			pline("You stepped on a trigger!");
			deacrandomintrinsic(rnz( (monster_difficulty() * 10) + 1));

		 break;

		 case HEEL_TRAP:
		 case ATTACKING_HEEL_TRAP:
		{
			int projectiledamage = 1;
			int randomkick = rnd(15);
			switch (trap->launch_otyp) {
				case 1:
					projectiledamage = rnd(10)+ rnd(monster_difficulty() + 1);
					break;
				case 2:
					projectiledamage = 1;
					break;
				case 3:
					projectiledamage = 1;
					break;
				case 4:
					projectiledamage = rnd(10)+ rnd(monster_difficulty() + 1);
					break;
				case 5:
					projectiledamage = rnd(10)+ rnd(monster_difficulty() + 1);
					break;
				case 6:
					projectiledamage = rnd(10)+ rnd( (monster_difficulty() * 2) + 1);
					break;
				case 7:
					projectiledamage = rnd(10)+ rnd( (monster_difficulty() * 4) + 1);
					break;
				case 8:
					projectiledamage = rnd(12)+ rnd( monster_difficulty() + 1);
					break;
				case 9:
					projectiledamage = rnd(20)+ rnd( monster_difficulty() + 1);
					break;
				case 10:
					projectiledamage = rnd(10)+ rnd( monster_difficulty() + 1);
					break;
				case 11:
					projectiledamage = rnd(4)+ rnd( monster_difficulty() + 1);
					break;
				case 12:
					projectiledamage = rnd(10)+ rnd( monster_difficulty() + 1);
					break;
				case 13:
					projectiledamage = rnd(12)+ rnd( monster_difficulty() + 1);
					break;
				case 14:
					projectiledamage = rnd(15)+ rnd( monster_difficulty() + 1);
					break;
				case 15:
					projectiledamage = 1;
					break;
				case 16:
					projectiledamage = rnd(12) + rnd( monster_difficulty() + 1);
					break;
				case 17:
					projectiledamage = rnd(4) + rnd( monster_difficulty() + 1);
					break;
				case 18:
					projectiledamage = rnd(20)+ rnd( (monster_difficulty() * 5) + 1);
					break;
				case 19:
					projectiledamage = rnd(12) + rnd( (monster_difficulty() * 2) + 1);
					break;
				case 20:
					projectiledamage = rnd(6) + rnd( (monster_difficulty() * 2) + 1);
					break;
				case 21:
					projectiledamage = rnd(5) + rnd( monster_difficulty() + 1);
					break;
				case 22:
					projectiledamage = rnd(10) + rnd( monster_difficulty() + 1);
					break;
				case 23:
					projectiledamage = rnd(10) + rnd( monster_difficulty() + 1);
					if (randomkick == 4) projectiledamage  = rnd(15) + rnd( (monster_difficulty() * 3) + 1);
					break;
				case 24:
					projectiledamage = rnd(10) + rnd( monster_difficulty() + 1);
					break;
				case 25:
					projectiledamage = rnd(15) + rnd( (monster_difficulty() * 2) + 1);
					break;
				case 26:
					projectiledamage = rnd(20) + rnd( (monster_difficulty() * 3) + 1);
					break;
				case 27:
					projectiledamage = rnd(10) + rnd( monster_difficulty() + 1);
					break;
				case 28:
					projectiledamage = rnd(22) + rnd( (monster_difficulty() * 3) + 1);
					break;
			}

			if (projectiledamage > 1) {
				if (u.ulevel == 1) projectiledamage /= 2;
				else if (u.ulevel == 2) {
					projectiledamage *= 2;
					projectiledamage /= 3;
				} else if (u.ulevel == 3) {
					projectiledamage *= 3;
					projectiledamage /= 4;
				} else if (u.ulevel == 4) {
					projectiledamage *= 4;
					projectiledamage /= 5;
				}
			}

			seetrap(trap);

			pline("You stepped on a trigger!");

			switch (trap->launch_otyp) {

				case 1:

				pline("A wedged sandal suddenly slams against your shins! It hurts!");
				losehp(projectiledamage,"wedged sandal",KILLED_BY_AN);
				break;

				case 2:

				pline("A dancing shoe suddenly slams against your shins! You stagger...");
				make_stunned(HStun + rnd(10) + rnd(monster_difficulty() + 1), FALSE);
				losehp(1,"dancing shoe",KILLED_BY_AN);
				break;

				case 3:

				pline("A feminine mocassin suddenly slams against your %ss, opening your arteries and squirting %s everywhere!", body_part(HAND), body_part(BLOOD));
			      incr_itimeout(&Glib, rnd(10) + rnd(monster_difficulty() + 1));
				break;

				case 4:

				pline("A soft sneaker hits your %ss!", body_part(HAND) );
			      incr_itimeout(&Glib, 2);
				losehp(projectiledamage,"soft sneaker",KILLED_BY_AN);
				break;

				case 5:

				pline("A leather peep-toe slams against your shins!");
				losehp(projectiledamage,"leather peep-toe",KILLED_BY_AN);

				if (multi >= 0 && !rn2(2)) {
				    if (flags.female) {
					pline("It hurts a bit, but not too badly." );
				    } else if (Free_action) {
					pline("It hurts like hell, but you bear it like a man.");            
				    } else {
					int paralysistime = monster_difficulty() + 1;
					paralysistime /= 2;
					if (paralysistime <= 0) paralysistime = 1;
					if (!isstunfish && (paralysistime > 1)) paralysistime = rnd(paralysistime);
					if (paralysistime > 10) {
						while (rn2(5) && (paralysistime > 10)) {
							paralysistime--;
						}
					}

					pline("It hurts like hell! You pass out from the intense pain.");            
					nomovemsg = "You finally manage to get up again.";
					nomul(-(paralysistime), "knocked out by a peep-toe trap", TRUE);
					exercise(A_DEX, FALSE);
				    }
				}

				break;

				case 6:

				pline("A sexy leather pump suddenly scratches up and down your %ss!", body_part(LEG) );

				losehp(projectiledamage,"sexy leather pump",KILLED_BY_AN);

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

				losehp(u.legscratching, "bloody leg scratches", KILLED_BY);
				u.legscratching++;
				register long side = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
				  const char *sidestr = (side == RIGHT_SIDE) ? "right" : "left";
			    set_wounded_legs(side, HWounded_legs + rnd(60-ACURR(A_DEX)));
			    exercise(A_STR, FALSE);
			    exercise(A_DEX, FALSE);

				break;

				case 7:

				pline("A stiletto boot suddenly kicks one of your sensitive body parts!" );

				losehp(projectiledamage,"stiletto boot",KILLED_BY_AN);
				if (!rn2(250)) pushplayer(TRUE);

				break;

				case 8:

				pline("Your %s is suddenly hit painfully by a high-heeled sandal!", body_part(HEAD) );

				losehp(projectiledamage,"heeled sandal to the head",KILLED_BY_AN);

				break;

				case 9:

				pline("Klock! A sexy leather pump suddenly slams on your %s, producing a beautiful sound.", body_part(HEAD) );

				losehp(projectiledamage,"sexy leather pump to the head",KILLED_BY_AN);

				break;

				case 10:

				pline("Suddenly, a spiked battle boot squeezes and stings your skin!" );

				losehp(projectiledamage,"battle boot with spikes",KILLED_BY_AN);
				    if (!rn2(6))
					poisoned("spikes", A_STR, "poisoned battle boot", 8);

				break;

				case 11:

				pline("A wedged little-girl sandal painfully thunders on your %s!", body_part(HEAD) );

				losehp(projectiledamage,"wedged girl sandal to the head",KILLED_BY_AN);

				break;

				case 12:

				pline("A girl's plateau boot unyieldingly bonks your %s!", body_part(HEAD) );

				losehp(projectiledamage,"female plateau boot to the head",KILLED_BY_AN);

				break;

				case 13:

				pline("Your %s is hit hard by a thick hugging boot!", body_part(HEAD) );

				losehp(projectiledamage,"female winter boot to the head",KILLED_BY_AN);
				if (Upolyd) u.mhmax--; /* lose one hit point */
				else u.uhpmax--; /* lose one hit point */
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				if (u.mh > u.mhmax) u.mh = u.mhmax;

				break;

				case 14:

				pline("An unyielding wooden sandal painfully lands on your %s!", body_part(HEAD) );

				losehp(projectiledamage,"wooden geta to the head",KILLED_BY_AN);

					if (multi >= 0 && !rn2(2)) {
					    if (Free_action) {
						pline("You struggle to stay on your %s.", makeplural(body_part(FOOT)));
					    } else {
						pline("You're knocked out and helplessly drop to the floor.");
						nomovemsg = 0;	/* default: "you can move again" */
						if (isstunfish) nomul(-rnz(5), "knocked out by a geta trap", TRUE);
						else nomul(-rnd(5), "knocked out by a geta trap", TRUE);
						exercise(A_DEX, FALSE);
						    }
						}

				break;

				case 15:

					if (Role_if(PM_COURIER)) pline("A lacquered dancing shoe appears and harmlessly scratches you.");
					else {pline("A lacquered dancing shoe appears and scratches your %s!", body_part(HEAD));

						if (!uarmh || uarmh->otyp != DUNCE_CAP) {

					    /* No such thing as mindless players... */
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
						    lifesaved++;
						    if (wizard) {
							    u.youaredead = 0;
							    u.youarereallydead = 0;
						    }
						}
					    }
					}
					/* adjattrib gives dunce cap message when appropriate */
					if (!rn2(10)) (void) adjattrib(A_INT, -rnd(2), FALSE, TRUE);
					else if (!rn2(2)) (void) adjattrib(A_INT, -1, FALSE, TRUE);
					if (!rn2(issoviet ? 2 : 3)) forget_levels(rnd(issoviet ? 25 : 10), FALSE);	/* lose memory of 25% of levels */
					if (!rn2(issoviet ? 3 : 5)) forget_objects(rnd(issoviet ? 25 : 10), FALSE);	/* lose memory of 25% of objects */
					exercise(A_WIS, FALSE);

						}

				break;

				case 16:
					pline("A block-heeled combat boot hits your %s with the massive heel!", body_part(HEAD));
					losehp(projectiledamage,"a massive block heel to the head",KILLED_BY);
					pline("You're seeing little asterisks everywhere.");
					make_confused(HConfusion + rnd(10) + rnd( monster_difficulty() + 1), FALSE);

				break;

				case 17:
					pline("A very erotic inka boot hits you, and you have an orgasm.");
					if (practicantterror) {
						pline("%s rings out: 'Wanking off is not permitted in my laboratory, but you know that. 200 zorkmids.'", noroelaname());
						fineforpracticant(200, 0, 0);
					}
					badeffect();
					losehp(projectiledamage,"inka boot to the head",KILLED_BY_AN);

				break;

				case 18:
					pline("AAAAAHHHHH! Your %s screams as the steel-capped sandal hits with the massive, unyielding metal heel!", body_part(HEAD));
					make_stunned(HStun + rnd(100) + rnd( (monster_difficulty() * 10) + 1), FALSE);
						if (!rn2(10)) {
							pline("You're knocked out and helplessly drop to the floor.");
							nomovemsg = 0;	/* default: "you can move again" */

							if (isstunfish) {
								if (StrongFree_action) nomul(-rnz(4), "knocked out by a steel-capped sandal", TRUE);
								else if (Free_action) nomul(-rnz(8), "knocked out by a steel-capped sandal", TRUE);
								else nomul(-rnz(20), "knocked out by a steel-capped sandal", TRUE);

							} else {
								if (StrongFree_action) nomul(-rnd(2), "knocked out by a steel-capped sandal", TRUE);
								else if (Free_action) nomul(-rnd(4), "knocked out by a steel-capped sandal", TRUE);
								else nomul(-rnd(8), "knocked out by a steel-capped sandal", TRUE);

							}
						}
					losehp(projectiledamage,"steel-capped sandal to the head",KILLED_BY_AN);

				break;

				case 19:
					pline("Your %s is bashed by a very fleecy block-heeled sandal!", body_part(HEAD));
					make_dimmed(HDimmed + rnd(50) + rnd( (monster_difficulty() * 2) + 1), TRUE);
					losehp(projectiledamage,"a pretty block-heeled sandal to the head",KILLED_BY);

				break;

				case 20:
					pline("Ouch! A soft lady shoe hits you, and despite being incredibly soft, hits your optical nerve!");
					make_blinded(Blinded + rnd(30) + rnd( (monster_difficulty() * 3) + 1),FALSE);
					losehp(projectiledamage,"a soft lady shoe to the head",KILLED_BY);

				break;

				case 21:
					pline("Ulch - a block-heeled boot hits your body, and you find out that the soles are immersed with dog shit!");

					    register struct obj *objX, *objX2;
					    for (objX = invent; objX; objX = objX2) {
					      objX2 = objX->nobj;
						if (!rn2(5)) rust_dmg(objX, xname(objX), 3, TRUE, &youmonst);
						if (objX && !rn2(100)) wither_dmg(objX, xname(objX), 3, TRUE, &youmonst);

					    }
					losehp(projectiledamage,"a dogshit boot trap",KILLED_BY);

				break;

				case 22:
					pline("Wow, a beautiful prostitute shoe slams on your %s with the massive heel! It's irresistible!", body_part(HEAD));
					losehp(projectiledamage,"a sexy prostitute shoe to the head",KILLED_BY);
					badeffect();

				break;

				case 23:
				{

					if (randomkick == 1) {
						pline("A hippie heel steps on your %s with the plateau heels!", body_part(HAND));
						incr_itimeout(&Glib, 20 + monster_difficulty()); /* painfully jamming your fingers */
						losehp(projectiledamage,"being jammed by a plateau heel",KILLED_BY);
					}
					if (randomkick == 2) {
						pline("A red whore boot jams your %ss!", body_part(TOE));

						register long side = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
						const char *sidestr = (side == RIGHT_SIDE) ? "right" : "left";
				    		set_wounded_legs(side, HWounded_legs + rnd(60-ACURR(A_DEX)));
				    		exercise(A_STR, FALSE);
				    		exercise(A_DEX, FALSE);
						losehp(projectiledamage,"being jammed by a red whore boot",KILLED_BY);
					}

					if (randomkick == 3) {
						pline("A prostitute shoe kicks right into your nuts!");
						losehp(projectiledamage,"being kicked in the nuts by a prostitute shoe",KILLED_BY);
						if (!flags.female) losehp(projectiledamage,"being kicked in the nuts by a prostitute shoe",KILLED_BY);
						else pline("But you don't actually have nuts. How the hell could this attack even affect you?");

					}

					if (randomkick == 4) {
						pline("An erotic lady boot painfully stomps your body!");
						u.uhp -= 1;
						u.uhpmax -= 1;
						u.uen -= 1;
						u.uenmax -= 1;
						losehp(projectiledamage,"being stomped by erotic lady boots",KILLED_BY);
					}

					if (randomkick == 5) {
						pline("An incredibly erotic female shoe kicks you and looks so lovely that you fall in love with her, and are unable to fight back.");
						nomovemsg = "You finally decide to stop admiring the sexy leather boots.";
						if (isstunfish) nomul(-rnz(5), "mesmerized by a pair of sexy leather boots", TRUE);
						else nomul(-rnd(5), "mesmerized by a pair of sexy leather boots", TRUE);
						losehp(projectiledamage,"being stomped by erotic female shoes",KILLED_BY);

					}

					if (randomkick == 6) {
						pline("You're being kicked by an incredibly high-heeled platform boot. Think of the sweet red leather your sputa will flow down.");
						morehungry(1000);
					      make_vomiting(Vomiting+20, TRUE);
						if (Sick && Sick < 100)
						 	set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
						losehp(projectiledamage,"being kicked by incredibly high-heeled platform boots",KILLED_BY);
					}

					if (randomkick == 7) {
						pline("You decide to close your %ss for a while, thinking about the sexy red overknees and their lovely block heels while they kick you very painfully.", body_part(EYE));
						make_blinded(Blinded + (monster_difficulty() * 5), FALSE);
						losehp(projectiledamage,"being kicked by sexy red overknees",KILLED_BY);
					}

					if (randomkick == 8) {
						pline("Wow... those wonderful high heels are soooooo mesmerizing and beautiful while they kick you...");
						make_hallucinated(HHallucination + (monster_difficulty() * 5),FALSE,0L);
						losehp(projectiledamage,"being kicked by wonderful high heels",KILLED_BY);
					}

					if (randomkick == 9) {
						pline("You wonder where the plateau heels come from. As they kick you, your %s spins in bewilderment.", body_part(HEAD));
						make_confused(HConfusion + (monster_difficulty() * 5), FALSE);
						losehp(projectiledamage,"being kicked by plateau heels",KILLED_BY);
					}

					if (randomkick == 10) {
						pline("Argh! You got hit by a fleecy red combat boot, but the massive heel strained a vital muscle!");
						make_stunned(HStun + (monster_difficulty() * 5),FALSE);
						losehp(projectiledamage,"being kicked by massive heels",KILLED_BY);
					}

					if (randomkick == 11) {
						if (!rn2(25)) {
							pline("Fuck! You were hit by a high heel, which was contaminated with spores!");
							make_sick(rn1(25,25), "contaminated high heel", TRUE, SICK_VOMITABLE);
							losehp(projectiledamage,"being kicked by unclean high heels",KILLED_BY);
						}
						else {
							pline("You get kicked by female boots, which some devious woman rubbed with venom! How unfair!");
							poisoned("block heel", A_STR, "poisoned block heel", 8);
							losehp(projectiledamage,"being kicked by venomous high heels",KILLED_BY);
						}
					}

					if (randomkick == 12) {
						pline("Ow, the slutty boots are stomping you repeatedly!");
						make_numbed(HNumbed + (monster_difficulty() * 5), FALSE);
						losehp(projectiledamage,"being stomped by slutty boots",KILLED_BY);
					}

					if (randomkick == 13) {
						pline("You are getting the creeps as an incredibly high heel kicks you.");
						make_frozen(HFrozen + (monster_difficulty() * 5),FALSE);
						losehp(projectiledamage,"being kicked by incredibly high heels",KILLED_BY);
						}

					if (randomkick == 14) {
						pline("A block-heeled lady boot kicks you, and burns your skin!");
						make_burned(HBurned + (monster_difficulty() * 5),FALSE);
						losehp(projectiledamage,"being kicked by block-heeled lady boots",KILLED_BY);
					}

					if (randomkick == 15) {
						pline("You shudder in fear as a violent high-heeled plateau boot performs lethal kick attacks on you.");
						make_feared(HFeared + (monster_difficulty() * 5),FALSE);
						losehp(projectiledamage,"being kicked by a violent high-heeled plateau boot",KILLED_BY);
					}

				}
				break;

				case 24:
					pline("A fleecy lady boot suddenly scratches over your %s with the lovely block heel!", body_part(LEG));
					playerbleed(monster_difficulty());
					losehp(projectiledamage,"a sharp-edged lady boot",KILLED_BY);

				break;

				case 25:
					pline("A razor-sharp stiletto sandal scratches very %sy wounds on your %s!", body_part(BLOOD), body_part(LEG));
					playerbleed(monster_difficulty());
					losehp(projectiledamage,"a sharp-edged stiletto sandal",KILLED_BY);

				break;

				case 26:
					pline("An italian high heel scratches your %s very painfully!!", body_part(LEG));
					playerbleed(monster_difficulty());
					badeffect();
					losehp(projectiledamage,"a sharp-edged italian heel",KILLED_BY);

				break;

				case 27:
					pline("Suddenly, a very sexy mary jane slams on your %s with the beautiful, massive block heel!", body_part(HEAD));
					losehp(projectiledamage,"a very sexy mary jane",KILLED_BY);

					if (multi >= 0) {
						pline("Mmmmmmm, the massive block heel can cause such beautiful pain! You decide to just stand there and enjoy the hard impacts.");

						if (isstunfish) nomul(-rnz(3), "swooning over a sexy mary jane", TRUE);
						else nomul(-rnd(3), "swooning over a sexy mary jane", TRUE);

					}

				break;

				case 28:

					if (Role_if(PM_COURIER)) pline("A cute kitten heel shoe appears and harmlessly scratches you.");
					else {
						pline("Arrgh! Auugh! Oww! A cute kitten heel pump bored her incredibly lovely, pointy heel into your %s!", body_part(HEAD));
						drain_alla(10);
						increasesanity(rnz(50));
						losehp(projectiledamage,"a cuuuuute kitten-heel pump",KILLED_BY);
					}

				break;

				default:
					impossible("heel trap type %d used", trap->launch_otyp);
				break;

			}
		}
		 break;

		 case AUTOMATIC_SWITCHER:

			if (RMBLoss || Superscroller || DisplayLoss || SpellLoss || YellowSpells || AutoDestruct || MemoryLoss || InventoryLoss || BlackNgWalls || MenuBug || SpeedBug || FreeHandLoss || Unidentify || Thirst || LuckLoss || ShadesOfGrey || FaintActive || Itemcursing || DifficultyIncreased || Deafness || CasterProblem || WeaknessProblem || NoDropProblem || RotThirteen || BishopGridbug || ConfusionProblem || DSTWProblem || StatusTrapProblem || AlignmentProblem || StairsProblem || UninformationProblem || TimerunBug || MeanBurdenEffect || CarrcapEffect || UmengEffect || EpviEffect || AefdeEffect || FuckfuckfuckEffect || RepeatingNastycurseEffect || BadPartBug || CompletelyBadPartBug || EvilVariantActive || IntrinsicLossProblem || BloodLossProblem || BadEffectProblem || TrapCreationProblem ||AutomaticVulnerabilitiy || TeleportingItems || NastinessProblem || CaptchaProblem || RespawnProblem || FarlookProblem || RecurringAmnesia || BigscriptEffect || BankTrapEffect || MapTrapEffect || TechTrapEffect || RecurringDisenchant || verisiertEffect || ChaosTerrain || Muteness || EngravingDoesntWork || MagicDeviceEffect || BookTrapEffect || LevelTrapEffect || QuizTrapEffect || FastMetabolismEffect || NoReturnEffect || AlwaysEgotypeMonsters || TimeGoesByFaster ||  FoodIsAlwaysRotten || AllSkillsUnskilled || AllStatsAreLower || PlayerCannotTrainSkills || PlayerCannotExerciseStats || TurnLimitation || WeakSight || RandomMessages || Desecration || StarvationEffect || NoDropsEffect || LowEffects || InvisibleTrapsEffect || GhostWorld || Dehydration || HateTrapEffect || TotterTrapEffect || Nonintrinsics || Dropcurses || Nakedness || Antileveling || ItemStealingEffect || Rebellions || CrapEffect || ProjectilesMisfire || WallTrapping || DisconnectedStairs || InterfaceScrewed || Bossfights || EntireLevelMode || BonesLevelChange || AutocursingEquipment || HighlevelStatus || SpellForgetting || SoundEffectBug || LootcutBug || MonsterSpeedBug || ScalingBug || EnmityBug || WhiteSpells || CompleteGraySpells || QuasarVision || MommaBugEffect || HorrorBugEffect || ArtificerBug || WereformBug || NonprayerBug || EvilPatchEffect || HardModeEffect || SecretAttackBug || EaterBugEffect || CovetousnessBug || NotSeenBug || DarkModeBug || AntisearchEffect || HomicideEffect || NastynationBug || WakeupCallBug || GrayoutBug || GrayCenterBug || CheckerboardBug || ClockwiseSpinBug || CounterclockwiseSpin || LagBugEffect || BlesscurseEffect || DeLightBug || DischargeBug || TrashingBugEffect || FilteringBug || DeformattingBug || FlickerStripBug || UndressingEffect || Hyperbluewalls || NoliteBug || ParanoiaBugEffect || FleecescriptBug || InterruptEffect || DustbinBug || ManaBatteryBug || Monsterfingers || MiscastBug || MessageSuppression || StuckAnnouncement || BloodthirstyEffect || MaximumDamageBug || LatencyBugEffect || StarlitBug || KnowledgeBug || HighscoreBug || PinkSpells || GreenSpells || EvencoreEffect || UnderlayerBug || DamageMeterBug || ArbitraryWeightBug || FuckedInfoBug || BlackSpells || CyanSpells || HeapEffectBug || BlueSpells || TronEffect || RedSpells || TooHeavyEffect || ElongationBug || WrapoverEffect || DestructionEffect || MeleePrefixBug || AutomoreBug || UnfairAttackBug || OrangeSpells || VioletSpells || LongingEffect || CursedParts || Quaversal || AppearanceShuffling || BrownSpells || Choicelessness || Goldspells || Deprovement || InitializationFail || GushlushEffect || SoiltypeEffect || DangerousTerrains || FalloutEffect || MojibakeEffect || GravationEffect || UncalledEffect || ExplodingDiceEffect || PermacurseEffect || ShroudedIdentity || FeelerGauges || LongScrewup || WingYellowChange || LifeSavingBug || CurseuseEffect || CutNutritionEffect || SkillLossEffect || AutopilotEffect || MysteriousForceActive || MonsterGlyphChange || ChangingDirectives || ContainerKaboom || StealDegrading || LeftInventoryBug || FluctuatingSpeed || TarmuStrokingNora || FailureEffects || BrightCyanSpells || FrequentationSpawns || PetAIScrewed || SatanEffect || RememberanceEffect || PokelieEffect || AlwaysAutopickup || DywypiProblem || SilverSpells || MetalSpells || PlatinumSpells || ManlerEffect || DoorningEffect || NownsibleEffect || ElmStreetEffect || MonnoiseEffect || RangCallEffect || RecurringSpellLoss || AntitrainingEffect || TechoutBug || StatDecay || Movemork || SanityTrebleEffect || StatDecreaseBug || SimeoutBug || GiantExplorerBug || YawmBug || TrapwarpingBug || EnthuEffect || MikraEffect || GotsTooGoodEffect || NoFunWallsEffect || CradleChaosEffect || TezEffect || KillerRoomEffect || ReallyBadTrapEffect || CovidTrapEffect || ArtiblastEffect || RealLieEffect || EscapePastEffect || PethateEffect || PetLashoutEffect || PetstarveEffect || PetscrewEffect || TechLossEffect || ProoflossEffect || UnInvisEffect || DetectationEffect || OptionBugEffect || MiscolorEffect || OneRainbowEffect || ColorshiftEffect || TopLineEffect || CapsBugEffect || UnKnowledgeEffect || DarkhanceEffect || DschueueuetEffect || NopeskillEffect ) {

			cure_nasty_traps();

			deltrap(trap); /* used up if anything was cured */

			}

		 break;

		 case SPEED_TRAP:

			if (SpeedBug) break;
			u.cnd_nastytrapamount++;

			SpeedBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case SWARM_TRAP:

			deltrap(trap); /* triggers only once */

			    coord cc;

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

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

				(void) makemon(randmonstforspawn, 0, 0, NO_MM_FLAGS);
			}

			}

			else if (!rn2(2)) {

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			monstercolor = rnd(15);

			if (wizard || !rn2(10)) You_feel("a colorful sensation!");

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(colormon(monstercolor), 0, 0, NO_MM_FLAGS);
			}

			}

			else {

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			monstercolor = rnd(434);

			if (wizard || !rn2(10)) You_feel("that a group has arrived!");

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(monstercolor), 0, 0, NO_MM_FLAGS);
			}

			}

			u.aggravation = 0;

		 break;

		 case CYANIDE_TRAP:
		{
			int projectiledamage = rnd(30) + rnd((monster_difficulty() * 5) + 1);
			if (projectiledamage > 1) {
				if (u.ulevel == 1) {
					projectiledamage *= 3;
					projectiledamage /= 10;
				} else if (u.ulevel == 2) {
					projectiledamage /= 2;
				} else if (u.ulevel == 3) {
					projectiledamage *= 2;
					projectiledamage /= 3;
				} else if (u.ulevel == 4) {
					projectiledamage *= 3;
					projectiledamage /= 4;
				}
			}

			pline("CLICK! You have triggered a trap!");
			seetrap(trap);
			pline("Cyanide gas blows in your %s!", body_part(FACE) ); /* unbreathing doesn't help --Amy */
			if (Upolyd) {
				if (u.mh > 1) u.mh /= 2;
				else losehp(projectiledamage, "cyanide trap", KILLED_BY_AN);
			} else {
				if (u.uhp > 1) u.uhp /= 2;
				else losehp(projectiledamage, "cyanide trap", KILLED_BY_AN);
			}
			You_feel("very poisoned...");
		      poisoned("gas", rn2(A_MAX), "cyanide trap", 5);
		}
		 break;

		 case PHOSGENE_TRAP:
			pline("CLICK! You have triggered a trap!");
			seetrap(trap);
			pline("Phosgene gas blows in your %s!", body_part(FACE) ); /* unbreathing doesn't help --Amy */

			You_feel("very poisoned...");
		      poisoned("gas", rn2(A_MAX), "phosgene trap", 5);
		      poisoned("gas", rn2(A_MAX), "phosgene trap", 5);
			playerbleed(rnd(monster_difficulty() * 2));
		 break;

		 case FALLING_ROCK_COLD:

		if (trap->once && !rn2(15)) {
		    pline("A trap door in %s opens, but nothing falls out!",
			  the(ceiling(u.ux,u.uy)));
		    deltrap(trap);
		    newsym(u.ux,u.uy);
		} else {
		    int dmg = d(2,6) + rnd((monster_difficulty() * 2 / 3) + 1); /* should be std ROCK dmg? */
		    if (!Cold_resistance || (StrongCold_resistance ? !rn2(20) : !rn2(4)) ) dmg += (d(2, 4) + rnd((monster_difficulty() / 3) + 1));
			if (dmg > 1) {
				if (u.ulevel == 1) dmg /= 2;
				else if (u.ulevel == 2) {
					dmg *= 2;
					dmg /= 3;
				} else if (u.ulevel == 3) {
					dmg *= 3;
					dmg /= 4;
				} else if (u.ulevel == 4) {
					dmg *= 4;
					dmg /= 5;
				}
			}

		    trap->once = 1;
		    seetrap(trap);

			if (!rn2(10)) make_frozen(HFrozen + (dmg * 5), TRUE); /* randomly freeze the player --Amy */
			if (isevilvariant || !rn2(issoviet ? 6 : Race_if(PM_GAVIL) ? 6 : Race_if(PM_HYPOTHERMIC) ? 6 : 33))
				destroy_item(POTION_CLASS, AD_COLD);
			pline("You are hit by cold!");

		    otmp = mksobj_at(ROCK, u.ux, u.uy, TRUE, FALSE, FALSE);
		    if (otmp) {
			    otmp->quan = 1L;
			    otmp->owt = weight(otmp);

			    pline("A trap door in %s opens and %s falls on your %s!",
				  the(ceiling(u.ux,u.uy)),
				  an(xname(otmp)),
				  body_part(HEAD));

			    if (uarmh) {
				if(is_hardmaterial(uarmh)) {
				    pline("Fortunately, you are wearing a hard helmet.");
				    dmg /= 4;
				} else if (flags.verbose) {
				    Your("%s does not protect you.", xname(uarmh));
				}
			    }

			    if (!Blind) otmp->dknown = 1;
			    stackobj(otmp);
			    newsym(u.ux,u.uy);	/* map the rock */

			    losehp(dmg, "falling rock", KILLED_BY_AN);
			    exercise(A_STR, FALSE);
		    }
		}

			break;
		 case RETURN_TRAP:

			if (u.returntimer) break; /* trigger only if not already returning */

			pline("You stepped on a trigger!");

			deltrap(trap); /* triggers only once */
			setupreturn(1); /* random destination */

			break;
		 case INTRINSIC_STEAL_TRAP:

			seetrap(trap);
			if (!rn2(20)) deltrap(trap);

			pline("You stepped on a trigger!");
			attrcurse();

			break;
		 case SCORE_AXE_TRAP:

			pline("Uh-oh, should have watched your step...");

			seetrap(trap);

			if (u.urexp > 1) {
				u.urexp /= 2;
				Your("score has been cut to half of its previous value!");
			} else pline("But nothing happened.");

			break;
		 case SCORE_DRAIN_TRAP:

			pline("You stepped on a trigger!");

			seetrap(trap);

			u.urexp -= ((level_difficulty() + 1) * 50);
			if (u.urexp < 0) u.urexp = 0;
			Your("score is drained!");

			break;
		 case SINGLE_UNIDENTIFY_TRAP:

			pline("You stepped on a trigger!");
			deltrap(trap); /* triggers only once */

			if (invent) {
			    for (otmpi = invent; otmpi; otmpi = otmpii) {
			      otmpii = otmpi->nobj;
	
				if (!rn2(8)) {
					otmpi->bknown = FALSE;
					u.cnd_unidentifycount++;
				}
				if (!rn2(8)) {
					otmpi->dknown = FALSE;
					u.cnd_unidentifycount++;
				}
				if (!rn2(8)) {
					otmpi->rknown = FALSE;
					u.cnd_unidentifycount++;
				}
				if (!rn2(8)) {
					otmpi->known = FALSE;
					u.cnd_unidentifycount++;
				}
			    }
				Your("%s unidentified itself!", FunnyHallu ? "shit" : "equipment");
			}

			break;
		 case UNLUCKY_TRAP:

			pline("CLICK! You have triggered a trap!");

			seetrap(trap);

			change_luck(-1);
			You_feel("unlucky.");

			break;
		 case ALIGNMENT_REDUCTION_TRAP:

			pline("You stepped on a trigger!");

			seetrap(trap);
			if (!rn2(50)) deltrap(trap);

			adjalign(-rnd(25));
			You("lose alignment points!");

			break;
		 case MALIGNANT_TRAP:

			pline("CLICK! You have triggered a trap!");

			seetrap(trap);
			if (!rn2(10)) deltrap(trap);

			You_feel("as if you need some help.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
			rndcurse();

			break;
		 case STAT_DAMAGE_TRAP:

			pline("You stepped on a trigger!");

			seetrap(trap);

			(void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);

			break;
		 case HALF_MEMORY_TRAP:

			pline("CLICK! You have triggered a trap!");

			seetrap(trap);

			spellmemoryhalve();

			break;
		 case HALF_TRAINING_TRAP:

			pline("CLICK! You have triggered a trap!");

			seetrap(trap);

			if (isdemagogue) {
				drain_alla(rnd(20));
				demagogueparole();
				aggravate();
			}
			else skilltraininghalve();

			break;
		 case DEBUFF_TRAP:

			pline("You stepped on a trigger!");

			seetrap(trap);

			{
				int debuffamount = rnd(6);
				while (debuffamount > 0) {
					statdebuff();
					debuffamount--;
				}
			}

			break;
		 case TRIP_ONCE_TRAP:

			pline("Uh-oh, should have watched your step...");

			deltrap(trap); /* triggers only once */

			HFumbling = FROMOUTSIDE | rnd(5);
			incr_itimeout(&HFumbling, 5);
			You("are about to trip.");

			break;
		 case NARCOLEPSY_TRAP:

			pline("Uh-oh, should have watched your step...");

			seetrap(trap);

			HSleeping = FROMOUTSIDE | rnd(5);
			incr_itimeout(&HSleeping, rnd(20));
			u.restfulsleepduration += rnz(10 * (monster_difficulty() + 1) );

			if (!rn2(3)) u.uprops[DEAC_SLEEP_RES].intrinsic += rn1(500,500);

			You_feel("narcoleptic...");

			break;
		 case MARTIAL_ARTS_TRAP:

			pline("CLICK! You have triggered a trap!");

			deltrap(trap); /* triggers only once */

			randommartialstyle();

			break;

		 case NEST_TRAP:

			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			    coord dd;
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
			monstercolor = rnd(434);

			if (wizard || !rn2(10)) You_feel("that a group has arrived!");

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(monstercolor), cx, cy, MM_ADJACENTOK);
			}

			}

			u.aggravation = 0;

		 break;

		 case FILLER_TRAP:

			deltrap(trap); /* triggers only once */
		 	pline("You stepped on a trigger!");

			{

				register int fillertraptype = rndtrap();
				register int filleramount = 4 + rnd(21);
				if (!rn2(5)) filleramount += rnd(25);
				if (!rn2(25)) filleramount += rnz(100);
				if (!rn2(2)) filleramount *= 2;
				int filx, fily;

				while (filleramount > 0) {
					filleramount--;
					boolean canbeinawall = FALSE;
					if (!rn2(Passes_walls ? 5 : 25)) canbeinawall = TRUE;

					filx = rn1(COLNO-3,2);
					fily = rn2(ROWNO);

					if (filx && fily && isok(filx, fily) && ((levl[filx][fily].typ > DBWALL) || canbeinawall) && !(t_at(filx, fily)) ) {
						(void) maketrap(filx, fily, fillertraptype, 0, FALSE);
					}
				}

			}

			pline("The floor around you vibrates in a strange way.");

			break;

		 case INSANITY_TRAP:

			deltrap(trap); /* triggers only once */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			coord ee;
			int ex,ey;

		      ex = rn2(COLNO);
		      ey = rn2(ROWNO);

			if (!rn2(4)) {

#ifdef BIGSLEX
			randsp = 5000;
#else
			randsp = 1680;
#endif
			randmnst = (rn2(187) + 1);
			randmnsx = (rn2(100) + 1);

			pline("CLICK! High tension! Fight for your life!!!");


			for (i = 0; i < randsp; i++) {
			/* This function will fill the map with a random amount of monsters of one class. --Amy */

				if (!enexto(&ee, u.ux, u.uy, (struct permonst *)0) ) continue;

#ifdef BIGSLEX
				if (i == 500) pline("This may take a while. Please be patient while the game selects monsters...");
				if (i == 1000) pline("Stand by while the monsters get generated...");
				if (i == 1500) pline("Spawning monsters...");
				if (i == 2000) pline("Please have some patience while monsters are selected...");
				if (i == 2500) pline("Halfway done...");
				if (i == 3000) pline("Don't worry, the game does not hang. Give it a little more time...");
				if (i == 3500) pline("Soon you can start fighting the hordes...");
				if (i == 4000) pline("It's taking a while. Sorry. That's because the level is very big...");
				if (i == 4500) pline("Almost finished...");
				if (i == 5000) pline("Done! The entire level is filled with monsters now! :-)");
#else
				if (i == 100) pline("This may take a while. Please be patient while the game selects monsters...");
				if (i == 200) pline("Stand by while the monsters get generated...");
				if (i == 400) pline("Spawning monsters...");
				if (i == 600) pline("Please have some patience while monsters are selected...");
				if (i == 800) pline("Halfway done...");
				if (i == 1000) pline("Don't worry, the game does not hang. Give it a little more time...");
				if (i == 1200) pline("Soon you can start fighting the hordes...");
				if (i == 1400) pline("Almost finished...");
				if (i == 1680) pline("Done! The entire level is filled with monsters now! :-)");
#endif

				if (!rn2(20)) reset_rndmonst(NON_PM);

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

			else if (!rn2(3)) {

#ifdef BIGSLEX
			randsp = 5000;
#else
			randsp = 1680;
#endif
			randmonstforspawn = rndmonst();

			pline("CLICK! Monsters! So many monsters! Good luck surviving the seething horde!");

			for (i = 0; i < randsp; i++) {

				if (!enexto(&ee, u.ux, u.uy, (struct permonst *)0) ) continue;

#ifdef BIGSLEX
				if (i == 500) pline("This may take a while. Please be patient while the game selects monsters...");
				if (i == 1000) pline("Stand by while the monsters get generated...");
				if (i == 1500) pline("Spawning monsters...");
				if (i == 2000) pline("Please have some patience while monsters are selected...");
				if (i == 2500) pline("Halfway done...");
				if (i == 3000) pline("Don't worry, the game does not hang. Give it a little more time...");
				if (i == 3500) pline("Soon you can start fighting the hordes...");
				if (i == 4000) pline("It's taking a while. Sorry. That's because the level is very big...");
				if (i == 4500) pline("Almost finished...");
				if (i == 5000) pline("Done! The entire level is filled with monsters now! :-)");
#else
				if (i == 100) pline("This may take a while. Please be patient while the game selects monsters...");
				if (i == 200) pline("Stand by while the monsters get generated...");
				if (i == 400) pline("Spawning monsters...");
				if (i == 600) pline("Please have some patience while monsters are selected...");
				if (i == 800) pline("Halfway done...");
				if (i == 1000) pline("Don't worry, the game does not hang. Give it a little more time...");
				if (i == 1200) pline("Soon you can start fighting the hordes...");
				if (i == 1400) pline("Almost finished...");
				if (i == 1680) pline("Done! The entire level is filled with monsters now! :-)");
#endif

				(void) makemon(randmonstforspawn, 0, 0, NO_MM_FLAGS);
			}

			}

			else if (!rn2(2)) {

#ifdef BIGSLEX
			randsp = 5000;
#else
			randsp = 1680;
#endif
			monstercolor = rnd(15);

			pline("CLICK! All you can make out is a strong predominant color... and an endless amount of monsters.");

			for (i = 0; i < randsp; i++) {

				if (!enexto(&ee, u.ux, u.uy, (struct permonst *)0) ) continue;

#ifdef BIGSLEX
				if (i == 500) pline("This may take a while. Please be patient while the game selects monsters...");
				if (i == 1000) pline("Stand by while the monsters get generated...");
				if (i == 1500) pline("Spawning monsters...");
				if (i == 2000) pline("Please have some patience while monsters are selected...");
				if (i == 2500) pline("Halfway done...");
				if (i == 3000) pline("Don't worry, the game does not hang. Give it a little more time...");
				if (i == 3500) pline("Soon you can start fighting the hordes...");
				if (i == 4000) pline("It's taking a while. Sorry. That's because the level is very big...");
				if (i == 4500) pline("Almost finished...");
				if (i == 5000) pline("Done! The entire level is filled with monsters now! :-)");
#else
				if (i == 100) pline("This may take a while. Please be patient while the game selects monsters...");
				if (i == 200) pline("Stand by while the monsters get generated...");
				if (i == 400) pline("Spawning monsters...");
				if (i == 600) pline("Please have some patience while monsters are selected...");
				if (i == 800) pline("Halfway done...");
				if (i == 1000) pline("Don't worry, the game does not hang. Give it a little more time...");
				if (i == 1200) pline("Soon you can start fighting the hordes...");
				if (i == 1400) pline("Almost finished...");
				if (i == 1680) pline("Done! The entire level is filled with monsters now! :-)");
#endif

				if (!rn2(20)) reset_rndmonst(NON_PM);

				(void) makemon(colormon(monstercolor), 0, 0, NO_MM_FLAGS);
			}

			}

			else {

#ifdef BIGSLEX
			randsp = 5000;
#else
			randsp = 1680;
#endif
			monstercolor = rnd(434);

			pline("CLICK! The entire area is filled with monsters! And they have one thing in common: they want to make your life miserable!");

			for (i = 0; i < randsp; i++) {

				if (!enexto(&ee, u.ux, u.uy, (struct permonst *)0) ) continue;

#ifdef BIGSLEX
				if (i == 500) pline("This may take a while. Please be patient while the game selects monsters...");
				if (i == 1000) pline("Stand by while the monsters get generated...");
				if (i == 1500) pline("Spawning monsters...");
				if (i == 2000) pline("Please have some patience while monsters are selected...");
				if (i == 2500) pline("Halfway done...");
				if (i == 3000) pline("Don't worry, the game does not hang. Give it a little more time...");
				if (i == 3500) pline("Soon you can start fighting the hordes...");
				if (i == 4000) pline("It's taking a while. Sorry. That's because the level is very big...");
				if (i == 4500) pline("Almost finished...");
				if (i == 5000) pline("Done! The entire level is filled with monsters now! :-)");
#else
				if (i == 100) pline("This may take a while. Please be patient while the game selects monsters...");
				if (i == 200) pline("Stand by while the monsters get generated...");
				if (i == 400) pline("Spawning monsters...");
				if (i == 600) pline("Please have some patience while monsters are selected...");
				if (i == 800) pline("Halfway done...");
				if (i == 1000) pline("Don't worry, the game does not hang. Give it a little more time...");
				if (i == 1200) pline("Soon you can start fighting the hordes...");
				if (i == 1400) pline("Almost finished...");
				if (i == 1680) pline("Done! The entire level is filled with monsters now! :-)");
#endif

				if (!rn2(20)) reset_rndmonst(NON_PM);

				(void) makemon(specialtensmon(monstercolor), 0, 0, NO_MM_FLAGS);
			}

			}

			u.aggravation = 0;

		 break;

		 case DISPLAY_TRAP:

			if (DisplayLoss) break;
			u.cnd_nastytrapamount++;

			DisplayLoss = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case SPELL_LOSS_TRAP:

			if (SpellLoss) break;
			u.cnd_nastytrapamount++;

			SpellLoss = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case YELLOW_SPELL_TRAP:

			if (YellowSpells) break;
			u.cnd_nastytrapamount++;

			YellowSpells = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case AUTO_DESTRUCT_TRAP:

			if (AutoDestruct) break;
			u.cnd_nastytrapamount++;

			AutoDestruct = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case MEMORY_TRAP:

			if (MemoryLoss) break;
			u.cnd_nastytrapamount++;

			MemoryLoss = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case INVENTORY_TRAP:

			if (InventoryLoss) break;
			u.cnd_nastytrapamount++;

			InventoryLoss = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case BLACK_NG_WALL_TRAP:

			if (BlackNgWalls) break;
			u.cnd_nastytrapamount++;

			BlackNgWalls = (blackngdur - (monster_difficulty() * 3));
			(void) makemon(&mons[PM_BLACKY], 0, 0, NO_MM_FLAGS);

		 break;


	    default:
		seetrap(trap);
		impossible("You hit a trap of type %u", trap->ttyp);
	}
}

STATIC_OVL void
rocks_fall(zx,zy)
int zx,zy;
{
	int dmg = 0;
	int mdmg,rocks,rx,ry;
	boolean somehit = FALSE;
	struct monst* mtmp;
	struct obj* mhat;
	struct obj* otmp;

	/* make sure this isn't an unavoidable instadeath at XL1 --Amy */
	int amountofrocks = rnd(100) + rnd(100);
	if (u.ulevel == 1) amountofrocks = rnd(30) + rnd(30);
	if (u.ulevel == 2) amountofrocks = rnd(50) + rnd(50);
	if (u.ulevel == 3) amountofrocks = rnd(70) + rnd(70);

	for (rocks = amountofrocks; rocks > 0; rocks--) {
		rx = zx + rn2(3)-1; 
		ry = zy + rn2(3)-1;
		if (isok(rx, ry) && levl[rx][ry].typ == ROOM || levl[rx][ry].typ == CORR) {
			if (rn2(50)) {
				otmp = mksobj(ROCK,FALSE,FALSE, FALSE);
			} else {
				otmp = mksobj(BOULDER,FALSE,FALSE, FALSE);
			}
			if (!otmp) continue;

			if(!rn2(8)) {
				otmp->spe = rne(2);
				if (rn2(2)) otmp->blessed = rn2(2);
				 else	blessorcurse(otmp, 3);
			} else if(!rn2(10)) {
				if (rn2(10)) curse(otmp);
				 else	blessorcurse(otmp, 3);
				otmp->spe = -rne(2);
			} else	blessorcurse(otmp, 10);

			/* in case of weirdness */
			if (!flooreffects(otmp,rx,ry,"fall")) {
				place_object(otmp,rx,ry);
				stackobj(otmp);
			}
			/* postpone player's possible death 'til all the rocks are down */
			if (rx == u.ux && ry == u.uy) { 
				somehit = TRUE; 
				dmg += rn2(2) + (otmp->otyp == BOULDER ? d(2,4) : 0);
			} else if (mtmp = (m_at(rx,ry))) {
				/* spare the player a barrage of messages for each time
				 * a monster is hit by a rock; just show the killed message */
				mhat = which_armor(mtmp, W_ARMH);
				mdmg = rn2(2) + (otmp->otyp == BOULDER ? d(2,4) : 0);
				if (thick_skinned(mtmp->data) || (mhat && is_metallic(mhat))) {
					mdmg /= 2;
				}
				if (!unsolid(mtmp->data)) {
					damage_mon(mtmp,mdmg,AD_PHYS);
					if (mtmp->mhp <= 0) { monkilled(mtmp,"shower of rocks",AD_PHYS); }
				}
			}
			newsym(rx,ry);
		}
	}

	if (somehit) {
		pline("Some rocks land on your %s!",body_part(HEAD));
		if (uarmh) {
			if (is_hardmaterial(uarmh)) {
				pline("Your hard helmet protects you somewhat.");
				dmg /= 2;
			} else if (flags.verbose) {
				Your("%s does not protect you.", xname(uarmh));
			}
		}
	}
	if (dmg > 1) {
		if (u.ulevel == 1) dmg /= 2;
		else if (u.ulevel == 2) {
			dmg *= 2;
			dmg /= 3;
		} else if (u.ulevel == 3) {
			dmg *= 3;
			dmg /= 4;
		} else if (u.ulevel == 4) {
			dmg *= 4;
			dmg /= 5;
		}
	}
	losehp(dmg + rnd((monster_difficulty()*2) + 1 ),"shower of rocks",KILLED_BY_AN);

}



STATIC_OVL int
steedintrap(trap, otmp)
struct trap *trap;
struct obj *otmp;
{
	struct monst *mtmp = u.usteed;
	struct permonst *mptr;
	int tt;
	boolean in_sight;
	boolean trapkilled = FALSE;
	boolean steedhit = FALSE;

	if (!u.usteed || !trap) return 0;
	mptr = mtmp->data;
	tt = trap->ttyp;
	mtmp->mx = u.ux;
	mtmp->my = u.uy;

	in_sight = !Blind;
	switch (tt) {
		case PET_TRAP:
			pline("%s triggers a trap and dies!", Monnam(mtmp));
			mondead(mtmp);
			steedhit = TRUE;
			trapkilled = TRUE;
			break;
		case ARROW_TRAP:
			if(!otmp) {
				impossible("steed hit by non-existant arrow?");
				return 0;
			}
			if (thitm(8, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
			steedhit = TRUE;
			break;
		case POISON_ARROW_TRAP:
			if(!otmp) {
				impossible("steed hit by non-existant arrow?");
				return 0;
			}
			if (thitm(8, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
			steedhit = TRUE;
			break;
		case BOLT_TRAP:
			if(!otmp) {
				impossible("steed hit by non-existant bolt?");
				return 0;
			}
			if (thitm(8, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
			steedhit = TRUE;
			break;
		case POISON_BOLT_TRAP:
			if(!otmp) {
				impossible("steed hit by non-existant bolt?");
				return 0;
			}
			if (thitm(8, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
			steedhit = TRUE;
			break;
		case BULLET_TRAP:
			if(!otmp) {
				impossible("steed hit by non-existant bullet?");
				return 0;
			}
			if (thitm(8, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
			steedhit = TRUE;
			break;
		case GLASS_ARROW_TRAP:
			if(!otmp) {
				impossible("steed hit by non-existant glass arrow?");
				return 0;
			}
			if (thitm(8, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
			steedhit = TRUE;
			break;
		case GLASS_BOLT_TRAP:
			if(!otmp) {
				impossible("steed hit by non-existant glass bolt?");
				return 0;
			}
			if (thitm(8, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
			steedhit = TRUE;
			break;
		case DART_TRAP:
			if(!otmp) {
				impossible("steed hit by non-existant dart?");
				return 0;
			}
			if (thitm(7, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
			steedhit = TRUE;
			break;
		case THROWING_STAR_TRAP:
			if(!otmp) {
				impossible("steed hit by non-existant shuriken?");
				return 0;
			}
			if (thitm(8, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
			steedhit = TRUE;
			break;
		case SLP_GAS_TRAP:
		    if (!resists_sleep(mtmp) && !breathless(mptr) && (!mtmp->egotype_undead) &&
				!mtmp->msleeping && mtmp->mcanmove) {
			    mtmp->mcanmove = 0;
			    mtmp->mfrozen = rnd(25);
			    if (in_sight) {
				pline("%s suddenly falls asleep!",
				      Monnam(mtmp));
			    }
			}
			steedhit = TRUE;
			break;

		case PARALYSIS_TRAP:

			if (FemtrapActiveKatrin && !mtmp->mpeaceful && !mtmp->mtame) break; /* immune */

			if (dmgtype(mtmp->data, AD_PLYS)) break;
		      {
			    mtmp->mcanmove = 0;
			    mtmp->mfrozen = rnd(10);
			    if (in_sight) {
				pline("%s is suddenly paralyzed!", Monnam(mtmp));
			    }
			}
			steedhit = TRUE;
			break;

		case SLOW_GAS_TRAP:
		    if (!breathless(mptr) && (!mtmp->egotype_undead) ) {

			    if (in_sight) {
				pline("%s inhales a cloud of foggy gas!", Monnam(mtmp));
			    }
			mon_adjust_speed(mtmp, -1, (struct obj *)0);
			}
			steedhit = TRUE;
			break;

		case POISON_GAS_TRAP:
		    if (!resists_poison(mtmp) && !breathless(mptr) && (!mtmp->egotype_undead) ) {

			    if (in_sight) {
				pline("%s inhales a cloud of poison gas!", Monnam(mtmp));
				seetrap(trap);

			    }

	               int num = d(3,4);
	               if (thitm(0, mtmp, (struct obj *)0, num, FALSE))
	                 trapkilled = TRUE;
			}
			steedhit = TRUE;
			break;


		case SPEAR_TRAP:
			if (thitm(0, mtmp, (struct obj*)0, rnd(10)+10, FALSE)) {
				trapkilled = TRUE;
			}
			steedhit = TRUE;
			break;
		case LANDMINE:
			if (thitm(0, mtmp, (struct obj *)0, rnd(16), FALSE))
			    trapkilled = TRUE;
			steedhit = TRUE;
			break;
		case PIT:
		case SPIKED_PIT:
		case GIANT_CHASM:
		case SHIT_PIT:
		case MANA_PIT:
		case ANOXIC_PIT:
		case HYPOXIC_PIT:
		case ACID_PIT:
			if (mtmp->mhp <= 0 ||
				thitm(0, mtmp, (struct obj *)0,
				      rnd((tt == PIT) ? 6 : (tt == GIANT_CHASM) ? 15 : 10), FALSE))
			    trapkilled = TRUE;
			steedhit = TRUE;
			break;
		case POLY_TRAP: 
		    if (!resists_magm(mtmp)) {
			if (!resist(mtmp, WAND_CLASS, 0, NOTELL)) {
			    (void) mon_spec_poly(mtmp, (struct permonst *)0, 0L, FALSE, FALSE, FALSE, TRUE); 
			    if (!can_saddle(mtmp) || !can_ride(mtmp)) {
				dismount_steed(DISMOUNT_POLY);
			    } else {
				You("have to adjust yourself in the saddle on %s.",
					x_monnam(mtmp,
					mtmp->mnamelth ? ARTICLE_NONE : ARTICLE_A,
					(char *)0, SUPPRESS_SADDLE, FALSE));
				if (nogoodsteed(mtmp)) pline("This steed can't carry you effectively!");
				else if (dedicatedsteed(mtmp)) You_feel("comfortable riding %s.",
					x_monnam(mtmp,
					mtmp->mnamelth ? ARTICLE_NONE : ARTICLE_A,
					(char *)0, SUPPRESS_SADDLE, FALSE) );
				else Your("steed doesn't seem all that comfortable.");
			    }
			}
			steedhit = TRUE;
		    }
		    break;
		case COLLAPSE_TRAP:	 /* TODO: make this affect steeds */
			 break;
		default:
			return 0;
	}
	if(trapkilled) {
		dismount_steed(DISMOUNT_POLY);
		return 2;
	}
	else if(steedhit) return 1;
	else return 0;
}

/* some actions common to both player and monsters for triggered landmine */
void
blow_up_landmine(trap)
struct trap *trap;
{
	(void)scatter(trap->tx, trap->ty, 4,
		MAY_DESTROY | MAY_HIT | MAY_FRACTURE | VIS_EFFECTS,
		(struct obj *)0);
	del_engr_at(trap->tx, trap->ty);
	wake_nearto(trap->tx, trap->ty, 400);
	/* ALI - artifact doors */
	if (IS_DOOR(levl[trap->tx][trap->ty].typ) &&
		!artifact_door(trap->tx, trap->ty))
	    levl[trap->tx][trap->ty].doormask = D_BROKEN;
	/* TODO: destroy drawbridge if present */
	/* caller may subsequently fill pit, e.g. with a boulder */
	trap->ttyp = PIT;		/* explosion creates a pit */
	trap->madeby_u = FALSE;		/* resulting pit isn't yours */
	seetrap(trap);			/* and it isn't concealed */
}

#endif /* OVLB */
#ifdef OVL3

/*
 * Move obj from (x1,y1) to (x2,y2)
 *
 * Return 0 if no object was launched.
 *        1 if an object was launched and placed somewhere.
 *        2 if an object was launched, but used up.
 */
int
launch_obj(otyp, x1, y1, x2, y2, style)
short otyp;
register int x1,y1,x2,y2;
int style;
{
	register struct monst *mtmp;
	register struct obj *otmp, *otmp2;
	register int dx,dy;
	struct obj *singleobj;
	boolean used_up = FALSE;
	boolean otherside = FALSE;
	int dist;
	int tmp;
	int delaycnt = 0;

	otmp = sobj_at(otyp, x1, y1);
	/* Try the other side too, for rolling boulder traps */
	if (!otmp && otyp == BOULDER) {
		otherside = TRUE;
		otmp = sobj_at(otyp, x2, y2);
	}
	if (!otmp) return 0;
	if (otherside) {	/* swap 'em */
		int tx, ty;

		tx = x1; ty = y1;
		x1 = x2; y1 = y2;
		x2 = tx; y2 = ty;
	}

	if (otmp->quan == 1L) {
	    obj_extract_self(otmp);
	    singleobj = otmp;
	    otmp = (struct obj *) 0;
	} else {
	    singleobj = splitobj(otmp, 1L);
	    obj_extract_self(singleobj);
	}
	newsym(x1,y1);
	/* in case you're using a pick-axe to chop the boulder that's being
	   launched (perhaps a monster triggered it), destroy context so that
	   next dig attempt never thinks you're resuming previous effort */
	if ((otyp == BOULDER || otyp == STATUE) &&
	    singleobj->ox == digging.pos.x && singleobj->oy == digging.pos.y)
	    (void) memset((void *)&digging, 0, sizeof digging);

	dist = distmin(x1,y1,x2,y2);
	bhitpos.x = x1;
	bhitpos.y = y1;
	dx = sgn(x2 - x1);
	dy = sgn(y2 - y1);
	switch (style) {
	    case ROLL|LAUNCH_UNSEEN:
			if (otyp == BOULDER) {
			    You_hear(FunnyHallu ?
				     "someone bowling." :
				     "rumbling in the distance.");
			}
			style &= ~LAUNCH_UNSEEN;
			goto roll;
	    case ROLL|LAUNCH_KNOWN:
			/* use otrapped as a flag to ohitmon */
			singleobj->otrapped = 1;
			style &= ~LAUNCH_KNOWN;
			/* fall through */
	    roll:
	    case ROLL:
			delaycnt = 2;
			/* fall through */
	    default:
			if (!delaycnt) delaycnt = 1;
			if (!cansee(bhitpos.x,bhitpos.y)) curs_on_u();
			tmp_at(DISP_FLASH, obj_to_glyph(singleobj));
			tmp_at(bhitpos.x, bhitpos.y);
	}

	/* Set the object in motion */
	while(dist-- > 0 && !used_up) {
		struct trap *t;
		tmp_at(bhitpos.x, bhitpos.y);
		tmp = delaycnt;

		/* dstage@u.washington.edu -- Delay only if hero sees it */
		if (cansee(bhitpos.x, bhitpos.y))
			while (tmp-- > 0) delay_output();

		bhitpos.x += dx;
		bhitpos.y += dy;
		t = t_at(bhitpos.x, bhitpos.y);
		
		if ((mtmp = m_at(bhitpos.x, bhitpos.y)) != 0) {
			if (otyp == BOULDER && throws_rocks(mtmp->data)) {
			    if (rn2(3)) {
				if (cansee(bhitpos.x, bhitpos.y))
				    pline("%s snatches the boulder.",
					    Monnam(mtmp));
				else
				    You_hear("a rumbling stop abruptly.");
				singleobj->otrapped = 0;
				(void) mpickobj(mtmp, singleobj, FALSE);
				used_up = TRUE;
				break;
			    }
			}
			if (ohitmon((struct monst *) 0, mtmp,singleobj,
					(style==ROLL) ? -1 : dist, FALSE)) {
				used_up = TRUE;
				break;
			}
		} else if (bhitpos.x == u.ux && bhitpos.y == u.uy) {
			if (multi) nomul(0, 0, FALSE);
			if (thitu(9 + singleobj->spe,
				  dmgval(singleobj, &youmonst),
				  singleobj, (char *)0))
			    stop_occupation();
		}
		if (style == ROLL) {
		    if (down_gate(bhitpos.x, bhitpos.y) != -1) {
		        if(ship_object(singleobj, bhitpos.x, bhitpos.y, FALSE)){
				used_up = TRUE;
				break;
			}
		    }
		    if (t && otyp == BOULDER) {
			switch(t->ttyp) {
			case LANDMINE:
			    if (rn2(10) > 2) {
			  	pline(
				  "KAABLAMM!!!%s",
				  cansee(bhitpos.x, bhitpos.y) ?
					" The rolling boulder triggers a land mine." : "");
				deltrap(t);
				del_engr_at(bhitpos.x,bhitpos.y);
				place_object(singleobj, bhitpos.x, bhitpos.y);
				singleobj->otrapped = 0;
				fracture_rock(singleobj);
				(void)scatter(bhitpos.x,bhitpos.y, 4,
					MAY_DESTROY|MAY_HIT|MAY_FRACTURE|VIS_EFFECTS,
					(struct obj *)0);
				if (cansee(bhitpos.x,bhitpos.y))
					newsym(bhitpos.x,bhitpos.y);
			        used_up = TRUE;
			    }
			    break;		
			case LEVEL_TELEP:
			case LEVEL_BEAMER:
			case TELEP_TRAP:
			case BEAMER_TRAP:
			    if (cansee(bhitpos.x, bhitpos.y))
			    	pline("Suddenly the rolling boulder disappears!");
			    else
			    	You_hear("a rumbling stop abruptly.");
			    singleobj->otrapped = 0;
			    if (t->ttyp == TELEP_TRAP)
				rloco(singleobj);
			    else if (t->ttyp == BEAMER_TRAP)
				rloco(singleobj);
			    else {
				int newlev = random_teleport_level();
				d_level dest;

				if (newlev == depth(&u.uz) || In_endgame(&u.uz))
				    continue;
				add_to_migration(singleobj);
				get_level(&dest, newlev);
				singleobj->ox = dest.dnum;
				singleobj->oy = dest.dlevel;
				singleobj->owornmask = (long)MIGR_RANDOM;
			    }
		    	    seetrap(t);
			    used_up = TRUE;
			    break;
			case PIT:
			case SPIKED_PIT:
			case GIANT_CHASM:
			case SHIT_PIT:
			case MANA_PIT:
			case ANOXIC_PIT:
			case HYPOXIC_PIT:
			case ACID_PIT:
			case SHAFT_TRAP:
			case CURRENT_SHAFT:
			case HOLE:
			case TRAPDOOR:
			    /* the boulder won't be used up if there is a
			       monster in the trap; stop rolling anyway */
			    x2 = bhitpos.x,  y2 = bhitpos.y;  /* stops here */
			    if (flooreffects(singleobj, x2, y2, "fall"))
				used_up = TRUE;
			    dist = -1;	/* stop rolling immediately */
			    break;
			}
			if (used_up || dist == -1) break;
		    }
		    if (flooreffects(singleobj, bhitpos.x, bhitpos.y, "fall")) {
			used_up = TRUE;
			break;
		    }
		    if (otyp == BOULDER &&
		       (otmp2 = sobj_at(BOULDER, bhitpos.x, bhitpos.y)) != 0) {
			const char *bmsg =
				" as one boulder sets another in motion";

			if (!isok(bhitpos.x + dx, bhitpos.y + dy) || !dist ||
			    IS_ROCK(levl[bhitpos.x + dx][bhitpos.y + dy].typ))
			    bmsg = " as one boulder hits another";

		    	You_hear("a loud crash%s!",
				cansee(bhitpos.x, bhitpos.y) ? bmsg : "");
			obj_extract_self(otmp2);
			/* pass off the otrapped flag to the next boulder */
			otmp2->otrapped = singleobj->otrapped;
			singleobj->otrapped = 0;
			place_object(singleobj, bhitpos.x, bhitpos.y);
			singleobj = otmp2;
			otmp2 = (struct obj *)0;
			wake_nearto(bhitpos.x, bhitpos.y, 10*10);
		    }
		}
		if (otyp == BOULDER && closed_door(bhitpos.x,bhitpos.y)) {
			if (cansee(bhitpos.x, bhitpos.y))
				pline_The("boulder crashes through a door.");
			levl[bhitpos.x][bhitpos.y].doormask = D_BROKEN;
			if (dist) unblock_point(bhitpos.x, bhitpos.y);
		}

		/* if about to hit iron bars, do so now */
		if (dist > 0 && isok(bhitpos.x + dx,bhitpos.y + dy) &&
			levl[bhitpos.x + dx][bhitpos.y + dy].typ == IRONBARS) {
		    x2 = bhitpos.x,  y2 = bhitpos.y;	/* object stops here */
		    if (hits_bars(&singleobj, x2, y2, !rn2(20), 0)) {
			if (!singleobj) used_up = TRUE;
			break;
		    }
		}
	}
	tmp_at(DISP_END, 0);
	if (!used_up) {
		singleobj->otrapped = 0;
		place_object(singleobj, x2,y2);
		newsym(x2,y2);
		return 1;
	} else
		return 2;
}

#endif /* OVL3 */
#ifdef OVLB

void
seetrap(trap)
	register struct trap *trap;
{

	if (Race_if(PM_SPARD) && rn2(3)) return;

	if (uarmf && itemhasappearance(uarmf, APP_SKI_HEELS)) return;

	if(!trap->tseen && !trap->hiddentrap) {
	    trap->tseen = 1;
	    newsym(trap->tx, trap->ty);
	}
}

#endif /* OVLB */
#ifdef OVL3

STATIC_OVL int
mkroll_launch(ttmp, x, y, otyp, ocount)
struct trap *ttmp;
xchar x,y;
short otyp;
long ocount;
{
	struct obj *otmp;
	register int tmp;
	schar dx,dy;
	int distance;
	coord cc;
	coord bcc;
	int trycount = 0;
	boolean success = FALSE;
	int mindist = 0;

	if (ttmp->ttyp == ROLLING_BOULDER_TRAP) mindist = 2;
	distance = rnd(8);    /* 4..8 away */
	tmp = rn2(8);		/* randomly pick a direction to try first */
	while (distance >= mindist) {
		dx = xdir[tmp];
		dy = ydir[tmp];
		cc.x = x; cc.y = y;
		/* Prevent boulder from being placed on water */
		if (ttmp->ttyp == ROLLING_BOULDER_TRAP
				&& is_waterypool(x+distance*dx,y+distance*dy))
			success = FALSE;
		else success = isclearpath(&cc, distance, dx, dy);
		if (ttmp->ttyp == ROLLING_BOULDER_TRAP) {
			boolean success_otherway;
			bcc.x = x; bcc.y = y;
			success_otherway = isclearpath(&bcc, distance,
						-(dx), -(dy));
			if (!success_otherway) success = FALSE;
		}
		if (success) break;
		if (++tmp > 7) tmp = 0;
		if ((++trycount % 8) == 0) --distance;
	}
	if (!success) {
	    /* create the trap without any ammo, launch pt at trap location */
		cc.x = bcc.x = x;
		cc.y = bcc.y = y;
	} else {
		otmp = mksobj(otyp, TRUE, FALSE, FALSE);
		if (otmp) {
			otmp->quan = ocount;
			otmp->owt = weight(otmp);
			place_object(otmp, cc.x, cc.y);
			stackobj(otmp);
		}
	}
	ttmp->launch.x = cc.x;
	ttmp->launch.y = cc.y;
	if (ttmp->ttyp == ROLLING_BOULDER_TRAP) {
		ttmp->launch2.x = bcc.x;
		ttmp->launch2.y = bcc.y;
	} else
		ttmp->launch_otyp = otyp;
	newsym(ttmp->launch.x, ttmp->launch.y);
	return 1;
}

STATIC_OVL boolean
isclearpath(cc,distance,dx,dy)
coord *cc;
int distance;
schar dx,dy;
{
	uchar typ;
	xchar x, y;

	x = cc->x;
	y = cc->y;
	while (distance-- > 0) {
		x += dx;
		y += dy;
		typ = levl[x][y].typ;
		if (!isok(x,y) || !ZAP_POS(typ) || closed_door(x,y))
			return FALSE;
	}
	cc->x = x;
	cc->y = y;
	return TRUE;
}
#endif /* OVL3 */
#ifdef OVL1

void
mon_drain_en(mtmp, n)
struct monst *mtmp;
register int n;
{
	if (!mtmp->m_enmax) return;
	if (canseemon(mtmp)) 
		pline("%s looks less powerful!", Monnam(mtmp));
	mtmp->m_en -= n;
	if(mtmp->m_en < 0)  {
		mtmp->m_enmax += mtmp->m_en;
		if(mtmp->m_enmax < 0) mtmp->m_enmax = 0;
		mtmp->m_en = 0;
	}
}


int
mintrap(mtmp)
register struct monst *mtmp;
{
	register struct trap *trap = t_at(mtmp->mx, mtmp->my);
	boolean trapkilled = FALSE;
	struct permonst *mptr = mtmp->data;
	struct obj *otmp;

	if (!trap) {
	    mtmp->mtrapped = 0;	/* perhaps teleported? */
	} else if (mtmp->mtrapped) {	/* is currently in the trap */
	    if (!trap->tseen && !trap->hiddentrap && 
		cansee(mtmp->mx, mtmp->my) && canseemon(mtmp) &&
		(trap->ttyp == SPIKED_PIT || trap->ttyp == GIANT_CHASM || trap->ttyp == SHIT_PIT || trap->ttyp == MANA_PIT || trap->ttyp == ANOXIC_PIT || trap->ttyp == HYPOXIC_PIT || trap->ttyp == BEAR_TRAP || trap->ttyp == ACID_PIT ||
		 trap->ttyp == HOLE || trap->ttyp == PIT ||
		 trap->ttyp == WEB)) {
		/* If you come upon an obviously trapped monster, then
		 * you must be able to see the trap it's in too.
		 */
		seetrap(trap);
	    }

		/* it's ridiculous that they take soooooo damn long to wriggle free from a trap, especially considering
		 * that your char can escape from pits and bear traps in 5 turns or so... --Amy */
	    if (!rn2(strongmonst(mptr) ? 5 : 10)) {
		if (sobj_at(BOULDER, mtmp->mx, mtmp->my) &&
			(trap->ttyp == PIT || trap->ttyp == SPIKED_PIT || trap->ttyp == GIANT_CHASM || trap->ttyp == SHIT_PIT || trap->ttyp == MANA_PIT || trap->ttyp == ANOXIC_PIT || trap->ttyp == HYPOXIC_PIT || trap->ttyp == ACID_PIT)) {
		    if (!rn2(2)) {
			mtmp->mtrapped = 0;
			if (canseemon(mtmp))
			    pline("%s pulls free...", Monnam(mtmp));
			fill_pit(mtmp->mx, mtmp->my);
		    }
		} else {
		    mtmp->mtrapped = 0;
		}
	    } else if (metallivorous(mptr)) {
		if (trap->ttyp == BEAR_TRAP) {
		    if (canseemon(mtmp))
			pline("%s eats a bear trap!", Monnam(mtmp));
		    deltrap(trap);
		    mtmp->meating = 5;
		    mtmp->mtrapped = 0;
		} else if (trap->ttyp == SPIKED_PIT) {
		    if (canseemon(mtmp))
			pline("%s munches on some spikes!", Monnam(mtmp));
		    trap->ttyp = PIT;
		    mtmp->meating = 5;
		}
	    } else if (trap->ttyp == BEAR_TRAP && strongmonst(mptr) && !rn2(250)) {
		    if (canseemon(mtmp))
			pline("%s manages to rip the bear trap in half!", Monnam(mtmp));
		    deltrap(trap);
		    mtmp->mtrapped = 0;
	    }
	} else {
	    register int tt = trap->ttyp;
	    boolean in_sight, tear_web, see_it,
		    inescapable = ((tt == HOLE || tt == PIT) &&
				   In_sokoban(&u.uz) && !trap->madeby_u);
	    const char *fallverb;

	    /* true when called from dotrap, inescapable is not an option */
	    if (mtmp == u.usteed) inescapable = TRUE;
	    if (!inescapable &&
		    (/*(mtmp->mtrapseen & (1 << (tt-1))) != 0 ||*/ !rn2(3) ||
			(tt == HOLE && !mindless(mtmp->data)))) {
		/* it has been in such a trap - perhaps it escapes */
		/* Amy edit: mtrapseen does no longer exist, so just generally have a chance of the monster escaping */
		if(rn2(4)) return(0);
	    } /*else {
		mtmp->mtrapseen |= (1 << (tt-1));
	    }*/
	    /* Monster is aggravated by being trapped by you.
	       Recognizing who made the trap isn't completely
	       unreasonable; everybody has their own style. */
	    if (trap->madeby_u && rnl(5)) setmangry(mtmp);

	    in_sight = canseemon(mtmp);
	    see_it = cansee(mtmp->mx, mtmp->my);
	    /* assume hero can tell what's going on for the steed */
	    if (mtmp == u.usteed) in_sight = TRUE;
	    switch (tt) {
		case ARROW_TRAP:
			if (trap->once && !rn2(15)) {
			    if (in_sight && see_it)
				pline("%s triggers a trap but nothing happens.",
				      Monnam(mtmp));
			    deltrap(trap);
			    newsym(mtmp->mx, mtmp->my);
			    break;
			}
			trap->once = 1;
			otmp = mksobj(ARROW, TRUE, FALSE, FALSE);
			if (otmp) {
				otmp->quan = 1L;
				otmp->owt = weight(otmp);
				otmp->opoisoned = 0;
				if (in_sight) seetrap(trap);
				if (thitm(8, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
			}
			break;
		case POISON_ARROW_TRAP:
			if (trap->once && !rn2(15)) {
			    if (in_sight && see_it)
				pline("%s triggers a trap but nothing happens.",
				      Monnam(mtmp));
			    deltrap(trap);
			    newsym(mtmp->mx, mtmp->my);
			    break;
			}
			trap->once = 1;
			otmp = mksobj(ARROW, TRUE, FALSE, FALSE);
			if (otmp) {
				otmp->quan = 1L;
				otmp->owt = weight(otmp);
				otmp->opoisoned = TRUE;
				if (in_sight) seetrap(trap);
				if (thitm(8, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
			}
			break;
		case BOLT_TRAP:
			if (trap->once && !rn2(15)) {
			    if (in_sight && see_it)
				pline("%s triggers a trap but nothing happens.",
				      Monnam(mtmp));
			    deltrap(trap);
			    newsym(mtmp->mx, mtmp->my);
			    break;
			}
			trap->once = 1;
			otmp = mksobj(CROSSBOW_BOLT, TRUE, FALSE, FALSE);
			if (otmp) {
				otmp->quan = 1L;
				otmp->owt = weight(otmp);
				otmp->opoisoned = 0;
				if (in_sight) seetrap(trap);
				if (thitm(8, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
			}
			break;
		case POISON_BOLT_TRAP:
			if (trap->once && !rn2(15)) {
			    if (in_sight && see_it)
				pline("%s triggers a trap but nothing happens.",
				      Monnam(mtmp));
			    deltrap(trap);
			    newsym(mtmp->mx, mtmp->my);
			    break;
			}
			trap->once = 1;
			otmp = mksobj(CROSSBOW_BOLT, TRUE, FALSE, FALSE);
			if (otmp) {
				otmp->quan = 1L;
				otmp->owt = weight(otmp);
				otmp->opoisoned = TRUE;
				if (in_sight) seetrap(trap);
				if (thitm(8, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
			}
			break;
		case BULLET_TRAP:
			if (trap->once && !rn2(15)) {
			    if (in_sight && see_it)
				pline("%s triggers a trap but nothing happens.",
				      Monnam(mtmp));
			    deltrap(trap);
			    newsym(mtmp->mx, mtmp->my);
			    break;
			}
			trap->once = 1;
			otmp = mksobj(PISTOL_BULLET, TRUE, FALSE, FALSE);
			if (otmp) {
				otmp->quan = 1L;
				otmp->owt = weight(otmp);
				otmp->opoisoned = 0;
				if (in_sight) seetrap(trap);
				if (thitm(8, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
			}
			break;
		case GLASS_ARROW_TRAP:
			if (trap->once && !rn2(15)) {
			    if (in_sight && see_it)
				pline("%s triggers a trap but nothing happens.",
				      Monnam(mtmp));
			    deltrap(trap);
			    newsym(mtmp->mx, mtmp->my);
			    break;
			}
			trap->once = 1;
			otmp = mksobj(DROVEN_ARROW, TRUE, FALSE, FALSE);
			if (otmp) {
				otmp->quan = 1L;
				otmp->owt = weight(otmp);
				otmp->opoisoned = 0;
				if (in_sight) seetrap(trap);
				if (thitm(8, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
			}
			break;
		case GLASS_BOLT_TRAP:
			if (trap->once && !rn2(15)) {
			    if (in_sight && see_it)
				pline("%s triggers a trap but nothing happens.",
				      Monnam(mtmp));
			    deltrap(trap);
			    newsym(mtmp->mx, mtmp->my);
			    break;
			}
			trap->once = 1;
			otmp = mksobj(DROVEN_BOLT, TRUE, FALSE, FALSE);
			if (otmp) {
				otmp->quan = 1L;
				otmp->owt = weight(otmp);
				otmp->opoisoned = 0;
				if (in_sight) seetrap(trap);
				if (thitm(8, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
			}
			break;
		case DART_TRAP:
			if (trap->once && !rn2(15)) {
			    if (in_sight && see_it)
				pline("%s triggers a trap but nothing happens.",
				      Monnam(mtmp));
			    deltrap(trap);
			    newsym(mtmp->mx, mtmp->my);
			    break;
			}
			trap->once = 1;
			otmp = mksobj(DART, TRUE, FALSE, FALSE);
			if (otmp) {
				otmp->quan = 1L;
				otmp->owt = weight(otmp);
				if (!rn2(6)) otmp->opoisoned = 1;
				if (in_sight) seetrap(trap);
				if (thitm(7, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
			}
			break;
		case THROWING_STAR_TRAP:
			if (trap->once && !rn2(15)) {
			    if (in_sight && see_it)
				pline("%s triggers a trap but nothing happens.",
				      Monnam(mtmp));
			    deltrap(trap);
			    newsym(mtmp->mx, mtmp->my);
			    break;
			}
			trap->once = 1;
			otmp = mksobj(SHURIKEN, TRUE, FALSE, FALSE);
			if (otmp) {
				otmp->quan = 1L;
				otmp->owt = weight(otmp);
				if (!rn2(6)) otmp->opoisoned = 1;
				if (in_sight) seetrap(trap);
				if (thitm(8, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
			}
			break;
		case ROCKTRAP:
			if (trap->once && !rn2(15)) {
			    if (in_sight && see_it)
				pline("A trap door above %s opens, but nothing falls out!",
				      mon_nam(mtmp));
			    deltrap(trap);
			    newsym(mtmp->mx, mtmp->my);
			    break;
			}
			trap->once = 1;
			otmp = mksobj(ROCK, TRUE, FALSE, FALSE);
			if (otmp) {
				otmp->quan = 1L;
				otmp->owt = weight(otmp);
				otmp->opoisoned = 0;
				if (in_sight) seetrap(trap);
				if (thitm(0, mtmp, otmp, d(2, 6), FALSE))
				    trapkilled = TRUE;
			}
			break;

		case FALLING_BOULDER_TRAP:
			if (trap->once && !rn2(15)) {
			    if (in_sight && see_it)
				pline("A trap door above %s opens, but nothing falls out!",
				      mon_nam(mtmp));
			    deltrap(trap);
			    newsym(mtmp->mx, mtmp->my);
			    break;
			}
			trap->once = 1;
			otmp = mksobj(BOULDER, TRUE, FALSE, FALSE);
			if (otmp) {
				otmp->quan = 1L;
				otmp->owt = weight(otmp);
				otmp->opoisoned = 0;
				if (in_sight) seetrap(trap);
				if (thitm(0, mtmp, otmp, d(6, 6), FALSE))
				    trapkilled = TRUE;
			}
			break;

		case SQKY_BOARD:
			if(is_flyer(mptr) || mtmp->egotype_flying) break;
			/* stepped on a squeaky board */
			if (in_sight) {
			    pline("A board beneath %s squeaks loudly.", mon_nam(mtmp));
			    seetrap(trap);
			} else
			   You_hear("a distant squeak.");
			/* wake up nearby monsters */
			wake_nearto(mtmp->mx, mtmp->my, 40);
			break;

		case NOISE_TRAP:
			if (in_sight) {
			    pline("Suddenly %s steps on a trigger, and generates a hellish noise!", mon_nam(mtmp));
			    seetrap(trap);
			} else
			   You_hear("a hellish noise!");
			/* wake up nearby monsters */
			wake_nearto(mtmp->mx, mtmp->my, 40);
			{
				register struct monst *mtmp3;
				for(mtmp3 = fmon; mtmp3; mtmp3 = mtmp3->nmon) {
				    if (!DEADMONSTER(mtmp3) && !rn2(3)) {
					mtmp3->msleeping = 0;
				    }
				}

			}
			break;

		case BEAR_TRAP:
			/* Amy edit: very large monsters, as well as strong monsters, aren't always affected */
			if(mptr->msize > MZ_SMALL &&
				!amorphous(mptr) && !is_flyer(mptr) && (!mtmp->egotype_flying) &&
				!is_whirly(mptr) && !(strongmonst(mptr) && rn2(3)) && !(hugemonst(mptr) && rn2(3)) && !((mptr)->msize == MZ_HUGE && rn2(2)) && !unsolid(mptr)) {
			    mtmp->mtrapped = 1;
			    if(in_sight) {
				pline("%s is caught in %s bear trap!",
				      Monnam(mtmp), a_your[trap->madeby_u]);
				seetrap(trap);
			    } else {
				if((mptr == &mons[PM_OWLBEAR]
				    || mptr == &mons[PM_BUGBEAR])
				   && flags.soundok)
				    You_hear("the roaring of an angry bear!");
			    }
			}
			break;

		case SLP_GAS_TRAP:
		    if (!(mtmp->mtame) && rn2(3)) break;
		    if (!resists_sleep(mtmp) && !breathless(mptr) && (!mtmp->egotype_undead) &&
				!mtmp->msleeping && mtmp->mcanmove) {
			    mtmp->mcanmove = 0;
			    mtmp->mfrozen = rnd(25);
			    if (in_sight) {
				pline("%s suddenly falls asleep!",
				      Monnam(mtmp));
				seetrap(trap);
			    }
			if (!rn2(50) && !(mtmp && mtmp->mtame)) deltrap(trap);
			}
			break;

		case PARALYSIS_TRAP:
		    if (!(mtmp->mtame) && rn2(10)) break;
			if (dmgtype(mtmp->data, AD_PLYS)) break;

		      {
			    mtmp->mcanmove = 0;
			    mtmp->mfrozen = rnd(10);
			    if (in_sight) {
				pline("%s is suddenly paralyzed!", Monnam(mtmp));
				seetrap(trap);
			    }
			if (!rn2(50) && !(mtmp && mtmp->mtame)) deltrap(trap);
			}
			break;

		case POISON_GAS_TRAP:
		    if (!(mtmp->mtame) && rn2(3)) break;
		    if (!resists_poison(mtmp) && !breathless(mptr) && (!mtmp->egotype_undead) ) {

			    if (in_sight) {
				pline("%s inhales a cloud of poison gas!", Monnam(mtmp));
				seetrap(trap);

			    }
	               int num = d(3,4);
	               if (thitm(0, mtmp, (struct obj *)0, num, FALSE))
	                 trapkilled = TRUE;

			if (!rn2(50) && !(mtmp && mtmp->mtame)) deltrap(trap);
			}
			break;

		case SLOW_GAS_TRAP:
		    if (!(mtmp->mtame) && rn2(3)) break;
		    if (!breathless(mptr) && (!mtmp->egotype_undead) ) {

			    if (in_sight) {
				pline("%s inhales a cloud of foggy gas!", Monnam(mtmp));
				seetrap(trap);
			    }
			mon_adjust_speed(mtmp, -1, (struct obj *)0);
			if (!rn2(50) && !(mtmp && mtmp->mtame)) deltrap(trap);
			}
			break;

		case RUST_TRAP:
		    if (!(mtmp->mtame) && rn2(7)) break;
		    {
			struct obj *target;

			if (in_sight)
			    seetrap(trap);
			switch (rn2(5)) {
			case 0:
			    if (in_sight)
				pline("%s %s on the %s!", A_gush_of_water_hits,
				    mon_nam(mtmp), mbodypart(mtmp, HEAD));
			    target = which_armor(mtmp, W_ARMH);
			    (void) rust_dmg(target, "helmet", 1, TRUE, mtmp);
			    break;
			case 1:
			    if (in_sight)
				pline("%s %s's left %s!", A_gush_of_water_hits,
				    mon_nam(mtmp), mbodypart(mtmp, ARM));
			    target = which_armor(mtmp, W_ARMS);
			    if (rust_dmg(target, "shield", 1, TRUE, mtmp))
				break;
			    target = MON_WEP(mtmp);
			    if (target && bimanual(target))
				erode_obj(target, FALSE, TRUE);
glovecheck:		    target = which_armor(mtmp, W_ARMG);
			    if (target) (void) rust_dmg(target, "gauntlets", 1, TRUE, mtmp);
			    break;
			case 2:
			    if (in_sight)
				pline("%s %s's right %s!", A_gush_of_water_hits,
				    mon_nam(mtmp), mbodypart(mtmp, ARM));
			    if (MON_WEP(mtmp) && !snuff_lit(MON_WEP(mtmp)))
				erode_obj(MON_WEP(mtmp), FALSE, TRUE);
			    goto glovecheck;
			default:
			    if (in_sight)
				pline("%s %s!", A_gush_of_water_hits,
				    mon_nam(mtmp));
			    for (otmp=mtmp->minvent; otmp; otmp = otmp->nobj)
				(void) snuff_lit(otmp);
			    target = which_armor(mtmp, W_ARMC);
			    if (target)
				(void) rust_dmg(target, cloak_simple_name(target),
						 1, TRUE, mtmp);
			    else {
				target = which_armor(mtmp, W_ARM);
				if (target)
				    (void) rust_dmg(target, "armor", 1, TRUE, mtmp);
				else {
				    target = which_armor(mtmp, W_ARMU);
				    (void) rust_dmg(target, "shirt", 1, TRUE, mtmp);
				}
			    }
			}
			if (mptr == &mons[PM_IRON_GOLEM]) {
				if (in_sight)
				    pline("%s falls to pieces!", Monnam(mtmp));
				else if(mtmp->mtame)
				    pline("May %s rust in peace.",
								mon_nam(mtmp));
				mondied(mtmp);
				if (mtmp->mhp <= 0)
					trapkilled = TRUE;
			} else if (mptr == &mons[PM_FLAMING_SPHERE] || mptr == &mons[PM_SUMMONED_FLAMING_SPHERE]) {
				if (in_sight)
				    pline("%s is extinguished!", Monnam(mtmp));
				mondied(mtmp);
				if (mtmp->mhp <= 0)
					trapkilled = TRUE;
			} else if (splittinggremlin(mtmp->data) && rn2(3)) {
				(void)split_mon(mtmp, (struct monst *)0);
			}
			if (!rn2(50) && !(mtmp && mtmp->mtame)) deltrap(trap);
			break;
		    }
		case FIRE_TRAP:
 mfiretrap:
		    if (!(mtmp->mtame) && rn2(5)) break;

			if (in_sight)
			    pline("A %s erupts from the %s under %s!",
				  tower_of_flame,
				  surface(mtmp->mx,mtmp->my), mon_nam(mtmp));
			else if (see_it)  /* evidently `mtmp' is invisible */
			    You("see a %s erupt from the %s!",
				tower_of_flame, surface(mtmp->mx,mtmp->my));
			/*if (Slimed) {
				pline("The slime that covers you is burned away!");
				Slimed = 0;
			}*/
			/* commented out - it fired whenever a monster wandered into a fire trap on the level! --Amy */

			if (resists_fire(mtmp)) {
			    if (in_sight) {
				shieldeff(mtmp->mx,mtmp->my);
				pline("%s is uninjured.", Monnam(mtmp));
			    }
			} else {
			    int num = d(2,4), alt;
			    boolean immolate = FALSE;

			    /* paper burns very fast, assume straw is tightly
			     * packed and burns a bit slower */
			    switch (monsndx(mtmp->data)) {
			    case PM_PAPER_GOLEM:   immolate = TRUE;
						   alt = mtmp->mhpmax; break;
			    case PM_STRAW_GOLEM:   alt = mtmp->mhpmax / 2; break;
			    case PM_WOOD_GOLEM:    alt = mtmp->mhpmax / 4; break;
			    case PM_LEATHER_GOLEM: alt = mtmp->mhpmax / 8; break;
			    default: alt = 0; break;
			    }
			    if (alt > num) num = alt;

			    if (thitm(0, mtmp, (struct obj *)0, num, immolate))
				trapkilled = TRUE;
			    else
				/* we know mhp is at least `num' below mhpmax,
				   so no (mhp > mhpmax) check is needed here */
				mtmp->mhpmax -= rn2(num + 1);
			}
			if (!rn2(33)) 
			    (void) destroy_mitem(mtmp, SCROLL_CLASS, AD_FIRE);
			if (!rn2(33)) 
			    (void) destroy_mitem(mtmp, SPBOOK_CLASS, AD_FIRE);
			if (!rn2(33)) 
			    (void) destroy_mitem(mtmp, POTION_CLASS, AD_FIRE);

			if (burn_floor_paper(mtmp->mx, mtmp->my, see_it, FALSE) &&
				!see_it && distu(mtmp->mx, mtmp->my) <= 3*3)
			    You("smell smoke.");
			if (is_ice(mtmp->mx,mtmp->my))
			    melt_ice(mtmp->mx,mtmp->my);
			if (see_it) seetrap(trap);
			if (!rn2(50) && !(mtmp && mtmp->mtame)) deltrap(trap);
			break;

		case PLASMA_TRAP:
		    if (!(mtmp->mtame) && rn2(15)) break;
			if (in_sight)
			    pline("A plasma cloud encloses %s!", mon_nam(mtmp));
			else if (see_it)  /* evidently `mtmp' is invisible */
			    You("see a plasma cloud around %s!", surface(mtmp->mx,mtmp->my));

			if (resists_fire(mtmp)) {
			    if (in_sight) {
				shieldeff(mtmp->mx,mtmp->my);
				pline("%s is uninjured.", Monnam(mtmp));
			    }
			} else {
			    int num = d(4,4), alt;
			    boolean immolate = FALSE;

			    /* paper burns very fast, assume straw is tightly
			     * packed and burns a bit slower */
			    switch (monsndx(mtmp->data)) {
			    case PM_PAPER_GOLEM:   immolate = TRUE;
						   alt = mtmp->mhpmax; break;
			    case PM_STRAW_GOLEM:   alt = mtmp->mhpmax / 2; break;
			    case PM_WOOD_GOLEM:    alt = mtmp->mhpmax / 4; break;
			    case PM_LEATHER_GOLEM: alt = mtmp->mhpmax / 8; break;
			    default: alt = 0; break;
			    }
			    if (alt > num) num = alt;

			    if (thitm(0, mtmp, (struct obj *)0, num, immolate))
				trapkilled = TRUE;
			    else
				/* we know mhp is at least `num' below mhpmax,
				   so no (mhp > mhpmax) check is needed here */
				mtmp->mhpmax -= rn2(num + 1);
			}
			if (!rn2(5)) 
			    (void) destroy_mitem(mtmp, SCROLL_CLASS, AD_FIRE);
			if (!rn2(5)) 
			    (void) destroy_mitem(mtmp, SPBOOK_CLASS, AD_FIRE);
			if (!rn2(5)) 
			    (void) destroy_mitem(mtmp, POTION_CLASS, AD_FIRE);

			if (burn_floor_paper(mtmp->mx, mtmp->my, see_it, FALSE) &&
				!see_it && distu(mtmp->mx, mtmp->my) <= 3*3)
			    You("smell smoke.");
			if (is_ice(mtmp->mx,mtmp->my))
			    melt_ice(mtmp->mx,mtmp->my);
			if (see_it) seetrap(trap);
			if (!rn2(50) && !(mtmp && mtmp->mtame)) deltrap(trap);
			break;

           case ICE_TRAP:
		    if (!(mtmp->mtame) && rn2(5)) break;
           if (in_sight)
             pline("A freezing cloud shoots from the %s under %s!",
             surface(mtmp->mx, mtmp->my), mon_nam(mtmp));
           else if (see_it)
             You("see a freezing cloud shoot from the %s!", surface(mtmp->mx, mtmp->my));
             if (resists_cold(mtmp)) {
               if (in_sight) {
               shieldeff(mtmp->mx,mtmp->my);
               pline("%s is uninjured.", Monnam(mtmp));
               }
             } else {
               int num = d(2,4);
               if (thitm(0, mtmp, (struct obj *)0, num, FALSE))
                 trapkilled = TRUE;
               else if (!rn2(Race_if(PM_GAVIL) ? 6 : Race_if(PM_HYPOTHERMIC) ? 6 : 33))
                (void) destroy_mitem(mtmp, POTION_CLASS, AD_COLD);
               }
           if (see_it) seetrap(trap);
		if (!rn2(50) && !(mtmp && mtmp->mtame)) deltrap(trap);
           break;

           case SHOCK_TRAP:
		    if (!(mtmp->mtame) && rn2(5)) break;
           if (in_sight)
             pline("A lightning bolt strikes %s!", mon_nam(mtmp));
           else if (see_it)
             You("see a flashing lightning bolt!");
             if (resists_elec(mtmp)) {
               if (in_sight) {
               shieldeff(mtmp->mx,mtmp->my);
               pline("%s is uninjured.", Monnam(mtmp));
               }
             } else {
               int num = d(2,4);
               if (thitm(0, mtmp, (struct obj *)0, num, FALSE))
                 trapkilled = TRUE;
               else if (!rn2(33)) {
                (void) destroy_mitem(mtmp, WAND_CLASS, AD_ELEC);
                (void) destroy_mitem(mtmp, RING_CLASS, AD_ELEC);
			}
               }
           if (see_it) seetrap(trap);
		if (!rn2(50) && !(mtmp && mtmp->mtame)) deltrap(trap);
           break;

           case ACID_POOL:
		    if (!(mtmp->mtame) && rn2(10)) break;

		if (is_floater(mtmp->data) || is_flyer(mtmp->data) || mtmp->egotype_flying) break;

           if (in_sight)
             pline("%s is sprayed with acid!", Monnam(mtmp));
           else if (see_it)
             You("see a spray of acid!");
             if (resists_acid(mtmp)) {
               if (in_sight) {
               shieldeff(mtmp->mx,mtmp->my);
               pline("%s is uninjured.", Monnam(mtmp));
               }
             } else {
               int num = d(2,4);
               if (thitm(0, mtmp, (struct obj *)0, num, FALSE))
                 trapkilled = TRUE;
		}
           if (see_it) seetrap(trap);
		if (!rn2(50) && !(mtmp && mtmp->mtame)) deltrap(trap);
           break;

           case DEATH_TRAP:
		    if (!(mtmp->mtame) && rn2(25)) break;

		if (nonliving(mtmp->data) || is_demon(mtmp->data) || resists_death(mtmp) || mtmp->data->msound == MS_NEMESIS || resists_magm(mtmp)) break;

		else {

               if (in_sight) pline("%s is lit by a black glow...", Monnam(mtmp));

               int num = d(5,4);
               if (thitm(0, mtmp, (struct obj *)0, num, FALSE))
                 trapkilled = TRUE;

		}

		if (see_it) seetrap(trap);
		if (!rn2(50) && !(mtmp && mtmp->mtame)) deltrap(trap);

		break;

           case DISINTEGRATION_TRAP:
		    if (!(mtmp->mtame) && rn2(25)) break;

		if (resists_disint(mtmp)) break;

		else {

               if (in_sight) pline("%s is shaken heavily!", Monnam(mtmp));

               int num = d(4,4);
               if (thitm(0, mtmp, (struct obj *)0, num, FALSE))
                 trapkilled = TRUE;

		}

		if (see_it) seetrap(trap);
		if (!rn2(50) && !(mtmp && mtmp->mtame)) deltrap(trap);

		break;

           case DRAIN_TRAP:
		    if (!(mtmp->mtame) && rn2(25)) break;

		if (resists_drli(mtmp)) break;

		else {

               if (in_sight) pline("%s looks less powerful!", Monnam(mtmp));

               int num = d(3,4);
               if (thitm(0, mtmp, (struct obj *)0, num, FALSE))
                 trapkilled = TRUE;

		}

		if (see_it) seetrap(trap);
		if (!rn2(50) && !(mtmp && mtmp->mtame)) deltrap(trap);

		break;

           case WATER_POOL:
		    if (!(mtmp->mtame) && rn2(10)) break;

		if (is_floater(mtmp->data) || is_flyer(mtmp->data) || mtmp->egotype_flying || mtmp->egotype_watersplasher || is_swimmer(mtmp->data) || amphibious(mtmp->data) || breathless(mtmp->data) || mtmp->egotype_undead ) break;

           if (in_sight)
             pline("%s falls into a pool of water!", Monnam(mtmp));
           else if (see_it)
             You("see a splash of water!");

               int num = d(2,2);
               if (thitm(0, mtmp, (struct obj *)0, num, FALSE))
                 trapkilled = TRUE;

           if (see_it) seetrap(trap);
		if (!rn2(50) && !(mtmp && mtmp->mtame)) deltrap(trap);
           break;

           case SHIT_TRAP:

		if( (is_flyer(mptr) || mtmp->egotype_flying || (uarmf && itemhasappearance(uarmf, APP_TREADED_HEELS) && mtmp->data->mlet == S_DOG) ) && (mptr != &mons[PM_ARABELLA]) && (mptr != &mons[PM_ARABELLA_THE_TUFTED_ASIAN_GIRL]) && (mptr != &mons[PM_ANASTASIA_THE_SWEET_BLONDE]) && (mptr != &mons[PM_HENRIETTA_THE_BURLY_WOMAN]) && (mptr != &mons[PM_KATRIN_THE_VIOLET_BEAUTY]) && (mptr != &mons[PM_JANA_THE_SOFT_WENCH]) && (mptr != &mons[PM_ANASTASIA]) && (mptr != &mons[PM_HENRIETTA]) && (mptr != &mons[PM_KATRIN]) && (mptr != &mons[PM_JANA]) & (mptr != &mons[PM_TUFTED_ASIAN_GIRL]) && (mptr != &mons[PM_SWEET_BLONDE]) && (mptr != &mons[PM_BURLY_WOMAN]) && (mptr != &mons[PM_VIOLET_BEAUTY]) && (mptr != &mons[PM_SOFT_WENCH]) && (mptr != &mons[PM_ARABELLA_THE_MONEY_THIEF]) ) break; /* since this is a ground-based trap */
		/* The spacewars fighter nemeses somehow have affinity to this type of trap. */

           if (in_sight)
             pline("%s steps into a heap of shit!", mon_nam(mtmp));
           else if (see_it)
             You("see %s stepping into a heap of shit!", surface(mtmp->mx, mtmp->my));

			mon_adjust_speed(mtmp, 1, (struct obj *)0);

           if (see_it) seetrap(trap);
		if (!rn2(20) && !(mtmp && mtmp->mtame)) deltrap(trap);
           break;

		case PIT:
		case SPIKED_PIT:
		case GIANT_CHASM:
		case SHIT_PIT:
		case MANA_PIT:
		case ANOXIC_PIT:
		case HYPOXIC_PIT:
		case ACID_PIT:
			fallverb = "falls";
			if (is_flyer(mptr) || mtmp->egotype_flying || is_floater(mptr) ||
				(mtmp->wormno && count_wsegs(mtmp) > 5) ||
				is_clinger(mptr)) {
			    if (!inescapable) break;	/* avoids trap */
			    fallverb = "is dragged";	/* sokoban pit */
			}
			if (!passes_walls(mptr) && (!mtmp->egotype_wallwalk) )
			    mtmp->mtrapped = 1;
			if(in_sight) {
			    pline("%s %s into %s pit!",
				  Monnam(mtmp), fallverb,
				  a_your[trap->madeby_u]);
			    if (mptr == &mons[PM_PIT_VIPER] || mptr == &mons[PM_PIT_FIEND])
				pline("How pitiful.  Isn't that the pits?");
			    seetrap(trap);
			}
			if (tt == SHIT_PIT) mon_adjust_speed(mtmp, 1, (struct obj *)0);

			mselftouch(mtmp, "Falling, ", FALSE);
			if (mtmp->mhp <= 0 ||
				thitm(0, mtmp, (struct obj *)0,
				      rnd((tt == PIT) ? 6 : (tt == GIANT_CHASM) ? 15 : 10), FALSE))
			    trapkilled = TRUE;
			break;
		case HOLE:
		case TRAPDOOR:
		case SHAFT_TRAP:
		case CURRENT_SHAFT:
			if (!Can_fall_thru(&u.uz)) {
			 impossible("mintrap: %ss cannot exist on this level.",
				    defsyms[trap_to_defsym(tt)].explanation);
			    deltrap(trap);
			    break;	/* don't activate it after all */
			}
			if (is_flyer(mptr) || mtmp->egotype_flying || is_floater(mptr) ||
				mptr == &mons[PM_WUMPUS] ||
				(mtmp->wormno && count_wsegs(mtmp) > 5) ||
				mptr->msize >= MZ_HUGE) {
			    if (inescapable) {	/* sokoban hole */
				if (in_sight) {
				    pline("%s seems to be yanked down!",
					  Monnam(mtmp));
				    /* suppress message in mlevel_tele_trap() */
				    in_sight = FALSE;
				    seetrap(trap);
				}
			    } else
				break;
			}
			{
			    int mlev_res;
			    mlev_res = mlevel_tele_trap(mtmp, trap,
							inescapable, in_sight);
			    if (mlev_res) return(mlev_res);
			}
			break;
			/* used to be a fallthrough but we don't want that anymore --Amy */
		case LEVEL_TELEP:
		case LEVEL_BEAMER:

			if (mtmp->isshk || mtmp->ispriest) break; /* we don't want shks to become angry through no fault of your own, unless you have e.g. a wakeup call trap in effect --Amy */

			{
			    int mlev_res;
			    mlev_res = mlevel_tele_trap(mtmp, trap, inescapable, in_sight);
			    if (mlev_res) return(mlev_res);
			}
			break;

		case BRANCH_TELEPORTER:
		case BRANCH_BEAMER:

			if (mtmp->isshk || mtmp->ispriest) break; /* we don't want shks to become angry through no fault of your own, unless you have e.g. a wakeup call trap in effect --Amy */

			{
				if (in_sight) {
					pline("%s triggers a branchporting trap!", mon_nam(mtmp));
					seetrap(trap);
				}

				if (rn2(20)) {

				    int mlev_res;
				    mlev_res = mlevel_tele_trap(mtmp, trap, inescapable, in_sight);
				    if (mlev_res) return(mlev_res);
				} else {
					d_level flev;

					if (mon_has_amulet(mtmp) || In_endgame(&u.uz)) break;
					flev = random_branchport_level();
					migrate_to_level(mtmp, ledger_no(&flev), MIGR_RANDOM, (coord *)0);

				}
			}
			break;

		case S_PRESSING_TRAP:

			if (!mtmp->mpeaceful && !mtmp->mtame && !mtmp->spressingseen && !canseemon(mtmp)) {
				pline("%s was so stupid and stepped into your s-pressing trap!", noit_Monnam(mtmp));
				if (thitm(0, mtmp, (struct obj *)0, trap->launch_otyp, FALSE))
					trapkilled = TRUE;
				deltrap(trap); /* only triggers once */
			}

			break;

		case MAGIC_PORTAL: /* no longer allows players to simply get rid of them --Amy */
		case RELOCATION_TRAP:

			if (rn2(2)) break; /* don't trigger too often */
			if (mtmp->isshk || mtmp->ispriest) break; /* we don't want shks to become angry through no fault of your own, unless you have e.g. a wakeup call trap in effect --Amy */
			if (in_sight) {
				pline("%s seems very disoriented!", mon_nam(mtmp));
				seetrap(trap);
			}

			rloc(mtmp, FALSE);
			break;

		case TELEP_TRAP:
		case BEAMER_TRAP:

			if (mtmp->isshk || mtmp->ispriest) break; /* we don't want shks to become angry through no fault of your own, unless you have e.g. a wakeup call trap in effect --Amy */

			mtele_trap(mtmp, trap, in_sight);
			break;

		case WEB:
			/* Monster in a web. */
			if (webmaker(mptr) || dmgtype(mptr, AD_WEBS) || mtmp->egotype_webber) break;
			if (amorphous(mptr) || is_whirly(mptr) || unsolid(mptr)){
			    if(acidic(mptr) ||
			       mptr == &mons[PM_GELATINOUS_CUBE] ||
			       mptr == &mons[PM_FIRE_ELEMENTAL]) {
				if (in_sight)
				    pline("%s %s %s spider web!",
					  Monnam(mtmp),
					  (mptr == &mons[PM_FIRE_ELEMENTAL]) ?
					    "burns" : "dissolves",
					  a_your[trap->madeby_u]);
				deltrap(trap);
				newsym(mtmp->mx, mtmp->my);
				break;
			    }
			    if (in_sight) {
				pline("%s flows through %s spider web.",
				      Monnam(mtmp),
				      a_your[trap->madeby_u]);
				seetrap(trap);
			    }
			    break;
			}
			tear_web = FALSE;
			switch (monsndx(mptr)) {
			    case PM_OWLBEAR: /* Eric Backus */
			    case PM_BUGBEAR:
				if (!in_sight) {
				    You_hear("the roaring of a confused bear!");
				    mtmp->mtrapped = 1;
				    break;
				}
				/* fall though */
			    default:
				if (mptr->mlet == S_GIANT ||
				    (mptr->mlet == S_DRAGON &&
					extra_nasty(mptr)) || /* excl. babies */
				    (mtmp->wormno && count_wsegs(mtmp) > 5)) {
				    tear_web = TRUE;
				} else if (in_sight) {
				    pline("%s is caught in %s spider web.",
					  Monnam(mtmp),
					  a_your[trap->madeby_u]);
				    seetrap(trap);
				}
				mtmp->mtrapped = tear_web ? 0 : 1;
				break;
			    /* this list is fairly arbitrary; it deliberately
			       excludes wumpus & giant/ettin zombies/mummies */
			    case PM_TITANOTHERE:
			    case PM_BALUCHITHERIUM:
			    case PM_PURPLE_WORM:
			    case PM_JABBERWOCK:
			    case PM_IRON_GOLEM:
			    case PM_BALROG:
			    case PM_KRAKEN:
			    case PM_MASTODON:
				tear_web = TRUE;
				break;
			}
			if (tear_web) {
			    if (in_sight)
				pline("%s tears through %s spider web!",
				      Monnam(mtmp), a_your[trap->madeby_u]);
			    deltrap(trap);
			    newsym(mtmp->mx, mtmp->my);
			}
			break;

		case STATUE_TRAP:
		case ANIMATION_TRAP:
		case GLYPH_OF_WARDING:

		case SUPERSCROLLER_TRAP:
		case NUPESELL_TRAP:
		case ACTIVE_SUPERSCROLLER_TRAP:
		case RMB_LOSS_TRAP:
		case DISPLAY_TRAP:
		case SPELL_LOSS_TRAP:
		case YELLOW_SPELL_TRAP:
		case AUTO_DESTRUCT_TRAP:
		case MEMORY_TRAP:
		case INVENTORY_TRAP:
		case BLACK_NG_WALL_TRAP:
		case UNKNOWN_TRAP:
		case TRAP_PERCENTS:
		case MENU_TRAP:
		case SPEED_TRAP:
		case SWARM_TRAP:
		case HEEL_TRAP:
		case VULN_TRAP:
		case AUTOMATIC_SWITCHER:

		case LAVA_TRAP:
		case FLOOD_TRAP:
		case FREE_HAND_TRAP:
		case UNIDENTIFY_TRAP:
		case THIRST_TRAP:
		case LUCK_TRAP:
		case SHADES_OF_GREY_TRAP:
		case ITEM_TELEP_TRAP:
		case GRAVITY_TRAP:
		case STONE_TO_FLESH_TRAP:
		case QUICKSAND_TRAP:
		case FAINT_TRAP:
		case CURSE_TRAP:
		case DIFFICULTY_TRAP:
		case SOUND_TRAP:
		case CASTER_TRAP:
		case WEAKNESS_TRAP:
		case ROT_THIRTEEN_TRAP:
		case BISHOP_TRAP:
		case STAIRS_TRAP:
		case CONFUSION_TRAP:
		case DROP_TRAP:
		case DSTW_TRAP:
		case STATUS_TRAP:
		case ALIGNMENT_TRAP:
		case UNINFORMATION_TRAP:
		case TIMERUN_TRAP:
		case MEAN_BURDEN_TRAP:
		case CARRCAP_TRAP:
		case UMENG_TRAP:
		case AEFDE_TRAP:
		case EPVI_TRAP:
		case FUCKFUCKFUCK_TRAP:
		case OPTION_TRAP:
		case MISCOLOR_TRAP:
		case ONE_RAINBOW_TRAP:
		case COLORSHIFT_TRAP:
		case TOP_LINE_TRAP:
		case CAPS_TRAP:
		case UN_KNOWLEDGE_TRAP:
		case DARKHANCE_TRAP:
		case DSCHUEUEUET_TRAP:
		case NOPESKILL_TRAP:
		case REAL_LIE_TRAP:
		case ESCAPE_PAST_TRAP:
		case PETHATE_TRAP:
		case PET_LASHOUT_TRAP:
		case PETSTARVE_TRAP:
		case PETSCREW_TRAP:
		case TECH_LOSS_TRAP:
		case PROOFLOSS_TRAP:
		case UN_INVIS_TRAP:
		case DETECTATION_TRAP:
		case REPEATING_NASTYCURSE_TRAP:
		case CALLING_OUT_TRAP:
		case FIELD_BREAK_TRAP:
		case TENTH_TRAP:
		case DEBT_TRAP:
		case INVERSION_TRAP:
		case WINCE_TRAP:
		case FUCK_OVER_TRAP:
		case U_HAVE_BEEN_TRAP:

		case PERSISTENT_FART_TRAP:
		case ATTACKING_HEEL_TRAP:
		case TRAP_TELEPORTER:
		case ALIGNMENT_TRASH_TRAP:
		case RESHUFFLE_TRAP:
		case MUSEHAND_TRAP:
		case DOGSIDE_TRAP:
		case BANKRUPT_TRAP:
		case FILLUP_TRAP:
		case AIRSTRIKE_TRAP:
		case DYNAMITE_TRAP:
		case MALEVOLENCE_TRAP:
		case LEAFLET_TRAP:
		case TENTADEEP_TRAP:
		case STATHALF_TRAP:
		case CUTSTAT_TRAP:
		case RARE_SPAWN_TRAP:
		case YOU_ARE_AN_IDIOT_TRAP:
		case NASTYCURSE_TRAP:

		case REALLY_BAD_TRAP:
		case COVID_TRAP:
		case ARTIBLAST_TRAP:
		case GIANT_EXPLORER_TRAP:
		case TRAPWARP_TRAP:
		case YAWM_TRAP:
		case CRADLE_OF_CHAOS_TRAP:
		case TEZCATLIPOCA_TRAP:
		case ENTHUMESIS_TRAP:
		case MIKRAANESIS_TRAP:
		case GOTS_TOO_GOOD_TRAP:
		case KILLER_ROOM_TRAP:
		case NO_FUN_WALLS_TRAP:
		case BAD_PART_TRAP:
		case COMPLETELY_BAD_PART_TRAP:
		case EVIL_VARIANT_TRAP:
		case INTRINSIC_LOSS_TRAP:
		case BLOOD_LOSS_TRAP:
		case BAD_EFFECT_TRAP:
		case MULTIPLY_TRAP:
		case AUTO_VULN_TRAP:
		case TELE_ITEMS_TRAP:
		case NASTINESS_TRAP:
		case LOOTCUT_TRAP:
		case MONSTER_SPEED_TRAP:
		case SCALING_TRAP:
		case ENMITY_TRAP:
		case WHITE_SPELL_TRAP:
		case COMPLETE_GRAY_SPELL_TRAP:
		case QUASAR_TRAP:
		case MOMMA_TRAP:
		case HORROR_TRAP:
		case ARTIFICER_TRAP:
		case WEREFORM_TRAP:
		case NON_PRAYER_TRAP:
		case EVIL_PATCH_TRAP:
		case HARD_MODE_TRAP:
		case SECRET_ATTACK_TRAP:
		case EATER_TRAP:
		case COVETOUSNESS_TRAP:
		case NOT_SEEN_TRAP:
		case DARK_MODE_TRAP:
		case ANTISEARCH_TRAP:
		case HOMICIDE_TRAP:
		case NASTY_NATION_TRAP:
		case WAKEUP_CALL_TRAP:
		case GRAYOUT_TRAP:
		case GRAY_CENTER_TRAP:
		case CHECKERBOARD_TRAP:
		case CLOCKWISE_SPIN_TRAP:
		case COUNTERCLOCKWISE_SPIN_TRAP:
		case LAG_TRAP:
		case BLESSCURSE_TRAP:
		case DE_LIGHT_TRAP:
		case DISCHARGE_TRAP:
		case TRASHING_TRAP:
		case FILTERING_TRAP:
		case DEFORMATTING_TRAP:
		case FLICKER_STRIP_TRAP:
		case UNDRESSING_TRAP:
		case HYPERBLUEWALL_TRAP:
		case NOLITE_TRAP:
		case PARANOIA_TRAP:
		case FLEECESCRIPT_TRAP:
		case INTERRUPT_TRAP:
		case DUSTBIN_TRAP:
		case MANA_BATTERY_TRAP:
		case MONSTERFINGERS_TRAP:
		case MISCAST_TRAP:
		case MESSAGE_SUPPRESSION_TRAP:
		case STUCK_ANNOUNCEMENT_TRAP:
		case BLOODTHIRSTY_TRAP:
		case MAXIMUM_DAMAGE_TRAP:
		case LATENCY_TRAP:
		case STARLIT_TRAP:
		case KNOWLEDGE_TRAP:
		case HIGHSCORE_TRAP:
		case PINK_SPELL_TRAP:
		case GREEN_SPELL_TRAP:
		case EVC_TRAP:
		case UNDERLAYER_TRAP:
		case DAMAGE_METER_TRAP:
		case ARBITRARY_WEIGHT_TRAP:
		case FUCKED_INFO_TRAP:
		case BLACK_SPELL_TRAP:
		case CYAN_SPELL_TRAP:
		case HEAP_TRAP:
		case BLUE_SPELL_TRAP:
		case TRON_TRAP:
		case RED_SPELL_TRAP:
		case TOO_HEAVY_TRAP:
		case ELONGATION_TRAP:
		case WRAPOVER_TRAP:
		case DESTRUCTION_TRAP:
		case MELEE_PREFIX_TRAP:
		case AUTOMORE_TRAP:
		case UNFAIR_ATTACK_TRAP:

		case ORANGE_SPELL_TRAP:
		case VIOLET_SPELL_TRAP:
		case TRAP_OF_LONGING:
		case CURSED_PART_TRAP:
		case QUAVERSAL_TRAP:
		case APPEARANCE_SHUFFLING_TRAP:
		case BROWN_SPELL_TRAP:
		case CHOICELESS_TRAP:
		case GOLDSPELL_TRAP:
		case DEPROVEMENT_TRAP:
		case INITIALIZATION_TRAP:
		case GUSHLUSH_TRAP:
		case SOILTYPE_TRAP:
		case DANGEROUS_TERRAIN_TRAP:
		case FALLOUT_TRAP:
		case MOJIBAKE_TRAP:
		case GRAVATION_TRAP:
		case UNCALLED_TRAP:
		case EXPLODING_DICE_TRAP:
		case PERMACURSE_TRAP:
		case SHROUDED_IDENTITY_TRAP:
		case FEELER_GAUGES_TRAP:
		case LONG_SCREWUP_TRAP:
		case WING_YELLOW_CHANGER:
		case LIFE_SAVING_TRAP:
		case CURSEUSE_TRAP:
		case CUT_NUTRITION_TRAP:
		case SKILL_LOSS_TRAP:
		case AUTOPILOT_TRAP:
		case FORCE_TRAP:
		case MONSTER_GLYPH_TRAP:
		case CHANGING_DIRECTIVE_TRAP:
		case CONTAINER_KABOOM_TRAP:
		case STEAL_DEGRADE_TRAP:
		case LEFT_INVENTORY_TRAP:
		case FLUCTUATING_SPEED_TRAP:
		case TARMUSTROKINGNORA_TRAP:
		case FAILURE_TRAP:
		case BRIGHT_CYAN_SPELL_TRAP:
		case FREQUENTATION_SPAWN_TRAP:
		case PET_AI_TRAP:
		case SATAN_TRAP:
		case REMEMBERANCE_TRAP:
		case POKELIE_TRAP:
		case AUTOPICKUP_TRAP:
		case DYWYPI_TRAP:
		case SILVER_SPELL_TRAP:
		case METAL_SPELL_TRAP:
		case PLATINUM_SPELL_TRAP:
		case MANLER_TRAP:
		case DOORNING_TRAP:
		case NOWNSIBLE_TRAP:
		case ELM_STREET_TRAP:
		case MONNOISE_TRAP:
		case RANG_CALL_TRAP:
		case RECURRING_SPELL_LOSS_TRAP:
		case ANTITRAINING_TRAP:
		case TECHOUT_TRAP:
		case STAT_DECAY_TRAP:
		case MOVEMORK_TRAP:

		case NEXUS_TRAP:
		case LEG_TRAP:
		case ARTIFACT_JACKPOT_TRAP:
		case MAP_AMNESIA_TRAP:
		case SPREADING_TRAP:
		case ADJACENT_TRAP:
		case SUPERTHING_TRAP:
		case LEVITATION_TRAP:
		case BOWEL_CRAMPS_TRAP:
		case UNEQUIPPING_TRAP:
		case GOOD_ARTIFACT_TRAP:
		case GENDER_TRAP:
		case TRAP_OF_OPPOSITE_ALIGNMENT:
		case SINCOUNT_TRAP:
		case WRENCHING_TRAP:
		case TRACKER_TRAP:
		case NURSE_TRAP:
		case BACK_TO_START_TRAP:
		case NEMESIS_TRAP:
		case STREW_TRAP:
		case OUTTA_DEPTH_TRAP:
		case PUNISHMENT_TRAP:
		case BOON_TRAP:
		case FOUNTAIN_TRAP:
		case THRONE_TRAP:
		case FILLER_TRAP:
		case TOXIC_VENOM_TRAP:
		case INSANITY_TRAP:
		case MADNESS_TRAP:

		case HYBRID_TRAP:
		case SHAPECHANGE_TRAP:
		case MIGUC_TRAP:
		case DIRECTIVE_TRAP:
		case SATATUE_TRAP:
		case FARTING_WEB:
		case CATACLYSM_TRAP:
		case DATA_DELETE_TRAP:
		case ELDER_TENTACLING_TRAP:
		case FOOTERER_TRAP:

		case GRAVE_WALL_TRAP:
		case TUNNEL_TRAP:
		case FARMLAND_TRAP:
		case MOUNTAIN_TRAP:
		case WATER_TUNNEL_TRAP:
		case CRYSTAL_FLOOD_TRAP:
		case MOORLAND_TRAP:
		case URINE_TRAP:
		case SHIFTING_SAND_TRAP:
		case STYX_TRAP:
		case PENTAGRAM_TRAP:
		case SNOW_TRAP:
		case ASH_TRAP:
		case SAND_TRAP:
		case PAVEMENT_TRAP:
		case HIGHWAY_TRAP:
		case GRASSLAND_TRAP:
		case NETHER_MIST_TRAP:
		case STALACTITE_TRAP:
		case CRYPTFLOOR_TRAP:
		case BUBBLE_TRAP:
		case RAIN_CLOUD_TRAP:

		case ITEM_NASTIFICATION_TRAP:
		case SANITY_INCREASE_TRAP:
		case PSI_TRAP:
		case GAY_TRAP:

		case SANITY_TREBLE_TRAP:
		case STAT_DECREASE_TRAP:
		case SIMEOUT_TRAP:


		case EVIL_HEEL_TRAP:
		case BAD_EQUIPMENT_TRAP:
		case TEMPOCONFLICT_TRAP:
		case TEMPOHUNGER_TRAP:
		case TELEPORTITIS_TRAP:
		case POLYMORPHITIS_TRAP:
		case PREMATURE_DEATH_TRAP:
		case LASTING_AMNESIA_TRAP:
		case RAGNAROK_TRAP:
		case SINGLE_DISENCHANT_TRAP:

		case SEVERE_DISENCHANT_TRAP:
		case PAIN_TRAP:
		case TECHCAP_TRAP:
		case TREMBLING_TRAP:
		case SPELL_MEMORY_TRAP:
		case SKILL_REDUCTION_TRAP:
		case SKILLCAP_TRAP:
		case PERMANENT_STAT_DAMAGE_TRAP:

		case DISCONNECT_TRAP:
		case INTERFACE_SCREW_TRAP:
		case DIMNESS_TRAP:
		case EVIL_ARTIFACT_TRAP:
		case BOSSFIGHT_TRAP:
		case ENTIRE_LEVEL_TRAP:
		case BONES_TRAP:
		case RODNEY_TRAP:
		case AUTOCURSE_TRAP:
		case HIGHLEVEL_TRAP:
		case SPELL_FORGETTING_TRAP:
		case SOUND_EFFECT_TRAP:

		case PESTILENCE_TRAP:
		case FAMINE_TRAP:

		case RECURSION_TRAP:
		case RESPAWN_TRAP:
		case WARP_ZONE:
		case CAPTCHA_TRAP:
		case MIND_WIPE_TRAP:
		case LOCK_TRAP:
		case MAGIC_CANCELLATION_TRAP:
		case FARLOOK_TRAP:
		case GATEWAY_FROM_HELL:
		case GROWING_TRAP:
		case COOLING_TRAP:
		case BAR_TRAP:
		case LOCKING_TRAP:
		case AIR_TRAP:
		case TERRAIN_TRAP:

		case NEST_TRAP:
		case CYANIDE_TRAP:
		case LASER_TRAP:
		case FART_TRAP:
		case CONFUSE_TRAP:
		case STUN_TRAP:
		case HALLUCINATION_TRAP:
		case PETRIFICATION_TRAP:
		case NUMBNESS_TRAP:
		case FREEZING_TRAP:
		case BURNING_TRAP:
		case FEAR_TRAP:
		case BLINDNESS_TRAP:
		case GLIB_TRAP:
		case SLIME_TRAP:
		case INERTIA_TRAP:
		case TIME_TRAP:
		case LYCANTHROPY_TRAP:
		case UNLIGHT_TRAP:
		case ELEMENTAL_TRAP:
		case ESCALATING_TRAP:
		case NEGATIVE_TRAP:
		case MANA_TRAP:
		case SIN_TRAP:
		case DESTROY_ARMOR_TRAP:
		case DIVINE_ANGER_TRAP:
		case GENETIC_TRAP:
		case MISSINGNO_TRAP:
		case CANCELLATION_TRAP:
		case HOSTILITY_TRAP:
		case BOSS_TRAP:
		case WISHING_TRAP:
		case BOMB_TRAP:
		case EARTHQUAKE_TRAP:
		case GLUE_TRAP:
		case GUILLOTINE_TRAP:
		case BISECTION_TRAP:
		case VOLT_TRAP:
		case HORDE_TRAP:
		case IMMOBILITY_TRAP:
		case GREEN_GLYPH:
		case BLUE_GLYPH:
		case YELLOW_GLYPH:
		case ORANGE_GLYPH:
		case BLACK_GLYPH:
		case PURPLE_GLYPH:

		case RECURRING_AMNESIA_TRAP:
		case BIGSCRIPT_TRAP:
		case BANK_TRAP:
		case ONLY_TRAP:
		case MAP_TRAP:
		case TECH_TRAP:
		case DISENCHANT_TRAP:
		case VERISIERT:
		case CHAOS_TRAP:
		case MUTENESS_TRAP:
		case NTLL_TRAP:
		case ENGRAVING_TRAP:
		case MAGIC_DEVICE_TRAP:
		case BOOK_TRAP:
		case LEVEL_TRAP:
		case QUIZ_TRAP:

		case METABOLIC_TRAP:
		case TRAP_OF_NO_RETURN:
		case EGOTRAP:
		case FAST_FORWARD_TRAP:
		case TRAP_OF_ROTTENNESS:
		case UNSKILLED_TRAP:
		case LOW_STATS_TRAP:
		case TRAINING_TRAP:
		case EXERCISE_TRAP:
		case FALLING_LOADSTONE_TRAP:
		case SUMMON_UNDEAD_TRAP:
		case FALLING_NASTYSTONE_TRAP:

		case ELEMENTAL_PORTAL:
		case GIRLINESS_TRAP:
		case FUMBLING_TRAP:
		case EGOMONSTER_TRAP:
		case FLOODING_TRAP:
		case MONSTER_CUBE:
		case CURSED_GRAVE:
		case LIMITATION_TRAP:
		case WEAK_SIGHT_TRAP:
		case RANDOM_MESSAGE_TRAP:

		case DESECRATION_TRAP:
		case STARVATION_TRAP:
		case DROPLESS_TRAP:
		case LOW_EFFECT_TRAP:
		case INVISIBLE_TRAP:
		case GHOST_WORLD_TRAP:
		case DEHYDRATION_TRAP:
		case HATE_TRAP:
		case SPACEWARS_TRAP:
		case TEMPORARY_RECURSION_TRAP:
		case TOTTER_TRAP:
		case NONINTRINSICAL_TRAP:
		case DROPCURSE_TRAP:
		case NAKEDNESS_TRAP:
		case ANTILEVEL_TRAP:
		case VENTILATOR:
		case STEALER_TRAP:
		case REBELLION_TRAP:
		case CRAP_TRAP:
		case MISFIRE_TRAP:
		case TRAP_OF_WALLS:

		case SPINED_BALL_TRAP:
		case PENDULUM_TRAP:
		case TURN_TABLE:
		case SCENT_TRAP:
		case BANANA_TRAP:
		case FALLING_TUB_TRAP:
		case ALARM:
		case CALTROPS_TRAP:
		case BLADE_WIRE:
		case MAGNET_TRAP:
		case SLINGSHOT_TRAP:
		case CANNON_TRAP:
		case VENOM_SPRINKLER:
		case FUMAROLE:
		case KOP_CUBE:
		case BOSS_SPAWNER:
		case CONTAMINATION_TRAP:

			break;

		case WALL_TRAP:
		case MONSTER_GENERATOR:
		case POTION_DISPENSER:
		case SPACEWARS_SPAWN_TRAP:
		case TV_TROPES_TRAP:
		case SYMBIOTE_TRAP:
		case KILL_SYMBIOTE_TRAP:
		case SYMBIOTE_REPLACEMENT_TRAP:
		case SHUTDOWN_TRAP:
		case CORONA_TRAP:
		case UNPROOFING_TRAP:
		case VISIBILITY_TRAP:
		case FEMINISM_STONE_TRAP:
		case SHUEFT_TRAP:
		case MOTH_LARVAE_TRAP:
		case WORTHINESS_TRAP:
		case CONDUCT_TRAP:
		case STRIKETHROUGH_TRAP:
		case MULTIPLE_GATHER_TRAP:
		case VIVISECTION_TRAP:
		case INSTAFEMINISM_TRAP:
		case INSTANASTY_TRAP:
		case SKILL_POINT_LOSS_TRAP:
		case PERFECT_MATCH_TRAP:
		case DUMBIE_LIGHTSABER_TRAP:
		case WRONG_STAIRS:
		case TECHSTOP_TRAP:
		case AMNESIA_SWITCH_TRAP:
		case SKILL_SWAP_TRAP:
		case SKILL_UPORDOWN_TRAP:
		case SKILL_RANDOMIZE_TRAP:
 		case MACE_TRAP:
 		case DAGGER_TRAP:
 		case RAZOR_TRAP:
 		case PHOSGENE_TRAP:
 		case CHLOROFORM_TRAP:
 		case CORROSION_TRAP:
 		case FLAME_TRAP:
 		case WITHER_TRAP:
 		case PHASEPORTER:
 		case PHASE_BEAMER:
 		case VULNERATE_TRAP:
		case FALLING_ROCK_COLD:
		case RETURN_TRAP:
		case INTRINSIC_STEAL_TRAP:
		case SCORE_AXE_TRAP:
		case SCORE_DRAIN_TRAP:
		case SINGLE_UNIDENTIFY_TRAP:
		case UNLUCKY_TRAP:
		case ALIGNMENT_REDUCTION_TRAP:
		case MALIGNANT_TRAP:
		case STAT_DAMAGE_TRAP:
		case HALF_MEMORY_TRAP:
		case HALF_TRAINING_TRAP:
		case DEBUFF_TRAP:
		case TRIP_ONCE_TRAP:
		case NARCOLEPSY_TRAP:
		case MARTIAL_ARTS_TRAP:

		case SKILL_MULTIPLY_TRAP:
		case TRAPWALK_TRAP:
		case CLUSTER_TRAP:
		case FIELD_TRAP:
		case MONICIDE_TRAP:
		case TRAP_CREATION_TRAP:
		case LEOLD_TRAP:
		case BURDEN_TRAP:
		case MAGIC_VACUUM_TRAP:
		case ANIMEBAND_TRAP:
		case PERFUME_TRAP:
		case COURT_TRAP:
		case ELDER_SCROLLS_TRAP:
		case JOKE_TRAP:
		case DUNGEON_LORDS_TRAP:
		case FORTYTWO_TRAP:
		case RANDOMIZE_TRAP:
		case EVILROOM_TRAP:
		case AOE_TRAP:
		case ELONA_TRAP:
		case RELIGION_TRAP:
		case STEAMBAND_TRAP:
		case HARDCORE_TRAP:
		case MACHINE_TRAP:
		case BEE_TRAP:
		case MIGO_TRAP:
		case ANGBAND_TRAP:
		case DNETHACK_TRAP:
		case EVIL_SPAWN_TRAP:
		case SHOE_TRAP:
		case INSIDE_TRAP:
		case DOOM_TRAP:
		case MILITARY_TRAP:
		case ILLUSION_TRAP:
		case DIABLO_TRAP:

			break;

		case FEMMY_TRAP:
			if (FemtrapActiveRita && !FemaleTrapFemmy) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case MADELEINE_TRAP:
			if (FemtrapActiveRita && !FemaleTrapMadeleine) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case RUTH_TRAP:
			if (FemtrapActiveRita && !FemaleTrapRuth) {
				/* no message
				 * if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!"); */
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case MAGDALENA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapMagdalena) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case MARLEEN_TRAP:
			if (FemtrapActiveRita && !FemaleTrapMarleen) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case KLARA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapKlara) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case FRIEDERIKE_TRAP:
			if (FemtrapActiveRita && !FemaleTrapFriederike) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case NAOMI_TRAP:
			if (FemtrapActiveRita && !FemaleTrapNaomi) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case UTE_TRAP:
			if (FemtrapActiveRita && !FemaleTrapUte) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case JASIEEN_TRAP:
			if (FemtrapActiveRita && !FemaleTrapJasieen) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case YASAMAN_TRAP:
			if (FemtrapActiveRita && !FemaleTrapYasaman) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case MAY_BRITT_TRAP:
			if (FemtrapActiveRita && !FemaleTrapMayBritt) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case MARLENA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapMarlena) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case SABRINA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapSabrina) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case TANJA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapTanja) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case SONJA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapSonja) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case RHEA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapRhea) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case LARA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapLara) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case NADINE_TRAP:
			if (FemtrapActiveRita && !FemaleTrapNadine) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case LUISA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapLuisa) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case IRINA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapIrina) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case LISELOTTE_TRAP:
			if (FemtrapActiveRita && !FemaleTrapLiselotte) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case GRETA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapGreta) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case JANE_TRAP:
			if (FemtrapActiveRita && !FemaleTrapJane) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case SUE_LYN_TRAP:
			if (FemtrapActiveRita && !FemaleTrapSueLyn) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case CHARLOTTE_TRAP:
			if (FemtrapActiveRita && !FemaleTrapCharlotte) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case HANNAH_TRAP:
			if (FemtrapActiveRita && !FemaleTrapHannah) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case LITTLE_MARIE_TRAP:
			if (FemtrapActiveRita && !FemaleTrapLittleMarie) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case KSENIA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapKsenia) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case LYDIA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapLydia) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case CONNY_TRAP:
			if (FemtrapActiveRita && !FemaleTrapConny) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case KATIA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapKatia) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case MARIYA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapMariya) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case ELISE_TRAP:
			if (FemtrapActiveRita && !FemaleTrapElise) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case RONJA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapRonja) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case ARIANE_TRAP:
			if (FemtrapActiveRita && !FemaleTrapAriane) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case JOHANNA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapJohanna) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case INGE_TRAP:
			if (FemtrapActiveRita && !FemaleTrapInge) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case ROSA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapRosa) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case JANINA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapJanina) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case KRISTIN_TRAP:
			if (FemtrapActiveRita && !FemaleTrapKristin) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case ANNA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapAnna) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case RUEA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapRuea) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case DORA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapDora) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case MARIKE_TRAP:
			if (FemtrapActiveRita && !FemaleTrapMarike) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case JETTE_TRAP:
			if (FemtrapActiveRita && !FemaleTrapJette) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case INA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapIna) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case SING_TRAP:
			if (FemtrapActiveRita && !FemaleTrapSing) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case VICTORIA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapVictoria) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case MELISSA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapMelissa) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case ANITA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapAnita) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case HENRIETTA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapHenrietta) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case VERENA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapVerena) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case ARABELLA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapArabella) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case NELLY_TRAP:
			if (FemtrapActiveRita && !FemaleTrapNelly) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case EVELINE_TRAP:
			if (FemtrapActiveRita && !FemaleTrapEveline) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case KARIN_TRAP:
			if (FemtrapActiveRita && !FemaleTrapKarin) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case JUEN_TRAP:
			if (FemtrapActiveRita && !FemaleTrapJuen) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case KRISTINA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapKristina) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case ALMUT_TRAP:
			if (FemtrapActiveRita && !FemaleTrapAlmut) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case JULIETTA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapJulietta) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case LOU_TRAP:
			if (FemtrapActiveRita && !FemaleTrapLou) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case ANASTASIA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapAnastasia) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case JESSICA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapJessica) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case SOLVEJG_TRAP:
			if (FemtrapActiveRita && !FemaleTrapSolvejg) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case WENDY_TRAP:
			if (FemtrapActiveRita && !FemaleTrapWendy) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case KATHARINA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapKatharina) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case ELENA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapElena) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case THAI_TRAP:
			if (FemtrapActiveRita && !FemaleTrapThai) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case ELIF_TRAP:
			if (FemtrapActiveRita && !FemaleTrapElif) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case NADJA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapNadja) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case SANDRA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapSandra) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case NATALJE_TRAP:
			if (FemtrapActiveRita && !FemaleTrapNatalje) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case JEANETTA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapJeanetta) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case YVONNE_TRAP:
			if (FemtrapActiveRita && !FemaleTrapYvonne) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case MAURAH_TRAP:
			if (FemtrapActiveRita && !FemaleTrapMaurah) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case MELTEM_TRAP:
			if (FemtrapActiveRita && !FemaleTrapMeltem) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case ANNEMARIE_TRAP:
			if (FemtrapActiveRita && !FemaleTrapAnnemarie) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case JIL_TRAP:
			if (FemtrapActiveRita && !FemaleTrapJil) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case JANA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapJana) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case KATRIN_TRAP:
			if (FemtrapActiveRita && !FemaleTrapKatrin) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case GUDRUN_TRAP:
			if (FemtrapActiveRita && !FemaleTrapGudrun) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case ELLA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapElla) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case MANUELA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapManuela) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case JENNIFER_TRAP:
			if (FemtrapActiveRita && !FemaleTrapJennifer) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case PATRICIA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapPatricia) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case ANTJE_TRAP:
			if (FemtrapActiveRita && !FemaleTrapAntje) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case ANTJE_TRAP_X:
			if (FemtrapActiveRita && !FemaleTrapAntjeX) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case KERSTIN_TRAP:
			if (FemtrapActiveRita && !FemaleTrapKerstin) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case LAURA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapLaura) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case LARISSA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapLarissa) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case NORA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapNora) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case NATALIA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapNatalia) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case SUSANNE_TRAP:
			if (FemtrapActiveRita && !FemaleTrapSusanne) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case LISA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapLisa) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case BRIDGHITTE_TRAP:
			if (FemtrapActiveRita && !FemaleTrapBridghitte) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case JULIA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapJulia) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case NICOLE_TRAP:
			if (FemtrapActiveRita && !FemaleTrapNicole) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case RITA_TRAP: /* always triggered by monsters */
			if (!FemaleTrapRita) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case SARAH_TRAP:
			if (FemtrapActiveRita && !FemaleTrapSarah) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case CLAUDIA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapClaudia) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case LUDGERA_TRAP:
			if (FemtrapActiveRita && !FemaleTrapLudgera) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;
		case KATI_TRAP:
			if (FemtrapActiveRita && !FemaleTrapKati) {
				if (!FemtrapActiveRuth) pline("Haha, someone triggered a feminism trap and now YOU are affected by it! Hahaha!");
				dotrap(trap, DONTREVEAL|SKIPGARBAGE);
				u.cnd_ritatrapcnt++;
			}
			break;

		case PET_TRAP:

			if (mtmp->mtame) {
				if (in_sight) pline("%s triggers a trap and dies.", Monnam(mtmp));
				if (in_sight) seetrap(trap);
				if (!in_sight) You("sense the death of a companion.");
				mondead(mtmp);
				trapkilled = TRUE;
			}

			break;

		case LOUDSPEAKER:
			{
				pline("%s", fauxmessage());
				u.cnd_plineamount++;
				if (!rn2(3)) {
					pline("%s", fauxmessage());
					u.cnd_plineamount++;
				}
			}
			break;
		case ARABELLA_SPEAKER:
			{
				pline("%s", fauxmessage());
				u.cnd_plineamount++;
				if (!rn2(3)) {
					pline("%s", fauxmessage());
					u.cnd_plineamount++;
				}
			}
			break;
		case MAGIC_TRAP:

			if (mtmp->mcan) {
				deltrap(trap);
				newsym(mtmp->mx, mtmp->my);
				if (in_sight) pline("%s is caught in a magical explosion.", Monnam(mtmp));
				mtmp->mcan = 0;
				break; /* do not call deltrap twice! */
			}

			/* A magic trap.  Monsters usually immune. */
			if (!rn2(30)) {
			    deltrap(trap);
			    newsym(mtmp->mx, mtmp->my);
			    if (in_sight) 
				pline("%s is caught in a magical explosion.",
				    Monnam(mtmp));
			    if (thitm(0, mtmp, (struct obj *)0, rnd(10), FALSE))
				trapkilled = TRUE;
			    else {
			    	if (in_sight)
				    pline("%s looks filled with power.",
					    Monnam(mtmp));
			    	mtmp->m_en = (mtmp->m_enmax += 2);
			    }
			} else if (!rn2(21)) goto mfiretrap;
			break;
		case ANTI_MAGIC:
		case OUT_OF_MAGIC_TRAP:
		    if (!(mtmp->mtame) && rn2(10)) break;
			if (in_sight) seetrap(trap);
		    	if (resists_magm(mtmp)) {
			    if (in_sight) {
				shieldeff(mtmp->mx,mtmp->my);
				pline("%s is uninjured.", Monnam(mtmp));
			    }
		    	} else {
		    	    mon_drain_en(mtmp, 
				((mtmp->m_lev > 0) ? (rnd(mtmp->m_lev)) : 0) + 1);
		    	}
			if (!rn2(50) && !(mtmp && mtmp->mtame)) deltrap(trap);
			break;
		case LANDMINE:
		    {
			int xtradamage = 0;
			if (trap->launch_otyp == 33) xtradamage = rnd(30); /* for specific land mine artifact by Amy */

			if(rn2(3))
				break; /* monsters usually don't set it off */
			if(is_flyer(mptr) || mtmp->egotype_flying) {
				boolean already_seen = trap->tseen;
				if (in_sight && !already_seen) {
	pline("A trigger appears in a pile of soil below %s.", mon_nam(mtmp));
					seetrap(trap);
				}
				if (rn2(3)) break;
				if (in_sight) {
					newsym(mtmp->mx, mtmp->my);
					pline_The("air currents set %s off!",
					  already_seen ? "a land mine" : "it");
				}
			} else if(in_sight) {
			    newsym(mtmp->mx, mtmp->my);
			    pline("KAABLAMM!!!  %s triggers %s land mine!",
				Monnam(mtmp), a_your[trap->madeby_u]);
			}
			if (!in_sight)
				pline("Kaablamm!  You hear an explosion in the distance!");
			blow_up_landmine(trap);
			if (thitm(0, mtmp, (struct obj *)0, rnd(16) + xtradamage, FALSE))
				trapkilled = TRUE;
			else {
				/* monsters recursively fall into new pit */
				if (mintrap(mtmp) == 2) trapkilled=TRUE;
			}
			/* a boulder may fill the new pit, crushing monster */
			fill_pit(trap->tx, trap->ty);
			if (mtmp->mhp <= 0) trapkilled = TRUE;
			if (unconscious()) {
				multi = -1;
				nomovemsg="The explosion awakens you!";
			}
			break;
		    }
		case POLY_TRAP:

			if (mtmp->isshk || mtmp->ispriest) break; /* I simply decided to make them immune, sue me :P --Amy */

		    if (resists_magm(mtmp)) {
			shieldeff(mtmp->mx, mtmp->my);
		    } else if (!resist(mtmp, WAND_CLASS, 0, NOTELL)) {
/*                        (void) newcham(mtmp, (struct permonst *)0, FALSE);*/
                        /* WAC use polymorph code from zap.c*/
#if 0
                        if (!rn2(25) || !mon_poly(mtmp)) {
                                if (in_sight) {
                                        pline("%s shudders!", Monnam(mtmp));
                                        seetrap(trap);
                                }
                                /* no corpse after system shock */
                                mtmp->mhp -= rnd(30);
                                if (mtmp->mhp < 0) mondead(mtmp);
                        } else {
#endif
			mon_poly(mtmp, FALSE, "%s changes!");
			if (in_sight) seetrap(trap);
		    }
			if (!rn2(5)) deltrap(trap); /* this should tone down abuse potential --Amy */
		    break;

		case ROLLING_BOULDER_TRAP:
		    if (!is_flyer(mptr) && (!mtmp->egotype_flying) ) {
			int style = ROLL | (in_sight ? 0 : LAUNCH_UNSEEN);

		        newsym(mtmp->mx,mtmp->my);
			if (in_sight)
			    pline("Click! %s triggers %s.", Monnam(mtmp),
				  trap->tseen ?
				  "a rolling boulder trap" :
				  something);
			if (launch_obj(BOULDER, trap->launch.x, trap->launch.y,
				trap->launch2.x, trap->launch2.y, style)) {
			    if (in_sight) trap->tseen = TRUE;
			    if (mtmp->mhp <= 0) trapkilled = TRUE;
			} else {
			    deltrap(trap);
			    newsym(mtmp->mx,mtmp->my);
				break; /* otherwise the effing deltrap below causes segfaults! ARGH!!! */
			}
		    }
			if (trap && !rn2(5)) deltrap(trap); /* this should tone down abuse potential --Amy */
		    break;

		case SPEAR_TRAP:
		    if (!(mtmp->mtame) && rn2(10)) break;
			if (in_sight) {
				seetrap(trap);
				pline("A spear stabs up from a hole in the ground!");
			}
			if (!rn2(5) && !(mtmp && mtmp->mtame)) { /* Sorry, no infinite conga lines of death. --Amy */
				if (in_sight) {
					pline("But %s breaks it in two!",mon_nam(mtmp));
				}
				deltrap(trap);
			} else if (thick_skinned(mptr)) {
				if (in_sight) {
					pline("But it breaks off against %s.",mon_nam(mtmp));
				}
				deltrap(trap);
			} else if (unsolid(mptr)) {
				if (in_sight) {
					pline("It passes right through %s!",mon_nam(mtmp));
				}
			} else if (is_flyer(mptr) || mtmp->egotype_flying) {
				if (in_sight) {
					pline("The spear isn't long enough to reach %s.",mon_nam(mtmp));
				}
			} else {
				if (thitm(0, mtmp, (struct obj *)0, rnd(10)+2, FALSE))
					trapkilled = TRUE;
				else if (in_sight) {
					pline("%s is skewered!",Monnam(mtmp));
				}
			}
			 break;

		case SCYTHING_BLADE:
		    if (!(mtmp->mtame) && rn2(25)) break;
			if (in_sight) {
				seetrap(trap);
			}
			if (!rn2(15) && !(mtmp && mtmp->mtame)) { /* Sorry, no infinite conga lines of death. --Amy */
				if (in_sight) {
					pline("You see %s smash a scything blade apart!",mon_nam(mtmp));
				}
				deltrap(trap);
			} else if (unsolid(mptr)) {
				if (in_sight) {
					pline("A scything blade passes right through %s!",mon_nam(mtmp));
				}
			} else {
				if (thitm(0, mtmp, (struct obj *)0, rnd(9 + (has_head(mtmp->data) ? 3 : 0)), FALSE)) {
					if (in_sight) pline("%s is fatally hit by a scything blade!",Monnam(mtmp));
					trapkilled = TRUE;
					}
				else if (in_sight) {
					pline("%s is hit by a scything blade!",Monnam(mtmp));
				}
			}
			 break;

		case COLLAPSE_TRAP:
			 /*if (in_sight) {
				 pline("Rocks suddenly fall all around %s!",mon_nam(mtmp));
				 deltrap(trap);
				 rocks_fall(mtmp->mx,mtmp->my);
			 }*/
			/* commented out due to bugginess --Amy */
			 break;
	    case MAGIC_BEAM_TRAP:
	    case PIERCING_BEAM_TRAP:
		    if (!(mtmp->mtame) && rn2(10)) break;
		if (distu(trap->tx, trap->ty) < 4) You_hear("a faint click.");
		if (in_sight) {
		    seetrap(trap);
		}
		if (rn2(3)) break; /* monsters are unlikely to trigger this */
		if (isok(trap->launch.x,trap->launch.y) && IS_STWALL(levl[trap->launch.x][trap->launch.y].typ)) {
		    buzz(trap->launch_otyp, 8,
			 trap->launch.x,trap->launch.y,
			 sgn(trap->tx - trap->launch.x),sgn(trap->ty - trap->launch.y));
		    trap->once = 1;
		} else {
		    deltrap(trap);
		    newsym(u.ux,u.uy);
			break; /* otherwise the second deltrap could cause segfaults */
		}
		if (trap && !rn2(7) && !(mtmp && mtmp->mtame)) deltrap(trap); /* making sure monsters don't get killed over and over --Amy */
		break;

		default:
			impossible("Some monster encountered a strange trap of type %d.", tt);
	    }
	}
	if(trapkilled) return 2;
	return mtmp->mtrapped;
}

#endif /* OVL1 */
#ifdef OVLB

/* Combine cockatrice checks into single functions to avoid repeating code. */
void
instapetrify(str)
const char *str;
{
	if (Stone_resistance && IntStone_resistance) return;
	if (poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
	    return;
	/* You("turn to stone...");
	killer_format = KILLED_BY;
	killer = str;
	done(STONING); */
	if (!Stoned) {
		if (Hallucination && rn2(10)) pline("Thankfully you are already stoned.");
		else {
			pline("Your status starts changing to statue.");
			Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
			u.cnd_stoningcount++;
			stop_occupation();
			delayed_killer = (str[0] ? str : "coming into contact with a petrifying object");
			flags.botl = TRUE;
		}
	}
/* Damn you annoying programmers! At least give us a chance to save ourselves with our stock of lizard corpses! */
}

void
minstapetrify(mon,byplayer)
struct monst *mon;
boolean byplayer;
{
	if (resists_ston(mon)) return;
	if (poly_when_stoned(mon->data)) {
		mon_to_stone(mon);
		return;
	}

	/* give a "<mon> is slowing down" message and also remove
	   intrinsic speed (comparable to similar effect on the hero) */
	mon_adjust_speed(mon, -3, (struct obj *)0);

	if (cansee(mon->mx, mon->my))
		pline("%s turns to stone.", Monnam(mon));
	if (byplayer) {
		stoned = TRUE;
		xkilled(mon,0);
	} else monstone(mon);
}

void
selftouch(arg)
const char *arg;
{
	static char kbuf[BUFSZ];

	if(uwep && uwep->otyp == CORPSE && touch_petrifies(&mons[uwep->corpsenm])
			&& (!Stone_resistance || (!IntStone_resistance && !rn2(20)) )) {
		pline("%s touch the %s corpse.", arg,
		        mons[uwep->corpsenm].mname);
		sprintf(kbuf, "touching a petrifying corpse");
		instapetrify(kbuf);
	}
	/* Or your secondary weapon, if wielded */
	if(u.twoweap && uswapwep && uswapwep->otyp == CORPSE &&
			touch_petrifies(&mons[uswapwep->corpsenm]) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) )){
		pline("%s touch the %s corpse.", arg,
		        mons[uswapwep->corpsenm].mname);
		sprintf(kbuf, "touching a petrifying corpse");
		instapetrify(kbuf);
	}
}

void
mselftouch(mon,arg,byplayer)
struct monst *mon;
const char *arg;
boolean byplayer;
{
	struct obj *mwep = MON_WEP(mon);

	if (mwep && mwep->otyp == CORPSE && touch_petrifies(&mons[mwep->corpsenm]) && !rn2(4)) {
		if (cansee(mon->mx, mon->my)) {
			pline("%s%s touches the %s corpse.",
			    arg ? arg : "", arg ? mon_nam(mon) : Monnam(mon),
			    mons[mwep->corpsenm].mname);
		}
		minstapetrify(mon, byplayer);
	}
}

/* KMH, balance patch -- several ways for the player to fix stoning */
int
uunstone()
{
	if (Stoned) {
		Stoned = 0;
		if (!FunnyHallu)
			You_feel("limber!");
	    else
		pline("What a pity - you just ruined a future piece of %sart!",
		      ACURR(A_CHA) > 15 ? "fine " : "");

		if (PlayerHearsSoundEffects) pline(issoviet ? "Vy tol'ko otsrochila neizbezhnoye i v konechnom schete zakonchatsya pobivaniye kamnyami lecheniy." : "Schwueing!");

		/* The problem was fixed */
		return (1);
	}
	/* No problem to fix */
	return (0);
}


void
float_up()
{
	if(u.utrap) {
		if(u.utraptype == TT_PIT) {
			u.utrap = 0;
			You("float up, out of the pit!");
			vision_full_recalc = 1;	/* vision limits change */
			fill_pit(u.ux, u.uy);
		} else if (u.utraptype == TT_INFLOOR) {
			Your("body pulls upward, but your %s are still stuck.",
			     makeplural(body_part(LEG)));
		} else {
			You("float up, only your %s is still stuck.",
				body_part(LEG));
		}
	}
	else if(Is_waterlevel(&u.uz))
		pline("It feels as though you've lost some weight.");
	else if(u.uinwater)
		spoteffects(TRUE);
	else if(u.uswallow)
		You(is_animal(u.ustuck->data) ?
			"float away from the %s."  :
			"spiral up into %s.",
		    is_animal(u.ustuck->data) ?
			surface(u.ux, u.uy) :
			mon_nam(u.ustuck));
	else if (FunnyHallu)
		pline("Up, up, and awaaaay!  You're walking on air!");
	else if(Is_airlevel(&u.uz))
		You("gain control over your movements.");
	else
		You("start to float in the air!");
	if (u.usteed && !is_floater(u.usteed->data) && !is_flyer(u.usteed->data) && (!u.usteed->egotype_flying) ) {
	    if (Lev_at_will)
	    	pline("%s magically floats up!", Monnam(u.usteed));
	    else {
	    	You("cannot stay on %s.", mon_nam(u.usteed));
	    	dismount_steed(DISMOUNT_GENERIC);
	    }
	}
	return;
}

void
fill_pit(x, y)
int x, y;
{
	struct obj *otmp;
	struct trap *t;

	if ((t = t_at(x, y)) &&
	    ((t->ttyp == PIT) || (t->ttyp == SPIKED_PIT) || (t->ttyp == GIANT_CHASM) || (t->ttyp == SHIT_PIT) || (t->ttyp == MANA_PIT) || (t->ttyp == ANOXIC_PIT) || (t->ttyp == HYPOXIC_PIT) || (t->ttyp == ACID_PIT) ) &&
	    (otmp = sobj_at(BOULDER, x, y))) {
		obj_extract_self(otmp);
		(void) flooreffects(otmp, x, y, "settle");
	}
}

int
float_down(hmask, emask)
long hmask, emask;     /* might cancel timeout */
{
	register struct trap *trap = (struct trap *)0;
	d_level current_dungeon_level;
	boolean no_msg = FALSE;

	HLevitation &= ~hmask;
	ELevitation &= ~emask;
	if(Levitation) return(0); /* maybe another ring/potion/boots */
	if(u.uswallow) {
	    You("float down, but you are still %s.",
		is_animal(u.ustuck->data) ? "swallowed" : "engulfed");
	    return(1);
	}

	if (Punished && !carried(uball) &&
	    (is_waterypool(uball->ox, uball->oy) ||
	     ((trap = t_at(uball->ox, uball->oy)) &&
	      ((trap->ttyp == PIT) || (trap->ttyp == SPIKED_PIT) || (trap->ttyp == GIANT_CHASM) || (trap->ttyp == SHIT_PIT) || (trap->ttyp == MANA_PIT) || (trap->ttyp == ANOXIC_PIT) || (trap->ttyp == HYPOXIC_PIT) || (trap->ttyp == ACID_PIT) || (trap->ttyp == SHAFT_TRAP) || (trap->ttyp == CURRENT_SHAFT) ||
	       (trap->ttyp == TRAPDOOR) || (trap->ttyp == HOLE))))) {
			u.ux0 = u.ux;
			u.uy0 = u.uy;
			u.ux = uball->ox;
			u.uy = uball->oy;
			movobj(uchain, uball->ox, uball->oy);
			newsym(u.ux0, u.uy0);
			vision_full_recalc = 1;	/* in case the hero moved. */
	}
	/* check for falling into pool - added by GAN 10/20/86 */
	if(!Flying && !(uarmf && uarmf->oartifact == ART_RUBBER_LOVE) && !(uarmc && itemhasappearance(uarmc, APP_FLIER_CLOAK) ) ) {
		if (!u.uswallow && u.ustuck) {
			if (sticks(youmonst.data))
				You("aren't able to maintain your hold on %s.",
					mon_nam(u.ustuck));
			else
				pline("Startled, %s can no longer hold you!",
					mon_nam(u.ustuck));
			u.ustuck = 0;
		}
		/* kludge alert:
		 * drown() and lava_effects() print various messages almost
		 * every time they're called which conflict with the "fall
		 * into" message below.  Thus, we want to avoid printing
		 * confusing, duplicate or out-of-order messages.
		 * Use knowledge of the two routines as a hack -- this
		 * should really be handled differently -dlc
		 */
		if(is_drowningpool(u.ux,u.uy) && !(is_crystalwater(u.ux,u.uy)) && !Wwalking && !Race_if(PM_KORONST) && !Swimming && !u.uinwater)
			no_msg = drown();

		if(is_lava(u.ux,u.uy)) {
			(void) lava_effects();
			no_msg = TRUE;
		}
	}
	if (!trap) {
	    trap = t_at(u.ux,u.uy);
	    if(Is_airlevel(&u.uz))
		You("begin to tumble in place.");
	    else if (Is_waterlevel(&u.uz) && !no_msg)
		You_feel("heavier.");
	    /* u.uinwater msgs already in spoteffects()/drown() */
	    else if (!u.uinwater && !no_msg) {
		if (!(emask & W_SADDLE))
		{
		    boolean sokoban_trap = (In_sokoban(&u.uz) && trap);
		    if (FunnyHallu)
			pline("Bummer!  You've %s.",
			      is_waterypool(u.ux,u.uy) ?
			      "splashed down" : sokoban_trap ? "crashed" :
			      "hit the ground");
		    else {
			if (!sokoban_trap)
			    You("float gently to the %s.",
				surface(u.ux, u.uy));
			else {
			    /* Justification elsewhere for Sokoban traps
			     * is based on air currents. This is
			     * consistent with that.
			     * The unexpected additional force of the
			     * air currents once leviation
			     * ceases knocks you off your feet.
			     */
			    You("fall over.");
			    losehp(rnd(2), "dangerous winds", KILLED_BY);
			    if (u.usteed && !mayfalloffsteed()) dismount_steed(DISMOUNT_FELL);
			    selftouch("As you fall, you");
			}
		    }
		}
	    }
	}

	/* can't rely on u.uz0 for detecting trap door-induced level change;
	   it gets changed to reflect the new level before we can check it */
	assign_level(&current_dungeon_level, &u.uz);

	if(trap)
		switch(trap->ttyp) {
		case STATUE_TRAP:
		case SATATUE_TRAP:
			break;
		case HOLE:
		case TRAPDOOR:
		case SHAFT_TRAP:
		case CURRENT_SHAFT:
			if(!Can_fall_thru(&u.uz) || u.ustuck)
				break;
			/* fall into next case */
		default:
			if (!u.utrap) /* not already in the trap */
				dotrap(trap, 0);
	}

	if (!Is_airlevel(&u.uz) && !Is_waterlevel(&u.uz) && !u.uswallow &&
		/* falling through trap door calls goto_level,
		   and goto_level does its own pickup() call */
		on_level(&u.uz, &current_dungeon_level))
	    (void) pickup(1);
	return 1;
}

STATIC_OVL void
doicetrap(box)
struct obj *box;        /* at the moment only for floor traps */
{
        int num = 0;
        num = d(4, 4) + rnd((monster_difficulty() / 3) + 1);
		if (num > 1) {
			if (u.ulevel == 1) num /= 2;
			else if (u.ulevel == 2) {
				num *= 2;
				num /= 3;
			} else if (u.ulevel == 3) {
				num *= 3;
				num /= 4;
			} else if (u.ulevel == 4) {
				num *= 4;
				num /= 5;
			}
		}
        if (box) {
                impossible("doicetrap() called with non-null box.");
                return;
        }

        pline("A freezing cloud shoots from %s!", surface(u.ux, u.uy));
        if (Cold_resistance) {
                shieldeff(u.ux, u.uy);
                num /= 2;
		    if (StrongCold_resistance && num > 1) num /= 2;
		    if (ColdImmunity) num = 0;
        }

        if (!num)
                You("are uninjured.");
        else
                losehp(num, "freezing cloud", KILLED_BY_AN);
			if (!rn2(10)) make_frozen(HFrozen + 1 + (num * 5), TRUE); /* randomly freeze the player --Amy */
		    if (isevilvariant || !rn2(issoviet ? 6 : Race_if(PM_GAVIL) ? 6 : Race_if(PM_HYPOTHERMIC) ? 6 : 33)) /* new calculations --Amy */        destroy_item(POTION_CLASS, AD_COLD);
}

STATIC_OVL void
doshocktrap(box)
struct obj *box;        /* at the moment only for floor traps */
{
        int num = 0;
        num = d(4, 4) + rnd((monster_difficulty() / 3) + 1);
		if (num > 1) {
			if (u.ulevel == 1) num /= 2;
			else if (u.ulevel == 2) {
				num *= 2;
				num /= 3;
			} else if (u.ulevel == 3) {
				num *= 3;
				num /= 4;
			} else if (u.ulevel == 4) {
				num *= 4;
				num /= 5;
			}
		}
        if (box) {
                impossible("doshocktrap() called with non-null box.");
                return;
        }

        pline("A lightning bolt hits you!");
        if (Shock_resistance) {
                shieldeff(u.ux, u.uy);
                num /= 2;
		    if (StrongShock_resistance && num > 1) num /= 2;
		    if (ShockImmunity) num = 0;
        }

        if (!num)
                You("are uninjured.");
        else
                losehp(num, "lightning trap", KILLED_BY_AN);
			if (!rn2(3)) make_numbed(HNumbed + 1 + (num * 5), TRUE); /* PLAYER NUMBED CAN'T DO --Amy */
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) destroy_item(WAND_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) destroy_item(RING_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 30 : 165)) /* new calculations --Amy */	destroy_item(AMULET_CLASS, AD_ELEC);
}

STATIC_OVL void
dovolttrap(box)
struct obj *box;        /* at the moment only for floor traps */
{
        int num = 0;
        num = d(8, 4) + rnd(monster_difficulty() + 1);
	  if (u.urmaxlvlUP < 5) num = d(5, 4) + rnd((monster_difficulty() / 2) + 1);
		if (num > 1) {
			if (u.ulevel == 1) num /= 2;
			else if (u.ulevel == 2) {
				num *= 2;
				num /= 3;
			} else if (u.ulevel == 3) {
				num *= 3;
				num /= 4;
			} else if (u.ulevel == 4) {
				num *= 4;
				num /= 5;
			}
		}
        if (box) {
                impossible("doshocktrap() called with non-null box.");
                return;
        }

        pline("A massive electric shock surges through your body!");
        if (Shock_resistance) {
                shieldeff(u.ux, u.uy);
                num *= 2;
		    num /= 3;
		    if (StrongShock_resistance && num > 1) num /= 2;
		    if (ShockImmunity) num = 0;
        }

        if (!num)
                You("are uninjured.");
        else
                losehp(num, "volt trap", KILLED_BY_AN);
		    make_numbed(HNumbed + 1 + (num * 5), TRUE); /* PLAYER NUMBED CAN'T DO --Amy */
		    if (isevilvariant || !rn2(issoviet ? 2 : 10)) destroy_item(WAND_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 2 : 10)) destroy_item(RING_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 10 : 50)) /* new calculations --Amy */	destroy_item(AMULET_CLASS, AD_ELEC);
			if (!rn2(3)) {
				if (!Free_action || !rn2(StrongFree_action ? 20 : 5)) {
				    pline("You are frozen in place!");
					if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
				    if (isstunfish) nomul(-rnz(7), "frozen by a volt trap", TRUE);
				    else nomul(-rnd(7), "frozen by a volt trap", TRUE);
				    nomovemsg = You_can_move_again;
				    exercise(A_DEX, FALSE);
				} else You("stiffen momentarily.");

			}

}

void
doshittrap(box)
struct obj *box;        /* at the moment only for floor traps */
{
        int num = 0;
        num = d(4, 4) + rnd((monster_difficulty() / 2) + 1);
		if (num > 1) {
			if (u.ulevel == 1) num /= 2;
			else if (u.ulevel == 2) {
				num *= 2;
				num /= 3;
			} else if (u.ulevel == 3) {
				num *= 3;
				num /= 4;
			} else if (u.ulevel == 4) {
				num *= 4;
				num /= 5;
			}
		}
        if (box) {
                impossible("doshittrap() called with non-null box.");
                return;
        }

        pline("You stepped into a heap of shit!");

		if (uarmf && uarmf->oartifact == ART_SMELL_LIKE_DOG_SHIT) {
			pline("Now you smell even worse than before.");
			(void) adjattrib(A_CHA, -1, TRUE, TRUE);
		}

		if (uarmf && uarmf->oartifact == ART_ELIANE_S_SHIN_SMASH) {
			pline("But your footwear is unaffected.");
			return;
		}

        if (Acid_resistance) { /* let's just assume the stuff is acidic or corrosive --Amy */
                shieldeff(u.ux, u.uy);
                num /= 2;
        }

	if (Stoned) fix_petrification();

	if (!rn2(10) || !(uarmf && itemhasappearance(uarmf, APP_PROFILED_BOOTS) ) ) {

	/* damage boots, but don't destroy them; if they would be destroyed, disenchant them instead --Amy */
	    if (uarmf && !rn2(5)) {
		if (uarmf->oeroded < MAX_ERODE) (void)rust_dmg(uarmf, xname(uarmf), 0, TRUE, &youmonst);
		else if (uarmf->spe > -20) {
			uarmf->spe--;
			pline("Eek, it's icky.");
		}
	    }
	    if (uarmf && !rn2(5)) {
		if (uarmf->oeroded < MAX_ERODE) (void)rust_dmg(uarmf, xname(uarmf), 1, TRUE, &youmonst);
		else if (uarmf->spe > -20) {
			uarmf->spe--;
			pline("Eek, it's icky.");
		}
	    }
	    if (uarmf && !rn2(5)) {
		if (uarmf->oeroded2 < MAX_ERODE) (void)rust_dmg(uarmf, xname(uarmf), 2, TRUE, &youmonst);
		else if (uarmf->spe > -20) {
			uarmf->spe--;
			pline("Eek, it's icky.");
		}
	    }
	    if (uarmf && !rn2(5)) {
		if (uarmf->oeroded2 < MAX_ERODE) (void)rust_dmg(uarmf, xname(uarmf), 3, TRUE, &youmonst);
		else if (uarmf->spe > -20) {
			uarmf->spe--;
			pline("Eek, it's icky.");
		}
	    }
		/* Dog shit is extremely aggressive to footwear. Let's give it a chance to do withering damage. --Amy */
	    if (uarmf && !rn2(25)) {
		if (uarmf->oeroded < MAX_ERODE) (void)wither_dmg(uarmf, xname(uarmf), 0, TRUE, &youmonst);
		else if (uarmf->spe > -20) {
			uarmf->spe -= 2;
			pline("Goddamn shit!");
		}
	    }
	    if (uarmf && !rn2(25)) {
		if (uarmf->oeroded < MAX_ERODE) (void)wither_dmg(uarmf, xname(uarmf), 1, TRUE, &youmonst);
		else if (uarmf->spe > -20) {
			uarmf->spe -= 2;
			pline("Goddamn shit!");
		}
	    }
	    if (uarmf && !rn2(25)) {
		if (uarmf->oeroded2 < MAX_ERODE) (void)wither_dmg(uarmf, xname(uarmf), 2, TRUE, &youmonst);
		else if (uarmf->spe > -20) {
			uarmf->spe -= 2;
			pline("Goddamn shit!");
		}
	    }
	    if (uarmf && !rn2(25)) {
		if (uarmf->oeroded2 < MAX_ERODE) (void)wither_dmg(uarmf, xname(uarmf), 3, TRUE, &youmonst);
		else if (uarmf->spe > -20) {
			uarmf->spe -= 2;
			pline("Goddamn shit!");
		}
	    }

	} /* profiled boots or random chance check end */

	if (!uarmf) {
		pline("You slip on the shit with your bare %s.", makeplural(body_part(FOOT)));
		num *= 2;
	}

	if (uarmf ? !rn2(20) : !rn2(15)) {
		HFumbling = FROMOUTSIDE | rnd(5);
		incr_itimeout(&HFumbling, rnd(2));
		u.fumbleduration += rnz(uarmf ? 30 : 20);
	}

	if ((uarmf && itemhasappearance(uarmf, APP_PROFILED_BOOTS) ) || Role_if(PM_HUSSY) ) {
	    if (!(HFast & INTRINSIC)) {
		if (!Fast)
		    You("speed up.");
		else
		    Your("quickness feels more natural.");
		exercise(A_DEX, TRUE);
	    }
	    HFast |= FROMOUTSIDE;
	} else if (!rn2(20)) u_slow_down();

	if ( !rn2(StrongFree_action ? 1000 : 100) || (!Free_action && !rn2(10))) {
		You("inhale the intense smell of shit! The world spins and goes dark.");
		nomovemsg = "You are conscious again.";
		if (isstunfish) nomul(-rnz(10), "unconscious from smelling dog shit", TRUE);
		else nomul(-rnd(10), "unconscious from smelling dog shit", TRUE);
		exercise(A_DEX, FALSE);
	}

	if (uarmf && itemhasappearance(uarmf, APP_PROFILED_BOOTS) ) num /= 4;
      if (num) losehp(num, "heap of shit", KILLED_BY_AN);

}

void
dofiretrap(box)
struct obj *box;	/* null for floor trap */
{
	boolean see_it = !Blind;
	int num, alt;

/* Bug: for box case, the equivalent of burn_floor_paper() ought
 * to be done upon its contents.
 */

	if ((box && !carried(box)) ? is_waterypool(box->ox, box->oy) : Underwater) {
	    pline("A cascade of steamy bubbles erupts from %s!",
		    the(box ? xname(box) : surface(u.ux,u.uy)));
	    if (Fire_resistance) You("are uninjured.");
	    else losehp(rnd(3), "boiling water", KILLED_BY);
	    return;
	}
	pline("A %s %s from %s!", tower_of_flame,
	      box ? "bursts" : "erupts",
	      the(box ? xname(box) : surface(u.ux,u.uy)));
	if (Slimed) {        
	      pline("The slime that covers you is burned away!");
	      Slimed = 0;
	}
    if (Frozen) {
	pline("The ice thaws!");
	make_frozen(0L, FALSE);
    }
	if (Fire_resistance) {
	    shieldeff(u.ux, u.uy);
	    num = rn2(2);
	} else if (Upolyd) {
	    num = d(2,4);
	    switch (u.umonnum) {
	    case PM_PAPER_GOLEM:   alt = u.mhmax; break;
	    case PM_STRAW_GOLEM:   alt = u.mhmax / 2; break;
	    case PM_WOOD_GOLEM:    alt = u.mhmax / 4; break;
	    case PM_LEATHER_GOLEM: alt = u.mhmax / 8; break;
	    default: alt = 0; break;
	    }
	    if (alt > num) num = alt;
	    if (u.mhmax > mons[u.umonnum].mlevel)
		u.mhmax -= rn2(min(u.mhmax,num + 1)), flags.botl = 1;
	} else {
	    num = d(2,4);
	    if ((u.uhpmax > u.ulevel) && (issoviet || !rn2(2))) {

		/* In Soviet Russia, everything that fucks up the player MUST also still fuck up the player in a variant.
		 * Woe to the heretical variant developer that DARES changing anything, for she is unjust and evil and
		 * her changes need to be reverted ASAP! --Amy */

		u.uhpmax -= rn2(min(u.uhpmax,5)), flags.botl = 1;
		if (issoviet) pline("Plamya plameni! Kha kha kha! Nadeyus', vy poteryayete vse svoi maksimal'nyye ochki zhizni, spasticheskaya zaderzhka.");
	    }
	}
	if (Race_if(PM_LOWER_ENT)) num *= 2;

	if (num > 1) {
		if (u.ulevel == 1) num /= 2;
		else if (u.ulevel == 2) {
			num *= 2;
			num /= 3;
		} else if (u.ulevel == 3) {
			num *= 3;
			num /= 4;
		} else if (u.ulevel == 4) {
			num *= 4;
			num /= 5;
		}
	}

	if (!num)
	    You("are uninjured.");
	else
	    losehp(num + rnd((monster_difficulty() / 3) + 1), tower_of_flame, KILLED_BY_AN);
	burn_away_slime();

	/*if ( burnarmor(&youmonst) || !rn2(33))*/ /* new calculation -- Amy */ {
	    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33)) destroy_item(SCROLL_CLASS, AD_FIRE);
	    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33)) destroy_item(SPBOOK_CLASS, AD_FIRE);
	    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33)) destroy_item(POTION_CLASS, AD_FIRE);
		if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 33)) burnarmor(&youmonst);
	}
	if (!box && burn_floor_paper(u.ux, u.uy, see_it, TRUE) && !see_it)
	    You("smell paper burning.");
	if (is_ice(u.ux, u.uy))
	    melt_ice(u.ux, u.uy);

	if (u.uhp > u.uhpmax) u.uhp = u.uhpmax; /* fail safe */

}

STATIC_OVL void
doplasmatrap(box)
struct obj *box;	/* null for floor trap */
{
	boolean see_it = !Blind;
	int num, alt;

/* Bug: for box case, the equivalent of burn_floor_paper() ought
 * to be done upon its contents.
 */

	if ((box && !carried(box)) ? is_waterypool(box->ox, box->oy) : Underwater) {
	    pline("A cascade of steamy bubbles erupts from %s!",
		    the(box ? xname(box) : surface(u.ux,u.uy)));
	    if (Fire_resistance) You("are uninjured.");
	    else losehp(rnd(3), "boiling water", KILLED_BY);
	    return;
	}
	pline("You are enveloped by searing plasma radiation!");
	if (Slimed) {        
	      pline("The slime that covers you is burned away!");
	      Slimed = 0;
	}
    if (Frozen) {
	pline("The ice thaws!");
	make_frozen(0L, FALSE);
    }
	if (Fire_resistance) {
	    shieldeff(u.ux, u.uy);
	    num = rnd(FireImmunity ? 1 : StrongFire_resistance ? 4 : 12);
	} else if (Upolyd) {
	    num = d(6,4);
	    switch (u.umonnum) {
	    case PM_PAPER_GOLEM:   alt = u.mhmax; break;
	    case PM_STRAW_GOLEM:   alt = u.mhmax / 2; break;
	    case PM_WOOD_GOLEM:    alt = u.mhmax / 4; break;
	    case PM_LEATHER_GOLEM: alt = u.mhmax / 8; break;
	    default: alt = 0; break;
	    }
	} else {
	    num = d(6,4);
	    if (u.urmaxlvlUP < 5) num = d(3, 4);
	}
	if (Race_if(PM_LOWER_ENT)) num *= 2;

	if (num > 1) {
		if (u.ulevel == 1) num /= 2;
		else if (u.ulevel == 2) {
			num *= 2;
			num /= 3;
		} else if (u.ulevel == 3) {
			num *= 3;
			num /= 4;
		} else if (u.ulevel == 4) {
			num *= 4;
			num /= 5;
		}
	}

	if (!num)
	    You("are uninjured.");
	else {
	    losehp(num + rnd(monster_difficulty() + 1), "plasma trap", KILLED_BY_AN);
		make_stunned(HStun + num + rnd(monster_difficulty() + 1), TRUE);
	}

	burn_away_slime();

	    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5)) destroy_item(SCROLL_CLASS, AD_FIRE);
	    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5)) destroy_item(SPBOOK_CLASS, AD_FIRE);
	    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5)) destroy_item(POTION_CLASS, AD_FIRE);
		if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5)) burnarmor(&youmonst);

	if (!box && burn_floor_paper(u.ux, u.uy, see_it, TRUE) && !see_it)
	    You("smell paper burning.");
	if (is_ice(u.ux, u.uy))
	    melt_ice(u.ux, u.uy);
}

STATIC_OVL void
domagictrap()
{
	register int fate = rnd(20);
	int cx, cy, attempts;
	coord cc;
	struct permonst *pm = 0;

	/* What happened to the poor sucker? */

	if (fate < 10) {
	  /* Most of the time, it creates some monsters. */
	  register int cnt = rnd(4);

	if (Aggravate_monster) {
		u.aggravation = 1;
		reset_rndmonst(NON_PM);
	}

	  if (!resists_blnd(&youmonst)) {
		You("are momentarily blinded by a flash of light!");
		make_blinded((long)rn1(5,10),FALSE);
		if (!Blind) Your("%s", vision_clears);
	  } else if (!Blind) {
		You("see a flash of light!");
	  }  else
		You_hear("a deafening roar!");
	  while(cnt--)
		(void) makemon((struct permonst *) 0, u.ux, u.uy, NO_MM_FLAGS);

	u.aggravation = 0;

	}
	else
	  switch (fate) {

	     case 10:
	     case 11:
		      /* sometimes nothing happens */
			break;
	     case 12: /* a flash of fire */
			dofiretrap((struct obj *)0);
			break;

	     /* odd feelings */
	     case 13:
			pline("A shiver runs up and down your %s!", body_part(SPINE));

			u.aggravation = 1;
			u.heavyaggravation = 1;
			reset_rndmonst(NON_PM);
		      cx = rn2(COLNO);
		      cy = rn2(ROWNO);
			if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) break;

			makemon(mkclass(S_GHOST,0), cx, cy, MM_ANGRY|MM_FRENZIED);

			u.aggravation = 0;
			u.heavyaggravation = 0;

			break;
	     case 14:
			You_hear(FunnyHallu ? "the moon howling at you." : "distant howling.");

			attempts = 0;
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		      cx = rn2(COLNO);
		      cy = rn2(ROWNO);
			if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) break;

newwere:
			do {
				pm = rndmonst();
				attempts++;
				if (attempts && (attempts % 10000 == 0)) u.mondiffhack++;
				if (!rn2(2000)) reset_rndmonst(NON_PM);

			} while ( (!pm || (pm && !(dmgtype(pm, AD_WERE)))) && attempts < 50000);

			if (!pm && rn2(50) ) {
				attempts = 0;
				goto newwere;
			}
			if (pm && !(dmgtype(pm, AD_WERE)) && rn2(50) ) {
				attempts = 0;
				goto newwere;
			}

			if (pm) (void) makemon(pm, cx, cy, MM_ANGRY|MM_FRENZIED);

			u.mondiffhack = 0;
			u.aggravation = 0;

			break;
	     case 15:	if (on_level(&u.uz, &qstart_level))
			    You_feel("%slike the prodigal son.",
			      (flags.female || (Upolyd && is_neuter(youmonst.data))) ?
				     "oddly " : "");
			else
			    You("suddenly yearn for %s.",
				FunnyHallu ? "Cleveland" :
			    (In_quest(&u.uz) || at_dgn_entrance("The Quest")) ?
						"your nearby homeland" :
						"your distant homeland");

			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		      cx = rn2(COLNO);
		      cy = rn2(ROWNO);
			if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) break;

			makemon(mkclass(S_NEMESE,0), cx, cy, MM_ANGRY|MM_FRENZIED);

			u.aggravation = 0;

			break;
	     case 16:
			{
				int dmg;

				Your("pack shakes violently!");
/* KMH, balance patch -- Idea by Wolfgang von Hansen <wvh@geodesy.inka.de> */
				dmg = jumble_pack();
				if (dmg)
					losehp(dmg, "violence", KILLED_BY);
			break;
			}
	     case 17:
			You(FunnyHallu ? "smell hamburgers." : "smell charred flesh.");

			int aggroamount = rnd(6);
			if (isfriday) aggroamount *= 2;

			while (aggroamount) {

			attempts = 0;
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		      cx = rn2(COLNO);
		      cy = rn2(ROWNO);
			if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) break;

newhamburger:
			do {
				pm = rndmonst();
				attempts++;
				if (attempts && (attempts % 10000 == 0)) u.mondiffhack++;
				if (!rn2(2000)) reset_rndmonst(NON_PM);

			} while ( (!pm || (pm && !(is_undead(pm)))) && attempts < 50000);

			if (!pm && rn2(50) ) {
				attempts = 0;
				goto newhamburger;
			}
			if (pm && !(is_undead(pm)) && rn2(50) ) {
				attempts = 0;
				goto newhamburger;
			}

			if (pm) (void) makemon(pm, cx, cy, MM_ANGRY);
			aggroamount--;

			u.mondiffhack = 0;

			} /* while (aggroamount) */

			u.aggravation = 0;

			break;
	     case 18:
			You_feel("tired.");
			if(HSleep_resistance & INTRINSIC) {
				HSleep_resistance &= ~INTRINSIC;
			} 
			if(HSleep_resistance & TIMEOUT) {
				HSleep_resistance &= ~TIMEOUT;
			}
			if (!rn2(5)) attrcurse();
			break;

	     /* very occasionally something nice happens. */

	     case 19:
		    /* tame nearby monsters */
		   {   register int i,j;
		       register struct monst *mtmp;

		       (void) adjattrib(A_CHA,1,FALSE, TRUE);
		       for(i = -1; i <= 1; i++) for(j = -1; j <= 1; j++) {
			   if(!isok(u.ux+i, u.uy+j)) continue;
			   mtmp = m_at(u.ux+i, u.uy+j);
			   if(mtmp)
			       (void) tamedog(mtmp, (struct obj *)0, FALSE);
		       }
		       break;
		   }

	     case 20:
		    /* uncurse stuff */
		   {	struct obj pseudo;
			long save_conf = HConfusion;

			pseudo = zeroobj;   /* neither cursed nor blessed */
			pseudo.otyp = SCR_REMOVE_CURSE;
			HConfusion = 0L;
			(void) seffects(&pseudo);
			HConfusion = save_conf;
			break;
		   }
	     default: break;
	  }
}

/*
 * Scrolls, spellbooks, potions, and flammable items
 * may get affected by the fire.
 *
 * Return number of objects destroyed. --ALI
 */
int
fire_damage(chain, force, here, x, y)
struct obj *chain;
boolean force, here;
xchar x, y;
{
    int chance;
    struct obj *obj, *otmp, *nobj, *ncobj;
    int retval = 0;
    int in_sight = !Blind && couldsee(x, y);	/* Don't care if it's lit */
    int dindx;

    for (obj = chain; obj; obj = nobj) {
	nobj = here ? obj->nexthere : obj->nobj;

	/* object might light in a controlled manner */
	if (catch_lit(obj))
	    continue;

	if (stack_too_big(obj)) continue;

	if (obj->oerodeproof) continue;

	if (Is_container(obj)) {
	    switch (obj->otyp) {
	    case LEAD_BOX:
	    case TOP_BOX:
	    case ICE_BOX:
	    case DISPERSION_BOX:
	    case ICE_BOX_OF_HOLDING:
	    case ICE_BOX_OF_WATERPROOFING:
	    case ICE_BOX_OF_DIGESTION:
		continue;		/* Immune */
		/*NOTREACHED*/
		break;
	    case CHEST:
	    case NANO_CHEST:
	    case CHEST_OF_HOLDING:
		chance = 60;
		break;
	    case LARGE_BOX:
	    case LARGE_BOX_OF_DIGESTION:
		chance = 55;
		break;
	    case TREASURE_CHEST:
	    case LOOT_CHEST:
		chance = 0;
		break;
	    default:
		chance = 50;
		break;
	    }
	    if (!force && (Luck + 45) > rn2(chance))
		continue;
	    /* Container is burnt up - dump contents out */
	    if (in_sight) pline("%s catches fire and burns.", Yname2(obj));
	    if (Has_contents(obj)) {
		if (in_sight) pline("Its contents fall out.");
		for (otmp = obj->cobj; otmp; otmp = ncobj) {
		    ncobj = otmp->nobj;
		    obj_extract_self(otmp);
		    if (!flooreffects(otmp, x, y, ""))
			place_object(otmp, x, y);
		}
	    }
	    delobj(obj);
	    retval++;
	} else if (!force && (Luck + 75) > rn2(90)) {
	    /*  chance per item of sustaining damage:
	     *	max luck (full moon):	 5%
	     *	max luck (elsewhen):	10%
	     *	avg luck (Luck==0):	75%
	     *	awful luck (Luck<-4):  100%
	     */
	    continue;
	} else if (obj->oclass == SCROLL_CLASS || obj->oclass == SPBOOK_CLASS) {
	    if (obj->otyp == SCR_FIRE || obj->otyp == SPE_FIREBALL || (rn2(5) && obj->oartifact) )
		continue;
	    if (obj->otyp == SPE_BOOK_OF_THE_DEAD) {
		if (in_sight) pline("Smoke rises from %s.", the(xname(obj)));
		continue;
	    }
	    dindx = (obj->oclass == SCROLL_CLASS) ? 2 : 3;
	    if (in_sight)
		pline("%s %s.", Yname2(obj), (obj->quan > 1) ?
		      destroy_strings[dindx*3 + 1] : destroy_strings[dindx*3]);
	    delobj(obj);
	    retval++;
	} else if (obj->oclass == POTION_CLASS) {
	    dindx = 1;
	    if (in_sight)
		pline("%s %s.", Yname2(obj), (obj->quan > 1) ?
		      destroy_strings[dindx*3 + 1] : destroy_strings[dindx*3]);
	    delobj(obj);
	    retval++;
	} else if (is_flammable(obj) && !(Race_if(PM_CHIQUAI) && rn2(4)) && !(obj->oartifact && rn2(4)) && obj->oeroded < MAX_ERODE &&
		   !(obj->oerodeproof || (obj->blessed && !rnl(4)))) {
	    if (in_sight) {
		pline("%s %s%s.", Yname2(obj), otense(obj, "burn"),
		      obj->oeroded+1 == MAX_ERODE ? " completely" :
		      obj->oeroded ? " further" : "");
	    }
	    obj->oeroded++;
	}
    }

    if (retval && !in_sight)
	You("smell smoke.");
    return retval;
}

/* This function is there for special handling of players being subjected to the lethe waters,
   which may happen because they stepped into it, got pulled down, or were hit by a geyser on a lethe level.
   What it does is "withering" objects, that is, damaging them even if they're erodeproof or outright immune to damage.
   We don't want this to happen to every single item though. --Amy */

void
lethe_damage(obj, force, here)
register struct obj *obj;
register boolean force, here;
{
	struct obj *otmp;

	for (; obj; obj = otmp) {

		otmp = here ? obj->nexthere : obj->nobj;


	/* The invocation artifacts and the Amulet of Yendor must be immune.
	 * Alignment keys, too, even though the game is still winnable without them. */

			if ( (!rn2(50) || force ) && !(Race_if(PM_CHIQUAI) && rn2(4)) && (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ) && (!obj->blessed || !rn2(4) ) && !stack_too_big(obj) && !is_unwitherable(obj) && obj->otyp != SPE_BOOK_OF_THE_DEAD && obj->otyp != AMULET_OF_YENDOR && obj->otyp != CANDELABRUM_OF_INVOCATION && obj->otyp != BELL_OF_OPENING && obj->oartifact != ART_KEY_OF_LAW && obj->oartifact != ART_KEY_OF_CHAOS && obj->oartifact != ART_KEY_OF_NEUTRALITY && obj->oartifact != ART_GAUNTLET_KEY   ) { /* 2% chance for each item to be affected, blessed ones are only affected with 0.5% chance --Amy */

				if (rn2(2)) {

					if (obj->oeroded < MAX_ERODE && !(obj->oartifact && rn2(4)) && !((obj->blessed && !rnl(4)))) obj->oeroded++;
					else if (obj->oeroded == MAX_ERODE && !evades_destruction(obj) && !hard_to_destruct(obj) && (!obj->oartifact || !rn2(4)))
					{
				    
					if (!evilfriday) pline("Your %s withered away!", xname(obj));
					else pline("One of your items withered away!");
					delobj(obj);
					update_inventory();
			    
					}
				}
				else {

					if (obj->oeroded2 < MAX_ERODE && !(obj->oartifact && rn2(4)) && !((obj->blessed && !rnl(4)))) obj->oeroded2++;
					else if (obj->oeroded2 == MAX_ERODE && !evades_destruction(obj) && !hard_to_destruct(obj) && (!obj->oartifact || !rn2(4)))
					{
				    
					if (!evilfriday) pline("Your %s withered away!", xname(obj));
					else pline("One of your items withered away!");
					delobj(obj);
					update_inventory();
			    
					}
				}

			}
	}
}

void
water_damage(obj, force, here)
register struct obj *obj;
register boolean force, here;
{
	/* Dips in the Lethe are a very poor idea */
	int luckpenalty = level.flags.lethe? 7 : 0;
	struct obj *otmp;

	/* Scrolls, spellbooks, potions, weapons and
	   pieces of armor may get affected by the water */
	for (; obj; obj = otmp) {
		otmp = here ? obj->nexthere : obj->nobj;

		if (uarmf && obj == uarmf && itemhasappearance(uarmf, APP_YELLOW_SNEAKERS) ) {
			pline("Urgh, your yellow sneakers hate getting wet!");
			if (isstunfish) nomul(-rnz(20), "getting their yellow sneakers wet", TRUE);
			else nomul(-rnd(20), "getting their yellow sneakers wet", TRUE);
			losehp(rnd(10), "endangering their yellow sneakers", KILLED_BY);
		}

		if (uarmf && obj == uarmf && uarmf->oartifact == ART_JANA_S_VAGINAL_FUN && !rn2(100)) {
			u.youaredead = 1;
			pline("Jana suddenly appears and pees on you, and you melt instantly.");
			killer_format = KILLED_BY;
			killer = "Jana's vaginal fun";
			done(DIED);
			u.youaredead = 0;
		}

		(void) snuff_lit(obj);

		if (stack_too_big(obj) && !issoviet) continue;

		if (itemhasappearance(obj, APP_BRAND_NEW_GLOVES) && rn2(4) ) continue;

		if (itemhasappearance(obj, APP_IMAGINARY_HEELS) ) continue;

		if (obj->oartifact == ART_SARTIRO ) continue;
		if (obj->oartifact == ART_CUMBERSOME_DESC ) continue;
		if (obj->oartifact == ART_GOODRES_ELVEN ) continue;
		if (obj->oartifact == ART_PARTICULARLY_SOLID_SKULL ) continue;

		if (obj->oartifact == ART_EXTENDED_DURABILITY && rn2(4)) continue;

		if (uarm && uarm->oartifact == ART_CANNOT_BE_HARMED_BLA_BLA && (obj->owornmask & W_ARMOR) ) continue;

		if (uwep && uwep->oartifact == ART_SLAM_ && rn2(10) && (obj->owornmask & W_ARMOR) ) continue;

		if (obj->oartifact == ART_PROOFINGNESS_POOFS && !obj->rknown) continue;

		if (obj->oartifact == ART_RATCH_CLOSURE_SCRATCHING && rn2(4) ) continue;

		if (itemhasappearance(obj, APP_WITHERED_CLOAK) ) continue;

		if ((obj->where != OBJ_FLOOR) && uarmh && itemhasappearance(uarmh, APP_SCUBA_HELMET) ) continue;
		if ((obj->where != OBJ_FLOOR) && uarmc && itemhasappearance(uarmc, APP_WETSUIT)) continue;
		if ((obj->where != OBJ_FLOOR) && uwep && uwep->oartifact == ART_FOAMONIA_WATER ) continue;
		if ((obj->where != OBJ_FLOOR) && uwep && uwep->oartifact == ART_GARY_S_RIVALRY ) continue;
		if (powerfulimplants() && uimplant && uimplant->oartifact == ART_NEWFOUND_AND_USEFUL) continue;
		if ((obj->where != OBJ_FLOOR) && uarmf && uarmf->oartifact == ART_JESUS_FOOTWEAR) continue;
		if ((obj->where != OBJ_FLOOR) && uarmh && uarmh->oartifact == ART_MASK_OF_TLALOC) continue;
		if ((obj->where != OBJ_FLOOR) && uwep && uwep->oartifact == ART_JIN_GANG_ZUO) continue;
		if ((obj->where != OBJ_FLOOR) && uwep && uwep->oartifact == ART_ALWAYS_IN_IT) continue;
		if ((obj->where != OBJ_FLOOR) && uarm && uarm->oartifact == ART_GO_UNDER_OR_UP) continue;
		if ((obj->where != OBJ_FLOOR) && u.umoved && uarmc && uarmc->oartifact == ART_BUT_SHES_HOMELESS) continue;
		if ((obj->where != OBJ_FLOOR) && uarmf && uarmf->oartifact == ART_WELCOME_ON_BOARD) continue;
		if ((obj->where != OBJ_FLOOR) && uarmf && uarmf->oartifact == ART_UNDERWATER_LOVE) continue;
		if ((obj->where != OBJ_FLOOR) && uarmh && uarmh->oartifact == ART_AIR_ON_HOLD) continue;
		if ((obj->where != OBJ_FLOOR) && have_whitewaterrafting()) continue;
		if ((obj->where != OBJ_FLOOR) && uarmc && uarmc->oartifact == ART_SCOOBA_COOBA) continue;
		if ((obj->where != OBJ_FLOOR) && uwep && uwep->oartifact == ART_TRIDENT_OF_POSEIDON) continue;		
		if ((obj->where != OBJ_FLOOR) && uwep && uwep->oartifact == ART_TEZCATLIPOCA_S_BUBBLESTORM) continue;		
		if ((obj->where != OBJ_FLOOR) && Race_if(PM_PLAYER_ATLANTEAN)) continue;
		if ((obj->where != OBJ_FLOOR) && uarmf && uarmf->oartifact == ART_JANA_S_VAGINAL_FUN) continue;
		if ((obj->where != OBJ_FLOOR) && uarmu && uarmu->oartifact == ART_THERMAL_BATH) continue;
		if ((obj->where != OBJ_FLOOR) && uarm && uarm->oartifact == ART_SWIMCHAMP) continue;
		if ((obj->where != OBJ_FLOOR) && Race_if(PM_SEA_ELF)) continue;
		if ((obj->where != OBJ_FLOOR) && tech_inuse(T_SILENT_OCEAN)) continue;
		if ((obj->where != OBJ_FLOOR) && uarmf && uarmf->oartifact == ART_STEERBOAT) continue;
		if ((obj->where != OBJ_FLOOR) && uwep && uwep->oartifact == ART_BLUE_CORSAR_SWIMMING) continue;

		if ((obj->where != OBJ_FLOOR) && Race_if(PM_VIKING) && (rn2(50) < u.ulevel) ) continue;

		if (obj && obj->oartifact == ART_ELIANE_S_SHIN_SMASH) {
			pline("The liquid destroys your footwear instantly.");
			delobj(obj);
			continue;
		}

		if(obj->otyp == CAN_OF_GREASE && obj->spe > 0) {
			continue;
		} else if(obj->otyp == LUBRICANT_CAN && obj->spe > 0) {
			continue;
		} else if(obj->greased) {
			if (force || !rn2(2)) obj->greased -= 1;
		} else if(Is_container(obj) && !Is_box(obj) && !(obj->oartifact == ART_BAG_OF_THE_HESPERIDES) && !(obj->oartifact == ART_KNOW_OF_THE_CURSE) && !(obj->oartifact == ART_SEMINARIOS_KARTOFFLES) && !(obj->otyp == ICE_BOX_OF_WATERPROOFING) && !(obj->otyp == DISPERSION_BOX) &&
			(obj->otyp != OILSKIN_SACK || (obj->cursed && !(obj->oartifact == ART_SURFING_FUN) && !rn2(3)))) {
			water_damage(obj->cobj, force, FALSE);
		} else if (!force && !Race_if(PM_ANCIPITAL) && (Luck - luckpenalty + 5 + (issoviet ? 0 : rnd(20)) ) > rn2(20)) {
			/*  chance per item of sustaining damage:
			 *	max luck (full moon):	 5%
			 *	max luck (elsewhen):	10%
			 *	avg luck (Luck==0):	75%
			 *	awful luck (Luck<-4):  100%
			 *  If this is the Lethe, things are much worse.
			 *  Amy edit: extra rnd(20) boost for the chance, so a single misstep won't necessarily blank everything
			 */
			continue;
		} else if (obj->blessed && rn2(issoviet ? 2 : 4) ) { /* blessed items get an extra saving throw --Amy */
			continue;
		/* An oil skin cloak protects your body armor  */
		} else if( obj->oclass == ARMOR_CLASS
			&& obj == uarm
			&& uarmc
			&& (uarmc->otyp == OILSKIN_CLOAK || itemhasappearance(uarmc, APP_TARPAULIN_CLOAK))
			&& (!uarmc->cursed || rn2(3))) {
		    continue;
		} else {
		    /* The Lethe strips blessed and cursed status... */
		    if (level.flags.lethe) {
			/* Amy edit: you cannot easily uncurse everything, sorry */
			if (obj->otyp == POT_WATER) uncurse(obj, TRUE);
			unbless(obj);
		    }

		    switch (obj->oclass) {
		    case SCROLL_CLASS:
			if (finalcancelled(obj)) break;
			if  (obj->otyp != SCR_HEALING && obj->otyp != SCR_EXTRA_HEALING && obj->otyp != SCR_HEAL_OTHER && obj->otyp != SCR_STANDARD_ID && obj->otyp != SCR_MANA && obj->otyp != SCR_GREATER_MANA_RESTORATION && obj->otyp != SCR_CURE && obj->otyp != SCR_PHASE_DOOR
#ifdef MAIL
		    && obj->otyp != SCR_MAIL
#endif
			)
		    {
			    /* The Lethe sometimes does a little rewrite */
			    obj->otyp = (level.flags.lethe && !rn2(10)) ?
					SCR_AMNESIA : SCR_BLANK_PAPER;
			if (obj->otyp == SCR_AMNESIA) {
				obj->blessed = FALSE;
				obj->cursed = TRUE;
				curse(obj);
				curse(obj);
				obj->finalcancel = TRUE;
			}
			obj->spe = 0;
		    }
			break;
		    case SPBOOK_CLASS:
			if (finalcancelled(obj)) break;
			/* Spell books get blanked... */
			if (obj->otyp == SPE_BOOK_OF_THE_DEAD)
				pline("Steam rises from %s.", the(xname(obj)));
			else obj->otyp = SPE_BLANK_PAPER;
			break;
		    case POTION_CLASS:
			if (finalcancelled(obj)) break;
			if (obj->otyp == POT_ACID) {
				/* damage player/monster? */
				pline("A potion explodes!");
				delobj(obj);
				continue;
			} else
			/* Potions turn to water or amnesia... */
			if (level.flags.lethe) {
			    if (obj->otyp == POT_WATER) {
				obj->otyp = POT_AMNESIA;
				obj->blessed = FALSE;
				obj->cursed = TRUE;
				curse(obj);
				curse(obj);
				obj->finalcancel = TRUE;
			    }
			    else if (obj->otyp != POT_AMNESIA) {
				obj->otyp = POT_WATER;
				obj->odiluted = 0;
			    }
			} else if (obj->odiluted || obj->otyp == POT_AMNESIA) {
				obj->otyp = POT_WATER;
				obj->blessed = obj->cursed = obj->hvycurse = obj->prmcurse = obj->morgcurse = obj->evilcurse = obj->bbrcurse = obj->stckcurse = 0;
				obj->odiluted = 0;
			} else if (obj->otyp != POT_WATER)
				obj->odiluted++;
			break;
		    case GEM_CLASS:
			if (level.flags.lethe && (obj->otyp == LUCKSTONE
					|| obj->otyp == LOADSTONE
					|| obj->otyp == HEALTHSTONE
					|| obj->otyp == MANASTONE
					|| obj->otyp == SLEEPSTONE
					|| obj->otyp == LOADBOULDER
					|| obj->otyp == STARLIGHTSTONE
					|| obj->otyp == STONE_OF_MAGIC_RESISTANCE
					|| obj->otyp == TOUCHSTONE)) {
			    obj->otyp = FLINT;
				recalc_health();
				recalc_mana();
			}
			break;
		    case TOOL_CLASS:
			if (level.flags.lethe) {
			    switch (obj->otyp) {
			    case MAGIC_LAMP:
				obj->otyp = OIL_LAMP;
				obj->age = 0; /* fixing a very obvious slashem bug... --Amy */
				break;
			    case MAGIC_CANDLE:
				obj->otyp = rn2(2)? WAX_CANDLE : TALLOW_CANDLE;
				obj->age = 0; /* fixing a very obvious slashem bug... --Amy */
				break;
			    case MAGIC_WHISTLE:
				obj->otyp = TIN_WHISTLE;
				break;	
			    case MAGIC_FLUTE:
				obj->otyp = WOODEN_FLUTE;
				obj->spe  = 0;
				break;	
			    case MAGIC_HARP:
				obj->otyp = WOODEN_HARP;
				obj->spe  = 0;
				break;
			    case FIRE_HORN:
			    case FROST_HORN:
			    case TEMPEST_HORN:
			    case ETHER_HORN:
			    case CHROME_HORN:
			    case DEATH_HORN:
			    case SHADOW_HORN:
			    case HORN_OF_PLENTY:
				obj->otyp = TOOLED_HORN;
				obj->spe  = 0;
				break;
			    case DRUM_OF_EARTHQUAKE:
				obj->otyp = LEATHER_DRUM;
				obj->spe  = 0;
				break;
			    }
			}

			/* Drop through */
			/* Weapons, armor and tools may be disenchanted... */
			/* Wands and rings lose a charge... */
		    case WEAPON_CLASS:
		    case ARMOR_CLASS:
		    case WAND_CLASS:
		    case RING_CLASS:
			if ( level.flags.lethe
					&& ( obj->oclass == WEAPON_CLASS
						|| obj->oclass == ARMOR_CLASS
						|| obj->oclass == WAND_CLASS
						|| obj->oclass == RING_CLASS
						|| is_weptool(obj) )) {

			    /* Shift enchantment one step closer to 0 */
			    if (obj->spe > 0) drain_item(obj);
			}

			/* Magic markers run... */
			if ( level.flags.lethe
					&& obj->otyp == MAGIC_MARKER ) {
			    obj->spe -= (3 + rn2(10));
			    if (obj->spe < 0) obj->spe = 0;
			}

			/* Drop through for rusting effects... */
			/* Weapons, armor, tools and other things may rust... */
		    default:
			if (is_rustprone(obj) && !(objects[obj->otyp].oc_material == MT_COBALT && rn2(2)) && !(objects[obj->otyp].oc_material == MT_BRONZE && rn2(2)) && !(Race_if(PM_CHIQUAI) && rn2(4)) && !(obj->oartifact && rn2(4)) && (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ) && obj->oeroded < MAX_ERODE &&
					!(obj->oerodeproof || 
					 (obj->blessed && !rnl(4))))
				obj->oeroded++;
			else if (is_rustprone(obj) && !(objects[obj->otyp].oc_material == MT_COBALT && rn2(2)) && !(objects[obj->otyp].oc_material == MT_BRONZE && rn2(2)) && !(Race_if(PM_CHIQUAI) && rn2(4)) && !(obj->oartifact && rn2(4)) && (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ) && obj->oeroded == MAX_ERODE && !evades_destruction(obj) && !hard_to_destruct(obj) &&
					!(obj->oerodeproof ))
			{
			    
				if (!evilfriday) pline("Your %s was destroyed by rust!", xname(obj));
				else pline("One of your items was destroyed by rust!");
				if (obj == uball) unpunish();
				if (obj == uchain) unpunish();
				delobj(obj);
				update_inventory();
			    
			}


			/* The Lethe may unfooproof the item... */
			if (level.flags.lethe
					&& obj->oerodeproof && !rn2(5))
			    obj->oerodeproof = FALSE;
		    }
		}
	}
}

void
actual_lethe_damage(obj, force, here)
register struct obj *obj;
register boolean force, here;
{
	struct obj *otmp;

	/* Scrolls, spellbooks, potions, weapons and
	   pieces of armor may get affected by the water */
	for (; obj; obj = otmp) {
		otmp = here ? obj->nexthere : obj->nobj;

		(void) snuff_lit(obj);

		if (stack_too_big(obj) && !issoviet) continue;
		if (rn2(10)) continue; /* used to affect everything unconditionally but that was way too harsh --Amy */
		if (obj->blessed && rn2(issoviet ? 2 : 4) ) { /* blessed items get an extra saving throw --Amy */
			continue;
		}

		{
			if (obj->otyp == POT_WATER) uncurse(obj, TRUE);
			unbless(obj);

		    switch (obj->oclass) {
		    case SCROLL_CLASS:
			if (finalcancelled(obj)) break;
			if  (!rn2(10) && obj->otyp != SCR_HEALING && obj->otyp != SCR_HEAL_OTHER && obj->otyp != SCR_EXTRA_HEALING && obj->otyp != SCR_STANDARD_ID && obj->otyp != SCR_MANA && obj->otyp != SCR_GREATER_MANA_RESTORATION && obj->otyp != SCR_CURE && obj->otyp != SCR_PHASE_DOOR
#ifdef MAIL
		    && obj->otyp != SCR_MAIL
#endif
			)
		    {
			obj->otyp = SCR_AMNESIA;
			obj->blessed = FALSE;
			obj->cursed = TRUE;
			curse(obj);
			curse(obj);
			obj->finalcancel = TRUE;
			obj->spe = 0;
		    }
			break;
		    case SPBOOK_CLASS:
			break;
		    case POTION_CLASS:
			if (finalcancelled(obj)) break;
			if (obj->otyp == POT_WATER || !rn2(10)) {
				obj->otyp = POT_AMNESIA;
				obj->blessed = FALSE;
				obj->cursed = TRUE;
				curse(obj);
				curse(obj);
				obj->finalcancel = TRUE;
			}
			break;
		    case GEM_CLASS:
			if ((obj->otyp == LUCKSTONE
					|| obj->otyp == HEALTHSTONE
					|| obj->otyp == MANASTONE
					|| obj->otyp == STONE_OF_MAGIC_RESISTANCE
					|| obj->otyp == TOUCHSTONE)) {
			    obj->otyp = FLINT;
				recalc_health();
				recalc_mana();
			}
			break;
		    case TOOL_CLASS:
			    switch (obj->otyp) {
			    case MAGIC_LAMP:
				obj->otyp = OIL_LAMP;
				obj->age = 0; /* fixing a very obvious slashem bug... --Amy */
				break;
			    case MAGIC_CANDLE:
				obj->otyp = rn2(2)? WAX_CANDLE : TALLOW_CANDLE;
				obj->age = 0; /* fixing a very obvious slashem bug... --Amy */
				break;
			    case MAGIC_WHISTLE:
				obj->otyp = TIN_WHISTLE;
				break;	
			    case MAGIC_FLUTE:
				obj->otyp = WOODEN_FLUTE;
				obj->spe  = 0;
				break;	
			    case MAGIC_HARP:
				obj->otyp = WOODEN_HARP;
				obj->spe  = 0;
				break;
			    case FIRE_HORN:
			    case FROST_HORN:
			    case TEMPEST_HORN:
			    case ETHER_HORN:
			    case CHROME_HORN:
			    case DEATH_HORN:
			    case SHADOW_HORN:
			    case HORN_OF_PLENTY:
				obj->otyp = TOOLED_HORN;
				obj->spe  = 0;
				break;
			    case DRUM_OF_EARTHQUAKE:
				obj->otyp = LEATHER_DRUM;
				obj->spe  = 0;
				break;
			    }

		    case WEAPON_CLASS:
		    case ARMOR_CLASS:
		    case WAND_CLASS:
		    case RING_CLASS:
			if (( obj->oclass == WEAPON_CLASS
					|| obj->oclass == ARMOR_CLASS
					|| obj->oclass == WAND_CLASS
					|| obj->oclass == RING_CLASS
					|| is_weptool(obj) )) {

			    /* Shift enchantment one step closer to 0 */
			    if (obj->spe > 0) drain_item(obj);
			}

			/* Magic markers run... */
			if (obj->otyp == MAGIC_MARKER ) {
			    obj->spe -= (3 + rn2(10));
			    if (obj->spe < 0) obj->spe = 0;
			}

			if (obj->oerodeproof && !rn2(5))
			    obj->oerodeproof = FALSE;
		    }
		}
	}
}

/* A function that can damage all of the player's items. --Amy */
void
withering_damage(obj, force, here)
register struct obj *obj;
register boolean force, here;
{
	struct obj *otmp;

	/* Scrolls, spellbooks, potions, weapons and
	   pieces of armor may get affected by the water */
	for (; obj; obj = otmp) {
		otmp = here ? obj->nexthere : obj->nobj;

		(void) snuff_lit(obj);

		if (stack_too_big(obj)) continue;

		if (Race_if(PM_CHIQUAI) && rn2(4)) continue;

		if (is_unwitherable(obj)) continue;

		if (itemhasappearance(obj, APP_BRAND_NEW_GLOVES) && rn2(2) ) continue;

		if (itemhasappearance(obj, APP_IMAGINARY_HEELS) ) continue;

		if (obj->oartifact == ART_SARTIRO ) continue;
		if (obj->oartifact == ART_CUMBERSOME_DESC ) continue;
		if (obj->oartifact == ART_GOODRES_ELVEN ) continue;
		if (obj->oartifact == ART_PARTICULARLY_SOLID_SKULL ) continue;

		if (obj->oartifact == ART_EXTENDED_DURABILITY && rn2(4)) continue;

		if (uarm && uarm->oartifact == ART_CANNOT_BE_HARMED_BLA_BLA && (obj->owornmask & W_ARMOR) ) continue;

		if (uwep && uwep->oartifact == ART_SLAM_ && rn2(10) && (obj->owornmask & W_ARMOR) ) continue;

		if (obj->oartifact == ART_PROOFINGNESS_POOFS && !obj->rknown) continue;

		if (obj->oartifact == ART_RATCH_CLOSURE_SCRATCHING && rn2(4) ) continue;

		if (itemhasappearance(obj, APP_WITHERED_CLOAK) ) continue;

		/* important quest items are immune */
		if (obj->otyp == SPE_BOOK_OF_THE_DEAD || obj->otyp == AMULET_OF_YENDOR || obj->otyp == CANDELABRUM_OF_INVOCATION || obj->otyp == BELL_OF_OPENING || obj->oartifact == ART_KEY_OF_LAW || obj->oartifact == ART_KEY_OF_NEUTRALITY || obj->oartifact == ART_KEY_OF_CHAOS || obj->oartifact == ART_GAUNTLET_KEY) continue;

		if (rn2(2)) {

			if (obj->oeroded < MAX_ERODE && !(obj->oartifact && rn2(4)) && (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ) && !( (obj->blessed && !rnl(4))))
				obj->oeroded++;
			else if (obj->oeroded == MAX_ERODE && !evades_destruction(obj) && !hard_to_destruct(obj) && !(obj->oartifact && rn2(4)) && (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ))
			{
			    
				if (!evilfriday) pline("Your %s withered away!", xname(obj));
				else pline("One of your items withered away!");
				delobj(obj);
				update_inventory();
			    
			}
		} else {

			if (obj->oeroded2 < MAX_ERODE && !(obj->oartifact && rn2(4)) && (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ) && !( (obj->blessed && !rnl(4))))
				obj->oeroded2++;
			else if (obj->oeroded2 == MAX_ERODE && !evades_destruction(obj) && !hard_to_destruct(obj) && !(obj->oartifact && rn2(4)) && (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ))
			{
			    
				if (!evilfriday) pline("Your %s withered away!", xname(obj));
				else pline("One of your items withered away!");
				delobj(obj);
				update_inventory();
			    
			}
		}
	}
}

void
antimatter_damage(obj, force, here)
register struct obj *obj;
register boolean force, here;
{
	struct obj *otmp;

	/* Scrolls, spellbooks, potions, weapons and
	   pieces of armor may get affected by the water */
	for (; obj; obj = otmp) {
		otmp = here ? obj->nexthere : obj->nobj;

		(void) snuff_lit(obj);

		if (stack_too_big(obj)) continue;

		if (is_unwitherable(obj)) continue;

		if (Race_if(PM_CHIQUAI) && rn2(4)) continue;

		if (rn2(10)) continue;

		if (obj->oartifact == ART_WASTEFUL_PLAYER) continue; /* so it doesn't damage itself */

		if (itemhasappearance(obj, APP_BRAND_NEW_GLOVES) && rn2(2) ) continue;

		if (itemhasappearance(obj, APP_IMAGINARY_HEELS) ) continue;

		if (obj->oartifact == ART_SARTIRO ) continue;
		if (obj->oartifact == ART_CUMBERSOME_DESC ) continue;
		if (obj->oartifact == ART_GOODRES_ELVEN ) continue;
		if (obj->oartifact == ART_PARTICULARLY_SOLID_SKULL ) continue;

		if (obj->oartifact == ART_EXTENDED_DURABILITY && rn2(4)) continue;

		if (uarm && uarm->oartifact == ART_CANNOT_BE_HARMED_BLA_BLA && (obj->owornmask & W_ARMOR) ) continue;

		if (uwep && uwep->oartifact == ART_SLAM_ && rn2(10) && (obj->owornmask & W_ARMOR) ) continue;

		if (obj->oartifact == ART_PROOFINGNESS_POOFS && !obj->rknown) continue;

		if (obj->oartifact == ART_RATCH_CLOSURE_SCRATCHING && rn2(4) ) continue;

		if (itemhasappearance(obj, APP_WITHERED_CLOAK) ) continue;

		/* important quest items are immune */
		if (obj->otyp == SPE_BOOK_OF_THE_DEAD || obj->otyp == AMULET_OF_YENDOR || obj->otyp == CANDELABRUM_OF_INVOCATION || obj->otyp == BELL_OF_OPENING || obj->oartifact == ART_KEY_OF_LAW || obj->oartifact == ART_KEY_OF_NEUTRALITY || obj->oartifact == ART_KEY_OF_CHAOS || obj->oartifact == ART_GAUNTLET_KEY) continue;

		if (rn2(2)) {

			if (obj->oeroded < MAX_ERODE && !(obj->oartifact && rn2(4)) && (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ) && !( (obj->blessed && !rnl(4))))
				obj->oeroded++;
			else if (obj->oeroded == MAX_ERODE && !(obj->oartifact && rn2(4)) && !evades_destruction(obj) && !hard_to_destruct(obj) && (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ))
			{
			    
				if (!evilfriday) pline("Your %s withered away!", xname(obj));
				else pline("One of your items withered away!");
				delobj(obj);
				update_inventory();
			    
			}
		} else {

			if (obj->oeroded2 < MAX_ERODE && !(obj->oartifact && rn2(4)) && (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ) && !( (obj->blessed && !rnl(4))))
				obj->oeroded2++;
			else if (obj->oeroded2 == MAX_ERODE && !(obj->oartifact && rn2(4)) && !evades_destruction(obj) && !hard_to_destruct(obj) && (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ))
			{
			    
				if (!evilfriday) pline("Your %s withered away!", xname(obj));
				else pline("One of your items withered away!");
				delobj(obj);
				update_inventory();
			    
			}
		}
	}
}

/*
 * This function is potentially expensive - rolling
 * inventory list multiple times.  Luckily it's seldom needed.
 * Returns TRUE if disrobing made player unencumbered enough to
 * crawl out of the current predicament.
 */
STATIC_OVL boolean
emergency_disrobe(lostsome)
boolean *lostsome;
{
	int invc = inv_cnt();

	while (near_capacity() > (Punished ? UNENCUMBERED : SLT_ENCUMBER)) {
	    register struct obj *obj, *otmp = (struct obj *)0;
	    register int i;

	    /* Pick a random object */
	    if (invc > 0) {
		i = rn2(invc);
		for (obj = invent; obj; obj = obj->nobj) {
		    /*
		     * Undroppables are: body armor, boots, gloves,
		     * amulets, and rings because of the time and effort
		     * in removing them + loadstone and other cursed stuff
		     * for obvious reasons. Amy edit: and the amulet of yendor, you filthy cheater :-P
		     */
		    if (!(( (obj->otyp == LOADSTONE || obj->otyp == LUCKSTONE || obj->otyp == HEALTHSTONE || obj->otyp == MANASTONE || obj->otyp == SLEEPSTONE || obj->otyp == LOADBOULDER || (obj->otyp == AMULET_OF_YENDOR && !u.freeplaymode) || (obj->otyp == FAKE_AMULET_OF_YENDOR && !u.freeplaymode) || obj->otyp == STARLIGHTSTONE || obj->otyp == STONE_OF_MAGIC_RESISTANCE || is_nastygraystone(obj) || is_feminismstone(obj) ) && (obj->cursed || (obj->otyp == AMULET_OF_YENDOR && !u.freeplaymode) || (obj->otyp == FAKE_AMULET_OF_YENDOR && !u.freeplaymode) ) ) ||
			  obj == uamul || obj == uimplant || obj == uleft || obj == uright ||
			  obj == ublindf || obj == uarm || obj == uarmc ||
			  obj == uarmg || obj == uarmf ||
			  obj == uarmu ||
			  (obj->otyp == LUCKSTONE && isevilvariant && !obj->cursed && !obj->blessed && Luck < 0) ||
			  (obj->cursed && (obj == uarmh || obj == uarms)) ||
			  welded(obj)))
			otmp = obj;
		    /* reached the mark and found some stuff to drop? */
		    if (--i < 0 && otmp) break;

		    /* else continue */
		}
	    }
#ifndef GOLDOBJ
	    if (!otmp) {
		/* Nothing available left to drop; try gold */
		if (u.ugold) {
		    pline("In desperation, you drop your purse.");
		    /* Hack: gold is not in the inventory, so make a gold object
		     * and put it at the head of the inventory list.
		     */
		    obj = mkgoldobj(u.ugold);    /* removes from u.ugold */
		    obj->in_use = TRUE;
		    u.ugold = obj->quan;         /* put the gold back */
		    assigninvlet(obj);           /* might end up as NOINVSYM */
		    obj->nobj = invent;
		    invent = obj;
		    *lostsome = TRUE;
		    dropx(obj);
		    continue;                    /* Try again */
		}
		/* We can't even drop gold! */
		return (FALSE);
	    }
#else
	    if (!otmp) return (FALSE); /* nothing to drop! */	
#endif
	    if (otmp->owornmask) remove_worn_item(otmp, FALSE);
	    *lostsome = TRUE;
	    dropx(otmp);
	    invc--;
	}
	return(TRUE);
}

/*
 *  return(TRUE) == player relocated
 */
boolean
drown()
{
	boolean inpool_ok = FALSE, crawl_ok;
	int i, x, y;
	const char *sparkle = level.flags.lethe? "sparkling " : "";

	if (uarm && uarm->oartifact == ART_WEGEO_ACQUA_DE_EISU_FORTE) {
		if (isok(u.ux, u.uy) && (levl[u.ux][u.uy].typ == WATER || levl[u.ux][u.uy].typ == POOL || levl[u.ux][u.uy].typ == MOAT)) {
			levl[u.ux][u.uy].typ = ICE;
			Norep("The water beneath you freezes.");
		}
		return FALSE;
	}

	/* happily wading in the same contiguous pool */
	if (u.uinwater && !(is_crystalwater(u.ux-u.dx,u.uy-u.dy)) && is_drowningpool(u.ux-u.dx,u.uy-u.dy) &&
	    (Swimming || Amphibious || Breathless || (u.usteed && is_swimmer(u.usteed->data)) )) {
		/* water effects on objects every now and then */
		u.udrowning = FALSE;
		if (!rn2(5)) inpool_ok = TRUE;
		else return(FALSE);
	}

	if (!u.uinwater) {
	    You("%s into the %swater%c",
		Is_waterlevel(&u.uz) ? "plunge" : "fall",
		sparkle,
		Amphibious || Breathless || Swimming ? '.' : '!');
	    if (!Swimming && !Is_waterlevel(&u.uz))
		    You("sink like %s.",
			FunnyHallu ? "the Titanic" : "a rock");

	    if (Swimming) pline("Thankfully you know how to swim, so you can stay on the surface.");
	}

	if (level.flags.lethe) {
	    /* Bad idea */
	    You_feel("the sparkling waters of the Lethe sweep away your cares!");
	    forget(5, FALSE); /* used to be 25 --Amy */
	}

	if ((!StrongSwimming || !rn2(10)) && !(u.usteed && is_swimmer(u.usteed->data)) && (!StrongMagical_breathing || !rn2(10))) {
		water_damage(invent, FALSE, FALSE);
		if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);
	}
	if (Burned) make_burned(0L, TRUE);

	if (splittinggremlin(youmonst.data) && rn2(3))
	    (void)split_mon(&youmonst, (struct monst *)0);
	else if (u.umonnum == PM_IRON_GOLEM) {
	    You("rust!");
	    i = d(2,6);
	    if (u.mhmax > i) u.mhmax -= i;
	    losehp(i, "rusting away", KILLED_BY);
	}
	if (inpool_ok) {
		u.udrowning = FALSE;
		return(FALSE);
	}

	if ((i = number_leashed()) > 0) {
		pline_The("leash%s slip%s loose.",
			(i > 1) ? "es" : "",
			(i > 1) ? "" : "s");
		unleash_all();
	}

	if (Amphibious || Breathless || Swimming) {
		if (Amphibious) {
			if (flags.verbose)
				pline("But you aren't drowning.");
			if (!Is_waterlevel(&u.uz)) {
				if (FunnyHallu || (Role_if(PM_PIRATE) || Role_if(PM_KORSAIR) || PirateSpeakOn) )
					Your("keel hits the bottom.");
				else
					You("touch bottom.");
			}
		}
		if (Punished) {
			unplacebc();
			placebc();
		}
		vision_recalc(2);	/* unsee old position */
		u.uinwater = 1;
		if (!Swimming) under_water(1);
		vision_full_recalc = 1;
		u.udrowning = FALSE;
		return(FALSE);
	}
	else if (Swimming && !Is_waterlevel(&u.uz)) {
		if (Punished) {
			unplacebc();
			placebc();
		}
		u.uinwater = 1;
		if (!Swimming) under_water(1);
		vision_full_recalc = 1;
		u.udrowning = FALSE;
		return(FALSE);
	}
	if ((Teleportation || can_teleport(youmonst.data)) &&
		    !u.usleep && (Teleport_control || rn2(3) < Luck+2)) {
		You("attempt a teleport spell.");	/* utcsri!carroll */
		if (!level.flags.noteleport && !Race_if(PM_STABILISATOR) && !u.antitelespelltimeout) {
			(void) dotele_post(FALSE);
			if(!is_drowningpool(u.ux,u.uy))
				return(TRUE);
		} else pline_The("attempted teleport spell fails.");
	}
	if (u.usteed && !(is_swimmer(u.usteed->data)) ) {
		dismount_steed(DISMOUNT_GENERIC);
		if(!is_drowningpool(u.ux,u.uy))
			return(TRUE);
		if(is_crystalwater(u.ux,u.uy))
			return(TRUE);
	}
	crawl_ok = FALSE;
	x = y = 0;		/* lint suppression */
	/* if sleeping, wake up now so that we don't crawl out of water
	   while still asleep; we can't do that the same way that waking
	   due to combat is handled; note unmul() clears u.usleep */
	if (u.usleep) unmul("Suddenly you wake up!");
	/* can't crawl if unable to move (crawl_ok flag stays false) */
	if (multi < 0 || (Upolyd && !youmonst.data->mmove)) goto crawl;
	/* look around for a place to crawl to */
	for (i = 0; i < 100; i++) {
		x = rn1(3,u.ux - 1);
		y = rn1(3,u.uy - 1);
		if (goodpos(x, y, &youmonst, MM_CRYSTALORNOT)) {
			crawl_ok = TRUE;
			goto crawl;
		}
	}
	/* one more scan */
	for (x = u.ux - 1; x <= u.ux + 1; x++)
		for (y = u.uy - 1; y <= u.uy + 1; y++)
			if (goodpos(x, y, &youmonst, MM_CRYSTALORNOT)) {
				crawl_ok = TRUE;
				goto crawl;
			}
 crawl:
	if (crawl_ok) {
		boolean lost = FALSE;
		/* time to do some strip-tease... */
		boolean succ = Is_waterlevel(&u.uz) ? TRUE :
				emergency_disrobe(&lost);

		if (uwep && uwep->oartifact == ART_LAKITU_GET_ME_OUT) succ = TRUE;

		You("try to crawl out of the water.");
		if (lost)
			You("dump some of your gear to lose weight...");
		if (succ) {
			pline("Pheew!  That was close.");
			teleds(x,y,TRUE);
			return(TRUE);
		}
		/* still too much weight */
		pline("But in vain.");
	}
	u.uinwater = 1;

	if (uarmf && itemhasappearance(uarmf, APP_FIN_BOOTS) ) {
		pline("Your fin boots prevent you from drowning.");
		u.udrowning = FALSE;
		return(FALSE);
	}

	if (uwep && uwep->oartifact == ART_LAKITU_GET_ME_OUT) {
		pline("Dieau-dieau-dieau-dieau-dieau! Lakitu comes and fishes you out of the water.");
		safe_teleds_normalterrain(TRUE);
		return TRUE;
	}

	if (Role_if(PM_PIRATE) || Role_if(PM_KORSAIR) || PirateSpeakOn) You("go to Davy Jones' locker.");
	else You("drown.");

	if (PlayerHearsSoundEffects) pline(issoviet ? "Nikto ne znayet, pochemu ty byl nastol'ko glup, chtoby upast' v vodu, no eto ne imeyet nikakogo znacheniya, v lyubom sluchaye, potomu chto vy mozhete svernut' novogo personazha pryamo seychas." : "HUAAAAAAA-A-AAAAHHHHHH!");

	/* [ALI] Vampires return to vampiric form on drowning.
	 */
	if (Upolyd && !Unchanging && Race_if(PM_VAMPIRE)) {
		rehumanize();
		u.uinwater = 0;
		You("fly up out of the water!");
		u.udrowning = FALSE;
		return (TRUE);
	}

	if (u.uhpmax >= 100) {
		u.udrowning = TRUE;
		You("inhale an unhealthy amount of water.");
		if (Upolyd) losehp(100, "drowning", NO_KILLER_PREFIX);
		losehp(rnd(100), "drowning", NO_KILLER_PREFIX);
		vision_recalc(2);	/* unsee old position */
		u.uinwater = 1;
		if (!Swimming) under_water(1);
		vision_full_recalc = 1;
		return(FALSE);
	}

	u.youaredead = 1;
	killer_format = KILLED_BY_AN;
	killer = (levl[u.ux][u.uy].typ == POOL || Is_medusa_level(&u.uz)) ?
	    "pool of water" : "moat";
	done(DROWNING);
	u.youaredead = 0;
	/* oops, we're still alive.  better get out of the water. */
	while (!safe_teleds_normalterrain(TRUE)) {
		u.youaredead = 1;
		pline("You're still drowning.");
		done(DROWNING);
		u.youaredead = 0;
	}
	if (u.uinwater) {
	u.uinwater = 0;
	You("find yourself back %s.", Is_waterlevel(&u.uz) ?
		"in an air bubble" : "on land");
	}
	return(TRUE);
	
}

boolean
crystaldrown()
{
	boolean inpool_ok = FALSE, crawl_ok;
	int i, x, y;
	const char *sparkle = level.flags.lethe? "sparkling " : "";

	if (Swimming || Amphibious || Breathless) inpool_ok = TRUE;

	if (level.flags.lethe) {
	    /* Bad idea */
	    You_feel("the sparkling waters of the Lethe sweep away your cares!");
	    forget(5, FALSE); /* used to be 25 --Amy */
	}

	if ((!StrongSwimming || !rn2(10)) && (!StrongMagical_breathing || !rn2(10))) {
		water_damage(invent, FALSE, FALSE);
		if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);
	}
	if (Burned) make_burned(0L, TRUE);

	if (splittinggremlin(youmonst.data) && rn2(3))
	    (void)split_mon(&youmonst, (struct monst *)0);
	else if (u.umonnum == PM_IRON_GOLEM) {
	    You("rust!");
	    i = d(2,6);
	    if (u.mhmax > i) u.mhmax -= i;
	    losehp(i, "rusting away", KILLED_BY);
	}
	if (inpool_ok) {
		u.udrowning = FALSE;
		return(FALSE);
	}

	if ((Teleportation || can_teleport(youmonst.data)) &&
		    !u.usleep && (Teleport_control || rn2(3) < Luck+2)) {
		You("attempt a teleport spell.");	/* utcsri!carroll */
		if (!level.flags.noteleport && !Race_if(PM_STABILISATOR) && !u.antitelespelltimeout) {
			(void) dotele_post(FALSE);
		} else pline_The("attempted teleport spell fails.");
	}
	crawl_ok = FALSE;
	x = y = 0;		/* lint suppression */
	/* if sleeping, wake up now so that we don't crawl out of water
	   while still asleep; we can't do that the same way that waking
	   due to combat is handled; note unmul() clears u.usleep */
	if (u.usleep) unmul("Suddenly you wake up!");
	/* can't crawl if unable to move (crawl_ok flag stays false) */
	if (multi < 0 || (Upolyd && !youmonst.data->mmove)) goto crawl;
	/* look around for a place to crawl to */
	for (i = 0; i < 100; i++) {
		x = rn1(3,u.ux - 1);
		y = rn1(3,u.uy - 1);
		if (goodpos(x, y, &youmonst, MM_CRYSTALORNOT)) {
			crawl_ok = TRUE;
			goto crawl;
		}
	}
	/* one more scan */
	for (x = u.ux - 1; x <= u.ux + 1; x++)
		for (y = u.uy - 1; y <= u.uy + 1; y++)
			if (goodpos(x, y, &youmonst, MM_CRYSTALORNOT)) {
				crawl_ok = TRUE;
				goto crawl;
			}
 crawl:
	if (crawl_ok) {
		boolean lost = FALSE;
		/* time to do some strip-tease... */
		boolean succ = Is_waterlevel(&u.uz) ? TRUE :
				emergency_disrobe(&lost);

		if (uwep && uwep->oartifact == ART_LAKITU_GET_ME_OUT) succ = TRUE;

		You("try to fly out of the water.");
		if (lost)
			You("dump some of your gear to lose weight...");
		if (succ) {
			pline("Pheew!  That was close.");
			teleds(x,y,TRUE);
			return(TRUE);
		}
		/* still too much weight */
		pline("But in vain.");
	}

	if (uarmf && itemhasappearance(uarmf, APP_FIN_BOOTS) ) {
		pline("Your fin boots prevent you from drowning.");
		u.udrowning = FALSE;
		return(FALSE);
	}

	if (uwep && uwep->oartifact == ART_LAKITU_GET_ME_OUT) {
		pline("Dieau-dieau-dieau-dieau-dieau! Lakitu comes and fishes you out of the water.");
		safe_teleds_normalterrain(TRUE);
		return TRUE;
	}

	u.youaredead = 1;

	You("drown.");
	if (PlayerHearsSoundEffects) pline(issoviet ? "Nikto ne znayet, pochemu ty byl nastol'ko glup, chtoby upast' v vodu, no eto ne imeyet nikakogo znacheniya, v lyubom sluchaye, potomu chto vy mozhete svernut' novogo personazha pryamo seychas." : "HUAAAAAAA-A-AAAAHHHHHH!");

	if (u.uhpmax >= 100) {
		u.udrowning = TRUE;
		You("inhale an unhealthy amount of water.");
		if (FunnyHallu) pline("Sadly, despite being crystal clear, it's still killing you.");
		if (Upolyd) losehp(100, "drowned in crystal water", NO_KILLER_PREFIX);
		losehp(rnd(100), "drowned in crystal water", NO_KILLER_PREFIX);
		vision_recalc(2);	/* unsee old position */
		u.uinwater = 1;
		if (!Swimming) under_water(1);
		vision_full_recalc = 1;
		return(FALSE);
	}

	killer_format = KILLED_BY_AN;
	killer = "crystal water";
	done(DROWNING);
	u.youaredead = 0;
	/* oops, we're still alive.  better get out of the water. */
	while (!safe_teleds_normalterrain(TRUE)) {
		u.youaredead = 1;
		pline("You're still drowning.");
		done(DROWNING);
		u.youaredead = 0;
	}
	return(TRUE);
	

}

void
drain_en(n)
register int n;
{
	int maxenloss;

	if (Race_if(PM_PLAYER_NIBELUNG)) {
		if (rn2(5)) {
			You("feel something trying to drain your magical energy, but resist the effect.");
			return;
		} else {
			n /= rnd(5);
			if (n < 1) n = 1;
		}
	}

	if (uarmf && itemhasappearance(uarmf, APP_CROSS_COUNTRY_BOOTS) && !rn2(5)) {
		pline("Thanks to your cross country boots, you resist the mana drain.");
		return;
	}

	if (!u.uenmax) return;
	You_feel("your magical energy drain away!");
	u.uen -= n;
	if(u.uen < 0)  {

		/* total nerf, because max Pw is hard to get back --Amy */
		maxenloss = -u.uen;
		maxenloss = rnd(maxenloss);
		maxenloss = rnd(maxenloss);
		if (rn2(2)) maxenloss = rn2(maxenloss);

		u.uenmax -= maxenloss;
		if(u.uenmax < 0) u.uenmax = 0;
		u.uen = 0;
	}
	if (u.uen < 0) u.uen = 0;
	if (u.uen > u.uenmax) u.uen = u.uenmax;

	flags.botl = 1;
}

int
dountrap()	/* disarm a trap */
{
	if (near_capacity() >= HVY_ENCUMBER) {
	    pline("You're too strained to do that.");
	    return 0;
	}
	if ((nohands(youmonst.data) && !webmaker(youmonst.data) && !Race_if(PM_TRANSFORMER) ) || !youmonst.data->mmove) {
	    pline("And just how do you expect to do that?");
		if (yn("Attempt it anyway?") == 'y') {
			if (rn2(3) && !polyskillchance()) {
				incr_itimeout(&Glib, rnd(40));
				pline("You failed, and your %s became very slippery.", makeplural(body_part(HAND)));
				if (!rn2(20)) badeffect();
				return 1;
			}

		}
		else return 0;
	}
	if (u.ustuck && sticks(youmonst.data)) {
	    pline("You'll have to let go of %s first.", mon_nam(u.ustuck));
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    return 0;
	}
	if (u.ustuck || (welded(uwep) && bimanual(uwep))) {
	    Your("%s seem to be too busy for that.",
		 makeplural(body_part(HAND)));
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    return 0;
	}
	return untrap(FALSE);
}
#endif /* OVLB */
#ifdef OVL2

/* Probability of disabling a trap.  Helge Hafting; Amy keyword "newtraps" */
STATIC_OVL int
untrap_prob(ttmp)
struct trap *ttmp;
{
	int chance = 3;

	/* Only spiders know how to deal with webs reliably */
	if ((ttmp->ttyp == WEB || ttmp->ttyp == FARTING_WEB) && !webmaker(youmonst.data) && !Race_if(PM_SPIDERMAN) )
	 	chance = 30;
	if (ttmp->ttyp == ACTIVE_SUPERSCROLLER_TRAP) chance = 20;
	if (ttmp->ttyp == HEEL_TRAP) chance = 10;
	if (ttmp->ttyp == ATTACKING_HEEL_TRAP) chance = 10;
	if (ttmp->ttyp == GLYPH_OF_WARDING) chance = 5;
	if (ttmp->ttyp == UNKNOWN_TRAP) chance = 5;
	if (ttmp->ttyp == SCYTHING_BLADE) chance = 4;

	if (ttmp->ttyp == ROCKTRAP) chance = 5;
	if (ttmp->ttyp == CALTROPS_TRAP) chance = 7;
	if (ttmp->ttyp == VENOM_SPRINKLER) chance = 20;
	if (ttmp->ttyp == SPEAR_TRAP) chance = 15;
	if (ttmp->ttyp == ROLLING_BOULDER_TRAP) chance = 10;
	if (ttmp->ttyp == SLP_GAS_TRAP) chance = 12;
	if (ttmp->ttyp == CHLOROFORM_TRAP) chance = 20;
	if (ttmp->ttyp == POISON_GAS_TRAP) chance = 12;
	if (ttmp->ttyp == SLOW_GAS_TRAP) chance = 12;
	if (ttmp->ttyp == TELEP_TRAP) chance = 8;
	if (ttmp->ttyp == PHASEPORTER) chance = 8;
	if (ttmp->ttyp == PHASE_BEAMER) chance = 8;
	if (ttmp->ttyp == BEAMER_TRAP) chance = 8;
	if (ttmp->ttyp == LEVEL_TELEP) chance = 20;
	if (ttmp->ttyp == LEVEL_BEAMER) chance = 20;
	if (ttmp->ttyp == BRANCH_TELEPORTER) chance = 35;
	if (ttmp->ttyp == BRANCH_BEAMER) chance = 35;
	if (ttmp->ttyp == STATUE_TRAP) chance = 12;
	if (ttmp->ttyp == SATATUE_TRAP) chance = 12;
	if (ttmp->ttyp == MAGIC_TRAP) chance = 16;
	if (ttmp->ttyp == ANTI_MAGIC) chance = 10;
	if (ttmp->ttyp == POLY_TRAP) chance = 30;
	if (ttmp->ttyp == GENETIC_TRAP) chance = 30;
	if (ttmp->ttyp == MISSINGNO_TRAP) chance = 30;
	if (ttmp->ttyp == ICE_TRAP) chance = 8;
	if (ttmp->ttyp == SHOCK_TRAP) chance = 8;
	if (ttmp->ttyp == COLLAPSE_TRAP) chance = 30;
	if (ttmp->ttyp == MAGIC_BEAM_TRAP) chance = 20;
	if (ttmp->ttyp == PIERCING_BEAM_TRAP) chance = 20;
	if (ttmp->ttyp == SHIT_TRAP) chance = 10;
	if (ttmp->ttyp == ANIMATION_TRAP) chance = 15;
	if (ttmp->ttyp == RETURN_TRAP) chance = 15;

	if (ttmp->ttyp == INTRINSIC_STEAL_TRAP) chance = 20;
	if (ttmp->ttyp == SCORE_AXE_TRAP) chance = 30;
	if (ttmp->ttyp == SINGLE_UNIDENTIFY_TRAP) chance = 10;
	if (ttmp->ttyp == UNLUCKY_TRAP) chance = 12;
	if (ttmp->ttyp == ALIGNMENT_REDUCTION_TRAP) chance = 8;
	if (ttmp->ttyp == MALIGNANT_TRAP) chance = 16;
	if (ttmp->ttyp == STAT_DAMAGE_TRAP) chance = 4;
	if (ttmp->ttyp == HALF_MEMORY_TRAP) chance = 12;
	if (ttmp->ttyp == HALF_TRAINING_TRAP) chance = 18;
	if (ttmp->ttyp == NARCOLEPSY_TRAP) chance = 5;

	if (ttmp->ttyp == VULN_TRAP) chance = 12;
	if (ttmp->ttyp == GRAVITY_TRAP) chance = 14;
	if (ttmp->ttyp == LOCK_TRAP) chance = 10;
	if (ttmp->ttyp == WRENCHING_TRAP) chance = 10;
	if (ttmp->ttyp == NEGATIVE_TRAP) chance = 10;
	if (ttmp->ttyp == MAGIC_CANCELLATION_TRAP) chance = 14;
	if (ttmp->ttyp == CYANIDE_TRAP) chance = 40;
	if (ttmp->ttyp == PHOSGENE_TRAP) chance = 30;
	if (ttmp->ttyp == LASER_TRAP) chance = 16;
	if (ttmp->ttyp == CONFUSE_TRAP) chance = 10;
	if (ttmp->ttyp == STUN_TRAP) chance = 10;
	if (ttmp->ttyp == DIMNESS_TRAP) chance = 10;
	if (ttmp->ttyp == HALLUCINATION_TRAP) chance = 12;
	if (ttmp->ttyp == NUMBNESS_TRAP) chance = 8;
	if (ttmp->ttyp == FREEZING_TRAP) chance = 20;
	if (ttmp->ttyp == BURNING_TRAP) chance = 14;
	if (ttmp->ttyp == FEAR_TRAP) chance = 11;
	if (ttmp->ttyp == BLINDNESS_TRAP) chance = 9;
	if (ttmp->ttyp == GLIB_TRAP) chance = 16;
	if (ttmp->ttyp == UNLIGHT_TRAP) chance = 20;
	if (ttmp->ttyp == ELEMENTAL_TRAP) chance = 10;
	if (ttmp->ttyp == ESCALATING_TRAP) chance = 16;
	if (ttmp->ttyp == MANA_TRAP) chance = 12;
	if (ttmp->ttyp == CANCELLATION_TRAP) chance = 24;
	if (ttmp->ttyp == HOSTILITY_TRAP) chance = 16;
	if (ttmp->ttyp == FALLING_BOULDER_TRAP) chance = 30;
	if (ttmp->ttyp == OUT_OF_MAGIC_TRAP) chance = 18;
	if (ttmp->ttyp == PLASMA_TRAP) chance = 24;
	if (ttmp->ttyp == BOMB_TRAP) chance = 16;
	if (ttmp->ttyp == EARTHQUAKE_TRAP) chance = 24;
	if (ttmp->ttyp == NOISE_TRAP) chance = 10;
	if (ttmp->ttyp == GLUE_TRAP) chance = 50;
	if (ttmp->ttyp == VOLT_TRAP) chance = 24;
	if (ttmp->ttyp == CORROSION_TRAP) chance = 6;
	if (ttmp->ttyp == WITHER_TRAP) chance = 30;
	if (ttmp->ttyp == FLAME_TRAP) chance = 6;
	if (ttmp->ttyp == BANANA_TRAP) chance = 8;
	if (ttmp->ttyp == FALLING_TUB_TRAP) chance = 26;
	if (ttmp->ttyp == ALARM) chance = 20;
	if (ttmp->ttyp == BLADE_WIRE) chance = 8;
	if (ttmp->ttyp == MAGNET_TRAP) chance = 24;
	if (ttmp->ttyp == CANNON_TRAP) chance = 30;
	if (ttmp->ttyp == FUMAROLE) chance = 20;
	if (ttmp->ttyp == FUMBLING_TRAP) chance = 20;
	if (ttmp->ttyp == NEXUS_TRAP) chance = 24;
	if (ttmp->ttyp == LEG_TRAP) chance = 5;
	if (ttmp->ttyp == LEVITATION_TRAP) chance = 20;
	if (ttmp->ttyp == BOWEL_CRAMPS_TRAP) chance = 8;
	if (ttmp->ttyp == SINCOUNT_TRAP) chance = 24;
	if (ttmp->ttyp == BACK_TO_START_TRAP) chance = 20;
	if (ttmp->ttyp == PUNISHMENT_TRAP) chance = 20;
	if (ttmp->ttyp == CONTAMINATION_TRAP) chance = 12;
	if (ttmp->ttyp == LASTING_AMNESIA_TRAP) chance = 15;
	if (ttmp->ttyp == PAIN_TRAP) chance = 15;
	if (ttmp->ttyp == TREMBLING_TRAP) chance = 18;
	if (ttmp->ttyp == TECHCAP_TRAP) chance = 13;
	if (ttmp->ttyp == SPELL_MEMORY_TRAP) chance = 13;
	if (ttmp->ttyp == SKILL_REDUCTION_TRAP) chance = 13;
	if (ttmp->ttyp == SKILLCAP_TRAP) chance = 24;
	if (ttmp->ttyp == PERMANENT_STAT_DAMAGE_TRAP) chance = 25;
	if (ttmp->ttyp == MIGUC_TRAP) chance = 16;
	if (ttmp->ttyp == DIRECTIVE_TRAP) chance = 16;
	if (ttmp->ttyp == SANITY_INCREASE_TRAP) chance = 10;
	if (ttmp->ttyp == PSI_TRAP) chance = 10;
	if (ttmp->ttyp == CORONA_TRAP) chance = 10;
	if (ttmp->ttyp == UNPROOFING_TRAP) chance = 15;
	if (ttmp->ttyp == VISIBILITY_TRAP) chance = 15;
	if (ttmp->ttyp == MOTH_LARVAE_TRAP) chance = 10;
	if (ttmp->ttyp == STRIKETHROUGH_TRAP) chance = 20;
	if (ttmp->ttyp == VIVISECTION_TRAP) chance = 30;
	if (ttmp->ttyp == SKILL_POINT_LOSS_TRAP) chance = 40;
	if (ttmp->ttyp == TECHSTOP_TRAP) chance = 10;
	if (ttmp->ttyp == TENTH_TRAP) chance = 10;
	if (ttmp->ttyp == DEBT_TRAP) chance = 10;
	if (ttmp->ttyp == INVERSION_TRAP) chance = 12;
	if (ttmp->ttyp == WINCE_TRAP) chance = 10;
	if (ttmp->ttyp == FUCK_OVER_TRAP) chance = 20;
	if (ttmp->ttyp == BURDEN_TRAP) chance = 20;
	if (ttmp->ttyp == RAZOR_TRAP) chance = 16;
	if (ttmp->ttyp == VULNERATE_TRAP) chance = 20;
	if (ttmp->ttyp == MAGIC_VACUUM_TRAP) chance = 16;
	if (ttmp->ttyp == ALIGNMENT_TRASH_TRAP) chance = 20;
	if (ttmp->ttyp == DOGSIDE_TRAP) chance = 20;
	if (ttmp->ttyp == BANKRUPT_TRAP) chance = 25;
	if (ttmp->ttyp == MALEVOLENCE_TRAP) chance = 30;
	if (ttmp->ttyp == STATHALF_TRAP) chance = 20;
	if (ttmp->ttyp == CUTSTAT_TRAP) chance = 20;

	if (ttmp->ttyp == FART_TRAP || ttmp->ttyp == PERSISTENT_FART_TRAP) chance = (ttmp->launch_otyp == 2) ? 4 : (ttmp->launch_otyp == 5) ? 3 : (ttmp->launch_otyp == 12) ? 7 : (ttmp->launch_otyp == 18) ? 6 : (ttmp->launch_otyp == 20) ? 8 : (ttmp->launch_otyp == 24) ? 15 : (ttmp->launch_otyp == 25) ? 20 :  (ttmp->launch_otyp == 27) ? 2 : (ttmp->launch_otyp == 28) ? 5 : (ttmp->launch_otyp == 29) ? 7 : (ttmp->launch_otyp == 31) ? 20 : (ttmp->launch_otyp == 41) ? 10 : (ttmp->launch_otyp == 42) ? 100 : (ttmp->launch_otyp < 12) ? 5 : (ttmp->launch_otyp < 33) ? 10 : 20;

	if (Confusion || Hallucination) chance++;
	if (Blind) chance++;
	if (Numbed) chance++;
	if (Stunned) chance += 2;
	if (Feared) chance += 2;
	if (isfriday) chance++;
	if (Fumbling) chance *= 2;
	/* Your own traps are better known than others. */
	if (ttmp && ttmp->madeby_u) chance--;
	if (Role_if(PM_ROGUE)) {
	    if (rn2(2 * MAXULEV) < u.ulevel) chance--;
	    if (u.uhave.questart && chance > 1) chance--;
	} else if (Role_if(PM_RANGER) && chance > 1) chance--;

	if (StrongDefusing && !rn2(2) && chance > 2) {
		chance -= rn2((chance / 2) + 1);
		if (chance < 2) chance = 2;
	}
	if (Role_if(PM_CYBERNINJA) && chance > 1) chance /= 2; /* cyberninja is really good at disarming --Amy */
	if (ublindf && ublindf->oartifact == ART_MAC_S_BOX && chance > 1) chance /= 2;

	if (chance < 1) chance = 1; /* fail safe */
	return rn2(chance);
}

/* Replace trap with object(s).  Helge Hafting */
STATIC_OVL void
cnv_trap_obj(otyp, cnt, ttmp)
int otyp;
int cnt;
struct trap *ttmp;
{
	struct obj *otmp;
	int cc, dd;

	if (otyp == LAND_MINE || otyp == BEARTRAP) otmp = mksobj(otyp, TRUE, 2, FALSE);
	else otmp = mksobj(otyp, TRUE, FALSE, FALSE);

	/* [ALI] Only dart traps are capable of being poisonous */
	if (otmp) {
		if (!timebasedlowerchance()) {
			obfree(otmp, (struct obj *)0);
		} else {
			/* Only dart traps are capable of being poisonous */
			if (otyp != DART)
				otmp->opoisoned = 0;
			/* Amy edit: and some specific other trap types */
			if (ttmp->ttyp == POISON_ARROW_TRAP || ttmp->ttyp == POISON_BOLT_TRAP) otmp->opoisoned = TRUE;
			otmp->quan=cnt;
			otmp->owt = weight(otmp);
			place_object(otmp, ttmp->tx, ttmp->ty);
			/* Sell your own traps only... */
			if (ttmp->madeby_u) sellobj(otmp, ttmp->tx, ttmp->ty);
			stackobj(otmp);
		}
	/* careful: otmp might have been freed */
	}
	cc = ttmp->tx;
	dd = ttmp->ty;
	deltrap(ttmp); /* ttmp is also freed at this point */
	newsym(cc, dd);
}

/* while attempting to disarm an adjacent trap, we've fallen into it */
STATIC_OVL void
move_into_trap(ttmp)
struct trap *ttmp;
{
	int bc;
	xchar x = ttmp->tx, y = ttmp->ty, bx, by, cx, cy;
	boolean unused;

	/* we know there's no monster in the way, and we're not trapped */
	if (!Punished || drag_ball(x, y, &bc, &bx, &by, &cx, &cy, &unused,
		TRUE)) {
	    u.ux0 = u.ux,  u.uy0 = u.uy;
	    u.ux = x,  u.uy = y;
	    u.umoved = TRUE;
	    newsym(u.ux0, u.uy0);
	    vision_recalc(1);
	    check_leash(u.ux0, u.uy0);
	    if (Punished) move_bc(0, bc, bx, by, cx, cy);
	    spoteffects(FALSE);	/* dotrap() */
	    exercise(A_WIS, FALSE);
	}
}

/* 0: doesn't even try
 * 1: tries and fails
 * 2: succeeds
 */
STATIC_OVL int
try_disarm(ttmp, force_failure, difficulttrap)
struct trap *ttmp;
boolean force_failure;
boolean difficulttrap;
{
	struct monst *mtmp = m_at(ttmp->tx,ttmp->ty);
	int ttype = ttmp->ttyp;
	boolean under_u = (!u.dx && !u.dy);
	boolean holdingtrap = (ttype == BEAR_TRAP || ttype == WEB || ttype == FARTING_WEB);

	if (tech_inuse(T_TELEKINESIS) && !force_failure)
		return 2;
	
	/* Test for monster first, monsters are displayed instead of trap. */
	if (mtmp && (!mtmp->mtrapped || !holdingtrap)) {
		pline("%s is in the way.", Monnam(mtmp));
		return 0;
	}
	/* We might be forced to move onto the trap's location. */
	if (sobj_at(BOULDER, ttmp->tx, ttmp->ty)
				&& !Passes_walls && !under_u) {
		There("is a boulder in your way.");
		return 0;
	}
	/* duplicate tight-space checks from test_move */
	if (u.dx && u.dy &&
	    bad_rock(&youmonst, u.ux, ttmp->ty) &&
	    bad_rock(&youmonst, ttmp->tx, u.uy)) {
	    if ((invent && (inv_weight() + weight_cap() > 5000)) ||
		bigmonst(youmonst.data)) {
		/* don't allow untrap if they can't get thru to it */
		You("are unable to reach the %s!",
		    defsyms[trap_to_defsym(ttype)].explanation);
		return 0;
	    }
	}
	/* untrappable traps are located on the ground. */
	if (!can_reach_floor()) {
		if (u.usteed && !(uwep && uwep->oartifact == ART_SORTIE_A_GAUCHE) && !(powerfulimplants() && uimplant && uimplant->oartifact == ART_READY_FOR_A_RIDE) && !FemtrapActiveKerstin && !(bmwride(ART_DEEPER_LAID_BMW)) && (PlayerCannotUseSkills || P_SKILL(P_RIDING) < P_BASIC) )
			You("aren't skilled enough to reach from %s.",
				mon_nam(u.usteed));
		else
		You("are unable to reach the %s!",
			defsyms[trap_to_defsym(ttype)].explanation);
		return 0;
	}

	if (ublindf && ublindf->oartifact == ART_COUNTER_TERRORISTS_WIN && u.ualign.type == A_LAWFUL && (ttmp->ttyp == LANDMINE || ttmp->ttyp == BOMB_TRAP) ) {
		return 2;
	}

	/* Will our hero succeed? */
	if (force_failure || untrap_prob(ttmp)) {
		if (rnl(5) && !(RngeDefusing && rn2(3) ) ) {

		    if (difficulttrap) {
			if (Role_if(PM_GANG_SCHOLAR) || Role_if(PM_WALSCHOLAR)) {
				pline("Oh no, you set it off!");
			} else {
				pline("You set off the trap!");
			}
		    } else {
			pline("Whoops...");
		    }
		    if (mtmp) {		/* must be a trap that holds monsters */
			if (ttype == BEAR_TRAP) {
			    if (mtmp->mtame) abuse_dog(mtmp);
			    if ((mtmp->mhp -= rnd(4)) <= 0) killed(mtmp);
			} else if (ttype == WEB || ttype == FARTING_WEB) {
			    if (!webmaker(youmonst.data)) {
				struct trap *ttmp2 = maketrap(u.ux, u.uy, WEB, 0, FALSE);
				if (ttmp2) {
				    pline_The("webbing sticks to you. You're caught too!");
				    dotrap(ttmp2, NOWEBMSG);
				    if (u.usteed && u.utrap && !mayfalloffsteed()) {
					/* you, not steed, are trapped */
					dismount_steed(DISMOUNT_FELL);
				    }
				}
			    } else
				pline("%s remains entangled.", Monnam(mtmp));
			}
		    } else if (under_u) {
			dotrap(ttmp, 0);
		    } else {
			move_into_trap(ttmp);
		    }
		    if (difficulttrap && !rn2(10) && (rn2(2) || !StrongDefusing) ) {
			badeffect();
			if (!rn2(20)) {
				pline("Oof - your very soul has been drained!");
				drain_alla(1);
			}
		    }
		} else {

		    if (difficulttrap) {
			if (Role_if(PM_GANG_SCHOLAR) || Role_if(PM_WALSCHOLAR)) {
				pline("It's not disarmed yet!");
			} else {
				pline("You failed to disarm the trap.");
			}
		    } else {
			    pline("%s %s is difficult to %s.",
				  ttmp->madeby_u ? "Your" : under_u ? "This" : "That",
				  defsyms[trap_to_defsym(ttype)].explanation,
				  (ttype == WEB) ? "remove" : "disarm");
		    }

		    if (difficulttrap && !rn2(40) && (rn2(2) || !StrongDefusing) ) {
			badeffect();
		    }
		}
		return 1;
	}
	return 2;
}

STATIC_OVL void
reward_untrap(ttmp, mtmp)
struct trap *ttmp;
struct monst *mtmp;
{
	if (!ttmp->madeby_u) {
	    if (rnl(10) < 8 && !mtmp->mpeaceful &&
		    !mtmp->msleeping && !mtmp->mfrozen &&
		    !mindless(mtmp->data) &&
		    mtmp->data->mlet != S_HUMAN) {
		if (!mtmp->mfrenzied) mtmp->mpeaceful = 1;
		set_malign(mtmp);	/* reset alignment */
		pline("%s is grateful.", Monnam(mtmp));
	    }
	    /* Helping someone out of a trap is a nice thing to do,
	     * A lawful may be rewarded, but not too often.  */
	    if (!rn2(3) && !rnl(8) && u.ualign.type == A_LAWFUL) {
		if (Race_if(PM_HUMANOID_DEVIL)) devil_misbehavior();
		else {
			adjalign(1);
			You_feel("that you did the right thing.");
		}
	    }
	}
}

int
disarm_holdingtrap(ttmp) /* Helge Hafting */
struct trap *ttmp;
{
	struct monst *mtmp;
	int fails = try_disarm(ttmp, FALSE, FALSE);

	if (fails < 2) return fails;

	/* ok, disarm it. */

	/* untrap the monster, if any.
	   There's no need for a cockatrice test, only the trap is touched */
	if ((mtmp = m_at(ttmp->tx,ttmp->ty)) != 0) {
		mtmp->mtrapped = 0;
		You("remove %s %s from %s.", the_your[ttmp->madeby_u],
			(ttmp->ttyp == BEAR_TRAP) ? "bear trap" : "webbing",
			mon_nam(mtmp));
		reward_untrap(ttmp, mtmp);
		u.cnd_untrapamount++;
	} else {
		if (ttmp->ttyp == BEAR_TRAP) {
			You("disarm %s bear trap.", the_your[ttmp->madeby_u]);
			if (!ttmp->madeby_u && u.ualign.type == A_LAWFUL) adjalign(1);
			cnv_trap_obj(BEARTRAP, 1, ttmp);
			u.cnd_untrapamount++;
		} else /* if (ttmp->ttyp == WEB) */ {
			You("succeed in removing %s web.", the_your[ttmp->madeby_u]);
			if (!ttmp->madeby_u && u.ualign.type == A_LAWFUL) adjalign(1);
			deltrap(ttmp);
			u.cnd_untrapamount++;
		}

		if (!ttmp->madeby_u) {
			more_experienced(5 * (deepest_lev_reached(FALSE) + 1),0);
			newexplevel();
		}

	}
	newsym(u.ux + u.dx, u.uy + u.dy);
	return 1;
}

int
disarm_landmine(ttmp) /* Helge Hafting */
struct trap *ttmp;
{
	int fails;

	if (ublindf && ublindf->oartifact == ART_COUNTER_TERRORISTS_WIN && u.ualign.type == A_LAWFUL && (ttmp->ttyp == LANDMINE || ttmp->ttyp == BOMB_TRAP) ) fails = 2; /* guaranteed success */
	else fails = try_disarm(ttmp, FALSE, FALSE);

	if (fails < 2) return fails;
	You("disarm %s land mine.", the_your[ttmp->madeby_u]);
	if (!ttmp->madeby_u && u.ualign.type == A_LAWFUL) adjalign(1);
	if (!ttmp->madeby_u) {
		more_experienced(20 * (deepest_lev_reached(FALSE) + 1),0);
		mightbooststat(A_DEX);
		if (ttmp->giveshp && (u.uhpmax < (u.ulevel * 10))) {
			u.uhpmax += 3;
			if (Upolyd) u.mhmax += 3;
			flags.botl = TRUE;
		}
		newexplevel();
	}
	cnv_trap_obj(LAND_MINE, 1, ttmp);
	u.cnd_untrapamount++;
	return 1;
}

int
disarm_rust_trap(ttmp) /* Paul Sonier */
struct trap *ttmp;
{
	xchar trapx = ttmp->tx, trapy = ttmp->ty;
	int fails = try_disarm(ttmp, FALSE, FALSE);

	if (fails < 2) return fails;
	You("disarm the water trap!");
	u.cnd_untrapamount++;
	if (u.ualign.type == A_LAWFUL) adjalign(1);
	more_experienced(10 * (deepest_lev_reached(FALSE) + 1),0);
	mightbooststat(A_DEX);
	if (ttmp->giveshp && (u.uhpmax < (u.ulevel * 10))) {
		u.uhpmax += 1;
		if (Upolyd) u.mhmax += 1;
		flags.botl = TRUE;
	}
	newexplevel();
	deltrap(ttmp);
	levl[trapx][trapy].typ = FOUNTAIN;
	newsym(trapx, trapy);
	level.flags.nfountains++;
	return 1;
}

int
disarm_glyph_trap(ttmp, exper)
struct trap *ttmp;
int exper;
{
	xchar trapx = ttmp->tx, trapy = ttmp->ty;
	int fails = try_disarm(ttmp, FALSE, FALSE);
	int hpboost = 2;

	if (fails < 2) return fails;
	You("disarm the trap!");
	u.cnd_untrapamount++;
	more_experienced(exper * (deepest_lev_reached(FALSE) + 1),0);
	flags.botl = TRUE;
	if (exper < 5) hpboost = 2;
	else if (exper < 26) hpboost = 5;
	else if (exper < 51) hpboost = 7;
	else if (exper < 101) hpboost = 10;
	else if (exper < 201) hpboost = 12;
	else if (exper < 401) hpboost = 15;
	else if (exper < 2402) hpboost = 20;
	mightbooststat(A_DEX);
	if (ttmp->giveshp && (u.uhpmax < (u.ulevel * 10))) {
		u.uhpmax += hpboost;
		if (Upolyd) u.mhmax += hpboost;
		newexplevel();
	}
	if (u.ualign.type == A_LAWFUL) adjalign(1);
	deltrap(ttmp);
	newsym(trapx, trapy);
	return 1;
}

int
disarm_blade_trap(ttmp)
struct trap *ttmp;
{
	xchar trapx = ttmp->tx, trapy = ttmp->ty;
	int fails = try_disarm(ttmp, FALSE, FALSE);

	if (fails < 2) return fails;
	You("disarm the trap!");
	u.cnd_untrapamount++;
	more_experienced(3 * (deepest_lev_reached(FALSE) + 1),0);
	mightbooststat(A_DEX);
	if (ttmp->giveshp && (u.uhpmax < (u.ulevel * 10))) {
		u.uhpmax += 2;
		if (Upolyd) u.mhmax += 2;
		flags.botl = TRUE;
	}
	newexplevel();
	if (u.ualign.type == A_LAWFUL) adjalign(1);
	deltrap(ttmp);
	newsym(trapx, trapy);
	return 1;
}

int
disarm_spined_ball(ttmp)
struct trap *ttmp;
{
	xchar trapx = ttmp->tx, trapy = ttmp->ty;
	int fails = try_disarm(ttmp, FALSE, FALSE);

	if (fails < 2) return fails;
	You("disarm the trap!");
	u.cnd_untrapamount++;
	more_experienced(3 * (deepest_lev_reached(FALSE) + 1),0);
	mightbooststat(A_DEX);
	if (ttmp->giveshp && (u.uhpmax < (u.ulevel * 10))) {
		u.uhpmax += 2;
		if (Upolyd) u.mhmax += 2;
		flags.botl = TRUE;
	}
	newexplevel();
	if (u.ualign.type == A_LAWFUL) adjalign(1);
	cnv_trap_obj(MORNING_STAR, 1, ttmp);
	newsym(trapx, trapy);
	return 1;
}

int
disarm_pendulum(ttmp)
struct trap *ttmp;
{
	xchar trapx = ttmp->tx, trapy = ttmp->ty;
	int fails = try_disarm(ttmp, FALSE, FALSE);

	if (fails < 2) return fails;
	You("disarm the trap!");
	u.cnd_untrapamount++;
	more_experienced(3 * (deepest_lev_reached(FALSE) + 1),0);
	mightbooststat(A_DEX);
	if (ttmp->giveshp && (u.uhpmax < (u.ulevel * 10))) {
		u.uhpmax += 3;
		if (Upolyd) u.mhmax += 3;
		flags.botl = TRUE;
	}
	newexplevel();
	if (u.ualign.type == A_LAWFUL) adjalign(1);
	cnv_trap_obj(IRON_CHAIN, 1, ttmp);
	newsym(trapx, trapy);
	return 1;
}

/* trying to disarm a "hard to disarm" trap that requires defusing trinsic; Amy keyword "newtraps" */
int
disarm_difficult_trap(ttmp)
struct trap *ttmp;
{
	xchar trapx = ttmp->tx, trapy = ttmp->ty;

	int multiplier = 3; /* amount of XP you get */

	switch (ttmp->ttyp) {
		case SPEAR_TRAP:
			multiplier = 5; break;
		case ROLLING_BOULDER_TRAP:
			multiplier = 3; break;
		case SLP_GAS_TRAP:
			multiplier = 5; break;
		case CHLOROFORM_TRAP:
			multiplier = 12; break;
		case TELEP_TRAP:
			multiplier = 3; break;
		case LEVEL_TELEP:
			multiplier = 8; break;
		case STATUE_TRAP:
			multiplier = 4; break;
		case MAGIC_TRAP:
			multiplier = 4; break;
		case ANTI_MAGIC:
			multiplier = 4; break;
		case POLY_TRAP:
			multiplier = 20; break;
		case ICE_TRAP:
			multiplier = 5; break;
		case COLLAPSE_TRAP:
			multiplier = 30; break;
		case MAGIC_BEAM_TRAP:
			multiplier = 30; break;
		case SHIT_TRAP:
			multiplier = 3; break;
		case ANIMATION_TRAP:
			multiplier = 10; break;
		case RETURN_TRAP:
			multiplier = 10; break;
		case POISON_GAS_TRAP:
			multiplier = 5; break;
		case SLOW_GAS_TRAP:
			multiplier = 5; break;
		case SHOCK_TRAP:
			multiplier = 8; break;
		case VULN_TRAP:
			multiplier = 10; break;
		case GRAVITY_TRAP:
			multiplier = 12; break;
		case LOCK_TRAP:
			multiplier = 10; break;
		case MAGIC_CANCELLATION_TRAP:
			multiplier = 15; break;
		case CYANIDE_TRAP:
			multiplier = 40; break;
		case PHOSGENE_TRAP:
			multiplier = 30; break;
		case LASER_TRAP:
			multiplier = 10; break;
		case CONFUSE_TRAP:
			multiplier = 5; break;
		case STUN_TRAP:
			multiplier = 5; break;
		case DIMNESS_TRAP:
			multiplier = 5; break;
		case HALLUCINATION_TRAP:
			multiplier = 5; break;
		case NUMBNESS_TRAP:
			multiplier = 5; break;
		case FREEZING_TRAP:
			multiplier = 6; break;
		case INTRINSIC_STEAL_TRAP:
			multiplier = 20; break;
		case BURNING_TRAP:
			multiplier = 5; break;
		case FEAR_TRAP:
			multiplier = 5; break;
		case BLINDNESS_TRAP:
			multiplier = 5; break;
		case UNLUCKY_TRAP:
			multiplier = 9; break;
		case GLIB_TRAP:
			multiplier = 5; break;
		case UNLIGHT_TRAP:
			multiplier = 10; break;
		case ELEMENTAL_TRAP:
			multiplier = 10; break;
		case SINGLE_UNIDENTIFY_TRAP:
			multiplier = 10; break;
		case ESCALATING_TRAP:
			multiplier = 12; break;
		case NEGATIVE_TRAP:
			multiplier = 10; break;
		case MANA_TRAP:
			multiplier = 5; break;
		case ALIGNMENT_REDUCTION_TRAP:
			multiplier = 5; break;
		case GENETIC_TRAP:
			multiplier = 25; break;
		case SCORE_AXE_TRAP:
			multiplier = 25; break;
		case MISSINGNO_TRAP:
			multiplier = 25; break;
		case CANCELLATION_TRAP:
			multiplier = 30; break;
		case HOSTILITY_TRAP:
			multiplier = 10; break;
		case FALLING_BOULDER_TRAP:
			multiplier = 15; break;
		case OUT_OF_MAGIC_TRAP:
			multiplier = 10; break;
		case PLASMA_TRAP:
			multiplier = 15; break;
		case BOMB_TRAP:
			multiplier = 15; break;
		case EARTHQUAKE_TRAP:
			multiplier = 15; break;
		case NOISE_TRAP:
			multiplier = 3; break;
		case GLUE_TRAP:
			multiplier = 8; break;
		case VOLT_TRAP:
			multiplier = 12; break;
		case BANANA_TRAP:
			multiplier = 3; break;
		case FALLING_TUB_TRAP:
			multiplier = 15; break;
		case ALARM:
			multiplier = 8; break;
		case BLADE_WIRE:
			multiplier = 5; break;
		case MAGNET_TRAP:
			multiplier = 12; break;
		case MALIGNANT_TRAP:
			multiplier = 12; break;
		case CANNON_TRAP:
			multiplier = 25; break;
		case WITHER_TRAP:
			multiplier = 30; break;
		case FUMAROLE:
			multiplier = 10; break;
		case FUMBLING_TRAP:
			multiplier = 12; break;
		case NEXUS_TRAP:
			multiplier = 15; break;
		case LEG_TRAP:
			multiplier = 4; break;
		case STAT_DAMAGE_TRAP:
			multiplier = 4; break;
		case LEVITATION_TRAP:
			multiplier = 10; break;
		case HALF_MEMORY_TRAP:
			multiplier = 12; break;
		case HALF_TRAINING_TRAP:
			multiplier = 22; break;
		case BOWEL_CRAMPS_TRAP:
			multiplier = 8; break;
		case SINCOUNT_TRAP:
			multiplier = 30; break;
		case BEAMER_TRAP:
			multiplier = 3; break;
		case LEVEL_BEAMER:
			multiplier = 8; break;
		case PHASEPORTER:
			multiplier = 3; break;
		case NARCOLEPSY_TRAP:
			multiplier = 4; break;
		case PHASE_BEAMER:
			multiplier = 3; break;
		case PIERCING_BEAM_TRAP:
			multiplier = 30; break;
		case WRENCHING_TRAP:
			multiplier = 10; break;
		case BACK_TO_START_TRAP:
			multiplier = 18; break;
		case PUNISHMENT_TRAP:
			multiplier = 15; break;
		case CONTAMINATION_TRAP:
			multiplier = 12; break;
		case LASTING_AMNESIA_TRAP:
			multiplier = 20; break;
		case PAIN_TRAP:
			multiplier = 10; break;
		case TREMBLING_TRAP:
			multiplier = 15; break;
		case TECHCAP_TRAP:
			multiplier = 8; break;
		case SPELL_MEMORY_TRAP:
			multiplier = 8; break;
		case SKILL_REDUCTION_TRAP:
			multiplier = 8; break;
		case SKILLCAP_TRAP:
			multiplier = 16; break;
		case PERMANENT_STAT_DAMAGE_TRAP:
			multiplier = 35; break;
		case MIGUC_TRAP:
			multiplier = 8; break;
		case SATATUE_TRAP:
			multiplier = 5; break;
		case DIRECTIVE_TRAP:
			multiplier = 10; break;
		case SANITY_INCREASE_TRAP:
			multiplier = 10; break;
		case PSI_TRAP:
			multiplier = 10; break;
		case CORONA_TRAP:
			multiplier = 8; break;
		case UNPROOFING_TRAP:
			multiplier = 12; break;
		case VISIBILITY_TRAP:
			multiplier = 10; break;
		case BRANCH_TELEPORTER:
			multiplier = 12; break;
		case BRANCH_BEAMER:
			multiplier = 12; break;
		case MOTH_LARVAE_TRAP:
			multiplier = 8; break;
		case STRIKETHROUGH_TRAP:
			multiplier = 12; break;
		case VIVISECTION_TRAP:
			multiplier = 20; break;
		case SKILL_POINT_LOSS_TRAP:
			multiplier = 30; break;
		case TECHSTOP_TRAP:
			multiplier = 5; break;
		case TENTH_TRAP:
			multiplier = 5; break;
		case DEBT_TRAP:
			multiplier = 5; break;
		case INVERSION_TRAP:
			multiplier = 6; break;
		case WINCE_TRAP:
			multiplier = 6; break;
		case FUCK_OVER_TRAP:
			multiplier = 10; break;
		case MAGIC_VACUUM_TRAP:
			multiplier = 9; break;
		case RAZOR_TRAP:
			multiplier = 9; break;
		case VULNERATE_TRAP:
			multiplier = 18; break;
		case BURDEN_TRAP:
			multiplier = 10; break;
		case ALIGNMENT_TRASH_TRAP:
			multiplier = 20; break;
		case DOGSIDE_TRAP:
			multiplier = 15; break;
		case BANKRUPT_TRAP:
			multiplier = 15; break;
		case MALEVOLENCE_TRAP:
			multiplier = 30; break;
		case STATHALF_TRAP:
			multiplier = 15; break;
		case CUTSTAT_TRAP:
			multiplier = 15; break;

		default: break;
	}

	int fails;

	if (ublindf && ublindf->oartifact == ART_COUNTER_TERRORISTS_WIN && u.ualign.type == A_LAWFUL && (ttmp->ttyp == LANDMINE || ttmp->ttyp == BOMB_TRAP) ) fails = 2; /* guaranteed success */
	else fails = try_disarm(ttmp, FALSE, TRUE);

	if (fails < 2) return fails;

	if (Role_if(PM_GANG_SCHOLAR) || Role_if(PM_WALSCHOLAR)) {
		pline("Disarmed! Now you can move on.");
	} else {
		You("disarmed the trap.");
	}
	u.cnd_untrapamount++;
	more_experienced(multiplier * (deepest_lev_reached(FALSE) + 1),0);
	mightbooststat(A_DEX);
	if (ttmp->giveshp && (u.uhpmax < (u.ulevel * 10))) {
		u.uhpmax += 3;
		if (Upolyd) u.mhmax += 3;
		flags.botl = TRUE;
	}
	newexplevel();
	if (u.ualign.type == A_LAWFUL) adjalign(1);
	deltrap(ttmp);
	newsym(trapx, trapy);
	return 1;
}

int
disarm_mace_trap(ttmp)
struct trap *ttmp;
{
	xchar trapx = ttmp->tx, trapy = ttmp->ty;
	int fails = try_disarm(ttmp, FALSE, FALSE);

	if (fails < 2) return fails;
	You("disarm the trap!");
	u.cnd_untrapamount++;
	more_experienced(3 * (deepest_lev_reached(FALSE) + 1),0);
	mightbooststat(A_DEX);
	if (ttmp->giveshp && (u.uhpmax < (u.ulevel * 10))) {
		u.uhpmax += 3;
		if (Upolyd) u.mhmax += 3;
		flags.botl = TRUE;
	}
	newexplevel();
	if (u.ualign.type == A_LAWFUL) adjalign(1);
	cnv_trap_obj(MACE, 1, ttmp);
	newsym(trapx, trapy);
	return 1;
}

int
disarm_dagger_trap(ttmp)
struct trap *ttmp;
{
	xchar trapx = ttmp->tx, trapy = ttmp->ty;
	int fails = try_disarm(ttmp, FALSE, FALSE);

	if (fails < 2) return fails;
	You("disarm the trap!");
	u.cnd_untrapamount++;
	more_experienced(3 * (deepest_lev_reached(FALSE) + 1),0);
	mightbooststat(A_DEX);
	if (ttmp->giveshp && (u.uhpmax < (u.ulevel * 10))) {
		u.uhpmax += 3;
		if (Upolyd) u.mhmax += 3;
		flags.botl = TRUE;
	}
	newexplevel();
	if (u.ualign.type == A_LAWFUL) adjalign(1);
	cnv_trap_obj(DAGGER, 1, ttmp);
	newsym(trapx, trapy);
	return 1;
}

int
disarm_acid_trap(ttmp)
struct trap *ttmp;
{
	xchar trapx = ttmp->tx, trapy = ttmp->ty;
	int fails = try_disarm(ttmp, FALSE, FALSE);

	if (fails < 2) return fails;
	You("disarm the trap!");
	u.cnd_untrapamount++;
	more_experienced(3 * (deepest_lev_reached(FALSE) + 1),0);
	mightbooststat(A_DEX);
	if (ttmp->giveshp && (u.uhpmax < (u.ulevel * 10))) {
		u.uhpmax += 3;
		if (Upolyd) u.mhmax += 3;
		flags.botl = TRUE;
	}
	newexplevel();
	if (u.ualign.type == A_LAWFUL) adjalign(1);
	if (!rn2(5)) cnv_trap_obj(POT_ACID, rnd(2), ttmp);
	else deltrap(ttmp);
	newsym(trapx, trapy);
	return 1;
}

int
disarm_trap_percents(ttmp)
struct trap *ttmp;
{
	xchar trapx = ttmp->tx, trapy = ttmp->ty;
	int fails = try_disarm(ttmp, FALSE, FALSE);

	if (fails < 2) return fails;
	You("disarm the trap!");
	u.cnd_untrapamount++;
	more_experienced(1 * (deepest_lev_reached(FALSE) + 1),0);
	mightbooststat(A_DEX);
	if (ttmp->giveshp && (u.uhpmax < (u.ulevel * 10))) {
		u.uhpmax += 1;
		if (Upolyd) u.mhmax += 1;
		flags.botl = TRUE;
	}
	newexplevel();
	if (u.ualign.type == A_LAWFUL) adjalign(1);
	deltrap(ttmp);
	newsym(trapx, trapy);
	return 1;
}

int
disarm_active_superscroller(ttmp)
struct trap *ttmp;
{
	xchar trapx = ttmp->tx, trapy = ttmp->ty;
	int fails = try_disarm(ttmp, FALSE, FALSE);

	if (fails < 2) return fails;
	You("removed the superscroller!");
	u.cnd_untrapamount++;
	more_experienced(1 * (deepest_lev_reached(FALSE) + 1),0);
	newexplevel();
	if (u.ualign.type == A_LAWFUL) adjalign(1);
	deltrap(ttmp);
	newsym(trapx, trapy);
	Superscroller = 0L;
	return 1;
}

int
disarm_unknowntrap(ttmp)
struct trap *ttmp;
{

	struct obj *unkrwrd;

	xchar trapx = ttmp->tx, trapy = ttmp->ty;
	int fails = try_disarm(ttmp, FALSE, FALSE);

	if (fails < 2) return fails;
	You("disarm the trap!");
	u.cnd_untrapamount++;
	more_experienced(5 * (deepest_lev_reached(FALSE) + 1),0);
	mightbooststat(A_DEX);
	if (ttmp->giveshp && (u.uhpmax < (u.ulevel * 10))) {
		u.uhpmax += 5;
		if (Upolyd) u.mhmax += 5;
		flags.botl = TRUE;
	}
	newexplevel();
	if (u.ualign.type == A_LAWFUL) adjalign(1);

	unkrwrd = mkobj(RANDOM_CLASS, FALSE, FALSE);
	if (unkrwrd) dropy(unkrwrd);

	deltrap(ttmp);
	newsym(trapx, trapy);
	return 1;
}

int
disarm_fartingtrap(ttmp)
struct trap *ttmp;
{

	struct obj *unkrwrd;
	int diceroll;

	xchar trapx = ttmp->tx, trapy = ttmp->ty;
	int fails = try_disarm(ttmp, FALSE, FALSE);

	if (fails < 2) return fails;

	pline("You bash %s's sexy butt.", farttrapnames[ttmp->launch_otyp]);
	if (u.ualign.type == A_LAWFUL) adjalign(1);
	diceroll = rnd(30);

	if (Role_if(PM_BUTT_LOVER)) {
		You_feel("bad for hurting one of your beloved butts!");
		adjalign(-5);
		if (u.negativeprotection > 0) u.negativeprotection--;
	}

	if (ttmp->launch_otyp == 2) diceroll -= rnd(20);
	if (ttmp->launch_otyp == 5) diceroll -= rnd(10);
	if (ttmp->launch_otyp == 12) diceroll -= rnd(10);
	if (ttmp->launch_otyp == 20) diceroll -= rnd(5);
	if (ttmp->launch_otyp == 25) diceroll -= rnd(5);
	if (ttmp->launch_otyp == 28) diceroll -= rnd(30);
	if (ttmp->launch_otyp == 29) diceroll -= rnd(20);
	if (ttmp->launch_otyp == 31) diceroll -= rnd(10);
	if (ttmp->launch_otyp == 41) diceroll -= rnd(15);
	if (diceroll < 1) diceroll = 1; /* fail safe */

	if (ttmp->launch_otyp < 12) diceroll -= rnd(diceroll);
	else if (!rn2(2) && ttmp->launch_otyp < 33) diceroll -= rnd(diceroll);

	if (diceroll < 0) diceroll = 0; /* fail safe */

	switch (diceroll) {

		case 0:
			pline("%s seems unphased.", farttrapnames[ttmp->launch_otyp]);
			return 1;
		case 1:
		case 2:
		case 3:
		case 4:
			pline("%s's sexy butt is getting beautiful red bruises.", farttrapnames[ttmp->launch_otyp]);
			return 1;
		case 5:
		case 6:
		case 7:
		case 8:
			pline("%s's sexy butt is getting sore from your beating.", farttrapnames[ttmp->launch_otyp]);
			return 1;
		case 9:
		case 10:
		case 11:
		case 12:
			pline("%s's sexy butt is hurt badly.", farttrapnames[ttmp->launch_otyp]);
			return 1;
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
			pline("%s's sexy butt looks like it's bleeding, but it instantly stops.", farttrapnames[ttmp->launch_otyp]);
			return 1;
		default: /* 20 or higher = disarmed */
			{
			boolean cangivehp = ttmp->giveshp;
			pline("You hurt %s so badly that she retreats her sexy butt, and decides to set up her high heels as a trap instead!", farttrapnames[ttmp->launch_otyp]);
			u.cnd_untrapamount++;
			mightbooststat(A_DEX);
			more_experienced(500 * (deepest_lev_reached(FALSE) + 1),0);
			/* always give boost because they're hard to disarm --Amy */
			u.uhpmax += 25;
			if (Upolyd) u.mhmax += 25;
			flags.botl = TRUE;
			newexplevel();
			deltrap(ttmp);
			ttmp = maketrap(trapx, trapy, HEEL_TRAP, 0, cangivehp);
			if (ttmp && !ttmp->hiddentrap ) ttmp->tseen = 1;
			newsym(trapx, trapy);

			if (Role_if(PM_BUTT_LOVER)) {
				You_feel("like someone has touched your forehead...");

				int skillimprove = randomgoodskill();

				if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
					unrestrict_weapon_skill(skillimprove);
					pline("You can now learn the %s skill.", wpskillname(skillimprove));
				} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
					unrestrict_weapon_skill(skillimprove);
					P_MAX_SKILL(skillimprove) = P_BASIC;
					pline("You can now learn the %s skill.", wpskillname(skillimprove));
				} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
					P_MAX_SKILL(skillimprove) = P_SKILLED;
					pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
				} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
					P_MAX_SKILL(skillimprove) = P_EXPERT;
					pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
				} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
					P_MAX_SKILL(skillimprove) = P_MASTER;
					pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
				} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
					P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
					pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
				} else if (!rn2(200) && P_MAX_SKILL(skillimprove) == P_GRAND_MASTER) {
					P_MAX_SKILL(skillimprove) = P_SUPREME_MASTER;
					pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
				} else pline("Unfortunately, you feel no different than before.");

				if (Race_if(PM_RUSMOT)) {
					if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
						unrestrict_weapon_skill(skillimprove);
						pline("You can now learn the %s skill.", wpskillname(skillimprove));
					} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
						unrestrict_weapon_skill(skillimprove);
						P_MAX_SKILL(skillimprove) = P_BASIC;
						pline("You can now learn the %s skill.", wpskillname(skillimprove));
					} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
						P_MAX_SKILL(skillimprove) = P_SKILLED;
						pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
					} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
						P_MAX_SKILL(skillimprove) = P_EXPERT;
						pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
					} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
						P_MAX_SKILL(skillimprove) = P_MASTER;
						pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
					} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
						P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
						pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
					} else if (!rn2(200) && P_MAX_SKILL(skillimprove) == P_GRAND_MASTER) {
						P_MAX_SKILL(skillimprove) = P_SUPREME_MASTER;
						pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
					} else pline("Unfortunately, you feel no different than before.");
				}

			}

			return 1;
			}
	}

}

int
disarm_water_trap(ttmp)
struct trap *ttmp;
{
	xchar trapx = ttmp->tx, trapy = ttmp->ty;
	int fails = try_disarm(ttmp, FALSE, FALSE);

	if (fails < 2) return fails;
	You("disarm the trap!");
	u.cnd_untrapamount++;
	more_experienced(3 * (deepest_lev_reached(FALSE) + 1),0);
	mightbooststat(A_DEX);
	if (ttmp->giveshp && (u.uhpmax < (u.ulevel * 10))) {
		u.uhpmax += 2;
		if (Upolyd) u.mhmax += 2;
		flags.botl = TRUE;
	}
	newexplevel();
	if (u.ualign.type == A_LAWFUL) adjalign(1);
	if (!rn2(10)) cnv_trap_obj(POT_WATER, rnd(4), ttmp);
	else deltrap(ttmp);
	newsym(trapx, trapy);
	return 1;
}

int
disarm_heel_trap(ttmp)
struct trap *ttmp;
{
	xchar trapx = ttmp->tx, trapy = ttmp->ty;
	int fails = try_disarm(ttmp, FALSE, FALSE);

	if (fails < 2) return fails;
	You("disarm the trap!");
	u.cnd_untrapamount++;
	more_experienced(20 * (deepest_lev_reached(FALSE) + 1),0);
	mightbooststat(A_DEX);
	if (ttmp->giveshp && (u.uhpmax < (u.ulevel * 10))) {
		u.uhpmax += 10;
		if (Upolyd) u.mhmax += 10;
		flags.botl = TRUE;
	}
	newexplevel();
	if (u.ualign.type == A_LAWFUL) adjalign(1);

	if (ttmp->launch_otyp == 1) cnv_trap_obj(WEDGE_SANDALS, 1, ttmp);
	else if (ttmp->launch_otyp == 2) cnv_trap_obj(DANCING_SHOES, 1, ttmp);
	else if (ttmp->launch_otyp == 3) cnv_trap_obj(SWEET_MOCASSINS, 1, ttmp);
	else if (ttmp->launch_otyp == 4) cnv_trap_obj(SOFT_SNEAKERS, 1, ttmp);
	else if (ttmp->launch_otyp == 5) cnv_trap_obj(LEATHER_PEEP_TOES, 1, ttmp);
	else if (ttmp->launch_otyp == 6) cnv_trap_obj(FEMININE_PUMPS, 1, ttmp);
	else if (ttmp->launch_otyp == 7) cnv_trap_obj(COMBAT_STILETTOS, 1, ttmp);
	else if (ttmp->launch_otyp == 8) cnv_trap_obj(HIGH_HEELED_SANDAL, 1, ttmp);
	else if (ttmp->launch_otyp == 9) cnv_trap_obj(SEXY_LEATHER_PUMP, 1, ttmp);
	else if (ttmp->launch_otyp == 10) cnv_trap_obj(SPIKED_BATTLE_BOOT, 1, ttmp);
	else if (ttmp->launch_otyp == 11) cnv_trap_obj(WEDGED_LITTLE_GIRL_SANDAL, 1, ttmp);
	else if (ttmp->launch_otyp == 12) cnv_trap_obj(STURDY_PLATEAU_BOOT_FOR_GIRLS, 1, ttmp);
	else if (ttmp->launch_otyp == 13) cnv_trap_obj(HUGGING_BOOT, 1, ttmp);
	else if (ttmp->launch_otyp == 14) cnv_trap_obj(WOODEN_GETA, 1, ttmp);
	else if (ttmp->launch_otyp == 15) cnv_trap_obj(LACQUERED_DANCING_SHOE, 1, ttmp);
	else if (ttmp->launch_otyp == 16) cnv_trap_obj(BLOCK_HEELED_COMBAT_BOOT, 1, ttmp);
	else if (ttmp->launch_otyp == 17) cnv_trap_obj(INKA_BOOT, 1, ttmp);
	else if (ttmp->launch_otyp == 18) cnv_trap_obj(STEEL_CAPPED_SANDAL, 1, ttmp);
	else if (ttmp->launch_otyp == 19) cnv_trap_obj(BLOCK_HEELED_SANDAL, 1, ttmp);
	else if (ttmp->launch_otyp == 20) cnv_trap_obj(SOFT_LADY_SHOE, 1, ttmp);
	else if (ttmp->launch_otyp == 21) cnv_trap_obj(DOGSHIT_BOOT, 1, ttmp);
	else if (ttmp->launch_otyp == 22) cnv_trap_obj(PROSTITUTE_SHOE, 1, ttmp);
	else if (ttmp->launch_otyp == 23) cnv_trap_obj(HIPPIE_HEELS, 1, ttmp);
	else if (ttmp->launch_otyp == 24) cnv_trap_obj(LADY_BOOTS, 1, ttmp);
	else if (ttmp->launch_otyp == 25) cnv_trap_obj(STILETTO_SANDALS, 1, ttmp);
	else if (ttmp->launch_otyp == 26) cnv_trap_obj(ITALIAN_HEELS, 1, ttmp);
	else if (ttmp->launch_otyp == 27) cnv_trap_obj(SEXY_MARY_JANE, 1, ttmp);
	else if (ttmp->launch_otyp == 28) cnv_trap_obj(KITTEN_HEEL_PUMP, 1, ttmp);
	else deltrap(ttmp);
	newsym(trapx, trapy);
	return 1;
}

/* getobj will filter down to cans of grease and known potions of oil */

static NEARDATA const char oil[] = { ALL_CLASSES, TOOL_CLASS, POTION_CLASS, 0 };
static NEARDATA const char disarmpotion[] = { ALL_CLASSES, POTION_CLASS, 0 };

/* water disarms, oil will explode */
int
disarm_fire_trap(ttmp) /* Paul Sonier */
struct trap *ttmp;
{
	int fails;
	struct obj *obj;
	boolean bad_tool;

	obj = getobj(disarmpotion, "untrap with");
	if (!obj) return 0;

	if (obj->otyp == POT_OIL)
	{
		Your("potion of oil explodes!");
		splatter_burning_oil(ttmp->tx,ttmp->ty);
		delobj(obj);
		return 1;
	}

	bad_tool = (obj->cursed ||
				(obj->otyp != POT_WATER));
	fails = try_disarm(ttmp, bad_tool, FALSE);
	if (fails < 2) return fails;

	useup(obj);
	makeknown(POT_WATER);
	You("manage to extinguish the pilot light!");
	u.cnd_untrapamount++;
	if (u.ualign.type == A_LAWFUL) adjalign(1);

	/* Lower chance of getting oil. The deltrap function may NOT be called if cnv_trap_obj is also called
	 * or the game will segfault due to dereferencing the trap twice! --Amy */

	if (!rn2(3)) cnv_trap_obj(POT_OIL, 4 - rnl(4), ttmp);
	else deltrap(ttmp);
	more_experienced(1 * (deepest_lev_reached(FALSE) + 1), 5);
	mightbooststat(A_DEX);
	if (ttmp->giveshp && (u.uhpmax < (u.ulevel * 10))) {
		u.uhpmax += 5;
		if (Upolyd) u.mhmax += 5;
		flags.botl = TRUE;
	}
	newexplevel();
	return 1;
}

/* it may not make much sense to use grease on floor boards, but so what? */
int
disarm_squeaky_board(ttmp)
struct trap *ttmp;
{
	struct obj *obj;
	boolean bad_tool;
	int fails, trapx = ttmp->tx, trapy = ttmp->ty;

	obj = getobj(oil, "untrap with");
	if (!obj) return 0;

	bad_tool = (obj->cursed ||
			((obj->otyp != POT_OIL || obj->lamplit) &&
			 ((obj->otyp != CAN_OF_GREASE && obj->otyp != LUBRICANT_CAN) || !obj->spe)));

	fails = try_disarm(ttmp, bad_tool, FALSE);
	if (fails < 2) return fails;

	/* successfully used oil or grease to fix squeaky board */
	if (obj->otyp == CAN_OF_GREASE) {
	    consume_obj_charge(obj, TRUE);
	} else if (obj->otyp == LUBRICANT_CAN) {
	    consume_obj_charge(obj, TRUE);
	} else {
	    useup(obj);	/* oil */
	    makeknown(POT_OIL);
	}
	You("repair the squeaky board.");	/* no madeby_u */
	u.cnd_untrapamount++;
	if (u.ualign.type == A_LAWFUL) adjalign(1);
	deltrap(ttmp);
	newsym(trapx, trapy);
	more_experienced(1 * (deepest_lev_reached(FALSE) + 1), 5);
	mightbooststat(A_DEX);
	if (ttmp->giveshp && (u.uhpmax < (u.ulevel * 10))) {
		u.uhpmax += 1;
		if (Upolyd) u.mhmax += 1;
		flags.botl = TRUE;
	}
	newexplevel();
	return 1;
}

/* removes traps that shoot arrows, darts, etc. */
int
disarm_shooting_trap(ttmp, otyp)
struct trap *ttmp;
int otyp;
{
	int fails = try_disarm(ttmp, FALSE, FALSE);

	if (fails < 2) return fails;
	You("disarm %s trap.", the_your[ttmp->madeby_u]);
	u.cnd_untrapamount++;
	more_experienced(10 * (deepest_lev_reached(FALSE) + 1), 0);
	mightbooststat(A_DEX);
	if (ttmp->giveshp && (u.uhpmax < (u.ulevel * 10))) {
		u.uhpmax += 1;
		if (Upolyd) u.mhmax += 1;
		flags.botl = TRUE;
	}
	newexplevel();
	if (u.ualign.type == A_LAWFUL) adjalign(1);
	cnv_trap_obj(otyp, 50-rnl(50), ttmp);
	return 1;
}

int
disarm_hard_shooting_trap(ttmp, otyp)
struct trap *ttmp;
int otyp;
{
	int multiplier = 10; /* amount of XP you get */
	if (ttmp->ttyp == VENOM_SPRINKLER) multiplier = 25;

	int fails = try_disarm(ttmp, FALSE, TRUE);

	if (fails < 2) return fails;

	if (Role_if(PM_GANG_SCHOLAR) || Role_if(PM_WALSCHOLAR)) {
		pline("Disarmed! Now you can move on.");
	} else {
		You("disarmed the trap.");
	}

	u.cnd_untrapamount++;
	more_experienced(multiplier * (deepest_lev_reached(FALSE) + 1), 0);
	mightbooststat(A_DEX);
	if (ttmp->giveshp && (u.uhpmax < (u.ulevel * 10))) {
		u.uhpmax += 1;
		if (Upolyd) u.mhmax += 1;
		flags.botl = TRUE;
	}
	newexplevel();
	if (u.ualign.type == A_LAWFUL) adjalign(1);
	cnv_trap_obj(otyp, 50-rnl(50), ttmp);
	return 1;
}

/* Is the weight too heavy?
 * Formula as in near_capacity() & check_capacity() */
STATIC_OVL int
try_lift(mtmp, ttmp, wt, stuff)
struct monst *mtmp;
struct trap *ttmp;
int wt;
boolean stuff;
{
	int wc = weight_cap();

	if (((wt * 2) / wc) >= HVY_ENCUMBER) {
	    pline("%s is %s for you to lift.", Monnam(mtmp),
		  stuff ? "carrying too much" : "too heavy");
	    if (!ttmp->madeby_u && !mtmp->mpeaceful && mtmp->mcanmove &&
		    !mindless(mtmp->data) &&
		    mtmp->data->mlet != S_HUMAN && rnl(10) < 3) {
		if (!mtmp->mfrenzied) mtmp->mpeaceful = 1;
		set_malign(mtmp);		/* reset alignment */
		pline("%s thinks it was nice of you to try.", Monnam(mtmp));
	    }
	    return 0;
	}
	return 1;
}

/* Help trapped monster (out of a (spiked) pit) */
STATIC_OVL int
help_monster_out(mtmp, ttmp)
struct monst *mtmp;
struct trap *ttmp;
{
	int wt;
	struct obj *otmp;
	boolean uprob;

	/*
	 * This works when levitating too -- consistent with the ability
	 * to hit monsters while levitating.
	 *
	 * Should perhaps check that our hero has arms/hands at the
	 * moment.  Helping can also be done by engulfing...
	 *
	 * Test the monster first - monsters are displayed before traps.
	 */
	if (!mtmp->mtrapped) {
		pline("%s isn't trapped.", Monnam(mtmp));
		return 0;
	}
	/* Do you have the necessary capacity to lift anything? */
	if (check_capacity((char *)0)) return 1;

	/* Will our hero succeed? */
	if ((uprob = untrap_prob(ttmp)) && !mtmp->msleeping && mtmp->mcanmove) {
		You("try to reach out your %s, but %s backs away skeptically.",
			makeplural(body_part(ARM)),
			mon_nam(mtmp));
		return 1;
	}


	/* is it a cockatrice?... */
	if (touch_petrifies(mtmp->data) && (!uarmg || FingerlessGloves) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) )) {
		You("grab the trapped %s using your bare %s.",
				mtmp->data->mname, makeplural(body_part(HAND)));

		if (poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
			display_nhwindow(WIN_MESSAGE, FALSE);
		else {
			static char kbuf[BUFSZ];

			sprintf(kbuf, "trying to help a petrifying monster out of a pit");
			instapetrify(kbuf);
			return 1;
		}
	}
	/* need to do cockatrice check first if sleeping or paralyzed */
	if (uprob) {
	    You("try to grab %s, but cannot get a firm grasp.",
		mon_nam(mtmp));
	    if (mtmp->msleeping) {
		mtmp->msleeping = 0;
		pline("%s awakens.", Monnam(mtmp));
	    }
	    return 1;
	}

	You("reach out your %s and grab %s.",
	    makeplural(body_part(ARM)), mon_nam(mtmp));

	if (mtmp->msleeping) {
	    mtmp->msleeping = 0;
	    pline("%s awakens.", Monnam(mtmp));
	} else if (mtmp->mfrozen && !rn2(mtmp->mfrozen)) {
	    /* After such manhandling, perhaps the effect wears off */
	    mtmp->mcanmove = 1;
	    mtmp->masleep = 0;
	    mtmp->mfrozen = 0;
	    pline("%s stirs.", Monnam(mtmp));
	}

	/* is the monster too heavy? */
	wt = inv_weight() + mtmp->data->cwt;
	if (!try_lift(mtmp, ttmp, wt, FALSE)) return 1;

	/* is the monster with inventory too heavy? */
	for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
		wt += otmp->owt;
	if (!try_lift(mtmp, ttmp, wt, TRUE)) return 1;

	You("pull %s out of the pit.", mon_nam(mtmp));
	mtmp->mtrapped = 0;
	fill_pit(mtmp->mx, mtmp->my);
	reward_untrap(ttmp, mtmp);
	return 1;
}

/* player tries to untrap some trap, Amy keyword "newtraps" */
int
untrap(force)
boolean force;
{
	register struct obj *otmp;
	register boolean confused = (Confusion > 0 || Hallucination > 0);
	register int x,y;
	int ch;
	struct trap *ttmp;
	struct monst *mtmp;
	boolean trap_skipped = FALSE;
	boolean box_here = FALSE;
	boolean deal_with_floor_trap = FALSE;
	char the_trap[BUFSZ], qbuf[QBUFSZ];
	int containercnt = 0;

	if(!getdir((char *)0)) return(0);
	x = u.ux + u.dx;
	y = u.uy + u.dy;

	for(otmp = level.objects[x][y]; otmp; otmp = otmp->nexthere) {
		if(Is_box(otmp) && !u.dx && !u.dy) {
			box_here = TRUE;
			containercnt++;
			if (containercnt > 1) break;
		}
	}

	if ((ttmp = t_at(x,y)) && ttmp->tseen) {
		deal_with_floor_trap = TRUE;
		strcpy(the_trap, the(defsyms[trap_to_defsym(ttmp->ttyp)].explanation));
		if (box_here) {
			if (ttmp->ttyp == PIT || ttmp->ttyp == SPIKED_PIT || ttmp->ttyp == GIANT_CHASM || ttmp->ttyp == SHIT_PIT || ttmp->ttyp == MANA_PIT || ttmp->ttyp == ANOXIC_PIT || ttmp->ttyp == HYPOXIC_PIT || ttmp->ttyp == ACID_PIT) {
			    You_cant("do much about %s%s.",
					the_trap, u.utrap ?
					" that you're stuck in" :
					" while standing on the edge of it");
			    trap_skipped = TRUE;
			    deal_with_floor_trap = FALSE;
			} else {
			    sprintf(qbuf, "There %s and %s here. %s %s?",
				(containercnt == 1) ? "is a container" : "are containers",
				an(defsyms[trap_to_defsym(ttmp->ttyp)].explanation),
				ttmp->ttyp == WEB ? "Remove" : "Disarm", the_trap);
			    switch (ynq(qbuf)) {
				case 'q': return(0);
				case 'n': trap_skipped = TRUE;
					  deal_with_floor_trap = FALSE;
					  break;
			    }
			}
		}
		if (deal_with_floor_trap) {
		if (u.utrap) {
			You("cannot deal with %s while trapped%s!", the_trap,
				(x == u.ux && y == u.uy) ? " in it" : "");
			return 1;
		}
		switch(ttmp->ttyp) {
			case BEAR_TRAP:
			case WEB:
			case FARTING_WEB:
				return disarm_holdingtrap(ttmp);
			case LANDMINE:
				return disarm_landmine(ttmp);
			case SQKY_BOARD:
				return disarm_squeaky_board(ttmp);
			case DART_TRAP:
				return disarm_shooting_trap(ttmp, DART);
			case SLINGSHOT_TRAP:
				return disarm_shooting_trap(ttmp, ROCK);
			case THROWING_STAR_TRAP:
				return disarm_shooting_trap(ttmp, SHURIKEN);
			case HEEL_TRAP:
			case ATTACKING_HEEL_TRAP:
				return disarm_heel_trap(ttmp);
			case ARROW_TRAP:
				return disarm_shooting_trap(ttmp, ARROW);
			case POISON_ARROW_TRAP:
				return disarm_shooting_trap(ttmp, ARROW);
			case BOLT_TRAP:
				return disarm_shooting_trap(ttmp, CROSSBOW_BOLT);
			case POISON_BOLT_TRAP:
				return disarm_shooting_trap(ttmp, CROSSBOW_BOLT);
			case BULLET_TRAP:
				return disarm_shooting_trap(ttmp, PISTOL_BULLET);
			case GLASS_ARROW_TRAP:
				return disarm_shooting_trap(ttmp, DROVEN_ARROW);
			case GLASS_BOLT_TRAP:
				return disarm_shooting_trap(ttmp, DROVEN_BOLT);
			case RUST_TRAP:
				return disarm_rust_trap(ttmp);
			case ACID_POOL:
				return disarm_acid_trap(ttmp);
			case UNKNOWN_TRAP:
				return disarm_unknowntrap(ttmp);
			case FART_TRAP:
			case PERSISTENT_FART_TRAP:
				return disarm_fartingtrap(ttmp);
			case ACTIVE_SUPERSCROLLER_TRAP:
				return disarm_active_superscroller(ttmp);
			case TRAP_PERCENTS:
				return disarm_trap_percents(ttmp);
			case WATER_POOL:
				return disarm_water_trap(ttmp);
			case GLYPH_OF_WARDING:
				return disarm_glyph_trap(ttmp, 4);
			case GREEN_GLYPH:
				return disarm_glyph_trap(ttmp, 50);
			case BLUE_GLYPH:
				return disarm_glyph_trap(ttmp, 200);
			case YELLOW_GLYPH:
				return disarm_glyph_trap(ttmp, 25);
			case ORANGE_GLYPH:
				return disarm_glyph_trap(ttmp, 100);
			case BLACK_GLYPH:
				return disarm_glyph_trap(ttmp, 400);
			case PURPLE_GLYPH:
				return disarm_glyph_trap(ttmp, 2401);
			case SCYTHING_BLADE:
				return disarm_blade_trap(ttmp);
			case SPINED_BALL_TRAP:
				return disarm_spined_ball(ttmp);
			case PENDULUM_TRAP:
				return disarm_pendulum(ttmp);
			case MACE_TRAP:
				return disarm_mace_trap(ttmp);
			case DAGGER_TRAP:
				return disarm_dagger_trap(ttmp);
			case FIRE_TRAP:
				return disarm_fire_trap(ttmp);
			case PIT:
			case SPIKED_PIT:
			case GIANT_CHASM:
			case SHIT_PIT:
			case MANA_PIT:
			case ANOXIC_PIT:
			case HYPOXIC_PIT:
			case ACID_PIT:
				if (!u.dx && !u.dy) {
				    You("are already on the edge of the pit.");
				    return 0;
				}
				if (!(mtmp = m_at(x,y))) {
				    pline("Try filling the pit instead.");
				    return 0;
				}
				return help_monster_out(mtmp, ttmp);

			case ROCKTRAP:
			case FALLING_ROCK_COLD:
				if (Defusing) return disarm_hard_shooting_trap(ttmp, ROCK);
				else {
					You("cannot disable %s trap.", (u.dx || u.dy) ? "that" : "this");
					return 0;
				}
			case CALTROPS_TRAP:
				if (Defusing) return disarm_hard_shooting_trap(ttmp, CALTROP);
				else {
					You("cannot disable %s trap.", (u.dx || u.dy) ? "that" : "this");
					return 0;
				}
			case VENOM_SPRINKLER:
				if (Defusing) return disarm_hard_shooting_trap(ttmp, rn2(2) ? ACID_VENOM : BLINDING_VENOM);
				else {
					You("cannot disable %s trap.", (u.dx || u.dy) ? "that" : "this");
					return 0;
				}

			case SPEAR_TRAP:
			case ROLLING_BOULDER_TRAP:
			case SLP_GAS_TRAP:
			case CHLOROFORM_TRAP:
			case TELEP_TRAP:
			case LEVEL_TELEP:
			case STATUE_TRAP:
			case MAGIC_TRAP:
			case ANTI_MAGIC:
			case POLY_TRAP:
			case ICE_TRAP:
			case COLLAPSE_TRAP:
			case MAGIC_BEAM_TRAP:
			case SHIT_TRAP:
			case ANIMATION_TRAP:
			case INTRINSIC_STEAL_TRAP:
			case SCORE_AXE_TRAP:
			case SCORE_DRAIN_TRAP:
			case SINGLE_UNIDENTIFY_TRAP:
			case UNLUCKY_TRAP:
			case ALIGNMENT_REDUCTION_TRAP:
			case MALIGNANT_TRAP:
			case STAT_DAMAGE_TRAP:
			case HALF_MEMORY_TRAP:
			case HALF_TRAINING_TRAP:
			case DEBUFF_TRAP:
			case TRIP_ONCE_TRAP:
			case NARCOLEPSY_TRAP:
			case RETURN_TRAP:
			case POISON_GAS_TRAP:
			case SLOW_GAS_TRAP:
			case SHOCK_TRAP:
			case VULN_TRAP:
			case GRAVITY_TRAP:
			case LOCK_TRAP:
			case MAGIC_CANCELLATION_TRAP:
			case CYANIDE_TRAP:
			case PHOSGENE_TRAP:
			case LASER_TRAP:
			case CONFUSE_TRAP:
			case STUN_TRAP:
			case DIMNESS_TRAP:
			case HALLUCINATION_TRAP:
			case NUMBNESS_TRAP:
			case FREEZING_TRAP:
			case BURNING_TRAP:
			case FEAR_TRAP:
			case BLINDNESS_TRAP:
			case GLIB_TRAP:
			case UNLIGHT_TRAP:
			case ELEMENTAL_TRAP:
			case ESCALATING_TRAP:
			case NEGATIVE_TRAP:
			case MANA_TRAP:
			case GENETIC_TRAP:
			case MISSINGNO_TRAP:
			case CANCELLATION_TRAP:
			case HOSTILITY_TRAP:
			case FALLING_BOULDER_TRAP:
			case OUT_OF_MAGIC_TRAP:
			case PLASMA_TRAP:
			case BOMB_TRAP:
			case EARTHQUAKE_TRAP:
			case NOISE_TRAP:
			case GLUE_TRAP:
			case VOLT_TRAP:
			case CORROSION_TRAP:
			case WITHER_TRAP:
			case FLAME_TRAP:
			case BANANA_TRAP:
			case FALLING_TUB_TRAP:
			case ALARM:
			case BLADE_WIRE:
			case MAGNET_TRAP:
			case CANNON_TRAP:
			case FUMAROLE:
			case FUMBLING_TRAP:
			case NEXUS_TRAP:
			case LEG_TRAP:
			case LEVITATION_TRAP:
			case BOWEL_CRAMPS_TRAP:
			case SINCOUNT_TRAP:
			case BEAMER_TRAP:
			case PHASEPORTER:
			case PHASE_BEAMER:
			case LEVEL_BEAMER:
			case PIERCING_BEAM_TRAP:
			case WRENCHING_TRAP:
			case BACK_TO_START_TRAP:
			case PUNISHMENT_TRAP:
			case CONTAMINATION_TRAP:
			case LASTING_AMNESIA_TRAP:
			case PAIN_TRAP:
			case TREMBLING_TRAP:
			case TECHCAP_TRAP:
			case SPELL_MEMORY_TRAP:
			case SKILL_REDUCTION_TRAP:
			case SKILLCAP_TRAP:
			case PERMANENT_STAT_DAMAGE_TRAP:
			case MIGUC_TRAP:
			case SATATUE_TRAP:
			case DIRECTIVE_TRAP:
			case SANITY_INCREASE_TRAP:
			case PSI_TRAP:
			case CORONA_TRAP:
			case UNPROOFING_TRAP:
			case VISIBILITY_TRAP:
			case BRANCH_TELEPORTER:
			case BRANCH_BEAMER:
			case MOTH_LARVAE_TRAP:
			case STRIKETHROUGH_TRAP:
			case VIVISECTION_TRAP:
			case SKILL_POINT_LOSS_TRAP:
			case TECHSTOP_TRAP:
			case TENTH_TRAP:
			case DEBT_TRAP:
			case INVERSION_TRAP:
			case FUCK_OVER_TRAP:
			case WINCE_TRAP:
			case BURDEN_TRAP:
			case MAGIC_VACUUM_TRAP:
			case RAZOR_TRAP:
			case VULNERATE_TRAP:
			case ALIGNMENT_TRASH_TRAP:
			case DOGSIDE_TRAP:
			case BANKRUPT_TRAP:
			case MALEVOLENCE_TRAP:
			case STATHALF_TRAP:
			case CUTSTAT_TRAP:
				if (Defusing) return disarm_difficult_trap(ttmp);
				else {
					You("cannot disable %s trap.", (u.dx || u.dy) ? "that" : "this");
					return 0;
				}

			default:
				You("cannot disable %s trap.", (u.dx || u.dy) ? "that" : "this");
				return 0;
		    }
		}
	} /* end if */

	if(!u.dx && !u.dy) {
	    for(otmp = level.objects[x][y]; otmp; otmp = otmp->nexthere)
		if(Is_box(otmp)) {
		    sprintf(qbuf, "There is %s here. Check it for traps?",
			safe_qbuf("", sizeof("There is  here. Check it for traps?"),
				doname(otmp), an(simple_typename(otmp->otyp)), "a box"));
		    switch (ynq(qbuf)) {
			case 'q': return(0);
			case 'n': continue;
		    }
		    if (u.usteed && !(uwep && uwep->oartifact == ART_SORTIE_A_GAUCHE) && !(powerfulimplants() && uimplant && uimplant->oartifact == ART_READY_FOR_A_RIDE) && !FemtrapActiveKerstin && !(bmwride(ART_DEEPER_LAID_BMW)) && (PlayerCannotUseSkills || P_SKILL(P_RIDING) < P_BASIC) ) {
			You("aren't skilled enough to reach from %s.",
				mon_nam(u.usteed));
			return(0);
		    }
		    if((otmp->otrapped && (force || (!confused
				&& rn2(MAXULEV + 1 - (issoviet ? u.ulevel : rn2(u.ulevel) ) ) < 10)))
		       || (!force && confused && !rn2(3))) {
			if (!issoviet) You("find a trap on %s!", the(xname(otmp)));
			else pline("Tip bloka l'da polozhit' lovushku na etoy grudi, no on takzhe sdelal eto tak, chto vy vsegda mozhete nayti yego, chto v etom sluchaye vy sdelali.");
			if (!confused) exercise(A_WIS, TRUE);

			switch (ynq("Disarm it?")) {
			    case 'q': return(1);
			    case 'n': trap_skipped = TRUE;  continue;
			}

			if(otmp->otrapped) {
			    exercise(A_DEX, TRUE);
			    ch = ACURR(A_DEX) + u.ulevel;
			    if (Role_if(PM_ROGUE)) ch *= 2;
			    if (Role_if(PM_CYBERNINJA)) ch *= rnd(6);
			    if(!force && (confused || Fumbling ||
				rnd(75+level_difficulty()/2) > ch)) {
				(void) chest_trap(otmp, FINGER, TRUE);
			    } else {
				You("disarm it!");
				u.cnd_untrapamount++;
				use_skill(P_SEARCHING,1);
				if (u.ualign.type == A_LAWFUL) adjalign(1);
				otmp->otrapped = 0;
			    }
			} else pline("That %s was not trapped.", xname(otmp));
			return(1);
		    } else {
			You("find no traps on %s.", the(xname(otmp)));
			return(1);
		    }
		}

	    You(trap_skipped ? "find no other traps here."
			     : "know of no traps here.");
	    return(0);
	}

	if ((mtmp = m_at(x,y))				&&
		mtmp->m_ap_type == M_AP_FURNITURE	&&
		(mtmp->mappearance == S_hcdoor ||
			mtmp->mappearance == S_vcdoor)	&&
		!Protection_from_shape_changers)	 {

	    stumble_onto_mimic(mtmp);
	    return(1);
	}

	if (!IS_DOOR(levl[x][y].typ)) {
	    if ((ttmp = t_at(x,y)) && ttmp->tseen)
		You("cannot disable that trap.");
	    else
		You("know of no traps there.");
	    return(0);
	}

	switch (levl[x][y].doormask) {
	    case D_NODOOR:
		You("%s no door there.", Blind ? "feel" : "see");
		return(0);
	    case D_ISOPEN:
		pline("This door is safely open.");
		return(0);
	    case D_BROKEN:
		pline("This door is broken.");
		return(0);
	}

	if ((levl[x][y].doormask & D_TRAPPED
	     && (force ||
		 (!confused && rn2(MAXULEV - u.ulevel + 11) < 10)))
	    || (!force && confused && !rn2(3))) {
		You("find a trap on the door!");
		exercise(A_WIS, TRUE);
		if (ynq("Disarm it?") != 'y') return(1);
		if (levl[x][y].doormask & D_TRAPPED) {
		    ch = 15 + (Role_if(PM_ROGUE) ? u.ulevel*3 : u.ulevel);
		    exercise(A_DEX, TRUE);
		    if(!force && (confused || Fumbling ||
				     rnd(75+level_difficulty()/2) > ch)) {
			You("set it off!");
			b_trapped("door", FINGER);
			levl[x][y].doormask = D_NODOOR;
			unblock_point(x, y);
			newsym(x, y);
			/* (probably ought to charge for this damage...) */
			if (*in_rooms(x, y, SHOPBASE)) add_damage(x, y, 0L);
		    } else {
			You("disarm it!");
			u.cnd_untrapamount++;
			use_skill(P_SEARCHING,1);
			if (u.ualign.type == A_LAWFUL) adjalign(1);
			levl[x][y].doormask &= ~D_TRAPPED;
		    }
		} else pline("This door was not trapped.");
		return(1);
	} else {
		You("find no traps on the door.");
		return(1);
	}
}
#endif /* OVL2 */
#ifdef OVLB

/* only called when the player is doing something to the chest directly */
boolean
chest_trap(obj, bodypart, disarm)
register struct obj *obj;
register int bodypart;
boolean disarm;
{
	register struct obj *otmp = obj, *otmp2;
	register struct trap *ttmp;
	char	buf[80];
	const char *msg;
	coord cc;

	if (get_obj_location(obj, &cc.x, &cc.y, 0))	/* might be carried */
	    obj->ox = cc.x,  obj->oy = cc.y;

	otmp->otrapped = 0;	/* trap is one-shot; clear flag first in case
				   chest kills you and ends up in bones file */
	You(disarm ? "set it off!" : "trigger a trap!");

	/* in the Evil Variant, those traps are always created --Amy */
	if (!rn2(10) || evilfriday) {

		if (isok(u.ux, u.uy) && !(t_at(u.ux, u.uy)) ) {
			ttmp = maketrap(u.ux, u.uy, rndtrap(), 100, FALSE);
			if (ttmp) {
				ttmp->tseen = 0;
				ttmp->hiddentrap = 1;
			}
		}

		int tryct = 0;
		int x, y;
		boolean canbeinawall = FALSE;
		if (!rn2(Passes_walls ? 5 : 25)) canbeinawall = TRUE;

		for (tryct = 0; tryct < 2000; tryct++) {
			x = rn1(COLNO-3,2);
			y = rn2(ROWNO);

			if (isok(x, y) && ((levl[x][y].typ > DBWALL) || canbeinawall) && !(t_at(x, y)) ) {
					ttmp = maketrap(x, y, rndtrap(), 0, FALSE);
				if (ttmp) {
					ttmp->tseen = 0;
					ttmp->hiddentrap = 1;
				}
				if (rn2(3)) break;
			}
		}
	}

	display_nhwindow(WIN_MESSAGE, FALSE);
	if (Luck > -13 && rn2(13+Luck) > 7) {	/* saved by luck */
	    /* trap went off, but good luck prevents damage */
	    switch (rn2(13)) {
		case 12:
		case 11:  msg = "explosive charge is a dud";  break;
		case 10:
		case  9:  msg = "electric charge is grounded";  break;
		case  8:
		case  7:  msg = "flame fizzles out";  break;
		case  6:
		case  5:
		case  4:  msg = "poisoned needle misses";  break;
		case  3:
		case  2:
		case  1:
		case  0:  msg = "gas cloud blows away";  break;
		default:  impossible("chest disarm bug");  msg = (char *)0;
			  break;
	    }
	    if (msg) pline("But luckily the %s!", msg);
	} else {
	    switch(rn2(20) ? ((Luck >= 13) ? 0 : rn2(13-Luck)) : rn2(26)) {
		case 25:
		case 24:
		case 23:
		case 22:
		case 21: {
			  struct monst *shkp = 0;
			  long loss = 0L;
			  boolean costly, insider;
			  register xchar ox = obj->ox, oy = obj->oy;

			  /* the obj location need not be that of player */
			  costly = (costly_spot(ox, oy) &&
				   (shkp = shop_keeper(*in_rooms(ox, oy,
				    SHOPBASE))) != (struct monst *)0);
			  insider = (*u.ushops && inside_shop(u.ux, u.uy) &&
				    *in_rooms(ox, oy, SHOPBASE) == *u.ushops);

			  pline("%s!", Tobjnam(obj, "explode"));
			  sprintf(buf, "exploding %s", xname(obj));

			  if(costly)
			      loss += stolen_value(obj, ox, oy,
				      (boolean)shkp->mpeaceful, TRUE, TRUE);
			  delete_contents(obj);
			  /* we're about to delete all things at this location,
			   * which could include the ball & chain.
			   * If we attempt to call unpunish() in the
			   * for-loop below we can end up with otmp2
			   * being invalid once the chain is gone.
			   * Deal with ball & chain right now instead.
			   */
			  if (Punished && !(uchain && uchain->oartifact == ART_DON_T_GO_AWAY) && !(uball && uball->oartifact == ART_DOCKEM_GOOD) && !carried(uball) &&
				((uchain->ox == u.ux && uchain->oy == u.uy) ||
				 (uball->ox == u.ux && uball->oy == u.uy)))
				unpunish();

			  for(otmp = level.objects[u.ux][u.uy];
							otmp; otmp = otmp2) {
			      otmp2 = otmp->nexthere;
			      if(costly)
				  loss += stolen_value(otmp, otmp->ox,
					  otmp->oy, (boolean)shkp->mpeaceful,
					  TRUE, TRUE);
			      delobj(otmp);
			  }
			  wake_nearby();
			  losehp(d(6,6), buf, KILLED_BY_AN);
			  exercise(A_STR, FALSE);
			  if(costly && loss) {
			      if(insider)
			      You("owe %ld %s for objects destroyed.",
							loss, currency(loss));
			      else {
				  You("caused %ld %s worth of damage!",
							loss, currency(loss));
				  make_angry_shk(shkp, ox, oy);
			      }
			  }
			  return TRUE;
			}
		case 20:
		case 19:
		case 18:
		case 17:
			pline("A cloud of noxious gas billows from %s.",
							the(xname(obj)));
			poisoned("gas cloud", A_STR, "cloud of poison gas",15);
			exercise(A_CON, FALSE);
			break;
		case 16:
		case 15:
		case 14:
		case 13:
			You_feel("a needle prick your %s.",body_part(bodypart));
			poisoned("needle", A_CON, "poisoned needle",10);
			exercise(A_CON, FALSE);
			break;
		case 12:
		case 11:
		case 10:
		case 9:
			dofiretrap(obj);
			break;
		case 8:
		case 7:
		case 6: {
			int dmg;

			You("are jolted by a surge of electricity!");
			if ((Shock_resistance && (StrongShock_resistance || rn2(10))) || ShockImmunity ) {
			    shieldeff(u.ux, u.uy);
			    You("don't seem to be affected.");
			    dmg = 0;
			} else
			    dmg = d(4, 4);
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) /* new calculations --Amy */	destroy_item(RING_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) /* new calculations --Amy */	destroy_item(WAND_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 30 : 165)) /* new calculations --Amy */	destroy_item(AMULET_CLASS, AD_ELEC);
			if (dmg) losehp(dmg, "electric shock", KILLED_BY_AN);
			break;
		      }
		case 5:
		case 4:
		case 3:
			if (!Free_action || !rn2(StrongFree_action ? 100 : 20)) {                        
			pline("Suddenly you are frozen in place!");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
			if (isstunfish) nomul(-rnz(11), "frozen by a container trap", TRUE);
			else nomul(-rn1(5, 6), "frozen by a container trap", TRUE);
			exercise(A_DEX, FALSE);
			nomovemsg = You_can_move_again;
			} else You("momentarily stiffen.");
			break;
		case 2:
		case 1:
		case 0:
			pline("A cloud of %s gas billows from %s.",
				Blind ? blindgas[rn2(SIZE(blindgas))] :
				rndcolor(), the(xname(obj)));
			if(!Stunned) {
			    if (FunnyHallu)
				pline("What a groovy feeling!");
			    else if (Blind)
				You("%s and get dizzy...",
				    stagger(youmonst.data, "stagger"));
			    else
				You("%s and your vision blurs...",
				    stagger(youmonst.data, "stagger"));
			}
			if (PlayerHearsSoundEffects) pline(issoviet ? "Imet' delo s effektami statusa ili sdat'sya!" : "Wrueue-ue-e-ue-e-ue-e...");
			make_stunned(HStun + rn1(7, 16),FALSE);
			(void) make_hallucinated(HHallucination + rn1(5, 16),FALSE,0L);
			break;
		default: impossible("bad chest trap");
			break;
	    }
	    bot();			/* to get immediate botl re-display */
	}
	return FALSE;
}

#endif /* OVLB */
#ifdef OVL0

struct trap *
t_at(x,y)
register int x, y;
{
	register struct trap *trap = ftrap;
	while(trap) {
		if(trap->tx == x && trap->ty == y) return(trap);
		trap = trap->ntrap;
	}
	return((struct trap *)0);
}

#endif /* OVL0 */
#ifdef OVLB

void
deltrap(trap)
register struct trap *trap;
{
	register struct trap *ttmp;

	if (!trap) return; 
	if(trap == ftrap)
		ftrap = ftrap->ntrap;
	else {
		for(ttmp = ftrap; ttmp && ttmp->ntrap != trap; ttmp = ttmp->ntrap) ;
		if (!trap) return; 
		ttmp->ntrap = trap->ntrap;
	}
	dealloc_trap(trap);
}

boolean
delfloortrap(ttmp)
register struct trap *ttmp;
{
	/* Destroy a trap that emanates from the floor. */
	/* some of these are arbitrary -dlc */
	if (ttmp && ((ttmp->ttyp == SQKY_BOARD) ||
		     (ttmp->ttyp == BEAR_TRAP) ||
		     (ttmp->ttyp == LANDMINE) ||
		     (ttmp->ttyp == FIRE_TRAP) ||
		     (ttmp->ttyp == ICE_TRAP) ||
		     (ttmp->ttyp == PIT) ||
		     (ttmp->ttyp == SPIKED_PIT) ||
		     (ttmp->ttyp == HOLE) ||
		     (ttmp->ttyp == TRAPDOOR) ||
		     (ttmp->ttyp == SHAFT_TRAP) ||
		     (ttmp->ttyp == CURRENT_SHAFT) ||
		     (ttmp->ttyp == SHIT_PIT) ||
		     (ttmp->ttyp == MANA_PIT) ||
		     (ttmp->ttyp == ANOXIC_PIT) ||
		     (ttmp->ttyp == HYPOXIC_PIT) ||
		     (ttmp->ttyp == ACID_PIT) ||
		     (ttmp->ttyp == TELEP_TRAP) ||
		     (ttmp->ttyp == BEAMER_TRAP) ||
		     (ttmp->ttyp == LEVEL_TELEP) ||
		     (ttmp->ttyp == LEVEL_BEAMER) ||
		     (ttmp->ttyp == WEB) ||
		     (ttmp->ttyp == MAGIC_TRAP) ||
		     (ttmp->ttyp == ANTI_MAGIC))) {
	    register struct monst *mtmp;

	    if (ttmp->tx == u.ux && ttmp->ty == u.uy) {
		u.utrap = 0;
		u.utraptype = 0;
	    } else if ((mtmp = m_at(ttmp->tx, ttmp->ty)) != 0) {
		mtmp->mtrapped = 0;
	    }
	    deltrap(ttmp);
	    return TRUE;
	} else
	    return FALSE;
}

/* used for doors (also tins).  can be used for anything else that opens. */
void
b_trapped(item, bodypart)
register const char *item;
register int bodypart;
{
	register int lvl = level_difficulty();
	register struct trap *ttmp;
	int dmg = rnd(5 + (lvl < 5 ? lvl : 2+lvl/2));

	if (Role_if(PM_CYBERNINJA) && rn2(5) && !(DoorningEffect || u.uprops[DOORNING_EFFECT].extrinsic || have_doorningstone()) ) {
		You("safely discharge the trap found on %s.", the(item));
		return;
	}

	pline("KABOOM!!  %s was booby-trapped!", The(item));
	wake_nearby();
	losehp(dmg, "explosion", KILLED_BY_AN);
	exercise(A_STR, FALSE);
	if (bodypart) exercise(A_CON, FALSE);
	make_stunned(HStun + dmg, TRUE);

	/* Grunthack door traps are evil, so they have to be evil in the Evil Variant too --Amy */

	if (evilfriday || DoorningEffect || u.uprops[DOORNING_EFFECT].extrinsic || have_doorningstone()) {
		(void) destroy_item(POTION_CLASS, AD_FIRE);
		(void) destroy_item(SCROLL_CLASS, AD_FIRE);
		(void) destroy_item(SPBOOK_CLASS, AD_FIRE);
		burnarmor(&youmonst);
	}

	if ((evilfriday || DoorningEffect || u.uprops[DOORNING_EFFECT].extrinsic || have_doorningstone()) && !rn2(10)) {
		pline("SCREEEEEECH!");
		aggravate();
	}

	if ((evilfriday || DoorningEffect || u.uprops[DOORNING_EFFECT].extrinsic || have_doorningstone()) && !rn2(10)) {
		pline("A static discharge shoots through your entire body!");
		destroy_item(WAND_CLASS, AD_ELEC);
		destroy_item(RING_CLASS, AD_ELEC);
		destroy_item(AMULET_CLASS, AD_ELEC);
	}

	if ((evilfriday || DoorningEffect || u.uprops[DOORNING_EFFECT].extrinsic || have_doorningstone()) && !rn2(10)) {
		pline("Hahaha, a water bucket falls on top of you and all your shit gets wet! LOL!");
		if ((!StrongSwimming || !rn2(10)) && (!StrongMagical_breathing || !rn2(10))) {
			water_damage(invent, FALSE, FALSE);
			if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);
		}
	}

	if ((evilfriday || DoorningEffect || u.uprops[DOORNING_EFFECT].extrinsic || have_doorningstone()) && !rn2(10)) {
		register struct obj *otmp3;
		otmp3 = mksobj(BOULDER, FALSE, FALSE, FALSE);
		if (!otmp3) goto boulderdone;
		otmp3->quan = 1;
		otmp3->owt = weight(otmp3);
		pline("A boulder slams on top of you!");
		if (amorphous(youmonst.data) || Passes_walls || noncorporeal(youmonst.data) || unsolid(youmonst.data)) pline("Even though you should be immune to it, the boulder damages you anyway, and you hear a voice announce: 'Harharharharharharhar!'");

		/* Must be before the losehp(), for bones files */
		if (!flooreffects(otmp3, u.ux, u.uy, "fall")) {
			place_object(otmp3, u.ux, u.uy);
			stackobj(otmp3);
			newsym(u.ux, u.uy);
		}

		losehp(24, "boulder doortrap", KILLED_BY_AN);

	}

boulderdone:

	if ((evilfriday || DoorningEffect || u.uprops[DOORNING_EFFECT].extrinsic || have_doorningstone()) && !rn2(10)) {
		badeffect();
		if (!rn2(5)) {
			badeffect();
			while (!rn2(3)) badeffect();
		}
	}

	if (!rn2(10) || evilfriday || DoorningEffect || u.uprops[DOORNING_EFFECT].extrinsic || have_doorningstone()) {

		int i, j;

		for (i = -1; i <= 1; i++) for(j = -1; j <= 1; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if (levl[u.ux + i][u.uy + j].typ <= DBWALL) continue;
			if (t_at(u.ux + i, u.uy + j)) continue;

			ttmp = maketrap(u.ux + i, u.uy + j, rndtrap(), 100, FALSE);
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

			if (isok(x, y) && (levl[x][y].typ > DBWALL) && !(t_at(x, y)) ) {
					ttmp = maketrap(x, y, rndtrap(), 100, FALSE);
				if (ttmp) {
					ttmp->tseen = 0;
					ttmp->hiddentrap = 1;
				}
				if (rn2(3)) break;
			}
		}
	}

}

/* Monster is hit by trap. */
/* Note: doesn't work if both obj and d_override are null */
STATIC_OVL boolean
thitm(tlev, mon, obj, d_override, nocorpse)
int tlev;
struct monst *mon;
struct obj *obj;
int d_override;
boolean nocorpse;
{
	int strike;
	boolean trapkilled = FALSE;

	if (d_override) strike = 1;
	else if (obj) strike = (find_mac(mon) + tlev + obj->spe <= rnd(20));
	else strike = (find_mac(mon) + tlev <= rnd(20));

	/* Actually more accurate than thitu, which doesn't take
	 * obj->spe into account.
	 */
	if(!strike) {
		if (obj && cansee(mon->mx, mon->my))
		    pline("%s is almost hit by %s!", Monnam(mon), doname(obj));
	} else {
		int dam = 1;

		if (obj && cansee(mon->mx, mon->my))
			pline("%s is hit by %s!", Monnam(mon), doname(obj));
		if (d_override) dam = d_override;
		else if (obj) {
			dam = dmgval(obj, mon);
			if (dam < 1) dam = 1;
		}
		if ((mon->mhp -= dam) <= 0) {
			int xx = mon->mx;
			int yy = mon->my;

			monkilled(mon, "", nocorpse ? -AD_RBRE : AD_PHYS);
			if (mon->mhp <= 0) {
				newsym(xx, yy);
				trapkilled = TRUE;
			}
		}
	}
	if (obj && (!strike || d_override) && timebasedlowerchance() ) {
		place_object(obj, mon->mx, mon->my);
		stackobj(obj);
	} else if (obj) dealloc_obj(obj);

	return trapkilled;
}

boolean
unconscious()
{
	return((boolean)(multi < 0 && (!nomovemsg ||
		u.usleep ||
		!strncmp(nomovemsg,"You regain con", 14) ||
		!strncmp(nomovemsg,"You are consci", 14))));
}

static const char lava_killer[] = "molten lava";

void
fartingweb()
{
	register struct trap *ttmp = t_at(u.ux, u.uy);
	if (!ttmp) return;
	if (ttmp->ttyp != FARTING_WEB) return;

	if (ttmp->launch_otyp < 12) pline("%s produces %s farting noises with her sexy butt.", farttrapnames[ttmp->launch_otyp], rn2(2) ? "tender" : "soft");
	else if (ttmp->launch_otyp < 33) pline("%s produces %s farting noises with her sexy butt.", farttrapnames[ttmp->launch_otyp], rn2(2) ? "beautiful" : "squeaky");
	else pline("%s produces %s farting noises with her sexy butt.", farttrapnames[ttmp->launch_otyp], rn2(2) ? "disgusting" : "loud");
	u.cnd_fartingcount++;
	if (Role_if(PM_CLIMACTERIAL)) climtrainsqueaking(1);
	if (Role_if(PM_BUTT_LOVER) && !rn2(20)) buttlovertrigger();
	if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();

	if (uarmf && uarmf->oartifact == ART_ELIANE_S_SHIN_SMASH) {
		pline("The farting gas destroys your footwear instantly.");
	      useup(uarmf);
	}

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

void
nofunwalltrigger(x, y)
int x, y;
{
		int funwalltype = levl[x][y].nofunwall;

		if (funwalltype < 12) pline("%s produces %s farting noises with her sexy butt.", farttrapnames[funwalltype], rn2(2) ? "tender" : "soft");
		else if (funwalltype < 33) pline("%s produces %s farting noises with her sexy butt.", farttrapnames[funwalltype], rn2(2) ? "beautiful" : "squeaky");
		else pline("%s produces %s farting noises with her sexy butt.", farttrapnames[funwalltype], rn2(2) ? "disgusting" : "loud");
		u.cnd_fartingcount++;
		if (Role_if(PM_CLIMACTERIAL)) climtrainsqueaking(1);
		if (Role_if(PM_BUTT_LOVER) && !rn2(20)) buttlovertrigger();
		if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();

		if (uarmf && uarmf->oartifact == ART_ELIANE_S_SHIN_SMASH) {
			pline("The farting gas destroys your footwear instantly.");
		      useup(uarmf);
		}

		if (uarmf && uarmf->oartifact == ART_ELIANE_S_COMBAT_SNEAKERS) {
			pline("Eek! You can't stand farting gas!");
			badeffect();
			badeffect();
			badeffect();
			badeffect();
		}

		if (!extralongsqueak()) {
			if (FunwallXtra) reallybadeffect();
			else badeffect();
		}

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

int
yawm_distance()
{
	int yawmdistance = 626;
	if (u.yawmtime > 100) yawmdistance = 601;
	if (u.yawmtime > 200) yawmdistance = 551;
	if (u.yawmtime > 300) yawmdistance = 501;
	if (u.yawmtime > 400) yawmdistance = 451;
	if (u.yawmtime > 500) yawmdistance = 401;
	if (u.yawmtime > 750) yawmdistance = 351;
	if (u.yawmtime > 1000) yawmdistance = 301;
	if (u.yawmtime > 2000) yawmdistance = 251;
	if (u.yawmtime > 3000) yawmdistance = 201;
	if (u.yawmtime > 4000) yawmdistance = 151;
	if (u.yawmtime > 5000) yawmdistance = 101;
	if (u.yawmtime > 6000) yawmdistance = 91;
	if (u.yawmtime > 7000) yawmdistance = 81;
	if (u.yawmtime > 8000) yawmdistance = 71;
	if (u.yawmtime > 9000) yawmdistance = 61;
	if (u.yawmtime > 10000) yawmdistance = 51;
	if (u.yawmtime > 15000) yawmdistance = 41;
	if (u.yawmtime > 20000) yawmdistance = 31;
	if (u.yawmtime > 25000) yawmdistance = 26;
	if (u.yawmtime > 30000) yawmdistance = 21;
	if (u.yawmtime > 35000) yawmdistance = 16;
	if (u.yawmtime > 40000) yawmdistance = 11;
	if (u.yawmtime > 50000) yawmdistance = 9;
	if (u.yawmtime > 75000) yawmdistance = 7;
	if (u.yawmtime > 100000) yawmdistance = 5;
	if (u.yawmtime > 200000) yawmdistance = 1;

	return yawmdistance;
}

boolean
lava_effects()
{
    register struct obj *obj, *obj2;
    int dmg;
    boolean usurvive;

    burn_away_slime();

    if (Slimed) {
	pline("The slime boils away!");
	Slimed = 0;
    }

    if (Frozen) {
	pline("The ice thaws!");
	make_frozen(0L, FALSE);
    }

    if (likes_lava(youmonst.data)) return FALSE;
    if (Race_if(PM_PLAYER_SALAMANDER)) return FALSE;
    if (uamul && uamul->otyp == AMULET_OF_D_TYPE_EQUIPMENT) return FALSE;
    if (powerfulimplants() && uimplant && uimplant->oartifact == ART_RUBBER_SHOALS) return FALSE;
    if (uwep && uwep->oartifact == ART_EVERYTHING_MUST_BURN) return FALSE;
    if (uarm && uarm->oartifact == ART_LAURA_CROFT_S_BATTLEWEAR) return FALSE;
    if (uwep && uwep->oartifact == ART_MANUELA_S_PRACTICANT_TERRO) return FALSE;
    if (uarm && uarm->oartifact == ART_D_TYPE_EQUIPMENT) return FALSE;
    if (uarmc && uarmc->oartifact == ART_SCOOBA_COOBA) return FALSE;
    if (uarmf && uarmf->oartifact == ART_JOHANNA_S_RED_CHARM) return FALSE;
    if (uarmf && itemhasappearance(uarmf, APP_HOT_BOOTS) ) return FALSE;
    if (FireImmunity) return FALSE;

    if (!Fire_resistance) {

	if(Wwalking || Race_if(PM_KORONST)) {
	    dmg = d(6,6);
	    pline_The("lava here burns you!");
	    if(dmg < u.uhp) {
		losehp(dmg, lava_killer, KILLED_BY);
		goto burn_stuff;
	    }
	} else
	    You("fall into the lava!");

	usurvive = Lifesaved || Second_chance || discover;
#ifdef WIZARD
	if (wizard) usurvive = TRUE;
#endif
	for(obj = invent; obj; obj = obj2) {
	    obj2 = obj->nobj;
	    if(is_organic(obj) && !obj->oerodeproof && !(uarmc && itemhasappearance(uarmc, APP_VOLCANIC_CLOAK) && rn2(2)) && !rn2(obj->blessed ? 15 : 3) && !stack_too_big(obj)) {
		if(obj->owornmask) {
		    if (usurvive)
			Your("%s into flame!", aobjnam(obj, "burst"));

		    if(obj == uarm) (void) Armor_gone();
		    else if(obj == uarmc) (void) Cloak_off();
		    else if(obj == uarmh) (void) Helmet_off();
		    else if(obj == uarms) (void) Shield_off();
		    else if(obj == uarmg) (void) Gloves_off();
		    else if(obj == uarmf) (void) Boots_off();
		    else if(obj == uarmu) setnotworn(obj);
		    else if(obj == uleft) Ring_gone(obj);
		    else if(obj == uright) Ring_gone(obj);
		    else if(obj == ublindf) Blindf_off(obj);
		    else if(obj == uamul) Amulet_off();
		    else if(obj == uimplant) Implant_off();
		    else if(obj == uwep) uwepgone();
		    else if (obj == uquiver) uqwepgone();
		    else if (obj == uswapwep) uswapwepgone();
		}
		/* losing your main container will screw you over completely, so they get another saving throw --Amy */
		if (Is_container(obj)) {

			switch (obj->otyp) {
				case LEAD_BOX:
				case TOP_BOX:
				case ICE_BOX:
				case DISPERSION_BOX:
				case ICE_BOX_OF_HOLDING:
				case ICE_BOX_OF_WATERPROOFING:
				case ICE_BOX_OF_DIGESTION:
				continue;		/* Immune */
				/*NOTREACHED*/
				break;
			case CHEST:
			case NANO_CHEST:
			case CHEST_OF_HOLDING:
				if (rnd(10) < 7) continue; /* 60% chance of survival */
				break;
			case LARGE_BOX:
			case LARGE_BOX_OF_DIGESTION:
				if (rnd(20) < 12) continue; /* 55% chance of survival */
				break;
			case TREASURE_CHEST:
			case LOOT_CHEST:
				continue;		/* Immune, if you somehow get one into your inventory :P */
				/*NOTREACHED*/
				break;
			default:
				if (!rn2(2)) continue; /* 50% chance of survival */
				break;
			}
		}
		useupall(obj);
	    }
	}

	/* Amy edit: let's be nice for once, and change the behavior of lava so that it's not always an instakill.
	 * Now, if you have more than 10 max HP, it reduces the maximum by half (but doesn't take off more than 50 at once)
	 * and puts you in the lava, so it's still very dangerous, but at least your game doesn't necessarily end
	 * from a single misstep, and also we made the item destruction less harsh */
	if (u.uhpmax > 10) {

		pline_The("lava here burns you, and your health is severely damaged!");
		losehp(rnd(36), lava_killer, KILLED_BY);
		if (u.uhpmax > 100) u.uhpmax -= 50;
		else u.uhpmax /= 2;
		if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;

	} else {

	/* s/he died... */
	u.youaredead = 1;
	u.uhp = -1;
	killer_format = KILLED_BY;
	killer = lava_killer;
	You("burn to a crisp...");
	if (PlayerHearsSoundEffects) pline(issoviet ? "Do svidaniya! Vy, navernoye, sdelal oshibku khodit' vokrug vo vremya oglusheniya ili sputannost' soznaniya, i teper' vy zaplatili samuyu vysokuyu tsenu." : "Ohoho-ho-ho!");
	done(BURNING);
	u.youaredead = 0;
	while (!safe_teleds_normalterrain(TRUE)) {
		u.youaredead = 1;
		pline("You're still burning.");
		done(BURNING);
		u.youaredead = 0;
	}

	You("find yourself back on solid %s.", surface(u.ux, u.uy));
	return(TRUE);

	} /* player had less than 11 max HP */

    }

    if (!Wwalking && !Race_if(PM_KORONST)) {
	u.utrap = rn1(4, 4) + (rn1(4, 12) << 8);
	u.utraptype = TT_LAVA;
	if (Fire_resistance) You("sink into the lava, but it only burns slightly!");
	else You("rapidly sink into the lava!");
	if (u.uhp > 1)
	    losehp(1, lava_killer, KILLED_BY);
    }
    /* just want to burn boots, not all armor; destroy_item doesn't work on armor anyway */
burn_stuff:
    if(uarmf && !uarmf->oerodeproof && is_organic(uarmf) && !(uarmc && itemhasappearance(uarmc, APP_VOLCANIC_CLOAK) && rn2(2)) && !rn2(uarmf->blessed ? 15 : 3)) {
	/* save uarmf value because Boots_off() sets uarmf to null */
	obj = uarmf;
	Your("%s bursts into flame!", xname(obj));
	(void) Boots_off();
	useup(obj);
    }
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 3 : 15)) /* new calculations --Amy */    destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 3 : 15)) /* new calculations --Amy */    destroy_item(SPBOOK_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 3 : 15)) /* new calculations --Amy */    destroy_item(POTION_CLASS, AD_FIRE);
    return(FALSE);
}

#endif /* OVLB */

/*trap.c*/
