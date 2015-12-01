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
STATIC_DCL struct permonst *FDECL(muse_newcham_mon, (struct monst *));
#endif
STATIC_DCL int FDECL(precheck, (struct monst *,struct obj *));
STATIC_DCL void FDECL(mzapmsg, (struct monst *,struct obj *,BOOLEAN_P));
STATIC_DCL void FDECL(mreadmsg, (struct monst *,struct obj *));
STATIC_DCL void FDECL(mquaffmsg, (struct monst *,struct obj *));
STATIC_PTR int FDECL(mbhitm, (struct monst *,struct obj *));
STATIC_DCL void FDECL(mbhit,
	(struct monst *,int,int FDECL((*),(MONST_P,OBJ_P)),
	int FDECL((*),(OBJ_P,OBJ_P)),struct obj *));
STATIC_DCL void FDECL(you_aggravate, (struct monst *));
STATIC_DCL void FDECL(mon_consume_unstone, (struct monst *,struct obj *,
	BOOLEAN_P,BOOLEAN_P));

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
genericptr_t poolcnt;
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
		if (randomx && randomy && isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

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

	if ((ttmp = t_at(x, y)) != 0 && !delfloortrap(ttmp))
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
genericptr_t poolcnt;
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
		if (randomx && randomy && isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

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

	if ((ttmp = t_at(x, y)) != 0 && !delfloortrap(ttmp))
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
do_lockfloodd(x, y, poolcnt)
int x, y;
genericptr_t poolcnt;
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
		if (randomx && randomy && isok(randomx, randomy) && ((levl[randomx][randomy].wall_info & W_NONDIGGABLE) == 0) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR || (levl[randomx][randomy].typ == DOOR && levl[randomx][randomy].doormask == D_NODOOR) ) ) {

			if (rn2(3)) doorlockX(randomx, randomy);
			else {
				if (levl[randomx][randomy].typ != DOOR) levl[randomx][randomy].typ = STONE;
				else levl[randomx][randomy].typ = CROSSWALL;
				block_point(randomx,randomy);
				del_engr_at(randomx, randomy);

				if ((mtmp = m_at(randomx, randomy)) != 0) {
					(void) minliquid(mtmp);
				} else {
					newsym(randomx,randomy);
				}

			}
		}
	}
	if (rn2(3)) doorlockX(x, y);

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].wall_info & W_NONDIGGABLE) != 0 || (levl[x][y].typ != CORR && levl[x][y].typ != ROOM && (levl[x][y].typ != DOOR || levl[x][y].doormask != D_NODOOR) ))
		return;

	if ((ttmp = t_at(x, y)) != 0 && !delfloortrap(ttmp))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a wall at x, y */
		if (levl[x][y].typ != DOOR) levl[x][y].typ = STONE;
		else levl[x][y].typ = CROSSWALL;
		block_point(x,y);
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
genericptr_t poolcnt;
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
		if (randomx && randomy && isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = TREE;
			block_point(randomx,randomy);
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

	if ((ttmp = t_at(x, y)) != 0 && !delfloortrap(ttmp))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = TREE;
		block_point(x,y);
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
genericptr_t poolcnt;
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
		if (randomx && randomy && isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
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

	if ((ttmp = t_at(x, y)) != 0 && !delfloortrap(ttmp))
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
genericptr_t poolcnt;
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
		if (randomx && randomy && isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = CLOUD;
			block_point(randomx,randomy);
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

	if ((ttmp = t_at(x, y)) != 0 && !delfloortrap(ttmp))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = CLOUD;
		block_point(x,y);
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
genericptr_t poolcnt;
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
		if (randomx && randomy && isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = IRONBARS;
			block_point(randomx,randomy);
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

	if ((ttmp = t_at(x, y)) != 0 && !delfloortrap(ttmp))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = IRONBARS;
		block_point(x,y);
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
genericptr_t poolcnt;
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
		if (randomx && randomy && isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = randomwalltype();
			block_point(randomx,randomy);
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

	if ((ttmp = t_at(x, y)) != 0 && !delfloortrap(ttmp))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = randomwalltype();
		block_point(x,y);
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
	    const char *potion_descr;
	    struct monst *mtmp;
#define POTION_OCCUPANT_CHANCE(n) (13 + 2*(n))	/* also in potion.c */

	    potion_descr = OBJ_DESCR(objects[obj->otyp]);
	    if (potion_descr && (!strcmp(potion_descr, "milky") || !strcmp(potion_descr, "ghostly") || !strcmp(potion_descr, "hallowed") || !strcmp(potion_descr, "spiritual")) ) {
	        if ( flags.ghost_count < MAXMONNO &&
		    !rn2(POTION_OCCUPANT_CHANCE(flags.ghost_count))) {
		    if (!enexto(&cc, mon->mx, mon->my, &mons[PM_GHOST])) return 0;
		    mquaffmsg(mon, obj);
		    m_useup(mon, obj);
		    mtmp = makemon(&mons[PM_GHOST], cc.x, cc.y, NO_MM_FLAGS);
		    if (!mtmp) {
			if (vis) pline(empty);
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
	    if (potion_descr && !strcmp(potion_descr, "smoky") &&
		    flags.djinni_count < MAXMONNO &&
		    !rn2(POTION_OCCUPANT_CHANCE(flags.djinni_count))) {
		if (!enexto(&cc, mon->mx, mon->my, &mons[PM_DJINNI])) return 0;
		mquaffmsg(mon, obj);
		m_useup(mon, obj);
		mtmp = makemon(&mons[PM_DJINNI], cc.x, cc.y, NO_MM_FLAGS);
		if (!mtmp) {
		    if (vis) pline(empty);
		} else {
		    if (vis)
			pline("In a cloud of smoke, %s emerges!",
							a_monnam(mtmp));
		    pline("%s speaks.", vis ? Monnam(mtmp) : Something);
		/* I suspect few players will be upset that monsters */
		/* can't wish for wands of death here.... */

		/* Amy edit: I am a "few player" obviously. But then, what did you expect of a variant whose author is into BDSM? I'll allow monsters to wish for random musable items; this is different from certain other variants, and may be changed someday, but for now it's good enough for me. */

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
	    if (potion_descr && !strcmp(potion_descr, "vapor") &&
		    flags.dao_count < MAXMONNO &&
		    !rn2(POTION_OCCUPANT_CHANCE(flags.dao_count))) {
		if (!enexto(&cc, mon->mx, mon->my, &mons[PM_DAO])) return 0;
		mquaffmsg(mon, obj);
		m_useup(mon, obj);
		mtmp = makemon(&mons[PM_DAO], cc.x, cc.y, NO_MM_FLAGS);
		if (!mtmp) {
		    if (vis) pline(empty);
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
	    if (potion_descr && !strcmp(potion_descr, "fuming") &&
		    flags.efreeti_count < MAXMONNO &&
		    !rn2(POTION_OCCUPANT_CHANCE(flags.efreeti_count))) {
		if (!enexto(&cc, mon->mx, mon->my, &mons[PM_EFREETI])) return 0;
		mquaffmsg(mon, obj);
		m_useup(mon, obj);
		mtmp = makemon(&mons[PM_EFREETI], cc.x, cc.y, NO_MM_FLAGS);
		if (!mtmp) {
		    if (vis) pline(empty);
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
	    if (potion_descr && !strcmp(potion_descr, "sizzling") &&
		    flags.marid_count < MAXMONNO &&
		    !rn2(POTION_OCCUPANT_CHANCE(flags.marid_count))) {
		if (!enexto(&cc, mon->mx, mon->my, &mons[PM_MARID])) return 0;
		mquaffmsg(mon, obj);
		m_useup(mon, obj);
		mtmp = makemon(&mons[PM_MARID], cc.x, cc.y, NO_MM_FLAGS);
		if (!mtmp) {
		    if (vis) pline(empty);
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
}

STATIC_OVL void
mreadmsg(mtmp, otmp)
struct monst *mtmp;
struct obj *otmp;
{
	boolean vismon = canseemon(mtmp);
	char onambuf[BUFSZ];
	short saverole;
	unsigned savebknown;

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
	Strcpy(onambuf, singular(otmp, doname));
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
		if (flags.soundok)
			You_hear("a chugging sound.");
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

	/* "Revert some Fake Difficulty. Mindless monsters and animals should not be able to wear armor, nor use potions/scrolls/wands." Dunno why that's supposed to be fake difficulty, but in Soviet Russia, players apparently can't handle dogs zapping wands of lightning or something. Everyone else will have to contend with monsters using items the way they're supposed to. --Amy */

	if ((is_animal(mtmp->data) || mindless(mtmp->data)) && issoviet)
		return FALSE;
	if(dist2(x, y, mtmp->mux, mtmp->muy) > 25)
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
	    if ((obj = m_carrying(mtmp, SCR_HEALING)) != 0) {
		m.defensive = obj;
		m.has_defense = MUSE_SCR_HEALING;
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
		if ((obj = m_carrying(mtmp, SCR_HEALING)) != 0) {
		    m.defensive = obj;
		    m.has_defense = MUSE_SCR_HEALING;
		    return TRUE;
		}
		if (is_vampire(mtmp->data) &&
		  (obj = m_carrying(mtmp, POT_VAMPIRE_BLOOD)) !=0) {
		    m.defensive = obj;
		    m.has_defense = MUSE_POT_VAMPIRE_BLOOD;
		    return TRUE;
		}
	    /*}*/
	    return FALSE;
	}

	if (levl[x][y].typ == STAIRS && !stuck && !immobile) {
		if (x == xdnstair && y == ydnstair && !is_floater(mtmp->data))
			m.has_defense = MUSE_DOWNSTAIRS;
		if (x == xupstair && y == yupstair && ledger_no(&u.uz) != 1)
	/* Unfair to let the monsters leave the dungeon with the Amulet */
	/* (or go to the endlevel since you also need it, to get there) */
			m.has_defense = MUSE_UPSTAIRS;
	} else if (levl[x][y].typ == LADDER && !stuck && !immobile) {
		if (x == xupladder && y == yupladder)
			m.has_defense = MUSE_UP_LADDER;
		if (x == xdnladder && y == ydnladder && !is_floater(mtmp->data))
			m.has_defense = MUSE_DN_LADDER;
	} else if (sstairs.sx && sstairs.sx == x && sstairs.sy == y) {
		m.has_defense = MUSE_SSTAIRS;
	} else if (!stuck && !immobile) {
	/* Note: trap doors take precedence over teleport traps. */
		int xx, yy;

		for(xx = x-1; xx <= x+1; xx++) for(yy = y-1; yy <= y+1; yy++)
		if (isok(xx,yy))
		if (xx != u.ux && yy != u.uy)
		if ((mtmp->data != &mons[PM_GRID_BUG] && mtmp->data != &mons[PM_WEREGRIDBUG] && mtmp->data != &mons[PM_GRID_XORN] && mtmp->data != &mons[PM_STONE_BUG] && mtmp->data != &mons[PM_NATURAL_BUG] && mtmp->data != &mons[PM_MELEE_BUG] && mtmp->data != &mons[PM_WEAPON_BUG]) || xx == x || yy == y)
		if ((xx==x && yy==y) || !level.monsters[xx][yy])
		if ((t = t_at(xx,yy)) != 0)
		if ((verysmall(mtmp->data) || throws_rocks(mtmp->data) ||
		     passes_walls(mtmp->data) || (mtmp->egotype_wallwalk) ) || !sobj_at(BOULDER, xx, yy))
		if (!onscary(xx,yy,mtmp)) {
			if ((t->ttyp == TRAPDOOR || t->ttyp == HOLE || t->ttyp == SHAFT_TRAP)
				&& !is_floater(mtmp->data)
				&& !mtmp->isshk && !mtmp->isgd
				&& !mtmp->ispriest
				&& Can_fall_thru(&u.uz)
						) {
				trapx = xx;
				trapy = yy;
				m.has_defense = MUSE_TRAPDOOR;
			} else if (t->ttyp == TELEP_TRAP && m.has_defense != MUSE_TRAPDOOR) {
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
	if (t && (t->ttyp == PIT || t->ttyp == SPIKED_PIT || t->ttyp == GIANT_CHASM || t->ttyp == SHIT_PIT ||
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
		    if (!level.flags.noteleport ||
			!(mtmp->mtrapseen & (1 << (TELEP_TRAP-1)))) {
			m.defensive = obj;
			m.has_defense = (mon_has_amulet(mtmp))
				? MUSE_WAN_TELEPORTATION
				: MUSE_WAN_TELEPORTATION_SELF;
		    }
		}
		nomore(MUSE_SCR_TELEPORTATION);
		if(obj->otyp == SCR_TELEPORTATION && mtmp->mcansee
		   && haseyes(mtmp->data)
		   && (!obj->cursed ||
		       (!(mtmp->isshk && inhishop(mtmp))
			    && !mtmp->isgd && !mtmp->ispriest))) {
		    /* see WAN_TELEPORTATION case above */
		    if (!level.flags.noteleport ||
			!(mtmp->mtrapseen & (1 << (TELEP_TRAP-1)))) {
			m.defensive = obj;
			m.has_defense = MUSE_SCR_TELEPORTATION;
		    }
		}

		nomore(MUSE_SCR_ROOT_PASSWORD_DETECTION);
		if(obj->otyp == SCR_ROOT_PASSWORD_DETECTION
		   && (!(mtmp->isshk && inhishop(mtmp))
			    && !mtmp->isgd && !mtmp->ispriest)) {
			m.defensive = obj;
			m.has_defense = MUSE_SCR_ROOT_PASSWORD_DETECTION;
		}

		nomore(MUSE_SCR_TELE_LEVEL);
		if(obj->otyp == SCR_TELE_LEVEL
		   && (!(mtmp->isshk && inhishop(mtmp))
			    && !mtmp->isgd && !mtmp->ispriest)) {
			m.defensive = obj;
			m.has_defense = MUSE_SCR_TELE_LEVEL;
		}

		nomore(MUSE_WAN_TELE_LEVEL);
		if(obj->otyp == WAN_TELE_LEVEL && obj->spe > 0
		   && (!(mtmp->isshk && inhishop(mtmp))
			    && !mtmp->isgd && !mtmp->ispriest)) {
			m.defensive = obj;
			m.has_defense = MUSE_WAN_TELE_LEVEL;
		}

		nomore(MUSE_SCR_WARPING);
		if(obj->otyp == SCR_WARPING
		   && (!(mtmp->isshk && inhishop(mtmp))
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

	    if (mtmp->data != &mons[PM_PESTILENCE]) {
		nomore(MUSE_POT_FULL_HEALING);
		if(obj->otyp == POT_FULL_HEALING) {
			m.defensive = obj;
			m.has_defense = MUSE_POT_FULL_HEALING;
		}
		nomore(MUSE_POT_EXTRA_HEALING);
		if(obj->otyp == POT_EXTRA_HEALING) {
			m.defensive = obj;
			m.has_defense = MUSE_POT_EXTRA_HEALING;
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
		nomore(MUSE_SCR_HEALING);
		if(obj->otyp == SCR_HEALING) {
			m.defensive = obj;
			m.has_defense = MUSE_SCR_HEALING;
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
		nomore(MUSE_POT_VAMPIRE_BLOOD);
		if(is_vampire(mtmp->data) && obj->otyp == POT_VAMPIRE_BLOOD) {
			m.defensive = obj;
			m.has_defense = MUSE_POT_VAMPIRE_BLOOD;
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
	    }
		nomore(MUSE_SCR_SUMMON_BOSS);
		if(obj->otyp == SCR_SUMMON_BOSS) {
			m.defensive = obj;
			m.has_defense = MUSE_SCR_SUMMON_BOSS;
		}
		nomore(MUSE_SCR_CREATE_MONSTER);
		if(obj->otyp == SCR_CREATE_MONSTER) {
			m.defensive = obj;
			m.has_defense = MUSE_SCR_CREATE_MONSTER;
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
		if (vismon)
			pline("%s plays %s!", Monnam(mtmp), doname(otmp));
		else if (flags.soundok)
			You_hear("a bugle playing reveille!");
		awaken_soldiers();
		return 2;
	case MUSE_WAN_TELEPORTATION_SELF:
		if ((mtmp->isshk && inhishop(mtmp))
		       || mtmp->isgd || mtmp->ispriest) return 2;
		m_flee(mtmp);
		mzapmsg(mtmp, otmp, TRUE);
		if (rn2(2) || !ishaxor) otmp->spe--;
		how = WAN_TELEPORTATION;
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
mon_tele:
		if (tele_restrict(mtmp)) {	/* mysterious force... */
		    if (vismon && how)		/* mentions 'teleport' */
			makeknown(how);
		    /* monster learns that teleportation isn't useful here */
		    if (level.flags.noteleport)
			mtmp->mtrapseen |= (1 << (TELEP_TRAP-1));
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
		if (rn2(2) || !ishaxor) otmp->spe--;
		m_using = TRUE;
		mbhit(mtmp,rn1(8,6),mbhitm,bhito,otmp);
		/* monster learns that teleportation isn't useful here */
		if (level.flags.noteleport)
		    mtmp->mtrapseen |= (1 << (TELEP_TRAP-1));
		m_using = FALSE;
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;
	case MUSE_SCR_TELEPORTATION:
	    {
		int obj_is_cursed = otmp->cursed;

		if (mtmp->isshk || mtmp->isgd || mtmp->ispriest) return 2;
		m_flee(mtmp);
		mreadmsg(mtmp, otmp);
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */
		how = SCR_TELEPORTATION;
		if (obj_is_cursed || mtmp->mconf) {
			int nlev;
			d_level flev;

			if (mon_has_amulet(mtmp) || In_endgame(&u.uz)) {
			    if (vismon)
				pline("%s seems very disoriented for a moment.",
					Monnam(mtmp));
			    return 2;
			}
			nlev = random_teleport_level();
			if (nlev == depth(&u.uz)) {
			    if (vismon)
				pline("%s shudders for a moment.",
								Monnam(mtmp));
			    return 2;
			}
			get_level(&flev, nlev);
			migrate_to_level(mtmp, ledger_no(&flev), MIGR_RANDOM,
				(coord *)0);
			if (oseen) makeknown(SCR_TELEPORTATION);
		} else goto mon_tele;
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
			    if (vismon)
				pline("%s seems very disoriented for a moment.",
					Monnam(mtmp));
			    return 2;
			}
			nlev = random_teleport_level();
			if (nlev == depth(&u.uz)) {
			    if (vismon)
				pline("%s shudders for a moment.",
								Monnam(mtmp));
			    return 2;
			}
			get_level(&flev, nlev);
			migrate_to_level(mtmp, ledger_no(&flev), MIGR_RANDOM,
				(coord *)0);
			if (oseen) makeknown(SCR_TELE_LEVEL);

		return 2;
	    }
	case MUSE_WAN_TELE_LEVEL:
		if (mtmp->isshk || mtmp->isgd || mtmp->ispriest) return 2;
		m_flee(mtmp);
		mzapmsg(mtmp, otmp, TRUE);
		if (rn2(2) || !ishaxor) otmp->spe--;
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		how = WAN_TELE_LEVEL;
			int nlev;
			d_level flev;

			if (mon_has_amulet(mtmp) || In_endgame(&u.uz)) {
			    if (vismon)
				pline("%s seems very disoriented for a moment.",
					Monnam(mtmp));
			    return 2;
			}
			nlev = random_teleport_level();
			if (nlev == depth(&u.uz)) {
			    if (vismon)
				pline("%s shudders for a moment.",
								Monnam(mtmp));
			    return 2;
			}
			get_level(&flev, nlev);
			migrate_to_level(mtmp, ledger_no(&flev), MIGR_RANDOM,
				(coord *)0);
			if (oseen) makeknown(SCR_TELE_LEVEL);

		return 2;

	case MUSE_SCR_WARPING:
	    {
		if (mtmp->isshk || mtmp->isgd || mtmp->ispriest) return 2;
		m_flee(mtmp);
		mreadmsg(mtmp, otmp);

		if (!objects[SCR_WARPING].oc_name_known
			&& !objects[SCR_WARPING].oc_uname)
		    docall(otmp);

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		if (u.uevent.udemigod) { (void) rloc(mtmp, FALSE); return 2; }
		u_teleport_monB(mtmp, TRUE);

		return 2;
	    }
	case MUSE_SCR_ROOT_PASSWORD_DETECTION:
	    {
		if (mtmp->isshk || mtmp->isgd || mtmp->ispriest) return 2;
		m_flee(mtmp);
		mreadmsg(mtmp, otmp);

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		how = SCR_ROOT_PASSWORD_DETECTION;

			int nlev;
			d_level flev;

			if (mon_has_amulet(mtmp) || In_endgame(&u.uz)) {
			    if (vismon)
				pline("%s seems very disoriented for a moment.",
					Monnam(mtmp));
			    return 2;
			}
			nlev = random_teleport_level();
			if (nlev == depth(&u.uz)) {
			    if (vismon)
				pline("%s shudders for a moment.",
								Monnam(mtmp));
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
		if (rn2(2) || !ishaxor) otmp->spe--;
		if (oseen) makeknown(WAN_DIGGING);
		if (IS_FURNITURE(levl[mtmp->mx][mtmp->my].typ) ||
		    IS_DRAWBRIDGE(levl[mtmp->mx][mtmp->my].typ) ||
		    (is_drawbridge_wall(mtmp->mx, mtmp->my) >= 0) ||
		    (sstairs.sx && sstairs.sx == mtmp->mx &&
				   sstairs.sy == mtmp->my)) {
			pline_The("digging ray is ineffective.");
			return 2;
		}
		if (!Can_dig_down(&u.uz)) {
		    if(canseemon(mtmp))
			pline_The("%s here is too hard to dig in.",
					surface(mtmp->mx, mtmp->my));
		    return 2;
		}
		ttmp = maketrap(mtmp->mx, mtmp->my, HOLE);
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
		/*if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);*/	/* segfaults possible... --Amy */
		return 2;
	    }
	case MUSE_WAN_CREATE_HORDE:
	    {   coord cc;
		struct permonst *pm=rndmonst();
		int cnt = 1;
		if (!enexto(&cc, mtmp->mx, mtmp->my, pm)) return 0;
		mzapmsg(mtmp, otmp, FALSE);
		if (rn2(2) || !ishaxor) otmp->spe--;
		if (oseen) makeknown(WAN_CREATE_HORDE);
		cnt = rnd(4) + 10;
		while(cnt--) {
			struct monst *mon;
			if (!enexto(&cc, mtmp->mx, mtmp->my, pm)) continue;
			mon = makemon(rndmonst(), cc.x, cc.y, NO_MM_FLAGS);
			if (mon) newsym(mon->mx,mon->my);
		}
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;
	    }
	case MUSE_WAN_CREATE_MONSTER:
	    {	coord cc;

		struct permonst *pm = 0;
		struct monst *mon;

		if (!enexto(&cc, mtmp->mx, mtmp->my, pm)) return 0;
		mzapmsg(mtmp, otmp, FALSE);
		if (rn2(2) || !ishaxor) otmp->spe--;
		mon = makemon((struct permonst *)0, cc.x, cc.y, NO_MM_FLAGS);
		if (mon && canspotmon(mon) && oseen)
		    makeknown(WAN_CREATE_MONSTER);
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;
	    }

	case MUSE_SCR_CREATE_MONSTER:
	    {	coord cc;
		struct permonst *pm = 0, *fish = 0;
		int cnt = 1;
		struct monst *mon;
		boolean known = FALSE;

		if (!rn2(73)) cnt += rnd(4);
		if (mtmp->mconf || otmp->cursed) cnt += 12;
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

		mon = makemon((struct permonst *)0, cc.x, cc.y, NO_MM_FLAGS);
		if (mon && canspotmon(mon) && oseen)
		    makeknown(BAG_OF_TRICKS);
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;
	    }

	case MUSE_WAN_SUMMON_UNDEAD:
	    {	coord cc;

		struct permonst *pm = 0;
		struct monst *mon;

		if (!enexto(&cc, mtmp->mx, mtmp->my, pm)) return 0;
		mzapmsg(mtmp, otmp, FALSE);
		if (rn2(2) || !ishaxor) otmp->spe--;

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
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;
	    }

	case MUSE_SCR_SUMMON_UNDEAD:

	    {	coord cc;
		struct permonst *pm = 0;
		int cnt = 1;
		struct monst *mon;
		boolean known = FALSE;

		if (rn2(2)) cnt += rnz(2);
		if (!rn2(73)) cnt += rnd(4);
		if (mtmp->mconf || otmp->cursed) cnt += 12;
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

		mreadmsg(mtmp, otmp);

newboss:
		do {
			pm = rndmonst();
			attempts++;

		} while ( (!pm || (pm && !(pm->geno & G_UNIQ))) && attempts < 50000);

		if (!pm && rn2(50) ) {
			attempts = 0;
			goto newboss;
		}
		if (pm && !(pm->geno & G_UNIQ) && rn2(50) ) {
			attempts = 0;
			goto newboss;
		}

		mon = makemon(pm, cc.x, cc.y, NO_MM_FLAGS);
	      if (mon && canspotmon(mon)) known = TRUE;

		if (known)
		    makeknown(SCR_SUMMON_BOSS);
		else if (!objects[SCR_SUMMON_BOSS].oc_name_known
			&& !objects[SCR_SUMMON_BOSS].oc_uname)
		    docall(otmp);
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;
	    }

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
			t->ttyp == TRAPDOOR ? "trap door" : t->ttyp == SHAFT_TRAP ? "shaft" : "hole");
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
		if (rn2(2) || !ishaxor) otmp->spe--;
		i = d(5,2) + 5 * !!bcsign(otmp);
		mtmp->mhp += i;
		if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = ++mtmp->mhpmax;
		if (!otmp->cursed) mtmp->mcansee = 1;
		if (vismon) pline("%s begins to look better.", Monnam(mtmp));
		if (oseen) makeknown(WAN_HEALING);
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;
	case MUSE_WAN_EXTRA_HEALING:
		mzapmsg(mtmp, otmp, TRUE);
		if (rn2(2) || !ishaxor) otmp->spe--;
		i = d(5,4) + 10 * !!bcsign(otmp);
		mtmp->mhp += i;
		if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = ++mtmp->mhpmax;
		if (!otmp->cursed) mtmp->mcansee = 1;
		if (vismon) pline("%s begins to look better.", Monnam(mtmp));
		if (oseen) makeknown(WAN_EXTRA_HEALING);
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;
	case MUSE_WAN_FULL_HEALING:
		mzapmsg(mtmp, otmp, TRUE);
		if (rn2(2) || !ishaxor) otmp->spe--;
		i = d(5,8) + 20 * !!bcsign(otmp);
		mtmp->mhp += i;
		if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = ++mtmp->mhpmax;
		if (!otmp->cursed) mtmp->mcansee = 1;
		if (vismon) pline("%s begins to look better.", Monnam(mtmp));
		if (oseen) makeknown(WAN_FULL_HEALING);
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_SCR_HEALING:

		mreadmsg(mtmp, otmp);
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		if (!rn2(20)) i = mtmp->mhpmax;
		else if (!rn2(5)) i = d(8, 8);
		else i = d(8, 4);
		mtmp->mhp += i;
		if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;

		if (vismon) pline("%s looks better.", Monnam(mtmp));
		if (oseen) makeknown(SCR_HEALING);
		/*m_useup(mtmp, otmp); */
		/* the wrong dual use of m_useup caused the game to destabilize!!! --Amy
		 * I'm sure nobody wants to know how much headache and nausea this bug has caused to me.
		 * But I mean, why the motherfucking hell does the game just close, with no error message???
		 * Actually there's the panic code for a reason! */
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
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;
	case MUSE_POT_EXTRA_HEALING:
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
		    i = rnd(8) + rnd(2);
		    mtmp->mhp += i;
		    mtmp->mhpmax += i;
		    if (vismon) pline("%s looks full of life.", Monnam(mtmp));
		}
		else if (vismon)
		    pline("%s discards the congealed blood in disgust.", Monnam(mtmp));
		if (oseen) makeknown(POT_VAMPIRE_BLOOD);
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
# ifdef KOPS
			|| pm->mlet == S_KOP
# endif
		) && issoviet) return 0;
    try_again:
	switch (rn2(8 + (difficulty > 3) + (difficulty > 6) +
				(difficulty > 8))) {
		case 6: case 9:
			if (level.flags.noteleport && ++trycnt < 2)
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
# ifdef KOPS
			|| pm->mlet == S_KOP
# endif
		) && issoviet) return 0;
	switch (rn2(25)) {

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
	if (mtmp->mpeaceful || ((is_animal(mtmp->data) ||
				mindless(mtmp->data) || nohands(mtmp->data)) && issoviet) )
		return FALSE;
	if (u.uswallow) return FALSE;
	if (in_your_sanctuary(mtmp, 0, 0)) return FALSE;
	if (dmgtype(mtmp->data, AD_HEAL) && !uwep
#ifdef TOURIST
	    && !uarmu
#endif
	    && !uarm && !uarmh && !uarms && !uarmg && !uarmc && !uarmf)
		return FALSE;

	if (!ranged_stuff) return FALSE;
#define nomore(x) if(m.has_offense==x) continue;
	for(obj=mtmp->minvent; obj; obj=obj->nobj) {
		/* nomore(MUSE_WAN_DEATH); */
		if (!reflection_skip) {
		    if(obj->otyp == WAN_DEATH && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_DEATH;
		    }
		    nomore(MUSE_WAN_SLEEP);
		    if(obj->otyp == WAN_SLEEP && obj->spe > 0 && multi >= 0) {
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

		    nomore(MUSE_WAN_FIREBALL);                    
		    if(obj->otyp == WAN_FIREBALL && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_FIREBALL;
                    }
		    nomore(MUSE_WAN_ACID);                    
		    if(obj->otyp == WAN_ACID && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_ACID;
                    }
		    nomore(MUSE_WAN_SOLAR_BEAM);                    
		    if(obj->otyp == WAN_SOLAR_BEAM && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_SOLAR_BEAM;
                    }
		    nomore(MUSE_WAN_FIRE);
		    if(obj->otyp == WAN_FIRE && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_FIRE;
		    }
		    nomore(MUSE_FIRE_HORN);
		    if(obj->otyp == FIRE_HORN && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_FIRE_HORN;
		    }
		    nomore(MUSE_WAN_COLD);
		    if(obj->otyp == WAN_COLD && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_COLD;
		    }
		    nomore(MUSE_FROST_HORN);
		    if(obj->otyp == FROST_HORN && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_FROST_HORN;
		    }
		    nomore(MUSE_WAN_LIGHTNING);
		    if(obj->otyp == WAN_LIGHTNING && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_LIGHTNING;
		    }
		    nomore(MUSE_WAN_MAGIC_MISSILE);
		    if(obj->otyp == WAN_MAGIC_MISSILE && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_MAGIC_MISSILE;
		    }
		}
		nomore(MUSE_WAN_DRAINING);
		if(obj->otyp == WAN_DRAINING && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_DRAINING;
		}
		nomore(MUSE_WAN_FEAR);
		if(obj->otyp == WAN_FEAR && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_FEAR;
		}
		nomore(MUSE_WAN_SLOW_MONSTER);
		if(obj->otyp == WAN_SLOW_MONSTER && obj->spe > 0 && (HFast & (TIMEOUT | INTRINSIC)) ) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_SLOW_MONSTER;
		}
		nomore(MUSE_WAN_STRIKING);
		if(obj->otyp == WAN_STRIKING && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_STRIKING;
		}
		nomore(MUSE_WAN_BANISHMENT);
		if(obj->otyp == WAN_BANISHMENT && obj->spe > 0 && !u.banishmentbeam) {
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
		nomore(MUSE_WAN_CURSE_ITEMS);
		if(obj->otyp == WAN_CURSE_ITEMS && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_CURSE_ITEMS;
		}
		nomore(MUSE_WAN_BAD_LUCK);
		if(obj->otyp == WAN_BAD_LUCK && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_BAD_LUCK;
		}
		nomore(MUSE_WAN_FUMBLING);
		if(obj->otyp == WAN_FUMBLING && obj->spe > 0 && !Fumbling) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_FUMBLING;
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
		if(obj->otyp == WAN_CONFUSION && obj->spe > 0) {
			m.offensive = obj;
			m.has_offense = MUSE_WAN_CONFUSION;
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
			if (Antimagic) {
			    shieldeff(u.ux, u.uy);
			    pline("Boing!");
			} else if (rnd(20) < 10 + u.uac || !rn2(3) ) { /* good ac will no longer be 100% protection --Amy */
			    pline_The("wand hits you!");
			    tmp = d(2,12);
			    if(Half_spell_damage && rn2(2) ) tmp = (tmp+1) / 2;
			    losehp(tmp, "wand", KILLED_BY_AN);
			} else pline_The("wand misses you.");
			stop_occupation();
			nomul(0, 0);
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

		if (u.uevent.udemigod || u.uhave.amulet || NoReturnEffect || u.uprops[NORETURN].extrinsic || have_noreturnstone() || (u.usteed && mon_has_amulet(u.usteed))) { pline("You shudder for a moment."); (void) safe_teleds(FALSE);  break; }

		if (flags.lostsoul || flags.uberlostsoul || u.uprops[STORM_HELM].extrinsic) { 
		pline("Somehow, the banishment beam doesn't do anything."); break;}

		if (mtmp == &youmonst) {
			u.banishmentbeam = 1;
			nomul(-2, "being banished"); /* because it's not called until you get another turn... */
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

			if (!Free_action) {
			    pline("You are frozen in place!");
			    nomul(-rnz(5), "frozen by a wand");
			    nomovemsg = You_can_move_again;
			    exercise(A_DEX, FALSE);
			} else You("stiffen momentarily.");

		} else {

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
			Strcpy(nambuf, Monnam(mtmp));
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

			if (Disint_resistance && rn2(100)) {
			    You("are not disintegrated.");
			    break;
	            } else if (Invulnerable || (Stoned_chiller && Stoned)) {
	                pline("You are unharmed!");
	                break;
			} else if (uarms) {
			    /* destroy shield; other possessions are safe */
			    (void) destroy_arm(uarms);
			    break;
			} else if (uarmc) {
			    /* destroy cloak; other possessions are safe */
			    (void) destroy_arm(uarmc);
			    break;
			} else if (uarm) {
			    /* destroy suit */
			    (void) destroy_arm(uarm);
			    break;
	#ifdef TOURIST
			} else if (uarmu) {
			    /* destroy shirt */
			    (void) destroy_arm(uarmu);
			    break;
	#endif
			}
			killer_format = KILLED_BY_AN;
			killer = "wand of disintegration";
		      done(DIED);
			return 0;

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
#ifdef TOURIST
			} else if (mtmp->misc_worn_check & W_ARMU) {
			    /* destroy shirt */
			    otmpS = which_armor(mtmp, W_ARMU);
			    pline("%s %s is disintegrated!", s_suffix(Monnam(mtmp)), distant_name(otmpS, xname));
			    m_useup(mtmp, otmpS);
			    break;
#endif
			}
		    else {

			struct obj *otmpX, *otmpX2, *m_amulet = mlifesaver(mtmp);

#define oresist_disintegration(obj) \
		(objects[obj->otyp].oc_oprop == DISINT_RES || \
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

		    if (!Stone_resistance &&
			!(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {
			if (!Stoned) { Stoned = 7;
				pline("You start turning to stone!");
			}
			Sprintf(killer_buf, "wand of stoning");
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
			if (Drain_resistance) {
				shieldeff(u.ux, u.uy);
				pline("Boing!");
			} else
				losexp("life drainage", FALSE, TRUE);
			if (zap_oseen)
				makeknown(WAN_DRAINING);
			stop_occupation();
			nomul(0, 0);
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
	case WAN_REDUCE_MAX_HITPOINTS:	/* evil patch idea by jonadab */

	/* he wanted it to be a wand of halve max hitpoints, but that would be too evil even for this game. --Amy */

		if (mtmp == &youmonst) {
			pline("You feel drained...");
				if (Upolyd) u.mhmax -= rnd(5);
				else u.uhpmax -= rnd(5);
				if (u.mhmax < 1) u.mhmax = 1;
				if (u.uhpmax < 1) u.uhpmax = 1;
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				if (u.mh > u.mhmax) u.mh = u.mhmax;
			if (zap_oseen)
				makeknown(WAN_REDUCE_MAX_HITPOINTS);
			stop_occupation();
			nomul(0, 0);
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
			break;
		} else {

			if (!is_undead(mtmp->data) && (!mtmp->egotype_undead) &&
			    !resist(mtmp, otmp->oclass, 0, NOTELL) &&
			    (!mtmp->mflee || mtmp->mfleetim)) {
			     if (canseemon(mtmp))
				 pline("%s suddenly panics!", Monnam(mtmp));
			     monflee(mtmp, rnd(10), FALSE, TRUE);
			}
			break;

		}
		if (cansee(mtmp->mx, mtmp->my) && zap_oseen)
			makeknown(WAN_FEAR);
		break;
	case WAN_SLOW_MONSTER:
		if (mtmp == &youmonst) {
			u_slow_down();
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
	}


	if (reveal_invis) {
	    if (mtmp->mhp > 0 && cansee(bhitpos.x,bhitpos.y)
							&& !canspotmon(mtmp))
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
int FDECL((*fhitm),(MONST_P,OBJ_P));
int FDECL((*fhito),(OBJ_P,OBJ_P));	/* fns called when mon/obj hit */
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
		if (find_drawbridge(&x,&y))
		    switch (obj->otyp) {
			case WAN_STRIKING:
			    destroy_drawbridge(x,y);
		    }
		if(bhitpos.x==u.ux && bhitpos.y==u.uy) {
			(*fhitm)(&youmonst, obj);
			range -= 3;
		} else if(MON_AT(bhitpos.x, bhitpos.y)){
			mtmp = m_at(bhitpos.x,bhitpos.y);
			if (cansee(bhitpos.x,bhitpos.y) && !canspotmon(mtmp))
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
	case MUSE_WAN_FIRE:
	case MUSE_WAN_COLD:
	case MUSE_WAN_LIGHTNING:
	case MUSE_WAN_MAGIC_MISSILE:
		mzapmsg(mtmp, otmp, FALSE);
		if (rn2(2) || !ishaxor) otmp->spe--;
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

		/* Monsters zapping wands will be more dangerous later in the game. --Amy */
		buzz((int)(-30 - (otmp->otyp - WAN_MAGIC_MISSILE)),
			(otmp->otyp == WAN_MAGIC_MISSILE) ? 2 + (rnd(monster_difficulty()) / 10) : (otmp->otyp == WAN_SOLAR_BEAM) ? 8 + (rnd(monster_difficulty()) / 4) : 6 + (rnd(monster_difficulty()) / 8),
			mtmp->mx, mtmp->my,
			sgn(mtmp->mux-mtmp->mx), sgn(mtmp->muy-mtmp->my));
		m_using = FALSE;
/*                }*/

		if (mtmp->mhp > 0) { /* cutting down on annoying segfaults --Amy */
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		}
		return (mtmp->mhp <= 0) ? 1 : 2;
	case MUSE_FIRE_HORN:
	case MUSE_FROST_HORN:
		if (oseen) {
			makeknown(otmp->otyp);
			pline("%s plays a %s!", Monnam(mtmp), xname(otmp));
		} else
			You_hear("a horn being played.");
		if (rn2(2) || !ishaxor) otmp->spe--;
		m_using = TRUE;
		buzz(-30 - ((otmp->otyp==FROST_HORN) ? AD_COLD-1 : AD_FIRE-1),
			rn1(6,6), mtmp->mx, mtmp->my,
			sgn(mtmp->mux-mtmp->mx), sgn(mtmp->muy-mtmp->my));
		m_using = FALSE;
		if (mtmp->mhp > 0) { /* cutting down on annoying segfaults --Amy */
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		}
		return (mtmp->mhp <= 0) ? 1 : 2;
/*      case MUSE_WAN_TELEPORTATION:*/
	case MUSE_WAN_STRIKING:
	case MUSE_WAN_SLOW_MONSTER:
	case MUSE_WAN_FEAR:
	case MUSE_WAN_DRAINING:	/* KMH */
	case MUSE_WAN_CANCELLATION:  /* Lethe */
	case MUSE_WAN_STONING:
	case MUSE_WAN_REDUCE_MAX_HITPOINTS:
	case MUSE_WAN_PARALYSIS:
	case MUSE_WAN_MAKE_VISIBLE:
	case MUSE_WAN_DISINTEGRATION:
		zap_oseen = oseen;
		mzapmsg(mtmp, otmp, FALSE);
		if (rn2(2) || !ishaxor) otmp->spe--;
		m_using = TRUE;
		mbhit(mtmp,rn1(8,6),mbhitm,bhito,otmp);
		m_using = FALSE;
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;
	case MUSE_WAN_BANISHMENT:
		zap_oseen = oseen;
		mzapmsg(mtmp, otmp, FALSE);
		if (rn2(2) || !ishaxor) otmp->spe--;
		mbhitm(&youmonst,otmp);
		/*if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);*/ /* This was crashing too often. --Amy */
		return 2;
	case MUSE_SCR_ELEMENTALISM:
	    {
		coord cc;
		int cnt = rnd(9);
		if (mtmp->mconf) cnt += rnd(12);
		if (otmp->cursed) cnt += rnd(5);

		if (!enexto(&cc, mtmp->mx, mtmp->my, 0)) break;

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		while(cnt--) {
			if (!enexto(&cc, mtmp->mx, mtmp->my, 0)) break;

			makemon(mkclass(S_ELEMENTAL,0), cc.x, cc.y, NO_MM_FLAGS);
		}
	    }
		pline("The inhabitants of the elemental planes appear!");

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_DEMONOLOGY:
	    {
		coord cc;
		int cnt = rnd(9);
		if (mtmp->mconf) cnt += rnd(12);
		if (otmp->cursed) cnt += rnd(5);

		if (!enexto(&cc, mtmp->mx, mtmp->my, 0)) break;

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		while(cnt--) {
			if (!enexto(&cc, mtmp->mx, mtmp->my, 0)) break;

			makemon(mkclass(S_DEMON,0), cc.x, cc.y, NO_MM_FLAGS);
		}
	    }
		pline("The denizens of Gehennom appear!");

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_GIRLINESS:
		mreadmsg(mtmp, otmp);

	    {
		int cnt = rnd(9);
		if (otmp->cursed) cnt += rnd(18);
		if (mtmp->mconf) cnt += rnd(100);
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

		switch (rnd(64)) {

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
				if (u.bankcashlimit == 0) u.bankcashlimit = rnz(1000 * (monster_difficulty() + 1));
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

		}
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

		      for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
				if (!isok(u.ux + i, u.uy + j)) continue;
				if ((levl[u.ux + i][u.uy + j].typ <= DBWALL) || MON_AT(u.ux + i, u.uy + j)) continue;
				if (t_at(u.ux + i, u.uy + j)) continue;

			      rtrap = randomtrap();

				(void) maketrap(u.ux + i, u.uy + j, rtrap);
			}
		}
		makerandomtrap();
		if (!rn2(2)) makerandomtrap();
		if (!rn2(4)) makerandomtrap();
		if (!rn2(8)) makerandomtrap();
		if (!rn2(16)) makerandomtrap();
		if (!rn2(32)) makerandomtrap();
		if (!rn2(64)) makerandomtrap();
		if (!rn2(128)) makerandomtrap();
		if (!rn2(256)) makerandomtrap();

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

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
			maketrap(u.ux + i, u.uy + j, rn2(5) ? SHIT_TRAP : SHIT_PIT);
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
			do_clear_areaX(u.ux, u.uy, radius, do_floodd, (genericptr_t)&madepool);

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
				do_floodd(u.ux, u.uy, (genericptr_t)&stilldry);
			if (madepool)
				pline(Hallucination ?
						"A totally gnarly wave comes in!" :
						"A flood surges through the area!" );
			if (!stilldry && !Wwalking && !Flying && !Levitation)
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
		else if (Antimagic && rn2(5)) pline("Your body shakes violently!");
		/* and grease will always offer protection but can wear off */
		else if (otmp2 && otmp2->greased) {
			pline("Your body shakes violently!");
			 if (!rn2(2)) {
				pline_The("grease wears off.");
				otmp2->greased = 0;
				update_inventory();
			 }
		}

		else if (!otmp2) pline("Your skin itches.");
	      else if(!destroy_arm(otmp2)) pline("Your skin itches.");
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
			do_clear_areaX(u.ux, u.uy, radiusB, do_lavafloodd, (genericptr_t)&madepoolB);

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
				pline(Hallucination ?
						"Wow, that's, like, TOTALLY HOT, dude!" :
						"A stream of lava surges through the area!" );

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_STONING:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		    if (!Stone_resistance &&
			!(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {
			if (!Stoned) { Stoned = 7;
				pline("You start turning to stone!");
			}
			Sprintf(killer_buf, "a petrification scroll");
			delayed_killer = killer_buf;
		
		    }

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_WOUNDS:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

		You("feel bad!");
			if (!rn2(20)) losehp(d(10,8), "a scroll of wounds", KILLED_BY);
			else if (!rn2(5)) losehp(d(6,8), "a scroll of wounds", KILLED_BY);
			else losehp(d(4,6), "a scroll of wounds", KILLED_BY);

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

		return 2;

	case MUSE_SCR_LOCKOUT:

		mreadmsg(mtmp, otmp);
		makeknown(otmp->otyp);

			int madepoolQ = 0;
			int xQ,yQ,safe_posQ=0;
			int radiusC = 5;
			if (!rn2(3)) radiusC += rnd(4);
			if (!rn2(10)) radiusC += rnd(6);
			if (!rn2(25)) radiusC += rnd(8);
			if (radiusC > MAX_RADIUS) radiusC = MAX_RADIUS;
			do_clear_areaX(u.ux, u.uy, radiusC, do_lockfloodd, (genericptr_t)&madepoolQ);

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
				pline(Hallucination ?
						"It's getting a little bit tight in here!" :
						"Walls and obstacles shoot up from the ground!" );
			else pline(Hallucination ?
						"You hear a grating that reminds you of Chinese water torture!" :
						"You see dust particles flying around." );

		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);	/* otmp might be free'ed */

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
			do_clear_areaX(u.ux, u.uy, radiusD, do_treefloodd, (genericptr_t)&madepoolC);

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
				pline(Hallucination ?
						"Uh... everything is so... green!?" :
						"You see trees growing out of the ground!" );

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
			do_clear_areaX(u.ux, u.uy, radiusE, do_icefloodd, (genericptr_t)&madepoolD);

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
				pline(Hallucination ?
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
			do_clear_areaX(u.ux, u.uy, radiusF, do_cloudfloodd, (genericptr_t)&madepoolE);

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
				pline(Hallucination ?
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
			do_clear_areaX(u.ux, u.uy, radiusG, do_barfloodd, (genericptr_t)&madepoolF);

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
				pline(Hallucination ?
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
			do_clear_areaX(u.ux, u.uy, radiusR, do_terrainfloodd, (genericptr_t)&madepoolR);

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
				pline(Hallucination ?
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

		mzapmsg(mtmp, otmp, FALSE);
		if (rn2(2) || !ishaxor) otmp->spe--;

newboss:
		do {
			pm = rndmonst();
			attempts++;

		} while ( (!pm || (pm && !(pm->msound == MS_FART_LOUD || pm->msound == MS_FART_NORMAL || pm->msound == MS_FART_QUIET ))) && attempts < 50000);

		if (!pm && rn2(50) ) {
			attempts = 0;
			goto newboss;
		}
		if (pm && !(pm->msound == MS_FART_LOUD || pm->msound == MS_FART_NORMAL || pm->msound == MS_FART_QUIET) && rn2(50) ) {
			attempts = 0;
			goto newboss;
		}

		mon = makemon(pm, cc.x, cc.y, NO_MM_FLAGS);
	      if (mon && canspotmon(mon)) known = TRUE;

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);

		return 2;
	    }


	case MUSE_WAN_TRAP_CREATION:

		mzapmsg(mtmp, otmp, FALSE);
		if (rn2(2) || !ishaxor) otmp->spe--;
		makeknown(otmp->otyp);
	      You_feel("endangered!!");
		{
			int rtrap;
		    int i, j, bd;
			bd = 1;
			if (!rn2(5)) bd += rnz(1);

		      for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
				if (!isok(u.ux + i, u.uy + j)) continue;
				if ((levl[u.ux + i][u.uy + j].typ <= DBWALL) || MON_AT(u.ux + i, u.uy + j)) continue;
				if (t_at(u.ux + i, u.uy + j)) continue;

			      rtrap = randomtrap();

				(void) maketrap(u.ux + i, u.uy + j, rtrap);
			}
		}

		makerandomtrap();
		if (!rn2(2)) makerandomtrap();
		if (!rn2(4)) makerandomtrap();
		if (!rn2(8)) makerandomtrap();
		if (!rn2(16)) makerandomtrap();
		if (!rn2(32)) makerandomtrap();
		if (!rn2(64)) makerandomtrap();
		if (!rn2(128)) makerandomtrap();
		if (!rn2(256)) makerandomtrap();

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);

		return 2;

	case MUSE_WAN_BAD_EFFECT:

		mzapmsg(mtmp, otmp, FALSE);
		if (rn2(2) || !ishaxor) otmp->spe--;

		badeffect();

		if (oseen) makeknown(WAN_BAD_EFFECT);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_CURSE_ITEMS:

		mzapmsg(mtmp, otmp, FALSE);
		if (rn2(2) || !ishaxor) otmp->spe--;

		pline("A black glow surrounds you...");
		rndcurse();

		if (oseen) makeknown(WAN_CURSE_ITEMS);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_AMNESIA:

		mzapmsg(mtmp, otmp, FALSE);
		if (rn2(2) || !ishaxor) otmp->spe--;

		You_feel("dizzy!");
		forget(1 + rn2(5));

		if (oseen) makeknown(WAN_AMNESIA);

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
		if (rn2(2) || !ishaxor) otmp->spe--;

		pline("You feel very unlucky.");
		change_luck(-1);

		if (oseen) makeknown(WAN_BAD_LUCK);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_REMOVE_RESISTANCE:

		mzapmsg(mtmp, otmp, FALSE);
		if (rn2(2) || !ishaxor) otmp->spe--;

		attrcurse();
		while (rn2(3)) {
			attrcurse();
		}

		if (oseen) makeknown(WAN_REMOVE_RESISTANCE);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_CORROSION:

		mzapmsg(mtmp, otmp, FALSE);
		if (rn2(2) || !ishaxor) otmp->spe--;

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
		if (rn2(2) || !ishaxor) otmp->spe--;

		pline("You start trembling...");
		HFumbling = FROMOUTSIDE | rnd(100);

		if (oseen) makeknown(WAN_FUMBLING);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_STARVATION:

		mzapmsg(mtmp, otmp, FALSE);
		if (rn2(2) || !ishaxor) otmp->spe--;

		pline("You feel a hole in your %s!", body_part(STOMACH) );
		morehungry(rnd(1000));

		if (oseen) makeknown(WAN_STARVATION);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_CONFUSION:

		mzapmsg(mtmp, otmp, FALSE);
		if (rn2(2) || !ishaxor) otmp->spe--;

		if(!Confusion) {
		    if (Hallucination) {
			pline("What a trippy feeling!");
		    } else if (Role_if(PM_PIRATE) || Role_if(PM_KORSAIR))
			pline("Blimey! Ye're one sheet to the wind!");
			else 
			pline("Huh, What?  Where am I?");
		}
		make_confused(HConfusion + rn1(35, 115), FALSE);

		if (oseen) makeknown(WAN_CONFUSION);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_SLIMING:

		mzapmsg(mtmp, otmp, FALSE);
		if (rn2(2) || !ishaxor) otmp->spe--;

		if (!Slimed && !flaming(youmonst.data) && !Unchanging && !slime_on_touch(youmonst.data) ) {
		    You("don't feel very well.");
		    Slimed = 100L;
		    flags.botl = 1;
		}

		if (oseen) makeknown(WAN_SLIMING);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_LYCANTHROPY:

		mzapmsg(mtmp, otmp, FALSE);
		if (rn2(2) || !ishaxor) otmp->spe--;

		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREWOLF;
			pline("You feel feverish.");
		}

		if (oseen) makeknown(WAN_LYCANTHROPY);

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_PUNISHMENT:

		mzapmsg(mtmp, otmp, FALSE);
		if (rn2(2) || !ishaxor) otmp->spe--;

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
		    if (mtmp->minvis)
			map_invisible(mtmp->mx, mtmp->my);
		    if (oseen) makeknown(otmp->otyp);
		}

	    	/* Loop through the surrounding squares */
	    	for (x = mmx-1; x <= mmx+1; x++) {
	    	    for (y = mmy-1; y <= mmy+1; y++) {
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
	    	    	    otmp2 = mksobj(confused ? ROCK : BOULDER,
	    	    	    		FALSE, FALSE);
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
				    if ((mtmp2->minvis && !canspotmon(mtmp2)) || mtmp2->minvisreal)
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
		    otmp2 = mksobj(confused ? ROCK : BOULDER,
				FALSE, FALSE);
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
			    if(is_metallic(uarmh)) {
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
			pline("%s is uninjured.", Monnam(mtmp));
			makeknown(otmp->otyp);
			if (!rn2(33)) (void) destroy_mitem(mtmp, SCROLL_CLASS, AD_FIRE);
			if (!rn2(33)) (void) destroy_mitem(mtmp, SPBOOK_CLASS, AD_FIRE);
			if (!rn2(33)) (void) destroy_mitem(mtmp, POTION_CLASS, AD_FIRE);
			num = (2*(rn1(3, 3) + 2 * bcsign(otmp)) + 1)/3;
			if (Slimed) {
			      Your("slimy parts are burned away!");
			      Slimed = 0;
			}
			if (Fire_resistance)
			    You("are not harmed.");
			burn_away_slime();
			if (Half_spell_damage && rn2(2) ) num = (num+1) / 2;
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
		if (cansee(mtmp->mx, mtmp->my)) {
			otmp->dknown = 1;
			pline("%s hurls %s!", Monnam(mtmp),
						singular(otmp, doname));
		}
		else pline("You hear a hurling sound.");

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
# ifdef KOPS
			|| pm->mlet == S_KOP
# endif
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
struct monst *mtmp;
{
	struct permonst *pm = mtmp->data;

	if((is_animal(pm) || attacktype(pm, AT_EXPL) || mindless(mtmp->data)
			|| pm->mlet == S_GHOST
# ifdef KOPS
			|| pm->mlet == S_KOP
# endif
		) && issoviet) return 0;
	switch (rn2(75)) {

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
		case 33: return WAN_BANISHMENT;
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

boolean
find_misc(mtmp)
struct monst *mtmp;
{
	register struct obj *obj;
	struct permonst *mdat = mtmp->data;
	int x = mtmp->mx, y = mtmp->my;
	struct trap *t;
	int xx, yy;
#ifdef STEED
	boolean immobile = (mdat->mmove == 0 || mtmp == u.usteed);
#else
	boolean immobile = (mdat->mmove == 0);
#endif
	boolean stuck = (mtmp == u.ustuck);

	m.misc = (struct obj *)0;
	m.has_misc = 0;
	if ((is_animal(mdat) || mindless(mdat)) && issoviet)
		return 0;
	if (u.uswallow && stuck) return FALSE;

	/* We arbitrarily limit to times when a player is nearby for the
	 * same reason as Junior Pac-Man doesn't have energizers eaten until
	 * you can see them...
	 */
	if(dist2(x, y, mtmp->mux, mtmp->muy) > 36)
		return FALSE;

	if (!stuck && !immobile && !mtmp->cham && monstr[monsndx(mdat)] < 6) {
	  boolean ignore_boulders = (verysmall(mdat) ||
				     throws_rocks(mdat) || (mtmp->egotype_wallwalk) ||
				     passes_walls(mdat));
	  for(xx = x-1; xx <= x+1; xx++)
	    for(yy = y-1; yy <= y+1; yy++)
		if (isok(xx,yy) && (xx != u.ux || yy != u.uy))
		    if ((mdat != &mons[PM_GRID_BUG] && mdat != &mons[PM_WEREGRIDBUG] && mdat != &mons[PM_GRID_XORN] && mdat != &mons[PM_STONE_BUG] && mdat != &mons[PM_WEAPON_BUG] && mdat != &mons[PM_NATURAL_BUG] && mdat != &mons[PM_MELEE_BUG]) || xx == x || yy == y)
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
	/*if (nohands(mdat))
		return 0;*/

#define nomore(x) if(m.has_misc==x) continue;
	for(obj=mtmp->minvent; obj; obj=obj->nobj) {
		/* Monsters shouldn't recognize cursed items; this kludge is */
		/* necessary to prevent serious problems though... */
		if(obj->otyp == POT_GAIN_LEVEL && (!obj->cursed ||
			    (!mtmp->isgd && !mtmp->isshk && !mtmp->ispriest))) {
			m.misc = obj;
			m.has_misc = MUSE_POT_GAIN_LEVEL;
		}
		nomore(MUSE_BULLWHIP);
/*  WAC kludge here so monsters don't attempt to grab cursed weapon */
		if(obj->otyp == BULLWHIP && (MON_WEP(mtmp) == obj) &&
		   distu(mtmp->mx,mtmp->my)==1 && uwep && !uwep->cursed &&
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
		    (!mtmp->mpeaceful || See_invisible) &&
		    (!attacktype(mtmp->data, AT_GAZE) || mtmp->mcan)) {
			m.misc = obj;
			m.has_misc = MUSE_WAN_MAKE_INVISIBLE;
		}
		nomore(MUSE_POT_INVISIBILITY);
		if(obj->otyp == POT_INVISIBILITY &&
		    !mtmp->minvis && !mtmp->invis_blkd &&
		    (!mtmp->mpeaceful || See_invisible) &&
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
		if(obj->otyp == WAN_CLONE_MONSTER && obj->spe > 0) {
			m.misc = obj;
			m.has_misc = MUSE_WAN_CLONE_MONSTER;
		}
		nomore(MUSE_POT_POLYMORPH);	/* will also be used if high-level monster is low on health --Amy */
		if(obj->otyp == POT_POLYMORPH && !mtmp->cham
				&& ((monstr[monsndx(mdat)] < 6) || (mtmp->mhp*3 < mtmp->mhpmax) ) ) {
			m.misc = obj;
			m.has_misc = MUSE_POT_POLYMORPH;
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
		nomore(MUSE_SCR_CREATE_MONSTER_M);
		if(obj->otyp == SCR_CREATE_MONSTER && !rn2(25) && !mtmp->mpeaceful ) {
			m.misc = obj;
			m.has_misc = MUSE_SCR_CREATE_MONSTER_M;
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
		    if (rn2(2) || !ishaxor) otmp->spe--;
		} else
		    mquaffmsg(mtmp, otmp);
		/* format monster's name before altering its visibility */
		Strcpy(nambuf, See_invisible ? Monnam(mtmp) : mon_nam(mtmp));
		mon_set_minvis(mtmp);
		if (vismon && mtmp->minvis) {	/* was seen, now invisible */
		    if (See_invisible)
			pline("%s body takes on a %s transparency.",
			      s_suffix(nambuf),
			      Hallucination ? "normal" : "strange");
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
		if (rn2(2) || !ishaxor) otmp->spe--;
		mon_adjust_speed(mtmp, 1, otmp);
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_GAIN_LEVEL:

		mzapmsg(mtmp, otmp, TRUE);
		if (rn2(2) || !ishaxor) otmp->spe--;

		if (vismon) pline("%s seems more experienced.", Monnam(mtmp));
		if (oseen) makeknown(WAN_GAIN_LEVEL);
		if (!grow_up(mtmp,(struct monst *)0)) return 1;
			/* grew into genocided monster */

		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_INCREASE_MAX_HITPOINTS:

		mzapmsg(mtmp, otmp, TRUE);
		if (rn2(2) || !ishaxor) otmp->spe--;

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
		if (rn2(2) || !ishaxor) otmp->spe--;
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
		if (rn2(2) || !ishaxor) otmp->spe--;
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
	case MUSE_POT_MUTATION:
		mquaffmsg(mtmp, otmp);

		mtmp->isegotype = 1;
		switch (rnd(124)) {
			case 1:
			case 2:
			case 3: mtmp->egotype_thief = 1; break;
			case 4: mtmp->egotype_wallwalk = 1; break;
			case 5: mtmp->egotype_disenchant = 1; break;
			case 6:
			case 7: mtmp->egotype_rust = 1; break;
			case 8: 
			case 9: mtmp->egotype_corrosion = 1; break;
			case 10: 
			case 11: mtmp->egotype_decay = 1; break;
			case 12: mtmp->egotype_wither = 1; break;
			case 13: 
			case 14: 
			case 15: mtmp->egotype_grab = 1; break;
			case 16: 
			case 17: mtmp->egotype_flying = 1; break;
			case 18: 
			case 19: mtmp->egotype_hide = 1; break;
			case 20: 
			case 21: 
			case 22: mtmp->egotype_regeneration = 1; break;
			case 23: 
			case 24: 
			case 25: mtmp->egotype_undead = 1; break;
			case 26: mtmp->egotype_domestic = 1; break;
			case 27: mtmp->egotype_covetous = 1; break;
			case 28: 
			case 29: mtmp->egotype_avoider = 1; break;
			case 30: mtmp->egotype_petty = 1; break;
			case 31: mtmp->egotype_pokemon = 1; break;
			case 32: mtmp->egotype_slows = 1; break;
			case 33: mtmp->egotype_vampire = 1; break;
			case 34: mtmp->egotype_teleportself = 1; break;
			case 35: mtmp->egotype_teleportyou = 1; break;
			case 36: 
			case 37: mtmp->egotype_wrap = 1; break;
			case 38: mtmp->egotype_disease = 1; break;
			case 39: mtmp->egotype_slime = 1; break;
			case 40: 
			case 41: 
			case 42: 
			case 43: mtmp->egotype_engrave = 1; break;
			case 44: 
			case 45: mtmp->egotype_dark = 1; break;
			case 46: mtmp->egotype_luck = 1; break;
			case 47: 
			case 48: 
			case 49: mtmp->egotype_push = 1; break;
			case 50: mtmp->egotype_arcane = 1; break;
			case 51: mtmp->egotype_clerical = 1; break;
			case 52: 
			case 53: mtmp->egotype_armorer = 1; break;
			case 54: mtmp->egotype_tank = 1; break;
			case 55: 
			case 56: mtmp->egotype_speedster = 1; break;
			case 57: mtmp->egotype_racer = 1; break;
			case 58: mtmp->egotype_randomizer = 1; break;
			case 59: mtmp->egotype_blaster = 1; break;
			case 60: mtmp->egotype_multiplicator = 1; break;
			case 61: mtmp->egotype_gator = 1; break;
			case 62: mtmp->egotype_reflecting = 1; break;
			case 63: mtmp->egotype_hugger = 1; break;
			case 64: mtmp->egotype_mimic = 1; set_mimic_sym(mtmp); break;
			case 65: mtmp->egotype_permamimic = 1; set_mimic_sym(mtmp); break;
			case 66:
			case 67: mtmp->egotype_poisoner = 1; break;
			case 68: mtmp->egotype_elementalist = 1; break;
			case 69: mtmp->egotype_resistor = 1; break;
			case 70:
			case 71: mtmp->egotype_acidspiller = 1; break;
			case 72:
			case 73: mtmp->egotype_watcher = 1; break;
			case 74: mtmp->egotype_metallivore = 1; break;
			case 75: mtmp->egotype_lithivore = 1; break;
			case 76: mtmp->egotype_organivore = 1; break;
			case 77: mtmp->egotype_breather = 1; break;
			case 78: mtmp->egotype_beamer = 1; break;
			case 79:
			case 80: mtmp->egotype_troll = 1; break;
			case 81:
			case 82:
			case 83:
			case 84:
			case 85:
			case 86: mtmp->egotype_faker = 1; break;
			case 87:
			case 88:
			case 89:
			case 90: mtmp->egotype_farter = 1; break;
			case 91: mtmp->egotype_timer = 1; break;
			case 92: mtmp->egotype_thirster = 1; break;
			case 93: mtmp->egotype_watersplasher = 1; break;
			case 94: mtmp->egotype_cancellator = 1; break;
			case 95: mtmp->egotype_banisher = 1; break;
			case 96: mtmp->egotype_shredder = 1; break;
			case 97: mtmp->egotype_abductor = 1; break;
			case 98:
			case 99: mtmp->egotype_incrementor = 1; break;
			case 100: mtmp->egotype_mirrorimage = 1; break;
			case 101:
			case 102: mtmp->egotype_curser = 1; break;
			case 103: mtmp->egotype_horner = 1; break;
			case 104: mtmp->egotype_lasher = 1; break;
			case 105: mtmp->egotype_cullen = 1; break;
			case 106:
			case 107:
			case 108: mtmp->egotype_webber = 1; break;
			case 109: mtmp->egotype_itemporter = 1; break;
			case 110: mtmp->egotype_schizo = 1; break;
			case 111: mtmp->egotype_nexus = 1; break;
			case 112: mtmp->egotype_sounder = 1; break;
			case 113: mtmp->egotype_gravitator = 1; break;
			case 114: mtmp->egotype_inert = 1; break;
			case 115:
			case 116: mtmp->egotype_antimage = 1; break;
			case 117: mtmp->egotype_plasmon = 1; break;
			case 118:
			case 119:
			case 120: mtmp->egotype_weaponizer = 1; break;
			case 121: mtmp->egotype_engulfer = 1; break;
			case 122: mtmp->egotype_bomber = 1; break;
			case 123:
			case 124: mtmp->egotype_exploder = 1; break;
		}

		if (oseen) makeknown(POT_MUTATION);
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;

	case MUSE_WAN_MUTATION:
		mzapmsg(mtmp, otmp, TRUE);
		if (rn2(2) || !ishaxor) otmp->spe--;

		mtmp->isegotype = 1;
		switch (rnd(124)) {
			case 1:
			case 2:
			case 3: mtmp->egotype_thief = 1; break;
			case 4: mtmp->egotype_wallwalk = 1; break;
			case 5: mtmp->egotype_disenchant = 1; break;
			case 6:
			case 7: mtmp->egotype_rust = 1; break;
			case 8: 
			case 9: mtmp->egotype_corrosion = 1; break;
			case 10: 
			case 11: mtmp->egotype_decay = 1; break;
			case 12: mtmp->egotype_wither = 1; break;
			case 13: 
			case 14: 
			case 15: mtmp->egotype_grab = 1; break;
			case 16: 
			case 17: mtmp->egotype_flying = 1; break;
			case 18: 
			case 19: mtmp->egotype_hide = 1; break;
			case 20: 
			case 21: 
			case 22: mtmp->egotype_regeneration = 1; break;
			case 23: 
			case 24: 
			case 25: mtmp->egotype_undead = 1; break;
			case 26: mtmp->egotype_domestic = 1; break;
			case 27: mtmp->egotype_covetous = 1; break;
			case 28: 
			case 29: mtmp->egotype_avoider = 1; break;
			case 30: mtmp->egotype_petty = 1; break;
			case 31: mtmp->egotype_pokemon = 1; break;
			case 32: mtmp->egotype_slows = 1; break;
			case 33: mtmp->egotype_vampire = 1; break;
			case 34: mtmp->egotype_teleportself = 1; break;
			case 35: mtmp->egotype_teleportyou = 1; break;
			case 36: 
			case 37: mtmp->egotype_wrap = 1; break;
			case 38: mtmp->egotype_disease = 1; break;
			case 39: mtmp->egotype_slime = 1; break;
			case 40: 
			case 41: 
			case 42: 
			case 43: mtmp->egotype_engrave = 1; break;
			case 44: 
			case 45: mtmp->egotype_dark = 1; break;
			case 46: mtmp->egotype_luck = 1; break;
			case 47: 
			case 48: 
			case 49: mtmp->egotype_push = 1; break;
			case 50: mtmp->egotype_arcane = 1; break;
			case 51: mtmp->egotype_clerical = 1; break;
			case 52: 
			case 53: mtmp->egotype_armorer = 1; break;
			case 54: mtmp->egotype_tank = 1; break;
			case 55: 
			case 56: mtmp->egotype_speedster = 1; break;
			case 57: mtmp->egotype_racer = 1; break;
			case 58: mtmp->egotype_randomizer = 1; break;
			case 59: mtmp->egotype_blaster = 1; break;
			case 60: mtmp->egotype_multiplicator = 1; break;
			case 61: mtmp->egotype_gator = 1; break;
			case 62: mtmp->egotype_reflecting = 1; break;
			case 63: mtmp->egotype_hugger = 1; break;
			case 64: mtmp->egotype_mimic = 1; set_mimic_sym(mtmp); break;
			case 65: mtmp->egotype_permamimic = 1; set_mimic_sym(mtmp); break;
			case 66:
			case 67: mtmp->egotype_poisoner = 1; break;
			case 68: mtmp->egotype_elementalist = 1; break;
			case 69: mtmp->egotype_resistor = 1; break;
			case 70:
			case 71: mtmp->egotype_acidspiller = 1; break;
			case 72:
			case 73: mtmp->egotype_watcher = 1; break;
			case 74: mtmp->egotype_metallivore = 1; break;
			case 75: mtmp->egotype_lithivore = 1; break;
			case 76: mtmp->egotype_organivore = 1; break;
			case 77: mtmp->egotype_breather = 1; break;
			case 78: mtmp->egotype_beamer = 1; break;
			case 79:
			case 80: mtmp->egotype_troll = 1; break;
			case 81:
			case 82:
			case 83:
			case 84:
			case 85:
			case 86: mtmp->egotype_faker = 1; break;
			case 87:
			case 88:
			case 89:
			case 90: mtmp->egotype_farter = 1; break;
			case 91: mtmp->egotype_timer = 1; break;
			case 92: mtmp->egotype_thirster = 1; break;
			case 93: mtmp->egotype_watersplasher = 1; break;
			case 94: mtmp->egotype_cancellator = 1; break;
			case 95: mtmp->egotype_banisher = 1; break;
			case 96: mtmp->egotype_shredder = 1; break;
			case 97: mtmp->egotype_abductor = 1; break;
			case 98:
			case 99: mtmp->egotype_incrementor = 1; break;
			case 100: mtmp->egotype_mirrorimage = 1; break;
			case 101:
			case 102: mtmp->egotype_curser = 1; break;
			case 103: mtmp->egotype_horner = 1; break;
			case 104: mtmp->egotype_lasher = 1; break;
			case 105: mtmp->egotype_cullen = 1; break;
			case 106:
			case 107:
			case 108: mtmp->egotype_webber = 1; break;
			case 109: mtmp->egotype_itemporter = 1; break;
			case 110: mtmp->egotype_schizo = 1; break;
			case 111: mtmp->egotype_nexus = 1; break;
			case 112: mtmp->egotype_sounder = 1; break;
			case 113: mtmp->egotype_gravitator = 1; break;
			case 114: mtmp->egotype_inert = 1; break;
			case 115:
			case 116: mtmp->egotype_antimage = 1; break;
			case 117: mtmp->egotype_plasmon = 1; break;
			case 118:
			case 119:
			case 120: mtmp->egotype_weaponizer = 1; break;
			case 121: mtmp->egotype_engulfer = 1; break;
			case 122: mtmp->egotype_bomber = 1; break;
			case 123:
			case 124: mtmp->egotype_exploder = 1; break;
		}

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

		mreadmsg(mtmp, otmp);

newboss:
		do {
			pm = rndmonst();
			attempts++;

		} while ( (!pm || (pm && !(pm->geno & G_UNIQ))) && attempts < 50000);

		if (!pm && rn2(50) ) {
			attempts = 0;
			goto newboss;
		}
		if (pm && !(pm->geno & G_UNIQ) && rn2(50) ) {
			attempts = 0;
			goto newboss;
		}

		mon = makemon(pm, cc.x, cc.y, NO_MM_FLAGS);
	      if (mon && canspotmon(mon)) known = TRUE;

		if (known)
		    makeknown(SCR_SUMMON_BOSS);
		else if (!objects[SCR_SUMMON_BOSS].oc_name_known
			&& !objects[SCR_SUMMON_BOSS].oc_uname)
		    docall(otmp);
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;
	    }

	case MUSE_SCR_CREATE_MONSTER_M:
	    {	coord cc;
		struct permonst *pm = 0, *fish = 0;
		int cnt = 1;
		struct monst *mon;
		boolean known = FALSE;

		if (!rn2(73)) cnt += rnd(4);
		if (mtmp->mconf || otmp->cursed) cnt += 12;
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
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;
	    }

	case MUSE_BAG_OF_TRICKS_M:

		/* jonadab wants monsters to not use up charges if they apply this thing, but that would be too evil. --Amy */
	    {	coord cc;

		struct permonst *pm = 0;
		struct monst *mon;

		if (!enexto(&cc, mtmp->mx, mtmp->my, pm)) return 0;
		mzapmsg(mtmp, otmp, FALSE);
		if (rn2(2) || !ishaxor) otmp->spe--;
		mon = makemon((struct permonst *)0, cc.x, cc.y, NO_MM_FLAGS);
		if (mon && canspotmon(mon) && oseen)
		    makeknown(BAG_OF_TRICKS);
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;
	    }

	case MUSE_WAN_SUMMON_UNDEAD_M:
	    {	coord cc;

		struct permonst *pm = 0;
		struct monst *mon;

		if (!enexto(&cc, mtmp->mx, mtmp->my, pm)) return 0;
		mzapmsg(mtmp, otmp, FALSE);
		if (rn2(2) || !ishaxor) otmp->spe--;

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
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;
	    }

	case MUSE_SCR_SUMMON_UNDEAD_M:

	    {	coord cc;
		struct permonst *pm = 0;
		int cnt = 1;
		struct monst *mon;
		boolean known = FALSE;

		if (rn2(2)) cnt += rnz(2);
		if (!rn2(73)) cnt += rnd(4);
		if (mtmp->mconf || otmp->cursed) cnt += 12;
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
		if (rn2(2) || !ishaxor) m_useup(mtmp, otmp);
		return 2;
	    }

	case MUSE_WAN_CREATE_MONSTER_M:
	    {	coord cc;

		struct permonst *pm = 0;
		struct monst *mon;

		if (!enexto(&cc, mtmp->mx, mtmp->my, pm)) return 0;
		mzapmsg(mtmp, otmp, FALSE);
		if (rn2(2) || !ishaxor) otmp->spe--;
		mon = makemon((struct permonst *)0, cc.x, cc.y, NO_MM_FLAGS);
		if (mon && canspotmon(mon) && oseen)
		    makeknown(WAN_CREATE_MONSTER);
		if (otmp->spe == 0 && rn2(4) ) m_useup(mtmp, otmp);
		return 2;
	    }

	case MUSE_WAN_CREATE_HORDE_M:
	    {   coord cc;
		struct permonst *pm=rndmonst();
		int cnt = 1;
		if (!enexto(&cc, mtmp->mx, mtmp->my, pm)) return 0;
		mzapmsg(mtmp, otmp, FALSE);
		if (rn2(2) || !ishaxor) otmp->spe--;
		if (oseen) makeknown(WAN_CREATE_HORDE);
		cnt = rnd(4) + 10;
		while(cnt--) {
			struct monst *mon;
			if (!enexto(&cc, mtmp->mx, mtmp->my, pm)) continue;
			mon = makemon(rndmonst(), cc.x, cc.y, NO_MM_FLAGS);
			if (mon) newsym(mon->mx,mon->my);
		}
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
	case MUSE_BULLWHIP:
		/* attempt to disarm hero */
		if (uwep && !rn2(5)) {
		    const char *The_whip = vismon ? "The bullwhip" : "A whip";
		    int where_to = rn2(4);
		    struct obj *obj = uwep;
		    const char *hand;
		    char the_weapon[BUFSZ];

		    Strcpy(the_weapon, the(xname(obj)));
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
		    } else if (where_to == 3 && hates_silver(mtmp->data) &&
			    objects[obj->otyp].oc_material == SILVER) {
			/* this monster won't want to catch a silver
			   weapon; drop it at hero's feet instead */
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
	if (!canspotmon(mtmp))
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
# ifdef KOPS
			|| pm->mlet == S_KOP
# endif
		) && issoviet) return 0;
	/* Unlike other rnd_item functions, we only allow _weak_ monsters
	 * to have this item; after all, the item will be used to strengthen
	 * the monster and strong monsters won't use it at all...
	 */
	if (difficulty < 6 && !rn2(30))
	    return rn2(6) ? POT_POLYMORPH : WAN_POLYMORPH;

	if (!rn2(40)/* && !nonliving(pm)*/) return AMULET_OF_LIFE_SAVING;

	switch (rn2(3)) {
		case 0:
			if (mtmp->isgd) return 0;
			return rn2(6) ? POT_SPEED : WAN_SPEED_MONSTER;
		case 1:
			if (mtmp->mpeaceful && !See_invisible) return 0;
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
# ifdef KOPS
			|| pm->mlet == S_KOP
# endif
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
		    typ == WAN_BANISHMENT ||
		    typ == WAN_TELEPORTATION ||
		    typ == WAN_CREATE_MONSTER ||
		    typ == WAN_SUMMON_UNDEAD ||
		    typ == WAN_TRAP_CREATION ||
		    typ == WAN_SUMMON_SEXY_GIRL ||
		    typ == WAN_CREATE_HORDE ||
		    typ == WAN_DRAINING	||
		    typ == WAN_REDUCE_MAX_HITPOINTS	||
		    typ == WAN_INCREASE_MAX_HITPOINTS	||
		    typ == WAN_SLOW_MONSTER	||
		    typ == WAN_FEAR	||
		    typ == WAN_HEALING ||
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
		    typ == WAN_MUTATION ||
		    typ == WAN_BAD_LUCK ||
		    typ == WAN_REMOVE_RESISTANCE ||
		    typ == WAN_CORROSION ||
		    typ == WAN_FUMBLING ||
		    typ == WAN_STARVATION ||
		    typ == WAN_CONFUSION ||
		    typ == WAN_SLIMING ||
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
	    if (typ == POT_HEALING ||
		    typ == POT_EXTRA_HEALING ||
		    typ == POT_FULL_HEALING ||
		    typ == POT_POLYMORPH ||
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
		    typ == POT_AMNESIA)
		return TRUE;
	    if (typ == POT_BLINDNESS && !attacktype(mon->data, AT_GAZE))
		return TRUE;
	    break;
	case SCROLL_CLASS:
	    if (typ == SCR_TELEPORTATION || typ == SCR_HEALING || typ == SCR_TELE_LEVEL || typ == SCR_WARPING || typ == SCR_ROOT_PASSWORD_DETECTION || typ == SCR_CREATE_MONSTER || typ == SCR_SUMMON_UNDEAD || typ == SCR_FLOOD || typ == SCR_BULLSHIT || typ == SCR_DESTROY_ARMOR || typ == SCR_LAVA || typ == SCR_SUMMON_BOSS || typ == SCR_STONING || typ == SCR_AMNESIA || typ == SCR_LOCKOUT || typ == SCR_GROWTH || typ == SCR_ICE || typ == SCR_BAD_EFFECT || typ == SCR_CLOUDS || typ == SCR_BARRHING || typ == SCR_CHAOS_TERRAIN || typ == SCR_PUNISHMENT || typ == SCR_EARTH || typ == SCR_TRAP_CREATION || typ == SCR_FIRE || typ == SCR_WOUNDS || typ == SCR_DEMONOLOGY || typ == SCR_ELEMENTALISM || typ == SCR_GIRLINESS || typ == SCR_NASTINESS )
		return TRUE;
	    break;
	case AMULET_CLASS:
	    if (typ == AMULET_OF_LIFE_SAVING || typ == AMULET_OF_DATA_STORAGE || typ == AMULET_OF_REFLECTION)
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
	    if (typ == UNICORN_HORN)
		return (boolean)(!obj->cursed && !is_unicorn(mon->data));
	    if (typ == FROST_HORN || typ == FIRE_HORN)
		return (obj->spe > 0);
	    if (is_weptool(obj))
	    	return (boolean)likes_objs(mon->data);
	    if (typ == BAG_OF_TRICKS)
		return TRUE;
	    break;
	case FOOD_CLASS:
	    if (typ == CORPSE)
		return (boolean)(((mon->misc_worn_check & W_ARMG) &&
				    touch_petrifies(&mons[obj->corpsenm])) ||
				(!resists_ston(mon) &&
				    (obj->corpsenm == PM_LIZARD ||
					(acidic(&mons[obj->corpsenm]) && !slime_on_touch(&mons[obj->corpsenm])) )));
	    if (typ == EGG)
		return (boolean)(touch_petrifies(&mons[obj->corpsenm]));
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

	if (orefl && orefl->otyp == SHIELD_OF_REFLECTION) {
	    if (str) {
		pline(str, s_suffix(mon_nam(mon)), "shield");
		makeknown(SHIELD_OF_REFLECTION);
	    }
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
	} else if (mon->data == &mons[PM_NIGHTMARE]) {
	    pline(str,s_suffix(mon_nam(mon)),"horn");
	    return TRUE;
	} else if ((orefl = which_armor(mon, W_ARM)) &&
		(orefl->otyp == SILVER_DRAGON_SCALES || orefl->otyp == SILVER_DRAGON_SCALE_MAIL)) {
	    if (str)
		pline(str, s_suffix(mon_nam(mon)), "armor");
	    return TRUE;
	} else if (mon->data == &mons[PM_SILVER_DRAGON] || mon->data == &mons[PM_OLD_SILVER_DRAGON] || mon->data == &mons[PM_VERY_OLD_SILVER_DRAGON] || mon->data == &mons[PM_AUREAL] || mon->data == &mons[PM_ANCIENT_SILVER_DRAGON] ||
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
	} else if (mon->data == &mons[PM_SHAMBLING_MOUND] || mon->data == &mons[PM_GREAT_WYRM_OF_POWER] || mon->data == &mons[PM_SILVER_WOLF] || mon->data == &mons[PM_MIRROR_GOLEM] || mon->data == &mons[PM_MIRROR_MONSTER] || mon->data == &mons[PM_MIRROR_VORTEX] || mon->data == &mons[PM_VENOM_ATRONACH] || mon->data == &mons[PM_DIVISION_THIEF] || mon->data == &mons[PM_SILVER_OGRE] || mon->data == &mons[PM_DIVISION_JEDI] || mon->data == &mons[PM_ECM_NERVE_HEAD] || mon->data == &mons[PM_ECM_ARCHER] || mon->data == &mons[PM_ILLUSION_WEAVER] || mon->data == &mons[PM_SEDUCER_SAINT] || mon->data == &mons[PM_MIRROR_MOLD] || mon->data == &mons[PM_MIRROR_GROWTH] || mon->data == &mons[PM_MIRROR_FUNGUS] || mon->data == &mons[PM_MIRROR_PATCH] || mon->data == &mons[PM_MIRROR_SPORE] || mon->data == &mons[PM_MIRROR_MUSHROOM] || mon->data == &mons[PM_MIRROR_STALK] || mon->data == &mons[PM_MIRROR_COLONY] || mon->data == &mons[PM_MIRROR_FORCE_FUNGUS] || mon->data == &mons[PM_MIRROR_FORCE_PATCH] || mon->data == &mons[PM_MIRROR_WARP_FUNGUS] || mon->data == &mons[PM_MIRROR_WARP_PATCH] || (mon->egotype_reflecting) || (mon->egotype_breather) ) {
		/* in ADOM this thing would absorb bolts instead */
	    if (str)
		pline(str, s_suffix(mon_nam(mon)), "absorbing shell");
	    return TRUE;
	} else if (mon->data == &mons[PM_DIAMOND_GOLEM]
	         || mon->data == &mons[PM_SAPPHIRE_GOLEM]
	         || mon->data == &mons[PM_CRYSTAL_GOLEM]) {
	    /* Some of the higher golems have intrinsic reflection */
	    if (str)
		pline(str, s_suffix(mon_nam(mon)), "body");
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
	    	makeknown(SHIELD_OF_REFLECTION);
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
	    	makeknown(AMULET_OF_REFLECTION);
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
	    	makeknown(CLOAK_OF_REFLECTION);
	    return TRUE;
	} else if (EReflecting & W_ARMG) {
	    if (fmt && str)
	    	pline(fmt, str, "gauntlets");
	    	makeknown(GAUNTLETS_OF_REFLECTION);
	    return TRUE;
	} else if (youmonst.data == &mons[PM_SILVER_DRAGON] || youmonst.data == &mons[PM_OLD_SILVER_DRAGON] || youmonst.data == &mons[PM_VERY_OLD_SILVER_DRAGON] || youmonst.data == &mons[PM_AUREAL] || youmonst.data == &mons[PM_ANCIENT_SILVER_DRAGON]) {
	    if (fmt && str)
	    	pline(fmt, str, "scales");
	    return TRUE;
	} else if (youmonst.data == &mons[PM_SHAMBLING_MOUND] || youmonst.data == &mons[PM_GREAT_WYRM_OF_POWER] || youmonst.data == &mons[PM_SILVER_OGRE] || youmonst.data == &mons[PM_SILVER_WOLF] || youmonst.data == &mons[PM_MIRROR_GOLEM] || youmonst.data == &mons[PM_MIRROR_MONSTER] || youmonst.data == &mons[PM_MIRROR_VORTEX] || youmonst.data == &mons[PM_VENOM_ATRONACH] || youmonst.data == &mons[PM_DIVISION_THIEF] || youmonst.data == &mons[PM_DIVISION_JEDI] || youmonst.data == &mons[PM_ECM_NERVE_HEAD] || youmonst.data == &mons[PM_ECM_ARCHER] || youmonst.data == &mons[PM_ILLUSION_WEAVER] || youmonst.data == &mons[PM_MIRROR_MOLD] || youmonst.data == &mons[PM_MIRROR_GROWTH] || youmonst.data == &mons[PM_MIRROR_FUNGUS] || youmonst.data == &mons[PM_MIRROR_PATCH] || youmonst.data == &mons[PM_MIRROR_SPORE] || youmonst.data == &mons[PM_MIRROR_MUSHROOM] || youmonst.data == &mons[PM_MIRROR_STALK] || youmonst.data == &mons[PM_MIRROR_COLONY] || youmonst.data == &mons[PM_MIRROR_FORCE_FUNGUS] || youmonst.data == &mons[PM_MIRROR_FORCE_PATCH] || youmonst.data == &mons[PM_MIRROR_WARP_FUNGUS] || youmonst.data == &mons[PM_MIRROR_WARP_PATCH] ) {
	    if (fmt && str)
	    	pline(fmt, str, "surface");
	    return TRUE;
	} else if (youmonst.data == &mons[PM_DIAMOND_GOLEM]
	         || youmonst.data == &mons[PM_SAPPHIRE_GOLEM]
	         || youmonst.data == &mons[PM_CRYSTAL_GOLEM]) {
	    if (fmt && str)
	    	pline(fmt, str, "body");
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
    m_useup(mon, obj);
    if (((obj->otyp == POT_ACID) || acidic(&mons[obj->corpsenm])) &&
		    !resists_acid(mon)) {
	mon->mhp -= rnd(15);
	pline("%s has a very bad case of stomach acid.",
	    Monnam(mon));
    }
    if (mon->mhp <= 0) {
	pline("%s dies!", Monnam(mon));
	if (by_you) xkilled(mon, 0);
	else mondead(mon);
	return;
    }
    if (stoning && canseemon(mon)) {
	if (Hallucination)
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
    mon->mlstmv = monstermoves; /* it takes a turn */
}

/*muse.c*/
