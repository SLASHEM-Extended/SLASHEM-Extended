/*	SCCS Id: @(#)muse.c	3.4	2002/12/23	*/
/*	Copyright (C) 1990 by Ken Arromdee			   */
/* NetHack may be freely redistributed.  See license for details.  */

/*
 * Monster item usage routines.
 */

#include "hack.h"
#include "edog.h"

extern const int monstr[];

boolean m_using = FALSE;

/* Let monsters use magic items.  Arbitrary assumptions: Monsters only use
 * scrolls when they can see, monsters know when wands have 0 charges, monsters
 * cannot recognize if items are cursed are not, monsters which are confused
 * don't know not to read scrolls, etc....
 */

#if 0
STATIC_DCL struct permonst *muse_newcham_mon(struct monst *);
#endif
STATIC_DCL int precheck(struct monst *,struct obj *);
STATIC_DCL void mzapmsg(struct monst *,struct obj *,BOOLEAN_P);
STATIC_DCL void mreadmsg(struct monst *,struct obj *);
STATIC_DCL void mquaffmsg(struct monst *,struct obj *);
STATIC_PTR int mbhitm(struct monst *,struct obj *);
STATIC_DCL void mbhit(struct monst *,int,int (*)(MONST_P,OBJ_P),
	int (*)(OBJ_P,OBJ_P), struct obj *);
STATIC_DCL void you_aggravate(struct monst *);
STATIC_DCL void mon_consume_unstone(struct monst *,struct obj *,
	BOOLEAN_P,BOOLEAN_P);

static struct musable {
	struct obj *offensive;
	struct obj *defensive;
	struct obj *misc;
	int has_offense, has_defense, has_misc;
	/* =0, no capability; otherwise, different numbers.
	 * If it's an object, the object is also set (it's 0 otherwise).
	 */
} m;
static int trapx, trapy;
static boolean zap_oseen;
	/* for wands which use mbhitm and are zapped at players.  We usually
	 * want an oseen local to the function, but this is impossible since the
	 * function mbhitm has to be compatible with the normal zap routines,
	 * and those routines don't remember who zapped the wand.
	 */

/* Any preliminary checks which may result in the monster being unable to use
 * the item.  Returns 0 if nothing happened, 2 if the monster can't do anything
 * (i.e. it teleported) and 1 if it's dead.
 */

STATIC_PTR void
do_floodd(x, y, poolcnt)
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
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = POOL;
			del_engr_at(randomx, randomy);
			water_damage(level.objects[randomx][randomy], FALSE, TRUE);
	
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

STATIC_PTR void
do_lavafloodd(x, y, poolcnt)
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
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = LAVAPOOL;
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
do_gravefloodd(x, y, poolcnt)
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
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = GRAVEWALL;
			del_engr_at(randomx, randomy);
			blockorunblock_point(randomx,randomy);
	
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
		levl[x][y].typ = GRAVEWALL;
		del_engr_at(x, y);
		blockorunblock_point(x,y);

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
do_tunnelfloodd(x, y, poolcnt)
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
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = TUNNELWALL;
			del_engr_at(randomx, randomy);
			blockorunblock_point(randomx,randomy);
	
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
		levl[x][y].typ = TUNNELWALL;
		del_engr_at(x, y);
		blockorunblock_point(x,y);

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
do_farmfloodd(x, y, poolcnt)
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
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = FARMLAND;
			del_engr_at(randomx, randomy);
			blockorunblock_point(randomx,randomy);
	
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
		levl[x][y].typ = FARMLAND;
		del_engr_at(x, y);
		blockorunblock_point(x,y);

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
do_mountainfloodd(x, y, poolcnt)
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
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = MOUNTAIN;
			del_engr_at(randomx, randomy);
			blockorunblock_point(randomx,randomy);
	
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
		levl[x][y].typ = MOUNTAIN;
		del_engr_at(x, y);
		blockorunblock_point(x,y);

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
do_watertunnelfloodd(x, y, poolcnt)
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
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = WATERTUNNEL;
			del_engr_at(randomx, randomy);
			blockorunblock_point(randomx,randomy);
	
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
		levl[x][y].typ = WATERTUNNEL;
		del_engr_at(x, y);
		blockorunblock_point(x,y);

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
do_crystalwaterfloodd(x, y, poolcnt)
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
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

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
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
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
do_moorfloodd(x, y, poolcnt)
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
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

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
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
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
do_urinefloodd(x, y, poolcnt)
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
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

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
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
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
do_shiftingsandfloodd(x, y, poolcnt)
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
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

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
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
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
do_styxfloodd(x, y, poolcnt)
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
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

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
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
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
do_snowfloodd(x, y, poolcnt)
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
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

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
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
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
do_ashfloodd(x, y, poolcnt)
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
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

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
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
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
do_sandfloodd(x, y, poolcnt)
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
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

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
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
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
do_pavementfloodd(x, y, poolcnt)
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
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

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
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
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
do_highwayfloodd(x, y, poolcnt)
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
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

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
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
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
do_grassfloodd(x, y, poolcnt)
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
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

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
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
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
do_nethermistfloodd(x, y, poolcnt)
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
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

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
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
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
do_stalactitefloodd(x, y, poolcnt)
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
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

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
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
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
do_cryptfloodd(x, y, poolcnt)
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
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

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
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
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
do_bubblefloodd(x, y, poolcnt)
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
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = BUBBLES;
			del_engr_at(randomx, randomy);
			blockorunblock_point(randomx,randomy);
	
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
		levl[x][y].typ = BUBBLES;
		del_engr_at(x, y);
		blockorunblock_point(x,y);

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
do_raincloudfloodd(x, y, poolcnt)
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
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = RAINCLOUD;
			del_engr_at(randomx, randomy);
			blockorunblock_point(randomx,randomy);
	
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
		levl[x][y].typ = RAINCLOUD;
		del_engr_at(x, y);
		blockorunblock_point(x,y);

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
do_lockflooddboulder(x, y, poolcnt)
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
			(void) mksobj_at(BOULDER, randomx, randomy, TRUE, FALSE, FALSE);
		}
	}
	if (rn2(3)) {
		doorlockX(x, y, TRUE);
		if (levl[x][y].typ == DOOR) (void) mksobj_at(BOULDER, x, y, TRUE, FALSE, FALSE);
	}

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
		(void) mksobj_at(BOULDER, x, y, TRUE, FALSE, FALSE);

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

STATIC_OVL int
precheck(mon, obj)
struct monst *mon;
struct obj *obj;
{
	boolean vis;

	if (!obj) return 0;
	vis = cansee(mon->mx, mon->my);

	if (obj->oclass == POTION_CLASS) {
	    coord cc;
	    static const char *empty = "The potion turns out to be empty.";
	    struct monst *mtmp;
#define POTION_OCCUPANT_CHANCE(n) (13 + 2*(n))	/* also in potion.c */

	    if (itemhasappearance(obj, APP_POTION_MILKY) || itemhasappearance(obj, APP_POTION_GHOSTLY) || itemhasappearance(obj, APP_POTION_HALLOWED) || itemhasappearance(obj, APP_POTION_CAMPING) || itemhasappearance(obj, APP_POTION_SPIRITUAL)) {
	        if ( flags.ghost_count < MAXMONNO && !rn2(POTION_OCCUPANT_CHANCE(flags.ghost_count))) {
		    if (!enexto(&cc, mon->mx, mon->my, &mons[PM_GHOST])) return 0;
		    mquaffmsg(mon, obj);
		    m_useup(mon, obj);
		    mtmp = makemon(&mons[PM_GHOST], cc.x, cc.y, NO_MM_FLAGS);
		    if (!mtmp) {
			if (vis) pline("%s", empty);
		    } else {
			if (vis) {
			    pline("As %s opens the bottle, an enormous %s emerges!",
			       mon_nam(mon),
			       Hallucination ? rndmonnam() : (const char *)"ghost");
			    pline("%s is frightened to death, and unable to move.",
				    Monnam(mon));
			}
			mon->mcanmove = 0;
			mon->mfrozen = 3;
		    }
		    return 2;
		}
	    }
	    if (itemhasappearance(obj, APP_POTION_SMOKY) &&
		    flags.djinni_count < MAXMONNO &&
		    !rn2(POTION_OCCUPANT_CHANCE(flags.djinni_count))) {
		if (!enexto(&cc, mon->mx, mon->my, &mons[PM_DJINNI])) return 0;
		mquaffmsg(mon, obj);
		m_useup(mon, obj);
		mtmp = makemon(&mons[PM_DJINNI], cc.x, cc.y, NO_MM_FLAGS);
		if (!mtmp) {
		    if (vis) pline("%s", empty);
		} else {
		    if (vis)
			pline("In a cloud of smoke, %s emerges!",
							a_monnam(mtmp));
		    pline("%s speaks.", vis ? Monnam(mtmp) : Something);
		/* I suspect few players will be upset that monsters */
		/* can't wish for wands of death here.... */

		/* Amy edit: I am a "few player" obviously. But then, what did you expect of a variant whose author is into BDSM? I'll allow monsters to wish for random musable items; this is different from certain other variants, and may be changed someday, but for now it's good enough for me. */

		    if (rn2(3)) { /* No FIQ, the chances are NOT going to be synchronized between players and monsters,
					 * even though that revelation may come as a shock to you. This is not FIQslex. --Amy */
			verbalize("I am in your debt. I will grant one wish!");
			pline("%s wishes for an object.", Monnam(mon) );
			switch (rnd(6)) {
				case 1:
					(void) mongets(mon, rnd_defensive_item(mon));
					break;
				case 2:
					(void) mongets(mon, rnd_offensive_item(mon));
					break;
				case 3:
					(void) mongets(mon, rnd_misc_item(mon));
					break;
				case 4:
					(void) mongets(mon, rnd_defensive_item_new(mon));
					break;
				case 5:
					(void) mongets(mon, rnd_offensive_item_new(mon));
					break;
				case 6:
					(void) mongets(mon, rnd_misc_item_new(mon));
					break;
			}
			if (vis) pline("%s vanishes.", Monnam(mtmp));
			mongone(mtmp);
		    } else if (rn2(2)) {
			verbalize("You freed me!");
			mtmp->mpeaceful = 1;
			set_malign(mtmp);
		    } else {
			verbalize("It is about time.");
			if (vis) pline("%s vanishes.", Monnam(mtmp));
			mongone(mtmp);
		    }
		}
		return 2;
	    }
	    if (itemhasappearance(obj, APP_POTION_VAPOR) &&
		    flags.dao_count < MAXMONNO &&
		    !rn2(POTION_OCCUPANT_CHANCE(flags.dao_count))) {
		if (!enexto(&cc, mon->mx, mon->my, &mons[PM_DAO])) return 0;
		mquaffmsg(mon, obj);
		m_useup(mon, obj);
		mtmp = makemon(&mons[PM_DAO], cc.x, cc.y, NO_MM_FLAGS);
		if (!mtmp) {
		    if (vis) pline("%s", empty);
		} else {
		    if (vis)
			pline("In a cloud of smoke, %s emerges!",
							a_monnam(mtmp));
		    pline("%s speaks.", vis ? Monnam(mtmp) : Something);
		/* I suspect few players will be upset that monsters */
		/* can't wish for wands of death here.... */
		    if (rn2(3)) {
			verbalize("I am in your debt. I will grant one wish!");
			pline("%s wishes for an object.", Monnam(mon) );
			switch (rnd(6)) {
				case 1:
					(void) mongets(mon, rnd_defensive_item(mon));
					break;
				case 2:
					(void) mongets(mon, rnd_offensive_item(mon));
					break;
				case 3:
					(void) mongets(mon, rnd_misc_item(mon));
					break;
				case 4:
					(void) mongets(mon, rnd_defensive_item_new(mon));
					break;
				case 5:
					(void) mongets(mon, rnd_offensive_item_new(mon));
					break;
				case 6:
					(void) mongets(mon, rnd_misc_item_new(mon));
					break;
			}
			if (vis) pline("%s vanishes.", Monnam(mtmp));
			mongone(mtmp);
		    } else if (rn2(2)) {
			verbalize("You freed me!");
			mtmp->mpeaceful = 1;
			set_malign(mtmp);
		    } else {
			verbalize("It is about time.");
			if (vis) pline("%s vanishes.", Monnam(mtmp));
			mongone(mtmp);
		    }
		}
		return 2;
	    }
	    if (itemhasappearance(obj, APP_POTION_FUMING) &&
		    flags.efreeti_count < MAXMONNO &&
		    !rn2(POTION_OCCUPANT_CHANCE(flags.efreeti_count))) {
		if (!enexto(&cc, mon->mx, mon->my, &mons[PM_EFREETI])) return 0;
		mquaffmsg(mon, obj);
		m_useup(mon, obj);
		mtmp = makemon(&mons[PM_EFREETI], cc.x, cc.y, NO_MM_FLAGS);
		if (!mtmp) {
		    if (vis) pline("%s", empty);
		} else {
		    if (vis)
			pline("In a cloud of smoke, %s emerges!",
							a_monnam(mtmp));
		    pline("%s speaks.", vis ? Monnam(mtmp) : Something);
		/* I suspect few players will be upset that monsters */
		/* can't wish for wands of death here.... */
		    if (rn2(3)) {
			verbalize("I am in your debt. I will grant one wish!");
			pline("%s wishes for an object.", Monnam(mon) );
			switch (rnd(6)) {
				case 1:
					(void) mongets(mon, rnd_defensive_item(mon));
					break;
				case 2:
					(void) mongets(mon, rnd_offensive_item(mon));
					break;
				case 3:
					(void) mongets(mon, rnd_misc_item(mon));
					break;
				case 4:
					(void) mongets(mon, rnd_defensive_item_new(mon));
					break;
				case 5:
					(void) mongets(mon, rnd_offensive_item_new(mon));
					break;
				case 6:
					(void) mongets(mon, rnd_misc_item_new(mon));
					break;
			}
			if (vis) pline("%s vanishes.", Monnam(mtmp));
			mongone(mtmp);
		    } else if (rn2(2)) {
			verbalize("You freed me!");
			mtmp->mpeaceful = 1;
			set_malign(mtmp);
		    } else {
			verbalize("It is about time.");
			if (vis) pline("%s vanishes.", Monnam(mtmp));
			mongone(mtmp);
		    }
		}
		return 2;
	    }
	    if (itemhasappearance(obj, APP_POTION_SIZZLING) &&
		    flags.marid_count < MAXMONNO &&
		    !rn2(POTION_OCCUPANT_CHANCE(flags.marid_count))) {
		if (!enexto(&cc, mon->mx, mon->my, &mons[PM_MARID])) return 0;
		mquaffmsg(mon, obj);
		m_useup(mon, obj);
		mtmp = makemon(&mons[PM_MARID], cc.x, cc.y, NO_MM_FLAGS);
		if (!mtmp) {
		    if (vis) pline("%s", empty);
		} else {
		    if (vis)
			pline("In a cloud of smoke, %s emerges!",
							a_monnam(mtmp));
		    pline("%s speaks.", vis ? Monnam(mtmp) : Something);
		/* I suspect few players will be upset that monsters */
		/* can't wish for wands of death here.... */
		    if (rn2(3)) {
			verbalize("I am in your debt. I will grant one wish!");
			pline("%s wishes for an object.", Monnam(mon) );
			switch (rnd(6)) {
				case 1:
					(void) mongets(mon, rnd_defensive_item(mon));
					break;
				case 2:
					(void) mongets(mon, rnd_offensive_item(mon));
					break;
				case 3:
					(void) mongets(mon, rnd_misc_item(mon));
					break;
				case 4:
					(void) mongets(mon, rnd_defensive_item_new(mon));
					break;
				case 5:
					(void) mongets(mon, rnd_offensive_item_new(mon));
					break;
				case 6:
					(void) mongets(mon, rnd_misc_item_new(mon));
					break;
			}
			if (vis) pline("%s vanishes.", Monnam(mtmp));
			mongone(mtmp);
		    } else if (rn2(2)) {
			verbalize("You freed me!");
			mtmp->mpeaceful = 1;
			set_malign(mtmp);
		    } else {
			verbalize("It is about time.");
			if (vis) pline("%s vanishes.", Monnam(mtmp));
			mongone(mtmp);
		    }
		}
		return 2;
	    }
	    if (itemhasappearance(obj, APP_POTION_WHISKY) &&
		    flags.wineghost_count < MAXMONNO &&
		    !rn2(POTION_OCCUPANT_CHANCE(flags.wineghost_count))) {
		if (!enexto(&cc, mon->mx, mon->my, &mons[PM_WINE_GHOST])) return 0;
		mquaffmsg(mon, obj);
		m_useup(mon, obj);
		mtmp = makemon(&mons[PM_WINE_GHOST], cc.x, cc.y, NO_MM_FLAGS);
		if (!mtmp) {
		    if (vis) pline("%s", empty);
		} else {
		    if (vis)
			pline("In a cloud of smoke, %s emerges!",
							a_monnam(mtmp));
		    pline("%s speaks.", vis ? Monnam(mtmp) : Something);
		/* I suspect few players will be upset that monsters */
		/* can't wish for wands of death here.... */
		    if (rn2(3)) {
			verbalize("I am in your debt. I will grant one wish!");
			pline("%s wishes for an object.", Monnam(mon) );
			switch (rnd(6)) {
				case 1:
					(void) mongets(mon, rnd_defensive_item(mon));
					break;
				case 2:
					(void) mongets(mon, rnd_offensive_item(mon));
					break;
				case 3:
					(void) mongets(mon, rnd_misc_item(mon));
					break;
				case 4:
					(void) mongets(mon, rnd_defensive_item_new(mon));
					break;
				case 5:
					(void) mongets(mon, rnd_offensive_item_new(mon));
					break;
				case 6:
					(void) mongets(mon, rnd_misc_item_new(mon));
					break;
			}
			if (vis) pline("%s vanishes.", Monnam(mtmp));
			mongone(mtmp);
		    } else if (rn2(2)) {
			verbalize("You freed me!");
			mtmp->mpeaceful = 1;
			set_malign(mtmp);
		    } else {
			verbalize("It is about time.");
			if (vis) pline("%s vanishes.", Monnam(mtmp));
			mongone(mtmp);
		    }
		}
		return 2;
	    }
	    if (itemhasappearance(obj, APP_POTION_DIMLY_SHINING) && !rn2(10)) {
		if (!enexto(&cc, mon->mx, mon->my, &mons[PM_INFERNAL_POTATO])) return 0;
		mquaffmsg(mon, obj);
		m_useup(mon, obj);

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		mtmp = makemon(mkclass(S_DEMON,0), cc.x, cc.y, MM_ANGRY);

		u.aggravation = 0;

		if (!mtmp) {
		    if (vis) pline("%s", empty);
		} else {
		    if (vis)
			pline("In a cloud of smoke, %s emerges!", a_monnam(mtmp));
		}
		return 2;
	    }
	    if (itemhasappearance(obj, APP_POTION_GASEOUS) && !rn2(10)) {
		if (!enexto(&cc, mon->mx, mon->my, &mons[PM_AIR_ELEMENTAL])) return 0;
		mquaffmsg(mon, obj);
		m_useup(mon, obj);

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		mtmp = makemon(mkclass(S_ELEMENTAL,0), cc.x, cc.y, MM_ANGRY);

		u.aggravation = 0;

		if (!mtmp) {
		    if (vis) pline("%s", empty);
		} else {
		    if (vis)
			pline("In a cloud of smoke, %s emerges!", a_monnam(mtmp));
		}
		return 2;
	    }
	    if (itemhasappearance(obj, APP_POTION_STARLIGHT) && !rn2(10)) {
		if (!enexto(&cc, mon->mx, mon->my, &mons[PM_ANGEL])) return 0;
		mquaffmsg(mon, obj);
		m_useup(mon, obj);

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		mtmp = makemon(mkclass(S_ANGEL,0), cc.x, cc.y, MM_ANGRY);

		u.aggravation = 0;

		if (!mtmp) {
		    if (vis) pline("%s", empty);
		} else {
		    if (vis)
			pline("In a cloud of smoke, %s emerges!", a_monnam(mtmp));
		}
		return 2;
	    }

	    if (itemhasappearance(obj, APP_POTION_ENDBRINGER) && !rn2(64)) {
		ragnarok(TRUE);
		if (evilfriday) evilragnarok(TRUE,level_difficulty());
	    }
	    if (itemhasappearance(obj, APP_POTION_UNLUCKY) && !rn2(10)) {
		badeffect();
	    }
	    if (itemhasappearance(obj, APP_POTION_DEADWEIGHT) && !rn2(10)) {
		pline("Some sinister force causes you to wear an item!");
		bad_equipment(0);
	    }
	    if (itemhasappearance(obj, APP_POTION_PRESENT) && !rn2(10)) {
		pline("Some sinister force causes you to wear an artifact!");
		bad_artifact();
	    }
	    if (itemhasappearance(obj, APP_POTION_MALEEN) && !rn2(10)) {
		pline("Some sinister force causes you to wear a pair of heels!");
		bad_equipment_heel();
	    }
	    if (itemhasappearance(obj, APP_POTION_RESERVATROL)) {
		(void) create_gas_cloud(mon->mx, mon->my, 3+bcsign(obj), 8+4*bcsign(obj));
		You("smell chemicals.");
	    }
	    if (itemhasappearance(obj, APP_POTION_NITROGLYCERIN)) {
		struct obj *dynamite;
		dynamite = mksobj_at(STICK_OF_DYNAMITE, mon->mx, mon->my, TRUE, FALSE, FALSE);
		if (dynamite) {
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
	    if (itemhasappearance(obj, APP_POTION_SYMBIO)) {
		struct permonst *pm = 0;
		int attempts = 0;

		if (!enexto(&cc, mon->mx, mon->my, &mons[PM_GOAULD])) return 0;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		register struct monst *symbiomon;

newsymbio:
		do {
			pm = rndmonst();
			attempts++;

		} while ( (!pm || (pm && !(stationary(pm) || pm->mmove == 0 || pm->mlet == S_TURRET ))) && attempts < 50000);

		if (!pm && rn2(50) ) {
			attempts = 0;
			goto newsymbio;
		}
		if (pm && !(stationary(pm) || pm->mmove == 0 || pm->mlet == S_TURRET) && rn2(50) ) {
			attempts = 0;
			goto newsymbio;
		}

		if (pm) symbiomon = makemon(pm, cc.x, cc.y, MM_ANGRY);

		u.aggravation = 0;

		if (symbiomon && vis) {
			pline("A potential symbiote forms out of nowhere!");
		}
	    }
	    if (itemhasappearance(obj, APP_POTION_MICROBIOTIC)) {

		if (!enexto(&cc, mon->mx, mon->my, &mons[PM_LICHEN])) return 0;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		mtmp = makemon(mkclass(S_FUNGUS,0), cc.x, cc.y, MM_ANGRY);

		u.aggravation = 0;

		if (mtmp && vis) pline("A fungus forms out of nowhere!");
	    }
	}
	if (obj->oclass == WAND_CLASS && obj->cursed && !rn2(100)) {
	    int dam = d(obj->spe+2, 6);

	    if (flags.soundok) {
		if (vis) pline("%s zaps %s, which suddenly explodes!",
			Monnam(mon), an(xname(obj)));
		else You_hear("a zap and an explosion in the distance.");
	    }
	    m_useup(mon, obj);
	    if (mon->mhp <= dam) {
		monkilled(mon, "", AD_RBRE);
		return 1;
	    }
	    else mon->mhp -= dam;
	    m.has_defense = m.has_offense = m.has_misc = 0;
	    /* Only one needed to be set to 0 but the others are harmless */
	}
	return 0;
}

STATIC_OVL void
mzapmsg(mtmp, otmp, self)
struct monst *mtmp;
struct obj *otmp;
boolean self;
{
	if (!canseemon(mtmp)) {
		if (flags.soundok)
			You_hear("a %s zap.",
					(distu(mtmp->mx,mtmp->my) <= (BOLT_LIM+1)*(BOLT_LIM+1)) ?
					"nearby" : "distant");
	} else if (self)
		pline("%s zaps %sself with %s!",
		      Monnam(mtmp), mhim(mtmp), doname(otmp));
	else {
		pline("%s zaps %s!", Monnam(mtmp), an(xname(otmp)));
		stop_occupation();
	}

	u.cnd_monwandcount++;
}

STATIC_OVL void
mreadmsg(mtmp, otmp)
struct monst *mtmp;
struct obj *otmp;
{
	boolean vismon = canseemon(mtmp);
	char onambuf[BUFSZ];
	int saverole;
	unsigned savebknown;

	u.cnd_monscrollcount++;

	if (!vismon && !flags.soundok)
	    return;		/* no feedback */

	otmp->dknown = 1;  /* seeing or hearing it read reveals its label */
	/* shouldn't be able to hear curse/bless status of unseen scrolls;
	   for priest characters, bknown will always be set during naming */
	savebknown = otmp->bknown;
	saverole = Role_switch;
	if (!vismon) {
	    otmp->bknown = 0;
	    if (Role_if(PM_PRIEST)) Role_switch = 0;
	}
	strcpy(onambuf, singular(otmp, doname));
	Role_switch = saverole;
	otmp->bknown = savebknown;

	if (vismon)
	    pline("%s reads %s!", Monnam(mtmp), onambuf);
	else
	    You_hear("%s reading %s.",
		x_monnam(mtmp, ARTICLE_A, (char *)0,
		    (SUPPRESS_IT|SUPPRESS_INVISIBLE|SUPPRESS_SADDLE), FALSE),
		onambuf);

	if (mtmp->mconf)
	    pline("Being confused, %s mispronounces the magic words...",
		  vismon ? mon_nam(mtmp) : mhe(mtmp));
}

STATIC_OVL void
mquaffmsg(mtmp, otmp)
struct monst *mtmp;
struct obj *otmp;
{
	if (canseemon(mtmp)) {
		otmp->dknown = 1;
		pline("%s drinks %s!", Monnam(mtmp), singular(otmp, doname));
	} else
		if (flags.soundok) {
			if (FunnyHallu) You_hear(rn2(2) ? "a steam locomotive passing by." : "someone chant 'Chug, chug, chug!'");
			else You_hear("a chugging sound.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Odin men'she zel'ya, kotoryye vy mozhete ispol'zovat', i yest' bol'shaya veroyatnost' togo, chto eto bylo chto-to ochen' polezno. Potomu chto vy byli slishkom medlennymi, kha-kha!" : "Gluckgluckgluckgluck!");
		}

	u.cnd_monpotioncount++;
}

/* Defines for various types of stuff.  The order in which monsters prefer
 * to use them is determined by the order of the code logic, not the
 * numerical order in which they are defined.
 */
#define MUSE_SCR_TELEPORTATION 1
#define MUSE_WAN_TELEPORTATION_SELF 2
#define MUSE_POT_HEALING 3
#define MUSE_POT_EXTRA_HEALING 4
#define MUSE_WAN_DIGGING 5
#define MUSE_TRAPDOOR 6
#define MUSE_TELEPORT_TRAP 7
#define MUSE_UPSTAIRS 8
#define MUSE_DOWNSTAIRS 9
#define MUSE_WAN_CREATE_MONSTER 10
#define MUSE_SCR_CREATE_MONSTER 11
#define MUSE_UP_LADDER 12
#define MUSE_DN_LADDER 13
#define MUSE_SSTAIRS 14
#define MUSE_WAN_TELEPORTATION 15
#define MUSE_BUGLE 16
#define MUSE_UNICORN_HORN 17
#define MUSE_POT_FULL_HEALING 18
#define MUSE_LIZARD_CORPSE 19
/* [Tom] my new items... */
#define MUSE_WAN_HEALING 20
#define MUSE_WAN_EXTRA_HEALING 21
#define MUSE_WAN_CREATE_HORDE 22
#define MUSE_POT_VAMPIRE_BLOOD 23
#define MUSE_WAN_FULL_HEALING 24
#define MUSE_SCR_TELE_LEVEL 25
#define MUSE_SCR_ROOT_PASSWORD_DETECTION 26
#define MUSE_RIN_TIMELY_BACKUP 27
#define MUSE_SCR_SUMMON_UNDEAD 28
#define MUSE_WAN_SUMMON_UNDEAD 29
#define MUSE_SCR_HEALING 30
#define MUSE_SCR_WARPING 31
#define MUSE_BAG_OF_TRICKS 32
#define MUSE_WAN_TELE_LEVEL 33
#define MUSE_SCR_SUMMON_BOSS 34
#define MUSE_POT_CURE_WOUNDS 35
#define MUSE_POT_CURE_SERIOUS_WOUNDS 36
#define MUSE_POT_CURE_CRITICAL_WOUNDS 37
#define MUSE_SCR_POWER_HEALING 38
#define MUSE_SCR_CREATE_VICTIM 39
#define MUSE_SCR_GROUP_SUMMONING 40
#define MUSE_SCR_SUMMON_GHOST 41
#define MUSE_SCR_SUMMON_ELM 42
#define MUSE_WAN_SUMMON_ELM 43
#define MUSE_SCR_RELOCATION 44
#define MUSE_SCR_EXTRA_HEALING 45
#define MUSE_POT_BLOOD 46
#define MUSE_SCR_BRANCH_TELEPORT 47
#define MUSE_SCR_COURSE_TRAVELING 48
#define MUSE_POT_OIL 49
#define MUSE_WAN_SUPER_HEALING 50
#define MUSE_WAN_STRONG_HEALING 51
#define MUSE_WAN_MEDIUM_HEALING 52
#define MUSE_WAN_TELEPORT_SELF 53
/*
#define MUSE_INNATE_TPT 9999
 * We cannot use this.  Since monsters get unlimited teleportation, if they
 * were allowed to teleport at will you could never catch them.  Instead,
 * assume they only teleport at random times, despite the inconsistency that if
 * you polymorph into one you teleport at will.
 */

/* Select a defensive item/action for a monster.  Returns TRUE iff one is
 * found.
 */
boolean
find_defensive(mtmp)
struct monst *mtmp;
{
	register struct obj *obj = 0;
	struct trap *t;
	int x=mtmp->mx, y=mtmp->my;
	boolean stuck = (mtmp == u.ustuck);
	boolean immobile = (mtmp->data->mmove == 0);
	int fraction;

	if (!issoviet && !rn2(5)) return FALSE;

	/* "Revert some Fake Difficulty. Mindless monsters and animals should not be able to wear armor, nor use potions/scrolls/wands." Dunno why that's supposed to be fake difficulty, but in Soviet Russia, players apparently can't handle dogs zapping wands of lightning or something. Everyone else will have to contend with monsters using items the way they're supposed to. --Amy */

	if ((is_animal(mtmp->data) || mindless(mtmp->data)) && issoviet)
		return FALSE;
	if(dist2(x, y, mtmp->mux, mtmp->muy) > 25 && rn2(dist2(x, y, mtmp->mux, mtmp->muy) - 25) )
		return FALSE;
	if (u.uswallow && stuck) return FALSE;

	m.defensive = (struct obj *)0;
	m.has_defense = 0;

	/* since unicorn horns don't get used up, the monster would look
	 * silly trying to use the same cursed horn round after round
	 */
	if (mtmp->mconf || mtmp->mstun || !mtmp->mcansee) {
	    if (!is_unicorn(mtmp->data) && !nohands(mtmp->data)) {
		for(obj = mtmp->minvent; obj; obj = obj->nobj)
		    if (obj->otyp == UNICORN_HORN && !obj->cursed)
			break;
	    }
	    if (obj || is_unicorn(mtmp->data)) {
		m.defensive = obj;
		m.has_defense = MUSE_UNICORN_HORN;
		return TRUE;
	    }
	}

	if (mtmp->mconf) {
	    for(obj = mtmp->minvent; obj; obj = obj->nobj) {
		if (obj->otyp == CORPSE && obj->corpsenm == PM_LIZARD) {
		    m.defensive = obj;
		    m.has_defense = MUSE_LIZARD_CORPSE;
		    return TRUE;
		}
	    }
	}

	/* It so happens there are two unrelated cases when we might want to
	 * check specifically for healing alone.  The first is when the monster
	 * is blind (healing cures blindness).  The second is when the monster
	 * is peaceful; then we don't want to flee the player, and by
	 * coincidence healing is all there is that doesn't involve fleeing.
	 * These would be hard to combine because of the control flow.
	 * Pestilence won't use healing even when blind.
	 */
	if (!mtmp->mcansee /*&& !nohands(mtmp->data)*/ &&
		mtmp->data != &mons[PM_PESTILENCE]) {
	    if ((obj = m_carrying(mtmp, POT_FULL_HEALING)) != 0) {
		m.defensive = obj;
		m.has_defense = MUSE_POT_FULL_HEALING;
		return TRUE;
	    }
	    if ((obj = m_carrying(mtmp, POT_EXTRA_HEALING)) != 0) {
		m.defensive = obj;
		m.has_defense = MUSE_POT_EXTRA_HEALING;
		return TRUE;
	    }
	    if ((obj = m_carrying(mtmp, POT_HEALING)) != 0) {
		m.defensive = obj;
		m.has_defense = MUSE_POT_HEALING;
		return TRUE;
	    }
	    if ((obj = m_carrying(mtmp, POT_CURE_CRITICAL_WOUNDS)) != 0) {
		m.defensive = obj;
		m.has_defense = MUSE_POT_CURE_CRITICAL_WOUNDS;
		return TRUE;
	    }
	    if ((obj = m_carrying(mtmp, POT_CURE_SERIOUS_WOUNDS)) != 0) {
		m.defensive = obj;
		m.has_defense = MUSE_POT_CURE_SERIOUS_WOUNDS;
		return TRUE;
	    }
	    if ((obj = m_carrying(mtmp, POT_OIL)) != 0 && mtmp->data->msound == MS_CAR) {
		m.defensive = obj;
		m.has_defense = MUSE_POT_OIL;
		return TRUE;
	    }
	    if ((obj = m_carrying(mtmp, POT_CURE_WOUNDS)) != 0) {
		m.defensive = obj;
		m.has_defense = MUSE_POT_CURE_WOUNDS;
		return TRUE;
	    }
	    if ((obj = m_carrying(mtmp, SCR_HEALING)) != 0) {
		m.defensive = obj;
		m.has_defense = MUSE_SCR_HEALING;
		return TRUE;
	    }
	    if ((obj = m_carrying(mtmp, SCR_EXTRA_HEALING)) != 0) {
		m.defensive = obj;
		m.has_defense = MUSE_SCR_EXTRA_HEALING;
		return TRUE;
	    }
	    if ((obj = m_carrying(mtmp, SCR_POWER_HEALING)) != 0) {
		m.defensive = obj;
		m.has_defense = MUSE_SCR_POWER_HEALING;
		return TRUE;
	    }
	    if ((obj = m_carrying(mtmp, POT_FULL_HEALING)) !=0) {
		m.defensive = obj;
		m.has_defense = MUSE_POT_FULL_HEALING;
		return TRUE;
	    }
	}

	fraction = u.ulevel < 10 ? 5 : u.ulevel < 14 ? 4 : 3;
	if (rn2(3)) fraction -= 1;
	if(mtmp->mhp >= mtmp->mhpmax ||
			(mtmp->mhp >= 10 && mtmp->mhp*fraction >= mtmp->mhpmax))
		return FALSE;

	if (mtmp->mpeaceful) {
	    /*if (!nohands(mtmp->data)) {*/
		if ((obj = m_carrying(mtmp, POT_FULL_HEALING)) != 0) {
		    m.defensive = obj;
		    m.has_defense = MUSE_POT_FULL_HEALING;
		    return TRUE;
		}
		if ((obj = m_carrying(mtmp, POT_EXTRA_HEALING)) != 0) {
		    m.defensive = obj;
		    m.has_defense = MUSE_POT_EXTRA_HEALING;
		    return TRUE;
		}
		if ((obj = m_carrying(mtmp, POT_HEALING)) != 0) {
		    m.defensive = obj;
		    m.has_defense = MUSE_POT_HEALING;
		    return TRUE;
		}
	      if ((obj = m_carrying(mtmp, POT_CURE_CRITICAL_WOUNDS)) != 0) {
		    m.defensive = obj;
		    m.has_defense = MUSE_POT_CURE_CRITICAL_WOUNDS;
		    return TRUE;
	      }
	      if ((obj = m_carrying(mtmp, POT_CURE_SERIOUS_WOUNDS)) != 0) {
		    m.defensive = obj;
		    m.has_defense = MUSE_POT_CURE_SERIOUS_WOUNDS;
		    return TRUE;
	      }
	      if ((obj = m_carrying(mtmp, POT_CURE_WOUNDS)) != 0) {
		    m.defensive = obj;
		    m.has_defense = MUSE_POT_CURE_WOUNDS;
		    return TRUE;
	      }
		if ((obj = m_carrying(mtmp, SCR_HEALING)) != 0) {
		    m.defensive = obj;
		    m.has_defense = MUSE_SCR_HEALING;
		    return TRUE;
		}
		if ((obj = m_carrying(mtmp, SCR_EXTRA_HEALING)) != 0) {
		    m.defensive = obj;
		    m.has_defense = MUSE_SCR_EXTRA_HEALING;
		    return TRUE;
		}
		if ((obj = m_carrying(mtmp, SCR_POWER_HEALING)) != 0) {
		    m.defensive = obj;
		    m.has_defense = MUSE_SCR_POWER_HEALING;
		    return TRUE;
		}
		if (is_vampire(mtmp->data) &&
		  (obj = m_carrying(mtmp, POT_VAMPIRE_BLOOD)) !=0) {
		    m.defensive = obj;
		    m.has_defense = MUSE_POT_VAMPIRE_BLOOD;
		    return TRUE;
		}
		if (is_vampire(mtmp->data) &&
		  (obj = m_carrying(mtmp, POT_BLOOD)) !=0) {
		    m.defensive = obj;
		    m.has_defense = MUSE_POT_BLOOD;
		    return TRUE;
		}
	    /*}*/
	    return FALSE;
	}

	if (levl[x][y].typ == STAIRS && !(u.usteed && (mtmp == u.usteed)) && !stuck && !immobile) {
		if (x == xdnstair && y == ydnstair && !is_floater(mtmp->data))
			m.has_defense = MUSE_DOWNSTAIRS;
		if (x == xupstair && y == yupstair && ledger_no(&u.uz) != 1)
	/* Unfair to let the monsters leave the dungeon with the Amulet */
	/* (or go to the endlevel since you also need it, to get there) */
			m.has_defense = MUSE_UPSTAIRS;
	} else if (levl[x][y].typ == LADDER && !(u.usteed && (mtmp == u.usteed)) && !stuck && !immobile) {
		if (x == xupladder && y == yupladder)
			m.has_defense = MUSE_UP_LADDER;
		if (x == xdnladder && y == ydnladder && !is_floater(mtmp->data))
			m.has_defense = MUSE_DN_LADDER;
	} else if (sstairs.sx && !(u.usteed && (mtmp == u.usteed)) && sstairs.sx == x && sstairs.sy == y) {
		m.has_defense = MUSE_SSTAIRS;
	} else if (!stuck && !immobile) {
	/* Note: trap doors take precedence over teleport traps. */
		int xx, yy;

		for(xx = x-1; xx <= x+1; xx++) for(yy = y-1; yy <= y+1; yy++)
		if (isok(xx,yy))
		if (xx != u.ux || yy != u.uy)
		if (!(isgridbug(mtmp->data)) || xx == x || yy == y)
		if ((xx==x && yy==y) || !level.monsters[xx][yy])
		if ((t = t_at(xx,yy)) != 0)
		if ((verysmall(mtmp->data) || throws_rocks(mtmp->data) ||
		     passes_walls(mtmp->data) || (mtmp->egotype_wallwalk) ) || !sobj_at(BOULDER, xx, yy))
		if (!onscary(xx,yy,mtmp)) {
			if ((t->ttyp == TRAPDOOR || t->ttyp == HOLE || t->ttyp == SHAFT_TRAP || t->ttyp == CURRENT_SHAFT)
				&& !is_floater(mtmp->data)
				&& !mtmp->isshk && !mtmp->isgd
				&& !mtmp->ispriest
				&& !(u.usteed && (mtmp == u.usteed))
				&& Can_fall_thru(&u.uz)
						) {
				trapx = xx;
				trapy = yy;
				m.has_defense = MUSE_TRAPDOOR;
			} else if (t->ttyp == TELEP_TRAP && m.has_defense != MUSE_TRAPDOOR) {
				trapx = xx;
				trapy = yy;
				m.has_defense = MUSE_TELEPORT_TRAP;
			} else if (t->ttyp == BEAMER_TRAP && m.has_defense != MUSE_TRAPDOOR) {
				trapx = xx;
				trapy = yy;
				m.has_defense = MUSE_TELEPORT_TRAP;
			}
		}
	}

	/*if (nohands(mtmp->data))*/	/* can't use objects */
		/*goto botm;*/

	if (is_mercenary(mtmp->data) && (obj = m_carrying(mtmp, BUGLE))) {
		int xx, yy;
		struct monst *mon;

		/* Distance is arbitrary.  What we really want to do is
		 * have the soldier play the bugle when it sees or
		 * remembers soldiers nearby...
		 */
		for(xx = x-3; xx <= x+3; xx++) for(yy = y-3; yy <= y+3; yy++)
		if (isok(xx,yy))
		if ((mon = m_at(xx,yy)) && is_mercenary(mon->data) &&
				mon->data != &mons[PM_GUARD] &&
				mon->data != &mons[PM_MASTER_GUARD] &&
				mon->data != &mons[PM_EXPERIENCED_GUARD] &&
				mon->data != &mons[PM_EXCEPTIONAL_GUARD] &&
				mon->data != &mons[PM_ELITE_GUARD] &&
				mon->data != &mons[PM_CROUPIER] &&
				mon->data != &mons[PM_MASTER_CROUPIER] &&
				mon->data != &mons[PM_EXPERIENCED_CROUPIER] &&
				mon->data != &mons[PM_EXCEPTIONAL_CROUPIER] &&
				mon->data != &mons[PM_ELITE_CROUPIER] &&
				(mon->msleeping || (!mon->mcanmove))) {
			m.defensive = obj;
			m.has_defense = MUSE_BUGLE;
		}
	}

	/* use immediate physical escape prior to attempting magic */
	if (m.has_defense)    /* stairs, trap door or tele-trap, bugle alert */
		goto botm;

	/* kludge to cut down on trap destruction (particularly portals) */
	t = t_at(x,y);
	if (t && (t->ttyp == PIT || t->ttyp == SPIKED_PIT || t->ttyp == GIANT_CHASM || t->ttyp == SHIT_PIT || t->ttyp == MANA_PIT || t->ttyp == ANOXIC_PIT || t->ttyp == ACID_PIT ||
		  t->ttyp == WEB || t->ttyp == BEAR_TRAP))
		t = 0;		/* ok for monster to dig here */

#define nomore(x) if(m.has_defense==x) continue;
	for (obj = mtmp->minvent; obj; obj = obj->nobj) {
		/* don't always use the same selection pattern */
		if (m.has_defense && !rn2(3)) break;

		/* nomore(MUSE_WAN_DIGGING); */
		if (m.has_defense == MUSE_WAN_DIGGING) break;
		if (obj->otyp == WAN_DIGGING && obj->spe > 0 && !stuck && !t
		    && !mtmp->isshk && !mtmp->isgd && !mtmp->ispriest
		    && !is_floater(mtmp->data)
		    /* monsters digging in Sokoban can ruin things */
		    && !In_sokoban(&u.uz)
		    && !(u.usteed && (mtmp == u.usteed))
		    /* digging wouldn't be effective; assume they know that */
		    && !(levl[x][y].wall_info & W_NONDIGGABLE)
		    && !(Is_botlevel(&u.uz) || In_endgame(&u.uz))
		    && !(is_ice(x,y) || is_pool(x,y) || is_lava(x,y))
		    && !(mtmp->data == &mons[PM_VLAD_THE_IMPALER]
			 && In_V_tower(&u.uz))) {
			m.defensive = obj;
			m.has_defense = MUSE_WAN_DIGGING;
		}
		nomore(MUSE_WAN_TELEPORTATION_SELF);
		nomore(MUSE_WAN_TELEPORTATION);
		if(obj->otyp == WAN_TELEPORTATION && obj->spe > 0) {
		    /* use the TELEP_TRAP bit to determine if they know
		     * about noteleport on this level or not.  Avoids
		     * ineffective re-use of teleportation.  This does
		     * mean if the monster leaves the level, they'll know
		     * about teleport traps.
		     */
		    if (!level.flags.noteleport && !Race_if(PM_STABILISATOR) && !u.antitelespelltimeout) {
			m.defensive = obj;
			m.has_defense = (mon_has_amulet(mtmp))
				? MUSE_WAN_TELEPORTATION
				: MUSE_WAN_TELEPORTATION_SELF;
		    }
		}
		nomore(MUSE_WAN_TELEPORT_SELF);
		if(obj->otyp == WAN_TELEPORT_SELF && obj->spe > 0 && !mtmp->isgd && !(mtmp->isshk && inhishop(mtmp)) && !mtmp->ispriest && !mon_has_amulet(mtmp)) {
		    if (!level.flags.noteleport && !Race_if(PM_STABILISATOR) && !u.antitelespelltimeout) {
			m.defensive = obj;
			m.has_defense = MUSE_WAN_TELEPORT_SELF;
		    }
		}
		nomore(MUSE_SCR_TELEPORTATION);
		if(obj->otyp == SCR_TELEPORTATION && mtmp->mcansee
		   && haseyes(mtmp->data)
		   && (!obj->cursed ||
		       (!(mtmp->isshk && inhishop(mtmp))
			    && !(u.usteed && (mtmp == u.usteed))
			    && !mtmp->isgd && !mtmp->ispriest))) {
		    /* see WAN_TELEPORTATION case above */
		    if (!level.flags.noteleport && !Race_if(PM_STABILISATOR) && !u.antitelespelltimeout) {
			m.defensive = obj;
			m.has_defense = MUSE_SCR_TELEPORTATION;
		    }
		}

		nomore(MUSE_SCR_ROOT_PASSWORD_DETECTION);
		if(obj->otyp == SCR_ROOT_PASSWORD_DETECTION
		   && (!(mtmp->isshk && inhishop(mtmp))
			    && !(u.usteed && (mtmp == u.usteed))
			    && !mtmp->isgd && !mtmp->ispriest)) {
			m.defensive = obj;
			m.has_defense = MUSE_SCR_ROOT_PASSWORD_DETECTION;
		}

		nomore(MUSE_SCR_COURSE_TRAVELING);
		if(obj->otyp == SCR_COURSE_TRAVELING
		   && (!(mtmp->isshk && inhishop(mtmp))
			    && !(u.usteed && (mtmp == u.usteed))
			    && !mtmp->isgd && !mtmp->ispriest)) {
			m.defensive = obj;
			m.has_defense = MUSE_SCR_COURSE_TRAVELING;
		}

		nomore(MUSE_SCR_RELOCATION);
		if(obj->otyp == SCR_RELOCATION
		   && (!(mtmp->isshk && inhishop(mtmp))
			    && !mtmp->isgd && !mtmp->ispriest)) {
			m.defensive = obj;
			m.has_defense = MUSE_SCR_RELOCATION;
		}

		nomore(MUSE_SCR_TELE_LEVEL);
		if(obj->otyp == SCR_TELE_LEVEL
		   && (!(mtmp->isshk && inhishop(mtmp))
			    && !(u.usteed && (mtmp == u.usteed))
			    && !mtmp->isgd && !mtmp->ispriest)) {
			m.defensive = obj;
			m.has_defense = MUSE_SCR_TELE_LEVEL;
		}

		nomore(MUSE_SCR_BRANCH_TELEPORT);
		if(obj->otyp == SCR_BRANCH_TELEPORT && evilfriday
		   && (!(mtmp->isshk && inhishop(mtmp))
			    && !(u.usteed && (mtmp == u.usteed))
			    && !mtmp->isgd && !mtmp->ispriest)) {
			m.defensive = obj;
			m.has_defense = MUSE_SCR_BRANCH_TELEPORT;
		}

		nomore(MUSE_WAN_TELE_LEVEL);
		if(obj->otyp == WAN_TELE_LEVEL && obj->spe > 0
		   && (!(mtmp->isshk && inhishop(mtmp))
			    && !(u.usteed && (mtmp == u.usteed))
			    && !mtmp->isgd && !mtmp->ispriest)) {
			m.defensive = obj;
			m.has_defense = MUSE_WAN_TELE_LEVEL;
		}

		nomore(MUSE_SCR_WARPING);
		if(obj->otyp == SCR_WARPING
		   && (!(mtmp->isshk && inhishop(mtmp))
			    && !(u.usteed && (mtmp == u.usteed))
			    && !mtmp->isgd && !mtmp->ispriest)) {
			m.defensive = obj;
			m.has_defense = MUSE_SCR_WARPING;
		}

		nomore(MUSE_RIN_TIMELY_BACKUP);
		if(obj->otyp == RIN_TIMELY_BACKUP) {
			m.defensive = obj;
			m.has_defense = MUSE_RIN_TIMELY_BACKUP;
		}

		nomore(MUSE_SCR_SUMMON_UNDEAD);
		if(obj->otyp == SCR_SUMMON_UNDEAD) {
			m.defensive = obj;
			m.has_defense = MUSE_SCR_SUMMON_UNDEAD;
		}

		nomore(MUSE_SCR_GROUP_SUMMONING);
		if(obj->otyp == SCR_GROUP_SUMMONING) {
			m.defensive = obj;
			m.has_defense = MUSE_SCR_GROUP_SUMMONING;
		}

	    if (mtmp->data != &mons[PM_PESTILENCE]) {
		nomore(MUSE_POT_FULL_HEALING);
		if(obj->otyp == POT_FULL_HEALING) {
			m.defensive = obj;
			m.has_defense = MUSE_POT_FULL_HEALING;
		}
		nomore(MUSE_POT_CURE_CRITICAL_WOUNDS);
		if(obj->otyp == POT_CURE_CRITICAL_WOUNDS) {
			m.defensive = obj;
			m.has_defense = MUSE_POT_CURE_CRITICAL_WOUNDS;
		}
		nomore(MUSE_POT_EXTRA_HEALING);
		if(obj->otyp == POT_EXTRA_HEALING) {
			m.defensive = obj;
			m.has_defense = MUSE_POT_EXTRA_HEALING;
		}
		nomore(MUSE_POT_CURE_SERIOUS_WOUNDS);
		if(obj->otyp == POT_CURE_SERIOUS_WOUNDS) {
			m.defensive = obj;
			m.has_defense = MUSE_POT_CURE_SERIOUS_WOUNDS;
		}
		nomore(MUSE_WAN_CREATE_MONSTER);
		if(obj->otyp == WAN_CREATE_MONSTER && obj->spe > 0) {
			m.defensive = obj;
			m.has_defense = MUSE_WAN_CREATE_MONSTER;
		}
		nomore(MUSE_WAN_SUMMON_UNDEAD);
		if(obj->otyp == WAN_SUMMON_UNDEAD && obj->spe > 0) {
			m.defensive = obj;
			m.has_defense = MUSE_WAN_SUMMON_UNDEAD;
		}
		nomore(MUSE_WAN_SUMMON_ELM);
		if(obj->otyp == WAN_SUMMON_ELM && obj->spe > 0) {
			m.defensive = obj;
			m.has_defense = MUSE_WAN_SUMMON_ELM;
		}
		nomore(MUSE_WAN_CREATE_HORDE);
		if(obj->otyp == WAN_CREATE_HORDE && obj->spe > 0) {
			m.defensive = obj;
			m.has_defense = MUSE_WAN_CREATE_HORDE;
		}
		nomore(MUSE_POT_HEALING);
		if(obj->otyp == POT_HEALING) {
			m.defensive = obj;
			m.has_defense = MUSE_POT_HEALING;
		}
		nomore(MUSE_POT_CURE_WOUNDS);
		if(obj->otyp == POT_CURE_WOUNDS) {
			m.defensive = obj;
			m.has_defense = MUSE_POT_CURE_WOUNDS;
		}
		nomore(MUSE_POT_OIL);
		if(obj->otyp == POT_OIL && mtmp->data->msound == MS_CAR) {
			m.defensive = obj;
			m.has_defense = MUSE_POT_OIL;
		}
		nomore(MUSE_SCR_POWER_HEALING);
		if(obj->otyp == SCR_POWER_HEALING) {
			m.defensive = obj;
			m.has_defense = MUSE_SCR_POWER_HEALING;
		}
		nomore(MUSE_SCR_HEALING);
		if(obj->otyp == SCR_HEALING) {
			m.defensive = obj;
			m.has_defense = MUSE_SCR_HEALING;
		}
		nomore(MUSE_SCR_EXTRA_HEALING);
		if(obj->otyp == SCR_EXTRA_HEALING) {
			m.defensive = obj;
			m.has_defense = MUSE_SCR_EXTRA_HEALING;
		}
		nomore(MUSE_WAN_HEALING);
		if(obj->otyp == WAN_HEALING && obj->spe > 0) {
			m.defensive = obj;
			m.has_defense = MUSE_WAN_HEALING;
		}
		nomore(MUSE_WAN_EXTRA_HEALING);
		if(obj->otyp == WAN_EXTRA_HEALING && obj->spe > 0) {
			m.defensive = obj;
			m.has_defense = MUSE_WAN_EXTRA_HEALING;
		}
		nomore(MUSE_WAN_FULL_HEALING);
		if(obj->otyp == WAN_FULL_HEALING && obj->spe > 0) {
			m.defensive = obj;
			m.has_defense = MUSE_WAN_FULL_HEALING;
		}
		nomore(MUSE_WAN_MEDIUM_HEALING);
		if(obj->otyp == WAN_MEDIUM_HEALING && obj->spe > 0) {
			m.defensive = obj;
			m.has_defense = MUSE_WAN_MEDIUM_HEALING;
		}
		nomore(MUSE_WAN_STRONG_HEALING);
		if(obj->otyp == WAN_STRONG_HEALING && obj->spe > 0) {
			m.defensive = obj;
			m.has_defense = MUSE_WAN_STRONG_HEALING;
		}
		nomore(MUSE_WAN_SUPER_HEALING);
		if(obj->otyp == WAN_SUPER_HEALING && obj->spe > 0) {
			m.defensive = obj;
			m.has_defense = MUSE_WAN_SUPER_HEALING;
		}
		nomore(MUSE_POT_VAMPIRE_BLOOD);
		if(is_vampire(mtmp->data) && obj->otyp == POT_VAMPIRE_BLOOD) {
			m.defensive = obj;
			m.has_defense = MUSE_POT_VAMPIRE_BLOOD;
		}
		nomore(MUSE_POT_BLOOD);
		if(is_vampire(mtmp->data) && obj->otyp == POT_BLOOD) {
			m.defensive = obj;
			m.has_defense = MUSE_POT_BLOOD;
		}
	    } else {	/* Pestilence */
		nomore(MUSE_POT_FULL_HEALING);
		if (obj->otyp == POT_SICKNESS) {
			m.defensive = obj;
			m.has_defense = MUSE_POT_FULL_HEALING;
		}
		nomore(MUSE_WAN_CREATE_MONSTER);
		if (obj->otyp == WAN_CREATE_MONSTER && obj->spe > 0) {
			m.defensive = obj;
			m.has_defense = MUSE_WAN_CREATE_MONSTER;
		}
		nomore(MUSE_WAN_SUMMON_UNDEAD);
		if (obj->otyp == WAN_SUMMON_UNDEAD && obj->spe > 0) {
			m.defensive = obj;
			m.has_defense = MUSE_WAN_SUMMON_UNDEAD;
		}
		nomore(MUSE_WAN_SUMMON_ELM);
		if (obj->otyp == WAN_SUMMON_ELM && obj->spe > 0) {
			m.defensive = obj;
			m.has_defense = MUSE_WAN_SUMMON_ELM;
		}
	    }
		nomore(MUSE_SCR_SUMMON_BOSS);
		if(obj->otyp == SCR_SUMMON_BOSS) {
			m.defensive = obj;
			m.has_defense = MUSE_SCR_SUMMON_BOSS;
		}
		nomore(MUSE_SCR_SUMMON_GHOST);
		if(obj->otyp == SCR_SUMMON_GHOST) {
			m.defensive = obj;
			m.has_defense = MUSE_SCR_SUMMON_GHOST;
		}
		nomore(MUSE_SCR_SUMMON_ELM);
		if(obj->otyp == SCR_SUMMON_ELM) {
			m.defensive = obj;
			m.has_defense = MUSE_SCR_SUMMON_ELM;
		}
		nomore(MUSE_SCR_CREATE_MONSTER);
		if(obj->otyp == SCR_CREATE_MONSTER) {
			m.defensive = obj;
			m.has_defense = MUSE_SCR_CREATE_MONSTER;
		}
		nomore(MUSE_SCR_CREATE_VICTIM);
		if(obj->otyp == SCR_CREATE_VICTIM) {
			m.defensive = obj;
			m.has_defense = MUSE_SCR_CREATE_VICTIM;
		}
		nomore(MUSE_BAG_OF_TRICKS);
		if(obj->otyp == BAG_OF_TRICKS  && obj->spe > 0) {
			m.defensive = obj;
			m.has_defense = MUSE_BAG_OF_TRICKS;
		}
	}
botm:	return((boolean)(!!m.has_defense));
#undef nomore
}

/* Perform a defensive action for a monster.  Must be called immediately
 * after find_defensive().  Return values are 0: did something, 1: died,
 * 2: did something and can't attack again (i.e. teleported).
 */
int
use_defensive(mtmp)
struct monst *mtmp;
{
	int rinspe;
	int i, fleetim, how = 0;
	struct obj *otmp = m.defensive;
	boolean vis, vismon, oseen;
	const char *mcsa = "%s can see again.";

	if ((i = precheck(mtmp, otmp)) != 0) return i;
	vis = cansee(mtmp->mx, mtmp->my);
	vismon = canseemon(mtmp);
	oseen = otmp && vismon;

	/* when using defensive choice to run away, we want monster to avoid
	   rushing right straight back; don't override if already scared */
	fleetim = !mtmp->mflee ? (33 - (30 * mtmp->mhp / mtmp->mhpmax)) : 0;
#define m_flee(m)	if (fleetim && !m->iswiz) \
			{ monflee(m, fleetim, FALSE, FALSE); }

	switch(m.has_defense) {
	case MUSE_UNICORN_HORN:
		if (vismon) {
		    if (otmp)
			pline("%s uses a unicorn horn!", Monnam(mtmp));
		    else
			pline_The("tip of %s's horn glows!", mon_nam(mtmp));
		}
		if (!mtmp->mcansee) {
		    mtmp->mcansee = 1;
		    mtmp->mblinded = 0;
		    if (vismon) pline(mcsa, Monnam(mtmp));
		} else if (mtmp->mconf || mtmp->mstun) {
		    mtmp->mconf = mtmp->mstun = 0;
		    if (vismon)
			pline("%s seems steadier now.", Monnam(mtmp));
		} else impossible("No need for unicorn horn?");
		return 2;
	case MUSE_BUGLE:
		if (vismon) {
			pline("%s plays %s!", Monnam(mtmp), doname(otmp));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Nakonets soldaty uzhe ne spit. Sovetskaya vser'yez dumal, chto eto byla khoroshaya ideya, chtoby ne razbudit' ikh, yesli vy vvodite svoi baraki, NO seychas vy nakhodites' v nekotoroy ser'yeznoy boli, vy ublyudok." : "TAET-TAETAEAEAE!");
		}
		else if (flags.soundok) {
			You_hear("a bugle playing reveille!");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Nakonets soldaty uzhe ne spit. Sovetskaya vser'yez dumal, chto eto byla khoroshaya ideya, chtoby ne razbudit' ikh, yesli vy vvodite svoi baraki, NO seychas vy nakhodites' v nekotoroy ser'yeznoy boli, vy ublyudok." : "TAET-TAETAEAEAE!");
		}
		if (otmp && otmp->oartifact == ART_HEAR_FAR_AND_WIDE) awaken_soldiers(1);
		else if (otmp && otmp->oartifact == ART_THIS_IS_NOT_A_DRILL) awaken_soldiers(2);
		else awaken_soldiers(0);
		return 2;
	case MUSE_WAN_TELEPORTATION_SELF:
		if ((mtmp->isshk && inhishop(mtmp))
		       || mtmp->isgd || mtmp->ispriest) return 2;
		m_flee(mtmp);
		mzapmsg(mtmp, otmp, TRUE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		how = WAN_TELEPORTATION;
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
mon_tele:
		if (tele_restrict(mtmp)) {	/* mysterious force... */
		    if (vismon && how)		/* mentions 'teleport' */
			makeknown(how);
		    /* monster learns that teleportation isn't useful here */
		    /*if (level.flags.noteleport)
			mtmp->mtrapseen |= (1 << (TELEP_TRAP-1));*/
		    return 2;
		}
		if ((
#if 0
			mon_has_amulet(mtmp) ||
#endif
			On_W_tower_level(&u.uz)) && !rn2(3)) {
		    if (vismon)
			pline("%s seems disoriented for a moment.",
				Monnam(mtmp));
		    return 2;
		}
		if (oseen && how) makeknown(how);
		(void) rloc(mtmp, FALSE);
		return 2;
	case MUSE_WAN_TELEPORT_SELF:
		if ((mtmp->isshk && inhishop(mtmp))
		       || mtmp->isgd || mtmp->ispriest) return 2;
		m_flee(mtmp);
		mzapmsg(mtmp, otmp, TRUE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		how = WAN_TELEPORT_SELF;
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		if (tele_restrict(mtmp)) {	/* mysterious force... */
		    if (vismon && how)		/* mentions 'teleport' */
			makeknown(how);
		    return 2;
		}
		if ((
#if 0
			mon_has_amulet(mtmp) ||
#endif
			On_W_tower_level(&u.uz)) && !rn2(3)) {
		    if (vismon)
			pline("%s seems disoriented for a moment.",
				Monnam(mtmp));
		    return 2;
		}
		if (oseen && how) makeknown(how);
		(void) rloc(mtmp, FALSE);
		return 2;
	case MUSE_WAN_TELEPORTATION:
		zap_oseen = oseen;
		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		m_using = TRUE;
		mbhit(mtmp, EnglandMode ? rn1(10,10) : rn1(8,6),mbhitm,bhito,otmp);
		/* monster learns that teleportation isn't useful here */
		/*if (level.flags.noteleport)
		    mtmp->mtrapseen |= (1 << (TELEP_TRAP-1));*/
		m_using = FALSE;
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;
	case MUSE_SCR_TELEPORTATION:
	    {
		int obj_is_cursed = otmp->cursed;

		if (mtmp->isshk || mtmp->isgd || mtmp->ispriest) return 2;
		m_flee(mtmp);
		mreadmsg(mtmp, otmp);

		if (otmp->oartifact == ART_HEALAPORTATION) mtmp->mhp = mtmp->mhpmax;

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */
		how = SCR_TELEPORTATION;
		if (obj_is_cursed || mtmp->mconf) {
			int nlev;
			d_level flev;

			if (mon_has_amulet(mtmp) || In_endgame(&u.uz)) {
			    if (vismon) {
				pline("%s seems very disoriented for a moment.",
					Monnam(mtmp));
				if (oseen) makeknown(SCR_TELEPORTATION);
				}
			    return 2;
			}
			nlev = random_teleport_level();
			if (nlev == depth(&u.uz)) {
			    if (vismon) {
				pline("%s shudders for a moment.",
								Monnam(mtmp));
				if (oseen) makeknown(SCR_TELEPORTATION);
				}
			    return 2;
			}
			get_level(&flev, nlev);
			migrate_to_level(mtmp, ledger_no(&flev), MIGR_RANDOM,
				(coord *)0);
			if (oseen) makeknown(SCR_TELEPORTATION);
		} else goto mon_tele;
		return 2;
	    }
	case MUSE_SCR_RELOCATION:
	    {

		if (mtmp->isshk || mtmp->isgd || mtmp->ispriest) return 2;
		m_flee(mtmp);
		mreadmsg(mtmp, otmp);
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */
		rloc(mtmp, FALSE);
		if (oseen) makeknown(SCR_RELOCATION);

		return 2;
	    }
	case MUSE_SCR_TELE_LEVEL:
	    {
		if (mtmp->isshk || mtmp->isgd || mtmp->ispriest) return 2;
		m_flee(mtmp);
		mreadmsg(mtmp, otmp);
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */
		how = SCR_TELE_LEVEL;

			int nlev;
			d_level flev;

			if (mon_has_amulet(mtmp) || In_endgame(&u.uz)) {
			    if (vismon) {
				pline("%s seems very disoriented for a moment.",
					Monnam(mtmp));
				if (oseen) makeknown(SCR_TELE_LEVEL);
				}
			    return 2;
			}
			nlev = random_teleport_level();
			if (nlev == depth(&u.uz)) {
			    if (vismon) {
				pline("%s shudders for a moment.",
								Monnam(mtmp));
				if (oseen) makeknown(SCR_TELE_LEVEL);
				}
			    return 2;
			}
			get_level(&flev, nlev);
			migrate_to_level(mtmp, ledger_no(&flev), MIGR_RANDOM,
				(coord *)0);
			if (oseen) makeknown(SCR_TELE_LEVEL);

		return 2;
	    }

	case MUSE_SCR_BRANCH_TELEPORT:
	    {
		if (mtmp->isshk || mtmp->isgd || mtmp->ispriest) return 2;
		m_flee(mtmp);
		mreadmsg(mtmp, otmp);
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */
		how = SCR_BRANCH_TELEPORT;

			d_level flev;

			if (mon_has_amulet(mtmp) || In_endgame(&u.uz)) {
			    if (vismon) {
				pline("%s seems very disoriented for a moment.",
					Monnam(mtmp));
				if (oseen) makeknown(SCR_BRANCH_TELEPORT);
				}
			    return 2;
			}
			flev = random_branchport_level();
			migrate_to_level(mtmp, ledger_no(&flev), MIGR_RANDOM, (coord *)0);
			if (oseen) makeknown(SCR_BRANCH_TELEPORT);

		return 2;
	    }

	case MUSE_SCR_COURSE_TRAVELING:
	    {
		if (mtmp->isshk || mtmp->isgd || mtmp->ispriest) return 2;
		m_flee(mtmp);
		mreadmsg(mtmp, otmp);

		if (otmp && otmp->oartifact == ART_MAXIMUM_PENALTY) {
			reallybadeffect(); reallybadeffect(); reallybadeffect(); reallybadeffect(); reallybadeffect(); reallybadeffect(); reallybadeffect(); reallybadeffect(); reallybadeffect(); reallybadeffect();
		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */
		how = SCR_COURSE_TRAVELING;

			d_level flev;

			if (mon_has_amulet(mtmp) || In_endgame(&u.uz)) {
			    if (vismon) {
				pline("%s seems very disoriented for a moment.",
					Monnam(mtmp));
				if (oseen) makeknown(SCR_COURSE_TRAVELING);
				}
			    return 2;
			}
			flev = random_branchport_level();
			migrate_to_level(mtmp, ledger_no(&flev), MIGR_RANDOM, (coord *)0);
			if (oseen) makeknown(SCR_COURSE_TRAVELING);

		return 2;
	    }

	case MUSE_WAN_TELE_LEVEL:
		if (mtmp->isshk || mtmp->isgd || mtmp->ispriest) return 2;
		m_flee(mtmp);
		mzapmsg(mtmp, otmp, TRUE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		if (oseen) makeknown(WAN_TELE_LEVEL);
		how = WAN_TELE_LEVEL;
			int nlev;
			d_level flev;

			if (mon_has_amulet(mtmp) || In_endgame(&u.uz)) {
			    if (vismon) {
				pline("%s seems very disoriented for a moment.",
					Monnam(mtmp));
				}
			    return 2;
			}
			nlev = random_teleport_level();
			if (nlev == depth(&u.uz)) {
			    if (vismon) {
				pline("%s shudders for a moment.",
								Monnam(mtmp));
				}
			    return 2;
			}
			get_level(&flev, nlev);
			migrate_to_level(mtmp, ledger_no(&flev), MIGR_RANDOM,
				(coord *)0);

		return 2;

	case MUSE_SCR_WARPING:
	    {
		if (mtmp->isshk || mtmp->isgd || mtmp->ispriest) return 2;
		m_flee(mtmp);
		mreadmsg(mtmp, otmp);

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */
		if (oseen) makeknown(SCR_WARPING);

		u_teleport_monB(mtmp, TRUE);

		return 2;
	    }
	case MUSE_SCR_ROOT_PASSWORD_DETECTION:
	    {
		if (mtmp->isshk || mtmp->isgd || mtmp->ispriest) return 2;
		m_flee(mtmp);
		mreadmsg(mtmp, otmp);

		if (otmp && otmp->oartifact == ART_ERASE_ALL_DATA && isevilvariant) {
			u.datadeletedefer = 1;
			datadeleteattack();
			verbalize("Ha ha ha ha ha ha ha, so you thought you could play in evil variant mode and somehow avoid all the evil variant bullshit? Well sucker, now your data has been deleted and you might as well give up.");
			if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */
			return 2;
		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		how = SCR_ROOT_PASSWORD_DETECTION;

			int nlev;
			d_level flev;

			if (mon_has_amulet(mtmp) || In_endgame(&u.uz)) {
			    if (vismon) {
				pline("%s seems very disoriented for a moment.",
					Monnam(mtmp));
				if (oseen) makeknown(SCR_ROOT_PASSWORD_DETECTION);
				}
			    return 2;
			}
			nlev = random_teleport_level();
			if (nlev == depth(&u.uz)) {
			    if (vismon) {
				pline("%s shudders for a moment.",
								Monnam(mtmp));
				if (oseen) makeknown(SCR_ROOT_PASSWORD_DETECTION);
				}
			    return 2;
			}
			get_level(&flev, nlev);
			migrate_to_level(mtmp, ledger_no(&flev), MIGR_RANDOM,
				(coord *)0);
			if (oseen) makeknown(SCR_ROOT_PASSWORD_DETECTION);

		return 2;
	    }
	case MUSE_WAN_DIGGING:
	    {	struct trap *ttmp;

		m_flee(mtmp);
		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		if (oseen) makeknown(WAN_DIGGING);
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		if (IS_FURNITURE(levl[mtmp->mx][mtmp->my].typ) ||
		    IS_DRAWBRIDGE(levl[mtmp->mx][mtmp->my].typ) ||
		    (is_drawbridge_wall(mtmp->mx, mtmp->my) >= 0) ||
		    (sstairs.sx && sstairs.sx == mtmp->mx &&
				   sstairs.sy == mtmp->my)) {
			pline_The("digging ray is ineffective.");
			return 2;
		}
		if (!Can_dig_down(&u.uz) || In_minotaurmaze(&u.uz)) {
		    if(canseemon(mtmp))
			pline_The("%s here is too hard to dig in.",
					surface(mtmp->mx, mtmp->my));
		    return 2;
		}
		ttmp = maketrap(mtmp->mx, mtmp->my, HOLE, 0, FALSE);
		if (!ttmp) return 2;
		seetrap(ttmp);
		if (vis) {
		    pline("%s has made a hole in the %s.", Monnam(mtmp),
				surface(mtmp->mx, mtmp->my));
		    pline("%s %s through...", Monnam(mtmp),
			  is_flyer(mtmp->data) ? "dives" : "falls");
		} else if (flags.soundok)
			You_hear("%s crash through the %s.", something,
				surface(mtmp->mx, mtmp->my));
		/* we made sure that there is a level for mtmp to go to */
		migrate_to_level(mtmp, ledger_no(&u.uz) + 1,
				 MIGR_RANDOM, (coord *)0);
		return 2;
	    }
	case MUSE_WAN_CREATE_HORDE:
	    {   coord cc;
		struct permonst *pm=rndmonst();
		int cnt = 1;
		if (!enexto(&cc, mtmp->mx, mtmp->my, pm)) return 0;
		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		if (oseen) makeknown(WAN_CREATE_HORDE);
		cnt = rno(14);

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		while(cnt--) {
			struct monst *mon;
			if (!enexto(&cc, mtmp->mx, mtmp->my, pm)) continue;
			mon = makemon(rndmonst(), cc.x, cc.y, NO_MM_FLAGS);
			if (mon) newsym(mon->mx,mon->my);
		}

		u.aggravation = 0;

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;
	    }
	case MUSE_WAN_CREATE_MONSTER:
	    {	coord cc;

		struct permonst *pm = 0;
		struct monst *mon;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		if (!enexto(&cc, mtmp->mx, mtmp->my, pm)) return 0;
		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		mon = makemon((struct permonst *)0, cc.x, cc.y, NO_MM_FLAGS);
		if (mon && canspotmon(mon) && oseen)
		    makeknown(WAN_CREATE_MONSTER);

		u.aggravation = 0;

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;
	    }

	case MUSE_SCR_CREATE_MONSTER:
	    {	coord cc;
		struct permonst *pm = 0, *fish = 0;
		int cnt = 1;
		struct monst *mon;
		boolean known = FALSE;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		if (!rn2(73)) cnt += rno(4);
		if (mtmp->mconf || otmp->cursed) cnt += rno(12);
		/*if (mtmp->mconf) pm = fish = &mons[PM_ACID_BLOB];*/ /* no easy blob fort building --Amy */
		/*else if (is_pool(mtmp->mx, mtmp->my))
		    fish = &mons[u.uinwater ? PM_GIANT_EEL : PM_CROCODILE];*/
		mreadmsg(mtmp, otmp);
		while(cnt--) {
		    /* `fish' potentially gives bias towards water locations;
		       `pm' is what to actually create (0 => random) */
		    if (!enexto(&cc, mtmp->mx, mtmp->my, fish)) break;
		    mon = makemon(pm, cc.x, cc.y, NO_MM_FLAGS);
		    if (mon && canspotmon(mon)) known = TRUE;
		}
		/* The only case where we don't use oseen.  For wands, you
		 * have to be able to see the monster zap the wand to know
		 * what type it is.  For teleport scrolls, you have to see
		 * the monster to know it teleported.
		 */
		if (known)
		    makeknown(SCR_CREATE_MONSTER);
		else if (!objects[SCR_CREATE_MONSTER].oc_name_known
			&& !objects[SCR_CREATE_MONSTER].oc_uname)
		    docall(otmp);

		u.aggravation = 0;

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;
	    }

	case MUSE_SCR_CREATE_VICTIM:
	    {	coord cc;
		struct permonst *pm = 0, *fish = 0;
		int cnt = 1;
		struct monst *mon;
		boolean known = FALSE;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		if (!rn2(73)) cnt += rno(4);
		if (mtmp->mconf || otmp->cursed) cnt += rno(12);
		if (otmp->oartifact == ART_BE_CAREFUL__THERE_EVERYWHE) cnt += rn1(25, 25);
		mreadmsg(mtmp, otmp);
		while(cnt--) {
		    mon = makemon(pm, 0, 0, NO_MM_FLAGS);
		    if (mon && canspotmon(mon)) known = TRUE;
		}
		/* The only case where we don't use oseen.  For wands, you
		 * have to be able to see the monster zap the wand to know
		 * what type it is.  For teleport scrolls, you have to see
		 * the monster to know it teleported.
		 */
		if (known)
		    makeknown(SCR_CREATE_VICTIM);
		else if (!objects[SCR_CREATE_VICTIM].oc_name_known
			&& !objects[SCR_CREATE_VICTIM].oc_uname)
		    docall(otmp);

		u.aggravation = 0;

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;
	    }

	case MUSE_BAG_OF_TRICKS:

		/* jonadab wants monsters to not use up charges if they apply this thing, but that would be too evil. --Amy */
	    {	coord cc;

		struct permonst *pm = 0;
		struct monst *mon;

		if (!enexto(&cc, mtmp->mx, mtmp->my, pm)) return 0;
		mzapmsg(mtmp, otmp, FALSE);
		if (rn2(2) || !ishaxor) otmp->spe--;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		mon = makemon((struct permonst *)0, cc.x, cc.y, NO_MM_FLAGS);
		if (mon && canspotmon(mon) && oseen)
		    makeknown(BAG_OF_TRICKS);

		if (otmp && otmp->oartifact == ART_VERY_TRICKY_INDEED) {
			if (!enexto(&cc, mtmp->mx, mtmp->my, pm)) return 0;
			mon = makemon((struct permonst *)0, cc.x, cc.y, NO_MM_FLAGS);
			if (mon && canspotmon(mon) && oseen)
			    makeknown(BAG_OF_TRICKS);
		}

		u.aggravation = 0;

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;
	    }

	case MUSE_WAN_SUMMON_UNDEAD:
	    {	coord cc;

		struct permonst *pm = 0;
		struct monst *mon;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		if (!enexto(&cc, mtmp->mx, mtmp->my, pm)) return 0;
		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		    switch (rn2(10)+1) {
		    case 1:
			mon = makemon(mkclass(S_VAMPIRE,0), cc.x, cc.y, NO_MM_FLAGS);
			break;
		    case 2:
		    case 3:
		    case 4:
		    case 5:
			mon = makemon(mkclass(S_ZOMBIE,0), cc.x, cc.y, NO_MM_FLAGS);
			break;
		    case 6:
		    case 7:
		    case 8:
			mon = makemon(mkclass(S_MUMMY,0), cc.x, cc.y, NO_MM_FLAGS);
			break;
		    case 9:
			mon = makemon(mkclass(S_GHOST,0), cc.x, cc.y, NO_MM_FLAGS);
			break;
		    case 10:
			mon = makemon(mkclass(S_WRAITH,0), cc.x, cc.y, NO_MM_FLAGS);
			break;
		    }

		/*mon = makemon((struct permonst *)0, cc.x, cc.y, NO_MM_FLAGS);*/
		if (mon && canspotmon(mon) && oseen)
		    makeknown(WAN_SUMMON_UNDEAD);

		u.aggravation = 0;

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;
	    }

	case MUSE_WAN_SUMMON_ELM:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
	      makeknown(WAN_SUMMON_ELM);

		{
		int aligntype;
		aligntype = rn2((int)A_LAWFUL+2) - 1;
		pline("A servant of %s appears!",aligns[1 - aligntype].noun);
		summon_minion(aligntype, TRUE);
		}

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_SCR_SUMMON_UNDEAD:

	    {	coord cc;
		struct permonst *pm = 0;
		int cnt = 1;
		struct monst *mon;
		boolean known = FALSE;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		if (rn2(2)) cnt += rnz(2);
		if (!rn2(73)) cnt += rno(4);
		if (mtmp->mconf || otmp->cursed) cnt += rno(12);
		mreadmsg(mtmp, otmp);
		while(cnt--) {

		    if (!enexto(&cc, mtmp->mx, mtmp->my, 0)) break;

		    /*mon = makemon(pm, cc.x, cc.y, NO_MM_FLAGS);*/

		    switch (rn2(10)+1) {
		    case 1:
			mon = makemon(mkclass(S_VAMPIRE,0), cc.x, cc.y, NO_MM_FLAGS);
			break;
		    case 2:
		    case 3:
		    case 4:
		    case 5:
			mon = makemon(mkclass(S_ZOMBIE,0), cc.x, cc.y, NO_MM_FLAGS);
			break;
		    case 6:
		    case 7:
		    case 8:
			mon = makemon(mkclass(S_MUMMY,0), cc.x, cc.y, NO_MM_FLAGS);
			break;
		    case 9:
			mon = makemon(mkclass(S_GHOST,0), cc.x, cc.y, NO_MM_FLAGS);
			break;
		    case 10:
			mon = makemon(mkclass(S_WRAITH,0), cc.x, cc.y, NO_MM_FLAGS);
			break;
		    }
		    if (mon && canspotmon(mon)) known = TRUE;
		}
		/* The only case where we don't use oseen.  For wands, you
		 * have to be able to see the monster zap the wand to know
		 * what type it is.  For teleport scrolls, you have to see
		 * the monster to know it teleported.
		 */
		if (known)
		    makeknown(SCR_SUMMON_UNDEAD);
		else if (!objects[SCR_SUMMON_UNDEAD].oc_name_known
			&& !objects[SCR_SUMMON_UNDEAD].oc_uname)
		    docall(otmp);

		u.aggravation = 0;

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;
	    }

	case MUSE_SCR_GROUP_SUMMONING:

	    {	coord cc;
		struct permonst *pm = 0;
		int cnt = rn3(14) + 2;
		struct monst *mon;
		boolean known = FALSE;
		int randmnst;
		int randmnsx;
		struct permonst *randmonstforspawn;
		int monstercolor;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		int spawntype = rnd(4);

		if (spawntype == 1) {
			randmnst = (rn2(187) + 1);
			randmnsx = (rn2(100) + 1);
		} else if (spawntype == 2) {
			randmonstforspawn = rndmonst();
		} else if (spawntype == 3) {
			monstercolor = rnd(15);
		} else {
			monstercolor = rnd(432);
		}

		if (mtmp->mconf || otmp->cursed) cnt += rno(12);

		    if (!enexto(&cc, mtmp->mx, mtmp->my, 0)) break;

		mreadmsg(mtmp, otmp);
		while(cnt--) {

		    if (!enexto(&cc, mtmp->mx, mtmp->my, 0)) break;

			if (spawntype == 1) {

			if (randmnst < 6)
		 	    mon = makemon(mkclass(S_ANT,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 9)
		 	    mon = makemon(mkclass(S_BLOB,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 11)
		 	    mon = makemon(mkclass(S_COCKATRICE,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 15)
		 	    mon = makemon(mkclass(S_DOG,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 18)
		 	    mon = makemon(mkclass(S_EYE,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 22)
		 	    mon = makemon(mkclass(S_FELINE,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 24)
		 	    mon = makemon(mkclass(S_GREMLIN,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 29)
		 	    mon = makemon(mkclass(S_HUMANOID,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 33)
		 	    mon = makemon(mkclass(S_IMP,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 36)
		 	    mon = makemon(mkclass(S_JELLY,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 41)
		 	    mon = makemon(mkclass(S_KOBOLD,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 44)
		 	    mon = makemon(mkclass(S_LEPRECHAUN,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 47)
		 	    mon = makemon(mkclass(S_MIMIC,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 50)
		 	    mon = makemon(mkclass(S_NYMPH,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 54)
		 	    mon = makemon(mkclass(S_ORC,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 55)
		 	    mon = makemon(mkclass(S_PIERCER,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 58)
		 	    mon = makemon(mkclass(S_QUADRUPED,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 62)
		 	    mon = makemon(mkclass(S_RODENT,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 65)
		 	    mon = makemon(mkclass(S_SPIDER,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 66)
		 	    mon = makemon(mkclass(S_TRAPPER,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 69)
		 	    mon = makemon(mkclass(S_UNICORN,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 71)
		 	    mon = makemon(mkclass(S_VORTEX,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 73)
		 	    mon = makemon(mkclass(S_WORM,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 75)
		 	    mon = makemon(mkclass(S_XAN,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 76)
		 	    mon = makemon(mkclass(S_LIGHT,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 77)
		 	    mon = makemon(mkclass(S_ZOUTHERN,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 78)
		 	    mon = makemon(mkclass(S_ANGEL,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 81)
		 	    mon = makemon(mkclass(S_BAT,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 83)
		 	    mon = makemon(mkclass(S_CENTAUR,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 86)
		 	    mon = makemon(mkclass(S_DRAGON,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 89)
		 	    mon = makemon(mkclass(S_ELEMENTAL,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 94)
		 	    mon = makemon(mkclass(S_FUNGUS,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 99)
		 	    mon = makemon(mkclass(S_GNOME,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 102)
		 	    mon = makemon(mkclass(S_GIANT,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 103)
		 	    mon = makemon(mkclass(S_JABBERWOCK,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 104)
		 	    mon = makemon(mkclass(S_KOP,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 105)
		 	    mon = makemon(mkclass(S_LICH,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 108)
		 	    mon = makemon(mkclass(S_MUMMY,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 110)
		 	    mon = makemon(mkclass(S_NAGA,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 113)
		 	    mon = makemon(mkclass(S_OGRE,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 115)
		 	    mon = makemon(mkclass(S_PUDDING,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 116)
		 	    mon = makemon(mkclass(S_QUANTMECH,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 118)
		 	    mon = makemon(mkclass(S_RUSTMONST,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 121)
		 	    mon = makemon(mkclass(S_SNAKE,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 123)
		 	    mon = makemon(mkclass(S_TROLL,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 124)
		 	    mon = makemon(mkclass(S_UMBER,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 125)
		 	    mon = makemon(mkclass(S_VAMPIRE,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 127)
		 	    mon = makemon(mkclass(S_WRAITH,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 128)
		 	    mon = makemon(mkclass(S_XORN,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 130)
		 	    mon = makemon(mkclass(S_YETI,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 135)
		 	    mon = makemon(mkclass(S_ZOMBIE,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 145)
		 	    mon = makemon(mkclass(S_HUMAN,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 147)
		 	    mon = makemon(mkclass(S_GHOST,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 149)
		 	    mon = makemon(mkclass(S_GOLEM,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 152)
		 	    mon = makemon(mkclass(S_DEMON,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 155)
		 	    mon = makemon(mkclass(S_EEL,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 160)
		 	    mon = makemon(mkclass(S_LIZARD,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 162)
		 	    mon = makemon(mkclass(S_BAD_FOOD,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 165)
		 	    mon = makemon(mkclass(S_BAD_COINS,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 166) {
				if (randmnsx < 96)
		 	    mon = makemon(mkclass(S_HUMAN,0), cc.x, cc.y, MM_ADJACENTOK);
				else
		 	    mon = makemon(mkclass(S_NEMESE,0), cc.x, cc.y, MM_ADJACENTOK);
				}
			else if (randmnst < 171)
		 	    mon = makemon(mkclass(S_GRUE,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 176)
		 	    mon = makemon(mkclass(S_WALLMONST,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 180)
		 	    mon = makemon(mkclass(S_RUBMONST,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 181) {
				if (randmnsx < 99)
		 	    mon = makemon(mkclass(S_HUMAN,0), cc.x, cc.y, MM_ADJACENTOK);
				else
		 	    mon = makemon(mkclass(S_ARCHFIEND,0), cc.x, cc.y, MM_ADJACENTOK);
				}
			else if (randmnst < 186)
		 	    mon = makemon(mkclass(S_TURRET,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 187)
		 	    mon = makemon(mkclass(S_FLYFISH,0), cc.x, cc.y, MM_ADJACENTOK);
			else
		 	    mon = makemon((struct permonst *)0, cc.x, cc.y, MM_ADJACENTOK);

			} else if (spawntype == 2) {

				mon = makemon(randmonstforspawn, cc.x, cc.y, MM_ADJACENTOK);

			} else if (spawntype == 3) {

				mon = makemon(colormon(monstercolor), cc.x, cc.y, MM_ADJACENTOK);

			} else {

				mon = makemon(specialtensmon(monstercolor), cc.x, cc.y, MM_ADJACENTOK);

			}

		    if (mon && canspotmon(mon)) known = TRUE;
		}
		/* The only case where we don't use oseen.  For wands, you
		 * have to be able to see the monster zap the wand to know
		 * what type it is.  For teleport scrolls, you have to see
		 * the monster to know it teleported.
		 */

		u.aggravation = 0;

		if (known)
		    makeknown(SCR_GROUP_SUMMONING);
		else if (!objects[SCR_GROUP_SUMMONING].oc_name_known
			&& !objects[SCR_GROUP_SUMMONING].oc_uname)
		    docall(otmp);
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;
	    }

	case MUSE_SCR_SUMMON_BOSS:

	    {	coord cc;
		struct permonst *pm = 0;
		struct monst *mon;
		boolean known = FALSE;
		int attempts = 0;

		if (!enexto(&cc, mtmp->mx, mtmp->my, 0)) break;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		mreadmsg(mtmp, otmp);

newboss:
		do {
			pm = rndmonst();
			attempts++;
			if (attempts && (attempts % 10000 == 0)) u.mondiffhack++;
			if (!rn2(2000)) reset_rndmonst(NON_PM);

		} while ( (!pm || (pm && !(pm->geno & G_UNIQ))) && attempts < 50000);

		if (!pm && rn2(50) ) {
			attempts = 0;
			goto newboss;
		}
		if (pm && !(pm->geno & G_UNIQ) && rn2(50) ) {
			attempts = 0;
			goto newboss;
		}

		if (pm) mon = makemon(pm, cc.x, cc.y, NO_MM_FLAGS);
	      if (mon && canspotmon(mon)) known = TRUE;

		if (known)
		    makeknown(SCR_SUMMON_BOSS);
		else if (!objects[SCR_SUMMON_BOSS].oc_name_known
			&& !objects[SCR_SUMMON_BOSS].oc_uname)
		    docall(otmp);

		u.aggravation = 0;
		u.mondiffhack = 0;

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;
	    }

	case MUSE_SCR_SUMMON_GHOST:

		{
		coord cc;   
		if (!enexto(&cc, mtmp->mx, mtmp->my, 0)) break;

		mreadmsg(mtmp, otmp);

		tt_mname(&cc, FALSE, 0);
		if (!objects[SCR_SUMMON_GHOST].oc_name_known
			&& !objects[SCR_SUMMON_GHOST].oc_uname)
		    docall(otmp);

		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;

	case MUSE_SCR_SUMMON_ELM:

		mreadmsg(mtmp, otmp);

		{
		int aligntype;
		aligntype = rn2((int)A_LAWFUL+2) - 1;
		pline("A servant of %s appears!",aligns[1 - aligntype].noun);
		summon_minion(aligntype, TRUE);
		}

		if (!objects[SCR_SUMMON_ELM].oc_name_known
			&& !objects[SCR_SUMMON_ELM].oc_uname)
		    docall(otmp);

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;

	case MUSE_TRAPDOOR:
		/* trap doors on "bottom" levels of dungeons are rock-drop
		 * trap doors, not holes in the floor.  We check here for
		 * safety.
		 */
		if (Is_botlevel(&u.uz)) return 0;
		m_flee(mtmp);
		if (vis) {
			struct trap *t;
			t = t_at(trapx,trapy);
			pline("%s %s into a %s!", Monnam(mtmp),
			makeplural(locomotion(mtmp->data, "jump")),
			t->ttyp == TRAPDOOR ? "trap door" : t->ttyp == SHAFT_TRAP ? "shaft" : t->ttyp == CURRENT_SHAFT ? "shaft" : "hole");
			if (levl[trapx][trapy].typ == SCORR) {
			    levl[trapx][trapy].typ = CORR;
			    unblock_point(trapx, trapy);
			}
			seetrap(t_at(trapx,trapy));
		}

		/*  don't use rloc_to() because worm tails must "move" */
		remove_monster(mtmp->mx, mtmp->my);
		newsym(mtmp->mx, mtmp->my);	/* update old location */
		place_monster(mtmp, trapx, trapy);
		if (mtmp->wormno) worm_move(mtmp);
		newsym(trapx, trapy);

		migrate_to_level(mtmp, ledger_no(&u.uz) + 1,
				 MIGR_RANDOM, (coord *)0);
		return 2;
	case MUSE_UPSTAIRS:
		/* Monsters without amulets escape the dungeon and are
		 * gone for good when they leave up the up stairs.
		 * Monsters with amulets would reach the endlevel,
		 * which we cannot allow since that would leave the
		 * player stranded.
		 */
		if (ledger_no(&u.uz) == 1) {
			if (mon_has_special(mtmp))
				return 0;
			if (vismon)
			    pline("%s escapes the dungeon!", Monnam(mtmp));
			mongone(mtmp);
			return 2;
		}
		m_flee(mtmp);
		if (Inhell && mon_has_amulet(mtmp) && !rn2(4) &&
			(dunlev(&u.uz) < dunlevs_in_dungeon(&u.uz) - 3)) {
		    if (vismon) pline(
     "As %s climbs the stairs, a mysterious force momentarily surrounds %s...",
				     mon_nam(mtmp), mhim(mtmp));
		    /* simpler than for the player; this will usually be
		       the Wizard and he'll immediately go right to the
		       upstairs, so there's not much point in having any
		       chance for a random position on the current level */
		    migrate_to_level(mtmp, ledger_no(&u.uz) + 1,
				     MIGR_RANDOM, (coord *)0);
		} else {
		    if (vismon) pline("%s escapes upstairs!", Monnam(mtmp));
		    migrate_to_level(mtmp, ledger_no(&u.uz) - 1,
				     MIGR_STAIRS_DOWN, (coord *)0);
		}
		return 2;
	case MUSE_DOWNSTAIRS:
		m_flee(mtmp);
		if (vismon) pline("%s escapes downstairs!", Monnam(mtmp));
		migrate_to_level(mtmp, ledger_no(&u.uz) + 1,
				 MIGR_STAIRS_UP, (coord *)0);
		return 2;
	case MUSE_UP_LADDER:
		m_flee(mtmp);
		if (vismon) pline("%s escapes up the ladder!", Monnam(mtmp));
		migrate_to_level(mtmp, ledger_no(&u.uz) - 1,
				 MIGR_LADDER_DOWN, (coord *)0);
		return 2;
	case MUSE_DN_LADDER:
		m_flee(mtmp);
		if (vismon) pline("%s escapes down the ladder!", Monnam(mtmp));
		migrate_to_level(mtmp, ledger_no(&u.uz) + 1,
				 MIGR_LADDER_UP, (coord *)0);
		return 2;
	case MUSE_SSTAIRS:
		m_flee(mtmp);
		/* the stairs leading up from the 1st level are */
		/* regular stairs, not sstairs.			*/
		if (sstairs.up) {
			if (vismon)
			    pline("%s escapes upstairs!", Monnam(mtmp));
			if(Inhell) {
			    migrate_to_level(mtmp, ledger_no(&sstairs.tolev),
					     MIGR_RANDOM, (coord *)0);
			    return 2;
			}
		} else	if (vismon)
		    pline("%s escapes downstairs!", Monnam(mtmp));
		migrate_to_level(mtmp, ledger_no(&sstairs.tolev),
				 MIGR_SSTAIRS, (coord *)0);
		return 2;
	case MUSE_TELEPORT_TRAP:
		m_flee(mtmp);
		if (vis) {
			pline("%s %s onto a teleport trap!", Monnam(mtmp),
				makeplural(locomotion(mtmp->data, "jump")));
			if (levl[trapx][trapy].typ == SCORR) {
			    levl[trapx][trapy].typ = CORR;
			    unblock_point(trapx, trapy);
			}
			seetrap(t_at(trapx,trapy));
		}
		/*  don't use rloc_to() because worm tails must "move" */
		remove_monster(mtmp->mx, mtmp->my);
		newsym(mtmp->mx, mtmp->my);	/* update old location */
		place_monster(mtmp, trapx, trapy);
		if (mtmp->wormno) worm_move(mtmp);
		newsym(trapx, trapy);

		goto mon_tele;
	/* [Tom] */
	case MUSE_WAN_HEALING:
		mzapmsg(mtmp, otmp, TRUE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		i = d(5,2) + 5 * !!bcsign(otmp);
		mtmp->mhp += i;
		if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = ++mtmp->mhpmax;
		if (!otmp->cursed) mtmp->mcansee = 1;
		if (vismon) pline("%s begins to look better.", Monnam(mtmp));
		if (oseen) makeknown(WAN_HEALING);
		if (mtmp->bleedout && mtmp->bleedout <= i) {
			mtmp->bleedout = 0;
			if (vismon) pline("%s's bleeding stops.", Monnam(mtmp));
		} else if (mtmp->bleedout) {
			mtmp->bleedout -= i;
			if (mtmp->bleedout < 0) mtmp->bleedout = 0; /* should never happen */
			if (vismon) pline("%s's bleeding diminishes.", Monnam(mtmp));
		}
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;
	case MUSE_WAN_EXTRA_HEALING:
		mzapmsg(mtmp, otmp, TRUE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		i = d(5,4) + 10 * !!bcsign(otmp);
		mtmp->mhp += i;
		if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = ++mtmp->mhpmax;
		if (!otmp->cursed) mtmp->mcansee = 1;
		if (vismon) pline("%s begins to look better.", Monnam(mtmp));
		if (oseen) makeknown(WAN_EXTRA_HEALING);
		if (mtmp->bleedout && mtmp->bleedout <= i) {
			mtmp->bleedout = 0;
			if (vismon) pline("%s's bleeding stops.", Monnam(mtmp));
		} else if (mtmp->bleedout) {
			mtmp->bleedout -= i;
			if (mtmp->bleedout < 0) mtmp->bleedout = 0; /* should never happen */
			if (vismon) pline("%s's bleeding diminishes.", Monnam(mtmp));
		}
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;
	case MUSE_WAN_FULL_HEALING:
		mzapmsg(mtmp, otmp, TRUE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		i = d(5,8) + 20 * !!bcsign(otmp);
		mtmp->mhp += i;
		if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = ++mtmp->mhpmax;
		if (!otmp->cursed) mtmp->mcansee = 1;
		if (vismon) pline("%s begins to look better.", Monnam(mtmp));
		if (oseen) makeknown(WAN_FULL_HEALING);
		if (mtmp->bleedout && mtmp->bleedout <= i) {
			mtmp->bleedout = 0;
			if (vismon) pline("%s's bleeding stops.", Monnam(mtmp));
		} else if (mtmp->bleedout) {
			mtmp->bleedout -= i;
			if (mtmp->bleedout < 0) mtmp->bleedout = 0; /* should never happen */
			if (vismon) pline("%s's bleeding diminishes.", Monnam(mtmp));
		}
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_MEDIUM_HEALING:
		mzapmsg(mtmp, otmp, TRUE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		i = d(5,3) + 7 * !!bcsign(otmp);
		mtmp->mhp += i;
		if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = ++mtmp->mhpmax;
		if (!otmp->cursed) mtmp->mcansee = 1;
		if (vismon) pline("%s begins to look better.", Monnam(mtmp));
		if (oseen) makeknown(WAN_MEDIUM_HEALING);
		if (mtmp->bleedout && mtmp->bleedout <= i) {
			mtmp->bleedout = 0;
			if (vismon) pline("%s's bleeding stops.", Monnam(mtmp));
		} else if (mtmp->bleedout) {
			mtmp->bleedout -= i;
			if (mtmp->bleedout < 0) mtmp->bleedout = 0; /* should never happen */
			if (vismon) pline("%s's bleeding diminishes.", Monnam(mtmp));
		}
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;
	case MUSE_WAN_STRONG_HEALING:
		mzapmsg(mtmp, otmp, TRUE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		i = d(5,6) + 15 * !!bcsign(otmp);
		mtmp->mhp += i;
		if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = ++mtmp->mhpmax;
		if (!otmp->cursed) mtmp->mcansee = 1;
		if (vismon) pline("%s begins to look better.", Monnam(mtmp));
		if (oseen) makeknown(WAN_STRONG_HEALING);
		if (mtmp->bleedout && mtmp->bleedout <= i) {
			mtmp->bleedout = 0;
			if (vismon) pline("%s's bleeding stops.", Monnam(mtmp));
		} else if (mtmp->bleedout) {
			mtmp->bleedout -= i;
			if (mtmp->bleedout < 0) mtmp->bleedout = 0; /* should never happen */
			if (vismon) pline("%s's bleeding diminishes.", Monnam(mtmp));
		}
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;
	case MUSE_WAN_SUPER_HEALING:
		mzapmsg(mtmp, otmp, TRUE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		i = d(5,12) + 30 * !!bcsign(otmp);
		mtmp->mhp += i;
		if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = ++mtmp->mhpmax;
		if (!otmp->cursed) mtmp->mcansee = 1;
		if (vismon) pline("%s begins to look better.", Monnam(mtmp));
		if (oseen) makeknown(WAN_SUPER_HEALING);
		if (mtmp->bleedout && mtmp->bleedout <= i) {
			mtmp->bleedout = 0;
			if (vismon) pline("%s's bleeding stops.", Monnam(mtmp));
		} else if (mtmp->bleedout) {
			mtmp->bleedout -= i;
			if (mtmp->bleedout < 0) mtmp->bleedout = 0; /* should never happen */
			if (vismon) pline("%s's bleeding diminishes.", Monnam(mtmp));
		}
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_SCR_HEALING:

		mreadmsg(mtmp, otmp);

		if (!rn2(20)) i = mtmp->mhpmax;
		else if (!rn2(5)) i = d(8, 8);
		else i = d(8, 4);

		if (otmp->oartifact == ART_OVERHEAL_ME) i *= 3;

		mtmp->mhp += i;
		if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;

		if (vismon) pline("%s looks better.", Monnam(mtmp));
		if (oseen) makeknown(SCR_HEALING);
		if (mtmp->bleedout && mtmp->bleedout <= i) {
			mtmp->bleedout = 0;
			if (vismon) pline("%s's bleeding stops.", Monnam(mtmp));
		} else if (mtmp->bleedout) {
			mtmp->bleedout -= i;
			if (mtmp->bleedout < 0) mtmp->bleedout = 0; /* should never happen */
			if (vismon) pline("%s's bleeding diminishes.", Monnam(mtmp));
		}
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		/*m_useup(mtmp, otmp); */
		/* the wrong dual use of m_useup caused the game to destabilize!!! --Amy
		 * I'm sure nobody wants to know how much headache and nausea this bug has caused to me.
		 * But I mean, why the motherfucking hell does the game just close, with no error message???
		 * Actually there's the panic code for a reason! */
		return 2;

	case MUSE_SCR_EXTRA_HEALING:

		mreadmsg(mtmp, otmp);

		if (!rn2(20)) i = mtmp->mhpmax;
		else if (!rn2(5)) i = d(8, 40);
		else i = d(8, 20);

		if (otmp->oartifact == ART_SAVING_FOR_A_RAINY_DAY) i *= 10;

		mtmp->mhp += i;
		if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;

		if (vismon) pline("%s looks much better.", Monnam(mtmp));
		if (oseen) makeknown(SCR_EXTRA_HEALING);
		if (mtmp->bleedout && mtmp->bleedout <= i) {
			mtmp->bleedout = 0;
			if (vismon) pline("%s's bleeding stops.", Monnam(mtmp));
		} else if (mtmp->bleedout) {
			mtmp->bleedout -= i;
			if (mtmp->bleedout < 0) mtmp->bleedout = 0; /* should never happen */
			if (vismon) pline("%s's bleeding diminishes.", Monnam(mtmp));
		}
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_POWER_HEALING:

		mreadmsg(mtmp, otmp);

		i = mtmp->mhpmax;
		mtmp->mhp += i;
		if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;

		if (vismon) pline("%s looks fully healed.", Monnam(mtmp));
		if (oseen) makeknown(SCR_POWER_HEALING);
		if (mtmp->bleedout && mtmp->bleedout <= i) {
			mtmp->bleedout = 0;
			if (vismon) pline("%s's bleeding stops.", Monnam(mtmp));
		} else if (mtmp->bleedout) {
			mtmp->bleedout -= i;
			if (mtmp->bleedout < 0) mtmp->bleedout = 0; /* should never happen */
			if (vismon) pline("%s's bleeding diminishes.", Monnam(mtmp));
		}
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */
		return 2;

	case MUSE_POT_HEALING:
		mquaffmsg(mtmp, otmp);
		i = d(6 + 2 * bcsign(otmp), 4);
		mtmp->mhp += i;
		if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = ++mtmp->mhpmax;
		if (!otmp->cursed && !mtmp->mcansee) {
			mtmp->mcansee = 1;
			mtmp->mblinded = 0;
			if (vismon) pline(mcsa, Monnam(mtmp));
		}
		if (vismon) pline("%s looks better.", Monnam(mtmp));
		if (oseen) makeknown(POT_HEALING);
		if (mtmp->bleedout && mtmp->bleedout <= i) {
			mtmp->bleedout = 0;
			if (vismon) pline("%s's bleeding stops.", Monnam(mtmp));
		} else if (mtmp->bleedout) {
			mtmp->bleedout -= i;
			if (mtmp->bleedout < 0) mtmp->bleedout = 0; /* should never happen */
			if (vismon) pline("%s's bleeding diminishes.", Monnam(mtmp));
		}
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;
	case MUSE_POT_EXTRA_HEALING:

		if (otmp->oartifact == ART_DAMN_WORD_PLAY) {
			mtmp->mhpmax += 5;
			mtmp->mhp += 5;
		}

		mquaffmsg(mtmp, otmp);
		i = d(6 + 2 * bcsign(otmp), 8);
		mtmp->mhp += i;
		if (mtmp->mhp > mtmp->mhpmax)
			mtmp->mhp = (mtmp->mhpmax += (otmp->blessed ? 5 : 2));
		if (!mtmp->mcansee) {
			mtmp->mcansee = 1;
			mtmp->mblinded = 0;
			if (vismon) pline(mcsa, Monnam(mtmp));
		}
		if (vismon) pline("%s looks much better.", Monnam(mtmp));
		if (oseen) makeknown(POT_EXTRA_HEALING);
		if (mtmp->bleedout && mtmp->bleedout <= i) {
			mtmp->bleedout = 0;
			if (vismon) pline("%s's bleeding stops.", Monnam(mtmp));
		} else if (mtmp->bleedout) {
			mtmp->bleedout -= i;
			if (mtmp->bleedout < 0) mtmp->bleedout = 0; /* should never happen */
			if (vismon) pline("%s's bleeding diminishes.", Monnam(mtmp));
		}
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;
	case MUSE_POT_FULL_HEALING:
		mquaffmsg(mtmp, otmp);
		if (otmp->otyp == POT_SICKNESS) unbless(otmp); /* Pestilence */
		mtmp->mhp = (mtmp->mhpmax += (otmp->blessed ? 8 : 4));
		if (!mtmp->mcansee && otmp->otyp != POT_SICKNESS) {
			mtmp->mcansee = 1;
			mtmp->mblinded = 0;
			if (vismon) pline(mcsa, Monnam(mtmp));
		}
		if (vismon) pline("%s looks completely healed.", Monnam(mtmp));
		if (oseen) makeknown(otmp->otyp);
		if (mtmp->bleedout && mtmp->bleedout <= 400) {
			mtmp->bleedout = 0;
			if (vismon) pline("%s's bleeding stops.", Monnam(mtmp));
		} else if (mtmp->bleedout) {
			mtmp->bleedout -= 400;
			if (mtmp->bleedout < 0) mtmp->bleedout = 0; /* should never happen */
			if (vismon) pline("%s's bleeding diminishes.", Monnam(mtmp));
		}
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;
	case MUSE_POT_OIL:
		mquaffmsg(mtmp, otmp);
		mtmp->mhp += 200;
		if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;
		if (vismon) pline("%s restored its fuel!", Monnam(mtmp));
		if (oseen) makeknown(POT_OIL);
		if (mtmp->bleedout) {
			mtmp->bleedout = 0;
			if (vismon) pline("%s's leak is repaired.", Monnam(mtmp));
		}
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;
	case MUSE_POT_CURE_WOUNDS:
		mquaffmsg(mtmp, otmp);
		i = d(6 + 2 * bcsign(otmp), 4);
		mtmp->mhp += i;
		if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;
		if (vismon) pline("%s looks better.", Monnam(mtmp));
		if (oseen) makeknown(POT_CURE_WOUNDS);
		if (mtmp->bleedout && mtmp->bleedout <= i) {
			mtmp->bleedout = 0;
			if (vismon) pline("%s's bleeding stops.", Monnam(mtmp));
		} else if (mtmp->bleedout) {
			mtmp->bleedout -= i;
			if (mtmp->bleedout < 0) mtmp->bleedout = 0; /* should never happen */
			if (vismon) pline("%s's bleeding diminishes.", Monnam(mtmp));
		}
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;
	case MUSE_POT_CURE_SERIOUS_WOUNDS:
		mquaffmsg(mtmp, otmp);
		i = d(6 + 2 * bcsign(otmp), 8);
		mtmp->mhp += i;
		if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;
		if (vismon) pline("%s looks much better.", Monnam(mtmp));
		if (oseen) makeknown(POT_CURE_SERIOUS_WOUNDS);
		if (mtmp->bleedout && mtmp->bleedout <= i) {
			mtmp->bleedout = 0;
			if (vismon) pline("%s's bleeding stops.", Monnam(mtmp));
		} else if (mtmp->bleedout) {
			mtmp->bleedout -= i;
			if (mtmp->bleedout < 0) mtmp->bleedout = 0; /* should never happen */
			if (vismon) pline("%s's bleeding diminishes.", Monnam(mtmp));
		}
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;
	case MUSE_POT_CURE_CRITICAL_WOUNDS:
		mquaffmsg(mtmp, otmp);
		mtmp->mhp = mtmp->mhpmax;
		if (vismon) pline("%s looks completely healed.", Monnam(mtmp));
		if (oseen) makeknown(otmp->otyp);
		if (mtmp->bleedout && mtmp->bleedout <= 400) {
			mtmp->bleedout = 0;
			if (vismon) pline("%s's bleeding stops.", Monnam(mtmp));
		} else if (mtmp->bleedout) {
			mtmp->bleedout -= 400;
			if (mtmp->bleedout < 0) mtmp->bleedout = 0; /* should never happen */
			if (vismon) pline("%s's bleeding diminishes.", Monnam(mtmp));
		}
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;
	case MUSE_RIN_TIMELY_BACKUP:
		rinspe = (otmp->spe);
		if (rinspe < 1) {
		if (vismon) pline("%s fumbles around, but nothing happens!", Monnam(mtmp));
		if (oseen) makeknown(otmp->otyp);
		m_useup(mtmp, otmp);
		return 2;
		}
		mtmp->mhp = (mtmp->mhpmax += (otmp->blessed ? 8 : 4));
		mtmp->mhpmax += rinspe;
		if (!mtmp->mcansee && otmp->otyp != POT_SICKNESS) {
			mtmp->mcansee = 1;
			mtmp->mblinded = 0;
			if (vismon) pline(mcsa, Monnam(mtmp));
		}
		if (vismon) pline("%s used a backup, and is looking healthy again!", Monnam(mtmp));
		if (oseen) makeknown(otmp->otyp);
		if (rn2(2) || !ishaxor) otmp->spe -= 1;
		return 2;
	case MUSE_POT_VAMPIRE_BLOOD:
		mquaffmsg(mtmp, otmp);
		if (!otmp->cursed) {
		    i = rnd(50) + rnd(5); /* boosted by Amy */
		    mtmp->mhp += i;
		    mtmp->mhpmax += rnd(5);
		    if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;
		    if (vismon) pline("%s looks full of life.", Monnam(mtmp));
		    if (mtmp->bleedout) mtmp->bleedout = 0;
		}
		else if (vismon)
		    pline("%s discards the congealed blood in disgust.", Monnam(mtmp));
		if (oseen) makeknown(POT_VAMPIRE_BLOOD);
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;
	case MUSE_POT_BLOOD:
		mquaffmsg(mtmp, otmp);
		if (!otmp->cursed) {
		    i = rnd(8) + rnd(2);
		    mtmp->mhp += i;
		    mtmp->mhpmax += i;
		    if (vismon) pline("%s looks full of life.", Monnam(mtmp));
		    if (mtmp->bleedout) mtmp->bleedout = 0;
		}
		else if (vismon)
		    pline("%s discards the congealed blood in disgust.", Monnam(mtmp));
		if (oseen) makeknown(POT_BLOOD);
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;
	case MUSE_LIZARD_CORPSE:
		/* not actually called for its unstoning effect */
		mon_consume_unstone(mtmp, otmp, FALSE, FALSE);
		return 2;
	case 0: return 0; /* i.e. an exploded wand */
	default: impossible("%s wanted to perform action %d?", Monnam(mtmp),
			m.has_defense);
		break;
	}
	return 0;
#undef m_flee
}

int
rnd_defensive_item(mtmp)
struct monst *mtmp;
{
	struct permonst *pm = mtmp->data;
	int difficulty = monstr[(monsndx(pm))];
	int trycnt = 0;

	if((is_animal(pm) || attacktype(pm, AT_EXPL) || mindless(mtmp->data)
			|| pm->mlet == S_GHOST
			|| pm->mlet == S_KOP
		) && issoviet) return 0;
    try_again:
	switch (rn2(8 + (difficulty > 3) + (difficulty > 6) +
				(difficulty > 8))) {
		case 6: case 9:
			if ((level.flags.noteleport || Race_if(PM_STABILISATOR) || u.antitelespelltimeout) && ++trycnt < 2)
			    goto try_again;
			if (!rn2(3)) return WAN_TELEPORTATION;
			/* else FALLTHRU */
		case 0: case 1:
			return SCR_TELEPORTATION;
		case 8: case 10:
			if (!rn2(3)) return WAN_CREATE_MONSTER;
			/* else FALLTHRU */
		case 2: return SCR_CREATE_MONSTER;
		case 3: return POT_HEALING;
		case 4: return POT_EXTRA_HEALING;
		case 5: return (mtmp->data != &mons[PM_PESTILENCE]) ?
				POT_FULL_HEALING : POT_SICKNESS;
		case 7: if (is_floater(pm) || mtmp->isshk || mtmp->isgd
						|| mtmp->ispriest
									)
				return 0;
			else
				return WAN_DIGGING;
	}
	/*NOTREACHED*/
	return 0;
}

int
rnd_defensive_item_new(mtmp)
struct monst *mtmp;
{
	struct permonst *pm = mtmp->data;

	if((is_animal(pm) || attacktype(pm, AT_EXPL) || mindless(mtmp->data)
			|| pm->mlet == S_GHOST
			|| pm->mlet == S_KOP
		) && issoviet) return 0;
	switch (rn2(42)) {

		case 0: return SCR_TELEPORTATION;
		case 1: return POT_HEALING;
		case 2: return POT_EXTRA_HEALING;
		case 3: return WAN_DIGGING;
		case 4: return WAN_CREATE_MONSTER;
		case 5: return SCR_CREATE_MONSTER;
		case 6: return WAN_TELEPORTATION;
		case 7: return BUGLE;
		case 8: return UNICORN_HORN;
		case 9: return POT_FULL_HEALING;
		case 10: return SCR_SUMMON_UNDEAD;
		case 11: return WAN_HEALING;
		case 12: return WAN_EXTRA_HEALING;
		case 13: return WAN_CREATE_HORDE;
		case 14: return POT_VAMPIRE_BLOOD;
		case 15: return WAN_FULL_HEALING;
		case 16: return SCR_TELE_LEVEL;
		case 17: return SCR_ROOT_PASSWORD_DETECTION;
		case 18: return RIN_TIMELY_BACKUP;
		case 19: return WAN_SUMMON_UNDEAD;
		case 20: return SCR_HEALING;
		case 21: return SCR_WARPING;
		case 22: return BAG_OF_TRICKS;
		case 23: return WAN_TELE_LEVEL;
		case 24: return SCR_SUMMON_BOSS;
		case 25: return POT_CURE_WOUNDS;
		case 26: return POT_CURE_SERIOUS_WOUNDS;
		case 27: return POT_CURE_CRITICAL_WOUNDS;
		case 28: return SCR_POWER_HEALING;
		case 29: return SCR_CREATE_VICTIM;
		case 30: return SCR_GROUP_SUMMONING;
		case 31: return SCR_SUMMON_GHOST;
		case 32: return SCR_SUMMON_ELM;
		case 33: return WAN_SUMMON_ELM;
		case 34: return SCR_RELOCATION;
		case 35: return SCR_EXTRA_HEALING;
		case 36: return POT_BLOOD;
		case 37: return SCR_COURSE_TRAVELING;
		case 38: return WAN_MEDIUM_HEALING;
		case 39: return WAN_STRONG_HEALING;
		case 40: return WAN_SUPER_HEALING;
		case 41: return WAN_TELEPORT_SELF;
	}
	/*NOTREACHED*/
	return 0;
}


#define MUSE_WAN_DEATH 1
#define MUSE_WAN_SLEEP 2
#define MUSE_WAN_FIREBALL 3
#define MUSE_WAN_FIRE 4
#define MUSE_WAN_COLD 5
#define MUSE_WAN_LIGHTNING 6
#define MUSE_WAN_MAGIC_MISSILE 7
#define MUSE_WAN_STRIKING 8
#define MUSE_SCR_FIRE 9
#define MUSE_POT_PARALYSIS 10
#define MUSE_POT_BLINDNESS 11
#define MUSE_POT_CONFUSION 12
#define MUSE_POT_SLEEPING 13
#define MUSE_POT_ACID 14
#define MUSE_FROST_HORN 15
#define MUSE_FIRE_HORN 16
#define MUSE_WAN_DRAINING 17	/* KMH */
/*#define MUSE_WAN_TELEPORTATION 18*/
#define MUSE_SCR_EARTH 19
#define MUSE_POT_AMNESIA 20
#define MUSE_WAN_CANCELLATION 21	/* Lethe */
#define MUSE_POT_CYANIDE 22
#define MUSE_POT_RADIUM 23
#define MUSE_WAN_ACID 24
#define MUSE_SCR_TRAP_CREATION 25
#define MUSE_WAN_TRAP_CREATION 26
#define MUSE_SCR_FLOOD 27
#define MUSE_SCR_LAVA 28
#define MUSE_SCR_GROWTH 29
#define MUSE_SCR_ICE 30
#define MUSE_SCR_CLOUDS 31
#define MUSE_SCR_BARRHING 32
#define MUSE_WAN_SOLAR_BEAM 33
#define MUSE_SCR_LOCKOUT 34
#define MUSE_WAN_BANISHMENT 35
#define MUSE_POT_HALLUCINATION 36
#define MUSE_POT_ICE 37
#define MUSE_POT_STUNNING 38
#define MUSE_POT_NUMBNESS 39
#define MUSE_SCR_BAD_EFFECT 40
#define MUSE_WAN_BAD_EFFECT 41
#define MUSE_POT_FIRE 42
#define MUSE_WAN_SLOW_MONSTER 43
#define MUSE_WAN_FEAR 44
#define MUSE_POT_FEAR 45
#define MUSE_SCR_DESTROY_ARMOR 46
#define MUSE_SCR_STONING 47
#define MUSE_POT_URINE 48
#define MUSE_POT_SLIME 49
#define MUSE_POT_CANCELLATION 50
#define MUSE_WAN_STONING 51
#define MUSE_WAN_DISINTEGRATION 52
#define MUSE_WAN_PARALYSIS 53
#define MUSE_WAN_CURSE_ITEMS 54
#define MUSE_WAN_AMNESIA 55
#define MUSE_WAN_BAD_LUCK 56
#define MUSE_WAN_REMOVE_RESISTANCE 57
#define MUSE_WAN_CORROSION 58
#define MUSE_WAN_FUMBLING 59
#define MUSE_WAN_STARVATION 60
#define MUSE_WAN_PUNISHMENT 61
#define MUSE_SCR_PUNISHMENT 62
#define MUSE_WAN_MAKE_VISIBLE 63
#define MUSE_WAN_REDUCE_MAX_HITPOINTS 64
#define MUSE_WAN_CONFUSION 65
#define MUSE_WAN_SLIMING 66
#define MUSE_WAN_LYCANTHROPY 67
#define MUSE_SCR_CHAOS_TERRAIN 68
#define MUSE_SCR_WOUNDS 69
#define MUSE_SCR_BULLSHIT 70
#define MUSE_SCR_AMNESIA 71
#define MUSE_WAN_SUMMON_SEXY_GIRL 72
#define MUSE_SCR_DEMONOLOGY 73
#define MUSE_SCR_ELEMENTALISM 74
#define MUSE_SCR_NASTINESS 75
#define MUSE_SCR_GIRLINESS 76
#define MUSE_TEMPEST_HORN 77
#define MUSE_WAN_POISON 78
#define MUSE_SCR_CREATE_TRAP 79
#define MUSE_SCR_DESTROY_WEAPON 80
#define MUSE_WAN_DISINTEGRATION_BEAM 81
#define MUSE_WAN_CHROMATIC_BEAM 82
#define MUSE_WAN_STUN_MONSTER 83
#define MUSE_SCR_MEGALOAD 84
#define MUSE_SCR_ENRAGE 85
#define MUSE_WAN_TIDAL_WAVE 86
#define MUSE_SCR_ANTIMATTER 87
#define MUSE_WAN_DRAIN_MANA 88
#define MUSE_WAN_FINGER_BENDING 89
#define MUSE_SCR_IMMOBILITY 90
#define MUSE_WAN_IMMOBILITY 91
#define MUSE_SCR_FLOODING 92
#define MUSE_SCR_EGOISM 93
#define MUSE_WAN_EGOISM 94
#define MUSE_SCR_RUMOR 95
#define MUSE_SCR_MESSAGE 96
#define MUSE_SCR_SIN 97
#define MUSE_WAN_SIN 98
#define MUSE_WAN_INERTIA 99
#define MUSE_WAN_TIME 100
#define MUSE_WAN_LEVITATION 101
#define MUSE_WAN_PSYBEAM 102
#define MUSE_WAN_HYPER_BEAM 103
#define MUSE_WAN_DESLEXIFICATION 104
#define MUSE_WAN_INFERNO 105
#define MUSE_WAN_ICE_BEAM 106
#define MUSE_WAN_THUNDER 107
#define MUSE_WAN_SLUDGE 108
#define MUSE_WAN_TOXIC 109
#define MUSE_WAN_NETHER_BEAM 110
#define MUSE_WAN_AURORA_BEAM 111
#define MUSE_WAN_GRAVITY_BEAM 112
#define MUSE_WAN_CHLOROFORM 113
#define MUSE_WAN_DREAM_EATER 114
#define MUSE_WAN_BUBBLEBEAM 115
#define MUSE_WAN_GOOD_NIGHT 116
#define MUSE_SCR_VILENESS 117
#define MUSE_POT_DIMNESS 118
#define MUSE_POT_SALT_WATER 119
#define MUSE_SCR_OFFLEVEL_ITEM 120
#define MUSE_SCR_RAGNAROK 121
#define MUSE_POT_SANITY 122
#define MUSE_SCR_BAD_EQUIPMENT 123
#define MUSE_WAN_INSANITY 124
#define MUSE_WAN_BAD_EQUIPMENT 125
#define MUSE_SCR_NASTY_CURSE 126
#define MUSE_SCR_HYBRIDIZATION 127
#define MUSE_WAN_DISENCHANTMENT 128
#define MUSE_WAN_CONTAMINATION 129
#define MUSE_WAN_TREMBLING 130
#define MUSE_SCR_GRAVE 131
#define MUSE_SCR_TUNNELS 132
#define MUSE_SCR_FARMING 133
#define MUSE_SCR_MOUNTAINS 134
#define MUSE_SCR_DIVING 135
#define MUSE_SCR_CRYSTALLIZATION 136
#define MUSE_SCR_MOORLAND 137
#define MUSE_SCR_URINE 138
#define MUSE_SCR_QUICKSAND 139
#define MUSE_SCR_STYX 140
#define MUSE_SCR_SNOW 141
#define MUSE_SCR_ASH 142
#define MUSE_SCR_SAND 143
#define MUSE_SCR_PAVING 144
#define MUSE_SCR_HIGHWAY 145
#define MUSE_SCR_GRASSLAND 146
#define MUSE_SCR_NETHER 147
#define MUSE_SCR_STALACTITE 148
#define MUSE_SCR_CRYPT 149
#define MUSE_SCR_BUBBLE_BOBBLE 150
#define MUSE_SCR_RAIN 151
#define MUSE_WAN_CHAOS_TERRAIN 152
#define MUSE_WAN_FLEECY_TERRAIN 153
#define MUSE_ETHER_HORN 154
#define MUSE_SHADOW_HORN 155
#define MUSE_CHROME_HORN 156
#define MUSE_SCR_ILLUSION 157
#define MUSE_SCR_EVIL_VARIANT 158
#define MUSE_SCR_FEMINISM 159
#define MUSE_WAN_BLEEDING 160
#define MUSE_WAN_UNDRESSING 161
#define MUSE_WAN_STAT_REDUCTION 162
#define MUSE_SCR_VISIBLE_ITEM 163
#define MUSE_POT_NITROGLYCERIN 164
#define MUSE_DEATH_HORN 165

/* Select an offensive item/action for a monster.  Returns TRUE iff one is
 * found.
 */
boolean
find_offensive(mtmp)
struct monst *mtmp;
{
	register struct obj *obj;
	boolean ranged_stuff = lined_up(mtmp);
	boolean reflection_skip = (Reflecting && rn2(2));
	struct obj *helmet = which_armor(mtmp, W_ARMH);

	m.offensive = (struct obj *)0;
	m.has_offense = 0;

	if (!issoviet && !rn2(5)) return FALSE;

	if (mtmp->mpeaceful || ((is_animal(mtmp->data) ||
				mindless(mtmp->data) || nohands(mtmp->data)) && issoviet) )
		return FALSE;
	if (u.uswallow) return FALSE;
	if (in_your_sanctuary(mtmp, 0, 0)) return FALSE;
	if (dmgtype(mtmp->data, AD_HEAL) && !uwep
	    && !uarmu
	    && !uarm && !uarmh && !uarms && !uarmg && !uarmc && !uarmf)
		return FALSE;

	if (!ranged_stuff) return FALSE;
#define nomore(x) if(m.has_offense==x) continue;
	for(obj=mtmp->minvent; obj; obj=obj->nobj) {

		if (m.has_offense && rn2(3)) break; /* don't always use the same offensive pattern --Amy */

		/* nomore(MUSE_WAN_DEATH); */
		if (!reflection_skip) {
		    if(obj->otyp == WAN_DEATH && obj->spe > 0 && (ZAP_POS(levl[u.ux][u.uy].typ) )) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_DEATH;
		    }
		    nomore(MUSE_WAN_SLEEP);
		    if(obj->otyp == WAN_SLEEP && obj->spe > 0 && multi >= 0 && (ZAP_POS(levl[u.ux][u.uy].typ) )) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_SLEEP;
		    }
		    nomore(MUSE_WAN_PARALYSIS);
		    if(obj->otyp == WAN_PARALYSIS && obj->spe > 0 && multi >= 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_PARALYSIS;
		    }
		    nomore(MUSE_WAN_STONING);
		    if(obj->otyp == WAN_STONING && obj->spe > 0 && !Stoned) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_STONING;
		    }
		    nomore(MUSE_WAN_DISINTEGRATION);
		    if(obj->otyp == WAN_DISINTEGRATION && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_DISINTEGRATION;
		    }
/*WAC fixed*/
/* [Tom] doesn't work...*/

		    nomore(MUSE_WAN_INFERNO);
		    if(obj->otyp == WAN_INFERNO && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_INFERNO;
                    }
		    nomore(MUSE_WAN_ICE_BEAM);
		    if(obj->otyp == WAN_ICE_BEAM && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_ICE_BEAM;
                    }
		    nomore(MUSE_WAN_THUNDER);
		    if(obj->otyp == WAN_THUNDER && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_THUNDER;
                    }
		    nomore(MUSE_WAN_SLUDGE);
		    if(obj->otyp == WAN_SLUDGE && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_SLUDGE;
                    }
		    nomore(MUSE_WAN_TOXIC);
		    if(obj->otyp == WAN_TOXIC && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_TOXIC;
                    }
		    nomore(MUSE_WAN_NETHER_BEAM);
		    if(obj->otyp == WAN_NETHER_BEAM && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_NETHER_BEAM;
                    }
		    nomore(MUSE_WAN_AURORA_BEAM);
		    if(obj->otyp == WAN_AURORA_BEAM && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_AURORA_BEAM;
                    }
		    nomore(MUSE_WAN_GRAVITY_BEAM);
		    if(obj->otyp == WAN_GRAVITY_BEAM && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_GRAVITY_BEAM;
                    }
		    nomore(MUSE_WAN_CHLOROFORM);
		    if(obj->otyp == WAN_CHLOROFORM && obj->spe > 0 && multi >= 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_CHLOROFORM;
                    }
		    nomore(MUSE_WAN_DREAM_EATER);
		    if(obj->otyp == WAN_DREAM_EATER && obj->spe > 0 && multi < 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_DREAM_EATER;
                    }
		    nomore(MUSE_WAN_BUBBLEBEAM);
		    if(obj->otyp == WAN_BUBBLEBEAM && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_BUBBLEBEAM;
                    }
		    nomore(MUSE_WAN_GOOD_NIGHT);
		    if(obj->otyp == WAN_GOOD_NIGHT && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_GOOD_NIGHT;
                    }
		    nomore(MUSE_WAN_FIREBALL);
		    if(obj->otyp == WAN_FIREBALL && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_FIREBALL;
                    }
		    nomore(MUSE_WAN_ACID);
		    if(obj->otyp == WAN_ACID && obj->spe > 0 && (ZAP_POS(levl[u.ux][u.uy].typ) )) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_ACID;
                    }
		    nomore(MUSE_WAN_SOLAR_BEAM);                    
		    if(obj->otyp == WAN_SOLAR_BEAM && obj->spe > 0 && (ZAP_POS(levl[u.ux][u.uy].typ) )) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_SOLAR_BEAM;
                    }
		    nomore(MUSE_WAN_PSYBEAM);                    
		    if(obj->otyp == WAN_PSYBEAM && obj->spe > 0 && (ZAP_POS(levl[u.ux][u.uy].typ) )) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_PSYBEAM;
                    }
		    nomore(MUSE_WAN_POISON);
		    if(obj->otyp == WAN_POISON && obj->spe > 0 && (ZAP_POS(levl[u.ux][u.uy].typ) )) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_POISON;
                    }
		    nomore(MUSE_WAN_HYPER_BEAM);
		    if(obj->otyp == WAN_HYPER_BEAM && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_HYPER_BEAM;
                    }
		    nomore(MUSE_WAN_CHROMATIC_BEAM);
		    if(obj->otyp == WAN_CHROMATIC_BEAM && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_CHROMATIC_BEAM;
                    }
		    nomore(MUSE_WAN_DISINTEGRATION_BEAM);
		    if(obj->otyp == WAN_DISINTEGRATION_BEAM && obj->spe > 0 && (ZAP_POS(levl[u.ux][u.uy].typ) )) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_DISINTEGRATION_BEAM;
                    }
		    nomore(MUSE_WAN_FIRE);
		    if(obj->otyp == WAN_FIRE && obj->spe > 0 && (ZAP_POS(levl[u.ux][u.uy].typ) )) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_FIRE;
		    }
		    nomore(MUSE_FIRE_HORN);
		    if(obj->otyp == FIRE_HORN && obj->spe > 0 && (ZAP_POS(levl[u.ux][u.uy].typ) )) {
			m.offensive = obj;
			m.has_offense = MUSE_FIRE_HORN;
		    }
		    nomore(MUSE_WAN_COLD);
		    if(obj->otyp == WAN_COLD && obj->spe > 0 && (ZAP_POS(levl[u.ux][u.uy].typ) )) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_COLD;
		    }
		    nomore(MUSE_FROST_HORN);
		    if(obj->otyp == FROST_HORN && obj->spe > 0 && (ZAP_POS(levl[u.ux][u.uy].typ) )) {
			m.offensive = obj;
			m.has_offense = MUSE_FROST_HORN;
		    }
		    nomore(MUSE_TEMPEST_HORN);
		    if(obj->otyp == TEMPEST_HORN && obj->spe > 0 && (ZAP_POS(levl[u.ux][u.uy].typ) )) {
			m.offensive = obj;
			m.has_offense = MUSE_TEMPEST_HORN;
		    }
		    nomore(MUSE_ETHER_HORN);
		    if(obj->otyp == ETHER_HORN && obj->spe > 0 && (ZAP_POS(levl[u.ux][u.uy].typ) )) {
			m.offensive = obj;
			m.has_offense = MUSE_ETHER_HORN;
		    }
		    nomore(MUSE_SHADOW_HORN);
		    if(obj->otyp == SHADOW_HORN && obj->spe > 0 && (ZAP_POS(levl[u.ux][u.uy].typ) )) {
			m.offensive = obj;
			m.has_offense = MUSE_SHADOW_HORN;
		    }
		    nomore(MUSE_CHROME_HORN);
		    if(obj->otyp == CHROME_HORN && obj->spe > 0 && (ZAP_POS(levl[u.ux][u.uy].typ) )) {
			m.offensive = obj;
			m.has_offense = MUSE_CHROME_HORN;
		    }
		    nomore(MUSE_DEATH_HORN);
		    if(obj->otyp == DEATH_HORN && obj->spe > 0 && (ZAP_POS(levl[u.ux][u.uy].typ) )) {
			m.offensive = obj;
			m.has_offense = MUSE_DEATH_HORN;
		    }
		    nomore(MUSE_WAN_LIGHTNING);
		    if(obj->otyp == WAN_LIGHTNING && obj->spe > 0 && (ZAP_POS(levl[u.ux][u.uy].typ) )) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_LIGHTNING;
		    }
		    nomore(MUSE_WAN_MAGIC_MISSILE);
		    if(obj->otyp == WAN_MAGIC_MISSILE && obj->spe > 0 && (ZAP_POS(levl[u.ux][u.uy].typ) )) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_MAGIC_MISSILE;
		    }
		}
		/* wands of draining in monster hands before you've had the chance to level up at least once are BS...
		 * if you've ever been XL2 or higher, though, they can and will zap you at XL1 and end your game --Amy */
		nomore(MUSE_WAN_DRAINING);
		if(obj->otyp == WAN_DRAINING && (u.urmaxlvlUP >= 2) && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_DRAINING;
		}
		nomore(MUSE_WAN_TIME);
		if(obj->otyp == WAN_TIME && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_TIME;
		}
		nomore(MUSE_WAN_FEAR);
		if(obj->otyp == WAN_FEAR && obj->spe > 0 && !Feared) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_FEAR;
		}
		nomore(MUSE_WAN_INERTIA);
		if(obj->otyp == WAN_INERTIA && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_INERTIA;
		}
		nomore(MUSE_WAN_SLOW_MONSTER);
		if(obj->otyp == WAN_SLOW_MONSTER && obj->spe > 0 && ((HFast & (TIMEOUT | INTRINSIC)) || isevilvariant) ) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_SLOW_MONSTER;
		}
		nomore(MUSE_WAN_STRIKING);
		if(obj->otyp == WAN_STRIKING && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_STRIKING;
		}
		nomore(MUSE_WAN_BANISHMENT);
		if(obj->otyp == WAN_BANISHMENT && !(obj->oartifact == ART_ASS_DEAF_AUGER) && obj->spe > 0 && !u.banishmentbeam) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_BANISHMENT;
		}
		nomore(MUSE_POT_PARALYSIS);
		if(obj->otyp == POT_PARALYSIS && multi >= 0) {
			m.offensive = obj;
			m.has_offense = MUSE_POT_PARALYSIS;
		}
		nomore(MUSE_POT_BLINDNESS);
		if(obj->otyp == POT_BLINDNESS && !attacktype(mtmp->data, AT_GAZE)) {
			m.offensive = obj;
			m.has_offense = MUSE_POT_BLINDNESS;
		}
		nomore(MUSE_POT_CONFUSION);
		if(obj->otyp == POT_CONFUSION) {
			m.offensive = obj;
			m.has_offense = MUSE_POT_CONFUSION;
		}

		nomore(MUSE_POT_STUNNING);
		if(obj->otyp == POT_STUNNING) {
			m.offensive = obj;
			m.has_offense = MUSE_POT_STUNNING;
		}
		nomore(MUSE_POT_ICE);
		if(obj->otyp == POT_ICE) {
			m.offensive = obj;
			m.has_offense = MUSE_POT_ICE;
		}
		nomore(MUSE_POT_FEAR);
		if(obj->otyp == POT_FEAR) {
			m.offensive = obj;
			m.has_offense = MUSE_POT_FEAR;
		}
		nomore(MUSE_POT_FIRE);
		if(obj->otyp == POT_FIRE) {
			m.offensive = obj;
			m.has_offense = MUSE_POT_FIRE;
		}
		nomore(MUSE_POT_NITROGLYCERIN);
		if(itemhasappearance(obj, APP_POTION_NITROGLYCERIN)) {
			m.offensive = obj;
			m.has_offense = MUSE_POT_NITROGLYCERIN;
		}
		nomore(MUSE_POT_SALT_WATER);
		if(obj->otyp == POT_SALT_WATER && Race_if(PM_ELONA_SNAIL) && !Upolyd) {
			m.offensive = obj;
			m.has_offense = MUSE_POT_SALT_WATER;
		}
		nomore(MUSE_POT_DIMNESS);
		if(obj->otyp == POT_DIMNESS) {
			m.offensive = obj;
			m.has_offense = MUSE_POT_DIMNESS;
		}
		nomore(MUSE_POT_SANITY);
		if(obj->otyp == POT_SANITY) {
			m.offensive = obj;
			m.has_offense = MUSE_POT_SANITY;
		}
		nomore(MUSE_POT_NUMBNESS);
		if(obj->otyp == POT_NUMBNESS) {
			m.offensive = obj;
			m.has_offense = MUSE_POT_NUMBNESS;
		}
		nomore(MUSE_POT_URINE);
		if(obj->otyp == POT_URINE) {
			m.offensive = obj;
			m.has_offense = MUSE_POT_URINE;
		}
		nomore(MUSE_POT_SLIME);
		if(obj->otyp == POT_SLIME && !Slimed) {
			m.offensive = obj;
			m.has_offense = MUSE_POT_SLIME;
		}
		nomore(MUSE_POT_CANCELLATION);
		if(obj->otyp == POT_CANCELLATION) {
			m.offensive = obj;
			m.has_offense = MUSE_POT_CANCELLATION;
		}
		nomore(MUSE_POT_HALLUCINATION);
		if(obj->otyp == POT_HALLUCINATION) {
			m.offensive = obj;
			m.has_offense = MUSE_POT_HALLUCINATION;
		}

		nomore(MUSE_POT_SLEEPING);
		if(obj->otyp == POT_SLEEPING) {
			m.offensive = obj;
			m.has_offense = MUSE_POT_SLEEPING;
		}
		/* Mik's Lethe patch - monsters use !oAmnesia */
		nomore(MUSE_POT_AMNESIA);
		if (obj->otyp == POT_AMNESIA) {
			m.offensive   = obj;
			m.has_offense = MUSE_POT_AMNESIA;
		}
		nomore(MUSE_POT_CYANIDE);
		if (obj->otyp == POT_CYANIDE) {
			m.offensive   = obj;
			m.has_offense = MUSE_POT_CYANIDE;
		}
		nomore(MUSE_POT_RADIUM);
		if (obj->otyp == POT_RADIUM) {
			m.offensive   = obj;
			m.has_offense = MUSE_POT_RADIUM;
		}
		nomore(MUSE_POT_SLEEPING);
		if(obj->otyp == POT_SLEEPING) {
			m.offensive = obj;
			m.has_offense = MUSE_POT_SLEEPING;
		}
		/* KMH, balance patch -- monsters use potion of acid */
		nomore(MUSE_POT_ACID);
		if(obj->otyp == POT_ACID) {
			m.offensive = obj;
			m.has_offense = MUSE_POT_ACID;
		}
		nomore(MUSE_SCR_TRAP_CREATION);
		if(obj->otyp == SCR_TRAP_CREATION) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_TRAP_CREATION;
		}
		nomore(MUSE_SCR_CREATE_TRAP);
		if(obj->otyp == SCR_CREATE_TRAP) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_CREATE_TRAP;
		}
		nomore(MUSE_SCR_GIRLINESS);
		if(obj->otyp == SCR_GIRLINESS) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_GIRLINESS;
		}
		nomore(MUSE_SCR_NASTINESS);
		if(obj->otyp == SCR_NASTINESS) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_NASTINESS;
		}
		nomore(MUSE_SCR_DEMONOLOGY);
		if(obj->otyp == SCR_DEMONOLOGY) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_DEMONOLOGY;
		}
		nomore(MUSE_SCR_ELEMENTALISM);
		if(obj->otyp == SCR_ELEMENTALISM) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_ELEMENTALISM;
		}
		nomore(MUSE_SCR_FLOOD);
		if(obj->otyp == SCR_FLOOD) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_FLOOD;
		}
		nomore(MUSE_SCR_LAVA);
		if(obj->otyp == SCR_LAVA) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_LAVA;
		}
		nomore(MUSE_SCR_GRAVE);
		if(obj->otyp == SCR_GRAVE) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_GRAVE;
		}
		nomore(MUSE_SCR_TUNNELS);
		if(obj->otyp == SCR_TUNNELS) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_TUNNELS;
		}
		nomore(MUSE_SCR_FARMING);
		if(obj->otyp == SCR_FARMING) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_FARMING;
		}
		nomore(MUSE_SCR_MOUNTAINS);
		if(obj->otyp == SCR_MOUNTAINS) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_MOUNTAINS;
		}
		nomore(MUSE_SCR_DIVING);
		if(obj->otyp == SCR_DIVING) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_DIVING;
		}
		nomore(MUSE_SCR_CRYSTALLIZATION);
		if(obj->otyp == SCR_CRYSTALLIZATION) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_CRYSTALLIZATION;
		}
		nomore(MUSE_SCR_MOORLAND);
		if(obj->otyp == SCR_MOORLAND) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_MOORLAND;
		}
		nomore(MUSE_SCR_URINE);
		if(obj->otyp == SCR_URINE) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_URINE;
		}
		nomore(MUSE_SCR_QUICKSAND);
		if(obj->otyp == SCR_QUICKSAND) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_QUICKSAND;
		}
		nomore(MUSE_SCR_STYX);
		if(obj->otyp == SCR_STYX) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_STYX;
		}
		nomore(MUSE_SCR_SNOW);
		if(obj->otyp == SCR_SNOW) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_SNOW;
		}
		nomore(MUSE_SCR_ASH);
		if(obj->otyp == SCR_ASH) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_ASH;
		}
		nomore(MUSE_SCR_SAND);
		if(obj->otyp == SCR_SAND) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_SAND;
		}
		nomore(MUSE_SCR_PAVING);
		if(obj->otyp == SCR_PAVING) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_PAVING;
		}
		nomore(MUSE_SCR_HIGHWAY);
		if(obj->otyp == SCR_HIGHWAY) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_HIGHWAY;
		}
		nomore(MUSE_SCR_GRASSLAND);
		if(obj->otyp == SCR_GRASSLAND) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_GRASSLAND;
		}
		nomore(MUSE_SCR_NETHER);
		if(obj->otyp == SCR_NETHER) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_NETHER;
		}
		nomore(MUSE_SCR_STALACTITE);
		if(obj->otyp == SCR_STALACTITE) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_STALACTITE;
		}
		nomore(MUSE_SCR_CRYPT);
		if(obj->otyp == SCR_CRYPT) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_CRYPT;
		}
		nomore(MUSE_SCR_BUBBLE_BOBBLE);
		if(obj->otyp == SCR_BUBBLE_BOBBLE) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_BUBBLE_BOBBLE;
		}
		nomore(MUSE_SCR_RAIN);
		if(obj->otyp == SCR_RAIN) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_RAIN;
		}
		nomore(MUSE_SCR_FLOODING);
		if(obj->otyp == SCR_FLOODING) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_FLOODING;
		}
		nomore(MUSE_SCR_LOCKOUT);
		if(obj->otyp == SCR_LOCKOUT) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_LOCKOUT;
		}
		nomore(MUSE_SCR_GROWTH);
		if(obj->otyp == SCR_GROWTH) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_GROWTH;
		}
		nomore(MUSE_SCR_ICE);
		if(obj->otyp == SCR_ICE) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_ICE;
		}
		nomore(MUSE_SCR_BAD_EFFECT);
		if(obj->otyp == SCR_BAD_EFFECT) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_BAD_EFFECT;
		}
		nomore(MUSE_SCR_CLOUDS);
		if(obj->otyp == SCR_CLOUDS) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_CLOUDS;
		}
		nomore(MUSE_SCR_BARRHING);
		if(obj->otyp == SCR_BARRHING) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_BARRHING;
		}
		nomore(MUSE_SCR_CHAOS_TERRAIN);
		if(obj->otyp == SCR_CHAOS_TERRAIN) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_CHAOS_TERRAIN;
		}
		nomore(MUSE_WAN_TRAP_CREATION);
		if(obj->otyp == WAN_TRAP_CREATION && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_TRAP_CREATION;
		}
		nomore(MUSE_WAN_SUMMON_SEXY_GIRL);
		if(obj->otyp == WAN_SUMMON_SEXY_GIRL && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_SUMMON_SEXY_GIRL;
		}
		nomore(MUSE_WAN_BAD_EFFECT);
		if(obj->otyp == WAN_BAD_EFFECT && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_BAD_EFFECT;
		}
		/* we can safely put this scroll here since the locations that
		 * are in a 1 square radius are a subset of the locations that
		 * are in wand range
		 */
		nomore(MUSE_SCR_EARTH);
		if (obj->otyp == SCR_EARTH
		       && ((helmet && is_metallic(helmet)) ||
				mtmp->mconf || amorphous(mtmp->data) ||
				passes_walls(mtmp->data) || (mtmp->egotype_wallwalk) ||
				noncorporeal(mtmp->data) ||
				unsolid(mtmp->data) || !rn2(10))
		       && dist2(mtmp->mx,mtmp->my,mtmp->mux,mtmp->muy) <= 2
		       && mtmp->mcansee && haseyes(mtmp->data)
#ifdef REINCARNATION
		       && !Is_rogue_level(&u.uz)
#endif
		       && (!In_endgame(&u.uz) || Is_earthlevel(&u.uz))) {
		    m.offensive = obj;
		    m.has_offense = MUSE_SCR_EARTH;
		}
		nomore(MUSE_WAN_CANCELLATION);
		if (obj->otyp == WAN_CANCELLATION && obj->spe > 0) {
			m.offensive   = obj;
			m.has_offense = MUSE_WAN_CANCELLATION;
		}
		nomore(MUSE_SCR_FIRE); /* screw it, monsters will just ignore the fire. --Amy */
		if (obj->otyp == SCR_FIRE/* && resists_fire(mtmp)*/
		   && dist2(mtmp->mx,mtmp->my,mtmp->mux,mtmp->muy) <= 2
		   && mtmp->mcansee && haseyes(mtmp->data)) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_FIRE;
		}
		nomore(MUSE_SCR_DESTROY_ARMOR);
		if(obj->otyp == SCR_DESTROY_ARMOR) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_DESTROY_ARMOR;
		}
		nomore(MUSE_SCR_RAGNAROK);
		if(obj->otyp == SCR_RAGNAROK) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_RAGNAROK;
		}
		nomore(MUSE_SCR_OFFLEVEL_ITEM);
		if(obj->otyp == SCR_OFFLEVEL_ITEM) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_OFFLEVEL_ITEM;
		}
		nomore(MUSE_SCR_MEGALOAD);
		if(obj->otyp == SCR_MEGALOAD) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_MEGALOAD;
		}
		nomore(MUSE_SCR_VILENESS);
		if(obj->otyp == SCR_VILENESS) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_VILENESS;
		}
		nomore(MUSE_SCR_HYBRIDIZATION);
		if(obj->otyp == SCR_HYBRIDIZATION) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_HYBRIDIZATION;
		}
		nomore(MUSE_SCR_NASTY_CURSE);
		if(obj->otyp == SCR_NASTY_CURSE) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_NASTY_CURSE;
		}
		nomore(MUSE_SCR_BAD_EQUIPMENT);
		if(obj->otyp == SCR_BAD_EQUIPMENT) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_BAD_EQUIPMENT;
		}
		nomore(MUSE_SCR_ANTIMATTER);
		if(obj->otyp == SCR_ANTIMATTER) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_ANTIMATTER;
		}
		nomore(MUSE_SCR_RUMOR);
		if(obj->otyp == SCR_RUMOR) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_RUMOR;
		}
		nomore(MUSE_SCR_MESSAGE);
		if(obj->otyp == SCR_MESSAGE) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_MESSAGE;
		}
		nomore(MUSE_SCR_ILLUSION);
		if(obj->otyp == SCR_ILLUSION) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_ILLUSION;
		}
		nomore(MUSE_SCR_VISIBLE_ITEM);
		if(obj->otyp == SCR_VISIBLE_ITEM) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_VISIBLE_ITEM;
		}
		nomore(MUSE_SCR_FEMINISM);
		if(obj->otyp == SCR_FEMINISM) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_FEMINISM;
		}
		nomore(MUSE_SCR_EVIL_VARIANT);
		if(obj->otyp == SCR_EVIL_VARIANT) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_EVIL_VARIANT;
		}
		nomore(MUSE_SCR_SIN);
		if(obj->otyp == SCR_SIN) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_SIN;
		}
		nomore(MUSE_SCR_IMMOBILITY);
		if(obj->otyp == SCR_IMMOBILITY) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_IMMOBILITY;
		}
		nomore(MUSE_SCR_EGOISM);
		if(obj->otyp == SCR_EGOISM) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_EGOISM;
		}
		nomore(MUSE_SCR_ENRAGE);
		if(obj->otyp == SCR_ENRAGE) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_ENRAGE;
		}
		nomore(MUSE_SCR_DESTROY_WEAPON);
		if(obj->otyp == SCR_DESTROY_WEAPON) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_DESTROY_WEAPON;
		}
		nomore(MUSE_SCR_STONING);
		if(obj->otyp == SCR_STONING && !Stoned) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_STONING;
		}
		nomore(MUSE_SCR_PUNISHMENT);
		if(obj->otyp == SCR_PUNISHMENT) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_PUNISHMENT;
		}
		nomore(MUSE_SCR_AMNESIA);
		if(obj->otyp == SCR_AMNESIA) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_AMNESIA;
		}
		nomore(MUSE_WAN_AMNESIA);
		if(obj->otyp == WAN_AMNESIA && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_AMNESIA;
		}
		nomore(MUSE_WAN_IMMOBILITY);
		if(obj->otyp == WAN_IMMOBILITY && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_IMMOBILITY;
		}
		nomore(MUSE_WAN_INSANITY);
		if(obj->otyp == WAN_INSANITY && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_INSANITY;
		}
		nomore(MUSE_WAN_BAD_EQUIPMENT);
		if(obj->otyp == WAN_BAD_EQUIPMENT && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_BAD_EQUIPMENT;
		}
		nomore(MUSE_WAN_EGOISM);
		if(obj->otyp == WAN_EGOISM && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_EGOISM;
		}
		nomore(MUSE_WAN_CORROSION);
		if(obj->otyp == WAN_CORROSION && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_CORROSION;
		}
		nomore(MUSE_WAN_REMOVE_RESISTANCE);
		if(obj->otyp == WAN_REMOVE_RESISTANCE && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_REMOVE_RESISTANCE;
		}
		nomore(MUSE_WAN_DISENCHANTMENT);
		if(obj->otyp == WAN_DISENCHANTMENT && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_DISENCHANTMENT;
		}
		nomore(MUSE_WAN_CHAOS_TERRAIN);
		if(obj->otyp == WAN_CHAOS_TERRAIN && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_CHAOS_TERRAIN;
		}
		nomore(MUSE_WAN_FLEECY_TERRAIN);
		if(obj->otyp == WAN_FLEECY_TERRAIN && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_FLEECY_TERRAIN;
		}
		nomore(MUSE_WAN_STAT_REDUCTION);
		if(obj->otyp == WAN_STAT_REDUCTION && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_STAT_REDUCTION;
		}
		nomore(MUSE_WAN_CONTAMINATION);
		if(obj->otyp == WAN_CONTAMINATION && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_CONTAMINATION;
		}
		nomore(MUSE_WAN_TREMBLING);
		if(obj->otyp == WAN_TREMBLING && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_TREMBLING;
		}
		nomore(MUSE_WAN_CURSE_ITEMS);
		if(obj->otyp == WAN_CURSE_ITEMS && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_CURSE_ITEMS;
		}
		nomore(MUSE_WAN_LEVITATION);
		if(obj->otyp == WAN_LEVITATION && obj->spe > 0 && !Levitation ) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_LEVITATION;
		}
		nomore(MUSE_WAN_BLEEDING);
		if(obj->otyp == WAN_BLEEDING && obj->spe > 0 && !Levitation ) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_BLEEDING;
		}
		nomore(MUSE_WAN_UNDRESSING);
		if(obj->otyp == WAN_UNDRESSING && obj->spe > 0 && !Levitation ) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_UNDRESSING;
		}
		nomore(MUSE_WAN_BAD_LUCK);
		if(obj->otyp == WAN_BAD_LUCK && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_BAD_LUCK;
		}
		nomore(MUSE_WAN_FUMBLING);
		if(obj->otyp == WAN_FUMBLING && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_FUMBLING;
		}
		nomore(MUSE_WAN_SIN);
		if(obj->otyp == WAN_SIN && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_SIN;
		}
		nomore(MUSE_WAN_DESLEXIFICATION);
		if(obj->otyp == WAN_DESLEXIFICATION && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_DESLEXIFICATION;
		}
		nomore(MUSE_WAN_FINGER_BENDING);
		if(obj->otyp == WAN_FINGER_BENDING && obj->spe > 0 && !IsGlib) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_FINGER_BENDING;
		}
		nomore(MUSE_WAN_DRAIN_MANA);
		if(obj->otyp == WAN_DRAIN_MANA && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_DRAIN_MANA;
		}
		nomore(MUSE_WAN_TIDAL_WAVE);
		if(obj->otyp == WAN_TIDAL_WAVE && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_TIDAL_WAVE;
		}
		nomore(MUSE_WAN_STARVATION);
		if(obj->otyp == WAN_STARVATION && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_STARVATION;
		}
		nomore(MUSE_WAN_PUNISHMENT);
		if(obj->otyp == WAN_PUNISHMENT && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_PUNISHMENT;
		}
		nomore(MUSE_WAN_MAKE_VISIBLE);
		if(obj->otyp == WAN_MAKE_VISIBLE && obj->spe > 0 && ((HInvis & INTRINSIC) || (HInvis & TIMEOUT)) ) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_MAKE_VISIBLE;
		}
		nomore(MUSE_WAN_REDUCE_MAX_HITPOINTS);
		if(obj->otyp == WAN_REDUCE_MAX_HITPOINTS && obj->spe > 0 && (u.uhpmax > 1 || (Upolyd && u.mhmax > 1) ) ) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_REDUCE_MAX_HITPOINTS;
		}
		nomore(MUSE_WAN_CONFUSION);
		if(obj->otyp == WAN_CONFUSION && obj->spe > 0 && HConfusion < 11) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_CONFUSION;
		}
		nomore(MUSE_WAN_STUN_MONSTER);
		if(obj->otyp == WAN_STUN_MONSTER && obj->spe > 0 && HStun < 11) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_STUN_MONSTER;
		}
		nomore(MUSE_WAN_SLIMING);
		if(obj->otyp == WAN_SLIMING && obj->spe > 0 && !Slimed) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_SLIMING;
		}
		nomore(MUSE_WAN_LYCANTHROPY);
		if(obj->otyp == WAN_LYCANTHROPY && obj->spe > 0 && !Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC) && !u.ulycn) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_LYCANTHROPY;
		}
		nomore(MUSE_SCR_WOUNDS);
		if(obj->otyp == SCR_WOUNDS) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_WOUNDS;
		}
		nomore(MUSE_SCR_BULLSHIT);
		if(obj->otyp == SCR_BULLSHIT) {
			m.offensive = obj;
			m.has_offense = MUSE_SCR_BULLSHIT;
		}
	}
	return((boolean)(!!m.has_offense));
#undef nomore
}

STATIC_PTR
int
mbhitm(mtmp, otmp)
register struct monst *mtmp;
register struct obj *otmp;
{
	int tmp;

	boolean reveal_invis = FALSE;
	if (mtmp != &youmonst) {
		mtmp->msleeping = 0;
		if (mtmp->m_ap_type) seemimic(mtmp);
	}
	switch(otmp->otyp) {
	case WAN_STRIKING:
		reveal_invis = TRUE;
		if (mtmp == &youmonst) {
			if (zap_oseen) makeknown(WAN_STRIKING);
			if (Antimagic && !Race_if(PM_KUTAR) && rn2(StrongAntimagic ? 20 : 5) ) {
			    shieldeff(u.ux, u.uy);
			    pline("Boing!");
			} else if (rnd(20) < 10 + u.uac || !rn2(3) ) { /* good ac will no longer be 100% protection --Amy */
			    pline_The("wand hits you!");
			    tmp = d(2,12);
			    tmp += rnd(monster_difficulty() + 1);
			    if(Half_spell_damage && rn2(2) ) tmp = (tmp+1) / 2;
			    if(StrongHalf_spell_damage && rn2(2) ) tmp = (tmp+1) / 2;
			    losehp(tmp, "wand of striking", KILLED_BY_AN);
			} else pline_The("wand misses you.");
			stop_occupation();
			nomul(0, 0, FALSE);
		} else if (resists_magm(mtmp)) {
			shieldeff(mtmp->mx, mtmp->my);
			pline("Boing!");
		} else if (rnd(20) < 10+find_mac(mtmp)) {
			tmp = d(2,12);
			hit("wand", mtmp, exclam(tmp));
			(void) resist(mtmp, otmp->oclass, tmp, TELL);
			if (cansee(mtmp->mx, mtmp->my) && zap_oseen)
				makeknown(WAN_STRIKING);
		} else {
			miss("wand", mtmp);
			if (cansee(mtmp->mx, mtmp->my) && zap_oseen)
				makeknown(WAN_STRIKING);
		}
		break;
	case WAN_GRAVITY_BEAM:
		reveal_invis = TRUE;
		if (mtmp == &youmonst) {
			if (zap_oseen) makeknown(WAN_GRAVITY_BEAM);
			pline("Gravity warps around you!");
			tmp = d(6,12);
			tmp += rnd( (monster_difficulty() * 2) + 1);
			if(Half_spell_damage && rn2(2) ) tmp = (tmp+1) / 2;
			if(StrongHalf_spell_damage && rn2(2) ) tmp = (tmp+1) / 2;
			losehp(tmp, "wand of gravity beam", KILLED_BY_AN);
			stop_occupation();
			nomul(0, 0, FALSE);
			phase_door(0);
			pushplayer(FALSE);
		} else {
			tmp = d(6,12);
			hit("wand", mtmp, exclam(tmp));
			(void) resist(mtmp, otmp->oclass, tmp, TELL);
			if (cansee(mtmp->mx, mtmp->my) && zap_oseen)
				makeknown(WAN_GRAVITY_BEAM);
		}
		break;
	case WAN_TELEPORTATION:
		if (mtmp == &youmonst) {
			if (zap_oseen) makeknown(WAN_TELEPORTATION);
			tele();
		} else {
			/* for consistency with zap.c, don't identify */
			if (mtmp->ispriest &&
				*in_rooms(mtmp->mx, mtmp->my, TEMPLE)) {
			    if (cansee(mtmp->mx, mtmp->my))
				pline("%s resists the magic!", Monnam(mtmp));
			    mtmp->msleeping = 0;
			    if(mtmp->m_ap_type) seemimic(mtmp);
			} else if (!tele_restrict(mtmp))
			    (void) rloc(mtmp, FALSE);
		}
		break;
	case WAN_BANISHMENT:
		if (zap_oseen) makeknown(WAN_BANISHMENT);

		if (((u.uhave.amulet) && !u.freeplaymode) || CannotTeleport || (u.usteed && mon_has_amulet(u.usteed))) { pline("You shudder for a moment."); (void) safe_teleds_normalterrain(FALSE);  break; }

		if (playerlevelportdisabled()) { 
			pline("Somehow, the banishment beam doesn't do anything.");
			break;
		}

		if (mtmp == &youmonst) {
			u.banishmentbeam = 1;
			nomul(-2, "being banished", FALSE); /* because it's not called until you get another turn... */
		}

		break;
	case WAN_CANCELLATION:
	case SPE_CANCELLATION:
		if (zap_oseen) makeknown(WAN_CANCELLATION);
		(void) cancel_monst(mtmp, otmp, FALSE, TRUE, FALSE);
		break;
	case WAN_PARALYSIS:
		if (zap_oseen) makeknown(WAN_PARALYSIS);

		if (mtmp == &youmonst) {

			if (!Free_action || !rn2(StrongFree_action ? 20 : 5)) {
			    pline("You are frozen in place!");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
			    if (isstunfish) nomul(-rnz(5), "frozen by a wand", TRUE);
			    else nomul(-rnd(5), "frozen by a wand", TRUE);
			    nomovemsg = You_can_move_again;
			    exercise(A_DEX, FALSE);
			} else You("stiffen momentarily.");

		} else if (!(dmgtype(mtmp->data, AD_PLYS))) {

		    if (canseemon(mtmp) ) {
			pline("%s is frozen by the beam.", Monnam(mtmp) );
		    }
		    mtmp->mcanmove = 0;
		    mtmp->mfrozen = rnz(20);
		    mtmp->mstrategy &= ~STRAT_WAITFORU;

		}

		break;

	case WAN_MAKE_VISIBLE:

		if (mtmp == &youmonst) {

			HInvis &= ~INTRINSIC;
			HInvis &= ~TIMEOUT;
			pline("You become more opaque.");
			makeknown(WAN_MAKE_VISIBLE);
			newsym(u.ux, u.uy);

		} else {

			if (mtmp->minvisreal) break;
			int oldinvis = mtmp->minvis;
			char nambuf[BUFSZ];
	
			mtmp->perminvis = 0;
			mtmp->minvis = 0;
			strcpy(nambuf, Monnam(mtmp));
			newsym(mtmp->mx, mtmp->my);		/* make it appear */
			if (oldinvis) {
			    pline("%s becomes visible!", nambuf);
			    makeknown(WAN_MAKE_VISIBLE);
			}

		}

		break;

	case WAN_DISINTEGRATION:
		if (zap_oseen) makeknown(WAN_DISINTEGRATION);

		if (mtmp == &youmonst) {

			if (Disint_resistance && rn2(StrongDisint_resistance ? 1000 : 100) && !(evilfriday && (uarms || uarmc || uarm || uarmu))) {
			    You("are not disintegrated.");
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
				losehp(rnz(100 + level_difficulty()), "zap you died", KILLED_BY);
				break;
			} else {
				u.youaredead = 1;
				killer_format = KILLED_BY_AN;
				killer = "wand of disintegration";
			      done(DIED);
				u.youaredead = 0;
				return 0;
			}

		} else {

			struct obj *otmpS;

		    if (resists_disint(mtmp)) {
	
			pline("%s is unharmed.", Monnam(mtmp) );
	
		    } else if (mtmp->misc_worn_check & W_ARMS) {
			    /* destroy shield; other possessions are safe */
			    otmpS = which_armor(mtmp, W_ARMS);
			    pline("%s %s is disintegrated!", s_suffix(Monnam(mtmp)), distant_name(otmpS, xname));
			    m_useup(mtmp, otmpS);
			    break;
			} else if (mtmp->misc_worn_check & W_ARMC) {
			    /* destroy cloak; other possessions are safe */
			    otmpS = which_armor(mtmp, W_ARMC);
			    pline("%s %s is disintegrated!", s_suffix(Monnam(mtmp)), distant_name(otmpS, xname));
			    m_useup(mtmp, otmpS);
			    break;
			} else if (mtmp->misc_worn_check & W_ARM) {
			    /* destroy suit */
			    otmpS = which_armor(mtmp, W_ARM);
			    pline("%s %s is disintegrated!", s_suffix(Monnam(mtmp)), distant_name(otmpS, xname));
			    m_useup(mtmp, otmpS);
			    break;
			} else if (mtmp->misc_worn_check & W_ARMU) {
			    /* destroy shirt */
			    otmpS = which_armor(mtmp, W_ARMU);
			    pline("%s %s is disintegrated!", s_suffix(Monnam(mtmp)), distant_name(otmpS, xname));
			    m_useup(mtmp, otmpS);
			    break;
			}
		    else {

			struct obj *otmpX, *otmpX2, *m_amulet = mlifesaver(mtmp);

#define oresist_disintegration(obj) \
		(objects[obj->otyp].oc_oprop == DISINT_RES || (obj->oartifact == ART_ROCK_SOLID_SEAT) || \
		 obj_resists(obj, 5, 50) || is_quest_artifact(obj) || \
		 obj == m_amulet)

			pline("%s is disintegrated!", Monnam(mtmp) );

			for (otmpX = mtmp->minvent; otmpX; otmpX = otmpX2) {
			    otmpX2 = otmpX->nobj;
			    if (!oresist_disintegration(otmpX) && !stack_too_big(otmpX) ) {
				if (Has_contents(otmpX)) delete_contents(otmpX);
				obj_extract_self(otmpX);
				obfree(otmpX, (struct obj *)0);
			    }
			}
	
			mondead(mtmp);
	
		    }
		}

		break;

	case WAN_STONING:
		if (zap_oseen) makeknown(WAN_STONING);

		if (mtmp == &youmonst) {

		    if ((!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) &&
			!(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {
			if (!Stoned) {
				if (Hallucination && rn2(10)) pline("Thankfully you are already stoned.");
				else {
					Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
					u.cnd_stoningcount++;
					pline("You start turning to stone!");
					stop_occupation();
				}
			}
			sprintf(killer_buf, "wand of stoning");
			delayed_killer = killer_buf;
		
		    }

		} else {
			if (!munstone(mtmp, FALSE))
			    minstapetrify(mtmp, FALSE);

		}

		break;

	case WAN_DRAINING:	/* KMH */
		tmp = d(2,6);
		if (mtmp == &youmonst) {
			if (Drain_resistance && !rn2(StrongDrain_resistance ? 16 : 4) ) {
				shieldeff(u.ux, u.uy);
				pline("Boing!");
			} else
				losexp("life drainage", FALSE, TRUE);
			if (zap_oseen)
				makeknown(WAN_DRAINING);
			stop_occupation();
			nomul(0, 0, FALSE);
			break;
		} else if (resists_drli(mtmp)) {
			shieldeff(mtmp->mx, mtmp->my);
			break;	/* skip makeknown */
		} else if (!resist(mtmp, otmp->oclass, tmp, NOTELL) &&
				mtmp->mhp > 0) {
			mtmp->mhpmax -= tmp;
			if (mtmp->mhpmax <= 0 || mtmp->m_lev <= 0)
				monkilled(mtmp, "", AD_DRLI);
			else {
				mtmp->m_lev--;
				if (canseemon(mtmp)) {
					pline("%s suddenly seems weaker!", Monnam(mtmp));
				}
			}
		}
		if (cansee(mtmp->mx, mtmp->my) && zap_oseen)
			makeknown(WAN_DRAINING);
		break;
	case WAN_TIME:
		tmp = d(2,6);
		if (mtmp == &youmonst && !(powerfulimplants() && uimplant && uimplant->oartifact == ART_TIMEAGE_OF_REALMS) ) {

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
		if (dmg) losehp(dmg, "the forces of time", KILLED_BY);
			if (zap_oseen) makeknown(WAN_TIME);

			break;
		} else {
			mtmp->mhpmax -= tmp;
			if (mtmp->mhpmax <= 0 || mtmp->m_lev <= 0)
				monkilled(mtmp, "", AD_DRLI);
			else {
				mtmp->m_lev--;
				if (canseemon(mtmp)) {
					pline("%s suddenly seems weaker!", Monnam(mtmp));
				}
			}
		}
		if (cansee(mtmp->mx, mtmp->my) && zap_oseen)
			makeknown(WAN_TIME);
		break;
	case WAN_REDUCE_MAX_HITPOINTS:	/* evil patch idea by jonadab */

	/* he wanted it to be a wand of halve max hitpoints, but that would be too evil even for this game. --Amy */

		if (mtmp == &youmonst) {
			You_feel("drained...");
				if (Upolyd) u.mhmax -= rnd(5);
				else u.uhpmax -= rnd(5);
				if (u.mhmax < 1) u.mhmax = 1;
				if (u.uhpmax < 1) u.uhpmax = 1;
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				if (u.mh > u.mhmax) u.mh = u.mhmax;
			if (zap_oseen)
				makeknown(WAN_REDUCE_MAX_HITPOINTS);
			stop_occupation();
			nomul(0, 0, FALSE);
			break;
		} else {
			mtmp->mhpmax -= rnd(8);
			if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;
			if (mtmp->mhpmax <= 0 || mtmp->m_lev <= 0)
				monkilled(mtmp, "", AD_DRLI);
			else {
				if (canseemon(mtmp)) {
					pline("%s suddenly seems weaker!", Monnam(mtmp));
				}
			}
		}
		if (cansee(mtmp->mx, mtmp->my) && zap_oseen)
			makeknown(WAN_REDUCE_MAX_HITPOINTS);

		break;
	case WAN_FEAR:
		if (mtmp == &youmonst) {
			You("suddenly panic!");
			make_feared(HFeared + rnd(50 + (monster_difficulty() * 5) ),TRUE);
			if (zap_oseen) makeknown(WAN_FEAR);
			break;
		} else {

			if (!is_undead(mtmp->data) && (!mtmp->egotype_undead) &&
			    !resist(mtmp, otmp->oclass, 0, NOTELL) &&
			    (!mtmp->mflee || mtmp->mfleetim)) {
			     if (canseemon(mtmp)) {
				 pline("%s suddenly panics!", Monnam(mtmp));
				 makeknown(WAN_FEAR);
				}
			     monflee(mtmp, rnd(10), FALSE, TRUE);
			}
			break;

		}
		if (cansee(mtmp->mx, mtmp->my) && zap_oseen)
			makeknown(WAN_FEAR);
		break;

	case WAN_BUBBLEBEAM:
		if (mtmp == &youmonst) {
			if (!Swimming && !Amphibious && !Breathless) {
				pline("You're drowned in a stream of water bubbles and can't breathe!");
			      tmp = d(4,12);
			      tmp += rnd(monster_difficulty() + 1);
			      losehp(tmp, "wand of bubblebeam", KILLED_BY_AN);
			}
			if (zap_oseen)
				makeknown(WAN_BUBBLEBEAM);
			break;
		}
		break;

	case WAN_GOOD_NIGHT:
		if (mtmp == &youmonst) {
		    tmp = d(2,12);
		    tmp += rnd(monster_difficulty() + 1);
		    if (u.ualign.type == A_LAWFUL) tmp *= 2;
		    if (u.ualign.type == A_CHAOTIC) tmp /= 2;
		    if (nonliving(youmonst.data)) tmp /= 2;
		    if(Half_spell_damage && rn2(2) ) tmp = (tmp+1) / 2;
		    if(StrongHalf_spell_damage && rn2(2) ) tmp = (tmp+1) / 2;
		    pline("Good night, %s!", playeraliasname);
		    losehp(tmp, "wand of good night", KILLED_BY_AN);
		    make_blinded(Blinded+rnz(100),FALSE);
			if (zap_oseen)
				makeknown(WAN_GOOD_NIGHT);
			break;
		}
		break;

	case WAN_DREAM_EATER:
		if (mtmp == &youmonst) {
			tmp = d(10, 10);
			tmp += rnd( (monster_difficulty() * 4) + 1);
			pline("Your dream is eaten!");
			losehp(tmp, "wand of dream eater", KILLED_BY_AN);
			if (zap_oseen)
				makeknown(WAN_DREAM_EATER);
			break;
		}

		break;

	case WAN_SLOW_MONSTER:
		if (mtmp == &youmonst) {
			u_slow_down();
			if (zap_oseen)
				makeknown(WAN_SLOW_MONSTER);

			/* In FIQhack, monsters with wands of slow monster will 0wn you by making you super slow.
			 * It's largely like the wand of inertia, so have it work like the wand of inertia in the evil variant */
			if (isevilvariant && !inertiaprotection()) {
				u.uprops[DEAC_FAST].intrinsic += (( rnd(10) + rnd(monster_difficulty() + 1) ) * 10);
				pline(u.inertia ? "You feel even slower." : "You become super slow thanks to the wand destruction patch.");
				u.inertia += (rnd(10) + rnd(monster_difficulty() + 1));
			}

			break;
		} else {

			if (!resist(mtmp, otmp->oclass, 0, NOTELL)) {
				mon_adjust_speed(mtmp, -1, otmp);
				m_dowear(mtmp, FALSE); /* might want speed boots */
				if (u.uswallow && (mtmp == u.ustuck) &&
				    is_whirly(mtmp->data)) {
					pline("Something disrupts %s!", mon_nam(mtmp));
					pline("A huge hole opens up...");
					expels(mtmp, mtmp->data, TRUE);
				}
			}
			break;

		}
		if (cansee(mtmp->mx, mtmp->my) && zap_oseen)
			makeknown(WAN_SLOW_MONSTER);
		break;
	case WAN_INERTIA:
		if (mtmp == &youmonst) {

			if (inertiaprotection()) break;

			u_slow_down();
			u.uprops[DEAC_FAST].intrinsic += (( rnd(10) + rnd(monster_difficulty() + 1) ) * 10);
			pline(u.inertia ? "You feel even slower." : "You slow down to a crawl.");
			u.inertia += (rnd(10) + rnd(monster_difficulty() + 1));
			if (zap_oseen)
				makeknown(WAN_INERTIA);
			break;
		} else {

			mon_adjust_speed(mtmp, -1, otmp);
			m_dowear(mtmp, FALSE); /* might want speed boots */
			if (u.uswallow && (mtmp == u.ustuck) &&
			    is_whirly(mtmp->data)) {
				pline("Something disrupts %s!", mon_nam(mtmp));
				pline("A huge hole opens up...");
				expels(mtmp, mtmp->data, TRUE);
			}
			break;

		}
		if (cansee(mtmp->mx, mtmp->my) && zap_oseen)
			makeknown(WAN_INERTIA);
		break;
	}


	if (reveal_invis) {
	    if (mtmp->mhp > 0 && cansee(bhitpos.x,bhitpos.y)
							&& !canspotmon(mtmp) && !(mtmp->data->msound == MS_DEEPSTATE) && !(mtmp->egotype_deepstatemember))
		map_invisible(bhitpos.x, bhitpos.y);
	}
	return 0;
}

/* A modified bhit() for monsters.  Based on bhit() in zap.c.  Unlike
 * buzz(), bhit() doesn't take into account the possibility of a monster
 * zapping you, so we need a special function for it.  (Unless someone wants
 * to merge the two functions...)
 */
STATIC_OVL void
mbhit(mon,range,fhitm,fhito,obj)
struct monst *mon;			/* monster shooting the wand */
register int range;			/* direction and range */
int (*fhitm)(MONST_P,OBJ_P);
int (*fhito)(OBJ_P,OBJ_P);	/* fns called when mon/obj hit */
struct obj *obj;			/* 2nd arg to fhitm/fhito */
{
	register struct monst *mtmp;
	register struct obj *otmp;
	register uchar typ;
	int ddx, ddy;

	bhitpos.x = mon->mx;
	bhitpos.y = mon->my;
	u.dx = ddx = sgn(mon->mux - mon->mx);
	u.dy = ddy = sgn(mon->muy - mon->my);

	while(range-- > 0) {
		int x,y;

		bhitpos.x += ddx;
		bhitpos.y += ddy;
		x = bhitpos.x; y = bhitpos.y;

		if (!isok(x,y)) {
		    bhitpos.x -= ddx;
		    bhitpos.y -= ddy;
		    break;
		}

		if (obj->otyp == WAN_BUBBLEBEAM && !rn2(10) && (levl[x][y].typ == ROOM || levl[x][y].typ == CORR) ) {
			levl[x][y].typ = POOL;
			/* no minliquid or drowning the player because that would be too evil --Amy */
		}

		if (obj->otyp == WAN_GOOD_NIGHT) {
			levl[x][y].lit = 0;
		}

		if (find_drawbridge(&x,&y))
		    switch (obj->otyp) {
			case WAN_STRIKING:
			case WAN_GRAVITY_BEAM:
			    destroy_drawbridge(x,y);
		    }
		if(bhitpos.x==u.ux && bhitpos.y==u.uy) {
			(*fhitm)(&youmonst, obj);
			range -= 3;
		} else if(MON_AT(bhitpos.x, bhitpos.y)){
			mtmp = m_at(bhitpos.x,bhitpos.y);
			if (cansee(bhitpos.x,bhitpos.y) && !canspotmon(mtmp) && !(mtmp->data->msound == MS_DEEPSTATE) && !(mtmp->egotype_deepstatemember))
			    map_invisible(bhitpos.x, bhitpos.y);
			(*fhitm)(mtmp, obj);
			range -= 3;
		}
		/* modified by GAN to hit all objects */
		if(fhito){
		    int hitanything = 0;
		    register struct obj *next_obj;

		    for(otmp = level.objects[bhitpos.x][bhitpos.y];
							otmp; otmp = next_obj) {
			/* Fix for polymorph bug, Tim Wright */
			next_obj = otmp->nexthere;
			hitanything += (*fhito)(otmp, obj);
		    }
		    if(hitanything)	range--;
		}
		typ = levl[bhitpos.x][bhitpos.y].typ;
		if(IS_DOOR(typ) || typ == SDOOR) {
		    switch (obj->otyp) {
			/* note: monsters don't use opening or locking magic
			   at present, but keep these as placeholders */
			case WAN_OPENING:
			case WAN_LOCKING:
			case WAN_STRIKING:
			case WAN_GRAVITY_BEAM:
			    if (doorlock(obj, bhitpos.x, bhitpos.y)) {
				makeknown(obj->otyp);
				/* if a shop door gets broken, add it to
				   the shk's fix list (no cost to player) */
				if (levl[bhitpos.x][bhitpos.y].doormask ==
					D_BROKEN &&
				    *in_rooms(bhitpos.x, bhitpos.y, SHOPBASE))
				    add_damage(bhitpos.x, bhitpos.y, 0L);
			    }
			    break;
		    }
		}
		if(!ZAP_POS(typ) || (IS_DOOR(typ) &&
		   (levl[bhitpos.x][bhitpos.y].doormask & (D_LOCKED | D_CLOSED)))
		  ) {
			bhitpos.x -= ddx;
			bhitpos.y -= ddy;
			break;
		}
	}
}

/* Perform an offensive action for a monster.  Must be called immediately
 * after find_offensive().  Return values are same as use_defensive().
 */
int
use_offensive(mtmp)
struct monst *mtmp;
{
	int i;
	struct obj *otmp = m.offensive;
	struct obj *otmp2;
	boolean oseen;

	/* offensive potions are not drunk, they're thrown */
	if (otmp->oclass != POTION_CLASS && (i = precheck(mtmp, otmp)) != 0)
		return i;
	oseen = otmp && canseemon(mtmp);

	switch(m.has_offense) {
	case MUSE_WAN_DEATH:
	case MUSE_WAN_SLEEP:
/*WAC reactivate*/
	case MUSE_WAN_FIREBALL:
	case MUSE_WAN_ACID:
	case MUSE_WAN_SOLAR_BEAM:
	case MUSE_WAN_PSYBEAM:
	case MUSE_WAN_FIRE:
	case MUSE_WAN_COLD:
	case MUSE_WAN_LIGHTNING:
	case MUSE_WAN_MAGIC_MISSILE:
		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		if (oseen) makeknown(otmp->otyp);
		m_using = TRUE;
/*WAC Handled later
		if (otmp->otyp == WAN_FIREBALL) {
		    buzz((int) -10 - SPE_FIREBALL - SPE_MAGIC_MISSILE, rn2(2)+3 + (rnd(monster_difficulty()) / 5) ,
		    mtmp->mx, mtmp->my,
		    sgn(mtmp->mux-mtmp->mx), sgn(mtmp->muy-mtmp->my));
		}
		else {
*/

		if (otmp->oartifact == ART_DRAINING_ARROW) {
			if (u.uhpmax > 1) {
				u.uhpmax--;
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			}
			if (Upolyd && u.mhmax > 1) {
				u.mhmax--;
				if (u.mh > u.mhmax) u.mh = u.mhmax;
			}
			Your("health has been drained!");
		}

		if (isevilvariant) { /* wand destruction patch, YAY - reflection does jack diddly because of course it does */

			/* monsters have no wand skill in SLEX, so they just generally pierce reflection
			 * in Nethack Fourk they'd have to be at least skilled, but this is the Evil Variant(TM) --Amy */

			switch (m.has_offense) {
				default: break;
				case MUSE_WAN_FIRE:
				case MUSE_WAN_FIREBALL:
					burnarmor(&youmonst);
					destroy_item(POTION_CLASS, AD_FIRE);
					destroy_item(SCROLL_CLASS, AD_FIRE);
					destroy_item(SPBOOK_CLASS, AD_FIRE);
					break;
				case MUSE_WAN_COLD:
					destroy_item(POTION_CLASS, AD_COLD);
					break;
				case MUSE_WAN_LIGHTNING:
					destroy_item(WAND_CLASS, AD_ELEC);
					destroy_item(RING_CLASS, AD_ELEC);
					destroy_item(AMULET_CLASS, AD_ELEC);
					break;
			}
		}

		/* Monsters zapping wands will be more dangerous later in the game. --Amy */
		buzz((int)(-30 - (otmp->otyp - WAN_MAGIC_MISSILE)),
			(otmp->otyp == WAN_MAGIC_MISSILE) ? 2 + (rnd(monster_difficulty()) / 10) : (otmp->otyp == WAN_SOLAR_BEAM) ? 8 + (rnd(monster_difficulty()) / 4) : (otmp->otyp == WAN_PSYBEAM) ? 7 + (rnd(monster_difficulty()) / 5) : 6 + (rnd(monster_difficulty()) / 8),
			mtmp->mx, mtmp->my,
			sgn(mtmp->mux-mtmp->mx), sgn(mtmp->muy-mtmp->my));
		m_using = FALSE;
/*                }*/

		if (mtmp->mhp > 0) { /* cutting down on annoying segfaults --Amy */
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		}
		return (mtmp->mhp <= 0) ? 1 : 2;
	case MUSE_WAN_POISON:
		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		if (oseen) makeknown(otmp->otyp);
		m_using = TRUE;

		buzz((int)(-26), 7 + (rnd(monster_difficulty()) / 6),
			mtmp->mx, mtmp->my,
			sgn(mtmp->mux-mtmp->mx), sgn(mtmp->muy-mtmp->my));
		m_using = FALSE;
		if (mtmp->mhp > 0) { /* cutting down on annoying segfaults --Amy */
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		}

		return (mtmp->mhp <= 0) ? 1 : 2;

	case MUSE_WAN_INFERNO:
		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		if (oseen) makeknown(otmp->otyp);
		m_using = TRUE;
		if (!rn2(3)) u.uprops[DEAC_REFLECTING].intrinsic += rnd(5);

		if (isevilvariant) {
			burnarmor(&youmonst);
			destroy_item(POTION_CLASS, AD_FIRE);
			destroy_item(SCROLL_CLASS, AD_FIRE);
			destroy_item(SPBOOK_CLASS, AD_FIRE);
		}

		pline("Your %s is singed by searing flames!", body_part(FACE));
		if (Upolyd && u.mhmax > 1) {
			u.mhmax--;
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		}
		else if (!Upolyd && u.uhpmax > 1) {
			u.uhpmax--;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		}
		make_blinded(Blinded+rnz(100),FALSE);

		buzz((int)(-21), 12 + (rnd(monster_difficulty()) / 4),
			mtmp->mx, mtmp->my,
			sgn(mtmp->mux-mtmp->mx), sgn(mtmp->muy-mtmp->my));
		m_using = FALSE;
		if (mtmp->mhp > 0) { /* cutting down on annoying segfaults --Amy */
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		}

		return (mtmp->mhp <= 0) ? 1 : 2;

	case MUSE_WAN_ICE_BEAM:
		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		if (oseen) makeknown(otmp->otyp);
		m_using = TRUE;
		if (!rn2(3)) u.uprops[DEAC_REFLECTING].intrinsic += rnd(5);

		if (isevilvariant) {
			destroy_item(POTION_CLASS, AD_COLD);
		}

		u_slow_down();

		buzz((int)(-22), 12 + (rnd(monster_difficulty()) / 4),
			mtmp->mx, mtmp->my,
			sgn(mtmp->mux-mtmp->mx), sgn(mtmp->muy-mtmp->my));
		m_using = FALSE;
		if (mtmp->mhp > 0) { /* cutting down on annoying segfaults --Amy */
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		}

		return (mtmp->mhp <= 0) ? 1 : 2;

	case MUSE_WAN_THUNDER:
		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		if (oseen) makeknown(otmp->otyp);
		m_using = TRUE;
		if (!rn2(3)) u.uprops[DEAC_REFLECTING].intrinsic += rnd(5);

		if (isevilvariant) {
			destroy_item(WAND_CLASS, AD_ELEC);
			destroy_item(RING_CLASS, AD_ELEC);
			destroy_item(AMULET_CLASS, AD_ELEC);
		}

		if (!rn2(3) && multi >= 0) {
			if (isstunfish) nomul(-rnz(3), "paralyzed by thunder", TRUE);
			else nomul(-rnd(3), "paralyzed by thunder", TRUE);
		}
		if (!rn2(2)) make_numbed(HNumbed + rnz(150), TRUE);

		buzz((int)(-25), 12 + (rnd(monster_difficulty()) / 4),
			mtmp->mx, mtmp->my,
			sgn(mtmp->mux-mtmp->mx), sgn(mtmp->muy-mtmp->my));
		m_using = FALSE;
		if (mtmp->mhp > 0) { /* cutting down on annoying segfaults --Amy */
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		}

		return (mtmp->mhp <= 0) ? 1 : 2;

	case MUSE_WAN_SLUDGE:
		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		if (oseen) makeknown(otmp->otyp);
		m_using = TRUE;
		if (!rn2(3)) u.uprops[DEAC_REFLECTING].intrinsic += rnd(5);

		{
		    register struct obj *objX, *objX2;
		    for (objX = invent; objX; objX = objX2) {
		      objX2 = objX->nobj;
			if (!rn2(5)) rust_dmg(objX, xname(objX), 3, TRUE, &youmonst);
		    }
		}

		buzz((int)(-27), 12 + (rnd(monster_difficulty()) / 4),
			mtmp->mx, mtmp->my,
			sgn(mtmp->mux-mtmp->mx), sgn(mtmp->muy-mtmp->my));
		m_using = FALSE;
		if (mtmp->mhp > 0) { /* cutting down on annoying segfaults --Amy */
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		}

		return (mtmp->mhp <= 0) ? 1 : 2;

	case MUSE_WAN_TOXIC:
		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		if (oseen) makeknown(otmp->otyp);
		m_using = TRUE;
		if (!rn2(3)) u.uprops[DEAC_REFLECTING].intrinsic += rnd(5);

		if (!Poison_resistance) pline("You're badly poisoned!");
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_STR, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_DEX, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_CON, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_INT, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_WIS, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_CHA, -rnd(2), FALSE, TRUE);

		buzz((int)(-26), 12 + (rnd(monster_difficulty()) / 4),
			mtmp->mx, mtmp->my,
			sgn(mtmp->mux-mtmp->mx), sgn(mtmp->muy-mtmp->my));
		m_using = FALSE;
		if (mtmp->mhp > 0) { /* cutting down on annoying segfaults --Amy */
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		}

		return (mtmp->mhp <= 0) ? 1 : 2;

	case MUSE_WAN_NETHER_BEAM:
		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		if (oseen) makeknown(otmp->otyp);
		m_using = TRUE;
		if (!rn2(3)) u.uprops[DEAC_REFLECTING].intrinsic += rnd(5);

		if (Upolyd && u.mh > 1) u.mh /= 2;
		else if (!Upolyd && u.uhp > 1) u.uhp /= 2;
		losehp(1, "nether beam", KILLED_BY_AN);

		buzz((int)(-29), 12 + (rnd(monster_difficulty()) / 4),
			mtmp->mx, mtmp->my,
			sgn(mtmp->mux-mtmp->mx), sgn(mtmp->muy-mtmp->my));
		m_using = FALSE;
		if (mtmp->mhp > 0) { /* cutting down on annoying segfaults --Amy */
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		}

		return (mtmp->mhp <= 0) ? 1 : 2;

	case MUSE_WAN_AURORA_BEAM:
		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		if (oseen) makeknown(otmp->otyp);
		m_using = TRUE;
		if (!rn2(3)) u.uprops[DEAC_REFLECTING].intrinsic += rnd(5);

	      (void) cancel_monst(&youmonst, otmp, FALSE, TRUE, FALSE);

		buzz((int)(-28), 16 + (rnd(monster_difficulty()) / 3),
			mtmp->mx, mtmp->my,
			sgn(mtmp->mux-mtmp->mx), sgn(mtmp->muy-mtmp->my));
		m_using = FALSE;
		if (mtmp->mhp > 0) { /* cutting down on annoying segfaults --Amy */
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		}

		return (mtmp->mhp <= 0) ? 1 : 2;

	case MUSE_WAN_CHLOROFORM:
		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		if (oseen) makeknown(otmp->otyp);
		m_using = TRUE;
		if (!rn2(3)) u.uprops[DEAC_REFLECTING].intrinsic += rnd(5);

		losehp(rnd(monster_difficulty() + 2), "chloroform", KILLED_BY);

		buzz((int)(-23), 8 + (rnd(monster_difficulty()) / 6),
			mtmp->mx, mtmp->my,
			sgn(mtmp->mux-mtmp->mx), sgn(mtmp->muy-mtmp->my));
		m_using = FALSE;
		if (mtmp->mhp > 0) { /* cutting down on annoying segfaults --Amy */
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		}

		return (mtmp->mhp <= 0) ? 1 : 2;

	case MUSE_WAN_HYPER_BEAM:
		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		if (oseen) makeknown(otmp->otyp);
		m_using = TRUE;
		if (!rn2(3)) u.uprops[DEAC_REFLECTING].intrinsic += rnd(5);

		buzz((int)(-20), 6 + (rnd(monster_difficulty()) / 3),
			mtmp->mx, mtmp->my,
			sgn(mtmp->mux-mtmp->mx), sgn(mtmp->muy-mtmp->my));
		m_using = FALSE;
		if (mtmp->mhp > 0) { /* cutting down on annoying segfaults --Amy */
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		}

		return (mtmp->mhp <= 0) ? 1 : 2;

	case MUSE_WAN_CHROMATIC_BEAM:
		{
		int damagetype = -(20 + rn2(8));
		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		if (oseen) makeknown(otmp->otyp);
		m_using = TRUE;

		buzz((int)(damagetype), damagetype == -26 ? 7 + (rnd(monster_difficulty()) / 6) : damagetype == -20 ? 2 + (rnd(monster_difficulty()) / 10) : damagetype == -28 ? 8 + (rnd(monster_difficulty()) / 4) : 6 + (rnd(monster_difficulty()) / 8),
			mtmp->mx, mtmp->my,
			sgn(mtmp->mux-mtmp->mx), sgn(mtmp->muy-mtmp->my));
		m_using = FALSE;
		if (mtmp->mhp > 0) { /* cutting down on annoying segfaults --Amy */
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		}

		return (mtmp->mhp <= 0) ? 1 : 2;
		}

	case MUSE_WAN_DISINTEGRATION_BEAM:
		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		if (oseen) makeknown(otmp->otyp);
		m_using = TRUE;

		buzz((int)(-24), 7 + (rnd(monster_difficulty()) / 6),
			mtmp->mx, mtmp->my,
			sgn(mtmp->mux-mtmp->mx), sgn(mtmp->muy-mtmp->my));
		m_using = FALSE;
		if (mtmp->mhp > 0) { /* cutting down on annoying segfaults --Amy */
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		}

		return (mtmp->mhp <= 0) ? 1 : 2;

	case MUSE_FIRE_HORN:
	case MUSE_FROST_HORN:
	case MUSE_TEMPEST_HORN:
	case MUSE_ETHER_HORN:
	case MUSE_SHADOW_HORN:
	case MUSE_CHROME_HORN:
	case MUSE_DEATH_HORN:
		if (oseen) {
			makeknown(otmp->otyp);
			pline("%s plays a %s!", Monnam(mtmp), xname(otmp));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Strela boli prikhodit vash put'. I yesli vy khotite ispol'zovat' rog dlya sebya, teper' on imeyet odin men'she ostavshegosya zaryada." : "Taet-taetaeaeae!");
		} else {
			You_hear("a horn being played.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Strela boli prikhodit vash put'. I yesli vy khotite ispol'zovat' rog dlya sebya, teper' on imeyet odin men'she ostavshegosya zaryada." : "Taet-taetaeaeae!");
		}
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		m_using = TRUE;

		if (isevilvariant) {
			switch (m.has_offense) {
				default: break;
				case MUSE_FIRE_HORN:
					burnarmor(&youmonst);
					destroy_item(POTION_CLASS, AD_FIRE);
					destroy_item(SCROLL_CLASS, AD_FIRE);
					destroy_item(SPBOOK_CLASS, AD_FIRE);
					break;
				case MUSE_FROST_HORN:
					destroy_item(POTION_CLASS, AD_COLD);
					break;
				case MUSE_TEMPEST_HORN:
					destroy_item(WAND_CLASS, AD_ELEC);
					destroy_item(RING_CLASS, AD_ELEC);
					destroy_item(AMULET_CLASS, AD_ELEC);
					break;
			}
		}

		if (otmp->otyp == CHROME_HORN)
			buzz(-26, rn1(6,6), mtmp->mx, mtmp->my, sgn(mtmp->mux-mtmp->mx), sgn(mtmp->muy-mtmp->my));
		else if (otmp->otyp == DEATH_HORN)
			buzz(-24, rn1(6,6), mtmp->mx, mtmp->my, sgn(mtmp->mux-mtmp->mx), sgn(mtmp->muy-mtmp->my));

		else
			buzz(-30 - ((otmp->otyp==FROST_HORN) ? AD_COLD-1 : (otmp->otyp==TEMPEST_HORN) ? AD_ELEC-1 : (otmp->otyp==SHADOW_HORN) ? AD_ACID-1 : (otmp->otyp==ETHER_HORN) ? AD_MAGM-1 : (otmp->otyp==CHROME_HORN) ? AD_DRST-1 : (otmp->otyp==DEATH_HORN) ? AD_DISN-1 : AD_FIRE-1),
			rn1(6,6), mtmp->mx, mtmp->my,
			sgn(mtmp->mux-mtmp->mx), sgn(mtmp->muy-mtmp->my));
		m_using = FALSE;
		if (mtmp->mhp > 0) { /* cutting down on annoying segfaults --Amy */
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		}
		return (mtmp->mhp <= 0) ? 1 : 2;
/*      case MUSE_WAN_TELEPORTATION:*/
	case MUSE_WAN_STRIKING:
	case MUSE_WAN_GRAVITY_BEAM:
	case MUSE_WAN_BUBBLEBEAM:
	case MUSE_WAN_DREAM_EATER:
	case MUSE_WAN_GOOD_NIGHT:
	case MUSE_WAN_SLOW_MONSTER:
	case MUSE_WAN_INERTIA:
	case MUSE_WAN_FEAR:
	case MUSE_WAN_DRAINING:	/* KMH */
	case MUSE_WAN_TIME:
	case MUSE_WAN_CANCELLATION:  /* Lethe */
	case MUSE_WAN_STONING:
	case MUSE_WAN_REDUCE_MAX_HITPOINTS:
	case MUSE_WAN_PARALYSIS:
	case MUSE_WAN_MAKE_VISIBLE:
	case MUSE_WAN_DISINTEGRATION:
		zap_oseen = oseen;
		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		m_using = TRUE;
		mbhit(mtmp, EnglandMode ? rn1(10,10) : rn1(8,6),mbhitm,bhito,otmp);
		m_using = FALSE;
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		stop_occupation(); /* thanks FIQ - no idea why this wasn't interrupting you */
		return 2;
	case MUSE_WAN_BANISHMENT:
		zap_oseen = oseen;
		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(10) || !otmp->oartifact)) otmp->spe--;
		mbhitm(&youmonst,otmp);
		/*if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);*/ /* This was crashing too often. --Amy */
		return 2;
	case MUSE_SCR_ELEMENTALISM:
	    {
		coord cc;
		int cnt = rno(9);
		if (mtmp->mconf) cnt += rno(12);
		if (otmp->cursed) cnt += rno(5);

		if (!enexto(&cc, mtmp->mx, mtmp->my, 0)) break;

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		while(cnt--) {
			if (!enexto(&cc, mtmp->mx, mtmp->my, 0)) break;

			makemon(mkclass(S_ELEMENTAL,0), cc.x, cc.y, NO_MM_FLAGS);
		}
	    }
		pline("The inhabitants of the elemental planes appear!");

		u.aggravation = 0;

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_DEMONOLOGY:
	    {
		coord cc;
		int cnt = rno(9);
		if (mtmp->mconf) cnt += rno(12);
		if (otmp->cursed) cnt += rno(5);

		if (!enexto(&cc, mtmp->mx, mtmp->my, 0)) break;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		while(cnt--) {
			if (!enexto(&cc, mtmp->mx, mtmp->my, 0)) break;

			makemon(mkclass(S_DEMON,0), cc.x, cc.y, NO_MM_FLAGS);
		}
	    }
		pline("The denizens of Gehennom appear!");

		u.aggravation = 0;

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_GIRLINESS:
		mreadmsg(mtmp, otmp);

	    {
		int cnt = rno(9);
		if (otmp->cursed) cnt += rno(18);
		if (mtmp->mconf) cnt += rno(100);
		while(cnt--) {
			makegirlytrap();
		}
	    }

		if (!objects[SCR_GIRLINESS].oc_name_known
			&& !objects[SCR_GIRLINESS].oc_uname)
		    docall(otmp);

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_NASTINESS:
		mreadmsg(mtmp, otmp);

		{

		int nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
		if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
		int blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
		if (!blackngdur ) blackngdur = 500; /* fail safe */

		if (!rn2(100)) pline("You have a bad feeling in your %s.",body_part(STOMACH) );

		randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), (blackngdur - (monster_difficulty() * 3)));
		}

		if (!objects[SCR_NASTINESS].oc_name_known
			&& !objects[SCR_NASTINESS].oc_uname)
		    docall(otmp);

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_TRAP_CREATION:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);
	      You_feel("endangered!!");
		{
			int rtrap;
		    int i, j, bd;
			bd = 1;
			if (!rn2(5)) bd += rnz(1);
			boolean canbeinawall = FALSE;
			if (!rn2(Passes_walls ? 5 : 25)) canbeinawall = TRUE;

		      for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
				if (!isok(u.ux + i, u.uy + j)) continue;
				if ((levl[u.ux + i][u.uy + j].typ <= DBWALL) && !canbeinawall) continue;
				if (t_at(u.ux + i, u.uy + j)) continue;

			      rtrap = rndtrap();

				(void) maketrap(u.ux + i, u.uy + j, rtrap, 100, TRUE);
			}
		}
		makerandomtrap(TRUE);
		if (!rn2(2)) makerandomtrap(TRUE);
		if (!rn2(4)) makerandomtrap(TRUE);
		if (!rn2(8)) makerandomtrap(TRUE);
		if (!rn2(16)) makerandomtrap(TRUE);
		if (!rn2(32)) makerandomtrap(TRUE);
		if (!rn2(64)) makerandomtrap(TRUE);
		if (!rn2(128)) makerandomtrap(TRUE);
		if (!rn2(256)) makerandomtrap(TRUE);

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_CREATE_TRAP:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		if (otmp->oartifact == ART_FUFUFUFUFU__KEKEKEKEKE___) {
			int newtraps = 20;
			while (newtraps > 0) {
				newtraps--;
				makerandomtrap(TRUE);
			}
		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		/* don't trigger traps that might send the player to a different level due to danger of segfaults --Amy */

		{
		struct trap *ttmp2 = maketrap(u.ux, u.uy, rndtrap(), 100, TRUE);
		if (ttmp2 && (ttmp2->ttyp != HOLE) && (ttmp2->ttyp != TRAPDOOR) && (ttmp2->ttyp != LEVEL_TELEP) && (ttmp2->ttyp != LEVEL_BEAMER) && (ttmp2->ttyp != BRANCH_TELEPORTER) && (ttmp2->ttyp != BRANCH_BEAMER) && (ttmp2->ttyp != WRONG_STAIRS) && (ttmp2->ttyp != NEXUS_TRAP) && (ttmp2->ttyp != MAGIC_PORTAL) && (ttmp2->ttyp != UNKNOWN_TRAP) && (ttmp2->ttyp != WARP_ZONE) && (ttmp2->ttyp != SHAFT_TRAP) && (ttmp2->ttyp != CURRENT_SHAFT) ) dotrap(ttmp2, 0);
		}

		return 2;

	case MUSE_SCR_BULLSHIT:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);
		pline("You notice a vile stench...");

		    int i, j, bd = mtmp->mconf ? 100 : otmp->cursed ? 2 : 1;
		    for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
				if (!isok(u.ux + i, u.uy + j)) continue;
				if (levl[u.ux + i][u.uy + j].typ <= DBWALL) continue;
				if (t_at(u.ux + i, u.uy + j)) continue;
			maketrap(u.ux + i, u.uy + j, rn2(5) ? SHIT_TRAP : SHIT_PIT, 0, TRUE);
		    }

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_FLOOD:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5;
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radius, do_floodd, (void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* "scroll of bullshit" (term created by Khor) - let's be nice, and reduce the chance of screwing up the player --Amy */
			if ((safe_pos > 0) && !rn2(20 + Luck) )
				do_floodd(u.ux, u.uy, (void *)&stilldry);
			if (madepool)
				pline(FunnyHallu ?
						"A totally gnarly wave comes in!" :
						"A flood surges through the area!" );
			if (!stilldry && !Wwalking && !Race_if(PM_KORONST) && !Flying && !Levitation)
				drown();

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_DESTROY_ARMOR:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		otmp2 = some_armor(&youmonst);
		if (otmp2 && otmp2->blessed && rn2(5)) pline("Your body shakes violently!");
		/* extra saving throw for highly enchanted armors --Amy */
		else if (otmp2 && (otmp2->spe > 1) && (rn2(otmp2->spe)) ) pline("Your body shakes violently!");
		/* being magic resistant also offers protection */
		else if (Antimagic && rn2(StrongAntimagic ? 5 : 3)) pline("Your body shakes violently!");
		/* artifacts are highly resistant */
		else if (otmp2 && otmp2->oartifact && rn2(20)) pline("Your body shakes violently!");
		/* and grease will always offer protection but can wear off */
		else if (otmp2 && otmp2->greased) {
			pline("Your body shakes violently!");
			 if (!rn2(2) || (isfriday && !rn2(2))) {
				pline_The("grease wears off.");
				otmp2->greased -= 1;
				update_inventory();
			 }
		}

		else if (!otmp2) pline("Your skin itches.");
	      else if(!destroy_arm(otmp2)) pline("Your skin itches.");
		exercise(A_STR, FALSE);
		exercise(A_CON, FALSE);

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_MEGALOAD:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		{

		struct obj *ldstone;

		pline("A gray stone appears from nowhere!");

		ldstone = mksobj_at(LOADSTONE, u.ux, u.uy, TRUE, FALSE, FALSE);
		if (ldstone) {
			ldstone->quan = 1L;
			ldstone->owt = weight(ldstone);
			if (!Blind) ldstone->dknown = 1;
			if (ldstone) {
			      pline("The stone automatically wanders into your knapsack!");
				(void) pickup_object(ldstone, 1L, TRUE, TRUE);
			}
		}

		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_BAD_EQUIPMENT:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		bad_equipment(0);

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_VILENESS:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		bad_artifact();

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_HYBRIDIZATION:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		changehybridization(0); /* random, either give or remove one */

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_NASTY_CURSE:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		nastytrapcurse();

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_OFFLEVEL_ITEM:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		if (otmp->oartifact == ART_HURO_HAEHAE) {

			int mattries = 0;

			register struct monst *offmon;
			while ((inv_cnt() > 0) && mattries++ < 50000) {
				if ((offmon = makemon((struct permonst *)0, 0, 0, NO_MM_FLAGS)) != 0) {
					char bufof[BUFSZ];
					bufof[0] = '\0';
					steal(offmon, bufof, TRUE, TRUE);
					mdrop_special_objs(offmon); /* don't levelport ones that have the amulet! */
					u_teleport_monB(offmon, FALSE);
				}
			}
			Your("inventory is gone.");

			if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp can be gone now! we NEED the return below */
			return 2;
		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		{

			register int inventcount = inv_cnt();

			if (inventcount > 0) {
				inventcount /= 8;
				if (inventcount < 1) inventcount = 1;

				while (inv_cnt() && inventcount) {
					char bufof[BUFSZ];
					bufof[0] = '\0';
					steal(mtmp, bufof, TRUE, TRUE);
					inventcount--;
				}

			}

			mdrop_special_objs(mtmp); /* make sure it doesn't tele to an unreachable place with the book of the dead or something */
			u_teleport_monB(mtmp, FALSE);
			pline("Some of your possessions have been stolen!");

		}

		return 2;

	case MUSE_SCR_RAGNAROK:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		ragnarok(TRUE);
		if (evilfriday) evilragnarok(TRUE,level_difficulty());

		return 2;

	case MUSE_SCR_ANTIMATTER:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		pline("You are caught in an antimatter storm!");
		withering_damage(invent, FALSE, FALSE);

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_RUMOR:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		{
			const char *line;
			char buflin[BUFSZ];
			if (rn2(2)) line = getrumor(-1, buflin, TRUE);
			else line = getrumor(0, buflin, TRUE);
			if (!*line) line = "Slash'EM rumors file closed for renovation.";
			pline("%s", line);
		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_MESSAGE:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		pline("%s", fauxmessage());
		u.cnd_plineamount++;
		if (!rn2(3)) {
			pline("%s", fauxmessage());
			u.cnd_plineamount++;
		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_ILLUSION:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

	    {
		coord cc;
		int cnt = rnd(6);
		if (mtmp->mconf) cnt += rno(6);
		if (otmp->cursed) cnt += rno(3);

		if (!enexto(&cc, mtmp->mx, mtmp->my, 0)) break;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		while(cnt--) {
			if (!enexto(&cc, mtmp->mx, mtmp->my, 0)) break;

			(void) makemon(illusionmon(), cc.x, cc.y, NO_MM_FLAGS);

		}
	    }
		pline("Some monsters from the Illusory Castle are summoned!");

		u.aggravation = 0;

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_VISIBLE_ITEM:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		pline("You are surrounded by a translucent glow!");
		{
			register struct obj *objX, *objX2;
			for (objX = invent; objX; objX = objX2) {
				objX2 = objX->nobj;
				if (!rn2(5)) objX->oinvis = objX->oinvisreal = FALSE;
			}
		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_EVIL_VARIANT:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

	    {
		coord cc;
		int cnt = rnd(6);
		if (mtmp->mconf) cnt += rno(6);
		if (otmp->cursed) cnt += rno(3);

		if (!enexto(&cc, mtmp->mx, mtmp->my, 0)) break;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		while(cnt--) {
			if (!enexto(&cc, mtmp->mx, mtmp->my, 0)) break;

			(void) makemon(specialtensmon(341), cc.x, cc.y, NO_MM_FLAGS); /* M5_EVIL */

		}
	    }
		pline("Some monsters from the Evil Variant are summoned!");

		u.aggravation = 0;

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_FEMINISM:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		if (!rn2(2)) {
			randomfeminismtrap(rnz( (level_difficulty() + 2) * rnd(50)));
			if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */
			return 2;
		}

	    {
		coord cc;
		int cnt = rnd(6);
		if (mtmp->mconf) cnt += rno(6);
		if (otmp->cursed) cnt += rno(3);

		if (!enexto(&cc, mtmp->mx, mtmp->my, 0)) break;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		while(cnt--) {
			if (!enexto(&cc, mtmp->mx, mtmp->my, 0)) break;

			(void) makemon(specialtensmon(!rn2(50) ? 369 : !rn2(20) ? 333 : !rn2(3) ? 38 : !rn2(2) ? 39 : 40), cc.x, cc.y, NO_MM_FLAGS); /* AD_FEMI, MS_STENCH, and the three MS_FART_foo */

		}
	    }
		pline("Several women appear from nowhere!");

		u.aggravation = 0;

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_SIN:

		mreadmsg(mtmp, otmp);

		{
		int dmg = 0;
		struct obj *otmpi, *otmpii;

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
						if (isstunfish) nomul(-rnz(5), "paralyzed by a scroll of sin", TRUE);
						else nomul(-rnd(5), "paralyzed by a scroll of sin", TRUE);
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
				if (flags.soundok) {
					You_hear("a chuckling laughter.");
					if (PlayerHearsSoundEffects) pline(issoviet ? "Kha-kha-kha-kha-kha-KDZH KDZH, tip bloka l'da smeyetsya yego tortsa, potomu chto vy teryayete vse vashi vstroyennyye funktsii!" : "Hoehoehoehoe!");
				}
			      attrcurse();
			      attrcurse();
				break;
			case 7: /* pride */
			      pline("The RNG determines to take you down a peg or two...");
				if (!rn2(3)) {
				    poisoned("air", rn2(A_MAX), "scroll of sin", 30);
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
				      polyself(FALSE);
				}
				break;
			    case 2:
				You("need reboot.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Eto poshel na khuy vverkh. No chto zhe vy ozhidali? Igra, v kotoruyu vy mozhete legko vyigrat'? Durak!" : "DUEUEDUET!");
				if (!Race_if(PM_UNGENOMOLD)) newman();
				else polyself(FALSE);
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
					if (isstunfish) nomul(-rnz(5), "paralyzed by a scroll of sin", TRUE);
					else nomul(-rnd(5), "paralyzed by a scroll of sin", TRUE);
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

		if (!objects[SCR_SIN].oc_name_known
			&& !objects[SCR_SIN].oc_uname)
		    docall(otmp);

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_IMMOBILITY:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		{

		int monstcnt;
		monstcnt = 8 + rno(10);
		if (otmp->cursed) monstcnt += (8 + rno(10));
		if (mtmp->mconf) monstcnt += (12 + rno(15));
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

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_EGOISM:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		{

		int monstcnt;
		monstcnt = rno(5);
		if (otmp->cursed) monstcnt += rno(6);
		if (mtmp->mconf) monstcnt += rno(12);
		int sessileattempts;
		int sessilemnum;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

	      while(--monstcnt >= 0) {
			for (sessileattempts = 0; sessileattempts < 10000; sessileattempts++) {
				sessilemnum = rndmonnum();
				if (sessilemnum != -1 && always_egotype(&mons[sessilemnum]) ) sessileattempts = 10000;
			}
			if (sessilemnum != -1) (void) makemon( &mons[sessilemnum], u.ux, u.uy, NO_MM_FLAGS);
		}

		}

		u.aggravation = 0;

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_ENRAGE:

		mreadmsg(mtmp, otmp);

		{

		int effectradius = (otmp->blessed ? 5 : otmp->cursed ? 20 : 10);
		if (mtmp->mconf) effectradius *= 3;
	      register struct monst *mtmp2;
		struct edog* edog;

		for (mtmp2 = fmon; mtmp2; mtmp2 = mtmp2->nmon) {
			if (rn2(3) && distu(mtmp2->mx,mtmp2->my) < effectradius) {
				if (mtmp2->mtame) {
					edog = (mtmp2->isminion) ? 0 : EDOG(mtmp2);
					if (mtmp2->mtame <= rnd(21) || (edog && edog->abuse >= rn2(6) )) {

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
							if (mtmp2->mleashed) { m_unleash(mtmp2,FALSE); }
						}
					}
				} else if (mtmp2->mpeaceful && !is_infrastructure_monster(mtmp2)) {
					mtmp2->mpeaceful = 0;
				} else {
					if (!is_infrastructure_monster(mtmp2)) {
						if (!rn2(5)) mtmp2->mfrenzied = 1;
						mtmp2->mhp = mtmp2->mhpmax;
					}
				}
			}
		}
		pline("It seems a little more dangerous here now...");

		if (!objects[SCR_ENRAGE].oc_name_known
			&& !objects[SCR_ENRAGE].oc_uname)
		    docall(otmp);

		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_DESTROY_WEAPON:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		if (otmp->oartifact == ART_HIJO_NI_HAYAI_WARUI) {
			badeffect(); badeffect(); badeffect(); badeffect(); badeffect();
		}

		otmp2 = uwep;
		if (otmp2 && stack_too_big(otmp2)) pline("Your fingers shake violently!");

		else if (otmp2 && otmp2->blessed && rn2(5)) pline("Your fingers shake violently!");
		/* extra saving throw for highly enchanted weapons --Amy */
		else if (otmp2 && (otmp2->spe > 1) && (rn2(otmp2->spe)) ) pline("Your fingers shake violently!");
		/* being magic resistant also offers protection */
		else if (Antimagic && rn2(StrongAntimagic ? 5 : 3)) pline("Your fingers shake violently!");
		/* artifacts are highly resistant */
		else if (otmp2 && otmp2->oartifact && rn2(20)) pline("Your fingers shake violently!");
		/* and grease will always offer protection but can wear off */
		else if (otmp2 && otmp2->greased) {
			pline("Your fingers shake violently!");
			 if (!rn2(2) || (isfriday && !rn2(2))) {
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
		exercise(A_STR, FALSE);
		exercise(A_CON, FALSE);

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_LAVA:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

			int madepoolB = 0;
			int xB,yB,safe_posB=0;
			int radiusB = 5;
			if (!rn2(3)) radiusB += rnd(4);
			if (!rn2(10)) radiusB += rnd(6);
			if (!rn2(25)) radiusB += rnd(8);
			if (radiusB > MAX_RADIUS) radiusB = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusB, do_lavafloodd, (void *)&madepoolB);

			/* check if there are safe tiles around the player */
			for (xB = u.ux-1; xB <= u.ux+1; xB++) {
				for (yB = u.uy - 1; yB <= u.uy + 1; yB++) {
					if (xB != u.ux && yB != u.uy &&
					    goodpos(xB, yB, &youmonst, 0)) {
						safe_posB++;
					}
				}
			}

			if (madepoolB)
				pline(FunnyHallu ?
						"Wow, that's, like, TOTALLY HOT, dude!" :
						"A stream of lava surges through the area!" );

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_GRAVE:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		{

			int madepoolSS = 0;
			int xSS,ySS,safe_posSS=0;
			int radiusSS = 5;
			if (!rn2(3)) radiusSS += rnd(4);
			if (!rn2(10)) radiusSS += rnd(6);
			if (!rn2(25)) radiusSS += rnd(8);
			if (radiusSS > MAX_RADIUS) radiusSS = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusSS, do_gravefloodd, (void *)&madepoolSS);

			/* check if there are safe tiles around the player */
			for (xSS = u.ux-1; xSS <= u.ux+1; xSS++) {
				for (ySS = u.uy - 1; ySS <= u.uy + 1; ySS++) {
					if (xSS != u.ux && ySS != u.uy &&
					    goodpos(xSS, ySS, &youmonst, 0)) {
						safe_posSS++;
					}
				}
			}

			if (madepoolSS)
				pline(FunnyHallu ?
						"Hans Walt has hidden the diamonds here! Are you a badass enough hussy to steal them?" :
						"Hans Walt erects grave walls!" );

		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_TUNNELS:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		{

			int madepoolSS = 0;
			int xSS,ySS,safe_posSS=0;
			int radiusSS = 5;
			if (!rn2(3)) radiusSS += rnd(4);
			if (!rn2(10)) radiusSS += rnd(6);
			if (!rn2(25)) radiusSS += rnd(8);
			if (radiusSS > MAX_RADIUS) radiusSS = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusSS, do_tunnelfloodd, (void *)&madepoolSS);

			/* check if there are safe tiles around the player */
			for (xSS = u.ux-1; xSS <= u.ux+1; xSS++) {
				for (ySS = u.uy - 1; ySS <= u.uy + 1; ySS++) {
					if (xSS != u.ux && ySS != u.uy &&
					    goodpos(xSS, ySS, &youmonst, 0)) {
						safe_posSS++;
					}
				}
			}

			if (madepoolSS)
				pline(FunnyHallu ?
						"The course is driving through a tunnel and therefore your mobile phone does not go!" :
						"It seems that construction workers have carved tunnels through the rock." );

		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_FARMING:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		{

			int madepoolSS = 0;
			int xSS,ySS,safe_posSS=0;
			int radiusSS = 5;
			if (!rn2(3)) radiusSS += rnd(4);
			if (!rn2(10)) radiusSS += rnd(6);
			if (!rn2(25)) radiusSS += rnd(8);
			if (radiusSS > MAX_RADIUS) radiusSS = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusSS, do_farmfloodd, (void *)&madepoolSS);

			/* check if there are safe tiles around the player */
			for (xSS = u.ux-1; xSS <= u.ux+1; xSS++) {
				for (ySS = u.uy - 1; ySS <= u.uy + 1; ySS++) {
					if (xSS != u.ux && ySS != u.uy &&
					    goodpos(xSS, ySS, &youmonst, 0)) {
						safe_posSS++;
					}
				}
			}

			if (madepoolSS)
				pline(FunnyHallu ?
						"The Greens are forcing new nature preservation laws upon you!" :
						"All the local farmers are claiming land on this dungeon level." );

		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_MOUNTAINS:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		{

			int madepoolSS = 0;
			int xSS,ySS,safe_posSS=0;
			int radiusSS = 5;
			if (!rn2(3)) radiusSS += rnd(4);
			if (!rn2(10)) radiusSS += rnd(6);
			if (!rn2(25)) radiusSS += rnd(8);
			if (radiusSS > MAX_RADIUS) radiusSS = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusSS, do_mountainfloodd, (void *)&madepoolSS);

			/* check if there are safe tiles around the player */
			for (xSS = u.ux-1; xSS <= u.ux+1; xSS++) {
				for (ySS = u.uy - 1; ySS <= u.uy + 1; ySS++) {
					if (xSS != u.ux && ySS != u.uy &&
					    goodpos(xSS, ySS, &youmonst, 0)) {
						safe_posSS++;
					}
				}
			}

			if (madepoolSS)
				pline(FunnyHallu ?
						"The Amy her roommate uses a wing-tufted facial expression on you! :-)" :
						"Strange... the underground dungeon seems to become mountainous." );

		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_DIVING:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		{

			int madepoolSS = 0;
			int xSS,ySS,safe_posSS=0;
			int radiusSS = 5;
			if (!rn2(3)) radiusSS += rnd(4);
			if (!rn2(10)) radiusSS += rnd(6);
			if (!rn2(25)) radiusSS += rnd(8);
			if (radiusSS > MAX_RADIUS) radiusSS = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusSS, do_watertunnelfloodd, (void *)&madepoolSS);

			/* check if there are safe tiles around the player */
			for (xSS = u.ux-1; xSS <= u.ux+1; xSS++) {
				for (ySS = u.uy - 1; ySS <= u.uy + 1; ySS++) {
					if (xSS != u.ux && ySS != u.uy &&
					    goodpos(xSS, ySS, &youmonst, 0)) {
						safe_posSS++;
					}
				}
			}

			if (madepoolSS)
				pline(FunnyHallu ?
						"Deep sea diving challenge! You must collect 50 clams in three minutes or lose the game automatically!" :
						"Watery tunnels are erected!" );

		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_CRYSTALLIZATION:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		{

			int madepoolSS = 0;
			int xSS,ySS,safe_posSS=0;
			int radiusSS = 5;
			if (!rn2(3)) radiusSS += rnd(4);
			if (!rn2(10)) radiusSS += rnd(6);
			if (!rn2(25)) radiusSS += rnd(8);
			if (radiusSS > MAX_RADIUS) radiusSS = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusSS, do_crystalwaterfloodd, (void *)&madepoolSS);

			/* check if there are safe tiles around the player */
			for (xSS = u.ux-1; xSS <= u.ux+1; xSS++) {
				for (ySS = u.uy - 1; ySS <= u.uy + 1; ySS++) {
					if (xSS != u.ux && ySS != u.uy &&
					    goodpos(xSS, ySS, &youmonst, 0)) {
						safe_posSS++;
					}
				}
			}

			if (madepoolSS)
				pline(FunnyHallu ?
						"Due to the laws of physics, the water flows upward." :
						"Crystallized water appears on the ceiling." );

		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_MOORLAND:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		{

			int madepoolSS = 0;
			int xSS,ySS,safe_posSS=0;
			int radiusSS = 5;
			if (!rn2(3)) radiusSS += rnd(4);
			if (!rn2(10)) radiusSS += rnd(6);
			if (!rn2(25)) radiusSS += rnd(8);
			if (radiusSS > MAX_RADIUS) radiusSS = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusSS, do_moorfloodd, (void *)&madepoolSS);

			/* check if there are safe tiles around the player */
			for (xSS = u.ux-1; xSS <= u.ux+1; xSS++) {
				for (ySS = u.uy - 1; ySS <= u.uy + 1; ySS++) {
					if (xSS != u.ux && ySS != u.uy &&
					    goodpos(xSS, ySS, &youmonst, 0)) {
						safe_posSS++;
					}
				}
			}

			if (madepoolSS)
				pline(FunnyHallu ?
						"Watch out, the swamp holes are like shifting sand - you drown if you fall into them." :
						"The dungeon gets swampy." );

		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_URINE:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		{

			int madepoolSS = 0;
			int xSS,ySS,safe_posSS=0;
			int radiusSS = 5;
			if (!rn2(3)) radiusSS += rnd(4);
			if (!rn2(10)) radiusSS += rnd(6);
			if (!rn2(25)) radiusSS += rnd(8);
			if (radiusSS > MAX_RADIUS) radiusSS = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusSS, do_urinefloodd, (void *)&madepoolSS);

			/* check if there are safe tiles around the player */
			for (xSS = u.ux-1; xSS <= u.ux+1; xSS++) {
				for (ySS = u.uy - 1; ySS <= u.uy + 1; ySS++) {
					if (xSS != u.ux && ySS != u.uy &&
					    goodpos(xSS, ySS, &youmonst, 0)) {
						safe_posSS++;
					}
				}
			}

			if (madepoolSS)
				pline(FunnyHallu ?
						"Holy crap, you get to watch Mira create her 'swimming pools' firsthand..." :
						"Mira pees all over the dungeon." );

		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_QUICKSAND:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		{

			int madepoolSS = 0;
			int xSS,ySS,safe_posSS=0;
			int radiusSS = 5;
			if (!rn2(3)) radiusSS += rnd(4);
			if (!rn2(10)) radiusSS += rnd(6);
			if (!rn2(25)) radiusSS += rnd(8);
			if (radiusSS > MAX_RADIUS) radiusSS = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusSS, do_shiftingsandfloodd, (void *)&madepoolSS);

			/* check if there are safe tiles around the player */
			for (xSS = u.ux-1; xSS <= u.ux+1; xSS++) {
				for (ySS = u.uy - 1; ySS <= u.uy + 1; ySS++) {
					if (xSS != u.ux && ySS != u.uy &&
					    goodpos(xSS, ySS, &youmonst, 0)) {
						safe_posSS++;
					}
				}
			}

			if (madepoolSS)
				pline(FunnyHallu ?
						"Uh-oh, on one side you have a cube and there's a cute asian girl on the other! Both of them are deadly!" :
						"Deadly sandholes appear." );

		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_STYX:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		{

			int madepoolSS = 0;
			int xSS,ySS,safe_posSS=0;
			int radiusSS = 5;
			if (!rn2(3)) radiusSS += rnd(4);
			if (!rn2(10)) radiusSS += rnd(6);
			if (!rn2(25)) radiusSS += rnd(8);
			if (radiusSS > MAX_RADIUS) radiusSS = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusSS, do_styxfloodd, (void *)&madepoolSS);

			/* check if there are safe tiles around the player */
			for (xSS = u.ux-1; xSS <= u.ux+1; xSS++) {
				for (ySS = u.uy - 1; ySS <= u.uy + 1; ySS++) {
					if (xSS != u.ux && ySS != u.uy &&
					    goodpos(xSS, ySS, &youmonst, 0)) {
						safe_posSS++;
					}
				}
			}

			if (madepoolSS)
				pline(FunnyHallu ?
						"A constant 'Jason... Jason... Jason...' chant is audible in the distance..." :
						"Hellish green water flows into the dungeon!" );

		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_SNOW:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		{

			int madepoolSS = 0;
			int xSS,ySS,safe_posSS=0;
			int radiusSS = 5;
			if (!rn2(3)) radiusSS += rnd(4);
			if (!rn2(10)) radiusSS += rnd(6);
			if (!rn2(25)) radiusSS += rnd(8);
			if (radiusSS > MAX_RADIUS) radiusSS = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusSS, do_snowfloodd, (void *)&madepoolSS);

			/* check if there are safe tiles around the player */
			for (xSS = u.ux-1; xSS <= u.ux+1; xSS++) {
				for (ySS = u.uy - 1; ySS <= u.uy + 1; ySS++) {
					if (xSS != u.ux && ySS != u.uy &&
					    goodpos(xSS, ySS, &youmonst, 0)) {
						safe_posSS++;
					}
				}
			}

			if (madepoolSS)
				pline(FunnyHallu ?
						"A hailstorm! Quick, find a shelter, because otherwise the hailstones will get bigger and bigger until they crush you!" :
						"It starts snowing!" );

		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_ASH:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		{

			int madepoolSS = 0;
			int xSS,ySS,safe_posSS=0;
			int radiusSS = 5;
			if (!rn2(3)) radiusSS += rnd(4);
			if (!rn2(10)) radiusSS += rnd(6);
			if (!rn2(25)) radiusSS += rnd(8);
			if (radiusSS > MAX_RADIUS) radiusSS = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusSS, do_ashfloodd, (void *)&madepoolSS);

			/* check if there are safe tiles around the player */
			for (xSS = u.ux-1; xSS <= u.ux+1; xSS++) {
				for (ySS = u.uy - 1; ySS <= u.uy + 1; ySS++) {
					if (xSS != u.ux && ySS != u.uy &&
					    goodpos(xSS, ySS, &youmonst, 0)) {
						safe_posSS++;
					}
				}
			}

			if (madepoolSS)
				pline(FunnyHallu ?
						"Mehrunes Dagon has opened yet another gate to Oblivion!" :
						"The floor becomes red." );

		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_SAND:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		{

			int madepoolSS = 0;
			int xSS,ySS,safe_posSS=0;
			int radiusSS = 5;
			if (!rn2(3)) radiusSS += rnd(4);
			if (!rn2(10)) radiusSS += rnd(6);
			if (!rn2(25)) radiusSS += rnd(8);
			if (radiusSS > MAX_RADIUS) radiusSS = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusSS, do_sandfloodd, (void *)&madepoolSS);

			/* check if there are safe tiles around the player */
			for (xSS = u.ux-1; xSS <= u.ux+1; xSS++) {
				for (ySS = u.uy - 1; ySS <= u.uy + 1; ySS++) {
					if (xSS != u.ux && ySS != u.uy &&
					    goodpos(xSS, ySS, &youmonst, 0)) {
						safe_posSS++;
					}
				}
			}

			if (madepoolSS)
				pline(FunnyHallu ?
						"Oh no, you're stranded in the desert and your water reserves are depleted! This is the end!" :
						"Soft sand appears in the dungeon." );

		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_PAVING:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		{

			int madepoolSS = 0;
			int xSS,ySS,safe_posSS=0;
			int radiusSS = 5;
			if (!rn2(3)) radiusSS += rnd(4);
			if (!rn2(10)) radiusSS += rnd(6);
			if (!rn2(25)) radiusSS += rnd(8);
			if (radiusSS > MAX_RADIUS) radiusSS = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusSS, do_pavementfloodd, (void *)&madepoolSS);

			/* check if there are safe tiles around the player */
			for (xSS = u.ux-1; xSS <= u.ux+1; xSS++) {
				for (ySS = u.uy - 1; ySS <= u.uy + 1; ySS++) {
					if (xSS != u.ux && ySS != u.uy &&
					    goodpos(xSS, ySS, &youmonst, 0)) {
						safe_posSS++;
					}
				}
			}

			if (madepoolSS)
				pline(FunnyHallu ?
						"Holy shit there are paved roads and large fountains and whoa you absolutely need to play part 2 of this game!" :
						"Suddenly there are paved roads everywhere!" );

		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_HIGHWAY:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		{

			int madepoolSS = 0;
			int xSS,ySS,safe_posSS=0;
			int radiusSS = 5;
			if (!rn2(3)) radiusSS += rnd(4);
			if (!rn2(10)) radiusSS += rnd(6);
			if (!rn2(25)) radiusSS += rnd(8);
			if (radiusSS > MAX_RADIUS) radiusSS = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusSS, do_highwayfloodd, (void *)&madepoolSS);

			/* check if there are safe tiles around the player */
			for (xSS = u.ux-1; xSS <= u.ux+1; xSS++) {
				for (ySS = u.uy - 1; ySS <= u.uy + 1; ySS++) {
					if (xSS != u.ux && ySS != u.uy &&
					    goodpos(xSS, ySS, &youmonst, 0)) {
						safe_posSS++;
					}
				}
			}

			if (madepoolSS)
				pline(FunnyHallu ?
						"DUDE! The secret entrance to the right is open! RUN, it will close in 100 turns!" :
						"Highways are being built all over the dungeon!" );

		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_GRASSLAND:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		{

			int madepoolSS = 0;
			int xSS,ySS,safe_posSS=0;
			int radiusSS = 5;
			if (!rn2(3)) radiusSS += rnd(4);
			if (!rn2(10)) radiusSS += rnd(6);
			if (!rn2(25)) radiusSS += rnd(8);
			if (radiusSS > MAX_RADIUS) radiusSS = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusSS, do_grassfloodd, (void *)&madepoolSS);

			/* check if there are safe tiles around the player */
			for (xSS = u.ux-1; xSS <= u.ux+1; xSS++) {
				for (ySS = u.uy - 1; ySS <= u.uy + 1; ySS++) {
					if (xSS != u.ux && ySS != u.uy &&
					    goodpos(xSS, ySS, &youmonst, 0)) {
						safe_posSS++;
					}
				}
			}

			if (madepoolSS)
				pline(FunnyHallu ?
						"You sense the presence of a bunch of spies." :
						"You feel the grass grow." );

		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_NETHER:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		{

			int madepoolSS = 0;
			int xSS,ySS,safe_posSS=0;
			int radiusSS = 5;
			if (!rn2(3)) radiusSS += rnd(4);
			if (!rn2(10)) radiusSS += rnd(6);
			if (!rn2(25)) radiusSS += rnd(8);
			if (radiusSS > MAX_RADIUS) radiusSS = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusSS, do_nethermistfloodd, (void *)&madepoolSS);

			/* check if there are safe tiles around the player */
			for (xSS = u.ux-1; xSS <= u.ux+1; xSS++) {
				for (ySS = u.uy - 1; ySS <= u.uy + 1; ySS++) {
					if (xSS != u.ux && ySS != u.uy &&
					    goodpos(xSS, ySS, &youmonst, 0)) {
						safe_posSS++;
					}
				}
			}

			if (madepoolSS)
				pline(FunnyHallu ?
						"Suddenly Galadriel throws you into the void, and you need to defeat Melkor before you can come back." :
						"Purple mist appears." );

		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_STALACTITE:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		{

			int madepoolSS = 0;
			int xSS,ySS,safe_posSS=0;
			int radiusSS = 5;
			if (!rn2(3)) radiusSS += rnd(4);
			if (!rn2(10)) radiusSS += rnd(6);
			if (!rn2(25)) radiusSS += rnd(8);
			if (radiusSS > MAX_RADIUS) radiusSS = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusSS, do_stalactitefloodd, (void *)&madepoolSS);

			/* check if there are safe tiles around the player */
			for (xSS = u.ux-1; xSS <= u.ux+1; xSS++) {
				for (ySS = u.uy - 1; ySS <= u.uy + 1; ySS++) {
					if (xSS != u.ux && ySS != u.uy &&
					    goodpos(xSS, ySS, &youmonst, 0)) {
						safe_posSS++;
					}
				}
			}

			if (madepoolSS)
				pline(FunnyHallu ?
						"Team Piercer is attacking, even though they're actually rather rare." :
						"Stalactites shoot out of the ceiling!" );

		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_CRYPT:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		{

			int madepoolSS = 0;
			int xSS,ySS,safe_posSS=0;
			int radiusSS = 5;
			if (!rn2(3)) radiusSS += rnd(4);
			if (!rn2(10)) radiusSS += rnd(6);
			if (!rn2(25)) radiusSS += rnd(8);
			if (radiusSS > MAX_RADIUS) radiusSS = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusSS, do_cryptfloodd, (void *)&madepoolSS);

			/* check if there are safe tiles around the player */
			for (xSS = u.ux-1; xSS <= u.ux+1; xSS++) {
				for (ySS = u.uy - 1; ySS <= u.uy + 1; ySS++) {
					if (xSS != u.ux && ySS != u.uy &&
					    goodpos(xSS, ySS, &youmonst, 0)) {
						safe_posSS++;
					}
				}
			}

			if (madepoolSS)
				pline(FunnyHallu ?
						"Argh! In your stupidity, you forgot to get the torch and now the crypts are completely dark!" :
						"Seems you're in the crypt now." );

		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_BUBBLE_BOBBLE:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		{

			int madepoolSS = 0;
			int xSS,ySS,safe_posSS=0;
			int radiusSS = 5;
			if (!rn2(3)) radiusSS += rnd(4);
			if (!rn2(10)) radiusSS += rnd(6);
			if (!rn2(25)) radiusSS += rnd(8);
			if (radiusSS > MAX_RADIUS) radiusSS = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusSS, do_bubblefloodd, (void *)&madepoolSS);

			/* check if there are safe tiles around the player */
			for (xSS = u.ux-1; xSS <= u.ux+1; xSS++) {
				for (ySS = u.uy - 1; ySS <= u.uy + 1; ySS++) {
					if (xSS != u.ux && ySS != u.uy &&
					    goodpos(xSS, ySS, &youmonst, 0)) {
						safe_posSS++;
					}
				}
			}

			if (madepoolSS)
				pline(FunnyHallu ?
						"There are some air bubbles... maybe you can reach higher places with them?" :
						"Floating bubbles appear!" );

		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_RAIN:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		{

			int madepoolSS = 0;
			int xSS,ySS,safe_posSS=0;
			int radiusSS = 5;
			if (!rn2(3)) radiusSS += rnd(4);
			if (!rn2(10)) radiusSS += rnd(6);
			if (!rn2(25)) radiusSS += rnd(8);
			if (radiusSS > MAX_RADIUS) radiusSS = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusSS, do_raincloudfloodd, (void *)&madepoolSS);

			/* check if there are safe tiles around the player */
			for (xSS = u.ux-1; xSS <= u.ux+1; xSS++) {
				for (ySS = u.uy - 1; ySS <= u.uy + 1; ySS++) {
					if (xSS != u.ux && ySS != u.uy &&
					    goodpos(xSS, ySS, &youmonst, 0)) {
						safe_posSS++;
					}
				}
			}

			if (madepoolSS)
				pline(FunnyHallu ?
						"Suddenly, rain starts to pour down from the sky." :
						"It starts to rain." );

		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_FLOODING:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

			int madepoolL = 0;
			int xL,yL,safe_posL=0;
			int radiusL = 5;
			if (!rn2(3)) radiusL += rnd(4);
			if (!rn2(10)) radiusL += rnd(6);
			if (!rn2(25)) radiusL += rnd(8);
			if (radiusL > MAX_RADIUS) radiusL = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusL, do_megafloodingd, (void *)&madepoolL);

			/* check if there are safe tiles around the player */
			for (xL = u.ux-1; xL <= u.ux+1; xL++) {
				for (yL = u.uy - 1; yL <= u.uy + 1; yL++) {
					if (xL != u.ux && yL != u.uy &&
					    goodpos(xL, yL, &youmonst, 0)) {
						safe_posL++;
					}
				}
			}

			if (madepoolL)
				pline(FunnyHallu ?
						"Whoa, swimming pools and stuff!" :
						"The dungeon is flooded!" );

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_STONING:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		    if ((!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) &&
			!(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {
			if (!Stoned) {
				if (Hallucination && rn2(10)) pline("Thankfully you are already stoned.");
				else {
					Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
					u.cnd_stoningcount++;
					pline("You start turning to stone!");
					stop_occupation();
				}
			}
			sprintf(killer_buf, "a petrification scroll");
			delayed_killer = killer_buf;
		
		    }

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_WOUNDS:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		You_feel("bad!");
			if (!rn2(20)) losehp(d(10,8), "a scroll of wounds", KILLED_BY);
			else if (!rn2(5)) losehp(d(6,8), "a scroll of wounds", KILLED_BY);
			else losehp(d(4,6), "a scroll of wounds", KILLED_BY);

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_LOCKOUT:

		{
		boolean boulderlockout = FALSE;
		if (otmp->oartifact == ART_BLOCK_IT_REAL) boulderlockout = TRUE;

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

			int madepoolQ = 0;
			int xQ,yQ,safe_posQ=0;
			int radiusC = 5;
			if (!rn2(3)) radiusC += rnd(4);
			if (!rn2(10)) radiusC += rnd(6);
			if (!rn2(25)) radiusC += rnd(8);
			if (radiusC > MAX_RADIUS) radiusC = MAX_RADIUS;
			if (boulderlockout) do_clear_areaX(u.ux, u.uy, radiusC, do_lockflooddboulder, (void *)&madepoolQ);
			else do_clear_areaX(u.ux, u.uy, radiusC, do_lockfloodd, (void *)&madepoolQ);

			/* check if there are safe tiles around the player */
			for (xQ = u.ux-1; xQ <= u.ux+1; xQ++) {
				for (yQ = u.uy - 1; yQ <= u.uy + 1; yQ++) {
					if (xQ != u.ux && yQ != u.uy &&
					    goodpos(xQ, yQ, &youmonst, 0)) {
						safe_posQ++;
					}
				}
			}

			if (madepoolQ)
				pline(FunnyHallu ?
						"It's getting a little bit tight in here!" :
						"Walls and obstacles shoot up from the ground!" );
			else pline(FunnyHallu ?
						"You hear a grating that reminds you of Chinese water torture!" :
						"You see dust particles flying around." );

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		}

		return 2;

	case MUSE_SCR_GROWTH:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

			int madepoolC = 0;
			int xC,yC,safe_posC=0;
			int radiusD = 5;
			if (!rn2(3)) radiusD += rnd(4);
			if (!rn2(10)) radiusD += rnd(6);
			if (!rn2(25)) radiusD += rnd(8);
			if (radiusD > MAX_RADIUS) radiusD = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusD, do_treefloodd, (void *)&madepoolC);

			/* check if there are safe tiles around the player */
			for (xC = u.ux-1; xC <= u.ux+1; xC++) {
				for (yC = u.uy - 1; yC <= u.uy + 1; yC++) {
					if (xC != u.ux && yC != u.uy &&
					    goodpos(xC, yC, &youmonst, 0)) {
						safe_posC++;
					}
				}
			}

			if (madepoolC)
				pline(FunnyHallu ?
						"Uh... everything is so... green!?" :
						"You see trees growing out of the ground!" );

		if (otmp && otmp->oartifact == ART_PANIC_IN_GOTHAM_FOREST) {
		    int i, j, bd = 100;
		    for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if (levl[u.ux + i][u.uy + j].typ == ROOM || levl[u.ux + i][u.uy + j].typ == CORR) {
				levl[u.ux + i][u.uy + j].typ = TREE;
				blockorunblock_point(u.ux + i,u.uy + j);
				if (!(levl[u.ux + i][u.uy + j].wall_info & W_EASYGROWTH)) levl[u.ux + i][u.uy + j].wall_info |= W_HARDGROWTH;
				del_engr_at(u.ux + i, u.uy + j);
	
				newsym(u.ux + i,u.uy + j);
			}

		    }
			pline("Uh-oh... there has been a strange increase in the number of trees lately. This is of course very dangerous :-), because if it turns out that this is the work of Poison Ivy, we'll have a major panic on our hands.");
		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_ICE:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

			int madepoolD = 0;
			int xD,yD,safe_posD=0;
			int radiusE = 5;
			if (!rn2(3)) radiusE += rnd(4);
			if (!rn2(10)) radiusE += rnd(6);
			if (!rn2(25)) radiusE += rnd(8);
			if (radiusE > MAX_RADIUS) radiusE = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusE, do_icefloodd, (void *)&madepoolD);

			/* check if there are safe tiles around the player */
			for (xD = u.ux-1; xD <= u.ux+1; xD++) {
				for (yD = u.uy - 1; yD <= u.uy + 1; yD++) {
					if (xD != u.ux && yD != u.uy &&
					    goodpos(xD, yD, &youmonst, 0)) {
						safe_posD++;
					}
				}
			}

			if (madepoolD)
				pline(FunnyHallu ?
						"Damn, this is giving you the chills!" :
						"The floor crackles with ice!" );

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_BAD_EFFECT:

		mreadmsg(mtmp, otmp);

		makeknown(otmp->otyp);

		badeffect();

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_CLOUDS:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

			int madepoolE = 0;
			int xE,yE,safe_posE=0;
			int radiusF = 5;
			if (!rn2(3)) radiusF += rnd(4);
			if (!rn2(10)) radiusF += rnd(6);
			if (!rn2(25)) radiusF += rnd(8);
			if (radiusF > MAX_RADIUS) radiusF = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusF, do_cloudfloodd, (void *)&madepoolE);

			/* check if there are safe tiles around the player */
			for (xE = u.ux-1; xE <= u.ux+1; xE++) {
				for (yE = u.uy - 1; yE <= u.uy + 1; yE++) {
					if (xE != u.ux && yE != u.uy &&
					    goodpos(xE, yE, &youmonst, 0)) {
						safe_posE++;
					}
				}
			}

			if (madepoolE)
				pline(FunnyHallu ?
						"Wow! Floating clouds..." :
						"Foggy clouds appear out of thin air!" );

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_BARRHING:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

			int madepoolF = 0;
			int xF,yF,safe_posF=0;
			int radiusG = 5;
			if (!rn2(3)) radiusG += rnd(4);
			if (!rn2(10)) radiusG += rnd(6);
			if (!rn2(25)) radiusG += rnd(8);
			if (radiusG > MAX_RADIUS) radiusG = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusG, do_barfloodd, (void *)&madepoolF);

			/* check if there are safe tiles around the player */
			for (xF = u.ux-1; xF <= u.ux+1; xF++) {
				for (yF = u.uy - 1; yF <= u.uy + 1; yF++) {
					if (xF != u.ux && yF != u.uy &&
					    goodpos(xF, yF, &youmonst, 0)) {
						safe_posF++;
					}
				}
			}

			if (madepoolF)
				pline(FunnyHallu ?
						"Aw shit, this feels like being in a jail!" :
						"Iron bars shoot up from the ground!" );

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_CHAOS_TERRAIN:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

			int madepoolR = 0;
			int xR,yR,safe_posR=0;
			int radiusR = 5;
			if (!rn2(3)) radiusR += rnd(4);
			if (!rn2(10)) radiusR += rnd(6);
			if (!rn2(25)) radiusR += rnd(8);
			if (radiusR > MAX_RADIUS) radiusR = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusR, do_terrainfloodd, (void *)&madepoolR);

			/* check if there are safe tiles around the player */
			for (xR = u.ux-1; xR <= u.ux+1; xR++) {
				for (yR = u.uy - 1; yR <= u.uy + 1; yR++) {
					if (xR != u.ux && yR != u.uy &&
					    goodpos(xR, yR, &youmonst, 0)) {
						safe_posR++;
					}
				}
			}

			if (madepoolR)
				pline(FunnyHallu ?
						"Oh wow, look at all the stuff that is happening around you!" :
						"What the heck is happening to the dungeon?!" );

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_WAN_SUMMON_SEXY_GIRL:

	    {	coord cc;
		struct permonst *pm = 0;
		struct monst *mon;
		boolean known = FALSE;
		int attempts = 0;

		if (!enexto(&cc, mtmp->mx, mtmp->my, 0)) break;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

newboss:
		do {
			pm = rndmonst();
			attempts++;
			if (attempts && (attempts % 10000 == 0)) u.mondiffhack++;
			if (!rn2(2000)) reset_rndmonst(NON_PM);

		} while ( (!pm || (pm && !(pm->msound == MS_FART_LOUD || pm->msound == MS_FART_NORMAL || pm->msound == MS_FART_QUIET ))) && attempts < 50000);

		if (!pm && rn2(50) ) {
			attempts = 0;
			goto newboss;
		}
		if (pm && !(pm->msound == MS_FART_LOUD || pm->msound == MS_FART_NORMAL || pm->msound == MS_FART_QUIET) && rn2(50) ) {
			attempts = 0;
			goto newboss;
		}

		if (pm) mon = makemon(pm, cc.x, cc.y, NO_MM_FLAGS);
	      if (mon && canspotmon(mon)) known = TRUE;
		if (known) makeknown(otmp->otyp);

		u.aggravation = 0;
		u.mondiffhack = 0;

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);

		return 2;
	    }


	case MUSE_WAN_TRAP_CREATION:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		makeknown(otmp->otyp);
	      You_feel("endangered!!");
		{
			int rtrap;
		    int i, j, bd;
			bd = 1;
			if (!rn2(5)) bd += rnz(1);
			boolean canbeinawall = FALSE;
			if (!rn2(Passes_walls ? 5 : 25)) canbeinawall = TRUE;

		      for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
				if (!isok(u.ux + i, u.uy + j)) continue;
				if ((levl[u.ux + i][u.uy + j].typ <= DBWALL) && !canbeinawall) continue;
				if (t_at(u.ux + i, u.uy + j)) continue;

			      rtrap = rndtrap();

				(void) maketrap(u.ux + i, u.uy + j, rtrap, 100, TRUE);
			}
		}

		makerandomtrap(TRUE);
		if (!rn2(2)) makerandomtrap(TRUE);
		if (!rn2(4)) makerandomtrap(TRUE);
		if (!rn2(8)) makerandomtrap(TRUE);
		if (!rn2(16)) makerandomtrap(TRUE);
		if (!rn2(32)) makerandomtrap(TRUE);
		if (!rn2(64)) makerandomtrap(TRUE);
		if (!rn2(128)) makerandomtrap(TRUE);
		if (!rn2(256)) makerandomtrap(TRUE);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);

		return 2;

	case MUSE_WAN_BAD_EFFECT:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		badeffect();

		if (oseen) makeknown(WAN_BAD_EFFECT);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_BLEEDING:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		playerbleed(rnd(2 + (level_difficulty() * 10)));
		if (oseen) makeknown(WAN_BLEEDING);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_UNDRESSING:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		shank_player();
		if (oseen) makeknown(WAN_UNDRESSING);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_CURSE_ITEMS:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		pline("A black glow surrounds you...");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
		rndcurse();

		if (oseen) makeknown(WAN_CURSE_ITEMS);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_LEVITATION:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		incr_itimeout(&HLevitation, rnd(100) );
		pline("You float up!");

		if (oseen) makeknown(WAN_LEVITATION);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_AMNESIA:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		You_feel("dizzy!");
		forget(1 + rn2(5));

		if (oseen) makeknown(WAN_AMNESIA);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_BAD_EQUIPMENT:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		bad_equipment(0);

		if (oseen) makeknown(WAN_BAD_EQUIPMENT);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_INSANITY:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		increasesanity(rnd((level_difficulty() * 5) + 20));

		if (oseen) makeknown(WAN_INSANITY);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_IMMOBILITY:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		{

		int monstcnt;
		monstcnt = 8 + rno(10);
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

		if (oseen) makeknown(WAN_IMMOBILITY);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_EGOISM:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		{

		int monstcnt;
		monstcnt = rno(5);
		int sessileattempts;
		int sessilemnum;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

	      while(--monstcnt >= 0) {
			for (sessileattempts = 0; sessileattempts < 10000; sessileattempts++) {
				sessilemnum = rndmonnum();
				if (sessilemnum != -1 && always_egotype(&mons[sessilemnum]) ) sessileattempts = 10000;
			}
			if (sessilemnum != -1) (void) makemon( &mons[sessilemnum], u.ux, u.uy, NO_MM_FLAGS);
		}

		}

		u.aggravation = 0;

		if (oseen) makeknown(WAN_EGOISM);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_SCR_AMNESIA:

		mreadmsg(mtmp, otmp);

		You_feel("dizzy!");
		forget(1 + rn2(5));

		makeknown(otmp->otyp); /* do this after you forgot stuff */

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_WAN_BAD_LUCK:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		You_feel("very unlucky.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Perekhod perekhod monstra s palochkoy nevezeniya! Zapiski igroka neskol'ko raz, chtoby sdelat' etot plaksivyy malen'kiy ublyudok konchatsya udachi polnost'yu i umeret'!" : "Dieuuuuuuu!");
		change_luck(-1);

		if (oseen) makeknown(WAN_BAD_LUCK);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_REMOVE_RESISTANCE:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		attrcurse();

		if (oseen) makeknown(WAN_REMOVE_RESISTANCE);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_CHAOS_TERRAIN:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		wandofchaosterrain();

		if (oseen) makeknown(WAN_CHAOS_TERRAIN);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_FLEECY_TERRAIN:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		wandoffleecyterrain();
		pline("Some changes in terrain are happening.");

		if (oseen) makeknown(WAN_FLEECY_TERRAIN);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_STAT_REDUCTION:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		statdebuff();

		if (oseen) makeknown(WAN_STAT_REDUCTION);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_DISENCHANTMENT:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		{
			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(10)) (void) drain_item_severely(otmpE);
			}
			Your("equipment seems less effective.");
			u.cnd_disenchantamount++;
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		}

		if (oseen) makeknown(WAN_DISENCHANTMENT);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_CONTAMINATION:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		contaminate(rnd(10 + level_difficulty()), TRUE);

		if (oseen) makeknown(WAN_CONTAMINATION);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_TREMBLING:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		pline("Your %s are trembling!", makeplural(body_part(HAND)));
		u.tremblingamount++;

		if (oseen) makeknown(WAN_TREMBLING);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_CORROSION:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		    register struct obj *objX, *objX2;
		    for (objX = invent; objX; objX = objX2) {
		      objX2 = objX->nobj;
			if (!rn2(5)) rust_dmg(objX, xname(objX), 3, TRUE, &youmonst);
		    }

		if (oseen) makeknown(WAN_CORROSION);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_FUMBLING:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		pline("You start trembling...");
		HFumbling = FROMOUTSIDE | rnd(5);
		incr_itimeout(&HFumbling, rnd(20));
		u.fumbleduration += rnz(1000);

		if (oseen) makeknown(WAN_FUMBLING);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_SIN:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		{
		int dmg = 0;
		struct obj *otmpi, *otmpii;

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
						if (isstunfish) nomul(-rnz(5), "paralyzed by a wand of sin", TRUE);
						else nomul(-rnd(5), "paralyzed by a wand of sin", TRUE);
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
				if (flags.soundok) {
					You_hear("a chuckling laughter.");
					if (PlayerHearsSoundEffects) pline(issoviet ? "Kha-kha-kha-kha-kha-KDZH KDZH, tip bloka l'da smeyetsya yego tortsa, potomu chto vy teryayete vse vashi vstroyennyye funktsii!" : "Hoehoehoehoe!");
				}
			      attrcurse();
			      attrcurse();
				break;
			case 7: /* pride */
			      pline("The RNG determines to take you down a peg or two...");
				if (!rn2(3)) {
				    poisoned("air", rn2(A_MAX), "wand of sin", 30);
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
				      polyself(FALSE);
				}
				break;
			    case 2:
				You("need reboot.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Eto poshel na khuy vverkh. No chto zhe vy ozhidali? Igra, v kotoruyu vy mozhete legko vyigrat'? Durak!" : "DUEUEDUET!");
				if (!Race_if(PM_UNGENOMOLD)) newman();
				else polyself(FALSE);
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
					if (isstunfish) nomul(-rnz(5), "paralyzed by a wand of sin", TRUE);
					else nomul(-rnd(5), "paralyzed by a wand of sin", TRUE);
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

		if (oseen) makeknown(WAN_SIN);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_FINGER_BENDING:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		pline("Your %s bend themselves!", makeplural(body_part(FINGER)) );
		incr_itimeout(&Glib, rnd(15) + rnd(monster_difficulty() + 1) );

		if (oseen) makeknown(WAN_FINGER_BENDING);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_DESLEXIFICATION:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		/* You are being deslexified, because even in soviet mode you're still technically playing slex. --Amy */

		if (Upolyd && u.mh > 1) u.mh /= 2;
		else if (!Upolyd && u.uhp > 1) u.uhp /= 2;
		losehp(1, "deslexification beam", KILLED_BY_AN);
		switch (rnd(10)) { /* These fallthroughs are intentional. --Amy */
			case 1: badeffect();
			case 2: badeffect();
			case 3: badeffect();
			case 4: badeffect();
			case 5: badeffect();
			case 6: badeffect();
			case 7: badeffect();
			case 8: badeffect();
			case 9: badeffect();
			case 10: badeffect();
			break;
		}

		if (oseen) makeknown(WAN_DESLEXIFICATION);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_DRAIN_MANA:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		pline("You lose  Mana");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vasha magicheskaya energiya udalyayetsya v nastoyashcheye vremya. Skoro on budet raven nulyu, a zatem vy dolzhny igrat' bez zaklinaniy, potomu chto vy sosat', GA GA GA!" : "Due-l-ue-l-ue-l!");
		drain_en(rnz(monster_difficulty() + 1) );

		if (oseen) makeknown(WAN_DRAIN_MANA);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_TIDAL_WAVE:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		pline("A sudden geyser slams into you from nowhere!");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vse promokli. Vy zhe pomnite, chtoby polozhit' vodu chuvstvitel'nyy material v konteyner, ne tak li?" : "Schwatschhhhhh!");

		if (uwep && uwep->otyp == UMBRELLA && rn2(2)) {
			pline("But your umbrella protects you from it.");
			return 2;
		}

		if ((!StrongSwimming || !rn2(10)) && (!StrongMagical_breathing || !rn2(10))) {
			water_damage(invent, FALSE, FALSE);
			if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);
		}
		if (Burned) make_burned(0L, TRUE);

		if (oseen) makeknown(WAN_TIDAL_WAVE);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_STARVATION:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		You_feel("a hole in your %s!", body_part(STOMACH) );
		if (u.uhunger > 0) morehungry(rnd(1000));
		else morehungry(rnd(200)); /* we don't want to be TOO unfair... --Amy */

		if (oseen) makeknown(WAN_STARVATION);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_CONFUSION:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		if(!Confusion) {
		    if (FunnyHallu) {
			pline("What a trippy feeling!");
		    } else if (Role_if(PM_PIRATE) || Role_if(PM_KORSAIR) || PirateSpeakOn)
			pline("Blimey! Ye're one sheet to the wind!");
			else 
			pline("Huh, What?  Where am I?");
		}
		make_confused(HConfusion + rn1(35, 115), FALSE);

		if (oseen) makeknown(WAN_CONFUSION);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_STUN_MONSTER:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		make_stunned(HStun + rn1(35, 115), TRUE);

		if (oseen) makeknown(WAN_STUN_MONSTER);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_SLIMING:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		if (!Slimed && !flaming(youmonst.data) && !Unchanging && !slime_on_touch(youmonst.data) ) {
		    You("don't feel very well.");
			stop_occupation();
			make_slimed(100);
		    killer_format = KILLED_BY_AN;
		    delayed_killer = "slimed by a wand of sliming";
		}

		if (oseen) makeknown(WAN_SLIMING);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_LYCANTHROPY:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREWOLF;
			You_feel("feverish.");
			u.cnd_lycanthropecount++;
		}

		if (oseen) makeknown(WAN_LYCANTHROPY);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_PUNISHMENT:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		punishx();

		if (oseen) makeknown(WAN_PUNISHMENT);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_SCR_PUNISHMENT:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		punishx();

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;

	case MUSE_SCR_EARTH:
	    {
		int earthradius = 1;
		if (otmp->oartifact == ART_RUMPLE_RUMPLE) earthradius = 2;

		/* TODO: handle steeds */
	    	register int x, y;
		/* don't use monster fields after killing it */
		boolean confused = (mtmp->mconf ? TRUE : FALSE);
		int mmx = mtmp->mx, mmy = mtmp->my;

		mreadmsg(mtmp, otmp);
	    	/* Identify the scroll */
		if (canspotmon(mtmp)) {
		    pline_The("%s rumbles %s %s!", ceiling(mtmp->mx, mtmp->my),
	    			otmp->blessed ? "around" : "above",
				mon_nam(mtmp));
		    if (oseen) makeknown(otmp->otyp);
		} else if (cansee(mtmp->mx, mtmp->my)) {
		    pline_The("%s rumbles in the middle of nowhere!",
			ceiling(mtmp->mx, mtmp->my));
		    if (mtmp->minvis && !(mtmp->data->msound == MS_DEEPSTATE) && !(mtmp->egotype_deepstatemember))
			map_invisible(mtmp->mx, mtmp->my);
		    if (oseen) makeknown(otmp->otyp);
		}

	    	/* Loop through the surrounding squares */
	    	for (x = mmx-earthradius; x <= mmx+earthradius; x++) {
	    	    for (y = mmy-earthradius; y <= mmy+earthradius; y++) {
	    	    	/* Is this a suitable spot? */
	    	    	if (isok(x, y) && !closed_door(x, y) &&
	    	    			!IS_ROCK(levl[x][y].typ) &&
	    	    			!IS_AIR(levl[x][y].typ) &&
	    	    			(((x == mmx) && (y == mmy)) ?
	    	    			    !otmp->blessed : !otmp->cursed) &&
					(x != u.ux || y != u.uy)) {
			    register struct obj *otmp2;
			    register struct monst *mtmp2;

	    	    	    /* Make the object(s) */
	    	    	    otmp2 = mksobj(confused ? ROCK : BOULDER, FALSE, FALSE, FALSE);
	    	    	    if (!otmp2) continue;  /* Shouldn't happen */

				if(!rn2(8)) {
					otmp2->spe = rne(2);
					if (rn2(2)) otmp2->blessed = rn2(2);
					 else	blessorcurse(otmp2, 3);
				} else if(!rn2(10)) {
					if (rn2(10)) curse(otmp2);
					 else	blessorcurse(otmp2, 3);
					otmp2->spe = -rne(2);
				} else	blessorcurse(otmp2, 10);

	    	    	    otmp2->quan = confused ? rn1(5,2) : 1;
	    	    	    otmp2->owt = weight(otmp2);

	    	    	    /* Find the monster here (might be same as mtmp) */
	    	    	    mtmp2 = m_at(x, y);
	    	    	    if (mtmp2 && !amorphous(mtmp2->data) &&
	    	    	    		!passes_walls(mtmp2->data) && (!mtmp2->egotype_wallwalk) &&
	    	    	    		!noncorporeal(mtmp2->data) &&
	    	    	    		!unsolid(mtmp2->data)) {
				struct obj *helmet = which_armor(mtmp2, W_ARMH);
				int mdmg;

				if (cansee(mtmp2->mx, mtmp2->my)) {
				    pline("%s is hit by %s!", Monnam(mtmp2),
	    	    	    			doname(otmp2));
				    if (((mtmp2->minvis && !canspotmon(mtmp2)) || mtmp2->minvisreal) && !(mtmp2->data->msound == MS_DEEPSTATE) && !(mtmp2->egotype_deepstatemember))
					map_invisible(mtmp2->mx, mtmp2->my);
				}
	    	    	    	mdmg = dmgval(otmp2, mtmp2) * otmp2->quan;
				if (helmet) {
				    if(is_metallic(helmet)) {
					if (canspotmon(mtmp2))
					    pline("Fortunately, %s is wearing a hard helmet.", mon_nam(mtmp2));
					else if (flags.soundok)
					    You_hear("a clanging sound.");
					if (mdmg > 2) mdmg = 2;
				    } else {
					if (canspotmon(mtmp2))
					    pline("%s's %s does not protect %s.",
						Monnam(mtmp2), xname(helmet),
						mhim(mtmp2));
				    }
				}
	    	    	    	mtmp2->mhp -= mdmg;
	    	    	    	if (mtmp2->mhp <= 0) {
				    pline("%s is killed.", Monnam(mtmp2));
	    	    	    	    mondied(mtmp2);
				}
	    	    	    }
	    	    	    /* Drop the rock/boulder to the floor */
	    	    	    if (!flooreffects(otmp2, x, y, "fall")) {
	    	    	    	place_object(otmp2, x, y);
	    	    	    	stackobj(otmp2);
	    	    	    	newsym(x, y);  /* map the rock */
	    	    	    }
	    	    	}
		    }
		}
		/* Attack the player */
		if (distmin(mmx, mmy, u.ux, u.uy) == 1 && !otmp->cursed) {
		    int dmg;
		    struct obj *otmp2;

		    /* Okay, _you_ write this without repeating the code */
		    otmp2 = mksobj(confused ? ROCK : BOULDER, FALSE, FALSE, FALSE);
		    if (!otmp2) goto xxx_noobj;  /* Shouldn't happen */

			if(!rn2(8)) {
				otmp2->spe = rne(2);
				if (rn2(2)) otmp2->blessed = rn2(2);
				 else	blessorcurse(otmp2, 3);
			} else if(!rn2(10)) {
				if (rn2(10)) curse(otmp2);
				 else	blessorcurse(otmp2, 3);
				otmp2->spe = -rne(2);
			} else	blessorcurse(otmp2, 10);

		    otmp2->quan = confused ? rn1(5,2) : 1;
		    otmp2->owt = weight(otmp2);
		    if (!amorphous(youmonst.data) &&
			    !Passes_walls &&
			    !noncorporeal(youmonst.data) &&
			    !unsolid(youmonst.data)) {
			You("are hit by %s!", doname(otmp2));
			dmg = dmgval(otmp2, &youmonst) * otmp2->quan;
			if (uarmh) {
			    if(is_hardmaterial(uarmh)) {
				pline("Fortunately, you are wearing a hard helmet.");
				if (dmg > 2) dmg = 2;
			    } else if (flags.verbose) {
				Your("%s does not protect you.",
						xname(uarmh));
			    }
			}
		    } else
			dmg = 0;
		    if (!flooreffects(otmp2, u.ux, u.uy, "fall")) {
			place_object(otmp2, u.ux, u.uy);
			stackobj(otmp2);
			newsym(u.ux, u.uy);
		    }
		    if (dmg) losehp(dmg, "scroll of earth", KILLED_BY_AN);
		}
xxx_noobj:

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);

		return (mtmp->mhp <= 0) ? 1 : 2;
	    }
	case MUSE_SCR_FIRE:
	      {
		boolean vis = cansee(mtmp->mx, mtmp->my);

		mreadmsg(mtmp, otmp);
		if (mtmp->mconf) {
			if (vis)
			    pline("Oh, what a pretty fire!");
		} else {
			struct monst *mtmp2;
			int num;

			if (vis)
			    pline_The("scroll erupts in a tower of flame!");
			shieldeff(mtmp->mx, mtmp->my);
			makeknown(otmp->otyp);
			if (!rn2(33)) (void) destroy_mitem(mtmp, SCROLL_CLASS, AD_FIRE);
			if (!rn2(33)) (void) destroy_mitem(mtmp, SPBOOK_CLASS, AD_FIRE);
			if (!rn2(33)) (void) destroy_mitem(mtmp, POTION_CLASS, AD_FIRE);
			num = (2*(rn1(3, 3) + 2 * bcsign(otmp)) + 1)/3;
			if (Slimed) {
			      Your("slimy parts are burned away!");
			      Slimed = 0;
			}
			burn_away_slime();
			if (Half_spell_damage && rn2(2) ) num = (num+1) / 2;
			if (StrongHalf_spell_damage && rn2(2) ) num = (num+1) / 2;
			else losehp(num, "scroll of fire", KILLED_BY_AN);
			for(mtmp2 = fmon; mtmp2; mtmp2 = mtmp2->nmon) {
			   if(DEADMONSTER(mtmp2)) continue;
			   if(mtmp == mtmp2) continue;
			   if(dist2(mtmp2->mx,mtmp2->my,mtmp->mx,mtmp->my) < 3){
				if (resists_fire(mtmp2)) continue;
				mtmp2->mhp -= num;
				if (resists_cold(mtmp2))
				    mtmp2->mhp -= 3*num;
				if(mtmp2->mhp < 1) {
				    mondied(mtmp2);
				    break;
				}
			    }
			}
		}
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);

		return 2;
	      }
	case MUSE_POT_PARALYSIS:
	case MUSE_POT_BLINDNESS:
	case MUSE_POT_CONFUSION:
	case MUSE_POT_SLEEPING:
	case MUSE_POT_ACID:
	case MUSE_POT_AMNESIA:
	case MUSE_POT_CYANIDE:
	case MUSE_POT_RADIUM:
	case MUSE_POT_HALLUCINATION:
	case MUSE_POT_ICE:
	case MUSE_POT_FEAR:
	case MUSE_POT_FIRE:
	case MUSE_POT_NITROGLYCERIN:
	case MUSE_POT_SALT_WATER:
	case MUSE_POT_DIMNESS:
	case MUSE_POT_SANITY:
	case MUSE_POT_NUMBNESS:
	case MUSE_POT_URINE:
	case MUSE_POT_SLIME:
	case MUSE_POT_CANCELLATION:
	case MUSE_POT_STUNNING:
		/* Note: this setting of dknown doesn't suffice.  A monster
		 * which is out of sight might throw and it hits something _in_
		 * sight, a problem not existing with wands because wand rays
		 * are not objects.  Also set dknown in mthrowu.c.
		 */

		if (otmp && otmp->otyp == POT_SALT_WATER && mtmp && mtmp->data == &mons[PM_CLEANER] && Race_if(PM_ELONA_SNAIL)) {
			verbalize(rn2(2) ? "Snail!" : "Kill!");
		}

		if (cansee(mtmp->mx, mtmp->my)) {
			otmp->dknown = 1;
			pline("%s hurls %s!", Monnam(mtmp),
						singular(otmp, doname));
		}
		else if (flags.soundok) You_hear("a hurling sound.");

		m_throw(mtmp, mtmp->mx, mtmp->my, sgn(mtmp->mux-mtmp->mx),
			sgn(mtmp->muy-mtmp->my),
			distmin(mtmp->mx,mtmp->my,mtmp->mux,mtmp->muy), otmp);
		return 2;
	case 0: return 0; /* i.e. an exploded wand */
	default: impossible("%s wanted to perform action %d?", Monnam(mtmp),
			m.has_offense);
		break;
	}
	return 0;
}

int
rnd_offensive_item(mtmp)
struct monst *mtmp;
{
	struct permonst *pm = mtmp->data;
	int difficulty = monstr[(monsndx(pm))];

	if((is_animal(pm) || attacktype(pm, AT_EXPL) || mindless(mtmp->data)
			|| pm->mlet == S_GHOST
			|| pm->mlet == S_KOP
		) && issoviet) return 0;
	if (difficulty > 7 && !rn2(350)) return WAN_DEATH;
	if (difficulty > 6 && !rn2(125)) return WAN_FIREBALL;
	/* Amy edit: wand of draining removed */
	switch (rn2(9 - (difficulty < 4) + 3 * (difficulty > 6))) {

		case 0: {
		    struct obj *helmet = which_armor(mtmp, W_ARMH);

		    if ((helmet && is_metallic(helmet)) || amorphous(pm) || passes_walls(pm) || (mtmp->egotype_wallwalk) || noncorporeal(pm) || unsolid(pm))
			return SCR_EARTH;
		} /* fall through */
		case 1: return WAN_STRIKING;
		case 2: return POT_ACID;
		case 3: return POT_CONFUSION;
		case 4: return POT_BLINDNESS;
		case 5: return POT_SLEEPING;
		case 6: return POT_PARALYSIS;
		case 7: return WAN_MAGIC_MISSILE;
		case 8: return WAN_SLEEP;
		case 9: return WAN_FIRE;
		case 10: return WAN_COLD;
		case 11: return WAN_LIGHTNING;
		/*case 12: return WAN_DRAINING;*/
	}
	/*NOTREACHED*/
	return 0;
}

int
rnd_offensive_item_new(mtmp)
/* edit by Amy: keep rnd_offensive_potion() (see below) updated when adding new potion types */
struct monst *mtmp;
{
	struct permonst *pm = mtmp->data;

	if((is_animal(pm) || attacktype(pm, AT_EXPL) || mindless(mtmp->data)
			|| pm->mlet == S_GHOST
			|| pm->mlet == S_KOP
		) && issoviet) return 0;
	switch (rn2(269)) {

		case 0: return WAN_DEATH;
		case 1: return WAN_SLEEP;
		case 2: return WAN_FIREBALL;
		case 3: return WAN_FIRE;
		case 4: return WAN_COLD;
		case 5: return WAN_LIGHTNING;
		case 6: return WAN_MAGIC_MISSILE;
		case 7: return WAN_STRIKING;
		case 8: return SCR_FIRE;
		case 9: return POT_PARALYSIS;
		case 10: return POT_BLINDNESS;
		case 11: return POT_CONFUSION;
		case 12: return POT_SLEEPING;
		case 13: return POT_ACID;
		case 14: return FROST_HORN;
		case 15: return FIRE_HORN;
		case 16: return WAN_DRAINING;
		case 17: return SCR_EARTH;
		case 18: return POT_AMNESIA;
		case 19: return WAN_CANCELLATION;
		case 20: return POT_CYANIDE;
		case 21: return POT_RADIUM;
		case 22: return WAN_ACID;
		case 23: return SCR_TRAP_CREATION;
		case 24: return WAN_TRAP_CREATION;
		case 25: return SCR_FLOOD;
		case 26: return SCR_LAVA;
		case 27: return SCR_GROWTH;
		case 28: return SCR_ICE;
		case 29: return SCR_CLOUDS;
		case 30: return SCR_BARRHING;
		case 31: return WAN_SOLAR_BEAM;
		case 32: return SCR_LOCKOUT;
		case 33: return (rn2(10) ? WAN_TELEPORTATION : WAN_BANISHMENT);
		case 34: return POT_HALLUCINATION;
		case 35: return POT_NUMBNESS;
		case 36: return POT_ICE;
		case 37: return POT_STUNNING;
		case 38: return SCR_BAD_EFFECT;
		case 39: return WAN_BAD_EFFECT;
		case 40: return POT_FIRE;
		case 41: return WAN_SLOW_MONSTER;
		case 42: return WAN_FEAR;
		case 43: return POT_FEAR;
		case 44: return SCR_DESTROY_ARMOR;
		case 45: return SCR_STONING;
		case 46: return POT_URINE;
		case 47: return POT_SLIME;
		case 48: return POT_CANCELLATION;
		case 49: return WAN_STONING;
		case 50: return WAN_DISINTEGRATION;
		case 51: return WAN_PARALYSIS;
		case 52: return WAN_CURSE_ITEMS;
		case 53: return WAN_AMNESIA;
		case 54: return WAN_BAD_LUCK;
		case 55: return WAN_REMOVE_RESISTANCE;
		case 56: return WAN_CORROSION;
		case 57: return WAN_FUMBLING;
		case 58: return WAN_STARVATION;
		case 59: return WAN_PUNISHMENT;
		case 60: return SCR_PUNISHMENT;
		case 61: return WAN_MAKE_VISIBLE;
		case 62: return WAN_REDUCE_MAX_HITPOINTS;
		case 63: return WAN_CONFUSION;
		case 64: return WAN_SLIMING;
		case 65: return WAN_LYCANTHROPY;
		case 66: return SCR_CHAOS_TERRAIN;
		case 67: return SCR_WOUNDS;
		case 68: return SCR_BULLSHIT;
		case 69: return SCR_AMNESIA;
		case 70: return WAN_SUMMON_SEXY_GIRL;
		case 71: return SCR_DEMONOLOGY;
		case 72: return SCR_NASTINESS;
		case 73: return SCR_GIRLINESS;
		case 74: return SCR_ELEMENTALISM;
		case 75: return TEMPEST_HORN;
		case 76: return WAN_POISON;
		case 77: return SCR_DESTROY_WEAPON;
		case 78: return WAN_DISINTEGRATION_BEAM;
		case 79: return WAN_CHROMATIC_BEAM;
		case 80: return WAN_STUN_MONSTER;
		case 81: return SCR_MEGALOAD;
		case 82: return SCR_ENRAGE;
		case 83: return WAN_TIDAL_WAVE;
		case 84: return SCR_ANTIMATTER;
		case 85: return WAN_DRAIN_MANA;
		case 86: return WAN_FINGER_BENDING;
		case 87: return SCR_IMMOBILITY;
		case 88: return WAN_IMMOBILITY;
		case 89: return SCR_FLOODING;
		case 90: return SCR_EGOISM;
		case 91: return WAN_EGOISM;
		case 92: return SCR_RUMOR;
		case 93: return SCR_MESSAGE;
		case 94: return SCR_SIN;
		case 95: return WAN_SIN;
		case 96: return WAN_INERTIA;
		case 97: return WAN_TIME;
		case 98: return WAN_LEVITATION;
		case 99: return WAN_PSYBEAM;
		case 100: return WAN_HYPER_BEAM;
		case 101: return WAN_STRIKING;
		case 102: return POT_ACID;
		case 103: return POT_CONFUSION;
		case 104: return POT_BLINDNESS;
		case 105: return POT_SLEEPING;
		case 106: return POT_PARALYSIS;
		case 107: return WAN_MAGIC_MISSILE;
		case 108: return WAN_SLEEP;
		case 109: return WAN_FIRE;
		case 110: return WAN_COLD;
		case 111: return WAN_STRIKING;
		case 112: return POT_ACID;
		case 113: return POT_CONFUSION;
		case 114: return POT_BLINDNESS;
		case 115: return POT_SLEEPING;
		case 116: return POT_PARALYSIS;
		case 117: return WAN_MAGIC_MISSILE;
		case 118: return WAN_SLEEP;
		case 119: return WAN_FIRE;
		case 120: return WAN_COLD;
		case 121: return WAN_STRIKING;
		case 122: return POT_ACID;
		case 123: return POT_CONFUSION;
		case 124: return POT_BLINDNESS;
		case 125: return POT_SLEEPING;
		case 126: return POT_PARALYSIS;
		case 127: return WAN_MAGIC_MISSILE;
		case 128: return WAN_SLEEP;
		case 129: return WAN_FIRE;
		case 130: return WAN_COLD;
		case 131: return WAN_STRIKING;
		case 132: return POT_ACID;
		case 133: return POT_CONFUSION;
		case 134: return POT_BLINDNESS;
		case 135: return POT_SLEEPING;
		case 136: return POT_PARALYSIS;
		case 137: return WAN_MAGIC_MISSILE;
		case 138: return WAN_SLEEP;
		case 139: return WAN_FIRE;
		case 140: return WAN_COLD;
		case 141: return WAN_STRIKING;
		case 142: return POT_ACID;
		case 143: return POT_CONFUSION;
		case 144: return POT_BLINDNESS;
		case 145: return POT_SLEEPING;
		case 146: return POT_PARALYSIS;
		case 147: return WAN_MAGIC_MISSILE;
		case 148: return WAN_SLEEP;
		case 149: return WAN_FIRE;
		case 150: return WAN_COLD;
		case 151: return WAN_LIGHTNING;
		case 152: return WAN_LIGHTNING;
		case 153: return WAN_LIGHTNING;
		case 154: return WAN_LIGHTNING;
		case 155: return WAN_LIGHTNING;
		case 156: return POT_HALLUCINATION;
		case 157: return POT_NUMBNESS;
		case 158: return POT_ICE;
		case 159: return POT_STUNNING;
		case 160: return SCR_BAD_EFFECT;
		case 161: return POT_FIRE;
		case 162: return POT_FEAR;
		case 163: return POT_HALLUCINATION;
		case 164: return POT_NUMBNESS;
		case 165: return POT_ICE;
		case 166: return POT_STUNNING;
		case 167: return SCR_BAD_EFFECT;
		case 168: return POT_FIRE;
		case 169: return POT_FEAR;
		case 170: return POT_HALLUCINATION;
		case 171: return POT_NUMBNESS;
		case 172: return POT_ICE;
		case 173: return POT_STUNNING;
		case 174: return SCR_BAD_EFFECT;
		case 175: return POT_FIRE;
		case 176: return POT_FEAR;
		case 177: return POT_HALLUCINATION;
		case 178: return POT_NUMBNESS;
		case 179: return POT_ICE;
		case 180: return POT_STUNNING;
		case 181: return SCR_BAD_EFFECT;
		case 182: return POT_FIRE;
		case 183: return POT_FEAR;
		case 184: return POT_HALLUCINATION;
		case 185: return POT_NUMBNESS;
		case 186: return POT_ICE;
		case 187: return POT_STUNNING;
		case 188: return SCR_BAD_EFFECT;
		case 189: return POT_FIRE;
		case 190: return POT_FEAR;
		case 191: return WAN_STRIKING;
		case 192: return POT_ACID;
		case 193: return POT_CONFUSION;
		case 194: return POT_BLINDNESS;
		case 195: return POT_SLEEPING;
		case 196: return POT_PARALYSIS;
		case 197: return WAN_MAGIC_MISSILE;
		case 198: return WAN_SLEEP;
		case 199: return WAN_FIRE;
		case 200: return WAN_COLD;
		case 201: return WAN_LIGHTNING;
		case 202: return WAN_INFERNO;
		case 203: return WAN_ICE_BEAM;
		case 204: return WAN_THUNDER;
		case 205: return WAN_SLUDGE;
		case 206: return WAN_TOXIC;
		case 207: return WAN_NETHER_BEAM;
		case 208: return WAN_AURORA_BEAM;
		case 209: return WAN_GRAVITY_BEAM;
		case 210: return WAN_CHLOROFORM;
		case 211: return WAN_DREAM_EATER;
		case 212: return WAN_BUBBLEBEAM;
		case 213: return WAN_GOOD_NIGHT;
		case 214: return SCR_VILENESS;
		case 215: return POT_DIMNESS;
		case 216: return POT_DIMNESS;
		case 217: return POT_DIMNESS;
		case 218: return POT_DIMNESS;
		case 219: return POT_DIMNESS;
		case 220: return POT_DIMNESS;
		case 221: return POT_DIMNESS;
		case 222: return SCR_OFFLEVEL_ITEM;
		case 223: return POT_SANITY;
		case 224: return POT_SANITY;
		case 225: return POT_SANITY;
		case 226: return POT_SANITY;
		case 227: return SCR_BAD_EQUIPMENT;
		case 228: return WAN_INSANITY;
		case 229: return WAN_BAD_EQUIPMENT;
		case 230: return SCR_NASTY_CURSE;
		case 231: return SCR_HYBRIDIZATION;
		case 232: return WAN_DISENCHANTMENT;
		case 233: return WAN_TREMBLING;
		case 234: return WAN_CONTAMINATION;
		case 235: return SCR_GRAVE;
		case 236: return SCR_TUNNELS;
		case 237: return SCR_FARMING;
		case 238: return SCR_MOUNTAINS;
		case 239: return SCR_DIVING;
		case 240: return SCR_CRYSTALLIZATION;
		case 241: return SCR_MOORLAND;
		case 242: return SCR_URINE;
		case 243: return SCR_QUICKSAND;
		case 244: return SCR_STYX;
		case 245: return SCR_SNOW;
		case 246: return SCR_ASH;
		case 247: return SCR_SAND;
		case 248: return SCR_PAVING;
		case 249: return SCR_HIGHWAY;
		case 250: return SCR_GRASSLAND;
		case 251: return SCR_NETHER;
		case 252: return SCR_STALACTITE;
		case 253: return SCR_CRYPT;
		case 254: return SCR_BUBBLE_BOBBLE;
		case 255: return SCR_RAIN;
		case 256: return WAN_CHAOS_TERRAIN;
		case 257: return WAN_FLEECY_TERRAIN;
		case 258: return ETHER_HORN;
		case 259: return SHADOW_HORN;
		case 260: return CHROME_HORN;
		case 261: return SCR_ILLUSION;
		case 262: return SCR_EVIL_VARIANT;
		case 263: return SCR_FEMINISM;
		case 264: return WAN_BLEEDING;
		case 265: return WAN_UNDRESSING;
		case 266: return WAN_STAT_REDUCTION;
		case 267: return SCR_VISIBLE_ITEM;
		case 268: return DEATH_HORN;

	}
	/*NOTREACHED*/
	return 0;
}

int
rnd_offensive_potion(mtmp)
struct monst *mtmp;
{
	struct permonst *pm = mtmp->data;

	switch (rn2(88)) {

		case 0: return POT_PARALYSIS;
		case 1: return POT_BLINDNESS;
		case 2: return POT_CONFUSION;
		case 3: return POT_SLEEPING;
		case 4: return POT_ACID;
		case 5: return POT_AMNESIA;
		case 6: return POT_CYANIDE;
		case 7: return POT_RADIUM;
		case 8: return POT_HALLUCINATION;
		case 9: return POT_NUMBNESS;
		case 10: return POT_ICE;
		case 11: return POT_STUNNING;
		case 12: return POT_FIRE;
		case 13: return POT_FEAR;
		case 14: return POT_URINE;
		case 15: return POT_SLIME;
		case 16: return POT_CANCELLATION;
		case 17: return POT_ACID;
		case 18: return POT_CONFUSION;
		case 19: return POT_BLINDNESS;
		case 20: return POT_SLEEPING;
		case 21: return POT_PARALYSIS;
		case 22: return POT_ACID;
		case 23: return POT_CONFUSION;
		case 24: return POT_BLINDNESS;
		case 25: return POT_SLEEPING;
		case 26: return POT_PARALYSIS;
		case 27: return POT_ACID;
		case 28: return POT_CONFUSION;
		case 29: return POT_BLINDNESS;
		case 30: return POT_SLEEPING;
		case 31: return POT_PARALYSIS;
		case 32: return POT_ACID;
		case 33: return POT_CONFUSION;
		case 34: return POT_BLINDNESS;
		case 35: return POT_SLEEPING;
		case 36: return POT_PARALYSIS;
		case 37: return POT_ACID;
		case 38: return POT_CONFUSION;
		case 39: return POT_BLINDNESS;
		case 40: return POT_SLEEPING;
		case 41: return POT_PARALYSIS;
		case 42: return POT_HALLUCINATION;
		case 43: return POT_NUMBNESS;
		case 44: return POT_ICE;
		case 45: return POT_STUNNING;
		case 46: return POT_FIRE;
		case 47: return POT_FEAR;
		case 48: return POT_HALLUCINATION;
		case 49: return POT_NUMBNESS;
		case 50: return POT_ICE;
		case 51: return POT_STUNNING;
		case 52: return POT_FIRE;
		case 53: return POT_FEAR;
		case 54: return POT_HALLUCINATION;
		case 55: return POT_NUMBNESS;
		case 56: return POT_ICE;
		case 57: return POT_STUNNING;
		case 58: return POT_FIRE;
		case 59: return POT_FEAR;
		case 60: return POT_HALLUCINATION;
		case 61: return POT_NUMBNESS;
		case 62: return POT_ICE;
		case 63: return POT_STUNNING;
		case 64: return POT_FIRE;
		case 65: return POT_FEAR;
		case 66: return POT_HALLUCINATION;
		case 67: return POT_NUMBNESS;
		case 68: return POT_ICE;
		case 69: return POT_STUNNING;
		case 70: return POT_FIRE;
		case 71: return POT_FEAR;
		case 72: return POT_ACID;
		case 73: return POT_CONFUSION;
		case 74: return POT_BLINDNESS;
		case 75: return POT_SLEEPING;
		case 76: return POT_PARALYSIS;
		case 77: return POT_DIMNESS;
		case 78: return POT_DIMNESS;
		case 79: return POT_DIMNESS;
		case 80: return POT_DIMNESS;
		case 81: return POT_DIMNESS;
		case 82: return POT_DIMNESS;
		case 83: return POT_DIMNESS;
		case 84: return POT_SANITY;
		case 85: return POT_SANITY;
		case 86: return POT_SANITY;
		case 87: return POT_SANITY;

	}
	/*NOTREACHED*/
	return 0;
}

#define MUSE_POT_GAIN_LEVEL 1
#define MUSE_WAN_MAKE_INVISIBLE 2
#define MUSE_POT_INVISIBILITY 3
#define MUSE_POLY_TRAP 4
#define MUSE_WAN_POLYMORPH 5
#define MUSE_POT_SPEED 6
#define MUSE_WAN_SPEED_MONSTER 7
#define MUSE_BULLWHIP 8
#define MUSE_POT_POLYMORPH 9
#define MUSE_WAN_CLONE_MONSTER 10
#define MUSE_WAN_HASTE_MONSTER 11
#define MUSE_POT_MUTATION 12
#define MUSE_WAN_MUTATION 13
#define MUSE_WAN_GAIN_LEVEL 14
#define MUSE_WAN_INCREASE_MAX_HITPOINTS 15
#define MUSE_SCR_SUMMON_BOSS_M 16
#define MUSE_SCR_CREATE_MONSTER_M 17
#define MUSE_BAG_OF_TRICKS_M 18
#define MUSE_WAN_CREATE_MONSTER_M 19
#define MUSE_WAN_SUMMON_UNDEAD_M 20
#define MUSE_WAN_CREATE_HORDE_M 21
#define MUSE_SCR_SUMMON_UNDEAD_M 22
#define MUSE_SCR_CREATE_VICTIM_M 23
#define MUSE_SCR_GROUP_SUMMONING_M 24
#define MUSE_SCR_SUMMON_GHOST_M 25
#define MUSE_SCR_SUMMON_ELM_M 26
#define MUSE_WAN_SUMMON_ELM_M 27
#define MUSE_MAGIC_TRAP 28
#define MUSE_POT_GAIN_ENERGY 29

boolean
find_misc(mtmp)
struct monst *mtmp;
{
	register struct obj *obj;
	struct permonst *mdat = mtmp->data;
	int x = mtmp->mx, y = mtmp->my;
	struct trap *t;
	int xx, yy;
	boolean immobile = (mdat->mmove == 0 || mtmp == u.usteed);
	boolean stuck = (mtmp == u.ustuck);

	m.misc = (struct obj *)0;
	m.has_misc = 0;

	if (!issoviet && !rn2(5)) return FALSE;

	if ((is_animal(mdat) || mindless(mdat)) && issoviet)
		return 0;
	if (u.uswallow && stuck) return FALSE;

	/* We arbitrarily limit to times when a player is nearby for the
	 * same reason as Junior Pac-Man doesn't have energizers eaten until
	 * you can see them...
	 */
	if(dist2(x, y, mtmp->mux, mtmp->muy) > 36 && rn2(dist2(x, y, mtmp->mux, mtmp->muy) - 36) )
		return FALSE;

	if (!stuck && !immobile && !mtmp->cham && monstr[monsndx(mdat)] < 6) {
	  boolean ignore_boulders = (verysmall(mdat) ||
				     throws_rocks(mdat) || (mtmp->egotype_wallwalk) ||
				     passes_walls(mdat));
	  for(xx = x-1; xx <= x+1; xx++)
	    for(yy = y-1; yy <= y+1; yy++)
		if (isok(xx,yy) && (xx != u.ux || yy != u.uy))
		    if (!(isgridbug(mdat)) || xx == x || yy == y)
			if (/* (xx==x && yy==y) || */ !level.monsters[xx][yy])
			    if ((t = t_at(xx, yy)) != 0 &&
			      (ignore_boulders || !sobj_at(BOULDER, xx, yy))
			      && !onscary(xx, yy, mtmp)) {
				if (t->ttyp == POLY_TRAP) {
				    trapx = xx;
				    trapy = yy;
				    m.has_misc = MUSE_POLY_TRAP;
				    return TRUE;
				}
			    }
	}

	if (!stuck && !immobile && mtmp->mcan) {
	  boolean ignore_boulders = (verysmall(mdat) ||
				     throws_rocks(mdat) || (mtmp->egotype_wallwalk) ||
				     passes_walls(mdat));
	  for(xx = x-1; xx <= x+1; xx++)
	    for(yy = y-1; yy <= y+1; yy++)
		if (isok(xx,yy) && (xx != u.ux || yy != u.uy))
		    if (!(isgridbug(mdat)) || xx == x || yy == y)
			if (/* (xx==x && yy==y) || */ !level.monsters[xx][yy])
			    if ((t = t_at(xx, yy)) != 0 &&
			      (ignore_boulders || !sobj_at(BOULDER, xx, yy))
			      && !onscary(xx, yy, mtmp)) {
				if (t->ttyp == MAGIC_TRAP) {
				    trapx = xx;
				    trapy = yy;
				    m.has_misc = MUSE_MAGIC_TRAP;
				    return TRUE;
				}
			    }
	}

	/*if (nohands(mdat))
		return 0;*/

#define nomore(x) if(m.has_misc==x) continue;
	for(obj=mtmp->minvent; obj; obj=obj->nobj) {

		if (m.has_misc && rn2(2)) break; /* don't always use the same misc pattern --Amy */

		/* Monsters shouldn't recognize cursed items; this kludge is */
		/* necessary to prevent serious problems though... */
		if(obj->otyp == POT_GAIN_LEVEL && (!obj->cursed ||
			    (!mtmp->isgd && !mtmp->isshk && !mtmp->ispriest))) {
			m.misc = obj;
			m.has_misc = MUSE_POT_GAIN_LEVEL;
		}
		nomore(MUSE_BULLWHIP);
/*  WAC kludge here so monsters don't attempt to grab cursed weapon */
		if((objects[obj->otyp].oc_skill == P_WHIP) && (obj->otyp != RUBBER_HOSE) && (MON_WEP(mtmp) == obj) &&
		   distu(mtmp->mx,mtmp->my) == 1 && uwep && !uwep->cursed &&
		   !mtmp->mpeaceful) {
			m.misc = obj;
			m.has_misc = MUSE_BULLWHIP;
		}
		/* Note: peaceful/tame monsters won't make themselves
		 * invisible unless you can see them.  Not really right, but...
		 */
		nomore(MUSE_WAN_MAKE_INVISIBLE);
		if(obj->otyp == WAN_MAKE_INVISIBLE && obj->spe > 0 &&
		    !mtmp->minvis && !mtmp->invis_blkd &&
		    (!mtmp->mpeaceful || (See_invisible && mtmp->seeinvisble) || StrongSee_invisible) &&
		    (!attacktype(mtmp->data, AT_GAZE) || mtmp->mcan)) {
			m.misc = obj;
			m.has_misc = MUSE_WAN_MAKE_INVISIBLE;
		}
		nomore(MUSE_POT_INVISIBILITY);
		if(obj->otyp == POT_INVISIBILITY &&
		    !mtmp->minvis && !mtmp->invis_blkd &&
		    (!mtmp->mpeaceful || (See_invisible && mtmp->seeinvisble) || StrongSee_invisible) &&
		    (!attacktype(mtmp->data, AT_GAZE) || mtmp->mcan)) {
			m.misc = obj;
			m.has_misc = MUSE_POT_INVISIBILITY;
		}
		nomore(MUSE_WAN_SPEED_MONSTER);
		if(obj->otyp == WAN_SPEED_MONSTER && obj->spe > 0
				&& mtmp->mspeed != MFAST && !mtmp->isgd) {
			m.misc = obj;
			m.has_misc = MUSE_WAN_SPEED_MONSTER;
		}
		nomore(MUSE_WAN_HASTE_MONSTER);
		if(obj->otyp == WAN_HASTE_MONSTER && obj->spe > 0
				&& mtmp->mspeed != MFAST && !mtmp->isgd) {
			m.misc = obj;
			m.has_misc = MUSE_WAN_HASTE_MONSTER;
		}
		nomore(MUSE_POT_SPEED);
		if(obj->otyp == POT_SPEED && mtmp->mspeed != MFAST
							&& !mtmp->isgd) {
			m.misc = obj;
			m.has_misc = MUSE_POT_SPEED;
		}
		nomore(MUSE_WAN_POLYMORPH);	/* will also be used if high-level monster is low on health --Amy */
		if(obj->otyp == WAN_POLYMORPH && obj->spe > 0 && !mtmp->cham
				&& ((monstr[monsndx(mdat)] < 6) || (mtmp->mhp*3 < mtmp->mhpmax) ) ) {
			m.misc = obj;
			m.has_misc = MUSE_WAN_POLYMORPH;
		}
		nomore(MUSE_WAN_CLONE_MONSTER);
		if(obj->otyp == WAN_CLONE_MONSTER && obj->spe > 0 && mtmp->mhp > 1) {
			m.misc = obj;
			m.has_misc = MUSE_WAN_CLONE_MONSTER;
		}
		nomore(MUSE_POT_POLYMORPH);	/* will also be used if high-level monster is low on health --Amy */
		if(obj->otyp == POT_POLYMORPH && !mtmp->cham
				&& ((monstr[monsndx(mdat)] < 6) || (mtmp->mhp*3 < mtmp->mhpmax) ) ) {
			m.misc = obj;
			m.has_misc = MUSE_POT_POLYMORPH;
		}
		nomore(MUSE_POT_GAIN_ENERGY);
		if(obj->otyp == POT_GAIN_ENERGY && mtmp->mcan) {
			m.misc = obj;
			m.has_misc = MUSE_POT_GAIN_ENERGY;
		}
		nomore(MUSE_POT_MUTATION);
		if(obj->otyp == POT_MUTATION) {
			m.misc = obj;
			m.has_misc = MUSE_POT_MUTATION;
		}
		nomore(MUSE_WAN_MUTATION);
		if(obj->otyp == WAN_MUTATION && obj->spe > 0) {
			m.misc = obj;
			m.has_misc = MUSE_WAN_MUTATION;
		}
		nomore(MUSE_WAN_GAIN_LEVEL);
		if(obj->otyp == WAN_GAIN_LEVEL && obj->spe > 0) {
			m.misc = obj;
			m.has_misc = MUSE_WAN_GAIN_LEVEL;
		}
		nomore(MUSE_WAN_INCREASE_MAX_HITPOINTS);
		if(obj->otyp == WAN_INCREASE_MAX_HITPOINTS && obj->spe > 0) {
			m.misc = obj;
			m.has_misc = MUSE_WAN_INCREASE_MAX_HITPOINTS;
		}
		nomore(MUSE_SCR_SUMMON_BOSS_M);
		if(obj->otyp == SCR_SUMMON_BOSS && !rn2(25) && !mtmp->mpeaceful) {
			m.misc = obj;
			m.has_misc = MUSE_SCR_SUMMON_BOSS_M;
		}
		nomore(MUSE_SCR_SUMMON_GHOST_M);
		if(obj->otyp == SCR_SUMMON_GHOST && !rn2(25) && !mtmp->mpeaceful) {
			m.misc = obj;
			m.has_misc = MUSE_SCR_SUMMON_GHOST_M;
		}
		nomore(MUSE_SCR_SUMMON_ELM_M);
		if(obj->otyp == SCR_SUMMON_ELM && !rn2(25) && !mtmp->mpeaceful) {
			m.misc = obj;
			m.has_misc = MUSE_SCR_SUMMON_ELM_M;
		}
		nomore(MUSE_SCR_CREATE_MONSTER_M);
		if(obj->otyp == SCR_CREATE_MONSTER && !rn2(25) && !mtmp->mpeaceful ) {
			m.misc = obj;
			m.has_misc = MUSE_SCR_CREATE_MONSTER_M;
		}
		nomore(MUSE_SCR_CREATE_VICTIM_M);
		if(obj->otyp == SCR_CREATE_VICTIM && !rn2(25) && !mtmp->mpeaceful ) {
			m.misc = obj;
			m.has_misc = MUSE_SCR_CREATE_VICTIM_M;
		}
		nomore(MUSE_BAG_OF_TRICKS_M);
		if(obj->otyp == BAG_OF_TRICKS && obj->spe > 0 && !rn2(25) && !mtmp->mpeaceful ) {
			m.misc = obj;
			m.has_misc = MUSE_BAG_OF_TRICKS_M;
		}
		nomore(MUSE_WAN_CREATE_MONSTER_M);
		if (obj->otyp == WAN_CREATE_MONSTER && obj->spe > 0 && !rn2(25) && !mtmp->mpeaceful ) {
			m.misc = obj;
			m.has_misc = MUSE_WAN_CREATE_MONSTER_M;
		}
		nomore(MUSE_WAN_SUMMON_UNDEAD_M);
		if (obj->otyp == WAN_SUMMON_UNDEAD && obj->spe > 0 && !rn2(25) && !mtmp->mpeaceful ) {
			m.misc = obj;
			m.has_misc = MUSE_WAN_SUMMON_UNDEAD_M;
		}
		nomore(MUSE_WAN_SUMMON_ELM_M);
		if (obj->otyp == WAN_SUMMON_ELM && obj->spe > 0 && !rn2(25) && !mtmp->mpeaceful ) {
			m.misc = obj;
			m.has_misc = MUSE_WAN_SUMMON_ELM_M;
		}
		nomore(MUSE_WAN_CREATE_HORDE_M);
		if(obj->otyp == WAN_CREATE_HORDE && obj->spe > 0 && !rn2(25) && !mtmp->mpeaceful ) {
			m.misc = obj;
			m.has_misc = MUSE_WAN_CREATE_HORDE_M;
		}
		nomore(MUSE_SCR_SUMMON_UNDEAD_M);
		if(obj->otyp == SCR_SUMMON_UNDEAD && !rn2(25) && !mtmp->mpeaceful ) {
			m.misc = obj;
			m.has_misc = MUSE_SCR_SUMMON_UNDEAD_M;
		}
		nomore(MUSE_SCR_GROUP_SUMMONING_M);
		if(obj->otyp == SCR_GROUP_SUMMONING && !rn2(25) && !mtmp->mpeaceful ) {
			m.misc = obj;
			m.has_misc = MUSE_SCR_GROUP_SUMMONING_M;
		}

	}
	return((boolean)(!!m.has_misc));
#undef nomore
}

#if 0
/* type of monster to polymorph into; defaults to one suitable for the
   current level rather than the totally arbitrary choice of newcham() */
static struct permonst *
muse_newcham_mon(mon)
struct monst *mon;
{
	struct obj *m_armr;

	if ((m_armr = which_armor(mon, W_ARM)) != 0) {
	    if (Is_dragon_scales(m_armr))
		return Dragon_scales_to_pm(m_armr);
	    else if (Is_dragon_mail(m_armr))
		return Dragon_mail_to_pm(m_armr);
	}
	return rndmonst();
}
#endif

int
use_misc(mtmp)
struct monst *mtmp;
{
	int i;
	struct obj *otmp = m.misc;
	boolean vis, vismon, oseen;
	char nambuf[BUFSZ];

	if ((i = precheck(mtmp, otmp)) != 0) return i;
	vis = cansee(mtmp->mx, mtmp->my);
	vismon = canseemon(mtmp);
	oseen = otmp && vismon;

	switch(m.has_misc) {
	case MUSE_POT_GAIN_LEVEL:
		mquaffmsg(mtmp, otmp);
		if (otmp->cursed) {
		    if (Can_rise_up(mtmp->mx, mtmp->my, &u.uz)) {
			register int tolev = depth(&u.uz)-1;
			d_level tolevel;

			get_level(&tolevel, tolev);
			/* insurance against future changes... */
			if(on_level(&tolevel, &u.uz)) goto skipmsg;
			if (vismon) {
			    pline("%s rises up, through the %s!",
				  Monnam(mtmp), ceiling(mtmp->mx, mtmp->my));
			    if(!objects[POT_GAIN_LEVEL].oc_name_known
			      && !objects[POT_GAIN_LEVEL].oc_uname)
				docall(otmp);
			}
			m_useup(mtmp, otmp);
			migrate_to_level(mtmp, ledger_no(&tolevel),
					 MIGR_RANDOM, (coord *)0);
			return 2;
		    } else {
skipmsg:
			if (vismon) {
			    pline("%s looks uneasy.", Monnam(mtmp));
			    if(!objects[POT_GAIN_LEVEL].oc_name_known
			      && !objects[POT_GAIN_LEVEL].oc_uname)
				docall(otmp);
			}
			m_useup(mtmp, otmp);
			return 2;
		    }
		}
		if (vismon) pline("%s seems more experienced.", Monnam(mtmp));
		if (oseen) makeknown(POT_GAIN_LEVEL);
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		if (!grow_up(mtmp,(struct monst *)0)) return 1;
			/* grew into genocided monster */
		return 2;
	case MUSE_WAN_MAKE_INVISIBLE:
	case MUSE_POT_INVISIBILITY:
		if (otmp->otyp == WAN_MAKE_INVISIBLE) {
		    mzapmsg(mtmp, otmp, TRUE);
		    if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		} else
		    mquaffmsg(mtmp, otmp);
		/* format monster's name before altering its visibility */
		strcpy(nambuf, ((See_invisible && mtmp->seeinvisble) || StrongSee_invisible) ? Monnam(mtmp) : mon_nam(mtmp));
		mon_set_minvis(mtmp);
		if (vismon && mtmp->minvis) {	/* was seen, now invisible */
		    if ((See_invisible && mtmp->seeinvisble) || StrongSee_invisible)
			pline("%s body takes on a %s transparency.",
			      s_suffix(nambuf),
			      FunnyHallu ? "normal" : "strange");
		    else
			pline("Suddenly you cannot see %s.", nambuf);
		    if (oseen) makeknown(otmp->otyp);
		}
		if (otmp->otyp == POT_INVISIBILITY) {
		    if (otmp->cursed) you_aggravate(mtmp);
		    if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
			return 2;
		}
		if (otmp->otyp == WAN_MAKE_INVISIBLE) {
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
			return 2;
		}
		return 2;
	case MUSE_WAN_SPEED_MONSTER:
	case MUSE_WAN_HASTE_MONSTER:
		mzapmsg(mtmp, otmp, TRUE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		mon_adjust_speed(mtmp, 1, otmp);
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_GAIN_LEVEL:

		mzapmsg(mtmp, otmp, TRUE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		if (vismon) pline("%s seems more experienced.", Monnam(mtmp));
		if (oseen) makeknown(WAN_GAIN_LEVEL);
		if (!grow_up(mtmp,(struct monst *)0)) return 1;
			/* grew into genocided monster */

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_INCREASE_MAX_HITPOINTS:

		mzapmsg(mtmp, otmp, TRUE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		if (vismon) pline("%s seems stronger.", Monnam(mtmp));
		if (oseen) makeknown(WAN_INCREASE_MAX_HITPOINTS);

		mtmp->mhp += rnd(8);
		mtmp->mhpmax += rnd(8);
		if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_POT_SPEED:
		mquaffmsg(mtmp, otmp);
		/* note difference in potion effect due to substantially
		   different methods of maintaining speed ratings:
		   player's character becomes "very fast" temporarily;
		   monster becomes "one stage faster" permanently */
		mon_adjust_speed(mtmp, 1, otmp);
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;
	case MUSE_WAN_POLYMORPH:
		mzapmsg(mtmp, otmp, TRUE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		if (otmp->oartifact == ART_DIKKIN_S_DEADLIGHT) YellowSpells += rnz(10 * (monster_difficulty() + 1));
		if (mtmp->mhp < mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;
#if 0
		(void) newcham(mtmp, muse_newcham_mon(), TRUE, vismon);
#else
		(void) mon_poly(mtmp, FALSE, "%s changes!");
#endif
		if (oseen) makeknown(WAN_POLYMORPH);
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;
	case MUSE_WAN_CLONE_MONSTER:
		mzapmsg(mtmp, otmp, TRUE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
	    clone_mon(mtmp, 0, 0);
		if (oseen) makeknown(WAN_CLONE_MONSTER);
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;
	case MUSE_POT_POLYMORPH:
		mquaffmsg(mtmp, otmp);
		if (mtmp->mhp < mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;
#if 0
		if (vismon) pline("%s suddenly mutates!", Monnam(mtmp));
		(void) newcham(mtmp, muse_newcham_mon(mtmp), FALSE, vismon);
#else
		(void) mon_poly(mtmp, FALSE, "%s suddenly mutates!");
#endif
		if (oseen) makeknown(POT_POLYMORPH);
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;
	case MUSE_POT_GAIN_ENERGY:
		mquaffmsg(mtmp, otmp);
		mtmp->mcan = 0;
		if (vismon) pline("%s looks filled with power!", Monnam(mtmp));

		if (oseen) makeknown(POT_GAIN_ENERGY);
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;
	case MUSE_POT_MUTATION:
		mquaffmsg(mtmp, otmp);

		add_monster_egotype(mtmp);

		if (oseen) makeknown(POT_MUTATION);
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_MUTATION:
		mzapmsg(mtmp, otmp, TRUE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		add_monster_egotype(mtmp);

		if (oseen) makeknown(WAN_MUTATION);
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_SCR_SUMMON_BOSS_M:

	    {	coord cc;
		struct permonst *pm = 0;
		struct monst *mon;
		boolean known = FALSE;
		int attempts = 0;

		if (!enexto(&cc, mtmp->mx, mtmp->my, 0)) break;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		mreadmsg(mtmp, otmp);

newboss:
		do {
			pm = rndmonst();
			attempts++;
			if (attempts && (attempts % 10000 == 0)) u.mondiffhack++;
			if (!rn2(2000)) reset_rndmonst(NON_PM);

		} while ( (!pm || (pm && !(pm->geno & G_UNIQ))) && attempts < 50000);

		if (!pm && rn2(50) ) {
			attempts = 0;
			goto newboss;
		}
		if (pm && !(pm->geno & G_UNIQ) && rn2(50) ) {
			attempts = 0;
			goto newboss;
		}

		if (pm) mon = makemon(pm, cc.x, cc.y, NO_MM_FLAGS);
	      if (mon && canspotmon(mon)) known = TRUE;

		if (known)
		    makeknown(SCR_SUMMON_BOSS);
		else if (!objects[SCR_SUMMON_BOSS].oc_name_known
			&& !objects[SCR_SUMMON_BOSS].oc_uname)
		    docall(otmp);

		u.aggravation = 0;
		u.mondiffhack = 0;

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;
	    }

	case MUSE_SCR_SUMMON_GHOST_M:

		{
		coord cc;   
		if (!enexto(&cc, mtmp->mx, mtmp->my, 0)) break;

		mreadmsg(mtmp, otmp);

		tt_mname(&cc, FALSE, 0);
		if (!objects[SCR_SUMMON_GHOST].oc_name_known
			&& !objects[SCR_SUMMON_GHOST].oc_uname)
		    docall(otmp);

		}

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;

	case MUSE_SCR_SUMMON_ELM_M:

		mreadmsg(mtmp, otmp);

		{

		int aligntype;
		aligntype = rn2((int)A_LAWFUL+2) - 1;
		pline("A servant of %s appears!",aligns[1 - aligntype].noun);
		summon_minion(aligntype, TRUE);

		}

		if (!objects[SCR_SUMMON_ELM].oc_name_known
			&& !objects[SCR_SUMMON_ELM].oc_uname)
		    docall(otmp);

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;

	case MUSE_SCR_CREATE_MONSTER_M:
	    {	coord cc;
		struct permonst *pm = 0, *fish = 0;
		int cnt = 1;
		struct monst *mon;
		boolean known = FALSE;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		if (!rn2(73)) cnt += rno(4);
		if (mtmp->mconf || otmp->cursed) cnt += rno(12);
		/*if (mtmp->mconf) pm = fish = &mons[PM_ACID_BLOB];*/ /* no easy blob fort building --Amy */
		/*else if (is_pool(mtmp->mx, mtmp->my))
		    fish = &mons[u.uinwater ? PM_GIANT_EEL : PM_CROCODILE];*/
		mreadmsg(mtmp, otmp);
		while(cnt--) {
		    /* `fish' potentially gives bias towards water locations;
		       `pm' is what to actually create (0 => random) */
		    if (!enexto(&cc, mtmp->mx, mtmp->my, fish)) break;
		    mon = makemon(pm, cc.x, cc.y, NO_MM_FLAGS);
		    if (mon && canspotmon(mon)) known = TRUE;
		}
		/* The only case where we don't use oseen.  For wands, you
		 * have to be able to see the monster zap the wand to know
		 * what type it is.  For teleport scrolls, you have to see
		 * the monster to know it teleported.
		 */
		if (known)
		    makeknown(SCR_CREATE_MONSTER);
		else if (!objects[SCR_CREATE_MONSTER].oc_name_known
			&& !objects[SCR_CREATE_MONSTER].oc_uname)
		    docall(otmp);

		u.aggravation = 0;

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;
	    }

	case MUSE_SCR_CREATE_VICTIM_M:

	    {	coord cc;
		struct permonst *pm = 0, *fish = 0;
		int cnt = 1;
		struct monst *mon;
		boolean known = FALSE;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		if (!rn2(73)) cnt += rno(4);
		if (mtmp->mconf || otmp->cursed) cnt += rno(12);
		mreadmsg(mtmp, otmp);
		while(cnt--) {
		    mon = makemon(pm, 0, 0, NO_MM_FLAGS);
		    if (mon && canspotmon(mon)) known = TRUE;
		}
		/* The only case where we don't use oseen.  For wands, you
		 * have to be able to see the monster zap the wand to know
		 * what type it is.  For teleport scrolls, you have to see
		 * the monster to know it teleported.
		 */
		if (known)
		    makeknown(SCR_CREATE_VICTIM);
		else if (!objects[SCR_CREATE_VICTIM].oc_name_known
			&& !objects[SCR_CREATE_VICTIM].oc_uname)
		    docall(otmp);

		u.aggravation = 0;

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;
	    }

	case MUSE_BAG_OF_TRICKS_M:

		/* jonadab wants monsters to not use up charges if they apply this thing, but that would be too evil. --Amy */
	    {	coord cc;

		struct permonst *pm = 0;
		struct monst *mon;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		if (!enexto(&cc, mtmp->mx, mtmp->my, pm)) return 0;
		mzapmsg(mtmp, otmp, FALSE);
		if (rn2(2) || !ishaxor) otmp->spe--;
		mon = makemon((struct permonst *)0, cc.x, cc.y, NO_MM_FLAGS);
		if (mon && canspotmon(mon) && oseen)
		    makeknown(BAG_OF_TRICKS);

		if (otmp && otmp->oartifact == ART_VERY_TRICKY_INDEED) {
			if (!enexto(&cc, mtmp->mx, mtmp->my, pm)) return 0;
			mon = makemon((struct permonst *)0, cc.x, cc.y, NO_MM_FLAGS);
			if (mon && canspotmon(mon) && oseen)
			    makeknown(BAG_OF_TRICKS);
		}

		u.aggravation = 0;

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;
	    }

	case MUSE_WAN_SUMMON_UNDEAD_M:
	    {	coord cc;

		struct permonst *pm = 0;
		struct monst *mon;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		if (!enexto(&cc, mtmp->mx, mtmp->my, pm)) return 0;
		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;

		    switch (rn2(10)+1) {
		    case 1:
			mon = makemon(mkclass(S_VAMPIRE,0), cc.x, cc.y, NO_MM_FLAGS);
			break;
		    case 2:
		    case 3:
		    case 4:
		    case 5:
			mon = makemon(mkclass(S_ZOMBIE,0), cc.x, cc.y, NO_MM_FLAGS);
			break;
		    case 6:
		    case 7:
		    case 8:
			mon = makemon(mkclass(S_MUMMY,0), cc.x, cc.y, NO_MM_FLAGS);
			break;
		    case 9:
			mon = makemon(mkclass(S_GHOST,0), cc.x, cc.y, NO_MM_FLAGS);
			break;
		    case 10:
			mon = makemon(mkclass(S_WRAITH,0), cc.x, cc.y, NO_MM_FLAGS);
			break;
		    }

		/*mon = makemon((struct permonst *)0, cc.x, cc.y, NO_MM_FLAGS);*/
		if (mon && canspotmon(mon) && oseen)
		    makeknown(WAN_SUMMON_UNDEAD);

		u.aggravation = 0;

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;
	    }

	case MUSE_WAN_SUMMON_ELM_M:

		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
	      makeknown(WAN_SUMMON_ELM);

		{
		int aligntype;
		aligntype = rn2((int)A_LAWFUL+2) - 1;
		pline("A servant of %s appears!",aligns[1 - aligntype].noun);
		summon_minion(aligntype, TRUE);
		}

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_SCR_SUMMON_UNDEAD_M:

	    {	coord cc;
		struct permonst *pm = 0;
		int cnt = 1;
		struct monst *mon;
		boolean known = FALSE;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		if (rn2(2)) cnt += rnz(2);
		if (!rn2(73)) cnt += rno(4);
		if (mtmp->mconf || otmp->cursed) cnt += rno(12);
		mreadmsg(mtmp, otmp);
		while(cnt--) {

		    if (!enexto(&cc, mtmp->mx, mtmp->my, 0)) break;

		    /*mon = makemon(pm, cc.x, cc.y, NO_MM_FLAGS);*/

		    switch (rn2(10)+1) {
		    case 1:
			mon = makemon(mkclass(S_VAMPIRE,0), cc.x, cc.y, NO_MM_FLAGS);
			break;
		    case 2:
		    case 3:
		    case 4:
		    case 5:
			mon = makemon(mkclass(S_ZOMBIE,0), cc.x, cc.y, NO_MM_FLAGS);
			break;
		    case 6:
		    case 7:
		    case 8:
			mon = makemon(mkclass(S_MUMMY,0), cc.x, cc.y, NO_MM_FLAGS);
			break;
		    case 9:
			mon = makemon(mkclass(S_GHOST,0), cc.x, cc.y, NO_MM_FLAGS);
			break;
		    case 10:
			mon = makemon(mkclass(S_WRAITH,0), cc.x, cc.y, NO_MM_FLAGS);
			break;
		    }
		    if (mon && canspotmon(mon)) known = TRUE;
		}
		/* The only case where we don't use oseen.  For wands, you
		 * have to be able to see the monster zap the wand to know
		 * what type it is.  For teleport scrolls, you have to see
		 * the monster to know it teleported.
		 */

		u.aggravation = 0;

		if (known)
		    makeknown(SCR_SUMMON_UNDEAD);
		else if (!objects[SCR_SUMMON_UNDEAD].oc_name_known
			&& !objects[SCR_SUMMON_UNDEAD].oc_uname)
		    docall(otmp);
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;
	    }

	case MUSE_SCR_GROUP_SUMMONING_M:

	    {	coord cc;
		struct permonst *pm = 0;
		int cnt = rn3(14) + 2;
		struct monst *mon;
		boolean known = FALSE;
		int randmnst;
		int randmnsx;
		struct permonst *randmonstforspawn;
		int monstercolor;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		int spawntype = rnd(4);

		if (spawntype == 1) {
			randmnst = (rn2(187) + 1);
			randmnsx = (rn2(100) + 1);
		} else if (spawntype == 2) {
			randmonstforspawn = rndmonst();
		} else if (spawntype == 3) {
			monstercolor = rnd(15);
		} else {
			monstercolor = rnd(432);
		}

		if (mtmp->mconf || otmp->cursed) cnt += rno(12);

		    if (!enexto(&cc, mtmp->mx, mtmp->my, 0)) break;

		mreadmsg(mtmp, otmp);
		while(cnt--) {

		    if (!enexto(&cc, mtmp->mx, mtmp->my, 0)) break;

			if (spawntype == 1) {

			if (randmnst < 6)
		 	    mon = makemon(mkclass(S_ANT,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 9)
		 	    mon = makemon(mkclass(S_BLOB,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 11)
		 	    mon = makemon(mkclass(S_COCKATRICE,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 15)
		 	    mon = makemon(mkclass(S_DOG,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 18)
		 	    mon = makemon(mkclass(S_EYE,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 22)
		 	    mon = makemon(mkclass(S_FELINE,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 24)
		 	    mon = makemon(mkclass(S_GREMLIN,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 29)
		 	    mon = makemon(mkclass(S_HUMANOID,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 33)
		 	    mon = makemon(mkclass(S_IMP,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 36)
		 	    mon = makemon(mkclass(S_JELLY,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 41)
		 	    mon = makemon(mkclass(S_KOBOLD,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 44)
		 	    mon = makemon(mkclass(S_LEPRECHAUN,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 47)
		 	    mon = makemon(mkclass(S_MIMIC,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 50)
		 	    mon = makemon(mkclass(S_NYMPH,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 54)
		 	    mon = makemon(mkclass(S_ORC,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 55)
		 	    mon = makemon(mkclass(S_PIERCER,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 58)
		 	    mon = makemon(mkclass(S_QUADRUPED,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 62)
		 	    mon = makemon(mkclass(S_RODENT,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 65)
		 	    mon = makemon(mkclass(S_SPIDER,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 66)
		 	    mon = makemon(mkclass(S_TRAPPER,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 69)
		 	    mon = makemon(mkclass(S_UNICORN,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 71)
		 	    mon = makemon(mkclass(S_VORTEX,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 73)
		 	    mon = makemon(mkclass(S_WORM,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 75)
		 	    mon = makemon(mkclass(S_XAN,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 76)
		 	    mon = makemon(mkclass(S_LIGHT,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 77)
		 	    mon = makemon(mkclass(S_ZOUTHERN,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 78)
		 	    mon = makemon(mkclass(S_ANGEL,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 81)
		 	    mon = makemon(mkclass(S_BAT,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 83)
		 	    mon = makemon(mkclass(S_CENTAUR,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 86)
		 	    mon = makemon(mkclass(S_DRAGON,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 89)
		 	    mon = makemon(mkclass(S_ELEMENTAL,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 94)
		 	    mon = makemon(mkclass(S_FUNGUS,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 99)
		 	    mon = makemon(mkclass(S_GNOME,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 102)
		 	    mon = makemon(mkclass(S_GIANT,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 103)
		 	    mon = makemon(mkclass(S_JABBERWOCK,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 104)
		 	    mon = makemon(mkclass(S_KOP,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 105)
		 	    mon = makemon(mkclass(S_LICH,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 108)
		 	    mon = makemon(mkclass(S_MUMMY,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 110)
		 	    mon = makemon(mkclass(S_NAGA,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 113)
		 	    mon = makemon(mkclass(S_OGRE,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 115)
		 	    mon = makemon(mkclass(S_PUDDING,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 116)
		 	    mon = makemon(mkclass(S_QUANTMECH,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 118)
		 	    mon = makemon(mkclass(S_RUSTMONST,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 121)
		 	    mon = makemon(mkclass(S_SNAKE,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 123)
		 	    mon = makemon(mkclass(S_TROLL,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 124)
		 	    mon = makemon(mkclass(S_UMBER,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 125)
		 	    mon = makemon(mkclass(S_VAMPIRE,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 127)
		 	    mon = makemon(mkclass(S_WRAITH,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 128)
		 	    mon = makemon(mkclass(S_XORN,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 130)
		 	    mon = makemon(mkclass(S_YETI,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 135)
		 	    mon = makemon(mkclass(S_ZOMBIE,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 145)
		 	    mon = makemon(mkclass(S_HUMAN,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 147)
		 	    mon = makemon(mkclass(S_GHOST,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 149)
		 	    mon = makemon(mkclass(S_GOLEM,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 152)
		 	    mon = makemon(mkclass(S_DEMON,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 155)
		 	    mon = makemon(mkclass(S_EEL,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 160)
		 	    mon = makemon(mkclass(S_LIZARD,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 162)
		 	    mon = makemon(mkclass(S_BAD_FOOD,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 165)
		 	    mon = makemon(mkclass(S_BAD_COINS,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 166) {
				if (randmnsx < 96)
		 	    mon = makemon(mkclass(S_HUMAN,0), cc.x, cc.y, MM_ADJACENTOK);
				else
		 	    mon = makemon(mkclass(S_NEMESE,0), cc.x, cc.y, MM_ADJACENTOK);
				}
			else if (randmnst < 171)
		 	    mon = makemon(mkclass(S_GRUE,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 176)
		 	    mon = makemon(mkclass(S_WALLMONST,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 180)
		 	    mon = makemon(mkclass(S_RUBMONST,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 181) {
				if (randmnsx < 99)
		 	    mon = makemon(mkclass(S_HUMAN,0), cc.x, cc.y, MM_ADJACENTOK);
				else
		 	    mon = makemon(mkclass(S_ARCHFIEND,0), cc.x, cc.y, MM_ADJACENTOK);
				}
			else if (randmnst < 186)
		 	    mon = makemon(mkclass(S_TURRET,0), cc.x, cc.y, MM_ADJACENTOK);
			else if (randmnst < 187)
		 	    mon = makemon(mkclass(S_FLYFISH,0), cc.x, cc.y, MM_ADJACENTOK);
			else
		 	    mon = makemon((struct permonst *)0, cc.x, cc.y, MM_ADJACENTOK);

			} else if (spawntype == 2) {

				mon = makemon(randmonstforspawn, cc.x, cc.y, MM_ADJACENTOK);

			} else if (spawntype == 3) {

				mon = makemon(colormon(monstercolor), cc.x, cc.y, MM_ADJACENTOK);

			} else {

				mon = makemon(specialtensmon(monstercolor), cc.x, cc.y, MM_ADJACENTOK);

			}

		    if (mon && canspotmon(mon)) known = TRUE;
		}
		/* The only case where we don't use oseen.  For wands, you
		 * have to be able to see the monster zap the wand to know
		 * what type it is.  For teleport scrolls, you have to see
		 * the monster to know it teleported.
		 */

		u.aggravation = 0;

		if (known)
		    makeknown(SCR_GROUP_SUMMONING);
		else if (!objects[SCR_GROUP_SUMMONING].oc_name_known
			&& !objects[SCR_GROUP_SUMMONING].oc_uname)
		    docall(otmp);
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;
	    }

	case MUSE_WAN_CREATE_MONSTER_M:
	    {	coord cc;

		struct permonst *pm = 0;
		struct monst *mon;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		if (!enexto(&cc, mtmp->mx, mtmp->my, pm)) return 0;
		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		mon = makemon((struct permonst *)0, cc.x, cc.y, NO_MM_FLAGS);
		if (mon && canspotmon(mon) && oseen)
		    makeknown(WAN_CREATE_MONSTER);

		u.aggravation = 0;

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;
	    }

	case MUSE_WAN_CREATE_HORDE_M:
	    {   coord cc;
		struct permonst *pm=rndmonst();
		int cnt = 1;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		if (!enexto(&cc, mtmp->mx, mtmp->my, pm)) return 0;
		mzapmsg(mtmp, otmp, FALSE);
		if ((rn2(2) || !ishaxor) && (!rn2(2) || !otmp->oartifact)) otmp->spe--;
		if (oseen) makeknown(WAN_CREATE_HORDE);
		cnt = rno(14);
		while(cnt--) {
			struct monst *mon;
			if (!enexto(&cc, mtmp->mx, mtmp->my, pm)) continue;
			mon = makemon(rndmonst(), cc.x, cc.y, NO_MM_FLAGS);
			if (mon) newsym(mon->mx,mon->my);
		}

		u.aggravation = 0;

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;
	    }

	case MUSE_POLY_TRAP:
		if (vismon)
		    pline("%s deliberately %s onto a polymorph trap!",
			Monnam(mtmp),
			makeplural(locomotion(mtmp->data, "jump")));
		if (vis) seetrap(t_at(trapx,trapy));

		/*  don't use rloc() due to worms */
		remove_monster(mtmp->mx, mtmp->my);
		newsym(mtmp->mx, mtmp->my);
		place_monster(mtmp, trapx, trapy);
		if (mtmp->wormno) worm_move(mtmp);
		newsym(trapx, trapy);

#if 0
		(void) newcham(mtmp, (struct permonst *)0, FALSE, vismon);
#else
		(void) mon_poly(mtmp, FALSE, "%s changes!");
#endif
		return 2;
	case MUSE_MAGIC_TRAP:
		if (vismon)
		    pline("%s deliberately %s onto a magic trap!",
			Monnam(mtmp),
			makeplural(locomotion(mtmp->data, "jump")));
		if (vis) seetrap(t_at(trapx,trapy));

		/*  don't use rloc() due to worms */
		remove_monster(mtmp->mx, mtmp->my);
		if (t_at(trapx,trapy)) deltrap(t_at(trapx,trapy));
		newsym(mtmp->mx, mtmp->my);
		place_monster(mtmp, trapx, trapy);
		mtmp->mcan = 0;
		if (mtmp->wormno) worm_move(mtmp);
		newsym(trapx, trapy);

		return 2;
	case MUSE_BULLWHIP:
		/* attempt to disarm hero */
		if (uwep && !rn2(5)) {
		    const char *The_whip = vismon ? "The bullwhip" : "A whip";
		    int where_to = rn2(4);
		    struct obj *obj = uwep;
		    const char *hand;
		    char the_weapon[BUFSZ];

		    strcpy(the_weapon, the(xname(obj)));
		    hand = body_part(HAND);
		    if (bimanual(obj)) hand = makeplural(hand);

		    if (vismon)
			pline("%s flicks a bullwhip towards your %s!",
			      Monnam(mtmp), hand);
		    if (obj->otyp == HEAVY_IRON_BALL) {
			pline("%s fails to wrap around %s.",
			      The_whip, the_weapon);
			return 1;
		    }
		    pline("%s wraps around %s you're wielding!",
			  The_whip, the_weapon);
		    if (welded(obj)) {
			pline("%s welded to your %s%c",
			      !is_plural(obj) ? "It is" : "They are",
			      hand, !obj->bknown ? '!' : '.');
			/* obj->bknown = 1; */ /* welded() takes care of this */
			where_to = 0;
		    }
		    if (!where_to) {
			pline_The("whip slips free.");  /* not `The_whip' */
			return 1;
		    } else if (where_to == 3 && hates_silver(mtmp->data) && issoviet &&
			    objects[obj->otyp].oc_material == MT_SILVER) {
			/* this monster won't want to catch a silver weapon; drop it at hero's feet instead */
			/* Amy edit: but only in soviet mode :-P catching it doesn't damage the monster! */
			where_to = 2;
		    }
		    freeinv(obj);
		    uwepgone();
		    switch (where_to) {
			case 1:		/* onto floor beneath mon */
			    pline("%s yanks %s from your %s!", Monnam(mtmp),
				  the_weapon, hand);
			    place_object(obj, mtmp->mx, mtmp->my);
			    break;
			case 2:		/* onto floor beneath you */
			    pline("%s yanks %s to the %s!", Monnam(mtmp),
				  the_weapon, surface(u.ux, u.uy));
			    dropy(obj);
			    break;
			case 3:		/* into mon's inventory */
			    pline("%s snatches %s!", Monnam(mtmp),
				  the_weapon);
			    (void) mpickobj(mtmp,obj,FALSE);
			    break;
		    }
		    return 1;
		}
		return 0;
	case 0: return 0; /* i.e. an exploded wand */
	default: impossible("%s wanted to perform action %d?", Monnam(mtmp),
			m.has_misc);
		break;
	}
	return 0;
}

STATIC_OVL void
you_aggravate(mtmp)
struct monst *mtmp;
{
	pline("For some reason, %s presence is known to you.",
		s_suffix(noit_mon_nam(mtmp)));
	cls();
#ifdef CLIPPING
	cliparound(mtmp->mx, mtmp->my);
#endif
	show_glyph(mtmp->mx, mtmp->my, mon_to_glyph(mtmp));
	display_self();
	You_feel("aggravated at %s.", noit_mon_nam(mtmp));
	display_nhwindow(WIN_MAP, TRUE);
	docrt();
	if (unconscious()) {
		multi = -1;
		nomovemsg =
		      "Aggravated, you are jolted into full consciousness.";
	}
	newsym(mtmp->mx,mtmp->my);
	if (!canspotmon(mtmp) && !(mtmp->data->msound == MS_DEEPSTATE) && !(mtmp->egotype_deepstatemember))
	    map_invisible(mtmp->mx, mtmp->my);
}

int
rnd_misc_item(mtmp)
struct monst *mtmp;
{
	struct permonst *pm = mtmp->data;
	int difficulty = monstr[(monsndx(pm))];

	if((is_animal(pm) || attacktype(pm, AT_EXPL) || mindless(mtmp->data)
			|| pm->mlet == S_GHOST
			|| pm->mlet == S_KOP
		) && issoviet) return 0;
	/* Unlike other rnd_item functions, we only allow _weak_ monsters
	 * to have this item; after all, the item will be used to strengthen
	 * the monster and strong monsters won't use it at all...
	 */
	if (difficulty < 6 && !rn2(30))
	    return rn2(6) ? POT_POLYMORPH : WAN_POLYMORPH;

	if (!rn2(40)/* && !nonliving(pm)*/) return AMULET_OF_MONSTER_SAVING;

	switch (rn2(3)) {
		case 0:
			if (mtmp->isgd) return 0;
			return rn2(6) ? POT_SPEED : WAN_SPEED_MONSTER;
		case 1:
			if (mtmp->mpeaceful && !StrongSee_invisible) return 0;
			return rn2(6) ? POT_INVISIBILITY : WAN_MAKE_INVISIBLE;
		case 2:
			return POT_GAIN_LEVEL;
	}
	/*NOTREACHED*/
	return 0;
}

int
rnd_misc_item_new(mtmp)
struct monst *mtmp;
{
	struct permonst *pm = mtmp->data;

	if((is_animal(pm) || attacktype(pm, AT_EXPL) || mindless(mtmp->data)
			|| pm->mlet == S_GHOST
			|| pm->mlet == S_KOP
		) && issoviet) return 0;
	switch (rn2(14)) {

		case 0: return POT_GAIN_LEVEL;
		case 1: return WAN_MAKE_INVISIBLE;
		case 2: return POT_INVISIBILITY;
		case 3: return WAN_POLYMORPH;
		case 4: return POT_SPEED;
		case 5: return WAN_SPEED_MONSTER;
		case 6: return BULLWHIP;
		case 7: return POT_POLYMORPH;
		case 8: return WAN_CLONE_MONSTER;
		case 9: return WAN_HASTE_MONSTER;
		case 10: return POT_MUTATION;
		case 11: return WAN_MUTATION;
		case 12: return WAN_GAIN_LEVEL;
		case 13: return WAN_INCREASE_MAX_HITPOINTS;

	}
	/*NOTREACHED*/
	return 0;
}

boolean
searches_for_item(mon, obj)
struct monst *mon;
struct obj *obj;
{
	int typ = obj->otyp;

	if (!issoviet && !rn2(5)) return FALSE;

	if ((is_animal(mon->data) ||
		mindless(mon->data) ||
		mon->data == &mons[PM_GHOST]) && issoviet)	/* don't loot bones piles */
	    return FALSE;

	if (typ == WAN_MAKE_INVISIBLE || typ == POT_INVISIBILITY)
	    return (boolean)(!mon->minvis && !mon->invis_blkd && !attacktype(mon->data, AT_GAZE));
	if (typ == WAN_SPEED_MONSTER || typ == WAN_HASTE_MONSTER || typ == POT_SPEED)
	    return (boolean)(mon->mspeed != MFAST);

	switch (obj->oclass) {
	case WAND_CLASS:
	    if (obj->spe <= 0)
		return FALSE;
	    if (typ == WAN_DIGGING)
		return (boolean)(!is_floater(mon->data));
	    if (typ == WAN_POLYMORPH)
		return (boolean)(monstr[monsndx(mon->data)] < 6);
	    if (objects[typ].oc_dir == RAY ||
		    typ == WAN_STRIKING ||
		    typ == WAN_GRAVITY_BEAM ||
		    typ == WAN_BUBBLEBEAM ||
		    typ == WAN_DREAM_EATER ||
		    typ == WAN_GOOD_NIGHT ||
		    typ == WAN_BANISHMENT ||
		    typ == WAN_TELEPORTATION ||
		    typ == WAN_CREATE_MONSTER ||
		    typ == WAN_SUMMON_UNDEAD ||
		    typ == WAN_SUMMON_ELM ||
		    typ == WAN_TRAP_CREATION ||
		    typ == WAN_SUMMON_SEXY_GIRL ||
		    typ == WAN_CREATE_HORDE ||
		    typ == WAN_DRAINING	||
		    typ == WAN_TIME	||
		    typ == WAN_REDUCE_MAX_HITPOINTS	||
		    typ == WAN_INCREASE_MAX_HITPOINTS	||
		    typ == WAN_SLOW_MONSTER	||
		    typ == WAN_INERTIA	||
		    typ == WAN_FEAR	||
		    typ == WAN_HEALING ||
		    typ == WAN_MEDIUM_HEALING ||
		    typ == WAN_STRONG_HEALING ||
		    typ == WAN_SUPER_HEALING ||
		    typ == WAN_TELEPORT_SELF ||
		    typ == WAN_CLONE_MONSTER ||
		    typ == WAN_EXTRA_HEALING ||
		    typ == WAN_FULL_HEALING ||
		    typ == WAN_BAD_EFFECT ||
		    typ == WAN_STONING ||
		    typ == WAN_DISINTEGRATION ||
		    typ == WAN_PARALYSIS ||
		    typ == WAN_MAKE_VISIBLE ||
		    typ == WAN_CURSE_ITEMS ||
		    typ == WAN_AMNESIA ||
		    typ == WAN_BLEEDING ||
		    typ == WAN_UNDRESSING ||
		    typ == WAN_LEVITATION ||
		    typ == WAN_IMMOBILITY ||
		    typ == WAN_INSANITY ||
		    typ == WAN_BAD_EQUIPMENT ||
		    typ == WAN_EGOISM ||
		    typ == WAN_MUTATION ||
		    typ == WAN_BAD_LUCK ||
		    typ == WAN_REMOVE_RESISTANCE ||
		    typ == WAN_CORROSION ||
		    typ == WAN_CHAOS_TERRAIN ||
		    typ == WAN_FLEECY_TERRAIN ||
		    typ == WAN_STAT_REDUCTION ||
		    typ == WAN_DISENCHANTMENT ||
		    typ == WAN_CONTAMINATION ||
		    typ == WAN_TREMBLING ||
		    typ == WAN_FUMBLING ||
		    typ == WAN_SIN ||
		    typ == WAN_DESLEXIFICATION ||
		    typ == WAN_FINGER_BENDING ||
		    typ == WAN_DRAIN_MANA ||
		    typ == WAN_TIDAL_WAVE ||
		    typ == WAN_STARVATION ||
		    typ == WAN_CONFUSION ||
		    typ == WAN_STUN_MONSTER ||
		    typ == WAN_SLIMING ||
		    typ == WAN_POISON ||
		    typ == WAN_HYPER_BEAM ||
		    typ == WAN_CHROMATIC_BEAM ||
		    typ == WAN_DISINTEGRATION_BEAM ||
		    typ == WAN_LYCANTHROPY ||
		    typ == WAN_PUNISHMENT ||
		    typ == WAN_TELE_LEVEL ||
		    typ == WAN_GAIN_LEVEL ||
		    typ == WAN_CANCELLATION)
		return TRUE;
	    break;
	case POTION_CLASS:
	    if (typ == POT_VAMPIRE_BLOOD)
		return is_vampire(mon->data);
	    if (typ == POT_BLOOD)
		return is_vampire(mon->data);
	    if (typ == POT_HEALING ||
		    typ == POT_EXTRA_HEALING ||
		    typ == POT_FULL_HEALING ||
		    typ == POT_POLYMORPH ||
		    (typ == POT_GAIN_ENERGY && mon->mcan) ||
		    typ == POT_MUTATION ||
		    typ == POT_GAIN_LEVEL ||
		    typ == POT_PARALYSIS ||
		    typ == POT_SLEEPING ||
		    typ == POT_ACID ||
		    typ == POT_CONFUSION ||
		    typ == POT_CYANIDE ||
		    typ == POT_RADIUM ||
		    typ == POT_HALLUCINATION ||
		    typ == POT_STUNNING ||
		    typ == POT_NUMBNESS ||
		    typ == POT_SLIME ||
		    typ == POT_URINE ||
		    typ == POT_CANCELLATION ||
		    typ == POT_ICE ||
		    typ == POT_FEAR ||
		    typ == POT_FIRE ||
		    itemhasappearance(obj, APP_POTION_NITROGLYCERIN) || 
			/* Monsters will pick up potions of salt water even if your snail is polymorphed,
			 * but they'll only throw them if you're actually in snail form --Amy */
		    (typ == POT_SALT_WATER && Race_if(PM_ELONA_SNAIL)) ||
		    (typ == POT_OIL && mon->data->msound == MS_CAR) ||
		    typ == POT_DIMNESS ||
		    typ == POT_SANITY ||
		    typ == POT_CURE_WOUNDS ||
		    typ == POT_CURE_SERIOUS_WOUNDS ||
		    typ == POT_CURE_CRITICAL_WOUNDS ||
		    typ == POT_AMNESIA)
		return TRUE;
	    if (typ == POT_BLINDNESS && !attacktype(mon->data, AT_GAZE))
		return TRUE;
	    break;
	case SCROLL_CLASS:
	    if (typ == SCR_TELEPORTATION ||
		 typ == SCR_RELOCATION ||
		 typ == SCR_HEALING ||
		 typ == SCR_EXTRA_HEALING ||
		 typ == SCR_POWER_HEALING ||
		 typ == SCR_TELE_LEVEL ||
		 typ == SCR_BRANCH_TELEPORT ||
		 typ == SCR_WARPING ||
		 typ == SCR_COURSE_TRAVELING ||
		 typ == SCR_ROOT_PASSWORD_DETECTION ||
		 typ == SCR_CREATE_MONSTER ||
		 typ == SCR_CREATE_TRAP ||
		 typ == SCR_CREATE_VICTIM ||
		 typ == SCR_SUMMON_UNDEAD ||
		 typ == SCR_GROUP_SUMMONING ||
		 typ == SCR_FLOOD ||
		 typ == SCR_VILENESS ||
		 typ == SCR_HYBRIDIZATION ||
		 typ == SCR_NASTY_CURSE ||
		 typ == SCR_BAD_EQUIPMENT ||
		 typ == SCR_MEGALOAD ||
		 typ == SCR_ANTIMATTER ||
		 typ == SCR_RUMOR ||
		 typ == SCR_MESSAGE ||
		 typ == SCR_ILLUSION ||
		 typ == SCR_VISIBLE_ITEM ||
		 typ == SCR_EVIL_VARIANT ||
		 typ == SCR_FEMINISM ||
		 typ == SCR_SIN ||
		 typ == SCR_IMMOBILITY ||
		 typ == SCR_EGOISM ||
		 typ == SCR_ENRAGE ||
		 typ == SCR_BULLSHIT ||
		 typ == SCR_DESTROY_ARMOR ||
		 typ == SCR_DESTROY_WEAPON ||
		 typ == SCR_LAVA ||
		 typ == SCR_OFFLEVEL_ITEM ||
		 typ == SCR_RAGNAROK ||
		 typ == SCR_FLOODING ||
		 typ == SCR_SUMMON_BOSS ||
		 typ == SCR_SUMMON_GHOST ||
		 typ == SCR_SUMMON_ELM ||
		 typ == SCR_STONING ||
		 typ == SCR_AMNESIA ||
		 typ == SCR_LOCKOUT ||
		 typ == SCR_GROWTH ||
		 typ == SCR_ICE ||
		 typ == SCR_BAD_EFFECT ||
		 typ == SCR_CLOUDS ||
		 typ == SCR_BARRHING ||
		 typ == SCR_CHAOS_TERRAIN ||
		 typ == SCR_PUNISHMENT ||
		 typ == SCR_EARTH ||
		 typ == SCR_GRAVE ||
		 typ == SCR_TUNNELS ||
		 typ == SCR_FARMING ||
		 typ == SCR_MOUNTAINS ||
		 typ == SCR_DIVING ||
		 typ == SCR_CRYSTALLIZATION ||
		 typ == SCR_MOORLAND ||
		 typ == SCR_URINE ||
		 typ == SCR_QUICKSAND ||
		 typ == SCR_STYX ||
		 typ == SCR_SNOW ||
		 typ == SCR_ASH ||
		 typ == SCR_SAND ||
		 typ == SCR_PAVING ||
		 typ == SCR_HIGHWAY ||
		 typ == SCR_GRASSLAND ||
		 typ == SCR_NETHER ||
		 typ == SCR_STALACTITE ||
		 typ == SCR_CRYPT ||
		 typ == SCR_BUBBLE_BOBBLE ||
		 typ == SCR_RAIN ||
		 typ == SCR_TRAP_CREATION ||
		 typ == SCR_FIRE ||
		 typ == SCR_WOUNDS ||
		 typ == SCR_DEMONOLOGY ||
		 typ == SCR_ELEMENTALISM ||
		 typ == SCR_GIRLINESS ||
		 typ == SCR_NASTINESS )
		return TRUE;
	    break;
	case AMULET_CLASS:
	    if (typ == AMULET_OF_MONSTER_SAVING || typ == AMULET_OF_DATA_STORAGE || typ == AMULET_OF_REFLECTION || typ == AMULET_OF_PRISM || typ == AMULET_OF_WARP_DIMENSION)
		return /*(boolean)(!nonliving(mon->data))*/TRUE;
	    if (typ == AMULET_OF_REFLECTION)
		return TRUE;
	    break;
	case RING_CLASS:
	    if (typ == RIN_TIMELY_BACKUP)
		return TRUE;
	    break;
	case CHAIN_CLASS:
	case BALL_CLASS:
		if (!Punished) return TRUE;
	    break;
	/* this was causing the game to crash if the player was punished... */

	case TOOL_CLASS:
	    if (typ == PICK_AXE)
		return (boolean)needspick(mon->data);
	    if (typ == CONGLOMERATE_PICK)
		return (boolean)needspick(mon->data);
	    if (typ == UNWIELDY_PICK)
		return (boolean)needspick(mon->data);
	    if (typ == CONUNDRUM_PICK)
		return (boolean)needspick(mon->data);
	    if (typ == MYSTERY_PICK)
		return (boolean)needspick(mon->data);
	    if (typ == BRONZE_PICK)
		return (boolean)needspick(mon->data);
	    if (typ == BRICK_PICK)
		return (boolean)needspick(mon->data);
	    if (typ == MYSTERIOUS_PICK)
		return (boolean)needspick(mon->data);
	    if (typ == NANO_PICK)
		return (boolean)needspick(mon->data);
	    if (typ == UNICORN_HORN)
		return (boolean)(!obj->cursed && !is_unicorn(mon->data));
	    if (typ == FROST_HORN || typ == FIRE_HORN || typ == TEMPEST_HORN || typ == ETHER_HORN || typ == SHADOW_HORN || typ == DEATH_HORN || typ == CHROME_HORN)
		return (obj->spe > 0);
	    if (is_weptool(obj))
	    	return (boolean)likes_objs(mon->data);
	    if (typ == BAG_OF_TRICKS)
		return TRUE;
	    if (typ == BUGLE) /* wtf they would not pick them up in vanilla??? makes zero sense --Amy */
		return is_mercenary(mon->data);
	    break;
	case FOOD_CLASS:
	    if (typ == CORPSE)
		return (boolean)(((mon->misc_worn_check & W_ARMG) &&
				    touch_petrifies(&mons[obj->corpsenm])) ||
				(!resists_ston(mon) &&
				    (obj->corpsenm == PM_LIZARD ||
					(acidic(&mons[obj->corpsenm]) && !slime_on_touch(&mons[obj->corpsenm])) )));
	    if (typ == EGG)
		return (boolean)(touch_petrifies(&mons[obj->corpsenm]) && (obj->corpsenm != PM_PLAYERMON));
	    break;
	default:
	    break;
	}

	return FALSE;
}

boolean
mon_reflects(mon,str)
struct monst *mon;
const char *str;
{
	struct obj *orefl = which_armor(mon, W_ARMS);
	struct obj *monwepon = MON_WEP(mon);

	if (orefl && orefl->otyp == SHIELD_OF_REFLECTION) {
	    if (str) {
		pline(str, s_suffix(mon_nam(mon)), "shield");
		makeknown(SHIELD_OF_REFLECTION);
	    }
	    return TRUE;
	} else if (monwepon && monwepon->otyp == BEAM_REFLECTOR_GUN) {
	    if (str)
		pline(str, s_suffix(mon_nam(mon)), "weapon");
	    return TRUE;
	} else if (arti_reflects(MON_WEP(mon))) {
	    /* due to wielded artifact weapon */
	    if (str)
		pline(str, s_suffix(mon_nam(mon)), "weapon");
	    return TRUE;
	} else if ((orefl = which_armor(mon, W_AMUL)) &&
				orefl->otyp == AMULET_OF_REFLECTION) {
	    if (str) {
		pline(str, s_suffix(mon_nam(mon)), "amulet");
		makeknown(AMULET_OF_REFLECTION);
	    }
	    return TRUE;
	} else if ((orefl = which_armor(mon, W_AMUL)) &&
				orefl->otyp == AMULET_OF_PRISM) {
	    if (str) {
		pline(str, s_suffix(mon_nam(mon)), "amulet");
		makeknown(AMULET_OF_PRISM);
	    }
	    return TRUE;
	} else if ((orefl = which_armor(mon, W_AMUL)) &&
				orefl->otyp == AMULET_OF_WARP_DIMENSION) {
	    if (str) {
		pline(str, s_suffix(mon_nam(mon)), "amulet");
		makeknown(AMULET_OF_WARP_DIMENSION);
	    }
	    return TRUE;
	} else if ((orefl = which_armor(mon, W_AMUL)) &&
				orefl->otyp == AMULET_OF_DATA_STORAGE) {
	    if (str) {
		pline(str, s_suffix(mon_nam(mon)), "amulet");
		makeknown(AMULET_OF_DATA_STORAGE);
	    }
	    return TRUE;
	} else if ((orefl = which_armor(mon, W_ARMC)) &&
				orefl->otyp == CLOAK_OF_REFLECTION) {
	    if (str) {
		pline(str, s_suffix(mon_nam(mon)), "cloak");
		makeknown(CLOAK_OF_REFLECTION);
	    }
	    return TRUE;
	} else if ((orefl = which_armor(mon, W_ARMG)) &&
				orefl->otyp == GAUNTLETS_OF_REFLECTION) {
	    if (str) {
		pline(str, s_suffix(mon_nam(mon)), "gauntlets");
		makeknown(GAUNTLETS_OF_REFLECTION);
	    }
	    return TRUE;
	} else if ((orefl = which_armor(mon, W_SADDLE)) &&
				orefl->oartifact == ART_HELLRIDER_S_SADDLE) {
	    if (str) {
		pline(str, s_suffix(mon_nam(mon)), "saddle");
	    }
	    return TRUE;
	} else if ((orefl = which_armor(mon, W_SADDLE)) &&
				orefl->oartifact == ART_SADDLE_OF_REFLECTION) {
	    if (str) {
		pline(str, s_suffix(mon_nam(mon)), "saddle");
	    }
	    return TRUE;
	} else if (mon->data == &mons[PM_NIGHTMARE]) {
	    pline(str,s_suffix(mon_nam(mon)),"horn");
	    return TRUE;
	} else if ((orefl = which_armor(mon, W_ARM)) &&
		(orefl->otyp == JUMPSUIT || orefl->otyp == SILVER_DRAGON_SCALES || orefl->otyp == SILVER_DRAGON_SCALE_MAIL)) {
	    if (str)
		pline(str, s_suffix(mon_nam(mon)), "armor");
	    return TRUE;
	} else if (mon->data == &mons[PM_SILVER_DRAGON] || mon->data == &mons[PM_OLD_SILVER_DRAGON] || mon->data == &mons[PM_VERY_OLD_SILVER_DRAGON] || mon->data == &mons[PM_AUREAL] || mon->data == &mons[PM_SILVER_DRACONIAN] || mon->data == &mons[PM_ANCIENT_SILVER_DRAGON] ||
		mon->data == &mons[PM_CHROMATIC_DRAGON]) {
	    /* Silver dragons only reflect when mature; babies do not */
	    if (str)
		pline(str, s_suffix(mon_nam(mon)), "scales");
	    return TRUE;
	} else if (mon->data == &mons[PM_ARCH_LICHEN]) {
		/* Intrinsic reflection for the greatest one */
	    if (str)
		pline(str, s_suffix(mon_nam(mon)), "RNG-endowed mirror on a stick");
	    return TRUE;
	} else if (mon->data == &mons[PM_DIAMOND_GOLEM]
	         || mon->data == &mons[PM_SAPPHIRE_GOLEM]
	         || mon->data == &mons[PM_CRYSTAL_GOLEM]) {
	    /* Some of the higher golems have intrinsic reflection */
	    if (str)
		pline(str, s_suffix(mon_nam(mon)), "body");
	    return TRUE;
	} else if (is_reflector(mon->data) || (mon->egotype_reflecting) || (mon->hominglazer) || (mon->egotype_breather) ) {
		/* in ADOM the silver wolf would absorb bolts instead, apparently? */
	    if (str)
		pline(str, s_suffix(mon_nam(mon)), "absorbing shell");
	    return TRUE;
 	} else if (attackdamagetype(mon->data, AT_BREA, AD_RBRE) ) {
	    if (str)
		pline(str, s_suffix(mon_nam(mon)), "reflexive surface");
	    return TRUE;
 	} else if (FemtrapActiveMarleen && humanoid(mon->data) && is_female(mon->data) && u.marleenproperties) {
	    if (str)
		pline(str, s_suffix(mon_nam(mon)), "female skin");
	    return TRUE;
	}
	return FALSE;
}

boolean
ureflects (fmt, str)
const char *fmt, *str;
{
	/* Check from outermost to innermost objects */

	if (EReflecting & W_ARMS) {
	    if (fmt && str) {
	    	pline(fmt, str, "shield");
	    	/*makeknown(SHIELD_OF_REFLECTION);*/
	    }
	    return TRUE;
	} else if (HReflecting) {
	    if (fmt && str)
		  pline(fmt,str,"magical shield");
	    return TRUE;
	} else if (EReflecting & W_WEP) {
	    /* Due to wielded artifact weapon */
	    if (fmt && str)
	    	pline(fmt, str, "weapon");
	    return TRUE;
	} else if (EReflecting & W_AMUL) {
	    if (fmt && str) {
	    	pline(fmt, str, "medallion");
	    	/*makeknown(AMULET_OF_REFLECTION);*/
	    }
	    return TRUE;
	} else if (EReflecting & W_ARM) {
	    if (fmt && str)
	    	pline(fmt, str, "armor");
	    return TRUE;
	} else if (EReflecting & W_ARMU) {
	    if (fmt && str)
	    	pline(fmt, str, "shirt");
	    return TRUE;
	} else if (EReflecting & W_ARMF) {
	    if (fmt && str)
	    	pline(fmt, str, "footwear");
	    return TRUE;
	} else if (EReflecting & W_ARMH) {
	    if (fmt && str)
	    	pline(fmt, str, "helmet");
	    return TRUE;
	} else if (EReflecting & W_ARMC) {
	    if (fmt && str)
	    	pline(fmt, str, "cloak");
	    	/*makeknown(CLOAK_OF_REFLECTION);*/
	    return TRUE;
	} else if (EReflecting & W_ARMG) {
	    if (fmt && str)
	    	pline(fmt, str, "gauntlets");
	    	/*makeknown(GAUNTLETS_OF_REFLECTION);*/
	    return TRUE;
	} else if (youmonst.data == &mons[PM_SILVER_DRAGON] || youmonst.data == &mons[PM_OLD_SILVER_DRAGON] || youmonst.data == &mons[PM_VERY_OLD_SILVER_DRAGON] || youmonst.data == &mons[PM_AUREAL] || youmonst.data == &mons[PM_SILVER_DRACONIAN] || youmonst.data == &mons[PM_ANCIENT_SILVER_DRAGON]) {
	    if (fmt && str)
	    	pline(fmt, str, "scales");
	    return TRUE;
	} else if (youmonst.data == &mons[PM_DIAMOND_GOLEM]
	         || youmonst.data == &mons[PM_SAPPHIRE_GOLEM]
	         || youmonst.data == &mons[PM_IT]
	         || youmonst.data == &mons[PM_CRYSTAL_GOLEM]) {
	    if (fmt && str)
	    	pline(fmt, str, "body");
	    return TRUE;
	} else if (is_reflector(youmonst.data) ) {
	    if (fmt && str)
	    	pline(fmt, str, "surface");
	    return TRUE;
	}
	return FALSE;
}


/* TRUE if the monster ate something */
boolean
munstone(mon, by_you)
struct monst *mon;
boolean by_you;
{
	struct obj *obj;

	if (resists_ston(mon)) return FALSE;
	if (mon->meating || !mon->mcanmove || mon->msleeping) return FALSE;

	for(obj = mon->minvent; obj; obj = obj->nobj) {
	    /* Monsters can also use potions of acid */
	    if ((obj->otyp == POT_ACID) || (obj->otyp == CORPSE &&
	    		(obj->corpsenm == PM_LIZARD || (acidic(&mons[obj->corpsenm]) && !slime_on_touch(&mons[obj->corpsenm]) )))) {
		mon_consume_unstone(mon, obj, by_you, TRUE);
		return TRUE;
	    }
	}
	return FALSE;
}

STATIC_OVL void
mon_consume_unstone(mon, obj, by_you, stoning)
struct monst *mon;
struct obj *obj;
boolean by_you;
boolean stoning;
{
    int nutrit = (obj->otyp == CORPSE) ? dog_nutrition(mon, obj) : 0;
    /* also sets meating */

    /* give a "<mon> is slowing down" message and also remove
       intrinsic speed (comparable to similar effect on the hero) */
    mon_adjust_speed(mon, -3, (struct obj *)0);

    if (canseemon(mon)) {
	long save_quan = obj->quan;

	obj->quan = 1L;
	pline("%s %ss %s.", Monnam(mon),
		    (obj->otyp == POT_ACID) ? "quaff" : "eat",
		    distant_name(obj,doname));
	obj->quan = save_quan;
    } else if (flags.soundok)
	You_hear("%s.", (obj->otyp == POT_ACID) ? "drinking" : "chewing");
    if (((obj->otyp == POT_ACID) || acidic(&mons[obj->corpsenm])) &&
		    !resists_acid(mon)) {
	mon->mhp -= rnd(15);
	pline("%s has a very bad case of stomach acid.",
	    Monnam(mon));
    }
    if (mon->mhp <= 0) {
	m_useup(mon, obj);
	pline("%s dies!", Monnam(mon));
	if (by_you) xkilled(mon, 0);
	else mondead(mon);
	return;
    }
    if (stoning && canseemon(mon)) {
	if (FunnyHallu)
    pline("What a pity - %s just ruined a future piece of art!",
	    mon_nam(mon));
	else
	    pline("%s seems limber!", Monnam(mon));
    }
    if (obj->otyp == CORPSE && obj->corpsenm == PM_LIZARD && mon->mconf) {
	mon->mconf = 0;
	if (canseemon(mon))
	    pline("%s seems steadier now.", Monnam(mon));
    }
    if (mon->mtame && !mon->isminion && nutrit > 0) {
	struct edog *edog = EDOG(mon);

	if (edog->hungrytime < monstermoves) edog->hungrytime = monstermoves;
	edog->hungrytime += nutrit;
	mon->mconf = 0;
    }
    m_useup(mon, obj);
    mon->mlstmv = monstermoves; /* it takes a turn */
}

/* if a monster has a non-locked container, it empties its contents --Amy */
void
monsteremptycontainers(mtmp)
struct monst *mtmp;
{
	register struct obj *obj;
	struct obj *curr, *otmp;

	if (mtmp->data == &mons[PM_COW_HILL_GIANT]) return;
	if (mtmp->data == &mons[PM_IT_S_TOO_HEAVY_FOR_YOUR_HILL_GIANT]) return;
	if (mtmp->data == &mons[PM_NEW_POTATO_ER]) return;
	if (mtmp->data == &mons[PM_SMALL_POTATO_ER]) return;
	if (mtmp->data == &mons[PM_POTATO_ER]) return;
	if (mtmp->data == &mons[PM_OLDER_POTATO_ER]) return;
	if (mtmp->data == &mons[PM_LARGER_POTATO_ER]) return;
	if (mtmp->data == &mons[PM_OLD_POTATO_ER]) return;
	if (mtmp->data == &mons[PM_SELFIT_CUBE]) return;
	if (mtmp->data == &mons[PM_BOXIT_CUBE]) return;
	if (mtmp->data == &mons[PM_BOXER]) return;
	if (mtmp->data == &mons[PM_CHESTER]) return;

	for(obj=mtmp->minvent; obj; obj=obj->nobj) {
		if (Is_container(obj)) {
			if (obj->otyp == MEDICAL_KIT) continue;
			if (obj->otyp == LEAD_BOX) continue;
			if (obj->otyp == DISPERSION_BOX) continue;
			if (obj->otyp == BAG_OF_TRICKS) continue;
			if (obj->olocked) continue; /* don't bother with keys, lock traps etc. */
			if (!Has_contents(obj)) continue;

			/* cursed holding, or digestion, can now erase stuff just as if you had used it */
			if (Is_mbag(obj) && obj->cursed) {
			    for (curr = obj->cobj; curr; curr = otmp) {
				otmp = curr->nobj;
				if (!rn2(13) && !evades_destruction(curr) && !stack_too_big(curr)) {
				    (void) mbag_item_gone(0, curr, FALSE);
				}
			    }

			}

			if ( (obj->otyp == BAG_OF_DIGESTION || obj->otyp == ICE_BOX_OF_DIGESTION || obj->otyp == LARGE_BOX_OF_DIGESTION) && !rn2(obj->blessed ? 20 : (obj->cursed ? 2 : 10))) {
			    for (curr = obj->cobj; curr; curr = otmp) {
				otmp = curr->nobj;
				if (!evades_destruction(curr) && !stack_too_big(curr)) {
				    (void) mbag_item_gone(0, curr, FALSE);
				}
			    }
			}

			/* the above could have resulted in an empty container now, in which case we don't bother */
			if (!Has_contents(obj)) continue;

			if (canseemon(mtmp)) pline("%s empties a container on the %s.", Monnam(mtmp), surface(mtmp->mx, mtmp->my));
			if (mtmp->data == &mons[PM_BOXPORT_NYMPH]) dump_container_tele(obj, FALSE, mtmp->mx, mtmp->my);
			else if (mtmp->data == &mons[PM_HURO_NYMPH]) dump_container_huro(obj, FALSE, mtmp->mx, mtmp->my);
			else if (mtmp->data == &mons[PM_SUPERHURO_NYMPH]) dump_container_superhuro(obj, FALSE, mtmp->mx, mtmp->my);
			else dump_container(obj, FALSE, mtmp->mx, mtmp->my);

		}

	}

}

/*muse.c*/
